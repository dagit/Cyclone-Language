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
Cyc_Toc__npop_handler_pr; static char _temp50[ 16u]="Match_Exception"; static
struct _tagged_string Cyc_Toc_Match_Exception_string=( struct _tagged_string){
_temp50, _temp50, _temp50 + 16u}; static struct _tagged_string* Cyc_Toc_Match_Exception_sp=&
Cyc_Toc_Match_Exception_string; static char _temp53[ 15u]="Cyc_new_string";
static struct _tagged_string Cyc_Toc_Cyc_new_string_string=( struct
_tagged_string){ _temp53, _temp53, _temp53 + 15u}; static struct _tagged_string*
Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string; static char _temp56[
16u]="Cyc_rnew_string"; static struct _tagged_string Cyc_Toc_Cyc_rnew_string_string=(
struct _tagged_string){ _temp56, _temp56, _temp56 + 16u}; static struct
_tagged_string* Cyc_Toc_Cyc_rnew_string_sp=& Cyc_Toc_Cyc_rnew_string_string;
static char _temp59[ 14u]="_RegionHandle"; static struct _tagged_string Cyc_Toc__region_handle_string=(
struct _tagged_string){ _temp59, _temp59, _temp59 + 14u}; static struct
_tagged_string* Cyc_Toc__region_handle_sp=& Cyc_Toc__region_handle_string;
static char _temp62[ 12u]="_new_region"; static struct _tagged_string Cyc_Toc__new_region_string=(
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
struct Cyc_List_List* _temp135; struct Cyc_Core_Opt* _temp137; _LL65: if(*(( int*)
_temp63) == Cyc_Absyn_Const_e){ _LL90: _temp89=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp63)->f1; if(( unsigned int) _temp89 > 1u?*(( int*) _temp89) == Cyc_Absyn_Char_c:
0){ _LL94: _temp93=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp89)->f1;
goto _LL92; _LL92: _temp91=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp89)->f2;
goto _LL66;} else{ goto _LL67;}} else{ goto _LL67;} _LL67: if(*(( int*) _temp63)
== Cyc_Absyn_Const_e){ _LL96: _temp95=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp63)->f1; if(( unsigned int) _temp95 > 1u?*(( int*) _temp95) == Cyc_Absyn_Short_c:
0){ _LL100: _temp99=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp95)->f1;
goto _LL98; _LL98: _temp97=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp95)->f2;
goto _LL68;} else{ goto _LL69;}} else{ goto _LL69;} _LL69: if(*(( int*) _temp63)
== Cyc_Absyn_Const_e){ _LL102: _temp101=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp63)->f1; if(( unsigned int) _temp101 > 1u?*(( int*) _temp101) == Cyc_Absyn_Int_c:
0){ _LL106: _temp105=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp101)->f1;
goto _LL104; _LL104: _temp103=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp101)->f2;
goto _LL70;} else{ goto _LL71;}} else{ goto _LL71;} _LL71: if(*(( int*) _temp63)
== Cyc_Absyn_Const_e){ _LL108: _temp107=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp63)->f1; if(( unsigned int) _temp107 > 1u?*(( int*) _temp107) == Cyc_Absyn_LongLong_c:
0){ _LL112: _temp111=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp107)->f1;
goto _LL110; _LL110: _temp109=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp107)->f2; goto _LL72;} else{ goto _LL73;}} else{ goto _LL73;} _LL73: if(*((
int*) _temp63) == Cyc_Absyn_Const_e){ _LL114: _temp113=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp63)->f1; if( _temp113 ==( void*) Cyc_Absyn_Null_c){ goto _LL74;} else{ goto
_LL75;}} else{ goto _LL75;} _LL75: if(*(( int*) _temp63) == Cyc_Absyn_Cast_e){
_LL118: _temp117=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp63)->f1; goto
_LL116; _LL116: _temp115=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp63)->f2; goto _LL76;} else{ goto _LL77;} _LL77: if(*(( int*) _temp63) ==
Cyc_Absyn_Tuple_e){ _LL120: _temp119=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp63)->f1; goto _LL78;} else{ goto _LL79;} _LL79: if(*(( int*) _temp63) ==
Cyc_Absyn_Array_e){ _LL122: _temp121=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp63)->f1; goto _LL80;} else{ goto _LL81;} _LL81: if(*(( int*) _temp63) ==
Cyc_Absyn_Struct_e){ _LL130: _temp129=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp63)->f1; goto _LL128; _LL128: _temp127=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp63)->f2; goto _LL126; _LL126: _temp125=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp63)->f3; goto _LL124; _LL124: _temp123=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp63)->f4; goto
_LL82;} else{ goto _LL83;} _LL83: if(*(( int*) _temp63) == Cyc_Absyn_CompoundLit_e){
_LL134: _temp133=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp63)->f1; goto _LL132; _LL132: _temp131=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp63)->f2; goto _LL84;} else{ goto _LL85;}
_LL85: if(*(( int*) _temp63) == Cyc_Absyn_UnresolvedMem_e){ _LL138: _temp137=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp63)->f1;
goto _LL136; _LL136: _temp135=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp63)->f2; goto _LL86;} else{ goto _LL87;} _LL87: goto _LL88; _LL66: return
_temp91 =='\000'; _LL68: return _temp97 == 0; _LL70: return _temp103 == 0; _LL72:
return _temp109 == 0; _LL74: return 1; _LL76: return Cyc_Toc_is_zero( _temp115);
_LL78: return(( int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_forall)( Cyc_Toc_is_zero, _temp119); _LL80: _temp125= _temp121;
goto _LL82; _LL82: _temp131= _temp125; goto _LL84; _LL84: _temp135= _temp131;
goto _LL86; _LL86: for( 0; _temp135 != 0; _temp135=({ struct Cyc_List_List*
_temp139= _temp135; if( _temp139 == 0){ _throw( Null_Exception);} _temp139->tl;})){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)({ struct Cyc_List_List* _temp140=
_temp135; if( _temp140 == 0){ _throw( Null_Exception);} _temp140->hd;}))).f2)){
return 0;}} return 1; _LL88: return 0; _LL64:;} static int Cyc_Toc_is_nullable(
void* t){ void* _temp141= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp147; struct Cyc_Absyn_Conref* _temp149; struct Cyc_Absyn_Tqual _temp151;
struct Cyc_Absyn_Conref* _temp153; void* _temp155; void* _temp157; _LL143: if((
unsigned int) _temp141 > 4u?*(( int*) _temp141) == Cyc_Absyn_PointerType: 0){
_LL148: _temp147=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp141)->f1; _LL158: _temp157=( void*) _temp147.elt_typ; goto _LL156; _LL156:
_temp155=( void*) _temp147.rgn_typ; goto _LL154; _LL154: _temp153=( struct Cyc_Absyn_Conref*)
_temp147.nullable; goto _LL152; _LL152: _temp151=( struct Cyc_Absyn_Tqual)
_temp147.tq; goto _LL150; _LL150: _temp149=( struct Cyc_Absyn_Conref*) _temp147.bounds;
goto _LL144;} else{ goto _LL145;} _LL145: goto _LL146; _LL144: { void* _temp159=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp153))->v; int _temp165; _LL161: if(( unsigned int) _temp159 > 1u?*(( int*)
_temp159) == Cyc_Absyn_Eq_constr: 0){ _LL166: _temp165=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp159)->f1; goto _LL162;} else{ goto _LL163;} _LL163: goto _LL164; _LL162:
return _temp165; _LL164: return 0; _LL160:;} _LL146:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp167=(
char*)"is_nullable"; struct _tagged_string _temp168; _temp168.curr= _temp167;
_temp168.base= _temp167; _temp168.last_plus_one= _temp167 + 12; _temp168;}));
return 0; _LL142:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct
_tuple0* x, struct _tagged_string tag){ return({ struct _tuple0* _temp169=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp169->f1=(* x).f1;
_temp169->f2=({ struct _tagged_string* _temp170=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp170[ 0]= Cyc_String_strconcat(*(*
x).f2, tag); _temp170;}); _temp169;});} static void Cyc_Toc_add_tuple_decl(
struct _tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List* fs= 0;{
int i= 1; for( 0; ts != 0; ts=({ struct Cyc_List_List* _temp171= ts; if(
_temp171 == 0){ _throw( Null_Exception);} _temp171->tl;}), i ++){ fs=({ struct
Cyc_List_List* _temp172=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
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
_temp220; _LL214: if(( unsigned int) _temp212 > 4u?*(( int*) _temp212) == Cyc_Absyn_IntType:
0){ _LL221: _temp220=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp212)->f1;
if( _temp220 ==( void*) Cyc_Absyn_Unsigned){ goto _LL219;} else{ goto _LL216;}
_LL219: _temp218=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp212)->f2; if(
_temp218 ==( void*) Cyc_Absyn_B1){ goto _LL215;} else{ goto _LL216;}} else{ goto
_LL216;} _LL216: goto _LL217; _LL215: return Cyc_Toc__tagged_string_sp; _LL217:
goto _LL213; _LL213:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
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
4u?*(( int*) _temp253) == Cyc_Absyn_ArrayType: 0){ _LL266: _temp265=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp253)->f1; goto _LL264; _LL264: _temp263=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp253)->f2;
goto _LL262; _LL262: _temp261=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp253)->f3; goto _LL256;} else{ goto _LL257;} _LL257: if(( unsigned int)
_temp253 > 4u?*(( int*) _temp253) == Cyc_Absyn_Evar: 0){ _LL275: _temp274=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp253)->f1; goto _LL270; _LL270: _temp269=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp253)->f2; if(
_temp269 == 0){ goto _LL259;} else{ _temp271=* _temp269; _LL273: _temp272=( void*)
_temp271.v; goto _LL268;} _LL268: _temp267=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp253)->f3; goto _LL258;} else{ goto _LL259;} _LL259: goto _LL260; _LL256:
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp276=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp276[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp277; _temp277.tag= Cyc_Absyn_ArrayType; _temp277.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp265); _temp277.f2= _temp263; _temp277.f3=
_temp261; _temp277;}); _temp276;}); _LL258: return Cyc_Toc_typ_to_c_array(
_temp272); _LL260: return Cyc_Toc_typ_to_c( t); _LL254:;} static void* Cyc_Toc_typ_to_c(
void* t); static struct Cyc_Absyn_Structfield* Cyc_Toc_structfield_to_c( struct
Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield* _temp278=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp278->name= f->name; _temp278->tq= f->tq; _temp278->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp278->width= f->width; _temp278->attributes= f->attributes;
_temp278;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp279= t; int
_temp331; struct Cyc_Core_Opt* _temp333; void* _temp335; int _temp337; struct
Cyc_Core_Opt* _temp339; struct Cyc_Core_Opt _temp341; void* _temp342; void*
_temp344; struct Cyc_Absyn_Tvar* _temp346; struct Cyc_Absyn_TunionInfo _temp348;
void* _temp350; struct Cyc_List_List* _temp352; void* _temp354; struct Cyc_Absyn_Tuniondecl*
_temp356; struct Cyc_Absyn_TunionInfo _temp358; struct Cyc_Absyn_TunionFieldInfo
_temp360; struct Cyc_List_List* _temp362; void* _temp364; struct Cyc_Absyn_Tunionfield*
_temp366; struct Cyc_Absyn_Tuniondecl* _temp368; struct Cyc_Absyn_TunionFieldInfo
_temp370; struct Cyc_Absyn_PtrInfo _temp372; struct Cyc_Absyn_Conref* _temp374;
struct Cyc_Absyn_Tqual _temp376; struct Cyc_Absyn_Conref* _temp378; void*
_temp380; void* _temp382; void* _temp384; void* _temp386; struct Cyc_Absyn_Exp*
_temp388; struct Cyc_Absyn_Tqual _temp390; void* _temp392; struct Cyc_Absyn_FnInfo
_temp394; struct Cyc_List_List* _temp396; int _temp398; struct Cyc_List_List*
_temp400; void* _temp402; struct Cyc_Core_Opt* _temp404; struct Cyc_List_List*
_temp406; struct Cyc_List_List* _temp408; struct Cyc_List_List* _temp410; struct
Cyc_List_List* _temp412; struct Cyc_Absyn_Uniondecl** _temp414; struct Cyc_List_List*
_temp416; struct _tuple0* _temp418; struct Cyc_Absyn_Structdecl** _temp420;
struct Cyc_List_List* _temp422; struct _tuple0* _temp424; struct Cyc_Absyn_Enumdecl*
_temp426; struct _tuple0* _temp428; struct Cyc_Core_Opt* _temp430; struct Cyc_List_List*
_temp432; struct _tuple0* _temp434; void* _temp436; void* _temp438; struct Cyc_List_List*
_temp440; _LL281: if( _temp279 ==( void*) Cyc_Absyn_VoidType){ goto _LL282;}
else{ goto _LL283;} _LL283: if(( unsigned int) _temp279 > 4u?*(( int*) _temp279)
== Cyc_Absyn_Evar: 0){ _LL336: _temp335=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp279)->f1; goto _LL334; _LL334: _temp333=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp279)->f2; if( _temp333 == 0){ goto _LL332;} else{
goto _LL285;} _LL332: _temp331=( int)(( struct Cyc_Absyn_Evar_struct*) _temp279)->f3;
goto _LL284;} else{ goto _LL285;} _LL285: if(( unsigned int) _temp279 > 4u?*((
int*) _temp279) == Cyc_Absyn_Evar: 0){ _LL345: _temp344=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp279)->f1; goto _LL340; _LL340: _temp339=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp279)->f2; if( _temp339 == 0){ goto _LL287;} else{
_temp341=* _temp339; _LL343: _temp342=( void*) _temp341.v; goto _LL338;} _LL338:
_temp337=( int)(( struct Cyc_Absyn_Evar_struct*) _temp279)->f3; goto _LL286;}
else{ goto _LL287;} _LL287: if(( unsigned int) _temp279 > 4u?*(( int*) _temp279)
== Cyc_Absyn_VarType: 0){ _LL347: _temp346=( struct Cyc_Absyn_Tvar*)(( struct
Cyc_Absyn_VarType_struct*) _temp279)->f1; goto _LL288;} else{ goto _LL289;}
_LL289: if(( unsigned int) _temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_TunionType:
0){ _LL349: _temp348=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp279)->f1; _LL355: _temp354=( void*) _temp348.tunion_info; if(*(( int*)
_temp354) == Cyc_Absyn_KnownTunion){ _LL357: _temp356=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp354)->f1; goto _LL353;} else{ goto
_LL291;} _LL353: _temp352=( struct Cyc_List_List*) _temp348.targs; goto _LL351;
_LL351: _temp350=( void*) _temp348.rgn; goto _LL290;} else{ goto _LL291;} _LL291:
if(( unsigned int) _temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_TunionType: 0){
_LL359: _temp358=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp279)->f1; goto _LL292;} else{ goto _LL293;} _LL293: if(( unsigned int)
_temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_TunionFieldType: 0){ _LL361:
_temp360=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp279)->f1; _LL365: _temp364=( void*) _temp360.field_info; if(*(( int*)
_temp364) == Cyc_Absyn_KnownTunionfield){ _LL369: _temp368=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp364)->f1; goto _LL367; _LL367:
_temp366=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp364)->f2; goto _LL363;} else{ goto _LL295;} _LL363: _temp362=( struct Cyc_List_List*)
_temp360.targs; goto _LL294;} else{ goto _LL295;} _LL295: if(( unsigned int)
_temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_TunionFieldType: 0){ _LL371:
_temp370=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp279)->f1; goto _LL296;} else{ goto _LL297;} _LL297: if(( unsigned int)
_temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_PointerType: 0){ _LL373: _temp372=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp279)->f1;
_LL383: _temp382=( void*) _temp372.elt_typ; goto _LL381; _LL381: _temp380=( void*)
_temp372.rgn_typ; goto _LL379; _LL379: _temp378=( struct Cyc_Absyn_Conref*)
_temp372.nullable; goto _LL377; _LL377: _temp376=( struct Cyc_Absyn_Tqual)
_temp372.tq; goto _LL375; _LL375: _temp374=( struct Cyc_Absyn_Conref*) _temp372.bounds;
goto _LL298;} else{ goto _LL299;} _LL299: if(( unsigned int) _temp279 > 4u?*((
int*) _temp279) == Cyc_Absyn_IntType: 0){ _LL387: _temp386=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp279)->f1; goto _LL385; _LL385: _temp384=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp279)->f2; goto _LL300;} else{ goto _LL301;} _LL301: if( _temp279 ==( void*)
Cyc_Absyn_FloatType){ goto _LL302;} else{ goto _LL303;} _LL303: if( _temp279 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL304;} else{ goto _LL305;} _LL305: if((
unsigned int) _temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_ArrayType: 0){
_LL393: _temp392=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp279)->f1;
goto _LL391; _LL391: _temp390=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp279)->f2; goto _LL389; _LL389: _temp388=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp279)->f3; goto _LL306;} else{ goto _LL307;}
_LL307: if(( unsigned int) _temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_FnType:
0){ _LL395: _temp394=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp279)->f1; _LL407: _temp406=( struct Cyc_List_List*) _temp394.tvars; goto
_LL405; _LL405: _temp404=( struct Cyc_Core_Opt*) _temp394.effect; goto _LL403;
_LL403: _temp402=( void*) _temp394.ret_typ; goto _LL401; _LL401: _temp400=(
struct Cyc_List_List*) _temp394.args; goto _LL399; _LL399: _temp398=( int)
_temp394.varargs; goto _LL397; _LL397: _temp396=( struct Cyc_List_List*)
_temp394.attributes; goto _LL308;} else{ goto _LL309;} _LL309: if(( unsigned int)
_temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_TupleType: 0){ _LL409: _temp408=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp279)->f1; goto
_LL310;} else{ goto _LL311;} _LL311: if(( unsigned int) _temp279 > 4u?*(( int*)
_temp279) == Cyc_Absyn_AnonStructType: 0){ _LL411: _temp410=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp279)->f1; goto _LL312;} else{ goto
_LL313;} _LL313: if(( unsigned int) _temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_AnonUnionType:
0){ _LL413: _temp412=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp279)->f1; goto _LL314;} else{ goto _LL315;} _LL315: if(( unsigned int)
_temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_UnionType: 0){ _LL419: _temp418=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp279)->f1; goto
_LL417; _LL417: _temp416=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp279)->f2; goto _LL415; _LL415: _temp414=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp279)->f3; goto _LL316;} else{ goto
_LL317;} _LL317: if(( unsigned int) _temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_StructType:
0){ _LL425: _temp424=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp279)->f1; goto _LL423; _LL423: _temp422=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp279)->f2; goto _LL421; _LL421: _temp420=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp279)->f3;
goto _LL318;} else{ goto _LL319;} _LL319: if(( unsigned int) _temp279 > 4u?*((
int*) _temp279) == Cyc_Absyn_EnumType: 0){ _LL429: _temp428=( struct _tuple0*)((
struct Cyc_Absyn_EnumType_struct*) _temp279)->f1; goto _LL427; _LL427: _temp426=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp279)->f2;
goto _LL320;} else{ goto _LL321;} _LL321: if(( unsigned int) _temp279 > 4u?*((
int*) _temp279) == Cyc_Absyn_TypedefType: 0){ _LL435: _temp434=( struct _tuple0*)((
struct Cyc_Absyn_TypedefType_struct*) _temp279)->f1; goto _LL433; _LL433:
_temp432=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp279)->f2; goto _LL431; _LL431: _temp430=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp279)->f3; goto _LL322;} else{ goto _LL323;}
_LL323: if(( unsigned int) _temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_RgnHandleType:
0){ _LL437: _temp436=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp279)->f1;
goto _LL324;} else{ goto _LL325;} _LL325: if( _temp279 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL326;} else{ goto _LL327;} _LL327: if(( unsigned int) _temp279 > 4u?*((
int*) _temp279) == Cyc_Absyn_AccessEff: 0){ _LL439: _temp438=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp279)->f1; goto _LL328;} else{ goto _LL329;}
_LL329: if(( unsigned int) _temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_JoinEff:
0){ _LL441: _temp440=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp279)->f1; goto _LL330;} else{ goto _LL280;} _LL282: return t; _LL284:
return( void*) Cyc_Absyn_VoidType; _LL286: return Cyc_Toc_typ_to_c( _temp342);
_LL288: return Cyc_Absyn_void_star_typ(); _LL290: return Cyc_Absyn_void_star_typ();
_LL292:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp442=( char*)"unresolved TunionType"; struct
_tagged_string _temp443; _temp443.curr= _temp442; _temp443.base= _temp442;
_temp443.last_plus_one= _temp442 + 22; _temp443;}))); _LL294: if( _temp366->typs
== 0){ if( _temp368->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp366->name,(
struct _tagged_string)({ char* _temp444=( char*)"_struct"; struct _tagged_string
_temp445; _temp445.curr= _temp444; _temp445.base= _temp444; _temp445.last_plus_one=
_temp444 + 8; _temp445;})));} _LL296:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp446=(
char*)"unresolved TunionFieldType"; struct _tagged_string _temp447; _temp447.curr=
_temp446; _temp447.base= _temp446; _temp447.last_plus_one= _temp446 + 27;
_temp447;}))); _LL298: _temp382= Cyc_Toc_typ_to_c_array( _temp382);{ void*
_temp448=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp374))->v; void* _temp454; _LL450: if((
unsigned int) _temp448 > 1u?*(( int*) _temp448) == Cyc_Absyn_Eq_constr: 0){
_LL455: _temp454=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp448)->f1;
if( _temp454 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL451;} else{ goto _LL452;}}
else{ goto _LL452;} _LL452: goto _LL453; _LL451: { struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
_temp382, _temp376); return Cyc_Absyn_strct( n);} _LL453: return Cyc_Absyn_star_typ(
_temp382,( void*) Cyc_Absyn_HeapRgn, _temp376); _LL449:;} _LL300: return t;
_LL302: return t; _LL304: return t; _LL306: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp456=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp456[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp457; _temp457.tag= Cyc_Absyn_ArrayType;
_temp457.f1=( void*) Cyc_Toc_typ_to_c_array( _temp392); _temp457.f2= _temp390;
_temp457.f3= _temp388; _temp457;}); _temp456;}); _LL308: { struct Cyc_List_List*
new_atts= 0; for( 0; _temp396 != 0; _temp396=({ struct Cyc_List_List* _temp458=
_temp396; if( _temp458 == 0){ _throw( Null_Exception);} _temp458->tl;})){ void*
_temp460=( void*)({ struct Cyc_List_List* _temp459= _temp396; if( _temp459 == 0){
_throw( Null_Exception);} _temp459->hd;}); _LL462: if( _temp460 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL463;} else{ goto _LL464;} _LL464: if( _temp460 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL465;} else{ goto _LL466;} _LL466: if( _temp460 ==( void*) Cyc_Absyn_Const_att){
goto _LL467;} else{ goto _LL468;} _LL468: goto _LL469; _LL463: goto _LL465;
_LL465: goto _LL467; _LL467: continue; _LL469: new_atts=({ struct Cyc_List_List*
_temp470=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp470->hd=( void*)(( void*)({ struct Cyc_List_List* _temp471= _temp396; if(
_temp471 == 0){ _throw( Null_Exception);} _temp471->hd;})); _temp470->tl=
new_atts; _temp470;}); goto _LL461; _LL461:;} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp472=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp472[ 0]=({ struct Cyc_Absyn_FnType_struct _temp473; _temp473.tag= Cyc_Absyn_FnType;
_temp473.f1=({ struct Cyc_Absyn_FnInfo _temp474; _temp474.tvars= 0; _temp474.effect=
0; _temp474.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp402); _temp474.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp400); _temp474.varargs= _temp398;
_temp474.attributes= new_atts; _temp474;}); _temp473;}); _temp472;});} _LL310:
_temp408=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp408);{ struct
_tagged_string* n= Cyc_Toc_add_tuple_type( _temp408); return Cyc_Absyn_strct( n);}
_LL312: return( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp475=(
struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp475[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp476; _temp476.tag=
Cyc_Absyn_AnonStructType; _temp476.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp410); _temp476;}); _temp475;}); _LL314: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp477=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp477[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp478; _temp478.tag=
Cyc_Absyn_AnonUnionType; _temp478.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp412); _temp478;}); _temp477;}); _LL316: if( _temp418 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp479=( char*)"anonymous union"; struct _tagged_string _temp480; _temp480.curr=
_temp479; _temp480.base= _temp479; _temp480.last_plus_one= _temp479 + 16;
_temp480;}));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)({ struct _tuple0*
_temp481= _temp418; if( _temp481 == 0){ _throw( Null_Exception);} _temp481;}));
_LL318: if( _temp424 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp482=( char*)"anonymous struct"; struct
_tagged_string _temp483; _temp483.curr= _temp482; _temp483.base= _temp482;
_temp483.last_plus_one= _temp482 + 17; _temp483;}));} return Cyc_Absyn_strctq((
struct _tuple0*)({ struct _tuple0* _temp484= _temp424; if( _temp484 == 0){
_throw( Null_Exception);} _temp484;})); _LL320: return t; _LL322: if( _temp430
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp485=( char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp486; _temp486.curr= _temp485; _temp486.base= _temp485;
_temp486.last_plus_one= _temp485 + 38; _temp486;}));} return( void*)({ struct
Cyc_Absyn_TypedefType_struct* _temp487=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp487[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp488; _temp488.tag= Cyc_Absyn_TypedefType;
_temp488.f1= _temp434; _temp488.f2= 0; _temp488.f3=({ struct Cyc_Core_Opt*
_temp489=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp489->v=( void*) Cyc_Toc_typ_to_c_array(( void*)({ struct Cyc_Core_Opt*
_temp490= _temp430; if( _temp490 == 0){ _throw( Null_Exception);} _temp490->v;}));
_temp489;}); _temp488;}); _temp487;}); _LL324: return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__region_handle_sp), Cyc_Toc_mt_tq); _LL326: return(( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp491=(
char*)"Toc::typ_to_c: type translation passed the heap region"; struct
_tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 55; _temp492;})); _LL328: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp493=( char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp494; _temp494.curr= _temp493; _temp494.base= _temp493;
_temp494.last_plus_one= _temp493 + 54; _temp494;})); _LL330: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp495=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp496; _temp496.curr= _temp495; _temp496.base= _temp495;
_temp496.last_plus_one= _temp495 + 52; _temp496;})); _LL280:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp497= t; struct Cyc_Absyn_Exp* _temp503; struct Cyc_Absyn_Tqual
_temp505; void* _temp507; _LL499: if(( unsigned int) _temp497 > 4u?*(( int*)
_temp497) == Cyc_Absyn_ArrayType: 0){ _LL508: _temp507=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp497)->f1; goto _LL506; _LL506: _temp505=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp497)->f2; goto _LL504; _LL504: _temp503=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp497)->f3; goto
_LL500;} else{ goto _LL501;} _LL501: goto _LL502; _LL500: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp507,( void*) Cyc_Absyn_HeapRgn, _temp505), e, l);
_LL502: return e; _LL498:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp509= t; struct Cyc_Absyn_Exp* _temp515; struct Cyc_Absyn_Tqual _temp517;
void* _temp519; _LL511: if(( unsigned int) _temp509 > 4u?*(( int*) _temp509) ==
Cyc_Absyn_ArrayType: 0){ _LL520: _temp519=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp509)->f1; goto _LL518; _LL518: _temp517=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp509)->f2; goto _LL516; _LL516: _temp515=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp509)->f3; goto
_LL512;} else{ goto _LL513;} _LL513: goto _LL514; _LL512: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp519,( void*) Cyc_Absyn_HeapRgn, _temp517), e, l);
_LL514: return Cyc_Absyn_cast_exp( t, e, l); _LL510:;} static int Cyc_Toc_atomic_typ(
void* t){ void* _temp521= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp557; void* _temp559; void* _temp561; struct Cyc_Absyn_Exp* _temp563; struct
Cyc_Absyn_Tqual _temp565; void* _temp567; struct Cyc_Absyn_FnInfo _temp569;
struct Cyc_List_List* _temp571; struct Cyc_Absyn_Structdecl** _temp573; struct
Cyc_List_List* _temp575; struct _tuple0* _temp577; struct Cyc_Absyn_TunionInfo
_temp579; struct Cyc_List_List* _temp581; struct Cyc_List_List* _temp583; struct
Cyc_Absyn_TunionFieldInfo _temp585; struct Cyc_List_List* _temp587; void*
_temp589; struct Cyc_Absyn_Tunionfield* _temp591; struct Cyc_Absyn_Tuniondecl*
_temp593; struct Cyc_Absyn_Uniondecl** _temp595; struct Cyc_List_List* _temp597;
struct _tuple0* _temp599; struct Cyc_Absyn_PtrInfo _temp601; void* _temp603;
_LL523: if( _temp521 ==( void*) Cyc_Absyn_VoidType){ goto _LL524;} else{ goto
_LL525;} _LL525: if(( unsigned int) _temp521 > 4u?*(( int*) _temp521) == Cyc_Absyn_VarType:
0){ _LL558: _temp557=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp521)->f1; goto _LL526;} else{ goto _LL527;} _LL527: if(( unsigned int)
_temp521 > 4u?*(( int*) _temp521) == Cyc_Absyn_IntType: 0){ _LL562: _temp561=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp521)->f1; goto _LL560; _LL560:
_temp559=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp521)->f2; goto _LL528;}
else{ goto _LL529;} _LL529: if( _temp521 ==( void*) Cyc_Absyn_FloatType){ goto
_LL530;} else{ goto _LL531;} _LL531: if( _temp521 ==( void*) Cyc_Absyn_DoubleType){
goto _LL532;} else{ goto _LL533;} _LL533: if(( unsigned int) _temp521 > 4u?*((
int*) _temp521) == Cyc_Absyn_ArrayType: 0){ _LL568: _temp567=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp521)->f1; goto _LL566; _LL566: _temp565=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp521)->f2;
goto _LL564; _LL564: _temp563=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp521)->f3; goto _LL534;} else{ goto _LL535;} _LL535: if(( unsigned int)
_temp521 > 4u?*(( int*) _temp521) == Cyc_Absyn_FnType: 0){ _LL570: _temp569=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp521)->f1; goto
_LL536;} else{ goto _LL537;} _LL537: if(( unsigned int) _temp521 > 4u?*(( int*)
_temp521) == Cyc_Absyn_TupleType: 0){ _LL572: _temp571=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp521)->f1; goto _LL538;} else{ goto
_LL539;} _LL539: if(( unsigned int) _temp521 > 4u?*(( int*) _temp521) == Cyc_Absyn_StructType:
0){ _LL578: _temp577=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp521)->f1; goto _LL576; _LL576: _temp575=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp521)->f2; goto _LL574; _LL574: _temp573=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp521)->f3;
goto _LL540;} else{ goto _LL541;} _LL541: if(( unsigned int) _temp521 > 4u?*((
int*) _temp521) == Cyc_Absyn_TunionType: 0){ _LL580: _temp579=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp521)->f1; goto _LL542;} else{ goto
_LL543;} _LL543: if(( unsigned int) _temp521 > 4u?*(( int*) _temp521) == Cyc_Absyn_AnonStructType:
0){ _LL582: _temp581=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp521)->f1; goto _LL544;} else{ goto _LL545;} _LL545: if(( unsigned int)
_temp521 > 4u?*(( int*) _temp521) == Cyc_Absyn_AnonUnionType: 0){ _LL584:
_temp583=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp521)->f1; goto _LL546;} else{ goto _LL547;} _LL547: if(( unsigned int)
_temp521 > 4u?*(( int*) _temp521) == Cyc_Absyn_TunionFieldType: 0){ _LL586:
_temp585=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp521)->f1; _LL590: _temp589=( void*) _temp585.field_info; if(*(( int*)
_temp589) == Cyc_Absyn_KnownTunionfield){ _LL594: _temp593=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp589)->f1; goto _LL592; _LL592:
_temp591=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp589)->f2; goto _LL588;} else{ goto _LL549;} _LL588: _temp587=( struct Cyc_List_List*)
_temp585.targs; goto _LL548;} else{ goto _LL549;} _LL549: if(( unsigned int)
_temp521 > 4u?*(( int*) _temp521) == Cyc_Absyn_UnionType: 0){ _LL600: _temp599=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp521)->f1; goto
_LL598; _LL598: _temp597=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp521)->f2; goto _LL596; _LL596: _temp595=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp521)->f3; goto _LL550;} else{ goto
_LL551;} _LL551: if(( unsigned int) _temp521 > 4u?*(( int*) _temp521) == Cyc_Absyn_PointerType:
0){ _LL602: _temp601=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp521)->f1; goto _LL552;} else{ goto _LL553;} _LL553: if(( unsigned int)
_temp521 > 4u?*(( int*) _temp521) == Cyc_Absyn_RgnHandleType: 0){ _LL604:
_temp603=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp521)->f1; goto
_LL554;} else{ goto _LL555;} _LL555: goto _LL556; _LL524: return 1; _LL526:
return 0; _LL528: return 1; _LL530: return 1; _LL532: return 0; _LL534: return
Cyc_Toc_atomic_typ( _temp567); _LL536: return 1; _LL538: for( 0; _temp571 != 0;
_temp571=({ struct Cyc_List_List* _temp605= _temp571; if( _temp605 == 0){ _throw(
Null_Exception);} _temp605->tl;})){ if( ! Cyc_Toc_atomic_typ((*(( struct _tuple3*)({
struct Cyc_List_List* _temp606= _temp571; if( _temp606 == 0){ _throw(
Null_Exception);} _temp606->hd;}))).f2)){ return 0;}} return 1; _LL540: return 0;
_LL542: return 0; _LL544: _temp583= _temp581; goto _LL546; _LL546: for( 0;
_temp583 != 0; _temp583=({ struct Cyc_List_List* _temp607= _temp583; if(
_temp607 == 0){ _throw( Null_Exception);} _temp607->tl;})){ if( ! Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp608=
_temp583; if( _temp608 == 0){ _throw( Null_Exception);} _temp608->hd;}))->type)){
return 0;}} return 1; _LL548: return _temp591->typs == 0; _LL550: return 0;
_LL552: return 0; _LL554: return 0; _LL556:(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp609= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _temp609.last_plus_one - _temp609.curr,
_temp609.curr);})); return 0; _LL522:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp610= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp616;
struct Cyc_Absyn_Conref* _temp618; struct Cyc_Absyn_Tqual _temp620; struct Cyc_Absyn_Conref*
_temp622; void* _temp624; void* _temp626; _LL612: if(( unsigned int) _temp610 >
4u?*(( int*) _temp610) == Cyc_Absyn_PointerType: 0){ _LL617: _temp616=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp610)->f1; _LL627:
_temp626=( void*) _temp616.elt_typ; goto _LL625; _LL625: _temp624=( void*)
_temp616.rgn_typ; goto _LL623; _LL623: _temp622=( struct Cyc_Absyn_Conref*)
_temp616.nullable; goto _LL621; _LL621: _temp620=( struct Cyc_Absyn_Tqual)
_temp616.tq; goto _LL619; _LL619: _temp618=( struct Cyc_Absyn_Conref*) _temp616.bounds;
goto _LL613;} else{ goto _LL614;} _LL614: goto _LL615; _LL613: { void* _temp628=
Cyc_Tcutil_compress( _temp626); _LL630: if( _temp628 ==( void*) Cyc_Absyn_VoidType){
goto _LL631;} else{ goto _LL632;} _LL632: goto _LL633; _LL631: return 1; _LL633:
return 0; _LL629:;} _LL615: return 0; _LL611:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp634= Cyc_Tcutil_compress( t);
struct Cyc_List_List* _temp650; struct Cyc_List_List* _temp652; struct Cyc_Absyn_Structdecl**
_temp654; struct Cyc_Absyn_Structdecl* _temp656; struct Cyc_List_List* _temp657;
struct _tuple0* _temp659; struct Cyc_Absyn_Structdecl** _temp661; struct Cyc_List_List*
_temp663; struct _tuple0* _temp665; struct Cyc_Absyn_Uniondecl** _temp667;
struct Cyc_Absyn_Uniondecl* _temp669; struct Cyc_List_List* _temp670; struct
_tuple0* _temp672; struct Cyc_Absyn_Uniondecl** _temp674; struct Cyc_List_List*
_temp676; struct _tuple0* _temp678; _LL636: if(( unsigned int) _temp634 > 4u?*((
int*) _temp634) == Cyc_Absyn_AnonUnionType: 0){ _LL651: _temp650=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp634)->f1; goto _LL637;} else{ goto
_LL638;} _LL638: if(( unsigned int) _temp634 > 4u?*(( int*) _temp634) == Cyc_Absyn_AnonStructType:
0){ _LL653: _temp652=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp634)->f1; goto _LL639;} else{ goto _LL640;} _LL640: if(( unsigned int)
_temp634 > 4u?*(( int*) _temp634) == Cyc_Absyn_StructType: 0){ _LL660: _temp659=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp634)->f1; goto
_LL658; _LL658: _temp657=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp634)->f2; goto _LL655; _LL655: _temp654=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp634)->f3; if( _temp654 == 0){ goto
_LL642;} else{ _temp656=* _temp654; goto _LL641;}} else{ goto _LL642;} _LL642:
if(( unsigned int) _temp634 > 4u?*(( int*) _temp634) == Cyc_Absyn_StructType: 0){
_LL666: _temp665=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp634)->f1; goto _LL664; _LL664: _temp663=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp634)->f2; goto _LL662; _LL662: _temp661=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp634)->f3;
if( _temp661 == 0){ goto _LL643;} else{ goto _LL644;}} else{ goto _LL644;}
_LL644: if(( unsigned int) _temp634 > 4u?*(( int*) _temp634) == Cyc_Absyn_UnionType:
0){ _LL673: _temp672=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp634)->f1; goto _LL671; _LL671: _temp670=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp634)->f2; goto _LL668; _LL668: _temp667=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp634)->f3;
if( _temp667 == 0){ goto _LL646;} else{ _temp669=* _temp667; goto _LL645;}}
else{ goto _LL646;} _LL646: if(( unsigned int) _temp634 > 4u?*(( int*) _temp634)
== Cyc_Absyn_UnionType: 0){ _LL679: _temp678=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp634)->f1; goto _LL677; _LL677: _temp676=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp634)->f2; goto _LL675; _LL675: _temp674=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp634)->f3;
if( _temp674 == 0){ goto _LL647;} else{ goto _LL648;}} else{ goto _LL648;}
_LL648: goto _LL649; _LL637: _temp652= _temp650; goto _LL639; _LL639: { struct
Cyc_Absyn_Structfield* field= Cyc_Absyn_lookup_field( _temp652, f); if( field ==
0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp681=*({ struct Cyc_Absyn_Structfield* _temp680= field; if(
_temp680 == 0){ _throw( Null_Exception);} _temp680->name;}); xprintf("is_poly_field: bad field %.*s",
_temp681.last_plus_one - _temp681.curr, _temp681.curr);}));} return Cyc_Toc_is_void_star((
void*)({ struct Cyc_Absyn_Structfield* _temp682= field; if( _temp682 == 0){
_throw( Null_Exception);} _temp682->type;}));} _LL641: if( _temp659 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp683=( char*)"StructType name not properly set"; struct _tagged_string
_temp684; _temp684.curr= _temp683; _temp684.base= _temp683; _temp684.last_plus_one=
_temp683 + 33; _temp684;}));} if( _temp656->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp686= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp685= _temp659; if( _temp685 == 0){
_throw( Null_Exception);} _temp685;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp686.last_plus_one - _temp686.curr, _temp686.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp693= _temp656->fields;
if( _temp693 == 0){ _throw( Null_Exception);} _temp693->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp687= fields; if( _temp687 == 0){ _throw(
Null_Exception);} _temp687->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp688= fields; if( _temp688 == 0){ _throw(
Null_Exception);} _temp688->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp689= fields;
if( _temp689 == 0){ _throw( Null_Exception);} _temp689->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp691=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp690= _temp659;
if( _temp690 == 0){ _throw( Null_Exception);} _temp690;})); struct
_tagged_string _temp692=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp691.last_plus_one - _temp691.curr, _temp691.curr, _temp692.last_plus_one -
_temp692.curr, _temp692.curr);})); return 0;} _LL643: if( _temp665 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp694=( char*)"StructType name not properly set"; struct _tagged_string
_temp695; _temp695.curr= _temp694; _temp695.base= _temp694; _temp695.last_plus_one=
_temp694 + 33; _temp695;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp697= Cyc_Absynpp_qvar2string(( struct _tuple0*)({
struct _tuple0* _temp696= _temp665; if( _temp696 == 0){ _throw( Null_Exception);}
_temp696;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp697.last_plus_one
- _temp697.curr, _temp697.curr);})); return 0; _LL645: if( _temp672 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp698=( char*)"UnionType name not properly set"; struct _tagged_string
_temp699; _temp699.curr= _temp698; _temp699.base= _temp698; _temp699.last_plus_one=
_temp698 + 32; _temp699;}));} if( _temp669->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp701= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp700= _temp672; if( _temp700 == 0){
_throw( Null_Exception);} _temp700;})); xprintf("is_poly_field:  union %.*s missing fields",
_temp701.last_plus_one - _temp701.curr, _temp701.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp708= _temp669->fields;
if( _temp708 == 0){ _throw( Null_Exception);} _temp708->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp702= fields; if( _temp702 == 0){ _throw(
Null_Exception);} _temp702->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp703= fields; if( _temp703 == 0){ _throw(
Null_Exception);} _temp703->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp704= fields;
if( _temp704 == 0){ _throw( Null_Exception);} _temp704->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp706=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp705= _temp672;
if( _temp705 == 0){ _throw( Null_Exception);} _temp705;})); struct
_tagged_string _temp707=* f; xprintf("is_poly_field:  union %.*s has no %.*s field",
_temp706.last_plus_one - _temp706.curr, _temp706.curr, _temp707.last_plus_one -
_temp707.curr, _temp707.curr);})); return 0;} _LL647: if( _temp678 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp709=( char*)"UnionType name not properly set"; struct _tagged_string
_temp710; _temp710.curr= _temp709; _temp710.base= _temp709; _temp710.last_plus_one=
_temp709 + 32; _temp710;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp712= Cyc_Absynpp_qvar2string(( struct _tuple0*)({
struct _tuple0* _temp711= _temp678; if( _temp711 == 0){ _throw( Null_Exception);}
_temp711;})); xprintf("is_poly_field:  union %.*s missing structdecl", _temp712.last_plus_one
- _temp712.curr, _temp712.curr);})); return 0; _LL649:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp713= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp713.last_plus_one - _temp713.curr,
_temp713.curr);})); return 0; _LL635:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp714=( void*) e->r; struct _tagged_string*
_temp722; struct Cyc_Absyn_Exp* _temp724; struct _tagged_string* _temp726;
struct Cyc_Absyn_Exp* _temp728; _LL716: if(*(( int*) _temp714) == Cyc_Absyn_StructMember_e){
_LL725: _temp724=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp714)->f1; goto _LL723; _LL723: _temp722=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp714)->f2; goto _LL717;} else{ goto _LL718;}
_LL718: if(*(( int*) _temp714) == Cyc_Absyn_StructArrow_e){ _LL729: _temp728=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp714)->f1;
goto _LL727; _LL727: _temp726=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp714)->f2; goto _LL719;} else{ goto _LL720;} _LL720: goto _LL721; _LL717:
return Cyc_Toc_is_poly_field(( void*)({ struct Cyc_Core_Opt* _temp730= _temp724->topt;
if( _temp730 == 0){ _throw( Null_Exception);} _temp730->v;}), _temp722); _LL719: {
void* _temp732= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp731=
_temp728->topt; if( _temp731 == 0){ _throw( Null_Exception);} _temp731->v;}));
struct Cyc_Absyn_PtrInfo _temp738; struct Cyc_Absyn_Conref* _temp740; struct Cyc_Absyn_Tqual
_temp742; struct Cyc_Absyn_Conref* _temp744; void* _temp746; void* _temp748;
_LL734: if(( unsigned int) _temp732 > 4u?*(( int*) _temp732) == Cyc_Absyn_PointerType:
0){ _LL739: _temp738=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp732)->f1; _LL749: _temp748=( void*) _temp738.elt_typ; goto _LL747; _LL747:
_temp746=( void*) _temp738.rgn_typ; goto _LL745; _LL745: _temp744=( struct Cyc_Absyn_Conref*)
_temp738.nullable; goto _LL743; _LL743: _temp742=( struct Cyc_Absyn_Tqual)
_temp738.tq; goto _LL741; _LL741: _temp740=( struct Cyc_Absyn_Conref*) _temp738.bounds;
goto _LL735;} else{ goto _LL736;} _LL736: goto _LL737; _LL735: return Cyc_Toc_is_poly_field(
_temp748, _temp726); _LL737:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp751= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp750= _temp728->topt; if( _temp750 == 0){ _throw( Null_Exception);} _temp750->v;}));
xprintf("is_poly_project:  bad type %.*s", _temp751.last_plus_one - _temp751.curr,
_temp751.curr);})); return 0; _LL733:;} _LL721: return 0; _LL715:;} static char
_temp754[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp754, _temp754, _temp754 + 10u}; static struct
_tuple0 Cyc_Toc_gc_malloc_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var,
0),({ struct Cyc_List_List* _temp755=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp755->hd=( void*) s; _temp755->tl= 0; _temp755;}), 0);}
static char _temp758[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp758, _temp758, _temp758 + 17u}; static struct
_tuple0 Cyc_Toc_gc_malloc_atomic_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_atomic_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var,
0),({ struct Cyc_List_List* _temp759=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp759->hd=( void*) s; _temp759->tl= 0; _temp759;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static char _temp762[ 15u]="_region_malloc"; static struct _tagged_string
Cyc_Toc__region_malloc_string=( struct _tagged_string){ _temp762, _temp762,
_temp762 + 15u}; static struct _tuple0 Cyc_Toc_region_malloc_var=( struct
_tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__region_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_region_malloc_var,
0),({ struct Cyc_List_List* _temp763=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp763->hd=( void*) rgn; _temp763->tl=({ struct Cyc_List_List*
_temp764=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp764->hd=( void*) s; _temp764->tl= 0; _temp764;}); _temp763;}), 0);} static
const int Cyc_Toc_NullCheck= 0; struct Cyc_Toc_NullCheck_struct{ int tag; void*
f1; } ; static const int Cyc_Toc_UntagPtr= 1; struct Cyc_Toc_UntagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; int f3; } ; static const int Cyc_Toc_TagPtr=
2; struct Cyc_Toc_TagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; }
; typedef void* Cyc_Toc_conv; struct _tuple7{ void* f1; void* f2; } ; static
struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp766=({ struct _tuple7
_temp765; _temp765.f1= t1; _temp765.f2= t2; _temp765;}); void* _temp772; struct
Cyc_Absyn_PtrInfo _temp774; void* _temp776; struct Cyc_Absyn_PtrInfo _temp778;
_LL768: _LL777: _temp776= _temp766.f1; if(( unsigned int) _temp776 > 4u?*(( int*)
_temp776) == Cyc_Absyn_PointerType: 0){ _LL779: _temp778=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp776)->f1; goto _LL773;} else{ goto
_LL770;} _LL773: _temp772= _temp766.f2; if(( unsigned int) _temp772 > 4u?*(( int*)
_temp772) == Cyc_Absyn_PointerType: 0){ _LL775: _temp774=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp772)->f1; goto _LL769;} else{ goto
_LL770;} _LL770: goto _LL771; _LL769: { int n1=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp778.nullable); int n2=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp774.nullable); void* b1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp778.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp774.bounds); struct Cyc_List_List* convs= 0;{
struct _tuple7 _temp781=({ struct _tuple7 _temp780; _temp780.f1= b1; _temp780.f2=
b2; _temp780;}); void* _temp791; struct Cyc_Absyn_Exp* _temp793; void* _temp795;
struct Cyc_Absyn_Exp* _temp797; void* _temp799; void* _temp801; void* _temp803;
void* _temp805; struct Cyc_Absyn_Exp* _temp807; void* _temp809; struct Cyc_Absyn_Exp*
_temp811; void* _temp813; _LL783: _LL796: _temp795= _temp781.f1; if((
unsigned int) _temp795 > 1u?*(( int*) _temp795) == Cyc_Absyn_Upper_b: 0){ _LL798:
_temp797=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp795)->f1;
goto _LL792;} else{ goto _LL785;} _LL792: _temp791= _temp781.f2; if((
unsigned int) _temp791 > 1u?*(( int*) _temp791) == Cyc_Absyn_Upper_b: 0){ _LL794:
_temp793=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp791)->f1;
goto _LL784;} else{ goto _LL785;} _LL785: _LL802: _temp801= _temp781.f1; if(
_temp801 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL800;} else{ goto _LL787;}
_LL800: _temp799= _temp781.f2; if( _temp799 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL786;} else{ goto _LL787;} _LL787: _LL806: _temp805= _temp781.f1; if((
unsigned int) _temp805 > 1u?*(( int*) _temp805) == Cyc_Absyn_Upper_b: 0){ _LL808:
_temp807=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp805)->f1;
goto _LL804;} else{ goto _LL789;} _LL804: _temp803= _temp781.f2; if( _temp803 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL788;} else{ goto _LL789;} _LL789: _LL814:
_temp813= _temp781.f1; if( _temp813 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL810;}
else{ goto _LL782;} _LL810: _temp809= _temp781.f2; if(( unsigned int) _temp809 >
1u?*(( int*) _temp809) == Cyc_Absyn_Upper_b: 0){ _LL812: _temp811=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp809)->f1; goto _LL790;} else{ goto _LL782;}
_LL784: if( n1? ! n2: 0){ convs=({ struct Cyc_List_List* _temp815=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp815->hd=( void*)(( void*)({
struct Cyc_Toc_NullCheck_struct* _temp816=( struct Cyc_Toc_NullCheck_struct*)
GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct)); _temp816[ 0]=({ struct Cyc_Toc_NullCheck_struct
_temp817; _temp817.tag= Cyc_Toc_NullCheck; _temp817.f1=( void*) t2; _temp817;});
_temp816;})); _temp815->tl= convs; _temp815;});} goto _LL782; _LL786: goto
_LL782; _LL788: convs=({ struct Cyc_List_List* _temp818=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp818->hd=( void*)(( void*)({
struct Cyc_Toc_TagPtr_struct* _temp819=( struct Cyc_Toc_TagPtr_struct*)
GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct)); _temp819[ 0]=({ struct Cyc_Toc_TagPtr_struct
_temp820; _temp820.tag= Cyc_Toc_TagPtr; _temp820.f1=( void*) Cyc_Toc_typ_to_c(
t1); _temp820.f2= _temp807; _temp820;}); _temp819;})); _temp818->tl= convs;
_temp818;}); goto _LL782; _LL790: convs=({ struct Cyc_List_List* _temp821=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp821->hd=(
void*)(( void*)({ struct Cyc_Toc_UntagPtr_struct* _temp822=( struct Cyc_Toc_UntagPtr_struct*)
GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct)); _temp822[ 0]=({ struct Cyc_Toc_UntagPtr_struct
_temp823; _temp823.tag= Cyc_Toc_UntagPtr; _temp823.f1=( void*) Cyc_Toc_typ_to_c(
t1); _temp823.f2= _temp811; _temp823.f3= n2; _temp823;}); _temp822;})); _temp821->tl=
convs; _temp821;}); goto _LL782; _LL782:;} return convs;} _LL771: return 0;
_LL767:;}} static char _temp826[ 7u]="_throw"; static struct _tagged_string Cyc_Toc__throw_string=(
struct _tagged_string){ _temp826, _temp826, _temp826 + 7u}; static struct
_tuple0 Cyc_Toc__throw_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__throw_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__throw_var,
0),({ struct Cyc_List_List* _temp827=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp827->hd=( void*) e; _temp827->tl= 0; _temp827;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp828=( char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp829; _temp829.curr= _temp828; _temp829.base= _temp828;
_temp829.last_plus_one= _temp828 + 44; _temp829;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple0* a= Cyc_Toc_temp_var();
struct _tuple0* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp830= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp836; struct Cyc_Absyn_Conref* _temp838; struct Cyc_Absyn_Tqual
_temp840; struct Cyc_Absyn_Conref* _temp842; void* _temp844; void* _temp846;
_LL832: if(( unsigned int) _temp830 > 4u?*(( int*) _temp830) == Cyc_Absyn_PointerType:
0){ _LL837: _temp836=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp830)->f1; _LL847: _temp846=( void*) _temp836.elt_typ; goto _LL845; _LL845:
_temp844=( void*) _temp836.rgn_typ; goto _LL843; _LL843: _temp842=( struct Cyc_Absyn_Conref*)
_temp836.nullable; goto _LL841; _LL841: _temp840=( struct Cyc_Absyn_Tqual)
_temp836.tq; goto _LL839; _LL839: _temp838=( struct Cyc_Absyn_Conref*) _temp836.bounds;
goto _LL833;} else{ goto _LL834;} _LL834: goto _LL835; _LL833: elt_typ= _temp846;{
struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp846, Cyc_Toc_mt_tq);
strt_typ= Cyc_Absyn_strct( n); goto _LL831;} _LL835: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp848=( char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp849;
_temp849.curr= _temp848; _temp849.base= _temp848; _temp849.last_plus_one=
_temp848 + 28; _temp849;})); _LL831:;} if( toplevel){ int is_string= 0;{ void*
_temp850=( void*) e->r; void* _temp856; struct _tagged_string _temp858; _LL852:
if(*(( int*) _temp850) == Cyc_Absyn_Const_e){ _LL857: _temp856=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp850)->f1; if(( unsigned int) _temp856 > 1u?*((
int*) _temp856) == Cyc_Absyn_String_c: 0){ _LL859: _temp858=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp856)->f1; goto _LL853;}
else{ goto _LL854;}} else{ goto _LL854;} _LL854: goto _LL855; _LL853: is_string=
1; goto _LL851; _LL855: goto _LL851; _LL851:;}{ struct Cyc_Absyn_Exp* xexp; if(
is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp863=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp863[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp864; _temp864.tag= Cyc_Absyn_ArrayType; _temp864.f1=(
void*) elt_typ; _temp864.f2= Cyc_Toc_mt_tq; _temp864.f3=( struct Cyc_Absyn_Exp*)
sz; _temp864;}); _temp863;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp860=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp860->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp861=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp861[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp862; _temp862.tag= Cyc_Absyn_Var_d;
_temp862.f1= vd; _temp862;}); _temp861;}), 0); _temp860->tl= Cyc_Toc_result_decls;
_temp860;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp865=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp865->hd=( void*)({ struct _tuple4* _temp870=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp870->f1= 0; _temp870->f2=
xexp; _temp870;}); _temp865->tl=({ struct Cyc_List_List* _temp866=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp866->hd=( void*)({ struct
_tuple4* _temp869=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp869->f1= 0; _temp869->f2= xexp; _temp869;}); _temp866->tl=({ struct Cyc_List_List*
_temp867=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp867->hd=( void*)({ struct _tuple4* _temp868=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp868->f1= 0; _temp868->f2= xplussz; _temp868;});
_temp867->tl= 0; _temp867;}); _temp866;}); _temp865;}), 0); return urm_exp;}}}
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
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp871=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp872; _temp872.curr= _temp871; _temp872.base= _temp871; _temp872.last_plus_one=
_temp871 + 45; _temp872;}));}{ struct _tuple0* x= Cyc_Toc_temp_var(); int
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
_temp873= cs; if( _temp873 == 0){ _throw( Null_Exception);} _temp873->tl;})){
void* _temp875=( void*)({ struct Cyc_List_List* _temp874= cs; if( _temp874 == 0){
_throw( Null_Exception);} _temp874->hd;}); void* _temp883; struct Cyc_Absyn_Exp*
_temp885; void* _temp887; int _temp889; struct Cyc_Absyn_Exp* _temp891; void*
_temp893; _LL877: if(*(( int*) _temp875) == Cyc_Toc_NullCheck){ _LL884: _temp883=(
void*)(( struct Cyc_Toc_NullCheck_struct*) _temp875)->f1; goto _LL878;} else{
goto _LL879;} _LL879: if(*(( int*) _temp875) == Cyc_Toc_TagPtr){ _LL888:
_temp887=( void*)(( struct Cyc_Toc_TagPtr_struct*) _temp875)->f1; goto _LL886;
_LL886: _temp885=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_TagPtr_struct*)
_temp875)->f2; goto _LL880;} else{ goto _LL881;} _LL881: if(*(( int*) _temp875)
== Cyc_Toc_UntagPtr){ _LL894: _temp893=( void*)(( struct Cyc_Toc_UntagPtr_struct*)
_temp875)->f1; goto _LL892; _LL892: _temp891=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Toc_UntagPtr_struct*) _temp875)->f2; goto _LL890; _LL890: _temp889=( int)((
struct Cyc_Toc_UntagPtr_struct*) _temp875)->f3; goto _LL882;} else{ goto _LL876;}
_LL878: e= Cyc_Toc_null_check_conv( toplevel, _temp883, e); goto _LL876; _LL880:
e= Cyc_Toc_tag_ptr_conv( toplevel, _temp887, _temp885, e); goto _LL876; _LL882:
e= Cyc_Toc_untag_ptr_conv( toplevel, _temp893, _temp891, _temp889, e); goto
_LL876; _LL876:;}} return e;} struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab;
struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt* fallthru_info; struct
Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct Cyc_Toc_Env* Cyc_Toc_env_t;
static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({ struct Cyc_Toc_Env*
_temp895=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct Cyc_Toc_Env));
_temp895->break_lab= 0; _temp895->continue_lab= 0; _temp895->fallthru_info= 0;
_temp895->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp895->toplevel= 1;
_temp895;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp896=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp896->break_lab= e->break_lab; _temp896->continue_lab=
e->continue_lab; _temp896->fallthru_info= e->fallthru_info; _temp896->varmap= e->varmap;
_temp896->toplevel= e->toplevel; _temp896;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp897=(* x).f1; struct
Cyc_List_List* _temp903; _LL899: if(( unsigned int) _temp897 > 1u?*(( int*)
_temp897) == Cyc_Absyn_Rel_n: 0){ _LL904: _temp903=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp897)->f1; goto _LL900;} else{ goto _LL901;}
_LL901: goto _LL902; _LL900:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp905=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp905[ 0]=({ struct Cyc_Core_Impossible_struct _temp906; _temp906.tag= Cyc_Core_Impossible;
_temp906.f1=({ struct _tagged_string _temp907= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp907.last_plus_one - _temp907.curr,
_temp907.curr);}); _temp906;}); _temp905;})); _LL902: goto _LL898; _LL898:;}{
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
_temp908= fallthru_binders; if( _temp908 == 0){ _throw( Null_Exception);}
_temp908->tl;})){ fallthru_vars=({ struct Cyc_List_List* _temp909=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp909->hd=( void*)(( struct Cyc_Absyn_Vardecl*)({
struct Cyc_List_List* _temp910= fallthru_binders; if( _temp910 == 0){ _throw(
Null_Exception);} _temp910->hd;}))->name; _temp909->tl= fallthru_vars; _temp909;});}
fallthru_vars=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fallthru_vars);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp911=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp911->v=( void*) break_l; _temp911;}); ans->fallthru_info=({
struct Cyc_Core_Opt* _temp912=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp912->v=( void*)({ struct _tuple8* _temp913=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp913->f1= fallthru_l; _temp913->f2=
fallthru_vars; _temp913->f3= next_case_env->varmap; _temp913;}); _temp912;});
return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct
Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab=({ struct Cyc_Core_Opt* _temp914=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp914->v=( void*) break_l; _temp914;});
ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env* e, struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp915=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp915->v=( void*)({ struct _tuple8* _temp916=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp916->f1= next_l; _temp916->f2= 0; _temp916->f3=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp916;}); _temp915;}); return ans;} struct _tuple9{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
struct Cyc_List_List* f4; char f5; int f6; } ; struct _tuple10{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp917=( char*)"expecting a literal format string";
struct _tagged_string _temp918; _temp918.curr= _temp917; _temp918.base= _temp917;
_temp918.last_plus_one= _temp917 + 34; _temp918;}));}{ struct _tagged_string
fmt_str;{ void* _temp920=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp919= args; if( _temp919 == 0){ _throw( Null_Exception);} _temp919->hd;}))->r;
void* _temp926; struct _tagged_string _temp928; _LL922: if(*(( int*) _temp920)
== Cyc_Absyn_Const_e){ _LL927: _temp926=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp920)->f1; if(( unsigned int) _temp926 > 1u?*(( int*) _temp926) == Cyc_Absyn_String_c:
0){ _LL929: _temp928=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp926)->f1; goto _LL923;} else{ goto _LL924;}} else{ goto _LL924;} _LL924:
goto _LL925; _LL923: fmt_str= _temp928; goto _LL921; _LL925: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp930=( char*)"expecting a literal format string"; struct _tagged_string
_temp931; _temp931.curr= _temp930; _temp931.base= _temp930; _temp931.last_plus_one=
_temp930 + 34; _temp931;})); _LL921:;}{ int len=( int)({ struct _tagged_string
_temp1038= fmt_str;( unsigned int)( _temp1038.last_plus_one - _temp1038.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(({ struct _tagged_string _temp932=
fmt_str; char* _temp934= _temp932.curr + i; if( _temp932.base == 0? 1:( _temp934
< _temp932.base? 1: _temp934 >= _temp932.last_plus_one)){ _throw( Null_Exception);}*
_temp934;}) =='%'){ i ++; if( i < len?({ struct _tagged_string _temp935= fmt_str;
char* _temp937= _temp935.curr + i; if( _temp935.base == 0? 1:( _temp937 <
_temp935.base? 1: _temp937 >= _temp935.last_plus_one)){ _throw( Null_Exception);}*
_temp937;}) =='s': 0){ break;}}} if( i == len){ if( aopt != 0){ args=({ struct
Cyc_List_List* _temp938=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp938->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp939=
aopt; if( _temp939 == 0){ _throw( Null_Exception);} _temp939->v;})); _temp938->tl=
args; _temp938;});} return Cyc_Absyn_primop_exp( p, args, 0);} args=({ struct
Cyc_List_List* _temp940= args; if( _temp940 == 0){ _throw( Null_Exception);}
_temp940->tl;}); typs=({ struct Cyc_List_List* _temp941= typs; if( _temp941 == 0){
_throw( Null_Exception);} _temp941->tl;});{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ char c=({ struct _tagged_string _temp1028= fmt_str; char*
_temp1030= _temp1028.curr + i; if( _temp1028.base == 0? 1:( _temp1030 <
_temp1028.base? 1: _temp1030 >= _temp1028.last_plus_one)){ _throw(
Null_Exception);}* _temp1030;}); rev_fmt=({ struct Cyc_List_List* _temp942=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp942->hd=(
void*)(( int) c); _temp942->tl= rev_fmt; _temp942;}); if( c !='%'){ continue;}{
struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification( fmt_str, i
+ 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp943=( char*)"bad format string"; struct
_tagged_string _temp944; _temp944.curr= _temp943; _temp944.base= _temp943;
_temp944.last_plus_one= _temp943 + 18; _temp944;}));}{ struct _tuple9 _temp948;
int _temp949; char _temp951; struct Cyc_List_List* _temp953; struct Cyc_List_List*
_temp955; struct Cyc_List_List* _temp957; struct Cyc_List_List* _temp959; struct
_tuple9* _temp946=( struct _tuple9*)({ struct Cyc_Core_Opt* _temp945= x; if(
_temp945 == 0){ _throw( Null_Exception);} _temp945->v;}); _temp948=* _temp946;
_LL960: _temp959= _temp948.f1; goto _LL958; _LL958: _temp957= _temp948.f2; goto
_LL956; _LL956: _temp955= _temp948.f3; goto _LL954; _LL954: _temp953= _temp948.f4;
goto _LL952; _LL952: _temp951= _temp948.f5; goto _LL950; _LL950: _temp949=
_temp948.f6; goto _LL947; _LL947: i= _temp949 - 1; if( _temp951 !='s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp961=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
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
_LL969;} else{ goto _LL970;} _LL970: goto _LL971; _LL969: { struct _tuple0* temp=
Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp978=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp978->hd=( void*)({ struct
_tuple10* _temp979=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp979->f1= temp; _temp979->f2=( void*)({ struct Cyc_List_List* _temp981= typs;
if( _temp981 == 0){ _throw( Null_Exception);} _temp981->hd;}); _temp979->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp980= args; if( _temp980 == 0){
_throw( Null_Exception);} _temp980->hd;}); _temp979;}); _temp978->tl= rev_temps;
_temp978;}); rev_result=({ struct Cyc_List_List* _temp982=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp982->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp982->tl= rev_result; _temp982;}); args=({ struct Cyc_List_List*
_temp983= args; if( _temp983 == 0){ _throw( Null_Exception);} _temp983->tl;});
typs=({ struct Cyc_List_List* _temp984= typs; if( _temp984 == 0){ _throw(
Null_Exception);} _temp984->tl;}); goto _LL967;} _LL971: goto _LL967; _LL967:;}{
struct Cyc_List_List* _temp985= _temp955; struct Cyc_List_List _temp992; struct
Cyc_List_List* _temp993; struct Cyc_List_List _temp995; struct Cyc_List_List*
_temp996; int _temp998; int _temp1000; _LL987: if( _temp985 == 0){ goto _LL989;}
else{ _temp992=* _temp985; _LL1001: _temp1000=( int) _temp992.hd; goto _LL994;
_LL994: _temp993=( struct Cyc_List_List*) _temp992.tl; if( _temp993 == 0){ goto
_LL989;} else{ _temp995=* _temp993; _LL999: _temp998=( int) _temp995.hd; goto
_LL997; _LL997: _temp996=( struct Cyc_List_List*) _temp995.tl; if( _temp996 == 0){
goto _LL991;} else{ goto _LL989;}}} _LL991: if( _temp1000 =='.'? _temp998 =='*':
0){ goto _LL988;} else{ goto _LL989;} _LL989: goto _LL990; _LL988: { struct
_tuple0* temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp1002=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1002->hd=(
void*)({ struct _tuple10* _temp1003=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1003->f1= temp; _temp1003->f2=( void*)({ struct Cyc_List_List*
_temp1005= typs; if( _temp1005 == 0){ _throw( Null_Exception);} _temp1005->hd;});
_temp1003->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1004= args;
if( _temp1004 == 0){ _throw( Null_Exception);} _temp1004->hd;}); _temp1003;});
_temp1002->tl= rev_temps; _temp1002;}); rev_result=({ struct Cyc_List_List*
_temp1006=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1006->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp1006->tl= rev_result;
_temp1006;}); args=({ struct Cyc_List_List* _temp1007= args; if( _temp1007 == 0){
_throw( Null_Exception);} _temp1007->tl;}); typs=({ struct Cyc_List_List*
_temp1008= typs; if( _temp1008 == 0){ _throw( Null_Exception);} _temp1008->tl;});
goto _LL986;} _LL990: goto _LL986; _LL986:;} if( _temp951 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1009= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp1009.last_plus_one - _temp1009.curr, _temp1009.curr);}));}{ struct _tuple0*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp1010=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1010->hd=( void*)({
struct _tuple10* _temp1011=( struct _tuple10*) GC_malloc( sizeof( struct
_tuple10)); _temp1011->f1= temp; _temp1011->f2=( void*)({ struct Cyc_List_List*
_temp1013= typs; if( _temp1013 == 0){ _throw( Null_Exception);} _temp1013->hd;});
_temp1011->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1012= args;
if( _temp1012 == 0){ _throw( Null_Exception);} _temp1012->hd;}); _temp1011;});
_temp1010->tl= rev_temps; _temp1010;}); rev_result=({ struct Cyc_List_List*
_temp1014=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1014->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp1014->tl= rev_result;
_temp1014;}); args=({ struct Cyc_List_List* _temp1015= args; if( _temp1015 == 0){
_throw( Null_Exception);} _temp1015->tl;}); typs=({ struct Cyc_List_List*
_temp1016= typs; if( _temp1016 == 0){ _throw( Null_Exception);} _temp1016->tl;});}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp1017=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1017->hd=( void*)(( int)'.');
_temp1017->tl= rev_fmt; _temp1017;}); rev_fmt=({ struct Cyc_List_List* _temp1018=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1018->hd=(
void*)(( int)'*'); _temp1018->tl= rev_fmt; _temp1018;}); rev_fmt=({ struct Cyc_List_List*
_temp1019=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1019->hd=( void*)(( int)'s'); _temp1019->tl= rev_fmt; _temp1019;});{ struct
_tuple0* temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp1020=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1020->hd=(
void*)({ struct _tuple10* _temp1021=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1021->f1= temp; _temp1021->f2=( void*)({ struct Cyc_List_List*
_temp1023= typs; if( _temp1023 == 0){ _throw( Null_Exception);} _temp1023->hd;});
_temp1021->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1022= args;
if( _temp1022 == 0){ _throw( Null_Exception);} _temp1022->hd;}); _temp1021;});
_temp1020->tl= rev_temps; _temp1020;});{ struct Cyc_Absyn_Exp* size_exp= Cyc_Absyn_subtract_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp,
0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0),
0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List* _temp1024=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1024->hd=(
void*) size_exp; _temp1024->tl= rev_result; _temp1024;}); rev_result=({ struct
Cyc_List_List* _temp1025=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1025->hd=( void*) cstring_exp; _temp1025->tl= rev_result; _temp1025;});
args=({ struct Cyc_List_List* _temp1026= args; if( _temp1026 == 0){ _throw(
Null_Exception);} _temp1026->tl;}); typs=({ struct Cyc_List_List* _temp1027=
typs; if( _temp1027 == 0){ _throw( Null_Exception);} _temp1027->tl;});}}}}}}{
struct Cyc_List_List* es=({ struct Cyc_List_List* _temp1037=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1037->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp1037->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_result); _temp1037;}); if( aopt != 0){ es=({ struct Cyc_List_List*
_temp1031=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1031->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp1032=
aopt; if( _temp1032 == 0){ _throw( Null_Exception);} _temp1032->v;})); _temp1031->tl=
es; _temp1031;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p, es, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps != 0;
rev_temps=({ struct Cyc_List_List* _temp1033= rev_temps; if( _temp1033 == 0){
_throw( Null_Exception);} _temp1033->tl;})){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)({ struct Cyc_List_List* _temp1034= rev_temps; if( _temp1034 ==
0){ _throw( Null_Exception);} _temp1034->hd;}))).f1,(*(( struct _tuple10*)({
struct Cyc_List_List* _temp1035= rev_temps; if( _temp1035 == 0){ _throw(
Null_Exception);} _temp1035->hd;}))).f2,( struct Cyc_Absyn_Exp*)(*(( struct
_tuple10*)({ struct Cyc_List_List* _temp1036= rev_temps; if( _temp1036 == 0){
_throw( Null_Exception);} _temp1036->hd;}))).f3, s, 0);} return Cyc_Absyn_stmt_exp(
s, 0);}}}}}} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1039=( char*)"Missing type in primop "; struct
_tagged_string _temp1040; _temp1040.curr= _temp1039; _temp1040.base= _temp1039;
_temp1040.last_plus_one= _temp1039 + 24; _temp1040;}));} return Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp1041= e->topt; if( _temp1041 == 0){ _throw(
Null_Exception);} _temp1041->v;}));} static void* Cyc_Toc_get_cyc_typ( struct
Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1042=( char*)"Missing type in primop ";
struct _tagged_string _temp1043; _temp1043.curr= _temp1042; _temp1043.base=
_temp1042; _temp1043.last_plus_one= _temp1042 + 24; _temp1043;}));} return( void*)({
struct Cyc_Core_Opt* _temp1044= e->topt; if( _temp1044 == 0){ _throw(
Null_Exception);} _temp1044->v;});} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1045=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1045->f1= Cyc_Toc_mt_tq; _temp1045->f2=
Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1046= e->topt; if(
_temp1046 == 0){ _throw( Null_Exception);} _temp1046->v;})); _temp1045;});}
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e);
static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp1047=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1047[ 0]=({
struct _tuple4 _temp1048; _temp1048.f1= 0; _temp1048.f2= e; _temp1048;});
_temp1047;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
se= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic( se): Cyc_Toc_malloc_ptr(
se), 0);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1049= rgnopt; if( _temp1049 == 0){ _throw( Null_Exception);} _temp1049;});
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
_temp1050= dles; if( _temp1050 == 0){ _throw( Null_Exception);} _temp1050->tl;})){
struct _tuple4 _temp1054; struct Cyc_Absyn_Exp* _temp1055; struct Cyc_List_List*
_temp1057; struct _tuple4* _temp1052=( struct _tuple4*)({ struct Cyc_List_List*
_temp1051= dles; if( _temp1051 == 0){ _throw( Null_Exception);} _temp1051->hd;});
_temp1054=* _temp1052; _LL1058: _temp1057= _temp1054.f1; goto _LL1056; _LL1056:
_temp1055= _temp1054.f2; goto _LL1053; _LL1053: { struct Cyc_Absyn_Exp* e_index;
if( _temp1057 == 0){ e_index= Cyc_Absyn_signed_int_exp(( count --), 0);} else{
if(({ struct Cyc_List_List* _temp1059= _temp1057; if( _temp1059 == 0){ _throw(
Null_Exception);} _temp1059->tl;}) != 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1060=( char*)"multiple designators in array";
struct _tagged_string _temp1061; _temp1061.curr= _temp1060; _temp1061.base=
_temp1060; _temp1061.last_plus_one= _temp1060 + 30; _temp1061;}));}{ void* d=(
void*)({ struct Cyc_List_List* _temp1074= _temp1057; if( _temp1074 == 0){ _throw(
Null_Exception);} _temp1074->hd;}); void* _temp1062= d; struct Cyc_Absyn_Exp*
_temp1068; struct _tagged_string* _temp1070; _LL1064: if(*(( int*) _temp1062) ==
Cyc_Absyn_ArrayElement){ _LL1069: _temp1068=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayElement_struct*) _temp1062)->f1; goto _LL1065;} else{ goto
_LL1066;} _LL1066: if(*(( int*) _temp1062) == Cyc_Absyn_FieldName){ _LL1071:
_temp1070=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1062)->f1; goto _LL1067;} else{ goto _LL1063;} _LL1065: Cyc_Toc_exp_to_c(
nv, _temp1068); e_index= _temp1068; goto _LL1063; _LL1067: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1072=( char*)"field name designators in array"; struct _tagged_string
_temp1073; _temp1073.curr= _temp1072; _temp1073.base= _temp1072; _temp1073.last_plus_one=
_temp1072 + 32; _temp1073;})); goto _LL1063; _LL1063:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp1075=( void*)
_temp1055->r; struct Cyc_List_List* _temp1085; struct Cyc_Absyn_Exp* _temp1087;
struct Cyc_Absyn_Exp* _temp1089; struct Cyc_Absyn_Vardecl* _temp1091; struct Cyc_List_List*
_temp1093; void* _temp1095; _LL1077: if(*(( int*) _temp1075) == Cyc_Absyn_Array_e){
_LL1086: _temp1085=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1075)->f1; goto _LL1078;} else{ goto _LL1079;} _LL1079: if(*(( int*)
_temp1075) == Cyc_Absyn_Comprehension_e){ _LL1092: _temp1091=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1075)->f1; goto _LL1090; _LL1090:
_temp1089=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1075)->f2; goto _LL1088; _LL1088: _temp1087=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1075)->f3; goto _LL1080;} else{
goto _LL1081;} _LL1081: if(*(( int*) _temp1075) == Cyc_Absyn_AnonStruct_e){
_LL1096: _temp1095=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1075)->f1;
goto _LL1094; _LL1094: _temp1093=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1075)->f2; goto _LL1082;} else{ goto _LL1083;} _LL1083: goto _LL1084;
_LL1078: s= Cyc_Toc_init_array( nv, lval, _temp1085, s); goto _LL1076; _LL1080:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1091, _temp1089, _temp1087, s, 0);
goto _LL1076; _LL1082: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1095,
_temp1093, s); goto _LL1076; _LL1084: Cyc_Toc_exp_to_c( nv, _temp1055); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp1055, 0),
s, 0); goto _LL1076; _LL1076:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* x1= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp( x1,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1119=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1119[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1120; _temp1120.tag= Cyc_Absyn_Local_b;
_temp1120.f1= vd; _temp1120;}); _temp1119;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_var_exp( max, 0), 0);
struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( x1, 0), 0);
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( x1,
0), 0); struct Cyc_Absyn_Stmt* body;{ void* _temp1097=( void*) e2->r; struct Cyc_List_List*
_temp1107; struct Cyc_Absyn_Exp* _temp1109; struct Cyc_Absyn_Exp* _temp1111;
struct Cyc_Absyn_Vardecl* _temp1113; struct Cyc_List_List* _temp1115; void*
_temp1117; _LL1099: if(*(( int*) _temp1097) == Cyc_Absyn_Array_e){ _LL1108:
_temp1107=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1097)->f1;
goto _LL1100;} else{ goto _LL1101;} _LL1101: if(*(( int*) _temp1097) == Cyc_Absyn_Comprehension_e){
_LL1114: _temp1113=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1097)->f1; goto _LL1112; _LL1112: _temp1111=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1097)->f2; goto _LL1110; _LL1110:
_temp1109=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1097)->f3; goto _LL1102;} else{ goto _LL1103;} _LL1103: if(*(( int*)
_temp1097) == Cyc_Absyn_AnonStruct_e){ _LL1118: _temp1117=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1097)->f1; goto _LL1116; _LL1116: _temp1115=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1097)->f2; goto _LL1104;} else{ goto
_LL1105;} _LL1105: goto _LL1106; _LL1100: body= Cyc_Toc_init_array( nv2, lval,
_temp1107, Cyc_Absyn_skip_stmt( 0)); goto _LL1098; _LL1102: body= Cyc_Toc_init_comprehension(
nv2, lval, _temp1113, _temp1111, _temp1109, Cyc_Absyn_skip_stmt( 0), 0); goto
_LL1098; _LL1104: body= Cyc_Toc_init_anon_struct( nv, lval, _temp1117, _temp1115,
Cyc_Absyn_skip_stmt( 0)); goto _LL1098; _LL1106: Cyc_Toc_exp_to_c( nv2, e2);
body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL1098; _LL1098:;}{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt(
max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( x1,
Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( dles); for( 0; es != 0; es=({ struct Cyc_List_List* _temp1121= es;
if( _temp1121 == 0){ _throw( Null_Exception);} _temp1121->tl;})){ struct _tuple4
_temp1125; struct Cyc_Absyn_Exp* _temp1126; struct Cyc_List_List* _temp1128;
struct _tuple4* _temp1123=( struct _tuple4*)({ struct Cyc_List_List* _temp1122=
es; if( _temp1122 == 0){ _throw( Null_Exception);} _temp1122->hd;}); _temp1125=*
_temp1123; _LL1129: _temp1128= _temp1125.f1; goto _LL1127; _LL1127: _temp1126=
_temp1125.f2; goto _LL1124; _LL1124: if( _temp1128 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1130=(
char*)"empty designator list"; struct _tagged_string _temp1131; _temp1131.curr=
_temp1130; _temp1131.base= _temp1130; _temp1131.last_plus_one= _temp1130 + 22;
_temp1131;}));} if(({ struct Cyc_List_List* _temp1132= _temp1128; if( _temp1132
== 0){ _throw( Null_Exception);} _temp1132->tl;}) != 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1133=(
char*)"too many designators in anonymous struct"; struct _tagged_string
_temp1134; _temp1134.curr= _temp1133; _temp1134.base= _temp1133; _temp1134.last_plus_one=
_temp1133 + 41; _temp1134;}));}{ void* _temp1136=( void*)({ struct Cyc_List_List*
_temp1135= _temp1128; if( _temp1135 == 0){ _throw( Null_Exception);} _temp1135->hd;});
struct _tagged_string* _temp1142; _LL1138: if(*(( int*) _temp1136) == Cyc_Absyn_FieldName){
_LL1143: _temp1142=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1136)->f1; goto _LL1139;} else{ goto _LL1140;} _LL1140: goto _LL1141;
_LL1139: { struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs,
_temp1142, 0);{ void* _temp1144=( void*) _temp1126->r; struct Cyc_List_List*
_temp1154; struct Cyc_Absyn_Exp* _temp1156; struct Cyc_Absyn_Exp* _temp1158;
struct Cyc_Absyn_Vardecl* _temp1160; struct Cyc_List_List* _temp1162; void*
_temp1164; _LL1146: if(*(( int*) _temp1144) == Cyc_Absyn_Array_e){ _LL1155:
_temp1154=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1144)->f1;
goto _LL1147;} else{ goto _LL1148;} _LL1148: if(*(( int*) _temp1144) == Cyc_Absyn_Comprehension_e){
_LL1161: _temp1160=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1144)->f1; goto _LL1159; _LL1159: _temp1158=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1144)->f2; goto _LL1157; _LL1157:
_temp1156=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1144)->f3; goto _LL1149;} else{ goto _LL1150;} _LL1150: if(*(( int*)
_temp1144) == Cyc_Absyn_AnonStruct_e){ _LL1165: _temp1164=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1144)->f1; goto _LL1163; _LL1163: _temp1162=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1144)->f2; goto _LL1151;} else{ goto
_LL1152;} _LL1152: goto _LL1153; _LL1147: s= Cyc_Toc_init_array( nv, lval,
_temp1154, s); goto _LL1145; _LL1149: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1160, _temp1158, _temp1156, s, 0); goto _LL1145; _LL1151: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp1164, _temp1162, s); goto _LL1145; _LL1153: Cyc_Toc_exp_to_c( nv,
_temp1126); if( Cyc_Toc_is_poly_field( struct_type, _temp1142)){ _temp1126= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1126, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp1126, 0), 0), s, 0); goto _LL1145; _LL1145:;}
goto _LL1137;} _LL1141:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1166=( char*)"array designator in struct";
struct _tagged_string _temp1167; _temp1167.curr= _temp1166; _temp1167.base=
_temp1166; _temp1167.last_plus_one= _temp1166 + 27; _temp1167;}))); _LL1137:;}}}
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
_temp1168= es; if( _temp1168 == 0){ _throw( Null_Exception);} _temp1168->tl;}),
-- i){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1188= es; if( _temp1188 == 0){ _throw( Null_Exception);} _temp1188->hd;});
struct Cyc_Absyn_Exp* lval= lval_f( xexp,({ struct _tagged_string* _temp1187=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1187[ 0]= xprintf("f%d", i); _temp1187;}), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)({ struct Cyc_Core_Opt* _temp1169= e->topt; if( _temp1169 == 0){ _throw(
Null_Exception);} _temp1169->v;})): 0;{ void* _temp1170=( void*) e->r; struct
Cyc_List_List* _temp1178; struct Cyc_Absyn_Exp* _temp1180; struct Cyc_Absyn_Exp*
_temp1182; struct Cyc_Absyn_Vardecl* _temp1184; _LL1172: if(*(( int*) _temp1170)
== Cyc_Absyn_Array_e){ _LL1179: _temp1178=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1170)->f1; goto _LL1173;} else{ goto _LL1174;} _LL1174: if(*(( int*)
_temp1170) == Cyc_Absyn_Comprehension_e){ _LL1185: _temp1184=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1170)->f1; goto _LL1183; _LL1183:
_temp1182=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1170)->f2; goto _LL1181; _LL1181: _temp1180=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1170)->f3; goto _LL1175;} else{
goto _LL1176;} _LL1176: goto _LL1177; _LL1173: s= Cyc_Toc_init_array( nv, lval,
_temp1178, s); goto _LL1171; _LL1175: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1184, _temp1182, _temp1180, s, 0); goto _LL1171; _LL1177: Cyc_Toc_exp_to_c(
nv, e); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f(
xexp,({ struct _tagged_string* _temp1186=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1186[ 0]= xprintf("f%d", i); _temp1186;}),
0), e, 0), 0), s, 0); goto _LL1171; _LL1171:;}}} return Cyc_Toc_make_struct( nv,
x, Cyc_Absyn_strct( n), s, pointer, rgnopt, is_atomic);} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_struct( struct Cyc_Toc_Env* nv, void* struct_type, int pointer,
struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* dles, struct _tuple0* tdn){
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(*
lval_f)( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;{
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( dles); for( 0; es != 0; es=({ struct Cyc_List_List* _temp1189= es;
if( _temp1189 == 0){ _throw( Null_Exception);} _temp1189->tl;})){ struct _tuple4
_temp1193; struct Cyc_Absyn_Exp* _temp1194; struct Cyc_List_List* _temp1196;
struct _tuple4* _temp1191=( struct _tuple4*)({ struct Cyc_List_List* _temp1190=
es; if( _temp1190 == 0){ _throw( Null_Exception);} _temp1190->hd;}); _temp1193=*
_temp1191; _LL1197: _temp1196= _temp1193.f1; goto _LL1195; _LL1195: _temp1194=
_temp1193.f2; goto _LL1192; _LL1192: is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)({ struct Cyc_Core_Opt* _temp1198= _temp1194->topt; if( _temp1198 == 0){
_throw( Null_Exception);} _temp1198->v;})): 0; if( _temp1196 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1199=( char*)"empty designator list"; struct _tagged_string _temp1200;
_temp1200.curr= _temp1199; _temp1200.base= _temp1199; _temp1200.last_plus_one=
_temp1199 + 22; _temp1200;}));} if(({ struct Cyc_List_List* _temp1201= _temp1196;
if( _temp1201 == 0){ _throw( Null_Exception);} _temp1201->tl;}) != 0){ struct
_tuple0* y= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* yexp= Cyc_Absyn_var_exp( y,
0); for( 0; _temp1196 != 0; _temp1196=({ struct Cyc_List_List* _temp1202=
_temp1196; if( _temp1202 == 0){ _throw( Null_Exception);} _temp1202->tl;})){
void* _temp1204=( void*)({ struct Cyc_List_List* _temp1203= _temp1196; if(
_temp1203 == 0){ _throw( Null_Exception);} _temp1203->hd;}); struct
_tagged_string* _temp1210; _LL1206: if(*(( int*) _temp1204) == Cyc_Absyn_FieldName){
_LL1211: _temp1210=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1204)->f1; goto _LL1207;} else{ goto _LL1208;} _LL1208: goto _LL1209;
_LL1207: if( Cyc_Toc_is_poly_field( struct_type, _temp1210)){ yexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), yexp, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval_f( xexp, _temp1210, 0), yexp, 0), 0), s, 0); goto
_LL1205; _LL1209:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1212=( char*)"array designator in struct";
struct _tagged_string _temp1213; _temp1213.curr= _temp1212; _temp1213.base=
_temp1212; _temp1213.last_plus_one= _temp1212 + 27; _temp1213;}))); _LL1205:;}
Cyc_Toc_exp_to_c( nv, _temp1194); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
yexp, _temp1194, 0), 0), s, 0);} else{ void* _temp1215=( void*)({ struct Cyc_List_List*
_temp1214= _temp1196; if( _temp1214 == 0){ _throw( Null_Exception);} _temp1214->hd;});
struct _tagged_string* _temp1221; _LL1217: if(*(( int*) _temp1215) == Cyc_Absyn_FieldName){
_LL1222: _temp1221=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1215)->f1; goto _LL1218;} else{ goto _LL1219;} _LL1219: goto _LL1220;
_LL1218: { struct Cyc_Absyn_Exp* lval= lval_f( xexp, _temp1221, 0);{ void*
_temp1223=( void*) _temp1194->r; struct Cyc_List_List* _temp1233; struct Cyc_Absyn_Exp*
_temp1235; struct Cyc_Absyn_Exp* _temp1237; struct Cyc_Absyn_Vardecl* _temp1239;
struct Cyc_List_List* _temp1241; void* _temp1243; _LL1225: if(*(( int*)
_temp1223) == Cyc_Absyn_Array_e){ _LL1234: _temp1233=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1223)->f1; goto _LL1226;} else{ goto
_LL1227;} _LL1227: if(*(( int*) _temp1223) == Cyc_Absyn_Comprehension_e){
_LL1240: _temp1239=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1223)->f1; goto _LL1238; _LL1238: _temp1237=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1223)->f2; goto _LL1236; _LL1236:
_temp1235=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1223)->f3; goto _LL1228;} else{ goto _LL1229;} _LL1229: if(*(( int*)
_temp1223) == Cyc_Absyn_AnonStruct_e){ _LL1244: _temp1243=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1223)->f1; goto _LL1242; _LL1242: _temp1241=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStruct_e_struct*) _temp1223)->f2; goto _LL1230;} else{ goto
_LL1231;} _LL1231: goto _LL1232; _LL1226: s= Cyc_Toc_init_array( nv, lval,
_temp1233, s); goto _LL1224; _LL1228: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1239, _temp1237, _temp1235, s, 0); goto _LL1224; _LL1230: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp1243, _temp1241, s); goto _LL1224; _LL1232: Cyc_Toc_exp_to_c( nv,
_temp1194); if( Cyc_Toc_is_poly_field( struct_type, _temp1221)){ _temp1194= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1194, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp1194, 0), 0), s, 0); goto _LL1224; _LL1224:;}
goto _LL1216;} _LL1220:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1245=( char*)"array designator in struct";
struct _tagged_string _temp1246; _temp1246.curr= _temp1245; _temp1246.base=
_temp1245; _temp1246.last_plus_one= _temp1245 + 27; _temp1246;}))); _LL1216:;}}}
return Cyc_Toc_make_struct( nv, x, Cyc_Absyn_strctq( tdn), s, pointer, rgnopt,
is_atomic);} struct _tuple11{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2;
} ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp*
el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2,
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp*
e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Increment_e_struct* _temp1247=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp1247[ 0]=({
struct Cyc_Absyn_Increment_e_struct _temp1248; _temp1248.tag= Cyc_Absyn_Increment_e;
_temp1248.f1= e1; _temp1248.f2=( void*) incr; _temp1248;}); _temp1247;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp1249=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp1259; struct Cyc_Absyn_Exp*
_temp1261; void* _temp1263; struct _tagged_string* _temp1265; struct Cyc_Absyn_Exp*
_temp1267; _LL1251: if(*(( int*) _temp1249) == Cyc_Absyn_StmtExp_e){ _LL1260:
_temp1259=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1249)->f1; goto _LL1252;} else{ goto _LL1253;} _LL1253: if(*(( int*)
_temp1249) == Cyc_Absyn_Cast_e){ _LL1264: _temp1263=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1249)->f1; goto _LL1262; _LL1262: _temp1261=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Cast_e_struct*) _temp1249)->f2; goto _LL1254;} else{ goto
_LL1255;} _LL1255: if(*(( int*) _temp1249) == Cyc_Absyn_StructMember_e){ _LL1268:
_temp1267=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1249)->f1; goto _LL1266; _LL1266: _temp1265=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1249)->f2; goto _LL1256;} else{
goto _LL1257;} _LL1257: goto _LL1258; _LL1252:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1259, fs, f, f_env);
goto _LL1250; _LL1254:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1261, fs, f, f_env); goto _LL1250; _LL1256:( void*)( e1->r=( void*)(( void*)
_temp1267->r));(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
e1,({ struct Cyc_List_List* _temp1269=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1269->hd=( void*) _temp1265; _temp1269->tl=
fs; _temp1269;}), f, f_env); goto _LL1250; _LL1258: if( ! Cyc_Absyn_is_lvalue(
e1)){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1270= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1270.last_plus_one - _temp1270.curr, _temp1270.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp1271= fs; if( _temp1271 == 0){ _throw( Null_Exception);} _temp1271->tl;})){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_string*)({ struct
Cyc_List_List* _temp1272= fs; if( _temp1272 == 0){ _throw( Null_Exception);}
_temp1272->hd;}), e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy,
f_env))->r)); goto _LL1250;} _LL1250:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp1273=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1283; struct Cyc_Absyn_Stmt* _temp1285; struct Cyc_Absyn_Decl*
_temp1287; struct Cyc_Absyn_Stmt* _temp1289; struct Cyc_Absyn_Stmt* _temp1291;
_LL1275: if(( unsigned int) _temp1273 > 1u?*(( int*) _temp1273) == Cyc_Absyn_Exp_s:
0){ _LL1284: _temp1283=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
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
_temp1293.curr);})); goto _LL1274; _LL1274:;} static struct Cyc_Absyn_Exp* Cyc_Toc_known_size_subscript(
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
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1294=
Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s", _temp1294.last_plus_one
- _temp1294.curr, _temp1294.curr);})); return;}{ void* old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1995= e->topt; if( _temp1995 == 0){ _throw( Null_Exception);}
_temp1995->v;}); void* _temp1295= r; void* _temp1371; void* _temp1373; void*
_temp1375; struct _tuple0* _temp1377; struct _tuple0* _temp1379; struct Cyc_List_List*
_temp1381; void* _temp1383; void* _temp1385; struct Cyc_Absyn_Exp* _temp1387;
struct Cyc_Absyn_Exp* _temp1389; struct Cyc_Core_Opt* _temp1391; struct Cyc_Absyn_Exp*
_temp1393; struct Cyc_Absyn_Exp* _temp1395; struct Cyc_Absyn_Exp* _temp1397;
struct Cyc_Absyn_Exp* _temp1399; struct Cyc_Absyn_Exp* _temp1401; struct Cyc_Absyn_Exp*
_temp1403; struct Cyc_List_List* _temp1405; struct Cyc_Absyn_Exp* _temp1407;
struct Cyc_List_List* _temp1409; struct Cyc_Absyn_Exp* _temp1411; struct Cyc_Absyn_Exp*
_temp1413; struct Cyc_Absyn_Exp* _temp1415; struct Cyc_List_List* _temp1417;
struct Cyc_Absyn_Exp* _temp1419; struct Cyc_Absyn_Exp* _temp1421; struct Cyc_Absyn_Exp**
_temp1423; void* _temp1424; void** _temp1426; struct Cyc_Absyn_Exp* _temp1427;
struct Cyc_Absyn_Exp* _temp1429; struct Cyc_Absyn_Exp* _temp1431; void*
_temp1433; struct Cyc_Absyn_Exp* _temp1435; struct Cyc_Absyn_Exp* _temp1437;
struct _tagged_string* _temp1439; struct Cyc_Absyn_Exp* _temp1441; struct
_tagged_string* _temp1443; struct Cyc_Absyn_Exp* _temp1445; struct Cyc_Absyn_Exp*
_temp1447; struct Cyc_Absyn_Exp* _temp1449; struct Cyc_List_List* _temp1451;
struct Cyc_List_List* _temp1453; struct _tuple1* _temp1455; struct Cyc_List_List*
_temp1457; struct Cyc_Absyn_Exp* _temp1459; struct Cyc_Absyn_Exp* _temp1461;
struct Cyc_Absyn_Vardecl* _temp1463; struct Cyc_Absyn_Structdecl* _temp1465;
struct Cyc_List_List* _temp1467; struct Cyc_Core_Opt* _temp1469; struct _tuple0*
_temp1471; struct Cyc_List_List* _temp1473; void* _temp1475; struct Cyc_Absyn_Tunionfield*
_temp1477; struct Cyc_Absyn_Tuniondecl* _temp1479; struct Cyc_List_List*
_temp1481; struct Cyc_Core_Opt* _temp1483; struct Cyc_Core_Opt* _temp1485;
struct Cyc_Absyn_Tunionfield* _temp1487; struct Cyc_Absyn_Tuniondecl* _temp1489;
struct Cyc_List_List* _temp1491; struct Cyc_Core_Opt* _temp1493; struct Cyc_Core_Opt*
_temp1495; struct Cyc_Absyn_Enumfield* _temp1497; struct Cyc_Absyn_Enumdecl*
_temp1499; struct _tuple0* _temp1501; void* _temp1503; struct Cyc_Absyn_Exp*
_temp1505; struct Cyc_Absyn_Stmt* _temp1507; struct Cyc_List_List* _temp1509;
struct Cyc_Core_Opt* _temp1511; struct Cyc_Absyn_Fndecl* _temp1513; struct Cyc_Absyn_Exp*
_temp1515; _LL1297: if(*(( int*) _temp1295) == Cyc_Absyn_Const_e){ _LL1372:
_temp1371=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1295)->f1; if(
_temp1371 ==( void*) Cyc_Absyn_Null_c){ goto _LL1298;} else{ goto _LL1299;}}
else{ goto _LL1299;} _LL1299: if(*(( int*) _temp1295) == Cyc_Absyn_Const_e){
_LL1374: _temp1373=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1295)->f1;
goto _LL1300;} else{ goto _LL1301;} _LL1301: if(*(( int*) _temp1295) == Cyc_Absyn_Var_e){
_LL1378: _temp1377=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1295)->f1; goto _LL1376; _LL1376: _temp1375=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1295)->f2; goto _LL1302;} else{ goto _LL1303;} _LL1303: if(*(( int*)
_temp1295) == Cyc_Absyn_UnknownId_e){ _LL1380: _temp1379=( struct _tuple0*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1295)->f1; goto _LL1304;} else{ goto
_LL1305;} _LL1305: if(*(( int*) _temp1295) == Cyc_Absyn_Primop_e){ _LL1384:
_temp1383=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1295)->f1; goto
_LL1382; _LL1382: _temp1381=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1295)->f2; goto _LL1306;} else{ goto _LL1307;} _LL1307: if(*(( int*)
_temp1295) == Cyc_Absyn_Increment_e){ _LL1388: _temp1387=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp1295)->f1; goto _LL1386; _LL1386:
_temp1385=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1295)->f2; goto
_LL1308;} else{ goto _LL1309;} _LL1309: if(*(( int*) _temp1295) == Cyc_Absyn_AssignOp_e){
_LL1394: _temp1393=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1295)->f1; goto _LL1392; _LL1392: _temp1391=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1295)->f2; goto _LL1390; _LL1390:
_temp1389=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1295)->f3; goto _LL1310;} else{ goto _LL1311;} _LL1311: if(*(( int*)
_temp1295) == Cyc_Absyn_Conditional_e){ _LL1400: _temp1399=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1295)->f1; goto _LL1398; _LL1398:
_temp1397=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1295)->f2; goto _LL1396; _LL1396: _temp1395=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1295)->f3; goto _LL1312;} else{
goto _LL1313;} _LL1313: if(*(( int*) _temp1295) == Cyc_Absyn_SeqExp_e){ _LL1404:
_temp1403=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1295)->f1; goto _LL1402; _LL1402: _temp1401=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1295)->f2; goto _LL1314;} else{ goto
_LL1315;} _LL1315: if(*(( int*) _temp1295) == Cyc_Absyn_UnknownCall_e){ _LL1408:
_temp1407=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1295)->f1; goto _LL1406; _LL1406: _temp1405=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1295)->f2; goto _LL1316;} else{
goto _LL1317;} _LL1317: if(*(( int*) _temp1295) == Cyc_Absyn_FnCall_e){ _LL1412:
_temp1411=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1295)->f1; goto _LL1410; _LL1410: _temp1409=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1295)->f2; goto _LL1318;} else{ goto
_LL1319;} _LL1319: if(*(( int*) _temp1295) == Cyc_Absyn_Throw_e){ _LL1414:
_temp1413=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1295)->f1;
goto _LL1320;} else{ goto _LL1321;} _LL1321: if(*(( int*) _temp1295) == Cyc_Absyn_NoInstantiate_e){
_LL1416: _temp1415=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1295)->f1; goto _LL1322;} else{ goto _LL1323;} _LL1323: if(*(( int*)
_temp1295) == Cyc_Absyn_Instantiate_e){ _LL1420: _temp1419=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1295)->f1; goto _LL1418; _LL1418:
_temp1417=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1295)->f2; goto _LL1324;} else{ goto _LL1325;} _LL1325: if(*(( int*)
_temp1295) == Cyc_Absyn_Cast_e){ _LL1425: _temp1424=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1295)->f1; _temp1426=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1295)->f1;
goto _LL1422; _LL1422: _temp1421=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1295)->f2; _temp1423=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1295)->f2;
goto _LL1326;} else{ goto _LL1327;} _LL1327: if(*(( int*) _temp1295) == Cyc_Absyn_Address_e){
_LL1428: _temp1427=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp1295)->f1; goto _LL1328;} else{ goto _LL1329;} _LL1329: if(*(( int*)
_temp1295) == Cyc_Absyn_New_e){ _LL1432: _temp1431=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1295)->f1; goto _LL1430; _LL1430: _temp1429=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1295)->f2; goto
_LL1330;} else{ goto _LL1331;} _LL1331: if(*(( int*) _temp1295) == Cyc_Absyn_Sizeoftyp_e){
_LL1434: _temp1433=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1295)->f1;
goto _LL1332;} else{ goto _LL1333;} _LL1333: if(*(( int*) _temp1295) == Cyc_Absyn_Sizeofexp_e){
_LL1436: _temp1435=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1295)->f1; goto _LL1334;} else{ goto _LL1335;} _LL1335: if(*(( int*)
_temp1295) == Cyc_Absyn_Deref_e){ _LL1438: _temp1437=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp1295)->f1; goto _LL1336;} else{ goto
_LL1337;} _LL1337: if(*(( int*) _temp1295) == Cyc_Absyn_StructMember_e){ _LL1442:
_temp1441=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1295)->f1; goto _LL1440; _LL1440: _temp1439=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1295)->f2; goto _LL1338;} else{
goto _LL1339;} _LL1339: if(*(( int*) _temp1295) == Cyc_Absyn_StructArrow_e){
_LL1446: _temp1445=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1295)->f1; goto _LL1444; _LL1444: _temp1443=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1295)->f2; goto _LL1340;} else{
goto _LL1341;} _LL1341: if(*(( int*) _temp1295) == Cyc_Absyn_Subscript_e){
_LL1450: _temp1449=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1295)->f1; goto _LL1448; _LL1448: _temp1447=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1295)->f2; goto _LL1342;} else{ goto
_LL1343;} _LL1343: if(*(( int*) _temp1295) == Cyc_Absyn_Tuple_e){ _LL1452:
_temp1451=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1295)->f1;
goto _LL1344;} else{ goto _LL1345;} _LL1345: if(*(( int*) _temp1295) == Cyc_Absyn_CompoundLit_e){
_LL1456: _temp1455=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1295)->f1; goto _LL1454; _LL1454: _temp1453=( struct Cyc_List_List*)((
struct Cyc_Absyn_CompoundLit_e_struct*) _temp1295)->f2; goto _LL1346;} else{
goto _LL1347;} _LL1347: if(*(( int*) _temp1295) == Cyc_Absyn_Array_e){ _LL1458:
_temp1457=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1295)->f1;
goto _LL1348;} else{ goto _LL1349;} _LL1349: if(*(( int*) _temp1295) == Cyc_Absyn_Comprehension_e){
_LL1464: _temp1463=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1295)->f1; goto _LL1462; _LL1462: _temp1461=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1295)->f2; goto _LL1460; _LL1460:
_temp1459=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1295)->f3; goto _LL1350;} else{ goto _LL1351;} _LL1351: if(*(( int*)
_temp1295) == Cyc_Absyn_Struct_e){ _LL1472: _temp1471=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1295)->f1; goto _LL1470; _LL1470:
_temp1469=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1295)->f2;
goto _LL1468; _LL1468: _temp1467=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1295)->f3; goto _LL1466; _LL1466: _temp1465=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1295)->f4; goto _LL1352;} else{ goto
_LL1353;} _LL1353: if(*(( int*) _temp1295) == Cyc_Absyn_AnonStruct_e){ _LL1476:
_temp1475=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1295)->f1; goto
_LL1474; _LL1474: _temp1473=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1295)->f2; goto _LL1354;} else{ goto _LL1355;} _LL1355: if(*(( int*)
_temp1295) == Cyc_Absyn_Tunion_e){ _LL1486: _temp1485=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1295)->f1; goto _LL1484; _LL1484:
_temp1483=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1295)->f2;
goto _LL1482; _LL1482: _temp1481=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1295)->f3; if( _temp1481 == 0){ goto _LL1480;} else{ goto _LL1357;} _LL1480:
_temp1479=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1295)->f4; goto _LL1478; _LL1478: _temp1477=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1295)->f5; goto _LL1356;} else{ goto
_LL1357;} _LL1357: if(*(( int*) _temp1295) == Cyc_Absyn_Tunion_e){ _LL1496:
_temp1495=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1295)->f1;
goto _LL1494; _LL1494: _temp1493=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1295)->f2; goto _LL1492; _LL1492: _temp1491=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1295)->f3; goto _LL1490; _LL1490:
_temp1489=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1295)->f4; goto _LL1488; _LL1488: _temp1487=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1295)->f5; goto _LL1358;} else{ goto
_LL1359;} _LL1359: if(*(( int*) _temp1295) == Cyc_Absyn_Enum_e){ _LL1502:
_temp1501=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1295)->f1;
goto _LL1500; _LL1500: _temp1499=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1295)->f2; goto _LL1498; _LL1498: _temp1497=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1295)->f3; goto _LL1360;} else{ goto
_LL1361;} _LL1361: if(*(( int*) _temp1295) == Cyc_Absyn_Malloc_e){ _LL1506:
_temp1505=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1295)->f1; goto _LL1504; _LL1504: _temp1503=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1295)->f2; goto _LL1362;} else{ goto _LL1363;} _LL1363: if(*(( int*)
_temp1295) == Cyc_Absyn_StmtExp_e){ _LL1508: _temp1507=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1295)->f1; goto _LL1364;} else{ goto
_LL1365;} _LL1365: if(*(( int*) _temp1295) == Cyc_Absyn_UnresolvedMem_e){
_LL1512: _temp1511=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1295)->f1; goto _LL1510; _LL1510: _temp1509=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1295)->f2; goto _LL1366;} else{
goto _LL1367;} _LL1367: if(*(( int*) _temp1295) == Cyc_Absyn_Codegen_e){ _LL1514:
_temp1513=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1295)->f1; goto _LL1368;} else{ goto _LL1369;} _LL1369: if(*(( int*)
_temp1295) == Cyc_Absyn_Fill_e){ _LL1516: _temp1515=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp1295)->f1; goto _LL1370;} else{ goto
_LL1296;} _LL1298:{ void* _temp1517= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1523; struct Cyc_Absyn_Conref* _temp1525; struct Cyc_Absyn_Tqual _temp1527;
struct Cyc_Absyn_Conref* _temp1529; void* _temp1531; void* _temp1533; _LL1519:
if(( unsigned int) _temp1517 > 4u?*(( int*) _temp1517) == Cyc_Absyn_PointerType:
0){ _LL1524: _temp1523=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1517)->f1; _LL1534: _temp1533=( void*) _temp1523.elt_typ; goto _LL1532;
_LL1532: _temp1531=( void*) _temp1523.rgn_typ; goto _LL1530; _LL1530: _temp1529=(
struct Cyc_Absyn_Conref*) _temp1523.nullable; goto _LL1528; _LL1528: _temp1527=(
struct Cyc_Absyn_Tqual) _temp1523.tq; goto _LL1526; _LL1526: _temp1525=( struct
Cyc_Absyn_Conref*) _temp1523.bounds; goto _LL1520;} else{ goto _LL1521;} _LL1521:
goto _LL1522; _LL1520: if(( void*) _temp1525->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1518;}{ void* _temp1535=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1525); struct Cyc_Absyn_Exp* _temp1541; _LL1537: if( _temp1535 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1538;} else{ goto _LL1539;} _LL1539: if((
unsigned int) _temp1535 > 1u?*(( int*) _temp1535) == Cyc_Absyn_Upper_b: 0){
_LL1542: _temp1541=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1535)->f1; goto _LL1540;} else{ goto _LL1536;} _LL1538: { struct _tuple4*
zero=({ struct _tuple4* _temp1546=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1546->f1= 0; _temp1546->f2= Cyc_Absyn_signed_int_exp( 0, 0);
_temp1546;}); struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp( 0,({ struct
Cyc_List_List* _temp1543=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1543->hd=( void*) zero; _temp1543->tl=({ struct Cyc_List_List* _temp1544=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1544->hd=(
void*) zero; _temp1544->tl=({ struct Cyc_List_List* _temp1545=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1545->hd=( void*) zero;
_temp1545->tl= 0; _temp1545;}); _temp1544;}); _temp1543;}), 0); if( nv->toplevel){(
void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct( Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1533), Cyc_Toc_mt_tq)); struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*) um, Cyc_Absyn_exp_stmt(
xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}
goto _LL1536;} _LL1540:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1536; _LL1536:;} goto _LL1518; _LL1522:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1547= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1547.last_plus_one -
_temp1547.curr, _temp1547.curr);})); goto _LL1518; _LL1518:;} goto _LL1296;
_LL1300: goto _LL1296; _LL1302:{ struct _handler_cons _temp1548; _push_handler(&
_temp1548);{ void* _temp1549=( void*) setjmp( _temp1548.handler); if( !
_temp1549){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap, _temp1377))->r));;
_pop_handler();} else{ void* _temp1551= _temp1549; _LL1553: if( _temp1551 == Cyc_Dict_Absent){
goto _LL1554;} else{ goto _LL1555;} _LL1555: goto _LL1556; _LL1554:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1557=
Cyc_Absynpp_qvar2string( _temp1377); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1557.last_plus_one - _temp1557.curr, _temp1557.curr);})); return; _LL1556:(
void) _throw( _temp1551); _LL1552:;}}} goto _LL1296; _LL1304:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1558=(
char*)"unknownid"; struct _tagged_string _temp1559; _temp1559.curr= _temp1558;
_temp1559.base= _temp1558; _temp1559.last_plus_one= _temp1558 + 10; _temp1559;}));
goto _LL1296; _LL1306: { struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1381); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1381);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1381);{ void* _temp1560= _temp1383; _LL1562: if( _temp1560 ==( void*)
Cyc_Absyn_Size){ goto _LL1563;} else{ goto _LL1564;} _LL1564: if( _temp1560 ==(
void*) Cyc_Absyn_Printf){ goto _LL1565;} else{ goto _LL1566;} _LL1566: if(
_temp1560 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1567;} else{ goto _LL1568;}
_LL1568: if( _temp1560 ==( void*) Cyc_Absyn_Fprintf){ goto _LL1569;} else{ goto
_LL1570;} _LL1570: if( _temp1560 ==( void*) Cyc_Absyn_Sscanf){ goto _LL1571;}
else{ goto _LL1572;} _LL1572: if( _temp1560 ==( void*) Cyc_Absyn_Plus){ goto
_LL1573;} else{ goto _LL1574;} _LL1574: if( _temp1560 ==( void*) Cyc_Absyn_Minus){
goto _LL1575;} else{ goto _LL1576;} _LL1576: if( _temp1560 ==( void*) Cyc_Absyn_Eq){
goto _LL1577;} else{ goto _LL1578;} _LL1578: if( _temp1560 ==( void*) Cyc_Absyn_Neq){
goto _LL1579;} else{ goto _LL1580;} _LL1580: if( _temp1560 ==( void*) Cyc_Absyn_Gt){
goto _LL1581;} else{ goto _LL1582;} _LL1582: if( _temp1560 ==( void*) Cyc_Absyn_Gte){
goto _LL1583;} else{ goto _LL1584;} _LL1584: if( _temp1560 ==( void*) Cyc_Absyn_Lt){
goto _LL1585;} else{ goto _LL1586;} _LL1586: if( _temp1560 ==( void*) Cyc_Absyn_Lte){
goto _LL1587;} else{ goto _LL1588;} _LL1588: goto _LL1589; _LL1563: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1623= _temp1381; if(
_temp1623 == 0){ _throw( Null_Exception);} _temp1623->hd;});{ void* _temp1591=(
void*)({ struct Cyc_Core_Opt* _temp1590= arg->topt; if( _temp1590 == 0){ _throw(
Null_Exception);} _temp1590->v;}); struct Cyc_Absyn_Exp* _temp1599; struct Cyc_Absyn_Tqual
_temp1601; void* _temp1603; struct Cyc_Absyn_PtrInfo _temp1605; struct Cyc_Absyn_Conref*
_temp1607; struct Cyc_Absyn_Tqual _temp1609; struct Cyc_Absyn_Conref* _temp1611;
void* _temp1613; void* _temp1615; _LL1593: if(( unsigned int) _temp1591 > 4u?*((
int*) _temp1591) == Cyc_Absyn_ArrayType: 0){ _LL1604: _temp1603=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1591)->f1; goto _LL1602; _LL1602:
_temp1601=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1591)->f2; goto _LL1600; _LL1600: _temp1599=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1591)->f3; goto _LL1594;} else{ goto
_LL1595;} _LL1595: if(( unsigned int) _temp1591 > 4u?*(( int*) _temp1591) == Cyc_Absyn_PointerType:
0){ _LL1606: _temp1605=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1591)->f1; _LL1616: _temp1615=( void*) _temp1605.elt_typ; goto _LL1614;
_LL1614: _temp1613=( void*) _temp1605.rgn_typ; goto _LL1612; _LL1612: _temp1611=(
struct Cyc_Absyn_Conref*) _temp1605.nullable; goto _LL1610; _LL1610: _temp1609=(
struct Cyc_Absyn_Tqual) _temp1605.tq; goto _LL1608; _LL1608: _temp1607=( struct
Cyc_Absyn_Conref*) _temp1605.bounds; goto _LL1596;} else{ goto _LL1597;} _LL1597:
goto _LL1598; _LL1594:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Exp*
_temp1617= _temp1599; if( _temp1617 == 0){ _throw( Null_Exception);} _temp1617->r;})));
goto _LL1592; _LL1596: { struct _tuple0* temp= Cyc_Toc_temp_var(); void*
temp_typ= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1618= arg->topt;
if( _temp1618 == 0){ _throw( Null_Exception);} _temp1618->v;})); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
Cyc_Absyn_subtract_exp( e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt(
e4, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1592;}
_LL1598:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1621= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_List_List*
_temp1619= typs; if( _temp1619 == 0){ _throw( Null_Exception);} _temp1619->hd;}));
struct _tagged_string _temp1622= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1620= arg->topt; if( _temp1620 == 0){ _throw( Null_Exception);} _temp1620->v;}));
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1621.last_plus_one
- _temp1621.curr, _temp1621.curr, _temp1622.last_plus_one - _temp1622.curr,
_temp1622.curr);})); return; _LL1592:;} goto _LL1561;} _LL1565:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1383, 0, _temp1381, typs))->r)); goto
_LL1561; _LL1567:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1383, 0, _temp1381, typs))->r)); goto _LL1561; _LL1569: if( _temp1381 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1624=( char*)"fprintf without arguments"; struct _tagged_string
_temp1625; _temp1625.curr= _temp1624; _temp1625.base= _temp1624; _temp1625.last_plus_one=
_temp1624 + 26; _temp1625;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1383,({ struct Cyc_Core_Opt* _temp1626=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1626->v=( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1627= _temp1381; if( _temp1627 == 0){ _throw(
Null_Exception);} _temp1627->hd;})); _temp1626;}),({ struct Cyc_List_List*
_temp1628= _temp1381; if( _temp1628 == 0){ _throw( Null_Exception);} _temp1628->tl;}),({
struct Cyc_List_List* _temp1629= typs; if( _temp1629 == 0){ _throw(
Null_Exception);} _temp1629->tl;})))->r)); goto _LL1561; _LL1571: if( _temp1381
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1630=( char*)"sscanf without arguments"; struct
_tagged_string _temp1631; _temp1631.curr= _temp1630; _temp1631.base= _temp1630;
_temp1631.last_plus_one= _temp1630 + 25; _temp1631;}));}( void*)((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1632= _temp1381; if( _temp1632 == 0){ _throw(
Null_Exception);} _temp1632->hd;}))->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1633= _temp1381; if( _temp1633 == 0){ _throw( Null_Exception);} _temp1633->hd;}))->r,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1634= _temp1381; if(
_temp1634 == 0){ _throw( Null_Exception);} _temp1634->hd;}))->loc), Cyc_Toc_curr_sp,
0))->r)); goto _LL1561; _LL1573: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1638= _temp1381; if( _temp1638 == 0){ _throw( Null_Exception);} _temp1638->hd;});
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1637=({ struct Cyc_List_List* _temp1636= _temp1381; if( _temp1636 == 0){
_throw( Null_Exception);} _temp1636->tl;}); if( _temp1637 == 0){ _throw(
Null_Exception);} _temp1637->hd;}); struct _tuple0* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1635=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1635->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1635;}),
e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1561; _LL1575: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)({ struct Cyc_List_List* _temp1639= old_typs; if( _temp1639 == 0){ _throw(
Null_Exception);} _temp1639->hd;}))){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1646= _temp1381; if( _temp1646 == 0){ _throw(
Null_Exception);} _temp1646->hd;}); struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1645=({ struct Cyc_List_List* _temp1644= _temp1381;
if( _temp1644 == 0){ _throw( Null_Exception);} _temp1644->tl;}); if( _temp1645
== 0){ _throw( Null_Exception);} _temp1645->hd;}); void* old_typ_e2=( void*)({
struct Cyc_List_List* _temp1643=({ struct Cyc_List_List* _temp1642= old_typs;
if( _temp1642 == 0){ _throw( Null_Exception);} _temp1642->tl;}); if( _temp1643
== 0){ _throw( Null_Exception);} _temp1643->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_List_List* _temp1641= old_typs; if( _temp1641 == 0){ _throw(
Null_Exception);} _temp1641->hd;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1640=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1640->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp1640;}), e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0);
s= Cyc_Absyn_declare_stmt( temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1561;
_LL1577: goto _LL1579; _LL1579: goto _LL1581; _LL1581: goto _LL1583; _LL1583:
goto _LL1585; _LL1585: goto _LL1587; _LL1587: { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1652= _temp1381; if(
_temp1652 == 0){ _throw( Null_Exception);} _temp1652->hd;}); struct Cyc_Absyn_Exp*
e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1651=({ struct Cyc_List_List*
_temp1650= _temp1381; if( _temp1650 == 0){ _throw( Null_Exception);} _temp1650->tl;});
if( _temp1651 == 0){ _throw( Null_Exception);} _temp1651->hd;}); void* t1=( void*)({
struct Cyc_List_List* _temp1649= old_typs; if( _temp1649 == 0){ _throw(
Null_Exception);} _temp1649->hd;}); void* t2=( void*)({ struct Cyc_List_List*
_temp1648=({ struct Cyc_List_List* _temp1647= old_typs; if( _temp1647 == 0){
_throw( Null_Exception);} _temp1647->tl;}); if( _temp1648 == 0){ _throw(
Null_Exception);} _temp1648->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1561;} _LL1589: goto
_LL1561; _LL1561:;} goto _LL1296;} _LL1308: { void* e2_cyc_typ=( void*)({ struct
Cyc_Core_Opt* _temp1653= _temp1387->topt; if( _temp1653 == 0){ _throw(
Null_Exception);} _temp1653->v;}); Cyc_Toc_exp_to_c( nv, _temp1387); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){ if( _temp1385 ==( void*) Cyc_Absyn_PreInc? 1: _temp1385 ==( void*)
Cyc_Absyn_PreDec){ struct _tuple0* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Absyn_star_typ(
Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
temp, 0), 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0), _temp1385, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp, temp_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1387, 0), s, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));} else{ struct _tuple0* temp=
Cyc_Toc_temp_var(); struct _tuple0* temp2= Cyc_Toc_temp_var(); void* temp2_typ=
Cyc_Toc_typ_to_c( e2_cyc_typ); void* temp_typ= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c(
e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp2, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( temp,
0), Cyc_Toc_curr_sp, 0), _temp1385, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp2, temp2_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( temp, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1387, 0), s,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} else{
if( ! Cyc_Absyn_is_lvalue( _temp1387)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1387, 0, Cyc_Toc_incr_lvalue,
_temp1385);( void*)( e->r=( void*)(( void*) _temp1387->r));}} goto _LL1296;}
_LL1310: { int e1_poly= Cyc_Toc_is_poly_project( _temp1393); void* e1_old_typ=(
void*)({ struct Cyc_Core_Opt* _temp1659= _temp1393->topt; if( _temp1659 == 0){
_throw( Null_Exception);} _temp1659->v;}); void* e2_old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1658= _temp1389->topt; if( _temp1658 == 0){ _throw(
Null_Exception);} _temp1658->v;}); Cyc_Toc_exp_to_c( nv, _temp1393); Cyc_Toc_exp_to_c(
nv, _temp1389); if( _temp1391 != 0?( void*)({ struct Cyc_Core_Opt* _temp1654=
_temp1391; if( _temp1654 == 0){ _throw( Null_Exception);} _temp1654->v;}) ==(
void*) Cyc_Absyn_Plus? 1:( void*)({ struct Cyc_Core_Opt* _temp1655= _temp1391;
if( _temp1655 == 0){ _throw( Null_Exception);} _temp1655->v;}) ==( void*) Cyc_Absyn_Minus:
0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){( void*)( _temp1393->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)
_temp1393->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1389->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1389->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1389->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1389->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1393)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1393, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1656=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1656[ 0]=({ struct _tuple11 _temp1657; _temp1657.f1=
_temp1391; _temp1657.f2= _temp1389; _temp1657;}); _temp1656;}));( void*)( e->r=(
void*)(( void*) _temp1393->r));} goto _LL1296;} _LL1312: Cyc_Toc_exp_to_c( nv,
_temp1399); Cyc_Toc_exp_to_c( nv, _temp1397); Cyc_Toc_exp_to_c( nv, _temp1395);
goto _LL1296; _LL1314: Cyc_Toc_exp_to_c( nv, _temp1403); Cyc_Toc_exp_to_c( nv,
_temp1401); goto _LL1296; _LL1316: _temp1411= _temp1407; _temp1409= _temp1405;
goto _LL1318; _LL1318: Cyc_Toc_exp_to_c( nv, _temp1411);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1409); goto _LL1296; _LL1320:
Cyc_Toc_exp_to_c( nv, _temp1413);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1413), 0))->r)); goto
_LL1296; _LL1322: Cyc_Toc_exp_to_c( nv, _temp1415); goto _LL1296; _LL1324: Cyc_Toc_exp_to_c(
nv, _temp1419);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1662= e->topt; if( _temp1662 == 0){ _throw( Null_Exception);} _temp1662->v;}));
for( 0; _temp1417 != 0; _temp1417=({ struct Cyc_List_List* _temp1660= _temp1417;
if( _temp1660 == 0){ _throw( Null_Exception);} _temp1660->tl;})){ void* k= Cyc_Tcutil_typ_kind((
void*)({ struct Cyc_List_List* _temp1661= _temp1417; if( _temp1661 == 0){ _throw(
Null_Exception);} _temp1661->hd;})); if( k !=( void*) Cyc_Absyn_EffKind? k !=(
void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1419, 0))->r)); break;}} goto _LL1296;} _LL1326: { void* old_t2=(
void*)({ struct Cyc_Core_Opt* _temp1663=(* _temp1423)->topt; if( _temp1663 == 0){
_throw( Null_Exception);} _temp1663->v;}); void* new_typ=* _temp1426;* _temp1426=
Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,* _temp1423);* _temp1423= Cyc_Toc_convert_exp(
nv->toplevel, old_t2, new_typ,* _temp1423); goto _LL1296;} _LL1328:{ void*
_temp1664=( void*) _temp1427->r; struct Cyc_Absyn_Structdecl* _temp1672; struct
Cyc_List_List* _temp1674; struct Cyc_Core_Opt* _temp1676; struct _tuple0*
_temp1678; struct Cyc_List_List* _temp1680; _LL1666: if(*(( int*) _temp1664) ==
Cyc_Absyn_Struct_e){ _LL1679: _temp1678=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1664)->f1; goto _LL1677; _LL1677: _temp1676=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1664)->f2; goto _LL1675; _LL1675:
_temp1674=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1664)->f3; goto _LL1673; _LL1673: _temp1672=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1664)->f4; goto _LL1667;} else{ goto
_LL1668;} _LL1668: if(*(( int*) _temp1664) == Cyc_Absyn_Tuple_e){ _LL1681:
_temp1680=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1664)->f1;
goto _LL1669;} else{ goto _LL1670;} _LL1670: goto _LL1671; _LL1667: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string
_temp1682= Cyc_Position_string_of_segment( _temp1427->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1682.last_plus_one - _temp1682.curr, _temp1682.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)({ struct Cyc_Core_Opt*
_temp1683= _temp1427->topt; if( _temp1683 == 0){ _throw( Null_Exception);}
_temp1683->v;}), 1, 0, _temp1674, _temp1678))->r)); goto _LL1665; _LL1669: if(
nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct
_tagged_string _temp1684= Cyc_Position_string_of_segment( _temp1427->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1684.last_plus_one -
_temp1684.curr, _temp1684.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, 0, _temp1680))->r)); goto _LL1665; _LL1671: Cyc_Toc_exp_to_c( nv,
_temp1427); if( ! Cyc_Absyn_is_lvalue( _temp1427)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1427, 0, Cyc_Toc_address_lvalue,
1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1685= e->topt; if( _temp1685 == 0){ _throw(
Null_Exception);} _temp1685->v;})), _temp1427, 0))->r));} goto _LL1665; _LL1665:;}
goto _LL1296; _LL1330: if( nv->toplevel){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ struct _tagged_string _temp1686= Cyc_Position_string_of_segment(
_temp1429->loc); xprintf("%.*s: new at top-level", _temp1686.last_plus_one -
_temp1686.curr, _temp1686.curr);}));}{ void* _temp1687=( void*) _temp1429->r;
struct Cyc_List_List* _temp1701; void* _temp1703; struct _tagged_string
_temp1705; struct Cyc_Absyn_Exp* _temp1707; struct Cyc_Absyn_Exp* _temp1709;
struct Cyc_Absyn_Vardecl* _temp1711; struct Cyc_Absyn_Structdecl* _temp1713;
struct Cyc_List_List* _temp1715; struct Cyc_Core_Opt* _temp1717; struct _tuple0*
_temp1719; struct Cyc_List_List* _temp1721; _LL1689: if(*(( int*) _temp1687) ==
Cyc_Absyn_Array_e){ _LL1702: _temp1701=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1687)->f1; goto _LL1690;} else{ goto _LL1691;} _LL1691: if(*(( int*)
_temp1687) == Cyc_Absyn_Const_e){ _LL1704: _temp1703=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1687)->f1; if(( unsigned int) _temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_String_c:
0){ _LL1706: _temp1705=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1703)->f1; goto _LL1692;} else{ goto _LL1693;}} else{ goto _LL1693;}
_LL1693: if(*(( int*) _temp1687) == Cyc_Absyn_Comprehension_e){ _LL1712:
_temp1711=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1687)->f1; goto _LL1710; _LL1710: _temp1709=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1687)->f2; goto _LL1708; _LL1708:
_temp1707=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1687)->f3; goto _LL1694;} else{ goto _LL1695;} _LL1695: if(*(( int*)
_temp1687) == Cyc_Absyn_Struct_e){ _LL1720: _temp1719=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1687)->f1; goto _LL1718; _LL1718:
_temp1717=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1687)->f2;
goto _LL1716; _LL1716: _temp1715=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1687)->f3; goto _LL1714; _LL1714: _temp1713=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1687)->f4; goto _LL1696;} else{ goto
_LL1697;} _LL1697: if(*(( int*) _temp1687) == Cyc_Absyn_Tuple_e){ _LL1722:
_temp1721=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1687)->f1;
goto _LL1698;} else{ goto _LL1699;} _LL1699: goto _LL1700; _LL1690: { struct
_tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x,
0); struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_array( nv, xexp, _temp1701, Cyc_Absyn_exp_stmt(
xexp, 0)); void* old_elt_typ;{ void* _temp1723= Cyc_Tcutil_compress( old_typ);
struct Cyc_Absyn_PtrInfo _temp1729; struct Cyc_Absyn_Conref* _temp1731; struct
Cyc_Absyn_Tqual _temp1733; struct Cyc_Absyn_Conref* _temp1735; void* _temp1737;
void* _temp1739; _LL1725: if(( unsigned int) _temp1723 > 4u?*(( int*) _temp1723)
== Cyc_Absyn_PointerType: 0){ _LL1730: _temp1729=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1723)->f1; _LL1740: _temp1739=( void*)
_temp1729.elt_typ; goto _LL1738; _LL1738: _temp1737=( void*) _temp1729.rgn_typ;
goto _LL1736; _LL1736: _temp1735=( struct Cyc_Absyn_Conref*) _temp1729.nullable;
goto _LL1734; _LL1734: _temp1733=( struct Cyc_Absyn_Tqual) _temp1729.tq; goto
_LL1732; _LL1732: _temp1731=( struct Cyc_Absyn_Conref*) _temp1729.bounds; goto
_LL1726;} else{ goto _LL1727;} _LL1727: goto _LL1728; _LL1726: old_elt_typ=
_temp1739; goto _LL1724; _LL1728: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1741=( char*)"exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1742; _temp1742.curr= _temp1741; _temp1742.base=
_temp1741; _temp1742.last_plus_one= _temp1741 + 52; _temp1742;})); goto _LL1724;
_LL1724:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1701), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1431 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, e2);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1743= _temp1431; if( _temp1743 == 0){ _throw(
Null_Exception);} _temp1743;}); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp(
r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,(
struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1688;}}} _LL1692: if(
_temp1431 == 0){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1744=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1744->f1= Cyc_Toc_abs_ns; _temp1744->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1744;}), 0),({ struct Cyc_List_List* _temp1745=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1745->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1746=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1746[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1747; _temp1747.tag= Cyc_Absyn_Const_e; _temp1747.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1748=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1748[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1749; _temp1749.tag= Cyc_Absyn_String_c;
_temp1749.f1= _temp1705; _temp1749;}); _temp1748;})); _temp1747;}); _temp1746;}),
0); _temp1745->tl= 0; _temp1745;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1756= _temp1431; if(
_temp1756 == 0){ _throw( Null_Exception);} _temp1756;}); Cyc_Toc_exp_to_c( nv, r);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1750=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1750->f1= Cyc_Toc_abs_ns; _temp1750->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1750;}), 0),({ struct Cyc_List_List* _temp1751=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1751->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1752=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1752[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1753; _temp1753.tag= Cyc_Absyn_Const_e; _temp1753.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1754=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1754[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1755; _temp1755.tag= Cyc_Absyn_String_c;
_temp1755.f1= _temp1705; _temp1755;}); _temp1754;})); _temp1753;}); _temp1752;}),
0); _temp1751->tl= 0; _temp1751;}), 0))->r));} goto _LL1688; _LL1694: { int
is_tagged_ptr= 0;{ void* _temp1757= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1763; struct Cyc_Absyn_Conref* _temp1765; struct Cyc_Absyn_Tqual _temp1767;
struct Cyc_Absyn_Conref* _temp1769; void* _temp1771; void* _temp1773; _LL1759:
if(( unsigned int) _temp1757 > 4u?*(( int*) _temp1757) == Cyc_Absyn_PointerType:
0){ _LL1764: _temp1763=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1757)->f1; _LL1774: _temp1773=( void*) _temp1763.elt_typ; goto _LL1772;
_LL1772: _temp1771=( void*) _temp1763.rgn_typ; goto _LL1770; _LL1770: _temp1769=(
struct Cyc_Absyn_Conref*) _temp1763.nullable; goto _LL1768; _LL1768: _temp1767=(
struct Cyc_Absyn_Tqual) _temp1763.tq; goto _LL1766; _LL1766: _temp1765=( struct
Cyc_Absyn_Conref*) _temp1763.bounds; goto _LL1760;} else{ goto _LL1761;} _LL1761:
goto _LL1762; _LL1760:{ void* _temp1775=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1765))->v; void*
_temp1781; _LL1777: if(( unsigned int) _temp1775 > 1u?*(( int*) _temp1775) ==
Cyc_Absyn_Eq_constr: 0){ _LL1782: _temp1781=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1775)->f1; if( _temp1781 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1778;}
else{ goto _LL1779;}} else{ goto _LL1779;} _LL1779: goto _LL1780; _LL1778:
is_tagged_ptr= 1; goto _LL1776; _LL1780: goto _LL1776; _LL1776:;} goto _LL1758;
_LL1762:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1783=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1784; _temp1784.curr= _temp1783; _temp1784.base=
_temp1783; _temp1784.last_plus_one= _temp1783 + 42; _temp1784;})); goto _LL1758;
_LL1758:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)({ struct Cyc_Core_Opt* _temp1808= _temp1707->topt;
if( _temp1808 == 0){ _throw( Null_Exception);} _temp1808->v;}); void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1709);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1711, Cyc_Absyn_var_exp( max, 0), _temp1707,
Cyc_Absyn_skip_stmt( 0), 1); struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1806=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1806->hd=( void*)({ struct _tuple10* _temp1807=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1807->f1= max; _temp1807->f2= Cyc_Absyn_uint_t;
_temp1807->f3=( struct Cyc_Absyn_Exp*) _temp1709; _temp1807;}); _temp1806->tl= 0;
_temp1806;}); struct Cyc_Absyn_Exp* ai; if( _temp1431 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp1785= _temp1431; if( _temp1785 == 0){ _throw( Null_Exception);}
_temp1785;}); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct
Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1786=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1786->hd=( void*)({ struct _tuple10* _temp1787=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1787->f1= a; _temp1787->f2= ptr_typ;
_temp1787->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1787;}); _temp1786->tl= decls;
_temp1786;}); if( is_tagged_ptr){ struct _tuple0* b= Cyc_Toc_temp_var(); void* t=
Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* binit= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp1790=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1790->hd=( void*)({ struct _tuple4* _temp1795=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1795->f1= 0;
_temp1795->f2= Cyc_Absyn_var_exp( a, 0); _temp1795;}); _temp1790->tl=({ struct
Cyc_List_List* _temp1791=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1791->hd=( void*)({ struct _tuple4* _temp1794=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1794->f1= 0; _temp1794->f2= Cyc_Absyn_var_exp( a,
0); _temp1794;}); _temp1791->tl=({ struct Cyc_List_List* _temp1792=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1792->hd=( void*)({ struct
_tuple4* _temp1793=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1793->f1= 0; _temp1793->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1793;}); _temp1792->tl= 0; _temp1792;});
_temp1791;}); _temp1790;}), 0); decls=({ struct Cyc_List_List* _temp1788=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1788->hd=(
void*)({ struct _tuple10* _temp1789=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1789->f1= b; _temp1789->f2= t; _temp1789->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1789;}); _temp1788->tl= decls; _temp1788;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp1796= ds; if(
_temp1796 == 0){ _throw( Null_Exception);} _temp1796->tl;})){ struct Cyc_Absyn_Exp*
_temp1800; void* _temp1802; struct _tuple0* _temp1804; struct _tuple10 _temp1798=*((
struct _tuple10*)({ struct Cyc_List_List* _temp1797= ds; if( _temp1797 == 0){
_throw( Null_Exception);} _temp1797->hd;})); _LL1805: _temp1804= _temp1798.f1;
goto _LL1803; _LL1803: _temp1802= _temp1798.f2; goto _LL1801; _LL1801: _temp1800=
_temp1798.f3; goto _LL1799; _LL1799: s= Cyc_Absyn_declare_stmt( _temp1804,
_temp1802, _temp1800, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r)); goto _LL1688;}}}} _LL1696:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1809= _temp1429->topt; if( _temp1809 ==
0){ _throw( Null_Exception);} _temp1809->v;}), 1, _temp1431, _temp1715,
_temp1719))->r)); goto _LL1688; _LL1698:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1431, _temp1721))->r)); goto _LL1688; _LL1700: { void* old_elt_typ=(
void*)({ struct Cyc_Core_Opt* _temp1811= _temp1429->topt; if( _temp1811 == 0){
_throw( Null_Exception);} _temp1811->v;}); struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0);
struct Cyc_Absyn_Exp* e_index= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c(
nv, _temp1429); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1429, 0), s, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); struct Cyc_Absyn_Exp*
e1; if( _temp1431 == 0){ e1= Cyc_Toc_malloc_exp( old_elt_typ, e2);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1810=
_temp1431; if( _temp1810 == 0){ _throw( Null_Exception);} _temp1810;}); Cyc_Toc_exp_to_c(
nv, r); e1= Cyc_Toc_rmalloc_exp( r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp(
ptr_typ, e1, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, ptr_typ,( struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1688;}}}
_LL1688:;} goto _LL1296; _LL1332:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1812=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1812[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1813; _temp1813.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1813.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1433);
_temp1813;}); _temp1812;}))); goto _LL1296; _LL1334: Cyc_Toc_exp_to_c( nv,
_temp1435); goto _LL1296; _LL1336: { void* e1_typ= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp1843= _temp1437->topt; if( _temp1843 == 0){ _throw(
Null_Exception);} _temp1843->v;})); Cyc_Toc_exp_to_c( nv, _temp1437);{ void*
_temp1814= e1_typ; struct Cyc_Absyn_PtrInfo _temp1820; struct Cyc_Absyn_Conref*
_temp1822; struct Cyc_Absyn_Tqual _temp1824; struct Cyc_Absyn_Conref* _temp1826;
void* _temp1828; void* _temp1830; _LL1816: if(( unsigned int) _temp1814 > 4u?*((
int*) _temp1814) == Cyc_Absyn_PointerType: 0){ _LL1821: _temp1820=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1814)->f1; _LL1831: _temp1830=( void*)
_temp1820.elt_typ; goto _LL1829; _LL1829: _temp1828=( void*) _temp1820.rgn_typ;
goto _LL1827; _LL1827: _temp1826=( struct Cyc_Absyn_Conref*) _temp1820.nullable;
goto _LL1825; _LL1825: _temp1824=( struct Cyc_Absyn_Tqual) _temp1820.tq; goto
_LL1823; _LL1823: _temp1822=( struct Cyc_Absyn_Conref*) _temp1820.bounds; goto
_LL1817;} else{ goto _LL1818;} _LL1818: goto _LL1819; _LL1817:{ void* _temp1832=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1822); struct
Cyc_Absyn_Exp* _temp1838; _LL1834: if( _temp1832 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1835;} else{ goto _LL1836;} _LL1836: if(( unsigned int) _temp1832 > 1u?*((
int*) _temp1832) == Cyc_Absyn_Upper_b: 0){ _LL1839: _temp1838=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1832)->f1; goto _LL1837;} else{ goto
_LL1833;} _LL1835: { struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array( _temp1830); struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1, _temp1824); void* tx= Cyc_Absyn_strct(
n); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1824); struct Cyc_Absyn_Exp*
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
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp1437, s1, 0), 0))->r));
goto _LL1833;} _LL1837: if( Cyc_Toc_is_nullable( e1_typ)){ struct _tuple0* x=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1840= _temp1437->topt; if( _temp1840 == 0){ _throw( Null_Exception);}
_temp1840->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1437, s3, 0), 0))->r));} goto _LL1833; _LL1833:;} goto _LL1815; _LL1819:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1841=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1842; _temp1842.curr= _temp1841; _temp1842.base= _temp1841; _temp1842.last_plus_one=
_temp1841 + 29; _temp1842;})); goto _LL1815; _LL1815:;} goto _LL1296;} _LL1338:
Cyc_Toc_exp_to_c( nv, _temp1441); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({ struct
Cyc_Core_Opt* _temp1844= e->topt; if( _temp1844 == 0){ _throw( Null_Exception);}
_temp1844->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1296;
_LL1340: Cyc_Toc_exp_to_c( nv, _temp1445);{ int is_poly= Cyc_Toc_is_poly_project(
e); if( Cyc_Toc_is_nullable(( void*)({ struct Cyc_Core_Opt* _temp1845= _temp1445->topt;
if( _temp1845 == 0){ _throw( Null_Exception);} _temp1845->v;}))){ struct _tuple0*
x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1846= _temp1445->topt; if( _temp1846 == 0){ _throw( Null_Exception);}
_temp1846->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0),
_temp1443, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1445, s3, 0), 0))->r));} if( is_poly){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1847= e->topt; if( _temp1847 == 0){ _throw(
Null_Exception);} _temp1847->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}
goto _LL1296;} _LL1342: { void* e1_typ= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp1900= _temp1449->topt; if( _temp1900 == 0){ _throw(
Null_Exception);} _temp1900->v;})); Cyc_Toc_exp_to_c( nv, _temp1449); Cyc_Toc_exp_to_c(
nv, _temp1447);( void*)( _temp1449->r=( void*)(( void*)( Cyc_Toc_array_to_ptr(
Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp(( void*) _temp1449->r, 0), 0))->r));{
void* _temp1848= e1_typ; struct Cyc_List_List* _temp1858; struct Cyc_Absyn_Exp*
_temp1860; struct Cyc_Absyn_Tqual _temp1862; void* _temp1864; struct Cyc_Absyn_PtrInfo
_temp1866; struct Cyc_Absyn_Conref* _temp1868; struct Cyc_Absyn_Tqual _temp1870;
struct Cyc_Absyn_Conref* _temp1872; void* _temp1874; void* _temp1876; _LL1850:
if(( unsigned int) _temp1848 > 4u?*(( int*) _temp1848) == Cyc_Absyn_TupleType: 0){
_LL1859: _temp1858=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1848)->f1; goto _LL1851;} else{ goto _LL1852;} _LL1852: if(( unsigned int)
_temp1848 > 4u?*(( int*) _temp1848) == Cyc_Absyn_ArrayType: 0){ _LL1865:
_temp1864=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1848)->f1; goto
_LL1863; _LL1863: _temp1862=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1848)->f2; goto _LL1861; _LL1861: _temp1860=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1848)->f3; goto _LL1853;} else{ goto
_LL1854;} _LL1854: if(( unsigned int) _temp1848 > 4u?*(( int*) _temp1848) == Cyc_Absyn_PointerType:
0){ _LL1867: _temp1866=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1848)->f1; _LL1877: _temp1876=( void*) _temp1866.elt_typ; goto _LL1875;
_LL1875: _temp1874=( void*) _temp1866.rgn_typ; goto _LL1873; _LL1873: _temp1872=(
struct Cyc_Absyn_Conref*) _temp1866.nullable; goto _LL1871; _LL1871: _temp1870=(
struct Cyc_Absyn_Tqual) _temp1866.tq; goto _LL1869; _LL1869: _temp1868=( struct
Cyc_Absyn_Conref*) _temp1866.bounds; goto _LL1855;} else{ goto _LL1856;} _LL1856:
goto _LL1857; _LL1851: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1447) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1449,({ struct _tagged_string* _temp1878=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1878[ 0]= xprintf("f%d",( int)
i); _temp1878;}), 0))->r)); goto _LL1849;} _LL1853: if( _temp1860 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1879=( char*)"exp_to_c: Subscript_e array type bound null"; struct
_tagged_string _temp1880; _temp1880.curr= _temp1879; _temp1880.base= _temp1879;
_temp1880.last_plus_one= _temp1879 + 44; _temp1880;}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_known_size_subscript( _temp1449, _temp1447, 0,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1881= _temp1860; if( _temp1881 == 0){ _throw(
Null_Exception);} _temp1881;}), _temp1864, _temp1862, 0))->r)); goto _LL1849;
_LL1855:{ void* _temp1882=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1868); struct Cyc_Absyn_Exp* _temp1888; _LL1884: if(( unsigned int)
_temp1882 > 1u?*(( int*) _temp1882) == Cyc_Absyn_Upper_b: 0){ _LL1889: _temp1888=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1882)->f1; goto
_LL1885;} else{ goto _LL1886;} _LL1886: if( _temp1882 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1887;} else{ goto _LL1883;} _LL1885: { int possibly_null= 1;{ void*
_temp1890=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1872))->v; int _temp1896; _LL1892: if((
unsigned int) _temp1890 > 1u?*(( int*) _temp1890) == Cyc_Absyn_Eq_constr: 0){
_LL1897: _temp1896=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1890)->f1;
goto _LL1893;} else{ goto _LL1894;} _LL1894: goto _LL1895; _LL1893:
possibly_null= _temp1896; goto _LL1891; _LL1895: possibly_null= 0; goto _LL1891;
_LL1891:;}( void*)( e->r=( void*)(( void*)( Cyc_Toc_known_size_subscript(
_temp1449, _temp1447, possibly_null, _temp1888, _temp1876, _temp1870, 0))->r));
goto _LL1883;} _LL1887: { struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0*
i= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array(
_temp1876); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1,
_temp1870); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1,
_temp1870); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1447, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1449, s1, 0), 0))->r)); goto _LL1883;} _LL1883:;}
goto _LL1849; _LL1857:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1898=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1899; _temp1899.curr= _temp1898; _temp1899.base=
_temp1898; _temp1899.last_plus_one= _temp1898 + 49; _temp1899;})); goto _LL1849;
_LL1849:;} goto _LL1296;} _LL1344: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1451))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1451); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp1451 != 0; _temp1451=({ struct Cyc_List_List* _temp1901=
_temp1451; if( _temp1901 == 0){ _throw( Null_Exception);} _temp1901->tl;}), i ++){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1902=
_temp1451; if( _temp1902 == 0){ _throw( Null_Exception);} _temp1902->hd;}));{
struct Cyc_List_List* des=({ struct Cyc_List_List* _temp1906=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1906->hd=( void*)(( void*)({
struct Cyc_Absyn_FieldName_struct* _temp1907=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp1907[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp1908; _temp1908.tag= Cyc_Absyn_FieldName;
_temp1908.f1=({ struct _tagged_string* _temp1909=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1909[ 0]= xprintf("f%d", i);
_temp1909;}); _temp1908;}); _temp1907;})); _temp1906->tl= 0; _temp1906;}); dles=({
struct Cyc_List_List* _temp1903=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1903->hd=( void*)({ struct _tuple4* _temp1904=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1904->f1=( struct Cyc_List_List*)
des; _temp1904->f2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1905=
_temp1451; if( _temp1905 == 0){ _throw( Null_Exception);} _temp1905->hd;});
_temp1904;}); _temp1903->tl= dles; _temp1903;});}}} dles=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp( 0,
dles, 0), 0))->r));} goto _LL1296; _LL1346:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1910=( char*)"compoundlit";
struct _tagged_string _temp1911; _temp1911.curr= _temp1910; _temp1911.base=
_temp1910; _temp1911.last_plus_one= _temp1910 + 12; _temp1911;})); goto _LL1296;
_LL1348:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1457, 0))->r));{ struct Cyc_List_List* dles= _temp1457; for( 0; dles != 0;
dles=({ struct Cyc_List_List* _temp1912= dles; if( _temp1912 == 0){ _throw(
Null_Exception);} _temp1912->tl;})){ struct _tuple4 _temp1916; struct Cyc_Absyn_Exp*
_temp1917; struct Cyc_List_List* _temp1919; struct _tuple4* _temp1914=( struct
_tuple4*)({ struct Cyc_List_List* _temp1913= dles; if( _temp1913 == 0){ _throw(
Null_Exception);} _temp1913->hd;}); _temp1916=* _temp1914; _LL1920: _temp1919=
_temp1916.f1; goto _LL1918; _LL1918: _temp1917= _temp1916.f2; goto _LL1915;
_LL1915: Cyc_Toc_exp_to_c( nv, _temp1917);}} goto _LL1296; _LL1350: {
unsigned int sz= Cyc_Evexp_eval_const_uint_exp( _temp1461); Cyc_Toc_exp_to_c( nv,
_temp1459);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp1459)){
unsigned int i= 0; for( 0; i < sz; i ++){ es=({ struct Cyc_List_List* _temp1921=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1921->hd=(
void*)({ struct _tuple4* _temp1922=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1922->f1= 0; _temp1922->f2= _temp1459; _temp1922;}); _temp1921->tl=
es; _temp1921;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL1296;}} _LL1352: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1467, _temp1471))->r));}
else{ struct _tuple0* n= _temp1471; struct Cyc_Absyn_Structdecl* sd2=( struct
Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl* _temp1935= _temp1465 == 0?((
struct Cyc_Absyn_Structdecl*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1934= Cyc_Absynpp_qvar2string( _temp1471); xprintf("no structdecl %.*s",
_temp1934.last_plus_one - _temp1934.curr, _temp1934.curr);})): _temp1465; if(
_temp1935 == 0){ _throw( Null_Exception);} _temp1935;}); struct _RegionHandle
_temp1923= _new_region(); struct _RegionHandle* rgn=& _temp1923; _push_region(
rgn);{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, e->loc,
_temp1467,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1933=(( struct
Cyc_Absyn_Structdecl*) sd2)->fields; if( _temp1933 == 0){ _throw( Null_Exception);}
_temp1933->v;})); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp1924=
fields; if( _temp1924 == 0){ _throw( Null_Exception);} _temp1924->tl;})){ struct
_tuple12 _temp1928; struct Cyc_Absyn_Exp* _temp1929; struct Cyc_Absyn_Structfield*
_temp1931; struct _tuple12* _temp1926=( struct _tuple12*)({ struct Cyc_List_List*
_temp1925= fields; if( _temp1925 == 0){ _throw( Null_Exception);} _temp1925->hd;});
_temp1928=* _temp1926; _LL1932: _temp1931= _temp1928.f1; goto _LL1930; _LL1930:
_temp1929= _temp1928.f2; goto _LL1927; _LL1927: { void* fieldtyp=( void*)
_temp1931->type; Cyc_Toc_exp_to_c( nv, _temp1929); if( Cyc_Toc_is_void_star(
fieldtyp)){( void*)( _temp1929->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1929->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0,
_temp1467, 0), 0))->r));}; _pop_region(& _temp1923);} goto _LL1296; _LL1354: {
struct Cyc_List_List* fs;{ void* _temp1936= Cyc_Tcutil_compress( _temp1475);
struct Cyc_List_List* _temp1942; _LL1938: if(( unsigned int) _temp1936 > 4u?*((
int*) _temp1936) == Cyc_Absyn_AnonStructType: 0){ _LL1943: _temp1942=( struct
Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*) _temp1936)->f1; goto
_LL1939;} else{ goto _LL1940;} _LL1940: goto _LL1941; _LL1939: fs= _temp1942;
goto _LL1937; _LL1941: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1944= Cyc_Absynpp_typ2string(
_temp1475); xprintf("anon struct has type %.*s", _temp1944.last_plus_one -
_temp1944.curr, _temp1944.curr);})); goto _LL1937; _LL1937:;}{ struct
_RegionHandle _temp1945= _new_region(); struct _RegionHandle* rgn=& _temp1945;
_push_region( rgn);{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1473, fs); for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp1946= fields; if( _temp1946 == 0){ _throw( Null_Exception);} _temp1946->tl;})){
struct _tuple12 _temp1950; struct Cyc_Absyn_Exp* _temp1951; struct Cyc_Absyn_Structfield*
_temp1953; struct _tuple12* _temp1948=( struct _tuple12*)({ struct Cyc_List_List*
_temp1947= fields; if( _temp1947 == 0){ _throw( Null_Exception);} _temp1947->hd;});
_temp1950=* _temp1948; _LL1954: _temp1953= _temp1950.f1; goto _LL1952; _LL1952:
_temp1951= _temp1950.f2; goto _LL1949; _LL1949: { void* fieldtyp=( void*)
_temp1953->type; Cyc_Toc_exp_to_c( nv, _temp1951); if( Cyc_Toc_is_void_star(
fieldtyp)){( void*)( _temp1951->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1951->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1473, 0))->r));}; _pop_region(&
_temp1945);} goto _LL1296;} _LL1356: { struct _tuple0* qv= _temp1477->name; if(
_temp1479->is_xtunion? 1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp(
qv, 0))->r));} else{ int tag_count= 0; struct Cyc_List_List* fields= _temp1479->fields
== 0? 0:( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1958= _temp1479->fields;
if( _temp1958 == 0){ _throw( Null_Exception);} _temp1958->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1955= fields;
if( _temp1955 == 0){ _throw( Null_Exception);} _temp1955->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1956= fields;
if( _temp1956 == 0){ _throw( Null_Exception);} _temp1956->hd;}))->typs == 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1957= fields; if( _temp1957
== 0){ _throw( Null_Exception);} _temp1957->tl;});}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto _LL1296;}
_LL1358: { struct _tuple0* qv= _temp1487->name; struct Cyc_List_List* tqts=
_temp1487->typs; struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1986=( char*)"_struct"; struct
_tagged_string _temp1987; _temp1987.curr= _temp1986; _temp1987.base= _temp1986;
_temp1987.last_plus_one= _temp1986 + 8; _temp1987;}))); if( nv->toplevel){
struct Cyc_Absyn_Exp* tag_exp; if( _temp1489->is_xtunion){ tag_exp= Cyc_Absyn_var_exp(
qv, 0);} else{ int tag_count= 0; struct Cyc_List_List* fields= _temp1489->fields
== 0? 0:( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1962= _temp1489->fields;
if( _temp1962 == 0){ _throw( Null_Exception);} _temp1962->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1959= fields;
if( _temp1959 == 0){ _throw( Null_Exception);} _temp1959->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1960= fields;
if( _temp1960 == 0){ _throw( Null_Exception);} _temp1960->hd;}))->typs != 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1961= fields; if( _temp1961
== 0){ _throw( Null_Exception);} _temp1961->tl;});} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles= 0; for( 0; _temp1491
!= 0; _temp1491=({ struct Cyc_List_List* _temp1963= _temp1491; if( _temp1963 ==
0){ _throw( Null_Exception);} _temp1963->tl;}), tqts=({ struct Cyc_List_List*
_temp1964= tqts; if( _temp1964 == 0){ _throw( Null_Exception);} _temp1964->tl;})){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1970= _temp1491; if( _temp1970 == 0){ _throw( Null_Exception);} _temp1970->hd;});
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp1969= tqts; if( _temp1969 == 0){ _throw( Null_Exception);} _temp1969->hd;}))).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} dles=({ struct Cyc_List_List* _temp1965=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1965[ 0]=({ struct Cyc_List_List
_temp1966; _temp1966.hd=( void*)({ struct _tuple4* _temp1967=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp1967[ 0]=({ struct _tuple4
_temp1968; _temp1968.f1= 0; _temp1968.f2= cur_e; _temp1968;}); _temp1967;});
_temp1966.tl= dles; _temp1966;}); _temp1965;});} dles=({ struct Cyc_List_List*
_temp1971=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1971[ 0]=({ struct Cyc_List_List _temp1972; _temp1972.hd=( void*)({ struct
_tuple4* _temp1973=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp1973[ 0]=({ struct _tuple4 _temp1974; _temp1974.f1= 0; _temp1974.f2=
tag_exp; _temp1974;}); _temp1973;}); _temp1972.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); _temp1972;}); _temp1971;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1975=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1975->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({
char* _temp1976=( char*)"_struct"; struct _tagged_string _temp1977; _temp1977.curr=
_temp1976; _temp1977.base= _temp1976; _temp1977.last_plus_one= _temp1976 + 8;
_temp1977;})); _temp1975;}), dles, 0))->r));}} else{ struct Cyc_List_List*
rev_assign=({ struct Cyc_List_List* _temp1985=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1985->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( xexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( qv, 0),
0); _temp1985->tl= 0; _temp1985;});{ int i= 1; for( 0; _temp1491 != 0;(
_temp1491=({ struct Cyc_List_List* _temp1978= _temp1491; if( _temp1978 == 0){
_throw( Null_Exception);} _temp1978->tl;}), i ++), tqts=({ struct Cyc_List_List*
_temp1979= tqts; if( _temp1979 == 0){ _throw( Null_Exception);} _temp1979->tl;})){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1983= _temp1491; if( _temp1983 == 0){ _throw( Null_Exception);} _temp1983->hd;});
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp1982= tqts; if( _temp1982 == 0){ _throw( Null_Exception);} _temp1982->hd;}))).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1981=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1981[ 0]= xprintf("f%d", i); _temp1981;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1980=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1980->hd=( void*) a; _temp1980->tl=
rev_assign; _temp1980;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1984=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1984->hd=(
void*) final_stmt; _temp1984->tl= rev_assign; _temp1984;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL1296;} _LL1360: goto _LL1296; _LL1362: { void* t_c= Cyc_Toc_typ_to_c(
_temp1503); if( _temp1505 != 0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1988= _temp1505; if( _temp1988 == 0){ _throw(
Null_Exception);} _temp1988;}); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)((
void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1503, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} goto _LL1296;} _LL1364: Cyc_Toc_stmt_to_c( nv, _temp1507); goto
_LL1296; _LL1366:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1989=( char*)"UnresolvedMem"; struct
_tagged_string _temp1990; _temp1990.curr= _temp1989; _temp1990.base= _temp1989;
_temp1990.last_plus_one= _temp1989 + 14; _temp1990;})); goto _LL1296; _LL1368:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1991=( char*)"codegen"; struct _tagged_string _temp1992; _temp1992.curr=
_temp1991; _temp1992.base= _temp1991; _temp1992.last_plus_one= _temp1991 + 8;
_temp1992;})); goto _LL1296; _LL1370:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1993=( char*)"fill"; struct _tagged_string
_temp1994; _temp1994.curr= _temp1993; _temp1994.base= _temp1993; _temp1994.last_plus_one=
_temp1993 + 5; _temp1994;})); goto _LL1296; _LL1296:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple14{ struct _tuple0* f1; void* f2;
} ; struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, void* t,
struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat* p,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1996=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp2032; struct Cyc_Absyn_Vardecl _temp2034; struct Cyc_List_List* _temp2035;
struct Cyc_Core_Opt* _temp2037; struct Cyc_Absyn_Exp* _temp2039; void* _temp2041;
struct Cyc_Absyn_Tqual _temp2043; struct _tuple0* _temp2045; void* _temp2047;
struct Cyc_Absyn_Vardecl* _temp2049; int _temp2051; void* _temp2053; char
_temp2055; struct _tagged_string _temp2057; struct Cyc_Absyn_Enumfield*
_temp2059; struct Cyc_Absyn_Enumdecl* _temp2061; struct Cyc_List_List* _temp2063;
struct Cyc_List_List* _temp2065; struct Cyc_Absyn_Tunionfield* _temp2067; struct
Cyc_Absyn_Tuniondecl* _temp2069; struct Cyc_Absyn_Pat* _temp2072; struct Cyc_Absyn_Pat
_temp2074; struct Cyc_Position_Segment* _temp2075; struct Cyc_Core_Opt*
_temp2077; void* _temp2079; struct Cyc_List_List* _temp2081; struct Cyc_List_List*
_temp2083; struct Cyc_Absyn_Tunionfield* _temp2085; struct Cyc_Absyn_Tuniondecl*
_temp2087; struct Cyc_List_List* _temp2089; struct Cyc_List_List* _temp2091;
struct Cyc_Absyn_Tunionfield* _temp2093; struct Cyc_Absyn_Tuniondecl* _temp2095;
struct Cyc_List_List* _temp2097; struct Cyc_List_List* _temp2099; struct Cyc_List_List*
_temp2101; struct Cyc_Core_Opt* _temp2103; struct Cyc_Absyn_Structdecl*
_temp2105; struct Cyc_Absyn_Pat* _temp2107; struct _tuple0* _temp2109; struct
Cyc_List_List* _temp2111; struct Cyc_List_List* _temp2113; struct _tuple0*
_temp2115; struct Cyc_List_List* _temp2117; struct Cyc_List_List* _temp2119;
struct _tuple0* _temp2121; _LL1998: if(( unsigned int) _temp1996 > 2u?*(( int*)
_temp1996) == Cyc_Absyn_Var_p: 0){ _LL2033: _temp2032=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1996)->f1; _temp2034=* _temp2032; _LL2048:
_temp2047=( void*) _temp2034.sc; goto _LL2046; _LL2046: _temp2045=( struct
_tuple0*) _temp2034.name; goto _LL2044; _LL2044: _temp2043=( struct Cyc_Absyn_Tqual)
_temp2034.tq; goto _LL2042; _LL2042: _temp2041=( void*) _temp2034.type; goto
_LL2040; _LL2040: _temp2039=( struct Cyc_Absyn_Exp*) _temp2034.initializer; goto
_LL2038; _LL2038: _temp2037=( struct Cyc_Core_Opt*) _temp2034.rgn; goto _LL2036;
_LL2036: _temp2035=( struct Cyc_List_List*) _temp2034.attributes; goto _LL1999;}
else{ goto _LL2000;} _LL2000: if( _temp1996 ==( void*) Cyc_Absyn_Wild_p){ goto
_LL2001;} else{ goto _LL2002;} _LL2002: if(( unsigned int) _temp1996 > 2u?*((
int*) _temp1996) == Cyc_Absyn_Reference_p: 0){ _LL2050: _temp2049=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Reference_p_struct*) _temp1996)->f1; goto _LL2003;} else{ goto
_LL2004;} _LL2004: if( _temp1996 ==( void*) Cyc_Absyn_Null_p){ goto _LL2005;}
else{ goto _LL2006;} _LL2006: if(( unsigned int) _temp1996 > 2u?*(( int*)
_temp1996) == Cyc_Absyn_Int_p: 0){ _LL2054: _temp2053=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp1996)->f1; goto _LL2052; _LL2052: _temp2051=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1996)->f2; goto _LL2007;} else{ goto _LL2008;} _LL2008: if(( unsigned int)
_temp1996 > 2u?*(( int*) _temp1996) == Cyc_Absyn_Char_p: 0){ _LL2056: _temp2055=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp1996)->f1; goto _LL2009;} else{
goto _LL2010;} _LL2010: if(( unsigned int) _temp1996 > 2u?*(( int*) _temp1996)
== Cyc_Absyn_Float_p: 0){ _LL2058: _temp2057=( struct _tagged_string)(( struct
Cyc_Absyn_Float_p_struct*) _temp1996)->f1; goto _LL2011;} else{ goto _LL2012;}
_LL2012: if(( unsigned int) _temp1996 > 2u?*(( int*) _temp1996) == Cyc_Absyn_Enum_p:
0){ _LL2062: _temp2061=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1996)->f1; goto _LL2060; _LL2060: _temp2059=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1996)->f2; goto _LL2013;} else{ goto
_LL2014;} _LL2014: if(( unsigned int) _temp1996 > 2u?*(( int*) _temp1996) == Cyc_Absyn_Tunion_p:
0){ _LL2070: _temp2069=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1996)->f1; goto _LL2068; _LL2068: _temp2067=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1996)->f2; goto _LL2066; _LL2066:
_temp2065=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1996)->f3; goto _LL2064; _LL2064: _temp2063=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1996)->f4; if( _temp2063 == 0){ goto
_LL2015;} else{ goto _LL2016;}} else{ goto _LL2016;} _LL2016: if(( unsigned int)
_temp1996 > 2u?*(( int*) _temp1996) == Cyc_Absyn_Pointer_p: 0){ _LL2073:
_temp2072=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1996)->f1; _temp2074=* _temp2072; _LL2080: _temp2079=( void*) _temp2074.r;
if(( unsigned int) _temp2079 > 2u?*(( int*) _temp2079) == Cyc_Absyn_Tunion_p: 0){
_LL2088: _temp2087=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2079)->f1; goto _LL2086; _LL2086: _temp2085=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2079)->f2; goto _LL2084; _LL2084:
_temp2083=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2079)->f3; goto _LL2082; _LL2082: _temp2081=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2079)->f4; goto _LL2078;} else{ goto
_LL2018;} _LL2078: _temp2077=( struct Cyc_Core_Opt*) _temp2074.topt; goto
_LL2076; _LL2076: _temp2075=( struct Cyc_Position_Segment*) _temp2074.loc; goto
_LL2071;} else{ goto _LL2018;} _LL2071: if( _temp2081 != 0){ goto _LL2017;}
else{ goto _LL2018;} _LL2018: if(( unsigned int) _temp1996 > 2u?*(( int*)
_temp1996) == Cyc_Absyn_Tunion_p: 0){ _LL2096: _temp2095=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1996)->f1; goto _LL2094; _LL2094:
_temp2093=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1996)->f2; goto _LL2092; _LL2092: _temp2091=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1996)->f3; goto _LL2090; _LL2090:
_temp2089=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1996)->f4; goto _LL2019;} else{ goto _LL2020;} _LL2020: if(( unsigned int)
_temp1996 > 2u?*(( int*) _temp1996) == Cyc_Absyn_Tuple_p: 0){ _LL2098: _temp2097=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp1996)->f1; goto
_LL2021;} else{ goto _LL2022;} _LL2022: if(( unsigned int) _temp1996 > 2u?*((
int*) _temp1996) == Cyc_Absyn_Struct_p: 0){ _LL2106: _temp2105=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1996)->f1; goto _LL2104; _LL2104:
_temp2103=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1996)->f2;
goto _LL2102; _LL2102: _temp2101=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1996)->f3; goto _LL2100; _LL2100: _temp2099=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1996)->f4; goto _LL2023;} else{ goto
_LL2024;} _LL2024: if(( unsigned int) _temp1996 > 2u?*(( int*) _temp1996) == Cyc_Absyn_Pointer_p:
0){ _LL2108: _temp2107=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1996)->f1; goto _LL2025;} else{ goto _LL2026;} _LL2026: if(( unsigned int)
_temp1996 > 2u?*(( int*) _temp1996) == Cyc_Absyn_UnknownId_p: 0){ _LL2110:
_temp2109=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1996)->f1;
goto _LL2027;} else{ goto _LL2028;} _LL2028: if(( unsigned int) _temp1996 > 2u?*((
int*) _temp1996) == Cyc_Absyn_UnknownCall_p: 0){ _LL2116: _temp2115=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1996)->f1; goto _LL2114;
_LL2114: _temp2113=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1996)->f2; goto _LL2112; _LL2112: _temp2111=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1996)->f3; goto _LL2029;} else{
goto _LL2030;} _LL2030: if(( unsigned int) _temp1996 > 2u?*(( int*) _temp1996)
== Cyc_Absyn_UnknownFields_p: 0){ _LL2122: _temp2121=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp1996)->f1; goto _LL2120; _LL2120:
_temp2119=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1996)->f2; goto _LL2118; _LL2118: _temp2117=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1996)->f3; goto _LL2031;} else{
goto _LL1997;} _LL1999: nv= Cyc_Toc_add_varmap( nv, _temp2045, r); goto _LL2001;
_LL2001: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1997; _LL2003: { struct
_tuple0* v= Cyc_Toc_temp_var(); decls=({ struct Cyc_List_List* _temp2123=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2123->hd=(
void*)({ struct _tuple14* _temp2124=( struct _tuple14*) GC_malloc( sizeof(
struct _tuple14)); _temp2124->f1= v; _temp2124->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp2124;}); _temp2123->tl= decls; _temp2123;}); nv= Cyc_Toc_add_varmap(
nv, _temp2049->name, Cyc_Absyn_var_exp( v, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL1997;} _LL2005: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL1997; _LL2007: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2053, _temp2051, 0), succ_lab, fail_lab); goto _LL1997;
_LL2009: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2055, 0),
succ_lab, fail_lab); goto _LL1997; _LL2011: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2057, 0), succ_lab, fail_lab); goto _LL1997; _LL2013: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2125=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2125[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2126; _temp2126.tag= Cyc_Absyn_Enum_e;
_temp2126.f1= _temp2059->name; _temp2126.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2061; _temp2126.f3=( struct Cyc_Absyn_Enumfield*) _temp2059; _temp2126;});
_temp2125;}), 0), succ_lab, fail_lab); goto _LL1997; _LL2015: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2067->name, 0); if( ! _temp2069->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1997;} _LL2017: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2081);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2085->name,( struct
_tagged_string)({ char* _temp2138=( char*)"_struct"; struct _tagged_string
_temp2139; _temp2139.curr= _temp2138; _temp2139.base= _temp2138; _temp2139.last_plus_one=
_temp2138 + 8; _temp2139;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2081); for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp2127= rps; if(
_temp2127 == 0){ _throw( Null_Exception);} _temp2127->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2134= rps; if(
_temp2134 == 0){ _throw( Null_Exception);} _temp2134->hd;}); struct _tuple0* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp2133= p2->topt;
if( _temp2133 == 0){ _throw( Null_Exception);} _temp2133->v;}); void* t2c= Cyc_Toc_typ_to_c(
t2); struct _tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2128=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2128->hd=( void*)({ struct _tuple14* _temp2129=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2129->f1= v; _temp2129->f2= t2c;
_temp2129;}); _temp2128->tl= decls; _temp2128;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp2132=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2132[ 0]= xprintf("f%d", cnt); _temp2132;}), 0), p2,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp2131=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2131[ 0]= xprintf("f%d", cnt); _temp2131;}), 0), 0),
0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List*
_temp2130=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2130->hd=( void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp2130->tl= ss;
_temp2130;}); succ_lab= lab;}}}}{ struct Cyc_Absyn_Exp* test_exp; if( _temp2087->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2085->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2137= _temp2087->fields; if( _temp2137 == 0){ _throw( Null_Exception);}
_temp2137->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp2135= fs; if(
_temp2135 == 0){ _throw( Null_Exception);} _temp2135->tl;})){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2136= fs; if(
_temp2136 == 0){ _throw( Null_Exception);} _temp2136->hd;}); if( f->typs == 0){
max_tag ++;}}}{ struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t,
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0);
struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2085->name,
0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0):
Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL1997;}} _LL2019:
_temp2097= _temp2089; goto _LL2021; _LL2021: { struct Cyc_List_List* ss= 0; int
cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2097);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2097); for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp2140= rps; if(
_temp2140 == 0){ _throw( Null_Exception);} _temp2140->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2147= rps; if(
_temp2147 == 0){ _throw( Null_Exception);} _temp2147->hd;}); struct _tuple0* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp2146= p2->topt;
if( _temp2146 == 0){ _throw( Null_Exception);} _temp2146->v;}); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2141=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2141->hd=( void*)({ struct _tuple14* _temp2142=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2142->f1= v; _temp2142->f2= Cyc_Toc_typ_to_c(
t2); _temp2142;}); _temp2141->tl= decls; _temp2141;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp2145=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2145[ 0]= xprintf("f%d", cnt);
_temp2145;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp2144=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2144[ 0]= xprintf("f%d", cnt); _temp2144;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp2143=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2143->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp2143->tl= ss; _temp2143;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1997;} _LL2023: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2099); for( 0; dlps != 0; dlps=({ struct Cyc_List_List* _temp2148= dlps;
if( _temp2148 == 0){ _throw( Null_Exception);} _temp2148->tl;})){ struct
_tuple15* tup=( struct _tuple15*)({ struct Cyc_List_List* _temp2162= dlps; if(
_temp2162 == 0){ _throw( Null_Exception);} _temp2162->hd;}); struct Cyc_Absyn_Pat*
p2=(* tup).f2; struct _tagged_string* f;{ void* _temp2150=( void*)({ struct Cyc_List_List*
_temp2149=(* tup).f1; if( _temp2149 == 0){ _throw( Null_Exception);} _temp2149->hd;});
struct _tagged_string* _temp2156; _LL2152: if(*(( int*) _temp2150) == Cyc_Absyn_FieldName){
_LL2157: _temp2156=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp2150)->f1; goto _LL2153;} else{ goto _LL2154;} _LL2154: goto _LL2155;
_LL2153: f= _temp2156; goto _LL2151; _LL2155:( void) _throw(( void*) Cyc_Toc_Match_error);
_LL2151:;}{ struct _tuple0* v= Cyc_Toc_temp_var(); void* t2=( void*)({ struct
Cyc_Core_Opt* _temp2161= p2->topt; if( _temp2161 == 0){ _throw( Null_Exception);}
_temp2161->v;}); void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab=
Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List* _temp2158=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2158->hd=( void*)({ struct
_tuple14* _temp2159=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp2159->f1= v; _temp2159->f2= t2c; _temp2159;}); _temp2158->tl= decls;
_temp2158;});{ struct _tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structmember_exp( path, f, 0), p2, succ_lab, fail_lab, decls);
nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s2= result.f3; struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp( r, f, 0), 0), 0), s2,
0); ss=({ struct Cyc_List_List* _temp2160=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2160->hd=( void*) Cyc_Absyn_label_stmt( lab,
s3, 0); _temp2160->tl= ss; _temp2160;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1997;} _LL2025: { struct _tuple0* v= Cyc_Toc_temp_var(); void*
t2=( void*)({ struct Cyc_Core_Opt* _temp2165= _temp2107->topt; if( _temp2165 ==
0){ _throw( Null_Exception);} _temp2165->v;}); decls=({ struct Cyc_List_List*
_temp2163=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2163->hd=( void*)({ struct _tuple14* _temp2164=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2164->f1= v; _temp2164->f2= Cyc_Toc_typ_to_c(
t2); _temp2164;}); _temp2163->tl= decls; _temp2163;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path,
0), _temp2107, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0),
s2, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);}
else{ s= s3;} goto _LL1997;}}} _LL2027: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2166=(
char*)"unknownid"; struct _tagged_string _temp2167; _temp2167.curr= _temp2166;
_temp2167.base= _temp2166; _temp2167.last_plus_one= _temp2166 + 10; _temp2167;}));
goto _LL1997; _LL2029: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2168=( char*)"unknowncall";
struct _tagged_string _temp2169; _temp2169.curr= _temp2168; _temp2169.base=
_temp2168; _temp2169.last_plus_one= _temp2168 + 12; _temp2169;})); goto _LL1997;
_LL2031: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2170=( char*)"unknownfields"; struct
_tagged_string _temp2171; _temp2171.curr= _temp2170; _temp2171.base= _temp2170;
_temp2171.last_plus_one= _temp2170 + 14; _temp2171;})); goto _LL1997; _LL1997:;}
return({ struct _tuple13 _temp2172; _temp2172.f1= nv; _temp2172.f2= decls;
_temp2172.f3= s; _temp2172;});} struct _tuple16{ struct _tagged_string* f1;
struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause* f3; } ; static struct
_tuple16* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause* sc){ return({
struct _tuple16* _temp2173=( struct _tuple16*) GC_malloc( sizeof( struct
_tuple16)); _temp2173->f1= Cyc_Toc_fresh_label(); _temp2173->f2= Cyc_Toc_fresh_label();
_temp2173->f3= sc; _temp2173;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* t=( void*)({ struct Cyc_Core_Opt*
_temp2229= e->topt; if( _temp2229 == 0){ _throw( Null_Exception);} _temp2229->v;});
int leave_as_switch;{ void* _temp2174= Cyc_Tcutil_compress( t); void* _temp2180;
void* _temp2182; _LL2176: if(( unsigned int) _temp2174 > 4u?*(( int*) _temp2174)
== Cyc_Absyn_IntType: 0){ _LL2183: _temp2182=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2174)->f1; goto _LL2181; _LL2181: _temp2180=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2174)->f2; goto _LL2177;} else{ goto _LL2178;} _LL2178: goto _LL2179;
_LL2177: leave_as_switch= 1; goto _LL2175; _LL2179: leave_as_switch= 0; goto
_LL2175; _LL2175:;}{ struct Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1=({
struct Cyc_List_List* _temp2184= scs1; if( _temp2184 == 0){ _throw(
Null_Exception);} _temp2184->tl;})){ if(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2186=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp2185=
scs1; if( _temp2185 == 0){ _throw( Null_Exception);} _temp2185->hd;}))->pat_vars;
if( _temp2186 == 0){ _throw( Null_Exception);} _temp2186->v;}) != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* scs1= scs; for( 0; scs1 !=
0; scs1=({ struct Cyc_List_List* _temp2187= scs1; if( _temp2187 == 0){ _throw(
Null_Exception);} _temp2187->tl;})){ struct Cyc_Absyn_Stmt* s=(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2189= scs1; if( _temp2189 == 0){ _throw(
Null_Exception);} _temp2189->hd;}))->body;(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2188= scs1; if( _temp2188 == 0){ _throw(
Null_Exception);} _temp2188->hd;}))->body= Cyc_Absyn_label_stmt( next_l, s, 0);
next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), s);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0);
struct _tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple16*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* lscs2= lscs; for( 0; lscs2 != 0; lscs2=({ struct Cyc_List_List*
_temp2190= lscs2; if( _temp2190 == 0){ _throw( Null_Exception);} _temp2190->tl;})){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2202= lscs2; if( _temp2202 == 0){ _throw( Null_Exception);} _temp2202->hd;}))).f3;
struct _tagged_string* fail_lab=({ struct Cyc_List_List* _temp2199= lscs2; if(
_temp2199 == 0){ _throw( Null_Exception);} _temp2199->tl;}) == 0? end_l:(*((
struct _tuple16*)({ struct Cyc_List_List* _temp2201=({ struct Cyc_List_List*
_temp2200= lscs2; if( _temp2200 == 0){ _throw( Null_Exception);} _temp2200->tl;});
if( _temp2201 == 0){ _throw( Null_Exception);} _temp2201->hd;}))).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2198= lscs2; if( _temp2198 == 0){ _throw( Null_Exception);} _temp2198->hd;}))).f2;
if( sc->where_clause == 0){ struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r,
path, sc->pattern, succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List*
_temp2191=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2191->hd=( void*) res.f1; _temp2191->tl= nvs; _temp2191;}); decls= res.f2;
test_stmts=({ struct Cyc_List_List* _temp2192=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2192->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2193= lscs2; if( _temp2193 == 0){
_throw( Null_Exception);} _temp2193->hd;}))).f1, res.f3, 0); _temp2192->tl=
test_stmts; _temp2192;});} else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2197= sc->where_clause; if( _temp2197 == 0){ _throw(
Null_Exception);} _temp2197;}); struct _tagged_string* intermed_lab= Cyc_Toc_fresh_label();
struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r, path, sc->pattern,
intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c( res.f1, e); nvs=({ struct Cyc_List_List*
_temp2194=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2194->hd=( void*) res.f1; _temp2194->tl= nvs; _temp2194;}); decls= res.f2;{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt( e, Cyc_Absyn_goto_stmt(
succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3=
Cyc_Absyn_label_stmt( intermed_lab, s2, 0); test_stmts=({ struct Cyc_List_List*
_temp2195=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2195->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2196= lscs2; if( _temp2196 == 0){ _throw( Null_Exception);} _temp2196->hd;}))).f1,
Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0); _temp2195->tl= test_stmts; _temp2195;});}}}}{
struct Cyc_Absyn_Stmt* test_stmt= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( test_stmts), 0); nvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt*
clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){ for( 0;({ struct Cyc_List_List*
_temp2203= lscs; if( _temp2203 == 0){ _throw( Null_Exception);} _temp2203->tl;})
!= 0; lscs=({ struct Cyc_List_List* _temp2204= lscs; if( _temp2204 == 0){ _throw(
Null_Exception);} _temp2204->tl;}), nvs=({ struct Cyc_List_List* _temp2205= nvs;
if( _temp2205 == 0){ _throw( Null_Exception);} _temp2205->tl;})){ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2215= nvs; if( _temp2215
== 0){ _throw( Null_Exception);} _temp2215->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2214= lscs; if( _temp2214 == 0){
_throw( Null_Exception);} _temp2214->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
e, end_l,(*(( struct _tuple16*)({ struct Cyc_List_List* _temp2207=({ struct Cyc_List_List*
_temp2206= lscs; if( _temp2206 == 0){ _throw( Null_Exception);} _temp2206->tl;});
if( _temp2207 == 0){ _throw( Null_Exception);} _temp2207->hd;}))).f2,( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2210=((*(( struct _tuple16*)({
struct Cyc_List_List* _temp2209=({ struct Cyc_List_List* _temp2208= lscs; if(
_temp2208 == 0){ _throw( Null_Exception);} _temp2208->tl;}); if( _temp2209 == 0){
_throw( Null_Exception);} _temp2209->hd;}))).f3)->pat_vars; if( _temp2210 == 0){
_throw( Null_Exception);} _temp2210->v;}),( struct Cyc_Toc_Env*)({ struct Cyc_List_List*
_temp2212=({ struct Cyc_List_List* _temp2211= nvs; if( _temp2211 == 0){ _throw(
Null_Exception);} _temp2211->tl;}); if( _temp2212 == 0){ _throw( Null_Exception);}
_temp2212->hd;})), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2213= lscs; if( _temp2213 == 0){
_throw( Null_Exception);} _temp2213->hd;}))).f2, s, 0), 0);}{ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2218= nvs; if( _temp2218
== 0){ _throw( Null_Exception);} _temp2218->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2217= lscs; if( _temp2217 == 0){
_throw( Null_Exception);} _temp2217->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env(
e, end_l), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2216= lscs; if( _temp2216 == 0){
_throw( Null_Exception);} _temp2216->hd;}))).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( test_stmt, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=({
struct Cyc_List_List* _temp2219= decls; if( _temp2219 == 0){ _throw(
Null_Exception);} _temp2219->tl;})){ struct _tuple14 _temp2223; void* _temp2224;
struct _tuple0* _temp2226; struct _tuple14* _temp2221=( struct _tuple14*)({
struct Cyc_List_List* _temp2220= decls; if( _temp2220 == 0){ _throw(
Null_Exception);} _temp2220->hd;}); _temp2223=* _temp2221; _LL2227: _temp2226=
_temp2223.f1; goto _LL2225; _LL2225: _temp2224= _temp2223.f2; goto _LL2222;
_LL2222: res= Cyc_Absyn_declare_stmt( _temp2226, _temp2224, 0, res, 0);}( void*)(
whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( v, Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp2228= e->topt; if( _temp2228 == 0){ _throw(
Null_Exception);} _temp2228->v;})),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2230=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2230[ 0]=({ struct Cyc_List_List _temp2231; _temp2231.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2231.tl= 0; _temp2231;}); _temp2230;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2232=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2272; struct Cyc_Absyn_Stmt* _temp2274; struct Cyc_Absyn_Stmt*
_temp2276; struct Cyc_Absyn_Exp* _temp2278; struct Cyc_Absyn_Stmt* _temp2280;
struct Cyc_Absyn_Stmt* _temp2282; struct Cyc_Absyn_Exp* _temp2284; struct Cyc_Absyn_Stmt*
_temp2286; struct _tuple2 _temp2288; struct Cyc_Absyn_Stmt* _temp2290; struct
Cyc_Absyn_Exp* _temp2292; struct Cyc_Absyn_Stmt* _temp2294; struct Cyc_Absyn_Stmt*
_temp2296; struct Cyc_Absyn_Stmt* _temp2298; struct _tagged_string* _temp2300;
struct Cyc_Absyn_Stmt* _temp2302; struct _tuple2 _temp2304; struct Cyc_Absyn_Stmt*
_temp2306; struct Cyc_Absyn_Exp* _temp2308; struct _tuple2 _temp2310; struct Cyc_Absyn_Stmt*
_temp2312; struct Cyc_Absyn_Exp* _temp2314; struct Cyc_Absyn_Exp* _temp2316;
struct Cyc_List_List* _temp2318; struct Cyc_Absyn_Exp* _temp2320; struct Cyc_Absyn_Switch_clause**
_temp2322; struct Cyc_List_List* _temp2324; struct Cyc_Absyn_Stmt* _temp2326;
struct Cyc_Absyn_Decl* _temp2328; struct Cyc_Absyn_Stmt* _temp2330; struct Cyc_Absyn_Stmt*
_temp2332; struct Cyc_Absyn_Stmt* _temp2334; struct _tagged_string* _temp2336;
struct _tuple2 _temp2338; struct Cyc_Absyn_Stmt* _temp2340; struct Cyc_Absyn_Exp*
_temp2342; struct Cyc_Absyn_Stmt* _temp2344; struct Cyc_List_List* _temp2346;
struct Cyc_Absyn_Stmt* _temp2348; struct Cyc_Absyn_Stmt* _temp2350; struct Cyc_Absyn_Vardecl*
_temp2352; struct Cyc_Absyn_Tvar* _temp2354; _LL2234: if( _temp2232 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2235;} else{ goto _LL2236;} _LL2236: if((
unsigned int) _temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_Exp_s: 0){
_LL2273: _temp2272=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp2232)->f1; goto _LL2237;} else{ goto _LL2238;} _LL2238: if(( unsigned int)
_temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_Seq_s: 0){ _LL2277: _temp2276=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp2232)->f1; goto
_LL2275; _LL2275: _temp2274=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp2232)->f2; goto _LL2239;} else{ goto _LL2240;} _LL2240: if(( unsigned int)
_temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_Return_s: 0){ _LL2279:
_temp2278=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp2232)->f1; goto _LL2241;} else{ goto _LL2242;} _LL2242: if(( unsigned int)
_temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_IfThenElse_s: 0){ _LL2285:
_temp2284=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2232)->f1; goto _LL2283; _LL2283: _temp2282=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2232)->f2; goto _LL2281; _LL2281:
_temp2280=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2232)->f3; goto _LL2243;} else{ goto _LL2244;} _LL2244: if(( unsigned int)
_temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_While_s: 0){ _LL2289: _temp2288=(
struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp2232)->f1; _LL2293:
_temp2292= _temp2288.f1; goto _LL2291; _LL2291: _temp2290= _temp2288.f2; goto
_LL2287; _LL2287: _temp2286=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp2232)->f2; goto _LL2245;} else{ goto _LL2246;} _LL2246: if(( unsigned int)
_temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_Break_s: 0){ _LL2295: _temp2294=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp2232)->f1; goto
_LL2247;} else{ goto _LL2248;} _LL2248: if(( unsigned int) _temp2232 > 1u?*((
int*) _temp2232) == Cyc_Absyn_Continue_s: 0){ _LL2297: _temp2296=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp2232)->f1; goto _LL2249;} else{ goto
_LL2250;} _LL2250: if(( unsigned int) _temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_Goto_s:
0){ _LL2301: _temp2300=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp2232)->f1; goto _LL2299; _LL2299: _temp2298=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp2232)->f2; goto _LL2251;} else{ goto
_LL2252;} _LL2252: if(( unsigned int) _temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_For_s:
0){ _LL2317: _temp2316=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp2232)->f1; goto _LL2311; _LL2311: _temp2310=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp2232)->f2; _LL2315: _temp2314= _temp2310.f1; goto _LL2313; _LL2313:
_temp2312= _temp2310.f2; goto _LL2305; _LL2305: _temp2304=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp2232)->f3; _LL2309: _temp2308= _temp2304.f1;
goto _LL2307; _LL2307: _temp2306= _temp2304.f2; goto _LL2303; _LL2303: _temp2302=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp2232)->f4; goto
_LL2253;} else{ goto _LL2254;} _LL2254: if(( unsigned int) _temp2232 > 1u?*((
int*) _temp2232) == Cyc_Absyn_Switch_s: 0){ _LL2321: _temp2320=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp2232)->f1; goto _LL2319; _LL2319:
_temp2318=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp2232)->f2; goto _LL2255;} else{ goto _LL2256;} _LL2256: if(( unsigned int)
_temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_Fallthru_s: 0){ _LL2325:
_temp2324=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2232)->f1; goto _LL2323; _LL2323: _temp2322=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp2232)->f2; goto _LL2257;} else{ goto
_LL2258;} _LL2258: if(( unsigned int) _temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_Decl_s:
0){ _LL2329: _temp2328=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp2232)->f1; goto _LL2327; _LL2327: _temp2326=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp2232)->f2; goto _LL2259;} else{ goto
_LL2260;} _LL2260: if(( unsigned int) _temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_Cut_s:
0){ _LL2331: _temp2330=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp2232)->f1; goto _LL2261;} else{ goto _LL2262;} _LL2262: if(( unsigned int)
_temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_Splice_s: 0){ _LL2333:
_temp2332=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp2232)->f1; goto _LL2263;} else{ goto _LL2264;} _LL2264: if(( unsigned int)
_temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_Label_s: 0){ _LL2337: _temp2336=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp2232)->f1; goto
_LL2335; _LL2335: _temp2334=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp2232)->f2; goto _LL2265;} else{ goto _LL2266;} _LL2266: if(( unsigned int)
_temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_Do_s: 0){ _LL2345: _temp2344=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp2232)->f1; goto
_LL2339; _LL2339: _temp2338=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp2232)->f2; _LL2343: _temp2342= _temp2338.f1; goto _LL2341; _LL2341:
_temp2340= _temp2338.f2; goto _LL2267;} else{ goto _LL2268;} _LL2268: if((
unsigned int) _temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_TryCatch_s: 0){
_LL2349: _temp2348=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2232)->f1; goto _LL2347; _LL2347: _temp2346=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp2232)->f2; goto _LL2269;} else{ goto
_LL2270;} _LL2270: if(( unsigned int) _temp2232 > 1u?*(( int*) _temp2232) == Cyc_Absyn_Region_s:
0){ _LL2355: _temp2354=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2232)->f1; goto _LL2353; _LL2353: _temp2352=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp2232)->f2; goto _LL2351; _LL2351:
_temp2350=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2232)->f3; goto _LL2271;} else{ goto _LL2233;} _LL2235: return; _LL2237:
Cyc_Toc_exp_to_c( nv, _temp2272); return; _LL2239: Cyc_Toc_stmt_to_c( nv,
_temp2276); s= _temp2274; continue; _LL2241: { struct Cyc_Core_Opt* topt= 0; if(
_temp2278 != 0){ topt=({ struct Cyc_Core_Opt* _temp2356=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2356->v=( void*) Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp2358=({ struct Cyc_Absyn_Exp* _temp2357=
_temp2278; if( _temp2357 == 0){ _throw( Null_Exception);} _temp2357->topt;});
if( _temp2358 == 0){ _throw( Null_Exception);} _temp2358->v;})); _temp2356;});
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2359=
_temp2278; if( _temp2359 == 0){ _throw( Null_Exception);} _temp2359;}));} if( s->try_depth
> 0){ if( topt != 0){ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
retn_stmt= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x,
0), 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( x,( void*)({
struct Cyc_Core_Opt* _temp2360= topt; if( _temp2360 == 0){ _throw(
Null_Exception);} _temp2360->v;}), _temp2278, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), retn_stmt, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL2243: Cyc_Toc_exp_to_c( nv, _temp2284); Cyc_Toc_stmt_to_c( nv,
_temp2282); s= _temp2280; continue; _LL2245: Cyc_Toc_exp_to_c( nv, _temp2292);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2286); return; _LL2247: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)({ struct Cyc_Core_Opt* _temp2361= nv->break_lab; if( _temp2361
== 0){ _throw( Null_Exception);} _temp2361->v;}), 0))->r));}{ int dest_depth=
_temp2294 == 0? 0:({ struct Cyc_Absyn_Stmt* _temp2362= _temp2294; if( _temp2362
== 0){ _throw( Null_Exception);} _temp2362->try_depth;}); Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2249: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)({
struct Cyc_Core_Opt* _temp2363= nv->continue_lab; if( _temp2363 == 0){ _throw(
Null_Exception);} _temp2363->v;}), 0))->r));} Cyc_Toc_do_npop_before( s->try_depth
-({ struct Cyc_Absyn_Stmt* _temp2364= _temp2296; if( _temp2364 == 0){ _throw(
Null_Exception);} _temp2364->try_depth;}), s); return; _LL2251: Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2365= _temp2298; if( _temp2365 == 0){
_throw( Null_Exception);} _temp2365->try_depth;}), s); return; _LL2253: Cyc_Toc_exp_to_c(
nv, _temp2316); Cyc_Toc_exp_to_c( nv, _temp2314); Cyc_Toc_exp_to_c( nv,
_temp2308); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2302); return;
_LL2255: Cyc_Toc_xlate_switch( nv, s, _temp2320, _temp2318); return; _LL2257:
if( nv->fallthru_info == 0){( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2366=( char*)"fallthru in unexpected place";
struct _tagged_string _temp2367; _temp2367.curr= _temp2366; _temp2367.base=
_temp2366; _temp2367.last_plus_one= _temp2366 + 29; _temp2367;})));}{ struct
_tuple8 _temp2371; struct Cyc_Dict_Dict* _temp2372; struct Cyc_List_List*
_temp2374; struct _tagged_string* _temp2376; struct _tuple8* _temp2369=( struct
_tuple8*)({ struct Cyc_Core_Opt* _temp2368= nv->fallthru_info; if( _temp2368 ==
0){ _throw( Null_Exception);} _temp2368->v;}); _temp2371=* _temp2369; _LL2377:
_temp2376= _temp2371.f1; goto _LL2375; _LL2375: _temp2374= _temp2371.f2; goto
_LL2373; _LL2373: _temp2372= _temp2371.f3; goto _LL2370; _LL2370: { struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_goto_stmt( _temp2376, 0); Cyc_Toc_do_npop_before( s->try_depth -(({
struct Cyc_Absyn_Switch_clause** _temp2378= _temp2322; if( _temp2378 == 0){
_throw( Null_Exception);}* _temp2378;})->body)->try_depth, s2);{ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2374); struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2324); for( 0; vs2 != 0; vs2=({ struct Cyc_List_List*
_temp2379= vs2; if( _temp2379 == 0){ _throw( Null_Exception);} _temp2379->tl;}),
es2=({ struct Cyc_List_List* _temp2380= es2; if( _temp2380 == 0){ _throw(
Null_Exception);} _temp2380->tl;})){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp2381= es2; if( _temp2381 == 0){ _throw(
Null_Exception);} _temp2381->hd;})); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2372,( struct _tuple0*)({ struct Cyc_List_List* _temp2382= vs2; if(
_temp2382 == 0){ _throw( Null_Exception);} _temp2382->hd;})),( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp2383= es2; if( _temp2383 == 0){ _throw(
Null_Exception);} _temp2383->hd;}), 0), s2, 0);}( void*)( s->r=( void*)(( void*)
s2->r)); return;}}} _LL2259:{ void* _temp2384=( void*) _temp2328->r; int
_temp2392; struct Cyc_Absyn_Exp* _temp2394; struct Cyc_Core_Opt* _temp2396;
struct Cyc_Core_Opt* _temp2398; struct Cyc_Absyn_Pat* _temp2400; struct Cyc_Absyn_Vardecl*
_temp2402; _LL2386: if(*(( int*) _temp2384) == Cyc_Absyn_Let_d){ _LL2401:
_temp2400=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2384)->f1;
goto _LL2399; _LL2399: _temp2398=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2384)->f2; goto _LL2397; _LL2397: _temp2396=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2384)->f3; goto _LL2395; _LL2395: _temp2394=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2384)->f4; goto
_LL2393; _LL2393: _temp2392=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2384)->f5;
goto _LL2387;} else{ goto _LL2388;} _LL2388: if(*(( int*) _temp2384) == Cyc_Absyn_Var_d){
_LL2403: _temp2402=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2384)->f1; goto _LL2389;} else{ goto _LL2390;} _LL2390: goto _LL2391;
_LL2387: { int var_case= 0; struct Cyc_Absyn_Vardecl* vdopt= 0;{ void* _temp2404=(
void*) _temp2400->r; struct Cyc_Absyn_Vardecl* _temp2410; _LL2406: if((
unsigned int) _temp2404 > 2u?*(( int*) _temp2404) == Cyc_Absyn_Var_p: 0){
_LL2411: _temp2410=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp2404)->f1; goto _LL2407;} else{ goto _LL2408;} _LL2408: goto _LL2409;
_LL2407: var_case= 1; vdopt=( struct Cyc_Absyn_Vardecl*) _temp2410; _temp2410->initializer=(
struct Cyc_Absyn_Exp*) _temp2394;( void*)( _temp2328->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2412=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2412[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2413; _temp2413.tag= Cyc_Absyn_Var_d; _temp2413.f1= _temp2410; _temp2413;});
_temp2412;}))); goto _LL2405; _LL2409: goto _LL2405; _LL2405:;} if( var_case){
_temp2402=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_Absyn_Vardecl* _temp2414=
vdopt; if( _temp2414 == 0){ _throw( Null_Exception);} _temp2414;}); goto _LL2389;}(
void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2400, _temp2396,
_temp2394, _temp2392, _temp2326))->r)); goto _LL2385;} _LL2389: { struct _tuple0*
c_name= _temp2402->name;( void*)( _temp2402->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2402->type));{ struct Cyc_Toc_Env* nv2= Cyc_Toc_add_varmap( nv,
c_name, Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2428=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2428[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2429; _temp2429.tag= Cyc_Absyn_Local_b;
_temp2429.f1= _temp2402; _temp2429;}); _temp2428;}), 0)); Cyc_Toc_stmt_to_c( nv2,
_temp2326); if( _temp2402->initializer != 0){ struct Cyc_Absyn_Exp* init=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2427= _temp2402->initializer;
if( _temp2427 == 0){ _throw( Null_Exception);} _temp2427;}); void* _temp2415=(
void*) init->r; struct Cyc_Absyn_Exp* _temp2421; struct Cyc_Absyn_Exp* _temp2423;
struct Cyc_Absyn_Vardecl* _temp2425; _LL2417: if(*(( int*) _temp2415) == Cyc_Absyn_Comprehension_e){
_LL2426: _temp2425=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2415)->f1; goto _LL2424; _LL2424: _temp2423=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2415)->f2; goto _LL2422; _LL2422:
_temp2421=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2415)->f3; goto _LL2418;} else{ goto _LL2419;} _LL2419: goto _LL2420;
_LL2418: _temp2402->initializer= 0;( void*)( _temp2326->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( c_name, 0), _temp2425, _temp2423, _temp2421, Cyc_Absyn_new_stmt((
void*) _temp2326->r, 0), 0))->r)); goto _LL2416; _LL2420: Cyc_Toc_exp_to_c( nv,
init); goto _LL2416; _LL2416:;} goto _LL2385;}} _LL2391:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2430=(
char*)"bad nested declaration within function"; struct _tagged_string _temp2431;
_temp2431.curr= _temp2430; _temp2431.base= _temp2430; _temp2431.last_plus_one=
_temp2430 + 39; _temp2431;})); goto _LL2385; _LL2385:;} return; _LL2261:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp2432=( char*)"cut"; struct _tagged_string _temp2433; _temp2433.curr=
_temp2432; _temp2433.base= _temp2432; _temp2433.last_plus_one= _temp2432 + 4;
_temp2433;})); return; _LL2263:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2434=( char*)"splice"; struct _tagged_string
_temp2435; _temp2435.curr= _temp2434; _temp2435.base= _temp2434; _temp2435.last_plus_one=
_temp2434 + 7; _temp2435;})); return; _LL2265: s= _temp2334; continue; _LL2267:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2344); Cyc_Toc_exp_to_c( nv,
_temp2342); return; _LL2269: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct
_tuple0* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2451=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2451->f1= Cyc_Toc_abs_ns; _temp2451->f2= Cyc_Toc__push_handler_sp;
_temp2451;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple0* _temp2450=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2450->f1= Cyc_Toc_abs_ns; _temp2450->f2= Cyc_Toc_setjmp_sp; _temp2450;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2449=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2449->f1=
Cyc_Toc_abs_ns; _temp2449->f2= Cyc_Toc__pop_handler_sp; _temp2449;}), 0); e_exp->topt=({
struct Cyc_Core_Opt* _temp2436=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2436->v=( void*) e_typ; _temp2436;}); nv= Cyc_Toc_add_varmap(
nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2348);{ struct Cyc_Absyn_Stmt*
tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp2348, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0); struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl* x_vd= Cyc_Absyn_new_vardecl(
x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt* _temp2437=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2437->v=( void*)
Cyc_Absyn_exn_typ; _temp2437;});{ struct Cyc_Absyn_Pat* p=({ struct Cyc_Absyn_Pat*
_temp2445=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof( struct Cyc_Absyn_Pat));
_temp2445->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp2447=(
struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2447[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2448; _temp2448.tag= Cyc_Absyn_Var_p;
_temp2448.f1= x_vd; _temp2448;}); _temp2447;})); _temp2445->topt=({ struct Cyc_Core_Opt*
_temp2446=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2446->v=( void*) Cyc_Absyn_exn_typ; _temp2446;}); _temp2445->loc= 0;
_temp2445;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp2438=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2438->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2438;});{
struct Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp2442=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2442->pattern= p;
_temp2442->pat_vars=({ struct Cyc_Core_Opt* _temp2443=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2443->v=( void*)({ struct Cyc_List_List*
_temp2444=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2444->hd=( void*) x_vd; _temp2444->tl= 0; _temp2444;}); _temp2443;});
_temp2442->where_clause= 0; _temp2442->body= rs; _temp2442->loc= 0; _temp2442;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2346,({
struct Cyc_List_List* _temp2441=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2441->hd=( void*) default_case; _temp2441->tl= 0;
_temp2441;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( e_typ, Cyc_Absyn_fncall_exp( setjmp_exp,({
struct Cyc_List_List* _temp2440=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2440->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2440->tl= 0; _temp2440;}), 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List*
_temp2439=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2439->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2439->tl= 0;
_temp2439;}), 0), 0), Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Not,
e_exp, 0), tryandpop_stmt, handler_stmt, 0), 0), 0), 0))->r)); return;}}}}}
_LL2271: { void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__region_handle_sp);
void* rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq);
void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var=
Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp2352->name; struct Cyc_Absyn_Exp*
rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp(
x_var,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp2457=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2457[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2458; _temp2458.tag= Cyc_Absyn_Local_b; _temp2458.f1=
_temp2352; _temp2458;}); _temp2457;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2456=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2456->f1= Cyc_Toc_abs_ns; _temp2456->f2= Cyc_Toc__push_region_sp;
_temp2456;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2455=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2455->f1= Cyc_Toc_abs_ns; _temp2455->f2= Cyc_Toc__pop_region_sp; _temp2455;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2454=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2454->f1=
Cyc_Toc_abs_ns; _temp2454->f2= Cyc_Toc__new_region_sp; _temp2454;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2453=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2453->hd=( void*) addr_rh_exp; _temp2453->tl= 0; _temp2453;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2452=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2452->hd=( void*) x_exp; _temp2452->tl= 0; _temp2452;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2350);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2350, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2233:;}} struct _tuple17{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0;(
void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{ struct
Cyc_List_List* args= f->args; for( 0; args != 0; args=({ struct Cyc_List_List*
_temp2459= args; if( _temp2459 == 0){ _throw( Null_Exception);} _temp2459->tl;})){(*((
struct _tuple17*)({ struct Cyc_List_List* _temp2460= args; if( _temp2460 == 0){
_throw( Null_Exception);} _temp2460->hd;}))).f3= Cyc_Toc_typ_to_c((*(( struct
_tuple17*)({ struct Cyc_List_List* _temp2461= args; if( _temp2461 == 0){ _throw(
Null_Exception);} _temp2461->hd;}))).f3);{ struct _tuple0* x=({ struct _tuple0*
_temp2462=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2462->f1=(
void*) Cyc_Absyn_Loc_n; _temp2462->f2=(*(( struct _tuple17*)({ struct Cyc_List_List*
_temp2463= args; if( _temp2463 == 0){ _throw( Null_Exception);} _temp2463->hd;}))).f1;
_temp2462;}); nv= Cyc_Toc_add_varmap( nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel( nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){
void* _temp2464= s; _LL2466: if( _temp2464 ==( void*) Cyc_Absyn_Abstract){ goto
_LL2467;} else{ goto _LL2468;} _LL2468: if( _temp2464 ==( void*) Cyc_Absyn_ExternC){
goto _LL2469;} else{ goto _LL2470;} _LL2470: goto _LL2471; _LL2467: return( void*)
Cyc_Absyn_Public; _LL2469: return( void*) Cyc_Absyn_Extern; _LL2471: return s;
_LL2465:;} struct Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* n=( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2484= s->name; if( _temp2484 == 0){ _throw( Null_Exception);}
_temp2484->v;}); if( Cyc_Toc_structs_so_far == 0){ Cyc_Toc_structs_so_far=({
struct Cyc_Core_Opt* _temp2472=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2472->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2472;});}{ int seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2483= Cyc_Toc_structs_so_far;
if( _temp2483 == 0){ _throw( Null_Exception);} _temp2483->v;}),(* n).f2); if(
dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2473= Cyc_Toc_structs_so_far; if( _temp2473 == 0){ _throw( Null_Exception);}
_temp2473->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2474= Cyc_Toc_structs_so_far;
if( _temp2474 == 0){ _throw( Null_Exception);} _temp2474->v;}),(* n).f2, s);});}
else{ if((( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Core_Opt* _temp2475= dopt;
if( _temp2475 == 0){ _throw( Null_Exception);} _temp2475->v;}))->fields == 0){(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2476= Cyc_Toc_structs_so_far;
if( _temp2476 == 0){ _throw( Null_Exception);} _temp2476->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2477= Cyc_Toc_structs_so_far; if( _temp2477 == 0){ _throw(
Null_Exception);} _temp2477->v;}),(* n).f2, s);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) s->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) s->sc))); s->tvs= 0; s->name=({
struct Cyc_Core_Opt* _temp2478=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2478->v=( void*) n; _temp2478;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2482= s->fields; if( _temp2482 == 0){
_throw( Null_Exception);} _temp2482->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2479= fields; if( _temp2479 == 0){ _throw( Null_Exception);}
_temp2479->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2480= fields; if( _temp2480 == 0){ _throw( Null_Exception);} _temp2480->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2481= fields; if( _temp2481 == 0){ _throw( Null_Exception);}
_temp2481->hd;}))->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* n=( struct _tuple0*)({ struct Cyc_Core_Opt* _temp2497= u->name; if(
_temp2497 == 0){ _throw( Null_Exception);} _temp2497->v;}); if( Cyc_Toc_unions_so_far
== 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2485=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2485->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2485;});}{ int seen_defn_before; struct Cyc_Core_Opt*
dopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2496= Cyc_Toc_unions_so_far; if( _temp2496 == 0){ _throw( Null_Exception);}
_temp2496->v;}),(* n).f2); if( dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2486= Cyc_Toc_unions_so_far; if( _temp2486 == 0){
_throw( Null_Exception);} _temp2486->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2487= Cyc_Toc_unions_so_far; if( _temp2487 == 0){ _throw( Null_Exception);}
_temp2487->v;}),(* n).f2, u);});} else{ if((( struct Cyc_Absyn_Uniondecl*)({
struct Cyc_Core_Opt* _temp2488= dopt; if( _temp2488 == 0){ _throw(
Null_Exception);} _temp2488->v;}))->fields == 0){( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2489= Cyc_Toc_unions_so_far; if( _temp2489 == 0){
_throw( Null_Exception);} _temp2489->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2490= Cyc_Toc_unions_so_far; if( _temp2490 == 0){ _throw( Null_Exception);}
_temp2490->v;}),(* n).f2, u);}); seen_defn_before= 0;} else{ seen_defn_before= 1;}}(
void*)( u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*)
u->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2491=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2491->v=( void*) n; _temp2491;}); if( u->fields != 0){ if(
seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2495= u->fields; if( _temp2495 == 0){
_throw( Null_Exception);} _temp2495->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2492= fields; if( _temp2492 == 0){ _throw( Null_Exception);}
_temp2492->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2493= fields; if( _temp2493 == 0){ _throw( Null_Exception);} _temp2493->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2494= fields; if( _temp2494 == 0){ _throw( Null_Exception);}
_temp2494->hd;}))->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2498=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2498->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2498;});}{ struct
_tuple0* n= tud->name; if( tud->fields == 0){ return;} if((( int(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2499= Cyc_Toc_tunions_so_far; if( _temp2499 == 0){ _throw( Null_Exception);}
_temp2499->v;}), n)){ return;}( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2500= Cyc_Toc_tunions_so_far; if( _temp2500 == 0){ _throw( Null_Exception);}
_temp2500->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct
_tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt*
_temp2501= Cyc_Toc_tunions_so_far; if( _temp2501 == 0){ _throw( Null_Exception);}
_temp2501->v;}), n);});{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2528= tud->fields;
if( _temp2528 == 0){ _throw( Null_Exception);} _temp2528->v;}); for( 0; fields
!= 0; fields=({ struct Cyc_List_List* _temp2502= fields; if( _temp2502 == 0){
_throw( Null_Exception);} _temp2502->tl;})){ struct Cyc_Absyn_Tunionfield* f=(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2527= fields; if(
_temp2527 == 0){ _throw( Null_Exception);} _temp2527->hd;}); if( f->typs == 0){
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
tag_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2503; _temp2503.q_const=
1; _temp2503.q_volatile= 0; _temp2503.q_restrict= 0; _temp2503;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2504=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2504->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2505=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2505[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2506; _temp2506.tag= Cyc_Absyn_Var_d; _temp2506.f1= vd; _temp2506;});
_temp2505;}), 0); _temp2504->tl= Cyc_Toc_result_decls; _temp2504;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
box_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2507; _temp2507.q_const=
1; _temp2507.q_volatile= 0; _temp2507.q_restrict= 0; _temp2507;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2508=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2508->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2509=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2509[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2510; _temp2510.tag= Cyc_Absyn_Var_d; _temp2510.f1= vd; _temp2510;});
_temp2509;}), 0); _temp2508->tl= Cyc_Toc_result_decls; _temp2508;});{ struct Cyc_List_List*
fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts=({
struct Cyc_List_List* _temp2511= ts; if( _temp2511 == 0){ _throw( Null_Exception);}
_temp2511->tl;}), i ++){ struct _tagged_string* fname=( struct _tagged_string*)({
struct _tagged_string* _temp2516=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp2516[ 0]= xprintf("f%d", i); _temp2516;}); struct
Cyc_Absyn_Structfield* f=({ struct Cyc_Absyn_Structfield* _temp2513=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2513->name= fname;
_temp2513->tq=(*(( struct _tuple3*)({ struct Cyc_List_List* _temp2515= ts; if(
_temp2515 == 0){ _throw( Null_Exception);} _temp2515->hd;}))).f1; _temp2513->type=(
void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp2514= ts; if( _temp2514 == 0){ _throw( Null_Exception);} _temp2514->hd;}))).f2);
_temp2513->width= 0; _temp2513->attributes= 0; _temp2513;}); fs=({ struct Cyc_List_List*
_temp2512=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2512->hd=( void*) f; _temp2512->tl= fs; _temp2512;});}} fs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs); fs=({ struct Cyc_List_List*
_temp2517=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2517->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2518=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2518->name= Cyc_Toc_tag_sp;
_temp2518->tq= Cyc_Toc_mt_tq; _temp2518->type=( void*) Cyc_Absyn_sint_t;
_temp2518->width= 0; _temp2518->attributes= 0; _temp2518;}); _temp2517->tl= fs;
_temp2517;});{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp2522=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2522->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2522->name=({ struct Cyc_Core_Opt*
_temp2524=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2524->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2525=( char*)"_struct"; struct _tagged_string _temp2526; _temp2526.curr=
_temp2525; _temp2526.base= _temp2525; _temp2526.last_plus_one= _temp2525 + 8;
_temp2526;})); _temp2524;}); _temp2522->tvs= 0; _temp2522->fields=({ struct Cyc_Core_Opt*
_temp2523=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2523->v=( void*) fs; _temp2523;}); _temp2522->attributes= 0; _temp2522;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2519=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2519->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2520=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2520[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2521; _temp2521.tag= Cyc_Absyn_Struct_d;
_temp2521.f1= sd; _temp2521;}); _temp2520;}), 0); _temp2519->tl= Cyc_Toc_result_decls;
_temp2519;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2529=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2529->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2529;});} if( xd->fields
== 0){ return;}{ struct _tuple0* n= xd->name; struct Cyc_List_List* fs=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2563= xd->fields; if( _temp2563 == 0){
_throw( Null_Exception);} _temp2563->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp2530= fs; if( _temp2530 == 0){ _throw( Null_Exception);} _temp2530->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp2562= fs; if( _temp2562 == 0){ _throw( Null_Exception);} _temp2562->hd;});
struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_uint_exp(({
struct _tagged_string _temp2561=* fn;( unsigned int)( _temp2561.last_plus_one -
_temp2561.curr);}) + 4, 0); struct Cyc_Absyn_ArrayType_struct* tag_typ=({ struct
Cyc_Absyn_ArrayType_struct* _temp2559=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2559[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2560; _temp2560.tag= Cyc_Absyn_ArrayType;
_temp2560.f1=( void*) Cyc_Absyn_uchar_t; _temp2560.f2= Cyc_Toc_mt_tq; _temp2560.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp2560;}); _temp2559;}); struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) xd->sc !=( void*) Cyc_Absyn_Extern? !(( int(*)( struct
Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)({
struct Cyc_Core_Opt* _temp2531= Cyc_Toc_xtunion_fields_so_far; if( _temp2531 ==
0){ _throw( Null_Exception);} _temp2531->v;}), f->name): 0){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(({ int _temp2532= 0; int _temp2533= 0; int
_temp2534= 0; int _temp2535= 0; struct _tagged_string _temp2536=* fn; xprintf("%c%c%c%c%.*s",
_temp2532, _temp2533, _temp2534, _temp2535, _temp2536.last_plus_one - _temp2536.curr,
_temp2536.curr);}), 0);}{ struct Cyc_Absyn_Vardecl* tag_decl= Cyc_Absyn_new_vardecl(
f->name,( void*) tag_typ, initopt);( void*)( tag_decl->sc=( void*)(( void*) xd->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2537=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2537->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2538=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2538[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2539; _temp2539.tag= Cyc_Absyn_Var_d; _temp2539.f1= tag_decl; _temp2539;});
_temp2538;}), 0); _temp2537->tl= Cyc_Toc_result_decls; _temp2537;}); if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)({
struct Cyc_Core_Opt* _temp2540= Cyc_Toc_xtunion_fields_so_far; if( _temp2540 ==
0){ _throw( Null_Exception);} _temp2540->v;}), f->name)){ continue;}( struct Cyc_Set_Set*)({
struct Cyc_Core_Opt* _temp2541= Cyc_Toc_xtunion_fields_so_far; if( _temp2541 ==
0){ _throw( Null_Exception);} _temp2541->v=( void*)(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)({
struct Cyc_Core_Opt* _temp2542= Cyc_Toc_xtunion_fields_so_far; if( _temp2542 ==
0){ _throw( Null_Exception);} _temp2542->v;}), f->name);}); if( f->typs != 0){
struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List* tqts= f->typs;
for( 0; tqts != 0; tqts=({ struct Cyc_List_List* _temp2543= tqts; if( _temp2543
== 0){ _throw( Null_Exception);} _temp2543->tl;}), i ++){ struct _tagged_string*
field_n=( struct _tagged_string*)({ struct _tagged_string* _temp2548=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2548[ 0]=
xprintf("f%d", i); _temp2548;}); struct Cyc_Absyn_Structfield* newf=({ struct
Cyc_Absyn_Structfield* _temp2545=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp2545->name= field_n; _temp2545->tq=(*((
struct _tuple3*)({ struct Cyc_List_List* _temp2547= tqts; if( _temp2547 == 0){
_throw( Null_Exception);} _temp2547->hd;}))).f1; _temp2545->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)({ struct Cyc_List_List* _temp2546= tqts; if( _temp2546 == 0){
_throw( Null_Exception);} _temp2546->hd;}))).f2); _temp2545->width= 0; _temp2545->attributes=
0; _temp2545;}); fields=({ struct Cyc_List_List* _temp2544=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2544->hd=( void*) newf;
_temp2544->tl= fields; _temp2544;});}} fields=({ struct Cyc_List_List* _temp2549=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2549->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp2550=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2550->name= Cyc_Toc_tag_sp;
_temp2550->tq= Cyc_Toc_mt_tq; _temp2550->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2550->width= 0; _temp2550->attributes= 0; _temp2550;});
_temp2549->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2549;});{ struct Cyc_Absyn_Structdecl* strct_decl=({ struct Cyc_Absyn_Structdecl*
_temp2554=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2554->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2554->name=({ struct Cyc_Core_Opt*
_temp2556=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2556->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2557=( char*)"_struct"; struct _tagged_string _temp2558; _temp2558.curr=
_temp2557; _temp2558.base= _temp2557; _temp2558.last_plus_one= _temp2557 + 8;
_temp2558;})); _temp2556;}); _temp2554->tvs= 0; _temp2554->fields=({ struct Cyc_Core_Opt*
_temp2555=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2555->v=( void*) fields; _temp2555;}); _temp2554->attributes= 0; _temp2554;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2551=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2551->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2552=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2552[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2553; _temp2553.tag= Cyc_Absyn_Struct_d;
_temp2553.f1= strct_decl; _temp2553;}); _temp2552;}), 0); _temp2551->tl= Cyc_Toc_result_decls;
_temp2551;});}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* n= ed->name;( void*)( ed->sc=(
void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) ed->sc ==( void*)
Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public:
Cyc_Toc_scope_to_c(( void*) ed->sc)));{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp2564= fs; if( _temp2564 == 0){
_throw( Null_Exception);} _temp2564->tl;})){ if((( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2565= fs; if( _temp2565 == 0){ _throw( Null_Exception);}
_temp2565->hd;}))->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2568=(( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2567= fs; if( _temp2567 == 0){ _throw( Null_Exception);}
_temp2567->hd;}))->tag; if( _temp2568 == 0){ _throw( Null_Exception);} _temp2568;}));((
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp2566= fs; if(
_temp2566 == 0){ _throw( Null_Exception);} _temp2566->hd;}))->tag=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)({
struct Cyc_Core_Opt* _temp2587= topt; if( _temp2587 == 0){ _throw(
Null_Exception);} _temp2587->v;}); Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x=
Cyc_Toc_temp_var(); struct _tagged_string* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_string* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();
struct Cyc_Absyn_Stmt* _temp2571; struct Cyc_List_List* _temp2573; struct Cyc_Toc_Env*
_temp2575; struct _tuple13 _temp2569= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL2576: _temp2575=
_temp2569.f1; goto _LL2574; _LL2574: _temp2573= _temp2569.f2; goto _LL2572;
_LL2572: _temp2571= _temp2569.f3; goto _LL2570; _LL2570: Cyc_Toc_stmt_to_c(
_temp2575, s);{ struct Cyc_Absyn_Stmt* succ_stmt= Cyc_Absyn_label_stmt( succ_lab,
s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2571, succ_stmt, 0), 0);} else{
struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({ struct
_tuple0* _temp2577=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2577->f1= Cyc_Toc_abs_ns; _temp2577->f2= Cyc_Toc_Match_Exception_sp;
_temp2577;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2571, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp2573 != 0; _temp2573=({ struct
Cyc_List_List* _temp2578= _temp2573; if( _temp2578 == 0){ _throw( Null_Exception);}
_temp2578->tl;})){ struct _tuple14 _temp2582; void* _temp2583; struct _tuple0*
_temp2585; struct _tuple14* _temp2580=( struct _tuple14*)({ struct Cyc_List_List*
_temp2579= _temp2573; if( _temp2579 == 0){ _throw( Null_Exception);} _temp2579->hd;});
_temp2582=* _temp2580; _LL2586: _temp2585= _temp2582.f1; goto _LL2584; _LL2584:
_temp2583= _temp2582.f2; goto _LL2581; _LL2581: s= Cyc_Absyn_declare_stmt(
_temp2585, _temp2583, 0, s, 0);} return s;}}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=({
struct Cyc_List_List* _temp2588= ds; if( _temp2588 == 0){ _throw( Null_Exception);}
_temp2588->tl;})){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct
Cyc_List_List* _temp2662= ds; if( _temp2662 == 0){ _throw( Null_Exception);}
_temp2662->hd;}); void* _temp2589=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2613; struct Cyc_Absyn_Fndecl* _temp2615; int _temp2617; struct Cyc_Absyn_Exp*
_temp2619; struct Cyc_Core_Opt* _temp2621; struct Cyc_Core_Opt* _temp2623;
struct Cyc_Absyn_Pat* _temp2625; struct Cyc_Absyn_Structdecl* _temp2627; struct
Cyc_Absyn_Uniondecl* _temp2629; struct Cyc_Absyn_Tuniondecl* _temp2631; struct
Cyc_Absyn_Enumdecl* _temp2633; struct Cyc_Absyn_Typedefdecl* _temp2635; struct
Cyc_List_List* _temp2637; struct _tagged_string* _temp2639; struct Cyc_List_List*
_temp2641; struct _tuple0* _temp2643; struct Cyc_List_List* _temp2645; _LL2591:
if(*(( int*) _temp2589) == Cyc_Absyn_Var_d){ _LL2614: _temp2613=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp2589)->f1; goto _LL2592;} else{ goto
_LL2593;} _LL2593: if(*(( int*) _temp2589) == Cyc_Absyn_Fn_d){ _LL2616:
_temp2615=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp2589)->f1;
goto _LL2594;} else{ goto _LL2595;} _LL2595: if(*(( int*) _temp2589) == Cyc_Absyn_Let_d){
_LL2626: _temp2625=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2589)->f1; goto _LL2624; _LL2624: _temp2623=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2589)->f2; goto _LL2622; _LL2622: _temp2621=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2589)->f3; goto
_LL2620; _LL2620: _temp2619=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2589)->f4; goto _LL2618; _LL2618: _temp2617=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp2589)->f5; goto _LL2596;} else{ goto _LL2597;} _LL2597: if(*(( int*)
_temp2589) == Cyc_Absyn_Struct_d){ _LL2628: _temp2627=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp2589)->f1; goto _LL2598;} else{ goto
_LL2599;} _LL2599: if(*(( int*) _temp2589) == Cyc_Absyn_Union_d){ _LL2630:
_temp2629=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp2589)->f1; goto _LL2600;} else{ goto _LL2601;} _LL2601: if(*(( int*)
_temp2589) == Cyc_Absyn_Tunion_d){ _LL2632: _temp2631=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp2589)->f1; goto _LL2602;} else{ goto
_LL2603;} _LL2603: if(*(( int*) _temp2589) == Cyc_Absyn_Enum_d){ _LL2634:
_temp2633=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp2589)->f1; goto _LL2604;} else{ goto _LL2605;} _LL2605: if(*(( int*)
_temp2589) == Cyc_Absyn_Typedef_d){ _LL2636: _temp2635=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp2589)->f1; goto _LL2606;} else{ goto
_LL2607;} _LL2607: if(*(( int*) _temp2589) == Cyc_Absyn_Namespace_d){ _LL2640:
_temp2639=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2589)->f1; goto _LL2638; _LL2638: _temp2637=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2589)->f2; goto _LL2608;} else{ goto
_LL2609;} _LL2609: if(*(( int*) _temp2589) == Cyc_Absyn_Using_d){ _LL2644:
_temp2643=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp2589)->f1;
goto _LL2642; _LL2642: _temp2641=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2589)->f2; goto _LL2610;} else{ goto _LL2611;} _LL2611: if(*(( int*)
_temp2589) == Cyc_Absyn_ExternC_d){ _LL2646: _temp2645=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp2589)->f1; goto _LL2612;} else{ goto
_LL2590;} _LL2592: { struct _tuple0* c_name= _temp2613->name; if(( void*)
_temp2613->sc ==( void*) Cyc_Absyn_ExternC){ c_name=({ struct _tuple0* _temp2647=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2647->f1= Cyc_Toc_abs_ns;
_temp2647->f2=(* c_name).f2; _temp2647;});} if( _temp2613->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2648=
_temp2613->initializer; if( _temp2648 == 0){ _throw( Null_Exception);} _temp2648;}));}
if( ! nv->toplevel){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp2649=( char*)"decls_to_c: not at toplevel!";
struct _tagged_string _temp2650; _temp2650.curr= _temp2649; _temp2650.base=
_temp2649; _temp2650.last_plus_one= _temp2649 + 29; _temp2650;}));} nv= Cyc_Toc_add_varmap(
nv, _temp2613->name, Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2651=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2651[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2652; _temp2652.tag= Cyc_Absyn_Global_b;
_temp2652.f1= _temp2613; _temp2652;}); _temp2651;}), 0)); _temp2613->name=
c_name;( void*)( _temp2613->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2613->sc));(
void*)( _temp2613->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2613->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2653=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2653->hd=( void*) d; _temp2653->tl=
Cyc_Toc_result_decls; _temp2653;}); goto _LL2590;} _LL2594: { struct _tuple0*
c_name= _temp2615->name; if(( void*) _temp2615->sc ==( void*) Cyc_Absyn_ExternC){
c_name=({ struct _tuple0* _temp2654=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp2654->f1= Cyc_Toc_abs_ns; _temp2654->f2=(* c_name).f2; _temp2654;});}
nv= Cyc_Toc_add_varmap( nv, _temp2615->name, Cyc_Absyn_var_exp( c_name, 0));
_temp2615->name= c_name; Cyc_Toc_fndecl_to_c( nv, _temp2615); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2655=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2655->hd=( void*) d; _temp2655->tl= Cyc_Toc_result_decls;
_temp2655;}); goto _LL2590;} _LL2596:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2656=( char*)"letdecl"; struct
_tagged_string _temp2657; _temp2657.curr= _temp2656; _temp2657.base= _temp2656;
_temp2657.last_plus_one= _temp2656 + 8; _temp2657;})); goto _LL2590; _LL2598:
Cyc_Toc_structdecl_to_c( _temp2627); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2658=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2658->hd=( void*) d; _temp2658->tl= Cyc_Toc_result_decls; _temp2658;});
goto _LL2590; _LL2600: Cyc_Toc_uniondecl_to_c( _temp2629); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2659=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2659->hd=( void*) d; _temp2659->tl= Cyc_Toc_result_decls;
_temp2659;}); goto _LL2590; _LL2602: if( _temp2631->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2631);} else{ Cyc_Toc_tuniondecl_to_c( _temp2631);} goto _LL2590; _LL2604:
Cyc_Toc_enumdecl_to_c( nv, _temp2633); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2660=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2660->hd=( void*) d; _temp2660->tl= Cyc_Toc_result_decls; _temp2660;});
goto _LL2590; _LL2606: _temp2635->name= _temp2635->name; _temp2635->tvs= 0;(
void*)( _temp2635->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2635->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2661=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2661->hd=( void*) d; _temp2661->tl=
Cyc_Toc_result_decls; _temp2661;}); goto _LL2590; _LL2608: _temp2641= _temp2637;
goto _LL2610; _LL2610: _temp2645= _temp2641; goto _LL2612; _LL2612: nv= Cyc_Toc_decls_to_c(
nv, _temp2645, top); goto _LL2590; _LL2590:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}