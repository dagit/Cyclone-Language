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
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t;
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError[ 19u]; struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter_c( void(*
f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_revappend( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_flatten( struct Cyc_List_List* x); extern char Cyc_List_Nth[ 8u];
extern int Cyc_List_forall( int(* pred)( void*), struct Cyc_List_List* x);
extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct _tagged_string*);
extern struct _tagged_string Cyc_String_strconcat( struct _tagged_string, struct
_tagged_string); extern struct _tagged_string Cyc_String_implode( struct Cyc_List_List*
c); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef
struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern
int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u];
extern char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict* d, void* key); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
extern struct _tagged_string Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp( struct
_tuple0*, struct _tuple0*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void*
Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_star_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_cstar_typ( void* t,
struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ(); extern void*
Cyc_Absyn_strct( struct _tagged_string* name); extern void* Cyc_Absyn_strctq(
struct _tuple0* name); extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_string s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*, struct Cyc_List_List*
es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp(
void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp( struct
Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*, void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_pointer_exn_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt(
struct _tuple0*, void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt(
struct _tagged_string* v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string*
lab, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x,
void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init); extern int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field(
struct Cyc_List_List*, struct _tagged_string*); struct Cyc_PP_Ppstate; typedef
struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out*
Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern
struct _tagged_string Cyc_Absynpp_typ2string( void*); extern struct
_tagged_string Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*); extern struct
_tagged_string Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt*); extern struct
_tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); extern struct
_tagged_string Cyc_Absynpp_prim2string( void* p); static const int Cyc_Tcenv_VarRes=
0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void* Cyc_Tcutil_typ_kind(
void* t); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify(
void*, void*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern void*
Cyc_Tcutil_snd_tqt( struct _tuple3*); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification(
struct _tagged_string s, int i); extern struct Cyc_List_List* Cyc_Toc_toc(
struct Cyc_List_List* ds); static struct Cyc_List_List* Cyc_Toc_result_decls= 0;
struct Cyc_Absyn_Abs_n_struct Cyc_Toc_abs_ns_value={ 1u, 0}; void* Cyc_Toc_abs_ns=(
void*)& Cyc_Toc_abs_ns_value; struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq=( struct Cyc_Absyn_Tqual){.q_const=
0,.q_volatile= 0,.q_restrict= 0}; char Cyc_Toc_Toc_Unimplemented[ 22u]="\000\000\000\000Toc_Unimplemented";
char Cyc_Toc_Toc_Impossible[ 19u]="\000\000\000\000Toc_Impossible"; static void*
Cyc_Toc_unimp( struct _tagged_string s){({ struct _tagged_string _temp1= s;
fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp1.last_plus_one - _temp1.curr, _temp1.curr);});
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw((
void*) Cyc_Toc_Toc_Unimplemented);} static void* Cyc_Toc_toc_impos( struct
_tagged_string s){({ struct _tagged_string _temp2= s; fprintf( Cyc_Stdio_stderr,"%.*s\n",
_temp2.last_plus_one - _temp2.curr, _temp2.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(( void*) Cyc_Toc_Toc_Impossible);}
char Cyc_Toc_Match_error[ 16u]="\000\000\000\000Match_error"; static char _temp5[
5u]="curr"; static struct _tagged_string Cyc_Toc_curr_string=( struct
_tagged_string){ _temp5, _temp5, _temp5 + 5u}; static struct _tagged_string* Cyc_Toc_curr_sp=&
Cyc_Toc_curr_string; static char _temp8[ 14u]="last_plus_one"; static struct
_tagged_string Cyc_Toc_last_plus_one_string=( struct _tagged_string){ _temp8,
_temp8, _temp8 + 14u}; static struct _tagged_string* Cyc_Toc_last_plus_one_sp=&
Cyc_Toc_last_plus_one_string; static char _temp11[ 5u]="base"; static struct
_tagged_string Cyc_Toc_base_string=( struct _tagged_string){ _temp11, _temp11,
_temp11 + 5u}; static struct _tagged_string* Cyc_Toc_base_sp=& Cyc_Toc_base_string;
static char _temp14[ 15u]="_tagged_string"; static struct _tagged_string Cyc_Toc__tagged_string_string=(
struct _tagged_string){ _temp14, _temp14, _temp14 + 15u}; static struct
_tagged_string* Cyc_Toc__tagged_string_sp=& Cyc_Toc__tagged_string_string;
static char _temp17[ 8u]="xtunion"; static struct _tagged_string Cyc_Toc_xtunion_string=(
struct _tagged_string){ _temp17, _temp17, _temp17 + 8u}; static struct
_tagged_string* Cyc_Toc_xtunion_sp=& Cyc_Toc_xtunion_string; static char _temp20[
2u]="v"; static struct _tagged_string Cyc_Toc_v_string=( struct _tagged_string){
_temp20, _temp20, _temp20 + 2u}; static struct _tagged_string* Cyc_Toc_v_sp=&
Cyc_Toc_v_string; static char _temp23[ 4u]="tag"; static struct _tagged_string
Cyc_Toc_tag_string=( struct _tagged_string){ _temp23, _temp23, _temp23 + 4u};
static struct _tagged_string* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string; static char
_temp26[ 14u]="_handler_cons"; static struct _tagged_string Cyc_Toc__handler_cons_string=(
struct _tagged_string){ _temp26, _temp26, _temp26 + 14u}; static struct
_tagged_string* Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string; static
char _temp29[ 7u]="setjmp"; static struct _tagged_string Cyc_Toc_setjmp_string=(
struct _tagged_string){ _temp29, _temp29, _temp29 + 7u}; static struct
_tagged_string* Cyc_Toc_setjmp_sp=& Cyc_Toc_setjmp_string; static char _temp32[
14u]="_push_handler"; static struct _tagged_string Cyc_Toc__push_handler_string=(
struct _tagged_string){ _temp32, _temp32, _temp32 + 14u}; static struct
_tagged_string* Cyc_Toc__push_handler_sp=& Cyc_Toc__push_handler_string; static
char _temp35[ 13u]="_pop_handler"; static struct _tagged_string Cyc_Toc__pop_handler_string=(
struct _tagged_string){ _temp35, _temp35, _temp35 + 13u}; static struct
_tagged_string* Cyc_Toc__pop_handler_sp=& Cyc_Toc__pop_handler_string; static
char _temp38[ 13u]="_push_region"; static struct _tagged_string Cyc_Toc__push_region_string=(
struct _tagged_string){ _temp38, _temp38, _temp38 + 13u}; static struct
_tagged_string* Cyc_Toc__push_region_sp=& Cyc_Toc__push_region_string; static
char _temp41[ 12u]="_pop_region"; static struct _tagged_string Cyc_Toc__pop_region_string=(
struct _tagged_string){ _temp41, _temp41, _temp41 + 12u}; static struct
_tagged_string* Cyc_Toc__pop_region_sp=& Cyc_Toc__pop_region_string; static char
_temp44[ 8u]="handler"; static struct _tagged_string Cyc_Toc_handler_string=(
struct _tagged_string){ _temp44, _temp44, _temp44 + 8u}; static struct
_tagged_string* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static char _temp47[
14u]="_npop_handler"; static struct _tagged_string Cyc_Toc__npop_handler_string=(
struct _tagged_string){ _temp47, _temp47, _temp47 + 14u}; static struct _tuple0
Cyc_Toc__npop_handler_pr=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__npop_handler_string}; static struct _tuple0* Cyc_Toc__npop_handler_qv=&
Cyc_Toc__npop_handler_pr; static char _temp50[ 12u]="_exn_thrown"; static struct
_tagged_string Cyc_Toc__exn_thrown_string=( struct _tagged_string){ _temp50,
_temp50, _temp50 + 12u}; static struct _tagged_string* Cyc_Toc__exn_thrown_sp=&
Cyc_Toc__exn_thrown_string; static char _temp53[ 16u]="Match_Exception"; static
struct _tagged_string Cyc_Toc_Match_Exception_string=( struct _tagged_string){
_temp53, _temp53, _temp53 + 16u}; static struct _tagged_string* Cyc_Toc_Match_Exception_sp=&
Cyc_Toc_Match_Exception_string; static char _temp56[ 15u]="Cyc_new_string";
static struct _tagged_string Cyc_Toc_Cyc_new_string_string=( struct
_tagged_string){ _temp56, _temp56, _temp56 + 15u}; static struct _tagged_string*
Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string; static char _temp59[
16u]="Cyc_rnew_string"; static struct _tagged_string Cyc_Toc_Cyc_rnew_string_string=(
struct _tagged_string){ _temp59, _temp59, _temp59 + 16u}; static struct
_tagged_string* Cyc_Toc_Cyc_rnew_string_sp=& Cyc_Toc_Cyc_rnew_string_string;
static char _temp62[ 14u]="_RegionHandle"; static struct _tagged_string Cyc_Toc__region_handle_string=(
struct _tagged_string){ _temp62, _temp62, _temp62 + 14u}; static struct
_tagged_string* Cyc_Toc__region_handle_sp=& Cyc_Toc__region_handle_string;
static char _temp65[ 12u]="_new_region"; static struct _tagged_string Cyc_Toc__new_region_string=(
struct _tagged_string){ _temp65, _temp65, _temp65 + 12u}; static struct
_tagged_string* Cyc_Toc__new_region_sp=& Cyc_Toc__new_region_string; struct
_tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp* e){ void* _temp66=( void*) e->r; void* _temp92; char
_temp94; void* _temp96; void* _temp98; short _temp100; void* _temp102; void*
_temp104; int _temp106; void* _temp108; void* _temp110; long long _temp112; void*
_temp114; void* _temp116; struct Cyc_Absyn_Exp* _temp118; void* _temp120; struct
Cyc_List_List* _temp122; struct Cyc_List_List* _temp124; struct Cyc_Absyn_Structdecl*
_temp126; struct Cyc_List_List* _temp128; struct Cyc_Core_Opt* _temp130; struct
_tuple0* _temp132; struct Cyc_List_List* _temp134; struct _tuple1* _temp136;
struct Cyc_List_List* _temp138; struct Cyc_Core_Opt* _temp140; _LL68: if(*(( int*)
_temp66) == Cyc_Absyn_Const_e){ _LL93: _temp92=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp66)->f1; if(( unsigned int) _temp92 > 1u?*(( int*) _temp92) == Cyc_Absyn_Char_c:
0){ _LL97: _temp96=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp92)->f1;
goto _LL95; _LL95: _temp94=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp92)->f2;
goto _LL69;} else{ goto _LL70;}} else{ goto _LL70;} _LL70: if(*(( int*) _temp66)
== Cyc_Absyn_Const_e){ _LL99: _temp98=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp66)->f1; if(( unsigned int) _temp98 > 1u?*(( int*) _temp98) == Cyc_Absyn_Short_c:
0){ _LL103: _temp102=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp98)->f1;
goto _LL101; _LL101: _temp100=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp98)->f2; goto _LL71;} else{ goto _LL72;}} else{ goto _LL72;} _LL72: if(*((
int*) _temp66) == Cyc_Absyn_Const_e){ _LL105: _temp104=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp66)->f1; if(( unsigned int) _temp104 > 1u?*(( int*) _temp104) == Cyc_Absyn_Int_c:
0){ _LL109: _temp108=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp104)->f1;
goto _LL107; _LL107: _temp106=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp104)->f2;
goto _LL73;} else{ goto _LL74;}} else{ goto _LL74;} _LL74: if(*(( int*) _temp66)
== Cyc_Absyn_Const_e){ _LL111: _temp110=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp66)->f1; if(( unsigned int) _temp110 > 1u?*(( int*) _temp110) == Cyc_Absyn_LongLong_c:
0){ _LL115: _temp114=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp110)->f1;
goto _LL113; _LL113: _temp112=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp110)->f2; goto _LL75;} else{ goto _LL76;}} else{ goto _LL76;} _LL76: if(*((
int*) _temp66) == Cyc_Absyn_Const_e){ _LL117: _temp116=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp66)->f1; if( _temp116 ==( void*) Cyc_Absyn_Null_c){ goto _LL77;} else{ goto
_LL78;}} else{ goto _LL78;} _LL78: if(*(( int*) _temp66) == Cyc_Absyn_Cast_e){
_LL121: _temp120=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp66)->f1; goto
_LL119; _LL119: _temp118=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp66)->f2; goto _LL79;} else{ goto _LL80;} _LL80: if(*(( int*) _temp66) ==
Cyc_Absyn_Tuple_e){ _LL123: _temp122=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp66)->f1; goto _LL81;} else{ goto _LL82;} _LL82: if(*(( int*) _temp66) ==
Cyc_Absyn_Array_e){ _LL125: _temp124=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp66)->f1; goto _LL83;} else{ goto _LL84;} _LL84: if(*(( int*) _temp66) ==
Cyc_Absyn_Struct_e){ _LL133: _temp132=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp66)->f1; goto _LL131; _LL131: _temp130=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp66)->f2; goto _LL129; _LL129: _temp128=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp66)->f3; goto _LL127; _LL127: _temp126=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp66)->f4; goto
_LL85;} else{ goto _LL86;} _LL86: if(*(( int*) _temp66) == Cyc_Absyn_CompoundLit_e){
_LL137: _temp136=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp66)->f1; goto _LL135; _LL135: _temp134=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp66)->f2; goto _LL87;} else{ goto _LL88;}
_LL88: if(*(( int*) _temp66) == Cyc_Absyn_UnresolvedMem_e){ _LL141: _temp140=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp66)->f1;
goto _LL139; _LL139: _temp138=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp66)->f2; goto _LL89;} else{ goto _LL90;} _LL90: goto _LL91; _LL69: return
_temp94 =='\000'; _LL71: return _temp100 == 0; _LL73: return _temp106 == 0;
_LL75: return _temp112 == 0; _LL77: return 1; _LL79: return Cyc_Toc_is_zero(
_temp118); _LL81: return(( int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct
Cyc_List_List* x)) Cyc_List_forall)( Cyc_Toc_is_zero, _temp122); _LL83: _temp128=
_temp124; goto _LL85; _LL85: _temp134= _temp128; goto _LL87; _LL87: _temp138=
_temp134; goto _LL89; _LL89: for( 0; _temp138 != 0; _temp138=({ struct Cyc_List_List*
_temp142= _temp138; if( _temp142 == 0){ _throw( Null_Exception);} _temp142->tl;})){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)({ struct Cyc_List_List* _temp143=
_temp138; if( _temp143 == 0){ _throw( Null_Exception);} _temp143->hd;}))).f2)){
return 0;}} return 1; _LL91: return 0; _LL67:;} static int Cyc_Toc_is_nullable(
void* t){ void* _temp144= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp150; struct Cyc_Absyn_Conref* _temp152; struct Cyc_Absyn_Tqual _temp154;
struct Cyc_Absyn_Conref* _temp156; void* _temp158; void* _temp160; _LL146: if((
unsigned int) _temp144 > 4u?*(( int*) _temp144) == Cyc_Absyn_PointerType: 0){
_LL151: _temp150=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp144)->f1; _LL161: _temp160=( void*) _temp150.elt_typ; goto _LL159; _LL159:
_temp158=( void*) _temp150.rgn_typ; goto _LL157; _LL157: _temp156=( struct Cyc_Absyn_Conref*)
_temp150.nullable; goto _LL155; _LL155: _temp154=( struct Cyc_Absyn_Tqual)
_temp150.tq; goto _LL153; _LL153: _temp152=( struct Cyc_Absyn_Conref*) _temp150.bounds;
goto _LL147;} else{ goto _LL148;} _LL148: goto _LL149; _LL147: { void* _temp162=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp156))->v; int _temp168; _LL164: if(( unsigned int) _temp162 > 1u?*(( int*)
_temp162) == Cyc_Absyn_Eq_constr: 0){ _LL169: _temp168=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp162)->f1; goto _LL165;} else{ goto _LL166;} _LL166: goto _LL167; _LL165:
return _temp168; _LL167: return 0; _LL163:;} _LL149:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp170=(
char*)"is_nullable"; struct _tagged_string _temp171; _temp171.curr= _temp170;
_temp171.base= _temp170; _temp171.last_plus_one= _temp170 + 12; _temp171;}));
return 0; _LL145:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct
_tuple0* x, struct _tagged_string tag){ return({ struct _tuple0* _temp172=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp172->f1=(* x).f1;
_temp172->f2=({ struct _tagged_string* _temp173=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp173[ 0]= Cyc_String_strconcat(*(*
x).f2, tag); _temp173;}); _temp172;});} static void Cyc_Toc_add_tuple_decl(
struct _tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List* fs= 0;{
int i= 1; for( 0; ts != 0; ts=({ struct Cyc_List_List* _temp174= ts; if(
_temp174 == 0){ _throw( Null_Exception);} _temp174->tl;}), i ++){ fs=({ struct
Cyc_List_List* _temp175=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp175->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp176=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp176->name=({ struct
_tagged_string* _temp178=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp178[ 0]= xprintf("f%d", i); _temp178;}); _temp176->tq=
Cyc_Toc_mt_tq; _temp176->type=( void*)(( void*)({ struct Cyc_List_List* _temp177=
ts; if( _temp177 == 0){ _throw( Null_Exception);} _temp177->hd;})); _temp176->width=
0; _temp176->attributes= 0; _temp176;}); _temp175->tl= fs; _temp175;});}} fs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);{
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp182=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp182->sc=( void*)(( void*) Cyc_Absyn_Public); _temp182->name=({ struct Cyc_Core_Opt*
_temp184=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp184->v=( void*)({ struct _tuple0* _temp185=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp185->f1= Cyc_Toc_abs_ns; _temp185->f2= x;
_temp185;}); _temp184;}); _temp182->tvs= 0; _temp182->fields=({ struct Cyc_Core_Opt*
_temp183=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp183->v=( void*) fs; _temp183;}); _temp182->attributes= 0; _temp182;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp179=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp179->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp180=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp180[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp181; _temp181.tag= Cyc_Absyn_Struct_d; _temp181.f1=
sd; _temp181;}); _temp180;}), 0); _temp179->tl= Cyc_Toc_result_decls; _temp179;});}}
static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x, void* t){
void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Structfield* f1=({ struct Cyc_Absyn_Structfield* _temp198=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp198->name= Cyc_Toc_curr_sp; _temp198->tq= Cyc_Toc_mt_tq; _temp198->type=(
void*) tptr; _temp198->width= 0; _temp198->attributes= 0; _temp198;}); struct
Cyc_Absyn_Structfield* f2=({ struct Cyc_Absyn_Structfield* _temp197=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp197->name= Cyc_Toc_base_sp;
_temp197->tq= Cyc_Toc_mt_tq; _temp197->type=( void*) tptr; _temp197->width= 0;
_temp197->attributes= 0; _temp197;}); struct Cyc_Absyn_Structfield* f3=({ struct
Cyc_Absyn_Structfield* _temp196=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp196->name= Cyc_Toc_last_plus_one_sp;
_temp196->tq= Cyc_Toc_mt_tq; _temp196->type=( void*) tptr; _temp196->width= 0;
_temp196->attributes= 0; _temp196;}); struct Cyc_List_List* fs=({ struct Cyc_List_List*
_temp193=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp193->hd=( void*) f1; _temp193->tl=({ struct Cyc_List_List* _temp194=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp194->hd=(
void*) f2; _temp194->tl=({ struct Cyc_List_List* _temp195=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp195->hd=( void*) f3; _temp195->tl=
0; _temp195;}); _temp194;}); _temp193;}); struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp189=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp189->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp189->name=({ struct Cyc_Core_Opt* _temp191=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp191->v=( void*)({ struct _tuple0*
_temp192=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp192->f1=
Cyc_Toc_abs_ns; _temp192->f2= x; _temp192;}); _temp191;}); _temp189->tvs= 0;
_temp189->fields=({ struct Cyc_Core_Opt* _temp190=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp190->v=( void*)(( struct Cyc_List_List*)
fs); _temp190;}); _temp189->attributes= 0; _temp189;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp186=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp186->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp187=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp187[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp188; _temp188.tag= Cyc_Absyn_Struct_d; _temp188.f1=
sd; _temp188;}); _temp187;}), 0); _temp186->tl= Cyc_Toc_result_decls; _temp186;});}
static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int Cyc_Toc_tuple_type_counter=
0; struct _tuple5{ struct _tagged_string* f1; struct Cyc_List_List* f2; } ;
static struct _tagged_string* Cyc_Toc_add_tuple_type( struct Cyc_List_List* tqs0){{
struct Cyc_List_List* tts= Cyc_Toc_tuple_types; for( 0; tts != 0; tts=({ struct
Cyc_List_List* _temp199= tts; if( _temp199 == 0){ _throw( Null_Exception);}
_temp199->tl;})){ struct _tuple5 _temp203; struct Cyc_List_List* _temp204;
struct _tagged_string* _temp206; struct _tuple5* _temp201=( struct _tuple5*)({
struct Cyc_List_List* _temp200= tts; if( _temp200 == 0){ _throw( Null_Exception);}
_temp200->hd;}); _temp203=* _temp201; _LL207: _temp206= _temp203.f1; goto _LL205;
_LL205: _temp204= _temp203.f2; goto _LL202; _LL202: { int okay= 1; struct Cyc_List_List*
tqs= tqs0; for( 0; tqs != 0? _temp204 != 0: 0; tqs=({ struct Cyc_List_List*
_temp208= tqs; if( _temp208 == 0){ _throw( Null_Exception);} _temp208->tl;}),
_temp204=({ struct Cyc_List_List* _temp209= _temp204; if( _temp209 == 0){ _throw(
Null_Exception);} _temp209->tl;})){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)({
struct Cyc_List_List* _temp210= tqs; if( _temp210 == 0){ _throw( Null_Exception);}
_temp210->hd;}))).f2,( void*)({ struct Cyc_List_List* _temp211= _temp204; if(
_temp211 == 0){ _throw( Null_Exception);} _temp211->hd;}))){ okay= 0; break;}}
if(( ! okay? 1: tqs != 0)? 1: _temp204 != 0){ continue;} return _temp206;}}}{
struct _tagged_string* x=({ struct _tagged_string* _temp214=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp214[ 0]=
xprintf("_tuple%d",( Cyc_Toc_tuple_type_counter ++)); _temp214;}); struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); Cyc_Toc_add_tuple_decl( x, ts);
Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp212=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp212->hd=( void*)({ struct
_tuple5* _temp213=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp213->f1= x; _temp213->f2= ts; _temp213;}); _temp212->tl= Cyc_Toc_tuple_types;
_temp212;}); return x;}} static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types=
0; static int Cyc_Toc_tagged_ptr_counter= 0; struct _tuple6{ struct
_tagged_string* f1; void* f2; } ; static struct _tagged_string* Cyc_Toc_add_tagged_ptr_type(
void* t, struct Cyc_Absyn_Tqual tq){{ void* _temp215= t; void* _temp221; void*
_temp223; _LL217: if(( unsigned int) _temp215 > 4u?*(( int*) _temp215) == Cyc_Absyn_IntType:
0){ _LL224: _temp223=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp215)->f1;
if( _temp223 ==( void*) Cyc_Absyn_Unsigned){ goto _LL222;} else{ goto _LL219;}
_LL222: _temp221=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp215)->f2; if(
_temp221 ==( void*) Cyc_Absyn_B1){ goto _LL218;} else{ goto _LL219;}} else{ goto
_LL219;} _LL219: goto _LL220; _LL218: return Cyc_Toc__tagged_string_sp; _LL220:
goto _LL216; _LL216:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts=({ struct Cyc_List_List* _temp225= tts; if( _temp225 == 0){
_throw( Null_Exception);} _temp225->tl;})){ struct _tuple6 _temp229; void*
_temp230; struct _tagged_string* _temp232; struct _tuple6* _temp227=( struct
_tuple6*)({ struct Cyc_List_List* _temp226= tts; if( _temp226 == 0){ _throw(
Null_Exception);} _temp226->hd;}); _temp229=* _temp227; _LL233: _temp232=
_temp229.f1; goto _LL231; _LL231: _temp230= _temp229.f2; goto _LL228; _LL228:
if( ! Cyc_Tcutil_unify( t, _temp230)){ continue;} return _temp232;}}{ struct
_tagged_string* x=({ struct _tagged_string* _temp236=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp236[ 0]= xprintf("_tagged_ptr%d",(
Cyc_Toc_tagged_ptr_counter ++)); _temp236;}); Cyc_Toc_add_tagged_ptr_decl( x, t);
Cyc_Toc_tagged_ptr_types=({ struct Cyc_List_List* _temp234=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp234->hd=( void*)({ struct
_tuple6* _temp235=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp235->f1= x; _temp235->f2= t; _temp235;}); _temp234->tl= Cyc_Toc_tagged_ptr_types;
_temp234;}); return x;}} static int Cyc_Toc_temp_var_counter= 0; static struct
_tuple0* Cyc_Toc_temp_var(){ return({ struct _tuple0* _temp237=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp237->f1= Cyc_Toc_abs_ns; _temp237->f2=({
struct _tagged_string* _temp238=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp238[ 0]= xprintf("_temp%d",( Cyc_Toc_temp_var_counter
++)); _temp238;}); _temp237;});} static int Cyc_Toc_fresh_label_counter= 0;
static struct _tagged_string* Cyc_Toc_fresh_label(){ return({ struct
_tagged_string* _temp239=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp239[ 0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter
++)); _temp239;});} static void* Cyc_Toc_typ_to_c( void* t); static struct
_tuple1* Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp242; struct Cyc_Absyn_Tqual
_temp244; struct Cyc_Core_Opt* _temp246; struct _tuple1 _temp240=* a; _LL247:
_temp246= _temp240.f1; goto _LL245; _LL245: _temp244= _temp240.f2; goto _LL243;
_LL243: _temp242= _temp240.f3; goto _LL241; _LL241: return({ struct _tuple1*
_temp248=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp248->f1=
_temp246; _temp248->f2= _temp244; _temp248->f3= Cyc_Toc_typ_to_c( _temp242);
_temp248;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp251; struct Cyc_Absyn_Tqual _temp253; struct _tuple3 _temp249=* x;
_LL254: _temp253= _temp249.f1; goto _LL252; _LL252: _temp251= _temp249.f2; goto
_LL250; _LL250: return({ struct _tuple3* _temp255=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp255->f1= _temp253; _temp255->f2= Cyc_Toc_typ_to_c(
_temp251); _temp255;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp256= t; struct Cyc_Absyn_Exp* _temp264; struct Cyc_Absyn_Tqual _temp266;
void* _temp268; int _temp270; struct Cyc_Core_Opt* _temp272; struct Cyc_Core_Opt
_temp274; void* _temp275; void* _temp277; _LL258: if(( unsigned int) _temp256 >
4u?*(( int*) _temp256) == Cyc_Absyn_ArrayType: 0){ _LL269: _temp268=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp256)->f1; goto _LL267; _LL267: _temp266=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp256)->f2;
goto _LL265; _LL265: _temp264=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp256)->f3; goto _LL259;} else{ goto _LL260;} _LL260: if(( unsigned int)
_temp256 > 4u?*(( int*) _temp256) == Cyc_Absyn_Evar: 0){ _LL278: _temp277=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp256)->f1; goto _LL273; _LL273: _temp272=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp256)->f2; if(
_temp272 == 0){ goto _LL262;} else{ _temp274=* _temp272; _LL276: _temp275=( void*)
_temp274.v; goto _LL271;} _LL271: _temp270=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp256)->f3; goto _LL261;} else{ goto _LL262;} _LL262: goto _LL263; _LL259:
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp279=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp279[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp280; _temp280.tag= Cyc_Absyn_ArrayType; _temp280.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp268); _temp280.f2= _temp266; _temp280.f3=
_temp264; _temp280;}); _temp279;}); _LL261: return Cyc_Toc_typ_to_c_array(
_temp275); _LL263: return Cyc_Toc_typ_to_c( t); _LL257:;} static void* Cyc_Toc_typ_to_c(
void* t); static struct Cyc_Absyn_Structfield* Cyc_Toc_structfield_to_c( struct
Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield* _temp281=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp281->name= f->name; _temp281->tq= f->tq; _temp281->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp281->width= f->width; _temp281->attributes= f->attributes;
_temp281;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp282= t; int
_temp334; struct Cyc_Core_Opt* _temp336; void* _temp338; int _temp340; struct
Cyc_Core_Opt* _temp342; struct Cyc_Core_Opt _temp344; void* _temp345; void*
_temp347; struct Cyc_Absyn_Tvar* _temp349; struct Cyc_Absyn_TunionInfo _temp351;
void* _temp353; struct Cyc_List_List* _temp355; void* _temp357; struct Cyc_Absyn_Tuniondecl*
_temp359; struct Cyc_Absyn_TunionInfo _temp361; struct Cyc_Absyn_TunionFieldInfo
_temp363; struct Cyc_List_List* _temp365; void* _temp367; struct Cyc_Absyn_Tunionfield*
_temp369; struct Cyc_Absyn_Tuniondecl* _temp371; struct Cyc_Absyn_TunionFieldInfo
_temp373; struct Cyc_Absyn_PtrInfo _temp375; struct Cyc_Absyn_Conref* _temp377;
struct Cyc_Absyn_Tqual _temp379; struct Cyc_Absyn_Conref* _temp381; void*
_temp383; void* _temp385; void* _temp387; void* _temp389; struct Cyc_Absyn_Exp*
_temp391; struct Cyc_Absyn_Tqual _temp393; void* _temp395; struct Cyc_Absyn_FnInfo
_temp397; struct Cyc_List_List* _temp399; int _temp401; struct Cyc_List_List*
_temp403; void* _temp405; struct Cyc_Core_Opt* _temp407; struct Cyc_List_List*
_temp409; struct Cyc_List_List* _temp411; struct Cyc_List_List* _temp413; struct
Cyc_List_List* _temp415; struct Cyc_Absyn_Uniondecl** _temp417; struct Cyc_List_List*
_temp419; struct _tuple0* _temp421; struct Cyc_Absyn_Structdecl** _temp423;
struct Cyc_List_List* _temp425; struct _tuple0* _temp427; struct Cyc_Absyn_Enumdecl*
_temp429; struct _tuple0* _temp431; struct Cyc_Core_Opt* _temp433; struct Cyc_List_List*
_temp435; struct _tuple0* _temp437; void* _temp439; void* _temp441; struct Cyc_List_List*
_temp443; _LL284: if( _temp282 ==( void*) Cyc_Absyn_VoidType){ goto _LL285;}
else{ goto _LL286;} _LL286: if(( unsigned int) _temp282 > 4u?*(( int*) _temp282)
== Cyc_Absyn_Evar: 0){ _LL339: _temp338=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp282)->f1; goto _LL337; _LL337: _temp336=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp282)->f2; if( _temp336 == 0){ goto _LL335;} else{
goto _LL288;} _LL335: _temp334=( int)(( struct Cyc_Absyn_Evar_struct*) _temp282)->f3;
goto _LL287;} else{ goto _LL288;} _LL288: if(( unsigned int) _temp282 > 4u?*((
int*) _temp282) == Cyc_Absyn_Evar: 0){ _LL348: _temp347=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp282)->f1; goto _LL343; _LL343: _temp342=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp282)->f2; if( _temp342 == 0){ goto _LL290;} else{
_temp344=* _temp342; _LL346: _temp345=( void*) _temp344.v; goto _LL341;} _LL341:
_temp340=( int)(( struct Cyc_Absyn_Evar_struct*) _temp282)->f3; goto _LL289;}
else{ goto _LL290;} _LL290: if(( unsigned int) _temp282 > 4u?*(( int*) _temp282)
== Cyc_Absyn_VarType: 0){ _LL350: _temp349=( struct Cyc_Absyn_Tvar*)(( struct
Cyc_Absyn_VarType_struct*) _temp282)->f1; goto _LL291;} else{ goto _LL292;}
_LL292: if(( unsigned int) _temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_TunionType:
0){ _LL352: _temp351=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp282)->f1; _LL358: _temp357=( void*) _temp351.tunion_info; if(*(( int*)
_temp357) == Cyc_Absyn_KnownTunion){ _LL360: _temp359=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp357)->f1; goto _LL356;} else{ goto
_LL294;} _LL356: _temp355=( struct Cyc_List_List*) _temp351.targs; goto _LL354;
_LL354: _temp353=( void*) _temp351.rgn; goto _LL293;} else{ goto _LL294;} _LL294:
if(( unsigned int) _temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_TunionType: 0){
_LL362: _temp361=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp282)->f1; goto _LL295;} else{ goto _LL296;} _LL296: if(( unsigned int)
_temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_TunionFieldType: 0){ _LL364:
_temp363=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp282)->f1; _LL368: _temp367=( void*) _temp363.field_info; if(*(( int*)
_temp367) == Cyc_Absyn_KnownTunionfield){ _LL372: _temp371=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp367)->f1; goto _LL370; _LL370:
_temp369=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp367)->f2; goto _LL366;} else{ goto _LL298;} _LL366: _temp365=( struct Cyc_List_List*)
_temp363.targs; goto _LL297;} else{ goto _LL298;} _LL298: if(( unsigned int)
_temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_TunionFieldType: 0){ _LL374:
_temp373=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp282)->f1; goto _LL299;} else{ goto _LL300;} _LL300: if(( unsigned int)
_temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_PointerType: 0){ _LL376: _temp375=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp282)->f1;
_LL386: _temp385=( void*) _temp375.elt_typ; goto _LL384; _LL384: _temp383=( void*)
_temp375.rgn_typ; goto _LL382; _LL382: _temp381=( struct Cyc_Absyn_Conref*)
_temp375.nullable; goto _LL380; _LL380: _temp379=( struct Cyc_Absyn_Tqual)
_temp375.tq; goto _LL378; _LL378: _temp377=( struct Cyc_Absyn_Conref*) _temp375.bounds;
goto _LL301;} else{ goto _LL302;} _LL302: if(( unsigned int) _temp282 > 4u?*((
int*) _temp282) == Cyc_Absyn_IntType: 0){ _LL390: _temp389=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp282)->f1; goto _LL388; _LL388: _temp387=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp282)->f2; goto _LL303;} else{ goto _LL304;} _LL304: if( _temp282 ==( void*)
Cyc_Absyn_FloatType){ goto _LL305;} else{ goto _LL306;} _LL306: if( _temp282 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL307;} else{ goto _LL308;} _LL308: if((
unsigned int) _temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_ArrayType: 0){
_LL396: _temp395=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp282)->f1;
goto _LL394; _LL394: _temp393=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp282)->f2; goto _LL392; _LL392: _temp391=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp282)->f3; goto _LL309;} else{ goto _LL310;}
_LL310: if(( unsigned int) _temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_FnType:
0){ _LL398: _temp397=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp282)->f1; _LL410: _temp409=( struct Cyc_List_List*) _temp397.tvars; goto
_LL408; _LL408: _temp407=( struct Cyc_Core_Opt*) _temp397.effect; goto _LL406;
_LL406: _temp405=( void*) _temp397.ret_typ; goto _LL404; _LL404: _temp403=(
struct Cyc_List_List*) _temp397.args; goto _LL402; _LL402: _temp401=( int)
_temp397.varargs; goto _LL400; _LL400: _temp399=( struct Cyc_List_List*)
_temp397.attributes; goto _LL311;} else{ goto _LL312;} _LL312: if(( unsigned int)
_temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_TupleType: 0){ _LL412: _temp411=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp282)->f1; goto
_LL313;} else{ goto _LL314;} _LL314: if(( unsigned int) _temp282 > 4u?*(( int*)
_temp282) == Cyc_Absyn_AnonStructType: 0){ _LL414: _temp413=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp282)->f1; goto _LL315;} else{ goto
_LL316;} _LL316: if(( unsigned int) _temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_AnonUnionType:
0){ _LL416: _temp415=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp282)->f1; goto _LL317;} else{ goto _LL318;} _LL318: if(( unsigned int)
_temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_UnionType: 0){ _LL422: _temp421=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp282)->f1; goto
_LL420; _LL420: _temp419=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp282)->f2; goto _LL418; _LL418: _temp417=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp282)->f3; goto _LL319;} else{ goto
_LL320;} _LL320: if(( unsigned int) _temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_StructType:
0){ _LL428: _temp427=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp282)->f1; goto _LL426; _LL426: _temp425=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp282)->f2; goto _LL424; _LL424: _temp423=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp282)->f3;
goto _LL321;} else{ goto _LL322;} _LL322: if(( unsigned int) _temp282 > 4u?*((
int*) _temp282) == Cyc_Absyn_EnumType: 0){ _LL432: _temp431=( struct _tuple0*)((
struct Cyc_Absyn_EnumType_struct*) _temp282)->f1; goto _LL430; _LL430: _temp429=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp282)->f2;
goto _LL323;} else{ goto _LL324;} _LL324: if(( unsigned int) _temp282 > 4u?*((
int*) _temp282) == Cyc_Absyn_TypedefType: 0){ _LL438: _temp437=( struct _tuple0*)((
struct Cyc_Absyn_TypedefType_struct*) _temp282)->f1; goto _LL436; _LL436:
_temp435=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp282)->f2; goto _LL434; _LL434: _temp433=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp282)->f3; goto _LL325;} else{ goto _LL326;}
_LL326: if(( unsigned int) _temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_RgnHandleType:
0){ _LL440: _temp439=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp282)->f1;
goto _LL327;} else{ goto _LL328;} _LL328: if( _temp282 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL329;} else{ goto _LL330;} _LL330: if(( unsigned int) _temp282 > 4u?*((
int*) _temp282) == Cyc_Absyn_AccessEff: 0){ _LL442: _temp441=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp282)->f1; goto _LL331;} else{ goto _LL332;}
_LL332: if(( unsigned int) _temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_JoinEff:
0){ _LL444: _temp443=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp282)->f1; goto _LL333;} else{ goto _LL283;} _LL285: return t; _LL287:
return( void*) Cyc_Absyn_VoidType; _LL289: return Cyc_Toc_typ_to_c( _temp345);
_LL291: return Cyc_Absyn_void_star_typ(); _LL293: return Cyc_Absyn_void_star_typ();
_LL295:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp445=( char*)"unresolved TunionType"; struct
_tagged_string _temp446; _temp446.curr= _temp445; _temp446.base= _temp445;
_temp446.last_plus_one= _temp445 + 22; _temp446;}))); _LL297: if( _temp369->typs
== 0){ if( _temp371->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp369->name,(
struct _tagged_string)({ char* _temp447=( char*)"_struct"; struct _tagged_string
_temp448; _temp448.curr= _temp447; _temp448.base= _temp447; _temp448.last_plus_one=
_temp447 + 8; _temp448;})));} _LL299:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp449=(
char*)"unresolved TunionFieldType"; struct _tagged_string _temp450; _temp450.curr=
_temp449; _temp450.base= _temp449; _temp450.last_plus_one= _temp449 + 27;
_temp450;}))); _LL301: _temp385= Cyc_Toc_typ_to_c_array( _temp385);{ void*
_temp451=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp377))->v; void* _temp457; _LL453: if((
unsigned int) _temp451 > 1u?*(( int*) _temp451) == Cyc_Absyn_Eq_constr: 0){
_LL458: _temp457=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp451)->f1;
if( _temp457 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL454;} else{ goto _LL455;}}
else{ goto _LL455;} _LL455: goto _LL456; _LL454: { struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
_temp385, _temp379); return Cyc_Absyn_strct( n);} _LL456: return Cyc_Absyn_star_typ(
_temp385,( void*) Cyc_Absyn_HeapRgn, _temp379); _LL452:;} _LL303: return t;
_LL305: return t; _LL307: return t; _LL309: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp459=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp459[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp460; _temp460.tag= Cyc_Absyn_ArrayType;
_temp460.f1=( void*) Cyc_Toc_typ_to_c_array( _temp395); _temp460.f2= _temp393;
_temp460.f3= _temp391; _temp460;}); _temp459;}); _LL311: { struct Cyc_List_List*
new_atts= 0; for( 0; _temp399 != 0; _temp399=({ struct Cyc_List_List* _temp461=
_temp399; if( _temp461 == 0){ _throw( Null_Exception);} _temp461->tl;})){ void*
_temp463=( void*)({ struct Cyc_List_List* _temp462= _temp399; if( _temp462 == 0){
_throw( Null_Exception);} _temp462->hd;}); _LL465: if( _temp463 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL466;} else{ goto _LL467;} _LL467: if( _temp463 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL468;} else{ goto _LL469;} _LL469: if( _temp463 ==( void*) Cyc_Absyn_Const_att){
goto _LL470;} else{ goto _LL471;} _LL471: goto _LL472; _LL466: goto _LL468;
_LL468: goto _LL470; _LL470: continue; _LL472: new_atts=({ struct Cyc_List_List*
_temp473=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp473->hd=( void*)(( void*)({ struct Cyc_List_List* _temp474= _temp399; if(
_temp474 == 0){ _throw( Null_Exception);} _temp474->hd;})); _temp473->tl=
new_atts; _temp473;}); goto _LL464; _LL464:;} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp475=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp475[ 0]=({ struct Cyc_Absyn_FnType_struct _temp476; _temp476.tag= Cyc_Absyn_FnType;
_temp476.f1=({ struct Cyc_Absyn_FnInfo _temp477; _temp477.tvars= 0; _temp477.effect=
0; _temp477.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp405); _temp477.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp403); _temp477.varargs= _temp401;
_temp477.attributes= new_atts; _temp477;}); _temp476;}); _temp475;});} _LL313:
_temp411=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp411);{ struct
_tagged_string* n= Cyc_Toc_add_tuple_type( _temp411); return Cyc_Absyn_strct( n);}
_LL315: return( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp478=(
struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp478[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp479; _temp479.tag=
Cyc_Absyn_AnonStructType; _temp479.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp413); _temp479;}); _temp478;}); _LL317: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp480=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp480[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp481; _temp481.tag=
Cyc_Absyn_AnonUnionType; _temp481.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp415); _temp481;}); _temp480;}); _LL319: if( _temp421 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp482=( char*)"anonymous union"; struct _tagged_string _temp483; _temp483.curr=
_temp482; _temp483.base= _temp482; _temp483.last_plus_one= _temp482 + 16;
_temp483;}));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)({ struct _tuple0*
_temp484= _temp421; if( _temp484 == 0){ _throw( Null_Exception);} _temp484;}));
_LL321: if( _temp427 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp485=( char*)"anonymous struct"; struct
_tagged_string _temp486; _temp486.curr= _temp485; _temp486.base= _temp485;
_temp486.last_plus_one= _temp485 + 17; _temp486;}));} return Cyc_Absyn_strctq((
struct _tuple0*)({ struct _tuple0* _temp487= _temp427; if( _temp487 == 0){
_throw( Null_Exception);} _temp487;})); _LL323: return t; _LL325: if( _temp433
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp488=( char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp489; _temp489.curr= _temp488; _temp489.base= _temp488;
_temp489.last_plus_one= _temp488 + 38; _temp489;}));} return( void*)({ struct
Cyc_Absyn_TypedefType_struct* _temp490=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp490[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp491; _temp491.tag= Cyc_Absyn_TypedefType;
_temp491.f1= _temp437; _temp491.f2= 0; _temp491.f3=({ struct Cyc_Core_Opt*
_temp492=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp492->v=( void*) Cyc_Toc_typ_to_c_array(( void*)({ struct Cyc_Core_Opt*
_temp493= _temp433; if( _temp493 == 0){ _throw( Null_Exception);} _temp493->v;}));
_temp492;}); _temp491;}); _temp490;}); _LL327: return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__region_handle_sp), Cyc_Toc_mt_tq); _LL329: return(( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp494=(
char*)"Toc::typ_to_c: type translation passed the heap region"; struct
_tagged_string _temp495; _temp495.curr= _temp494; _temp495.base= _temp494;
_temp495.last_plus_one= _temp494 + 55; _temp495;})); _LL331: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp496=( char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp497; _temp497.curr= _temp496; _temp497.base= _temp496;
_temp497.last_plus_one= _temp496 + 54; _temp497;})); _LL333: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp498=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp499; _temp499.curr= _temp498; _temp499.base= _temp498;
_temp499.last_plus_one= _temp498 + 52; _temp499;})); _LL283:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp500= t; struct Cyc_Absyn_Exp* _temp506; struct Cyc_Absyn_Tqual
_temp508; void* _temp510; _LL502: if(( unsigned int) _temp500 > 4u?*(( int*)
_temp500) == Cyc_Absyn_ArrayType: 0){ _LL511: _temp510=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp500)->f1; goto _LL509; _LL509: _temp508=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp500)->f2; goto _LL507; _LL507: _temp506=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp500)->f3; goto
_LL503;} else{ goto _LL504;} _LL504: goto _LL505; _LL503: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp510,( void*) Cyc_Absyn_HeapRgn, _temp508), e, l);
_LL505: return e; _LL501:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp512= t; struct Cyc_Absyn_Exp* _temp518; struct Cyc_Absyn_Tqual _temp520;
void* _temp522; _LL514: if(( unsigned int) _temp512 > 4u?*(( int*) _temp512) ==
Cyc_Absyn_ArrayType: 0){ _LL523: _temp522=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp512)->f1; goto _LL521; _LL521: _temp520=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp512)->f2; goto _LL519; _LL519: _temp518=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp512)->f3; goto
_LL515;} else{ goto _LL516;} _LL516: goto _LL517; _LL515: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp522,( void*) Cyc_Absyn_HeapRgn, _temp520), e, l);
_LL517: return Cyc_Absyn_cast_exp( t, e, l); _LL513:;} static int Cyc_Toc_atomic_typ(
void* t){ void* _temp524= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp560; void* _temp562; void* _temp564; struct Cyc_Absyn_Exp* _temp566; struct
Cyc_Absyn_Tqual _temp568; void* _temp570; struct Cyc_Absyn_FnInfo _temp572;
struct Cyc_List_List* _temp574; struct Cyc_Absyn_Structdecl** _temp576; struct
Cyc_List_List* _temp578; struct _tuple0* _temp580; struct Cyc_Absyn_TunionInfo
_temp582; struct Cyc_List_List* _temp584; struct Cyc_List_List* _temp586; struct
Cyc_Absyn_TunionFieldInfo _temp588; struct Cyc_List_List* _temp590; void*
_temp592; struct Cyc_Absyn_Tunionfield* _temp594; struct Cyc_Absyn_Tuniondecl*
_temp596; struct Cyc_Absyn_Uniondecl** _temp598; struct Cyc_List_List* _temp600;
struct _tuple0* _temp602; struct Cyc_Absyn_PtrInfo _temp604; void* _temp606;
_LL526: if( _temp524 ==( void*) Cyc_Absyn_VoidType){ goto _LL527;} else{ goto
_LL528;} _LL528: if(( unsigned int) _temp524 > 4u?*(( int*) _temp524) == Cyc_Absyn_VarType:
0){ _LL561: _temp560=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp524)->f1; goto _LL529;} else{ goto _LL530;} _LL530: if(( unsigned int)
_temp524 > 4u?*(( int*) _temp524) == Cyc_Absyn_IntType: 0){ _LL565: _temp564=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp524)->f1; goto _LL563; _LL563:
_temp562=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp524)->f2; goto _LL531;}
else{ goto _LL532;} _LL532: if( _temp524 ==( void*) Cyc_Absyn_FloatType){ goto
_LL533;} else{ goto _LL534;} _LL534: if( _temp524 ==( void*) Cyc_Absyn_DoubleType){
goto _LL535;} else{ goto _LL536;} _LL536: if(( unsigned int) _temp524 > 4u?*((
int*) _temp524) == Cyc_Absyn_ArrayType: 0){ _LL571: _temp570=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp524)->f1; goto _LL569; _LL569: _temp568=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp524)->f2;
goto _LL567; _LL567: _temp566=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp524)->f3; goto _LL537;} else{ goto _LL538;} _LL538: if(( unsigned int)
_temp524 > 4u?*(( int*) _temp524) == Cyc_Absyn_FnType: 0){ _LL573: _temp572=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp524)->f1; goto
_LL539;} else{ goto _LL540;} _LL540: if(( unsigned int) _temp524 > 4u?*(( int*)
_temp524) == Cyc_Absyn_TupleType: 0){ _LL575: _temp574=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp524)->f1; goto _LL541;} else{ goto
_LL542;} _LL542: if(( unsigned int) _temp524 > 4u?*(( int*) _temp524) == Cyc_Absyn_StructType:
0){ _LL581: _temp580=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp524)->f1; goto _LL579; _LL579: _temp578=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp524)->f2; goto _LL577; _LL577: _temp576=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp524)->f3;
goto _LL543;} else{ goto _LL544;} _LL544: if(( unsigned int) _temp524 > 4u?*((
int*) _temp524) == Cyc_Absyn_TunionType: 0){ _LL583: _temp582=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp524)->f1; goto _LL545;} else{ goto
_LL546;} _LL546: if(( unsigned int) _temp524 > 4u?*(( int*) _temp524) == Cyc_Absyn_AnonStructType:
0){ _LL585: _temp584=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp524)->f1; goto _LL547;} else{ goto _LL548;} _LL548: if(( unsigned int)
_temp524 > 4u?*(( int*) _temp524) == Cyc_Absyn_AnonUnionType: 0){ _LL587:
_temp586=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp524)->f1; goto _LL549;} else{ goto _LL550;} _LL550: if(( unsigned int)
_temp524 > 4u?*(( int*) _temp524) == Cyc_Absyn_TunionFieldType: 0){ _LL589:
_temp588=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp524)->f1; _LL593: _temp592=( void*) _temp588.field_info; if(*(( int*)
_temp592) == Cyc_Absyn_KnownTunionfield){ _LL597: _temp596=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp592)->f1; goto _LL595; _LL595:
_temp594=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp592)->f2; goto _LL591;} else{ goto _LL552;} _LL591: _temp590=( struct Cyc_List_List*)
_temp588.targs; goto _LL551;} else{ goto _LL552;} _LL552: if(( unsigned int)
_temp524 > 4u?*(( int*) _temp524) == Cyc_Absyn_UnionType: 0){ _LL603: _temp602=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp524)->f1; goto
_LL601; _LL601: _temp600=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp524)->f2; goto _LL599; _LL599: _temp598=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp524)->f3; goto _LL553;} else{ goto
_LL554;} _LL554: if(( unsigned int) _temp524 > 4u?*(( int*) _temp524) == Cyc_Absyn_PointerType:
0){ _LL605: _temp604=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp524)->f1; goto _LL555;} else{ goto _LL556;} _LL556: if(( unsigned int)
_temp524 > 4u?*(( int*) _temp524) == Cyc_Absyn_RgnHandleType: 0){ _LL607:
_temp606=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp524)->f1; goto
_LL557;} else{ goto _LL558;} _LL558: goto _LL559; _LL527: return 1; _LL529:
return 0; _LL531: return 1; _LL533: return 1; _LL535: return 0; _LL537: return
Cyc_Toc_atomic_typ( _temp570); _LL539: return 1; _LL541: for( 0; _temp574 != 0;
_temp574=({ struct Cyc_List_List* _temp608= _temp574; if( _temp608 == 0){ _throw(
Null_Exception);} _temp608->tl;})){ if( ! Cyc_Toc_atomic_typ((*(( struct _tuple3*)({
struct Cyc_List_List* _temp609= _temp574; if( _temp609 == 0){ _throw(
Null_Exception);} _temp609->hd;}))).f2)){ return 0;}} return 1; _LL543: return 0;
_LL545: return 0; _LL547: _temp586= _temp584; goto _LL549; _LL549: for( 0;
_temp586 != 0; _temp586=({ struct Cyc_List_List* _temp610= _temp586; if(
_temp610 == 0){ _throw( Null_Exception);} _temp610->tl;})){ if( ! Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp611=
_temp586; if( _temp611 == 0){ _throw( Null_Exception);} _temp611->hd;}))->type)){
return 0;}} return 1; _LL551: return _temp594->typs == 0; _LL553: return 0;
_LL555: return 0; _LL557: return 0; _LL559:(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp612= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _temp612.last_plus_one - _temp612.curr,
_temp612.curr);})); return 0; _LL525:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp613= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp619;
struct Cyc_Absyn_Conref* _temp621; struct Cyc_Absyn_Tqual _temp623; struct Cyc_Absyn_Conref*
_temp625; void* _temp627; void* _temp629; _LL615: if(( unsigned int) _temp613 >
4u?*(( int*) _temp613) == Cyc_Absyn_PointerType: 0){ _LL620: _temp619=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp613)->f1; _LL630:
_temp629=( void*) _temp619.elt_typ; goto _LL628; _LL628: _temp627=( void*)
_temp619.rgn_typ; goto _LL626; _LL626: _temp625=( struct Cyc_Absyn_Conref*)
_temp619.nullable; goto _LL624; _LL624: _temp623=( struct Cyc_Absyn_Tqual)
_temp619.tq; goto _LL622; _LL622: _temp621=( struct Cyc_Absyn_Conref*) _temp619.bounds;
goto _LL616;} else{ goto _LL617;} _LL617: goto _LL618; _LL616: { void* _temp631=
Cyc_Tcutil_compress( _temp629); _LL633: if( _temp631 ==( void*) Cyc_Absyn_VoidType){
goto _LL634;} else{ goto _LL635;} _LL635: goto _LL636; _LL634: return 1; _LL636:
return 0; _LL632:;} _LL618: return 0; _LL614:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp637= Cyc_Tcutil_compress( t);
struct Cyc_List_List* _temp653; struct Cyc_List_List* _temp655; struct Cyc_Absyn_Structdecl**
_temp657; struct Cyc_Absyn_Structdecl* _temp659; struct Cyc_List_List* _temp660;
struct _tuple0* _temp662; struct Cyc_Absyn_Structdecl** _temp664; struct Cyc_List_List*
_temp666; struct _tuple0* _temp668; struct Cyc_Absyn_Uniondecl** _temp670;
struct Cyc_Absyn_Uniondecl* _temp672; struct Cyc_List_List* _temp673; struct
_tuple0* _temp675; struct Cyc_Absyn_Uniondecl** _temp677; struct Cyc_List_List*
_temp679; struct _tuple0* _temp681; _LL639: if(( unsigned int) _temp637 > 4u?*((
int*) _temp637) == Cyc_Absyn_AnonUnionType: 0){ _LL654: _temp653=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp637)->f1; goto _LL640;} else{ goto
_LL641;} _LL641: if(( unsigned int) _temp637 > 4u?*(( int*) _temp637) == Cyc_Absyn_AnonStructType:
0){ _LL656: _temp655=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp637)->f1; goto _LL642;} else{ goto _LL643;} _LL643: if(( unsigned int)
_temp637 > 4u?*(( int*) _temp637) == Cyc_Absyn_StructType: 0){ _LL663: _temp662=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp637)->f1; goto
_LL661; _LL661: _temp660=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp637)->f2; goto _LL658; _LL658: _temp657=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp637)->f3; if( _temp657 == 0){ goto
_LL645;} else{ _temp659=* _temp657; goto _LL644;}} else{ goto _LL645;} _LL645:
if(( unsigned int) _temp637 > 4u?*(( int*) _temp637) == Cyc_Absyn_StructType: 0){
_LL669: _temp668=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp637)->f1; goto _LL667; _LL667: _temp666=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp637)->f2; goto _LL665; _LL665: _temp664=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp637)->f3;
if( _temp664 == 0){ goto _LL646;} else{ goto _LL647;}} else{ goto _LL647;}
_LL647: if(( unsigned int) _temp637 > 4u?*(( int*) _temp637) == Cyc_Absyn_UnionType:
0){ _LL676: _temp675=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp637)->f1; goto _LL674; _LL674: _temp673=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp637)->f2; goto _LL671; _LL671: _temp670=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp637)->f3;
if( _temp670 == 0){ goto _LL649;} else{ _temp672=* _temp670; goto _LL648;}}
else{ goto _LL649;} _LL649: if(( unsigned int) _temp637 > 4u?*(( int*) _temp637)
== Cyc_Absyn_UnionType: 0){ _LL682: _temp681=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp637)->f1; goto _LL680; _LL680: _temp679=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp637)->f2; goto _LL678; _LL678: _temp677=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp637)->f3;
if( _temp677 == 0){ goto _LL650;} else{ goto _LL651;}} else{ goto _LL651;}
_LL651: goto _LL652; _LL640: _temp655= _temp653; goto _LL642; _LL642: { struct
Cyc_Absyn_Structfield* field= Cyc_Absyn_lookup_field( _temp655, f); if( field ==
0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp684=*({ struct Cyc_Absyn_Structfield* _temp683= field; if(
_temp683 == 0){ _throw( Null_Exception);} _temp683->name;}); xprintf("is_poly_field: bad field %.*s",
_temp684.last_plus_one - _temp684.curr, _temp684.curr);}));} return Cyc_Toc_is_void_star((
void*)({ struct Cyc_Absyn_Structfield* _temp685= field; if( _temp685 == 0){
_throw( Null_Exception);} _temp685->type;}));} _LL644: if( _temp662 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp686=( char*)"StructType name not properly set"; struct _tagged_string
_temp687; _temp687.curr= _temp686; _temp687.base= _temp686; _temp687.last_plus_one=
_temp686 + 33; _temp687;}));} if( _temp659->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp689= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp688= _temp662; if( _temp688 == 0){
_throw( Null_Exception);} _temp688;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp689.last_plus_one - _temp689.curr, _temp689.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp696= _temp659->fields;
if( _temp696 == 0){ _throw( Null_Exception);} _temp696->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp690= fields; if( _temp690 == 0){ _throw(
Null_Exception);} _temp690->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp691= fields; if( _temp691 == 0){ _throw(
Null_Exception);} _temp691->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp692= fields;
if( _temp692 == 0){ _throw( Null_Exception);} _temp692->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp694=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp693= _temp662;
if( _temp693 == 0){ _throw( Null_Exception);} _temp693;})); struct
_tagged_string _temp695=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp694.last_plus_one - _temp694.curr, _temp694.curr, _temp695.last_plus_one -
_temp695.curr, _temp695.curr);})); return 0;} _LL646: if( _temp668 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp697=( char*)"StructType name not properly set"; struct _tagged_string
_temp698; _temp698.curr= _temp697; _temp698.base= _temp697; _temp698.last_plus_one=
_temp697 + 33; _temp698;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp700= Cyc_Absynpp_qvar2string(( struct _tuple0*)({
struct _tuple0* _temp699= _temp668; if( _temp699 == 0){ _throw( Null_Exception);}
_temp699;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp700.last_plus_one
- _temp700.curr, _temp700.curr);})); return 0; _LL648: if( _temp675 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp701=( char*)"UnionType name not properly set"; struct _tagged_string
_temp702; _temp702.curr= _temp701; _temp702.base= _temp701; _temp702.last_plus_one=
_temp701 + 32; _temp702;}));} if( _temp672->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp704= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp703= _temp675; if( _temp703 == 0){
_throw( Null_Exception);} _temp703;})); xprintf("is_poly_field:  union %.*s missing fields",
_temp704.last_plus_one - _temp704.curr, _temp704.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp711= _temp672->fields;
if( _temp711 == 0){ _throw( Null_Exception);} _temp711->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp705= fields; if( _temp705 == 0){ _throw(
Null_Exception);} _temp705->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp706= fields; if( _temp706 == 0){ _throw(
Null_Exception);} _temp706->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp707= fields;
if( _temp707 == 0){ _throw( Null_Exception);} _temp707->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp709=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp708= _temp675;
if( _temp708 == 0){ _throw( Null_Exception);} _temp708;})); struct
_tagged_string _temp710=* f; xprintf("is_poly_field:  union %.*s has no %.*s field",
_temp709.last_plus_one - _temp709.curr, _temp709.curr, _temp710.last_plus_one -
_temp710.curr, _temp710.curr);})); return 0;} _LL650: if( _temp681 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp712=( char*)"UnionType name not properly set"; struct _tagged_string
_temp713; _temp713.curr= _temp712; _temp713.base= _temp712; _temp713.last_plus_one=
_temp712 + 32; _temp713;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp715= Cyc_Absynpp_qvar2string(( struct _tuple0*)({
struct _tuple0* _temp714= _temp681; if( _temp714 == 0){ _throw( Null_Exception);}
_temp714;})); xprintf("is_poly_field:  union %.*s missing structdecl", _temp715.last_plus_one
- _temp715.curr, _temp715.curr);})); return 0; _LL652:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp716= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp716.last_plus_one - _temp716.curr,
_temp716.curr);})); return 0; _LL638:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp717=( void*) e->r; struct _tagged_string*
_temp725; struct Cyc_Absyn_Exp* _temp727; struct _tagged_string* _temp729;
struct Cyc_Absyn_Exp* _temp731; _LL719: if(*(( int*) _temp717) == Cyc_Absyn_StructMember_e){
_LL728: _temp727=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp717)->f1; goto _LL726; _LL726: _temp725=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp717)->f2; goto _LL720;} else{ goto _LL721;}
_LL721: if(*(( int*) _temp717) == Cyc_Absyn_StructArrow_e){ _LL732: _temp731=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp717)->f1;
goto _LL730; _LL730: _temp729=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp717)->f2; goto _LL722;} else{ goto _LL723;} _LL723: goto _LL724; _LL720:
return Cyc_Toc_is_poly_field(( void*)({ struct Cyc_Core_Opt* _temp733= _temp727->topt;
if( _temp733 == 0){ _throw( Null_Exception);} _temp733->v;}), _temp725); _LL722: {
void* _temp735= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp734=
_temp731->topt; if( _temp734 == 0){ _throw( Null_Exception);} _temp734->v;}));
struct Cyc_Absyn_PtrInfo _temp741; struct Cyc_Absyn_Conref* _temp743; struct Cyc_Absyn_Tqual
_temp745; struct Cyc_Absyn_Conref* _temp747; void* _temp749; void* _temp751;
_LL737: if(( unsigned int) _temp735 > 4u?*(( int*) _temp735) == Cyc_Absyn_PointerType:
0){ _LL742: _temp741=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp735)->f1; _LL752: _temp751=( void*) _temp741.elt_typ; goto _LL750; _LL750:
_temp749=( void*) _temp741.rgn_typ; goto _LL748; _LL748: _temp747=( struct Cyc_Absyn_Conref*)
_temp741.nullable; goto _LL746; _LL746: _temp745=( struct Cyc_Absyn_Tqual)
_temp741.tq; goto _LL744; _LL744: _temp743=( struct Cyc_Absyn_Conref*) _temp741.bounds;
goto _LL738;} else{ goto _LL739;} _LL739: goto _LL740; _LL738: return Cyc_Toc_is_poly_field(
_temp751, _temp729); _LL740:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp754= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp753= _temp731->topt; if( _temp753 == 0){ _throw( Null_Exception);} _temp753->v;}));
xprintf("is_poly_project:  bad type %.*s", _temp754.last_plus_one - _temp754.curr,
_temp754.curr);})); return 0; _LL736:;} _LL724: return 0; _LL718:;} static char
_temp757[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp757, _temp757, _temp757 + 10u}; static struct
_tuple0 Cyc_Toc_gc_malloc_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var,
0),({ struct Cyc_List_List* _temp758=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp758->hd=( void*) s; _temp758->tl= 0; _temp758;}), 0);}
static char _temp761[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp761, _temp761, _temp761 + 17u}; static struct
_tuple0 Cyc_Toc_gc_malloc_atomic_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_atomic_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var,
0),({ struct Cyc_List_List* _temp762=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp762->hd=( void*) s; _temp762->tl= 0; _temp762;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static char _temp765[ 15u]="_region_malloc"; static struct _tagged_string
Cyc_Toc__region_malloc_string=( struct _tagged_string){ _temp765, _temp765,
_temp765 + 15u}; static struct _tuple0 Cyc_Toc_region_malloc_var=( struct
_tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__region_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_region_malloc_var,
0),({ struct Cyc_List_List* _temp766=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp766->hd=( void*) rgn; _temp766->tl=({ struct Cyc_List_List*
_temp767=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp767->hd=( void*) s; _temp767->tl= 0; _temp767;}); _temp766;}), 0);} static
const int Cyc_Toc_NullCheck= 0; struct Cyc_Toc_NullCheck_struct{ int tag; void*
f1; } ; static const int Cyc_Toc_UntagPtr= 1; struct Cyc_Toc_UntagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; int f3; } ; static const int Cyc_Toc_TagPtr=
2; struct Cyc_Toc_TagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; }
; typedef void* Cyc_Toc_conv; struct _tuple7{ void* f1; void* f2; } ; static
struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp769=({ struct _tuple7
_temp768; _temp768.f1= t1; _temp768.f2= t2; _temp768;}); void* _temp775; struct
Cyc_Absyn_PtrInfo _temp777; void* _temp779; struct Cyc_Absyn_PtrInfo _temp781;
_LL771: _LL780: _temp779= _temp769.f1; if(( unsigned int) _temp779 > 4u?*(( int*)
_temp779) == Cyc_Absyn_PointerType: 0){ _LL782: _temp781=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp779)->f1; goto _LL776;} else{ goto
_LL773;} _LL776: _temp775= _temp769.f2; if(( unsigned int) _temp775 > 4u?*(( int*)
_temp775) == Cyc_Absyn_PointerType: 0){ _LL778: _temp777=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp775)->f1; goto _LL772;} else{ goto
_LL773;} _LL773: goto _LL774; _LL772: { int n1=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp781.nullable); int n2=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp777.nullable); void* b1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp781.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp777.bounds); struct Cyc_List_List* convs= 0;{
struct _tuple7 _temp784=({ struct _tuple7 _temp783; _temp783.f1= b1; _temp783.f2=
b2; _temp783;}); void* _temp794; struct Cyc_Absyn_Exp* _temp796; void* _temp798;
struct Cyc_Absyn_Exp* _temp800; void* _temp802; void* _temp804; void* _temp806;
void* _temp808; struct Cyc_Absyn_Exp* _temp810; void* _temp812; struct Cyc_Absyn_Exp*
_temp814; void* _temp816; _LL786: _LL799: _temp798= _temp784.f1; if((
unsigned int) _temp798 > 1u?*(( int*) _temp798) == Cyc_Absyn_Upper_b: 0){ _LL801:
_temp800=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp798)->f1;
goto _LL795;} else{ goto _LL788;} _LL795: _temp794= _temp784.f2; if((
unsigned int) _temp794 > 1u?*(( int*) _temp794) == Cyc_Absyn_Upper_b: 0){ _LL797:
_temp796=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp794)->f1;
goto _LL787;} else{ goto _LL788;} _LL788: _LL805: _temp804= _temp784.f1; if(
_temp804 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL803;} else{ goto _LL790;}
_LL803: _temp802= _temp784.f2; if( _temp802 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL789;} else{ goto _LL790;} _LL790: _LL809: _temp808= _temp784.f1; if((
unsigned int) _temp808 > 1u?*(( int*) _temp808) == Cyc_Absyn_Upper_b: 0){ _LL811:
_temp810=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp808)->f1;
goto _LL807;} else{ goto _LL792;} _LL807: _temp806= _temp784.f2; if( _temp806 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL791;} else{ goto _LL792;} _LL792: _LL817:
_temp816= _temp784.f1; if( _temp816 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL813;}
else{ goto _LL785;} _LL813: _temp812= _temp784.f2; if(( unsigned int) _temp812 >
1u?*(( int*) _temp812) == Cyc_Absyn_Upper_b: 0){ _LL815: _temp814=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp812)->f1; goto _LL793;} else{ goto _LL785;}
_LL787: if( n1? ! n2: 0){ convs=({ struct Cyc_List_List* _temp818=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp818->hd=( void*)(( void*)({
struct Cyc_Toc_NullCheck_struct* _temp819=( struct Cyc_Toc_NullCheck_struct*)
GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct)); _temp819[ 0]=({ struct Cyc_Toc_NullCheck_struct
_temp820; _temp820.tag= Cyc_Toc_NullCheck; _temp820.f1=( void*) t2; _temp820;});
_temp819;})); _temp818->tl= convs; _temp818;});} goto _LL785; _LL789: goto
_LL785; _LL791: convs=({ struct Cyc_List_List* _temp821=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp821->hd=( void*)(( void*)({
struct Cyc_Toc_TagPtr_struct* _temp822=( struct Cyc_Toc_TagPtr_struct*)
GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct)); _temp822[ 0]=({ struct Cyc_Toc_TagPtr_struct
_temp823; _temp823.tag= Cyc_Toc_TagPtr; _temp823.f1=( void*) Cyc_Toc_typ_to_c(
t1); _temp823.f2= _temp810; _temp823;}); _temp822;})); _temp821->tl= convs;
_temp821;}); goto _LL785; _LL793: convs=({ struct Cyc_List_List* _temp824=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp824->hd=(
void*)(( void*)({ struct Cyc_Toc_UntagPtr_struct* _temp825=( struct Cyc_Toc_UntagPtr_struct*)
GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct)); _temp825[ 0]=({ struct Cyc_Toc_UntagPtr_struct
_temp826; _temp826.tag= Cyc_Toc_UntagPtr; _temp826.f1=( void*) Cyc_Toc_typ_to_c(
t1); _temp826.f2= _temp814; _temp826.f3= n2; _temp826;}); _temp825;})); _temp824->tl=
convs; _temp824;}); goto _LL785; _LL785:;} return convs;} _LL774: return 0;
_LL770:;}} static char _temp829[ 7u]="_throw"; static struct _tagged_string Cyc_Toc__throw_string=(
struct _tagged_string){ _temp829, _temp829, _temp829 + 7u}; static struct
_tuple0 Cyc_Toc__throw_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__throw_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__throw_var,
0),({ struct Cyc_List_List* _temp830=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp830->hd=( void*) e; _temp830->tl= 0; _temp830;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp831=( char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp832; _temp832.curr= _temp831; _temp832.base= _temp831;
_temp832.last_plus_one= _temp831 + 44; _temp832;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple0* a= Cyc_Toc_temp_var();
struct _tuple0* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp833= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp839; struct Cyc_Absyn_Conref* _temp841; struct Cyc_Absyn_Tqual
_temp843; struct Cyc_Absyn_Conref* _temp845; void* _temp847; void* _temp849;
_LL835: if(( unsigned int) _temp833 > 4u?*(( int*) _temp833) == Cyc_Absyn_PointerType:
0){ _LL840: _temp839=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp833)->f1; _LL850: _temp849=( void*) _temp839.elt_typ; goto _LL848; _LL848:
_temp847=( void*) _temp839.rgn_typ; goto _LL846; _LL846: _temp845=( struct Cyc_Absyn_Conref*)
_temp839.nullable; goto _LL844; _LL844: _temp843=( struct Cyc_Absyn_Tqual)
_temp839.tq; goto _LL842; _LL842: _temp841=( struct Cyc_Absyn_Conref*) _temp839.bounds;
goto _LL836;} else{ goto _LL837;} _LL837: goto _LL838; _LL836: elt_typ= _temp849;{
struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp849, Cyc_Toc_mt_tq);
strt_typ= Cyc_Absyn_strct( n); goto _LL834;} _LL838: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp851=( char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp852;
_temp852.curr= _temp851; _temp852.base= _temp851; _temp852.last_plus_one=
_temp851 + 28; _temp852;})); _LL834:;} if( toplevel){ int is_string= 0;{ void*
_temp853=( void*) e->r; void* _temp859; struct _tagged_string _temp861; _LL855:
if(*(( int*) _temp853) == Cyc_Absyn_Const_e){ _LL860: _temp859=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp853)->f1; if(( unsigned int) _temp859 > 1u?*((
int*) _temp859) == Cyc_Absyn_String_c: 0){ _LL862: _temp861=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp859)->f1; goto _LL856;}
else{ goto _LL857;}} else{ goto _LL857;} _LL857: goto _LL858; _LL856: is_string=
1; goto _LL854; _LL858: goto _LL854; _LL854:;}{ struct Cyc_Absyn_Exp* xexp; if(
is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp866=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp866[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp867; _temp867.tag= Cyc_Absyn_ArrayType; _temp867.f1=(
void*) elt_typ; _temp867.f2= Cyc_Toc_mt_tq; _temp867.f3=( struct Cyc_Absyn_Exp*)
sz; _temp867;}); _temp866;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp863=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp863->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp864=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp864[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp865; _temp865.tag= Cyc_Absyn_Var_d;
_temp865.f1= vd; _temp865;}); _temp864;}), 0); _temp863->tl= Cyc_Toc_result_decls;
_temp863;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp868=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp868->hd=( void*)({ struct _tuple4* _temp873=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp873->f1= 0; _temp873->f2=
xexp; _temp873;}); _temp868->tl=({ struct Cyc_List_List* _temp869=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp869->hd=( void*)({ struct
_tuple4* _temp872=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp872->f1= 0; _temp872->f2= xexp; _temp872;}); _temp869->tl=({ struct Cyc_List_List*
_temp870=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp870->hd=( void*)({ struct _tuple4* _temp871=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp871->f1= 0; _temp871->f2= xplussz; _temp871;});
_temp870->tl= 0; _temp870;}); _temp869;}); _temp868;}), 0); return urm_exp;}}}
else{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0);
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
b, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_signed_int_exp( szvalue, 0), 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( b, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_var_exp(
a, 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( b, 0), Cyc_Toc_curr_sp, 0), Cyc_Absyn_var_exp( a, 0), 0), s,
0); s= Cyc_Absyn_declare_stmt( b, strt_typ, 0, s, 0); s= Cyc_Absyn_declare_stmt(
a, Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq),( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, e, 0), s, 0); return Cyc_Absyn_stmt_exp( s, 0);}} static struct Cyc_Absyn_Exp*
Cyc_Toc_untag_ptr_conv( int toplevel, void* t1, struct Cyc_Absyn_Exp* sz, int
nullable, struct Cyc_Absyn_Exp* e){ if( toplevel){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp874=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp875; _temp875.curr= _temp874; _temp875.base= _temp874; _temp875.last_plus_one=
_temp874 + 45; _temp875;}));}{ struct _tuple0* x= Cyc_Toc_temp_var(); int
szvalue=( int) Cyc_Evexp_eval_const_uint_exp( sz); struct Cyc_Absyn_Exp* xcurr=
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xcurr, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_add_exp( xcurr, Cyc_Absyn_signed_int_exp( szvalue, 0), 0); struct
Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_gt_exp( e1, e2, 0); struct Cyc_Absyn_Exp*
e4= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( xcurr, Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), e3, 0); struct Cyc_Absyn_Exp* e5= ! nullable? e4:
Cyc_Absyn_and_exp( Cyc_Absyn_neq_exp( xcurr, Cyc_Absyn_uint_exp( 0, 0), 0), e4,
0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_ifthenelse_stmt( e5, Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt(
0), 0), s, 0); s= Cyc_Absyn_declare_stmt( x, t1,( struct Cyc_Absyn_Exp*) e, s, 0);
return Cyc_Absyn_stmt_exp( s, 0);}} static struct Cyc_Absyn_Exp* Cyc_Toc_convert_exp(
int toplevel, void* t1, void* t2, struct Cyc_Absyn_Exp* e){{ struct Cyc_List_List*
cs= Cyc_Toc_conversion( t1, t2); for( 0; cs != 0; cs=({ struct Cyc_List_List*
_temp876= cs; if( _temp876 == 0){ _throw( Null_Exception);} _temp876->tl;})){
void* _temp878=( void*)({ struct Cyc_List_List* _temp877= cs; if( _temp877 == 0){
_throw( Null_Exception);} _temp877->hd;}); void* _temp886; struct Cyc_Absyn_Exp*
_temp888; void* _temp890; int _temp892; struct Cyc_Absyn_Exp* _temp894; void*
_temp896; _LL880: if(*(( int*) _temp878) == Cyc_Toc_NullCheck){ _LL887: _temp886=(
void*)(( struct Cyc_Toc_NullCheck_struct*) _temp878)->f1; goto _LL881;} else{
goto _LL882;} _LL882: if(*(( int*) _temp878) == Cyc_Toc_TagPtr){ _LL891:
_temp890=( void*)(( struct Cyc_Toc_TagPtr_struct*) _temp878)->f1; goto _LL889;
_LL889: _temp888=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_TagPtr_struct*)
_temp878)->f2; goto _LL883;} else{ goto _LL884;} _LL884: if(*(( int*) _temp878)
== Cyc_Toc_UntagPtr){ _LL897: _temp896=( void*)(( struct Cyc_Toc_UntagPtr_struct*)
_temp878)->f1; goto _LL895; _LL895: _temp894=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Toc_UntagPtr_struct*) _temp878)->f2; goto _LL893; _LL893: _temp892=( int)((
struct Cyc_Toc_UntagPtr_struct*) _temp878)->f3; goto _LL885;} else{ goto _LL879;}
_LL881: e= Cyc_Toc_null_check_conv( toplevel, _temp886, e); goto _LL879; _LL883:
e= Cyc_Toc_tag_ptr_conv( toplevel, _temp890, _temp888, e); goto _LL879; _LL885:
e= Cyc_Toc_untag_ptr_conv( toplevel, _temp896, _temp894, _temp892, e); goto
_LL879; _LL879:;}} return e;} struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab;
struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt* fallthru_info; struct
Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct Cyc_Toc_Env* Cyc_Toc_env_t;
static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({ struct Cyc_Toc_Env*
_temp898=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct Cyc_Toc_Env));
_temp898->break_lab= 0; _temp898->continue_lab= 0; _temp898->fallthru_info= 0;
_temp898->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp898->toplevel= 1;
_temp898;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp899=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp899->break_lab= e->break_lab; _temp899->continue_lab=
e->continue_lab; _temp899->fallthru_info= e->fallthru_info; _temp899->varmap= e->varmap;
_temp899->toplevel= e->toplevel; _temp899;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp900=(* x).f1; struct
Cyc_List_List* _temp906; _LL902: if(( unsigned int) _temp900 > 1u?*(( int*)
_temp900) == Cyc_Absyn_Rel_n: 0){ _LL907: _temp906=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp900)->f1; goto _LL903;} else{ goto _LL904;}
_LL904: goto _LL905; _LL903:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp908=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp908[ 0]=({ struct Cyc_Core_Impossible_struct _temp909; _temp909.tag= Cyc_Core_Impossible;
_temp909.f1=({ struct _tagged_string _temp910= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp910.last_plus_one - _temp910.curr,
_temp910.curr);}); _temp909;}); _temp908;})); _LL905: goto _LL901; _LL901:;}{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)(
ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=
0; ans->continue_lab= 0; return ans;} struct _tuple8{ struct _tagged_string* f1;
struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env*
Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_string*
break_l, struct _tagged_string* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
fallthru_vars= 0; for( 0; fallthru_binders != 0; fallthru_binders=({ struct Cyc_List_List*
_temp911= fallthru_binders; if( _temp911 == 0){ _throw( Null_Exception);}
_temp911->tl;})){ fallthru_vars=({ struct Cyc_List_List* _temp912=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp912->hd=( void*)(( struct Cyc_Absyn_Vardecl*)({
struct Cyc_List_List* _temp913= fallthru_binders; if( _temp913 == 0){ _throw(
Null_Exception);} _temp913->hd;}))->name; _temp912->tl= fallthru_vars; _temp912;});}
fallthru_vars=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fallthru_vars);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp914=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp914->v=( void*) break_l; _temp914;}); ans->fallthru_info=({
struct Cyc_Core_Opt* _temp915=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp915->v=( void*)({ struct _tuple8* _temp916=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp916->f1= fallthru_l; _temp916->f2=
fallthru_vars; _temp916->f3= next_case_env->varmap; _temp916;}); _temp915;});
return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct
Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab=({ struct Cyc_Core_Opt* _temp917=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp917->v=( void*) break_l; _temp917;});
ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env* e, struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp918=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp918->v=( void*)({ struct _tuple8* _temp919=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp919->f1= next_l; _temp919->f2= 0; _temp919->f3=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp919;}); _temp918;}); return ans;} struct _tuple9{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
struct Cyc_List_List* f4; char f5; int f6; } ; struct _tuple10{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp920=( char*)"expecting a literal format string";
struct _tagged_string _temp921; _temp921.curr= _temp920; _temp921.base= _temp920;
_temp921.last_plus_one= _temp920 + 34; _temp921;}));}{ struct _tagged_string
fmt_str;{ void* _temp923=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp922= args; if( _temp922 == 0){ _throw( Null_Exception);} _temp922->hd;}))->r;
void* _temp929; struct _tagged_string _temp931; _LL925: if(*(( int*) _temp923)
== Cyc_Absyn_Const_e){ _LL930: _temp929=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp923)->f1; if(( unsigned int) _temp929 > 1u?*(( int*) _temp929) == Cyc_Absyn_String_c:
0){ _LL932: _temp931=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp929)->f1; goto _LL926;} else{ goto _LL927;}} else{ goto _LL927;} _LL927:
goto _LL928; _LL926: fmt_str= _temp931; goto _LL924; _LL928: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp933=( char*)"expecting a literal format string"; struct _tagged_string
_temp934; _temp934.curr= _temp933; _temp934.base= _temp933; _temp934.last_plus_one=
_temp933 + 34; _temp934;})); _LL924:;}{ int len=( int)({ struct _tagged_string
_temp1041= fmt_str;( unsigned int)( _temp1041.last_plus_one - _temp1041.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(({ struct _tagged_string _temp935=
fmt_str; char* _temp937= _temp935.curr + i; if( _temp935.base == 0? 1:( _temp937
< _temp935.base? 1: _temp937 >= _temp935.last_plus_one)){ _throw( Null_Exception);}*
_temp937;}) =='%'){ i ++; if( i < len?({ struct _tagged_string _temp938= fmt_str;
char* _temp940= _temp938.curr + i; if( _temp938.base == 0? 1:( _temp940 <
_temp938.base? 1: _temp940 >= _temp938.last_plus_one)){ _throw( Null_Exception);}*
_temp940;}) =='s': 0){ break;}}} if( i == len){ if( aopt != 0){ args=({ struct
Cyc_List_List* _temp941=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp941->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp942=
aopt; if( _temp942 == 0){ _throw( Null_Exception);} _temp942->v;})); _temp941->tl=
args; _temp941;});} return Cyc_Absyn_primop_exp( p, args, 0);} args=({ struct
Cyc_List_List* _temp943= args; if( _temp943 == 0){ _throw( Null_Exception);}
_temp943->tl;}); typs=({ struct Cyc_List_List* _temp944= typs; if( _temp944 == 0){
_throw( Null_Exception);} _temp944->tl;});{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ char c=({ struct _tagged_string _temp1031= fmt_str; char*
_temp1033= _temp1031.curr + i; if( _temp1031.base == 0? 1:( _temp1033 <
_temp1031.base? 1: _temp1033 >= _temp1031.last_plus_one)){ _throw(
Null_Exception);}* _temp1033;}); rev_fmt=({ struct Cyc_List_List* _temp945=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp945->hd=(
void*)(( int) c); _temp945->tl= rev_fmt; _temp945;}); if( c !='%'){ continue;}{
struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification( fmt_str, i
+ 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp946=( char*)"bad format string"; struct
_tagged_string _temp947; _temp947.curr= _temp946; _temp947.base= _temp946;
_temp947.last_plus_one= _temp946 + 18; _temp947;}));}{ struct _tuple9 _temp951;
int _temp952; char _temp954; struct Cyc_List_List* _temp956; struct Cyc_List_List*
_temp958; struct Cyc_List_List* _temp960; struct Cyc_List_List* _temp962; struct
_tuple9* _temp949=( struct _tuple9*)({ struct Cyc_Core_Opt* _temp948= x; if(
_temp948 == 0){ _throw( Null_Exception);} _temp948->v;}); _temp951=* _temp949;
_LL963: _temp962= _temp951.f1; goto _LL961; _LL961: _temp960= _temp951.f2; goto
_LL959; _LL959: _temp958= _temp951.f3; goto _LL957; _LL957: _temp956= _temp951.f4;
goto _LL955; _LL955: _temp954= _temp951.f5; goto _LL953; _LL953: _temp952=
_temp951.f6; goto _LL950; _LL950: i= _temp952 - 1; if( _temp954 !='s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp964=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp964->hd=( void*) _temp962; _temp964->tl=({ struct Cyc_List_List* _temp965=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp965->hd=(
void*) _temp960; _temp965->tl=({ struct Cyc_List_List* _temp966=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp966->hd=( void*) _temp958;
_temp966->tl=({ struct Cyc_List_List* _temp967=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp967->hd=( void*) _temp956;
_temp967->tl= 0; _temp967;}); _temp966;}); _temp965;}); _temp964;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp968=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp968->hd=( void*)(( int) _temp954); _temp968->tl=
rev_fmt; _temp968;});{ struct Cyc_List_List* _temp969= _temp960; struct Cyc_List_List
_temp976; struct Cyc_List_List* _temp977; int _temp979; _LL971: if( _temp969 ==
0){ goto _LL973;} else{ _temp976=* _temp969; _LL980: _temp979=( int) _temp976.hd;
goto _LL978; _LL978: _temp977=( struct Cyc_List_List*) _temp976.tl; if( _temp977
== 0){ goto _LL975;} else{ goto _LL973;}} _LL975: if( _temp979 =='*'){ goto
_LL972;} else{ goto _LL973;} _LL973: goto _LL974; _LL972: { struct _tuple0* temp=
Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp981=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp981->hd=( void*)({ struct
_tuple10* _temp982=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp982->f1= temp; _temp982->f2=( void*)({ struct Cyc_List_List* _temp984= typs;
if( _temp984 == 0){ _throw( Null_Exception);} _temp984->hd;}); _temp982->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp983= args; if( _temp983 == 0){
_throw( Null_Exception);} _temp983->hd;}); _temp982;}); _temp981->tl= rev_temps;
_temp981;}); rev_result=({ struct Cyc_List_List* _temp985=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp985->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp985->tl= rev_result; _temp985;}); args=({ struct Cyc_List_List*
_temp986= args; if( _temp986 == 0){ _throw( Null_Exception);} _temp986->tl;});
typs=({ struct Cyc_List_List* _temp987= typs; if( _temp987 == 0){ _throw(
Null_Exception);} _temp987->tl;}); goto _LL970;} _LL974: goto _LL970; _LL970:;}{
struct Cyc_List_List* _temp988= _temp958; struct Cyc_List_List _temp995; struct
Cyc_List_List* _temp996; struct Cyc_List_List _temp998; struct Cyc_List_List*
_temp999; int _temp1001; int _temp1003; _LL990: if( _temp988 == 0){ goto _LL992;}
else{ _temp995=* _temp988; _LL1004: _temp1003=( int) _temp995.hd; goto _LL997;
_LL997: _temp996=( struct Cyc_List_List*) _temp995.tl; if( _temp996 == 0){ goto
_LL992;} else{ _temp998=* _temp996; _LL1002: _temp1001=( int) _temp998.hd; goto
_LL1000; _LL1000: _temp999=( struct Cyc_List_List*) _temp998.tl; if( _temp999 ==
0){ goto _LL994;} else{ goto _LL992;}}} _LL994: if( _temp1003 =='.'? _temp1001
=='*': 0){ goto _LL991;} else{ goto _LL992;} _LL992: goto _LL993; _LL991: {
struct _tuple0* temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List*
_temp1005=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1005->hd=( void*)({ struct _tuple10* _temp1006=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1006->f1= temp; _temp1006->f2=( void*)({
struct Cyc_List_List* _temp1008= typs; if( _temp1008 == 0){ _throw(
Null_Exception);} _temp1008->hd;}); _temp1006->f3=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1007= args; if( _temp1007 == 0){ _throw(
Null_Exception);} _temp1007->hd;}); _temp1006;}); _temp1005->tl= rev_temps;
_temp1005;}); rev_result=({ struct Cyc_List_List* _temp1009=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1009->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp1009->tl= rev_result; _temp1009;}); args=({ struct Cyc_List_List*
_temp1010= args; if( _temp1010 == 0){ _throw( Null_Exception);} _temp1010->tl;});
typs=({ struct Cyc_List_List* _temp1011= typs; if( _temp1011 == 0){ _throw(
Null_Exception);} _temp1011->tl;}); goto _LL989;} _LL993: goto _LL989; _LL989:;}
if( _temp954 !='%'){ if( args == 0? 1: typs == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1012= Cyc_Absynpp_prim2string(
p); xprintf("missing arg to %.*s", _temp1012.last_plus_one - _temp1012.curr,
_temp1012.curr);}));}{ struct _tuple0* temp= Cyc_Toc_temp_var(); rev_temps=({
struct Cyc_List_List* _temp1013=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1013->hd=( void*)({ struct _tuple10* _temp1014=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1014->f1= temp;
_temp1014->f2=( void*)({ struct Cyc_List_List* _temp1016= typs; if( _temp1016 ==
0){ _throw( Null_Exception);} _temp1016->hd;}); _temp1014->f3=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1015= args; if( _temp1015 == 0){ _throw(
Null_Exception);} _temp1015->hd;}); _temp1014;}); _temp1013->tl= rev_temps;
_temp1013;}); rev_result=({ struct Cyc_List_List* _temp1017=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1017->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp1017->tl= rev_result; _temp1017;}); args=({ struct Cyc_List_List*
_temp1018= args; if( _temp1018 == 0){ _throw( Null_Exception);} _temp1018->tl;});
typs=({ struct Cyc_List_List* _temp1019= typs; if( _temp1019 == 0){ _throw(
Null_Exception);} _temp1019->tl;});}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp1020=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1020->hd=( void*)(( int)'.'); _temp1020->tl= rev_fmt; _temp1020;}); rev_fmt=({
struct Cyc_List_List* _temp1021=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1021->hd=( void*)(( int)'*'); _temp1021->tl=
rev_fmt; _temp1021;}); rev_fmt=({ struct Cyc_List_List* _temp1022=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1022->hd=( void*)(( int)'s');
_temp1022->tl= rev_fmt; _temp1022;});{ struct _tuple0* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp1023=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1023->hd=( void*)({ struct _tuple10*
_temp1024=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1024->f1=
temp; _temp1024->f2=( void*)({ struct Cyc_List_List* _temp1026= typs; if(
_temp1026 == 0){ _throw( Null_Exception);} _temp1026->hd;}); _temp1024->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1025= args; if( _temp1025 ==
0){ _throw( Null_Exception);} _temp1025->hd;}); _temp1024;}); _temp1023->tl=
rev_temps; _temp1023;});{ struct Cyc_Absyn_Exp* size_exp= Cyc_Absyn_subtract_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp,
0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0),
0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List* _temp1027=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1027->hd=(
void*) size_exp; _temp1027->tl= rev_result; _temp1027;}); rev_result=({ struct
Cyc_List_List* _temp1028=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1028->hd=( void*) cstring_exp; _temp1028->tl= rev_result; _temp1028;});
args=({ struct Cyc_List_List* _temp1029= args; if( _temp1029 == 0){ _throw(
Null_Exception);} _temp1029->tl;}); typs=({ struct Cyc_List_List* _temp1030=
typs; if( _temp1030 == 0){ _throw( Null_Exception);} _temp1030->tl;});}}}}}}{
struct Cyc_List_List* es=({ struct Cyc_List_List* _temp1040=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1040->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp1040->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_result); _temp1040;}); if( aopt != 0){ es=({ struct Cyc_List_List*
_temp1034=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1034->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp1035=
aopt; if( _temp1035 == 0){ _throw( Null_Exception);} _temp1035->v;})); _temp1034->tl=
es; _temp1034;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p, es, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps != 0;
rev_temps=({ struct Cyc_List_List* _temp1036= rev_temps; if( _temp1036 == 0){
_throw( Null_Exception);} _temp1036->tl;})){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)({ struct Cyc_List_List* _temp1037= rev_temps; if( _temp1037 ==
0){ _throw( Null_Exception);} _temp1037->hd;}))).f1,(*(( struct _tuple10*)({
struct Cyc_List_List* _temp1038= rev_temps; if( _temp1038 == 0){ _throw(
Null_Exception);} _temp1038->hd;}))).f2,( struct Cyc_Absyn_Exp*)(*(( struct
_tuple10*)({ struct Cyc_List_List* _temp1039= rev_temps; if( _temp1039 == 0){
_throw( Null_Exception);} _temp1039->hd;}))).f3, s, 0);} return Cyc_Absyn_stmt_exp(
s, 0);}}}}}} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1042=( char*)"Missing type in primop "; struct
_tagged_string _temp1043; _temp1043.curr= _temp1042; _temp1043.base= _temp1042;
_temp1043.last_plus_one= _temp1042 + 24; _temp1043;}));} return Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp1044= e->topt; if( _temp1044 == 0){ _throw(
Null_Exception);} _temp1044->v;}));} static void* Cyc_Toc_get_cyc_typ( struct
Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1045=( char*)"Missing type in primop ";
struct _tagged_string _temp1046; _temp1046.curr= _temp1045; _temp1046.base=
_temp1045; _temp1046.last_plus_one= _temp1045 + 24; _temp1046;}));} return( void*)({
struct Cyc_Core_Opt* _temp1047= e->topt; if( _temp1047 == 0){ _throw(
Null_Exception);} _temp1047->v;});} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1048=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1048->f1= Cyc_Toc_mt_tq; _temp1048->f2=
Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1049= e->topt; if(
_temp1049 == 0){ _throw( Null_Exception);} _temp1049->v;})); _temp1048;});}
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e);
static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp1050=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1050[ 0]=({
struct _tuple4 _temp1051; _temp1051.f1= 0; _temp1051.f2= e; _temp1051;});
_temp1050;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
se= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic( se): Cyc_Toc_malloc_ptr(
se), 0);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1052= rgnopt; if( _temp1052 == 0){ _throw( Null_Exception);} _temp1052;});
Cyc_Toc_exp_to_c( nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, se), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles0); for( 0; dles != 0; dles=({ struct Cyc_List_List*
_temp1053= dles; if( _temp1053 == 0){ _throw( Null_Exception);} _temp1053->tl;})){
struct _tuple4 _temp1057; struct Cyc_Absyn_Exp* _temp1058; struct Cyc_List_List*
_temp1060; struct _tuple4* _temp1055=( struct _tuple4*)({ struct Cyc_List_List*
_temp1054= dles; if( _temp1054 == 0){ _throw( Null_Exception);} _temp1054->hd;});
_temp1057=* _temp1055; _LL1061: _temp1060= _temp1057.f1; goto _LL1059; _LL1059:
_temp1058= _temp1057.f2; goto _LL1056; _LL1056: { struct Cyc_Absyn_Exp* e_index;
if( _temp1060 == 0){ e_index= Cyc_Absyn_signed_int_exp(( count --), 0);} else{
if(({ struct Cyc_List_List* _temp1062= _temp1060; if( _temp1062 == 0){ _throw(
Null_Exception);} _temp1062->tl;}) != 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1063=( char*)"multiple designators in array";
struct _tagged_string _temp1064; _temp1064.curr= _temp1063; _temp1064.base=
_temp1063; _temp1064.last_plus_one= _temp1063 + 30; _temp1064;}));}{ void* d=(
void*)({ struct Cyc_List_List* _temp1077= _temp1060; if( _temp1077 == 0){ _throw(
Null_Exception);} _temp1077->hd;}); void* _temp1065= d; struct Cyc_Absyn_Exp*
_temp1071; struct _tagged_string* _temp1073; _LL1067: if(*(( int*) _temp1065) ==
Cyc_Absyn_ArrayElement){ _LL1072: _temp1071=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayElement_struct*) _temp1065)->f1; goto _LL1068;} else{ goto
_LL1069;} _LL1069: if(*(( int*) _temp1065) == Cyc_Absyn_FieldName){ _LL1074:
_temp1073=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1065)->f1; goto _LL1070;} else{ goto _LL1066;} _LL1068: Cyc_Toc_exp_to_c(
nv, _temp1071); e_index= _temp1071; goto _LL1066; _LL1070: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1075=( char*)"field name designators in array"; struct _tagged_string
_temp1076; _temp1076.curr= _temp1075; _temp1076.base= _temp1075; _temp1076.last_plus_one=
_temp1075 + 32; _temp1076;})); goto _LL1066; _LL1066:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp1078=( void*)
_temp1058->r; struct Cyc_List_List* _temp1088; struct Cyc_Absyn_Exp* _temp1090;
struct Cyc_Absyn_Exp* _temp1092; struct Cyc_Absyn_Vardecl* _temp1094; struct Cyc_List_List*
_temp1096; void* _temp1098; _LL1080: if(*(( int*) _temp1078) == Cyc_Absyn_Array_e){
_LL1089: _temp1088=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1078)->f1; goto _LL1081;} else{ goto _LL1082;} _LL1082: if(*(( int*)
_temp1078) == Cyc_Absyn_Comprehension_e){ _LL1095: _temp1094=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1078)->f1; goto _LL1093; _LL1093:
_temp1092=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1078)->f2; goto _LL1091; _LL1091: _temp1090=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1078)->f3; goto _LL1083;} else{
goto _LL1084;} _LL1084: if(*(( int*) _temp1078) == Cyc_Absyn_AnonStruct_e){
_LL1099: _temp1098=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1078)->f1;
goto _LL1097; _LL1097: _temp1096=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1078)->f2; goto _LL1085;} else{ goto _LL1086;} _LL1086: goto _LL1087;
_LL1081: s= Cyc_Toc_init_array( nv, lval, _temp1088, s); goto _LL1079; _LL1083:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1094, _temp1092, _temp1090, s, 0);
goto _LL1079; _LL1085: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1098,
_temp1096, s); goto _LL1079; _LL1087: Cyc_Toc_exp_to_c( nv, _temp1058); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp1058, 0),
s, 0); goto _LL1079; _LL1079:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* x1= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp( x1,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1122=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1122[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1123; _temp1123.tag= Cyc_Absyn_Local_b;
_temp1123.f1= vd; _temp1123;}); _temp1122;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_var_exp( max, 0), 0);
struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( x1, 0), 0);
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( x1,
0), 0); struct Cyc_Absyn_Stmt* body;{ void* _temp1100=( void*) e2->r; struct Cyc_List_List*
_temp1110; struct Cyc_Absyn_Exp* _temp1112; struct Cyc_Absyn_Exp* _temp1114;
struct Cyc_Absyn_Vardecl* _temp1116; struct Cyc_List_List* _temp1118; void*
_temp1120; _LL1102: if(*(( int*) _temp1100) == Cyc_Absyn_Array_e){ _LL1111:
_temp1110=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1100)->f1;
goto _LL1103;} else{ goto _LL1104;} _LL1104: if(*(( int*) _temp1100) == Cyc_Absyn_Comprehension_e){
_LL1117: _temp1116=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1100)->f1; goto _LL1115; _LL1115: _temp1114=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1100)->f2; goto _LL1113; _LL1113:
_temp1112=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1100)->f3; goto _LL1105;} else{ goto _LL1106;} _LL1106: if(*(( int*)
_temp1100) == Cyc_Absyn_AnonStruct_e){ _LL1121: _temp1120=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1100)->f1; goto _LL1119; _LL1119: _temp1118=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1100)->f2; goto _LL1107;} else{ goto
_LL1108;} _LL1108: goto _LL1109; _LL1103: body= Cyc_Toc_init_array( nv2, lval,
_temp1110, Cyc_Absyn_skip_stmt( 0)); goto _LL1101; _LL1105: body= Cyc_Toc_init_comprehension(
nv2, lval, _temp1116, _temp1114, _temp1112, Cyc_Absyn_skip_stmt( 0), 0); goto
_LL1101; _LL1107: body= Cyc_Toc_init_anon_struct( nv, lval, _temp1120, _temp1118,
Cyc_Absyn_skip_stmt( 0)); goto _LL1101; _LL1109: Cyc_Toc_exp_to_c( nv2, e2);
body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL1101; _LL1101:;}{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt(
max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( x1,
Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( dles); for( 0; es != 0; es=({ struct Cyc_List_List* _temp1124= es;
if( _temp1124 == 0){ _throw( Null_Exception);} _temp1124->tl;})){ struct _tuple4
_temp1128; struct Cyc_Absyn_Exp* _temp1129; struct Cyc_List_List* _temp1131;
struct _tuple4* _temp1126=( struct _tuple4*)({ struct Cyc_List_List* _temp1125=
es; if( _temp1125 == 0){ _throw( Null_Exception);} _temp1125->hd;}); _temp1128=*
_temp1126; _LL1132: _temp1131= _temp1128.f1; goto _LL1130; _LL1130: _temp1129=
_temp1128.f2; goto _LL1127; _LL1127: if( _temp1131 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1133=(
char*)"empty designator list"; struct _tagged_string _temp1134; _temp1134.curr=
_temp1133; _temp1134.base= _temp1133; _temp1134.last_plus_one= _temp1133 + 22;
_temp1134;}));} if(({ struct Cyc_List_List* _temp1135= _temp1131; if( _temp1135
== 0){ _throw( Null_Exception);} _temp1135->tl;}) != 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1136=(
char*)"too many designators in anonymous struct"; struct _tagged_string
_temp1137; _temp1137.curr= _temp1136; _temp1137.base= _temp1136; _temp1137.last_plus_one=
_temp1136 + 41; _temp1137;}));}{ void* _temp1139=( void*)({ struct Cyc_List_List*
_temp1138= _temp1131; if( _temp1138 == 0){ _throw( Null_Exception);} _temp1138->hd;});
struct _tagged_string* _temp1145; _LL1141: if(*(( int*) _temp1139) == Cyc_Absyn_FieldName){
_LL1146: _temp1145=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1139)->f1; goto _LL1142;} else{ goto _LL1143;} _LL1143: goto _LL1144;
_LL1142: { struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs,
_temp1145, 0);{ void* _temp1147=( void*) _temp1129->r; struct Cyc_List_List*
_temp1157; struct Cyc_Absyn_Exp* _temp1159; struct Cyc_Absyn_Exp* _temp1161;
struct Cyc_Absyn_Vardecl* _temp1163; struct Cyc_List_List* _temp1165; void*
_temp1167; _LL1149: if(*(( int*) _temp1147) == Cyc_Absyn_Array_e){ _LL1158:
_temp1157=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1147)->f1;
goto _LL1150;} else{ goto _LL1151;} _LL1151: if(*(( int*) _temp1147) == Cyc_Absyn_Comprehension_e){
_LL1164: _temp1163=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1147)->f1; goto _LL1162; _LL1162: _temp1161=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1147)->f2; goto _LL1160; _LL1160:
_temp1159=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1147)->f3; goto _LL1152;} else{ goto _LL1153;} _LL1153: if(*(( int*)
_temp1147) == Cyc_Absyn_AnonStruct_e){ _LL1168: _temp1167=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1147)->f1; goto _LL1166; _LL1166: _temp1165=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1147)->f2; goto _LL1154;} else{ goto
_LL1155;} _LL1155: goto _LL1156; _LL1150: s= Cyc_Toc_init_array( nv, lval,
_temp1157, s); goto _LL1148; _LL1152: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1163, _temp1161, _temp1159, s, 0); goto _LL1148; _LL1154: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp1167, _temp1165, s); goto _LL1148; _LL1156: Cyc_Toc_exp_to_c( nv,
_temp1129); if( Cyc_Toc_is_poly_field( struct_type, _temp1145)){ _temp1129= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1129, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp1129, 0), 0), s, 0); goto _LL1148; _LL1148:;}
goto _LL1140;} _LL1144:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1169=( char*)"array designator in struct";
struct _tagged_string _temp1170; _temp1170.curr= _temp1169; _temp1170.base=
_temp1169; _temp1170.last_plus_one= _temp1169 + 27; _temp1170;}))); _LL1140:;}}}
return s;} static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env*
nv, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
Cyc_List_List* tqs=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, es); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(* lval_f)( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1; es=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( es);{ int i=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es); for( 0; es != 0; es=({ struct Cyc_List_List*
_temp1171= es; if( _temp1171 == 0){ _throw( Null_Exception);} _temp1171->tl;}),
-- i){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1191= es; if( _temp1191 == 0){ _throw( Null_Exception);} _temp1191->hd;});
struct Cyc_Absyn_Exp* lval= lval_f( xexp,({ struct _tagged_string* _temp1190=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1190[ 0]= xprintf("f%d", i); _temp1190;}), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)({ struct Cyc_Core_Opt* _temp1172= e->topt; if( _temp1172 == 0){ _throw(
Null_Exception);} _temp1172->v;})): 0;{ void* _temp1173=( void*) e->r; struct
Cyc_List_List* _temp1181; struct Cyc_Absyn_Exp* _temp1183; struct Cyc_Absyn_Exp*
_temp1185; struct Cyc_Absyn_Vardecl* _temp1187; _LL1175: if(*(( int*) _temp1173)
== Cyc_Absyn_Array_e){ _LL1182: _temp1181=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1173)->f1; goto _LL1176;} else{ goto _LL1177;} _LL1177: if(*(( int*)
_temp1173) == Cyc_Absyn_Comprehension_e){ _LL1188: _temp1187=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1173)->f1; goto _LL1186; _LL1186:
_temp1185=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1173)->f2; goto _LL1184; _LL1184: _temp1183=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1173)->f3; goto _LL1178;} else{
goto _LL1179;} _LL1179: goto _LL1180; _LL1176: s= Cyc_Toc_init_array( nv, lval,
_temp1181, s); goto _LL1174; _LL1178: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1187, _temp1185, _temp1183, s, 0); goto _LL1174; _LL1180: Cyc_Toc_exp_to_c(
nv, e); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f(
xexp,({ struct _tagged_string* _temp1189=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1189[ 0]= xprintf("f%d", i); _temp1189;}),
0), e, 0), 0), s, 0); goto _LL1174; _LL1174:;}}} return Cyc_Toc_make_struct( nv,
x, Cyc_Absyn_strct( n), s, pointer, rgnopt, is_atomic);} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_struct( struct Cyc_Toc_Env* nv, void* struct_type, int pointer,
struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* dles, struct _tuple0* tdn){
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(*
lval_f)( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;{
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( dles); for( 0; es != 0; es=({ struct Cyc_List_List* _temp1192= es;
if( _temp1192 == 0){ _throw( Null_Exception);} _temp1192->tl;})){ struct _tuple4
_temp1196; struct Cyc_Absyn_Exp* _temp1197; struct Cyc_List_List* _temp1199;
struct _tuple4* _temp1194=( struct _tuple4*)({ struct Cyc_List_List* _temp1193=
es; if( _temp1193 == 0){ _throw( Null_Exception);} _temp1193->hd;}); _temp1196=*
_temp1194; _LL1200: _temp1199= _temp1196.f1; goto _LL1198; _LL1198: _temp1197=
_temp1196.f2; goto _LL1195; _LL1195: is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)({ struct Cyc_Core_Opt* _temp1201= _temp1197->topt; if( _temp1201 == 0){
_throw( Null_Exception);} _temp1201->v;})): 0; if( _temp1199 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1202=( char*)"empty designator list"; struct _tagged_string _temp1203;
_temp1203.curr= _temp1202; _temp1203.base= _temp1202; _temp1203.last_plus_one=
_temp1202 + 22; _temp1203;}));} if(({ struct Cyc_List_List* _temp1204= _temp1199;
if( _temp1204 == 0){ _throw( Null_Exception);} _temp1204->tl;}) != 0){ struct
_tuple0* y= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* yexp= Cyc_Absyn_var_exp( y,
0); for( 0; _temp1199 != 0; _temp1199=({ struct Cyc_List_List* _temp1205=
_temp1199; if( _temp1205 == 0){ _throw( Null_Exception);} _temp1205->tl;})){
void* _temp1207=( void*)({ struct Cyc_List_List* _temp1206= _temp1199; if(
_temp1206 == 0){ _throw( Null_Exception);} _temp1206->hd;}); struct
_tagged_string* _temp1213; _LL1209: if(*(( int*) _temp1207) == Cyc_Absyn_FieldName){
_LL1214: _temp1213=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1207)->f1; goto _LL1210;} else{ goto _LL1211;} _LL1211: goto _LL1212;
_LL1210: if( Cyc_Toc_is_poly_field( struct_type, _temp1213)){ yexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), yexp, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval_f( xexp, _temp1213, 0), yexp, 0), 0), s, 0); goto
_LL1208; _LL1212:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1215=( char*)"array designator in struct";
struct _tagged_string _temp1216; _temp1216.curr= _temp1215; _temp1216.base=
_temp1215; _temp1216.last_plus_one= _temp1215 + 27; _temp1216;}))); _LL1208:;}
Cyc_Toc_exp_to_c( nv, _temp1197); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
yexp, _temp1197, 0), 0), s, 0);} else{ void* _temp1218=( void*)({ struct Cyc_List_List*
_temp1217= _temp1199; if( _temp1217 == 0){ _throw( Null_Exception);} _temp1217->hd;});
struct _tagged_string* _temp1224; _LL1220: if(*(( int*) _temp1218) == Cyc_Absyn_FieldName){
_LL1225: _temp1224=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1218)->f1; goto _LL1221;} else{ goto _LL1222;} _LL1222: goto _LL1223;
_LL1221: { struct Cyc_Absyn_Exp* lval= lval_f( xexp, _temp1224, 0);{ void*
_temp1226=( void*) _temp1197->r; struct Cyc_List_List* _temp1236; struct Cyc_Absyn_Exp*
_temp1238; struct Cyc_Absyn_Exp* _temp1240; struct Cyc_Absyn_Vardecl* _temp1242;
struct Cyc_List_List* _temp1244; void* _temp1246; _LL1228: if(*(( int*)
_temp1226) == Cyc_Absyn_Array_e){ _LL1237: _temp1236=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1226)->f1; goto _LL1229;} else{ goto
_LL1230;} _LL1230: if(*(( int*) _temp1226) == Cyc_Absyn_Comprehension_e){
_LL1243: _temp1242=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1226)->f1; goto _LL1241; _LL1241: _temp1240=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1226)->f2; goto _LL1239; _LL1239:
_temp1238=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1226)->f3; goto _LL1231;} else{ goto _LL1232;} _LL1232: if(*(( int*)
_temp1226) == Cyc_Absyn_AnonStruct_e){ _LL1247: _temp1246=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1226)->f1; goto _LL1245; _LL1245: _temp1244=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1226)->f2; goto _LL1233;} else{ goto
_LL1234;} _LL1234: goto _LL1235; _LL1229: s= Cyc_Toc_init_array( nv, lval,
_temp1236, s); goto _LL1227; _LL1231: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1242, _temp1240, _temp1238, s, 0); goto _LL1227; _LL1233: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp1246, _temp1244, s); goto _LL1227; _LL1235: Cyc_Toc_exp_to_c( nv,
_temp1197); if( Cyc_Toc_is_poly_field( struct_type, _temp1224)){ _temp1197= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1197, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp1197, 0), 0), s, 0); goto _LL1227; _LL1227:;}
goto _LL1219;} _LL1223:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1248=( char*)"array designator in struct";
struct _tagged_string _temp1249; _temp1249.curr= _temp1248; _temp1249.base=
_temp1248; _temp1249.last_plus_one= _temp1248 + 27; _temp1249;}))); _LL1219:;}}}
return Cyc_Toc_make_struct( nv, x, Cyc_Absyn_strctq( tdn), s, pointer, rgnopt,
is_atomic);} struct _tuple11{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2;
} ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp*
el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2,
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp*
e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Increment_e_struct* _temp1250=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp1250[ 0]=({
struct Cyc_Absyn_Increment_e_struct _temp1251; _temp1251.tag= Cyc_Absyn_Increment_e;
_temp1251.f1= e1; _temp1251.f2=( void*) incr; _temp1251;}); _temp1250;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp1252=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp1262; struct Cyc_Absyn_Exp*
_temp1264; void* _temp1266; struct _tagged_string* _temp1268; struct Cyc_Absyn_Exp*
_temp1270; _LL1254: if(*(( int*) _temp1252) == Cyc_Absyn_StmtExp_e){ _LL1263:
_temp1262=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1252)->f1; goto _LL1255;} else{ goto _LL1256;} _LL1256: if(*(( int*)
_temp1252) == Cyc_Absyn_Cast_e){ _LL1267: _temp1266=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1252)->f1; goto _LL1265; _LL1265: _temp1264=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Cast_e_struct*) _temp1252)->f2; goto _LL1257;} else{ goto
_LL1258;} _LL1258: if(*(( int*) _temp1252) == Cyc_Absyn_StructMember_e){ _LL1271:
_temp1270=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1252)->f1; goto _LL1269; _LL1269: _temp1268=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1252)->f2; goto _LL1259;} else{
goto _LL1260;} _LL1260: goto _LL1261; _LL1255:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1262, fs, f, f_env);
goto _LL1253; _LL1257:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1264, fs, f, f_env); goto _LL1253; _LL1259:( void*)( e1->r=( void*)(( void*)
_temp1270->r));(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
e1,({ struct Cyc_List_List* _temp1272=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1272->hd=( void*) _temp1268; _temp1272->tl=
fs; _temp1272;}), f, f_env); goto _LL1253; _LL1261: if( ! Cyc_Absyn_is_lvalue(
e1)){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1273= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1273.last_plus_one - _temp1273.curr, _temp1273.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp1274= fs; if( _temp1274 == 0){ _throw( Null_Exception);} _temp1274->tl;})){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_string*)({ struct
Cyc_List_List* _temp1275= fs; if( _temp1275 == 0){ _throw( Null_Exception);}
_temp1275->hd;}), e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy,
f_env))->r)); goto _LL1253;} _LL1253:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp1276=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1286; struct Cyc_Absyn_Stmt* _temp1288; struct Cyc_Absyn_Decl*
_temp1290; struct Cyc_Absyn_Stmt* _temp1292; struct Cyc_Absyn_Stmt* _temp1294;
_LL1278: if(( unsigned int) _temp1276 > 1u?*(( int*) _temp1276) == Cyc_Absyn_Exp_s:
0){ _LL1287: _temp1286=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1276)->f1; goto _LL1279;} else{ goto _LL1280;} _LL1280: if(( unsigned int)
_temp1276 > 1u?*(( int*) _temp1276) == Cyc_Absyn_Decl_s: 0){ _LL1291: _temp1290=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1276)->f1; goto
_LL1289; _LL1289: _temp1288=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1276)->f2; goto _LL1281;} else{ goto _LL1282;} _LL1282: if(( unsigned int)
_temp1276 > 1u?*(( int*) _temp1276) == Cyc_Absyn_Seq_s: 0){ _LL1295: _temp1294=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1276)->f1; goto
_LL1293; _LL1293: _temp1292=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1276)->f2; goto _LL1283;} else{ goto _LL1284;} _LL1284: goto _LL1285;
_LL1279:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1286, fs, f, f_env); goto _LL1277; _LL1281:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1288, fs, f, f_env);
goto _LL1277; _LL1283:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1292, fs, f, f_env); goto _LL1277; _LL1285:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1296= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1296.last_plus_one - _temp1296.curr,
_temp1296.curr);})); goto _LL1277; _LL1277:;} static struct Cyc_Absyn_Exp* Cyc_Toc_known_size_subscript(
struct Cyc_Absyn_Exp* arr, struct Cyc_Absyn_Exp* ind, int possibly_null, struct
Cyc_Absyn_Exp* esz, void* elt_typ, struct Cyc_Absyn_Tqual tq, int take_address){
unsigned int sz= Cyc_Evexp_eval_const_uint_exp( esz); struct _tuple0* a= Cyc_Toc_temp_var();
struct _tuple0* i= Cyc_Toc_temp_var(); void* ta= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
elt_typ), tq); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte,
Cyc_Absyn_var_exp( i, 0), Cyc_Absyn_uint_exp( sz, 0), 0); if( possibly_null){ e3=
Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_new_exp(( void*) e3->r, 0), 0);}{ struct Cyc_Absyn_Exp* e4=
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_ifthenelse_stmt(
e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0), 0); if( take_address){ e5=
Cyc_Absyn_address_exp( e5, 0);}{ struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt(
e5, 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) ind, s3,
0); return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta,( struct Cyc_Absyn_Exp*)
arr, s1, 0), 0);}}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Stmt* s); struct _tuple12{ struct Cyc_Absyn_Structfield* f1;
struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* r=( void*) e->r; if( e->topt == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1297=
Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s", _temp1297.last_plus_one
- _temp1297.curr, _temp1297.curr);})); return;}{ void* old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1999= e->topt; if( _temp1999 == 0){ _throw( Null_Exception);}
_temp1999->v;}); void* _temp1298= r; void* _temp1374; void* _temp1376; void*
_temp1378; struct _tuple0* _temp1380; struct _tuple0* _temp1382; struct Cyc_List_List*
_temp1384; void* _temp1386; void* _temp1388; struct Cyc_Absyn_Exp* _temp1390;
struct Cyc_Absyn_Exp* _temp1392; struct Cyc_Core_Opt* _temp1394; struct Cyc_Absyn_Exp*
_temp1396; struct Cyc_Absyn_Exp* _temp1398; struct Cyc_Absyn_Exp* _temp1400;
struct Cyc_Absyn_Exp* _temp1402; struct Cyc_Absyn_Exp* _temp1404; struct Cyc_Absyn_Exp*
_temp1406; struct Cyc_List_List* _temp1408; struct Cyc_Absyn_Exp* _temp1410;
struct Cyc_List_List* _temp1412; struct Cyc_Absyn_Exp* _temp1414; struct Cyc_Absyn_Exp*
_temp1416; struct Cyc_Absyn_Exp* _temp1418; struct Cyc_List_List* _temp1420;
struct Cyc_Absyn_Exp* _temp1422; struct Cyc_Absyn_Exp* _temp1424; struct Cyc_Absyn_Exp**
_temp1426; void* _temp1427; void** _temp1429; struct Cyc_Absyn_Exp* _temp1430;
struct Cyc_Absyn_Exp* _temp1432; struct Cyc_Absyn_Exp* _temp1434; void*
_temp1436; struct Cyc_Absyn_Exp* _temp1438; struct Cyc_Absyn_Exp* _temp1440;
struct _tagged_string* _temp1442; struct Cyc_Absyn_Exp* _temp1444; struct
_tagged_string* _temp1446; struct Cyc_Absyn_Exp* _temp1448; struct Cyc_Absyn_Exp*
_temp1450; struct Cyc_Absyn_Exp* _temp1452; struct Cyc_List_List* _temp1454;
struct Cyc_List_List* _temp1456; struct _tuple1* _temp1458; struct Cyc_List_List*
_temp1460; struct Cyc_Absyn_Exp* _temp1462; struct Cyc_Absyn_Exp* _temp1464;
struct Cyc_Absyn_Vardecl* _temp1466; struct Cyc_Absyn_Structdecl* _temp1468;
struct Cyc_List_List* _temp1470; struct Cyc_Core_Opt* _temp1472; struct _tuple0*
_temp1474; struct Cyc_List_List* _temp1476; void* _temp1478; struct Cyc_Absyn_Tunionfield*
_temp1480; struct Cyc_Absyn_Tuniondecl* _temp1482; struct Cyc_List_List*
_temp1484; struct Cyc_Core_Opt* _temp1486; struct Cyc_Core_Opt* _temp1488;
struct Cyc_Absyn_Tunionfield* _temp1490; struct Cyc_Absyn_Tuniondecl* _temp1492;
struct Cyc_List_List* _temp1494; struct Cyc_Core_Opt* _temp1496; struct Cyc_Core_Opt*
_temp1498; struct Cyc_Absyn_Enumfield* _temp1500; struct Cyc_Absyn_Enumdecl*
_temp1502; struct _tuple0* _temp1504; void* _temp1506; struct Cyc_Absyn_Exp*
_temp1508; struct Cyc_Absyn_Stmt* _temp1510; struct Cyc_List_List* _temp1512;
struct Cyc_Core_Opt* _temp1514; struct Cyc_Absyn_Fndecl* _temp1516; struct Cyc_Absyn_Exp*
_temp1518; _LL1300: if(*(( int*) _temp1298) == Cyc_Absyn_Const_e){ _LL1375:
_temp1374=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1298)->f1; if(
_temp1374 ==( void*) Cyc_Absyn_Null_c){ goto _LL1301;} else{ goto _LL1302;}}
else{ goto _LL1302;} _LL1302: if(*(( int*) _temp1298) == Cyc_Absyn_Const_e){
_LL1377: _temp1376=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1298)->f1;
goto _LL1303;} else{ goto _LL1304;} _LL1304: if(*(( int*) _temp1298) == Cyc_Absyn_Var_e){
_LL1381: _temp1380=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1298)->f1; goto _LL1379; _LL1379: _temp1378=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1298)->f2; goto _LL1305;} else{ goto _LL1306;} _LL1306: if(*(( int*)
_temp1298) == Cyc_Absyn_UnknownId_e){ _LL1383: _temp1382=( struct _tuple0*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1298)->f1; goto _LL1307;} else{ goto
_LL1308;} _LL1308: if(*(( int*) _temp1298) == Cyc_Absyn_Primop_e){ _LL1387:
_temp1386=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1298)->f1; goto
_LL1385; _LL1385: _temp1384=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1298)->f2; goto _LL1309;} else{ goto _LL1310;} _LL1310: if(*(( int*)
_temp1298) == Cyc_Absyn_Increment_e){ _LL1391: _temp1390=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp1298)->f1; goto _LL1389; _LL1389:
_temp1388=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1298)->f2; goto
_LL1311;} else{ goto _LL1312;} _LL1312: if(*(( int*) _temp1298) == Cyc_Absyn_AssignOp_e){
_LL1397: _temp1396=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1298)->f1; goto _LL1395; _LL1395: _temp1394=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1298)->f2; goto _LL1393; _LL1393:
_temp1392=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1298)->f3; goto _LL1313;} else{ goto _LL1314;} _LL1314: if(*(( int*)
_temp1298) == Cyc_Absyn_Conditional_e){ _LL1403: _temp1402=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1298)->f1; goto _LL1401; _LL1401:
_temp1400=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1298)->f2; goto _LL1399; _LL1399: _temp1398=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1298)->f3; goto _LL1315;} else{
goto _LL1316;} _LL1316: if(*(( int*) _temp1298) == Cyc_Absyn_SeqExp_e){ _LL1407:
_temp1406=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1298)->f1; goto _LL1405; _LL1405: _temp1404=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1298)->f2; goto _LL1317;} else{ goto
_LL1318;} _LL1318: if(*(( int*) _temp1298) == Cyc_Absyn_UnknownCall_e){ _LL1411:
_temp1410=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1298)->f1; goto _LL1409; _LL1409: _temp1408=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1298)->f2; goto _LL1319;} else{
goto _LL1320;} _LL1320: if(*(( int*) _temp1298) == Cyc_Absyn_FnCall_e){ _LL1415:
_temp1414=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1298)->f1; goto _LL1413; _LL1413: _temp1412=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1298)->f2; goto _LL1321;} else{ goto
_LL1322;} _LL1322: if(*(( int*) _temp1298) == Cyc_Absyn_Throw_e){ _LL1417:
_temp1416=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1298)->f1;
goto _LL1323;} else{ goto _LL1324;} _LL1324: if(*(( int*) _temp1298) == Cyc_Absyn_NoInstantiate_e){
_LL1419: _temp1418=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1298)->f1; goto _LL1325;} else{ goto _LL1326;} _LL1326: if(*(( int*)
_temp1298) == Cyc_Absyn_Instantiate_e){ _LL1423: _temp1422=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1298)->f1; goto _LL1421; _LL1421:
_temp1420=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1298)->f2; goto _LL1327;} else{ goto _LL1328;} _LL1328: if(*(( int*)
_temp1298) == Cyc_Absyn_Cast_e){ _LL1428: _temp1427=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1298)->f1; _temp1429=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1298)->f1;
goto _LL1425; _LL1425: _temp1424=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1298)->f2; _temp1426=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1298)->f2;
goto _LL1329;} else{ goto _LL1330;} _LL1330: if(*(( int*) _temp1298) == Cyc_Absyn_Address_e){
_LL1431: _temp1430=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp1298)->f1; goto _LL1331;} else{ goto _LL1332;} _LL1332: if(*(( int*)
_temp1298) == Cyc_Absyn_New_e){ _LL1435: _temp1434=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1298)->f1; goto _LL1433; _LL1433: _temp1432=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1298)->f2; goto
_LL1333;} else{ goto _LL1334;} _LL1334: if(*(( int*) _temp1298) == Cyc_Absyn_Sizeoftyp_e){
_LL1437: _temp1436=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1298)->f1;
goto _LL1335;} else{ goto _LL1336;} _LL1336: if(*(( int*) _temp1298) == Cyc_Absyn_Sizeofexp_e){
_LL1439: _temp1438=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1298)->f1; goto _LL1337;} else{ goto _LL1338;} _LL1338: if(*(( int*)
_temp1298) == Cyc_Absyn_Deref_e){ _LL1441: _temp1440=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp1298)->f1; goto _LL1339;} else{ goto
_LL1340;} _LL1340: if(*(( int*) _temp1298) == Cyc_Absyn_StructMember_e){ _LL1445:
_temp1444=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1298)->f1; goto _LL1443; _LL1443: _temp1442=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1298)->f2; goto _LL1341;} else{
goto _LL1342;} _LL1342: if(*(( int*) _temp1298) == Cyc_Absyn_StructArrow_e){
_LL1449: _temp1448=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1298)->f1; goto _LL1447; _LL1447: _temp1446=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1298)->f2; goto _LL1343;} else{
goto _LL1344;} _LL1344: if(*(( int*) _temp1298) == Cyc_Absyn_Subscript_e){
_LL1453: _temp1452=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1298)->f1; goto _LL1451; _LL1451: _temp1450=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1298)->f2; goto _LL1345;} else{ goto
_LL1346;} _LL1346: if(*(( int*) _temp1298) == Cyc_Absyn_Tuple_e){ _LL1455:
_temp1454=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1298)->f1;
goto _LL1347;} else{ goto _LL1348;} _LL1348: if(*(( int*) _temp1298) == Cyc_Absyn_CompoundLit_e){
_LL1459: _temp1458=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1298)->f1; goto _LL1457; _LL1457: _temp1456=( struct Cyc_List_List*)((
struct Cyc_Absyn_CompoundLit_e_struct*) _temp1298)->f2; goto _LL1349;} else{
goto _LL1350;} _LL1350: if(*(( int*) _temp1298) == Cyc_Absyn_Array_e){ _LL1461:
_temp1460=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1298)->f1;
goto _LL1351;} else{ goto _LL1352;} _LL1352: if(*(( int*) _temp1298) == Cyc_Absyn_Comprehension_e){
_LL1467: _temp1466=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1298)->f1; goto _LL1465; _LL1465: _temp1464=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1298)->f2; goto _LL1463; _LL1463:
_temp1462=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1298)->f3; goto _LL1353;} else{ goto _LL1354;} _LL1354: if(*(( int*)
_temp1298) == Cyc_Absyn_Struct_e){ _LL1475: _temp1474=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1298)->f1; goto _LL1473; _LL1473:
_temp1472=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1298)->f2;
goto _LL1471; _LL1471: _temp1470=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1298)->f3; goto _LL1469; _LL1469: _temp1468=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1298)->f4; goto _LL1355;} else{ goto
_LL1356;} _LL1356: if(*(( int*) _temp1298) == Cyc_Absyn_AnonStruct_e){ _LL1479:
_temp1478=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1298)->f1; goto
_LL1477; _LL1477: _temp1476=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1298)->f2; goto _LL1357;} else{ goto _LL1358;} _LL1358: if(*(( int*)
_temp1298) == Cyc_Absyn_Tunion_e){ _LL1489: _temp1488=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1298)->f1; goto _LL1487; _LL1487:
_temp1486=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1298)->f2;
goto _LL1485; _LL1485: _temp1484=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1298)->f3; if( _temp1484 == 0){ goto _LL1483;} else{ goto _LL1360;} _LL1483:
_temp1482=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1298)->f4; goto _LL1481; _LL1481: _temp1480=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1298)->f5; goto _LL1359;} else{ goto
_LL1360;} _LL1360: if(*(( int*) _temp1298) == Cyc_Absyn_Tunion_e){ _LL1499:
_temp1498=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1298)->f1;
goto _LL1497; _LL1497: _temp1496=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1298)->f2; goto _LL1495; _LL1495: _temp1494=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1298)->f3; goto _LL1493; _LL1493:
_temp1492=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1298)->f4; goto _LL1491; _LL1491: _temp1490=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1298)->f5; goto _LL1361;} else{ goto
_LL1362;} _LL1362: if(*(( int*) _temp1298) == Cyc_Absyn_Enum_e){ _LL1505:
_temp1504=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1298)->f1;
goto _LL1503; _LL1503: _temp1502=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1298)->f2; goto _LL1501; _LL1501: _temp1500=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1298)->f3; goto _LL1363;} else{ goto
_LL1364;} _LL1364: if(*(( int*) _temp1298) == Cyc_Absyn_Malloc_e){ _LL1509:
_temp1508=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1298)->f1; goto _LL1507; _LL1507: _temp1506=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1298)->f2; goto _LL1365;} else{ goto _LL1366;} _LL1366: if(*(( int*)
_temp1298) == Cyc_Absyn_StmtExp_e){ _LL1511: _temp1510=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1298)->f1; goto _LL1367;} else{ goto
_LL1368;} _LL1368: if(*(( int*) _temp1298) == Cyc_Absyn_UnresolvedMem_e){
_LL1515: _temp1514=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1298)->f1; goto _LL1513; _LL1513: _temp1512=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1298)->f2; goto _LL1369;} else{
goto _LL1370;} _LL1370: if(*(( int*) _temp1298) == Cyc_Absyn_Codegen_e){ _LL1517:
_temp1516=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1298)->f1; goto _LL1371;} else{ goto _LL1372;} _LL1372: if(*(( int*)
_temp1298) == Cyc_Absyn_Fill_e){ _LL1519: _temp1518=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp1298)->f1; goto _LL1373;} else{ goto
_LL1299;} _LL1301:{ void* _temp1520= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1526; struct Cyc_Absyn_Conref* _temp1528; struct Cyc_Absyn_Tqual _temp1530;
struct Cyc_Absyn_Conref* _temp1532; void* _temp1534; void* _temp1536; _LL1522:
if(( unsigned int) _temp1520 > 4u?*(( int*) _temp1520) == Cyc_Absyn_PointerType:
0){ _LL1527: _temp1526=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1520)->f1; _LL1537: _temp1536=( void*) _temp1526.elt_typ; goto _LL1535;
_LL1535: _temp1534=( void*) _temp1526.rgn_typ; goto _LL1533; _LL1533: _temp1532=(
struct Cyc_Absyn_Conref*) _temp1526.nullable; goto _LL1531; _LL1531: _temp1530=(
struct Cyc_Absyn_Tqual) _temp1526.tq; goto _LL1529; _LL1529: _temp1528=( struct
Cyc_Absyn_Conref*) _temp1526.bounds; goto _LL1523;} else{ goto _LL1524;} _LL1524:
goto _LL1525; _LL1523: if(( void*) _temp1528->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1521;}{ void* _temp1538=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1528); struct Cyc_Absyn_Exp* _temp1544; _LL1540: if( _temp1538 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1541;} else{ goto _LL1542;} _LL1542: if((
unsigned int) _temp1538 > 1u?*(( int*) _temp1538) == Cyc_Absyn_Upper_b: 0){
_LL1545: _temp1544=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1538)->f1; goto _LL1543;} else{ goto _LL1539;} _LL1541: { struct _tuple4*
zero=({ struct _tuple4* _temp1549=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1549->f1= 0; _temp1549->f2= Cyc_Absyn_signed_int_exp( 0, 0);
_temp1549;}); struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp( 0,({ struct
Cyc_List_List* _temp1546=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1546->hd=( void*) zero; _temp1546->tl=({ struct Cyc_List_List* _temp1547=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1547->hd=(
void*) zero; _temp1547->tl=({ struct Cyc_List_List* _temp1548=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1548->hd=( void*) zero;
_temp1548->tl= 0; _temp1548;}); _temp1547;}); _temp1546;}), 0); if( nv->toplevel){(
void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct( Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1536), Cyc_Toc_mt_tq)); struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*) um, Cyc_Absyn_exp_stmt(
xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}
goto _LL1539;} _LL1543:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1539; _LL1539:;} goto _LL1521; _LL1525:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1550= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1550.last_plus_one -
_temp1550.curr, _temp1550.curr);})); goto _LL1521; _LL1521:;} goto _LL1299;
_LL1303: goto _LL1299; _LL1305:{ struct _handler_cons _temp1551; _push_handler(&
_temp1551);{ int _temp1553= 0; if( setjmp( _temp1551.handler)){ _temp1553= 1;}
if( ! _temp1553){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1380))->r));; _pop_handler();} else{ void* _temp1552=( void*) _exn_thrown;
void* _temp1555= _temp1552; _LL1557: if( _temp1555 == Cyc_Dict_Absent){ goto
_LL1558;} else{ goto _LL1559;} _LL1559: goto _LL1560; _LL1558:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1561= Cyc_Absynpp_qvar2string(
_temp1380); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1561.last_plus_one
- _temp1561.curr, _temp1561.curr);})); return; _LL1560:( void) _throw( _temp1555);
_LL1556:;}}} goto _LL1299; _LL1307:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1562=( char*)"unknownid"; struct
_tagged_string _temp1563; _temp1563.curr= _temp1562; _temp1563.base= _temp1562;
_temp1563.last_plus_one= _temp1562 + 10; _temp1563;})); goto _LL1299; _LL1309: {
struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1384); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1384);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1384);{ void* _temp1564= _temp1386; _LL1566: if( _temp1564 ==( void*)
Cyc_Absyn_Size){ goto _LL1567;} else{ goto _LL1568;} _LL1568: if( _temp1564 ==(
void*) Cyc_Absyn_Printf){ goto _LL1569;} else{ goto _LL1570;} _LL1570: if(
_temp1564 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1571;} else{ goto _LL1572;}
_LL1572: if( _temp1564 ==( void*) Cyc_Absyn_Fprintf){ goto _LL1573;} else{ goto
_LL1574;} _LL1574: if( _temp1564 ==( void*) Cyc_Absyn_Sscanf){ goto _LL1575;}
else{ goto _LL1576;} _LL1576: if( _temp1564 ==( void*) Cyc_Absyn_Plus){ goto
_LL1577;} else{ goto _LL1578;} _LL1578: if( _temp1564 ==( void*) Cyc_Absyn_Minus){
goto _LL1579;} else{ goto _LL1580;} _LL1580: if( _temp1564 ==( void*) Cyc_Absyn_Eq){
goto _LL1581;} else{ goto _LL1582;} _LL1582: if( _temp1564 ==( void*) Cyc_Absyn_Neq){
goto _LL1583;} else{ goto _LL1584;} _LL1584: if( _temp1564 ==( void*) Cyc_Absyn_Gt){
goto _LL1585;} else{ goto _LL1586;} _LL1586: if( _temp1564 ==( void*) Cyc_Absyn_Gte){
goto _LL1587;} else{ goto _LL1588;} _LL1588: if( _temp1564 ==( void*) Cyc_Absyn_Lt){
goto _LL1589;} else{ goto _LL1590;} _LL1590: if( _temp1564 ==( void*) Cyc_Absyn_Lte){
goto _LL1591;} else{ goto _LL1592;} _LL1592: goto _LL1593; _LL1567: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1627= _temp1384; if(
_temp1627 == 0){ _throw( Null_Exception);} _temp1627->hd;});{ void* _temp1595=(
void*)({ struct Cyc_Core_Opt* _temp1594= arg->topt; if( _temp1594 == 0){ _throw(
Null_Exception);} _temp1594->v;}); struct Cyc_Absyn_Exp* _temp1603; struct Cyc_Absyn_Tqual
_temp1605; void* _temp1607; struct Cyc_Absyn_PtrInfo _temp1609; struct Cyc_Absyn_Conref*
_temp1611; struct Cyc_Absyn_Tqual _temp1613; struct Cyc_Absyn_Conref* _temp1615;
void* _temp1617; void* _temp1619; _LL1597: if(( unsigned int) _temp1595 > 4u?*((
int*) _temp1595) == Cyc_Absyn_ArrayType: 0){ _LL1608: _temp1607=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1595)->f1; goto _LL1606; _LL1606:
_temp1605=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1595)->f2; goto _LL1604; _LL1604: _temp1603=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1595)->f3; goto _LL1598;} else{ goto
_LL1599;} _LL1599: if(( unsigned int) _temp1595 > 4u?*(( int*) _temp1595) == Cyc_Absyn_PointerType:
0){ _LL1610: _temp1609=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1595)->f1; _LL1620: _temp1619=( void*) _temp1609.elt_typ; goto _LL1618;
_LL1618: _temp1617=( void*) _temp1609.rgn_typ; goto _LL1616; _LL1616: _temp1615=(
struct Cyc_Absyn_Conref*) _temp1609.nullable; goto _LL1614; _LL1614: _temp1613=(
struct Cyc_Absyn_Tqual) _temp1609.tq; goto _LL1612; _LL1612: _temp1611=( struct
Cyc_Absyn_Conref*) _temp1609.bounds; goto _LL1600;} else{ goto _LL1601;} _LL1601:
goto _LL1602; _LL1598:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Exp*
_temp1621= _temp1603; if( _temp1621 == 0){ _throw( Null_Exception);} _temp1621->r;})));
goto _LL1596; _LL1600: { struct _tuple0* temp= Cyc_Toc_temp_var(); void*
temp_typ= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1622= arg->topt;
if( _temp1622 == 0){ _throw( Null_Exception);} _temp1622->v;})); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
Cyc_Absyn_subtract_exp( e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt(
e4, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1596;}
_LL1602:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1625= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_List_List*
_temp1623= typs; if( _temp1623 == 0){ _throw( Null_Exception);} _temp1623->hd;}));
struct _tagged_string _temp1626= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1624= arg->topt; if( _temp1624 == 0){ _throw( Null_Exception);} _temp1624->v;}));
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1625.last_plus_one
- _temp1625.curr, _temp1625.curr, _temp1626.last_plus_one - _temp1626.curr,
_temp1626.curr);})); return; _LL1596:;} goto _LL1565;} _LL1569:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1386, 0, _temp1384, typs))->r)); goto
_LL1565; _LL1571:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1386, 0, _temp1384, typs))->r)); goto _LL1565; _LL1573: if( _temp1384 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1628=( char*)"fprintf without arguments"; struct _tagged_string
_temp1629; _temp1629.curr= _temp1628; _temp1629.base= _temp1628; _temp1629.last_plus_one=
_temp1628 + 26; _temp1629;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1386,({ struct Cyc_Core_Opt* _temp1630=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1630->v=( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1631= _temp1384; if( _temp1631 == 0){ _throw(
Null_Exception);} _temp1631->hd;})); _temp1630;}),({ struct Cyc_List_List*
_temp1632= _temp1384; if( _temp1632 == 0){ _throw( Null_Exception);} _temp1632->tl;}),({
struct Cyc_List_List* _temp1633= typs; if( _temp1633 == 0){ _throw(
Null_Exception);} _temp1633->tl;})))->r)); goto _LL1565; _LL1575: if( _temp1384
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1634=( char*)"sscanf without arguments"; struct
_tagged_string _temp1635; _temp1635.curr= _temp1634; _temp1635.base= _temp1634;
_temp1635.last_plus_one= _temp1634 + 25; _temp1635;}));}( void*)((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1636= _temp1384; if( _temp1636 == 0){ _throw(
Null_Exception);} _temp1636->hd;}))->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1637= _temp1384; if( _temp1637 == 0){ _throw( Null_Exception);} _temp1637->hd;}))->r,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1638= _temp1384; if(
_temp1638 == 0){ _throw( Null_Exception);} _temp1638->hd;}))->loc), Cyc_Toc_curr_sp,
0))->r)); goto _LL1565; _LL1577: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1642= _temp1384; if( _temp1642 == 0){ _throw( Null_Exception);} _temp1642->hd;});
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1641=({ struct Cyc_List_List* _temp1640= _temp1384; if( _temp1640 == 0){
_throw( Null_Exception);} _temp1640->tl;}); if( _temp1641 == 0){ _throw(
Null_Exception);} _temp1641->hd;}); struct _tuple0* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1639=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1639->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1639;}),
e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1565; _LL1579: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)({ struct Cyc_List_List* _temp1643= old_typs; if( _temp1643 == 0){ _throw(
Null_Exception);} _temp1643->hd;}))){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1650= _temp1384; if( _temp1650 == 0){ _throw(
Null_Exception);} _temp1650->hd;}); struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1649=({ struct Cyc_List_List* _temp1648= _temp1384;
if( _temp1648 == 0){ _throw( Null_Exception);} _temp1648->tl;}); if( _temp1649
== 0){ _throw( Null_Exception);} _temp1649->hd;}); void* old_typ_e2=( void*)({
struct Cyc_List_List* _temp1647=({ struct Cyc_List_List* _temp1646= old_typs;
if( _temp1646 == 0){ _throw( Null_Exception);} _temp1646->tl;}); if( _temp1647
== 0){ _throw( Null_Exception);} _temp1647->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_List_List* _temp1645= old_typs; if( _temp1645 == 0){ _throw(
Null_Exception);} _temp1645->hd;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1644=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1644->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp1644;}), e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0);
s= Cyc_Absyn_declare_stmt( temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1565;
_LL1581: goto _LL1583; _LL1583: goto _LL1585; _LL1585: goto _LL1587; _LL1587:
goto _LL1589; _LL1589: goto _LL1591; _LL1591: { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1656= _temp1384; if(
_temp1656 == 0){ _throw( Null_Exception);} _temp1656->hd;}); struct Cyc_Absyn_Exp*
e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1655=({ struct Cyc_List_List*
_temp1654= _temp1384; if( _temp1654 == 0){ _throw( Null_Exception);} _temp1654->tl;});
if( _temp1655 == 0){ _throw( Null_Exception);} _temp1655->hd;}); void* t1=( void*)({
struct Cyc_List_List* _temp1653= old_typs; if( _temp1653 == 0){ _throw(
Null_Exception);} _temp1653->hd;}); void* t2=( void*)({ struct Cyc_List_List*
_temp1652=({ struct Cyc_List_List* _temp1651= old_typs; if( _temp1651 == 0){
_throw( Null_Exception);} _temp1651->tl;}); if( _temp1652 == 0){ _throw(
Null_Exception);} _temp1652->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1565;} _LL1593: goto
_LL1565; _LL1565:;} goto _LL1299;} _LL1311: { void* e2_cyc_typ=( void*)({ struct
Cyc_Core_Opt* _temp1657= _temp1390->topt; if( _temp1657 == 0){ _throw(
Null_Exception);} _temp1657->v;}); Cyc_Toc_exp_to_c( nv, _temp1390); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){ if( _temp1388 ==( void*) Cyc_Absyn_PreInc? 1: _temp1388 ==( void*)
Cyc_Absyn_PreDec){ struct _tuple0* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Absyn_star_typ(
Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
temp, 0), 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0), _temp1388, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp, temp_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1390, 0), s, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));} else{ struct _tuple0* temp=
Cyc_Toc_temp_var(); struct _tuple0* temp2= Cyc_Toc_temp_var(); void* temp2_typ=
Cyc_Toc_typ_to_c( e2_cyc_typ); void* temp_typ= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c(
e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp2, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( temp,
0), Cyc_Toc_curr_sp, 0), _temp1388, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp2, temp2_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( temp, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1390, 0), s,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} else{
if( ! Cyc_Absyn_is_lvalue( _temp1390)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1390, 0, Cyc_Toc_incr_lvalue,
_temp1388);( void*)( e->r=( void*)(( void*) _temp1390->r));}} goto _LL1299;}
_LL1313: { int e1_poly= Cyc_Toc_is_poly_project( _temp1396); void* e1_old_typ=(
void*)({ struct Cyc_Core_Opt* _temp1663= _temp1396->topt; if( _temp1663 == 0){
_throw( Null_Exception);} _temp1663->v;}); void* e2_old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1662= _temp1392->topt; if( _temp1662 == 0){ _throw(
Null_Exception);} _temp1662->v;}); Cyc_Toc_exp_to_c( nv, _temp1396); Cyc_Toc_exp_to_c(
nv, _temp1392); if( _temp1394 != 0?( void*)({ struct Cyc_Core_Opt* _temp1658=
_temp1394; if( _temp1658 == 0){ _throw( Null_Exception);} _temp1658->v;}) ==(
void*) Cyc_Absyn_Plus? 1:( void*)({ struct Cyc_Core_Opt* _temp1659= _temp1394;
if( _temp1659 == 0){ _throw( Null_Exception);} _temp1659->v;}) ==( void*) Cyc_Absyn_Minus:
0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){( void*)( _temp1396->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)
_temp1396->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1392->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1392->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1392->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1392->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1396)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1396, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1660=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1660[ 0]=({ struct _tuple11 _temp1661; _temp1661.f1=
_temp1394; _temp1661.f2= _temp1392; _temp1661;}); _temp1660;}));( void*)( e->r=(
void*)(( void*) _temp1396->r));} goto _LL1299;} _LL1315: Cyc_Toc_exp_to_c( nv,
_temp1402); Cyc_Toc_exp_to_c( nv, _temp1400); Cyc_Toc_exp_to_c( nv, _temp1398);
goto _LL1299; _LL1317: Cyc_Toc_exp_to_c( nv, _temp1406); Cyc_Toc_exp_to_c( nv,
_temp1404); goto _LL1299; _LL1319: _temp1414= _temp1410; _temp1412= _temp1408;
goto _LL1321; _LL1321: Cyc_Toc_exp_to_c( nv, _temp1414);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1412); goto _LL1299; _LL1323:
Cyc_Toc_exp_to_c( nv, _temp1416);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1416), 0))->r)); goto
_LL1299; _LL1325: Cyc_Toc_exp_to_c( nv, _temp1418); goto _LL1299; _LL1327: Cyc_Toc_exp_to_c(
nv, _temp1422);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1666= e->topt; if( _temp1666 == 0){ _throw( Null_Exception);} _temp1666->v;}));
for( 0; _temp1420 != 0; _temp1420=({ struct Cyc_List_List* _temp1664= _temp1420;
if( _temp1664 == 0){ _throw( Null_Exception);} _temp1664->tl;})){ void* k= Cyc_Tcutil_typ_kind((
void*)({ struct Cyc_List_List* _temp1665= _temp1420; if( _temp1665 == 0){ _throw(
Null_Exception);} _temp1665->hd;})); if( k !=( void*) Cyc_Absyn_EffKind? k !=(
void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1422, 0))->r)); break;}} goto _LL1299;} _LL1329: { void* old_t2=(
void*)({ struct Cyc_Core_Opt* _temp1667=(* _temp1426)->topt; if( _temp1667 == 0){
_throw( Null_Exception);} _temp1667->v;}); void* new_typ=* _temp1429;* _temp1429=
Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,* _temp1426);* _temp1426= Cyc_Toc_convert_exp(
nv->toplevel, old_t2, new_typ,* _temp1426); goto _LL1299;} _LL1331:{ void*
_temp1668=( void*) _temp1430->r; struct Cyc_Absyn_Structdecl* _temp1676; struct
Cyc_List_List* _temp1678; struct Cyc_Core_Opt* _temp1680; struct _tuple0*
_temp1682; struct Cyc_List_List* _temp1684; _LL1670: if(*(( int*) _temp1668) ==
Cyc_Absyn_Struct_e){ _LL1683: _temp1682=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1668)->f1; goto _LL1681; _LL1681: _temp1680=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1668)->f2; goto _LL1679; _LL1679:
_temp1678=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1668)->f3; goto _LL1677; _LL1677: _temp1676=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1668)->f4; goto _LL1671;} else{ goto
_LL1672;} _LL1672: if(*(( int*) _temp1668) == Cyc_Absyn_Tuple_e){ _LL1685:
_temp1684=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1668)->f1;
goto _LL1673;} else{ goto _LL1674;} _LL1674: goto _LL1675; _LL1671: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string
_temp1686= Cyc_Position_string_of_segment( _temp1430->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1686.last_plus_one - _temp1686.curr, _temp1686.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)({ struct Cyc_Core_Opt*
_temp1687= _temp1430->topt; if( _temp1687 == 0){ _throw( Null_Exception);}
_temp1687->v;}), 1, 0, _temp1678, _temp1682))->r)); goto _LL1669; _LL1673: if(
nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct
_tagged_string _temp1688= Cyc_Position_string_of_segment( _temp1430->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1688.last_plus_one -
_temp1688.curr, _temp1688.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, 0, _temp1684))->r)); goto _LL1669; _LL1675: Cyc_Toc_exp_to_c( nv,
_temp1430); if( ! Cyc_Absyn_is_lvalue( _temp1430)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1430, 0, Cyc_Toc_address_lvalue,
1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1689= e->topt; if( _temp1689 == 0){ _throw(
Null_Exception);} _temp1689->v;})), _temp1430, 0))->r));} goto _LL1669; _LL1669:;}
goto _LL1299; _LL1333: if( nv->toplevel){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ struct _tagged_string _temp1690= Cyc_Position_string_of_segment(
_temp1432->loc); xprintf("%.*s: new at top-level", _temp1690.last_plus_one -
_temp1690.curr, _temp1690.curr);}));}{ void* _temp1691=( void*) _temp1432->r;
struct Cyc_List_List* _temp1705; void* _temp1707; struct _tagged_string
_temp1709; struct Cyc_Absyn_Exp* _temp1711; struct Cyc_Absyn_Exp* _temp1713;
struct Cyc_Absyn_Vardecl* _temp1715; struct Cyc_Absyn_Structdecl* _temp1717;
struct Cyc_List_List* _temp1719; struct Cyc_Core_Opt* _temp1721; struct _tuple0*
_temp1723; struct Cyc_List_List* _temp1725; _LL1693: if(*(( int*) _temp1691) ==
Cyc_Absyn_Array_e){ _LL1706: _temp1705=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1691)->f1; goto _LL1694;} else{ goto _LL1695;} _LL1695: if(*(( int*)
_temp1691) == Cyc_Absyn_Const_e){ _LL1708: _temp1707=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1691)->f1; if(( unsigned int) _temp1707 > 1u?*(( int*) _temp1707) == Cyc_Absyn_String_c:
0){ _LL1710: _temp1709=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1707)->f1; goto _LL1696;} else{ goto _LL1697;}} else{ goto _LL1697;}
_LL1697: if(*(( int*) _temp1691) == Cyc_Absyn_Comprehension_e){ _LL1716:
_temp1715=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1691)->f1; goto _LL1714; _LL1714: _temp1713=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1691)->f2; goto _LL1712; _LL1712:
_temp1711=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1691)->f3; goto _LL1698;} else{ goto _LL1699;} _LL1699: if(*(( int*)
_temp1691) == Cyc_Absyn_Struct_e){ _LL1724: _temp1723=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1691)->f1; goto _LL1722; _LL1722:
_temp1721=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1691)->f2;
goto _LL1720; _LL1720: _temp1719=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1691)->f3; goto _LL1718; _LL1718: _temp1717=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1691)->f4; goto _LL1700;} else{ goto
_LL1701;} _LL1701: if(*(( int*) _temp1691) == Cyc_Absyn_Tuple_e){ _LL1726:
_temp1725=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1691)->f1;
goto _LL1702;} else{ goto _LL1703;} _LL1703: goto _LL1704; _LL1694: { struct
_tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x,
0); struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_array( nv, xexp, _temp1705, Cyc_Absyn_exp_stmt(
xexp, 0)); void* old_elt_typ;{ void* _temp1727= Cyc_Tcutil_compress( old_typ);
struct Cyc_Absyn_PtrInfo _temp1733; struct Cyc_Absyn_Conref* _temp1735; struct
Cyc_Absyn_Tqual _temp1737; struct Cyc_Absyn_Conref* _temp1739; void* _temp1741;
void* _temp1743; _LL1729: if(( unsigned int) _temp1727 > 4u?*(( int*) _temp1727)
== Cyc_Absyn_PointerType: 0){ _LL1734: _temp1733=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1727)->f1; _LL1744: _temp1743=( void*)
_temp1733.elt_typ; goto _LL1742; _LL1742: _temp1741=( void*) _temp1733.rgn_typ;
goto _LL1740; _LL1740: _temp1739=( struct Cyc_Absyn_Conref*) _temp1733.nullable;
goto _LL1738; _LL1738: _temp1737=( struct Cyc_Absyn_Tqual) _temp1733.tq; goto
_LL1736; _LL1736: _temp1735=( struct Cyc_Absyn_Conref*) _temp1733.bounds; goto
_LL1730;} else{ goto _LL1731;} _LL1731: goto _LL1732; _LL1730: old_elt_typ=
_temp1743; goto _LL1728; _LL1732: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1745=( char*)"exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1746; _temp1746.curr= _temp1745; _temp1746.base=
_temp1745; _temp1746.last_plus_one= _temp1745 + 52; _temp1746;})); goto _LL1728;
_LL1728:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1705), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1434 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, e2);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1747= _temp1434; if( _temp1747 == 0){ _throw(
Null_Exception);} _temp1747;}); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp(
r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,(
struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1692;}}} _LL1696: if(
_temp1434 == 0){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1748=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1748->f1= Cyc_Toc_abs_ns; _temp1748->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1748;}), 0),({ struct Cyc_List_List* _temp1749=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1749->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1750=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1750[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1751; _temp1751.tag= Cyc_Absyn_Const_e; _temp1751.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1752=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1752[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1753; _temp1753.tag= Cyc_Absyn_String_c;
_temp1753.f1= _temp1709; _temp1753;}); _temp1752;})); _temp1751;}); _temp1750;}),
0); _temp1749->tl= 0; _temp1749;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1760= _temp1434; if(
_temp1760 == 0){ _throw( Null_Exception);} _temp1760;}); Cyc_Toc_exp_to_c( nv, r);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1754=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1754->f1= Cyc_Toc_abs_ns; _temp1754->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1754;}), 0),({ struct Cyc_List_List* _temp1755=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1755->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1756=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1756[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1757; _temp1757.tag= Cyc_Absyn_Const_e; _temp1757.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1758=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1758[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1759; _temp1759.tag= Cyc_Absyn_String_c;
_temp1759.f1= _temp1709; _temp1759;}); _temp1758;})); _temp1757;}); _temp1756;}),
0); _temp1755->tl= 0; _temp1755;}), 0))->r));} goto _LL1692; _LL1698: { int
is_tagged_ptr= 0;{ void* _temp1761= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1767; struct Cyc_Absyn_Conref* _temp1769; struct Cyc_Absyn_Tqual _temp1771;
struct Cyc_Absyn_Conref* _temp1773; void* _temp1775; void* _temp1777; _LL1763:
if(( unsigned int) _temp1761 > 4u?*(( int*) _temp1761) == Cyc_Absyn_PointerType:
0){ _LL1768: _temp1767=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1761)->f1; _LL1778: _temp1777=( void*) _temp1767.elt_typ; goto _LL1776;
_LL1776: _temp1775=( void*) _temp1767.rgn_typ; goto _LL1774; _LL1774: _temp1773=(
struct Cyc_Absyn_Conref*) _temp1767.nullable; goto _LL1772; _LL1772: _temp1771=(
struct Cyc_Absyn_Tqual) _temp1767.tq; goto _LL1770; _LL1770: _temp1769=( struct
Cyc_Absyn_Conref*) _temp1767.bounds; goto _LL1764;} else{ goto _LL1765;} _LL1765:
goto _LL1766; _LL1764:{ void* _temp1779=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1769))->v; void*
_temp1785; _LL1781: if(( unsigned int) _temp1779 > 1u?*(( int*) _temp1779) ==
Cyc_Absyn_Eq_constr: 0){ _LL1786: _temp1785=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1779)->f1; if( _temp1785 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1782;}
else{ goto _LL1783;}} else{ goto _LL1783;} _LL1783: goto _LL1784; _LL1782:
is_tagged_ptr= 1; goto _LL1780; _LL1784: goto _LL1780; _LL1780:;} goto _LL1762;
_LL1766:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1787=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1788; _temp1788.curr= _temp1787; _temp1788.base=
_temp1787; _temp1788.last_plus_one= _temp1787 + 42; _temp1788;})); goto _LL1762;
_LL1762:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)({ struct Cyc_Core_Opt* _temp1812= _temp1711->topt;
if( _temp1812 == 0){ _throw( Null_Exception);} _temp1812->v;}); void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1713);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1715, Cyc_Absyn_var_exp( max, 0), _temp1711,
Cyc_Absyn_skip_stmt( 0), 1); struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1810=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1810->hd=( void*)({ struct _tuple10* _temp1811=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1811->f1= max; _temp1811->f2= Cyc_Absyn_uint_t;
_temp1811->f3=( struct Cyc_Absyn_Exp*) _temp1713; _temp1811;}); _temp1810->tl= 0;
_temp1810;}); struct Cyc_Absyn_Exp* ai; if( _temp1434 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp1789= _temp1434; if( _temp1789 == 0){ _throw( Null_Exception);}
_temp1789;}); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct
Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1790=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1790->hd=( void*)({ struct _tuple10* _temp1791=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1791->f1= a; _temp1791->f2= ptr_typ;
_temp1791->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1791;}); _temp1790->tl= decls;
_temp1790;}); if( is_tagged_ptr){ struct _tuple0* b= Cyc_Toc_temp_var(); void* t=
Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* binit= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp1794=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1794->hd=( void*)({ struct _tuple4* _temp1799=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1799->f1= 0;
_temp1799->f2= Cyc_Absyn_var_exp( a, 0); _temp1799;}); _temp1794->tl=({ struct
Cyc_List_List* _temp1795=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1795->hd=( void*)({ struct _tuple4* _temp1798=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1798->f1= 0; _temp1798->f2= Cyc_Absyn_var_exp( a,
0); _temp1798;}); _temp1795->tl=({ struct Cyc_List_List* _temp1796=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1796->hd=( void*)({ struct
_tuple4* _temp1797=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1797->f1= 0; _temp1797->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1797;}); _temp1796->tl= 0; _temp1796;});
_temp1795;}); _temp1794;}), 0); decls=({ struct Cyc_List_List* _temp1792=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1792->hd=(
void*)({ struct _tuple10* _temp1793=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1793->f1= b; _temp1793->f2= t; _temp1793->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1793;}); _temp1792->tl= decls; _temp1792;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp1800= ds; if(
_temp1800 == 0){ _throw( Null_Exception);} _temp1800->tl;})){ struct Cyc_Absyn_Exp*
_temp1804; void* _temp1806; struct _tuple0* _temp1808; struct _tuple10 _temp1802=*((
struct _tuple10*)({ struct Cyc_List_List* _temp1801= ds; if( _temp1801 == 0){
_throw( Null_Exception);} _temp1801->hd;})); _LL1809: _temp1808= _temp1802.f1;
goto _LL1807; _LL1807: _temp1806= _temp1802.f2; goto _LL1805; _LL1805: _temp1804=
_temp1802.f3; goto _LL1803; _LL1803: s= Cyc_Absyn_declare_stmt( _temp1808,
_temp1806, _temp1804, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r)); goto _LL1692;}}}} _LL1700:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1813= _temp1432->topt; if( _temp1813 ==
0){ _throw( Null_Exception);} _temp1813->v;}), 1, _temp1434, _temp1719,
_temp1723))->r)); goto _LL1692; _LL1702:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1434, _temp1725))->r)); goto _LL1692; _LL1704: { void* old_elt_typ=(
void*)({ struct Cyc_Core_Opt* _temp1815= _temp1432->topt; if( _temp1815 == 0){
_throw( Null_Exception);} _temp1815->v;}); struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0);
struct Cyc_Absyn_Exp* e_index= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c(
nv, _temp1432); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1432, 0), s, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); struct Cyc_Absyn_Exp*
e1; if( _temp1434 == 0){ e1= Cyc_Toc_malloc_exp( old_elt_typ, e2);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1814=
_temp1434; if( _temp1814 == 0){ _throw( Null_Exception);} _temp1814;}); Cyc_Toc_exp_to_c(
nv, r); e1= Cyc_Toc_rmalloc_exp( r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp(
ptr_typ, e1, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, ptr_typ,( struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1692;}}}
_LL1692:;} goto _LL1299; _LL1335:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1816=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1816[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1817; _temp1817.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1817.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1436);
_temp1817;}); _temp1816;}))); goto _LL1299; _LL1337: Cyc_Toc_exp_to_c( nv,
_temp1438); goto _LL1299; _LL1339: { void* e1_typ= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp1847= _temp1440->topt; if( _temp1847 == 0){ _throw(
Null_Exception);} _temp1847->v;})); Cyc_Toc_exp_to_c( nv, _temp1440);{ void*
_temp1818= e1_typ; struct Cyc_Absyn_PtrInfo _temp1824; struct Cyc_Absyn_Conref*
_temp1826; struct Cyc_Absyn_Tqual _temp1828; struct Cyc_Absyn_Conref* _temp1830;
void* _temp1832; void* _temp1834; _LL1820: if(( unsigned int) _temp1818 > 4u?*((
int*) _temp1818) == Cyc_Absyn_PointerType: 0){ _LL1825: _temp1824=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1818)->f1; _LL1835: _temp1834=( void*)
_temp1824.elt_typ; goto _LL1833; _LL1833: _temp1832=( void*) _temp1824.rgn_typ;
goto _LL1831; _LL1831: _temp1830=( struct Cyc_Absyn_Conref*) _temp1824.nullable;
goto _LL1829; _LL1829: _temp1828=( struct Cyc_Absyn_Tqual) _temp1824.tq; goto
_LL1827; _LL1827: _temp1826=( struct Cyc_Absyn_Conref*) _temp1824.bounds; goto
_LL1821;} else{ goto _LL1822;} _LL1822: goto _LL1823; _LL1821:{ void* _temp1836=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1826); struct
Cyc_Absyn_Exp* _temp1842; _LL1838: if( _temp1836 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1839;} else{ goto _LL1840;} _LL1840: if(( unsigned int) _temp1836 > 1u?*((
int*) _temp1836) == Cyc_Absyn_Upper_b: 0){ _LL1843: _temp1842=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1836)->f1; goto _LL1841;} else{ goto
_LL1837;} _LL1839: { struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array( _temp1834); struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1, _temp1828); void* tx= Cyc_Absyn_strct(
n); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1828); struct Cyc_Absyn_Exp*
test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_or_exp(
Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt(
test_exp, s4, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt(
s3, s5, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt( a, ta2,( struct
Cyc_Absyn_Exp*) a_init, s2, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp1440, s1, 0), 0))->r));
goto _LL1837;} _LL1841: if( Cyc_Toc_is_nullable( e1_typ)){ struct _tuple0* x=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1844= _temp1440->topt; if( _temp1844 == 0){ _throw( Null_Exception);}
_temp1844->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1440, s3, 0), 0))->r));} goto _LL1837; _LL1837:;} goto _LL1819; _LL1823:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1845=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1846; _temp1846.curr= _temp1845; _temp1846.base= _temp1845; _temp1846.last_plus_one=
_temp1845 + 29; _temp1846;})); goto _LL1819; _LL1819:;} goto _LL1299;} _LL1341:
Cyc_Toc_exp_to_c( nv, _temp1444); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({ struct
Cyc_Core_Opt* _temp1848= e->topt; if( _temp1848 == 0){ _throw( Null_Exception);}
_temp1848->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1299;
_LL1343: Cyc_Toc_exp_to_c( nv, _temp1448);{ int is_poly= Cyc_Toc_is_poly_project(
e); if( Cyc_Toc_is_nullable(( void*)({ struct Cyc_Core_Opt* _temp1849= _temp1448->topt;
if( _temp1849 == 0){ _throw( Null_Exception);} _temp1849->v;}))){ struct _tuple0*
x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1850= _temp1448->topt; if( _temp1850 == 0){ _throw( Null_Exception);}
_temp1850->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0),
_temp1446, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1448, s3, 0), 0))->r));} if( is_poly){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1851= e->topt; if( _temp1851 == 0){ _throw(
Null_Exception);} _temp1851->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}
goto _LL1299;} _LL1345: { void* e1_typ= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp1904= _temp1452->topt; if( _temp1904 == 0){ _throw(
Null_Exception);} _temp1904->v;})); Cyc_Toc_exp_to_c( nv, _temp1452); Cyc_Toc_exp_to_c(
nv, _temp1450);( void*)( _temp1452->r=( void*)(( void*)( Cyc_Toc_array_to_ptr(
Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp(( void*) _temp1452->r, 0), 0))->r));{
void* _temp1852= e1_typ; struct Cyc_List_List* _temp1862; struct Cyc_Absyn_Exp*
_temp1864; struct Cyc_Absyn_Tqual _temp1866; void* _temp1868; struct Cyc_Absyn_PtrInfo
_temp1870; struct Cyc_Absyn_Conref* _temp1872; struct Cyc_Absyn_Tqual _temp1874;
struct Cyc_Absyn_Conref* _temp1876; void* _temp1878; void* _temp1880; _LL1854:
if(( unsigned int) _temp1852 > 4u?*(( int*) _temp1852) == Cyc_Absyn_TupleType: 0){
_LL1863: _temp1862=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1852)->f1; goto _LL1855;} else{ goto _LL1856;} _LL1856: if(( unsigned int)
_temp1852 > 4u?*(( int*) _temp1852) == Cyc_Absyn_ArrayType: 0){ _LL1869:
_temp1868=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1852)->f1; goto
_LL1867; _LL1867: _temp1866=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1852)->f2; goto _LL1865; _LL1865: _temp1864=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1852)->f3; goto _LL1857;} else{ goto
_LL1858;} _LL1858: if(( unsigned int) _temp1852 > 4u?*(( int*) _temp1852) == Cyc_Absyn_PointerType:
0){ _LL1871: _temp1870=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1852)->f1; _LL1881: _temp1880=( void*) _temp1870.elt_typ; goto _LL1879;
_LL1879: _temp1878=( void*) _temp1870.rgn_typ; goto _LL1877; _LL1877: _temp1876=(
struct Cyc_Absyn_Conref*) _temp1870.nullable; goto _LL1875; _LL1875: _temp1874=(
struct Cyc_Absyn_Tqual) _temp1870.tq; goto _LL1873; _LL1873: _temp1872=( struct
Cyc_Absyn_Conref*) _temp1870.bounds; goto _LL1859;} else{ goto _LL1860;} _LL1860:
goto _LL1861; _LL1855: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1450) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1452,({ struct _tagged_string* _temp1882=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1882[ 0]= xprintf("f%d",( int)
i); _temp1882;}), 0))->r)); goto _LL1853;} _LL1857: if( _temp1864 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1883=( char*)"exp_to_c: Subscript_e array type bound null"; struct
_tagged_string _temp1884; _temp1884.curr= _temp1883; _temp1884.base= _temp1883;
_temp1884.last_plus_one= _temp1883 + 44; _temp1884;}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_known_size_subscript( _temp1452, _temp1450, 0,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1885= _temp1864; if( _temp1885 == 0){ _throw(
Null_Exception);} _temp1885;}), _temp1868, _temp1866, 0))->r)); goto _LL1853;
_LL1859:{ void* _temp1886=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1872); struct Cyc_Absyn_Exp* _temp1892; _LL1888: if(( unsigned int)
_temp1886 > 1u?*(( int*) _temp1886) == Cyc_Absyn_Upper_b: 0){ _LL1893: _temp1892=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1886)->f1; goto
_LL1889;} else{ goto _LL1890;} _LL1890: if( _temp1886 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1891;} else{ goto _LL1887;} _LL1889: { int possibly_null= 1;{ void*
_temp1894=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1876))->v; int _temp1900; _LL1896: if((
unsigned int) _temp1894 > 1u?*(( int*) _temp1894) == Cyc_Absyn_Eq_constr: 0){
_LL1901: _temp1900=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1894)->f1;
goto _LL1897;} else{ goto _LL1898;} _LL1898: goto _LL1899; _LL1897:
possibly_null= _temp1900; goto _LL1895; _LL1899: possibly_null= 0; goto _LL1895;
_LL1895:;}( void*)( e->r=( void*)(( void*)( Cyc_Toc_known_size_subscript(
_temp1452, _temp1450, possibly_null, _temp1892, _temp1880, _temp1874, 0))->r));
goto _LL1887;} _LL1891: { struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0*
i= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array(
_temp1880); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1,
_temp1874); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1,
_temp1874); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1450, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1452, s1, 0), 0))->r)); goto _LL1887;} _LL1887:;}
goto _LL1853; _LL1861:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1902=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1903; _temp1903.curr= _temp1902; _temp1903.base=
_temp1902; _temp1903.last_plus_one= _temp1902 + 49; _temp1903;})); goto _LL1853;
_LL1853:;} goto _LL1299;} _LL1347: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1454))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1454); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp1454 != 0; _temp1454=({ struct Cyc_List_List* _temp1905=
_temp1454; if( _temp1905 == 0){ _throw( Null_Exception);} _temp1905->tl;}), i ++){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1906=
_temp1454; if( _temp1906 == 0){ _throw( Null_Exception);} _temp1906->hd;}));{
struct Cyc_List_List* des=({ struct Cyc_List_List* _temp1910=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1910->hd=( void*)(( void*)({
struct Cyc_Absyn_FieldName_struct* _temp1911=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp1911[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp1912; _temp1912.tag= Cyc_Absyn_FieldName;
_temp1912.f1=({ struct _tagged_string* _temp1913=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1913[ 0]= xprintf("f%d", i);
_temp1913;}); _temp1912;}); _temp1911;})); _temp1910->tl= 0; _temp1910;}); dles=({
struct Cyc_List_List* _temp1907=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1907->hd=( void*)({ struct _tuple4* _temp1908=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1908->f1=( struct Cyc_List_List*)
des; _temp1908->f2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1909=
_temp1454; if( _temp1909 == 0){ _throw( Null_Exception);} _temp1909->hd;});
_temp1908;}); _temp1907->tl= dles; _temp1907;});}}} dles=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp( 0,
dles, 0), 0))->r));} goto _LL1299; _LL1349:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1914=( char*)"compoundlit";
struct _tagged_string _temp1915; _temp1915.curr= _temp1914; _temp1915.base=
_temp1914; _temp1915.last_plus_one= _temp1914 + 12; _temp1915;})); goto _LL1299;
_LL1351:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1460, 0))->r));{ struct Cyc_List_List* dles= _temp1460; for( 0; dles != 0;
dles=({ struct Cyc_List_List* _temp1916= dles; if( _temp1916 == 0){ _throw(
Null_Exception);} _temp1916->tl;})){ struct _tuple4 _temp1920; struct Cyc_Absyn_Exp*
_temp1921; struct Cyc_List_List* _temp1923; struct _tuple4* _temp1918=( struct
_tuple4*)({ struct Cyc_List_List* _temp1917= dles; if( _temp1917 == 0){ _throw(
Null_Exception);} _temp1917->hd;}); _temp1920=* _temp1918; _LL1924: _temp1923=
_temp1920.f1; goto _LL1922; _LL1922: _temp1921= _temp1920.f2; goto _LL1919;
_LL1919: Cyc_Toc_exp_to_c( nv, _temp1921);}} goto _LL1299; _LL1353: {
unsigned int sz= Cyc_Evexp_eval_const_uint_exp( _temp1464); Cyc_Toc_exp_to_c( nv,
_temp1462);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp1462)){
unsigned int i= 0; for( 0; i < sz; i ++){ es=({ struct Cyc_List_List* _temp1925=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1925->hd=(
void*)({ struct _tuple4* _temp1926=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1926->f1= 0; _temp1926->f2= _temp1462; _temp1926;}); _temp1925->tl=
es; _temp1925;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL1299;}} _LL1355: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1470, _temp1474))->r));}
else{ struct _tuple0* n= _temp1474; struct Cyc_Absyn_Structdecl* sd2=( struct
Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl* _temp1939= _temp1468 == 0?((
struct Cyc_Absyn_Structdecl*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1938= Cyc_Absynpp_qvar2string( _temp1474); xprintf("no structdecl %.*s",
_temp1938.last_plus_one - _temp1938.curr, _temp1938.curr);})): _temp1468; if(
_temp1939 == 0){ _throw( Null_Exception);} _temp1939;}); struct _RegionHandle
_temp1927= _new_region(); struct _RegionHandle* rgn=& _temp1927; _push_region(
rgn);{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, e->loc,
_temp1470,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1937=(( struct
Cyc_Absyn_Structdecl*) sd2)->fields; if( _temp1937 == 0){ _throw( Null_Exception);}
_temp1937->v;})); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp1928=
fields; if( _temp1928 == 0){ _throw( Null_Exception);} _temp1928->tl;})){ struct
_tuple12 _temp1932; struct Cyc_Absyn_Exp* _temp1933; struct Cyc_Absyn_Structfield*
_temp1935; struct _tuple12* _temp1930=( struct _tuple12*)({ struct Cyc_List_List*
_temp1929= fields; if( _temp1929 == 0){ _throw( Null_Exception);} _temp1929->hd;});
_temp1932=* _temp1930; _LL1936: _temp1935= _temp1932.f1; goto _LL1934; _LL1934:
_temp1933= _temp1932.f2; goto _LL1931; _LL1931: { void* fieldtyp=( void*)
_temp1935->type; Cyc_Toc_exp_to_c( nv, _temp1933); if( Cyc_Toc_is_void_star(
fieldtyp)){( void*)( _temp1933->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1933->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0,
_temp1470, 0), 0))->r));}; _pop_region(& _temp1927);} goto _LL1299; _LL1357: {
struct Cyc_List_List* fs;{ void* _temp1940= Cyc_Tcutil_compress( _temp1478);
struct Cyc_List_List* _temp1946; _LL1942: if(( unsigned int) _temp1940 > 4u?*((
int*) _temp1940) == Cyc_Absyn_AnonStructType: 0){ _LL1947: _temp1946=( struct
Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*) _temp1940)->f1; goto
_LL1943;} else{ goto _LL1944;} _LL1944: goto _LL1945; _LL1943: fs= _temp1946;
goto _LL1941; _LL1945: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1948= Cyc_Absynpp_typ2string(
_temp1478); xprintf("anon struct has type %.*s", _temp1948.last_plus_one -
_temp1948.curr, _temp1948.curr);})); goto _LL1941; _LL1941:;}{ struct
_RegionHandle _temp1949= _new_region(); struct _RegionHandle* rgn=& _temp1949;
_push_region( rgn);{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1476, fs); for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp1950= fields; if( _temp1950 == 0){ _throw( Null_Exception);} _temp1950->tl;})){
struct _tuple12 _temp1954; struct Cyc_Absyn_Exp* _temp1955; struct Cyc_Absyn_Structfield*
_temp1957; struct _tuple12* _temp1952=( struct _tuple12*)({ struct Cyc_List_List*
_temp1951= fields; if( _temp1951 == 0){ _throw( Null_Exception);} _temp1951->hd;});
_temp1954=* _temp1952; _LL1958: _temp1957= _temp1954.f1; goto _LL1956; _LL1956:
_temp1955= _temp1954.f2; goto _LL1953; _LL1953: { void* fieldtyp=( void*)
_temp1957->type; Cyc_Toc_exp_to_c( nv, _temp1955); if( Cyc_Toc_is_void_star(
fieldtyp)){( void*)( _temp1955->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1955->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1476, 0))->r));}; _pop_region(&
_temp1949);} goto _LL1299;} _LL1359: { struct _tuple0* qv= _temp1480->name; if(
_temp1482->is_xtunion? 1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp(
qv, 0))->r));} else{ int tag_count= 0; struct Cyc_List_List* fields= _temp1482->fields
== 0? 0:( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1962= _temp1482->fields;
if( _temp1962 == 0){ _throw( Null_Exception);} _temp1962->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1959= fields;
if( _temp1959 == 0){ _throw( Null_Exception);} _temp1959->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1960= fields;
if( _temp1960 == 0){ _throw( Null_Exception);} _temp1960->hd;}))->typs == 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1961= fields; if( _temp1961
== 0){ _throw( Null_Exception);} _temp1961->tl;});}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto _LL1299;}
_LL1361: { struct _tuple0* qv= _temp1490->name; struct Cyc_List_List* tqts=
_temp1490->typs; struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1990=( char*)"_struct"; struct
_tagged_string _temp1991; _temp1991.curr= _temp1990; _temp1991.base= _temp1990;
_temp1991.last_plus_one= _temp1990 + 8; _temp1991;}))); if( nv->toplevel){
struct Cyc_Absyn_Exp* tag_exp; if( _temp1492->is_xtunion){ tag_exp= Cyc_Absyn_var_exp(
qv, 0);} else{ int tag_count= 0; struct Cyc_List_List* fields= _temp1492->fields
== 0? 0:( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1966= _temp1492->fields;
if( _temp1966 == 0){ _throw( Null_Exception);} _temp1966->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1963= fields;
if( _temp1963 == 0){ _throw( Null_Exception);} _temp1963->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1964= fields;
if( _temp1964 == 0){ _throw( Null_Exception);} _temp1964->hd;}))->typs != 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1965= fields; if( _temp1965
== 0){ _throw( Null_Exception);} _temp1965->tl;});} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles= 0; for( 0; _temp1494
!= 0; _temp1494=({ struct Cyc_List_List* _temp1967= _temp1494; if( _temp1967 ==
0){ _throw( Null_Exception);} _temp1967->tl;}), tqts=({ struct Cyc_List_List*
_temp1968= tqts; if( _temp1968 == 0){ _throw( Null_Exception);} _temp1968->tl;})){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1974= _temp1494; if( _temp1974 == 0){ _throw( Null_Exception);} _temp1974->hd;});
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp1973= tqts; if( _temp1973 == 0){ _throw( Null_Exception);} _temp1973->hd;}))).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} dles=({ struct Cyc_List_List* _temp1969=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1969[ 0]=({ struct Cyc_List_List
_temp1970; _temp1970.hd=( void*)({ struct _tuple4* _temp1971=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp1971[ 0]=({ struct _tuple4
_temp1972; _temp1972.f1= 0; _temp1972.f2= cur_e; _temp1972;}); _temp1971;});
_temp1970.tl= dles; _temp1970;}); _temp1969;});} dles=({ struct Cyc_List_List*
_temp1975=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1975[ 0]=({ struct Cyc_List_List _temp1976; _temp1976.hd=( void*)({ struct
_tuple4* _temp1977=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp1977[ 0]=({ struct _tuple4 _temp1978; _temp1978.f1= 0; _temp1978.f2=
tag_exp; _temp1978;}); _temp1977;}); _temp1976.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); _temp1976;}); _temp1975;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1979=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1979->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({
char* _temp1980=( char*)"_struct"; struct _tagged_string _temp1981; _temp1981.curr=
_temp1980; _temp1981.base= _temp1980; _temp1981.last_plus_one= _temp1980 + 8;
_temp1981;})); _temp1979;}), dles, 0))->r));}} else{ struct Cyc_List_List*
rev_assign=({ struct Cyc_List_List* _temp1989=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1989->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( xexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( qv, 0),
0); _temp1989->tl= 0; _temp1989;});{ int i= 1; for( 0; _temp1494 != 0;(
_temp1494=({ struct Cyc_List_List* _temp1982= _temp1494; if( _temp1982 == 0){
_throw( Null_Exception);} _temp1982->tl;}), i ++), tqts=({ struct Cyc_List_List*
_temp1983= tqts; if( _temp1983 == 0){ _throw( Null_Exception);} _temp1983->tl;})){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1987= _temp1494; if( _temp1987 == 0){ _throw( Null_Exception);} _temp1987->hd;});
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp1986= tqts; if( _temp1986 == 0){ _throw( Null_Exception);} _temp1986->hd;}))).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1985=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1985[ 0]= xprintf("f%d", i); _temp1985;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1984=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1984->hd=( void*) a; _temp1984->tl=
rev_assign; _temp1984;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1988=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1988->hd=(
void*) final_stmt; _temp1988->tl= rev_assign; _temp1988;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL1299;} _LL1363: goto _LL1299; _LL1365: { void* t_c= Cyc_Toc_typ_to_c(
_temp1506); if( _temp1508 != 0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1992= _temp1508; if( _temp1992 == 0){ _throw(
Null_Exception);} _temp1992;}); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)((
void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1506, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} goto _LL1299;} _LL1367: Cyc_Toc_stmt_to_c( nv, _temp1510); goto
_LL1299; _LL1369:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1993=( char*)"UnresolvedMem"; struct
_tagged_string _temp1994; _temp1994.curr= _temp1993; _temp1994.base= _temp1993;
_temp1994.last_plus_one= _temp1993 + 14; _temp1994;})); goto _LL1299; _LL1371:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1995=( char*)"codegen"; struct _tagged_string _temp1996; _temp1996.curr=
_temp1995; _temp1996.base= _temp1995; _temp1996.last_plus_one= _temp1995 + 8;
_temp1996;})); goto _LL1299; _LL1373:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1997=( char*)"fill"; struct _tagged_string
_temp1998; _temp1998.curr= _temp1997; _temp1998.base= _temp1997; _temp1998.last_plus_one=
_temp1997 + 5; _temp1998;})); goto _LL1299; _LL1299:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple14{ struct _tuple0* f1; void* f2;
} ; struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, void* t,
struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat* p,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp2000=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp2036; struct Cyc_Absyn_Vardecl _temp2038; struct Cyc_List_List* _temp2039;
struct Cyc_Core_Opt* _temp2041; struct Cyc_Absyn_Exp* _temp2043; void* _temp2045;
struct Cyc_Absyn_Tqual _temp2047; struct _tuple0* _temp2049; void* _temp2051;
struct Cyc_Absyn_Vardecl* _temp2053; int _temp2055; void* _temp2057; char
_temp2059; struct _tagged_string _temp2061; struct Cyc_Absyn_Enumfield*
_temp2063; struct Cyc_Absyn_Enumdecl* _temp2065; struct Cyc_List_List* _temp2067;
struct Cyc_List_List* _temp2069; struct Cyc_Absyn_Tunionfield* _temp2071; struct
Cyc_Absyn_Tuniondecl* _temp2073; struct Cyc_Absyn_Pat* _temp2076; struct Cyc_Absyn_Pat
_temp2078; struct Cyc_Position_Segment* _temp2079; struct Cyc_Core_Opt*
_temp2081; void* _temp2083; struct Cyc_List_List* _temp2085; struct Cyc_List_List*
_temp2087; struct Cyc_Absyn_Tunionfield* _temp2089; struct Cyc_Absyn_Tuniondecl*
_temp2091; struct Cyc_List_List* _temp2093; struct Cyc_List_List* _temp2095;
struct Cyc_Absyn_Tunionfield* _temp2097; struct Cyc_Absyn_Tuniondecl* _temp2099;
struct Cyc_List_List* _temp2101; struct Cyc_List_List* _temp2103; struct Cyc_List_List*
_temp2105; struct Cyc_Core_Opt* _temp2107; struct Cyc_Absyn_Structdecl*
_temp2109; struct Cyc_Absyn_Pat* _temp2111; struct _tuple0* _temp2113; struct
Cyc_List_List* _temp2115; struct Cyc_List_List* _temp2117; struct _tuple0*
_temp2119; struct Cyc_List_List* _temp2121; struct Cyc_List_List* _temp2123;
struct _tuple0* _temp2125; _LL2002: if(( unsigned int) _temp2000 > 2u?*(( int*)
_temp2000) == Cyc_Absyn_Var_p: 0){ _LL2037: _temp2036=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp2000)->f1; _temp2038=* _temp2036; _LL2052:
_temp2051=( void*) _temp2038.sc; goto _LL2050; _LL2050: _temp2049=( struct
_tuple0*) _temp2038.name; goto _LL2048; _LL2048: _temp2047=( struct Cyc_Absyn_Tqual)
_temp2038.tq; goto _LL2046; _LL2046: _temp2045=( void*) _temp2038.type; goto
_LL2044; _LL2044: _temp2043=( struct Cyc_Absyn_Exp*) _temp2038.initializer; goto
_LL2042; _LL2042: _temp2041=( struct Cyc_Core_Opt*) _temp2038.rgn; goto _LL2040;
_LL2040: _temp2039=( struct Cyc_List_List*) _temp2038.attributes; goto _LL2003;}
else{ goto _LL2004;} _LL2004: if( _temp2000 ==( void*) Cyc_Absyn_Wild_p){ goto
_LL2005;} else{ goto _LL2006;} _LL2006: if(( unsigned int) _temp2000 > 2u?*((
int*) _temp2000) == Cyc_Absyn_Reference_p: 0){ _LL2054: _temp2053=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Reference_p_struct*) _temp2000)->f1; goto _LL2007;} else{ goto
_LL2008;} _LL2008: if( _temp2000 ==( void*) Cyc_Absyn_Null_p){ goto _LL2009;}
else{ goto _LL2010;} _LL2010: if(( unsigned int) _temp2000 > 2u?*(( int*)
_temp2000) == Cyc_Absyn_Int_p: 0){ _LL2058: _temp2057=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp2000)->f1; goto _LL2056; _LL2056: _temp2055=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp2000)->f2; goto _LL2011;} else{ goto _LL2012;} _LL2012: if(( unsigned int)
_temp2000 > 2u?*(( int*) _temp2000) == Cyc_Absyn_Char_p: 0){ _LL2060: _temp2059=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp2000)->f1; goto _LL2013;} else{
goto _LL2014;} _LL2014: if(( unsigned int) _temp2000 > 2u?*(( int*) _temp2000)
== Cyc_Absyn_Float_p: 0){ _LL2062: _temp2061=( struct _tagged_string)(( struct
Cyc_Absyn_Float_p_struct*) _temp2000)->f1; goto _LL2015;} else{ goto _LL2016;}
_LL2016: if(( unsigned int) _temp2000 > 2u?*(( int*) _temp2000) == Cyc_Absyn_Enum_p:
0){ _LL2066: _temp2065=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp2000)->f1; goto _LL2064; _LL2064: _temp2063=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp2000)->f2; goto _LL2017;} else{ goto
_LL2018;} _LL2018: if(( unsigned int) _temp2000 > 2u?*(( int*) _temp2000) == Cyc_Absyn_Tunion_p:
0){ _LL2074: _temp2073=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2000)->f1; goto _LL2072; _LL2072: _temp2071=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2000)->f2; goto _LL2070; _LL2070:
_temp2069=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2000)->f3; goto _LL2068; _LL2068: _temp2067=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2000)->f4; if( _temp2067 == 0){ goto
_LL2019;} else{ goto _LL2020;}} else{ goto _LL2020;} _LL2020: if(( unsigned int)
_temp2000 > 2u?*(( int*) _temp2000) == Cyc_Absyn_Pointer_p: 0){ _LL2077:
_temp2076=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2000)->f1; _temp2078=* _temp2076; _LL2084: _temp2083=( void*) _temp2078.r;
if(( unsigned int) _temp2083 > 2u?*(( int*) _temp2083) == Cyc_Absyn_Tunion_p: 0){
_LL2092: _temp2091=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2083)->f1; goto _LL2090; _LL2090: _temp2089=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2083)->f2; goto _LL2088; _LL2088:
_temp2087=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2083)->f3; goto _LL2086; _LL2086: _temp2085=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2083)->f4; goto _LL2082;} else{ goto
_LL2022;} _LL2082: _temp2081=( struct Cyc_Core_Opt*) _temp2078.topt; goto
_LL2080; _LL2080: _temp2079=( struct Cyc_Position_Segment*) _temp2078.loc; goto
_LL2075;} else{ goto _LL2022;} _LL2075: if( _temp2085 != 0){ goto _LL2021;}
else{ goto _LL2022;} _LL2022: if(( unsigned int) _temp2000 > 2u?*(( int*)
_temp2000) == Cyc_Absyn_Tunion_p: 0){ _LL2100: _temp2099=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2000)->f1; goto _LL2098; _LL2098:
_temp2097=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2000)->f2; goto _LL2096; _LL2096: _temp2095=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2000)->f3; goto _LL2094; _LL2094:
_temp2093=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2000)->f4; goto _LL2023;} else{ goto _LL2024;} _LL2024: if(( unsigned int)
_temp2000 > 2u?*(( int*) _temp2000) == Cyc_Absyn_Tuple_p: 0){ _LL2102: _temp2101=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp2000)->f1; goto
_LL2025;} else{ goto _LL2026;} _LL2026: if(( unsigned int) _temp2000 > 2u?*((
int*) _temp2000) == Cyc_Absyn_Struct_p: 0){ _LL2110: _temp2109=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp2000)->f1; goto _LL2108; _LL2108:
_temp2107=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp2000)->f2;
goto _LL2106; _LL2106: _temp2105=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp2000)->f3; goto _LL2104; _LL2104: _temp2103=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp2000)->f4; goto _LL2027;} else{ goto
_LL2028;} _LL2028: if(( unsigned int) _temp2000 > 2u?*(( int*) _temp2000) == Cyc_Absyn_Pointer_p:
0){ _LL2112: _temp2111=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2000)->f1; goto _LL2029;} else{ goto _LL2030;} _LL2030: if(( unsigned int)
_temp2000 > 2u?*(( int*) _temp2000) == Cyc_Absyn_UnknownId_p: 0){ _LL2114:
_temp2113=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2000)->f1;
goto _LL2031;} else{ goto _LL2032;} _LL2032: if(( unsigned int) _temp2000 > 2u?*((
int*) _temp2000) == Cyc_Absyn_UnknownCall_p: 0){ _LL2120: _temp2119=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp2000)->f1; goto _LL2118;
_LL2118: _temp2117=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2000)->f2; goto _LL2116; _LL2116: _temp2115=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp2000)->f3; goto _LL2033;} else{
goto _LL2034;} _LL2034: if(( unsigned int) _temp2000 > 2u?*(( int*) _temp2000)
== Cyc_Absyn_UnknownFields_p: 0){ _LL2126: _temp2125=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp2000)->f1; goto _LL2124; _LL2124:
_temp2123=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2000)->f2; goto _LL2122; _LL2122: _temp2121=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp2000)->f3; goto _LL2035;} else{
goto _LL2001;} _LL2003: nv= Cyc_Toc_add_varmap( nv, _temp2049, r); goto _LL2005;
_LL2005: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL2001; _LL2007: { struct
_tuple0* v= Cyc_Toc_temp_var(); decls=({ struct Cyc_List_List* _temp2127=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2127->hd=(
void*)({ struct _tuple14* _temp2128=( struct _tuple14*) GC_malloc( sizeof(
struct _tuple14)); _temp2128->f1= v; _temp2128->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp2128;}); _temp2127->tl= decls; _temp2127;}); nv= Cyc_Toc_add_varmap(
nv, _temp2053->name, Cyc_Absyn_var_exp( v, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2001;} _LL2009: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2001; _LL2011: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2057, _temp2055, 0), succ_lab, fail_lab); goto _LL2001;
_LL2013: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2059, 0),
succ_lab, fail_lab); goto _LL2001; _LL2015: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2061, 0), succ_lab, fail_lab); goto _LL2001; _LL2017: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2129=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2129[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2130; _temp2130.tag= Cyc_Absyn_Enum_e;
_temp2130.f1= _temp2063->name; _temp2130.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2065; _temp2130.f3=( struct Cyc_Absyn_Enumfield*) _temp2063; _temp2130;});
_temp2129;}), 0), succ_lab, fail_lab); goto _LL2001; _LL2019: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2071->name, 0); if( ! _temp2073->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2001;} _LL2021: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2085);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2089->name,( struct
_tagged_string)({ char* _temp2142=( char*)"_struct"; struct _tagged_string
_temp2143; _temp2143.curr= _temp2142; _temp2143.base= _temp2142; _temp2143.last_plus_one=
_temp2142 + 8; _temp2143;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2085); for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp2131= rps; if(
_temp2131 == 0){ _throw( Null_Exception);} _temp2131->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2138= rps; if(
_temp2138 == 0){ _throw( Null_Exception);} _temp2138->hd;}); struct _tuple0* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp2137= p2->topt;
if( _temp2137 == 0){ _throw( Null_Exception);} _temp2137->v;}); void* t2c= Cyc_Toc_typ_to_c(
t2); struct _tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2132=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2132->hd=( void*)({ struct _tuple14* _temp2133=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2133->f1= v; _temp2133->f2= t2c;
_temp2133;}); _temp2132->tl= decls; _temp2132;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp2136=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2136[ 0]= xprintf("f%d", cnt); _temp2136;}), 0), p2,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp2135=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2135[ 0]= xprintf("f%d", cnt); _temp2135;}), 0), 0),
0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List*
_temp2134=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2134->hd=( void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp2134->tl= ss;
_temp2134;}); succ_lab= lab;}}}}{ struct Cyc_Absyn_Exp* test_exp; if( _temp2091->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2089->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2141= _temp2091->fields; if( _temp2141 == 0){ _throw( Null_Exception);}
_temp2141->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp2139= fs; if(
_temp2139 == 0){ _throw( Null_Exception);} _temp2139->tl;})){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2140= fs; if(
_temp2140 == 0){ _throw( Null_Exception);} _temp2140->hd;}); if( f->typs == 0){
max_tag ++;}}}{ struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t,
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0);
struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2089->name,
0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0):
Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2001;}} _LL2023:
_temp2101= _temp2093; goto _LL2025; _LL2025: { struct Cyc_List_List* ss= 0; int
cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2101);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2101); for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp2144= rps; if(
_temp2144 == 0){ _throw( Null_Exception);} _temp2144->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2151= rps; if(
_temp2151 == 0){ _throw( Null_Exception);} _temp2151->hd;}); struct _tuple0* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp2150= p2->topt;
if( _temp2150 == 0){ _throw( Null_Exception);} _temp2150->v;}); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2145=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2145->hd=( void*)({ struct _tuple14* _temp2146=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2146->f1= v; _temp2146->f2= Cyc_Toc_typ_to_c(
t2); _temp2146;}); _temp2145->tl= decls; _temp2145;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp2149=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2149[ 0]= xprintf("f%d", cnt);
_temp2149;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp2148=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2148[ 0]= xprintf("f%d", cnt); _temp2148;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp2147=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2147->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp2147->tl= ss; _temp2147;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL2001;} _LL2027: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2103); for( 0; dlps != 0; dlps=({ struct Cyc_List_List* _temp2152= dlps;
if( _temp2152 == 0){ _throw( Null_Exception);} _temp2152->tl;})){ struct
_tuple15* tup=( struct _tuple15*)({ struct Cyc_List_List* _temp2166= dlps; if(
_temp2166 == 0){ _throw( Null_Exception);} _temp2166->hd;}); struct Cyc_Absyn_Pat*
p2=(* tup).f2; struct _tagged_string* f;{ void* _temp2154=( void*)({ struct Cyc_List_List*
_temp2153=(* tup).f1; if( _temp2153 == 0){ _throw( Null_Exception);} _temp2153->hd;});
struct _tagged_string* _temp2160; _LL2156: if(*(( int*) _temp2154) == Cyc_Absyn_FieldName){
_LL2161: _temp2160=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp2154)->f1; goto _LL2157;} else{ goto _LL2158;} _LL2158: goto _LL2159;
_LL2157: f= _temp2160; goto _LL2155; _LL2159:( void) _throw(( void*) Cyc_Toc_Match_error);
_LL2155:;}{ struct _tuple0* v= Cyc_Toc_temp_var(); void* t2=( void*)({ struct
Cyc_Core_Opt* _temp2165= p2->topt; if( _temp2165 == 0){ _throw( Null_Exception);}
_temp2165->v;}); void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab=
Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List* _temp2162=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2162->hd=( void*)({ struct
_tuple14* _temp2163=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp2163->f1= v; _temp2163->f2= t2c; _temp2163;}); _temp2162->tl= decls;
_temp2162;});{ struct _tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structmember_exp( path, f, 0), p2, succ_lab, fail_lab, decls);
nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s2= result.f3; struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp( r, f, 0), 0), 0), s2,
0); ss=({ struct Cyc_List_List* _temp2164=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2164->hd=( void*) Cyc_Absyn_label_stmt( lab,
s3, 0); _temp2164->tl= ss; _temp2164;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL2001;} _LL2029: { struct _tuple0* v= Cyc_Toc_temp_var(); void*
t2=( void*)({ struct Cyc_Core_Opt* _temp2169= _temp2111->topt; if( _temp2169 ==
0){ _throw( Null_Exception);} _temp2169->v;}); decls=({ struct Cyc_List_List*
_temp2167=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2167->hd=( void*)({ struct _tuple14* _temp2168=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2168->f1= v; _temp2168->f2= Cyc_Toc_typ_to_c(
t2); _temp2168;}); _temp2167->tl= decls; _temp2167;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path,
0), _temp2111, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0),
s2, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);}
else{ s= s3;} goto _LL2001;}}} _LL2031: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2170=(
char*)"unknownid"; struct _tagged_string _temp2171; _temp2171.curr= _temp2170;
_temp2171.base= _temp2170; _temp2171.last_plus_one= _temp2170 + 10; _temp2171;}));
goto _LL2001; _LL2033: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2172=( char*)"unknowncall";
struct _tagged_string _temp2173; _temp2173.curr= _temp2172; _temp2173.base=
_temp2172; _temp2173.last_plus_one= _temp2172 + 12; _temp2173;})); goto _LL2001;
_LL2035: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2174=( char*)"unknownfields"; struct
_tagged_string _temp2175; _temp2175.curr= _temp2174; _temp2175.base= _temp2174;
_temp2175.last_plus_one= _temp2174 + 14; _temp2175;})); goto _LL2001; _LL2001:;}
return({ struct _tuple13 _temp2176; _temp2176.f1= nv; _temp2176.f2= decls;
_temp2176.f3= s; _temp2176;});} struct _tuple16{ struct _tagged_string* f1;
struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause* f3; } ; static struct
_tuple16* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause* sc){ return({
struct _tuple16* _temp2177=( struct _tuple16*) GC_malloc( sizeof( struct
_tuple16)); _temp2177->f1= Cyc_Toc_fresh_label(); _temp2177->f2= Cyc_Toc_fresh_label();
_temp2177->f3= sc; _temp2177;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* t=( void*)({ struct Cyc_Core_Opt*
_temp2233= e->topt; if( _temp2233 == 0){ _throw( Null_Exception);} _temp2233->v;});
int leave_as_switch;{ void* _temp2178= Cyc_Tcutil_compress( t); void* _temp2184;
void* _temp2186; _LL2180: if(( unsigned int) _temp2178 > 4u?*(( int*) _temp2178)
== Cyc_Absyn_IntType: 0){ _LL2187: _temp2186=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2178)->f1; goto _LL2185; _LL2185: _temp2184=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2178)->f2; goto _LL2181;} else{ goto _LL2182;} _LL2182: goto _LL2183;
_LL2181: leave_as_switch= 1; goto _LL2179; _LL2183: leave_as_switch= 0; goto
_LL2179; _LL2179:;}{ struct Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1=({
struct Cyc_List_List* _temp2188= scs1; if( _temp2188 == 0){ _throw(
Null_Exception);} _temp2188->tl;})){ if(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2190=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp2189=
scs1; if( _temp2189 == 0){ _throw( Null_Exception);} _temp2189->hd;}))->pat_vars;
if( _temp2190 == 0){ _throw( Null_Exception);} _temp2190->v;}) != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* scs1= scs; for( 0; scs1 !=
0; scs1=({ struct Cyc_List_List* _temp2191= scs1; if( _temp2191 == 0){ _throw(
Null_Exception);} _temp2191->tl;})){ struct Cyc_Absyn_Stmt* s=(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2193= scs1; if( _temp2193 == 0){ _throw(
Null_Exception);} _temp2193->hd;}))->body;(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2192= scs1; if( _temp2192 == 0){ _throw(
Null_Exception);} _temp2192->hd;}))->body= Cyc_Absyn_label_stmt( next_l, s, 0);
next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), s);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0);
struct _tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple16*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* lscs2= lscs; for( 0; lscs2 != 0; lscs2=({ struct Cyc_List_List*
_temp2194= lscs2; if( _temp2194 == 0){ _throw( Null_Exception);} _temp2194->tl;})){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2206= lscs2; if( _temp2206 == 0){ _throw( Null_Exception);} _temp2206->hd;}))).f3;
struct _tagged_string* fail_lab=({ struct Cyc_List_List* _temp2203= lscs2; if(
_temp2203 == 0){ _throw( Null_Exception);} _temp2203->tl;}) == 0? end_l:(*((
struct _tuple16*)({ struct Cyc_List_List* _temp2205=({ struct Cyc_List_List*
_temp2204= lscs2; if( _temp2204 == 0){ _throw( Null_Exception);} _temp2204->tl;});
if( _temp2205 == 0){ _throw( Null_Exception);} _temp2205->hd;}))).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2202= lscs2; if( _temp2202 == 0){ _throw( Null_Exception);} _temp2202->hd;}))).f2;
if( sc->where_clause == 0){ struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r,
path, sc->pattern, succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List*
_temp2195=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2195->hd=( void*) res.f1; _temp2195->tl= nvs; _temp2195;}); decls= res.f2;
test_stmts=({ struct Cyc_List_List* _temp2196=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2196->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2197= lscs2; if( _temp2197 == 0){
_throw( Null_Exception);} _temp2197->hd;}))).f1, res.f3, 0); _temp2196->tl=
test_stmts; _temp2196;});} else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2201= sc->where_clause; if( _temp2201 == 0){ _throw(
Null_Exception);} _temp2201;}); struct _tagged_string* intermed_lab= Cyc_Toc_fresh_label();
struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r, path, sc->pattern,
intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c( res.f1, e); nvs=({ struct Cyc_List_List*
_temp2198=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2198->hd=( void*) res.f1; _temp2198->tl= nvs; _temp2198;}); decls= res.f2;{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt( e, Cyc_Absyn_goto_stmt(
succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3=
Cyc_Absyn_label_stmt( intermed_lab, s2, 0); test_stmts=({ struct Cyc_List_List*
_temp2199=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2199->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2200= lscs2; if( _temp2200 == 0){ _throw( Null_Exception);} _temp2200->hd;}))).f1,
Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0); _temp2199->tl= test_stmts; _temp2199;});}}}}{
struct Cyc_Absyn_Stmt* test_stmt= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( test_stmts), 0); nvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt*
clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){ for( 0;({ struct Cyc_List_List*
_temp2207= lscs; if( _temp2207 == 0){ _throw( Null_Exception);} _temp2207->tl;})
!= 0; lscs=({ struct Cyc_List_List* _temp2208= lscs; if( _temp2208 == 0){ _throw(
Null_Exception);} _temp2208->tl;}), nvs=({ struct Cyc_List_List* _temp2209= nvs;
if( _temp2209 == 0){ _throw( Null_Exception);} _temp2209->tl;})){ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2219= nvs; if( _temp2219
== 0){ _throw( Null_Exception);} _temp2219->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2218= lscs; if( _temp2218 == 0){
_throw( Null_Exception);} _temp2218->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
e, end_l,(*(( struct _tuple16*)({ struct Cyc_List_List* _temp2211=({ struct Cyc_List_List*
_temp2210= lscs; if( _temp2210 == 0){ _throw( Null_Exception);} _temp2210->tl;});
if( _temp2211 == 0){ _throw( Null_Exception);} _temp2211->hd;}))).f2,( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2214=((*(( struct _tuple16*)({
struct Cyc_List_List* _temp2213=({ struct Cyc_List_List* _temp2212= lscs; if(
_temp2212 == 0){ _throw( Null_Exception);} _temp2212->tl;}); if( _temp2213 == 0){
_throw( Null_Exception);} _temp2213->hd;}))).f3)->pat_vars; if( _temp2214 == 0){
_throw( Null_Exception);} _temp2214->v;}),( struct Cyc_Toc_Env*)({ struct Cyc_List_List*
_temp2216=({ struct Cyc_List_List* _temp2215= nvs; if( _temp2215 == 0){ _throw(
Null_Exception);} _temp2215->tl;}); if( _temp2216 == 0){ _throw( Null_Exception);}
_temp2216->hd;})), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2217= lscs; if( _temp2217 == 0){
_throw( Null_Exception);} _temp2217->hd;}))).f2, s, 0), 0);}{ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2222= nvs; if( _temp2222
== 0){ _throw( Null_Exception);} _temp2222->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2221= lscs; if( _temp2221 == 0){
_throw( Null_Exception);} _temp2221->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env(
e, end_l), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2220= lscs; if( _temp2220 == 0){
_throw( Null_Exception);} _temp2220->hd;}))).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( test_stmt, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=({
struct Cyc_List_List* _temp2223= decls; if( _temp2223 == 0){ _throw(
Null_Exception);} _temp2223->tl;})){ struct _tuple14 _temp2227; void* _temp2228;
struct _tuple0* _temp2230; struct _tuple14* _temp2225=( struct _tuple14*)({
struct Cyc_List_List* _temp2224= decls; if( _temp2224 == 0){ _throw(
Null_Exception);} _temp2224->hd;}); _temp2227=* _temp2225; _LL2231: _temp2230=
_temp2227.f1; goto _LL2229; _LL2229: _temp2228= _temp2227.f2; goto _LL2226;
_LL2226: res= Cyc_Absyn_declare_stmt( _temp2230, _temp2228, 0, res, 0);}( void*)(
whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( v, Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp2232= e->topt; if( _temp2232 == 0){ _throw(
Null_Exception);} _temp2232->v;})),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2234=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2234[ 0]=({ struct Cyc_List_List _temp2235; _temp2235.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2235.tl= 0; _temp2235;}); _temp2234;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2236=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2276; struct Cyc_Absyn_Stmt* _temp2278; struct Cyc_Absyn_Stmt*
_temp2280; struct Cyc_Absyn_Exp* _temp2282; struct Cyc_Absyn_Stmt* _temp2284;
struct Cyc_Absyn_Stmt* _temp2286; struct Cyc_Absyn_Exp* _temp2288; struct Cyc_Absyn_Stmt*
_temp2290; struct _tuple2 _temp2292; struct Cyc_Absyn_Stmt* _temp2294; struct
Cyc_Absyn_Exp* _temp2296; struct Cyc_Absyn_Stmt* _temp2298; struct Cyc_Absyn_Stmt*
_temp2300; struct Cyc_Absyn_Stmt* _temp2302; struct _tagged_string* _temp2304;
struct Cyc_Absyn_Stmt* _temp2306; struct _tuple2 _temp2308; struct Cyc_Absyn_Stmt*
_temp2310; struct Cyc_Absyn_Exp* _temp2312; struct _tuple2 _temp2314; struct Cyc_Absyn_Stmt*
_temp2316; struct Cyc_Absyn_Exp* _temp2318; struct Cyc_Absyn_Exp* _temp2320;
struct Cyc_List_List* _temp2322; struct Cyc_Absyn_Exp* _temp2324; struct Cyc_Absyn_Switch_clause**
_temp2326; struct Cyc_List_List* _temp2328; struct Cyc_Absyn_Stmt* _temp2330;
struct Cyc_Absyn_Decl* _temp2332; struct Cyc_Absyn_Stmt* _temp2334; struct Cyc_Absyn_Stmt*
_temp2336; struct Cyc_Absyn_Stmt* _temp2338; struct _tagged_string* _temp2340;
struct _tuple2 _temp2342; struct Cyc_Absyn_Stmt* _temp2344; struct Cyc_Absyn_Exp*
_temp2346; struct Cyc_Absyn_Stmt* _temp2348; struct Cyc_List_List* _temp2350;
struct Cyc_Absyn_Stmt* _temp2352; struct Cyc_Absyn_Stmt* _temp2354; struct Cyc_Absyn_Vardecl*
_temp2356; struct Cyc_Absyn_Tvar* _temp2358; _LL2238: if( _temp2236 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2239;} else{ goto _LL2240;} _LL2240: if((
unsigned int) _temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_Exp_s: 0){
_LL2277: _temp2276=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp2236)->f1; goto _LL2241;} else{ goto _LL2242;} _LL2242: if(( unsigned int)
_temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_Seq_s: 0){ _LL2281: _temp2280=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp2236)->f1; goto
_LL2279; _LL2279: _temp2278=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp2236)->f2; goto _LL2243;} else{ goto _LL2244;} _LL2244: if(( unsigned int)
_temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_Return_s: 0){ _LL2283:
_temp2282=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp2236)->f1; goto _LL2245;} else{ goto _LL2246;} _LL2246: if(( unsigned int)
_temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_IfThenElse_s: 0){ _LL2289:
_temp2288=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2236)->f1; goto _LL2287; _LL2287: _temp2286=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2236)->f2; goto _LL2285; _LL2285:
_temp2284=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2236)->f3; goto _LL2247;} else{ goto _LL2248;} _LL2248: if(( unsigned int)
_temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_While_s: 0){ _LL2293: _temp2292=(
struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp2236)->f1; _LL2297:
_temp2296= _temp2292.f1; goto _LL2295; _LL2295: _temp2294= _temp2292.f2; goto
_LL2291; _LL2291: _temp2290=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp2236)->f2; goto _LL2249;} else{ goto _LL2250;} _LL2250: if(( unsigned int)
_temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_Break_s: 0){ _LL2299: _temp2298=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp2236)->f1; goto
_LL2251;} else{ goto _LL2252;} _LL2252: if(( unsigned int) _temp2236 > 1u?*((
int*) _temp2236) == Cyc_Absyn_Continue_s: 0){ _LL2301: _temp2300=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp2236)->f1; goto _LL2253;} else{ goto
_LL2254;} _LL2254: if(( unsigned int) _temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_Goto_s:
0){ _LL2305: _temp2304=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp2236)->f1; goto _LL2303; _LL2303: _temp2302=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp2236)->f2; goto _LL2255;} else{ goto
_LL2256;} _LL2256: if(( unsigned int) _temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_For_s:
0){ _LL2321: _temp2320=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp2236)->f1; goto _LL2315; _LL2315: _temp2314=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp2236)->f2; _LL2319: _temp2318= _temp2314.f1; goto _LL2317; _LL2317:
_temp2316= _temp2314.f2; goto _LL2309; _LL2309: _temp2308=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp2236)->f3; _LL2313: _temp2312= _temp2308.f1;
goto _LL2311; _LL2311: _temp2310= _temp2308.f2; goto _LL2307; _LL2307: _temp2306=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp2236)->f4; goto
_LL2257;} else{ goto _LL2258;} _LL2258: if(( unsigned int) _temp2236 > 1u?*((
int*) _temp2236) == Cyc_Absyn_Switch_s: 0){ _LL2325: _temp2324=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp2236)->f1; goto _LL2323; _LL2323:
_temp2322=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp2236)->f2; goto _LL2259;} else{ goto _LL2260;} _LL2260: if(( unsigned int)
_temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_Fallthru_s: 0){ _LL2329:
_temp2328=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2236)->f1; goto _LL2327; _LL2327: _temp2326=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp2236)->f2; goto _LL2261;} else{ goto
_LL2262;} _LL2262: if(( unsigned int) _temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_Decl_s:
0){ _LL2333: _temp2332=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp2236)->f1; goto _LL2331; _LL2331: _temp2330=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp2236)->f2; goto _LL2263;} else{ goto
_LL2264;} _LL2264: if(( unsigned int) _temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_Cut_s:
0){ _LL2335: _temp2334=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp2236)->f1; goto _LL2265;} else{ goto _LL2266;} _LL2266: if(( unsigned int)
_temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_Splice_s: 0){ _LL2337:
_temp2336=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp2236)->f1; goto _LL2267;} else{ goto _LL2268;} _LL2268: if(( unsigned int)
_temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_Label_s: 0){ _LL2341: _temp2340=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp2236)->f1; goto
_LL2339; _LL2339: _temp2338=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp2236)->f2; goto _LL2269;} else{ goto _LL2270;} _LL2270: if(( unsigned int)
_temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_Do_s: 0){ _LL2349: _temp2348=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp2236)->f1; goto
_LL2343; _LL2343: _temp2342=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp2236)->f2; _LL2347: _temp2346= _temp2342.f1; goto _LL2345; _LL2345:
_temp2344= _temp2342.f2; goto _LL2271;} else{ goto _LL2272;} _LL2272: if((
unsigned int) _temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_TryCatch_s: 0){
_LL2353: _temp2352=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2236)->f1; goto _LL2351; _LL2351: _temp2350=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp2236)->f2; goto _LL2273;} else{ goto
_LL2274;} _LL2274: if(( unsigned int) _temp2236 > 1u?*(( int*) _temp2236) == Cyc_Absyn_Region_s:
0){ _LL2359: _temp2358=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2236)->f1; goto _LL2357; _LL2357: _temp2356=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp2236)->f2; goto _LL2355; _LL2355:
_temp2354=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2236)->f3; goto _LL2275;} else{ goto _LL2237;} _LL2239: return; _LL2241:
Cyc_Toc_exp_to_c( nv, _temp2276); return; _LL2243: Cyc_Toc_stmt_to_c( nv,
_temp2280); s= _temp2278; continue; _LL2245: { struct Cyc_Core_Opt* topt= 0; if(
_temp2282 != 0){ topt=({ struct Cyc_Core_Opt* _temp2360=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2360->v=( void*) Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp2362=({ struct Cyc_Absyn_Exp* _temp2361=
_temp2282; if( _temp2361 == 0){ _throw( Null_Exception);} _temp2361->topt;});
if( _temp2362 == 0){ _throw( Null_Exception);} _temp2362->v;})); _temp2360;});
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2363=
_temp2282; if( _temp2363 == 0){ _throw( Null_Exception);} _temp2363;}));} if( s->try_depth
> 0){ if( topt != 0){ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
retn_stmt= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x,
0), 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( x,( void*)({
struct Cyc_Core_Opt* _temp2364= topt; if( _temp2364 == 0){ _throw(
Null_Exception);} _temp2364->v;}), _temp2282, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), retn_stmt, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL2247: Cyc_Toc_exp_to_c( nv, _temp2288); Cyc_Toc_stmt_to_c( nv,
_temp2286); s= _temp2284; continue; _LL2249: Cyc_Toc_exp_to_c( nv, _temp2296);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2290); return; _LL2251: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)({ struct Cyc_Core_Opt* _temp2365= nv->break_lab; if( _temp2365
== 0){ _throw( Null_Exception);} _temp2365->v;}), 0))->r));}{ int dest_depth=
_temp2298 == 0? 0:({ struct Cyc_Absyn_Stmt* _temp2366= _temp2298; if( _temp2366
== 0){ _throw( Null_Exception);} _temp2366->try_depth;}); Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2253: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)({
struct Cyc_Core_Opt* _temp2367= nv->continue_lab; if( _temp2367 == 0){ _throw(
Null_Exception);} _temp2367->v;}), 0))->r));} Cyc_Toc_do_npop_before( s->try_depth
-({ struct Cyc_Absyn_Stmt* _temp2368= _temp2300; if( _temp2368 == 0){ _throw(
Null_Exception);} _temp2368->try_depth;}), s); return; _LL2255: Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2369= _temp2302; if( _temp2369 == 0){
_throw( Null_Exception);} _temp2369->try_depth;}), s); return; _LL2257: Cyc_Toc_exp_to_c(
nv, _temp2320); Cyc_Toc_exp_to_c( nv, _temp2318); Cyc_Toc_exp_to_c( nv,
_temp2312); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2306); return;
_LL2259: Cyc_Toc_xlate_switch( nv, s, _temp2324, _temp2322); return; _LL2261:
if( nv->fallthru_info == 0){( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2370=( char*)"fallthru in unexpected place";
struct _tagged_string _temp2371; _temp2371.curr= _temp2370; _temp2371.base=
_temp2370; _temp2371.last_plus_one= _temp2370 + 29; _temp2371;})));}{ struct
_tuple8 _temp2375; struct Cyc_Dict_Dict* _temp2376; struct Cyc_List_List*
_temp2378; struct _tagged_string* _temp2380; struct _tuple8* _temp2373=( struct
_tuple8*)({ struct Cyc_Core_Opt* _temp2372= nv->fallthru_info; if( _temp2372 ==
0){ _throw( Null_Exception);} _temp2372->v;}); _temp2375=* _temp2373; _LL2381:
_temp2380= _temp2375.f1; goto _LL2379; _LL2379: _temp2378= _temp2375.f2; goto
_LL2377; _LL2377: _temp2376= _temp2375.f3; goto _LL2374; _LL2374: { struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_goto_stmt( _temp2380, 0); Cyc_Toc_do_npop_before( s->try_depth -(({
struct Cyc_Absyn_Switch_clause** _temp2382= _temp2326; if( _temp2382 == 0){
_throw( Null_Exception);}* _temp2382;})->body)->try_depth, s2);{ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2378); struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2328); for( 0; vs2 != 0; vs2=({ struct Cyc_List_List*
_temp2383= vs2; if( _temp2383 == 0){ _throw( Null_Exception);} _temp2383->tl;}),
es2=({ struct Cyc_List_List* _temp2384= es2; if( _temp2384 == 0){ _throw(
Null_Exception);} _temp2384->tl;})){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp2385= es2; if( _temp2385 == 0){ _throw(
Null_Exception);} _temp2385->hd;})); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2376,( struct _tuple0*)({ struct Cyc_List_List* _temp2386= vs2; if(
_temp2386 == 0){ _throw( Null_Exception);} _temp2386->hd;})),( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp2387= es2; if( _temp2387 == 0){ _throw(
Null_Exception);} _temp2387->hd;}), 0), s2, 0);}( void*)( s->r=( void*)(( void*)
s2->r)); return;}}} _LL2263:{ void* _temp2388=( void*) _temp2332->r; int
_temp2396; struct Cyc_Absyn_Exp* _temp2398; struct Cyc_Core_Opt* _temp2400;
struct Cyc_Core_Opt* _temp2402; struct Cyc_Absyn_Pat* _temp2404; struct Cyc_Absyn_Vardecl*
_temp2406; _LL2390: if(*(( int*) _temp2388) == Cyc_Absyn_Let_d){ _LL2405:
_temp2404=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2388)->f1;
goto _LL2403; _LL2403: _temp2402=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2388)->f2; goto _LL2401; _LL2401: _temp2400=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2388)->f3; goto _LL2399; _LL2399: _temp2398=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2388)->f4; goto
_LL2397; _LL2397: _temp2396=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2388)->f5;
goto _LL2391;} else{ goto _LL2392;} _LL2392: if(*(( int*) _temp2388) == Cyc_Absyn_Var_d){
_LL2407: _temp2406=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2388)->f1; goto _LL2393;} else{ goto _LL2394;} _LL2394: goto _LL2395;
_LL2391: { int var_case= 0; struct Cyc_Absyn_Vardecl* vdopt= 0;{ void* _temp2408=(
void*) _temp2404->r; struct Cyc_Absyn_Vardecl* _temp2414; _LL2410: if((
unsigned int) _temp2408 > 2u?*(( int*) _temp2408) == Cyc_Absyn_Var_p: 0){
_LL2415: _temp2414=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp2408)->f1; goto _LL2411;} else{ goto _LL2412;} _LL2412: goto _LL2413;
_LL2411: var_case= 1; vdopt=( struct Cyc_Absyn_Vardecl*) _temp2414; _temp2414->initializer=(
struct Cyc_Absyn_Exp*) _temp2398;( void*)( _temp2332->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2416=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2416[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2417; _temp2417.tag= Cyc_Absyn_Var_d; _temp2417.f1= _temp2414; _temp2417;});
_temp2416;}))); goto _LL2409; _LL2413: goto _LL2409; _LL2409:;} if( var_case){
_temp2406=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_Absyn_Vardecl* _temp2418=
vdopt; if( _temp2418 == 0){ _throw( Null_Exception);} _temp2418;}); goto _LL2393;}(
void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2404, _temp2400,
_temp2398, _temp2396, _temp2330))->r)); goto _LL2389;} _LL2393: { struct _tuple0*
c_name= _temp2406->name;( void*)( _temp2406->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2406->type));{ struct Cyc_Toc_Env* nv2= Cyc_Toc_add_varmap( nv,
c_name, Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2432=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2432[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2433; _temp2433.tag= Cyc_Absyn_Local_b;
_temp2433.f1= _temp2406; _temp2433;}); _temp2432;}), 0)); Cyc_Toc_stmt_to_c( nv2,
_temp2330); if( _temp2406->initializer != 0){ struct Cyc_Absyn_Exp* init=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2431= _temp2406->initializer;
if( _temp2431 == 0){ _throw( Null_Exception);} _temp2431;}); void* _temp2419=(
void*) init->r; struct Cyc_Absyn_Exp* _temp2425; struct Cyc_Absyn_Exp* _temp2427;
struct Cyc_Absyn_Vardecl* _temp2429; _LL2421: if(*(( int*) _temp2419) == Cyc_Absyn_Comprehension_e){
_LL2430: _temp2429=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2419)->f1; goto _LL2428; _LL2428: _temp2427=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2419)->f2; goto _LL2426; _LL2426:
_temp2425=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2419)->f3; goto _LL2422;} else{ goto _LL2423;} _LL2423: goto _LL2424;
_LL2422: _temp2406->initializer= 0;( void*)( _temp2330->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( c_name, 0), _temp2429, _temp2427, _temp2425, Cyc_Absyn_new_stmt((
void*) _temp2330->r, 0), 0))->r)); goto _LL2420; _LL2424: Cyc_Toc_exp_to_c( nv,
init); goto _LL2420; _LL2420:;} goto _LL2389;}} _LL2395:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2434=(
char*)"bad nested declaration within function"; struct _tagged_string _temp2435;
_temp2435.curr= _temp2434; _temp2435.base= _temp2434; _temp2435.last_plus_one=
_temp2434 + 39; _temp2435;})); goto _LL2389; _LL2389:;} return; _LL2265:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp2436=( char*)"cut"; struct _tagged_string _temp2437; _temp2437.curr=
_temp2436; _temp2437.base= _temp2436; _temp2437.last_plus_one= _temp2436 + 4;
_temp2437;})); return; _LL2267:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2438=( char*)"splice"; struct _tagged_string
_temp2439; _temp2439.curr= _temp2438; _temp2439.base= _temp2438; _temp2439.last_plus_one=
_temp2438 + 7; _temp2439;})); return; _LL2269: s= _temp2338; continue; _LL2271:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2348); Cyc_Toc_exp_to_c( nv,
_temp2346); return; _LL2273: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct
_tuple0* e_var= Cyc_Toc_temp_var(); struct _tuple0* was_thrown_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2456=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2456->f1= Cyc_Toc_abs_ns; _temp2456->f2= Cyc_Toc__push_handler_sp;
_temp2456;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple0* _temp2455=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2455->f1= Cyc_Toc_abs_ns; _temp2455->f2= Cyc_Toc_setjmp_sp; _temp2455;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2454=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2454->f1=
Cyc_Toc_abs_ns; _temp2454->f2= Cyc_Toc__pop_handler_sp; _temp2454;}), 0); struct
Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp(({ struct _tuple0* _temp2453=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2453->f1= Cyc_Toc_abs_ns;
_temp2453->f2= Cyc_Toc__exn_thrown_sp; _temp2453;}), 0); e_exp->topt=({ struct
Cyc_Core_Opt* _temp2440=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2440->v=( void*) e_typ; _temp2440;}); nv= Cyc_Toc_add_varmap( nv, e_var,
e_exp); Cyc_Toc_stmt_to_c( nv, _temp2352);{ struct Cyc_Absyn_Stmt*
tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp2352, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0); struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl* x_vd= Cyc_Absyn_new_vardecl(
x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt* _temp2441=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2441->v=( void*)
Cyc_Absyn_exn_typ; _temp2441;});{ struct Cyc_Absyn_Pat* p=({ struct Cyc_Absyn_Pat*
_temp2449=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof( struct Cyc_Absyn_Pat));
_temp2449->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp2451=(
struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2451[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2452; _temp2452.tag= Cyc_Absyn_Var_p;
_temp2452.f1= x_vd; _temp2452;}); _temp2451;})); _temp2449->topt=({ struct Cyc_Core_Opt*
_temp2450=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2450->v=( void*) Cyc_Absyn_exn_typ; _temp2450;}); _temp2449->loc= 0;
_temp2449;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp2442=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2442->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2442;});{
struct Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp2446=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2446->pattern= p;
_temp2446->pat_vars=({ struct Cyc_Core_Opt* _temp2447=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2447->v=( void*)({ struct Cyc_List_List*
_temp2448=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2448->hd=( void*) x_vd; _temp2448->tl= 0; _temp2448;}); _temp2447;});
_temp2446->where_clause= 0; _temp2446->body= rs; _temp2446->loc= 0; _temp2446;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2350,({
struct Cyc_List_List* _temp2445=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2445->hd=( void*) default_case; _temp2445->tl= 0;
_temp2445;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List* _temp2444=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2444->hd=(
void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0); _temp2444->tl=
0; _temp2444;}), 0); struct Cyc_Absyn_Stmt* pushhandler_call= Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List* _temp2443=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2443->hd=(
void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2443->tl= 0; _temp2443;}), 0), 0);
struct Cyc_Absyn_Exp* zero_exp= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0,
0); struct Cyc_Absyn_Exp* one_exp= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed,
1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0,
Cyc_Absyn_seq_stmt( pushhandler_call, Cyc_Absyn_declare_stmt( was_thrown_var,
was_thrown_typ,( struct Cyc_Absyn_Exp*) zero_exp, Cyc_Absyn_seq_stmt( Cyc_Absyn_ifthenelse_stmt(
setjmp_call, Cyc_Absyn_assign_stmt( was_thrown_exp, one_exp, 0), Cyc_Absyn_skip_stmt(
0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Not,
was_thrown_exp, 0), tryandpop_stmt, Cyc_Absyn_declare_stmt( e_var, e_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp, 0),
handler_stmt, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL2275: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__region_handle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2356->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2462=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2462[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2463; _temp2463.tag= Cyc_Absyn_Local_b; _temp2463.f1=
_temp2356; _temp2463;}); _temp2462;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2461=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2461->f1= Cyc_Toc_abs_ns; _temp2461->f2= Cyc_Toc__push_region_sp;
_temp2461;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2460=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2460->f1= Cyc_Toc_abs_ns; _temp2460->f2= Cyc_Toc__pop_region_sp; _temp2460;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2459=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2459->f1=
Cyc_Toc_abs_ns; _temp2459->f2= Cyc_Toc__new_region_sp; _temp2459;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2458=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2458->hd=( void*) addr_rh_exp; _temp2458->tl= 0; _temp2458;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2457=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2457->hd=( void*) x_exp; _temp2457->tl= 0; _temp2457;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2354);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2354, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2237:;}} struct _tuple17{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0;(
void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{ struct
Cyc_List_List* args= f->args; for( 0; args != 0; args=({ struct Cyc_List_List*
_temp2464= args; if( _temp2464 == 0){ _throw( Null_Exception);} _temp2464->tl;})){(*((
struct _tuple17*)({ struct Cyc_List_List* _temp2465= args; if( _temp2465 == 0){
_throw( Null_Exception);} _temp2465->hd;}))).f3= Cyc_Toc_typ_to_c((*(( struct
_tuple17*)({ struct Cyc_List_List* _temp2466= args; if( _temp2466 == 0){ _throw(
Null_Exception);} _temp2466->hd;}))).f3);{ struct _tuple0* x=({ struct _tuple0*
_temp2467=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2467->f1=(
void*) Cyc_Absyn_Loc_n; _temp2467->f2=(*(( struct _tuple17*)({ struct Cyc_List_List*
_temp2468= args; if( _temp2468 == 0){ _throw( Null_Exception);} _temp2468->hd;}))).f1;
_temp2467;}); nv= Cyc_Toc_add_varmap( nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel( nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){
void* _temp2469= s; _LL2471: if( _temp2469 ==( void*) Cyc_Absyn_Abstract){ goto
_LL2472;} else{ goto _LL2473;} _LL2473: if( _temp2469 ==( void*) Cyc_Absyn_ExternC){
goto _LL2474;} else{ goto _LL2475;} _LL2475: goto _LL2476; _LL2472: return( void*)
Cyc_Absyn_Public; _LL2474: return( void*) Cyc_Absyn_Extern; _LL2476: return s;
_LL2470:;} struct Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* n=( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2489= s->name; if( _temp2489 == 0){ _throw( Null_Exception);}
_temp2489->v;}); if( Cyc_Toc_structs_so_far == 0){ Cyc_Toc_structs_so_far=({
struct Cyc_Core_Opt* _temp2477=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2477->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2477;});}{ int seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2488= Cyc_Toc_structs_so_far;
if( _temp2488 == 0){ _throw( Null_Exception);} _temp2488->v;}),(* n).f2); if(
dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2478= Cyc_Toc_structs_so_far; if( _temp2478 == 0){ _throw( Null_Exception);}
_temp2478->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2479= Cyc_Toc_structs_so_far;
if( _temp2479 == 0){ _throw( Null_Exception);} _temp2479->v;}),(* n).f2, s);});}
else{ if((( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Core_Opt* _temp2480= dopt;
if( _temp2480 == 0){ _throw( Null_Exception);} _temp2480->v;}))->fields == 0){(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2481= Cyc_Toc_structs_so_far;
if( _temp2481 == 0){ _throw( Null_Exception);} _temp2481->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2482= Cyc_Toc_structs_so_far; if( _temp2482 == 0){ _throw(
Null_Exception);} _temp2482->v;}),(* n).f2, s);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) s->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) s->sc))); s->tvs= 0; s->name=({
struct Cyc_Core_Opt* _temp2483=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2483->v=( void*) n; _temp2483;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2487= s->fields; if( _temp2487 == 0){
_throw( Null_Exception);} _temp2487->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2484= fields; if( _temp2484 == 0){ _throw( Null_Exception);}
_temp2484->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2485= fields; if( _temp2485 == 0){ _throw( Null_Exception);} _temp2485->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2486= fields; if( _temp2486 == 0){ _throw( Null_Exception);}
_temp2486->hd;}))->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* n=( struct _tuple0*)({ struct Cyc_Core_Opt* _temp2502= u->name; if(
_temp2502 == 0){ _throw( Null_Exception);} _temp2502->v;}); if( Cyc_Toc_unions_so_far
== 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2490=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2490->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2490;});}{ int seen_defn_before; struct Cyc_Core_Opt*
dopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2501= Cyc_Toc_unions_so_far; if( _temp2501 == 0){ _throw( Null_Exception);}
_temp2501->v;}),(* n).f2); if( dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2491= Cyc_Toc_unions_so_far; if( _temp2491 == 0){
_throw( Null_Exception);} _temp2491->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2492= Cyc_Toc_unions_so_far; if( _temp2492 == 0){ _throw( Null_Exception);}
_temp2492->v;}),(* n).f2, u);});} else{ if((( struct Cyc_Absyn_Uniondecl*)({
struct Cyc_Core_Opt* _temp2493= dopt; if( _temp2493 == 0){ _throw(
Null_Exception);} _temp2493->v;}))->fields == 0){( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2494= Cyc_Toc_unions_so_far; if( _temp2494 == 0){
_throw( Null_Exception);} _temp2494->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2495= Cyc_Toc_unions_so_far; if( _temp2495 == 0){ _throw( Null_Exception);}
_temp2495->v;}),(* n).f2, u);}); seen_defn_before= 0;} else{ seen_defn_before= 1;}}(
void*)( u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*)
u->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2496=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2496->v=( void*) n; _temp2496;}); if( u->fields != 0){ if(
seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2500= u->fields; if( _temp2500 == 0){
_throw( Null_Exception);} _temp2500->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2497= fields; if( _temp2497 == 0){ _throw( Null_Exception);}
_temp2497->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2498= fields; if( _temp2498 == 0){ _throw( Null_Exception);} _temp2498->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2499= fields; if( _temp2499 == 0){ _throw( Null_Exception);}
_temp2499->hd;}))->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2503=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2503->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2503;});}{ struct
_tuple0* n= tud->name; if( tud->fields == 0){ return;} if((( int(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2504= Cyc_Toc_tunions_so_far; if( _temp2504 == 0){ _throw( Null_Exception);}
_temp2504->v;}), n)){ return;}( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2505= Cyc_Toc_tunions_so_far; if( _temp2505 == 0){ _throw( Null_Exception);}
_temp2505->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct
_tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2506= Cyc_Toc_tunions_so_far; if( _temp2506 == 0){ _throw( Null_Exception);}
_temp2506->v;}), n);});{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2533= tud->fields;
if( _temp2533 == 0){ _throw( Null_Exception);} _temp2533->v;}); for( 0; fields
!= 0; fields=({ struct Cyc_List_List* _temp2507= fields; if( _temp2507 == 0){
_throw( Null_Exception);} _temp2507->tl;})){ struct Cyc_Absyn_Tunionfield* f=(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2532= fields; if(
_temp2532 == 0){ _throw( Null_Exception);} _temp2532->hd;}); if( f->typs == 0){
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
tag_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2508; _temp2508.q_const=
1; _temp2508.q_volatile= 0; _temp2508.q_restrict= 0; _temp2508;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2509=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2509->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2510=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2510[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2511; _temp2511.tag= Cyc_Absyn_Var_d; _temp2511.f1= vd; _temp2511;});
_temp2510;}), 0); _temp2509->tl= Cyc_Toc_result_decls; _temp2509;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
box_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2512; _temp2512.q_const=
1; _temp2512.q_volatile= 0; _temp2512.q_restrict= 0; _temp2512;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2513=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2513->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2514=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2514[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2515; _temp2515.tag= Cyc_Absyn_Var_d; _temp2515.f1= vd; _temp2515;});
_temp2514;}), 0); _temp2513->tl= Cyc_Toc_result_decls; _temp2513;});{ struct Cyc_List_List*
fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts=({
struct Cyc_List_List* _temp2516= ts; if( _temp2516 == 0){ _throw( Null_Exception);}
_temp2516->tl;}), i ++){ struct _tagged_string* fname=( struct _tagged_string*)({
struct _tagged_string* _temp2521=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp2521[ 0]= xprintf("f%d", i); _temp2521;}); struct
Cyc_Absyn_Structfield* f=({ struct Cyc_Absyn_Structfield* _temp2518=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2518->name= fname;
_temp2518->tq=(*(( struct _tuple3*)({ struct Cyc_List_List* _temp2520= ts; if(
_temp2520 == 0){ _throw( Null_Exception);} _temp2520->hd;}))).f1; _temp2518->type=(
void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp2519= ts; if( _temp2519 == 0){ _throw( Null_Exception);} _temp2519->hd;}))).f2);
_temp2518->width= 0; _temp2518->attributes= 0; _temp2518;}); fs=({ struct Cyc_List_List*
_temp2517=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2517->hd=( void*) f; _temp2517->tl= fs; _temp2517;});}} fs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs); fs=({ struct Cyc_List_List*
_temp2522=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2522->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2523=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2523->name= Cyc_Toc_tag_sp;
_temp2523->tq= Cyc_Toc_mt_tq; _temp2523->type=( void*) Cyc_Absyn_sint_t;
_temp2523->width= 0; _temp2523->attributes= 0; _temp2523;}); _temp2522->tl= fs;
_temp2522;});{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp2527=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2527->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2527->name=({ struct Cyc_Core_Opt*
_temp2529=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2529->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2530=( char*)"_struct"; struct _tagged_string _temp2531; _temp2531.curr=
_temp2530; _temp2531.base= _temp2530; _temp2531.last_plus_one= _temp2530 + 8;
_temp2531;})); _temp2529;}); _temp2527->tvs= 0; _temp2527->fields=({ struct Cyc_Core_Opt*
_temp2528=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2528->v=( void*) fs; _temp2528;}); _temp2527->attributes= 0; _temp2527;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2524=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2524->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2525=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2525[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2526; _temp2526.tag= Cyc_Absyn_Struct_d;
_temp2526.f1= sd; _temp2526;}); _temp2525;}), 0); _temp2524->tl= Cyc_Toc_result_decls;
_temp2524;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2534=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2534->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2534;});} if( xd->fields
== 0){ return;}{ struct _tuple0* n= xd->name; struct Cyc_List_List* fs=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2568= xd->fields; if( _temp2568 == 0){
_throw( Null_Exception);} _temp2568->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp2535= fs; if( _temp2535 == 0){ _throw( Null_Exception);} _temp2535->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp2567= fs; if( _temp2567 == 0){ _throw( Null_Exception);} _temp2567->hd;});
struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_uint_exp(({
struct _tagged_string _temp2566=* fn;( unsigned int)( _temp2566.last_plus_one -
_temp2566.curr);}) + 4, 0); struct Cyc_Absyn_ArrayType_struct* tag_typ=({ struct
Cyc_Absyn_ArrayType_struct* _temp2564=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2564[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2565; _temp2565.tag= Cyc_Absyn_ArrayType;
_temp2565.f1=( void*) Cyc_Absyn_uchar_t; _temp2565.f2= Cyc_Toc_mt_tq; _temp2565.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp2565;}); _temp2564;}); struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) xd->sc !=( void*) Cyc_Absyn_Extern? !(( int(*)( struct
Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)({
struct Cyc_Core_Opt* _temp2536= Cyc_Toc_xtunion_fields_so_far; if( _temp2536 ==
0){ _throw( Null_Exception);} _temp2536->v;}), f->name): 0){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(({ int _temp2537= 0; int _temp2538= 0; int
_temp2539= 0; int _temp2540= 0; struct _tagged_string _temp2541=* fn; xprintf("%c%c%c%c%.*s",
_temp2537, _temp2538, _temp2539, _temp2540, _temp2541.last_plus_one - _temp2541.curr,
_temp2541.curr);}), 0);}{ struct Cyc_Absyn_Vardecl* tag_decl= Cyc_Absyn_new_vardecl(
f->name,( void*) tag_typ, initopt);( void*)( tag_decl->sc=( void*)(( void*) xd->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2542=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2542->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2543=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2543[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2544; _temp2544.tag= Cyc_Absyn_Var_d; _temp2544.f1= tag_decl; _temp2544;});
_temp2543;}), 0); _temp2542->tl= Cyc_Toc_result_decls; _temp2542;}); if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)({
struct Cyc_Core_Opt* _temp2545= Cyc_Toc_xtunion_fields_so_far; if( _temp2545 ==
0){ _throw( Null_Exception);} _temp2545->v;}), f->name)){ continue;}( struct Cyc_Set_Set*)({
struct Cyc_Core_Opt* _temp2546= Cyc_Toc_xtunion_fields_so_far; if( _temp2546 ==
0){ _throw( Null_Exception);} _temp2546->v=( void*)(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)({
struct Cyc_Core_Opt* _temp2547= Cyc_Toc_xtunion_fields_so_far; if( _temp2547 ==
0){ _throw( Null_Exception);} _temp2547->v;}), f->name);}); if( f->typs != 0){
struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List* tqts= f->typs;
for( 0; tqts != 0; tqts=({ struct Cyc_List_List* _temp2548= tqts; if( _temp2548
== 0){ _throw( Null_Exception);} _temp2548->tl;}), i ++){ struct _tagged_string*
field_n=( struct _tagged_string*)({ struct _tagged_string* _temp2553=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2553[ 0]=
xprintf("f%d", i); _temp2553;}); struct Cyc_Absyn_Structfield* newf=({ struct
Cyc_Absyn_Structfield* _temp2550=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp2550->name= field_n; _temp2550->tq=(*((
struct _tuple3*)({ struct Cyc_List_List* _temp2552= tqts; if( _temp2552 == 0){
_throw( Null_Exception);} _temp2552->hd;}))).f1; _temp2550->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)({ struct Cyc_List_List* _temp2551= tqts; if( _temp2551 == 0){
_throw( Null_Exception);} _temp2551->hd;}))).f2); _temp2550->width= 0; _temp2550->attributes=
0; _temp2550;}); fields=({ struct Cyc_List_List* _temp2549=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2549->hd=( void*) newf;
_temp2549->tl= fields; _temp2549;});}} fields=({ struct Cyc_List_List* _temp2554=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2554->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp2555=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2555->name= Cyc_Toc_tag_sp;
_temp2555->tq= Cyc_Toc_mt_tq; _temp2555->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2555->width= 0; _temp2555->attributes= 0; _temp2555;});
_temp2554->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2554;});{ struct Cyc_Absyn_Structdecl* strct_decl=({ struct Cyc_Absyn_Structdecl*
_temp2559=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2559->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2559->name=({ struct Cyc_Core_Opt*
_temp2561=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2561->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2562=( char*)"_struct"; struct _tagged_string _temp2563; _temp2563.curr=
_temp2562; _temp2563.base= _temp2562; _temp2563.last_plus_one= _temp2562 + 8;
_temp2563;})); _temp2561;}); _temp2559->tvs= 0; _temp2559->fields=({ struct Cyc_Core_Opt*
_temp2560=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2560->v=( void*) fields; _temp2560;}); _temp2559->attributes= 0; _temp2559;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2556=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2556->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2557=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2557[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2558; _temp2558.tag= Cyc_Absyn_Struct_d;
_temp2558.f1= strct_decl; _temp2558;}); _temp2557;}), 0); _temp2556->tl= Cyc_Toc_result_decls;
_temp2556;});}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* n= ed->name;( void*)( ed->sc=(
void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) ed->sc ==( void*)
Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public:
Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields != 0){ struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2574= ed->fields; if(
_temp2574 == 0){ _throw( Null_Exception);} _temp2574->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp2569= fs; if( _temp2569 == 0){ _throw( Null_Exception);}
_temp2569->tl;})){ if((( struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List*
_temp2570= fs; if( _temp2570 == 0){ _throw( Null_Exception);} _temp2570->hd;}))->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2573=(( struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List*
_temp2572= fs; if( _temp2572 == 0){ _throw( Null_Exception);} _temp2572->hd;}))->tag;
if( _temp2573 == 0){ _throw( Null_Exception);} _temp2573;}));(( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2571= fs; if( _temp2571 == 0){ _throw( Null_Exception);}
_temp2571->hd;}))->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( i, 0);}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)({ struct Cyc_Core_Opt*
_temp2593= topt; if( _temp2593 == 0){ _throw( Null_Exception);} _temp2593->v;});
Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var(); struct
_tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string* fail_lab=
exhaust? succ_lab: Cyc_Toc_fresh_label(); struct Cyc_Absyn_Stmt* _temp2577;
struct Cyc_List_List* _temp2579; struct Cyc_Toc_Env* _temp2581; struct _tuple13
_temp2575= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp(
x, 0), p, succ_lab, fail_lab, 0); _LL2582: _temp2581= _temp2575.f1; goto _LL2580;
_LL2580: _temp2579= _temp2575.f2; goto _LL2578; _LL2578: _temp2577= _temp2575.f3;
goto _LL2576; _LL2576: Cyc_Toc_stmt_to_c( _temp2581, s);{ struct Cyc_Absyn_Stmt*
succ_stmt= Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2577, succ_stmt, 0), 0);} else{ struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2583=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2583->f1= Cyc_Toc_abs_ns; _temp2583->f2= Cyc_Toc_Match_Exception_sp;
_temp2583;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2577, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp2579 != 0; _temp2579=({ struct
Cyc_List_List* _temp2584= _temp2579; if( _temp2584 == 0){ _throw( Null_Exception);}
_temp2584->tl;})){ struct _tuple14 _temp2588; void* _temp2589; struct _tuple0*
_temp2591; struct _tuple14* _temp2586=( struct _tuple14*)({ struct Cyc_List_List*
_temp2585= _temp2579; if( _temp2585 == 0){ _throw( Null_Exception);} _temp2585->hd;});
_temp2588=* _temp2586; _LL2592: _temp2591= _temp2588.f1; goto _LL2590; _LL2590:
_temp2589= _temp2588.f2; goto _LL2587; _LL2587: s= Cyc_Absyn_declare_stmt(
_temp2591, _temp2589, 0, s, 0);} return s;}}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=({
struct Cyc_List_List* _temp2594= ds; if( _temp2594 == 0){ _throw( Null_Exception);}
_temp2594->tl;})){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct
Cyc_List_List* _temp2668= ds; if( _temp2668 == 0){ _throw( Null_Exception);}
_temp2668->hd;}); void* _temp2595=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2619; struct Cyc_Absyn_Fndecl* _temp2621; int _temp2623; struct Cyc_Absyn_Exp*
_temp2625; struct Cyc_Core_Opt* _temp2627; struct Cyc_Core_Opt* _temp2629;
struct Cyc_Absyn_Pat* _temp2631; struct Cyc_Absyn_Structdecl* _temp2633; struct
Cyc_Absyn_Uniondecl* _temp2635; struct Cyc_Absyn_Tuniondecl* _temp2637; struct
Cyc_Absyn_Enumdecl* _temp2639; struct Cyc_Absyn_Typedefdecl* _temp2641; struct
Cyc_List_List* _temp2643; struct _tagged_string* _temp2645; struct Cyc_List_List*
_temp2647; struct _tuple0* _temp2649; struct Cyc_List_List* _temp2651; _LL2597:
if(*(( int*) _temp2595) == Cyc_Absyn_Var_d){ _LL2620: _temp2619=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp2595)->f1; goto _LL2598;} else{ goto
_LL2599;} _LL2599: if(*(( int*) _temp2595) == Cyc_Absyn_Fn_d){ _LL2622:
_temp2621=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp2595)->f1;
goto _LL2600;} else{ goto _LL2601;} _LL2601: if(*(( int*) _temp2595) == Cyc_Absyn_Let_d){
_LL2632: _temp2631=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2595)->f1; goto _LL2630; _LL2630: _temp2629=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2595)->f2; goto _LL2628; _LL2628: _temp2627=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2595)->f3; goto
_LL2626; _LL2626: _temp2625=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2595)->f4; goto _LL2624; _LL2624: _temp2623=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp2595)->f5; goto _LL2602;} else{ goto _LL2603;} _LL2603: if(*(( int*)
_temp2595) == Cyc_Absyn_Struct_d){ _LL2634: _temp2633=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp2595)->f1; goto _LL2604;} else{ goto
_LL2605;} _LL2605: if(*(( int*) _temp2595) == Cyc_Absyn_Union_d){ _LL2636:
_temp2635=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp2595)->f1; goto _LL2606;} else{ goto _LL2607;} _LL2607: if(*(( int*)
_temp2595) == Cyc_Absyn_Tunion_d){ _LL2638: _temp2637=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp2595)->f1; goto _LL2608;} else{ goto
_LL2609;} _LL2609: if(*(( int*) _temp2595) == Cyc_Absyn_Enum_d){ _LL2640:
_temp2639=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp2595)->f1; goto _LL2610;} else{ goto _LL2611;} _LL2611: if(*(( int*)
_temp2595) == Cyc_Absyn_Typedef_d){ _LL2642: _temp2641=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp2595)->f1; goto _LL2612;} else{ goto
_LL2613;} _LL2613: if(*(( int*) _temp2595) == Cyc_Absyn_Namespace_d){ _LL2646:
_temp2645=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2595)->f1; goto _LL2644; _LL2644: _temp2643=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2595)->f2; goto _LL2614;} else{ goto
_LL2615;} _LL2615: if(*(( int*) _temp2595) == Cyc_Absyn_Using_d){ _LL2650:
_temp2649=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp2595)->f1;
goto _LL2648; _LL2648: _temp2647=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2595)->f2; goto _LL2616;} else{ goto _LL2617;} _LL2617: if(*(( int*)
_temp2595) == Cyc_Absyn_ExternC_d){ _LL2652: _temp2651=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp2595)->f1; goto _LL2618;} else{ goto
_LL2596;} _LL2598: { struct _tuple0* c_name= _temp2619->name; if(( void*)
_temp2619->sc ==( void*) Cyc_Absyn_ExternC){ c_name=({ struct _tuple0* _temp2653=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2653->f1= Cyc_Toc_abs_ns;
_temp2653->f2=(* c_name).f2; _temp2653;});} if( _temp2619->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2654=
_temp2619->initializer; if( _temp2654 == 0){ _throw( Null_Exception);} _temp2654;}));}
if( ! nv->toplevel){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp2655=( char*)"decls_to_c: not at toplevel!";
struct _tagged_string _temp2656; _temp2656.curr= _temp2655; _temp2656.base=
_temp2655; _temp2656.last_plus_one= _temp2655 + 29; _temp2656;}));} nv= Cyc_Toc_add_varmap(
nv, _temp2619->name, Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2657=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2657[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2658; _temp2658.tag= Cyc_Absyn_Global_b;
_temp2658.f1= _temp2619; _temp2658;}); _temp2657;}), 0)); _temp2619->name=
c_name;( void*)( _temp2619->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2619->sc));(
void*)( _temp2619->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2619->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2659=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2659->hd=( void*) d; _temp2659->tl=
Cyc_Toc_result_decls; _temp2659;}); goto _LL2596;} _LL2600: { struct _tuple0*
c_name= _temp2621->name; if(( void*) _temp2621->sc ==( void*) Cyc_Absyn_ExternC){
c_name=({ struct _tuple0* _temp2660=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp2660->f1= Cyc_Toc_abs_ns; _temp2660->f2=(* c_name).f2; _temp2660;});}
nv= Cyc_Toc_add_varmap( nv, _temp2621->name, Cyc_Absyn_var_exp( c_name, 0));
_temp2621->name= c_name; Cyc_Toc_fndecl_to_c( nv, _temp2621); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2661=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2661->hd=( void*) d; _temp2661->tl= Cyc_Toc_result_decls;
_temp2661;}); goto _LL2596;} _LL2602:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2662=( char*)"letdecl"; struct
_tagged_string _temp2663; _temp2663.curr= _temp2662; _temp2663.base= _temp2662;
_temp2663.last_plus_one= _temp2662 + 8; _temp2663;})); goto _LL2596; _LL2604:
Cyc_Toc_structdecl_to_c( _temp2633); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2664=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2664->hd=( void*) d; _temp2664->tl= Cyc_Toc_result_decls; _temp2664;});
goto _LL2596; _LL2606: Cyc_Toc_uniondecl_to_c( _temp2635); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2665=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2665->hd=( void*) d; _temp2665->tl= Cyc_Toc_result_decls;
_temp2665;}); goto _LL2596; _LL2608: if( _temp2637->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2637);} else{ Cyc_Toc_tuniondecl_to_c( _temp2637);} goto _LL2596; _LL2610:
Cyc_Toc_enumdecl_to_c( nv, _temp2639); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2666=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2666->hd=( void*) d; _temp2666->tl= Cyc_Toc_result_decls; _temp2666;});
goto _LL2596; _LL2612: _temp2641->name= _temp2641->name; _temp2641->tvs= 0;(
void*)( _temp2641->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2641->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2667=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2667->hd=( void*) d; _temp2667->tl=
Cyc_Toc_result_decls; _temp2667;}); goto _LL2596; _LL2614: _temp2647= _temp2643;
goto _LL2616; _LL2616: _temp2651= _temp2647; goto _LL2618; _LL2618: nv= Cyc_Toc_decls_to_c(
nv, _temp2651, top); goto _LL2596; _LL2596:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}