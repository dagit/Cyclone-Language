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
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileCloseError[
19u]; extern char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t; typedef
struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
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
struct Cyc_List_List* ds); int Cyc_Toc_use_runtimecheck_funs= 1; static struct
Cyc_List_List* Cyc_Toc_result_decls= 0; struct Cyc_Absyn_Abs_n_struct Cyc_Toc_abs_ns_value={
1u, 0}; void* Cyc_Toc_abs_ns=( void*)& Cyc_Toc_abs_ns_value; struct Cyc_Absyn_Tqual
Cyc_Toc_mt_tq=( struct Cyc_Absyn_Tqual){.q_const= 0,.q_volatile= 0,.q_restrict=
0}; char Cyc_Toc_Toc_Unimplemented[ 22u]="\000\000\000\000Toc_Unimplemented";
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
_tagged_string* Cyc_Toc__new_region_sp=& Cyc_Toc__new_region_string; static char
_temp68[ 12u]="_check_null"; static struct _tagged_string Cyc_Toc__check_null_string=(
struct _tagged_string){ _temp68, _temp68, _temp68 + 12u}; static struct
_tagged_string* Cyc_Toc__check_null_sp=& Cyc_Toc__check_null_string; static
struct _tuple0 Cyc_Toc__check_null_pr=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__check_null_string}; static struct _tuple0* Cyc_Toc__check_null_qv=& Cyc_Toc__check_null_pr;
static char _temp71[ 25u]="_check_unknown_subscript"; static struct
_tagged_string Cyc_Toc__check_unknown_subscript_string=( struct _tagged_string){
_temp71, _temp71, _temp71 + 25u}; static struct _tagged_string* Cyc_Toc__check_unknown_subscript_sp=&
Cyc_Toc__check_unknown_subscript_string; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr=(
struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__check_unknown_subscript_string};
static struct _tuple0* Cyc_Toc__check_unknown_subscript_qv=& Cyc_Toc__check_unknown_subscript_pr;
static char _temp74[ 28u]="_check_known_subscript_null"; static struct
_tagged_string Cyc_Toc__check_known_subscript_null_string=( struct
_tagged_string){ _temp74, _temp74, _temp74 + 28u}; static struct _tagged_string*
Cyc_Toc__check_known_subscript_null_sp=& Cyc_Toc__check_known_subscript_null_string;
static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr=( struct _tuple0){.f1=(
void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__check_known_subscript_null_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_null_qv=& Cyc_Toc__check_known_subscript_null_pr;
static char _temp77[ 31u]="_check_known_subscript_notnull"; static struct
_tagged_string Cyc_Toc__check_known_subscript_notnull_string=( struct
_tagged_string){ _temp77, _temp77, _temp77 + 31u}; static struct _tagged_string*
Cyc_Toc__check_known_subscript_notnull_sp=& Cyc_Toc__check_known_subscript_notnull_string;
static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr=( struct _tuple0){.f1=(
void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__check_known_subscript_notnull_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_notnull_qv=& Cyc_Toc__check_known_subscript_notnull_pr;
struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static
int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp78=( void*) e->r; void*
_temp104; char _temp106; void* _temp108; void* _temp110; short _temp112; void*
_temp114; void* _temp116; int _temp118; void* _temp120; void* _temp122;
long long _temp124; void* _temp126; void* _temp128; struct Cyc_Absyn_Exp*
_temp130; void* _temp132; struct Cyc_List_List* _temp134; struct Cyc_List_List*
_temp136; struct Cyc_Absyn_Structdecl* _temp138; struct Cyc_List_List* _temp140;
struct Cyc_Core_Opt* _temp142; struct _tuple0* _temp144; struct Cyc_List_List*
_temp146; struct _tuple1* _temp148; struct Cyc_List_List* _temp150; struct Cyc_Core_Opt*
_temp152; _LL80: if(*(( int*) _temp78) == Cyc_Absyn_Const_e){ _LL105: _temp104=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp78)->f1; if(( unsigned int)
_temp104 > 1u?*(( int*) _temp104) == Cyc_Absyn_Char_c: 0){ _LL109: _temp108=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp104)->f1; goto _LL107; _LL107:
_temp106=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp104)->f2; goto _LL81;}
else{ goto _LL82;}} else{ goto _LL82;} _LL82: if(*(( int*) _temp78) == Cyc_Absyn_Const_e){
_LL111: _temp110=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp78)->f1; if((
unsigned int) _temp110 > 1u?*(( int*) _temp110) == Cyc_Absyn_Short_c: 0){ _LL115:
_temp114=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp110)->f1; goto _LL113;
_LL113: _temp112=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp110)->f2;
goto _LL83;} else{ goto _LL84;}} else{ goto _LL84;} _LL84: if(*(( int*) _temp78)
== Cyc_Absyn_Const_e){ _LL117: _temp116=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp78)->f1; if(( unsigned int) _temp116 > 1u?*(( int*) _temp116) == Cyc_Absyn_Int_c:
0){ _LL121: _temp120=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp116)->f1;
goto _LL119; _LL119: _temp118=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp116)->f2;
goto _LL85;} else{ goto _LL86;}} else{ goto _LL86;} _LL86: if(*(( int*) _temp78)
== Cyc_Absyn_Const_e){ _LL123: _temp122=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp78)->f1; if(( unsigned int) _temp122 > 1u?*(( int*) _temp122) == Cyc_Absyn_LongLong_c:
0){ _LL127: _temp126=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp122)->f1;
goto _LL125; _LL125: _temp124=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp122)->f2; goto _LL87;} else{ goto _LL88;}} else{ goto _LL88;} _LL88: if(*((
int*) _temp78) == Cyc_Absyn_Const_e){ _LL129: _temp128=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp78)->f1; if( _temp128 ==( void*) Cyc_Absyn_Null_c){ goto _LL89;} else{ goto
_LL90;}} else{ goto _LL90;} _LL90: if(*(( int*) _temp78) == Cyc_Absyn_Cast_e){
_LL133: _temp132=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp78)->f1; goto
_LL131; _LL131: _temp130=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp78)->f2; goto _LL91;} else{ goto _LL92;} _LL92: if(*(( int*) _temp78) ==
Cyc_Absyn_Tuple_e){ _LL135: _temp134=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp78)->f1; goto _LL93;} else{ goto _LL94;} _LL94: if(*(( int*) _temp78) ==
Cyc_Absyn_Array_e){ _LL137: _temp136=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp78)->f1; goto _LL95;} else{ goto _LL96;} _LL96: if(*(( int*) _temp78) ==
Cyc_Absyn_Struct_e){ _LL145: _temp144=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp78)->f1; goto _LL143; _LL143: _temp142=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp78)->f2; goto _LL141; _LL141: _temp140=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp78)->f3; goto _LL139; _LL139: _temp138=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp78)->f4; goto
_LL97;} else{ goto _LL98;} _LL98: if(*(( int*) _temp78) == Cyc_Absyn_CompoundLit_e){
_LL149: _temp148=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp78)->f1; goto _LL147; _LL147: _temp146=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp78)->f2; goto _LL99;} else{ goto _LL100;}
_LL100: if(*(( int*) _temp78) == Cyc_Absyn_UnresolvedMem_e){ _LL153: _temp152=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp78)->f1;
goto _LL151; _LL151: _temp150=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp78)->f2; goto _LL101;} else{ goto _LL102;} _LL102: goto _LL103; _LL81:
return _temp106 =='\000'; _LL83: return _temp112 == 0; _LL85: return _temp118 ==
0; _LL87: return _temp124 == 0; _LL89: return 1; _LL91: return Cyc_Toc_is_zero(
_temp130); _LL93: return(( int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct
Cyc_List_List* x)) Cyc_List_forall)( Cyc_Toc_is_zero, _temp134); _LL95: _temp140=
_temp136; goto _LL97; _LL97: _temp146= _temp140; goto _LL99; _LL99: _temp150=
_temp146; goto _LL101; _LL101: for( 0; _temp150 != 0; _temp150=({ struct Cyc_List_List*
_temp154= _temp150; if( _temp154 == 0){ _throw( Null_Exception);} _temp154->tl;})){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)({ struct Cyc_List_List* _temp155=
_temp150; if( _temp155 == 0){ _throw( Null_Exception);} _temp155->hd;}))).f2)){
return 0;}} return 1; _LL103: return 0; _LL79:;} static int Cyc_Toc_is_nullable(
void* t){ void* _temp156= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp162; struct Cyc_Absyn_Conref* _temp164; struct Cyc_Absyn_Tqual _temp166;
struct Cyc_Absyn_Conref* _temp168; void* _temp170; void* _temp172; _LL158: if((
unsigned int) _temp156 > 4u?*(( int*) _temp156) == Cyc_Absyn_PointerType: 0){
_LL163: _temp162=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp156)->f1; _LL173: _temp172=( void*) _temp162.elt_typ; goto _LL171; _LL171:
_temp170=( void*) _temp162.rgn_typ; goto _LL169; _LL169: _temp168=( struct Cyc_Absyn_Conref*)
_temp162.nullable; goto _LL167; _LL167: _temp166=( struct Cyc_Absyn_Tqual)
_temp162.tq; goto _LL165; _LL165: _temp164=( struct Cyc_Absyn_Conref*) _temp162.bounds;
goto _LL159;} else{ goto _LL160;} _LL160: goto _LL161; _LL159: { void* _temp174=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp168))->v; int _temp180; _LL176: if(( unsigned int) _temp174 > 1u?*(( int*)
_temp174) == Cyc_Absyn_Eq_constr: 0){ _LL181: _temp180=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp174)->f1; goto _LL177;} else{ goto _LL178;} _LL178: goto _LL179; _LL177:
return _temp180; _LL179: return 0; _LL175:;} _LL161:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp182=(
char*)"is_nullable"; struct _tagged_string _temp183; _temp183.curr= _temp182;
_temp183.base= _temp182; _temp183.last_plus_one= _temp182 + 12; _temp183;}));
return 0; _LL157:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct
_tuple0* x, struct _tagged_string tag){ return({ struct _tuple0* _temp184=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp184->f1=(* x).f1;
_temp184->f2=({ struct _tagged_string* _temp185=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp185[ 0]= Cyc_String_strconcat(*(*
x).f2, tag); _temp185;}); _temp184;});} static void Cyc_Toc_add_tuple_decl(
struct _tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List* fs= 0;{
int i= 1; for( 0; ts != 0; ts=({ struct Cyc_List_List* _temp186= ts; if(
_temp186 == 0){ _throw( Null_Exception);} _temp186->tl;}), i ++){ fs=({ struct
Cyc_List_List* _temp187=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp187->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp188=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp188->name=({ struct
_tagged_string* _temp190=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp190[ 0]= xprintf("f%d", i); _temp190;}); _temp188->tq=
Cyc_Toc_mt_tq; _temp188->type=( void*)(( void*)({ struct Cyc_List_List* _temp189=
ts; if( _temp189 == 0){ _throw( Null_Exception);} _temp189->hd;})); _temp188->width=
0; _temp188->attributes= 0; _temp188;}); _temp187->tl= fs; _temp187;});}} fs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);{
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp194=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp194->sc=( void*)(( void*) Cyc_Absyn_Public); _temp194->name=({ struct Cyc_Core_Opt*
_temp196=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp196->v=( void*)({ struct _tuple0* _temp197=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp197->f1= Cyc_Toc_abs_ns; _temp197->f2= x;
_temp197;}); _temp196;}); _temp194->tvs= 0; _temp194->fields=({ struct Cyc_Core_Opt*
_temp195=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp195->v=( void*) fs; _temp195;}); _temp194->attributes= 0; _temp194;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp191=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp191->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp192=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp192[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp193; _temp193.tag= Cyc_Absyn_Struct_d; _temp193.f1=
sd; _temp193;}); _temp192;}), 0); _temp191->tl= Cyc_Toc_result_decls; _temp191;});}}
static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x, void* t){
void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Structfield* f1=({ struct Cyc_Absyn_Structfield* _temp210=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp210->name= Cyc_Toc_curr_sp; _temp210->tq= Cyc_Toc_mt_tq; _temp210->type=(
void*) tptr; _temp210->width= 0; _temp210->attributes= 0; _temp210;}); struct
Cyc_Absyn_Structfield* f2=({ struct Cyc_Absyn_Structfield* _temp209=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp209->name= Cyc_Toc_base_sp;
_temp209->tq= Cyc_Toc_mt_tq; _temp209->type=( void*) tptr; _temp209->width= 0;
_temp209->attributes= 0; _temp209;}); struct Cyc_Absyn_Structfield* f3=({ struct
Cyc_Absyn_Structfield* _temp208=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp208->name= Cyc_Toc_last_plus_one_sp;
_temp208->tq= Cyc_Toc_mt_tq; _temp208->type=( void*) tptr; _temp208->width= 0;
_temp208->attributes= 0; _temp208;}); struct Cyc_List_List* fs=({ struct Cyc_List_List*
_temp205=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp205->hd=( void*) f1; _temp205->tl=({ struct Cyc_List_List* _temp206=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp206->hd=(
void*) f2; _temp206->tl=({ struct Cyc_List_List* _temp207=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp207->hd=( void*) f3; _temp207->tl=
0; _temp207;}); _temp206;}); _temp205;}); struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp201=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp201->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp201->name=({ struct Cyc_Core_Opt* _temp203=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp203->v=( void*)({ struct _tuple0*
_temp204=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp204->f1=
Cyc_Toc_abs_ns; _temp204->f2= x; _temp204;}); _temp203;}); _temp201->tvs= 0;
_temp201->fields=({ struct Cyc_Core_Opt* _temp202=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp202->v=( void*)(( struct Cyc_List_List*)
fs); _temp202;}); _temp201->attributes= 0; _temp201;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp198=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp198->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp199=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp199[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp200; _temp200.tag= Cyc_Absyn_Struct_d; _temp200.f1=
sd; _temp200;}); _temp199;}), 0); _temp198->tl= Cyc_Toc_result_decls; _temp198;});}
static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int Cyc_Toc_tuple_type_counter=
0; struct _tuple5{ struct _tagged_string* f1; struct Cyc_List_List* f2; } ;
static struct _tagged_string* Cyc_Toc_add_tuple_type( struct Cyc_List_List* tqs0){{
struct Cyc_List_List* tts= Cyc_Toc_tuple_types; for( 0; tts != 0; tts=({ struct
Cyc_List_List* _temp211= tts; if( _temp211 == 0){ _throw( Null_Exception);}
_temp211->tl;})){ struct _tuple5 _temp215; struct Cyc_List_List* _temp216;
struct _tagged_string* _temp218; struct _tuple5* _temp213=( struct _tuple5*)({
struct Cyc_List_List* _temp212= tts; if( _temp212 == 0){ _throw( Null_Exception);}
_temp212->hd;}); _temp215=* _temp213; _LL219: _temp218= _temp215.f1; goto _LL217;
_LL217: _temp216= _temp215.f2; goto _LL214; _LL214: { int okay= 1; struct Cyc_List_List*
tqs= tqs0; for( 0; tqs != 0? _temp216 != 0: 0; tqs=({ struct Cyc_List_List*
_temp220= tqs; if( _temp220 == 0){ _throw( Null_Exception);} _temp220->tl;}),
_temp216=({ struct Cyc_List_List* _temp221= _temp216; if( _temp221 == 0){ _throw(
Null_Exception);} _temp221->tl;})){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)({
struct Cyc_List_List* _temp222= tqs; if( _temp222 == 0){ _throw( Null_Exception);}
_temp222->hd;}))).f2,( void*)({ struct Cyc_List_List* _temp223= _temp216; if(
_temp223 == 0){ _throw( Null_Exception);} _temp223->hd;}))){ okay= 0; break;}}
if(( ! okay? 1: tqs != 0)? 1: _temp216 != 0){ continue;} return _temp218;}}}{
struct _tagged_string* x=({ struct _tagged_string* _temp226=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp226[ 0]=
xprintf("_tuple%d",( Cyc_Toc_tuple_type_counter ++)); _temp226;}); struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); Cyc_Toc_add_tuple_decl( x, ts);
Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp224=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp224->hd=( void*)({ struct
_tuple5* _temp225=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp225->f1= x; _temp225->f2= ts; _temp225;}); _temp224->tl= Cyc_Toc_tuple_types;
_temp224;}); return x;}} static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types=
0; static int Cyc_Toc_tagged_ptr_counter= 0; static void* Cyc_Toc_tagged_string_type_v=(
void*) 0u; static void* Cyc_Toc_tagged_string_type(){ if( Cyc_Toc_tagged_string_type_v
==( void*) Cyc_Absyn_VoidType){ Cyc_Toc_tagged_string_type_v= Cyc_Absyn_strct(
Cyc_Toc__tagged_string_sp);} return Cyc_Toc_tagged_string_type_v;} struct
_tuple6{ struct _tagged_string* f1; void* f2; } ; static void* Cyc_Toc_add_tagged_ptr_type(
void* t, struct Cyc_Absyn_Tqual tq){{ void* _temp227= t; void* _temp233; void*
_temp235; _LL229: if(( unsigned int) _temp227 > 4u?*(( int*) _temp227) == Cyc_Absyn_IntType:
0){ _LL236: _temp235=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp227)->f1;
if( _temp235 ==( void*) Cyc_Absyn_Unsigned){ goto _LL234;} else{ goto _LL231;}
_LL234: _temp233=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp227)->f2; if(
_temp233 ==( void*) Cyc_Absyn_B1){ goto _LL230;} else{ goto _LL231;}} else{ goto
_LL231;} _LL231: goto _LL232; _LL230: return Cyc_Toc_tagged_string_type();
_LL232: goto _LL228; _LL228:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts=({ struct Cyc_List_List* _temp237= tts; if( _temp237 == 0){
_throw( Null_Exception);} _temp237->tl;})){ struct _tuple6 _temp241; void*
_temp242; struct _tagged_string* _temp244; struct _tuple6* _temp239=( struct
_tuple6*)({ struct Cyc_List_List* _temp238= tts; if( _temp238 == 0){ _throw(
Null_Exception);} _temp238->hd;}); _temp241=* _temp239; _LL245: _temp244=
_temp241.f1; goto _LL243; _LL243: _temp242= _temp241.f2; goto _LL240; _LL240:
if( ! Cyc_Tcutil_unify( t, _temp242)){ continue;} return Cyc_Absyn_strct(
_temp244);}}{ struct _tagged_string* x=({ struct _tagged_string* _temp248=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp248[
0]= xprintf("_tagged_ptr%d",( Cyc_Toc_tagged_ptr_counter ++)); _temp248;}); Cyc_Toc_add_tagged_ptr_decl(
x, t); Cyc_Toc_tagged_ptr_types=({ struct Cyc_List_List* _temp246=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp246->hd=( void*)({ struct
_tuple6* _temp247=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp247->f1= x; _temp247->f2= t; _temp247;}); _temp246->tl= Cyc_Toc_tagged_ptr_types;
_temp246;}); return Cyc_Absyn_strct( x);}} static int Cyc_Toc_temp_var_counter=
0; static struct _tuple0* Cyc_Toc_temp_var(){ return({ struct _tuple0* _temp249=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp249->f1= Cyc_Toc_abs_ns;
_temp249->f2=({ struct _tagged_string* _temp250=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp250[ 0]= xprintf("_temp%d",(
Cyc_Toc_temp_var_counter ++)); _temp250;}); _temp249;});} static int Cyc_Toc_fresh_label_counter=
0; static struct _tagged_string* Cyc_Toc_fresh_label(){ return({ struct
_tagged_string* _temp251=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp251[ 0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter
++)); _temp251;});} static void* Cyc_Toc_typ_to_c( void* t); static struct
_tuple1* Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp254; struct Cyc_Absyn_Tqual
_temp256; struct Cyc_Core_Opt* _temp258; struct _tuple1 _temp252=* a; _LL259:
_temp258= _temp252.f1; goto _LL257; _LL257: _temp256= _temp252.f2; goto _LL255;
_LL255: _temp254= _temp252.f3; goto _LL253; _LL253: return({ struct _tuple1*
_temp260=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp260->f1=
_temp258; _temp260->f2= _temp256; _temp260->f3= Cyc_Toc_typ_to_c( _temp254);
_temp260;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp263; struct Cyc_Absyn_Tqual _temp265; struct _tuple3 _temp261=* x;
_LL266: _temp265= _temp261.f1; goto _LL264; _LL264: _temp263= _temp261.f2; goto
_LL262; _LL262: return({ struct _tuple3* _temp267=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp267->f1= _temp265; _temp267->f2= Cyc_Toc_typ_to_c(
_temp263); _temp267;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp268= t; struct Cyc_Absyn_Exp* _temp276; struct Cyc_Absyn_Tqual _temp278;
void* _temp280; int _temp282; struct Cyc_Core_Opt* _temp284; struct Cyc_Core_Opt
_temp286; void* _temp287; void* _temp289; _LL270: if(( unsigned int) _temp268 >
4u?*(( int*) _temp268) == Cyc_Absyn_ArrayType: 0){ _LL281: _temp280=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp268)->f1; goto _LL279; _LL279: _temp278=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp268)->f2;
goto _LL277; _LL277: _temp276=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp268)->f3; goto _LL271;} else{ goto _LL272;} _LL272: if(( unsigned int)
_temp268 > 4u?*(( int*) _temp268) == Cyc_Absyn_Evar: 0){ _LL290: _temp289=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp268)->f1; goto _LL285; _LL285: _temp284=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp268)->f2; if(
_temp284 == 0){ goto _LL274;} else{ _temp286=* _temp284; _LL288: _temp287=( void*)
_temp286.v; goto _LL283;} _LL283: _temp282=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp268)->f3; goto _LL273;} else{ goto _LL274;} _LL274: goto _LL275; _LL271:
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp291=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp291[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp292; _temp292.tag= Cyc_Absyn_ArrayType; _temp292.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp280); _temp292.f2= _temp278; _temp292.f3=
_temp276; _temp292;}); _temp291;}); _LL273: return Cyc_Toc_typ_to_c_array(
_temp287); _LL275: return Cyc_Toc_typ_to_c( t); _LL269:;} static void* Cyc_Toc_typ_to_c(
void* t); static struct Cyc_Absyn_Structfield* Cyc_Toc_structfield_to_c( struct
Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield* _temp293=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp293->name= f->name; _temp293->tq= f->tq; _temp293->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp293->width= f->width; _temp293->attributes= f->attributes;
_temp293;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp294= t; int
_temp346; struct Cyc_Core_Opt* _temp348; void* _temp350; int _temp352; struct
Cyc_Core_Opt* _temp354; struct Cyc_Core_Opt _temp356; void* _temp357; void*
_temp359; struct Cyc_Absyn_Tvar* _temp361; struct Cyc_Absyn_TunionInfo _temp363;
void* _temp365; struct Cyc_List_List* _temp367; void* _temp369; struct Cyc_Absyn_Tuniondecl*
_temp371; struct Cyc_Absyn_TunionInfo _temp373; struct Cyc_Absyn_TunionFieldInfo
_temp375; struct Cyc_List_List* _temp377; void* _temp379; struct Cyc_Absyn_Tunionfield*
_temp381; struct Cyc_Absyn_Tuniondecl* _temp383; struct Cyc_Absyn_TunionFieldInfo
_temp385; struct Cyc_Absyn_PtrInfo _temp387; struct Cyc_Absyn_Conref* _temp389;
struct Cyc_Absyn_Tqual _temp391; struct Cyc_Absyn_Conref* _temp393; void*
_temp395; void* _temp397; void* _temp399; void* _temp401; struct Cyc_Absyn_Exp*
_temp403; struct Cyc_Absyn_Tqual _temp405; void* _temp407; struct Cyc_Absyn_FnInfo
_temp409; struct Cyc_List_List* _temp411; int _temp413; struct Cyc_List_List*
_temp415; void* _temp417; struct Cyc_Core_Opt* _temp419; struct Cyc_List_List*
_temp421; struct Cyc_List_List* _temp423; struct Cyc_List_List* _temp425; struct
Cyc_List_List* _temp427; struct Cyc_Absyn_Uniondecl** _temp429; struct Cyc_List_List*
_temp431; struct _tuple0* _temp433; struct Cyc_Absyn_Structdecl** _temp435;
struct Cyc_List_List* _temp437; struct _tuple0* _temp439; struct Cyc_Absyn_Enumdecl*
_temp441; struct _tuple0* _temp443; struct Cyc_Core_Opt* _temp445; struct Cyc_List_List*
_temp447; struct _tuple0* _temp449; void* _temp451; void* _temp453; struct Cyc_List_List*
_temp455; _LL296: if( _temp294 ==( void*) Cyc_Absyn_VoidType){ goto _LL297;}
else{ goto _LL298;} _LL298: if(( unsigned int) _temp294 > 4u?*(( int*) _temp294)
== Cyc_Absyn_Evar: 0){ _LL351: _temp350=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp294)->f1; goto _LL349; _LL349: _temp348=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp294)->f2; if( _temp348 == 0){ goto _LL347;} else{
goto _LL300;} _LL347: _temp346=( int)(( struct Cyc_Absyn_Evar_struct*) _temp294)->f3;
goto _LL299;} else{ goto _LL300;} _LL300: if(( unsigned int) _temp294 > 4u?*((
int*) _temp294) == Cyc_Absyn_Evar: 0){ _LL360: _temp359=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp294)->f1; goto _LL355; _LL355: _temp354=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp294)->f2; if( _temp354 == 0){ goto _LL302;} else{
_temp356=* _temp354; _LL358: _temp357=( void*) _temp356.v; goto _LL353;} _LL353:
_temp352=( int)(( struct Cyc_Absyn_Evar_struct*) _temp294)->f3; goto _LL301;}
else{ goto _LL302;} _LL302: if(( unsigned int) _temp294 > 4u?*(( int*) _temp294)
== Cyc_Absyn_VarType: 0){ _LL362: _temp361=( struct Cyc_Absyn_Tvar*)(( struct
Cyc_Absyn_VarType_struct*) _temp294)->f1; goto _LL303;} else{ goto _LL304;}
_LL304: if(( unsigned int) _temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_TunionType:
0){ _LL364: _temp363=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp294)->f1; _LL370: _temp369=( void*) _temp363.tunion_info; if(*(( int*)
_temp369) == Cyc_Absyn_KnownTunion){ _LL372: _temp371=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp369)->f1; goto _LL368;} else{ goto
_LL306;} _LL368: _temp367=( struct Cyc_List_List*) _temp363.targs; goto _LL366;
_LL366: _temp365=( void*) _temp363.rgn; goto _LL305;} else{ goto _LL306;} _LL306:
if(( unsigned int) _temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_TunionType: 0){
_LL374: _temp373=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp294)->f1; goto _LL307;} else{ goto _LL308;} _LL308: if(( unsigned int)
_temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_TunionFieldType: 0){ _LL376:
_temp375=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp294)->f1; _LL380: _temp379=( void*) _temp375.field_info; if(*(( int*)
_temp379) == Cyc_Absyn_KnownTunionfield){ _LL384: _temp383=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp379)->f1; goto _LL382; _LL382:
_temp381=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp379)->f2; goto _LL378;} else{ goto _LL310;} _LL378: _temp377=( struct Cyc_List_List*)
_temp375.targs; goto _LL309;} else{ goto _LL310;} _LL310: if(( unsigned int)
_temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_TunionFieldType: 0){ _LL386:
_temp385=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp294)->f1; goto _LL311;} else{ goto _LL312;} _LL312: if(( unsigned int)
_temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_PointerType: 0){ _LL388: _temp387=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp294)->f1;
_LL398: _temp397=( void*) _temp387.elt_typ; goto _LL396; _LL396: _temp395=( void*)
_temp387.rgn_typ; goto _LL394; _LL394: _temp393=( struct Cyc_Absyn_Conref*)
_temp387.nullable; goto _LL392; _LL392: _temp391=( struct Cyc_Absyn_Tqual)
_temp387.tq; goto _LL390; _LL390: _temp389=( struct Cyc_Absyn_Conref*) _temp387.bounds;
goto _LL313;} else{ goto _LL314;} _LL314: if(( unsigned int) _temp294 > 4u?*((
int*) _temp294) == Cyc_Absyn_IntType: 0){ _LL402: _temp401=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp294)->f1; goto _LL400; _LL400: _temp399=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp294)->f2; goto _LL315;} else{ goto _LL316;} _LL316: if( _temp294 ==( void*)
Cyc_Absyn_FloatType){ goto _LL317;} else{ goto _LL318;} _LL318: if( _temp294 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL319;} else{ goto _LL320;} _LL320: if((
unsigned int) _temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_ArrayType: 0){
_LL408: _temp407=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp294)->f1;
goto _LL406; _LL406: _temp405=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp294)->f2; goto _LL404; _LL404: _temp403=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp294)->f3; goto _LL321;} else{ goto _LL322;}
_LL322: if(( unsigned int) _temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_FnType:
0){ _LL410: _temp409=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp294)->f1; _LL422: _temp421=( struct Cyc_List_List*) _temp409.tvars; goto
_LL420; _LL420: _temp419=( struct Cyc_Core_Opt*) _temp409.effect; goto _LL418;
_LL418: _temp417=( void*) _temp409.ret_typ; goto _LL416; _LL416: _temp415=(
struct Cyc_List_List*) _temp409.args; goto _LL414; _LL414: _temp413=( int)
_temp409.varargs; goto _LL412; _LL412: _temp411=( struct Cyc_List_List*)
_temp409.attributes; goto _LL323;} else{ goto _LL324;} _LL324: if(( unsigned int)
_temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_TupleType: 0){ _LL424: _temp423=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp294)->f1; goto
_LL325;} else{ goto _LL326;} _LL326: if(( unsigned int) _temp294 > 4u?*(( int*)
_temp294) == Cyc_Absyn_AnonStructType: 0){ _LL426: _temp425=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp294)->f1; goto _LL327;} else{ goto
_LL328;} _LL328: if(( unsigned int) _temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_AnonUnionType:
0){ _LL428: _temp427=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp294)->f1; goto _LL329;} else{ goto _LL330;} _LL330: if(( unsigned int)
_temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_UnionType: 0){ _LL434: _temp433=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp294)->f1; goto
_LL432; _LL432: _temp431=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp294)->f2; goto _LL430; _LL430: _temp429=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp294)->f3; goto _LL331;} else{ goto
_LL332;} _LL332: if(( unsigned int) _temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_StructType:
0){ _LL440: _temp439=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp294)->f1; goto _LL438; _LL438: _temp437=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp294)->f2; goto _LL436; _LL436: _temp435=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp294)->f3;
goto _LL333;} else{ goto _LL334;} _LL334: if(( unsigned int) _temp294 > 4u?*((
int*) _temp294) == Cyc_Absyn_EnumType: 0){ _LL444: _temp443=( struct _tuple0*)((
struct Cyc_Absyn_EnumType_struct*) _temp294)->f1; goto _LL442; _LL442: _temp441=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp294)->f2;
goto _LL335;} else{ goto _LL336;} _LL336: if(( unsigned int) _temp294 > 4u?*((
int*) _temp294) == Cyc_Absyn_TypedefType: 0){ _LL450: _temp449=( struct _tuple0*)((
struct Cyc_Absyn_TypedefType_struct*) _temp294)->f1; goto _LL448; _LL448:
_temp447=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp294)->f2; goto _LL446; _LL446: _temp445=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp294)->f3; goto _LL337;} else{ goto _LL338;}
_LL338: if(( unsigned int) _temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_RgnHandleType:
0){ _LL452: _temp451=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp294)->f1;
goto _LL339;} else{ goto _LL340;} _LL340: if( _temp294 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL341;} else{ goto _LL342;} _LL342: if(( unsigned int) _temp294 > 4u?*((
int*) _temp294) == Cyc_Absyn_AccessEff: 0){ _LL454: _temp453=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp294)->f1; goto _LL343;} else{ goto _LL344;}
_LL344: if(( unsigned int) _temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_JoinEff:
0){ _LL456: _temp455=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp294)->f1; goto _LL345;} else{ goto _LL295;} _LL297: return t; _LL299:
return( void*) Cyc_Absyn_VoidType; _LL301: return Cyc_Toc_typ_to_c( _temp357);
_LL303: return Cyc_Absyn_void_star_typ(); _LL305: return Cyc_Absyn_void_star_typ();
_LL307:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp457=( char*)"unresolved TunionType"; struct
_tagged_string _temp458; _temp458.curr= _temp457; _temp458.base= _temp457;
_temp458.last_plus_one= _temp457 + 22; _temp458;}))); _LL309: if( _temp381->typs
== 0){ if( _temp383->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp381->name,(
struct _tagged_string)({ char* _temp459=( char*)"_struct"; struct _tagged_string
_temp460; _temp460.curr= _temp459; _temp460.base= _temp459; _temp460.last_plus_one=
_temp459 + 8; _temp460;})));} _LL311:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp461=(
char*)"unresolved TunionFieldType"; struct _tagged_string _temp462; _temp462.curr=
_temp461; _temp462.base= _temp461; _temp462.last_plus_one= _temp461 + 27;
_temp462;}))); _LL313: _temp397= Cyc_Toc_typ_to_c_array( _temp397);{ void*
_temp463=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp389))->v; void* _temp469; _LL465: if((
unsigned int) _temp463 > 1u?*(( int*) _temp463) == Cyc_Absyn_Eq_constr: 0){
_LL470: _temp469=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp463)->f1;
if( _temp469 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL466;} else{ goto _LL467;}}
else{ goto _LL467;} _LL467: goto _LL468; _LL466: return Cyc_Toc_add_tagged_ptr_type(
_temp397, _temp391); _LL468: return Cyc_Absyn_star_typ( _temp397,( void*) Cyc_Absyn_HeapRgn,
_temp391); _LL464:;} _LL315: return t; _LL317: return t; _LL319: return t;
_LL321: return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp471=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp471[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp472; _temp472.tag= Cyc_Absyn_ArrayType;
_temp472.f1=( void*) Cyc_Toc_typ_to_c_array( _temp407); _temp472.f2= _temp405;
_temp472.f3= _temp403; _temp472;}); _temp471;}); _LL323: { struct Cyc_List_List*
new_atts= 0; for( 0; _temp411 != 0; _temp411=({ struct Cyc_List_List* _temp473=
_temp411; if( _temp473 == 0){ _throw( Null_Exception);} _temp473->tl;})){ void*
_temp475=( void*)({ struct Cyc_List_List* _temp474= _temp411; if( _temp474 == 0){
_throw( Null_Exception);} _temp474->hd;}); _LL477: if( _temp475 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL478;} else{ goto _LL479;} _LL479: if( _temp475 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL480;} else{ goto _LL481;} _LL481: if( _temp475 ==( void*) Cyc_Absyn_Const_att){
goto _LL482;} else{ goto _LL483;} _LL483: goto _LL484; _LL478: goto _LL480;
_LL480: goto _LL482; _LL482: continue; _LL484: new_atts=({ struct Cyc_List_List*
_temp485=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp485->hd=( void*)(( void*)({ struct Cyc_List_List* _temp486= _temp411; if(
_temp486 == 0){ _throw( Null_Exception);} _temp486->hd;})); _temp485->tl=
new_atts; _temp485;}); goto _LL476; _LL476:;} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp487=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp487[ 0]=({ struct Cyc_Absyn_FnType_struct _temp488; _temp488.tag= Cyc_Absyn_FnType;
_temp488.f1=({ struct Cyc_Absyn_FnInfo _temp489; _temp489.tvars= 0; _temp489.effect=
0; _temp489.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp417); _temp489.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp415); _temp489.varargs= _temp413;
_temp489.attributes= new_atts; _temp489;}); _temp488;}); _temp487;});} _LL325:
_temp423=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp423);{ struct
_tagged_string* n= Cyc_Toc_add_tuple_type( _temp423); return Cyc_Absyn_strct( n);}
_LL327: return( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp490=(
struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp490[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp491; _temp491.tag=
Cyc_Absyn_AnonStructType; _temp491.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp425); _temp491;}); _temp490;}); _LL329: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp492=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp492[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp493; _temp493.tag=
Cyc_Absyn_AnonUnionType; _temp493.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp427); _temp493;}); _temp492;}); _LL331: if( _temp433 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp494=( char*)"anonymous union"; struct _tagged_string _temp495; _temp495.curr=
_temp494; _temp495.base= _temp494; _temp495.last_plus_one= _temp494 + 16;
_temp495;}));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)({ struct _tuple0*
_temp496= _temp433; if( _temp496 == 0){ _throw( Null_Exception);} _temp496;}));
_LL333: if( _temp439 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp497=( char*)"anonymous struct"; struct
_tagged_string _temp498; _temp498.curr= _temp497; _temp498.base= _temp497;
_temp498.last_plus_one= _temp497 + 17; _temp498;}));} return Cyc_Absyn_strctq((
struct _tuple0*)({ struct _tuple0* _temp499= _temp439; if( _temp499 == 0){
_throw( Null_Exception);} _temp499;})); _LL335: return t; _LL337: if( _temp445
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp500=( char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp501; _temp501.curr= _temp500; _temp501.base= _temp500;
_temp501.last_plus_one= _temp500 + 38; _temp501;}));} return( void*)({ struct
Cyc_Absyn_TypedefType_struct* _temp502=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp502[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp503; _temp503.tag= Cyc_Absyn_TypedefType;
_temp503.f1= _temp449; _temp503.f2= 0; _temp503.f3=({ struct Cyc_Core_Opt*
_temp504=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp504->v=( void*) Cyc_Toc_typ_to_c_array(( void*)({ struct Cyc_Core_Opt*
_temp505= _temp445; if( _temp505 == 0){ _throw( Null_Exception);} _temp505->v;}));
_temp504;}); _temp503;}); _temp502;}); _LL339: return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__region_handle_sp), Cyc_Toc_mt_tq); _LL341: return(( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp506=(
char*)"Toc::typ_to_c: type translation passed the heap region"; struct
_tagged_string _temp507; _temp507.curr= _temp506; _temp507.base= _temp506;
_temp507.last_plus_one= _temp506 + 55; _temp507;})); _LL343: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp508=( char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp509; _temp509.curr= _temp508; _temp509.base= _temp508;
_temp509.last_plus_one= _temp508 + 54; _temp509;})); _LL345: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp510=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp511; _temp511.curr= _temp510; _temp511.base= _temp510;
_temp511.last_plus_one= _temp510 + 52; _temp511;})); _LL295:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp512= t; struct Cyc_Absyn_Exp* _temp518; struct Cyc_Absyn_Tqual
_temp520; void* _temp522; _LL514: if(( unsigned int) _temp512 > 4u?*(( int*)
_temp512) == Cyc_Absyn_ArrayType: 0){ _LL523: _temp522=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp512)->f1; goto _LL521; _LL521: _temp520=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp512)->f2; goto _LL519; _LL519: _temp518=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp512)->f3; goto
_LL515;} else{ goto _LL516;} _LL516: goto _LL517; _LL515: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp522,( void*) Cyc_Absyn_HeapRgn, _temp520), e, l);
_LL517: return e; _LL513:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp524= t; struct Cyc_Absyn_Exp* _temp530; struct Cyc_Absyn_Tqual _temp532;
void* _temp534; _LL526: if(( unsigned int) _temp524 > 4u?*(( int*) _temp524) ==
Cyc_Absyn_ArrayType: 0){ _LL535: _temp534=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp524)->f1; goto _LL533; _LL533: _temp532=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp524)->f2; goto _LL531; _LL531: _temp530=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp524)->f3; goto
_LL527;} else{ goto _LL528;} _LL528: goto _LL529; _LL527: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp534,( void*) Cyc_Absyn_HeapRgn, _temp532), e, l);
_LL529: return Cyc_Absyn_cast_exp( t, e, l); _LL525:;} static int Cyc_Toc_atomic_typ(
void* t){ void* _temp536= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp572; void* _temp574; void* _temp576; struct Cyc_Absyn_Exp* _temp578; struct
Cyc_Absyn_Tqual _temp580; void* _temp582; struct Cyc_Absyn_FnInfo _temp584;
struct Cyc_List_List* _temp586; struct Cyc_Absyn_Structdecl** _temp588; struct
Cyc_List_List* _temp590; struct _tuple0* _temp592; struct Cyc_Absyn_TunionInfo
_temp594; struct Cyc_List_List* _temp596; struct Cyc_List_List* _temp598; struct
Cyc_Absyn_TunionFieldInfo _temp600; struct Cyc_List_List* _temp602; void*
_temp604; struct Cyc_Absyn_Tunionfield* _temp606; struct Cyc_Absyn_Tuniondecl*
_temp608; struct Cyc_Absyn_Uniondecl** _temp610; struct Cyc_List_List* _temp612;
struct _tuple0* _temp614; struct Cyc_Absyn_PtrInfo _temp616; void* _temp618;
_LL538: if( _temp536 ==( void*) Cyc_Absyn_VoidType){ goto _LL539;} else{ goto
_LL540;} _LL540: if(( unsigned int) _temp536 > 4u?*(( int*) _temp536) == Cyc_Absyn_VarType:
0){ _LL573: _temp572=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp536)->f1; goto _LL541;} else{ goto _LL542;} _LL542: if(( unsigned int)
_temp536 > 4u?*(( int*) _temp536) == Cyc_Absyn_IntType: 0){ _LL577: _temp576=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp536)->f1; goto _LL575; _LL575:
_temp574=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp536)->f2; goto _LL543;}
else{ goto _LL544;} _LL544: if( _temp536 ==( void*) Cyc_Absyn_FloatType){ goto
_LL545;} else{ goto _LL546;} _LL546: if( _temp536 ==( void*) Cyc_Absyn_DoubleType){
goto _LL547;} else{ goto _LL548;} _LL548: if(( unsigned int) _temp536 > 4u?*((
int*) _temp536) == Cyc_Absyn_ArrayType: 0){ _LL583: _temp582=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp536)->f1; goto _LL581; _LL581: _temp580=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp536)->f2;
goto _LL579; _LL579: _temp578=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp536)->f3; goto _LL549;} else{ goto _LL550;} _LL550: if(( unsigned int)
_temp536 > 4u?*(( int*) _temp536) == Cyc_Absyn_FnType: 0){ _LL585: _temp584=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp536)->f1; goto
_LL551;} else{ goto _LL552;} _LL552: if(( unsigned int) _temp536 > 4u?*(( int*)
_temp536) == Cyc_Absyn_TupleType: 0){ _LL587: _temp586=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp536)->f1; goto _LL553;} else{ goto
_LL554;} _LL554: if(( unsigned int) _temp536 > 4u?*(( int*) _temp536) == Cyc_Absyn_StructType:
0){ _LL593: _temp592=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp536)->f1; goto _LL591; _LL591: _temp590=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp536)->f2; goto _LL589; _LL589: _temp588=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp536)->f3;
goto _LL555;} else{ goto _LL556;} _LL556: if(( unsigned int) _temp536 > 4u?*((
int*) _temp536) == Cyc_Absyn_TunionType: 0){ _LL595: _temp594=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp536)->f1; goto _LL557;} else{ goto
_LL558;} _LL558: if(( unsigned int) _temp536 > 4u?*(( int*) _temp536) == Cyc_Absyn_AnonStructType:
0){ _LL597: _temp596=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp536)->f1; goto _LL559;} else{ goto _LL560;} _LL560: if(( unsigned int)
_temp536 > 4u?*(( int*) _temp536) == Cyc_Absyn_AnonUnionType: 0){ _LL599:
_temp598=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp536)->f1; goto _LL561;} else{ goto _LL562;} _LL562: if(( unsigned int)
_temp536 > 4u?*(( int*) _temp536) == Cyc_Absyn_TunionFieldType: 0){ _LL601:
_temp600=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp536)->f1; _LL605: _temp604=( void*) _temp600.field_info; if(*(( int*)
_temp604) == Cyc_Absyn_KnownTunionfield){ _LL609: _temp608=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp604)->f1; goto _LL607; _LL607:
_temp606=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp604)->f2; goto _LL603;} else{ goto _LL564;} _LL603: _temp602=( struct Cyc_List_List*)
_temp600.targs; goto _LL563;} else{ goto _LL564;} _LL564: if(( unsigned int)
_temp536 > 4u?*(( int*) _temp536) == Cyc_Absyn_UnionType: 0){ _LL615: _temp614=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp536)->f1; goto
_LL613; _LL613: _temp612=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp536)->f2; goto _LL611; _LL611: _temp610=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp536)->f3; goto _LL565;} else{ goto
_LL566;} _LL566: if(( unsigned int) _temp536 > 4u?*(( int*) _temp536) == Cyc_Absyn_PointerType:
0){ _LL617: _temp616=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp536)->f1; goto _LL567;} else{ goto _LL568;} _LL568: if(( unsigned int)
_temp536 > 4u?*(( int*) _temp536) == Cyc_Absyn_RgnHandleType: 0){ _LL619:
_temp618=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp536)->f1; goto
_LL569;} else{ goto _LL570;} _LL570: goto _LL571; _LL539: return 1; _LL541:
return 0; _LL543: return 1; _LL545: return 1; _LL547: return 0; _LL549: return
Cyc_Toc_atomic_typ( _temp582); _LL551: return 1; _LL553: for( 0; _temp586 != 0;
_temp586=({ struct Cyc_List_List* _temp620= _temp586; if( _temp620 == 0){ _throw(
Null_Exception);} _temp620->tl;})){ if( ! Cyc_Toc_atomic_typ((*(( struct _tuple3*)({
struct Cyc_List_List* _temp621= _temp586; if( _temp621 == 0){ _throw(
Null_Exception);} _temp621->hd;}))).f2)){ return 0;}} return 1; _LL555: return 0;
_LL557: return 0; _LL559: _temp598= _temp596; goto _LL561; _LL561: for( 0;
_temp598 != 0; _temp598=({ struct Cyc_List_List* _temp622= _temp598; if(
_temp622 == 0){ _throw( Null_Exception);} _temp622->tl;})){ if( ! Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp623=
_temp598; if( _temp623 == 0){ _throw( Null_Exception);} _temp623->hd;}))->type)){
return 0;}} return 1; _LL563: return _temp606->typs == 0; _LL565: return 0;
_LL567: return 0; _LL569: return 0; _LL571:(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp624= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _temp624.last_plus_one - _temp624.curr,
_temp624.curr);})); return 0; _LL537:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp625= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp631;
struct Cyc_Absyn_Conref* _temp633; struct Cyc_Absyn_Tqual _temp635; struct Cyc_Absyn_Conref*
_temp637; void* _temp639; void* _temp641; _LL627: if(( unsigned int) _temp625 >
4u?*(( int*) _temp625) == Cyc_Absyn_PointerType: 0){ _LL632: _temp631=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp625)->f1; _LL642:
_temp641=( void*) _temp631.elt_typ; goto _LL640; _LL640: _temp639=( void*)
_temp631.rgn_typ; goto _LL638; _LL638: _temp637=( struct Cyc_Absyn_Conref*)
_temp631.nullable; goto _LL636; _LL636: _temp635=( struct Cyc_Absyn_Tqual)
_temp631.tq; goto _LL634; _LL634: _temp633=( struct Cyc_Absyn_Conref*) _temp631.bounds;
goto _LL628;} else{ goto _LL629;} _LL629: goto _LL630; _LL628: { void* _temp643=
Cyc_Tcutil_compress( _temp641); _LL645: if( _temp643 ==( void*) Cyc_Absyn_VoidType){
goto _LL646;} else{ goto _LL647;} _LL647: goto _LL648; _LL646: return 1; _LL648:
return 0; _LL644:;} _LL630: return 0; _LL626:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp649= Cyc_Tcutil_compress( t);
struct Cyc_List_List* _temp665; struct Cyc_List_List* _temp667; struct Cyc_Absyn_Structdecl**
_temp669; struct Cyc_Absyn_Structdecl* _temp671; struct Cyc_List_List* _temp672;
struct _tuple0* _temp674; struct Cyc_Absyn_Structdecl** _temp676; struct Cyc_List_List*
_temp678; struct _tuple0* _temp680; struct Cyc_Absyn_Uniondecl** _temp682;
struct Cyc_Absyn_Uniondecl* _temp684; struct Cyc_List_List* _temp685; struct
_tuple0* _temp687; struct Cyc_Absyn_Uniondecl** _temp689; struct Cyc_List_List*
_temp691; struct _tuple0* _temp693; _LL651: if(( unsigned int) _temp649 > 4u?*((
int*) _temp649) == Cyc_Absyn_AnonUnionType: 0){ _LL666: _temp665=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp649)->f1; goto _LL652;} else{ goto
_LL653;} _LL653: if(( unsigned int) _temp649 > 4u?*(( int*) _temp649) == Cyc_Absyn_AnonStructType:
0){ _LL668: _temp667=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp649)->f1; goto _LL654;} else{ goto _LL655;} _LL655: if(( unsigned int)
_temp649 > 4u?*(( int*) _temp649) == Cyc_Absyn_StructType: 0){ _LL675: _temp674=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp649)->f1; goto
_LL673; _LL673: _temp672=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp649)->f2; goto _LL670; _LL670: _temp669=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp649)->f3; if( _temp669 == 0){ goto
_LL657;} else{ _temp671=* _temp669; goto _LL656;}} else{ goto _LL657;} _LL657:
if(( unsigned int) _temp649 > 4u?*(( int*) _temp649) == Cyc_Absyn_StructType: 0){
_LL681: _temp680=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp649)->f1; goto _LL679; _LL679: _temp678=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp649)->f2; goto _LL677; _LL677: _temp676=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp649)->f3;
if( _temp676 == 0){ goto _LL658;} else{ goto _LL659;}} else{ goto _LL659;}
_LL659: if(( unsigned int) _temp649 > 4u?*(( int*) _temp649) == Cyc_Absyn_UnionType:
0){ _LL688: _temp687=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp649)->f1; goto _LL686; _LL686: _temp685=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp649)->f2; goto _LL683; _LL683: _temp682=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp649)->f3;
if( _temp682 == 0){ goto _LL661;} else{ _temp684=* _temp682; goto _LL660;}}
else{ goto _LL661;} _LL661: if(( unsigned int) _temp649 > 4u?*(( int*) _temp649)
== Cyc_Absyn_UnionType: 0){ _LL694: _temp693=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp649)->f1; goto _LL692; _LL692: _temp691=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp649)->f2; goto _LL690; _LL690: _temp689=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp649)->f3;
if( _temp689 == 0){ goto _LL662;} else{ goto _LL663;}} else{ goto _LL663;}
_LL663: goto _LL664; _LL652: _temp667= _temp665; goto _LL654; _LL654: { struct
Cyc_Absyn_Structfield* field= Cyc_Absyn_lookup_field( _temp667, f); if( field ==
0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp696=*({ struct Cyc_Absyn_Structfield* _temp695= field; if(
_temp695 == 0){ _throw( Null_Exception);} _temp695->name;}); xprintf("is_poly_field: bad field %.*s",
_temp696.last_plus_one - _temp696.curr, _temp696.curr);}));} return Cyc_Toc_is_void_star((
void*)({ struct Cyc_Absyn_Structfield* _temp697= field; if( _temp697 == 0){
_throw( Null_Exception);} _temp697->type;}));} _LL656: if( _temp674 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp698=( char*)"StructType name not properly set"; struct _tagged_string
_temp699; _temp699.curr= _temp698; _temp699.base= _temp698; _temp699.last_plus_one=
_temp698 + 33; _temp699;}));} if( _temp671->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp701= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp700= _temp674; if( _temp700 == 0){
_throw( Null_Exception);} _temp700;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp701.last_plus_one - _temp701.curr, _temp701.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp708= _temp671->fields;
if( _temp708 == 0){ _throw( Null_Exception);} _temp708->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp702= fields; if( _temp702 == 0){ _throw(
Null_Exception);} _temp702->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp703= fields; if( _temp703 == 0){ _throw(
Null_Exception);} _temp703->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp704= fields;
if( _temp704 == 0){ _throw( Null_Exception);} _temp704->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp706=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp705= _temp674;
if( _temp705 == 0){ _throw( Null_Exception);} _temp705;})); struct
_tagged_string _temp707=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp706.last_plus_one - _temp706.curr, _temp706.curr, _temp707.last_plus_one -
_temp707.curr, _temp707.curr);})); return 0;} _LL658: if( _temp680 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp709=( char*)"StructType name not properly set"; struct _tagged_string
_temp710; _temp710.curr= _temp709; _temp710.base= _temp709; _temp710.last_plus_one=
_temp709 + 33; _temp710;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp712= Cyc_Absynpp_qvar2string(( struct _tuple0*)({
struct _tuple0* _temp711= _temp680; if( _temp711 == 0){ _throw( Null_Exception);}
_temp711;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp712.last_plus_one
- _temp712.curr, _temp712.curr);})); return 0; _LL660: if( _temp687 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp713=( char*)"UnionType name not properly set"; struct _tagged_string
_temp714; _temp714.curr= _temp713; _temp714.base= _temp713; _temp714.last_plus_one=
_temp713 + 32; _temp714;}));} if( _temp684->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp716= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp715= _temp687; if( _temp715 == 0){
_throw( Null_Exception);} _temp715;})); xprintf("is_poly_field:  union %.*s missing fields",
_temp716.last_plus_one - _temp716.curr, _temp716.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp723= _temp684->fields;
if( _temp723 == 0){ _throw( Null_Exception);} _temp723->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp717= fields; if( _temp717 == 0){ _throw(
Null_Exception);} _temp717->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp718= fields; if( _temp718 == 0){ _throw(
Null_Exception);} _temp718->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp719= fields;
if( _temp719 == 0){ _throw( Null_Exception);} _temp719->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp721=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp720= _temp687;
if( _temp720 == 0){ _throw( Null_Exception);} _temp720;})); struct
_tagged_string _temp722=* f; xprintf("is_poly_field:  union %.*s has no %.*s field",
_temp721.last_plus_one - _temp721.curr, _temp721.curr, _temp722.last_plus_one -
_temp722.curr, _temp722.curr);})); return 0;} _LL662: if( _temp693 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp724=( char*)"UnionType name not properly set"; struct _tagged_string
_temp725; _temp725.curr= _temp724; _temp725.base= _temp724; _temp725.last_plus_one=
_temp724 + 32; _temp725;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp727= Cyc_Absynpp_qvar2string(( struct _tuple0*)({
struct _tuple0* _temp726= _temp693; if( _temp726 == 0){ _throw( Null_Exception);}
_temp726;})); xprintf("is_poly_field:  union %.*s missing structdecl", _temp727.last_plus_one
- _temp727.curr, _temp727.curr);})); return 0; _LL664:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp728= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp728.last_plus_one - _temp728.curr,
_temp728.curr);})); return 0; _LL650:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp729=( void*) e->r; struct _tagged_string*
_temp737; struct Cyc_Absyn_Exp* _temp739; struct _tagged_string* _temp741;
struct Cyc_Absyn_Exp* _temp743; _LL731: if(*(( int*) _temp729) == Cyc_Absyn_StructMember_e){
_LL740: _temp739=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp729)->f1; goto _LL738; _LL738: _temp737=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp729)->f2; goto _LL732;} else{ goto _LL733;}
_LL733: if(*(( int*) _temp729) == Cyc_Absyn_StructArrow_e){ _LL744: _temp743=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp729)->f1;
goto _LL742; _LL742: _temp741=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp729)->f2; goto _LL734;} else{ goto _LL735;} _LL735: goto _LL736; _LL732:
return Cyc_Toc_is_poly_field(( void*)({ struct Cyc_Core_Opt* _temp745= _temp739->topt;
if( _temp745 == 0){ _throw( Null_Exception);} _temp745->v;}), _temp737); _LL734: {
void* _temp747= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp746=
_temp743->topt; if( _temp746 == 0){ _throw( Null_Exception);} _temp746->v;}));
struct Cyc_Absyn_PtrInfo _temp753; struct Cyc_Absyn_Conref* _temp755; struct Cyc_Absyn_Tqual
_temp757; struct Cyc_Absyn_Conref* _temp759; void* _temp761; void* _temp763;
_LL749: if(( unsigned int) _temp747 > 4u?*(( int*) _temp747) == Cyc_Absyn_PointerType:
0){ _LL754: _temp753=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp747)->f1; _LL764: _temp763=( void*) _temp753.elt_typ; goto _LL762; _LL762:
_temp761=( void*) _temp753.rgn_typ; goto _LL760; _LL760: _temp759=( struct Cyc_Absyn_Conref*)
_temp753.nullable; goto _LL758; _LL758: _temp757=( struct Cyc_Absyn_Tqual)
_temp753.tq; goto _LL756; _LL756: _temp755=( struct Cyc_Absyn_Conref*) _temp753.bounds;
goto _LL750;} else{ goto _LL751;} _LL751: goto _LL752; _LL750: return Cyc_Toc_is_poly_field(
_temp763, _temp741); _LL752:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp766= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp765= _temp743->topt; if( _temp765 == 0){ _throw( Null_Exception);} _temp765->v;}));
xprintf("is_poly_project:  bad type %.*s", _temp766.last_plus_one - _temp766.curr,
_temp766.curr);})); return 0; _LL748:;} _LL736: return 0; _LL730:;} static char
_temp769[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp769, _temp769, _temp769 + 10u}; static struct
_tuple0 Cyc_Toc_gc_malloc_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var,
0),({ struct Cyc_List_List* _temp770=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp770->hd=( void*) s; _temp770->tl= 0; _temp770;}), 0);}
static char _temp773[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp773, _temp773, _temp773 + 17u}; static struct
_tuple0 Cyc_Toc_gc_malloc_atomic_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_atomic_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var,
0),({ struct Cyc_List_List* _temp774=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp774->hd=( void*) s; _temp774->tl= 0; _temp774;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static char _temp777[ 15u]="_region_malloc"; static struct _tagged_string
Cyc_Toc__region_malloc_string=( struct _tagged_string){ _temp777, _temp777,
_temp777 + 15u}; static struct _tuple0 Cyc_Toc_region_malloc_var=( struct
_tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__region_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_region_malloc_var,
0),({ struct Cyc_List_List* _temp778=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp778->hd=( void*) rgn; _temp778->tl=({ struct Cyc_List_List*
_temp779=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp779->hd=( void*) s; _temp779->tl= 0; _temp779;}); _temp778;}), 0);} static
const int Cyc_Toc_NullCheck= 0; struct Cyc_Toc_NullCheck_struct{ int tag; void*
f1; } ; static const int Cyc_Toc_UntagPtr= 1; struct Cyc_Toc_UntagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; int f3; } ; static const int Cyc_Toc_TagPtr=
2; struct Cyc_Toc_TagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; }
; typedef void* Cyc_Toc_conv; struct _tuple7{ void* f1; void* f2; } ; static
struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp781=({ struct _tuple7
_temp780; _temp780.f1= t1; _temp780.f2= t2; _temp780;}); void* _temp787; struct
Cyc_Absyn_PtrInfo _temp789; void* _temp791; struct Cyc_Absyn_PtrInfo _temp793;
_LL783: _LL792: _temp791= _temp781.f1; if(( unsigned int) _temp791 > 4u?*(( int*)
_temp791) == Cyc_Absyn_PointerType: 0){ _LL794: _temp793=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp791)->f1; goto _LL788;} else{ goto
_LL785;} _LL788: _temp787= _temp781.f2; if(( unsigned int) _temp787 > 4u?*(( int*)
_temp787) == Cyc_Absyn_PointerType: 0){ _LL790: _temp789=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp787)->f1; goto _LL784;} else{ goto
_LL785;} _LL785: goto _LL786; _LL784: { int n1=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp793.nullable); int n2=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp789.nullable); void* b1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp793.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp789.bounds); struct Cyc_List_List* convs= 0;{
struct _tuple7 _temp796=({ struct _tuple7 _temp795; _temp795.f1= b1; _temp795.f2=
b2; _temp795;}); void* _temp806; struct Cyc_Absyn_Exp* _temp808; void* _temp810;
struct Cyc_Absyn_Exp* _temp812; void* _temp814; void* _temp816; void* _temp818;
void* _temp820; struct Cyc_Absyn_Exp* _temp822; void* _temp824; struct Cyc_Absyn_Exp*
_temp826; void* _temp828; _LL798: _LL811: _temp810= _temp796.f1; if((
unsigned int) _temp810 > 1u?*(( int*) _temp810) == Cyc_Absyn_Upper_b: 0){ _LL813:
_temp812=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp810)->f1;
goto _LL807;} else{ goto _LL800;} _LL807: _temp806= _temp796.f2; if((
unsigned int) _temp806 > 1u?*(( int*) _temp806) == Cyc_Absyn_Upper_b: 0){ _LL809:
_temp808=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp806)->f1;
goto _LL799;} else{ goto _LL800;} _LL800: _LL817: _temp816= _temp796.f1; if(
_temp816 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL815;} else{ goto _LL802;}
_LL815: _temp814= _temp796.f2; if( _temp814 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL801;} else{ goto _LL802;} _LL802: _LL821: _temp820= _temp796.f1; if((
unsigned int) _temp820 > 1u?*(( int*) _temp820) == Cyc_Absyn_Upper_b: 0){ _LL823:
_temp822=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp820)->f1;
goto _LL819;} else{ goto _LL804;} _LL819: _temp818= _temp796.f2; if( _temp818 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL803;} else{ goto _LL804;} _LL804: _LL829:
_temp828= _temp796.f1; if( _temp828 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL825;}
else{ goto _LL797;} _LL825: _temp824= _temp796.f2; if(( unsigned int) _temp824 >
1u?*(( int*) _temp824) == Cyc_Absyn_Upper_b: 0){ _LL827: _temp826=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp824)->f1; goto _LL805;} else{ goto _LL797;}
_LL799: if( n1? ! n2: 0){ convs=({ struct Cyc_List_List* _temp830=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp830->hd=( void*)(( void*)({
struct Cyc_Toc_NullCheck_struct* _temp831=( struct Cyc_Toc_NullCheck_struct*)
GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct)); _temp831[ 0]=({ struct Cyc_Toc_NullCheck_struct
_temp832; _temp832.tag= Cyc_Toc_NullCheck; _temp832.f1=( void*) t2; _temp832;});
_temp831;})); _temp830->tl= convs; _temp830;});} goto _LL797; _LL801: goto
_LL797; _LL803: convs=({ struct Cyc_List_List* _temp833=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp833->hd=( void*)(( void*)({
struct Cyc_Toc_TagPtr_struct* _temp834=( struct Cyc_Toc_TagPtr_struct*)
GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct)); _temp834[ 0]=({ struct Cyc_Toc_TagPtr_struct
_temp835; _temp835.tag= Cyc_Toc_TagPtr; _temp835.f1=( void*) Cyc_Toc_typ_to_c(
t1); _temp835.f2= _temp822; _temp835;}); _temp834;})); _temp833->tl= convs;
_temp833;}); goto _LL797; _LL805: convs=({ struct Cyc_List_List* _temp836=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp836->hd=(
void*)(( void*)({ struct Cyc_Toc_UntagPtr_struct* _temp837=( struct Cyc_Toc_UntagPtr_struct*)
GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct)); _temp837[ 0]=({ struct Cyc_Toc_UntagPtr_struct
_temp838; _temp838.tag= Cyc_Toc_UntagPtr; _temp838.f1=( void*) Cyc_Toc_typ_to_c(
t1); _temp838.f2= _temp826; _temp838.f3= n2; _temp838;}); _temp837;})); _temp836->tl=
convs; _temp836;}); goto _LL797; _LL797:;} return convs;} _LL786: return 0;
_LL782:;}} static char _temp841[ 7u]="_throw"; static struct _tagged_string Cyc_Toc__throw_string=(
struct _tagged_string){ _temp841, _temp841, _temp841 + 7u}; static struct
_tuple0 Cyc_Toc__throw_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__throw_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__throw_var,
0),({ struct Cyc_List_List* _temp842=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp842->hd=( void*) e; _temp842->tl= 0; _temp842;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp843=( char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp844; _temp844.curr= _temp843; _temp844.base= _temp843;
_temp844.last_plus_one= _temp843 + 44; _temp844;}));} if( Cyc_Toc_use_runtimecheck_funs){
return Cyc_Absyn_cast_exp( new_typ, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp845=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp845->hd=( void*) e; _temp845->tl= 0; _temp845;}), 0),
0);} else{ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0),
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0),
Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
x, 0), 0), 0); return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, new_typ,(
struct Cyc_Absyn_Exp*) e, s, 0), 0);}} static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv(
int toplevel, void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){
struct _tuple0* a= Cyc_Toc_temp_var(); struct _tuple0* b= Cyc_Toc_temp_var();
int szvalue=( int) Cyc_Evexp_eval_const_uint_exp( sz); void* strt_typ; void*
elt_typ;{ void* _temp846= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp852; struct Cyc_Absyn_Conref* _temp854; struct Cyc_Absyn_Tqual _temp856;
struct Cyc_Absyn_Conref* _temp858; void* _temp860; void* _temp862; _LL848: if((
unsigned int) _temp846 > 4u?*(( int*) _temp846) == Cyc_Absyn_PointerType: 0){
_LL853: _temp852=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp846)->f1; _LL863: _temp862=( void*) _temp852.elt_typ; goto _LL861; _LL861:
_temp860=( void*) _temp852.rgn_typ; goto _LL859; _LL859: _temp858=( struct Cyc_Absyn_Conref*)
_temp852.nullable; goto _LL857; _LL857: _temp856=( struct Cyc_Absyn_Tqual)
_temp852.tq; goto _LL855; _LL855: _temp854=( struct Cyc_Absyn_Conref*) _temp852.bounds;
goto _LL849;} else{ goto _LL850;} _LL850: goto _LL851; _LL849: elt_typ= _temp862;
strt_typ= Cyc_Toc_add_tagged_ptr_type( _temp862, Cyc_Toc_mt_tq); goto _LL847;
_LL851: return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp864=( char*)"tag_ptr_conv: not a pointer";
struct _tagged_string _temp865; _temp865.curr= _temp864; _temp865.base= _temp864;
_temp865.last_plus_one= _temp864 + 28; _temp865;})); _LL847:;} if( toplevel){
int is_string= 0;{ void* _temp866=( void*) e->r; void* _temp872; struct
_tagged_string _temp874; _LL868: if(*(( int*) _temp866) == Cyc_Absyn_Const_e){
_LL873: _temp872=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp866)->f1; if((
unsigned int) _temp872 > 1u?*(( int*) _temp872) == Cyc_Absyn_String_c: 0){
_LL875: _temp874=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp872)->f1; goto _LL869;} else{ goto _LL870;}} else{ goto _LL870;} _LL870:
goto _LL871; _LL869: is_string= 1; goto _LL867; _LL871: goto _LL867; _LL867:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp879=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp879[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp880; _temp880.tag= Cyc_Absyn_ArrayType; _temp880.f1=(
void*) elt_typ; _temp880.f2= Cyc_Toc_mt_tq; _temp880.f3=( struct Cyc_Absyn_Exp*)
sz; _temp880;}); _temp879;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp876=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp876->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp877=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp877[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp878; _temp878.tag= Cyc_Absyn_Var_d;
_temp878.f1= vd; _temp878;}); _temp877;}), 0); _temp876->tl= Cyc_Toc_result_decls;
_temp876;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp881=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp881->hd=( void*)({ struct _tuple4* _temp886=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp886->f1= 0; _temp886->f2=
xexp; _temp886;}); _temp881->tl=({ struct Cyc_List_List* _temp882=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp882->hd=( void*)({ struct
_tuple4* _temp885=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp885->f1= 0; _temp885->f2= xexp; _temp885;}); _temp882->tl=({ struct Cyc_List_List*
_temp883=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp883->hd=( void*)({ struct _tuple4* _temp884=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp884->f1= 0; _temp884->f2= xplussz; _temp884;});
_temp883->tl= 0; _temp883;}); _temp882;}); _temp881;}), 0); return urm_exp;}}}
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
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp887=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp888; _temp888.curr= _temp887; _temp888.base= _temp887; _temp888.last_plus_one=
_temp887 + 45; _temp888;}));}{ struct _tuple0* x= Cyc_Toc_temp_var(); int
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
_temp889= cs; if( _temp889 == 0){ _throw( Null_Exception);} _temp889->tl;})){
void* _temp891=( void*)({ struct Cyc_List_List* _temp890= cs; if( _temp890 == 0){
_throw( Null_Exception);} _temp890->hd;}); void* _temp899; struct Cyc_Absyn_Exp*
_temp901; void* _temp903; int _temp905; struct Cyc_Absyn_Exp* _temp907; void*
_temp909; _LL893: if(*(( int*) _temp891) == Cyc_Toc_NullCheck){ _LL900: _temp899=(
void*)(( struct Cyc_Toc_NullCheck_struct*) _temp891)->f1; goto _LL894;} else{
goto _LL895;} _LL895: if(*(( int*) _temp891) == Cyc_Toc_TagPtr){ _LL904:
_temp903=( void*)(( struct Cyc_Toc_TagPtr_struct*) _temp891)->f1; goto _LL902;
_LL902: _temp901=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_TagPtr_struct*)
_temp891)->f2; goto _LL896;} else{ goto _LL897;} _LL897: if(*(( int*) _temp891)
== Cyc_Toc_UntagPtr){ _LL910: _temp909=( void*)(( struct Cyc_Toc_UntagPtr_struct*)
_temp891)->f1; goto _LL908; _LL908: _temp907=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Toc_UntagPtr_struct*) _temp891)->f2; goto _LL906; _LL906: _temp905=( int)((
struct Cyc_Toc_UntagPtr_struct*) _temp891)->f3; goto _LL898;} else{ goto _LL892;}
_LL894: e= Cyc_Toc_null_check_conv( toplevel, _temp899, e); goto _LL892; _LL896:
e= Cyc_Toc_tag_ptr_conv( toplevel, _temp903, _temp901, e); goto _LL892; _LL898:
e= Cyc_Toc_untag_ptr_conv( toplevel, _temp909, _temp907, _temp905, e); goto
_LL892; _LL892:;}} return e;} struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab;
struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt* fallthru_info; struct
Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct Cyc_Toc_Env* Cyc_Toc_env_t;
static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({ struct Cyc_Toc_Env*
_temp911=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct Cyc_Toc_Env));
_temp911->break_lab= 0; _temp911->continue_lab= 0; _temp911->fallthru_info= 0;
_temp911->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp911->toplevel= 1;
_temp911;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp912=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp912->break_lab= e->break_lab; _temp912->continue_lab=
e->continue_lab; _temp912->fallthru_info= e->fallthru_info; _temp912->varmap= e->varmap;
_temp912->toplevel= e->toplevel; _temp912;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp913=(* x).f1; struct
Cyc_List_List* _temp919; _LL915: if(( unsigned int) _temp913 > 1u?*(( int*)
_temp913) == Cyc_Absyn_Rel_n: 0){ _LL920: _temp919=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp913)->f1; goto _LL916;} else{ goto _LL917;}
_LL917: goto _LL918; _LL916:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp921=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp921[ 0]=({ struct Cyc_Core_Impossible_struct _temp922; _temp922.tag= Cyc_Core_Impossible;
_temp922.f1=({ struct _tagged_string _temp923= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp923.last_plus_one - _temp923.curr,
_temp923.curr);}); _temp922;}); _temp921;})); _LL918: goto _LL914; _LL914:;}{
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
_temp924= fallthru_binders; if( _temp924 == 0){ _throw( Null_Exception);}
_temp924->tl;})){ fallthru_vars=({ struct Cyc_List_List* _temp925=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp925->hd=( void*)(( struct Cyc_Absyn_Vardecl*)({
struct Cyc_List_List* _temp926= fallthru_binders; if( _temp926 == 0){ _throw(
Null_Exception);} _temp926->hd;}))->name; _temp925->tl= fallthru_vars; _temp925;});}
fallthru_vars=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fallthru_vars);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp927=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp927->v=( void*) break_l; _temp927;}); ans->fallthru_info=({
struct Cyc_Core_Opt* _temp928=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp928->v=( void*)({ struct _tuple8* _temp929=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp929->f1= fallthru_l; _temp929->f2=
fallthru_vars; _temp929->f3= next_case_env->varmap; _temp929;}); _temp928;});
return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct
Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab=({ struct Cyc_Core_Opt* _temp930=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp930->v=( void*) break_l; _temp930;});
ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env* e, struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp931=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp931->v=( void*)({ struct _tuple8* _temp932=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp932->f1= next_l; _temp932->f2= 0; _temp932->f3=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp932;}); _temp931;}); return ans;} struct _tuple9{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
struct Cyc_List_List* f4; char f5; int f6; } ; struct _tuple10{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp933=( char*)"expecting a literal format string";
struct _tagged_string _temp934; _temp934.curr= _temp933; _temp934.base= _temp933;
_temp934.last_plus_one= _temp933 + 34; _temp934;}));}{ struct _tagged_string
fmt_str;{ void* _temp936=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp935= args; if( _temp935 == 0){ _throw( Null_Exception);} _temp935->hd;}))->r;
void* _temp942; struct _tagged_string _temp944; _LL938: if(*(( int*) _temp936)
== Cyc_Absyn_Const_e){ _LL943: _temp942=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp936)->f1; if(( unsigned int) _temp942 > 1u?*(( int*) _temp942) == Cyc_Absyn_String_c:
0){ _LL945: _temp944=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp942)->f1; goto _LL939;} else{ goto _LL940;}} else{ goto _LL940;} _LL940:
goto _LL941; _LL939: fmt_str= _temp944; goto _LL937; _LL941: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp946=( char*)"expecting a literal format string"; struct _tagged_string
_temp947; _temp947.curr= _temp946; _temp947.base= _temp946; _temp947.last_plus_one=
_temp946 + 34; _temp947;})); _LL937:;}{ int len=( int)({ struct _tagged_string
_temp1054= fmt_str;( unsigned int)( _temp1054.last_plus_one - _temp1054.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(({ struct _tagged_string _temp948=
fmt_str; char* _temp950= _temp948.curr + i; if( _temp948.base == 0? 1:( _temp950
< _temp948.base? 1: _temp950 >= _temp948.last_plus_one)){ _throw( Null_Exception);}*
_temp950;}) =='%'){ i ++; if( i < len?({ struct _tagged_string _temp951= fmt_str;
char* _temp953= _temp951.curr + i; if( _temp951.base == 0? 1:( _temp953 <
_temp951.base? 1: _temp953 >= _temp951.last_plus_one)){ _throw( Null_Exception);}*
_temp953;}) =='s': 0){ break;}}} if( i == len){ if( aopt != 0){ args=({ struct
Cyc_List_List* _temp954=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp954->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp955=
aopt; if( _temp955 == 0){ _throw( Null_Exception);} _temp955->v;})); _temp954->tl=
args; _temp954;});} return Cyc_Absyn_primop_exp( p, args, 0);} args=({ struct
Cyc_List_List* _temp956= args; if( _temp956 == 0){ _throw( Null_Exception);}
_temp956->tl;}); typs=({ struct Cyc_List_List* _temp957= typs; if( _temp957 == 0){
_throw( Null_Exception);} _temp957->tl;});{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ char c=({ struct _tagged_string _temp1044= fmt_str; char*
_temp1046= _temp1044.curr + i; if( _temp1044.base == 0? 1:( _temp1046 <
_temp1044.base? 1: _temp1046 >= _temp1044.last_plus_one)){ _throw(
Null_Exception);}* _temp1046;}); rev_fmt=({ struct Cyc_List_List* _temp958=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp958->hd=(
void*)(( int) c); _temp958->tl= rev_fmt; _temp958;}); if( c !='%'){ continue;}{
struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification( fmt_str, i
+ 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp959=( char*)"bad format string"; struct
_tagged_string _temp960; _temp960.curr= _temp959; _temp960.base= _temp959;
_temp960.last_plus_one= _temp959 + 18; _temp960;}));}{ struct _tuple9 _temp964;
int _temp965; char _temp967; struct Cyc_List_List* _temp969; struct Cyc_List_List*
_temp971; struct Cyc_List_List* _temp973; struct Cyc_List_List* _temp975; struct
_tuple9* _temp962=( struct _tuple9*)({ struct Cyc_Core_Opt* _temp961= x; if(
_temp961 == 0){ _throw( Null_Exception);} _temp961->v;}); _temp964=* _temp962;
_LL976: _temp975= _temp964.f1; goto _LL974; _LL974: _temp973= _temp964.f2; goto
_LL972; _LL972: _temp971= _temp964.f3; goto _LL970; _LL970: _temp969= _temp964.f4;
goto _LL968; _LL968: _temp967= _temp964.f5; goto _LL966; _LL966: _temp965=
_temp964.f6; goto _LL963; _LL963: i= _temp965 - 1; if( _temp967 !='s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp977=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp977->hd=( void*) _temp975; _temp977->tl=({ struct Cyc_List_List* _temp978=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp978->hd=(
void*) _temp973; _temp978->tl=({ struct Cyc_List_List* _temp979=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp979->hd=( void*) _temp971;
_temp979->tl=({ struct Cyc_List_List* _temp980=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp980->hd=( void*) _temp969;
_temp980->tl= 0; _temp980;}); _temp979;}); _temp978;}); _temp977;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp981=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp981->hd=( void*)(( int) _temp967); _temp981->tl=
rev_fmt; _temp981;});{ struct Cyc_List_List* _temp982= _temp973; struct Cyc_List_List
_temp989; struct Cyc_List_List* _temp990; int _temp992; _LL984: if( _temp982 ==
0){ goto _LL986;} else{ _temp989=* _temp982; _LL993: _temp992=( int) _temp989.hd;
goto _LL991; _LL991: _temp990=( struct Cyc_List_List*) _temp989.tl; if( _temp990
== 0){ goto _LL988;} else{ goto _LL986;}} _LL988: if( _temp992 =='*'){ goto
_LL985;} else{ goto _LL986;} _LL986: goto _LL987; _LL985: { struct _tuple0* temp=
Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp994=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp994->hd=( void*)({ struct
_tuple10* _temp995=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp995->f1= temp; _temp995->f2=( void*)({ struct Cyc_List_List* _temp997= typs;
if( _temp997 == 0){ _throw( Null_Exception);} _temp997->hd;}); _temp995->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp996= args; if( _temp996 == 0){
_throw( Null_Exception);} _temp996->hd;}); _temp995;}); _temp994->tl= rev_temps;
_temp994;}); rev_result=({ struct Cyc_List_List* _temp998=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp998->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp998->tl= rev_result; _temp998;}); args=({ struct Cyc_List_List*
_temp999= args; if( _temp999 == 0){ _throw( Null_Exception);} _temp999->tl;});
typs=({ struct Cyc_List_List* _temp1000= typs; if( _temp1000 == 0){ _throw(
Null_Exception);} _temp1000->tl;}); goto _LL983;} _LL987: goto _LL983; _LL983:;}{
struct Cyc_List_List* _temp1001= _temp971; struct Cyc_List_List _temp1008;
struct Cyc_List_List* _temp1009; struct Cyc_List_List _temp1011; struct Cyc_List_List*
_temp1012; int _temp1014; int _temp1016; _LL1003: if( _temp1001 == 0){ goto
_LL1005;} else{ _temp1008=* _temp1001; _LL1017: _temp1016=( int) _temp1008.hd;
goto _LL1010; _LL1010: _temp1009=( struct Cyc_List_List*) _temp1008.tl; if(
_temp1009 == 0){ goto _LL1005;} else{ _temp1011=* _temp1009; _LL1015: _temp1014=(
int) _temp1011.hd; goto _LL1013; _LL1013: _temp1012=( struct Cyc_List_List*)
_temp1011.tl; if( _temp1012 == 0){ goto _LL1007;} else{ goto _LL1005;}}} _LL1007:
if( _temp1016 =='.'? _temp1014 =='*': 0){ goto _LL1004;} else{ goto _LL1005;}
_LL1005: goto _LL1006; _LL1004: { struct _tuple0* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp1018=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1018->hd=( void*)({ struct _tuple10*
_temp1019=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1019->f1=
temp; _temp1019->f2=( void*)({ struct Cyc_List_List* _temp1021= typs; if(
_temp1021 == 0){ _throw( Null_Exception);} _temp1021->hd;}); _temp1019->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1020= args; if( _temp1020 ==
0){ _throw( Null_Exception);} _temp1020->hd;}); _temp1019;}); _temp1018->tl=
rev_temps; _temp1018;}); rev_result=({ struct Cyc_List_List* _temp1022=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1022->hd=( void*)
Cyc_Absyn_var_exp( temp, 0); _temp1022->tl= rev_result; _temp1022;}); args=({
struct Cyc_List_List* _temp1023= args; if( _temp1023 == 0){ _throw(
Null_Exception);} _temp1023->tl;}); typs=({ struct Cyc_List_List* _temp1024=
typs; if( _temp1024 == 0){ _throw( Null_Exception);} _temp1024->tl;}); goto
_LL1002;} _LL1006: goto _LL1002; _LL1002:;} if( _temp967 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1025= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp1025.last_plus_one - _temp1025.curr, _temp1025.curr);}));}{ struct _tuple0*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp1026=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1026->hd=( void*)({
struct _tuple10* _temp1027=( struct _tuple10*) GC_malloc( sizeof( struct
_tuple10)); _temp1027->f1= temp; _temp1027->f2=( void*)({ struct Cyc_List_List*
_temp1029= typs; if( _temp1029 == 0){ _throw( Null_Exception);} _temp1029->hd;});
_temp1027->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1028= args;
if( _temp1028 == 0){ _throw( Null_Exception);} _temp1028->hd;}); _temp1027;});
_temp1026->tl= rev_temps; _temp1026;}); rev_result=({ struct Cyc_List_List*
_temp1030=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1030->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp1030->tl= rev_result;
_temp1030;}); args=({ struct Cyc_List_List* _temp1031= args; if( _temp1031 == 0){
_throw( Null_Exception);} _temp1031->tl;}); typs=({ struct Cyc_List_List*
_temp1032= typs; if( _temp1032 == 0){ _throw( Null_Exception);} _temp1032->tl;});}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp1033=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1033->hd=( void*)(( int)'.');
_temp1033->tl= rev_fmt; _temp1033;}); rev_fmt=({ struct Cyc_List_List* _temp1034=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1034->hd=(
void*)(( int)'*'); _temp1034->tl= rev_fmt; _temp1034;}); rev_fmt=({ struct Cyc_List_List*
_temp1035=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1035->hd=( void*)(( int)'s'); _temp1035->tl= rev_fmt; _temp1035;});{ struct
_tuple0* temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp1036=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1036->hd=(
void*)({ struct _tuple10* _temp1037=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1037->f1= temp; _temp1037->f2=( void*)({ struct Cyc_List_List*
_temp1039= typs; if( _temp1039 == 0){ _throw( Null_Exception);} _temp1039->hd;});
_temp1037->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1038= args;
if( _temp1038 == 0){ _throw( Null_Exception);} _temp1038->hd;}); _temp1037;});
_temp1036->tl= rev_temps; _temp1036;});{ struct Cyc_Absyn_Exp* size_exp= Cyc_Absyn_subtract_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp,
0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0),
0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List* _temp1040=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1040->hd=(
void*) size_exp; _temp1040->tl= rev_result; _temp1040;}); rev_result=({ struct
Cyc_List_List* _temp1041=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1041->hd=( void*) cstring_exp; _temp1041->tl= rev_result; _temp1041;});
args=({ struct Cyc_List_List* _temp1042= args; if( _temp1042 == 0){ _throw(
Null_Exception);} _temp1042->tl;}); typs=({ struct Cyc_List_List* _temp1043=
typs; if( _temp1043 == 0){ _throw( Null_Exception);} _temp1043->tl;});}}}}}}{
struct Cyc_List_List* es=({ struct Cyc_List_List* _temp1053=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1053->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp1053->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_result); _temp1053;}); if( aopt != 0){ es=({ struct Cyc_List_List*
_temp1047=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1047->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp1048=
aopt; if( _temp1048 == 0){ _throw( Null_Exception);} _temp1048->v;})); _temp1047->tl=
es; _temp1047;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p, es, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps != 0;
rev_temps=({ struct Cyc_List_List* _temp1049= rev_temps; if( _temp1049 == 0){
_throw( Null_Exception);} _temp1049->tl;})){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)({ struct Cyc_List_List* _temp1050= rev_temps; if( _temp1050 ==
0){ _throw( Null_Exception);} _temp1050->hd;}))).f1,(*(( struct _tuple10*)({
struct Cyc_List_List* _temp1051= rev_temps; if( _temp1051 == 0){ _throw(
Null_Exception);} _temp1051->hd;}))).f2,( struct Cyc_Absyn_Exp*)(*(( struct
_tuple10*)({ struct Cyc_List_List* _temp1052= rev_temps; if( _temp1052 == 0){
_throw( Null_Exception);} _temp1052->hd;}))).f3, s, 0);} return Cyc_Absyn_stmt_exp(
s, 0);}}}}}} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1055=( char*)"Missing type in primop "; struct
_tagged_string _temp1056; _temp1056.curr= _temp1055; _temp1056.base= _temp1055;
_temp1056.last_plus_one= _temp1055 + 24; _temp1056;}));} return Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp1057= e->topt; if( _temp1057 == 0){ _throw(
Null_Exception);} _temp1057->v;}));} static void* Cyc_Toc_get_cyc_typ( struct
Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1058=( char*)"Missing type in primop ";
struct _tagged_string _temp1059; _temp1059.curr= _temp1058; _temp1059.base=
_temp1058; _temp1059.last_plus_one= _temp1058 + 24; _temp1059;}));} return( void*)({
struct Cyc_Core_Opt* _temp1060= e->topt; if( _temp1060 == 0){ _throw(
Null_Exception);} _temp1060->v;});} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1061=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1061->f1= Cyc_Toc_mt_tq; _temp1061->f2=
Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1062= e->topt; if(
_temp1062 == 0){ _throw( Null_Exception);} _temp1062->v;})); _temp1061;});}
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e);
static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp1063=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1063[ 0]=({
struct _tuple4 _temp1064; _temp1064.f1= 0; _temp1064.f2= e; _temp1064;});
_temp1063;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
se= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic( se): Cyc_Toc_malloc_ptr(
se), 0);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1065= rgnopt; if( _temp1065 == 0){ _throw( Null_Exception);} _temp1065;});
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
_temp1066= dles; if( _temp1066 == 0){ _throw( Null_Exception);} _temp1066->tl;})){
struct _tuple4 _temp1070; struct Cyc_Absyn_Exp* _temp1071; struct Cyc_List_List*
_temp1073; struct _tuple4* _temp1068=( struct _tuple4*)({ struct Cyc_List_List*
_temp1067= dles; if( _temp1067 == 0){ _throw( Null_Exception);} _temp1067->hd;});
_temp1070=* _temp1068; _LL1074: _temp1073= _temp1070.f1; goto _LL1072; _LL1072:
_temp1071= _temp1070.f2; goto _LL1069; _LL1069: { struct Cyc_Absyn_Exp* e_index;
if( _temp1073 == 0){ e_index= Cyc_Absyn_signed_int_exp(( count --), 0);} else{
if(({ struct Cyc_List_List* _temp1075= _temp1073; if( _temp1075 == 0){ _throw(
Null_Exception);} _temp1075->tl;}) != 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1076=( char*)"multiple designators in array";
struct _tagged_string _temp1077; _temp1077.curr= _temp1076; _temp1077.base=
_temp1076; _temp1077.last_plus_one= _temp1076 + 30; _temp1077;}));}{ void* d=(
void*)({ struct Cyc_List_List* _temp1090= _temp1073; if( _temp1090 == 0){ _throw(
Null_Exception);} _temp1090->hd;}); void* _temp1078= d; struct Cyc_Absyn_Exp*
_temp1084; struct _tagged_string* _temp1086; _LL1080: if(*(( int*) _temp1078) ==
Cyc_Absyn_ArrayElement){ _LL1085: _temp1084=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayElement_struct*) _temp1078)->f1; goto _LL1081;} else{ goto
_LL1082;} _LL1082: if(*(( int*) _temp1078) == Cyc_Absyn_FieldName){ _LL1087:
_temp1086=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1078)->f1; goto _LL1083;} else{ goto _LL1079;} _LL1081: Cyc_Toc_exp_to_c(
nv, _temp1084); e_index= _temp1084; goto _LL1079; _LL1083: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1088=( char*)"field name designators in array"; struct _tagged_string
_temp1089; _temp1089.curr= _temp1088; _temp1089.base= _temp1088; _temp1089.last_plus_one=
_temp1088 + 32; _temp1089;})); goto _LL1079; _LL1079:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp1091=( void*)
_temp1071->r; struct Cyc_List_List* _temp1101; struct Cyc_Absyn_Exp* _temp1103;
struct Cyc_Absyn_Exp* _temp1105; struct Cyc_Absyn_Vardecl* _temp1107; struct Cyc_List_List*
_temp1109; void* _temp1111; _LL1093: if(*(( int*) _temp1091) == Cyc_Absyn_Array_e){
_LL1102: _temp1101=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1091)->f1; goto _LL1094;} else{ goto _LL1095;} _LL1095: if(*(( int*)
_temp1091) == Cyc_Absyn_Comprehension_e){ _LL1108: _temp1107=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1091)->f1; goto _LL1106; _LL1106:
_temp1105=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1091)->f2; goto _LL1104; _LL1104: _temp1103=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1091)->f3; goto _LL1096;} else{
goto _LL1097;} _LL1097: if(*(( int*) _temp1091) == Cyc_Absyn_AnonStruct_e){
_LL1112: _temp1111=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1091)->f1;
goto _LL1110; _LL1110: _temp1109=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1091)->f2; goto _LL1098;} else{ goto _LL1099;} _LL1099: goto _LL1100;
_LL1094: s= Cyc_Toc_init_array( nv, lval, _temp1101, s); goto _LL1092; _LL1096:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1107, _temp1105, _temp1103, s, 0);
goto _LL1092; _LL1098: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1111,
_temp1109, s); goto _LL1092; _LL1100: Cyc_Toc_exp_to_c( nv, _temp1071); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp1071, 0),
s, 0); goto _LL1092; _LL1092:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* x1= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp( x1,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1135=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1135[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1136; _temp1136.tag= Cyc_Absyn_Local_b;
_temp1136.f1= vd; _temp1136;}); _temp1135;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_var_exp( max, 0), 0);
struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( x1, 0), 0);
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( x1,
0), 0); struct Cyc_Absyn_Stmt* body;{ void* _temp1113=( void*) e2->r; struct Cyc_List_List*
_temp1123; struct Cyc_Absyn_Exp* _temp1125; struct Cyc_Absyn_Exp* _temp1127;
struct Cyc_Absyn_Vardecl* _temp1129; struct Cyc_List_List* _temp1131; void*
_temp1133; _LL1115: if(*(( int*) _temp1113) == Cyc_Absyn_Array_e){ _LL1124:
_temp1123=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1113)->f1;
goto _LL1116;} else{ goto _LL1117;} _LL1117: if(*(( int*) _temp1113) == Cyc_Absyn_Comprehension_e){
_LL1130: _temp1129=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1113)->f1; goto _LL1128; _LL1128: _temp1127=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1113)->f2; goto _LL1126; _LL1126:
_temp1125=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1113)->f3; goto _LL1118;} else{ goto _LL1119;} _LL1119: if(*(( int*)
_temp1113) == Cyc_Absyn_AnonStruct_e){ _LL1134: _temp1133=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1113)->f1; goto _LL1132; _LL1132: _temp1131=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1113)->f2; goto _LL1120;} else{ goto
_LL1121;} _LL1121: goto _LL1122; _LL1116: body= Cyc_Toc_init_array( nv2, lval,
_temp1123, Cyc_Absyn_skip_stmt( 0)); goto _LL1114; _LL1118: body= Cyc_Toc_init_comprehension(
nv2, lval, _temp1129, _temp1127, _temp1125, Cyc_Absyn_skip_stmt( 0), 0); goto
_LL1114; _LL1120: body= Cyc_Toc_init_anon_struct( nv, lval, _temp1133, _temp1131,
Cyc_Absyn_skip_stmt( 0)); goto _LL1114; _LL1122: Cyc_Toc_exp_to_c( nv2, e2);
body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL1114; _LL1114:;}{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt(
max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( x1,
Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( dles); for( 0; es != 0; es=({ struct Cyc_List_List* _temp1137= es;
if( _temp1137 == 0){ _throw( Null_Exception);} _temp1137->tl;})){ struct _tuple4
_temp1141; struct Cyc_Absyn_Exp* _temp1142; struct Cyc_List_List* _temp1144;
struct _tuple4* _temp1139=( struct _tuple4*)({ struct Cyc_List_List* _temp1138=
es; if( _temp1138 == 0){ _throw( Null_Exception);} _temp1138->hd;}); _temp1141=*
_temp1139; _LL1145: _temp1144= _temp1141.f1; goto _LL1143; _LL1143: _temp1142=
_temp1141.f2; goto _LL1140; _LL1140: if( _temp1144 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1146=(
char*)"empty designator list"; struct _tagged_string _temp1147; _temp1147.curr=
_temp1146; _temp1147.base= _temp1146; _temp1147.last_plus_one= _temp1146 + 22;
_temp1147;}));} if(({ struct Cyc_List_List* _temp1148= _temp1144; if( _temp1148
== 0){ _throw( Null_Exception);} _temp1148->tl;}) != 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1149=(
char*)"too many designators in anonymous struct"; struct _tagged_string
_temp1150; _temp1150.curr= _temp1149; _temp1150.base= _temp1149; _temp1150.last_plus_one=
_temp1149 + 41; _temp1150;}));}{ void* _temp1152=( void*)({ struct Cyc_List_List*
_temp1151= _temp1144; if( _temp1151 == 0){ _throw( Null_Exception);} _temp1151->hd;});
struct _tagged_string* _temp1158; _LL1154: if(*(( int*) _temp1152) == Cyc_Absyn_FieldName){
_LL1159: _temp1158=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1152)->f1; goto _LL1155;} else{ goto _LL1156;} _LL1156: goto _LL1157;
_LL1155: { struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs,
_temp1158, 0);{ void* _temp1160=( void*) _temp1142->r; struct Cyc_List_List*
_temp1170; struct Cyc_Absyn_Exp* _temp1172; struct Cyc_Absyn_Exp* _temp1174;
struct Cyc_Absyn_Vardecl* _temp1176; struct Cyc_List_List* _temp1178; void*
_temp1180; _LL1162: if(*(( int*) _temp1160) == Cyc_Absyn_Array_e){ _LL1171:
_temp1170=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1160)->f1;
goto _LL1163;} else{ goto _LL1164;} _LL1164: if(*(( int*) _temp1160) == Cyc_Absyn_Comprehension_e){
_LL1177: _temp1176=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1160)->f1; goto _LL1175; _LL1175: _temp1174=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1160)->f2; goto _LL1173; _LL1173:
_temp1172=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1160)->f3; goto _LL1165;} else{ goto _LL1166;} _LL1166: if(*(( int*)
_temp1160) == Cyc_Absyn_AnonStruct_e){ _LL1181: _temp1180=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1160)->f1; goto _LL1179; _LL1179: _temp1178=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1160)->f2; goto _LL1167;} else{ goto
_LL1168;} _LL1168: goto _LL1169; _LL1163: s= Cyc_Toc_init_array( nv, lval,
_temp1170, s); goto _LL1161; _LL1165: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1176, _temp1174, _temp1172, s, 0); goto _LL1161; _LL1167: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp1180, _temp1178, s); goto _LL1161; _LL1169: Cyc_Toc_exp_to_c( nv,
_temp1142); if( Cyc_Toc_is_poly_field( struct_type, _temp1158)){ _temp1142= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1142, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp1142, 0), 0), s, 0); goto _LL1161; _LL1161:;}
goto _LL1153;} _LL1157:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1182=( char*)"array designator in struct";
struct _tagged_string _temp1183; _temp1183.curr= _temp1182; _temp1183.base=
_temp1182; _temp1183.last_plus_one= _temp1182 + 27; _temp1183;}))); _LL1153:;}}}
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
_temp1184= es; if( _temp1184 == 0){ _throw( Null_Exception);} _temp1184->tl;}),
-- i){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1204= es; if( _temp1204 == 0){ _throw( Null_Exception);} _temp1204->hd;});
struct Cyc_Absyn_Exp* lval= lval_f( xexp,({ struct _tagged_string* _temp1203=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1203[ 0]= xprintf("f%d", i); _temp1203;}), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)({ struct Cyc_Core_Opt* _temp1185= e->topt; if( _temp1185 == 0){ _throw(
Null_Exception);} _temp1185->v;})): 0;{ void* _temp1186=( void*) e->r; struct
Cyc_List_List* _temp1194; struct Cyc_Absyn_Exp* _temp1196; struct Cyc_Absyn_Exp*
_temp1198; struct Cyc_Absyn_Vardecl* _temp1200; _LL1188: if(*(( int*) _temp1186)
== Cyc_Absyn_Array_e){ _LL1195: _temp1194=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1186)->f1; goto _LL1189;} else{ goto _LL1190;} _LL1190: if(*(( int*)
_temp1186) == Cyc_Absyn_Comprehension_e){ _LL1201: _temp1200=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1186)->f1; goto _LL1199; _LL1199:
_temp1198=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1186)->f2; goto _LL1197; _LL1197: _temp1196=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1186)->f3; goto _LL1191;} else{
goto _LL1192;} _LL1192: goto _LL1193; _LL1189: s= Cyc_Toc_init_array( nv, lval,
_temp1194, s); goto _LL1187; _LL1191: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1200, _temp1198, _temp1196, s, 0); goto _LL1187; _LL1193: Cyc_Toc_exp_to_c(
nv, e); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f(
xexp,({ struct _tagged_string* _temp1202=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1202[ 0]= xprintf("f%d", i); _temp1202;}),
0), e, 0), 0), s, 0); goto _LL1187; _LL1187:;}}} return Cyc_Toc_make_struct( nv,
x, Cyc_Absyn_strct( n), s, pointer, rgnopt, is_atomic);} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_struct( struct Cyc_Toc_Env* nv, void* struct_type, int pointer,
struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* dles, struct _tuple0* tdn){
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(*
lval_f)( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;{
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( dles); for( 0; es != 0; es=({ struct Cyc_List_List* _temp1205= es;
if( _temp1205 == 0){ _throw( Null_Exception);} _temp1205->tl;})){ struct _tuple4
_temp1209; struct Cyc_Absyn_Exp* _temp1210; struct Cyc_List_List* _temp1212;
struct _tuple4* _temp1207=( struct _tuple4*)({ struct Cyc_List_List* _temp1206=
es; if( _temp1206 == 0){ _throw( Null_Exception);} _temp1206->hd;}); _temp1209=*
_temp1207; _LL1213: _temp1212= _temp1209.f1; goto _LL1211; _LL1211: _temp1210=
_temp1209.f2; goto _LL1208; _LL1208: is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)({ struct Cyc_Core_Opt* _temp1214= _temp1210->topt; if( _temp1214 == 0){
_throw( Null_Exception);} _temp1214->v;})): 0; if( _temp1212 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1215=( char*)"empty designator list"; struct _tagged_string _temp1216;
_temp1216.curr= _temp1215; _temp1216.base= _temp1215; _temp1216.last_plus_one=
_temp1215 + 22; _temp1216;}));} if(({ struct Cyc_List_List* _temp1217= _temp1212;
if( _temp1217 == 0){ _throw( Null_Exception);} _temp1217->tl;}) != 0){ struct
_tuple0* y= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* yexp= Cyc_Absyn_var_exp( y,
0); for( 0; _temp1212 != 0; _temp1212=({ struct Cyc_List_List* _temp1218=
_temp1212; if( _temp1218 == 0){ _throw( Null_Exception);} _temp1218->tl;})){
void* _temp1220=( void*)({ struct Cyc_List_List* _temp1219= _temp1212; if(
_temp1219 == 0){ _throw( Null_Exception);} _temp1219->hd;}); struct
_tagged_string* _temp1226; _LL1222: if(*(( int*) _temp1220) == Cyc_Absyn_FieldName){
_LL1227: _temp1226=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1220)->f1; goto _LL1223;} else{ goto _LL1224;} _LL1224: goto _LL1225;
_LL1223: if( Cyc_Toc_is_poly_field( struct_type, _temp1226)){ yexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), yexp, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval_f( xexp, _temp1226, 0), yexp, 0), 0), s, 0); goto
_LL1221; _LL1225:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1228=( char*)"array designator in struct";
struct _tagged_string _temp1229; _temp1229.curr= _temp1228; _temp1229.base=
_temp1228; _temp1229.last_plus_one= _temp1228 + 27; _temp1229;}))); _LL1221:;}
Cyc_Toc_exp_to_c( nv, _temp1210); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
yexp, _temp1210, 0), 0), s, 0);} else{ void* _temp1231=( void*)({ struct Cyc_List_List*
_temp1230= _temp1212; if( _temp1230 == 0){ _throw( Null_Exception);} _temp1230->hd;});
struct _tagged_string* _temp1237; _LL1233: if(*(( int*) _temp1231) == Cyc_Absyn_FieldName){
_LL1238: _temp1237=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1231)->f1; goto _LL1234;} else{ goto _LL1235;} _LL1235: goto _LL1236;
_LL1234: { struct Cyc_Absyn_Exp* lval= lval_f( xexp, _temp1237, 0);{ void*
_temp1239=( void*) _temp1210->r; struct Cyc_List_List* _temp1249; struct Cyc_Absyn_Exp*
_temp1251; struct Cyc_Absyn_Exp* _temp1253; struct Cyc_Absyn_Vardecl* _temp1255;
struct Cyc_List_List* _temp1257; void* _temp1259; _LL1241: if(*(( int*)
_temp1239) == Cyc_Absyn_Array_e){ _LL1250: _temp1249=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1239)->f1; goto _LL1242;} else{ goto
_LL1243;} _LL1243: if(*(( int*) _temp1239) == Cyc_Absyn_Comprehension_e){
_LL1256: _temp1255=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1239)->f1; goto _LL1254; _LL1254: _temp1253=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1239)->f2; goto _LL1252; _LL1252:
_temp1251=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1239)->f3; goto _LL1244;} else{ goto _LL1245;} _LL1245: if(*(( int*)
_temp1239) == Cyc_Absyn_AnonStruct_e){ _LL1260: _temp1259=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1239)->f1; goto _LL1258; _LL1258: _temp1257=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1239)->f2; goto _LL1246;} else{ goto
_LL1247;} _LL1247: goto _LL1248; _LL1242: s= Cyc_Toc_init_array( nv, lval,
_temp1249, s); goto _LL1240; _LL1244: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1255, _temp1253, _temp1251, s, 0); goto _LL1240; _LL1246: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp1259, _temp1257, s); goto _LL1240; _LL1248: Cyc_Toc_exp_to_c( nv,
_temp1210); if( Cyc_Toc_is_poly_field( struct_type, _temp1237)){ _temp1210= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1210, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp1210, 0), 0), s, 0); goto _LL1240; _LL1240:;}
goto _LL1232;} _LL1236:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1261=( char*)"array designator in struct";
struct _tagged_string _temp1262; _temp1262.curr= _temp1261; _temp1262.base=
_temp1261; _temp1262.last_plus_one= _temp1261 + 27; _temp1262;}))); _LL1232:;}}}
return Cyc_Toc_make_struct( nv, x, Cyc_Absyn_strctq( tdn), s, pointer, rgnopt,
is_atomic);} struct _tuple11{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2;
} ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp*
el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2,
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp*
e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Increment_e_struct* _temp1263=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp1263[ 0]=({
struct Cyc_Absyn_Increment_e_struct _temp1264; _temp1264.tag= Cyc_Absyn_Increment_e;
_temp1264.f1= e1; _temp1264.f2=( void*) incr; _temp1264;}); _temp1263;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp1265=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp1275; struct Cyc_Absyn_Exp*
_temp1277; void* _temp1279; struct _tagged_string* _temp1281; struct Cyc_Absyn_Exp*
_temp1283; _LL1267: if(*(( int*) _temp1265) == Cyc_Absyn_StmtExp_e){ _LL1276:
_temp1275=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1265)->f1; goto _LL1268;} else{ goto _LL1269;} _LL1269: if(*(( int*)
_temp1265) == Cyc_Absyn_Cast_e){ _LL1280: _temp1279=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1265)->f1; goto _LL1278; _LL1278: _temp1277=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Cast_e_struct*) _temp1265)->f2; goto _LL1270;} else{ goto
_LL1271;} _LL1271: if(*(( int*) _temp1265) == Cyc_Absyn_StructMember_e){ _LL1284:
_temp1283=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1265)->f1; goto _LL1282; _LL1282: _temp1281=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1265)->f2; goto _LL1272;} else{
goto _LL1273;} _LL1273: goto _LL1274; _LL1268:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1275, fs, f, f_env);
goto _LL1266; _LL1270:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1277, fs, f, f_env); goto _LL1266; _LL1272:( void*)( e1->r=( void*)(( void*)
_temp1283->r));(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
e1,({ struct Cyc_List_List* _temp1285=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1285->hd=( void*) _temp1281; _temp1285->tl=
fs; _temp1285;}), f, f_env); goto _LL1266; _LL1274: if( ! Cyc_Absyn_is_lvalue(
e1)){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1286= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1286.last_plus_one - _temp1286.curr, _temp1286.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp1287= fs; if( _temp1287 == 0){ _throw( Null_Exception);} _temp1287->tl;})){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_string*)({ struct
Cyc_List_List* _temp1288= fs; if( _temp1288 == 0){ _throw( Null_Exception);}
_temp1288->hd;}), e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy,
f_env))->r)); goto _LL1266;} _LL1266:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp1289=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1299; struct Cyc_Absyn_Stmt* _temp1301; struct Cyc_Absyn_Decl*
_temp1303; struct Cyc_Absyn_Stmt* _temp1305; struct Cyc_Absyn_Stmt* _temp1307;
_LL1291: if(( unsigned int) _temp1289 > 1u?*(( int*) _temp1289) == Cyc_Absyn_Exp_s:
0){ _LL1300: _temp1299=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1289)->f1; goto _LL1292;} else{ goto _LL1293;} _LL1293: if(( unsigned int)
_temp1289 > 1u?*(( int*) _temp1289) == Cyc_Absyn_Decl_s: 0){ _LL1304: _temp1303=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1289)->f1; goto
_LL1302; _LL1302: _temp1301=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1289)->f2; goto _LL1294;} else{ goto _LL1295;} _LL1295: if(( unsigned int)
_temp1289 > 1u?*(( int*) _temp1289) == Cyc_Absyn_Seq_s: 0){ _LL1308: _temp1307=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1289)->f1; goto
_LL1306; _LL1306: _temp1305=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1289)->f2; goto _LL1296;} else{ goto _LL1297;} _LL1297: goto _LL1298;
_LL1292:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1299, fs, f, f_env); goto _LL1290; _LL1294:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1301, fs, f, f_env);
goto _LL1290; _LL1296:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1305, fs, f, f_env); goto _LL1290; _LL1298:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1309= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1309.last_plus_one - _temp1309.curr,
_temp1309.curr);})); goto _LL1290; _LL1290:;} static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s); struct _tuple12{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* r=( void*) e->r; if( e->topt == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1310=
Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s", _temp1310.last_plus_one
- _temp1310.curr, _temp1310.curr);})); return;}{ void* old_typ=( void*)({ struct
Cyc_Core_Opt* _temp2023= e->topt; if( _temp2023 == 0){ _throw( Null_Exception);}
_temp2023->v;}); void* _temp1311= r; void* _temp1387; void* _temp1389; void*
_temp1391; struct _tuple0* _temp1393; struct _tuple0* _temp1395; struct Cyc_List_List*
_temp1397; void* _temp1399; void* _temp1401; struct Cyc_Absyn_Exp* _temp1403;
struct Cyc_Absyn_Exp* _temp1405; struct Cyc_Core_Opt* _temp1407; struct Cyc_Absyn_Exp*
_temp1409; struct Cyc_Absyn_Exp* _temp1411; struct Cyc_Absyn_Exp* _temp1413;
struct Cyc_Absyn_Exp* _temp1415; struct Cyc_Absyn_Exp* _temp1417; struct Cyc_Absyn_Exp*
_temp1419; struct Cyc_List_List* _temp1421; struct Cyc_Absyn_Exp* _temp1423;
struct Cyc_List_List* _temp1425; struct Cyc_Absyn_Exp* _temp1427; struct Cyc_Absyn_Exp*
_temp1429; struct Cyc_Absyn_Exp* _temp1431; struct Cyc_List_List* _temp1433;
struct Cyc_Absyn_Exp* _temp1435; struct Cyc_Absyn_Exp* _temp1437; struct Cyc_Absyn_Exp**
_temp1439; void* _temp1440; void** _temp1442; struct Cyc_Absyn_Exp* _temp1443;
struct Cyc_Absyn_Exp* _temp1445; struct Cyc_Absyn_Exp* _temp1447; void*
_temp1449; struct Cyc_Absyn_Exp* _temp1451; struct Cyc_Absyn_Exp* _temp1453;
struct _tagged_string* _temp1455; struct Cyc_Absyn_Exp* _temp1457; struct
_tagged_string* _temp1459; struct Cyc_Absyn_Exp* _temp1461; struct Cyc_Absyn_Exp*
_temp1463; struct Cyc_Absyn_Exp* _temp1465; struct Cyc_List_List* _temp1467;
struct Cyc_List_List* _temp1469; struct _tuple1* _temp1471; struct Cyc_List_List*
_temp1473; struct Cyc_Absyn_Exp* _temp1475; struct Cyc_Absyn_Exp* _temp1477;
struct Cyc_Absyn_Vardecl* _temp1479; struct Cyc_Absyn_Structdecl* _temp1481;
struct Cyc_List_List* _temp1483; struct Cyc_Core_Opt* _temp1485; struct _tuple0*
_temp1487; struct Cyc_List_List* _temp1489; void* _temp1491; struct Cyc_Absyn_Tunionfield*
_temp1493; struct Cyc_Absyn_Tuniondecl* _temp1495; struct Cyc_List_List*
_temp1497; struct Cyc_Core_Opt* _temp1499; struct Cyc_Core_Opt* _temp1501;
struct Cyc_Absyn_Tunionfield* _temp1503; struct Cyc_Absyn_Tuniondecl* _temp1505;
struct Cyc_List_List* _temp1507; struct Cyc_Core_Opt* _temp1509; struct Cyc_Core_Opt*
_temp1511; struct Cyc_Absyn_Enumfield* _temp1513; struct Cyc_Absyn_Enumdecl*
_temp1515; struct _tuple0* _temp1517; void* _temp1519; struct Cyc_Absyn_Exp*
_temp1521; struct Cyc_Absyn_Stmt* _temp1523; struct Cyc_List_List* _temp1525;
struct Cyc_Core_Opt* _temp1527; struct Cyc_Absyn_Fndecl* _temp1529; struct Cyc_Absyn_Exp*
_temp1531; _LL1313: if(*(( int*) _temp1311) == Cyc_Absyn_Const_e){ _LL1388:
_temp1387=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1311)->f1; if(
_temp1387 ==( void*) Cyc_Absyn_Null_c){ goto _LL1314;} else{ goto _LL1315;}}
else{ goto _LL1315;} _LL1315: if(*(( int*) _temp1311) == Cyc_Absyn_Const_e){
_LL1390: _temp1389=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1311)->f1;
goto _LL1316;} else{ goto _LL1317;} _LL1317: if(*(( int*) _temp1311) == Cyc_Absyn_Var_e){
_LL1394: _temp1393=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1311)->f1; goto _LL1392; _LL1392: _temp1391=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1311)->f2; goto _LL1318;} else{ goto _LL1319;} _LL1319: if(*(( int*)
_temp1311) == Cyc_Absyn_UnknownId_e){ _LL1396: _temp1395=( struct _tuple0*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1311)->f1; goto _LL1320;} else{ goto
_LL1321;} _LL1321: if(*(( int*) _temp1311) == Cyc_Absyn_Primop_e){ _LL1400:
_temp1399=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1311)->f1; goto
_LL1398; _LL1398: _temp1397=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1311)->f2; goto _LL1322;} else{ goto _LL1323;} _LL1323: if(*(( int*)
_temp1311) == Cyc_Absyn_Increment_e){ _LL1404: _temp1403=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp1311)->f1; goto _LL1402; _LL1402:
_temp1401=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1311)->f2; goto
_LL1324;} else{ goto _LL1325;} _LL1325: if(*(( int*) _temp1311) == Cyc_Absyn_AssignOp_e){
_LL1410: _temp1409=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1311)->f1; goto _LL1408; _LL1408: _temp1407=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1311)->f2; goto _LL1406; _LL1406:
_temp1405=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1311)->f3; goto _LL1326;} else{ goto _LL1327;} _LL1327: if(*(( int*)
_temp1311) == Cyc_Absyn_Conditional_e){ _LL1416: _temp1415=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1311)->f1; goto _LL1414; _LL1414:
_temp1413=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1311)->f2; goto _LL1412; _LL1412: _temp1411=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1311)->f3; goto _LL1328;} else{
goto _LL1329;} _LL1329: if(*(( int*) _temp1311) == Cyc_Absyn_SeqExp_e){ _LL1420:
_temp1419=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1311)->f1; goto _LL1418; _LL1418: _temp1417=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1311)->f2; goto _LL1330;} else{ goto
_LL1331;} _LL1331: if(*(( int*) _temp1311) == Cyc_Absyn_UnknownCall_e){ _LL1424:
_temp1423=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1311)->f1; goto _LL1422; _LL1422: _temp1421=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1311)->f2; goto _LL1332;} else{
goto _LL1333;} _LL1333: if(*(( int*) _temp1311) == Cyc_Absyn_FnCall_e){ _LL1428:
_temp1427=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1311)->f1; goto _LL1426; _LL1426: _temp1425=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1311)->f2; goto _LL1334;} else{ goto
_LL1335;} _LL1335: if(*(( int*) _temp1311) == Cyc_Absyn_Throw_e){ _LL1430:
_temp1429=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1311)->f1;
goto _LL1336;} else{ goto _LL1337;} _LL1337: if(*(( int*) _temp1311) == Cyc_Absyn_NoInstantiate_e){
_LL1432: _temp1431=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1311)->f1; goto _LL1338;} else{ goto _LL1339;} _LL1339: if(*(( int*)
_temp1311) == Cyc_Absyn_Instantiate_e){ _LL1436: _temp1435=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1311)->f1; goto _LL1434; _LL1434:
_temp1433=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1311)->f2; goto _LL1340;} else{ goto _LL1341;} _LL1341: if(*(( int*)
_temp1311) == Cyc_Absyn_Cast_e){ _LL1441: _temp1440=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1311)->f1; _temp1442=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1311)->f1;
goto _LL1438; _LL1438: _temp1437=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1311)->f2; _temp1439=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1311)->f2;
goto _LL1342;} else{ goto _LL1343;} _LL1343: if(*(( int*) _temp1311) == Cyc_Absyn_Address_e){
_LL1444: _temp1443=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp1311)->f1; goto _LL1344;} else{ goto _LL1345;} _LL1345: if(*(( int*)
_temp1311) == Cyc_Absyn_New_e){ _LL1448: _temp1447=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1311)->f1; goto _LL1446; _LL1446: _temp1445=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1311)->f2; goto
_LL1346;} else{ goto _LL1347;} _LL1347: if(*(( int*) _temp1311) == Cyc_Absyn_Sizeoftyp_e){
_LL1450: _temp1449=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1311)->f1;
goto _LL1348;} else{ goto _LL1349;} _LL1349: if(*(( int*) _temp1311) == Cyc_Absyn_Sizeofexp_e){
_LL1452: _temp1451=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1311)->f1; goto _LL1350;} else{ goto _LL1351;} _LL1351: if(*(( int*)
_temp1311) == Cyc_Absyn_Deref_e){ _LL1454: _temp1453=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp1311)->f1; goto _LL1352;} else{ goto
_LL1353;} _LL1353: if(*(( int*) _temp1311) == Cyc_Absyn_StructMember_e){ _LL1458:
_temp1457=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1311)->f1; goto _LL1456; _LL1456: _temp1455=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1311)->f2; goto _LL1354;} else{
goto _LL1355;} _LL1355: if(*(( int*) _temp1311) == Cyc_Absyn_StructArrow_e){
_LL1462: _temp1461=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1311)->f1; goto _LL1460; _LL1460: _temp1459=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1311)->f2; goto _LL1356;} else{
goto _LL1357;} _LL1357: if(*(( int*) _temp1311) == Cyc_Absyn_Subscript_e){
_LL1466: _temp1465=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1311)->f1; goto _LL1464; _LL1464: _temp1463=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1311)->f2; goto _LL1358;} else{ goto
_LL1359;} _LL1359: if(*(( int*) _temp1311) == Cyc_Absyn_Tuple_e){ _LL1468:
_temp1467=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1311)->f1;
goto _LL1360;} else{ goto _LL1361;} _LL1361: if(*(( int*) _temp1311) == Cyc_Absyn_CompoundLit_e){
_LL1472: _temp1471=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1311)->f1; goto _LL1470; _LL1470: _temp1469=( struct Cyc_List_List*)((
struct Cyc_Absyn_CompoundLit_e_struct*) _temp1311)->f2; goto _LL1362;} else{
goto _LL1363;} _LL1363: if(*(( int*) _temp1311) == Cyc_Absyn_Array_e){ _LL1474:
_temp1473=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1311)->f1;
goto _LL1364;} else{ goto _LL1365;} _LL1365: if(*(( int*) _temp1311) == Cyc_Absyn_Comprehension_e){
_LL1480: _temp1479=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1311)->f1; goto _LL1478; _LL1478: _temp1477=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1311)->f2; goto _LL1476; _LL1476:
_temp1475=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1311)->f3; goto _LL1366;} else{ goto _LL1367;} _LL1367: if(*(( int*)
_temp1311) == Cyc_Absyn_Struct_e){ _LL1488: _temp1487=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1311)->f1; goto _LL1486; _LL1486:
_temp1485=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1311)->f2;
goto _LL1484; _LL1484: _temp1483=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1311)->f3; goto _LL1482; _LL1482: _temp1481=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1311)->f4; goto _LL1368;} else{ goto
_LL1369;} _LL1369: if(*(( int*) _temp1311) == Cyc_Absyn_AnonStruct_e){ _LL1492:
_temp1491=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1311)->f1; goto
_LL1490; _LL1490: _temp1489=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1311)->f2; goto _LL1370;} else{ goto _LL1371;} _LL1371: if(*(( int*)
_temp1311) == Cyc_Absyn_Tunion_e){ _LL1502: _temp1501=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1311)->f1; goto _LL1500; _LL1500:
_temp1499=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1311)->f2;
goto _LL1498; _LL1498: _temp1497=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1311)->f3; if( _temp1497 == 0){ goto _LL1496;} else{ goto _LL1373;} _LL1496:
_temp1495=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1311)->f4; goto _LL1494; _LL1494: _temp1493=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1311)->f5; goto _LL1372;} else{ goto
_LL1373;} _LL1373: if(*(( int*) _temp1311) == Cyc_Absyn_Tunion_e){ _LL1512:
_temp1511=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1311)->f1;
goto _LL1510; _LL1510: _temp1509=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1311)->f2; goto _LL1508; _LL1508: _temp1507=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1311)->f3; goto _LL1506; _LL1506:
_temp1505=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1311)->f4; goto _LL1504; _LL1504: _temp1503=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1311)->f5; goto _LL1374;} else{ goto
_LL1375;} _LL1375: if(*(( int*) _temp1311) == Cyc_Absyn_Enum_e){ _LL1518:
_temp1517=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1311)->f1;
goto _LL1516; _LL1516: _temp1515=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1311)->f2; goto _LL1514; _LL1514: _temp1513=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1311)->f3; goto _LL1376;} else{ goto
_LL1377;} _LL1377: if(*(( int*) _temp1311) == Cyc_Absyn_Malloc_e){ _LL1522:
_temp1521=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1311)->f1; goto _LL1520; _LL1520: _temp1519=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1311)->f2; goto _LL1378;} else{ goto _LL1379;} _LL1379: if(*(( int*)
_temp1311) == Cyc_Absyn_StmtExp_e){ _LL1524: _temp1523=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1311)->f1; goto _LL1380;} else{ goto
_LL1381;} _LL1381: if(*(( int*) _temp1311) == Cyc_Absyn_UnresolvedMem_e){
_LL1528: _temp1527=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1311)->f1; goto _LL1526; _LL1526: _temp1525=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1311)->f2; goto _LL1382;} else{
goto _LL1383;} _LL1383: if(*(( int*) _temp1311) == Cyc_Absyn_Codegen_e){ _LL1530:
_temp1529=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1311)->f1; goto _LL1384;} else{ goto _LL1385;} _LL1385: if(*(( int*)
_temp1311) == Cyc_Absyn_Fill_e){ _LL1532: _temp1531=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp1311)->f1; goto _LL1386;} else{ goto
_LL1312;} _LL1314:{ void* _temp1533= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1539; struct Cyc_Absyn_Conref* _temp1541; struct Cyc_Absyn_Tqual _temp1543;
struct Cyc_Absyn_Conref* _temp1545; void* _temp1547; void* _temp1549; _LL1535:
if(( unsigned int) _temp1533 > 4u?*(( int*) _temp1533) == Cyc_Absyn_PointerType:
0){ _LL1540: _temp1539=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1533)->f1; _LL1550: _temp1549=( void*) _temp1539.elt_typ; goto _LL1548;
_LL1548: _temp1547=( void*) _temp1539.rgn_typ; goto _LL1546; _LL1546: _temp1545=(
struct Cyc_Absyn_Conref*) _temp1539.nullable; goto _LL1544; _LL1544: _temp1543=(
struct Cyc_Absyn_Tqual) _temp1539.tq; goto _LL1542; _LL1542: _temp1541=( struct
Cyc_Absyn_Conref*) _temp1539.bounds; goto _LL1536;} else{ goto _LL1537;} _LL1537:
goto _LL1538; _LL1536: if(( void*) _temp1541->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1534;}{ void* _temp1551=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1541); struct Cyc_Absyn_Exp* _temp1557; _LL1553: if( _temp1551 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1554;} else{ goto _LL1555;} _LL1555: if((
unsigned int) _temp1551 > 1u?*(( int*) _temp1551) == Cyc_Absyn_Upper_b: 0){
_LL1558: _temp1557=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1551)->f1; goto _LL1556;} else{ goto _LL1552;} _LL1554: { struct _tuple4*
zero=({ struct _tuple4* _temp1562=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1562->f1= 0; _temp1562->f2= Cyc_Absyn_signed_int_exp( 0, 0);
_temp1562;}); struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp( 0,({ struct
Cyc_List_List* _temp1559=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1559->hd=( void*) zero; _temp1559->tl=({ struct Cyc_List_List* _temp1560=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1560->hd=(
void*) zero; _temp1560->tl=({ struct Cyc_List_List* _temp1561=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1561->hd=( void*) zero;
_temp1561->tl= 0; _temp1561;}); _temp1560;}); _temp1559;}), 0); if( nv->toplevel){(
void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1549), Cyc_Toc_mt_tq); struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*) um, Cyc_Absyn_exp_stmt(
xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}
goto _LL1552;} _LL1556:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1552; _LL1552:;} goto _LL1534; _LL1538:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1563= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1563.last_plus_one -
_temp1563.curr, _temp1563.curr);})); goto _LL1534; _LL1534:;} goto _LL1312;
_LL1316: goto _LL1312; _LL1318:{ struct _handler_cons _temp1564; _push_handler(&
_temp1564);{ int _temp1566= 0; if( setjmp( _temp1564.handler)){ _temp1566= 1;}
if( ! _temp1566){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1393))->r));; _pop_handler();} else{ void* _temp1565=( void*) _exn_thrown;
void* _temp1568= _temp1565; _LL1570: if( _temp1568 == Cyc_Dict_Absent){ goto
_LL1571;} else{ goto _LL1572;} _LL1572: goto _LL1573; _LL1571:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1574= Cyc_Absynpp_qvar2string(
_temp1393); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1574.last_plus_one
- _temp1574.curr, _temp1574.curr);})); return; _LL1573:( void) _throw( _temp1568);
_LL1569:;}}} goto _LL1312; _LL1320:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1575=( char*)"unknownid"; struct
_tagged_string _temp1576; _temp1576.curr= _temp1575; _temp1576.base= _temp1575;
_temp1576.last_plus_one= _temp1575 + 10; _temp1576;})); goto _LL1312; _LL1322: {
struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1397); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1397);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1397);{ void* _temp1577= _temp1399; _LL1579: if( _temp1577 ==( void*)
Cyc_Absyn_Size){ goto _LL1580;} else{ goto _LL1581;} _LL1581: if( _temp1577 ==(
void*) Cyc_Absyn_Printf){ goto _LL1582;} else{ goto _LL1583;} _LL1583: if(
_temp1577 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1584;} else{ goto _LL1585;}
_LL1585: if( _temp1577 ==( void*) Cyc_Absyn_Fprintf){ goto _LL1586;} else{ goto
_LL1587;} _LL1587: if( _temp1577 ==( void*) Cyc_Absyn_Sscanf){ goto _LL1588;}
else{ goto _LL1589;} _LL1589: if( _temp1577 ==( void*) Cyc_Absyn_Plus){ goto
_LL1590;} else{ goto _LL1591;} _LL1591: if( _temp1577 ==( void*) Cyc_Absyn_Minus){
goto _LL1592;} else{ goto _LL1593;} _LL1593: if( _temp1577 ==( void*) Cyc_Absyn_Eq){
goto _LL1594;} else{ goto _LL1595;} _LL1595: if( _temp1577 ==( void*) Cyc_Absyn_Neq){
goto _LL1596;} else{ goto _LL1597;} _LL1597: if( _temp1577 ==( void*) Cyc_Absyn_Gt){
goto _LL1598;} else{ goto _LL1599;} _LL1599: if( _temp1577 ==( void*) Cyc_Absyn_Gte){
goto _LL1600;} else{ goto _LL1601;} _LL1601: if( _temp1577 ==( void*) Cyc_Absyn_Lt){
goto _LL1602;} else{ goto _LL1603;} _LL1603: if( _temp1577 ==( void*) Cyc_Absyn_Lte){
goto _LL1604;} else{ goto _LL1605;} _LL1605: goto _LL1606; _LL1580: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1640= _temp1397; if(
_temp1640 == 0){ _throw( Null_Exception);} _temp1640->hd;});{ void* _temp1608=(
void*)({ struct Cyc_Core_Opt* _temp1607= arg->topt; if( _temp1607 == 0){ _throw(
Null_Exception);} _temp1607->v;}); struct Cyc_Absyn_Exp* _temp1616; struct Cyc_Absyn_Tqual
_temp1618; void* _temp1620; struct Cyc_Absyn_PtrInfo _temp1622; struct Cyc_Absyn_Conref*
_temp1624; struct Cyc_Absyn_Tqual _temp1626; struct Cyc_Absyn_Conref* _temp1628;
void* _temp1630; void* _temp1632; _LL1610: if(( unsigned int) _temp1608 > 4u?*((
int*) _temp1608) == Cyc_Absyn_ArrayType: 0){ _LL1621: _temp1620=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1608)->f1; goto _LL1619; _LL1619:
_temp1618=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1608)->f2; goto _LL1617; _LL1617: _temp1616=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1608)->f3; goto _LL1611;} else{ goto
_LL1612;} _LL1612: if(( unsigned int) _temp1608 > 4u?*(( int*) _temp1608) == Cyc_Absyn_PointerType:
0){ _LL1623: _temp1622=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1608)->f1; _LL1633: _temp1632=( void*) _temp1622.elt_typ; goto _LL1631;
_LL1631: _temp1630=( void*) _temp1622.rgn_typ; goto _LL1629; _LL1629: _temp1628=(
struct Cyc_Absyn_Conref*) _temp1622.nullable; goto _LL1627; _LL1627: _temp1626=(
struct Cyc_Absyn_Tqual) _temp1622.tq; goto _LL1625; _LL1625: _temp1624=( struct
Cyc_Absyn_Conref*) _temp1622.bounds; goto _LL1613;} else{ goto _LL1614;} _LL1614:
goto _LL1615; _LL1611:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Exp*
_temp1634= _temp1616; if( _temp1634 == 0){ _throw( Null_Exception);} _temp1634->r;})));
goto _LL1609; _LL1613: { struct _tuple0* temp= Cyc_Toc_temp_var(); void*
temp_typ= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1635= arg->topt;
if( _temp1635 == 0){ _throw( Null_Exception);} _temp1635->v;})); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
Cyc_Absyn_subtract_exp( e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt(
e4, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1609;}
_LL1615:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1638= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_List_List*
_temp1636= typs; if( _temp1636 == 0){ _throw( Null_Exception);} _temp1636->hd;}));
struct _tagged_string _temp1639= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1637= arg->topt; if( _temp1637 == 0){ _throw( Null_Exception);} _temp1637->v;}));
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1638.last_plus_one
- _temp1638.curr, _temp1638.curr, _temp1639.last_plus_one - _temp1639.curr,
_temp1639.curr);})); return; _LL1609:;} goto _LL1578;} _LL1582:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1399, 0, _temp1397, typs))->r)); goto
_LL1578; _LL1584:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1399, 0, _temp1397, typs))->r)); goto _LL1578; _LL1586: if( _temp1397 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1641=( char*)"fprintf without arguments"; struct _tagged_string
_temp1642; _temp1642.curr= _temp1641; _temp1642.base= _temp1641; _temp1642.last_plus_one=
_temp1641 + 26; _temp1642;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1399,({ struct Cyc_Core_Opt* _temp1643=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1643->v=( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1644= _temp1397; if( _temp1644 == 0){ _throw(
Null_Exception);} _temp1644->hd;})); _temp1643;}),({ struct Cyc_List_List*
_temp1645= _temp1397; if( _temp1645 == 0){ _throw( Null_Exception);} _temp1645->tl;}),({
struct Cyc_List_List* _temp1646= typs; if( _temp1646 == 0){ _throw(
Null_Exception);} _temp1646->tl;})))->r)); goto _LL1578; _LL1588: if( _temp1397
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1647=( char*)"sscanf without arguments"; struct
_tagged_string _temp1648; _temp1648.curr= _temp1647; _temp1648.base= _temp1647;
_temp1648.last_plus_one= _temp1647 + 25; _temp1648;}));}( void*)((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1649= _temp1397; if( _temp1649 == 0){ _throw(
Null_Exception);} _temp1649->hd;}))->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1650= _temp1397; if( _temp1650 == 0){ _throw( Null_Exception);} _temp1650->hd;}))->r,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1651= _temp1397; if(
_temp1651 == 0){ _throw( Null_Exception);} _temp1651->hd;}))->loc), Cyc_Toc_curr_sp,
0))->r)); goto _LL1578; _LL1590: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1655= _temp1397; if( _temp1655 == 0){ _throw( Null_Exception);} _temp1655->hd;});
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1654=({ struct Cyc_List_List* _temp1653= _temp1397; if( _temp1653 == 0){
_throw( Null_Exception);} _temp1653->tl;}); if( _temp1654 == 0){ _throw(
Null_Exception);} _temp1654->hd;}); struct _tuple0* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1652=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1652->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1652;}),
e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1578; _LL1592: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)({ struct Cyc_List_List* _temp1656= old_typs; if( _temp1656 == 0){ _throw(
Null_Exception);} _temp1656->hd;}))){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1663= _temp1397; if( _temp1663 == 0){ _throw(
Null_Exception);} _temp1663->hd;}); struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1662=({ struct Cyc_List_List* _temp1661= _temp1397;
if( _temp1661 == 0){ _throw( Null_Exception);} _temp1661->tl;}); if( _temp1662
== 0){ _throw( Null_Exception);} _temp1662->hd;}); void* old_typ_e2=( void*)({
struct Cyc_List_List* _temp1660=({ struct Cyc_List_List* _temp1659= old_typs;
if( _temp1659 == 0){ _throw( Null_Exception);} _temp1659->tl;}); if( _temp1660
== 0){ _throw( Null_Exception);} _temp1660->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_List_List* _temp1658= old_typs; if( _temp1658 == 0){ _throw(
Null_Exception);} _temp1658->hd;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1657=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1657->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp1657;}), e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0);
s= Cyc_Absyn_declare_stmt( temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1578;
_LL1594: goto _LL1596; _LL1596: goto _LL1598; _LL1598: goto _LL1600; _LL1600:
goto _LL1602; _LL1602: goto _LL1604; _LL1604: { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1669= _temp1397; if(
_temp1669 == 0){ _throw( Null_Exception);} _temp1669->hd;}); struct Cyc_Absyn_Exp*
e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1668=({ struct Cyc_List_List*
_temp1667= _temp1397; if( _temp1667 == 0){ _throw( Null_Exception);} _temp1667->tl;});
if( _temp1668 == 0){ _throw( Null_Exception);} _temp1668->hd;}); void* t1=( void*)({
struct Cyc_List_List* _temp1666= old_typs; if( _temp1666 == 0){ _throw(
Null_Exception);} _temp1666->hd;}); void* t2=( void*)({ struct Cyc_List_List*
_temp1665=({ struct Cyc_List_List* _temp1664= old_typs; if( _temp1664 == 0){
_throw( Null_Exception);} _temp1664->tl;}); if( _temp1665 == 0){ _throw(
Null_Exception);} _temp1665->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1578;} _LL1606: goto
_LL1578; _LL1578:;} goto _LL1312;} _LL1324: { void* e2_cyc_typ=( void*)({ struct
Cyc_Core_Opt* _temp1670= _temp1403->topt; if( _temp1670 == 0){ _throw(
Null_Exception);} _temp1670->v;}); Cyc_Toc_exp_to_c( nv, _temp1403); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){ if( _temp1401 ==( void*) Cyc_Absyn_PreInc? 1: _temp1401 ==( void*)
Cyc_Absyn_PreDec){ struct _tuple0* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Absyn_star_typ(
Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
temp, 0), 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0), _temp1401, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp, temp_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1403, 0), s, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));} else{ struct _tuple0* temp=
Cyc_Toc_temp_var(); struct _tuple0* temp2= Cyc_Toc_temp_var(); void* temp2_typ=
Cyc_Toc_typ_to_c( e2_cyc_typ); void* temp_typ= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c(
e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp2, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( temp,
0), Cyc_Toc_curr_sp, 0), _temp1401, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp2, temp2_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( temp, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1403, 0), s,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} else{
if( ! Cyc_Absyn_is_lvalue( _temp1403)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1403, 0, Cyc_Toc_incr_lvalue,
_temp1401);( void*)( e->r=( void*)(( void*) _temp1403->r));}} goto _LL1312;}
_LL1326: { int e1_poly= Cyc_Toc_is_poly_project( _temp1409); void* e1_old_typ=(
void*)({ struct Cyc_Core_Opt* _temp1676= _temp1409->topt; if( _temp1676 == 0){
_throw( Null_Exception);} _temp1676->v;}); void* e2_old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1675= _temp1405->topt; if( _temp1675 == 0){ _throw(
Null_Exception);} _temp1675->v;}); Cyc_Toc_exp_to_c( nv, _temp1409); Cyc_Toc_exp_to_c(
nv, _temp1405); if( _temp1407 != 0?( void*)({ struct Cyc_Core_Opt* _temp1671=
_temp1407; if( _temp1671 == 0){ _throw( Null_Exception);} _temp1671->v;}) ==(
void*) Cyc_Absyn_Plus? 1:( void*)({ struct Cyc_Core_Opt* _temp1672= _temp1407;
if( _temp1672 == 0){ _throw( Null_Exception);} _temp1672->v;}) ==( void*) Cyc_Absyn_Minus:
0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){( void*)( _temp1409->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)
_temp1409->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1405->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1405->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1405->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1405->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1409)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1409, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1673=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1673[ 0]=({ struct _tuple11 _temp1674; _temp1674.f1=
_temp1407; _temp1674.f2= _temp1405; _temp1674;}); _temp1673;}));( void*)( e->r=(
void*)(( void*) _temp1409->r));} goto _LL1312;} _LL1328: Cyc_Toc_exp_to_c( nv,
_temp1415); Cyc_Toc_exp_to_c( nv, _temp1413); Cyc_Toc_exp_to_c( nv, _temp1411);
goto _LL1312; _LL1330: Cyc_Toc_exp_to_c( nv, _temp1419); Cyc_Toc_exp_to_c( nv,
_temp1417); goto _LL1312; _LL1332: _temp1427= _temp1423; _temp1425= _temp1421;
goto _LL1334; _LL1334: Cyc_Toc_exp_to_c( nv, _temp1427);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1425); goto _LL1312; _LL1336:
Cyc_Toc_exp_to_c( nv, _temp1429);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1429), 0))->r)); goto
_LL1312; _LL1338: Cyc_Toc_exp_to_c( nv, _temp1431); goto _LL1312; _LL1340: Cyc_Toc_exp_to_c(
nv, _temp1435);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1679= e->topt; if( _temp1679 == 0){ _throw( Null_Exception);} _temp1679->v;}));
for( 0; _temp1433 != 0; _temp1433=({ struct Cyc_List_List* _temp1677= _temp1433;
if( _temp1677 == 0){ _throw( Null_Exception);} _temp1677->tl;})){ void* k= Cyc_Tcutil_typ_kind((
void*)({ struct Cyc_List_List* _temp1678= _temp1433; if( _temp1678 == 0){ _throw(
Null_Exception);} _temp1678->hd;})); if( k !=( void*) Cyc_Absyn_EffKind? k !=(
void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1435, 0))->r)); break;}} goto _LL1312;} _LL1342: { void* old_t2=(
void*)({ struct Cyc_Core_Opt* _temp1680=(* _temp1439)->topt; if( _temp1680 == 0){
_throw( Null_Exception);} _temp1680->v;}); void* new_typ=* _temp1442;* _temp1442=
Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,* _temp1439);* _temp1439= Cyc_Toc_convert_exp(
nv->toplevel, old_t2, new_typ,* _temp1439); goto _LL1312;} _LL1344:{ void*
_temp1681=( void*) _temp1443->r; struct Cyc_Absyn_Structdecl* _temp1689; struct
Cyc_List_List* _temp1691; struct Cyc_Core_Opt* _temp1693; struct _tuple0*
_temp1695; struct Cyc_List_List* _temp1697; _LL1683: if(*(( int*) _temp1681) ==
Cyc_Absyn_Struct_e){ _LL1696: _temp1695=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1681)->f1; goto _LL1694; _LL1694: _temp1693=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1681)->f2; goto _LL1692; _LL1692:
_temp1691=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1681)->f3; goto _LL1690; _LL1690: _temp1689=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1681)->f4; goto _LL1684;} else{ goto
_LL1685;} _LL1685: if(*(( int*) _temp1681) == Cyc_Absyn_Tuple_e){ _LL1698:
_temp1697=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1681)->f1;
goto _LL1686;} else{ goto _LL1687;} _LL1687: goto _LL1688; _LL1684: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string
_temp1699= Cyc_Position_string_of_segment( _temp1443->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1699.last_plus_one - _temp1699.curr, _temp1699.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)({ struct Cyc_Core_Opt*
_temp1700= _temp1443->topt; if( _temp1700 == 0){ _throw( Null_Exception);}
_temp1700->v;}), 1, 0, _temp1691, _temp1695))->r)); goto _LL1682; _LL1686: if(
nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct
_tagged_string _temp1701= Cyc_Position_string_of_segment( _temp1443->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1701.last_plus_one -
_temp1701.curr, _temp1701.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, 0, _temp1697))->r)); goto _LL1682; _LL1688: Cyc_Toc_exp_to_c( nv,
_temp1443); if( ! Cyc_Absyn_is_lvalue( _temp1443)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1443, 0, Cyc_Toc_address_lvalue,
1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1702= e->topt; if( _temp1702 == 0){ _throw(
Null_Exception);} _temp1702->v;})), _temp1443, 0))->r));} goto _LL1682; _LL1682:;}
goto _LL1312; _LL1346: if( nv->toplevel){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ struct _tagged_string _temp1703= Cyc_Position_string_of_segment(
_temp1445->loc); xprintf("%.*s: new at top-level", _temp1703.last_plus_one -
_temp1703.curr, _temp1703.curr);}));}{ void* _temp1704=( void*) _temp1445->r;
struct Cyc_List_List* _temp1718; void* _temp1720; struct _tagged_string
_temp1722; struct Cyc_Absyn_Exp* _temp1724; struct Cyc_Absyn_Exp* _temp1726;
struct Cyc_Absyn_Vardecl* _temp1728; struct Cyc_Absyn_Structdecl* _temp1730;
struct Cyc_List_List* _temp1732; struct Cyc_Core_Opt* _temp1734; struct _tuple0*
_temp1736; struct Cyc_List_List* _temp1738; _LL1706: if(*(( int*) _temp1704) ==
Cyc_Absyn_Array_e){ _LL1719: _temp1718=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1704)->f1; goto _LL1707;} else{ goto _LL1708;} _LL1708: if(*(( int*)
_temp1704) == Cyc_Absyn_Const_e){ _LL1721: _temp1720=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1704)->f1; if(( unsigned int) _temp1720 > 1u?*(( int*) _temp1720) == Cyc_Absyn_String_c:
0){ _LL1723: _temp1722=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1720)->f1; goto _LL1709;} else{ goto _LL1710;}} else{ goto _LL1710;}
_LL1710: if(*(( int*) _temp1704) == Cyc_Absyn_Comprehension_e){ _LL1729:
_temp1728=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1704)->f1; goto _LL1727; _LL1727: _temp1726=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1704)->f2; goto _LL1725; _LL1725:
_temp1724=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1704)->f3; goto _LL1711;} else{ goto _LL1712;} _LL1712: if(*(( int*)
_temp1704) == Cyc_Absyn_Struct_e){ _LL1737: _temp1736=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1704)->f1; goto _LL1735; _LL1735:
_temp1734=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1704)->f2;
goto _LL1733; _LL1733: _temp1732=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1704)->f3; goto _LL1731; _LL1731: _temp1730=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1704)->f4; goto _LL1713;} else{ goto
_LL1714;} _LL1714: if(*(( int*) _temp1704) == Cyc_Absyn_Tuple_e){ _LL1739:
_temp1738=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1704)->f1;
goto _LL1715;} else{ goto _LL1716;} _LL1716: goto _LL1717; _LL1707: { struct
_tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x,
0); struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_array( nv, xexp, _temp1718, Cyc_Absyn_exp_stmt(
xexp, 0)); void* old_elt_typ;{ void* _temp1740= Cyc_Tcutil_compress( old_typ);
struct Cyc_Absyn_PtrInfo _temp1746; struct Cyc_Absyn_Conref* _temp1748; struct
Cyc_Absyn_Tqual _temp1750; struct Cyc_Absyn_Conref* _temp1752; void* _temp1754;
void* _temp1756; _LL1742: if(( unsigned int) _temp1740 > 4u?*(( int*) _temp1740)
== Cyc_Absyn_PointerType: 0){ _LL1747: _temp1746=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1740)->f1; _LL1757: _temp1756=( void*)
_temp1746.elt_typ; goto _LL1755; _LL1755: _temp1754=( void*) _temp1746.rgn_typ;
goto _LL1753; _LL1753: _temp1752=( struct Cyc_Absyn_Conref*) _temp1746.nullable;
goto _LL1751; _LL1751: _temp1750=( struct Cyc_Absyn_Tqual) _temp1746.tq; goto
_LL1749; _LL1749: _temp1748=( struct Cyc_Absyn_Conref*) _temp1746.bounds; goto
_LL1743;} else{ goto _LL1744;} _LL1744: goto _LL1745; _LL1743: old_elt_typ=
_temp1756; goto _LL1741; _LL1745: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1758=( char*)"exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1759; _temp1759.curr= _temp1758; _temp1759.base=
_temp1758; _temp1759.last_plus_one= _temp1758 + 52; _temp1759;})); goto _LL1741;
_LL1741:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1718), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1447 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, e2);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1760= _temp1447; if( _temp1760 == 0){ _throw(
Null_Exception);} _temp1760;}); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp(
r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,(
struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1705;}}} _LL1709: if(
_temp1447 == 0){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1761=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1761->f1= Cyc_Toc_abs_ns; _temp1761->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1761;}), 0),({ struct Cyc_List_List* _temp1762=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1762->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1763=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1763[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1764; _temp1764.tag= Cyc_Absyn_Const_e; _temp1764.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1765=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1765[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1766; _temp1766.tag= Cyc_Absyn_String_c;
_temp1766.f1= _temp1722; _temp1766;}); _temp1765;})); _temp1764;}); _temp1763;}),
0); _temp1762->tl= 0; _temp1762;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1773= _temp1447; if(
_temp1773 == 0){ _throw( Null_Exception);} _temp1773;}); Cyc_Toc_exp_to_c( nv, r);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1767=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1767->f1= Cyc_Toc_abs_ns; _temp1767->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1767;}), 0),({ struct Cyc_List_List* _temp1768=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1768->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1769=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1769[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1770; _temp1770.tag= Cyc_Absyn_Const_e; _temp1770.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1771=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1771[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1772; _temp1772.tag= Cyc_Absyn_String_c;
_temp1772.f1= _temp1722; _temp1772;}); _temp1771;})); _temp1770;}); _temp1769;}),
0); _temp1768->tl= 0; _temp1768;}), 0))->r));} goto _LL1705; _LL1711: { int
is_tagged_ptr= 0;{ void* _temp1774= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1780; struct Cyc_Absyn_Conref* _temp1782; struct Cyc_Absyn_Tqual _temp1784;
struct Cyc_Absyn_Conref* _temp1786; void* _temp1788; void* _temp1790; _LL1776:
if(( unsigned int) _temp1774 > 4u?*(( int*) _temp1774) == Cyc_Absyn_PointerType:
0){ _LL1781: _temp1780=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1774)->f1; _LL1791: _temp1790=( void*) _temp1780.elt_typ; goto _LL1789;
_LL1789: _temp1788=( void*) _temp1780.rgn_typ; goto _LL1787; _LL1787: _temp1786=(
struct Cyc_Absyn_Conref*) _temp1780.nullable; goto _LL1785; _LL1785: _temp1784=(
struct Cyc_Absyn_Tqual) _temp1780.tq; goto _LL1783; _LL1783: _temp1782=( struct
Cyc_Absyn_Conref*) _temp1780.bounds; goto _LL1777;} else{ goto _LL1778;} _LL1778:
goto _LL1779; _LL1777:{ void* _temp1792=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1782))->v; void*
_temp1798; _LL1794: if(( unsigned int) _temp1792 > 1u?*(( int*) _temp1792) ==
Cyc_Absyn_Eq_constr: 0){ _LL1799: _temp1798=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1792)->f1; if( _temp1798 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1795;}
else{ goto _LL1796;}} else{ goto _LL1796;} _LL1796: goto _LL1797; _LL1795:
is_tagged_ptr= 1; goto _LL1793; _LL1797: goto _LL1793; _LL1793:;} goto _LL1775;
_LL1779:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1800=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1801; _temp1801.curr= _temp1800; _temp1801.base=
_temp1800; _temp1801.last_plus_one= _temp1800 + 42; _temp1801;})); goto _LL1775;
_LL1775:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)({ struct Cyc_Core_Opt* _temp1825= _temp1724->topt;
if( _temp1825 == 0){ _throw( Null_Exception);} _temp1825->v;}); void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1726);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1728, Cyc_Absyn_var_exp( max, 0), _temp1724,
Cyc_Absyn_skip_stmt( 0), 1); struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1823=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1823->hd=( void*)({ struct _tuple10* _temp1824=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1824->f1= max; _temp1824->f2= Cyc_Absyn_uint_t;
_temp1824->f3=( struct Cyc_Absyn_Exp*) _temp1726; _temp1824;}); _temp1823->tl= 0;
_temp1823;}); struct Cyc_Absyn_Exp* ai; if( _temp1447 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp1802= _temp1447; if( _temp1802 == 0){ _throw( Null_Exception);}
_temp1802;}); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct
Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1803=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1803->hd=( void*)({ struct _tuple10* _temp1804=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1804->f1= a; _temp1804->f2= ptr_typ;
_temp1804->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1804;}); _temp1803->tl= decls;
_temp1803;}); if( is_tagged_ptr){ struct _tuple0* b= Cyc_Toc_temp_var(); void* t=
Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* binit= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp1807=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1807->hd=( void*)({ struct _tuple4* _temp1812=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1812->f1= 0;
_temp1812->f2= Cyc_Absyn_var_exp( a, 0); _temp1812;}); _temp1807->tl=({ struct
Cyc_List_List* _temp1808=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1808->hd=( void*)({ struct _tuple4* _temp1811=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1811->f1= 0; _temp1811->f2= Cyc_Absyn_var_exp( a,
0); _temp1811;}); _temp1808->tl=({ struct Cyc_List_List* _temp1809=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1809->hd=( void*)({ struct
_tuple4* _temp1810=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1810->f1= 0; _temp1810->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1810;}); _temp1809->tl= 0; _temp1809;});
_temp1808;}); _temp1807;}), 0); decls=({ struct Cyc_List_List* _temp1805=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1805->hd=(
void*)({ struct _tuple10* _temp1806=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1806->f1= b; _temp1806->f2= t; _temp1806->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1806;}); _temp1805->tl= decls; _temp1805;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp1813= ds; if(
_temp1813 == 0){ _throw( Null_Exception);} _temp1813->tl;})){ struct Cyc_Absyn_Exp*
_temp1817; void* _temp1819; struct _tuple0* _temp1821; struct _tuple10 _temp1815=*((
struct _tuple10*)({ struct Cyc_List_List* _temp1814= ds; if( _temp1814 == 0){
_throw( Null_Exception);} _temp1814->hd;})); _LL1822: _temp1821= _temp1815.f1;
goto _LL1820; _LL1820: _temp1819= _temp1815.f2; goto _LL1818; _LL1818: _temp1817=
_temp1815.f3; goto _LL1816; _LL1816: s= Cyc_Absyn_declare_stmt( _temp1821,
_temp1819, _temp1817, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r)); goto _LL1705;}}}} _LL1713:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1826= _temp1445->topt; if( _temp1826 ==
0){ _throw( Null_Exception);} _temp1826->v;}), 1, _temp1447, _temp1732,
_temp1736))->r)); goto _LL1705; _LL1715:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1447, _temp1738))->r)); goto _LL1705; _LL1717: { void* old_elt_typ=(
void*)({ struct Cyc_Core_Opt* _temp1828= _temp1445->topt; if( _temp1828 == 0){
_throw( Null_Exception);} _temp1828->v;}); struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0);
struct Cyc_Absyn_Exp* e_index= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c(
nv, _temp1445); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1445, 0), s, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); struct Cyc_Absyn_Exp*
e1; if( _temp1447 == 0){ e1= Cyc_Toc_malloc_exp( old_elt_typ, e2);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1827=
_temp1447; if( _temp1827 == 0){ _throw( Null_Exception);} _temp1827;}); Cyc_Toc_exp_to_c(
nv, r); e1= Cyc_Toc_rmalloc_exp( r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp(
ptr_typ, e1, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, ptr_typ,( struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1705;}}}
_LL1705:;} goto _LL1312; _LL1348:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1829=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1829[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1830; _temp1830.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1830.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1449);
_temp1830;}); _temp1829;}))); goto _LL1312; _LL1350: Cyc_Toc_exp_to_c( nv,
_temp1451); goto _LL1312; _LL1352: { void* e1_typ= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp1861= _temp1453->topt; if( _temp1861 == 0){ _throw(
Null_Exception);} _temp1861->v;})); Cyc_Toc_exp_to_c( nv, _temp1453);{ void*
_temp1831= e1_typ; struct Cyc_Absyn_PtrInfo _temp1837; struct Cyc_Absyn_Conref*
_temp1839; struct Cyc_Absyn_Tqual _temp1841; struct Cyc_Absyn_Conref* _temp1843;
void* _temp1845; void* _temp1847; _LL1833: if(( unsigned int) _temp1831 > 4u?*((
int*) _temp1831) == Cyc_Absyn_PointerType: 0){ _LL1838: _temp1837=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1831)->f1; _LL1848: _temp1847=( void*)
_temp1837.elt_typ; goto _LL1846; _LL1846: _temp1845=( void*) _temp1837.rgn_typ;
goto _LL1844; _LL1844: _temp1843=( struct Cyc_Absyn_Conref*) _temp1837.nullable;
goto _LL1842; _LL1842: _temp1841=( struct Cyc_Absyn_Tqual) _temp1837.tq; goto
_LL1840; _LL1840: _temp1839=( struct Cyc_Absyn_Conref*) _temp1837.bounds; goto
_LL1834;} else{ goto _LL1835;} _LL1835: goto _LL1836; _LL1834:{ void* _temp1849=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1839); struct
Cyc_Absyn_Exp* _temp1855; _LL1851: if( _temp1849 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1852;} else{ goto _LL1853;} _LL1853: if(( unsigned int) _temp1849 > 1u?*((
int*) _temp1849) == Cyc_Absyn_Upper_b: 0){ _LL1856: _temp1855=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1849)->f1; goto _LL1854;} else{ goto
_LL1850;} _LL1852: { struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array( _temp1847); void* tx= Cyc_Toc_add_tagged_ptr_type(
ta1, _temp1841); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1841); struct Cyc_Absyn_Exp*
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
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp1453, s1, 0), 0))->r));
goto _LL1850;} _LL1854: if( Cyc_Toc_is_nullable( e1_typ)){ if( Cyc_Toc_use_runtimecheck_funs){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp1857= _temp1453->topt; if( _temp1857 == 0){
_throw( Null_Exception);} _temp1857->v;})), Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1858=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1858->hd=( void*) _temp1453;
_temp1858->tl= 0; _temp1858;}), 0), 0), 0))->r));} else{ struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3=
Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, e1_typ,( struct Cyc_Absyn_Exp*) _temp1453, s3, 0), 0))->r));}}
goto _LL1850; _LL1850:;} goto _LL1832; _LL1836:(( void(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1859=( char*)"exp_to_c: Deref: non-pointer";
struct _tagged_string _temp1860; _temp1860.curr= _temp1859; _temp1860.base=
_temp1859; _temp1860.last_plus_one= _temp1859 + 29; _temp1860;})); goto _LL1832;
_LL1832:;} goto _LL1312;} _LL1354: Cyc_Toc_exp_to_c( nv, _temp1457); if( Cyc_Toc_is_poly_project(
e)){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp1862= e->topt; if( _temp1862 == 0){ _throw(
Null_Exception);} _temp1862->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}
goto _LL1312; _LL1356: Cyc_Toc_exp_to_c( nv, _temp1461);{ int is_poly= Cyc_Toc_is_poly_project(
e); if( Cyc_Toc_is_nullable(( void*)({ struct Cyc_Core_Opt* _temp1863= _temp1461->topt;
if( _temp1863 == 0){ _throw( Null_Exception);} _temp1863->v;}))){ if( Cyc_Toc_use_runtimecheck_funs){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1864= _temp1461->topt; if(
_temp1864 == 0){ _throw( Null_Exception);} _temp1864->v;})), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1865=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1865->hd=(
void*) _temp1461; _temp1865->tl= 0; _temp1865;}), 0), 0), _temp1459, 0))->r));}
else{ struct _tuple0* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1866= _temp1461->topt; if( _temp1866 == 0){ _throw(
Null_Exception);} _temp1866->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( x, 0), _temp1459, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
s1, s2, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, tx,( struct Cyc_Absyn_Exp*) _temp1461, s3, 0), 0))->r));}} if( is_poly){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp1867= e->topt; if( _temp1867 == 0){ _throw(
Null_Exception);} _temp1867->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}
goto _LL1312;} _LL1358: { void* e1_typ= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp1928= _temp1465->topt; if( _temp1928 == 0){ _throw(
Null_Exception);} _temp1928->v;})); Cyc_Toc_exp_to_c( nv, _temp1465); Cyc_Toc_exp_to_c(
nv, _temp1463);( void*)( _temp1465->r=( void*)(( void*)( Cyc_Toc_array_to_ptr(
Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp(( void*) _temp1465->r, 0), 0))->r));{
void* _temp1868= e1_typ; struct Cyc_List_List* _temp1876; struct Cyc_Absyn_PtrInfo
_temp1878; struct Cyc_Absyn_Conref* _temp1880; struct Cyc_Absyn_Tqual _temp1882;
struct Cyc_Absyn_Conref* _temp1884; void* _temp1886; void* _temp1888; _LL1870:
if(( unsigned int) _temp1868 > 4u?*(( int*) _temp1868) == Cyc_Absyn_TupleType: 0){
_LL1877: _temp1876=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1868)->f1; goto _LL1871;} else{ goto _LL1872;} _LL1872: if(( unsigned int)
_temp1868 > 4u?*(( int*) _temp1868) == Cyc_Absyn_PointerType: 0){ _LL1879:
_temp1878=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1868)->f1; _LL1889: _temp1888=( void*) _temp1878.elt_typ; goto _LL1887;
_LL1887: _temp1886=( void*) _temp1878.rgn_typ; goto _LL1885; _LL1885: _temp1884=(
struct Cyc_Absyn_Conref*) _temp1878.nullable; goto _LL1883; _LL1883: _temp1882=(
struct Cyc_Absyn_Tqual) _temp1878.tq; goto _LL1881; _LL1881: _temp1880=( struct
Cyc_Absyn_Conref*) _temp1878.bounds; goto _LL1873;} else{ goto _LL1874;} _LL1874:
goto _LL1875; _LL1871: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1463) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1465,({ struct _tagged_string* _temp1890=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1890[ 0]= xprintf("f%d",( int)
i); _temp1890;}), 0))->r)); goto _LL1869;} _LL1873:{ void* _temp1891=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1880); struct Cyc_Absyn_Exp*
_temp1897; _LL1893: if(( unsigned int) _temp1891 > 1u?*(( int*) _temp1891) ==
Cyc_Absyn_Upper_b: 0){ _LL1898: _temp1897=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1891)->f1; goto _LL1894;} else{ goto _LL1895;} _LL1895: if( _temp1891 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1896;} else{ goto _LL1892;} _LL1894:
_temp1897= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp( _temp1897), 0);{
int possibly_null;{ void* _temp1899=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1884))->v; int
_temp1905; _LL1901: if(( unsigned int) _temp1899 > 1u?*(( int*) _temp1899) ==
Cyc_Absyn_Eq_constr: 0){ _LL1906: _temp1905=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1899)->f1; goto _LL1902;} else{ goto _LL1903;} _LL1903: goto _LL1904;
_LL1902: possibly_null= _temp1905; goto _LL1900; _LL1904: possibly_null= 0; goto
_LL1900; _LL1900:;}{ void* ta1= Cyc_Toc_typ_to_c( _temp1888); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1882); if( Cyc_Toc_use_runtimecheck_funs){ if( possibly_null){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv, 0),({ struct Cyc_List_List*
_temp1907=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1907->hd=( void*) _temp1465; _temp1907->tl=({ struct Cyc_List_List*
_temp1908=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1908->hd=( void*) _temp1897; _temp1908->tl=({ struct Cyc_List_List*
_temp1909=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1909->hd=( void*) Cyc_Absyn_sizeoftyp_exp( ta1, 0); _temp1909->tl=({ struct
Cyc_List_List* _temp1910=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1910->hd=( void*) _temp1463; _temp1910->tl= 0; _temp1910;}); _temp1909;});
_temp1908;}); _temp1907;}), 0), 0), 0))->r));} else{( void*)( _temp1463->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List* _temp1911=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1911->hd=( void*) _temp1897; _temp1911->tl=({
struct Cyc_List_List* _temp1912=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1912->hd=( void*) Cyc_Absyn_copy_exp( _temp1463);
_temp1912->tl= 0; _temp1912;}); _temp1911;}), 0))->r));}} else{ struct _tuple0*
a= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte, Cyc_Absyn_var_exp( i, 0),
_temp1897, 0); if( possibly_null){ e3= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_new_exp(( void*) e3->r, 0),
0);}{ struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt(
i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) _temp1463, s3, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*)
_temp1465, s1, 0), 0))->r));}} goto _LL1892;}} _LL1896: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp1888); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1882); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* argtyps=({ struct Cyc_List_List* _temp1919=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1919->hd=( void*)({ struct
_tuple1* _temp1924=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1924->f1= 0; _temp1924->f2= Cyc_Toc_mt_tq; _temp1924->f3= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp1925= _temp1465->topt; if( _temp1925 == 0){
_throw( Null_Exception);} _temp1925->v;})); _temp1924;}); _temp1919->tl=({
struct Cyc_List_List* _temp1920=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1920->hd=( void*)({ struct _tuple1* _temp1923=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1923->f1= 0;
_temp1923->f2= Cyc_Toc_mt_tq; _temp1923->f3= Cyc_Absyn_uint_t; _temp1923;});
_temp1920->tl=({ struct Cyc_List_List* _temp1921=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1921->hd=( void*)({ struct
_tuple1* _temp1922=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1922->f1= 0; _temp1922->f2= Cyc_Toc_mt_tq; _temp1922->f3= Cyc_Absyn_uint_t;
_temp1922;}); _temp1921->tl= 0; _temp1921;}); _temp1920;}); _temp1919;}); void*
fntyp= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct* _temp1916=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1916[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1917; _temp1917.tag= Cyc_Absyn_FnType;
_temp1917.f1=({ struct Cyc_Absyn_FnInfo _temp1918; _temp1918.tvars= 0; _temp1918.effect=
0; _temp1918.ret_typ=( void*) ta2; _temp1918.args= argtyps; _temp1918.varargs= 0;
_temp1918.attributes= 0; _temp1918;}); _temp1917;}); _temp1916;}), Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* fnexp= Cyc_Absyn_cast_exp( fntyp, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv,
0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_fncall_exp(
fnexp,({ struct Cyc_List_List* _temp1913=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1913->hd=( void*) _temp1465; _temp1913->tl=({
struct Cyc_List_List* _temp1914=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1914->hd=( void*) Cyc_Absyn_sizeoftyp_exp( ta1, 0);
_temp1914->tl=({ struct Cyc_List_List* _temp1915=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1915->hd=( void*) _temp1463;
_temp1915->tl= 0; _temp1915;}); _temp1914;}); _temp1913;}), 0), 0))->r));} else{
struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type(
ta1, _temp1882); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1463, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1465, s1, 0), 0))->r));} goto _LL1892;} _LL1892:;}
goto _LL1869; _LL1875:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1926=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1927; _temp1927.curr= _temp1926; _temp1927.base=
_temp1926; _temp1927.last_plus_one= _temp1926 + 49; _temp1927;})); goto _LL1869;
_LL1869:;} goto _LL1312;} _LL1360: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1467))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1467); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp1467 != 0; _temp1467=({ struct Cyc_List_List* _temp1929=
_temp1467; if( _temp1929 == 0){ _throw( Null_Exception);} _temp1929->tl;}), i ++){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1930=
_temp1467; if( _temp1930 == 0){ _throw( Null_Exception);} _temp1930->hd;}));{
struct Cyc_List_List* des=({ struct Cyc_List_List* _temp1934=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1934->hd=( void*)(( void*)({
struct Cyc_Absyn_FieldName_struct* _temp1935=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp1935[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp1936; _temp1936.tag= Cyc_Absyn_FieldName;
_temp1936.f1=({ struct _tagged_string* _temp1937=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1937[ 0]= xprintf("f%d", i);
_temp1937;}); _temp1936;}); _temp1935;})); _temp1934->tl= 0; _temp1934;}); dles=({
struct Cyc_List_List* _temp1931=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1931->hd=( void*)({ struct _tuple4* _temp1932=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1932->f1=( struct Cyc_List_List*)
des; _temp1932->f2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1933=
_temp1467; if( _temp1933 == 0){ _throw( Null_Exception);} _temp1933->hd;});
_temp1932;}); _temp1931->tl= dles; _temp1931;});}}} dles=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp( 0,
dles, 0), 0))->r));} goto _LL1312; _LL1362:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1938=( char*)"compoundlit";
struct _tagged_string _temp1939; _temp1939.curr= _temp1938; _temp1939.base=
_temp1938; _temp1939.last_plus_one= _temp1938 + 12; _temp1939;})); goto _LL1312;
_LL1364:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1473, 0))->r));{ struct Cyc_List_List* dles= _temp1473; for( 0; dles != 0;
dles=({ struct Cyc_List_List* _temp1940= dles; if( _temp1940 == 0){ _throw(
Null_Exception);} _temp1940->tl;})){ struct _tuple4 _temp1944; struct Cyc_Absyn_Exp*
_temp1945; struct Cyc_List_List* _temp1947; struct _tuple4* _temp1942=( struct
_tuple4*)({ struct Cyc_List_List* _temp1941= dles; if( _temp1941 == 0){ _throw(
Null_Exception);} _temp1941->hd;}); _temp1944=* _temp1942; _LL1948: _temp1947=
_temp1944.f1; goto _LL1946; _LL1946: _temp1945= _temp1944.f2; goto _LL1943;
_LL1943: Cyc_Toc_exp_to_c( nv, _temp1945);}} goto _LL1312; _LL1366: {
unsigned int sz= Cyc_Evexp_eval_const_uint_exp( _temp1477); Cyc_Toc_exp_to_c( nv,
_temp1475);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp1475)){
unsigned int i= 0; for( 0; i < sz; i ++){ es=({ struct Cyc_List_List* _temp1949=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1949->hd=(
void*)({ struct _tuple4* _temp1950=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1950->f1= 0; _temp1950->f2= _temp1475; _temp1950;}); _temp1949->tl=
es; _temp1949;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL1312;}} _LL1368: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1483, _temp1487))->r));}
else{ struct _tuple0* n= _temp1487; struct Cyc_Absyn_Structdecl* sd2=( struct
Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl* _temp1963= _temp1481 == 0?((
struct Cyc_Absyn_Structdecl*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1962= Cyc_Absynpp_qvar2string( _temp1487); xprintf("no structdecl %.*s",
_temp1962.last_plus_one - _temp1962.curr, _temp1962.curr);})): _temp1481; if(
_temp1963 == 0){ _throw( Null_Exception);} _temp1963;}); struct _RegionHandle
_temp1951= _new_region(); struct _RegionHandle* rgn=& _temp1951; _push_region(
rgn);{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, e->loc,
_temp1483,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1961=(( struct
Cyc_Absyn_Structdecl*) sd2)->fields; if( _temp1961 == 0){ _throw( Null_Exception);}
_temp1961->v;})); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp1952=
fields; if( _temp1952 == 0){ _throw( Null_Exception);} _temp1952->tl;})){ struct
_tuple12 _temp1956; struct Cyc_Absyn_Exp* _temp1957; struct Cyc_Absyn_Structfield*
_temp1959; struct _tuple12* _temp1954=( struct _tuple12*)({ struct Cyc_List_List*
_temp1953= fields; if( _temp1953 == 0){ _throw( Null_Exception);} _temp1953->hd;});
_temp1956=* _temp1954; _LL1960: _temp1959= _temp1956.f1; goto _LL1958; _LL1958:
_temp1957= _temp1956.f2; goto _LL1955; _LL1955: { void* fieldtyp=( void*)
_temp1959->type; Cyc_Toc_exp_to_c( nv, _temp1957); if( Cyc_Toc_is_void_star(
fieldtyp)){( void*)( _temp1957->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1957->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0,
_temp1483, 0), 0))->r));}; _pop_region(& _temp1951);} goto _LL1312; _LL1370: {
struct Cyc_List_List* fs;{ void* _temp1964= Cyc_Tcutil_compress( _temp1491);
struct Cyc_List_List* _temp1970; _LL1966: if(( unsigned int) _temp1964 > 4u?*((
int*) _temp1964) == Cyc_Absyn_AnonStructType: 0){ _LL1971: _temp1970=( struct
Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*) _temp1964)->f1; goto
_LL1967;} else{ goto _LL1968;} _LL1968: goto _LL1969; _LL1967: fs= _temp1970;
goto _LL1965; _LL1969: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1972= Cyc_Absynpp_typ2string(
_temp1491); xprintf("anon struct has type %.*s", _temp1972.last_plus_one -
_temp1972.curr, _temp1972.curr);})); goto _LL1965; _LL1965:;}{ struct
_RegionHandle _temp1973= _new_region(); struct _RegionHandle* rgn=& _temp1973;
_push_region( rgn);{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1489, fs); for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp1974= fields; if( _temp1974 == 0){ _throw( Null_Exception);} _temp1974->tl;})){
struct _tuple12 _temp1978; struct Cyc_Absyn_Exp* _temp1979; struct Cyc_Absyn_Structfield*
_temp1981; struct _tuple12* _temp1976=( struct _tuple12*)({ struct Cyc_List_List*
_temp1975= fields; if( _temp1975 == 0){ _throw( Null_Exception);} _temp1975->hd;});
_temp1978=* _temp1976; _LL1982: _temp1981= _temp1978.f1; goto _LL1980; _LL1980:
_temp1979= _temp1978.f2; goto _LL1977; _LL1977: { void* fieldtyp=( void*)
_temp1981->type; Cyc_Toc_exp_to_c( nv, _temp1979); if( Cyc_Toc_is_void_star(
fieldtyp)){( void*)( _temp1979->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1979->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1489, 0))->r));}; _pop_region(&
_temp1973);} goto _LL1312;} _LL1372: { struct _tuple0* qv= _temp1493->name; if(
_temp1495->is_xtunion? 1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp(
qv, 0))->r));} else{ int tag_count= 0; struct Cyc_List_List* fields= _temp1495->fields
== 0? 0:( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1986= _temp1495->fields;
if( _temp1986 == 0){ _throw( Null_Exception);} _temp1986->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1983= fields;
if( _temp1983 == 0){ _throw( Null_Exception);} _temp1983->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1984= fields;
if( _temp1984 == 0){ _throw( Null_Exception);} _temp1984->hd;}))->typs == 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1985= fields; if( _temp1985
== 0){ _throw( Null_Exception);} _temp1985->tl;});}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto _LL1312;}
_LL1374: { struct _tuple0* qv= _temp1503->name; struct Cyc_List_List* tqts=
_temp1503->typs; struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp2014=( char*)"_struct"; struct
_tagged_string _temp2015; _temp2015.curr= _temp2014; _temp2015.base= _temp2014;
_temp2015.last_plus_one= _temp2014 + 8; _temp2015;}))); if( nv->toplevel){
struct Cyc_Absyn_Exp* tag_exp; if( _temp1505->is_xtunion){ tag_exp= Cyc_Absyn_var_exp(
qv, 0);} else{ int tag_count= 0; struct Cyc_List_List* fields= _temp1505->fields
== 0? 0:( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1990= _temp1505->fields;
if( _temp1990 == 0){ _throw( Null_Exception);} _temp1990->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1987= fields;
if( _temp1987 == 0){ _throw( Null_Exception);} _temp1987->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1988= fields;
if( _temp1988 == 0){ _throw( Null_Exception);} _temp1988->hd;}))->typs != 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1989= fields; if( _temp1989
== 0){ _throw( Null_Exception);} _temp1989->tl;});} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles= 0; for( 0; _temp1507
!= 0; _temp1507=({ struct Cyc_List_List* _temp1991= _temp1507; if( _temp1991 ==
0){ _throw( Null_Exception);} _temp1991->tl;}), tqts=({ struct Cyc_List_List*
_temp1992= tqts; if( _temp1992 == 0){ _throw( Null_Exception);} _temp1992->tl;})){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1998= _temp1507; if( _temp1998 == 0){ _throw( Null_Exception);} _temp1998->hd;});
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp1997= tqts; if( _temp1997 == 0){ _throw( Null_Exception);} _temp1997->hd;}))).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} dles=({ struct Cyc_List_List* _temp1993=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1993[ 0]=({ struct Cyc_List_List
_temp1994; _temp1994.hd=( void*)({ struct _tuple4* _temp1995=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp1995[ 0]=({ struct _tuple4
_temp1996; _temp1996.f1= 0; _temp1996.f2= cur_e; _temp1996;}); _temp1995;});
_temp1994.tl= dles; _temp1994;}); _temp1993;});} dles=({ struct Cyc_List_List*
_temp1999=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1999[ 0]=({ struct Cyc_List_List _temp2000; _temp2000.hd=( void*)({ struct
_tuple4* _temp2001=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2001[ 0]=({ struct _tuple4 _temp2002; _temp2002.f1= 0; _temp2002.f2=
tag_exp; _temp2002;}); _temp2001;}); _temp2000.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); _temp2000;}); _temp1999;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2003=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2003->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({
char* _temp2004=( char*)"_struct"; struct _tagged_string _temp2005; _temp2005.curr=
_temp2004; _temp2005.base= _temp2004; _temp2005.last_plus_one= _temp2004 + 8;
_temp2005;})); _temp2003;}), dles, 0))->r));}} else{ struct Cyc_List_List*
rev_assign=({ struct Cyc_List_List* _temp2013=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2013->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( xexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( qv, 0),
0); _temp2013->tl= 0; _temp2013;});{ int i= 1; for( 0; _temp1507 != 0;(
_temp1507=({ struct Cyc_List_List* _temp2006= _temp1507; if( _temp2006 == 0){
_throw( Null_Exception);} _temp2006->tl;}), i ++), tqts=({ struct Cyc_List_List*
_temp2007= tqts; if( _temp2007 == 0){ _throw( Null_Exception);} _temp2007->tl;})){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp2011= _temp1507; if( _temp2011 == 0){ _throw( Null_Exception);} _temp2011->hd;});
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp2010= tqts; if( _temp2010 == 0){ _throw( Null_Exception);} _temp2010->hd;}))).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp2009=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2009[ 0]= xprintf("f%d", i); _temp2009;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp2008=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2008->hd=( void*) a; _temp2008->tl=
rev_assign; _temp2008;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp2012=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2012->hd=(
void*) final_stmt; _temp2012->tl= rev_assign; _temp2012;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL1312;} _LL1376: goto _LL1312; _LL1378: { void* t_c= Cyc_Toc_typ_to_c(
_temp1519); if( _temp1521 != 0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2016= _temp1521; if( _temp2016 == 0){ _throw(
Null_Exception);} _temp2016;}); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)((
void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1519, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} goto _LL1312;} _LL1380: Cyc_Toc_stmt_to_c( nv, _temp1523); goto
_LL1312; _LL1382:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2017=( char*)"UnresolvedMem"; struct
_tagged_string _temp2018; _temp2018.curr= _temp2017; _temp2018.base= _temp2017;
_temp2018.last_plus_one= _temp2017 + 14; _temp2018;})); goto _LL1312; _LL1384:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp2019=( char*)"codegen"; struct _tagged_string _temp2020; _temp2020.curr=
_temp2019; _temp2020.base= _temp2019; _temp2020.last_plus_one= _temp2019 + 8;
_temp2020;})); goto _LL1312; _LL1386:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2021=( char*)"fill"; struct _tagged_string
_temp2022; _temp2022.curr= _temp2021; _temp2022.base= _temp2021; _temp2022.last_plus_one=
_temp2021 + 5; _temp2022;})); goto _LL1312; _LL1312:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple14{ struct _tuple0* f1; void* f2;
} ; struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, void* t,
struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat* p,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp2024=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp2060; struct Cyc_Absyn_Vardecl _temp2062; struct Cyc_List_List* _temp2063;
struct Cyc_Core_Opt* _temp2065; struct Cyc_Absyn_Exp* _temp2067; void* _temp2069;
struct Cyc_Absyn_Tqual _temp2071; struct _tuple0* _temp2073; void* _temp2075;
struct Cyc_Absyn_Vardecl* _temp2077; int _temp2079; void* _temp2081; char
_temp2083; struct _tagged_string _temp2085; struct Cyc_Absyn_Enumfield*
_temp2087; struct Cyc_Absyn_Enumdecl* _temp2089; struct Cyc_List_List* _temp2091;
struct Cyc_List_List* _temp2093; struct Cyc_Absyn_Tunionfield* _temp2095; struct
Cyc_Absyn_Tuniondecl* _temp2097; struct Cyc_Absyn_Pat* _temp2100; struct Cyc_Absyn_Pat
_temp2102; struct Cyc_Position_Segment* _temp2103; struct Cyc_Core_Opt*
_temp2105; void* _temp2107; struct Cyc_List_List* _temp2109; struct Cyc_List_List*
_temp2111; struct Cyc_Absyn_Tunionfield* _temp2113; struct Cyc_Absyn_Tuniondecl*
_temp2115; struct Cyc_List_List* _temp2117; struct Cyc_List_List* _temp2119;
struct Cyc_Absyn_Tunionfield* _temp2121; struct Cyc_Absyn_Tuniondecl* _temp2123;
struct Cyc_List_List* _temp2125; struct Cyc_List_List* _temp2127; struct Cyc_List_List*
_temp2129; struct Cyc_Core_Opt* _temp2131; struct Cyc_Absyn_Structdecl*
_temp2133; struct Cyc_Absyn_Pat* _temp2135; struct _tuple0* _temp2137; struct
Cyc_List_List* _temp2139; struct Cyc_List_List* _temp2141; struct _tuple0*
_temp2143; struct Cyc_List_List* _temp2145; struct Cyc_List_List* _temp2147;
struct _tuple0* _temp2149; _LL2026: if(( unsigned int) _temp2024 > 2u?*(( int*)
_temp2024) == Cyc_Absyn_Var_p: 0){ _LL2061: _temp2060=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp2024)->f1; _temp2062=* _temp2060; _LL2076:
_temp2075=( void*) _temp2062.sc; goto _LL2074; _LL2074: _temp2073=( struct
_tuple0*) _temp2062.name; goto _LL2072; _LL2072: _temp2071=( struct Cyc_Absyn_Tqual)
_temp2062.tq; goto _LL2070; _LL2070: _temp2069=( void*) _temp2062.type; goto
_LL2068; _LL2068: _temp2067=( struct Cyc_Absyn_Exp*) _temp2062.initializer; goto
_LL2066; _LL2066: _temp2065=( struct Cyc_Core_Opt*) _temp2062.rgn; goto _LL2064;
_LL2064: _temp2063=( struct Cyc_List_List*) _temp2062.attributes; goto _LL2027;}
else{ goto _LL2028;} _LL2028: if( _temp2024 ==( void*) Cyc_Absyn_Wild_p){ goto
_LL2029;} else{ goto _LL2030;} _LL2030: if(( unsigned int) _temp2024 > 2u?*((
int*) _temp2024) == Cyc_Absyn_Reference_p: 0){ _LL2078: _temp2077=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Reference_p_struct*) _temp2024)->f1; goto _LL2031;} else{ goto
_LL2032;} _LL2032: if( _temp2024 ==( void*) Cyc_Absyn_Null_p){ goto _LL2033;}
else{ goto _LL2034;} _LL2034: if(( unsigned int) _temp2024 > 2u?*(( int*)
_temp2024) == Cyc_Absyn_Int_p: 0){ _LL2082: _temp2081=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp2024)->f1; goto _LL2080; _LL2080: _temp2079=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp2024)->f2; goto _LL2035;} else{ goto _LL2036;} _LL2036: if(( unsigned int)
_temp2024 > 2u?*(( int*) _temp2024) == Cyc_Absyn_Char_p: 0){ _LL2084: _temp2083=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp2024)->f1; goto _LL2037;} else{
goto _LL2038;} _LL2038: if(( unsigned int) _temp2024 > 2u?*(( int*) _temp2024)
== Cyc_Absyn_Float_p: 0){ _LL2086: _temp2085=( struct _tagged_string)(( struct
Cyc_Absyn_Float_p_struct*) _temp2024)->f1; goto _LL2039;} else{ goto _LL2040;}
_LL2040: if(( unsigned int) _temp2024 > 2u?*(( int*) _temp2024) == Cyc_Absyn_Enum_p:
0){ _LL2090: _temp2089=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp2024)->f1; goto _LL2088; _LL2088: _temp2087=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp2024)->f2; goto _LL2041;} else{ goto
_LL2042;} _LL2042: if(( unsigned int) _temp2024 > 2u?*(( int*) _temp2024) == Cyc_Absyn_Tunion_p:
0){ _LL2098: _temp2097=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2024)->f1; goto _LL2096; _LL2096: _temp2095=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2024)->f2; goto _LL2094; _LL2094:
_temp2093=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2024)->f3; goto _LL2092; _LL2092: _temp2091=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2024)->f4; if( _temp2091 == 0){ goto
_LL2043;} else{ goto _LL2044;}} else{ goto _LL2044;} _LL2044: if(( unsigned int)
_temp2024 > 2u?*(( int*) _temp2024) == Cyc_Absyn_Pointer_p: 0){ _LL2101:
_temp2100=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2024)->f1; _temp2102=* _temp2100; _LL2108: _temp2107=( void*) _temp2102.r;
if(( unsigned int) _temp2107 > 2u?*(( int*) _temp2107) == Cyc_Absyn_Tunion_p: 0){
_LL2116: _temp2115=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2107)->f1; goto _LL2114; _LL2114: _temp2113=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2107)->f2; goto _LL2112; _LL2112:
_temp2111=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2107)->f3; goto _LL2110; _LL2110: _temp2109=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2107)->f4; goto _LL2106;} else{ goto
_LL2046;} _LL2106: _temp2105=( struct Cyc_Core_Opt*) _temp2102.topt; goto
_LL2104; _LL2104: _temp2103=( struct Cyc_Position_Segment*) _temp2102.loc; goto
_LL2099;} else{ goto _LL2046;} _LL2099: if( _temp2109 != 0){ goto _LL2045;}
else{ goto _LL2046;} _LL2046: if(( unsigned int) _temp2024 > 2u?*(( int*)
_temp2024) == Cyc_Absyn_Tunion_p: 0){ _LL2124: _temp2123=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2024)->f1; goto _LL2122; _LL2122:
_temp2121=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2024)->f2; goto _LL2120; _LL2120: _temp2119=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2024)->f3; goto _LL2118; _LL2118:
_temp2117=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2024)->f4; goto _LL2047;} else{ goto _LL2048;} _LL2048: if(( unsigned int)
_temp2024 > 2u?*(( int*) _temp2024) == Cyc_Absyn_Tuple_p: 0){ _LL2126: _temp2125=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp2024)->f1; goto
_LL2049;} else{ goto _LL2050;} _LL2050: if(( unsigned int) _temp2024 > 2u?*((
int*) _temp2024) == Cyc_Absyn_Struct_p: 0){ _LL2134: _temp2133=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp2024)->f1; goto _LL2132; _LL2132:
_temp2131=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp2024)->f2;
goto _LL2130; _LL2130: _temp2129=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp2024)->f3; goto _LL2128; _LL2128: _temp2127=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp2024)->f4; goto _LL2051;} else{ goto
_LL2052;} _LL2052: if(( unsigned int) _temp2024 > 2u?*(( int*) _temp2024) == Cyc_Absyn_Pointer_p:
0){ _LL2136: _temp2135=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2024)->f1; goto _LL2053;} else{ goto _LL2054;} _LL2054: if(( unsigned int)
_temp2024 > 2u?*(( int*) _temp2024) == Cyc_Absyn_UnknownId_p: 0){ _LL2138:
_temp2137=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2024)->f1;
goto _LL2055;} else{ goto _LL2056;} _LL2056: if(( unsigned int) _temp2024 > 2u?*((
int*) _temp2024) == Cyc_Absyn_UnknownCall_p: 0){ _LL2144: _temp2143=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp2024)->f1; goto _LL2142;
_LL2142: _temp2141=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2024)->f2; goto _LL2140; _LL2140: _temp2139=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp2024)->f3; goto _LL2057;} else{
goto _LL2058;} _LL2058: if(( unsigned int) _temp2024 > 2u?*(( int*) _temp2024)
== Cyc_Absyn_UnknownFields_p: 0){ _LL2150: _temp2149=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp2024)->f1; goto _LL2148; _LL2148:
_temp2147=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2024)->f2; goto _LL2146; _LL2146: _temp2145=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp2024)->f3; goto _LL2059;} else{
goto _LL2025;} _LL2027: nv= Cyc_Toc_add_varmap( nv, _temp2073, r); goto _LL2029;
_LL2029: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL2025; _LL2031: { struct
_tuple0* v= Cyc_Toc_temp_var(); decls=({ struct Cyc_List_List* _temp2151=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2151->hd=(
void*)({ struct _tuple14* _temp2152=( struct _tuple14*) GC_malloc( sizeof(
struct _tuple14)); _temp2152->f1= v; _temp2152->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp2152;}); _temp2151->tl= decls; _temp2151;}); nv= Cyc_Toc_add_varmap(
nv, _temp2077->name, Cyc_Absyn_var_exp( v, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2025;} _LL2033: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2025; _LL2035: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2081, _temp2079, 0), succ_lab, fail_lab); goto _LL2025;
_LL2037: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2083, 0),
succ_lab, fail_lab); goto _LL2025; _LL2039: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2085, 0), succ_lab, fail_lab); goto _LL2025; _LL2041: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2153=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2153[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2154; _temp2154.tag= Cyc_Absyn_Enum_e;
_temp2154.f1= _temp2087->name; _temp2154.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2089; _temp2154.f3=( struct Cyc_Absyn_Enumfield*) _temp2087; _temp2154;});
_temp2153;}), 0), succ_lab, fail_lab); goto _LL2025; _LL2043: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2095->name, 0); if( ! _temp2097->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2025;} _LL2045: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2109);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2113->name,( struct
_tagged_string)({ char* _temp2166=( char*)"_struct"; struct _tagged_string
_temp2167; _temp2167.curr= _temp2166; _temp2167.base= _temp2166; _temp2167.last_plus_one=
_temp2166 + 8; _temp2167;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2109); for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp2155= rps; if(
_temp2155 == 0){ _throw( Null_Exception);} _temp2155->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2162= rps; if(
_temp2162 == 0){ _throw( Null_Exception);} _temp2162->hd;}); struct _tuple0* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp2161= p2->topt;
if( _temp2161 == 0){ _throw( Null_Exception);} _temp2161->v;}); void* t2c= Cyc_Toc_typ_to_c(
t2); struct _tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2156=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2156->hd=( void*)({ struct _tuple14* _temp2157=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2157->f1= v; _temp2157->f2= t2c;
_temp2157;}); _temp2156->tl= decls; _temp2156;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp2160=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2160[ 0]= xprintf("f%d", cnt); _temp2160;}), 0), p2,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp2159=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2159[ 0]= xprintf("f%d", cnt); _temp2159;}), 0), 0),
0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List*
_temp2158=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2158->hd=( void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp2158->tl= ss;
_temp2158;}); succ_lab= lab;}}}}{ struct Cyc_Absyn_Exp* test_exp; if( _temp2115->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2113->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2165= _temp2115->fields; if( _temp2165 == 0){ _throw( Null_Exception);}
_temp2165->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp2163= fs; if(
_temp2163 == 0){ _throw( Null_Exception);} _temp2163->tl;})){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2164= fs; if(
_temp2164 == 0){ _throw( Null_Exception);} _temp2164->hd;}); if( f->typs == 0){
max_tag ++;}}}{ struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t,
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0);
struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2113->name,
0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0):
Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2025;}} _LL2047:
_temp2125= _temp2117; goto _LL2049; _LL2049: { struct Cyc_List_List* ss= 0; int
cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2125);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2125); for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp2168= rps; if(
_temp2168 == 0){ _throw( Null_Exception);} _temp2168->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2175= rps; if(
_temp2175 == 0){ _throw( Null_Exception);} _temp2175->hd;}); struct _tuple0* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp2174= p2->topt;
if( _temp2174 == 0){ _throw( Null_Exception);} _temp2174->v;}); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2169=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2169->hd=( void*)({ struct _tuple14* _temp2170=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2170->f1= v; _temp2170->f2= Cyc_Toc_typ_to_c(
t2); _temp2170;}); _temp2169->tl= decls; _temp2169;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp2173=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2173[ 0]= xprintf("f%d", cnt);
_temp2173;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp2172=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2172[ 0]= xprintf("f%d", cnt); _temp2172;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp2171=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2171->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp2171->tl= ss; _temp2171;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL2025;} _LL2051: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2127); for( 0; dlps != 0; dlps=({ struct Cyc_List_List* _temp2176= dlps;
if( _temp2176 == 0){ _throw( Null_Exception);} _temp2176->tl;})){ struct
_tuple15* tup=( struct _tuple15*)({ struct Cyc_List_List* _temp2190= dlps; if(
_temp2190 == 0){ _throw( Null_Exception);} _temp2190->hd;}); struct Cyc_Absyn_Pat*
p2=(* tup).f2; struct _tagged_string* f;{ void* _temp2178=( void*)({ struct Cyc_List_List*
_temp2177=(* tup).f1; if( _temp2177 == 0){ _throw( Null_Exception);} _temp2177->hd;});
struct _tagged_string* _temp2184; _LL2180: if(*(( int*) _temp2178) == Cyc_Absyn_FieldName){
_LL2185: _temp2184=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp2178)->f1; goto _LL2181;} else{ goto _LL2182;} _LL2182: goto _LL2183;
_LL2181: f= _temp2184; goto _LL2179; _LL2183:( void) _throw(( void*) Cyc_Toc_Match_error);
_LL2179:;}{ struct _tuple0* v= Cyc_Toc_temp_var(); void* t2=( void*)({ struct
Cyc_Core_Opt* _temp2189= p2->topt; if( _temp2189 == 0){ _throw( Null_Exception);}
_temp2189->v;}); void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab=
Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List* _temp2186=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2186->hd=( void*)({ struct
_tuple14* _temp2187=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp2187->f1= v; _temp2187->f2= t2c; _temp2187;}); _temp2186->tl= decls;
_temp2186;});{ struct _tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structmember_exp( path, f, 0), p2, succ_lab, fail_lab, decls);
nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s2= result.f3; struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp( r, f, 0), 0), 0), s2,
0); ss=({ struct Cyc_List_List* _temp2188=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2188->hd=( void*) Cyc_Absyn_label_stmt( lab,
s3, 0); _temp2188->tl= ss; _temp2188;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL2025;} _LL2053: { struct _tuple0* v= Cyc_Toc_temp_var(); void*
t2=( void*)({ struct Cyc_Core_Opt* _temp2193= _temp2135->topt; if( _temp2193 ==
0){ _throw( Null_Exception);} _temp2193->v;}); decls=({ struct Cyc_List_List*
_temp2191=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2191->hd=( void*)({ struct _tuple14* _temp2192=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2192->f1= v; _temp2192->f2= Cyc_Toc_typ_to_c(
t2); _temp2192;}); _temp2191->tl= decls; _temp2191;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path,
0), _temp2135, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0),
s2, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);}
else{ s= s3;} goto _LL2025;}}} _LL2055: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2194=(
char*)"unknownid"; struct _tagged_string _temp2195; _temp2195.curr= _temp2194;
_temp2195.base= _temp2194; _temp2195.last_plus_one= _temp2194 + 10; _temp2195;}));
goto _LL2025; _LL2057: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2196=( char*)"unknowncall";
struct _tagged_string _temp2197; _temp2197.curr= _temp2196; _temp2197.base=
_temp2196; _temp2197.last_plus_one= _temp2196 + 12; _temp2197;})); goto _LL2025;
_LL2059: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2198=( char*)"unknownfields"; struct
_tagged_string _temp2199; _temp2199.curr= _temp2198; _temp2199.base= _temp2198;
_temp2199.last_plus_one= _temp2198 + 14; _temp2199;})); goto _LL2025; _LL2025:;}
return({ struct _tuple13 _temp2200; _temp2200.f1= nv; _temp2200.f2= decls;
_temp2200.f3= s; _temp2200;});} struct _tuple16{ struct _tagged_string* f1;
struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause* f3; } ; static struct
_tuple16* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause* sc){ return({
struct _tuple16* _temp2201=( struct _tuple16*) GC_malloc( sizeof( struct
_tuple16)); _temp2201->f1= Cyc_Toc_fresh_label(); _temp2201->f2= Cyc_Toc_fresh_label();
_temp2201->f3= sc; _temp2201;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* t=( void*)({ struct Cyc_Core_Opt*
_temp2257= e->topt; if( _temp2257 == 0){ _throw( Null_Exception);} _temp2257->v;});
int leave_as_switch;{ void* _temp2202= Cyc_Tcutil_compress( t); void* _temp2208;
void* _temp2210; _LL2204: if(( unsigned int) _temp2202 > 4u?*(( int*) _temp2202)
== Cyc_Absyn_IntType: 0){ _LL2211: _temp2210=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2202)->f1; goto _LL2209; _LL2209: _temp2208=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2202)->f2; goto _LL2205;} else{ goto _LL2206;} _LL2206: goto _LL2207;
_LL2205: leave_as_switch= 1; goto _LL2203; _LL2207: leave_as_switch= 0; goto
_LL2203; _LL2203:;}{ struct Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1=({
struct Cyc_List_List* _temp2212= scs1; if( _temp2212 == 0){ _throw(
Null_Exception);} _temp2212->tl;})){ if(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2214=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp2213=
scs1; if( _temp2213 == 0){ _throw( Null_Exception);} _temp2213->hd;}))->pat_vars;
if( _temp2214 == 0){ _throw( Null_Exception);} _temp2214->v;}) != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* scs1= scs; for( 0; scs1 !=
0; scs1=({ struct Cyc_List_List* _temp2215= scs1; if( _temp2215 == 0){ _throw(
Null_Exception);} _temp2215->tl;})){ struct Cyc_Absyn_Stmt* s=(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2217= scs1; if( _temp2217 == 0){ _throw(
Null_Exception);} _temp2217->hd;}))->body;(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2216= scs1; if( _temp2216 == 0){ _throw(
Null_Exception);} _temp2216->hd;}))->body= Cyc_Absyn_label_stmt( next_l, s, 0);
next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), s);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0);
struct _tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple16*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* lscs2= lscs; for( 0; lscs2 != 0; lscs2=({ struct Cyc_List_List*
_temp2218= lscs2; if( _temp2218 == 0){ _throw( Null_Exception);} _temp2218->tl;})){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2230= lscs2; if( _temp2230 == 0){ _throw( Null_Exception);} _temp2230->hd;}))).f3;
struct _tagged_string* fail_lab=({ struct Cyc_List_List* _temp2227= lscs2; if(
_temp2227 == 0){ _throw( Null_Exception);} _temp2227->tl;}) == 0? end_l:(*((
struct _tuple16*)({ struct Cyc_List_List* _temp2229=({ struct Cyc_List_List*
_temp2228= lscs2; if( _temp2228 == 0){ _throw( Null_Exception);} _temp2228->tl;});
if( _temp2229 == 0){ _throw( Null_Exception);} _temp2229->hd;}))).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2226= lscs2; if( _temp2226 == 0){ _throw( Null_Exception);} _temp2226->hd;}))).f2;
if( sc->where_clause == 0){ struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r,
path, sc->pattern, succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List*
_temp2219=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2219->hd=( void*) res.f1; _temp2219->tl= nvs; _temp2219;}); decls= res.f2;
test_stmts=({ struct Cyc_List_List* _temp2220=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2220->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2221= lscs2; if( _temp2221 == 0){
_throw( Null_Exception);} _temp2221->hd;}))).f1, res.f3, 0); _temp2220->tl=
test_stmts; _temp2220;});} else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2225= sc->where_clause; if( _temp2225 == 0){ _throw(
Null_Exception);} _temp2225;}); struct _tagged_string* intermed_lab= Cyc_Toc_fresh_label();
struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r, path, sc->pattern,
intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c( res.f1, e); nvs=({ struct Cyc_List_List*
_temp2222=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2222->hd=( void*) res.f1; _temp2222->tl= nvs; _temp2222;}); decls= res.f2;{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt( e, Cyc_Absyn_goto_stmt(
succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3=
Cyc_Absyn_label_stmt( intermed_lab, s2, 0); test_stmts=({ struct Cyc_List_List*
_temp2223=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2223->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2224= lscs2; if( _temp2224 == 0){ _throw( Null_Exception);} _temp2224->hd;}))).f1,
Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0); _temp2223->tl= test_stmts; _temp2223;});}}}}{
struct Cyc_Absyn_Stmt* test_stmt= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( test_stmts), 0); nvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt*
clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){ for( 0;({ struct Cyc_List_List*
_temp2231= lscs; if( _temp2231 == 0){ _throw( Null_Exception);} _temp2231->tl;})
!= 0; lscs=({ struct Cyc_List_List* _temp2232= lscs; if( _temp2232 == 0){ _throw(
Null_Exception);} _temp2232->tl;}), nvs=({ struct Cyc_List_List* _temp2233= nvs;
if( _temp2233 == 0){ _throw( Null_Exception);} _temp2233->tl;})){ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2243= nvs; if( _temp2243
== 0){ _throw( Null_Exception);} _temp2243->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2242= lscs; if( _temp2242 == 0){
_throw( Null_Exception);} _temp2242->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
e, end_l,(*(( struct _tuple16*)({ struct Cyc_List_List* _temp2235=({ struct Cyc_List_List*
_temp2234= lscs; if( _temp2234 == 0){ _throw( Null_Exception);} _temp2234->tl;});
if( _temp2235 == 0){ _throw( Null_Exception);} _temp2235->hd;}))).f2,( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2238=((*(( struct _tuple16*)({
struct Cyc_List_List* _temp2237=({ struct Cyc_List_List* _temp2236= lscs; if(
_temp2236 == 0){ _throw( Null_Exception);} _temp2236->tl;}); if( _temp2237 == 0){
_throw( Null_Exception);} _temp2237->hd;}))).f3)->pat_vars; if( _temp2238 == 0){
_throw( Null_Exception);} _temp2238->v;}),( struct Cyc_Toc_Env*)({ struct Cyc_List_List*
_temp2240=({ struct Cyc_List_List* _temp2239= nvs; if( _temp2239 == 0){ _throw(
Null_Exception);} _temp2239->tl;}); if( _temp2240 == 0){ _throw( Null_Exception);}
_temp2240->hd;})), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2241= lscs; if( _temp2241 == 0){
_throw( Null_Exception);} _temp2241->hd;}))).f2, s, 0), 0);}{ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2246= nvs; if( _temp2246
== 0){ _throw( Null_Exception);} _temp2246->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2245= lscs; if( _temp2245 == 0){
_throw( Null_Exception);} _temp2245->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env(
e, end_l), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2244= lscs; if( _temp2244 == 0){
_throw( Null_Exception);} _temp2244->hd;}))).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( test_stmt, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=({
struct Cyc_List_List* _temp2247= decls; if( _temp2247 == 0){ _throw(
Null_Exception);} _temp2247->tl;})){ struct _tuple14 _temp2251; void* _temp2252;
struct _tuple0* _temp2254; struct _tuple14* _temp2249=( struct _tuple14*)({
struct Cyc_List_List* _temp2248= decls; if( _temp2248 == 0){ _throw(
Null_Exception);} _temp2248->hd;}); _temp2251=* _temp2249; _LL2255: _temp2254=
_temp2251.f1; goto _LL2253; _LL2253: _temp2252= _temp2251.f2; goto _LL2250;
_LL2250: res= Cyc_Absyn_declare_stmt( _temp2254, _temp2252, 0, res, 0);}( void*)(
whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( v, Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp2256= e->topt; if( _temp2256 == 0){ _throw(
Null_Exception);} _temp2256->v;})),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2258=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2258[ 0]=({ struct Cyc_List_List _temp2259; _temp2259.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2259.tl= 0; _temp2259;}); _temp2258;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2260=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2300; struct Cyc_Absyn_Stmt* _temp2302; struct Cyc_Absyn_Stmt*
_temp2304; struct Cyc_Absyn_Exp* _temp2306; struct Cyc_Absyn_Stmt* _temp2308;
struct Cyc_Absyn_Stmt* _temp2310; struct Cyc_Absyn_Exp* _temp2312; struct Cyc_Absyn_Stmt*
_temp2314; struct _tuple2 _temp2316; struct Cyc_Absyn_Stmt* _temp2318; struct
Cyc_Absyn_Exp* _temp2320; struct Cyc_Absyn_Stmt* _temp2322; struct Cyc_Absyn_Stmt*
_temp2324; struct Cyc_Absyn_Stmt* _temp2326; struct _tagged_string* _temp2328;
struct Cyc_Absyn_Stmt* _temp2330; struct _tuple2 _temp2332; struct Cyc_Absyn_Stmt*
_temp2334; struct Cyc_Absyn_Exp* _temp2336; struct _tuple2 _temp2338; struct Cyc_Absyn_Stmt*
_temp2340; struct Cyc_Absyn_Exp* _temp2342; struct Cyc_Absyn_Exp* _temp2344;
struct Cyc_List_List* _temp2346; struct Cyc_Absyn_Exp* _temp2348; struct Cyc_Absyn_Switch_clause**
_temp2350; struct Cyc_List_List* _temp2352; struct Cyc_Absyn_Stmt* _temp2354;
struct Cyc_Absyn_Decl* _temp2356; struct Cyc_Absyn_Stmt* _temp2358; struct Cyc_Absyn_Stmt*
_temp2360; struct Cyc_Absyn_Stmt* _temp2362; struct _tagged_string* _temp2364;
struct _tuple2 _temp2366; struct Cyc_Absyn_Stmt* _temp2368; struct Cyc_Absyn_Exp*
_temp2370; struct Cyc_Absyn_Stmt* _temp2372; struct Cyc_List_List* _temp2374;
struct Cyc_Absyn_Stmt* _temp2376; struct Cyc_Absyn_Stmt* _temp2378; struct Cyc_Absyn_Vardecl*
_temp2380; struct Cyc_Absyn_Tvar* _temp2382; _LL2262: if( _temp2260 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2263;} else{ goto _LL2264;} _LL2264: if((
unsigned int) _temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_Exp_s: 0){
_LL2301: _temp2300=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp2260)->f1; goto _LL2265;} else{ goto _LL2266;} _LL2266: if(( unsigned int)
_temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_Seq_s: 0){ _LL2305: _temp2304=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp2260)->f1; goto
_LL2303; _LL2303: _temp2302=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp2260)->f2; goto _LL2267;} else{ goto _LL2268;} _LL2268: if(( unsigned int)
_temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_Return_s: 0){ _LL2307:
_temp2306=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp2260)->f1; goto _LL2269;} else{ goto _LL2270;} _LL2270: if(( unsigned int)
_temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_IfThenElse_s: 0){ _LL2313:
_temp2312=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2260)->f1; goto _LL2311; _LL2311: _temp2310=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2260)->f2; goto _LL2309; _LL2309:
_temp2308=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2260)->f3; goto _LL2271;} else{ goto _LL2272;} _LL2272: if(( unsigned int)
_temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_While_s: 0){ _LL2317: _temp2316=(
struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp2260)->f1; _LL2321:
_temp2320= _temp2316.f1; goto _LL2319; _LL2319: _temp2318= _temp2316.f2; goto
_LL2315; _LL2315: _temp2314=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp2260)->f2; goto _LL2273;} else{ goto _LL2274;} _LL2274: if(( unsigned int)
_temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_Break_s: 0){ _LL2323: _temp2322=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp2260)->f1; goto
_LL2275;} else{ goto _LL2276;} _LL2276: if(( unsigned int) _temp2260 > 1u?*((
int*) _temp2260) == Cyc_Absyn_Continue_s: 0){ _LL2325: _temp2324=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp2260)->f1; goto _LL2277;} else{ goto
_LL2278;} _LL2278: if(( unsigned int) _temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_Goto_s:
0){ _LL2329: _temp2328=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp2260)->f1; goto _LL2327; _LL2327: _temp2326=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp2260)->f2; goto _LL2279;} else{ goto
_LL2280;} _LL2280: if(( unsigned int) _temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_For_s:
0){ _LL2345: _temp2344=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp2260)->f1; goto _LL2339; _LL2339: _temp2338=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp2260)->f2; _LL2343: _temp2342= _temp2338.f1; goto _LL2341; _LL2341:
_temp2340= _temp2338.f2; goto _LL2333; _LL2333: _temp2332=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp2260)->f3; _LL2337: _temp2336= _temp2332.f1;
goto _LL2335; _LL2335: _temp2334= _temp2332.f2; goto _LL2331; _LL2331: _temp2330=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp2260)->f4; goto
_LL2281;} else{ goto _LL2282;} _LL2282: if(( unsigned int) _temp2260 > 1u?*((
int*) _temp2260) == Cyc_Absyn_Switch_s: 0){ _LL2349: _temp2348=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp2260)->f1; goto _LL2347; _LL2347:
_temp2346=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp2260)->f2; goto _LL2283;} else{ goto _LL2284;} _LL2284: if(( unsigned int)
_temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_Fallthru_s: 0){ _LL2353:
_temp2352=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2260)->f1; goto _LL2351; _LL2351: _temp2350=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp2260)->f2; goto _LL2285;} else{ goto
_LL2286;} _LL2286: if(( unsigned int) _temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_Decl_s:
0){ _LL2357: _temp2356=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp2260)->f1; goto _LL2355; _LL2355: _temp2354=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp2260)->f2; goto _LL2287;} else{ goto
_LL2288;} _LL2288: if(( unsigned int) _temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_Cut_s:
0){ _LL2359: _temp2358=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp2260)->f1; goto _LL2289;} else{ goto _LL2290;} _LL2290: if(( unsigned int)
_temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_Splice_s: 0){ _LL2361:
_temp2360=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp2260)->f1; goto _LL2291;} else{ goto _LL2292;} _LL2292: if(( unsigned int)
_temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_Label_s: 0){ _LL2365: _temp2364=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp2260)->f1; goto
_LL2363; _LL2363: _temp2362=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp2260)->f2; goto _LL2293;} else{ goto _LL2294;} _LL2294: if(( unsigned int)
_temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_Do_s: 0){ _LL2373: _temp2372=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp2260)->f1; goto
_LL2367; _LL2367: _temp2366=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp2260)->f2; _LL2371: _temp2370= _temp2366.f1; goto _LL2369; _LL2369:
_temp2368= _temp2366.f2; goto _LL2295;} else{ goto _LL2296;} _LL2296: if((
unsigned int) _temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_TryCatch_s: 0){
_LL2377: _temp2376=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2260)->f1; goto _LL2375; _LL2375: _temp2374=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp2260)->f2; goto _LL2297;} else{ goto
_LL2298;} _LL2298: if(( unsigned int) _temp2260 > 1u?*(( int*) _temp2260) == Cyc_Absyn_Region_s:
0){ _LL2383: _temp2382=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2260)->f1; goto _LL2381; _LL2381: _temp2380=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp2260)->f2; goto _LL2379; _LL2379:
_temp2378=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2260)->f3; goto _LL2299;} else{ goto _LL2261;} _LL2263: return; _LL2265:
Cyc_Toc_exp_to_c( nv, _temp2300); return; _LL2267: Cyc_Toc_stmt_to_c( nv,
_temp2304); s= _temp2302; continue; _LL2269: { struct Cyc_Core_Opt* topt= 0; if(
_temp2306 != 0){ topt=({ struct Cyc_Core_Opt* _temp2384=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2384->v=( void*) Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp2386=({ struct Cyc_Absyn_Exp* _temp2385=
_temp2306; if( _temp2385 == 0){ _throw( Null_Exception);} _temp2385->topt;});
if( _temp2386 == 0){ _throw( Null_Exception);} _temp2386->v;})); _temp2384;});
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2387=
_temp2306; if( _temp2387 == 0){ _throw( Null_Exception);} _temp2387;}));} if( s->try_depth
> 0){ if( topt != 0){ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
retn_stmt= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x,
0), 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( x,( void*)({
struct Cyc_Core_Opt* _temp2388= topt; if( _temp2388 == 0){ _throw(
Null_Exception);} _temp2388->v;}), _temp2306, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), retn_stmt, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL2271: Cyc_Toc_exp_to_c( nv, _temp2312); Cyc_Toc_stmt_to_c( nv,
_temp2310); s= _temp2308; continue; _LL2273: Cyc_Toc_exp_to_c( nv, _temp2320);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2314); return; _LL2275: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)({ struct Cyc_Core_Opt* _temp2389= nv->break_lab; if( _temp2389
== 0){ _throw( Null_Exception);} _temp2389->v;}), 0))->r));}{ int dest_depth=
_temp2322 == 0? 0:({ struct Cyc_Absyn_Stmt* _temp2390= _temp2322; if( _temp2390
== 0){ _throw( Null_Exception);} _temp2390->try_depth;}); Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2277: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)({
struct Cyc_Core_Opt* _temp2391= nv->continue_lab; if( _temp2391 == 0){ _throw(
Null_Exception);} _temp2391->v;}), 0))->r));} Cyc_Toc_do_npop_before( s->try_depth
-({ struct Cyc_Absyn_Stmt* _temp2392= _temp2324; if( _temp2392 == 0){ _throw(
Null_Exception);} _temp2392->try_depth;}), s); return; _LL2279: Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2393= _temp2326; if( _temp2393 == 0){
_throw( Null_Exception);} _temp2393->try_depth;}), s); return; _LL2281: Cyc_Toc_exp_to_c(
nv, _temp2344); Cyc_Toc_exp_to_c( nv, _temp2342); Cyc_Toc_exp_to_c( nv,
_temp2336); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2330); return;
_LL2283: Cyc_Toc_xlate_switch( nv, s, _temp2348, _temp2346); return; _LL2285:
if( nv->fallthru_info == 0){( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2394=( char*)"fallthru in unexpected place";
struct _tagged_string _temp2395; _temp2395.curr= _temp2394; _temp2395.base=
_temp2394; _temp2395.last_plus_one= _temp2394 + 29; _temp2395;})));}{ struct
_tuple8 _temp2399; struct Cyc_Dict_Dict* _temp2400; struct Cyc_List_List*
_temp2402; struct _tagged_string* _temp2404; struct _tuple8* _temp2397=( struct
_tuple8*)({ struct Cyc_Core_Opt* _temp2396= nv->fallthru_info; if( _temp2396 ==
0){ _throw( Null_Exception);} _temp2396->v;}); _temp2399=* _temp2397; _LL2405:
_temp2404= _temp2399.f1; goto _LL2403; _LL2403: _temp2402= _temp2399.f2; goto
_LL2401; _LL2401: _temp2400= _temp2399.f3; goto _LL2398; _LL2398: { struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_goto_stmt( _temp2404, 0); Cyc_Toc_do_npop_before( s->try_depth -(({
struct Cyc_Absyn_Switch_clause** _temp2406= _temp2350; if( _temp2406 == 0){
_throw( Null_Exception);}* _temp2406;})->body)->try_depth, s2);{ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2402); struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2352); for( 0; vs2 != 0; vs2=({ struct Cyc_List_List*
_temp2407= vs2; if( _temp2407 == 0){ _throw( Null_Exception);} _temp2407->tl;}),
es2=({ struct Cyc_List_List* _temp2408= es2; if( _temp2408 == 0){ _throw(
Null_Exception);} _temp2408->tl;})){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp2409= es2; if( _temp2409 == 0){ _throw(
Null_Exception);} _temp2409->hd;})); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2400,( struct _tuple0*)({ struct Cyc_List_List* _temp2410= vs2; if(
_temp2410 == 0){ _throw( Null_Exception);} _temp2410->hd;})),( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp2411= es2; if( _temp2411 == 0){ _throw(
Null_Exception);} _temp2411->hd;}), 0), s2, 0);}( void*)( s->r=( void*)(( void*)
s2->r)); return;}}} _LL2287:{ void* _temp2412=( void*) _temp2356->r; int
_temp2420; struct Cyc_Absyn_Exp* _temp2422; struct Cyc_Core_Opt* _temp2424;
struct Cyc_Core_Opt* _temp2426; struct Cyc_Absyn_Pat* _temp2428; struct Cyc_Absyn_Vardecl*
_temp2430; _LL2414: if(*(( int*) _temp2412) == Cyc_Absyn_Let_d){ _LL2429:
_temp2428=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2412)->f1;
goto _LL2427; _LL2427: _temp2426=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2412)->f2; goto _LL2425; _LL2425: _temp2424=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2412)->f3; goto _LL2423; _LL2423: _temp2422=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2412)->f4; goto
_LL2421; _LL2421: _temp2420=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2412)->f5;
goto _LL2415;} else{ goto _LL2416;} _LL2416: if(*(( int*) _temp2412) == Cyc_Absyn_Var_d){
_LL2431: _temp2430=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2412)->f1; goto _LL2417;} else{ goto _LL2418;} _LL2418: goto _LL2419;
_LL2415:( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2428,
_temp2424, _temp2422, _temp2420, _temp2354))->r)); goto _LL2413; _LL2417: {
struct _tuple0* c_name= _temp2430->name;( void*)( _temp2430->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2430->type));{ struct Cyc_Toc_Env* nv2= Cyc_Toc_add_varmap( nv,
c_name, Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2445=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2445[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2446; _temp2446.tag= Cyc_Absyn_Local_b;
_temp2446.f1= _temp2430; _temp2446;}); _temp2445;}), 0)); Cyc_Toc_stmt_to_c( nv2,
_temp2354); if( _temp2430->initializer != 0){ struct Cyc_Absyn_Exp* init=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2444= _temp2430->initializer;
if( _temp2444 == 0){ _throw( Null_Exception);} _temp2444;}); void* _temp2432=(
void*) init->r; struct Cyc_Absyn_Exp* _temp2438; struct Cyc_Absyn_Exp* _temp2440;
struct Cyc_Absyn_Vardecl* _temp2442; _LL2434: if(*(( int*) _temp2432) == Cyc_Absyn_Comprehension_e){
_LL2443: _temp2442=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2432)->f1; goto _LL2441; _LL2441: _temp2440=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2432)->f2; goto _LL2439; _LL2439:
_temp2438=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2432)->f3; goto _LL2435;} else{ goto _LL2436;} _LL2436: goto _LL2437;
_LL2435: _temp2430->initializer= 0;( void*)( _temp2354->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( c_name, 0), _temp2442, _temp2440, _temp2438, Cyc_Absyn_new_stmt((
void*) _temp2354->r, 0), 0))->r)); goto _LL2433; _LL2437: Cyc_Toc_exp_to_c( nv,
init); goto _LL2433; _LL2433:;} goto _LL2413;}} _LL2419:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2447=(
char*)"bad nested declaration within function"; struct _tagged_string _temp2448;
_temp2448.curr= _temp2447; _temp2448.base= _temp2447; _temp2448.last_plus_one=
_temp2447 + 39; _temp2448;})); goto _LL2413; _LL2413:;} return; _LL2289:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp2449=( char*)"cut"; struct _tagged_string _temp2450; _temp2450.curr=
_temp2449; _temp2450.base= _temp2449; _temp2450.last_plus_one= _temp2449 + 4;
_temp2450;})); return; _LL2291:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2451=( char*)"splice"; struct _tagged_string
_temp2452; _temp2452.curr= _temp2451; _temp2452.base= _temp2451; _temp2452.last_plus_one=
_temp2451 + 7; _temp2452;})); return; _LL2293: s= _temp2362; continue; _LL2295:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2372); Cyc_Toc_exp_to_c( nv,
_temp2370); return; _LL2297: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct
_tuple0* e_var= Cyc_Toc_temp_var(); struct _tuple0* was_thrown_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2469=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2469->f1= Cyc_Toc_abs_ns; _temp2469->f2= Cyc_Toc__push_handler_sp;
_temp2469;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple0* _temp2468=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2468->f1= Cyc_Toc_abs_ns; _temp2468->f2= Cyc_Toc_setjmp_sp; _temp2468;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2467=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2467->f1=
Cyc_Toc_abs_ns; _temp2467->f2= Cyc_Toc__pop_handler_sp; _temp2467;}), 0); struct
Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp(({ struct _tuple0* _temp2466=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2466->f1= Cyc_Toc_abs_ns;
_temp2466->f2= Cyc_Toc__exn_thrown_sp; _temp2466;}), 0); e_exp->topt=({ struct
Cyc_Core_Opt* _temp2453=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2453->v=( void*) e_typ; _temp2453;}); nv= Cyc_Toc_add_varmap( nv, e_var,
e_exp); Cyc_Toc_stmt_to_c( nv, _temp2376);{ struct Cyc_Absyn_Stmt*
tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp2376, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0); struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl* x_vd= Cyc_Absyn_new_vardecl(
x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt* _temp2454=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2454->v=( void*)
Cyc_Absyn_exn_typ; _temp2454;});{ struct Cyc_Absyn_Pat* p=({ struct Cyc_Absyn_Pat*
_temp2462=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof( struct Cyc_Absyn_Pat));
_temp2462->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp2464=(
struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2464[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2465; _temp2465.tag= Cyc_Absyn_Var_p;
_temp2465.f1= x_vd; _temp2465;}); _temp2464;})); _temp2462->topt=({ struct Cyc_Core_Opt*
_temp2463=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2463->v=( void*) Cyc_Absyn_exn_typ; _temp2463;}); _temp2462->loc= 0;
_temp2462;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp2455=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2455->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2455;});{
struct Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp2459=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2459->pattern= p;
_temp2459->pat_vars=({ struct Cyc_Core_Opt* _temp2460=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2460->v=( void*)({ struct Cyc_List_List*
_temp2461=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2461->hd=( void*) x_vd; _temp2461->tl= 0; _temp2461;}); _temp2460;});
_temp2459->where_clause= 0; _temp2459->body= rs; _temp2459->loc= 0; _temp2459;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2374,({
struct Cyc_List_List* _temp2458=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2458->hd=( void*) default_case; _temp2458->tl= 0;
_temp2458;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List* _temp2457=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2457->hd=(
void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0); _temp2457->tl=
0; _temp2457;}), 0); struct Cyc_Absyn_Stmt* pushhandler_call= Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List* _temp2456=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2456->hd=(
void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2456->tl= 0; _temp2456;}), 0), 0);
struct Cyc_Absyn_Exp* zero_exp= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0,
0); struct Cyc_Absyn_Exp* one_exp= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed,
1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0,
Cyc_Absyn_seq_stmt( pushhandler_call, Cyc_Absyn_declare_stmt( was_thrown_var,
was_thrown_typ,( struct Cyc_Absyn_Exp*) zero_exp, Cyc_Absyn_seq_stmt( Cyc_Absyn_ifthenelse_stmt(
setjmp_call, Cyc_Absyn_assign_stmt( was_thrown_exp, one_exp, 0), Cyc_Absyn_skip_stmt(
0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Not,
was_thrown_exp, 0), tryandpop_stmt, Cyc_Absyn_declare_stmt( e_var, e_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp, 0),
handler_stmt, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL2299: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__region_handle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2380->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2475=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2475[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2476; _temp2476.tag= Cyc_Absyn_Local_b; _temp2476.f1=
_temp2380; _temp2476;}); _temp2475;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2474=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2474->f1= Cyc_Toc_abs_ns; _temp2474->f2= Cyc_Toc__push_region_sp;
_temp2474;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2473=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2473->f1= Cyc_Toc_abs_ns; _temp2473->f2= Cyc_Toc__pop_region_sp; _temp2473;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2472=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2472->f1=
Cyc_Toc_abs_ns; _temp2472->f2= Cyc_Toc__new_region_sp; _temp2472;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2471=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2471->hd=( void*) addr_rh_exp; _temp2471->tl= 0; _temp2471;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2470=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2470->hd=( void*) x_exp; _temp2470->tl= 0; _temp2470;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2378);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2378, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2261:;}} struct _tuple17{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0;(
void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{ struct
Cyc_List_List* args= f->args; for( 0; args != 0; args=({ struct Cyc_List_List*
_temp2477= args; if( _temp2477 == 0){ _throw( Null_Exception);} _temp2477->tl;})){(*((
struct _tuple17*)({ struct Cyc_List_List* _temp2478= args; if( _temp2478 == 0){
_throw( Null_Exception);} _temp2478->hd;}))).f3= Cyc_Toc_typ_to_c((*(( struct
_tuple17*)({ struct Cyc_List_List* _temp2479= args; if( _temp2479 == 0){ _throw(
Null_Exception);} _temp2479->hd;}))).f3);{ struct _tuple0* x=({ struct _tuple0*
_temp2480=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2480->f1=(
void*) Cyc_Absyn_Loc_n; _temp2480->f2=(*(( struct _tuple17*)({ struct Cyc_List_List*
_temp2481= args; if( _temp2481 == 0){ _throw( Null_Exception);} _temp2481->hd;}))).f1;
_temp2480;}); nv= Cyc_Toc_add_varmap( nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel( nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){
void* _temp2482= s; _LL2484: if( _temp2482 ==( void*) Cyc_Absyn_Abstract){ goto
_LL2485;} else{ goto _LL2486;} _LL2486: if( _temp2482 ==( void*) Cyc_Absyn_ExternC){
goto _LL2487;} else{ goto _LL2488;} _LL2488: goto _LL2489; _LL2485: return( void*)
Cyc_Absyn_Public; _LL2487: return( void*) Cyc_Absyn_Extern; _LL2489: return s;
_LL2483:;} struct Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* n=( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2502= s->name; if( _temp2502 == 0){ _throw( Null_Exception);}
_temp2502->v;}); if( Cyc_Toc_structs_so_far == 0){ Cyc_Toc_structs_so_far=({
struct Cyc_Core_Opt* _temp2490=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2490->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2490;});}{ int seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2501= Cyc_Toc_structs_so_far;
if( _temp2501 == 0){ _throw( Null_Exception);} _temp2501->v;}),(* n).f2); if(
dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2491= Cyc_Toc_structs_so_far; if( _temp2491 == 0){ _throw( Null_Exception);}
_temp2491->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2492= Cyc_Toc_structs_so_far;
if( _temp2492 == 0){ _throw( Null_Exception);} _temp2492->v;}),(* n).f2, s);});}
else{ if((( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Core_Opt* _temp2493= dopt;
if( _temp2493 == 0){ _throw( Null_Exception);} _temp2493->v;}))->fields == 0){(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2494= Cyc_Toc_structs_so_far;
if( _temp2494 == 0){ _throw( Null_Exception);} _temp2494->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2495= Cyc_Toc_structs_so_far; if( _temp2495 == 0){ _throw(
Null_Exception);} _temp2495->v;}),(* n).f2, s);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) s->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) s->sc))); s->tvs= 0; s->name=({
struct Cyc_Core_Opt* _temp2496=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2496->v=( void*) n; _temp2496;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2500= s->fields; if( _temp2500 == 0){
_throw( Null_Exception);} _temp2500->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2497= fields; if( _temp2497 == 0){ _throw( Null_Exception);}
_temp2497->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2498= fields; if( _temp2498 == 0){ _throw( Null_Exception);} _temp2498->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2499= fields; if( _temp2499 == 0){ _throw( Null_Exception);}
_temp2499->hd;}))->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* n=( struct _tuple0*)({ struct Cyc_Core_Opt* _temp2515= u->name; if(
_temp2515 == 0){ _throw( Null_Exception);} _temp2515->v;}); if( Cyc_Toc_unions_so_far
== 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2503=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2503->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2503;});}{ int seen_defn_before; struct Cyc_Core_Opt*
dopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2514= Cyc_Toc_unions_so_far; if( _temp2514 == 0){ _throw( Null_Exception);}
_temp2514->v;}),(* n).f2); if( dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2504= Cyc_Toc_unions_so_far; if( _temp2504 == 0){
_throw( Null_Exception);} _temp2504->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2505= Cyc_Toc_unions_so_far; if( _temp2505 == 0){ _throw( Null_Exception);}
_temp2505->v;}),(* n).f2, u);});} else{ if((( struct Cyc_Absyn_Uniondecl*)({
struct Cyc_Core_Opt* _temp2506= dopt; if( _temp2506 == 0){ _throw(
Null_Exception);} _temp2506->v;}))->fields == 0){( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2507= Cyc_Toc_unions_so_far; if( _temp2507 == 0){
_throw( Null_Exception);} _temp2507->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2508= Cyc_Toc_unions_so_far; if( _temp2508 == 0){ _throw( Null_Exception);}
_temp2508->v;}),(* n).f2, u);}); seen_defn_before= 0;} else{ seen_defn_before= 1;}}(
void*)( u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*)
u->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2509=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2509->v=( void*) n; _temp2509;}); if( u->fields != 0){ if(
seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2513= u->fields; if( _temp2513 == 0){
_throw( Null_Exception);} _temp2513->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2510= fields; if( _temp2510 == 0){ _throw( Null_Exception);}
_temp2510->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2511= fields; if( _temp2511 == 0){ _throw( Null_Exception);} _temp2511->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2512= fields; if( _temp2512 == 0){ _throw( Null_Exception);}
_temp2512->hd;}))->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2516=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2516->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2516;});}{ struct
_tuple0* n= tud->name; if( tud->fields == 0){ return;} if((( int(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2517= Cyc_Toc_tunions_so_far; if( _temp2517 == 0){ _throw( Null_Exception);}
_temp2517->v;}), n)){ return;}( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2518= Cyc_Toc_tunions_so_far; if( _temp2518 == 0){ _throw( Null_Exception);}
_temp2518->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct
_tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2519= Cyc_Toc_tunions_so_far; if( _temp2519 == 0){ _throw( Null_Exception);}
_temp2519->v;}), n);});{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2546= tud->fields;
if( _temp2546 == 0){ _throw( Null_Exception);} _temp2546->v;}); for( 0; fields
!= 0; fields=({ struct Cyc_List_List* _temp2520= fields; if( _temp2520 == 0){
_throw( Null_Exception);} _temp2520->tl;})){ struct Cyc_Absyn_Tunionfield* f=(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2545= fields; if(
_temp2545 == 0){ _throw( Null_Exception);} _temp2545->hd;}); if( f->typs == 0){
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
tag_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2521; _temp2521.q_const=
1; _temp2521.q_volatile= 0; _temp2521.q_restrict= 0; _temp2521;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2522=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2522->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2523=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2523[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2524; _temp2524.tag= Cyc_Absyn_Var_d; _temp2524.f1= vd; _temp2524;});
_temp2523;}), 0); _temp2522->tl= Cyc_Toc_result_decls; _temp2522;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
box_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2525; _temp2525.q_const=
1; _temp2525.q_volatile= 0; _temp2525.q_restrict= 0; _temp2525;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2526=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2526->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2527=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2527[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2528; _temp2528.tag= Cyc_Absyn_Var_d; _temp2528.f1= vd; _temp2528;});
_temp2527;}), 0); _temp2526->tl= Cyc_Toc_result_decls; _temp2526;});{ struct Cyc_List_List*
fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts=({
struct Cyc_List_List* _temp2529= ts; if( _temp2529 == 0){ _throw( Null_Exception);}
_temp2529->tl;}), i ++){ struct _tagged_string* fname=( struct _tagged_string*)({
struct _tagged_string* _temp2534=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp2534[ 0]= xprintf("f%d", i); _temp2534;}); struct
Cyc_Absyn_Structfield* f=({ struct Cyc_Absyn_Structfield* _temp2531=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2531->name= fname;
_temp2531->tq=(*(( struct _tuple3*)({ struct Cyc_List_List* _temp2533= ts; if(
_temp2533 == 0){ _throw( Null_Exception);} _temp2533->hd;}))).f1; _temp2531->type=(
void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp2532= ts; if( _temp2532 == 0){ _throw( Null_Exception);} _temp2532->hd;}))).f2);
_temp2531->width= 0; _temp2531->attributes= 0; _temp2531;}); fs=({ struct Cyc_List_List*
_temp2530=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2530->hd=( void*) f; _temp2530->tl= fs; _temp2530;});}} fs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs); fs=({ struct Cyc_List_List*
_temp2535=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2535->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2536=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2536->name= Cyc_Toc_tag_sp;
_temp2536->tq= Cyc_Toc_mt_tq; _temp2536->type=( void*) Cyc_Absyn_sint_t;
_temp2536->width= 0; _temp2536->attributes= 0; _temp2536;}); _temp2535->tl= fs;
_temp2535;});{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp2540=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2540->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2540->name=({ struct Cyc_Core_Opt*
_temp2542=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2542->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2543=( char*)"_struct"; struct _tagged_string _temp2544; _temp2544.curr=
_temp2543; _temp2544.base= _temp2543; _temp2544.last_plus_one= _temp2543 + 8;
_temp2544;})); _temp2542;}); _temp2540->tvs= 0; _temp2540->fields=({ struct Cyc_Core_Opt*
_temp2541=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2541->v=( void*) fs; _temp2541;}); _temp2540->attributes= 0; _temp2540;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2537=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2537->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2538=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2538[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2539; _temp2539.tag= Cyc_Absyn_Struct_d;
_temp2539.f1= sd; _temp2539;}); _temp2538;}), 0); _temp2537->tl= Cyc_Toc_result_decls;
_temp2537;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2547=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2547->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2547;});} if( xd->fields
== 0){ return;}{ struct _tuple0* n= xd->name; struct Cyc_List_List* fs=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2601= xd->fields; if( _temp2601 == 0){
_throw( Null_Exception);} _temp2601->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp2548= fs; if( _temp2548 == 0){ _throw( Null_Exception);} _temp2548->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp2600= fs; if( _temp2600 == 0){ _throw( Null_Exception);} _temp2600->hd;});
struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_uint_exp(({
struct _tagged_string _temp2599=* fn;( unsigned int)( _temp2599.last_plus_one -
_temp2599.curr);}) + 4, 0); struct Cyc_Absyn_ArrayType_struct* tag_typ=({ struct
Cyc_Absyn_ArrayType_struct* _temp2597=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2597[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2598; _temp2598.tag= Cyc_Absyn_ArrayType;
_temp2598.f1=( void*) Cyc_Absyn_uchar_t; _temp2598.f2= Cyc_Toc_mt_tq; _temp2598.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp2598;}); _temp2597;}); struct Cyc_Core_Opt*
_temp2550=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2549= Cyc_Toc_xtunion_fields_so_far; if( _temp2549 == 0){ _throw(
Null_Exception);} _temp2549->v;}), f->name); struct Cyc_Core_Opt _temp2558; int
_temp2559; _LL2552: if( _temp2550 == 0){ goto _LL2553;} else{ goto _LL2554;}
_LL2554: if( _temp2550 == 0){ goto _LL2556;} else{ _temp2558=* _temp2550;
_LL2560: _temp2559=( int) _temp2558.v; if( _temp2559 == 0){ goto _LL2555;} else{
goto _LL2556;}} _LL2556: goto _LL2557; _LL2553: { struct Cyc_Absyn_Exp* initopt=
0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp(({ int _temp2561= 0; int _temp2562= 0; int _temp2563= 0;
int _temp2564= 0; struct _tagged_string _temp2565=* fn; xprintf("%c%c%c%c%.*s",
_temp2561, _temp2562, _temp2563, _temp2564, _temp2565.last_plus_one - _temp2565.curr,
_temp2565.curr);}), 0);}{ struct Cyc_Absyn_Vardecl* tag_decl= Cyc_Absyn_new_vardecl(
f->name,( void*) tag_typ, initopt);( void*)( tag_decl->sc=( void*)(( void*) f->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2566=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2566->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2567=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2567[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2568; _temp2568.tag= Cyc_Absyn_Var_d; _temp2568.f1= tag_decl; _temp2568;});
_temp2567;}), 0); _temp2566->tl= Cyc_Toc_result_decls; _temp2566;});( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2569= Cyc_Toc_xtunion_fields_so_far; if( _temp2569 ==
0){ _throw( Null_Exception);} _temp2569->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2570= Cyc_Toc_xtunion_fields_so_far;
if( _temp2570 == 0){ _throw( Null_Exception);} _temp2570->v;}), f->name,( void*)
f->sc !=( void*) Cyc_Absyn_Extern);}); if( f->typs != 0){ struct Cyc_List_List*
fields= 0; int i= 1;{ struct Cyc_List_List* tqts= f->typs; for( 0; tqts != 0;
tqts=({ struct Cyc_List_List* _temp2571= tqts; if( _temp2571 == 0){ _throw(
Null_Exception);} _temp2571->tl;}), i ++){ struct _tagged_string* field_n=(
struct _tagged_string*)({ struct _tagged_string* _temp2576=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2576[ 0]=
xprintf("f%d", i); _temp2576;}); struct Cyc_Absyn_Structfield* newf=({ struct
Cyc_Absyn_Structfield* _temp2573=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp2573->name= field_n; _temp2573->tq=(*((
struct _tuple3*)({ struct Cyc_List_List* _temp2575= tqts; if( _temp2575 == 0){
_throw( Null_Exception);} _temp2575->hd;}))).f1; _temp2573->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)({ struct Cyc_List_List* _temp2574= tqts; if( _temp2574 == 0){
_throw( Null_Exception);} _temp2574->hd;}))).f2); _temp2573->width= 0; _temp2573->attributes=
0; _temp2573;}); fields=({ struct Cyc_List_List* _temp2572=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2572->hd=( void*) newf;
_temp2572->tl= fields; _temp2572;});}} fields=({ struct Cyc_List_List* _temp2577=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2577->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp2578=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2578->name= Cyc_Toc_tag_sp;
_temp2578->tq= Cyc_Toc_mt_tq; _temp2578->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2578->width= 0; _temp2578->attributes= 0; _temp2578;});
_temp2577->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2577;});{ struct Cyc_Absyn_Structdecl* strct_decl=({ struct Cyc_Absyn_Structdecl*
_temp2582=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2582->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2582->name=({ struct Cyc_Core_Opt*
_temp2584=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2584->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2585=( char*)"_struct"; struct _tagged_string _temp2586; _temp2586.curr=
_temp2585; _temp2586.base= _temp2585; _temp2586.last_plus_one= _temp2585 + 8;
_temp2586;})); _temp2584;}); _temp2582->tvs= 0; _temp2582->fields=({ struct Cyc_Core_Opt*
_temp2583=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2583->v=( void*) fields; _temp2583;}); _temp2582->attributes= 0; _temp2582;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2579=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2579->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2580=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2580[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2581; _temp2581.tag= Cyc_Absyn_Struct_d;
_temp2581.f1= strct_decl; _temp2581;}); _temp2580;}), 0); _temp2579->tl= Cyc_Toc_result_decls;
_temp2579;});}} goto _LL2551;}} _LL2555: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* initopt= Cyc_Absyn_string_exp(({ int _temp2592= 0; int
_temp2593= 0; int _temp2594= 0; int _temp2595= 0; struct _tagged_string
_temp2596=* fn; xprintf("%c%c%c%c%.*s", _temp2592, _temp2593, _temp2594,
_temp2595, _temp2596.last_plus_one - _temp2596.curr, _temp2596.curr);}), 0);
struct Cyc_Absyn_Vardecl* tag_decl= Cyc_Absyn_new_vardecl( f->name,( void*)
tag_typ,( struct Cyc_Absyn_Exp*) initopt);( void*)( tag_decl->sc=( void*)(( void*)
f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2587=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2587->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2588=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2588[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2589; _temp2589.tag= Cyc_Absyn_Var_d; _temp2589.f1= tag_decl; _temp2589;});
_temp2588;}), 0); _temp2587->tl= Cyc_Toc_result_decls; _temp2587;});( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2590= Cyc_Toc_xtunion_fields_so_far; if( _temp2590 ==
0){ _throw( Null_Exception);} _temp2590->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2591= Cyc_Toc_xtunion_fields_so_far;
if( _temp2591 == 0){ _throw( Null_Exception);} _temp2591->v;}), f->name, 1);});}
goto _LL2551; _LL2557: goto _LL2551; _LL2551:;}}} static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* n= ed->name;(
void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern? 1:( void*)
ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc)));{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2607= ed->fields; if(
_temp2607 == 0){ _throw( Null_Exception);} _temp2607->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp2602= fs; if( _temp2602 == 0){ _throw( Null_Exception);}
_temp2602->tl;})){ if((( struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List*
_temp2603= fs; if( _temp2603 == 0){ _throw( Null_Exception);} _temp2603->hd;}))->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2606=(( struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List*
_temp2605= fs; if( _temp2605 == 0){ _throw( Null_Exception);} _temp2605->hd;}))->tag;
if( _temp2606 == 0){ _throw( Null_Exception);} _temp2606;}));(( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2604= fs; if( _temp2604 == 0){ _throw( Null_Exception);}
_temp2604->hd;}))->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( i, 0);}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)({ struct Cyc_Core_Opt*
_temp2626= topt; if( _temp2626 == 0){ _throw( Null_Exception);} _temp2626->v;});
Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var(); struct
_tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string* fail_lab=
exhaust? succ_lab: Cyc_Toc_fresh_label(); struct Cyc_Absyn_Stmt* _temp2610;
struct Cyc_List_List* _temp2612; struct Cyc_Toc_Env* _temp2614; struct _tuple13
_temp2608= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp(
x, 0), p, succ_lab, fail_lab, 0); _LL2615: _temp2614= _temp2608.f1; goto _LL2613;
_LL2613: _temp2612= _temp2608.f2; goto _LL2611; _LL2611: _temp2610= _temp2608.f3;
goto _LL2609; _LL2609: Cyc_Toc_stmt_to_c( _temp2614, s);{ struct Cyc_Absyn_Stmt*
succ_stmt= Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2610, succ_stmt, 0), 0);} else{ struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2616=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2616->f1= Cyc_Toc_abs_ns; _temp2616->f2= Cyc_Toc_Match_Exception_sp;
_temp2616;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2610, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp2612 != 0; _temp2612=({ struct
Cyc_List_List* _temp2617= _temp2612; if( _temp2617 == 0){ _throw( Null_Exception);}
_temp2617->tl;})){ struct _tuple14 _temp2621; void* _temp2622; struct _tuple0*
_temp2624; struct _tuple14* _temp2619=( struct _tuple14*)({ struct Cyc_List_List*
_temp2618= _temp2612; if( _temp2618 == 0){ _throw( Null_Exception);} _temp2618->hd;});
_temp2621=* _temp2619; _LL2625: _temp2624= _temp2621.f1; goto _LL2623; _LL2623:
_temp2622= _temp2621.f2; goto _LL2620; _LL2620: s= Cyc_Absyn_declare_stmt(
_temp2624, _temp2622, 0, s, 0);} return s;}}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=({
struct Cyc_List_List* _temp2627= ds; if( _temp2627 == 0){ _throw( Null_Exception);}
_temp2627->tl;})){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct
Cyc_List_List* _temp2701= ds; if( _temp2701 == 0){ _throw( Null_Exception);}
_temp2701->hd;}); void* _temp2628=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2652; struct Cyc_Absyn_Fndecl* _temp2654; int _temp2656; struct Cyc_Absyn_Exp*
_temp2658; struct Cyc_Core_Opt* _temp2660; struct Cyc_Core_Opt* _temp2662;
struct Cyc_Absyn_Pat* _temp2664; struct Cyc_Absyn_Structdecl* _temp2666; struct
Cyc_Absyn_Uniondecl* _temp2668; struct Cyc_Absyn_Tuniondecl* _temp2670; struct
Cyc_Absyn_Enumdecl* _temp2672; struct Cyc_Absyn_Typedefdecl* _temp2674; struct
Cyc_List_List* _temp2676; struct _tagged_string* _temp2678; struct Cyc_List_List*
_temp2680; struct _tuple0* _temp2682; struct Cyc_List_List* _temp2684; _LL2630:
if(*(( int*) _temp2628) == Cyc_Absyn_Var_d){ _LL2653: _temp2652=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp2628)->f1; goto _LL2631;} else{ goto
_LL2632;} _LL2632: if(*(( int*) _temp2628) == Cyc_Absyn_Fn_d){ _LL2655:
_temp2654=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp2628)->f1;
goto _LL2633;} else{ goto _LL2634;} _LL2634: if(*(( int*) _temp2628) == Cyc_Absyn_Let_d){
_LL2665: _temp2664=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2628)->f1; goto _LL2663; _LL2663: _temp2662=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2628)->f2; goto _LL2661; _LL2661: _temp2660=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2628)->f3; goto
_LL2659; _LL2659: _temp2658=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2628)->f4; goto _LL2657; _LL2657: _temp2656=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp2628)->f5; goto _LL2635;} else{ goto _LL2636;} _LL2636: if(*(( int*)
_temp2628) == Cyc_Absyn_Struct_d){ _LL2667: _temp2666=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp2628)->f1; goto _LL2637;} else{ goto
_LL2638;} _LL2638: if(*(( int*) _temp2628) == Cyc_Absyn_Union_d){ _LL2669:
_temp2668=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp2628)->f1; goto _LL2639;} else{ goto _LL2640;} _LL2640: if(*(( int*)
_temp2628) == Cyc_Absyn_Tunion_d){ _LL2671: _temp2670=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp2628)->f1; goto _LL2641;} else{ goto
_LL2642;} _LL2642: if(*(( int*) _temp2628) == Cyc_Absyn_Enum_d){ _LL2673:
_temp2672=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp2628)->f1; goto _LL2643;} else{ goto _LL2644;} _LL2644: if(*(( int*)
_temp2628) == Cyc_Absyn_Typedef_d){ _LL2675: _temp2674=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp2628)->f1; goto _LL2645;} else{ goto
_LL2646;} _LL2646: if(*(( int*) _temp2628) == Cyc_Absyn_Namespace_d){ _LL2679:
_temp2678=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2628)->f1; goto _LL2677; _LL2677: _temp2676=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2628)->f2; goto _LL2647;} else{ goto
_LL2648;} _LL2648: if(*(( int*) _temp2628) == Cyc_Absyn_Using_d){ _LL2683:
_temp2682=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp2628)->f1;
goto _LL2681; _LL2681: _temp2680=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2628)->f2; goto _LL2649;} else{ goto _LL2650;} _LL2650: if(*(( int*)
_temp2628) == Cyc_Absyn_ExternC_d){ _LL2685: _temp2684=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp2628)->f1; goto _LL2651;} else{ goto
_LL2629;} _LL2631: { struct _tuple0* c_name= _temp2652->name; if(( void*)
_temp2652->sc ==( void*) Cyc_Absyn_ExternC){ c_name=({ struct _tuple0* _temp2686=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2686->f1= Cyc_Toc_abs_ns;
_temp2686->f2=(* c_name).f2; _temp2686;});} if( _temp2652->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2687=
_temp2652->initializer; if( _temp2687 == 0){ _throw( Null_Exception);} _temp2687;}));}
if( ! nv->toplevel){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp2688=( char*)"decls_to_c: not at toplevel!";
struct _tagged_string _temp2689; _temp2689.curr= _temp2688; _temp2689.base=
_temp2688; _temp2689.last_plus_one= _temp2688 + 29; _temp2689;}));} nv= Cyc_Toc_add_varmap(
nv, _temp2652->name, Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2690=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2690[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2691; _temp2691.tag= Cyc_Absyn_Global_b;
_temp2691.f1= _temp2652; _temp2691;}); _temp2690;}), 0)); _temp2652->name=
c_name;( void*)( _temp2652->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2652->sc));(
void*)( _temp2652->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2652->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2692=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2692->hd=( void*) d; _temp2692->tl=
Cyc_Toc_result_decls; _temp2692;}); goto _LL2629;} _LL2633: { struct _tuple0*
c_name= _temp2654->name; if(( void*) _temp2654->sc ==( void*) Cyc_Absyn_ExternC){
c_name=({ struct _tuple0* _temp2693=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp2693->f1= Cyc_Toc_abs_ns; _temp2693->f2=(* c_name).f2; _temp2693;});}
nv= Cyc_Toc_add_varmap( nv, _temp2654->name, Cyc_Absyn_var_exp( c_name, 0));
_temp2654->name= c_name; Cyc_Toc_fndecl_to_c( nv, _temp2654); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2694=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2694->hd=( void*) d; _temp2694->tl= Cyc_Toc_result_decls;
_temp2694;}); goto _LL2629;} _LL2635:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2695=( char*)"letdecl"; struct
_tagged_string _temp2696; _temp2696.curr= _temp2695; _temp2696.base= _temp2695;
_temp2696.last_plus_one= _temp2695 + 8; _temp2696;})); goto _LL2629; _LL2637:
Cyc_Toc_structdecl_to_c( _temp2666); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2697=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2697->hd=( void*) d; _temp2697->tl= Cyc_Toc_result_decls; _temp2697;});
goto _LL2629; _LL2639: Cyc_Toc_uniondecl_to_c( _temp2668); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2698=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2698->hd=( void*) d; _temp2698->tl= Cyc_Toc_result_decls;
_temp2698;}); goto _LL2629; _LL2641: if( _temp2670->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2670);} else{ Cyc_Toc_tuniondecl_to_c( _temp2670);} goto _LL2629; _LL2643:
Cyc_Toc_enumdecl_to_c( nv, _temp2672); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2699=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2699->hd=( void*) d; _temp2699->tl= Cyc_Toc_result_decls; _temp2699;});
goto _LL2629; _LL2645: _temp2674->name= _temp2674->name; _temp2674->tvs= 0;(
void*)( _temp2674->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2674->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2700=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2700->hd=( void*) d; _temp2700->tl=
Cyc_Toc_result_decls; _temp2700;}); goto _LL2629; _LL2647: _temp2680= _temp2676;
goto _LL2649; _LL2649: _temp2684= _temp2680; goto _LL2651; _LL2651: nv= Cyc_Toc_decls_to_c(
nv, _temp2684, top); goto _LL2629; _LL2629:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}