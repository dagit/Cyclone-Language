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
struct Cyc_List_List* ds); extern struct _tuple0* Cyc_Toc_temp_var(); int Cyc_Toc_use_runtimecheck_funs=
1; static struct Cyc_List_List* Cyc_Toc_result_decls= 0; struct Cyc_Absyn_Abs_n_struct
Cyc_Toc_abs_ns_value={ 1u, 0}; void* Cyc_Toc_abs_ns=( void*)& Cyc_Toc_abs_ns_value;
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq=( struct Cyc_Absyn_Tqual){.q_const= 0,.q_volatile=
0,.q_restrict= 0}; char Cyc_Toc_Toc_Unimplemented[ 22u]="\000\000\000\000Toc_Unimplemented";
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
4u]="tag"; static struct _tagged_string Cyc_Toc_tag_string=( struct
_tagged_string){ _temp20, _temp20, _temp20 + 4u}; static struct _tagged_string*
Cyc_Toc_tag_sp=& Cyc_Toc_tag_string; static char _temp23[ 14u]="_handler_cons";
static struct _tagged_string Cyc_Toc__handler_cons_string=( struct
_tagged_string){ _temp23, _temp23, _temp23 + 14u}; static struct _tagged_string*
Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string; static char _temp26[ 13u]="_push_region";
static struct _tagged_string Cyc_Toc__push_region_string=( struct _tagged_string){
_temp26, _temp26, _temp26 + 13u}; static struct _tagged_string* Cyc_Toc__push_region_sp=&
Cyc_Toc__push_region_string; static char _temp29[ 12u]="_pop_region"; static
struct _tagged_string Cyc_Toc__pop_region_string=( struct _tagged_string){
_temp29, _temp29, _temp29 + 12u}; static struct _tagged_string* Cyc_Toc__pop_region_sp=&
Cyc_Toc__pop_region_string; static char _temp32[ 8u]="handler"; static struct
_tagged_string Cyc_Toc_handler_string=( struct _tagged_string){ _temp32, _temp32,
_temp32 + 8u}; static struct _tagged_string* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _temp35[ 16u]="Match_Exception"; static struct _tagged_string Cyc_Toc_Match_Exception_string=(
struct _tagged_string){ _temp35, _temp35, _temp35 + 16u}; static struct
_tagged_string* Cyc_Toc_Match_Exception_sp=& Cyc_Toc_Match_Exception_string;
static char _temp38[ 15u]="Cyc_new_string"; static struct _tagged_string Cyc_Toc_Cyc_new_string_string=(
struct _tagged_string){ _temp38, _temp38, _temp38 + 15u}; static struct
_tagged_string* Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string;
static char _temp41[ 16u]="Cyc_rnew_string"; static struct _tagged_string Cyc_Toc_Cyc_rnew_string_string=(
struct _tagged_string){ _temp41, _temp41, _temp41 + 16u}; static struct
_tagged_string* Cyc_Toc_Cyc_rnew_string_sp=& Cyc_Toc_Cyc_rnew_string_string;
static char _temp44[ 14u]="_RegionHandle"; static struct _tagged_string Cyc_Toc__RegionHandle_string=(
struct _tagged_string){ _temp44, _temp44, _temp44 + 14u}; static struct
_tagged_string* Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string; static
char _temp47[ 12u]="_new_region"; static struct _tagged_string Cyc_Toc__new_region_string=(
struct _tagged_string){ _temp47, _temp47, _temp47 + 12u}; static struct
_tagged_string* Cyc_Toc__new_region_sp=& Cyc_Toc__new_region_string; static char
_temp50[ 7u]="_throw"; static struct _tagged_string Cyc_Toc__throw_string=(
struct _tagged_string){ _temp50, _temp50, _temp50 + 7u}; static struct _tuple0
Cyc_Toc__throw_pr=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__throw_string};
static struct _tuple0* Cyc_Toc__throw_qv=& Cyc_Toc__throw_pr; static char
_temp53[ 7u]="setjmp"; static struct _tagged_string Cyc_Toc_setjmp_string=(
struct _tagged_string){ _temp53, _temp53, _temp53 + 7u}; static struct _tuple0
Cyc_Toc_setjmp_pr=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc_setjmp_string};
static struct _tuple0* Cyc_Toc_setjmp_qv=& Cyc_Toc_setjmp_pr; static char
_temp56[ 14u]="_push_handler"; static struct _tagged_string Cyc_Toc__push_handler_string=(
struct _tagged_string){ _temp56, _temp56, _temp56 + 14u}; static struct _tuple0
Cyc_Toc__push_handler_pr=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__push_handler_string}; static struct _tuple0* Cyc_Toc__push_handler_qv=&
Cyc_Toc__push_handler_pr; static char _temp59[ 13u]="_pop_handler"; static
struct _tagged_string Cyc_Toc__pop_handler_string=( struct _tagged_string){
_temp59, _temp59, _temp59 + 13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr=(
struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__pop_handler_string};
static struct _tuple0* Cyc_Toc__pop_handler_qv=& Cyc_Toc__pop_handler_pr; static
char _temp62[ 12u]="_exn_thrown"; static struct _tagged_string Cyc_Toc__exn_thrown_string=(
struct _tagged_string){ _temp62, _temp62, _temp62 + 12u}; static struct _tuple0
Cyc_Toc__exn_thrown_pr=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__exn_thrown_string}; static struct _tuple0* Cyc_Toc__exn_thrown_qv=& Cyc_Toc__exn_thrown_pr;
static char _temp65[ 14u]="_npop_handler"; static struct _tagged_string Cyc_Toc__npop_handler_string=(
struct _tagged_string){ _temp65, _temp65, _temp65 + 14u}; static struct _tuple0
Cyc_Toc__npop_handler_pr=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__npop_handler_string}; static struct _tuple0* Cyc_Toc__npop_handler_qv=&
Cyc_Toc__npop_handler_pr; static char _temp68[ 12u]="_check_null"; static struct
_tagged_string Cyc_Toc__check_null_string=( struct _tagged_string){ _temp68,
_temp68, _temp68 + 12u}; static struct _tuple0 Cyc_Toc__check_null_pr=( struct
_tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__check_null_string};
static struct _tuple0* Cyc_Toc__check_null_qv=& Cyc_Toc__check_null_pr; static
char _temp71[ 25u]="_check_unknown_subscript"; static struct _tagged_string Cyc_Toc__check_unknown_subscript_string=(
struct _tagged_string){ _temp71, _temp71, _temp71 + 25u}; static struct _tuple0
Cyc_Toc__check_unknown_subscript_pr=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__check_unknown_subscript_string}; static struct _tuple0* Cyc_Toc__check_unknown_subscript_qv=&
Cyc_Toc__check_unknown_subscript_pr; static char _temp74[ 28u]="_check_known_subscript_null";
static struct _tagged_string Cyc_Toc__check_known_subscript_null_string=( struct
_tagged_string){ _temp74, _temp74, _temp74 + 28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr=(
struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__check_known_subscript_null_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_null_qv=& Cyc_Toc__check_known_subscript_null_pr;
static char _temp77[ 31u]="_check_known_subscript_notnull"; static struct
_tagged_string Cyc_Toc__check_known_subscript_notnull_string=( struct
_tagged_string){ _temp77, _temp77, _temp77 + 31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr=(
struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__check_known_subscript_notnull_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_notnull_qv=& Cyc_Toc__check_known_subscript_notnull_pr;
static char _temp80[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_GC_malloc_string=(
struct _tagged_string){ _temp80, _temp80, _temp80 + 10u}; static struct _tuple0
Cyc_Toc_GC_malloc_pr=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_GC_malloc_string}; static struct _tuple0* Cyc_Toc_GC_malloc_qv=& Cyc_Toc_GC_malloc_pr;
static char _temp83[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_GC_malloc_atomic_string=(
struct _tagged_string){ _temp83, _temp83, _temp83 + 17u}; static struct _tuple0
Cyc_Toc_GC_malloc_atomic_pr=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_GC_malloc_atomic_string}; static struct _tuple0* Cyc_Toc_GC_malloc_atomic_qv=&
Cyc_Toc_GC_malloc_atomic_pr; static char _temp86[ 15u]="_region_malloc"; static
struct _tagged_string Cyc_Toc__region_malloc_string=( struct _tagged_string){
_temp86, _temp86, _temp86 + 15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr=(
struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__region_malloc_string};
static struct _tuple0* Cyc_Toc__region_malloc_qv=& Cyc_Toc__region_malloc_pr;
struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static
int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp87=( void*) e->r; void*
_temp113; char _temp115; void* _temp117; void* _temp119; short _temp121; void*
_temp123; void* _temp125; int _temp127; void* _temp129; void* _temp131;
long long _temp133; void* _temp135; void* _temp137; struct Cyc_Absyn_Exp*
_temp139; void* _temp141; struct Cyc_List_List* _temp143; struct Cyc_List_List*
_temp145; struct Cyc_Absyn_Structdecl* _temp147; struct Cyc_List_List* _temp149;
struct Cyc_Core_Opt* _temp151; struct _tuple0* _temp153; struct Cyc_List_List*
_temp155; struct _tuple1* _temp157; struct Cyc_List_List* _temp159; struct Cyc_Core_Opt*
_temp161; _LL89: if(*(( int*) _temp87) == Cyc_Absyn_Const_e){ _LL114: _temp113=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp87)->f1; if(( unsigned int)
_temp113 > 1u?*(( int*) _temp113) == Cyc_Absyn_Char_c: 0){ _LL118: _temp117=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp113)->f1; goto _LL116; _LL116:
_temp115=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp113)->f2; goto _LL90;}
else{ goto _LL91;}} else{ goto _LL91;} _LL91: if(*(( int*) _temp87) == Cyc_Absyn_Const_e){
_LL120: _temp119=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp87)->f1; if((
unsigned int) _temp119 > 1u?*(( int*) _temp119) == Cyc_Absyn_Short_c: 0){ _LL124:
_temp123=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp119)->f1; goto _LL122;
_LL122: _temp121=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp119)->f2;
goto _LL92;} else{ goto _LL93;}} else{ goto _LL93;} _LL93: if(*(( int*) _temp87)
== Cyc_Absyn_Const_e){ _LL126: _temp125=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp87)->f1; if(( unsigned int) _temp125 > 1u?*(( int*) _temp125) == Cyc_Absyn_Int_c:
0){ _LL130: _temp129=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp125)->f1;
goto _LL128; _LL128: _temp127=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp125)->f2;
goto _LL94;} else{ goto _LL95;}} else{ goto _LL95;} _LL95: if(*(( int*) _temp87)
== Cyc_Absyn_Const_e){ _LL132: _temp131=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp87)->f1; if(( unsigned int) _temp131 > 1u?*(( int*) _temp131) == Cyc_Absyn_LongLong_c:
0){ _LL136: _temp135=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp131)->f1;
goto _LL134; _LL134: _temp133=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp131)->f2; goto _LL96;} else{ goto _LL97;}} else{ goto _LL97;} _LL97: if(*((
int*) _temp87) == Cyc_Absyn_Const_e){ _LL138: _temp137=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp87)->f1; if( _temp137 ==( void*) Cyc_Absyn_Null_c){ goto _LL98;} else{ goto
_LL99;}} else{ goto _LL99;} _LL99: if(*(( int*) _temp87) == Cyc_Absyn_Cast_e){
_LL142: _temp141=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp87)->f1; goto
_LL140; _LL140: _temp139=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp87)->f2; goto _LL100;} else{ goto _LL101;} _LL101: if(*(( int*) _temp87) ==
Cyc_Absyn_Tuple_e){ _LL144: _temp143=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp87)->f1; goto _LL102;} else{ goto _LL103;} _LL103: if(*(( int*) _temp87) ==
Cyc_Absyn_Array_e){ _LL146: _temp145=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp87)->f1; goto _LL104;} else{ goto _LL105;} _LL105: if(*(( int*) _temp87) ==
Cyc_Absyn_Struct_e){ _LL154: _temp153=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp87)->f1; goto _LL152; _LL152: _temp151=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp87)->f2; goto _LL150; _LL150: _temp149=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp87)->f3; goto _LL148; _LL148: _temp147=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp87)->f4; goto
_LL106;} else{ goto _LL107;} _LL107: if(*(( int*) _temp87) == Cyc_Absyn_CompoundLit_e){
_LL158: _temp157=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp87)->f1; goto _LL156; _LL156: _temp155=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp87)->f2; goto _LL108;} else{ goto _LL109;}
_LL109: if(*(( int*) _temp87) == Cyc_Absyn_UnresolvedMem_e){ _LL162: _temp161=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp87)->f1;
goto _LL160; _LL160: _temp159=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp87)->f2; goto _LL110;} else{ goto _LL111;} _LL111: goto _LL112; _LL90:
return _temp115 =='\000'; _LL92: return _temp121 == 0; _LL94: return _temp127 ==
0; _LL96: return _temp133 == 0; _LL98: return 1; _LL100: return Cyc_Toc_is_zero(
_temp139); _LL102: return(( int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct
Cyc_List_List* x)) Cyc_List_forall)( Cyc_Toc_is_zero, _temp143); _LL104:
_temp149= _temp145; goto _LL106; _LL106: _temp155= _temp149; goto _LL108; _LL108:
_temp159= _temp155; goto _LL110; _LL110: for( 0; _temp159 != 0; _temp159=((
struct Cyc_List_List*) _check_null( _temp159))->tl){ if( ! Cyc_Toc_is_zero((*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp159))->hd)).f2)){
return 0;}} return 1; _LL112: return 0; _LL88:;} static int Cyc_Toc_is_nullable(
void* t){ void* _temp163= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp169; struct Cyc_Absyn_Conref* _temp171; struct Cyc_Absyn_Tqual _temp173;
struct Cyc_Absyn_Conref* _temp175; void* _temp177; void* _temp179; _LL165: if((
unsigned int) _temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_PointerType: 0){
_LL170: _temp169=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp163)->f1; _LL180: _temp179=( void*) _temp169.elt_typ; goto _LL178; _LL178:
_temp177=( void*) _temp169.rgn_typ; goto _LL176; _LL176: _temp175=( struct Cyc_Absyn_Conref*)
_temp169.nullable; goto _LL174; _LL174: _temp173=( struct Cyc_Absyn_Tqual)
_temp169.tq; goto _LL172; _LL172: _temp171=( struct Cyc_Absyn_Conref*) _temp169.bounds;
goto _LL166;} else{ goto _LL167;} _LL167: goto _LL168; _LL166: { void* _temp181=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp175))->v; int _temp187; _LL183: if(( unsigned int) _temp181 > 1u?*(( int*)
_temp181) == Cyc_Absyn_Eq_constr: 0){ _LL188: _temp187=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp181)->f1; goto _LL184;} else{ goto _LL185;} _LL185: goto _LL186; _LL184:
return _temp187; _LL186: return 0; _LL182:;} _LL168:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp189=(
char*)"is_nullable"; struct _tagged_string _temp190; _temp190.curr= _temp189;
_temp190.base= _temp189; _temp190.last_plus_one= _temp189 + 12; _temp190;}));
return 0; _LL164:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct
_tuple0* x, struct _tagged_string tag){ return({ struct _tuple0* _temp191=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp191->f1=(* x).f1;
_temp191->f2=({ struct _tagged_string* _temp192=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp192[ 0]= Cyc_String_strconcat(*(*
x).f2, tag); _temp192;}); _temp191;});} static void Cyc_Toc_add_tuple_decl(
struct _tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List*
_temp193= 0; goto _LL194; _LL194:{ int i= 1; for( 0; ts != 0;( ts=(( struct Cyc_List_List*)
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
struct Cyc_Absyn_ArrayType_struct*) _temp283)->f1; goto _LL294; _LL294: _temp293=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp283)->f2;
goto _LL292; _LL292: _temp291=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp283)->f3; goto _LL286;} else{ goto _LL287;} _LL287: if(( unsigned int)
_temp283 > 4u?*(( int*) _temp283) == Cyc_Absyn_Evar: 0){ _LL305: _temp304=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp283)->f1; goto _LL300; _LL300: _temp299=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp283)->f2; if(
_temp299 == 0){ goto _LL289;} else{ _temp301=* _temp299; _LL303: _temp302=( void*)
_temp301.v; goto _LL298;} _LL298: _temp297=( int)(( struct Cyc_Absyn_Evar_struct*)
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
_temp309)->f1; goto _LL364; _LL364: _temp363=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp309)->f2; if( _temp363 == 0){ goto _LL362;} else{
goto _LL315;} _LL362: _temp361=( int)(( struct Cyc_Absyn_Evar_struct*) _temp309)->f3;
goto _LL314;} else{ goto _LL315;} _LL315: if(( unsigned int) _temp309 > 4u?*((
int*) _temp309) == Cyc_Absyn_Evar: 0){ _LL375: _temp374=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp309)->f1; goto _LL370; _LL370: _temp369=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp309)->f2; if( _temp369 == 0){ goto _LL317;} else{
_temp371=* _temp369; _LL373: _temp372=( void*) _temp371.v; goto _LL368;} _LL368:
_temp367=( int)(( struct Cyc_Absyn_Evar_struct*) _temp309)->f3; goto _LL316;}
else{ goto _LL317;} _LL317: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309)
== Cyc_Absyn_VarType: 0){ _LL377: _temp376=( struct Cyc_Absyn_Tvar*)(( struct
Cyc_Absyn_VarType_struct*) _temp309)->f1; goto _LL318;} else{ goto _LL319;}
_LL319: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_TunionType:
0){ _LL379: _temp378=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp309)->f1; _LL385: _temp384=( void*) _temp378.tunion_info; if(*(( int*)
_temp384) == Cyc_Absyn_KnownTunion){ _LL387: _temp386=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp384)->f1; goto _LL383;} else{ goto
_LL321;} _LL383: _temp382=( struct Cyc_List_List*) _temp378.targs; goto _LL381;
_LL381: _temp380=( void*) _temp378.rgn; goto _LL320;} else{ goto _LL321;} _LL321:
if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_TunionType: 0){
_LL389: _temp388=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp309)->f1; goto _LL322;} else{ goto _LL323;} _LL323: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_TunionFieldType: 0){ _LL391:
_temp390=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp309)->f1; _LL395: _temp394=( void*) _temp390.field_info; if(*(( int*)
_temp394) == Cyc_Absyn_KnownTunionfield){ _LL399: _temp398=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp394)->f1; goto _LL397; _LL397:
_temp396=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp394)->f2; goto _LL393;} else{ goto _LL325;} _LL393: _temp392=( struct Cyc_List_List*)
_temp390.targs; goto _LL324;} else{ goto _LL325;} _LL325: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_TunionFieldType: 0){ _LL401:
_temp400=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp309)->f1; goto _LL326;} else{ goto _LL327;} _LL327: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_PointerType: 0){ _LL403: _temp402=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp309)->f1;
_LL413: _temp412=( void*) _temp402.elt_typ; goto _LL411; _LL411: _temp410=( void*)
_temp402.rgn_typ; goto _LL409; _LL409: _temp408=( struct Cyc_Absyn_Conref*)
_temp402.nullable; goto _LL407; _LL407: _temp406=( struct Cyc_Absyn_Tqual)
_temp402.tq; goto _LL405; _LL405: _temp404=( struct Cyc_Absyn_Conref*) _temp402.bounds;
goto _LL328;} else{ goto _LL329;} _LL329: if(( unsigned int) _temp309 > 4u?*((
int*) _temp309) == Cyc_Absyn_IntType: 0){ _LL417: _temp416=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp309)->f1; goto _LL415; _LL415: _temp414=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp309)->f2; goto _LL330;} else{ goto _LL331;} _LL331: if( _temp309 ==( void*)
Cyc_Absyn_FloatType){ goto _LL332;} else{ goto _LL333;} _LL333: if( _temp309 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL334;} else{ goto _LL335;} _LL335: if((
unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_ArrayType: 0){
_LL423: _temp422=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp309)->f1;
goto _LL421; _LL421: _temp420=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp309)->f2; goto _LL419; _LL419: _temp418=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp309)->f3; goto _LL336;} else{ goto _LL337;}
_LL337: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_FnType:
0){ _LL425: _temp424=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp309)->f1; _LL437: _temp436=( struct Cyc_List_List*) _temp424.tvars; goto
_LL435; _LL435: _temp434=( struct Cyc_Core_Opt*) _temp424.effect; goto _LL433;
_LL433: _temp432=( void*) _temp424.ret_typ; goto _LL431; _LL431: _temp430=(
struct Cyc_List_List*) _temp424.args; goto _LL429; _LL429: _temp428=( int)
_temp424.varargs; goto _LL427; _LL427: _temp426=( struct Cyc_List_List*)
_temp424.attributes; goto _LL338;} else{ goto _LL339;} _LL339: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_TupleType: 0){ _LL439: _temp438=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp309)->f1; goto
_LL340;} else{ goto _LL341;} _LL341: if(( unsigned int) _temp309 > 4u?*(( int*)
_temp309) == Cyc_Absyn_AnonStructType: 0){ _LL441: _temp440=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp309)->f1; goto _LL342;} else{ goto
_LL343;} _LL343: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_AnonUnionType:
0){ _LL443: _temp442=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp309)->f1; goto _LL344;} else{ goto _LL345;} _LL345: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_UnionType: 0){ _LL449: _temp448=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp309)->f1; goto
_LL447; _LL447: _temp446=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp309)->f2; goto _LL445; _LL445: _temp444=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp309)->f3; goto _LL346;} else{ goto
_LL347;} _LL347: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_StructType:
0){ _LL455: _temp454=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp309)->f1; goto _LL453; _LL453: _temp452=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp309)->f2; goto _LL451; _LL451: _temp450=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp309)->f3;
goto _LL348;} else{ goto _LL349;} _LL349: if(( unsigned int) _temp309 > 4u?*((
int*) _temp309) == Cyc_Absyn_EnumType: 0){ _LL459: _temp458=( struct _tuple0*)((
struct Cyc_Absyn_EnumType_struct*) _temp309)->f1; goto _LL457; _LL457: _temp456=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp309)->f2;
goto _LL350;} else{ goto _LL351;} _LL351: if(( unsigned int) _temp309 > 4u?*((
int*) _temp309) == Cyc_Absyn_TypedefType: 0){ _LL465: _temp464=( struct _tuple0*)((
struct Cyc_Absyn_TypedefType_struct*) _temp309)->f1; goto _LL463; _LL463:
_temp462=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp309)->f2; goto _LL461; _LL461: _temp460=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp309)->f3; goto _LL352;} else{ goto _LL353;}
_LL353: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_RgnHandleType:
0){ _LL467: _temp466=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp309)->f1;
goto _LL354;} else{ goto _LL355;} _LL355: if( _temp309 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL356;} else{ goto _LL357;} _LL357: if(( unsigned int) _temp309 > 4u?*((
int*) _temp309) == Cyc_Absyn_AccessEff: 0){ _LL469: _temp468=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp309)->f1; goto _LL358;} else{ goto _LL359;}
_LL359: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_JoinEff:
0){ _LL471: _temp470=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp309)->f1; goto _LL360;} else{ goto _LL310;} _LL312: return t; _LL314:
return( void*) Cyc_Absyn_VoidType; _LL316: return Cyc_Toc_typ_to_c( _temp372);
_LL318: return Cyc_Absyn_void_star_typ(); _LL320: return Cyc_Absyn_void_star_typ();
_LL322:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp472=( char*)"unresolved TunionType"; struct
_tagged_string _temp473; _temp473.curr= _temp472; _temp473.base= _temp472;
_temp473.last_plus_one= _temp472 + 22; _temp473;}))); _LL324: if( _temp396->typs
== 0){ if( _temp398->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp396->name,(
struct _tagged_string)({ char* _temp474=( char*)"_struct"; struct _tagged_string
_temp475; _temp475.curr= _temp474; _temp475.base= _temp474; _temp475.last_plus_one=
_temp474 + 8; _temp475;})));} _LL326:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp476=(
char*)"unresolved TunionFieldType"; struct _tagged_string _temp477; _temp477.curr=
_temp476; _temp477.base= _temp476; _temp477.last_plus_one= _temp476 + 27;
_temp477;}))); _LL328: _temp412= Cyc_Toc_typ_to_c_array( _temp412);{ void*
_temp478=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp404))->v; void* _temp484; _LL480: if((
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
_temp488= 0; goto _LL489; _LL489: for( 0; _temp426 != 0; _temp426=(( struct Cyc_List_List*)
_check_null( _temp426))->tl){ void* _temp490=( void*)(( struct Cyc_List_List*)
_check_null( _temp426))->hd; _LL492: if( _temp490 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL493;} else{ goto _LL494;} _LL494: if( _temp490 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL495;} else{ goto _LL496;} _LL496: if( _temp490 ==( void*) Cyc_Absyn_Const_att){
goto _LL497;} else{ goto _LL498;} _LL498: goto _LL499; _LL493: goto _LL495;
_LL495: goto _LL497; _LL497: continue; _LL499: _temp488=({ struct Cyc_List_List*
_temp500=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp500->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( _temp426))->hd);
_temp500->tl= _temp488; _temp500;}); goto _LL491; _LL491:;} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp501=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp501[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp502; _temp502.tag= Cyc_Absyn_FnType; _temp502.f1=({ struct Cyc_Absyn_FnInfo
_temp503; _temp503.tvars= 0; _temp503.effect= 0; _temp503.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp432); _temp503.args=(( struct Cyc_List_List*(*)( struct _tuple1*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp430); _temp503.varargs= _temp428; _temp503.attributes= _temp488; _temp503;});
_temp502;}); _temp501;});} _LL340: _temp438=(( struct Cyc_List_List*(*)( struct
_tuple3*(* f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp438);{ struct _tagged_string* _temp504= Cyc_Toc_add_tuple_type( _temp438);
goto _LL505; _LL505: return Cyc_Absyn_strct( _temp504);} _LL342: return( void*)({
struct Cyc_Absyn_AnonStructType_struct* _temp506=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp506[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp507; _temp507.tag= Cyc_Absyn_AnonStructType;
_temp507.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp440); _temp507;}); _temp506;}); _LL344: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp508=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp508[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp509; _temp509.tag=
Cyc_Absyn_AnonUnionType; _temp509.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp442); _temp509;}); _temp508;}); _LL346: if( _temp448 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp510=( char*)"anonymous union"; struct _tagged_string _temp511; _temp511.curr=
_temp510; _temp511.base= _temp510; _temp511.last_plus_one= _temp510 + 16;
_temp511;}));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)(( struct _tuple0*)
_check_null( _temp448))); _LL348: if( _temp454 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp512=(
char*)"anonymous struct"; struct _tagged_string _temp513; _temp513.curr=
_temp512; _temp513.base= _temp512; _temp513.last_plus_one= _temp512 + 17;
_temp513;}));} return Cyc_Absyn_strctq(( struct _tuple0*)(( struct _tuple0*)
_check_null( _temp454))); _LL350: return t; _LL352: if( _temp460 == 0){ return((
void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp514=( char*)"Toc::typ_to_c: unresolved TypedefType"; struct
_tagged_string _temp515; _temp515.curr= _temp514; _temp515.base= _temp514;
_temp515.last_plus_one= _temp514 + 38; _temp515;}));} return( void*)({ struct
Cyc_Absyn_TypedefType_struct* _temp516=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp516[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp517; _temp517.tag= Cyc_Absyn_TypedefType;
_temp517.f1= _temp464; _temp517.f2= 0; _temp517.f3=({ struct Cyc_Core_Opt*
_temp518=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp518->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp460))->v); _temp518;}); _temp517;}); _temp516;}); _LL354:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL356: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp519=( char*)"Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp520; _temp520.curr= _temp519; _temp520.base= _temp519;
_temp520.last_plus_one= _temp519 + 55; _temp520;})); _LL358: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp521=( char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp522; _temp522.curr= _temp521; _temp522.base= _temp521;
_temp522.last_plus_one= _temp521 + 54; _temp522;})); _LL360: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp523=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 52; _temp524;})); _LL310:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp525= t; struct Cyc_Absyn_Exp* _temp531; struct Cyc_Absyn_Tqual
_temp533; void* _temp535; _LL527: if(( unsigned int) _temp525 > 4u?*(( int*)
_temp525) == Cyc_Absyn_ArrayType: 0){ _LL536: _temp535=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp525)->f1; goto _LL534; _LL534: _temp533=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp525)->f2; goto _LL532; _LL532: _temp531=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp525)->f3; goto
_LL528;} else{ goto _LL529;} _LL529: goto _LL530; _LL528: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp535,( void*) Cyc_Absyn_HeapRgn, _temp533), e, l);
_LL530: return Cyc_Absyn_cast_exp( t, e, l); _LL526:;} static int Cyc_Toc_atomic_typ(
void* t){ void* _temp537= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp573; void* _temp575; void* _temp577; struct Cyc_Absyn_Exp* _temp579; struct
Cyc_Absyn_Tqual _temp581; void* _temp583; struct Cyc_Absyn_FnInfo _temp585;
struct Cyc_List_List* _temp587; struct Cyc_Absyn_Structdecl** _temp589; struct
Cyc_List_List* _temp591; struct _tuple0* _temp593; struct Cyc_Absyn_TunionInfo
_temp595; struct Cyc_List_List* _temp597; struct Cyc_List_List* _temp599; struct
Cyc_Absyn_TunionFieldInfo _temp601; struct Cyc_List_List* _temp603; void*
_temp605; struct Cyc_Absyn_Tunionfield* _temp607; struct Cyc_Absyn_Tuniondecl*
_temp609; struct Cyc_Absyn_Uniondecl** _temp611; struct Cyc_List_List* _temp613;
struct _tuple0* _temp615; struct Cyc_Absyn_PtrInfo _temp617; void* _temp619;
_LL539: if( _temp537 ==( void*) Cyc_Absyn_VoidType){ goto _LL540;} else{ goto
_LL541;} _LL541: if(( unsigned int) _temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_VarType:
0){ _LL574: _temp573=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp537)->f1; goto _LL542;} else{ goto _LL543;} _LL543: if(( unsigned int)
_temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_IntType: 0){ _LL578: _temp577=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp537)->f1; goto _LL576; _LL576:
_temp575=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp537)->f2; goto _LL544;}
else{ goto _LL545;} _LL545: if( _temp537 ==( void*) Cyc_Absyn_FloatType){ goto
_LL546;} else{ goto _LL547;} _LL547: if( _temp537 ==( void*) Cyc_Absyn_DoubleType){
goto _LL548;} else{ goto _LL549;} _LL549: if(( unsigned int) _temp537 > 4u?*((
int*) _temp537) == Cyc_Absyn_ArrayType: 0){ _LL584: _temp583=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp537)->f1; goto _LL582; _LL582: _temp581=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp537)->f2;
goto _LL580; _LL580: _temp579=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp537)->f3; goto _LL550;} else{ goto _LL551;} _LL551: if(( unsigned int)
_temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_FnType: 0){ _LL586: _temp585=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp537)->f1; goto
_LL552;} else{ goto _LL553;} _LL553: if(( unsigned int) _temp537 > 4u?*(( int*)
_temp537) == Cyc_Absyn_TupleType: 0){ _LL588: _temp587=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp537)->f1; goto _LL554;} else{ goto
_LL555;} _LL555: if(( unsigned int) _temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_StructType:
0){ _LL594: _temp593=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp537)->f1; goto _LL592; _LL592: _temp591=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp537)->f2; goto _LL590; _LL590: _temp589=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp537)->f3;
goto _LL556;} else{ goto _LL557;} _LL557: if(( unsigned int) _temp537 > 4u?*((
int*) _temp537) == Cyc_Absyn_TunionType: 0){ _LL596: _temp595=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp537)->f1; goto _LL558;} else{ goto
_LL559;} _LL559: if(( unsigned int) _temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_AnonStructType:
0){ _LL598: _temp597=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp537)->f1; goto _LL560;} else{ goto _LL561;} _LL561: if(( unsigned int)
_temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_AnonUnionType: 0){ _LL600:
_temp599=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp537)->f1; goto _LL562;} else{ goto _LL563;} _LL563: if(( unsigned int)
_temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_TunionFieldType: 0){ _LL602:
_temp601=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp537)->f1; _LL606: _temp605=( void*) _temp601.field_info; if(*(( int*)
_temp605) == Cyc_Absyn_KnownTunionfield){ _LL610: _temp609=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp605)->f1; goto _LL608; _LL608:
_temp607=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp605)->f2; goto _LL604;} else{ goto _LL565;} _LL604: _temp603=( struct Cyc_List_List*)
_temp601.targs; goto _LL564;} else{ goto _LL565;} _LL565: if(( unsigned int)
_temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_UnionType: 0){ _LL616: _temp615=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp537)->f1; goto
_LL614; _LL614: _temp613=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp537)->f2; goto _LL612; _LL612: _temp611=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp537)->f3; goto _LL566;} else{ goto
_LL567;} _LL567: if(( unsigned int) _temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_PointerType:
0){ _LL618: _temp617=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp537)->f1; goto _LL568;} else{ goto _LL569;} _LL569: if(( unsigned int)
_temp537 > 4u?*(( int*) _temp537) == Cyc_Absyn_RgnHandleType: 0){ _LL620:
_temp619=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp537)->f1; goto
_LL570;} else{ goto _LL571;} _LL571: goto _LL572; _LL540: return 1; _LL542:
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
Cyc_Absyn_PointerType: 0){ _LL629: _temp628=( struct Cyc_Absyn_PtrInfo)(( struct
Cyc_Absyn_PointerType_struct*) _temp622)->f1; _LL639: _temp638=( void*) _temp628.elt_typ;
goto _LL637; _LL637: _temp636=( void*) _temp628.rgn_typ; goto _LL635; _LL635:
_temp634=( struct Cyc_Absyn_Conref*) _temp628.nullable; goto _LL633; _LL633:
_temp632=( struct Cyc_Absyn_Tqual) _temp628.tq; goto _LL631; _LL631: _temp630=(
struct Cyc_Absyn_Conref*) _temp628.bounds; goto _LL625;} else{ goto _LL626;}
_LL626: goto _LL627; _LL625: { void* _temp640= Cyc_Tcutil_compress( _temp638);
_LL642: if( _temp640 ==( void*) Cyc_Absyn_VoidType){ goto _LL643;} else{ goto
_LL644;} _LL644: goto _LL645; _LL643: return 1; _LL645: return 0; _LL641:;}
_LL627: return 0; _LL623:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_string* f){ void* _temp646= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp662; struct Cyc_List_List* _temp664; struct Cyc_Absyn_Structdecl** _temp666;
struct Cyc_Absyn_Structdecl* _temp668; struct Cyc_List_List* _temp669; struct
_tuple0* _temp671; struct Cyc_Absyn_Structdecl** _temp673; struct Cyc_List_List*
_temp675; struct _tuple0* _temp677; struct Cyc_Absyn_Uniondecl** _temp679;
struct Cyc_Absyn_Uniondecl* _temp681; struct Cyc_List_List* _temp682; struct
_tuple0* _temp684; struct Cyc_Absyn_Uniondecl** _temp686; struct Cyc_List_List*
_temp688; struct _tuple0* _temp690; _LL648: if(( unsigned int) _temp646 > 4u?*((
int*) _temp646) == Cyc_Absyn_AnonUnionType: 0){ _LL663: _temp662=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp646)->f1; goto _LL649;} else{ goto
_LL650;} _LL650: if(( unsigned int) _temp646 > 4u?*(( int*) _temp646) == Cyc_Absyn_AnonStructType:
0){ _LL665: _temp664=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp646)->f1; goto _LL651;} else{ goto _LL652;} _LL652: if(( unsigned int)
_temp646 > 4u?*(( int*) _temp646) == Cyc_Absyn_StructType: 0){ _LL672: _temp671=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp646)->f1; goto
_LL670; _LL670: _temp669=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp646)->f2; goto _LL667; _LL667: _temp666=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp646)->f3; if( _temp666 == 0){ goto
_LL654;} else{ _temp668=* _temp666; goto _LL653;}} else{ goto _LL654;} _LL654:
if(( unsigned int) _temp646 > 4u?*(( int*) _temp646) == Cyc_Absyn_StructType: 0){
_LL678: _temp677=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp646)->f1; goto _LL676; _LL676: _temp675=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp646)->f2; goto _LL674; _LL674: _temp673=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp646)->f3;
if( _temp673 == 0){ goto _LL655;} else{ goto _LL656;}} else{ goto _LL656;}
_LL656: if(( unsigned int) _temp646 > 4u?*(( int*) _temp646) == Cyc_Absyn_UnionType:
0){ _LL685: _temp684=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp646)->f1; goto _LL683; _LL683: _temp682=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp646)->f2; goto _LL680; _LL680: _temp679=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp646)->f3;
if( _temp679 == 0){ goto _LL658;} else{ _temp681=* _temp679; goto _LL657;}}
else{ goto _LL658;} _LL658: if(( unsigned int) _temp646 > 4u?*(( int*) _temp646)
== Cyc_Absyn_UnionType: 0){ _LL691: _temp690=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp646)->f1; goto _LL689; _LL689: _temp688=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp646)->f2; goto _LL687; _LL687: _temp686=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp646)->f3;
if( _temp686 == 0){ goto _LL659;} else{ goto _LL660;}} else{ goto _LL660;}
_LL660: goto _LL661; _LL649: _temp664= _temp662; goto _LL651; _LL651: { struct
Cyc_Absyn_Structfield* _temp692= Cyc_Absyn_lookup_field( _temp664, f); goto
_LL693; _LL693: if( _temp692 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp694=*(( struct Cyc_Absyn_Structfield*) _check_null(
_temp692))->name; xprintf("is_poly_field: bad field %.*s", _temp694.last_plus_one
- _temp694.curr, _temp694.curr);}));} return Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp692))->type);} _LL653: if(
_temp671 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp695=( char*)"StructType name not properly set";
struct _tagged_string _temp696; _temp696.curr= _temp695; _temp696.base= _temp695;
_temp696.last_plus_one= _temp695 + 33; _temp696;}));} if( _temp668->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp697= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct _tuple0*)
_check_null( _temp671))); xprintf("is_poly_field:  struct %.*s missing fields",
_temp697.last_plus_one - _temp697.curr, _temp697.curr);}));}{ struct Cyc_List_List*
_temp698=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp668->fields))->v;
goto _LL699; _LL699: for( 0; _temp698 != 0; _temp698=(( struct Cyc_List_List*)
_check_null( _temp698))->tl){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp698))->hd)->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp698))->hd)->type);}}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp700= Cyc_Absynpp_qvar2string(( struct _tuple0*)((
struct _tuple0*) _check_null( _temp671))); struct _tagged_string _temp701=* f;
xprintf("is_poly_field:  struct %.*s has no %.*s field", _temp700.last_plus_one
- _temp700.curr, _temp700.curr, _temp701.last_plus_one - _temp701.curr, _temp701.curr);}));
return 0;} _LL655: if( _temp677 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp702=( char*)"StructType name not properly set";
struct _tagged_string _temp703; _temp703.curr= _temp702; _temp703.base= _temp702;
_temp703.last_plus_one= _temp702 + 33; _temp703;}));}(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp704= Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct _tuple0*) _check_null( _temp677))); xprintf("is_poly_field:  struct %.*s missing structdecl",
_temp704.last_plus_one - _temp704.curr, _temp704.curr);})); return 0; _LL657:
if( _temp684 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp705=( char*)"UnionType name not properly set";
struct _tagged_string _temp706; _temp706.curr= _temp705; _temp706.base= _temp705;
_temp706.last_plus_one= _temp705 + 32; _temp706;}));} if( _temp681->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp707= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct _tuple0*)
_check_null( _temp684))); xprintf("is_poly_field:  union %.*s missing fields",
_temp707.last_plus_one - _temp707.curr, _temp707.curr);}));}{ struct Cyc_List_List*
_temp708=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp681->fields))->v;
goto _LL709; _LL709: for( 0; _temp708 != 0; _temp708=(( struct Cyc_List_List*)
_check_null( _temp708))->tl){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp708))->hd)->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp708))->hd)->type);}}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp710= Cyc_Absynpp_qvar2string(( struct _tuple0*)((
struct _tuple0*) _check_null( _temp684))); struct _tagged_string _temp711=* f;
xprintf("is_poly_field:  union %.*s has no %.*s field", _temp710.last_plus_one -
_temp710.curr, _temp710.curr, _temp711.last_plus_one - _temp711.curr, _temp711.curr);}));
return 0;} _LL659: if( _temp690 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp712=( char*)"UnionType name not properly set";
struct _tagged_string _temp713; _temp713.curr= _temp712; _temp713.base= _temp712;
_temp713.last_plus_one= _temp712 + 32; _temp713;}));}(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp714= Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct _tuple0*) _check_null( _temp690))); xprintf("is_poly_field:  union %.*s missing structdecl",
_temp714.last_plus_one - _temp714.curr, _temp714.curr);})); return 0; _LL661:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp715= Cyc_Absynpp_typ2string( t); xprintf("is_poly_field:  bad type %.*s",
_temp715.last_plus_one - _temp715.curr, _temp715.curr);})); return 0; _LL647:;}
static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void* _temp716=(
void*) e->r; struct _tagged_string* _temp724; struct Cyc_Absyn_Exp* _temp726;
struct _tagged_string* _temp728; struct Cyc_Absyn_Exp* _temp730; _LL718: if(*((
int*) _temp716) == Cyc_Absyn_StructMember_e){ _LL727: _temp726=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp716)->f1; goto _LL725; _LL725:
_temp724=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp716)->f2; goto _LL719;} else{ goto _LL720;} _LL720: if(*(( int*) _temp716)
== Cyc_Absyn_StructArrow_e){ _LL731: _temp730=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp716)->f1; goto _LL729; _LL729: _temp728=(
struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp716)->f2;
goto _LL721;} else{ goto _LL722;} _LL722: goto _LL723; _LL719: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp726->topt))->v, _temp724);
_LL721: { void* _temp732= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp730->topt))->v); struct Cyc_Absyn_PtrInfo _temp738; struct Cyc_Absyn_Conref*
_temp740; struct Cyc_Absyn_Tqual _temp742; struct Cyc_Absyn_Conref* _temp744;
void* _temp746; void* _temp748; _LL734: if(( unsigned int) _temp732 > 4u?*(( int*)
_temp732) == Cyc_Absyn_PointerType: 0){ _LL739: _temp738=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp732)->f1; _LL749: _temp748=( void*)
_temp738.elt_typ; goto _LL747; _LL747: _temp746=( void*) _temp738.rgn_typ; goto
_LL745; _LL745: _temp744=( struct Cyc_Absyn_Conref*) _temp738.nullable; goto
_LL743; _LL743: _temp742=( struct Cyc_Absyn_Tqual) _temp738.tq; goto _LL741;
_LL741: _temp740=( struct Cyc_Absyn_Conref*) _temp738.bounds; goto _LL735;}
else{ goto _LL736;} _LL736: goto _LL737; _LL735: return Cyc_Toc_is_poly_field(
_temp748, _temp728); _LL737:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp750= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp730->topt))->v); xprintf("is_poly_project:  bad type %.*s",
_temp750.last_plus_one - _temp750.curr, _temp750.curr);})); return 0; _LL733:;}
_LL723: return 0; _LL717:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_qv,
0),({ struct Cyc_List_List* _temp751=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp751->hd=( void*) s; _temp751->tl= 0; _temp751;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_atomic_qv, 0),({
struct Cyc_List_List* _temp752=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp752->hd=( void*) s; _temp752->tl= 0; _temp752;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__region_malloc_qv,
0),({ struct Cyc_List_List* _temp753=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp753->hd=( void*) rgn; _temp753->tl=({ struct Cyc_List_List*
_temp754=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp754->hd=( void*) s; _temp754->tl= 0; _temp754;}); _temp753;}), 0);} static
const int Cyc_Toc_NullCheck= 0; struct Cyc_Toc_NullCheck_struct{ int tag; void*
f1; } ; static const int Cyc_Toc_UntagPtr= 1; struct Cyc_Toc_UntagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; int f3; } ; static const int Cyc_Toc_TagPtr=
2; struct Cyc_Toc_TagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; }
; typedef void* Cyc_Toc_conv; struct _tuple7{ void* f1; void* f2; } ; static
struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp756=({ struct _tuple7
_temp755; _temp755.f1= t1; _temp755.f2= t2; _temp755;}); void* _temp762; struct
Cyc_Absyn_PtrInfo _temp764; void* _temp766; struct Cyc_Absyn_PtrInfo _temp768;
_LL758: _LL767: _temp766= _temp756.f1; if(( unsigned int) _temp766 > 4u?*(( int*)
_temp766) == Cyc_Absyn_PointerType: 0){ _LL769: _temp768=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp766)->f1; goto _LL763;} else{ goto
_LL760;} _LL763: _temp762= _temp756.f2; if(( unsigned int) _temp762 > 4u?*(( int*)
_temp762) == Cyc_Absyn_PointerType: 0){ _LL765: _temp764=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp762)->f1; goto _LL759;} else{ goto
_LL760;} _LL760: goto _LL761; _LL759: { int _temp770=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp768.nullable); goto _LL771; _LL771: { int
_temp772=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp764.nullable);
goto _LL773; _LL773: { void* _temp774=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp768.bounds); goto _LL775; _LL775: { void* _temp776=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp764.bounds);
goto _LL777; _LL777: { struct Cyc_List_List* convs= 0;{ struct _tuple7 _temp779=({
struct _tuple7 _temp778; _temp778.f1= _temp774; _temp778.f2= _temp776; _temp778;});
void* _temp789; struct Cyc_Absyn_Exp* _temp791; void* _temp793; struct Cyc_Absyn_Exp*
_temp795; void* _temp797; void* _temp799; void* _temp801; void* _temp803; struct
Cyc_Absyn_Exp* _temp805; void* _temp807; struct Cyc_Absyn_Exp* _temp809; void*
_temp811; _LL781: _LL794: _temp793= _temp779.f1; if(( unsigned int) _temp793 > 1u?*((
int*) _temp793) == Cyc_Absyn_Upper_b: 0){ _LL796: _temp795=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp793)->f1; goto _LL790;} else{ goto _LL783;}
_LL790: _temp789= _temp779.f2; if(( unsigned int) _temp789 > 1u?*(( int*)
_temp789) == Cyc_Absyn_Upper_b: 0){ _LL792: _temp791=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp789)->f1; goto _LL782;} else{ goto _LL783;}
_LL783: _LL800: _temp799= _temp779.f1; if( _temp799 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL798;} else{ goto _LL785;} _LL798: _temp797= _temp779.f2; if( _temp797 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL784;} else{ goto _LL785;} _LL785: _LL804:
_temp803= _temp779.f1; if(( unsigned int) _temp803 > 1u?*(( int*) _temp803) ==
Cyc_Absyn_Upper_b: 0){ _LL806: _temp805=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp803)->f1; goto _LL802;} else{ goto _LL787;} _LL802: _temp801= _temp779.f2;
if( _temp801 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL786;} else{ goto _LL787;}
_LL787: _LL812: _temp811= _temp779.f1; if( _temp811 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL808;} else{ goto _LL780;} _LL808: _temp807= _temp779.f2; if((
unsigned int) _temp807 > 1u?*(( int*) _temp807) == Cyc_Absyn_Upper_b: 0){ _LL810:
_temp809=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp807)->f1;
goto _LL788;} else{ goto _LL780;} _LL782: if( _temp770? ! _temp772: 0){ convs=({
struct Cyc_List_List* _temp813=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp813->hd=( void*)(( void*)({ struct Cyc_Toc_NullCheck_struct*
_temp814=( struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp814[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp815; _temp815.tag= Cyc_Toc_NullCheck;
_temp815.f1=( void*) t2; _temp815;}); _temp814;})); _temp813->tl= convs;
_temp813;});} goto _LL780; _LL784: goto _LL780; _LL786: convs=({ struct Cyc_List_List*
_temp816=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp816->hd=( void*)(( void*)({ struct Cyc_Toc_TagPtr_struct* _temp817=( struct
Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp817[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp818; _temp818.tag= Cyc_Toc_TagPtr;
_temp818.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp818.f2= _temp805; _temp818;});
_temp817;})); _temp816->tl= convs; _temp816;}); goto _LL780; _LL788: convs=({
struct Cyc_List_List* _temp819=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp819->hd=( void*)(( void*)({ struct Cyc_Toc_UntagPtr_struct*
_temp820=( struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp820[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp821; _temp821.tag= Cyc_Toc_UntagPtr;
_temp821.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp821.f2= _temp809; _temp821.f3=
_temp772; _temp821;}); _temp820;})); _temp819->tl= convs; _temp819;}); goto
_LL780; _LL780:;} return convs;}}}}} _LL761: return 0; _LL757:;}} static struct
Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__throw_qv, 0),({ struct Cyc_List_List* _temp822=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp822->hd=(
void*) e; _temp822->tl= 0; _temp822;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv(
int toplevel, void* t, struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c(
t); if( toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp823=( char*)"can't do null-check conversion at top-level";
struct _tagged_string _temp824; _temp824.curr= _temp823; _temp824.base= _temp823;
_temp824.last_plus_one= _temp823 + 44; _temp824;}));} if( Cyc_Toc_use_runtimecheck_funs){
return Cyc_Absyn_cast_exp( new_typ, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp825=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp825->hd=( void*) e; _temp825->tl= 0; _temp825;}), 0),
0);} else{ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0),
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0),
Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
x, 0), 0), 0); return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, new_typ,(
struct Cyc_Absyn_Exp*) e, s, 0), 0);}} static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv(
int toplevel, void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){
struct _tuple0* a= Cyc_Toc_temp_var(); struct _tuple0* b= Cyc_Toc_temp_var();
int szvalue=( int) Cyc_Evexp_eval_const_uint_exp( sz); void* strt_typ; void*
elt_typ;{ void* _temp826= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp832; struct Cyc_Absyn_Conref* _temp834; struct Cyc_Absyn_Tqual _temp836;
struct Cyc_Absyn_Conref* _temp838; void* _temp840; void* _temp842; _LL828: if((
unsigned int) _temp826 > 4u?*(( int*) _temp826) == Cyc_Absyn_PointerType: 0){
_LL833: _temp832=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp826)->f1; _LL843: _temp842=( void*) _temp832.elt_typ; goto _LL841; _LL841:
_temp840=( void*) _temp832.rgn_typ; goto _LL839; _LL839: _temp838=( struct Cyc_Absyn_Conref*)
_temp832.nullable; goto _LL837; _LL837: _temp836=( struct Cyc_Absyn_Tqual)
_temp832.tq; goto _LL835; _LL835: _temp834=( struct Cyc_Absyn_Conref*) _temp832.bounds;
goto _LL829;} else{ goto _LL830;} _LL830: goto _LL831; _LL829: elt_typ= _temp842;
strt_typ= Cyc_Toc_add_tagged_ptr_type( _temp842, Cyc_Toc_mt_tq); goto _LL827;
_LL831: return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp844=( char*)"tag_ptr_conv: not a pointer";
struct _tagged_string _temp845; _temp845.curr= _temp844; _temp845.base= _temp844;
_temp845.last_plus_one= _temp844 + 28; _temp845;})); _LL827:;} if( toplevel){
int is_string= 0;{ void* _temp846=( void*) e->r; void* _temp852; struct
_tagged_string _temp854; _LL848: if(*(( int*) _temp846) == Cyc_Absyn_Const_e){
_LL853: _temp852=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp846)->f1; if((
unsigned int) _temp852 > 1u?*(( int*) _temp852) == Cyc_Absyn_String_c: 0){
_LL855: _temp854=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp852)->f1; goto _LL849;} else{ goto _LL850;}} else{ goto _LL850;} _LL850:
goto _LL851; _LL849: is_string= 1; goto _LL847; _LL851: goto _LL847; _LL847:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp859=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp859[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp860; _temp860.tag= Cyc_Absyn_ArrayType; _temp860.f1=(
void*) elt_typ; _temp860.f2= Cyc_Toc_mt_tq; _temp860.f3=( struct Cyc_Absyn_Exp*)
sz; _temp860;}); _temp859;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp856=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp856->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp857=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp857[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp858; _temp858.tag= Cyc_Absyn_Var_d;
_temp858.f1= vd; _temp858;}); _temp857;}), 0); _temp856->tl= Cyc_Toc_result_decls;
_temp856;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp861=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp861->hd=( void*)({ struct _tuple4* _temp866=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp866->f1= 0; _temp866->f2=
xexp; _temp866;}); _temp861->tl=({ struct Cyc_List_List* _temp862=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp862->hd=( void*)({ struct
_tuple4* _temp865=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp865->f1= 0; _temp865->f2= xexp; _temp865;}); _temp862->tl=({ struct Cyc_List_List*
_temp863=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp863->hd=( void*)({ struct _tuple4* _temp864=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp864->f1= 0; _temp864->f2= xplussz; _temp864;});
_temp863->tl= 0; _temp863;}); _temp862;}); _temp861;}), 0); return urm_exp;}}}
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
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp867=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp868; _temp868.curr= _temp867; _temp868.base= _temp867; _temp868.last_plus_one=
_temp867 + 45; _temp868;}));}{ struct _tuple0* x= Cyc_Toc_temp_var(); int
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
cs= Cyc_Toc_conversion( t1, t2); for( 0; cs != 0; cs=(( struct Cyc_List_List*)
_check_null( cs))->tl){ void* _temp869=( void*)(( struct Cyc_List_List*)
_check_null( cs))->hd; void* _temp877; struct Cyc_Absyn_Exp* _temp879; void*
_temp881; int _temp883; struct Cyc_Absyn_Exp* _temp885; void* _temp887; _LL871:
if(*(( int*) _temp869) == Cyc_Toc_NullCheck){ _LL878: _temp877=( void*)(( struct
Cyc_Toc_NullCheck_struct*) _temp869)->f1; goto _LL872;} else{ goto _LL873;}
_LL873: if(*(( int*) _temp869) == Cyc_Toc_TagPtr){ _LL882: _temp881=( void*)((
struct Cyc_Toc_TagPtr_struct*) _temp869)->f1; goto _LL880; _LL880: _temp879=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_TagPtr_struct*) _temp869)->f2; goto
_LL874;} else{ goto _LL875;} _LL875: if(*(( int*) _temp869) == Cyc_Toc_UntagPtr){
_LL888: _temp887=( void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp869)->f1; goto
_LL886; _LL886: _temp885=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_UntagPtr_struct*)
_temp869)->f2; goto _LL884; _LL884: _temp883=( int)(( struct Cyc_Toc_UntagPtr_struct*)
_temp869)->f3; goto _LL876;} else{ goto _LL870;} _LL872: e= Cyc_Toc_null_check_conv(
toplevel, _temp877, e); goto _LL870; _LL874: e= Cyc_Toc_tag_ptr_conv( toplevel,
_temp881, _temp879, e); goto _LL870; _LL876: e= Cyc_Toc_untag_ptr_conv( toplevel,
_temp887, _temp885, _temp883, e); goto _LL870; _LL870:;}} return e;} struct Cyc_Toc_Env{
struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt*
fallthru_info; struct Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct
Cyc_Toc_Env* Cyc_Toc_env_t; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){
return({ struct Cyc_Toc_Env* _temp889=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp889->break_lab= 0; _temp889->continue_lab= 0;
_temp889->fallthru_info= 0; _temp889->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp889->toplevel= 1; _temp889;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env(
struct Cyc_Toc_Env* e){ return({ struct Cyc_Toc_Env* _temp890=( struct Cyc_Toc_Env*)
GC_malloc( sizeof( struct Cyc_Toc_Env)); _temp890->break_lab= e->break_lab;
_temp890->continue_lab= e->continue_lab; _temp890->fallthru_info= e->fallthru_info;
_temp890->varmap= e->varmap; _temp890->toplevel= e->toplevel; _temp890;});}
static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env* e, struct
_tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp891=(* x).f1; struct Cyc_List_List*
_temp897; _LL893: if(( unsigned int) _temp891 > 1u?*(( int*) _temp891) == Cyc_Absyn_Rel_n:
0){ _LL898: _temp897=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp891)->f1; goto _LL894;} else{ goto _LL895;} _LL895: goto _LL896; _LL894:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp899=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp899[ 0]=({ struct
Cyc_Core_Impossible_struct _temp900; _temp900.tag= Cyc_Core_Impossible; _temp900.f1=({
struct _tagged_string _temp901= Cyc_Absynpp_qvar2string( x); xprintf("Toc::add_varmap on Rel_n: %.*s\n",
_temp901.last_plus_one - _temp901.curr, _temp901.curr);}); _temp900;}); _temp899;}));
_LL896: goto _LL892; _LL892:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap, x, y); return
ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple8{ struct _tagged_string* f1; struct Cyc_List_List*
f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l, struct _tagged_string*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp902= 0; goto _LL903; _LL903: for( 0;
fallthru_binders != 0; fallthru_binders=(( struct Cyc_List_List*) _check_null(
fallthru_binders))->tl){ _temp902=({ struct Cyc_List_List* _temp904=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp904->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp904->tl=
_temp902; _temp904;});} _temp902=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp902);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp905=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp905->v=( void*) break_l; _temp905;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp906=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp906->v=( void*)({ struct _tuple8*
_temp907=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp907->f1=
fallthru_l; _temp907->f2= _temp902; _temp907->f3= next_case_env->varmap;
_temp907;}); _temp906;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp908=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp908->v=( void*)
break_l; _temp908;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_string*
next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp909=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp909->v=( void*)({ struct _tuple8* _temp910=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp910->f1= next_l; _temp910->f2= 0;
_temp910->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp910;}); _temp909;});
return ans;} struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; char f5; int f6; } ; struct
_tuple10{ struct _tuple0* f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; static
struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c( void* p, struct Cyc_Core_Opt* aopt,
struct Cyc_List_List* args, struct Cyc_List_List* typs){ if( args == 0){ return((
struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp911=( char*)"expecting a literal format string";
struct _tagged_string _temp912; _temp912.curr= _temp911; _temp912.base= _temp911;
_temp912.last_plus_one= _temp911 + 34; _temp912;}));}{ struct _tagged_string
fmt_str;{ void* _temp913=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd)->r; void* _temp919; struct _tagged_string _temp921;
_LL915: if(*(( int*) _temp913) == Cyc_Absyn_Const_e){ _LL920: _temp919=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp913)->f1; if(( unsigned int) _temp919 > 1u?*((
int*) _temp919) == Cyc_Absyn_String_c: 0){ _LL922: _temp921=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp919)->f1; goto _LL916;}
else{ goto _LL917;}} else{ goto _LL917;} _LL917: goto _LL918; _LL916: fmt_str=
_temp921; goto _LL914; _LL918: return(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp923=(
char*)"expecting a literal format string"; struct _tagged_string _temp924;
_temp924.curr= _temp923; _temp924.base= _temp923; _temp924.last_plus_one=
_temp923 + 34; _temp924;})); _LL914:;}{ int len=( int)({ struct _tagged_string
_temp1026= fmt_str;( unsigned int)( _temp1026.last_plus_one - _temp1026.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str, sizeof( char),
i) =='%'){ i ++; if( i < len?*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( fmt_str, sizeof( char), i) =='s': 0){
break;}}} if( i == len){ if( aopt != 0){ args=({ struct Cyc_List_List* _temp925=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp925->hd=(
void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( aopt))->v);
_temp925->tl= args; _temp925;});} return Cyc_Absyn_primop_exp( p, args, 0);}
args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl;{ struct Cyc_List_List* rev_fmt= 0; struct Cyc_List_List*
rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i= 0; i < len; i ++){
char c=*(( char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( fmt_str, sizeof( char), i); rev_fmt=({ struct Cyc_List_List*
_temp926=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp926->hd=( void*)(( int) c); _temp926->tl= rev_fmt; _temp926;}); if( c !='%'){
continue;}{ struct Cyc_Core_Opt* _temp927= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); goto _LL928; _LL928: if( _temp927 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp929=(
char*)"bad format string"; struct _tagged_string _temp930; _temp930.curr=
_temp929; _temp930.base= _temp929; _temp930.last_plus_one= _temp929 + 18;
_temp930;}));}{ struct _tuple9 _temp933; int _temp934; char _temp936; struct Cyc_List_List*
_temp938; struct Cyc_List_List* _temp940; struct Cyc_List_List* _temp942; struct
Cyc_List_List* _temp944; struct _tuple9* _temp931=( struct _tuple9*)(( struct
Cyc_Core_Opt*) _check_null( _temp927))->v; _temp933=* _temp931; _LL945: _temp944=
_temp933.f1; goto _LL943; _LL943: _temp942= _temp933.f2; goto _LL941; _LL941:
_temp940= _temp933.f3; goto _LL939; _LL939: _temp938= _temp933.f4; goto _LL937;
_LL937: _temp936= _temp933.f5; goto _LL935; _LL935: _temp934= _temp933.f6; goto
_LL932; _LL932: i= _temp934 - 1; if( _temp936 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct Cyc_List_List*
_temp946=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp946->hd=( void*) _temp944; _temp946->tl=({ struct Cyc_List_List* _temp947=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp947->hd=(
void*) _temp942; _temp947->tl=({ struct Cyc_List_List* _temp948=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp948->hd=( void*) _temp940;
_temp948->tl=({ struct Cyc_List_List* _temp949=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp949->hd=( void*) _temp938;
_temp949->tl= 0; _temp949;}); _temp948;}); _temp947;}); _temp946;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp950=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp950->hd=( void*)(( int) _temp936); _temp950->tl=
rev_fmt; _temp950;});{ struct Cyc_List_List* _temp951= _temp942; struct Cyc_List_List
_temp958; struct Cyc_List_List* _temp959; int _temp961; _LL953: if( _temp951 ==
0){ goto _LL955;} else{ _temp958=* _temp951; _LL962: _temp961=( int) _temp958.hd;
goto _LL960; _LL960: _temp959=( struct Cyc_List_List*) _temp958.tl; if( _temp959
== 0){ goto _LL957;} else{ goto _LL955;}} _LL957: if( _temp961 =='*'){ goto
_LL954;} else{ goto _LL955;} _LL955: goto _LL956; _LL954: { struct _tuple0*
_temp963= Cyc_Toc_temp_var(); goto _LL964; _LL964: rev_temps=({ struct Cyc_List_List*
_temp965=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp965->hd=( void*)({ struct _tuple10* _temp966=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp966->f1= _temp963; _temp966->f2=( void*)((
struct Cyc_List_List*) _check_null( typs))->hd; _temp966->f3=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd; _temp966;}); _temp965->tl=
rev_temps; _temp965;}); rev_result=({ struct Cyc_List_List* _temp967=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp967->hd=( void*)
Cyc_Absyn_var_exp( _temp963, 0); _temp967->tl= rev_result; _temp967;}); args=((
struct Cyc_List_List*) _check_null( args))->tl; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl; goto _LL952;} _LL956: goto _LL952; _LL952:;}{ struct
Cyc_List_List* _temp968= _temp940; struct Cyc_List_List _temp975; struct Cyc_List_List*
_temp976; struct Cyc_List_List _temp978; struct Cyc_List_List* _temp979; int
_temp981; int _temp983; _LL970: if( _temp968 == 0){ goto _LL972;} else{ _temp975=*
_temp968; _LL984: _temp983=( int) _temp975.hd; goto _LL977; _LL977: _temp976=(
struct Cyc_List_List*) _temp975.tl; if( _temp976 == 0){ goto _LL972;} else{
_temp978=* _temp976; _LL982: _temp981=( int) _temp978.hd; goto _LL980; _LL980:
_temp979=( struct Cyc_List_List*) _temp978.tl; if( _temp979 == 0){ goto _LL974;}
else{ goto _LL972;}}} _LL974: if( _temp983 =='.'? _temp981 =='*': 0){ goto
_LL971;} else{ goto _LL972;} _LL972: goto _LL973; _LL971: { struct _tuple0*
_temp985= Cyc_Toc_temp_var(); goto _LL986; _LL986: rev_temps=({ struct Cyc_List_List*
_temp987=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp987->hd=( void*)({ struct _tuple10* _temp988=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp988->f1= _temp985; _temp988->f2=( void*)((
struct Cyc_List_List*) _check_null( typs))->hd; _temp988->f3=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd; _temp988;}); _temp987->tl=
rev_temps; _temp987;}); rev_result=({ struct Cyc_List_List* _temp989=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp989->hd=( void*)
Cyc_Absyn_var_exp( _temp985, 0); _temp989->tl= rev_result; _temp989;}); args=((
struct Cyc_List_List*) _check_null( args))->tl; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl; goto _LL969;} _LL973: goto _LL969; _LL969:;} if(
_temp936 !='%'){ if( args == 0? 1: typs == 0){(( void(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp990= Cyc_Absynpp_prim2string(
p); xprintf("missing arg to %.*s", _temp990.last_plus_one - _temp990.curr,
_temp990.curr);}));}{ struct _tuple0* _temp991= Cyc_Toc_temp_var(); goto _LL992;
_LL992: rev_temps=({ struct Cyc_List_List* _temp993=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp993->hd=( void*)({ struct
_tuple10* _temp994=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp994->f1= _temp991; _temp994->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp994->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp994;}); _temp993->tl= rev_temps; _temp993;});
rev_result=({ struct Cyc_List_List* _temp995=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp995->hd=( void*) Cyc_Absyn_var_exp(
_temp991, 0); _temp995->tl= rev_result; _temp995;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp996=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp996->hd=( void*)(( int)'.');
_temp996->tl= rev_fmt; _temp996;}); rev_fmt=({ struct Cyc_List_List* _temp997=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp997->hd=(
void*)(( int)'*'); _temp997->tl= rev_fmt; _temp997;}); rev_fmt=({ struct Cyc_List_List*
_temp998=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp998->hd=( void*)(( int)'s'); _temp998->tl= rev_fmt; _temp998;});{ struct
_tuple0* _temp999= Cyc_Toc_temp_var(); goto _LL1000; _LL1000: rev_temps=({
struct Cyc_List_List* _temp1001=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1001->hd=( void*)({ struct _tuple10* _temp1002=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1002->f1= _temp999;
_temp1002->f2=( void*)(( struct Cyc_List_List*) _check_null( typs))->hd;
_temp1002->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
args))->hd; _temp1002;}); _temp1001->tl= rev_temps; _temp1001;});{ struct Cyc_Absyn_Exp*
_temp1003= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp999, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp999, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL1004; _LL1004: { struct Cyc_Absyn_Exp*
_temp1005= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( _temp999, 0), Cyc_Toc_curr_sp,
0); goto _LL1006; _LL1006: rev_result=({ struct Cyc_List_List* _temp1007=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1007->hd=(
void*) _temp1003; _temp1007->tl= rev_result; _temp1007;}); rev_result=({ struct
Cyc_List_List* _temp1008=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1008->hd=( void*) _temp1005; _temp1008->tl= rev_result; _temp1008;}); args=((
struct Cyc_List_List*) _check_null( args))->tl; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl;}}}}}}}{ struct Cyc_List_List* _temp1010=({ struct Cyc_List_List*
_temp1009=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1009->hd=( void*) Cyc_Absyn_string_exp( Cyc_String_implode((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( rev_fmt)), 0); _temp1009->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
rev_result); _temp1009;}); goto _LL1011; _LL1011: if( aopt != 0){ _temp1010=({
struct Cyc_List_List* _temp1012=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1012->hd=( void*)(( struct Cyc_Absyn_Exp*)(( struct
Cyc_Core_Opt*) _check_null( aopt))->v); _temp1012->tl= _temp1010; _temp1012;});}{
struct Cyc_Absyn_Exp* _temp1013= Cyc_Absyn_primop_exp( p, _temp1010, 0); goto
_LL1014; _LL1014: { struct Cyc_Absyn_Stmt* _temp1015= Cyc_Absyn_exp_stmt(
_temp1013, 0); goto _LL1016; _LL1016: for( 0; rev_temps != 0; rev_temps=((
struct Cyc_List_List*) _check_null( rev_temps))->tl){ struct _tuple10 _temp1019;
struct Cyc_Absyn_Exp* _temp1020; void* _temp1022; struct _tuple0* _temp1024;
struct _tuple10* _temp1017=( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd; _temp1019=* _temp1017; _LL1025: _temp1024=
_temp1019.f1; goto _LL1023; _LL1023: _temp1022= _temp1019.f2; goto _LL1021;
_LL1021: _temp1020= _temp1019.f3; goto _LL1018; _LL1018: _temp1015= Cyc_Absyn_declare_stmt(
_temp1024, _temp1022,( struct Cyc_Absyn_Exp*) _temp1020, _temp1015, 0);} return
Cyc_Absyn_stmt_exp( _temp1015, 0);}}}}}}} static void* Cyc_Toc_get_c_typ( struct
Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1027=( char*)"Missing type in primop ";
struct _tagged_string _temp1028; _temp1028.curr= _temp1027; _temp1028.base=
_temp1027; _temp1028.last_plus_one= _temp1027 + 24; _temp1028;}));} return Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1029=( char*)"Missing type in primop ";
struct _tagged_string _temp1030; _temp1030.curr= _temp1029; _temp1030.base=
_temp1029; _temp1030.last_plus_one= _temp1029 + 24; _temp1030;}));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1031=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1031->f1= Cyc_Toc_mt_tq; _temp1031->f2=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_temp1031;});} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4*
_temp1032=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1032[
0]=({ struct _tuple4 _temp1033; _temp1033.f1= 0; _temp1033.f2= e; _temp1033;});
_temp1032;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
_temp1034= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); goto _LL1035; _LL1035: if(
rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic(
_temp1034): Cyc_Toc_malloc_ptr( _temp1034), 0);} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( rgnopt)); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp1034), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp1036=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); goto _LL1037; _LL1037: for( 0;
_temp1036 != 0; _temp1036=(( struct Cyc_List_List*) _check_null( _temp1036))->tl){
struct _tuple4 _temp1040; struct Cyc_Absyn_Exp* _temp1041; struct Cyc_List_List*
_temp1043; struct _tuple4* _temp1038=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1036))->hd; _temp1040=* _temp1038; _LL1044: _temp1043=
_temp1040.f1; goto _LL1042; _LL1042: _temp1041= _temp1040.f2; goto _LL1039;
_LL1039: { struct Cyc_Absyn_Exp* e_index; if( _temp1043 == 0){ e_index= Cyc_Absyn_signed_int_exp(
count --, 0);} else{ if((( struct Cyc_List_List*) _check_null( _temp1043))->tl
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp1045=( char*)"multiple designators in array";
struct _tagged_string _temp1046; _temp1046.curr= _temp1045; _temp1046.base=
_temp1045; _temp1046.last_plus_one= _temp1045 + 30; _temp1046;}));}{ void*
_temp1047=( void*)(( struct Cyc_List_List*) _check_null( _temp1043))->hd; goto
_LL1048; _LL1048: { void* _temp1049= _temp1047; struct Cyc_Absyn_Exp* _temp1055;
struct _tagged_string* _temp1057; _LL1051: if(*(( int*) _temp1049) == Cyc_Absyn_ArrayElement){
_LL1056: _temp1055=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1049)->f1; goto _LL1052;} else{ goto _LL1053;} _LL1053: if(*(( int*)
_temp1049) == Cyc_Absyn_FieldName){ _LL1058: _temp1057=( struct _tagged_string*)((
struct Cyc_Absyn_FieldName_struct*) _temp1049)->f1; goto _LL1054;} else{ goto
_LL1050;} _LL1052: Cyc_Toc_exp_to_c( nv, _temp1055); e_index= _temp1055; goto
_LL1050; _LL1054: e_index=(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1059=( char*)"field name designators in array";
struct _tagged_string _temp1060; _temp1060.curr= _temp1059; _temp1060.base=
_temp1059; _temp1060.last_plus_one= _temp1059 + 32; _temp1060;})); goto _LL1050;
_LL1050:;}}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index,
0); void* _temp1061=( void*) _temp1041->r; struct Cyc_List_List* _temp1071;
struct Cyc_Absyn_Exp* _temp1073; struct Cyc_Absyn_Exp* _temp1075; struct Cyc_Absyn_Vardecl*
_temp1077; struct Cyc_List_List* _temp1079; void* _temp1081; _LL1063: if(*(( int*)
_temp1061) == Cyc_Absyn_Array_e){ _LL1072: _temp1071=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1061)->f1; goto _LL1064;} else{ goto
_LL1065;} _LL1065: if(*(( int*) _temp1061) == Cyc_Absyn_Comprehension_e){
_LL1078: _temp1077=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1061)->f1; goto _LL1076; _LL1076: _temp1075=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1061)->f2; goto _LL1074; _LL1074:
_temp1073=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1061)->f3; goto _LL1066;} else{ goto _LL1067;} _LL1067: if(*(( int*)
_temp1061) == Cyc_Absyn_AnonStruct_e){ _LL1082: _temp1081=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1061)->f1; goto _LL1080; _LL1080: _temp1079=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1061)->f2; goto _LL1068;} else{ goto
_LL1069;} _LL1069: goto _LL1070; _LL1064: s= Cyc_Toc_init_array( nv, lval,
_temp1071, s); goto _LL1062; _LL1066: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1077, _temp1075, _temp1073, s, 0); goto _LL1062; _LL1068: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp1081, _temp1079, s); goto _LL1062; _LL1070: Cyc_Toc_exp_to_c( nv,
_temp1041); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp1041, 0), s, 0); goto _LL1062; _LL1062:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp1083= vd->name; goto _LL1084;
_LL1084: if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp1083, Cyc_Absyn_varb_exp( _temp1083,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1107=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1107[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1108; _temp1108.tag= Cyc_Absyn_Local_b; _temp1108.f1=
vd; _temp1108;}); _temp1107;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp1083, 0),
Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( _temp1083, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp*
ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( _temp1083, 0), 0); struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( _temp1083, 0), 0); struct
Cyc_Absyn_Stmt* body;{ void* _temp1085=( void*) e2->r; struct Cyc_List_List*
_temp1095; struct Cyc_Absyn_Exp* _temp1097; struct Cyc_Absyn_Exp* _temp1099;
struct Cyc_Absyn_Vardecl* _temp1101; struct Cyc_List_List* _temp1103; void*
_temp1105; _LL1087: if(*(( int*) _temp1085) == Cyc_Absyn_Array_e){ _LL1096:
_temp1095=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1085)->f1;
goto _LL1088;} else{ goto _LL1089;} _LL1089: if(*(( int*) _temp1085) == Cyc_Absyn_Comprehension_e){
_LL1102: _temp1101=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1085)->f1; goto _LL1100; _LL1100: _temp1099=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1085)->f2; goto _LL1098; _LL1098:
_temp1097=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1085)->f3; goto _LL1090;} else{ goto _LL1091;} _LL1091: if(*(( int*)
_temp1085) == Cyc_Absyn_AnonStruct_e){ _LL1106: _temp1105=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1085)->f1; goto _LL1104; _LL1104: _temp1103=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1085)->f2; goto _LL1092;} else{ goto
_LL1093;} _LL1093: goto _LL1094; _LL1088: body= Cyc_Toc_init_array( nv2, lval,
_temp1095, Cyc_Absyn_skip_stmt( 0)); goto _LL1086; _LL1090: body= Cyc_Toc_init_comprehension(
nv2, lval, _temp1101, _temp1099, _temp1097, Cyc_Absyn_skip_stmt( 0), 0); goto
_LL1086; _LL1092: body= Cyc_Toc_init_anon_struct( nv, lval, _temp1105, _temp1103,
Cyc_Absyn_skip_stmt( 0)); goto _LL1086; _LL1094: Cyc_Toc_exp_to_c( nv2, e2);
body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL1086; _LL1086:;}{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt(
max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp1083, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct
Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
lhs, void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp1109=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); goto _LL1110; _LL1110: for( 0; _temp1109 != 0;
_temp1109=(( struct Cyc_List_List*) _check_null( _temp1109))->tl){ struct
_tuple4 _temp1113; struct Cyc_Absyn_Exp* _temp1114; struct Cyc_List_List*
_temp1116; struct _tuple4* _temp1111=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1109))->hd; _temp1113=* _temp1111; _LL1117: _temp1116=
_temp1113.f1; goto _LL1115; _LL1115: _temp1114= _temp1113.f2; goto _LL1112;
_LL1112: if( _temp1116 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1118=( char*)"empty designator list"; struct
_tagged_string _temp1119; _temp1119.curr= _temp1118; _temp1119.base= _temp1118;
_temp1119.last_plus_one= _temp1118 + 22; _temp1119;}));} if((( struct Cyc_List_List*)
_check_null( _temp1116))->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1120=( char*)"too many designators in anonymous struct";
struct _tagged_string _temp1121; _temp1121.curr= _temp1120; _temp1121.base=
_temp1120; _temp1121.last_plus_one= _temp1120 + 41; _temp1121;}));}{ void*
_temp1122=( void*)(( struct Cyc_List_List*) _check_null( _temp1116))->hd; struct
_tagged_string* _temp1128; _LL1124: if(*(( int*) _temp1122) == Cyc_Absyn_FieldName){
_LL1129: _temp1128=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1122)->f1; goto _LL1125;} else{ goto _LL1126;} _LL1126: goto _LL1127;
_LL1125: { struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs,
_temp1128, 0);{ void* _temp1130=( void*) _temp1114->r; struct Cyc_List_List*
_temp1140; struct Cyc_Absyn_Exp* _temp1142; struct Cyc_Absyn_Exp* _temp1144;
struct Cyc_Absyn_Vardecl* _temp1146; struct Cyc_List_List* _temp1148; void*
_temp1150; _LL1132: if(*(( int*) _temp1130) == Cyc_Absyn_Array_e){ _LL1141:
_temp1140=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1130)->f1;
goto _LL1133;} else{ goto _LL1134;} _LL1134: if(*(( int*) _temp1130) == Cyc_Absyn_Comprehension_e){
_LL1147: _temp1146=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1130)->f1; goto _LL1145; _LL1145: _temp1144=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1130)->f2; goto _LL1143; _LL1143:
_temp1142=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1130)->f3; goto _LL1135;} else{ goto _LL1136;} _LL1136: if(*(( int*)
_temp1130) == Cyc_Absyn_AnonStruct_e){ _LL1151: _temp1150=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1130)->f1; goto _LL1149; _LL1149: _temp1148=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1130)->f2; goto _LL1137;} else{ goto
_LL1138;} _LL1138: goto _LL1139; _LL1133: s= Cyc_Toc_init_array( nv, lval,
_temp1140, s); goto _LL1131; _LL1135: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1146, _temp1144, _temp1142, s, 0); goto _LL1131; _LL1137: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp1150, _temp1148, s); goto _LL1131; _LL1139: Cyc_Toc_exp_to_c( nv,
_temp1114); if( Cyc_Toc_is_poly_field( struct_type, _temp1128)){ _temp1114= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1114, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp1114, 0), 0), s, 0); goto _LL1131; _LL1131:;}
goto _LL1123;} _LL1127:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1152=( char*)"array designator in struct";
struct _tagged_string _temp1153; _temp1153.curr= _temp1152; _temp1153.base=
_temp1152; _temp1153.last_plus_one= _temp1152 + 27; _temp1153;}))); _LL1123:;}}}
return s;} static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env*
nv, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
Cyc_List_List* _temp1154=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
es); goto _LL1155; _LL1155: { struct _tagged_string* _temp1156= Cyc_Toc_add_tuple_type(
_temp1154); goto _LL1157; _LL1157: { struct _tuple0* _temp1158= Cyc_Toc_temp_var();
goto _LL1159; _LL1159: { struct Cyc_Absyn_Exp* _temp1160= Cyc_Absyn_var_exp(
_temp1158, 0); goto _LL1161; _LL1161: { struct Cyc_Absyn_Stmt* _temp1162= Cyc_Absyn_exp_stmt(
_temp1160, 0); goto _LL1163; _LL1163: { struct Cyc_Absyn_Exp*(* _temp1164)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1165;
_LL1165: { int is_atomic= 1; es=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( es);{ int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es); for( 0; es != 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl, -- i)){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd; struct Cyc_Absyn_Exp* lval= _temp1164( _temp1160,({
struct _tagged_string* _temp1183=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp1183[ 0]= xprintf("f%d", i); _temp1183;}), 0);
is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp1166=( void*) e->r; struct Cyc_List_List*
_temp1174; struct Cyc_Absyn_Exp* _temp1176; struct Cyc_Absyn_Exp* _temp1178;
struct Cyc_Absyn_Vardecl* _temp1180; _LL1168: if(*(( int*) _temp1166) == Cyc_Absyn_Array_e){
_LL1175: _temp1174=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1166)->f1; goto _LL1169;} else{ goto _LL1170;} _LL1170: if(*(( int*)
_temp1166) == Cyc_Absyn_Comprehension_e){ _LL1181: _temp1180=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1166)->f1; goto _LL1179; _LL1179:
_temp1178=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1166)->f2; goto _LL1177; _LL1177: _temp1176=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1166)->f3; goto _LL1171;} else{
goto _LL1172;} _LL1172: goto _LL1173; _LL1169: _temp1162= Cyc_Toc_init_array( nv,
lval, _temp1174, _temp1162); goto _LL1167; _LL1171: _temp1162= Cyc_Toc_init_comprehension(
nv, lval, _temp1180, _temp1178, _temp1176, _temp1162, 0); goto _LL1167; _LL1173:
Cyc_Toc_exp_to_c( nv, e); _temp1162= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1164( _temp1160,({ struct _tagged_string* _temp1182=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1182[ 0]= xprintf("f%d", i);
_temp1182;}), 0), e, 0), 0), _temp1162, 0); goto _LL1167; _LL1167:;}}} return
Cyc_Toc_make_struct( nv, _temp1158, Cyc_Absyn_strct( _temp1156), _temp1162,
pointer, rgnopt, is_atomic);}}}}}}} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct(
struct Cyc_Toc_Env* nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* dles, struct _tuple0* tdn){ struct _tuple0*
_temp1184= Cyc_Toc_temp_var(); goto _LL1185; _LL1185: { struct Cyc_Absyn_Exp*
_temp1186= Cyc_Absyn_var_exp( _temp1184, 0); goto _LL1187; _LL1187: { struct Cyc_Absyn_Stmt*
_temp1188= Cyc_Absyn_exp_stmt( _temp1186, 0); goto _LL1189; _LL1189: { struct
Cyc_Absyn_Exp*(* _temp1190)( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
goto _LL1191; _LL1191: { int is_atomic= 1;{ struct Cyc_List_List* _temp1192=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto
_LL1193; _LL1193: for( 0; _temp1192 != 0; _temp1192=(( struct Cyc_List_List*)
_check_null( _temp1192))->tl){ struct _tuple4 _temp1196; struct Cyc_Absyn_Exp*
_temp1197; struct Cyc_List_List* _temp1199; struct _tuple4* _temp1194=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp1192))->hd; _temp1196=*
_temp1194; _LL1200: _temp1199= _temp1196.f1; goto _LL1198; _LL1198: _temp1197=
_temp1196.f2; goto _LL1195; _LL1195: is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1197->topt))->v): 0; if(
_temp1199 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1201=( char*)"empty designator list"; struct
_tagged_string _temp1202; _temp1202.curr= _temp1201; _temp1202.base= _temp1201;
_temp1202.last_plus_one= _temp1201 + 22; _temp1202;}));} if((( struct Cyc_List_List*)
_check_null( _temp1199))->tl != 0){ struct _tuple0* _temp1203= Cyc_Toc_temp_var();
goto _LL1204; _LL1204: { struct Cyc_Absyn_Exp* _temp1205= Cyc_Absyn_var_exp(
_temp1203, 0); goto _LL1206; _LL1206: for( 0; _temp1199 != 0; _temp1199=((
struct Cyc_List_List*) _check_null( _temp1199))->tl){ void* _temp1207=( void*)((
struct Cyc_List_List*) _check_null( _temp1199))->hd; struct _tagged_string*
_temp1213; _LL1209: if(*(( int*) _temp1207) == Cyc_Absyn_FieldName){ _LL1214:
_temp1213=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1207)->f1; goto _LL1210;} else{ goto _LL1211;} _LL1211: goto _LL1212;
_LL1210: if( Cyc_Toc_is_poly_field( struct_type, _temp1213)){ _temp1205= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1205, 0);} _temp1188= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp1190( _temp1186, _temp1213, 0), _temp1205, 0), 0),
_temp1188, 0); goto _LL1208; _LL1212:( void) _throw((( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1215=(
char*)"array designator in struct"; struct _tagged_string _temp1216; _temp1216.curr=
_temp1215; _temp1216.base= _temp1215; _temp1216.last_plus_one= _temp1215 + 27;
_temp1216;}))); _LL1208:;} Cyc_Toc_exp_to_c( nv, _temp1197); _temp1188= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp1205, _temp1197, 0), 0),
_temp1188, 0);}} else{ void* _temp1217=( void*)(( struct Cyc_List_List*)
_check_null( _temp1199))->hd; struct _tagged_string* _temp1223; _LL1219: if(*((
int*) _temp1217) == Cyc_Absyn_FieldName){ _LL1224: _temp1223=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1217)->f1; goto
_LL1220;} else{ goto _LL1221;} _LL1221: goto _LL1222; _LL1220: { struct Cyc_Absyn_Exp*
lval= _temp1190( _temp1186, _temp1223, 0);{ void* _temp1225=( void*) _temp1197->r;
struct Cyc_List_List* _temp1235; struct Cyc_Absyn_Exp* _temp1237; struct Cyc_Absyn_Exp*
_temp1239; struct Cyc_Absyn_Vardecl* _temp1241; struct Cyc_List_List* _temp1243;
void* _temp1245; _LL1227: if(*(( int*) _temp1225) == Cyc_Absyn_Array_e){ _LL1236:
_temp1235=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1225)->f1;
goto _LL1228;} else{ goto _LL1229;} _LL1229: if(*(( int*) _temp1225) == Cyc_Absyn_Comprehension_e){
_LL1242: _temp1241=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1225)->f1; goto _LL1240; _LL1240: _temp1239=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1225)->f2; goto _LL1238; _LL1238:
_temp1237=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1225)->f3; goto _LL1230;} else{ goto _LL1231;} _LL1231: if(*(( int*)
_temp1225) == Cyc_Absyn_AnonStruct_e){ _LL1246: _temp1245=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1225)->f1; goto _LL1244; _LL1244: _temp1243=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1225)->f2; goto _LL1232;} else{ goto
_LL1233;} _LL1233: goto _LL1234; _LL1228: _temp1188= Cyc_Toc_init_array( nv,
lval, _temp1235, _temp1188); goto _LL1226; _LL1230: _temp1188= Cyc_Toc_init_comprehension(
nv, lval, _temp1241, _temp1239, _temp1237, _temp1188, 0); goto _LL1226; _LL1232:
_temp1188= Cyc_Toc_init_anon_struct( nv, lval, _temp1245, _temp1243, _temp1188);
goto _LL1226; _LL1234: Cyc_Toc_exp_to_c( nv, _temp1197); if( Cyc_Toc_is_poly_field(
struct_type, _temp1223)){ _temp1197= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1197, 0);} _temp1188= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1197, 0), 0), _temp1188, 0); goto _LL1226; _LL1226:;} goto _LL1218;}
_LL1222:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1247=( char*)"array designator in struct";
struct _tagged_string _temp1248; _temp1248.curr= _temp1247; _temp1248.base=
_temp1247; _temp1248.last_plus_one= _temp1247 + 27; _temp1248;}))); _LL1218:;}}}
return Cyc_Toc_make_struct( nv, _temp1184, Cyc_Absyn_strctq( tdn), _temp1188,
pointer, rgnopt, is_atomic);}}}}} struct _tuple11{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp(
el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp1249=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp1249[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp1250; _temp1250.tag=
Cyc_Absyn_Increment_e; _temp1250.f1= e1; _temp1250.f2=( void*) incr; _temp1250;});
_temp1249;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1251=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp1261; struct Cyc_Absyn_Exp* _temp1263; void* _temp1265; struct
_tagged_string* _temp1267; struct Cyc_Absyn_Exp* _temp1269; _LL1253: if(*(( int*)
_temp1251) == Cyc_Absyn_StmtExp_e){ _LL1262: _temp1261=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1251)->f1; goto _LL1254;} else{ goto
_LL1255;} _LL1255: if(*(( int*) _temp1251) == Cyc_Absyn_Cast_e){ _LL1266:
_temp1265=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1251)->f1; goto
_LL1264; _LL1264: _temp1263=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1251)->f2; goto _LL1256;} else{ goto _LL1257;} _LL1257: if(*(( int*)
_temp1251) == Cyc_Absyn_StructMember_e){ _LL1270: _temp1269=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1251)->f1; goto _LL1268; _LL1268:
_temp1267=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1251)->f2; goto _LL1258;} else{ goto _LL1259;} _LL1259: goto _LL1260;
_LL1254:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1261, fs, f, f_env); goto _LL1252; _LL1256:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1263, fs, f, f_env); goto
_LL1252; _LL1258:( void*)( e1->r=( void*)(( void*) _temp1269->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1271=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1271->hd=( void*) _temp1267; _temp1271->tl= fs;
_temp1271;}), f, f_env); goto _LL1252; _LL1260: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1272= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1272.last_plus_one - _temp1272.curr, _temp1272.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1252;}
_LL1252:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1273=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1283; struct Cyc_Absyn_Stmt* _temp1285; struct Cyc_Absyn_Decl* _temp1287;
struct Cyc_Absyn_Stmt* _temp1289; struct Cyc_Absyn_Stmt* _temp1291; _LL1275: if((
unsigned int) _temp1273 > 1u?*(( int*) _temp1273) == Cyc_Absyn_Exp_s: 0){
_LL1284: _temp1283=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1273)->f1; goto _LL1276;} else{ goto _LL1277;} _LL1277: if(( unsigned int)
_temp1273 > 1u?*(( int*) _temp1273) == Cyc_Absyn_Decl_s: 0){ _LL1288: _temp1287=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1273)->f1; goto
_LL1286; _LL1286: _temp1285=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1273)->f2; goto _LL1278;} else{ goto _LL1279;} _LL1279: if(( unsigned int)
_temp1273 > 1u?*(( int*) _temp1273) == Cyc_Absyn_Seq_s: 0){ _LL1292: _temp1291=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1273)->f1; goto
_LL1290; _LL1290: _temp1289=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1273)->f2; goto _LL1280;} else{ goto _LL1281;} _LL1281: goto _LL1282;
_LL1276:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1283, fs, f, f_env); goto _LL1274; _LL1278:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1285, fs, f, f_env);
goto _LL1274; _LL1280:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1289, fs, f, f_env); goto _LL1274; _LL1282:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1293= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1293.last_plus_one - _temp1293.curr,
_temp1293.curr);})); goto _LL1274; _LL1274:;} static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s); struct _tuple12{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp1294=( void*) e->r; goto _LL1295;
_LL1295: if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1296= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp1296.last_plus_one - _temp1296.curr, _temp1296.curr);})); return;}{ void*
old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void*
_temp1297= _temp1294; void* _temp1373; void* _temp1375; void* _temp1377; struct
_tuple0* _temp1379; struct _tuple0* _temp1381; struct Cyc_List_List* _temp1383;
void* _temp1385; void* _temp1387; struct Cyc_Absyn_Exp* _temp1389; struct Cyc_Absyn_Exp*
_temp1391; struct Cyc_Core_Opt* _temp1393; struct Cyc_Absyn_Exp* _temp1395;
struct Cyc_Absyn_Exp* _temp1397; struct Cyc_Absyn_Exp* _temp1399; struct Cyc_Absyn_Exp*
_temp1401; struct Cyc_Absyn_Exp* _temp1403; struct Cyc_Absyn_Exp* _temp1405;
struct Cyc_List_List* _temp1407; struct Cyc_Absyn_Exp* _temp1409; struct Cyc_List_List*
_temp1411; struct Cyc_Absyn_Exp* _temp1413; struct Cyc_Absyn_Exp* _temp1415;
struct Cyc_Absyn_Exp* _temp1417; struct Cyc_List_List* _temp1419; struct Cyc_Absyn_Exp*
_temp1421; struct Cyc_Absyn_Exp* _temp1423; struct Cyc_Absyn_Exp** _temp1425;
void* _temp1426; void** _temp1428; struct Cyc_Absyn_Exp* _temp1429; struct Cyc_Absyn_Exp*
_temp1431; struct Cyc_Absyn_Exp* _temp1433; void* _temp1435; struct Cyc_Absyn_Exp*
_temp1437; struct Cyc_Absyn_Exp* _temp1439; struct _tagged_string* _temp1441;
struct Cyc_Absyn_Exp* _temp1443; struct _tagged_string* _temp1445; struct Cyc_Absyn_Exp*
_temp1447; struct Cyc_Absyn_Exp* _temp1449; struct Cyc_Absyn_Exp* _temp1451;
struct Cyc_List_List* _temp1453; struct Cyc_List_List* _temp1455; struct _tuple1*
_temp1457; struct Cyc_List_List* _temp1459; struct Cyc_Absyn_Exp* _temp1461;
struct Cyc_Absyn_Exp* _temp1463; struct Cyc_Absyn_Vardecl* _temp1465; struct Cyc_Absyn_Structdecl*
_temp1467; struct Cyc_List_List* _temp1469; struct Cyc_Core_Opt* _temp1471;
struct _tuple0* _temp1473; struct Cyc_List_List* _temp1475; void* _temp1477;
struct Cyc_Absyn_Tunionfield* _temp1479; struct Cyc_Absyn_Tuniondecl* _temp1481;
struct Cyc_List_List* _temp1483; struct Cyc_Core_Opt* _temp1485; struct Cyc_Core_Opt*
_temp1487; struct Cyc_Absyn_Tunionfield* _temp1489; struct Cyc_Absyn_Tuniondecl*
_temp1491; struct Cyc_List_List* _temp1493; struct Cyc_Core_Opt* _temp1495;
struct Cyc_Core_Opt* _temp1497; struct Cyc_Absyn_Enumfield* _temp1499; struct
Cyc_Absyn_Enumdecl* _temp1501; struct _tuple0* _temp1503; void* _temp1505;
struct Cyc_Absyn_Exp* _temp1507; struct Cyc_Absyn_Stmt* _temp1509; struct Cyc_List_List*
_temp1511; struct Cyc_Core_Opt* _temp1513; struct Cyc_Absyn_Fndecl* _temp1515;
struct Cyc_Absyn_Exp* _temp1517; _LL1299: if(*(( int*) _temp1297) == Cyc_Absyn_Const_e){
_LL1374: _temp1373=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1297)->f1;
if( _temp1373 ==( void*) Cyc_Absyn_Null_c){ goto _LL1300;} else{ goto _LL1301;}}
else{ goto _LL1301;} _LL1301: if(*(( int*) _temp1297) == Cyc_Absyn_Const_e){
_LL1376: _temp1375=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1297)->f1;
goto _LL1302;} else{ goto _LL1303;} _LL1303: if(*(( int*) _temp1297) == Cyc_Absyn_Var_e){
_LL1380: _temp1379=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1297)->f1; goto _LL1378; _LL1378: _temp1377=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1297)->f2; goto _LL1304;} else{ goto _LL1305;} _LL1305: if(*(( int*)
_temp1297) == Cyc_Absyn_UnknownId_e){ _LL1382: _temp1381=( struct _tuple0*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1297)->f1; goto _LL1306;} else{ goto
_LL1307;} _LL1307: if(*(( int*) _temp1297) == Cyc_Absyn_Primop_e){ _LL1386:
_temp1385=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1297)->f1; goto
_LL1384; _LL1384: _temp1383=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1297)->f2; goto _LL1308;} else{ goto _LL1309;} _LL1309: if(*(( int*)
_temp1297) == Cyc_Absyn_Increment_e){ _LL1390: _temp1389=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp1297)->f1; goto _LL1388; _LL1388:
_temp1387=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1297)->f2; goto
_LL1310;} else{ goto _LL1311;} _LL1311: if(*(( int*) _temp1297) == Cyc_Absyn_AssignOp_e){
_LL1396: _temp1395=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1297)->f1; goto _LL1394; _LL1394: _temp1393=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1297)->f2; goto _LL1392; _LL1392:
_temp1391=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1297)->f3; goto _LL1312;} else{ goto _LL1313;} _LL1313: if(*(( int*)
_temp1297) == Cyc_Absyn_Conditional_e){ _LL1402: _temp1401=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1297)->f1; goto _LL1400; _LL1400:
_temp1399=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1297)->f2; goto _LL1398; _LL1398: _temp1397=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1297)->f3; goto _LL1314;} else{
goto _LL1315;} _LL1315: if(*(( int*) _temp1297) == Cyc_Absyn_SeqExp_e){ _LL1406:
_temp1405=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1297)->f1; goto _LL1404; _LL1404: _temp1403=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1297)->f2; goto _LL1316;} else{ goto
_LL1317;} _LL1317: if(*(( int*) _temp1297) == Cyc_Absyn_UnknownCall_e){ _LL1410:
_temp1409=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1297)->f1; goto _LL1408; _LL1408: _temp1407=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1297)->f2; goto _LL1318;} else{
goto _LL1319;} _LL1319: if(*(( int*) _temp1297) == Cyc_Absyn_FnCall_e){ _LL1414:
_temp1413=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1297)->f1; goto _LL1412; _LL1412: _temp1411=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1297)->f2; goto _LL1320;} else{ goto
_LL1321;} _LL1321: if(*(( int*) _temp1297) == Cyc_Absyn_Throw_e){ _LL1416:
_temp1415=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1297)->f1;
goto _LL1322;} else{ goto _LL1323;} _LL1323: if(*(( int*) _temp1297) == Cyc_Absyn_NoInstantiate_e){
_LL1418: _temp1417=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1297)->f1; goto _LL1324;} else{ goto _LL1325;} _LL1325: if(*(( int*)
_temp1297) == Cyc_Absyn_Instantiate_e){ _LL1422: _temp1421=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1297)->f1; goto _LL1420; _LL1420:
_temp1419=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1297)->f2; goto _LL1326;} else{ goto _LL1327;} _LL1327: if(*(( int*)
_temp1297) == Cyc_Absyn_Cast_e){ _LL1427: _temp1426=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1297)->f1; _temp1428=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1297)->f1;
goto _LL1424; _LL1424: _temp1423=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1297)->f2; _temp1425=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1297)->f2;
goto _LL1328;} else{ goto _LL1329;} _LL1329: if(*(( int*) _temp1297) == Cyc_Absyn_Address_e){
_LL1430: _temp1429=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp1297)->f1; goto _LL1330;} else{ goto _LL1331;} _LL1331: if(*(( int*)
_temp1297) == Cyc_Absyn_New_e){ _LL1434: _temp1433=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1297)->f1; goto _LL1432; _LL1432: _temp1431=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1297)->f2; goto
_LL1332;} else{ goto _LL1333;} _LL1333: if(*(( int*) _temp1297) == Cyc_Absyn_Sizeoftyp_e){
_LL1436: _temp1435=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1297)->f1;
goto _LL1334;} else{ goto _LL1335;} _LL1335: if(*(( int*) _temp1297) == Cyc_Absyn_Sizeofexp_e){
_LL1438: _temp1437=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1297)->f1; goto _LL1336;} else{ goto _LL1337;} _LL1337: if(*(( int*)
_temp1297) == Cyc_Absyn_Deref_e){ _LL1440: _temp1439=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp1297)->f1; goto _LL1338;} else{ goto
_LL1339;} _LL1339: if(*(( int*) _temp1297) == Cyc_Absyn_StructMember_e){ _LL1444:
_temp1443=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1297)->f1; goto _LL1442; _LL1442: _temp1441=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1297)->f2; goto _LL1340;} else{
goto _LL1341;} _LL1341: if(*(( int*) _temp1297) == Cyc_Absyn_StructArrow_e){
_LL1448: _temp1447=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1297)->f1; goto _LL1446; _LL1446: _temp1445=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1297)->f2; goto _LL1342;} else{
goto _LL1343;} _LL1343: if(*(( int*) _temp1297) == Cyc_Absyn_Subscript_e){
_LL1452: _temp1451=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1297)->f1; goto _LL1450; _LL1450: _temp1449=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1297)->f2; goto _LL1344;} else{ goto
_LL1345;} _LL1345: if(*(( int*) _temp1297) == Cyc_Absyn_Tuple_e){ _LL1454:
_temp1453=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1297)->f1;
goto _LL1346;} else{ goto _LL1347;} _LL1347: if(*(( int*) _temp1297) == Cyc_Absyn_CompoundLit_e){
_LL1458: _temp1457=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1297)->f1; goto _LL1456; _LL1456: _temp1455=( struct Cyc_List_List*)((
struct Cyc_Absyn_CompoundLit_e_struct*) _temp1297)->f2; goto _LL1348;} else{
goto _LL1349;} _LL1349: if(*(( int*) _temp1297) == Cyc_Absyn_Array_e){ _LL1460:
_temp1459=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1297)->f1;
goto _LL1350;} else{ goto _LL1351;} _LL1351: if(*(( int*) _temp1297) == Cyc_Absyn_Comprehension_e){
_LL1466: _temp1465=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1297)->f1; goto _LL1464; _LL1464: _temp1463=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1297)->f2; goto _LL1462; _LL1462:
_temp1461=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1297)->f3; goto _LL1352;} else{ goto _LL1353;} _LL1353: if(*(( int*)
_temp1297) == Cyc_Absyn_Struct_e){ _LL1474: _temp1473=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1297)->f1; goto _LL1472; _LL1472:
_temp1471=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1297)->f2;
goto _LL1470; _LL1470: _temp1469=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1297)->f3; goto _LL1468; _LL1468: _temp1467=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1297)->f4; goto _LL1354;} else{ goto
_LL1355;} _LL1355: if(*(( int*) _temp1297) == Cyc_Absyn_AnonStruct_e){ _LL1478:
_temp1477=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1297)->f1; goto
_LL1476; _LL1476: _temp1475=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1297)->f2; goto _LL1356;} else{ goto _LL1357;} _LL1357: if(*(( int*)
_temp1297) == Cyc_Absyn_Tunion_e){ _LL1488: _temp1487=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1297)->f1; goto _LL1486; _LL1486:
_temp1485=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1297)->f2;
goto _LL1484; _LL1484: _temp1483=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1297)->f3; if( _temp1483 == 0){ goto _LL1482;} else{ goto _LL1359;} _LL1482:
_temp1481=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1297)->f4; goto _LL1480; _LL1480: _temp1479=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1297)->f5; goto _LL1358;} else{ goto
_LL1359;} _LL1359: if(*(( int*) _temp1297) == Cyc_Absyn_Tunion_e){ _LL1498:
_temp1497=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1297)->f1;
goto _LL1496; _LL1496: _temp1495=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1297)->f2; goto _LL1494; _LL1494: _temp1493=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1297)->f3; goto _LL1492; _LL1492:
_temp1491=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1297)->f4; goto _LL1490; _LL1490: _temp1489=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1297)->f5; goto _LL1360;} else{ goto
_LL1361;} _LL1361: if(*(( int*) _temp1297) == Cyc_Absyn_Enum_e){ _LL1504:
_temp1503=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1297)->f1;
goto _LL1502; _LL1502: _temp1501=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1297)->f2; goto _LL1500; _LL1500: _temp1499=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1297)->f3; goto _LL1362;} else{ goto
_LL1363;} _LL1363: if(*(( int*) _temp1297) == Cyc_Absyn_Malloc_e){ _LL1508:
_temp1507=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1297)->f1; goto _LL1506; _LL1506: _temp1505=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1297)->f2; goto _LL1364;} else{ goto _LL1365;} _LL1365: if(*(( int*)
_temp1297) == Cyc_Absyn_StmtExp_e){ _LL1510: _temp1509=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1297)->f1; goto _LL1366;} else{ goto
_LL1367;} _LL1367: if(*(( int*) _temp1297) == Cyc_Absyn_UnresolvedMem_e){
_LL1514: _temp1513=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1297)->f1; goto _LL1512; _LL1512: _temp1511=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1297)->f2; goto _LL1368;} else{
goto _LL1369;} _LL1369: if(*(( int*) _temp1297) == Cyc_Absyn_Codegen_e){ _LL1516:
_temp1515=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1297)->f1; goto _LL1370;} else{ goto _LL1371;} _LL1371: if(*(( int*)
_temp1297) == Cyc_Absyn_Fill_e){ _LL1518: _temp1517=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp1297)->f1; goto _LL1372;} else{ goto
_LL1298;} _LL1300:{ void* _temp1519= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1525; struct Cyc_Absyn_Conref* _temp1527; struct Cyc_Absyn_Tqual _temp1529;
struct Cyc_Absyn_Conref* _temp1531; void* _temp1533; void* _temp1535; _LL1521:
if(( unsigned int) _temp1519 > 4u?*(( int*) _temp1519) == Cyc_Absyn_PointerType:
0){ _LL1526: _temp1525=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1519)->f1; _LL1536: _temp1535=( void*) _temp1525.elt_typ; goto _LL1534;
_LL1534: _temp1533=( void*) _temp1525.rgn_typ; goto _LL1532; _LL1532: _temp1531=(
struct Cyc_Absyn_Conref*) _temp1525.nullable; goto _LL1530; _LL1530: _temp1529=(
struct Cyc_Absyn_Tqual) _temp1525.tq; goto _LL1528; _LL1528: _temp1527=( struct
Cyc_Absyn_Conref*) _temp1525.bounds; goto _LL1522;} else{ goto _LL1523;} _LL1523:
goto _LL1524; _LL1522: if(( void*) _temp1527->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1520;}{ void* _temp1537=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1527); struct Cyc_Absyn_Exp* _temp1543; _LL1539: if( _temp1537 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1540;} else{ goto _LL1541;} _LL1541: if((
unsigned int) _temp1537 > 1u?*(( int*) _temp1537) == Cyc_Absyn_Upper_b: 0){
_LL1544: _temp1543=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1537)->f1; goto _LL1542;} else{ goto _LL1538;} _LL1540: { struct _tuple4*
_temp1546=({ struct _tuple4* _temp1545=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1545->f1= 0; _temp1545->f2= Cyc_Absyn_signed_int_exp( 0,
0); _temp1545;}); goto _LL1547; _LL1547: { struct Cyc_Absyn_Exp* _temp1551= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp1548=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1548->hd=( void*) _temp1546; _temp1548->tl=({
struct Cyc_List_List* _temp1549=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1549->hd=( void*) _temp1546; _temp1549->tl=({
struct Cyc_List_List* _temp1550=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1550->hd=( void*) _temp1546; _temp1550->tl= 0;
_temp1550;}); _temp1549;}); _temp1548;}), 0); goto _LL1552; _LL1552: if( nv->toplevel){(
void*)( e->r=( void*)(( void*) _temp1551->r));} else{ void* _temp1553= Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1535), Cyc_Toc_mt_tq); goto _LL1554; _LL1554: { struct
_tuple0* _temp1555= Cyc_Toc_temp_var(); goto _LL1556; _LL1556: { struct Cyc_Absyn_Exp*
_temp1557= Cyc_Absyn_var_exp( _temp1555, 0); goto _LL1558; _LL1558: { struct Cyc_Absyn_Stmt*
_temp1559= Cyc_Absyn_declare_stmt( _temp1555, _temp1553,( struct Cyc_Absyn_Exp*)
_temp1551, Cyc_Absyn_exp_stmt( _temp1557, 0), 0); goto _LL1560; _LL1560:( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1559, 0))->r));}}}} goto
_LL1538;}} _LL1542:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0,
0))->r)); goto _LL1538; _LL1538:;} goto _LL1520; _LL1524:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1561= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1561.last_plus_one -
_temp1561.curr, _temp1561.curr);})); goto _LL1520; _LL1520:;} goto _LL1298;
_LL1302: goto _LL1298; _LL1304:{ struct _handler_cons _temp1562; _push_handler(&
_temp1562);{ int _temp1564= 0; if( setjmp( _temp1562.handler)){ _temp1564= 1;}
if( ! _temp1564){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1379))->r));; _pop_handler();} else{ void* _temp1563=( void*) _exn_thrown;
void* _temp1566= _temp1563; _LL1568: if( _temp1566 == Cyc_Dict_Absent){ goto
_LL1569;} else{ goto _LL1570;} _LL1570: goto _LL1571; _LL1569:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1572= Cyc_Absynpp_qvar2string(
_temp1379); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1572.last_plus_one
- _temp1572.curr, _temp1572.curr);})); return; _LL1571:( void) _throw( _temp1566);
_LL1567:;}}} goto _LL1298; _LL1306:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1573=( char*)"unknownid"; struct
_tagged_string _temp1574; _temp1574.curr= _temp1573; _temp1574.base= _temp1573;
_temp1574.last_plus_one= _temp1573 + 10; _temp1574;})); goto _LL1298; _LL1308: {
struct Cyc_List_List* _temp1575=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1383); goto _LL1576; _LL1576: { struct Cyc_List_List* _temp1577=(( struct
Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Toc_get_c_typ, _temp1383); goto _LL1578; _LL1578:(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1383);{
void* _temp1579= _temp1385; _LL1581: if( _temp1579 ==( void*) Cyc_Absyn_Size){
goto _LL1582;} else{ goto _LL1583;} _LL1583: if( _temp1579 ==( void*) Cyc_Absyn_Printf){
goto _LL1584;} else{ goto _LL1585;} _LL1585: if( _temp1579 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1586;} else{ goto _LL1587;} _LL1587: if( _temp1579 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1588;} else{ goto _LL1589;} _LL1589: if( _temp1579 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1590;} else{ goto _LL1591;} _LL1591: if( _temp1579 ==( void*) Cyc_Absyn_Plus){
goto _LL1592;} else{ goto _LL1593;} _LL1593: if( _temp1579 ==( void*) Cyc_Absyn_Minus){
goto _LL1594;} else{ goto _LL1595;} _LL1595: if( _temp1579 ==( void*) Cyc_Absyn_Eq){
goto _LL1596;} else{ goto _LL1597;} _LL1597: if( _temp1579 ==( void*) Cyc_Absyn_Neq){
goto _LL1598;} else{ goto _LL1599;} _LL1599: if( _temp1579 ==( void*) Cyc_Absyn_Gt){
goto _LL1600;} else{ goto _LL1601;} _LL1601: if( _temp1579 ==( void*) Cyc_Absyn_Gte){
goto _LL1602;} else{ goto _LL1603;} _LL1603: if( _temp1579 ==( void*) Cyc_Absyn_Lt){
goto _LL1604;} else{ goto _LL1605;} _LL1605: if( _temp1579 ==( void*) Cyc_Absyn_Lte){
goto _LL1606;} else{ goto _LL1607;} _LL1607: goto _LL1608; _LL1582: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1383))->hd;{
void* _temp1609=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1617; struct Cyc_Absyn_Tqual _temp1619; void*
_temp1621; struct Cyc_Absyn_PtrInfo _temp1623; struct Cyc_Absyn_Conref*
_temp1625; struct Cyc_Absyn_Tqual _temp1627; struct Cyc_Absyn_Conref* _temp1629;
void* _temp1631; void* _temp1633; _LL1611: if(( unsigned int) _temp1609 > 4u?*((
int*) _temp1609) == Cyc_Absyn_ArrayType: 0){ _LL1622: _temp1621=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1609)->f1; goto _LL1620; _LL1620:
_temp1619=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1609)->f2; goto _LL1618; _LL1618: _temp1617=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1609)->f3; goto _LL1612;} else{ goto
_LL1613;} _LL1613: if(( unsigned int) _temp1609 > 4u?*(( int*) _temp1609) == Cyc_Absyn_PointerType:
0){ _LL1624: _temp1623=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1609)->f1; _LL1634: _temp1633=( void*) _temp1623.elt_typ; goto _LL1632;
_LL1632: _temp1631=( void*) _temp1623.rgn_typ; goto _LL1630; _LL1630: _temp1629=(
struct Cyc_Absyn_Conref*) _temp1623.nullable; goto _LL1628; _LL1628: _temp1627=(
struct Cyc_Absyn_Tqual) _temp1623.tq; goto _LL1626; _LL1626: _temp1625=( struct
Cyc_Absyn_Conref*) _temp1623.bounds; goto _LL1614;} else{ goto _LL1615;} _LL1615:
goto _LL1616; _LL1612:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1617))->r)); goto _LL1610; _LL1614: { struct _tuple0*
_temp1635= Cyc_Toc_temp_var(); goto _LL1636; _LL1636: { void* _temp1637= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); goto _LL1638;
_LL1638: { struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1635, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1635, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4=
Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp( e2, e3, 0), 0);
struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1635, _temp1637,(
struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1610;}}} _LL1616:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1639= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*) _check_null(
_temp1577))->hd); struct _tagged_string _temp1640= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_temp1639.last_plus_one - _temp1639.curr, _temp1639.curr, _temp1640.last_plus_one
- _temp1640.curr, _temp1640.curr);})); return; _LL1610:;} goto _LL1580;} _LL1584:(
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1385, 0, _temp1383,
_temp1577))->r)); goto _LL1580; _LL1586:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1385, 0, _temp1383, _temp1577))->r)); goto _LL1580; _LL1588: if( _temp1383
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1641=( char*)"fprintf without arguments"; struct
_tagged_string _temp1642; _temp1642.curr= _temp1641; _temp1642.base= _temp1641;
_temp1642.last_plus_one= _temp1641 + 26; _temp1642;}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_printf_to_c( _temp1385,({ struct Cyc_Core_Opt* _temp1643=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1643->v=(
void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1383))->hd);
_temp1643;}),(( struct Cyc_List_List*) _check_null( _temp1383))->tl,(( struct
Cyc_List_List*) _check_null( _temp1577))->tl))->r)); goto _LL1580; _LL1590: if(
_temp1383 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1644=( char*)"sscanf without arguments";
struct _tagged_string _temp1645; _temp1645.curr= _temp1644; _temp1645.base=
_temp1644; _temp1645.last_plus_one= _temp1644 + 25; _temp1645;}));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1383))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1383))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1383))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1580; _LL1592: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1383))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1383))->tl))->hd; struct _tuple0* _temp1646= Cyc_Toc_temp_var(); goto
_LL1647; _LL1647: { void* _temp1648= Cyc_Toc_typ_to_c( old_typ); goto _LL1649;
_LL1649: { struct Cyc_Absyn_Stmt* _temp1650= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1646, 0), 0); goto _LL1651; _LL1651: { struct Cyc_Absyn_Exp* _temp1652= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1646, 0), Cyc_Toc_curr_sp, 0); goto _LL1653; _LL1653: {
struct Cyc_Absyn_Exp* _temp1655= Cyc_Absyn_assignop_exp( _temp1652,({ struct Cyc_Core_Opt*
_temp1654=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1654->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1654;}), e2, 0); goto
_LL1656; _LL1656: _temp1650= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1655,
0), _temp1650, 0); _temp1650= Cyc_Absyn_declare_stmt( _temp1646, _temp1648,(
struct Cyc_Absyn_Exp*) e1, _temp1650, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1650, 0))->r));}}}}} goto _LL1580; _LL1594: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null( _temp1575))->hd)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1383))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1383))->tl))->hd; void*
old_typ_e2=( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1575))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* _temp1657= Cyc_Toc_temp_var(); goto _LL1658;
_LL1658: { void* _temp1659= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_List_List*)
_check_null( _temp1575))->hd); goto _LL1660; _LL1660: { struct Cyc_Absyn_Stmt*
_temp1661= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1657, 0), 0); goto
_LL1662; _LL1662: { struct Cyc_Absyn_Exp* _temp1663= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1657, 0), Cyc_Toc_curr_sp, 0); goto _LL1664; _LL1664: {
struct Cyc_Absyn_Exp* _temp1666= Cyc_Absyn_assignop_exp( _temp1663,({ struct Cyc_Core_Opt*
_temp1665=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1665->v=( void*)(( void*) Cyc_Absyn_Minus); _temp1665;}), e2, 0); goto
_LL1667; _LL1667: _temp1661= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1666,
0), _temp1661, 0); _temp1661= Cyc_Absyn_declare_stmt( _temp1657, _temp1659,(
struct Cyc_Absyn_Exp*) e1, _temp1661, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1661, 0))->r));}}}}}} goto _LL1580; _LL1596: goto _LL1598; _LL1598: goto
_LL1600; _LL1600: goto _LL1602; _LL1602: goto _LL1604; _LL1604: goto _LL1606;
_LL1606: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1383))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1383))->tl))->hd; void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp1575))->hd; void* t2=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1575))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1580;} _LL1608: goto
_LL1580; _LL1580:;} goto _LL1298;}} _LL1310: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1389->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1389); if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if( _temp1387 ==(
void*) Cyc_Absyn_PreInc? 1: _temp1387 ==( void*) Cyc_Absyn_PreDec){ struct
_tuple0* _temp1668= Cyc_Toc_temp_var(); goto _LL1669; _LL1669: { void* _temp1670=
Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1671; _LL1671: { struct Cyc_Absyn_Stmt*
_temp1672= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp1668,
0), 0), 0); goto _LL1673; _LL1673: { struct Cyc_Absyn_Exp* _temp1674= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1668, 0), Cyc_Toc_curr_sp, 0),
_temp1387, 0); goto _LL1675; _LL1675: _temp1672= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1674, 0), _temp1672, 0); _temp1672= Cyc_Absyn_declare_stmt( _temp1668,
_temp1670,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1389, 0),
_temp1672, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1672, 0))->r));}}}}
else{ struct _tuple0* _temp1676= Cyc_Toc_temp_var(); goto _LL1677; _LL1677: {
struct _tuple0* _temp1678= Cyc_Toc_temp_var(); goto _LL1679; _LL1679: { void*
_temp1680= Cyc_Toc_typ_to_c( e2_cyc_typ); goto _LL1681; _LL1681: { void*
_temp1682= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1683; _LL1683: { struct Cyc_Absyn_Stmt*
_temp1684= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1678, 0), 0); goto
_LL1685; _LL1685: { struct Cyc_Absyn_Exp* _temp1686= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1676, 0), Cyc_Toc_curr_sp, 0),
_temp1387, 0); goto _LL1687; _LL1687: _temp1684= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1686, 0), _temp1684, 0); _temp1684= Cyc_Absyn_declare_stmt( _temp1678,
_temp1680,( struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp1676, 0), 0), _temp1684, 0); _temp1684= Cyc_Absyn_declare_stmt( _temp1676,
_temp1682,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1389, 0),
_temp1684, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1684, 0))->r));}}}}}}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1389)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1389, 0, Cyc_Toc_incr_lvalue,
_temp1387);( void*)( e->r=( void*)(( void*) _temp1389->r));}} goto _LL1298;}
_LL1312: { int e1_poly= Cyc_Toc_is_poly_project( _temp1395); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1395->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1391->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1395); Cyc_Toc_exp_to_c( nv, _temp1391); if(
_temp1393 != 0?( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1393))->v ==(
void*) Cyc_Absyn_Plus? 1:( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1393))->v
==( void*) Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){(
void*)( _temp1395->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1395->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1391->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1391->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1391->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1391->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1395)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1395, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1688=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1688[ 0]=({ struct _tuple11 _temp1689; _temp1689.f1=
_temp1393; _temp1689.f2= _temp1391; _temp1689;}); _temp1688;}));( void*)( e->r=(
void*)(( void*) _temp1395->r));} goto _LL1298;} _LL1314: Cyc_Toc_exp_to_c( nv,
_temp1401); Cyc_Toc_exp_to_c( nv, _temp1399); Cyc_Toc_exp_to_c( nv, _temp1397);
goto _LL1298; _LL1316: Cyc_Toc_exp_to_c( nv, _temp1405); Cyc_Toc_exp_to_c( nv,
_temp1403); goto _LL1298; _LL1318: _temp1413= _temp1409; _temp1411= _temp1407;
goto _LL1320; _LL1320: Cyc_Toc_exp_to_c( nv, _temp1413);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1411); goto _LL1298; _LL1322:
Cyc_Toc_exp_to_c( nv, _temp1415);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1415), 0))->r)); goto
_LL1298; _LL1324: Cyc_Toc_exp_to_c( nv, _temp1417); goto _LL1298; _LL1326: Cyc_Toc_exp_to_c(
nv, _temp1421);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); for( 0; _temp1419 != 0; _temp1419=(( struct Cyc_List_List*)
_check_null( _temp1419))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct
Cyc_List_List*) _check_null( _temp1419))->hd); if( k !=( void*) Cyc_Absyn_EffKind?
k !=( void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1421, 0))->r)); break;}} goto _LL1298;} _LL1328: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null((* _temp1425)->topt))->v; void*
new_typ=* _temp1428;* _temp1428= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv,* _temp1425);* _temp1425= Cyc_Toc_convert_exp( nv->toplevel, old_t2, new_typ,*
_temp1425); goto _LL1298;} _LL1330:{ void* _temp1690=( void*) _temp1429->r;
struct Cyc_Absyn_Structdecl* _temp1698; struct Cyc_List_List* _temp1700; struct
Cyc_Core_Opt* _temp1702; struct _tuple0* _temp1704; struct Cyc_List_List*
_temp1706; _LL1692: if(*(( int*) _temp1690) == Cyc_Absyn_Struct_e){ _LL1705:
_temp1704=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1690)->f1;
goto _LL1703; _LL1703: _temp1702=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1690)->f2; goto _LL1701; _LL1701: _temp1700=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1690)->f3; goto _LL1699; _LL1699:
_temp1698=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1690)->f4; goto _LL1693;} else{ goto _LL1694;} _LL1694: if(*(( int*)
_temp1690) == Cyc_Absyn_Tuple_e){ _LL1707: _temp1706=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_e_struct*) _temp1690)->f1; goto _LL1695;} else{ goto
_LL1696;} _LL1696: goto _LL1697; _LL1693: if( nv->toplevel){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1708= Cyc_Position_string_of_segment(
_temp1429->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1708.last_plus_one - _temp1708.curr, _temp1708.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1429->topt))->v, 1, 0, _temp1700, _temp1704))->r)); goto
_LL1691; _LL1695: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1709= Cyc_Position_string_of_segment( _temp1429->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1709.last_plus_one -
_temp1709.curr, _temp1709.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, 0, _temp1706))->r)); goto _LL1691; _LL1697: Cyc_Toc_exp_to_c( nv,
_temp1429); if( ! Cyc_Absyn_is_lvalue( _temp1429)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1429, 0, Cyc_Toc_address_lvalue,
1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v), _temp1429, 0))->r));} goto
_LL1691; _LL1691:;} goto _LL1298; _LL1332: if( nv->toplevel){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1710= Cyc_Position_string_of_segment(
_temp1431->loc); xprintf("%.*s: new at top-level", _temp1710.last_plus_one -
_temp1710.curr, _temp1710.curr);}));}{ void* _temp1711=( void*) _temp1431->r;
struct Cyc_List_List* _temp1725; void* _temp1727; struct _tagged_string
_temp1729; struct Cyc_Absyn_Exp* _temp1731; struct Cyc_Absyn_Exp* _temp1733;
struct Cyc_Absyn_Vardecl* _temp1735; struct Cyc_Absyn_Structdecl* _temp1737;
struct Cyc_List_List* _temp1739; struct Cyc_Core_Opt* _temp1741; struct _tuple0*
_temp1743; struct Cyc_List_List* _temp1745; _LL1713: if(*(( int*) _temp1711) ==
Cyc_Absyn_Array_e){ _LL1726: _temp1725=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1711)->f1; goto _LL1714;} else{ goto _LL1715;} _LL1715: if(*(( int*)
_temp1711) == Cyc_Absyn_Const_e){ _LL1728: _temp1727=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1711)->f1; if(( unsigned int) _temp1727 > 1u?*(( int*) _temp1727) == Cyc_Absyn_String_c:
0){ _LL1730: _temp1729=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1727)->f1; goto _LL1716;} else{ goto _LL1717;}} else{ goto _LL1717;}
_LL1717: if(*(( int*) _temp1711) == Cyc_Absyn_Comprehension_e){ _LL1736:
_temp1735=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1711)->f1; goto _LL1734; _LL1734: _temp1733=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1711)->f2; goto _LL1732; _LL1732:
_temp1731=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1711)->f3; goto _LL1718;} else{ goto _LL1719;} _LL1719: if(*(( int*)
_temp1711) == Cyc_Absyn_Struct_e){ _LL1744: _temp1743=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1711)->f1; goto _LL1742; _LL1742:
_temp1741=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1711)->f2;
goto _LL1740; _LL1740: _temp1739=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1711)->f3; goto _LL1738; _LL1738: _temp1737=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1711)->f4; goto _LL1720;} else{ goto
_LL1721;} _LL1721: if(*(( int*) _temp1711) == Cyc_Absyn_Tuple_e){ _LL1746:
_temp1745=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1711)->f1;
goto _LL1722;} else{ goto _LL1723;} _LL1723: goto _LL1724; _LL1714: { struct
_tuple0* _temp1747= Cyc_Toc_temp_var(); goto _LL1748; _LL1748: { struct Cyc_Absyn_Exp*
_temp1749= Cyc_Absyn_var_exp( _temp1747, 0); goto _LL1750; _LL1750: { struct Cyc_Absyn_Stmt*
_temp1751= Cyc_Toc_init_array( nv, _temp1749, _temp1725, Cyc_Absyn_exp_stmt(
_temp1749, 0)); goto _LL1752; _LL1752: { void* old_elt_typ;{ void* _temp1753=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1759; struct Cyc_Absyn_Conref*
_temp1761; struct Cyc_Absyn_Tqual _temp1763; struct Cyc_Absyn_Conref* _temp1765;
void* _temp1767; void* _temp1769; _LL1755: if(( unsigned int) _temp1753 > 4u?*((
int*) _temp1753) == Cyc_Absyn_PointerType: 0){ _LL1760: _temp1759=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1753)->f1; _LL1770: _temp1769=( void*)
_temp1759.elt_typ; goto _LL1768; _LL1768: _temp1767=( void*) _temp1759.rgn_typ;
goto _LL1766; _LL1766: _temp1765=( struct Cyc_Absyn_Conref*) _temp1759.nullable;
goto _LL1764; _LL1764: _temp1763=( struct Cyc_Absyn_Tqual) _temp1759.tq; goto
_LL1762; _LL1762: _temp1761=( struct Cyc_Absyn_Conref*) _temp1759.bounds; goto
_LL1756;} else{ goto _LL1757;} _LL1757: goto _LL1758; _LL1756: old_elt_typ=
_temp1769; goto _LL1754; _LL1758: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1771=( char*)"exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1772; _temp1772.curr= _temp1771; _temp1772.base=
_temp1771; _temp1772.last_plus_one= _temp1771 + 52; _temp1772;})); goto _LL1754;
_LL1754:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* _temp1773= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1774; _LL1774: { struct Cyc_Absyn_Exp*
_temp1775= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1725), 0), 0); goto
_LL1776; _LL1776: { struct Cyc_Absyn_Exp* e1; if( _temp1433 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1775);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*) _check_null( _temp1433)); Cyc_Toc_exp_to_c( nv, r); e1=
Cyc_Toc_rmalloc_exp( r, _temp1775);}{ struct Cyc_Absyn_Exp* _temp1777= Cyc_Absyn_cast_exp(
_temp1773, e1, 0); goto _LL1778; _LL1778:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1747, _temp1773,( struct Cyc_Absyn_Exp*) _temp1777,
_temp1751, 0), 0))->r)); goto _LL1712;}}}}}}}} _LL1716: if( _temp1433 == 0){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1779=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1779->f1= Cyc_Toc_abs_ns; _temp1779->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1779;}), 0),({ struct Cyc_List_List* _temp1780=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1780->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1781=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1781[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1782; _temp1782.tag= Cyc_Absyn_Const_e; _temp1782.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1783=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1783[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1784; _temp1784.tag= Cyc_Absyn_String_c;
_temp1784.f1= _temp1729; _temp1784;}); _temp1783;})); _temp1782;}); _temp1781;}),
0); _temp1780->tl= 0; _temp1780;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( _temp1433)); Cyc_Toc_exp_to_c(
nv, r);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1785=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1785->f1= Cyc_Toc_abs_ns; _temp1785->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1785;}), 0),({ struct Cyc_List_List* _temp1786=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1786->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1787=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1787[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1788; _temp1788.tag= Cyc_Absyn_Const_e; _temp1788.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1789=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1789[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1790; _temp1790.tag= Cyc_Absyn_String_c;
_temp1790.f1= _temp1729; _temp1790;}); _temp1789;})); _temp1788;}); _temp1787;}),
0); _temp1786->tl= 0; _temp1786;}), 0))->r));} goto _LL1712; _LL1718: { int
is_tagged_ptr= 0;{ void* _temp1791= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1797; struct Cyc_Absyn_Conref* _temp1799; struct Cyc_Absyn_Tqual _temp1801;
struct Cyc_Absyn_Conref* _temp1803; void* _temp1805; void* _temp1807; _LL1793:
if(( unsigned int) _temp1791 > 4u?*(( int*) _temp1791) == Cyc_Absyn_PointerType:
0){ _LL1798: _temp1797=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1791)->f1; _LL1808: _temp1807=( void*) _temp1797.elt_typ; goto _LL1806;
_LL1806: _temp1805=( void*) _temp1797.rgn_typ; goto _LL1804; _LL1804: _temp1803=(
struct Cyc_Absyn_Conref*) _temp1797.nullable; goto _LL1802; _LL1802: _temp1801=(
struct Cyc_Absyn_Tqual) _temp1797.tq; goto _LL1800; _LL1800: _temp1799=( struct
Cyc_Absyn_Conref*) _temp1797.bounds; goto _LL1794;} else{ goto _LL1795;} _LL1795:
goto _LL1796; _LL1794:{ void* _temp1809=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1799))->v; void*
_temp1815; _LL1811: if(( unsigned int) _temp1809 > 1u?*(( int*) _temp1809) ==
Cyc_Absyn_Eq_constr: 0){ _LL1816: _temp1815=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1809)->f1; if( _temp1815 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1812;}
else{ goto _LL1813;}} else{ goto _LL1813;} _LL1813: goto _LL1814; _LL1812:
is_tagged_ptr= 1; goto _LL1810; _LL1814: goto _LL1810; _LL1810:;} goto _LL1792;
_LL1796:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1817=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1818; _temp1818.curr= _temp1817; _temp1818.base=
_temp1817; _temp1818.last_plus_one= _temp1817 + 42; _temp1818;})); goto _LL1792;
_LL1792:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1731->topt))->v;
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1733);{ struct Cyc_Absyn_Stmt*
s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( a, 0), _temp1735, Cyc_Absyn_var_exp(
max, 0), _temp1731, Cyc_Absyn_skip_stmt( 0), 1); struct Cyc_List_List* decls=({
struct Cyc_List_List* _temp1845=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1845->hd=( void*)({ struct _tuple10* _temp1846=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1846->f1= max;
_temp1846->f2= Cyc_Absyn_uint_t; _temp1846->f3=( struct Cyc_Absyn_Exp*)
_temp1733; _temp1846;}); _temp1845->tl= 0; _temp1845;}); struct Cyc_Absyn_Exp*
ai; if( _temp1433 == 0){ ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{
struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1433)); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1819=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1819->hd=( void*)({ struct _tuple10*
_temp1820=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1820->f1=
a; _temp1820->f2= ptr_typ; _temp1820->f3=( struct Cyc_Absyn_Exp*) ainit;
_temp1820;}); _temp1819->tl= decls; _temp1819;}); if( is_tagged_ptr){ struct
_tuple0* _temp1821= Cyc_Toc_temp_var(); goto _LL1822; _LL1822: { void* _temp1823=
Cyc_Toc_typ_to_c( old_typ); goto _LL1824; _LL1824: { struct Cyc_Absyn_Exp*
_temp1831= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List* _temp1825=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1825->hd=(
void*)({ struct _tuple4* _temp1830=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1830->f1= 0; _temp1830->f2= Cyc_Absyn_var_exp( a, 0); _temp1830;});
_temp1825->tl=({ struct Cyc_List_List* _temp1826=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1826->hd=( void*)({ struct
_tuple4* _temp1829=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1829->f1= 0; _temp1829->f2= Cyc_Absyn_var_exp( a, 0); _temp1829;});
_temp1826->tl=({ struct Cyc_List_List* _temp1827=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1827->hd=( void*)({ struct
_tuple4* _temp1828=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1828->f1= 0; _temp1828->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1828;}); _temp1827->tl= 0; _temp1827;});
_temp1826;}); _temp1825;}), 0); goto _LL1832; _LL1832: decls=({ struct Cyc_List_List*
_temp1833=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1833->hd=( void*)({ struct _tuple10* _temp1834=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1834->f1= _temp1821; _temp1834->f2=
_temp1823; _temp1834->f3=( struct Cyc_Absyn_Exp*) _temp1831; _temp1834;});
_temp1833->tl= decls; _temp1833;}); s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( _temp1821, 0), 0), 0);}}} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List* _temp1835= decls; goto
_LL1836; _LL1836: for( 0; _temp1835 != 0; _temp1835=(( struct Cyc_List_List*)
_check_null( _temp1835))->tl){ struct Cyc_Absyn_Exp* _temp1839; void* _temp1841;
struct _tuple0* _temp1843; struct _tuple10 _temp1837=*(( struct _tuple10*)((
struct Cyc_List_List*) _check_null( _temp1835))->hd); _LL1844: _temp1843=
_temp1837.f1; goto _LL1842; _LL1842: _temp1841= _temp1837.f2; goto _LL1840;
_LL1840: _temp1839= _temp1837.f3; goto _LL1838; _LL1838: s= Cyc_Absyn_declare_stmt(
_temp1843, _temp1841, _temp1839, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r)); goto _LL1712;}}}} _LL1720:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1431->topt))->v, 1,
_temp1433, _temp1739, _temp1743))->r)); goto _LL1712; _LL1722:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1433, _temp1745))->r)); goto
_LL1712; _LL1724: { void* _temp1847=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1431->topt))->v; goto _LL1848; _LL1848: { struct _tuple0* _temp1849= Cyc_Toc_temp_var();
goto _LL1850; _LL1850: { struct Cyc_Absyn_Stmt* _temp1851= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( _temp1849, 0), 0); goto _LL1852; _LL1852: { struct Cyc_Absyn_Exp*
_temp1853= Cyc_Absyn_signed_int_exp( 0, 0); goto _LL1854; _LL1854: Cyc_Toc_exp_to_c(
nv, _temp1431); _temp1851= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1849, 0), _temp1853, 0), _temp1431, 0), _temp1851, 0);{
void* elt_typ= Cyc_Toc_typ_to_c( _temp1847); void* _temp1855= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1856; _LL1856: { struct Cyc_Absyn_Exp*
_temp1857= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); goto _LL1858; _LL1858: { struct
Cyc_Absyn_Exp* e1; if( _temp1433 == 0){ e1= Cyc_Toc_malloc_exp( _temp1847,
_temp1857);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1433)); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1857);}{ struct Cyc_Absyn_Exp* _temp1859= Cyc_Absyn_cast_exp( _temp1855, e1,
0); goto _LL1860; _LL1860:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1849, _temp1855,( struct Cyc_Absyn_Exp*) _temp1859,
_temp1851, 0), 0))->r)); goto _LL1712;}}}}}}}} _LL1712:;} goto _LL1298; _LL1334:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct* _temp1861=(
struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1861[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1862; _temp1862.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1862.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1435);
_temp1862;}); _temp1861;}))); goto _LL1298; _LL1336: Cyc_Toc_exp_to_c( nv,
_temp1437); goto _LL1298; _LL1338: { void* e1_typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1439->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1439);{ void* _temp1863= e1_typ; struct Cyc_Absyn_PtrInfo _temp1869; struct
Cyc_Absyn_Conref* _temp1871; struct Cyc_Absyn_Tqual _temp1873; struct Cyc_Absyn_Conref*
_temp1875; void* _temp1877; void* _temp1879; _LL1865: if(( unsigned int)
_temp1863 > 4u?*(( int*) _temp1863) == Cyc_Absyn_PointerType: 0){ _LL1870:
_temp1869=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1863)->f1; _LL1880: _temp1879=( void*) _temp1869.elt_typ; goto _LL1878;
_LL1878: _temp1877=( void*) _temp1869.rgn_typ; goto _LL1876; _LL1876: _temp1875=(
struct Cyc_Absyn_Conref*) _temp1869.nullable; goto _LL1874; _LL1874: _temp1873=(
struct Cyc_Absyn_Tqual) _temp1869.tq; goto _LL1872; _LL1872: _temp1871=( struct
Cyc_Absyn_Conref*) _temp1869.bounds; goto _LL1866;} else{ goto _LL1867;} _LL1867:
goto _LL1868; _LL1866:{ void* _temp1881=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1871); struct Cyc_Absyn_Exp* _temp1887; _LL1883: if(
_temp1881 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1884;} else{ goto _LL1885;}
_LL1885: if(( unsigned int) _temp1881 > 1u?*(( int*) _temp1881) == Cyc_Absyn_Upper_b:
0){ _LL1888: _temp1887=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1881)->f1; goto _LL1886;} else{ goto _LL1882;} _LL1884: { struct _tuple0* x=
Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void* _temp1889= Cyc_Toc_typ_to_c_array(
_temp1879); goto _LL1890; _LL1890: { void* tx= Cyc_Toc_add_tagged_ptr_type(
_temp1889, _temp1873); void* ta2= Cyc_Absyn_cstar_typ( _temp1889, _temp1873);
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
struct Cyc_Absyn_Exp*) _temp1439, s1, 0), 0))->r)); goto _LL1882;}} _LL1886: if(
Cyc_Toc_is_nullable( e1_typ)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1439->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1891=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1891->hd=(
void*) _temp1439; _temp1891->tl= 0; _temp1891;}), 0), 0), 0))->r));} else{
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, e1_typ,(
struct Cyc_Absyn_Exp*) _temp1439, s3, 0), 0))->r));}} goto _LL1882; _LL1882:;}
goto _LL1864; _LL1868:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1892=( char*)"exp_to_c: Deref: non-pointer";
struct _tagged_string _temp1893; _temp1893.curr= _temp1892; _temp1893.base=
_temp1892; _temp1893.last_plus_one= _temp1892 + 29; _temp1893;})); goto _LL1864;
_LL1864:;} goto _LL1298;} _LL1340: Cyc_Toc_exp_to_c( nv, _temp1443); if( Cyc_Toc_is_poly_project(
e)){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1298; _LL1342: Cyc_Toc_exp_to_c( nv,
_temp1447);{ int is_poly= Cyc_Toc_is_poly_project( e); if( Cyc_Toc_is_nullable((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1447->topt))->v)){ if( Cyc_Toc_use_runtimecheck_funs){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1447->topt))->v),
Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct
Cyc_List_List* _temp1894=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1894->hd=( void*) _temp1447; _temp1894->tl= 0; _temp1894;}), 0), 0),
_temp1445, 0))->r));} else{ struct _tuple0* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1447->topt))->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp1445, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1447, s3, 0), 0))->r));}} if( is_poly){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto
_LL1298;} _LL1344: { void* _temp1895= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1451->topt))->v); goto _LL1896; _LL1896: Cyc_Toc_exp_to_c( nv,
_temp1451); Cyc_Toc_exp_to_c( nv, _temp1449);{ void* _temp1897= _temp1895;
struct Cyc_List_List* _temp1905; struct Cyc_Absyn_PtrInfo _temp1907; struct Cyc_Absyn_Conref*
_temp1909; struct Cyc_Absyn_Tqual _temp1911; struct Cyc_Absyn_Conref* _temp1913;
void* _temp1915; void* _temp1917; _LL1899: if(( unsigned int) _temp1897 > 4u?*((
int*) _temp1897) == Cyc_Absyn_TupleType: 0){ _LL1906: _temp1905=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1897)->f1; goto _LL1900;} else{ goto
_LL1901;} _LL1901: if(( unsigned int) _temp1897 > 4u?*(( int*) _temp1897) == Cyc_Absyn_PointerType:
0){ _LL1908: _temp1907=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1897)->f1; _LL1918: _temp1917=( void*) _temp1907.elt_typ; goto _LL1916;
_LL1916: _temp1915=( void*) _temp1907.rgn_typ; goto _LL1914; _LL1914: _temp1913=(
struct Cyc_Absyn_Conref*) _temp1907.nullable; goto _LL1912; _LL1912: _temp1911=(
struct Cyc_Absyn_Tqual) _temp1907.tq; goto _LL1910; _LL1910: _temp1909=( struct
Cyc_Absyn_Conref*) _temp1907.bounds; goto _LL1902;} else{ goto _LL1903;} _LL1903:
goto _LL1904; _LL1900: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1449) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1451,({ struct _tagged_string* _temp1919=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1919[ 0]= xprintf("f%d",( int)
i); _temp1919;}), 0))->r)); goto _LL1898;} _LL1902:{ void* _temp1920=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1909); struct Cyc_Absyn_Exp*
_temp1926; _LL1922: if(( unsigned int) _temp1920 > 1u?*(( int*) _temp1920) ==
Cyc_Absyn_Upper_b: 0){ _LL1927: _temp1926=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1920)->f1; goto _LL1923;} else{ goto _LL1924;} _LL1924: if( _temp1920 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1925;} else{ goto _LL1921;} _LL1923:
_temp1926= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp( _temp1926), 0);{
int possibly_null;{ void* _temp1928=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1913))->v; int
_temp1934; _LL1930: if(( unsigned int) _temp1928 > 1u?*(( int*) _temp1928) ==
Cyc_Absyn_Eq_constr: 0){ _LL1935: _temp1934=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1928)->f1; goto _LL1931;} else{ goto _LL1932;} _LL1932: goto _LL1933;
_LL1931: possibly_null= _temp1934; goto _LL1929; _LL1933: possibly_null= 0; goto
_LL1929; _LL1929:;}{ void* ta1= Cyc_Toc_typ_to_c( _temp1917); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1911); if( Cyc_Toc_use_runtimecheck_funs){ if( possibly_null){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv, 0),({ struct Cyc_List_List*
_temp1936=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1936->hd=( void*) _temp1451; _temp1936->tl=({ struct Cyc_List_List*
_temp1937=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1937->hd=( void*) _temp1926; _temp1937->tl=({ struct Cyc_List_List*
_temp1938=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1938->hd=( void*) Cyc_Absyn_sizeoftyp_exp( ta1, 0); _temp1938->tl=({ struct
Cyc_List_List* _temp1939=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1939->hd=( void*) _temp1449; _temp1939->tl= 0; _temp1939;}); _temp1938;});
_temp1937;}); _temp1936;}), 0), 0), 0))->r));} else{( void*)( _temp1449->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List* _temp1940=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1940->hd=( void*) _temp1926; _temp1940->tl=({
struct Cyc_List_List* _temp1941=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1941->hd=( void*) Cyc_Absyn_copy_exp( _temp1449);
_temp1941->tl= 0; _temp1941;}); _temp1940;}), 0))->r));}} else{ struct _tuple0*
a= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte, Cyc_Absyn_var_exp( i, 0),
_temp1926, 0); if( possibly_null){ e3= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_new_exp(( void*) e3->r, 0),
0);}{ struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt(
i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) _temp1449, s3, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*)
_temp1451, s1, 0), 0))->r));}} goto _LL1921;}} _LL1925: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp1917); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1911); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp1948=({ struct Cyc_List_List* _temp1942=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1942->hd=( void*)({ struct
_tuple1* _temp1947=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1947->f1= 0; _temp1947->f2= Cyc_Toc_mt_tq; _temp1947->f3= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1451->topt))->v); _temp1947;});
_temp1942->tl=({ struct Cyc_List_List* _temp1943=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1943->hd=( void*)({ struct
_tuple1* _temp1946=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1946->f1= 0; _temp1946->f2= Cyc_Toc_mt_tq; _temp1946->f3= Cyc_Absyn_uint_t;
_temp1946;}); _temp1943->tl=({ struct Cyc_List_List* _temp1944=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1944->hd=( void*)({ struct
_tuple1* _temp1945=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1945->f1= 0; _temp1945->f2= Cyc_Toc_mt_tq; _temp1945->f3= Cyc_Absyn_uint_t;
_temp1945;}); _temp1944->tl= 0; _temp1944;}); _temp1943;}); _temp1942;}); goto
_LL1949; _LL1949: { void* _temp1953= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1950=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1950[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1951; _temp1951.tag= Cyc_Absyn_FnType;
_temp1951.f1=({ struct Cyc_Absyn_FnInfo _temp1952; _temp1952.tvars= 0; _temp1952.effect=
0; _temp1952.ret_typ=( void*) ta2; _temp1952.args= _temp1948; _temp1952.varargs=
0; _temp1952.attributes= 0; _temp1952;}); _temp1951;}); _temp1950;}), Cyc_Toc_mt_tq);
goto _LL1954; _LL1954: { struct Cyc_Absyn_Exp* fnexp= Cyc_Absyn_cast_exp(
_temp1953, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv, 0), 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_fncall_exp( fnexp,({
struct Cyc_List_List* _temp1955=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1955->hd=( void*) _temp1451; _temp1955->tl=({
struct Cyc_List_List* _temp1956=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1956->hd=( void*) Cyc_Absyn_sizeoftyp_exp( ta1, 0);
_temp1956->tl=({ struct Cyc_List_List* _temp1957=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1957->hd=( void*) _temp1449;
_temp1957->tl= 0; _temp1957;}); _temp1956;}); _temp1955;}), 0), 0))->r));}}}
else{ struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type(
ta1, _temp1911); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1449, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1451, s1, 0), 0))->r));} goto _LL1921;} _LL1921:;}
goto _LL1898; _LL1904:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1958=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1959; _temp1959.curr= _temp1958; _temp1959.base=
_temp1958; _temp1959.last_plus_one= _temp1958 + 49; _temp1959;})); goto _LL1898;
_LL1898:;} goto _LL1298;} _LL1346: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1453))->r));} else{ struct Cyc_List_List*
_temp1960=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1453); goto
_LL1961; _LL1961: { struct _tagged_string* _temp1962= Cyc_Toc_add_tuple_type(
_temp1960); goto _LL1963; _LL1963: { struct Cyc_List_List* dles= 0;{ int i= 1;
for( 0; _temp1453 != 0;( _temp1453=(( struct Cyc_List_List*) _check_null(
_temp1453))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1453))->hd);{ struct Cyc_List_List* _temp1968=({
struct Cyc_List_List* _temp1964=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1964->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct*
_temp1965=( struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp1965[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp1966; _temp1966.tag= Cyc_Absyn_FieldName;
_temp1966.f1=({ struct _tagged_string* _temp1967=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1967[ 0]= xprintf("f%d", i);
_temp1967;}); _temp1966;}); _temp1965;})); _temp1964->tl= 0; _temp1964;}); goto
_LL1969; _LL1969: dles=({ struct Cyc_List_List* _temp1970=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1970->hd=( void*)({ struct
_tuple4* _temp1971=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1971->f1=( struct Cyc_List_List*) _temp1968; _temp1971->f2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1453))->hd; _temp1971;}); _temp1970->tl=
dles; _temp1970;});}}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_strct( _temp1962), Cyc_Absyn_unresolvedmem_exp( 0, dles, 0), 0))->r));}}}
goto _LL1298; _LL1348:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1972=( char*)"compoundlit"; struct
_tagged_string _temp1973; _temp1973.curr= _temp1972; _temp1973.base= _temp1972;
_temp1973.last_plus_one= _temp1972 + 12; _temp1973;})); goto _LL1298; _LL1350:(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1459, 0))->r));{
struct Cyc_List_List* _temp1974= _temp1459; goto _LL1975; _LL1975: for( 0;
_temp1974 != 0; _temp1974=(( struct Cyc_List_List*) _check_null( _temp1974))->tl){
struct _tuple4 _temp1978; struct Cyc_Absyn_Exp* _temp1979; struct Cyc_List_List*
_temp1981; struct _tuple4* _temp1976=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1974))->hd; _temp1978=* _temp1976; _LL1982: _temp1981=
_temp1978.f1; goto _LL1980; _LL1980: _temp1979= _temp1978.f2; goto _LL1977;
_LL1977: Cyc_Toc_exp_to_c( nv, _temp1979);}} goto _LL1298; _LL1352: {
unsigned int _temp1983= Cyc_Evexp_eval_const_uint_exp( _temp1463); goto _LL1984;
_LL1984: Cyc_Toc_exp_to_c( nv, _temp1461);{ struct Cyc_List_List* es= 0; if( !
Cyc_Toc_is_zero( _temp1461)){ unsigned int i= 0; for( 0; i < _temp1983; i ++){
es=({ struct Cyc_List_List* _temp1985=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1985->hd=( void*)({ struct _tuple4*
_temp1986=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1986->f1=
0; _temp1986->f2= _temp1461; _temp1986;}); _temp1985->tl= es; _temp1985;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL1298;}} _LL1354: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1469, _temp1473))->r));} else{
struct _tuple0* _temp1987= _temp1473; goto _LL1988; _LL1988: { struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Structdecl*) _check_null(
_temp1467 == 0?(( struct Cyc_Absyn_Structdecl*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp2001= Cyc_Absynpp_qvar2string( _temp1473); xprintf("no structdecl %.*s",
_temp2001.last_plus_one - _temp2001.curr, _temp2001.curr);})): _temp1467));
struct _RegionHandle _temp1989= _new_region(); struct _RegionHandle* rgn=&
_temp1989; _push_region( rgn);{ struct Cyc_List_List* _temp1990=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1469,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Structdecl*) sd2)->fields))->v); goto _LL1991; _LL1991: for( 0;
_temp1990 != 0; _temp1990=(( struct Cyc_List_List*) _check_null( _temp1990))->tl){
struct _tuple12 _temp1994; struct Cyc_Absyn_Exp* _temp1995; struct Cyc_Absyn_Structfield*
_temp1997; struct _tuple12* _temp1992=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( _temp1990))->hd; _temp1994=* _temp1992; _LL1998: _temp1997=
_temp1994.f1; goto _LL1996; _LL1996: _temp1995= _temp1994.f2; goto _LL1993;
_LL1993: { void* _temp1999=( void*) _temp1997->type; goto _LL2000; _LL2000: Cyc_Toc_exp_to_c(
nv, _temp1995); if( Cyc_Toc_is_void_star( _temp1999)){( void*)( _temp1995->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp1995->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_strctq( _temp1987), Cyc_Absyn_unresolvedmem_exp( 0, _temp1469, 0), 0))->r));};
_pop_region(& _temp1989);}} goto _LL1298; _LL1356: { struct Cyc_List_List* fs;{
void* _temp2002= Cyc_Tcutil_compress( _temp1477); struct Cyc_List_List*
_temp2008; _LL2004: if(( unsigned int) _temp2002 > 4u?*(( int*) _temp2002) ==
Cyc_Absyn_AnonStructType: 0){ _LL2009: _temp2008=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp2002)->f1; goto _LL2005;} else{
goto _LL2006;} _LL2006: goto _LL2007; _LL2005: fs= _temp2008; goto _LL2003;
_LL2007: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp2010= Cyc_Absynpp_typ2string( _temp1477); xprintf("anon struct has type %.*s",
_temp2010.last_plus_one - _temp2010.curr, _temp2010.curr);})); goto _LL2003;
_LL2003:;}{ struct _RegionHandle _temp2011= _new_region(); struct _RegionHandle*
rgn=& _temp2011; _push_region( rgn);{ struct Cyc_List_List* _temp2012=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1475, fs); goto _LL2013; _LL2013: for( 0; _temp2012 != 0;
_temp2012=(( struct Cyc_List_List*) _check_null( _temp2012))->tl){ struct
_tuple12 _temp2016; struct Cyc_Absyn_Exp* _temp2017; struct Cyc_Absyn_Structfield*
_temp2019; struct _tuple12* _temp2014=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( _temp2012))->hd; _temp2016=* _temp2014; _LL2020: _temp2019=
_temp2016.f1; goto _LL2018; _LL2018: _temp2017= _temp2016.f2; goto _LL2015;
_LL2015: { void* _temp2021=( void*) _temp2019->type; goto _LL2022; _LL2022: Cyc_Toc_exp_to_c(
nv, _temp2017); if( Cyc_Toc_is_void_star( _temp2021)){( void*)( _temp2017->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp2017->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1475, 0))->r));}; _pop_region(& _temp2011);} goto _LL1298;} _LL1358: {
struct _tuple0* qv= _temp1479->name; if( _temp1481->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp2023= _temp1481->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1481->fields))->v;
goto _LL2024; _LL2024: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2023))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp2023))->hd)->typs
== 0){ tag_count ++;} _temp2023=(( struct Cyc_List_List*) _check_null( _temp2023))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1298;} _LL1360: { struct _tuple0* _temp2025= _temp1489->name; goto
_LL2026; _LL2026: { struct Cyc_List_List* _temp2027= _temp1489->typs; goto
_LL2028; _LL2028: { struct _tuple0* _temp2029= Cyc_Toc_temp_var(); goto _LL2030;
_LL2030: { struct Cyc_Absyn_Exp* _temp2031= Cyc_Absyn_var_exp( _temp2029, 0);
goto _LL2032; _LL2032: { void* _temp2035= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2025,( struct _tagged_string)({ char* _temp2033=( char*)"_struct"; struct
_tagged_string _temp2034; _temp2034.curr= _temp2033; _temp2034.base= _temp2033;
_temp2034.last_plus_one= _temp2033 + 8; _temp2034;}))); goto _LL2036; _LL2036:
if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp; if( _temp1491->is_xtunion){
tag_exp= Cyc_Absyn_var_exp( _temp2025, 0);} else{ int tag_count= 0; struct Cyc_List_List*
_temp2037= _temp1491->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1491->fields))->v; goto _LL2038; _LL2038: while( Cyc_Absyn_qvar_cmp(
_temp2025,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2037))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2037))->hd)->typs != 0){ tag_count ++;}
_temp2037=(( struct Cyc_List_List*) _check_null( _temp2037))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2039= 0; goto _LL2040;
_LL2040: for( 0; _temp1493 != 0;( _temp1493=(( struct Cyc_List_List*)
_check_null( _temp1493))->tl, _temp2027=(( struct Cyc_List_List*) _check_null(
_temp2027))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1493))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2027))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2039=({ struct Cyc_List_List* _temp2041=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2041[ 0]=({ struct Cyc_List_List
_temp2042; _temp2042.hd=( void*)({ struct _tuple4* _temp2043=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2043[ 0]=({ struct _tuple4
_temp2044; _temp2044.f1= 0; _temp2044.f2= cur_e; _temp2044;}); _temp2043;});
_temp2042.tl= _temp2039; _temp2042;}); _temp2041;});} _temp2039=({ struct Cyc_List_List*
_temp2045=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2045[ 0]=({ struct Cyc_List_List _temp2046; _temp2046.hd=( void*)({ struct
_tuple4* _temp2047=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2047[ 0]=({ struct _tuple4 _temp2048; _temp2048.f1= 0; _temp2048.f2=
tag_exp; _temp2048;}); _temp2047;}); _temp2046.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2039); _temp2046;}); _temp2045;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2049=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2049->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2025,( struct
_tagged_string)({ char* _temp2050=( char*)"_struct"; struct _tagged_string
_temp2051; _temp2051.curr= _temp2050; _temp2051.base= _temp2050; _temp2051.last_plus_one=
_temp2050 + 8; _temp2051;})); _temp2049;}), _temp2039, 0))->r));}} else{ struct
Cyc_List_List* _temp2053=({ struct Cyc_List_List* _temp2052=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2052->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2031, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp2025, 0), 0); _temp2052->tl= 0; _temp2052;}); goto _LL2054; _LL2054:{ int i=
1; for( 0; _temp1493 != 0;((( _temp1493=(( struct Cyc_List_List*) _check_null(
_temp1493))->tl, i ++)), _temp2027=(( struct Cyc_List_List*) _check_null(
_temp2027))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1493))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2027))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp2056= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2031,({ struct _tagged_string* _temp2055=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2055[ 0]= xprintf("f%d", i); _temp2055;}), 0), cur_e, 0); goto _LL2057;
_LL2057: _temp2053=({ struct Cyc_List_List* _temp2058=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2058->hd=( void*) _temp2056;
_temp2058->tl= _temp2053; _temp2058;});}}}{ struct Cyc_Absyn_Stmt* _temp2059=
Cyc_Absyn_exp_stmt( _temp2031, 0); goto _LL2060; _LL2060: { struct Cyc_Absyn_Stmt*
_temp2062= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp2061=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2061->hd=( void*) _temp2059;
_temp2061->tl= _temp2053; _temp2061;})), 0); goto _LL2063; _LL2063:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp2029,
_temp2035, 0, _temp2062, 0), 0))->r));}}} goto _LL1298;}}}}} _LL1362: goto
_LL1298; _LL1364: { void* t_c= Cyc_Toc_typ_to_c( _temp1505); if( _temp1507 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1507)); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)((
void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1505, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} goto _LL1298;} _LL1366: Cyc_Toc_stmt_to_c( nv, _temp1509); goto
_LL1298; _LL1368:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2064=( char*)"UnresolvedMem"; struct
_tagged_string _temp2065; _temp2065.curr= _temp2064; _temp2065.base= _temp2064;
_temp2065.last_plus_one= _temp2064 + 14; _temp2065;})); goto _LL1298; _LL1370:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp2066=( char*)"codegen"; struct _tagged_string _temp2067; _temp2067.curr=
_temp2066; _temp2067.base= _temp2066; _temp2067.last_plus_one= _temp2066 + 8;
_temp2067;})); goto _LL1298; _LL1372:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2068=( char*)"fill"; struct _tagged_string
_temp2069; _temp2069.curr= _temp2068; _temp2069.base= _temp2068; _temp2069.last_plus_one=
_temp2068 + 5; _temp2069;})); goto _LL1298; _LL1298:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple14{ struct _tuple0* f1; void* f2;
} ; struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, void* t,
struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat* p,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp2070=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp2106; struct Cyc_Absyn_Vardecl _temp2108; struct Cyc_List_List* _temp2109;
struct Cyc_Core_Opt* _temp2111; struct Cyc_Absyn_Exp* _temp2113; void* _temp2115;
struct Cyc_Absyn_Tqual _temp2117; struct _tuple0* _temp2119; void* _temp2121;
struct Cyc_Absyn_Vardecl* _temp2123; int _temp2125; void* _temp2127; char
_temp2129; struct _tagged_string _temp2131; struct Cyc_Absyn_Enumfield*
_temp2133; struct Cyc_Absyn_Enumdecl* _temp2135; struct Cyc_List_List* _temp2137;
struct Cyc_List_List* _temp2139; struct Cyc_Absyn_Tunionfield* _temp2141; struct
Cyc_Absyn_Tuniondecl* _temp2143; struct Cyc_Absyn_Pat* _temp2146; struct Cyc_Absyn_Pat
_temp2148; struct Cyc_Position_Segment* _temp2149; struct Cyc_Core_Opt*
_temp2151; void* _temp2153; struct Cyc_List_List* _temp2155; struct Cyc_List_List*
_temp2157; struct Cyc_Absyn_Tunionfield* _temp2159; struct Cyc_Absyn_Tuniondecl*
_temp2161; struct Cyc_List_List* _temp2163; struct Cyc_List_List* _temp2165;
struct Cyc_Absyn_Tunionfield* _temp2167; struct Cyc_Absyn_Tuniondecl* _temp2169;
struct Cyc_List_List* _temp2171; struct Cyc_List_List* _temp2173; struct Cyc_List_List*
_temp2175; struct Cyc_Core_Opt* _temp2177; struct Cyc_Absyn_Structdecl*
_temp2179; struct Cyc_Absyn_Pat* _temp2181; struct _tuple0* _temp2183; struct
Cyc_List_List* _temp2185; struct Cyc_List_List* _temp2187; struct _tuple0*
_temp2189; struct Cyc_List_List* _temp2191; struct Cyc_List_List* _temp2193;
struct _tuple0* _temp2195; _LL2072: if(( unsigned int) _temp2070 > 2u?*(( int*)
_temp2070) == Cyc_Absyn_Var_p: 0){ _LL2107: _temp2106=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp2070)->f1; _temp2108=* _temp2106; _LL2122:
_temp2121=( void*) _temp2108.sc; goto _LL2120; _LL2120: _temp2119=( struct
_tuple0*) _temp2108.name; goto _LL2118; _LL2118: _temp2117=( struct Cyc_Absyn_Tqual)
_temp2108.tq; goto _LL2116; _LL2116: _temp2115=( void*) _temp2108.type; goto
_LL2114; _LL2114: _temp2113=( struct Cyc_Absyn_Exp*) _temp2108.initializer; goto
_LL2112; _LL2112: _temp2111=( struct Cyc_Core_Opt*) _temp2108.rgn; goto _LL2110;
_LL2110: _temp2109=( struct Cyc_List_List*) _temp2108.attributes; goto _LL2073;}
else{ goto _LL2074;} _LL2074: if( _temp2070 ==( void*) Cyc_Absyn_Wild_p){ goto
_LL2075;} else{ goto _LL2076;} _LL2076: if(( unsigned int) _temp2070 > 2u?*((
int*) _temp2070) == Cyc_Absyn_Reference_p: 0){ _LL2124: _temp2123=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Reference_p_struct*) _temp2070)->f1; goto _LL2077;} else{ goto
_LL2078;} _LL2078: if( _temp2070 ==( void*) Cyc_Absyn_Null_p){ goto _LL2079;}
else{ goto _LL2080;} _LL2080: if(( unsigned int) _temp2070 > 2u?*(( int*)
_temp2070) == Cyc_Absyn_Int_p: 0){ _LL2128: _temp2127=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp2070)->f1; goto _LL2126; _LL2126: _temp2125=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp2070)->f2; goto _LL2081;} else{ goto _LL2082;} _LL2082: if(( unsigned int)
_temp2070 > 2u?*(( int*) _temp2070) == Cyc_Absyn_Char_p: 0){ _LL2130: _temp2129=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp2070)->f1; goto _LL2083;} else{
goto _LL2084;} _LL2084: if(( unsigned int) _temp2070 > 2u?*(( int*) _temp2070)
== Cyc_Absyn_Float_p: 0){ _LL2132: _temp2131=( struct _tagged_string)(( struct
Cyc_Absyn_Float_p_struct*) _temp2070)->f1; goto _LL2085;} else{ goto _LL2086;}
_LL2086: if(( unsigned int) _temp2070 > 2u?*(( int*) _temp2070) == Cyc_Absyn_Enum_p:
0){ _LL2136: _temp2135=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp2070)->f1; goto _LL2134; _LL2134: _temp2133=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp2070)->f2; goto _LL2087;} else{ goto
_LL2088;} _LL2088: if(( unsigned int) _temp2070 > 2u?*(( int*) _temp2070) == Cyc_Absyn_Tunion_p:
0){ _LL2144: _temp2143=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2070)->f1; goto _LL2142; _LL2142: _temp2141=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2070)->f2; goto _LL2140; _LL2140:
_temp2139=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2070)->f3; goto _LL2138; _LL2138: _temp2137=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2070)->f4; if( _temp2137 == 0){ goto
_LL2089;} else{ goto _LL2090;}} else{ goto _LL2090;} _LL2090: if(( unsigned int)
_temp2070 > 2u?*(( int*) _temp2070) == Cyc_Absyn_Pointer_p: 0){ _LL2147:
_temp2146=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2070)->f1; _temp2148=* _temp2146; _LL2154: _temp2153=( void*) _temp2148.r;
if(( unsigned int) _temp2153 > 2u?*(( int*) _temp2153) == Cyc_Absyn_Tunion_p: 0){
_LL2162: _temp2161=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2153)->f1; goto _LL2160; _LL2160: _temp2159=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2153)->f2; goto _LL2158; _LL2158:
_temp2157=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2153)->f3; goto _LL2156; _LL2156: _temp2155=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2153)->f4; goto _LL2152;} else{ goto
_LL2092;} _LL2152: _temp2151=( struct Cyc_Core_Opt*) _temp2148.topt; goto
_LL2150; _LL2150: _temp2149=( struct Cyc_Position_Segment*) _temp2148.loc; goto
_LL2145;} else{ goto _LL2092;} _LL2145: if( _temp2155 != 0){ goto _LL2091;}
else{ goto _LL2092;} _LL2092: if(( unsigned int) _temp2070 > 2u?*(( int*)
_temp2070) == Cyc_Absyn_Tunion_p: 0){ _LL2170: _temp2169=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2070)->f1; goto _LL2168; _LL2168:
_temp2167=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2070)->f2; goto _LL2166; _LL2166: _temp2165=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2070)->f3; goto _LL2164; _LL2164:
_temp2163=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2070)->f4; goto _LL2093;} else{ goto _LL2094;} _LL2094: if(( unsigned int)
_temp2070 > 2u?*(( int*) _temp2070) == Cyc_Absyn_Tuple_p: 0){ _LL2172: _temp2171=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp2070)->f1; goto
_LL2095;} else{ goto _LL2096;} _LL2096: if(( unsigned int) _temp2070 > 2u?*((
int*) _temp2070) == Cyc_Absyn_Struct_p: 0){ _LL2180: _temp2179=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp2070)->f1; goto _LL2178; _LL2178:
_temp2177=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp2070)->f2;
goto _LL2176; _LL2176: _temp2175=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp2070)->f3; goto _LL2174; _LL2174: _temp2173=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp2070)->f4; goto _LL2097;} else{ goto
_LL2098;} _LL2098: if(( unsigned int) _temp2070 > 2u?*(( int*) _temp2070) == Cyc_Absyn_Pointer_p:
0){ _LL2182: _temp2181=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2070)->f1; goto _LL2099;} else{ goto _LL2100;} _LL2100: if(( unsigned int)
_temp2070 > 2u?*(( int*) _temp2070) == Cyc_Absyn_UnknownId_p: 0){ _LL2184:
_temp2183=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2070)->f1;
goto _LL2101;} else{ goto _LL2102;} _LL2102: if(( unsigned int) _temp2070 > 2u?*((
int*) _temp2070) == Cyc_Absyn_UnknownCall_p: 0){ _LL2190: _temp2189=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp2070)->f1; goto _LL2188;
_LL2188: _temp2187=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2070)->f2; goto _LL2186; _LL2186: _temp2185=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp2070)->f3; goto _LL2103;} else{
goto _LL2104;} _LL2104: if(( unsigned int) _temp2070 > 2u?*(( int*) _temp2070)
== Cyc_Absyn_UnknownFields_p: 0){ _LL2196: _temp2195=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp2070)->f1; goto _LL2194; _LL2194:
_temp2193=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2070)->f2; goto _LL2192; _LL2192: _temp2191=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp2070)->f3; goto _LL2105;} else{
goto _LL2071;} _LL2073: nv= Cyc_Toc_add_varmap( nv, _temp2119, r); goto _LL2075;
_LL2075: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL2071; _LL2077: { struct
_tuple0* _temp2197= Cyc_Toc_temp_var(); goto _LL2198; _LL2198: decls=({ struct
Cyc_List_List* _temp2199=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2199->hd=( void*)({ struct _tuple14* _temp2200=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2200->f1= _temp2197; _temp2200->f2=
Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2200;});
_temp2199->tl= decls; _temp2199;}); nv= Cyc_Toc_add_varmap( nv, _temp2123->name,
Cyc_Absyn_var_exp( _temp2197, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2197, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2071;} _LL2079: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2071; _LL2081: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2127, _temp2125, 0), succ_lab, fail_lab); goto _LL2071;
_LL2083: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2129, 0),
succ_lab, fail_lab); goto _LL2071; _LL2085: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2131, 0), succ_lab, fail_lab); goto _LL2071; _LL2087: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2201=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2201[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2202; _temp2202.tag= Cyc_Absyn_Enum_e;
_temp2202.f1= _temp2133->name; _temp2202.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2135; _temp2202.f3=( struct Cyc_Absyn_Enumfield*) _temp2133; _temp2202;});
_temp2201;}), 0), succ_lab, fail_lab); goto _LL2071; _LL2089: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2141->name, 0); if( ! _temp2143->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2071;} _LL2091: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2155);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2159->name,( struct
_tagged_string)({ char* _temp2232=( char*)"_struct"; struct _tagged_string
_temp2233; _temp2233.curr= _temp2232; _temp2233.base= _temp2232; _temp2233.last_plus_one=
_temp2232 + 8; _temp2233;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0);{ struct
Cyc_List_List* _temp2203=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp2155); goto _LL2204; _LL2204: for( 0; _temp2203 != 0;(
_temp2203=(( struct Cyc_List_List*) _check_null( _temp2203))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2205=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2203))->hd; goto _LL2206; _LL2206: { struct _tuple0* _temp2207=
Cyc_Toc_temp_var(); goto _LL2208; _LL2208: { void* _temp2209=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2205->topt))->v; goto _LL2210; _LL2210: { void*
_temp2211= Cyc_Toc_typ_to_c( _temp2209); goto _LL2212; _LL2212: { struct
_tagged_string* _temp2213= Cyc_Toc_fresh_label(); goto _LL2214; _LL2214: decls=({
struct Cyc_List_List* _temp2215=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2215->hd=( void*)({ struct _tuple14* _temp2216=(
struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2216->f1= _temp2207;
_temp2216->f2= _temp2211; _temp2216;}); _temp2215->tl= decls; _temp2215;});{
struct _tuple13 _temp2218= Cyc_Toc_xlate_pat( nv, _temp2209, Cyc_Absyn_var_exp(
_temp2207, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string*
_temp2217=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2217[ 0]= xprintf("f%d", cnt); _temp2217;}), 0), _temp2205, succ_lab,
fail_lab, decls); goto _LL2219; _LL2219: nv= _temp2218.f1; decls= _temp2218.f2;{
struct Cyc_Absyn_Stmt* _temp2220= _temp2218.f3; goto _LL2221; _LL2221: { struct
Cyc_Absyn_Stmt* _temp2223= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2207,
0), Cyc_Absyn_cast_exp( _temp2211, Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp2222=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2222[ 0]= xprintf("f%d", cnt); _temp2222;}), 0), 0),
0); goto _LL2224; _LL2224: { struct Cyc_Absyn_Stmt* _temp2225= Cyc_Absyn_seq_stmt(
_temp2223, _temp2220, 0); goto _LL2226; _LL2226: ss=({ struct Cyc_List_List*
_temp2227=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2227->hd=( void*) Cyc_Absyn_label_stmt( _temp2213, _temp2225, 0); _temp2227->tl=
ss; _temp2227;}); succ_lab= _temp2213;}}}}}}}}}}{ struct Cyc_Absyn_Exp* test_exp;
if( _temp2161->is_xtunion){ struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp(
e2, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2159->name, 0);
test_exp= Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e, 0), Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);}
else{ unsigned int max_tag= 0;{ struct Cyc_List_List* _temp2228=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2161->fields))->v; goto _LL2229; _LL2229:
for( 0; _temp2228 != 0; _temp2228=(( struct Cyc_List_List*) _check_null(
_temp2228))->tl){ struct Cyc_Absyn_Tunionfield* _temp2230=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2228))->hd; goto _LL2231; _LL2231: if(
_temp2230->typs == 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp(
max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp(
e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2159->name,
0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0):
Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2071;}} _LL2093:
_temp2171= _temp2163; goto _LL2095; _LL2095: { struct Cyc_List_List* ss= 0; int
cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2171);{ struct
Cyc_List_List* _temp2234=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp2171); goto _LL2235; _LL2235: for( 0; _temp2234 != 0;(
_temp2234=(( struct Cyc_List_List*) _check_null( _temp2234))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2236=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2234))->hd; goto _LL2237; _LL2237: { struct _tuple0* _temp2238=
Cyc_Toc_temp_var(); goto _LL2239; _LL2239: { void* _temp2240=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2236->topt))->v; goto _LL2241; _LL2241: {
struct _tagged_string* _temp2242= Cyc_Toc_fresh_label(); goto _LL2243; _LL2243:
decls=({ struct Cyc_List_List* _temp2244=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2244->hd=( void*)({ struct _tuple14*
_temp2245=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2245->f1=
_temp2238; _temp2245->f2= Cyc_Toc_typ_to_c( _temp2240); _temp2245;}); _temp2244->tl=
decls; _temp2244;});{ struct _tuple13 _temp2247= Cyc_Toc_xlate_pat( nv,
_temp2240, Cyc_Absyn_var_exp( _temp2238, 0), Cyc_Absyn_structmember_exp( path,({
struct _tagged_string* _temp2246=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2246[ 0]= xprintf("f%d", cnt); _temp2246;}), 0),
_temp2236, succ_lab, fail_lab, decls); goto _LL2248; _LL2248: nv= _temp2247.f1;
decls= _temp2247.f2;{ struct Cyc_Absyn_Stmt* _temp2249= _temp2247.f3; goto
_LL2250; _LL2250: { struct Cyc_Absyn_Stmt* _temp2252= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2238, 0), Cyc_Absyn_structmember_exp( r,({ struct
_tagged_string* _temp2251=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2251[ 0]= xprintf("f%d", cnt); _temp2251;}), 0), 0),
_temp2249, 0); goto _LL2253; _LL2253: ss=({ struct Cyc_List_List* _temp2254=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2254->hd=(
void*) Cyc_Absyn_label_stmt( _temp2242, _temp2252, 0); _temp2254->tl= ss;
_temp2254;}); succ_lab= _temp2242;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2071;} _LL2097: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2255=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2173); goto _LL2256; _LL2256: for( 0; _temp2255 != 0; _temp2255=(( struct
Cyc_List_List*) _check_null( _temp2255))->tl){ struct _tuple15* _temp2257=(
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2255))->hd; goto
_LL2258; _LL2258: { struct Cyc_Absyn_Pat* _temp2259=(* _temp2257).f2; goto
_LL2260; _LL2260: { struct _tagged_string* f;{ void* _temp2261=( void*)(( struct
Cyc_List_List*) _check_null((* _temp2257).f1))->hd; struct _tagged_string*
_temp2267; _LL2263: if(*(( int*) _temp2261) == Cyc_Absyn_FieldName){ _LL2268:
_temp2267=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp2261)->f1; goto _LL2264;} else{ goto _LL2265;} _LL2265: goto _LL2266;
_LL2264: f= _temp2267; goto _LL2262; _LL2266:( void) _throw(( void*) Cyc_Toc_Match_error);
_LL2262:;}{ struct _tuple0* _temp2269= Cyc_Toc_temp_var(); goto _LL2270; _LL2270: {
void* _temp2271=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2259->topt))->v;
goto _LL2272; _LL2272: { void* _temp2273= Cyc_Toc_typ_to_c( _temp2271); goto
_LL2274; _LL2274: { struct _tagged_string* _temp2275= Cyc_Toc_fresh_label();
goto _LL2276; _LL2276: decls=({ struct Cyc_List_List* _temp2277=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2277->hd=( void*)({ struct
_tuple14* _temp2278=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp2278->f1= _temp2269; _temp2278->f2= _temp2273; _temp2278;}); _temp2277->tl=
decls; _temp2277;});{ struct _tuple13 _temp2279= Cyc_Toc_xlate_pat( nv,
_temp2271, Cyc_Absyn_var_exp( _temp2269, 0), Cyc_Absyn_structmember_exp( path, f,
0), _temp2259, succ_lab, fail_lab, decls); goto _LL2280; _LL2280: nv= _temp2279.f1;
decls= _temp2279.f2;{ struct Cyc_Absyn_Stmt* _temp2281= _temp2279.f3; goto
_LL2282; _LL2282: { struct Cyc_Absyn_Stmt* _temp2283= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2269, 0), Cyc_Absyn_cast_exp( _temp2273, Cyc_Absyn_structmember_exp(
r, f, 0), 0), 0), _temp2281, 0); goto _LL2284; _LL2284: ss=({ struct Cyc_List_List*
_temp2285=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2285->hd=( void*) Cyc_Absyn_label_stmt( _temp2275, _temp2283, 0); _temp2285->tl=
ss; _temp2285;}); succ_lab= _temp2275;}}}}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL2071;} _LL2099: { struct _tuple0* _temp2286= Cyc_Toc_temp_var(); goto
_LL2287; _LL2287: { void* _temp2288=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp2181->topt))->v; goto _LL2289; _LL2289: decls=({ struct Cyc_List_List*
_temp2290=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2290->hd=( void*)({ struct _tuple14* _temp2291=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2291->f1= _temp2286; _temp2291->f2=
Cyc_Toc_typ_to_c( _temp2288); _temp2291;}); _temp2290->tl= decls; _temp2290;});{
struct _tuple13 _temp2292= Cyc_Toc_xlate_pat( nv, _temp2288, Cyc_Absyn_var_exp(
_temp2286, 0), Cyc_Absyn_deref_exp( path, 0), _temp2181, succ_lab, fail_lab,
decls); goto _LL2293; _LL2293: nv= _temp2292.f1; decls= _temp2292.f2;{ struct
Cyc_Absyn_Stmt* _temp2294= _temp2292.f3; goto _LL2295; _LL2295: { struct Cyc_Absyn_Stmt*
_temp2296= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2286, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2294, 0); goto _LL2297;
_LL2297: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
_temp2296, 0);} else{ s= _temp2296;} goto _LL2071;}}}}} _LL2101: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp2298=( char*)"unknownid"; struct _tagged_string _temp2299; _temp2299.curr=
_temp2298; _temp2299.base= _temp2298; _temp2299.last_plus_one= _temp2298 + 10;
_temp2299;})); goto _LL2071; _LL2103: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2300=(
char*)"unknowncall"; struct _tagged_string _temp2301; _temp2301.curr= _temp2300;
_temp2301.base= _temp2300; _temp2301.last_plus_one= _temp2300 + 12; _temp2301;}));
goto _LL2071; _LL2105: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2302=( char*)"unknownfields";
struct _tagged_string _temp2303; _temp2303.curr= _temp2302; _temp2303.base=
_temp2302; _temp2303.last_plus_one= _temp2302 + 14; _temp2303;})); goto _LL2071;
_LL2071:;} return({ struct _tuple13 _temp2304; _temp2304.f1= nv; _temp2304.f2=
decls; _temp2304.f3= s; _temp2304;});} struct _tuple16{ struct _tagged_string*
f1; struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause* f3; } ; static
struct _tuple16* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause* sc){ return({
struct _tuple16* _temp2305=( struct _tuple16*) GC_malloc( sizeof( struct
_tuple16)); _temp2305->f1= Cyc_Toc_fresh_label(); _temp2305->f2= Cyc_Toc_fresh_label();
_temp2305->f3= sc; _temp2305;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp2306=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2307; _LL2307: { int leave_as_switch;{ void*
_temp2308= Cyc_Tcutil_compress( _temp2306); void* _temp2314; void* _temp2316;
_LL2310: if(( unsigned int) _temp2308 > 4u?*(( int*) _temp2308) == Cyc_Absyn_IntType:
0){ _LL2317: _temp2316=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2308)->f1;
goto _LL2315; _LL2315: _temp2314=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2308)->f2; goto _LL2311;} else{ goto _LL2312;} _LL2312: goto _LL2313;
_LL2311: leave_as_switch= 1; goto _LL2309; _LL2313: leave_as_switch= 0; goto
_LL2309; _LL2309:;}{ struct Cyc_List_List* _temp2318= scs; goto _LL2319; _LL2319:
for( 0; _temp2318 != 0; _temp2318=(( struct Cyc_List_List*) _check_null(
_temp2318))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2318))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2318))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2320= scs; goto
_LL2321; _LL2321: for( 0; _temp2320 != 0; _temp2320=(( struct Cyc_List_List*)
_check_null( _temp2320))->tl){ struct Cyc_Absyn_Stmt* _temp2322=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2320))->hd)->body; goto _LL2323;
_LL2323:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2320))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2322, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2322);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct
_tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple16*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* _temp2324= lscs; goto _LL2325; _LL2325: for( 0; _temp2324
!= 0; _temp2324=(( struct Cyc_List_List*) _check_null( _temp2324))->tl){ struct
Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2324))->hd)).f3; struct _tagged_string* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp2324))->tl == 0? end_l:(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp2324))->tl))->hd)).f1;
struct _tagged_string* succ_lab=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2324))->hd)).f2; if( sc->where_clause == 0){ struct _tuple13
_temp2326= Cyc_Toc_xlate_pat( nv, _temp2306, r, path, sc->pattern, succ_lab,
fail_lab, decls); goto _LL2327; _LL2327: nvs=({ struct Cyc_List_List* _temp2328=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2328->hd=(
void*) _temp2326.f1; _temp2328->tl= nvs; _temp2328;}); decls= _temp2326.f2;
test_stmts=({ struct Cyc_List_List* _temp2329=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2329->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2324))->hd)).f1,
_temp2326.f3, 0); _temp2329->tl= test_stmts; _temp2329;});} else{ struct Cyc_Absyn_Exp*
_temp2330=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause));
goto _LL2331; _LL2331: { struct _tagged_string* _temp2332= Cyc_Toc_fresh_label();
goto _LL2333; _LL2333: { struct _tuple13 _temp2334= Cyc_Toc_xlate_pat( nv,
_temp2306, r, path, sc->pattern, _temp2332, fail_lab, decls); goto _LL2335;
_LL2335: Cyc_Toc_exp_to_c( _temp2334.f1, _temp2330); nvs=({ struct Cyc_List_List*
_temp2336=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2336->hd=( void*) _temp2334.f1; _temp2336->tl= nvs; _temp2336;}); decls=
_temp2334.f2;{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt( _temp2330,
Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2332, s2, 0); test_stmts=({
struct Cyc_List_List* _temp2337=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2337->hd=( void*) Cyc_Absyn_label_stmt((*(( struct
_tuple16*)(( struct Cyc_List_List*) _check_null( _temp2324))->hd)).f1, Cyc_Absyn_seq_stmt(
_temp2334.f3, s3, 0), 0); _temp2337->tl= test_stmts; _temp2337;});}}}}}}{ struct
Cyc_Absyn_Stmt* _temp2338= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( test_stmts), 0); goto _LL2339;
_LL2339: nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl != 0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2340=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2341; _LL2341: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2340, end_l,(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2342=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2343; _LL2343: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2342, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2338, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple14 _temp2346;
void* _temp2347; struct _tuple0* _temp2349; struct _tuple14* _temp2344=( struct
_tuple14*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2346=*
_temp2344; _LL2350: _temp2349= _temp2346.f1; goto _LL2348; _LL2348: _temp2347=
_temp2346.f2; goto _LL2345; _LL2345: res= Cyc_Absyn_declare_stmt( _temp2349,
_temp2347, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct
Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt* Cyc_Toc_make_npop_handler(
int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv,
0),({ struct Cyc_List_List* _temp2351=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List) * 1); _temp2351[ 0]=({ struct Cyc_List_List
_temp2352; _temp2352.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0);
_temp2352.tl= 0; _temp2352;}); _temp2351;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2353=( void*) s->r; struct Cyc_Absyn_Exp*
_temp2393; struct Cyc_Absyn_Stmt* _temp2395; struct Cyc_Absyn_Stmt* _temp2397;
struct Cyc_Absyn_Exp* _temp2399; struct Cyc_Absyn_Stmt* _temp2401; struct Cyc_Absyn_Stmt*
_temp2403; struct Cyc_Absyn_Exp* _temp2405; struct Cyc_Absyn_Stmt* _temp2407;
struct _tuple2 _temp2409; struct Cyc_Absyn_Stmt* _temp2411; struct Cyc_Absyn_Exp*
_temp2413; struct Cyc_Absyn_Stmt* _temp2415; struct Cyc_Absyn_Stmt* _temp2417;
struct Cyc_Absyn_Stmt* _temp2419; struct _tagged_string* _temp2421; struct Cyc_Absyn_Stmt*
_temp2423; struct _tuple2 _temp2425; struct Cyc_Absyn_Stmt* _temp2427; struct
Cyc_Absyn_Exp* _temp2429; struct _tuple2 _temp2431; struct Cyc_Absyn_Stmt*
_temp2433; struct Cyc_Absyn_Exp* _temp2435; struct Cyc_Absyn_Exp* _temp2437;
struct Cyc_List_List* _temp2439; struct Cyc_Absyn_Exp* _temp2441; struct Cyc_Absyn_Switch_clause**
_temp2443; struct Cyc_List_List* _temp2445; struct Cyc_Absyn_Stmt* _temp2447;
struct Cyc_Absyn_Decl* _temp2449; struct Cyc_Absyn_Stmt* _temp2451; struct Cyc_Absyn_Stmt*
_temp2453; struct Cyc_Absyn_Stmt* _temp2455; struct _tagged_string* _temp2457;
struct _tuple2 _temp2459; struct Cyc_Absyn_Stmt* _temp2461; struct Cyc_Absyn_Exp*
_temp2463; struct Cyc_Absyn_Stmt* _temp2465; struct Cyc_List_List* _temp2467;
struct Cyc_Absyn_Stmt* _temp2469; struct Cyc_Absyn_Stmt* _temp2471; struct Cyc_Absyn_Vardecl*
_temp2473; struct Cyc_Absyn_Tvar* _temp2475; _LL2355: if( _temp2353 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2356;} else{ goto _LL2357;} _LL2357: if((
unsigned int) _temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_Exp_s: 0){
_LL2394: _temp2393=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp2353)->f1; goto _LL2358;} else{ goto _LL2359;} _LL2359: if(( unsigned int)
_temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_Seq_s: 0){ _LL2398: _temp2397=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp2353)->f1; goto
_LL2396; _LL2396: _temp2395=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp2353)->f2; goto _LL2360;} else{ goto _LL2361;} _LL2361: if(( unsigned int)
_temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_Return_s: 0){ _LL2400:
_temp2399=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp2353)->f1; goto _LL2362;} else{ goto _LL2363;} _LL2363: if(( unsigned int)
_temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_IfThenElse_s: 0){ _LL2406:
_temp2405=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2353)->f1; goto _LL2404; _LL2404: _temp2403=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2353)->f2; goto _LL2402; _LL2402:
_temp2401=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2353)->f3; goto _LL2364;} else{ goto _LL2365;} _LL2365: if(( unsigned int)
_temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_While_s: 0){ _LL2410: _temp2409=(
struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp2353)->f1; _LL2414:
_temp2413= _temp2409.f1; goto _LL2412; _LL2412: _temp2411= _temp2409.f2; goto
_LL2408; _LL2408: _temp2407=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp2353)->f2; goto _LL2366;} else{ goto _LL2367;} _LL2367: if(( unsigned int)
_temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_Break_s: 0){ _LL2416: _temp2415=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp2353)->f1; goto
_LL2368;} else{ goto _LL2369;} _LL2369: if(( unsigned int) _temp2353 > 1u?*((
int*) _temp2353) == Cyc_Absyn_Continue_s: 0){ _LL2418: _temp2417=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp2353)->f1; goto _LL2370;} else{ goto
_LL2371;} _LL2371: if(( unsigned int) _temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_Goto_s:
0){ _LL2422: _temp2421=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp2353)->f1; goto _LL2420; _LL2420: _temp2419=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp2353)->f2; goto _LL2372;} else{ goto
_LL2373;} _LL2373: if(( unsigned int) _temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_For_s:
0){ _LL2438: _temp2437=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp2353)->f1; goto _LL2432; _LL2432: _temp2431=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp2353)->f2; _LL2436: _temp2435= _temp2431.f1; goto _LL2434; _LL2434:
_temp2433= _temp2431.f2; goto _LL2426; _LL2426: _temp2425=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp2353)->f3; _LL2430: _temp2429= _temp2425.f1;
goto _LL2428; _LL2428: _temp2427= _temp2425.f2; goto _LL2424; _LL2424: _temp2423=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp2353)->f4; goto
_LL2374;} else{ goto _LL2375;} _LL2375: if(( unsigned int) _temp2353 > 1u?*((
int*) _temp2353) == Cyc_Absyn_Switch_s: 0){ _LL2442: _temp2441=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp2353)->f1; goto _LL2440; _LL2440:
_temp2439=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp2353)->f2; goto _LL2376;} else{ goto _LL2377;} _LL2377: if(( unsigned int)
_temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_Fallthru_s: 0){ _LL2446:
_temp2445=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2353)->f1; goto _LL2444; _LL2444: _temp2443=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp2353)->f2; goto _LL2378;} else{ goto
_LL2379;} _LL2379: if(( unsigned int) _temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_Decl_s:
0){ _LL2450: _temp2449=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp2353)->f1; goto _LL2448; _LL2448: _temp2447=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp2353)->f2; goto _LL2380;} else{ goto
_LL2381;} _LL2381: if(( unsigned int) _temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_Cut_s:
0){ _LL2452: _temp2451=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp2353)->f1; goto _LL2382;} else{ goto _LL2383;} _LL2383: if(( unsigned int)
_temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_Splice_s: 0){ _LL2454:
_temp2453=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp2353)->f1; goto _LL2384;} else{ goto _LL2385;} _LL2385: if(( unsigned int)
_temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_Label_s: 0){ _LL2458: _temp2457=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp2353)->f1; goto
_LL2456; _LL2456: _temp2455=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp2353)->f2; goto _LL2386;} else{ goto _LL2387;} _LL2387: if(( unsigned int)
_temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_Do_s: 0){ _LL2466: _temp2465=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp2353)->f1; goto
_LL2460; _LL2460: _temp2459=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp2353)->f2; _LL2464: _temp2463= _temp2459.f1; goto _LL2462; _LL2462:
_temp2461= _temp2459.f2; goto _LL2388;} else{ goto _LL2389;} _LL2389: if((
unsigned int) _temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_TryCatch_s: 0){
_LL2470: _temp2469=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2353)->f1; goto _LL2468; _LL2468: _temp2467=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp2353)->f2; goto _LL2390;} else{ goto
_LL2391;} _LL2391: if(( unsigned int) _temp2353 > 1u?*(( int*) _temp2353) == Cyc_Absyn_Region_s:
0){ _LL2476: _temp2475=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2353)->f1; goto _LL2474; _LL2474: _temp2473=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp2353)->f2; goto _LL2472; _LL2472:
_temp2471=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2353)->f3; goto _LL2392;} else{ goto _LL2354;} _LL2356: return; _LL2358:
Cyc_Toc_exp_to_c( nv, _temp2393); return; _LL2360: Cyc_Toc_stmt_to_c( nv,
_temp2397); s= _temp2395; continue; _LL2362: { struct Cyc_Core_Opt* topt= 0; if(
_temp2399 != 0){ topt=({ struct Cyc_Core_Opt* _temp2477=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2477->v=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*) _check_null(
_temp2399))->topt))->v); _temp2477;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*) _check_null( _temp2399)));} if( s->try_depth > 0){ if(
topt != 0){ struct _tuple0* _temp2478= Cyc_Toc_temp_var(); goto _LL2479; _LL2479: {
struct Cyc_Absyn_Stmt* _temp2480= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*)
Cyc_Absyn_var_exp( _temp2478, 0), 0); goto _LL2481; _LL2481:( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp2478,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp2399, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp2480, 0), 0))->r));}} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL2364: Cyc_Toc_exp_to_c( nv, _temp2405); Cyc_Toc_stmt_to_c( nv,
_temp2403); s= _temp2401; continue; _LL2366: Cyc_Toc_exp_to_c( nv, _temp2413);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2407); return; _LL2368: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)(( struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{
int dest_depth= _temp2415 == 0? 0:(( struct Cyc_Absyn_Stmt*) _check_null(
_temp2415))->try_depth; Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s);
return;} _LL2370: if( nv->continue_lab != 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_goto_stmt(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before( s->try_depth
-(( struct Cyc_Absyn_Stmt*) _check_null( _temp2417))->try_depth, s); return;
_LL2372: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2419))->try_depth, s); return; _LL2374: Cyc_Toc_exp_to_c( nv,
_temp2437); Cyc_Toc_exp_to_c( nv, _temp2435); Cyc_Toc_exp_to_c( nv, _temp2429);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2423); return; _LL2376: Cyc_Toc_xlate_switch(
nv, s, _temp2441, _temp2439); return; _LL2378: if( nv->fallthru_info == 0){(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp2482=( char*)"fallthru in unexpected place"; struct
_tagged_string _temp2483; _temp2483.curr= _temp2482; _temp2483.base= _temp2482;
_temp2483.last_plus_one= _temp2482 + 29; _temp2483;})));}{ struct _tuple8
_temp2486; struct Cyc_Dict_Dict* _temp2487; struct Cyc_List_List* _temp2489;
struct _tagged_string* _temp2491; struct _tuple8* _temp2484=( struct _tuple8*)((
struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp2486=* _temp2484;
_LL2492: _temp2491= _temp2486.f1; goto _LL2490; _LL2490: _temp2489= _temp2486.f2;
goto _LL2488; _LL2488: _temp2487= _temp2486.f3; goto _LL2485; _LL2485: { struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp2491, 0); Cyc_Toc_do_npop_before(
s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp2443)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp2493=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2489); goto _LL2494; _LL2494: { struct Cyc_List_List*
_temp2495=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2445); goto _LL2496; _LL2496: for( 0; _temp2493 != 0;( _temp2493=(( struct
Cyc_List_List*) _check_null( _temp2493))->tl, _temp2495=(( struct Cyc_List_List*)
_check_null( _temp2495))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp2495))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp2487,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp2493))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp2495))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}}} _LL2380:{ void* _temp2497=( void*) _temp2449->r; int
_temp2505; struct Cyc_Absyn_Exp* _temp2507; struct Cyc_Core_Opt* _temp2509;
struct Cyc_Core_Opt* _temp2511; struct Cyc_Absyn_Pat* _temp2513; struct Cyc_Absyn_Vardecl*
_temp2515; _LL2499: if(*(( int*) _temp2497) == Cyc_Absyn_Let_d){ _LL2514:
_temp2513=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2497)->f1;
goto _LL2512; _LL2512: _temp2511=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2497)->f2; goto _LL2510; _LL2510: _temp2509=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2497)->f3; goto _LL2508; _LL2508: _temp2507=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2497)->f4; goto
_LL2506; _LL2506: _temp2505=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2497)->f5;
goto _LL2500;} else{ goto _LL2501;} _LL2501: if(*(( int*) _temp2497) == Cyc_Absyn_Var_d){
_LL2516: _temp2515=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2497)->f1; goto _LL2502;} else{ goto _LL2503;} _LL2503: goto _LL2504;
_LL2500:( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2513,
_temp2509, _temp2507, _temp2505, _temp2447))->r)); goto _LL2498; _LL2502: {
struct _tuple0* _temp2517= _temp2515->name; goto _LL2518; _LL2518:( void*)(
_temp2515->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2515->type));{
struct Cyc_Toc_Env* _temp2521= Cyc_Toc_add_varmap( nv, _temp2517, Cyc_Absyn_varb_exp(
_temp2517,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp2519=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2519[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2520; _temp2520.tag= Cyc_Absyn_Local_b; _temp2520.f1=
_temp2515; _temp2520;}); _temp2519;}), 0)); goto _LL2522; _LL2522: Cyc_Toc_stmt_to_c(
_temp2521, _temp2447); if( _temp2515->initializer != 0){ struct Cyc_Absyn_Exp*
init=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( _temp2515->initializer));
void* _temp2523=( void*) init->r; struct Cyc_Absyn_Exp* _temp2529; struct Cyc_Absyn_Exp*
_temp2531; struct Cyc_Absyn_Vardecl* _temp2533; _LL2525: if(*(( int*) _temp2523)
== Cyc_Absyn_Comprehension_e){ _LL2534: _temp2533=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2523)->f1; goto _LL2532; _LL2532:
_temp2531=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2523)->f2; goto _LL2530; _LL2530: _temp2529=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2523)->f3; goto _LL2526;} else{
goto _LL2527;} _LL2527: goto _LL2528; _LL2526: _temp2515->initializer= 0;( void*)(
_temp2447->r=( void*)(( void*)( Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
_temp2517, 0), _temp2533, _temp2531, _temp2529, Cyc_Absyn_new_stmt(( void*)
_temp2447->r, 0), 0))->r)); goto _LL2524; _LL2528: Cyc_Toc_exp_to_c( nv, init);
goto _LL2524; _LL2524:;} goto _LL2498;}} _LL2504:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2535=(
char*)"bad nested declaration within function"; struct _tagged_string _temp2536;
_temp2536.curr= _temp2535; _temp2536.base= _temp2535; _temp2536.last_plus_one=
_temp2535 + 39; _temp2536;})); goto _LL2498; _LL2498:;} return; _LL2382:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp2537=( char*)"cut"; struct _tagged_string _temp2538; _temp2538.curr=
_temp2537; _temp2538.base= _temp2537; _temp2538.last_plus_one= _temp2537 + 4;
_temp2538;})); return; _LL2384:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2539=( char*)"splice"; struct _tagged_string
_temp2540; _temp2540.curr= _temp2539; _temp2540.base= _temp2539; _temp2540.last_plus_one=
_temp2539 + 7; _temp2540;})); return; _LL2386: s= _temp2455; continue; _LL2388:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2465); Cyc_Toc_exp_to_c( nv,
_temp2463); return; _LL2390: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct
_tuple0* e_var= Cyc_Toc_temp_var(); struct _tuple0* was_thrown_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__push_handler_qv,
0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp( Cyc_Toc_setjmp_qv, 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__pop_handler_qv,
0); struct Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp( Cyc_Toc__exn_thrown_qv,
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2541=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2541->v=( void*) e_typ; _temp2541;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2469);{
struct Cyc_Absyn_Stmt* _temp2542= Cyc_Absyn_seq_stmt( _temp2469, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); goto _LL2543; _LL2543: {
struct _tuple0* _temp2544= Cyc_Toc_temp_var(); goto _LL2545; _LL2545: { struct
Cyc_Absyn_Exp* _temp2546= Cyc_Absyn_var_exp( _temp2544, 0); goto _LL2547;
_LL2547: { struct Cyc_Absyn_Vardecl* _temp2548= Cyc_Absyn_new_vardecl( _temp2544,
Cyc_Absyn_exn_typ, 0); goto _LL2549; _LL2549: _temp2546->topt=({ struct Cyc_Core_Opt*
_temp2550=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2550->v=( void*) Cyc_Absyn_exn_typ; _temp2550;});{ struct Cyc_Absyn_Pat*
_temp2555=({ struct Cyc_Absyn_Pat* _temp2551=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2551->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2553=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2553[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2554; _temp2554.tag= Cyc_Absyn_Var_p;
_temp2554.f1= _temp2548; _temp2554;}); _temp2553;})); _temp2551->topt=({ struct
Cyc_Core_Opt* _temp2552=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2552->v=( void*) Cyc_Absyn_exn_typ; _temp2552;}); _temp2551->loc= 0;
_temp2551;}); goto _LL2556; _LL2556: { struct Cyc_Absyn_Exp* _temp2557= Cyc_Absyn_throw_exp(
_temp2546, 0); goto _LL2558; _LL2558: _temp2557->topt=({ struct Cyc_Core_Opt*
_temp2559=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2559->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2559;});{ struct Cyc_Absyn_Stmt*
_temp2560= Cyc_Absyn_exp_stmt( _temp2557, 0); goto _LL2561; _LL2561: { struct
Cyc_Absyn_Switch_clause* _temp2565=({ struct Cyc_Absyn_Switch_clause* _temp2562=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2562->pattern= _temp2555; _temp2562->pat_vars=({ struct Cyc_Core_Opt*
_temp2563=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2563->v=( void*)({ struct Cyc_List_List* _temp2564=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2564->hd=( void*) _temp2548;
_temp2564->tl= 0; _temp2564;}); _temp2563;}); _temp2562->where_clause= 0;
_temp2562->body= _temp2560; _temp2562->loc= 0; _temp2562;}); goto _LL2566;
_LL2566: { struct Cyc_Absyn_Stmt* _temp2568= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2467,({ struct Cyc_List_List* _temp2567=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2567->hd=( void*) _temp2565; _temp2567->tl=
0; _temp2567;})), 0); goto _LL2569; _LL2569: Cyc_Toc_stmt_to_c( nv, _temp2568);{
struct Cyc_Absyn_Exp* _temp2571= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2570=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2570->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2570->tl= 0; _temp2570;}), 0); goto _LL2572; _LL2572: { struct Cyc_Absyn_Stmt*
_temp2574= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2573=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2573->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2573->tl= 0;
_temp2573;}), 0), 0); goto _LL2575; _LL2575: { struct Cyc_Absyn_Exp* _temp2576=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2577; _LL2577: {
struct Cyc_Absyn_Exp* _temp2578= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2579; _LL2579:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2574, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2576, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2571, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2578, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2542, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2568, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2392: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2473->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2585=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2585[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2586; _temp2586.tag= Cyc_Absyn_Local_b; _temp2586.f1=
_temp2473; _temp2586;}); _temp2585;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2584=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2584->f1= Cyc_Toc_abs_ns; _temp2584->f2= Cyc_Toc__push_region_sp;
_temp2584;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2583=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2583->f1= Cyc_Toc_abs_ns; _temp2583->f2= Cyc_Toc__pop_region_sp; _temp2583;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2582=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2582->f1=
Cyc_Toc_abs_ns; _temp2582->f2= Cyc_Toc__new_region_sp; _temp2582;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2581=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2581->hd=( void*) addr_rh_exp; _temp2581->tl= 0; _temp2581;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2580=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2580->hd=( void*) x_exp; _temp2580->tl= 0; _temp2580;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2471);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2471, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2354:;}} struct _tuple17{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0;(
void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{ struct
Cyc_List_List* _temp2587= f->args; goto _LL2588; _LL2588: for( 0; _temp2587 != 0;
_temp2587=(( struct Cyc_List_List*) _check_null( _temp2587))->tl){(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp2587))->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp2587))->hd)).f3);{
struct _tuple0* _temp2590=({ struct _tuple0* _temp2589=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2589->f1=( void*) Cyc_Absyn_Loc_n;
_temp2589->f2=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp2587))->hd)).f1; _temp2589;}); goto _LL2591; _LL2591: nv= Cyc_Toc_add_varmap(
nv, _temp2590, Cyc_Absyn_var_exp( _temp2590, 0));}}}{ struct Cyc_List_List*
_temp2592=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( f->param_vardecls))->v;
goto _LL2593; _LL2593: for( 0; _temp2592 != 0; _temp2592=(( struct Cyc_List_List*)
_check_null( _temp2592))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_List_List*) _check_null( _temp2592))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2592))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2594= s; _LL2596: if(
_temp2594 ==( void*) Cyc_Absyn_Abstract){ goto _LL2597;} else{ goto _LL2598;}
_LL2598: if( _temp2594 ==( void*) Cyc_Absyn_ExternC){ goto _LL2599;} else{ goto
_LL2600;} _LL2600: goto _LL2601; _LL2597: return( void*) Cyc_Absyn_Public;
_LL2599: return( void*) Cyc_Absyn_Extern; _LL2601: return s; _LL2595:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2602=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2603; _LL2603: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2604=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2604->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2604;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2605=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2602).f2); goto _LL2606;
_LL2606: if( _temp2605 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2602).f2, s));}
else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null(
_temp2605))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2602).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2607=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2607->v=( void*)
_temp2602; _temp2607;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2608=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2609; _LL2609: for( 0; _temp2608 != 0;
_temp2608=(( struct Cyc_List_List*) _check_null( _temp2608))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2608))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2608))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2610=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2611; _LL2611: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2612=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2612->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2612;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2613=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2610).f2); goto _LL2614; _LL2614:
if( _temp2613 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2610).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2613))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2610).f2, u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)(
u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==(
void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?( void*)
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2615=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2615->v=( void*) _temp2610; _temp2615;}); if( u->fields !=
0){ if( seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* _temp2616=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( u->fields))->v; goto
_LL2617; _LL2617: for( 0; _temp2616 != 0; _temp2616=(( struct Cyc_List_List*)
_check_null( _temp2616))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2616))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2616))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2618=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2618->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2618;});}{ struct
_tuple0* _temp2619= tud->name; goto _LL2620; _LL2620: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2619)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2619));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2621=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2622; _LL2622: for( 0;
_temp2621 != 0; _temp2621=(( struct Cyc_List_List*) _check_null( _temp2621))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2621))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2623; _temp2623.q_const= 1; _temp2623.q_volatile= 0;
_temp2623.q_restrict= 0; _temp2623;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2624=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2624->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2625=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2625[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2626; _temp2626.tag= Cyc_Absyn_Var_d;
_temp2626.f1= vd; _temp2626;}); _temp2625;}), 0); _temp2624->tl= Cyc_Toc_result_decls;
_temp2624;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2627; _temp2627.q_const=
1; _temp2627.q_volatile= 0; _temp2627.q_restrict= 0; _temp2627;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2628=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2628->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2629=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2629[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2630; _temp2630.tag= Cyc_Absyn_Var_d; _temp2630.f1= vd; _temp2630;});
_temp2629;}), 0); _temp2628->tl= Cyc_Toc_result_decls; _temp2628;});{ struct Cyc_List_List*
_temp2631= 0; goto _LL2632; _LL2632: { int i= 1;{ struct Cyc_List_List*
_temp2633= f->typs; goto _LL2634; _LL2634: for( 0; _temp2633 != 0;( _temp2633=((
struct Cyc_List_List*) _check_null( _temp2633))->tl, i ++)){ struct
_tagged_string* _temp2636=( struct _tagged_string*)({ struct _tagged_string*
_temp2635=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp2635[ 0]= xprintf("f%d", i); _temp2635;}); goto _LL2637; _LL2637: { struct
Cyc_Absyn_Structfield* _temp2639=({ struct Cyc_Absyn_Structfield* _temp2638=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2638->name= _temp2636; _temp2638->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2633))->hd)).f1; _temp2638->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2633))->hd)).f2);
_temp2638->width= 0; _temp2638->attributes= 0; _temp2638;}); goto _LL2640;
_LL2640: _temp2631=({ struct Cyc_List_List* _temp2641=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2641->hd=( void*) _temp2639;
_temp2641->tl= _temp2631; _temp2641;});}}} _temp2631=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2631); _temp2631=({ struct Cyc_List_List*
_temp2642=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2642->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2643=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2643->name= Cyc_Toc_tag_sp;
_temp2643->tq= Cyc_Toc_mt_tq; _temp2643->type=( void*) Cyc_Absyn_sint_t;
_temp2643->width= 0; _temp2643->attributes= 0; _temp2643;}); _temp2642->tl=
_temp2631; _temp2642;});{ struct Cyc_Absyn_Structdecl* _temp2649=({ struct Cyc_Absyn_Structdecl*
_temp2644=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2644->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2644->name=({ struct Cyc_Core_Opt*
_temp2646=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2646->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2647=( char*)"_struct"; struct _tagged_string _temp2648; _temp2648.curr=
_temp2647; _temp2648.base= _temp2647; _temp2648.last_plus_one= _temp2647 + 8;
_temp2648;})); _temp2646;}); _temp2644->tvs= 0; _temp2644->fields=({ struct Cyc_Core_Opt*
_temp2645=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2645->v=( void*) _temp2631; _temp2645;}); _temp2644->attributes= 0;
_temp2644;}); goto _LL2650; _LL2650: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2651=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2651->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2652=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2652[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2653; _temp2653.tag= Cyc_Absyn_Struct_d;
_temp2653.f1= _temp2649; _temp2653;}); _temp2652;}), 0); _temp2651->tl= Cyc_Toc_result_decls;
_temp2651;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2654=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2654->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2654;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2655= xd->name; goto _LL2656; _LL2656: {
struct Cyc_List_List* _temp2657=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2658; _LL2658: for( 0; _temp2657 != 0;
_temp2657=(( struct Cyc_List_List*) _check_null( _temp2657))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2657))->hd; struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2660= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2659=* fn;(
unsigned int)( _temp2659.last_plus_one - _temp2659.curr);}) + 4, 0); goto
_LL2661; _LL2661: { struct Cyc_Absyn_ArrayType_struct* _temp2664=({ struct Cyc_Absyn_ArrayType_struct*
_temp2662=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2662[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2663; _temp2663.tag= Cyc_Absyn_ArrayType;
_temp2663.f1=( void*) Cyc_Absyn_uchar_t; _temp2663.f2= Cyc_Toc_mt_tq; _temp2663.f3=(
struct Cyc_Absyn_Exp*) _temp2660; _temp2663;}); _temp2662;}); goto _LL2665;
_LL2665: { struct Cyc_Core_Opt* _temp2666=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2674; int _temp2675; _LL2668: if( _temp2666 == 0){ goto
_LL2669;} else{ goto _LL2670;} _LL2670: if( _temp2666 == 0){ goto _LL2672;}
else{ _temp2674=* _temp2666; _LL2676: _temp2675=( int) _temp2674.v; if(
_temp2675 == 0){ goto _LL2671;} else{ goto _LL2672;}} _LL2672: goto _LL2673;
_LL2669: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(({ int _temp2677= 0; int
_temp2678= 0; int _temp2679= 0; int _temp2680= 0; struct _tagged_string
_temp2681=* fn; xprintf("%c%c%c%c%.*s", _temp2677, _temp2678, _temp2679,
_temp2680, _temp2681.last_plus_one - _temp2681.curr, _temp2681.curr);}), 0);}{
struct Cyc_Absyn_Vardecl* _temp2682= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2664, initopt); goto _LL2683; _LL2683:( void*)( _temp2682->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2684=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2684->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2685=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2685[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2686; _temp2686.tag= Cyc_Absyn_Var_d;
_temp2686.f1= _temp2682; _temp2686;}); _temp2685;}), 0); _temp2684->tl= Cyc_Toc_result_decls;
_temp2684;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2687= f->typs; goto _LL2688; _LL2688: for( 0;
_temp2687 != 0;( _temp2687=(( struct Cyc_List_List*) _check_null( _temp2687))->tl,
i ++)){ struct _tagged_string* _temp2690=( struct _tagged_string*)({ struct
_tagged_string* _temp2689=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2689[ 0]= xprintf("f%d", i); _temp2689;}); goto
_LL2691; _LL2691: { struct Cyc_Absyn_Structfield* _temp2693=({ struct Cyc_Absyn_Structfield*
_temp2692=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2692->name= _temp2690; _temp2692->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2687))->hd)).f1; _temp2692->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2687))->hd)).f2);
_temp2692->width= 0; _temp2692->attributes= 0; _temp2692;}); goto _LL2694;
_LL2694: fields=({ struct Cyc_List_List* _temp2695=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2695->hd=( void*) _temp2693;
_temp2695->tl= fields; _temp2695;});}}} fields=({ struct Cyc_List_List*
_temp2696=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2696->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2697=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2697->name= Cyc_Toc_tag_sp;
_temp2697->tq= Cyc_Toc_mt_tq; _temp2697->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2697->width= 0; _temp2697->attributes= 0; _temp2697;});
_temp2696->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2696;});{ struct Cyc_Absyn_Structdecl* _temp2703=({ struct Cyc_Absyn_Structdecl*
_temp2698=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2698->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2698->name=({ struct Cyc_Core_Opt*
_temp2700=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2700->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2701=( char*)"_struct"; struct _tagged_string _temp2702; _temp2702.curr=
_temp2701; _temp2702.base= _temp2701; _temp2702.last_plus_one= _temp2701 + 8;
_temp2702;})); _temp2700;}); _temp2698->tvs= 0; _temp2698->fields=({ struct Cyc_Core_Opt*
_temp2699=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2699->v=( void*) fields; _temp2699;}); _temp2698->attributes= 0; _temp2698;});
goto _LL2704; _LL2704: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2705=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2705->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2706=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2706[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2707; _temp2707.tag= Cyc_Absyn_Struct_d;
_temp2707.f1= _temp2703; _temp2707;}); _temp2706;}), 0); _temp2705->tl= Cyc_Toc_result_decls;
_temp2705;});}} goto _LL2667;}} _LL2671: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2713= Cyc_Absyn_string_exp(({ int _temp2708= 0; int
_temp2709= 0; int _temp2710= 0; int _temp2711= 0; struct _tagged_string
_temp2712=* fn; xprintf("%c%c%c%c%.*s", _temp2708, _temp2709, _temp2710,
_temp2711, _temp2712.last_plus_one - _temp2712.curr, _temp2712.curr);}), 0);
goto _LL2714; _LL2714: { struct Cyc_Absyn_Vardecl* _temp2715= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2664,( struct Cyc_Absyn_Exp*) _temp2713); goto _LL2716;
_LL2716:( void*)( _temp2715->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2717=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2717->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2718=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2718[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2719; _temp2719.tag= Cyc_Absyn_Var_d; _temp2719.f1= _temp2715; _temp2719;});
_temp2718;}), 0); _temp2717->tl= Cyc_Toc_result_decls; _temp2717;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2667; _LL2673: goto
_LL2667; _LL2667:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2720= ed->name; goto
_LL2721; _LL2721:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc)));{ struct Cyc_List_List*
_temp2722=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
goto _LL2723; _LL2723: for( 0; _temp2722 != 0; _temp2722=(( struct Cyc_List_List*)
_check_null( _temp2722))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2722))->hd)->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp2722))->hd)->tag)));(( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp2722))->hd)->tag=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_string* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_string* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();
struct Cyc_Absyn_Stmt* _temp2726; struct Cyc_List_List* _temp2728; struct Cyc_Toc_Env*
_temp2730; struct _tuple13 _temp2724= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL2731: _temp2730=
_temp2724.f1; goto _LL2729; _LL2729: _temp2728= _temp2724.f2; goto _LL2727;
_LL2727: _temp2726= _temp2724.f3; goto _LL2725; _LL2725: Cyc_Toc_stmt_to_c(
_temp2730, s);{ struct Cyc_Absyn_Stmt* _temp2732= Cyc_Absyn_label_stmt( succ_lab,
s, 0); goto _LL2733; _LL2733: if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2726, _temp2732, 0), 0);}
else{ struct Cyc_Absyn_Exp* _temp2735= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp2734=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2734->f1= Cyc_Toc_abs_ns; _temp2734->f2= Cyc_Toc_Match_Exception_sp;
_temp2734;}), 0)); goto _LL2736; _LL2736: { struct Cyc_Absyn_Stmt* _temp2737=
Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2735, 0), 0); goto
_LL2738; _LL2738: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2726, Cyc_Absyn_seq_stmt( _temp2737,
_temp2732, 0), 0), 0);}} for( 0; _temp2728 != 0; _temp2728=(( struct Cyc_List_List*)
_check_null( _temp2728))->tl){ struct _tuple14 _temp2741; void* _temp2742;
struct _tuple0* _temp2744; struct _tuple14* _temp2739=( struct _tuple14*)((
struct Cyc_List_List*) _check_null( _temp2728))->hd; _temp2741=* _temp2739;
_LL2745: _temp2744= _temp2741.f1; goto _LL2743; _LL2743: _temp2742= _temp2741.f2;
goto _LL2740; _LL2740: s= Cyc_Absyn_declare_stmt( _temp2744, _temp2742, 0, s, 0);}
return s;}}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2746=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp2770; struct Cyc_Absyn_Fndecl* _temp2772; int
_temp2774; struct Cyc_Absyn_Exp* _temp2776; struct Cyc_Core_Opt* _temp2778;
struct Cyc_Core_Opt* _temp2780; struct Cyc_Absyn_Pat* _temp2782; struct Cyc_Absyn_Structdecl*
_temp2784; struct Cyc_Absyn_Uniondecl* _temp2786; struct Cyc_Absyn_Tuniondecl*
_temp2788; struct Cyc_Absyn_Enumdecl* _temp2790; struct Cyc_Absyn_Typedefdecl*
_temp2792; struct Cyc_List_List* _temp2794; struct _tagged_string* _temp2796;
struct Cyc_List_List* _temp2798; struct _tuple0* _temp2800; struct Cyc_List_List*
_temp2802; _LL2748: if(*(( int*) _temp2746) == Cyc_Absyn_Var_d){ _LL2771:
_temp2770=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2746)->f1; goto _LL2749;} else{ goto _LL2750;} _LL2750: if(*(( int*)
_temp2746) == Cyc_Absyn_Fn_d){ _LL2773: _temp2772=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp2746)->f1; goto _LL2751;} else{ goto _LL2752;}
_LL2752: if(*(( int*) _temp2746) == Cyc_Absyn_Let_d){ _LL2783: _temp2782=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2746)->f1; goto
_LL2781; _LL2781: _temp2780=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2746)->f2; goto _LL2779; _LL2779: _temp2778=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2746)->f3; goto _LL2777; _LL2777: _temp2776=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2746)->f4; goto
_LL2775; _LL2775: _temp2774=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2746)->f5;
goto _LL2753;} else{ goto _LL2754;} _LL2754: if(*(( int*) _temp2746) == Cyc_Absyn_Struct_d){
_LL2785: _temp2784=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp2746)->f1; goto _LL2755;} else{ goto _LL2756;} _LL2756: if(*(( int*)
_temp2746) == Cyc_Absyn_Union_d){ _LL2787: _temp2786=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp2746)->f1; goto _LL2757;} else{ goto
_LL2758;} _LL2758: if(*(( int*) _temp2746) == Cyc_Absyn_Tunion_d){ _LL2789:
_temp2788=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2746)->f1; goto _LL2759;} else{ goto _LL2760;} _LL2760: if(*(( int*)
_temp2746) == Cyc_Absyn_Enum_d){ _LL2791: _temp2790=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp2746)->f1; goto _LL2761;} else{ goto
_LL2762;} _LL2762: if(*(( int*) _temp2746) == Cyc_Absyn_Typedef_d){ _LL2793:
_temp2792=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2746)->f1; goto _LL2763;} else{ goto _LL2764;} _LL2764: if(*(( int*)
_temp2746) == Cyc_Absyn_Namespace_d){ _LL2797: _temp2796=( struct _tagged_string*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2746)->f1; goto _LL2795; _LL2795:
_temp2794=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2746)->f2; goto _LL2765;} else{ goto _LL2766;} _LL2766: if(*(( int*)
_temp2746) == Cyc_Absyn_Using_d){ _LL2801: _temp2800=( struct _tuple0*)(( struct
Cyc_Absyn_Using_d_struct*) _temp2746)->f1; goto _LL2799; _LL2799: _temp2798=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp2746)->f2; goto
_LL2767;} else{ goto _LL2768;} _LL2768: if(*(( int*) _temp2746) == Cyc_Absyn_ExternC_d){
_LL2803: _temp2802=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2746)->f1; goto _LL2769;} else{ goto _LL2747;} _LL2749: { struct _tuple0*
_temp2804= _temp2770->name; goto _LL2805; _LL2805: if(( void*) _temp2770->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2804=({ struct _tuple0* _temp2806=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2806->f1= Cyc_Toc_abs_ns;
_temp2806->f2=(* _temp2804).f2; _temp2806;});} if( _temp2770->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp2770->initializer)));} if( ! nv->toplevel){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2807=(
char*)"decls_to_c: not at toplevel!"; struct _tagged_string _temp2808; _temp2808.curr=
_temp2807; _temp2808.base= _temp2807; _temp2808.last_plus_one= _temp2807 + 29;
_temp2808;}));} nv= Cyc_Toc_add_varmap( nv, _temp2770->name, Cyc_Absyn_varb_exp(
_temp2804,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp2809=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp2809[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp2810; _temp2810.tag= Cyc_Absyn_Global_b;
_temp2810.f1= _temp2770; _temp2810;}); _temp2809;}), 0)); _temp2770->name=
_temp2804;( void*)( _temp2770->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2770->sc));(
void*)( _temp2770->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2770->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2811=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2811->hd=( void*) d; _temp2811->tl=
Cyc_Toc_result_decls; _temp2811;}); goto _LL2747;} _LL2751: { struct _tuple0*
_temp2812= _temp2772->name; goto _LL2813; _LL2813: if(( void*) _temp2772->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2812=({ struct _tuple0* _temp2814=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2814->f1= Cyc_Toc_abs_ns;
_temp2814->f2=(* _temp2812).f2; _temp2814;});} nv= Cyc_Toc_add_varmap( nv,
_temp2772->name, Cyc_Absyn_var_exp( _temp2812, 0)); _temp2772->name= _temp2812;
Cyc_Toc_fndecl_to_c( nv, _temp2772); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2815=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2815->hd=( void*) d; _temp2815->tl= Cyc_Toc_result_decls; _temp2815;});
goto _LL2747;} _LL2753:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2816=( char*)"letdecl"; struct
_tagged_string _temp2817; _temp2817.curr= _temp2816; _temp2817.base= _temp2816;
_temp2817.last_plus_one= _temp2816 + 8; _temp2817;})); goto _LL2747; _LL2755:
Cyc_Toc_structdecl_to_c( _temp2784); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2818=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2818->hd=( void*) d; _temp2818->tl= Cyc_Toc_result_decls; _temp2818;});
goto _LL2747; _LL2757: Cyc_Toc_uniondecl_to_c( _temp2786); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2819=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2819->hd=( void*) d; _temp2819->tl= Cyc_Toc_result_decls;
_temp2819;}); goto _LL2747; _LL2759: if( _temp2788->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2788);} else{ Cyc_Toc_tuniondecl_to_c( _temp2788);} goto _LL2747; _LL2761:
Cyc_Toc_enumdecl_to_c( nv, _temp2790); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2820=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2820->hd=( void*) d; _temp2820->tl= Cyc_Toc_result_decls; _temp2820;});
goto _LL2747; _LL2763: _temp2792->name= _temp2792->name; _temp2792->tvs= 0;(
void*)( _temp2792->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2792->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2821=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2821->hd=( void*) d; _temp2821->tl=
Cyc_Toc_result_decls; _temp2821;}); goto _LL2747; _LL2765: _temp2798= _temp2794;
goto _LL2767; _LL2767: _temp2802= _temp2798; goto _LL2769; _LL2769: nv= Cyc_Toc_decls_to_c(
nv, _temp2802, top); goto _LL2747; _LL2747:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}