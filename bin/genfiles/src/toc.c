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
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void
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
Cyc_List_rrev( struct _RegionHandle*, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List* x); extern char
Cyc_List_Nth[ 8u]; extern int Cyc_List_forall( int(* pred)( void*), struct Cyc_List_List*
x); extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct
_tagged_string*); extern struct _tagged_string Cyc_String_strconcat( struct
_tagged_string, struct _tagged_string); extern struct _tagged_string Cyc_String_implode(
struct Cyc_List_List* c); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
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
0; static const int Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Fastcall_att=
2; static const int Cyc_Absyn_Noreturn_att= 3; static const int Cyc_Absyn_Const_att=
4; static const int Cyc_Absyn_Aligned_att= 1; struct Cyc_Absyn_Aligned_att_struct{
int tag; int f1; } ; static const int Cyc_Absyn_Packed_att= 5; static const int
Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{ int tag; struct
_tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att= 6; static const
int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att= 8; static
const int Cyc_Absyn_Weak_att= 9; static const int Cyc_Absyn_Dllimport_att= 10;
static const int Cyc_Absyn_Dllexport_att= 11; static const int Cyc_Absyn_No_instrument_function_att=
12; static const int Cyc_Absyn_Constructor_att= 13; static const int Cyc_Absyn_Destructor_att=
14; static const int Cyc_Absyn_No_check_memory_usage_att= 15; static const int
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
int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Struct_d= 4; struct Cyc_Absyn_Struct_d_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Absyn_Union_d=
5; struct Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ;
static const int Cyc_Absyn_Tunion_d= 6; struct Cyc_Absyn_Tunion_d_struct{ int
tag; struct Cyc_Absyn_Tuniondecl* f1; } ; static const int Cyc_Absyn_Enum_d= 7;
struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ;
static const int Cyc_Absyn_Typedef_d= 8; struct Cyc_Absyn_Typedef_d_struct{ int
tag; struct Cyc_Absyn_Typedefdecl* f1; } ; static const int Cyc_Absyn_Namespace_d=
9; struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
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
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple0*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt( struct _tagged_string*
v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x, void*
t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl(
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
struct Cyc_List_List* ds); extern struct _tuple0* Cyc_Toc_temp_var(); int Cyc_Toc_use_runtimecheck_funs=
1; static struct Cyc_List_List* Cyc_Toc_result_decls= 0; struct Cyc_Absyn_Abs_n_struct
Cyc_Toc_abs_ns_value={ 1u, 0}; void* Cyc_Toc_abs_ns=( void*)& Cyc_Toc_abs_ns_value;
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={ 0, 0, 0}; char Cyc_Toc_Toc_Unimplemented[
22u]="\000\000\000\000Toc_Unimplemented"; char Cyc_Toc_Toc_Impossible[ 19u]="\000\000\000\000Toc_Impossible";
static void* Cyc_Toc_unimp( struct _tagged_string s){({ struct _tagged_string
_temp1= s; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp1.last_plus_one - _temp1.curr,
_temp1.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);(
void) _throw(( void*) Cyc_Toc_Toc_Unimplemented);} static void* Cyc_Toc_toc_impos(
struct _tagged_string s){({ struct _tagged_string _temp2= s; fprintf( Cyc_Stdio_stderr,"%.*s\n",
_temp2.last_plus_one - _temp2.curr, _temp2.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(( void*) Cyc_Toc_Toc_Impossible);}
char Cyc_Toc_Match_error[ 16u]="\000\000\000\000Match_error"; static char _temp5[
5u]="curr"; static struct _tagged_string Cyc_Toc_curr_string={ _temp5, _temp5,
_temp5 + 5u}; static struct _tagged_string* Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _temp8[ 14u]="last_plus_one"; static struct _tagged_string Cyc_Toc_last_plus_one_string={
_temp8, _temp8, _temp8 + 14u}; static struct _tagged_string* Cyc_Toc_last_plus_one_sp=&
Cyc_Toc_last_plus_one_string; static char _temp11[ 5u]="base"; static struct
_tagged_string Cyc_Toc_base_string={ _temp11, _temp11, _temp11 + 5u}; static
struct _tagged_string* Cyc_Toc_base_sp=& Cyc_Toc_base_string; static char
_temp14[ 15u]="_tagged_string"; static struct _tagged_string Cyc_Toc__tagged_string_string={
_temp14, _temp14, _temp14 + 15u}; static struct _tagged_string* Cyc_Toc__tagged_string_sp=&
Cyc_Toc__tagged_string_string; static char _temp17[ 8u]="xtunion"; static struct
_tagged_string Cyc_Toc_xtunion_string={ _temp17, _temp17, _temp17 + 8u}; static
struct _tagged_string* Cyc_Toc_xtunion_sp=& Cyc_Toc_xtunion_string; static char
_temp20[ 4u]="tag"; static struct _tagged_string Cyc_Toc_tag_string={ _temp20,
_temp20, _temp20 + 4u}; static struct _tagged_string* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static char _temp23[ 14u]="_handler_cons"; static struct _tagged_string Cyc_Toc__handler_cons_string={
_temp23, _temp23, _temp23 + 14u}; static struct _tagged_string* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static char _temp26[ 13u]="_push_region"; static
struct _tagged_string Cyc_Toc__push_region_string={ _temp26, _temp26, _temp26 +
13u}; static struct _tagged_string* Cyc_Toc__push_region_sp=& Cyc_Toc__push_region_string;
static char _temp29[ 12u]="_pop_region"; static struct _tagged_string Cyc_Toc__pop_region_string={
_temp29, _temp29, _temp29 + 12u}; static struct _tagged_string* Cyc_Toc__pop_region_sp=&
Cyc_Toc__pop_region_string; static char _temp32[ 8u]="handler"; static struct
_tagged_string Cyc_Toc_handler_string={ _temp32, _temp32, _temp32 + 8u}; static
struct _tagged_string* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static char
_temp35[ 16u]="Match_Exception"; static struct _tagged_string Cyc_Toc_Match_Exception_string={
_temp35, _temp35, _temp35 + 16u}; static struct _tagged_string* Cyc_Toc_Match_Exception_sp=&
Cyc_Toc_Match_Exception_string; static char _temp38[ 15u]="Cyc_new_string";
static struct _tagged_string Cyc_Toc_Cyc_new_string_string={ _temp38, _temp38,
_temp38 + 15u}; static struct _tagged_string* Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string;
static char _temp41[ 16u]="Cyc_rnew_string"; static struct _tagged_string Cyc_Toc_Cyc_rnew_string_string={
_temp41, _temp41, _temp41 + 16u}; static struct _tagged_string* Cyc_Toc_Cyc_rnew_string_sp=&
Cyc_Toc_Cyc_rnew_string_string; static char _temp44[ 14u]="_RegionHandle";
static struct _tagged_string Cyc_Toc__RegionHandle_string={ _temp44, _temp44,
_temp44 + 14u}; static struct _tagged_string* Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _temp47[ 12u]="_new_region"; static struct _tagged_string Cyc_Toc__new_region_string={
_temp47, _temp47, _temp47 + 12u}; static struct _tagged_string* Cyc_Toc__new_region_sp=&
Cyc_Toc__new_region_string; static char _temp50[ 7u]="_throw"; static struct
_tagged_string Cyc_Toc__throw_string={ _temp50, _temp50, _temp50 + 7u}; static
struct _tuple0 Cyc_Toc__throw_pr={( void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__throw_string};
static struct _tuple0* Cyc_Toc__throw_qv=& Cyc_Toc__throw_pr; static char
_temp53[ 7u]="setjmp"; static struct _tagged_string Cyc_Toc_setjmp_string={
_temp53, _temp53, _temp53 + 7u}; static struct _tuple0 Cyc_Toc_setjmp_pr={( void*)&
Cyc_Toc_abs_ns_value,& Cyc_Toc_setjmp_string}; static struct _tuple0* Cyc_Toc_setjmp_qv=&
Cyc_Toc_setjmp_pr; static char _temp56[ 14u]="_push_handler"; static struct
_tagged_string Cyc_Toc__push_handler_string={ _temp56, _temp56, _temp56 + 14u};
static struct _tuple0 Cyc_Toc__push_handler_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__push_handler_string}; static struct _tuple0* Cyc_Toc__push_handler_qv=&
Cyc_Toc__push_handler_pr; static char _temp59[ 13u]="_pop_handler"; static
struct _tagged_string Cyc_Toc__pop_handler_string={ _temp59, _temp59, _temp59 +
13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__pop_handler_string}; static struct _tuple0* Cyc_Toc__pop_handler_qv=&
Cyc_Toc__pop_handler_pr; static char _temp62[ 12u]="_exn_thrown"; static struct
_tagged_string Cyc_Toc__exn_thrown_string={ _temp62, _temp62, _temp62 + 12u};
static struct _tuple0 Cyc_Toc__exn_thrown_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__exn_thrown_string}; static struct _tuple0* Cyc_Toc__exn_thrown_qv=& Cyc_Toc__exn_thrown_pr;
static char _temp65[ 14u]="_npop_handler"; static struct _tagged_string Cyc_Toc__npop_handler_string={
_temp65, _temp65, _temp65 + 14u}; static struct _tuple0 Cyc_Toc__npop_handler_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__npop_handler_string}; static struct
_tuple0* Cyc_Toc__npop_handler_qv=& Cyc_Toc__npop_handler_pr; static char
_temp68[ 12u]="_check_null"; static struct _tagged_string Cyc_Toc__check_null_string={
_temp68, _temp68, _temp68 + 12u}; static struct _tuple0 Cyc_Toc__check_null_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__check_null_string}; static struct
_tuple0* Cyc_Toc__check_null_qv=& Cyc_Toc__check_null_pr; static char _temp71[
25u]="_check_unknown_subscript"; static struct _tagged_string Cyc_Toc__check_unknown_subscript_string={
_temp71, _temp71, _temp71 + 25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__check_unknown_subscript_string}; static
struct _tuple0* Cyc_Toc__check_unknown_subscript_qv=& Cyc_Toc__check_unknown_subscript_pr;
static char _temp74[ 28u]="_check_known_subscript_null"; static struct
_tagged_string Cyc_Toc__check_known_subscript_null_string={ _temp74, _temp74,
_temp74 + 28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__check_known_subscript_null_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_null_qv=& Cyc_Toc__check_known_subscript_null_pr;
static char _temp77[ 31u]="_check_known_subscript_notnull"; static struct
_tagged_string Cyc_Toc__check_known_subscript_notnull_string={ _temp77, _temp77,
_temp77 + 31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__check_known_subscript_notnull_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_notnull_qv=& Cyc_Toc__check_known_subscript_notnull_pr;
static char _temp80[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_GC_malloc_string={
_temp80, _temp80, _temp80 + 10u}; static struct _tuple0 Cyc_Toc_GC_malloc_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc_GC_malloc_string}; static struct _tuple0*
Cyc_Toc_GC_malloc_qv=& Cyc_Toc_GC_malloc_pr; static char _temp83[ 17u]="GC_malloc_atomic";
static struct _tagged_string Cyc_Toc_GC_malloc_atomic_string={ _temp83, _temp83,
_temp83 + 17u}; static struct _tuple0 Cyc_Toc_GC_malloc_atomic_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc_GC_malloc_atomic_string}; static struct _tuple0* Cyc_Toc_GC_malloc_atomic_qv=&
Cyc_Toc_GC_malloc_atomic_pr; static char _temp86[ 15u]="_region_malloc"; static
struct _tagged_string Cyc_Toc__region_malloc_string={ _temp86, _temp86, _temp86
+ 15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__region_malloc_string}; static struct _tuple0* Cyc_Toc__region_malloc_qv=&
Cyc_Toc__region_malloc_pr; struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; static int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp87=(
void*) e->r; void* _temp113; char _temp115; void* _temp117; void* _temp119;
short _temp121; void* _temp123; void* _temp125; int _temp127; void* _temp129;
void* _temp131; long long _temp133; void* _temp135; void* _temp137; struct Cyc_Absyn_Exp*
_temp139; void* _temp141; struct Cyc_List_List* _temp143; struct Cyc_List_List*
_temp145; struct Cyc_Absyn_Structdecl* _temp147; struct Cyc_List_List* _temp149;
struct Cyc_Core_Opt* _temp151; struct _tuple0* _temp153; struct Cyc_List_List*
_temp155; struct _tuple1* _temp157; struct Cyc_List_List* _temp159; struct Cyc_Core_Opt*
_temp161; _LL89: if(*(( int*) _temp87) == Cyc_Absyn_Const_e){ _LL114: _temp113=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp87)->f1; if(( unsigned int)
_temp113 > 1u?*(( int*) _temp113) == Cyc_Absyn_Char_c: 0){ _LL118: _temp117=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp113)->f1; goto _LL116; _LL116:
_temp115=(( struct Cyc_Absyn_Char_c_struct*) _temp113)->f2; goto _LL90;} else{
goto _LL91;}} else{ goto _LL91;} _LL91: if(*(( int*) _temp87) == Cyc_Absyn_Const_e){
_LL120: _temp119=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp87)->f1; if((
unsigned int) _temp119 > 1u?*(( int*) _temp119) == Cyc_Absyn_Short_c: 0){ _LL124:
_temp123=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp119)->f1; goto _LL122;
_LL122: _temp121=(( struct Cyc_Absyn_Short_c_struct*) _temp119)->f2; goto _LL92;}
else{ goto _LL93;}} else{ goto _LL93;} _LL93: if(*(( int*) _temp87) == Cyc_Absyn_Const_e){
_LL126: _temp125=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp87)->f1; if((
unsigned int) _temp125 > 1u?*(( int*) _temp125) == Cyc_Absyn_Int_c: 0){ _LL130:
_temp129=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp125)->f1; goto _LL128;
_LL128: _temp127=(( struct Cyc_Absyn_Int_c_struct*) _temp125)->f2; goto _LL94;}
else{ goto _LL95;}} else{ goto _LL95;} _LL95: if(*(( int*) _temp87) == Cyc_Absyn_Const_e){
_LL132: _temp131=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp87)->f1; if((
unsigned int) _temp131 > 1u?*(( int*) _temp131) == Cyc_Absyn_LongLong_c: 0){
_LL136: _temp135=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp131)->f1;
goto _LL134; _LL134: _temp133=(( struct Cyc_Absyn_LongLong_c_struct*) _temp131)->f2;
goto _LL96;} else{ goto _LL97;}} else{ goto _LL97;} _LL97: if(*(( int*) _temp87)
== Cyc_Absyn_Const_e){ _LL138: _temp137=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp87)->f1; if( _temp137 ==( void*) Cyc_Absyn_Null_c){ goto _LL98;} else{ goto
_LL99;}} else{ goto _LL99;} _LL99: if(*(( int*) _temp87) == Cyc_Absyn_Cast_e){
_LL142: _temp141=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp87)->f1; goto
_LL140; _LL140: _temp139=(( struct Cyc_Absyn_Cast_e_struct*) _temp87)->f2; goto
_LL100;} else{ goto _LL101;} _LL101: if(*(( int*) _temp87) == Cyc_Absyn_Tuple_e){
_LL144: _temp143=(( struct Cyc_Absyn_Tuple_e_struct*) _temp87)->f1; goto _LL102;}
else{ goto _LL103;} _LL103: if(*(( int*) _temp87) == Cyc_Absyn_Array_e){ _LL146:
_temp145=(( struct Cyc_Absyn_Array_e_struct*) _temp87)->f1; goto _LL104;} else{
goto _LL105;} _LL105: if(*(( int*) _temp87) == Cyc_Absyn_Struct_e){ _LL154:
_temp153=(( struct Cyc_Absyn_Struct_e_struct*) _temp87)->f1; goto _LL152; _LL152:
_temp151=(( struct Cyc_Absyn_Struct_e_struct*) _temp87)->f2; goto _LL150; _LL150:
_temp149=(( struct Cyc_Absyn_Struct_e_struct*) _temp87)->f3; goto _LL148; _LL148:
_temp147=(( struct Cyc_Absyn_Struct_e_struct*) _temp87)->f4; goto _LL106;} else{
goto _LL107;} _LL107: if(*(( int*) _temp87) == Cyc_Absyn_CompoundLit_e){ _LL158:
_temp157=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp87)->f1; goto _LL156;
_LL156: _temp155=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp87)->f2; goto
_LL108;} else{ goto _LL109;} _LL109: if(*(( int*) _temp87) == Cyc_Absyn_UnresolvedMem_e){
_LL162: _temp161=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp87)->f1; goto
_LL160; _LL160: _temp159=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp87)->f2;
goto _LL110;} else{ goto _LL111;} _LL111: goto _LL112; _LL90: return _temp115 =='\000';
_LL92: return _temp121 == 0; _LL94: return _temp127 == 0; _LL96: return _temp133
== 0; _LL98: return 1; _LL100: return Cyc_Toc_is_zero( _temp139); _LL102: return((
int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Toc_is_zero, _temp143); _LL104: _temp149= _temp145; goto _LL106; _LL106:
_temp155= _temp149; goto _LL108; _LL108: _temp159= _temp155; goto _LL110; _LL110:
for( 0; _temp159 != 0; _temp159=(( struct Cyc_List_List*) _check_null( _temp159))->tl){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp159))->hd)).f2)){ return 0;}} return 1; _LL112: return 0; _LL88:;} static
int Cyc_Toc_is_nullable( void* t){ void* _temp163= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp169; struct Cyc_Absyn_Conref* _temp171; struct Cyc_Absyn_Tqual
_temp173; struct Cyc_Absyn_Conref* _temp175; void* _temp177; void* _temp179;
_LL165: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_PointerType:
0){ _LL170: _temp169=(( struct Cyc_Absyn_PointerType_struct*) _temp163)->f1;
_LL180: _temp179=( void*) _temp169.elt_typ; goto _LL178; _LL178: _temp177=( void*)
_temp169.rgn_typ; goto _LL176; _LL176: _temp175= _temp169.nullable; goto _LL174;
_LL174: _temp173= _temp169.tq; goto _LL172; _LL172: _temp171= _temp169.bounds;
goto _LL166;} else{ goto _LL167;} _LL167: goto _LL168; _LL166: { void* _temp181=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp175))->v; int _temp187; _LL183: if(( unsigned int) _temp181 > 1u?*(( int*)
_temp181) == Cyc_Absyn_Eq_constr: 0){ _LL188: _temp187=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp181)->f1; goto _LL184;} else{ goto _LL185;} _LL185: goto _LL186; _LL184:
return _temp187; _LL186: return 0; _LL182:;} _LL168:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp189="is_nullable"; struct
_tagged_string _temp190; _temp190.curr= _temp189; _temp190.base= _temp189;
_temp190.last_plus_one= _temp189 + 12; _temp190;})); return 0; _LL164:;} static
struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct _tuple0* x, struct
_tagged_string tag){ return({ struct _tuple0* _temp191=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp191->f1=(* x).f1; _temp191->f2=({
struct _tagged_string* _temp192=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp192[ 0]= Cyc_String_strconcat(*(* x).f2, tag);
_temp192;}); _temp191;});} static void Cyc_Toc_add_tuple_decl( struct
_tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List* _temp193= 0;
goto _LL194; _LL194:{ int i= 1; for( 0; ts != 0;( ts=(( struct Cyc_List_List*)
_check_null( ts))->tl, i ++)){ _temp193=({ struct Cyc_List_List* _temp195=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp195->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp196=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp196->name=({ struct
_tagged_string* _temp197=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp197[ 0]= xprintf("f%d", i); _temp197;}); _temp196->tq=
Cyc_Toc_mt_tq; _temp196->type=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); _temp196->width= 0; _temp196->attributes= 0; _temp196;});
_temp195->tl= _temp193; _temp195;});}} _temp193=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp193);{ struct Cyc_Absyn_Structdecl*
_temp202=({ struct Cyc_Absyn_Structdecl* _temp198=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp198->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp198->name=({ struct Cyc_Core_Opt* _temp200=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp200->v=( void*)({ struct _tuple0*
_temp201=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp201->f1=
Cyc_Toc_abs_ns; _temp201->f2= x; _temp201;}); _temp200;}); _temp198->tvs= 0;
_temp198->fields=({ struct Cyc_Core_Opt* _temp199=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp199->v=( void*) _temp193;
_temp199;}); _temp198->attributes= 0; _temp198;}); goto _LL203; _LL203: Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp204=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp204->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp205=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp205[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp206; _temp206.tag= Cyc_Absyn_Struct_d; _temp206.f1=
_temp202; _temp206;}); _temp205;}), 0); _temp204->tl= Cyc_Toc_result_decls;
_temp204;});}} static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x,
void* t){ void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Structfield* _temp208=({ struct Cyc_Absyn_Structfield* _temp207=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp207->name= Cyc_Toc_curr_sp; _temp207->tq= Cyc_Toc_mt_tq; _temp207->type=(
void*) tptr; _temp207->width= 0; _temp207->attributes= 0; _temp207;}); goto
_LL209; _LL209: { struct Cyc_Absyn_Structfield* _temp211=({ struct Cyc_Absyn_Structfield*
_temp210=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp210->name= Cyc_Toc_base_sp; _temp210->tq= Cyc_Toc_mt_tq; _temp210->type=(
void*) tptr; _temp210->width= 0; _temp210->attributes= 0; _temp210;}); goto
_LL212; _LL212: { struct Cyc_Absyn_Structfield* _temp214=({ struct Cyc_Absyn_Structfield*
_temp213=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp213->name= Cyc_Toc_last_plus_one_sp; _temp213->tq= Cyc_Toc_mt_tq; _temp213->type=(
void*) tptr; _temp213->width= 0; _temp213->attributes= 0; _temp213;}); goto
_LL215; _LL215: { struct Cyc_List_List* _temp219=({ struct Cyc_List_List*
_temp216=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp216->hd=( void*) _temp208; _temp216->tl=({ struct Cyc_List_List* _temp217=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp217->hd=(
void*) _temp211; _temp217->tl=({ struct Cyc_List_List* _temp218=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp218->hd=( void*) _temp214;
_temp218->tl= 0; _temp218;}); _temp217;}); _temp216;}); goto _LL220; _LL220: {
struct Cyc_Absyn_Structdecl* _temp225=({ struct Cyc_Absyn_Structdecl* _temp221=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp221->sc=( void*)(( void*) Cyc_Absyn_Public); _temp221->name=({ struct Cyc_Core_Opt*
_temp223=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp223->v=( void*)({ struct _tuple0* _temp224=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp224->f1= Cyc_Toc_abs_ns; _temp224->f2= x;
_temp224;}); _temp223;}); _temp221->tvs= 0; _temp221->fields=({ struct Cyc_Core_Opt*
_temp222=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp222->v=( void*)(( struct Cyc_List_List*) _temp219); _temp222;}); _temp221->attributes=
0; _temp221;}); goto _LL226; _LL226: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp227=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp227->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp228=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp228[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp229; _temp229.tag= Cyc_Absyn_Struct_d;
_temp229.f1= _temp225; _temp229;}); _temp228;}), 0); _temp227->tl= Cyc_Toc_result_decls;
_temp227;});}}}}} static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static
int Cyc_Toc_tuple_type_counter= 0; struct _tuple5{ struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static struct _tagged_string* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp230= Cyc_Toc_tuple_types;
goto _LL231; _LL231: for( 0; _temp230 != 0; _temp230=(( struct Cyc_List_List*)
_check_null( _temp230))->tl){ struct _tuple5 _temp234; struct Cyc_List_List*
_temp235; struct _tagged_string* _temp237; struct _tuple5* _temp232=( struct
_tuple5*)(( struct Cyc_List_List*) _check_null( _temp230))->hd; _temp234=*
_temp232; _LL238: _temp237= _temp234.f1; goto _LL236; _LL236: _temp235= _temp234.f2;
goto _LL233; _LL233: { int okay= 1; struct Cyc_List_List* _temp239= tqs0; goto
_LL240; _LL240: for( 0; _temp239 != 0? _temp235 != 0: 0;( _temp239=(( struct Cyc_List_List*)
_check_null( _temp239))->tl, _temp235=(( struct Cyc_List_List*) _check_null(
_temp235))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp239))->hd)).f2,( void*)(( struct Cyc_List_List*) _check_null(
_temp235))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp239 != 0)? 1: _temp235
!= 0){ continue;} return _temp237;}}}{ struct _tagged_string* x=({ struct
_tagged_string* _temp243=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp243[ 0]= xprintf("_tuple%d", Cyc_Toc_tuple_type_counter
++); _temp243;}); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt,
tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({ struct Cyc_List_List*
_temp241=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp241->hd=( void*)({ struct _tuple5* _temp242=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp242->f1= x; _temp242->f2= ts; _temp242;});
_temp241->tl= Cyc_Toc_tuple_types; _temp241;}); return x;}} static struct Cyc_List_List*
Cyc_Toc_tagged_ptr_types= 0; static int Cyc_Toc_tagged_ptr_counter= 0; static
void* Cyc_Toc_tagged_string_type_v=( void*) 0u; static void* Cyc_Toc_tagged_string_type(){
if( Cyc_Toc_tagged_string_type_v ==( void*) Cyc_Absyn_VoidType){ Cyc_Toc_tagged_string_type_v=
Cyc_Absyn_strct( Cyc_Toc__tagged_string_sp);} return Cyc_Toc_tagged_string_type_v;}
struct _tuple6{ struct _tagged_string* f1; void* f2; } ; static void* Cyc_Toc_add_tagged_ptr_type(
void* t, struct Cyc_Absyn_Tqual tq){{ void* _temp244= t; void* _temp250; void*
_temp252; _LL246: if(( unsigned int) _temp244 > 4u?*(( int*) _temp244) == Cyc_Absyn_IntType:
0){ _LL253: _temp252=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp244)->f1;
if( _temp252 ==( void*) Cyc_Absyn_Unsigned){ goto _LL251;} else{ goto _LL248;}
_LL251: _temp250=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp244)->f2; if(
_temp250 ==( void*) Cyc_Absyn_B1){ goto _LL247;} else{ goto _LL248;}} else{ goto
_LL248;} _LL248: goto _LL249; _LL247: return Cyc_Toc_tagged_string_type();
_LL249: goto _LL245; _LL245:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts=(( struct Cyc_List_List*) _check_null( tts))->tl){ struct
_tuple6 _temp256; void* _temp257; struct _tagged_string* _temp259; struct
_tuple6* _temp254=( struct _tuple6*)(( struct Cyc_List_List*) _check_null( tts))->hd;
_temp256=* _temp254; _LL260: _temp259= _temp256.f1; goto _LL258; _LL258:
_temp257= _temp256.f2; goto _LL255; _LL255: if( ! Cyc_Tcutil_unify( t, _temp257)){
continue;} return Cyc_Absyn_strct( _temp259);}}{ struct _tagged_string* x=({
struct _tagged_string* _temp263=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp263[ 0]= xprintf("_tagged_ptr%d", Cyc_Toc_tagged_ptr_counter
++); _temp263;}); Cyc_Toc_add_tagged_ptr_decl( x, t); Cyc_Toc_tagged_ptr_types=({
struct Cyc_List_List* _temp261=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp261->hd=( void*)({ struct _tuple6* _temp262=(
struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp262->f1= x; _temp262->f2=
t; _temp262;}); _temp261->tl= Cyc_Toc_tagged_ptr_types; _temp261;}); return Cyc_Absyn_strct(
x);}} static int Cyc_Toc_temp_var_counter= 0; struct _tuple0* Cyc_Toc_temp_var(){
return({ struct _tuple0* _temp264=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp264->f1=( void*) Cyc_Absyn_Loc_n; _temp264->f2=({ struct
_tagged_string* _temp265=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp265[ 0]= xprintf("_temp%d", Cyc_Toc_temp_var_counter
++); _temp265;}); _temp264;});} static int Cyc_Toc_fresh_label_counter= 0;
static struct _tagged_string* Cyc_Toc_fresh_label(){ return({ struct
_tagged_string* _temp266=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp266[ 0]= xprintf("_LL%d", Cyc_Toc_temp_var_counter ++);
_temp266;});} static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple1*
Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp269; struct Cyc_Absyn_Tqual
_temp271; struct Cyc_Core_Opt* _temp273; struct _tuple1 _temp267=* a; _LL274:
_temp273= _temp267.f1; goto _LL272; _LL272: _temp271= _temp267.f2; goto _LL270;
_LL270: _temp269= _temp267.f3; goto _LL268; _LL268: return({ struct _tuple1*
_temp275=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp275->f1=
_temp273; _temp275->f2= _temp271; _temp275->f3= Cyc_Toc_typ_to_c( _temp269);
_temp275;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp278; struct Cyc_Absyn_Tqual _temp280; struct _tuple3 _temp276=* x;
_LL281: _temp280= _temp276.f1; goto _LL279; _LL279: _temp278= _temp276.f2; goto
_LL277; _LL277: return({ struct _tuple3* _temp282=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp282->f1= _temp280; _temp282->f2= Cyc_Toc_typ_to_c(
_temp278); _temp282;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp283= t; struct Cyc_Absyn_Exp* _temp291; struct Cyc_Absyn_Tqual _temp293;
void* _temp295; int _temp297; struct Cyc_Core_Opt* _temp299; struct Cyc_Core_Opt
_temp301; void* _temp302; void* _temp304; _LL285: if(( unsigned int) _temp283 >
4u?*(( int*) _temp283) == Cyc_Absyn_ArrayType: 0){ _LL296: _temp295=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp283)->f1; goto _LL294; _LL294: _temp293=((
struct Cyc_Absyn_ArrayType_struct*) _temp283)->f2; goto _LL292; _LL292: _temp291=((
struct Cyc_Absyn_ArrayType_struct*) _temp283)->f3; goto _LL286;} else{ goto
_LL287;} _LL287: if(( unsigned int) _temp283 > 4u?*(( int*) _temp283) == Cyc_Absyn_Evar:
0){ _LL305: _temp304=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp283)->f1;
goto _LL300; _LL300: _temp299=(( struct Cyc_Absyn_Evar_struct*) _temp283)->f2;
if( _temp299 == 0){ goto _LL289;} else{ _temp301=* _temp299; _LL303: _temp302=(
void*) _temp301.v; goto _LL298;} _LL298: _temp297=(( struct Cyc_Absyn_Evar_struct*)
_temp283)->f3; goto _LL288;} else{ goto _LL289;} _LL289: goto _LL290; _LL286:
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp306=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp306[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp307; _temp307.tag= Cyc_Absyn_ArrayType; _temp307.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp295); _temp307.f2= _temp293; _temp307.f3=
_temp291; _temp307;}); _temp306;}); _LL288: return Cyc_Toc_typ_to_c_array(
_temp302); _LL290: return Cyc_Toc_typ_to_c( t); _LL284:;} static void* Cyc_Toc_typ_to_c(
void* t); static struct Cyc_Absyn_Structfield* Cyc_Toc_structfield_to_c( struct
Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield* _temp308=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp308->name= f->name; _temp308->tq= f->tq; _temp308->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp308->width= f->width; _temp308->attributes= f->attributes;
_temp308;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp309= t; int
_temp361; struct Cyc_Core_Opt* _temp363; void* _temp365; int _temp367; struct
Cyc_Core_Opt* _temp369; struct Cyc_Core_Opt _temp371; void* _temp372; void*
_temp374; struct Cyc_Absyn_Tvar* _temp376; struct Cyc_Absyn_TunionInfo _temp378;
void* _temp380; struct Cyc_List_List* _temp382; void* _temp384; struct Cyc_Absyn_Tuniondecl*
_temp386; struct Cyc_Absyn_TunionInfo _temp388; struct Cyc_Absyn_TunionFieldInfo
_temp390; struct Cyc_List_List* _temp392; void* _temp394; struct Cyc_Absyn_Tunionfield*
_temp396; struct Cyc_Absyn_Tuniondecl* _temp398; struct Cyc_Absyn_TunionFieldInfo
_temp400; struct Cyc_Absyn_PtrInfo _temp402; struct Cyc_Absyn_Conref* _temp404;
struct Cyc_Absyn_Tqual _temp406; struct Cyc_Absyn_Conref* _temp408; void*
_temp410; void* _temp412; void* _temp414; void* _temp416; struct Cyc_Absyn_Exp*
_temp418; struct Cyc_Absyn_Tqual _temp420; void* _temp422; struct Cyc_Absyn_FnInfo
_temp424; struct Cyc_List_List* _temp426; int _temp428; struct Cyc_List_List*
_temp430; void* _temp432; struct Cyc_Core_Opt* _temp434; struct Cyc_List_List*
_temp436; struct Cyc_List_List* _temp438; struct Cyc_List_List* _temp440; struct
Cyc_List_List* _temp442; struct Cyc_Absyn_Uniondecl** _temp444; struct Cyc_List_List*
_temp446; struct _tuple0* _temp448; struct Cyc_Absyn_Structdecl** _temp450;
struct Cyc_List_List* _temp452; struct _tuple0* _temp454; struct Cyc_Absyn_Enumdecl*
_temp456; struct _tuple0* _temp458; struct Cyc_Core_Opt* _temp460; struct Cyc_List_List*
_temp462; struct _tuple0* _temp464; void* _temp466; void* _temp468; struct Cyc_List_List*
_temp470; _LL311: if( _temp309 ==( void*) Cyc_Absyn_VoidType){ goto _LL312;}
else{ goto _LL313;} _LL313: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309)
== Cyc_Absyn_Evar: 0){ _LL366: _temp365=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp309)->f1; goto _LL364; _LL364: _temp363=(( struct Cyc_Absyn_Evar_struct*)
_temp309)->f2; if( _temp363 == 0){ goto _LL362;} else{ goto _LL315;} _LL362:
_temp361=(( struct Cyc_Absyn_Evar_struct*) _temp309)->f3; goto _LL314;} else{
goto _LL315;} _LL315: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) ==
Cyc_Absyn_Evar: 0){ _LL375: _temp374=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp309)->f1; goto _LL370; _LL370: _temp369=(( struct Cyc_Absyn_Evar_struct*)
_temp309)->f2; if( _temp369 == 0){ goto _LL317;} else{ _temp371=* _temp369;
_LL373: _temp372=( void*) _temp371.v; goto _LL368;} _LL368: _temp367=(( struct
Cyc_Absyn_Evar_struct*) _temp309)->f3; goto _LL316;} else{ goto _LL317;} _LL317:
if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_VarType: 0){
_LL377: _temp376=(( struct Cyc_Absyn_VarType_struct*) _temp309)->f1; goto _LL318;}
else{ goto _LL319;} _LL319: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309)
== Cyc_Absyn_TunionType: 0){ _LL379: _temp378=(( struct Cyc_Absyn_TunionType_struct*)
_temp309)->f1; _LL385: _temp384=( void*) _temp378.tunion_info; if(*(( int*)
_temp384) == Cyc_Absyn_KnownTunion){ _LL387: _temp386=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp384)->f1; goto _LL383;} else{ goto _LL321;} _LL383: _temp382= _temp378.targs;
goto _LL381; _LL381: _temp380=( void*) _temp378.rgn; goto _LL320;} else{ goto
_LL321;} _LL321: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_TunionType:
0){ _LL389: _temp388=(( struct Cyc_Absyn_TunionType_struct*) _temp309)->f1; goto
_LL322;} else{ goto _LL323;} _LL323: if(( unsigned int) _temp309 > 4u?*(( int*)
_temp309) == Cyc_Absyn_TunionFieldType: 0){ _LL391: _temp390=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp309)->f1; _LL395: _temp394=( void*) _temp390.field_info; if(*(( int*)
_temp394) == Cyc_Absyn_KnownTunionfield){ _LL399: _temp398=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp394)->f1; goto _LL397; _LL397: _temp396=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp394)->f2; goto _LL393;} else{ goto _LL325;} _LL393: _temp392= _temp390.targs;
goto _LL324;} else{ goto _LL325;} _LL325: if(( unsigned int) _temp309 > 4u?*((
int*) _temp309) == Cyc_Absyn_TunionFieldType: 0){ _LL401: _temp400=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp309)->f1; goto _LL326;} else{ goto _LL327;} _LL327: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_PointerType: 0){ _LL403: _temp402=((
struct Cyc_Absyn_PointerType_struct*) _temp309)->f1; _LL413: _temp412=( void*)
_temp402.elt_typ; goto _LL411; _LL411: _temp410=( void*) _temp402.rgn_typ; goto
_LL409; _LL409: _temp408= _temp402.nullable; goto _LL407; _LL407: _temp406=
_temp402.tq; goto _LL405; _LL405: _temp404= _temp402.bounds; goto _LL328;} else{
goto _LL329;} _LL329: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) ==
Cyc_Absyn_IntType: 0){ _LL417: _temp416=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp309)->f1; goto _LL415; _LL415: _temp414=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp309)->f2; goto _LL330;} else{ goto _LL331;} _LL331: if( _temp309 ==( void*)
Cyc_Absyn_FloatType){ goto _LL332;} else{ goto _LL333;} _LL333: if( _temp309 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL334;} else{ goto _LL335;} _LL335: if((
unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_ArrayType: 0){
_LL423: _temp422=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp309)->f1;
goto _LL421; _LL421: _temp420=(( struct Cyc_Absyn_ArrayType_struct*) _temp309)->f2;
goto _LL419; _LL419: _temp418=(( struct Cyc_Absyn_ArrayType_struct*) _temp309)->f3;
goto _LL336;} else{ goto _LL337;} _LL337: if(( unsigned int) _temp309 > 4u?*((
int*) _temp309) == Cyc_Absyn_FnType: 0){ _LL425: _temp424=(( struct Cyc_Absyn_FnType_struct*)
_temp309)->f1; _LL437: _temp436= _temp424.tvars; goto _LL435; _LL435: _temp434=
_temp424.effect; goto _LL433; _LL433: _temp432=( void*) _temp424.ret_typ; goto
_LL431; _LL431: _temp430= _temp424.args; goto _LL429; _LL429: _temp428= _temp424.varargs;
goto _LL427; _LL427: _temp426= _temp424.attributes; goto _LL338;} else{ goto
_LL339;} _LL339: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_TupleType:
0){ _LL439: _temp438=(( struct Cyc_Absyn_TupleType_struct*) _temp309)->f1; goto
_LL340;} else{ goto _LL341;} _LL341: if(( unsigned int) _temp309 > 4u?*(( int*)
_temp309) == Cyc_Absyn_AnonStructType: 0){ _LL441: _temp440=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp309)->f1; goto _LL342;} else{ goto _LL343;} _LL343: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_AnonUnionType: 0){ _LL443:
_temp442=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp309)->f1; goto _LL344;}
else{ goto _LL345;} _LL345: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309)
== Cyc_Absyn_UnionType: 0){ _LL449: _temp448=(( struct Cyc_Absyn_UnionType_struct*)
_temp309)->f1; goto _LL447; _LL447: _temp446=(( struct Cyc_Absyn_UnionType_struct*)
_temp309)->f2; goto _LL445; _LL445: _temp444=(( struct Cyc_Absyn_UnionType_struct*)
_temp309)->f3; goto _LL346;} else{ goto _LL347;} _LL347: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_StructType: 0){ _LL455: _temp454=((
struct Cyc_Absyn_StructType_struct*) _temp309)->f1; goto _LL453; _LL453:
_temp452=(( struct Cyc_Absyn_StructType_struct*) _temp309)->f2; goto _LL451;
_LL451: _temp450=(( struct Cyc_Absyn_StructType_struct*) _temp309)->f3; goto
_LL348;} else{ goto _LL349;} _LL349: if(( unsigned int) _temp309 > 4u?*(( int*)
_temp309) == Cyc_Absyn_EnumType: 0){ _LL459: _temp458=(( struct Cyc_Absyn_EnumType_struct*)
_temp309)->f1; goto _LL457; _LL457: _temp456=(( struct Cyc_Absyn_EnumType_struct*)
_temp309)->f2; goto _LL350;} else{ goto _LL351;} _LL351: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_TypedefType: 0){ _LL465: _temp464=((
struct Cyc_Absyn_TypedefType_struct*) _temp309)->f1; goto _LL463; _LL463:
_temp462=(( struct Cyc_Absyn_TypedefType_struct*) _temp309)->f2; goto _LL461;
_LL461: _temp460=(( struct Cyc_Absyn_TypedefType_struct*) _temp309)->f3; goto
_LL352;} else{ goto _LL353;} _LL353: if(( unsigned int) _temp309 > 4u?*(( int*)
_temp309) == Cyc_Absyn_RgnHandleType: 0){ _LL467: _temp466=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp309)->f1; goto _LL354;} else{ goto _LL355;} _LL355: if( _temp309 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL356;} else{ goto _LL357;} _LL357: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_AccessEff: 0){ _LL469: _temp468=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp309)->f1; goto _LL358;} else{
goto _LL359;} _LL359: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) ==
Cyc_Absyn_JoinEff: 0){ _LL471: _temp470=(( struct Cyc_Absyn_JoinEff_struct*)
_temp309)->f1; goto _LL360;} else{ goto _LL310;} _LL312: return t; _LL314:
return( void*) Cyc_Absyn_VoidType; _LL316: return Cyc_Toc_typ_to_c( _temp372);
_LL318: return Cyc_Absyn_void_star_typ(); _LL320: return Cyc_Absyn_void_star_typ();
_LL322:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
char* _temp472="unresolved TunionType"; struct _tagged_string _temp473; _temp473.curr=
_temp472; _temp473.base= _temp472; _temp473.last_plus_one= _temp472 + 22;
_temp473;}))); _LL324: if( _temp396->typs == 0){ if( _temp398->is_xtunion){
return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);}
else{ return Cyc_Absyn_uint_t;}} else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp396->name,({ char* _temp474="_struct"; struct _tagged_string _temp475;
_temp475.curr= _temp474; _temp475.base= _temp474; _temp475.last_plus_one=
_temp474 + 8; _temp475;})));} _LL326:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ char* _temp476="unresolved TunionFieldType";
struct _tagged_string _temp477; _temp477.curr= _temp476; _temp477.base= _temp476;
_temp477.last_plus_one= _temp476 + 27; _temp477;}))); _LL328: _temp412= Cyc_Toc_typ_to_c_array(
_temp412);{ void* _temp478=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp404))->v; void* _temp484; _LL480: if((
unsigned int) _temp478 > 1u?*(( int*) _temp478) == Cyc_Absyn_Eq_constr: 0){
_LL485: _temp484=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp478)->f1;
if( _temp484 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL481;} else{ goto _LL482;}}
else{ goto _LL482;} _LL482: goto _LL483; _LL481: return Cyc_Toc_add_tagged_ptr_type(
_temp412, _temp406); _LL483: return Cyc_Absyn_star_typ( _temp412,( void*) Cyc_Absyn_HeapRgn,
_temp406); _LL479:;} _LL330: return t; _LL332: return t; _LL334: return t;
_LL336: return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp486=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp486[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp487; _temp487.tag= Cyc_Absyn_ArrayType;
_temp487.f1=( void*) Cyc_Toc_typ_to_c_array( _temp422); _temp487.f2= _temp420;
_temp487.f3= _temp418; _temp487;}); _temp486;}); _LL338: { struct Cyc_List_List*
_temp488= _temp426; goto _LL489; _LL489: { struct Cyc_List_List* _temp490= 0;
goto _LL491; _LL491: for( 0; _temp426 != 0; _temp426=(( struct Cyc_List_List*)
_check_null( _temp426))->tl){ void* _temp492=( void*)(( struct Cyc_List_List*)
_check_null( _temp426))->hd; _LL494: if( _temp492 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL495;} else{ goto _LL496;} _LL496: if( _temp492 ==( void*) Cyc_Absyn_Const_att){
goto _LL497;} else{ goto _LL498;} _LL498: goto _LL499; _LL495: goto _LL497;
_LL497: continue; _LL499: _temp490=({ struct Cyc_List_List* _temp500=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp500->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( _temp426))->hd); _temp500->tl=
_temp490; _temp500;}); goto _LL493; _LL493:;} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp501=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp501[ 0]=({ struct Cyc_Absyn_FnType_struct _temp502; _temp502.tag= Cyc_Absyn_FnType;
_temp502.f1=({ struct Cyc_Absyn_FnInfo _temp503; _temp503.tvars= 0; _temp503.effect=
0; _temp503.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp432); _temp503.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp430); _temp503.varargs= _temp428;
_temp503.attributes= _temp490; _temp503;}); _temp502;}); _temp501;});}} _LL340:
_temp438=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp438);{ struct
_tagged_string* _temp504= Cyc_Toc_add_tuple_type( _temp438); goto _LL505; _LL505:
return Cyc_Absyn_strct( _temp504);} _LL342: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp506=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp506[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp507; _temp507.tag= Cyc_Absyn_AnonStructType; _temp507.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp440); _temp507;}); _temp506;});
_LL344: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp508=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp508[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp509; _temp509.tag=
Cyc_Absyn_AnonUnionType; _temp509.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp442); _temp509;}); _temp508;}); _LL346: if( _temp448 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ char* _temp510="anonymous union";
struct _tagged_string _temp511; _temp511.curr= _temp510; _temp511.base= _temp510;
_temp511.last_plus_one= _temp510 + 16; _temp511;}));} return Cyc_Absyn_unionq_typ((
struct _tuple0*) _check_null( _temp448)); _LL348: if( _temp454 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ char* _temp512="anonymous struct";
struct _tagged_string _temp513; _temp513.curr= _temp512; _temp513.base= _temp512;
_temp513.last_plus_one= _temp512 + 17; _temp513;}));} return Cyc_Absyn_strctq((
struct _tuple0*) _check_null( _temp454)); _LL350: return t; _LL352: if( _temp460
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp514="Toc::typ_to_c: unresolved TypedefType"; struct _tagged_string _temp515;
_temp515.curr= _temp514; _temp515.base= _temp514; _temp515.last_plus_one=
_temp514 + 38; _temp515;}));} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp516=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp516[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp517; _temp517.tag= Cyc_Absyn_TypedefType;
_temp517.f1= _temp464; _temp517.f2= 0; _temp517.f3=({ struct Cyc_Core_Opt*
_temp518=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp518->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp460))->v); _temp518;}); _temp517;}); _temp516;}); _LL354:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL356: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp519="Toc::typ_to_c: type translation passed the heap region"; struct
_tagged_string _temp520; _temp520.curr= _temp519; _temp520.base= _temp519;
_temp520.last_plus_one= _temp519 + 55; _temp520;})); _LL358: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp521="Toc::typ_to_c: type translation passed the access eff";
struct _tagged_string _temp522; _temp522.curr= _temp521; _temp522.base= _temp521;
_temp522.last_plus_one= _temp521 + 54; _temp522;})); _LL360: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp523="Toc::typ_to_c: type translation passed the join eff";
struct _tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 52; _temp524;})); _LL310:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp525= t; struct Cyc_Absyn_Exp* _temp531; struct Cyc_Absyn_Tqual
_temp533; void* _temp535; _LL527: if(( unsigned int) _temp525 > 4u?*(( int*)
_temp525) == Cyc_Absyn_ArrayType: 0){ _LL536: _temp535=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp525)->f1; goto _LL534; _LL534: _temp533=(( struct Cyc_Absyn_ArrayType_struct*)
_temp525)->f2; goto _LL532; _LL532: _temp531=(( struct Cyc_Absyn_ArrayType_struct*)
_temp525)->f3; goto _LL528;} else{ goto _LL529;} _LL529: goto _LL530; _LL528:
return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ( _temp535,( void*) Cyc_Absyn_HeapRgn,
_temp533), e, l); _LL530: return Cyc_Absyn_cast_exp( t, e, l); _LL526:;} static
int Cyc_Toc_atomic_typ( void* t){ void* _temp537= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp573; void* _temp575; void* _temp577; struct Cyc_Absyn_Exp*
_temp579; struct Cyc_Absyn_Tqual _temp581; void* _temp583; struct Cyc_Absyn_FnInfo
_temp585; struct Cyc_List_List* _temp587; struct Cyc_Absyn_Structdecl** _temp589;
struct Cyc_List_List* _temp591; struct _tuple0* _temp593; struct Cyc_Absyn_TunionInfo
_temp595; struct Cyc_List_List* _temp597; struct Cyc_List_List* _temp599; struct
Cyc_Absyn_TunionFieldInfo _temp601; struct Cyc_List_List* _temp603; void*
_temp605; struct Cyc_Absyn_Tunionfield* _temp607; struct Cyc_Absyn_Tuniondecl*
_temp609; struct Cyc_Absyn_Uniondecl** _temp611; struct Cyc_List_List* _temp613;
struct _tuple0* _temp615; struct Cyc_Absyn_PtrInfo _temp617; void* _temp619;
_LL539: if( _temp537 ==( void*) Cyc_Absyn_VoidType){ goto _LL540;} else{ goto
_LL541;} _LL541: if(( unsigned int) _temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_VarType:
0){ _LL574: _temp573=(( struct Cyc_Absyn_VarType_struct*) _temp537)->f1; goto
_LL542;} else{ goto _LL543;} _LL543: if(( unsigned int) _temp537 > 4u?*(( int*)
_temp537) == Cyc_Absyn_IntType: 0){ _LL578: _temp577=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp537)->f1; goto _LL576; _LL576: _temp575=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp537)->f2; goto _LL544;} else{ goto _LL545;} _LL545: if( _temp537 ==( void*)
Cyc_Absyn_FloatType){ goto _LL546;} else{ goto _LL547;} _LL547: if( _temp537 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL548;} else{ goto _LL549;} _LL549: if((
unsigned int) _temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_ArrayType: 0){
_LL584: _temp583=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp537)->f1;
goto _LL582; _LL582: _temp581=(( struct Cyc_Absyn_ArrayType_struct*) _temp537)->f2;
goto _LL580; _LL580: _temp579=(( struct Cyc_Absyn_ArrayType_struct*) _temp537)->f3;
goto _LL550;} else{ goto _LL551;} _LL551: if(( unsigned int) _temp537 > 4u?*((
int*) _temp537) == Cyc_Absyn_FnType: 0){ _LL586: _temp585=(( struct Cyc_Absyn_FnType_struct*)
_temp537)->f1; goto _LL552;} else{ goto _LL553;} _LL553: if(( unsigned int)
_temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_TupleType: 0){ _LL588: _temp587=((
struct Cyc_Absyn_TupleType_struct*) _temp537)->f1; goto _LL554;} else{ goto
_LL555;} _LL555: if(( unsigned int) _temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_StructType:
0){ _LL594: _temp593=(( struct Cyc_Absyn_StructType_struct*) _temp537)->f1; goto
_LL592; _LL592: _temp591=(( struct Cyc_Absyn_StructType_struct*) _temp537)->f2;
goto _LL590; _LL590: _temp589=(( struct Cyc_Absyn_StructType_struct*) _temp537)->f3;
goto _LL556;} else{ goto _LL557;} _LL557: if(( unsigned int) _temp537 > 4u?*((
int*) _temp537) == Cyc_Absyn_TunionType: 0){ _LL596: _temp595=(( struct Cyc_Absyn_TunionType_struct*)
_temp537)->f1; goto _LL558;} else{ goto _LL559;} _LL559: if(( unsigned int)
_temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_AnonStructType: 0){ _LL598:
_temp597=(( struct Cyc_Absyn_AnonStructType_struct*) _temp537)->f1; goto _LL560;}
else{ goto _LL561;} _LL561: if(( unsigned int) _temp537 > 4u?*(( int*) _temp537)
== Cyc_Absyn_AnonUnionType: 0){ _LL600: _temp599=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp537)->f1; goto _LL562;} else{ goto _LL563;} _LL563: if(( unsigned int)
_temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_TunionFieldType: 0){ _LL602:
_temp601=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp537)->f1; _LL606:
_temp605=( void*) _temp601.field_info; if(*(( int*) _temp605) == Cyc_Absyn_KnownTunionfield){
_LL610: _temp609=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp605)->f1;
goto _LL608; _LL608: _temp607=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp605)->f2; goto _LL604;} else{ goto _LL565;} _LL604: _temp603= _temp601.targs;
goto _LL564;} else{ goto _LL565;} _LL565: if(( unsigned int) _temp537 > 4u?*((
int*) _temp537) == Cyc_Absyn_UnionType: 0){ _LL616: _temp615=(( struct Cyc_Absyn_UnionType_struct*)
_temp537)->f1; goto _LL614; _LL614: _temp613=(( struct Cyc_Absyn_UnionType_struct*)
_temp537)->f2; goto _LL612; _LL612: _temp611=(( struct Cyc_Absyn_UnionType_struct*)
_temp537)->f3; goto _LL566;} else{ goto _LL567;} _LL567: if(( unsigned int)
_temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_PointerType: 0){ _LL618: _temp617=((
struct Cyc_Absyn_PointerType_struct*) _temp537)->f1; goto _LL568;} else{ goto
_LL569;} _LL569: if(( unsigned int) _temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_RgnHandleType:
0){ _LL620: _temp619=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp537)->f1;
goto _LL570;} else{ goto _LL571;} _LL571: goto _LL572; _LL540: return 1; _LL542:
return 0; _LL544: return 1; _LL546: return 1; _LL548: return 0; _LL550: return
Cyc_Toc_atomic_typ( _temp583); _LL552: return 1; _LL554: for( 0; _temp587 != 0;
_temp587=(( struct Cyc_List_List*) _check_null( _temp587))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp587))->hd)).f2)){
return 0;}} return 1; _LL556: return 0; _LL558: return 0; _LL560: _temp599=
_temp597; goto _LL562; _LL562: for( 0; _temp599 != 0; _temp599=(( struct Cyc_List_List*)
_check_null( _temp599))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp599))->hd)->type)){ return 0;}} return
1; _LL564: return _temp607->typs == 0; _LL566: return 0; _LL568: return 0;
_LL570: return 0; _LL572:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp621= Cyc_Absynpp_typ2string( t); xprintf("atomic_typ:  bad type %.*s",
_temp621.last_plus_one - _temp621.curr, _temp621.curr);})); return 0; _LL538:;}
static int Cyc_Toc_is_void_star( void* t){ void* _temp622= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp628; struct Cyc_Absyn_Conref* _temp630; struct
Cyc_Absyn_Tqual _temp632; struct Cyc_Absyn_Conref* _temp634; void* _temp636;
void* _temp638; _LL624: if(( unsigned int) _temp622 > 4u?*(( int*) _temp622) ==
Cyc_Absyn_PointerType: 0){ _LL629: _temp628=(( struct Cyc_Absyn_PointerType_struct*)
_temp622)->f1; _LL639: _temp638=( void*) _temp628.elt_typ; goto _LL637; _LL637:
_temp636=( void*) _temp628.rgn_typ; goto _LL635; _LL635: _temp634= _temp628.nullable;
goto _LL633; _LL633: _temp632= _temp628.tq; goto _LL631; _LL631: _temp630=
_temp628.bounds; goto _LL625;} else{ goto _LL626;} _LL626: goto _LL627; _LL625: {
void* _temp640= Cyc_Tcutil_compress( _temp638); _LL642: if( _temp640 ==( void*)
Cyc_Absyn_VoidType){ goto _LL643;} else{ goto _LL644;} _LL644: goto _LL645;
_LL643: return 1; _LL645: return 0; _LL641:;} _LL627: return 0; _LL623:;} static
int Cyc_Toc_is_poly_field( void* t, struct _tagged_string* f){ void* _temp646=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl** _temp658; struct Cyc_Absyn_Structdecl*
_temp660; struct Cyc_List_List* _temp661; struct _tuple0* _temp663; struct Cyc_List_List*
_temp665; struct Cyc_List_List* _temp667; struct Cyc_Absyn_Uniondecl** _temp669;
struct Cyc_List_List* _temp671; struct _tuple0* _temp673; _LL648: if((
unsigned int) _temp646 > 4u?*(( int*) _temp646) == Cyc_Absyn_StructType: 0){
_LL664: _temp663=(( struct Cyc_Absyn_StructType_struct*) _temp646)->f1; goto
_LL662; _LL662: _temp661=(( struct Cyc_Absyn_StructType_struct*) _temp646)->f2;
goto _LL659; _LL659: _temp658=(( struct Cyc_Absyn_StructType_struct*) _temp646)->f3;
if( _temp658 == 0){ goto _LL650;} else{ _temp660=* _temp658; goto _LL649;}}
else{ goto _LL650;} _LL650: if(( unsigned int) _temp646 > 4u?*(( int*) _temp646)
== Cyc_Absyn_AnonStructType: 0){ _LL666: _temp665=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp646)->f1; goto _LL651;} else{ goto _LL652;} _LL652: if(( unsigned int)
_temp646 > 4u?*(( int*) _temp646) == Cyc_Absyn_AnonUnionType: 0){ _LL668:
_temp667=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp646)->f1; goto _LL653;}
else{ goto _LL654;} _LL654: if(( unsigned int) _temp646 > 4u?*(( int*) _temp646)
== Cyc_Absyn_UnionType: 0){ _LL674: _temp673=(( struct Cyc_Absyn_UnionType_struct*)
_temp646)->f1; goto _LL672; _LL672: _temp671=(( struct Cyc_Absyn_UnionType_struct*)
_temp646)->f2; goto _LL670; _LL670: _temp669=(( struct Cyc_Absyn_UnionType_struct*)
_temp646)->f3; goto _LL655;} else{ goto _LL656;} _LL656: goto _LL657; _LL649:
if( _temp660->fields == 0){( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ char* _temp675="is_poly_field: struct missing fields";
struct _tagged_string _temp676; _temp676.curr= _temp675; _temp676.base= _temp675;
_temp676.last_plus_one= _temp675 + 37; _temp676;})));} _temp665=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp660->fields))->v; goto _LL651; _LL651: {
struct Cyc_Absyn_Structfield* _temp677= Cyc_Absyn_lookup_field( _temp665, f);
goto _LL678; _LL678: if( _temp677 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp679=*(( struct Cyc_Absyn_Structfield*)
_check_null( _temp677))->name; xprintf("is_poly_field: bad field %.*s", _temp679.last_plus_one
- _temp679.curr, _temp679.curr);}));} return Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp677))->type);} _LL653: return 0;
_LL655: return 0; _LL657:( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp680= Cyc_Absynpp_typ2string( t);
xprintf("is_poly_field: bad type %.*s", _temp680.last_plus_one - _temp680.curr,
_temp680.curr);}))); _LL647:;} static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp*
e){ void* _temp681=( void*) e->r; struct _tagged_string* _temp689; struct Cyc_Absyn_Exp*
_temp691; struct _tagged_string* _temp693; struct Cyc_Absyn_Exp* _temp695;
_LL683: if(*(( int*) _temp681) == Cyc_Absyn_StructMember_e){ _LL692: _temp691=((
struct Cyc_Absyn_StructMember_e_struct*) _temp681)->f1; goto _LL690; _LL690:
_temp689=(( struct Cyc_Absyn_StructMember_e_struct*) _temp681)->f2; goto _LL684;}
else{ goto _LL685;} _LL685: if(*(( int*) _temp681) == Cyc_Absyn_StructArrow_e){
_LL696: _temp695=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp681)->f1; goto
_LL694; _LL694: _temp693=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp681)->f2;
goto _LL686;} else{ goto _LL687;} _LL687: goto _LL688; _LL684: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp691->topt))->v, _temp689);
_LL686: { void* _temp697= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp695->topt))->v); struct Cyc_Absyn_PtrInfo _temp703; struct Cyc_Absyn_Conref*
_temp705; struct Cyc_Absyn_Tqual _temp707; struct Cyc_Absyn_Conref* _temp709;
void* _temp711; void* _temp713; _LL699: if(( unsigned int) _temp697 > 4u?*(( int*)
_temp697) == Cyc_Absyn_PointerType: 0){ _LL704: _temp703=(( struct Cyc_Absyn_PointerType_struct*)
_temp697)->f1; _LL714: _temp713=( void*) _temp703.elt_typ; goto _LL712; _LL712:
_temp711=( void*) _temp703.rgn_typ; goto _LL710; _LL710: _temp709= _temp703.nullable;
goto _LL708; _LL708: _temp707= _temp703.tq; goto _LL706; _LL706: _temp705=
_temp703.bounds; goto _LL700;} else{ goto _LL701;} _LL701: goto _LL702; _LL700:
return Cyc_Toc_is_poly_field( _temp713, _temp693); _LL702:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp715= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp695->topt))->v); xprintf("is_poly_project:  bad type %.*s",
_temp715.last_plus_one - _temp715.curr, _temp715.curr);})); return 0; _LL698:;}
_LL688: return 0; _LL682:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_qv,
0),({ struct Cyc_List_List* _temp716=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp716->hd=( void*) s; _temp716->tl= 0; _temp716;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_atomic_qv, 0),({
struct Cyc_List_List* _temp717=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp717->hd=( void*) s; _temp717->tl= 0; _temp717;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__region_malloc_qv,
0),({ struct Cyc_List_List* _temp718=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp718->hd=( void*) rgn; _temp718->tl=({ struct Cyc_List_List*
_temp719=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp719->hd=( void*) s; _temp719->tl= 0; _temp719;}); _temp718;}), 0);} static
const int Cyc_Toc_NullCheck= 0; struct Cyc_Toc_NullCheck_struct{ int tag; void*
f1; } ; static const int Cyc_Toc_UntagPtr= 1; struct Cyc_Toc_UntagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; int f3; } ; static const int Cyc_Toc_TagPtr=
2; struct Cyc_Toc_TagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; }
; static const int Cyc_Toc_NoConv= 0; typedef void* Cyc_Toc_conv; struct _tuple7{
void* f1; void* f2; } ; static void* Cyc_Toc_conversion( void* t1, void* t2){ t1=
Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp721=({
struct _tuple7 _temp720; _temp720.f1= t1; _temp720.f2= t2; _temp720;}); void*
_temp727; struct Cyc_Absyn_PtrInfo _temp729; void* _temp731; struct Cyc_Absyn_PtrInfo
_temp733; _LL723: _LL732: _temp731= _temp721.f1; if(( unsigned int) _temp731 > 4u?*((
int*) _temp731) == Cyc_Absyn_PointerType: 0){ _LL734: _temp733=(( struct Cyc_Absyn_PointerType_struct*)
_temp731)->f1; goto _LL728;} else{ goto _LL725;} _LL728: _temp727= _temp721.f2;
if(( unsigned int) _temp727 > 4u?*(( int*) _temp727) == Cyc_Absyn_PointerType: 0){
_LL730: _temp729=(( struct Cyc_Absyn_PointerType_struct*) _temp727)->f1; goto
_LL724;} else{ goto _LL725;} _LL725: goto _LL726; _LL724: { int _temp735=(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp733.nullable); goto
_LL736; _LL736: { int _temp737=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp729.nullable); goto _LL738; _LL738: { void* _temp739=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp733.bounds); goto _LL740;
_LL740: { void* _temp741=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp729.bounds); goto _LL742; _LL742: { struct _tuple7 _temp744=({ struct
_tuple7 _temp743; _temp743.f1= _temp739; _temp743.f2= _temp741; _temp743;});
void* _temp754; struct Cyc_Absyn_Exp* _temp756; void* _temp758; struct Cyc_Absyn_Exp*
_temp760; void* _temp762; void* _temp764; void* _temp766; void* _temp768; struct
Cyc_Absyn_Exp* _temp770; void* _temp772; struct Cyc_Absyn_Exp* _temp774; void*
_temp776; _LL746: _LL759: _temp758= _temp744.f1; if(( unsigned int) _temp758 > 1u?*((
int*) _temp758) == Cyc_Absyn_Upper_b: 0){ _LL761: _temp760=(( struct Cyc_Absyn_Upper_b_struct*)
_temp758)->f1; goto _LL755;} else{ goto _LL748;} _LL755: _temp754= _temp744.f2;
if(( unsigned int) _temp754 > 1u?*(( int*) _temp754) == Cyc_Absyn_Upper_b: 0){
_LL757: _temp756=(( struct Cyc_Absyn_Upper_b_struct*) _temp754)->f1; goto _LL747;}
else{ goto _LL748;} _LL748: _LL765: _temp764= _temp744.f1; if( _temp764 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL763;} else{ goto _LL750;} _LL763: _temp762=
_temp744.f2; if( _temp762 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL749;} else{
goto _LL750;} _LL750: _LL769: _temp768= _temp744.f1; if(( unsigned int) _temp768
> 1u?*(( int*) _temp768) == Cyc_Absyn_Upper_b: 0){ _LL771: _temp770=(( struct
Cyc_Absyn_Upper_b_struct*) _temp768)->f1; goto _LL767;} else{ goto _LL752;}
_LL767: _temp766= _temp744.f2; if( _temp766 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL751;} else{ goto _LL752;} _LL752: _LL777: _temp776= _temp744.f1; if(
_temp776 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL773;} else{ goto _LL745;}
_LL773: _temp772= _temp744.f2; if(( unsigned int) _temp772 > 1u?*(( int*)
_temp772) == Cyc_Absyn_Upper_b: 0){ _LL775: _temp774=(( struct Cyc_Absyn_Upper_b_struct*)
_temp772)->f1; goto _LL753;} else{ goto _LL745;} _LL747: if( _temp735? !
_temp737: 0){ return( void*)({ struct Cyc_Toc_NullCheck_struct* _temp778=(
struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp778[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp779; _temp779.tag= Cyc_Toc_NullCheck;
_temp779.f1=( void*) t2; _temp779;}); _temp778;});} return( void*) Cyc_Toc_NoConv;
_LL749: return( void*) Cyc_Toc_NoConv; _LL751: return( void*)({ struct Cyc_Toc_TagPtr_struct*
_temp780=( struct Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp780[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp781; _temp781.tag= Cyc_Toc_TagPtr;
_temp781.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp781.f2= _temp770; _temp781;});
_temp780;}); _LL753: return( void*)({ struct Cyc_Toc_UntagPtr_struct* _temp782=(
struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp782[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp783; _temp783.tag= Cyc_Toc_UntagPtr;
_temp783.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp783.f2= _temp774; _temp783.f3=
_temp737; _temp783;}); _temp782;}); _LL745:;}}}}} _LL726: return( void*) Cyc_Toc_NoConv;
_LL722:;}} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp*
e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__throw_qv, 0),({
struct Cyc_List_List* _temp784=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp784->hd=( void*) e; _temp784->tl= 0; _temp784;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ char* _temp785="can't do null-check conversion at top-level";
struct _tagged_string _temp786; _temp786.curr= _temp785; _temp786.base= _temp785;
_temp786.last_plus_one= _temp785 + 44; _temp786;}));} if( Cyc_Toc_use_runtimecheck_funs){
return Cyc_Absyn_cast_exp( new_typ, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp787=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp787->hd=( void*) e; _temp787->tl= 0; _temp787;}), 0),
0);} else{ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0),
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0),
Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
x, 0), 0), 0); return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, new_typ,(
struct Cyc_Absyn_Exp*) e, s, 0), 0);}} static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv(
int toplevel, void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){
struct _tuple0* a= Cyc_Toc_temp_var(); struct _tuple0* b= Cyc_Toc_temp_var();
int szvalue=( int) Cyc_Evexp_eval_const_uint_exp( sz); void* strt_typ; void*
elt_typ;{ void* _temp788= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp794; struct Cyc_Absyn_Conref* _temp796; struct Cyc_Absyn_Tqual _temp798;
struct Cyc_Absyn_Conref* _temp800; void* _temp802; void* _temp804; _LL790: if((
unsigned int) _temp788 > 4u?*(( int*) _temp788) == Cyc_Absyn_PointerType: 0){
_LL795: _temp794=(( struct Cyc_Absyn_PointerType_struct*) _temp788)->f1; _LL805:
_temp804=( void*) _temp794.elt_typ; goto _LL803; _LL803: _temp802=( void*)
_temp794.rgn_typ; goto _LL801; _LL801: _temp800= _temp794.nullable; goto _LL799;
_LL799: _temp798= _temp794.tq; goto _LL797; _LL797: _temp796= _temp794.bounds;
goto _LL791;} else{ goto _LL792;} _LL792: goto _LL793; _LL791: elt_typ= _temp804;
strt_typ= Cyc_Toc_add_tagged_ptr_type( _temp804, Cyc_Toc_mt_tq); goto _LL789;
_LL793: return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp806="tag_ptr_conv: not a pointer"; struct _tagged_string _temp807;
_temp807.curr= _temp806; _temp807.base= _temp806; _temp807.last_plus_one=
_temp806 + 28; _temp807;})); _LL789:;} if( toplevel){ int is_string= 0;{ void*
_temp808=( void*) e->r; void* _temp814; struct _tagged_string _temp816; _LL810:
if(*(( int*) _temp808) == Cyc_Absyn_Const_e){ _LL815: _temp814=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp808)->f1; if(( unsigned int) _temp814 > 1u?*((
int*) _temp814) == Cyc_Absyn_String_c: 0){ _LL817: _temp816=(( struct Cyc_Absyn_String_c_struct*)
_temp814)->f1; goto _LL811;} else{ goto _LL812;}} else{ goto _LL812;} _LL812:
goto _LL813; _LL811: is_string= 1; goto _LL809; _LL813: goto _LL809; _LL809:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp821=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp821[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp822; _temp822.tag= Cyc_Absyn_ArrayType; _temp822.f1=(
void*) elt_typ; _temp822.f2= Cyc_Toc_mt_tq; _temp822.f3=( struct Cyc_Absyn_Exp*)
sz; _temp822;}); _temp821;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp818=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp818->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp819=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp819[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp820; _temp820.tag= Cyc_Absyn_Var_d;
_temp820.f1= vd; _temp820;}); _temp819;}), 0); _temp818->tl= Cyc_Toc_result_decls;
_temp818;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp823=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp823->hd=( void*)({ struct _tuple4* _temp828=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp828->f1= 0; _temp828->f2=
xexp; _temp828;}); _temp823->tl=({ struct Cyc_List_List* _temp824=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp824->hd=( void*)({ struct
_tuple4* _temp827=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp827->f1= 0; _temp827->f2= xexp; _temp827;}); _temp824->tl=({ struct Cyc_List_List*
_temp825=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp825->hd=( void*)({ struct _tuple4* _temp826=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp826->f1= 0; _temp826->f2= xplussz; _temp826;});
_temp825->tl= 0; _temp825;}); _temp824;}); _temp823;}), 0); return urm_exp;}}}
else{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0);
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
b, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_signed_int_exp( szvalue, 0), 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( b, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_var_exp(
a, 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( b, 0), Cyc_Toc_curr_sp, 0), Cyc_Absyn_var_exp( a, 0), 0), s,
0); s= Cyc_Absyn_declare_stmt( b, strt_typ, 0, s, 0); s= Cyc_Absyn_declare_stmt(
a, Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq),( struct Cyc_Absyn_Exp*) e, s, 0);
return Cyc_Absyn_stmt_exp( s, 0);}} static struct Cyc_Absyn_Exp* Cyc_Toc_untag_ptr_conv(
int toplevel, void* t1, struct Cyc_Absyn_Exp* sz, int nullable, struct Cyc_Absyn_Exp*
e){ if( toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ char*
_temp829="can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp830; _temp830.curr= _temp829; _temp830.base= _temp829; _temp830.last_plus_one=
_temp829 + 45; _temp830;}));}{ struct _tuple0* x= Cyc_Toc_temp_var(); int
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
return Cyc_Absyn_stmt_exp( s, 0);}} struct Cyc_Toc_Env{ struct Cyc_Core_Opt*
break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt* fallthru_info;
struct Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct Cyc_Toc_Env* Cyc_Toc_env_t;
static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({ struct Cyc_Toc_Env*
_temp831=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct Cyc_Toc_Env));
_temp831->break_lab= 0; _temp831->continue_lab= 0; _temp831->fallthru_info= 0;
_temp831->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp831->toplevel= 1;
_temp831;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp832=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp832->break_lab= e->break_lab; _temp832->continue_lab=
e->continue_lab; _temp832->fallthru_info= e->fallthru_info; _temp832->varmap= e->varmap;
_temp832->toplevel= e->toplevel; _temp832;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp833=(* x).f1; struct
Cyc_List_List* _temp839; _LL835: if(( unsigned int) _temp833 > 1u?*(( int*)
_temp833) == Cyc_Absyn_Rel_n: 0){ _LL840: _temp839=(( struct Cyc_Absyn_Rel_n_struct*)
_temp833)->f1; goto _LL836;} else{ goto _LL837;} _LL837: goto _LL838; _LL836:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp841=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp841[ 0]=({ struct
Cyc_Core_Impossible_struct _temp842; _temp842.tag= Cyc_Core_Impossible; _temp842.f1=({
struct _tagged_string _temp843= Cyc_Absynpp_qvar2string( x); xprintf("Toc::add_varmap on Rel_n: %.*s\n",
_temp843.last_plus_one - _temp843.curr, _temp843.curr);}); _temp842;}); _temp841;}));
_LL838: goto _LL834; _LL834:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap, x, y); return
ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple8{ struct _tagged_string* f1; struct Cyc_List_List*
f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l, struct _tagged_string*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp844= 0; goto _LL845; _LL845: for( 0;
fallthru_binders != 0; fallthru_binders=(( struct Cyc_List_List*) _check_null(
fallthru_binders))->tl){ _temp844=({ struct Cyc_List_List* _temp846=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp846->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp846->tl=
_temp844; _temp846;});} _temp844=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp844);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp847=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp847->v=( void*) break_l; _temp847;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp848=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp848->v=( void*)({ struct _tuple8*
_temp849=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp849->f1=
fallthru_l; _temp849->f2= _temp844; _temp849->f3= next_case_env->varmap;
_temp849;}); _temp848;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp850=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp850->v=( void*)
break_l; _temp850;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_string*
next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp851=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp851->v=( void*)({ struct _tuple8* _temp852=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp852->f1= next_l; _temp852->f2= 0;
_temp852->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp852;}); _temp851;});
return ans;} struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; char f5; int f6; } ; struct
_tuple10{ struct _tuple0* f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; static
struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c( void* p, struct Cyc_Core_Opt* aopt,
struct Cyc_List_List* args, struct Cyc_List_List* typs){ if( args == 0){ return((
struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp853="expecting a literal format string"; struct _tagged_string _temp854;
_temp854.curr= _temp853; _temp854.base= _temp853; _temp854.last_plus_one=
_temp853 + 34; _temp854;}));}{ struct _tagged_string fmt_str;{ void* _temp855=(
void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd)->r;
void* _temp861; struct _tagged_string _temp863; _LL857: if(*(( int*) _temp855)
== Cyc_Absyn_Const_e){ _LL862: _temp861=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp855)->f1; if(( unsigned int) _temp861 > 1u?*(( int*) _temp861) == Cyc_Absyn_String_c:
0){ _LL864: _temp863=(( struct Cyc_Absyn_String_c_struct*) _temp861)->f1; goto
_LL858;} else{ goto _LL859;}} else{ goto _LL859;} _LL859: goto _LL860; _LL858:
fmt_str= _temp863; goto _LL856; _LL860: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp865="expecting a literal format string";
struct _tagged_string _temp866; _temp866.curr= _temp865; _temp866.base= _temp865;
_temp866.last_plus_one= _temp865 + 34; _temp866;})); _LL856:;}{ int len=( int)({
struct _tagged_string _temp968= fmt_str;( unsigned int)( _temp968.last_plus_one
- _temp968.curr);}); int i= 0; for( i= 0; i < len; i ++){ if(*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
fmt_str, sizeof( char), i) =='%'){ i ++; if( i < len?*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str,
sizeof( char), i) =='s': 0){ break;}}} if( i == len){ if( aopt != 0){ args=({
struct Cyc_List_List* _temp867=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp867->hd=( void*)(( struct Cyc_Absyn_Exp*)(( struct
Cyc_Core_Opt*) _check_null( aopt))->v); _temp867->tl= args; _temp867;});} return
Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*) _check_null(
args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{ struct Cyc_List_List*
rev_fmt= 0; struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps=
0; for( i= 0; i < len; i ++){ char c=*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str, sizeof( char),
i); rev_fmt=({ struct Cyc_List_List* _temp868=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp868->hd=( void*)(( int) c); _temp868->tl=
rev_fmt; _temp868;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* _temp869=
Cyc_Formatstr_parse_conversionspecification( fmt_str, i + 1); goto _LL870;
_LL870: if( _temp869 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp871="bad format string"; struct _tagged_string _temp872; _temp872.curr=
_temp871; _temp872.base= _temp871; _temp872.last_plus_one= _temp871 + 18;
_temp872;}));}{ struct _tuple9 _temp875; int _temp876; char _temp878; struct Cyc_List_List*
_temp880; struct Cyc_List_List* _temp882; struct Cyc_List_List* _temp884; struct
Cyc_List_List* _temp886; struct _tuple9* _temp873=( struct _tuple9*)(( struct
Cyc_Core_Opt*) _check_null( _temp869))->v; _temp875=* _temp873; _LL887: _temp886=
_temp875.f1; goto _LL885; _LL885: _temp884= _temp875.f2; goto _LL883; _LL883:
_temp882= _temp875.f3; goto _LL881; _LL881: _temp880= _temp875.f4; goto _LL879;
_LL879: _temp878= _temp875.f5; goto _LL877; _LL877: _temp876= _temp875.f6; goto
_LL874; _LL874: i= _temp876 - 1; if( _temp878 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct Cyc_List_List*
_temp888=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp888->hd=( void*) _temp886; _temp888->tl=({ struct Cyc_List_List* _temp889=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp889->hd=(
void*) _temp884; _temp889->tl=({ struct Cyc_List_List* _temp890=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp890->hd=( void*) _temp882;
_temp890->tl=({ struct Cyc_List_List* _temp891=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp891->hd=( void*) _temp880;
_temp891->tl= 0; _temp891;}); _temp890;}); _temp889;}); _temp888;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp892=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp892->hd=( void*)(( int) _temp878); _temp892->tl=
rev_fmt; _temp892;});{ struct Cyc_List_List* _temp893= _temp884; struct Cyc_List_List
_temp900; struct Cyc_List_List* _temp901; int _temp903; _LL895: if( _temp893 ==
0){ goto _LL897;} else{ _temp900=* _temp893; _LL904: _temp903=( int) _temp900.hd;
goto _LL902; _LL902: _temp901= _temp900.tl; if( _temp901 == 0){ goto _LL899;}
else{ goto _LL897;}} _LL899: if( _temp903 =='*'){ goto _LL896;} else{ goto
_LL897;} _LL897: goto _LL898; _LL896: { struct _tuple0* _temp905= Cyc_Toc_temp_var();
goto _LL906; _LL906: rev_temps=({ struct Cyc_List_List* _temp907=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp907->hd=( void*)({ struct
_tuple10* _temp908=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp908->f1= _temp905; _temp908->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp908->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp908;}); _temp907->tl= rev_temps; _temp907;});
rev_result=({ struct Cyc_List_List* _temp909=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp909->hd=( void*) Cyc_Absyn_var_exp(
_temp905, 0); _temp909->tl= rev_result; _temp909;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL894;} _LL898: goto _LL894; _LL894:;}{ struct Cyc_List_List* _temp910=
_temp882; struct Cyc_List_List _temp917; struct Cyc_List_List* _temp918; struct
Cyc_List_List _temp920; struct Cyc_List_List* _temp921; int _temp923; int
_temp925; _LL912: if( _temp910 == 0){ goto _LL914;} else{ _temp917=* _temp910;
_LL926: _temp925=( int) _temp917.hd; goto _LL919; _LL919: _temp918= _temp917.tl;
if( _temp918 == 0){ goto _LL914;} else{ _temp920=* _temp918; _LL924: _temp923=(
int) _temp920.hd; goto _LL922; _LL922: _temp921= _temp920.tl; if( _temp921 == 0){
goto _LL916;} else{ goto _LL914;}}} _LL916: if( _temp925 =='.'? _temp923 =='*':
0){ goto _LL913;} else{ goto _LL914;} _LL914: goto _LL915; _LL913: { struct
_tuple0* _temp927= Cyc_Toc_temp_var(); goto _LL928; _LL928: rev_temps=({ struct
Cyc_List_List* _temp929=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp929->hd=( void*)({ struct _tuple10* _temp930=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp930->f1= _temp927; _temp930->f2=( void*)((
struct Cyc_List_List*) _check_null( typs))->hd; _temp930->f3=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd; _temp930;}); _temp929->tl=
rev_temps; _temp929;}); rev_result=({ struct Cyc_List_List* _temp931=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp931->hd=( void*)
Cyc_Absyn_var_exp( _temp927, 0); _temp931->tl= rev_result; _temp931;}); args=((
struct Cyc_List_List*) _check_null( args))->tl; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl; goto _LL911;} _LL915: goto _LL911; _LL911:;} if(
_temp878 !='%'){ if( args == 0? 1: typs == 0){(( void(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp932= Cyc_Absynpp_prim2string(
p); xprintf("missing arg to %.*s", _temp932.last_plus_one - _temp932.curr,
_temp932.curr);}));}{ struct _tuple0* _temp933= Cyc_Toc_temp_var(); goto _LL934;
_LL934: rev_temps=({ struct Cyc_List_List* _temp935=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp935->hd=( void*)({ struct
_tuple10* _temp936=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp936->f1= _temp933; _temp936->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp936->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp936;}); _temp935->tl= rev_temps; _temp935;});
rev_result=({ struct Cyc_List_List* _temp937=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp937->hd=( void*) Cyc_Absyn_var_exp(
_temp933, 0); _temp937->tl= rev_result; _temp937;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp938=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp938->hd=( void*)(( int)'.');
_temp938->tl= rev_fmt; _temp938;}); rev_fmt=({ struct Cyc_List_List* _temp939=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp939->hd=(
void*)(( int)'*'); _temp939->tl= rev_fmt; _temp939;}); rev_fmt=({ struct Cyc_List_List*
_temp940=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp940->hd=( void*)(( int)'s'); _temp940->tl= rev_fmt; _temp940;});{ struct
_tuple0* _temp941= Cyc_Toc_temp_var(); goto _LL942; _LL942: rev_temps=({ struct
Cyc_List_List* _temp943=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp943->hd=( void*)({ struct _tuple10* _temp944=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp944->f1= _temp941; _temp944->f2=( void*)((
struct Cyc_List_List*) _check_null( typs))->hd; _temp944->f3=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd; _temp944;}); _temp943->tl=
rev_temps; _temp943;});{ struct Cyc_Absyn_Exp* _temp945= Cyc_Absyn_subtract_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( _temp941, 0), Cyc_Toc_last_plus_one_sp,
0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( _temp941, 0), Cyc_Toc_curr_sp,
0), 0); goto _LL946; _LL946: { struct Cyc_Absyn_Exp* _temp947= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp941, 0), Cyc_Toc_curr_sp, 0); goto _LL948; _LL948:
rev_result=({ struct Cyc_List_List* _temp949=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp949->hd=( void*) _temp945; _temp949->tl=
rev_result; _temp949;}); rev_result=({ struct Cyc_List_List* _temp950=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp950->hd=( void*)
_temp947; _temp950->tl= rev_result; _temp950;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}}{
struct Cyc_List_List* _temp952=({ struct Cyc_List_List* _temp951=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp951->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
rev_fmt)), 0); _temp951->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( rev_result); _temp951;}); goto _LL953; _LL953: if( aopt != 0){
_temp952=({ struct Cyc_List_List* _temp954=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp954->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp954->tl= _temp952; _temp954;});}{
struct Cyc_Absyn_Exp* _temp955= Cyc_Absyn_primop_exp( p, _temp952, 0); goto
_LL956; _LL956: { struct Cyc_Absyn_Stmt* _temp957= Cyc_Absyn_exp_stmt( _temp955,
0); goto _LL958; _LL958: for( 0; rev_temps != 0; rev_temps=(( struct Cyc_List_List*)
_check_null( rev_temps))->tl){ struct _tuple10 _temp961; struct Cyc_Absyn_Exp*
_temp962; void* _temp964; struct _tuple0* _temp966; struct _tuple10* _temp959=(
struct _tuple10*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd;
_temp961=* _temp959; _LL967: _temp966= _temp961.f1; goto _LL965; _LL965:
_temp964= _temp961.f2; goto _LL963; _LL963: _temp962= _temp961.f3; goto _LL960;
_LL960: _temp957= Cyc_Absyn_declare_stmt( _temp966, _temp964,( struct Cyc_Absyn_Exp*)
_temp962, _temp957, 0);} return Cyc_Absyn_stmt_exp( _temp957, 0);}}}}}}} static
void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp969="Missing type in primop ";
struct _tagged_string _temp970; _temp970.curr= _temp969; _temp970.base= _temp969;
_temp970.last_plus_one= _temp969 + 24; _temp970;}));} return Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ char* _temp971="Missing type in primop "; struct
_tagged_string _temp972; _temp972.curr= _temp971; _temp972.base= _temp971;
_temp972.last_plus_one= _temp971 + 24; _temp972;}));} return( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp973=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp973->f1= Cyc_Toc_mt_tq; _temp973->f2=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_temp973;});} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4*
_temp974=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp974[ 0]=({
struct _tuple4 _temp975; _temp975.f1= 0; _temp975.f2= e; _temp975;}); _temp974;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp976= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); goto _LL977; _LL977: if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic( _temp976): Cyc_Toc_malloc_ptr(
_temp976), 0);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( rgnopt); Cyc_Toc_exp_to_c( nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, Cyc_Toc_rmalloc_exp( r, _temp976), 0);}}} else{ t= struct_typ; eo= 0;} return
Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, t, eo, s, 0), 0);} static struct
Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Absyn_Stmt* s, int e1_already_translated); static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s); static
struct Cyc_Absyn_Stmt* Cyc_Toc_init_array( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
lhs, struct Cyc_List_List* dles0, struct Cyc_Absyn_Stmt* s){ int count=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( dles0) - 1;{ struct Cyc_List_List*
_temp978=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
dles0); goto _LL979; _LL979: for( 0; _temp978 != 0; _temp978=(( struct Cyc_List_List*)
_check_null( _temp978))->tl){ struct _tuple4 _temp982; struct Cyc_Absyn_Exp*
_temp983; struct Cyc_List_List* _temp985; struct _tuple4* _temp980=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp978))->hd; _temp982=*
_temp980; _LL986: _temp985= _temp982.f1; goto _LL984; _LL984: _temp983= _temp982.f2;
goto _LL981; _LL981: { struct Cyc_Absyn_Exp* e_index; if( _temp985 == 0){
e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if((( struct Cyc_List_List*)
_check_null( _temp985))->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
char* _temp987="multiple designators in array"; struct _tagged_string _temp988;
_temp988.curr= _temp987; _temp988.base= _temp987; _temp988.last_plus_one=
_temp987 + 30; _temp988;}));}{ void* _temp989=( void*)(( struct Cyc_List_List*)
_check_null( _temp985))->hd; goto _LL990; _LL990: { void* _temp991= _temp989;
struct Cyc_Absyn_Exp* _temp997; struct _tagged_string* _temp999; _LL993: if(*((
int*) _temp991) == Cyc_Absyn_ArrayElement){ _LL998: _temp997=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp991)->f1; goto _LL994;} else{ goto _LL995;} _LL995: if(*(( int*) _temp991)
== Cyc_Absyn_FieldName){ _LL1000: _temp999=(( struct Cyc_Absyn_FieldName_struct*)
_temp991)->f1; goto _LL996;} else{ goto _LL992;} _LL994: Cyc_Toc_exp_to_c( nv,
_temp997); e_index= _temp997; goto _LL992; _LL996: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ char* _temp1001="field name designators in array";
struct _tagged_string _temp1002; _temp1002.curr= _temp1001; _temp1002.base=
_temp1001; _temp1002.last_plus_one= _temp1001 + 32; _temp1002;})); goto _LL992;
_LL992:;}}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index,
0); void* _temp1003=( void*) _temp983->r; struct Cyc_List_List* _temp1013;
struct Cyc_Absyn_Exp* _temp1015; struct Cyc_Absyn_Exp* _temp1017; struct Cyc_Absyn_Vardecl*
_temp1019; struct Cyc_List_List* _temp1021; void* _temp1023; _LL1005: if(*(( int*)
_temp1003) == Cyc_Absyn_Array_e){ _LL1014: _temp1013=(( struct Cyc_Absyn_Array_e_struct*)
_temp1003)->f1; goto _LL1006;} else{ goto _LL1007;} _LL1007: if(*(( int*)
_temp1003) == Cyc_Absyn_Comprehension_e){ _LL1020: _temp1019=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1003)->f1; goto _LL1018; _LL1018: _temp1017=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1003)->f2; goto _LL1016; _LL1016: _temp1015=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1003)->f3; goto _LL1008;} else{ goto _LL1009;} _LL1009: if(*(( int*)
_temp1003) == Cyc_Absyn_AnonStruct_e){ _LL1024: _temp1023=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1003)->f1; goto _LL1022; _LL1022: _temp1021=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1003)->f2; goto _LL1010;} else{ goto _LL1011;} _LL1011: goto _LL1012;
_LL1006: s= Cyc_Toc_init_array( nv, lval, _temp1013, s); goto _LL1004; _LL1008:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1019, _temp1017, _temp1015, s, 0);
goto _LL1004; _LL1010: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1023,
_temp1021, s); goto _LL1004; _LL1012: Cyc_Toc_exp_to_c( nv, _temp983); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp983, 0),
s, 0); goto _LL1004; _LL1004:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* _temp1025= vd->name; goto _LL1026;
_LL1026: if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp1025, Cyc_Absyn_varb_exp( _temp1025,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1049=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1049[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1050; _temp1050.tag= Cyc_Absyn_Local_b; _temp1050.f1=
vd; _temp1050;}); _temp1049;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp1025, 0),
Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( _temp1025, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp*
ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( _temp1025, 0), 0); struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( _temp1025, 0), 0); struct
Cyc_Absyn_Stmt* body;{ void* _temp1027=( void*) e2->r; struct Cyc_List_List*
_temp1037; struct Cyc_Absyn_Exp* _temp1039; struct Cyc_Absyn_Exp* _temp1041;
struct Cyc_Absyn_Vardecl* _temp1043; struct Cyc_List_List* _temp1045; void*
_temp1047; _LL1029: if(*(( int*) _temp1027) == Cyc_Absyn_Array_e){ _LL1038:
_temp1037=(( struct Cyc_Absyn_Array_e_struct*) _temp1027)->f1; goto _LL1030;}
else{ goto _LL1031;} _LL1031: if(*(( int*) _temp1027) == Cyc_Absyn_Comprehension_e){
_LL1044: _temp1043=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1027)->f1;
goto _LL1042; _LL1042: _temp1041=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1027)->f2; goto _LL1040; _LL1040: _temp1039=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1027)->f3; goto _LL1032;} else{ goto _LL1033;} _LL1033: if(*(( int*)
_temp1027) == Cyc_Absyn_AnonStruct_e){ _LL1048: _temp1047=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1027)->f1; goto _LL1046; _LL1046: _temp1045=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1027)->f2; goto _LL1034;} else{ goto _LL1035;} _LL1035: goto _LL1036;
_LL1030: body= Cyc_Toc_init_array( nv2, lval, _temp1037, Cyc_Absyn_skip_stmt( 0));
goto _LL1028; _LL1032: body= Cyc_Toc_init_comprehension( nv2, lval, _temp1043,
_temp1041, _temp1039, Cyc_Absyn_skip_stmt( 0), 0); goto _LL1028; _LL1034: body=
Cyc_Toc_init_anon_struct( nv, lval, _temp1047, _temp1045, Cyc_Absyn_skip_stmt( 0));
goto _LL1028; _LL1036: Cyc_Toc_exp_to_c( nv2, e2); body= Cyc_Absyn_assign_stmt(
lval, e2, 0); goto _LL1028; _LL1028:;}{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt(
ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( _temp1025, Cyc_Absyn_uint_t,
0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s){{ struct Cyc_List_List* _temp1051=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto _LL1052; _LL1052: for( 0;
_temp1051 != 0; _temp1051=(( struct Cyc_List_List*) _check_null( _temp1051))->tl){
struct _tuple4 _temp1055; struct Cyc_Absyn_Exp* _temp1056; struct Cyc_List_List*
_temp1058; struct _tuple4* _temp1053=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1051))->hd; _temp1055=* _temp1053; _LL1059: _temp1058=
_temp1055.f1; goto _LL1057; _LL1057: _temp1056= _temp1055.f2; goto _LL1054;
_LL1054: if( _temp1058 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp1060="empty designator list"; struct _tagged_string _temp1061;
_temp1061.curr= _temp1060; _temp1061.base= _temp1060; _temp1061.last_plus_one=
_temp1060 + 22; _temp1061;}));} if((( struct Cyc_List_List*) _check_null(
_temp1058))->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp1062="too many designators in anonymous struct"; struct
_tagged_string _temp1063; _temp1063.curr= _temp1062; _temp1063.base= _temp1062;
_temp1063.last_plus_one= _temp1062 + 41; _temp1063;}));}{ void* _temp1064=( void*)((
struct Cyc_List_List*) _check_null( _temp1058))->hd; struct _tagged_string*
_temp1070; _LL1066: if(*(( int*) _temp1064) == Cyc_Absyn_FieldName){ _LL1071:
_temp1070=(( struct Cyc_Absyn_FieldName_struct*) _temp1064)->f1; goto _LL1067;}
else{ goto _LL1068;} _LL1068: goto _LL1069; _LL1067: { struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_structmember_exp( lhs, _temp1070, 0);{ void* _temp1072=( void*)
_temp1056->r; struct Cyc_List_List* _temp1082; struct Cyc_Absyn_Exp* _temp1084;
struct Cyc_Absyn_Exp* _temp1086; struct Cyc_Absyn_Vardecl* _temp1088; struct Cyc_List_List*
_temp1090; void* _temp1092; _LL1074: if(*(( int*) _temp1072) == Cyc_Absyn_Array_e){
_LL1083: _temp1082=(( struct Cyc_Absyn_Array_e_struct*) _temp1072)->f1; goto
_LL1075;} else{ goto _LL1076;} _LL1076: if(*(( int*) _temp1072) == Cyc_Absyn_Comprehension_e){
_LL1089: _temp1088=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1072)->f1;
goto _LL1087; _LL1087: _temp1086=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1072)->f2; goto _LL1085; _LL1085: _temp1084=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1072)->f3; goto _LL1077;} else{ goto _LL1078;} _LL1078: if(*(( int*)
_temp1072) == Cyc_Absyn_AnonStruct_e){ _LL1093: _temp1092=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1072)->f1; goto _LL1091; _LL1091: _temp1090=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1072)->f2; goto _LL1079;} else{ goto _LL1080;} _LL1080: goto _LL1081;
_LL1075: s= Cyc_Toc_init_array( nv, lval, _temp1082, s); goto _LL1073; _LL1077:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1088, _temp1086, _temp1084, s, 0);
goto _LL1073; _LL1079: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1092,
_temp1090, s); goto _LL1073; _LL1081: Cyc_Toc_exp_to_c( nv, _temp1056); if( Cyc_Toc_is_poly_field(
struct_type, _temp1070)){ _temp1056= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1056, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1056, 0), 0), s, 0); goto _LL1073; _LL1073:;} goto _LL1065;} _LL1069:(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp1094="array designator in struct"; struct _tagged_string _temp1095;
_temp1095.curr= _temp1094; _temp1095.base= _temp1094; _temp1095.last_plus_one=
_temp1094 + 27; _temp1095;}))); _LL1065:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct Cyc_List_List* _temp1096=(( struct Cyc_List_List*(*)(
struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, es); goto _LL1097; _LL1097: { struct _tagged_string* _temp1098=
Cyc_Toc_add_tuple_type( _temp1096); goto _LL1099; _LL1099: { struct _tuple0*
_temp1100= Cyc_Toc_temp_var(); goto _LL1101; _LL1101: { struct Cyc_Absyn_Exp*
_temp1102= Cyc_Absyn_var_exp( _temp1100, 0); goto _LL1103; _LL1103: { struct Cyc_Absyn_Stmt*
_temp1104= Cyc_Absyn_exp_stmt( _temp1102, 0); goto _LL1105; _LL1105: { struct
Cyc_Absyn_Exp*(* _temp1106)( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
goto _LL1107; _LL1107: { int is_atomic= 1; es=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( es);{ int i=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es); for( 0; es != 0;( es=(( struct Cyc_List_List*)
_check_null( es))->tl, -- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; struct Cyc_Absyn_Exp* lval=
_temp1106( _temp1102,({ struct _tagged_string* _temp1125=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1125[ 0]= xprintf("f%d", i);
_temp1125;}), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp1108=( void*) e->r; struct Cyc_List_List*
_temp1116; struct Cyc_Absyn_Exp* _temp1118; struct Cyc_Absyn_Exp* _temp1120;
struct Cyc_Absyn_Vardecl* _temp1122; _LL1110: if(*(( int*) _temp1108) == Cyc_Absyn_Array_e){
_LL1117: _temp1116=(( struct Cyc_Absyn_Array_e_struct*) _temp1108)->f1; goto
_LL1111;} else{ goto _LL1112;} _LL1112: if(*(( int*) _temp1108) == Cyc_Absyn_Comprehension_e){
_LL1123: _temp1122=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1108)->f1;
goto _LL1121; _LL1121: _temp1120=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1108)->f2; goto _LL1119; _LL1119: _temp1118=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1108)->f3; goto _LL1113;} else{ goto _LL1114;} _LL1114: goto _LL1115;
_LL1111: _temp1104= Cyc_Toc_init_array( nv, lval, _temp1116, _temp1104); goto
_LL1109; _LL1113: _temp1104= Cyc_Toc_init_comprehension( nv, lval, _temp1122,
_temp1120, _temp1118, _temp1104, 0); goto _LL1109; _LL1115: Cyc_Toc_exp_to_c( nv,
e); _temp1104= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1106( _temp1102,({ struct _tagged_string* _temp1124=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1124[ 0]= xprintf("f%d", i);
_temp1124;}), 0), e, 0), 0), _temp1104, 0); goto _LL1109; _LL1109:;}}} return
Cyc_Toc_make_struct( nv, _temp1100, Cyc_Absyn_strct( _temp1098), _temp1104,
pointer, rgnopt, is_atomic);}}}}}}} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct(
struct Cyc_Toc_Env* nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* dles, struct _tuple0* tdn){ struct _tuple0*
_temp1126= Cyc_Toc_temp_var(); goto _LL1127; _LL1127: { struct Cyc_Absyn_Exp*
_temp1128= Cyc_Absyn_var_exp( _temp1126, 0); goto _LL1129; _LL1129: { struct Cyc_Absyn_Stmt*
_temp1130= Cyc_Absyn_exp_stmt( _temp1128, 0); goto _LL1131; _LL1131: { struct
Cyc_Absyn_Exp*(* _temp1132)( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
goto _LL1133; _LL1133: { int is_atomic= 1;{ struct Cyc_List_List* _temp1134=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto
_LL1135; _LL1135: for( 0; _temp1134 != 0; _temp1134=(( struct Cyc_List_List*)
_check_null( _temp1134))->tl){ struct _tuple4 _temp1138; struct Cyc_Absyn_Exp*
_temp1139; struct Cyc_List_List* _temp1141; struct _tuple4* _temp1136=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp1134))->hd; _temp1138=*
_temp1136; _LL1142: _temp1141= _temp1138.f1; goto _LL1140; _LL1140: _temp1139=
_temp1138.f2; goto _LL1137; _LL1137: is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1139->topt))->v): 0; if(
_temp1141 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp1143="empty designator list"; struct _tagged_string _temp1144; _temp1144.curr=
_temp1143; _temp1144.base= _temp1143; _temp1144.last_plus_one= _temp1143 + 22;
_temp1144;}));} if((( struct Cyc_List_List*) _check_null( _temp1141))->tl != 0){
struct _tuple0* _temp1145= Cyc_Toc_temp_var(); goto _LL1146; _LL1146: { struct
Cyc_Absyn_Exp* _temp1147= Cyc_Absyn_var_exp( _temp1145, 0); goto _LL1148;
_LL1148: for( 0; _temp1141 != 0; _temp1141=(( struct Cyc_List_List*) _check_null(
_temp1141))->tl){ void* _temp1149=( void*)(( struct Cyc_List_List*) _check_null(
_temp1141))->hd; struct _tagged_string* _temp1155; _LL1151: if(*(( int*)
_temp1149) == Cyc_Absyn_FieldName){ _LL1156: _temp1155=(( struct Cyc_Absyn_FieldName_struct*)
_temp1149)->f1; goto _LL1152;} else{ goto _LL1153;} _LL1153: goto _LL1154;
_LL1152: if( Cyc_Toc_is_poly_field( struct_type, _temp1155)){ _temp1147= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1147, 0);} _temp1130= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp1132( _temp1128, _temp1155, 0), _temp1147, 0), 0),
_temp1130, 0); goto _LL1150; _LL1154:( void) _throw((( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp1157="array designator in struct";
struct _tagged_string _temp1158; _temp1158.curr= _temp1157; _temp1158.base=
_temp1157; _temp1158.last_plus_one= _temp1157 + 27; _temp1158;}))); _LL1150:;}
Cyc_Toc_exp_to_c( nv, _temp1139); _temp1130= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp1147, _temp1139, 0), 0), _temp1130, 0);}} else{ void*
_temp1159=( void*)(( struct Cyc_List_List*) _check_null( _temp1141))->hd; struct
_tagged_string* _temp1165; _LL1161: if(*(( int*) _temp1159) == Cyc_Absyn_FieldName){
_LL1166: _temp1165=(( struct Cyc_Absyn_FieldName_struct*) _temp1159)->f1; goto
_LL1162;} else{ goto _LL1163;} _LL1163: goto _LL1164; _LL1162: { struct Cyc_Absyn_Exp*
lval= _temp1132( _temp1128, _temp1165, 0);{ void* _temp1167=( void*) _temp1139->r;
struct Cyc_List_List* _temp1177; struct Cyc_Absyn_Exp* _temp1179; struct Cyc_Absyn_Exp*
_temp1181; struct Cyc_Absyn_Vardecl* _temp1183; struct Cyc_List_List* _temp1185;
void* _temp1187; _LL1169: if(*(( int*) _temp1167) == Cyc_Absyn_Array_e){ _LL1178:
_temp1177=(( struct Cyc_Absyn_Array_e_struct*) _temp1167)->f1; goto _LL1170;}
else{ goto _LL1171;} _LL1171: if(*(( int*) _temp1167) == Cyc_Absyn_Comprehension_e){
_LL1184: _temp1183=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1167)->f1;
goto _LL1182; _LL1182: _temp1181=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1167)->f2; goto _LL1180; _LL1180: _temp1179=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1167)->f3; goto _LL1172;} else{ goto _LL1173;} _LL1173: if(*(( int*)
_temp1167) == Cyc_Absyn_AnonStruct_e){ _LL1188: _temp1187=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1167)->f1; goto _LL1186; _LL1186: _temp1185=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1167)->f2; goto _LL1174;} else{ goto _LL1175;} _LL1175: goto _LL1176;
_LL1170: _temp1130= Cyc_Toc_init_array( nv, lval, _temp1177, _temp1130); goto
_LL1168; _LL1172: _temp1130= Cyc_Toc_init_comprehension( nv, lval, _temp1183,
_temp1181, _temp1179, _temp1130, 0); goto _LL1168; _LL1174: _temp1130= Cyc_Toc_init_anon_struct(
nv, lval, _temp1187, _temp1185, _temp1130); goto _LL1168; _LL1176: Cyc_Toc_exp_to_c(
nv, _temp1139); if( Cyc_Toc_is_poly_field( struct_type, _temp1165)){ _temp1139=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1139, 0);} _temp1130= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval, _temp1139, 0), 0), _temp1130, 0);
goto _LL1168; _LL1168:;} goto _LL1160;} _LL1164:( void) _throw((( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp1189="array designator in struct";
struct _tagged_string _temp1190; _temp1190.curr= _temp1189; _temp1190.base=
_temp1189; _temp1190.last_plus_one= _temp1189 + 27; _temp1190;}))); _LL1160:;}}}
return Cyc_Toc_make_struct( nv, _temp1126, Cyc_Absyn_strctq( tdn), _temp1130,
pointer, rgnopt, is_atomic);}}}}} struct _tuple11{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp(
el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp1191=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp1191[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp1192; _temp1192.tag=
Cyc_Absyn_Increment_e; _temp1192.f1= e1; _temp1192.f2=( void*) incr; _temp1192;});
_temp1191;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1193=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp1203; struct Cyc_Absyn_Exp* _temp1205; void* _temp1207; struct
_tagged_string* _temp1209; struct Cyc_Absyn_Exp* _temp1211; _LL1195: if(*(( int*)
_temp1193) == Cyc_Absyn_StmtExp_e){ _LL1204: _temp1203=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1193)->f1; goto _LL1196;} else{ goto _LL1197;} _LL1197: if(*(( int*)
_temp1193) == Cyc_Absyn_Cast_e){ _LL1208: _temp1207=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1193)->f1; goto _LL1206; _LL1206: _temp1205=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1193)->f2; goto _LL1198;} else{ goto _LL1199;} _LL1199: if(*(( int*)
_temp1193) == Cyc_Absyn_StructMember_e){ _LL1212: _temp1211=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1193)->f1; goto _LL1210; _LL1210: _temp1209=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1193)->f2; goto _LL1200;} else{ goto _LL1201;} _LL1201: goto _LL1202;
_LL1196:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1203, fs, f, f_env); goto _LL1194; _LL1198:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1205, fs, f, f_env); goto
_LL1194; _LL1200:( void*)( e1->r=( void*)(( void*) _temp1211->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1213=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1213->hd=( void*) _temp1209; _temp1213->tl= fs;
_temp1213;}), f, f_env); goto _LL1194; _LL1202: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1214= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1214.last_plus_one - _temp1214.curr, _temp1214.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1194;}
_LL1194:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1215=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1225; struct Cyc_Absyn_Stmt* _temp1227; struct Cyc_Absyn_Decl* _temp1229;
struct Cyc_Absyn_Stmt* _temp1231; struct Cyc_Absyn_Stmt* _temp1233; _LL1217: if((
unsigned int) _temp1215 > 1u?*(( int*) _temp1215) == Cyc_Absyn_Exp_s: 0){
_LL1226: _temp1225=(( struct Cyc_Absyn_Exp_s_struct*) _temp1215)->f1; goto
_LL1218;} else{ goto _LL1219;} _LL1219: if(( unsigned int) _temp1215 > 1u?*((
int*) _temp1215) == Cyc_Absyn_Decl_s: 0){ _LL1230: _temp1229=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1215)->f1; goto _LL1228; _LL1228: _temp1227=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1215)->f2; goto _LL1220;} else{ goto _LL1221;} _LL1221: if(( unsigned int)
_temp1215 > 1u?*(( int*) _temp1215) == Cyc_Absyn_Seq_s: 0){ _LL1234: _temp1233=((
struct Cyc_Absyn_Seq_s_struct*) _temp1215)->f1; goto _LL1232; _LL1232: _temp1231=((
struct Cyc_Absyn_Seq_s_struct*) _temp1215)->f2; goto _LL1222;} else{ goto
_LL1223;} _LL1223: goto _LL1224; _LL1218:(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1225, fs, f, f_env); goto
_LL1216; _LL1220:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1227, fs, f, f_env); goto _LL1216; _LL1222:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1231, fs, f, f_env);
goto _LL1216; _LL1224:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1235= Cyc_Absynpp_stmt2string( s); xprintf("lvalue_assign_stmt: %.*s",
_temp1235.last_plus_one - _temp1235.curr, _temp1235.curr);})); goto _LL1216;
_LL1216:;} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
s); struct _tuple12{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2;
} ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ void* _temp1236=( void*) e->r; goto _LL1237; _LL1237: if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1238= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp1238.last_plus_one - _temp1238.curr, _temp1238.curr);})); return;}{ void*
old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void*
_temp1239= _temp1236; void* _temp1315; void* _temp1317; void* _temp1319; struct
_tuple0* _temp1321; struct _tuple0* _temp1323; struct Cyc_List_List* _temp1325;
void* _temp1327; void* _temp1329; struct Cyc_Absyn_Exp* _temp1331; struct Cyc_Absyn_Exp*
_temp1333; struct Cyc_Core_Opt* _temp1335; struct Cyc_Absyn_Exp* _temp1337;
struct Cyc_Absyn_Exp* _temp1339; struct Cyc_Absyn_Exp* _temp1341; struct Cyc_Absyn_Exp*
_temp1343; struct Cyc_Absyn_Exp* _temp1345; struct Cyc_Absyn_Exp* _temp1347;
struct Cyc_List_List* _temp1349; struct Cyc_Absyn_Exp* _temp1351; struct Cyc_List_List*
_temp1353; struct Cyc_Absyn_Exp* _temp1355; struct Cyc_Absyn_Exp* _temp1357;
struct Cyc_Absyn_Exp* _temp1359; struct Cyc_List_List* _temp1361; struct Cyc_Absyn_Exp*
_temp1363; struct Cyc_Absyn_Exp* _temp1365; void* _temp1367; void** _temp1369;
struct Cyc_Absyn_Exp* _temp1370; struct Cyc_Absyn_Exp* _temp1372; struct Cyc_Absyn_Exp*
_temp1374; void* _temp1376; struct Cyc_Absyn_Exp* _temp1378; struct Cyc_Absyn_Exp*
_temp1380; struct _tagged_string* _temp1382; struct Cyc_Absyn_Exp* _temp1384;
struct _tagged_string* _temp1386; struct Cyc_Absyn_Exp* _temp1388; struct Cyc_Absyn_Exp*
_temp1390; struct Cyc_Absyn_Exp* _temp1392; struct Cyc_List_List* _temp1394;
struct Cyc_List_List* _temp1396; struct _tuple1* _temp1398; struct Cyc_List_List*
_temp1400; struct Cyc_Absyn_Exp* _temp1402; struct Cyc_Absyn_Exp* _temp1404;
struct Cyc_Absyn_Vardecl* _temp1406; struct Cyc_Absyn_Structdecl* _temp1408;
struct Cyc_List_List* _temp1410; struct Cyc_Core_Opt* _temp1412; struct _tuple0*
_temp1414; struct Cyc_List_List* _temp1416; void* _temp1418; struct Cyc_Absyn_Tunionfield*
_temp1420; struct Cyc_Absyn_Tuniondecl* _temp1422; struct Cyc_List_List*
_temp1424; struct Cyc_Core_Opt* _temp1426; struct Cyc_Core_Opt* _temp1428;
struct Cyc_Absyn_Tunionfield* _temp1430; struct Cyc_Absyn_Tuniondecl* _temp1432;
struct Cyc_List_List* _temp1434; struct Cyc_Core_Opt* _temp1436; struct Cyc_Core_Opt*
_temp1438; struct Cyc_Absyn_Enumfield* _temp1440; struct Cyc_Absyn_Enumdecl*
_temp1442; struct _tuple0* _temp1444; void* _temp1446; struct Cyc_Absyn_Exp*
_temp1448; struct Cyc_Absyn_Stmt* _temp1450; struct Cyc_List_List* _temp1452;
struct Cyc_Core_Opt* _temp1454; struct Cyc_Absyn_Fndecl* _temp1456; struct Cyc_Absyn_Exp*
_temp1458; _LL1241: if(*(( int*) _temp1239) == Cyc_Absyn_Const_e){ _LL1316:
_temp1315=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1239)->f1; if(
_temp1315 ==( void*) Cyc_Absyn_Null_c){ goto _LL1242;} else{ goto _LL1243;}}
else{ goto _LL1243;} _LL1243: if(*(( int*) _temp1239) == Cyc_Absyn_Const_e){
_LL1318: _temp1317=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1239)->f1;
goto _LL1244;} else{ goto _LL1245;} _LL1245: if(*(( int*) _temp1239) == Cyc_Absyn_Var_e){
_LL1322: _temp1321=(( struct Cyc_Absyn_Var_e_struct*) _temp1239)->f1; goto
_LL1320; _LL1320: _temp1319=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1239)->f2;
goto _LL1246;} else{ goto _LL1247;} _LL1247: if(*(( int*) _temp1239) == Cyc_Absyn_UnknownId_e){
_LL1324: _temp1323=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1239)->f1; goto
_LL1248;} else{ goto _LL1249;} _LL1249: if(*(( int*) _temp1239) == Cyc_Absyn_Primop_e){
_LL1328: _temp1327=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1239)->f1;
goto _LL1326; _LL1326: _temp1325=(( struct Cyc_Absyn_Primop_e_struct*) _temp1239)->f2;
goto _LL1250;} else{ goto _LL1251;} _LL1251: if(*(( int*) _temp1239) == Cyc_Absyn_Increment_e){
_LL1332: _temp1331=(( struct Cyc_Absyn_Increment_e_struct*) _temp1239)->f1; goto
_LL1330; _LL1330: _temp1329=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1239)->f2; goto _LL1252;} else{ goto _LL1253;} _LL1253: if(*(( int*)
_temp1239) == Cyc_Absyn_AssignOp_e){ _LL1338: _temp1337=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1239)->f1; goto _LL1336; _LL1336: _temp1335=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1239)->f2; goto _LL1334; _LL1334: _temp1333=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1239)->f3; goto _LL1254;} else{ goto _LL1255;} _LL1255: if(*(( int*)
_temp1239) == Cyc_Absyn_Conditional_e){ _LL1344: _temp1343=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1239)->f1; goto _LL1342; _LL1342: _temp1341=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1239)->f2; goto _LL1340; _LL1340: _temp1339=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1239)->f3; goto _LL1256;} else{ goto _LL1257;} _LL1257: if(*(( int*)
_temp1239) == Cyc_Absyn_SeqExp_e){ _LL1348: _temp1347=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1239)->f1; goto _LL1346; _LL1346: _temp1345=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1239)->f2; goto _LL1258;} else{ goto _LL1259;} _LL1259: if(*(( int*)
_temp1239) == Cyc_Absyn_UnknownCall_e){ _LL1352: _temp1351=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1239)->f1; goto _LL1350; _LL1350: _temp1349=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1239)->f2; goto _LL1260;} else{ goto _LL1261;} _LL1261: if(*(( int*)
_temp1239) == Cyc_Absyn_FnCall_e){ _LL1356: _temp1355=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1239)->f1; goto _LL1354; _LL1354: _temp1353=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1239)->f2; goto _LL1262;} else{ goto _LL1263;} _LL1263: if(*(( int*)
_temp1239) == Cyc_Absyn_Throw_e){ _LL1358: _temp1357=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1239)->f1; goto _LL1264;} else{ goto _LL1265;} _LL1265: if(*(( int*)
_temp1239) == Cyc_Absyn_NoInstantiate_e){ _LL1360: _temp1359=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1239)->f1; goto _LL1266;} else{ goto _LL1267;} _LL1267: if(*(( int*)
_temp1239) == Cyc_Absyn_Instantiate_e){ _LL1364: _temp1363=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1239)->f1; goto _LL1362; _LL1362: _temp1361=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1239)->f2; goto _LL1268;} else{ goto _LL1269;} _LL1269: if(*(( int*)
_temp1239) == Cyc_Absyn_Cast_e){ _LL1368: _temp1367=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1239)->f1; _temp1369=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1239)->f1); goto _LL1366; _LL1366: _temp1365=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1239)->f2; goto _LL1270;} else{ goto _LL1271;} _LL1271: if(*(( int*)
_temp1239) == Cyc_Absyn_Address_e){ _LL1371: _temp1370=(( struct Cyc_Absyn_Address_e_struct*)
_temp1239)->f1; goto _LL1272;} else{ goto _LL1273;} _LL1273: if(*(( int*)
_temp1239) == Cyc_Absyn_New_e){ _LL1375: _temp1374=(( struct Cyc_Absyn_New_e_struct*)
_temp1239)->f1; goto _LL1373; _LL1373: _temp1372=(( struct Cyc_Absyn_New_e_struct*)
_temp1239)->f2; goto _LL1274;} else{ goto _LL1275;} _LL1275: if(*(( int*)
_temp1239) == Cyc_Absyn_Sizeoftyp_e){ _LL1377: _temp1376=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1239)->f1; goto _LL1276;} else{ goto _LL1277;} _LL1277: if(*(( int*)
_temp1239) == Cyc_Absyn_Sizeofexp_e){ _LL1379: _temp1378=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1239)->f1; goto _LL1278;} else{ goto _LL1279;} _LL1279: if(*(( int*)
_temp1239) == Cyc_Absyn_Deref_e){ _LL1381: _temp1380=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1239)->f1; goto _LL1280;} else{ goto _LL1281;} _LL1281: if(*(( int*)
_temp1239) == Cyc_Absyn_StructMember_e){ _LL1385: _temp1384=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1239)->f1; goto _LL1383; _LL1383: _temp1382=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1239)->f2; goto _LL1282;} else{ goto _LL1283;} _LL1283: if(*(( int*)
_temp1239) == Cyc_Absyn_StructArrow_e){ _LL1389: _temp1388=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1239)->f1; goto _LL1387; _LL1387: _temp1386=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1239)->f2; goto _LL1284;} else{ goto _LL1285;} _LL1285: if(*(( int*)
_temp1239) == Cyc_Absyn_Subscript_e){ _LL1393: _temp1392=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1239)->f1; goto _LL1391; _LL1391: _temp1390=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1239)->f2; goto _LL1286;} else{ goto _LL1287;} _LL1287: if(*(( int*)
_temp1239) == Cyc_Absyn_Tuple_e){ _LL1395: _temp1394=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1239)->f1; goto _LL1288;} else{ goto _LL1289;} _LL1289: if(*(( int*)
_temp1239) == Cyc_Absyn_CompoundLit_e){ _LL1399: _temp1398=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1239)->f1; goto _LL1397; _LL1397: _temp1396=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1239)->f2; goto _LL1290;} else{ goto _LL1291;} _LL1291: if(*(( int*)
_temp1239) == Cyc_Absyn_Array_e){ _LL1401: _temp1400=(( struct Cyc_Absyn_Array_e_struct*)
_temp1239)->f1; goto _LL1292;} else{ goto _LL1293;} _LL1293: if(*(( int*)
_temp1239) == Cyc_Absyn_Comprehension_e){ _LL1407: _temp1406=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1239)->f1; goto _LL1405; _LL1405: _temp1404=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1239)->f2; goto _LL1403; _LL1403: _temp1402=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1239)->f3; goto _LL1294;} else{ goto _LL1295;} _LL1295: if(*(( int*)
_temp1239) == Cyc_Absyn_Struct_e){ _LL1415: _temp1414=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1239)->f1; goto _LL1413; _LL1413: _temp1412=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1239)->f2; goto _LL1411; _LL1411: _temp1410=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1239)->f3; goto _LL1409; _LL1409: _temp1408=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1239)->f4; goto _LL1296;} else{ goto _LL1297;} _LL1297: if(*(( int*)
_temp1239) == Cyc_Absyn_AnonStruct_e){ _LL1419: _temp1418=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1239)->f1; goto _LL1417; _LL1417: _temp1416=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1239)->f2; goto _LL1298;} else{ goto _LL1299;} _LL1299: if(*(( int*)
_temp1239) == Cyc_Absyn_Tunion_e){ _LL1429: _temp1428=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1239)->f1; goto _LL1427; _LL1427: _temp1426=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1239)->f2; goto _LL1425; _LL1425: _temp1424=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1239)->f3; if( _temp1424 == 0){ goto _LL1423;} else{ goto _LL1301;} _LL1423:
_temp1422=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1239)->f4; goto _LL1421;
_LL1421: _temp1420=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1239)->f5; goto
_LL1300;} else{ goto _LL1301;} _LL1301: if(*(( int*) _temp1239) == Cyc_Absyn_Tunion_e){
_LL1439: _temp1438=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1239)->f1; goto
_LL1437; _LL1437: _temp1436=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1239)->f2;
goto _LL1435; _LL1435: _temp1434=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1239)->f3;
goto _LL1433; _LL1433: _temp1432=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1239)->f4;
goto _LL1431; _LL1431: _temp1430=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1239)->f5;
goto _LL1302;} else{ goto _LL1303;} _LL1303: if(*(( int*) _temp1239) == Cyc_Absyn_Enum_e){
_LL1445: _temp1444=(( struct Cyc_Absyn_Enum_e_struct*) _temp1239)->f1; goto
_LL1443; _LL1443: _temp1442=(( struct Cyc_Absyn_Enum_e_struct*) _temp1239)->f2;
goto _LL1441; _LL1441: _temp1440=(( struct Cyc_Absyn_Enum_e_struct*) _temp1239)->f3;
goto _LL1304;} else{ goto _LL1305;} _LL1305: if(*(( int*) _temp1239) == Cyc_Absyn_Malloc_e){
_LL1449: _temp1448=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1239)->f1; goto
_LL1447; _LL1447: _temp1446=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1239)->f2; goto _LL1306;} else{ goto _LL1307;} _LL1307: if(*(( int*)
_temp1239) == Cyc_Absyn_StmtExp_e){ _LL1451: _temp1450=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1239)->f1; goto _LL1308;} else{ goto _LL1309;} _LL1309: if(*(( int*)
_temp1239) == Cyc_Absyn_UnresolvedMem_e){ _LL1455: _temp1454=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1239)->f1; goto _LL1453; _LL1453: _temp1452=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1239)->f2; goto _LL1310;} else{ goto _LL1311;} _LL1311: if(*(( int*)
_temp1239) == Cyc_Absyn_Codegen_e){ _LL1457: _temp1456=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1239)->f1; goto _LL1312;} else{ goto _LL1313;} _LL1313: if(*(( int*)
_temp1239) == Cyc_Absyn_Fill_e){ _LL1459: _temp1458=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1239)->f1; goto _LL1314;} else{ goto _LL1240;} _LL1242:{ void* _temp1460=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1466; struct Cyc_Absyn_Conref*
_temp1468; struct Cyc_Absyn_Tqual _temp1470; struct Cyc_Absyn_Conref* _temp1472;
void* _temp1474; void* _temp1476; _LL1462: if(( unsigned int) _temp1460 > 4u?*((
int*) _temp1460) == Cyc_Absyn_PointerType: 0){ _LL1467: _temp1466=(( struct Cyc_Absyn_PointerType_struct*)
_temp1460)->f1; _LL1477: _temp1476=( void*) _temp1466.elt_typ; goto _LL1475;
_LL1475: _temp1474=( void*) _temp1466.rgn_typ; goto _LL1473; _LL1473: _temp1472=
_temp1466.nullable; goto _LL1471; _LL1471: _temp1470= _temp1466.tq; goto _LL1469;
_LL1469: _temp1468= _temp1466.bounds; goto _LL1463;} else{ goto _LL1464;}
_LL1464: goto _LL1465; _LL1463: if(( void*) _temp1468->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1461;}{ void* _temp1478=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1468); struct Cyc_Absyn_Exp* _temp1484; _LL1480: if( _temp1478 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1481;} else{ goto _LL1482;} _LL1482: if((
unsigned int) _temp1478 > 1u?*(( int*) _temp1478) == Cyc_Absyn_Upper_b: 0){
_LL1485: _temp1484=(( struct Cyc_Absyn_Upper_b_struct*) _temp1478)->f1; goto
_LL1483;} else{ goto _LL1479;} _LL1481: { struct _tuple4* _temp1487=({ struct
_tuple4* _temp1486=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1486->f1= 0; _temp1486->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1486;});
goto _LL1488; _LL1488: { struct Cyc_Absyn_Exp* _temp1492= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp1489=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1489->hd=( void*) _temp1487; _temp1489->tl=({
struct Cyc_List_List* _temp1490=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1490->hd=( void*) _temp1487; _temp1490->tl=({
struct Cyc_List_List* _temp1491=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1491->hd=( void*) _temp1487; _temp1491->tl= 0;
_temp1491;}); _temp1490;}); _temp1489;}), 0); goto _LL1493; _LL1493: if( nv->toplevel){(
void*)( e->r=( void*)(( void*) _temp1492->r));} else{ void* _temp1494= Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1476), Cyc_Toc_mt_tq); goto _LL1495; _LL1495: { struct
_tuple0* _temp1496= Cyc_Toc_temp_var(); goto _LL1497; _LL1497: { struct Cyc_Absyn_Exp*
_temp1498= Cyc_Absyn_var_exp( _temp1496, 0); goto _LL1499; _LL1499: { struct Cyc_Absyn_Stmt*
_temp1500= Cyc_Absyn_declare_stmt( _temp1496, _temp1494,( struct Cyc_Absyn_Exp*)
_temp1492, Cyc_Absyn_exp_stmt( _temp1498, 0), 0); goto _LL1501; _LL1501:( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1500, 0))->r));}}}} goto
_LL1479;}} _LL1483:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0,
0))->r)); goto _LL1479; _LL1479:;} goto _LL1461; _LL1465:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1502= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1502.last_plus_one -
_temp1502.curr, _temp1502.curr);})); goto _LL1461; _LL1461:;} goto _LL1240;
_LL1244: goto _LL1240; _LL1246:{ struct _handler_cons _temp1503; _push_handler(&
_temp1503);{ int _temp1505= 0; if( setjmp( _temp1503.handler)){ _temp1505= 1;}
if( ! _temp1505){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1321))->r));; _pop_handler();} else{ void* _temp1504=( void*) _exn_thrown;
void* _temp1507= _temp1504; _LL1509: if( _temp1507 == Cyc_Dict_Absent){ goto
_LL1510;} else{ goto _LL1511;} _LL1511: goto _LL1512; _LL1510:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1513= Cyc_Absynpp_qvar2string(
_temp1321); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1513.last_plus_one
- _temp1513.curr, _temp1513.curr);})); return; _LL1512:( void) _throw( _temp1507);
_LL1508:;}}} goto _LL1240; _LL1248:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp1514="unknownid"; struct _tagged_string _temp1515; _temp1515.curr=
_temp1514; _temp1515.base= _temp1514; _temp1515.last_plus_one= _temp1514 + 10;
_temp1515;})); goto _LL1240; _LL1250: { struct Cyc_List_List* _temp1516=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp1325); goto _LL1517; _LL1517: {
struct Cyc_List_List* _temp1518=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1325); goto _LL1519; _LL1519:(( void(*)( void(* f)( struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)(
Cyc_Toc_exp_to_c, nv, _temp1325);{ void* _temp1520= _temp1327; _LL1522: if(
_temp1520 ==( void*) Cyc_Absyn_Size){ goto _LL1523;} else{ goto _LL1524;}
_LL1524: if( _temp1520 ==( void*) Cyc_Absyn_Printf){ goto _LL1525;} else{ goto
_LL1526;} _LL1526: if( _temp1520 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1527;}
else{ goto _LL1528;} _LL1528: if( _temp1520 ==( void*) Cyc_Absyn_Fprintf){ goto
_LL1529;} else{ goto _LL1530;} _LL1530: if( _temp1520 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1531;} else{ goto _LL1532;} _LL1532: if( _temp1520 ==( void*) Cyc_Absyn_Plus){
goto _LL1533;} else{ goto _LL1534;} _LL1534: if( _temp1520 ==( void*) Cyc_Absyn_Minus){
goto _LL1535;} else{ goto _LL1536;} _LL1536: if( _temp1520 ==( void*) Cyc_Absyn_Eq){
goto _LL1537;} else{ goto _LL1538;} _LL1538: if( _temp1520 ==( void*) Cyc_Absyn_Neq){
goto _LL1539;} else{ goto _LL1540;} _LL1540: if( _temp1520 ==( void*) Cyc_Absyn_Gt){
goto _LL1541;} else{ goto _LL1542;} _LL1542: if( _temp1520 ==( void*) Cyc_Absyn_Gte){
goto _LL1543;} else{ goto _LL1544;} _LL1544: if( _temp1520 ==( void*) Cyc_Absyn_Lt){
goto _LL1545;} else{ goto _LL1546;} _LL1546: if( _temp1520 ==( void*) Cyc_Absyn_Lte){
goto _LL1547;} else{ goto _LL1548;} _LL1548: goto _LL1549; _LL1523: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1325))->hd;{
void* _temp1550=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1558; struct Cyc_Absyn_Tqual _temp1560; void*
_temp1562; struct Cyc_Absyn_PtrInfo _temp1564; struct Cyc_Absyn_Conref*
_temp1566; struct Cyc_Absyn_Tqual _temp1568; struct Cyc_Absyn_Conref* _temp1570;
void* _temp1572; void* _temp1574; _LL1552: if(( unsigned int) _temp1550 > 4u?*((
int*) _temp1550) == Cyc_Absyn_ArrayType: 0){ _LL1563: _temp1562=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1550)->f1; goto _LL1561; _LL1561:
_temp1560=(( struct Cyc_Absyn_ArrayType_struct*) _temp1550)->f2; goto _LL1559;
_LL1559: _temp1558=(( struct Cyc_Absyn_ArrayType_struct*) _temp1550)->f3; goto
_LL1553;} else{ goto _LL1554;} _LL1554: if(( unsigned int) _temp1550 > 4u?*((
int*) _temp1550) == Cyc_Absyn_PointerType: 0){ _LL1565: _temp1564=(( struct Cyc_Absyn_PointerType_struct*)
_temp1550)->f1; _LL1575: _temp1574=( void*) _temp1564.elt_typ; goto _LL1573;
_LL1573: _temp1572=( void*) _temp1564.rgn_typ; goto _LL1571; _LL1571: _temp1570=
_temp1564.nullable; goto _LL1569; _LL1569: _temp1568= _temp1564.tq; goto _LL1567;
_LL1567: _temp1566= _temp1564.bounds; goto _LL1555;} else{ goto _LL1556;}
_LL1556: goto _LL1557; _LL1553:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1558))->r)); goto _LL1551; _LL1555: { struct _tuple0*
_temp1576= Cyc_Toc_temp_var(); goto _LL1577; _LL1577: { void* _temp1578= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); goto _LL1579;
_LL1579: { struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1576, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1576, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4=
Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp( e2, e3, 0), 0);
struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1576, _temp1578,(
struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1551;}}} _LL1557:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1580= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*) _check_null(
_temp1518))->hd); struct _tagged_string _temp1581= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_temp1580.last_plus_one - _temp1580.curr, _temp1580.curr, _temp1581.last_plus_one
- _temp1581.curr, _temp1581.curr);})); return; _LL1551:;} goto _LL1521;} _LL1525:(
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1327, 0, _temp1325,
_temp1518))->r)); goto _LL1521; _LL1527:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1327, 0, _temp1325, _temp1518))->r)); goto _LL1521; _LL1529: if( _temp1325
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp1582="fprintf without arguments"; struct _tagged_string _temp1583;
_temp1583.curr= _temp1582; _temp1583.base= _temp1582; _temp1583.last_plus_one=
_temp1582 + 26; _temp1583;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1327,({ struct Cyc_Core_Opt* _temp1584=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1584->v=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1325))->hd); _temp1584;}),(( struct Cyc_List_List*)
_check_null( _temp1325))->tl,(( struct Cyc_List_List*) _check_null( _temp1518))->tl))->r));
goto _LL1521; _LL1531: if( _temp1325 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ char* _temp1585="sscanf without arguments"; struct
_tagged_string _temp1586; _temp1586.curr= _temp1585; _temp1586.base= _temp1585;
_temp1586.last_plus_one= _temp1585 + 25; _temp1586;}));}( void*)((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1325))->hd)->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1325))->hd)->r,(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1325))->hd)->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1521;
_LL1533: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1325))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1325))->tl))->hd; struct
_tuple0* _temp1587= Cyc_Toc_temp_var(); goto _LL1588; _LL1588: { void* _temp1589=
Cyc_Toc_typ_to_c( old_typ); goto _LL1590; _LL1590: { struct Cyc_Absyn_Stmt*
_temp1591= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1587, 0), 0); goto
_LL1592; _LL1592: { struct Cyc_Absyn_Exp* _temp1593= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1587, 0), Cyc_Toc_curr_sp, 0); goto _LL1594; _LL1594: {
struct Cyc_Absyn_Exp* _temp1596= Cyc_Absyn_assignop_exp( _temp1593,({ struct Cyc_Core_Opt*
_temp1595=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1595->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1595;}), e2, 0); goto
_LL1597; _LL1597: _temp1591= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1596,
0), _temp1591, 0); _temp1591= Cyc_Absyn_declare_stmt( _temp1587, _temp1589,(
struct Cyc_Absyn_Exp*) e1, _temp1591, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1591, 0))->r));}}}}} goto _LL1521; _LL1535: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null( _temp1516))->hd)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1325))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1325))->tl))->hd; void*
old_typ_e2=( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1516))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* _temp1598= Cyc_Toc_temp_var(); goto _LL1599;
_LL1599: { void* _temp1600= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_List_List*)
_check_null( _temp1516))->hd); goto _LL1601; _LL1601: { struct Cyc_Absyn_Stmt*
_temp1602= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1598, 0), 0); goto
_LL1603; _LL1603: { struct Cyc_Absyn_Exp* _temp1604= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1598, 0), Cyc_Toc_curr_sp, 0); goto _LL1605; _LL1605: {
struct Cyc_Absyn_Exp* _temp1607= Cyc_Absyn_assignop_exp( _temp1604,({ struct Cyc_Core_Opt*
_temp1606=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1606->v=( void*)(( void*) Cyc_Absyn_Minus); _temp1606;}), e2, 0); goto
_LL1608; _LL1608: _temp1602= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1607,
0), _temp1602, 0); _temp1602= Cyc_Absyn_declare_stmt( _temp1598, _temp1600,(
struct Cyc_Absyn_Exp*) e1, _temp1602, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1602, 0))->r));}}}}}} goto _LL1521; _LL1537: goto _LL1539; _LL1539: goto
_LL1541; _LL1541: goto _LL1543; _LL1543: goto _LL1545; _LL1545: goto _LL1547;
_LL1547: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1325))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1325))->tl))->hd; void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp1516))->hd; void* t2=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1516))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1521;} _LL1549: goto
_LL1521; _LL1521:;} goto _LL1240;}} _LL1252: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1331->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1331); if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if( _temp1329 ==(
void*) Cyc_Absyn_PreInc? 1: _temp1329 ==( void*) Cyc_Absyn_PreDec){ struct
_tuple0* _temp1609= Cyc_Toc_temp_var(); goto _LL1610; _LL1610: { void* _temp1611=
Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1612; _LL1612: { struct Cyc_Absyn_Stmt*
_temp1613= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp1609,
0), 0), 0); goto _LL1614; _LL1614: { struct Cyc_Absyn_Exp* _temp1615= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1609, 0), Cyc_Toc_curr_sp, 0),
_temp1329, 0); goto _LL1616; _LL1616: _temp1613= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1615, 0), _temp1613, 0); _temp1613= Cyc_Absyn_declare_stmt( _temp1609,
_temp1611,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1331, 0),
_temp1613, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1613, 0))->r));}}}}
else{ struct _tuple0* _temp1617= Cyc_Toc_temp_var(); goto _LL1618; _LL1618: {
struct _tuple0* _temp1619= Cyc_Toc_temp_var(); goto _LL1620; _LL1620: { void*
_temp1621= Cyc_Toc_typ_to_c( e2_cyc_typ); goto _LL1622; _LL1622: { void*
_temp1623= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1624; _LL1624: { struct Cyc_Absyn_Stmt*
_temp1625= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1619, 0), 0); goto
_LL1626; _LL1626: { struct Cyc_Absyn_Exp* _temp1627= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1617, 0), Cyc_Toc_curr_sp, 0),
_temp1329, 0); goto _LL1628; _LL1628: _temp1625= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1627, 0), _temp1625, 0); _temp1625= Cyc_Absyn_declare_stmt( _temp1619,
_temp1621,( struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp1617, 0), 0), _temp1625, 0); _temp1625= Cyc_Absyn_declare_stmt( _temp1617,
_temp1623,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1331, 0),
_temp1625, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1625, 0))->r));}}}}}}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1331)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1331, 0, Cyc_Toc_incr_lvalue,
_temp1329);( void*)( e->r=( void*)(( void*) _temp1331->r));}} goto _LL1240;}
_LL1254: { int e1_poly= Cyc_Toc_is_poly_project( _temp1337); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1337->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1333->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1337); Cyc_Toc_exp_to_c( nv, _temp1333); if(
_temp1335 != 0?( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1335))->v ==(
void*) Cyc_Absyn_Plus? 1:( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1335))->v
==( void*) Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){(
void*)( _temp1337->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1337->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1333->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1333->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1333->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1333->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1337)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1337, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1629=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1629[ 0]=({ struct _tuple11 _temp1630; _temp1630.f1=
_temp1335; _temp1630.f2= _temp1333; _temp1630;}); _temp1629;}));( void*)( e->r=(
void*)(( void*) _temp1337->r));} goto _LL1240;} _LL1256: Cyc_Toc_exp_to_c( nv,
_temp1343); Cyc_Toc_exp_to_c( nv, _temp1341); Cyc_Toc_exp_to_c( nv, _temp1339);
goto _LL1240; _LL1258: Cyc_Toc_exp_to_c( nv, _temp1347); Cyc_Toc_exp_to_c( nv,
_temp1345); goto _LL1240; _LL1260: _temp1355= _temp1351; _temp1353= _temp1349;
goto _LL1262; _LL1262: Cyc_Toc_exp_to_c( nv, _temp1355);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1353); goto _LL1240; _LL1264:
Cyc_Toc_exp_to_c( nv, _temp1357);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1357), 0))->r)); goto
_LL1240; _LL1266: Cyc_Toc_exp_to_c( nv, _temp1359); goto _LL1240; _LL1268: Cyc_Toc_exp_to_c(
nv, _temp1363);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); for( 0; _temp1361 != 0; _temp1361=(( struct Cyc_List_List*)
_check_null( _temp1361))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct
Cyc_List_List*) _check_null( _temp1361))->hd); if( k !=( void*) Cyc_Absyn_EffKind?
k !=( void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1363, 0))->r)); break;}} goto _LL1240;} _LL1270: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1365->topt))->v; void* new_typ=*
_temp1369;* _temp1369= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1365);{ int noconv= 0;{ void* _temp1631= Cyc_Toc_conversion( old_t2,
new_typ); void* _temp1641; struct Cyc_Absyn_Exp* _temp1643; void* _temp1645; int
_temp1647; struct Cyc_Absyn_Exp* _temp1649; void* _temp1651; _LL1633: if(
_temp1631 ==( void*) Cyc_Toc_NoConv){ goto _LL1634;} else{ goto _LL1635;}
_LL1635: if(( unsigned int) _temp1631 > 1u?*(( int*) _temp1631) == Cyc_Toc_NullCheck:
0){ _LL1642: _temp1641=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp1631)->f1;
goto _LL1636;} else{ goto _LL1637;} _LL1637: if(( unsigned int) _temp1631 > 1u?*((
int*) _temp1631) == Cyc_Toc_TagPtr: 0){ _LL1646: _temp1645=( void*)(( struct Cyc_Toc_TagPtr_struct*)
_temp1631)->f1; goto _LL1644; _LL1644: _temp1643=(( struct Cyc_Toc_TagPtr_struct*)
_temp1631)->f2; goto _LL1638;} else{ goto _LL1639;} _LL1639: if(( unsigned int)
_temp1631 > 1u?*(( int*) _temp1631) == Cyc_Toc_UntagPtr: 0){ _LL1652: _temp1651=(
void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp1631)->f1; goto _LL1650; _LL1650:
_temp1649=(( struct Cyc_Toc_UntagPtr_struct*) _temp1631)->f2; goto _LL1648;
_LL1648: _temp1647=(( struct Cyc_Toc_UntagPtr_struct*) _temp1631)->f3; goto
_LL1640;} else{ goto _LL1632;} _LL1634: goto _LL1632; _LL1636:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_null_check_conv( nv->toplevel, _temp1641, _temp1365))->r));
goto _LL1632; _LL1638:( void*)( e->r=( void*)(( void*)( Cyc_Toc_tag_ptr_conv( nv->toplevel,
_temp1645, _temp1643, _temp1365))->r)); goto _LL1632; _LL1640:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_untag_ptr_conv( nv->toplevel, _temp1651, _temp1649,
_temp1647, _temp1365))->r)); goto _LL1632; _LL1632:;} goto _LL1240;}} _LL1272:{
void* _temp1653=( void*) _temp1370->r; struct Cyc_Absyn_Structdecl* _temp1661;
struct Cyc_List_List* _temp1663; struct Cyc_Core_Opt* _temp1665; struct _tuple0*
_temp1667; struct Cyc_List_List* _temp1669; _LL1655: if(*(( int*) _temp1653) ==
Cyc_Absyn_Struct_e){ _LL1668: _temp1667=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1653)->f1; goto _LL1666; _LL1666: _temp1665=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1653)->f2; goto _LL1664; _LL1664: _temp1663=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1653)->f3; goto _LL1662; _LL1662: _temp1661=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1653)->f4; goto _LL1656;} else{ goto _LL1657;} _LL1657: if(*(( int*)
_temp1653) == Cyc_Absyn_Tuple_e){ _LL1670: _temp1669=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1653)->f1; goto _LL1658;} else{ goto _LL1659;} _LL1659: goto _LL1660;
_LL1656: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1671= Cyc_Position_string_of_segment( _temp1370->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1671.last_plus_one -
_temp1671.curr, _temp1671.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1370->topt))->v, 1, 0,
_temp1663, _temp1667))->r)); goto _LL1654; _LL1658: if( nv->toplevel){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1672= Cyc_Position_string_of_segment(
_temp1370->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1672.last_plus_one - _temp1672.curr, _temp1672.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1669))->r)); goto _LL1654;
_LL1660: Cyc_Toc_exp_to_c( nv, _temp1370); if( ! Cyc_Absyn_is_lvalue( _temp1370)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1370,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1370, 0))->r));} goto _LL1654; _LL1654:;} goto _LL1240; _LL1274: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string
_temp1673= Cyc_Position_string_of_segment( _temp1372->loc); xprintf("%.*s: new at top-level",
_temp1673.last_plus_one - _temp1673.curr, _temp1673.curr);}));}{ void* _temp1674=(
void*) _temp1372->r; struct Cyc_List_List* _temp1688; void* _temp1690; struct
_tagged_string _temp1692; struct Cyc_Absyn_Exp* _temp1694; struct Cyc_Absyn_Exp*
_temp1696; struct Cyc_Absyn_Vardecl* _temp1698; struct Cyc_Absyn_Structdecl*
_temp1700; struct Cyc_List_List* _temp1702; struct Cyc_Core_Opt* _temp1704;
struct _tuple0* _temp1706; struct Cyc_List_List* _temp1708; _LL1676: if(*(( int*)
_temp1674) == Cyc_Absyn_Array_e){ _LL1689: _temp1688=(( struct Cyc_Absyn_Array_e_struct*)
_temp1674)->f1; goto _LL1677;} else{ goto _LL1678;} _LL1678: if(*(( int*)
_temp1674) == Cyc_Absyn_Const_e){ _LL1691: _temp1690=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1674)->f1; if(( unsigned int) _temp1690 > 1u?*(( int*) _temp1690) == Cyc_Absyn_String_c:
0){ _LL1693: _temp1692=(( struct Cyc_Absyn_String_c_struct*) _temp1690)->f1;
goto _LL1679;} else{ goto _LL1680;}} else{ goto _LL1680;} _LL1680: if(*(( int*)
_temp1674) == Cyc_Absyn_Comprehension_e){ _LL1699: _temp1698=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1674)->f1; goto _LL1697; _LL1697: _temp1696=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1674)->f2; goto _LL1695; _LL1695: _temp1694=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1674)->f3; goto _LL1681;} else{ goto _LL1682;} _LL1682: if(*(( int*)
_temp1674) == Cyc_Absyn_Struct_e){ _LL1707: _temp1706=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1674)->f1; goto _LL1705; _LL1705: _temp1704=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1674)->f2; goto _LL1703; _LL1703: _temp1702=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1674)->f3; goto _LL1701; _LL1701: _temp1700=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1674)->f4; goto _LL1683;} else{ goto _LL1684;} _LL1684: if(*(( int*)
_temp1674) == Cyc_Absyn_Tuple_e){ _LL1709: _temp1708=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1674)->f1; goto _LL1685;} else{ goto _LL1686;} _LL1686: goto _LL1687;
_LL1677: { struct _tuple0* _temp1710= Cyc_Toc_temp_var(); goto _LL1711; _LL1711: {
struct Cyc_Absyn_Exp* _temp1712= Cyc_Absyn_var_exp( _temp1710, 0); goto _LL1713;
_LL1713: { struct Cyc_Absyn_Stmt* _temp1714= Cyc_Toc_init_array( nv, _temp1712,
_temp1688, Cyc_Absyn_exp_stmt( _temp1712, 0)); goto _LL1715; _LL1715: { void*
old_elt_typ;{ void* _temp1716= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1722; struct Cyc_Absyn_Conref* _temp1724; struct Cyc_Absyn_Tqual _temp1726;
struct Cyc_Absyn_Conref* _temp1728; void* _temp1730; void* _temp1732; _LL1718:
if(( unsigned int) _temp1716 > 4u?*(( int*) _temp1716) == Cyc_Absyn_PointerType:
0){ _LL1723: _temp1722=(( struct Cyc_Absyn_PointerType_struct*) _temp1716)->f1;
_LL1733: _temp1732=( void*) _temp1722.elt_typ; goto _LL1731; _LL1731: _temp1730=(
void*) _temp1722.rgn_typ; goto _LL1729; _LL1729: _temp1728= _temp1722.nullable;
goto _LL1727; _LL1727: _temp1726= _temp1722.tq; goto _LL1725; _LL1725: _temp1724=
_temp1722.bounds; goto _LL1719;} else{ goto _LL1720;} _LL1720: goto _LL1721;
_LL1719: old_elt_typ= _temp1732; goto _LL1717; _LL1721: old_elt_typ=(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp1734="exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1735; _temp1735.curr= _temp1734; _temp1735.base=
_temp1734; _temp1735.last_plus_one= _temp1734 + 52; _temp1735;})); goto _LL1717;
_LL1717:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* _temp1736= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1737; _LL1737: { struct Cyc_Absyn_Exp*
_temp1738= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1688), 0), 0); goto
_LL1739; _LL1739: { struct Cyc_Absyn_Exp* e1; if( _temp1374 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1738);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1374); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1738);}{ struct Cyc_Absyn_Exp* _temp1740= Cyc_Absyn_cast_exp( _temp1736, e1,
0); goto _LL1741; _LL1741:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1710, _temp1736,( struct Cyc_Absyn_Exp*) _temp1740,
_temp1714, 0), 0))->r)); goto _LL1675;}}}}}}}} _LL1679: if( _temp1374 == 0){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1742=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1742->f1= Cyc_Toc_abs_ns; _temp1742->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1742;}), 0),({ struct Cyc_List_List* _temp1743=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1743->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1744=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1744[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1745; _temp1745.tag= Cyc_Absyn_Const_e; _temp1745.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1746=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1746[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1747; _temp1747.tag= Cyc_Absyn_String_c;
_temp1747.f1= _temp1692; _temp1747;}); _temp1746;})); _temp1745;}); _temp1744;}),
0); _temp1743->tl= 0; _temp1743;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( _temp1374); Cyc_Toc_exp_to_c( nv, r);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({ struct
_tuple0* _temp1748=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1748->f1= Cyc_Toc_abs_ns; _temp1748->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1748;}), 0),({ struct Cyc_List_List* _temp1749=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1749->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1750=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1750[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1751; _temp1751.tag= Cyc_Absyn_Const_e; _temp1751.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1752=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1752[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1753; _temp1753.tag= Cyc_Absyn_String_c;
_temp1753.f1= _temp1692; _temp1753;}); _temp1752;})); _temp1751;}); _temp1750;}),
0); _temp1749->tl= 0; _temp1749;}), 0))->r));} goto _LL1675; _LL1681: { int
is_tagged_ptr= 0;{ void* _temp1754= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1760; struct Cyc_Absyn_Conref* _temp1762; struct Cyc_Absyn_Tqual _temp1764;
struct Cyc_Absyn_Conref* _temp1766; void* _temp1768; void* _temp1770; _LL1756:
if(( unsigned int) _temp1754 > 4u?*(( int*) _temp1754) == Cyc_Absyn_PointerType:
0){ _LL1761: _temp1760=(( struct Cyc_Absyn_PointerType_struct*) _temp1754)->f1;
_LL1771: _temp1770=( void*) _temp1760.elt_typ; goto _LL1769; _LL1769: _temp1768=(
void*) _temp1760.rgn_typ; goto _LL1767; _LL1767: _temp1766= _temp1760.nullable;
goto _LL1765; _LL1765: _temp1764= _temp1760.tq; goto _LL1763; _LL1763: _temp1762=
_temp1760.bounds; goto _LL1757;} else{ goto _LL1758;} _LL1758: goto _LL1759;
_LL1757:{ void* _temp1772=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1762))->v; void* _temp1778; _LL1774: if((
unsigned int) _temp1772 > 1u?*(( int*) _temp1772) == Cyc_Absyn_Eq_constr: 0){
_LL1779: _temp1778=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1772)->f1;
if( _temp1778 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1775;} else{ goto _LL1776;}}
else{ goto _LL1776;} _LL1776: goto _LL1777; _LL1775: is_tagged_ptr= 1; goto
_LL1773; _LL1777: goto _LL1773; _LL1773:;} goto _LL1755; _LL1759:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp1780="exp_to_c: comprehension not an array type";
struct _tagged_string _temp1781; _temp1781.curr= _temp1780; _temp1781.base=
_temp1780; _temp1781.last_plus_one= _temp1780 + 42; _temp1781;})); goto _LL1755;
_LL1755:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1694->topt))->v;
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1696);{ struct Cyc_Absyn_Stmt*
s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( a, 0), _temp1698, Cyc_Absyn_var_exp(
max, 0), _temp1694, Cyc_Absyn_skip_stmt( 0), 1); struct Cyc_List_List* decls=({
struct Cyc_List_List* _temp1808=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1808->hd=( void*)({ struct _tuple10* _temp1809=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1809->f1= max;
_temp1809->f2= Cyc_Absyn_uint_t; _temp1809->f3=( struct Cyc_Absyn_Exp*)
_temp1696; _temp1809;}); _temp1808->tl= 0; _temp1808;}); struct Cyc_Absyn_Exp*
ai; if( _temp1374 == 0){ ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{
struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1374); Cyc_Toc_exp_to_c(
nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp(
ptr_typ, ai, 0); decls=({ struct Cyc_List_List* _temp1782=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1782->hd=( void*)({ struct
_tuple10* _temp1783=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp1783->f1= a; _temp1783->f2= ptr_typ; _temp1783->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1783;}); _temp1782->tl= decls; _temp1782;}); if( is_tagged_ptr){
struct _tuple0* _temp1784= Cyc_Toc_temp_var(); goto _LL1785; _LL1785: { void*
_temp1786= Cyc_Toc_typ_to_c( old_typ); goto _LL1787; _LL1787: { struct Cyc_Absyn_Exp*
_temp1794= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List* _temp1788=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1788->hd=(
void*)({ struct _tuple4* _temp1793=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1793->f1= 0; _temp1793->f2= Cyc_Absyn_var_exp( a, 0); _temp1793;});
_temp1788->tl=({ struct Cyc_List_List* _temp1789=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1789->hd=( void*)({ struct
_tuple4* _temp1792=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1792->f1= 0; _temp1792->f2= Cyc_Absyn_var_exp( a, 0); _temp1792;});
_temp1789->tl=({ struct Cyc_List_List* _temp1790=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1790->hd=( void*)({ struct
_tuple4* _temp1791=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1791->f1= 0; _temp1791->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1791;}); _temp1790->tl= 0; _temp1790;});
_temp1789;}); _temp1788;}), 0); goto _LL1795; _LL1795: decls=({ struct Cyc_List_List*
_temp1796=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1796->hd=( void*)({ struct _tuple10* _temp1797=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1797->f1= _temp1784; _temp1797->f2=
_temp1786; _temp1797->f3=( struct Cyc_Absyn_Exp*) _temp1794; _temp1797;});
_temp1796->tl= decls; _temp1796;}); s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( _temp1784, 0), 0), 0);}}} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List* _temp1798= decls; goto
_LL1799; _LL1799: for( 0; _temp1798 != 0; _temp1798=(( struct Cyc_List_List*)
_check_null( _temp1798))->tl){ struct Cyc_Absyn_Exp* _temp1802; void* _temp1804;
struct _tuple0* _temp1806; struct _tuple10 _temp1800=*(( struct _tuple10*)((
struct Cyc_List_List*) _check_null( _temp1798))->hd); _LL1807: _temp1806=
_temp1800.f1; goto _LL1805; _LL1805: _temp1804= _temp1800.f2; goto _LL1803;
_LL1803: _temp1802= _temp1800.f3; goto _LL1801; _LL1801: s= Cyc_Absyn_declare_stmt(
_temp1806, _temp1804, _temp1802, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r)); goto _LL1675;}}}} _LL1683:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1372->topt))->v, 1,
_temp1374, _temp1702, _temp1706))->r)); goto _LL1675; _LL1685:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1374, _temp1708))->r)); goto
_LL1675; _LL1687: { void* _temp1810=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1372->topt))->v; goto _LL1811; _LL1811: { struct _tuple0* _temp1812= Cyc_Toc_temp_var();
goto _LL1813; _LL1813: { struct Cyc_Absyn_Stmt* _temp1814= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( _temp1812, 0), 0); goto _LL1815; _LL1815: { struct Cyc_Absyn_Exp*
_temp1816= Cyc_Absyn_signed_int_exp( 0, 0); goto _LL1817; _LL1817: Cyc_Toc_exp_to_c(
nv, _temp1372); _temp1814= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1812, 0), _temp1816, 0), _temp1372, 0), _temp1814, 0);{
void* elt_typ= Cyc_Toc_typ_to_c( _temp1810); void* _temp1818= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1819; _LL1819: { struct Cyc_Absyn_Exp*
_temp1820= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); goto _LL1821; _LL1821: { struct
Cyc_Absyn_Exp* e1; if( _temp1374 == 0){ e1= Cyc_Toc_malloc_exp( _temp1810,
_temp1820);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null(
_temp1374); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r, _temp1820);}{
struct Cyc_Absyn_Exp* _temp1822= Cyc_Absyn_cast_exp( _temp1818, e1, 0); goto
_LL1823; _LL1823:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1812, _temp1818,( struct Cyc_Absyn_Exp*) _temp1822, _temp1814, 0), 0))->r));
goto _LL1675;}}}}}}}} _LL1675:;} goto _LL1240; _LL1276:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct* _temp1824=( struct Cyc_Absyn_Sizeoftyp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct)); _temp1824[ 0]=({
struct Cyc_Absyn_Sizeoftyp_e_struct _temp1825; _temp1825.tag= Cyc_Absyn_Sizeoftyp_e;
_temp1825.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1376); _temp1825;});
_temp1824;}))); goto _LL1240; _LL1278: Cyc_Toc_exp_to_c( nv, _temp1378); goto
_LL1240; _LL1280: { void* e1_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1380->topt))->v); Cyc_Toc_exp_to_c( nv, _temp1380);{ void*
_temp1826= e1_typ; struct Cyc_Absyn_PtrInfo _temp1832; struct Cyc_Absyn_Conref*
_temp1834; struct Cyc_Absyn_Tqual _temp1836; struct Cyc_Absyn_Conref* _temp1838;
void* _temp1840; void* _temp1842; _LL1828: if(( unsigned int) _temp1826 > 4u?*((
int*) _temp1826) == Cyc_Absyn_PointerType: 0){ _LL1833: _temp1832=(( struct Cyc_Absyn_PointerType_struct*)
_temp1826)->f1; _LL1843: _temp1842=( void*) _temp1832.elt_typ; goto _LL1841;
_LL1841: _temp1840=( void*) _temp1832.rgn_typ; goto _LL1839; _LL1839: _temp1838=
_temp1832.nullable; goto _LL1837; _LL1837: _temp1836= _temp1832.tq; goto _LL1835;
_LL1835: _temp1834= _temp1832.bounds; goto _LL1829;} else{ goto _LL1830;}
_LL1830: goto _LL1831; _LL1829:{ void* _temp1844=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1834); struct Cyc_Absyn_Exp* _temp1850; _LL1846:
if( _temp1844 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1847;} else{ goto _LL1848;}
_LL1848: if(( unsigned int) _temp1844 > 1u?*(( int*) _temp1844) == Cyc_Absyn_Upper_b:
0){ _LL1851: _temp1850=(( struct Cyc_Absyn_Upper_b_struct*) _temp1844)->f1; goto
_LL1849;} else{ goto _LL1845;} _LL1847: { struct _tuple0* x= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); void* _temp1852= Cyc_Toc_typ_to_c_array(
_temp1842); goto _LL1853; _LL1853: { void* tx= Cyc_Toc_add_tagged_ptr_type(
_temp1852, _temp1836); void* ta2= Cyc_Absyn_cstar_typ( _temp1852, _temp1836);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
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
struct Cyc_Absyn_Exp*) _temp1380, s1, 0), 0))->r)); goto _LL1845;}} _LL1849: if(
Cyc_Toc_is_nullable( e1_typ)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1380->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1854=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1854->hd=(
void*) _temp1380; _temp1854->tl= 0; _temp1854;}), 0), 0), 0))->r));} else{
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, e1_typ,(
struct Cyc_Absyn_Exp*) _temp1380, s3, 0), 0))->r));}} goto _LL1845; _LL1845:;}
goto _LL1827; _LL1831:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp1855="exp_to_c: Deref: non-pointer"; struct _tagged_string _temp1856;
_temp1856.curr= _temp1855; _temp1856.base= _temp1855; _temp1856.last_plus_one=
_temp1855 + 29; _temp1856;})); goto _LL1827; _LL1827:;} goto _LL1240;} _LL1282:
Cyc_Toc_exp_to_c( nv, _temp1384); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL1240; _LL1284: Cyc_Toc_exp_to_c( nv, _temp1388);{ int is_poly=
Cyc_Toc_is_poly_project( e); if( Cyc_Toc_is_nullable(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1388->topt))->v)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1388->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1857=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1857->hd=(
void*) _temp1388; _temp1857->tl= 0; _temp1857;}), 0), 0), _temp1386, 0))->r));}
else{ struct _tuple0* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1388->topt))->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp1386, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1388, s3, 0), 0))->r));}} if( is_poly){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto
_LL1240;} _LL1286: { void* _temp1858= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1392->topt))->v); goto _LL1859; _LL1859: Cyc_Toc_exp_to_c( nv,
_temp1392); Cyc_Toc_exp_to_c( nv, _temp1390);{ void* _temp1860= _temp1858;
struct Cyc_List_List* _temp1868; struct Cyc_Absyn_PtrInfo _temp1870; struct Cyc_Absyn_Conref*
_temp1872; struct Cyc_Absyn_Tqual _temp1874; struct Cyc_Absyn_Conref* _temp1876;
void* _temp1878; void* _temp1880; _LL1862: if(( unsigned int) _temp1860 > 4u?*((
int*) _temp1860) == Cyc_Absyn_TupleType: 0){ _LL1869: _temp1868=(( struct Cyc_Absyn_TupleType_struct*)
_temp1860)->f1; goto _LL1863;} else{ goto _LL1864;} _LL1864: if(( unsigned int)
_temp1860 > 4u?*(( int*) _temp1860) == Cyc_Absyn_PointerType: 0){ _LL1871:
_temp1870=(( struct Cyc_Absyn_PointerType_struct*) _temp1860)->f1; _LL1881:
_temp1880=( void*) _temp1870.elt_typ; goto _LL1879; _LL1879: _temp1878=( void*)
_temp1870.rgn_typ; goto _LL1877; _LL1877: _temp1876= _temp1870.nullable; goto
_LL1875; _LL1875: _temp1874= _temp1870.tq; goto _LL1873; _LL1873: _temp1872=
_temp1870.bounds; goto _LL1865;} else{ goto _LL1866;} _LL1866: goto _LL1867;
_LL1863: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1390) + 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp1392,({ struct
_tagged_string* _temp1882=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp1882[ 0]= xprintf("f%d",( int) i); _temp1882;}), 0))->r));
goto _LL1861;} _LL1865:{ void* _temp1883=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1872); struct Cyc_Absyn_Exp* _temp1889; _LL1885: if((
unsigned int) _temp1883 > 1u?*(( int*) _temp1883) == Cyc_Absyn_Upper_b: 0){
_LL1890: _temp1889=(( struct Cyc_Absyn_Upper_b_struct*) _temp1883)->f1; goto
_LL1886;} else{ goto _LL1887;} _LL1887: if( _temp1883 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1888;} else{ goto _LL1884;} _LL1886: _temp1889= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1889), 0);{ int possibly_null;{ void* _temp1891=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1876))->v; int
_temp1897; _LL1893: if(( unsigned int) _temp1891 > 1u?*(( int*) _temp1891) ==
Cyc_Absyn_Eq_constr: 0){ _LL1898: _temp1897=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1891)->f1; goto _LL1894;} else{ goto _LL1895;} _LL1895: goto _LL1896;
_LL1894: possibly_null= _temp1897; goto _LL1892; _LL1896: possibly_null= 0; goto
_LL1892; _LL1892:;}{ void* ta1= Cyc_Toc_typ_to_c( _temp1880); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1874); if( Cyc_Toc_use_runtimecheck_funs){ if( possibly_null){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv, 0),({ struct Cyc_List_List*
_temp1899=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1899->hd=( void*) _temp1392; _temp1899->tl=({ struct Cyc_List_List*
_temp1900=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1900->hd=( void*) _temp1889; _temp1900->tl=({ struct Cyc_List_List*
_temp1901=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1901->hd=( void*) Cyc_Absyn_sizeoftyp_exp( ta1, 0); _temp1901->tl=({ struct
Cyc_List_List* _temp1902=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1902->hd=( void*) _temp1390; _temp1902->tl= 0; _temp1902;}); _temp1901;});
_temp1900;}); _temp1899;}), 0), 0), 0))->r));} else{( void*)( _temp1390->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List* _temp1903=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1903->hd=( void*) _temp1889; _temp1903->tl=({
struct Cyc_List_List* _temp1904=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1904->hd=( void*) Cyc_Absyn_copy_exp( _temp1390);
_temp1904->tl= 0; _temp1904;}); _temp1903;}), 0))->r));}} else{ struct _tuple0*
a= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte, Cyc_Absyn_var_exp( i, 0),
_temp1889, 0); if( possibly_null){ e3= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_new_exp(( void*) e3->r, 0),
0);}{ struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt(
i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) _temp1390, s3, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*)
_temp1392, s1, 0), 0))->r));}} goto _LL1884;}} _LL1888: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp1880); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1874); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp1911=({ struct Cyc_List_List* _temp1905=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1905->hd=( void*)({ struct
_tuple1* _temp1910=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1910->f1= 0; _temp1910->f2= Cyc_Toc_mt_tq; _temp1910->f3= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1392->topt))->v); _temp1910;});
_temp1905->tl=({ struct Cyc_List_List* _temp1906=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1906->hd=( void*)({ struct
_tuple1* _temp1909=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1909->f1= 0; _temp1909->f2= Cyc_Toc_mt_tq; _temp1909->f3= Cyc_Absyn_uint_t;
_temp1909;}); _temp1906->tl=({ struct Cyc_List_List* _temp1907=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1907->hd=( void*)({ struct
_tuple1* _temp1908=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1908->f1= 0; _temp1908->f2= Cyc_Toc_mt_tq; _temp1908->f3= Cyc_Absyn_uint_t;
_temp1908;}); _temp1907->tl= 0; _temp1907;}); _temp1906;}); _temp1905;}); goto
_LL1912; _LL1912: { void* _temp1916= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1913=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1913[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1914; _temp1914.tag= Cyc_Absyn_FnType;
_temp1914.f1=({ struct Cyc_Absyn_FnInfo _temp1915; _temp1915.tvars= 0; _temp1915.effect=
0; _temp1915.ret_typ=( void*) ta2; _temp1915.args= _temp1911; _temp1915.varargs=
0; _temp1915.attributes= 0; _temp1915;}); _temp1914;}); _temp1913;}), Cyc_Toc_mt_tq);
goto _LL1917; _LL1917: { struct Cyc_Absyn_Exp* fnexp= Cyc_Absyn_cast_exp(
_temp1916, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv, 0), 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_fncall_exp( fnexp,({
struct Cyc_List_List* _temp1918=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1918->hd=( void*) _temp1392; _temp1918->tl=({
struct Cyc_List_List* _temp1919=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1919->hd=( void*) Cyc_Absyn_sizeoftyp_exp( ta1, 0);
_temp1919->tl=({ struct Cyc_List_List* _temp1920=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1920->hd=( void*) _temp1390;
_temp1920->tl= 0; _temp1920;}); _temp1919;}); _temp1918;}), 0), 0))->r));}}}
else{ struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type(
ta1, _temp1874); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1390, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1392, s1, 0), 0))->r));} goto _LL1884;} _LL1884:;}
goto _LL1861; _LL1867:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp1921="exp_to_c: Subscript on non-tuple/array/tuple ptr"; struct
_tagged_string _temp1922; _temp1922.curr= _temp1921; _temp1922.base= _temp1921;
_temp1922.last_plus_one= _temp1921 + 49; _temp1922;})); goto _LL1861; _LL1861:;}
goto _LL1240;} _LL1288: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_tuple( nv, 0, 0, _temp1394))->r));} else{ struct Cyc_List_List*
_temp1923=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1394); goto
_LL1924; _LL1924: { struct _tagged_string* _temp1925= Cyc_Toc_add_tuple_type(
_temp1923); goto _LL1926; _LL1926: { struct Cyc_List_List* dles= 0;{ int i= 1;
for( 0; _temp1394 != 0;( _temp1394=(( struct Cyc_List_List*) _check_null(
_temp1394))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1394))->hd); dles=({ struct Cyc_List_List*
_temp1927=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1927->hd=( void*)({ struct _tuple4* _temp1928=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1928->f1= 0; _temp1928->f2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1394))->hd; _temp1928;}); _temp1927->tl=
dles; _temp1927;});}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, dles, 0))->r));}}} goto _LL1240; _LL1290:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ char* _temp1929="compoundlit"; struct _tagged_string _temp1930;
_temp1930.curr= _temp1929; _temp1930.base= _temp1929; _temp1930.last_plus_one=
_temp1929 + 12; _temp1930;})); goto _LL1240; _LL1292:( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1400, 0))->r));{ struct Cyc_List_List*
_temp1931= _temp1400; goto _LL1932; _LL1932: for( 0; _temp1931 != 0; _temp1931=((
struct Cyc_List_List*) _check_null( _temp1931))->tl){ struct _tuple4 _temp1935;
struct Cyc_Absyn_Exp* _temp1936; struct Cyc_List_List* _temp1938; struct _tuple4*
_temp1933=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1931))->hd;
_temp1935=* _temp1933; _LL1939: _temp1938= _temp1935.f1; goto _LL1937; _LL1937:
_temp1936= _temp1935.f2; goto _LL1934; _LL1934: Cyc_Toc_exp_to_c( nv, _temp1936);}}
goto _LL1240; _LL1294: { unsigned int _temp1940= Cyc_Evexp_eval_const_uint_exp(
_temp1404); goto _LL1941; _LL1941: Cyc_Toc_exp_to_c( nv, _temp1402);{ struct Cyc_List_List*
es= 0; if( ! Cyc_Toc_is_zero( _temp1402)){ unsigned int i= 0; for( 0; i <
_temp1940; i ++){ es=({ struct Cyc_List_List* _temp1942=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1942->hd=( void*)({ struct
_tuple4* _temp1943=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1943->f1= 0; _temp1943->f2= _temp1402; _temp1943;}); _temp1942->tl= es;
_temp1942;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
es, 0))->r)); goto _LL1240;}} _LL1296: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1410, _temp1414))->r));}
else{ if( _temp1408 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp1944="Struct_e: missing structdecl pointer"; struct _tagged_string
_temp1945; _temp1945.curr= _temp1944; _temp1945.base= _temp1944; _temp1945.last_plus_one=
_temp1944 + 37; _temp1945;}));}{ struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp1408); struct _RegionHandle _temp1946= _new_region(); struct
_RegionHandle* rgn=& _temp1946; _push_region( rgn);{ struct Cyc_List_List*
_temp1947=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1410,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); goto _LL1948; _LL1948: { struct Cyc_List_List*
_temp1949= 0; goto _LL1950; _LL1950:{ struct Cyc_List_List* _temp1951=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; goto
_LL1952; _LL1952: for( 0; _temp1951 != 0; _temp1951=(( struct Cyc_List_List*)
_check_null( _temp1951))->tl){ struct Cyc_List_List* _temp1953= _temp1947; goto
_LL1954; _LL1954: for( 0; _temp1953 != 0; _temp1953=(( struct Cyc_List_List*)
_check_null( _temp1953))->tl){ if((*(( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( _temp1953))->hd)).f1 ==( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp1951))->hd){ struct _tuple12 _temp1957; struct
Cyc_Absyn_Exp* _temp1958; struct Cyc_Absyn_Structfield* _temp1960; struct
_tuple12* _temp1955=( struct _tuple12*)(( struct Cyc_List_List*) _check_null(
_temp1953))->hd; _temp1957=* _temp1955; _LL1961: _temp1960= _temp1957.f1; goto
_LL1959; _LL1959: _temp1958= _temp1957.f2; goto _LL1956; _LL1956: { void*
_temp1962=( void*) _temp1960->type; goto _LL1963; _LL1963: Cyc_Toc_exp_to_c( nv,
_temp1958); if( Cyc_Toc_is_void_star( _temp1962)){( void*)( _temp1958->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1958->r, 0), 0))->r));} _temp1949=({ struct Cyc_List_List* _temp1964=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1964->hd=(
void*)({ struct _tuple4* _temp1965=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1965->f1= 0; _temp1965->f2= _temp1958; _temp1965;}); _temp1964->tl=
_temp1949; _temp1964;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1949), 0))->r));}}; _pop_region(& _temp1946);}} goto _LL1240; _LL1298: {
struct Cyc_List_List* fs;{ void* _temp1966= Cyc_Tcutil_compress( _temp1418);
struct Cyc_List_List* _temp1972; _LL1968: if(( unsigned int) _temp1966 > 4u?*((
int*) _temp1966) == Cyc_Absyn_AnonStructType: 0){ _LL1973: _temp1972=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1966)->f1; goto _LL1969;} else{ goto
_LL1970;} _LL1970: goto _LL1971; _LL1969: fs= _temp1972; goto _LL1967; _LL1971:
fs=(( struct Cyc_List_List*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1974= Cyc_Absynpp_typ2string( _temp1418); xprintf("anon struct has type %.*s",
_temp1974.last_plus_one - _temp1974.curr, _temp1974.curr);})); goto _LL1967;
_LL1967:;}{ struct _RegionHandle _temp1975= _new_region(); struct _RegionHandle*
rgn=& _temp1975; _push_region( rgn);{ struct Cyc_List_List* _temp1976=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1416, fs); goto _LL1977; _LL1977: for( 0; _temp1976 != 0;
_temp1976=(( struct Cyc_List_List*) _check_null( _temp1976))->tl){ struct
_tuple12 _temp1980; struct Cyc_Absyn_Exp* _temp1981; struct Cyc_Absyn_Structfield*
_temp1983; struct _tuple12* _temp1978=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( _temp1976))->hd; _temp1980=* _temp1978; _LL1984: _temp1983=
_temp1980.f1; goto _LL1982; _LL1982: _temp1981= _temp1980.f2; goto _LL1979;
_LL1979: { void* _temp1985=( void*) _temp1983->type; goto _LL1986; _LL1986: Cyc_Toc_exp_to_c(
nv, _temp1981); if( Cyc_Toc_is_void_star( _temp1985)){( void*)( _temp1981->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp1981->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1416, 0))->r));}; _pop_region(& _temp1975);} goto _LL1240;} _LL1300: {
struct _tuple0* qv= _temp1420->name; if( _temp1422->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp1987= _temp1422->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1422->fields))->v;
goto _LL1988; _LL1988: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1987))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp1987))->hd)->typs
== 0){ tag_count ++;} _temp1987=(( struct Cyc_List_List*) _check_null( _temp1987))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1240;} _LL1302: { struct _tuple0* _temp1989= _temp1430->name; goto
_LL1990; _LL1990: { struct Cyc_List_List* _temp1991= _temp1430->typs; goto
_LL1992; _LL1992: { struct _tuple0* _temp1993= Cyc_Toc_temp_var(); goto _LL1994;
_LL1994: { struct Cyc_Absyn_Exp* _temp1995= Cyc_Absyn_var_exp( _temp1993, 0);
goto _LL1996; _LL1996: { void* _temp1999= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp1989,({ char* _temp1997="_struct"; struct _tagged_string _temp1998;
_temp1998.curr= _temp1997; _temp1998.base= _temp1997; _temp1998.last_plus_one=
_temp1997 + 8; _temp1998;}))); goto _LL2000; _LL2000: if( nv->toplevel){ struct
Cyc_Absyn_Exp* tag_exp; if( _temp1432->is_xtunion){ tag_exp= Cyc_Absyn_var_exp(
_temp1989, 0);} else{ int tag_count= 0; struct Cyc_List_List* _temp2001=
_temp1432->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1432->fields))->v; goto _LL2002; _LL2002: while( Cyc_Absyn_qvar_cmp(
_temp1989,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2001))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2001))->hd)->typs != 0){ tag_count ++;}
_temp2001=(( struct Cyc_List_List*) _check_null( _temp2001))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2003= 0; goto _LL2004;
_LL2004: for( 0; _temp1434 != 0;( _temp1434=(( struct Cyc_List_List*)
_check_null( _temp1434))->tl, _temp1991=(( struct Cyc_List_List*) _check_null(
_temp1991))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1434))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1991))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2003=({ struct Cyc_List_List* _temp2005=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2005[ 0]=({ struct Cyc_List_List
_temp2006; _temp2006.hd=( void*)({ struct _tuple4* _temp2007=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2007[ 0]=({ struct _tuple4
_temp2008; _temp2008.f1= 0; _temp2008.f2= cur_e; _temp2008;}); _temp2007;});
_temp2006.tl= _temp2003; _temp2006;}); _temp2005;});} _temp2003=({ struct Cyc_List_List*
_temp2009=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2009[ 0]=({ struct Cyc_List_List _temp2010; _temp2010.hd=( void*)({ struct
_tuple4* _temp2011=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2011[ 0]=({ struct _tuple4 _temp2012; _temp2012.f1= 0; _temp2012.f2=
tag_exp; _temp2012;}); _temp2011;}); _temp2010.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2003); _temp2010;}); _temp2009;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2013=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2013->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1989,({ char* _temp2014="_struct";
struct _tagged_string _temp2015; _temp2015.curr= _temp2014; _temp2015.base=
_temp2014; _temp2015.last_plus_one= _temp2014 + 8; _temp2015;})); _temp2013;}),
_temp2003, 0))->r));}} else{ struct Cyc_List_List* _temp2017=({ struct Cyc_List_List*
_temp2016=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2016->hd=( void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1995, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1989, 0), 0); _temp2016->tl=
0; _temp2016;}); goto _LL2018; _LL2018:{ int i= 1; for( 0; _temp1434 != 0;(((
_temp1434=(( struct Cyc_List_List*) _check_null( _temp1434))->tl, i ++)),
_temp1991=(( struct Cyc_List_List*) _check_null( _temp1991))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1434))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1991))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp2020= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1995,({
struct _tagged_string* _temp2019=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2019[ 0]= xprintf("f%d", i); _temp2019;}), 0),
cur_e, 0); goto _LL2021; _LL2021: _temp2017=({ struct Cyc_List_List* _temp2022=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2022->hd=(
void*) _temp2020; _temp2022->tl= _temp2017; _temp2022;});}}}{ struct Cyc_Absyn_Stmt*
_temp2023= Cyc_Absyn_exp_stmt( _temp1995, 0); goto _LL2024; _LL2024: { struct
Cyc_Absyn_Stmt* _temp2026= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp2025=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2025->hd=(
void*) _temp2023; _temp2025->tl= _temp2017; _temp2025;})), 0); goto _LL2027;
_LL2027:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1993, _temp1999, 0, _temp2026, 0), 0))->r));}}} goto _LL1240;}}}}} _LL1304:
goto _LL1240; _LL1306: { void* t_c= Cyc_Toc_typ_to_c( _temp1446); if( _temp1448
!= 0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1448);
Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp(
rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} else{( void*)( e->r=( void*)((
void*)( Cyc_Toc_malloc_exp( _temp1446, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
goto _LL1240;} _LL1308: Cyc_Toc_stmt_to_c( nv, _temp1450); goto _LL1240; _LL1310:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp2028="UnresolvedMem";
struct _tagged_string _temp2029; _temp2029.curr= _temp2028; _temp2029.base=
_temp2028; _temp2029.last_plus_one= _temp2028 + 14; _temp2029;})); goto _LL1240;
_LL1312:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ char* _temp2030="codegen";
struct _tagged_string _temp2031; _temp2031.curr= _temp2030; _temp2031.base=
_temp2030; _temp2031.last_plus_one= _temp2030 + 8; _temp2031;})); goto _LL1240;
_LL1314:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ char* _temp2032="fill";
struct _tagged_string _temp2033; _temp2033.curr= _temp2032; _temp2033.base=
_temp2032; _temp2033.last_plus_one= _temp2032 + 5; _temp2033;})); goto _LL1240;
_LL1240:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct _tagged_string* succ_lab, struct
_tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
0);} struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct
Cyc_Absyn_Stmt* f3; } ; struct _tuple14{ struct _tuple0* f1; void* f2; } ;
struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static
struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, void* t, struct Cyc_Absyn_Exp*
r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat* p, struct _tagged_string*
succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List* decls){ struct
Cyc_Absyn_Stmt* s;{ void* _temp2034=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp2070; struct Cyc_Absyn_Vardecl _temp2072; struct Cyc_List_List* _temp2073;
struct Cyc_Core_Opt* _temp2075; struct Cyc_Absyn_Exp* _temp2077; void* _temp2079;
struct Cyc_Absyn_Tqual _temp2081; struct _tuple0* _temp2083; void* _temp2085;
struct Cyc_Absyn_Vardecl* _temp2087; int _temp2089; void* _temp2091; char
_temp2093; struct _tagged_string _temp2095; struct Cyc_Absyn_Enumfield*
_temp2097; struct Cyc_Absyn_Enumdecl* _temp2099; struct Cyc_List_List* _temp2101;
struct Cyc_List_List* _temp2103; struct Cyc_Absyn_Tunionfield* _temp2105; struct
Cyc_Absyn_Tuniondecl* _temp2107; struct Cyc_Absyn_Pat* _temp2110; struct Cyc_Absyn_Pat
_temp2112; struct Cyc_Position_Segment* _temp2113; struct Cyc_Core_Opt*
_temp2115; void* _temp2117; struct Cyc_List_List* _temp2119; struct Cyc_List_List*
_temp2121; struct Cyc_Absyn_Tunionfield* _temp2123; struct Cyc_Absyn_Tuniondecl*
_temp2125; struct Cyc_List_List* _temp2127; struct Cyc_List_List* _temp2129;
struct Cyc_Absyn_Tunionfield* _temp2131; struct Cyc_Absyn_Tuniondecl* _temp2133;
struct Cyc_List_List* _temp2135; struct Cyc_List_List* _temp2137; struct Cyc_List_List*
_temp2139; struct Cyc_Core_Opt* _temp2141; struct Cyc_Absyn_Structdecl*
_temp2143; struct Cyc_Absyn_Pat* _temp2145; struct _tuple0* _temp2147; struct
Cyc_List_List* _temp2149; struct Cyc_List_List* _temp2151; struct _tuple0*
_temp2153; struct Cyc_List_List* _temp2155; struct Cyc_List_List* _temp2157;
struct _tuple0* _temp2159; _LL2036: if(( unsigned int) _temp2034 > 2u?*(( int*)
_temp2034) == Cyc_Absyn_Var_p: 0){ _LL2071: _temp2070=(( struct Cyc_Absyn_Var_p_struct*)
_temp2034)->f1; _temp2072=* _temp2070; _LL2086: _temp2085=( void*) _temp2072.sc;
goto _LL2084; _LL2084: _temp2083= _temp2072.name; goto _LL2082; _LL2082:
_temp2081= _temp2072.tq; goto _LL2080; _LL2080: _temp2079=( void*) _temp2072.type;
goto _LL2078; _LL2078: _temp2077= _temp2072.initializer; goto _LL2076; _LL2076:
_temp2075= _temp2072.rgn; goto _LL2074; _LL2074: _temp2073= _temp2072.attributes;
goto _LL2037;} else{ goto _LL2038;} _LL2038: if( _temp2034 ==( void*) Cyc_Absyn_Wild_p){
goto _LL2039;} else{ goto _LL2040;} _LL2040: if(( unsigned int) _temp2034 > 2u?*((
int*) _temp2034) == Cyc_Absyn_Reference_p: 0){ _LL2088: _temp2087=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2034)->f1; goto _LL2041;} else{ goto _LL2042;} _LL2042: if( _temp2034 ==(
void*) Cyc_Absyn_Null_p){ goto _LL2043;} else{ goto _LL2044;} _LL2044: if((
unsigned int) _temp2034 > 2u?*(( int*) _temp2034) == Cyc_Absyn_Int_p: 0){
_LL2092: _temp2091=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp2034)->f1;
goto _LL2090; _LL2090: _temp2089=(( struct Cyc_Absyn_Int_p_struct*) _temp2034)->f2;
goto _LL2045;} else{ goto _LL2046;} _LL2046: if(( unsigned int) _temp2034 > 2u?*((
int*) _temp2034) == Cyc_Absyn_Char_p: 0){ _LL2094: _temp2093=(( struct Cyc_Absyn_Char_p_struct*)
_temp2034)->f1; goto _LL2047;} else{ goto _LL2048;} _LL2048: if(( unsigned int)
_temp2034 > 2u?*(( int*) _temp2034) == Cyc_Absyn_Float_p: 0){ _LL2096: _temp2095=((
struct Cyc_Absyn_Float_p_struct*) _temp2034)->f1; goto _LL2049;} else{ goto
_LL2050;} _LL2050: if(( unsigned int) _temp2034 > 2u?*(( int*) _temp2034) == Cyc_Absyn_Enum_p:
0){ _LL2100: _temp2099=(( struct Cyc_Absyn_Enum_p_struct*) _temp2034)->f1; goto
_LL2098; _LL2098: _temp2097=(( struct Cyc_Absyn_Enum_p_struct*) _temp2034)->f2;
goto _LL2051;} else{ goto _LL2052;} _LL2052: if(( unsigned int) _temp2034 > 2u?*((
int*) _temp2034) == Cyc_Absyn_Tunion_p: 0){ _LL2108: _temp2107=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2034)->f1; goto _LL2106; _LL2106: _temp2105=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2034)->f2; goto _LL2104; _LL2104: _temp2103=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2034)->f3; goto _LL2102; _LL2102: _temp2101=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2034)->f4; if( _temp2101 == 0){ goto _LL2053;} else{ goto _LL2054;}} else{
goto _LL2054;} _LL2054: if(( unsigned int) _temp2034 > 2u?*(( int*) _temp2034)
== Cyc_Absyn_Pointer_p: 0){ _LL2111: _temp2110=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2034)->f1; _temp2112=* _temp2110; _LL2118: _temp2117=( void*) _temp2112.r;
if(( unsigned int) _temp2117 > 2u?*(( int*) _temp2117) == Cyc_Absyn_Tunion_p: 0){
_LL2126: _temp2125=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2117)->f1; goto
_LL2124; _LL2124: _temp2123=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2117)->f2;
goto _LL2122; _LL2122: _temp2121=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2117)->f3;
goto _LL2120; _LL2120: _temp2119=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2117)->f4;
goto _LL2116;} else{ goto _LL2056;} _LL2116: _temp2115= _temp2112.topt; goto
_LL2114; _LL2114: _temp2113= _temp2112.loc; goto _LL2109;} else{ goto _LL2056;}
_LL2109: if( _temp2119 != 0){ goto _LL2055;} else{ goto _LL2056;} _LL2056: if((
unsigned int) _temp2034 > 2u?*(( int*) _temp2034) == Cyc_Absyn_Tunion_p: 0){
_LL2134: _temp2133=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2034)->f1; goto
_LL2132; _LL2132: _temp2131=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2034)->f2;
goto _LL2130; _LL2130: _temp2129=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2034)->f3;
goto _LL2128; _LL2128: _temp2127=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2034)->f4;
goto _LL2057;} else{ goto _LL2058;} _LL2058: if(( unsigned int) _temp2034 > 2u?*((
int*) _temp2034) == Cyc_Absyn_Tuple_p: 0){ _LL2136: _temp2135=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp2034)->f1; goto _LL2059;} else{ goto _LL2060;} _LL2060: if(( unsigned int)
_temp2034 > 2u?*(( int*) _temp2034) == Cyc_Absyn_Struct_p: 0){ _LL2144:
_temp2143=(( struct Cyc_Absyn_Struct_p_struct*) _temp2034)->f1; goto _LL2142;
_LL2142: _temp2141=(( struct Cyc_Absyn_Struct_p_struct*) _temp2034)->f2; goto
_LL2140; _LL2140: _temp2139=(( struct Cyc_Absyn_Struct_p_struct*) _temp2034)->f3;
goto _LL2138; _LL2138: _temp2137=(( struct Cyc_Absyn_Struct_p_struct*) _temp2034)->f4;
goto _LL2061;} else{ goto _LL2062;} _LL2062: if(( unsigned int) _temp2034 > 2u?*((
int*) _temp2034) == Cyc_Absyn_Pointer_p: 0){ _LL2146: _temp2145=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2034)->f1; goto _LL2063;} else{ goto _LL2064;} _LL2064: if(( unsigned int)
_temp2034 > 2u?*(( int*) _temp2034) == Cyc_Absyn_UnknownId_p: 0){ _LL2148:
_temp2147=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2034)->f1; goto _LL2065;}
else{ goto _LL2066;} _LL2066: if(( unsigned int) _temp2034 > 2u?*(( int*)
_temp2034) == Cyc_Absyn_UnknownCall_p: 0){ _LL2154: _temp2153=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2034)->f1; goto _LL2152; _LL2152: _temp2151=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2034)->f2; goto _LL2150; _LL2150: _temp2149=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2034)->f3; goto _LL2067;} else{ goto _LL2068;} _LL2068: if(( unsigned int)
_temp2034 > 2u?*(( int*) _temp2034) == Cyc_Absyn_UnknownFields_p: 0){ _LL2160:
_temp2159=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2034)->f1; goto
_LL2158; _LL2158: _temp2157=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2034)->f2; goto _LL2156; _LL2156: _temp2155=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2034)->f3; goto _LL2069;} else{ goto _LL2035;} _LL2037: nv= Cyc_Toc_add_varmap(
nv, _temp2083, r); goto _LL2039; _LL2039: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL2035; _LL2041: { struct _tuple0* _temp2161= Cyc_Toc_temp_var(); goto
_LL2162; _LL2162: decls=({ struct Cyc_List_List* _temp2163=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2163->hd=( void*)({ struct
_tuple14* _temp2164=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp2164->f1= _temp2161; _temp2164->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp2164;}); _temp2163->tl= decls; _temp2163;}); nv= Cyc_Toc_add_varmap(
nv, _temp2087->name, Cyc_Absyn_var_exp( _temp2161, 0)); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2161, 0), Cyc_Absyn_address_exp(
path, 0), 0), Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL2035;} _LL2043: s=
Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab);
goto _LL2035; _LL2045: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp(
_temp2091, _temp2089, 0), succ_lab, fail_lab); goto _LL2035; _LL2047: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_char_exp( _temp2093, 0), succ_lab, fail_lab); goto _LL2035; _LL2049:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp( _temp2095, 0), succ_lab,
fail_lab); goto _LL2035; _LL2051: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2165=( struct Cyc_Absyn_Enum_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct)); _temp2165[ 0]=({ struct Cyc_Absyn_Enum_e_struct
_temp2166; _temp2166.tag= Cyc_Absyn_Enum_e; _temp2166.f1= _temp2097->name;
_temp2166.f2=( struct Cyc_Absyn_Enumdecl*) _temp2099; _temp2166.f3=( struct Cyc_Absyn_Enumfield*)
_temp2097; _temp2166;}); _temp2165;}), 0), succ_lab, fail_lab); goto _LL2035;
_LL2053: { struct Cyc_Absyn_Exp* cmp_exp= Cyc_Absyn_var_exp( _temp2105->name, 0);
if( ! _temp2107->is_xtunion){ cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt( r, cmp_exp, succ_lab, fail_lab); goto
_LL2035;} _LL2055: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp2119); struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag(
_temp2123->name,({ char* _temp2202="_struct"; struct _tagged_string _temp2203;
_temp2203.curr= _temp2202; _temp2203.base= _temp2202; _temp2203.last_plus_one=
_temp2202 + 8; _temp2203;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0);{ struct
_RegionHandle _temp2167= _new_region(); struct _RegionHandle* rgn=& _temp2167;
_push_region( rgn);{ struct Cyc_List_List* _temp2168=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2123->typs);
goto _LL2169; _LL2169: { struct Cyc_List_List* _temp2170=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2119);
goto _LL2171; _LL2171: for( 0; _temp2170 != 0;((( _temp2170=(( struct Cyc_List_List*)
_check_null( _temp2170))->tl, _temp2168=(( struct Cyc_List_List*) _check_null(
_temp2168))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp2172=( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp2170))->hd; goto _LL2173; _LL2173: {
void* _temp2174=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2168))->hd)).f2; goto _LL2175; _LL2175: { struct _tuple0* _temp2176= Cyc_Toc_temp_var();
goto _LL2177; _LL2177: { void* _temp2178=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2172->topt))->v; goto _LL2179; _LL2179: { void* _temp2180= Cyc_Toc_typ_to_c(
_temp2178); goto _LL2181; _LL2181: { struct _tagged_string* _temp2182= Cyc_Toc_fresh_label();
goto _LL2183; _LL2183: { struct Cyc_Absyn_Exp* _temp2185= Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp2184=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2184[ 0]= xprintf("f%d", cnt);
_temp2184;}), 0); goto _LL2186; _LL2186: if( Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c(
_temp2174))){ _temp2185= Cyc_Absyn_cast_exp( _temp2180, _temp2185, 0);} decls=({
struct Cyc_List_List* _temp2187=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2187->hd=( void*)({ struct _tuple14* _temp2188=(
struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2188->f1= _temp2176;
_temp2188->f2= _temp2180; _temp2188;}); _temp2187->tl= decls; _temp2187;});{
struct _tuple13 _temp2189= Cyc_Toc_xlate_pat( nv, _temp2178, Cyc_Absyn_var_exp(
_temp2176, 0), _temp2185, _temp2172, succ_lab, fail_lab, decls); goto _LL2190;
_LL2190: nv= _temp2189.f1; decls= _temp2189.f2;{ struct Cyc_Absyn_Stmt*
_temp2191= _temp2189.f3; goto _LL2192; _LL2192: { struct Cyc_Absyn_Stmt*
_temp2193= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2176, 0), _temp2185, 0);
goto _LL2194; _LL2194: { struct Cyc_Absyn_Stmt* _temp2195= Cyc_Absyn_seq_stmt(
_temp2193, _temp2191, 0); goto _LL2196; _LL2196: ss=({ struct Cyc_List_List*
_temp2197=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2197->hd=( void*) Cyc_Absyn_label_stmt( _temp2182, _temp2195, 0); _temp2197->tl=
ss; _temp2197;}); succ_lab= _temp2182;}}}}}}}}}}}}}; _pop_region(& _temp2167);}{
struct Cyc_Absyn_Exp* test_exp; if( _temp2125->is_xtunion){ struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0); struct Cyc_Absyn_Exp*
e= Cyc_Absyn_var_exp( _temp2123->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0);
s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp2198=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2125->fields))->v; goto _LL2199; _LL2199: for( 0; _temp2198 !=
0; _temp2198=(( struct Cyc_List_List*) _check_null( _temp2198))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2200=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2198))->hd; goto _LL2201; _LL2201: if( _temp2200->typs == 0){ max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r,
0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp,
0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2123->name, 0); test_exp=
max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2035;}} _LL2057: _temp2135=
_temp2127; goto _LL2059; _LL2059: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2135);{ struct Cyc_List_List*
_temp2204=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2135); goto _LL2205; _LL2205: for( 0; _temp2204 != 0;( _temp2204=(( struct
Cyc_List_List*) _check_null( _temp2204))->tl, cnt --)){ struct Cyc_Absyn_Pat*
_temp2206=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null(
_temp2204))->hd; goto _LL2207; _LL2207: { struct _tuple0* _temp2208= Cyc_Toc_temp_var();
goto _LL2209; _LL2209: { void* _temp2210=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2206->topt))->v; goto _LL2211; _LL2211: { struct
_tagged_string* _temp2212= Cyc_Toc_fresh_label(); goto _LL2213; _LL2213: decls=({
struct Cyc_List_List* _temp2214=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2214->hd=( void*)({ struct _tuple14* _temp2215=(
struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2215->f1= _temp2208;
_temp2215->f2= Cyc_Toc_typ_to_c( _temp2210); _temp2215;}); _temp2214->tl= decls;
_temp2214;});{ struct _tuple13 _temp2217= Cyc_Toc_xlate_pat( nv, _temp2210, Cyc_Absyn_var_exp(
_temp2208, 0), Cyc_Absyn_structmember_exp( path,({ struct _tagged_string*
_temp2216=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2216[ 0]= xprintf("f%d", cnt); _temp2216;}), 0), _temp2206, succ_lab,
fail_lab, decls); goto _LL2218; _LL2218: nv= _temp2217.f1; decls= _temp2217.f2;{
struct Cyc_Absyn_Stmt* _temp2219= _temp2217.f3; goto _LL2220; _LL2220: { struct
Cyc_Absyn_Stmt* _temp2222= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2208, 0), Cyc_Absyn_structmember_exp( r,({ struct _tagged_string* _temp2221=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2221[ 0]= xprintf("f%d", cnt); _temp2221;}), 0), 0), _temp2219, 0); goto
_LL2223; _LL2223: ss=({ struct Cyc_List_List* _temp2224=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2224->hd=( void*) Cyc_Absyn_label_stmt(
_temp2212, _temp2222, 0); _temp2224->tl= ss; _temp2224;}); succ_lab= _temp2212;}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2035;} _LL2061: { struct Cyc_List_List*
ss= 0;{ struct Cyc_List_List* _temp2225=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2137); goto _LL2226; _LL2226: for( 0; _temp2225 != 0;
_temp2225=(( struct Cyc_List_List*) _check_null( _temp2225))->tl){ struct
_tuple15* _temp2227=( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2225))->hd; goto _LL2228; _LL2228: { struct Cyc_Absyn_Pat* _temp2229=(*
_temp2227).f2; goto _LL2230; _LL2230: { struct _tagged_string* f;{ void*
_temp2231=( void*)(( struct Cyc_List_List*) _check_null((* _temp2227).f1))->hd;
struct _tagged_string* _temp2237; _LL2233: if(*(( int*) _temp2231) == Cyc_Absyn_FieldName){
_LL2238: _temp2237=(( struct Cyc_Absyn_FieldName_struct*) _temp2231)->f1; goto
_LL2234;} else{ goto _LL2235;} _LL2235: goto _LL2236; _LL2234: f= _temp2237;
goto _LL2232; _LL2236:( void) _throw(( void*) Cyc_Toc_Match_error); _LL2232:;}{
struct _tuple0* _temp2239= Cyc_Toc_temp_var(); goto _LL2240; _LL2240: { void*
_temp2241=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2229->topt))->v;
goto _LL2242; _LL2242: { void* _temp2243= Cyc_Toc_typ_to_c( _temp2241); goto
_LL2244; _LL2244: { struct _tagged_string* _temp2245= Cyc_Toc_fresh_label();
goto _LL2246; _LL2246: decls=({ struct Cyc_List_List* _temp2247=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2247->hd=( void*)({ struct
_tuple14* _temp2248=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp2248->f1= _temp2239; _temp2248->f2= _temp2243; _temp2248;}); _temp2247->tl=
decls; _temp2247;});{ struct _tuple13 _temp2249= Cyc_Toc_xlate_pat( nv,
_temp2241, Cyc_Absyn_var_exp( _temp2239, 0), Cyc_Absyn_structmember_exp( path, f,
0), _temp2229, succ_lab, fail_lab, decls); goto _LL2250; _LL2250: nv= _temp2249.f1;
decls= _temp2249.f2;{ struct Cyc_Absyn_Exp* _temp2251= Cyc_Absyn_structmember_exp(
r, f, 0); goto _LL2252; _LL2252: if( Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2143->fields))->v, f)))->type)){ _temp2251= Cyc_Absyn_cast_exp(
_temp2243, _temp2251, 0);}{ struct Cyc_Absyn_Stmt* _temp2253= _temp2249.f3; goto
_LL2254; _LL2254: { struct Cyc_Absyn_Stmt* _temp2255= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2239, 0), _temp2251, 0), _temp2253, 0); goto _LL2256;
_LL2256: ss=({ struct Cyc_List_List* _temp2257=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2257->hd=( void*) Cyc_Absyn_label_stmt(
_temp2245, _temp2255, 0); _temp2257->tl= ss; _temp2257;}); succ_lab= _temp2245;}}}}}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2035;} _LL2063: { struct _tuple0*
_temp2258= Cyc_Toc_temp_var(); goto _LL2259; _LL2259: { void* _temp2260=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2145->topt))->v; goto _LL2261; _LL2261:
decls=({ struct Cyc_List_List* _temp2262=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2262->hd=( void*)({ struct _tuple14*
_temp2263=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2263->f1=
_temp2258; _temp2263->f2= Cyc_Toc_typ_to_c( _temp2260); _temp2263;}); _temp2262->tl=
decls; _temp2262;});{ struct _tuple13 _temp2264= Cyc_Toc_xlate_pat( nv,
_temp2260, Cyc_Absyn_var_exp( _temp2258, 0), Cyc_Absyn_deref_exp( path, 0),
_temp2145, succ_lab, fail_lab, decls); goto _LL2265; _LL2265: nv= _temp2264.f1;
decls= _temp2264.f2;{ struct Cyc_Absyn_Stmt* _temp2266= _temp2264.f3; goto
_LL2267; _LL2267: { struct Cyc_Absyn_Stmt* _temp2268= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2258, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2266, 0);
goto _LL2269; _LL2269: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2268, 0);} else{ s= _temp2268;} goto _LL2035;}}}}} _LL2065: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp2270="unknownid"; struct _tagged_string _temp2271; _temp2271.curr=
_temp2270; _temp2271.base= _temp2270; _temp2271.last_plus_one= _temp2270 + 10;
_temp2271;})); goto _LL2035; _LL2067: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp2272="unknowncall"; struct
_tagged_string _temp2273; _temp2273.curr= _temp2272; _temp2273.base= _temp2272;
_temp2273.last_plus_one= _temp2272 + 12; _temp2273;})); goto _LL2035; _LL2069: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp2274="unknownfields"; struct _tagged_string _temp2275; _temp2275.curr=
_temp2274; _temp2275.base= _temp2274; _temp2275.last_plus_one= _temp2274 + 14;
_temp2275;})); goto _LL2035; _LL2035:;} return({ struct _tuple13 _temp2276;
_temp2276.f1= nv; _temp2276.f2= decls; _temp2276.f3= s; _temp2276;});} struct
_tuple16{ struct _tagged_string* f1; struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple16* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause*
sc){ return({ struct _tuple16* _temp2277=( struct _tuple16*) GC_malloc( sizeof(
struct _tuple16)); _temp2277->f1= Cyc_Toc_fresh_label(); _temp2277->f2= Cyc_Toc_fresh_label();
_temp2277->f3= sc; _temp2277;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp2278=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2279; _LL2279: { int leave_as_switch;{ void*
_temp2280= Cyc_Tcutil_compress( _temp2278); void* _temp2286; void* _temp2288;
_LL2282: if(( unsigned int) _temp2280 > 4u?*(( int*) _temp2280) == Cyc_Absyn_IntType:
0){ _LL2289: _temp2288=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2280)->f1;
goto _LL2287; _LL2287: _temp2286=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2280)->f2; goto _LL2283;} else{ goto _LL2284;} _LL2284: goto _LL2285;
_LL2283: leave_as_switch= 1; goto _LL2281; _LL2285: leave_as_switch= 0; goto
_LL2281; _LL2281:;}{ struct Cyc_List_List* _temp2290= scs; goto _LL2291; _LL2291:
for( 0; _temp2290 != 0; _temp2290=(( struct Cyc_List_List*) _check_null(
_temp2290))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2290))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2290))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2292= scs; goto
_LL2293; _LL2293: for( 0; _temp2292 != 0; _temp2292=(( struct Cyc_List_List*)
_check_null( _temp2292))->tl){ struct Cyc_Absyn_Stmt* _temp2294=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2292))->hd)->body; goto _LL2295;
_LL2295:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2292))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2294, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2294);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct
_tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple16*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* _temp2296= lscs; goto _LL2297; _LL2297: for( 0; _temp2296
!= 0; _temp2296=(( struct Cyc_List_List*) _check_null( _temp2296))->tl){ struct
Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2296))->hd)).f3; struct _tagged_string* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp2296))->tl == 0? end_l:(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp2296))->tl))->hd)).f1;
struct _tagged_string* succ_lab=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2296))->hd)).f2; if( sc->where_clause == 0){ struct _tuple13
_temp2298= Cyc_Toc_xlate_pat( nv, _temp2278, r, path, sc->pattern, succ_lab,
fail_lab, decls); goto _LL2299; _LL2299: nvs=({ struct Cyc_List_List* _temp2300=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2300->hd=(
void*) _temp2298.f1; _temp2300->tl= nvs; _temp2300;}); decls= _temp2298.f2;
test_stmts=({ struct Cyc_List_List* _temp2301=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2301->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2296))->hd)).f1,
_temp2298.f3, 0); _temp2301->tl= test_stmts; _temp2301;});} else{ struct Cyc_Absyn_Exp*
_temp2302=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); goto _LL2303;
_LL2303: { struct _tagged_string* _temp2304= Cyc_Toc_fresh_label(); goto _LL2305;
_LL2305: { struct _tuple13 _temp2306= Cyc_Toc_xlate_pat( nv, _temp2278, r, path,
sc->pattern, _temp2304, fail_lab, decls); goto _LL2307; _LL2307: Cyc_Toc_exp_to_c(
_temp2306.f1, _temp2302); nvs=({ struct Cyc_List_List* _temp2308=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2308->hd=( void*) _temp2306.f1;
_temp2308->tl= nvs; _temp2308;}); decls= _temp2306.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2302, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2304, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2309=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2309->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2296))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2306.f3, s3, 0), 0); _temp2309->tl= test_stmts;
_temp2309;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2310= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2311; _LL2311: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0;( lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2312=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2313; _LL2313: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2312, end_l,(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2314=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2315; _LL2315: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2314, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2310, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple14 _temp2318;
void* _temp2319; struct _tuple0* _temp2321; struct _tuple14* _temp2316=( struct
_tuple14*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2318=*
_temp2316; _LL2322: _temp2321= _temp2318.f1; goto _LL2320; _LL2320: _temp2319=
_temp2318.f2; goto _LL2317; _LL2317: res= Cyc_Absyn_declare_stmt( _temp2321,
_temp2319, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct
Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt* Cyc_Toc_make_npop_handler(
int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv,
0),({ struct Cyc_List_List* _temp2323=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List) * 1); _temp2323[ 0]=({ struct Cyc_List_List
_temp2324; _temp2324.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0);
_temp2324.tl= 0; _temp2324;}); _temp2323;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2325=( void*) s->r; struct Cyc_Absyn_Exp*
_temp2365; struct Cyc_Absyn_Stmt* _temp2367; struct Cyc_Absyn_Stmt* _temp2369;
struct Cyc_Absyn_Exp* _temp2371; struct Cyc_Absyn_Stmt* _temp2373; struct Cyc_Absyn_Stmt*
_temp2375; struct Cyc_Absyn_Exp* _temp2377; struct Cyc_Absyn_Stmt* _temp2379;
struct _tuple2 _temp2381; struct Cyc_Absyn_Stmt* _temp2383; struct Cyc_Absyn_Exp*
_temp2385; struct Cyc_Absyn_Stmt* _temp2387; struct Cyc_Absyn_Stmt* _temp2389;
struct Cyc_Absyn_Stmt* _temp2391; struct _tagged_string* _temp2393; struct Cyc_Absyn_Stmt*
_temp2395; struct _tuple2 _temp2397; struct Cyc_Absyn_Stmt* _temp2399; struct
Cyc_Absyn_Exp* _temp2401; struct _tuple2 _temp2403; struct Cyc_Absyn_Stmt*
_temp2405; struct Cyc_Absyn_Exp* _temp2407; struct Cyc_Absyn_Exp* _temp2409;
struct Cyc_List_List* _temp2411; struct Cyc_Absyn_Exp* _temp2413; struct Cyc_Absyn_Switch_clause**
_temp2415; struct Cyc_List_List* _temp2417; struct Cyc_Absyn_Stmt* _temp2419;
struct Cyc_Absyn_Decl* _temp2421; struct Cyc_Absyn_Stmt* _temp2423; struct Cyc_Absyn_Stmt*
_temp2425; struct Cyc_Absyn_Stmt* _temp2427; struct _tagged_string* _temp2429;
struct _tuple2 _temp2431; struct Cyc_Absyn_Stmt* _temp2433; struct Cyc_Absyn_Exp*
_temp2435; struct Cyc_Absyn_Stmt* _temp2437; struct Cyc_List_List* _temp2439;
struct Cyc_Absyn_Stmt* _temp2441; struct Cyc_Absyn_Stmt* _temp2443; struct Cyc_Absyn_Vardecl*
_temp2445; struct Cyc_Absyn_Tvar* _temp2447; _LL2327: if( _temp2325 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2328;} else{ goto _LL2329;} _LL2329: if((
unsigned int) _temp2325 > 1u?*(( int*) _temp2325) == Cyc_Absyn_Exp_s: 0){
_LL2366: _temp2365=(( struct Cyc_Absyn_Exp_s_struct*) _temp2325)->f1; goto
_LL2330;} else{ goto _LL2331;} _LL2331: if(( unsigned int) _temp2325 > 1u?*((
int*) _temp2325) == Cyc_Absyn_Seq_s: 0){ _LL2370: _temp2369=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2325)->f1; goto _LL2368; _LL2368: _temp2367=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2325)->f2; goto _LL2332;} else{ goto _LL2333;} _LL2333: if(( unsigned int)
_temp2325 > 1u?*(( int*) _temp2325) == Cyc_Absyn_Return_s: 0){ _LL2372:
_temp2371=(( struct Cyc_Absyn_Return_s_struct*) _temp2325)->f1; goto _LL2334;}
else{ goto _LL2335;} _LL2335: if(( unsigned int) _temp2325 > 1u?*(( int*)
_temp2325) == Cyc_Absyn_IfThenElse_s: 0){ _LL2378: _temp2377=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2325)->f1; goto _LL2376; _LL2376: _temp2375=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2325)->f2; goto _LL2374; _LL2374: _temp2373=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2325)->f3; goto _LL2336;} else{ goto _LL2337;} _LL2337: if(( unsigned int)
_temp2325 > 1u?*(( int*) _temp2325) == Cyc_Absyn_While_s: 0){ _LL2382: _temp2381=((
struct Cyc_Absyn_While_s_struct*) _temp2325)->f1; _LL2386: _temp2385= _temp2381.f1;
goto _LL2384; _LL2384: _temp2383= _temp2381.f2; goto _LL2380; _LL2380: _temp2379=((
struct Cyc_Absyn_While_s_struct*) _temp2325)->f2; goto _LL2338;} else{ goto
_LL2339;} _LL2339: if(( unsigned int) _temp2325 > 1u?*(( int*) _temp2325) == Cyc_Absyn_Break_s:
0){ _LL2388: _temp2387=(( struct Cyc_Absyn_Break_s_struct*) _temp2325)->f1; goto
_LL2340;} else{ goto _LL2341;} _LL2341: if(( unsigned int) _temp2325 > 1u?*((
int*) _temp2325) == Cyc_Absyn_Continue_s: 0){ _LL2390: _temp2389=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2325)->f1; goto _LL2342;} else{ goto _LL2343;} _LL2343: if(( unsigned int)
_temp2325 > 1u?*(( int*) _temp2325) == Cyc_Absyn_Goto_s: 0){ _LL2394: _temp2393=((
struct Cyc_Absyn_Goto_s_struct*) _temp2325)->f1; goto _LL2392; _LL2392:
_temp2391=(( struct Cyc_Absyn_Goto_s_struct*) _temp2325)->f2; goto _LL2344;}
else{ goto _LL2345;} _LL2345: if(( unsigned int) _temp2325 > 1u?*(( int*)
_temp2325) == Cyc_Absyn_For_s: 0){ _LL2410: _temp2409=(( struct Cyc_Absyn_For_s_struct*)
_temp2325)->f1; goto _LL2404; _LL2404: _temp2403=(( struct Cyc_Absyn_For_s_struct*)
_temp2325)->f2; _LL2408: _temp2407= _temp2403.f1; goto _LL2406; _LL2406:
_temp2405= _temp2403.f2; goto _LL2398; _LL2398: _temp2397=(( struct Cyc_Absyn_For_s_struct*)
_temp2325)->f3; _LL2402: _temp2401= _temp2397.f1; goto _LL2400; _LL2400:
_temp2399= _temp2397.f2; goto _LL2396; _LL2396: _temp2395=(( struct Cyc_Absyn_For_s_struct*)
_temp2325)->f4; goto _LL2346;} else{ goto _LL2347;} _LL2347: if(( unsigned int)
_temp2325 > 1u?*(( int*) _temp2325) == Cyc_Absyn_Switch_s: 0){ _LL2414:
_temp2413=(( struct Cyc_Absyn_Switch_s_struct*) _temp2325)->f1; goto _LL2412;
_LL2412: _temp2411=(( struct Cyc_Absyn_Switch_s_struct*) _temp2325)->f2; goto
_LL2348;} else{ goto _LL2349;} _LL2349: if(( unsigned int) _temp2325 > 1u?*((
int*) _temp2325) == Cyc_Absyn_Fallthru_s: 0){ _LL2418: _temp2417=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2325)->f1; goto _LL2416; _LL2416: _temp2415=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2325)->f2; goto _LL2350;} else{ goto _LL2351;} _LL2351: if(( unsigned int)
_temp2325 > 1u?*(( int*) _temp2325) == Cyc_Absyn_Decl_s: 0){ _LL2422: _temp2421=((
struct Cyc_Absyn_Decl_s_struct*) _temp2325)->f1; goto _LL2420; _LL2420:
_temp2419=(( struct Cyc_Absyn_Decl_s_struct*) _temp2325)->f2; goto _LL2352;}
else{ goto _LL2353;} _LL2353: if(( unsigned int) _temp2325 > 1u?*(( int*)
_temp2325) == Cyc_Absyn_Cut_s: 0){ _LL2424: _temp2423=(( struct Cyc_Absyn_Cut_s_struct*)
_temp2325)->f1; goto _LL2354;} else{ goto _LL2355;} _LL2355: if(( unsigned int)
_temp2325 > 1u?*(( int*) _temp2325) == Cyc_Absyn_Splice_s: 0){ _LL2426:
_temp2425=(( struct Cyc_Absyn_Splice_s_struct*) _temp2325)->f1; goto _LL2356;}
else{ goto _LL2357;} _LL2357: if(( unsigned int) _temp2325 > 1u?*(( int*)
_temp2325) == Cyc_Absyn_Label_s: 0){ _LL2430: _temp2429=(( struct Cyc_Absyn_Label_s_struct*)
_temp2325)->f1; goto _LL2428; _LL2428: _temp2427=(( struct Cyc_Absyn_Label_s_struct*)
_temp2325)->f2; goto _LL2358;} else{ goto _LL2359;} _LL2359: if(( unsigned int)
_temp2325 > 1u?*(( int*) _temp2325) == Cyc_Absyn_Do_s: 0){ _LL2438: _temp2437=((
struct Cyc_Absyn_Do_s_struct*) _temp2325)->f1; goto _LL2432; _LL2432: _temp2431=((
struct Cyc_Absyn_Do_s_struct*) _temp2325)->f2; _LL2436: _temp2435= _temp2431.f1;
goto _LL2434; _LL2434: _temp2433= _temp2431.f2; goto _LL2360;} else{ goto
_LL2361;} _LL2361: if(( unsigned int) _temp2325 > 1u?*(( int*) _temp2325) == Cyc_Absyn_TryCatch_s:
0){ _LL2442: _temp2441=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2325)->f1;
goto _LL2440; _LL2440: _temp2439=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2325)->f2; goto _LL2362;} else{ goto _LL2363;} _LL2363: if(( unsigned int)
_temp2325 > 1u?*(( int*) _temp2325) == Cyc_Absyn_Region_s: 0){ _LL2448:
_temp2447=(( struct Cyc_Absyn_Region_s_struct*) _temp2325)->f1; goto _LL2446;
_LL2446: _temp2445=(( struct Cyc_Absyn_Region_s_struct*) _temp2325)->f2; goto
_LL2444; _LL2444: _temp2443=(( struct Cyc_Absyn_Region_s_struct*) _temp2325)->f3;
goto _LL2364;} else{ goto _LL2326;} _LL2328: return; _LL2330: Cyc_Toc_exp_to_c(
nv, _temp2365); return; _LL2332: Cyc_Toc_stmt_to_c( nv, _temp2369); s= _temp2367;
continue; _LL2334: { struct Cyc_Core_Opt* topt= 0; if( _temp2371 != 0){ topt=({
struct Cyc_Core_Opt* _temp2449=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2449->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2371))->topt))->v);
_temp2449;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2371));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2450=
Cyc_Toc_temp_var(); goto _LL2451; _LL2451: { struct Cyc_Absyn_Stmt* _temp2452=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2450, 0),
0); goto _LL2453; _LL2453:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2450,( void*)(( struct Cyc_Core_Opt*) _check_null( topt))->v, _temp2371,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), _temp2452, 0), 0))->r));}}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2336: Cyc_Toc_exp_to_c(
nv, _temp2377); Cyc_Toc_stmt_to_c( nv, _temp2375); s= _temp2373; continue;
_LL2338: Cyc_Toc_exp_to_c( nv, _temp2385); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2379); return; _LL2340: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null( nv->break_lab))->v, 0))->r));}{ int dest_depth= _temp2387 == 0? 0:((
struct Cyc_Absyn_Stmt*) _check_null( _temp2387))->try_depth; Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2342: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -(( struct Cyc_Absyn_Stmt*) _check_null( _temp2389))->try_depth, s);
return; _LL2344: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2391))->try_depth, s); return; _LL2346: Cyc_Toc_exp_to_c( nv,
_temp2409); Cyc_Toc_exp_to_c( nv, _temp2407); Cyc_Toc_exp_to_c( nv, _temp2401);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2395); return; _LL2348: Cyc_Toc_xlate_switch(
nv, s, _temp2413, _temp2411); return; _LL2350: if( nv->fallthru_info == 0){(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp2454="fallthru in unexpected place"; struct _tagged_string _temp2455;
_temp2455.curr= _temp2454; _temp2455.base= _temp2454; _temp2455.last_plus_one=
_temp2454 + 29; _temp2455;})));}{ struct _tuple8 _temp2458; struct Cyc_Dict_Dict*
_temp2459; struct Cyc_List_List* _temp2461; struct _tagged_string* _temp2463;
struct _tuple8* _temp2456=( struct _tuple8*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp2458=* _temp2456; _LL2464: _temp2463= _temp2458.f1;
goto _LL2462; _LL2462: _temp2461= _temp2458.f2; goto _LL2460; _LL2460: _temp2459=
_temp2458.f3; goto _LL2457; _LL2457: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp2463, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp2415)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp2465=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2461); goto _LL2466; _LL2466: { struct Cyc_List_List* _temp2467=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp2417); goto
_LL2468; _LL2468: for( 0; _temp2465 != 0;( _temp2465=(( struct Cyc_List_List*)
_check_null( _temp2465))->tl, _temp2467=(( struct Cyc_List_List*) _check_null(
_temp2467))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp2467))->hd); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2459,( struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp2465))->hd),(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp2467))->hd, 0),
s2, 0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}}} _LL2352:{ void*
_temp2469=( void*) _temp2421->r; int _temp2479; struct Cyc_Absyn_Exp* _temp2481;
struct Cyc_Core_Opt* _temp2483; struct Cyc_Core_Opt* _temp2485; struct Cyc_Absyn_Pat*
_temp2487; struct Cyc_List_List* _temp2489; struct Cyc_Absyn_Vardecl* _temp2491;
_LL2471: if(*(( int*) _temp2469) == Cyc_Absyn_Let_d){ _LL2488: _temp2487=((
struct Cyc_Absyn_Let_d_struct*) _temp2469)->f1; goto _LL2486; _LL2486: _temp2485=((
struct Cyc_Absyn_Let_d_struct*) _temp2469)->f2; goto _LL2484; _LL2484: _temp2483=((
struct Cyc_Absyn_Let_d_struct*) _temp2469)->f3; goto _LL2482; _LL2482: _temp2481=((
struct Cyc_Absyn_Let_d_struct*) _temp2469)->f4; goto _LL2480; _LL2480: _temp2479=((
struct Cyc_Absyn_Let_d_struct*) _temp2469)->f5; goto _LL2472;} else{ goto
_LL2473;} _LL2473: if(*(( int*) _temp2469) == Cyc_Absyn_Letv_d){ _LL2490:
_temp2489=(( struct Cyc_Absyn_Letv_d_struct*) _temp2469)->f1; goto _LL2474;}
else{ goto _LL2475;} _LL2475: if(*(( int*) _temp2469) == Cyc_Absyn_Var_d){
_LL2492: _temp2491=(( struct Cyc_Absyn_Var_d_struct*) _temp2469)->f1; goto
_LL2476;} else{ goto _LL2477;} _LL2477: goto _LL2478; _LL2472:( void*)( s->r=(
void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2487, _temp2483, _temp2481,
_temp2479, _temp2419))->r)); goto _LL2470; _LL2474: { struct Cyc_List_List*
_temp2493=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2489); goto _LL2494; _LL2494: if( _temp2493 == 0){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ char* _temp2495="empty Letv_d"; struct
_tagged_string _temp2496; _temp2496.curr= _temp2495; _temp2496.base= _temp2495;
_temp2496.last_plus_one= _temp2495 + 13; _temp2496;}));}( void*)( _temp2421->r=(
void*)(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2497=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2497[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2498; _temp2498.tag= Cyc_Absyn_Var_d; _temp2498.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2493))->hd; _temp2498;}); _temp2497;})));
_temp2493=(( struct Cyc_List_List*) _check_null( _temp2493))->tl; for( 0;
_temp2493 != 0; _temp2493=(( struct Cyc_List_List*) _check_null( _temp2493))->tl){
struct Cyc_Absyn_Decl* _temp2501= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2499=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2499[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2500; _temp2500.tag= Cyc_Absyn_Var_d;
_temp2500.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2493))->hd; _temp2500;}); _temp2499;}), 0); goto _LL2502; _LL2502:( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp2501, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL2470;} _LL2476: {
struct _tuple0* _temp2503= _temp2491->name; goto _LL2504; _LL2504:( void*)(
_temp2491->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2491->type));{
struct Cyc_Toc_Env* _temp2507= Cyc_Toc_add_varmap( nv, _temp2503, Cyc_Absyn_varb_exp(
_temp2503,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp2505=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2505[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2506; _temp2506.tag= Cyc_Absyn_Local_b; _temp2506.f1=
_temp2491; _temp2506;}); _temp2505;}), 0)); goto _LL2508; _LL2508: Cyc_Toc_stmt_to_c(
_temp2507, _temp2419); if( _temp2491->initializer != 0){ struct Cyc_Absyn_Exp*
init=( struct Cyc_Absyn_Exp*) _check_null( _temp2491->initializer); void*
_temp2509=( void*) init->r; struct Cyc_Absyn_Exp* _temp2515; struct Cyc_Absyn_Exp*
_temp2517; struct Cyc_Absyn_Vardecl* _temp2519; _LL2511: if(*(( int*) _temp2509)
== Cyc_Absyn_Comprehension_e){ _LL2520: _temp2519=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2509)->f1; goto _LL2518; _LL2518: _temp2517=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2509)->f2; goto _LL2516; _LL2516: _temp2515=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2509)->f3; goto _LL2512;} else{ goto _LL2513;} _LL2513: goto _LL2514;
_LL2512: _temp2491->initializer= 0;( void*)( _temp2419->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2503, 0), _temp2519, _temp2517, _temp2515, Cyc_Absyn_new_stmt((
void*) _temp2419->r, 0), 0))->r)); goto _LL2510; _LL2514: Cyc_Toc_exp_to_c( nv,
init); goto _LL2510; _LL2510:;} goto _LL2470;}} _LL2478:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ char* _temp2521="bad nested declaration within function";
struct _tagged_string _temp2522; _temp2522.curr= _temp2521; _temp2522.base=
_temp2521; _temp2522.last_plus_one= _temp2521 + 39; _temp2522;})); goto _LL2470;
_LL2470:;} return; _LL2354:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
char* _temp2523="cut"; struct _tagged_string _temp2524; _temp2524.curr=
_temp2523; _temp2524.base= _temp2523; _temp2524.last_plus_one= _temp2523 + 4;
_temp2524;})); return; _LL2356:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
char* _temp2525="splice"; struct _tagged_string _temp2526; _temp2526.curr=
_temp2525; _temp2526.base= _temp2525; _temp2526.last_plus_one= _temp2525 + 7;
_temp2526;})); return; _LL2358: s= _temp2427; continue; _LL2360: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp2437); Cyc_Toc_exp_to_c( nv, _temp2435); return;
_LL2362: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__push_handler_qv,
0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp( Cyc_Toc_setjmp_qv, 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__pop_handler_qv,
0); struct Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp( Cyc_Toc__exn_thrown_qv,
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2527=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2527->v=( void*) e_typ; _temp2527;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2441);{
struct Cyc_Absyn_Stmt* _temp2528= Cyc_Absyn_seq_stmt( _temp2441, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); goto _LL2529; _LL2529: {
struct _tuple0* _temp2530= Cyc_Toc_temp_var(); goto _LL2531; _LL2531: { struct
Cyc_Absyn_Exp* _temp2532= Cyc_Absyn_var_exp( _temp2530, 0); goto _LL2533;
_LL2533: { struct Cyc_Absyn_Vardecl* _temp2534= Cyc_Absyn_new_vardecl( _temp2530,
Cyc_Absyn_exn_typ, 0); goto _LL2535; _LL2535: _temp2532->topt=({ struct Cyc_Core_Opt*
_temp2536=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2536->v=( void*) Cyc_Absyn_exn_typ; _temp2536;});{ struct Cyc_Absyn_Pat*
_temp2541=({ struct Cyc_Absyn_Pat* _temp2537=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2537->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2539=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2539[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2540; _temp2540.tag= Cyc_Absyn_Var_p;
_temp2540.f1= _temp2534; _temp2540;}); _temp2539;})); _temp2537->topt=({ struct
Cyc_Core_Opt* _temp2538=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2538->v=( void*) Cyc_Absyn_exn_typ; _temp2538;}); _temp2537->loc= 0;
_temp2537;}); goto _LL2542; _LL2542: { struct Cyc_Absyn_Exp* _temp2543= Cyc_Absyn_throw_exp(
_temp2532, 0); goto _LL2544; _LL2544: _temp2543->topt=({ struct Cyc_Core_Opt*
_temp2545=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2545->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2545;});{ struct Cyc_Absyn_Stmt*
_temp2546= Cyc_Absyn_exp_stmt( _temp2543, 0); goto _LL2547; _LL2547: { struct
Cyc_Absyn_Switch_clause* _temp2551=({ struct Cyc_Absyn_Switch_clause* _temp2548=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2548->pattern= _temp2541; _temp2548->pat_vars=({ struct Cyc_Core_Opt*
_temp2549=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2549->v=( void*)({ struct Cyc_List_List* _temp2550=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2550->hd=( void*) _temp2534;
_temp2550->tl= 0; _temp2550;}); _temp2549;}); _temp2548->where_clause= 0;
_temp2548->body= _temp2546; _temp2548->loc= 0; _temp2548;}); goto _LL2552;
_LL2552: { struct Cyc_Absyn_Stmt* _temp2554= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2439,({ struct Cyc_List_List* _temp2553=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2553->hd=( void*) _temp2551; _temp2553->tl=
0; _temp2553;})), 0); goto _LL2555; _LL2555: Cyc_Toc_stmt_to_c( nv, _temp2554);{
struct Cyc_Absyn_Exp* _temp2557= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2556=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2556->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2556->tl= 0; _temp2556;}), 0); goto _LL2558; _LL2558: { struct Cyc_Absyn_Stmt*
_temp2560= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2559=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2559->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2559->tl= 0;
_temp2559;}), 0), 0); goto _LL2561; _LL2561: { struct Cyc_Absyn_Exp* _temp2562=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2563; _LL2563: {
struct Cyc_Absyn_Exp* _temp2564= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2565; _LL2565:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2560, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2562, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2557, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2564, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2528, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2554, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2364: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2445->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2571=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2571[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2572; _temp2572.tag= Cyc_Absyn_Local_b; _temp2572.f1=
_temp2445; _temp2572;}); _temp2571;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2570=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2570->f1= Cyc_Toc_abs_ns; _temp2570->f2= Cyc_Toc__push_region_sp;
_temp2570;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2569=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2569->f1= Cyc_Toc_abs_ns; _temp2569->f2= Cyc_Toc__pop_region_sp; _temp2569;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2568=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2568->f1=
Cyc_Toc_abs_ns; _temp2568->f2= Cyc_Toc__new_region_sp; _temp2568;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2567=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2567->hd=( void*) addr_rh_exp; _temp2567->tl= 0; _temp2567;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2566=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2566->hd=( void*) x_exp; _temp2566->tl= 0; _temp2566;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2443);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2443, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2326:;}} struct _tuple17{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0;(
void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{ struct
Cyc_List_List* _temp2573= f->args; goto _LL2574; _LL2574: for( 0; _temp2573 != 0;
_temp2573=(( struct Cyc_List_List*) _check_null( _temp2573))->tl){(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp2573))->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp2573))->hd)).f3);{
struct _tuple0* _temp2576=({ struct _tuple0* _temp2575=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2575->f1=( void*) Cyc_Absyn_Loc_n;
_temp2575->f2=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp2573))->hd)).f1; _temp2575;}); goto _LL2577; _LL2577: nv= Cyc_Toc_add_varmap(
nv, _temp2576, Cyc_Absyn_var_exp( _temp2576, 0));}}}{ struct Cyc_List_List*
_temp2578=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( f->param_vardecls))->v;
goto _LL2579; _LL2579: for( 0; _temp2578 != 0; _temp2578=(( struct Cyc_List_List*)
_check_null( _temp2578))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_List_List*) _check_null( _temp2578))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2578))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2580= s; _LL2582: if(
_temp2580 ==( void*) Cyc_Absyn_Abstract){ goto _LL2583;} else{ goto _LL2584;}
_LL2584: if( _temp2580 ==( void*) Cyc_Absyn_ExternC){ goto _LL2585;} else{ goto
_LL2586;} _LL2586: goto _LL2587; _LL2583: return( void*) Cyc_Absyn_Public;
_LL2585: return( void*) Cyc_Absyn_Extern; _LL2587: return s; _LL2581:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2588=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2589; _LL2589: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2590=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2590->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2590;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2591=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2588).f2); goto _LL2592;
_LL2592: if( _temp2591 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2588).f2, s));}
else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null(
_temp2591))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2588).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2593=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2593->v=( void*)
_temp2588; _temp2593;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2594=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2595; _LL2595: for( 0; _temp2594 != 0;
_temp2594=(( struct Cyc_List_List*) _check_null( _temp2594))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2594))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2594))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2596=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2597; _LL2597: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2598=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2598->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2598;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2599=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2596).f2); goto _LL2600; _LL2600:
if( _temp2599 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2596).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2599))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2596).f2, u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)(
u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==(
void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?( void*)
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2601=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2601->v=( void*) _temp2596; _temp2601;}); if( u->fields !=
0){ if( seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* _temp2602=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( u->fields))->v; goto
_LL2603; _LL2603: for( 0; _temp2602 != 0; _temp2602=(( struct Cyc_List_List*)
_check_null( _temp2602))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2602))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2602))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2604=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2604->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2604;});}{ struct
_tuple0* _temp2605= tud->name; goto _LL2606; _LL2606: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2605)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2605));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2607=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2608; _LL2608: for( 0;
_temp2607 != 0; _temp2607=(( struct Cyc_List_List*) _check_null( _temp2607))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2607))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2609; _temp2609.q_const= 1; _temp2609.q_volatile= 0;
_temp2609.q_restrict= 0; _temp2609;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2610=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2610->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2611=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2611[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2612; _temp2612.tag= Cyc_Absyn_Var_d;
_temp2612.f1= vd; _temp2612;}); _temp2611;}), 0); _temp2610->tl= Cyc_Toc_result_decls;
_temp2610;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2613; _temp2613.q_const=
1; _temp2613.q_volatile= 0; _temp2613.q_restrict= 0; _temp2613;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2614=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2614->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2615=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2615[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2616; _temp2616.tag= Cyc_Absyn_Var_d; _temp2616.f1= vd; _temp2616;});
_temp2615;}), 0); _temp2614->tl= Cyc_Toc_result_decls; _temp2614;});{ struct Cyc_List_List*
_temp2617= 0; goto _LL2618; _LL2618: { int i= 1;{ struct Cyc_List_List*
_temp2619= f->typs; goto _LL2620; _LL2620: for( 0; _temp2619 != 0;( _temp2619=((
struct Cyc_List_List*) _check_null( _temp2619))->tl, i ++)){ struct
_tagged_string* _temp2622=( struct _tagged_string*)({ struct _tagged_string*
_temp2621=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp2621[ 0]= xprintf("f%d", i); _temp2621;}); goto _LL2623; _LL2623: { struct
Cyc_Absyn_Structfield* _temp2625=({ struct Cyc_Absyn_Structfield* _temp2624=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2624->name= _temp2622; _temp2624->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2619))->hd)).f1; _temp2624->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2619))->hd)).f2);
_temp2624->width= 0; _temp2624->attributes= 0; _temp2624;}); goto _LL2626;
_LL2626: _temp2617=({ struct Cyc_List_List* _temp2627=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2627->hd=( void*) _temp2625;
_temp2627->tl= _temp2617; _temp2627;});}}} _temp2617=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2617); _temp2617=({ struct Cyc_List_List*
_temp2628=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2628->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2629=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2629->name= Cyc_Toc_tag_sp;
_temp2629->tq= Cyc_Toc_mt_tq; _temp2629->type=( void*) Cyc_Absyn_sint_t;
_temp2629->width= 0; _temp2629->attributes= 0; _temp2629;}); _temp2628->tl=
_temp2617; _temp2628;});{ struct Cyc_Absyn_Structdecl* _temp2635=({ struct Cyc_Absyn_Structdecl*
_temp2630=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2630->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2630->name=({ struct Cyc_Core_Opt*
_temp2632=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2632->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ char* _temp2633="_struct";
struct _tagged_string _temp2634; _temp2634.curr= _temp2633; _temp2634.base=
_temp2633; _temp2634.last_plus_one= _temp2633 + 8; _temp2634;})); _temp2632;});
_temp2630->tvs= 0; _temp2630->fields=({ struct Cyc_Core_Opt* _temp2631=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2631->v=( void*)
_temp2617; _temp2631;}); _temp2630->attributes= 0; _temp2630;}); goto _LL2636;
_LL2636: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2637=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2637->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2638=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2638[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2639; _temp2639.tag= Cyc_Absyn_Struct_d;
_temp2639.f1= _temp2635; _temp2639;}); _temp2638;}), 0); _temp2637->tl= Cyc_Toc_result_decls;
_temp2637;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2640=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2640->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2640;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2641= xd->name; goto _LL2642; _LL2642: {
struct Cyc_List_List* _temp2643=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2644; _LL2644: for( 0; _temp2643 != 0;
_temp2643=(( struct Cyc_List_List*) _check_null( _temp2643))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2643))->hd; struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2646= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2645=* fn;(
unsigned int)( _temp2645.last_plus_one - _temp2645.curr);}) + 4, 0); goto
_LL2647; _LL2647: { struct Cyc_Absyn_ArrayType_struct* _temp2650=({ struct Cyc_Absyn_ArrayType_struct*
_temp2648=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2648[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2649; _temp2649.tag= Cyc_Absyn_ArrayType;
_temp2649.f1=( void*) Cyc_Absyn_uchar_t; _temp2649.f2= Cyc_Toc_mt_tq; _temp2649.f3=(
struct Cyc_Absyn_Exp*) _temp2646; _temp2649;}); _temp2648;}); goto _LL2651;
_LL2651: { struct Cyc_Core_Opt* _temp2652=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2660; int _temp2661; _LL2654: if( _temp2652 == 0){ goto
_LL2655;} else{ goto _LL2656;} _LL2656: if( _temp2652 == 0){ goto _LL2658;}
else{ _temp2660=* _temp2652; _LL2662: _temp2661=( int) _temp2660.v; if(
_temp2661 == 0){ goto _LL2657;} else{ goto _LL2658;}} _LL2658: goto _LL2659;
_LL2655: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(({ int _temp2663= 0; int
_temp2664= 0; int _temp2665= 0; int _temp2666= 0; struct _tagged_string
_temp2667=* fn; xprintf("%c%c%c%c%.*s", _temp2663, _temp2664, _temp2665,
_temp2666, _temp2667.last_plus_one - _temp2667.curr, _temp2667.curr);}), 0);}{
struct Cyc_Absyn_Vardecl* _temp2668= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2650, initopt); goto _LL2669; _LL2669:( void*)( _temp2668->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2670=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2670->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2671=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2671[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2672; _temp2672.tag= Cyc_Absyn_Var_d;
_temp2672.f1= _temp2668; _temp2672;}); _temp2671;}), 0); _temp2670->tl= Cyc_Toc_result_decls;
_temp2670;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2673= f->typs; goto _LL2674; _LL2674: for( 0;
_temp2673 != 0;( _temp2673=(( struct Cyc_List_List*) _check_null( _temp2673))->tl,
i ++)){ struct _tagged_string* _temp2676=( struct _tagged_string*)({ struct
_tagged_string* _temp2675=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2675[ 0]= xprintf("f%d", i); _temp2675;}); goto
_LL2677; _LL2677: { struct Cyc_Absyn_Structfield* _temp2679=({ struct Cyc_Absyn_Structfield*
_temp2678=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2678->name= _temp2676; _temp2678->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2673))->hd)).f1; _temp2678->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2673))->hd)).f2);
_temp2678->width= 0; _temp2678->attributes= 0; _temp2678;}); goto _LL2680;
_LL2680: fields=({ struct Cyc_List_List* _temp2681=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2681->hd=( void*) _temp2679;
_temp2681->tl= fields; _temp2681;});}}} fields=({ struct Cyc_List_List*
_temp2682=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2682->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2683=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2683->name= Cyc_Toc_tag_sp;
_temp2683->tq= Cyc_Toc_mt_tq; _temp2683->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2683->width= 0; _temp2683->attributes= 0; _temp2683;});
_temp2682->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2682;});{ struct Cyc_Absyn_Structdecl* _temp2689=({ struct Cyc_Absyn_Structdecl*
_temp2684=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2684->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2684->name=({ struct Cyc_Core_Opt*
_temp2686=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2686->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ char* _temp2687="_struct";
struct _tagged_string _temp2688; _temp2688.curr= _temp2687; _temp2688.base=
_temp2687; _temp2688.last_plus_one= _temp2687 + 8; _temp2688;})); _temp2686;});
_temp2684->tvs= 0; _temp2684->fields=({ struct Cyc_Core_Opt* _temp2685=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2685->v=( void*)
fields; _temp2685;}); _temp2684->attributes= 0; _temp2684;}); goto _LL2690;
_LL2690: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2691=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2691->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2692=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2692[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2693; _temp2693.tag= Cyc_Absyn_Struct_d;
_temp2693.f1= _temp2689; _temp2693;}); _temp2692;}), 0); _temp2691->tl= Cyc_Toc_result_decls;
_temp2691;});}} goto _LL2653;}} _LL2657: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2699= Cyc_Absyn_string_exp(({ int _temp2694= 0; int
_temp2695= 0; int _temp2696= 0; int _temp2697= 0; struct _tagged_string
_temp2698=* fn; xprintf("%c%c%c%c%.*s", _temp2694, _temp2695, _temp2696,
_temp2697, _temp2698.last_plus_one - _temp2698.curr, _temp2698.curr);}), 0);
goto _LL2700; _LL2700: { struct Cyc_Absyn_Vardecl* _temp2701= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2650,( struct Cyc_Absyn_Exp*) _temp2699); goto _LL2702;
_LL2702:( void*)( _temp2701->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2703=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2703->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2704=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2704[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2705; _temp2705.tag= Cyc_Absyn_Var_d; _temp2705.f1= _temp2701; _temp2705;});
_temp2704;}), 0); _temp2703->tl= Cyc_Toc_result_decls; _temp2703;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2653; _LL2659: goto
_LL2653; _LL2653:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2706= ed->name; goto
_LL2707; _LL2707:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2708=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; goto _LL2709; _LL2709: for( 0; _temp2708 != 0;
_temp2708=(( struct Cyc_List_List*) _check_null( _temp2708))->tl){ if((( struct
Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( _temp2708))->hd)->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp2708))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2708))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label(); struct Cyc_Absyn_Stmt*
_temp2712; struct Cyc_List_List* _temp2714; struct Cyc_Toc_Env* _temp2716;
struct _tuple13 _temp2710= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp( x, 0),
Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL2717: _temp2716=
_temp2710.f1; goto _LL2715; _LL2715: _temp2714= _temp2710.f2; goto _LL2713;
_LL2713: _temp2712= _temp2710.f3; goto _LL2711; _LL2711: Cyc_Toc_stmt_to_c(
_temp2716, s);{ struct Cyc_Absyn_Stmt* _temp2718= Cyc_Absyn_label_stmt( succ_lab,
s, 0); goto _LL2719; _LL2719: if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2712, _temp2718, 0), 0);}
else{ struct Cyc_Absyn_Exp* _temp2721= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp2720=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2720->f1= Cyc_Toc_abs_ns; _temp2720->f2= Cyc_Toc_Match_Exception_sp;
_temp2720;}), 0)); goto _LL2722; _LL2722: { struct Cyc_Absyn_Stmt* _temp2723=
Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2721, 0), 0); goto
_LL2724; _LL2724: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2712, Cyc_Absyn_seq_stmt( _temp2723,
_temp2718, 0), 0), 0);}} for( 0; _temp2714 != 0; _temp2714=(( struct Cyc_List_List*)
_check_null( _temp2714))->tl){ struct _tuple14 _temp2727; void* _temp2728;
struct _tuple0* _temp2730; struct _tuple14* _temp2725=( struct _tuple14*)((
struct Cyc_List_List*) _check_null( _temp2714))->hd; _temp2727=* _temp2725;
_LL2731: _temp2730= _temp2727.f1; goto _LL2729; _LL2729: _temp2728= _temp2727.f2;
goto _LL2726; _LL2726: s= Cyc_Absyn_declare_stmt( _temp2730, _temp2728, 0, s, 0);}
return s;}}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2732=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp2758; struct Cyc_Absyn_Fndecl* _temp2760; int
_temp2762; struct Cyc_Absyn_Exp* _temp2764; struct Cyc_Core_Opt* _temp2766;
struct Cyc_Core_Opt* _temp2768; struct Cyc_Absyn_Pat* _temp2770; struct Cyc_List_List*
_temp2772; struct Cyc_Absyn_Structdecl* _temp2774; struct Cyc_Absyn_Uniondecl*
_temp2776; struct Cyc_Absyn_Tuniondecl* _temp2778; struct Cyc_Absyn_Enumdecl*
_temp2780; struct Cyc_Absyn_Typedefdecl* _temp2782; struct Cyc_List_List*
_temp2784; struct _tagged_string* _temp2786; struct Cyc_List_List* _temp2788;
struct _tuple0* _temp2790; struct Cyc_List_List* _temp2792; _LL2734: if(*(( int*)
_temp2732) == Cyc_Absyn_Var_d){ _LL2759: _temp2758=(( struct Cyc_Absyn_Var_d_struct*)
_temp2732)->f1; goto _LL2735;} else{ goto _LL2736;} _LL2736: if(*(( int*)
_temp2732) == Cyc_Absyn_Fn_d){ _LL2761: _temp2760=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2732)->f1; goto _LL2737;} else{ goto _LL2738;} _LL2738: if(*(( int*)
_temp2732) == Cyc_Absyn_Let_d){ _LL2771: _temp2770=(( struct Cyc_Absyn_Let_d_struct*)
_temp2732)->f1; goto _LL2769; _LL2769: _temp2768=(( struct Cyc_Absyn_Let_d_struct*)
_temp2732)->f2; goto _LL2767; _LL2767: _temp2766=(( struct Cyc_Absyn_Let_d_struct*)
_temp2732)->f3; goto _LL2765; _LL2765: _temp2764=(( struct Cyc_Absyn_Let_d_struct*)
_temp2732)->f4; goto _LL2763; _LL2763: _temp2762=(( struct Cyc_Absyn_Let_d_struct*)
_temp2732)->f5; goto _LL2739;} else{ goto _LL2740;} _LL2740: if(*(( int*)
_temp2732) == Cyc_Absyn_Letv_d){ _LL2773: _temp2772=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2732)->f1; goto _LL2741;} else{ goto _LL2742;} _LL2742: if(*(( int*)
_temp2732) == Cyc_Absyn_Struct_d){ _LL2775: _temp2774=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2732)->f1; goto _LL2743;} else{ goto _LL2744;} _LL2744: if(*(( int*)
_temp2732) == Cyc_Absyn_Union_d){ _LL2777: _temp2776=(( struct Cyc_Absyn_Union_d_struct*)
_temp2732)->f1; goto _LL2745;} else{ goto _LL2746;} _LL2746: if(*(( int*)
_temp2732) == Cyc_Absyn_Tunion_d){ _LL2779: _temp2778=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2732)->f1; goto _LL2747;} else{ goto _LL2748;} _LL2748: if(*(( int*)
_temp2732) == Cyc_Absyn_Enum_d){ _LL2781: _temp2780=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2732)->f1; goto _LL2749;} else{ goto _LL2750;} _LL2750: if(*(( int*)
_temp2732) == Cyc_Absyn_Typedef_d){ _LL2783: _temp2782=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2732)->f1; goto _LL2751;} else{ goto _LL2752;} _LL2752: if(*(( int*)
_temp2732) == Cyc_Absyn_Namespace_d){ _LL2787: _temp2786=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2732)->f1; goto _LL2785; _LL2785: _temp2784=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2732)->f2; goto _LL2753;} else{ goto _LL2754;} _LL2754: if(*(( int*)
_temp2732) == Cyc_Absyn_Using_d){ _LL2791: _temp2790=(( struct Cyc_Absyn_Using_d_struct*)
_temp2732)->f1; goto _LL2789; _LL2789: _temp2788=(( struct Cyc_Absyn_Using_d_struct*)
_temp2732)->f2; goto _LL2755;} else{ goto _LL2756;} _LL2756: if(*(( int*)
_temp2732) == Cyc_Absyn_ExternC_d){ _LL2793: _temp2792=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2732)->f1; goto _LL2757;} else{ goto _LL2733;} _LL2735: { struct _tuple0*
_temp2794= _temp2758->name; goto _LL2795; _LL2795: if(( void*) _temp2758->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2794=({ struct _tuple0* _temp2796=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2796->f1= Cyc_Toc_abs_ns;
_temp2796->f2=(* _temp2794).f2; _temp2796;});} if( _temp2758->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null( _temp2758->initializer));}
if( ! nv->toplevel){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
char* _temp2797="decls_to_c: not at toplevel!"; struct _tagged_string _temp2798;
_temp2798.curr= _temp2797; _temp2798.base= _temp2797; _temp2798.last_plus_one=
_temp2797 + 29; _temp2798;}));} nv= Cyc_Toc_add_varmap( nv, _temp2758->name, Cyc_Absyn_varb_exp(
_temp2794,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp2799=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp2799[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp2800; _temp2800.tag= Cyc_Absyn_Global_b;
_temp2800.f1= _temp2758; _temp2800;}); _temp2799;}), 0)); _temp2758->name=
_temp2794;( void*)( _temp2758->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2758->sc));(
void*)( _temp2758->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2758->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2801=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2801->hd=( void*) d; _temp2801->tl=
Cyc_Toc_result_decls; _temp2801;}); goto _LL2733;} _LL2737: { struct _tuple0*
_temp2802= _temp2760->name; goto _LL2803; _LL2803: if(( void*) _temp2760->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2802=({ struct _tuple0* _temp2804=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2804->f1= Cyc_Toc_abs_ns;
_temp2804->f2=(* _temp2802).f2; _temp2804;});} nv= Cyc_Toc_add_varmap( nv,
_temp2760->name, Cyc_Absyn_var_exp( _temp2802, 0)); _temp2760->name= _temp2802;
Cyc_Toc_fndecl_to_c( nv, _temp2760); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2805=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2805->hd=( void*) d; _temp2805->tl= Cyc_Toc_result_decls; _temp2805;});
goto _LL2733;} _LL2739:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp2806="letdecl"; struct _tagged_string _temp2807; _temp2807.curr=
_temp2806; _temp2807.base= _temp2806; _temp2807.last_plus_one= _temp2806 + 8;
_temp2807;})); goto _LL2733; _LL2741:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp2808="letdecl"; struct _tagged_string _temp2809; _temp2809.curr=
_temp2808; _temp2809.base= _temp2808; _temp2809.last_plus_one= _temp2808 + 8;
_temp2809;})); goto _LL2733; _LL2743: Cyc_Toc_structdecl_to_c( _temp2774); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2810=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2810->hd=( void*) d; _temp2810->tl= Cyc_Toc_result_decls;
_temp2810;}); goto _LL2733; _LL2745: Cyc_Toc_uniondecl_to_c( _temp2776); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2811=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2811->hd=( void*) d; _temp2811->tl= Cyc_Toc_result_decls;
_temp2811;}); goto _LL2733; _LL2747: if( _temp2778->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2778);} else{ Cyc_Toc_tuniondecl_to_c( _temp2778);} goto _LL2733; _LL2749:
Cyc_Toc_enumdecl_to_c( nv, _temp2780); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2812=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2812->hd=( void*) d; _temp2812->tl= Cyc_Toc_result_decls; _temp2812;});
goto _LL2733; _LL2751: _temp2782->name= _temp2782->name; _temp2782->tvs= 0;(
void*)( _temp2782->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2782->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2813=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2813->hd=( void*) d; _temp2813->tl=
Cyc_Toc_result_decls; _temp2813;}); goto _LL2733; _LL2753: _temp2788= _temp2784;
goto _LL2755; _LL2755: _temp2792= _temp2788; goto _LL2757; _LL2757: nv= Cyc_Toc_decls_to_c(
nv, _temp2792, top); goto _LL2733; _LL2733:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}