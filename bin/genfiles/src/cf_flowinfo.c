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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern int Cyc_Core_intcmp( int, int);
extern int Cyc_Core_ptrcmp( void**, void**); extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List* x); extern char Cyc_List_List_mismatch[
18u]; extern char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem( int(* compare)(
void*, void*), struct Cyc_List_List* l, void* x); extern int Cyc_List_list_cmp(
int(* cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern struct
Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern struct
Cyc_Set_Set* Cyc_Set_delete( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_is_empty(
struct Cyc_Set_Set* s); extern char Cyc_Set_Absent[ 11u]; extern void* Cyc_Set_choose(
struct Cyc_Set_Set* s); struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u];
extern char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern void Cyc_Dict_iter_c( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map_c( void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern
int Cyc_Dict_forall_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern int Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); struct Cyc_Stdio___sFILE; typedef
struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t; extern
char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[ 18u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern int Cyc_String_strcmp( struct _tagged_string s1, struct _tagged_string s2);
extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct _tagged_string*);
struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string
line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; typedef struct
Cyc_Position_Segment* Cyc_Position_seg_t; static const int Cyc_Position_Lex= 0;
static const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext[ 14u]; typedef struct _tagged_string* Cyc_Absyn_field_name_t;
typedef struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string*
Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t; struct _tuple0{ void*
f1; struct _tagged_string* f2; } ; typedef struct _tuple0* Cyc_Absyn_qvar_t;
typedef struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo;
struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp;
struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_VarargInfo
Cyc_Absyn_vararg_info_t; typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t; typedef struct Cyc_Absyn_TunionFieldInfo
Cyc_Absyn_tunion_field_info_t; typedef void* Cyc_Absyn_type_t; typedef void* Cyc_Absyn_rgntype_t;
typedef void* Cyc_Absyn_funcparams_t; typedef void* Cyc_Absyn_type_modifier_t;
typedef void* Cyc_Absyn_cnst_t; typedef void* Cyc_Absyn_primop_t; typedef void*
Cyc_Absyn_incrementor_t; typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void* Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
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
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
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
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att=
0; struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Stdcall_att= 0; static const int Cyc_Absyn_Cdecl_att= 1; static const
int Cyc_Absyn_Fastcall_att= 2; static const int Cyc_Absyn_Noreturn_att= 3;
static const int Cyc_Absyn_Const_att= 4; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
6; static const int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att=
8; static const int Cyc_Absyn_Weak_att= 9; static const int Cyc_Absyn_Dllimport_att=
10; static const int Cyc_Absyn_Dllexport_att= 11; static const int Cyc_Absyn_No_instrument_function_att=
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
struct Cyc_Absyn_VarargCallInfo{ int num_varargs; struct Cyc_List_List*
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
Cyc_Absyn_Primop_e_struct{ int tag; void* f1; struct Cyc_List_List* f2; } ;
static const int Cyc_Absyn_AssignOp_e= 4; struct Cyc_Absyn_AssignOp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Increment_e= 5; struct Cyc_Absyn_Increment_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_Conditional_e=
6; struct Cyc_Absyn_Conditional_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_SeqExp_e=
7; struct Cyc_Absyn_SeqExp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_FnCall_e= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; struct Cyc_Absyn_VarargCallInfo* f3; } ; static
const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e= 11;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
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
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
12u]; extern void* Cyc_Tcutil_compress( void* t); struct Cyc_CfFlowInfo_Place;
typedef void* Cyc_CfFlowInfo_local_root_t; typedef void* Cyc_CfFlowInfo_field_t;
typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t; typedef void* Cyc_CfFlowInfo_escaped_t;
typedef void* Cyc_CfFlowInfo_init_level_t; typedef void* Cyc_CfFlowInfo_init_state_t;
typedef void* Cyc_CfFlowInfo_path_info_t; typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t;
typedef void* Cyc_CfFlowInfo_flow_info_t; static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1; } ;
static const int Cyc_CfFlowInfo_TupleF= 1; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; static const int Cyc_CfFlowInfo_Esc= 0; static const int Cyc_CfFlowInfo_Unesc=
1; static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct
Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI= 2; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern void* Cyc_CfFlowInfo_mkLeafPI(
void* esc, void* il); extern int Cyc_CfFlowInfo_local_root_cmp( void*, void*);
extern int Cyc_CfFlowInfo_field_cmp( void*, void*); extern int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
void* t); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d,
struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo);
extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict* pinfo_dict, void*
pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo,
struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*,
void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_noneil={
0u,( void*)(( void*) 1u),( void*)(( void*) 0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_unes_thisil={ 0u,( void*)(( void*) 1u),( void*)(( void*)
1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_allil={
0u,( void*)(( void*) 1u),( void*)(( void*) 2u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_esc_noneil={ 0u,( void*)(( void*) 0u),( void*)(( void*)
0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_esc_thisil={
0u,( void*)(( void*) 0u),( void*)(( void*) 1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_esc_allil={ 0u,( void*)(( void*) 0u),( void*)(( void*)
2u)}; static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_none_v={
0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_noneil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_unesc_this_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_thisil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_all_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_allil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_esc_none_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_noneil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_this_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_thisil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_esc_all_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_allil)};
void* Cyc_CfFlowInfo_leaf_unesc_none=( void*)& Cyc_CfFlowInfo_leaf_unesc_none_v;
void* Cyc_CfFlowInfo_leaf_unesc_this=( void*)& Cyc_CfFlowInfo_leaf_unesc_this_v;
void* Cyc_CfFlowInfo_leaf_unesc_all=( void*)& Cyc_CfFlowInfo_leaf_unesc_all_v;
void* Cyc_CfFlowInfo_leaf_esc_none=( void*)& Cyc_CfFlowInfo_leaf_esc_none_v;
void* Cyc_CfFlowInfo_leaf_esc_this=( void*)& Cyc_CfFlowInfo_leaf_esc_this_v;
void* Cyc_CfFlowInfo_leaf_esc_all=( void*)& Cyc_CfFlowInfo_leaf_esc_all_v;
struct _tuple3{ void* f1; void* f2; } ; void* Cyc_CfFlowInfo_mkLeafPI( void* esc,
void* il){ struct _tuple3 _temp13=({ struct _tuple3 _temp12; _temp12.f1= esc;
_temp12.f2= il; _temp12;}); void* _temp27; void* _temp29; void* _temp31; void*
_temp33; void* _temp35; void* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; void* _temp47; void* _temp49; _LL15: _LL30: _temp29=
_temp13.f1; if( _temp29 ==( void*) Cyc_CfFlowInfo_Unesc){ goto _LL28;} else{
goto _LL17;} _LL28: _temp27= _temp13.f2; if( _temp27 ==( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL16;} else{ goto _LL17;} _LL17: _LL34: _temp33= _temp13.f1; if( _temp33
==( void*) Cyc_CfFlowInfo_Unesc){ goto _LL32;} else{ goto _LL19;} _LL32: _temp31=
_temp13.f2; if( _temp31 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL18;} else{
goto _LL19;} _LL19: _LL38: _temp37= _temp13.f1; if( _temp37 ==( void*) Cyc_CfFlowInfo_Unesc){
goto _LL36;} else{ goto _LL21;} _LL36: _temp35= _temp13.f2; if( _temp35 ==( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL20;} else{ goto _LL21;} _LL21: _LL42: _temp41=
_temp13.f1; if( _temp41 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL40;} else{ goto
_LL23;} _LL40: _temp39= _temp13.f2; if( _temp39 ==( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL22;} else{ goto _LL23;} _LL23: _LL46: _temp45= _temp13.f1; if( _temp45
==( void*) Cyc_CfFlowInfo_Esc){ goto _LL44;} else{ goto _LL25;} _LL44: _temp43=
_temp13.f2; if( _temp43 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL24;} else{
goto _LL25;} _LL25: _LL50: _temp49= _temp13.f1; if( _temp49 ==( void*) Cyc_CfFlowInfo_Esc){
goto _LL48;} else{ goto _LL14;} _LL48: _temp47= _temp13.f2; if( _temp47 ==( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL26;} else{ goto _LL14;} _LL16: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL18: return Cyc_CfFlowInfo_leaf_unesc_this; _LL20: return Cyc_CfFlowInfo_leaf_unesc_all;
_LL22: return Cyc_CfFlowInfo_leaf_esc_none; _LL24: return Cyc_CfFlowInfo_leaf_esc_this;
_LL26: return Cyc_CfFlowInfo_leaf_esc_all; _LL14:;} int Cyc_CfFlowInfo_local_root_cmp(
void* r1, void* r2){ if( r1 == r2){ return 0;}{ struct _tuple3 _temp52=({ struct
_tuple3 _temp51; _temp51.f1= r1; _temp51.f2= r2; _temp51;}); void* _temp62;
struct Cyc_Absyn_Exp* _temp64; void* _temp66; struct Cyc_Absyn_Vardecl* _temp68;
void* _temp70; struct Cyc_Absyn_Vardecl* _temp72; void* _temp74; struct Cyc_Absyn_Exp*
_temp76; void* _temp78; struct Cyc_Absyn_Vardecl* _temp80; void* _temp82; struct
Cyc_Absyn_Vardecl* _temp84; void* _temp86; struct Cyc_Absyn_Exp* _temp88; void*
_temp90; struct Cyc_Absyn_Exp* _temp92; _LL54: _LL67: _temp66= _temp52.f1; if(*((
int*) _temp66) == Cyc_CfFlowInfo_VarRoot){ _LL69: _temp68=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp66)->f1; goto _LL63;} else{ goto _LL56;} _LL63: _temp62= _temp52.f2; if(*((
int*) _temp62) == Cyc_CfFlowInfo_MallocPt){ _LL65: _temp64=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp62)->f1; goto _LL55;} else{ goto _LL56;} _LL56: _LL75: _temp74= _temp52.f1;
if(*(( int*) _temp74) == Cyc_CfFlowInfo_MallocPt){ _LL77: _temp76=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp74)->f1; goto _LL71;} else{ goto _LL58;} _LL71: _temp70= _temp52.f2; if(*((
int*) _temp70) == Cyc_CfFlowInfo_VarRoot){ _LL73: _temp72=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp70)->f1; goto _LL57;} else{ goto _LL58;} _LL58: _LL83: _temp82= _temp52.f1;
if(*(( int*) _temp82) == Cyc_CfFlowInfo_VarRoot){ _LL85: _temp84=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp82)->f1; goto _LL79;} else{ goto _LL60;} _LL79: _temp78= _temp52.f2; if(*((
int*) _temp78) == Cyc_CfFlowInfo_VarRoot){ _LL81: _temp80=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp78)->f1; goto _LL59;} else{ goto _LL60;} _LL60: _LL91: _temp90= _temp52.f1;
if(*(( int*) _temp90) == Cyc_CfFlowInfo_MallocPt){ _LL93: _temp92=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp90)->f1; goto _LL87;} else{ goto _LL53;} _LL87: _temp86= _temp52.f2; if(*((
int*) _temp86) == Cyc_CfFlowInfo_MallocPt){ _LL89: _temp88=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp86)->f1; goto _LL61;} else{ goto _LL53;} _LL55: return 1; _LL57: return - 1;
_LL59: return(( int(*)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp)( _temp84, _temp80); _LL61: return(( int(*)( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp)( _temp92, _temp88); _LL53:;}} int Cyc_CfFlowInfo_field_cmp(
void* f1, void* f2){ if( f1 == f2){ return 0;}{ struct _tuple3 _temp95=({ struct
_tuple3 _temp94; _temp94.f1= f1; _temp94.f2= f2; _temp94;}); void* _temp105; int
_temp107; void* _temp109; struct _tagged_string* _temp111; void* _temp113;
struct _tagged_string* _temp115; void* _temp117; int _temp119; void* _temp121;
struct _tagged_string* _temp123; void* _temp125; struct _tagged_string* _temp127;
void* _temp129; int _temp131; void* _temp133; int _temp135; _LL97: _LL110:
_temp109= _temp95.f1; if(*(( int*) _temp109) == Cyc_CfFlowInfo_StructF){ _LL112:
_temp111=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp109)->f1; goto _LL106;}
else{ goto _LL99;} _LL106: _temp105= _temp95.f2; if(*(( int*) _temp105) == Cyc_CfFlowInfo_TupleF){
_LL108: _temp107=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp105)->f1; goto
_LL98;} else{ goto _LL99;} _LL99: _LL118: _temp117= _temp95.f1; if(*(( int*)
_temp117) == Cyc_CfFlowInfo_TupleF){ _LL120: _temp119=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp117)->f1; goto _LL114;} else{ goto _LL101;} _LL114: _temp113= _temp95.f2;
if(*(( int*) _temp113) == Cyc_CfFlowInfo_StructF){ _LL116: _temp115=(( struct
Cyc_CfFlowInfo_StructF_struct*) _temp113)->f1; goto _LL100;} else{ goto _LL101;}
_LL101: _LL126: _temp125= _temp95.f1; if(*(( int*) _temp125) == Cyc_CfFlowInfo_StructF){
_LL128: _temp127=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp125)->f1; goto
_LL122;} else{ goto _LL103;} _LL122: _temp121= _temp95.f2; if(*(( int*) _temp121)
== Cyc_CfFlowInfo_StructF){ _LL124: _temp123=(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp121)->f1; goto _LL102;} else{ goto _LL103;} _LL103: _LL134: _temp133=
_temp95.f1; if(*(( int*) _temp133) == Cyc_CfFlowInfo_TupleF){ _LL136: _temp135=((
struct Cyc_CfFlowInfo_TupleF_struct*) _temp133)->f1; goto _LL130;} else{ goto
_LL96;} _LL130: _temp129= _temp95.f2; if(*(( int*) _temp129) == Cyc_CfFlowInfo_TupleF){
_LL132: _temp131=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp129)->f1; goto
_LL104;} else{ goto _LL96;} _LL98: return 1; _LL100: return - 1; _LL102: return((
int(*)( struct _tagged_string*, struct _tagged_string*)) Cyc_Core_ptrcmp)(
_temp127, _temp123); _LL104: return _temp135 - _temp131; _LL96:;}} int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place* p1, struct Cyc_CfFlowInfo_Place* p2){ if( p1 == p2){
return 0;}{ int i= Cyc_CfFlowInfo_local_root_cmp(( void*) p1->root,( void*) p2->root);
if( i != 0){ return i;} return(( int(*)( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_CfFlowInfo_field_cmp, p1->fields,
p2->fields);}} struct _tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; } ; void*
Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t){ void* _temp137= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_TunionFieldInfo _temp149; struct Cyc_List_List* _temp151;
void* _temp153; struct Cyc_Absyn_Tunionfield* _temp155; struct Cyc_Absyn_Tuniondecl*
_temp157; struct Cyc_List_List* _temp159; struct Cyc_List_List* _temp161; struct
Cyc_Absyn_Structdecl** _temp163; struct Cyc_Absyn_Structdecl* _temp165; struct
Cyc_Absyn_Structdecl _temp166; struct Cyc_List_List* _temp167; struct Cyc_Core_Opt*
_temp169; struct Cyc_Core_Opt _temp171; struct Cyc_List_List* _temp172; struct
Cyc_List_List* _temp174; struct Cyc_Core_Opt* _temp176; void* _temp178; struct
Cyc_List_List* _temp180; struct _tuple0* _temp182; _LL139: if(( unsigned int)
_temp137 > 4u?*(( int*) _temp137) == Cyc_Absyn_TunionFieldType: 0){ _LL150:
_temp149=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp137)->f1; _LL154:
_temp153=( void*) _temp149.field_info; if(*(( int*) _temp153) == Cyc_Absyn_KnownTunionfield){
_LL158: _temp157=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp153)->f1;
goto _LL156; _LL156: _temp155=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp153)->f2; goto _LL152;} else{ goto _LL141;} _LL152: _temp151= _temp149.targs;
goto _LL140;} else{ goto _LL141;} _LL141: if(( unsigned int) _temp137 > 4u?*((
int*) _temp137) == Cyc_Absyn_TupleType: 0){ _LL160: _temp159=(( struct Cyc_Absyn_TupleType_struct*)
_temp137)->f1; goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int)
_temp137 > 4u?*(( int*) _temp137) == Cyc_Absyn_AnonStructType: 0){ _LL162:
_temp161=(( struct Cyc_Absyn_AnonStructType_struct*) _temp137)->f1; goto _LL144;}
else{ goto _LL145;} _LL145: if(( unsigned int) _temp137 > 4u?*(( int*) _temp137)
== Cyc_Absyn_StructType: 0){ _LL183: _temp182=(( struct Cyc_Absyn_StructType_struct*)
_temp137)->f1; goto _LL181; _LL181: _temp180=(( struct Cyc_Absyn_StructType_struct*)
_temp137)->f2; goto _LL164; _LL164: _temp163=(( struct Cyc_Absyn_StructType_struct*)
_temp137)->f3; if( _temp163 == 0){ goto _LL147;} else{ _temp165=* _temp163;
_temp166=* _temp165; _LL179: _temp178=( void*) _temp166.sc; goto _LL177; _LL177:
_temp176= _temp166.name; goto _LL175; _LL175: _temp174= _temp166.tvs; goto
_LL170; _LL170: _temp169= _temp166.fields; if( _temp169 == 0){ goto _LL147;}
else{ _temp171=* _temp169; _LL173: _temp172=( struct Cyc_List_List*) _temp171.v;
goto _LL168;} _LL168: _temp167= _temp166.attributes; goto _LL146;}} else{ goto
_LL147;} _LL147: goto _LL148; _LL140: if( _temp155->typs == 0){ return Cyc_CfFlowInfo_leaf_unesc_none;}
_temp159= _temp155->typs; goto _LL142; _LL142: { struct Cyc_Dict_Dict* _temp184=((
struct Cyc_Dict_Dict*(*)( int(* comp)( int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);
goto _LL185; _LL185:{ int i= 0; for( 0; _temp159 != 0;( _temp159=(( struct Cyc_List_List*)
_check_null( _temp159))->tl, ++ i)){ struct _tuple4 _temp188; void* _temp189;
struct Cyc_Absyn_Tqual _temp191; struct _tuple4* _temp186=( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp159))->hd; _temp188=* _temp186; _LL192:
_temp191= _temp188.f1; goto _LL190; _LL190: _temp189= _temp188.f2; goto _LL187;
_LL187: _temp184=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key,
void* data)) Cyc_Dict_insert)( _temp184, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp189));}} return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp193=(
struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp193[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp194; _temp194.tag= Cyc_CfFlowInfo_TuplePI;
_temp194.f1= _temp184; _temp194;}); _temp193;});} _LL144: _temp172= _temp161;
goto _LL146; _LL146: { struct Cyc_Dict_Dict* _temp195=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); goto _LL196; _LL196: for( 0; _temp172 != 0; _temp172=((
struct Cyc_List_List*) _check_null( _temp172))->tl){ struct Cyc_Absyn_Structfield
_temp199; struct Cyc_List_List* _temp200; struct Cyc_Core_Opt* _temp202; void*
_temp204; struct Cyc_Absyn_Tqual _temp206; struct _tagged_string* _temp208;
struct Cyc_Absyn_Structfield* _temp197=( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp172))->hd; _temp199=* _temp197; _LL209:
_temp208= _temp199.name; goto _LL207; _LL207: _temp206= _temp199.tq; goto _LL205;
_LL205: _temp204=( void*) _temp199.type; goto _LL203; _LL203: _temp202= _temp199.width;
goto _LL201; _LL201: _temp200= _temp199.attributes; goto _LL198; _LL198: if( Cyc_String_strcmp(*
_temp208,({ char* _temp210=""; struct _tagged_string _temp211; _temp211.curr=
_temp210; _temp211.base= _temp210; _temp211.last_plus_one= _temp210 + 1;
_temp211;})) != 0){ _temp195=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( _temp195, _temp208,
Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( _temp204));}} return( void*)({ struct
Cyc_CfFlowInfo_StructPI_struct* _temp212=( struct Cyc_CfFlowInfo_StructPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct)); _temp212[ 0]=({
struct Cyc_CfFlowInfo_StructPI_struct _temp213; _temp213.tag= Cyc_CfFlowInfo_StructPI;
_temp213.f1= _temp195; _temp213;}); _temp212;});} _LL148: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL138:;} void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place){ struct Cyc_CfFlowInfo_Place _temp216; struct Cyc_List_List* _temp217;
void* _temp219; struct Cyc_CfFlowInfo_Place* _temp214= place; _temp216=*
_temp214; _LL220: _temp219=( void*) _temp216.root; goto _LL218; _LL218: _temp217=
_temp216.fields; goto _LL215; _LL215: { void* _temp221=(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( d, _temp219); goto _LL222; _LL222: for( 0;
_temp217 != 0; _temp217=(( struct Cyc_List_List*) _check_null( _temp217))->tl){
struct _tuple3 _temp224=({ struct _tuple3 _temp223; _temp223.f1= _temp221;
_temp223.f2=( void*)(( struct Cyc_List_List*) _check_null( _temp217))->hd;
_temp223;}); void* _temp232; int _temp234; void* _temp236; struct Cyc_Dict_Dict*
_temp238; void* _temp240; struct _tagged_string* _temp242; void* _temp244;
struct Cyc_Dict_Dict* _temp246; _LL226: _LL237: _temp236= _temp224.f1; if(*((
int*) _temp236) == Cyc_CfFlowInfo_TuplePI){ _LL239: _temp238=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp236)->f1; goto _LL233;} else{ goto _LL228;} _LL233: _temp232= _temp224.f2;
if(*(( int*) _temp232) == Cyc_CfFlowInfo_TupleF){ _LL235: _temp234=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp232)->f1; goto _LL227;} else{ goto _LL228;} _LL228: _LL245: _temp244=
_temp224.f1; if(*(( int*) _temp244) == Cyc_CfFlowInfo_StructPI){ _LL247:
_temp246=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp244)->f1; goto _LL241;}
else{ goto _LL230;} _LL241: _temp240= _temp224.f2; if(*(( int*) _temp240) == Cyc_CfFlowInfo_StructF){
_LL243: _temp242=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp240)->f1; goto
_LL229;} else{ goto _LL230;} _LL230: goto _LL231; _LL227: _temp221=(( void*(*)(
struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp238, _temp234); goto
_LL225; _LL229: _temp221=(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp246, _temp242); goto _LL225; _LL231:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp248=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp248[ 0]=({ struct
Cyc_Core_Impossible_struct _temp249; _temp249.tag= Cyc_Core_Impossible; _temp249.f1=({
char* _temp250="bad lookup_place"; struct _tagged_string _temp251; _temp251.curr=
_temp250; _temp251.base= _temp250; _temp251.last_plus_one= _temp250 + 17;
_temp251;}); _temp249;}); _temp248;})); _LL225:;} return _temp221;}} struct
_tuple5{ void* f1; struct Cyc_List_List* f2; } ; void* Cyc_CfFlowInfo_insert_place_rec(
void* insert_path, struct Cyc_List_List* fields, void* pinfo){ if( fields == 0){
return pinfo;}{ struct _tuple5 _temp253=({ struct _tuple5 _temp252; _temp252.f1=
insert_path; _temp252.f2= fields; _temp252;}); struct Cyc_List_List* _temp261;
struct Cyc_List_List _temp263; struct Cyc_List_List* _temp264; void* _temp266;
int _temp268; void* _temp270; struct Cyc_Dict_Dict* _temp272; struct Cyc_List_List*
_temp274; struct Cyc_List_List _temp276; struct Cyc_List_List* _temp277; void*
_temp279; struct _tagged_string* _temp281; void* _temp283; struct Cyc_Dict_Dict*
_temp285; _LL255: _LL271: _temp270= _temp253.f1; if(*(( int*) _temp270) == Cyc_CfFlowInfo_TuplePI){
_LL273: _temp272=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp270)->f1; goto
_LL262;} else{ goto _LL257;} _LL262: _temp261= _temp253.f2; if( _temp261 == 0){
goto _LL257;} else{ _temp263=* _temp261; _LL267: _temp266=( void*) _temp263.hd;
if(*(( int*) _temp266) == Cyc_CfFlowInfo_TupleF){ _LL269: _temp268=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp266)->f1; goto _LL265;} else{ goto _LL257;} _LL265: _temp264= _temp263.tl;
goto _LL256;} _LL257: _LL284: _temp283= _temp253.f1; if(*(( int*) _temp283) ==
Cyc_CfFlowInfo_StructPI){ _LL286: _temp285=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp283)->f1; goto _LL275;} else{ goto _LL259;} _LL275: _temp274= _temp253.f2;
if( _temp274 == 0){ goto _LL259;} else{ _temp276=* _temp274; _LL280: _temp279=(
void*) _temp276.hd; if(*(( int*) _temp279) == Cyc_CfFlowInfo_StructF){ _LL282:
_temp281=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp279)->f1; goto _LL278;}
else{ goto _LL259;} _LL278: _temp277= _temp276.tl; goto _LL258;} _LL259: goto
_LL260; _LL256: { void* _temp287=(( void*(*)( struct Cyc_Dict_Dict* d, int key))
Cyc_Dict_lookup)( _temp272, _temp268); goto _LL288; _LL288: return( void*)({
struct Cyc_CfFlowInfo_TuplePI_struct* _temp289=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp289[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp290; _temp290.tag= Cyc_CfFlowInfo_TuplePI;
_temp290.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void*
data)) Cyc_Dict_insert)( _temp272, _temp268, Cyc_CfFlowInfo_insert_place_rec(
_temp287, _temp264, pinfo)); _temp290;}); _temp289;});} _LL258: { void* _temp291=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
_temp285, _temp281); goto _LL292; _LL292: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp293=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp293[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp294; _temp294.tag=
Cyc_CfFlowInfo_StructPI; _temp294.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( _temp285, _temp281,
Cyc_CfFlowInfo_insert_place_rec( _temp291, _temp277, pinfo)); _temp294;});
_temp293;});} _LL260:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp295=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp295[ 0]=({ struct Cyc_Core_Impossible_struct _temp296; _temp296.tag= Cyc_Core_Impossible;
_temp296.f1=({ char* _temp297="bad insert_place"; struct _tagged_string _temp298;
_temp298.curr= _temp297; _temp298.base= _temp297; _temp298.last_plus_one=
_temp297 + 17; _temp298;}); _temp296;}); _temp295;})); _LL254:;}} struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place, void* pinfo){ struct Cyc_CfFlowInfo_Place _temp301; struct Cyc_List_List*
_temp302; void* _temp304; struct Cyc_CfFlowInfo_Place* _temp299= place; _temp301=*
_temp299; _LL305: _temp304=( void*) _temp301.root; goto _LL303; _LL303: _temp302=
_temp301.fields; goto _LL300; _LL300: return(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( d, _temp304, Cyc_CfFlowInfo_insert_place_rec(((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d, _temp304),
_temp302, pinfo));} struct _tuple6{ struct Cyc_Dict_Dict* f1; struct Cyc_List_List*
f2; } ; int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path);
int Cyc_CfFlowInfo_isAllInit_is( struct _tuple6* env, void* state){ void*
_temp306= state; void* _temp314; void* _temp316; void* _temp318; void* _temp320;
struct Cyc_CfFlowInfo_Place* _temp322; _LL308: if(*(( int*) _temp306) == Cyc_CfFlowInfo_UnknownIS){
_LL317: _temp316=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp306)->f1;
goto _LL315; _LL315: _temp314=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp306)->f2; if( _temp314 ==( void*) Cyc_CfFlowInfo_AllIL){ goto _LL309;}
else{ goto _LL310;}} else{ goto _LL310;} _LL310: if(*(( int*) _temp306) == Cyc_CfFlowInfo_UnknownIS){
_LL321: _temp320=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp306)->f1;
goto _LL319; _LL319: _temp318=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp306)->f2; goto _LL311;} else{ goto _LL312;} _LL312: if(*(( int*) _temp306)
== Cyc_CfFlowInfo_MustPointTo){ _LL323: _temp322=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp306)->f1; goto _LL313;} else{ goto _LL307;} _LL309: return 1; _LL311:
return 0; _LL313: { struct _tuple6 _temp326; struct Cyc_List_List* _temp327;
struct Cyc_List_List** _temp329; struct Cyc_Dict_Dict* _temp330; struct _tuple6*
_temp324= env; _temp326=* _temp324; _LL331: _temp330= _temp326.f1; goto _LL328;
_LL328: _temp327= _temp326.f2; _temp329=&(* _temp324).f2; goto _LL325; _LL325:
if((( int(*)( int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
_temp329, _temp322)){ return 1;}* _temp329=({ struct Cyc_List_List* _temp332=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp332[
0]=({ struct Cyc_List_List _temp333; _temp333.hd=( void*) _temp322; _temp333.tl=*
_temp329; _temp333;}); _temp332;}); return Cyc_CfFlowInfo_isAllInit_pinfo( env,
Cyc_CfFlowInfo_lookup_place( _temp330, _temp322));} _LL307:;} int Cyc_CfFlowInfo_isAllInit_f(
struct _tuple6* env, void* a, void* path){ return Cyc_CfFlowInfo_isAllInit_pinfo(
env, path);} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path){
void* _temp334= path; void* _temp342; struct Cyc_Dict_Dict* _temp344; struct Cyc_Dict_Dict*
_temp346; _LL336: if(*(( int*) _temp334) == Cyc_CfFlowInfo_LeafPI){ _LL343:
_temp342=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp334)->f1; goto
_LL337;} else{ goto _LL338;} _LL338: if(*(( int*) _temp334) == Cyc_CfFlowInfo_TuplePI){
_LL345: _temp344=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp334)->f1; goto
_LL339;} else{ goto _LL340;} _LL340: if(*(( int*) _temp334) == Cyc_CfFlowInfo_StructPI){
_LL347: _temp346=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp334)->f1; goto
_LL341;} else{ goto _LL335;} _LL337: return Cyc_CfFlowInfo_isAllInit_is( env,
_temp342); _LL339: return(( int(*)( int(* f)( struct _tuple6*, int, void*),
struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)(
struct _tuple6* env, int a, void* path)) Cyc_CfFlowInfo_isAllInit_f, env,
_temp344); _LL341: return(( int(*)( int(* f)( struct _tuple6*, struct
_tagged_string*, void*), struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)((
int(*)( struct _tuple6* env, struct _tagged_string* a, void* path)) Cyc_CfFlowInfo_isAllInit_f,
env, _temp346); _LL335:;} int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo){ struct _tuple6 _temp349=({ struct _tuple6 _temp348;
_temp348.f1= pinfo_dict; _temp348.f2= 0; _temp348;}); goto _LL350; _LL350:
return Cyc_CfFlowInfo_isAllInit_pinfo(& _temp349, pinfo);} struct _tuple7{
struct Cyc_Dict_Dict* f1; struct Cyc_Set_Set** f2; } ; void* Cyc_CfFlowInfo_pinfo_getplaces_esc(
struct _tuple7* env, void* pinfo){ struct _tuple7 _temp353; struct Cyc_Set_Set**
_temp354; struct Cyc_Dict_Dict* _temp356; struct _tuple7* _temp351= env;
_temp353=* _temp351; _LL357: _temp356= _temp353.f1; goto _LL355; _LL355:
_temp354= _temp353.f2; goto _LL352; _LL352: { void* _temp358= pinfo; void*
_temp368; struct Cyc_CfFlowInfo_Place* _temp370; void* _temp372; void* _temp374;
void* _temp376; struct Cyc_Dict_Dict* _temp378; struct Cyc_Dict_Dict* _temp380;
_LL360: if(*(( int*) _temp358) == Cyc_CfFlowInfo_LeafPI){ _LL369: _temp368=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp358)->f1; if(*(( int*)
_temp368) == Cyc_CfFlowInfo_MustPointTo){ _LL371: _temp370=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp368)->f1; goto _LL361;} else{ goto _LL362;}} else{ goto _LL362;} _LL362:
if(*(( int*) _temp358) == Cyc_CfFlowInfo_LeafPI){ _LL373: _temp372=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp358)->f1; if(*(( int*) _temp372) ==
Cyc_CfFlowInfo_UnknownIS){ _LL377: _temp376=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp372)->f1; goto _LL375; _LL375: _temp374=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp372)->f2; goto _LL363;} else{ goto _LL364;}} else{ goto _LL364;} _LL364:
if(*(( int*) _temp358) == Cyc_CfFlowInfo_TuplePI){ _LL379: _temp378=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp358)->f1; goto _LL365;} else{ goto _LL366;}
_LL366: if(*(( int*) _temp358) == Cyc_CfFlowInfo_StructPI){ _LL381: _temp380=((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp358)->f1; goto _LL367;} else{ goto
_LL359;} _LL361:* _temp354=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* _temp354, _temp370); if(
Cyc_CfFlowInfo_isAllInit( _temp356, Cyc_CfFlowInfo_lookup_place( _temp356,
_temp370))){ return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,( void*)
Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_ThisIL); _LL363: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Esc, _temp374); _LL365: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp382=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp382[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp383; _temp383.tag= Cyc_CfFlowInfo_TuplePI;
_temp383.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct _tuple7*, void*),
struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc,
env, _temp378); _temp383;}); _temp382;}); _LL367: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp384=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp384[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp385; _temp385.tag=
Cyc_CfFlowInfo_StructPI; _temp385.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct _tuple7*, void*), struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)(
Cyc_CfFlowInfo_pinfo_getplaces_esc, env, _temp380); _temp385;}); _temp384;});
_LL359:;}} void Cyc_CfFlowInfo_pinfo_getplaces( struct Cyc_Set_Set** places_ptr,
void* a, void* pinfo){ void* _temp386= pinfo; void* _temp396; void* _temp398;
void* _temp400; void* _temp402; struct Cyc_CfFlowInfo_Place* _temp404; struct
Cyc_Dict_Dict* _temp406; struct Cyc_Dict_Dict* _temp408; _LL388: if(*(( int*)
_temp386) == Cyc_CfFlowInfo_LeafPI){ _LL397: _temp396=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp386)->f1; if(*(( int*) _temp396) == Cyc_CfFlowInfo_UnknownIS){ _LL401:
_temp400=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp396)->f1; goto
_LL399; _LL399: _temp398=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp396)->f2; goto _LL389;} else{ goto _LL390;}} else{ goto _LL390;} _LL390:
if(*(( int*) _temp386) == Cyc_CfFlowInfo_LeafPI){ _LL403: _temp402=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp386)->f1; if(*(( int*) _temp402) ==
Cyc_CfFlowInfo_MustPointTo){ _LL405: _temp404=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp402)->f1; goto _LL391;} else{ goto _LL392;}} else{ goto _LL392;} _LL392:
if(*(( int*) _temp386) == Cyc_CfFlowInfo_TuplePI){ _LL407: _temp406=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp386)->f1; goto _LL393;} else{ goto _LL394;}
_LL394: if(*(( int*) _temp386) == Cyc_CfFlowInfo_StructPI){ _LL409: _temp408=((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp386)->f1; goto _LL395;} else{ goto
_LL387;} _LL389: return; _LL391:* places_ptr=(( struct Cyc_Set_Set*(*)( struct
Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* places_ptr,
_temp404); return; _LL393:(( void(*)( void(* f)( struct Cyc_Set_Set**, int, void*),
struct Cyc_Set_Set** env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct Cyc_Set_Set** places_ptr, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces,
places_ptr, _temp406); return; _LL395:(( void(*)( void(* f)( struct Cyc_Set_Set**,
struct _tagged_string*, void*), struct Cyc_Set_Set** env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct Cyc_Set_Set** places_ptr, struct
_tagged_string* a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces, places_ptr,
_temp408); return; _LL387:;} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_these(
struct Cyc_Set_Set* places, struct Cyc_Dict_Dict* d){ while( !(( int(*)( struct
Cyc_Set_Set* s)) Cyc_Set_is_empty)( places)) { struct Cyc_CfFlowInfo_Place*
_temp410=(( struct Cyc_CfFlowInfo_Place*(*)( struct Cyc_Set_Set* s)) Cyc_Set_choose)(
places); goto _LL411; _LL411: places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_delete)( places, _temp410);{ void*
place_old_pinfo;{ struct _handler_cons _temp412; _push_handler(& _temp412);{ int
_temp414= 0; if( setjmp( _temp412.handler)){ _temp414= 1;} if( ! _temp414){
place_old_pinfo= Cyc_CfFlowInfo_lookup_place( d, _temp410);; _pop_handler();}
else{ void* _temp413=( void*) _exn_thrown; void* _temp416= _temp413; _LL418: if(
_temp416 == Cyc_Dict_Absent){ goto _LL419;} else{ goto _LL420;} _LL420: goto
_LL421; _LL419: continue; _LL421:( void) _throw( _temp416); _LL417:;}}}{ struct
_tuple7 _temp423=({ struct _tuple7 _temp422; _temp422.f1= d; _temp422.f2=&
places; _temp422;}); goto _LL424; _LL424: { void* _temp425= Cyc_CfFlowInfo_pinfo_getplaces_esc(&
_temp423, place_old_pinfo); goto _LL426; _LL426: if( place_old_pinfo != _temp425){
d= Cyc_CfFlowInfo_insert_place( d, _temp410, _temp425);}}}}} return d;} static
struct Cyc_Set_Set** Cyc_CfFlowInfo_empty_place_set_ptr= 0; struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_escape_pointsto( void* pinfo, struct Cyc_Dict_Dict* d){ if( Cyc_CfFlowInfo_empty_place_set_ptr
== 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({ struct Cyc_Set_Set** _temp427=(
struct Cyc_Set_Set**) GC_malloc( sizeof( struct Cyc_Set_Set*) * 1); _temp427[ 0]=((
struct Cyc_Set_Set*(*)( int(* comp)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*)))
Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp); _temp427;});}{ struct Cyc_Set_Set*
_temp428=*(( struct Cyc_Set_Set**) _check_null( Cyc_CfFlowInfo_empty_place_set_ptr));
goto _LL429; _LL429:(( void(*)( struct Cyc_Set_Set** places_ptr, int a, void*
pinfo)) Cyc_CfFlowInfo_pinfo_getplaces)(& _temp428, 0, pinfo); return Cyc_CfFlowInfo_escape_these(
_temp428, d);}} void* Cyc_CfFlowInfo_assign_unknown_dict( void* level, void*
pinfo){ void* _temp430= pinfo; void* _temp440; void* _temp442; void* _temp444;
void* _temp446; struct Cyc_Dict_Dict* _temp448; struct Cyc_Dict_Dict* _temp450;
_LL432: if(*(( int*) _temp430) == Cyc_CfFlowInfo_LeafPI){ _LL441: _temp440=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp430)->f1; if(*(( int*)
_temp440) == Cyc_CfFlowInfo_UnknownIS){ _LL445: _temp444=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp440)->f1; if( _temp444 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL443;} else{
goto _LL434;} _LL443: _temp442=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp440)->f2; goto _LL433;} else{ goto _LL434;}} else{ goto _LL434;} _LL434:
if(*(( int*) _temp430) == Cyc_CfFlowInfo_LeafPI){ _LL447: _temp446=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp430)->f1; goto _LL435;} else{ goto
_LL436;} _LL436: if(*(( int*) _temp430) == Cyc_CfFlowInfo_TuplePI){ _LL449:
_temp448=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp430)->f1; goto _LL437;}
else{ goto _LL438;} _LL438: if(*(( int*) _temp430) == Cyc_CfFlowInfo_StructPI){
_LL451: _temp450=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp430)->f1; goto
_LL439;} else{ goto _LL431;} _LL433: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,
level); _LL435: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,
level); _LL437: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp452=(
struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp452[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp453; _temp453.tag= Cyc_CfFlowInfo_TuplePI;
_temp453.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp448); _temp453;}); _temp452;}); _LL439: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp454=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp454[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp455; _temp455.tag=
Cyc_CfFlowInfo_StructPI; _temp455.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
void*, void*), void* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp450); _temp455;}); _temp454;}); _LL431:;} struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict* d2; struct Cyc_Set_Set** places;
} ; typedef struct Cyc_CfFlowInfo_JoinEnv* Cyc_CfFlowInfo_join_env_t; void* Cyc_CfFlowInfo_join_escape(
void* esc1, void* esc2){ struct _tuple3 _temp457=({ struct _tuple3 _temp456;
_temp456.f1= esc1; _temp456.f2= esc2; _temp456;}); void* _temp465; void*
_temp467; void* _temp469; void* _temp471; _LL459: _LL468: _temp467= _temp457.f1;
if( _temp467 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL466;} else{ goto _LL461;}
_LL466: _temp465= _temp457.f2; goto _LL460; _LL461: _LL472: _temp471= _temp457.f1;
goto _LL470; _LL470: _temp469= _temp457.f2; if( _temp469 ==( void*) Cyc_CfFlowInfo_Esc){
goto _LL462;} else{ goto _LL463;} _LL463: goto _LL464; _LL460: goto _LL462;
_LL462: return( void*) Cyc_CfFlowInfo_Esc; _LL464: return( void*) Cyc_CfFlowInfo_Unesc;
_LL458:;} void* Cyc_CfFlowInfo_join_initlevel( void* il1, void* il2){ struct
_tuple3 _temp474=({ struct _tuple3 _temp473; _temp473.f1= il1; _temp473.f2= il2;
_temp473;}); void* _temp486; void* _temp488; void* _temp490; void* _temp492;
void* _temp494; void* _temp496; void* _temp498; void* _temp500; _LL476: _LL489:
_temp488= _temp474.f1; if( _temp488 ==( void*) Cyc_CfFlowInfo_NoneIL){ goto
_LL487;} else{ goto _LL478;} _LL487: _temp486= _temp474.f2; goto _LL477; _LL478:
_LL493: _temp492= _temp474.f1; goto _LL491; _LL491: _temp490= _temp474.f2; if(
_temp490 ==( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL479;} else{ goto _LL480;}
_LL480: _LL497: _temp496= _temp474.f1; if( _temp496 ==( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL495;} else{ goto _LL482;} _LL495: _temp494= _temp474.f2; goto _LL481;
_LL482: _LL501: _temp500= _temp474.f1; goto _LL499; _LL499: _temp498= _temp474.f2;
if( _temp498 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL483;} else{ goto _LL484;}
_LL484: goto _LL485; _LL477: goto _LL479; _LL479: return( void*) Cyc_CfFlowInfo_NoneIL;
_LL481: goto _LL483; _LL483: return( void*) Cyc_CfFlowInfo_ThisIL; _LL485:
return( void*) Cyc_CfFlowInfo_AllIL; _LL475:;} void* Cyc_CfFlowInfo_join_initstate(
struct Cyc_CfFlowInfo_JoinEnv* env, void* state1, void* state2){ struct _tuple3
_temp503=({ struct _tuple3 _temp502; _temp502.f1= state1; _temp502.f2= state2;
_temp502;}); void* _temp513; void* _temp515; void* _temp517; void* _temp519;
void* _temp521; void* _temp523; void* _temp525; struct Cyc_CfFlowInfo_Place*
_temp527; void* _temp529; void* _temp531; void* _temp533; void* _temp535; void*
_temp537; void* _temp539; void* _temp541; struct Cyc_CfFlowInfo_Place* _temp543;
void* _temp545; struct Cyc_CfFlowInfo_Place* _temp547; void* _temp549; struct
Cyc_CfFlowInfo_Place* _temp551; _LL505: _LL520: _temp519= _temp503.f1; if(*((
int*) _temp519) == Cyc_CfFlowInfo_UnknownIS){ _LL524: _temp523=( void*)(( struct
Cyc_CfFlowInfo_UnknownIS_struct*) _temp519)->f1; goto _LL522; _LL522: _temp521=(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp519)->f2; goto _LL514;}
else{ goto _LL507;} _LL514: _temp513= _temp503.f2; if(*(( int*) _temp513) == Cyc_CfFlowInfo_UnknownIS){
_LL518: _temp517=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp513)->f1;
goto _LL516; _LL516: _temp515=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp513)->f2; goto _LL506;} else{ goto _LL507;} _LL507: _LL530: _temp529=
_temp503.f1; if(*(( int*) _temp529) == Cyc_CfFlowInfo_UnknownIS){ _LL534:
_temp533=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp529)->f1; goto
_LL532; _LL532: _temp531=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp529)->f2; goto _LL526;} else{ goto _LL509;} _LL526: _temp525= _temp503.f2;
if(*(( int*) _temp525) == Cyc_CfFlowInfo_MustPointTo){ _LL528: _temp527=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp525)->f1; goto _LL508;} else{
goto _LL509;} _LL509: _LL542: _temp541= _temp503.f1; if(*(( int*) _temp541) ==
Cyc_CfFlowInfo_MustPointTo){ _LL544: _temp543=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp541)->f1; goto _LL536;} else{ goto _LL511;} _LL536: _temp535= _temp503.f2;
if(*(( int*) _temp535) == Cyc_CfFlowInfo_UnknownIS){ _LL540: _temp539=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp535)->f1; goto _LL538; _LL538:
_temp537=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp535)->f2; goto
_LL510;} else{ goto _LL511;} _LL511: _LL550: _temp549= _temp503.f1; if(*(( int*)
_temp549) == Cyc_CfFlowInfo_MustPointTo){ _LL552: _temp551=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp549)->f1; goto _LL546;} else{ goto _LL504;} _LL546: _temp545= _temp503.f2;
if(*(( int*) _temp545) == Cyc_CfFlowInfo_MustPointTo){ _LL548: _temp547=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp545)->f1; goto _LL512;} else{
goto _LL504;} _LL506: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_join_escape(
_temp523, _temp517), Cyc_CfFlowInfo_join_initlevel( _temp521, _temp515)); _LL508: {
struct _tuple6 _temp554=({ struct _tuple6 _temp553; _temp553.f1= env->d2;
_temp553.f2= 0; _temp553;}); goto _LL555; _LL555: if( _temp531 ==( void*) Cyc_CfFlowInfo_AllIL?
Cyc_CfFlowInfo_isAllInit_pinfo(& _temp554, Cyc_CfFlowInfo_lookup_place( env->d2,
_temp527)): 0){* env->places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places, _temp527);
return Cyc_CfFlowInfo_mkLeafPI( _temp533, _temp531);} return Cyc_CfFlowInfo_mkLeafPI(
_temp533, Cyc_CfFlowInfo_join_initlevel( _temp531,( void*) Cyc_CfFlowInfo_ThisIL));}
_LL510: { struct _tuple6 _temp557=({ struct _tuple6 _temp556; _temp556.f1= env->d1;
_temp556.f2= 0; _temp556;}); goto _LL558; _LL558: if( _temp537 ==( void*) Cyc_CfFlowInfo_AllIL?
Cyc_CfFlowInfo_isAllInit_pinfo(& _temp557, Cyc_CfFlowInfo_lookup_place( env->d1,
_temp543)): 0){* env->places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places, _temp543);
return Cyc_CfFlowInfo_mkLeafPI( _temp539, _temp537);} return Cyc_CfFlowInfo_mkLeafPI(
_temp539, Cyc_CfFlowInfo_join_initlevel(( void*) Cyc_CfFlowInfo_ThisIL, _temp537));}
_LL512: if( Cyc_CfFlowInfo_place_cmp( _temp551, _temp547) == 0){ return( void*)({
struct Cyc_CfFlowInfo_LeafPI_struct* _temp559=( struct Cyc_CfFlowInfo_LeafPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct)); _temp559[ 0]=({ struct
Cyc_CfFlowInfo_LeafPI_struct _temp560; _temp560.tag= Cyc_CfFlowInfo_LeafPI;
_temp560.f1=( void*) state1; _temp560;}); _temp559;});}{ struct _tuple6 _temp562=({
struct _tuple6 _temp561; _temp561.f1= env->d1; _temp561.f2= 0; _temp561;}); goto
_LL563; _LL563: { struct _tuple6 _temp565=({ struct _tuple6 _temp564; _temp564.f1=
env->d2; _temp564.f2= 0; _temp564;}); goto _LL566; _LL566: if( Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp562, Cyc_CfFlowInfo_lookup_place( env->d1, _temp551))? Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp565, Cyc_CfFlowInfo_lookup_place( env->d2, _temp547)): 0){* env->places=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(* env->places, _temp551);* env->places=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places,
_temp547); return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,( void*)
Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,(
void*) Cyc_CfFlowInfo_ThisIL);}} _LL504:;} void* Cyc_CfFlowInfo_join_pathinfo(
struct Cyc_CfFlowInfo_JoinEnv* env, void* pinfo1, void* pinfo2){ if( pinfo1 ==
pinfo2){ return pinfo1;}{ struct _tuple3 _temp568=({ struct _tuple3 _temp567;
_temp567.f1= pinfo1; _temp567.f2= pinfo2; _temp567;}); void* _temp578; void*
_temp580; void* _temp582; void* _temp584; void* _temp586; struct Cyc_Dict_Dict*
_temp588; void* _temp590; struct Cyc_Dict_Dict* _temp592; void* _temp594; struct
Cyc_Dict_Dict* _temp596; void* _temp598; struct Cyc_Dict_Dict* _temp600; _LL570:
_LL583: _temp582= _temp568.f1; if(*(( int*) _temp582) == Cyc_CfFlowInfo_LeafPI){
_LL585: _temp584=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp582)->f1;
goto _LL579;} else{ goto _LL572;} _LL579: _temp578= _temp568.f2; if(*(( int*)
_temp578) == Cyc_CfFlowInfo_LeafPI){ _LL581: _temp580=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp578)->f1; goto _LL571;} else{ goto _LL572;} _LL572: _LL591: _temp590=
_temp568.f1; if(*(( int*) _temp590) == Cyc_CfFlowInfo_TuplePI){ _LL593: _temp592=((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp590)->f1; goto _LL587;} else{ goto
_LL574;} _LL587: _temp586= _temp568.f2; if(*(( int*) _temp586) == Cyc_CfFlowInfo_TuplePI){
_LL589: _temp588=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp586)->f1; goto
_LL573;} else{ goto _LL574;} _LL574: _LL599: _temp598= _temp568.f1; if(*(( int*)
_temp598) == Cyc_CfFlowInfo_StructPI){ _LL601: _temp600=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp598)->f1; goto _LL595;} else{ goto _LL576;} _LL595: _temp594= _temp568.f2;
if(*(( int*) _temp594) == Cyc_CfFlowInfo_StructPI){ _LL597: _temp596=(( struct
Cyc_CfFlowInfo_StructPI_struct*) _temp594)->f1; goto _LL575;} else{ goto _LL576;}
_LL576: goto _LL577; _LL571: if( _temp584 == _temp580){ return pinfo1;} return
Cyc_CfFlowInfo_join_initstate( env, _temp584, _temp580); _LL573: if( _temp592 ==
_temp588){ return pinfo1;} return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp602=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp602[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp603; _temp603.tag= Cyc_CfFlowInfo_TuplePI;
_temp603.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,
env, _temp592, _temp588); _temp603;}); _temp602;}); _LL575: if( _temp600 ==
_temp596){ return pinfo1;} return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp604=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp604[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp605; _temp605.tag=
Cyc_CfFlowInfo_StructPI; _temp605.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(
Cyc_CfFlowInfo_join_pathinfo, env, _temp600, _temp596); _temp605;}); _temp604;});
_LL577:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp606=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp606[ 0]=({ struct Cyc_Core_Impossible_struct _temp607; _temp607.tag= Cyc_Core_Impossible;
_temp607.f1=({ char* _temp608="join_pathinfo"; struct _tagged_string _temp609;
_temp609.curr= _temp608; _temp609.base= _temp608; _temp609.last_plus_one=
_temp608 + 14; _temp609;}); _temp607;}); _temp606;})); _LL569:;}} void* Cyc_CfFlowInfo_join_flow(
void* f1, void* f2){ if( Cyc_CfFlowInfo_empty_place_set_ptr == 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({
struct Cyc_Set_Set** _temp610=( struct Cyc_Set_Set**) GC_malloc( sizeof( struct
Cyc_Set_Set*) * 1); _temp610[ 0]=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*))) Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp);
_temp610;});} if( f1 == f2){ return f1;}{ struct _tuple3 _temp612=({ struct
_tuple3 _temp611; _temp611.f1= f1; _temp611.f2= f2; _temp611;}); void* _temp620;
void* _temp622; void* _temp624; void* _temp626; void* _temp628; struct Cyc_Dict_Dict*
_temp630; void* _temp632; struct Cyc_Dict_Dict* _temp634; _LL614: _LL623:
_temp622= _temp612.f1; if( _temp622 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL621;} else{ goto _LL616;} _LL621: _temp620= _temp612.f2; goto _LL615; _LL616:
_LL627: _temp626= _temp612.f1; goto _LL625; _LL625: _temp624= _temp612.f2; if(
_temp624 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL617;} else{ goto _LL618;}
_LL618: _LL633: _temp632= _temp612.f1; if(( unsigned int) _temp632 > 1u?*(( int*)
_temp632) == Cyc_CfFlowInfo_InitsFL: 0){ _LL635: _temp634=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp632)->f1; goto _LL629;} else{ goto _LL613;} _LL629: _temp628= _temp612.f2;
if(( unsigned int) _temp628 > 1u?*(( int*) _temp628) == Cyc_CfFlowInfo_InitsFL:
0){ _LL631: _temp630=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp628)->f1;
goto _LL619;} else{ goto _LL613;} _LL615: return f2; _LL617: return f1; _LL619:
if( _temp634 == _temp630){ return f1;}{ struct Cyc_Set_Set* _temp636=*(( struct
Cyc_Set_Set**) _check_null( Cyc_CfFlowInfo_empty_place_set_ptr)); goto _LL637;
_LL637: { struct Cyc_CfFlowInfo_JoinEnv _temp639=({ struct Cyc_CfFlowInfo_JoinEnv
_temp638; _temp638.d1= _temp634; _temp638.d2= _temp630; _temp638.places=&
_temp636; _temp638;}); goto _LL640; _LL640: { struct Cyc_Dict_Dict* _temp641=((
struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*, void*,
void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,& _temp639, _temp634,
_temp630); goto _LL642; _LL642: return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp643=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp643[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp644; _temp644.tag= Cyc_CfFlowInfo_InitsFL;
_temp644.f1= Cyc_CfFlowInfo_escape_these( _temp636, _temp641); _temp644;});
_temp643;});}}} _LL613:;}} int Cyc_CfFlowInfo_initstate_lessthan_approx( void*
state1, void* state2){ struct _tuple3 _temp646=({ struct _tuple3 _temp645;
_temp645.f1= state1; _temp645.f2= state2; _temp645;}); void* _temp654; void*
_temp656; void* _temp658; void* _temp660; void* _temp662; void* _temp664; void*
_temp666; struct Cyc_CfFlowInfo_Place* _temp668; void* _temp670; struct Cyc_CfFlowInfo_Place*
_temp672; _LL648: _LL661: _temp660= _temp646.f1; if(*(( int*) _temp660) == Cyc_CfFlowInfo_UnknownIS){
_LL665: _temp664=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp660)->f1;
goto _LL663; _LL663: _temp662=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp660)->f2; goto _LL655;} else{ goto _LL650;} _LL655: _temp654= _temp646.f2;
if(*(( int*) _temp654) == Cyc_CfFlowInfo_UnknownIS){ _LL659: _temp658=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp654)->f1; goto _LL657; _LL657:
_temp656=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp654)->f2; goto
_LL649;} else{ goto _LL650;} _LL650: _LL671: _temp670= _temp646.f1; if(*(( int*)
_temp670) == Cyc_CfFlowInfo_MustPointTo){ _LL673: _temp672=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp670)->f1; goto _LL667;} else{ goto _LL652;} _LL667: _temp666= _temp646.f2;
if(*(( int*) _temp666) == Cyc_CfFlowInfo_MustPointTo){ _LL669: _temp668=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp666)->f1; goto _LL651;} else{
goto _LL652;} _LL652: goto _LL653; _LL649: return Cyc_CfFlowInfo_join_escape(
_temp664, _temp658) == _temp658? Cyc_CfFlowInfo_join_initlevel( _temp662,
_temp656) == _temp656: 0; _LL651: return Cyc_CfFlowInfo_place_cmp( _temp672,
_temp668) == 0; _LL653: return 0; _LL647:;} int Cyc_CfFlowInfo_pinfo_lessthan_approx(
void* ignore, void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return 1;}{
struct _tuple3 _temp675=({ struct _tuple3 _temp674; _temp674.f1= pinfo1;
_temp674.f2= pinfo2; _temp674;}); void* _temp685; void* _temp687; void* _temp689;
void* _temp691; void* _temp693; struct Cyc_Dict_Dict* _temp695; void* _temp697;
struct Cyc_Dict_Dict* _temp699; void* _temp701; struct Cyc_Dict_Dict* _temp703;
void* _temp705; struct Cyc_Dict_Dict* _temp707; _LL677: _LL690: _temp689=
_temp675.f1; if(*(( int*) _temp689) == Cyc_CfFlowInfo_LeafPI){ _LL692: _temp691=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp689)->f1; goto _LL686;}
else{ goto _LL679;} _LL686: _temp685= _temp675.f2; if(*(( int*) _temp685) == Cyc_CfFlowInfo_LeafPI){
_LL688: _temp687=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp685)->f1;
goto _LL678;} else{ goto _LL679;} _LL679: _LL698: _temp697= _temp675.f1; if(*((
int*) _temp697) == Cyc_CfFlowInfo_TuplePI){ _LL700: _temp699=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp697)->f1; goto _LL694;} else{ goto _LL681;} _LL694: _temp693= _temp675.f2;
if(*(( int*) _temp693) == Cyc_CfFlowInfo_TuplePI){ _LL696: _temp695=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp693)->f1; goto _LL680;} else{ goto _LL681;}
_LL681: _LL706: _temp705= _temp675.f1; if(*(( int*) _temp705) == Cyc_CfFlowInfo_StructPI){
_LL708: _temp707=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp705)->f1; goto
_LL702;} else{ goto _LL683;} _LL702: _temp701= _temp675.f2; if(*(( int*)
_temp701) == Cyc_CfFlowInfo_StructPI){ _LL704: _temp703=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp701)->f1; goto _LL682;} else{ goto _LL683;} _LL683: goto _LL684; _LL678:
return _temp691 == _temp687? 1: Cyc_CfFlowInfo_initstate_lessthan_approx(
_temp691, _temp687); _LL680: return _temp699 == _temp695? 1:(( int(*)( int(* f)(
int, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( int ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp699, _temp695); _LL682: return _temp707 == _temp703? 1:(( int(*)( int(* f)(
struct _tagged_string*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_string* ignore, void*
pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp707, _temp703);
_LL684:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp709=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp709[ 0]=({ struct Cyc_Core_Impossible_struct _temp710; _temp710.tag= Cyc_Core_Impossible;
_temp710.f1=({ char* _temp711="pinfo_lessthan"; struct _tagged_string _temp712;
_temp712.curr= _temp711; _temp712.base= _temp711; _temp712.last_plus_one=
_temp711 + 15; _temp712;}); _temp710;}); _temp709;})); _LL676:;}} int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2){ if( f1 == f2){ return 1;}{ struct _tuple3 _temp714=({
struct _tuple3 _temp713; _temp713.f1= f1; _temp713.f2= f2; _temp713;}); void*
_temp722; void* _temp724; void* _temp726; void* _temp728; void* _temp730; struct
Cyc_Dict_Dict* _temp732; void* _temp734; struct Cyc_Dict_Dict* _temp736; _LL716:
_LL725: _temp724= _temp714.f1; if( _temp724 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL723;} else{ goto _LL718;} _LL723: _temp722= _temp714.f2; goto _LL717;
_LL718: _LL729: _temp728= _temp714.f1; goto _LL727; _LL727: _temp726= _temp714.f2;
if( _temp726 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL719;} else{ goto
_LL720;} _LL720: _LL735: _temp734= _temp714.f1; if(( unsigned int) _temp734 > 1u?*((
int*) _temp734) == Cyc_CfFlowInfo_InitsFL: 0){ _LL737: _temp736=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp734)->f1; goto _LL731;} else{ goto _LL715;} _LL731: _temp730= _temp714.f2;
if(( unsigned int) _temp730 > 1u?*(( int*) _temp730) == Cyc_CfFlowInfo_InitsFL:
0){ _LL733: _temp732=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp730)->f1;
goto _LL721;} else{ goto _LL715;} _LL717: return 1; _LL719: return 0; _LL721:
if( _temp736 == _temp732){ return 1;} return(( int(*)( int(* f)( void*, void*,
void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( void* ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp736, _temp732); _LL715:;}}