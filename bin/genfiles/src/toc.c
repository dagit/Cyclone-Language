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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
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
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType= 11;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 12; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 13; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_Tunion_e= 27;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 28; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
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
struct Cyc_Absyn_Exp*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate*
Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t;
struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct
_tagged_string Cyc_Absynpp_typ2string( void*); extern struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0*); extern struct _tagged_string Cyc_Absynpp_prim2string( void* p);
static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void* Cyc_Tcutil_typ_kind(
void* t); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify(
void*, void*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_Absyn_Structdecl* sd); extern int Cyc_Tcutil_is_tagged_pointer_typ(
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
Cyc_Stdio_fflush( Cyc_Stdio_stderr);( void) _throw(( void*) Cyc_Toc_Toc_Unimplemented);}
static void* Cyc_Toc_toc_impos( struct _tagged_string s){({ struct
_tagged_string _temp2= s; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp2.last_plus_one
- _temp2.curr, _temp2.curr);}); Cyc_Stdio_fflush( Cyc_Stdio_stderr);( void)
_throw(( void*) Cyc_Toc_Toc_Impossible);} char Cyc_Toc_Match_error[ 16u]="\000\000\000\000Match_error";
static char _temp5[ 5u]="curr"; static struct _tagged_string Cyc_Toc_curr_string=(
struct _tagged_string){ _temp5, _temp5, _temp5 + 5u}; static struct
_tagged_string* Cyc_Toc_curr_sp=& Cyc_Toc_curr_string; static char _temp8[ 14u]="last_plus_one";
static struct _tagged_string Cyc_Toc_last_plus_one_string=( struct
_tagged_string){ _temp8, _temp8, _temp8 + 14u}; static struct _tagged_string*
Cyc_Toc_last_plus_one_sp=& Cyc_Toc_last_plus_one_string; static char _temp11[ 5u]="base";
static struct _tagged_string Cyc_Toc_base_string=( struct _tagged_string){
_temp11, _temp11, _temp11 + 5u}; static struct _tagged_string* Cyc_Toc_base_sp=&
Cyc_Toc_base_string; static char _temp14[ 15u]="_tagged_string"; static struct
_tagged_string Cyc_Toc__tagged_string_string=( struct _tagged_string){ _temp14,
_temp14, _temp14 + 15u}; static struct _tagged_string* Cyc_Toc__tagged_string_sp=&
Cyc_Toc__tagged_string_string; static char _temp17[ 8u]="xtunion"; static struct
_tagged_string Cyc_Toc_xtunion_string=( struct _tagged_string){ _temp17, _temp17,
_temp17 + 8u}; static struct _tagged_string* Cyc_Toc_xtunion_sp=& Cyc_Toc_xtunion_string;
static char _temp20[ 2u]="v"; static struct _tagged_string Cyc_Toc_v_string=(
struct _tagged_string){ _temp20, _temp20, _temp20 + 2u}; static struct
_tagged_string* Cyc_Toc_v_sp=& Cyc_Toc_v_string; static char _temp23[ 4u]="tag";
static struct _tagged_string Cyc_Toc_tag_string=( struct _tagged_string){
_temp23, _temp23, _temp23 + 4u}; static struct _tagged_string* Cyc_Toc_tag_sp=&
Cyc_Toc_tag_string; static char _temp26[ 14u]="_handler_cons"; static struct
_tagged_string Cyc_Toc__handler_cons_string=( struct _tagged_string){ _temp26,
_temp26, _temp26 + 14u}; static struct _tagged_string* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static char _temp29[ 7u]="setjmp"; static struct
_tagged_string Cyc_Toc_setjmp_string=( struct _tagged_string){ _temp29, _temp29,
_temp29 + 7u}; static struct _tagged_string* Cyc_Toc_setjmp_sp=& Cyc_Toc_setjmp_string;
static char _temp32[ 14u]="_push_handler"; static struct _tagged_string Cyc_Toc__push_handler_string=(
struct _tagged_string){ _temp32, _temp32, _temp32 + 14u}; static struct
_tagged_string* Cyc_Toc__push_handler_sp=& Cyc_Toc__push_handler_string; static
char _temp35[ 13u]="_pop_handler"; static struct _tagged_string Cyc_Toc__pop_handler_string=(
struct _tagged_string){ _temp35, _temp35, _temp35 + 13u}; static struct
_tagged_string* Cyc_Toc__pop_handler_sp=& Cyc_Toc__pop_handler_string; static
char _temp38[ 8u]="handler"; static struct _tagged_string Cyc_Toc_handler_string=(
struct _tagged_string){ _temp38, _temp38, _temp38 + 8u}; static struct
_tagged_string* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static char _temp41[
14u]="_npop_handler"; static struct _tagged_string Cyc_Toc__npop_handler_string=(
struct _tagged_string){ _temp41, _temp41, _temp41 + 14u}; static struct _tuple0
Cyc_Toc__npop_handler_pr=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__npop_handler_string}; static struct _tuple0* Cyc_Toc__npop_handler_qv=&
Cyc_Toc__npop_handler_pr; static char _temp44[ 16u]="Match_Exception"; static
struct _tagged_string Cyc_Toc_Match_Exception_string=( struct _tagged_string){
_temp44, _temp44, _temp44 + 16u}; static struct _tagged_string* Cyc_Toc_Match_Exception_sp=&
Cyc_Toc_Match_Exception_string; static char _temp47[ 15u]="Cyc_new_string";
static struct _tagged_string Cyc_Toc_Cyc_new_string_string=( struct
_tagged_string){ _temp47, _temp47, _temp47 + 15u}; static struct _tagged_string*
Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string; static char _temp50[
16u]="Cyc_rnew_string"; static struct _tagged_string Cyc_Toc_Cyc_rnew_string_string=(
struct _tagged_string){ _temp50, _temp50, _temp50 + 16u}; static struct
_tagged_string* Cyc_Toc_Cyc_rnew_string_sp=& Cyc_Toc_Cyc_rnew_string_string;
static char _temp53[ 14u]="_RegionHandle"; static struct _tagged_string Cyc_Toc__region_handle_string=(
struct _tagged_string){ _temp53, _temp53, _temp53 + 14u}; static struct
_tagged_string* Cyc_Toc__region_handle_sp=& Cyc_Toc__region_handle_string;
static char _temp56[ 13u]="_free_region"; static struct _tagged_string Cyc_Toc__free_region_string=(
struct _tagged_string){ _temp56, _temp56, _temp56 + 13u}; static struct
_tagged_string* Cyc_Toc__free_region_sp=& Cyc_Toc__free_region_string; static
char _temp59[ 12u]="_new_region"; static struct _tagged_string Cyc_Toc__new_region_string=(
struct _tagged_string){ _temp59, _temp59, _temp59 + 12u}; static struct
_tagged_string* Cyc_Toc__new_region_sp=& Cyc_Toc__new_region_string; struct
_tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp* e){ void* _temp60=( void*) e->r; void* _temp86; char
_temp88; void* _temp90; void* _temp92; short _temp94; void* _temp96; void*
_temp98; int _temp100; void* _temp102; void* _temp104; long long _temp106; void*
_temp108; void* _temp110; struct Cyc_Absyn_Exp* _temp112; void* _temp114; struct
Cyc_List_List* _temp116; struct Cyc_List_List* _temp118; struct Cyc_Absyn_Structdecl*
_temp120; struct Cyc_List_List* _temp122; struct Cyc_Core_Opt* _temp124; struct
_tuple0* _temp126; struct Cyc_List_List* _temp128; struct _tuple1* _temp130;
struct Cyc_List_List* _temp132; struct Cyc_Core_Opt* _temp134; _LL62: if(*(( int*)
_temp60) == Cyc_Absyn_Const_e){ _LL87: _temp86=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp60)->f1; if(( unsigned int) _temp86 > 1u?*(( int*) _temp86) == Cyc_Absyn_Char_c:
0){ _LL91: _temp90=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp86)->f1;
goto _LL89; _LL89: _temp88=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp86)->f2;
goto _LL63;} else{ goto _LL64;}} else{ goto _LL64;} _LL64: if(*(( int*) _temp60)
== Cyc_Absyn_Const_e){ _LL93: _temp92=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp60)->f1; if(( unsigned int) _temp92 > 1u?*(( int*) _temp92) == Cyc_Absyn_Short_c:
0){ _LL97: _temp96=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp92)->f1;
goto _LL95; _LL95: _temp94=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp92)->f2;
goto _LL65;} else{ goto _LL66;}} else{ goto _LL66;} _LL66: if(*(( int*) _temp60)
== Cyc_Absyn_Const_e){ _LL99: _temp98=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp60)->f1; if(( unsigned int) _temp98 > 1u?*(( int*) _temp98) == Cyc_Absyn_Int_c:
0){ _LL103: _temp102=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp98)->f1;
goto _LL101; _LL101: _temp100=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp98)->f2;
goto _LL67;} else{ goto _LL68;}} else{ goto _LL68;} _LL68: if(*(( int*) _temp60)
== Cyc_Absyn_Const_e){ _LL105: _temp104=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp60)->f1; if(( unsigned int) _temp104 > 1u?*(( int*) _temp104) == Cyc_Absyn_LongLong_c:
0){ _LL109: _temp108=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp104)->f1;
goto _LL107; _LL107: _temp106=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp104)->f2; goto _LL69;} else{ goto _LL70;}} else{ goto _LL70;} _LL70: if(*((
int*) _temp60) == Cyc_Absyn_Const_e){ _LL111: _temp110=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp60)->f1; if( _temp110 ==( void*) Cyc_Absyn_Null_c){ goto _LL71;} else{ goto
_LL72;}} else{ goto _LL72;} _LL72: if(*(( int*) _temp60) == Cyc_Absyn_Cast_e){
_LL115: _temp114=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp60)->f1; goto
_LL113; _LL113: _temp112=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp60)->f2; goto _LL73;} else{ goto _LL74;} _LL74: if(*(( int*) _temp60) ==
Cyc_Absyn_Tuple_e){ _LL117: _temp116=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp60)->f1; goto _LL75;} else{ goto _LL76;} _LL76: if(*(( int*) _temp60) ==
Cyc_Absyn_Array_e){ _LL119: _temp118=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp60)->f1; goto _LL77;} else{ goto _LL78;} _LL78: if(*(( int*) _temp60) ==
Cyc_Absyn_Struct_e){ _LL127: _temp126=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp60)->f1; goto _LL125; _LL125: _temp124=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp60)->f2; goto _LL123; _LL123: _temp122=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp60)->f3; goto _LL121; _LL121: _temp120=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp60)->f4; goto
_LL79;} else{ goto _LL80;} _LL80: if(*(( int*) _temp60) == Cyc_Absyn_CompoundLit_e){
_LL131: _temp130=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp60)->f1; goto _LL129; _LL129: _temp128=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp60)->f2; goto _LL81;} else{ goto _LL82;}
_LL82: if(*(( int*) _temp60) == Cyc_Absyn_UnresolvedMem_e){ _LL135: _temp134=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp60)->f1;
goto _LL133; _LL133: _temp132=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp60)->f2; goto _LL83;} else{ goto _LL84;} _LL84: goto _LL85; _LL63: return
_temp88 =='\000'; _LL65: return _temp94 == 0; _LL67: return _temp100 == 0; _LL69:
return _temp106 == 0; _LL71: return 1; _LL73: return Cyc_Toc_is_zero( _temp112);
_LL75: return(( int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_forall)( Cyc_Toc_is_zero, _temp116); _LL77: _temp122= _temp118;
goto _LL79; _LL79: _temp128= _temp122; goto _LL81; _LL81: _temp132= _temp128;
goto _LL83; _LL83: for( 0; _temp132 != 0; _temp132=({ struct Cyc_List_List*
_temp136= _temp132; if( _temp136 == 0){ _throw( Null_Exception);} _temp136->tl;})){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)({ struct Cyc_List_List* _temp137=
_temp132; if( _temp137 == 0){ _throw( Null_Exception);} _temp137->hd;}))).f2)){
return 0;}} return 1; _LL85: return 0; _LL61:;} static int Cyc_Toc_is_nullable(
void* t){ void* _temp138= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp144; struct Cyc_Absyn_Conref* _temp146; struct Cyc_Absyn_Tqual _temp148;
struct Cyc_Absyn_Conref* _temp150; void* _temp152; void* _temp154; _LL140: if((
unsigned int) _temp138 > 4u?*(( int*) _temp138) == Cyc_Absyn_PointerType: 0){
_LL145: _temp144=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp138)->f1; _LL155: _temp154=( void*) _temp144.elt_typ; goto _LL153; _LL153:
_temp152=( void*) _temp144.rgn_typ; goto _LL151; _LL151: _temp150=( struct Cyc_Absyn_Conref*)
_temp144.nullable; goto _LL149; _LL149: _temp148=( struct Cyc_Absyn_Tqual)
_temp144.tq; goto _LL147; _LL147: _temp146=( struct Cyc_Absyn_Conref*) _temp144.bounds;
goto _LL141;} else{ goto _LL142;} _LL142: goto _LL143; _LL141: { void* _temp156=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp150))->v; int _temp162; _LL158: if(( unsigned int) _temp156 > 1u?*(( int*)
_temp156) == Cyc_Absyn_Eq_constr: 0){ _LL163: _temp162=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp156)->f1; goto _LL159;} else{ goto _LL160;} _LL160: goto _LL161; _LL159:
return _temp162; _LL161: return 0; _LL157:;} _LL143:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp164=(
char*)"is_nullable"; struct _tagged_string _temp165; _temp165.curr= _temp164;
_temp165.base= _temp164; _temp165.last_plus_one= _temp164 + 12; _temp165;}));
return 0; _LL139:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct
_tuple0* x, struct _tagged_string tag){ return({ struct _tuple0* _temp166=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp166->f1=(* x).f1;
_temp166->f2=({ struct _tagged_string* _temp167=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp167[ 0]= Cyc_String_strconcat(*(*
x).f2, tag); _temp167;}); _temp166;});} static void Cyc_Toc_add_tuple_decl(
struct _tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List* fs= 0;{
int i= 1; for( 0; ts != 0; ts=({ struct Cyc_List_List* _temp168= ts; if(
_temp168 == 0){ _throw( Null_Exception);} _temp168->tl;}), i ++){ fs=({ struct
Cyc_List_List* _temp169=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp169->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp170=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp170->name=({ struct
_tagged_string* _temp172=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp172[ 0]= xprintf("f%d", i); _temp172;}); _temp170->tq=
Cyc_Toc_mt_tq; _temp170->type=( void*)(( void*)({ struct Cyc_List_List* _temp171=
ts; if( _temp171 == 0){ _throw( Null_Exception);} _temp171->hd;})); _temp170->width=
0; _temp170->attributes= 0; _temp170;}); _temp169->tl= fs; _temp169;});}} fs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);{
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp176=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp176->sc=( void*)(( void*) Cyc_Absyn_Public); _temp176->name=({ struct Cyc_Core_Opt*
_temp178=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp178->v=( void*)({ struct _tuple0* _temp179=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp179->f1= Cyc_Toc_abs_ns; _temp179->f2= x;
_temp179;}); _temp178;}); _temp176->tvs= 0; _temp176->fields=({ struct Cyc_Core_Opt*
_temp177=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp177->v=( void*) fs; _temp177;}); _temp176->attributes= 0; _temp176;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp173=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp173->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp174=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp174[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp175; _temp175.tag= Cyc_Absyn_Struct_d; _temp175.f1=
sd; _temp175;}); _temp174;}), 0); _temp173->tl= Cyc_Toc_result_decls; _temp173;});}}
static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x, void* t){
void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Structfield* f1=({ struct Cyc_Absyn_Structfield* _temp192=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp192->name= Cyc_Toc_curr_sp; _temp192->tq= Cyc_Toc_mt_tq; _temp192->type=(
void*) tptr; _temp192->width= 0; _temp192->attributes= 0; _temp192;}); struct
Cyc_Absyn_Structfield* f2=({ struct Cyc_Absyn_Structfield* _temp191=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp191->name= Cyc_Toc_base_sp;
_temp191->tq= Cyc_Toc_mt_tq; _temp191->type=( void*) tptr; _temp191->width= 0;
_temp191->attributes= 0; _temp191;}); struct Cyc_Absyn_Structfield* f3=({ struct
Cyc_Absyn_Structfield* _temp190=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp190->name= Cyc_Toc_last_plus_one_sp;
_temp190->tq= Cyc_Toc_mt_tq; _temp190->type=( void*) tptr; _temp190->width= 0;
_temp190->attributes= 0; _temp190;}); struct Cyc_List_List* fs=({ struct Cyc_List_List*
_temp187=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp187->hd=( void*) f1; _temp187->tl=({ struct Cyc_List_List* _temp188=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp188->hd=(
void*) f2; _temp188->tl=({ struct Cyc_List_List* _temp189=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp189->hd=( void*) f3; _temp189->tl=
0; _temp189;}); _temp188;}); _temp187;}); struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp183=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp183->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp183->name=({ struct Cyc_Core_Opt* _temp185=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp185->v=( void*)({ struct _tuple0*
_temp186=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp186->f1=
Cyc_Toc_abs_ns; _temp186->f2= x; _temp186;}); _temp185;}); _temp183->tvs= 0;
_temp183->fields=({ struct Cyc_Core_Opt* _temp184=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp184->v=( void*)(( struct Cyc_List_List*)
fs); _temp184;}); _temp183->attributes= 0; _temp183;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp180=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp180->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp181=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp181[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp182; _temp182.tag= Cyc_Absyn_Struct_d; _temp182.f1=
sd; _temp182;}); _temp181;}), 0); _temp180->tl= Cyc_Toc_result_decls; _temp180;});}
static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int Cyc_Toc_tuple_type_counter=
0; struct _tuple5{ struct _tagged_string* f1; struct Cyc_List_List* f2; } ;
static struct _tagged_string* Cyc_Toc_add_tuple_type( struct Cyc_List_List* tqs0){{
struct Cyc_List_List* tts= Cyc_Toc_tuple_types; for( 0; tts != 0; tts=({ struct
Cyc_List_List* _temp193= tts; if( _temp193 == 0){ _throw( Null_Exception);}
_temp193->tl;})){ struct _tuple5 _temp197; struct Cyc_List_List* _temp198;
struct _tagged_string* _temp200; struct _tuple5* _temp195=( struct _tuple5*)({
struct Cyc_List_List* _temp194= tts; if( _temp194 == 0){ _throw( Null_Exception);}
_temp194->hd;}); _temp197=* _temp195; _LL201: _temp200= _temp197.f1; goto _LL199;
_LL199: _temp198= _temp197.f2; goto _LL196; _LL196: { int okay= 1; struct Cyc_List_List*
tqs= tqs0; for( 0; tqs != 0? _temp198 != 0: 0; tqs=({ struct Cyc_List_List*
_temp202= tqs; if( _temp202 == 0){ _throw( Null_Exception);} _temp202->tl;}),
_temp198=({ struct Cyc_List_List* _temp203= _temp198; if( _temp203 == 0){ _throw(
Null_Exception);} _temp203->tl;})){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)({
struct Cyc_List_List* _temp204= tqs; if( _temp204 == 0){ _throw( Null_Exception);}
_temp204->hd;}))).f2,( void*)({ struct Cyc_List_List* _temp205= _temp198; if(
_temp205 == 0){ _throw( Null_Exception);} _temp205->hd;}))){ okay= 0; break;}}
if(( ! okay? 1: tqs != 0)? 1: _temp198 != 0){ continue;} return _temp200;}}}{
struct _tagged_string* x=({ struct _tagged_string* _temp208=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp208[ 0]=
xprintf("_tuple%d",( Cyc_Toc_tuple_type_counter ++)); _temp208;}); struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); Cyc_Toc_add_tuple_decl( x, ts);
Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp206=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp206->hd=( void*)({ struct
_tuple5* _temp207=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp207->f1= x; _temp207->f2= ts; _temp207;}); _temp206->tl= Cyc_Toc_tuple_types;
_temp206;}); return x;}} static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types=
0; static int Cyc_Toc_tagged_ptr_counter= 0; struct _tuple6{ struct
_tagged_string* f1; void* f2; } ; static struct _tagged_string* Cyc_Toc_add_tagged_ptr_type(
void* t, struct Cyc_Absyn_Tqual tq){{ void* _temp209= t; void* _temp215; void*
_temp217; _LL211: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType:
0){ _LL218: _temp217=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1;
if( _temp217 ==( void*) Cyc_Absyn_Unsigned){ goto _LL216;} else{ goto _LL213;}
_LL216: _temp215=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if(
_temp215 ==( void*) Cyc_Absyn_B1){ goto _LL212;} else{ goto _LL213;}} else{ goto
_LL213;} _LL213: goto _LL214; _LL212: return Cyc_Toc__tagged_string_sp; _LL214:
goto _LL210; _LL210:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts=({ struct Cyc_List_List* _temp219= tts; if( _temp219 == 0){
_throw( Null_Exception);} _temp219->tl;})){ struct _tuple6 _temp223; void*
_temp224; struct _tagged_string* _temp226; struct _tuple6* _temp221=( struct
_tuple6*)({ struct Cyc_List_List* _temp220= tts; if( _temp220 == 0){ _throw(
Null_Exception);} _temp220->hd;}); _temp223=* _temp221; _LL227: _temp226=
_temp223.f1; goto _LL225; _LL225: _temp224= _temp223.f2; goto _LL222; _LL222:
if( ! Cyc_Tcutil_unify( t, _temp224)){ continue;} return _temp226;}}{ struct
_tagged_string* x=({ struct _tagged_string* _temp230=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp230[ 0]= xprintf("_tagged_ptr%d",(
Cyc_Toc_tagged_ptr_counter ++)); _temp230;}); Cyc_Toc_add_tagged_ptr_decl( x, t);
Cyc_Toc_tagged_ptr_types=({ struct Cyc_List_List* _temp228=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp228->hd=( void*)({ struct
_tuple6* _temp229=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp229->f1= x; _temp229->f2= t; _temp229;}); _temp228->tl= Cyc_Toc_tagged_ptr_types;
_temp228;}); return x;}} static int Cyc_Toc_temp_var_counter= 0; static struct
_tuple0* Cyc_Toc_temp_var(){ return({ struct _tuple0* _temp231=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp231->f1= Cyc_Toc_abs_ns; _temp231->f2=({
struct _tagged_string* _temp232=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp232[ 0]= xprintf("_temp%d",( Cyc_Toc_temp_var_counter
++)); _temp232;}); _temp231;});} static int Cyc_Toc_fresh_label_counter= 0;
static struct _tagged_string* Cyc_Toc_fresh_label(){ return({ struct
_tagged_string* _temp233=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp233[ 0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter
++)); _temp233;});} static void* Cyc_Toc_typ_to_c( void* t); static struct
_tuple1* Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp236; struct Cyc_Absyn_Tqual
_temp238; struct Cyc_Core_Opt* _temp240; struct _tuple1 _temp234=* a; _LL241:
_temp240= _temp234.f1; goto _LL239; _LL239: _temp238= _temp234.f2; goto _LL237;
_LL237: _temp236= _temp234.f3; goto _LL235; _LL235: return({ struct _tuple1*
_temp242=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp242->f1=
_temp240; _temp242->f2= _temp238; _temp242->f3= Cyc_Toc_typ_to_c( _temp236);
_temp242;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp245; struct Cyc_Absyn_Tqual _temp247; struct _tuple3 _temp243=* x;
_LL248: _temp247= _temp243.f1; goto _LL246; _LL246: _temp245= _temp243.f2; goto
_LL244; _LL244: return({ struct _tuple3* _temp249=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp249->f1= _temp247; _temp249->f2= Cyc_Toc_typ_to_c(
_temp245); _temp249;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp250= t; struct Cyc_Absyn_Exp* _temp258; struct Cyc_Absyn_Tqual _temp260;
void* _temp262; int _temp264; struct Cyc_Core_Opt* _temp266; struct Cyc_Core_Opt
_temp268; void* _temp269; void* _temp271; _LL252: if(( unsigned int) _temp250 >
4u?*(( int*) _temp250) == Cyc_Absyn_ArrayType: 0){ _LL263: _temp262=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp250)->f1; goto _LL261; _LL261: _temp260=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp250)->f2;
goto _LL259; _LL259: _temp258=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp250)->f3; goto _LL253;} else{ goto _LL254;} _LL254: if(( unsigned int)
_temp250 > 4u?*(( int*) _temp250) == Cyc_Absyn_Evar: 0){ _LL272: _temp271=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp250)->f1; goto _LL267; _LL267: _temp266=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp250)->f2; if(
_temp266 == 0){ goto _LL256;} else{ _temp268=* _temp266; _LL270: _temp269=( void*)
_temp268.v; goto _LL265;} _LL265: _temp264=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp250)->f3; goto _LL255;} else{ goto _LL256;} _LL256: goto _LL257; _LL253:
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp273=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp273[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp274; _temp274.tag= Cyc_Absyn_ArrayType; _temp274.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp262); _temp274.f2= _temp260; _temp274.f3=
_temp258; _temp274;}); _temp273;}); _LL255: return Cyc_Toc_typ_to_c_array(
_temp269); _LL257: return Cyc_Toc_typ_to_c( t); _LL251:;} static void* Cyc_Toc_typ_to_c(
void* t){ void* _temp275= t; int _temp323; struct Cyc_Core_Opt* _temp325; void*
_temp327; int _temp329; struct Cyc_Core_Opt* _temp331; struct Cyc_Core_Opt
_temp333; void* _temp334; void* _temp336; struct Cyc_Absyn_Tvar* _temp338;
struct Cyc_Absyn_TunionInfo _temp340; void* _temp342; struct Cyc_List_List*
_temp344; void* _temp346; struct Cyc_Absyn_Tuniondecl* _temp348; struct Cyc_Absyn_TunionInfo
_temp350; struct Cyc_Absyn_TunionFieldInfo _temp352; struct Cyc_List_List*
_temp354; void* _temp356; struct Cyc_Absyn_Tunionfield* _temp358; struct Cyc_Absyn_Tuniondecl*
_temp360; struct Cyc_Absyn_TunionFieldInfo _temp362; struct Cyc_Absyn_PtrInfo
_temp364; struct Cyc_Absyn_Conref* _temp366; struct Cyc_Absyn_Tqual _temp368;
struct Cyc_Absyn_Conref* _temp370; void* _temp372; void* _temp374; void*
_temp376; void* _temp378; struct Cyc_Absyn_Exp* _temp380; struct Cyc_Absyn_Tqual
_temp382; void* _temp384; struct Cyc_Absyn_FnInfo _temp386; struct Cyc_List_List*
_temp388; int _temp390; struct Cyc_List_List* _temp392; void* _temp394; struct
Cyc_Core_Opt* _temp396; struct Cyc_List_List* _temp398; struct Cyc_List_List*
_temp400; struct Cyc_Absyn_Uniondecl** _temp402; struct Cyc_List_List* _temp404;
struct _tuple0* _temp406; struct Cyc_Absyn_Structdecl** _temp408; struct Cyc_List_List*
_temp410; struct _tuple0* _temp412; struct Cyc_Absyn_Enumdecl* _temp414; struct
_tuple0* _temp416; struct Cyc_Core_Opt* _temp418; struct Cyc_List_List* _temp420;
struct _tuple0* _temp422; void* _temp424; void* _temp426; struct Cyc_List_List*
_temp428; _LL277: if( _temp275 ==( void*) Cyc_Absyn_VoidType){ goto _LL278;}
else{ goto _LL279;} _LL279: if(( unsigned int) _temp275 > 4u?*(( int*) _temp275)
== Cyc_Absyn_Evar: 0){ _LL328: _temp327=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp275)->f1; goto _LL326; _LL326: _temp325=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp275)->f2; if( _temp325 == 0){ goto _LL324;} else{
goto _LL281;} _LL324: _temp323=( int)(( struct Cyc_Absyn_Evar_struct*) _temp275)->f3;
goto _LL280;} else{ goto _LL281;} _LL281: if(( unsigned int) _temp275 > 4u?*((
int*) _temp275) == Cyc_Absyn_Evar: 0){ _LL337: _temp336=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp275)->f1; goto _LL332; _LL332: _temp331=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp275)->f2; if( _temp331 == 0){ goto _LL283;} else{
_temp333=* _temp331; _LL335: _temp334=( void*) _temp333.v; goto _LL330;} _LL330:
_temp329=( int)(( struct Cyc_Absyn_Evar_struct*) _temp275)->f3; goto _LL282;}
else{ goto _LL283;} _LL283: if(( unsigned int) _temp275 > 4u?*(( int*) _temp275)
== Cyc_Absyn_VarType: 0){ _LL339: _temp338=( struct Cyc_Absyn_Tvar*)(( struct
Cyc_Absyn_VarType_struct*) _temp275)->f1; goto _LL284;} else{ goto _LL285;}
_LL285: if(( unsigned int) _temp275 > 4u?*(( int*) _temp275) == Cyc_Absyn_TunionType:
0){ _LL341: _temp340=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp275)->f1; _LL347: _temp346=( void*) _temp340.tunion_info; if(*(( int*)
_temp346) == Cyc_Absyn_KnownTunion){ _LL349: _temp348=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp346)->f1; goto _LL345;} else{ goto
_LL287;} _LL345: _temp344=( struct Cyc_List_List*) _temp340.targs; goto _LL343;
_LL343: _temp342=( void*) _temp340.rgn; goto _LL286;} else{ goto _LL287;} _LL287:
if(( unsigned int) _temp275 > 4u?*(( int*) _temp275) == Cyc_Absyn_TunionType: 0){
_LL351: _temp350=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp275)->f1; goto _LL288;} else{ goto _LL289;} _LL289: if(( unsigned int)
_temp275 > 4u?*(( int*) _temp275) == Cyc_Absyn_TunionFieldType: 0){ _LL353:
_temp352=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp275)->f1; _LL357: _temp356=( void*) _temp352.field_info; if(*(( int*)
_temp356) == Cyc_Absyn_KnownTunionfield){ _LL361: _temp360=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp356)->f1; goto _LL359; _LL359:
_temp358=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp356)->f2; goto _LL355;} else{ goto _LL291;} _LL355: _temp354=( struct Cyc_List_List*)
_temp352.targs; goto _LL290;} else{ goto _LL291;} _LL291: if(( unsigned int)
_temp275 > 4u?*(( int*) _temp275) == Cyc_Absyn_TunionFieldType: 0){ _LL363:
_temp362=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp275)->f1; goto _LL292;} else{ goto _LL293;} _LL293: if(( unsigned int)
_temp275 > 4u?*(( int*) _temp275) == Cyc_Absyn_PointerType: 0){ _LL365: _temp364=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp275)->f1;
_LL375: _temp374=( void*) _temp364.elt_typ; goto _LL373; _LL373: _temp372=( void*)
_temp364.rgn_typ; goto _LL371; _LL371: _temp370=( struct Cyc_Absyn_Conref*)
_temp364.nullable; goto _LL369; _LL369: _temp368=( struct Cyc_Absyn_Tqual)
_temp364.tq; goto _LL367; _LL367: _temp366=( struct Cyc_Absyn_Conref*) _temp364.bounds;
goto _LL294;} else{ goto _LL295;} _LL295: if(( unsigned int) _temp275 > 4u?*((
int*) _temp275) == Cyc_Absyn_IntType: 0){ _LL379: _temp378=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp275)->f1; goto _LL377; _LL377: _temp376=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp275)->f2; goto _LL296;} else{ goto _LL297;} _LL297: if( _temp275 ==( void*)
Cyc_Absyn_FloatType){ goto _LL298;} else{ goto _LL299;} _LL299: if( _temp275 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL300;} else{ goto _LL301;} _LL301: if((
unsigned int) _temp275 > 4u?*(( int*) _temp275) == Cyc_Absyn_ArrayType: 0){
_LL385: _temp384=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp275)->f1;
goto _LL383; _LL383: _temp382=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp275)->f2; goto _LL381; _LL381: _temp380=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp275)->f3; goto _LL302;} else{ goto _LL303;}
_LL303: if(( unsigned int) _temp275 > 4u?*(( int*) _temp275) == Cyc_Absyn_FnType:
0){ _LL387: _temp386=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp275)->f1; _LL399: _temp398=( struct Cyc_List_List*) _temp386.tvars; goto
_LL397; _LL397: _temp396=( struct Cyc_Core_Opt*) _temp386.effect; goto _LL395;
_LL395: _temp394=( void*) _temp386.ret_typ; goto _LL393; _LL393: _temp392=(
struct Cyc_List_List*) _temp386.args; goto _LL391; _LL391: _temp390=( int)
_temp386.varargs; goto _LL389; _LL389: _temp388=( struct Cyc_List_List*)
_temp386.attributes; goto _LL304;} else{ goto _LL305;} _LL305: if(( unsigned int)
_temp275 > 4u?*(( int*) _temp275) == Cyc_Absyn_TupleType: 0){ _LL401: _temp400=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp275)->f1; goto
_LL306;} else{ goto _LL307;} _LL307: if(( unsigned int) _temp275 > 4u?*(( int*)
_temp275) == Cyc_Absyn_UnionType: 0){ _LL407: _temp406=( struct _tuple0*)((
struct Cyc_Absyn_UnionType_struct*) _temp275)->f1; goto _LL405; _LL405: _temp404=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp275)->f2; goto
_LL403; _LL403: _temp402=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp275)->f3; goto _LL308;} else{ goto _LL309;} _LL309: if(( unsigned int)
_temp275 > 4u?*(( int*) _temp275) == Cyc_Absyn_StructType: 0){ _LL413: _temp412=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp275)->f1; goto
_LL411; _LL411: _temp410=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp275)->f2; goto _LL409; _LL409: _temp408=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp275)->f3; goto _LL310;} else{ goto
_LL311;} _LL311: if(( unsigned int) _temp275 > 4u?*(( int*) _temp275) == Cyc_Absyn_EnumType:
0){ _LL417: _temp416=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*)
_temp275)->f1; goto _LL415; _LL415: _temp414=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp275)->f2; goto _LL312;} else{ goto
_LL313;} _LL313: if(( unsigned int) _temp275 > 4u?*(( int*) _temp275) == Cyc_Absyn_TypedefType:
0){ _LL423: _temp422=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp275)->f1; goto _LL421; _LL421: _temp420=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp275)->f2; goto _LL419; _LL419: _temp418=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*) _temp275)->f3;
goto _LL314;} else{ goto _LL315;} _LL315: if(( unsigned int) _temp275 > 4u?*((
int*) _temp275) == Cyc_Absyn_RgnHandleType: 0){ _LL425: _temp424=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp275)->f1; goto _LL316;} else{ goto
_LL317;} _LL317: if( _temp275 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL318;} else{
goto _LL319;} _LL319: if(( unsigned int) _temp275 > 4u?*(( int*) _temp275) ==
Cyc_Absyn_AccessEff: 0){ _LL427: _temp426=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp275)->f1; goto _LL320;} else{ goto _LL321;} _LL321: if(( unsigned int)
_temp275 > 4u?*(( int*) _temp275) == Cyc_Absyn_JoinEff: 0){ _LL429: _temp428=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp275)->f1; goto
_LL322;} else{ goto _LL276;} _LL278: return t; _LL280: return( void*) Cyc_Absyn_VoidType;
_LL282: return Cyc_Toc_typ_to_c( _temp334); _LL284: return Cyc_Absyn_void_star_typ();
_LL286: return Cyc_Absyn_void_star_typ(); _LL288:( void) _throw((( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp430=( char*)"unresolved TunionType"; struct _tagged_string _temp431;
_temp431.curr= _temp430; _temp431.base= _temp430; _temp431.last_plus_one=
_temp430 + 22; _temp431;}))); _LL290: if( _temp358->typs == 0){ if( _temp360->is_xtunion){
return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);}
else{ return Cyc_Absyn_uint_t;}} else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp358->name,( struct _tagged_string)({ char* _temp432=( char*)"_struct";
struct _tagged_string _temp433; _temp433.curr= _temp432; _temp433.base= _temp432;
_temp433.last_plus_one= _temp432 + 8; _temp433;})));} _LL292:( void) _throw(((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp434=( char*)"unresolved TunionFieldType"; struct _tagged_string
_temp435; _temp435.curr= _temp434; _temp435.base= _temp434; _temp435.last_plus_one=
_temp434 + 27; _temp435;}))); _LL294: _temp374= Cyc_Toc_typ_to_c_array( _temp374);{
void* _temp436=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp366))->v; void* _temp442; _LL438: if((
unsigned int) _temp436 > 1u?*(( int*) _temp436) == Cyc_Absyn_Eq_constr: 0){
_LL443: _temp442=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp436)->f1;
if( _temp442 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL439;} else{ goto _LL440;}}
else{ goto _LL440;} _LL440: goto _LL441; _LL439: { struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
_temp374, _temp368); return Cyc_Absyn_strct( n);} _LL441: return Cyc_Absyn_star_typ(
_temp374,( void*) Cyc_Absyn_HeapRgn, _temp368); _LL437:;} _LL296: return t;
_LL298: return t; _LL300: return t; _LL302: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp444=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp444[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp445; _temp445.tag= Cyc_Absyn_ArrayType;
_temp445.f1=( void*) Cyc_Toc_typ_to_c_array( _temp384); _temp445.f2= _temp382;
_temp445.f3= _temp380; _temp445;}); _temp444;}); _LL304: { struct Cyc_List_List*
new_atts= 0; for( 0; _temp388 != 0; _temp388=({ struct Cyc_List_List* _temp446=
_temp388; if( _temp446 == 0){ _throw( Null_Exception);} _temp446->tl;})){ void*
_temp448=( void*)({ struct Cyc_List_List* _temp447= _temp388; if( _temp447 == 0){
_throw( Null_Exception);} _temp447->hd;}); _LL450: if( _temp448 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL451;} else{ goto _LL452;} _LL452: if( _temp448 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL453;} else{ goto _LL454;} _LL454: if( _temp448 ==( void*) Cyc_Absyn_Const_att){
goto _LL455;} else{ goto _LL456;} _LL456: goto _LL457; _LL451: goto _LL453;
_LL453: goto _LL455; _LL455: continue; _LL457: new_atts=({ struct Cyc_List_List*
_temp458=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp458->hd=( void*)(( void*)({ struct Cyc_List_List* _temp459= _temp388; if(
_temp459 == 0){ _throw( Null_Exception);} _temp459->hd;})); _temp458->tl=
new_atts; _temp458;}); goto _LL449; _LL449:;} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp460=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp460[ 0]=({ struct Cyc_Absyn_FnType_struct _temp461; _temp461.tag= Cyc_Absyn_FnType;
_temp461.f1=({ struct Cyc_Absyn_FnInfo _temp462; _temp462.tvars= 0; _temp462.effect=
0; _temp462.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp394); _temp462.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp392); _temp462.varargs= _temp390;
_temp462.attributes= new_atts; _temp462;}); _temp461;}); _temp460;});} _LL306:
_temp400=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp400);{ struct
_tagged_string* n= Cyc_Toc_add_tuple_type( _temp400); return Cyc_Absyn_strct( n);}
_LL308: if( _temp406 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp463=( char*)"anonymous union"; struct
_tagged_string _temp464; _temp464.curr= _temp463; _temp464.base= _temp463;
_temp464.last_plus_one= _temp463 + 16; _temp464;}));} return Cyc_Absyn_unionq_typ((
struct _tuple0*)({ struct _tuple0* _temp465= _temp406; if( _temp465 == 0){
_throw( Null_Exception);} _temp465;})); _LL310: if( _temp412 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp466=( char*)"anonymous struct"; struct _tagged_string _temp467; _temp467.curr=
_temp466; _temp467.base= _temp466; _temp467.last_plus_one= _temp466 + 17;
_temp467;}));} return Cyc_Absyn_strctq(( struct _tuple0*)({ struct _tuple0*
_temp468= _temp412; if( _temp468 == 0){ _throw( Null_Exception);} _temp468;}));
_LL312: return t; _LL314: if( _temp418 == 0){ return(( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp469=(
char*)"Toc::typ_to_c: unresolved TypedefType"; struct _tagged_string _temp470;
_temp470.curr= _temp469; _temp470.base= _temp469; _temp470.last_plus_one=
_temp469 + 38; _temp470;}));} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp471=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp471[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp472; _temp472.tag= Cyc_Absyn_TypedefType;
_temp472.f1= _temp422; _temp472.f2= 0; _temp472.f3=({ struct Cyc_Core_Opt*
_temp473=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp473->v=( void*) Cyc_Toc_typ_to_c_array(( void*)({ struct Cyc_Core_Opt*
_temp474= _temp418; if( _temp474 == 0){ _throw( Null_Exception);} _temp474->v;}));
_temp473;}); _temp472;}); _temp471;}); _LL316: return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__region_handle_sp), Cyc_Toc_mt_tq); _LL318: return(( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp475=(
char*)"Toc::typ_to_c: type translation passed the heap region"; struct
_tagged_string _temp476; _temp476.curr= _temp475; _temp476.base= _temp475;
_temp476.last_plus_one= _temp475 + 55; _temp476;})); _LL320: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp477=( char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp478; _temp478.curr= _temp477; _temp478.base= _temp477;
_temp478.last_plus_one= _temp477 + 54; _temp478;})); _LL322: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp479=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp480; _temp480.curr= _temp479; _temp480.base= _temp479;
_temp480.last_plus_one= _temp479 + 52; _temp480;})); _LL276:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp481= t; struct Cyc_Absyn_Exp* _temp487; struct Cyc_Absyn_Tqual
_temp489; void* _temp491; _LL483: if(( unsigned int) _temp481 > 4u?*(( int*)
_temp481) == Cyc_Absyn_ArrayType: 0){ _LL492: _temp491=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp481)->f1; goto _LL490; _LL490: _temp489=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp481)->f2; goto _LL488; _LL488: _temp487=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp481)->f3; goto
_LL484;} else{ goto _LL485;} _LL485: goto _LL486; _LL484: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp491,( void*) Cyc_Absyn_HeapRgn, _temp489), e, l);
_LL486: return e; _LL482:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp493= t; struct Cyc_Absyn_Exp* _temp499; struct Cyc_Absyn_Tqual _temp501;
void* _temp503; _LL495: if(( unsigned int) _temp493 > 4u?*(( int*) _temp493) ==
Cyc_Absyn_ArrayType: 0){ _LL504: _temp503=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp493)->f1; goto _LL502; _LL502: _temp501=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp493)->f2; goto _LL500; _LL500: _temp499=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp493)->f3; goto
_LL496;} else{ goto _LL497;} _LL497: goto _LL498; _LL496: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp503,( void*) Cyc_Absyn_HeapRgn, _temp501), e, l);
_LL498: return Cyc_Absyn_cast_exp( t, e, l); _LL494:;} static int Cyc_Toc_atomic_typ(
void* t){ void* _temp505= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp537; void* _temp539; void* _temp541; struct Cyc_Absyn_Exp* _temp543; struct
Cyc_Absyn_Tqual _temp545; void* _temp547; struct Cyc_Absyn_FnInfo _temp549;
struct Cyc_List_List* _temp551; struct Cyc_Absyn_Structdecl** _temp553; struct
Cyc_List_List* _temp555; struct _tuple0* _temp557; struct Cyc_Absyn_TunionInfo
_temp559; struct Cyc_Absyn_TunionFieldInfo _temp561; struct Cyc_List_List*
_temp563; void* _temp565; struct Cyc_Absyn_Tunionfield* _temp567; struct Cyc_Absyn_Tuniondecl*
_temp569; struct Cyc_Absyn_Uniondecl** _temp571; struct Cyc_List_List* _temp573;
struct _tuple0* _temp575; struct Cyc_Absyn_PtrInfo _temp577; void* _temp579;
_LL507: if( _temp505 ==( void*) Cyc_Absyn_VoidType){ goto _LL508;} else{ goto
_LL509;} _LL509: if(( unsigned int) _temp505 > 4u?*(( int*) _temp505) == Cyc_Absyn_VarType:
0){ _LL538: _temp537=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp505)->f1; goto _LL510;} else{ goto _LL511;} _LL511: if(( unsigned int)
_temp505 > 4u?*(( int*) _temp505) == Cyc_Absyn_IntType: 0){ _LL542: _temp541=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp505)->f1; goto _LL540; _LL540:
_temp539=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp505)->f2; goto _LL512;}
else{ goto _LL513;} _LL513: if( _temp505 ==( void*) Cyc_Absyn_FloatType){ goto
_LL514;} else{ goto _LL515;} _LL515: if( _temp505 ==( void*) Cyc_Absyn_DoubleType){
goto _LL516;} else{ goto _LL517;} _LL517: if(( unsigned int) _temp505 > 4u?*((
int*) _temp505) == Cyc_Absyn_ArrayType: 0){ _LL548: _temp547=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp505)->f1; goto _LL546; _LL546: _temp545=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp505)->f2;
goto _LL544; _LL544: _temp543=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp505)->f3; goto _LL518;} else{ goto _LL519;} _LL519: if(( unsigned int)
_temp505 > 4u?*(( int*) _temp505) == Cyc_Absyn_FnType: 0){ _LL550: _temp549=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp505)->f1; goto
_LL520;} else{ goto _LL521;} _LL521: if(( unsigned int) _temp505 > 4u?*(( int*)
_temp505) == Cyc_Absyn_TupleType: 0){ _LL552: _temp551=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp505)->f1; goto _LL522;} else{ goto
_LL523;} _LL523: if(( unsigned int) _temp505 > 4u?*(( int*) _temp505) == Cyc_Absyn_StructType:
0){ _LL558: _temp557=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp505)->f1; goto _LL556; _LL556: _temp555=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp505)->f2; goto _LL554; _LL554: _temp553=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp505)->f3;
goto _LL524;} else{ goto _LL525;} _LL525: if(( unsigned int) _temp505 > 4u?*((
int*) _temp505) == Cyc_Absyn_TunionType: 0){ _LL560: _temp559=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp505)->f1; goto _LL526;} else{ goto
_LL527;} _LL527: if(( unsigned int) _temp505 > 4u?*(( int*) _temp505) == Cyc_Absyn_TunionFieldType:
0){ _LL562: _temp561=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp505)->f1; _LL566: _temp565=( void*) _temp561.field_info; if(*(( int*)
_temp565) == Cyc_Absyn_KnownTunionfield){ _LL570: _temp569=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp565)->f1; goto _LL568; _LL568:
_temp567=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp565)->f2; goto _LL564;} else{ goto _LL529;} _LL564: _temp563=( struct Cyc_List_List*)
_temp561.targs; goto _LL528;} else{ goto _LL529;} _LL529: if(( unsigned int)
_temp505 > 4u?*(( int*) _temp505) == Cyc_Absyn_UnionType: 0){ _LL576: _temp575=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp505)->f1; goto
_LL574; _LL574: _temp573=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp505)->f2; goto _LL572; _LL572: _temp571=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp505)->f3; goto _LL530;} else{ goto
_LL531;} _LL531: if(( unsigned int) _temp505 > 4u?*(( int*) _temp505) == Cyc_Absyn_PointerType:
0){ _LL578: _temp577=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp505)->f1; goto _LL532;} else{ goto _LL533;} _LL533: if(( unsigned int)
_temp505 > 4u?*(( int*) _temp505) == Cyc_Absyn_RgnHandleType: 0){ _LL580:
_temp579=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp505)->f1; goto
_LL534;} else{ goto _LL535;} _LL535: goto _LL536; _LL508: return 1; _LL510:
return 0; _LL512: return 1; _LL514: return 1; _LL516: return 0; _LL518: return
Cyc_Toc_atomic_typ( _temp547); _LL520: return 1; _LL522: for( 0; _temp551 != 0;
_temp551=({ struct Cyc_List_List* _temp581= _temp551; if( _temp581 == 0){ _throw(
Null_Exception);} _temp581->tl;})){ if( ! Cyc_Toc_atomic_typ((*(( struct _tuple3*)({
struct Cyc_List_List* _temp582= _temp551; if( _temp582 == 0){ _throw(
Null_Exception);} _temp582->hd;}))).f2)){ return 0;}} return 1; _LL524: return 0;
_LL526: return 0; _LL528: return _temp567->typs == 0; _LL530: return 0; _LL532:
return 0; _LL534: return 0; _LL536:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp583= Cyc_Absynpp_typ2string( t); xprintf("atomic_typ:  bad type %.*s",
_temp583.last_plus_one - _temp583.curr, _temp583.curr);})); return 0; _LL506:;}
static int Cyc_Toc_is_void_star( void* t){ void* _temp584= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp590; struct Cyc_Absyn_Conref* _temp592; struct
Cyc_Absyn_Tqual _temp594; struct Cyc_Absyn_Conref* _temp596; void* _temp598;
void* _temp600; _LL586: if(( unsigned int) _temp584 > 4u?*(( int*) _temp584) ==
Cyc_Absyn_PointerType: 0){ _LL591: _temp590=( struct Cyc_Absyn_PtrInfo)(( struct
Cyc_Absyn_PointerType_struct*) _temp584)->f1; _LL601: _temp600=( void*) _temp590.elt_typ;
goto _LL599; _LL599: _temp598=( void*) _temp590.rgn_typ; goto _LL597; _LL597:
_temp596=( struct Cyc_Absyn_Conref*) _temp590.nullable; goto _LL595; _LL595:
_temp594=( struct Cyc_Absyn_Tqual) _temp590.tq; goto _LL593; _LL593: _temp592=(
struct Cyc_Absyn_Conref*) _temp590.bounds; goto _LL587;} else{ goto _LL588;}
_LL588: goto _LL589; _LL587: { void* _temp602= Cyc_Tcutil_compress( _temp600);
_LL604: if( _temp602 ==( void*) Cyc_Absyn_VoidType){ goto _LL605;} else{ goto
_LL606;} _LL606: goto _LL607; _LL605: return 1; _LL607: return 0; _LL603:;}
_LL589: return 0; _LL585:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_string* f){ void* _temp608= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp620; struct Cyc_Absyn_Structdecl* _temp622; struct Cyc_List_List* _temp623;
struct _tuple0* _temp625; struct Cyc_Absyn_Structdecl** _temp627; struct Cyc_List_List*
_temp629; struct _tuple0* _temp631; struct Cyc_Absyn_Uniondecl** _temp633;
struct Cyc_Absyn_Uniondecl* _temp635; struct Cyc_List_List* _temp636; struct
_tuple0* _temp638; struct Cyc_Absyn_Uniondecl** _temp640; struct Cyc_List_List*
_temp642; struct _tuple0* _temp644; _LL610: if(( unsigned int) _temp608 > 4u?*((
int*) _temp608) == Cyc_Absyn_StructType: 0){ _LL626: _temp625=( struct _tuple0*)((
struct Cyc_Absyn_StructType_struct*) _temp608)->f1; goto _LL624; _LL624:
_temp623=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp608)->f2; goto _LL621; _LL621: _temp620=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp608)->f3; if( _temp620 == 0){ goto
_LL612;} else{ _temp622=* _temp620; goto _LL611;}} else{ goto _LL612;} _LL612:
if(( unsigned int) _temp608 > 4u?*(( int*) _temp608) == Cyc_Absyn_StructType: 0){
_LL632: _temp631=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp608)->f1; goto _LL630; _LL630: _temp629=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp608)->f2; goto _LL628; _LL628: _temp627=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp608)->f3;
if( _temp627 == 0){ goto _LL613;} else{ goto _LL614;}} else{ goto _LL614;}
_LL614: if(( unsigned int) _temp608 > 4u?*(( int*) _temp608) == Cyc_Absyn_UnionType:
0){ _LL639: _temp638=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp608)->f1; goto _LL637; _LL637: _temp636=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp608)->f2; goto _LL634; _LL634: _temp633=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp608)->f3;
if( _temp633 == 0){ goto _LL616;} else{ _temp635=* _temp633; goto _LL615;}}
else{ goto _LL616;} _LL616: if(( unsigned int) _temp608 > 4u?*(( int*) _temp608)
== Cyc_Absyn_UnionType: 0){ _LL645: _temp644=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp608)->f1; goto _LL643; _LL643: _temp642=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp608)->f2; goto _LL641; _LL641: _temp640=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp608)->f3;
if( _temp640 == 0){ goto _LL617;} else{ goto _LL618;}} else{ goto _LL618;}
_LL618: goto _LL619; _LL611: if( _temp625 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp646=(
char*)"StructType name not properly set"; struct _tagged_string _temp647;
_temp647.curr= _temp646; _temp647.base= _temp646; _temp647.last_plus_one=
_temp646 + 33; _temp647;}));} if( _temp622->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp649= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp648= _temp625; if( _temp648 == 0){
_throw( Null_Exception);} _temp648;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp649.last_plus_one - _temp649.curr, _temp649.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp656= _temp622->fields;
if( _temp656 == 0){ _throw( Null_Exception);} _temp656->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp650= fields; if( _temp650 == 0){ _throw(
Null_Exception);} _temp650->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp651= fields; if( _temp651 == 0){ _throw(
Null_Exception);} _temp651->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp652= fields;
if( _temp652 == 0){ _throw( Null_Exception);} _temp652->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp654=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp653= _temp625;
if( _temp653 == 0){ _throw( Null_Exception);} _temp653;})); struct
_tagged_string _temp655=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp654.last_plus_one - _temp654.curr, _temp654.curr, _temp655.last_plus_one -
_temp655.curr, _temp655.curr);})); return 0;} _LL613: if( _temp631 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp657=( char*)"StructType name not properly set"; struct _tagged_string
_temp658; _temp658.curr= _temp657; _temp658.base= _temp657; _temp658.last_plus_one=
_temp657 + 33; _temp658;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp660= Cyc_Absynpp_qvar2string(( struct _tuple0*)({
struct _tuple0* _temp659= _temp631; if( _temp659 == 0){ _throw( Null_Exception);}
_temp659;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp660.last_plus_one
- _temp660.curr, _temp660.curr);})); return 0; _LL615: if( _temp638 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp661=( char*)"UnionType name not properly set"; struct _tagged_string
_temp662; _temp662.curr= _temp661; _temp662.base= _temp661; _temp662.last_plus_one=
_temp661 + 32; _temp662;}));} if( _temp635->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp664= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp663= _temp638; if( _temp663 == 0){
_throw( Null_Exception);} _temp663;})); xprintf("is_poly_field:  union %.*s missing fields",
_temp664.last_plus_one - _temp664.curr, _temp664.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp671= _temp635->fields;
if( _temp671 == 0){ _throw( Null_Exception);} _temp671->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp665= fields; if( _temp665 == 0){ _throw(
Null_Exception);} _temp665->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp666= fields; if( _temp666 == 0){ _throw(
Null_Exception);} _temp666->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp667= fields;
if( _temp667 == 0){ _throw( Null_Exception);} _temp667->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp669=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp668= _temp638;
if( _temp668 == 0){ _throw( Null_Exception);} _temp668;})); struct
_tagged_string _temp670=* f; xprintf("is_poly_field:  union %.*s has no %.*s field",
_temp669.last_plus_one - _temp669.curr, _temp669.curr, _temp670.last_plus_one -
_temp670.curr, _temp670.curr);})); return 0;} _LL617: if( _temp644 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp672=( char*)"UnionType name not properly set"; struct _tagged_string
_temp673; _temp673.curr= _temp672; _temp673.base= _temp672; _temp673.last_plus_one=
_temp672 + 32; _temp673;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp675= Cyc_Absynpp_qvar2string(( struct _tuple0*)({
struct _tuple0* _temp674= _temp644; if( _temp674 == 0){ _throw( Null_Exception);}
_temp674;})); xprintf("is_poly_field:  union %.*s missing structdecl", _temp675.last_plus_one
- _temp675.curr, _temp675.curr);})); return 0; _LL619:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp676= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp676.last_plus_one - _temp676.curr,
_temp676.curr);})); return 0; _LL609:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp677=( void*) e->r; struct _tagged_string*
_temp685; struct Cyc_Absyn_Exp* _temp687; struct _tagged_string* _temp689;
struct Cyc_Absyn_Exp* _temp691; _LL679: if(*(( int*) _temp677) == Cyc_Absyn_StructMember_e){
_LL688: _temp687=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp677)->f1; goto _LL686; _LL686: _temp685=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp677)->f2; goto _LL680;} else{ goto _LL681;}
_LL681: if(*(( int*) _temp677) == Cyc_Absyn_StructArrow_e){ _LL692: _temp691=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp677)->f1;
goto _LL690; _LL690: _temp689=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp677)->f2; goto _LL682;} else{ goto _LL683;} _LL683: goto _LL684; _LL680:
return Cyc_Toc_is_poly_field(( void*)({ struct Cyc_Core_Opt* _temp693= _temp687->topt;
if( _temp693 == 0){ _throw( Null_Exception);} _temp693->v;}), _temp685); _LL682: {
void* _temp695= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp694=
_temp691->topt; if( _temp694 == 0){ _throw( Null_Exception);} _temp694->v;}));
struct Cyc_Absyn_PtrInfo _temp701; struct Cyc_Absyn_Conref* _temp703; struct Cyc_Absyn_Tqual
_temp705; struct Cyc_Absyn_Conref* _temp707; void* _temp709; void* _temp711;
_LL697: if(( unsigned int) _temp695 > 4u?*(( int*) _temp695) == Cyc_Absyn_PointerType:
0){ _LL702: _temp701=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp695)->f1; _LL712: _temp711=( void*) _temp701.elt_typ; goto _LL710; _LL710:
_temp709=( void*) _temp701.rgn_typ; goto _LL708; _LL708: _temp707=( struct Cyc_Absyn_Conref*)
_temp701.nullable; goto _LL706; _LL706: _temp705=( struct Cyc_Absyn_Tqual)
_temp701.tq; goto _LL704; _LL704: _temp703=( struct Cyc_Absyn_Conref*) _temp701.bounds;
goto _LL698;} else{ goto _LL699;} _LL699: goto _LL700; _LL698: return Cyc_Toc_is_poly_field(
_temp711, _temp689); _LL700:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp714= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp713= _temp691->topt; if( _temp713 == 0){ _throw( Null_Exception);} _temp713->v;}));
xprintf("is_poly_project:  bad type %.*s", _temp714.last_plus_one - _temp714.curr,
_temp714.curr);})); return 0; _LL696:;} _LL684: return 0; _LL678:;} static char
_temp717[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp717, _temp717, _temp717 + 10u}; static struct
_tuple0 Cyc_Toc_gc_malloc_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var,
0),({ struct Cyc_List_List* _temp718=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp718->hd=( void*) s; _temp718->tl= 0; _temp718;}), 0);}
static char _temp721[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp721, _temp721, _temp721 + 17u}; static struct
_tuple0 Cyc_Toc_gc_malloc_atomic_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_atomic_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var,
0),({ struct Cyc_List_List* _temp722=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp722->hd=( void*) s; _temp722->tl= 0; _temp722;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static char _temp725[ 15u]="_region_malloc"; static struct _tagged_string
Cyc_Toc__region_malloc_string=( struct _tagged_string){ _temp725, _temp725,
_temp725 + 15u}; static struct _tuple0 Cyc_Toc_region_malloc_var=( struct
_tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__region_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_region_malloc_var,
0),({ struct Cyc_List_List* _temp726=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp726->hd=( void*) rgn; _temp726->tl=({ struct Cyc_List_List*
_temp727=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp727->hd=( void*) s; _temp727->tl= 0; _temp727;}); _temp726;}), 0);} static
const int Cyc_Toc_NullCheck= 0; struct Cyc_Toc_NullCheck_struct{ int tag; void*
f1; } ; static const int Cyc_Toc_UntagPtr= 1; struct Cyc_Toc_UntagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; int f3; } ; static const int Cyc_Toc_TagPtr=
2; struct Cyc_Toc_TagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; }
; typedef void* Cyc_Toc_conv; struct _tuple7{ void* f1; void* f2; } ; static
struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp729=({ struct _tuple7
_temp728; _temp728.f1= t1; _temp728.f2= t2; _temp728;}); void* _temp735; struct
Cyc_Absyn_PtrInfo _temp737; void* _temp739; struct Cyc_Absyn_PtrInfo _temp741;
_LL731: _LL740: _temp739= _temp729.f1; if(( unsigned int) _temp739 > 4u?*(( int*)
_temp739) == Cyc_Absyn_PointerType: 0){ _LL742: _temp741=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp739)->f1; goto _LL736;} else{ goto
_LL733;} _LL736: _temp735= _temp729.f2; if(( unsigned int) _temp735 > 4u?*(( int*)
_temp735) == Cyc_Absyn_PointerType: 0){ _LL738: _temp737=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp735)->f1; goto _LL732;} else{ goto
_LL733;} _LL733: goto _LL734; _LL732: { int n1=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp741.nullable); int n2=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp737.nullable); void* b1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp741.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp737.bounds); struct Cyc_List_List* convs= 0;{
struct _tuple7 _temp744=({ struct _tuple7 _temp743; _temp743.f1= b1; _temp743.f2=
b2; _temp743;}); void* _temp754; struct Cyc_Absyn_Exp* _temp756; void* _temp758;
struct Cyc_Absyn_Exp* _temp760; void* _temp762; void* _temp764; void* _temp766;
void* _temp768; struct Cyc_Absyn_Exp* _temp770; void* _temp772; struct Cyc_Absyn_Exp*
_temp774; void* _temp776; _LL746: _LL759: _temp758= _temp744.f1; if((
unsigned int) _temp758 > 1u?*(( int*) _temp758) == Cyc_Absyn_Upper_b: 0){ _LL761:
_temp760=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp758)->f1;
goto _LL755;} else{ goto _LL748;} _LL755: _temp754= _temp744.f2; if((
unsigned int) _temp754 > 1u?*(( int*) _temp754) == Cyc_Absyn_Upper_b: 0){ _LL757:
_temp756=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp754)->f1;
goto _LL747;} else{ goto _LL748;} _LL748: _LL765: _temp764= _temp744.f1; if(
_temp764 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL763;} else{ goto _LL750;}
_LL763: _temp762= _temp744.f2; if( _temp762 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL749;} else{ goto _LL750;} _LL750: _LL769: _temp768= _temp744.f1; if((
unsigned int) _temp768 > 1u?*(( int*) _temp768) == Cyc_Absyn_Upper_b: 0){ _LL771:
_temp770=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp768)->f1;
goto _LL767;} else{ goto _LL752;} _LL767: _temp766= _temp744.f2; if( _temp766 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL751;} else{ goto _LL752;} _LL752: _LL777:
_temp776= _temp744.f1; if( _temp776 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL773;}
else{ goto _LL745;} _LL773: _temp772= _temp744.f2; if(( unsigned int) _temp772 >
1u?*(( int*) _temp772) == Cyc_Absyn_Upper_b: 0){ _LL775: _temp774=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp772)->f1; goto _LL753;} else{ goto _LL745;}
_LL747: if( n1? ! n2: 0){ convs=({ struct Cyc_List_List* _temp778=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp778->hd=( void*)(( void*)({
struct Cyc_Toc_NullCheck_struct* _temp779=( struct Cyc_Toc_NullCheck_struct*)
GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct)); _temp779[ 0]=({ struct Cyc_Toc_NullCheck_struct
_temp780; _temp780.tag= Cyc_Toc_NullCheck; _temp780.f1=( void*) t2; _temp780;});
_temp779;})); _temp778->tl= convs; _temp778;});} goto _LL745; _LL749: goto
_LL745; _LL751: convs=({ struct Cyc_List_List* _temp781=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp781->hd=( void*)(( void*)({
struct Cyc_Toc_TagPtr_struct* _temp782=( struct Cyc_Toc_TagPtr_struct*)
GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct)); _temp782[ 0]=({ struct Cyc_Toc_TagPtr_struct
_temp783; _temp783.tag= Cyc_Toc_TagPtr; _temp783.f1=( void*) Cyc_Toc_typ_to_c(
t1); _temp783.f2= _temp770; _temp783;}); _temp782;})); _temp781->tl= convs;
_temp781;}); goto _LL745; _LL753: convs=({ struct Cyc_List_List* _temp784=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp784->hd=(
void*)(( void*)({ struct Cyc_Toc_UntagPtr_struct* _temp785=( struct Cyc_Toc_UntagPtr_struct*)
GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct)); _temp785[ 0]=({ struct Cyc_Toc_UntagPtr_struct
_temp786; _temp786.tag= Cyc_Toc_UntagPtr; _temp786.f1=( void*) Cyc_Toc_typ_to_c(
t1); _temp786.f2= _temp774; _temp786.f3= n2; _temp786;}); _temp785;})); _temp784->tl=
convs; _temp784;}); goto _LL745; _LL745:;} return convs;} _LL734: return 0;
_LL730:;}} static char _temp789[ 7u]="_throw"; static struct _tagged_string Cyc_Toc__throw_string=(
struct _tagged_string){ _temp789, _temp789, _temp789 + 7u}; static struct
_tuple0 Cyc_Toc__throw_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__throw_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__throw_var,
0),({ struct Cyc_List_List* _temp790=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp790->hd=( void*) e; _temp790->tl= 0; _temp790;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp791=( char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp792; _temp792.curr= _temp791; _temp792.base= _temp791;
_temp792.last_plus_one= _temp791 + 44; _temp792;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple0* a= Cyc_Toc_temp_var();
struct _tuple0* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp793= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp799; struct Cyc_Absyn_Conref* _temp801; struct Cyc_Absyn_Tqual
_temp803; struct Cyc_Absyn_Conref* _temp805; void* _temp807; void* _temp809;
_LL795: if(( unsigned int) _temp793 > 4u?*(( int*) _temp793) == Cyc_Absyn_PointerType:
0){ _LL800: _temp799=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp793)->f1; _LL810: _temp809=( void*) _temp799.elt_typ; goto _LL808; _LL808:
_temp807=( void*) _temp799.rgn_typ; goto _LL806; _LL806: _temp805=( struct Cyc_Absyn_Conref*)
_temp799.nullable; goto _LL804; _LL804: _temp803=( struct Cyc_Absyn_Tqual)
_temp799.tq; goto _LL802; _LL802: _temp801=( struct Cyc_Absyn_Conref*) _temp799.bounds;
goto _LL796;} else{ goto _LL797;} _LL797: goto _LL798; _LL796: elt_typ= _temp809;{
struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp809, Cyc_Toc_mt_tq);
strt_typ= Cyc_Absyn_strct( n); goto _LL794;} _LL798: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp811=( char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp812;
_temp812.curr= _temp811; _temp812.base= _temp811; _temp812.last_plus_one=
_temp811 + 28; _temp812;})); _LL794:;} if( toplevel){ int is_string= 0;{ void*
_temp813=( void*) e->r; void* _temp819; struct _tagged_string _temp821; _LL815:
if(*(( int*) _temp813) == Cyc_Absyn_Const_e){ _LL820: _temp819=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp813)->f1; if(( unsigned int) _temp819 > 1u?*((
int*) _temp819) == Cyc_Absyn_String_c: 0){ _LL822: _temp821=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp819)->f1; goto _LL816;}
else{ goto _LL817;}} else{ goto _LL817;} _LL817: goto _LL818; _LL816: is_string=
1; goto _LL814; _LL818: goto _LL814; _LL814:;}{ struct Cyc_Absyn_Exp* xexp; if(
is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp826=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp826[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp827; _temp827.tag= Cyc_Absyn_ArrayType; _temp827.f1=(
void*) elt_typ; _temp827.f2= Cyc_Toc_mt_tq; _temp827.f3=( struct Cyc_Absyn_Exp*)
sz; _temp827;}); _temp826;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp823=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp823->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp824=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp824[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp825; _temp825.tag= Cyc_Absyn_Var_d;
_temp825.f1= vd; _temp825;}); _temp824;}), 0); _temp823->tl= Cyc_Toc_result_decls;
_temp823;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp828=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp828->hd=( void*)({ struct _tuple4* _temp833=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp833->f1= 0; _temp833->f2=
xexp; _temp833;}); _temp828->tl=({ struct Cyc_List_List* _temp829=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp829->hd=( void*)({ struct
_tuple4* _temp832=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp832->f1= 0; _temp832->f2= xexp; _temp832;}); _temp829->tl=({ struct Cyc_List_List*
_temp830=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp830->hd=( void*)({ struct _tuple4* _temp831=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp831->f1= 0; _temp831->f2= xplussz; _temp831;});
_temp830->tl= 0; _temp830;}); _temp829;}); _temp828;}), 0); return urm_exp;}}}
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
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp834=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp835; _temp835.curr= _temp834; _temp835.base= _temp834; _temp835.last_plus_one=
_temp834 + 45; _temp835;}));}{ struct _tuple0* x= Cyc_Toc_temp_var(); int
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
_temp836= cs; if( _temp836 == 0){ _throw( Null_Exception);} _temp836->tl;})){
void* _temp838=( void*)({ struct Cyc_List_List* _temp837= cs; if( _temp837 == 0){
_throw( Null_Exception);} _temp837->hd;}); void* _temp846; struct Cyc_Absyn_Exp*
_temp848; void* _temp850; int _temp852; struct Cyc_Absyn_Exp* _temp854; void*
_temp856; _LL840: if(*(( int*) _temp838) == Cyc_Toc_NullCheck){ _LL847: _temp846=(
void*)(( struct Cyc_Toc_NullCheck_struct*) _temp838)->f1; goto _LL841;} else{
goto _LL842;} _LL842: if(*(( int*) _temp838) == Cyc_Toc_TagPtr){ _LL851:
_temp850=( void*)(( struct Cyc_Toc_TagPtr_struct*) _temp838)->f1; goto _LL849;
_LL849: _temp848=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_TagPtr_struct*)
_temp838)->f2; goto _LL843;} else{ goto _LL844;} _LL844: if(*(( int*) _temp838)
== Cyc_Toc_UntagPtr){ _LL857: _temp856=( void*)(( struct Cyc_Toc_UntagPtr_struct*)
_temp838)->f1; goto _LL855; _LL855: _temp854=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Toc_UntagPtr_struct*) _temp838)->f2; goto _LL853; _LL853: _temp852=( int)((
struct Cyc_Toc_UntagPtr_struct*) _temp838)->f3; goto _LL845;} else{ goto _LL839;}
_LL841: e= Cyc_Toc_null_check_conv( toplevel, _temp846, e); goto _LL839; _LL843:
e= Cyc_Toc_tag_ptr_conv( toplevel, _temp850, _temp848, e); goto _LL839; _LL845:
e= Cyc_Toc_untag_ptr_conv( toplevel, _temp856, _temp854, _temp852, e); goto
_LL839; _LL839:;}} return e;} struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab;
struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt* fallthru_info; struct
Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct Cyc_Toc_Env* Cyc_Toc_env_t;
static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({ struct Cyc_Toc_Env*
_temp858=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct Cyc_Toc_Env));
_temp858->break_lab= 0; _temp858->continue_lab= 0; _temp858->fallthru_info= 0;
_temp858->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp858->toplevel= 1;
_temp858;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp859=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp859->break_lab= e->break_lab; _temp859->continue_lab=
e->continue_lab; _temp859->fallthru_info= e->fallthru_info; _temp859->varmap= e->varmap;
_temp859->toplevel= e->toplevel; _temp859;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp860=(* x).f1; struct
Cyc_List_List* _temp866; _LL862: if(( unsigned int) _temp860 > 1u?*(( int*)
_temp860) == Cyc_Absyn_Rel_n: 0){ _LL867: _temp866=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp860)->f1; goto _LL863;} else{ goto _LL864;}
_LL864: goto _LL865; _LL863:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp868=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp868[ 0]=({ struct Cyc_Core_Impossible_struct _temp869; _temp869.tag= Cyc_Core_Impossible;
_temp869.f1=({ struct _tagged_string _temp870= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp870.last_plus_one - _temp870.curr,
_temp870.curr);}); _temp869;}); _temp868;})); _LL865: goto _LL861; _LL861:;}{
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
_temp871= fallthru_binders; if( _temp871 == 0){ _throw( Null_Exception);}
_temp871->tl;})){ fallthru_vars=({ struct Cyc_List_List* _temp872=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp872->hd=( void*)(( struct Cyc_Absyn_Vardecl*)({
struct Cyc_List_List* _temp873= fallthru_binders; if( _temp873 == 0){ _throw(
Null_Exception);} _temp873->hd;}))->name; _temp872->tl= fallthru_vars; _temp872;});}
fallthru_vars=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fallthru_vars);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp874=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp874->v=( void*) break_l; _temp874;}); ans->fallthru_info=({
struct Cyc_Core_Opt* _temp875=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp875->v=( void*)({ struct _tuple8* _temp876=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp876->f1= fallthru_l; _temp876->f2=
fallthru_vars; _temp876->f3= next_case_env->varmap; _temp876;}); _temp875;});
return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct
Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab=({ struct Cyc_Core_Opt* _temp877=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp877->v=( void*) break_l; _temp877;});
ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env* e, struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp878=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp878->v=( void*)({ struct _tuple8* _temp879=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp879->f1= next_l; _temp879->f2= 0; _temp879->f3=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp879;}); _temp878;}); return ans;} struct _tuple9{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
struct Cyc_List_List* f4; char f5; int f6; } ; struct _tuple10{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp880=( char*)"expecting a literal format string";
struct _tagged_string _temp881; _temp881.curr= _temp880; _temp881.base= _temp880;
_temp881.last_plus_one= _temp880 + 34; _temp881;}));}{ struct _tagged_string
fmt_str;{ void* _temp883=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp882= args; if( _temp882 == 0){ _throw( Null_Exception);} _temp882->hd;}))->r;
void* _temp889; struct _tagged_string _temp891; _LL885: if(*(( int*) _temp883)
== Cyc_Absyn_Const_e){ _LL890: _temp889=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp883)->f1; if(( unsigned int) _temp889 > 1u?*(( int*) _temp889) == Cyc_Absyn_String_c:
0){ _LL892: _temp891=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp889)->f1; goto _LL886;} else{ goto _LL887;}} else{ goto _LL887;} _LL887:
goto _LL888; _LL886: fmt_str= _temp891; goto _LL884; _LL888: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp893=( char*)"expecting a literal format string"; struct _tagged_string
_temp894; _temp894.curr= _temp893; _temp894.base= _temp893; _temp894.last_plus_one=
_temp893 + 34; _temp894;})); _LL884:;}{ int len=( int)({ struct _tagged_string
_temp1001= fmt_str;( unsigned int)( _temp1001.last_plus_one - _temp1001.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(({ struct _tagged_string _temp895=
fmt_str; char* _temp897= _temp895.curr + i; if( _temp895.base == 0? 1:( _temp897
< _temp895.base? 1: _temp897 >= _temp895.last_plus_one)){ _throw( Null_Exception);}*
_temp897;}) =='%'){ i ++; if( i < len?({ struct _tagged_string _temp898= fmt_str;
char* _temp900= _temp898.curr + i; if( _temp898.base == 0? 1:( _temp900 <
_temp898.base? 1: _temp900 >= _temp898.last_plus_one)){ _throw( Null_Exception);}*
_temp900;}) =='s': 0){ break;}}} if( i == len){ if( aopt != 0){ args=({ struct
Cyc_List_List* _temp901=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp901->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp902=
aopt; if( _temp902 == 0){ _throw( Null_Exception);} _temp902->v;})); _temp901->tl=
args; _temp901;});} return Cyc_Absyn_primop_exp( p, args, 0);} args=({ struct
Cyc_List_List* _temp903= args; if( _temp903 == 0){ _throw( Null_Exception);}
_temp903->tl;}); typs=({ struct Cyc_List_List* _temp904= typs; if( _temp904 == 0){
_throw( Null_Exception);} _temp904->tl;});{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ char c=({ struct _tagged_string _temp991= fmt_str; char*
_temp993= _temp991.curr + i; if( _temp991.base == 0? 1:( _temp993 < _temp991.base?
1: _temp993 >= _temp991.last_plus_one)){ _throw( Null_Exception);}* _temp993;});
rev_fmt=({ struct Cyc_List_List* _temp905=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp905->hd=( void*)(( int) c); _temp905->tl=
rev_fmt; _temp905;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp906=( char*)"bad format string"; struct
_tagged_string _temp907; _temp907.curr= _temp906; _temp907.base= _temp906;
_temp907.last_plus_one= _temp906 + 18; _temp907;}));}{ struct _tuple9 _temp911;
int _temp912; char _temp914; struct Cyc_List_List* _temp916; struct Cyc_List_List*
_temp918; struct Cyc_List_List* _temp920; struct Cyc_List_List* _temp922; struct
_tuple9* _temp909=( struct _tuple9*)({ struct Cyc_Core_Opt* _temp908= x; if(
_temp908 == 0){ _throw( Null_Exception);} _temp908->v;}); _temp911=* _temp909;
_LL923: _temp922= _temp911.f1; goto _LL921; _LL921: _temp920= _temp911.f2; goto
_LL919; _LL919: _temp918= _temp911.f3; goto _LL917; _LL917: _temp916= _temp911.f4;
goto _LL915; _LL915: _temp914= _temp911.f5; goto _LL913; _LL913: _temp912=
_temp911.f6; goto _LL910; _LL910: i= _temp912 - 1; if( _temp914 !='s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp924=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp924->hd=( void*) _temp922; _temp924->tl=({ struct Cyc_List_List* _temp925=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp925->hd=(
void*) _temp920; _temp925->tl=({ struct Cyc_List_List* _temp926=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp926->hd=( void*) _temp918;
_temp926->tl=({ struct Cyc_List_List* _temp927=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp927->hd=( void*) _temp916;
_temp927->tl= 0; _temp927;}); _temp926;}); _temp925;}); _temp924;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp928=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp928->hd=( void*)(( int) _temp914); _temp928->tl=
rev_fmt; _temp928;});{ struct Cyc_List_List* _temp929= _temp920; struct Cyc_List_List
_temp936; struct Cyc_List_List* _temp937; int _temp939; _LL931: if( _temp929 ==
0){ goto _LL933;} else{ _temp936=* _temp929; _LL940: _temp939=( int) _temp936.hd;
goto _LL938; _LL938: _temp937=( struct Cyc_List_List*) _temp936.tl; if( _temp937
== 0){ goto _LL935;} else{ goto _LL933;}} _LL935: if( _temp939 =='*'){ goto
_LL932;} else{ goto _LL933;} _LL933: goto _LL934; _LL932: { struct _tuple0* temp=
Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp941=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp941->hd=( void*)({ struct
_tuple10* _temp942=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp942->f1= temp; _temp942->f2=( void*)({ struct Cyc_List_List* _temp944= typs;
if( _temp944 == 0){ _throw( Null_Exception);} _temp944->hd;}); _temp942->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp943= args; if( _temp943 == 0){
_throw( Null_Exception);} _temp943->hd;}); _temp942;}); _temp941->tl= rev_temps;
_temp941;}); rev_result=({ struct Cyc_List_List* _temp945=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp945->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp945->tl= rev_result; _temp945;}); args=({ struct Cyc_List_List*
_temp946= args; if( _temp946 == 0){ _throw( Null_Exception);} _temp946->tl;});
typs=({ struct Cyc_List_List* _temp947= typs; if( _temp947 == 0){ _throw(
Null_Exception);} _temp947->tl;}); goto _LL930;} _LL934: goto _LL930; _LL930:;}{
struct Cyc_List_List* _temp948= _temp918; struct Cyc_List_List _temp955; struct
Cyc_List_List* _temp956; struct Cyc_List_List _temp958; struct Cyc_List_List*
_temp959; int _temp961; int _temp963; _LL950: if( _temp948 == 0){ goto _LL952;}
else{ _temp955=* _temp948; _LL964: _temp963=( int) _temp955.hd; goto _LL957;
_LL957: _temp956=( struct Cyc_List_List*) _temp955.tl; if( _temp956 == 0){ goto
_LL952;} else{ _temp958=* _temp956; _LL962: _temp961=( int) _temp958.hd; goto
_LL960; _LL960: _temp959=( struct Cyc_List_List*) _temp958.tl; if( _temp959 == 0){
goto _LL954;} else{ goto _LL952;}}} _LL954: if( _temp963 =='.'? _temp961 =='*':
0){ goto _LL951;} else{ goto _LL952;} _LL952: goto _LL953; _LL951: { struct
_tuple0* temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp965=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp965->hd=(
void*)({ struct _tuple10* _temp966=( struct _tuple10*) GC_malloc( sizeof( struct
_tuple10)); _temp966->f1= temp; _temp966->f2=( void*)({ struct Cyc_List_List*
_temp968= typs; if( _temp968 == 0){ _throw( Null_Exception);} _temp968->hd;});
_temp966->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp967= args;
if( _temp967 == 0){ _throw( Null_Exception);} _temp967->hd;}); _temp966;});
_temp965->tl= rev_temps; _temp965;}); rev_result=({ struct Cyc_List_List*
_temp969=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp969->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp969->tl= rev_result;
_temp969;}); args=({ struct Cyc_List_List* _temp970= args; if( _temp970 == 0){
_throw( Null_Exception);} _temp970->tl;}); typs=({ struct Cyc_List_List*
_temp971= typs; if( _temp971 == 0){ _throw( Null_Exception);} _temp971->tl;});
goto _LL949;} _LL953: goto _LL949; _LL949:;} if( _temp914 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp972= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp972.last_plus_one - _temp972.curr, _temp972.curr);}));}{ struct _tuple0*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp973=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp973->hd=( void*)({
struct _tuple10* _temp974=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp974->f1= temp; _temp974->f2=( void*)({ struct Cyc_List_List* _temp976= typs;
if( _temp976 == 0){ _throw( Null_Exception);} _temp976->hd;}); _temp974->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp975= args; if( _temp975 == 0){
_throw( Null_Exception);} _temp975->hd;}); _temp974;}); _temp973->tl= rev_temps;
_temp973;}); rev_result=({ struct Cyc_List_List* _temp977=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp977->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp977->tl= rev_result; _temp977;}); args=({ struct Cyc_List_List*
_temp978= args; if( _temp978 == 0){ _throw( Null_Exception);} _temp978->tl;});
typs=({ struct Cyc_List_List* _temp979= typs; if( _temp979 == 0){ _throw(
Null_Exception);} _temp979->tl;});}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp980=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp980->hd=( void*)(( int)'.'); _temp980->tl= rev_fmt; _temp980;}); rev_fmt=({
struct Cyc_List_List* _temp981=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp981->hd=( void*)(( int)'*'); _temp981->tl= rev_fmt;
_temp981;}); rev_fmt=({ struct Cyc_List_List* _temp982=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp982->hd=( void*)(( int)'s');
_temp982->tl= rev_fmt; _temp982;});{ struct _tuple0* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp983=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp983->hd=( void*)({ struct _tuple10*
_temp984=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp984->f1=
temp; _temp984->f2=( void*)({ struct Cyc_List_List* _temp986= typs; if( _temp986
== 0){ _throw( Null_Exception);} _temp986->hd;}); _temp984->f3=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp985= args; if( _temp985 == 0){ _throw( Null_Exception);}
_temp985->hd;}); _temp984;}); _temp983->tl= rev_temps; _temp983;});{ struct Cyc_Absyn_Exp*
size_exp= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0), 0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List*
_temp987=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp987->hd=( void*) size_exp; _temp987->tl= rev_result; _temp987;});
rev_result=({ struct Cyc_List_List* _temp988=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp988->hd=( void*) cstring_exp; _temp988->tl=
rev_result; _temp988;}); args=({ struct Cyc_List_List* _temp989= args; if(
_temp989 == 0){ _throw( Null_Exception);} _temp989->tl;}); typs=({ struct Cyc_List_List*
_temp990= typs; if( _temp990 == 0){ _throw( Null_Exception);} _temp990->tl;});}}}}}}{
struct Cyc_List_List* es=({ struct Cyc_List_List* _temp1000=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1000->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp1000->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_result); _temp1000;}); if( aopt != 0){ es=({ struct Cyc_List_List*
_temp994=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp994->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp995=
aopt; if( _temp995 == 0){ _throw( Null_Exception);} _temp995->v;})); _temp994->tl=
es; _temp994;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p, es, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps != 0;
rev_temps=({ struct Cyc_List_List* _temp996= rev_temps; if( _temp996 == 0){
_throw( Null_Exception);} _temp996->tl;})){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)({ struct Cyc_List_List* _temp997= rev_temps; if( _temp997 == 0){
_throw( Null_Exception);} _temp997->hd;}))).f1,(*(( struct _tuple10*)({ struct
Cyc_List_List* _temp998= rev_temps; if( _temp998 == 0){ _throw( Null_Exception);}
_temp998->hd;}))).f2,( struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)({ struct Cyc_List_List*
_temp999= rev_temps; if( _temp999 == 0){ _throw( Null_Exception);} _temp999->hd;}))).f3,
s, 0);} return Cyc_Absyn_stmt_exp( s, 0);}}}}}} static void* Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1002=( char*)"Missing type in primop ";
struct _tagged_string _temp1003; _temp1003.curr= _temp1002; _temp1003.base=
_temp1002; _temp1003.last_plus_one= _temp1002 + 24; _temp1003;}));} return Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp1004= e->topt; if( _temp1004 == 0){ _throw(
Null_Exception);} _temp1004->v;}));} static void* Cyc_Toc_get_cyc_typ( struct
Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1005=( char*)"Missing type in primop ";
struct _tagged_string _temp1006; _temp1006.curr= _temp1005; _temp1006.base=
_temp1005; _temp1006.last_plus_one= _temp1005 + 24; _temp1006;}));} return( void*)({
struct Cyc_Core_Opt* _temp1007= e->topt; if( _temp1007 == 0){ _throw(
Null_Exception);} _temp1007->v;});} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1008=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1008->f1= Cyc_Toc_mt_tq; _temp1008->f2=
Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1009= e->topt; if(
_temp1009 == 0){ _throw( Null_Exception);} _temp1009->v;})); _temp1008;});}
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e);
static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp1010=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1010[ 0]=({
struct _tuple4 _temp1011; _temp1011.f1= 0; _temp1011.f2= e; _temp1011;});
_temp1010;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
se= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic( se): Cyc_Toc_malloc_ptr(
se), 0);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1012= rgnopt; if( _temp1012 == 0){ _throw( Null_Exception);} _temp1012;});
Cyc_Toc_exp_to_c( nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, se), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles0); for( 0; dles != 0; dles=({ struct Cyc_List_List*
_temp1013= dles; if( _temp1013 == 0){ _throw( Null_Exception);} _temp1013->tl;})){
struct _tuple4 _temp1017; struct Cyc_Absyn_Exp* _temp1018; struct Cyc_List_List*
_temp1020; struct _tuple4* _temp1015=( struct _tuple4*)({ struct Cyc_List_List*
_temp1014= dles; if( _temp1014 == 0){ _throw( Null_Exception);} _temp1014->hd;});
_temp1017=* _temp1015; _LL1021: _temp1020= _temp1017.f1; goto _LL1019; _LL1019:
_temp1018= _temp1017.f2; goto _LL1016; _LL1016: { struct Cyc_Absyn_Exp* e_index;
if( _temp1020 == 0){ e_index= Cyc_Absyn_signed_int_exp(( count --), 0);} else{
if(({ struct Cyc_List_List* _temp1022= _temp1020; if( _temp1022 == 0){ _throw(
Null_Exception);} _temp1022->tl;}) != 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1023=( char*)"multiple designators in array";
struct _tagged_string _temp1024; _temp1024.curr= _temp1023; _temp1024.base=
_temp1023; _temp1024.last_plus_one= _temp1023 + 30; _temp1024;}));}{ void* d=(
void*)({ struct Cyc_List_List* _temp1037= _temp1020; if( _temp1037 == 0){ _throw(
Null_Exception);} _temp1037->hd;}); void* _temp1025= d; struct Cyc_Absyn_Exp*
_temp1031; struct _tagged_string* _temp1033; _LL1027: if(*(( int*) _temp1025) ==
Cyc_Absyn_ArrayElement){ _LL1032: _temp1031=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayElement_struct*) _temp1025)->f1; goto _LL1028;} else{ goto
_LL1029;} _LL1029: if(*(( int*) _temp1025) == Cyc_Absyn_FieldName){ _LL1034:
_temp1033=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1025)->f1; goto _LL1030;} else{ goto _LL1026;} _LL1028: Cyc_Toc_exp_to_c(
nv, _temp1031); e_index= _temp1031; goto _LL1026; _LL1030: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1035=( char*)"field name designators in array"; struct _tagged_string
_temp1036; _temp1036.curr= _temp1035; _temp1036.base= _temp1035; _temp1036.last_plus_one=
_temp1035 + 32; _temp1036;})); goto _LL1026; _LL1026:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp1038=( void*)
_temp1018->r; struct Cyc_List_List* _temp1046; struct Cyc_Absyn_Exp* _temp1048;
struct Cyc_Absyn_Exp* _temp1050; struct Cyc_Absyn_Vardecl* _temp1052; _LL1040:
if(*(( int*) _temp1038) == Cyc_Absyn_Array_e){ _LL1047: _temp1046=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1038)->f1; goto _LL1041;} else{ goto
_LL1042;} _LL1042: if(*(( int*) _temp1038) == Cyc_Absyn_Comprehension_e){
_LL1053: _temp1052=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1038)->f1; goto _LL1051; _LL1051: _temp1050=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1038)->f2; goto _LL1049; _LL1049:
_temp1048=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1038)->f3; goto _LL1043;} else{ goto _LL1044;} _LL1044: goto _LL1045;
_LL1041: s= Cyc_Toc_init_array( nv, lval, _temp1046, s); goto _LL1039; _LL1043:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1052, _temp1050, _temp1048, s, 0);
goto _LL1039; _LL1045: Cyc_Toc_exp_to_c( nv, _temp1018); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp1018, 0),
s, 0); goto _LL1039; _LL1039:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* x1= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp( x1,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1070=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1070[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1071; _temp1071.tag= Cyc_Absyn_Local_b;
_temp1071.f1= vd; _temp1071;}); _temp1070;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_var_exp( max, 0), 0);
struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( x1, 0), 0);
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( x1,
0), 0); struct Cyc_Absyn_Stmt* body;{ void* _temp1054=( void*) e2->r; struct Cyc_List_List*
_temp1062; struct Cyc_Absyn_Exp* _temp1064; struct Cyc_Absyn_Exp* _temp1066;
struct Cyc_Absyn_Vardecl* _temp1068; _LL1056: if(*(( int*) _temp1054) == Cyc_Absyn_Array_e){
_LL1063: _temp1062=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1054)->f1; goto _LL1057;} else{ goto _LL1058;} _LL1058: if(*(( int*)
_temp1054) == Cyc_Absyn_Comprehension_e){ _LL1069: _temp1068=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1054)->f1; goto _LL1067; _LL1067:
_temp1066=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1054)->f2; goto _LL1065; _LL1065: _temp1064=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1054)->f3; goto _LL1059;} else{
goto _LL1060;} _LL1060: goto _LL1061; _LL1057: body= Cyc_Toc_init_array( nv2,
lval, _temp1062, Cyc_Absyn_skip_stmt( 0)); goto _LL1055; _LL1059: body= Cyc_Toc_init_comprehension(
nv2, lval, _temp1068, _temp1066, _temp1064, Cyc_Absyn_skip_stmt( 0), 0); goto
_LL1055; _LL1061: Cyc_Toc_exp_to_c( nv2, e2); body= Cyc_Absyn_assign_stmt( lval,
e2, 0); goto _LL1055; _LL1055:;}{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt(
ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( x1, Cyc_Absyn_uint_t, 0, s2,
0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple(
struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
es){ struct Cyc_List_List* tqs=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
es); struct _tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct _tuple0* x=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct
Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(* lval_f)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( es);{
int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es); for( 0; es !=
0; es=({ struct Cyc_List_List* _temp1072= es; if( _temp1072 == 0){ _throw(
Null_Exception);} _temp1072->tl;}), -- i){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1092= es; if( _temp1092 == 0){ _throw( Null_Exception);}
_temp1092->hd;}); struct Cyc_Absyn_Exp* lval= lval_f( xexp,({ struct
_tagged_string* _temp1091=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1091[ 0]= xprintf("f%d", i); _temp1091;}), 0);
is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)({ struct Cyc_Core_Opt*
_temp1073= e->topt; if( _temp1073 == 0){ _throw( Null_Exception);} _temp1073->v;})):
0;{ void* _temp1074=( void*) e->r; struct Cyc_List_List* _temp1082; struct Cyc_Absyn_Exp*
_temp1084; struct Cyc_Absyn_Exp* _temp1086; struct Cyc_Absyn_Vardecl* _temp1088;
_LL1076: if(*(( int*) _temp1074) == Cyc_Absyn_Array_e){ _LL1083: _temp1082=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1074)->f1; goto
_LL1077;} else{ goto _LL1078;} _LL1078: if(*(( int*) _temp1074) == Cyc_Absyn_Comprehension_e){
_LL1089: _temp1088=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1074)->f1; goto _LL1087; _LL1087: _temp1086=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1074)->f2; goto _LL1085; _LL1085:
_temp1084=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1074)->f3; goto _LL1079;} else{ goto _LL1080;} _LL1080: goto _LL1081;
_LL1077: s= Cyc_Toc_init_array( nv, lval, _temp1082, s); goto _LL1075; _LL1079:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1088, _temp1086, _temp1084, s, 0);
goto _LL1075; _LL1081: Cyc_Toc_exp_to_c( nv, e); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval_f( xexp,({ struct _tagged_string* _temp1090=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp1090[ 0]=
xprintf("f%d", i); _temp1090;}), 0), e, 0), 0), s, 0); goto _LL1075; _LL1075:;}}}
return Cyc_Toc_make_struct( nv, x, Cyc_Absyn_strct( n), s, pointer, rgnopt,
is_atomic);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Exp*(* lval_f)( struct Cyc_Absyn_Exp*, struct
_tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1;{ struct Cyc_List_List* es=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( dles); for( 0;
es != 0; es=({ struct Cyc_List_List* _temp1093= es; if( _temp1093 == 0){ _throw(
Null_Exception);} _temp1093->tl;})){ struct _tuple4 _temp1097; struct Cyc_Absyn_Exp*
_temp1098; struct Cyc_List_List* _temp1100; struct _tuple4* _temp1095=( struct
_tuple4*)({ struct Cyc_List_List* _temp1094= es; if( _temp1094 == 0){ _throw(
Null_Exception);} _temp1094->hd;}); _temp1097=* _temp1095; _LL1101: _temp1100=
_temp1097.f1; goto _LL1099; _LL1099: _temp1098= _temp1097.f2; goto _LL1096;
_LL1096: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)({ struct Cyc_Core_Opt*
_temp1102= _temp1098->topt; if( _temp1102 == 0){ _throw( Null_Exception);}
_temp1102->v;})): 0; if( _temp1100 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1103=( char*)"empty designator list";
struct _tagged_string _temp1104; _temp1104.curr= _temp1103; _temp1104.base=
_temp1103; _temp1104.last_plus_one= _temp1103 + 22; _temp1104;}));} if(({ struct
Cyc_List_List* _temp1105= _temp1100; if( _temp1105 == 0){ _throw( Null_Exception);}
_temp1105->tl;}) != 0){ struct _tuple0* y= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
yexp= Cyc_Absyn_var_exp( y, 0); for( 0; _temp1100 != 0; _temp1100=({ struct Cyc_List_List*
_temp1106= _temp1100; if( _temp1106 == 0){ _throw( Null_Exception);} _temp1106->tl;})){
void* _temp1108=( void*)({ struct Cyc_List_List* _temp1107= _temp1100; if(
_temp1107 == 0){ _throw( Null_Exception);} _temp1107->hd;}); struct
_tagged_string* _temp1114; _LL1110: if(*(( int*) _temp1108) == Cyc_Absyn_FieldName){
_LL1115: _temp1114=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1108)->f1; goto _LL1111;} else{ goto _LL1112;} _LL1112: goto _LL1113;
_LL1111: if( Cyc_Toc_is_poly_field( struct_type, _temp1114)){ yexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), yexp, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval_f( xexp, _temp1114, 0), yexp, 0), 0), s, 0); goto
_LL1109; _LL1113:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1116=( char*)"array designator in struct";
struct _tagged_string _temp1117; _temp1117.curr= _temp1116; _temp1117.base=
_temp1116; _temp1117.last_plus_one= _temp1116 + 27; _temp1117;}))); _LL1109:;}
Cyc_Toc_exp_to_c( nv, _temp1098); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
yexp, _temp1098, 0), 0), s, 0);} else{ void* _temp1119=( void*)({ struct Cyc_List_List*
_temp1118= _temp1100; if( _temp1118 == 0){ _throw( Null_Exception);} _temp1118->hd;});
struct _tagged_string* _temp1125; _LL1121: if(*(( int*) _temp1119) == Cyc_Absyn_FieldName){
_LL1126: _temp1125=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1119)->f1; goto _LL1122;} else{ goto _LL1123;} _LL1123: goto _LL1124;
_LL1122: { struct Cyc_Absyn_Exp* lval= lval_f( xexp, _temp1125, 0);{ void*
_temp1127=( void*) _temp1098->r; struct Cyc_List_List* _temp1135; struct Cyc_Absyn_Exp*
_temp1137; struct Cyc_Absyn_Exp* _temp1139; struct Cyc_Absyn_Vardecl* _temp1141;
_LL1129: if(*(( int*) _temp1127) == Cyc_Absyn_Array_e){ _LL1136: _temp1135=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1127)->f1; goto
_LL1130;} else{ goto _LL1131;} _LL1131: if(*(( int*) _temp1127) == Cyc_Absyn_Comprehension_e){
_LL1142: _temp1141=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1127)->f1; goto _LL1140; _LL1140: _temp1139=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1127)->f2; goto _LL1138; _LL1138:
_temp1137=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1127)->f3; goto _LL1132;} else{ goto _LL1133;} _LL1133: goto _LL1134;
_LL1130: s= Cyc_Toc_init_array( nv, lval, _temp1135, s); goto _LL1128; _LL1132:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1141, _temp1139, _temp1137, s, 0);
goto _LL1128; _LL1134: Cyc_Toc_exp_to_c( nv, _temp1098); if( Cyc_Toc_is_poly_field(
struct_type, _temp1125)){ _temp1098= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1098, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1098, 0), 0), s, 0); goto _LL1128; _LL1128:;} goto _LL1120;} _LL1124:(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1143=( char*)"array designator in struct"; struct
_tagged_string _temp1144; _temp1144.curr= _temp1143; _temp1144.base= _temp1143;
_temp1144.last_plus_one= _temp1143 + 27; _temp1144;}))); _LL1120:;}}} return Cyc_Toc_make_struct(
nv, x, Cyc_Absyn_strctq( tdn), s, pointer, rgnopt, is_atomic);} struct _tuple11{
struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return
Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp(
e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp*
e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp1145=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp1145[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp1146; _temp1146.tag=
Cyc_Absyn_Increment_e; _temp1146.f1= e1; _temp1146.f2=( void*) incr; _temp1146;});
_temp1145;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1147=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp1157; struct Cyc_Absyn_Exp* _temp1159; void* _temp1161; struct
_tagged_string* _temp1163; struct Cyc_Absyn_Exp* _temp1165; _LL1149: if(*(( int*)
_temp1147) == Cyc_Absyn_StmtExp_e){ _LL1158: _temp1157=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1147)->f1; goto _LL1150;} else{ goto
_LL1151;} _LL1151: if(*(( int*) _temp1147) == Cyc_Absyn_Cast_e){ _LL1162:
_temp1161=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1147)->f1; goto
_LL1160; _LL1160: _temp1159=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1147)->f2; goto _LL1152;} else{ goto _LL1153;} _LL1153: if(*(( int*)
_temp1147) == Cyc_Absyn_StructMember_e){ _LL1166: _temp1165=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1147)->f1; goto _LL1164; _LL1164:
_temp1163=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1147)->f2; goto _LL1154;} else{ goto _LL1155;} _LL1155: goto _LL1156;
_LL1150:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1157, fs, f, f_env); goto _LL1148; _LL1152:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1159, fs, f, f_env); goto
_LL1148; _LL1154:( void*)( e1->r=( void*)(( void*) _temp1165->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1167=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1167->hd=( void*) _temp1163; _temp1167->tl= fs;
_temp1167;}), f, f_env); goto _LL1148; _LL1156: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1168= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1168.last_plus_one - _temp1168.curr, _temp1168.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp1169= fs; if( _temp1169 == 0){ _throw( Null_Exception);} _temp1169->tl;})){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_string*)({ struct
Cyc_List_List* _temp1170= fs; if( _temp1170 == 0){ _throw( Null_Exception);}
_temp1170->hd;}), e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy,
f_env))->r)); goto _LL1148;} _LL1148:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp1171=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1181; struct Cyc_Absyn_Stmt* _temp1183; struct Cyc_Absyn_Decl*
_temp1185; struct Cyc_Absyn_Stmt* _temp1187; struct Cyc_Absyn_Stmt* _temp1189;
_LL1173: if(( unsigned int) _temp1171 > 1u?*(( int*) _temp1171) == Cyc_Absyn_Exp_s:
0){ _LL1182: _temp1181=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1171)->f1; goto _LL1174;} else{ goto _LL1175;} _LL1175: if(( unsigned int)
_temp1171 > 1u?*(( int*) _temp1171) == Cyc_Absyn_Decl_s: 0){ _LL1186: _temp1185=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1171)->f1; goto
_LL1184; _LL1184: _temp1183=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1171)->f2; goto _LL1176;} else{ goto _LL1177;} _LL1177: if(( unsigned int)
_temp1171 > 1u?*(( int*) _temp1171) == Cyc_Absyn_Seq_s: 0){ _LL1190: _temp1189=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1171)->f1; goto
_LL1188; _LL1188: _temp1187=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1171)->f2; goto _LL1178;} else{ goto _LL1179;} _LL1179: goto _LL1180;
_LL1174:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1181, fs, f, f_env); goto _LL1172; _LL1176:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1183, fs, f, f_env);
goto _LL1172; _LL1178:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1187, fs, f, f_env); goto _LL1172; _LL1180:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1191= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1191.last_plus_one - _temp1191.curr,
_temp1191.curr);})); goto _LL1172; _LL1172:;} static struct Cyc_Absyn_Exp* Cyc_Toc_known_size_subscript(
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
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1192=
Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s", _temp1192.last_plus_one
- _temp1192.curr, _temp1192.curr);})); return;}{ void* old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1868= e->topt; if( _temp1868 == 0){ _throw( Null_Exception);}
_temp1868->v;}); void* _temp1193= r; void* _temp1267; void* _temp1269; void*
_temp1271; struct _tuple0* _temp1273; struct _tuple0* _temp1275; struct Cyc_List_List*
_temp1277; void* _temp1279; void* _temp1281; struct Cyc_Absyn_Exp* _temp1283;
struct Cyc_Absyn_Exp* _temp1285; struct Cyc_Core_Opt* _temp1287; struct Cyc_Absyn_Exp*
_temp1289; struct Cyc_Absyn_Exp* _temp1291; struct Cyc_Absyn_Exp* _temp1293;
struct Cyc_Absyn_Exp* _temp1295; struct Cyc_Absyn_Exp* _temp1297; struct Cyc_Absyn_Exp*
_temp1299; struct Cyc_List_List* _temp1301; struct Cyc_Absyn_Exp* _temp1303;
struct Cyc_List_List* _temp1305; struct Cyc_Absyn_Exp* _temp1307; struct Cyc_Absyn_Exp*
_temp1309; struct Cyc_Absyn_Exp* _temp1311; struct Cyc_List_List* _temp1313;
struct Cyc_Absyn_Exp* _temp1315; struct Cyc_Absyn_Exp* _temp1317; struct Cyc_Absyn_Exp**
_temp1319; void* _temp1320; void** _temp1322; struct Cyc_Absyn_Exp* _temp1323;
struct Cyc_Absyn_Exp* _temp1325; struct Cyc_Absyn_Exp* _temp1327; void*
_temp1329; struct Cyc_Absyn_Exp* _temp1331; struct Cyc_Absyn_Exp* _temp1333;
struct _tagged_string* _temp1335; struct Cyc_Absyn_Exp* _temp1337; struct
_tagged_string* _temp1339; struct Cyc_Absyn_Exp* _temp1341; struct Cyc_Absyn_Exp*
_temp1343; struct Cyc_Absyn_Exp* _temp1345; struct Cyc_List_List* _temp1347;
struct Cyc_List_List* _temp1349; struct _tuple1* _temp1351; struct Cyc_List_List*
_temp1353; struct Cyc_Absyn_Exp* _temp1355; struct Cyc_Absyn_Exp* _temp1357;
struct Cyc_Absyn_Vardecl* _temp1359; struct Cyc_Absyn_Structdecl* _temp1361;
struct Cyc_List_List* _temp1363; struct Cyc_Core_Opt* _temp1365; struct _tuple0*
_temp1367; struct Cyc_Absyn_Tunionfield* _temp1369; struct Cyc_Absyn_Tuniondecl*
_temp1371; struct Cyc_List_List* _temp1373; struct Cyc_Core_Opt* _temp1375;
struct Cyc_Core_Opt* _temp1377; struct Cyc_Absyn_Tunionfield* _temp1379; struct
Cyc_Absyn_Tuniondecl* _temp1381; struct Cyc_List_List* _temp1383; struct Cyc_Core_Opt*
_temp1385; struct Cyc_Core_Opt* _temp1387; struct Cyc_Absyn_Enumfield* _temp1389;
struct Cyc_Absyn_Enumdecl* _temp1391; struct _tuple0* _temp1393; void* _temp1395;
struct Cyc_Absyn_Exp* _temp1397; struct Cyc_Absyn_Stmt* _temp1399; struct Cyc_List_List*
_temp1401; struct Cyc_Core_Opt* _temp1403; struct Cyc_Absyn_Fndecl* _temp1405;
struct Cyc_Absyn_Exp* _temp1407; _LL1195: if(*(( int*) _temp1193) == Cyc_Absyn_Const_e){
_LL1268: _temp1267=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1193)->f1;
if( _temp1267 ==( void*) Cyc_Absyn_Null_c){ goto _LL1196;} else{ goto _LL1197;}}
else{ goto _LL1197;} _LL1197: if(*(( int*) _temp1193) == Cyc_Absyn_Const_e){
_LL1270: _temp1269=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1193)->f1;
goto _LL1198;} else{ goto _LL1199;} _LL1199: if(*(( int*) _temp1193) == Cyc_Absyn_Var_e){
_LL1274: _temp1273=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1193)->f1; goto _LL1272; _LL1272: _temp1271=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1193)->f2; goto _LL1200;} else{ goto _LL1201;} _LL1201: if(*(( int*)
_temp1193) == Cyc_Absyn_UnknownId_e){ _LL1276: _temp1275=( struct _tuple0*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1193)->f1; goto _LL1202;} else{ goto
_LL1203;} _LL1203: if(*(( int*) _temp1193) == Cyc_Absyn_Primop_e){ _LL1280:
_temp1279=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1193)->f1; goto
_LL1278; _LL1278: _temp1277=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1193)->f2; goto _LL1204;} else{ goto _LL1205;} _LL1205: if(*(( int*)
_temp1193) == Cyc_Absyn_Increment_e){ _LL1284: _temp1283=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp1193)->f1; goto _LL1282; _LL1282:
_temp1281=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1193)->f2; goto
_LL1206;} else{ goto _LL1207;} _LL1207: if(*(( int*) _temp1193) == Cyc_Absyn_AssignOp_e){
_LL1290: _temp1289=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1193)->f1; goto _LL1288; _LL1288: _temp1287=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1193)->f2; goto _LL1286; _LL1286:
_temp1285=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1193)->f3; goto _LL1208;} else{ goto _LL1209;} _LL1209: if(*(( int*)
_temp1193) == Cyc_Absyn_Conditional_e){ _LL1296: _temp1295=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1193)->f1; goto _LL1294; _LL1294:
_temp1293=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1193)->f2; goto _LL1292; _LL1292: _temp1291=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1193)->f3; goto _LL1210;} else{
goto _LL1211;} _LL1211: if(*(( int*) _temp1193) == Cyc_Absyn_SeqExp_e){ _LL1300:
_temp1299=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1193)->f1; goto _LL1298; _LL1298: _temp1297=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1193)->f2; goto _LL1212;} else{ goto
_LL1213;} _LL1213: if(*(( int*) _temp1193) == Cyc_Absyn_UnknownCall_e){ _LL1304:
_temp1303=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1193)->f1; goto _LL1302; _LL1302: _temp1301=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1193)->f2; goto _LL1214;} else{
goto _LL1215;} _LL1215: if(*(( int*) _temp1193) == Cyc_Absyn_FnCall_e){ _LL1308:
_temp1307=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1193)->f1; goto _LL1306; _LL1306: _temp1305=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1193)->f2; goto _LL1216;} else{ goto
_LL1217;} _LL1217: if(*(( int*) _temp1193) == Cyc_Absyn_Throw_e){ _LL1310:
_temp1309=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1193)->f1;
goto _LL1218;} else{ goto _LL1219;} _LL1219: if(*(( int*) _temp1193) == Cyc_Absyn_NoInstantiate_e){
_LL1312: _temp1311=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1193)->f1; goto _LL1220;} else{ goto _LL1221;} _LL1221: if(*(( int*)
_temp1193) == Cyc_Absyn_Instantiate_e){ _LL1316: _temp1315=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1193)->f1; goto _LL1314; _LL1314:
_temp1313=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1193)->f2; goto _LL1222;} else{ goto _LL1223;} _LL1223: if(*(( int*)
_temp1193) == Cyc_Absyn_Cast_e){ _LL1321: _temp1320=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1193)->f1; _temp1322=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1193)->f1;
goto _LL1318; _LL1318: _temp1317=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1193)->f2; _temp1319=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1193)->f2;
goto _LL1224;} else{ goto _LL1225;} _LL1225: if(*(( int*) _temp1193) == Cyc_Absyn_Address_e){
_LL1324: _temp1323=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp1193)->f1; goto _LL1226;} else{ goto _LL1227;} _LL1227: if(*(( int*)
_temp1193) == Cyc_Absyn_New_e){ _LL1328: _temp1327=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1193)->f1; goto _LL1326; _LL1326: _temp1325=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1193)->f2; goto
_LL1228;} else{ goto _LL1229;} _LL1229: if(*(( int*) _temp1193) == Cyc_Absyn_Sizeoftyp_e){
_LL1330: _temp1329=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1193)->f1;
goto _LL1230;} else{ goto _LL1231;} _LL1231: if(*(( int*) _temp1193) == Cyc_Absyn_Sizeofexp_e){
_LL1332: _temp1331=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1193)->f1; goto _LL1232;} else{ goto _LL1233;} _LL1233: if(*(( int*)
_temp1193) == Cyc_Absyn_Deref_e){ _LL1334: _temp1333=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp1193)->f1; goto _LL1234;} else{ goto
_LL1235;} _LL1235: if(*(( int*) _temp1193) == Cyc_Absyn_StructMember_e){ _LL1338:
_temp1337=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1193)->f1; goto _LL1336; _LL1336: _temp1335=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1193)->f2; goto _LL1236;} else{
goto _LL1237;} _LL1237: if(*(( int*) _temp1193) == Cyc_Absyn_StructArrow_e){
_LL1342: _temp1341=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1193)->f1; goto _LL1340; _LL1340: _temp1339=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1193)->f2; goto _LL1238;} else{
goto _LL1239;} _LL1239: if(*(( int*) _temp1193) == Cyc_Absyn_Subscript_e){
_LL1346: _temp1345=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1193)->f1; goto _LL1344; _LL1344: _temp1343=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1193)->f2; goto _LL1240;} else{ goto
_LL1241;} _LL1241: if(*(( int*) _temp1193) == Cyc_Absyn_Tuple_e){ _LL1348:
_temp1347=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1193)->f1;
goto _LL1242;} else{ goto _LL1243;} _LL1243: if(*(( int*) _temp1193) == Cyc_Absyn_CompoundLit_e){
_LL1352: _temp1351=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1193)->f1; goto _LL1350; _LL1350: _temp1349=( struct Cyc_List_List*)((
struct Cyc_Absyn_CompoundLit_e_struct*) _temp1193)->f2; goto _LL1244;} else{
goto _LL1245;} _LL1245: if(*(( int*) _temp1193) == Cyc_Absyn_Array_e){ _LL1354:
_temp1353=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1193)->f1;
goto _LL1246;} else{ goto _LL1247;} _LL1247: if(*(( int*) _temp1193) == Cyc_Absyn_Comprehension_e){
_LL1360: _temp1359=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1193)->f1; goto _LL1358; _LL1358: _temp1357=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1193)->f2; goto _LL1356; _LL1356:
_temp1355=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1193)->f3; goto _LL1248;} else{ goto _LL1249;} _LL1249: if(*(( int*)
_temp1193) == Cyc_Absyn_Struct_e){ _LL1368: _temp1367=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1193)->f1; goto _LL1366; _LL1366:
_temp1365=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1193)->f2;
goto _LL1364; _LL1364: _temp1363=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1193)->f3; goto _LL1362; _LL1362: _temp1361=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1193)->f4; goto _LL1250;} else{ goto
_LL1251;} _LL1251: if(*(( int*) _temp1193) == Cyc_Absyn_Tunion_e){ _LL1378:
_temp1377=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1193)->f1;
goto _LL1376; _LL1376: _temp1375=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1193)->f2; goto _LL1374; _LL1374: _temp1373=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1193)->f3; if( _temp1373 == 0){ goto
_LL1372;} else{ goto _LL1253;} _LL1372: _temp1371=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1193)->f4; goto _LL1370; _LL1370:
_temp1369=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1193)->f5; goto _LL1252;} else{ goto _LL1253;} _LL1253: if(*(( int*)
_temp1193) == Cyc_Absyn_Tunion_e){ _LL1388: _temp1387=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1193)->f1; goto _LL1386; _LL1386:
_temp1385=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1193)->f2;
goto _LL1384; _LL1384: _temp1383=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1193)->f3; goto _LL1382; _LL1382: _temp1381=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1193)->f4; goto _LL1380; _LL1380:
_temp1379=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1193)->f5; goto _LL1254;} else{ goto _LL1255;} _LL1255: if(*(( int*)
_temp1193) == Cyc_Absyn_Enum_e){ _LL1394: _temp1393=( struct _tuple0*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp1193)->f1; goto _LL1392; _LL1392: _temp1391=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1193)->f2;
goto _LL1390; _LL1390: _temp1389=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1193)->f3; goto _LL1256;} else{ goto _LL1257;} _LL1257: if(*(( int*)
_temp1193) == Cyc_Absyn_Malloc_e){ _LL1398: _temp1397=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Malloc_e_struct*) _temp1193)->f1; goto _LL1396; _LL1396:
_temp1395=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp1193)->f2; goto
_LL1258;} else{ goto _LL1259;} _LL1259: if(*(( int*) _temp1193) == Cyc_Absyn_StmtExp_e){
_LL1400: _temp1399=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1193)->f1; goto _LL1260;} else{ goto _LL1261;} _LL1261: if(*(( int*)
_temp1193) == Cyc_Absyn_UnresolvedMem_e){ _LL1404: _temp1403=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1193)->f1; goto _LL1402; _LL1402:
_temp1401=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1193)->f2; goto _LL1262;} else{ goto _LL1263;} _LL1263: if(*(( int*)
_temp1193) == Cyc_Absyn_Codegen_e){ _LL1406: _temp1405=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp1193)->f1; goto _LL1264;} else{ goto
_LL1265;} _LL1265: if(*(( int*) _temp1193) == Cyc_Absyn_Fill_e){ _LL1408:
_temp1407=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1193)->f1;
goto _LL1266;} else{ goto _LL1194;} _LL1196:{ void* _temp1409= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1415; struct Cyc_Absyn_Conref* _temp1417;
struct Cyc_Absyn_Tqual _temp1419; struct Cyc_Absyn_Conref* _temp1421; void*
_temp1423; void* _temp1425; _LL1411: if(( unsigned int) _temp1409 > 4u?*(( int*)
_temp1409) == Cyc_Absyn_PointerType: 0){ _LL1416: _temp1415=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1409)->f1; _LL1426: _temp1425=( void*)
_temp1415.elt_typ; goto _LL1424; _LL1424: _temp1423=( void*) _temp1415.rgn_typ;
goto _LL1422; _LL1422: _temp1421=( struct Cyc_Absyn_Conref*) _temp1415.nullable;
goto _LL1420; _LL1420: _temp1419=( struct Cyc_Absyn_Tqual) _temp1415.tq; goto
_LL1418; _LL1418: _temp1417=( struct Cyc_Absyn_Conref*) _temp1415.bounds; goto
_LL1412;} else{ goto _LL1413;} _LL1413: goto _LL1414; _LL1412: if(( void*)
_temp1417->v ==( void*) Cyc_Absyn_No_constr){( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto _LL1410;}{ void* _temp1427=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1417); struct Cyc_Absyn_Exp*
_temp1433; _LL1429: if( _temp1427 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1430;}
else{ goto _LL1431;} _LL1431: if(( unsigned int) _temp1427 > 1u?*(( int*)
_temp1427) == Cyc_Absyn_Upper_b: 0){ _LL1434: _temp1433=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1427)->f1; goto _LL1432;} else{ goto
_LL1428;} _LL1430: { struct _tuple4* zero=({ struct _tuple4* _temp1438=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1438->f1= 0; _temp1438->f2=
Cyc_Absyn_signed_int_exp( 0, 0); _temp1438;}); struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp1435=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1435->hd=( void*) zero; _temp1435->tl=({ struct Cyc_List_List*
_temp1436=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1436->hd=( void*) zero; _temp1436->tl=({ struct Cyc_List_List* _temp1437=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1437->hd=(
void*) zero; _temp1437->tl= 0; _temp1437;}); _temp1436;}); _temp1435;}), 0); if(
nv->toplevel){( void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct(
Cyc_Toc_add_tagged_ptr_type( Cyc_Toc_typ_to_c( _temp1425), Cyc_Toc_mt_tq));
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*)
um, Cyc_Absyn_exp_stmt( xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));} goto _LL1428;} _LL1432:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1428; _LL1428:;} goto _LL1410; _LL1414:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1439= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1439.last_plus_one -
_temp1439.curr, _temp1439.curr);})); goto _LL1410; _LL1410:;} goto _LL1194;
_LL1198: goto _LL1194; _LL1200:{ struct _handler_cons _temp1440; _push_handler(&
_temp1440);{ void* _temp1441=( void*) setjmp( _temp1440.handler); if( !
_temp1441){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap, _temp1273))->r));;
_pop_handler();} else{ void* _temp1443= _temp1441; _LL1445: if( _temp1443 == Cyc_Dict_Absent){
goto _LL1446;} else{ goto _LL1447;} _LL1447: goto _LL1448; _LL1446:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1449=
Cyc_Absynpp_qvar2string( _temp1273); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1449.last_plus_one - _temp1449.curr, _temp1449.curr);})); return; _LL1448:(
void) _throw( _temp1443); _LL1444:;}}} goto _LL1194; _LL1202:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1450=(
char*)"unknownid"; struct _tagged_string _temp1451; _temp1451.curr= _temp1450;
_temp1451.base= _temp1450; _temp1451.last_plus_one= _temp1450 + 10; _temp1451;}));
goto _LL1194; _LL1204: { struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1277); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1277);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1277);{ void* _temp1452= _temp1279; _LL1454: if( _temp1452 ==( void*)
Cyc_Absyn_Size){ goto _LL1455;} else{ goto _LL1456;} _LL1456: if( _temp1452 ==(
void*) Cyc_Absyn_Printf){ goto _LL1457;} else{ goto _LL1458;} _LL1458: if(
_temp1452 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1459;} else{ goto _LL1460;}
_LL1460: if( _temp1452 ==( void*) Cyc_Absyn_Fprintf){ goto _LL1461;} else{ goto
_LL1462;} _LL1462: if( _temp1452 ==( void*) Cyc_Absyn_Sscanf){ goto _LL1463;}
else{ goto _LL1464;} _LL1464: if( _temp1452 ==( void*) Cyc_Absyn_Plus){ goto
_LL1465;} else{ goto _LL1466;} _LL1466: if( _temp1452 ==( void*) Cyc_Absyn_Minus){
goto _LL1467;} else{ goto _LL1468;} _LL1468: if( _temp1452 ==( void*) Cyc_Absyn_Eq){
goto _LL1469;} else{ goto _LL1470;} _LL1470: if( _temp1452 ==( void*) Cyc_Absyn_Neq){
goto _LL1471;} else{ goto _LL1472;} _LL1472: if( _temp1452 ==( void*) Cyc_Absyn_Gt){
goto _LL1473;} else{ goto _LL1474;} _LL1474: if( _temp1452 ==( void*) Cyc_Absyn_Gte){
goto _LL1475;} else{ goto _LL1476;} _LL1476: if( _temp1452 ==( void*) Cyc_Absyn_Lt){
goto _LL1477;} else{ goto _LL1478;} _LL1478: if( _temp1452 ==( void*) Cyc_Absyn_Lte){
goto _LL1479;} else{ goto _LL1480;} _LL1480: goto _LL1481; _LL1455: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1515= _temp1277; if(
_temp1515 == 0){ _throw( Null_Exception);} _temp1515->hd;});{ void* _temp1483=(
void*)({ struct Cyc_Core_Opt* _temp1482= arg->topt; if( _temp1482 == 0){ _throw(
Null_Exception);} _temp1482->v;}); struct Cyc_Absyn_Exp* _temp1491; struct Cyc_Absyn_Tqual
_temp1493; void* _temp1495; struct Cyc_Absyn_PtrInfo _temp1497; struct Cyc_Absyn_Conref*
_temp1499; struct Cyc_Absyn_Tqual _temp1501; struct Cyc_Absyn_Conref* _temp1503;
void* _temp1505; void* _temp1507; _LL1485: if(( unsigned int) _temp1483 > 4u?*((
int*) _temp1483) == Cyc_Absyn_ArrayType: 0){ _LL1496: _temp1495=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1483)->f1; goto _LL1494; _LL1494:
_temp1493=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1483)->f2; goto _LL1492; _LL1492: _temp1491=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1483)->f3; goto _LL1486;} else{ goto
_LL1487;} _LL1487: if(( unsigned int) _temp1483 > 4u?*(( int*) _temp1483) == Cyc_Absyn_PointerType:
0){ _LL1498: _temp1497=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1483)->f1; _LL1508: _temp1507=( void*) _temp1497.elt_typ; goto _LL1506;
_LL1506: _temp1505=( void*) _temp1497.rgn_typ; goto _LL1504; _LL1504: _temp1503=(
struct Cyc_Absyn_Conref*) _temp1497.nullable; goto _LL1502; _LL1502: _temp1501=(
struct Cyc_Absyn_Tqual) _temp1497.tq; goto _LL1500; _LL1500: _temp1499=( struct
Cyc_Absyn_Conref*) _temp1497.bounds; goto _LL1488;} else{ goto _LL1489;} _LL1489:
goto _LL1490; _LL1486:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Exp*
_temp1509= _temp1491; if( _temp1509 == 0){ _throw( Null_Exception);} _temp1509->r;})));
goto _LL1484; _LL1488: { struct _tuple0* temp= Cyc_Toc_temp_var(); void*
temp_typ= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1510= arg->topt;
if( _temp1510 == 0){ _throw( Null_Exception);} _temp1510->v;})); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
Cyc_Absyn_subtract_exp( e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt(
e4, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1484;}
_LL1490:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1513= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_List_List*
_temp1511= typs; if( _temp1511 == 0){ _throw( Null_Exception);} _temp1511->hd;}));
struct _tagged_string _temp1514= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1512= arg->topt; if( _temp1512 == 0){ _throw( Null_Exception);} _temp1512->v;}));
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1513.last_plus_one
- _temp1513.curr, _temp1513.curr, _temp1514.last_plus_one - _temp1514.curr,
_temp1514.curr);})); return; _LL1484:;} goto _LL1453;} _LL1457:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1279, 0, _temp1277, typs))->r)); goto
_LL1453; _LL1459:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1279, 0, _temp1277, typs))->r)); goto _LL1453; _LL1461: if( _temp1277 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1516=( char*)"fprintf without arguments"; struct _tagged_string
_temp1517; _temp1517.curr= _temp1516; _temp1517.base= _temp1516; _temp1517.last_plus_one=
_temp1516 + 26; _temp1517;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1279,({ struct Cyc_Core_Opt* _temp1518=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1518->v=( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1519= _temp1277; if( _temp1519 == 0){ _throw(
Null_Exception);} _temp1519->hd;})); _temp1518;}),({ struct Cyc_List_List*
_temp1520= _temp1277; if( _temp1520 == 0){ _throw( Null_Exception);} _temp1520->tl;}),({
struct Cyc_List_List* _temp1521= typs; if( _temp1521 == 0){ _throw(
Null_Exception);} _temp1521->tl;})))->r)); goto _LL1453; _LL1463: if( _temp1277
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1522=( char*)"sscanf without arguments"; struct
_tagged_string _temp1523; _temp1523.curr= _temp1522; _temp1523.base= _temp1522;
_temp1523.last_plus_one= _temp1522 + 25; _temp1523;}));}( void*)((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1524= _temp1277; if( _temp1524 == 0){ _throw(
Null_Exception);} _temp1524->hd;}))->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1525= _temp1277; if( _temp1525 == 0){ _throw( Null_Exception);} _temp1525->hd;}))->r,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1526= _temp1277; if(
_temp1526 == 0){ _throw( Null_Exception);} _temp1526->hd;}))->loc), Cyc_Toc_curr_sp,
0))->r)); goto _LL1453; _LL1465: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1530= _temp1277; if( _temp1530 == 0){ _throw( Null_Exception);} _temp1530->hd;});
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1529=({ struct Cyc_List_List* _temp1528= _temp1277; if( _temp1528 == 0){
_throw( Null_Exception);} _temp1528->tl;}); if( _temp1529 == 0){ _throw(
Null_Exception);} _temp1529->hd;}); struct _tuple0* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1527=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1527->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1527;}),
e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1453; _LL1467: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)({ struct Cyc_List_List* _temp1531= old_typs; if( _temp1531 == 0){ _throw(
Null_Exception);} _temp1531->hd;}))){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1538= _temp1277; if( _temp1538 == 0){ _throw(
Null_Exception);} _temp1538->hd;}); struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1537=({ struct Cyc_List_List* _temp1536= _temp1277;
if( _temp1536 == 0){ _throw( Null_Exception);} _temp1536->tl;}); if( _temp1537
== 0){ _throw( Null_Exception);} _temp1537->hd;}); void* old_typ_e2=( void*)({
struct Cyc_List_List* _temp1535=({ struct Cyc_List_List* _temp1534= old_typs;
if( _temp1534 == 0){ _throw( Null_Exception);} _temp1534->tl;}); if( _temp1535
== 0){ _throw( Null_Exception);} _temp1535->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_List_List* _temp1533= old_typs; if( _temp1533 == 0){ _throw(
Null_Exception);} _temp1533->hd;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1532=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1532->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp1532;}), e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0);
s= Cyc_Absyn_declare_stmt( temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1453;
_LL1469: goto _LL1471; _LL1471: goto _LL1473; _LL1473: goto _LL1475; _LL1475:
goto _LL1477; _LL1477: goto _LL1479; _LL1479: { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1544= _temp1277; if(
_temp1544 == 0){ _throw( Null_Exception);} _temp1544->hd;}); struct Cyc_Absyn_Exp*
e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1543=({ struct Cyc_List_List*
_temp1542= _temp1277; if( _temp1542 == 0){ _throw( Null_Exception);} _temp1542->tl;});
if( _temp1543 == 0){ _throw( Null_Exception);} _temp1543->hd;}); void* t1=( void*)({
struct Cyc_List_List* _temp1541= old_typs; if( _temp1541 == 0){ _throw(
Null_Exception);} _temp1541->hd;}); void* t2=( void*)({ struct Cyc_List_List*
_temp1540=({ struct Cyc_List_List* _temp1539= old_typs; if( _temp1539 == 0){
_throw( Null_Exception);} _temp1539->tl;}); if( _temp1540 == 0){ _throw(
Null_Exception);} _temp1540->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1453;} _LL1481: goto
_LL1453; _LL1453:;} goto _LL1194;} _LL1206: { void* e2_cyc_typ=( void*)({ struct
Cyc_Core_Opt* _temp1545= _temp1283->topt; if( _temp1545 == 0){ _throw(
Null_Exception);} _temp1545->v;}); Cyc_Toc_exp_to_c( nv, _temp1283); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){ if( _temp1281 ==( void*) Cyc_Absyn_PreInc? 1: _temp1281 ==( void*)
Cyc_Absyn_PreDec){ struct _tuple0* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Absyn_star_typ(
Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
temp, 0), 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0), _temp1281, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp, temp_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1283, 0), s, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));} else{ struct _tuple0* temp=
Cyc_Toc_temp_var(); struct _tuple0* temp2= Cyc_Toc_temp_var(); void* temp2_typ=
Cyc_Toc_typ_to_c( e2_cyc_typ); void* temp_typ= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c(
e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp2, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( temp,
0), Cyc_Toc_curr_sp, 0), _temp1281, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp2, temp2_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( temp, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1283, 0), s,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} else{
if( ! Cyc_Absyn_is_lvalue( _temp1283)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1283, 0, Cyc_Toc_incr_lvalue,
_temp1281);( void*)( e->r=( void*)(( void*) _temp1283->r));}} goto _LL1194;}
_LL1208: { int e1_poly= Cyc_Toc_is_poly_project( _temp1289); void* e1_old_typ=(
void*)({ struct Cyc_Core_Opt* _temp1551= _temp1289->topt; if( _temp1551 == 0){
_throw( Null_Exception);} _temp1551->v;}); void* e2_old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1550= _temp1285->topt; if( _temp1550 == 0){ _throw(
Null_Exception);} _temp1550->v;}); Cyc_Toc_exp_to_c( nv, _temp1289); Cyc_Toc_exp_to_c(
nv, _temp1285); if( _temp1287 != 0?( void*)({ struct Cyc_Core_Opt* _temp1546=
_temp1287; if( _temp1546 == 0){ _throw( Null_Exception);} _temp1546->v;}) ==(
void*) Cyc_Absyn_Plus? 1:( void*)({ struct Cyc_Core_Opt* _temp1547= _temp1287;
if( _temp1547 == 0){ _throw( Null_Exception);} _temp1547->v;}) ==( void*) Cyc_Absyn_Minus:
0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){( void*)( _temp1289->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)
_temp1289->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1285->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1285->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1285->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1285->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1289)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1289, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1548=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1548[ 0]=({ struct _tuple11 _temp1549; _temp1549.f1=
_temp1287; _temp1549.f2= _temp1285; _temp1549;}); _temp1548;}));( void*)( e->r=(
void*)(( void*) _temp1289->r));} goto _LL1194;} _LL1210: Cyc_Toc_exp_to_c( nv,
_temp1295); Cyc_Toc_exp_to_c( nv, _temp1293); Cyc_Toc_exp_to_c( nv, _temp1291);
goto _LL1194; _LL1212: Cyc_Toc_exp_to_c( nv, _temp1299); Cyc_Toc_exp_to_c( nv,
_temp1297); goto _LL1194; _LL1214: _temp1307= _temp1303; _temp1305= _temp1301;
goto _LL1216; _LL1216: Cyc_Toc_exp_to_c( nv, _temp1307);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1305); goto _LL1194; _LL1218:
Cyc_Toc_exp_to_c( nv, _temp1309);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1309), 0))->r)); goto
_LL1194; _LL1220: Cyc_Toc_exp_to_c( nv, _temp1311); goto _LL1194; _LL1222: Cyc_Toc_exp_to_c(
nv, _temp1315);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1554= e->topt; if( _temp1554 == 0){ _throw( Null_Exception);} _temp1554->v;}));
for( 0; _temp1313 != 0; _temp1313=({ struct Cyc_List_List* _temp1552= _temp1313;
if( _temp1552 == 0){ _throw( Null_Exception);} _temp1552->tl;})){ void* k= Cyc_Tcutil_typ_kind((
void*)({ struct Cyc_List_List* _temp1553= _temp1313; if( _temp1553 == 0){ _throw(
Null_Exception);} _temp1553->hd;})); if( k !=( void*) Cyc_Absyn_EffKind? k !=(
void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1315, 0))->r)); break;}} goto _LL1194;} _LL1224: { void* old_t2=(
void*)({ struct Cyc_Core_Opt* _temp1555=(* _temp1319)->topt; if( _temp1555 == 0){
_throw( Null_Exception);} _temp1555->v;}); void* new_typ=* _temp1322;* _temp1322=
Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,* _temp1319);* _temp1319= Cyc_Toc_convert_exp(
nv->toplevel, old_t2, new_typ,* _temp1319); goto _LL1194;} _LL1226:{ void*
_temp1556=( void*) _temp1323->r; struct Cyc_Absyn_Structdecl* _temp1564; struct
Cyc_List_List* _temp1566; struct Cyc_Core_Opt* _temp1568; struct _tuple0*
_temp1570; struct Cyc_List_List* _temp1572; _LL1558: if(*(( int*) _temp1556) ==
Cyc_Absyn_Struct_e){ _LL1571: _temp1570=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1556)->f1; goto _LL1569; _LL1569: _temp1568=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1556)->f2; goto _LL1567; _LL1567:
_temp1566=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1556)->f3; goto _LL1565; _LL1565: _temp1564=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1556)->f4; goto _LL1559;} else{ goto
_LL1560;} _LL1560: if(*(( int*) _temp1556) == Cyc_Absyn_Tuple_e){ _LL1573:
_temp1572=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1556)->f1;
goto _LL1561;} else{ goto _LL1562;} _LL1562: goto _LL1563; _LL1559: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string
_temp1574= Cyc_Position_string_of_segment( _temp1323->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1574.last_plus_one - _temp1574.curr, _temp1574.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)({ struct Cyc_Core_Opt*
_temp1575= _temp1323->topt; if( _temp1575 == 0){ _throw( Null_Exception);}
_temp1575->v;}), 1, 0, _temp1566, _temp1570))->r)); goto _LL1557; _LL1561: if(
nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct
_tagged_string _temp1576= Cyc_Position_string_of_segment( _temp1323->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1576.last_plus_one -
_temp1576.curr, _temp1576.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, 0, _temp1572))->r)); goto _LL1557; _LL1563: Cyc_Toc_exp_to_c( nv,
_temp1323); if( ! Cyc_Absyn_is_lvalue( _temp1323)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1323, 0, Cyc_Toc_address_lvalue,
1);( void*)( e->r=( void*)(( void*) _temp1323->r));} goto _LL1557; _LL1557:;}
goto _LL1194; _LL1228: if( nv->toplevel){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ struct _tagged_string _temp1577= Cyc_Position_string_of_segment(
_temp1325->loc); xprintf("%.*s: new at top-level", _temp1577.last_plus_one -
_temp1577.curr, _temp1577.curr);}));}{ void* _temp1578=( void*) _temp1325->r;
struct Cyc_List_List* _temp1592; void* _temp1594; struct _tagged_string
_temp1596; struct Cyc_Absyn_Exp* _temp1598; struct Cyc_Absyn_Exp* _temp1600;
struct Cyc_Absyn_Vardecl* _temp1602; struct Cyc_Absyn_Structdecl* _temp1604;
struct Cyc_List_List* _temp1606; struct Cyc_Core_Opt* _temp1608; struct _tuple0*
_temp1610; struct Cyc_List_List* _temp1612; _LL1580: if(*(( int*) _temp1578) ==
Cyc_Absyn_Array_e){ _LL1593: _temp1592=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1578)->f1; goto _LL1581;} else{ goto _LL1582;} _LL1582: if(*(( int*)
_temp1578) == Cyc_Absyn_Const_e){ _LL1595: _temp1594=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1578)->f1; if(( unsigned int) _temp1594 > 1u?*(( int*) _temp1594) == Cyc_Absyn_String_c:
0){ _LL1597: _temp1596=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1594)->f1; goto _LL1583;} else{ goto _LL1584;}} else{ goto _LL1584;}
_LL1584: if(*(( int*) _temp1578) == Cyc_Absyn_Comprehension_e){ _LL1603:
_temp1602=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1578)->f1; goto _LL1601; _LL1601: _temp1600=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1578)->f2; goto _LL1599; _LL1599:
_temp1598=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1578)->f3; goto _LL1585;} else{ goto _LL1586;} _LL1586: if(*(( int*)
_temp1578) == Cyc_Absyn_Struct_e){ _LL1611: _temp1610=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1578)->f1; goto _LL1609; _LL1609:
_temp1608=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1578)->f2;
goto _LL1607; _LL1607: _temp1606=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1578)->f3; goto _LL1605; _LL1605: _temp1604=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1578)->f4; goto _LL1587;} else{ goto
_LL1588;} _LL1588: if(*(( int*) _temp1578) == Cyc_Absyn_Tuple_e){ _LL1613:
_temp1612=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1578)->f1;
goto _LL1589;} else{ goto _LL1590;} _LL1590: goto _LL1591; _LL1581: { struct
_tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x,
0); struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_array( nv, xexp, _temp1592, Cyc_Absyn_exp_stmt(
xexp, 0)); void* old_elt_typ;{ void* _temp1614= Cyc_Tcutil_compress( old_typ);
struct Cyc_Absyn_PtrInfo _temp1620; struct Cyc_Absyn_Conref* _temp1622; struct
Cyc_Absyn_Tqual _temp1624; struct Cyc_Absyn_Conref* _temp1626; void* _temp1628;
void* _temp1630; _LL1616: if(( unsigned int) _temp1614 > 4u?*(( int*) _temp1614)
== Cyc_Absyn_PointerType: 0){ _LL1621: _temp1620=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1614)->f1; _LL1631: _temp1630=( void*)
_temp1620.elt_typ; goto _LL1629; _LL1629: _temp1628=( void*) _temp1620.rgn_typ;
goto _LL1627; _LL1627: _temp1626=( struct Cyc_Absyn_Conref*) _temp1620.nullable;
goto _LL1625; _LL1625: _temp1624=( struct Cyc_Absyn_Tqual) _temp1620.tq; goto
_LL1623; _LL1623: _temp1622=( struct Cyc_Absyn_Conref*) _temp1620.bounds; goto
_LL1617;} else{ goto _LL1618;} _LL1618: goto _LL1619; _LL1617: old_elt_typ=
_temp1630; goto _LL1615; _LL1619: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1632=( char*)"exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1633; _temp1633.curr= _temp1632; _temp1633.base=
_temp1632; _temp1633.last_plus_one= _temp1632 + 52; _temp1633;})); goto _LL1615;
_LL1615:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1592), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1327 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, e2);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1634= _temp1327; if( _temp1634 == 0){ _throw(
Null_Exception);} _temp1634;}); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp(
r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,(
struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1579;}}} _LL1583: if(
_temp1327 == 0){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1635=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1635->f1= Cyc_Toc_abs_ns; _temp1635->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1635;}), 0),({ struct Cyc_List_List* _temp1636=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1636->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1637=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1637[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1638; _temp1638.tag= Cyc_Absyn_Const_e; _temp1638.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1639=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1639[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1640; _temp1640.tag= Cyc_Absyn_String_c;
_temp1640.f1= _temp1596; _temp1640;}); _temp1639;})); _temp1638;}); _temp1637;}),
0); _temp1636->tl= 0; _temp1636;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1647= _temp1327; if(
_temp1647 == 0){ _throw( Null_Exception);} _temp1647;}); Cyc_Toc_exp_to_c( nv, r);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1641=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1641->f1= Cyc_Toc_abs_ns; _temp1641->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1641;}), 0),({ struct Cyc_List_List* _temp1642=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1642->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1643=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1643[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1644; _temp1644.tag= Cyc_Absyn_Const_e; _temp1644.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1645=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1645[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1646; _temp1646.tag= Cyc_Absyn_String_c;
_temp1646.f1= _temp1596; _temp1646;}); _temp1645;})); _temp1644;}); _temp1643;}),
0); _temp1642->tl= 0; _temp1642;}), 0))->r));} goto _LL1579; _LL1585: { int
is_tagged_ptr= 0;{ void* _temp1648= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1654; struct Cyc_Absyn_Conref* _temp1656; struct Cyc_Absyn_Tqual _temp1658;
struct Cyc_Absyn_Conref* _temp1660; void* _temp1662; void* _temp1664; _LL1650:
if(( unsigned int) _temp1648 > 4u?*(( int*) _temp1648) == Cyc_Absyn_PointerType:
0){ _LL1655: _temp1654=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1648)->f1; _LL1665: _temp1664=( void*) _temp1654.elt_typ; goto _LL1663;
_LL1663: _temp1662=( void*) _temp1654.rgn_typ; goto _LL1661; _LL1661: _temp1660=(
struct Cyc_Absyn_Conref*) _temp1654.nullable; goto _LL1659; _LL1659: _temp1658=(
struct Cyc_Absyn_Tqual) _temp1654.tq; goto _LL1657; _LL1657: _temp1656=( struct
Cyc_Absyn_Conref*) _temp1654.bounds; goto _LL1651;} else{ goto _LL1652;} _LL1652:
goto _LL1653; _LL1651:{ void* _temp1666=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1656))->v; void*
_temp1672; _LL1668: if(( unsigned int) _temp1666 > 1u?*(( int*) _temp1666) ==
Cyc_Absyn_Eq_constr: 0){ _LL1673: _temp1672=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1666)->f1; if( _temp1672 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1669;}
else{ goto _LL1670;}} else{ goto _LL1670;} _LL1670: goto _LL1671; _LL1669:
is_tagged_ptr= 1; goto _LL1667; _LL1671: goto _LL1667; _LL1667:;} goto _LL1649;
_LL1653:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1674=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1675; _temp1675.curr= _temp1674; _temp1675.base=
_temp1674; _temp1675.last_plus_one= _temp1674 + 42; _temp1675;})); goto _LL1649;
_LL1649:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)({ struct Cyc_Core_Opt* _temp1699= _temp1598->topt;
if( _temp1699 == 0){ _throw( Null_Exception);} _temp1699->v;}); void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1600);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1602, Cyc_Absyn_var_exp( max, 0), _temp1598,
Cyc_Absyn_skip_stmt( 0), 1); struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1697=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1697->hd=( void*)({ struct _tuple10* _temp1698=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1698->f1= max; _temp1698->f2= Cyc_Absyn_uint_t;
_temp1698->f3=( struct Cyc_Absyn_Exp*) _temp1600; _temp1698;}); _temp1697->tl= 0;
_temp1697;}); struct Cyc_Absyn_Exp* ai; if( _temp1327 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp1676= _temp1327; if( _temp1676 == 0){ _throw( Null_Exception);}
_temp1676;}); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct
Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1677=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1677->hd=( void*)({ struct _tuple10* _temp1678=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1678->f1= a; _temp1678->f2= ptr_typ;
_temp1678->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1678;}); _temp1677->tl= decls;
_temp1677;}); if( is_tagged_ptr){ struct _tuple0* b= Cyc_Toc_temp_var(); void* t=
Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* binit= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp1681=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1681->hd=( void*)({ struct _tuple4* _temp1686=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1686->f1= 0;
_temp1686->f2= Cyc_Absyn_var_exp( a, 0); _temp1686;}); _temp1681->tl=({ struct
Cyc_List_List* _temp1682=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1682->hd=( void*)({ struct _tuple4* _temp1685=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1685->f1= 0; _temp1685->f2= Cyc_Absyn_var_exp( a,
0); _temp1685;}); _temp1682->tl=({ struct Cyc_List_List* _temp1683=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1683->hd=( void*)({ struct
_tuple4* _temp1684=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1684->f1= 0; _temp1684->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1684;}); _temp1683->tl= 0; _temp1683;});
_temp1682;}); _temp1681;}), 0); decls=({ struct Cyc_List_List* _temp1679=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1679->hd=(
void*)({ struct _tuple10* _temp1680=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1680->f1= b; _temp1680->f2= t; _temp1680->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1680;}); _temp1679->tl= decls; _temp1679;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp1687= ds; if(
_temp1687 == 0){ _throw( Null_Exception);} _temp1687->tl;})){ struct Cyc_Absyn_Exp*
_temp1691; void* _temp1693; struct _tuple0* _temp1695; struct _tuple10 _temp1689=*((
struct _tuple10*)({ struct Cyc_List_List* _temp1688= ds; if( _temp1688 == 0){
_throw( Null_Exception);} _temp1688->hd;})); _LL1696: _temp1695= _temp1689.f1;
goto _LL1694; _LL1694: _temp1693= _temp1689.f2; goto _LL1692; _LL1692: _temp1691=
_temp1689.f3; goto _LL1690; _LL1690: s= Cyc_Absyn_declare_stmt( _temp1695,
_temp1693, _temp1691, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r)); goto _LL1579;}}}} _LL1587:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1700= _temp1325->topt; if( _temp1700 ==
0){ _throw( Null_Exception);} _temp1700->v;}), 1, _temp1327, _temp1606,
_temp1610))->r)); goto _LL1579; _LL1589:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1327, _temp1612))->r)); goto _LL1579; _LL1591: { void* old_elt_typ=(
void*)({ struct Cyc_Core_Opt* _temp1702= _temp1325->topt; if( _temp1702 == 0){
_throw( Null_Exception);} _temp1702->v;}); struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0);
struct Cyc_Absyn_Exp* e_index= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c(
nv, _temp1325); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1325, 0), s, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); struct Cyc_Absyn_Exp*
e1; if( _temp1327 == 0){ e1= Cyc_Toc_malloc_exp( old_elt_typ, e2);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1701=
_temp1327; if( _temp1701 == 0){ _throw( Null_Exception);} _temp1701;}); Cyc_Toc_exp_to_c(
nv, r); e1= Cyc_Toc_rmalloc_exp( r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp(
ptr_typ, e1, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, ptr_typ,( struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1579;}}}
_LL1579:;} goto _LL1194; _LL1230:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1703=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1703[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1704; _temp1704.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1704.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1329);
_temp1704;}); _temp1703;}))); goto _LL1194; _LL1232: Cyc_Toc_exp_to_c( nv,
_temp1331); goto _LL1194; _LL1234: { void* e1_typ= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp1734= _temp1333->topt; if( _temp1734 == 0){ _throw(
Null_Exception);} _temp1734->v;})); Cyc_Toc_exp_to_c( nv, _temp1333);{ void*
_temp1705= e1_typ; struct Cyc_Absyn_PtrInfo _temp1711; struct Cyc_Absyn_Conref*
_temp1713; struct Cyc_Absyn_Tqual _temp1715; struct Cyc_Absyn_Conref* _temp1717;
void* _temp1719; void* _temp1721; _LL1707: if(( unsigned int) _temp1705 > 4u?*((
int*) _temp1705) == Cyc_Absyn_PointerType: 0){ _LL1712: _temp1711=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1705)->f1; _LL1722: _temp1721=( void*)
_temp1711.elt_typ; goto _LL1720; _LL1720: _temp1719=( void*) _temp1711.rgn_typ;
goto _LL1718; _LL1718: _temp1717=( struct Cyc_Absyn_Conref*) _temp1711.nullable;
goto _LL1716; _LL1716: _temp1715=( struct Cyc_Absyn_Tqual) _temp1711.tq; goto
_LL1714; _LL1714: _temp1713=( struct Cyc_Absyn_Conref*) _temp1711.bounds; goto
_LL1708;} else{ goto _LL1709;} _LL1709: goto _LL1710; _LL1708:{ void* _temp1723=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1713); struct
Cyc_Absyn_Exp* _temp1729; _LL1725: if( _temp1723 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1726;} else{ goto _LL1727;} _LL1727: if(( unsigned int) _temp1723 > 1u?*((
int*) _temp1723) == Cyc_Absyn_Upper_b: 0){ _LL1730: _temp1729=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1723)->f1; goto _LL1728;} else{ goto
_LL1724;} _LL1726: { struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array( _temp1721); struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1, _temp1715); void* tx= Cyc_Absyn_strct(
n); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1715); struct Cyc_Absyn_Exp*
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
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp1333, s1, 0), 0))->r));
goto _LL1724;} _LL1728: if( Cyc_Toc_is_nullable( e1_typ)){ struct _tuple0* x=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1731= _temp1333->topt; if( _temp1731 == 0){ _throw( Null_Exception);}
_temp1731->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1333, s3, 0), 0))->r));} goto _LL1724; _LL1724:;} goto _LL1706; _LL1710:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1732=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1733; _temp1733.curr= _temp1732; _temp1733.base= _temp1732; _temp1733.last_plus_one=
_temp1732 + 29; _temp1733;})); goto _LL1706; _LL1706:;} goto _LL1194;} _LL1236:
Cyc_Toc_exp_to_c( nv, _temp1337); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({ struct
Cyc_Core_Opt* _temp1735= e->topt; if( _temp1735 == 0){ _throw( Null_Exception);}
_temp1735->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1194;
_LL1238: Cyc_Toc_exp_to_c( nv, _temp1341);{ int is_poly= Cyc_Toc_is_poly_project(
e); if( Cyc_Toc_is_nullable(( void*)({ struct Cyc_Core_Opt* _temp1736= _temp1341->topt;
if( _temp1736 == 0){ _throw( Null_Exception);} _temp1736->v;}))){ struct _tuple0*
x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1737= _temp1341->topt; if( _temp1737 == 0){ _throw( Null_Exception);}
_temp1737->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0),
_temp1339, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1341, s3, 0), 0))->r));} if( is_poly){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1738= e->topt; if( _temp1738 == 0){ _throw(
Null_Exception);} _temp1738->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}
goto _LL1194;} _LL1240: { void* e1_typ= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp1791= _temp1345->topt; if( _temp1791 == 0){ _throw(
Null_Exception);} _temp1791->v;})); Cyc_Toc_exp_to_c( nv, _temp1345); Cyc_Toc_exp_to_c(
nv, _temp1343);( void*)( _temp1345->r=( void*)(( void*)( Cyc_Toc_array_to_ptr(
Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp(( void*) _temp1345->r, 0), 0))->r));{
void* _temp1739= e1_typ; struct Cyc_List_List* _temp1749; struct Cyc_Absyn_Exp*
_temp1751; struct Cyc_Absyn_Tqual _temp1753; void* _temp1755; struct Cyc_Absyn_PtrInfo
_temp1757; struct Cyc_Absyn_Conref* _temp1759; struct Cyc_Absyn_Tqual _temp1761;
struct Cyc_Absyn_Conref* _temp1763; void* _temp1765; void* _temp1767; _LL1741:
if(( unsigned int) _temp1739 > 4u?*(( int*) _temp1739) == Cyc_Absyn_TupleType: 0){
_LL1750: _temp1749=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1739)->f1; goto _LL1742;} else{ goto _LL1743;} _LL1743: if(( unsigned int)
_temp1739 > 4u?*(( int*) _temp1739) == Cyc_Absyn_ArrayType: 0){ _LL1756:
_temp1755=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1739)->f1; goto
_LL1754; _LL1754: _temp1753=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1739)->f2; goto _LL1752; _LL1752: _temp1751=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1739)->f3; goto _LL1744;} else{ goto
_LL1745;} _LL1745: if(( unsigned int) _temp1739 > 4u?*(( int*) _temp1739) == Cyc_Absyn_PointerType:
0){ _LL1758: _temp1757=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1739)->f1; _LL1768: _temp1767=( void*) _temp1757.elt_typ; goto _LL1766;
_LL1766: _temp1765=( void*) _temp1757.rgn_typ; goto _LL1764; _LL1764: _temp1763=(
struct Cyc_Absyn_Conref*) _temp1757.nullable; goto _LL1762; _LL1762: _temp1761=(
struct Cyc_Absyn_Tqual) _temp1757.tq; goto _LL1760; _LL1760: _temp1759=( struct
Cyc_Absyn_Conref*) _temp1757.bounds; goto _LL1746;} else{ goto _LL1747;} _LL1747:
goto _LL1748; _LL1742: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1343) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1345,({ struct _tagged_string* _temp1769=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1769[ 0]= xprintf("f%d",( int)
i); _temp1769;}), 0))->r)); goto _LL1740;} _LL1744: if( _temp1751 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1770=( char*)"exp_to_c: Subscript_e array type bound null"; struct
_tagged_string _temp1771; _temp1771.curr= _temp1770; _temp1771.base= _temp1770;
_temp1771.last_plus_one= _temp1770 + 44; _temp1771;}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_known_size_subscript( _temp1345, _temp1343, 0,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1772= _temp1751; if( _temp1772 == 0){ _throw(
Null_Exception);} _temp1772;}), _temp1755, _temp1753, 0))->r)); goto _LL1740;
_LL1746:{ void* _temp1773=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1759); struct Cyc_Absyn_Exp* _temp1779; _LL1775: if(( unsigned int)
_temp1773 > 1u?*(( int*) _temp1773) == Cyc_Absyn_Upper_b: 0){ _LL1780: _temp1779=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1773)->f1; goto
_LL1776;} else{ goto _LL1777;} _LL1777: if( _temp1773 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1778;} else{ goto _LL1774;} _LL1776: { int possibly_null= 1;{ void*
_temp1781=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1763))->v; int _temp1787; _LL1783: if((
unsigned int) _temp1781 > 1u?*(( int*) _temp1781) == Cyc_Absyn_Eq_constr: 0){
_LL1788: _temp1787=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1781)->f1;
goto _LL1784;} else{ goto _LL1785;} _LL1785: goto _LL1786; _LL1784:
possibly_null= _temp1787; goto _LL1782; _LL1786: possibly_null= 0; goto _LL1782;
_LL1782:;}( void*)( e->r=( void*)(( void*)( Cyc_Toc_known_size_subscript(
_temp1345, _temp1343, possibly_null, _temp1779, _temp1767, _temp1761, 0))->r));
goto _LL1774;} _LL1778: { struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0*
i= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array(
_temp1767); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1,
_temp1761); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1,
_temp1761); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1343, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1345, s1, 0), 0))->r)); goto _LL1774;} _LL1774:;}
goto _LL1740; _LL1748:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1789=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1790; _temp1790.curr= _temp1789; _temp1790.base=
_temp1789; _temp1790.last_plus_one= _temp1789 + 49; _temp1790;})); goto _LL1740;
_LL1740:;} goto _LL1194;} _LL1242: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1347))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1347); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp1347 != 0; _temp1347=({ struct Cyc_List_List* _temp1792=
_temp1347; if( _temp1792 == 0){ _throw( Null_Exception);} _temp1792->tl;}), i ++){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1793=
_temp1347; if( _temp1793 == 0){ _throw( Null_Exception);} _temp1793->hd;}));{
struct Cyc_List_List* des=({ struct Cyc_List_List* _temp1797=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1797->hd=( void*)(( void*)({
struct Cyc_Absyn_FieldName_struct* _temp1798=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp1798[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp1799; _temp1799.tag= Cyc_Absyn_FieldName;
_temp1799.f1=({ struct _tagged_string* _temp1800=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1800[ 0]= xprintf("f%d", i);
_temp1800;}); _temp1799;}); _temp1798;})); _temp1797->tl= 0; _temp1797;}); dles=({
struct Cyc_List_List* _temp1794=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1794->hd=( void*)({ struct _tuple4* _temp1795=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1795->f1=( struct Cyc_List_List*)
des; _temp1795->f2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1796=
_temp1347; if( _temp1796 == 0){ _throw( Null_Exception);} _temp1796->hd;});
_temp1795;}); _temp1794->tl= dles; _temp1794;});}}} dles=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp( 0,
dles, 0), 0))->r));} goto _LL1194; _LL1244:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1801=( char*)"compoundlit";
struct _tagged_string _temp1802; _temp1802.curr= _temp1801; _temp1802.base=
_temp1801; _temp1802.last_plus_one= _temp1801 + 12; _temp1802;})); goto _LL1194;
_LL1246:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1353, 0))->r));{ struct Cyc_List_List* dles= _temp1353; for( 0; dles != 0;
dles=({ struct Cyc_List_List* _temp1803= dles; if( _temp1803 == 0){ _throw(
Null_Exception);} _temp1803->tl;})){ struct _tuple4 _temp1807; struct Cyc_Absyn_Exp*
_temp1808; struct Cyc_List_List* _temp1810; struct _tuple4* _temp1805=( struct
_tuple4*)({ struct Cyc_List_List* _temp1804= dles; if( _temp1804 == 0){ _throw(
Null_Exception);} _temp1804->hd;}); _temp1807=* _temp1805; _LL1811: _temp1810=
_temp1807.f1; goto _LL1809; _LL1809: _temp1808= _temp1807.f2; goto _LL1806;
_LL1806: Cyc_Toc_exp_to_c( nv, _temp1808);}} goto _LL1194; _LL1248: {
unsigned int sz= Cyc_Evexp_eval_const_uint_exp( _temp1357); Cyc_Toc_exp_to_c( nv,
_temp1355);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp1355)){
unsigned int i= 0; for( 0; i < sz; i ++){ es=({ struct Cyc_List_List* _temp1812=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1812->hd=(
void*)({ struct _tuple4* _temp1813=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1813->f1= 0; _temp1813->f2= _temp1355; _temp1813;}); _temp1812->tl=
es; _temp1812;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL1194;}} _LL1250: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1363, _temp1367))->r));}
else{ struct _tuple0* n= _temp1367; struct Cyc_Absyn_Structdecl* sd2=( struct
Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl* _temp1827= _temp1361 == 0?((
struct Cyc_Absyn_Structdecl*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1826= Cyc_Absynpp_qvar2string( _temp1367); xprintf("no structdecl %.*s",
_temp1826.last_plus_one - _temp1826.curr, _temp1826.curr);})): _temp1361; if(
_temp1827 == 0){ _throw( Null_Exception);} _temp1827;}); struct _RegionHandle
_temp1814= _new_region(); struct _RegionHandle* rgn=& _temp1814; struct
_handler_cons _temp1815; _push_handler(& _temp1815);{ void* _temp1816=( void*)
setjmp( _temp1815.handler); if( ! _temp1816){{ struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1363,( struct Cyc_Absyn_Structdecl*) sd2); for( 0; fields != 0;
fields=({ struct Cyc_List_List* _temp1817= fields; if( _temp1817 == 0){ _throw(
Null_Exception);} _temp1817->tl;})){ struct _tuple12 _temp1821; struct Cyc_Absyn_Exp*
_temp1822; struct Cyc_Absyn_Structfield* _temp1824; struct _tuple12* _temp1819=(
struct _tuple12*)({ struct Cyc_List_List* _temp1818= fields; if( _temp1818 == 0){
_throw( Null_Exception);} _temp1818->hd;}); _temp1821=* _temp1819; _LL1825:
_temp1824= _temp1821.f1; goto _LL1823; _LL1823: _temp1822= _temp1821.f2; goto
_LL1820; _LL1820: { void* fieldtyp=( void*) _temp1824->type; Cyc_Toc_exp_to_c(
nv, _temp1822); if( Cyc_Toc_is_void_star( fieldtyp)){( void*)( _temp1822->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp1822->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0, _temp1363, 0), 0))->r));}
_free_region(& _temp1814); _pop_handler();} else{ _free_region(& _temp1814);
_throw( _temp1816);}}} goto _LL1194; _LL1252: { struct _tuple0* qv= _temp1369->name;
if( _temp1371->is_xtunion? 1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_var_exp( qv, 0))->r));} else{ int tag_count= 0; struct Cyc_List_List*
fields= _temp1371->fields == 0? 0:( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1831= _temp1371->fields; if( _temp1831 == 0){ _throw( Null_Exception);}
_temp1831->v;}); while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1828= fields; if( _temp1828 == 0){ _throw(
Null_Exception);} _temp1828->hd;}))->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1829= fields; if( _temp1829 == 0){ _throw(
Null_Exception);} _temp1829->hd;}))->typs == 0){ tag_count ++;} fields=({ struct
Cyc_List_List* _temp1830= fields; if( _temp1830 == 0){ _throw( Null_Exception);}
_temp1830->tl;});}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0))->r));} goto _LL1194;} _LL1254: { struct _tuple0* qv=
_temp1379->name; struct Cyc_List_List* tqts= _temp1379->typs; struct _tuple0* x=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); void*
strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1859=( char*)"_struct"; struct _tagged_string
_temp1860; _temp1860.curr= _temp1859; _temp1860.base= _temp1859; _temp1860.last_plus_one=
_temp1859 + 8; _temp1860;}))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1381->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( qv, 0);} else{ int
tag_count= 0; struct Cyc_List_List* fields= _temp1381->fields == 0? 0:( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1835= _temp1381->fields; if(
_temp1835 == 0){ _throw( Null_Exception);} _temp1835->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1832= fields;
if( _temp1832 == 0){ _throw( Null_Exception);} _temp1832->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1833= fields;
if( _temp1833 == 0){ _throw( Null_Exception);} _temp1833->hd;}))->typs != 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1834= fields; if( _temp1834
== 0){ _throw( Null_Exception);} _temp1834->tl;});} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles= 0; for( 0; _temp1383
!= 0; _temp1383=({ struct Cyc_List_List* _temp1836= _temp1383; if( _temp1836 ==
0){ _throw( Null_Exception);} _temp1836->tl;}), tqts=({ struct Cyc_List_List*
_temp1837= tqts; if( _temp1837 == 0){ _throw( Null_Exception);} _temp1837->tl;})){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1843= _temp1383; if( _temp1843 == 0){ _throw( Null_Exception);} _temp1843->hd;});
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp1842= tqts; if( _temp1842 == 0){ _throw( Null_Exception);} _temp1842->hd;}))).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} dles=({ struct Cyc_List_List* _temp1838=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1838[ 0]=({ struct Cyc_List_List
_temp1839; _temp1839.hd=( void*)({ struct _tuple4* _temp1840=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp1840[ 0]=({ struct _tuple4
_temp1841; _temp1841.f1= 0; _temp1841.f2= cur_e; _temp1841;}); _temp1840;});
_temp1839.tl= dles; _temp1839;}); _temp1838;});} dles=({ struct Cyc_List_List*
_temp1844=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1844[ 0]=({ struct Cyc_List_List _temp1845; _temp1845.hd=( void*)({ struct
_tuple4* _temp1846=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp1846[ 0]=({ struct _tuple4 _temp1847; _temp1847.f1= 0; _temp1847.f2=
tag_exp; _temp1847;}); _temp1846;}); _temp1845.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); _temp1845;}); _temp1844;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1848=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1848->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({
char* _temp1849=( char*)"_struct"; struct _tagged_string _temp1850; _temp1850.curr=
_temp1849; _temp1850.base= _temp1849; _temp1850.last_plus_one= _temp1849 + 8;
_temp1850;})); _temp1848;}), dles, 0))->r));}} else{ struct Cyc_List_List*
rev_assign=({ struct Cyc_List_List* _temp1858=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1858->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( xexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( qv, 0),
0); _temp1858->tl= 0; _temp1858;});{ int i= 1; for( 0; _temp1383 != 0;(
_temp1383=({ struct Cyc_List_List* _temp1851= _temp1383; if( _temp1851 == 0){
_throw( Null_Exception);} _temp1851->tl;}), i ++), tqts=({ struct Cyc_List_List*
_temp1852= tqts; if( _temp1852 == 0){ _throw( Null_Exception);} _temp1852->tl;})){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1856= _temp1383; if( _temp1856 == 0){ _throw( Null_Exception);} _temp1856->hd;});
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp1855= tqts; if( _temp1855 == 0){ _throw( Null_Exception);} _temp1855->hd;}))).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1854=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1854[ 0]= xprintf("f%d", i); _temp1854;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1853=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1853->hd=( void*) a; _temp1853->tl=
rev_assign; _temp1853;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1857=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1857->hd=(
void*) final_stmt; _temp1857->tl= rev_assign; _temp1857;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL1194;} _LL1256: goto _LL1194; _LL1258: { void* t_c= Cyc_Toc_typ_to_c(
_temp1395); if( _temp1397 != 0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1861= _temp1397; if( _temp1861 == 0){ _throw(
Null_Exception);} _temp1861;}); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)((
void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1395, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} goto _LL1194;} _LL1260: Cyc_Toc_stmt_to_c( nv, _temp1399); goto
_LL1194; _LL1262:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1862=( char*)"UnresolvedMem"; struct
_tagged_string _temp1863; _temp1863.curr= _temp1862; _temp1863.base= _temp1862;
_temp1863.last_plus_one= _temp1862 + 14; _temp1863;})); goto _LL1194; _LL1264:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1864=( char*)"codegen"; struct _tagged_string _temp1865; _temp1865.curr=
_temp1864; _temp1865.base= _temp1864; _temp1865.last_plus_one= _temp1864 + 8;
_temp1865;})); goto _LL1194; _LL1266:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1866=( char*)"fill"; struct _tagged_string
_temp1867; _temp1867.curr= _temp1866; _temp1867.base= _temp1866; _temp1867.last_plus_one=
_temp1866 + 5; _temp1867;})); goto _LL1194; _LL1194:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple14{ struct _tuple0* f1; void* f2;
} ; struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, void* t,
struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat* p,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1869=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1905; struct Cyc_Absyn_Vardecl _temp1907; struct Cyc_List_List* _temp1908;
struct Cyc_Core_Opt* _temp1910; struct Cyc_Absyn_Exp* _temp1912; void* _temp1914;
struct Cyc_Absyn_Tqual _temp1916; struct _tuple0* _temp1918; void* _temp1920;
struct Cyc_Absyn_Vardecl* _temp1922; int _temp1924; void* _temp1926; char
_temp1928; struct _tagged_string _temp1930; struct Cyc_Absyn_Enumfield*
_temp1932; struct Cyc_Absyn_Enumdecl* _temp1934; struct Cyc_List_List* _temp1936;
struct Cyc_List_List* _temp1938; struct Cyc_Absyn_Tunionfield* _temp1940; struct
Cyc_Absyn_Tuniondecl* _temp1942; struct Cyc_Absyn_Pat* _temp1945; struct Cyc_Absyn_Pat
_temp1947; struct Cyc_Position_Segment* _temp1948; struct Cyc_Core_Opt*
_temp1950; void* _temp1952; struct Cyc_List_List* _temp1954; struct Cyc_List_List*
_temp1956; struct Cyc_Absyn_Tunionfield* _temp1958; struct Cyc_Absyn_Tuniondecl*
_temp1960; struct Cyc_List_List* _temp1962; struct Cyc_List_List* _temp1964;
struct Cyc_Absyn_Tunionfield* _temp1966; struct Cyc_Absyn_Tuniondecl* _temp1968;
struct Cyc_List_List* _temp1970; struct Cyc_List_List* _temp1972; struct Cyc_List_List*
_temp1974; struct Cyc_Core_Opt* _temp1976; struct Cyc_Absyn_Structdecl*
_temp1978; struct Cyc_Absyn_Pat* _temp1980; struct _tuple0* _temp1982; struct
Cyc_List_List* _temp1984; struct Cyc_List_List* _temp1986; struct _tuple0*
_temp1988; struct Cyc_List_List* _temp1990; struct Cyc_List_List* _temp1992;
struct _tuple0* _temp1994; _LL1871: if(( unsigned int) _temp1869 > 2u?*(( int*)
_temp1869) == Cyc_Absyn_Var_p: 0){ _LL1906: _temp1905=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1869)->f1; _temp1907=* _temp1905; _LL1921:
_temp1920=( void*) _temp1907.sc; goto _LL1919; _LL1919: _temp1918=( struct
_tuple0*) _temp1907.name; goto _LL1917; _LL1917: _temp1916=( struct Cyc_Absyn_Tqual)
_temp1907.tq; goto _LL1915; _LL1915: _temp1914=( void*) _temp1907.type; goto
_LL1913; _LL1913: _temp1912=( struct Cyc_Absyn_Exp*) _temp1907.initializer; goto
_LL1911; _LL1911: _temp1910=( struct Cyc_Core_Opt*) _temp1907.rgn; goto _LL1909;
_LL1909: _temp1908=( struct Cyc_List_List*) _temp1907.attributes; goto _LL1872;}
else{ goto _LL1873;} _LL1873: if( _temp1869 ==( void*) Cyc_Absyn_Wild_p){ goto
_LL1874;} else{ goto _LL1875;} _LL1875: if(( unsigned int) _temp1869 > 2u?*((
int*) _temp1869) == Cyc_Absyn_Reference_p: 0){ _LL1923: _temp1922=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Reference_p_struct*) _temp1869)->f1; goto _LL1876;} else{ goto
_LL1877;} _LL1877: if( _temp1869 ==( void*) Cyc_Absyn_Null_p){ goto _LL1878;}
else{ goto _LL1879;} _LL1879: if(( unsigned int) _temp1869 > 2u?*(( int*)
_temp1869) == Cyc_Absyn_Int_p: 0){ _LL1927: _temp1926=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp1869)->f1; goto _LL1925; _LL1925: _temp1924=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1869)->f2; goto _LL1880;} else{ goto _LL1881;} _LL1881: if(( unsigned int)
_temp1869 > 2u?*(( int*) _temp1869) == Cyc_Absyn_Char_p: 0){ _LL1929: _temp1928=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp1869)->f1; goto _LL1882;} else{
goto _LL1883;} _LL1883: if(( unsigned int) _temp1869 > 2u?*(( int*) _temp1869)
== Cyc_Absyn_Float_p: 0){ _LL1931: _temp1930=( struct _tagged_string)(( struct
Cyc_Absyn_Float_p_struct*) _temp1869)->f1; goto _LL1884;} else{ goto _LL1885;}
_LL1885: if(( unsigned int) _temp1869 > 2u?*(( int*) _temp1869) == Cyc_Absyn_Enum_p:
0){ _LL1935: _temp1934=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1869)->f1; goto _LL1933; _LL1933: _temp1932=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1869)->f2; goto _LL1886;} else{ goto
_LL1887;} _LL1887: if(( unsigned int) _temp1869 > 2u?*(( int*) _temp1869) == Cyc_Absyn_Tunion_p:
0){ _LL1943: _temp1942=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1869)->f1; goto _LL1941; _LL1941: _temp1940=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1869)->f2; goto _LL1939; _LL1939:
_temp1938=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1869)->f3; goto _LL1937; _LL1937: _temp1936=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1869)->f4; if( _temp1936 == 0){ goto
_LL1888;} else{ goto _LL1889;}} else{ goto _LL1889;} _LL1889: if(( unsigned int)
_temp1869 > 2u?*(( int*) _temp1869) == Cyc_Absyn_Pointer_p: 0){ _LL1946:
_temp1945=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1869)->f1; _temp1947=* _temp1945; _LL1953: _temp1952=( void*) _temp1947.r;
if(( unsigned int) _temp1952 > 2u?*(( int*) _temp1952) == Cyc_Absyn_Tunion_p: 0){
_LL1961: _temp1960=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1952)->f1; goto _LL1959; _LL1959: _temp1958=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1952)->f2; goto _LL1957; _LL1957:
_temp1956=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1952)->f3; goto _LL1955; _LL1955: _temp1954=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1952)->f4; goto _LL1951;} else{ goto
_LL1891;} _LL1951: _temp1950=( struct Cyc_Core_Opt*) _temp1947.topt; goto
_LL1949; _LL1949: _temp1948=( struct Cyc_Position_Segment*) _temp1947.loc; goto
_LL1944;} else{ goto _LL1891;} _LL1944: if( _temp1954 != 0){ goto _LL1890;}
else{ goto _LL1891;} _LL1891: if(( unsigned int) _temp1869 > 2u?*(( int*)
_temp1869) == Cyc_Absyn_Tunion_p: 0){ _LL1969: _temp1968=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1869)->f1; goto _LL1967; _LL1967:
_temp1966=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1869)->f2; goto _LL1965; _LL1965: _temp1964=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1869)->f3; goto _LL1963; _LL1963:
_temp1962=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1869)->f4; goto _LL1892;} else{ goto _LL1893;} _LL1893: if(( unsigned int)
_temp1869 > 2u?*(( int*) _temp1869) == Cyc_Absyn_Tuple_p: 0){ _LL1971: _temp1970=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp1869)->f1; goto
_LL1894;} else{ goto _LL1895;} _LL1895: if(( unsigned int) _temp1869 > 2u?*((
int*) _temp1869) == Cyc_Absyn_Struct_p: 0){ _LL1979: _temp1978=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1869)->f1; goto _LL1977; _LL1977:
_temp1976=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1869)->f2;
goto _LL1975; _LL1975: _temp1974=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1869)->f3; goto _LL1973; _LL1973: _temp1972=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1869)->f4; goto _LL1896;} else{ goto
_LL1897;} _LL1897: if(( unsigned int) _temp1869 > 2u?*(( int*) _temp1869) == Cyc_Absyn_Pointer_p:
0){ _LL1981: _temp1980=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1869)->f1; goto _LL1898;} else{ goto _LL1899;} _LL1899: if(( unsigned int)
_temp1869 > 2u?*(( int*) _temp1869) == Cyc_Absyn_UnknownId_p: 0){ _LL1983:
_temp1982=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1869)->f1;
goto _LL1900;} else{ goto _LL1901;} _LL1901: if(( unsigned int) _temp1869 > 2u?*((
int*) _temp1869) == Cyc_Absyn_UnknownCall_p: 0){ _LL1989: _temp1988=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1869)->f1; goto _LL1987;
_LL1987: _temp1986=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1869)->f2; goto _LL1985; _LL1985: _temp1984=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1869)->f3; goto _LL1902;} else{
goto _LL1903;} _LL1903: if(( unsigned int) _temp1869 > 2u?*(( int*) _temp1869)
== Cyc_Absyn_UnknownFields_p: 0){ _LL1995: _temp1994=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp1869)->f1; goto _LL1993; _LL1993:
_temp1992=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1869)->f2; goto _LL1991; _LL1991: _temp1990=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1869)->f3; goto _LL1904;} else{
goto _LL1870;} _LL1872: nv= Cyc_Toc_add_varmap( nv, _temp1918, r); goto _LL1874;
_LL1874: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1870; _LL1876: { struct
_tuple0* v= Cyc_Toc_temp_var(); decls=({ struct Cyc_List_List* _temp1996=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1996->hd=(
void*)({ struct _tuple14* _temp1997=( struct _tuple14*) GC_malloc( sizeof(
struct _tuple14)); _temp1997->f1= v; _temp1997->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp1997;}); _temp1996->tl= decls; _temp1996;}); nv= Cyc_Toc_add_varmap(
nv, _temp1922->name, Cyc_Absyn_var_exp( v, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL1870;} _LL1878: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL1870; _LL1880: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp1926, _temp1924, 0), succ_lab, fail_lab); goto _LL1870;
_LL1882: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp1928, 0),
succ_lab, fail_lab); goto _LL1870; _LL1884: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp1930, 0), succ_lab, fail_lab); goto _LL1870; _LL1886: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp1998=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1998[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1999; _temp1999.tag= Cyc_Absyn_Enum_e;
_temp1999.f1= _temp1932->name; _temp1999.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1934; _temp1999.f3=( struct Cyc_Absyn_Enumfield*) _temp1932; _temp1999;});
_temp1998;}), 0), succ_lab, fail_lab); goto _LL1870; _LL1888: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1940->name, 0); if( ! _temp1942->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1870;} _LL1890: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1954);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1958->name,( struct
_tagged_string)({ char* _temp2011=( char*)"_struct"; struct _tagged_string
_temp2012; _temp2012.curr= _temp2011; _temp2012.base= _temp2011; _temp2012.last_plus_one=
_temp2011 + 8; _temp2012;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1954); for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp2000= rps; if(
_temp2000 == 0){ _throw( Null_Exception);} _temp2000->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2007= rps; if(
_temp2007 == 0){ _throw( Null_Exception);} _temp2007->hd;}); struct _tuple0* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp2006= p2->topt;
if( _temp2006 == 0){ _throw( Null_Exception);} _temp2006->v;}); void* t2c= Cyc_Toc_typ_to_c(
t2); struct _tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2001=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2001->hd=( void*)({ struct _tuple14* _temp2002=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2002->f1= v; _temp2002->f2= t2c;
_temp2002;}); _temp2001->tl= decls; _temp2001;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp2005=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2005[ 0]= xprintf("f%d", cnt); _temp2005;}), 0), p2,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp2004=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2004[ 0]= xprintf("f%d", cnt); _temp2004;}), 0), 0),
0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List*
_temp2003=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2003->hd=( void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp2003->tl= ss;
_temp2003;}); succ_lab= lab;}}}}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1960->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1958->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2010= _temp1960->fields; if( _temp2010 == 0){ _throw( Null_Exception);}
_temp2010->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp2008= fs; if(
_temp2008 == 0){ _throw( Null_Exception);} _temp2008->tl;})){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2009= fs; if(
_temp2009 == 0){ _throw( Null_Exception);} _temp2009->hd;}); if( f->typs == 0){
max_tag ++;}}}{ struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t,
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0);
struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1958->name,
0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0):
Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL1870;}} _LL1892:
_temp1970= _temp1962; goto _LL1894; _LL1894: { struct Cyc_List_List* ss= 0; int
cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1970);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1970); for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp2013= rps; if(
_temp2013 == 0){ _throw( Null_Exception);} _temp2013->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2020= rps; if(
_temp2020 == 0){ _throw( Null_Exception);} _temp2020->hd;}); struct _tuple0* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp2019= p2->topt;
if( _temp2019 == 0){ _throw( Null_Exception);} _temp2019->v;}); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2014=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2014->hd=( void*)({ struct _tuple14* _temp2015=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2015->f1= v; _temp2015->f2= Cyc_Toc_typ_to_c(
t2); _temp2015;}); _temp2014->tl= decls; _temp2014;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp2018=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2018[ 0]= xprintf("f%d", cnt);
_temp2018;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp2017=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2017[ 0]= xprintf("f%d", cnt); _temp2017;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp2016=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2016->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp2016->tl= ss; _temp2016;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1870;} _LL1896: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1972); for( 0; dlps != 0; dlps=({ struct Cyc_List_List* _temp2021= dlps;
if( _temp2021 == 0){ _throw( Null_Exception);} _temp2021->tl;})){ struct
_tuple15* tup=( struct _tuple15*)({ struct Cyc_List_List* _temp2035= dlps; if(
_temp2035 == 0){ _throw( Null_Exception);} _temp2035->hd;}); struct Cyc_Absyn_Pat*
p2=(* tup).f2; struct _tagged_string* f;{ void* _temp2023=( void*)({ struct Cyc_List_List*
_temp2022=(* tup).f1; if( _temp2022 == 0){ _throw( Null_Exception);} _temp2022->hd;});
struct _tagged_string* _temp2029; _LL2025: if(*(( int*) _temp2023) == Cyc_Absyn_FieldName){
_LL2030: _temp2029=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp2023)->f1; goto _LL2026;} else{ goto _LL2027;} _LL2027: goto _LL2028;
_LL2026: f= _temp2029; goto _LL2024; _LL2028:( void) _throw(( void*) Cyc_Toc_Match_error);
_LL2024:;}{ struct _tuple0* v= Cyc_Toc_temp_var(); void* t2=( void*)({ struct
Cyc_Core_Opt* _temp2034= p2->topt; if( _temp2034 == 0){ _throw( Null_Exception);}
_temp2034->v;}); void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab=
Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List* _temp2031=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2031->hd=( void*)({ struct
_tuple14* _temp2032=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp2032->f1= v; _temp2032->f2= t2c; _temp2032;}); _temp2031->tl= decls;
_temp2031;});{ struct _tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structmember_exp( path, f, 0), p2, succ_lab, fail_lab, decls);
nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s2= result.f3; struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp( r, f, 0), 0), 0), s2,
0); ss=({ struct Cyc_List_List* _temp2033=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2033->hd=( void*) Cyc_Absyn_label_stmt( lab,
s3, 0); _temp2033->tl= ss; _temp2033;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1870;} _LL1898: { struct _tuple0* v= Cyc_Toc_temp_var(); void*
t2=( void*)({ struct Cyc_Core_Opt* _temp2038= _temp1980->topt; if( _temp2038 ==
0){ _throw( Null_Exception);} _temp2038->v;}); decls=({ struct Cyc_List_List*
_temp2036=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2036->hd=( void*)({ struct _tuple14* _temp2037=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2037->f1= v; _temp2037->f2= Cyc_Toc_typ_to_c(
t2); _temp2037;}); _temp2036->tl= decls; _temp2036;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path,
0), _temp1980, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0),
s2, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);}
else{ s= s3;} goto _LL1870;}}} _LL1900: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2039=(
char*)"unknownid"; struct _tagged_string _temp2040; _temp2040.curr= _temp2039;
_temp2040.base= _temp2039; _temp2040.last_plus_one= _temp2039 + 10; _temp2040;}));
goto _LL1870; _LL1902: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2041=( char*)"unknowncall";
struct _tagged_string _temp2042; _temp2042.curr= _temp2041; _temp2042.base=
_temp2041; _temp2042.last_plus_one= _temp2041 + 12; _temp2042;})); goto _LL1870;
_LL1904: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2043=( char*)"unknownfields"; struct
_tagged_string _temp2044; _temp2044.curr= _temp2043; _temp2044.base= _temp2043;
_temp2044.last_plus_one= _temp2043 + 14; _temp2044;})); goto _LL1870; _LL1870:;}
return({ struct _tuple13 _temp2045; _temp2045.f1= nv; _temp2045.f2= decls;
_temp2045.f3= s; _temp2045;});} struct _tuple16{ struct _tagged_string* f1;
struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause* f3; } ; static struct
_tuple16* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause* sc){ return({
struct _tuple16* _temp2046=( struct _tuple16*) GC_malloc( sizeof( struct
_tuple16)); _temp2046->f1= Cyc_Toc_fresh_label(); _temp2046->f2= Cyc_Toc_fresh_label();
_temp2046->f3= sc; _temp2046;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* t=( void*)({ struct Cyc_Core_Opt*
_temp2102= e->topt; if( _temp2102 == 0){ _throw( Null_Exception);} _temp2102->v;});
int leave_as_switch;{ void* _temp2047= Cyc_Tcutil_compress( t); void* _temp2053;
void* _temp2055; _LL2049: if(( unsigned int) _temp2047 > 4u?*(( int*) _temp2047)
== Cyc_Absyn_IntType: 0){ _LL2056: _temp2055=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2047)->f1; goto _LL2054; _LL2054: _temp2053=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2047)->f2; goto _LL2050;} else{ goto _LL2051;} _LL2051: goto _LL2052;
_LL2050: leave_as_switch= 1; goto _LL2048; _LL2052: leave_as_switch= 0; goto
_LL2048; _LL2048:;}{ struct Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1=({
struct Cyc_List_List* _temp2057= scs1; if( _temp2057 == 0){ _throw(
Null_Exception);} _temp2057->tl;})){ if(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2059=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp2058=
scs1; if( _temp2058 == 0){ _throw( Null_Exception);} _temp2058->hd;}))->pat_vars;
if( _temp2059 == 0){ _throw( Null_Exception);} _temp2059->v;}) != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* scs1= scs; for( 0; scs1 !=
0; scs1=({ struct Cyc_List_List* _temp2060= scs1; if( _temp2060 == 0){ _throw(
Null_Exception);} _temp2060->tl;})){ struct Cyc_Absyn_Stmt* s=(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2062= scs1; if( _temp2062 == 0){ _throw(
Null_Exception);} _temp2062->hd;}))->body;(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2061= scs1; if( _temp2061 == 0){ _throw(
Null_Exception);} _temp2061->hd;}))->body= Cyc_Absyn_label_stmt( next_l, s, 0);
next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), s);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0);
struct _tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple16*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* lscs2= lscs; for( 0; lscs2 != 0; lscs2=({ struct Cyc_List_List*
_temp2063= lscs2; if( _temp2063 == 0){ _throw( Null_Exception);} _temp2063->tl;})){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2075= lscs2; if( _temp2075 == 0){ _throw( Null_Exception);} _temp2075->hd;}))).f3;
struct _tagged_string* fail_lab=({ struct Cyc_List_List* _temp2072= lscs2; if(
_temp2072 == 0){ _throw( Null_Exception);} _temp2072->tl;}) == 0? end_l:(*((
struct _tuple16*)({ struct Cyc_List_List* _temp2074=({ struct Cyc_List_List*
_temp2073= lscs2; if( _temp2073 == 0){ _throw( Null_Exception);} _temp2073->tl;});
if( _temp2074 == 0){ _throw( Null_Exception);} _temp2074->hd;}))).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2071= lscs2; if( _temp2071 == 0){ _throw( Null_Exception);} _temp2071->hd;}))).f2;
if( sc->where_clause == 0){ struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r,
path, sc->pattern, succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List*
_temp2064=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2064->hd=( void*) res.f1; _temp2064->tl= nvs; _temp2064;}); decls= res.f2;
test_stmts=({ struct Cyc_List_List* _temp2065=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2065->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2066= lscs2; if( _temp2066 == 0){
_throw( Null_Exception);} _temp2066->hd;}))).f1, res.f3, 0); _temp2065->tl=
test_stmts; _temp2065;});} else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2070= sc->where_clause; if( _temp2070 == 0){ _throw(
Null_Exception);} _temp2070;}); struct _tagged_string* intermed_lab= Cyc_Toc_fresh_label();
struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r, path, sc->pattern,
intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c( res.f1, e); nvs=({ struct Cyc_List_List*
_temp2067=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2067->hd=( void*) res.f1; _temp2067->tl= nvs; _temp2067;}); decls= res.f2;{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt( e, Cyc_Absyn_goto_stmt(
succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3=
Cyc_Absyn_label_stmt( intermed_lab, s2, 0); test_stmts=({ struct Cyc_List_List*
_temp2068=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2068->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2069= lscs2; if( _temp2069 == 0){ _throw( Null_Exception);} _temp2069->hd;}))).f1,
Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0); _temp2068->tl= test_stmts; _temp2068;});}}}}{
struct Cyc_Absyn_Stmt* test_stmt= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( test_stmts), 0); nvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt*
clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){ for( 0;({ struct Cyc_List_List*
_temp2076= lscs; if( _temp2076 == 0){ _throw( Null_Exception);} _temp2076->tl;})
!= 0; lscs=({ struct Cyc_List_List* _temp2077= lscs; if( _temp2077 == 0){ _throw(
Null_Exception);} _temp2077->tl;}), nvs=({ struct Cyc_List_List* _temp2078= nvs;
if( _temp2078 == 0){ _throw( Null_Exception);} _temp2078->tl;})){ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2088= nvs; if( _temp2088
== 0){ _throw( Null_Exception);} _temp2088->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2087= lscs; if( _temp2087 == 0){
_throw( Null_Exception);} _temp2087->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
e, end_l,(*(( struct _tuple16*)({ struct Cyc_List_List* _temp2080=({ struct Cyc_List_List*
_temp2079= lscs; if( _temp2079 == 0){ _throw( Null_Exception);} _temp2079->tl;});
if( _temp2080 == 0){ _throw( Null_Exception);} _temp2080->hd;}))).f2,( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2083=((*(( struct _tuple16*)({
struct Cyc_List_List* _temp2082=({ struct Cyc_List_List* _temp2081= lscs; if(
_temp2081 == 0){ _throw( Null_Exception);} _temp2081->tl;}); if( _temp2082 == 0){
_throw( Null_Exception);} _temp2082->hd;}))).f3)->pat_vars; if( _temp2083 == 0){
_throw( Null_Exception);} _temp2083->v;}),( struct Cyc_Toc_Env*)({ struct Cyc_List_List*
_temp2085=({ struct Cyc_List_List* _temp2084= nvs; if( _temp2084 == 0){ _throw(
Null_Exception);} _temp2084->tl;}); if( _temp2085 == 0){ _throw( Null_Exception);}
_temp2085->hd;})), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2086= lscs; if( _temp2086 == 0){
_throw( Null_Exception);} _temp2086->hd;}))).f2, s, 0), 0);}{ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2091= nvs; if( _temp2091
== 0){ _throw( Null_Exception);} _temp2091->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2090= lscs; if( _temp2090 == 0){
_throw( Null_Exception);} _temp2090->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env(
e, end_l), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2089= lscs; if( _temp2089 == 0){
_throw( Null_Exception);} _temp2089->hd;}))).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( test_stmt, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=({
struct Cyc_List_List* _temp2092= decls; if( _temp2092 == 0){ _throw(
Null_Exception);} _temp2092->tl;})){ struct _tuple14 _temp2096; void* _temp2097;
struct _tuple0* _temp2099; struct _tuple14* _temp2094=( struct _tuple14*)({
struct Cyc_List_List* _temp2093= decls; if( _temp2093 == 0){ _throw(
Null_Exception);} _temp2093->hd;}); _temp2096=* _temp2094; _LL2100: _temp2099=
_temp2096.f1; goto _LL2098; _LL2098: _temp2097= _temp2096.f2; goto _LL2095;
_LL2095: res= Cyc_Absyn_declare_stmt( _temp2099, _temp2097, 0, res, 0);}( void*)(
whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( v, Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp2101= e->topt; if( _temp2101 == 0){ _throw(
Null_Exception);} _temp2101->v;})),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2103=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2103[ 0]=({ struct Cyc_List_List _temp2104; _temp2104.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2104.tl= 0; _temp2104;}); _temp2103;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2105=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2145; struct Cyc_Absyn_Stmt* _temp2147; struct Cyc_Absyn_Stmt*
_temp2149; struct Cyc_Absyn_Exp* _temp2151; struct Cyc_Absyn_Stmt* _temp2153;
struct Cyc_Absyn_Stmt* _temp2155; struct Cyc_Absyn_Exp* _temp2157; struct Cyc_Absyn_Stmt*
_temp2159; struct _tuple2 _temp2161; struct Cyc_Absyn_Stmt* _temp2163; struct
Cyc_Absyn_Exp* _temp2165; struct Cyc_Absyn_Stmt* _temp2167; struct Cyc_Absyn_Stmt*
_temp2169; struct Cyc_Absyn_Stmt* _temp2171; struct _tagged_string* _temp2173;
struct Cyc_Absyn_Stmt* _temp2175; struct _tuple2 _temp2177; struct Cyc_Absyn_Stmt*
_temp2179; struct Cyc_Absyn_Exp* _temp2181; struct _tuple2 _temp2183; struct Cyc_Absyn_Stmt*
_temp2185; struct Cyc_Absyn_Exp* _temp2187; struct Cyc_Absyn_Exp* _temp2189;
struct Cyc_List_List* _temp2191; struct Cyc_Absyn_Exp* _temp2193; struct Cyc_Absyn_Switch_clause**
_temp2195; struct Cyc_List_List* _temp2197; struct Cyc_Absyn_Stmt* _temp2199;
struct Cyc_Absyn_Decl* _temp2201; struct Cyc_Absyn_Stmt* _temp2203; struct Cyc_Absyn_Stmt*
_temp2205; struct Cyc_Absyn_Stmt* _temp2207; struct _tagged_string* _temp2209;
struct _tuple2 _temp2211; struct Cyc_Absyn_Stmt* _temp2213; struct Cyc_Absyn_Exp*
_temp2215; struct Cyc_Absyn_Stmt* _temp2217; struct Cyc_List_List* _temp2219;
struct Cyc_Absyn_Stmt* _temp2221; struct Cyc_Absyn_Stmt* _temp2223; struct Cyc_Absyn_Vardecl*
_temp2225; struct Cyc_Absyn_Tvar* _temp2227; _LL2107: if( _temp2105 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2108;} else{ goto _LL2109;} _LL2109: if((
unsigned int) _temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_Exp_s: 0){
_LL2146: _temp2145=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp2105)->f1; goto _LL2110;} else{ goto _LL2111;} _LL2111: if(( unsigned int)
_temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_Seq_s: 0){ _LL2150: _temp2149=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp2105)->f1; goto
_LL2148; _LL2148: _temp2147=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp2105)->f2; goto _LL2112;} else{ goto _LL2113;} _LL2113: if(( unsigned int)
_temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_Return_s: 0){ _LL2152:
_temp2151=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp2105)->f1; goto _LL2114;} else{ goto _LL2115;} _LL2115: if(( unsigned int)
_temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_IfThenElse_s: 0){ _LL2158:
_temp2157=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2105)->f1; goto _LL2156; _LL2156: _temp2155=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2105)->f2; goto _LL2154; _LL2154:
_temp2153=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2105)->f3; goto _LL2116;} else{ goto _LL2117;} _LL2117: if(( unsigned int)
_temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_While_s: 0){ _LL2162: _temp2161=(
struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp2105)->f1; _LL2166:
_temp2165= _temp2161.f1; goto _LL2164; _LL2164: _temp2163= _temp2161.f2; goto
_LL2160; _LL2160: _temp2159=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp2105)->f2; goto _LL2118;} else{ goto _LL2119;} _LL2119: if(( unsigned int)
_temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_Break_s: 0){ _LL2168: _temp2167=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp2105)->f1; goto
_LL2120;} else{ goto _LL2121;} _LL2121: if(( unsigned int) _temp2105 > 1u?*((
int*) _temp2105) == Cyc_Absyn_Continue_s: 0){ _LL2170: _temp2169=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp2105)->f1; goto _LL2122;} else{ goto
_LL2123;} _LL2123: if(( unsigned int) _temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_Goto_s:
0){ _LL2174: _temp2173=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp2105)->f1; goto _LL2172; _LL2172: _temp2171=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp2105)->f2; goto _LL2124;} else{ goto
_LL2125;} _LL2125: if(( unsigned int) _temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_For_s:
0){ _LL2190: _temp2189=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp2105)->f1; goto _LL2184; _LL2184: _temp2183=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp2105)->f2; _LL2188: _temp2187= _temp2183.f1; goto _LL2186; _LL2186:
_temp2185= _temp2183.f2; goto _LL2178; _LL2178: _temp2177=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp2105)->f3; _LL2182: _temp2181= _temp2177.f1;
goto _LL2180; _LL2180: _temp2179= _temp2177.f2; goto _LL2176; _LL2176: _temp2175=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp2105)->f4; goto
_LL2126;} else{ goto _LL2127;} _LL2127: if(( unsigned int) _temp2105 > 1u?*((
int*) _temp2105) == Cyc_Absyn_Switch_s: 0){ _LL2194: _temp2193=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp2105)->f1; goto _LL2192; _LL2192:
_temp2191=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp2105)->f2; goto _LL2128;} else{ goto _LL2129;} _LL2129: if(( unsigned int)
_temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_Fallthru_s: 0){ _LL2198:
_temp2197=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2105)->f1; goto _LL2196; _LL2196: _temp2195=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp2105)->f2; goto _LL2130;} else{ goto
_LL2131;} _LL2131: if(( unsigned int) _temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_Decl_s:
0){ _LL2202: _temp2201=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp2105)->f1; goto _LL2200; _LL2200: _temp2199=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp2105)->f2; goto _LL2132;} else{ goto
_LL2133;} _LL2133: if(( unsigned int) _temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_Cut_s:
0){ _LL2204: _temp2203=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp2105)->f1; goto _LL2134;} else{ goto _LL2135;} _LL2135: if(( unsigned int)
_temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_Splice_s: 0){ _LL2206:
_temp2205=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp2105)->f1; goto _LL2136;} else{ goto _LL2137;} _LL2137: if(( unsigned int)
_temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_Label_s: 0){ _LL2210: _temp2209=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp2105)->f1; goto
_LL2208; _LL2208: _temp2207=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp2105)->f2; goto _LL2138;} else{ goto _LL2139;} _LL2139: if(( unsigned int)
_temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_Do_s: 0){ _LL2218: _temp2217=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp2105)->f1; goto
_LL2212; _LL2212: _temp2211=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp2105)->f2; _LL2216: _temp2215= _temp2211.f1; goto _LL2214; _LL2214:
_temp2213= _temp2211.f2; goto _LL2140;} else{ goto _LL2141;} _LL2141: if((
unsigned int) _temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_TryCatch_s: 0){
_LL2222: _temp2221=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2105)->f1; goto _LL2220; _LL2220: _temp2219=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp2105)->f2; goto _LL2142;} else{ goto
_LL2143;} _LL2143: if(( unsigned int) _temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_Region_s:
0){ _LL2228: _temp2227=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2105)->f1; goto _LL2226; _LL2226: _temp2225=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp2105)->f2; goto _LL2224; _LL2224:
_temp2223=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2105)->f3; goto _LL2144;} else{ goto _LL2106;} _LL2108: return; _LL2110:
Cyc_Toc_exp_to_c( nv, _temp2145); return; _LL2112: Cyc_Toc_stmt_to_c( nv,
_temp2149); s= _temp2147; continue; _LL2114: { struct Cyc_Core_Opt* topt= 0; if(
_temp2151 != 0){ topt=({ struct Cyc_Core_Opt* _temp2229=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2229->v=( void*) Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp2231=({ struct Cyc_Absyn_Exp* _temp2230=
_temp2151; if( _temp2230 == 0){ _throw( Null_Exception);} _temp2230->topt;});
if( _temp2231 == 0){ _throw( Null_Exception);} _temp2231->v;})); _temp2229;});
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2232=
_temp2151; if( _temp2232 == 0){ _throw( Null_Exception);} _temp2232;}));} if( s->try_depth
> 0){ if( topt != 0){ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
retn_stmt= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x,
0), 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( x,( void*)({
struct Cyc_Core_Opt* _temp2233= topt; if( _temp2233 == 0){ _throw(
Null_Exception);} _temp2233->v;}), _temp2151, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), retn_stmt, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL2116: Cyc_Toc_exp_to_c( nv, _temp2157); Cyc_Toc_stmt_to_c( nv,
_temp2155); s= _temp2153; continue; _LL2118: Cyc_Toc_exp_to_c( nv, _temp2165);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2159); return; _LL2120: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)({ struct Cyc_Core_Opt* _temp2234= nv->break_lab; if( _temp2234
== 0){ _throw( Null_Exception);} _temp2234->v;}), 0))->r));}{ int dest_depth=
_temp2167 == 0? 0:({ struct Cyc_Absyn_Stmt* _temp2235= _temp2167; if( _temp2235
== 0){ _throw( Null_Exception);} _temp2235->try_depth;}); Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2122: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)({
struct Cyc_Core_Opt* _temp2236= nv->continue_lab; if( _temp2236 == 0){ _throw(
Null_Exception);} _temp2236->v;}), 0))->r));} Cyc_Toc_do_npop_before( s->try_depth
-({ struct Cyc_Absyn_Stmt* _temp2237= _temp2169; if( _temp2237 == 0){ _throw(
Null_Exception);} _temp2237->try_depth;}), s); return; _LL2124: Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2238= _temp2171; if( _temp2238 == 0){
_throw( Null_Exception);} _temp2238->try_depth;}), s); return; _LL2126: Cyc_Toc_exp_to_c(
nv, _temp2189); Cyc_Toc_exp_to_c( nv, _temp2187); Cyc_Toc_exp_to_c( nv,
_temp2181); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2175); return;
_LL2128: Cyc_Toc_xlate_switch( nv, s, _temp2193, _temp2191); return; _LL2130:
if( nv->fallthru_info == 0){( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2239=( char*)"fallthru in unexpected place";
struct _tagged_string _temp2240; _temp2240.curr= _temp2239; _temp2240.base=
_temp2239; _temp2240.last_plus_one= _temp2239 + 29; _temp2240;})));}{ struct
_tuple8 _temp2244; struct Cyc_Dict_Dict* _temp2245; struct Cyc_List_List*
_temp2247; struct _tagged_string* _temp2249; struct _tuple8* _temp2242=( struct
_tuple8*)({ struct Cyc_Core_Opt* _temp2241= nv->fallthru_info; if( _temp2241 ==
0){ _throw( Null_Exception);} _temp2241->v;}); _temp2244=* _temp2242; _LL2250:
_temp2249= _temp2244.f1; goto _LL2248; _LL2248: _temp2247= _temp2244.f2; goto
_LL2246; _LL2246: _temp2245= _temp2244.f3; goto _LL2243; _LL2243: { struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_goto_stmt( _temp2249, 0); Cyc_Toc_do_npop_before( s->try_depth -(({
struct Cyc_Absyn_Switch_clause** _temp2251= _temp2195; if( _temp2251 == 0){
_throw( Null_Exception);}* _temp2251;})->body)->try_depth, s2);{ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2247); struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2197); for( 0; vs2 != 0; vs2=({ struct Cyc_List_List*
_temp2252= vs2; if( _temp2252 == 0){ _throw( Null_Exception);} _temp2252->tl;}),
es2=({ struct Cyc_List_List* _temp2253= es2; if( _temp2253 == 0){ _throw(
Null_Exception);} _temp2253->tl;})){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp2254= es2; if( _temp2254 == 0){ _throw(
Null_Exception);} _temp2254->hd;})); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2245,( struct _tuple0*)({ struct Cyc_List_List* _temp2255= vs2; if(
_temp2255 == 0){ _throw( Null_Exception);} _temp2255->hd;})),( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp2256= es2; if( _temp2256 == 0){ _throw(
Null_Exception);} _temp2256->hd;}), 0), s2, 0);}( void*)( s->r=( void*)(( void*)
s2->r)); return;}}} _LL2132:{ void* _temp2257=( void*) _temp2201->r; int
_temp2265; struct Cyc_Absyn_Exp* _temp2267; struct Cyc_Core_Opt* _temp2269;
struct Cyc_Core_Opt* _temp2271; struct Cyc_Absyn_Pat* _temp2273; struct Cyc_Absyn_Vardecl*
_temp2275; _LL2259: if(*(( int*) _temp2257) == Cyc_Absyn_Let_d){ _LL2274:
_temp2273=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2257)->f1;
goto _LL2272; _LL2272: _temp2271=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2257)->f2; goto _LL2270; _LL2270: _temp2269=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2257)->f3; goto _LL2268; _LL2268: _temp2267=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2257)->f4; goto
_LL2266; _LL2266: _temp2265=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2257)->f5;
goto _LL2260;} else{ goto _LL2261;} _LL2261: if(*(( int*) _temp2257) == Cyc_Absyn_Var_d){
_LL2276: _temp2275=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2257)->f1; goto _LL2262;} else{ goto _LL2263;} _LL2263: goto _LL2264;
_LL2260: { int var_case= 0; struct Cyc_Absyn_Vardecl* vdopt= 0;{ void* _temp2277=(
void*) _temp2273->r; struct Cyc_Absyn_Vardecl* _temp2283; _LL2279: if((
unsigned int) _temp2277 > 2u?*(( int*) _temp2277) == Cyc_Absyn_Var_p: 0){
_LL2284: _temp2283=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp2277)->f1; goto _LL2280;} else{ goto _LL2281;} _LL2281: goto _LL2282;
_LL2280: var_case= 1; vdopt=( struct Cyc_Absyn_Vardecl*) _temp2283; _temp2283->initializer=(
struct Cyc_Absyn_Exp*) _temp2267;( void*)( _temp2201->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2285=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2285[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2286; _temp2286.tag= Cyc_Absyn_Var_d; _temp2286.f1= _temp2283; _temp2286;});
_temp2285;}))); goto _LL2278; _LL2282: goto _LL2278; _LL2278:;} if( var_case){
_temp2275=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_Absyn_Vardecl* _temp2287=
vdopt; if( _temp2287 == 0){ _throw( Null_Exception);} _temp2287;}); goto _LL2262;}(
void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2273, _temp2269,
_temp2267, _temp2265, _temp2199))->r)); goto _LL2258;} _LL2262: { struct _tuple0*
c_name= _temp2275->name;( void*)( _temp2275->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2275->type));{ struct Cyc_Toc_Env* nv2= Cyc_Toc_add_varmap( nv,
c_name, Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2301=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2301[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2302; _temp2302.tag= Cyc_Absyn_Local_b;
_temp2302.f1= _temp2275; _temp2302;}); _temp2301;}), 0)); Cyc_Toc_stmt_to_c( nv2,
_temp2199); if( _temp2275->initializer != 0){ struct Cyc_Absyn_Exp* init=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2300= _temp2275->initializer;
if( _temp2300 == 0){ _throw( Null_Exception);} _temp2300;}); void* _temp2288=(
void*) init->r; struct Cyc_Absyn_Exp* _temp2294; struct Cyc_Absyn_Exp* _temp2296;
struct Cyc_Absyn_Vardecl* _temp2298; _LL2290: if(*(( int*) _temp2288) == Cyc_Absyn_Comprehension_e){
_LL2299: _temp2298=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2288)->f1; goto _LL2297; _LL2297: _temp2296=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2288)->f2; goto _LL2295; _LL2295:
_temp2294=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2288)->f3; goto _LL2291;} else{ goto _LL2292;} _LL2292: goto _LL2293;
_LL2291: _temp2275->initializer= 0;( void*)( _temp2199->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( c_name, 0), _temp2298, _temp2296, _temp2294, Cyc_Absyn_new_stmt((
void*) _temp2199->r, 0), 0))->r)); goto _LL2289; _LL2293: Cyc_Toc_exp_to_c( nv,
init); goto _LL2289; _LL2289:;} goto _LL2258;}} _LL2264:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2303=(
char*)"bad nested declaration within function"; struct _tagged_string _temp2304;
_temp2304.curr= _temp2303; _temp2304.base= _temp2303; _temp2304.last_plus_one=
_temp2303 + 39; _temp2304;})); goto _LL2258; _LL2258:;} return; _LL2134:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp2305=( char*)"cut"; struct _tagged_string _temp2306; _temp2306.curr=
_temp2305; _temp2306.base= _temp2305; _temp2306.last_plus_one= _temp2305 + 4;
_temp2306;})); return; _LL2136:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2307=( char*)"splice"; struct _tagged_string
_temp2308; _temp2308.curr= _temp2307; _temp2308.base= _temp2307; _temp2308.last_plus_one=
_temp2307 + 7; _temp2308;})); return; _LL2138: s= _temp2207; continue; _LL2140:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2217); Cyc_Toc_exp_to_c( nv,
_temp2215); return; _LL2142: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct
_tuple0* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2324=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2324->f1= Cyc_Toc_abs_ns; _temp2324->f2= Cyc_Toc__push_handler_sp;
_temp2324;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple0* _temp2323=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2323->f1= Cyc_Toc_abs_ns; _temp2323->f2= Cyc_Toc_setjmp_sp; _temp2323;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2322=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2322->f1=
Cyc_Toc_abs_ns; _temp2322->f2= Cyc_Toc__pop_handler_sp; _temp2322;}), 0); e_exp->topt=({
struct Cyc_Core_Opt* _temp2309=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2309->v=( void*) e_typ; _temp2309;}); nv= Cyc_Toc_add_varmap(
nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2221);{ struct Cyc_Absyn_Stmt*
tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp2221, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0); struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl* x_vd= Cyc_Absyn_new_vardecl(
x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt* _temp2310=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2310->v=( void*)
Cyc_Absyn_exn_typ; _temp2310;});{ struct Cyc_Absyn_Pat* p=({ struct Cyc_Absyn_Pat*
_temp2318=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof( struct Cyc_Absyn_Pat));
_temp2318->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp2320=(
struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2320[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2321; _temp2321.tag= Cyc_Absyn_Var_p;
_temp2321.f1= x_vd; _temp2321;}); _temp2320;})); _temp2318->topt=({ struct Cyc_Core_Opt*
_temp2319=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2319->v=( void*) Cyc_Absyn_exn_typ; _temp2319;}); _temp2318->loc= 0;
_temp2318;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp2311=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2311->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2311;});{
struct Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp2315=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2315->pattern= p;
_temp2315->pat_vars=({ struct Cyc_Core_Opt* _temp2316=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2316->v=( void*)({ struct Cyc_List_List*
_temp2317=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2317->hd=( void*) x_vd; _temp2317->tl= 0; _temp2317;}); _temp2316;});
_temp2315->where_clause= 0; _temp2315->body= rs; _temp2315->loc= 0; _temp2315;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2219,({
struct Cyc_List_List* _temp2314=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2314->hd=( void*) default_case; _temp2314->tl= 0;
_temp2314;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( e_typ, Cyc_Absyn_fncall_exp( setjmp_exp,({
struct Cyc_List_List* _temp2313=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2313->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2313->tl= 0; _temp2313;}), 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List*
_temp2312=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2312->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2312->tl= 0;
_temp2312;}), 0), 0), Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Not,
e_exp, 0), tryandpop_stmt, handler_stmt, 0), 0), 0), 0))->r)); return;}}}}}
_LL2144: { void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__region_handle_sp);
void* rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq);
void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct _tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0*
x_var= _temp2225->name; struct _tuple0* h_var= Cyc_Toc_temp_var(); struct
_tuple0* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2333=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2333[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2334; _temp2334.tag= Cyc_Absyn_Local_b; _temp2334.f1=
_temp2225; _temp2334;}); _temp2333;}), 0); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp*
pushhandler_exp= Cyc_Absyn_var_exp(({ struct _tuple0* _temp2332=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2332->f1= Cyc_Toc_abs_ns; _temp2332->f2=
Cyc_Toc__push_handler_sp; _temp2332;}), 0); struct Cyc_Absyn_Exp* setjmp_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2331=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2331->f1= Cyc_Toc_abs_ns; _temp2331->f2= Cyc_Toc_setjmp_sp;
_temp2331;}), 0); struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2330=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2330->f1= Cyc_Toc_abs_ns; _temp2330->f2= Cyc_Toc__pop_handler_sp; _temp2330;}),
0); struct Cyc_Absyn_Exp* freeregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2329=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2329->f1=
Cyc_Toc_abs_ns; _temp2329->f2= Cyc_Toc__free_region_sp; _temp2329;}), 0); struct
Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0* _temp2328=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2328->f1= Cyc_Toc_abs_ns;
_temp2328->f2= Cyc_Toc__new_region_sp; _temp2328;}), 0); struct Cyc_Absyn_Exp*
addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2223);{ struct Cyc_Absyn_Stmt*
free_region_stmt= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( freeregion_exp,({
struct Cyc_List_List* _temp2327=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2327->hd=( void*) addr_rh_exp; _temp2327->tl= 0;
_temp2327;}), 0), 0); struct Cyc_Absyn_Stmt* tryandpop_stmt= Cyc_Absyn_seq_stmt(
_temp2223, Cyc_Absyn_seq_stmt( free_region_stmt, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0), 0); struct Cyc_Absyn_Stmt* rethrow_stmt= Cyc_Absyn_seq_stmt(
free_region_stmt, Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( e_exp), 0), 0);
struct Cyc_Absyn_Stmt* if_stmt= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, e_exp, 0), tryandpop_stmt, rethrow_stmt, 0); struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ), Cyc_Absyn_fncall_exp(
setjmp_exp,({ struct Cyc_List_List* _temp2326=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2326->hd=( void*) Cyc_Absyn_structmember_exp(
h_exp, Cyc_Toc_handler_sp, 0); _temp2326->tl= 0; _temp2326;}), 0), 0); struct
Cyc_Absyn_Stmt* push_stmt= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pushhandler_exp,({ struct Cyc_List_List* _temp2325=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2325->hd=( void*) Cyc_Absyn_address_exp(
h_exp, 0); _temp2325->tl= 0; _temp2325;}), 0), 0);( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
push_stmt, Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, if_stmt, 0), 0), 0), 0), 0))->r)); return;}} _LL2106:;}} struct
_tuple17{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* args= f->args; for( 0; args != 0;
args=({ struct Cyc_List_List* _temp2335= args; if( _temp2335 == 0){ _throw(
Null_Exception);} _temp2335->tl;})){(*(( struct _tuple17*)({ struct Cyc_List_List*
_temp2336= args; if( _temp2336 == 0){ _throw( Null_Exception);} _temp2336->hd;}))).f3=
Cyc_Toc_typ_to_c((*(( struct _tuple17*)({ struct Cyc_List_List* _temp2337= args;
if( _temp2337 == 0){ _throw( Null_Exception);} _temp2337->hd;}))).f3);{ struct
_tuple0* x=({ struct _tuple0* _temp2338=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2338->f1=( void*) Cyc_Absyn_Loc_n; _temp2338->f2=(*((
struct _tuple17*)({ struct Cyc_List_List* _temp2339= args; if( _temp2339 == 0){
_throw( Null_Exception);} _temp2339->hd;}))).f1; _temp2338;}); nv= Cyc_Toc_add_varmap(
nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2340= s;
_LL2342: if( _temp2340 ==( void*) Cyc_Absyn_Abstract){ goto _LL2343;} else{ goto
_LL2344;} _LL2344: if( _temp2340 ==( void*) Cyc_Absyn_ExternC){ goto _LL2345;}
else{ goto _LL2346;} _LL2346: goto _LL2347; _LL2343: return( void*) Cyc_Absyn_Public;
_LL2345: return( void*) Cyc_Absyn_Extern; _LL2347: return s; _LL2341:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* n=( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2360= s->name; if( _temp2360 == 0){ _throw( Null_Exception);}
_temp2360->v;}); if( Cyc_Toc_structs_so_far == 0){ Cyc_Toc_structs_so_far=({
struct Cyc_Core_Opt* _temp2348=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2348->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2348;});}{ int seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2359= Cyc_Toc_structs_so_far;
if( _temp2359 == 0){ _throw( Null_Exception);} _temp2359->v;}),(* n).f2); if(
dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2349= Cyc_Toc_structs_so_far; if( _temp2349 == 0){ _throw( Null_Exception);}
_temp2349->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2350= Cyc_Toc_structs_so_far;
if( _temp2350 == 0){ _throw( Null_Exception);} _temp2350->v;}),(* n).f2, s);});}
else{ if((( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Core_Opt* _temp2351= dopt;
if( _temp2351 == 0){ _throw( Null_Exception);} _temp2351->v;}))->fields == 0){(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2352= Cyc_Toc_structs_so_far;
if( _temp2352 == 0){ _throw( Null_Exception);} _temp2352->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2353= Cyc_Toc_structs_so_far; if( _temp2353 == 0){ _throw(
Null_Exception);} _temp2353->v;}),(* n).f2, s);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) s->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) s->sc))); s->tvs= 0; s->name=({
struct Cyc_Core_Opt* _temp2354=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2354->v=( void*) n; _temp2354;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2358= s->fields; if( _temp2358 == 0){
_throw( Null_Exception);} _temp2358->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2355= fields; if( _temp2355 == 0){ _throw( Null_Exception);}
_temp2355->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2356= fields; if( _temp2356 == 0){ _throw( Null_Exception);} _temp2356->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2357= fields; if( _temp2357 == 0){ _throw( Null_Exception);}
_temp2357->hd;}))->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* n=( struct _tuple0*)({ struct Cyc_Core_Opt* _temp2373= u->name; if(
_temp2373 == 0){ _throw( Null_Exception);} _temp2373->v;}); if( Cyc_Toc_unions_so_far
== 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2361=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2361->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2361;});}{ int seen_defn_before; struct Cyc_Core_Opt*
dopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2372= Cyc_Toc_unions_so_far; if( _temp2372 == 0){ _throw( Null_Exception);}
_temp2372->v;}),(* n).f2); if( dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2362= Cyc_Toc_unions_so_far; if( _temp2362 == 0){
_throw( Null_Exception);} _temp2362->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2363= Cyc_Toc_unions_so_far; if( _temp2363 == 0){ _throw( Null_Exception);}
_temp2363->v;}),(* n).f2, u);});} else{ if((( struct Cyc_Absyn_Uniondecl*)({
struct Cyc_Core_Opt* _temp2364= dopt; if( _temp2364 == 0){ _throw(
Null_Exception);} _temp2364->v;}))->fields == 0){( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2365= Cyc_Toc_unions_so_far; if( _temp2365 == 0){
_throw( Null_Exception);} _temp2365->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2366= Cyc_Toc_unions_so_far; if( _temp2366 == 0){ _throw( Null_Exception);}
_temp2366->v;}),(* n).f2, u);}); seen_defn_before= 0;} else{ seen_defn_before= 1;}}(
void*)( u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*)
u->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2367=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2367->v=( void*) n; _temp2367;}); if( u->fields != 0){ if(
seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2371= u->fields; if( _temp2371 == 0){
_throw( Null_Exception);} _temp2371->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2368= fields; if( _temp2368 == 0){ _throw( Null_Exception);}
_temp2368->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2369= fields; if( _temp2369 == 0){ _throw( Null_Exception);} _temp2369->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2370= fields; if( _temp2370 == 0){ _throw( Null_Exception);}
_temp2370->hd;}))->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2374=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2374->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2374;});}{ struct
_tuple0* n= tud->name; if( tud->fields == 0){ return;} if((( int(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2375= Cyc_Toc_tunions_so_far; if( _temp2375 == 0){ _throw( Null_Exception);}
_temp2375->v;}), n)){ return;}( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2376= Cyc_Toc_tunions_so_far; if( _temp2376 == 0){ _throw( Null_Exception);}
_temp2376->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct
_tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2377= Cyc_Toc_tunions_so_far; if( _temp2377 == 0){ _throw( Null_Exception);}
_temp2377->v;}), n);});{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2404= tud->fields;
if( _temp2404 == 0){ _throw( Null_Exception);} _temp2404->v;}); for( 0; fields
!= 0; fields=({ struct Cyc_List_List* _temp2378= fields; if( _temp2378 == 0){
_throw( Null_Exception);} _temp2378->tl;})){ struct Cyc_Absyn_Tunionfield* f=(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2403= fields; if(
_temp2403 == 0){ _throw( Null_Exception);} _temp2403->hd;}); if( f->typs == 0){
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
tag_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2379; _temp2379.q_const=
1; _temp2379.q_volatile= 0; _temp2379.q_restrict= 0; _temp2379;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2380=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2380->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2381=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2381[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2382; _temp2382.tag= Cyc_Absyn_Var_d; _temp2382.f1= vd; _temp2382;});
_temp2381;}), 0); _temp2380->tl= Cyc_Toc_result_decls; _temp2380;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
box_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2383; _temp2383.q_const=
1; _temp2383.q_volatile= 0; _temp2383.q_restrict= 0; _temp2383;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2384=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2384->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2385=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2385[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2386; _temp2386.tag= Cyc_Absyn_Var_d; _temp2386.f1= vd; _temp2386;});
_temp2385;}), 0); _temp2384->tl= Cyc_Toc_result_decls; _temp2384;});{ struct Cyc_List_List*
fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts=({
struct Cyc_List_List* _temp2387= ts; if( _temp2387 == 0){ _throw( Null_Exception);}
_temp2387->tl;}), i ++){ struct _tagged_string* fname=( struct _tagged_string*)({
struct _tagged_string* _temp2392=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp2392[ 0]= xprintf("f%d", i); _temp2392;}); struct
Cyc_Absyn_Structfield* f=({ struct Cyc_Absyn_Structfield* _temp2389=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2389->name= fname;
_temp2389->tq=(*(( struct _tuple3*)({ struct Cyc_List_List* _temp2391= ts; if(
_temp2391 == 0){ _throw( Null_Exception);} _temp2391->hd;}))).f1; _temp2389->type=(
void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp2390= ts; if( _temp2390 == 0){ _throw( Null_Exception);} _temp2390->hd;}))).f2);
_temp2389->width= 0; _temp2389->attributes= 0; _temp2389;}); fs=({ struct Cyc_List_List*
_temp2388=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2388->hd=( void*) f; _temp2388->tl= fs; _temp2388;});}} fs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs); fs=({ struct Cyc_List_List*
_temp2393=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2393->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2394=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2394->name= Cyc_Toc_tag_sp;
_temp2394->tq= Cyc_Toc_mt_tq; _temp2394->type=( void*) Cyc_Absyn_sint_t;
_temp2394->width= 0; _temp2394->attributes= 0; _temp2394;}); _temp2393->tl= fs;
_temp2393;});{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp2398=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2398->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2398->name=({ struct Cyc_Core_Opt*
_temp2400=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2400->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2401=( char*)"_struct"; struct _tagged_string _temp2402; _temp2402.curr=
_temp2401; _temp2402.base= _temp2401; _temp2402.last_plus_one= _temp2401 + 8;
_temp2402;})); _temp2400;}); _temp2398->tvs= 0; _temp2398->fields=({ struct Cyc_Core_Opt*
_temp2399=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2399->v=( void*) fs; _temp2399;}); _temp2398->attributes= 0; _temp2398;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2395=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2395->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2396=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2396[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2397; _temp2397.tag= Cyc_Absyn_Struct_d;
_temp2397.f1= sd; _temp2397;}); _temp2396;}), 0); _temp2395->tl= Cyc_Toc_result_decls;
_temp2395;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2405=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2405->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2405;});} if( xd->fields
== 0){ return;}{ struct _tuple0* n= xd->name; struct Cyc_List_List* fs=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2439= xd->fields; if( _temp2439 == 0){
_throw( Null_Exception);} _temp2439->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp2406= fs; if( _temp2406 == 0){ _throw( Null_Exception);} _temp2406->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp2438= fs; if( _temp2438 == 0){ _throw( Null_Exception);} _temp2438->hd;});
struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_uint_exp(({
struct _tagged_string _temp2437=* fn;( unsigned int)( _temp2437.last_plus_one -
_temp2437.curr);}) + 4, 0); struct Cyc_Absyn_ArrayType_struct* tag_typ=({ struct
Cyc_Absyn_ArrayType_struct* _temp2435=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2435[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2436; _temp2436.tag= Cyc_Absyn_ArrayType;
_temp2436.f1=( void*) Cyc_Absyn_uchar_t; _temp2436.f2= Cyc_Toc_mt_tq; _temp2436.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp2436;}); _temp2435;}); struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) xd->sc !=( void*) Cyc_Absyn_Extern? !(( int(*)( struct
Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)({
struct Cyc_Core_Opt* _temp2407= Cyc_Toc_xtunion_fields_so_far; if( _temp2407 ==
0){ _throw( Null_Exception);} _temp2407->v;}), f->name): 0){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(({ int _temp2408= 0; int _temp2409= 0; int
_temp2410= 0; int _temp2411= 0; struct _tagged_string _temp2412=* fn; xprintf("%c%c%c%c%.*s",
_temp2408, _temp2409, _temp2410, _temp2411, _temp2412.last_plus_one - _temp2412.curr,
_temp2412.curr);}), 0);}{ struct Cyc_Absyn_Vardecl* tag_decl= Cyc_Absyn_new_vardecl(
f->name,( void*) tag_typ, initopt);( void*)( tag_decl->sc=( void*)(( void*) xd->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2413=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2413->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2414=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2414[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2415; _temp2415.tag= Cyc_Absyn_Var_d; _temp2415.f1= tag_decl; _temp2415;});
_temp2414;}), 0); _temp2413->tl= Cyc_Toc_result_decls; _temp2413;}); if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)({
struct Cyc_Core_Opt* _temp2416= Cyc_Toc_xtunion_fields_so_far; if( _temp2416 ==
0){ _throw( Null_Exception);} _temp2416->v;}), f->name)){ continue;}( struct Cyc_Set_Set*)({
struct Cyc_Core_Opt* _temp2417= Cyc_Toc_xtunion_fields_so_far; if( _temp2417 ==
0){ _throw( Null_Exception);} _temp2417->v=( void*)(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)({
struct Cyc_Core_Opt* _temp2418= Cyc_Toc_xtunion_fields_so_far; if( _temp2418 ==
0){ _throw( Null_Exception);} _temp2418->v;}), f->name);}); if( f->typs != 0){
struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List* tqts= f->typs;
for( 0; tqts != 0; tqts=({ struct Cyc_List_List* _temp2419= tqts; if( _temp2419
== 0){ _throw( Null_Exception);} _temp2419->tl;}), i ++){ struct _tagged_string*
field_n=( struct _tagged_string*)({ struct _tagged_string* _temp2424=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2424[ 0]=
xprintf("f%d", i); _temp2424;}); struct Cyc_Absyn_Structfield* newf=({ struct
Cyc_Absyn_Structfield* _temp2421=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp2421->name= field_n; _temp2421->tq=(*((
struct _tuple3*)({ struct Cyc_List_List* _temp2423= tqts; if( _temp2423 == 0){
_throw( Null_Exception);} _temp2423->hd;}))).f1; _temp2421->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)({ struct Cyc_List_List* _temp2422= tqts; if( _temp2422 == 0){
_throw( Null_Exception);} _temp2422->hd;}))).f2); _temp2421->width= 0; _temp2421->attributes=
0; _temp2421;}); fields=({ struct Cyc_List_List* _temp2420=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2420->hd=( void*) newf;
_temp2420->tl= fields; _temp2420;});}} fields=({ struct Cyc_List_List* _temp2425=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2425->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp2426=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2426->name= Cyc_Toc_tag_sp;
_temp2426->tq= Cyc_Toc_mt_tq; _temp2426->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2426->width= 0; _temp2426->attributes= 0; _temp2426;});
_temp2425->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2425;});{ struct Cyc_Absyn_Structdecl* strct_decl=({ struct Cyc_Absyn_Structdecl*
_temp2430=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2430->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2430->name=({ struct Cyc_Core_Opt*
_temp2432=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2432->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2433=( char*)"_struct"; struct _tagged_string _temp2434; _temp2434.curr=
_temp2433; _temp2434.base= _temp2433; _temp2434.last_plus_one= _temp2433 + 8;
_temp2434;})); _temp2432;}); _temp2430->tvs= 0; _temp2430->fields=({ struct Cyc_Core_Opt*
_temp2431=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2431->v=( void*) fields; _temp2431;}); _temp2430->attributes= 0; _temp2430;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2427=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2427->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2428=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2428[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2429; _temp2429.tag= Cyc_Absyn_Struct_d;
_temp2429.f1= strct_decl; _temp2429;}); _temp2428;}), 0); _temp2427->tl= Cyc_Toc_result_decls;
_temp2427;});}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* n= ed->name;( void*)( ed->sc=(
void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) ed->sc ==( void*)
Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public:
Cyc_Toc_scope_to_c(( void*) ed->sc)));{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp2440= fs; if( _temp2440 == 0){
_throw( Null_Exception);} _temp2440->tl;})){ if((( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2441= fs; if( _temp2441 == 0){ _throw( Null_Exception);}
_temp2441->hd;}))->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2444=(( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2443= fs; if( _temp2443 == 0){ _throw( Null_Exception);}
_temp2443->hd;}))->tag; if( _temp2444 == 0){ _throw( Null_Exception);} _temp2444;}));((
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp2442= fs; if(
_temp2442 == 0){ _throw( Null_Exception);} _temp2442->hd;}))->tag=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)({
struct Cyc_Core_Opt* _temp2463= topt; if( _temp2463 == 0){ _throw(
Null_Exception);} _temp2463->v;}); Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x=
Cyc_Toc_temp_var(); struct _tagged_string* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_string* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();
struct Cyc_Absyn_Stmt* _temp2447; struct Cyc_List_List* _temp2449; struct Cyc_Toc_Env*
_temp2451; struct _tuple13 _temp2445= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL2452: _temp2451=
_temp2445.f1; goto _LL2450; _LL2450: _temp2449= _temp2445.f2; goto _LL2448;
_LL2448: _temp2447= _temp2445.f3; goto _LL2446; _LL2446: Cyc_Toc_stmt_to_c(
_temp2451, s);{ struct Cyc_Absyn_Stmt* succ_stmt= Cyc_Absyn_label_stmt( succ_lab,
s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2447, succ_stmt, 0), 0);} else{
struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({ struct
_tuple0* _temp2453=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2453->f1= Cyc_Toc_abs_ns; _temp2453->f2= Cyc_Toc_Match_Exception_sp;
_temp2453;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2447, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp2449 != 0; _temp2449=({ struct
Cyc_List_List* _temp2454= _temp2449; if( _temp2454 == 0){ _throw( Null_Exception);}
_temp2454->tl;})){ struct _tuple14 _temp2458; void* _temp2459; struct _tuple0*
_temp2461; struct _tuple14* _temp2456=( struct _tuple14*)({ struct Cyc_List_List*
_temp2455= _temp2449; if( _temp2455 == 0){ _throw( Null_Exception);} _temp2455->hd;});
_temp2458=* _temp2456; _LL2462: _temp2461= _temp2458.f1; goto _LL2460; _LL2460:
_temp2459= _temp2458.f2; goto _LL2457; _LL2457: s= Cyc_Absyn_declare_stmt(
_temp2461, _temp2459, 0, s, 0);} return s;}}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=({
struct Cyc_List_List* _temp2464= ds; if( _temp2464 == 0){ _throw( Null_Exception);}
_temp2464->tl;})){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct
Cyc_List_List* _temp2538= ds; if( _temp2538 == 0){ _throw( Null_Exception);}
_temp2538->hd;}); void* _temp2465=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2489; struct Cyc_Absyn_Fndecl* _temp2491; int _temp2493; struct Cyc_Absyn_Exp*
_temp2495; struct Cyc_Core_Opt* _temp2497; struct Cyc_Core_Opt* _temp2499;
struct Cyc_Absyn_Pat* _temp2501; struct Cyc_Absyn_Structdecl* _temp2503; struct
Cyc_Absyn_Uniondecl* _temp2505; struct Cyc_Absyn_Tuniondecl* _temp2507; struct
Cyc_Absyn_Enumdecl* _temp2509; struct Cyc_Absyn_Typedefdecl* _temp2511; struct
Cyc_List_List* _temp2513; struct _tagged_string* _temp2515; struct Cyc_List_List*
_temp2517; struct _tuple0* _temp2519; struct Cyc_List_List* _temp2521; _LL2467:
if(*(( int*) _temp2465) == Cyc_Absyn_Var_d){ _LL2490: _temp2489=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp2465)->f1; goto _LL2468;} else{ goto
_LL2469;} _LL2469: if(*(( int*) _temp2465) == Cyc_Absyn_Fn_d){ _LL2492:
_temp2491=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp2465)->f1;
goto _LL2470;} else{ goto _LL2471;} _LL2471: if(*(( int*) _temp2465) == Cyc_Absyn_Let_d){
_LL2502: _temp2501=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2465)->f1; goto _LL2500; _LL2500: _temp2499=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2465)->f2; goto _LL2498; _LL2498: _temp2497=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2465)->f3; goto
_LL2496; _LL2496: _temp2495=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2465)->f4; goto _LL2494; _LL2494: _temp2493=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp2465)->f5; goto _LL2472;} else{ goto _LL2473;} _LL2473: if(*(( int*)
_temp2465) == Cyc_Absyn_Struct_d){ _LL2504: _temp2503=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp2465)->f1; goto _LL2474;} else{ goto
_LL2475;} _LL2475: if(*(( int*) _temp2465) == Cyc_Absyn_Union_d){ _LL2506:
_temp2505=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp2465)->f1; goto _LL2476;} else{ goto _LL2477;} _LL2477: if(*(( int*)
_temp2465) == Cyc_Absyn_Tunion_d){ _LL2508: _temp2507=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp2465)->f1; goto _LL2478;} else{ goto
_LL2479;} _LL2479: if(*(( int*) _temp2465) == Cyc_Absyn_Enum_d){ _LL2510:
_temp2509=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp2465)->f1; goto _LL2480;} else{ goto _LL2481;} _LL2481: if(*(( int*)
_temp2465) == Cyc_Absyn_Typedef_d){ _LL2512: _temp2511=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp2465)->f1; goto _LL2482;} else{ goto
_LL2483;} _LL2483: if(*(( int*) _temp2465) == Cyc_Absyn_Namespace_d){ _LL2516:
_temp2515=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2465)->f1; goto _LL2514; _LL2514: _temp2513=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2465)->f2; goto _LL2484;} else{ goto
_LL2485;} _LL2485: if(*(( int*) _temp2465) == Cyc_Absyn_Using_d){ _LL2520:
_temp2519=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp2465)->f1;
goto _LL2518; _LL2518: _temp2517=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2465)->f2; goto _LL2486;} else{ goto _LL2487;} _LL2487: if(*(( int*)
_temp2465) == Cyc_Absyn_ExternC_d){ _LL2522: _temp2521=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp2465)->f1; goto _LL2488;} else{ goto
_LL2466;} _LL2468: { struct _tuple0* c_name= _temp2489->name; if(( void*)
_temp2489->sc ==( void*) Cyc_Absyn_ExternC){ c_name=({ struct _tuple0* _temp2523=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2523->f1= Cyc_Toc_abs_ns;
_temp2523->f2=(* c_name).f2; _temp2523;});} if( _temp2489->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2524=
_temp2489->initializer; if( _temp2524 == 0){ _throw( Null_Exception);} _temp2524;}));}
if( ! nv->toplevel){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp2525=( char*)"decls_to_c: not at toplevel!";
struct _tagged_string _temp2526; _temp2526.curr= _temp2525; _temp2526.base=
_temp2525; _temp2526.last_plus_one= _temp2525 + 29; _temp2526;}));} nv= Cyc_Toc_add_varmap(
nv, _temp2489->name, Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2527=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2527[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2528; _temp2528.tag= Cyc_Absyn_Global_b;
_temp2528.f1= _temp2489; _temp2528;}); _temp2527;}), 0)); _temp2489->name=
c_name;( void*)( _temp2489->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2489->sc));(
void*)( _temp2489->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2489->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2529=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2529->hd=( void*) d; _temp2529->tl=
Cyc_Toc_result_decls; _temp2529;}); goto _LL2466;} _LL2470: { struct _tuple0*
c_name= _temp2491->name; if(( void*) _temp2491->sc ==( void*) Cyc_Absyn_ExternC){
c_name=({ struct _tuple0* _temp2530=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp2530->f1= Cyc_Toc_abs_ns; _temp2530->f2=(* c_name).f2; _temp2530;});}
nv= Cyc_Toc_add_varmap( nv, _temp2491->name, Cyc_Absyn_var_exp( c_name, 0));
_temp2491->name= c_name; Cyc_Toc_fndecl_to_c( nv, _temp2491); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2531=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2531->hd=( void*) d; _temp2531->tl= Cyc_Toc_result_decls;
_temp2531;}); goto _LL2466;} _LL2472:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2532=( char*)"letdecl"; struct
_tagged_string _temp2533; _temp2533.curr= _temp2532; _temp2533.base= _temp2532;
_temp2533.last_plus_one= _temp2532 + 8; _temp2533;})); goto _LL2466; _LL2474:
Cyc_Toc_structdecl_to_c( _temp2503); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2534=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2534->hd=( void*) d; _temp2534->tl= Cyc_Toc_result_decls; _temp2534;});
goto _LL2466; _LL2476: Cyc_Toc_uniondecl_to_c( _temp2505); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2535=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2535->hd=( void*) d; _temp2535->tl= Cyc_Toc_result_decls;
_temp2535;}); goto _LL2466; _LL2478: if( _temp2507->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2507);} else{ Cyc_Toc_tuniondecl_to_c( _temp2507);} goto _LL2466; _LL2480:
Cyc_Toc_enumdecl_to_c( nv, _temp2509); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2536=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2536->hd=( void*) d; _temp2536->tl= Cyc_Toc_result_decls; _temp2536;});
goto _LL2466; _LL2482: _temp2511->name= _temp2511->name; _temp2511->tvs= 0;(
void*)( _temp2511->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2511->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2537=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2537->hd=( void*) d; _temp2537->tl=
Cyc_Toc_result_decls; _temp2537;}); goto _LL2466; _LL2484: _temp2517= _temp2513;
goto _LL2486; _LL2486: _temp2521= _temp2517; goto _LL2488; _LL2488: nv= Cyc_Toc_decls_to_c(
nv, _temp2521, top); goto _LL2466; _LL2466:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}