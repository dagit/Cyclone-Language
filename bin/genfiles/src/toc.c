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
0,.q_volatile= 0,.q_restrict= 0}; char Cyc_Toc_Toc_Unimplemented[ 22u]="Toc_Unimplemented";
char Cyc_Toc_Toc_Impossible[ 19u]="Toc_Impossible"; static void* Cyc_Toc_unimp(
struct _tagged_string s){({ struct _tagged_string _temp1= s; fprintf( Cyc_Stdio_stderr,"%.*s\n",
_temp1.last_plus_one - _temp1.curr, _temp1.curr);}); Cyc_Stdio_fflush( Cyc_Stdio_stderr);(
void) _throw(( void*) Cyc_Toc_Toc_Unimplemented);} static void* Cyc_Toc_toc_impos(
struct _tagged_string s){({ struct _tagged_string _temp2= s; fprintf( Cyc_Stdio_stderr,"%.*s\n",
_temp2.last_plus_one - _temp2.curr, _temp2.curr);}); Cyc_Stdio_fflush( Cyc_Stdio_stderr);(
void) _throw(( void*) Cyc_Toc_Toc_Impossible);} char Cyc_Toc_Match_error[ 16u]="Match_error";
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
Cyc_Toc__tagged_string_string; static char _temp17[ 15u]="_tunion_struct";
static struct _tagged_string Cyc_Toc__tunion_struct_string=( struct
_tagged_string){ _temp17, _temp17, _temp17 + 15u}; static struct _tagged_string*
Cyc_Toc__tunion_struct_sp=& Cyc_Toc__tunion_struct_string; static char _temp20[
16u]="_xtunion_struct"; static struct _tagged_string Cyc_Toc__xtunion_struct_string=(
struct _tagged_string){ _temp20, _temp20, _temp20 + 16u}; static struct
_tagged_string* Cyc_Toc__xtunion_struct_sp=& Cyc_Toc__xtunion_struct_string;
static char _temp23[ 2u]="v"; static struct _tagged_string Cyc_Toc_v_string=(
struct _tagged_string){ _temp23, _temp23, _temp23 + 2u}; static struct
_tagged_string* Cyc_Toc_v_sp=& Cyc_Toc_v_string; static char _temp26[ 4u]="tag";
static struct _tagged_string Cyc_Toc_tag_string=( struct _tagged_string){
_temp26, _temp26, _temp26 + 4u}; static struct _tagged_string* Cyc_Toc_tag_sp=&
Cyc_Toc_tag_string; static char _temp29[ 14u]="_handler_cons"; static struct
_tagged_string Cyc_Toc__handler_cons_string=( struct _tagged_string){ _temp29,
_temp29, _temp29 + 14u}; static struct _tagged_string* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static char _temp32[ 7u]="setjmp"; static struct
_tagged_string Cyc_Toc_setjmp_string=( struct _tagged_string){ _temp32, _temp32,
_temp32 + 7u}; static struct _tagged_string* Cyc_Toc_setjmp_sp=& Cyc_Toc_setjmp_string;
static char _temp35[ 14u]="_push_handler"; static struct _tagged_string Cyc_Toc__push_handler_string=(
struct _tagged_string){ _temp35, _temp35, _temp35 + 14u}; static struct
_tagged_string* Cyc_Toc__push_handler_sp=& Cyc_Toc__push_handler_string; static
char _temp38[ 13u]="_pop_handler"; static struct _tagged_string Cyc_Toc__pop_handler_string=(
struct _tagged_string){ _temp38, _temp38, _temp38 + 13u}; static struct
_tagged_string* Cyc_Toc__pop_handler_sp=& Cyc_Toc__pop_handler_string; static
char _temp41[ 8u]="handler"; static struct _tagged_string Cyc_Toc_handler_string=(
struct _tagged_string){ _temp41, _temp41, _temp41 + 8u}; static struct
_tagged_string* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static char _temp44[
14u]="_npop_handler"; static struct _tagged_string Cyc_Toc__npop_handler_string=(
struct _tagged_string){ _temp44, _temp44, _temp44 + 14u}; static struct _tuple0
Cyc_Toc__npop_handler_pr=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__npop_handler_string}; static struct _tuple0* Cyc_Toc__npop_handler_qv=&
Cyc_Toc__npop_handler_pr; static char _temp47[ 16u]="Match_Exception"; static
struct _tagged_string Cyc_Toc_Match_Exception_string=( struct _tagged_string){
_temp47, _temp47, _temp47 + 16u}; static struct _tagged_string* Cyc_Toc_Match_Exception_sp=&
Cyc_Toc_Match_Exception_string; static char _temp50[ 15u]="Cyc_new_string";
static struct _tagged_string Cyc_Toc_Cyc_new_string_string=( struct
_tagged_string){ _temp50, _temp50, _temp50 + 15u}; static struct _tagged_string*
Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string; static char _temp53[
16u]="Cyc_rnew_string"; static struct _tagged_string Cyc_Toc_Cyc_rnew_string_string=(
struct _tagged_string){ _temp53, _temp53, _temp53 + 16u}; static struct
_tagged_string* Cyc_Toc_Cyc_rnew_string_sp=& Cyc_Toc_Cyc_rnew_string_string;
static char _temp56[ 14u]="_RegionHandle"; static struct _tagged_string Cyc_Toc__region_handle_string=(
struct _tagged_string){ _temp56, _temp56, _temp56 + 14u}; static struct
_tagged_string* Cyc_Toc__region_handle_sp=& Cyc_Toc__region_handle_string;
static char _temp59[ 13u]="_free_region"; static struct _tagged_string Cyc_Toc__free_region_string=(
struct _tagged_string){ _temp59, _temp59, _temp59 + 13u}; static struct
_tagged_string* Cyc_Toc__free_region_sp=& Cyc_Toc__free_region_string; static
char _temp62[ 12u]="_new_region"; static struct _tagged_string Cyc_Toc__new_region_string=(
struct _tagged_string){ _temp62, _temp62, _temp62 + 12u}; static struct
_tagged_string* Cyc_Toc__new_region_sp=& Cyc_Toc__new_region_string; struct
_tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp* e){ void* _temp63=( void*) e->r; void* _temp89; char
_temp91; void* _temp93; void* _temp95; short _temp97; void* _temp99; void*
_temp101; int _temp103; void* _temp105; void* _temp107; long long _temp109; void*
_temp111; void* _temp113; struct Cyc_Absyn_Exp* _temp115; void* _temp117; struct
Cyc_List_List* _temp119; struct Cyc_List_List* _temp121; struct Cyc_Absyn_Structdecl*
_temp123; struct Cyc_List_List* _temp125; struct Cyc_Core_Opt* _temp127; struct
_tuple0* _temp129; struct Cyc_List_List* _temp131; struct _tuple1* _temp133;
struct Cyc_List_List* _temp135; struct Cyc_Core_Opt* _temp137; _LL65: if(((
struct _tunion_struct*) _temp63)->tag == Cyc_Absyn_Const_e){ _LL90: _temp89=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp63)->f1; if(( unsigned int)
_temp89 > 1u?(( struct _tunion_struct*) _temp89)->tag == Cyc_Absyn_Char_c: 0){
_LL94: _temp93=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp89)->f1; goto
_LL92; _LL92: _temp91=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp89)->f2;
goto _LL66;} else{ goto _LL67;}} else{ goto _LL67;} _LL67: if((( struct
_tunion_struct*) _temp63)->tag == Cyc_Absyn_Const_e){ _LL96: _temp95=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp63)->f1; if(( unsigned int) _temp95 > 1u?((
struct _tunion_struct*) _temp95)->tag == Cyc_Absyn_Short_c: 0){ _LL100: _temp99=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp95)->f1; goto _LL98; _LL98:
_temp97=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp95)->f2; goto _LL68;}
else{ goto _LL69;}} else{ goto _LL69;} _LL69: if((( struct _tunion_struct*)
_temp63)->tag == Cyc_Absyn_Const_e){ _LL102: _temp101=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp63)->f1; if(( unsigned int) _temp101 > 1u?(( struct _tunion_struct*)
_temp101)->tag == Cyc_Absyn_Int_c: 0){ _LL106: _temp105=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp101)->f1; goto _LL104; _LL104: _temp103=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp101)->f2; goto _LL70;} else{ goto _LL71;}} else{ goto _LL71;} _LL71: if(((
struct _tunion_struct*) _temp63)->tag == Cyc_Absyn_Const_e){ _LL108: _temp107=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp63)->f1; if(( unsigned int)
_temp107 > 1u?(( struct _tunion_struct*) _temp107)->tag == Cyc_Absyn_LongLong_c:
0){ _LL112: _temp111=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp107)->f1;
goto _LL110; _LL110: _temp109=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp107)->f2; goto _LL72;} else{ goto _LL73;}} else{ goto _LL73;} _LL73: if(((
struct _tunion_struct*) _temp63)->tag == Cyc_Absyn_Const_e){ _LL114: _temp113=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp63)->f1; if( _temp113 ==( void*)
Cyc_Absyn_Null_c){ goto _LL74;} else{ goto _LL75;}} else{ goto _LL75;} _LL75:
if((( struct _tunion_struct*) _temp63)->tag == Cyc_Absyn_Cast_e){ _LL118:
_temp117=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp63)->f1; goto _LL116;
_LL116: _temp115=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp63)->f2; goto _LL76;} else{ goto _LL77;} _LL77: if((( struct _tunion_struct*)
_temp63)->tag == Cyc_Absyn_Tuple_e){ _LL120: _temp119=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_e_struct*) _temp63)->f1; goto _LL78;} else{ goto _LL79;}
_LL79: if((( struct _tunion_struct*) _temp63)->tag == Cyc_Absyn_Array_e){ _LL122:
_temp121=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp63)->f1;
goto _LL80;} else{ goto _LL81;} _LL81: if((( struct _tunion_struct*) _temp63)->tag
== Cyc_Absyn_Struct_e){ _LL130: _temp129=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp63)->f1; goto _LL128; _LL128: _temp127=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp63)->f2; goto _LL126; _LL126: _temp125=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp63)->f3; goto _LL124; _LL124: _temp123=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp63)->f4; goto
_LL82;} else{ goto _LL83;} _LL83: if((( struct _tunion_struct*) _temp63)->tag ==
Cyc_Absyn_CompoundLit_e){ _LL134: _temp133=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp63)->f1; goto _LL132; _LL132: _temp131=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp63)->f2; goto _LL84;} else{ goto _LL85;}
_LL85: if((( struct _tunion_struct*) _temp63)->tag == Cyc_Absyn_UnresolvedMem_e){
_LL138: _temp137=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp63)->f1; goto _LL136; _LL136: _temp135=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp63)->f2; goto _LL86;} else{ goto _LL87;}
_LL87: goto _LL88; _LL66: return _temp91 =='\000'; _LL68: return _temp97 == 0;
_LL70: return _temp103 == 0; _LL72: return _temp109 == 0; _LL74: return 1; _LL76:
return Cyc_Toc_is_zero( _temp115); _LL78: return(( int(*)( int(* pred)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)( Cyc_Toc_is_zero,
_temp119); _LL80: _temp125= _temp121; goto _LL82; _LL82: _temp131= _temp125;
goto _LL84; _LL84: _temp135= _temp131; goto _LL86; _LL86: for( 0; _temp135 != 0;
_temp135=({ struct Cyc_List_List* _temp139= _temp135; if( _temp139 == 0){ _throw(
Null_Exception);} _temp139->tl;})){ if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)({
struct Cyc_List_List* _temp140= _temp135; if( _temp140 == 0){ _throw(
Null_Exception);} _temp140->hd;}))).f2)){ return 0;}} return 1; _LL88: return 0;
_LL64:;} static int Cyc_Toc_is_nullable( void* t){ void* _temp141= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp147; struct Cyc_Absyn_Conref* _temp149; struct
Cyc_Absyn_Tqual _temp151; struct Cyc_Absyn_Conref* _temp153; void* _temp155;
void* _temp157; _LL143: if(( unsigned int) _temp141 > 4u?(( struct
_tunion_struct*) _temp141)->tag == Cyc_Absyn_PointerType: 0){ _LL148: _temp147=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp141)->f1;
_LL158: _temp157=( void*) _temp147.elt_typ; goto _LL156; _LL156: _temp155=( void*)
_temp147.rgn_typ; goto _LL154; _LL154: _temp153=( struct Cyc_Absyn_Conref*)
_temp147.nullable; goto _LL152; _LL152: _temp151=( struct Cyc_Absyn_Tqual)
_temp147.tq; goto _LL150; _LL150: _temp149=( struct Cyc_Absyn_Conref*) _temp147.bounds;
goto _LL144;} else{ goto _LL145;} _LL145: goto _LL146; _LL144: { void* _temp159=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp153))->v; int _temp165; _LL161: if(( unsigned int) _temp159 > 1u?(( struct
_tunion_struct*) _temp159)->tag == Cyc_Absyn_Eq_constr: 0){ _LL166: _temp165=(
int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp159)->f1; goto _LL162;} else{
goto _LL163;} _LL163: goto _LL164; _LL162: return _temp165; _LL164: return 0;
_LL160:;} _LL146:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp167=( char*)"is_nullable"; struct
_tagged_string _temp168; _temp168.curr= _temp167; _temp168.base= _temp167;
_temp168.last_plus_one= _temp167 + 12; _temp168;})); return 0; _LL142:;} static
struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct _tuple0* x, struct
_tagged_string tag){ return({ struct _tuple0* _temp169=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp169->f1=(* x).f1; _temp169->f2=({
struct _tagged_string* _temp170=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp170[ 0]= Cyc_String_strconcat(*(* x).f2, tag);
_temp170;}); _temp169;});} static void Cyc_Toc_add_tuple_decl( struct
_tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List* fs= 0;{ int
i= 1; for( 0; ts != 0; ts=({ struct Cyc_List_List* _temp171= ts; if( _temp171 ==
0){ _throw( Null_Exception);} _temp171->tl;}), i ++){ fs=({ struct Cyc_List_List*
_temp172=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp172->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp173=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp173->name=({ struct
_tagged_string* _temp175=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp175[ 0]= xprintf("f%d", i); _temp175;}); _temp173->tq=
Cyc_Toc_mt_tq; _temp173->type=( void*)(( void*)({ struct Cyc_List_List* _temp174=
ts; if( _temp174 == 0){ _throw( Null_Exception);} _temp174->hd;})); _temp173->width=
0; _temp173->attributes= 0; _temp173;}); _temp172->tl= fs; _temp172;});}} fs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);{
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp179=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp179->sc=( void*)(( void*) Cyc_Absyn_Public); _temp179->name=({ struct Cyc_Core_Opt*
_temp181=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp181->v=( void*)({ struct _tuple0* _temp182=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp182->f1= Cyc_Toc_abs_ns; _temp182->f2= x;
_temp182;}); _temp181;}); _temp179->tvs= 0; _temp179->fields=({ struct Cyc_Core_Opt*
_temp180=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp180->v=( void*) fs; _temp180;}); _temp179->attributes= 0; _temp179;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp176=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp176->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp177=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp177[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp178; _temp178.tag= Cyc_Absyn_Struct_d; _temp178.f1=
sd; _temp178;}); _temp177;}), 0); _temp176->tl= Cyc_Toc_result_decls; _temp176;});}}
static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x, void* t){
void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Structfield* f1=({ struct Cyc_Absyn_Structfield* _temp195=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp195->name= Cyc_Toc_curr_sp; _temp195->tq= Cyc_Toc_mt_tq; _temp195->type=(
void*) tptr; _temp195->width= 0; _temp195->attributes= 0; _temp195;}); struct
Cyc_Absyn_Structfield* f2=({ struct Cyc_Absyn_Structfield* _temp194=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp194->name= Cyc_Toc_base_sp;
_temp194->tq= Cyc_Toc_mt_tq; _temp194->type=( void*) tptr; _temp194->width= 0;
_temp194->attributes= 0; _temp194;}); struct Cyc_Absyn_Structfield* f3=({ struct
Cyc_Absyn_Structfield* _temp193=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp193->name= Cyc_Toc_last_plus_one_sp;
_temp193->tq= Cyc_Toc_mt_tq; _temp193->type=( void*) tptr; _temp193->width= 0;
_temp193->attributes= 0; _temp193;}); struct Cyc_List_List* fs=({ struct Cyc_List_List*
_temp190=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp190->hd=( void*) f1; _temp190->tl=({ struct Cyc_List_List* _temp191=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp191->hd=(
void*) f2; _temp191->tl=({ struct Cyc_List_List* _temp192=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp192->hd=( void*) f3; _temp192->tl=
0; _temp192;}); _temp191;}); _temp190;}); struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp186=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp186->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp186->name=({ struct Cyc_Core_Opt* _temp188=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp188->v=( void*)({ struct _tuple0*
_temp189=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp189->f1=
Cyc_Toc_abs_ns; _temp189->f2= x; _temp189;}); _temp188;}); _temp186->tvs= 0;
_temp186->fields=({ struct Cyc_Core_Opt* _temp187=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp187->v=( void*)(( struct Cyc_List_List*)
fs); _temp187;}); _temp186->attributes= 0; _temp186;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp183=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp183->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp184=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp184[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp185; _temp185.tag= Cyc_Absyn_Struct_d; _temp185.f1=
sd; _temp185;}); _temp184;}), 0); _temp183->tl= Cyc_Toc_result_decls; _temp183;});}
static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int Cyc_Toc_tuple_type_counter=
0; struct _tuple5{ struct _tagged_string* f1; struct Cyc_List_List* f2; } ;
static struct _tagged_string* Cyc_Toc_add_tuple_type( struct Cyc_List_List* tqs0){{
struct Cyc_List_List* tts= Cyc_Toc_tuple_types; for( 0; tts != 0; tts=({ struct
Cyc_List_List* _temp196= tts; if( _temp196 == 0){ _throw( Null_Exception);}
_temp196->tl;})){ struct _tuple5 _temp200; struct Cyc_List_List* _temp201;
struct _tagged_string* _temp203; struct _tuple5* _temp198=( struct _tuple5*)({
struct Cyc_List_List* _temp197= tts; if( _temp197 == 0){ _throw( Null_Exception);}
_temp197->hd;}); _temp200=* _temp198; _LL204: _temp203= _temp200.f1; goto _LL202;
_LL202: _temp201= _temp200.f2; goto _LL199; _LL199: { int okay= 1; struct Cyc_List_List*
tqs= tqs0; for( 0; tqs != 0? _temp201 != 0: 0; tqs=({ struct Cyc_List_List*
_temp205= tqs; if( _temp205 == 0){ _throw( Null_Exception);} _temp205->tl;}),
_temp201=({ struct Cyc_List_List* _temp206= _temp201; if( _temp206 == 0){ _throw(
Null_Exception);} _temp206->tl;})){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)({
struct Cyc_List_List* _temp207= tqs; if( _temp207 == 0){ _throw( Null_Exception);}
_temp207->hd;}))).f2,( void*)({ struct Cyc_List_List* _temp208= _temp201; if(
_temp208 == 0){ _throw( Null_Exception);} _temp208->hd;}))){ okay= 0; break;}}
if(( ! okay? 1: tqs != 0)? 1: _temp201 != 0){ continue;} return _temp203;}}}{
struct _tagged_string* x=({ struct _tagged_string* _temp211=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp211[ 0]=
xprintf("_tuple%d",( Cyc_Toc_tuple_type_counter ++)); _temp211;}); struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); Cyc_Toc_add_tuple_decl( x, ts);
Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp209=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp209->hd=( void*)({ struct
_tuple5* _temp210=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp210->f1= x; _temp210->f2= ts; _temp210;}); _temp209->tl= Cyc_Toc_tuple_types;
_temp209;}); return x;}} static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types=
0; static int Cyc_Toc_tagged_ptr_counter= 0; struct _tuple6{ struct
_tagged_string* f1; void* f2; } ; static struct _tagged_string* Cyc_Toc_add_tagged_ptr_type(
void* t, struct Cyc_Absyn_Tqual tq){{ void* _temp212= t; void* _temp218; void*
_temp220; _LL214: if(( unsigned int) _temp212 > 4u?(( struct _tunion_struct*)
_temp212)->tag == Cyc_Absyn_IntType: 0){ _LL221: _temp220=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp212)->f1; if( _temp220 ==( void*) Cyc_Absyn_Unsigned){ goto _LL219;} else{
goto _LL216;} _LL219: _temp218=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp212)->f2; if( _temp218 ==( void*) Cyc_Absyn_B1){ goto _LL215;} else{ goto
_LL216;}} else{ goto _LL216;} _LL216: goto _LL217; _LL215: return Cyc_Toc__tagged_string_sp;
_LL217: goto _LL213; _LL213:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts=({ struct Cyc_List_List* _temp222= tts; if( _temp222 == 0){
_throw( Null_Exception);} _temp222->tl;})){ struct _tuple6 _temp226; void*
_temp227; struct _tagged_string* _temp229; struct _tuple6* _temp224=( struct
_tuple6*)({ struct Cyc_List_List* _temp223= tts; if( _temp223 == 0){ _throw(
Null_Exception);} _temp223->hd;}); _temp226=* _temp224; _LL230: _temp229=
_temp226.f1; goto _LL228; _LL228: _temp227= _temp226.f2; goto _LL225; _LL225:
if( ! Cyc_Tcutil_unify( t, _temp227)){ continue;} return _temp229;}}{ struct
_tagged_string* x=({ struct _tagged_string* _temp233=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp233[ 0]= xprintf("_tagged_ptr%d",(
Cyc_Toc_tagged_ptr_counter ++)); _temp233;}); Cyc_Toc_add_tagged_ptr_decl( x, t);
Cyc_Toc_tagged_ptr_types=({ struct Cyc_List_List* _temp231=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp231->hd=( void*)({ struct
_tuple6* _temp232=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp232->f1= x; _temp232->f2= t; _temp232;}); _temp231->tl= Cyc_Toc_tagged_ptr_types;
_temp231;}); return x;}} static int Cyc_Toc_temp_var_counter= 0; static struct
_tuple0* Cyc_Toc_temp_var(){ return({ struct _tuple0* _temp234=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp234->f1= Cyc_Toc_abs_ns; _temp234->f2=({
struct _tagged_string* _temp235=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp235[ 0]= xprintf("_temp%d",( Cyc_Toc_temp_var_counter
++)); _temp235;}); _temp234;});} static int Cyc_Toc_fresh_label_counter= 0;
static struct _tagged_string* Cyc_Toc_fresh_label(){ return({ struct
_tagged_string* _temp236=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp236[ 0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter
++)); _temp236;});} static void* Cyc_Toc_typ_to_c( void* t); static struct
_tuple1* Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp239; struct Cyc_Absyn_Tqual
_temp241; struct Cyc_Core_Opt* _temp243; struct _tuple1 _temp237=* a; _LL244:
_temp243= _temp237.f1; goto _LL242; _LL242: _temp241= _temp237.f2; goto _LL240;
_LL240: _temp239= _temp237.f3; goto _LL238; _LL238: return({ struct _tuple1*
_temp245=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp245->f1=
_temp243; _temp245->f2= _temp241; _temp245->f3= Cyc_Toc_typ_to_c( _temp239);
_temp245;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp248; struct Cyc_Absyn_Tqual _temp250; struct _tuple3 _temp246=* x;
_LL251: _temp250= _temp246.f1; goto _LL249; _LL249: _temp248= _temp246.f2; goto
_LL247; _LL247: return({ struct _tuple3* _temp252=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp252->f1= _temp250; _temp252->f2= Cyc_Toc_typ_to_c(
_temp248); _temp252;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp253= t; struct Cyc_Absyn_Exp* _temp261; struct Cyc_Absyn_Tqual _temp263;
void* _temp265; int _temp267; struct Cyc_Core_Opt* _temp269; struct Cyc_Core_Opt
_temp271; void* _temp272; void* _temp274; _LL255: if(( unsigned int) _temp253 >
4u?(( struct _tunion_struct*) _temp253)->tag == Cyc_Absyn_ArrayType: 0){ _LL266:
_temp265=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp253)->f1; goto
_LL264; _LL264: _temp263=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp253)->f2; goto _LL262; _LL262: _temp261=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp253)->f3; goto _LL256;} else{ goto _LL257;}
_LL257: if(( unsigned int) _temp253 > 4u?(( struct _tunion_struct*) _temp253)->tag
== Cyc_Absyn_Evar: 0){ _LL275: _temp274=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp253)->f1; goto _LL270; _LL270: _temp269=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp253)->f2; if( _temp269 == 0){ goto _LL259;} else{
_temp271=* _temp269; _LL273: _temp272=( void*) _temp271.v; goto _LL268;} _LL268:
_temp267=( int)(( struct Cyc_Absyn_Evar_struct*) _temp253)->f3; goto _LL258;}
else{ goto _LL259;} _LL259: goto _LL260; _LL256: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp276=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp276[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp277; _temp277.tag= Cyc_Absyn_ArrayType;
_temp277.f1=( void*) Cyc_Toc_typ_to_c_array( _temp265); _temp277.f2= _temp263;
_temp277.f3= _temp261; _temp277;}); _temp276;}); _LL258: return Cyc_Toc_typ_to_c_array(
_temp272); _LL260: return Cyc_Toc_typ_to_c( t); _LL254:;} static void* Cyc_Toc_typ_to_c(
void* t){ void* _temp278= t; int _temp326; struct Cyc_Core_Opt* _temp328; void*
_temp330; int _temp332; struct Cyc_Core_Opt* _temp334; struct Cyc_Core_Opt
_temp336; void* _temp337; void* _temp339; struct Cyc_Absyn_Tvar* _temp341;
struct Cyc_Absyn_TunionInfo _temp343; void* _temp345; struct Cyc_List_List*
_temp347; void* _temp349; struct Cyc_Absyn_Tuniondecl* _temp351; struct Cyc_Absyn_TunionInfo
_temp353; struct Cyc_Absyn_TunionFieldInfo _temp355; struct Cyc_List_List*
_temp357; void* _temp359; struct Cyc_Absyn_Tunionfield* _temp361; struct Cyc_Absyn_Tuniondecl*
_temp363; struct Cyc_Absyn_TunionFieldInfo _temp365; struct Cyc_Absyn_PtrInfo
_temp367; struct Cyc_Absyn_Conref* _temp369; struct Cyc_Absyn_Tqual _temp371;
struct Cyc_Absyn_Conref* _temp373; void* _temp375; void* _temp377; void*
_temp379; void* _temp381; struct Cyc_Absyn_Exp* _temp383; struct Cyc_Absyn_Tqual
_temp385; void* _temp387; struct Cyc_Absyn_FnInfo _temp389; struct Cyc_List_List*
_temp391; int _temp393; struct Cyc_List_List* _temp395; void* _temp397; struct
Cyc_Core_Opt* _temp399; struct Cyc_List_List* _temp401; struct Cyc_List_List*
_temp403; struct Cyc_Absyn_Uniondecl** _temp405; struct Cyc_List_List* _temp407;
struct _tuple0* _temp409; struct Cyc_Absyn_Structdecl** _temp411; struct Cyc_List_List*
_temp413; struct _tuple0* _temp415; struct Cyc_Absyn_Enumdecl* _temp417; struct
_tuple0* _temp419; struct Cyc_Core_Opt* _temp421; struct Cyc_List_List* _temp423;
struct _tuple0* _temp425; void* _temp427; void* _temp429; struct Cyc_List_List*
_temp431; _LL280: if( _temp278 ==( void*) Cyc_Absyn_VoidType){ goto _LL281;}
else{ goto _LL282;} _LL282: if(( unsigned int) _temp278 > 4u?(( struct
_tunion_struct*) _temp278)->tag == Cyc_Absyn_Evar: 0){ _LL331: _temp330=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp278)->f1; goto _LL329; _LL329: _temp328=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp278)->f2; if(
_temp328 == 0){ goto _LL327;} else{ goto _LL284;} _LL327: _temp326=( int)((
struct Cyc_Absyn_Evar_struct*) _temp278)->f3; goto _LL283;} else{ goto _LL284;}
_LL284: if(( unsigned int) _temp278 > 4u?(( struct _tunion_struct*) _temp278)->tag
== Cyc_Absyn_Evar: 0){ _LL340: _temp339=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp278)->f1; goto _LL335; _LL335: _temp334=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp278)->f2; if( _temp334 == 0){ goto _LL286;} else{
_temp336=* _temp334; _LL338: _temp337=( void*) _temp336.v; goto _LL333;} _LL333:
_temp332=( int)(( struct Cyc_Absyn_Evar_struct*) _temp278)->f3; goto _LL285;}
else{ goto _LL286;} _LL286: if(( unsigned int) _temp278 > 4u?(( struct
_tunion_struct*) _temp278)->tag == Cyc_Absyn_VarType: 0){ _LL342: _temp341=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp278)->f1; goto
_LL287;} else{ goto _LL288;} _LL288: if(( unsigned int) _temp278 > 4u?(( struct
_tunion_struct*) _temp278)->tag == Cyc_Absyn_TunionType: 0){ _LL344: _temp343=(
struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*) _temp278)->f1;
_LL350: _temp349=( void*) _temp343.tunion_info; if((( struct _tunion_struct*)
_temp349)->tag == Cyc_Absyn_KnownTunion){ _LL352: _temp351=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp349)->f1; goto _LL348;} else{ goto
_LL290;} _LL348: _temp347=( struct Cyc_List_List*) _temp343.targs; goto _LL346;
_LL346: _temp345=( void*) _temp343.rgn; goto _LL289;} else{ goto _LL290;} _LL290:
if(( unsigned int) _temp278 > 4u?(( struct _tunion_struct*) _temp278)->tag ==
Cyc_Absyn_TunionType: 0){ _LL354: _temp353=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp278)->f1; goto _LL291;} else{ goto
_LL292;} _LL292: if(( unsigned int) _temp278 > 4u?(( struct _tunion_struct*)
_temp278)->tag == Cyc_Absyn_TunionFieldType: 0){ _LL356: _temp355=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp278)->f1; _LL360: _temp359=( void*)
_temp355.field_info; if((( struct _tunion_struct*) _temp359)->tag == Cyc_Absyn_KnownTunionfield){
_LL364: _temp363=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp359)->f1; goto _LL362; _LL362: _temp361=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp359)->f2; goto _LL358;} else{
goto _LL294;} _LL358: _temp357=( struct Cyc_List_List*) _temp355.targs; goto
_LL293;} else{ goto _LL294;} _LL294: if(( unsigned int) _temp278 > 4u?(( struct
_tunion_struct*) _temp278)->tag == Cyc_Absyn_TunionFieldType: 0){ _LL366:
_temp365=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp278)->f1; goto _LL295;} else{ goto _LL296;} _LL296: if(( unsigned int)
_temp278 > 4u?(( struct _tunion_struct*) _temp278)->tag == Cyc_Absyn_PointerType:
0){ _LL368: _temp367=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp278)->f1; _LL378: _temp377=( void*) _temp367.elt_typ; goto _LL376; _LL376:
_temp375=( void*) _temp367.rgn_typ; goto _LL374; _LL374: _temp373=( struct Cyc_Absyn_Conref*)
_temp367.nullable; goto _LL372; _LL372: _temp371=( struct Cyc_Absyn_Tqual)
_temp367.tq; goto _LL370; _LL370: _temp369=( struct Cyc_Absyn_Conref*) _temp367.bounds;
goto _LL297;} else{ goto _LL298;} _LL298: if(( unsigned int) _temp278 > 4u?((
struct _tunion_struct*) _temp278)->tag == Cyc_Absyn_IntType: 0){ _LL382:
_temp381=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp278)->f1; goto _LL380;
_LL380: _temp379=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp278)->f2;
goto _LL299;} else{ goto _LL300;} _LL300: if( _temp278 ==( void*) Cyc_Absyn_FloatType){
goto _LL301;} else{ goto _LL302;} _LL302: if( _temp278 ==( void*) Cyc_Absyn_DoubleType){
goto _LL303;} else{ goto _LL304;} _LL304: if(( unsigned int) _temp278 > 4u?((
struct _tunion_struct*) _temp278)->tag == Cyc_Absyn_ArrayType: 0){ _LL388:
_temp387=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp278)->f1; goto
_LL386; _LL386: _temp385=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp278)->f2; goto _LL384; _LL384: _temp383=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp278)->f3; goto _LL305;} else{ goto _LL306;}
_LL306: if(( unsigned int) _temp278 > 4u?(( struct _tunion_struct*) _temp278)->tag
== Cyc_Absyn_FnType: 0){ _LL390: _temp389=( struct Cyc_Absyn_FnInfo)(( struct
Cyc_Absyn_FnType_struct*) _temp278)->f1; _LL402: _temp401=( struct Cyc_List_List*)
_temp389.tvars; goto _LL400; _LL400: _temp399=( struct Cyc_Core_Opt*) _temp389.effect;
goto _LL398; _LL398: _temp397=( void*) _temp389.ret_typ; goto _LL396; _LL396:
_temp395=( struct Cyc_List_List*) _temp389.args; goto _LL394; _LL394: _temp393=(
int) _temp389.varargs; goto _LL392; _LL392: _temp391=( struct Cyc_List_List*)
_temp389.attributes; goto _LL307;} else{ goto _LL308;} _LL308: if(( unsigned int)
_temp278 > 4u?(( struct _tunion_struct*) _temp278)->tag == Cyc_Absyn_TupleType:
0){ _LL404: _temp403=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp278)->f1; goto _LL309;} else{ goto _LL310;} _LL310: if(( unsigned int)
_temp278 > 4u?(( struct _tunion_struct*) _temp278)->tag == Cyc_Absyn_UnionType:
0){ _LL410: _temp409=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp278)->f1; goto _LL408; _LL408: _temp407=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp278)->f2; goto _LL406; _LL406: _temp405=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp278)->f3;
goto _LL311;} else{ goto _LL312;} _LL312: if(( unsigned int) _temp278 > 4u?((
struct _tunion_struct*) _temp278)->tag == Cyc_Absyn_StructType: 0){ _LL416:
_temp415=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp278)->f1;
goto _LL414; _LL414: _temp413=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp278)->f2; goto _LL412; _LL412: _temp411=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp278)->f3; goto _LL313;} else{ goto
_LL314;} _LL314: if(( unsigned int) _temp278 > 4u?(( struct _tunion_struct*)
_temp278)->tag == Cyc_Absyn_EnumType: 0){ _LL420: _temp419=( struct _tuple0*)((
struct Cyc_Absyn_EnumType_struct*) _temp278)->f1; goto _LL418; _LL418: _temp417=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp278)->f2;
goto _LL315;} else{ goto _LL316;} _LL316: if(( unsigned int) _temp278 > 4u?((
struct _tunion_struct*) _temp278)->tag == Cyc_Absyn_TypedefType: 0){ _LL426:
_temp425=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp278)->f1;
goto _LL424; _LL424: _temp423=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp278)->f2; goto _LL422; _LL422: _temp421=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp278)->f3; goto _LL317;} else{ goto _LL318;}
_LL318: if(( unsigned int) _temp278 > 4u?(( struct _tunion_struct*) _temp278)->tag
== Cyc_Absyn_RgnHandleType: 0){ _LL428: _temp427=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp278)->f1; goto _LL319;} else{ goto _LL320;} _LL320: if( _temp278 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL321;} else{ goto _LL322;} _LL322: if(( unsigned int)
_temp278 > 4u?(( struct _tunion_struct*) _temp278)->tag == Cyc_Absyn_AccessEff:
0){ _LL430: _temp429=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp278)->f1;
goto _LL323;} else{ goto _LL324;} _LL324: if(( unsigned int) _temp278 > 4u?((
struct _tunion_struct*) _temp278)->tag == Cyc_Absyn_JoinEff: 0){ _LL432:
_temp431=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp278)->f1;
goto _LL325;} else{ goto _LL279;} _LL281: return t; _LL283: return( void*) Cyc_Absyn_VoidType;
_LL285: return Cyc_Toc_typ_to_c( _temp337); _LL287: return Cyc_Absyn_void_star_typ();
_LL289: return Cyc_Absyn_void_star_typ(); _LL291:( void) _throw((( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp433=( char*)"unresolved TunionType"; struct _tagged_string _temp434;
_temp434.curr= _temp433; _temp434.base= _temp433; _temp434.last_plus_one=
_temp433 + 22; _temp434;}))); _LL293: if( _temp361->typs == 0){ if( _temp363->is_xtunion){
return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);}
else{ return Cyc_Absyn_uint_t;}} else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp361->name,( struct _tagged_string)({ char* _temp435=( char*)"_struct";
struct _tagged_string _temp436; _temp436.curr= _temp435; _temp436.base= _temp435;
_temp436.last_plus_one= _temp435 + 8; _temp436;})));} _LL295:( void) _throw(((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp437=( char*)"unresolved TunionFieldType"; struct _tagged_string
_temp438; _temp438.curr= _temp437; _temp438.base= _temp437; _temp438.last_plus_one=
_temp437 + 27; _temp438;}))); _LL297: _temp377= Cyc_Toc_typ_to_c_array( _temp377);{
void* _temp439=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp369))->v; void* _temp445; _LL441: if((
unsigned int) _temp439 > 1u?(( struct _tunion_struct*) _temp439)->tag == Cyc_Absyn_Eq_constr:
0){ _LL446: _temp445=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp439)->f1;
if( _temp445 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL442;} else{ goto _LL443;}}
else{ goto _LL443;} _LL443: goto _LL444; _LL442: { struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
_temp377, _temp371); return Cyc_Absyn_strct( n);} _LL444: return Cyc_Absyn_star_typ(
_temp377,( void*) Cyc_Absyn_HeapRgn, _temp371); _LL440:;} _LL299: return t;
_LL301: return t; _LL303: return t; _LL305: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp447=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp447[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp448; _temp448.tag= Cyc_Absyn_ArrayType;
_temp448.f1=( void*) Cyc_Toc_typ_to_c_array( _temp387); _temp448.f2= _temp385;
_temp448.f3= _temp383; _temp448;}); _temp447;}); _LL307: { struct Cyc_List_List*
new_atts= 0; for( 0; _temp391 != 0; _temp391=({ struct Cyc_List_List* _temp449=
_temp391; if( _temp449 == 0){ _throw( Null_Exception);} _temp449->tl;})){ void*
_temp451=( void*)({ struct Cyc_List_List* _temp450= _temp391; if( _temp450 == 0){
_throw( Null_Exception);} _temp450->hd;}); _LL453: if( _temp451 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL454;} else{ goto _LL455;} _LL455: if( _temp451 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL456;} else{ goto _LL457;} _LL457: if( _temp451 ==( void*) Cyc_Absyn_Const_att){
goto _LL458;} else{ goto _LL459;} _LL459: goto _LL460; _LL454: goto _LL456;
_LL456: goto _LL458; _LL458: continue; _LL460: new_atts=({ struct Cyc_List_List*
_temp461=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp461->hd=( void*)(( void*)({ struct Cyc_List_List* _temp462= _temp391; if(
_temp462 == 0){ _throw( Null_Exception);} _temp462->hd;})); _temp461->tl=
new_atts; _temp461;}); goto _LL452; _LL452:;} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp463=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp463[ 0]=({ struct Cyc_Absyn_FnType_struct _temp464; _temp464.tag= Cyc_Absyn_FnType;
_temp464.f1=({ struct Cyc_Absyn_FnInfo _temp465; _temp465.tvars= 0; _temp465.effect=
0; _temp465.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp397); _temp465.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp395); _temp465.varargs= _temp393;
_temp465.attributes= new_atts; _temp465;}); _temp464;}); _temp463;});} _LL309:
_temp403=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp403);{ struct
_tagged_string* n= Cyc_Toc_add_tuple_type( _temp403); return Cyc_Absyn_strct( n);}
_LL311: if( _temp409 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp466=( char*)"anonymous union"; struct
_tagged_string _temp467; _temp467.curr= _temp466; _temp467.base= _temp466;
_temp467.last_plus_one= _temp466 + 16; _temp467;}));} return Cyc_Absyn_unionq_typ((
struct _tuple0*)({ struct _tuple0* _temp468= _temp409; if( _temp468 == 0){
_throw( Null_Exception);} _temp468;})); _LL313: if( _temp415 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp469=( char*)"anonymous struct"; struct _tagged_string _temp470; _temp470.curr=
_temp469; _temp470.base= _temp469; _temp470.last_plus_one= _temp469 + 17;
_temp470;}));} return Cyc_Absyn_strctq(( struct _tuple0*)({ struct _tuple0*
_temp471= _temp415; if( _temp471 == 0){ _throw( Null_Exception);} _temp471;}));
_LL315: return t; _LL317: if( _temp421 == 0){ return(( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp472=(
char*)"Toc::typ_to_c: unresolved TypedefType"; struct _tagged_string _temp473;
_temp473.curr= _temp472; _temp473.base= _temp472; _temp473.last_plus_one=
_temp472 + 38; _temp473;}));} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp474=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp474[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp475; _temp475.tag= Cyc_Absyn_TypedefType;
_temp475.f1= _temp425; _temp475.f2= 0; _temp475.f3=({ struct Cyc_Core_Opt*
_temp476=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp476->v=( void*) Cyc_Toc_typ_to_c_array(( void*)({ struct Cyc_Core_Opt*
_temp477= _temp421; if( _temp477 == 0){ _throw( Null_Exception);} _temp477->v;}));
_temp476;}); _temp475;}); _temp474;}); _LL319: return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__region_handle_sp), Cyc_Toc_mt_tq); _LL321: return(( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp478=(
char*)"Toc::typ_to_c: type translation passed the heap region"; struct
_tagged_string _temp479; _temp479.curr= _temp478; _temp479.base= _temp478;
_temp479.last_plus_one= _temp478 + 55; _temp479;})); _LL323: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp480=( char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp481; _temp481.curr= _temp480; _temp481.base= _temp480;
_temp481.last_plus_one= _temp480 + 54; _temp481;})); _LL325: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp482=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp483; _temp483.curr= _temp482; _temp483.base= _temp482;
_temp483.last_plus_one= _temp482 + 52; _temp483;})); _LL279:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp484= t; struct Cyc_Absyn_Exp* _temp490; struct Cyc_Absyn_Tqual
_temp492; void* _temp494; _LL486: if(( unsigned int) _temp484 > 4u?(( struct
_tunion_struct*) _temp484)->tag == Cyc_Absyn_ArrayType: 0){ _LL495: _temp494=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp484)->f1; goto _LL493; _LL493:
_temp492=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp484)->f2; goto _LL491; _LL491: _temp490=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp484)->f3; goto _LL487;} else{ goto _LL488;}
_LL488: goto _LL489; _LL487: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp494,( void*) Cyc_Absyn_HeapRgn, _temp492), e, l); _LL489: return e; _LL485:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* l){ void* _temp496= t; struct Cyc_Absyn_Exp*
_temp502; struct Cyc_Absyn_Tqual _temp504; void* _temp506; _LL498: if((
unsigned int) _temp496 > 4u?(( struct _tunion_struct*) _temp496)->tag == Cyc_Absyn_ArrayType:
0){ _LL507: _temp506=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp496)->f1;
goto _LL505; _LL505: _temp504=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp496)->f2; goto _LL503; _LL503: _temp502=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp496)->f3; goto _LL499;} else{ goto _LL500;}
_LL500: goto _LL501; _LL499: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp506,( void*) Cyc_Absyn_HeapRgn, _temp504), e, l); _LL501: return Cyc_Absyn_cast_exp(
t, e, l); _LL497:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp508= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp540; void* _temp542; void* _temp544; struct Cyc_Absyn_Exp*
_temp546; struct Cyc_Absyn_Tqual _temp548; void* _temp550; struct Cyc_Absyn_FnInfo
_temp552; struct Cyc_List_List* _temp554; struct Cyc_Absyn_Structdecl** _temp556;
struct Cyc_List_List* _temp558; struct _tuple0* _temp560; struct Cyc_Absyn_TunionInfo
_temp562; struct Cyc_Absyn_TunionFieldInfo _temp564; struct Cyc_List_List*
_temp566; void* _temp568; struct Cyc_Absyn_Tunionfield* _temp570; struct Cyc_Absyn_Tuniondecl*
_temp572; struct Cyc_Absyn_Uniondecl** _temp574; struct Cyc_List_List* _temp576;
struct _tuple0* _temp578; struct Cyc_Absyn_PtrInfo _temp580; void* _temp582;
_LL510: if( _temp508 ==( void*) Cyc_Absyn_VoidType){ goto _LL511;} else{ goto
_LL512;} _LL512: if(( unsigned int) _temp508 > 4u?(( struct _tunion_struct*)
_temp508)->tag == Cyc_Absyn_VarType: 0){ _LL541: _temp540=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp508)->f1; goto _LL513;} else{ goto _LL514;}
_LL514: if(( unsigned int) _temp508 > 4u?(( struct _tunion_struct*) _temp508)->tag
== Cyc_Absyn_IntType: 0){ _LL545: _temp544=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp508)->f1; goto _LL543; _LL543: _temp542=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp508)->f2; goto _LL515;} else{ goto _LL516;} _LL516: if( _temp508 ==( void*)
Cyc_Absyn_FloatType){ goto _LL517;} else{ goto _LL518;} _LL518: if( _temp508 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL519;} else{ goto _LL520;} _LL520: if((
unsigned int) _temp508 > 4u?(( struct _tunion_struct*) _temp508)->tag == Cyc_Absyn_ArrayType:
0){ _LL551: _temp550=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp508)->f1;
goto _LL549; _LL549: _temp548=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp508)->f2; goto _LL547; _LL547: _temp546=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp508)->f3; goto _LL521;} else{ goto _LL522;}
_LL522: if(( unsigned int) _temp508 > 4u?(( struct _tunion_struct*) _temp508)->tag
== Cyc_Absyn_FnType: 0){ _LL553: _temp552=( struct Cyc_Absyn_FnInfo)(( struct
Cyc_Absyn_FnType_struct*) _temp508)->f1; goto _LL523;} else{ goto _LL524;}
_LL524: if(( unsigned int) _temp508 > 4u?(( struct _tunion_struct*) _temp508)->tag
== Cyc_Absyn_TupleType: 0){ _LL555: _temp554=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TupleType_struct*) _temp508)->f1; goto _LL525;} else{ goto _LL526;}
_LL526: if(( unsigned int) _temp508 > 4u?(( struct _tunion_struct*) _temp508)->tag
== Cyc_Absyn_StructType: 0){ _LL561: _temp560=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp508)->f1; goto _LL559; _LL559: _temp558=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp508)->f2; goto _LL557; _LL557: _temp556=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp508)->f3;
goto _LL527;} else{ goto _LL528;} _LL528: if(( unsigned int) _temp508 > 4u?((
struct _tunion_struct*) _temp508)->tag == Cyc_Absyn_TunionType: 0){ _LL563:
_temp562=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp508)->f1; goto _LL529;} else{ goto _LL530;} _LL530: if(( unsigned int)
_temp508 > 4u?(( struct _tunion_struct*) _temp508)->tag == Cyc_Absyn_TunionFieldType:
0){ _LL565: _temp564=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp508)->f1; _LL569: _temp568=( void*) _temp564.field_info; if((( struct
_tunion_struct*) _temp568)->tag == Cyc_Absyn_KnownTunionfield){ _LL573: _temp572=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp568)->f1; goto _LL571; _LL571: _temp570=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp568)->f2; goto _LL567;} else{
goto _LL532;} _LL567: _temp566=( struct Cyc_List_List*) _temp564.targs; goto
_LL531;} else{ goto _LL532;} _LL532: if(( unsigned int) _temp508 > 4u?(( struct
_tunion_struct*) _temp508)->tag == Cyc_Absyn_UnionType: 0){ _LL579: _temp578=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp508)->f1; goto
_LL577; _LL577: _temp576=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp508)->f2; goto _LL575; _LL575: _temp574=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp508)->f3; goto _LL533;} else{ goto
_LL534;} _LL534: if(( unsigned int) _temp508 > 4u?(( struct _tunion_struct*)
_temp508)->tag == Cyc_Absyn_PointerType: 0){ _LL581: _temp580=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp508)->f1; goto _LL535;} else{ goto
_LL536;} _LL536: if(( unsigned int) _temp508 > 4u?(( struct _tunion_struct*)
_temp508)->tag == Cyc_Absyn_RgnHandleType: 0){ _LL583: _temp582=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp508)->f1; goto _LL537;} else{ goto
_LL538;} _LL538: goto _LL539; _LL511: return 1; _LL513: return 0; _LL515: return
1; _LL517: return 1; _LL519: return 0; _LL521: return Cyc_Toc_atomic_typ(
_temp550); _LL523: return 1; _LL525: for( 0; _temp554 != 0; _temp554=({ struct
Cyc_List_List* _temp584= _temp554; if( _temp584 == 0){ _throw( Null_Exception);}
_temp584->tl;})){ if( ! Cyc_Toc_atomic_typ((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp585= _temp554; if( _temp585 == 0){ _throw( Null_Exception);} _temp585->hd;}))).f2)){
return 0;}} return 1; _LL527: return 0; _LL529: return 0; _LL531: return
_temp570->typs == 0; _LL533: return 0; _LL535: return 0; _LL537: return 0;
_LL539:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp586= Cyc_Absynpp_typ2string( t); xprintf("atomic_typ:  bad type %.*s",
_temp586.last_plus_one - _temp586.curr, _temp586.curr);})); return 0; _LL509:;}
static int Cyc_Toc_is_void_star( void* t){ void* _temp587= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp593; struct Cyc_Absyn_Conref* _temp595; struct
Cyc_Absyn_Tqual _temp597; struct Cyc_Absyn_Conref* _temp599; void* _temp601;
void* _temp603; _LL589: if(( unsigned int) _temp587 > 4u?(( struct
_tunion_struct*) _temp587)->tag == Cyc_Absyn_PointerType: 0){ _LL594: _temp593=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp587)->f1;
_LL604: _temp603=( void*) _temp593.elt_typ; goto _LL602; _LL602: _temp601=( void*)
_temp593.rgn_typ; goto _LL600; _LL600: _temp599=( struct Cyc_Absyn_Conref*)
_temp593.nullable; goto _LL598; _LL598: _temp597=( struct Cyc_Absyn_Tqual)
_temp593.tq; goto _LL596; _LL596: _temp595=( struct Cyc_Absyn_Conref*) _temp593.bounds;
goto _LL590;} else{ goto _LL591;} _LL591: goto _LL592; _LL590: { void* _temp605=
Cyc_Tcutil_compress( _temp603); _LL607: if( _temp605 ==( void*) Cyc_Absyn_VoidType){
goto _LL608;} else{ goto _LL609;} _LL609: goto _LL610; _LL608: return 1; _LL610:
return 0; _LL606:;} _LL592: return 0; _LL588:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp611= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Structdecl** _temp623; struct Cyc_Absyn_Structdecl* _temp625;
struct Cyc_List_List* _temp626; struct _tuple0* _temp628; struct Cyc_Absyn_Structdecl**
_temp630; struct Cyc_List_List* _temp632; struct _tuple0* _temp634; struct Cyc_Absyn_Uniondecl**
_temp636; struct Cyc_Absyn_Uniondecl* _temp638; struct Cyc_List_List* _temp639;
struct _tuple0* _temp641; struct Cyc_Absyn_Uniondecl** _temp643; struct Cyc_List_List*
_temp645; struct _tuple0* _temp647; _LL613: if(( unsigned int) _temp611 > 4u?((
struct _tunion_struct*) _temp611)->tag == Cyc_Absyn_StructType: 0){ _LL629:
_temp628=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp611)->f1;
goto _LL627; _LL627: _temp626=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp611)->f2; goto _LL624; _LL624: _temp623=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp611)->f3; if( _temp623 == 0){ goto
_LL615;} else{ _temp625=* _temp623; goto _LL614;}} else{ goto _LL615;} _LL615:
if(( unsigned int) _temp611 > 4u?(( struct _tunion_struct*) _temp611)->tag ==
Cyc_Absyn_StructType: 0){ _LL635: _temp634=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp611)->f1; goto _LL633; _LL633: _temp632=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp611)->f2; goto _LL631; _LL631: _temp630=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp611)->f3;
if( _temp630 == 0){ goto _LL616;} else{ goto _LL617;}} else{ goto _LL617;}
_LL617: if(( unsigned int) _temp611 > 4u?(( struct _tunion_struct*) _temp611)->tag
== Cyc_Absyn_UnionType: 0){ _LL642: _temp641=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp611)->f1; goto _LL640; _LL640: _temp639=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp611)->f2; goto _LL637; _LL637: _temp636=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp611)->f3;
if( _temp636 == 0){ goto _LL619;} else{ _temp638=* _temp636; goto _LL618;}}
else{ goto _LL619;} _LL619: if(( unsigned int) _temp611 > 4u?(( struct
_tunion_struct*) _temp611)->tag == Cyc_Absyn_UnionType: 0){ _LL648: _temp647=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp611)->f1; goto
_LL646; _LL646: _temp645=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp611)->f2; goto _LL644; _LL644: _temp643=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp611)->f3; if( _temp643 == 0){ goto
_LL620;} else{ goto _LL621;}} else{ goto _LL621;} _LL621: goto _LL622; _LL614:
if( _temp628 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp649=( char*)"StructType name not properly set";
struct _tagged_string _temp650; _temp650.curr= _temp649; _temp650.base= _temp649;
_temp650.last_plus_one= _temp649 + 33; _temp650;}));} if( _temp625->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp652= Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp651=
_temp628; if( _temp651 == 0){ _throw( Null_Exception);} _temp651;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp652.last_plus_one - _temp652.curr, _temp652.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp659= _temp625->fields;
if( _temp659 == 0){ _throw( Null_Exception);} _temp659->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp653= fields; if( _temp653 == 0){ _throw(
Null_Exception);} _temp653->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp654= fields; if( _temp654 == 0){ _throw(
Null_Exception);} _temp654->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp655= fields;
if( _temp655 == 0){ _throw( Null_Exception);} _temp655->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp657=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp656= _temp628;
if( _temp656 == 0){ _throw( Null_Exception);} _temp656;})); struct
_tagged_string _temp658=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp657.last_plus_one - _temp657.curr, _temp657.curr, _temp658.last_plus_one -
_temp658.curr, _temp658.curr);})); return 0;} _LL616: if( _temp634 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp660=( char*)"StructType name not properly set"; struct _tagged_string
_temp661; _temp661.curr= _temp660; _temp661.base= _temp660; _temp661.last_plus_one=
_temp660 + 33; _temp661;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp663= Cyc_Absynpp_qvar2string(( struct _tuple0*)({
struct _tuple0* _temp662= _temp634; if( _temp662 == 0){ _throw( Null_Exception);}
_temp662;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp663.last_plus_one
- _temp663.curr, _temp663.curr);})); return 0; _LL618: if( _temp641 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp664=( char*)"UnionType name not properly set"; struct _tagged_string
_temp665; _temp665.curr= _temp664; _temp665.base= _temp664; _temp665.last_plus_one=
_temp664 + 32; _temp665;}));} if( _temp638->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp667= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp666= _temp641; if( _temp666 == 0){
_throw( Null_Exception);} _temp666;})); xprintf("is_poly_field:  union %.*s missing fields",
_temp667.last_plus_one - _temp667.curr, _temp667.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp674= _temp638->fields;
if( _temp674 == 0){ _throw( Null_Exception);} _temp674->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp668= fields; if( _temp668 == 0){ _throw(
Null_Exception);} _temp668->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp669= fields; if( _temp669 == 0){ _throw(
Null_Exception);} _temp669->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp670= fields;
if( _temp670 == 0){ _throw( Null_Exception);} _temp670->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp672=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp671= _temp641;
if( _temp671 == 0){ _throw( Null_Exception);} _temp671;})); struct
_tagged_string _temp673=* f; xprintf("is_poly_field:  union %.*s has no %.*s field",
_temp672.last_plus_one - _temp672.curr, _temp672.curr, _temp673.last_plus_one -
_temp673.curr, _temp673.curr);})); return 0;} _LL620: if( _temp647 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp675=( char*)"UnionType name not properly set"; struct _tagged_string
_temp676; _temp676.curr= _temp675; _temp676.base= _temp675; _temp676.last_plus_one=
_temp675 + 32; _temp676;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp678= Cyc_Absynpp_qvar2string(( struct _tuple0*)({
struct _tuple0* _temp677= _temp647; if( _temp677 == 0){ _throw( Null_Exception);}
_temp677;})); xprintf("is_poly_field:  union %.*s missing structdecl", _temp678.last_plus_one
- _temp678.curr, _temp678.curr);})); return 0; _LL622:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp679= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp679.last_plus_one - _temp679.curr,
_temp679.curr);})); return 0; _LL612:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp680=( void*) e->r; struct _tagged_string*
_temp688; struct Cyc_Absyn_Exp* _temp690; struct _tagged_string* _temp692;
struct Cyc_Absyn_Exp* _temp694; _LL682: if((( struct _tunion_struct*) _temp680)->tag
== Cyc_Absyn_StructMember_e){ _LL691: _temp690=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp680)->f1; goto _LL689; _LL689: _temp688=(
struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp680)->f2;
goto _LL683;} else{ goto _LL684;} _LL684: if((( struct _tunion_struct*) _temp680)->tag
== Cyc_Absyn_StructArrow_e){ _LL695: _temp694=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp680)->f1; goto _LL693; _LL693: _temp692=(
struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp680)->f2;
goto _LL685;} else{ goto _LL686;} _LL686: goto _LL687; _LL683: return Cyc_Toc_is_poly_field((
void*)({ struct Cyc_Core_Opt* _temp696= _temp690->topt; if( _temp696 == 0){
_throw( Null_Exception);} _temp696->v;}), _temp688); _LL685: { void* _temp698=
Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp697= _temp694->topt;
if( _temp697 == 0){ _throw( Null_Exception);} _temp697->v;})); struct Cyc_Absyn_PtrInfo
_temp704; struct Cyc_Absyn_Conref* _temp706; struct Cyc_Absyn_Tqual _temp708;
struct Cyc_Absyn_Conref* _temp710; void* _temp712; void* _temp714; _LL700: if((
unsigned int) _temp698 > 4u?(( struct _tunion_struct*) _temp698)->tag == Cyc_Absyn_PointerType:
0){ _LL705: _temp704=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp698)->f1; _LL715: _temp714=( void*) _temp704.elt_typ; goto _LL713; _LL713:
_temp712=( void*) _temp704.rgn_typ; goto _LL711; _LL711: _temp710=( struct Cyc_Absyn_Conref*)
_temp704.nullable; goto _LL709; _LL709: _temp708=( struct Cyc_Absyn_Tqual)
_temp704.tq; goto _LL707; _LL707: _temp706=( struct Cyc_Absyn_Conref*) _temp704.bounds;
goto _LL701;} else{ goto _LL702;} _LL702: goto _LL703; _LL701: return Cyc_Toc_is_poly_field(
_temp714, _temp692); _LL703:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp717= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp716= _temp694->topt; if( _temp716 == 0){ _throw( Null_Exception);} _temp716->v;}));
xprintf("is_poly_project:  bad type %.*s", _temp717.last_plus_one - _temp717.curr,
_temp717.curr);})); return 0; _LL699:;} _LL687: return 0; _LL681:;} static char
_temp720[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp720, _temp720, _temp720 + 10u}; static struct
_tuple0 Cyc_Toc_gc_malloc_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var,
0),({ struct Cyc_List_List* _temp721=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp721->hd=( void*) s; _temp721->tl= 0; _temp721;}), 0);}
static char _temp724[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp724, _temp724, _temp724 + 17u}; static struct
_tuple0 Cyc_Toc_gc_malloc_atomic_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_atomic_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var,
0),({ struct Cyc_List_List* _temp725=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp725->hd=( void*) s; _temp725->tl= 0; _temp725;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static char _temp728[ 15u]="_region_malloc"; static struct _tagged_string
Cyc_Toc__region_malloc_string=( struct _tagged_string){ _temp728, _temp728,
_temp728 + 15u}; static struct _tuple0 Cyc_Toc_region_malloc_var=( struct
_tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__region_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_region_malloc_var,
0),({ struct Cyc_List_List* _temp729=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp729->hd=( void*) rgn; _temp729->tl=({ struct Cyc_List_List*
_temp730=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp730->hd=( void*) s; _temp730->tl= 0; _temp730;}); _temp729;}), 0);} static
const int Cyc_Toc_NullCheck= 0; struct Cyc_Toc_NullCheck_struct{ int tag; void*
f1; } ; static const int Cyc_Toc_UntagPtr= 1; struct Cyc_Toc_UntagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; int f3; } ; static const int Cyc_Toc_TagPtr=
2; struct Cyc_Toc_TagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; }
; typedef void* Cyc_Toc_conv; struct _tuple7{ void* f1; void* f2; } ; static
struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp732=({ struct _tuple7
_temp731; _temp731.f1= t1; _temp731.f2= t2; _temp731;}); void* _temp738; struct
Cyc_Absyn_PtrInfo _temp740; void* _temp742; struct Cyc_Absyn_PtrInfo _temp744;
_LL734: _LL743: _temp742= _temp732.f1; if(( unsigned int) _temp742 > 4u?((
struct _tunion_struct*) _temp742)->tag == Cyc_Absyn_PointerType: 0){ _LL745:
_temp744=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp742)->f1; goto _LL739;} else{ goto _LL736;} _LL739: _temp738= _temp732.f2;
if(( unsigned int) _temp738 > 4u?(( struct _tunion_struct*) _temp738)->tag ==
Cyc_Absyn_PointerType: 0){ _LL741: _temp740=( struct Cyc_Absyn_PtrInfo)(( struct
Cyc_Absyn_PointerType_struct*) _temp738)->f1; goto _LL735;} else{ goto _LL736;}
_LL736: goto _LL737; _LL735: { int n1=(( int(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp744.nullable); int n2=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp740.nullable); void* b1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp744.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp740.bounds); struct Cyc_List_List* convs= 0;{
struct _tuple7 _temp747=({ struct _tuple7 _temp746; _temp746.f1= b1; _temp746.f2=
b2; _temp746;}); void* _temp757; struct Cyc_Absyn_Exp* _temp759; void* _temp761;
struct Cyc_Absyn_Exp* _temp763; void* _temp765; void* _temp767; void* _temp769;
void* _temp771; struct Cyc_Absyn_Exp* _temp773; void* _temp775; struct Cyc_Absyn_Exp*
_temp777; void* _temp779; _LL749: _LL762: _temp761= _temp747.f1; if((
unsigned int) _temp761 > 1u?(( struct _tunion_struct*) _temp761)->tag == Cyc_Absyn_Upper_b:
0){ _LL764: _temp763=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp761)->f1; goto _LL758;} else{ goto _LL751;} _LL758: _temp757= _temp747.f2;
if(( unsigned int) _temp757 > 1u?(( struct _tunion_struct*) _temp757)->tag ==
Cyc_Absyn_Upper_b: 0){ _LL760: _temp759=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp757)->f1; goto _LL750;} else{ goto _LL751;} _LL751: _LL768: _temp767=
_temp747.f1; if( _temp767 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL766;} else{
goto _LL753;} _LL766: _temp765= _temp747.f2; if( _temp765 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL752;} else{ goto _LL753;} _LL753: _LL772: _temp771= _temp747.f1; if((
unsigned int) _temp771 > 1u?(( struct _tunion_struct*) _temp771)->tag == Cyc_Absyn_Upper_b:
0){ _LL774: _temp773=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp771)->f1; goto _LL770;} else{ goto _LL755;} _LL770: _temp769= _temp747.f2;
if( _temp769 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL754;} else{ goto _LL755;}
_LL755: _LL780: _temp779= _temp747.f1; if( _temp779 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL776;} else{ goto _LL748;} _LL776: _temp775= _temp747.f2; if((
unsigned int) _temp775 > 1u?(( struct _tunion_struct*) _temp775)->tag == Cyc_Absyn_Upper_b:
0){ _LL778: _temp777=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp775)->f1; goto _LL756;} else{ goto _LL748;} _LL750: if( n1? ! n2: 0){ convs=({
struct Cyc_List_List* _temp781=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp781->hd=( void*)(( void*)({ struct Cyc_Toc_NullCheck_struct*
_temp782=( struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp782[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp783; _temp783.tag= Cyc_Toc_NullCheck;
_temp783.f1=( void*) t2; _temp783;}); _temp782;})); _temp781->tl= convs;
_temp781;});} goto _LL748; _LL752: goto _LL748; _LL754: convs=({ struct Cyc_List_List*
_temp784=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp784->hd=( void*)(( void*)({ struct Cyc_Toc_TagPtr_struct* _temp785=( struct
Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp785[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp786; _temp786.tag= Cyc_Toc_TagPtr;
_temp786.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp786.f2= _temp773; _temp786;});
_temp785;})); _temp784->tl= convs; _temp784;}); goto _LL748; _LL756: convs=({
struct Cyc_List_List* _temp787=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp787->hd=( void*)(( void*)({ struct Cyc_Toc_UntagPtr_struct*
_temp788=( struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp788[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp789; _temp789.tag= Cyc_Toc_UntagPtr;
_temp789.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp789.f2= _temp777; _temp789.f3=
n2; _temp789;}); _temp788;})); _temp787->tl= convs; _temp787;}); goto _LL748;
_LL748:;} return convs;} _LL737: return 0; _LL733:;}} static char _temp792[ 7u]="_throw";
static struct _tagged_string Cyc_Toc__throw_string=( struct _tagged_string){
_temp792, _temp792, _temp792 + 7u}; static struct _tuple0 Cyc_Toc__throw_var=(
struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__throw_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__throw_var, 0),({
struct Cyc_List_List* _temp793=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp793->hd=( void*) e; _temp793->tl= 0; _temp793;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp794=( char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp795; _temp795.curr= _temp794; _temp795.base= _temp794;
_temp795.last_plus_one= _temp794 + 44; _temp795;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple0* a= Cyc_Toc_temp_var();
struct _tuple0* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp796= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp802; struct Cyc_Absyn_Conref* _temp804; struct Cyc_Absyn_Tqual
_temp806; struct Cyc_Absyn_Conref* _temp808; void* _temp810; void* _temp812;
_LL798: if(( unsigned int) _temp796 > 4u?(( struct _tunion_struct*) _temp796)->tag
== Cyc_Absyn_PointerType: 0){ _LL803: _temp802=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp796)->f1; _LL813: _temp812=( void*)
_temp802.elt_typ; goto _LL811; _LL811: _temp810=( void*) _temp802.rgn_typ; goto
_LL809; _LL809: _temp808=( struct Cyc_Absyn_Conref*) _temp802.nullable; goto
_LL807; _LL807: _temp806=( struct Cyc_Absyn_Tqual) _temp802.tq; goto _LL805;
_LL805: _temp804=( struct Cyc_Absyn_Conref*) _temp802.bounds; goto _LL799;}
else{ goto _LL800;} _LL800: goto _LL801; _LL799: elt_typ= _temp812;{ struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp812, Cyc_Toc_mt_tq);
strt_typ= Cyc_Absyn_strct( n); goto _LL797;} _LL801: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp814=( char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp815;
_temp815.curr= _temp814; _temp815.base= _temp814; _temp815.last_plus_one=
_temp814 + 28; _temp815;})); _LL797:;} if( toplevel){ int is_string= 0;{ void*
_temp816=( void*) e->r; void* _temp822; struct _tagged_string _temp824; _LL818:
if((( struct _tunion_struct*) _temp816)->tag == Cyc_Absyn_Const_e){ _LL823:
_temp822=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp816)->f1; if((
unsigned int) _temp822 > 1u?(( struct _tunion_struct*) _temp822)->tag == Cyc_Absyn_String_c:
0){ _LL825: _temp824=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp822)->f1; goto _LL819;} else{ goto _LL820;}} else{ goto _LL820;} _LL820:
goto _LL821; _LL819: is_string= 1; goto _LL817; _LL821: goto _LL817; _LL817:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp829=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp829[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp830; _temp830.tag= Cyc_Absyn_ArrayType; _temp830.f1=(
void*) elt_typ; _temp830.f2= Cyc_Toc_mt_tq; _temp830.f3=( struct Cyc_Absyn_Exp*)
sz; _temp830;}); _temp829;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp826=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp826->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp827=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp827[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp828; _temp828.tag= Cyc_Absyn_Var_d;
_temp828.f1= vd; _temp828;}); _temp827;}), 0); _temp826->tl= Cyc_Toc_result_decls;
_temp826;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp831=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp831->hd=( void*)({ struct _tuple4* _temp836=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp836->f1= 0; _temp836->f2=
xexp; _temp836;}); _temp831->tl=({ struct Cyc_List_List* _temp832=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp832->hd=( void*)({ struct
_tuple4* _temp835=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp835->f1= 0; _temp835->f2= xexp; _temp835;}); _temp832->tl=({ struct Cyc_List_List*
_temp833=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp833->hd=( void*)({ struct _tuple4* _temp834=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp834->f1= 0; _temp834->f2= xplussz; _temp834;});
_temp833->tl= 0; _temp833;}); _temp832;}); _temp831;}), 0); return urm_exp;}}}
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
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp837=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp838; _temp838.curr= _temp837; _temp838.base= _temp837; _temp838.last_plus_one=
_temp837 + 45; _temp838;}));}{ struct _tuple0* x= Cyc_Toc_temp_var(); int
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
_temp839= cs; if( _temp839 == 0){ _throw( Null_Exception);} _temp839->tl;})){
void* _temp841=( void*)({ struct Cyc_List_List* _temp840= cs; if( _temp840 == 0){
_throw( Null_Exception);} _temp840->hd;}); void* _temp849; struct Cyc_Absyn_Exp*
_temp851; void* _temp853; int _temp855; struct Cyc_Absyn_Exp* _temp857; void*
_temp859; _LL843: if((( struct _tunion_struct*) _temp841)->tag == Cyc_Toc_NullCheck){
_LL850: _temp849=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp841)->f1;
goto _LL844;} else{ goto _LL845;} _LL845: if((( struct _tunion_struct*) _temp841)->tag
== Cyc_Toc_TagPtr){ _LL854: _temp853=( void*)(( struct Cyc_Toc_TagPtr_struct*)
_temp841)->f1; goto _LL852; _LL852: _temp851=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Toc_TagPtr_struct*) _temp841)->f2; goto _LL846;} else{ goto _LL847;} _LL847:
if((( struct _tunion_struct*) _temp841)->tag == Cyc_Toc_UntagPtr){ _LL860:
_temp859=( void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp841)->f1; goto _LL858;
_LL858: _temp857=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_UntagPtr_struct*)
_temp841)->f2; goto _LL856; _LL856: _temp855=( int)(( struct Cyc_Toc_UntagPtr_struct*)
_temp841)->f3; goto _LL848;} else{ goto _LL842;} _LL844: e= Cyc_Toc_null_check_conv(
toplevel, _temp849, e); goto _LL842; _LL846: e= Cyc_Toc_tag_ptr_conv( toplevel,
_temp853, _temp851, e); goto _LL842; _LL848: e= Cyc_Toc_untag_ptr_conv( toplevel,
_temp859, _temp857, _temp855, e); goto _LL842; _LL842:;}} return e;} struct Cyc_Toc_Env{
struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt*
fallthru_info; struct Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct
Cyc_Toc_Env* Cyc_Toc_env_t; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){
return({ struct Cyc_Toc_Env* _temp861=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp861->break_lab= 0; _temp861->continue_lab= 0;
_temp861->fallthru_info= 0; _temp861->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp861->toplevel= 1; _temp861;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env(
struct Cyc_Toc_Env* e){ return({ struct Cyc_Toc_Env* _temp862=( struct Cyc_Toc_Env*)
GC_malloc( sizeof( struct Cyc_Toc_Env)); _temp862->break_lab= e->break_lab;
_temp862->continue_lab= e->continue_lab; _temp862->fallthru_info= e->fallthru_info;
_temp862->varmap= e->varmap; _temp862->toplevel= e->toplevel; _temp862;});}
static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env* e, struct
_tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp863=(* x).f1; struct Cyc_List_List*
_temp869; _LL865: if(( unsigned int) _temp863 > 1u?(( struct _tunion_struct*)
_temp863)->tag == Cyc_Absyn_Rel_n: 0){ _LL870: _temp869=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp863)->f1; goto _LL866;} else{ goto _LL867;}
_LL867: goto _LL868; _LL866:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp871=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp871[ 0]=({ struct Cyc_Core_Impossible_struct _temp872; _temp872.tag= Cyc_Core_Impossible;
_temp872.f1=({ struct _tagged_string _temp873= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp873.last_plus_one - _temp873.curr,
_temp873.curr);}); _temp872;}); _temp871;})); _LL868: goto _LL864; _LL864:;}{
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
_temp874= fallthru_binders; if( _temp874 == 0){ _throw( Null_Exception);}
_temp874->tl;})){ fallthru_vars=({ struct Cyc_List_List* _temp875=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp875->hd=( void*)(( struct Cyc_Absyn_Vardecl*)({
struct Cyc_List_List* _temp876= fallthru_binders; if( _temp876 == 0){ _throw(
Null_Exception);} _temp876->hd;}))->name; _temp875->tl= fallthru_vars; _temp875;});}
fallthru_vars=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fallthru_vars);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp877=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp877->v=( void*) break_l; _temp877;}); ans->fallthru_info=({
struct Cyc_Core_Opt* _temp878=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp878->v=( void*)({ struct _tuple8* _temp879=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp879->f1= fallthru_l; _temp879->f2=
fallthru_vars; _temp879->f3= next_case_env->varmap; _temp879;}); _temp878;});
return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct
Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab=({ struct Cyc_Core_Opt* _temp880=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp880->v=( void*) break_l; _temp880;});
ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env* e, struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp881=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp881->v=( void*)({ struct _tuple8* _temp882=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp882->f1= next_l; _temp882->f2= 0; _temp882->f3=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp882;}); _temp881;}); return ans;} struct _tuple9{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
struct Cyc_List_List* f4; char f5; int f6; } ; struct _tuple10{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp883=( char*)"expecting a literal format string";
struct _tagged_string _temp884; _temp884.curr= _temp883; _temp884.base= _temp883;
_temp884.last_plus_one= _temp883 + 34; _temp884;}));}{ struct _tagged_string
fmt_str;{ void* _temp886=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp885= args; if( _temp885 == 0){ _throw( Null_Exception);} _temp885->hd;}))->r;
void* _temp892; struct _tagged_string _temp894; _LL888: if((( struct
_tunion_struct*) _temp886)->tag == Cyc_Absyn_Const_e){ _LL893: _temp892=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp886)->f1; if(( unsigned int) _temp892 > 1u?((
struct _tunion_struct*) _temp892)->tag == Cyc_Absyn_String_c: 0){ _LL895:
_temp894=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp892)->f1;
goto _LL889;} else{ goto _LL890;}} else{ goto _LL890;} _LL890: goto _LL891;
_LL889: fmt_str= _temp894; goto _LL887; _LL891: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp896=( char*)"expecting a literal format string"; struct _tagged_string
_temp897; _temp897.curr= _temp896; _temp897.base= _temp896; _temp897.last_plus_one=
_temp896 + 34; _temp897;})); _LL887:;}{ int len=( int)({ struct _tagged_string
_temp1004= fmt_str;( unsigned int)( _temp1004.last_plus_one - _temp1004.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(({ struct _tagged_string _temp898=
fmt_str; char* _temp900= _temp898.curr + i; if( _temp898.base == 0? 1:( _temp900
< _temp898.base? 1: _temp900 >= _temp898.last_plus_one)){ _throw( Null_Exception);}*
_temp900;}) =='%'){ i ++; if( i < len?({ struct _tagged_string _temp901= fmt_str;
char* _temp903= _temp901.curr + i; if( _temp901.base == 0? 1:( _temp903 <
_temp901.base? 1: _temp903 >= _temp901.last_plus_one)){ _throw( Null_Exception);}*
_temp903;}) =='s': 0){ break;}}} if( i == len){ if( aopt != 0){ args=({ struct
Cyc_List_List* _temp904=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp904->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp905=
aopt; if( _temp905 == 0){ _throw( Null_Exception);} _temp905->v;})); _temp904->tl=
args; _temp904;});} return Cyc_Absyn_primop_exp( p, args, 0);} args=({ struct
Cyc_List_List* _temp906= args; if( _temp906 == 0){ _throw( Null_Exception);}
_temp906->tl;}); typs=({ struct Cyc_List_List* _temp907= typs; if( _temp907 == 0){
_throw( Null_Exception);} _temp907->tl;});{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ char c=({ struct _tagged_string _temp994= fmt_str; char*
_temp996= _temp994.curr + i; if( _temp994.base == 0? 1:( _temp996 < _temp994.base?
1: _temp996 >= _temp994.last_plus_one)){ _throw( Null_Exception);}* _temp996;});
rev_fmt=({ struct Cyc_List_List* _temp908=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp908->hd=( void*)(( int) c); _temp908->tl=
rev_fmt; _temp908;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp909=( char*)"bad format string"; struct
_tagged_string _temp910; _temp910.curr= _temp909; _temp910.base= _temp909;
_temp910.last_plus_one= _temp909 + 18; _temp910;}));}{ struct _tuple9 _temp914;
int _temp915; char _temp917; struct Cyc_List_List* _temp919; struct Cyc_List_List*
_temp921; struct Cyc_List_List* _temp923; struct Cyc_List_List* _temp925; struct
_tuple9* _temp912=( struct _tuple9*)({ struct Cyc_Core_Opt* _temp911= x; if(
_temp911 == 0){ _throw( Null_Exception);} _temp911->v;}); _temp914=* _temp912;
_LL926: _temp925= _temp914.f1; goto _LL924; _LL924: _temp923= _temp914.f2; goto
_LL922; _LL922: _temp921= _temp914.f3; goto _LL920; _LL920: _temp919= _temp914.f4;
goto _LL918; _LL918: _temp917= _temp914.f5; goto _LL916; _LL916: _temp915=
_temp914.f6; goto _LL913; _LL913: i= _temp915 - 1; if( _temp917 !='s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp927=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp927->hd=( void*) _temp925; _temp927->tl=({ struct Cyc_List_List* _temp928=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp928->hd=(
void*) _temp923; _temp928->tl=({ struct Cyc_List_List* _temp929=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp929->hd=( void*) _temp921;
_temp929->tl=({ struct Cyc_List_List* _temp930=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp930->hd=( void*) _temp919;
_temp930->tl= 0; _temp930;}); _temp929;}); _temp928;}); _temp927;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp931=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp931->hd=( void*)(( int) _temp917); _temp931->tl=
rev_fmt; _temp931;});{ struct Cyc_List_List* _temp932= _temp923; struct Cyc_List_List
_temp939; struct Cyc_List_List* _temp940; int _temp942; _LL934: if( _temp932 ==
0){ goto _LL936;} else{ _temp939=* _temp932; _LL943: _temp942=( int) _temp939.hd;
goto _LL941; _LL941: _temp940=( struct Cyc_List_List*) _temp939.tl; if( _temp940
== 0){ goto _LL938;} else{ goto _LL936;}} _LL938: if( _temp942 =='*'){ goto
_LL935;} else{ goto _LL936;} _LL936: goto _LL937; _LL935: { struct _tuple0* temp=
Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp944=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp944->hd=( void*)({ struct
_tuple10* _temp945=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp945->f1= temp; _temp945->f2=( void*)({ struct Cyc_List_List* _temp947= typs;
if( _temp947 == 0){ _throw( Null_Exception);} _temp947->hd;}); _temp945->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp946= args; if( _temp946 == 0){
_throw( Null_Exception);} _temp946->hd;}); _temp945;}); _temp944->tl= rev_temps;
_temp944;}); rev_result=({ struct Cyc_List_List* _temp948=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp948->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp948->tl= rev_result; _temp948;}); args=({ struct Cyc_List_List*
_temp949= args; if( _temp949 == 0){ _throw( Null_Exception);} _temp949->tl;});
typs=({ struct Cyc_List_List* _temp950= typs; if( _temp950 == 0){ _throw(
Null_Exception);} _temp950->tl;}); goto _LL933;} _LL937: goto _LL933; _LL933:;}{
struct Cyc_List_List* _temp951= _temp921; struct Cyc_List_List _temp958; struct
Cyc_List_List* _temp959; struct Cyc_List_List _temp961; struct Cyc_List_List*
_temp962; int _temp964; int _temp966; _LL953: if( _temp951 == 0){ goto _LL955;}
else{ _temp958=* _temp951; _LL967: _temp966=( int) _temp958.hd; goto _LL960;
_LL960: _temp959=( struct Cyc_List_List*) _temp958.tl; if( _temp959 == 0){ goto
_LL955;} else{ _temp961=* _temp959; _LL965: _temp964=( int) _temp961.hd; goto
_LL963; _LL963: _temp962=( struct Cyc_List_List*) _temp961.tl; if( _temp962 == 0){
goto _LL957;} else{ goto _LL955;}}} _LL957: if( _temp966 =='.'? _temp964 =='*':
0){ goto _LL954;} else{ goto _LL955;} _LL955: goto _LL956; _LL954: { struct
_tuple0* temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp968=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp968->hd=(
void*)({ struct _tuple10* _temp969=( struct _tuple10*) GC_malloc( sizeof( struct
_tuple10)); _temp969->f1= temp; _temp969->f2=( void*)({ struct Cyc_List_List*
_temp971= typs; if( _temp971 == 0){ _throw( Null_Exception);} _temp971->hd;});
_temp969->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp970= args;
if( _temp970 == 0){ _throw( Null_Exception);} _temp970->hd;}); _temp969;});
_temp968->tl= rev_temps; _temp968;}); rev_result=({ struct Cyc_List_List*
_temp972=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp972->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp972->tl= rev_result;
_temp972;}); args=({ struct Cyc_List_List* _temp973= args; if( _temp973 == 0){
_throw( Null_Exception);} _temp973->tl;}); typs=({ struct Cyc_List_List*
_temp974= typs; if( _temp974 == 0){ _throw( Null_Exception);} _temp974->tl;});
goto _LL952;} _LL956: goto _LL952; _LL952:;} if( _temp917 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp975= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp975.last_plus_one - _temp975.curr, _temp975.curr);}));}{ struct _tuple0*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp976=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp976->hd=( void*)({
struct _tuple10* _temp977=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp977->f1= temp; _temp977->f2=( void*)({ struct Cyc_List_List* _temp979= typs;
if( _temp979 == 0){ _throw( Null_Exception);} _temp979->hd;}); _temp977->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp978= args; if( _temp978 == 0){
_throw( Null_Exception);} _temp978->hd;}); _temp977;}); _temp976->tl= rev_temps;
_temp976;}); rev_result=({ struct Cyc_List_List* _temp980=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp980->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp980->tl= rev_result; _temp980;}); args=({ struct Cyc_List_List*
_temp981= args; if( _temp981 == 0){ _throw( Null_Exception);} _temp981->tl;});
typs=({ struct Cyc_List_List* _temp982= typs; if( _temp982 == 0){ _throw(
Null_Exception);} _temp982->tl;});}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp983=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp983->hd=( void*)(( int)'.'); _temp983->tl= rev_fmt; _temp983;}); rev_fmt=({
struct Cyc_List_List* _temp984=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp984->hd=( void*)(( int)'*'); _temp984->tl= rev_fmt;
_temp984;}); rev_fmt=({ struct Cyc_List_List* _temp985=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp985->hd=( void*)(( int)'s');
_temp985->tl= rev_fmt; _temp985;});{ struct _tuple0* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp986=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp986->hd=( void*)({ struct _tuple10*
_temp987=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp987->f1=
temp; _temp987->f2=( void*)({ struct Cyc_List_List* _temp989= typs; if( _temp989
== 0){ _throw( Null_Exception);} _temp989->hd;}); _temp987->f3=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp988= args; if( _temp988 == 0){ _throw( Null_Exception);}
_temp988->hd;}); _temp987;}); _temp986->tl= rev_temps; _temp986;});{ struct Cyc_Absyn_Exp*
size_exp= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0), 0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List*
_temp990=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp990->hd=( void*) size_exp; _temp990->tl= rev_result; _temp990;});
rev_result=({ struct Cyc_List_List* _temp991=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp991->hd=( void*) cstring_exp; _temp991->tl=
rev_result; _temp991;}); args=({ struct Cyc_List_List* _temp992= args; if(
_temp992 == 0){ _throw( Null_Exception);} _temp992->tl;}); typs=({ struct Cyc_List_List*
_temp993= typs; if( _temp993 == 0){ _throw( Null_Exception);} _temp993->tl;});}}}}}}{
struct Cyc_List_List* es=({ struct Cyc_List_List* _temp1003=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1003->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp1003->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_result); _temp1003;}); if( aopt != 0){ es=({ struct Cyc_List_List*
_temp997=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp997->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp998=
aopt; if( _temp998 == 0){ _throw( Null_Exception);} _temp998->v;})); _temp997->tl=
es; _temp997;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p, es, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps != 0;
rev_temps=({ struct Cyc_List_List* _temp999= rev_temps; if( _temp999 == 0){
_throw( Null_Exception);} _temp999->tl;})){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)({ struct Cyc_List_List* _temp1000= rev_temps; if( _temp1000 ==
0){ _throw( Null_Exception);} _temp1000->hd;}))).f1,(*(( struct _tuple10*)({
struct Cyc_List_List* _temp1001= rev_temps; if( _temp1001 == 0){ _throw(
Null_Exception);} _temp1001->hd;}))).f2,( struct Cyc_Absyn_Exp*)(*(( struct
_tuple10*)({ struct Cyc_List_List* _temp1002= rev_temps; if( _temp1002 == 0){
_throw( Null_Exception);} _temp1002->hd;}))).f3, s, 0);} return Cyc_Absyn_stmt_exp(
s, 0);}}}}}} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1005=( char*)"Missing type in primop "; struct
_tagged_string _temp1006; _temp1006.curr= _temp1005; _temp1006.base= _temp1005;
_temp1006.last_plus_one= _temp1005 + 24; _temp1006;}));} return Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp1007= e->topt; if( _temp1007 == 0){ _throw(
Null_Exception);} _temp1007->v;}));} static void* Cyc_Toc_get_cyc_typ( struct
Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1008=( char*)"Missing type in primop ";
struct _tagged_string _temp1009; _temp1009.curr= _temp1008; _temp1009.base=
_temp1008; _temp1009.last_plus_one= _temp1008 + 24; _temp1009;}));} return( void*)({
struct Cyc_Core_Opt* _temp1010= e->topt; if( _temp1010 == 0){ _throw(
Null_Exception);} _temp1010->v;});} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1011=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1011->f1= Cyc_Toc_mt_tq; _temp1011->f2=
Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1012= e->topt; if(
_temp1012 == 0){ _throw( Null_Exception);} _temp1012->v;})); _temp1011;});}
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e);
static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp1013=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1013[ 0]=({
struct _tuple4 _temp1014; _temp1014.f1= 0; _temp1014.f2= e; _temp1014;});
_temp1013;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
se= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic( se): Cyc_Toc_malloc_ptr(
se), 0);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1015= rgnopt; if( _temp1015 == 0){ _throw( Null_Exception);} _temp1015;});
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
_temp1016= dles; if( _temp1016 == 0){ _throw( Null_Exception);} _temp1016->tl;})){
struct _tuple4 _temp1020; struct Cyc_Absyn_Exp* _temp1021; struct Cyc_List_List*
_temp1023; struct _tuple4* _temp1018=( struct _tuple4*)({ struct Cyc_List_List*
_temp1017= dles; if( _temp1017 == 0){ _throw( Null_Exception);} _temp1017->hd;});
_temp1020=* _temp1018; _LL1024: _temp1023= _temp1020.f1; goto _LL1022; _LL1022:
_temp1021= _temp1020.f2; goto _LL1019; _LL1019: { struct Cyc_Absyn_Exp* e_index;
if( _temp1023 == 0){ e_index= Cyc_Absyn_signed_int_exp(( count --), 0);} else{
if(({ struct Cyc_List_List* _temp1025= _temp1023; if( _temp1025 == 0){ _throw(
Null_Exception);} _temp1025->tl;}) != 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1026=( char*)"multiple designators in array";
struct _tagged_string _temp1027; _temp1027.curr= _temp1026; _temp1027.base=
_temp1026; _temp1027.last_plus_one= _temp1026 + 30; _temp1027;}));}{ void* d=(
void*)({ struct Cyc_List_List* _temp1040= _temp1023; if( _temp1040 == 0){ _throw(
Null_Exception);} _temp1040->hd;}); void* _temp1028= d; struct Cyc_Absyn_Exp*
_temp1034; struct _tagged_string* _temp1036; _LL1030: if((( struct
_tunion_struct*) _temp1028)->tag == Cyc_Absyn_ArrayElement){ _LL1035: _temp1034=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*) _temp1028)->f1;
goto _LL1031;} else{ goto _LL1032;} _LL1032: if((( struct _tunion_struct*)
_temp1028)->tag == Cyc_Absyn_FieldName){ _LL1037: _temp1036=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1028)->f1; goto
_LL1033;} else{ goto _LL1029;} _LL1031: Cyc_Toc_exp_to_c( nv, _temp1034);
e_index= _temp1034; goto _LL1029; _LL1033: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1038=( char*)"field name designators in array"; struct _tagged_string
_temp1039; _temp1039.curr= _temp1038; _temp1039.base= _temp1038; _temp1039.last_plus_one=
_temp1038 + 32; _temp1039;})); goto _LL1029; _LL1029:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp1041=( void*)
_temp1021->r; struct Cyc_List_List* _temp1049; struct Cyc_Absyn_Exp* _temp1051;
struct Cyc_Absyn_Exp* _temp1053; struct Cyc_Absyn_Vardecl* _temp1055; _LL1043:
if((( struct _tunion_struct*) _temp1041)->tag == Cyc_Absyn_Array_e){ _LL1050:
_temp1049=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1041)->f1;
goto _LL1044;} else{ goto _LL1045;} _LL1045: if((( struct _tunion_struct*)
_temp1041)->tag == Cyc_Absyn_Comprehension_e){ _LL1056: _temp1055=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1041)->f1; goto _LL1054; _LL1054:
_temp1053=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1041)->f2; goto _LL1052; _LL1052: _temp1051=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1041)->f3; goto _LL1046;} else{
goto _LL1047;} _LL1047: goto _LL1048; _LL1044: s= Cyc_Toc_init_array( nv, lval,
_temp1049, s); goto _LL1042; _LL1046: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1055, _temp1053, _temp1051, s, 0); goto _LL1042; _LL1048: Cyc_Toc_exp_to_c(
nv, _temp1021); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp1021, 0), s, 0); goto _LL1042; _LL1042:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* x1= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp( x1,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1073=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1073[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1074; _temp1074.tag= Cyc_Absyn_Local_b;
_temp1074.f1= vd; _temp1074;}); _temp1073;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_var_exp( max, 0), 0);
struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( x1, 0), 0);
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( x1,
0), 0); struct Cyc_Absyn_Stmt* body;{ void* _temp1057=( void*) e2->r; struct Cyc_List_List*
_temp1065; struct Cyc_Absyn_Exp* _temp1067; struct Cyc_Absyn_Exp* _temp1069;
struct Cyc_Absyn_Vardecl* _temp1071; _LL1059: if((( struct _tunion_struct*)
_temp1057)->tag == Cyc_Absyn_Array_e){ _LL1066: _temp1065=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1057)->f1; goto _LL1060;} else{ goto
_LL1061;} _LL1061: if((( struct _tunion_struct*) _temp1057)->tag == Cyc_Absyn_Comprehension_e){
_LL1072: _temp1071=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1057)->f1; goto _LL1070; _LL1070: _temp1069=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1057)->f2; goto _LL1068; _LL1068:
_temp1067=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1057)->f3; goto _LL1062;} else{ goto _LL1063;} _LL1063: goto _LL1064;
_LL1060: body= Cyc_Toc_init_array( nv2, lval, _temp1065, Cyc_Absyn_skip_stmt( 0));
goto _LL1058; _LL1062: body= Cyc_Toc_init_comprehension( nv2, lval, _temp1071,
_temp1069, _temp1067, Cyc_Absyn_skip_stmt( 0), 0); goto _LL1058; _LL1064: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL1058; _LL1058:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
x1, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct Cyc_List_List* tqs=(( struct Cyc_List_List*(*)(
struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, es); struct _tagged_string* n= Cyc_Toc_add_tuple_type( tqs);
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(*
lval_f)( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( es);{
int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es); for( 0; es !=
0; es=({ struct Cyc_List_List* _temp1075= es; if( _temp1075 == 0){ _throw(
Null_Exception);} _temp1075->tl;}), -- i){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1095= es; if( _temp1095 == 0){ _throw( Null_Exception);}
_temp1095->hd;}); struct Cyc_Absyn_Exp* lval= lval_f( xexp,({ struct
_tagged_string* _temp1094=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1094[ 0]= xprintf("f%d", i); _temp1094;}), 0);
is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)({ struct Cyc_Core_Opt*
_temp1076= e->topt; if( _temp1076 == 0){ _throw( Null_Exception);} _temp1076->v;})):
0;{ void* _temp1077=( void*) e->r; struct Cyc_List_List* _temp1085; struct Cyc_Absyn_Exp*
_temp1087; struct Cyc_Absyn_Exp* _temp1089; struct Cyc_Absyn_Vardecl* _temp1091;
_LL1079: if((( struct _tunion_struct*) _temp1077)->tag == Cyc_Absyn_Array_e){
_LL1086: _temp1085=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1077)->f1; goto _LL1080;} else{ goto _LL1081;} _LL1081: if((( struct
_tunion_struct*) _temp1077)->tag == Cyc_Absyn_Comprehension_e){ _LL1092:
_temp1091=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1077)->f1; goto _LL1090; _LL1090: _temp1089=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1077)->f2; goto _LL1088; _LL1088:
_temp1087=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1077)->f3; goto _LL1082;} else{ goto _LL1083;} _LL1083: goto _LL1084;
_LL1080: s= Cyc_Toc_init_array( nv, lval, _temp1085, s); goto _LL1078; _LL1082:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1091, _temp1089, _temp1087, s, 0);
goto _LL1078; _LL1084: Cyc_Toc_exp_to_c( nv, e); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval_f( xexp,({ struct _tagged_string* _temp1093=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp1093[ 0]=
xprintf("f%d", i); _temp1093;}), 0), e, 0), 0), s, 0); goto _LL1078; _LL1078:;}}}
return Cyc_Toc_make_struct( nv, x, Cyc_Absyn_strct( n), s, pointer, rgnopt,
is_atomic);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Exp*(* lval_f)( struct Cyc_Absyn_Exp*, struct
_tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1;{ struct Cyc_List_List* es=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( dles); for( 0;
es != 0; es=({ struct Cyc_List_List* _temp1096= es; if( _temp1096 == 0){ _throw(
Null_Exception);} _temp1096->tl;})){ struct _tuple4 _temp1100; struct Cyc_Absyn_Exp*
_temp1101; struct Cyc_List_List* _temp1103; struct _tuple4* _temp1098=( struct
_tuple4*)({ struct Cyc_List_List* _temp1097= es; if( _temp1097 == 0){ _throw(
Null_Exception);} _temp1097->hd;}); _temp1100=* _temp1098; _LL1104: _temp1103=
_temp1100.f1; goto _LL1102; _LL1102: _temp1101= _temp1100.f2; goto _LL1099;
_LL1099: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)({ struct Cyc_Core_Opt*
_temp1105= _temp1101->topt; if( _temp1105 == 0){ _throw( Null_Exception);}
_temp1105->v;})): 0; if( _temp1103 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1106=( char*)"empty designator list";
struct _tagged_string _temp1107; _temp1107.curr= _temp1106; _temp1107.base=
_temp1106; _temp1107.last_plus_one= _temp1106 + 22; _temp1107;}));} if(({ struct
Cyc_List_List* _temp1108= _temp1103; if( _temp1108 == 0){ _throw( Null_Exception);}
_temp1108->tl;}) != 0){ struct _tuple0* y= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
yexp= Cyc_Absyn_var_exp( y, 0); for( 0; _temp1103 != 0; _temp1103=({ struct Cyc_List_List*
_temp1109= _temp1103; if( _temp1109 == 0){ _throw( Null_Exception);} _temp1109->tl;})){
void* _temp1111=( void*)({ struct Cyc_List_List* _temp1110= _temp1103; if(
_temp1110 == 0){ _throw( Null_Exception);} _temp1110->hd;}); struct
_tagged_string* _temp1117; _LL1113: if((( struct _tunion_struct*) _temp1111)->tag
== Cyc_Absyn_FieldName){ _LL1118: _temp1117=( struct _tagged_string*)(( struct
Cyc_Absyn_FieldName_struct*) _temp1111)->f1; goto _LL1114;} else{ goto _LL1115;}
_LL1115: goto _LL1116; _LL1114: if( Cyc_Toc_is_poly_field( struct_type,
_temp1117)){ yexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), yexp, 0);} s=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f( xexp,
_temp1117, 0), yexp, 0), 0), s, 0); goto _LL1112; _LL1116:( void) _throw((( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1119=( char*)"array designator in struct"; struct _tagged_string _temp1120;
_temp1120.curr= _temp1119; _temp1120.base= _temp1119; _temp1120.last_plus_one=
_temp1119 + 27; _temp1120;}))); _LL1112:;} Cyc_Toc_exp_to_c( nv, _temp1101); s=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( yexp, _temp1101, 0),
0), s, 0);} else{ void* _temp1122=( void*)({ struct Cyc_List_List* _temp1121=
_temp1103; if( _temp1121 == 0){ _throw( Null_Exception);} _temp1121->hd;});
struct _tagged_string* _temp1128; _LL1124: if((( struct _tunion_struct*)
_temp1122)->tag == Cyc_Absyn_FieldName){ _LL1129: _temp1128=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1122)->f1; goto
_LL1125;} else{ goto _LL1126;} _LL1126: goto _LL1127; _LL1125: { struct Cyc_Absyn_Exp*
lval= lval_f( xexp, _temp1128, 0);{ void* _temp1130=( void*) _temp1101->r;
struct Cyc_List_List* _temp1138; struct Cyc_Absyn_Exp* _temp1140; struct Cyc_Absyn_Exp*
_temp1142; struct Cyc_Absyn_Vardecl* _temp1144; _LL1132: if((( struct
_tunion_struct*) _temp1130)->tag == Cyc_Absyn_Array_e){ _LL1139: _temp1138=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1130)->f1; goto
_LL1133;} else{ goto _LL1134;} _LL1134: if((( struct _tunion_struct*) _temp1130)->tag
== Cyc_Absyn_Comprehension_e){ _LL1145: _temp1144=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1130)->f1; goto _LL1143; _LL1143:
_temp1142=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1130)->f2; goto _LL1141; _LL1141: _temp1140=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1130)->f3; goto _LL1135;} else{
goto _LL1136;} _LL1136: goto _LL1137; _LL1133: s= Cyc_Toc_init_array( nv, lval,
_temp1138, s); goto _LL1131; _LL1135: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1144, _temp1142, _temp1140, s, 0); goto _LL1131; _LL1137: Cyc_Toc_exp_to_c(
nv, _temp1101); if( Cyc_Toc_is_poly_field( struct_type, _temp1128)){ _temp1101=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1101, 0);} s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval, _temp1101, 0), 0), s, 0); goto
_LL1131; _LL1131:;} goto _LL1123;} _LL1127:( void) _throw((( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1146=(
char*)"array designator in struct"; struct _tagged_string _temp1147; _temp1147.curr=
_temp1146; _temp1147.base= _temp1146; _temp1147.last_plus_one= _temp1146 + 27;
_temp1147;}))); _LL1123:;}}} return Cyc_Toc_make_struct( nv, x, Cyc_Absyn_strctq(
tdn), s, pointer, rgnopt, is_atomic);} struct _tuple11{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp(
el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp1148=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp1148[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp1149; _temp1149.tag=
Cyc_Absyn_Increment_e; _temp1149.f1= e1; _temp1149.f2=( void*) incr; _temp1149;});
_temp1148;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1150=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp1160; struct Cyc_Absyn_Exp* _temp1162; void* _temp1164; struct
_tagged_string* _temp1166; struct Cyc_Absyn_Exp* _temp1168; _LL1152: if(((
struct _tunion_struct*) _temp1150)->tag == Cyc_Absyn_StmtExp_e){ _LL1161:
_temp1160=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1150)->f1; goto _LL1153;} else{ goto _LL1154;} _LL1154: if((( struct
_tunion_struct*) _temp1150)->tag == Cyc_Absyn_Cast_e){ _LL1165: _temp1164=( void*)((
struct Cyc_Absyn_Cast_e_struct*) _temp1150)->f1; goto _LL1163; _LL1163:
_temp1162=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1150)->f2;
goto _LL1155;} else{ goto _LL1156;} _LL1156: if((( struct _tunion_struct*)
_temp1150)->tag == Cyc_Absyn_StructMember_e){ _LL1169: _temp1168=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1150)->f1; goto _LL1167; _LL1167:
_temp1166=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1150)->f2; goto _LL1157;} else{ goto _LL1158;} _LL1158: goto _LL1159;
_LL1153:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1160, fs, f, f_env); goto _LL1151; _LL1155:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1162, fs, f, f_env); goto
_LL1151; _LL1157:( void*)( e1->r=( void*)(( void*) _temp1168->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1170=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1170->hd=( void*) _temp1166; _temp1170->tl= fs;
_temp1170;}), f, f_env); goto _LL1151; _LL1159: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1171= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1171.last_plus_one - _temp1171.curr, _temp1171.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp1172= fs; if( _temp1172 == 0){ _throw( Null_Exception);} _temp1172->tl;})){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_string*)({ struct
Cyc_List_List* _temp1173= fs; if( _temp1173 == 0){ _throw( Null_Exception);}
_temp1173->hd;}), e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy,
f_env))->r)); goto _LL1151;} _LL1151:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp1174=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1184; struct Cyc_Absyn_Stmt* _temp1186; struct Cyc_Absyn_Decl*
_temp1188; struct Cyc_Absyn_Stmt* _temp1190; struct Cyc_Absyn_Stmt* _temp1192;
_LL1176: if(( unsigned int) _temp1174 > 1u?(( struct _tunion_struct*) _temp1174)->tag
== Cyc_Absyn_Exp_s: 0){ _LL1185: _temp1184=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1174)->f1; goto _LL1177;} else{ goto _LL1178;} _LL1178: if(( unsigned int)
_temp1174 > 1u?(( struct _tunion_struct*) _temp1174)->tag == Cyc_Absyn_Decl_s: 0){
_LL1189: _temp1188=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1174)->f1; goto _LL1187; _LL1187: _temp1186=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1174)->f2; goto _LL1179;} else{ goto
_LL1180;} _LL1180: if(( unsigned int) _temp1174 > 1u?(( struct _tunion_struct*)
_temp1174)->tag == Cyc_Absyn_Seq_s: 0){ _LL1193: _temp1192=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1174)->f1; goto _LL1191; _LL1191: _temp1190=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1174)->f2; goto
_LL1181;} else{ goto _LL1182;} _LL1182: goto _LL1183; _LL1177:(( void(*)( struct
Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1184, fs, f,
f_env); goto _LL1175; _LL1179:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1186, fs, f, f_env); goto _LL1175; _LL1181:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1190, fs, f, f_env);
goto _LL1175; _LL1183:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1194= Cyc_Absynpp_stmt2string( s); xprintf("lvalue_assign_stmt: %.*s",
_temp1194.last_plus_one - _temp1194.curr, _temp1194.curr);})); goto _LL1175;
_LL1175:;} static struct Cyc_Absyn_Exp* Cyc_Toc_known_size_subscript( struct Cyc_Absyn_Exp*
arr, struct Cyc_Absyn_Exp* ind, int possibly_null, struct Cyc_Absyn_Exp* esz,
void* elt_typ, struct Cyc_Absyn_Tqual tq, int take_address){ unsigned int sz=
Cyc_Evexp_eval_const_uint_exp( esz); struct _tuple0* a= Cyc_Toc_temp_var();
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
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1195=
Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s", _temp1195.last_plus_one
- _temp1195.curr, _temp1195.curr);})); return;}{ void* old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1871= e->topt; if( _temp1871 == 0){ _throw( Null_Exception);}
_temp1871->v;}); void* _temp1196= r; void* _temp1270; void* _temp1272; void*
_temp1274; struct _tuple0* _temp1276; struct _tuple0* _temp1278; struct Cyc_List_List*
_temp1280; void* _temp1282; void* _temp1284; struct Cyc_Absyn_Exp* _temp1286;
struct Cyc_Absyn_Exp* _temp1288; struct Cyc_Core_Opt* _temp1290; struct Cyc_Absyn_Exp*
_temp1292; struct Cyc_Absyn_Exp* _temp1294; struct Cyc_Absyn_Exp* _temp1296;
struct Cyc_Absyn_Exp* _temp1298; struct Cyc_Absyn_Exp* _temp1300; struct Cyc_Absyn_Exp*
_temp1302; struct Cyc_List_List* _temp1304; struct Cyc_Absyn_Exp* _temp1306;
struct Cyc_List_List* _temp1308; struct Cyc_Absyn_Exp* _temp1310; struct Cyc_Absyn_Exp*
_temp1312; struct Cyc_Absyn_Exp* _temp1314; struct Cyc_List_List* _temp1316;
struct Cyc_Absyn_Exp* _temp1318; struct Cyc_Absyn_Exp* _temp1320; struct Cyc_Absyn_Exp**
_temp1322; void* _temp1323; void** _temp1325; struct Cyc_Absyn_Exp* _temp1326;
struct Cyc_Absyn_Exp* _temp1328; struct Cyc_Absyn_Exp* _temp1330; void*
_temp1332; struct Cyc_Absyn_Exp* _temp1334; struct Cyc_Absyn_Exp* _temp1336;
struct _tagged_string* _temp1338; struct Cyc_Absyn_Exp* _temp1340; struct
_tagged_string* _temp1342; struct Cyc_Absyn_Exp* _temp1344; struct Cyc_Absyn_Exp*
_temp1346; struct Cyc_Absyn_Exp* _temp1348; struct Cyc_List_List* _temp1350;
struct Cyc_List_List* _temp1352; struct _tuple1* _temp1354; struct Cyc_List_List*
_temp1356; struct Cyc_Absyn_Exp* _temp1358; struct Cyc_Absyn_Exp* _temp1360;
struct Cyc_Absyn_Vardecl* _temp1362; struct Cyc_Absyn_Structdecl* _temp1364;
struct Cyc_List_List* _temp1366; struct Cyc_Core_Opt* _temp1368; struct _tuple0*
_temp1370; struct Cyc_Absyn_Tunionfield* _temp1372; struct Cyc_Absyn_Tuniondecl*
_temp1374; struct Cyc_List_List* _temp1376; struct Cyc_Core_Opt* _temp1378;
struct Cyc_Core_Opt* _temp1380; struct Cyc_Absyn_Tunionfield* _temp1382; struct
Cyc_Absyn_Tuniondecl* _temp1384; struct Cyc_List_List* _temp1386; struct Cyc_Core_Opt*
_temp1388; struct Cyc_Core_Opt* _temp1390; struct Cyc_Absyn_Enumfield* _temp1392;
struct Cyc_Absyn_Enumdecl* _temp1394; struct _tuple0* _temp1396; void* _temp1398;
struct Cyc_Absyn_Exp* _temp1400; struct Cyc_Absyn_Stmt* _temp1402; struct Cyc_List_List*
_temp1404; struct Cyc_Core_Opt* _temp1406; struct Cyc_Absyn_Fndecl* _temp1408;
struct Cyc_Absyn_Exp* _temp1410; _LL1198: if((( struct _tunion_struct*)
_temp1196)->tag == Cyc_Absyn_Const_e){ _LL1271: _temp1270=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1196)->f1; if( _temp1270 ==( void*) Cyc_Absyn_Null_c){ goto _LL1199;} else{
goto _LL1200;}} else{ goto _LL1200;} _LL1200: if((( struct _tunion_struct*)
_temp1196)->tag == Cyc_Absyn_Const_e){ _LL1273: _temp1272=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1196)->f1; goto _LL1201;} else{ goto _LL1202;} _LL1202: if((( struct
_tunion_struct*) _temp1196)->tag == Cyc_Absyn_Var_e){ _LL1277: _temp1276=(
struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp1196)->f1; goto _LL1275;
_LL1275: _temp1274=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1196)->f2;
goto _LL1203;} else{ goto _LL1204;} _LL1204: if((( struct _tunion_struct*)
_temp1196)->tag == Cyc_Absyn_UnknownId_e){ _LL1279: _temp1278=( struct _tuple0*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1196)->f1; goto _LL1205;} else{ goto
_LL1206;} _LL1206: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_Primop_e){
_LL1283: _temp1282=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1196)->f1;
goto _LL1281; _LL1281: _temp1280=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1196)->f2; goto _LL1207;} else{ goto _LL1208;} _LL1208: if((( struct
_tunion_struct*) _temp1196)->tag == Cyc_Absyn_Increment_e){ _LL1287: _temp1286=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1196)->f1;
goto _LL1285; _LL1285: _temp1284=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1196)->f2; goto _LL1209;} else{ goto _LL1210;} _LL1210: if((( struct
_tunion_struct*) _temp1196)->tag == Cyc_Absyn_AssignOp_e){ _LL1293: _temp1292=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1196)->f1;
goto _LL1291; _LL1291: _temp1290=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1196)->f2; goto _LL1289; _LL1289: _temp1288=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1196)->f3; goto _LL1211;} else{ goto
_LL1212;} _LL1212: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_Conditional_e){
_LL1299: _temp1298=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1196)->f1; goto _LL1297; _LL1297: _temp1296=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1196)->f2; goto _LL1295; _LL1295:
_temp1294=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1196)->f3; goto _LL1213;} else{ goto _LL1214;} _LL1214: if((( struct
_tunion_struct*) _temp1196)->tag == Cyc_Absyn_SeqExp_e){ _LL1303: _temp1302=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1196)->f1; goto
_LL1301; _LL1301: _temp1300=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1196)->f2; goto _LL1215;} else{ goto _LL1216;} _LL1216: if((( struct
_tunion_struct*) _temp1196)->tag == Cyc_Absyn_UnknownCall_e){ _LL1307: _temp1306=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp1196)->f1;
goto _LL1305; _LL1305: _temp1304=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1196)->f2; goto _LL1217;} else{ goto _LL1218;} _LL1218: if((( struct
_tunion_struct*) _temp1196)->tag == Cyc_Absyn_FnCall_e){ _LL1311: _temp1310=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp1196)->f1; goto
_LL1309; _LL1309: _temp1308=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1196)->f2; goto _LL1219;} else{ goto _LL1220;} _LL1220: if((( struct
_tunion_struct*) _temp1196)->tag == Cyc_Absyn_Throw_e){ _LL1313: _temp1312=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1196)->f1; goto
_LL1221;} else{ goto _LL1222;} _LL1222: if((( struct _tunion_struct*) _temp1196)->tag
== Cyc_Absyn_NoInstantiate_e){ _LL1315: _temp1314=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1196)->f1; goto _LL1223;} else{
goto _LL1224;} _LL1224: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_Instantiate_e){
_LL1319: _temp1318=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1196)->f1; goto _LL1317; _LL1317: _temp1316=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1196)->f2; goto _LL1225;} else{
goto _LL1226;} _LL1226: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_Cast_e){
_LL1324: _temp1323=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1196)->f1;
_temp1325=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1196)->f1; goto _LL1321;
_LL1321: _temp1320=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1196)->f2; _temp1322=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1196)->f2;
goto _LL1227;} else{ goto _LL1228;} _LL1228: if((( struct _tunion_struct*)
_temp1196)->tag == Cyc_Absyn_Address_e){ _LL1327: _temp1326=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1196)->f1; goto _LL1229;} else{ goto
_LL1230;} _LL1230: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_New_e){
_LL1331: _temp1330=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1196)->f1; goto _LL1329; _LL1329: _temp1328=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1196)->f2; goto _LL1231;} else{ goto
_LL1232;} _LL1232: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_Sizeoftyp_e){
_LL1333: _temp1332=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1196)->f1;
goto _LL1233;} else{ goto _LL1234;} _LL1234: if((( struct _tunion_struct*)
_temp1196)->tag == Cyc_Absyn_Sizeofexp_e){ _LL1335: _temp1334=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp1196)->f1; goto _LL1235;} else{ goto
_LL1236;} _LL1236: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_Deref_e){
_LL1337: _temp1336=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp1196)->f1; goto _LL1237;} else{ goto _LL1238;} _LL1238: if((( struct
_tunion_struct*) _temp1196)->tag == Cyc_Absyn_StructMember_e){ _LL1341:
_temp1340=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1196)->f1; goto _LL1339; _LL1339: _temp1338=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1196)->f2; goto _LL1239;} else{
goto _LL1240;} _LL1240: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_StructArrow_e){
_LL1345: _temp1344=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1196)->f1; goto _LL1343; _LL1343: _temp1342=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1196)->f2; goto _LL1241;} else{
goto _LL1242;} _LL1242: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_Subscript_e){
_LL1349: _temp1348=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1196)->f1; goto _LL1347; _LL1347: _temp1346=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1196)->f2; goto _LL1243;} else{ goto
_LL1244;} _LL1244: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_Tuple_e){
_LL1351: _temp1350=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1196)->f1; goto _LL1245;} else{ goto _LL1246;} _LL1246: if((( struct
_tunion_struct*) _temp1196)->tag == Cyc_Absyn_CompoundLit_e){ _LL1355: _temp1354=(
struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1196)->f1; goto
_LL1353; _LL1353: _temp1352=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1196)->f2; goto _LL1247;} else{ goto _LL1248;} _LL1248: if((( struct
_tunion_struct*) _temp1196)->tag == Cyc_Absyn_Array_e){ _LL1357: _temp1356=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1196)->f1; goto
_LL1249;} else{ goto _LL1250;} _LL1250: if((( struct _tunion_struct*) _temp1196)->tag
== Cyc_Absyn_Comprehension_e){ _LL1363: _temp1362=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1196)->f1; goto _LL1361; _LL1361:
_temp1360=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1196)->f2; goto _LL1359; _LL1359: _temp1358=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1196)->f3; goto _LL1251;} else{
goto _LL1252;} _LL1252: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_Struct_e){
_LL1371: _temp1370=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1196)->f1; goto _LL1369; _LL1369: _temp1368=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1196)->f2; goto _LL1367; _LL1367:
_temp1366=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1196)->f3; goto _LL1365; _LL1365: _temp1364=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1196)->f4; goto _LL1253;} else{ goto
_LL1254;} _LL1254: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_Tunion_e){
_LL1381: _temp1380=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1196)->f1; goto _LL1379; _LL1379: _temp1378=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1196)->f2; goto _LL1377; _LL1377:
_temp1376=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1196)->f3; if( _temp1376 == 0){ goto _LL1375;} else{ goto _LL1256;} _LL1375:
_temp1374=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1196)->f4; goto _LL1373; _LL1373: _temp1372=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1196)->f5; goto _LL1255;} else{ goto
_LL1256;} _LL1256: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_Tunion_e){
_LL1391: _temp1390=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1196)->f1; goto _LL1389; _LL1389: _temp1388=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1196)->f2; goto _LL1387; _LL1387:
_temp1386=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1196)->f3; goto _LL1385; _LL1385: _temp1384=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1196)->f4; goto _LL1383; _LL1383:
_temp1382=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1196)->f5; goto _LL1257;} else{ goto _LL1258;} _LL1258: if((( struct
_tunion_struct*) _temp1196)->tag == Cyc_Absyn_Enum_e){ _LL1397: _temp1396=(
struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1196)->f1; goto _LL1395;
_LL1395: _temp1394=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1196)->f2; goto _LL1393; _LL1393: _temp1392=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1196)->f3; goto _LL1259;} else{ goto
_LL1260;} _LL1260: if((( struct _tunion_struct*) _temp1196)->tag == Cyc_Absyn_Malloc_e){
_LL1401: _temp1400=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1196)->f1; goto _LL1399; _LL1399: _temp1398=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1196)->f2; goto _LL1261;} else{ goto _LL1262;} _LL1262: if((( struct
_tunion_struct*) _temp1196)->tag == Cyc_Absyn_StmtExp_e){ _LL1403: _temp1402=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1196)->f1;
goto _LL1263;} else{ goto _LL1264;} _LL1264: if((( struct _tunion_struct*)
_temp1196)->tag == Cyc_Absyn_UnresolvedMem_e){ _LL1407: _temp1406=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1196)->f1; goto _LL1405; _LL1405:
_temp1404=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1196)->f2; goto _LL1265;} else{ goto _LL1266;} _LL1266: if((( struct
_tunion_struct*) _temp1196)->tag == Cyc_Absyn_Codegen_e){ _LL1409: _temp1408=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp1196)->f1;
goto _LL1267;} else{ goto _LL1268;} _LL1268: if((( struct _tunion_struct*)
_temp1196)->tag == Cyc_Absyn_Fill_e){ _LL1411: _temp1410=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp1196)->f1; goto _LL1269;} else{ goto
_LL1197;} _LL1199:{ void* _temp1412= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1418; struct Cyc_Absyn_Conref* _temp1420; struct Cyc_Absyn_Tqual _temp1422;
struct Cyc_Absyn_Conref* _temp1424; void* _temp1426; void* _temp1428; _LL1414:
if(( unsigned int) _temp1412 > 4u?(( struct _tunion_struct*) _temp1412)->tag ==
Cyc_Absyn_PointerType: 0){ _LL1419: _temp1418=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1412)->f1; _LL1429: _temp1428=( void*)
_temp1418.elt_typ; goto _LL1427; _LL1427: _temp1426=( void*) _temp1418.rgn_typ;
goto _LL1425; _LL1425: _temp1424=( struct Cyc_Absyn_Conref*) _temp1418.nullable;
goto _LL1423; _LL1423: _temp1422=( struct Cyc_Absyn_Tqual) _temp1418.tq; goto
_LL1421; _LL1421: _temp1420=( struct Cyc_Absyn_Conref*) _temp1418.bounds; goto
_LL1415;} else{ goto _LL1416;} _LL1416: goto _LL1417; _LL1415: if(( void*)
_temp1420->v ==( void*) Cyc_Absyn_No_constr){( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto _LL1413;}{ void* _temp1430=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1420); struct Cyc_Absyn_Exp*
_temp1436; _LL1432: if( _temp1430 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1433;}
else{ goto _LL1434;} _LL1434: if(( unsigned int) _temp1430 > 1u?(( struct
_tunion_struct*) _temp1430)->tag == Cyc_Absyn_Upper_b: 0){ _LL1437: _temp1436=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1430)->f1; goto
_LL1435;} else{ goto _LL1431;} _LL1433: { struct _tuple4* zero=({ struct _tuple4*
_temp1441=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1441->f1=
0; _temp1441->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1441;}); struct Cyc_Absyn_Exp*
um= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List* _temp1438=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1438->hd=( void*)
zero; _temp1438->tl=({ struct Cyc_List_List* _temp1439=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1439->hd=( void*) zero;
_temp1439->tl=({ struct Cyc_List_List* _temp1440=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1440->hd=( void*) zero;
_temp1440->tl= 0; _temp1440;}); _temp1439;}); _temp1438;}), 0); if( nv->toplevel){(
void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct( Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1428), Cyc_Toc_mt_tq)); struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*) um, Cyc_Absyn_exp_stmt(
xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}
goto _LL1431;} _LL1435:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1431; _LL1431:;} goto _LL1413; _LL1417:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1442= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1442.last_plus_one -
_temp1442.curr, _temp1442.curr);})); goto _LL1413; _LL1413:;} goto _LL1197;
_LL1201: goto _LL1197; _LL1203:{ struct _handler_cons _temp1443; _push_handler(&
_temp1443);{ void* _temp1444=( void*) setjmp( _temp1443.handler); if( !
_temp1444){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap, _temp1276))->r));;
_pop_handler();} else{ void* _temp1446= _temp1444; _LL1448: if( _temp1446 == Cyc_Dict_Absent){
goto _LL1449;} else{ goto _LL1450;} _LL1450: goto _LL1451; _LL1449:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1452=
Cyc_Absynpp_qvar2string( _temp1276); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1452.last_plus_one - _temp1452.curr, _temp1452.curr);})); return; _LL1451:(
void) _throw( _temp1446); _LL1447:;}}} goto _LL1197; _LL1205:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1453=(
char*)"unknownid"; struct _tagged_string _temp1454; _temp1454.curr= _temp1453;
_temp1454.base= _temp1453; _temp1454.last_plus_one= _temp1453 + 10; _temp1454;}));
goto _LL1197; _LL1207: { struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1280); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1280);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1280);{ void* _temp1455= _temp1282; _LL1457: if( _temp1455 ==( void*)
Cyc_Absyn_Size){ goto _LL1458;} else{ goto _LL1459;} _LL1459: if( _temp1455 ==(
void*) Cyc_Absyn_Printf){ goto _LL1460;} else{ goto _LL1461;} _LL1461: if(
_temp1455 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1462;} else{ goto _LL1463;}
_LL1463: if( _temp1455 ==( void*) Cyc_Absyn_Fprintf){ goto _LL1464;} else{ goto
_LL1465;} _LL1465: if( _temp1455 ==( void*) Cyc_Absyn_Sscanf){ goto _LL1466;}
else{ goto _LL1467;} _LL1467: if( _temp1455 ==( void*) Cyc_Absyn_Plus){ goto
_LL1468;} else{ goto _LL1469;} _LL1469: if( _temp1455 ==( void*) Cyc_Absyn_Minus){
goto _LL1470;} else{ goto _LL1471;} _LL1471: if( _temp1455 ==( void*) Cyc_Absyn_Eq){
goto _LL1472;} else{ goto _LL1473;} _LL1473: if( _temp1455 ==( void*) Cyc_Absyn_Neq){
goto _LL1474;} else{ goto _LL1475;} _LL1475: if( _temp1455 ==( void*) Cyc_Absyn_Gt){
goto _LL1476;} else{ goto _LL1477;} _LL1477: if( _temp1455 ==( void*) Cyc_Absyn_Gte){
goto _LL1478;} else{ goto _LL1479;} _LL1479: if( _temp1455 ==( void*) Cyc_Absyn_Lt){
goto _LL1480;} else{ goto _LL1481;} _LL1481: if( _temp1455 ==( void*) Cyc_Absyn_Lte){
goto _LL1482;} else{ goto _LL1483;} _LL1483: goto _LL1484; _LL1458: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1518= _temp1280; if(
_temp1518 == 0){ _throw( Null_Exception);} _temp1518->hd;});{ void* _temp1486=(
void*)({ struct Cyc_Core_Opt* _temp1485= arg->topt; if( _temp1485 == 0){ _throw(
Null_Exception);} _temp1485->v;}); struct Cyc_Absyn_Exp* _temp1494; struct Cyc_Absyn_Tqual
_temp1496; void* _temp1498; struct Cyc_Absyn_PtrInfo _temp1500; struct Cyc_Absyn_Conref*
_temp1502; struct Cyc_Absyn_Tqual _temp1504; struct Cyc_Absyn_Conref* _temp1506;
void* _temp1508; void* _temp1510; _LL1488: if(( unsigned int) _temp1486 > 4u?((
struct _tunion_struct*) _temp1486)->tag == Cyc_Absyn_ArrayType: 0){ _LL1499:
_temp1498=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1486)->f1; goto
_LL1497; _LL1497: _temp1496=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1486)->f2; goto _LL1495; _LL1495: _temp1494=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1486)->f3; goto _LL1489;} else{ goto
_LL1490;} _LL1490: if(( unsigned int) _temp1486 > 4u?(( struct _tunion_struct*)
_temp1486)->tag == Cyc_Absyn_PointerType: 0){ _LL1501: _temp1500=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1486)->f1; _LL1511: _temp1510=( void*)
_temp1500.elt_typ; goto _LL1509; _LL1509: _temp1508=( void*) _temp1500.rgn_typ;
goto _LL1507; _LL1507: _temp1506=( struct Cyc_Absyn_Conref*) _temp1500.nullable;
goto _LL1505; _LL1505: _temp1504=( struct Cyc_Absyn_Tqual) _temp1500.tq; goto
_LL1503; _LL1503: _temp1502=( struct Cyc_Absyn_Conref*) _temp1500.bounds; goto
_LL1491;} else{ goto _LL1492;} _LL1492: goto _LL1493; _LL1489:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Exp* _temp1512= _temp1494; if( _temp1512 == 0){
_throw( Null_Exception);} _temp1512->r;}))); goto _LL1487; _LL1491: { struct
_tuple0* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1513= arg->topt; if( _temp1513 == 0){ _throw(
Null_Exception);} _temp1513->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp(
e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( temp,
temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1487;}
_LL1493:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1516= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_List_List*
_temp1514= typs; if( _temp1514 == 0){ _throw( Null_Exception);} _temp1514->hd;}));
struct _tagged_string _temp1517= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1515= arg->topt; if( _temp1515 == 0){ _throw( Null_Exception);} _temp1515->v;}));
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1516.last_plus_one
- _temp1516.curr, _temp1516.curr, _temp1517.last_plus_one - _temp1517.curr,
_temp1517.curr);})); return; _LL1487:;} goto _LL1456;} _LL1460:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1282, 0, _temp1280, typs))->r)); goto
_LL1456; _LL1462:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1282, 0, _temp1280, typs))->r)); goto _LL1456; _LL1464: if( _temp1280 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1519=( char*)"fprintf without arguments"; struct _tagged_string
_temp1520; _temp1520.curr= _temp1519; _temp1520.base= _temp1519; _temp1520.last_plus_one=
_temp1519 + 26; _temp1520;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1282,({ struct Cyc_Core_Opt* _temp1521=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1521->v=( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1522= _temp1280; if( _temp1522 == 0){ _throw(
Null_Exception);} _temp1522->hd;})); _temp1521;}),({ struct Cyc_List_List*
_temp1523= _temp1280; if( _temp1523 == 0){ _throw( Null_Exception);} _temp1523->tl;}),({
struct Cyc_List_List* _temp1524= typs; if( _temp1524 == 0){ _throw(
Null_Exception);} _temp1524->tl;})))->r)); goto _LL1456; _LL1466: if( _temp1280
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1525=( char*)"sscanf without arguments"; struct
_tagged_string _temp1526; _temp1526.curr= _temp1525; _temp1526.base= _temp1525;
_temp1526.last_plus_one= _temp1525 + 25; _temp1526;}));}( void*)((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1527= _temp1280; if( _temp1527 == 0){ _throw(
Null_Exception);} _temp1527->hd;}))->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1528= _temp1280; if( _temp1528 == 0){ _throw( Null_Exception);} _temp1528->hd;}))->r,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1529= _temp1280; if(
_temp1529 == 0){ _throw( Null_Exception);} _temp1529->hd;}))->loc), Cyc_Toc_curr_sp,
0))->r)); goto _LL1456; _LL1468: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1533= _temp1280; if( _temp1533 == 0){ _throw( Null_Exception);} _temp1533->hd;});
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1532=({ struct Cyc_List_List* _temp1531= _temp1280; if( _temp1531 == 0){
_throw( Null_Exception);} _temp1531->tl;}); if( _temp1532 == 0){ _throw(
Null_Exception);} _temp1532->hd;}); struct _tuple0* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1530=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1530->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1530;}),
e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1456; _LL1470: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)({ struct Cyc_List_List* _temp1534= old_typs; if( _temp1534 == 0){ _throw(
Null_Exception);} _temp1534->hd;}))){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1541= _temp1280; if( _temp1541 == 0){ _throw(
Null_Exception);} _temp1541->hd;}); struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1540=({ struct Cyc_List_List* _temp1539= _temp1280;
if( _temp1539 == 0){ _throw( Null_Exception);} _temp1539->tl;}); if( _temp1540
== 0){ _throw( Null_Exception);} _temp1540->hd;}); void* old_typ_e2=( void*)({
struct Cyc_List_List* _temp1538=({ struct Cyc_List_List* _temp1537= old_typs;
if( _temp1537 == 0){ _throw( Null_Exception);} _temp1537->tl;}); if( _temp1538
== 0){ _throw( Null_Exception);} _temp1538->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_List_List* _temp1536= old_typs; if( _temp1536 == 0){ _throw(
Null_Exception);} _temp1536->hd;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1535=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1535->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp1535;}), e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0);
s= Cyc_Absyn_declare_stmt( temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1456;
_LL1472: goto _LL1474; _LL1474: goto _LL1476; _LL1476: goto _LL1478; _LL1478:
goto _LL1480; _LL1480: goto _LL1482; _LL1482: { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1547= _temp1280; if(
_temp1547 == 0){ _throw( Null_Exception);} _temp1547->hd;}); struct Cyc_Absyn_Exp*
e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1546=({ struct Cyc_List_List*
_temp1545= _temp1280; if( _temp1545 == 0){ _throw( Null_Exception);} _temp1545->tl;});
if( _temp1546 == 0){ _throw( Null_Exception);} _temp1546->hd;}); void* t1=( void*)({
struct Cyc_List_List* _temp1544= old_typs; if( _temp1544 == 0){ _throw(
Null_Exception);} _temp1544->hd;}); void* t2=( void*)({ struct Cyc_List_List*
_temp1543=({ struct Cyc_List_List* _temp1542= old_typs; if( _temp1542 == 0){
_throw( Null_Exception);} _temp1542->tl;}); if( _temp1543 == 0){ _throw(
Null_Exception);} _temp1543->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1456;} _LL1484: goto
_LL1456; _LL1456:;} goto _LL1197;} _LL1209: { void* e2_cyc_typ=( void*)({ struct
Cyc_Core_Opt* _temp1548= _temp1286->topt; if( _temp1548 == 0){ _throw(
Null_Exception);} _temp1548->v;}); Cyc_Toc_exp_to_c( nv, _temp1286); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){ if( _temp1284 ==( void*) Cyc_Absyn_PreInc? 1: _temp1284 ==( void*)
Cyc_Absyn_PreDec){ struct _tuple0* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Absyn_star_typ(
Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
temp, 0), 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0), _temp1284, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp, temp_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1286, 0), s, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));} else{ struct _tuple0* temp=
Cyc_Toc_temp_var(); struct _tuple0* temp2= Cyc_Toc_temp_var(); void* temp2_typ=
Cyc_Toc_typ_to_c( e2_cyc_typ); void* temp_typ= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c(
e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp2, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( temp,
0), Cyc_Toc_curr_sp, 0), _temp1284, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp2, temp2_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( temp, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1286, 0), s,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} else{
if( ! Cyc_Absyn_is_lvalue( _temp1286)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1286, 0, Cyc_Toc_incr_lvalue,
_temp1284);( void*)( e->r=( void*)(( void*) _temp1286->r));}} goto _LL1197;}
_LL1211: { int e1_poly= Cyc_Toc_is_poly_project( _temp1292); void* e1_old_typ=(
void*)({ struct Cyc_Core_Opt* _temp1554= _temp1292->topt; if( _temp1554 == 0){
_throw( Null_Exception);} _temp1554->v;}); void* e2_old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1553= _temp1288->topt; if( _temp1553 == 0){ _throw(
Null_Exception);} _temp1553->v;}); Cyc_Toc_exp_to_c( nv, _temp1292); Cyc_Toc_exp_to_c(
nv, _temp1288); if( _temp1290 != 0?( void*)({ struct Cyc_Core_Opt* _temp1549=
_temp1290; if( _temp1549 == 0){ _throw( Null_Exception);} _temp1549->v;}) ==(
void*) Cyc_Absyn_Plus? 1:( void*)({ struct Cyc_Core_Opt* _temp1550= _temp1290;
if( _temp1550 == 0){ _throw( Null_Exception);} _temp1550->v;}) ==( void*) Cyc_Absyn_Minus:
0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){( void*)( _temp1292->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)
_temp1292->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1288->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1288->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1288->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1288->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1292)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1292, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1551=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1551[ 0]=({ struct _tuple11 _temp1552; _temp1552.f1=
_temp1290; _temp1552.f2= _temp1288; _temp1552;}); _temp1551;}));( void*)( e->r=(
void*)(( void*) _temp1292->r));} goto _LL1197;} _LL1213: Cyc_Toc_exp_to_c( nv,
_temp1298); Cyc_Toc_exp_to_c( nv, _temp1296); Cyc_Toc_exp_to_c( nv, _temp1294);
goto _LL1197; _LL1215: Cyc_Toc_exp_to_c( nv, _temp1302); Cyc_Toc_exp_to_c( nv,
_temp1300); goto _LL1197; _LL1217: _temp1310= _temp1306; _temp1308= _temp1304;
goto _LL1219; _LL1219: Cyc_Toc_exp_to_c( nv, _temp1310);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1308); goto _LL1197; _LL1221:
Cyc_Toc_exp_to_c( nv, _temp1312);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1312), 0))->r)); goto
_LL1197; _LL1223: Cyc_Toc_exp_to_c( nv, _temp1314); goto _LL1197; _LL1225: Cyc_Toc_exp_to_c(
nv, _temp1318);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1557= e->topt; if( _temp1557 == 0){ _throw( Null_Exception);} _temp1557->v;}));
for( 0; _temp1316 != 0; _temp1316=({ struct Cyc_List_List* _temp1555= _temp1316;
if( _temp1555 == 0){ _throw( Null_Exception);} _temp1555->tl;})){ void* k= Cyc_Tcutil_typ_kind((
void*)({ struct Cyc_List_List* _temp1556= _temp1316; if( _temp1556 == 0){ _throw(
Null_Exception);} _temp1556->hd;})); if( k !=( void*) Cyc_Absyn_EffKind? k !=(
void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1318, 0))->r)); break;}} goto _LL1197;} _LL1227: { void* old_t2=(
void*)({ struct Cyc_Core_Opt* _temp1558=(* _temp1322)->topt; if( _temp1558 == 0){
_throw( Null_Exception);} _temp1558->v;}); void* new_typ=* _temp1325;* _temp1325=
Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,* _temp1322);* _temp1322= Cyc_Toc_convert_exp(
nv->toplevel, old_t2, new_typ,* _temp1322); goto _LL1197;} _LL1229:{ void*
_temp1559=( void*) _temp1326->r; struct Cyc_Absyn_Structdecl* _temp1567; struct
Cyc_List_List* _temp1569; struct Cyc_Core_Opt* _temp1571; struct _tuple0*
_temp1573; struct Cyc_List_List* _temp1575; _LL1561: if((( struct _tunion_struct*)
_temp1559)->tag == Cyc_Absyn_Struct_e){ _LL1574: _temp1573=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1559)->f1; goto _LL1572; _LL1572:
_temp1571=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1559)->f2;
goto _LL1570; _LL1570: _temp1569=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1559)->f3; goto _LL1568; _LL1568: _temp1567=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1559)->f4; goto _LL1562;} else{ goto
_LL1563;} _LL1563: if((( struct _tunion_struct*) _temp1559)->tag == Cyc_Absyn_Tuple_e){
_LL1576: _temp1575=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1559)->f1; goto _LL1564;} else{ goto _LL1565;} _LL1565: goto _LL1566;
_LL1562: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1577= Cyc_Position_string_of_segment( _temp1326->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1577.last_plus_one -
_temp1577.curr, _temp1577.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1578= _temp1326->topt; if( _temp1578 ==
0){ _throw( Null_Exception);} _temp1578->v;}), 1, 0, _temp1569, _temp1573))->r));
goto _LL1560; _LL1564: if( nv->toplevel){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ struct _tagged_string _temp1579= Cyc_Position_string_of_segment(
_temp1326->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1579.last_plus_one - _temp1579.curr, _temp1579.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1575))->r)); goto _LL1560;
_LL1566: Cyc_Toc_exp_to_c( nv, _temp1326); if( ! Cyc_Absyn_is_lvalue( _temp1326)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1326,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*) _temp1326->r));}
goto _LL1560; _LL1560:;} goto _LL1197; _LL1231: if( nv->toplevel){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1580= Cyc_Position_string_of_segment(
_temp1328->loc); xprintf("%.*s: new at top-level", _temp1580.last_plus_one -
_temp1580.curr, _temp1580.curr);}));}{ void* _temp1581=( void*) _temp1328->r;
struct Cyc_List_List* _temp1595; void* _temp1597; struct _tagged_string
_temp1599; struct Cyc_Absyn_Exp* _temp1601; struct Cyc_Absyn_Exp* _temp1603;
struct Cyc_Absyn_Vardecl* _temp1605; struct Cyc_Absyn_Structdecl* _temp1607;
struct Cyc_List_List* _temp1609; struct Cyc_Core_Opt* _temp1611; struct _tuple0*
_temp1613; struct Cyc_List_List* _temp1615; _LL1583: if((( struct _tunion_struct*)
_temp1581)->tag == Cyc_Absyn_Array_e){ _LL1596: _temp1595=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1581)->f1; goto _LL1584;} else{ goto
_LL1585;} _LL1585: if((( struct _tunion_struct*) _temp1581)->tag == Cyc_Absyn_Const_e){
_LL1598: _temp1597=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1581)->f1;
if(( unsigned int) _temp1597 > 1u?(( struct _tunion_struct*) _temp1597)->tag ==
Cyc_Absyn_String_c: 0){ _LL1600: _temp1599=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1597)->f1; goto _LL1586;} else{ goto _LL1587;}} else{ goto _LL1587;}
_LL1587: if((( struct _tunion_struct*) _temp1581)->tag == Cyc_Absyn_Comprehension_e){
_LL1606: _temp1605=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1581)->f1; goto _LL1604; _LL1604: _temp1603=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1581)->f2; goto _LL1602; _LL1602:
_temp1601=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1581)->f3; goto _LL1588;} else{ goto _LL1589;} _LL1589: if((( struct
_tunion_struct*) _temp1581)->tag == Cyc_Absyn_Struct_e){ _LL1614: _temp1613=(
struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1581)->f1; goto
_LL1612; _LL1612: _temp1611=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1581)->f2; goto _LL1610; _LL1610: _temp1609=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1581)->f3; goto _LL1608; _LL1608:
_temp1607=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1581)->f4; goto _LL1590;} else{ goto _LL1591;} _LL1591: if((( struct
_tunion_struct*) _temp1581)->tag == Cyc_Absyn_Tuple_e){ _LL1616: _temp1615=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1581)->f1; goto
_LL1592;} else{ goto _LL1593;} _LL1593: goto _LL1594; _LL1584: { struct _tuple0*
x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_array( nv, xexp, _temp1595, Cyc_Absyn_exp_stmt(
xexp, 0)); void* old_elt_typ;{ void* _temp1617= Cyc_Tcutil_compress( old_typ);
struct Cyc_Absyn_PtrInfo _temp1623; struct Cyc_Absyn_Conref* _temp1625; struct
Cyc_Absyn_Tqual _temp1627; struct Cyc_Absyn_Conref* _temp1629; void* _temp1631;
void* _temp1633; _LL1619: if(( unsigned int) _temp1617 > 4u?(( struct
_tunion_struct*) _temp1617)->tag == Cyc_Absyn_PointerType: 0){ _LL1624:
_temp1623=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1617)->f1; _LL1634: _temp1633=( void*) _temp1623.elt_typ; goto _LL1632;
_LL1632: _temp1631=( void*) _temp1623.rgn_typ; goto _LL1630; _LL1630: _temp1629=(
struct Cyc_Absyn_Conref*) _temp1623.nullable; goto _LL1628; _LL1628: _temp1627=(
struct Cyc_Absyn_Tqual) _temp1623.tq; goto _LL1626; _LL1626: _temp1625=( struct
Cyc_Absyn_Conref*) _temp1623.bounds; goto _LL1620;} else{ goto _LL1621;} _LL1621:
goto _LL1622; _LL1620: old_elt_typ= _temp1633; goto _LL1618; _LL1622:
old_elt_typ=(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1635=( char*)"exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1636; _temp1636.curr= _temp1635; _temp1636.base=
_temp1635; _temp1636.last_plus_one= _temp1635 + 52; _temp1636;})); goto _LL1618;
_LL1618:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1595), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1330 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, e2);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1637= _temp1330; if( _temp1637 == 0){ _throw(
Null_Exception);} _temp1637;}); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp(
r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,(
struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1582;}}} _LL1586: if(
_temp1330 == 0){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1638=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1638->f1= Cyc_Toc_abs_ns; _temp1638->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1638;}), 0),({ struct Cyc_List_List* _temp1639=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1639->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1640=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1640[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1641; _temp1641.tag= Cyc_Absyn_Const_e; _temp1641.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1642=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1642[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1643; _temp1643.tag= Cyc_Absyn_String_c;
_temp1643.f1= _temp1599; _temp1643;}); _temp1642;})); _temp1641;}); _temp1640;}),
0); _temp1639->tl= 0; _temp1639;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1650= _temp1330; if(
_temp1650 == 0){ _throw( Null_Exception);} _temp1650;}); Cyc_Toc_exp_to_c( nv, r);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1644=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1644->f1= Cyc_Toc_abs_ns; _temp1644->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1644;}), 0),({ struct Cyc_List_List* _temp1645=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1645->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1646=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1646[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1647; _temp1647.tag= Cyc_Absyn_Const_e; _temp1647.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1648=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1648[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1649; _temp1649.tag= Cyc_Absyn_String_c;
_temp1649.f1= _temp1599; _temp1649;}); _temp1648;})); _temp1647;}); _temp1646;}),
0); _temp1645->tl= 0; _temp1645;}), 0))->r));} goto _LL1582; _LL1588: { int
is_tagged_ptr= 0;{ void* _temp1651= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1657; struct Cyc_Absyn_Conref* _temp1659; struct Cyc_Absyn_Tqual _temp1661;
struct Cyc_Absyn_Conref* _temp1663; void* _temp1665; void* _temp1667; _LL1653:
if(( unsigned int) _temp1651 > 4u?(( struct _tunion_struct*) _temp1651)->tag ==
Cyc_Absyn_PointerType: 0){ _LL1658: _temp1657=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1651)->f1; _LL1668: _temp1667=( void*)
_temp1657.elt_typ; goto _LL1666; _LL1666: _temp1665=( void*) _temp1657.rgn_typ;
goto _LL1664; _LL1664: _temp1663=( struct Cyc_Absyn_Conref*) _temp1657.nullable;
goto _LL1662; _LL1662: _temp1661=( struct Cyc_Absyn_Tqual) _temp1657.tq; goto
_LL1660; _LL1660: _temp1659=( struct Cyc_Absyn_Conref*) _temp1657.bounds; goto
_LL1654;} else{ goto _LL1655;} _LL1655: goto _LL1656; _LL1654:{ void* _temp1669=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1659))->v; void* _temp1675; _LL1671: if(( unsigned int) _temp1669 > 1u?((
struct _tunion_struct*) _temp1669)->tag == Cyc_Absyn_Eq_constr: 0){ _LL1676:
_temp1675=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1669)->f1; if(
_temp1675 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1672;} else{ goto _LL1673;}}
else{ goto _LL1673;} _LL1673: goto _LL1674; _LL1672: is_tagged_ptr= 1; goto
_LL1670; _LL1674: goto _LL1670; _LL1670:;} goto _LL1652; _LL1656:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1677=( char*)"exp_to_c: comprehension not an array type"; struct
_tagged_string _temp1678; _temp1678.curr= _temp1677; _temp1678.base= _temp1677;
_temp1678.last_plus_one= _temp1677 + 42; _temp1678;})); goto _LL1652; _LL1652:;}{
struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)({ struct Cyc_Core_Opt* _temp1702= _temp1601->topt;
if( _temp1702 == 0){ _throw( Null_Exception);} _temp1702->v;}); void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1603);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1605, Cyc_Absyn_var_exp( max, 0), _temp1601,
Cyc_Absyn_skip_stmt( 0), 1); struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1700=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1700->hd=( void*)({ struct _tuple10* _temp1701=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1701->f1= max; _temp1701->f2= Cyc_Absyn_uint_t;
_temp1701->f3=( struct Cyc_Absyn_Exp*) _temp1603; _temp1701;}); _temp1700->tl= 0;
_temp1700;}); struct Cyc_Absyn_Exp* ai; if( _temp1330 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp1679= _temp1330; if( _temp1679 == 0){ _throw( Null_Exception);}
_temp1679;}); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct
Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1680=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1680->hd=( void*)({ struct _tuple10* _temp1681=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1681->f1= a; _temp1681->f2= ptr_typ;
_temp1681->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1681;}); _temp1680->tl= decls;
_temp1680;}); if( is_tagged_ptr){ struct _tuple0* b= Cyc_Toc_temp_var(); void* t=
Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* binit= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp1684=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1684->hd=( void*)({ struct _tuple4* _temp1689=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1689->f1= 0;
_temp1689->f2= Cyc_Absyn_var_exp( a, 0); _temp1689;}); _temp1684->tl=({ struct
Cyc_List_List* _temp1685=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1685->hd=( void*)({ struct _tuple4* _temp1688=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1688->f1= 0; _temp1688->f2= Cyc_Absyn_var_exp( a,
0); _temp1688;}); _temp1685->tl=({ struct Cyc_List_List* _temp1686=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1686->hd=( void*)({ struct
_tuple4* _temp1687=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1687->f1= 0; _temp1687->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1687;}); _temp1686->tl= 0; _temp1686;});
_temp1685;}); _temp1684;}), 0); decls=({ struct Cyc_List_List* _temp1682=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1682->hd=(
void*)({ struct _tuple10* _temp1683=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1683->f1= b; _temp1683->f2= t; _temp1683->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1683;}); _temp1682->tl= decls; _temp1682;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp1690= ds; if(
_temp1690 == 0){ _throw( Null_Exception);} _temp1690->tl;})){ struct Cyc_Absyn_Exp*
_temp1694; void* _temp1696; struct _tuple0* _temp1698; struct _tuple10 _temp1692=*((
struct _tuple10*)({ struct Cyc_List_List* _temp1691= ds; if( _temp1691 == 0){
_throw( Null_Exception);} _temp1691->hd;})); _LL1699: _temp1698= _temp1692.f1;
goto _LL1697; _LL1697: _temp1696= _temp1692.f2; goto _LL1695; _LL1695: _temp1694=
_temp1692.f3; goto _LL1693; _LL1693: s= Cyc_Absyn_declare_stmt( _temp1698,
_temp1696, _temp1694, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r)); goto _LL1582;}}}} _LL1590:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1703= _temp1328->topt; if( _temp1703 ==
0){ _throw( Null_Exception);} _temp1703->v;}), 1, _temp1330, _temp1609,
_temp1613))->r)); goto _LL1582; _LL1592:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1330, _temp1615))->r)); goto _LL1582; _LL1594: { void* old_elt_typ=(
void*)({ struct Cyc_Core_Opt* _temp1705= _temp1328->topt; if( _temp1705 == 0){
_throw( Null_Exception);} _temp1705->v;}); struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0);
struct Cyc_Absyn_Exp* e_index= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c(
nv, _temp1328); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1328, 0), s, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); struct Cyc_Absyn_Exp*
e1; if( _temp1330 == 0){ e1= Cyc_Toc_malloc_exp( old_elt_typ, e2);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1704=
_temp1330; if( _temp1704 == 0){ _throw( Null_Exception);} _temp1704;}); Cyc_Toc_exp_to_c(
nv, r); e1= Cyc_Toc_rmalloc_exp( r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp(
ptr_typ, e1, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, ptr_typ,( struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1582;}}}
_LL1582:;} goto _LL1197; _LL1233:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1706=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1706[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1707; _temp1707.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1707.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1332);
_temp1707;}); _temp1706;}))); goto _LL1197; _LL1235: Cyc_Toc_exp_to_c( nv,
_temp1334); goto _LL1197; _LL1237: { void* e1_typ= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp1737= _temp1336->topt; if( _temp1737 == 0){ _throw(
Null_Exception);} _temp1737->v;})); Cyc_Toc_exp_to_c( nv, _temp1336);{ void*
_temp1708= e1_typ; struct Cyc_Absyn_PtrInfo _temp1714; struct Cyc_Absyn_Conref*
_temp1716; struct Cyc_Absyn_Tqual _temp1718; struct Cyc_Absyn_Conref* _temp1720;
void* _temp1722; void* _temp1724; _LL1710: if(( unsigned int) _temp1708 > 4u?((
struct _tunion_struct*) _temp1708)->tag == Cyc_Absyn_PointerType: 0){ _LL1715:
_temp1714=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1708)->f1; _LL1725: _temp1724=( void*) _temp1714.elt_typ; goto _LL1723;
_LL1723: _temp1722=( void*) _temp1714.rgn_typ; goto _LL1721; _LL1721: _temp1720=(
struct Cyc_Absyn_Conref*) _temp1714.nullable; goto _LL1719; _LL1719: _temp1718=(
struct Cyc_Absyn_Tqual) _temp1714.tq; goto _LL1717; _LL1717: _temp1716=( struct
Cyc_Absyn_Conref*) _temp1714.bounds; goto _LL1711;} else{ goto _LL1712;} _LL1712:
goto _LL1713; _LL1711:{ void* _temp1726=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1716); struct Cyc_Absyn_Exp* _temp1732; _LL1728: if(
_temp1726 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1729;} else{ goto _LL1730;}
_LL1730: if(( unsigned int) _temp1726 > 1u?(( struct _tunion_struct*) _temp1726)->tag
== Cyc_Absyn_Upper_b: 0){ _LL1733: _temp1732=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp1726)->f1; goto _LL1731;} else{ goto _LL1727;}
_LL1729: { struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* ta1= Cyc_Toc_typ_to_c_array( _temp1724); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
ta1, _temp1718); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1718); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp(
Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt( 0), 0); struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt* s1=
Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,(
struct Cyc_Absyn_Exp*) _temp1336, s1, 0), 0))->r)); goto _LL1727;} _LL1731: if(
Cyc_Toc_is_nullable( e1_typ)){ struct _tuple0* x= Cyc_Toc_temp_var(); void* tx=
Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1734= _temp1336->topt; if(
_temp1734 == 0){ _throw( Null_Exception);} _temp1734->v;})); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3=
Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp1336, s3, 0), 0))->r));}
goto _LL1727; _LL1727:;} goto _LL1709; _LL1713:(( void(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1735=( char*)"exp_to_c: Deref: non-pointer";
struct _tagged_string _temp1736; _temp1736.curr= _temp1735; _temp1736.base=
_temp1735; _temp1736.last_plus_one= _temp1735 + 29; _temp1736;})); goto _LL1709;
_LL1709:;} goto _LL1197;} _LL1239: Cyc_Toc_exp_to_c( nv, _temp1340); if( Cyc_Toc_is_poly_project(
e)){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp1738= e->topt; if( _temp1738 == 0){ _throw(
Null_Exception);} _temp1738->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}
goto _LL1197; _LL1241: Cyc_Toc_exp_to_c( nv, _temp1344);{ int is_poly= Cyc_Toc_is_poly_project(
e); if( Cyc_Toc_is_nullable(( void*)({ struct Cyc_Core_Opt* _temp1739= _temp1344->topt;
if( _temp1739 == 0){ _throw( Null_Exception);} _temp1739->v;}))){ struct _tuple0*
x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1740= _temp1344->topt; if( _temp1740 == 0){ _throw( Null_Exception);}
_temp1740->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0),
_temp1342, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1344, s3, 0), 0))->r));} if( is_poly){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1741= e->topt; if( _temp1741 == 0){ _throw(
Null_Exception);} _temp1741->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}
goto _LL1197;} _LL1243: { void* e1_typ= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp1794= _temp1348->topt; if( _temp1794 == 0){ _throw(
Null_Exception);} _temp1794->v;})); Cyc_Toc_exp_to_c( nv, _temp1348); Cyc_Toc_exp_to_c(
nv, _temp1346);( void*)( _temp1348->r=( void*)(( void*)( Cyc_Toc_array_to_ptr(
Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp(( void*) _temp1348->r, 0), 0))->r));{
void* _temp1742= e1_typ; struct Cyc_List_List* _temp1752; struct Cyc_Absyn_Exp*
_temp1754; struct Cyc_Absyn_Tqual _temp1756; void* _temp1758; struct Cyc_Absyn_PtrInfo
_temp1760; struct Cyc_Absyn_Conref* _temp1762; struct Cyc_Absyn_Tqual _temp1764;
struct Cyc_Absyn_Conref* _temp1766; void* _temp1768; void* _temp1770; _LL1744:
if(( unsigned int) _temp1742 > 4u?(( struct _tunion_struct*) _temp1742)->tag ==
Cyc_Absyn_TupleType: 0){ _LL1753: _temp1752=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TupleType_struct*) _temp1742)->f1; goto _LL1745;} else{ goto _LL1746;}
_LL1746: if(( unsigned int) _temp1742 > 4u?(( struct _tunion_struct*) _temp1742)->tag
== Cyc_Absyn_ArrayType: 0){ _LL1759: _temp1758=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1742)->f1; goto _LL1757; _LL1757: _temp1756=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp1742)->f2; goto _LL1755; _LL1755:
_temp1754=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1742)->f3; goto _LL1747;} else{ goto _LL1748;} _LL1748: if(( unsigned int)
_temp1742 > 4u?(( struct _tunion_struct*) _temp1742)->tag == Cyc_Absyn_PointerType:
0){ _LL1761: _temp1760=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1742)->f1; _LL1771: _temp1770=( void*) _temp1760.elt_typ; goto _LL1769;
_LL1769: _temp1768=( void*) _temp1760.rgn_typ; goto _LL1767; _LL1767: _temp1766=(
struct Cyc_Absyn_Conref*) _temp1760.nullable; goto _LL1765; _LL1765: _temp1764=(
struct Cyc_Absyn_Tqual) _temp1760.tq; goto _LL1763; _LL1763: _temp1762=( struct
Cyc_Absyn_Conref*) _temp1760.bounds; goto _LL1749;} else{ goto _LL1750;} _LL1750:
goto _LL1751; _LL1745: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1346) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1348,({ struct _tagged_string* _temp1772=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1772[ 0]= xprintf("f%d",( int)
i); _temp1772;}), 0))->r)); goto _LL1743;} _LL1747: if( _temp1754 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1773=( char*)"exp_to_c: Subscript_e array type bound null"; struct
_tagged_string _temp1774; _temp1774.curr= _temp1773; _temp1774.base= _temp1773;
_temp1774.last_plus_one= _temp1773 + 44; _temp1774;}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_known_size_subscript( _temp1348, _temp1346, 0,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1775= _temp1754; if( _temp1775 == 0){ _throw(
Null_Exception);} _temp1775;}), _temp1758, _temp1756, 0))->r)); goto _LL1743;
_LL1749:{ void* _temp1776=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1762); struct Cyc_Absyn_Exp* _temp1782; _LL1778: if(( unsigned int)
_temp1776 > 1u?(( struct _tunion_struct*) _temp1776)->tag == Cyc_Absyn_Upper_b:
0){ _LL1783: _temp1782=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1776)->f1; goto _LL1779;} else{ goto _LL1780;} _LL1780: if( _temp1776 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1781;} else{ goto _LL1777;} _LL1779: { int
possibly_null= 1;{ void* _temp1784=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1766))->v; int
_temp1790; _LL1786: if(( unsigned int) _temp1784 > 1u?(( struct _tunion_struct*)
_temp1784)->tag == Cyc_Absyn_Eq_constr: 0){ _LL1791: _temp1790=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp1784)->f1; goto _LL1787;} else{ goto _LL1788;}
_LL1788: goto _LL1789; _LL1787: possibly_null= _temp1790; goto _LL1785; _LL1789:
possibly_null= 0; goto _LL1785; _LL1785:;}( void*)( e->r=( void*)(( void*)( Cyc_Toc_known_size_subscript(
_temp1348, _temp1346, possibly_null, _temp1782, _temp1770, _temp1764, 0))->r));
goto _LL1777;} _LL1781: { struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0*
i= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array(
_temp1770); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1,
_temp1764); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1,
_temp1764); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1346, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1348, s1, 0), 0))->r)); goto _LL1777;} _LL1777:;}
goto _LL1743; _LL1751:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1792=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1793; _temp1793.curr= _temp1792; _temp1793.base=
_temp1792; _temp1793.last_plus_one= _temp1792 + 49; _temp1793;})); goto _LL1743;
_LL1743:;} goto _LL1197;} _LL1245: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1350))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1350); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp1350 != 0; _temp1350=({ struct Cyc_List_List* _temp1795=
_temp1350; if( _temp1795 == 0){ _throw( Null_Exception);} _temp1795->tl;}), i ++){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1796=
_temp1350; if( _temp1796 == 0){ _throw( Null_Exception);} _temp1796->hd;}));{
struct Cyc_List_List* des=({ struct Cyc_List_List* _temp1800=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1800->hd=( void*)(( void*)({
struct Cyc_Absyn_FieldName_struct* _temp1801=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp1801[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp1802; _temp1802.tag= Cyc_Absyn_FieldName;
_temp1802.f1=({ struct _tagged_string* _temp1803=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1803[ 0]= xprintf("f%d", i);
_temp1803;}); _temp1802;}); _temp1801;})); _temp1800->tl= 0; _temp1800;}); dles=({
struct Cyc_List_List* _temp1797=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1797->hd=( void*)({ struct _tuple4* _temp1798=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1798->f1=( struct Cyc_List_List*)
des; _temp1798->f2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1799=
_temp1350; if( _temp1799 == 0){ _throw( Null_Exception);} _temp1799->hd;});
_temp1798;}); _temp1797->tl= dles; _temp1797;});}}} dles=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp( 0,
dles, 0), 0))->r));} goto _LL1197; _LL1247:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1804=( char*)"compoundlit";
struct _tagged_string _temp1805; _temp1805.curr= _temp1804; _temp1805.base=
_temp1804; _temp1805.last_plus_one= _temp1804 + 12; _temp1805;})); goto _LL1197;
_LL1249:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1356, 0))->r));{ struct Cyc_List_List* dles= _temp1356; for( 0; dles != 0;
dles=({ struct Cyc_List_List* _temp1806= dles; if( _temp1806 == 0){ _throw(
Null_Exception);} _temp1806->tl;})){ struct _tuple4 _temp1810; struct Cyc_Absyn_Exp*
_temp1811; struct Cyc_List_List* _temp1813; struct _tuple4* _temp1808=( struct
_tuple4*)({ struct Cyc_List_List* _temp1807= dles; if( _temp1807 == 0){ _throw(
Null_Exception);} _temp1807->hd;}); _temp1810=* _temp1808; _LL1814: _temp1813=
_temp1810.f1; goto _LL1812; _LL1812: _temp1811= _temp1810.f2; goto _LL1809;
_LL1809: Cyc_Toc_exp_to_c( nv, _temp1811);}} goto _LL1197; _LL1251: {
unsigned int sz= Cyc_Evexp_eval_const_uint_exp( _temp1360); Cyc_Toc_exp_to_c( nv,
_temp1358);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp1358)){
unsigned int i= 0; for( 0; i < sz; i ++){ es=({ struct Cyc_List_List* _temp1815=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1815->hd=(
void*)({ struct _tuple4* _temp1816=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1816->f1= 0; _temp1816->f2= _temp1358; _temp1816;}); _temp1815->tl=
es; _temp1815;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL1197;}} _LL1253: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1366, _temp1370))->r));}
else{ struct _tuple0* n= _temp1370; struct Cyc_Absyn_Structdecl* sd2=( struct
Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl* _temp1830= _temp1364 == 0?((
struct Cyc_Absyn_Structdecl*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1829= Cyc_Absynpp_qvar2string( _temp1370); xprintf("no structdecl %.*s",
_temp1829.last_plus_one - _temp1829.curr, _temp1829.curr);})): _temp1364; if(
_temp1830 == 0){ _throw( Null_Exception);} _temp1830;}); struct _RegionHandle
_temp1817= _new_region(); struct _RegionHandle* rgn=& _temp1817; struct
_handler_cons _temp1818; _push_handler(& _temp1818);{ void* _temp1819=( void*)
setjmp( _temp1818.handler); if( ! _temp1819){{ struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1366,( struct Cyc_Absyn_Structdecl*) sd2); for( 0; fields != 0;
fields=({ struct Cyc_List_List* _temp1820= fields; if( _temp1820 == 0){ _throw(
Null_Exception);} _temp1820->tl;})){ struct _tuple12 _temp1824; struct Cyc_Absyn_Exp*
_temp1825; struct Cyc_Absyn_Structfield* _temp1827; struct _tuple12* _temp1822=(
struct _tuple12*)({ struct Cyc_List_List* _temp1821= fields; if( _temp1821 == 0){
_throw( Null_Exception);} _temp1821->hd;}); _temp1824=* _temp1822; _LL1828:
_temp1827= _temp1824.f1; goto _LL1826; _LL1826: _temp1825= _temp1824.f2; goto
_LL1823; _LL1823: { void* fieldtyp=( void*) _temp1827->type; Cyc_Toc_exp_to_c(
nv, _temp1825); if( Cyc_Toc_is_void_star( fieldtyp)){( void*)( _temp1825->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp1825->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0, _temp1366, 0), 0))->r));}
_free_region(& _temp1817); _pop_handler();} else{ _free_region(& _temp1817);
_throw( _temp1819);}}} goto _LL1197; _LL1255: { struct _tuple0* qv= _temp1372->name;
if( _temp1374->is_xtunion? 1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_var_exp( qv, 0))->r));} else{ int tag_count= 0; struct Cyc_List_List*
fields= _temp1374->fields == 0? 0:( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1834= _temp1374->fields; if( _temp1834 == 0){ _throw( Null_Exception);}
_temp1834->v;}); while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1831= fields; if( _temp1831 == 0){ _throw(
Null_Exception);} _temp1831->hd;}))->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1832= fields; if( _temp1832 == 0){ _throw(
Null_Exception);} _temp1832->hd;}))->typs == 0){ tag_count ++;} fields=({ struct
Cyc_List_List* _temp1833= fields; if( _temp1833 == 0){ _throw( Null_Exception);}
_temp1833->tl;});}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0))->r));} goto _LL1197;} _LL1257: { struct _tuple0* qv=
_temp1382->name; struct Cyc_List_List* tqts= _temp1382->typs; struct _tuple0* x=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); void*
strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1862=( char*)"_struct"; struct _tagged_string
_temp1863; _temp1863.curr= _temp1862; _temp1863.base= _temp1862; _temp1863.last_plus_one=
_temp1862 + 8; _temp1863;}))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1384->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( qv, 0);} else{ int
tag_count= 0; struct Cyc_List_List* fields= _temp1384->fields == 0? 0:( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1838= _temp1384->fields; if(
_temp1838 == 0){ _throw( Null_Exception);} _temp1838->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1835= fields;
if( _temp1835 == 0){ _throw( Null_Exception);} _temp1835->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1836= fields;
if( _temp1836 == 0){ _throw( Null_Exception);} _temp1836->hd;}))->typs != 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1837= fields; if( _temp1837
== 0){ _throw( Null_Exception);} _temp1837->tl;});} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles= 0; for( 0; _temp1386
!= 0; _temp1386=({ struct Cyc_List_List* _temp1839= _temp1386; if( _temp1839 ==
0){ _throw( Null_Exception);} _temp1839->tl;}), tqts=({ struct Cyc_List_List*
_temp1840= tqts; if( _temp1840 == 0){ _throw( Null_Exception);} _temp1840->tl;})){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1846= _temp1386; if( _temp1846 == 0){ _throw( Null_Exception);} _temp1846->hd;});
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp1845= tqts; if( _temp1845 == 0){ _throw( Null_Exception);} _temp1845->hd;}))).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} dles=({ struct Cyc_List_List* _temp1841=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1841[ 0]=({ struct Cyc_List_List
_temp1842; _temp1842.hd=( void*)({ struct _tuple4* _temp1843=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp1843[ 0]=({ struct _tuple4
_temp1844; _temp1844.f1= 0; _temp1844.f2= cur_e; _temp1844;}); _temp1843;});
_temp1842.tl= dles; _temp1842;}); _temp1841;});} dles=({ struct Cyc_List_List*
_temp1847=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1847[ 0]=({ struct Cyc_List_List _temp1848; _temp1848.hd=( void*)({ struct
_tuple4* _temp1849=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp1849[ 0]=({ struct _tuple4 _temp1850; _temp1850.f1= 0; _temp1850.f2=
tag_exp; _temp1850;}); _temp1849;}); _temp1848.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); _temp1848;}); _temp1847;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1851=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1851->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({
char* _temp1852=( char*)"_struct"; struct _tagged_string _temp1853; _temp1853.curr=
_temp1852; _temp1853.base= _temp1852; _temp1853.last_plus_one= _temp1852 + 8;
_temp1853;})); _temp1851;}), dles, 0))->r));}} else{ struct Cyc_List_List*
rev_assign=({ struct Cyc_List_List* _temp1861=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1861->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( xexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( qv, 0),
0); _temp1861->tl= 0; _temp1861;});{ int i= 1; for( 0; _temp1386 != 0;(
_temp1386=({ struct Cyc_List_List* _temp1854= _temp1386; if( _temp1854 == 0){
_throw( Null_Exception);} _temp1854->tl;}), i ++), tqts=({ struct Cyc_List_List*
_temp1855= tqts; if( _temp1855 == 0){ _throw( Null_Exception);} _temp1855->tl;})){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1859= _temp1386; if( _temp1859 == 0){ _throw( Null_Exception);} _temp1859->hd;});
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp1858= tqts; if( _temp1858 == 0){ _throw( Null_Exception);} _temp1858->hd;}))).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1857=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1857[ 0]= xprintf("f%d", i); _temp1857;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1856=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1856->hd=( void*) a; _temp1856->tl=
rev_assign; _temp1856;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1860=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1860->hd=(
void*) final_stmt; _temp1860->tl= rev_assign; _temp1860;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL1197;} _LL1259: goto _LL1197; _LL1261: { void* t_c= Cyc_Toc_typ_to_c(
_temp1398); if( _temp1400 != 0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1864= _temp1400; if( _temp1864 == 0){ _throw(
Null_Exception);} _temp1864;}); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)((
void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1398, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} goto _LL1197;} _LL1263: Cyc_Toc_stmt_to_c( nv, _temp1402); goto
_LL1197; _LL1265:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1865=( char*)"UnresolvedMem"; struct
_tagged_string _temp1866; _temp1866.curr= _temp1865; _temp1866.base= _temp1865;
_temp1866.last_plus_one= _temp1865 + 14; _temp1866;})); goto _LL1197; _LL1267:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1867=( char*)"codegen"; struct _tagged_string _temp1868; _temp1868.curr=
_temp1867; _temp1868.base= _temp1867; _temp1868.last_plus_one= _temp1867 + 8;
_temp1868;})); goto _LL1197; _LL1269:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1869=( char*)"fill"; struct _tagged_string
_temp1870; _temp1870.curr= _temp1869; _temp1870.base= _temp1869; _temp1870.last_plus_one=
_temp1869 + 5; _temp1870;})); goto _LL1197; _LL1197:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple14{ struct _tuple0* f1; void* f2;
} ; struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, void* t,
struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat* p,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1872=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1908; struct Cyc_Absyn_Vardecl _temp1910; struct Cyc_List_List* _temp1911;
struct Cyc_Core_Opt* _temp1913; struct Cyc_Absyn_Exp* _temp1915; void* _temp1917;
struct Cyc_Absyn_Tqual _temp1919; struct _tuple0* _temp1921; void* _temp1923;
struct Cyc_Absyn_Vardecl* _temp1925; int _temp1927; void* _temp1929; char
_temp1931; struct _tagged_string _temp1933; struct Cyc_Absyn_Enumfield*
_temp1935; struct Cyc_Absyn_Enumdecl* _temp1937; struct Cyc_List_List* _temp1939;
struct Cyc_List_List* _temp1941; struct Cyc_Absyn_Tunionfield* _temp1943; struct
Cyc_Absyn_Tuniondecl* _temp1945; struct Cyc_Absyn_Pat* _temp1948; struct Cyc_Absyn_Pat
_temp1950; struct Cyc_Position_Segment* _temp1951; struct Cyc_Core_Opt*
_temp1953; void* _temp1955; struct Cyc_List_List* _temp1957; struct Cyc_List_List*
_temp1959; struct Cyc_Absyn_Tunionfield* _temp1961; struct Cyc_Absyn_Tuniondecl*
_temp1963; struct Cyc_List_List* _temp1965; struct Cyc_List_List* _temp1967;
struct Cyc_Absyn_Tunionfield* _temp1969; struct Cyc_Absyn_Tuniondecl* _temp1971;
struct Cyc_List_List* _temp1973; struct Cyc_List_List* _temp1975; struct Cyc_List_List*
_temp1977; struct Cyc_Core_Opt* _temp1979; struct Cyc_Absyn_Structdecl*
_temp1981; struct Cyc_Absyn_Pat* _temp1983; struct _tuple0* _temp1985; struct
Cyc_List_List* _temp1987; struct Cyc_List_List* _temp1989; struct _tuple0*
_temp1991; struct Cyc_List_List* _temp1993; struct Cyc_List_List* _temp1995;
struct _tuple0* _temp1997; _LL1874: if(( unsigned int) _temp1872 > 2u?(( struct
_tunion_struct*) _temp1872)->tag == Cyc_Absyn_Var_p: 0){ _LL1909: _temp1908=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*) _temp1872)->f1;
_temp1910=* _temp1908; _LL1924: _temp1923=( void*) _temp1910.sc; goto _LL1922;
_LL1922: _temp1921=( struct _tuple0*) _temp1910.name; goto _LL1920; _LL1920:
_temp1919=( struct Cyc_Absyn_Tqual) _temp1910.tq; goto _LL1918; _LL1918:
_temp1917=( void*) _temp1910.type; goto _LL1916; _LL1916: _temp1915=( struct Cyc_Absyn_Exp*)
_temp1910.initializer; goto _LL1914; _LL1914: _temp1913=( struct Cyc_Core_Opt*)
_temp1910.rgn; goto _LL1912; _LL1912: _temp1911=( struct Cyc_List_List*)
_temp1910.attributes; goto _LL1875;} else{ goto _LL1876;} _LL1876: if( _temp1872
==( void*) Cyc_Absyn_Wild_p){ goto _LL1877;} else{ goto _LL1878;} _LL1878: if((
unsigned int) _temp1872 > 2u?(( struct _tunion_struct*) _temp1872)->tag == Cyc_Absyn_Reference_p:
0){ _LL1926: _temp1925=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1872)->f1; goto _LL1879;} else{ goto _LL1880;} _LL1880: if( _temp1872 ==(
void*) Cyc_Absyn_Null_p){ goto _LL1881;} else{ goto _LL1882;} _LL1882: if((
unsigned int) _temp1872 > 2u?(( struct _tunion_struct*) _temp1872)->tag == Cyc_Absyn_Int_p:
0){ _LL1930: _temp1929=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1872)->f1;
goto _LL1928; _LL1928: _temp1927=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1872)->f2; goto _LL1883;} else{ goto _LL1884;} _LL1884: if(( unsigned int)
_temp1872 > 2u?(( struct _tunion_struct*) _temp1872)->tag == Cyc_Absyn_Char_p: 0){
_LL1932: _temp1931=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1872)->f1;
goto _LL1885;} else{ goto _LL1886;} _LL1886: if(( unsigned int) _temp1872 > 2u?((
struct _tunion_struct*) _temp1872)->tag == Cyc_Absyn_Float_p: 0){ _LL1934:
_temp1933=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1872)->f1;
goto _LL1887;} else{ goto _LL1888;} _LL1888: if(( unsigned int) _temp1872 > 2u?((
struct _tunion_struct*) _temp1872)->tag == Cyc_Absyn_Enum_p: 0){ _LL1938:
_temp1937=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1872)->f1; goto _LL1936; _LL1936: _temp1935=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1872)->f2; goto _LL1889;} else{ goto
_LL1890;} _LL1890: if(( unsigned int) _temp1872 > 2u?(( struct _tunion_struct*)
_temp1872)->tag == Cyc_Absyn_Tunion_p: 0){ _LL1946: _temp1945=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1872)->f1; goto _LL1944; _LL1944:
_temp1943=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1872)->f2; goto _LL1942; _LL1942: _temp1941=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1872)->f3; goto _LL1940; _LL1940:
_temp1939=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1872)->f4; if( _temp1939 == 0){ goto _LL1891;} else{ goto _LL1892;}} else{
goto _LL1892;} _LL1892: if(( unsigned int) _temp1872 > 2u?(( struct
_tunion_struct*) _temp1872)->tag == Cyc_Absyn_Pointer_p: 0){ _LL1949: _temp1948=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*) _temp1872)->f1;
_temp1950=* _temp1948; _LL1956: _temp1955=( void*) _temp1950.r; if((
unsigned int) _temp1955 > 2u?(( struct _tunion_struct*) _temp1955)->tag == Cyc_Absyn_Tunion_p:
0){ _LL1964: _temp1963=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1955)->f1; goto _LL1962; _LL1962: _temp1961=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1955)->f2; goto _LL1960; _LL1960:
_temp1959=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1955)->f3; goto _LL1958; _LL1958: _temp1957=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1955)->f4; goto _LL1954;} else{ goto
_LL1894;} _LL1954: _temp1953=( struct Cyc_Core_Opt*) _temp1950.topt; goto
_LL1952; _LL1952: _temp1951=( struct Cyc_Position_Segment*) _temp1950.loc; goto
_LL1947;} else{ goto _LL1894;} _LL1947: if( _temp1957 != 0){ goto _LL1893;}
else{ goto _LL1894;} _LL1894: if(( unsigned int) _temp1872 > 2u?(( struct
_tunion_struct*) _temp1872)->tag == Cyc_Absyn_Tunion_p: 0){ _LL1972: _temp1971=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1872)->f1;
goto _LL1970; _LL1970: _temp1969=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1872)->f2; goto _LL1968; _LL1968: _temp1967=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1872)->f3; goto _LL1966; _LL1966:
_temp1965=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1872)->f4; goto _LL1895;} else{ goto _LL1896;} _LL1896: if(( unsigned int)
_temp1872 > 2u?(( struct _tunion_struct*) _temp1872)->tag == Cyc_Absyn_Tuple_p:
0){ _LL1974: _temp1973=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1872)->f1; goto _LL1897;} else{ goto _LL1898;} _LL1898: if(( unsigned int)
_temp1872 > 2u?(( struct _tunion_struct*) _temp1872)->tag == Cyc_Absyn_Struct_p:
0){ _LL1982: _temp1981=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1872)->f1; goto _LL1980; _LL1980: _temp1979=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1872)->f2; goto _LL1978; _LL1978:
_temp1977=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1872)->f3; goto _LL1976; _LL1976: _temp1975=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1872)->f4; goto _LL1899;} else{ goto
_LL1900;} _LL1900: if(( unsigned int) _temp1872 > 2u?(( struct _tunion_struct*)
_temp1872)->tag == Cyc_Absyn_Pointer_p: 0){ _LL1984: _temp1983=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1872)->f1; goto _LL1901;} else{ goto
_LL1902;} _LL1902: if(( unsigned int) _temp1872 > 2u?(( struct _tunion_struct*)
_temp1872)->tag == Cyc_Absyn_UnknownId_p: 0){ _LL1986: _temp1985=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1872)->f1; goto _LL1903;}
else{ goto _LL1904;} _LL1904: if(( unsigned int) _temp1872 > 2u?(( struct
_tunion_struct*) _temp1872)->tag == Cyc_Absyn_UnknownCall_p: 0){ _LL1992:
_temp1991=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1872)->f1;
goto _LL1990; _LL1990: _temp1989=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1872)->f2; goto _LL1988; _LL1988: _temp1987=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1872)->f3; goto _LL1905;} else{
goto _LL1906;} _LL1906: if(( unsigned int) _temp1872 > 2u?(( struct
_tunion_struct*) _temp1872)->tag == Cyc_Absyn_UnknownFields_p: 0){ _LL1998:
_temp1997=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1872)->f1; goto _LL1996; _LL1996: _temp1995=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1872)->f2; goto _LL1994; _LL1994:
_temp1993=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1872)->f3; goto _LL1907;} else{ goto _LL1873;} _LL1875: nv= Cyc_Toc_add_varmap(
nv, _temp1921, r); goto _LL1877; _LL1877: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL1873; _LL1879: { struct _tuple0* v= Cyc_Toc_temp_var(); decls=({ struct
Cyc_List_List* _temp1999=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1999->hd=( void*)({ struct _tuple14* _temp2000=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2000->f1= v; _temp2000->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2000;}); _temp1999->tl= decls;
_temp1999;}); nv= Cyc_Toc_add_varmap( nv, _temp1925->name, Cyc_Absyn_var_exp( v,
0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp(
path, 0), 0), Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1873;} _LL1881: s=
Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab);
goto _LL1873; _LL1883: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp(
_temp1929, _temp1927, 0), succ_lab, fail_lab); goto _LL1873; _LL1885: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_char_exp( _temp1931, 0), succ_lab, fail_lab); goto _LL1873; _LL1887:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp( _temp1933, 0), succ_lab,
fail_lab); goto _LL1873; _LL1889: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2001=( struct Cyc_Absyn_Enum_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct)); _temp2001[ 0]=({ struct Cyc_Absyn_Enum_e_struct
_temp2002; _temp2002.tag= Cyc_Absyn_Enum_e; _temp2002.f1= _temp1935->name;
_temp2002.f2=( struct Cyc_Absyn_Enumdecl*) _temp1937; _temp2002.f3=( struct Cyc_Absyn_Enumfield*)
_temp1935; _temp2002;}); _temp2001;}), 0), succ_lab, fail_lab); goto _LL1873;
_LL1891: { struct Cyc_Absyn_Exp* cmp_exp= Cyc_Absyn_var_exp( _temp1943->name, 0);
if( ! _temp1945->is_xtunion){ cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt( r, cmp_exp, succ_lab, fail_lab); goto
_LL1873;} _LL1893: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1957); struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag(
_temp1961->name,( struct _tagged_string)({ char* _temp2014=( char*)"_struct";
struct _tagged_string _temp2015; _temp2015.curr= _temp2014; _temp2015.base=
_temp2014; _temp2015.last_plus_one= _temp2014 + 8; _temp2015;})); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq),
r, 0);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1957); for( 0; rps != 0; rps=({ struct Cyc_List_List*
_temp2003= rps; if( _temp2003 == 0){ _throw( Null_Exception);} _temp2003->tl;}),
cnt --){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp2010= rps; if( _temp2010 == 0){ _throw( Null_Exception);} _temp2010->hd;});
struct _tuple0* v= Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp2009= p2->topt; if( _temp2009 == 0){ _throw( Null_Exception);} _temp2009->v;});
void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp2004=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2004->hd=( void*)({ struct _tuple14*
_temp2005=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2005->f1=
v; _temp2005->f2= t2c; _temp2005;}); _temp2004->tl= decls; _temp2004;});{ struct
_tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp2008=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2008[ 0]= xprintf("f%d", cnt);
_temp2008;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp2007=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2007[ 0]= xprintf("f%d", cnt);
_temp2007;}), 0), 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3,
0); ss=({ struct Cyc_List_List* _temp2006=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2006->hd=( void*) Cyc_Absyn_label_stmt( lab,
s2, 0); _temp2006->tl= ss; _temp2006;}); succ_lab= lab;}}}}{ struct Cyc_Absyn_Exp*
test_exp; if( _temp1963->is_xtunion){ struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xtunion_struct_sp), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_structmember_exp(
e2, Cyc_Toc_tag_sp, 0), 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1961->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2013= _temp1963->fields; if(
_temp2013 == 0){ _throw( Null_Exception);} _temp2013->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp2011= fs; if( _temp2011 == 0){ _throw( Null_Exception);}
_temp2011->tl;})){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp2012= fs; if( _temp2012 == 0){ _throw( Null_Exception);}
_temp2012->hd;}); if( f->typs == 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__tunion_struct_sp), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structarrow_exp( e3, Cyc_Toc_tag_sp,
0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1961->name,
0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0):
Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL1873;}} _LL1895:
_temp1973= _temp1965; goto _LL1897; _LL1897: { struct Cyc_List_List* ss= 0; int
cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1973);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1973); for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp2016= rps; if(
_temp2016 == 0){ _throw( Null_Exception);} _temp2016->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2023= rps; if(
_temp2023 == 0){ _throw( Null_Exception);} _temp2023->hd;}); struct _tuple0* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp2022= p2->topt;
if( _temp2022 == 0){ _throw( Null_Exception);} _temp2022->v;}); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2017=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2017->hd=( void*)({ struct _tuple14* _temp2018=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2018->f1= v; _temp2018->f2= Cyc_Toc_typ_to_c(
t2); _temp2018;}); _temp2017->tl= decls; _temp2017;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp2021=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2021[ 0]= xprintf("f%d", cnt);
_temp2021;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp2020=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2020[ 0]= xprintf("f%d", cnt); _temp2020;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp2019=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2019->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp2019->tl= ss; _temp2019;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1873;} _LL1899: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1975); for( 0; dlps != 0; dlps=({ struct Cyc_List_List* _temp2024= dlps;
if( _temp2024 == 0){ _throw( Null_Exception);} _temp2024->tl;})){ struct
_tuple15* tup=( struct _tuple15*)({ struct Cyc_List_List* _temp2038= dlps; if(
_temp2038 == 0){ _throw( Null_Exception);} _temp2038->hd;}); struct Cyc_Absyn_Pat*
p2=(* tup).f2; struct _tagged_string* f;{ void* _temp2026=( void*)({ struct Cyc_List_List*
_temp2025=(* tup).f1; if( _temp2025 == 0){ _throw( Null_Exception);} _temp2025->hd;});
struct _tagged_string* _temp2032; _LL2028: if((( struct _tunion_struct*)
_temp2026)->tag == Cyc_Absyn_FieldName){ _LL2033: _temp2032=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp2026)->f1; goto
_LL2029;} else{ goto _LL2030;} _LL2030: goto _LL2031; _LL2029: f= _temp2032;
goto _LL2027; _LL2031:( void) _throw(( void*) Cyc_Toc_Match_error); _LL2027:;}{
struct _tuple0* v= Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp2037= p2->topt; if( _temp2037 == 0){ _throw( Null_Exception);} _temp2037->v;});
void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp2034=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2034->hd=( void*)({ struct _tuple14*
_temp2035=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2035->f1=
v; _temp2035->f2= t2c; _temp2035;}); _temp2034->tl= decls; _temp2034;});{ struct
_tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path, f, 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp(
r, f, 0), 0), 0), s2, 0); ss=({ struct Cyc_List_List* _temp2036=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2036->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp2036->tl= ss; _temp2036;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1873;} _LL1901: { struct _tuple0* v= Cyc_Toc_temp_var(); void*
t2=( void*)({ struct Cyc_Core_Opt* _temp2041= _temp1983->topt; if( _temp2041 ==
0){ _throw( Null_Exception);} _temp2041->v;}); decls=({ struct Cyc_List_List*
_temp2039=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2039->hd=( void*)({ struct _tuple14* _temp2040=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2040->f1= v; _temp2040->f2= Cyc_Toc_typ_to_c(
t2); _temp2040;}); _temp2039->tl= decls; _temp2039;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path,
0), _temp1983, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0),
s2, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);}
else{ s= s3;} goto _LL1873;}}} _LL1903: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2042=(
char*)"unknownid"; struct _tagged_string _temp2043; _temp2043.curr= _temp2042;
_temp2043.base= _temp2042; _temp2043.last_plus_one= _temp2042 + 10; _temp2043;}));
goto _LL1873; _LL1905: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2044=( char*)"unknowncall";
struct _tagged_string _temp2045; _temp2045.curr= _temp2044; _temp2045.base=
_temp2044; _temp2045.last_plus_one= _temp2044 + 12; _temp2045;})); goto _LL1873;
_LL1907: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2046=( char*)"unknownfields"; struct
_tagged_string _temp2047; _temp2047.curr= _temp2046; _temp2047.base= _temp2046;
_temp2047.last_plus_one= _temp2046 + 14; _temp2047;})); goto _LL1873; _LL1873:;}
return({ struct _tuple13 _temp2048; _temp2048.f1= nv; _temp2048.f2= decls;
_temp2048.f3= s; _temp2048;});} struct _tuple16{ struct _tagged_string* f1;
struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause* f3; } ; static struct
_tuple16* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause* sc){ return({
struct _tuple16* _temp2049=( struct _tuple16*) GC_malloc( sizeof( struct
_tuple16)); _temp2049->f1= Cyc_Toc_fresh_label(); _temp2049->f2= Cyc_Toc_fresh_label();
_temp2049->f3= sc; _temp2049;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* t=( void*)({ struct Cyc_Core_Opt*
_temp2105= e->topt; if( _temp2105 == 0){ _throw( Null_Exception);} _temp2105->v;});
int leave_as_switch;{ void* _temp2050= Cyc_Tcutil_compress( t); void* _temp2056;
void* _temp2058; _LL2052: if(( unsigned int) _temp2050 > 4u?(( struct
_tunion_struct*) _temp2050)->tag == Cyc_Absyn_IntType: 0){ _LL2059: _temp2058=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2050)->f1; goto _LL2057; _LL2057:
_temp2056=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2050)->f2; goto
_LL2053;} else{ goto _LL2054;} _LL2054: goto _LL2055; _LL2053: leave_as_switch=
1; goto _LL2051; _LL2055: leave_as_switch= 0; goto _LL2051; _LL2051:;}{ struct
Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1=({ struct Cyc_List_List*
_temp2060= scs1; if( _temp2060 == 0){ _throw( Null_Exception);} _temp2060->tl;})){
if(( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2062=(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2061= scs1; if( _temp2061 == 0){ _throw(
Null_Exception);} _temp2061->hd;}))->pat_vars; if( _temp2062 == 0){ _throw(
Null_Exception);} _temp2062->v;}) != 0){ leave_as_switch= 0; break;}}} if(
leave_as_switch){ struct _tagged_string* next_l= Cyc_Toc_fresh_label();{ struct
Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1=({ struct Cyc_List_List*
_temp2063= scs1; if( _temp2063 == 0){ _throw( Null_Exception);} _temp2063->tl;})){
struct Cyc_Absyn_Stmt* s=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List*
_temp2065= scs1; if( _temp2065 == 0){ _throw( Null_Exception);} _temp2065->hd;}))->body;((
struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp2064= scs1; if(
_temp2064 == 0){ _throw( Null_Exception);} _temp2064->hd;}))->body= Cyc_Absyn_label_stmt(
next_l, s, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), s);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0);
struct _tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple16*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* lscs2= lscs; for( 0; lscs2 != 0; lscs2=({ struct Cyc_List_List*
_temp2066= lscs2; if( _temp2066 == 0){ _throw( Null_Exception);} _temp2066->tl;})){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2078= lscs2; if( _temp2078 == 0){ _throw( Null_Exception);} _temp2078->hd;}))).f3;
struct _tagged_string* fail_lab=({ struct Cyc_List_List* _temp2075= lscs2; if(
_temp2075 == 0){ _throw( Null_Exception);} _temp2075->tl;}) == 0? end_l:(*((
struct _tuple16*)({ struct Cyc_List_List* _temp2077=({ struct Cyc_List_List*
_temp2076= lscs2; if( _temp2076 == 0){ _throw( Null_Exception);} _temp2076->tl;});
if( _temp2077 == 0){ _throw( Null_Exception);} _temp2077->hd;}))).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2074= lscs2; if( _temp2074 == 0){ _throw( Null_Exception);} _temp2074->hd;}))).f2;
if( sc->where_clause == 0){ struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r,
path, sc->pattern, succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List*
_temp2067=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2067->hd=( void*) res.f1; _temp2067->tl= nvs; _temp2067;}); decls= res.f2;
test_stmts=({ struct Cyc_List_List* _temp2068=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2068->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2069= lscs2; if( _temp2069 == 0){
_throw( Null_Exception);} _temp2069->hd;}))).f1, res.f3, 0); _temp2068->tl=
test_stmts; _temp2068;});} else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2073= sc->where_clause; if( _temp2073 == 0){ _throw(
Null_Exception);} _temp2073;}); struct _tagged_string* intermed_lab= Cyc_Toc_fresh_label();
struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r, path, sc->pattern,
intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c( res.f1, e); nvs=({ struct Cyc_List_List*
_temp2070=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2070->hd=( void*) res.f1; _temp2070->tl= nvs; _temp2070;}); decls= res.f2;{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt( e, Cyc_Absyn_goto_stmt(
succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3=
Cyc_Absyn_label_stmt( intermed_lab, s2, 0); test_stmts=({ struct Cyc_List_List*
_temp2071=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2071->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2072= lscs2; if( _temp2072 == 0){ _throw( Null_Exception);} _temp2072->hd;}))).f1,
Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0); _temp2071->tl= test_stmts; _temp2071;});}}}}{
struct Cyc_Absyn_Stmt* test_stmt= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( test_stmts), 0); nvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt*
clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){ for( 0;({ struct Cyc_List_List*
_temp2079= lscs; if( _temp2079 == 0){ _throw( Null_Exception);} _temp2079->tl;})
!= 0; lscs=({ struct Cyc_List_List* _temp2080= lscs; if( _temp2080 == 0){ _throw(
Null_Exception);} _temp2080->tl;}), nvs=({ struct Cyc_List_List* _temp2081= nvs;
if( _temp2081 == 0){ _throw( Null_Exception);} _temp2081->tl;})){ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2091= nvs; if( _temp2091
== 0){ _throw( Null_Exception);} _temp2091->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2090= lscs; if( _temp2090 == 0){
_throw( Null_Exception);} _temp2090->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
e, end_l,(*(( struct _tuple16*)({ struct Cyc_List_List* _temp2083=({ struct Cyc_List_List*
_temp2082= lscs; if( _temp2082 == 0){ _throw( Null_Exception);} _temp2082->tl;});
if( _temp2083 == 0){ _throw( Null_Exception);} _temp2083->hd;}))).f2,( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2086=((*(( struct _tuple16*)({
struct Cyc_List_List* _temp2085=({ struct Cyc_List_List* _temp2084= lscs; if(
_temp2084 == 0){ _throw( Null_Exception);} _temp2084->tl;}); if( _temp2085 == 0){
_throw( Null_Exception);} _temp2085->hd;}))).f3)->pat_vars; if( _temp2086 == 0){
_throw( Null_Exception);} _temp2086->v;}),( struct Cyc_Toc_Env*)({ struct Cyc_List_List*
_temp2088=({ struct Cyc_List_List* _temp2087= nvs; if( _temp2087 == 0){ _throw(
Null_Exception);} _temp2087->tl;}); if( _temp2088 == 0){ _throw( Null_Exception);}
_temp2088->hd;})), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2089= lscs; if( _temp2089 == 0){
_throw( Null_Exception);} _temp2089->hd;}))).f2, s, 0), 0);}{ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2094= nvs; if( _temp2094
== 0){ _throw( Null_Exception);} _temp2094->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2093= lscs; if( _temp2093 == 0){
_throw( Null_Exception);} _temp2093->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env(
e, end_l), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2092= lscs; if( _temp2092 == 0){
_throw( Null_Exception);} _temp2092->hd;}))).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( test_stmt, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=({
struct Cyc_List_List* _temp2095= decls; if( _temp2095 == 0){ _throw(
Null_Exception);} _temp2095->tl;})){ struct _tuple14 _temp2099; void* _temp2100;
struct _tuple0* _temp2102; struct _tuple14* _temp2097=( struct _tuple14*)({
struct Cyc_List_List* _temp2096= decls; if( _temp2096 == 0){ _throw(
Null_Exception);} _temp2096->hd;}); _temp2099=* _temp2097; _LL2103: _temp2102=
_temp2099.f1; goto _LL2101; _LL2101: _temp2100= _temp2099.f2; goto _LL2098;
_LL2098: res= Cyc_Absyn_declare_stmt( _temp2102, _temp2100, 0, res, 0);}( void*)(
whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( v, Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp2104= e->topt; if( _temp2104 == 0){ _throw(
Null_Exception);} _temp2104->v;})),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2106=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2106[ 0]=({ struct Cyc_List_List _temp2107; _temp2107.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2107.tl= 0; _temp2107;}); _temp2106;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2108=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2148; struct Cyc_Absyn_Stmt* _temp2150; struct Cyc_Absyn_Stmt*
_temp2152; struct Cyc_Absyn_Exp* _temp2154; struct Cyc_Absyn_Stmt* _temp2156;
struct Cyc_Absyn_Stmt* _temp2158; struct Cyc_Absyn_Exp* _temp2160; struct Cyc_Absyn_Stmt*
_temp2162; struct _tuple2 _temp2164; struct Cyc_Absyn_Stmt* _temp2166; struct
Cyc_Absyn_Exp* _temp2168; struct Cyc_Absyn_Stmt* _temp2170; struct Cyc_Absyn_Stmt*
_temp2172; struct Cyc_Absyn_Stmt* _temp2174; struct _tagged_string* _temp2176;
struct Cyc_Absyn_Stmt* _temp2178; struct _tuple2 _temp2180; struct Cyc_Absyn_Stmt*
_temp2182; struct Cyc_Absyn_Exp* _temp2184; struct _tuple2 _temp2186; struct Cyc_Absyn_Stmt*
_temp2188; struct Cyc_Absyn_Exp* _temp2190; struct Cyc_Absyn_Exp* _temp2192;
struct Cyc_List_List* _temp2194; struct Cyc_Absyn_Exp* _temp2196; struct Cyc_Absyn_Switch_clause**
_temp2198; struct Cyc_List_List* _temp2200; struct Cyc_Absyn_Stmt* _temp2202;
struct Cyc_Absyn_Decl* _temp2204; struct Cyc_Absyn_Stmt* _temp2206; struct Cyc_Absyn_Stmt*
_temp2208; struct Cyc_Absyn_Stmt* _temp2210; struct _tagged_string* _temp2212;
struct _tuple2 _temp2214; struct Cyc_Absyn_Stmt* _temp2216; struct Cyc_Absyn_Exp*
_temp2218; struct Cyc_Absyn_Stmt* _temp2220; struct Cyc_List_List* _temp2222;
struct Cyc_Absyn_Stmt* _temp2224; struct Cyc_Absyn_Stmt* _temp2226; struct Cyc_Absyn_Vardecl*
_temp2228; struct Cyc_Absyn_Tvar* _temp2230; _LL2110: if( _temp2108 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2111;} else{ goto _LL2112;} _LL2112: if((
unsigned int) _temp2108 > 1u?(( struct _tunion_struct*) _temp2108)->tag == Cyc_Absyn_Exp_s:
0){ _LL2149: _temp2148=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp2108)->f1; goto _LL2113;} else{ goto _LL2114;} _LL2114: if(( unsigned int)
_temp2108 > 1u?(( struct _tunion_struct*) _temp2108)->tag == Cyc_Absyn_Seq_s: 0){
_LL2153: _temp2152=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp2108)->f1; goto _LL2151; _LL2151: _temp2150=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp2108)->f2; goto _LL2115;} else{ goto
_LL2116;} _LL2116: if(( unsigned int) _temp2108 > 1u?(( struct _tunion_struct*)
_temp2108)->tag == Cyc_Absyn_Return_s: 0){ _LL2155: _temp2154=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp2108)->f1; goto _LL2117;} else{ goto
_LL2118;} _LL2118: if(( unsigned int) _temp2108 > 1u?(( struct _tunion_struct*)
_temp2108)->tag == Cyc_Absyn_IfThenElse_s: 0){ _LL2161: _temp2160=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2108)->f1; goto _LL2159; _LL2159:
_temp2158=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2108)->f2; goto _LL2157; _LL2157: _temp2156=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2108)->f3; goto _LL2119;} else{ goto
_LL2120;} _LL2120: if(( unsigned int) _temp2108 > 1u?(( struct _tunion_struct*)
_temp2108)->tag == Cyc_Absyn_While_s: 0){ _LL2165: _temp2164=( struct _tuple2)((
struct Cyc_Absyn_While_s_struct*) _temp2108)->f1; _LL2169: _temp2168= _temp2164.f1;
goto _LL2167; _LL2167: _temp2166= _temp2164.f2; goto _LL2163; _LL2163: _temp2162=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp2108)->f2; goto
_LL2121;} else{ goto _LL2122;} _LL2122: if(( unsigned int) _temp2108 > 1u?((
struct _tunion_struct*) _temp2108)->tag == Cyc_Absyn_Break_s: 0){ _LL2171:
_temp2170=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp2108)->f1; goto _LL2123;} else{ goto _LL2124;} _LL2124: if(( unsigned int)
_temp2108 > 1u?(( struct _tunion_struct*) _temp2108)->tag == Cyc_Absyn_Continue_s:
0){ _LL2173: _temp2172=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp2108)->f1; goto _LL2125;} else{ goto _LL2126;} _LL2126: if(( unsigned int)
_temp2108 > 1u?(( struct _tunion_struct*) _temp2108)->tag == Cyc_Absyn_Goto_s: 0){
_LL2177: _temp2176=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp2108)->f1; goto _LL2175; _LL2175: _temp2174=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp2108)->f2; goto _LL2127;} else{ goto
_LL2128;} _LL2128: if(( unsigned int) _temp2108 > 1u?(( struct _tunion_struct*)
_temp2108)->tag == Cyc_Absyn_For_s: 0){ _LL2193: _temp2192=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_For_s_struct*) _temp2108)->f1; goto _LL2187; _LL2187: _temp2186=(
struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp2108)->f2; _LL2191:
_temp2190= _temp2186.f1; goto _LL2189; _LL2189: _temp2188= _temp2186.f2; goto
_LL2181; _LL2181: _temp2180=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp2108)->f3; _LL2185: _temp2184= _temp2180.f1; goto _LL2183; _LL2183:
_temp2182= _temp2180.f2; goto _LL2179; _LL2179: _temp2178=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_For_s_struct*) _temp2108)->f4; goto _LL2129;} else{ goto
_LL2130;} _LL2130: if(( unsigned int) _temp2108 > 1u?(( struct _tunion_struct*)
_temp2108)->tag == Cyc_Absyn_Switch_s: 0){ _LL2197: _temp2196=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp2108)->f1; goto _LL2195; _LL2195:
_temp2194=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp2108)->f2; goto _LL2131;} else{ goto _LL2132;} _LL2132: if(( unsigned int)
_temp2108 > 1u?(( struct _tunion_struct*) _temp2108)->tag == Cyc_Absyn_Fallthru_s:
0){ _LL2201: _temp2200=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2108)->f1; goto _LL2199; _LL2199: _temp2198=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp2108)->f2; goto _LL2133;} else{ goto
_LL2134;} _LL2134: if(( unsigned int) _temp2108 > 1u?(( struct _tunion_struct*)
_temp2108)->tag == Cyc_Absyn_Decl_s: 0){ _LL2205: _temp2204=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp2108)->f1; goto _LL2203; _LL2203:
_temp2202=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp2108)->f2;
goto _LL2135;} else{ goto _LL2136;} _LL2136: if(( unsigned int) _temp2108 > 1u?((
struct _tunion_struct*) _temp2108)->tag == Cyc_Absyn_Cut_s: 0){ _LL2207:
_temp2206=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*) _temp2108)->f1;
goto _LL2137;} else{ goto _LL2138;} _LL2138: if(( unsigned int) _temp2108 > 1u?((
struct _tunion_struct*) _temp2108)->tag == Cyc_Absyn_Splice_s: 0){ _LL2209:
_temp2208=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp2108)->f1; goto _LL2139;} else{ goto _LL2140;} _LL2140: if(( unsigned int)
_temp2108 > 1u?(( struct _tunion_struct*) _temp2108)->tag == Cyc_Absyn_Label_s:
0){ _LL2213: _temp2212=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp2108)->f1; goto _LL2211; _LL2211: _temp2210=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp2108)->f2; goto _LL2141;} else{ goto
_LL2142;} _LL2142: if(( unsigned int) _temp2108 > 1u?(( struct _tunion_struct*)
_temp2108)->tag == Cyc_Absyn_Do_s: 0){ _LL2221: _temp2220=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp2108)->f1; goto _LL2215; _LL2215: _temp2214=(
struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*) _temp2108)->f2; _LL2219:
_temp2218= _temp2214.f1; goto _LL2217; _LL2217: _temp2216= _temp2214.f2; goto
_LL2143;} else{ goto _LL2144;} _LL2144: if(( unsigned int) _temp2108 > 1u?((
struct _tunion_struct*) _temp2108)->tag == Cyc_Absyn_TryCatch_s: 0){ _LL2225:
_temp2224=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2108)->f1; goto _LL2223; _LL2223: _temp2222=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp2108)->f2; goto _LL2145;} else{ goto
_LL2146;} _LL2146: if(( unsigned int) _temp2108 > 1u?(( struct _tunion_struct*)
_temp2108)->tag == Cyc_Absyn_Region_s: 0){ _LL2231: _temp2230=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_Region_s_struct*) _temp2108)->f1; goto _LL2229; _LL2229:
_temp2228=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2108)->f2; goto _LL2227; _LL2227: _temp2226=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Region_s_struct*) _temp2108)->f3; goto _LL2147;} else{ goto
_LL2109;} _LL2111: return; _LL2113: Cyc_Toc_exp_to_c( nv, _temp2148); return;
_LL2115: Cyc_Toc_stmt_to_c( nv, _temp2152); s= _temp2150; continue; _LL2117: {
struct Cyc_Core_Opt* topt= 0; if( _temp2154 != 0){ topt=({ struct Cyc_Core_Opt*
_temp2232=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2232->v=( void*) Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp2234=({
struct Cyc_Absyn_Exp* _temp2233= _temp2154; if( _temp2233 == 0){ _throw(
Null_Exception);} _temp2233->topt;}); if( _temp2234 == 0){ _throw(
Null_Exception);} _temp2234->v;})); _temp2232;}); Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2235= _temp2154; if( _temp2235 == 0){
_throw( Null_Exception);} _temp2235;}));} if( s->try_depth > 0){ if( topt != 0){
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* retn_stmt= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x, 0), 0);( void*)( s->r=( void*)((
void*)( Cyc_Absyn_declare_stmt( x,( void*)({ struct Cyc_Core_Opt* _temp2236=
topt; if( _temp2236 == 0){ _throw( Null_Exception);} _temp2236->v;}), _temp2154,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), retn_stmt, 0), 0))->r));}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2119: Cyc_Toc_exp_to_c(
nv, _temp2160); Cyc_Toc_stmt_to_c( nv, _temp2158); s= _temp2156; continue;
_LL2121: Cyc_Toc_exp_to_c( nv, _temp2168); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2162); return; _LL2123: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)({ struct Cyc_Core_Opt*
_temp2237= nv->break_lab; if( _temp2237 == 0){ _throw( Null_Exception);}
_temp2237->v;}), 0))->r));}{ int dest_depth= _temp2170 == 0? 0:({ struct Cyc_Absyn_Stmt*
_temp2238= _temp2170; if( _temp2238 == 0){ _throw( Null_Exception);} _temp2238->try_depth;});
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL2125: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)({ struct Cyc_Core_Opt* _temp2239= nv->continue_lab; if(
_temp2239 == 0){ _throw( Null_Exception);} _temp2239->v;}), 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2240= _temp2172; if( _temp2240 == 0){
_throw( Null_Exception);} _temp2240->try_depth;}), s); return; _LL2127: Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2241= _temp2174; if( _temp2241 == 0){
_throw( Null_Exception);} _temp2241->try_depth;}), s); return; _LL2129: Cyc_Toc_exp_to_c(
nv, _temp2192); Cyc_Toc_exp_to_c( nv, _temp2190); Cyc_Toc_exp_to_c( nv,
_temp2184); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2178); return;
_LL2131: Cyc_Toc_xlate_switch( nv, s, _temp2196, _temp2194); return; _LL2133:
if( nv->fallthru_info == 0){( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2242=( char*)"fallthru in unexpected place";
struct _tagged_string _temp2243; _temp2243.curr= _temp2242; _temp2243.base=
_temp2242; _temp2243.last_plus_one= _temp2242 + 29; _temp2243;})));}{ struct
_tuple8 _temp2247; struct Cyc_Dict_Dict* _temp2248; struct Cyc_List_List*
_temp2250; struct _tagged_string* _temp2252; struct _tuple8* _temp2245=( struct
_tuple8*)({ struct Cyc_Core_Opt* _temp2244= nv->fallthru_info; if( _temp2244 ==
0){ _throw( Null_Exception);} _temp2244->v;}); _temp2247=* _temp2245; _LL2253:
_temp2252= _temp2247.f1; goto _LL2251; _LL2251: _temp2250= _temp2247.f2; goto
_LL2249; _LL2249: _temp2248= _temp2247.f3; goto _LL2246; _LL2246: { struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_goto_stmt( _temp2252, 0); Cyc_Toc_do_npop_before( s->try_depth -(({
struct Cyc_Absyn_Switch_clause** _temp2254= _temp2198; if( _temp2254 == 0){
_throw( Null_Exception);}* _temp2254;})->body)->try_depth, s2);{ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2250); struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2200); for( 0; vs2 != 0; vs2=({ struct Cyc_List_List*
_temp2255= vs2; if( _temp2255 == 0){ _throw( Null_Exception);} _temp2255->tl;}),
es2=({ struct Cyc_List_List* _temp2256= es2; if( _temp2256 == 0){ _throw(
Null_Exception);} _temp2256->tl;})){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp2257= es2; if( _temp2257 == 0){ _throw(
Null_Exception);} _temp2257->hd;})); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2248,( struct _tuple0*)({ struct Cyc_List_List* _temp2258= vs2; if(
_temp2258 == 0){ _throw( Null_Exception);} _temp2258->hd;})),( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp2259= es2; if( _temp2259 == 0){ _throw(
Null_Exception);} _temp2259->hd;}), 0), s2, 0);}( void*)( s->r=( void*)(( void*)
s2->r)); return;}}} _LL2135:{ void* _temp2260=( void*) _temp2204->r; int
_temp2268; struct Cyc_Absyn_Exp* _temp2270; struct Cyc_Core_Opt* _temp2272;
struct Cyc_Core_Opt* _temp2274; struct Cyc_Absyn_Pat* _temp2276; struct Cyc_Absyn_Vardecl*
_temp2278; _LL2262: if((( struct _tunion_struct*) _temp2260)->tag == Cyc_Absyn_Let_d){
_LL2277: _temp2276=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2260)->f1; goto _LL2275; _LL2275: _temp2274=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2260)->f2; goto _LL2273; _LL2273: _temp2272=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2260)->f3; goto
_LL2271; _LL2271: _temp2270=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2260)->f4; goto _LL2269; _LL2269: _temp2268=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp2260)->f5; goto _LL2263;} else{ goto _LL2264;} _LL2264: if((( struct
_tunion_struct*) _temp2260)->tag == Cyc_Absyn_Var_d){ _LL2279: _temp2278=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp2260)->f1;
goto _LL2265;} else{ goto _LL2266;} _LL2266: goto _LL2267; _LL2263: { int
var_case= 0; struct Cyc_Absyn_Vardecl* vdopt= 0;{ void* _temp2280=( void*)
_temp2276->r; struct Cyc_Absyn_Vardecl* _temp2286; _LL2282: if(( unsigned int)
_temp2280 > 2u?(( struct _tunion_struct*) _temp2280)->tag == Cyc_Absyn_Var_p: 0){
_LL2287: _temp2286=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp2280)->f1; goto _LL2283;} else{ goto _LL2284;} _LL2284: goto _LL2285;
_LL2283: var_case= 1; vdopt=( struct Cyc_Absyn_Vardecl*) _temp2286; _temp2286->initializer=(
struct Cyc_Absyn_Exp*) _temp2270;( void*)( _temp2204->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2288=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2288[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2289; _temp2289.tag= Cyc_Absyn_Var_d; _temp2289.f1= _temp2286; _temp2289;});
_temp2288;}))); goto _LL2281; _LL2285: goto _LL2281; _LL2281:;} if( var_case){
_temp2278=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_Absyn_Vardecl* _temp2290=
vdopt; if( _temp2290 == 0){ _throw( Null_Exception);} _temp2290;}); goto _LL2265;}(
void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2276, _temp2272,
_temp2270, _temp2268, _temp2202))->r)); goto _LL2261;} _LL2265: { struct _tuple0*
c_name= _temp2278->name;( void*)( _temp2278->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2278->type));{ struct Cyc_Toc_Env* nv2= Cyc_Toc_add_varmap( nv,
c_name, Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2304=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2304[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2305; _temp2305.tag= Cyc_Absyn_Local_b;
_temp2305.f1= _temp2278; _temp2305;}); _temp2304;}), 0)); Cyc_Toc_stmt_to_c( nv2,
_temp2202); if( _temp2278->initializer != 0){ struct Cyc_Absyn_Exp* init=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2303= _temp2278->initializer;
if( _temp2303 == 0){ _throw( Null_Exception);} _temp2303;}); void* _temp2291=(
void*) init->r; struct Cyc_Absyn_Exp* _temp2297; struct Cyc_Absyn_Exp* _temp2299;
struct Cyc_Absyn_Vardecl* _temp2301; _LL2293: if((( struct _tunion_struct*)
_temp2291)->tag == Cyc_Absyn_Comprehension_e){ _LL2302: _temp2301=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2291)->f1; goto _LL2300; _LL2300:
_temp2299=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2291)->f2; goto _LL2298; _LL2298: _temp2297=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2291)->f3; goto _LL2294;} else{
goto _LL2295;} _LL2295: goto _LL2296; _LL2294: _temp2278->initializer= 0;( void*)(
_temp2202->r=( void*)(( void*)( Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
c_name, 0), _temp2301, _temp2299, _temp2297, Cyc_Absyn_new_stmt(( void*)
_temp2202->r, 0), 0))->r)); goto _LL2292; _LL2296: Cyc_Toc_exp_to_c( nv, init);
goto _LL2292; _LL2292:;} goto _LL2261;}} _LL2267:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2306=(
char*)"bad nested declaration within function"; struct _tagged_string _temp2307;
_temp2307.curr= _temp2306; _temp2307.base= _temp2306; _temp2307.last_plus_one=
_temp2306 + 39; _temp2307;})); goto _LL2261; _LL2261:;} return; _LL2137:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp2308=( char*)"cut"; struct _tagged_string _temp2309; _temp2309.curr=
_temp2308; _temp2309.base= _temp2308; _temp2309.last_plus_one= _temp2308 + 4;
_temp2309;})); return; _LL2139:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2310=( char*)"splice"; struct _tagged_string
_temp2311; _temp2311.curr= _temp2310; _temp2311.base= _temp2310; _temp2311.last_plus_one=
_temp2310 + 7; _temp2311;})); return; _LL2141: s= _temp2210; continue; _LL2143:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2220); Cyc_Toc_exp_to_c( nv,
_temp2218); return; _LL2145: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct
_tuple0* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2327=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2327->f1= Cyc_Toc_abs_ns; _temp2327->f2= Cyc_Toc__push_handler_sp;
_temp2327;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple0* _temp2326=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2326->f1= Cyc_Toc_abs_ns; _temp2326->f2= Cyc_Toc_setjmp_sp; _temp2326;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2325=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2325->f1=
Cyc_Toc_abs_ns; _temp2325->f2= Cyc_Toc__pop_handler_sp; _temp2325;}), 0); e_exp->topt=({
struct Cyc_Core_Opt* _temp2312=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2312->v=( void*) e_typ; _temp2312;}); nv= Cyc_Toc_add_varmap(
nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2224);{ struct Cyc_Absyn_Stmt*
tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp2224, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0); struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl* x_vd= Cyc_Absyn_new_vardecl(
x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt* _temp2313=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2313->v=( void*)
Cyc_Absyn_exn_typ; _temp2313;});{ struct Cyc_Absyn_Pat* p=({ struct Cyc_Absyn_Pat*
_temp2321=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof( struct Cyc_Absyn_Pat));
_temp2321->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp2323=(
struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2323[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2324; _temp2324.tag= Cyc_Absyn_Var_p;
_temp2324.f1= x_vd; _temp2324;}); _temp2323;})); _temp2321->topt=({ struct Cyc_Core_Opt*
_temp2322=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2322->v=( void*) Cyc_Absyn_exn_typ; _temp2322;}); _temp2321->loc= 0;
_temp2321;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp2314=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2314->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2314;});{
struct Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp2318=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2318->pattern= p;
_temp2318->pat_vars=({ struct Cyc_Core_Opt* _temp2319=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2319->v=( void*)({ struct Cyc_List_List*
_temp2320=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2320->hd=( void*) x_vd; _temp2320->tl= 0; _temp2320;}); _temp2319;});
_temp2318->where_clause= 0; _temp2318->body= rs; _temp2318->loc= 0; _temp2318;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2222,({
struct Cyc_List_List* _temp2317=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2317->hd=( void*) default_case; _temp2317->tl= 0;
_temp2317;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( e_typ, Cyc_Absyn_fncall_exp( setjmp_exp,({
struct Cyc_List_List* _temp2316=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2316->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2316->tl= 0; _temp2316;}), 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List*
_temp2315=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2315->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2315->tl= 0;
_temp2315;}), 0), 0), Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Not,
e_exp, 0), tryandpop_stmt, handler_stmt, 0), 0), 0), 0))->r)); return;}}}}}
_LL2147: { void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__region_handle_sp);
void* rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq);
void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct _tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0*
x_var= _temp2228->name; struct _tuple0* h_var= Cyc_Toc_temp_var(); struct
_tuple0* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2336=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2336[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2337; _temp2337.tag= Cyc_Absyn_Local_b; _temp2337.f1=
_temp2228; _temp2337;}); _temp2336;}), 0); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp*
pushhandler_exp= Cyc_Absyn_var_exp(({ struct _tuple0* _temp2335=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2335->f1= Cyc_Toc_abs_ns; _temp2335->f2=
Cyc_Toc__push_handler_sp; _temp2335;}), 0); struct Cyc_Absyn_Exp* setjmp_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2334=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2334->f1= Cyc_Toc_abs_ns; _temp2334->f2= Cyc_Toc_setjmp_sp;
_temp2334;}), 0); struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2333=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2333->f1= Cyc_Toc_abs_ns; _temp2333->f2= Cyc_Toc__pop_handler_sp; _temp2333;}),
0); struct Cyc_Absyn_Exp* freeregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2332=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2332->f1=
Cyc_Toc_abs_ns; _temp2332->f2= Cyc_Toc__free_region_sp; _temp2332;}), 0); struct
Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0* _temp2331=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2331->f1= Cyc_Toc_abs_ns;
_temp2331->f2= Cyc_Toc__new_region_sp; _temp2331;}), 0); struct Cyc_Absyn_Exp*
addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2226);{ struct Cyc_Absyn_Stmt*
free_region_stmt= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( freeregion_exp,({
struct Cyc_List_List* _temp2330=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2330->hd=( void*) addr_rh_exp; _temp2330->tl= 0;
_temp2330;}), 0), 0); struct Cyc_Absyn_Stmt* tryandpop_stmt= Cyc_Absyn_seq_stmt(
_temp2226, Cyc_Absyn_seq_stmt( free_region_stmt, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0), 0); struct Cyc_Absyn_Stmt* rethrow_stmt= Cyc_Absyn_seq_stmt(
free_region_stmt, Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( e_exp), 0), 0);
struct Cyc_Absyn_Stmt* if_stmt= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, e_exp, 0), tryandpop_stmt, rethrow_stmt, 0); struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ), Cyc_Absyn_fncall_exp(
setjmp_exp,({ struct Cyc_List_List* _temp2329=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2329->hd=( void*) Cyc_Absyn_structmember_exp(
h_exp, Cyc_Toc_handler_sp, 0); _temp2329->tl= 0; _temp2329;}), 0), 0); struct
Cyc_Absyn_Stmt* push_stmt= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pushhandler_exp,({ struct Cyc_List_List* _temp2328=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2328->hd=( void*) Cyc_Absyn_address_exp(
h_exp, 0); _temp2328->tl= 0; _temp2328;}), 0), 0);( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
push_stmt, Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, if_stmt, 0), 0), 0), 0), 0))->r)); return;}} _LL2109:;}} struct
_tuple17{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* args= f->args; for( 0; args != 0;
args=({ struct Cyc_List_List* _temp2338= args; if( _temp2338 == 0){ _throw(
Null_Exception);} _temp2338->tl;})){(*(( struct _tuple17*)({ struct Cyc_List_List*
_temp2339= args; if( _temp2339 == 0){ _throw( Null_Exception);} _temp2339->hd;}))).f3=
Cyc_Toc_typ_to_c((*(( struct _tuple17*)({ struct Cyc_List_List* _temp2340= args;
if( _temp2340 == 0){ _throw( Null_Exception);} _temp2340->hd;}))).f3);{ struct
_tuple0* x=({ struct _tuple0* _temp2341=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2341->f1=( void*) Cyc_Absyn_Loc_n; _temp2341->f2=(*((
struct _tuple17*)({ struct Cyc_List_List* _temp2342= args; if( _temp2342 == 0){
_throw( Null_Exception);} _temp2342->hd;}))).f1; _temp2341;}); nv= Cyc_Toc_add_varmap(
nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2343= s;
_LL2345: if( _temp2343 ==( void*) Cyc_Absyn_Abstract){ goto _LL2346;} else{ goto
_LL2347;} _LL2347: if( _temp2343 ==( void*) Cyc_Absyn_ExternC){ goto _LL2348;}
else{ goto _LL2349;} _LL2349: goto _LL2350; _LL2346: return( void*) Cyc_Absyn_Public;
_LL2348: return( void*) Cyc_Absyn_Extern; _LL2350: return s; _LL2344:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* n=( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2363= s->name; if( _temp2363 == 0){ _throw( Null_Exception);}
_temp2363->v;}); if( Cyc_Toc_structs_so_far == 0){ Cyc_Toc_structs_so_far=({
struct Cyc_Core_Opt* _temp2351=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2351->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2351;});}{ int seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2362= Cyc_Toc_structs_so_far;
if( _temp2362 == 0){ _throw( Null_Exception);} _temp2362->v;}),(* n).f2); if(
dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2352= Cyc_Toc_structs_so_far; if( _temp2352 == 0){ _throw( Null_Exception);}
_temp2352->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2353= Cyc_Toc_structs_so_far;
if( _temp2353 == 0){ _throw( Null_Exception);} _temp2353->v;}),(* n).f2, s);});}
else{ if((( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Core_Opt* _temp2354= dopt;
if( _temp2354 == 0){ _throw( Null_Exception);} _temp2354->v;}))->fields == 0){(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2355= Cyc_Toc_structs_so_far;
if( _temp2355 == 0){ _throw( Null_Exception);} _temp2355->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2356= Cyc_Toc_structs_so_far; if( _temp2356 == 0){ _throw(
Null_Exception);} _temp2356->v;}),(* n).f2, s);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) s->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) s->sc))); s->tvs= 0; s->name=({
struct Cyc_Core_Opt* _temp2357=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2357->v=( void*) n; _temp2357;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2361= s->fields; if( _temp2361 == 0){
_throw( Null_Exception);} _temp2361->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2358= fields; if( _temp2358 == 0){ _throw( Null_Exception);}
_temp2358->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2359= fields; if( _temp2359 == 0){ _throw( Null_Exception);} _temp2359->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2360= fields; if( _temp2360 == 0){ _throw( Null_Exception);}
_temp2360->hd;}))->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* n=( struct _tuple0*)({ struct Cyc_Core_Opt* _temp2376= u->name; if(
_temp2376 == 0){ _throw( Null_Exception);} _temp2376->v;}); if( Cyc_Toc_unions_so_far
== 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2364=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2364->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2364;});}{ int seen_defn_before; struct Cyc_Core_Opt*
dopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2375= Cyc_Toc_unions_so_far; if( _temp2375 == 0){ _throw( Null_Exception);}
_temp2375->v;}),(* n).f2); if( dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2365= Cyc_Toc_unions_so_far; if( _temp2365 == 0){
_throw( Null_Exception);} _temp2365->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2366= Cyc_Toc_unions_so_far; if( _temp2366 == 0){ _throw( Null_Exception);}
_temp2366->v;}),(* n).f2, u);});} else{ if((( struct Cyc_Absyn_Uniondecl*)({
struct Cyc_Core_Opt* _temp2367= dopt; if( _temp2367 == 0){ _throw(
Null_Exception);} _temp2367->v;}))->fields == 0){( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2368= Cyc_Toc_unions_so_far; if( _temp2368 == 0){
_throw( Null_Exception);} _temp2368->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2369= Cyc_Toc_unions_so_far; if( _temp2369 == 0){ _throw( Null_Exception);}
_temp2369->v;}),(* n).f2, u);}); seen_defn_before= 0;} else{ seen_defn_before= 1;}}(
void*)( u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*)
u->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2370=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2370->v=( void*) n; _temp2370;}); if( u->fields != 0){ if(
seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2374= u->fields; if( _temp2374 == 0){
_throw( Null_Exception);} _temp2374->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2371= fields; if( _temp2371 == 0){ _throw( Null_Exception);}
_temp2371->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2372= fields; if( _temp2372 == 0){ _throw( Null_Exception);} _temp2372->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2373= fields; if( _temp2373 == 0){ _throw( Null_Exception);}
_temp2373->hd;}))->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2377=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2377->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2377;});}{ struct
_tuple0* n= tud->name; if( tud->fields == 0){ return;} if((( int(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2378= Cyc_Toc_tunions_so_far; if( _temp2378 == 0){ _throw( Null_Exception);}
_temp2378->v;}), n)){ return;}( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2379= Cyc_Toc_tunions_so_far; if( _temp2379 == 0){ _throw( Null_Exception);}
_temp2379->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct
_tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2380= Cyc_Toc_tunions_so_far; if( _temp2380 == 0){ _throw( Null_Exception);}
_temp2380->v;}), n);});{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2407= tud->fields;
if( _temp2407 == 0){ _throw( Null_Exception);} _temp2407->v;}); for( 0; fields
!= 0; fields=({ struct Cyc_List_List* _temp2381= fields; if( _temp2381 == 0){
_throw( Null_Exception);} _temp2381->tl;})){ struct Cyc_Absyn_Tunionfield* f=(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2406= fields; if(
_temp2406 == 0){ _throw( Null_Exception);} _temp2406->hd;}); if( f->typs == 0){
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
tag_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2382; _temp2382.q_const=
1; _temp2382.q_volatile= 0; _temp2382.q_restrict= 0; _temp2382;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2383=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2383->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2384=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2384[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2385; _temp2385.tag= Cyc_Absyn_Var_d; _temp2385.f1= vd; _temp2385;});
_temp2384;}), 0); _temp2383->tl= Cyc_Toc_result_decls; _temp2383;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
box_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2386; _temp2386.q_const=
1; _temp2386.q_volatile= 0; _temp2386.q_restrict= 0; _temp2386;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2387=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2387->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2388=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2388[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2389; _temp2389.tag= Cyc_Absyn_Var_d; _temp2389.f1= vd; _temp2389;});
_temp2388;}), 0); _temp2387->tl= Cyc_Toc_result_decls; _temp2387;});{ struct Cyc_List_List*
fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts=({
struct Cyc_List_List* _temp2390= ts; if( _temp2390 == 0){ _throw( Null_Exception);}
_temp2390->tl;}), i ++){ struct _tagged_string* fname=( struct _tagged_string*)({
struct _tagged_string* _temp2395=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp2395[ 0]= xprintf("f%d", i); _temp2395;}); struct
Cyc_Absyn_Structfield* f=({ struct Cyc_Absyn_Structfield* _temp2392=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2392->name= fname;
_temp2392->tq=(*(( struct _tuple3*)({ struct Cyc_List_List* _temp2394= ts; if(
_temp2394 == 0){ _throw( Null_Exception);} _temp2394->hd;}))).f1; _temp2392->type=(
void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp2393= ts; if( _temp2393 == 0){ _throw( Null_Exception);} _temp2393->hd;}))).f2);
_temp2392->width= 0; _temp2392->attributes= 0; _temp2392;}); fs=({ struct Cyc_List_List*
_temp2391=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2391->hd=( void*) f; _temp2391->tl= fs; _temp2391;});}} fs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs); fs=({ struct Cyc_List_List*
_temp2396=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2396->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2397=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2397->name= Cyc_Toc_tag_sp;
_temp2397->tq= Cyc_Toc_mt_tq; _temp2397->type=( void*) Cyc_Absyn_sint_t;
_temp2397->width= 0; _temp2397->attributes= 0; _temp2397;}); _temp2396->tl= fs;
_temp2396;});{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp2401=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2401->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2401->name=({ struct Cyc_Core_Opt*
_temp2403=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2403->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2404=( char*)"_struct"; struct _tagged_string _temp2405; _temp2405.curr=
_temp2404; _temp2405.base= _temp2404; _temp2405.last_plus_one= _temp2404 + 8;
_temp2405;})); _temp2403;}); _temp2401->tvs= 0; _temp2401->fields=({ struct Cyc_Core_Opt*
_temp2402=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2402->v=( void*) fs; _temp2402;}); _temp2401->attributes= 0; _temp2401;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2398=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2398->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2399=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2399[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2400; _temp2400.tag= Cyc_Absyn_Struct_d;
_temp2400.f1= sd; _temp2400;}); _temp2399;}), 0); _temp2398->tl= Cyc_Toc_result_decls;
_temp2398;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2408=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2408->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2408;});} if( xd->fields
== 0){ return;}{ struct _tuple0* n= xd->name; struct Cyc_List_List* fs=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2439= xd->fields; if( _temp2439 == 0){
_throw( Null_Exception);} _temp2439->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp2409= fs; if( _temp2409 == 0){ _throw( Null_Exception);} _temp2409->tl;})){
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
struct Cyc_Core_Opt* _temp2410= Cyc_Toc_xtunion_fields_so_far; if( _temp2410 ==
0){ _throw( Null_Exception);} _temp2410->v;}), f->name): 0){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp( Cyc_String_strconcat(( struct
_tagged_string)({ char* _temp2411=( char*)"\000\000\000\000"; struct
_tagged_string _temp2412; _temp2412.curr= _temp2411; _temp2412.base= _temp2411;
_temp2412.last_plus_one= _temp2411 + 5; _temp2412;}),* fn), 0);}{ struct Cyc_Absyn_Vardecl*
tag_decl= Cyc_Absyn_new_vardecl( f->name,( void*) tag_typ, initopt);( void*)(
tag_decl->sc=( void*)(( void*) xd->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2413=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2413->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2414=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2414[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2415; _temp2415.tag= Cyc_Absyn_Var_d;
_temp2415.f1= tag_decl; _temp2415;}); _temp2414;}), 0); _temp2413->tl= Cyc_Toc_result_decls;
_temp2413;}); if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp2416= Cyc_Toc_xtunion_fields_so_far;
if( _temp2416 == 0){ _throw( Null_Exception);} _temp2416->v;}), f->name)){
continue;}( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp2417= Cyc_Toc_xtunion_fields_so_far;
if( _temp2417 == 0){ _throw( Null_Exception);} _temp2417->v=( void*)(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_insert)((
struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp2418= Cyc_Toc_xtunion_fields_so_far;
if( _temp2418 == 0){ _throw( Null_Exception);} _temp2418->v;}), f->name);}); if(
f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
tqts= f->typs; for( 0; tqts != 0; tqts=({ struct Cyc_List_List* _temp2419= tqts;
if( _temp2419 == 0){ _throw( Null_Exception);} _temp2419->tl;}), i ++){ struct
_tagged_string* field_n=( struct _tagged_string*)({ struct _tagged_string*
_temp2424=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2424[ 0]= xprintf("f%d", i); _temp2424;}); struct Cyc_Absyn_Structfield*
newf=({ struct Cyc_Absyn_Structfield* _temp2421=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2421->name= field_n;
_temp2421->tq=(*(( struct _tuple3*)({ struct Cyc_List_List* _temp2423= tqts; if(
_temp2423 == 0){ _throw( Null_Exception);} _temp2423->hd;}))).f1; _temp2421->type=(
void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp2422= tqts; if( _temp2422 == 0){ _throw( Null_Exception);} _temp2422->hd;}))).f2);
_temp2421->width= 0; _temp2421->attributes= 0; _temp2421;}); fields=({ struct
Cyc_List_List* _temp2420=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2420->hd=( void*) newf; _temp2420->tl= fields; _temp2420;});}} fields=({
struct Cyc_List_List* _temp2425=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2425->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2426=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2426->name= Cyc_Toc_tag_sp; _temp2426->tq= Cyc_Toc_mt_tq; _temp2426->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2426->width=
0; _temp2426->attributes= 0; _temp2426;}); _temp2425->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2425;});{ struct Cyc_Absyn_Structdecl*
strct_decl=({ struct Cyc_Absyn_Structdecl* _temp2430=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2430->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2430->name=({ struct Cyc_Core_Opt* _temp2432=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2432->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({ char* _temp2433=(
char*)"_struct"; struct _tagged_string _temp2434; _temp2434.curr= _temp2433;
_temp2434.base= _temp2433; _temp2434.last_plus_one= _temp2433 + 8; _temp2434;}));
_temp2432;}); _temp2430->tvs= 0; _temp2430->fields=({ struct Cyc_Core_Opt*
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
if((( struct _tunion_struct*) _temp2465)->tag == Cyc_Absyn_Var_d){ _LL2490:
_temp2489=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2465)->f1; goto _LL2468;} else{ goto _LL2469;} _LL2469: if((( struct
_tunion_struct*) _temp2465)->tag == Cyc_Absyn_Fn_d){ _LL2492: _temp2491=( struct
Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp2465)->f1; goto _LL2470;}
else{ goto _LL2471;} _LL2471: if((( struct _tunion_struct*) _temp2465)->tag ==
Cyc_Absyn_Let_d){ _LL2502: _temp2501=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2465)->f1; goto _LL2500; _LL2500: _temp2499=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2465)->f2; goto _LL2498; _LL2498: _temp2497=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2465)->f3; goto
_LL2496; _LL2496: _temp2495=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2465)->f4; goto _LL2494; _LL2494: _temp2493=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp2465)->f5; goto _LL2472;} else{ goto _LL2473;} _LL2473: if((( struct
_tunion_struct*) _temp2465)->tag == Cyc_Absyn_Struct_d){ _LL2504: _temp2503=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp2465)->f1;
goto _LL2474;} else{ goto _LL2475;} _LL2475: if((( struct _tunion_struct*)
_temp2465)->tag == Cyc_Absyn_Union_d){ _LL2506: _temp2505=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp2465)->f1; goto _LL2476;} else{ goto
_LL2477;} _LL2477: if((( struct _tunion_struct*) _temp2465)->tag == Cyc_Absyn_Tunion_d){
_LL2508: _temp2507=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2465)->f1; goto _LL2478;} else{ goto _LL2479;} _LL2479: if((( struct
_tunion_struct*) _temp2465)->tag == Cyc_Absyn_Enum_d){ _LL2510: _temp2509=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp2465)->f1;
goto _LL2480;} else{ goto _LL2481;} _LL2481: if((( struct _tunion_struct*)
_temp2465)->tag == Cyc_Absyn_Typedef_d){ _LL2512: _temp2511=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp2465)->f1; goto _LL2482;} else{ goto
_LL2483;} _LL2483: if((( struct _tunion_struct*) _temp2465)->tag == Cyc_Absyn_Namespace_d){
_LL2516: _temp2515=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2465)->f1; goto _LL2514; _LL2514: _temp2513=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2465)->f2; goto _LL2484;} else{ goto
_LL2485;} _LL2485: if((( struct _tunion_struct*) _temp2465)->tag == Cyc_Absyn_Using_d){
_LL2520: _temp2519=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2465)->f1; goto _LL2518; _LL2518: _temp2517=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp2465)->f2; goto _LL2486;} else{ goto
_LL2487;} _LL2487: if((( struct _tunion_struct*) _temp2465)->tag == Cyc_Absyn_ExternC_d){
_LL2522: _temp2521=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2465)->f1; goto _LL2488;} else{ goto _LL2466;} _LL2468: { struct _tuple0*
c_name= _temp2489->name; if(( void*) _temp2489->sc ==( void*) Cyc_Absyn_ExternC){
c_name=({ struct _tuple0* _temp2523=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp2523->f1= Cyc_Toc_abs_ns; _temp2523->f2=(* c_name).f2; _temp2523;});}
if( _temp2489->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2524= _temp2489->initializer; if( _temp2524 == 0){
_throw( Null_Exception);} _temp2524;}));} if( ! nv->toplevel){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2525=(
char*)"decls_to_c: not at toplevel!"; struct _tagged_string _temp2526; _temp2526.curr=
_temp2525; _temp2526.base= _temp2525; _temp2526.last_plus_one= _temp2525 + 29;
_temp2526;}));} nv= Cyc_Toc_add_varmap( nv, _temp2489->name, Cyc_Absyn_varb_exp(
c_name,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp2527=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp2527[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp2528; _temp2528.tag= Cyc_Absyn_Global_b;
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