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
_temp146; goto _LL101; _LL101: for( 0; _temp150 != 0; _temp150=(( struct Cyc_List_List*)
_check_null( _temp150))->tl){ if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp150))->hd)).f2)){ return 0;}} return 1;
_LL103: return 0; _LL79:;} static int Cyc_Toc_is_nullable( void* t){ void*
_temp154= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp160; struct Cyc_Absyn_Conref*
_temp162; struct Cyc_Absyn_Tqual _temp164; struct Cyc_Absyn_Conref* _temp166;
void* _temp168; void* _temp170; _LL156: if(( unsigned int) _temp154 > 4u?*(( int*)
_temp154) == Cyc_Absyn_PointerType: 0){ _LL161: _temp160=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp154)->f1; _LL171: _temp170=( void*)
_temp160.elt_typ; goto _LL169; _LL169: _temp168=( void*) _temp160.rgn_typ; goto
_LL167; _LL167: _temp166=( struct Cyc_Absyn_Conref*) _temp160.nullable; goto
_LL165; _LL165: _temp164=( struct Cyc_Absyn_Tqual) _temp160.tq; goto _LL163;
_LL163: _temp162=( struct Cyc_Absyn_Conref*) _temp160.bounds; goto _LL157;}
else{ goto _LL158;} _LL158: goto _LL159; _LL157: { void* _temp172=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp166))->v; int _temp178; _LL174: if(( unsigned int) _temp172 > 1u?*(( int*)
_temp172) == Cyc_Absyn_Eq_constr: 0){ _LL179: _temp178=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp172)->f1; goto _LL175;} else{ goto _LL176;} _LL176: goto _LL177; _LL175:
return _temp178; _LL177: return 0; _LL173:;} _LL159:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp180=(
char*)"is_nullable"; struct _tagged_string _temp181; _temp181.curr= _temp180;
_temp181.base= _temp180; _temp181.last_plus_one= _temp180 + 12; _temp181;}));
return 0; _LL155:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct
_tuple0* x, struct _tagged_string tag){ return({ struct _tuple0* _temp182=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp182->f1=(* x).f1;
_temp182->f2=({ struct _tagged_string* _temp183=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp183[ 0]= Cyc_String_strconcat(*(*
x).f2, tag); _temp183;}); _temp182;});} static void Cyc_Toc_add_tuple_decl(
struct _tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List*
_temp184= 0; goto _LL185; _LL185:{ int i= 1; for( 0; ts != 0; ts=(( struct Cyc_List_List*)
_check_null( ts))->tl, i ++){ _temp184=({ struct Cyc_List_List* _temp186=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp186->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp187=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp187->name=({ struct
_tagged_string* _temp188=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp188[ 0]= xprintf("f%d", i); _temp188;}); _temp187->tq=
Cyc_Toc_mt_tq; _temp187->type=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); _temp187->width= 0; _temp187->attributes= 0; _temp187;});
_temp186->tl= _temp184; _temp186;});}} _temp184=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp184);{ struct Cyc_Absyn_Structdecl*
_temp193=({ struct Cyc_Absyn_Structdecl* _temp189=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp189->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp189->name=({ struct Cyc_Core_Opt* _temp191=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp191->v=( void*)({ struct _tuple0*
_temp192=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp192->f1=
Cyc_Toc_abs_ns; _temp192->f2= x; _temp192;}); _temp191;}); _temp189->tvs= 0;
_temp189->fields=({ struct Cyc_Core_Opt* _temp190=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp190->v=( void*) _temp184;
_temp190;}); _temp189->attributes= 0; _temp189;}); goto _LL194; _LL194: Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp195=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp195->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp196=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp196[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp197; _temp197.tag= Cyc_Absyn_Struct_d; _temp197.f1=
_temp193; _temp197;}); _temp196;}), 0); _temp195->tl= Cyc_Toc_result_decls;
_temp195;});}} static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x,
void* t){ void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Structfield* _temp199=({ struct Cyc_Absyn_Structfield* _temp198=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp198->name= Cyc_Toc_curr_sp; _temp198->tq= Cyc_Toc_mt_tq; _temp198->type=(
void*) tptr; _temp198->width= 0; _temp198->attributes= 0; _temp198;}); goto
_LL200; _LL200: { struct Cyc_Absyn_Structfield* _temp202=({ struct Cyc_Absyn_Structfield*
_temp201=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp201->name= Cyc_Toc_base_sp; _temp201->tq= Cyc_Toc_mt_tq; _temp201->type=(
void*) tptr; _temp201->width= 0; _temp201->attributes= 0; _temp201;}); goto
_LL203; _LL203: { struct Cyc_Absyn_Structfield* _temp205=({ struct Cyc_Absyn_Structfield*
_temp204=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp204->name= Cyc_Toc_last_plus_one_sp; _temp204->tq= Cyc_Toc_mt_tq; _temp204->type=(
void*) tptr; _temp204->width= 0; _temp204->attributes= 0; _temp204;}); goto
_LL206; _LL206: { struct Cyc_List_List* _temp210=({ struct Cyc_List_List*
_temp207=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp207->hd=( void*) _temp199; _temp207->tl=({ struct Cyc_List_List* _temp208=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp208->hd=(
void*) _temp202; _temp208->tl=({ struct Cyc_List_List* _temp209=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp209->hd=( void*) _temp205;
_temp209->tl= 0; _temp209;}); _temp208;}); _temp207;}); goto _LL211; _LL211: {
struct Cyc_Absyn_Structdecl* _temp216=({ struct Cyc_Absyn_Structdecl* _temp212=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp212->sc=( void*)(( void*) Cyc_Absyn_Public); _temp212->name=({ struct Cyc_Core_Opt*
_temp214=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp214->v=( void*)({ struct _tuple0* _temp215=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp215->f1= Cyc_Toc_abs_ns; _temp215->f2= x;
_temp215;}); _temp214;}); _temp212->tvs= 0; _temp212->fields=({ struct Cyc_Core_Opt*
_temp213=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp213->v=( void*)(( struct Cyc_List_List*) _temp210); _temp213;}); _temp212->attributes=
0; _temp212;}); goto _LL217; _LL217: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp218=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp218->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp219=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp219[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp220; _temp220.tag= Cyc_Absyn_Struct_d;
_temp220.f1= _temp216; _temp220;}); _temp219;}), 0); _temp218->tl= Cyc_Toc_result_decls;
_temp218;});}}}}} static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static
int Cyc_Toc_tuple_type_counter= 0; struct _tuple5{ struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static struct _tagged_string* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp221= Cyc_Toc_tuple_types;
goto _LL222; _LL222: for( 0; _temp221 != 0; _temp221=(( struct Cyc_List_List*)
_check_null( _temp221))->tl){ struct _tuple5 _temp225; struct Cyc_List_List*
_temp226; struct _tagged_string* _temp228; struct _tuple5* _temp223=( struct
_tuple5*)(( struct Cyc_List_List*) _check_null( _temp221))->hd; _temp225=*
_temp223; _LL229: _temp228= _temp225.f1; goto _LL227; _LL227: _temp226= _temp225.f2;
goto _LL224; _LL224: { int okay= 1; struct Cyc_List_List* _temp230= tqs0; goto
_LL231; _LL231: for( 0; _temp230 != 0? _temp226 != 0: 0; _temp230=(( struct Cyc_List_List*)
_check_null( _temp230))->tl, _temp226=(( struct Cyc_List_List*) _check_null(
_temp226))->tl){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp230))->hd)).f2,( void*)(( struct Cyc_List_List*) _check_null(
_temp226))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp230 != 0)? 1: _temp226
!= 0){ continue;} return _temp228;}}}{ struct _tagged_string* x=({ struct
_tagged_string* _temp234=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp234[ 0]= xprintf("_tuple%d",( Cyc_Toc_tuple_type_counter
++)); _temp234;}); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt,
tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({ struct Cyc_List_List*
_temp232=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp232->hd=( void*)({ struct _tuple5* _temp233=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp233->f1= x; _temp233->f2= ts; _temp233;});
_temp232->tl= Cyc_Toc_tuple_types; _temp232;}); return x;}} static struct Cyc_List_List*
Cyc_Toc_tagged_ptr_types= 0; static int Cyc_Toc_tagged_ptr_counter= 0; static
void* Cyc_Toc_tagged_string_type_v=( void*) 0u; static void* Cyc_Toc_tagged_string_type(){
if( Cyc_Toc_tagged_string_type_v ==( void*) Cyc_Absyn_VoidType){ Cyc_Toc_tagged_string_type_v=
Cyc_Absyn_strct( Cyc_Toc__tagged_string_sp);} return Cyc_Toc_tagged_string_type_v;}
struct _tuple6{ struct _tagged_string* f1; void* f2; } ; static void* Cyc_Toc_add_tagged_ptr_type(
void* t, struct Cyc_Absyn_Tqual tq){{ void* _temp235= t; void* _temp241; void*
_temp243; _LL237: if(( unsigned int) _temp235 > 4u?*(( int*) _temp235) == Cyc_Absyn_IntType:
0){ _LL244: _temp243=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp235)->f1;
if( _temp243 ==( void*) Cyc_Absyn_Unsigned){ goto _LL242;} else{ goto _LL239;}
_LL242: _temp241=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp235)->f2; if(
_temp241 ==( void*) Cyc_Absyn_B1){ goto _LL238;} else{ goto _LL239;}} else{ goto
_LL239;} _LL239: goto _LL240; _LL238: return Cyc_Toc_tagged_string_type();
_LL240: goto _LL236; _LL236:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts=(( struct Cyc_List_List*) _check_null( tts))->tl){ struct
_tuple6 _temp247; void* _temp248; struct _tagged_string* _temp250; struct
_tuple6* _temp245=( struct _tuple6*)(( struct Cyc_List_List*) _check_null( tts))->hd;
_temp247=* _temp245; _LL251: _temp250= _temp247.f1; goto _LL249; _LL249:
_temp248= _temp247.f2; goto _LL246; _LL246: if( ! Cyc_Tcutil_unify( t, _temp248)){
continue;} return Cyc_Absyn_strct( _temp250);}}{ struct _tagged_string* x=({
struct _tagged_string* _temp254=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp254[ 0]= xprintf("_tagged_ptr%d",( Cyc_Toc_tagged_ptr_counter
++)); _temp254;}); Cyc_Toc_add_tagged_ptr_decl( x, t); Cyc_Toc_tagged_ptr_types=({
struct Cyc_List_List* _temp252=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp252->hd=( void*)({ struct _tuple6* _temp253=(
struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp253->f1= x; _temp253->f2=
t; _temp253;}); _temp252->tl= Cyc_Toc_tagged_ptr_types; _temp252;}); return Cyc_Absyn_strct(
x);}} static int Cyc_Toc_temp_var_counter= 0; static struct _tuple0* Cyc_Toc_temp_var(){
return({ struct _tuple0* _temp255=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp255->f1= Cyc_Toc_abs_ns; _temp255->f2=({ struct _tagged_string*
_temp256=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp256[ 0]= xprintf("_temp%d",( Cyc_Toc_temp_var_counter ++)); _temp256;});
_temp255;});} static int Cyc_Toc_fresh_label_counter= 0; static struct
_tagged_string* Cyc_Toc_fresh_label(){ return({ struct _tagged_string* _temp257=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp257[
0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter ++)); _temp257;});} static void*
Cyc_Toc_typ_to_c( void* t); static struct _tuple1* Cyc_Toc_arg_to_c( struct
_tuple1* a){ void* _temp260; struct Cyc_Absyn_Tqual _temp262; struct Cyc_Core_Opt*
_temp264; struct _tuple1 _temp258=* a; _LL265: _temp264= _temp258.f1; goto
_LL263; _LL263: _temp262= _temp258.f2; goto _LL261; _LL261: _temp260= _temp258.f3;
goto _LL259; _LL259: return({ struct _tuple1* _temp266=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp266->f1= _temp264; _temp266->f2=
_temp262; _temp266->f3= Cyc_Toc_typ_to_c( _temp260); _temp266;});} static struct
_tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){ void* _temp269; struct Cyc_Absyn_Tqual
_temp271; struct _tuple3 _temp267=* x; _LL272: _temp271= _temp267.f1; goto
_LL270; _LL270: _temp269= _temp267.f2; goto _LL268; _LL268: return({ struct
_tuple3* _temp273=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3));
_temp273->f1= _temp271; _temp273->f2= Cyc_Toc_typ_to_c( _temp269); _temp273;});}
static void* Cyc_Toc_typ_to_c_array( void* t){ void* _temp274= t; struct Cyc_Absyn_Exp*
_temp282; struct Cyc_Absyn_Tqual _temp284; void* _temp286; int _temp288; struct
Cyc_Core_Opt* _temp290; struct Cyc_Core_Opt _temp292; void* _temp293; void*
_temp295; _LL276: if(( unsigned int) _temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_ArrayType:
0){ _LL287: _temp286=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp274)->f1;
goto _LL285; _LL285: _temp284=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp274)->f2; goto _LL283; _LL283: _temp282=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp274)->f3; goto _LL277;} else{ goto _LL278;}
_LL278: if(( unsigned int) _temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_Evar:
0){ _LL296: _temp295=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp274)->f1;
goto _LL291; _LL291: _temp290=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp274)->f2; if( _temp290 == 0){ goto _LL280;} else{ _temp292=* _temp290;
_LL294: _temp293=( void*) _temp292.v; goto _LL289;} _LL289: _temp288=( int)((
struct Cyc_Absyn_Evar_struct*) _temp274)->f3; goto _LL279;} else{ goto _LL280;}
_LL280: goto _LL281; _LL277: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp297=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp297[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp298; _temp298.tag= Cyc_Absyn_ArrayType;
_temp298.f1=( void*) Cyc_Toc_typ_to_c_array( _temp286); _temp298.f2= _temp284;
_temp298.f3= _temp282; _temp298;}); _temp297;}); _LL279: return Cyc_Toc_typ_to_c_array(
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
else{ goto _LL473;} _LL473: goto _LL474; _LL472: return Cyc_Toc_add_tagged_ptr_type(
_temp403, _temp397); _LL474: return Cyc_Absyn_star_typ( _temp403,( void*) Cyc_Absyn_HeapRgn,
_temp397); _LL470:;} _LL321: return t; _LL323: return t; _LL325: return t;
_LL327: return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp477=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp477[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp478; _temp478.tag= Cyc_Absyn_ArrayType;
_temp478.f1=( void*) Cyc_Toc_typ_to_c_array( _temp413); _temp478.f2= _temp411;
_temp478.f3= _temp409; _temp478;}); _temp477;}); _LL329: { struct Cyc_List_List*
_temp479= 0; goto _LL480; _LL480: for( 0; _temp417 != 0; _temp417=(( struct Cyc_List_List*)
_check_null( _temp417))->tl){ void* _temp481=( void*)(( struct Cyc_List_List*)
_check_null( _temp417))->hd; _LL483: if( _temp481 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL484;} else{ goto _LL485;} _LL485: if( _temp481 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL486;} else{ goto _LL487;} _LL487: if( _temp481 ==( void*) Cyc_Absyn_Const_att){
goto _LL488;} else{ goto _LL489;} _LL489: goto _LL490; _LL484: goto _LL486;
_LL486: goto _LL488; _LL488: continue; _LL490: _temp479=({ struct Cyc_List_List*
_temp491=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp491->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( _temp417))->hd);
_temp491->tl= _temp479; _temp491;}); goto _LL482; _LL482:;} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp492=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp492[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp493; _temp493.tag= Cyc_Absyn_FnType; _temp493.f1=({ struct Cyc_Absyn_FnInfo
_temp494; _temp494.tvars= 0; _temp494.effect= 0; _temp494.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp423); _temp494.args=(( struct Cyc_List_List*(*)( struct _tuple1*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp421); _temp494.varargs= _temp419; _temp494.attributes= _temp479; _temp494;});
_temp493;}); _temp492;});} _LL331: _temp429=(( struct Cyc_List_List*(*)( struct
_tuple3*(* f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp429);{ struct _tagged_string* _temp495= Cyc_Toc_add_tuple_type( _temp429);
goto _LL496; _LL496: return Cyc_Absyn_strct( _temp495);} _LL333: return( void*)({
struct Cyc_Absyn_AnonStructType_struct* _temp497=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp497[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp498; _temp498.tag= Cyc_Absyn_AnonStructType;
_temp498.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp431); _temp498;}); _temp497;}); _LL335: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp499=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp499[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp500; _temp500.tag=
Cyc_Absyn_AnonUnionType; _temp500.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp433); _temp500;}); _temp499;}); _LL337: if( _temp439 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp501=( char*)"anonymous union"; struct _tagged_string _temp502; _temp502.curr=
_temp501; _temp502.base= _temp501; _temp502.last_plus_one= _temp501 + 16;
_temp502;}));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)(( struct _tuple0*)
_check_null( _temp439))); _LL339: if( _temp445 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp503=(
char*)"anonymous struct"; struct _tagged_string _temp504; _temp504.curr=
_temp503; _temp504.base= _temp503; _temp504.last_plus_one= _temp503 + 17;
_temp504;}));} return Cyc_Absyn_strctq(( struct _tuple0*)(( struct _tuple0*)
_check_null( _temp445))); _LL341: return t; _LL343: if( _temp451 == 0){ return((
void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp505=( char*)"Toc::typ_to_c: unresolved TypedefType"; struct
_tagged_string _temp506; _temp506.curr= _temp505; _temp506.base= _temp505;
_temp506.last_plus_one= _temp505 + 38; _temp506;}));} return( void*)({ struct
Cyc_Absyn_TypedefType_struct* _temp507=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp507[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp508; _temp508.tag= Cyc_Absyn_TypedefType;
_temp508.f1= _temp455; _temp508.f2= 0; _temp508.f3=({ struct Cyc_Core_Opt*
_temp509=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp509->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp451))->v); _temp509;}); _temp508;}); _temp507;}); _LL345:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__region_handle_sp), Cyc_Toc_mt_tq);
_LL347: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp510=( char*)"Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp511; _temp511.curr= _temp510; _temp511.base= _temp510;
_temp511.last_plus_one= _temp510 + 55; _temp511;})); _LL349: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp512=( char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp513; _temp513.curr= _temp512; _temp513.base= _temp512;
_temp513.last_plus_one= _temp512 + 54; _temp513;})); _LL351: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp514=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp515; _temp515.curr= _temp514; _temp515.base= _temp514;
_temp515.last_plus_one= _temp514 + 52; _temp515;})); _LL301:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp516= t; struct Cyc_Absyn_Exp* _temp522; struct Cyc_Absyn_Tqual
_temp524; void* _temp526; _LL518: if(( unsigned int) _temp516 > 4u?*(( int*)
_temp516) == Cyc_Absyn_ArrayType: 0){ _LL527: _temp526=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp516)->f1; goto _LL525; _LL525: _temp524=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp516)->f2; goto _LL523; _LL523: _temp522=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp516)->f3; goto
_LL519;} else{ goto _LL520;} _LL520: goto _LL521; _LL519: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp526,( void*) Cyc_Absyn_HeapRgn, _temp524), e, l);
_LL521: return e; _LL517:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp528= t; struct Cyc_Absyn_Exp* _temp534; struct Cyc_Absyn_Tqual _temp536;
void* _temp538; _LL530: if(( unsigned int) _temp528 > 4u?*(( int*) _temp528) ==
Cyc_Absyn_ArrayType: 0){ _LL539: _temp538=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp528)->f1; goto _LL537; _LL537: _temp536=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp528)->f2; goto _LL535; _LL535: _temp534=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp528)->f3; goto
_LL531;} else{ goto _LL532;} _LL532: goto _LL533; _LL531: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp538,( void*) Cyc_Absyn_HeapRgn, _temp536), e, l);
_LL533: return Cyc_Absyn_cast_exp( t, e, l); _LL529:;} static int Cyc_Toc_atomic_typ(
void* t){ void* _temp540= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp576; void* _temp578; void* _temp580; struct Cyc_Absyn_Exp* _temp582; struct
Cyc_Absyn_Tqual _temp584; void* _temp586; struct Cyc_Absyn_FnInfo _temp588;
struct Cyc_List_List* _temp590; struct Cyc_Absyn_Structdecl** _temp592; struct
Cyc_List_List* _temp594; struct _tuple0* _temp596; struct Cyc_Absyn_TunionInfo
_temp598; struct Cyc_List_List* _temp600; struct Cyc_List_List* _temp602; struct
Cyc_Absyn_TunionFieldInfo _temp604; struct Cyc_List_List* _temp606; void*
_temp608; struct Cyc_Absyn_Tunionfield* _temp610; struct Cyc_Absyn_Tuniondecl*
_temp612; struct Cyc_Absyn_Uniondecl** _temp614; struct Cyc_List_List* _temp616;
struct _tuple0* _temp618; struct Cyc_Absyn_PtrInfo _temp620; void* _temp622;
_LL542: if( _temp540 ==( void*) Cyc_Absyn_VoidType){ goto _LL543;} else{ goto
_LL544;} _LL544: if(( unsigned int) _temp540 > 4u?*(( int*) _temp540) == Cyc_Absyn_VarType:
0){ _LL577: _temp576=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp540)->f1; goto _LL545;} else{ goto _LL546;} _LL546: if(( unsigned int)
_temp540 > 4u?*(( int*) _temp540) == Cyc_Absyn_IntType: 0){ _LL581: _temp580=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp540)->f1; goto _LL579; _LL579:
_temp578=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp540)->f2; goto _LL547;}
else{ goto _LL548;} _LL548: if( _temp540 ==( void*) Cyc_Absyn_FloatType){ goto
_LL549;} else{ goto _LL550;} _LL550: if( _temp540 ==( void*) Cyc_Absyn_DoubleType){
goto _LL551;} else{ goto _LL552;} _LL552: if(( unsigned int) _temp540 > 4u?*((
int*) _temp540) == Cyc_Absyn_ArrayType: 0){ _LL587: _temp586=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp540)->f1; goto _LL585; _LL585: _temp584=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp540)->f2;
goto _LL583; _LL583: _temp582=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp540)->f3; goto _LL553;} else{ goto _LL554;} _LL554: if(( unsigned int)
_temp540 > 4u?*(( int*) _temp540) == Cyc_Absyn_FnType: 0){ _LL589: _temp588=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp540)->f1; goto
_LL555;} else{ goto _LL556;} _LL556: if(( unsigned int) _temp540 > 4u?*(( int*)
_temp540) == Cyc_Absyn_TupleType: 0){ _LL591: _temp590=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp540)->f1; goto _LL557;} else{ goto
_LL558;} _LL558: if(( unsigned int) _temp540 > 4u?*(( int*) _temp540) == Cyc_Absyn_StructType:
0){ _LL597: _temp596=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp540)->f1; goto _LL595; _LL595: _temp594=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp540)->f2; goto _LL593; _LL593: _temp592=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp540)->f3;
goto _LL559;} else{ goto _LL560;} _LL560: if(( unsigned int) _temp540 > 4u?*((
int*) _temp540) == Cyc_Absyn_TunionType: 0){ _LL599: _temp598=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp540)->f1; goto _LL561;} else{ goto
_LL562;} _LL562: if(( unsigned int) _temp540 > 4u?*(( int*) _temp540) == Cyc_Absyn_AnonStructType:
0){ _LL601: _temp600=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp540)->f1; goto _LL563;} else{ goto _LL564;} _LL564: if(( unsigned int)
_temp540 > 4u?*(( int*) _temp540) == Cyc_Absyn_AnonUnionType: 0){ _LL603:
_temp602=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp540)->f1; goto _LL565;} else{ goto _LL566;} _LL566: if(( unsigned int)
_temp540 > 4u?*(( int*) _temp540) == Cyc_Absyn_TunionFieldType: 0){ _LL605:
_temp604=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp540)->f1; _LL609: _temp608=( void*) _temp604.field_info; if(*(( int*)
_temp608) == Cyc_Absyn_KnownTunionfield){ _LL613: _temp612=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp608)->f1; goto _LL611; _LL611:
_temp610=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp608)->f2; goto _LL607;} else{ goto _LL568;} _LL607: _temp606=( struct Cyc_List_List*)
_temp604.targs; goto _LL567;} else{ goto _LL568;} _LL568: if(( unsigned int)
_temp540 > 4u?*(( int*) _temp540) == Cyc_Absyn_UnionType: 0){ _LL619: _temp618=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp540)->f1; goto
_LL617; _LL617: _temp616=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp540)->f2; goto _LL615; _LL615: _temp614=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp540)->f3; goto _LL569;} else{ goto
_LL570;} _LL570: if(( unsigned int) _temp540 > 4u?*(( int*) _temp540) == Cyc_Absyn_PointerType:
0){ _LL621: _temp620=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp540)->f1; goto _LL571;} else{ goto _LL572;} _LL572: if(( unsigned int)
_temp540 > 4u?*(( int*) _temp540) == Cyc_Absyn_RgnHandleType: 0){ _LL623:
_temp622=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp540)->f1; goto
_LL573;} else{ goto _LL574;} _LL574: goto _LL575; _LL543: return 1; _LL545:
return 0; _LL547: return 1; _LL549: return 1; _LL551: return 0; _LL553: return
Cyc_Toc_atomic_typ( _temp586); _LL555: return 1; _LL557: for( 0; _temp590 != 0;
_temp590=(( struct Cyc_List_List*) _check_null( _temp590))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp590))->hd)).f2)){
return 0;}} return 1; _LL559: return 0; _LL561: return 0; _LL563: _temp602=
_temp600; goto _LL565; _LL565: for( 0; _temp602 != 0; _temp602=(( struct Cyc_List_List*)
_check_null( _temp602))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp602))->hd)->type)){ return 0;}} return
1; _LL567: return _temp610->typs == 0; _LL569: return 0; _LL571: return 0;
_LL573: return 0; _LL575:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp624= Cyc_Absynpp_typ2string( t); xprintf("atomic_typ:  bad type %.*s",
_temp624.last_plus_one - _temp624.curr, _temp624.curr);})); return 0; _LL541:;}
static int Cyc_Toc_is_void_star( void* t){ void* _temp625= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp631; struct Cyc_Absyn_Conref* _temp633; struct
Cyc_Absyn_Tqual _temp635; struct Cyc_Absyn_Conref* _temp637; void* _temp639;
void* _temp641; _LL627: if(( unsigned int) _temp625 > 4u?*(( int*) _temp625) ==
Cyc_Absyn_PointerType: 0){ _LL632: _temp631=( struct Cyc_Absyn_PtrInfo)(( struct
Cyc_Absyn_PointerType_struct*) _temp625)->f1; _LL642: _temp641=( void*) _temp631.elt_typ;
goto _LL640; _LL640: _temp639=( void*) _temp631.rgn_typ; goto _LL638; _LL638:
_temp637=( struct Cyc_Absyn_Conref*) _temp631.nullable; goto _LL636; _LL636:
_temp635=( struct Cyc_Absyn_Tqual) _temp631.tq; goto _LL634; _LL634: _temp633=(
struct Cyc_Absyn_Conref*) _temp631.bounds; goto _LL628;} else{ goto _LL629;}
_LL629: goto _LL630; _LL628: { void* _temp643= Cyc_Tcutil_compress( _temp641);
_LL645: if( _temp643 ==( void*) Cyc_Absyn_VoidType){ goto _LL646;} else{ goto
_LL647;} _LL647: goto _LL648; _LL646: return 1; _LL648: return 0; _LL644:;}
_LL630: return 0; _LL626:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_string* f){ void* _temp649= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp665; struct Cyc_List_List* _temp667; struct Cyc_Absyn_Structdecl** _temp669;
struct Cyc_Absyn_Structdecl* _temp671; struct Cyc_List_List* _temp672; struct
_tuple0* _temp674; struct Cyc_Absyn_Structdecl** _temp676; struct Cyc_List_List*
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
Cyc_Absyn_Structfield* _temp695= Cyc_Absyn_lookup_field( _temp667, f); goto
_LL696; _LL696: if( _temp695 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp697=*(( struct Cyc_Absyn_Structfield*) _check_null(
_temp695))->name; xprintf("is_poly_field: bad field %.*s", _temp697.last_plus_one
- _temp697.curr, _temp697.curr);}));} return Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp695))->type);} _LL656: if(
_temp674 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp698=( char*)"StructType name not properly set";
struct _tagged_string _temp699; _temp699.curr= _temp698; _temp699.base= _temp698;
_temp699.last_plus_one= _temp698 + 33; _temp699;}));} if( _temp671->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp700= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct _tuple0*)
_check_null( _temp674))); xprintf("is_poly_field:  struct %.*s missing fields",
_temp700.last_plus_one - _temp700.curr, _temp700.curr);}));}{ struct Cyc_List_List*
_temp701=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp671->fields))->v;
goto _LL702; _LL702: for( 0; _temp701 != 0; _temp701=(( struct Cyc_List_List*)
_check_null( _temp701))->tl){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp701))->hd)->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp701))->hd)->type);}}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp703= Cyc_Absynpp_qvar2string(( struct _tuple0*)((
struct _tuple0*) _check_null( _temp674))); struct _tagged_string _temp704=* f;
xprintf("is_poly_field:  struct %.*s has no %.*s field", _temp703.last_plus_one
- _temp703.curr, _temp703.curr, _temp704.last_plus_one - _temp704.curr, _temp704.curr);}));
return 0;} _LL658: if( _temp680 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp705=( char*)"StructType name not properly set";
struct _tagged_string _temp706; _temp706.curr= _temp705; _temp706.base= _temp705;
_temp706.last_plus_one= _temp705 + 33; _temp706;}));}(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp707= Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct _tuple0*) _check_null( _temp680))); xprintf("is_poly_field:  struct %.*s missing structdecl",
_temp707.last_plus_one - _temp707.curr, _temp707.curr);})); return 0; _LL660:
if( _temp687 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp708=( char*)"UnionType name not properly set";
struct _tagged_string _temp709; _temp709.curr= _temp708; _temp709.base= _temp708;
_temp709.last_plus_one= _temp708 + 32; _temp709;}));} if( _temp684->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp710= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct _tuple0*)
_check_null( _temp687))); xprintf("is_poly_field:  union %.*s missing fields",
_temp710.last_plus_one - _temp710.curr, _temp710.curr);}));}{ struct Cyc_List_List*
_temp711=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp684->fields))->v;
goto _LL712; _LL712: for( 0; _temp711 != 0; _temp711=(( struct Cyc_List_List*)
_check_null( _temp711))->tl){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp711))->hd)->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp711))->hd)->type);}}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp713= Cyc_Absynpp_qvar2string(( struct _tuple0*)((
struct _tuple0*) _check_null( _temp687))); struct _tagged_string _temp714=* f;
xprintf("is_poly_field:  union %.*s has no %.*s field", _temp713.last_plus_one -
_temp713.curr, _temp713.curr, _temp714.last_plus_one - _temp714.curr, _temp714.curr);}));
return 0;} _LL662: if( _temp693 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp715=( char*)"UnionType name not properly set";
struct _tagged_string _temp716; _temp716.curr= _temp715; _temp716.base= _temp715;
_temp716.last_plus_one= _temp715 + 32; _temp716;}));}(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp717= Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct _tuple0*) _check_null( _temp693))); xprintf("is_poly_field:  union %.*s missing structdecl",
_temp717.last_plus_one - _temp717.curr, _temp717.curr);})); return 0; _LL664:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp718= Cyc_Absynpp_typ2string( t); xprintf("is_poly_field:  bad type %.*s",
_temp718.last_plus_one - _temp718.curr, _temp718.curr);})); return 0; _LL650:;}
static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void* _temp719=(
void*) e->r; struct _tagged_string* _temp727; struct Cyc_Absyn_Exp* _temp729;
struct _tagged_string* _temp731; struct Cyc_Absyn_Exp* _temp733; _LL721: if(*((
int*) _temp719) == Cyc_Absyn_StructMember_e){ _LL730: _temp729=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp719)->f1; goto _LL728; _LL728:
_temp727=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp719)->f2; goto _LL722;} else{ goto _LL723;} _LL723: if(*(( int*) _temp719)
== Cyc_Absyn_StructArrow_e){ _LL734: _temp733=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp719)->f1; goto _LL732; _LL732: _temp731=(
struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp719)->f2;
goto _LL724;} else{ goto _LL725;} _LL725: goto _LL726; _LL722: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp729->topt))->v, _temp727);
_LL724: { void* _temp735= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp733->topt))->v); struct Cyc_Absyn_PtrInfo _temp741; struct Cyc_Absyn_Conref*
_temp743; struct Cyc_Absyn_Tqual _temp745; struct Cyc_Absyn_Conref* _temp747;
void* _temp749; void* _temp751; _LL737: if(( unsigned int) _temp735 > 4u?*(( int*)
_temp735) == Cyc_Absyn_PointerType: 0){ _LL742: _temp741=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp735)->f1; _LL752: _temp751=( void*)
_temp741.elt_typ; goto _LL750; _LL750: _temp749=( void*) _temp741.rgn_typ; goto
_LL748; _LL748: _temp747=( struct Cyc_Absyn_Conref*) _temp741.nullable; goto
_LL746; _LL746: _temp745=( struct Cyc_Absyn_Tqual) _temp741.tq; goto _LL744;
_LL744: _temp743=( struct Cyc_Absyn_Conref*) _temp741.bounds; goto _LL738;}
else{ goto _LL739;} _LL739: goto _LL740; _LL738: return Cyc_Toc_is_poly_field(
_temp751, _temp731); _LL740:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp753= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp733->topt))->v); xprintf("is_poly_project:  bad type %.*s",
_temp753.last_plus_one - _temp753.curr, _temp753.curr);})); return 0; _LL736:;}
_LL726: return 0; _LL720:;} static char _temp756[ 10u]="GC_malloc"; static
struct _tagged_string Cyc_Toc_gc_malloc_string=( struct _tagged_string){
_temp756, _temp756, _temp756 + 10u}; static struct _tuple0 Cyc_Toc_gc_malloc_var=(
struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc_gc_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var, 0),({
struct Cyc_List_List* _temp757=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp757->hd=( void*) s; _temp757->tl= 0; _temp757;}), 0);}
static char _temp760[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp760, _temp760, _temp760 + 17u}; static struct
_tuple0 Cyc_Toc_gc_malloc_atomic_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_atomic_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var,
0),({ struct Cyc_List_List* _temp761=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp761->hd=( void*) s; _temp761->tl= 0; _temp761;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static char _temp764[ 15u]="_region_malloc"; static struct _tagged_string
Cyc_Toc__region_malloc_string=( struct _tagged_string){ _temp764, _temp764,
_temp764 + 15u}; static struct _tuple0 Cyc_Toc_region_malloc_var=( struct
_tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__region_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_region_malloc_var,
0),({ struct Cyc_List_List* _temp765=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp765->hd=( void*) rgn; _temp765->tl=({ struct Cyc_List_List*
_temp766=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp766->hd=( void*) s; _temp766->tl= 0; _temp766;}); _temp765;}), 0);} static
const int Cyc_Toc_NullCheck= 0; struct Cyc_Toc_NullCheck_struct{ int tag; void*
f1; } ; static const int Cyc_Toc_UntagPtr= 1; struct Cyc_Toc_UntagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; int f3; } ; static const int Cyc_Toc_TagPtr=
2; struct Cyc_Toc_TagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; }
; typedef void* Cyc_Toc_conv; struct _tuple7{ void* f1; void* f2; } ; static
struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp768=({ struct _tuple7
_temp767; _temp767.f1= t1; _temp767.f2= t2; _temp767;}); void* _temp774; struct
Cyc_Absyn_PtrInfo _temp776; void* _temp778; struct Cyc_Absyn_PtrInfo _temp780;
_LL770: _LL779: _temp778= _temp768.f1; if(( unsigned int) _temp778 > 4u?*(( int*)
_temp778) == Cyc_Absyn_PointerType: 0){ _LL781: _temp780=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp778)->f1; goto _LL775;} else{ goto
_LL772;} _LL775: _temp774= _temp768.f2; if(( unsigned int) _temp774 > 4u?*(( int*)
_temp774) == Cyc_Absyn_PointerType: 0){ _LL777: _temp776=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp774)->f1; goto _LL771;} else{ goto
_LL772;} _LL772: goto _LL773; _LL771: { int _temp782=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp780.nullable); goto _LL783; _LL783: { int
_temp784=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp776.nullable);
goto _LL785; _LL785: { void* _temp786=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp780.bounds); goto _LL787; _LL787: { void* _temp788=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp776.bounds);
goto _LL789; _LL789: { struct Cyc_List_List* convs= 0;{ struct _tuple7 _temp791=({
struct _tuple7 _temp790; _temp790.f1= _temp786; _temp790.f2= _temp788; _temp790;});
void* _temp801; struct Cyc_Absyn_Exp* _temp803; void* _temp805; struct Cyc_Absyn_Exp*
_temp807; void* _temp809; void* _temp811; void* _temp813; void* _temp815; struct
Cyc_Absyn_Exp* _temp817; void* _temp819; struct Cyc_Absyn_Exp* _temp821; void*
_temp823; _LL793: _LL806: _temp805= _temp791.f1; if(( unsigned int) _temp805 > 1u?*((
int*) _temp805) == Cyc_Absyn_Upper_b: 0){ _LL808: _temp807=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp805)->f1; goto _LL802;} else{ goto _LL795;}
_LL802: _temp801= _temp791.f2; if(( unsigned int) _temp801 > 1u?*(( int*)
_temp801) == Cyc_Absyn_Upper_b: 0){ _LL804: _temp803=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp801)->f1; goto _LL794;} else{ goto _LL795;}
_LL795: _LL812: _temp811= _temp791.f1; if( _temp811 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL810;} else{ goto _LL797;} _LL810: _temp809= _temp791.f2; if( _temp809 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL796;} else{ goto _LL797;} _LL797: _LL816:
_temp815= _temp791.f1; if(( unsigned int) _temp815 > 1u?*(( int*) _temp815) ==
Cyc_Absyn_Upper_b: 0){ _LL818: _temp817=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp815)->f1; goto _LL814;} else{ goto _LL799;} _LL814: _temp813= _temp791.f2;
if( _temp813 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL798;} else{ goto _LL799;}
_LL799: _LL824: _temp823= _temp791.f1; if( _temp823 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL820;} else{ goto _LL792;} _LL820: _temp819= _temp791.f2; if((
unsigned int) _temp819 > 1u?*(( int*) _temp819) == Cyc_Absyn_Upper_b: 0){ _LL822:
_temp821=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp819)->f1;
goto _LL800;} else{ goto _LL792;} _LL794: if( _temp782? ! _temp784: 0){ convs=({
struct Cyc_List_List* _temp825=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp825->hd=( void*)(( void*)({ struct Cyc_Toc_NullCheck_struct*
_temp826=( struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp826[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp827; _temp827.tag= Cyc_Toc_NullCheck;
_temp827.f1=( void*) t2; _temp827;}); _temp826;})); _temp825->tl= convs;
_temp825;});} goto _LL792; _LL796: goto _LL792; _LL798: convs=({ struct Cyc_List_List*
_temp828=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp828->hd=( void*)(( void*)({ struct Cyc_Toc_TagPtr_struct* _temp829=( struct
Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp829[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp830; _temp830.tag= Cyc_Toc_TagPtr;
_temp830.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp830.f2= _temp817; _temp830;});
_temp829;})); _temp828->tl= convs; _temp828;}); goto _LL792; _LL800: convs=({
struct Cyc_List_List* _temp831=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp831->hd=( void*)(( void*)({ struct Cyc_Toc_UntagPtr_struct*
_temp832=( struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp832[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp833; _temp833.tag= Cyc_Toc_UntagPtr;
_temp833.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp833.f2= _temp821; _temp833.f3=
_temp784; _temp833;}); _temp832;})); _temp831->tl= convs; _temp831;}); goto
_LL792; _LL792:;} return convs;}}}}} _LL773: return 0; _LL769:;}} static char
_temp836[ 7u]="_throw"; static struct _tagged_string Cyc_Toc__throw_string=(
struct _tagged_string){ _temp836, _temp836, _temp836 + 7u}; static struct
_tuple0 Cyc_Toc__throw_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__throw_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__throw_var,
0),({ struct Cyc_List_List* _temp837=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp837->hd=( void*) e; _temp837->tl= 0; _temp837;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp838=( char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp839; _temp839.curr= _temp838; _temp839.base= _temp838;
_temp839.last_plus_one= _temp838 + 44; _temp839;}));} if( Cyc_Toc_use_runtimecheck_funs){
return Cyc_Absyn_cast_exp( new_typ, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp840=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp840->hd=( void*) e; _temp840->tl= 0; _temp840;}), 0),
0);} else{ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0),
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0),
Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
x, 0), 0), 0); return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, new_typ,(
struct Cyc_Absyn_Exp*) e, s, 0), 0);}} static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv(
int toplevel, void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){
struct _tuple0* a= Cyc_Toc_temp_var(); struct _tuple0* b= Cyc_Toc_temp_var();
int szvalue=( int) Cyc_Evexp_eval_const_uint_exp( sz); void* strt_typ; void*
elt_typ;{ void* _temp841= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp847; struct Cyc_Absyn_Conref* _temp849; struct Cyc_Absyn_Tqual _temp851;
struct Cyc_Absyn_Conref* _temp853; void* _temp855; void* _temp857; _LL843: if((
unsigned int) _temp841 > 4u?*(( int*) _temp841) == Cyc_Absyn_PointerType: 0){
_LL848: _temp847=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp841)->f1; _LL858: _temp857=( void*) _temp847.elt_typ; goto _LL856; _LL856:
_temp855=( void*) _temp847.rgn_typ; goto _LL854; _LL854: _temp853=( struct Cyc_Absyn_Conref*)
_temp847.nullable; goto _LL852; _LL852: _temp851=( struct Cyc_Absyn_Tqual)
_temp847.tq; goto _LL850; _LL850: _temp849=( struct Cyc_Absyn_Conref*) _temp847.bounds;
goto _LL844;} else{ goto _LL845;} _LL845: goto _LL846; _LL844: elt_typ= _temp857;
strt_typ= Cyc_Toc_add_tagged_ptr_type( _temp857, Cyc_Toc_mt_tq); goto _LL842;
_LL846: return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp859=( char*)"tag_ptr_conv: not a pointer";
struct _tagged_string _temp860; _temp860.curr= _temp859; _temp860.base= _temp859;
_temp860.last_plus_one= _temp859 + 28; _temp860;})); _LL842:;} if( toplevel){
int is_string= 0;{ void* _temp861=( void*) e->r; void* _temp867; struct
_tagged_string _temp869; _LL863: if(*(( int*) _temp861) == Cyc_Absyn_Const_e){
_LL868: _temp867=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp861)->f1; if((
unsigned int) _temp867 > 1u?*(( int*) _temp867) == Cyc_Absyn_String_c: 0){
_LL870: _temp869=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp867)->f1; goto _LL864;} else{ goto _LL865;}} else{ goto _LL865;} _LL865:
goto _LL866; _LL864: is_string= 1; goto _LL862; _LL866: goto _LL862; _LL862:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp874=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp874[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp875; _temp875.tag= Cyc_Absyn_ArrayType; _temp875.f1=(
void*) elt_typ; _temp875.f2= Cyc_Toc_mt_tq; _temp875.f3=( struct Cyc_Absyn_Exp*)
sz; _temp875;}); _temp874;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp871=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp871->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp872=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp872[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp873; _temp873.tag= Cyc_Absyn_Var_d;
_temp873.f1= vd; _temp873;}); _temp872;}), 0); _temp871->tl= Cyc_Toc_result_decls;
_temp871;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp876=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp876->hd=( void*)({ struct _tuple4* _temp881=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp881->f1= 0; _temp881->f2=
xexp; _temp881;}); _temp876->tl=({ struct Cyc_List_List* _temp877=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp877->hd=( void*)({ struct
_tuple4* _temp880=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp880->f1= 0; _temp880->f2= xexp; _temp880;}); _temp877->tl=({ struct Cyc_List_List*
_temp878=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp878->hd=( void*)({ struct _tuple4* _temp879=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp879->f1= 0; _temp879->f2= xplussz; _temp879;});
_temp878->tl= 0; _temp878;}); _temp877;}); _temp876;}), 0); return urm_exp;}}}
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
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp882=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp883; _temp883.curr= _temp882; _temp883.base= _temp882; _temp883.last_plus_one=
_temp882 + 45; _temp883;}));}{ struct _tuple0* x= Cyc_Toc_temp_var(); int
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
_check_null( cs))->tl){ void* _temp884=( void*)(( struct Cyc_List_List*)
_check_null( cs))->hd; void* _temp892; struct Cyc_Absyn_Exp* _temp894; void*
_temp896; int _temp898; struct Cyc_Absyn_Exp* _temp900; void* _temp902; _LL886:
if(*(( int*) _temp884) == Cyc_Toc_NullCheck){ _LL893: _temp892=( void*)(( struct
Cyc_Toc_NullCheck_struct*) _temp884)->f1; goto _LL887;} else{ goto _LL888;}
_LL888: if(*(( int*) _temp884) == Cyc_Toc_TagPtr){ _LL897: _temp896=( void*)((
struct Cyc_Toc_TagPtr_struct*) _temp884)->f1; goto _LL895; _LL895: _temp894=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_TagPtr_struct*) _temp884)->f2; goto
_LL889;} else{ goto _LL890;} _LL890: if(*(( int*) _temp884) == Cyc_Toc_UntagPtr){
_LL903: _temp902=( void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp884)->f1; goto
_LL901; _LL901: _temp900=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_UntagPtr_struct*)
_temp884)->f2; goto _LL899; _LL899: _temp898=( int)(( struct Cyc_Toc_UntagPtr_struct*)
_temp884)->f3; goto _LL891;} else{ goto _LL885;} _LL887: e= Cyc_Toc_null_check_conv(
toplevel, _temp892, e); goto _LL885; _LL889: e= Cyc_Toc_tag_ptr_conv( toplevel,
_temp896, _temp894, e); goto _LL885; _LL891: e= Cyc_Toc_untag_ptr_conv( toplevel,
_temp902, _temp900, _temp898, e); goto _LL885; _LL885:;}} return e;} struct Cyc_Toc_Env{
struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt*
fallthru_info; struct Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct
Cyc_Toc_Env* Cyc_Toc_env_t; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){
return({ struct Cyc_Toc_Env* _temp904=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp904->break_lab= 0; _temp904->continue_lab= 0;
_temp904->fallthru_info= 0; _temp904->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp904->toplevel= 1; _temp904;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env(
struct Cyc_Toc_Env* e){ return({ struct Cyc_Toc_Env* _temp905=( struct Cyc_Toc_Env*)
GC_malloc( sizeof( struct Cyc_Toc_Env)); _temp905->break_lab= e->break_lab;
_temp905->continue_lab= e->continue_lab; _temp905->fallthru_info= e->fallthru_info;
_temp905->varmap= e->varmap; _temp905->toplevel= e->toplevel; _temp905;});}
static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env* e, struct
_tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp906=(* x).f1; struct Cyc_List_List*
_temp912; _LL908: if(( unsigned int) _temp906 > 1u?*(( int*) _temp906) == Cyc_Absyn_Rel_n:
0){ _LL913: _temp912=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp906)->f1; goto _LL909;} else{ goto _LL910;} _LL910: goto _LL911; _LL909:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp914=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp914[ 0]=({ struct
Cyc_Core_Impossible_struct _temp915; _temp915.tag= Cyc_Core_Impossible; _temp915.f1=({
struct _tagged_string _temp916= Cyc_Absynpp_qvar2string( x); xprintf("Toc::add_varmap on Rel_n: %.*s\n",
_temp916.last_plus_one - _temp916.curr, _temp916.curr);}); _temp915;}); _temp914;}));
_LL911: goto _LL907; _LL907:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap, x, y); return
ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple8{ struct _tagged_string* f1; struct Cyc_List_List*
f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l, struct _tagged_string*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp917= 0; goto _LL918; _LL918: for( 0;
fallthru_binders != 0; fallthru_binders=(( struct Cyc_List_List*) _check_null(
fallthru_binders))->tl){ _temp917=({ struct Cyc_List_List* _temp919=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp919->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp919->tl=
_temp917; _temp919;});} _temp917=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp917);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp920=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp920->v=( void*) break_l; _temp920;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp921=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp921->v=( void*)({ struct _tuple8*
_temp922=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp922->f1=
fallthru_l; _temp922->f2= _temp917; _temp922->f3= next_case_env->varmap;
_temp922;}); _temp921;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp923=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp923->v=( void*)
break_l; _temp923;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_string*
next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp924=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp924->v=( void*)({ struct _tuple8* _temp925=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp925->f1= next_l; _temp925->f2= 0;
_temp925->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp925;}); _temp924;});
return ans;} struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; char f5; int f6; } ; struct
_tuple10{ struct _tuple0* f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; static
struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c( void* p, struct Cyc_Core_Opt* aopt,
struct Cyc_List_List* args, struct Cyc_List_List* typs){ if( args == 0){ return((
struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp926=( char*)"expecting a literal format string";
struct _tagged_string _temp927; _temp927.curr= _temp926; _temp927.base= _temp926;
_temp927.last_plus_one= _temp926 + 34; _temp927;}));}{ struct _tagged_string
fmt_str;{ void* _temp928=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd)->r; void* _temp934; struct _tagged_string _temp936;
_LL930: if(*(( int*) _temp928) == Cyc_Absyn_Const_e){ _LL935: _temp934=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp928)->f1; if(( unsigned int) _temp934 > 1u?*((
int*) _temp934) == Cyc_Absyn_String_c: 0){ _LL937: _temp936=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp934)->f1; goto _LL931;}
else{ goto _LL932;}} else{ goto _LL932;} _LL932: goto _LL933; _LL931: fmt_str=
_temp936; goto _LL929; _LL933: return(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp938=(
char*)"expecting a literal format string"; struct _tagged_string _temp939;
_temp939.curr= _temp938; _temp939.base= _temp938; _temp939.last_plus_one=
_temp938 + 34; _temp939;})); _LL929:;}{ int len=( int)({ struct _tagged_string
_temp1032= fmt_str;( unsigned int)( _temp1032.last_plus_one - _temp1032.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str, sizeof( char),
i) =='%'){ i ++; if( i < len?*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( fmt_str, sizeof( char), i) =='s': 0){
break;}}} if( i == len){ if( aopt != 0){ args=({ struct Cyc_List_List* _temp940=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp940->hd=(
void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( aopt))->v);
_temp940->tl= args; _temp940;});} return Cyc_Absyn_primop_exp( p, args, 0);}
args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl;{ struct Cyc_List_List* rev_fmt= 0; struct Cyc_List_List*
rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i= 0; i < len; i ++){
char c=*(( char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( fmt_str, sizeof( char), i); rev_fmt=({ struct Cyc_List_List*
_temp941=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp941->hd=( void*)(( int) c); _temp941->tl= rev_fmt; _temp941;}); if( c !='%'){
continue;}{ struct Cyc_Core_Opt* _temp942= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); goto _LL943; _LL943: if( _temp942 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp944=(
char*)"bad format string"; struct _tagged_string _temp945; _temp945.curr=
_temp944; _temp945.base= _temp944; _temp945.last_plus_one= _temp944 + 18;
_temp945;}));}{ struct _tuple9 _temp948; int _temp949; char _temp951; struct Cyc_List_List*
_temp953; struct Cyc_List_List* _temp955; struct Cyc_List_List* _temp957; struct
Cyc_List_List* _temp959; struct _tuple9* _temp946=( struct _tuple9*)(( struct
Cyc_Core_Opt*) _check_null( _temp942))->v; _temp948=* _temp946; _LL960: _temp959=
_temp948.f1; goto _LL958; _LL958: _temp957= _temp948.f2; goto _LL956; _LL956:
_temp955= _temp948.f3; goto _LL954; _LL954: _temp953= _temp948.f4; goto _LL952;
_LL952: _temp951= _temp948.f5; goto _LL950; _LL950: _temp949= _temp948.f6; goto
_LL947; _LL947: i= _temp949 - 1; if( _temp951 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct Cyc_List_List*
_temp961=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp961->hd=( void*) _temp959; _temp961->tl=({ struct Cyc_List_List* _temp962=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp962->hd=(
void*) _temp957; _temp962->tl=({ struct Cyc_List_List* _temp963=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp963->hd=( void*) _temp955;
_temp963->tl=({ struct Cyc_List_List* _temp964=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp964->hd=( void*) _temp953;
_temp964->tl= 0; _temp964;}); _temp963;}); _temp962;}); _temp961;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp965=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp965->hd=( void*)(( int) _temp951); _temp965->tl=
rev_fmt; _temp965;});{ struct Cyc_List_List* _temp966= _temp957; struct Cyc_List_List
_temp973; struct Cyc_List_List* _temp974; int _temp976; _LL968: if( _temp966 ==
0){ goto _LL970;} else{ _temp973=* _temp966; _LL977: _temp976=( int) _temp973.hd;
goto _LL975; _LL975: _temp974=( struct Cyc_List_List*) _temp973.tl; if( _temp974
== 0){ goto _LL972;} else{ goto _LL970;}} _LL972: if( _temp976 =='*'){ goto
_LL969;} else{ goto _LL970;} _LL970: goto _LL971; _LL969: { struct _tuple0*
_temp978= Cyc_Toc_temp_var(); goto _LL979; _LL979: rev_temps=({ struct Cyc_List_List*
_temp980=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp980->hd=( void*)({ struct _tuple10* _temp981=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp981->f1= _temp978; _temp981->f2=( void*)((
struct Cyc_List_List*) _check_null( typs))->hd; _temp981->f3=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd; _temp981;}); _temp980->tl=
rev_temps; _temp980;}); rev_result=({ struct Cyc_List_List* _temp982=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp982->hd=( void*)
Cyc_Absyn_var_exp( _temp978, 0); _temp982->tl= rev_result; _temp982;}); args=((
struct Cyc_List_List*) _check_null( args))->tl; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl; goto _LL967;} _LL971: goto _LL967; _LL967:;}{ struct
Cyc_List_List* _temp983= _temp955; struct Cyc_List_List _temp990; struct Cyc_List_List*
_temp991; struct Cyc_List_List _temp993; struct Cyc_List_List* _temp994; int
_temp996; int _temp998; _LL985: if( _temp983 == 0){ goto _LL987;} else{ _temp990=*
_temp983; _LL999: _temp998=( int) _temp990.hd; goto _LL992; _LL992: _temp991=(
struct Cyc_List_List*) _temp990.tl; if( _temp991 == 0){ goto _LL987;} else{
_temp993=* _temp991; _LL997: _temp996=( int) _temp993.hd; goto _LL995; _LL995:
_temp994=( struct Cyc_List_List*) _temp993.tl; if( _temp994 == 0){ goto _LL989;}
else{ goto _LL987;}}} _LL989: if( _temp998 =='.'? _temp996 =='*': 0){ goto
_LL986;} else{ goto _LL987;} _LL987: goto _LL988; _LL986: { struct _tuple0*
_temp1000= Cyc_Toc_temp_var(); goto _LL1001; _LL1001: rev_temps=({ struct Cyc_List_List*
_temp1002=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1002->hd=( void*)({ struct _tuple10* _temp1003=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1003->f1= _temp1000; _temp1003->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp1003->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp1003;});
_temp1002->tl= rev_temps; _temp1002;}); rev_result=({ struct Cyc_List_List*
_temp1004=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1004->hd=( void*) Cyc_Absyn_var_exp( _temp1000, 0); _temp1004->tl=
rev_result; _temp1004;}); args=(( struct Cyc_List_List*) _check_null( args))->tl;
typs=(( struct Cyc_List_List*) _check_null( typs))->tl; goto _LL984;} _LL988:
goto _LL984; _LL984:;} if( _temp951 !='%'){ if( args == 0? 1: typs == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1005=
Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s", _temp1005.last_plus_one
- _temp1005.curr, _temp1005.curr);}));}{ struct _tuple0* _temp1006= Cyc_Toc_temp_var();
goto _LL1007; _LL1007: rev_temps=({ struct Cyc_List_List* _temp1008=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1008->hd=( void*)({ struct
_tuple10* _temp1009=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp1009->f1= _temp1006; _temp1009->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp1009->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp1009;}); _temp1008->tl= rev_temps; _temp1008;});
rev_result=({ struct Cyc_List_List* _temp1010=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1010->hd=( void*) Cyc_Absyn_var_exp(
_temp1006, 0); _temp1010->tl= rev_result; _temp1010;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp1011=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1011->hd=( void*)(( int)'.');
_temp1011->tl= rev_fmt; _temp1011;}); rev_fmt=({ struct Cyc_List_List* _temp1012=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1012->hd=(
void*)(( int)'*'); _temp1012->tl= rev_fmt; _temp1012;}); rev_fmt=({ struct Cyc_List_List*
_temp1013=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1013->hd=( void*)(( int)'s'); _temp1013->tl= rev_fmt; _temp1013;});{ struct
_tuple0* _temp1014= Cyc_Toc_temp_var(); goto _LL1015; _LL1015: rev_temps=({
struct Cyc_List_List* _temp1016=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1016->hd=( void*)({ struct _tuple10* _temp1017=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1017->f1= _temp1014;
_temp1017->f2=( void*)(( struct Cyc_List_List*) _check_null( typs))->hd;
_temp1017->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
args))->hd; _temp1017;}); _temp1016->tl= rev_temps; _temp1016;});{ struct Cyc_Absyn_Exp*
_temp1018= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1014, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1014, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL1019; _LL1019: { struct Cyc_Absyn_Exp*
_temp1020= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( _temp1014, 0), Cyc_Toc_curr_sp,
0); goto _LL1021; _LL1021: rev_result=({ struct Cyc_List_List* _temp1022=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1022->hd=(
void*) _temp1018; _temp1022->tl= rev_result; _temp1022;}); rev_result=({ struct
Cyc_List_List* _temp1023=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1023->hd=( void*) _temp1020; _temp1023->tl= rev_result; _temp1023;}); args=((
struct Cyc_List_List*) _check_null( args))->tl; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl;}}}}}}}{ struct Cyc_List_List* _temp1025=({ struct Cyc_List_List*
_temp1024=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1024->hd=( void*) Cyc_Absyn_string_exp( Cyc_String_implode((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( rev_fmt)), 0); _temp1024->tl=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( rev_result);
_temp1024;}); goto _LL1026; _LL1026: if( aopt != 0){ _temp1025=({ struct Cyc_List_List*
_temp1027=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1027->hd=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( aopt))->v); _temp1027->tl= _temp1025; _temp1027;});}{ struct Cyc_Absyn_Exp*
_temp1028= Cyc_Absyn_primop_exp( p, _temp1025, 0); goto _LL1029; _LL1029: {
struct Cyc_Absyn_Stmt* _temp1030= Cyc_Absyn_exp_stmt( _temp1028, 0); goto
_LL1031; _LL1031: for( 0; rev_temps != 0; rev_temps=(( struct Cyc_List_List*)
_check_null( rev_temps))->tl){ _temp1030= Cyc_Absyn_declare_stmt((*(( struct
_tuple10*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f1,(*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f2,(
struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f3, _temp1030, 0);} return Cyc_Absyn_stmt_exp(
_temp1030, 0);}}}}}}} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){
if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1033=( char*)"Missing type in primop ";
struct _tagged_string _temp1034; _temp1034.curr= _temp1033; _temp1034.base=
_temp1033; _temp1034.last_plus_one= _temp1033 + 24; _temp1034;}));} return Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1035=( char*)"Missing type in primop ";
struct _tagged_string _temp1036; _temp1036.curr= _temp1035; _temp1036.base=
_temp1035; _temp1036.last_plus_one= _temp1035 + 24; _temp1036;}));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1037=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1037->f1= Cyc_Toc_mt_tq; _temp1037->f2=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_temp1037;});} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4*
_temp1038=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1038[
0]=({ struct _tuple4 _temp1039; _temp1039.f1= 0; _temp1039.f2= e; _temp1039;});
_temp1038;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
_temp1040= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); goto _LL1041; _LL1041: if(
rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic(
_temp1040): Cyc_Toc_malloc_ptr( _temp1040), 0);} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( rgnopt)); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp1040), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp1042=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); goto _LL1043; _LL1043: for( 0;
_temp1042 != 0; _temp1042=(( struct Cyc_List_List*) _check_null( _temp1042))->tl){
struct _tuple4 _temp1046; struct Cyc_Absyn_Exp* _temp1047; struct Cyc_List_List*
_temp1049; struct _tuple4* _temp1044=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1042))->hd; _temp1046=* _temp1044; _LL1050: _temp1049=
_temp1046.f1; goto _LL1048; _LL1048: _temp1047= _temp1046.f2; goto _LL1045;
_LL1045: { struct Cyc_Absyn_Exp* e_index; if( _temp1049 == 0){ e_index= Cyc_Absyn_signed_int_exp((
count --), 0);} else{ if((( struct Cyc_List_List*) _check_null( _temp1049))->tl
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp1051=( char*)"multiple designators in array";
struct _tagged_string _temp1052; _temp1052.curr= _temp1051; _temp1052.base=
_temp1051; _temp1052.last_plus_one= _temp1051 + 30; _temp1052;}));}{ void*
_temp1053=( void*)(( struct Cyc_List_List*) _check_null( _temp1049))->hd; goto
_LL1054; _LL1054: { void* _temp1055= _temp1053; struct Cyc_Absyn_Exp* _temp1061;
struct _tagged_string* _temp1063; _LL1057: if(*(( int*) _temp1055) == Cyc_Absyn_ArrayElement){
_LL1062: _temp1061=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1055)->f1; goto _LL1058;} else{ goto _LL1059;} _LL1059: if(*(( int*)
_temp1055) == Cyc_Absyn_FieldName){ _LL1064: _temp1063=( struct _tagged_string*)((
struct Cyc_Absyn_FieldName_struct*) _temp1055)->f1; goto _LL1060;} else{ goto
_LL1056;} _LL1058: Cyc_Toc_exp_to_c( nv, _temp1061); e_index= _temp1061; goto
_LL1056; _LL1060: e_index=(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1065=( char*)"field name designators in array";
struct _tagged_string _temp1066; _temp1066.curr= _temp1065; _temp1066.base=
_temp1065; _temp1066.last_plus_one= _temp1065 + 32; _temp1066;})); goto _LL1056;
_LL1056:;}}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index,
0); void* _temp1067=( void*) _temp1047->r; struct Cyc_List_List* _temp1077;
struct Cyc_Absyn_Exp* _temp1079; struct Cyc_Absyn_Exp* _temp1081; struct Cyc_Absyn_Vardecl*
_temp1083; struct Cyc_List_List* _temp1085; void* _temp1087; _LL1069: if(*(( int*)
_temp1067) == Cyc_Absyn_Array_e){ _LL1078: _temp1077=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1067)->f1; goto _LL1070;} else{ goto
_LL1071;} _LL1071: if(*(( int*) _temp1067) == Cyc_Absyn_Comprehension_e){
_LL1084: _temp1083=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1067)->f1; goto _LL1082; _LL1082: _temp1081=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1067)->f2; goto _LL1080; _LL1080:
_temp1079=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1067)->f3; goto _LL1072;} else{ goto _LL1073;} _LL1073: if(*(( int*)
_temp1067) == Cyc_Absyn_AnonStruct_e){ _LL1088: _temp1087=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1067)->f1; goto _LL1086; _LL1086: _temp1085=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1067)->f2; goto _LL1074;} else{ goto
_LL1075;} _LL1075: goto _LL1076; _LL1070: s= Cyc_Toc_init_array( nv, lval,
_temp1077, s); goto _LL1068; _LL1072: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1083, _temp1081, _temp1079, s, 0); goto _LL1068; _LL1074: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp1087, _temp1085, s); goto _LL1068; _LL1076: Cyc_Toc_exp_to_c( nv,
_temp1047); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp1047, 0), s, 0); goto _LL1068; _LL1068:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp1089= vd->name; goto _LL1090;
_LL1090: if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp1089, Cyc_Absyn_varb_exp( _temp1089,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1113=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1113[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1114; _temp1114.tag= Cyc_Absyn_Local_b; _temp1114.f1=
vd; _temp1114;}); _temp1113;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( _temp1089, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct
Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( _temp1089, 0), Cyc_Absyn_var_exp(
max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp(
_temp1089, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp(
_temp1089, 0), 0); struct Cyc_Absyn_Stmt* body;{ void* _temp1091=( void*) e2->r;
struct Cyc_List_List* _temp1101; struct Cyc_Absyn_Exp* _temp1103; struct Cyc_Absyn_Exp*
_temp1105; struct Cyc_Absyn_Vardecl* _temp1107; struct Cyc_List_List* _temp1109;
void* _temp1111; _LL1093: if(*(( int*) _temp1091) == Cyc_Absyn_Array_e){ _LL1102:
_temp1101=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1091)->f1;
goto _LL1094;} else{ goto _LL1095;} _LL1095: if(*(( int*) _temp1091) == Cyc_Absyn_Comprehension_e){
_LL1108: _temp1107=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1091)->f1; goto _LL1106; _LL1106: _temp1105=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1091)->f2; goto _LL1104; _LL1104:
_temp1103=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1091)->f3; goto _LL1096;} else{ goto _LL1097;} _LL1097: if(*(( int*)
_temp1091) == Cyc_Absyn_AnonStruct_e){ _LL1112: _temp1111=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1091)->f1; goto _LL1110; _LL1110: _temp1109=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1091)->f2; goto _LL1098;} else{ goto
_LL1099;} _LL1099: goto _LL1100; _LL1094: body= Cyc_Toc_init_array( nv2, lval,
_temp1101, Cyc_Absyn_skip_stmt( 0)); goto _LL1092; _LL1096: body= Cyc_Toc_init_comprehension(
nv2, lval, _temp1107, _temp1105, _temp1103, Cyc_Absyn_skip_stmt( 0), 0); goto
_LL1092; _LL1098: body= Cyc_Toc_init_anon_struct( nv, lval, _temp1111, _temp1109,
Cyc_Absyn_skip_stmt( 0)); goto _LL1092; _LL1100: Cyc_Toc_exp_to_c( nv2, e2);
body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL1092; _LL1092:;}{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt(
max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp1089, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct
Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
lhs, void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp1115=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); goto _LL1116; _LL1116: for( 0; _temp1115 != 0;
_temp1115=(( struct Cyc_List_List*) _check_null( _temp1115))->tl){ struct
_tuple4 _temp1119; struct Cyc_Absyn_Exp* _temp1120; struct Cyc_List_List*
_temp1122; struct _tuple4* _temp1117=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1115))->hd; _temp1119=* _temp1117; _LL1123: _temp1122=
_temp1119.f1; goto _LL1121; _LL1121: _temp1120= _temp1119.f2; goto _LL1118;
_LL1118: if( _temp1122 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1124=( char*)"empty designator list"; struct
_tagged_string _temp1125; _temp1125.curr= _temp1124; _temp1125.base= _temp1124;
_temp1125.last_plus_one= _temp1124 + 22; _temp1125;}));} if((( struct Cyc_List_List*)
_check_null( _temp1122))->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1126=( char*)"too many designators in anonymous struct";
struct _tagged_string _temp1127; _temp1127.curr= _temp1126; _temp1127.base=
_temp1126; _temp1127.last_plus_one= _temp1126 + 41; _temp1127;}));}{ void*
_temp1128=( void*)(( struct Cyc_List_List*) _check_null( _temp1122))->hd; struct
_tagged_string* _temp1134; _LL1130: if(*(( int*) _temp1128) == Cyc_Absyn_FieldName){
_LL1135: _temp1134=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1128)->f1; goto _LL1131;} else{ goto _LL1132;} _LL1132: goto _LL1133;
_LL1131: { struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs,
_temp1134, 0);{ void* _temp1136=( void*) _temp1120->r; struct Cyc_List_List*
_temp1146; struct Cyc_Absyn_Exp* _temp1148; struct Cyc_Absyn_Exp* _temp1150;
struct Cyc_Absyn_Vardecl* _temp1152; struct Cyc_List_List* _temp1154; void*
_temp1156; _LL1138: if(*(( int*) _temp1136) == Cyc_Absyn_Array_e){ _LL1147:
_temp1146=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1136)->f1;
goto _LL1139;} else{ goto _LL1140;} _LL1140: if(*(( int*) _temp1136) == Cyc_Absyn_Comprehension_e){
_LL1153: _temp1152=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1136)->f1; goto _LL1151; _LL1151: _temp1150=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1136)->f2; goto _LL1149; _LL1149:
_temp1148=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1136)->f3; goto _LL1141;} else{ goto _LL1142;} _LL1142: if(*(( int*)
_temp1136) == Cyc_Absyn_AnonStruct_e){ _LL1157: _temp1156=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1136)->f1; goto _LL1155; _LL1155: _temp1154=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1136)->f2; goto _LL1143;} else{ goto
_LL1144;} _LL1144: goto _LL1145; _LL1139: s= Cyc_Toc_init_array( nv, lval,
_temp1146, s); goto _LL1137; _LL1141: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1152, _temp1150, _temp1148, s, 0); goto _LL1137; _LL1143: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp1156, _temp1154, s); goto _LL1137; _LL1145: Cyc_Toc_exp_to_c( nv,
_temp1120); if( Cyc_Toc_is_poly_field( struct_type, _temp1134)){ _temp1120= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1120, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp1120, 0), 0), s, 0); goto _LL1137; _LL1137:;}
goto _LL1129;} _LL1133:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1158=( char*)"array designator in struct";
struct _tagged_string _temp1159; _temp1159.curr= _temp1158; _temp1159.base=
_temp1158; _temp1159.last_plus_one= _temp1158 + 27; _temp1159;}))); _LL1129:;}}}
return s;} static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env*
nv, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
Cyc_List_List* _temp1160=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
es); goto _LL1161; _LL1161: { struct _tagged_string* _temp1162= Cyc_Toc_add_tuple_type(
_temp1160); goto _LL1163; _LL1163: { struct _tuple0* _temp1164= Cyc_Toc_temp_var();
goto _LL1165; _LL1165: { struct Cyc_Absyn_Exp* _temp1166= Cyc_Absyn_var_exp(
_temp1164, 0); goto _LL1167; _LL1167: { struct Cyc_Absyn_Stmt* _temp1168= Cyc_Absyn_exp_stmt(
_temp1166, 0); goto _LL1169; _LL1169: { struct Cyc_Absyn_Exp*(* _temp1170)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1171;
_LL1171: { int is_atomic= 1; es=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( es);{ int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es); for( 0; es != 0; es=(( struct Cyc_List_List*) _check_null( es))->tl, -- i){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd; struct Cyc_Absyn_Exp* lval= _temp1170( _temp1166,({
struct _tagged_string* _temp1189=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp1189[ 0]= xprintf("f%d", i); _temp1189;}), 0);
is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp1172=( void*) e->r; struct Cyc_List_List*
_temp1180; struct Cyc_Absyn_Exp* _temp1182; struct Cyc_Absyn_Exp* _temp1184;
struct Cyc_Absyn_Vardecl* _temp1186; _LL1174: if(*(( int*) _temp1172) == Cyc_Absyn_Array_e){
_LL1181: _temp1180=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1172)->f1; goto _LL1175;} else{ goto _LL1176;} _LL1176: if(*(( int*)
_temp1172) == Cyc_Absyn_Comprehension_e){ _LL1187: _temp1186=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1172)->f1; goto _LL1185; _LL1185:
_temp1184=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1172)->f2; goto _LL1183; _LL1183: _temp1182=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1172)->f3; goto _LL1177;} else{
goto _LL1178;} _LL1178: goto _LL1179; _LL1175: _temp1168= Cyc_Toc_init_array( nv,
lval, _temp1180, _temp1168); goto _LL1173; _LL1177: _temp1168= Cyc_Toc_init_comprehension(
nv, lval, _temp1186, _temp1184, _temp1182, _temp1168, 0); goto _LL1173; _LL1179:
Cyc_Toc_exp_to_c( nv, e); _temp1168= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1170( _temp1166,({ struct _tagged_string* _temp1188=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1188[ 0]= xprintf("f%d", i);
_temp1188;}), 0), e, 0), 0), _temp1168, 0); goto _LL1173; _LL1173:;}}} return
Cyc_Toc_make_struct( nv, _temp1164, Cyc_Absyn_strct( _temp1162), _temp1168,
pointer, rgnopt, is_atomic);}}}}}}} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct(
struct Cyc_Toc_Env* nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* dles, struct _tuple0* tdn){ struct _tuple0*
_temp1190= Cyc_Toc_temp_var(); goto _LL1191; _LL1191: { struct Cyc_Absyn_Exp*
_temp1192= Cyc_Absyn_var_exp( _temp1190, 0); goto _LL1193; _LL1193: { struct Cyc_Absyn_Stmt*
_temp1194= Cyc_Absyn_exp_stmt( _temp1192, 0); goto _LL1195; _LL1195: { struct
Cyc_Absyn_Exp*(* _temp1196)( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
goto _LL1197; _LL1197: { int is_atomic= 1;{ struct Cyc_List_List* _temp1198=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto
_LL1199; _LL1199: for( 0; _temp1198 != 0; _temp1198=(( struct Cyc_List_List*)
_check_null( _temp1198))->tl){ struct _tuple4 _temp1202; struct Cyc_Absyn_Exp*
_temp1203; struct Cyc_List_List* _temp1205; struct _tuple4* _temp1200=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp1198))->hd; _temp1202=*
_temp1200; _LL1206: _temp1205= _temp1202.f1; goto _LL1204; _LL1204: _temp1203=
_temp1202.f2; goto _LL1201; _LL1201: is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1203->topt))->v): 0; if(
_temp1205 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1207=( char*)"empty designator list"; struct
_tagged_string _temp1208; _temp1208.curr= _temp1207; _temp1208.base= _temp1207;
_temp1208.last_plus_one= _temp1207 + 22; _temp1208;}));} if((( struct Cyc_List_List*)
_check_null( _temp1205))->tl != 0){ struct _tuple0* _temp1209= Cyc_Toc_temp_var();
goto _LL1210; _LL1210: { struct Cyc_Absyn_Exp* _temp1211= Cyc_Absyn_var_exp(
_temp1209, 0); goto _LL1212; _LL1212: for( 0; _temp1205 != 0; _temp1205=((
struct Cyc_List_List*) _check_null( _temp1205))->tl){ void* _temp1213=( void*)((
struct Cyc_List_List*) _check_null( _temp1205))->hd; struct _tagged_string*
_temp1219; _LL1215: if(*(( int*) _temp1213) == Cyc_Absyn_FieldName){ _LL1220:
_temp1219=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1213)->f1; goto _LL1216;} else{ goto _LL1217;} _LL1217: goto _LL1218;
_LL1216: if( Cyc_Toc_is_poly_field( struct_type, _temp1219)){ _temp1211= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1211, 0);} _temp1194= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp1196( _temp1192, _temp1219, 0), _temp1211, 0), 0),
_temp1194, 0); goto _LL1214; _LL1218:( void) _throw((( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1221=(
char*)"array designator in struct"; struct _tagged_string _temp1222; _temp1222.curr=
_temp1221; _temp1222.base= _temp1221; _temp1222.last_plus_one= _temp1221 + 27;
_temp1222;}))); _LL1214:;} Cyc_Toc_exp_to_c( nv, _temp1203); _temp1194= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp1211, _temp1203, 0), 0),
_temp1194, 0);}} else{ void* _temp1223=( void*)(( struct Cyc_List_List*)
_check_null( _temp1205))->hd; struct _tagged_string* _temp1229; _LL1225: if(*((
int*) _temp1223) == Cyc_Absyn_FieldName){ _LL1230: _temp1229=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1223)->f1; goto
_LL1226;} else{ goto _LL1227;} _LL1227: goto _LL1228; _LL1226: { struct Cyc_Absyn_Exp*
lval= _temp1196( _temp1192, _temp1229, 0);{ void* _temp1231=( void*) _temp1203->r;
struct Cyc_List_List* _temp1241; struct Cyc_Absyn_Exp* _temp1243; struct Cyc_Absyn_Exp*
_temp1245; struct Cyc_Absyn_Vardecl* _temp1247; struct Cyc_List_List* _temp1249;
void* _temp1251; _LL1233: if(*(( int*) _temp1231) == Cyc_Absyn_Array_e){ _LL1242:
_temp1241=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1231)->f1;
goto _LL1234;} else{ goto _LL1235;} _LL1235: if(*(( int*) _temp1231) == Cyc_Absyn_Comprehension_e){
_LL1248: _temp1247=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1231)->f1; goto _LL1246; _LL1246: _temp1245=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1231)->f2; goto _LL1244; _LL1244:
_temp1243=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1231)->f3; goto _LL1236;} else{ goto _LL1237;} _LL1237: if(*(( int*)
_temp1231) == Cyc_Absyn_AnonStruct_e){ _LL1252: _temp1251=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1231)->f1; goto _LL1250; _LL1250: _temp1249=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1231)->f2; goto _LL1238;} else{ goto
_LL1239;} _LL1239: goto _LL1240; _LL1234: _temp1194= Cyc_Toc_init_array( nv,
lval, _temp1241, _temp1194); goto _LL1232; _LL1236: _temp1194= Cyc_Toc_init_comprehension(
nv, lval, _temp1247, _temp1245, _temp1243, _temp1194, 0); goto _LL1232; _LL1238:
_temp1194= Cyc_Toc_init_anon_struct( nv, lval, _temp1251, _temp1249, _temp1194);
goto _LL1232; _LL1240: Cyc_Toc_exp_to_c( nv, _temp1203); if( Cyc_Toc_is_poly_field(
struct_type, _temp1229)){ _temp1203= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1203, 0);} _temp1194= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1203, 0), 0), _temp1194, 0); goto _LL1232; _LL1232:;} goto _LL1224;}
_LL1228:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1253=( char*)"array designator in struct";
struct _tagged_string _temp1254; _temp1254.curr= _temp1253; _temp1254.base=
_temp1253; _temp1254.last_plus_one= _temp1253 + 27; _temp1254;}))); _LL1224:;}}}
return Cyc_Toc_make_struct( nv, _temp1190, Cyc_Absyn_strctq( tdn), _temp1194,
pointer, rgnopt, is_atomic);}}}}} struct _tuple11{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp(
el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp1255=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp1255[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp1256; _temp1256.tag=
Cyc_Absyn_Increment_e; _temp1256.f1= e1; _temp1256.f2=( void*) incr; _temp1256;});
_temp1255;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1257=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp1267; struct Cyc_Absyn_Exp* _temp1269; void* _temp1271; struct
_tagged_string* _temp1273; struct Cyc_Absyn_Exp* _temp1275; _LL1259: if(*(( int*)
_temp1257) == Cyc_Absyn_StmtExp_e){ _LL1268: _temp1267=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1257)->f1; goto _LL1260;} else{ goto
_LL1261;} _LL1261: if(*(( int*) _temp1257) == Cyc_Absyn_Cast_e){ _LL1272:
_temp1271=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1257)->f1; goto
_LL1270; _LL1270: _temp1269=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1257)->f2; goto _LL1262;} else{ goto _LL1263;} _LL1263: if(*(( int*)
_temp1257) == Cyc_Absyn_StructMember_e){ _LL1276: _temp1275=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1257)->f1; goto _LL1274; _LL1274:
_temp1273=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1257)->f2; goto _LL1264;} else{ goto _LL1265;} _LL1265: goto _LL1266;
_LL1260:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1267, fs, f, f_env); goto _LL1258; _LL1262:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1269, fs, f, f_env); goto
_LL1258; _LL1264:( void*)( e1->r=( void*)(( void*) _temp1275->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1277=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1277->hd=( void*) _temp1273; _temp1277->tl= fs;
_temp1277;}), f, f_env); goto _LL1258; _LL1266: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1278= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1278.last_plus_one - _temp1278.curr, _temp1278.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1258;}
_LL1258:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1279=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1289; struct Cyc_Absyn_Stmt* _temp1291; struct Cyc_Absyn_Decl* _temp1293;
struct Cyc_Absyn_Stmt* _temp1295; struct Cyc_Absyn_Stmt* _temp1297; _LL1281: if((
unsigned int) _temp1279 > 1u?*(( int*) _temp1279) == Cyc_Absyn_Exp_s: 0){
_LL1290: _temp1289=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1279)->f1; goto _LL1282;} else{ goto _LL1283;} _LL1283: if(( unsigned int)
_temp1279 > 1u?*(( int*) _temp1279) == Cyc_Absyn_Decl_s: 0){ _LL1294: _temp1293=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1279)->f1; goto
_LL1292; _LL1292: _temp1291=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1279)->f2; goto _LL1284;} else{ goto _LL1285;} _LL1285: if(( unsigned int)
_temp1279 > 1u?*(( int*) _temp1279) == Cyc_Absyn_Seq_s: 0){ _LL1298: _temp1297=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1279)->f1; goto
_LL1296; _LL1296: _temp1295=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1279)->f2; goto _LL1286;} else{ goto _LL1287;} _LL1287: goto _LL1288;
_LL1282:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1289, fs, f, f_env); goto _LL1280; _LL1284:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1291, fs, f, f_env);
goto _LL1280; _LL1286:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1295, fs, f, f_env); goto _LL1280; _LL1288:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1299= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1299.last_plus_one - _temp1299.curr,
_temp1299.curr);})); goto _LL1280; _LL1280:;} static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s); struct _tuple12{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp1300=( void*) e->r; goto _LL1301;
_LL1301: if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1302= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp1302.last_plus_one - _temp1302.curr, _temp1302.curr);})); return;}{ void*
old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void*
_temp1303= _temp1300; void* _temp1379; void* _temp1381; void* _temp1383; struct
_tuple0* _temp1385; struct _tuple0* _temp1387; struct Cyc_List_List* _temp1389;
void* _temp1391; void* _temp1393; struct Cyc_Absyn_Exp* _temp1395; struct Cyc_Absyn_Exp*
_temp1397; struct Cyc_Core_Opt* _temp1399; struct Cyc_Absyn_Exp* _temp1401;
struct Cyc_Absyn_Exp* _temp1403; struct Cyc_Absyn_Exp* _temp1405; struct Cyc_Absyn_Exp*
_temp1407; struct Cyc_Absyn_Exp* _temp1409; struct Cyc_Absyn_Exp* _temp1411;
struct Cyc_List_List* _temp1413; struct Cyc_Absyn_Exp* _temp1415; struct Cyc_List_List*
_temp1417; struct Cyc_Absyn_Exp* _temp1419; struct Cyc_Absyn_Exp* _temp1421;
struct Cyc_Absyn_Exp* _temp1423; struct Cyc_List_List* _temp1425; struct Cyc_Absyn_Exp*
_temp1427; struct Cyc_Absyn_Exp* _temp1429; struct Cyc_Absyn_Exp** _temp1431;
void* _temp1432; void** _temp1434; struct Cyc_Absyn_Exp* _temp1435; struct Cyc_Absyn_Exp*
_temp1437; struct Cyc_Absyn_Exp* _temp1439; void* _temp1441; struct Cyc_Absyn_Exp*
_temp1443; struct Cyc_Absyn_Exp* _temp1445; struct _tagged_string* _temp1447;
struct Cyc_Absyn_Exp* _temp1449; struct _tagged_string* _temp1451; struct Cyc_Absyn_Exp*
_temp1453; struct Cyc_Absyn_Exp* _temp1455; struct Cyc_Absyn_Exp* _temp1457;
struct Cyc_List_List* _temp1459; struct Cyc_List_List* _temp1461; struct _tuple1*
_temp1463; struct Cyc_List_List* _temp1465; struct Cyc_Absyn_Exp* _temp1467;
struct Cyc_Absyn_Exp* _temp1469; struct Cyc_Absyn_Vardecl* _temp1471; struct Cyc_Absyn_Structdecl*
_temp1473; struct Cyc_List_List* _temp1475; struct Cyc_Core_Opt* _temp1477;
struct _tuple0* _temp1479; struct Cyc_List_List* _temp1481; void* _temp1483;
struct Cyc_Absyn_Tunionfield* _temp1485; struct Cyc_Absyn_Tuniondecl* _temp1487;
struct Cyc_List_List* _temp1489; struct Cyc_Core_Opt* _temp1491; struct Cyc_Core_Opt*
_temp1493; struct Cyc_Absyn_Tunionfield* _temp1495; struct Cyc_Absyn_Tuniondecl*
_temp1497; struct Cyc_List_List* _temp1499; struct Cyc_Core_Opt* _temp1501;
struct Cyc_Core_Opt* _temp1503; struct Cyc_Absyn_Enumfield* _temp1505; struct
Cyc_Absyn_Enumdecl* _temp1507; struct _tuple0* _temp1509; void* _temp1511;
struct Cyc_Absyn_Exp* _temp1513; struct Cyc_Absyn_Stmt* _temp1515; struct Cyc_List_List*
_temp1517; struct Cyc_Core_Opt* _temp1519; struct Cyc_Absyn_Fndecl* _temp1521;
struct Cyc_Absyn_Exp* _temp1523; _LL1305: if(*(( int*) _temp1303) == Cyc_Absyn_Const_e){
_LL1380: _temp1379=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1303)->f1;
if( _temp1379 ==( void*) Cyc_Absyn_Null_c){ goto _LL1306;} else{ goto _LL1307;}}
else{ goto _LL1307;} _LL1307: if(*(( int*) _temp1303) == Cyc_Absyn_Const_e){
_LL1382: _temp1381=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1303)->f1;
goto _LL1308;} else{ goto _LL1309;} _LL1309: if(*(( int*) _temp1303) == Cyc_Absyn_Var_e){
_LL1386: _temp1385=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1303)->f1; goto _LL1384; _LL1384: _temp1383=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1303)->f2; goto _LL1310;} else{ goto _LL1311;} _LL1311: if(*(( int*)
_temp1303) == Cyc_Absyn_UnknownId_e){ _LL1388: _temp1387=( struct _tuple0*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1303)->f1; goto _LL1312;} else{ goto
_LL1313;} _LL1313: if(*(( int*) _temp1303) == Cyc_Absyn_Primop_e){ _LL1392:
_temp1391=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1303)->f1; goto
_LL1390; _LL1390: _temp1389=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1303)->f2; goto _LL1314;} else{ goto _LL1315;} _LL1315: if(*(( int*)
_temp1303) == Cyc_Absyn_Increment_e){ _LL1396: _temp1395=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp1303)->f1; goto _LL1394; _LL1394:
_temp1393=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1303)->f2; goto
_LL1316;} else{ goto _LL1317;} _LL1317: if(*(( int*) _temp1303) == Cyc_Absyn_AssignOp_e){
_LL1402: _temp1401=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1303)->f1; goto _LL1400; _LL1400: _temp1399=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1303)->f2; goto _LL1398; _LL1398:
_temp1397=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1303)->f3; goto _LL1318;} else{ goto _LL1319;} _LL1319: if(*(( int*)
_temp1303) == Cyc_Absyn_Conditional_e){ _LL1408: _temp1407=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1303)->f1; goto _LL1406; _LL1406:
_temp1405=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1303)->f2; goto _LL1404; _LL1404: _temp1403=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1303)->f3; goto _LL1320;} else{
goto _LL1321;} _LL1321: if(*(( int*) _temp1303) == Cyc_Absyn_SeqExp_e){ _LL1412:
_temp1411=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1303)->f1; goto _LL1410; _LL1410: _temp1409=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1303)->f2; goto _LL1322;} else{ goto
_LL1323;} _LL1323: if(*(( int*) _temp1303) == Cyc_Absyn_UnknownCall_e){ _LL1416:
_temp1415=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1303)->f1; goto _LL1414; _LL1414: _temp1413=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1303)->f2; goto _LL1324;} else{
goto _LL1325;} _LL1325: if(*(( int*) _temp1303) == Cyc_Absyn_FnCall_e){ _LL1420:
_temp1419=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1303)->f1; goto _LL1418; _LL1418: _temp1417=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1303)->f2; goto _LL1326;} else{ goto
_LL1327;} _LL1327: if(*(( int*) _temp1303) == Cyc_Absyn_Throw_e){ _LL1422:
_temp1421=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1303)->f1;
goto _LL1328;} else{ goto _LL1329;} _LL1329: if(*(( int*) _temp1303) == Cyc_Absyn_NoInstantiate_e){
_LL1424: _temp1423=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1303)->f1; goto _LL1330;} else{ goto _LL1331;} _LL1331: if(*(( int*)
_temp1303) == Cyc_Absyn_Instantiate_e){ _LL1428: _temp1427=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1303)->f1; goto _LL1426; _LL1426:
_temp1425=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1303)->f2; goto _LL1332;} else{ goto _LL1333;} _LL1333: if(*(( int*)
_temp1303) == Cyc_Absyn_Cast_e){ _LL1433: _temp1432=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1303)->f1; _temp1434=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1303)->f1;
goto _LL1430; _LL1430: _temp1429=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1303)->f2; _temp1431=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1303)->f2;
goto _LL1334;} else{ goto _LL1335;} _LL1335: if(*(( int*) _temp1303) == Cyc_Absyn_Address_e){
_LL1436: _temp1435=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp1303)->f1; goto _LL1336;} else{ goto _LL1337;} _LL1337: if(*(( int*)
_temp1303) == Cyc_Absyn_New_e){ _LL1440: _temp1439=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1303)->f1; goto _LL1438; _LL1438: _temp1437=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1303)->f2; goto
_LL1338;} else{ goto _LL1339;} _LL1339: if(*(( int*) _temp1303) == Cyc_Absyn_Sizeoftyp_e){
_LL1442: _temp1441=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1303)->f1;
goto _LL1340;} else{ goto _LL1341;} _LL1341: if(*(( int*) _temp1303) == Cyc_Absyn_Sizeofexp_e){
_LL1444: _temp1443=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1303)->f1; goto _LL1342;} else{ goto _LL1343;} _LL1343: if(*(( int*)
_temp1303) == Cyc_Absyn_Deref_e){ _LL1446: _temp1445=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp1303)->f1; goto _LL1344;} else{ goto
_LL1345;} _LL1345: if(*(( int*) _temp1303) == Cyc_Absyn_StructMember_e){ _LL1450:
_temp1449=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1303)->f1; goto _LL1448; _LL1448: _temp1447=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1303)->f2; goto _LL1346;} else{
goto _LL1347;} _LL1347: if(*(( int*) _temp1303) == Cyc_Absyn_StructArrow_e){
_LL1454: _temp1453=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1303)->f1; goto _LL1452; _LL1452: _temp1451=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1303)->f2; goto _LL1348;} else{
goto _LL1349;} _LL1349: if(*(( int*) _temp1303) == Cyc_Absyn_Subscript_e){
_LL1458: _temp1457=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1303)->f1; goto _LL1456; _LL1456: _temp1455=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1303)->f2; goto _LL1350;} else{ goto
_LL1351;} _LL1351: if(*(( int*) _temp1303) == Cyc_Absyn_Tuple_e){ _LL1460:
_temp1459=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1303)->f1;
goto _LL1352;} else{ goto _LL1353;} _LL1353: if(*(( int*) _temp1303) == Cyc_Absyn_CompoundLit_e){
_LL1464: _temp1463=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1303)->f1; goto _LL1462; _LL1462: _temp1461=( struct Cyc_List_List*)((
struct Cyc_Absyn_CompoundLit_e_struct*) _temp1303)->f2; goto _LL1354;} else{
goto _LL1355;} _LL1355: if(*(( int*) _temp1303) == Cyc_Absyn_Array_e){ _LL1466:
_temp1465=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1303)->f1;
goto _LL1356;} else{ goto _LL1357;} _LL1357: if(*(( int*) _temp1303) == Cyc_Absyn_Comprehension_e){
_LL1472: _temp1471=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1303)->f1; goto _LL1470; _LL1470: _temp1469=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1303)->f2; goto _LL1468; _LL1468:
_temp1467=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1303)->f3; goto _LL1358;} else{ goto _LL1359;} _LL1359: if(*(( int*)
_temp1303) == Cyc_Absyn_Struct_e){ _LL1480: _temp1479=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1303)->f1; goto _LL1478; _LL1478:
_temp1477=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1303)->f2;
goto _LL1476; _LL1476: _temp1475=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1303)->f3; goto _LL1474; _LL1474: _temp1473=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1303)->f4; goto _LL1360;} else{ goto
_LL1361;} _LL1361: if(*(( int*) _temp1303) == Cyc_Absyn_AnonStruct_e){ _LL1484:
_temp1483=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1303)->f1; goto
_LL1482; _LL1482: _temp1481=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1303)->f2; goto _LL1362;} else{ goto _LL1363;} _LL1363: if(*(( int*)
_temp1303) == Cyc_Absyn_Tunion_e){ _LL1494: _temp1493=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1303)->f1; goto _LL1492; _LL1492:
_temp1491=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1303)->f2;
goto _LL1490; _LL1490: _temp1489=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1303)->f3; if( _temp1489 == 0){ goto _LL1488;} else{ goto _LL1365;} _LL1488:
_temp1487=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1303)->f4; goto _LL1486; _LL1486: _temp1485=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1303)->f5; goto _LL1364;} else{ goto
_LL1365;} _LL1365: if(*(( int*) _temp1303) == Cyc_Absyn_Tunion_e){ _LL1504:
_temp1503=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1303)->f1;
goto _LL1502; _LL1502: _temp1501=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1303)->f2; goto _LL1500; _LL1500: _temp1499=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1303)->f3; goto _LL1498; _LL1498:
_temp1497=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1303)->f4; goto _LL1496; _LL1496: _temp1495=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1303)->f5; goto _LL1366;} else{ goto
_LL1367;} _LL1367: if(*(( int*) _temp1303) == Cyc_Absyn_Enum_e){ _LL1510:
_temp1509=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1303)->f1;
goto _LL1508; _LL1508: _temp1507=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1303)->f2; goto _LL1506; _LL1506: _temp1505=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1303)->f3; goto _LL1368;} else{ goto
_LL1369;} _LL1369: if(*(( int*) _temp1303) == Cyc_Absyn_Malloc_e){ _LL1514:
_temp1513=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1303)->f1; goto _LL1512; _LL1512: _temp1511=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1303)->f2; goto _LL1370;} else{ goto _LL1371;} _LL1371: if(*(( int*)
_temp1303) == Cyc_Absyn_StmtExp_e){ _LL1516: _temp1515=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1303)->f1; goto _LL1372;} else{ goto
_LL1373;} _LL1373: if(*(( int*) _temp1303) == Cyc_Absyn_UnresolvedMem_e){
_LL1520: _temp1519=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1303)->f1; goto _LL1518; _LL1518: _temp1517=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1303)->f2; goto _LL1374;} else{
goto _LL1375;} _LL1375: if(*(( int*) _temp1303) == Cyc_Absyn_Codegen_e){ _LL1522:
_temp1521=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1303)->f1; goto _LL1376;} else{ goto _LL1377;} _LL1377: if(*(( int*)
_temp1303) == Cyc_Absyn_Fill_e){ _LL1524: _temp1523=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp1303)->f1; goto _LL1378;} else{ goto
_LL1304;} _LL1306:{ void* _temp1525= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1531; struct Cyc_Absyn_Conref* _temp1533; struct Cyc_Absyn_Tqual _temp1535;
struct Cyc_Absyn_Conref* _temp1537; void* _temp1539; void* _temp1541; _LL1527:
if(( unsigned int) _temp1525 > 4u?*(( int*) _temp1525) == Cyc_Absyn_PointerType:
0){ _LL1532: _temp1531=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1525)->f1; _LL1542: _temp1541=( void*) _temp1531.elt_typ; goto _LL1540;
_LL1540: _temp1539=( void*) _temp1531.rgn_typ; goto _LL1538; _LL1538: _temp1537=(
struct Cyc_Absyn_Conref*) _temp1531.nullable; goto _LL1536; _LL1536: _temp1535=(
struct Cyc_Absyn_Tqual) _temp1531.tq; goto _LL1534; _LL1534: _temp1533=( struct
Cyc_Absyn_Conref*) _temp1531.bounds; goto _LL1528;} else{ goto _LL1529;} _LL1529:
goto _LL1530; _LL1528: if(( void*) _temp1533->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1526;}{ void* _temp1543=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1533); struct Cyc_Absyn_Exp* _temp1549; _LL1545: if( _temp1543 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1546;} else{ goto _LL1547;} _LL1547: if((
unsigned int) _temp1543 > 1u?*(( int*) _temp1543) == Cyc_Absyn_Upper_b: 0){
_LL1550: _temp1549=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1543)->f1; goto _LL1548;} else{ goto _LL1544;} _LL1546: { struct _tuple4*
_temp1552=({ struct _tuple4* _temp1551=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1551->f1= 0; _temp1551->f2= Cyc_Absyn_signed_int_exp( 0,
0); _temp1551;}); goto _LL1553; _LL1553: { struct Cyc_Absyn_Exp* _temp1557= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp1554=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1554->hd=( void*) _temp1552; _temp1554->tl=({
struct Cyc_List_List* _temp1555=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1555->hd=( void*) _temp1552; _temp1555->tl=({
struct Cyc_List_List* _temp1556=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1556->hd=( void*) _temp1552; _temp1556->tl= 0;
_temp1556;}); _temp1555;}); _temp1554;}), 0); goto _LL1558; _LL1558: if( nv->toplevel){(
void*)( e->r=( void*)(( void*) _temp1557->r));} else{ void* _temp1559= Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1541), Cyc_Toc_mt_tq); goto _LL1560; _LL1560: { struct
_tuple0* _temp1561= Cyc_Toc_temp_var(); goto _LL1562; _LL1562: { struct Cyc_Absyn_Exp*
_temp1563= Cyc_Absyn_var_exp( _temp1561, 0); goto _LL1564; _LL1564: { struct Cyc_Absyn_Stmt*
_temp1565= Cyc_Absyn_declare_stmt( _temp1561, _temp1559,( struct Cyc_Absyn_Exp*)
_temp1557, Cyc_Absyn_exp_stmt( _temp1563, 0), 0); goto _LL1566; _LL1566:( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1565, 0))->r));}}}} goto
_LL1544;}} _LL1548:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0,
0))->r)); goto _LL1544; _LL1544:;} goto _LL1526; _LL1530:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1567= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1567.last_plus_one -
_temp1567.curr, _temp1567.curr);})); goto _LL1526; _LL1526:;} goto _LL1304;
_LL1308: goto _LL1304; _LL1310:{ struct _handler_cons _temp1568; _push_handler(&
_temp1568);{ int _temp1570= 0; if( setjmp( _temp1568.handler)){ _temp1570= 1;}
if( ! _temp1570){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1385))->r));; _pop_handler();} else{ void* _temp1569=( void*) _exn_thrown;
void* _temp1572= _temp1569; _LL1574: if( _temp1572 == Cyc_Dict_Absent){ goto
_LL1575;} else{ goto _LL1576;} _LL1576: goto _LL1577; _LL1575:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1578= Cyc_Absynpp_qvar2string(
_temp1385); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1578.last_plus_one
- _temp1578.curr, _temp1578.curr);})); return; _LL1577:( void) _throw( _temp1572);
_LL1573:;}}} goto _LL1304; _LL1312:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1579=( char*)"unknownid"; struct
_tagged_string _temp1580; _temp1580.curr= _temp1579; _temp1580.base= _temp1579;
_temp1580.last_plus_one= _temp1579 + 10; _temp1580;})); goto _LL1304; _LL1314: {
struct Cyc_List_List* _temp1581=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1389); goto _LL1582; _LL1582: { struct Cyc_List_List* _temp1583=(( struct
Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Toc_get_c_typ, _temp1389); goto _LL1584; _LL1584:(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1389);{
void* _temp1585= _temp1391; _LL1587: if( _temp1585 ==( void*) Cyc_Absyn_Size){
goto _LL1588;} else{ goto _LL1589;} _LL1589: if( _temp1585 ==( void*) Cyc_Absyn_Printf){
goto _LL1590;} else{ goto _LL1591;} _LL1591: if( _temp1585 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1592;} else{ goto _LL1593;} _LL1593: if( _temp1585 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1594;} else{ goto _LL1595;} _LL1595: if( _temp1585 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1596;} else{ goto _LL1597;} _LL1597: if( _temp1585 ==( void*) Cyc_Absyn_Plus){
goto _LL1598;} else{ goto _LL1599;} _LL1599: if( _temp1585 ==( void*) Cyc_Absyn_Minus){
goto _LL1600;} else{ goto _LL1601;} _LL1601: if( _temp1585 ==( void*) Cyc_Absyn_Eq){
goto _LL1602;} else{ goto _LL1603;} _LL1603: if( _temp1585 ==( void*) Cyc_Absyn_Neq){
goto _LL1604;} else{ goto _LL1605;} _LL1605: if( _temp1585 ==( void*) Cyc_Absyn_Gt){
goto _LL1606;} else{ goto _LL1607;} _LL1607: if( _temp1585 ==( void*) Cyc_Absyn_Gte){
goto _LL1608;} else{ goto _LL1609;} _LL1609: if( _temp1585 ==( void*) Cyc_Absyn_Lt){
goto _LL1610;} else{ goto _LL1611;} _LL1611: if( _temp1585 ==( void*) Cyc_Absyn_Lte){
goto _LL1612;} else{ goto _LL1613;} _LL1613: goto _LL1614; _LL1588: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1389))->hd;{
void* _temp1615=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1623; struct Cyc_Absyn_Tqual _temp1625; void*
_temp1627; struct Cyc_Absyn_PtrInfo _temp1629; struct Cyc_Absyn_Conref*
_temp1631; struct Cyc_Absyn_Tqual _temp1633; struct Cyc_Absyn_Conref* _temp1635;
void* _temp1637; void* _temp1639; _LL1617: if(( unsigned int) _temp1615 > 4u?*((
int*) _temp1615) == Cyc_Absyn_ArrayType: 0){ _LL1628: _temp1627=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1615)->f1; goto _LL1626; _LL1626:
_temp1625=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1615)->f2; goto _LL1624; _LL1624: _temp1623=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1615)->f3; goto _LL1618;} else{ goto
_LL1619;} _LL1619: if(( unsigned int) _temp1615 > 4u?*(( int*) _temp1615) == Cyc_Absyn_PointerType:
0){ _LL1630: _temp1629=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1615)->f1; _LL1640: _temp1639=( void*) _temp1629.elt_typ; goto _LL1638;
_LL1638: _temp1637=( void*) _temp1629.rgn_typ; goto _LL1636; _LL1636: _temp1635=(
struct Cyc_Absyn_Conref*) _temp1629.nullable; goto _LL1634; _LL1634: _temp1633=(
struct Cyc_Absyn_Tqual) _temp1629.tq; goto _LL1632; _LL1632: _temp1631=( struct
Cyc_Absyn_Conref*) _temp1629.bounds; goto _LL1620;} else{ goto _LL1621;} _LL1621:
goto _LL1622; _LL1618:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1623))->r)); goto _LL1616; _LL1620: { struct _tuple0*
_temp1641= Cyc_Toc_temp_var(); goto _LL1642; _LL1642: { void* _temp1643= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); goto _LL1644;
_LL1644: { struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1641, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1641, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4=
Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp( e2, e3, 0), 0);
struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1641, _temp1643,(
struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1616;}}} _LL1622:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1645= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*) _check_null(
_temp1583))->hd); struct _tagged_string _temp1646= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_temp1645.last_plus_one - _temp1645.curr, _temp1645.curr, _temp1646.last_plus_one
- _temp1646.curr, _temp1646.curr);})); return; _LL1616:;} goto _LL1586;} _LL1590:(
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1391, 0, _temp1389,
_temp1583))->r)); goto _LL1586; _LL1592:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1391, 0, _temp1389, _temp1583))->r)); goto _LL1586; _LL1594: if( _temp1389
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1647=( char*)"fprintf without arguments"; struct
_tagged_string _temp1648; _temp1648.curr= _temp1647; _temp1648.base= _temp1647;
_temp1648.last_plus_one= _temp1647 + 26; _temp1648;}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_printf_to_c( _temp1391,({ struct Cyc_Core_Opt* _temp1649=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1649->v=(
void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1389))->hd);
_temp1649;}),(( struct Cyc_List_List*) _check_null( _temp1389))->tl,(( struct
Cyc_List_List*) _check_null( _temp1583))->tl))->r)); goto _LL1586; _LL1596: if(
_temp1389 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1650=( char*)"sscanf without arguments";
struct _tagged_string _temp1651; _temp1651.curr= _temp1650; _temp1651.base=
_temp1650; _temp1651.last_plus_one= _temp1650 + 25; _temp1651;}));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1389))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1389))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1389))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1586; _LL1598: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1389))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1389))->tl))->hd; struct _tuple0* _temp1652= Cyc_Toc_temp_var(); goto
_LL1653; _LL1653: { void* _temp1654= Cyc_Toc_typ_to_c( old_typ); goto _LL1655;
_LL1655: { struct Cyc_Absyn_Stmt* _temp1656= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1652, 0), 0); goto _LL1657; _LL1657: { struct Cyc_Absyn_Exp* _temp1658= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1652, 0), Cyc_Toc_curr_sp, 0); goto _LL1659; _LL1659: {
struct Cyc_Absyn_Exp* _temp1661= Cyc_Absyn_assignop_exp( _temp1658,({ struct Cyc_Core_Opt*
_temp1660=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1660->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1660;}), e2, 0); goto
_LL1662; _LL1662: _temp1656= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1661,
0), _temp1656, 0); _temp1656= Cyc_Absyn_declare_stmt( _temp1652, _temp1654,(
struct Cyc_Absyn_Exp*) e1, _temp1656, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1656, 0))->r));}}}}} goto _LL1586; _LL1600: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null( _temp1581))->hd)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1389))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1389))->tl))->hd; void*
old_typ_e2=( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1581))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* _temp1663= Cyc_Toc_temp_var(); goto _LL1664;
_LL1664: { void* _temp1665= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_List_List*)
_check_null( _temp1581))->hd); goto _LL1666; _LL1666: { struct Cyc_Absyn_Stmt*
_temp1667= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1663, 0), 0); goto
_LL1668; _LL1668: { struct Cyc_Absyn_Exp* _temp1669= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1663, 0), Cyc_Toc_curr_sp, 0); goto _LL1670; _LL1670: {
struct Cyc_Absyn_Exp* _temp1672= Cyc_Absyn_assignop_exp( _temp1669,({ struct Cyc_Core_Opt*
_temp1671=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1671->v=( void*)(( void*) Cyc_Absyn_Minus); _temp1671;}), e2, 0); goto
_LL1673; _LL1673: _temp1667= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1672,
0), _temp1667, 0); _temp1667= Cyc_Absyn_declare_stmt( _temp1663, _temp1665,(
struct Cyc_Absyn_Exp*) e1, _temp1667, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1667, 0))->r));}}}}}} goto _LL1586; _LL1602: goto _LL1604; _LL1604: goto
_LL1606; _LL1606: goto _LL1608; _LL1608: goto _LL1610; _LL1610: goto _LL1612;
_LL1612: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1389))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1389))->tl))->hd; void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp1581))->hd; void* t2=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1581))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1586;} _LL1614: goto
_LL1586; _LL1586:;} goto _LL1304;}} _LL1316: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1395->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1395); if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if( _temp1393 ==(
void*) Cyc_Absyn_PreInc? 1: _temp1393 ==( void*) Cyc_Absyn_PreDec){ struct
_tuple0* _temp1674= Cyc_Toc_temp_var(); goto _LL1675; _LL1675: { void* _temp1676=
Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1677; _LL1677: { struct Cyc_Absyn_Stmt*
_temp1678= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp1674,
0), 0), 0); goto _LL1679; _LL1679: { struct Cyc_Absyn_Exp* _temp1680= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1674, 0), Cyc_Toc_curr_sp, 0),
_temp1393, 0); goto _LL1681; _LL1681: _temp1678= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1680, 0), _temp1678, 0); _temp1678= Cyc_Absyn_declare_stmt( _temp1674,
_temp1676,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1395, 0),
_temp1678, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1678, 0))->r));}}}}
else{ struct _tuple0* _temp1682= Cyc_Toc_temp_var(); goto _LL1683; _LL1683: {
struct _tuple0* _temp1684= Cyc_Toc_temp_var(); goto _LL1685; _LL1685: { void*
_temp1686= Cyc_Toc_typ_to_c( e2_cyc_typ); goto _LL1687; _LL1687: { void*
_temp1688= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1689; _LL1689: { struct Cyc_Absyn_Stmt*
_temp1690= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1684, 0), 0); goto
_LL1691; _LL1691: { struct Cyc_Absyn_Exp* _temp1692= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1682, 0), Cyc_Toc_curr_sp, 0),
_temp1393, 0); goto _LL1693; _LL1693: _temp1690= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1692, 0), _temp1690, 0); _temp1690= Cyc_Absyn_declare_stmt( _temp1684,
_temp1686,( struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp1682, 0), 0), _temp1690, 0); _temp1690= Cyc_Absyn_declare_stmt( _temp1682,
_temp1688,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1395, 0),
_temp1690, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1690, 0))->r));}}}}}}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1395)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1395, 0, Cyc_Toc_incr_lvalue,
_temp1393);( void*)( e->r=( void*)(( void*) _temp1395->r));}} goto _LL1304;}
_LL1318: { int e1_poly= Cyc_Toc_is_poly_project( _temp1401); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1401->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1397->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1401); Cyc_Toc_exp_to_c( nv, _temp1397); if(
_temp1399 != 0?( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1399))->v ==(
void*) Cyc_Absyn_Plus? 1:( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1399))->v
==( void*) Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){(
void*)( _temp1401->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1401->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1397->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1397->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1397->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1397->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1401)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1401, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1694=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1694[ 0]=({ struct _tuple11 _temp1695; _temp1695.f1=
_temp1399; _temp1695.f2= _temp1397; _temp1695;}); _temp1694;}));( void*)( e->r=(
void*)(( void*) _temp1401->r));} goto _LL1304;} _LL1320: Cyc_Toc_exp_to_c( nv,
_temp1407); Cyc_Toc_exp_to_c( nv, _temp1405); Cyc_Toc_exp_to_c( nv, _temp1403);
goto _LL1304; _LL1322: Cyc_Toc_exp_to_c( nv, _temp1411); Cyc_Toc_exp_to_c( nv,
_temp1409); goto _LL1304; _LL1324: _temp1419= _temp1415; _temp1417= _temp1413;
goto _LL1326; _LL1326: Cyc_Toc_exp_to_c( nv, _temp1419);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1417); goto _LL1304; _LL1328:
Cyc_Toc_exp_to_c( nv, _temp1421);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1421), 0))->r)); goto
_LL1304; _LL1330: Cyc_Toc_exp_to_c( nv, _temp1423); goto _LL1304; _LL1332: Cyc_Toc_exp_to_c(
nv, _temp1427);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); for( 0; _temp1425 != 0; _temp1425=(( struct Cyc_List_List*)
_check_null( _temp1425))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct
Cyc_List_List*) _check_null( _temp1425))->hd); if( k !=( void*) Cyc_Absyn_EffKind?
k !=( void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1427, 0))->r)); break;}} goto _LL1304;} _LL1334: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null((* _temp1431)->topt))->v; void*
new_typ=* _temp1434;* _temp1434= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv,* _temp1431);* _temp1431= Cyc_Toc_convert_exp( nv->toplevel, old_t2, new_typ,*
_temp1431); goto _LL1304;} _LL1336:{ void* _temp1696=( void*) _temp1435->r;
struct Cyc_Absyn_Structdecl* _temp1704; struct Cyc_List_List* _temp1706; struct
Cyc_Core_Opt* _temp1708; struct _tuple0* _temp1710; struct Cyc_List_List*
_temp1712; _LL1698: if(*(( int*) _temp1696) == Cyc_Absyn_Struct_e){ _LL1711:
_temp1710=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1696)->f1;
goto _LL1709; _LL1709: _temp1708=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1696)->f2; goto _LL1707; _LL1707: _temp1706=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1696)->f3; goto _LL1705; _LL1705:
_temp1704=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1696)->f4; goto _LL1699;} else{ goto _LL1700;} _LL1700: if(*(( int*)
_temp1696) == Cyc_Absyn_Tuple_e){ _LL1713: _temp1712=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_e_struct*) _temp1696)->f1; goto _LL1701;} else{ goto
_LL1702;} _LL1702: goto _LL1703; _LL1699: if( nv->toplevel){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1714= Cyc_Position_string_of_segment(
_temp1435->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1714.last_plus_one - _temp1714.curr, _temp1714.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1435->topt))->v, 1, 0, _temp1706, _temp1710))->r)); goto
_LL1697; _LL1701: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1715= Cyc_Position_string_of_segment( _temp1435->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1715.last_plus_one -
_temp1715.curr, _temp1715.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, 0, _temp1712))->r)); goto _LL1697; _LL1703: Cyc_Toc_exp_to_c( nv,
_temp1435); if( ! Cyc_Absyn_is_lvalue( _temp1435)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1435, 0, Cyc_Toc_address_lvalue,
1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v), _temp1435, 0))->r));} goto
_LL1697; _LL1697:;} goto _LL1304; _LL1338: if( nv->toplevel){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1716= Cyc_Position_string_of_segment(
_temp1437->loc); xprintf("%.*s: new at top-level", _temp1716.last_plus_one -
_temp1716.curr, _temp1716.curr);}));}{ void* _temp1717=( void*) _temp1437->r;
struct Cyc_List_List* _temp1731; void* _temp1733; struct _tagged_string
_temp1735; struct Cyc_Absyn_Exp* _temp1737; struct Cyc_Absyn_Exp* _temp1739;
struct Cyc_Absyn_Vardecl* _temp1741; struct Cyc_Absyn_Structdecl* _temp1743;
struct Cyc_List_List* _temp1745; struct Cyc_Core_Opt* _temp1747; struct _tuple0*
_temp1749; struct Cyc_List_List* _temp1751; _LL1719: if(*(( int*) _temp1717) ==
Cyc_Absyn_Array_e){ _LL1732: _temp1731=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1717)->f1; goto _LL1720;} else{ goto _LL1721;} _LL1721: if(*(( int*)
_temp1717) == Cyc_Absyn_Const_e){ _LL1734: _temp1733=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1717)->f1; if(( unsigned int) _temp1733 > 1u?*(( int*) _temp1733) == Cyc_Absyn_String_c:
0){ _LL1736: _temp1735=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1733)->f1; goto _LL1722;} else{ goto _LL1723;}} else{ goto _LL1723;}
_LL1723: if(*(( int*) _temp1717) == Cyc_Absyn_Comprehension_e){ _LL1742:
_temp1741=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1717)->f1; goto _LL1740; _LL1740: _temp1739=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1717)->f2; goto _LL1738; _LL1738:
_temp1737=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1717)->f3; goto _LL1724;} else{ goto _LL1725;} _LL1725: if(*(( int*)
_temp1717) == Cyc_Absyn_Struct_e){ _LL1750: _temp1749=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1717)->f1; goto _LL1748; _LL1748:
_temp1747=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1717)->f2;
goto _LL1746; _LL1746: _temp1745=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1717)->f3; goto _LL1744; _LL1744: _temp1743=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1717)->f4; goto _LL1726;} else{ goto
_LL1727;} _LL1727: if(*(( int*) _temp1717) == Cyc_Absyn_Tuple_e){ _LL1752:
_temp1751=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1717)->f1;
goto _LL1728;} else{ goto _LL1729;} _LL1729: goto _LL1730; _LL1720: { struct
_tuple0* _temp1753= Cyc_Toc_temp_var(); goto _LL1754; _LL1754: { struct Cyc_Absyn_Exp*
_temp1755= Cyc_Absyn_var_exp( _temp1753, 0); goto _LL1756; _LL1756: { struct Cyc_Absyn_Stmt*
_temp1757= Cyc_Toc_init_array( nv, _temp1755, _temp1731, Cyc_Absyn_exp_stmt(
_temp1755, 0)); goto _LL1758; _LL1758: { void* old_elt_typ;{ void* _temp1759=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1765; struct Cyc_Absyn_Conref*
_temp1767; struct Cyc_Absyn_Tqual _temp1769; struct Cyc_Absyn_Conref* _temp1771;
void* _temp1773; void* _temp1775; _LL1761: if(( unsigned int) _temp1759 > 4u?*((
int*) _temp1759) == Cyc_Absyn_PointerType: 0){ _LL1766: _temp1765=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1759)->f1; _LL1776: _temp1775=( void*)
_temp1765.elt_typ; goto _LL1774; _LL1774: _temp1773=( void*) _temp1765.rgn_typ;
goto _LL1772; _LL1772: _temp1771=( struct Cyc_Absyn_Conref*) _temp1765.nullable;
goto _LL1770; _LL1770: _temp1769=( struct Cyc_Absyn_Tqual) _temp1765.tq; goto
_LL1768; _LL1768: _temp1767=( struct Cyc_Absyn_Conref*) _temp1765.bounds; goto
_LL1762;} else{ goto _LL1763;} _LL1763: goto _LL1764; _LL1762: old_elt_typ=
_temp1775; goto _LL1760; _LL1764: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1777=( char*)"exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1778; _temp1778.curr= _temp1777; _temp1778.base=
_temp1777; _temp1778.last_plus_one= _temp1777 + 52; _temp1778;})); goto _LL1760;
_LL1760:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* _temp1779= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1780; _LL1780: { struct Cyc_Absyn_Exp*
_temp1781= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1731), 0), 0); goto
_LL1782; _LL1782: { struct Cyc_Absyn_Exp* e1; if( _temp1439 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1781);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*) _check_null( _temp1439)); Cyc_Toc_exp_to_c( nv, r); e1=
Cyc_Toc_rmalloc_exp( r, _temp1781);}{ struct Cyc_Absyn_Exp* _temp1783= Cyc_Absyn_cast_exp(
_temp1779, e1, 0); goto _LL1784; _LL1784:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1753, _temp1779,( struct Cyc_Absyn_Exp*) _temp1783,
_temp1757, 0), 0))->r)); goto _LL1718;}}}}}}}} _LL1722: if( _temp1439 == 0){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1785=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1785->f1= Cyc_Toc_abs_ns; _temp1785->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1785;}), 0),({ struct Cyc_List_List* _temp1786=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1786->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1787=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1787[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1788; _temp1788.tag= Cyc_Absyn_Const_e; _temp1788.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1789=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1789[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1790; _temp1790.tag= Cyc_Absyn_String_c;
_temp1790.f1= _temp1735; _temp1790;}); _temp1789;})); _temp1788;}); _temp1787;}),
0); _temp1786->tl= 0; _temp1786;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( _temp1439)); Cyc_Toc_exp_to_c(
nv, r);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1791=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1791->f1= Cyc_Toc_abs_ns; _temp1791->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1791;}), 0),({ struct Cyc_List_List* _temp1792=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1792->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1793=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1793[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1794; _temp1794.tag= Cyc_Absyn_Const_e; _temp1794.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1795=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1795[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1796; _temp1796.tag= Cyc_Absyn_String_c;
_temp1796.f1= _temp1735; _temp1796;}); _temp1795;})); _temp1794;}); _temp1793;}),
0); _temp1792->tl= 0; _temp1792;}), 0))->r));} goto _LL1718; _LL1724: { int
is_tagged_ptr= 0;{ void* _temp1797= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1803; struct Cyc_Absyn_Conref* _temp1805; struct Cyc_Absyn_Tqual _temp1807;
struct Cyc_Absyn_Conref* _temp1809; void* _temp1811; void* _temp1813; _LL1799:
if(( unsigned int) _temp1797 > 4u?*(( int*) _temp1797) == Cyc_Absyn_PointerType:
0){ _LL1804: _temp1803=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1797)->f1; _LL1814: _temp1813=( void*) _temp1803.elt_typ; goto _LL1812;
_LL1812: _temp1811=( void*) _temp1803.rgn_typ; goto _LL1810; _LL1810: _temp1809=(
struct Cyc_Absyn_Conref*) _temp1803.nullable; goto _LL1808; _LL1808: _temp1807=(
struct Cyc_Absyn_Tqual) _temp1803.tq; goto _LL1806; _LL1806: _temp1805=( struct
Cyc_Absyn_Conref*) _temp1803.bounds; goto _LL1800;} else{ goto _LL1801;} _LL1801:
goto _LL1802; _LL1800:{ void* _temp1815=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1805))->v; void*
_temp1821; _LL1817: if(( unsigned int) _temp1815 > 1u?*(( int*) _temp1815) ==
Cyc_Absyn_Eq_constr: 0){ _LL1822: _temp1821=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1815)->f1; if( _temp1821 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1818;}
else{ goto _LL1819;}} else{ goto _LL1819;} _LL1819: goto _LL1820; _LL1818:
is_tagged_ptr= 1; goto _LL1816; _LL1820: goto _LL1816; _LL1816:;} goto _LL1798;
_LL1802:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1823=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1824; _temp1824.curr= _temp1823; _temp1824.base=
_temp1823; _temp1824.last_plus_one= _temp1823 + 42; _temp1824;})); goto _LL1798;
_LL1798:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1737->topt))->v;
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1739);{ struct Cyc_Absyn_Stmt*
s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( a, 0), _temp1741, Cyc_Absyn_var_exp(
max, 0), _temp1737, Cyc_Absyn_skip_stmt( 0), 1); struct Cyc_List_List* decls=({
struct Cyc_List_List* _temp1851=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1851->hd=( void*)({ struct _tuple10* _temp1852=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1852->f1= max;
_temp1852->f2= Cyc_Absyn_uint_t; _temp1852->f3=( struct Cyc_Absyn_Exp*)
_temp1739; _temp1852;}); _temp1851->tl= 0; _temp1851;}); struct Cyc_Absyn_Exp*
ai; if( _temp1439 == 0){ ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{
struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1439)); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1825=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1825->hd=( void*)({ struct _tuple10*
_temp1826=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1826->f1=
a; _temp1826->f2= ptr_typ; _temp1826->f3=( struct Cyc_Absyn_Exp*) ainit;
_temp1826;}); _temp1825->tl= decls; _temp1825;}); if( is_tagged_ptr){ struct
_tuple0* _temp1827= Cyc_Toc_temp_var(); goto _LL1828; _LL1828: { void* _temp1829=
Cyc_Toc_typ_to_c( old_typ); goto _LL1830; _LL1830: { struct Cyc_Absyn_Exp*
_temp1837= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List* _temp1831=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1831->hd=(
void*)({ struct _tuple4* _temp1836=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1836->f1= 0; _temp1836->f2= Cyc_Absyn_var_exp( a, 0); _temp1836;});
_temp1831->tl=({ struct Cyc_List_List* _temp1832=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1832->hd=( void*)({ struct
_tuple4* _temp1835=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1835->f1= 0; _temp1835->f2= Cyc_Absyn_var_exp( a, 0); _temp1835;});
_temp1832->tl=({ struct Cyc_List_List* _temp1833=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1833->hd=( void*)({ struct
_tuple4* _temp1834=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1834->f1= 0; _temp1834->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1834;}); _temp1833->tl= 0; _temp1833;});
_temp1832;}); _temp1831;}), 0); goto _LL1838; _LL1838: decls=({ struct Cyc_List_List*
_temp1839=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1839->hd=( void*)({ struct _tuple10* _temp1840=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1840->f1= _temp1827; _temp1840->f2=
_temp1829; _temp1840->f3=( struct Cyc_Absyn_Exp*) _temp1837; _temp1840;});
_temp1839->tl= decls; _temp1839;}); s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( _temp1827, 0), 0), 0);}}} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List* _temp1841= decls; goto
_LL1842; _LL1842: for( 0; _temp1841 != 0; _temp1841=(( struct Cyc_List_List*)
_check_null( _temp1841))->tl){ struct Cyc_Absyn_Exp* _temp1845; void* _temp1847;
struct _tuple0* _temp1849; struct _tuple10 _temp1843=*(( struct _tuple10*)((
struct Cyc_List_List*) _check_null( _temp1841))->hd); _LL1850: _temp1849=
_temp1843.f1; goto _LL1848; _LL1848: _temp1847= _temp1843.f2; goto _LL1846;
_LL1846: _temp1845= _temp1843.f3; goto _LL1844; _LL1844: s= Cyc_Absyn_declare_stmt(
_temp1849, _temp1847, _temp1845, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r)); goto _LL1718;}}}} _LL1726:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1437->topt))->v, 1,
_temp1439, _temp1745, _temp1749))->r)); goto _LL1718; _LL1728:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1439, _temp1751))->r)); goto
_LL1718; _LL1730: { void* _temp1853=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1437->topt))->v; goto _LL1854; _LL1854: { struct _tuple0* _temp1855= Cyc_Toc_temp_var();
goto _LL1856; _LL1856: { struct Cyc_Absyn_Stmt* _temp1857= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( _temp1855, 0), 0); goto _LL1858; _LL1858: { struct Cyc_Absyn_Exp*
_temp1859= Cyc_Absyn_signed_int_exp( 0, 0); goto _LL1860; _LL1860: Cyc_Toc_exp_to_c(
nv, _temp1437); _temp1857= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1855, 0), _temp1859, 0), _temp1437, 0), _temp1857, 0);{
void* elt_typ= Cyc_Toc_typ_to_c( _temp1853); void* _temp1861= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1862; _LL1862: { struct Cyc_Absyn_Exp*
_temp1863= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); goto _LL1864; _LL1864: { struct
Cyc_Absyn_Exp* e1; if( _temp1439 == 0){ e1= Cyc_Toc_malloc_exp( _temp1853,
_temp1863);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1439)); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1863);}{ struct Cyc_Absyn_Exp* _temp1865= Cyc_Absyn_cast_exp( _temp1861, e1,
0); goto _LL1866; _LL1866:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1855, _temp1861,( struct Cyc_Absyn_Exp*) _temp1865,
_temp1857, 0), 0))->r)); goto _LL1718;}}}}}}}} _LL1718:;} goto _LL1304; _LL1340:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct* _temp1867=(
struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1867[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1868; _temp1868.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1868.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1441);
_temp1868;}); _temp1867;}))); goto _LL1304; _LL1342: Cyc_Toc_exp_to_c( nv,
_temp1443); goto _LL1304; _LL1344: { void* e1_typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1445->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1445);{ void* _temp1869= e1_typ; struct Cyc_Absyn_PtrInfo _temp1875; struct
Cyc_Absyn_Conref* _temp1877; struct Cyc_Absyn_Tqual _temp1879; struct Cyc_Absyn_Conref*
_temp1881; void* _temp1883; void* _temp1885; _LL1871: if(( unsigned int)
_temp1869 > 4u?*(( int*) _temp1869) == Cyc_Absyn_PointerType: 0){ _LL1876:
_temp1875=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1869)->f1; _LL1886: _temp1885=( void*) _temp1875.elt_typ; goto _LL1884;
_LL1884: _temp1883=( void*) _temp1875.rgn_typ; goto _LL1882; _LL1882: _temp1881=(
struct Cyc_Absyn_Conref*) _temp1875.nullable; goto _LL1880; _LL1880: _temp1879=(
struct Cyc_Absyn_Tqual) _temp1875.tq; goto _LL1878; _LL1878: _temp1877=( struct
Cyc_Absyn_Conref*) _temp1875.bounds; goto _LL1872;} else{ goto _LL1873;} _LL1873:
goto _LL1874; _LL1872:{ void* _temp1887=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1877); struct Cyc_Absyn_Exp* _temp1893; _LL1889: if(
_temp1887 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1890;} else{ goto _LL1891;}
_LL1891: if(( unsigned int) _temp1887 > 1u?*(( int*) _temp1887) == Cyc_Absyn_Upper_b:
0){ _LL1894: _temp1893=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1887)->f1; goto _LL1892;} else{ goto _LL1888;} _LL1890: { struct _tuple0* x=
Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void* _temp1895= Cyc_Toc_typ_to_c_array(
_temp1885); goto _LL1896; _LL1896: { void* tx= Cyc_Toc_add_tagged_ptr_type(
_temp1895, _temp1879); void* ta2= Cyc_Absyn_cstar_typ( _temp1895, _temp1879);
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
struct Cyc_Absyn_Exp*) _temp1445, s1, 0), 0))->r)); goto _LL1888;}} _LL1892: if(
Cyc_Toc_is_nullable( e1_typ)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1445->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1897=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1897->hd=(
void*) _temp1445; _temp1897->tl= 0; _temp1897;}), 0), 0), 0))->r));} else{
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, e1_typ,(
struct Cyc_Absyn_Exp*) _temp1445, s3, 0), 0))->r));}} goto _LL1888; _LL1888:;}
goto _LL1870; _LL1874:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1898=( char*)"exp_to_c: Deref: non-pointer";
struct _tagged_string _temp1899; _temp1899.curr= _temp1898; _temp1899.base=
_temp1898; _temp1899.last_plus_one= _temp1898 + 29; _temp1899;})); goto _LL1870;
_LL1870:;} goto _LL1304;} _LL1346: Cyc_Toc_exp_to_c( nv, _temp1449); if( Cyc_Toc_is_poly_project(
e)){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1304; _LL1348: Cyc_Toc_exp_to_c( nv,
_temp1453);{ int is_poly= Cyc_Toc_is_poly_project( e); if( Cyc_Toc_is_nullable((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1453->topt))->v)){ if( Cyc_Toc_use_runtimecheck_funs){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1453->topt))->v),
Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct
Cyc_List_List* _temp1900=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1900->hd=( void*) _temp1453; _temp1900->tl= 0; _temp1900;}), 0), 0),
_temp1451, 0))->r));} else{ struct _tuple0* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1453->topt))->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp1451, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1453, s3, 0), 0))->r));}} if( is_poly){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto
_LL1304;} _LL1350: { void* _temp1901= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1457->topt))->v); goto _LL1902; _LL1902: Cyc_Toc_exp_to_c( nv,
_temp1457); Cyc_Toc_exp_to_c( nv, _temp1455);( void*)( _temp1457->r=( void*)((
void*)( Cyc_Toc_array_to_ptr( Cyc_Toc_typ_to_c( _temp1901), Cyc_Absyn_new_exp((
void*) _temp1457->r, 0), 0))->r));{ void* _temp1903= _temp1901; struct Cyc_List_List*
_temp1911; struct Cyc_Absyn_PtrInfo _temp1913; struct Cyc_Absyn_Conref*
_temp1915; struct Cyc_Absyn_Tqual _temp1917; struct Cyc_Absyn_Conref* _temp1919;
void* _temp1921; void* _temp1923; _LL1905: if(( unsigned int) _temp1903 > 4u?*((
int*) _temp1903) == Cyc_Absyn_TupleType: 0){ _LL1912: _temp1911=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1903)->f1; goto _LL1906;} else{ goto
_LL1907;} _LL1907: if(( unsigned int) _temp1903 > 4u?*(( int*) _temp1903) == Cyc_Absyn_PointerType:
0){ _LL1914: _temp1913=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1903)->f1; _LL1924: _temp1923=( void*) _temp1913.elt_typ; goto _LL1922;
_LL1922: _temp1921=( void*) _temp1913.rgn_typ; goto _LL1920; _LL1920: _temp1919=(
struct Cyc_Absyn_Conref*) _temp1913.nullable; goto _LL1918; _LL1918: _temp1917=(
struct Cyc_Absyn_Tqual) _temp1913.tq; goto _LL1916; _LL1916: _temp1915=( struct
Cyc_Absyn_Conref*) _temp1913.bounds; goto _LL1908;} else{ goto _LL1909;} _LL1909:
goto _LL1910; _LL1906: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1455) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1457,({ struct _tagged_string* _temp1925=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1925[ 0]= xprintf("f%d",( int)
i); _temp1925;}), 0))->r)); goto _LL1904;} _LL1908:{ void* _temp1926=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1915); struct Cyc_Absyn_Exp*
_temp1932; _LL1928: if(( unsigned int) _temp1926 > 1u?*(( int*) _temp1926) ==
Cyc_Absyn_Upper_b: 0){ _LL1933: _temp1932=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1926)->f1; goto _LL1929;} else{ goto _LL1930;} _LL1930: if( _temp1926 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1931;} else{ goto _LL1927;} _LL1929:
_temp1932= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp( _temp1932), 0);{
int possibly_null;{ void* _temp1934=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1919))->v; int
_temp1940; _LL1936: if(( unsigned int) _temp1934 > 1u?*(( int*) _temp1934) ==
Cyc_Absyn_Eq_constr: 0){ _LL1941: _temp1940=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1934)->f1; goto _LL1937;} else{ goto _LL1938;} _LL1938: goto _LL1939;
_LL1937: possibly_null= _temp1940; goto _LL1935; _LL1939: possibly_null= 0; goto
_LL1935; _LL1935:;}{ void* ta1= Cyc_Toc_typ_to_c( _temp1923); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1917); if( Cyc_Toc_use_runtimecheck_funs){ if( possibly_null){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv, 0),({ struct Cyc_List_List*
_temp1942=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1942->hd=( void*) _temp1457; _temp1942->tl=({ struct Cyc_List_List*
_temp1943=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1943->hd=( void*) _temp1932; _temp1943->tl=({ struct Cyc_List_List*
_temp1944=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1944->hd=( void*) Cyc_Absyn_sizeoftyp_exp( ta1, 0); _temp1944->tl=({ struct
Cyc_List_List* _temp1945=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1945->hd=( void*) _temp1455; _temp1945->tl= 0; _temp1945;}); _temp1944;});
_temp1943;}); _temp1942;}), 0), 0), 0))->r));} else{( void*)( _temp1455->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List* _temp1946=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1946->hd=( void*) _temp1932; _temp1946->tl=({
struct Cyc_List_List* _temp1947=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1947->hd=( void*) Cyc_Absyn_copy_exp( _temp1455);
_temp1947->tl= 0; _temp1947;}); _temp1946;}), 0))->r));}} else{ struct _tuple0*
a= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte, Cyc_Absyn_var_exp( i, 0),
_temp1932, 0); if( possibly_null){ e3= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_new_exp(( void*) e3->r, 0),
0);}{ struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt(
i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) _temp1455, s3, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*)
_temp1457, s1, 0), 0))->r));}} goto _LL1927;}} _LL1931: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp1923); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1917); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp1954=({ struct Cyc_List_List* _temp1948=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1948->hd=( void*)({ struct
_tuple1* _temp1953=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1953->f1= 0; _temp1953->f2= Cyc_Toc_mt_tq; _temp1953->f3= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1457->topt))->v); _temp1953;});
_temp1948->tl=({ struct Cyc_List_List* _temp1949=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1949->hd=( void*)({ struct
_tuple1* _temp1952=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1952->f1= 0; _temp1952->f2= Cyc_Toc_mt_tq; _temp1952->f3= Cyc_Absyn_uint_t;
_temp1952;}); _temp1949->tl=({ struct Cyc_List_List* _temp1950=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1950->hd=( void*)({ struct
_tuple1* _temp1951=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1951->f1= 0; _temp1951->f2= Cyc_Toc_mt_tq; _temp1951->f3= Cyc_Absyn_uint_t;
_temp1951;}); _temp1950->tl= 0; _temp1950;}); _temp1949;}); _temp1948;}); goto
_LL1955; _LL1955: { void* _temp1959= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1956=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1956[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1957; _temp1957.tag= Cyc_Absyn_FnType;
_temp1957.f1=({ struct Cyc_Absyn_FnInfo _temp1958; _temp1958.tvars= 0; _temp1958.effect=
0; _temp1958.ret_typ=( void*) ta2; _temp1958.args= _temp1954; _temp1958.varargs=
0; _temp1958.attributes= 0; _temp1958;}); _temp1957;}); _temp1956;}), Cyc_Toc_mt_tq);
goto _LL1960; _LL1960: { struct Cyc_Absyn_Exp* fnexp= Cyc_Absyn_cast_exp(
_temp1959, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv, 0), 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_fncall_exp( fnexp,({
struct Cyc_List_List* _temp1961=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1961->hd=( void*) _temp1457; _temp1961->tl=({
struct Cyc_List_List* _temp1962=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1962->hd=( void*) Cyc_Absyn_sizeoftyp_exp( ta1, 0);
_temp1962->tl=({ struct Cyc_List_List* _temp1963=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1963->hd=( void*) _temp1455;
_temp1963->tl= 0; _temp1963;}); _temp1962;}); _temp1961;}), 0), 0))->r));}}}
else{ struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type(
ta1, _temp1917); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1455, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1457, s1, 0), 0))->r));} goto _LL1927;} _LL1927:;}
goto _LL1904; _LL1910:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1964=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1965; _temp1965.curr= _temp1964; _temp1965.base=
_temp1964; _temp1965.last_plus_one= _temp1964 + 49; _temp1965;})); goto _LL1904;
_LL1904:;} goto _LL1304;} _LL1352: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1459))->r));} else{ struct Cyc_List_List*
_temp1966=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1459); goto
_LL1967; _LL1967: { struct _tagged_string* _temp1968= Cyc_Toc_add_tuple_type(
_temp1966); goto _LL1969; _LL1969: { struct Cyc_List_List* dles= 0;{ int i= 1;
for( 0; _temp1459 != 0; _temp1459=(( struct Cyc_List_List*) _check_null(
_temp1459))->tl, i ++){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1459))->hd);{ struct Cyc_List_List* _temp1974=({
struct Cyc_List_List* _temp1970=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1970->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct*
_temp1971=( struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp1971[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp1972; _temp1972.tag= Cyc_Absyn_FieldName;
_temp1972.f1=({ struct _tagged_string* _temp1973=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1973[ 0]= xprintf("f%d", i);
_temp1973;}); _temp1972;}); _temp1971;})); _temp1970->tl= 0; _temp1970;}); goto
_LL1975; _LL1975: dles=({ struct Cyc_List_List* _temp1976=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1976->hd=( void*)({ struct
_tuple4* _temp1977=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1977->f1=( struct Cyc_List_List*) _temp1974; _temp1977->f2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1459))->hd; _temp1977;}); _temp1976->tl=
dles; _temp1976;});}}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_strct( _temp1968), Cyc_Absyn_unresolvedmem_exp( 0, dles, 0), 0))->r));}}}
goto _LL1304; _LL1354:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1978=( char*)"compoundlit"; struct
_tagged_string _temp1979; _temp1979.curr= _temp1978; _temp1979.base= _temp1978;
_temp1979.last_plus_one= _temp1978 + 12; _temp1979;})); goto _LL1304; _LL1356:(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1465, 0))->r));{
struct Cyc_List_List* _temp1980= _temp1465; goto _LL1981; _LL1981: for( 0;
_temp1980 != 0; _temp1980=(( struct Cyc_List_List*) _check_null( _temp1980))->tl){
struct _tuple4 _temp1984; struct Cyc_Absyn_Exp* _temp1985; struct Cyc_List_List*
_temp1987; struct _tuple4* _temp1982=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1980))->hd; _temp1984=* _temp1982; _LL1988: _temp1987=
_temp1984.f1; goto _LL1986; _LL1986: _temp1985= _temp1984.f2; goto _LL1983;
_LL1983: Cyc_Toc_exp_to_c( nv, _temp1985);}} goto _LL1304; _LL1358: {
unsigned int _temp1989= Cyc_Evexp_eval_const_uint_exp( _temp1469); goto _LL1990;
_LL1990: Cyc_Toc_exp_to_c( nv, _temp1467);{ struct Cyc_List_List* es= 0; if( !
Cyc_Toc_is_zero( _temp1467)){ unsigned int i= 0; for( 0; i < _temp1989; i ++){
es=({ struct Cyc_List_List* _temp1991=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1991->hd=( void*)({ struct _tuple4*
_temp1992=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1992->f1=
0; _temp1992->f2= _temp1467; _temp1992;}); _temp1991->tl= es; _temp1991;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL1304;}} _LL1360: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1475, _temp1479))->r));} else{
struct _tuple0* _temp1993= _temp1479; goto _LL1994; _LL1994: { struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Structdecl*) _check_null(
_temp1473 == 0?(( struct Cyc_Absyn_Structdecl*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp2007= Cyc_Absynpp_qvar2string( _temp1479); xprintf("no structdecl %.*s",
_temp2007.last_plus_one - _temp2007.curr, _temp2007.curr);})): _temp1473));
struct _RegionHandle _temp1995= _new_region(); struct _RegionHandle* rgn=&
_temp1995; _push_region( rgn);{ struct Cyc_List_List* _temp1996=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1475,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Structdecl*) sd2)->fields))->v); goto _LL1997; _LL1997: for( 0;
_temp1996 != 0; _temp1996=(( struct Cyc_List_List*) _check_null( _temp1996))->tl){
struct _tuple12 _temp2000; struct Cyc_Absyn_Exp* _temp2001; struct Cyc_Absyn_Structfield*
_temp2003; struct _tuple12* _temp1998=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( _temp1996))->hd; _temp2000=* _temp1998; _LL2004: _temp2003=
_temp2000.f1; goto _LL2002; _LL2002: _temp2001= _temp2000.f2; goto _LL1999;
_LL1999: { void* _temp2005=( void*) _temp2003->type; goto _LL2006; _LL2006: Cyc_Toc_exp_to_c(
nv, _temp2001); if( Cyc_Toc_is_void_star( _temp2005)){( void*)( _temp2001->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp2001->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_strctq( _temp1993), Cyc_Absyn_unresolvedmem_exp( 0, _temp1475, 0), 0))->r));};
_pop_region(& _temp1995);}} goto _LL1304; _LL1362: { struct Cyc_List_List* fs;{
void* _temp2008= Cyc_Tcutil_compress( _temp1483); struct Cyc_List_List*
_temp2014; _LL2010: if(( unsigned int) _temp2008 > 4u?*(( int*) _temp2008) ==
Cyc_Absyn_AnonStructType: 0){ _LL2015: _temp2014=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp2008)->f1; goto _LL2011;} else{
goto _LL2012;} _LL2012: goto _LL2013; _LL2011: fs= _temp2014; goto _LL2009;
_LL2013: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp2016= Cyc_Absynpp_typ2string( _temp1483); xprintf("anon struct has type %.*s",
_temp2016.last_plus_one - _temp2016.curr, _temp2016.curr);})); goto _LL2009;
_LL2009:;}{ struct _RegionHandle _temp2017= _new_region(); struct _RegionHandle*
rgn=& _temp2017; _push_region( rgn);{ struct Cyc_List_List* _temp2018=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1481, fs); goto _LL2019; _LL2019: for( 0; _temp2018 != 0;
_temp2018=(( struct Cyc_List_List*) _check_null( _temp2018))->tl){ struct
_tuple12 _temp2022; struct Cyc_Absyn_Exp* _temp2023; struct Cyc_Absyn_Structfield*
_temp2025; struct _tuple12* _temp2020=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( _temp2018))->hd; _temp2022=* _temp2020; _LL2026: _temp2025=
_temp2022.f1; goto _LL2024; _LL2024: _temp2023= _temp2022.f2; goto _LL2021;
_LL2021: { void* _temp2027=( void*) _temp2025->type; goto _LL2028; _LL2028: Cyc_Toc_exp_to_c(
nv, _temp2023); if( Cyc_Toc_is_void_star( _temp2027)){( void*)( _temp2023->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp2023->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1481, 0))->r));}; _pop_region(& _temp2017);} goto _LL1304;} _LL1364: {
struct _tuple0* qv= _temp1485->name; if( _temp1487->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp2029= _temp1487->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1487->fields))->v;
goto _LL2030; _LL2030: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2029))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp2029))->hd)->typs
== 0){ tag_count ++;} _temp2029=(( struct Cyc_List_List*) _check_null( _temp2029))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1304;} _LL1366: { struct _tuple0* _temp2031= _temp1495->name; goto
_LL2032; _LL2032: { struct Cyc_List_List* _temp2033= _temp1495->typs; goto
_LL2034; _LL2034: { struct _tuple0* _temp2035= Cyc_Toc_temp_var(); goto _LL2036;
_LL2036: { struct Cyc_Absyn_Exp* _temp2037= Cyc_Absyn_var_exp( _temp2035, 0);
goto _LL2038; _LL2038: { void* _temp2041= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2031,( struct _tagged_string)({ char* _temp2039=( char*)"_struct"; struct
_tagged_string _temp2040; _temp2040.curr= _temp2039; _temp2040.base= _temp2039;
_temp2040.last_plus_one= _temp2039 + 8; _temp2040;}))); goto _LL2042; _LL2042:
if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp; if( _temp1497->is_xtunion){
tag_exp= Cyc_Absyn_var_exp( _temp2031, 0);} else{ int tag_count= 0; struct Cyc_List_List*
_temp2043= _temp1497->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1497->fields))->v; goto _LL2044; _LL2044: while( Cyc_Absyn_qvar_cmp(
_temp2031,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2043))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2043))->hd)->typs != 0){ tag_count ++;}
_temp2043=(( struct Cyc_List_List*) _check_null( _temp2043))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2045= 0; goto _LL2046;
_LL2046: for( 0; _temp1499 != 0; _temp1499=(( struct Cyc_List_List*) _check_null(
_temp1499))->tl, _temp2033=(( struct Cyc_List_List*) _check_null( _temp2033))->tl){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1499))->hd; void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple3*)(( struct Cyc_List_List*) _check_null( _temp2033))->hd)).f2); Cyc_Toc_exp_to_c(
nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2045=({ struct Cyc_List_List* _temp2047=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2047[ 0]=({ struct Cyc_List_List
_temp2048; _temp2048.hd=( void*)({ struct _tuple4* _temp2049=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2049[ 0]=({ struct _tuple4
_temp2050; _temp2050.f1= 0; _temp2050.f2= cur_e; _temp2050;}); _temp2049;});
_temp2048.tl= _temp2045; _temp2048;}); _temp2047;});} _temp2045=({ struct Cyc_List_List*
_temp2051=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2051[ 0]=({ struct Cyc_List_List _temp2052; _temp2052.hd=( void*)({ struct
_tuple4* _temp2053=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2053[ 0]=({ struct _tuple4 _temp2054; _temp2054.f1= 0; _temp2054.f2=
tag_exp; _temp2054;}); _temp2053;}); _temp2052.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2045); _temp2052;}); _temp2051;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2055=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2055->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2031,( struct
_tagged_string)({ char* _temp2056=( char*)"_struct"; struct _tagged_string
_temp2057; _temp2057.curr= _temp2056; _temp2057.base= _temp2056; _temp2057.last_plus_one=
_temp2056 + 8; _temp2057;})); _temp2055;}), _temp2045, 0))->r));}} else{ struct
Cyc_List_List* _temp2059=({ struct Cyc_List_List* _temp2058=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2058->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2037, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp2031, 0), 0); _temp2058->tl= 0; _temp2058;}); goto _LL2060; _LL2060:{ int i=
1; for( 0; _temp1499 != 0;( _temp1499=(( struct Cyc_List_List*) _check_null(
_temp1499))->tl, i ++), _temp2033=(( struct Cyc_List_List*) _check_null(
_temp2033))->tl){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1499))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2033))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp2062= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2037,({ struct _tagged_string* _temp2061=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2061[ 0]= xprintf("f%d", i); _temp2061;}), 0), cur_e, 0); goto _LL2063;
_LL2063: _temp2059=({ struct Cyc_List_List* _temp2064=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2064->hd=( void*) _temp2062;
_temp2064->tl= _temp2059; _temp2064;});}}}{ struct Cyc_Absyn_Stmt* _temp2065=
Cyc_Absyn_exp_stmt( _temp2037, 0); goto _LL2066; _LL2066: { struct Cyc_Absyn_Stmt*
_temp2068= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp2067=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2067->hd=( void*) _temp2065;
_temp2067->tl= _temp2059; _temp2067;})), 0); goto _LL2069; _LL2069:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp2035,
_temp2041, 0, _temp2068, 0), 0))->r));}}} goto _LL1304;}}}}} _LL1368: goto
_LL1304; _LL1370: { void* t_c= Cyc_Toc_typ_to_c( _temp1511); if( _temp1513 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1513)); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)((
void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1511, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} goto _LL1304;} _LL1372: Cyc_Toc_stmt_to_c( nv, _temp1515); goto
_LL1304; _LL1374:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2070=( char*)"UnresolvedMem"; struct
_tagged_string _temp2071; _temp2071.curr= _temp2070; _temp2071.base= _temp2070;
_temp2071.last_plus_one= _temp2070 + 14; _temp2071;})); goto _LL1304; _LL1376:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp2072=( char*)"codegen"; struct _tagged_string _temp2073; _temp2073.curr=
_temp2072; _temp2073.base= _temp2072; _temp2073.last_plus_one= _temp2072 + 8;
_temp2073;})); goto _LL1304; _LL1378:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2074=( char*)"fill"; struct _tagged_string
_temp2075; _temp2075.curr= _temp2074; _temp2075.base= _temp2074; _temp2075.last_plus_one=
_temp2074 + 5; _temp2075;})); goto _LL1304; _LL1304:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple14{ struct _tuple0* f1; void* f2;
} ; struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, void* t,
struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat* p,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp2076=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp2112; struct Cyc_Absyn_Vardecl _temp2114; struct Cyc_List_List* _temp2115;
struct Cyc_Core_Opt* _temp2117; struct Cyc_Absyn_Exp* _temp2119; void* _temp2121;
struct Cyc_Absyn_Tqual _temp2123; struct _tuple0* _temp2125; void* _temp2127;
struct Cyc_Absyn_Vardecl* _temp2129; int _temp2131; void* _temp2133; char
_temp2135; struct _tagged_string _temp2137; struct Cyc_Absyn_Enumfield*
_temp2139; struct Cyc_Absyn_Enumdecl* _temp2141; struct Cyc_List_List* _temp2143;
struct Cyc_List_List* _temp2145; struct Cyc_Absyn_Tunionfield* _temp2147; struct
Cyc_Absyn_Tuniondecl* _temp2149; struct Cyc_Absyn_Pat* _temp2152; struct Cyc_Absyn_Pat
_temp2154; struct Cyc_Position_Segment* _temp2155; struct Cyc_Core_Opt*
_temp2157; void* _temp2159; struct Cyc_List_List* _temp2161; struct Cyc_List_List*
_temp2163; struct Cyc_Absyn_Tunionfield* _temp2165; struct Cyc_Absyn_Tuniondecl*
_temp2167; struct Cyc_List_List* _temp2169; struct Cyc_List_List* _temp2171;
struct Cyc_Absyn_Tunionfield* _temp2173; struct Cyc_Absyn_Tuniondecl* _temp2175;
struct Cyc_List_List* _temp2177; struct Cyc_List_List* _temp2179; struct Cyc_List_List*
_temp2181; struct Cyc_Core_Opt* _temp2183; struct Cyc_Absyn_Structdecl*
_temp2185; struct Cyc_Absyn_Pat* _temp2187; struct _tuple0* _temp2189; struct
Cyc_List_List* _temp2191; struct Cyc_List_List* _temp2193; struct _tuple0*
_temp2195; struct Cyc_List_List* _temp2197; struct Cyc_List_List* _temp2199;
struct _tuple0* _temp2201; _LL2078: if(( unsigned int) _temp2076 > 2u?*(( int*)
_temp2076) == Cyc_Absyn_Var_p: 0){ _LL2113: _temp2112=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp2076)->f1; _temp2114=* _temp2112; _LL2128:
_temp2127=( void*) _temp2114.sc; goto _LL2126; _LL2126: _temp2125=( struct
_tuple0*) _temp2114.name; goto _LL2124; _LL2124: _temp2123=( struct Cyc_Absyn_Tqual)
_temp2114.tq; goto _LL2122; _LL2122: _temp2121=( void*) _temp2114.type; goto
_LL2120; _LL2120: _temp2119=( struct Cyc_Absyn_Exp*) _temp2114.initializer; goto
_LL2118; _LL2118: _temp2117=( struct Cyc_Core_Opt*) _temp2114.rgn; goto _LL2116;
_LL2116: _temp2115=( struct Cyc_List_List*) _temp2114.attributes; goto _LL2079;}
else{ goto _LL2080;} _LL2080: if( _temp2076 ==( void*) Cyc_Absyn_Wild_p){ goto
_LL2081;} else{ goto _LL2082;} _LL2082: if(( unsigned int) _temp2076 > 2u?*((
int*) _temp2076) == Cyc_Absyn_Reference_p: 0){ _LL2130: _temp2129=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Reference_p_struct*) _temp2076)->f1; goto _LL2083;} else{ goto
_LL2084;} _LL2084: if( _temp2076 ==( void*) Cyc_Absyn_Null_p){ goto _LL2085;}
else{ goto _LL2086;} _LL2086: if(( unsigned int) _temp2076 > 2u?*(( int*)
_temp2076) == Cyc_Absyn_Int_p: 0){ _LL2134: _temp2133=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp2076)->f1; goto _LL2132; _LL2132: _temp2131=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp2076)->f2; goto _LL2087;} else{ goto _LL2088;} _LL2088: if(( unsigned int)
_temp2076 > 2u?*(( int*) _temp2076) == Cyc_Absyn_Char_p: 0){ _LL2136: _temp2135=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp2076)->f1; goto _LL2089;} else{
goto _LL2090;} _LL2090: if(( unsigned int) _temp2076 > 2u?*(( int*) _temp2076)
== Cyc_Absyn_Float_p: 0){ _LL2138: _temp2137=( struct _tagged_string)(( struct
Cyc_Absyn_Float_p_struct*) _temp2076)->f1; goto _LL2091;} else{ goto _LL2092;}
_LL2092: if(( unsigned int) _temp2076 > 2u?*(( int*) _temp2076) == Cyc_Absyn_Enum_p:
0){ _LL2142: _temp2141=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp2076)->f1; goto _LL2140; _LL2140: _temp2139=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp2076)->f2; goto _LL2093;} else{ goto
_LL2094;} _LL2094: if(( unsigned int) _temp2076 > 2u?*(( int*) _temp2076) == Cyc_Absyn_Tunion_p:
0){ _LL2150: _temp2149=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2076)->f1; goto _LL2148; _LL2148: _temp2147=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2076)->f2; goto _LL2146; _LL2146:
_temp2145=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2076)->f3; goto _LL2144; _LL2144: _temp2143=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2076)->f4; if( _temp2143 == 0){ goto
_LL2095;} else{ goto _LL2096;}} else{ goto _LL2096;} _LL2096: if(( unsigned int)
_temp2076 > 2u?*(( int*) _temp2076) == Cyc_Absyn_Pointer_p: 0){ _LL2153:
_temp2152=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2076)->f1; _temp2154=* _temp2152; _LL2160: _temp2159=( void*) _temp2154.r;
if(( unsigned int) _temp2159 > 2u?*(( int*) _temp2159) == Cyc_Absyn_Tunion_p: 0){
_LL2168: _temp2167=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2159)->f1; goto _LL2166; _LL2166: _temp2165=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2159)->f2; goto _LL2164; _LL2164:
_temp2163=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2159)->f3; goto _LL2162; _LL2162: _temp2161=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2159)->f4; goto _LL2158;} else{ goto
_LL2098;} _LL2158: _temp2157=( struct Cyc_Core_Opt*) _temp2154.topt; goto
_LL2156; _LL2156: _temp2155=( struct Cyc_Position_Segment*) _temp2154.loc; goto
_LL2151;} else{ goto _LL2098;} _LL2151: if( _temp2161 != 0){ goto _LL2097;}
else{ goto _LL2098;} _LL2098: if(( unsigned int) _temp2076 > 2u?*(( int*)
_temp2076) == Cyc_Absyn_Tunion_p: 0){ _LL2176: _temp2175=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2076)->f1; goto _LL2174; _LL2174:
_temp2173=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2076)->f2; goto _LL2172; _LL2172: _temp2171=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2076)->f3; goto _LL2170; _LL2170:
_temp2169=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2076)->f4; goto _LL2099;} else{ goto _LL2100;} _LL2100: if(( unsigned int)
_temp2076 > 2u?*(( int*) _temp2076) == Cyc_Absyn_Tuple_p: 0){ _LL2178: _temp2177=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp2076)->f1; goto
_LL2101;} else{ goto _LL2102;} _LL2102: if(( unsigned int) _temp2076 > 2u?*((
int*) _temp2076) == Cyc_Absyn_Struct_p: 0){ _LL2186: _temp2185=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp2076)->f1; goto _LL2184; _LL2184:
_temp2183=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp2076)->f2;
goto _LL2182; _LL2182: _temp2181=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp2076)->f3; goto _LL2180; _LL2180: _temp2179=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp2076)->f4; goto _LL2103;} else{ goto
_LL2104;} _LL2104: if(( unsigned int) _temp2076 > 2u?*(( int*) _temp2076) == Cyc_Absyn_Pointer_p:
0){ _LL2188: _temp2187=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2076)->f1; goto _LL2105;} else{ goto _LL2106;} _LL2106: if(( unsigned int)
_temp2076 > 2u?*(( int*) _temp2076) == Cyc_Absyn_UnknownId_p: 0){ _LL2190:
_temp2189=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2076)->f1;
goto _LL2107;} else{ goto _LL2108;} _LL2108: if(( unsigned int) _temp2076 > 2u?*((
int*) _temp2076) == Cyc_Absyn_UnknownCall_p: 0){ _LL2196: _temp2195=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp2076)->f1; goto _LL2194;
_LL2194: _temp2193=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2076)->f2; goto _LL2192; _LL2192: _temp2191=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp2076)->f3; goto _LL2109;} else{
goto _LL2110;} _LL2110: if(( unsigned int) _temp2076 > 2u?*(( int*) _temp2076)
== Cyc_Absyn_UnknownFields_p: 0){ _LL2202: _temp2201=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp2076)->f1; goto _LL2200; _LL2200:
_temp2199=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2076)->f2; goto _LL2198; _LL2198: _temp2197=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp2076)->f3; goto _LL2111;} else{
goto _LL2077;} _LL2079: nv= Cyc_Toc_add_varmap( nv, _temp2125, r); goto _LL2081;
_LL2081: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL2077; _LL2083: { struct
_tuple0* _temp2203= Cyc_Toc_temp_var(); goto _LL2204; _LL2204: decls=({ struct
Cyc_List_List* _temp2205=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2205->hd=( void*)({ struct _tuple14* _temp2206=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2206->f1= _temp2203; _temp2206->f2=
Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2206;});
_temp2205->tl= decls; _temp2205;}); nv= Cyc_Toc_add_varmap( nv, _temp2129->name,
Cyc_Absyn_var_exp( _temp2203, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2203, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2077;} _LL2085: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2077; _LL2087: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2133, _temp2131, 0), succ_lab, fail_lab); goto _LL2077;
_LL2089: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2135, 0),
succ_lab, fail_lab); goto _LL2077; _LL2091: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2137, 0), succ_lab, fail_lab); goto _LL2077; _LL2093: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2207=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2207[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2208; _temp2208.tag= Cyc_Absyn_Enum_e;
_temp2208.f1= _temp2139->name; _temp2208.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2141; _temp2208.f3=( struct Cyc_Absyn_Enumfield*) _temp2139; _temp2208;});
_temp2207;}), 0), succ_lab, fail_lab); goto _LL2077; _LL2095: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2147->name, 0); if( ! _temp2149->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2077;} _LL2097: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2161);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2165->name,( struct
_tagged_string)({ char* _temp2238=( char*)"_struct"; struct _tagged_string
_temp2239; _temp2239.curr= _temp2238; _temp2239.base= _temp2238; _temp2239.last_plus_one=
_temp2238 + 8; _temp2239;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0);{ struct
Cyc_List_List* _temp2209=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp2161); goto _LL2210; _LL2210: for( 0; _temp2209 != 0;
_temp2209=(( struct Cyc_List_List*) _check_null( _temp2209))->tl, cnt --){
struct Cyc_Absyn_Pat* _temp2211=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2209))->hd; goto _LL2212; _LL2212: { struct _tuple0* _temp2213=
Cyc_Toc_temp_var(); goto _LL2214; _LL2214: { void* _temp2215=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2211->topt))->v; goto _LL2216; _LL2216: { void*
_temp2217= Cyc_Toc_typ_to_c( _temp2215); goto _LL2218; _LL2218: { struct
_tagged_string* _temp2219= Cyc_Toc_fresh_label(); goto _LL2220; _LL2220: decls=({
struct Cyc_List_List* _temp2221=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2221->hd=( void*)({ struct _tuple14* _temp2222=(
struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2222->f1= _temp2213;
_temp2222->f2= _temp2217; _temp2222;}); _temp2221->tl= decls; _temp2221;});{
struct _tuple13 _temp2224= Cyc_Toc_xlate_pat( nv, _temp2215, Cyc_Absyn_var_exp(
_temp2213, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string*
_temp2223=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2223[ 0]= xprintf("f%d", cnt); _temp2223;}), 0), _temp2211, succ_lab,
fail_lab, decls); goto _LL2225; _LL2225: nv= _temp2224.f1; decls= _temp2224.f2;{
struct Cyc_Absyn_Stmt* _temp2226= _temp2224.f3; goto _LL2227; _LL2227: { struct
Cyc_Absyn_Stmt* _temp2229= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2213,
0), Cyc_Absyn_cast_exp( _temp2217, Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp2228=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2228[ 0]= xprintf("f%d", cnt); _temp2228;}), 0), 0),
0); goto _LL2230; _LL2230: { struct Cyc_Absyn_Stmt* _temp2231= Cyc_Absyn_seq_stmt(
_temp2229, _temp2226, 0); goto _LL2232; _LL2232: ss=({ struct Cyc_List_List*
_temp2233=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2233->hd=( void*) Cyc_Absyn_label_stmt( _temp2219, _temp2231, 0); _temp2233->tl=
ss; _temp2233;}); succ_lab= _temp2219;}}}}}}}}}}{ struct Cyc_Absyn_Exp* test_exp;
if( _temp2167->is_xtunion){ struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp(
e2, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2165->name, 0);
test_exp= Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e, 0), Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);}
else{ unsigned int max_tag= 0;{ struct Cyc_List_List* _temp2234=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2167->fields))->v; goto _LL2235; _LL2235:
for( 0; _temp2234 != 0; _temp2234=(( struct Cyc_List_List*) _check_null(
_temp2234))->tl){ struct Cyc_Absyn_Tunionfield* _temp2236=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2234))->hd; goto _LL2237; _LL2237: if(
_temp2236->typs == 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp(
max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp(
e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2165->name,
0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0):
Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2077;}} _LL2099:
_temp2177= _temp2169; goto _LL2101; _LL2101: { struct Cyc_List_List* ss= 0; int
cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2177);{ struct
Cyc_List_List* _temp2240=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp2177); goto _LL2241; _LL2241: for( 0; _temp2240 != 0;
_temp2240=(( struct Cyc_List_List*) _check_null( _temp2240))->tl, cnt --){
struct Cyc_Absyn_Pat* _temp2242=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2240))->hd; goto _LL2243; _LL2243: { struct _tuple0* _temp2244=
Cyc_Toc_temp_var(); goto _LL2245; _LL2245: { void* _temp2246=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2242->topt))->v; goto _LL2247; _LL2247: {
struct _tagged_string* _temp2248= Cyc_Toc_fresh_label(); goto _LL2249; _LL2249:
decls=({ struct Cyc_List_List* _temp2250=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2250->hd=( void*)({ struct _tuple14*
_temp2251=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2251->f1=
_temp2244; _temp2251->f2= Cyc_Toc_typ_to_c( _temp2246); _temp2251;}); _temp2250->tl=
decls; _temp2250;});{ struct _tuple13 _temp2253= Cyc_Toc_xlate_pat( nv,
_temp2246, Cyc_Absyn_var_exp( _temp2244, 0), Cyc_Absyn_structmember_exp( path,({
struct _tagged_string* _temp2252=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2252[ 0]= xprintf("f%d", cnt); _temp2252;}), 0),
_temp2242, succ_lab, fail_lab, decls); goto _LL2254; _LL2254: nv= _temp2253.f1;
decls= _temp2253.f2;{ struct Cyc_Absyn_Stmt* _temp2255= _temp2253.f3; goto
_LL2256; _LL2256: { struct Cyc_Absyn_Stmt* _temp2258= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2244, 0), Cyc_Absyn_structmember_exp( r,({ struct
_tagged_string* _temp2257=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2257[ 0]= xprintf("f%d", cnt); _temp2257;}), 0), 0),
_temp2255, 0); goto _LL2259; _LL2259: ss=({ struct Cyc_List_List* _temp2260=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2260->hd=(
void*) Cyc_Absyn_label_stmt( _temp2248, _temp2258, 0); _temp2260->tl= ss;
_temp2260;}); succ_lab= _temp2248;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2077;} _LL2103: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2261=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2179); goto _LL2262; _LL2262: for( 0; _temp2261 != 0; _temp2261=(( struct
Cyc_List_List*) _check_null( _temp2261))->tl){ struct _tuple15* _temp2263=(
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2261))->hd; goto
_LL2264; _LL2264: { struct Cyc_Absyn_Pat* _temp2265=(* _temp2263).f2; goto
_LL2266; _LL2266: { struct _tagged_string* f;{ void* _temp2267=( void*)(( struct
Cyc_List_List*) _check_null((* _temp2263).f1))->hd; struct _tagged_string*
_temp2273; _LL2269: if(*(( int*) _temp2267) == Cyc_Absyn_FieldName){ _LL2274:
_temp2273=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp2267)->f1; goto _LL2270;} else{ goto _LL2271;} _LL2271: goto _LL2272;
_LL2270: f= _temp2273; goto _LL2268; _LL2272:( void) _throw(( void*) Cyc_Toc_Match_error);
_LL2268:;}{ struct _tuple0* _temp2275= Cyc_Toc_temp_var(); goto _LL2276; _LL2276: {
void* _temp2277=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2265->topt))->v;
goto _LL2278; _LL2278: { void* _temp2279= Cyc_Toc_typ_to_c( _temp2277); goto
_LL2280; _LL2280: { struct _tagged_string* _temp2281= Cyc_Toc_fresh_label();
goto _LL2282; _LL2282: decls=({ struct Cyc_List_List* _temp2283=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2283->hd=( void*)({ struct
_tuple14* _temp2284=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp2284->f1= _temp2275; _temp2284->f2= _temp2279; _temp2284;}); _temp2283->tl=
decls; _temp2283;});{ struct _tuple13 _temp2285= Cyc_Toc_xlate_pat( nv,
_temp2277, Cyc_Absyn_var_exp( _temp2275, 0), Cyc_Absyn_structmember_exp( path, f,
0), _temp2265, succ_lab, fail_lab, decls); goto _LL2286; _LL2286: nv= _temp2285.f1;
decls= _temp2285.f2;{ struct Cyc_Absyn_Stmt* _temp2287= _temp2285.f3; goto
_LL2288; _LL2288: { struct Cyc_Absyn_Stmt* _temp2289= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2275, 0), Cyc_Absyn_cast_exp( _temp2279, Cyc_Absyn_structmember_exp(
r, f, 0), 0), 0), _temp2287, 0); goto _LL2290; _LL2290: ss=({ struct Cyc_List_List*
_temp2291=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2291->hd=( void*) Cyc_Absyn_label_stmt( _temp2281, _temp2289, 0); _temp2291->tl=
ss; _temp2291;}); succ_lab= _temp2281;}}}}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL2077;} _LL2105: { struct _tuple0* _temp2292= Cyc_Toc_temp_var(); goto
_LL2293; _LL2293: { void* _temp2294=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp2187->topt))->v; goto _LL2295; _LL2295: decls=({ struct Cyc_List_List*
_temp2296=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2296->hd=( void*)({ struct _tuple14* _temp2297=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2297->f1= _temp2292; _temp2297->f2=
Cyc_Toc_typ_to_c( _temp2294); _temp2297;}); _temp2296->tl= decls; _temp2296;});{
struct _tuple13 _temp2298= Cyc_Toc_xlate_pat( nv, _temp2294, Cyc_Absyn_var_exp(
_temp2292, 0), Cyc_Absyn_deref_exp( path, 0), _temp2187, succ_lab, fail_lab,
decls); goto _LL2299; _LL2299: nv= _temp2298.f1; decls= _temp2298.f2;{ struct
Cyc_Absyn_Stmt* _temp2300= _temp2298.f3; goto _LL2301; _LL2301: { struct Cyc_Absyn_Stmt*
_temp2302= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2292, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2300, 0); goto _LL2303;
_LL2303: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
_temp2302, 0);} else{ s= _temp2302;} goto _LL2077;}}}}} _LL2107: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp2304=( char*)"unknownid"; struct _tagged_string _temp2305; _temp2305.curr=
_temp2304; _temp2305.base= _temp2304; _temp2305.last_plus_one= _temp2304 + 10;
_temp2305;})); goto _LL2077; _LL2109: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2306=(
char*)"unknowncall"; struct _tagged_string _temp2307; _temp2307.curr= _temp2306;
_temp2307.base= _temp2306; _temp2307.last_plus_one= _temp2306 + 12; _temp2307;}));
goto _LL2077; _LL2111: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2308=( char*)"unknownfields";
struct _tagged_string _temp2309; _temp2309.curr= _temp2308; _temp2309.base=
_temp2308; _temp2309.last_plus_one= _temp2308 + 14; _temp2309;})); goto _LL2077;
_LL2077:;} return({ struct _tuple13 _temp2310; _temp2310.f1= nv; _temp2310.f2=
decls; _temp2310.f3= s; _temp2310;});} struct _tuple16{ struct _tagged_string*
f1; struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause* f3; } ; static
struct _tuple16* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause* sc){ return({
struct _tuple16* _temp2311=( struct _tuple16*) GC_malloc( sizeof( struct
_tuple16)); _temp2311->f1= Cyc_Toc_fresh_label(); _temp2311->f2= Cyc_Toc_fresh_label();
_temp2311->f3= sc; _temp2311;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp2312=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2313; _LL2313: { int leave_as_switch;{ void*
_temp2314= Cyc_Tcutil_compress( _temp2312); void* _temp2320; void* _temp2322;
_LL2316: if(( unsigned int) _temp2314 > 4u?*(( int*) _temp2314) == Cyc_Absyn_IntType:
0){ _LL2323: _temp2322=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2314)->f1;
goto _LL2321; _LL2321: _temp2320=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2314)->f2; goto _LL2317;} else{ goto _LL2318;} _LL2318: goto _LL2319;
_LL2317: leave_as_switch= 1; goto _LL2315; _LL2319: leave_as_switch= 0; goto
_LL2315; _LL2315:;}{ struct Cyc_List_List* _temp2324= scs; goto _LL2325; _LL2325:
for( 0; _temp2324 != 0; _temp2324=(( struct Cyc_List_List*) _check_null(
_temp2324))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2324))->hd)->pat_vars))->v != 0){ leave_as_switch= 0; break;}}}
if( leave_as_switch){ struct _tagged_string* next_l= Cyc_Toc_fresh_label();{
struct Cyc_List_List* _temp2326= scs; goto _LL2327; _LL2327: for( 0; _temp2326
!= 0; _temp2326=(( struct Cyc_List_List*) _check_null( _temp2326))->tl){ struct
Cyc_Absyn_Stmt* _temp2328=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2326))->hd)->body; goto _LL2329; _LL2329:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2326))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp2328, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp2328);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_string* end_l= Cyc_Toc_fresh_label();
struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)( struct _tuple16*(* f)(
struct Cyc_Absyn_Switch_clause*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label,
scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0; struct
Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2330= lscs; goto _LL2331;
_LL2331: for( 0; _temp2330 != 0; _temp2330=(( struct Cyc_List_List*) _check_null(
_temp2330))->tl){ struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( _temp2330))->hd)).f3; struct _tagged_string*
fail_lab=(( struct Cyc_List_List*) _check_null( _temp2330))->tl == 0? end_l:(*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp2330))->tl))->hd)).f1; struct _tagged_string* succ_lab=(*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2330))->hd)).f2;
if( sc->where_clause == 0){ struct _tuple13 _temp2332= Cyc_Toc_xlate_pat( nv,
_temp2312, r, path, sc->pattern, succ_lab, fail_lab, decls); goto _LL2333;
_LL2333: nvs=({ struct Cyc_List_List* _temp2334=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2334->hd=( void*) _temp2332.f1;
_temp2334->tl= nvs; _temp2334;}); decls= _temp2332.f2; test_stmts=({ struct Cyc_List_List*
_temp2335=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2335->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2330))->hd)).f1, _temp2332.f3, 0); _temp2335->tl= test_stmts;
_temp2335;});} else{ struct Cyc_Absyn_Exp* _temp2336=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*) _check_null( sc->where_clause)); goto _LL2337; _LL2337: {
struct _tagged_string* _temp2338= Cyc_Toc_fresh_label(); goto _LL2339; _LL2339: {
struct _tuple13 _temp2340= Cyc_Toc_xlate_pat( nv, _temp2312, r, path, sc->pattern,
_temp2338, fail_lab, decls); goto _LL2341; _LL2341: Cyc_Toc_exp_to_c( _temp2340.f1,
_temp2336); nvs=({ struct Cyc_List_List* _temp2342=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2342->hd=( void*) _temp2340.f1;
_temp2342->tl= nvs; _temp2342;}); decls= _temp2340.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2336, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2338, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2343=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2343->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2330))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2340.f3, s3, 0), 0); _temp2343->tl= test_stmts;
_temp2343;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2344= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2345; _LL2345: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0; lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl){ struct Cyc_Toc_Env* _temp2346=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2347; _LL2347: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2346, end_l,(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2348=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2349; _LL2349: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2348, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2344, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple14 _temp2352;
void* _temp2353; struct _tuple0* _temp2355; struct _tuple14* _temp2350=( struct
_tuple14*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2352=*
_temp2350; _LL2356: _temp2355= _temp2352.f1; goto _LL2354; _LL2354: _temp2353=
_temp2352.f2; goto _LL2351; _LL2351: res= Cyc_Absyn_declare_stmt( _temp2355,
_temp2353, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct
Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt* Cyc_Toc_make_npop_handler(
int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv,
0),({ struct Cyc_List_List* _temp2357=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List) * 1); _temp2357[ 0]=({ struct Cyc_List_List
_temp2358; _temp2358.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0);
_temp2358.tl= 0; _temp2358;}); _temp2357;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2359=( void*) s->r; struct Cyc_Absyn_Exp*
_temp2399; struct Cyc_Absyn_Stmt* _temp2401; struct Cyc_Absyn_Stmt* _temp2403;
struct Cyc_Absyn_Exp* _temp2405; struct Cyc_Absyn_Stmt* _temp2407; struct Cyc_Absyn_Stmt*
_temp2409; struct Cyc_Absyn_Exp* _temp2411; struct Cyc_Absyn_Stmt* _temp2413;
struct _tuple2 _temp2415; struct Cyc_Absyn_Stmt* _temp2417; struct Cyc_Absyn_Exp*
_temp2419; struct Cyc_Absyn_Stmt* _temp2421; struct Cyc_Absyn_Stmt* _temp2423;
struct Cyc_Absyn_Stmt* _temp2425; struct _tagged_string* _temp2427; struct Cyc_Absyn_Stmt*
_temp2429; struct _tuple2 _temp2431; struct Cyc_Absyn_Stmt* _temp2433; struct
Cyc_Absyn_Exp* _temp2435; struct _tuple2 _temp2437; struct Cyc_Absyn_Stmt*
_temp2439; struct Cyc_Absyn_Exp* _temp2441; struct Cyc_Absyn_Exp* _temp2443;
struct Cyc_List_List* _temp2445; struct Cyc_Absyn_Exp* _temp2447; struct Cyc_Absyn_Switch_clause**
_temp2449; struct Cyc_List_List* _temp2451; struct Cyc_Absyn_Stmt* _temp2453;
struct Cyc_Absyn_Decl* _temp2455; struct Cyc_Absyn_Stmt* _temp2457; struct Cyc_Absyn_Stmt*
_temp2459; struct Cyc_Absyn_Stmt* _temp2461; struct _tagged_string* _temp2463;
struct _tuple2 _temp2465; struct Cyc_Absyn_Stmt* _temp2467; struct Cyc_Absyn_Exp*
_temp2469; struct Cyc_Absyn_Stmt* _temp2471; struct Cyc_List_List* _temp2473;
struct Cyc_Absyn_Stmt* _temp2475; struct Cyc_Absyn_Stmt* _temp2477; struct Cyc_Absyn_Vardecl*
_temp2479; struct Cyc_Absyn_Tvar* _temp2481; _LL2361: if( _temp2359 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2362;} else{ goto _LL2363;} _LL2363: if((
unsigned int) _temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_Exp_s: 0){
_LL2400: _temp2399=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp2359)->f1; goto _LL2364;} else{ goto _LL2365;} _LL2365: if(( unsigned int)
_temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_Seq_s: 0){ _LL2404: _temp2403=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp2359)->f1; goto
_LL2402; _LL2402: _temp2401=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp2359)->f2; goto _LL2366;} else{ goto _LL2367;} _LL2367: if(( unsigned int)
_temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_Return_s: 0){ _LL2406:
_temp2405=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp2359)->f1; goto _LL2368;} else{ goto _LL2369;} _LL2369: if(( unsigned int)
_temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_IfThenElse_s: 0){ _LL2412:
_temp2411=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2359)->f1; goto _LL2410; _LL2410: _temp2409=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2359)->f2; goto _LL2408; _LL2408:
_temp2407=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2359)->f3; goto _LL2370;} else{ goto _LL2371;} _LL2371: if(( unsigned int)
_temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_While_s: 0){ _LL2416: _temp2415=(
struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp2359)->f1; _LL2420:
_temp2419= _temp2415.f1; goto _LL2418; _LL2418: _temp2417= _temp2415.f2; goto
_LL2414; _LL2414: _temp2413=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp2359)->f2; goto _LL2372;} else{ goto _LL2373;} _LL2373: if(( unsigned int)
_temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_Break_s: 0){ _LL2422: _temp2421=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp2359)->f1; goto
_LL2374;} else{ goto _LL2375;} _LL2375: if(( unsigned int) _temp2359 > 1u?*((
int*) _temp2359) == Cyc_Absyn_Continue_s: 0){ _LL2424: _temp2423=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp2359)->f1; goto _LL2376;} else{ goto
_LL2377;} _LL2377: if(( unsigned int) _temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_Goto_s:
0){ _LL2428: _temp2427=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp2359)->f1; goto _LL2426; _LL2426: _temp2425=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp2359)->f2; goto _LL2378;} else{ goto
_LL2379;} _LL2379: if(( unsigned int) _temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_For_s:
0){ _LL2444: _temp2443=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp2359)->f1; goto _LL2438; _LL2438: _temp2437=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp2359)->f2; _LL2442: _temp2441= _temp2437.f1; goto _LL2440; _LL2440:
_temp2439= _temp2437.f2; goto _LL2432; _LL2432: _temp2431=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp2359)->f3; _LL2436: _temp2435= _temp2431.f1;
goto _LL2434; _LL2434: _temp2433= _temp2431.f2; goto _LL2430; _LL2430: _temp2429=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp2359)->f4; goto
_LL2380;} else{ goto _LL2381;} _LL2381: if(( unsigned int) _temp2359 > 1u?*((
int*) _temp2359) == Cyc_Absyn_Switch_s: 0){ _LL2448: _temp2447=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp2359)->f1; goto _LL2446; _LL2446:
_temp2445=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp2359)->f2; goto _LL2382;} else{ goto _LL2383;} _LL2383: if(( unsigned int)
_temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_Fallthru_s: 0){ _LL2452:
_temp2451=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2359)->f1; goto _LL2450; _LL2450: _temp2449=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp2359)->f2; goto _LL2384;} else{ goto
_LL2385;} _LL2385: if(( unsigned int) _temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_Decl_s:
0){ _LL2456: _temp2455=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp2359)->f1; goto _LL2454; _LL2454: _temp2453=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp2359)->f2; goto _LL2386;} else{ goto
_LL2387;} _LL2387: if(( unsigned int) _temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_Cut_s:
0){ _LL2458: _temp2457=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp2359)->f1; goto _LL2388;} else{ goto _LL2389;} _LL2389: if(( unsigned int)
_temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_Splice_s: 0){ _LL2460:
_temp2459=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp2359)->f1; goto _LL2390;} else{ goto _LL2391;} _LL2391: if(( unsigned int)
_temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_Label_s: 0){ _LL2464: _temp2463=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp2359)->f1; goto
_LL2462; _LL2462: _temp2461=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp2359)->f2; goto _LL2392;} else{ goto _LL2393;} _LL2393: if(( unsigned int)
_temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_Do_s: 0){ _LL2472: _temp2471=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp2359)->f1; goto
_LL2466; _LL2466: _temp2465=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp2359)->f2; _LL2470: _temp2469= _temp2465.f1; goto _LL2468; _LL2468:
_temp2467= _temp2465.f2; goto _LL2394;} else{ goto _LL2395;} _LL2395: if((
unsigned int) _temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_TryCatch_s: 0){
_LL2476: _temp2475=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2359)->f1; goto _LL2474; _LL2474: _temp2473=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp2359)->f2; goto _LL2396;} else{ goto
_LL2397;} _LL2397: if(( unsigned int) _temp2359 > 1u?*(( int*) _temp2359) == Cyc_Absyn_Region_s:
0){ _LL2482: _temp2481=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2359)->f1; goto _LL2480; _LL2480: _temp2479=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp2359)->f2; goto _LL2478; _LL2478:
_temp2477=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2359)->f3; goto _LL2398;} else{ goto _LL2360;} _LL2362: return; _LL2364:
Cyc_Toc_exp_to_c( nv, _temp2399); return; _LL2366: Cyc_Toc_stmt_to_c( nv,
_temp2403); s= _temp2401; continue; _LL2368: { struct Cyc_Core_Opt* topt= 0; if(
_temp2405 != 0){ topt=({ struct Cyc_Core_Opt* _temp2483=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2483->v=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*) _check_null(
_temp2405))->topt))->v); _temp2483;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*) _check_null( _temp2405)));} if( s->try_depth > 0){ if(
topt != 0){ struct _tuple0* _temp2484= Cyc_Toc_temp_var(); goto _LL2485; _LL2485: {
struct Cyc_Absyn_Stmt* _temp2486= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*)
Cyc_Absyn_var_exp( _temp2484, 0), 0); goto _LL2487; _LL2487:( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp2484,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp2405, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp2486, 0), 0))->r));}} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL2370: Cyc_Toc_exp_to_c( nv, _temp2411); Cyc_Toc_stmt_to_c( nv,
_temp2409); s= _temp2407; continue; _LL2372: Cyc_Toc_exp_to_c( nv, _temp2419);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2413); return; _LL2374: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)(( struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{
int dest_depth= _temp2421 == 0? 0:(( struct Cyc_Absyn_Stmt*) _check_null(
_temp2421))->try_depth; Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s);
return;} _LL2376: if( nv->continue_lab != 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_goto_stmt(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before( s->try_depth
-(( struct Cyc_Absyn_Stmt*) _check_null( _temp2423))->try_depth, s); return;
_LL2378: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2425))->try_depth, s); return; _LL2380: Cyc_Toc_exp_to_c( nv,
_temp2443); Cyc_Toc_exp_to_c( nv, _temp2441); Cyc_Toc_exp_to_c( nv, _temp2435);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2429); return; _LL2382: Cyc_Toc_xlate_switch(
nv, s, _temp2447, _temp2445); return; _LL2384: if( nv->fallthru_info == 0){(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp2488=( char*)"fallthru in unexpected place"; struct
_tagged_string _temp2489; _temp2489.curr= _temp2488; _temp2489.base= _temp2488;
_temp2489.last_plus_one= _temp2488 + 29; _temp2489;})));}{ struct _tuple8
_temp2492; struct Cyc_Dict_Dict* _temp2493; struct Cyc_List_List* _temp2495;
struct _tagged_string* _temp2497; struct _tuple8* _temp2490=( struct _tuple8*)((
struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp2492=* _temp2490;
_LL2498: _temp2497= _temp2492.f1; goto _LL2496; _LL2496: _temp2495= _temp2492.f2;
goto _LL2494; _LL2494: _temp2493= _temp2492.f3; goto _LL2491; _LL2491: { struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp2497, 0); Cyc_Toc_do_npop_before(
s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp2449)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp2499=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2495); goto _LL2500; _LL2500: { struct Cyc_List_List*
_temp2501=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2451); goto _LL2502; _LL2502: for( 0; _temp2499 != 0; _temp2499=(( struct
Cyc_List_List*) _check_null( _temp2499))->tl, _temp2501=(( struct Cyc_List_List*)
_check_null( _temp2501))->tl){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp2501))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp2493,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp2499))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp2501))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}}} _LL2386:{ void* _temp2503=( void*) _temp2455->r; int
_temp2511; struct Cyc_Absyn_Exp* _temp2513; struct Cyc_Core_Opt* _temp2515;
struct Cyc_Core_Opt* _temp2517; struct Cyc_Absyn_Pat* _temp2519; struct Cyc_Absyn_Vardecl*
_temp2521; _LL2505: if(*(( int*) _temp2503) == Cyc_Absyn_Let_d){ _LL2520:
_temp2519=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2503)->f1;
goto _LL2518; _LL2518: _temp2517=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2503)->f2; goto _LL2516; _LL2516: _temp2515=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2503)->f3; goto _LL2514; _LL2514: _temp2513=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2503)->f4; goto
_LL2512; _LL2512: _temp2511=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2503)->f5;
goto _LL2506;} else{ goto _LL2507;} _LL2507: if(*(( int*) _temp2503) == Cyc_Absyn_Var_d){
_LL2522: _temp2521=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2503)->f1; goto _LL2508;} else{ goto _LL2509;} _LL2509: goto _LL2510;
_LL2506:( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2519,
_temp2515, _temp2513, _temp2511, _temp2453))->r)); goto _LL2504; _LL2508: {
struct _tuple0* _temp2523= _temp2521->name; goto _LL2524; _LL2524:( void*)(
_temp2521->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2521->type));{
struct Cyc_Toc_Env* _temp2527= Cyc_Toc_add_varmap( nv, _temp2523, Cyc_Absyn_varb_exp(
_temp2523,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp2525=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2525[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2526; _temp2526.tag= Cyc_Absyn_Local_b; _temp2526.f1=
_temp2521; _temp2526;}); _temp2525;}), 0)); goto _LL2528; _LL2528: Cyc_Toc_stmt_to_c(
_temp2527, _temp2453); if( _temp2521->initializer != 0){ struct Cyc_Absyn_Exp*
init=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( _temp2521->initializer));
void* _temp2529=( void*) init->r; struct Cyc_Absyn_Exp* _temp2535; struct Cyc_Absyn_Exp*
_temp2537; struct Cyc_Absyn_Vardecl* _temp2539; _LL2531: if(*(( int*) _temp2529)
== Cyc_Absyn_Comprehension_e){ _LL2540: _temp2539=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2529)->f1; goto _LL2538; _LL2538:
_temp2537=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2529)->f2; goto _LL2536; _LL2536: _temp2535=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2529)->f3; goto _LL2532;} else{
goto _LL2533;} _LL2533: goto _LL2534; _LL2532: _temp2521->initializer= 0;( void*)(
_temp2453->r=( void*)(( void*)( Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
_temp2523, 0), _temp2539, _temp2537, _temp2535, Cyc_Absyn_new_stmt(( void*)
_temp2453->r, 0), 0))->r)); goto _LL2530; _LL2534: Cyc_Toc_exp_to_c( nv, init);
goto _LL2530; _LL2530:;} goto _LL2504;}} _LL2510:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2541=(
char*)"bad nested declaration within function"; struct _tagged_string _temp2542;
_temp2542.curr= _temp2541; _temp2542.base= _temp2541; _temp2542.last_plus_one=
_temp2541 + 39; _temp2542;})); goto _LL2504; _LL2504:;} return; _LL2388:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp2543=( char*)"cut"; struct _tagged_string _temp2544; _temp2544.curr=
_temp2543; _temp2544.base= _temp2543; _temp2544.last_plus_one= _temp2543 + 4;
_temp2544;})); return; _LL2390:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2545=( char*)"splice"; struct _tagged_string
_temp2546; _temp2546.curr= _temp2545; _temp2546.base= _temp2545; _temp2546.last_plus_one=
_temp2545 + 7; _temp2546;})); return; _LL2392: s= _temp2461; continue; _LL2394:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2471); Cyc_Toc_exp_to_c( nv,
_temp2469); return; _LL2396: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct
_tuple0* e_var= Cyc_Toc_temp_var(); struct _tuple0* was_thrown_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2589=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2589->f1= Cyc_Toc_abs_ns; _temp2589->f2= Cyc_Toc__push_handler_sp;
_temp2589;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple0* _temp2588=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2588->f1= Cyc_Toc_abs_ns; _temp2588->f2= Cyc_Toc_setjmp_sp; _temp2588;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2587=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2587->f1=
Cyc_Toc_abs_ns; _temp2587->f2= Cyc_Toc__pop_handler_sp; _temp2587;}), 0); struct
Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp(({ struct _tuple0* _temp2586=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2586->f1= Cyc_Toc_abs_ns;
_temp2586->f2= Cyc_Toc__exn_thrown_sp; _temp2586;}), 0); e_exp->topt=({ struct
Cyc_Core_Opt* _temp2547=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2547->v=( void*) e_typ; _temp2547;}); nv= Cyc_Toc_add_varmap( nv, e_var,
e_exp); Cyc_Toc_stmt_to_c( nv, _temp2475);{ struct Cyc_Absyn_Stmt* _temp2548=
Cyc_Absyn_seq_stmt( _temp2475, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0); goto _LL2549; _LL2549: { struct _tuple0*
_temp2550= Cyc_Toc_temp_var(); goto _LL2551; _LL2551: { struct Cyc_Absyn_Exp*
_temp2552= Cyc_Absyn_var_exp( _temp2550, 0); goto _LL2553; _LL2553: { struct Cyc_Absyn_Vardecl*
_temp2554= Cyc_Absyn_new_vardecl( _temp2550, Cyc_Absyn_exn_typ, 0); goto _LL2555;
_LL2555: _temp2552->topt=({ struct Cyc_Core_Opt* _temp2556=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2556->v=( void*) Cyc_Absyn_exn_typ;
_temp2556;});{ struct Cyc_Absyn_Pat* _temp2561=({ struct Cyc_Absyn_Pat*
_temp2557=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof( struct Cyc_Absyn_Pat));
_temp2557->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp2559=(
struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2559[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2560; _temp2560.tag= Cyc_Absyn_Var_p;
_temp2560.f1= _temp2554; _temp2560;}); _temp2559;})); _temp2557->topt=({ struct
Cyc_Core_Opt* _temp2558=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2558->v=( void*) Cyc_Absyn_exn_typ; _temp2558;}); _temp2557->loc= 0;
_temp2557;}); goto _LL2562; _LL2562: { struct Cyc_Absyn_Exp* _temp2563= Cyc_Absyn_throw_exp(
_temp2552, 0); goto _LL2564; _LL2564: _temp2563->topt=({ struct Cyc_Core_Opt*
_temp2565=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2565->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2565;});{ struct Cyc_Absyn_Stmt*
_temp2566= Cyc_Absyn_exp_stmt( _temp2563, 0); goto _LL2567; _LL2567: { struct
Cyc_Absyn_Switch_clause* _temp2571=({ struct Cyc_Absyn_Switch_clause* _temp2568=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2568->pattern= _temp2561; _temp2568->pat_vars=({ struct Cyc_Core_Opt*
_temp2569=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2569->v=( void*)({ struct Cyc_List_List* _temp2570=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2570->hd=( void*) _temp2554;
_temp2570->tl= 0; _temp2570;}); _temp2569;}); _temp2568->where_clause= 0;
_temp2568->body= _temp2566; _temp2568->loc= 0; _temp2568;}); goto _LL2572;
_LL2572: { struct Cyc_Absyn_Stmt* _temp2574= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2473,({ struct Cyc_List_List* _temp2573=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2573->hd=( void*) _temp2571; _temp2573->tl=
0; _temp2573;})), 0); goto _LL2575; _LL2575: Cyc_Toc_stmt_to_c( nv, _temp2574);{
struct Cyc_Absyn_Exp* _temp2577= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2576=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2576->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2576->tl= 0; _temp2576;}), 0); goto _LL2578; _LL2578: { struct Cyc_Absyn_Stmt*
_temp2580= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2579=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2579->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2579->tl= 0;
_temp2579;}), 0), 0); goto _LL2581; _LL2581: { struct Cyc_Absyn_Exp* _temp2582=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2583; _LL2583: {
struct Cyc_Absyn_Exp* _temp2584= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2585; _LL2585:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2580, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2582, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2577, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2584, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2548, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2574, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2398: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__region_handle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2479->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2595=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2595[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2596; _temp2596.tag= Cyc_Absyn_Local_b; _temp2596.f1=
_temp2479; _temp2596;}); _temp2595;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2594=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2594->f1= Cyc_Toc_abs_ns; _temp2594->f2= Cyc_Toc__push_region_sp;
_temp2594;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2593=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2593->f1= Cyc_Toc_abs_ns; _temp2593->f2= Cyc_Toc__pop_region_sp; _temp2593;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2592=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2592->f1=
Cyc_Toc_abs_ns; _temp2592->f2= Cyc_Toc__new_region_sp; _temp2592;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2591=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2591->hd=( void*) addr_rh_exp; _temp2591->tl= 0; _temp2591;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2590=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2590->hd=( void*) x_exp; _temp2590->tl= 0; _temp2590;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2477);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2477, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2360:;}} struct _tuple17{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0;(
void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{ struct
Cyc_List_List* _temp2597= f->args; goto _LL2598; _LL2598: for( 0; _temp2597 != 0;
_temp2597=(( struct Cyc_List_List*) _check_null( _temp2597))->tl){(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp2597))->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp2597))->hd)).f3);{
struct _tuple0* _temp2600=({ struct _tuple0* _temp2599=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2599->f1=( void*) Cyc_Absyn_Loc_n;
_temp2599->f2=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp2597))->hd)).f1; _temp2599;}); goto _LL2601; _LL2601: nv= Cyc_Toc_add_varmap(
nv, _temp2600, Cyc_Absyn_var_exp( _temp2600, 0));}}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2602= s;
_LL2604: if( _temp2602 ==( void*) Cyc_Absyn_Abstract){ goto _LL2605;} else{ goto
_LL2606;} _LL2606: if( _temp2602 ==( void*) Cyc_Absyn_ExternC){ goto _LL2607;}
else{ goto _LL2608;} _LL2608: goto _LL2609; _LL2605: return( void*) Cyc_Absyn_Public;
_LL2607: return( void*) Cyc_Absyn_Extern; _LL2609: return s; _LL2603:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2610=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2611; _LL2611: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2612=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2612->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2612;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2613=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2610).f2); goto _LL2614;
_LL2614: if( _temp2613 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2610).f2, s));}
else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null(
_temp2613))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2610).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2615=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2615->v=( void*)
_temp2610; _temp2615;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2616=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2617; _LL2617: for( 0; _temp2616 != 0;
_temp2616=(( struct Cyc_List_List*) _check_null( _temp2616))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2616))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2616))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2618=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2619; _LL2619: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2620=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2620->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2620;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2621=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2618).f2); goto _LL2622; _LL2622:
if( _temp2621 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2618).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2621))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2618).f2, u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)(
u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==(
void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?( void*)
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2623=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2623->v=( void*) _temp2618; _temp2623;}); if( u->fields !=
0){ if( seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* _temp2624=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( u->fields))->v; goto
_LL2625; _LL2625: for( 0; _temp2624 != 0; _temp2624=(( struct Cyc_List_List*)
_check_null( _temp2624))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2624))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2624))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2626=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2626->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2626;});}{ struct
_tuple0* _temp2627= tud->name; goto _LL2628; _LL2628: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2627)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2627));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2629=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2630; _LL2630: for( 0;
_temp2629 != 0; _temp2629=(( struct Cyc_List_List*) _check_null( _temp2629))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2629))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(( tag_count ++), 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2631; _temp2631.q_const= 1; _temp2631.q_volatile= 0;
_temp2631.q_restrict= 0; _temp2631;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2632=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2632->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2633=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2633[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2634; _temp2634.tag= Cyc_Absyn_Var_d;
_temp2634.f1= vd; _temp2634;}); _temp2633;}), 0); _temp2632->tl= Cyc_Toc_result_decls;
_temp2632;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
box_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2635; _temp2635.q_const=
1; _temp2635.q_volatile= 0; _temp2635.q_restrict= 0; _temp2635;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2636=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2636->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2637=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2637[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2638; _temp2638.tag= Cyc_Absyn_Var_d; _temp2638.f1= vd; _temp2638;});
_temp2637;}), 0); _temp2636->tl= Cyc_Toc_result_decls; _temp2636;});{ struct Cyc_List_List*
_temp2639= 0; goto _LL2640; _LL2640: { int i= 1;{ struct Cyc_List_List*
_temp2641= f->typs; goto _LL2642; _LL2642: for( 0; _temp2641 != 0; _temp2641=((
struct Cyc_List_List*) _check_null( _temp2641))->tl, i ++){ struct
_tagged_string* _temp2644=( struct _tagged_string*)({ struct _tagged_string*
_temp2643=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp2643[ 0]= xprintf("f%d", i); _temp2643;}); goto _LL2645; _LL2645: { struct
Cyc_Absyn_Structfield* _temp2647=({ struct Cyc_Absyn_Structfield* _temp2646=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2646->name= _temp2644; _temp2646->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2641))->hd)).f1; _temp2646->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2641))->hd)).f2);
_temp2646->width= 0; _temp2646->attributes= 0; _temp2646;}); goto _LL2648;
_LL2648: _temp2639=({ struct Cyc_List_List* _temp2649=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2649->hd=( void*) _temp2647;
_temp2649->tl= _temp2639; _temp2649;});}}} _temp2639=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2639); _temp2639=({ struct Cyc_List_List*
_temp2650=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2650->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2651=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2651->name= Cyc_Toc_tag_sp;
_temp2651->tq= Cyc_Toc_mt_tq; _temp2651->type=( void*) Cyc_Absyn_sint_t;
_temp2651->width= 0; _temp2651->attributes= 0; _temp2651;}); _temp2650->tl=
_temp2639; _temp2650;});{ struct Cyc_Absyn_Structdecl* _temp2657=({ struct Cyc_Absyn_Structdecl*
_temp2652=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2652->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2652->name=({ struct Cyc_Core_Opt*
_temp2654=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2654->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2655=( char*)"_struct"; struct _tagged_string _temp2656; _temp2656.curr=
_temp2655; _temp2656.base= _temp2655; _temp2656.last_plus_one= _temp2655 + 8;
_temp2656;})); _temp2654;}); _temp2652->tvs= 0; _temp2652->fields=({ struct Cyc_Core_Opt*
_temp2653=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2653->v=( void*) _temp2639; _temp2653;}); _temp2652->attributes= 0;
_temp2652;}); goto _LL2658; _LL2658: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2659=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2659->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2660=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2660[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2661; _temp2661.tag= Cyc_Absyn_Struct_d;
_temp2661.f1= _temp2657; _temp2661;}); _temp2660;}), 0); _temp2659->tl= Cyc_Toc_result_decls;
_temp2659;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2662=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2662->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2662;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2663= xd->name; goto _LL2664; _LL2664: {
struct Cyc_List_List* _temp2665=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2666; _LL2666: for( 0; _temp2665 != 0;
_temp2665=(( struct Cyc_List_List*) _check_null( _temp2665))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2665))->hd; struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2668= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2667=* fn;(
unsigned int)( _temp2667.last_plus_one - _temp2667.curr);}) + 4, 0); goto
_LL2669; _LL2669: { struct Cyc_Absyn_ArrayType_struct* _temp2672=({ struct Cyc_Absyn_ArrayType_struct*
_temp2670=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2670[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2671; _temp2671.tag= Cyc_Absyn_ArrayType;
_temp2671.f1=( void*) Cyc_Absyn_uchar_t; _temp2671.f2= Cyc_Toc_mt_tq; _temp2671.f3=(
struct Cyc_Absyn_Exp*) _temp2668; _temp2671;}); _temp2670;}); goto _LL2673;
_LL2673: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) xd->sc !=( void*) Cyc_Absyn_Extern?
!(( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v,
f->name): 0){ initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(({ int
_temp2674= 0; int _temp2675= 0; int _temp2676= 0; int _temp2677= 0; struct
_tagged_string _temp2678=* fn; xprintf("%c%c%c%c%.*s", _temp2674, _temp2675,
_temp2676, _temp2677, _temp2678.last_plus_one - _temp2678.curr, _temp2678.curr);}),
0);}{ struct Cyc_Absyn_Vardecl* _temp2679= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2672, initopt); goto _LL2680; _LL2680:( void*)( _temp2679->sc=( void*)((
void*) xd->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2681=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2681->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2682=(
struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2682[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2683; _temp2683.tag= Cyc_Absyn_Var_d;
_temp2683.f1= _temp2679; _temp2683;}); _temp2682;}), 0); _temp2681->tl= Cyc_Toc_result_decls;
_temp2681;}); if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v,
f->name)){ continue;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name)); if( f->typs != 0){
struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List* _temp2684= f->typs;
goto _LL2685; _LL2685: for( 0; _temp2684 != 0; _temp2684=(( struct Cyc_List_List*)
_check_null( _temp2684))->tl, i ++){ struct _tagged_string* _temp2687=( struct
_tagged_string*)({ struct _tagged_string* _temp2686=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp2686[ 0]= xprintf("f%d", i);
_temp2686;}); goto _LL2688; _LL2688: { struct Cyc_Absyn_Structfield* _temp2690=({
struct Cyc_Absyn_Structfield* _temp2689=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2689->name= _temp2687;
_temp2689->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2684))->hd)).f1; _temp2689->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2684))->hd)).f2);
_temp2689->width= 0; _temp2689->attributes= 0; _temp2689;}); goto _LL2691;
_LL2691: fields=({ struct Cyc_List_List* _temp2692=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2692->hd=( void*) _temp2690;
_temp2692->tl= fields; _temp2692;});}}} fields=({ struct Cyc_List_List*
_temp2693=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2693->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2694=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2694->name= Cyc_Toc_tag_sp;
_temp2694->tq= Cyc_Toc_mt_tq; _temp2694->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2694->width= 0; _temp2694->attributes= 0; _temp2694;});
_temp2693->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2693;});{ struct Cyc_Absyn_Structdecl* _temp2700=({ struct Cyc_Absyn_Structdecl*
_temp2695=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2695->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2695->name=({ struct Cyc_Core_Opt*
_temp2697=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2697->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2698=( char*)"_struct"; struct _tagged_string _temp2699; _temp2699.curr=
_temp2698; _temp2699.base= _temp2698; _temp2699.last_plus_one= _temp2698 + 8;
_temp2699;})); _temp2697;}); _temp2695->tvs= 0; _temp2695->fields=({ struct Cyc_Core_Opt*
_temp2696=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2696->v=( void*) fields; _temp2696;}); _temp2695->attributes= 0; _temp2695;});
goto _LL2701; _LL2701: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2702=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2702->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2703=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2703[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2704; _temp2704.tag= Cyc_Absyn_Struct_d;
_temp2704.f1= _temp2700; _temp2704;}); _temp2703;}), 0); _temp2702->tl= Cyc_Toc_result_decls;
_temp2702;});}}}}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2705= ed->name; goto
_LL2706; _LL2706:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc)));{ struct Cyc_List_List*
_temp2707=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
goto _LL2708; _LL2708: for( 0; _temp2707 != 0; _temp2707=(( struct Cyc_List_List*)
_check_null( _temp2707))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2707))->hd)->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp2707))->hd)->tag)));(( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp2707))->hd)->tag=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_string* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_string* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();
struct Cyc_Absyn_Stmt* _temp2711; struct Cyc_List_List* _temp2713; struct Cyc_Toc_Env*
_temp2715; struct _tuple13 _temp2709= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL2716: _temp2715=
_temp2709.f1; goto _LL2714; _LL2714: _temp2713= _temp2709.f2; goto _LL2712;
_LL2712: _temp2711= _temp2709.f3; goto _LL2710; _LL2710: Cyc_Toc_stmt_to_c(
_temp2715, s);{ struct Cyc_Absyn_Stmt* _temp2717= Cyc_Absyn_label_stmt( succ_lab,
s, 0); goto _LL2718; _LL2718: if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2711, _temp2717, 0), 0);}
else{ struct Cyc_Absyn_Exp* _temp2720= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp2719=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2719->f1= Cyc_Toc_abs_ns; _temp2719->f2= Cyc_Toc_Match_Exception_sp;
_temp2719;}), 0)); goto _LL2721; _LL2721: { struct Cyc_Absyn_Stmt* _temp2722=
Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2720, 0), 0); goto
_LL2723; _LL2723: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2711, Cyc_Absyn_seq_stmt( _temp2722,
_temp2717, 0), 0), 0);}} for( 0; _temp2713 != 0; _temp2713=(( struct Cyc_List_List*)
_check_null( _temp2713))->tl){ struct _tuple14 _temp2726; void* _temp2727;
struct _tuple0* _temp2729; struct _tuple14* _temp2724=( struct _tuple14*)((
struct Cyc_List_List*) _check_null( _temp2713))->hd; _temp2726=* _temp2724;
_LL2730: _temp2729= _temp2726.f1; goto _LL2728; _LL2728: _temp2727= _temp2726.f2;
goto _LL2725; _LL2725: s= Cyc_Absyn_declare_stmt( _temp2729, _temp2727, 0, s, 0);}
return s;}}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2731=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp2755; struct Cyc_Absyn_Fndecl* _temp2757; int
_temp2759; struct Cyc_Absyn_Exp* _temp2761; struct Cyc_Core_Opt* _temp2763;
struct Cyc_Core_Opt* _temp2765; struct Cyc_Absyn_Pat* _temp2767; struct Cyc_Absyn_Structdecl*
_temp2769; struct Cyc_Absyn_Uniondecl* _temp2771; struct Cyc_Absyn_Tuniondecl*
_temp2773; struct Cyc_Absyn_Enumdecl* _temp2775; struct Cyc_Absyn_Typedefdecl*
_temp2777; struct Cyc_List_List* _temp2779; struct _tagged_string* _temp2781;
struct Cyc_List_List* _temp2783; struct _tuple0* _temp2785; struct Cyc_List_List*
_temp2787; _LL2733: if(*(( int*) _temp2731) == Cyc_Absyn_Var_d){ _LL2756:
_temp2755=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2731)->f1; goto _LL2734;} else{ goto _LL2735;} _LL2735: if(*(( int*)
_temp2731) == Cyc_Absyn_Fn_d){ _LL2758: _temp2757=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp2731)->f1; goto _LL2736;} else{ goto _LL2737;}
_LL2737: if(*(( int*) _temp2731) == Cyc_Absyn_Let_d){ _LL2768: _temp2767=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2731)->f1; goto
_LL2766; _LL2766: _temp2765=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2731)->f2; goto _LL2764; _LL2764: _temp2763=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2731)->f3; goto _LL2762; _LL2762: _temp2761=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2731)->f4; goto
_LL2760; _LL2760: _temp2759=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2731)->f5;
goto _LL2738;} else{ goto _LL2739;} _LL2739: if(*(( int*) _temp2731) == Cyc_Absyn_Struct_d){
_LL2770: _temp2769=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp2731)->f1; goto _LL2740;} else{ goto _LL2741;} _LL2741: if(*(( int*)
_temp2731) == Cyc_Absyn_Union_d){ _LL2772: _temp2771=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp2731)->f1; goto _LL2742;} else{ goto
_LL2743;} _LL2743: if(*(( int*) _temp2731) == Cyc_Absyn_Tunion_d){ _LL2774:
_temp2773=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2731)->f1; goto _LL2744;} else{ goto _LL2745;} _LL2745: if(*(( int*)
_temp2731) == Cyc_Absyn_Enum_d){ _LL2776: _temp2775=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp2731)->f1; goto _LL2746;} else{ goto
_LL2747;} _LL2747: if(*(( int*) _temp2731) == Cyc_Absyn_Typedef_d){ _LL2778:
_temp2777=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2731)->f1; goto _LL2748;} else{ goto _LL2749;} _LL2749: if(*(( int*)
_temp2731) == Cyc_Absyn_Namespace_d){ _LL2782: _temp2781=( struct _tagged_string*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2731)->f1; goto _LL2780; _LL2780:
_temp2779=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2731)->f2; goto _LL2750;} else{ goto _LL2751;} _LL2751: if(*(( int*)
_temp2731) == Cyc_Absyn_Using_d){ _LL2786: _temp2785=( struct _tuple0*)(( struct
Cyc_Absyn_Using_d_struct*) _temp2731)->f1; goto _LL2784; _LL2784: _temp2783=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp2731)->f2; goto
_LL2752;} else{ goto _LL2753;} _LL2753: if(*(( int*) _temp2731) == Cyc_Absyn_ExternC_d){
_LL2788: _temp2787=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2731)->f1; goto _LL2754;} else{ goto _LL2732;} _LL2734: { struct _tuple0*
_temp2789= _temp2755->name; goto _LL2790; _LL2790: if(( void*) _temp2755->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2789=({ struct _tuple0* _temp2791=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2791->f1= Cyc_Toc_abs_ns;
_temp2791->f2=(* _temp2789).f2; _temp2791;});} if( _temp2755->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp2755->initializer)));} if( ! nv->toplevel){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2792=(
char*)"decls_to_c: not at toplevel!"; struct _tagged_string _temp2793; _temp2793.curr=
_temp2792; _temp2793.base= _temp2792; _temp2793.last_plus_one= _temp2792 + 29;
_temp2793;}));} nv= Cyc_Toc_add_varmap( nv, _temp2755->name, Cyc_Absyn_varb_exp(
_temp2789,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp2794=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp2794[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp2795; _temp2795.tag= Cyc_Absyn_Global_b;
_temp2795.f1= _temp2755; _temp2795;}); _temp2794;}), 0)); _temp2755->name=
_temp2789;( void*)( _temp2755->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2755->sc));(
void*)( _temp2755->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2755->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2796=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2796->hd=( void*) d; _temp2796->tl=
Cyc_Toc_result_decls; _temp2796;}); goto _LL2732;} _LL2736: { struct _tuple0*
_temp2797= _temp2757->name; goto _LL2798; _LL2798: if(( void*) _temp2757->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2797=({ struct _tuple0* _temp2799=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2799->f1= Cyc_Toc_abs_ns;
_temp2799->f2=(* _temp2797).f2; _temp2799;});} nv= Cyc_Toc_add_varmap( nv,
_temp2757->name, Cyc_Absyn_var_exp( _temp2797, 0)); _temp2757->name= _temp2797;
Cyc_Toc_fndecl_to_c( nv, _temp2757); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2800=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2800->hd=( void*) d; _temp2800->tl= Cyc_Toc_result_decls; _temp2800;});
goto _LL2732;} _LL2738:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2801=( char*)"letdecl"; struct
_tagged_string _temp2802; _temp2802.curr= _temp2801; _temp2802.base= _temp2801;
_temp2802.last_plus_one= _temp2801 + 8; _temp2802;})); goto _LL2732; _LL2740:
Cyc_Toc_structdecl_to_c( _temp2769); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2803=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2803->hd=( void*) d; _temp2803->tl= Cyc_Toc_result_decls; _temp2803;});
goto _LL2732; _LL2742: Cyc_Toc_uniondecl_to_c( _temp2771); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2804=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2804->hd=( void*) d; _temp2804->tl= Cyc_Toc_result_decls;
_temp2804;}); goto _LL2732; _LL2744: if( _temp2773->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2773);} else{ Cyc_Toc_tuniondecl_to_c( _temp2773);} goto _LL2732; _LL2746:
Cyc_Toc_enumdecl_to_c( nv, _temp2775); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2805=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2805->hd=( void*) d; _temp2805->tl= Cyc_Toc_result_decls; _temp2805;});
goto _LL2732; _LL2748: _temp2777->name= _temp2777->name; _temp2777->tvs= 0;(
void*)( _temp2777->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2777->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2806=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2806->hd=( void*) d; _temp2806->tl=
Cyc_Toc_result_decls; _temp2806;}); goto _LL2732; _LL2750: _temp2783= _temp2779;
goto _LL2752; _LL2752: _temp2787= _temp2783; goto _LL2754; _LL2754: nv= Cyc_Toc_decls_to_c(
nv, _temp2787, top); goto _LL2732; _LL2732:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}