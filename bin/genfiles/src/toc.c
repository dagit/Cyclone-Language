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
struct _tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List*
_temp174= 0; goto _LL175; _LL175:{ int i= 1; for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp176= ts; if( _temp176 == 0){ _throw( Null_Exception);} _temp176->tl;}), i
++){ _temp174=({ struct Cyc_List_List* _temp177=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp177->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp178=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp178->name=({ struct _tagged_string* _temp180=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp180[ 0]= xprintf("f%d", i);
_temp180;}); _temp178->tq= Cyc_Toc_mt_tq; _temp178->type=( void*)(( void*)({
struct Cyc_List_List* _temp179= ts; if( _temp179 == 0){ _throw( Null_Exception);}
_temp179->hd;})); _temp178->width= 0; _temp178->attributes= 0; _temp178;});
_temp177->tl= _temp174; _temp177;});}} _temp174=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp174);{ struct Cyc_Absyn_Structdecl*
_temp185=({ struct Cyc_Absyn_Structdecl* _temp181=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp181->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp181->name=({ struct Cyc_Core_Opt* _temp183=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp183->v=( void*)({ struct _tuple0*
_temp184=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp184->f1=
Cyc_Toc_abs_ns; _temp184->f2= x; _temp184;}); _temp183;}); _temp181->tvs= 0;
_temp181->fields=({ struct Cyc_Core_Opt* _temp182=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp182->v=( void*) _temp174;
_temp182;}); _temp181->attributes= 0; _temp181;}); goto _LL186; _LL186: Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp187=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp187->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp188=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp188[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp189; _temp189.tag= Cyc_Absyn_Struct_d; _temp189.f1=
_temp185; _temp189;}); _temp188;}), 0); _temp187->tl= Cyc_Toc_result_decls;
_temp187;});}} static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x,
void* t){ void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Structfield* _temp191=({ struct Cyc_Absyn_Structfield* _temp190=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp190->name= Cyc_Toc_curr_sp; _temp190->tq= Cyc_Toc_mt_tq; _temp190->type=(
void*) tptr; _temp190->width= 0; _temp190->attributes= 0; _temp190;}); goto
_LL192; _LL192: { struct Cyc_Absyn_Structfield* _temp194=({ struct Cyc_Absyn_Structfield*
_temp193=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp193->name= Cyc_Toc_base_sp; _temp193->tq= Cyc_Toc_mt_tq; _temp193->type=(
void*) tptr; _temp193->width= 0; _temp193->attributes= 0; _temp193;}); goto
_LL195; _LL195: { struct Cyc_Absyn_Structfield* _temp197=({ struct Cyc_Absyn_Structfield*
_temp196=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp196->name= Cyc_Toc_last_plus_one_sp; _temp196->tq= Cyc_Toc_mt_tq; _temp196->type=(
void*) tptr; _temp196->width= 0; _temp196->attributes= 0; _temp196;}); goto
_LL198; _LL198: { struct Cyc_List_List* _temp202=({ struct Cyc_List_List*
_temp199=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp199->hd=( void*) _temp191; _temp199->tl=({ struct Cyc_List_List* _temp200=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp200->hd=(
void*) _temp194; _temp200->tl=({ struct Cyc_List_List* _temp201=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp201->hd=( void*) _temp197;
_temp201->tl= 0; _temp201;}); _temp200;}); _temp199;}); goto _LL203; _LL203: {
struct Cyc_Absyn_Structdecl* _temp208=({ struct Cyc_Absyn_Structdecl* _temp204=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp204->sc=( void*)(( void*) Cyc_Absyn_Public); _temp204->name=({ struct Cyc_Core_Opt*
_temp206=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp206->v=( void*)({ struct _tuple0* _temp207=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp207->f1= Cyc_Toc_abs_ns; _temp207->f2= x;
_temp207;}); _temp206;}); _temp204->tvs= 0; _temp204->fields=({ struct Cyc_Core_Opt*
_temp205=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp205->v=( void*)(( struct Cyc_List_List*) _temp202); _temp205;}); _temp204->attributes=
0; _temp204;}); goto _LL209; _LL209: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp210=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp210->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp211=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp211[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp212; _temp212.tag= Cyc_Absyn_Struct_d;
_temp212.f1= _temp208; _temp212;}); _temp211;}), 0); _temp210->tl= Cyc_Toc_result_decls;
_temp210;});}}}}} static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static
int Cyc_Toc_tuple_type_counter= 0; struct _tuple5{ struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static struct _tagged_string* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp213= Cyc_Toc_tuple_types;
goto _LL214; _LL214: for( 0; _temp213 != 0; _temp213=({ struct Cyc_List_List*
_temp215= _temp213; if( _temp215 == 0){ _throw( Null_Exception);} _temp215->tl;})){
struct _tuple5 _temp219; struct Cyc_List_List* _temp220; struct _tagged_string*
_temp222; struct _tuple5* _temp217=( struct _tuple5*)({ struct Cyc_List_List*
_temp216= _temp213; if( _temp216 == 0){ _throw( Null_Exception);} _temp216->hd;});
_temp219=* _temp217; _LL223: _temp222= _temp219.f1; goto _LL221; _LL221:
_temp220= _temp219.f2; goto _LL218; _LL218: { int okay= 1; struct Cyc_List_List*
_temp224= tqs0; goto _LL225; _LL225: for( 0; _temp224 != 0? _temp220 != 0: 0;
_temp224=({ struct Cyc_List_List* _temp226= _temp224; if( _temp226 == 0){ _throw(
Null_Exception);} _temp226->tl;}), _temp220=({ struct Cyc_List_List* _temp227=
_temp220; if( _temp227 == 0){ _throw( Null_Exception);} _temp227->tl;})){ if( !
Cyc_Tcutil_unify((*(( struct _tuple3*)({ struct Cyc_List_List* _temp228=
_temp224; if( _temp228 == 0){ _throw( Null_Exception);} _temp228->hd;}))).f2,(
void*)({ struct Cyc_List_List* _temp229= _temp220; if( _temp229 == 0){ _throw(
Null_Exception);} _temp229->hd;}))){ okay= 0; break;}} if(( ! okay? 1: _temp224
!= 0)? 1: _temp220 != 0){ continue;} return _temp222;}}}{ struct _tagged_string*
x=({ struct _tagged_string* _temp232=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp232[ 0]= xprintf("_tuple%d",( Cyc_Toc_tuple_type_counter
++)); _temp232;}); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt,
tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({ struct Cyc_List_List*
_temp230=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp230->hd=( void*)({ struct _tuple5* _temp231=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp231->f1= x; _temp231->f2= ts; _temp231;});
_temp230->tl= Cyc_Toc_tuple_types; _temp230;}); return x;}} static struct Cyc_List_List*
Cyc_Toc_tagged_ptr_types= 0; static int Cyc_Toc_tagged_ptr_counter= 0; struct
_tuple6{ struct _tagged_string* f1; void* f2; } ; static struct _tagged_string*
Cyc_Toc_add_tagged_ptr_type( void* t, struct Cyc_Absyn_Tqual tq){{ void*
_temp233= t; void* _temp239; void* _temp241; _LL235: if(( unsigned int) _temp233
> 4u?*(( int*) _temp233) == Cyc_Absyn_IntType: 0){ _LL242: _temp241=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp233)->f1; if( _temp241 ==( void*) Cyc_Absyn_Unsigned){
goto _LL240;} else{ goto _LL237;} _LL240: _temp239=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp233)->f2; if( _temp239 ==( void*) Cyc_Absyn_B1){ goto _LL236;} else{ goto
_LL237;}} else{ goto _LL237;} _LL237: goto _LL238; _LL236: return Cyc_Toc__tagged_string_sp;
_LL238: goto _LL234; _LL234:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts=({ struct Cyc_List_List* _temp243= tts; if( _temp243 == 0){
_throw( Null_Exception);} _temp243->tl;})){ struct _tuple6 _temp247; void*
_temp248; struct _tagged_string* _temp250; struct _tuple6* _temp245=( struct
_tuple6*)({ struct Cyc_List_List* _temp244= tts; if( _temp244 == 0){ _throw(
Null_Exception);} _temp244->hd;}); _temp247=* _temp245; _LL251: _temp250=
_temp247.f1; goto _LL249; _LL249: _temp248= _temp247.f2; goto _LL246; _LL246:
if( ! Cyc_Tcutil_unify( t, _temp248)){ continue;} return _temp250;}}{ struct
_tagged_string* x=({ struct _tagged_string* _temp254=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp254[ 0]= xprintf("_tagged_ptr%d",(
Cyc_Toc_tagged_ptr_counter ++)); _temp254;}); Cyc_Toc_add_tagged_ptr_decl( x, t);
Cyc_Toc_tagged_ptr_types=({ struct Cyc_List_List* _temp252=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp252->hd=( void*)({ struct
_tuple6* _temp253=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp253->f1= x; _temp253->f2= t; _temp253;}); _temp252->tl= Cyc_Toc_tagged_ptr_types;
_temp252;}); return x;}} static int Cyc_Toc_temp_var_counter= 0; static struct
_tuple0* Cyc_Toc_temp_var(){ return({ struct _tuple0* _temp255=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp255->f1= Cyc_Toc_abs_ns; _temp255->f2=({
struct _tagged_string* _temp256=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp256[ 0]= xprintf("_temp%d",( Cyc_Toc_temp_var_counter
++)); _temp256;}); _temp255;});} static int Cyc_Toc_fresh_label_counter= 0;
static struct _tagged_string* Cyc_Toc_fresh_label(){ return({ struct
_tagged_string* _temp257=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp257[ 0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter
++)); _temp257;});} static void* Cyc_Toc_typ_to_c( void* t); static struct
_tuple1* Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp260; struct Cyc_Absyn_Tqual
_temp262; struct Cyc_Core_Opt* _temp264; struct _tuple1 _temp258=* a; _LL265:
_temp264= _temp258.f1; goto _LL263; _LL263: _temp262= _temp258.f2; goto _LL261;
_LL261: _temp260= _temp258.f3; goto _LL259; _LL259: return({ struct _tuple1*
_temp266=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp266->f1=
_temp264; _temp266->f2= _temp262; _temp266->f3= Cyc_Toc_typ_to_c( _temp260);
_temp266;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp269; struct Cyc_Absyn_Tqual _temp271; struct _tuple3 _temp267=* x;
_LL272: _temp271= _temp267.f1; goto _LL270; _LL270: _temp269= _temp267.f2; goto
_LL268; _LL268: return({ struct _tuple3* _temp273=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp273->f1= _temp271; _temp273->f2= Cyc_Toc_typ_to_c(
_temp269); _temp273;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp274= t; struct Cyc_Absyn_Exp* _temp282; struct Cyc_Absyn_Tqual _temp284;
void* _temp286; int _temp288; struct Cyc_Core_Opt* _temp290; struct Cyc_Core_Opt
_temp292; void* _temp293; void* _temp295; _LL276: if(( unsigned int) _temp274 >
4u?*(( int*) _temp274) == Cyc_Absyn_ArrayType: 0){ _LL287: _temp286=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp274)->f1; goto _LL285; _LL285: _temp284=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp274)->f2;
goto _LL283; _LL283: _temp282=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp274)->f3; goto _LL277;} else{ goto _LL278;} _LL278: if(( unsigned int)
_temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_Evar: 0){ _LL296: _temp295=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp274)->f1; goto _LL291; _LL291: _temp290=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp274)->f2; if(
_temp290 == 0){ goto _LL280;} else{ _temp292=* _temp290; _LL294: _temp293=( void*)
_temp292.v; goto _LL289;} _LL289: _temp288=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp274)->f3; goto _LL279;} else{ goto _LL280;} _LL280: goto _LL281; _LL277:
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp297=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp297[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp298; _temp298.tag= Cyc_Absyn_ArrayType; _temp298.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp286); _temp298.f2= _temp284; _temp298.f3=
_temp282; _temp298;}); _temp297;}); _LL279: return Cyc_Toc_typ_to_c_array(
_temp293); _LL281: return Cyc_Toc_typ_to_c( t); _LL275:;} static void* Cyc_Toc_typ_to_c(
void* t); static struct Cyc_Absyn_Structfield* Cyc_Toc_structfield_to_c( struct
Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield* _temp299=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp299->name= f->name; _temp299->tq= f->tq; _temp299->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp299->width= f->width; _temp299->attributes= f->attributes;
_temp299;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp300= t; int
_temp352; struct Cyc_Core_Opt* _temp354; void* _temp356; int _temp358; struct
Cyc_Core_Opt* _temp360; struct Cyc_Core_Opt _temp362; void* _temp363; void*
_temp365; struct Cyc_Absyn_Tvar* _temp367; struct Cyc_Absyn_TunionInfo _temp369;
void* _temp371; struct Cyc_List_List* _temp373; void* _temp375; struct Cyc_Absyn_Tuniondecl*
_temp377; struct Cyc_Absyn_TunionInfo _temp379; struct Cyc_Absyn_TunionFieldInfo
_temp381; struct Cyc_List_List* _temp383; void* _temp385; struct Cyc_Absyn_Tunionfield*
_temp387; struct Cyc_Absyn_Tuniondecl* _temp389; struct Cyc_Absyn_TunionFieldInfo
_temp391; struct Cyc_Absyn_PtrInfo _temp393; struct Cyc_Absyn_Conref* _temp395;
struct Cyc_Absyn_Tqual _temp397; struct Cyc_Absyn_Conref* _temp399; void*
_temp401; void* _temp403; void* _temp405; void* _temp407; struct Cyc_Absyn_Exp*
_temp409; struct Cyc_Absyn_Tqual _temp411; void* _temp413; struct Cyc_Absyn_FnInfo
_temp415; struct Cyc_List_List* _temp417; int _temp419; struct Cyc_List_List*
_temp421; void* _temp423; struct Cyc_Core_Opt* _temp425; struct Cyc_List_List*
_temp427; struct Cyc_List_List* _temp429; struct Cyc_List_List* _temp431; struct
Cyc_List_List* _temp433; struct Cyc_Absyn_Uniondecl** _temp435; struct Cyc_List_List*
_temp437; struct _tuple0* _temp439; struct Cyc_Absyn_Structdecl** _temp441;
struct Cyc_List_List* _temp443; struct _tuple0* _temp445; struct Cyc_Absyn_Enumdecl*
_temp447; struct _tuple0* _temp449; struct Cyc_Core_Opt* _temp451; struct Cyc_List_List*
_temp453; struct _tuple0* _temp455; void* _temp457; void* _temp459; struct Cyc_List_List*
_temp461; _LL302: if( _temp300 ==( void*) Cyc_Absyn_VoidType){ goto _LL303;}
else{ goto _LL304;} _LL304: if(( unsigned int) _temp300 > 4u?*(( int*) _temp300)
== Cyc_Absyn_Evar: 0){ _LL357: _temp356=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp300)->f1; goto _LL355; _LL355: _temp354=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp300)->f2; if( _temp354 == 0){ goto _LL353;} else{
goto _LL306;} _LL353: _temp352=( int)(( struct Cyc_Absyn_Evar_struct*) _temp300)->f3;
goto _LL305;} else{ goto _LL306;} _LL306: if(( unsigned int) _temp300 > 4u?*((
int*) _temp300) == Cyc_Absyn_Evar: 0){ _LL366: _temp365=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp300)->f1; goto _LL361; _LL361: _temp360=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp300)->f2; if( _temp360 == 0){ goto _LL308;} else{
_temp362=* _temp360; _LL364: _temp363=( void*) _temp362.v; goto _LL359;} _LL359:
_temp358=( int)(( struct Cyc_Absyn_Evar_struct*) _temp300)->f3; goto _LL307;}
else{ goto _LL308;} _LL308: if(( unsigned int) _temp300 > 4u?*(( int*) _temp300)
== Cyc_Absyn_VarType: 0){ _LL368: _temp367=( struct Cyc_Absyn_Tvar*)(( struct
Cyc_Absyn_VarType_struct*) _temp300)->f1; goto _LL309;} else{ goto _LL310;}
_LL310: if(( unsigned int) _temp300 > 4u?*(( int*) _temp300) == Cyc_Absyn_TunionType:
0){ _LL370: _temp369=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp300)->f1; _LL376: _temp375=( void*) _temp369.tunion_info; if(*(( int*)
_temp375) == Cyc_Absyn_KnownTunion){ _LL378: _temp377=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp375)->f1; goto _LL374;} else{ goto
_LL312;} _LL374: _temp373=( struct Cyc_List_List*) _temp369.targs; goto _LL372;
_LL372: _temp371=( void*) _temp369.rgn; goto _LL311;} else{ goto _LL312;} _LL312:
if(( unsigned int) _temp300 > 4u?*(( int*) _temp300) == Cyc_Absyn_TunionType: 0){
_LL380: _temp379=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp300)->f1; goto _LL313;} else{ goto _LL314;} _LL314: if(( unsigned int)
_temp300 > 4u?*(( int*) _temp300) == Cyc_Absyn_TunionFieldType: 0){ _LL382:
_temp381=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp300)->f1; _LL386: _temp385=( void*) _temp381.field_info; if(*(( int*)
_temp385) == Cyc_Absyn_KnownTunionfield){ _LL390: _temp389=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp385)->f1; goto _LL388; _LL388:
_temp387=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp385)->f2; goto _LL384;} else{ goto _LL316;} _LL384: _temp383=( struct Cyc_List_List*)
_temp381.targs; goto _LL315;} else{ goto _LL316;} _LL316: if(( unsigned int)
_temp300 > 4u?*(( int*) _temp300) == Cyc_Absyn_TunionFieldType: 0){ _LL392:
_temp391=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp300)->f1; goto _LL317;} else{ goto _LL318;} _LL318: if(( unsigned int)
_temp300 > 4u?*(( int*) _temp300) == Cyc_Absyn_PointerType: 0){ _LL394: _temp393=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp300)->f1;
_LL404: _temp403=( void*) _temp393.elt_typ; goto _LL402; _LL402: _temp401=( void*)
_temp393.rgn_typ; goto _LL400; _LL400: _temp399=( struct Cyc_Absyn_Conref*)
_temp393.nullable; goto _LL398; _LL398: _temp397=( struct Cyc_Absyn_Tqual)
_temp393.tq; goto _LL396; _LL396: _temp395=( struct Cyc_Absyn_Conref*) _temp393.bounds;
goto _LL319;} else{ goto _LL320;} _LL320: if(( unsigned int) _temp300 > 4u?*((
int*) _temp300) == Cyc_Absyn_IntType: 0){ _LL408: _temp407=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp300)->f1; goto _LL406; _LL406: _temp405=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp300)->f2; goto _LL321;} else{ goto _LL322;} _LL322: if( _temp300 ==( void*)
Cyc_Absyn_FloatType){ goto _LL323;} else{ goto _LL324;} _LL324: if( _temp300 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL325;} else{ goto _LL326;} _LL326: if((
unsigned int) _temp300 > 4u?*(( int*) _temp300) == Cyc_Absyn_ArrayType: 0){
_LL414: _temp413=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp300)->f1;
goto _LL412; _LL412: _temp411=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp300)->f2; goto _LL410; _LL410: _temp409=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp300)->f3; goto _LL327;} else{ goto _LL328;}
_LL328: if(( unsigned int) _temp300 > 4u?*(( int*) _temp300) == Cyc_Absyn_FnType:
0){ _LL416: _temp415=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp300)->f1; _LL428: _temp427=( struct Cyc_List_List*) _temp415.tvars; goto
_LL426; _LL426: _temp425=( struct Cyc_Core_Opt*) _temp415.effect; goto _LL424;
_LL424: _temp423=( void*) _temp415.ret_typ; goto _LL422; _LL422: _temp421=(
struct Cyc_List_List*) _temp415.args; goto _LL420; _LL420: _temp419=( int)
_temp415.varargs; goto _LL418; _LL418: _temp417=( struct Cyc_List_List*)
_temp415.attributes; goto _LL329;} else{ goto _LL330;} _LL330: if(( unsigned int)
_temp300 > 4u?*(( int*) _temp300) == Cyc_Absyn_TupleType: 0){ _LL430: _temp429=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp300)->f1; goto
_LL331;} else{ goto _LL332;} _LL332: if(( unsigned int) _temp300 > 4u?*(( int*)
_temp300) == Cyc_Absyn_AnonStructType: 0){ _LL432: _temp431=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp300)->f1; goto _LL333;} else{ goto
_LL334;} _LL334: if(( unsigned int) _temp300 > 4u?*(( int*) _temp300) == Cyc_Absyn_AnonUnionType:
0){ _LL434: _temp433=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp300)->f1; goto _LL335;} else{ goto _LL336;} _LL336: if(( unsigned int)
_temp300 > 4u?*(( int*) _temp300) == Cyc_Absyn_UnionType: 0){ _LL440: _temp439=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp300)->f1; goto
_LL438; _LL438: _temp437=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp300)->f2; goto _LL436; _LL436: _temp435=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp300)->f3; goto _LL337;} else{ goto
_LL338;} _LL338: if(( unsigned int) _temp300 > 4u?*(( int*) _temp300) == Cyc_Absyn_StructType:
0){ _LL446: _temp445=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp300)->f1; goto _LL444; _LL444: _temp443=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp300)->f2; goto _LL442; _LL442: _temp441=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp300)->f3;
goto _LL339;} else{ goto _LL340;} _LL340: if(( unsigned int) _temp300 > 4u?*((
int*) _temp300) == Cyc_Absyn_EnumType: 0){ _LL450: _temp449=( struct _tuple0*)((
struct Cyc_Absyn_EnumType_struct*) _temp300)->f1; goto _LL448; _LL448: _temp447=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp300)->f2;
goto _LL341;} else{ goto _LL342;} _LL342: if(( unsigned int) _temp300 > 4u?*((
int*) _temp300) == Cyc_Absyn_TypedefType: 0){ _LL456: _temp455=( struct _tuple0*)((
struct Cyc_Absyn_TypedefType_struct*) _temp300)->f1; goto _LL454; _LL454:
_temp453=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp300)->f2; goto _LL452; _LL452: _temp451=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp300)->f3; goto _LL343;} else{ goto _LL344;}
_LL344: if(( unsigned int) _temp300 > 4u?*(( int*) _temp300) == Cyc_Absyn_RgnHandleType:
0){ _LL458: _temp457=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp300)->f1;
goto _LL345;} else{ goto _LL346;} _LL346: if( _temp300 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL347;} else{ goto _LL348;} _LL348: if(( unsigned int) _temp300 > 4u?*((
int*) _temp300) == Cyc_Absyn_AccessEff: 0){ _LL460: _temp459=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp300)->f1; goto _LL349;} else{ goto _LL350;}
_LL350: if(( unsigned int) _temp300 > 4u?*(( int*) _temp300) == Cyc_Absyn_JoinEff:
0){ _LL462: _temp461=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp300)->f1; goto _LL351;} else{ goto _LL301;} _LL303: return t; _LL305:
return( void*) Cyc_Absyn_VoidType; _LL307: return Cyc_Toc_typ_to_c( _temp363);
_LL309: return Cyc_Absyn_void_star_typ(); _LL311: return Cyc_Absyn_void_star_typ();
_LL313:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp463=( char*)"unresolved TunionType"; struct
_tagged_string _temp464; _temp464.curr= _temp463; _temp464.base= _temp463;
_temp464.last_plus_one= _temp463 + 22; _temp464;}))); _LL315: if( _temp387->typs
== 0){ if( _temp389->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp387->name,(
struct _tagged_string)({ char* _temp465=( char*)"_struct"; struct _tagged_string
_temp466; _temp466.curr= _temp465; _temp466.base= _temp465; _temp466.last_plus_one=
_temp465 + 8; _temp466;})));} _LL317:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp467=(
char*)"unresolved TunionFieldType"; struct _tagged_string _temp468; _temp468.curr=
_temp467; _temp468.base= _temp467; _temp468.last_plus_one= _temp467 + 27;
_temp468;}))); _LL319: _temp403= Cyc_Toc_typ_to_c_array( _temp403);{ void*
_temp469=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp395))->v; void* _temp475; _LL471: if((
unsigned int) _temp469 > 1u?*(( int*) _temp469) == Cyc_Absyn_Eq_constr: 0){
_LL476: _temp475=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp469)->f1;
if( _temp475 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL472;} else{ goto _LL473;}}
else{ goto _LL473;} _LL473: goto _LL474; _LL472: { struct _tagged_string*
_temp477= Cyc_Toc_add_tagged_ptr_type( _temp403, _temp397); goto _LL478; _LL478:
return Cyc_Absyn_strct( _temp477);} _LL474: return Cyc_Absyn_star_typ( _temp403,(
void*) Cyc_Absyn_HeapRgn, _temp397); _LL470:;} _LL321: return t; _LL323: return
t; _LL325: return t; _LL327: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp479=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp479[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp480; _temp480.tag= Cyc_Absyn_ArrayType;
_temp480.f1=( void*) Cyc_Toc_typ_to_c_array( _temp413); _temp480.f2= _temp411;
_temp480.f3= _temp409; _temp480;}); _temp479;}); _LL329: { struct Cyc_List_List*
_temp481= 0; goto _LL482; _LL482: for( 0; _temp417 != 0; _temp417=({ struct Cyc_List_List*
_temp483= _temp417; if( _temp483 == 0){ _throw( Null_Exception);} _temp483->tl;})){
void* _temp485=( void*)({ struct Cyc_List_List* _temp484= _temp417; if( _temp484
== 0){ _throw( Null_Exception);} _temp484->hd;}); _LL487: if( _temp485 ==( void*)
Cyc_Absyn_Cdecl_att){ goto _LL488;} else{ goto _LL489;} _LL489: if( _temp485 ==(
void*) Cyc_Absyn_Noreturn_att){ goto _LL490;} else{ goto _LL491;} _LL491: if(
_temp485 ==( void*) Cyc_Absyn_Const_att){ goto _LL492;} else{ goto _LL493;}
_LL493: goto _LL494; _LL488: goto _LL490; _LL490: goto _LL492; _LL492: continue;
_LL494: _temp481=({ struct Cyc_List_List* _temp495=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp495->hd=( void*)(( void*)({
struct Cyc_List_List* _temp496= _temp417; if( _temp496 == 0){ _throw(
Null_Exception);} _temp496->hd;})); _temp495->tl= _temp481; _temp495;}); goto
_LL486; _LL486:;} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp497=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp497[ 0]=({ struct Cyc_Absyn_FnType_struct _temp498; _temp498.tag= Cyc_Absyn_FnType;
_temp498.f1=({ struct Cyc_Absyn_FnInfo _temp499; _temp499.tvars= 0; _temp499.effect=
0; _temp499.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp423); _temp499.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp421); _temp499.varargs= _temp419;
_temp499.attributes= _temp481; _temp499;}); _temp498;}); _temp497;});} _LL331:
_temp429=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp429);{ struct
_tagged_string* _temp500= Cyc_Toc_add_tuple_type( _temp429); goto _LL501; _LL501:
return Cyc_Absyn_strct( _temp500);} _LL333: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp502=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp502[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp503; _temp503.tag= Cyc_Absyn_AnonStructType; _temp503.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp431); _temp503;}); _temp502;});
_LL335: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp504=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp504[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp505; _temp505.tag=
Cyc_Absyn_AnonUnionType; _temp505.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp433); _temp505;}); _temp504;}); _LL337: if( _temp439 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp506=( char*)"anonymous union"; struct _tagged_string _temp507; _temp507.curr=
_temp506; _temp507.base= _temp506; _temp507.last_plus_one= _temp506 + 16;
_temp507;}));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)({ struct _tuple0*
_temp508= _temp439; if( _temp508 == 0){ _throw( Null_Exception);} _temp508;}));
_LL339: if( _temp445 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp509=( char*)"anonymous struct"; struct
_tagged_string _temp510; _temp510.curr= _temp509; _temp510.base= _temp509;
_temp510.last_plus_one= _temp509 + 17; _temp510;}));} return Cyc_Absyn_strctq((
struct _tuple0*)({ struct _tuple0* _temp511= _temp445; if( _temp511 == 0){
_throw( Null_Exception);} _temp511;})); _LL341: return t; _LL343: if( _temp451
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp512=( char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp513; _temp513.curr= _temp512; _temp513.base= _temp512;
_temp513.last_plus_one= _temp512 + 38; _temp513;}));} return( void*)({ struct
Cyc_Absyn_TypedefType_struct* _temp514=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp514[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp515; _temp515.tag= Cyc_Absyn_TypedefType;
_temp515.f1= _temp455; _temp515.f2= 0; _temp515.f3=({ struct Cyc_Core_Opt*
_temp516=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp516->v=( void*) Cyc_Toc_typ_to_c_array(( void*)({ struct Cyc_Core_Opt*
_temp517= _temp451; if( _temp517 == 0){ _throw( Null_Exception);} _temp517->v;}));
_temp516;}); _temp515;}); _temp514;}); _LL345: return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__region_handle_sp), Cyc_Toc_mt_tq); _LL347: return(( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp518=(
char*)"Toc::typ_to_c: type translation passed the heap region"; struct
_tagged_string _temp519; _temp519.curr= _temp518; _temp519.base= _temp518;
_temp519.last_plus_one= _temp518 + 55; _temp519;})); _LL349: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp520=( char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp521; _temp521.curr= _temp520; _temp521.base= _temp520;
_temp521.last_plus_one= _temp520 + 54; _temp521;})); _LL351: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp522=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp523; _temp523.curr= _temp522; _temp523.base= _temp522;
_temp523.last_plus_one= _temp522 + 52; _temp523;})); _LL301:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp524= t; struct Cyc_Absyn_Exp* _temp530; struct Cyc_Absyn_Tqual
_temp532; void* _temp534; _LL526: if(( unsigned int) _temp524 > 4u?*(( int*)
_temp524) == Cyc_Absyn_ArrayType: 0){ _LL535: _temp534=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp524)->f1; goto _LL533; _LL533: _temp532=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp524)->f2; goto _LL531; _LL531: _temp530=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp524)->f3; goto
_LL527;} else{ goto _LL528;} _LL528: goto _LL529; _LL527: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp534,( void*) Cyc_Absyn_HeapRgn, _temp532), e, l);
_LL529: return e; _LL525:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp536= t; struct Cyc_Absyn_Exp* _temp542; struct Cyc_Absyn_Tqual _temp544;
void* _temp546; _LL538: if(( unsigned int) _temp536 > 4u?*(( int*) _temp536) ==
Cyc_Absyn_ArrayType: 0){ _LL547: _temp546=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp536)->f1; goto _LL545; _LL545: _temp544=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp536)->f2; goto _LL543; _LL543: _temp542=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp536)->f3; goto
_LL539;} else{ goto _LL540;} _LL540: goto _LL541; _LL539: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp546,( void*) Cyc_Absyn_HeapRgn, _temp544), e, l);
_LL541: return Cyc_Absyn_cast_exp( t, e, l); _LL537:;} static int Cyc_Toc_atomic_typ(
void* t){ void* _temp548= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp584; void* _temp586; void* _temp588; struct Cyc_Absyn_Exp* _temp590; struct
Cyc_Absyn_Tqual _temp592; void* _temp594; struct Cyc_Absyn_FnInfo _temp596;
struct Cyc_List_List* _temp598; struct Cyc_Absyn_Structdecl** _temp600; struct
Cyc_List_List* _temp602; struct _tuple0* _temp604; struct Cyc_Absyn_TunionInfo
_temp606; struct Cyc_List_List* _temp608; struct Cyc_List_List* _temp610; struct
Cyc_Absyn_TunionFieldInfo _temp612; struct Cyc_List_List* _temp614; void*
_temp616; struct Cyc_Absyn_Tunionfield* _temp618; struct Cyc_Absyn_Tuniondecl*
_temp620; struct Cyc_Absyn_Uniondecl** _temp622; struct Cyc_List_List* _temp624;
struct _tuple0* _temp626; struct Cyc_Absyn_PtrInfo _temp628; void* _temp630;
_LL550: if( _temp548 ==( void*) Cyc_Absyn_VoidType){ goto _LL551;} else{ goto
_LL552;} _LL552: if(( unsigned int) _temp548 > 4u?*(( int*) _temp548) == Cyc_Absyn_VarType:
0){ _LL585: _temp584=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp548)->f1; goto _LL553;} else{ goto _LL554;} _LL554: if(( unsigned int)
_temp548 > 4u?*(( int*) _temp548) == Cyc_Absyn_IntType: 0){ _LL589: _temp588=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp548)->f1; goto _LL587; _LL587:
_temp586=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp548)->f2; goto _LL555;}
else{ goto _LL556;} _LL556: if( _temp548 ==( void*) Cyc_Absyn_FloatType){ goto
_LL557;} else{ goto _LL558;} _LL558: if( _temp548 ==( void*) Cyc_Absyn_DoubleType){
goto _LL559;} else{ goto _LL560;} _LL560: if(( unsigned int) _temp548 > 4u?*((
int*) _temp548) == Cyc_Absyn_ArrayType: 0){ _LL595: _temp594=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp548)->f1; goto _LL593; _LL593: _temp592=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp548)->f2;
goto _LL591; _LL591: _temp590=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp548)->f3; goto _LL561;} else{ goto _LL562;} _LL562: if(( unsigned int)
_temp548 > 4u?*(( int*) _temp548) == Cyc_Absyn_FnType: 0){ _LL597: _temp596=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp548)->f1; goto
_LL563;} else{ goto _LL564;} _LL564: if(( unsigned int) _temp548 > 4u?*(( int*)
_temp548) == Cyc_Absyn_TupleType: 0){ _LL599: _temp598=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp548)->f1; goto _LL565;} else{ goto
_LL566;} _LL566: if(( unsigned int) _temp548 > 4u?*(( int*) _temp548) == Cyc_Absyn_StructType:
0){ _LL605: _temp604=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp548)->f1; goto _LL603; _LL603: _temp602=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp548)->f2; goto _LL601; _LL601: _temp600=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp548)->f3;
goto _LL567;} else{ goto _LL568;} _LL568: if(( unsigned int) _temp548 > 4u?*((
int*) _temp548) == Cyc_Absyn_TunionType: 0){ _LL607: _temp606=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp548)->f1; goto _LL569;} else{ goto
_LL570;} _LL570: if(( unsigned int) _temp548 > 4u?*(( int*) _temp548) == Cyc_Absyn_AnonStructType:
0){ _LL609: _temp608=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp548)->f1; goto _LL571;} else{ goto _LL572;} _LL572: if(( unsigned int)
_temp548 > 4u?*(( int*) _temp548) == Cyc_Absyn_AnonUnionType: 0){ _LL611:
_temp610=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp548)->f1; goto _LL573;} else{ goto _LL574;} _LL574: if(( unsigned int)
_temp548 > 4u?*(( int*) _temp548) == Cyc_Absyn_TunionFieldType: 0){ _LL613:
_temp612=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp548)->f1; _LL617: _temp616=( void*) _temp612.field_info; if(*(( int*)
_temp616) == Cyc_Absyn_KnownTunionfield){ _LL621: _temp620=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp616)->f1; goto _LL619; _LL619:
_temp618=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp616)->f2; goto _LL615;} else{ goto _LL576;} _LL615: _temp614=( struct Cyc_List_List*)
_temp612.targs; goto _LL575;} else{ goto _LL576;} _LL576: if(( unsigned int)
_temp548 > 4u?*(( int*) _temp548) == Cyc_Absyn_UnionType: 0){ _LL627: _temp626=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp548)->f1; goto
_LL625; _LL625: _temp624=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp548)->f2; goto _LL623; _LL623: _temp622=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp548)->f3; goto _LL577;} else{ goto
_LL578;} _LL578: if(( unsigned int) _temp548 > 4u?*(( int*) _temp548) == Cyc_Absyn_PointerType:
0){ _LL629: _temp628=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp548)->f1; goto _LL579;} else{ goto _LL580;} _LL580: if(( unsigned int)
_temp548 > 4u?*(( int*) _temp548) == Cyc_Absyn_RgnHandleType: 0){ _LL631:
_temp630=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp548)->f1; goto
_LL581;} else{ goto _LL582;} _LL582: goto _LL583; _LL551: return 1; _LL553:
return 0; _LL555: return 1; _LL557: return 1; _LL559: return 0; _LL561: return
Cyc_Toc_atomic_typ( _temp594); _LL563: return 1; _LL565: for( 0; _temp598 != 0;
_temp598=({ struct Cyc_List_List* _temp632= _temp598; if( _temp632 == 0){ _throw(
Null_Exception);} _temp632->tl;})){ if( ! Cyc_Toc_atomic_typ((*(( struct _tuple3*)({
struct Cyc_List_List* _temp633= _temp598; if( _temp633 == 0){ _throw(
Null_Exception);} _temp633->hd;}))).f2)){ return 0;}} return 1; _LL567: return 0;
_LL569: return 0; _LL571: _temp610= _temp608; goto _LL573; _LL573: for( 0;
_temp610 != 0; _temp610=({ struct Cyc_List_List* _temp634= _temp610; if(
_temp634 == 0){ _throw( Null_Exception);} _temp634->tl;})){ if( ! Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp635=
_temp610; if( _temp635 == 0){ _throw( Null_Exception);} _temp635->hd;}))->type)){
return 0;}} return 1; _LL575: return _temp618->typs == 0; _LL577: return 0;
_LL579: return 0; _LL581: return 0; _LL583:(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp636= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _temp636.last_plus_one - _temp636.curr,
_temp636.curr);})); return 0; _LL549:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp637= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp643;
struct Cyc_Absyn_Conref* _temp645; struct Cyc_Absyn_Tqual _temp647; struct Cyc_Absyn_Conref*
_temp649; void* _temp651; void* _temp653; _LL639: if(( unsigned int) _temp637 >
4u?*(( int*) _temp637) == Cyc_Absyn_PointerType: 0){ _LL644: _temp643=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp637)->f1; _LL654:
_temp653=( void*) _temp643.elt_typ; goto _LL652; _LL652: _temp651=( void*)
_temp643.rgn_typ; goto _LL650; _LL650: _temp649=( struct Cyc_Absyn_Conref*)
_temp643.nullable; goto _LL648; _LL648: _temp647=( struct Cyc_Absyn_Tqual)
_temp643.tq; goto _LL646; _LL646: _temp645=( struct Cyc_Absyn_Conref*) _temp643.bounds;
goto _LL640;} else{ goto _LL641;} _LL641: goto _LL642; _LL640: { void* _temp655=
Cyc_Tcutil_compress( _temp653); _LL657: if( _temp655 ==( void*) Cyc_Absyn_VoidType){
goto _LL658;} else{ goto _LL659;} _LL659: goto _LL660; _LL658: return 1; _LL660:
return 0; _LL656:;} _LL642: return 0; _LL638:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp661= Cyc_Tcutil_compress( t);
struct Cyc_List_List* _temp677; struct Cyc_List_List* _temp679; struct Cyc_Absyn_Structdecl**
_temp681; struct Cyc_Absyn_Structdecl* _temp683; struct Cyc_List_List* _temp684;
struct _tuple0* _temp686; struct Cyc_Absyn_Structdecl** _temp688; struct Cyc_List_List*
_temp690; struct _tuple0* _temp692; struct Cyc_Absyn_Uniondecl** _temp694;
struct Cyc_Absyn_Uniondecl* _temp696; struct Cyc_List_List* _temp697; struct
_tuple0* _temp699; struct Cyc_Absyn_Uniondecl** _temp701; struct Cyc_List_List*
_temp703; struct _tuple0* _temp705; _LL663: if(( unsigned int) _temp661 > 4u?*((
int*) _temp661) == Cyc_Absyn_AnonUnionType: 0){ _LL678: _temp677=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp661)->f1; goto _LL664;} else{ goto
_LL665;} _LL665: if(( unsigned int) _temp661 > 4u?*(( int*) _temp661) == Cyc_Absyn_AnonStructType:
0){ _LL680: _temp679=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp661)->f1; goto _LL666;} else{ goto _LL667;} _LL667: if(( unsigned int)
_temp661 > 4u?*(( int*) _temp661) == Cyc_Absyn_StructType: 0){ _LL687: _temp686=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp661)->f1; goto
_LL685; _LL685: _temp684=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp661)->f2; goto _LL682; _LL682: _temp681=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp661)->f3; if( _temp681 == 0){ goto
_LL669;} else{ _temp683=* _temp681; goto _LL668;}} else{ goto _LL669;} _LL669:
if(( unsigned int) _temp661 > 4u?*(( int*) _temp661) == Cyc_Absyn_StructType: 0){
_LL693: _temp692=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp661)->f1; goto _LL691; _LL691: _temp690=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp661)->f2; goto _LL689; _LL689: _temp688=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp661)->f3;
if( _temp688 == 0){ goto _LL670;} else{ goto _LL671;}} else{ goto _LL671;}
_LL671: if(( unsigned int) _temp661 > 4u?*(( int*) _temp661) == Cyc_Absyn_UnionType:
0){ _LL700: _temp699=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp661)->f1; goto _LL698; _LL698: _temp697=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp661)->f2; goto _LL695; _LL695: _temp694=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp661)->f3;
if( _temp694 == 0){ goto _LL673;} else{ _temp696=* _temp694; goto _LL672;}}
else{ goto _LL673;} _LL673: if(( unsigned int) _temp661 > 4u?*(( int*) _temp661)
== Cyc_Absyn_UnionType: 0){ _LL706: _temp705=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp661)->f1; goto _LL704; _LL704: _temp703=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp661)->f2; goto _LL702; _LL702: _temp701=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp661)->f3;
if( _temp701 == 0){ goto _LL674;} else{ goto _LL675;}} else{ goto _LL675;}
_LL675: goto _LL676; _LL664: _temp679= _temp677; goto _LL666; _LL666: { struct
Cyc_Absyn_Structfield* _temp707= Cyc_Absyn_lookup_field( _temp679, f); goto
_LL708; _LL708: if( _temp707 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp710=*({ struct Cyc_Absyn_Structfield* _temp709=
_temp707; if( _temp709 == 0){ _throw( Null_Exception);} _temp709->name;});
xprintf("is_poly_field: bad field %.*s", _temp710.last_plus_one - _temp710.curr,
_temp710.curr);}));} return Cyc_Toc_is_void_star(( void*)({ struct Cyc_Absyn_Structfield*
_temp711= _temp707; if( _temp711 == 0){ _throw( Null_Exception);} _temp711->type;}));}
_LL668: if( _temp686 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp712=( char*)"StructType name not properly set";
struct _tagged_string _temp713; _temp713.curr= _temp712; _temp713.base= _temp712;
_temp713.last_plus_one= _temp712 + 33; _temp713;}));} if( _temp683->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp715= Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp714=
_temp686; if( _temp714 == 0){ _throw( Null_Exception);} _temp714;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp715.last_plus_one - _temp715.curr, _temp715.curr);}));}{ struct Cyc_List_List*
_temp717=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp716= _temp683->fields;
if( _temp716 == 0){ _throw( Null_Exception);} _temp716->v;}); goto _LL718;
_LL718: for( 0; _temp717 != 0; _temp717=({ struct Cyc_List_List* _temp719=
_temp717; if( _temp719 == 0){ _throw( Null_Exception);} _temp719->tl;})){ if(
Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp720= _temp717; if( _temp720 == 0){ _throw( Null_Exception);} _temp720->hd;}))->name)
== 0){ return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp721= _temp717; if( _temp721 == 0){ _throw(
Null_Exception);} _temp721->hd;}))->type);}}(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp723= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp722= _temp686; if( _temp722 == 0){
_throw( Null_Exception);} _temp722;})); struct _tagged_string _temp724=* f;
xprintf("is_poly_field:  struct %.*s has no %.*s field", _temp723.last_plus_one
- _temp723.curr, _temp723.curr, _temp724.last_plus_one - _temp724.curr, _temp724.curr);}));
return 0;} _LL670: if( _temp692 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp725=( char*)"StructType name not properly set";
struct _tagged_string _temp726; _temp726.curr= _temp725; _temp726.base= _temp725;
_temp726.last_plus_one= _temp725 + 33; _temp726;}));}(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp728= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp727= _temp692; if( _temp727 == 0){
_throw( Null_Exception);} _temp727;})); xprintf("is_poly_field:  struct %.*s missing structdecl",
_temp728.last_plus_one - _temp728.curr, _temp728.curr);})); return 0; _LL672:
if( _temp699 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp729=( char*)"UnionType name not properly set";
struct _tagged_string _temp730; _temp730.curr= _temp729; _temp730.base= _temp729;
_temp730.last_plus_one= _temp729 + 32; _temp730;}));} if( _temp696->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp732= Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp731=
_temp699; if( _temp731 == 0){ _throw( Null_Exception);} _temp731;})); xprintf("is_poly_field:  union %.*s missing fields",
_temp732.last_plus_one - _temp732.curr, _temp732.curr);}));}{ struct Cyc_List_List*
_temp734=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp733= _temp696->fields;
if( _temp733 == 0){ _throw( Null_Exception);} _temp733->v;}); goto _LL735;
_LL735: for( 0; _temp734 != 0; _temp734=({ struct Cyc_List_List* _temp736=
_temp734; if( _temp736 == 0){ _throw( Null_Exception);} _temp736->tl;})){ if(
Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp737= _temp734; if( _temp737 == 0){ _throw( Null_Exception);} _temp737->hd;}))->name)
== 0){ return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp738= _temp734; if( _temp738 == 0){ _throw(
Null_Exception);} _temp738->hd;}))->type);}}(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp740= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp739= _temp699; if( _temp739 == 0){
_throw( Null_Exception);} _temp739;})); struct _tagged_string _temp741=* f;
xprintf("is_poly_field:  union %.*s has no %.*s field", _temp740.last_plus_one -
_temp740.curr, _temp740.curr, _temp741.last_plus_one - _temp741.curr, _temp741.curr);}));
return 0;} _LL674: if( _temp705 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp742=( char*)"UnionType name not properly set";
struct _tagged_string _temp743; _temp743.curr= _temp742; _temp743.base= _temp742;
_temp743.last_plus_one= _temp742 + 32; _temp743;}));}(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp745= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp744= _temp705; if( _temp744 == 0){
_throw( Null_Exception);} _temp744;})); xprintf("is_poly_field:  union %.*s missing structdecl",
_temp745.last_plus_one - _temp745.curr, _temp745.curr);})); return 0; _LL676:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp746= Cyc_Absynpp_typ2string( t); xprintf("is_poly_field:  bad type %.*s",
_temp746.last_plus_one - _temp746.curr, _temp746.curr);})); return 0; _LL662:;}
static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void* _temp747=(
void*) e->r; struct _tagged_string* _temp755; struct Cyc_Absyn_Exp* _temp757;
struct _tagged_string* _temp759; struct Cyc_Absyn_Exp* _temp761; _LL749: if(*((
int*) _temp747) == Cyc_Absyn_StructMember_e){ _LL758: _temp757=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp747)->f1; goto _LL756; _LL756:
_temp755=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp747)->f2; goto _LL750;} else{ goto _LL751;} _LL751: if(*(( int*) _temp747)
== Cyc_Absyn_StructArrow_e){ _LL762: _temp761=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp747)->f1; goto _LL760; _LL760: _temp759=(
struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp747)->f2;
goto _LL752;} else{ goto _LL753;} _LL753: goto _LL754; _LL750: return Cyc_Toc_is_poly_field((
void*)({ struct Cyc_Core_Opt* _temp763= _temp757->topt; if( _temp763 == 0){
_throw( Null_Exception);} _temp763->v;}), _temp755); _LL752: { void* _temp765=
Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp764= _temp761->topt;
if( _temp764 == 0){ _throw( Null_Exception);} _temp764->v;})); struct Cyc_Absyn_PtrInfo
_temp771; struct Cyc_Absyn_Conref* _temp773; struct Cyc_Absyn_Tqual _temp775;
struct Cyc_Absyn_Conref* _temp777; void* _temp779; void* _temp781; _LL767: if((
unsigned int) _temp765 > 4u?*(( int*) _temp765) == Cyc_Absyn_PointerType: 0){
_LL772: _temp771=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp765)->f1; _LL782: _temp781=( void*) _temp771.elt_typ; goto _LL780; _LL780:
_temp779=( void*) _temp771.rgn_typ; goto _LL778; _LL778: _temp777=( struct Cyc_Absyn_Conref*)
_temp771.nullable; goto _LL776; _LL776: _temp775=( struct Cyc_Absyn_Tqual)
_temp771.tq; goto _LL774; _LL774: _temp773=( struct Cyc_Absyn_Conref*) _temp771.bounds;
goto _LL768;} else{ goto _LL769;} _LL769: goto _LL770; _LL768: return Cyc_Toc_is_poly_field(
_temp781, _temp759); _LL770:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp784= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp783= _temp761->topt; if( _temp783 == 0){ _throw( Null_Exception);} _temp783->v;}));
xprintf("is_poly_project:  bad type %.*s", _temp784.last_plus_one - _temp784.curr,
_temp784.curr);})); return 0; _LL766:;} _LL754: return 0; _LL748:;} static char
_temp787[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp787, _temp787, _temp787 + 10u}; static struct
_tuple0 Cyc_Toc_gc_malloc_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var,
0),({ struct Cyc_List_List* _temp788=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp788->hd=( void*) s; _temp788->tl= 0; _temp788;}), 0);}
static char _temp791[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp791, _temp791, _temp791 + 17u}; static struct
_tuple0 Cyc_Toc_gc_malloc_atomic_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_atomic_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var,
0),({ struct Cyc_List_List* _temp792=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp792->hd=( void*) s; _temp792->tl= 0; _temp792;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static char _temp795[ 15u]="_region_malloc"; static struct _tagged_string
Cyc_Toc__region_malloc_string=( struct _tagged_string){ _temp795, _temp795,
_temp795 + 15u}; static struct _tuple0 Cyc_Toc_region_malloc_var=( struct
_tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__region_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_region_malloc_var,
0),({ struct Cyc_List_List* _temp796=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp796->hd=( void*) rgn; _temp796->tl=({ struct Cyc_List_List*
_temp797=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp797->hd=( void*) s; _temp797->tl= 0; _temp797;}); _temp796;}), 0);} static
const int Cyc_Toc_NullCheck= 0; struct Cyc_Toc_NullCheck_struct{ int tag; void*
f1; } ; static const int Cyc_Toc_UntagPtr= 1; struct Cyc_Toc_UntagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; int f3; } ; static const int Cyc_Toc_TagPtr=
2; struct Cyc_Toc_TagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; }
; typedef void* Cyc_Toc_conv; struct _tuple7{ void* f1; void* f2; } ; static
struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp799=({ struct _tuple7
_temp798; _temp798.f1= t1; _temp798.f2= t2; _temp798;}); void* _temp805; struct
Cyc_Absyn_PtrInfo _temp807; void* _temp809; struct Cyc_Absyn_PtrInfo _temp811;
_LL801: _LL810: _temp809= _temp799.f1; if(( unsigned int) _temp809 > 4u?*(( int*)
_temp809) == Cyc_Absyn_PointerType: 0){ _LL812: _temp811=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp809)->f1; goto _LL806;} else{ goto
_LL803;} _LL806: _temp805= _temp799.f2; if(( unsigned int) _temp805 > 4u?*(( int*)
_temp805) == Cyc_Absyn_PointerType: 0){ _LL808: _temp807=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp805)->f1; goto _LL802;} else{ goto
_LL803;} _LL803: goto _LL804; _LL802: { int _temp813=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp811.nullable); goto _LL814; _LL814: { int
_temp815=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp807.nullable);
goto _LL816; _LL816: { void* _temp817=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp811.bounds); goto _LL818; _LL818: { void* _temp819=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp807.bounds);
goto _LL820; _LL820: { struct Cyc_List_List* convs= 0;{ struct _tuple7 _temp822=({
struct _tuple7 _temp821; _temp821.f1= _temp817; _temp821.f2= _temp819; _temp821;});
void* _temp832; struct Cyc_Absyn_Exp* _temp834; void* _temp836; struct Cyc_Absyn_Exp*
_temp838; void* _temp840; void* _temp842; void* _temp844; void* _temp846; struct
Cyc_Absyn_Exp* _temp848; void* _temp850; struct Cyc_Absyn_Exp* _temp852; void*
_temp854; _LL824: _LL837: _temp836= _temp822.f1; if(( unsigned int) _temp836 > 1u?*((
int*) _temp836) == Cyc_Absyn_Upper_b: 0){ _LL839: _temp838=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp836)->f1; goto _LL833;} else{ goto _LL826;}
_LL833: _temp832= _temp822.f2; if(( unsigned int) _temp832 > 1u?*(( int*)
_temp832) == Cyc_Absyn_Upper_b: 0){ _LL835: _temp834=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp832)->f1; goto _LL825;} else{ goto _LL826;}
_LL826: _LL843: _temp842= _temp822.f1; if( _temp842 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL841;} else{ goto _LL828;} _LL841: _temp840= _temp822.f2; if( _temp840 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL827;} else{ goto _LL828;} _LL828: _LL847:
_temp846= _temp822.f1; if(( unsigned int) _temp846 > 1u?*(( int*) _temp846) ==
Cyc_Absyn_Upper_b: 0){ _LL849: _temp848=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp846)->f1; goto _LL845;} else{ goto _LL830;} _LL845: _temp844= _temp822.f2;
if( _temp844 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL829;} else{ goto _LL830;}
_LL830: _LL855: _temp854= _temp822.f1; if( _temp854 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL851;} else{ goto _LL823;} _LL851: _temp850= _temp822.f2; if((
unsigned int) _temp850 > 1u?*(( int*) _temp850) == Cyc_Absyn_Upper_b: 0){ _LL853:
_temp852=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp850)->f1;
goto _LL831;} else{ goto _LL823;} _LL825: if( _temp813? ! _temp815: 0){ convs=({
struct Cyc_List_List* _temp856=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp856->hd=( void*)(( void*)({ struct Cyc_Toc_NullCheck_struct*
_temp857=( struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp857[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp858; _temp858.tag= Cyc_Toc_NullCheck;
_temp858.f1=( void*) t2; _temp858;}); _temp857;})); _temp856->tl= convs;
_temp856;});} goto _LL823; _LL827: goto _LL823; _LL829: convs=({ struct Cyc_List_List*
_temp859=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp859->hd=( void*)(( void*)({ struct Cyc_Toc_TagPtr_struct* _temp860=( struct
Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp860[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp861; _temp861.tag= Cyc_Toc_TagPtr;
_temp861.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp861.f2= _temp848; _temp861;});
_temp860;})); _temp859->tl= convs; _temp859;}); goto _LL823; _LL831: convs=({
struct Cyc_List_List* _temp862=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp862->hd=( void*)(( void*)({ struct Cyc_Toc_UntagPtr_struct*
_temp863=( struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp863[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp864; _temp864.tag= Cyc_Toc_UntagPtr;
_temp864.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp864.f2= _temp852; _temp864.f3=
_temp815; _temp864;}); _temp863;})); _temp862->tl= convs; _temp862;}); goto
_LL823; _LL823:;} return convs;}}}}} _LL804: return 0; _LL800:;}} static char
_temp867[ 7u]="_throw"; static struct _tagged_string Cyc_Toc__throw_string=(
struct _tagged_string){ _temp867, _temp867, _temp867 + 7u}; static struct
_tuple0 Cyc_Toc__throw_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__throw_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__throw_var,
0),({ struct Cyc_List_List* _temp868=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp868->hd=( void*) e; _temp868->tl= 0; _temp868;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp869=( char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp870; _temp870.curr= _temp869; _temp870.base= _temp869;
_temp870.last_plus_one= _temp869 + 44; _temp870;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple0* a= Cyc_Toc_temp_var();
struct _tuple0* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp871= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp877; struct Cyc_Absyn_Conref* _temp879; struct Cyc_Absyn_Tqual
_temp881; struct Cyc_Absyn_Conref* _temp883; void* _temp885; void* _temp887;
_LL873: if(( unsigned int) _temp871 > 4u?*(( int*) _temp871) == Cyc_Absyn_PointerType:
0){ _LL878: _temp877=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp871)->f1; _LL888: _temp887=( void*) _temp877.elt_typ; goto _LL886; _LL886:
_temp885=( void*) _temp877.rgn_typ; goto _LL884; _LL884: _temp883=( struct Cyc_Absyn_Conref*)
_temp877.nullable; goto _LL882; _LL882: _temp881=( struct Cyc_Absyn_Tqual)
_temp877.tq; goto _LL880; _LL880: _temp879=( struct Cyc_Absyn_Conref*) _temp877.bounds;
goto _LL874;} else{ goto _LL875;} _LL875: goto _LL876; _LL874: elt_typ= _temp887;{
struct _tagged_string* _temp889= Cyc_Toc_add_tagged_ptr_type( _temp887, Cyc_Toc_mt_tq);
goto _LL890; _LL890: strt_typ= Cyc_Absyn_strct( _temp889); goto _LL872;} _LL876:
return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp891=( char*)"tag_ptr_conv: not a pointer";
struct _tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 28; _temp892;})); _LL872:;} if( toplevel){
int is_string= 0;{ void* _temp893=( void*) e->r; void* _temp899; struct
_tagged_string _temp901; _LL895: if(*(( int*) _temp893) == Cyc_Absyn_Const_e){
_LL900: _temp899=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp893)->f1; if((
unsigned int) _temp899 > 1u?*(( int*) _temp899) == Cyc_Absyn_String_c: 0){
_LL902: _temp901=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp899)->f1; goto _LL896;} else{ goto _LL897;}} else{ goto _LL897;} _LL897:
goto _LL898; _LL896: is_string= 1; goto _LL894; _LL898: goto _LL894; _LL894:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp906=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp906[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp907; _temp907.tag= Cyc_Absyn_ArrayType; _temp907.f1=(
void*) elt_typ; _temp907.f2= Cyc_Toc_mt_tq; _temp907.f3=( struct Cyc_Absyn_Exp*)
sz; _temp907;}); _temp906;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp903=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp903->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp904=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp904[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp905; _temp905.tag= Cyc_Absyn_Var_d;
_temp905.f1= vd; _temp905;}); _temp904;}), 0); _temp903->tl= Cyc_Toc_result_decls;
_temp903;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp908=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp908->hd=( void*)({ struct _tuple4* _temp913=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp913->f1= 0; _temp913->f2=
xexp; _temp913;}); _temp908->tl=({ struct Cyc_List_List* _temp909=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp909->hd=( void*)({ struct
_tuple4* _temp912=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp912->f1= 0; _temp912->f2= xexp; _temp912;}); _temp909->tl=({ struct Cyc_List_List*
_temp910=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp910->hd=( void*)({ struct _tuple4* _temp911=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp911->f1= 0; _temp911->f2= xplussz; _temp911;});
_temp910->tl= 0; _temp910;}); _temp909;}); _temp908;}), 0); return urm_exp;}}}
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
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp914=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp915; _temp915.curr= _temp914; _temp915.base= _temp914; _temp915.last_plus_one=
_temp914 + 45; _temp915;}));}{ struct _tuple0* x= Cyc_Toc_temp_var(); int
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
_temp916= cs; if( _temp916 == 0){ _throw( Null_Exception);} _temp916->tl;})){
void* _temp918=( void*)({ struct Cyc_List_List* _temp917= cs; if( _temp917 == 0){
_throw( Null_Exception);} _temp917->hd;}); void* _temp926; struct Cyc_Absyn_Exp*
_temp928; void* _temp930; int _temp932; struct Cyc_Absyn_Exp* _temp934; void*
_temp936; _LL920: if(*(( int*) _temp918) == Cyc_Toc_NullCheck){ _LL927: _temp926=(
void*)(( struct Cyc_Toc_NullCheck_struct*) _temp918)->f1; goto _LL921;} else{
goto _LL922;} _LL922: if(*(( int*) _temp918) == Cyc_Toc_TagPtr){ _LL931:
_temp930=( void*)(( struct Cyc_Toc_TagPtr_struct*) _temp918)->f1; goto _LL929;
_LL929: _temp928=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_TagPtr_struct*)
_temp918)->f2; goto _LL923;} else{ goto _LL924;} _LL924: if(*(( int*) _temp918)
== Cyc_Toc_UntagPtr){ _LL937: _temp936=( void*)(( struct Cyc_Toc_UntagPtr_struct*)
_temp918)->f1; goto _LL935; _LL935: _temp934=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Toc_UntagPtr_struct*) _temp918)->f2; goto _LL933; _LL933: _temp932=( int)((
struct Cyc_Toc_UntagPtr_struct*) _temp918)->f3; goto _LL925;} else{ goto _LL919;}
_LL921: e= Cyc_Toc_null_check_conv( toplevel, _temp926, e); goto _LL919; _LL923:
e= Cyc_Toc_tag_ptr_conv( toplevel, _temp930, _temp928, e); goto _LL919; _LL925:
e= Cyc_Toc_untag_ptr_conv( toplevel, _temp936, _temp934, _temp932, e); goto
_LL919; _LL919:;}} return e;} struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab;
struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt* fallthru_info; struct
Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct Cyc_Toc_Env* Cyc_Toc_env_t;
static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({ struct Cyc_Toc_Env*
_temp938=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct Cyc_Toc_Env));
_temp938->break_lab= 0; _temp938->continue_lab= 0; _temp938->fallthru_info= 0;
_temp938->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp938->toplevel= 1;
_temp938;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp939=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp939->break_lab= e->break_lab; _temp939->continue_lab=
e->continue_lab; _temp939->fallthru_info= e->fallthru_info; _temp939->varmap= e->varmap;
_temp939->toplevel= e->toplevel; _temp939;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp940=(* x).f1; struct
Cyc_List_List* _temp946; _LL942: if(( unsigned int) _temp940 > 1u?*(( int*)
_temp940) == Cyc_Absyn_Rel_n: 0){ _LL947: _temp946=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp940)->f1; goto _LL943;} else{ goto _LL944;}
_LL944: goto _LL945; _LL943:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp948=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp948[ 0]=({ struct Cyc_Core_Impossible_struct _temp949; _temp949.tag= Cyc_Core_Impossible;
_temp949.f1=({ struct _tagged_string _temp950= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp950.last_plus_one - _temp950.curr,
_temp950.curr);}); _temp949;}); _temp948;})); _LL945: goto _LL941; _LL941:;}{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)(
ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=
0; ans->continue_lab= 0; return ans;} struct _tuple8{ struct _tagged_string* f1;
struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env*
Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_string*
break_l, struct _tagged_string* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
_temp951= 0; goto _LL952; _LL952: for( 0; fallthru_binders != 0;
fallthru_binders=({ struct Cyc_List_List* _temp953= fallthru_binders; if(
_temp953 == 0){ _throw( Null_Exception);} _temp953->tl;})){ _temp951=({ struct
Cyc_List_List* _temp954=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp954->hd=( void*)(( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List*
_temp955= fallthru_binders; if( _temp955 == 0){ _throw( Null_Exception);}
_temp955->hd;}))->name; _temp954->tl= _temp951; _temp954;});} _temp951=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp951);{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt*
_temp956=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp956->v=( void*) break_l; _temp956;}); ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp957=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp957->v=( void*)({ struct _tuple8* _temp958=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp958->f1= fallthru_l; _temp958->f2= _temp951;
_temp958->f3= next_case_env->varmap; _temp958;}); _temp957;}); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e,
struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp959=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp959->v=( void*) break_l; _temp959;});
ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env* e, struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp960=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp960->v=( void*)({ struct _tuple8* _temp961=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp961->f1= next_l; _temp961->f2= 0; _temp961->f3=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp961;}); _temp960;}); return ans;} struct _tuple9{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
struct Cyc_List_List* f4; char f5; int f6; } ; struct _tuple10{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp962=( char*)"expecting a literal format string";
struct _tagged_string _temp963; _temp963.curr= _temp962; _temp963.base= _temp962;
_temp963.last_plus_one= _temp962 + 34; _temp963;}));}{ struct _tagged_string
fmt_str;{ void* _temp965=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp964= args; if( _temp964 == 0){ _throw( Null_Exception);} _temp964->hd;}))->r;
void* _temp971; struct _tagged_string _temp973; _LL967: if(*(( int*) _temp965)
== Cyc_Absyn_Const_e){ _LL972: _temp971=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp965)->f1; if(( unsigned int) _temp971 > 1u?*(( int*) _temp971) == Cyc_Absyn_String_c:
0){ _LL974: _temp973=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp971)->f1; goto _LL968;} else{ goto _LL969;}} else{ goto _LL969;} _LL969:
goto _LL970; _LL968: fmt_str= _temp973; goto _LL966; _LL970: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp975=( char*)"expecting a literal format string"; struct _tagged_string
_temp976; _temp976.curr= _temp975; _temp976.base= _temp975; _temp976.last_plus_one=
_temp975 + 34; _temp976;})); _LL966:;}{ int len=( int)({ struct _tagged_string
_temp1103= fmt_str;( unsigned int)( _temp1103.last_plus_one - _temp1103.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(({ struct _tagged_string _temp977=
fmt_str; char* _temp979= _temp977.curr + i; if( _temp977.base == 0? 1:( _temp979
< _temp977.base? 1: _temp979 >= _temp977.last_plus_one)){ _throw( Null_Exception);}*
_temp979;}) =='%'){ i ++; if( i < len?({ struct _tagged_string _temp980= fmt_str;
char* _temp982= _temp980.curr + i; if( _temp980.base == 0? 1:( _temp982 <
_temp980.base? 1: _temp982 >= _temp980.last_plus_one)){ _throw( Null_Exception);}*
_temp982;}) =='s': 0){ break;}}} if( i == len){ if( aopt != 0){ args=({ struct
Cyc_List_List* _temp983=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp983->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp984=
aopt; if( _temp984 == 0){ _throw( Null_Exception);} _temp984->v;})); _temp983->tl=
args; _temp983;});} return Cyc_Absyn_primop_exp( p, args, 0);} args=({ struct
Cyc_List_List* _temp985= args; if( _temp985 == 0){ _throw( Null_Exception);}
_temp985->tl;}); typs=({ struct Cyc_List_List* _temp986= typs; if( _temp986 == 0){
_throw( Null_Exception);} _temp986->tl;});{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ char c=({ struct _tagged_string _temp1087= fmt_str; char*
_temp1089= _temp1087.curr + i; if( _temp1087.base == 0? 1:( _temp1089 <
_temp1087.base? 1: _temp1089 >= _temp1087.last_plus_one)){ _throw(
Null_Exception);}* _temp1089;}); rev_fmt=({ struct Cyc_List_List* _temp987=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp987->hd=(
void*)(( int) c); _temp987->tl= rev_fmt; _temp987;}); if( c !='%'){ continue;}{
struct Cyc_Core_Opt* _temp988= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); goto _LL989; _LL989: if( _temp988 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp990=(
char*)"bad format string"; struct _tagged_string _temp991; _temp991.curr=
_temp990; _temp991.base= _temp990; _temp991.last_plus_one= _temp990 + 18;
_temp991;}));}{ struct _tuple9 _temp995; int _temp996; char _temp998; struct Cyc_List_List*
_temp1000; struct Cyc_List_List* _temp1002; struct Cyc_List_List* _temp1004;
struct Cyc_List_List* _temp1006; struct _tuple9* _temp993=( struct _tuple9*)({
struct Cyc_Core_Opt* _temp992= _temp988; if( _temp992 == 0){ _throw(
Null_Exception);} _temp992->v;}); _temp995=* _temp993; _LL1007: _temp1006=
_temp995.f1; goto _LL1005; _LL1005: _temp1004= _temp995.f2; goto _LL1003;
_LL1003: _temp1002= _temp995.f3; goto _LL1001; _LL1001: _temp1000= _temp995.f4;
goto _LL999; _LL999: _temp998= _temp995.f5; goto _LL997; _LL997: _temp996=
_temp995.f6; goto _LL994; _LL994: i= _temp996 - 1; if( _temp998 !='s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp1008=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1008->hd=( void*) _temp1006; _temp1008->tl=({ struct Cyc_List_List*
_temp1009=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1009->hd=( void*) _temp1004; _temp1009->tl=({ struct Cyc_List_List*
_temp1010=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1010->hd=( void*) _temp1002; _temp1010->tl=({ struct Cyc_List_List*
_temp1011=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1011->hd=( void*) _temp1000; _temp1011->tl= 0; _temp1011;}); _temp1010;});
_temp1009;}); _temp1008;})), rev_fmt); rev_fmt=({ struct Cyc_List_List*
_temp1012=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1012->hd=( void*)(( int) _temp998); _temp1012->tl= rev_fmt; _temp1012;});{
struct Cyc_List_List* _temp1013= _temp1004; struct Cyc_List_List _temp1020;
struct Cyc_List_List* _temp1021; int _temp1023; _LL1015: if( _temp1013 == 0){
goto _LL1017;} else{ _temp1020=* _temp1013; _LL1024: _temp1023=( int) _temp1020.hd;
goto _LL1022; _LL1022: _temp1021=( struct Cyc_List_List*) _temp1020.tl; if(
_temp1021 == 0){ goto _LL1019;} else{ goto _LL1017;}} _LL1019: if( _temp1023 =='*'){
goto _LL1016;} else{ goto _LL1017;} _LL1017: goto _LL1018; _LL1016: { struct
_tuple0* _temp1025= Cyc_Toc_temp_var(); goto _LL1026; _LL1026: rev_temps=({
struct Cyc_List_List* _temp1027=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1027->hd=( void*)({ struct _tuple10* _temp1028=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1028->f1= _temp1025;
_temp1028->f2=( void*)({ struct Cyc_List_List* _temp1030= typs; if( _temp1030 ==
0){ _throw( Null_Exception);} _temp1030->hd;}); _temp1028->f3=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1029= args; if( _temp1029 == 0){ _throw(
Null_Exception);} _temp1029->hd;}); _temp1028;}); _temp1027->tl= rev_temps;
_temp1027;}); rev_result=({ struct Cyc_List_List* _temp1031=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1031->hd=( void*) Cyc_Absyn_var_exp(
_temp1025, 0); _temp1031->tl= rev_result; _temp1031;}); args=({ struct Cyc_List_List*
_temp1032= args; if( _temp1032 == 0){ _throw( Null_Exception);} _temp1032->tl;});
typs=({ struct Cyc_List_List* _temp1033= typs; if( _temp1033 == 0){ _throw(
Null_Exception);} _temp1033->tl;}); goto _LL1014;} _LL1018: goto _LL1014;
_LL1014:;}{ struct Cyc_List_List* _temp1034= _temp1002; struct Cyc_List_List
_temp1041; struct Cyc_List_List* _temp1042; struct Cyc_List_List _temp1044;
struct Cyc_List_List* _temp1045; int _temp1047; int _temp1049; _LL1036: if(
_temp1034 == 0){ goto _LL1038;} else{ _temp1041=* _temp1034; _LL1050: _temp1049=(
int) _temp1041.hd; goto _LL1043; _LL1043: _temp1042=( struct Cyc_List_List*)
_temp1041.tl; if( _temp1042 == 0){ goto _LL1038;} else{ _temp1044=* _temp1042;
_LL1048: _temp1047=( int) _temp1044.hd; goto _LL1046; _LL1046: _temp1045=(
struct Cyc_List_List*) _temp1044.tl; if( _temp1045 == 0){ goto _LL1040;} else{
goto _LL1038;}}} _LL1040: if( _temp1049 =='.'? _temp1047 =='*': 0){ goto _LL1037;}
else{ goto _LL1038;} _LL1038: goto _LL1039; _LL1037: { struct _tuple0* _temp1051=
Cyc_Toc_temp_var(); goto _LL1052; _LL1052: rev_temps=({ struct Cyc_List_List*
_temp1053=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1053->hd=( void*)({ struct _tuple10* _temp1054=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1054->f1= _temp1051; _temp1054->f2=(
void*)({ struct Cyc_List_List* _temp1056= typs; if( _temp1056 == 0){ _throw(
Null_Exception);} _temp1056->hd;}); _temp1054->f3=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1055= args; if( _temp1055 == 0){ _throw(
Null_Exception);} _temp1055->hd;}); _temp1054;}); _temp1053->tl= rev_temps;
_temp1053;}); rev_result=({ struct Cyc_List_List* _temp1057=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1057->hd=( void*) Cyc_Absyn_var_exp(
_temp1051, 0); _temp1057->tl= rev_result; _temp1057;}); args=({ struct Cyc_List_List*
_temp1058= args; if( _temp1058 == 0){ _throw( Null_Exception);} _temp1058->tl;});
typs=({ struct Cyc_List_List* _temp1059= typs; if( _temp1059 == 0){ _throw(
Null_Exception);} _temp1059->tl;}); goto _LL1035;} _LL1039: goto _LL1035;
_LL1035:;} if( _temp998 !='%'){ if( args == 0? 1: typs == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1060= Cyc_Absynpp_prim2string(
p); xprintf("missing arg to %.*s", _temp1060.last_plus_one - _temp1060.curr,
_temp1060.curr);}));}{ struct _tuple0* _temp1061= Cyc_Toc_temp_var(); goto
_LL1062; _LL1062: rev_temps=({ struct Cyc_List_List* _temp1063=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1063->hd=( void*)({ struct
_tuple10* _temp1064=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp1064->f1= _temp1061; _temp1064->f2=( void*)({ struct Cyc_List_List*
_temp1066= typs; if( _temp1066 == 0){ _throw( Null_Exception);} _temp1066->hd;});
_temp1064->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1065= args;
if( _temp1065 == 0){ _throw( Null_Exception);} _temp1065->hd;}); _temp1064;});
_temp1063->tl= rev_temps; _temp1063;}); rev_result=({ struct Cyc_List_List*
_temp1067=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1067->hd=( void*) Cyc_Absyn_var_exp( _temp1061, 0); _temp1067->tl=
rev_result; _temp1067;}); args=({ struct Cyc_List_List* _temp1068= args; if(
_temp1068 == 0){ _throw( Null_Exception);} _temp1068->tl;}); typs=({ struct Cyc_List_List*
_temp1069= typs; if( _temp1069 == 0){ _throw( Null_Exception);} _temp1069->tl;});}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp1070=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1070->hd=( void*)(( int)'.');
_temp1070->tl= rev_fmt; _temp1070;}); rev_fmt=({ struct Cyc_List_List* _temp1071=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1071->hd=(
void*)(( int)'*'); _temp1071->tl= rev_fmt; _temp1071;}); rev_fmt=({ struct Cyc_List_List*
_temp1072=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1072->hd=( void*)(( int)'s'); _temp1072->tl= rev_fmt; _temp1072;});{ struct
_tuple0* _temp1073= Cyc_Toc_temp_var(); goto _LL1074; _LL1074: rev_temps=({
struct Cyc_List_List* _temp1075=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1075->hd=( void*)({ struct _tuple10* _temp1076=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1076->f1= _temp1073;
_temp1076->f2=( void*)({ struct Cyc_List_List* _temp1078= typs; if( _temp1078 ==
0){ _throw( Null_Exception);} _temp1078->hd;}); _temp1076->f3=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1077= args; if( _temp1077 == 0){ _throw(
Null_Exception);} _temp1077->hd;}); _temp1076;}); _temp1075->tl= rev_temps;
_temp1075;});{ struct Cyc_Absyn_Exp* _temp1079= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1073, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1073, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL1080; _LL1080: {
struct Cyc_Absyn_Exp* _temp1081= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1073, 0), Cyc_Toc_curr_sp, 0); goto _LL1082; _LL1082: rev_result=({ struct
Cyc_List_List* _temp1083=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1083->hd=( void*) _temp1079; _temp1083->tl= rev_result; _temp1083;});
rev_result=({ struct Cyc_List_List* _temp1084=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1084->hd=( void*) _temp1081; _temp1084->tl=
rev_result; _temp1084;}); args=({ struct Cyc_List_List* _temp1085= args; if(
_temp1085 == 0){ _throw( Null_Exception);} _temp1085->tl;}); typs=({ struct Cyc_List_List*
_temp1086= typs; if( _temp1086 == 0){ _throw( Null_Exception);} _temp1086->tl;});}}}}}}}{
struct Cyc_List_List* _temp1091=({ struct Cyc_List_List* _temp1090=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1090->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp1090->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_result); _temp1090;}); goto _LL1092; _LL1092: if( aopt !=
0){ _temp1091=({ struct Cyc_List_List* _temp1093=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1093->hd=( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp1094= aopt; if( _temp1094 == 0){ _throw(
Null_Exception);} _temp1094->v;})); _temp1093->tl= _temp1091; _temp1093;});}{
struct Cyc_Absyn_Exp* _temp1095= Cyc_Absyn_primop_exp( p, _temp1091, 0); goto
_LL1096; _LL1096: { struct Cyc_Absyn_Stmt* _temp1097= Cyc_Absyn_exp_stmt(
_temp1095, 0); goto _LL1098; _LL1098: for( 0; rev_temps != 0; rev_temps=({
struct Cyc_List_List* _temp1099= rev_temps; if( _temp1099 == 0){ _throw(
Null_Exception);} _temp1099->tl;})){ _temp1097= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)({ struct Cyc_List_List* _temp1100= rev_temps; if( _temp1100 ==
0){ _throw( Null_Exception);} _temp1100->hd;}))).f1,(*(( struct _tuple10*)({
struct Cyc_List_List* _temp1101= rev_temps; if( _temp1101 == 0){ _throw(
Null_Exception);} _temp1101->hd;}))).f2,( struct Cyc_Absyn_Exp*)(*(( struct
_tuple10*)({ struct Cyc_List_List* _temp1102= rev_temps; if( _temp1102 == 0){
_throw( Null_Exception);} _temp1102->hd;}))).f3, _temp1097, 0);} return Cyc_Absyn_stmt_exp(
_temp1097, 0);}}}}}}} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){
if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1104=( char*)"Missing type in primop ";
struct _tagged_string _temp1105; _temp1105.curr= _temp1104; _temp1105.base=
_temp1104; _temp1105.last_plus_one= _temp1104 + 24; _temp1105;}));} return Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp1106= e->topt; if( _temp1106 == 0){ _throw(
Null_Exception);} _temp1106->v;}));} static void* Cyc_Toc_get_cyc_typ( struct
Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1107=( char*)"Missing type in primop ";
struct _tagged_string _temp1108; _temp1108.curr= _temp1107; _temp1108.base=
_temp1107; _temp1108.last_plus_one= _temp1107 + 24; _temp1108;}));} return( void*)({
struct Cyc_Core_Opt* _temp1109= e->topt; if( _temp1109 == 0){ _throw(
Null_Exception);} _temp1109->v;});} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1110=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1110->f1= Cyc_Toc_mt_tq; _temp1110->f2=
Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1111= e->topt; if(
_temp1111 == 0){ _throw( Null_Exception);} _temp1111->v;})); _temp1110;});}
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e);
static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp1112=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1112[ 0]=({
struct _tuple4 _temp1113; _temp1113.f1= 0; _temp1113.f2= e; _temp1113;});
_temp1112;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
_temp1114= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); goto _LL1115; _LL1115: if(
rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic(
_temp1114): Cyc_Toc_malloc_ptr( _temp1114), 0);} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1116= rgnopt; if( _temp1116
== 0){ _throw( Null_Exception);} _temp1116;}); Cyc_Toc_exp_to_c( nv, r); eo=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp( r, _temp1114),
0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp1117=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); goto _LL1118; _LL1118: for( 0;
_temp1117 != 0; _temp1117=({ struct Cyc_List_List* _temp1119= _temp1117; if(
_temp1119 == 0){ _throw( Null_Exception);} _temp1119->tl;})){ struct _tuple4
_temp1123; struct Cyc_Absyn_Exp* _temp1124; struct Cyc_List_List* _temp1126;
struct _tuple4* _temp1121=( struct _tuple4*)({ struct Cyc_List_List* _temp1120=
_temp1117; if( _temp1120 == 0){ _throw( Null_Exception);} _temp1120->hd;});
_temp1123=* _temp1121; _LL1127: _temp1126= _temp1123.f1; goto _LL1125; _LL1125:
_temp1124= _temp1123.f2; goto _LL1122; _LL1122: { struct Cyc_Absyn_Exp* e_index;
if( _temp1126 == 0){ e_index= Cyc_Absyn_signed_int_exp(( count --), 0);} else{
if(({ struct Cyc_List_List* _temp1128= _temp1126; if( _temp1128 == 0){ _throw(
Null_Exception);} _temp1128->tl;}) != 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1129=( char*)"multiple designators in array";
struct _tagged_string _temp1130; _temp1130.curr= _temp1129; _temp1130.base=
_temp1129; _temp1130.last_plus_one= _temp1129 + 30; _temp1130;}));}{ void*
_temp1132=( void*)({ struct Cyc_List_List* _temp1131= _temp1126; if( _temp1131
== 0){ _throw( Null_Exception);} _temp1131->hd;}); goto _LL1133; _LL1133: { void*
_temp1134= _temp1132; struct Cyc_Absyn_Exp* _temp1140; struct _tagged_string*
_temp1142; _LL1136: if(*(( int*) _temp1134) == Cyc_Absyn_ArrayElement){ _LL1141:
_temp1140=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1134)->f1; goto _LL1137;} else{ goto _LL1138;} _LL1138: if(*(( int*)
_temp1134) == Cyc_Absyn_FieldName){ _LL1143: _temp1142=( struct _tagged_string*)((
struct Cyc_Absyn_FieldName_struct*) _temp1134)->f1; goto _LL1139;} else{ goto
_LL1135;} _LL1137: Cyc_Toc_exp_to_c( nv, _temp1140); e_index= _temp1140; goto
_LL1135; _LL1139: e_index=(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1144=( char*)"field name designators in array";
struct _tagged_string _temp1145; _temp1145.curr= _temp1144; _temp1145.base=
_temp1144; _temp1145.last_plus_one= _temp1144 + 32; _temp1145;})); goto _LL1135;
_LL1135:;}}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index,
0); void* _temp1146=( void*) _temp1124->r; struct Cyc_List_List* _temp1156;
struct Cyc_Absyn_Exp* _temp1158; struct Cyc_Absyn_Exp* _temp1160; struct Cyc_Absyn_Vardecl*
_temp1162; struct Cyc_List_List* _temp1164; void* _temp1166; _LL1148: if(*(( int*)
_temp1146) == Cyc_Absyn_Array_e){ _LL1157: _temp1156=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1146)->f1; goto _LL1149;} else{ goto
_LL1150;} _LL1150: if(*(( int*) _temp1146) == Cyc_Absyn_Comprehension_e){
_LL1163: _temp1162=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1146)->f1; goto _LL1161; _LL1161: _temp1160=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1146)->f2; goto _LL1159; _LL1159:
_temp1158=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1146)->f3; goto _LL1151;} else{ goto _LL1152;} _LL1152: if(*(( int*)
_temp1146) == Cyc_Absyn_AnonStruct_e){ _LL1167: _temp1166=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1146)->f1; goto _LL1165; _LL1165: _temp1164=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1146)->f2; goto _LL1153;} else{ goto
_LL1154;} _LL1154: goto _LL1155; _LL1149: s= Cyc_Toc_init_array( nv, lval,
_temp1156, s); goto _LL1147; _LL1151: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1162, _temp1160, _temp1158, s, 0); goto _LL1147; _LL1153: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp1166, _temp1164, s); goto _LL1147; _LL1155: Cyc_Toc_exp_to_c( nv,
_temp1124); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp1124, 0), s, 0); goto _LL1147; _LL1147:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp1168= vd->name; goto _LL1169;
_LL1169: if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp1168, Cyc_Absyn_varb_exp( _temp1168,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1192=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1192[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1193; _temp1193.tag= Cyc_Absyn_Local_b; _temp1193.f1=
vd; _temp1193;}); _temp1192;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( _temp1168, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct
Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( _temp1168, 0), Cyc_Absyn_var_exp(
max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp(
_temp1168, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp(
_temp1168, 0), 0); struct Cyc_Absyn_Stmt* body;{ void* _temp1170=( void*) e2->r;
struct Cyc_List_List* _temp1180; struct Cyc_Absyn_Exp* _temp1182; struct Cyc_Absyn_Exp*
_temp1184; struct Cyc_Absyn_Vardecl* _temp1186; struct Cyc_List_List* _temp1188;
void* _temp1190; _LL1172: if(*(( int*) _temp1170) == Cyc_Absyn_Array_e){ _LL1181:
_temp1180=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1170)->f1;
goto _LL1173;} else{ goto _LL1174;} _LL1174: if(*(( int*) _temp1170) == Cyc_Absyn_Comprehension_e){
_LL1187: _temp1186=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1170)->f1; goto _LL1185; _LL1185: _temp1184=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1170)->f2; goto _LL1183; _LL1183:
_temp1182=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1170)->f3; goto _LL1175;} else{ goto _LL1176;} _LL1176: if(*(( int*)
_temp1170) == Cyc_Absyn_AnonStruct_e){ _LL1191: _temp1190=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1170)->f1; goto _LL1189; _LL1189: _temp1188=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1170)->f2; goto _LL1177;} else{ goto
_LL1178;} _LL1178: goto _LL1179; _LL1173: body= Cyc_Toc_init_array( nv2, lval,
_temp1180, Cyc_Absyn_skip_stmt( 0)); goto _LL1171; _LL1175: body= Cyc_Toc_init_comprehension(
nv2, lval, _temp1186, _temp1184, _temp1182, Cyc_Absyn_skip_stmt( 0), 0); goto
_LL1171; _LL1177: body= Cyc_Toc_init_anon_struct( nv, lval, _temp1190, _temp1188,
Cyc_Absyn_skip_stmt( 0)); goto _LL1171; _LL1179: Cyc_Toc_exp_to_c( nv2, e2);
body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL1171; _LL1171:;}{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt(
max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp1168, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct
Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
lhs, void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp1194=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); goto _LL1195; _LL1195: for( 0; _temp1194 != 0;
_temp1194=({ struct Cyc_List_List* _temp1196= _temp1194; if( _temp1196 == 0){
_throw( Null_Exception);} _temp1196->tl;})){ struct _tuple4 _temp1200; struct
Cyc_Absyn_Exp* _temp1201; struct Cyc_List_List* _temp1203; struct _tuple4*
_temp1198=( struct _tuple4*)({ struct Cyc_List_List* _temp1197= _temp1194; if(
_temp1197 == 0){ _throw( Null_Exception);} _temp1197->hd;}); _temp1200=*
_temp1198; _LL1204: _temp1203= _temp1200.f1; goto _LL1202; _LL1202: _temp1201=
_temp1200.f2; goto _LL1199; _LL1199: if( _temp1203 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1205=(
char*)"empty designator list"; struct _tagged_string _temp1206; _temp1206.curr=
_temp1205; _temp1206.base= _temp1205; _temp1206.last_plus_one= _temp1205 + 22;
_temp1206;}));} if(({ struct Cyc_List_List* _temp1207= _temp1203; if( _temp1207
== 0){ _throw( Null_Exception);} _temp1207->tl;}) != 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1208=(
char*)"too many designators in anonymous struct"; struct _tagged_string
_temp1209; _temp1209.curr= _temp1208; _temp1209.base= _temp1208; _temp1209.last_plus_one=
_temp1208 + 41; _temp1209;}));}{ void* _temp1211=( void*)({ struct Cyc_List_List*
_temp1210= _temp1203; if( _temp1210 == 0){ _throw( Null_Exception);} _temp1210->hd;});
struct _tagged_string* _temp1217; _LL1213: if(*(( int*) _temp1211) == Cyc_Absyn_FieldName){
_LL1218: _temp1217=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1211)->f1; goto _LL1214;} else{ goto _LL1215;} _LL1215: goto _LL1216;
_LL1214: { struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs,
_temp1217, 0);{ void* _temp1219=( void*) _temp1201->r; struct Cyc_List_List*
_temp1229; struct Cyc_Absyn_Exp* _temp1231; struct Cyc_Absyn_Exp* _temp1233;
struct Cyc_Absyn_Vardecl* _temp1235; struct Cyc_List_List* _temp1237; void*
_temp1239; _LL1221: if(*(( int*) _temp1219) == Cyc_Absyn_Array_e){ _LL1230:
_temp1229=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1219)->f1;
goto _LL1222;} else{ goto _LL1223;} _LL1223: if(*(( int*) _temp1219) == Cyc_Absyn_Comprehension_e){
_LL1236: _temp1235=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1219)->f1; goto _LL1234; _LL1234: _temp1233=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1219)->f2; goto _LL1232; _LL1232:
_temp1231=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1219)->f3; goto _LL1224;} else{ goto _LL1225;} _LL1225: if(*(( int*)
_temp1219) == Cyc_Absyn_AnonStruct_e){ _LL1240: _temp1239=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1219)->f1; goto _LL1238; _LL1238: _temp1237=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1219)->f2; goto _LL1226;} else{ goto
_LL1227;} _LL1227: goto _LL1228; _LL1222: s= Cyc_Toc_init_array( nv, lval,
_temp1229, s); goto _LL1220; _LL1224: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1235, _temp1233, _temp1231, s, 0); goto _LL1220; _LL1226: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp1239, _temp1237, s); goto _LL1220; _LL1228: Cyc_Toc_exp_to_c( nv,
_temp1201); if( Cyc_Toc_is_poly_field( struct_type, _temp1217)){ _temp1201= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1201, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp1201, 0), 0), s, 0); goto _LL1220; _LL1220:;}
goto _LL1212;} _LL1216:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1241=( char*)"array designator in struct";
struct _tagged_string _temp1242; _temp1242.curr= _temp1241; _temp1242.base=
_temp1241; _temp1242.last_plus_one= _temp1241 + 27; _temp1242;}))); _LL1212:;}}}
return s;} static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env*
nv, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
Cyc_List_List* _temp1243=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
es); goto _LL1244; _LL1244: { struct _tagged_string* _temp1245= Cyc_Toc_add_tuple_type(
_temp1243); goto _LL1246; _LL1246: { struct _tuple0* _temp1247= Cyc_Toc_temp_var();
goto _LL1248; _LL1248: { struct Cyc_Absyn_Exp* _temp1249= Cyc_Absyn_var_exp(
_temp1247, 0); goto _LL1250; _LL1250: { struct Cyc_Absyn_Stmt* _temp1251= Cyc_Absyn_exp_stmt(
_temp1249, 0); goto _LL1252; _LL1252: { struct Cyc_Absyn_Exp*(* _temp1253)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1254;
_LL1254: { int is_atomic= 1; es=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( es);{ int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es); for( 0; es != 0; es=({ struct Cyc_List_List* _temp1255= es; if( _temp1255
== 0){ _throw( Null_Exception);} _temp1255->tl;}), -- i){ struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1275= es; if( _temp1275
== 0){ _throw( Null_Exception);} _temp1275->hd;}); struct Cyc_Absyn_Exp* lval=
_temp1253( _temp1249,({ struct _tagged_string* _temp1274=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1274[ 0]= xprintf("f%d", i);
_temp1274;}), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)({ struct Cyc_Core_Opt*
_temp1256= e->topt; if( _temp1256 == 0){ _throw( Null_Exception);} _temp1256->v;})):
0;{ void* _temp1257=( void*) e->r; struct Cyc_List_List* _temp1265; struct Cyc_Absyn_Exp*
_temp1267; struct Cyc_Absyn_Exp* _temp1269; struct Cyc_Absyn_Vardecl* _temp1271;
_LL1259: if(*(( int*) _temp1257) == Cyc_Absyn_Array_e){ _LL1266: _temp1265=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1257)->f1; goto
_LL1260;} else{ goto _LL1261;} _LL1261: if(*(( int*) _temp1257) == Cyc_Absyn_Comprehension_e){
_LL1272: _temp1271=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1257)->f1; goto _LL1270; _LL1270: _temp1269=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1257)->f2; goto _LL1268; _LL1268:
_temp1267=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1257)->f3; goto _LL1262;} else{ goto _LL1263;} _LL1263: goto _LL1264;
_LL1260: _temp1251= Cyc_Toc_init_array( nv, lval, _temp1265, _temp1251); goto
_LL1258; _LL1262: _temp1251= Cyc_Toc_init_comprehension( nv, lval, _temp1271,
_temp1269, _temp1267, _temp1251, 0); goto _LL1258; _LL1264: Cyc_Toc_exp_to_c( nv,
e); _temp1251= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1253( _temp1249,({ struct _tagged_string* _temp1273=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1273[ 0]= xprintf("f%d", i);
_temp1273;}), 0), e, 0), 0), _temp1251, 0); goto _LL1258; _LL1258:;}}} return
Cyc_Toc_make_struct( nv, _temp1247, Cyc_Absyn_strct( _temp1245), _temp1251,
pointer, rgnopt, is_atomic);}}}}}}} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct(
struct Cyc_Toc_Env* nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* dles, struct _tuple0* tdn){ struct _tuple0*
_temp1276= Cyc_Toc_temp_var(); goto _LL1277; _LL1277: { struct Cyc_Absyn_Exp*
_temp1278= Cyc_Absyn_var_exp( _temp1276, 0); goto _LL1279; _LL1279: { struct Cyc_Absyn_Stmt*
_temp1280= Cyc_Absyn_exp_stmt( _temp1278, 0); goto _LL1281; _LL1281: { struct
Cyc_Absyn_Exp*(* _temp1282)( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
goto _LL1283; _LL1283: { int is_atomic= 1;{ struct Cyc_List_List* _temp1284=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto
_LL1285; _LL1285: for( 0; _temp1284 != 0; _temp1284=({ struct Cyc_List_List*
_temp1286= _temp1284; if( _temp1286 == 0){ _throw( Null_Exception);} _temp1286->tl;})){
struct _tuple4 _temp1290; struct Cyc_Absyn_Exp* _temp1291; struct Cyc_List_List*
_temp1293; struct _tuple4* _temp1288=( struct _tuple4*)({ struct Cyc_List_List*
_temp1287= _temp1284; if( _temp1287 == 0){ _throw( Null_Exception);} _temp1287->hd;});
_temp1290=* _temp1288; _LL1294: _temp1293= _temp1290.f1; goto _LL1292; _LL1292:
_temp1291= _temp1290.f2; goto _LL1289; _LL1289: is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)({ struct Cyc_Core_Opt* _temp1295= _temp1291->topt; if( _temp1295 == 0){
_throw( Null_Exception);} _temp1295->v;})): 0; if( _temp1293 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1296=( char*)"empty designator list"; struct _tagged_string _temp1297;
_temp1297.curr= _temp1296; _temp1297.base= _temp1296; _temp1297.last_plus_one=
_temp1296 + 22; _temp1297;}));} if(({ struct Cyc_List_List* _temp1298= _temp1293;
if( _temp1298 == 0){ _throw( Null_Exception);} _temp1298->tl;}) != 0){ struct
_tuple0* _temp1299= Cyc_Toc_temp_var(); goto _LL1300; _LL1300: { struct Cyc_Absyn_Exp*
_temp1301= Cyc_Absyn_var_exp( _temp1299, 0); goto _LL1302; _LL1302: for( 0;
_temp1293 != 0; _temp1293=({ struct Cyc_List_List* _temp1303= _temp1293; if(
_temp1303 == 0){ _throw( Null_Exception);} _temp1303->tl;})){ void* _temp1305=(
void*)({ struct Cyc_List_List* _temp1304= _temp1293; if( _temp1304 == 0){ _throw(
Null_Exception);} _temp1304->hd;}); struct _tagged_string* _temp1311; _LL1307:
if(*(( int*) _temp1305) == Cyc_Absyn_FieldName){ _LL1312: _temp1311=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1305)->f1; goto
_LL1308;} else{ goto _LL1309;} _LL1309: goto _LL1310; _LL1308: if( Cyc_Toc_is_poly_field(
struct_type, _temp1311)){ _temp1301= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1301, 0);} _temp1280= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1282( _temp1278, _temp1311, 0), _temp1301, 0), 0), _temp1280, 0); goto
_LL1306; _LL1310:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1313=( char*)"array designator in struct";
struct _tagged_string _temp1314; _temp1314.curr= _temp1313; _temp1314.base=
_temp1313; _temp1314.last_plus_one= _temp1313 + 27; _temp1314;}))); _LL1306:;}
Cyc_Toc_exp_to_c( nv, _temp1291); _temp1280= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp1301, _temp1291, 0), 0), _temp1280, 0);}} else{ void*
_temp1316=( void*)({ struct Cyc_List_List* _temp1315= _temp1293; if( _temp1315
== 0){ _throw( Null_Exception);} _temp1315->hd;}); struct _tagged_string*
_temp1322; _LL1318: if(*(( int*) _temp1316) == Cyc_Absyn_FieldName){ _LL1323:
_temp1322=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1316)->f1; goto _LL1319;} else{ goto _LL1320;} _LL1320: goto _LL1321;
_LL1319: { struct Cyc_Absyn_Exp* lval= _temp1282( _temp1278, _temp1322, 0);{
void* _temp1324=( void*) _temp1291->r; struct Cyc_List_List* _temp1334; struct
Cyc_Absyn_Exp* _temp1336; struct Cyc_Absyn_Exp* _temp1338; struct Cyc_Absyn_Vardecl*
_temp1340; struct Cyc_List_List* _temp1342; void* _temp1344; _LL1326: if(*(( int*)
_temp1324) == Cyc_Absyn_Array_e){ _LL1335: _temp1334=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1324)->f1; goto _LL1327;} else{ goto
_LL1328;} _LL1328: if(*(( int*) _temp1324) == Cyc_Absyn_Comprehension_e){
_LL1341: _temp1340=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1324)->f1; goto _LL1339; _LL1339: _temp1338=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1324)->f2; goto _LL1337; _LL1337:
_temp1336=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1324)->f3; goto _LL1329;} else{ goto _LL1330;} _LL1330: if(*(( int*)
_temp1324) == Cyc_Absyn_AnonStruct_e){ _LL1345: _temp1344=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1324)->f1; goto _LL1343; _LL1343: _temp1342=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1324)->f2; goto _LL1331;} else{ goto
_LL1332;} _LL1332: goto _LL1333; _LL1327: _temp1280= Cyc_Toc_init_array( nv,
lval, _temp1334, _temp1280); goto _LL1325; _LL1329: _temp1280= Cyc_Toc_init_comprehension(
nv, lval, _temp1340, _temp1338, _temp1336, _temp1280, 0); goto _LL1325; _LL1331:
_temp1280= Cyc_Toc_init_anon_struct( nv, lval, _temp1344, _temp1342, _temp1280);
goto _LL1325; _LL1333: Cyc_Toc_exp_to_c( nv, _temp1291); if( Cyc_Toc_is_poly_field(
struct_type, _temp1322)){ _temp1291= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1291, 0);} _temp1280= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1291, 0), 0), _temp1280, 0); goto _LL1325; _LL1325:;} goto _LL1317;}
_LL1321:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1346=( char*)"array designator in struct";
struct _tagged_string _temp1347; _temp1347.curr= _temp1346; _temp1347.base=
_temp1346; _temp1347.last_plus_one= _temp1346 + 27; _temp1347;}))); _LL1317:;}}}
return Cyc_Toc_make_struct( nv, _temp1276, Cyc_Absyn_strctq( tdn), _temp1280,
pointer, rgnopt, is_atomic);}}}}} struct _tuple11{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp(
el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp1348=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp1348[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp1349; _temp1349.tag=
Cyc_Absyn_Increment_e; _temp1349.f1= e1; _temp1349.f2=( void*) incr; _temp1349;});
_temp1348;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1350=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp1360; struct Cyc_Absyn_Exp* _temp1362; void* _temp1364; struct
_tagged_string* _temp1366; struct Cyc_Absyn_Exp* _temp1368; _LL1352: if(*(( int*)
_temp1350) == Cyc_Absyn_StmtExp_e){ _LL1361: _temp1360=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1350)->f1; goto _LL1353;} else{ goto
_LL1354;} _LL1354: if(*(( int*) _temp1350) == Cyc_Absyn_Cast_e){ _LL1365:
_temp1364=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1350)->f1; goto
_LL1363; _LL1363: _temp1362=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1350)->f2; goto _LL1355;} else{ goto _LL1356;} _LL1356: if(*(( int*)
_temp1350) == Cyc_Absyn_StructMember_e){ _LL1369: _temp1368=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1350)->f1; goto _LL1367; _LL1367:
_temp1366=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1350)->f2; goto _LL1357;} else{ goto _LL1358;} _LL1358: goto _LL1359;
_LL1353:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1360, fs, f, f_env); goto _LL1351; _LL1355:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1362, fs, f, f_env); goto
_LL1351; _LL1357:( void*)( e1->r=( void*)(( void*) _temp1368->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1370=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1370->hd=( void*) _temp1366; _temp1370->tl= fs;
_temp1370;}), f, f_env); goto _LL1351; _LL1359: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1371= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1371.last_plus_one - _temp1371.curr, _temp1371.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp1372= fs; if( _temp1372 == 0){ _throw( Null_Exception);} _temp1372->tl;})){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_string*)({ struct
Cyc_List_List* _temp1373= fs; if( _temp1373 == 0){ _throw( Null_Exception);}
_temp1373->hd;}), e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy,
f_env))->r)); goto _LL1351;} _LL1351:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp1374=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1384; struct Cyc_Absyn_Stmt* _temp1386; struct Cyc_Absyn_Decl*
_temp1388; struct Cyc_Absyn_Stmt* _temp1390; struct Cyc_Absyn_Stmt* _temp1392;
_LL1376: if(( unsigned int) _temp1374 > 1u?*(( int*) _temp1374) == Cyc_Absyn_Exp_s:
0){ _LL1385: _temp1384=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1374)->f1; goto _LL1377;} else{ goto _LL1378;} _LL1378: if(( unsigned int)
_temp1374 > 1u?*(( int*) _temp1374) == Cyc_Absyn_Decl_s: 0){ _LL1389: _temp1388=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1374)->f1; goto
_LL1387; _LL1387: _temp1386=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1374)->f2; goto _LL1379;} else{ goto _LL1380;} _LL1380: if(( unsigned int)
_temp1374 > 1u?*(( int*) _temp1374) == Cyc_Absyn_Seq_s: 0){ _LL1393: _temp1392=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1374)->f1; goto
_LL1391; _LL1391: _temp1390=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1374)->f2; goto _LL1381;} else{ goto _LL1382;} _LL1382: goto _LL1383;
_LL1377:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1384, fs, f, f_env); goto _LL1375; _LL1379:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1386, fs, f, f_env);
goto _LL1375; _LL1381:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1390, fs, f, f_env); goto _LL1375; _LL1383:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1394= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1394.last_plus_one - _temp1394.curr,
_temp1394.curr);})); goto _LL1375; _LL1375:;} static struct Cyc_Absyn_Exp* Cyc_Toc_known_size_subscript(
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
nv, struct Cyc_Absyn_Exp* e){ void* _temp1395=( void*) e->r; goto _LL1396;
_LL1396: if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1397= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp1397.last_plus_one - _temp1397.curr, _temp1397.curr);})); return;}{ void*
old_typ=( void*)({ struct Cyc_Core_Opt* _temp2247= e->topt; if( _temp2247 == 0){
_throw( Null_Exception);} _temp2247->v;}); void* _temp1398= _temp1395; void*
_temp1474; void* _temp1476; void* _temp1478; struct _tuple0* _temp1480; struct
_tuple0* _temp1482; struct Cyc_List_List* _temp1484; void* _temp1486; void*
_temp1488; struct Cyc_Absyn_Exp* _temp1490; struct Cyc_Absyn_Exp* _temp1492;
struct Cyc_Core_Opt* _temp1494; struct Cyc_Absyn_Exp* _temp1496; struct Cyc_Absyn_Exp*
_temp1498; struct Cyc_Absyn_Exp* _temp1500; struct Cyc_Absyn_Exp* _temp1502;
struct Cyc_Absyn_Exp* _temp1504; struct Cyc_Absyn_Exp* _temp1506; struct Cyc_List_List*
_temp1508; struct Cyc_Absyn_Exp* _temp1510; struct Cyc_List_List* _temp1512;
struct Cyc_Absyn_Exp* _temp1514; struct Cyc_Absyn_Exp* _temp1516; struct Cyc_Absyn_Exp*
_temp1518; struct Cyc_List_List* _temp1520; struct Cyc_Absyn_Exp* _temp1522;
struct Cyc_Absyn_Exp* _temp1524; struct Cyc_Absyn_Exp** _temp1526; void*
_temp1527; void** _temp1529; struct Cyc_Absyn_Exp* _temp1530; struct Cyc_Absyn_Exp*
_temp1532; struct Cyc_Absyn_Exp* _temp1534; void* _temp1536; struct Cyc_Absyn_Exp*
_temp1538; struct Cyc_Absyn_Exp* _temp1540; struct _tagged_string* _temp1542;
struct Cyc_Absyn_Exp* _temp1544; struct _tagged_string* _temp1546; struct Cyc_Absyn_Exp*
_temp1548; struct Cyc_Absyn_Exp* _temp1550; struct Cyc_Absyn_Exp* _temp1552;
struct Cyc_List_List* _temp1554; struct Cyc_List_List* _temp1556; struct _tuple1*
_temp1558; struct Cyc_List_List* _temp1560; struct Cyc_Absyn_Exp* _temp1562;
struct Cyc_Absyn_Exp* _temp1564; struct Cyc_Absyn_Vardecl* _temp1566; struct Cyc_Absyn_Structdecl*
_temp1568; struct Cyc_List_List* _temp1570; struct Cyc_Core_Opt* _temp1572;
struct _tuple0* _temp1574; struct Cyc_List_List* _temp1576; void* _temp1578;
struct Cyc_Absyn_Tunionfield* _temp1580; struct Cyc_Absyn_Tuniondecl* _temp1582;
struct Cyc_List_List* _temp1584; struct Cyc_Core_Opt* _temp1586; struct Cyc_Core_Opt*
_temp1588; struct Cyc_Absyn_Tunionfield* _temp1590; struct Cyc_Absyn_Tuniondecl*
_temp1592; struct Cyc_List_List* _temp1594; struct Cyc_Core_Opt* _temp1596;
struct Cyc_Core_Opt* _temp1598; struct Cyc_Absyn_Enumfield* _temp1600; struct
Cyc_Absyn_Enumdecl* _temp1602; struct _tuple0* _temp1604; void* _temp1606;
struct Cyc_Absyn_Exp* _temp1608; struct Cyc_Absyn_Stmt* _temp1610; struct Cyc_List_List*
_temp1612; struct Cyc_Core_Opt* _temp1614; struct Cyc_Absyn_Fndecl* _temp1616;
struct Cyc_Absyn_Exp* _temp1618; _LL1400: if(*(( int*) _temp1398) == Cyc_Absyn_Const_e){
_LL1475: _temp1474=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1398)->f1;
if( _temp1474 ==( void*) Cyc_Absyn_Null_c){ goto _LL1401;} else{ goto _LL1402;}}
else{ goto _LL1402;} _LL1402: if(*(( int*) _temp1398) == Cyc_Absyn_Const_e){
_LL1477: _temp1476=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1398)->f1;
goto _LL1403;} else{ goto _LL1404;} _LL1404: if(*(( int*) _temp1398) == Cyc_Absyn_Var_e){
_LL1481: _temp1480=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1398)->f1; goto _LL1479; _LL1479: _temp1478=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1398)->f2; goto _LL1405;} else{ goto _LL1406;} _LL1406: if(*(( int*)
_temp1398) == Cyc_Absyn_UnknownId_e){ _LL1483: _temp1482=( struct _tuple0*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1398)->f1; goto _LL1407;} else{ goto
_LL1408;} _LL1408: if(*(( int*) _temp1398) == Cyc_Absyn_Primop_e){ _LL1487:
_temp1486=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1398)->f1; goto
_LL1485; _LL1485: _temp1484=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1398)->f2; goto _LL1409;} else{ goto _LL1410;} _LL1410: if(*(( int*)
_temp1398) == Cyc_Absyn_Increment_e){ _LL1491: _temp1490=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp1398)->f1; goto _LL1489; _LL1489:
_temp1488=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1398)->f2; goto
_LL1411;} else{ goto _LL1412;} _LL1412: if(*(( int*) _temp1398) == Cyc_Absyn_AssignOp_e){
_LL1497: _temp1496=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1398)->f1; goto _LL1495; _LL1495: _temp1494=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1398)->f2; goto _LL1493; _LL1493:
_temp1492=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1398)->f3; goto _LL1413;} else{ goto _LL1414;} _LL1414: if(*(( int*)
_temp1398) == Cyc_Absyn_Conditional_e){ _LL1503: _temp1502=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1398)->f1; goto _LL1501; _LL1501:
_temp1500=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1398)->f2; goto _LL1499; _LL1499: _temp1498=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1398)->f3; goto _LL1415;} else{
goto _LL1416;} _LL1416: if(*(( int*) _temp1398) == Cyc_Absyn_SeqExp_e){ _LL1507:
_temp1506=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1398)->f1; goto _LL1505; _LL1505: _temp1504=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1398)->f2; goto _LL1417;} else{ goto
_LL1418;} _LL1418: if(*(( int*) _temp1398) == Cyc_Absyn_UnknownCall_e){ _LL1511:
_temp1510=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1398)->f1; goto _LL1509; _LL1509: _temp1508=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1398)->f2; goto _LL1419;} else{
goto _LL1420;} _LL1420: if(*(( int*) _temp1398) == Cyc_Absyn_FnCall_e){ _LL1515:
_temp1514=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1398)->f1; goto _LL1513; _LL1513: _temp1512=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1398)->f2; goto _LL1421;} else{ goto
_LL1422;} _LL1422: if(*(( int*) _temp1398) == Cyc_Absyn_Throw_e){ _LL1517:
_temp1516=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1398)->f1;
goto _LL1423;} else{ goto _LL1424;} _LL1424: if(*(( int*) _temp1398) == Cyc_Absyn_NoInstantiate_e){
_LL1519: _temp1518=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1398)->f1; goto _LL1425;} else{ goto _LL1426;} _LL1426: if(*(( int*)
_temp1398) == Cyc_Absyn_Instantiate_e){ _LL1523: _temp1522=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1398)->f1; goto _LL1521; _LL1521:
_temp1520=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1398)->f2; goto _LL1427;} else{ goto _LL1428;} _LL1428: if(*(( int*)
_temp1398) == Cyc_Absyn_Cast_e){ _LL1528: _temp1527=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1398)->f1; _temp1529=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1398)->f1;
goto _LL1525; _LL1525: _temp1524=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1398)->f2; _temp1526=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1398)->f2;
goto _LL1429;} else{ goto _LL1430;} _LL1430: if(*(( int*) _temp1398) == Cyc_Absyn_Address_e){
_LL1531: _temp1530=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp1398)->f1; goto _LL1431;} else{ goto _LL1432;} _LL1432: if(*(( int*)
_temp1398) == Cyc_Absyn_New_e){ _LL1535: _temp1534=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1398)->f1; goto _LL1533; _LL1533: _temp1532=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1398)->f2; goto
_LL1433;} else{ goto _LL1434;} _LL1434: if(*(( int*) _temp1398) == Cyc_Absyn_Sizeoftyp_e){
_LL1537: _temp1536=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1398)->f1;
goto _LL1435;} else{ goto _LL1436;} _LL1436: if(*(( int*) _temp1398) == Cyc_Absyn_Sizeofexp_e){
_LL1539: _temp1538=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1398)->f1; goto _LL1437;} else{ goto _LL1438;} _LL1438: if(*(( int*)
_temp1398) == Cyc_Absyn_Deref_e){ _LL1541: _temp1540=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp1398)->f1; goto _LL1439;} else{ goto
_LL1440;} _LL1440: if(*(( int*) _temp1398) == Cyc_Absyn_StructMember_e){ _LL1545:
_temp1544=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1398)->f1; goto _LL1543; _LL1543: _temp1542=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1398)->f2; goto _LL1441;} else{
goto _LL1442;} _LL1442: if(*(( int*) _temp1398) == Cyc_Absyn_StructArrow_e){
_LL1549: _temp1548=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1398)->f1; goto _LL1547; _LL1547: _temp1546=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1398)->f2; goto _LL1443;} else{
goto _LL1444;} _LL1444: if(*(( int*) _temp1398) == Cyc_Absyn_Subscript_e){
_LL1553: _temp1552=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1398)->f1; goto _LL1551; _LL1551: _temp1550=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1398)->f2; goto _LL1445;} else{ goto
_LL1446;} _LL1446: if(*(( int*) _temp1398) == Cyc_Absyn_Tuple_e){ _LL1555:
_temp1554=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1398)->f1;
goto _LL1447;} else{ goto _LL1448;} _LL1448: if(*(( int*) _temp1398) == Cyc_Absyn_CompoundLit_e){
_LL1559: _temp1558=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1398)->f1; goto _LL1557; _LL1557: _temp1556=( struct Cyc_List_List*)((
struct Cyc_Absyn_CompoundLit_e_struct*) _temp1398)->f2; goto _LL1449;} else{
goto _LL1450;} _LL1450: if(*(( int*) _temp1398) == Cyc_Absyn_Array_e){ _LL1561:
_temp1560=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1398)->f1;
goto _LL1451;} else{ goto _LL1452;} _LL1452: if(*(( int*) _temp1398) == Cyc_Absyn_Comprehension_e){
_LL1567: _temp1566=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1398)->f1; goto _LL1565; _LL1565: _temp1564=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1398)->f2; goto _LL1563; _LL1563:
_temp1562=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1398)->f3; goto _LL1453;} else{ goto _LL1454;} _LL1454: if(*(( int*)
_temp1398) == Cyc_Absyn_Struct_e){ _LL1575: _temp1574=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1398)->f1; goto _LL1573; _LL1573:
_temp1572=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1398)->f2;
goto _LL1571; _LL1571: _temp1570=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1398)->f3; goto _LL1569; _LL1569: _temp1568=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1398)->f4; goto _LL1455;} else{ goto
_LL1456;} _LL1456: if(*(( int*) _temp1398) == Cyc_Absyn_AnonStruct_e){ _LL1579:
_temp1578=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1398)->f1; goto
_LL1577; _LL1577: _temp1576=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1398)->f2; goto _LL1457;} else{ goto _LL1458;} _LL1458: if(*(( int*)
_temp1398) == Cyc_Absyn_Tunion_e){ _LL1589: _temp1588=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1398)->f1; goto _LL1587; _LL1587:
_temp1586=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1398)->f2;
goto _LL1585; _LL1585: _temp1584=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1398)->f3; if( _temp1584 == 0){ goto _LL1583;} else{ goto _LL1460;} _LL1583:
_temp1582=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1398)->f4; goto _LL1581; _LL1581: _temp1580=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1398)->f5; goto _LL1459;} else{ goto
_LL1460;} _LL1460: if(*(( int*) _temp1398) == Cyc_Absyn_Tunion_e){ _LL1599:
_temp1598=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1398)->f1;
goto _LL1597; _LL1597: _temp1596=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1398)->f2; goto _LL1595; _LL1595: _temp1594=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1398)->f3; goto _LL1593; _LL1593:
_temp1592=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1398)->f4; goto _LL1591; _LL1591: _temp1590=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1398)->f5; goto _LL1461;} else{ goto
_LL1462;} _LL1462: if(*(( int*) _temp1398) == Cyc_Absyn_Enum_e){ _LL1605:
_temp1604=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1398)->f1;
goto _LL1603; _LL1603: _temp1602=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1398)->f2; goto _LL1601; _LL1601: _temp1600=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1398)->f3; goto _LL1463;} else{ goto
_LL1464;} _LL1464: if(*(( int*) _temp1398) == Cyc_Absyn_Malloc_e){ _LL1609:
_temp1608=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1398)->f1; goto _LL1607; _LL1607: _temp1606=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1398)->f2; goto _LL1465;} else{ goto _LL1466;} _LL1466: if(*(( int*)
_temp1398) == Cyc_Absyn_StmtExp_e){ _LL1611: _temp1610=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1398)->f1; goto _LL1467;} else{ goto
_LL1468;} _LL1468: if(*(( int*) _temp1398) == Cyc_Absyn_UnresolvedMem_e){
_LL1615: _temp1614=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1398)->f1; goto _LL1613; _LL1613: _temp1612=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1398)->f2; goto _LL1469;} else{
goto _LL1470;} _LL1470: if(*(( int*) _temp1398) == Cyc_Absyn_Codegen_e){ _LL1617:
_temp1616=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1398)->f1; goto _LL1471;} else{ goto _LL1472;} _LL1472: if(*(( int*)
_temp1398) == Cyc_Absyn_Fill_e){ _LL1619: _temp1618=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp1398)->f1; goto _LL1473;} else{ goto
_LL1399;} _LL1401:{ void* _temp1620= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1626; struct Cyc_Absyn_Conref* _temp1628; struct Cyc_Absyn_Tqual _temp1630;
struct Cyc_Absyn_Conref* _temp1632; void* _temp1634; void* _temp1636; _LL1622:
if(( unsigned int) _temp1620 > 4u?*(( int*) _temp1620) == Cyc_Absyn_PointerType:
0){ _LL1627: _temp1626=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1620)->f1; _LL1637: _temp1636=( void*) _temp1626.elt_typ; goto _LL1635;
_LL1635: _temp1634=( void*) _temp1626.rgn_typ; goto _LL1633; _LL1633: _temp1632=(
struct Cyc_Absyn_Conref*) _temp1626.nullable; goto _LL1631; _LL1631: _temp1630=(
struct Cyc_Absyn_Tqual) _temp1626.tq; goto _LL1629; _LL1629: _temp1628=( struct
Cyc_Absyn_Conref*) _temp1626.bounds; goto _LL1623;} else{ goto _LL1624;} _LL1624:
goto _LL1625; _LL1623: if(( void*) _temp1628->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1621;}{ void* _temp1638=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1628); struct Cyc_Absyn_Exp* _temp1644; _LL1640: if( _temp1638 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1641;} else{ goto _LL1642;} _LL1642: if((
unsigned int) _temp1638 > 1u?*(( int*) _temp1638) == Cyc_Absyn_Upper_b: 0){
_LL1645: _temp1644=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1638)->f1; goto _LL1643;} else{ goto _LL1639;} _LL1641: { struct _tuple4*
_temp1647=({ struct _tuple4* _temp1646=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1646->f1= 0; _temp1646->f2= Cyc_Absyn_signed_int_exp( 0,
0); _temp1646;}); goto _LL1648; _LL1648: { struct Cyc_Absyn_Exp* _temp1652= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp1649=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1649->hd=( void*) _temp1647; _temp1649->tl=({
struct Cyc_List_List* _temp1650=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1650->hd=( void*) _temp1647; _temp1650->tl=({
struct Cyc_List_List* _temp1651=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1651->hd=( void*) _temp1647; _temp1651->tl= 0;
_temp1651;}); _temp1650;}); _temp1649;}), 0); goto _LL1653; _LL1653: if( nv->toplevel){(
void*)( e->r=( void*)(( void*) _temp1652->r));} else{ void* _temp1654= Cyc_Absyn_strct(
Cyc_Toc_add_tagged_ptr_type( Cyc_Toc_typ_to_c( _temp1636), Cyc_Toc_mt_tq)); goto
_LL1655; _LL1655: { struct _tuple0* _temp1656= Cyc_Toc_temp_var(); goto _LL1657;
_LL1657: { struct Cyc_Absyn_Exp* _temp1658= Cyc_Absyn_var_exp( _temp1656, 0);
goto _LL1659; _LL1659: { struct Cyc_Absyn_Stmt* _temp1660= Cyc_Absyn_declare_stmt(
_temp1656, _temp1654,( struct Cyc_Absyn_Exp*) _temp1652, Cyc_Absyn_exp_stmt(
_temp1658, 0), 0); goto _LL1661; _LL1661:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1660, 0))->r));}}}} goto _LL1639;}} _LL1643:( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto _LL1639; _LL1639:;} goto _LL1621;
_LL1625:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1662= Cyc_Absynpp_typ2string( old_typ); xprintf("exp_to_c: null given type %.*s",
_temp1662.last_plus_one - _temp1662.curr, _temp1662.curr);})); goto _LL1621;
_LL1621:;} goto _LL1399; _LL1403: goto _LL1399; _LL1405:{ struct _handler_cons
_temp1663; _push_handler(& _temp1663);{ int _temp1665= 0; if( setjmp( _temp1663.handler)){
_temp1665= 1;} if( ! _temp1665){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1480))->r));; _pop_handler();} else{ void* _temp1664=( void*) _exn_thrown;
void* _temp1667= _temp1664; _LL1669: if( _temp1667 == Cyc_Dict_Absent){ goto
_LL1670;} else{ goto _LL1671;} _LL1671: goto _LL1672; _LL1670:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1673= Cyc_Absynpp_qvar2string(
_temp1480); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1673.last_plus_one
- _temp1673.curr, _temp1673.curr);})); return; _LL1672:( void) _throw( _temp1667);
_LL1668:;}}} goto _LL1399; _LL1407:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1674=( char*)"unknownid"; struct
_tagged_string _temp1675; _temp1675.curr= _temp1674; _temp1675.base= _temp1674;
_temp1675.last_plus_one= _temp1674 + 10; _temp1675;})); goto _LL1399; _LL1409: {
struct Cyc_List_List* _temp1676=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1484); goto _LL1677; _LL1677: { struct Cyc_List_List* _temp1678=(( struct
Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Toc_get_c_typ, _temp1484); goto _LL1679; _LL1679:(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1484);{
void* _temp1680= _temp1486; _LL1682: if( _temp1680 ==( void*) Cyc_Absyn_Size){
goto _LL1683;} else{ goto _LL1684;} _LL1684: if( _temp1680 ==( void*) Cyc_Absyn_Printf){
goto _LL1685;} else{ goto _LL1686;} _LL1686: if( _temp1680 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1687;} else{ goto _LL1688;} _LL1688: if( _temp1680 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1689;} else{ goto _LL1690;} _LL1690: if( _temp1680 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1691;} else{ goto _LL1692;} _LL1692: if( _temp1680 ==( void*) Cyc_Absyn_Plus){
goto _LL1693;} else{ goto _LL1694;} _LL1694: if( _temp1680 ==( void*) Cyc_Absyn_Minus){
goto _LL1695;} else{ goto _LL1696;} _LL1696: if( _temp1680 ==( void*) Cyc_Absyn_Eq){
goto _LL1697;} else{ goto _LL1698;} _LL1698: if( _temp1680 ==( void*) Cyc_Absyn_Neq){
goto _LL1699;} else{ goto _LL1700;} _LL1700: if( _temp1680 ==( void*) Cyc_Absyn_Gt){
goto _LL1701;} else{ goto _LL1702;} _LL1702: if( _temp1680 ==( void*) Cyc_Absyn_Gte){
goto _LL1703;} else{ goto _LL1704;} _LL1704: if( _temp1680 ==( void*) Cyc_Absyn_Lt){
goto _LL1705;} else{ goto _LL1706;} _LL1706: if( _temp1680 ==( void*) Cyc_Absyn_Lte){
goto _LL1707;} else{ goto _LL1708;} _LL1708: goto _LL1709; _LL1683: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1747= _temp1484; if(
_temp1747 == 0){ _throw( Null_Exception);} _temp1747->hd;});{ void* _temp1711=(
void*)({ struct Cyc_Core_Opt* _temp1710= arg->topt; if( _temp1710 == 0){ _throw(
Null_Exception);} _temp1710->v;}); struct Cyc_Absyn_Exp* _temp1719; struct Cyc_Absyn_Tqual
_temp1721; void* _temp1723; struct Cyc_Absyn_PtrInfo _temp1725; struct Cyc_Absyn_Conref*
_temp1727; struct Cyc_Absyn_Tqual _temp1729; struct Cyc_Absyn_Conref* _temp1731;
void* _temp1733; void* _temp1735; _LL1713: if(( unsigned int) _temp1711 > 4u?*((
int*) _temp1711) == Cyc_Absyn_ArrayType: 0){ _LL1724: _temp1723=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1711)->f1; goto _LL1722; _LL1722:
_temp1721=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1711)->f2; goto _LL1720; _LL1720: _temp1719=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1711)->f3; goto _LL1714;} else{ goto
_LL1715;} _LL1715: if(( unsigned int) _temp1711 > 4u?*(( int*) _temp1711) == Cyc_Absyn_PointerType:
0){ _LL1726: _temp1725=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1711)->f1; _LL1736: _temp1735=( void*) _temp1725.elt_typ; goto _LL1734;
_LL1734: _temp1733=( void*) _temp1725.rgn_typ; goto _LL1732; _LL1732: _temp1731=(
struct Cyc_Absyn_Conref*) _temp1725.nullable; goto _LL1730; _LL1730: _temp1729=(
struct Cyc_Absyn_Tqual) _temp1725.tq; goto _LL1728; _LL1728: _temp1727=( struct
Cyc_Absyn_Conref*) _temp1725.bounds; goto _LL1716;} else{ goto _LL1717;} _LL1717:
goto _LL1718; _LL1714:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Exp*
_temp1737= _temp1719; if( _temp1737 == 0){ _throw( Null_Exception);} _temp1737->r;})));
goto _LL1712; _LL1716: { struct _tuple0* _temp1738= Cyc_Toc_temp_var(); goto
_LL1739; _LL1739: { void* _temp1741= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1740= arg->topt; if( _temp1740 == 0){ _throw( Null_Exception);} _temp1740->v;}));
goto _LL1742; _LL1742: { struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1738, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( _temp1738, 0), Cyc_Toc_curr_sp,
0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp(
e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1738,
_temp1741,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1712;}}}
_LL1718:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1745= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_List_List*
_temp1743= _temp1678; if( _temp1743 == 0){ _throw( Null_Exception);} _temp1743->hd;}));
struct _tagged_string _temp1746= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1744= arg->topt; if( _temp1744 == 0){ _throw( Null_Exception);} _temp1744->v;}));
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1745.last_plus_one
- _temp1745.curr, _temp1745.curr, _temp1746.last_plus_one - _temp1746.curr,
_temp1746.curr);})); return; _LL1712:;} goto _LL1681;} _LL1685:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1486, 0, _temp1484, _temp1678))->r));
goto _LL1681; _LL1687:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1486, 0, _temp1484, _temp1678))->r)); goto _LL1681; _LL1689: if( _temp1484
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1748=( char*)"fprintf without arguments"; struct
_tagged_string _temp1749; _temp1749.curr= _temp1748; _temp1749.base= _temp1748;
_temp1749.last_plus_one= _temp1748 + 26; _temp1749;}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_printf_to_c( _temp1486,({ struct Cyc_Core_Opt* _temp1750=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1750->v=(
void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1751= _temp1484;
if( _temp1751 == 0){ _throw( Null_Exception);} _temp1751->hd;})); _temp1750;}),({
struct Cyc_List_List* _temp1752= _temp1484; if( _temp1752 == 0){ _throw(
Null_Exception);} _temp1752->tl;}),({ struct Cyc_List_List* _temp1753= _temp1678;
if( _temp1753 == 0){ _throw( Null_Exception);} _temp1753->tl;})))->r)); goto
_LL1681; _LL1691: if( _temp1484 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1754=( char*)"sscanf without arguments";
struct _tagged_string _temp1755; _temp1755.curr= _temp1754; _temp1755.base=
_temp1754; _temp1755.last_plus_one= _temp1754 + 25; _temp1755;}));}( void*)(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1756= _temp1484; if(
_temp1756 == 0){ _throw( Null_Exception);} _temp1756->hd;}))->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1757= _temp1484; if( _temp1757 == 0){ _throw(
Null_Exception);} _temp1757->hd;}))->r,(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1758= _temp1484; if( _temp1758 == 0){ _throw( Null_Exception);} _temp1758->hd;}))->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1681; _LL1693: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1772= _temp1484; if( _temp1772 == 0){ _throw( Null_Exception);} _temp1772->hd;});
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1771=({ struct Cyc_List_List* _temp1770= _temp1484; if( _temp1770 == 0){
_throw( Null_Exception);} _temp1770->tl;}); if( _temp1771 == 0){ _throw(
Null_Exception);} _temp1771->hd;}); struct _tuple0* _temp1759= Cyc_Toc_temp_var();
goto _LL1760; _LL1760: { void* _temp1761= Cyc_Toc_typ_to_c( old_typ); goto
_LL1762; _LL1762: { struct Cyc_Absyn_Stmt* _temp1763= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1759, 0), 0); goto _LL1764; _LL1764: { struct Cyc_Absyn_Exp* _temp1765= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1759, 0), Cyc_Toc_curr_sp, 0); goto _LL1766; _LL1766: {
struct Cyc_Absyn_Exp* _temp1768= Cyc_Absyn_assignop_exp( _temp1765,({ struct Cyc_Core_Opt*
_temp1767=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1767->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1767;}), e2, 0); goto
_LL1769; _LL1769: _temp1763= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1768,
0), _temp1763, 0); _temp1763= Cyc_Absyn_declare_stmt( _temp1759, _temp1761,(
struct Cyc_Absyn_Exp*) e1, _temp1763, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1763, 0))->r));}}}}} goto _LL1681; _LL1695: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)({ struct Cyc_List_List* _temp1773= _temp1676; if( _temp1773 == 0){ _throw(
Null_Exception);} _temp1773->hd;}))){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1790= _temp1484; if( _temp1790 == 0){ _throw(
Null_Exception);} _temp1790->hd;}); struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1789=({ struct Cyc_List_List* _temp1788= _temp1484;
if( _temp1788 == 0){ _throw( Null_Exception);} _temp1788->tl;}); if( _temp1789
== 0){ _throw( Null_Exception);} _temp1789->hd;}); void* old_typ_e2=( void*)({
struct Cyc_List_List* _temp1787=({ struct Cyc_List_List* _temp1786= _temp1676;
if( _temp1786 == 0){ _throw( Null_Exception);} _temp1786->tl;}); if( _temp1787
== 0){ _throw( Null_Exception);} _temp1787->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* _temp1774= Cyc_Toc_temp_var(); goto _LL1775;
_LL1775: { void* _temp1777= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_List_List*
_temp1776= _temp1676; if( _temp1776 == 0){ _throw( Null_Exception);} _temp1776->hd;}));
goto _LL1778; _LL1778: { struct Cyc_Absyn_Stmt* _temp1779= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( _temp1774, 0), 0); goto _LL1780; _LL1780: { struct Cyc_Absyn_Exp*
_temp1781= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( _temp1774, 0), Cyc_Toc_curr_sp,
0); goto _LL1782; _LL1782: { struct Cyc_Absyn_Exp* _temp1784= Cyc_Absyn_assignop_exp(
_temp1781,({ struct Cyc_Core_Opt* _temp1783=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1783->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp1783;}), e2, 0); goto _LL1785; _LL1785: _temp1779= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1784, 0), _temp1779, 0); _temp1779= Cyc_Absyn_declare_stmt( _temp1774,
_temp1777,( struct Cyc_Absyn_Exp*) e1, _temp1779, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( _temp1779, 0))->r));}}}}}} goto _LL1681; _LL1697:
goto _LL1699; _LL1699: goto _LL1701; _LL1701: goto _LL1703; _LL1703: goto
_LL1705; _LL1705: goto _LL1707; _LL1707: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1796= _temp1484; if( _temp1796 == 0){ _throw(
Null_Exception);} _temp1796->hd;}); struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1795=({ struct Cyc_List_List* _temp1794= _temp1484;
if( _temp1794 == 0){ _throw( Null_Exception);} _temp1794->tl;}); if( _temp1795
== 0){ _throw( Null_Exception);} _temp1795->hd;}); void* t1=( void*)({ struct
Cyc_List_List* _temp1793= _temp1676; if( _temp1793 == 0){ _throw( Null_Exception);}
_temp1793->hd;}); void* t2=( void*)({ struct Cyc_List_List* _temp1792=({ struct
Cyc_List_List* _temp1791= _temp1676; if( _temp1791 == 0){ _throw( Null_Exception);}
_temp1791->tl;}); if( _temp1792 == 0){ _throw( Null_Exception);} _temp1792->hd;});
if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){( void*)( e1->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp,
0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ( t2)){( void*)( e2->r=( void*)((
void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} goto _LL1681;} _LL1709: goto _LL1681; _LL1681:;} goto _LL1399;}}
_LL1411: { void* e2_cyc_typ=( void*)({ struct Cyc_Core_Opt* _temp1817= _temp1490->topt;
if( _temp1817 == 0){ _throw( Null_Exception);} _temp1817->v;}); Cyc_Toc_exp_to_c(
nv, _temp1490); if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if(
_temp1488 ==( void*) Cyc_Absyn_PreInc? 1: _temp1488 ==( void*) Cyc_Absyn_PreDec){
struct _tuple0* _temp1797= Cyc_Toc_temp_var(); goto _LL1798; _LL1798: { void*
_temp1799= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1800; _LL1800: { struct Cyc_Absyn_Stmt*
_temp1801= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp1797,
0), 0), 0); goto _LL1802; _LL1802: { struct Cyc_Absyn_Exp* _temp1803= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1797, 0), Cyc_Toc_curr_sp, 0),
_temp1488, 0); goto _LL1804; _LL1804: _temp1801= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1803, 0), _temp1801, 0); _temp1801= Cyc_Absyn_declare_stmt( _temp1797,
_temp1799,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1490, 0),
_temp1801, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1801, 0))->r));}}}}
else{ struct _tuple0* _temp1805= Cyc_Toc_temp_var(); goto _LL1806; _LL1806: {
struct _tuple0* _temp1807= Cyc_Toc_temp_var(); goto _LL1808; _LL1808: { void*
_temp1809= Cyc_Toc_typ_to_c( e2_cyc_typ); goto _LL1810; _LL1810: { void*
_temp1811= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1812; _LL1812: { struct Cyc_Absyn_Stmt*
_temp1813= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1807, 0), 0); goto
_LL1814; _LL1814: { struct Cyc_Absyn_Exp* _temp1815= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1805, 0), Cyc_Toc_curr_sp, 0),
_temp1488, 0); goto _LL1816; _LL1816: _temp1813= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1815, 0), _temp1813, 0); _temp1813= Cyc_Absyn_declare_stmt( _temp1807,
_temp1809,( struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp1805, 0), 0), _temp1813, 0); _temp1813= Cyc_Absyn_declare_stmt( _temp1805,
_temp1811,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1490, 0),
_temp1813, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1813, 0))->r));}}}}}}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1490)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1490, 0, Cyc_Toc_incr_lvalue,
_temp1488);( void*)( e->r=( void*)(( void*) _temp1490->r));}} goto _LL1399;}
_LL1413: { int e1_poly= Cyc_Toc_is_poly_project( _temp1496); void* e1_old_typ=(
void*)({ struct Cyc_Core_Opt* _temp1823= _temp1496->topt; if( _temp1823 == 0){
_throw( Null_Exception);} _temp1823->v;}); void* e2_old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1822= _temp1492->topt; if( _temp1822 == 0){ _throw(
Null_Exception);} _temp1822->v;}); Cyc_Toc_exp_to_c( nv, _temp1496); Cyc_Toc_exp_to_c(
nv, _temp1492); if( _temp1494 != 0?( void*)({ struct Cyc_Core_Opt* _temp1818=
_temp1494; if( _temp1818 == 0){ _throw( Null_Exception);} _temp1818->v;}) ==(
void*) Cyc_Absyn_Plus? 1:( void*)({ struct Cyc_Core_Opt* _temp1819= _temp1494;
if( _temp1819 == 0){ _throw( Null_Exception);} _temp1819->v;}) ==( void*) Cyc_Absyn_Minus:
0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){( void*)( _temp1496->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)
_temp1496->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1492->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1492->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1492->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1492->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1496)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1496, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1820=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1820[ 0]=({ struct _tuple11 _temp1821; _temp1821.f1=
_temp1494; _temp1821.f2= _temp1492; _temp1821;}); _temp1820;}));( void*)( e->r=(
void*)(( void*) _temp1496->r));} goto _LL1399;} _LL1415: Cyc_Toc_exp_to_c( nv,
_temp1502); Cyc_Toc_exp_to_c( nv, _temp1500); Cyc_Toc_exp_to_c( nv, _temp1498);
goto _LL1399; _LL1417: Cyc_Toc_exp_to_c( nv, _temp1506); Cyc_Toc_exp_to_c( nv,
_temp1504); goto _LL1399; _LL1419: _temp1514= _temp1510; _temp1512= _temp1508;
goto _LL1421; _LL1421: Cyc_Toc_exp_to_c( nv, _temp1514);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1512); goto _LL1399; _LL1423:
Cyc_Toc_exp_to_c( nv, _temp1516);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1516), 0))->r)); goto
_LL1399; _LL1425: Cyc_Toc_exp_to_c( nv, _temp1518); goto _LL1399; _LL1427: Cyc_Toc_exp_to_c(
nv, _temp1522);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1826= e->topt; if( _temp1826 == 0){ _throw( Null_Exception);} _temp1826->v;}));
for( 0; _temp1520 != 0; _temp1520=({ struct Cyc_List_List* _temp1824= _temp1520;
if( _temp1824 == 0){ _throw( Null_Exception);} _temp1824->tl;})){ void* k= Cyc_Tcutil_typ_kind((
void*)({ struct Cyc_List_List* _temp1825= _temp1520; if( _temp1825 == 0){ _throw(
Null_Exception);} _temp1825->hd;})); if( k !=( void*) Cyc_Absyn_EffKind? k !=(
void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1522, 0))->r)); break;}} goto _LL1399;} _LL1429: { void* old_t2=(
void*)({ struct Cyc_Core_Opt* _temp1827=(* _temp1526)->topt; if( _temp1827 == 0){
_throw( Null_Exception);} _temp1827->v;}); void* new_typ=* _temp1529;* _temp1529=
Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,* _temp1526);* _temp1526= Cyc_Toc_convert_exp(
nv->toplevel, old_t2, new_typ,* _temp1526); goto _LL1399;} _LL1431:{ void*
_temp1828=( void*) _temp1530->r; struct Cyc_Absyn_Structdecl* _temp1836; struct
Cyc_List_List* _temp1838; struct Cyc_Core_Opt* _temp1840; struct _tuple0*
_temp1842; struct Cyc_List_List* _temp1844; _LL1830: if(*(( int*) _temp1828) ==
Cyc_Absyn_Struct_e){ _LL1843: _temp1842=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1828)->f1; goto _LL1841; _LL1841: _temp1840=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1828)->f2; goto _LL1839; _LL1839:
_temp1838=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1828)->f3; goto _LL1837; _LL1837: _temp1836=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1828)->f4; goto _LL1831;} else{ goto
_LL1832;} _LL1832: if(*(( int*) _temp1828) == Cyc_Absyn_Tuple_e){ _LL1845:
_temp1844=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1828)->f1;
goto _LL1833;} else{ goto _LL1834;} _LL1834: goto _LL1835; _LL1831: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string
_temp1846= Cyc_Position_string_of_segment( _temp1530->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1846.last_plus_one - _temp1846.curr, _temp1846.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)({ struct Cyc_Core_Opt*
_temp1847= _temp1530->topt; if( _temp1847 == 0){ _throw( Null_Exception);}
_temp1847->v;}), 1, 0, _temp1838, _temp1842))->r)); goto _LL1829; _LL1833: if(
nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct
_tagged_string _temp1848= Cyc_Position_string_of_segment( _temp1530->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1848.last_plus_one -
_temp1848.curr, _temp1848.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, 0, _temp1844))->r)); goto _LL1829; _LL1835: Cyc_Toc_exp_to_c( nv,
_temp1530); if( ! Cyc_Absyn_is_lvalue( _temp1530)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1530, 0, Cyc_Toc_address_lvalue,
1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1849= e->topt; if( _temp1849 == 0){ _throw(
Null_Exception);} _temp1849->v;})), _temp1530, 0))->r));} goto _LL1829; _LL1829:;}
goto _LL1399; _LL1433: if( nv->toplevel){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ struct _tagged_string _temp1850= Cyc_Position_string_of_segment(
_temp1532->loc); xprintf("%.*s: new at top-level", _temp1850.last_plus_one -
_temp1850.curr, _temp1850.curr);}));}{ void* _temp1851=( void*) _temp1532->r;
struct Cyc_List_List* _temp1865; void* _temp1867; struct _tagged_string
_temp1869; struct Cyc_Absyn_Exp* _temp1871; struct Cyc_Absyn_Exp* _temp1873;
struct Cyc_Absyn_Vardecl* _temp1875; struct Cyc_Absyn_Structdecl* _temp1877;
struct Cyc_List_List* _temp1879; struct Cyc_Core_Opt* _temp1881; struct _tuple0*
_temp1883; struct Cyc_List_List* _temp1885; _LL1853: if(*(( int*) _temp1851) ==
Cyc_Absyn_Array_e){ _LL1866: _temp1865=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1851)->f1; goto _LL1854;} else{ goto _LL1855;} _LL1855: if(*(( int*)
_temp1851) == Cyc_Absyn_Const_e){ _LL1868: _temp1867=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1851)->f1; if(( unsigned int) _temp1867 > 1u?*(( int*) _temp1867) == Cyc_Absyn_String_c:
0){ _LL1870: _temp1869=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1867)->f1; goto _LL1856;} else{ goto _LL1857;}} else{ goto _LL1857;}
_LL1857: if(*(( int*) _temp1851) == Cyc_Absyn_Comprehension_e){ _LL1876:
_temp1875=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1851)->f1; goto _LL1874; _LL1874: _temp1873=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1851)->f2; goto _LL1872; _LL1872:
_temp1871=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1851)->f3; goto _LL1858;} else{ goto _LL1859;} _LL1859: if(*(( int*)
_temp1851) == Cyc_Absyn_Struct_e){ _LL1884: _temp1883=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1851)->f1; goto _LL1882; _LL1882:
_temp1881=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1851)->f2;
goto _LL1880; _LL1880: _temp1879=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1851)->f3; goto _LL1878; _LL1878: _temp1877=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1851)->f4; goto _LL1860;} else{ goto
_LL1861;} _LL1861: if(*(( int*) _temp1851) == Cyc_Absyn_Tuple_e){ _LL1886:
_temp1885=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1851)->f1;
goto _LL1862;} else{ goto _LL1863;} _LL1863: goto _LL1864; _LL1854: { struct
_tuple0* _temp1887= Cyc_Toc_temp_var(); goto _LL1888; _LL1888: { struct Cyc_Absyn_Exp*
_temp1889= Cyc_Absyn_var_exp( _temp1887, 0); goto _LL1890; _LL1890: { struct Cyc_Absyn_Stmt*
_temp1891= Cyc_Toc_init_array( nv, _temp1889, _temp1865, Cyc_Absyn_exp_stmt(
_temp1889, 0)); goto _LL1892; _LL1892: { void* old_elt_typ;{ void* _temp1893=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1899; struct Cyc_Absyn_Conref*
_temp1901; struct Cyc_Absyn_Tqual _temp1903; struct Cyc_Absyn_Conref* _temp1905;
void* _temp1907; void* _temp1909; _LL1895: if(( unsigned int) _temp1893 > 4u?*((
int*) _temp1893) == Cyc_Absyn_PointerType: 0){ _LL1900: _temp1899=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1893)->f1; _LL1910: _temp1909=( void*)
_temp1899.elt_typ; goto _LL1908; _LL1908: _temp1907=( void*) _temp1899.rgn_typ;
goto _LL1906; _LL1906: _temp1905=( struct Cyc_Absyn_Conref*) _temp1899.nullable;
goto _LL1904; _LL1904: _temp1903=( struct Cyc_Absyn_Tqual) _temp1899.tq; goto
_LL1902; _LL1902: _temp1901=( struct Cyc_Absyn_Conref*) _temp1899.bounds; goto
_LL1896;} else{ goto _LL1897;} _LL1897: goto _LL1898; _LL1896: old_elt_typ=
_temp1909; goto _LL1894; _LL1898: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1911=( char*)"exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1912; _temp1912.curr= _temp1911; _temp1912.base=
_temp1911; _temp1912.last_plus_one= _temp1911 + 52; _temp1912;})); goto _LL1894;
_LL1894:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* _temp1913= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1914; _LL1914: { struct Cyc_Absyn_Exp*
_temp1915= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1865), 0), 0); goto
_LL1916; _LL1916: { struct Cyc_Absyn_Exp* e1; if( _temp1534 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1915);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1917= _temp1534; if( _temp1917 == 0){ _throw(
Null_Exception);} _temp1917;}); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp(
r, _temp1915);}{ struct Cyc_Absyn_Exp* _temp1918= Cyc_Absyn_cast_exp( _temp1913,
e1, 0); goto _LL1919; _LL1919:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1887, _temp1913,( struct Cyc_Absyn_Exp*) _temp1918,
_temp1891, 0), 0))->r)); goto _LL1852;}}}}}}}} _LL1856: if( _temp1534 == 0){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1920=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1920->f1= Cyc_Toc_abs_ns; _temp1920->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1920;}), 0),({ struct Cyc_List_List* _temp1921=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1921->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1922=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1922[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1923; _temp1923.tag= Cyc_Absyn_Const_e; _temp1923.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1924=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1924[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1925; _temp1925.tag= Cyc_Absyn_String_c;
_temp1925.f1= _temp1869; _temp1925;}); _temp1924;})); _temp1923;}); _temp1922;}),
0); _temp1921->tl= 0; _temp1921;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1932= _temp1534; if(
_temp1932 == 0){ _throw( Null_Exception);} _temp1932;}); Cyc_Toc_exp_to_c( nv, r);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1926=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1926->f1= Cyc_Toc_abs_ns; _temp1926->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1926;}), 0),({ struct Cyc_List_List* _temp1927=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1927->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1928=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1928[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1929; _temp1929.tag= Cyc_Absyn_Const_e; _temp1929.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1930=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1930[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1931; _temp1931.tag= Cyc_Absyn_String_c;
_temp1931.f1= _temp1869; _temp1931;}); _temp1930;})); _temp1929;}); _temp1928;}),
0); _temp1927->tl= 0; _temp1927;}), 0))->r));} goto _LL1852; _LL1858: { int
is_tagged_ptr= 0;{ void* _temp1933= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1939; struct Cyc_Absyn_Conref* _temp1941; struct Cyc_Absyn_Tqual _temp1943;
struct Cyc_Absyn_Conref* _temp1945; void* _temp1947; void* _temp1949; _LL1935:
if(( unsigned int) _temp1933 > 4u?*(( int*) _temp1933) == Cyc_Absyn_PointerType:
0){ _LL1940: _temp1939=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1933)->f1; _LL1950: _temp1949=( void*) _temp1939.elt_typ; goto _LL1948;
_LL1948: _temp1947=( void*) _temp1939.rgn_typ; goto _LL1946; _LL1946: _temp1945=(
struct Cyc_Absyn_Conref*) _temp1939.nullable; goto _LL1944; _LL1944: _temp1943=(
struct Cyc_Absyn_Tqual) _temp1939.tq; goto _LL1942; _LL1942: _temp1941=( struct
Cyc_Absyn_Conref*) _temp1939.bounds; goto _LL1936;} else{ goto _LL1937;} _LL1937:
goto _LL1938; _LL1936:{ void* _temp1951=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1941))->v; void*
_temp1957; _LL1953: if(( unsigned int) _temp1951 > 1u?*(( int*) _temp1951) ==
Cyc_Absyn_Eq_constr: 0){ _LL1958: _temp1957=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1951)->f1; if( _temp1957 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1954;}
else{ goto _LL1955;}} else{ goto _LL1955;} _LL1955: goto _LL1956; _LL1954:
is_tagged_ptr= 1; goto _LL1952; _LL1956: goto _LL1952; _LL1952:;} goto _LL1934;
_LL1938:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1959=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1960; _temp1960.curr= _temp1959; _temp1960.base=
_temp1959; _temp1960.last_plus_one= _temp1959 + 42; _temp1960;})); goto _LL1934;
_LL1934:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)({ struct Cyc_Core_Opt* _temp1992= _temp1871->topt;
if( _temp1992 == 0){ _throw( Null_Exception);} _temp1992->v;}); void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1873);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1875, Cyc_Absyn_var_exp( max, 0), _temp1871,
Cyc_Absyn_skip_stmt( 0), 1); struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1990=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1990->hd=( void*)({ struct _tuple10* _temp1991=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1991->f1= max; _temp1991->f2= Cyc_Absyn_uint_t;
_temp1991->f3=( struct Cyc_Absyn_Exp*) _temp1873; _temp1991;}); _temp1990->tl= 0;
_temp1990;}); struct Cyc_Absyn_Exp* ai; if( _temp1534 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp1961= _temp1534; if( _temp1961 == 0){ _throw( Null_Exception);}
_temp1961;}); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct
Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1962=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1962->hd=( void*)({ struct _tuple10* _temp1963=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1963->f1= a; _temp1963->f2= ptr_typ;
_temp1963->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1963;}); _temp1962->tl= decls;
_temp1962;}); if( is_tagged_ptr){ struct _tuple0* _temp1964= Cyc_Toc_temp_var();
goto _LL1965; _LL1965: { void* _temp1966= Cyc_Toc_typ_to_c( old_typ); goto
_LL1967; _LL1967: { struct Cyc_Absyn_Exp* _temp1974= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp1968=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1968->hd=( void*)({ struct _tuple4* _temp1973=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1973->f1= 0;
_temp1973->f2= Cyc_Absyn_var_exp( a, 0); _temp1973;}); _temp1968->tl=({ struct
Cyc_List_List* _temp1969=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1969->hd=( void*)({ struct _tuple4* _temp1972=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1972->f1= 0; _temp1972->f2= Cyc_Absyn_var_exp( a,
0); _temp1972;}); _temp1969->tl=({ struct Cyc_List_List* _temp1970=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1970->hd=( void*)({ struct
_tuple4* _temp1971=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1971->f1= 0; _temp1971->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1971;}); _temp1970->tl= 0; _temp1970;});
_temp1969;}); _temp1968;}), 0); goto _LL1975; _LL1975: decls=({ struct Cyc_List_List*
_temp1976=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1976->hd=( void*)({ struct _tuple10* _temp1977=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1977->f1= _temp1964; _temp1977->f2=
_temp1966; _temp1977->f3=( struct Cyc_Absyn_Exp*) _temp1974; _temp1977;});
_temp1976->tl= decls; _temp1976;}); s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( _temp1964, 0), 0), 0);}}} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List* _temp1978= decls; goto
_LL1979; _LL1979: for( 0; _temp1978 != 0; _temp1978=({ struct Cyc_List_List*
_temp1980= _temp1978; if( _temp1980 == 0){ _throw( Null_Exception);} _temp1980->tl;})){
struct Cyc_Absyn_Exp* _temp1984; void* _temp1986; struct _tuple0* _temp1988;
struct _tuple10 _temp1982=*(( struct _tuple10*)({ struct Cyc_List_List*
_temp1981= _temp1978; if( _temp1981 == 0){ _throw( Null_Exception);} _temp1981->hd;}));
_LL1989: _temp1988= _temp1982.f1; goto _LL1987; _LL1987: _temp1986= _temp1982.f2;
goto _LL1985; _LL1985: _temp1984= _temp1982.f3; goto _LL1983; _LL1983: s= Cyc_Absyn_declare_stmt(
_temp1988, _temp1986, _temp1984, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r)); goto _LL1852;}}}} _LL1860:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1993= _temp1532->topt; if( _temp1993 ==
0){ _throw( Null_Exception);} _temp1993->v;}), 1, _temp1534, _temp1879,
_temp1883))->r)); goto _LL1852; _LL1862:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1534, _temp1885))->r)); goto _LL1852; _LL1864: { void* _temp1995=(
void*)({ struct Cyc_Core_Opt* _temp1994= _temp1532->topt; if( _temp1994 == 0){
_throw( Null_Exception);} _temp1994->v;}); goto _LL1996; _LL1996: { struct
_tuple0* _temp1997= Cyc_Toc_temp_var(); goto _LL1998; _LL1998: { struct Cyc_Absyn_Stmt*
_temp1999= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1997, 0), 0); goto
_LL2000; _LL2000: { struct Cyc_Absyn_Exp* _temp2001= Cyc_Absyn_signed_int_exp( 0,
0); goto _LL2002; _LL2002: Cyc_Toc_exp_to_c( nv, _temp1532); _temp1999= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1997, 0),
_temp2001, 0), _temp1532, 0), _temp1999, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
_temp1995); void* _temp2003= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); goto
_LL2004; _LL2004: { struct Cyc_Absyn_Exp* _temp2005= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0); goto _LL2006; _LL2006: { struct Cyc_Absyn_Exp* e1; if( _temp1534 ==
0){ e1= Cyc_Toc_malloc_exp( _temp1995, _temp2005);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2007= _temp1534; if(
_temp2007 == 0){ _throw( Null_Exception);} _temp2007;}); Cyc_Toc_exp_to_c( nv, r);
e1= Cyc_Toc_rmalloc_exp( r, _temp2005);}{ struct Cyc_Absyn_Exp* _temp2008= Cyc_Absyn_cast_exp(
_temp2003, e1, 0); goto _LL2009; _LL2009:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1997, _temp2003,( struct Cyc_Absyn_Exp*) _temp2008,
_temp1999, 0), 0))->r)); goto _LL1852;}}}}}}}} _LL1852:;} goto _LL1399; _LL1435:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct* _temp2010=(
struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp2010[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp2011; _temp2011.tag=
Cyc_Absyn_Sizeoftyp_e; _temp2011.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1536);
_temp2011;}); _temp2010;}))); goto _LL1399; _LL1437: Cyc_Toc_exp_to_c( nv,
_temp1538); goto _LL1399; _LL1439: { void* e1_typ= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp2045= _temp1540->topt; if( _temp2045 == 0){ _throw(
Null_Exception);} _temp2045->v;})); Cyc_Toc_exp_to_c( nv, _temp1540);{ void*
_temp2012= e1_typ; struct Cyc_Absyn_PtrInfo _temp2018; struct Cyc_Absyn_Conref*
_temp2020; struct Cyc_Absyn_Tqual _temp2022; struct Cyc_Absyn_Conref* _temp2024;
void* _temp2026; void* _temp2028; _LL2014: if(( unsigned int) _temp2012 > 4u?*((
int*) _temp2012) == Cyc_Absyn_PointerType: 0){ _LL2019: _temp2018=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp2012)->f1; _LL2029: _temp2028=( void*)
_temp2018.elt_typ; goto _LL2027; _LL2027: _temp2026=( void*) _temp2018.rgn_typ;
goto _LL2025; _LL2025: _temp2024=( struct Cyc_Absyn_Conref*) _temp2018.nullable;
goto _LL2023; _LL2023: _temp2022=( struct Cyc_Absyn_Tqual) _temp2018.tq; goto
_LL2021; _LL2021: _temp2020=( struct Cyc_Absyn_Conref*) _temp2018.bounds; goto
_LL2015;} else{ goto _LL2016;} _LL2016: goto _LL2017; _LL2015:{ void* _temp2030=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2020); struct
Cyc_Absyn_Exp* _temp2036; _LL2032: if( _temp2030 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL2033;} else{ goto _LL2034;} _LL2034: if(( unsigned int) _temp2030 > 1u?*((
int*) _temp2030) == Cyc_Absyn_Upper_b: 0){ _LL2037: _temp2036=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp2030)->f1; goto _LL2035;} else{ goto
_LL2031;} _LL2033: { struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* _temp2038= Cyc_Toc_typ_to_c_array( _temp2028); goto
_LL2039; _LL2039: { struct _tagged_string* _temp2040= Cyc_Toc_add_tagged_ptr_type(
_temp2038, _temp2022); goto _LL2041; _LL2041: { void* tx= Cyc_Absyn_strct(
_temp2040); void* ta2= Cyc_Absyn_cstar_typ( _temp2038, _temp2022); struct Cyc_Absyn_Exp*
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
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp1540, s1, 0), 0))->r));
goto _LL2031;}}} _LL2035: if( Cyc_Toc_is_nullable( e1_typ)){ struct _tuple0* x=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp2042= _temp1540->topt; if( _temp2042 == 0){ _throw( Null_Exception);}
_temp2042->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1540, s3, 0), 0))->r));} goto _LL2031; _LL2031:;} goto _LL2013; _LL2017:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp2043=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp2044; _temp2044.curr= _temp2043; _temp2044.base= _temp2043; _temp2044.last_plus_one=
_temp2043 + 29; _temp2044;})); goto _LL2013; _LL2013:;} goto _LL1399;} _LL1441:
Cyc_Toc_exp_to_c( nv, _temp1544); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({ struct
Cyc_Core_Opt* _temp2046= e->topt; if( _temp2046 == 0){ _throw( Null_Exception);}
_temp2046->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1399;
_LL1443: Cyc_Toc_exp_to_c( nv, _temp1548);{ int is_poly= Cyc_Toc_is_poly_project(
e); if( Cyc_Toc_is_nullable(( void*)({ struct Cyc_Core_Opt* _temp2047= _temp1548->topt;
if( _temp2047 == 0){ _throw( Null_Exception);} _temp2047->v;}))){ struct _tuple0*
x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp2048= _temp1548->topt; if( _temp2048 == 0){ _throw( Null_Exception);}
_temp2048->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0),
_temp1546, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1548, s3, 0), 0))->r));} if( is_poly){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp2049= e->topt; if( _temp2049 == 0){ _throw(
Null_Exception);} _temp2049->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}
goto _LL1399;} _LL1445: { void* _temp2051= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp2050= _temp1552->topt; if( _temp2050 == 0){ _throw(
Null_Exception);} _temp2050->v;})); goto _LL2052; _LL2052: Cyc_Toc_exp_to_c( nv,
_temp1552); Cyc_Toc_exp_to_c( nv, _temp1550);( void*)( _temp1552->r=( void*)((
void*)( Cyc_Toc_array_to_ptr( Cyc_Toc_typ_to_c( _temp2051), Cyc_Absyn_new_exp((
void*) _temp1552->r, 0), 0))->r));{ void* _temp2053= _temp2051; struct Cyc_List_List*
_temp2063; struct Cyc_Absyn_Exp* _temp2065; struct Cyc_Absyn_Tqual _temp2067;
void* _temp2069; struct Cyc_Absyn_PtrInfo _temp2071; struct Cyc_Absyn_Conref*
_temp2073; struct Cyc_Absyn_Tqual _temp2075; struct Cyc_Absyn_Conref* _temp2077;
void* _temp2079; void* _temp2081; _LL2055: if(( unsigned int) _temp2053 > 4u?*((
int*) _temp2053) == Cyc_Absyn_TupleType: 0){ _LL2064: _temp2063=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp2053)->f1; goto _LL2056;} else{ goto
_LL2057;} _LL2057: if(( unsigned int) _temp2053 > 4u?*(( int*) _temp2053) == Cyc_Absyn_ArrayType:
0){ _LL2070: _temp2069=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2053)->f1;
goto _LL2068; _LL2068: _temp2067=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2053)->f2; goto _LL2066; _LL2066: _temp2065=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2053)->f3; goto _LL2058;} else{ goto
_LL2059;} _LL2059: if(( unsigned int) _temp2053 > 4u?*(( int*) _temp2053) == Cyc_Absyn_PointerType:
0){ _LL2072: _temp2071=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2053)->f1; _LL2082: _temp2081=( void*) _temp2071.elt_typ; goto _LL2080;
_LL2080: _temp2079=( void*) _temp2071.rgn_typ; goto _LL2078; _LL2078: _temp2077=(
struct Cyc_Absyn_Conref*) _temp2071.nullable; goto _LL2076; _LL2076: _temp2075=(
struct Cyc_Absyn_Tqual) _temp2071.tq; goto _LL2074; _LL2074: _temp2073=( struct
Cyc_Absyn_Conref*) _temp2071.bounds; goto _LL2060;} else{ goto _LL2061;} _LL2061:
goto _LL2062; _LL2056: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1550) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1552,({ struct _tagged_string* _temp2083=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp2083[ 0]= xprintf("f%d",( int)
i); _temp2083;}), 0))->r)); goto _LL2054;} _LL2058: if( _temp2065 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp2084=( char*)"exp_to_c: Subscript_e array type bound null"; struct
_tagged_string _temp2085; _temp2085.curr= _temp2084; _temp2085.base= _temp2084;
_temp2085.last_plus_one= _temp2084 + 44; _temp2085;}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_known_size_subscript( _temp1552, _temp1550, 0,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2086= _temp2065; if( _temp2086 == 0){ _throw(
Null_Exception);} _temp2086;}), _temp2069, _temp2067, 0))->r)); goto _LL2054;
_LL2060:{ void* _temp2087=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp2073); struct Cyc_Absyn_Exp* _temp2093; _LL2089: if(( unsigned int)
_temp2087 > 1u?*(( int*) _temp2087) == Cyc_Absyn_Upper_b: 0){ _LL2094: _temp2093=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp2087)->f1; goto
_LL2090;} else{ goto _LL2091;} _LL2091: if( _temp2087 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL2092;} else{ goto _LL2088;} _LL2090: { int possibly_null= 1;{ void*
_temp2095=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp2077))->v; int _temp2101; _LL2097: if((
unsigned int) _temp2095 > 1u?*(( int*) _temp2095) == Cyc_Absyn_Eq_constr: 0){
_LL2102: _temp2101=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2095)->f1;
goto _LL2098;} else{ goto _LL2099;} _LL2099: goto _LL2100; _LL2098:
possibly_null= _temp2101; goto _LL2096; _LL2100: possibly_null= 0; goto _LL2096;
_LL2096:;}( void*)( e->r=( void*)(( void*)( Cyc_Toc_known_size_subscript(
_temp1552, _temp1550, possibly_null, _temp2093, _temp2081, _temp2075, 0))->r));
goto _LL2088;} _LL2092: { struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0*
i= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void* _temp2103=
Cyc_Toc_typ_to_c_array( _temp2081); goto _LL2104; _LL2104: { struct
_tagged_string* _temp2105= Cyc_Toc_add_tagged_ptr_type( _temp2103, _temp2075);
goto _LL2106; _LL2106: { void* tx= Cyc_Absyn_strct( _temp2105); void* ta2= Cyc_Absyn_cstar_typ(
_temp2103, _temp2075); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1550, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1552, s1, 0), 0))->r)); goto _LL2088;}}}
_LL2088:;} goto _LL2054; _LL2062:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2107=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp2108; _temp2108.curr= _temp2107; _temp2108.base=
_temp2107; _temp2108.last_plus_one= _temp2107 + 49; _temp2108;})); goto _LL2054;
_LL2054:;} goto _LL1399;} _LL1447: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1554))->r));} else{ struct Cyc_List_List*
_temp2109=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1554); goto
_LL2110; _LL2110: { struct _tagged_string* _temp2111= Cyc_Toc_add_tuple_type(
_temp2109); goto _LL2112; _LL2112: { struct Cyc_List_List* dles= 0;{ int i= 1;
for( 0; _temp1554 != 0; _temp1554=({ struct Cyc_List_List* _temp2113= _temp1554;
if( _temp2113 == 0){ _throw( Null_Exception);} _temp2113->tl;}), i ++){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp2114= _temp1554; if(
_temp2114 == 0){ _throw( Null_Exception);} _temp2114->hd;}));{ struct Cyc_List_List*
_temp2119=({ struct Cyc_List_List* _temp2115=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2115->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct*
_temp2116=( struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2116[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2117; _temp2117.tag= Cyc_Absyn_FieldName;
_temp2117.f1=({ struct _tagged_string* _temp2118=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp2118[ 0]= xprintf("f%d", i);
_temp2118;}); _temp2117;}); _temp2116;})); _temp2115->tl= 0; _temp2115;}); goto
_LL2120; _LL2120: dles=({ struct Cyc_List_List* _temp2121=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2121->hd=( void*)({ struct
_tuple4* _temp2122=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp2122->f1=( struct Cyc_List_List*) _temp2119; _temp2122->f2=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp2123= _temp1554; if( _temp2123 == 0){ _throw(
Null_Exception);} _temp2123->hd;}); _temp2122;}); _temp2121->tl= dles; _temp2121;});}}}
dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct(
_temp2111), Cyc_Absyn_unresolvedmem_exp( 0, dles, 0), 0))->r));}}} goto _LL1399;
_LL1449:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp2124=( char*)"compoundlit"; struct _tagged_string
_temp2125; _temp2125.curr= _temp2124; _temp2125.base= _temp2124; _temp2125.last_plus_one=
_temp2124 + 12; _temp2125;})); goto _LL1399; _LL1451:( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1560, 0))->r));{ struct Cyc_List_List*
_temp2126= _temp1560; goto _LL2127; _LL2127: for( 0; _temp2126 != 0; _temp2126=({
struct Cyc_List_List* _temp2128= _temp2126; if( _temp2128 == 0){ _throw(
Null_Exception);} _temp2128->tl;})){ struct _tuple4 _temp2132; struct Cyc_Absyn_Exp*
_temp2133; struct Cyc_List_List* _temp2135; struct _tuple4* _temp2130=( struct
_tuple4*)({ struct Cyc_List_List* _temp2129= _temp2126; if( _temp2129 == 0){
_throw( Null_Exception);} _temp2129->hd;}); _temp2132=* _temp2130; _LL2136:
_temp2135= _temp2132.f1; goto _LL2134; _LL2134: _temp2133= _temp2132.f2; goto
_LL2131; _LL2131: Cyc_Toc_exp_to_c( nv, _temp2133);}} goto _LL1399; _LL1453: {
unsigned int _temp2137= Cyc_Evexp_eval_const_uint_exp( _temp1564); goto _LL2138;
_LL2138: Cyc_Toc_exp_to_c( nv, _temp1562);{ struct Cyc_List_List* es= 0; if( !
Cyc_Toc_is_zero( _temp1562)){ unsigned int i= 0; for( 0; i < _temp2137; i ++){
es=({ struct Cyc_List_List* _temp2139=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2139->hd=( void*)({ struct _tuple4*
_temp2140=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp2140->f1=
0; _temp2140->f2= _temp1562; _temp2140;}); _temp2139->tl= es; _temp2139;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL1399;}} _LL1455: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1570, _temp1574))->r));} else{
struct _tuple0* _temp2141= _temp1574; goto _LL2142; _LL2142: { struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl* _temp2159=
_temp1568 == 0?(( struct Cyc_Absyn_Structdecl*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp2158= Cyc_Absynpp_qvar2string( _temp1574); xprintf("no structdecl %.*s",
_temp2158.last_plus_one - _temp2158.curr, _temp2158.curr);})): _temp1568; if(
_temp2159 == 0){ _throw( Null_Exception);} _temp2159;}); struct _RegionHandle
_temp2143= _new_region(); struct _RegionHandle* rgn=& _temp2143; _push_region(
rgn);{ struct Cyc_List_List* _temp2145=(( struct Cyc_List_List*(*)( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, e->loc,
_temp1570,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2144=(( struct
Cyc_Absyn_Structdecl*) sd2)->fields; if( _temp2144 == 0){ _throw( Null_Exception);}
_temp2144->v;})); goto _LL2146; _LL2146: for( 0; _temp2145 != 0; _temp2145=({
struct Cyc_List_List* _temp2147= _temp2145; if( _temp2147 == 0){ _throw(
Null_Exception);} _temp2147->tl;})){ struct _tuple12 _temp2151; struct Cyc_Absyn_Exp*
_temp2152; struct Cyc_Absyn_Structfield* _temp2154; struct _tuple12* _temp2149=(
struct _tuple12*)({ struct Cyc_List_List* _temp2148= _temp2145; if( _temp2148 ==
0){ _throw( Null_Exception);} _temp2148->hd;}); _temp2151=* _temp2149; _LL2155:
_temp2154= _temp2151.f1; goto _LL2153; _LL2153: _temp2152= _temp2151.f2; goto
_LL2150; _LL2150: { void* _temp2156=( void*) _temp2154->type; goto _LL2157;
_LL2157: Cyc_Toc_exp_to_c( nv, _temp2152); if( Cyc_Toc_is_void_star( _temp2156)){(
void*)( _temp2152->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp2152->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strctq( _temp2141), Cyc_Absyn_unresolvedmem_exp(
0, _temp1570, 0), 0))->r));}; _pop_region(& _temp2143);}} goto _LL1399; _LL1457: {
struct Cyc_List_List* fs;{ void* _temp2160= Cyc_Tcutil_compress( _temp1578);
struct Cyc_List_List* _temp2166; _LL2162: if(( unsigned int) _temp2160 > 4u?*((
int*) _temp2160) == Cyc_Absyn_AnonStructType: 0){ _LL2167: _temp2166=( struct
Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*) _temp2160)->f1; goto
_LL2163;} else{ goto _LL2164;} _LL2164: goto _LL2165; _LL2163: fs= _temp2166;
goto _LL2161; _LL2165: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp2168= Cyc_Absynpp_typ2string(
_temp1578); xprintf("anon struct has type %.*s", _temp2168.last_plus_one -
_temp2168.curr, _temp2168.curr);})); goto _LL2161; _LL2161:;}{ struct
_RegionHandle _temp2169= _new_region(); struct _RegionHandle* rgn=& _temp2169;
_push_region( rgn);{ struct Cyc_List_List* _temp2170=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1576, fs); goto _LL2171; _LL2171: for( 0; _temp2170 != 0; _temp2170=({
struct Cyc_List_List* _temp2172= _temp2170; if( _temp2172 == 0){ _throw(
Null_Exception);} _temp2172->tl;})){ struct _tuple12 _temp2176; struct Cyc_Absyn_Exp*
_temp2177; struct Cyc_Absyn_Structfield* _temp2179; struct _tuple12* _temp2174=(
struct _tuple12*)({ struct Cyc_List_List* _temp2173= _temp2170; if( _temp2173 ==
0){ _throw( Null_Exception);} _temp2173->hd;}); _temp2176=* _temp2174; _LL2180:
_temp2179= _temp2176.f1; goto _LL2178; _LL2178: _temp2177= _temp2176.f2; goto
_LL2175; _LL2175: { void* _temp2181=( void*) _temp2179->type; goto _LL2182;
_LL2182: Cyc_Toc_exp_to_c( nv, _temp2177); if( Cyc_Toc_is_void_star( _temp2181)){(
void*)( _temp2177->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp2177->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1576, 0))->r));}; _pop_region(&
_temp2169);} goto _LL1399;} _LL1459: { struct _tuple0* qv= _temp1580->name; if(
_temp1582->is_xtunion? 1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp(
qv, 0))->r));} else{ int tag_count= 0; struct Cyc_List_List* _temp2184=
_temp1582->fields == 0? 0:( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2183= _temp1582->fields; if( _temp2183 == 0){ _throw( Null_Exception);}
_temp2183->v;}); goto _LL2185; _LL2185: while( Cyc_Absyn_qvar_cmp( qv,(( struct
Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2186= _temp2184; if(
_temp2186 == 0){ _throw( Null_Exception);} _temp2186->hd;}))->name) != 0) { if(((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2187= _temp2184; if(
_temp2187 == 0){ _throw( Null_Exception);} _temp2187->hd;}))->typs == 0){
tag_count ++;} _temp2184=({ struct Cyc_List_List* _temp2188= _temp2184; if(
_temp2188 == 0){ _throw( Null_Exception);} _temp2188->tl;});}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL1399;} _LL1461: { struct _tuple0* _temp2189= _temp1590->name; goto _LL2190;
_LL2190: { struct Cyc_List_List* _temp2191= _temp1590->typs; goto _LL2192;
_LL2192: { struct _tuple0* _temp2193= Cyc_Toc_temp_var(); goto _LL2194; _LL2194: {
struct Cyc_Absyn_Exp* _temp2195= Cyc_Absyn_var_exp( _temp2193, 0); goto _LL2196;
_LL2196: { void* _temp2199= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2189,( struct _tagged_string)({ char* _temp2197=( char*)"_struct"; struct
_tagged_string _temp2198; _temp2198.curr= _temp2197; _temp2198.base= _temp2197;
_temp2198.last_plus_one= _temp2197 + 8; _temp2198;}))); goto _LL2200; _LL2200:
if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp; if( _temp1592->is_xtunion){
tag_exp= Cyc_Absyn_var_exp( _temp2189, 0);} else{ int tag_count= 0; struct Cyc_List_List*
_temp2202= _temp1592->fields == 0? 0:( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2201= _temp1592->fields; if( _temp2201 == 0){ _throw( Null_Exception);}
_temp2201->v;}); goto _LL2203; _LL2203: while( Cyc_Absyn_qvar_cmp( _temp2189,((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2204= _temp2202; if(
_temp2204 == 0){ _throw( Null_Exception);} _temp2204->hd;}))->name) != 0) { if(((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2205= _temp2202; if(
_temp2205 == 0){ _throw( Null_Exception);} _temp2205->hd;}))->typs != 0){
tag_count ++;} _temp2202=({ struct Cyc_List_List* _temp2206= _temp2202; if(
_temp2206 == 0){ _throw( Null_Exception);} _temp2206->tl;});} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2207= 0; goto _LL2208;
_LL2208: for( 0; _temp1594 != 0; _temp1594=({ struct Cyc_List_List* _temp2209=
_temp1594; if( _temp2209 == 0){ _throw( Null_Exception);} _temp2209->tl;}),
_temp2191=({ struct Cyc_List_List* _temp2210= _temp2191; if( _temp2210 == 0){
_throw( Null_Exception);} _temp2210->tl;})){ struct Cyc_Absyn_Exp* cur_e=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp2216= _temp1594; if(
_temp2216 == 0){ _throw( Null_Exception);} _temp2216->hd;}); void* field_typ=
Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List* _temp2215=
_temp2191; if( _temp2215 == 0){ _throw( Null_Exception);} _temp2215->hd;}))).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2207=({ struct Cyc_List_List* _temp2211=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2211[ 0]=({ struct Cyc_List_List
_temp2212; _temp2212.hd=( void*)({ struct _tuple4* _temp2213=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2213[ 0]=({ struct _tuple4
_temp2214; _temp2214.f1= 0; _temp2214.f2= cur_e; _temp2214;}); _temp2213;});
_temp2212.tl= _temp2207; _temp2212;}); _temp2211;});} _temp2207=({ struct Cyc_List_List*
_temp2217=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2217[ 0]=({ struct Cyc_List_List _temp2218; _temp2218.hd=( void*)({ struct
_tuple4* _temp2219=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2219[ 0]=({ struct _tuple4 _temp2220; _temp2220.f1= 0; _temp2220.f2=
tag_exp; _temp2220;}); _temp2219;}); _temp2218.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2207); _temp2218;}); _temp2217;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2221=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2221->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2189,( struct
_tagged_string)({ char* _temp2222=( char*)"_struct"; struct _tagged_string
_temp2223; _temp2223.curr= _temp2222; _temp2223.base= _temp2222; _temp2223.last_plus_one=
_temp2222 + 8; _temp2223;})); _temp2221;}), _temp2207, 0))->r));}} else{ struct
Cyc_List_List* _temp2225=({ struct Cyc_List_List* _temp2224=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2224->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2195, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp2189, 0), 0); _temp2224->tl= 0; _temp2224;}); goto _LL2226; _LL2226:{ int i=
1; for( 0; _temp1594 != 0;( _temp1594=({ struct Cyc_List_List* _temp2227=
_temp1594; if( _temp2227 == 0){ _throw( Null_Exception);} _temp2227->tl;}), i ++),
_temp2191=({ struct Cyc_List_List* _temp2228= _temp2191; if( _temp2228 == 0){
_throw( Null_Exception);} _temp2228->tl;})){ struct Cyc_Absyn_Exp* cur_e=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp2234= _temp1594; if(
_temp2234 == 0){ _throw( Null_Exception);} _temp2234->hd;}); void* field_typ=
Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List* _temp2233=
_temp2191; if( _temp2233 == 0){ _throw( Null_Exception);} _temp2233->hd;}))).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp2230= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2195,({ struct _tagged_string* _temp2229=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2229[ 0]= xprintf("f%d", i); _temp2229;}), 0), cur_e, 0); goto _LL2231;
_LL2231: _temp2225=({ struct Cyc_List_List* _temp2232=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2232->hd=( void*) _temp2230;
_temp2232->tl= _temp2225; _temp2232;});}}}{ struct Cyc_Absyn_Stmt* _temp2235=
Cyc_Absyn_exp_stmt( _temp2195, 0); goto _LL2236; _LL2236: { struct Cyc_Absyn_Stmt*
_temp2238= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp2237=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2237->hd=( void*) _temp2235;
_temp2237->tl= _temp2225; _temp2237;})), 0); goto _LL2239; _LL2239:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp2193,
_temp2199, 0, _temp2238, 0), 0))->r));}}} goto _LL1399;}}}}} _LL1463: goto
_LL1399; _LL1465: { void* t_c= Cyc_Toc_typ_to_c( _temp1606); if( _temp1608 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2240= _temp1608; if( _temp2240 == 0){ _throw( Null_Exception);} _temp2240;});
Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp(
rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} else{( void*)( e->r=( void*)((
void*)( Cyc_Toc_malloc_exp( _temp1606, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
goto _LL1399;} _LL1467: Cyc_Toc_stmt_to_c( nv, _temp1610); goto _LL1399; _LL1469:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp2241=( char*)"UnresolvedMem"; struct _tagged_string _temp2242;
_temp2242.curr= _temp2241; _temp2242.base= _temp2241; _temp2242.last_plus_one=
_temp2241 + 14; _temp2242;})); goto _LL1399; _LL1471:(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp2243=(
char*)"codegen"; struct _tagged_string _temp2244; _temp2244.curr= _temp2243;
_temp2244.base= _temp2243; _temp2244.last_plus_one= _temp2243 + 8; _temp2244;}));
goto _LL1399; _LL1473:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2245=( char*)"fill"; struct _tagged_string
_temp2246; _temp2246.curr= _temp2245; _temp2246.base= _temp2245; _temp2246.last_plus_one=
_temp2245 + 5; _temp2246;})); goto _LL1399; _LL1399:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple14{ struct _tuple0* f1; void* f2;
} ; struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, void* t,
struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat* p,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp2248=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp2284; struct Cyc_Absyn_Vardecl _temp2286; struct Cyc_List_List* _temp2287;
struct Cyc_Core_Opt* _temp2289; struct Cyc_Absyn_Exp* _temp2291; void* _temp2293;
struct Cyc_Absyn_Tqual _temp2295; struct _tuple0* _temp2297; void* _temp2299;
struct Cyc_Absyn_Vardecl* _temp2301; int _temp2303; void* _temp2305; char
_temp2307; struct _tagged_string _temp2309; struct Cyc_Absyn_Enumfield*
_temp2311; struct Cyc_Absyn_Enumdecl* _temp2313; struct Cyc_List_List* _temp2315;
struct Cyc_List_List* _temp2317; struct Cyc_Absyn_Tunionfield* _temp2319; struct
Cyc_Absyn_Tuniondecl* _temp2321; struct Cyc_Absyn_Pat* _temp2324; struct Cyc_Absyn_Pat
_temp2326; struct Cyc_Position_Segment* _temp2327; struct Cyc_Core_Opt*
_temp2329; void* _temp2331; struct Cyc_List_List* _temp2333; struct Cyc_List_List*
_temp2335; struct Cyc_Absyn_Tunionfield* _temp2337; struct Cyc_Absyn_Tuniondecl*
_temp2339; struct Cyc_List_List* _temp2341; struct Cyc_List_List* _temp2343;
struct Cyc_Absyn_Tunionfield* _temp2345; struct Cyc_Absyn_Tuniondecl* _temp2347;
struct Cyc_List_List* _temp2349; struct Cyc_List_List* _temp2351; struct Cyc_List_List*
_temp2353; struct Cyc_Core_Opt* _temp2355; struct Cyc_Absyn_Structdecl*
_temp2357; struct Cyc_Absyn_Pat* _temp2359; struct _tuple0* _temp2361; struct
Cyc_List_List* _temp2363; struct Cyc_List_List* _temp2365; struct _tuple0*
_temp2367; struct Cyc_List_List* _temp2369; struct Cyc_List_List* _temp2371;
struct _tuple0* _temp2373; _LL2250: if(( unsigned int) _temp2248 > 2u?*(( int*)
_temp2248) == Cyc_Absyn_Var_p: 0){ _LL2285: _temp2284=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp2248)->f1; _temp2286=* _temp2284; _LL2300:
_temp2299=( void*) _temp2286.sc; goto _LL2298; _LL2298: _temp2297=( struct
_tuple0*) _temp2286.name; goto _LL2296; _LL2296: _temp2295=( struct Cyc_Absyn_Tqual)
_temp2286.tq; goto _LL2294; _LL2294: _temp2293=( void*) _temp2286.type; goto
_LL2292; _LL2292: _temp2291=( struct Cyc_Absyn_Exp*) _temp2286.initializer; goto
_LL2290; _LL2290: _temp2289=( struct Cyc_Core_Opt*) _temp2286.rgn; goto _LL2288;
_LL2288: _temp2287=( struct Cyc_List_List*) _temp2286.attributes; goto _LL2251;}
else{ goto _LL2252;} _LL2252: if( _temp2248 ==( void*) Cyc_Absyn_Wild_p){ goto
_LL2253;} else{ goto _LL2254;} _LL2254: if(( unsigned int) _temp2248 > 2u?*((
int*) _temp2248) == Cyc_Absyn_Reference_p: 0){ _LL2302: _temp2301=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Reference_p_struct*) _temp2248)->f1; goto _LL2255;} else{ goto
_LL2256;} _LL2256: if( _temp2248 ==( void*) Cyc_Absyn_Null_p){ goto _LL2257;}
else{ goto _LL2258;} _LL2258: if(( unsigned int) _temp2248 > 2u?*(( int*)
_temp2248) == Cyc_Absyn_Int_p: 0){ _LL2306: _temp2305=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp2248)->f1; goto _LL2304; _LL2304: _temp2303=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp2248)->f2; goto _LL2259;} else{ goto _LL2260;} _LL2260: if(( unsigned int)
_temp2248 > 2u?*(( int*) _temp2248) == Cyc_Absyn_Char_p: 0){ _LL2308: _temp2307=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp2248)->f1; goto _LL2261;} else{
goto _LL2262;} _LL2262: if(( unsigned int) _temp2248 > 2u?*(( int*) _temp2248)
== Cyc_Absyn_Float_p: 0){ _LL2310: _temp2309=( struct _tagged_string)(( struct
Cyc_Absyn_Float_p_struct*) _temp2248)->f1; goto _LL2263;} else{ goto _LL2264;}
_LL2264: if(( unsigned int) _temp2248 > 2u?*(( int*) _temp2248) == Cyc_Absyn_Enum_p:
0){ _LL2314: _temp2313=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp2248)->f1; goto _LL2312; _LL2312: _temp2311=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp2248)->f2; goto _LL2265;} else{ goto
_LL2266;} _LL2266: if(( unsigned int) _temp2248 > 2u?*(( int*) _temp2248) == Cyc_Absyn_Tunion_p:
0){ _LL2322: _temp2321=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2248)->f1; goto _LL2320; _LL2320: _temp2319=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2248)->f2; goto _LL2318; _LL2318:
_temp2317=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2248)->f3; goto _LL2316; _LL2316: _temp2315=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2248)->f4; if( _temp2315 == 0){ goto
_LL2267;} else{ goto _LL2268;}} else{ goto _LL2268;} _LL2268: if(( unsigned int)
_temp2248 > 2u?*(( int*) _temp2248) == Cyc_Absyn_Pointer_p: 0){ _LL2325:
_temp2324=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2248)->f1; _temp2326=* _temp2324; _LL2332: _temp2331=( void*) _temp2326.r;
if(( unsigned int) _temp2331 > 2u?*(( int*) _temp2331) == Cyc_Absyn_Tunion_p: 0){
_LL2340: _temp2339=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2331)->f1; goto _LL2338; _LL2338: _temp2337=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2331)->f2; goto _LL2336; _LL2336:
_temp2335=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2331)->f3; goto _LL2334; _LL2334: _temp2333=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2331)->f4; goto _LL2330;} else{ goto
_LL2270;} _LL2330: _temp2329=( struct Cyc_Core_Opt*) _temp2326.topt; goto
_LL2328; _LL2328: _temp2327=( struct Cyc_Position_Segment*) _temp2326.loc; goto
_LL2323;} else{ goto _LL2270;} _LL2323: if( _temp2333 != 0){ goto _LL2269;}
else{ goto _LL2270;} _LL2270: if(( unsigned int) _temp2248 > 2u?*(( int*)
_temp2248) == Cyc_Absyn_Tunion_p: 0){ _LL2348: _temp2347=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2248)->f1; goto _LL2346; _LL2346:
_temp2345=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2248)->f2; goto _LL2344; _LL2344: _temp2343=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2248)->f3; goto _LL2342; _LL2342:
_temp2341=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2248)->f4; goto _LL2271;} else{ goto _LL2272;} _LL2272: if(( unsigned int)
_temp2248 > 2u?*(( int*) _temp2248) == Cyc_Absyn_Tuple_p: 0){ _LL2350: _temp2349=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp2248)->f1; goto
_LL2273;} else{ goto _LL2274;} _LL2274: if(( unsigned int) _temp2248 > 2u?*((
int*) _temp2248) == Cyc_Absyn_Struct_p: 0){ _LL2358: _temp2357=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp2248)->f1; goto _LL2356; _LL2356:
_temp2355=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp2248)->f2;
goto _LL2354; _LL2354: _temp2353=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp2248)->f3; goto _LL2352; _LL2352: _temp2351=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp2248)->f4; goto _LL2275;} else{ goto
_LL2276;} _LL2276: if(( unsigned int) _temp2248 > 2u?*(( int*) _temp2248) == Cyc_Absyn_Pointer_p:
0){ _LL2360: _temp2359=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2248)->f1; goto _LL2277;} else{ goto _LL2278;} _LL2278: if(( unsigned int)
_temp2248 > 2u?*(( int*) _temp2248) == Cyc_Absyn_UnknownId_p: 0){ _LL2362:
_temp2361=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2248)->f1;
goto _LL2279;} else{ goto _LL2280;} _LL2280: if(( unsigned int) _temp2248 > 2u?*((
int*) _temp2248) == Cyc_Absyn_UnknownCall_p: 0){ _LL2368: _temp2367=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp2248)->f1; goto _LL2366;
_LL2366: _temp2365=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2248)->f2; goto _LL2364; _LL2364: _temp2363=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp2248)->f3; goto _LL2281;} else{
goto _LL2282;} _LL2282: if(( unsigned int) _temp2248 > 2u?*(( int*) _temp2248)
== Cyc_Absyn_UnknownFields_p: 0){ _LL2374: _temp2373=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp2248)->f1; goto _LL2372; _LL2372:
_temp2371=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2248)->f2; goto _LL2370; _LL2370: _temp2369=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp2248)->f3; goto _LL2283;} else{
goto _LL2249;} _LL2251: nv= Cyc_Toc_add_varmap( nv, _temp2297, r); goto _LL2253;
_LL2253: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL2249; _LL2255: { struct
_tuple0* _temp2375= Cyc_Toc_temp_var(); goto _LL2376; _LL2376: decls=({ struct
Cyc_List_List* _temp2377=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2377->hd=( void*)({ struct _tuple14* _temp2378=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2378->f1= _temp2375; _temp2378->f2=
Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2378;});
_temp2377->tl= decls; _temp2377;}); nv= Cyc_Toc_add_varmap( nv, _temp2301->name,
Cyc_Absyn_var_exp( _temp2375, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2375, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2249;} _LL2257: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2249; _LL2259: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2305, _temp2303, 0), succ_lab, fail_lab); goto _LL2249;
_LL2261: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2307, 0),
succ_lab, fail_lab); goto _LL2249; _LL2263: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2309, 0), succ_lab, fail_lab); goto _LL2249; _LL2265: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2379=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2379[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2380; _temp2380.tag= Cyc_Absyn_Enum_e;
_temp2380.f1= _temp2311->name; _temp2380.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2313; _temp2380.f3=( struct Cyc_Absyn_Enumfield*) _temp2311; _temp2380;});
_temp2379;}), 0), succ_lab, fail_lab); goto _LL2249; _LL2267: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2319->name, 0); if( ! _temp2321->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2249;} _LL2269: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2333);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2337->name,( struct
_tagged_string)({ char* _temp2416=( char*)"_struct"; struct _tagged_string
_temp2417; _temp2417.curr= _temp2416; _temp2417.base= _temp2416; _temp2417.last_plus_one=
_temp2416 + 8; _temp2417;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0);{ struct
Cyc_List_List* _temp2381=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp2333); goto _LL2382; _LL2382: for( 0; _temp2381 != 0;
_temp2381=({ struct Cyc_List_List* _temp2383= _temp2381; if( _temp2383 == 0){
_throw( Null_Exception);} _temp2383->tl;}), cnt --){ struct Cyc_Absyn_Pat*
_temp2385=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2384= _temp2381;
if( _temp2384 == 0){ _throw( Null_Exception);} _temp2384->hd;}); goto _LL2386;
_LL2386: { struct _tuple0* _temp2387= Cyc_Toc_temp_var(); goto _LL2388; _LL2388: {
void* _temp2390=( void*)({ struct Cyc_Core_Opt* _temp2389= _temp2385->topt; if(
_temp2389 == 0){ _throw( Null_Exception);} _temp2389->v;}); goto _LL2391;
_LL2391: { void* _temp2392= Cyc_Toc_typ_to_c( _temp2390); goto _LL2393; _LL2393: {
struct _tagged_string* _temp2394= Cyc_Toc_fresh_label(); goto _LL2395; _LL2395:
decls=({ struct Cyc_List_List* _temp2396=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2396->hd=( void*)({ struct _tuple14*
_temp2397=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2397->f1=
_temp2387; _temp2397->f2= _temp2392; _temp2397;}); _temp2396->tl= decls;
_temp2396;});{ struct _tuple13 _temp2399= Cyc_Toc_xlate_pat( nv, _temp2390, Cyc_Absyn_var_exp(
_temp2387, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string*
_temp2398=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2398[ 0]= xprintf("f%d", cnt); _temp2398;}), 0), _temp2385, succ_lab,
fail_lab, decls); goto _LL2400; _LL2400: nv= _temp2399.f1; decls= _temp2399.f2;{
struct Cyc_Absyn_Stmt* _temp2401= _temp2399.f3; goto _LL2402; _LL2402: { struct
Cyc_Absyn_Stmt* _temp2404= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2387,
0), Cyc_Absyn_cast_exp( _temp2392, Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp2403=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2403[ 0]= xprintf("f%d", cnt); _temp2403;}), 0), 0),
0); goto _LL2405; _LL2405: { struct Cyc_Absyn_Stmt* _temp2406= Cyc_Absyn_seq_stmt(
_temp2404, _temp2401, 0); goto _LL2407; _LL2407: ss=({ struct Cyc_List_List*
_temp2408=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2408->hd=( void*) Cyc_Absyn_label_stmt( _temp2394, _temp2406, 0); _temp2408->tl=
ss; _temp2408;}); succ_lab= _temp2394;}}}}}}}}}}{ struct Cyc_Absyn_Exp* test_exp;
if( _temp2339->is_xtunion){ struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp(
e2, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2337->name, 0);
test_exp= Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e, 0), Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);}
else{ unsigned int max_tag= 0;{ struct Cyc_List_List* _temp2410=( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2409= _temp2339->fields; if( _temp2409 == 0){ _throw(
Null_Exception);} _temp2409->v;}); goto _LL2411; _LL2411: for( 0; _temp2410 != 0;
_temp2410=({ struct Cyc_List_List* _temp2412= _temp2410; if( _temp2412 == 0){
_throw( Null_Exception);} _temp2412->tl;})){ struct Cyc_Absyn_Tunionfield*
_temp2414=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2413=
_temp2410; if( _temp2413 == 0){ _throw( Null_Exception);} _temp2413->hd;}); goto
_LL2415; _LL2415: if( _temp2414->typs == 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp2337->name, 0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL2249;}} _LL2271: _temp2349= _temp2341; goto _LL2273; _LL2273: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2349);{ struct Cyc_List_List* _temp2418=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp2349); goto _LL2419; _LL2419: for( 0;
_temp2418 != 0; _temp2418=({ struct Cyc_List_List* _temp2420= _temp2418; if(
_temp2420 == 0){ _throw( Null_Exception);} _temp2420->tl;}), cnt --){ struct Cyc_Absyn_Pat*
_temp2422=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2421= _temp2418;
if( _temp2421 == 0){ _throw( Null_Exception);} _temp2421->hd;}); goto _LL2423;
_LL2423: { struct _tuple0* _temp2424= Cyc_Toc_temp_var(); goto _LL2425; _LL2425: {
void* _temp2427=( void*)({ struct Cyc_Core_Opt* _temp2426= _temp2422->topt; if(
_temp2426 == 0){ _throw( Null_Exception);} _temp2426->v;}); goto _LL2428;
_LL2428: { struct _tagged_string* _temp2429= Cyc_Toc_fresh_label(); goto _LL2430;
_LL2430: decls=({ struct Cyc_List_List* _temp2431=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2431->hd=( void*)({ struct
_tuple14* _temp2432=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp2432->f1= _temp2424; _temp2432->f2= Cyc_Toc_typ_to_c( _temp2427); _temp2432;});
_temp2431->tl= decls; _temp2431;});{ struct _tuple13 _temp2434= Cyc_Toc_xlate_pat(
nv, _temp2427, Cyc_Absyn_var_exp( _temp2424, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp2433=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2433[ 0]= xprintf("f%d", cnt);
_temp2433;}), 0), _temp2422, succ_lab, fail_lab, decls); goto _LL2435; _LL2435:
nv= _temp2434.f1; decls= _temp2434.f2;{ struct Cyc_Absyn_Stmt* _temp2436=
_temp2434.f3; goto _LL2437; _LL2437: { struct Cyc_Absyn_Stmt* _temp2439= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2424, 0), Cyc_Absyn_structmember_exp(
r,({ struct _tagged_string* _temp2438=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2438[ 0]= xprintf("f%d", cnt);
_temp2438;}), 0), 0), _temp2436, 0); goto _LL2440; _LL2440: ss=({ struct Cyc_List_List*
_temp2441=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2441->hd=( void*) Cyc_Absyn_label_stmt( _temp2429, _temp2439, 0); _temp2441->tl=
ss; _temp2441;}); succ_lab= _temp2429;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL2249;} _LL2275: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2442=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2351); goto _LL2443; _LL2443: for( 0; _temp2442 != 0; _temp2442=({ struct
Cyc_List_List* _temp2444= _temp2442; if( _temp2444 == 0){ _throw( Null_Exception);}
_temp2444->tl;})){ struct _tuple15* _temp2446=( struct _tuple15*)({ struct Cyc_List_List*
_temp2445= _temp2442; if( _temp2445 == 0){ _throw( Null_Exception);} _temp2445->hd;});
goto _LL2447; _LL2447: { struct Cyc_Absyn_Pat* _temp2448=(* _temp2446).f2; goto
_LL2449; _LL2449: { struct _tagged_string* f;{ void* _temp2451=( void*)({ struct
Cyc_List_List* _temp2450=(* _temp2446).f1; if( _temp2450 == 0){ _throw(
Null_Exception);} _temp2450->hd;}); struct _tagged_string* _temp2457; _LL2453:
if(*(( int*) _temp2451) == Cyc_Absyn_FieldName){ _LL2458: _temp2457=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp2451)->f1; goto
_LL2454;} else{ goto _LL2455;} _LL2455: goto _LL2456; _LL2454: f= _temp2457;
goto _LL2452; _LL2456:( void) _throw(( void*) Cyc_Toc_Match_error); _LL2452:;}{
struct _tuple0* _temp2459= Cyc_Toc_temp_var(); goto _LL2460; _LL2460: { void*
_temp2462=( void*)({ struct Cyc_Core_Opt* _temp2461= _temp2448->topt; if(
_temp2461 == 0){ _throw( Null_Exception);} _temp2461->v;}); goto _LL2463;
_LL2463: { void* _temp2464= Cyc_Toc_typ_to_c( _temp2462); goto _LL2465; _LL2465: {
struct _tagged_string* _temp2466= Cyc_Toc_fresh_label(); goto _LL2467; _LL2467:
decls=({ struct Cyc_List_List* _temp2468=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2468->hd=( void*)({ struct _tuple14*
_temp2469=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2469->f1=
_temp2459; _temp2469->f2= _temp2464; _temp2469;}); _temp2468->tl= decls;
_temp2468;});{ struct _tuple13 _temp2470= Cyc_Toc_xlate_pat( nv, _temp2462, Cyc_Absyn_var_exp(
_temp2459, 0), Cyc_Absyn_structmember_exp( path, f, 0), _temp2448, succ_lab,
fail_lab, decls); goto _LL2471; _LL2471: nv= _temp2470.f1; decls= _temp2470.f2;{
struct Cyc_Absyn_Stmt* _temp2472= _temp2470.f3; goto _LL2473; _LL2473: { struct
Cyc_Absyn_Stmt* _temp2474= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2459, 0), Cyc_Absyn_cast_exp( _temp2464, Cyc_Absyn_structmember_exp( r, f,
0), 0), 0), _temp2472, 0); goto _LL2475; _LL2475: ss=({ struct Cyc_List_List*
_temp2476=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2476->hd=( void*) Cyc_Absyn_label_stmt( _temp2466, _temp2474, 0); _temp2476->tl=
ss; _temp2476;}); succ_lab= _temp2466;}}}}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL2249;} _LL2277: { struct _tuple0* _temp2477= Cyc_Toc_temp_var(); goto
_LL2478; _LL2478: { void* _temp2480=( void*)({ struct Cyc_Core_Opt* _temp2479=
_temp2359->topt; if( _temp2479 == 0){ _throw( Null_Exception);} _temp2479->v;});
goto _LL2481; _LL2481: decls=({ struct Cyc_List_List* _temp2482=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2482->hd=( void*)({ struct
_tuple14* _temp2483=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp2483->f1= _temp2477; _temp2483->f2= Cyc_Toc_typ_to_c( _temp2480); _temp2483;});
_temp2482->tl= decls; _temp2482;});{ struct _tuple13 _temp2484= Cyc_Toc_xlate_pat(
nv, _temp2480, Cyc_Absyn_var_exp( _temp2477, 0), Cyc_Absyn_deref_exp( path, 0),
_temp2359, succ_lab, fail_lab, decls); goto _LL2485; _LL2485: nv= _temp2484.f1;
decls= _temp2484.f2;{ struct Cyc_Absyn_Stmt* _temp2486= _temp2484.f3; goto
_LL2487; _LL2487: { struct Cyc_Absyn_Stmt* _temp2488= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2477, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2486, 0);
goto _LL2489; _LL2489: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2488, 0);} else{ s= _temp2488;} goto _LL2249;}}}}} _LL2279: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp2490=( char*)"unknownid"; struct _tagged_string
_temp2491; _temp2491.curr= _temp2490; _temp2491.base= _temp2490; _temp2491.last_plus_one=
_temp2490 + 10; _temp2491;})); goto _LL2249; _LL2281: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp2492=( char*)"unknowncall"; struct _tagged_string _temp2493; _temp2493.curr=
_temp2492; _temp2493.base= _temp2492; _temp2493.last_plus_one= _temp2492 + 12;
_temp2493;})); goto _LL2249; _LL2283: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2494=(
char*)"unknownfields"; struct _tagged_string _temp2495; _temp2495.curr=
_temp2494; _temp2495.base= _temp2494; _temp2495.last_plus_one= _temp2494 + 14;
_temp2495;})); goto _LL2249; _LL2249:;} return({ struct _tuple13 _temp2496;
_temp2496.f1= nv; _temp2496.f2= decls; _temp2496.f3= s; _temp2496;});} struct
_tuple16{ struct _tagged_string* f1; struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple16* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause*
sc){ return({ struct _tuple16* _temp2497=( struct _tuple16*) GC_malloc( sizeof(
struct _tuple16)); _temp2497->f1= Cyc_Toc_fresh_label(); _temp2497->f2= Cyc_Toc_fresh_label();
_temp2497->f3= sc; _temp2497;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp2499=( void*)({ struct Cyc_Core_Opt*
_temp2498= e->topt; if( _temp2498 == 0){ _throw( Null_Exception);} _temp2498->v;});
goto _LL2500; _LL2500: { int leave_as_switch;{ void* _temp2501= Cyc_Tcutil_compress(
_temp2499); void* _temp2507; void* _temp2509; _LL2503: if(( unsigned int)
_temp2501 > 4u?*(( int*) _temp2501) == Cyc_Absyn_IntType: 0){ _LL2510: _temp2509=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2501)->f1; goto _LL2508; _LL2508:
_temp2507=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2501)->f2; goto
_LL2504;} else{ goto _LL2505;} _LL2505: goto _LL2506; _LL2504: leave_as_switch=
1; goto _LL2502; _LL2506: leave_as_switch= 0; goto _LL2502; _LL2502:;}{ struct
Cyc_List_List* _temp2511= scs; goto _LL2512; _LL2512: for( 0; _temp2511 != 0;
_temp2511=({ struct Cyc_List_List* _temp2513= _temp2511; if( _temp2513 == 0){
_throw( Null_Exception);} _temp2513->tl;})){ if(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2515=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List*
_temp2514= _temp2511; if( _temp2514 == 0){ _throw( Null_Exception);} _temp2514->hd;}))->pat_vars;
if( _temp2515 == 0){ _throw( Null_Exception);} _temp2515->v;}) != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2516= scs; goto
_LL2517; _LL2517: for( 0; _temp2516 != 0; _temp2516=({ struct Cyc_List_List*
_temp2518= _temp2516; if( _temp2518 == 0){ _throw( Null_Exception);} _temp2518->tl;})){
struct Cyc_Absyn_Stmt* _temp2520=(( struct Cyc_Absyn_Switch_clause*)({ struct
Cyc_List_List* _temp2519= _temp2516; if( _temp2519 == 0){ _throw( Null_Exception);}
_temp2519->hd;}))->body; goto _LL2521; _LL2521:(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2522= _temp2516; if( _temp2522 == 0){ _throw(
Null_Exception);} _temp2522->hd;}))->body= Cyc_Absyn_label_stmt( next_l,
_temp2520, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp2520);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_string* end_l= Cyc_Toc_fresh_label();
struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)( struct _tuple16*(* f)(
struct Cyc_Absyn_Switch_clause*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label,
scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0; struct
Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2523= lscs; goto _LL2524;
_LL2524: for( 0; _temp2523 != 0; _temp2523=({ struct Cyc_List_List* _temp2525=
_temp2523; if( _temp2525 == 0){ _throw( Null_Exception);} _temp2525->tl;})){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2545= _temp2523; if( _temp2545 == 0){ _throw( Null_Exception);} _temp2545->hd;}))).f3;
struct _tagged_string* fail_lab=({ struct Cyc_List_List* _temp2542= _temp2523;
if( _temp2542 == 0){ _throw( Null_Exception);} _temp2542->tl;}) == 0? end_l:(*((
struct _tuple16*)({ struct Cyc_List_List* _temp2544=({ struct Cyc_List_List*
_temp2543= _temp2523; if( _temp2543 == 0){ _throw( Null_Exception);} _temp2543->tl;});
if( _temp2544 == 0){ _throw( Null_Exception);} _temp2544->hd;}))).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2541= _temp2523; if( _temp2541 == 0){ _throw( Null_Exception);} _temp2541->hd;}))).f2;
if( sc->where_clause == 0){ struct _tuple13 _temp2526= Cyc_Toc_xlate_pat( nv,
_temp2499, r, path, sc->pattern, succ_lab, fail_lab, decls); goto _LL2527;
_LL2527: nvs=({ struct Cyc_List_List* _temp2528=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2528->hd=( void*) _temp2526.f1;
_temp2528->tl= nvs; _temp2528;}); decls= _temp2526.f2; test_stmts=({ struct Cyc_List_List*
_temp2529=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2529->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2530= _temp2523; if( _temp2530 == 0){ _throw( Null_Exception);} _temp2530->hd;}))).f1,
_temp2526.f3, 0); _temp2529->tl= test_stmts; _temp2529;});} else{ struct Cyc_Absyn_Exp*
_temp2532=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2531= sc->where_clause;
if( _temp2531 == 0){ _throw( Null_Exception);} _temp2531;}); goto _LL2533;
_LL2533: { struct _tagged_string* _temp2534= Cyc_Toc_fresh_label(); goto _LL2535;
_LL2535: { struct _tuple13 _temp2536= Cyc_Toc_xlate_pat( nv, _temp2499, r, path,
sc->pattern, _temp2534, fail_lab, decls); goto _LL2537; _LL2537: Cyc_Toc_exp_to_c(
_temp2536.f1, _temp2532); nvs=({ struct Cyc_List_List* _temp2538=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2538->hd=( void*) _temp2536.f1;
_temp2538->tl= nvs; _temp2538;}); decls= _temp2536.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2532, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2534, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2539=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2539->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2540= _temp2523; if( _temp2540 ==
0){ _throw( Null_Exception);} _temp2540->hd;}))).f1, Cyc_Absyn_seq_stmt(
_temp2536.f3, s3, 0), 0); _temp2539->tl= test_stmts; _temp2539;});}}}}}}{ struct
Cyc_Absyn_Stmt* _temp2546= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( test_stmts), 0); goto _LL2547;
_LL2547: nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){
for( 0;({ struct Cyc_List_List* _temp2548= lscs; if( _temp2548 == 0){ _throw(
Null_Exception);} _temp2548->tl;}) != 0; lscs=({ struct Cyc_List_List* _temp2549=
lscs; if( _temp2549 == 0){ _throw( Null_Exception);} _temp2549->tl;}), nvs=({
struct Cyc_List_List* _temp2550= nvs; if( _temp2550 == 0){ _throw(
Null_Exception);} _temp2550->tl;})){ struct Cyc_Toc_Env* _temp2552=( struct Cyc_Toc_Env*)({
struct Cyc_List_List* _temp2551= nvs; if( _temp2551 == 0){ _throw(
Null_Exception);} _temp2551->hd;}); goto _LL2553; _LL2553: { struct Cyc_Absyn_Stmt*
s=((*(( struct _tuple16*)({ struct Cyc_List_List* _temp2562= lscs; if( _temp2562
== 0){ _throw( Null_Exception);} _temp2562->hd;}))).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_non_last_switchclause_env( _temp2552, end_l,(*(( struct _tuple16*)({
struct Cyc_List_List* _temp2555=({ struct Cyc_List_List* _temp2554= lscs; if(
_temp2554 == 0){ _throw( Null_Exception);} _temp2554->tl;}); if( _temp2555 == 0){
_throw( Null_Exception);} _temp2555->hd;}))).f2,( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2558=((*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2557=({ struct Cyc_List_List* _temp2556= lscs; if( _temp2556 == 0){ _throw(
Null_Exception);} _temp2556->tl;}); if( _temp2557 == 0){ _throw( Null_Exception);}
_temp2557->hd;}))).f3)->pat_vars; if( _temp2558 == 0){ _throw( Null_Exception);}
_temp2558->v;}),( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2560=({
struct Cyc_List_List* _temp2559= nvs; if( _temp2559 == 0){ _throw(
Null_Exception);} _temp2559->tl;}); if( _temp2560 == 0){ _throw( Null_Exception);}
_temp2560->hd;})), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2561= lscs; if( _temp2561 == 0){
_throw( Null_Exception);} _temp2561->hd;}))).f2, s, 0), 0);}}{ struct Cyc_Toc_Env*
_temp2564=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2563= nvs; if(
_temp2563 == 0){ _throw( Null_Exception);} _temp2563->hd;}); goto _LL2565;
_LL2565: { struct Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2567= lscs; if( _temp2567 == 0){ _throw( Null_Exception);} _temp2567->hd;}))).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2564, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*)({
struct Cyc_List_List* _temp2566= lscs; if( _temp2566 == 0){ _throw(
Null_Exception);} _temp2566->hd;}))).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2546, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=({
struct Cyc_List_List* _temp2568= decls; if( _temp2568 == 0){ _throw(
Null_Exception);} _temp2568->tl;})){ struct _tuple14 _temp2572; void* _temp2573;
struct _tuple0* _temp2575; struct _tuple14* _temp2570=( struct _tuple14*)({
struct Cyc_List_List* _temp2569= decls; if( _temp2569 == 0){ _throw(
Null_Exception);} _temp2569->hd;}); _temp2572=* _temp2570; _LL2576: _temp2575=
_temp2572.f1; goto _LL2574; _LL2574: _temp2573= _temp2572.f2; goto _LL2571;
_LL2571: res= Cyc_Absyn_declare_stmt( _temp2575, _temp2573, 0, res, 0);}( void*)(
whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( v, Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp2577= e->topt; if( _temp2577 == 0){ _throw(
Null_Exception);} _temp2577->v;})),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2578=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2578[ 0]=({ struct Cyc_List_List _temp2579; _temp2579.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2579.tl= 0; _temp2579;}); _temp2578;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2580=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2620; struct Cyc_Absyn_Stmt* _temp2622; struct Cyc_Absyn_Stmt*
_temp2624; struct Cyc_Absyn_Exp* _temp2626; struct Cyc_Absyn_Stmt* _temp2628;
struct Cyc_Absyn_Stmt* _temp2630; struct Cyc_Absyn_Exp* _temp2632; struct Cyc_Absyn_Stmt*
_temp2634; struct _tuple2 _temp2636; struct Cyc_Absyn_Stmt* _temp2638; struct
Cyc_Absyn_Exp* _temp2640; struct Cyc_Absyn_Stmt* _temp2642; struct Cyc_Absyn_Stmt*
_temp2644; struct Cyc_Absyn_Stmt* _temp2646; struct _tagged_string* _temp2648;
struct Cyc_Absyn_Stmt* _temp2650; struct _tuple2 _temp2652; struct Cyc_Absyn_Stmt*
_temp2654; struct Cyc_Absyn_Exp* _temp2656; struct _tuple2 _temp2658; struct Cyc_Absyn_Stmt*
_temp2660; struct Cyc_Absyn_Exp* _temp2662; struct Cyc_Absyn_Exp* _temp2664;
struct Cyc_List_List* _temp2666; struct Cyc_Absyn_Exp* _temp2668; struct Cyc_Absyn_Switch_clause**
_temp2670; struct Cyc_List_List* _temp2672; struct Cyc_Absyn_Stmt* _temp2674;
struct Cyc_Absyn_Decl* _temp2676; struct Cyc_Absyn_Stmt* _temp2678; struct Cyc_Absyn_Stmt*
_temp2680; struct Cyc_Absyn_Stmt* _temp2682; struct _tagged_string* _temp2684;
struct _tuple2 _temp2686; struct Cyc_Absyn_Stmt* _temp2688; struct Cyc_Absyn_Exp*
_temp2690; struct Cyc_Absyn_Stmt* _temp2692; struct Cyc_List_List* _temp2694;
struct Cyc_Absyn_Stmt* _temp2696; struct Cyc_Absyn_Stmt* _temp2698; struct Cyc_Absyn_Vardecl*
_temp2700; struct Cyc_Absyn_Tvar* _temp2702; _LL2582: if( _temp2580 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2583;} else{ goto _LL2584;} _LL2584: if((
unsigned int) _temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_Exp_s: 0){
_LL2621: _temp2620=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp2580)->f1; goto _LL2585;} else{ goto _LL2586;} _LL2586: if(( unsigned int)
_temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_Seq_s: 0){ _LL2625: _temp2624=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp2580)->f1; goto
_LL2623; _LL2623: _temp2622=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp2580)->f2; goto _LL2587;} else{ goto _LL2588;} _LL2588: if(( unsigned int)
_temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_Return_s: 0){ _LL2627:
_temp2626=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp2580)->f1; goto _LL2589;} else{ goto _LL2590;} _LL2590: if(( unsigned int)
_temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_IfThenElse_s: 0){ _LL2633:
_temp2632=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2580)->f1; goto _LL2631; _LL2631: _temp2630=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2580)->f2; goto _LL2629; _LL2629:
_temp2628=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2580)->f3; goto _LL2591;} else{ goto _LL2592;} _LL2592: if(( unsigned int)
_temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_While_s: 0){ _LL2637: _temp2636=(
struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp2580)->f1; _LL2641:
_temp2640= _temp2636.f1; goto _LL2639; _LL2639: _temp2638= _temp2636.f2; goto
_LL2635; _LL2635: _temp2634=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp2580)->f2; goto _LL2593;} else{ goto _LL2594;} _LL2594: if(( unsigned int)
_temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_Break_s: 0){ _LL2643: _temp2642=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp2580)->f1; goto
_LL2595;} else{ goto _LL2596;} _LL2596: if(( unsigned int) _temp2580 > 1u?*((
int*) _temp2580) == Cyc_Absyn_Continue_s: 0){ _LL2645: _temp2644=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp2580)->f1; goto _LL2597;} else{ goto
_LL2598;} _LL2598: if(( unsigned int) _temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_Goto_s:
0){ _LL2649: _temp2648=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp2580)->f1; goto _LL2647; _LL2647: _temp2646=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp2580)->f2; goto _LL2599;} else{ goto
_LL2600;} _LL2600: if(( unsigned int) _temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_For_s:
0){ _LL2665: _temp2664=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp2580)->f1; goto _LL2659; _LL2659: _temp2658=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp2580)->f2; _LL2663: _temp2662= _temp2658.f1; goto _LL2661; _LL2661:
_temp2660= _temp2658.f2; goto _LL2653; _LL2653: _temp2652=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp2580)->f3; _LL2657: _temp2656= _temp2652.f1;
goto _LL2655; _LL2655: _temp2654= _temp2652.f2; goto _LL2651; _LL2651: _temp2650=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp2580)->f4; goto
_LL2601;} else{ goto _LL2602;} _LL2602: if(( unsigned int) _temp2580 > 1u?*((
int*) _temp2580) == Cyc_Absyn_Switch_s: 0){ _LL2669: _temp2668=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp2580)->f1; goto _LL2667; _LL2667:
_temp2666=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp2580)->f2; goto _LL2603;} else{ goto _LL2604;} _LL2604: if(( unsigned int)
_temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_Fallthru_s: 0){ _LL2673:
_temp2672=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2580)->f1; goto _LL2671; _LL2671: _temp2670=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp2580)->f2; goto _LL2605;} else{ goto
_LL2606;} _LL2606: if(( unsigned int) _temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_Decl_s:
0){ _LL2677: _temp2676=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp2580)->f1; goto _LL2675; _LL2675: _temp2674=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp2580)->f2; goto _LL2607;} else{ goto
_LL2608;} _LL2608: if(( unsigned int) _temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_Cut_s:
0){ _LL2679: _temp2678=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp2580)->f1; goto _LL2609;} else{ goto _LL2610;} _LL2610: if(( unsigned int)
_temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_Splice_s: 0){ _LL2681:
_temp2680=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp2580)->f1; goto _LL2611;} else{ goto _LL2612;} _LL2612: if(( unsigned int)
_temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_Label_s: 0){ _LL2685: _temp2684=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp2580)->f1; goto
_LL2683; _LL2683: _temp2682=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp2580)->f2; goto _LL2613;} else{ goto _LL2614;} _LL2614: if(( unsigned int)
_temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_Do_s: 0){ _LL2693: _temp2692=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp2580)->f1; goto
_LL2687; _LL2687: _temp2686=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp2580)->f2; _LL2691: _temp2690= _temp2686.f1; goto _LL2689; _LL2689:
_temp2688= _temp2686.f2; goto _LL2615;} else{ goto _LL2616;} _LL2616: if((
unsigned int) _temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_TryCatch_s: 0){
_LL2697: _temp2696=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2580)->f1; goto _LL2695; _LL2695: _temp2694=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp2580)->f2; goto _LL2617;} else{ goto
_LL2618;} _LL2618: if(( unsigned int) _temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_Region_s:
0){ _LL2703: _temp2702=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2580)->f1; goto _LL2701; _LL2701: _temp2700=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp2580)->f2; goto _LL2699; _LL2699:
_temp2698=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2580)->f3; goto _LL2619;} else{ goto _LL2581;} _LL2583: return; _LL2585:
Cyc_Toc_exp_to_c( nv, _temp2620); return; _LL2587: Cyc_Toc_stmt_to_c( nv,
_temp2624); s= _temp2622; continue; _LL2589: { struct Cyc_Core_Opt* topt= 0; if(
_temp2626 != 0){ topt=({ struct Cyc_Core_Opt* _temp2704=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2704->v=( void*) Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp2706=({ struct Cyc_Absyn_Exp* _temp2705=
_temp2626; if( _temp2705 == 0){ _throw( Null_Exception);} _temp2705->topt;});
if( _temp2706 == 0){ _throw( Null_Exception);} _temp2706->v;})); _temp2704;});
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2707=
_temp2626; if( _temp2707 == 0){ _throw( Null_Exception);} _temp2707;}));} if( s->try_depth
> 0){ if( topt != 0){ struct _tuple0* _temp2708= Cyc_Toc_temp_var(); goto
_LL2709; _LL2709: { struct Cyc_Absyn_Stmt* _temp2710= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2708, 0), 0); goto _LL2711;
_LL2711:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( _temp2708,(
void*)({ struct Cyc_Core_Opt* _temp2712= topt; if( _temp2712 == 0){ _throw(
Null_Exception);} _temp2712->v;}), _temp2626, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp2710, 0), 0))->r));}} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL2591: Cyc_Toc_exp_to_c( nv, _temp2632); Cyc_Toc_stmt_to_c( nv,
_temp2630); s= _temp2628; continue; _LL2593: Cyc_Toc_exp_to_c( nv, _temp2640);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2634); return; _LL2595: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)({ struct Cyc_Core_Opt* _temp2713= nv->break_lab; if( _temp2713
== 0){ _throw( Null_Exception);} _temp2713->v;}), 0))->r));}{ int dest_depth=
_temp2642 == 0? 0:({ struct Cyc_Absyn_Stmt* _temp2714= _temp2642; if( _temp2714
== 0){ _throw( Null_Exception);} _temp2714->try_depth;}); Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2597: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)({
struct Cyc_Core_Opt* _temp2715= nv->continue_lab; if( _temp2715 == 0){ _throw(
Null_Exception);} _temp2715->v;}), 0))->r));} Cyc_Toc_do_npop_before( s->try_depth
-({ struct Cyc_Absyn_Stmt* _temp2716= _temp2644; if( _temp2716 == 0){ _throw(
Null_Exception);} _temp2716->try_depth;}), s); return; _LL2599: Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2717= _temp2646; if( _temp2717 == 0){
_throw( Null_Exception);} _temp2717->try_depth;}), s); return; _LL2601: Cyc_Toc_exp_to_c(
nv, _temp2664); Cyc_Toc_exp_to_c( nv, _temp2662); Cyc_Toc_exp_to_c( nv,
_temp2656); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2650); return;
_LL2603: Cyc_Toc_xlate_switch( nv, s, _temp2668, _temp2666); return; _LL2605:
if( nv->fallthru_info == 0){( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2718=( char*)"fallthru in unexpected place";
struct _tagged_string _temp2719; _temp2719.curr= _temp2718; _temp2719.base=
_temp2718; _temp2719.last_plus_one= _temp2718 + 29; _temp2719;})));}{ struct
_tuple8 _temp2723; struct Cyc_Dict_Dict* _temp2724; struct Cyc_List_List*
_temp2726; struct _tagged_string* _temp2728; struct _tuple8* _temp2721=( struct
_tuple8*)({ struct Cyc_Core_Opt* _temp2720= nv->fallthru_info; if( _temp2720 ==
0){ _throw( Null_Exception);} _temp2720->v;}); _temp2723=* _temp2721; _LL2729:
_temp2728= _temp2723.f1; goto _LL2727; _LL2727: _temp2726= _temp2723.f2; goto
_LL2725; _LL2725: _temp2724= _temp2723.f3; goto _LL2722; _LL2722: { struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_goto_stmt( _temp2728, 0); Cyc_Toc_do_npop_before( s->try_depth -(({
struct Cyc_Absyn_Switch_clause** _temp2730= _temp2670; if( _temp2730 == 0){
_throw( Null_Exception);}* _temp2730;})->body)->try_depth, s2);{ struct Cyc_List_List*
_temp2731=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2726); goto _LL2732; _LL2732: { struct Cyc_List_List* _temp2733=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp2672); goto
_LL2734; _LL2734: for( 0; _temp2731 != 0; _temp2731=({ struct Cyc_List_List*
_temp2735= _temp2731; if( _temp2735 == 0){ _throw( Null_Exception);} _temp2735->tl;}),
_temp2733=({ struct Cyc_List_List* _temp2736= _temp2733; if( _temp2736 == 0){
_throw( Null_Exception);} _temp2736->tl;})){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp2737= _temp2733; if( _temp2737 == 0){ _throw(
Null_Exception);} _temp2737->hd;})); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2724,( struct _tuple0*)({ struct Cyc_List_List* _temp2738= _temp2731; if(
_temp2738 == 0){ _throw( Null_Exception);} _temp2738->hd;})),( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp2739= _temp2733; if( _temp2739 == 0){ _throw(
Null_Exception);} _temp2739->hd;}), 0), s2, 0);}( void*)( s->r=( void*)(( void*)
s2->r)); return;}}}} _LL2607:{ void* _temp2740=( void*) _temp2676->r; int
_temp2748; struct Cyc_Absyn_Exp* _temp2750; struct Cyc_Core_Opt* _temp2752;
struct Cyc_Core_Opt* _temp2754; struct Cyc_Absyn_Pat* _temp2756; struct Cyc_Absyn_Vardecl*
_temp2758; _LL2742: if(*(( int*) _temp2740) == Cyc_Absyn_Let_d){ _LL2757:
_temp2756=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2740)->f1;
goto _LL2755; _LL2755: _temp2754=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2740)->f2; goto _LL2753; _LL2753: _temp2752=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2740)->f3; goto _LL2751; _LL2751: _temp2750=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2740)->f4; goto
_LL2749; _LL2749: _temp2748=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2740)->f5;
goto _LL2743;} else{ goto _LL2744;} _LL2744: if(*(( int*) _temp2740) == Cyc_Absyn_Var_d){
_LL2759: _temp2758=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2740)->f1; goto _LL2745;} else{ goto _LL2746;} _LL2746: goto _LL2747;
_LL2743:( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2756,
_temp2752, _temp2750, _temp2748, _temp2674))->r)); goto _LL2741; _LL2745: {
struct _tuple0* _temp2760= _temp2758->name; goto _LL2761; _LL2761:( void*)(
_temp2758->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2758->type));{
struct Cyc_Toc_Env* _temp2764= Cyc_Toc_add_varmap( nv, _temp2760, Cyc_Absyn_varb_exp(
_temp2760,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp2762=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2762[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2763; _temp2763.tag= Cyc_Absyn_Local_b; _temp2763.f1=
_temp2758; _temp2763;}); _temp2762;}), 0)); goto _LL2765; _LL2765: Cyc_Toc_stmt_to_c(
_temp2764, _temp2674); if( _temp2758->initializer != 0){ struct Cyc_Absyn_Exp*
init=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2778= _temp2758->initializer;
if( _temp2778 == 0){ _throw( Null_Exception);} _temp2778;}); void* _temp2766=(
void*) init->r; struct Cyc_Absyn_Exp* _temp2772; struct Cyc_Absyn_Exp* _temp2774;
struct Cyc_Absyn_Vardecl* _temp2776; _LL2768: if(*(( int*) _temp2766) == Cyc_Absyn_Comprehension_e){
_LL2777: _temp2776=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2766)->f1; goto _LL2775; _LL2775: _temp2774=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2766)->f2; goto _LL2773; _LL2773:
_temp2772=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2766)->f3; goto _LL2769;} else{ goto _LL2770;} _LL2770: goto _LL2771;
_LL2769: _temp2758->initializer= 0;( void*)( _temp2674->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2760, 0), _temp2776, _temp2774, _temp2772, Cyc_Absyn_new_stmt((
void*) _temp2674->r, 0), 0))->r)); goto _LL2767; _LL2771: Cyc_Toc_exp_to_c( nv,
init); goto _LL2767; _LL2767:;} goto _LL2741;}} _LL2747:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2779=(
char*)"bad nested declaration within function"; struct _tagged_string _temp2780;
_temp2780.curr= _temp2779; _temp2780.base= _temp2779; _temp2780.last_plus_one=
_temp2779 + 39; _temp2780;})); goto _LL2741; _LL2741:;} return; _LL2609:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp2781=( char*)"cut"; struct _tagged_string _temp2782; _temp2782.curr=
_temp2781; _temp2782.base= _temp2781; _temp2782.last_plus_one= _temp2781 + 4;
_temp2782;})); return; _LL2611:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2783=( char*)"splice"; struct _tagged_string
_temp2784; _temp2784.curr= _temp2783; _temp2784.base= _temp2783; _temp2784.last_plus_one=
_temp2783 + 7; _temp2784;})); return; _LL2613: s= _temp2682; continue; _LL2615:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2692); Cyc_Toc_exp_to_c( nv,
_temp2690); return; _LL2617: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct
_tuple0* e_var= Cyc_Toc_temp_var(); struct _tuple0* was_thrown_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2827=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2827->f1= Cyc_Toc_abs_ns; _temp2827->f2= Cyc_Toc__push_handler_sp;
_temp2827;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple0* _temp2826=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2826->f1= Cyc_Toc_abs_ns; _temp2826->f2= Cyc_Toc_setjmp_sp; _temp2826;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2825=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2825->f1=
Cyc_Toc_abs_ns; _temp2825->f2= Cyc_Toc__pop_handler_sp; _temp2825;}), 0); struct
Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp(({ struct _tuple0* _temp2824=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2824->f1= Cyc_Toc_abs_ns;
_temp2824->f2= Cyc_Toc__exn_thrown_sp; _temp2824;}), 0); e_exp->topt=({ struct
Cyc_Core_Opt* _temp2785=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2785->v=( void*) e_typ; _temp2785;}); nv= Cyc_Toc_add_varmap( nv, e_var,
e_exp); Cyc_Toc_stmt_to_c( nv, _temp2696);{ struct Cyc_Absyn_Stmt* _temp2786=
Cyc_Absyn_seq_stmt( _temp2696, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0); goto _LL2787; _LL2787: { struct _tuple0*
_temp2788= Cyc_Toc_temp_var(); goto _LL2789; _LL2789: { struct Cyc_Absyn_Exp*
_temp2790= Cyc_Absyn_var_exp( _temp2788, 0); goto _LL2791; _LL2791: { struct Cyc_Absyn_Vardecl*
_temp2792= Cyc_Absyn_new_vardecl( _temp2788, Cyc_Absyn_exn_typ, 0); goto _LL2793;
_LL2793: _temp2790->topt=({ struct Cyc_Core_Opt* _temp2794=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2794->v=( void*) Cyc_Absyn_exn_typ;
_temp2794;});{ struct Cyc_Absyn_Pat* _temp2799=({ struct Cyc_Absyn_Pat*
_temp2795=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof( struct Cyc_Absyn_Pat));
_temp2795->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp2797=(
struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2797[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2798; _temp2798.tag= Cyc_Absyn_Var_p;
_temp2798.f1= _temp2792; _temp2798;}); _temp2797;})); _temp2795->topt=({ struct
Cyc_Core_Opt* _temp2796=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2796->v=( void*) Cyc_Absyn_exn_typ; _temp2796;}); _temp2795->loc= 0;
_temp2795;}); goto _LL2800; _LL2800: { struct Cyc_Absyn_Exp* _temp2801= Cyc_Absyn_throw_exp(
_temp2790, 0); goto _LL2802; _LL2802: _temp2801->topt=({ struct Cyc_Core_Opt*
_temp2803=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2803->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2803;});{ struct Cyc_Absyn_Stmt*
_temp2804= Cyc_Absyn_exp_stmt( _temp2801, 0); goto _LL2805; _LL2805: { struct
Cyc_Absyn_Switch_clause* _temp2809=({ struct Cyc_Absyn_Switch_clause* _temp2806=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2806->pattern= _temp2799; _temp2806->pat_vars=({ struct Cyc_Core_Opt*
_temp2807=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2807->v=( void*)({ struct Cyc_List_List* _temp2808=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2808->hd=( void*) _temp2792;
_temp2808->tl= 0; _temp2808;}); _temp2807;}); _temp2806->where_clause= 0;
_temp2806->body= _temp2804; _temp2806->loc= 0; _temp2806;}); goto _LL2810;
_LL2810: { struct Cyc_Absyn_Stmt* _temp2812= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2694,({ struct Cyc_List_List* _temp2811=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2811->hd=( void*) _temp2809; _temp2811->tl=
0; _temp2811;})), 0); goto _LL2813; _LL2813: Cyc_Toc_stmt_to_c( nv, _temp2812);{
struct Cyc_Absyn_Exp* _temp2815= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2814=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2814->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2814->tl= 0; _temp2814;}), 0); goto _LL2816; _LL2816: { struct Cyc_Absyn_Stmt*
_temp2818= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2817=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2817->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2817->tl= 0;
_temp2817;}), 0), 0); goto _LL2819; _LL2819: { struct Cyc_Absyn_Exp* _temp2820=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2821; _LL2821: {
struct Cyc_Absyn_Exp* _temp2822= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2823; _LL2823:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2818, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2820, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2815, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2822, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2786, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2812, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2619: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__region_handle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2700->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2833=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2833[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2834; _temp2834.tag= Cyc_Absyn_Local_b; _temp2834.f1=
_temp2700; _temp2834;}); _temp2833;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2832=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2832->f1= Cyc_Toc_abs_ns; _temp2832->f2= Cyc_Toc__push_region_sp;
_temp2832;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2831=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2831->f1= Cyc_Toc_abs_ns; _temp2831->f2= Cyc_Toc__pop_region_sp; _temp2831;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2830=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2830->f1=
Cyc_Toc_abs_ns; _temp2830->f2= Cyc_Toc__new_region_sp; _temp2830;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2829=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2829->hd=( void*) addr_rh_exp; _temp2829->tl= 0; _temp2829;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2828=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2828->hd=( void*) x_exp; _temp2828->tl= 0; _temp2828;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2698);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2698, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2581:;}} struct _tuple17{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0;(
void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{ struct
Cyc_List_List* _temp2835= f->args; goto _LL2836; _LL2836: for( 0; _temp2835 != 0;
_temp2835=({ struct Cyc_List_List* _temp2837= _temp2835; if( _temp2837 == 0){
_throw( Null_Exception);} _temp2837->tl;})){(*(( struct _tuple17*)({ struct Cyc_List_List*
_temp2838= _temp2835; if( _temp2838 == 0){ _throw( Null_Exception);} _temp2838->hd;}))).f3=
Cyc_Toc_typ_to_c((*(( struct _tuple17*)({ struct Cyc_List_List* _temp2839=
_temp2835; if( _temp2839 == 0){ _throw( Null_Exception);} _temp2839->hd;}))).f3);{
struct _tuple0* _temp2842=({ struct _tuple0* _temp2840=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2840->f1=( void*) Cyc_Absyn_Loc_n;
_temp2840->f2=(*(( struct _tuple17*)({ struct Cyc_List_List* _temp2841=
_temp2835; if( _temp2841 == 0){ _throw( Null_Exception);} _temp2841->hd;}))).f1;
_temp2840;}); goto _LL2843; _LL2843: nv= Cyc_Toc_add_varmap( nv, _temp2842, Cyc_Absyn_var_exp(
_temp2842, 0));}}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2844= s; _LL2846: if(
_temp2844 ==( void*) Cyc_Absyn_Abstract){ goto _LL2847;} else{ goto _LL2848;}
_LL2848: if( _temp2844 ==( void*) Cyc_Absyn_ExternC){ goto _LL2849;} else{ goto
_LL2850;} _LL2850: goto _LL2851; _LL2847: return( void*) Cyc_Absyn_Public;
_LL2849: return( void*) Cyc_Absyn_Extern; _LL2851: return s; _LL2845:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2853=( struct _tuple0*)({
struct Cyc_Core_Opt* _temp2852= s->name; if( _temp2852 == 0){ _throw(
Null_Exception);} _temp2852->v;}); goto _LL2854; _LL2854: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2855=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2855->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2855;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2857=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2856= Cyc_Toc_structs_so_far; if( _temp2856 == 0){ _throw( Null_Exception);}
_temp2856->v;}),(* _temp2853).f2); goto _LL2858; _LL2858: if( _temp2857 == 0){
seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2859=
Cyc_Toc_structs_so_far; if( _temp2859 == 0){ _throw( Null_Exception);} _temp2859->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2860= Cyc_Toc_structs_so_far;
if( _temp2860 == 0){ _throw( Null_Exception);} _temp2860->v;}),(* _temp2853).f2,
s);});} else{ if((( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Core_Opt*
_temp2861= _temp2857; if( _temp2861 == 0){ _throw( Null_Exception);} _temp2861->v;}))->fields
== 0){( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2862= Cyc_Toc_structs_so_far;
if( _temp2862 == 0){ _throw( Null_Exception);} _temp2862->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2863= Cyc_Toc_structs_so_far; if( _temp2863 == 0){ _throw(
Null_Exception);} _temp2863->v;}),(* _temp2853).f2, s);}); seen_defn_before= 0;}
else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==( void*)
Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) s->sc
==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2864=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2864->v=( void*)
_temp2853; _temp2864;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2866=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2865= s->fields; if( _temp2865 == 0){ _throw( Null_Exception);} _temp2865->v;});
goto _LL2867; _LL2867: for( 0; _temp2866 != 0; _temp2866=({ struct Cyc_List_List*
_temp2868= _temp2866; if( _temp2868 == 0){ _throw( Null_Exception);} _temp2868->tl;})){(
void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp2869=
_temp2866; if( _temp2869 == 0){ _throw( Null_Exception);} _temp2869->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2870= _temp2866; if( _temp2870 == 0){ _throw( Null_Exception);}
_temp2870->hd;}))->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp2872=( struct _tuple0*)({ struct Cyc_Core_Opt* _temp2871= u->name;
if( _temp2871 == 0){ _throw( Null_Exception);} _temp2871->v;}); goto _LL2873;
_LL2873: if( Cyc_Toc_unions_so_far == 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp2874=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2874->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2874;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2876=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2875= Cyc_Toc_unions_so_far;
if( _temp2875 == 0){ _throw( Null_Exception);} _temp2875->v;}),(* _temp2872).f2);
goto _LL2877; _LL2877: if( _temp2876 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2878= Cyc_Toc_unions_so_far; if( _temp2878 == 0){
_throw( Null_Exception);} _temp2878->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2879= Cyc_Toc_unions_so_far; if( _temp2879 == 0){ _throw( Null_Exception);}
_temp2879->v;}),(* _temp2872).f2, u);});} else{ if((( struct Cyc_Absyn_Uniondecl*)({
struct Cyc_Core_Opt* _temp2880= _temp2876; if( _temp2880 == 0){ _throw(
Null_Exception);} _temp2880->v;}))->fields == 0){( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2881= Cyc_Toc_unions_so_far; if( _temp2881 == 0){
_throw( Null_Exception);} _temp2881->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2882= Cyc_Toc_unions_so_far; if( _temp2882 == 0){ _throw( Null_Exception);}
_temp2882->v;}),(* _temp2872).f2, u);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}}( void*)( u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) u->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2883=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2883->v=( void*) _temp2872; _temp2883;}); if( u->fields !=
0){ if( seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* _temp2885=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2884= u->fields; if(
_temp2884 == 0){ _throw( Null_Exception);} _temp2884->v;}); goto _LL2886;
_LL2886: for( 0; _temp2885 != 0; _temp2885=({ struct Cyc_List_List* _temp2887=
_temp2885; if( _temp2887 == 0){ _throw( Null_Exception);} _temp2887->tl;})){(
void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp2888=
_temp2885; if( _temp2888 == 0){ _throw( Null_Exception);} _temp2888->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2889= _temp2885; if( _temp2889 == 0){ _throw( Null_Exception);}
_temp2889->hd;}))->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2890=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2890->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2890;});}{ struct
_tuple0* _temp2891= tud->name; goto _LL2892; _LL2892: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp2893= Cyc_Toc_tunions_so_far;
if( _temp2893 == 0){ _throw( Null_Exception);} _temp2893->v;}), _temp2891)){
return;}( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp2894= Cyc_Toc_tunions_so_far;
if( _temp2894 == 0){ _throw( Null_Exception);} _temp2894->v=( void*)(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_insert)((
struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp2895= Cyc_Toc_tunions_so_far;
if( _temp2895 == 0){ _throw( Null_Exception);} _temp2895->v;}), _temp2891);});{
int tag_count= 0; int box_count= 0; struct Cyc_List_List* _temp2897=( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2896= tud->fields; if( _temp2896 == 0){ _throw(
Null_Exception);} _temp2896->v;}); goto _LL2898; _LL2898: for( 0; _temp2897 != 0;
_temp2897=({ struct Cyc_List_List* _temp2899= _temp2897; if( _temp2899 == 0){
_throw( Null_Exception);} _temp2899->tl;})){ struct Cyc_Absyn_Tunionfield* f=(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2934= _temp2897; if(
_temp2934 == 0){ _throw( Null_Exception);} _temp2934->hd;}); if( f->typs == 0){
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
tag_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2900; _temp2900.q_const=
1; _temp2900.q_volatile= 0; _temp2900.q_restrict= 0; _temp2900;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2901=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2901->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2902=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2902[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2903; _temp2903.tag= Cyc_Absyn_Var_d; _temp2903.f1= vd; _temp2903;});
_temp2902;}), 0); _temp2901->tl= Cyc_Toc_result_decls; _temp2901;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
box_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2904; _temp2904.q_const=
1; _temp2904.q_volatile= 0; _temp2904.q_restrict= 0; _temp2904;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2905=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2905->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2906=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2906[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2907; _temp2907.tag= Cyc_Absyn_Var_d; _temp2907.f1= vd; _temp2907;});
_temp2906;}), 0); _temp2905->tl= Cyc_Toc_result_decls; _temp2905;});{ struct Cyc_List_List*
_temp2908= 0; goto _LL2909; _LL2909: { int i= 1;{ struct Cyc_List_List*
_temp2910= f->typs; goto _LL2911; _LL2911: for( 0; _temp2910 != 0; _temp2910=({
struct Cyc_List_List* _temp2912= _temp2910; if( _temp2912 == 0){ _throw(
Null_Exception);} _temp2912->tl;}), i ++){ struct _tagged_string* _temp2914=(
struct _tagged_string*)({ struct _tagged_string* _temp2913=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp2913[ 0]=
xprintf("f%d", i); _temp2913;}); goto _LL2915; _LL2915: { struct Cyc_Absyn_Structfield*
_temp2919=({ struct Cyc_Absyn_Structfield* _temp2916=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2916->name= _temp2914;
_temp2916->tq=(*(( struct _tuple3*)({ struct Cyc_List_List* _temp2918= _temp2910;
if( _temp2918 == 0){ _throw( Null_Exception);} _temp2918->hd;}))).f1; _temp2916->type=(
void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp2917= _temp2910; if( _temp2917 == 0){ _throw( Null_Exception);} _temp2917->hd;}))).f2);
_temp2916->width= 0; _temp2916->attributes= 0; _temp2916;}); goto _LL2920;
_LL2920: _temp2908=({ struct Cyc_List_List* _temp2921=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2921->hd=( void*) _temp2919;
_temp2921->tl= _temp2908; _temp2921;});}}} _temp2908=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2908); _temp2908=({ struct Cyc_List_List*
_temp2922=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2922->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2923=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2923->name= Cyc_Toc_tag_sp;
_temp2923->tq= Cyc_Toc_mt_tq; _temp2923->type=( void*) Cyc_Absyn_sint_t;
_temp2923->width= 0; _temp2923->attributes= 0; _temp2923;}); _temp2922->tl=
_temp2908; _temp2922;});{ struct Cyc_Absyn_Structdecl* _temp2929=({ struct Cyc_Absyn_Structdecl*
_temp2924=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2924->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2924->name=({ struct Cyc_Core_Opt*
_temp2926=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2926->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2927=( char*)"_struct"; struct _tagged_string _temp2928; _temp2928.curr=
_temp2927; _temp2928.base= _temp2927; _temp2928.last_plus_one= _temp2927 + 8;
_temp2928;})); _temp2926;}); _temp2924->tvs= 0; _temp2924->fields=({ struct Cyc_Core_Opt*
_temp2925=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2925->v=( void*) _temp2908; _temp2925;}); _temp2924->attributes= 0;
_temp2924;}); goto _LL2930; _LL2930: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2931=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2931->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2932=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2932[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2933; _temp2933.tag= Cyc_Absyn_Struct_d;
_temp2933.f1= _temp2929; _temp2933;}); _temp2932;}), 0); _temp2931->tl= Cyc_Toc_result_decls;
_temp2931;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2935=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2935->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2935;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2936= xd->name; goto _LL2937; _LL2937: {
struct Cyc_List_List* _temp2939=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2938= xd->fields; if( _temp2938 == 0){ _throw( Null_Exception);} _temp2938->v;});
goto _LL2940; _LL2940: for( 0; _temp2939 != 0; _temp2939=({ struct Cyc_List_List*
_temp2941= _temp2939; if( _temp2941 == 0){ _throw( Null_Exception);} _temp2941->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp2987= _temp2939; if( _temp2987 == 0){ _throw( Null_Exception);} _temp2987->hd;});
struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp* _temp2943= Cyc_Absyn_uint_exp(({
struct _tagged_string _temp2942=* fn;( unsigned int)( _temp2942.last_plus_one -
_temp2942.curr);}) + 4, 0); goto _LL2944; _LL2944: { struct Cyc_Absyn_ArrayType_struct*
_temp2947=({ struct Cyc_Absyn_ArrayType_struct* _temp2945=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2945[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2946; _temp2946.tag= Cyc_Absyn_ArrayType;
_temp2946.f1=( void*) Cyc_Absyn_uchar_t; _temp2946.f2= Cyc_Toc_mt_tq; _temp2946.f3=(
struct Cyc_Absyn_Exp*) _temp2943; _temp2946;}); _temp2945;}); goto _LL2948;
_LL2948: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) xd->sc !=( void*) Cyc_Absyn_Extern?
!(( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp2949= Cyc_Toc_xtunion_fields_so_far;
if( _temp2949 == 0){ _throw( Null_Exception);} _temp2949->v;}), f->name): 0){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(({ int _temp2950= 0; int
_temp2951= 0; int _temp2952= 0; int _temp2953= 0; struct _tagged_string
_temp2954=* fn; xprintf("%c%c%c%c%.*s", _temp2950, _temp2951, _temp2952,
_temp2953, _temp2954.last_plus_one - _temp2954.curr, _temp2954.curr);}), 0);}{
struct Cyc_Absyn_Vardecl* _temp2955= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2947, initopt); goto _LL2956; _LL2956:( void*)( _temp2955->sc=( void*)((
void*) xd->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2957=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2957->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2958=(
struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2958[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2959; _temp2959.tag= Cyc_Absyn_Var_d;
_temp2959.f1= _temp2955; _temp2959;}); _temp2958;}), 0); _temp2957->tl= Cyc_Toc_result_decls;
_temp2957;}); if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp2960= Cyc_Toc_xtunion_fields_so_far;
if( _temp2960 == 0){ _throw( Null_Exception);} _temp2960->v;}), f->name)){
continue;}( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp2961= Cyc_Toc_xtunion_fields_so_far;
if( _temp2961 == 0){ _throw( Null_Exception);} _temp2961->v=( void*)(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_insert)((
struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp2962= Cyc_Toc_xtunion_fields_so_far;
if( _temp2962 == 0){ _throw( Null_Exception);} _temp2962->v;}), f->name);}); if(
f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp2963= f->typs; goto _LL2964; _LL2964: for( 0; _temp2963 != 0; _temp2963=({
struct Cyc_List_List* _temp2965= _temp2963; if( _temp2965 == 0){ _throw(
Null_Exception);} _temp2965->tl;}), i ++){ struct _tagged_string* _temp2967=(
struct _tagged_string*)({ struct _tagged_string* _temp2966=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2966[ 0]=
xprintf("f%d", i); _temp2966;}); goto _LL2968; _LL2968: { struct Cyc_Absyn_Structfield*
_temp2972=({ struct Cyc_Absyn_Structfield* _temp2969=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2969->name= _temp2967;
_temp2969->tq=(*(( struct _tuple3*)({ struct Cyc_List_List* _temp2971= _temp2963;
if( _temp2971 == 0){ _throw( Null_Exception);} _temp2971->hd;}))).f1; _temp2969->type=(
void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp2970= _temp2963; if( _temp2970 == 0){ _throw( Null_Exception);} _temp2970->hd;}))).f2);
_temp2969->width= 0; _temp2969->attributes= 0; _temp2969;}); goto _LL2973;
_LL2973: fields=({ struct Cyc_List_List* _temp2974=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2974->hd=( void*) _temp2972;
_temp2974->tl= fields; _temp2974;});}}} fields=({ struct Cyc_List_List*
_temp2975=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2975->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2976=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2976->name= Cyc_Toc_tag_sp;
_temp2976->tq= Cyc_Toc_mt_tq; _temp2976->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2976->width= 0; _temp2976->attributes= 0; _temp2976;});
_temp2975->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2975;});{ struct Cyc_Absyn_Structdecl* _temp2982=({ struct Cyc_Absyn_Structdecl*
_temp2977=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2977->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2977->name=({ struct Cyc_Core_Opt*
_temp2979=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2979->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2980=( char*)"_struct"; struct _tagged_string _temp2981; _temp2981.curr=
_temp2980; _temp2981.base= _temp2980; _temp2981.last_plus_one= _temp2980 + 8;
_temp2981;})); _temp2979;}); _temp2977->tvs= 0; _temp2977->fields=({ struct Cyc_Core_Opt*
_temp2978=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2978->v=( void*) fields; _temp2978;}); _temp2977->attributes= 0; _temp2977;});
goto _LL2983; _LL2983: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2984=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2984->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2985=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2985[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2986; _temp2986.tag= Cyc_Absyn_Struct_d;
_temp2986.f1= _temp2982; _temp2986;}); _temp2985;}), 0); _temp2984->tl= Cyc_Toc_result_decls;
_temp2984;});}}}}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2988= ed->name; goto
_LL2989; _LL2989:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2991=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2990= ed->fields; if( _temp2990 == 0){ _throw( Null_Exception);} _temp2990->v;});
goto _LL2992; _LL2992: for( 0; _temp2991 != 0; _temp2991=({ struct Cyc_List_List*
_temp2993= _temp2991; if( _temp2993 == 0){ _throw( Null_Exception);} _temp2993->tl;})){
if((( struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp2994= _temp2991;
if( _temp2994 == 0){ _throw( Null_Exception);} _temp2994->hd;}))->tag != 0){
unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp2997=(( struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List*
_temp2996= _temp2991; if( _temp2996 == 0){ _throw( Null_Exception);} _temp2996->hd;}))->tag;
if( _temp2997 == 0){ _throw( Null_Exception);} _temp2997;}));(( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2995= _temp2991; if( _temp2995 == 0){ _throw(
Null_Exception);} _temp2995->hd;}))->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)({ struct Cyc_Core_Opt*
_temp3022= topt; if( _temp3022 == 0){ _throw( Null_Exception);} _temp3022->v;});
Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var(); struct
_tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string* fail_lab=
exhaust? succ_lab: Cyc_Toc_fresh_label(); struct Cyc_Absyn_Stmt* _temp3000;
struct Cyc_List_List* _temp3002; struct Cyc_Toc_Env* _temp3004; struct _tuple13
_temp2998= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp(
x, 0), p, succ_lab, fail_lab, 0); _LL3005: _temp3004= _temp2998.f1; goto _LL3003;
_LL3003: _temp3002= _temp2998.f2; goto _LL3001; _LL3001: _temp3000= _temp2998.f3;
goto _LL2999; _LL2999: Cyc_Toc_stmt_to_c( _temp3004, s);{ struct Cyc_Absyn_Stmt*
_temp3006= Cyc_Absyn_label_stmt( succ_lab, s, 0); goto _LL3007; _LL3007: if(
exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*)
e, Cyc_Absyn_seq_stmt( _temp3000, _temp3006, 0), 0);} else{ struct Cyc_Absyn_Exp*
_temp3009= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({ struct _tuple0* _temp3008=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3008->f1= Cyc_Toc_abs_ns;
_temp3008->f2= Cyc_Toc_Match_Exception_sp; _temp3008;}), 0)); goto _LL3010;
_LL3010: { struct Cyc_Absyn_Stmt* _temp3011= Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt(
_temp3009, 0), 0); goto _LL3012; _LL3012: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp3000, Cyc_Absyn_seq_stmt(
_temp3011, _temp3006, 0), 0), 0);}} for( 0; _temp3002 != 0; _temp3002=({ struct
Cyc_List_List* _temp3013= _temp3002; if( _temp3013 == 0){ _throw( Null_Exception);}
_temp3013->tl;})){ struct _tuple14 _temp3017; void* _temp3018; struct _tuple0*
_temp3020; struct _tuple14* _temp3015=( struct _tuple14*)({ struct Cyc_List_List*
_temp3014= _temp3002; if( _temp3014 == 0){ _throw( Null_Exception);} _temp3014->hd;});
_temp3017=* _temp3015; _LL3021: _temp3020= _temp3017.f1; goto _LL3019; _LL3019:
_temp3018= _temp3017.f2; goto _LL3016; _LL3016: s= Cyc_Absyn_declare_stmt(
_temp3020, _temp3018, 0, s, 0);} return s;}}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=({
struct Cyc_List_List* _temp3023= ds; if( _temp3023 == 0){ _throw( Null_Exception);}
_temp3023->tl;})){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct
Cyc_List_List* _temp3101= ds; if( _temp3101 == 0){ _throw( Null_Exception);}
_temp3101->hd;}); void* _temp3024=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp3048; struct Cyc_Absyn_Fndecl* _temp3050; int _temp3052; struct Cyc_Absyn_Exp*
_temp3054; struct Cyc_Core_Opt* _temp3056; struct Cyc_Core_Opt* _temp3058;
struct Cyc_Absyn_Pat* _temp3060; struct Cyc_Absyn_Structdecl* _temp3062; struct
Cyc_Absyn_Uniondecl* _temp3064; struct Cyc_Absyn_Tuniondecl* _temp3066; struct
Cyc_Absyn_Enumdecl* _temp3068; struct Cyc_Absyn_Typedefdecl* _temp3070; struct
Cyc_List_List* _temp3072; struct _tagged_string* _temp3074; struct Cyc_List_List*
_temp3076; struct _tuple0* _temp3078; struct Cyc_List_List* _temp3080; _LL3026:
if(*(( int*) _temp3024) == Cyc_Absyn_Var_d){ _LL3049: _temp3048=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp3024)->f1; goto _LL3027;} else{ goto
_LL3028;} _LL3028: if(*(( int*) _temp3024) == Cyc_Absyn_Fn_d){ _LL3051:
_temp3050=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp3024)->f1;
goto _LL3029;} else{ goto _LL3030;} _LL3030: if(*(( int*) _temp3024) == Cyc_Absyn_Let_d){
_LL3061: _temp3060=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp3024)->f1; goto _LL3059; _LL3059: _temp3058=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp3024)->f2; goto _LL3057; _LL3057: _temp3056=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp3024)->f3; goto
_LL3055; _LL3055: _temp3054=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp3024)->f4; goto _LL3053; _LL3053: _temp3052=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp3024)->f5; goto _LL3031;} else{ goto _LL3032;} _LL3032: if(*(( int*)
_temp3024) == Cyc_Absyn_Struct_d){ _LL3063: _temp3062=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp3024)->f1; goto _LL3033;} else{ goto
_LL3034;} _LL3034: if(*(( int*) _temp3024) == Cyc_Absyn_Union_d){ _LL3065:
_temp3064=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp3024)->f1; goto _LL3035;} else{ goto _LL3036;} _LL3036: if(*(( int*)
_temp3024) == Cyc_Absyn_Tunion_d){ _LL3067: _temp3066=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp3024)->f1; goto _LL3037;} else{ goto
_LL3038;} _LL3038: if(*(( int*) _temp3024) == Cyc_Absyn_Enum_d){ _LL3069:
_temp3068=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp3024)->f1; goto _LL3039;} else{ goto _LL3040;} _LL3040: if(*(( int*)
_temp3024) == Cyc_Absyn_Typedef_d){ _LL3071: _temp3070=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp3024)->f1; goto _LL3041;} else{ goto
_LL3042;} _LL3042: if(*(( int*) _temp3024) == Cyc_Absyn_Namespace_d){ _LL3075:
_temp3074=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp3024)->f1; goto _LL3073; _LL3073: _temp3072=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp3024)->f2; goto _LL3043;} else{ goto
_LL3044;} _LL3044: if(*(( int*) _temp3024) == Cyc_Absyn_Using_d){ _LL3079:
_temp3078=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp3024)->f1;
goto _LL3077; _LL3077: _temp3076=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp3024)->f2; goto _LL3045;} else{ goto _LL3046;} _LL3046: if(*(( int*)
_temp3024) == Cyc_Absyn_ExternC_d){ _LL3081: _temp3080=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp3024)->f1; goto _LL3047;} else{ goto
_LL3025;} _LL3027: { struct _tuple0* _temp3082= _temp3048->name; goto _LL3083;
_LL3083: if(( void*) _temp3048->sc ==( void*) Cyc_Absyn_ExternC){ _temp3082=({
struct _tuple0* _temp3084=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp3084->f1= Cyc_Toc_abs_ns; _temp3084->f2=(* _temp3082).f2; _temp3084;});}
if( _temp3048->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp3085= _temp3048->initializer; if( _temp3085 == 0){
_throw( Null_Exception);} _temp3085;}));} if( ! nv->toplevel){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp3086=(
char*)"decls_to_c: not at toplevel!"; struct _tagged_string _temp3087; _temp3087.curr=
_temp3086; _temp3087.base= _temp3086; _temp3087.last_plus_one= _temp3086 + 29;
_temp3087;}));} nv= Cyc_Toc_add_varmap( nv, _temp3048->name, Cyc_Absyn_varb_exp(
_temp3082,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp3088=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp3088[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp3089; _temp3089.tag= Cyc_Absyn_Global_b;
_temp3089.f1= _temp3048; _temp3089;}); _temp3088;}), 0)); _temp3048->name=
_temp3082;( void*)( _temp3048->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp3048->sc));(
void*)( _temp3048->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp3048->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3090=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3090->hd=( void*) d; _temp3090->tl=
Cyc_Toc_result_decls; _temp3090;}); goto _LL3025;} _LL3029: { struct _tuple0*
_temp3091= _temp3050->name; goto _LL3092; _LL3092: if(( void*) _temp3050->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3091=({ struct _tuple0* _temp3093=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3093->f1= Cyc_Toc_abs_ns;
_temp3093->f2=(* _temp3091).f2; _temp3093;});} nv= Cyc_Toc_add_varmap( nv,
_temp3050->name, Cyc_Absyn_var_exp( _temp3091, 0)); _temp3050->name= _temp3091;
Cyc_Toc_fndecl_to_c( nv, _temp3050); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3094=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3094->hd=( void*) d; _temp3094->tl= Cyc_Toc_result_decls; _temp3094;});
goto _LL3025;} _LL3031:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp3095=( char*)"letdecl"; struct
_tagged_string _temp3096; _temp3096.curr= _temp3095; _temp3096.base= _temp3095;
_temp3096.last_plus_one= _temp3095 + 8; _temp3096;})); goto _LL3025; _LL3033:
Cyc_Toc_structdecl_to_c( _temp3062); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3097=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3097->hd=( void*) d; _temp3097->tl= Cyc_Toc_result_decls; _temp3097;});
goto _LL3025; _LL3035: Cyc_Toc_uniondecl_to_c( _temp3064); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3098=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3098->hd=( void*) d; _temp3098->tl= Cyc_Toc_result_decls;
_temp3098;}); goto _LL3025; _LL3037: if( _temp3066->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp3066);} else{ Cyc_Toc_tuniondecl_to_c( _temp3066);} goto _LL3025; _LL3039:
Cyc_Toc_enumdecl_to_c( nv, _temp3068); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3099=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3099->hd=( void*) d; _temp3099->tl= Cyc_Toc_result_decls; _temp3099;});
goto _LL3025; _LL3041: _temp3070->name= _temp3070->name; _temp3070->tvs= 0;(
void*)( _temp3070->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp3070->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3100=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3100->hd=( void*) d; _temp3100->tl=
Cyc_Toc_result_decls; _temp3100;}); goto _LL3025; _LL3043: _temp3076= _temp3072;
goto _LL3045; _LL3045: _temp3080= _temp3076; goto _LL3047; _LL3047: nv= Cyc_Toc_decls_to_c(
nv, _temp3080, top); goto _LL3025; _LL3025:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}