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
struct Cyc_Core_Opt* Cyc_Core_opt_t; struct _tuple0{ void* f1; void* f2; } ;
extern void* Cyc_Core_snd( struct _tuple0*); extern char Cyc_Core_InvalidArg[ 15u];
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
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
char Cyc_List_Nth[ 8u]; extern struct Cyc_List_List* Cyc_List_rzip( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(* f)( void*,
int), void* env); struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_gdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u]; extern char Cyc_Dict_Absent[
11u]; struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef
struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern
int Cyc_Set_cardinality( struct Cyc_Set_Set* s); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent[ 11u]; struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrcmp( struct _tagged_string, struct
_tagged_string); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col; } ; typedef
struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext[ 14u]; typedef struct
_tagged_string* Cyc_Absyn_field_name_t; typedef struct _tagged_string* Cyc_Absyn_var_t;
typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t;
struct _tuple1{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple1*
Cyc_Absyn_qvar_t; typedef struct _tuple1* Cyc_Absyn_qvar_opt_t; typedef struct
_tuple1* Cyc_Absyn_typedef_name_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct
Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
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
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple1* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple1*
tunion_name; struct _tuple1* field_name; int is_xtunion; } ; static const int
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
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_AnonStructType=
11; struct Cyc_Absyn_AnonStructType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonUnionType= 12; struct Cyc_Absyn_AnonUnionType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_EnumType= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
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
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
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
f1; struct _tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 16; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple1* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; static const
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
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar( void*); extern
void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t; extern struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl( struct _tuple1* x, void* t, struct Cyc_Absyn_Exp* init);
struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct
Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef
struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_kind2string( void*); extern
struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); static const
int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ;
static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{ int
tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern void* Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern char
Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string);
extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_string);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle*, struct Cyc_List_List*, void*); struct _tuple4{ struct Cyc_Absyn_Tvar*
f1; void* f2; } ; extern struct _tuple4* Cyc_Tcutil_r_make_inst_var( struct
_RegionHandle*, struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct
_tagged_string err_msg); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); struct _tuple5{ struct Cyc_List_List* f1;
struct Cyc_List_List* f2; } ; extern struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; static void Cyc_Tcpat_resolve_pat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p){ void* _temp0=( void*) p->r; struct _tuple1*
_temp10; struct Cyc_List_List* _temp12; struct Cyc_List_List* _temp14; struct
_tuple1* _temp16; struct Cyc_List_List* _temp18; struct Cyc_List_List* _temp20;
struct _tuple1* _temp22; _LL2: if(( unsigned int) _temp0 > 2u?*(( int*) _temp0)
== Cyc_Absyn_UnknownId_p: 0){ _LL11: _temp10=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: if(( unsigned int) _temp0 > 2u?*((
int*) _temp0) == Cyc_Absyn_UnknownCall_p: 0){ _LL17: _temp16=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f1; goto _LL15; _LL15: _temp14=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f2; goto _LL13; _LL13: _temp12=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f3; goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 2u?*((
int*) _temp0) == Cyc_Absyn_UnknownFields_p: 0){ _LL23: _temp22=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f1; goto _LL21; _LL21: _temp20=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f2; goto _LL19; _LL19: _temp18=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f3; goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9; _LL3:{ struct
_handler_cons _temp24; _push_handler(& _temp24);{ int _temp26= 0; if( setjmp(
_temp24.handler)){ _temp26= 1;} if( ! _temp26){{ void* _temp27= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp10); struct Cyc_Absyn_Structdecl* _temp37; struct Cyc_Absyn_Tunionfield*
_temp39; struct Cyc_Absyn_Tuniondecl* _temp41; struct Cyc_Absyn_Enumfield*
_temp43; struct Cyc_Absyn_Enumdecl* _temp45; void* _temp47; _LL29: if(*(( int*)
_temp27) == Cyc_Tcenv_StructRes){ _LL38: _temp37=(( struct Cyc_Tcenv_StructRes_struct*)
_temp27)->f1; goto _LL30;} else{ goto _LL31;} _LL31: if(*(( int*) _temp27) ==
Cyc_Tcenv_TunionRes){ _LL42: _temp41=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp27)->f1; goto _LL40; _LL40: _temp39=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp27)->f2; goto _LL32;} else{ goto _LL33;} _LL33: if(*(( int*) _temp27) ==
Cyc_Tcenv_EnumRes){ _LL46: _temp45=(( struct Cyc_Tcenv_EnumRes_struct*) _temp27)->f1;
goto _LL44; _LL44: _temp43=(( struct Cyc_Tcenv_EnumRes_struct*) _temp27)->f2;
goto _LL34;} else{ goto _LL35;} _LL35: if(*(( int*) _temp27) == Cyc_Tcenv_VarRes){
_LL48: _temp47=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp27)->f1; goto
_LL36;} else{ goto _LL28;} _LL30: Cyc_Tcutil_terr( p->loc,({ char* _temp49="struct tag used without arguments in pattern";
struct _tagged_string _temp50; _temp50.curr= _temp49; _temp50.base= _temp49;
_temp50.last_plus_one= _temp49 + 45; _temp50;})); _npop_handler( 0u); return;
_LL32:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp51=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp51[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp52; _temp52.tag= Cyc_Absyn_Tunion_p;
_temp52.f1= _temp41; _temp52.f2= _temp39; _temp52.f3= 0; _temp52.f4= 0; _temp52;});
_temp51;}))); _npop_handler( 0u); return; _LL34:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_p_struct* _temp53=( struct Cyc_Absyn_Enum_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct)); _temp53[ 0]=({ struct Cyc_Absyn_Enum_p_struct
_temp54; _temp54.tag= Cyc_Absyn_Enum_p; _temp54.f1= _temp45; _temp54.f2= _temp43;
_temp54;}); _temp53;}))); _npop_handler( 0u); return; _LL36: goto _LL28; _LL28:;};
_pop_handler();} else{ void* _temp25=( void*) _exn_thrown; void* _temp56=
_temp25; _LL58: if( _temp56 == Cyc_Dict_Absent){ goto _LL59;} else{ goto _LL60;}
_LL60: goto _LL61; _LL59: goto _LL57; _LL61:( void) _throw( _temp56); _LL57:;}}}{
void* _temp62=(* _temp10).f1; struct Cyc_List_List* _temp70; _LL64: if( _temp62
==( void*) Cyc_Absyn_Loc_n){ goto _LL65;} else{ goto _LL66;} _LL66: if((
unsigned int) _temp62 > 1u?*(( int*) _temp62) == Cyc_Absyn_Rel_n: 0){ _LL71:
_temp70=(( struct Cyc_Absyn_Rel_n_struct*) _temp62)->f1; if( _temp70 == 0){ goto
_LL67;} else{ goto _LL68;}} else{ goto _LL68;} _LL68: goto _LL69; _LL65: goto
_LL67; _LL67:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;( void*)( p->r=( void*)((
void*)({ struct Cyc_Absyn_Var_p_struct* _temp72=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp72[ 0]=({ struct Cyc_Absyn_Var_p_struct
_temp73; _temp73.tag= Cyc_Absyn_Var_p; _temp73.f1= Cyc_Absyn_new_vardecl(
_temp10,( void*) Cyc_Absyn_VoidType, 0); _temp73;}); _temp72;}))); return; _LL69:
Cyc_Tcutil_terr( p->loc,({ char* _temp74="qualified variable in pattern"; struct
_tagged_string _temp75; _temp75.curr= _temp74; _temp75.base= _temp74; _temp75.last_plus_one=
_temp74 + 30; _temp75;})); return; _LL63:;} _LL5:{ struct _handler_cons _temp76;
_push_handler(& _temp76);{ int _temp78= 0; if( setjmp( _temp76.handler)){
_temp78= 1;} if( ! _temp78){{ void* _temp79= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp16); struct Cyc_Absyn_Structdecl* _temp89; struct Cyc_Absyn_Tunionfield*
_temp91; struct Cyc_Absyn_Tuniondecl* _temp93; struct Cyc_Absyn_Enumfield*
_temp95; struct Cyc_Absyn_Enumdecl* _temp97; void* _temp99; _LL81: if(*(( int*)
_temp79) == Cyc_Tcenv_StructRes){ _LL90: _temp89=(( struct Cyc_Tcenv_StructRes_struct*)
_temp79)->f1; goto _LL82;} else{ goto _LL83;} _LL83: if(*(( int*) _temp79) ==
Cyc_Tcenv_TunionRes){ _LL94: _temp93=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp79)->f1; goto _LL92; _LL92: _temp91=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp79)->f2; goto _LL84;} else{ goto _LL85;} _LL85: if(*(( int*) _temp79) ==
Cyc_Tcenv_EnumRes){ _LL98: _temp97=(( struct Cyc_Tcenv_EnumRes_struct*) _temp79)->f1;
goto _LL96; _LL96: _temp95=(( struct Cyc_Tcenv_EnumRes_struct*) _temp79)->f2;
goto _LL86;} else{ goto _LL87;} _LL87: if(*(( int*) _temp79) == Cyc_Tcenv_VarRes){
_LL100: _temp99=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp79)->f1; goto
_LL88;} else{ goto _LL80;} _LL82: { struct Cyc_List_List* _temp101= 0; goto
_LL102; _LL102: for( 0; _temp12 != 0; _temp12=(( struct Cyc_List_List*)
_check_null( _temp12))->tl){ _temp101=({ struct Cyc_List_List* _temp103=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp103->hd=( void*)({
struct _tuple6* _temp104=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp104->f1= 0; _temp104->f2=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp12))->hd; _temp104;}); _temp103->tl= _temp101; _temp103;});}(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp105=(
struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp105[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp106; _temp106.tag= Cyc_Absyn_Struct_p;
_temp106.f1= _temp89; _temp106.f2= 0; _temp106.f3= _temp14; _temp106.f4=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp101);
_temp106;}); _temp105;}))); _npop_handler( 0u); return;} _LL84:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp107=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp107[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp108; _temp108.tag= Cyc_Absyn_Tunion_p; _temp108.f1=
_temp93; _temp108.f2= _temp91; _temp108.f3= _temp14; _temp108.f4= _temp12;
_temp108;}); _temp107;}))); _npop_handler( 0u); return; _LL86: Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp109= Cyc_Absynpp_qvar2string( _temp16);
xprintf("bad enum constructor %.*s in pattern", _temp109.last_plus_one -
_temp109.curr, _temp109.curr);})); _npop_handler( 0u); return; _LL88: goto _LL80;
_LL80:;}; _pop_handler();} else{ void* _temp77=( void*) _exn_thrown; void*
_temp111= _temp77; _LL113: if( _temp111 == Cyc_Dict_Absent){ goto _LL114;} else{
goto _LL115;} _LL115: goto _LL116; _LL114: goto _LL112; _LL116:( void) _throw(
_temp111); _LL112:;}}} Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp117=
Cyc_Absynpp_qvar2string( _temp16); xprintf("%.*s is not a constructor in pattern",
_temp117.last_plus_one - _temp117.curr, _temp117.curr);})); return; _LL7:{
struct _handler_cons _temp118; _push_handler(& _temp118);{ int _temp120= 0; if(
setjmp( _temp118.handler)){ _temp120= 1;} if( ! _temp120){{ void* _temp121= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp22); struct Cyc_Absyn_Structdecl* _temp127; _LL123: if(*(( int*)
_temp121) == Cyc_Tcenv_StructRes){ _LL128: _temp127=(( struct Cyc_Tcenv_StructRes_struct*)
_temp121)->f1; goto _LL124;} else{ goto _LL125;} _LL125: goto _LL126; _LL124:(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp129=(
struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp129[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp130; _temp130.tag= Cyc_Absyn_Struct_p;
_temp130.f1= _temp127; _temp130.f2= 0; _temp130.f3= _temp20; _temp130.f4=
_temp18; _temp130;}); _temp129;}))); _npop_handler( 0u); return; _LL126: goto
_LL122; _LL122:;}; _pop_handler();} else{ void* _temp119=( void*) _exn_thrown;
void* _temp132= _temp119; _LL134: if( _temp132 == Cyc_Dict_Absent){ goto _LL135;}
else{ goto _LL136;} _LL136: goto _LL137; _LL135: goto _LL133; _LL137:( void)
_throw( _temp132); _LL133:;}}} Cyc_Tcutil_terr( p->loc,({ struct _tagged_string
_temp138= Cyc_Absynpp_qvar2string( _temp22); xprintf("%.*s is not a struct name, but pattern has designators",
_temp138.last_plus_one - _temp138.curr, _temp138.curr);})); return; _LL9:
return; _LL1:;} static struct _tagged_string* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl*
vd){ return(* vd->name).f2;} static void* Cyc_Tcpat_any_type( void** topt){ if(
topt != 0){ return*(( void**) _check_null( topt));} return Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind);} static void* Cyc_Tcpat_num_type( void** topt, void*
numt){ if( topt != 0? Cyc_Tcutil_coerceable(*(( void**) _check_null( topt))): 0){
return*(( void**) _check_null( topt));} return numt;} static void Cyc_Tcpat_set_vd(
struct Cyc_Absyn_Vardecl* vd, struct Cyc_List_List** v_result_ptr, void* t){(
void*)( vd->type=( void*) t); vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=({
struct Cyc_List_List* _temp139=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp139->hd=( void*) vd; _temp139->tl=* v_result_ptr;
_temp139;});} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; struct
_tuple8{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat* f2; } ; struct
_tuple5 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p,
void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t; struct
Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void* _temp140=(
void*) p->r; struct Cyc_Absyn_Vardecl* _temp174; struct Cyc_Absyn_Vardecl*
_temp176; int _temp178; void* _temp180; int _temp182; void* _temp184; char
_temp186; struct _tagged_string _temp188; struct Cyc_Absyn_Enumfield* _temp190;
struct Cyc_Absyn_Enumdecl* _temp192; struct Cyc_Absyn_Pat* _temp194; struct Cyc_List_List*
_temp196; struct Cyc_List_List* _temp198; struct Cyc_List_List* _temp200; struct
Cyc_Core_Opt* _temp202; struct Cyc_Core_Opt** _temp204; struct Cyc_Absyn_Structdecl*
_temp205; struct Cyc_List_List* _temp207; struct Cyc_List_List* _temp209; struct
Cyc_Absyn_Tunionfield* _temp211; struct Cyc_Absyn_Tuniondecl* _temp213; struct
_tuple1* _temp215; struct Cyc_List_List* _temp217; struct Cyc_List_List*
_temp219; struct _tuple1* _temp221; struct Cyc_List_List* _temp223; struct Cyc_List_List*
_temp225; struct _tuple1* _temp227; _LL142: if( _temp140 ==( void*) Cyc_Absyn_Wild_p){
goto _LL143;} else{ goto _LL144;} _LL144: if(( unsigned int) _temp140 > 2u?*((
int*) _temp140) == Cyc_Absyn_Var_p: 0){ _LL175: _temp174=(( struct Cyc_Absyn_Var_p_struct*)
_temp140)->f1; goto _LL145;} else{ goto _LL146;} _LL146: if(( unsigned int)
_temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_Reference_p: 0){ _LL177: _temp176=((
struct Cyc_Absyn_Reference_p_struct*) _temp140)->f1; goto _LL147;} else{ goto
_LL148;} _LL148: if(( unsigned int) _temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_Int_p:
0){ _LL181: _temp180=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp140)->f1;
if( _temp180 ==( void*) Cyc_Absyn_Unsigned){ goto _LL179;} else{ goto _LL150;}
_LL179: _temp178=(( struct Cyc_Absyn_Int_p_struct*) _temp140)->f2; goto _LL149;}
else{ goto _LL150;} _LL150: if(( unsigned int) _temp140 > 2u?*(( int*) _temp140)
== Cyc_Absyn_Int_p: 0){ _LL185: _temp184=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp140)->f1; if( _temp184 ==( void*) Cyc_Absyn_Signed){ goto _LL183;} else{
goto _LL152;} _LL183: _temp182=(( struct Cyc_Absyn_Int_p_struct*) _temp140)->f2;
goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int) _temp140 > 2u?*((
int*) _temp140) == Cyc_Absyn_Char_p: 0){ _LL187: _temp186=(( struct Cyc_Absyn_Char_p_struct*)
_temp140)->f1; goto _LL153;} else{ goto _LL154;} _LL154: if(( unsigned int)
_temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_Float_p: 0){ _LL189: _temp188=((
struct Cyc_Absyn_Float_p_struct*) _temp140)->f1; goto _LL155;} else{ goto _LL156;}
_LL156: if(( unsigned int) _temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_Enum_p:
0){ _LL193: _temp192=(( struct Cyc_Absyn_Enum_p_struct*) _temp140)->f1; goto
_LL191; _LL191: _temp190=(( struct Cyc_Absyn_Enum_p_struct*) _temp140)->f2; goto
_LL157;} else{ goto _LL158;} _LL158: if( _temp140 ==( void*) Cyc_Absyn_Null_p){
goto _LL159;} else{ goto _LL160;} _LL160: if(( unsigned int) _temp140 > 2u?*((
int*) _temp140) == Cyc_Absyn_Pointer_p: 0){ _LL195: _temp194=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp140)->f1; goto _LL161;} else{ goto _LL162;} _LL162: if(( unsigned int)
_temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_Tuple_p: 0){ _LL197: _temp196=((
struct Cyc_Absyn_Tuple_p_struct*) _temp140)->f1; goto _LL163;} else{ goto _LL164;}
_LL164: if(( unsigned int) _temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_Struct_p:
0){ _LL206: _temp205=(( struct Cyc_Absyn_Struct_p_struct*) _temp140)->f1; goto
_LL203; _LL203: _temp202=(( struct Cyc_Absyn_Struct_p_struct*) _temp140)->f2;
_temp204=&(( struct Cyc_Absyn_Struct_p_struct*) _temp140)->f2; goto _LL201;
_LL201: _temp200=(( struct Cyc_Absyn_Struct_p_struct*) _temp140)->f3; goto
_LL199; _LL199: _temp198=(( struct Cyc_Absyn_Struct_p_struct*) _temp140)->f4;
goto _LL165;} else{ goto _LL166;} _LL166: if(( unsigned int) _temp140 > 2u?*((
int*) _temp140) == Cyc_Absyn_Tunion_p: 0){ _LL214: _temp213=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp140)->f1; goto _LL212; _LL212: _temp211=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp140)->f2; goto _LL210; _LL210: _temp209=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp140)->f3; goto _LL208; _LL208: _temp207=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp140)->f4; goto _LL167;} else{ goto _LL168;} _LL168: if(( unsigned int)
_temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_UnknownId_p: 0){ _LL216: _temp215=((
struct Cyc_Absyn_UnknownId_p_struct*) _temp140)->f1; goto _LL169;} else{ goto
_LL170;} _LL170: if(( unsigned int) _temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_UnknownCall_p:
0){ _LL222: _temp221=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp140)->f1;
goto _LL220; _LL220: _temp219=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp140)->f2; goto _LL218; _LL218: _temp217=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp140)->f3; goto _LL171;} else{ goto _LL172;} _LL172: if(( unsigned int)
_temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_UnknownFields_p: 0){ _LL228:
_temp227=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp140)->f1; goto _LL226;
_LL226: _temp225=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp140)->f2;
goto _LL224; _LL224: _temp223=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp140)->f3; goto _LL173;} else{ goto _LL141;} _LL143: t= Cyc_Tcpat_any_type(
topt); goto _LL141; _LL145: t= Cyc_Tcpat_any_type( topt); Cyc_Tcpat_set_vd(
_temp174,& v_result, t); goto _LL141; _LL147: t= Cyc_Tcpat_any_type( topt); if(
region_opt == 0){ Cyc_Tcutil_terr( p->loc,({ char* _temp229="* pattern would point into an unknown region";
struct _tagged_string _temp230; _temp230.curr= _temp229; _temp230.base= _temp229;
_temp230.last_plus_one= _temp229 + 45; _temp230;})); goto _LL141;} Cyc_Tcpat_set_vd(
_temp176,& v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp231=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp231[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp232; _temp232.tag= Cyc_Absyn_PointerType;
_temp232.f1=({ struct Cyc_Absyn_PtrInfo _temp233; _temp233.elt_typ=( void*) t;
_temp233.rgn_typ=( void*)*(( void**) _check_null( region_opt)); _temp233.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0); _temp233.tq= Cyc_Absyn_empty_tqual();
_temp233.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp233;}); _temp232;}); _temp231;})); goto _LL141; _LL149: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_uint_t); goto _LL141; _LL151: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t);
goto _LL141; _LL153: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto
_LL141; _LL155: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_float_t); goto _LL141;
_LL157: t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp234=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp234[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp235; _temp235.tag= Cyc_Absyn_EnumType; _temp235.f1=
_temp192->name; _temp235.f2=( struct Cyc_Absyn_Enumdecl*) _temp192; _temp235;});
_temp234;}); goto _LL141; _LL159: if( topt != 0){ void* _temp236= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp242; _LL238: if((
unsigned int) _temp236 > 4u?*(( int*) _temp236) == Cyc_Absyn_PointerType: 0){
_LL243: _temp242=(( struct Cyc_Absyn_PointerType_struct*) _temp236)->f1; goto
_LL239;} else{ goto _LL240;} _LL240: goto _LL241; _LL239: t=*(( void**)
_check_null( topt)); goto tcpat_end; _LL241: goto _LL237; _LL237:;} t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp244=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp244[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp245; _temp245.tag= Cyc_Absyn_PointerType;
_temp245.f1=({ struct Cyc_Absyn_PtrInfo _temp246; _temp246.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp246.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp246.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp246.tq= Cyc_Absyn_empty_tqual(); _temp246.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp246;}); _temp245;});
_temp244;}); goto _LL141; _LL161: { void* inner_typ=( void*) Cyc_Absyn_VoidType;
void** _temp247= 0; goto _LL248; _LL248: if( topt != 0){ void* _temp249= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp255; struct Cyc_Absyn_Conref*
_temp257; struct Cyc_Absyn_Tqual _temp259; struct Cyc_Absyn_Conref* _temp261;
void* _temp263; void* _temp265; _LL251: if(( unsigned int) _temp249 > 4u?*(( int*)
_temp249) == Cyc_Absyn_PointerType: 0){ _LL256: _temp255=(( struct Cyc_Absyn_PointerType_struct*)
_temp249)->f1; _LL266: _temp265=( void*) _temp255.elt_typ; goto _LL264; _LL264:
_temp263=( void*) _temp255.rgn_typ; goto _LL262; _LL262: _temp261= _temp255.nullable;
goto _LL260; _LL260: _temp259= _temp255.tq; goto _LL258; _LL258: _temp257=
_temp255.bounds; goto _LL252;} else{ goto _LL253;} _LL253: goto _LL254; _LL252:
inner_typ= _temp265; _temp247=( void**)& inner_typ; goto _LL250; _LL254: goto
_LL250; _LL250:;}{ void* ptr_rgn= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
struct Cyc_List_List* _temp269; struct Cyc_List_List* _temp271; struct _tuple5
_temp267= Cyc_Tcpat_tcPatRec( te, _temp194, _temp247,( void**)& ptr_rgn); _LL272:
_temp271= _temp267.f1; goto _LL270; _LL270: _temp269= _temp267.f2; goto _LL268;
_LL268: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp271); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp269); if( _temp247 == 0){ void* _temp273= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp194->topt))->v); struct Cyc_Absyn_TunionFieldInfo
_temp279; struct Cyc_List_List* _temp281; void* _temp283; struct Cyc_Absyn_Tunionfield*
_temp285; struct Cyc_Absyn_Tuniondecl* _temp287; _LL275: if(( unsigned int)
_temp273 > 4u?*(( int*) _temp273) == Cyc_Absyn_TunionFieldType: 0){ _LL280:
_temp279=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp273)->f1; _LL284:
_temp283=( void*) _temp279.field_info; if(*(( int*) _temp283) == Cyc_Absyn_KnownTunionfield){
_LL288: _temp287=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp283)->f1;
goto _LL286; _LL286: _temp285=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp283)->f2; goto _LL282;} else{ goto _LL277;} _LL282: _temp281= _temp279.targs;
goto _LL276;} else{ goto _LL277;} _LL277: goto _LL278; _LL276: t=( void*)({
struct Cyc_Absyn_TunionType_struct* _temp289=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp289[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp290; _temp290.tag= Cyc_Absyn_TunionType;
_temp290.f1=({ struct Cyc_Absyn_TunionInfo _temp291; _temp291.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp292=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp292[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp293; _temp293.tag= Cyc_Absyn_KnownTunion;
_temp293.f1= _temp287; _temp293;}); _temp292;})); _temp291.targs= _temp281;
_temp291.rgn=( void*) ptr_rgn; _temp291;}); _temp290;}); _temp289;}); goto
_LL274; _LL278: t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp294=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp294[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp295; _temp295.tag= Cyc_Absyn_PointerType;
_temp295.f1=({ struct Cyc_Absyn_PtrInfo _temp296; _temp296.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp194->topt))->v); _temp296.rgn_typ=(
void*) ptr_rgn; _temp296.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp296.tq= Cyc_Absyn_empty_tqual(); _temp296.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp296;}); _temp295;}); _temp294;}); goto _LL274;
_LL274:;} else{ t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp297=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp297[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp298; _temp298.tag= Cyc_Absyn_PointerType;
_temp298.f1=({ struct Cyc_Absyn_PtrInfo _temp299; _temp299.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp194->topt))->v); _temp299.rgn_typ=(
void*) ptr_rgn; _temp299.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp299.tq= Cyc_Absyn_empty_tqual(); _temp299.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp299;}); _temp298;}); _temp297;});} goto _LL141;}}
_LL163: { struct Cyc_List_List* pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if(
topt != 0){ void* _temp300= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_List_List* _temp306; _LL302: if(( unsigned int) _temp300 > 4u?*(( int*)
_temp300) == Cyc_Absyn_TupleType: 0){ _LL307: _temp306=(( struct Cyc_Absyn_TupleType_struct*)
_temp300)->f1; goto _LL303;} else{ goto _LL304;} _LL304: goto _LL305; _LL303:
topt_ts= _temp306; goto _LL301; _LL305: goto _LL301; _LL301:;} for( 0; _temp196
!= 0; _temp196=(( struct Cyc_List_List*) _check_null( _temp196))->tl){ void**
_temp308= 0; goto _LL309; _LL309: if( topt_ts != 0){ _temp308=( void**)&(*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( topt_ts))->hd)).f2;
topt_ts=(( struct Cyc_List_List*) _check_null( topt_ts))->tl;}{ struct Cyc_List_List*
_temp312; struct Cyc_List_List* _temp314; struct _tuple5 _temp310= Cyc_Tcpat_tcPatRec(
te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp196))->hd,
_temp308, region_opt); _LL315: _temp314= _temp310.f1; goto _LL313; _LL313:
_temp312= _temp310.f2; goto _LL311; _LL311: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp314); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp312); pat_ts=({ struct Cyc_List_List*
_temp316=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp316->hd=( void*)({ struct _tuple7* _temp317=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp317->f1= Cyc_Absyn_empty_tqual(); _temp317->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Pat*)(( struct
Cyc_List_List*) _check_null( _temp196))->hd)->topt))->v; _temp317;}); _temp316->tl=
pat_ts; _temp316;});}} t=( void*)({ struct Cyc_Absyn_TupleType_struct* _temp318=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp318[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp319; _temp319.tag= Cyc_Absyn_TupleType;
_temp319.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
pat_ts); _temp319;}); _temp318;}); goto _LL141;} _LL165: if( _temp200 != 0){(
void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char*
_temp320="tcPat: struct<...> not implemented"; struct _tagged_string _temp321;
_temp321.curr= _temp320; _temp321.base= _temp320; _temp321.last_plus_one=
_temp320 + 35; _temp321;})));}{ struct _RegionHandle _temp322= _new_region();
struct _RegionHandle* rgn=& _temp322; _push_region( rgn);{ struct Cyc_List_List*
_temp323=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple4*(* f)(
struct _RegionHandle*, struct Cyc_Absyn_Tvar*), struct _RegionHandle* env,
struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var, rgn,
_temp205->tvs); goto _LL324; _LL324: { struct Cyc_List_List* _temp325=(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)((
void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp323); goto _LL326; _LL326: t=(
void*)({ struct Cyc_Absyn_StructType_struct* _temp327=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp327[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp328; _temp328.tag= Cyc_Absyn_StructType;
_temp328.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp205->name))->v); _temp328.f2= _temp325; _temp328.f3=({ struct
Cyc_Absyn_Structdecl** _temp329=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp329[ 0]= _temp205; _temp329;});
_temp328;}); _temp327;}); if( topt != 0){ Cyc_Tcutil_unify( t,*(( void**)
_check_null( topt)));}* _temp204=({ struct Cyc_Core_Opt* _temp330=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp330->v=( void*) _temp325;
_temp330;}); if( _temp205->fields == 0){ Cyc_Tcutil_terr( p->loc,({ char*
_temp331="can't destructure an abstract struct"; struct _tagged_string _temp332;
_temp332.curr= _temp331; _temp332.base= _temp331; _temp332.last_plus_one=
_temp331 + 37; _temp332;})); _npop_handler( 0u); goto _LL141;}{ struct Cyc_List_List*
fields=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, p->loc, _temp198,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp205->fields))->v); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple8 _temp335; struct Cyc_Absyn_Pat*
_temp336; struct Cyc_Absyn_Structfield* _temp338; struct _tuple8* _temp333=(
struct _tuple8*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp335=*
_temp333; _LL339: _temp338= _temp335.f1; goto _LL337; _LL337: _temp336= _temp335.f2;
goto _LL334; _LL334: { void* _temp340= Cyc_Tcutil_rsubstitute( rgn, _temp323,(
void*) _temp338->type); goto _LL341; _LL341: { struct Cyc_List_List* _temp344;
struct Cyc_List_List* _temp346; struct _tuple5 _temp342= Cyc_Tcpat_tcPatRec( te,
_temp336,( void**)& _temp340, region_opt); _LL347: _temp346= _temp342.f1; goto
_LL345; _LL345: _temp344= _temp342.f2; goto _LL343; _LL343: tv_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp346); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp344); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp336->topt))->v, _temp340)){ Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp348=* _temp338->name; struct _tagged_string
_temp349= Cyc_Absynpp_typ2string( _temp340); struct _tagged_string _temp350= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp336->topt))->v); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp348.last_plus_one - _temp348.curr, _temp348.curr, _temp349.last_plus_one -
_temp349.curr, _temp349.curr, _temp350.last_plus_one - _temp350.curr, _temp350.curr);}));}}}}}}};
_pop_region(& _temp322);} goto _LL141; _LL167: if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp209) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp211->tvs)){ Cyc_Tcutil_terr( p->loc,({ char* _temp351="wrong number of existential type variables in pattern";
struct _tagged_string _temp352; _temp352.curr= _temp351; _temp352.base= _temp351;
_temp352.last_plus_one= _temp351 + 54; _temp352;}));}{ struct _RegionHandle
_temp353= _new_region(); struct _RegionHandle* rgn=& _temp353; _push_region( rgn);{
struct Cyc_List_List* _temp354= 0; goto _LL355; _LL355:{ struct Cyc_List_List* t=
_temp209; for( 0; t != 0; t=(( struct Cyc_List_List*) _check_null( t))->tl){
struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( t))->hd;{ void* _temp356=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void*
_temp366; void* _temp368; struct Cyc_Absyn_Conref* _temp370; _LL358: if((
unsigned int) _temp356 > 1u?*(( int*) _temp356) == Cyc_Absyn_Eq_constr: 0){
_LL367: _temp366=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp356)->f1;
if( _temp366 ==( void*) Cyc_Absyn_BoxKind){ goto _LL359;} else{ goto _LL360;}}
else{ goto _LL360;} _LL360: if(( unsigned int) _temp356 > 1u?*(( int*) _temp356)
== Cyc_Absyn_Eq_constr: 0){ _LL369: _temp368=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp356)->f1; goto _LL361;} else{ goto _LL362;} _LL362: if( _temp356 ==( void*)
Cyc_Absyn_No_constr){ goto _LL363;} else{ goto _LL364;} _LL364: if((
unsigned int) _temp356 > 1u?*(( int*) _temp356) == Cyc_Absyn_Forward_constr: 0){
_LL371: _temp370=(( struct Cyc_Absyn_Forward_constr_struct*) _temp356)->f1; goto
_LL365;} else{ goto _LL357;} _LL359: goto _LL357; _LL361: Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp372= Cyc_Absynpp_qvar2string( _temp213->name); struct
_tagged_string _temp373=* tv->name; struct _tagged_string _temp374= Cyc_Absynpp_kind2string(
_temp368); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp372.last_plus_one - _temp372.curr, _temp372.curr, _temp373.last_plus_one -
_temp373.curr, _temp373.curr, _temp374.last_plus_one - _temp374.curr, _temp374.curr);}));
goto _LL357; _LL363:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp375=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp375[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp376; _temp376.tag= Cyc_Absyn_Eq_constr;
_temp376.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp376;}); _temp375;})));
goto _LL357; _LL365:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
char* _temp377="forward after compress in conref"; struct _tagged_string
_temp378; _temp378.curr= _temp377; _temp378.base= _temp377; _temp378.last_plus_one=
_temp377 + 33; _temp378;}))); _LL357:;} _temp354=({ struct Cyc_List_List*
_temp379=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp379->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp380=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp380[ 0]=({ struct Cyc_Absyn_VarType_struct _temp381; _temp381.tag= Cyc_Absyn_VarType;
_temp381.f1= tv; _temp381;}); _temp380;})); _temp379->tl= _temp354; _temp379;});}}
_temp354=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp354); tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp209);{ struct Cyc_Tcenv_Tenv*
te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp209); struct Cyc_List_List* tqts=
_temp211->typs; struct Cyc_List_List* _temp382=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp213->tvs); goto _LL383; _LL383: { struct Cyc_List_List* _temp384=((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp211->tvs, _temp354); goto _LL385; _LL385: { struct Cyc_List_List* _temp386=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp382); goto _LL387;
_LL387: { struct Cyc_List_List* _temp388=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, _temp384); goto _LL389; _LL389: { struct Cyc_List_List*
_temp390=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp382, _temp384); goto
_LL391; _LL391: if( tqts == 0){ t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp392=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp392[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp393; _temp393.tag= Cyc_Absyn_TunionType;
_temp393.f1=({ struct Cyc_Absyn_TunionInfo _temp394; _temp394.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp395=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp395[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp396; _temp396.tag= Cyc_Absyn_KnownTunion;
_temp396.f1= _temp213; _temp396;}); _temp395;})); _temp394.targs= _temp386;
_temp394.rgn=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp394;});
_temp393;}); _temp392;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp397=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp397[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp398; _temp398.tag= Cyc_Absyn_TunionFieldType; _temp398.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp399; _temp399.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp400=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp400[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp401; _temp401.tag= Cyc_Absyn_KnownTunionfield; _temp401.f1= _temp213;
_temp401.f2= _temp211; _temp401;}); _temp400;})); _temp399.targs= _temp386;
_temp399;}); _temp398;}); _temp397;});} if( topt != 0? tqts == 0: 0){ void*
_temp402= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_TunionFieldInfo
_temp410; struct Cyc_Absyn_TunionInfo _temp412; void* _temp414; struct Cyc_List_List*
_temp416; void* _temp418; _LL404: if(( unsigned int) _temp402 > 4u?*(( int*)
_temp402) == Cyc_Absyn_TunionFieldType: 0){ _LL411: _temp410=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp402)->f1; goto _LL405;} else{ goto _LL406;} _LL406: if(( unsigned int)
_temp402 > 4u?*(( int*) _temp402) == Cyc_Absyn_TunionType: 0){ _LL413: _temp412=((
struct Cyc_Absyn_TunionType_struct*) _temp402)->f1; _LL419: _temp418=( void*)
_temp412.tunion_info; goto _LL417; _LL417: _temp416= _temp412.targs; goto _LL415;
_LL415: _temp414=( void*) _temp412.rgn; goto _LL407;} else{ goto _LL408;} _LL408:
goto _LL409; _LL405: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp420=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp420[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp421; _temp421.tag= Cyc_Absyn_TunionFieldType; _temp421.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp422; _temp422.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp423=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp423[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp424; _temp424.tag= Cyc_Absyn_KnownTunionfield; _temp424.f1= _temp213;
_temp424.f2= _temp211; _temp424;}); _temp423;})); _temp422.targs= _temp386;
_temp422;}); _temp421;}); _temp420;}); goto _LL403; _LL407: { struct Cyc_List_List*
_temp425= _temp386; goto _LL426; _LL426: for( 0; _temp425 != 0? _temp416 != 0: 0;(
_temp425=(( struct Cyc_List_List*) _check_null( _temp425))->tl, _temp416=((
struct Cyc_List_List*) _check_null( _temp416))->tl)){ Cyc_Tcutil_unify(( void*)((
struct Cyc_List_List*) _check_null( _temp425))->hd,( void*)(( struct Cyc_List_List*)
_check_null( _temp416))->hd);} goto _LL403;} _LL409: goto _LL403; _LL403:;} for(
0; _temp207 != 0? tqts != 0: 0;( _temp207=(( struct Cyc_List_List*) _check_null(
_temp207))->tl, tqts=(( struct Cyc_List_List*) _check_null( tqts))->tl)){ struct
Cyc_Absyn_Pat* _temp427=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp207))->hd; goto _LL428; _LL428: { void* _temp429= Cyc_Tcutil_rsubstitute(
rgn, _temp390,(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( tqts))->hd)).f2);
goto _LL430; _LL430: { struct Cyc_List_List* _temp433; struct Cyc_List_List*
_temp435; struct _tuple5 _temp431= Cyc_Tcpat_tcPatRec( te2, _temp427,( void**)&
_temp429, region_opt); _LL436: _temp435= _temp431.f1; goto _LL434; _LL434:
_temp433= _temp431.f2; goto _LL432; _LL432: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp435); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp433); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp427->topt))->v, _temp429)){ Cyc_Tcutil_terr(
_temp427->loc,({ struct _tagged_string _temp437= Cyc_Absynpp_qvar2string(
_temp213->name); struct _tagged_string _temp438= Cyc_Absynpp_typ2string(
_temp429); struct _tagged_string _temp439= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp427->topt))->v); xprintf("%.*s expects argument type %.*s, not %.*s",
_temp437.last_plus_one - _temp437.curr, _temp437.curr, _temp438.last_plus_one -
_temp438.curr, _temp438.curr, _temp439.last_plus_one - _temp439.curr, _temp439.curr);}));}}}}
if( _temp207 != 0){ Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp440=
Cyc_Absynpp_qvar2string( _temp213->name); xprintf("too many arguments for tunion constructor %.*s",
_temp440.last_plus_one - _temp440.curr, _temp440.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp441= Cyc_Absynpp_qvar2string( _temp213->name);
xprintf("too few arguments for tunion constructor %.*s", _temp441.last_plus_one
- _temp441.curr, _temp441.curr);}));}}}}}}}; _pop_region(& _temp353);} goto
_LL141; _LL169: goto _LL171; _LL171: goto _LL173; _LL173: t= Cyc_Absyn_wildtyp();
goto _LL141; _LL141:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt* _temp442=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp442->v=(
void*) t; _temp442;}); return({ struct _tuple5 _temp443; _temp443.f1= tv_result;
_temp443.f2= v_result; _temp443;});}} struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt){ struct _tuple5
_temp444= Cyc_Tcpat_tcPatRec( te, p, topt, region_opt); goto _LL445; _LL445:{
struct _RegionHandle _temp446= _new_region(); struct _RegionHandle* r=& _temp446;
_push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_string*(* f)( struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Tcpat_get_name, _temp444.f2), p->loc,({
char* _temp447="pattern contains a repeated variable"; struct _tagged_string
_temp448; _temp448.curr= _temp447; _temp448.base= _temp447; _temp448.last_plus_one=
_temp447 + 37; _temp448;}));; _pop_region(& _temp446);} return _temp444;} void
Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p){
void* _temp449=( void*) p->r; struct Cyc_Absyn_Pat* _temp461; struct Cyc_List_List*
_temp463; struct Cyc_List_List* _temp465; struct Cyc_Absyn_Tunionfield* _temp467;
struct Cyc_Absyn_Tuniondecl* _temp469; struct Cyc_List_List* _temp471; struct
Cyc_List_List* _temp473; struct Cyc_Core_Opt* _temp475; struct Cyc_Absyn_Structdecl*
_temp477; struct Cyc_List_List* _temp479; _LL451: if(( unsigned int) _temp449 >
2u?*(( int*) _temp449) == Cyc_Absyn_Pointer_p: 0){ _LL462: _temp461=(( struct
Cyc_Absyn_Pointer_p_struct*) _temp449)->f1; goto _LL452;} else{ goto _LL453;}
_LL453: if(( unsigned int) _temp449 > 2u?*(( int*) _temp449) == Cyc_Absyn_Tunion_p:
0){ _LL470: _temp469=(( struct Cyc_Absyn_Tunion_p_struct*) _temp449)->f1; goto
_LL468; _LL468: _temp467=(( struct Cyc_Absyn_Tunion_p_struct*) _temp449)->f2;
goto _LL466; _LL466: _temp465=(( struct Cyc_Absyn_Tunion_p_struct*) _temp449)->f3;
goto _LL464; _LL464: _temp463=(( struct Cyc_Absyn_Tunion_p_struct*) _temp449)->f4;
goto _LL454;} else{ goto _LL455;} _LL455: if(( unsigned int) _temp449 > 2u?*((
int*) _temp449) == Cyc_Absyn_Struct_p: 0){ _LL478: _temp477=(( struct Cyc_Absyn_Struct_p_struct*)
_temp449)->f1; goto _LL476; _LL476: _temp475=(( struct Cyc_Absyn_Struct_p_struct*)
_temp449)->f2; goto _LL474; _LL474: _temp473=(( struct Cyc_Absyn_Struct_p_struct*)
_temp449)->f3; goto _LL472; _LL472: _temp471=(( struct Cyc_Absyn_Struct_p_struct*)
_temp449)->f4; goto _LL456;} else{ goto _LL457;} _LL457: if(( unsigned int)
_temp449 > 2u?*(( int*) _temp449) == Cyc_Absyn_Tuple_p: 0){ _LL480: _temp479=((
struct Cyc_Absyn_Tuple_p_struct*) _temp449)->f1; goto _LL458;} else{ goto _LL459;}
_LL459: goto _LL460; _LL452: Cyc_Tcpat_check_pat_regions( te, _temp461);{ void*
_temp481=( void*)(( struct Cyc_Core_Opt*) _check_null( p->topt))->v; struct Cyc_Absyn_PtrInfo
_temp489; struct Cyc_Absyn_Conref* _temp491; struct Cyc_Absyn_Tqual _temp493;
struct Cyc_Absyn_Conref* _temp495; void* _temp497; void* _temp499; struct Cyc_Absyn_TunionInfo
_temp501; void* _temp503; struct Cyc_List_List* _temp505; void* _temp507; _LL483:
if(( unsigned int) _temp481 > 4u?*(( int*) _temp481) == Cyc_Absyn_PointerType: 0){
_LL490: _temp489=(( struct Cyc_Absyn_PointerType_struct*) _temp481)->f1; _LL500:
_temp499=( void*) _temp489.elt_typ; goto _LL498; _LL498: _temp497=( void*)
_temp489.rgn_typ; goto _LL496; _LL496: _temp495= _temp489.nullable; goto _LL494;
_LL494: _temp493= _temp489.tq; goto _LL492; _LL492: _temp491= _temp489.bounds;
goto _LL484;} else{ goto _LL485;} _LL485: if(( unsigned int) _temp481 > 4u?*((
int*) _temp481) == Cyc_Absyn_TunionType: 0){ _LL502: _temp501=(( struct Cyc_Absyn_TunionType_struct*)
_temp481)->f1; _LL508: _temp507=( void*) _temp501.tunion_info; goto _LL506;
_LL506: _temp505= _temp501.targs; goto _LL504; _LL504: _temp503=( void*)
_temp501.rgn; goto _LL486;} else{ goto _LL487;} _LL487: goto _LL488; _LL484:
_temp503= _temp497; goto _LL486; _LL486: Cyc_Tcenv_check_rgn_accessible( te, p->loc,
_temp503); return; _LL488:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp509=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp509[ 0]=({ struct Cyc_Core_Impossible_struct _temp510; _temp510.tag= Cyc_Core_Impossible;
_temp510.f1=({ char* _temp511="check_pat_regions: bad pointer type"; struct
_tagged_string _temp512; _temp512.curr= _temp511; _temp512.base= _temp511;
_temp512.last_plus_one= _temp511 + 36; _temp512;}); _temp510;}); _temp509;}));
_LL482:;} _LL454: for( 0; _temp463 != 0; _temp463=(( struct Cyc_List_List*)
_check_null( _temp463))->tl){ Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp463))->hd);}{ void* _temp513=( void*)((
struct Cyc_Core_Opt*) _check_null( p->topt))->v; struct Cyc_Absyn_TunionInfo
_temp521; void* _temp523; struct Cyc_List_List* _temp525; void* _temp527; struct
Cyc_Absyn_TunionFieldInfo _temp529; _LL515: if(( unsigned int) _temp513 > 4u?*((
int*) _temp513) == Cyc_Absyn_TunionType: 0){ _LL522: _temp521=(( struct Cyc_Absyn_TunionType_struct*)
_temp513)->f1; _LL528: _temp527=( void*) _temp521.tunion_info; goto _LL526;
_LL526: _temp525= _temp521.targs; goto _LL524; _LL524: _temp523=( void*)
_temp521.rgn; goto _LL516;} else{ goto _LL517;} _LL517: if(( unsigned int)
_temp513 > 4u?*(( int*) _temp513) == Cyc_Absyn_TunionFieldType: 0){ _LL530:
_temp529=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp513)->f1; goto _LL518;}
else{ goto _LL519;} _LL519: goto _LL520; _LL516: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp523); return; _LL518: return; _LL520:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp531=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp531[ 0]=({ struct
Cyc_Core_Impossible_struct _temp532; _temp532.tag= Cyc_Core_Impossible; _temp532.f1=({
char* _temp533="check_pat_regions: bad tunion type"; struct _tagged_string
_temp534; _temp534.curr= _temp533; _temp534.base= _temp533; _temp534.last_plus_one=
_temp533 + 35; _temp534;}); _temp532;}); _temp531;})); _LL514:;} _LL456:
_temp479=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(* f)( struct _tuple6*),
struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)( struct
_tuple6*)) Cyc_Core_snd, _temp471); goto _LL458; _LL458: for( 0; _temp479 != 0;
_temp479=(( struct Cyc_List_List*) _check_null( _temp479))->tl){ Cyc_Tcpat_check_pat_regions(
te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp479))->hd);}
return; _LL460: return; _LL450:;} static const int Cyc_Tcpat_Name_v= 0; struct
Cyc_Tcpat_Name_v_struct{ int tag; struct _tagged_string f1; } ; static const int
Cyc_Tcpat_Int_v= 1; struct Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; typedef
void* Cyc_Tcpat_name_value_t; struct Cyc_Tcpat_Con_s{ void* name; int arity;
struct Cyc_Core_Opt* span; } ; typedef struct Cyc_Tcpat_Con_s* Cyc_Tcpat_con_t;
static const int Cyc_Tcpat_Any= 0; static const int Cyc_Tcpat_Con= 0; struct Cyc_Tcpat_Con_struct{
int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; typedef void*
Cyc_Tcpat_simple_pat_t; static int Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s*
c1, struct Cyc_Tcpat_Con_s* c2){ void* _temp535=( void*) c1->name; struct
_tagged_string _temp541; int _temp543; _LL537: if(*(( int*) _temp535) == Cyc_Tcpat_Name_v){
_LL542: _temp541=(( struct Cyc_Tcpat_Name_v_struct*) _temp535)->f1; goto _LL538;}
else{ goto _LL539;} _LL539: if(*(( int*) _temp535) == Cyc_Tcpat_Int_v){ _LL544:
_temp543=(( struct Cyc_Tcpat_Int_v_struct*) _temp535)->f1; goto _LL540;} else{
goto _LL536;} _LL538: { void* _temp545=( void*) c2->name; struct _tagged_string
_temp551; int _temp553; _LL547: if(*(( int*) _temp545) == Cyc_Tcpat_Name_v){
_LL552: _temp551=(( struct Cyc_Tcpat_Name_v_struct*) _temp545)->f1; goto _LL548;}
else{ goto _LL549;} _LL549: if(*(( int*) _temp545) == Cyc_Tcpat_Int_v){ _LL554:
_temp553=(( struct Cyc_Tcpat_Int_v_struct*) _temp545)->f1; goto _LL550;} else{
goto _LL546;} _LL548: return Cyc_String_zstrcmp( _temp541, _temp551); _LL550:
return - 1; _LL546:;} _LL540: { void* _temp555=( void*) c2->name; struct
_tagged_string _temp561; int _temp563; _LL557: if(*(( int*) _temp555) == Cyc_Tcpat_Name_v){
_LL562: _temp561=(( struct Cyc_Tcpat_Name_v_struct*) _temp555)->f1; goto _LL558;}
else{ goto _LL559;} _LL559: if(*(( int*) _temp555) == Cyc_Tcpat_Int_v){ _LL564:
_temp563=(( struct Cyc_Tcpat_Int_v_struct*) _temp555)->f1; goto _LL560;} else{
goto _LL556;} _LL558: return 1; _LL560: return _temp543 - _temp563; _LL556:;}
_LL536:;} static struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct
Cyc_Set_Set*(*)( int(* comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*)))
Cyc_Set_empty)( Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(
void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={( void*) 2}; static
struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;
static char _temp568[ 5u]="null"; static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={
0u,{ _temp568, _temp568, _temp568 + 5u}}; static char _temp572[ 4u]="ptr";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={ 0u,{ _temp572,
_temp572, _temp572 + 4u}}; static char _temp576[ 6u]="tuple"; static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_tuple_name_value={ 0u,{ _temp576, _temp576, _temp576 + 6u}}; static
struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={( void*)(( void*)& Cyc_Tcpat_null_name_value),
0,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={(
void*)(( void*)& Cyc_Tcpat_ptr_name_value), 1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={( void*)(( void*)& Cyc_Tcpat_ptr_name_value),
1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt}; static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=&
Cyc_Tcpat_null_ptr_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp577=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp577->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp578=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp578[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp579; _temp579.tag= Cyc_Tcpat_Int_v;
_temp579.f1= i; _temp579;}); _temp578;})); _temp577->arity= 0; _temp577->span= 0;
_temp577;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_string f){ return({ struct Cyc_Tcpat_Con_s* _temp580=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp580->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp581=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp581[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp582; _temp582.tag= Cyc_Tcpat_Name_v; _temp582.f1= f; _temp582;}); _temp581;}));
_temp580->arity= 0; _temp580->span= 0; _temp580;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( char c){ return({ struct Cyc_Tcpat_Con_s* _temp583=( struct
Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp583->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp585=( struct Cyc_Tcpat_Int_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp585[ 0]=({ struct Cyc_Tcpat_Int_v_struct
_temp586; _temp586.tag= Cyc_Tcpat_Int_v; _temp586.f1=( int) c; _temp586;});
_temp585;})); _temp583->arity= 0; _temp583->span=({ struct Cyc_Core_Opt*
_temp584=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp584->v=( void*) 256; _temp584;}); _temp583;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s* _temp587=( struct
Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp587->name=(
void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp587->arity= i; _temp587->span=
Cyc_Tcpat_one_opt_ptr; _temp587;});} static void* Cyc_Tcpat_null_pat(){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp588=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp588[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp589; _temp589.tag= Cyc_Tcpat_Con; _temp589.f1= Cyc_Tcpat_null_con; _temp589.f2=
0; _temp589;}); _temp588;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp590=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp590[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp591; _temp591.tag= Cyc_Tcpat_Con; _temp591.f1= Cyc_Tcpat_int_con( i);
_temp591.f2= 0; _temp591;}); _temp590;});} static void* Cyc_Tcpat_char_pat( char
c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp592=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp592[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp593; _temp593.tag= Cyc_Tcpat_Con; _temp593.f1= Cyc_Tcpat_char_con( c);
_temp593.f2= 0; _temp593;}); _temp592;});} static void* Cyc_Tcpat_float_pat(
struct _tagged_string f){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp594=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp594[ 0]=({ struct Cyc_Tcpat_Con_struct _temp595; _temp595.tag= Cyc_Tcpat_Con;
_temp595.f1= Cyc_Tcpat_float_con( f); _temp595.f2= 0; _temp595;}); _temp594;});}
static void* Cyc_Tcpat_null_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp596=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp596[ 0]=({ struct Cyc_Tcpat_Con_struct _temp597; _temp597.tag= Cyc_Tcpat_Con;
_temp597.f1= Cyc_Tcpat_null_ptr_con; _temp597.f2=({ struct Cyc_List_List*
_temp598=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp598->hd=( void*) p; _temp598->tl= 0; _temp598;}); _temp597;}); _temp596;});}
static void* Cyc_Tcpat_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp599=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp599[ 0]=({ struct Cyc_Tcpat_Con_struct _temp600; _temp600.tag= Cyc_Tcpat_Con;
_temp600.f1= Cyc_Tcpat_ptr_con; _temp600.f2=({ struct Cyc_List_List* _temp601=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp601->hd=(
void*) p; _temp601->tl= 0; _temp601;}); _temp600;}); _temp599;});} static void*
Cyc_Tcpat_tuple_pat( struct Cyc_List_List* ss){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp602=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp602[ 0]=({ struct Cyc_Tcpat_Con_struct _temp603; _temp603.tag= Cyc_Tcpat_Con;
_temp603.f1= Cyc_Tcpat_tuple_con((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ss)); _temp603.f2= ss; _temp603;}); _temp602;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_string con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List*
ps){ struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp606=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp606->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp607=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp607[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp608; _temp608.tag= Cyc_Tcpat_Name_v; _temp608.f1= con_name; _temp608;});
_temp607;})); _temp606->arity=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ps); _temp606->span= span; _temp606;}); return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp604=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp604[ 0]=({ struct Cyc_Tcpat_Con_struct _temp605; _temp605.tag= Cyc_Tcpat_Con;
_temp605.f1= c; _temp605.f2= ps; _temp605;}); _temp604;});} static void* Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat* p){ void* s;{ void* _temp609=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp643; int _temp645; void* _temp647; char _temp649; struct _tagged_string
_temp651; struct Cyc_Absyn_Vardecl* _temp653; struct Cyc_Absyn_Pat* _temp655;
struct Cyc_List_List* _temp657; struct Cyc_List_List* _temp659; struct Cyc_Absyn_Tunionfield*
_temp661; struct Cyc_Absyn_Tuniondecl* _temp663; struct Cyc_List_List* _temp665;
struct Cyc_List_List* _temp667; struct Cyc_Absyn_Tunionfield* _temp669; struct
Cyc_Absyn_Tuniondecl* _temp671; struct Cyc_List_List* _temp673; struct Cyc_List_List*
_temp675; struct Cyc_List_List* _temp677; struct Cyc_Core_Opt* _temp679; struct
Cyc_Absyn_Structdecl* _temp681; struct Cyc_Absyn_Enumfield* _temp683; struct Cyc_Absyn_Enumdecl*
_temp685; struct _tuple1* _temp687; struct Cyc_List_List* _temp689; struct Cyc_List_List*
_temp691; struct _tuple1* _temp693; struct Cyc_List_List* _temp695; struct Cyc_List_List*
_temp697; struct _tuple1* _temp699; _LL611: if( _temp609 ==( void*) Cyc_Absyn_Wild_p){
goto _LL612;} else{ goto _LL613;} _LL613: if(( unsigned int) _temp609 > 2u?*((
int*) _temp609) == Cyc_Absyn_Var_p: 0){ _LL644: _temp643=(( struct Cyc_Absyn_Var_p_struct*)
_temp609)->f1; goto _LL614;} else{ goto _LL615;} _LL615: if( _temp609 ==( void*)
Cyc_Absyn_Null_p){ goto _LL616;} else{ goto _LL617;} _LL617: if(( unsigned int)
_temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Int_p: 0){ _LL648: _temp647=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp609)->f1; goto _LL646; _LL646:
_temp645=(( struct Cyc_Absyn_Int_p_struct*) _temp609)->f2; goto _LL618;} else{
goto _LL619;} _LL619: if(( unsigned int) _temp609 > 2u?*(( int*) _temp609) ==
Cyc_Absyn_Char_p: 0){ _LL650: _temp649=(( struct Cyc_Absyn_Char_p_struct*)
_temp609)->f1; goto _LL620;} else{ goto _LL621;} _LL621: if(( unsigned int)
_temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Float_p: 0){ _LL652: _temp651=((
struct Cyc_Absyn_Float_p_struct*) _temp609)->f1; goto _LL622;} else{ goto _LL623;}
_LL623: if(( unsigned int) _temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Reference_p:
0){ _LL654: _temp653=(( struct Cyc_Absyn_Reference_p_struct*) _temp609)->f1;
goto _LL624;} else{ goto _LL625;} _LL625: if(( unsigned int) _temp609 > 2u?*((
int*) _temp609) == Cyc_Absyn_Pointer_p: 0){ _LL656: _temp655=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp609)->f1; goto _LL626;} else{ goto _LL627;} _LL627: if(( unsigned int)
_temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Tunion_p: 0){ _LL664: _temp663=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f1; goto _LL662; _LL662: _temp661=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f2; goto _LL660; _LL660: _temp659=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f3; goto _LL658; _LL658: _temp657=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f4; if( _temp657 == 0){ goto
_LL628;} else{ goto _LL629;}} else{ goto _LL629;} _LL629: if(( unsigned int)
_temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Tunion_p: 0){ _LL672: _temp671=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f1; goto _LL670; _LL670: _temp669=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f2; goto _LL668; _LL668: _temp667=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f3; goto _LL666; _LL666: _temp665=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f4; goto _LL630;} else{ goto
_LL631;} _LL631: if(( unsigned int) _temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Tuple_p:
0){ _LL674: _temp673=(( struct Cyc_Absyn_Tuple_p_struct*) _temp609)->f1; goto
_LL632;} else{ goto _LL633;} _LL633: if(( unsigned int) _temp609 > 2u?*(( int*)
_temp609) == Cyc_Absyn_Struct_p: 0){ _LL682: _temp681=(( struct Cyc_Absyn_Struct_p_struct*)
_temp609)->f1; goto _LL680; _LL680: _temp679=(( struct Cyc_Absyn_Struct_p_struct*)
_temp609)->f2; goto _LL678; _LL678: _temp677=(( struct Cyc_Absyn_Struct_p_struct*)
_temp609)->f3; goto _LL676; _LL676: _temp675=(( struct Cyc_Absyn_Struct_p_struct*)
_temp609)->f4; goto _LL634;} else{ goto _LL635;} _LL635: if(( unsigned int)
_temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Enum_p: 0){ _LL686: _temp685=((
struct Cyc_Absyn_Enum_p_struct*) _temp609)->f1; goto _LL684; _LL684: _temp683=((
struct Cyc_Absyn_Enum_p_struct*) _temp609)->f2; goto _LL636;} else{ goto _LL637;}
_LL637: if(( unsigned int) _temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_UnknownId_p:
0){ _LL688: _temp687=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp609)->f1;
goto _LL638;} else{ goto _LL639;} _LL639: if(( unsigned int) _temp609 > 2u?*((
int*) _temp609) == Cyc_Absyn_UnknownCall_p: 0){ _LL694: _temp693=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp609)->f1; goto _LL692; _LL692: _temp691=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp609)->f2; goto _LL690; _LL690: _temp689=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp609)->f3; goto _LL640;} else{ goto _LL641;} _LL641: if(( unsigned int)
_temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_UnknownFields_p: 0){ _LL700:
_temp699=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp609)->f1; goto _LL698;
_LL698: _temp697=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp609)->f2;
goto _LL696; _LL696: _temp695=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp609)->f3; goto _LL642;} else{ goto _LL610;} _LL612: s=( void*) Cyc_Tcpat_Any;
goto _LL610; _LL614: s=( void*) Cyc_Tcpat_Any; goto _LL610; _LL616: s= Cyc_Tcpat_null_pat();
goto _LL610; _LL618: s= Cyc_Tcpat_int_pat( _temp645); goto _LL610; _LL620: s=
Cyc_Tcpat_char_pat( _temp649); goto _LL610; _LL622: s= Cyc_Tcpat_float_pat(
_temp651); goto _LL610; _LL624: s=( void*) Cyc_Tcpat_Any; goto _LL610; _LL626:{
void* _temp701= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
p->topt))->v); struct Cyc_Absyn_PtrInfo _temp709; struct Cyc_Absyn_Conref*
_temp711; struct Cyc_Absyn_Tqual _temp713; struct Cyc_Absyn_Conref* _temp715;
void* _temp717; void* _temp719; struct Cyc_Absyn_TunionInfo _temp721; _LL703:
if(( unsigned int) _temp701 > 4u?*(( int*) _temp701) == Cyc_Absyn_PointerType: 0){
_LL710: _temp709=(( struct Cyc_Absyn_PointerType_struct*) _temp701)->f1; _LL720:
_temp719=( void*) _temp709.elt_typ; goto _LL718; _LL718: _temp717=( void*)
_temp709.rgn_typ; goto _LL716; _LL716: _temp715= _temp709.nullable; goto _LL714;
_LL714: _temp713= _temp709.tq; goto _LL712; _LL712: _temp711= _temp709.bounds;
goto _LL704;} else{ goto _LL705;} _LL705: if(( unsigned int) _temp701 > 4u?*((
int*) _temp701) == Cyc_Absyn_TunionType: 0){ _LL722: _temp721=(( struct Cyc_Absyn_TunionType_struct*)
_temp701)->f1; goto _LL706;} else{ goto _LL707;} _LL707: goto _LL708; _LL704: {
int is_nullable= 0; int still_working= 1; while( still_working) { void* _temp723=(
void*) _temp715->v; struct Cyc_Absyn_Conref* _temp731; int _temp733; _LL725: if((
unsigned int) _temp723 > 1u?*(( int*) _temp723) == Cyc_Absyn_Forward_constr: 0){
_LL732: _temp731=(( struct Cyc_Absyn_Forward_constr_struct*) _temp723)->f1; goto
_LL726;} else{ goto _LL727;} _LL727: if( _temp723 ==( void*) Cyc_Absyn_No_constr){
goto _LL728;} else{ goto _LL729;} _LL729: if(( unsigned int) _temp723 > 1u?*((
int*) _temp723) == Cyc_Absyn_Eq_constr: 0){ _LL734: _temp733=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp723)->f1; goto _LL730;} else{ goto _LL724;} _LL726:( void*)( _temp715->v=(
void*)(( void*) _temp731->v)); continue; _LL728:( void*)( _temp715->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp735=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp735[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp736; _temp736.tag= Cyc_Absyn_Eq_constr; _temp736.f1=(
void*) 0; _temp736;}); _temp735;}))); is_nullable= 0; still_working= 0; goto
_LL724; _LL730: is_nullable=( int) _temp733; still_working= 0; goto _LL724;
_LL724:;}{ void* ss= Cyc_Tcpat_compile_pat( _temp655); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL702;}} _LL706:{ void* _temp737=(
void*) _temp655->r; struct Cyc_List_List* _temp743; struct Cyc_List_List*
_temp745; struct Cyc_Absyn_Tunionfield* _temp747; struct Cyc_Absyn_Tuniondecl*
_temp749; _LL739: if(( unsigned int) _temp737 > 2u?*(( int*) _temp737) == Cyc_Absyn_Tunion_p:
0){ _LL750: _temp749=(( struct Cyc_Absyn_Tunion_p_struct*) _temp737)->f1; goto
_LL748; _LL748: _temp747=(( struct Cyc_Absyn_Tunion_p_struct*) _temp737)->f2;
goto _LL746; _LL746: _temp745=(( struct Cyc_Absyn_Tunion_p_struct*) _temp737)->f3;
goto _LL744; _LL744: _temp743=(( struct Cyc_Absyn_Tunion_p_struct*) _temp737)->f4;
goto _LL740;} else{ goto _LL741;} _LL741: goto _LL742; _LL740: { struct Cyc_Core_Opt*
span; if( _temp749->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt*
_temp751=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp751->v=( void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp749->fields))->v);
_temp751;});} s= Cyc_Tcpat_con_pat(*(* _temp747->name).f2, span,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp743)); goto _LL738;} _LL742: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
char* _temp752="non-[x]tunion pattern has tunion type"; struct _tagged_string
_temp753; _temp753.curr= _temp752; _temp753.base= _temp752; _temp753.last_plus_one=
_temp752 + 38; _temp753;})); goto _LL738; _LL738:;} goto _LL702; _LL708: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char* _temp754="pointer pattern does not have pointer type!";
struct _tagged_string _temp755; _temp755.curr= _temp754; _temp755.base= _temp754;
_temp755.last_plus_one= _temp754 + 44; _temp755;})); goto _LL702; _LL702:;} goto
_LL610; _LL628: { struct Cyc_Core_Opt* span;{ void* _temp756= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( p->topt))->v); struct Cyc_Absyn_TunionInfo
_temp764; struct Cyc_Absyn_TunionFieldInfo _temp766; _LL758: if(( unsigned int)
_temp756 > 4u?*(( int*) _temp756) == Cyc_Absyn_TunionType: 0){ _LL765: _temp764=((
struct Cyc_Absyn_TunionType_struct*) _temp756)->f1; goto _LL759;} else{ goto
_LL760;} _LL760: if(( unsigned int) _temp756 > 4u?*(( int*) _temp756) == Cyc_Absyn_TunionFieldType:
0){ _LL767: _temp766=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp756)->f1;
goto _LL761;} else{ goto _LL762;} _LL762: goto _LL763; _LL759: if( _temp663->is_xtunion){
span= 0;} else{ span=({ struct Cyc_Core_Opt* _temp768=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp768->v=( void*)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp663->fields))->v); _temp768;});} goto _LL757; _LL761: span=({
struct Cyc_Core_Opt* _temp769=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp769->v=( void*) 1; _temp769;}); goto _LL757; _LL763:
span=(( struct Cyc_Core_Opt*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
char* _temp770="void tunion pattern has bad type"; struct _tagged_string
_temp771; _temp771.curr= _temp770; _temp771.base= _temp770; _temp771.last_plus_one=
_temp770 + 33; _temp771;})); goto _LL757; _LL757:;} s= Cyc_Tcpat_con_pat(*(*
_temp661->name).f2, span, 0); goto _LL610;} _LL630: _temp673= _temp665; goto
_LL632; _LL632: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp673)); goto _LL610; _LL634: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp681->fields))->v;
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fields))->hd)->name,({ char* _temp797=""; struct _tagged_string
_temp798; _temp798.curr= _temp797; _temp798.base= _temp797; _temp798.last_plus_one=
_temp797 + 1; _temp798;})) == 0;{ struct Cyc_List_List* dlps0= _temp675; for( 0;
! found? dlps0 != 0: 0; dlps0=(( struct Cyc_List_List*) _check_null( dlps0))->tl){
struct _tuple6 _temp774; struct Cyc_Absyn_Pat* _temp775; struct Cyc_List_List*
_temp777; struct _tuple6* _temp772=( struct _tuple6*)(( struct Cyc_List_List*)
_check_null( dlps0))->hd; _temp774=* _temp772; _LL778: _temp777= _temp774.f1;
goto _LL776; _LL776: _temp775= _temp774.f2; goto _LL773; _LL773: { struct Cyc_List_List*
_temp779= _temp777; struct Cyc_List_List _temp785; struct Cyc_List_List*
_temp786; void* _temp788; struct _tagged_string* _temp790; _LL781: if( _temp779
== 0){ goto _LL783;} else{ _temp785=* _temp779; _LL789: _temp788=( void*)
_temp785.hd; if(*(( int*) _temp788) == Cyc_Absyn_FieldName){ _LL791: _temp790=((
struct Cyc_Absyn_FieldName_struct*) _temp788)->f1; goto _LL787;} else{ goto
_LL783;} _LL787: _temp786= _temp785.tl; if( _temp786 == 0){ goto _LL782;} else{
goto _LL783;}} _LL783: goto _LL784; _LL782: if( Cyc_String_zstrptrcmp( _temp790,((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fields))->hd)->name)
== 0){ ps=({ struct Cyc_List_List* _temp792=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp792->hd=( void*) Cyc_Tcpat_compile_pat(
_temp775); _temp792->tl= ps; _temp792;}); found= 1;} goto _LL780; _LL784:(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(({ char* _temp793="bad designator(s)";
struct _tagged_string _temp794; _temp794.curr= _temp793; _temp794.base= _temp793;
_temp794.last_plus_one= _temp793 + 18; _temp794;})); goto _LL780; _LL780:;}}}
if( ! found){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char*
_temp795="bad designator"; struct _tagged_string _temp796; _temp796.curr=
_temp795; _temp796.base= _temp795; _temp796.last_plus_one= _temp795 + 15;
_temp796;}));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL610;} _LL636: { int span=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp685->fields))->v); s= Cyc_Tcpat_con_pat(*(*
_temp683->name).f2,({ struct Cyc_Core_Opt* _temp799=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp799->v=( void*) span;
_temp799;}), 0); goto _LL610;} _LL638: s=(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ char* _temp800="compile_pat: unknownid"; struct
_tagged_string _temp801; _temp801.curr= _temp800; _temp801.base= _temp800;
_temp801.last_plus_one= _temp800 + 23; _temp801;})); goto _LL610; _LL640: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char* _temp802="compile_pat: unknowncall";
struct _tagged_string _temp803; _temp803.curr= _temp802; _temp803.base= _temp802;
_temp803.last_plus_one= _temp802 + 25; _temp803;})); goto _LL610; _LL642: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char* _temp804="compile_pat: unknownfields";
struct _tagged_string _temp805; _temp805.curr= _temp804; _temp805.base= _temp804;
_temp805.last_plus_one= _temp804 + 27; _temp805;})); goto _LL610; _LL610:;}
return s;} typedef struct Cyc_List_List* Cyc_Tcpat_match_t; static const int Cyc_Tcpat_Pos=
0; struct Cyc_Tcpat_Pos_struct{ int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{ int tag;
struct Cyc_Set_Set* f1; } ; typedef void* Cyc_Tcpat_term_desc_t; typedef struct
Cyc_List_List* Cyc_Tcpat_context_t; typedef struct Cyc_List_List* Cyc_Tcpat_access_t;
static const int Cyc_Tcpat_Failure= 0; static const int Cyc_Tcpat_Success= 0;
struct Cyc_Tcpat_Success_struct{ int tag; void* f1; } ; static const int Cyc_Tcpat_IfEq=
1; struct Cyc_Tcpat_IfEq_struct{ int tag; struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s*
f2; void* f3; void* f4; } ; typedef void* Cyc_Tcpat_decision_t; struct _tuple9{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; }
; typedef struct _tuple9* Cyc_Tcpat_work_stack_frame_t; typedef struct Cyc_List_List*
Cyc_Tcpat_work_stack_t; static const int Cyc_Tcpat_Yes= 0; static const int Cyc_Tcpat_No=
1; static const int Cyc_Tcpat_Maybe= 2; typedef void* Cyc_Tcpat_answer_t; static
void* Cyc_Tcpat_add_neg( void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp806=
td; struct Cyc_Set_Set* _temp812; struct Cyc_List_List* _temp814; struct Cyc_Tcpat_Con_s*
_temp816; _LL808: if(*(( int*) _temp806) == Cyc_Tcpat_Neg){ _LL813: _temp812=((
struct Cyc_Tcpat_Neg_struct*) _temp806)->f1; goto _LL809;} else{ goto _LL810;}
_LL810: if(*(( int*) _temp806) == Cyc_Tcpat_Pos){ _LL817: _temp816=(( struct Cyc_Tcpat_Pos_struct*)
_temp806)->f1; goto _LL815; _LL815: _temp814=(( struct Cyc_Tcpat_Pos_struct*)
_temp806)->f2; goto _LL811;} else{ goto _LL807;} _LL809: if((( int(*)( struct
Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp812, c)){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char* _temp818="add_neg called when constructor already in set";
struct _tagged_string _temp819; _temp819.curr= _temp818; _temp819.base= _temp818;
_temp819.last_plus_one= _temp818 + 47; _temp819;}));} if( c->span != 0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp812) + 1 >=( int)(( struct
Cyc_Core_Opt*) _check_null( c->span))->v: 0){(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ char* _temp820="add_neg called when |cs U {c}| >= span(c)";
struct _tagged_string _temp821; _temp821.curr= _temp820; _temp821.base= _temp820;
_temp821.last_plus_one= _temp820 + 42; _temp821;}));} return( void*)({ struct
Cyc_Tcpat_Neg_struct* _temp822=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp822[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp823; _temp823.tag= Cyc_Tcpat_Neg; _temp823.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp812,
c); _temp823;}); _temp822;}); _LL811: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ char* _temp824="add_neg called when td is Positive"; struct
_tagged_string _temp825; _temp825.curr= _temp824; _temp825.base= _temp824;
_temp825.last_plus_one= _temp824 + 35; _temp825;})); _LL807:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp826= td; struct Cyc_List_List*
_temp832; struct Cyc_Tcpat_Con_s* _temp834; struct Cyc_Set_Set* _temp836; _LL828:
if(*(( int*) _temp826) == Cyc_Tcpat_Pos){ _LL835: _temp834=(( struct Cyc_Tcpat_Pos_struct*)
_temp826)->f1; goto _LL833; _LL833: _temp832=(( struct Cyc_Tcpat_Pos_struct*)
_temp826)->f2; goto _LL829;} else{ goto _LL830;} _LL830: if(*(( int*) _temp826)
== Cyc_Tcpat_Neg){ _LL837: _temp836=(( struct Cyc_Tcpat_Neg_struct*) _temp826)->f1;
goto _LL831;} else{ goto _LL827;} _LL829: if( Cyc_Tcpat_compare_con( c, _temp834)
== 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL831:
if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp836, c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)((
struct Cyc_Core_Opt*) _check_null( c->span))->v ==(( int(*)( struct Cyc_Set_Set*
s)) Cyc_Set_cardinality)( _temp836) + 1: 0){ return( void*) Cyc_Tcpat_Yes;}
else{ return( void*) Cyc_Tcpat_Maybe;}} _LL827:;} struct _tuple10{ struct Cyc_Tcpat_Con_s*
f1; struct Cyc_List_List* f2; } ; static struct Cyc_List_List* Cyc_Tcpat_augment(
struct Cyc_List_List* ctxt, void* dsc){ struct Cyc_List_List* _temp838= ctxt;
struct Cyc_List_List _temp844; struct Cyc_List_List* _temp845; struct _tuple10*
_temp847; struct _tuple10 _temp849; struct Cyc_List_List* _temp850; struct Cyc_Tcpat_Con_s*
_temp852; _LL840: if( _temp838 == 0){ goto _LL841;} else{ goto _LL842;} _LL842:
if( _temp838 == 0){ goto _LL839;} else{ _temp844=* _temp838; _LL848: _temp847=(
struct _tuple10*) _temp844.hd; _temp849=* _temp847; _LL853: _temp852= _temp849.f1;
goto _LL851; _LL851: _temp850= _temp849.f2; goto _LL846; _LL846: _temp845=
_temp844.tl; goto _LL843;} _LL841: return 0; _LL843: return({ struct Cyc_List_List*
_temp854=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp854->hd=( void*)({ struct _tuple10* _temp855=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp855->f1= _temp852; _temp855->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp856=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp856->hd=( void*) dsc; _temp856->tl= _temp850;
_temp856;}); _temp855;}); _temp854->tl= _temp845; _temp854;}); _LL839:;} static
struct Cyc_List_List* Cyc_Tcpat_norm_context( struct Cyc_List_List* ctxt){
struct Cyc_List_List* _temp857= ctxt; struct Cyc_List_List _temp863; struct Cyc_List_List*
_temp864; struct _tuple10* _temp866; struct _tuple10 _temp868; struct Cyc_List_List*
_temp869; struct Cyc_Tcpat_Con_s* _temp871; _LL859: if( _temp857 == 0){ goto
_LL860;} else{ goto _LL861;} _LL861: if( _temp857 == 0){ goto _LL858;} else{
_temp863=* _temp857; _LL867: _temp866=( struct _tuple10*) _temp863.hd; _temp868=*
_temp866; _LL872: _temp871= _temp868.f1; goto _LL870; _LL870: _temp869= _temp868.f2;
goto _LL865; _LL865: _temp864= _temp863.tl; goto _LL862;} _LL860: return((
struct Cyc_List_List*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char*
_temp873="norm_context: empty context"; struct _tagged_string _temp874; _temp874.curr=
_temp873; _temp874.base= _temp873; _temp874.last_plus_one= _temp873 + 28;
_temp874;})); _LL862: return Cyc_Tcpat_augment( _temp864,( void*)({ struct Cyc_Tcpat_Pos_struct*
_temp875=( struct Cyc_Tcpat_Pos_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Pos_struct));
_temp875[ 0]=({ struct Cyc_Tcpat_Pos_struct _temp876; _temp876.tag= Cyc_Tcpat_Pos;
_temp876.f1= _temp871; _temp876.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp869); _temp876;}); _temp875;})); _LL858:;} static void*
Cyc_Tcpat_build_desc( struct Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List*
work){ struct _tuple5 _temp878=({ struct _tuple5 _temp877; _temp877.f1= ctxt;
_temp877.f2= work; _temp877;}); struct Cyc_List_List* _temp888; struct Cyc_List_List*
_temp890; struct Cyc_List_List* _temp892; struct Cyc_List_List* _temp894; struct
Cyc_List_List* _temp896; struct Cyc_List_List* _temp898; struct Cyc_List_List*
_temp900; struct Cyc_List_List _temp902; struct Cyc_List_List* _temp903; struct
_tuple9* _temp905; struct _tuple9 _temp907; struct Cyc_List_List* _temp908;
struct Cyc_List_List* _temp910; struct Cyc_List_List* _temp912; struct Cyc_List_List*
_temp914; struct Cyc_List_List _temp916; struct Cyc_List_List* _temp917; struct
_tuple10* _temp919; struct _tuple10 _temp921; struct Cyc_List_List* _temp922;
struct Cyc_Tcpat_Con_s* _temp924; _LL880: _LL891: _temp890= _temp878.f1; if(
_temp890 == 0){ goto _LL889;} else{ goto _LL882;} _LL889: _temp888= _temp878.f2;
if( _temp888 == 0){ goto _LL881;} else{ goto _LL882;} _LL882: _LL895: _temp894=
_temp878.f1; if( _temp894 == 0){ goto _LL893;} else{ goto _LL884;} _LL893:
_temp892= _temp878.f2; goto _LL883; _LL884: _LL899: _temp898= _temp878.f1; goto
_LL897; _LL897: _temp896= _temp878.f2; if( _temp896 == 0){ goto _LL885;} else{
goto _LL886;} _LL886: _LL915: _temp914= _temp878.f1; if( _temp914 == 0){ goto
_LL879;} else{ _temp916=* _temp914; _LL920: _temp919=( struct _tuple10*)
_temp916.hd; _temp921=* _temp919; _LL925: _temp924= _temp921.f1; goto _LL923;
_LL923: _temp922= _temp921.f2; goto _LL918; _LL918: _temp917= _temp916.tl; goto
_LL901;} _LL901: _temp900= _temp878.f2; if( _temp900 == 0){ goto _LL879;} else{
_temp902=* _temp900; _LL906: _temp905=( struct _tuple9*) _temp902.hd; _temp907=*
_temp905; _LL913: _temp912= _temp907.f1; goto _LL911; _LL911: _temp910= _temp907.f2;
goto _LL909; _LL909: _temp908= _temp907.f3; goto _LL904; _LL904: _temp903=
_temp902.tl; goto _LL887;} _LL881: return dsc; _LL883: return(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ char* _temp926="build_desc: ctxt and work don't match";
struct _tagged_string _temp927; _temp927.curr= _temp926; _temp927.base= _temp926;
_temp927.last_plus_one= _temp926 + 38; _temp927;})); _LL885: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(({ char* _temp928="build_desc: ctxt and work don't match";
struct _tagged_string _temp929; _temp929.curr= _temp928; _temp929.base= _temp928;
_temp929.last_plus_one= _temp928 + 38; _temp929;})); _LL887: { struct Cyc_Tcpat_Pos_struct*
_temp933=({ struct Cyc_Tcpat_Pos_struct* _temp930=( struct Cyc_Tcpat_Pos_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Pos_struct)); _temp930[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp931; _temp931.tag= Cyc_Tcpat_Pos; _temp931.f1= _temp924; _temp931.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp922),({
struct Cyc_List_List* _temp932=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp932->hd=( void*) dsc; _temp932->tl= _temp908;
_temp932;})); _temp931;}); _temp930;}); goto _LL934; _LL934: return Cyc_Tcpat_build_desc(
_temp917,( void*) _temp933, _temp903);} _LL879:;} static void* Cyc_Tcpat_match(
void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct
Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules); static
void* Cyc_Tcpat_or_match( void* dsc, struct Cyc_List_List* allmrules){ struct
Cyc_List_List* _temp935= allmrules; struct Cyc_List_List _temp941; struct Cyc_List_List*
_temp942; struct _tuple0* _temp944; struct _tuple0 _temp946; void* _temp947;
void* _temp949; _LL937: if( _temp935 == 0){ goto _LL938;} else{ goto _LL939;}
_LL939: if( _temp935 == 0){ goto _LL936;} else{ _temp941=* _temp935; _LL945:
_temp944=( struct _tuple0*) _temp941.hd; _temp946=* _temp944; _LL950: _temp949=
_temp946.f1; goto _LL948; _LL948: _temp947= _temp946.f2; goto _LL943; _LL943:
_temp942= _temp941.tl; goto _LL940;} _LL938: return( void*) Cyc_Tcpat_Failure;
_LL940: return(( void*(*)( void* p, struct Cyc_List_List* obj, void* dsc, struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp949, 0, dsc, 0, 0, _temp947,
_temp942); _LL936:;} static void* Cyc_Tcpat_match_compile( struct Cyc_List_List*
allmrules){ return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)((
void*)({ struct Cyc_Tcpat_Neg_struct* _temp951=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp951[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp952; _temp952.tag= Cyc_Tcpat_Neg; _temp952.f1= Cyc_Tcpat_empty_con_set();
_temp952;}); _temp951;}), allmrules);} static void* Cyc_Tcpat_and_match( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules){ struct Cyc_List_List* _temp953= work; struct Cyc_List_List
_temp961; struct Cyc_List_List* _temp962; struct _tuple9* _temp964; struct
_tuple9 _temp966; struct Cyc_List_List* _temp967; struct Cyc_List_List* _temp969;
struct Cyc_List_List* _temp971; struct Cyc_List_List _temp973; struct Cyc_List_List*
_temp974; struct _tuple9* _temp976; struct _tuple9 _temp978; struct Cyc_List_List*
_temp979; struct Cyc_List_List* _temp981; struct Cyc_List_List* _temp983; _LL955:
if( _temp953 == 0){ goto _LL956;} else{ goto _LL957;} _LL957: if( _temp953 == 0){
goto _LL959;} else{ _temp961=* _temp953; _LL965: _temp964=( struct _tuple9*)
_temp961.hd; _temp966=* _temp964; _LL972: _temp971= _temp966.f1; if( _temp971 ==
0){ goto _LL970;} else{ goto _LL959;} _LL970: _temp969= _temp966.f2; if(
_temp969 == 0){ goto _LL968;} else{ goto _LL959;} _LL968: _temp967= _temp966.f3;
if( _temp967 == 0){ goto _LL963;} else{ goto _LL959;} _LL963: _temp962= _temp961.tl;
goto _LL958;} _LL959: if( _temp953 == 0){ goto _LL954;} else{ _temp973=*
_temp953; _LL977: _temp976=( struct _tuple9*) _temp973.hd; _temp978=* _temp976;
_LL984: _temp983= _temp978.f1; goto _LL982; _LL982: _temp981= _temp978.f2; goto
_LL980; _LL980: _temp979= _temp978.f3; goto _LL975; _LL975: _temp974= _temp973.tl;
goto _LL960;} _LL956: return( void*)({ struct Cyc_Tcpat_Success_struct* _temp985=(
struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp985[ 0]=({ struct Cyc_Tcpat_Success_struct _temp986; _temp986.tag= Cyc_Tcpat_Success;
_temp986.f1=( void*) right_hand_side; _temp986;}); _temp985;}); _LL958: return((
void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context(
ctx), _temp962, right_hand_side, rules); _LL960: if(( _temp983 == 0? 1: _temp981
== 0)? 1: _temp979 == 0){ return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
char* _temp987="tcpat:and_match: malformed work frame"; struct _tagged_string
_temp988; _temp988.curr= _temp987; _temp988.base= _temp987; _temp988.last_plus_one=
_temp987 + 38; _temp988;}));}{ struct Cyc_List_List _temp991; struct Cyc_List_List*
_temp992; void* _temp994; struct Cyc_List_List* _temp989=( struct Cyc_List_List*)
_check_null( _temp983); _temp991=* _temp989; _LL995: _temp994=( void*) _temp991.hd;
goto _LL993; _LL993: _temp992= _temp991.tl; goto _LL990; _LL990: { struct Cyc_List_List
_temp998; struct Cyc_List_List* _temp999; struct Cyc_List_List* _temp1001;
struct Cyc_List_List* _temp996=( struct Cyc_List_List*) _check_null( _temp981);
_temp998=* _temp996; _LL1002: _temp1001=( struct Cyc_List_List*) _temp998.hd;
goto _LL1000; _LL1000: _temp999= _temp998.tl; goto _LL997; _LL997: { struct Cyc_List_List
_temp1005; struct Cyc_List_List* _temp1006; void* _temp1008; struct Cyc_List_List*
_temp1003=( struct Cyc_List_List*) _check_null( _temp979); _temp1005=* _temp1003;
_LL1009: _temp1008=( void*) _temp1005.hd; goto _LL1007; _LL1007: _temp1006=
_temp1005.tl; goto _LL1004; _LL1004: { struct _tuple9* _temp1011=({ struct
_tuple9* _temp1010=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp1010->f1= _temp992; _temp1010->f2= _temp999; _temp1010->f3= _temp1006;
_temp1010;}); goto _LL1012; _LL1012: return(( void*(*)( void* p, struct Cyc_List_List*
obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp994,
_temp1001, _temp1008, ctx,({ struct Cyc_List_List* _temp1013=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1013->hd=( void*) _temp1011;
_temp1013->tl= _temp974; _temp1013;}), right_hand_side, rules);}}}} _LL954:;}
static struct Cyc_List_List* Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon,
void* dsc){ void* _temp1014= dsc; struct Cyc_Set_Set* _temp1020; struct Cyc_List_List*
_temp1022; struct Cyc_Tcpat_Con_s* _temp1024; _LL1016: if(*(( int*) _temp1014)
== Cyc_Tcpat_Neg){ _LL1021: _temp1020=(( struct Cyc_Tcpat_Neg_struct*) _temp1014)->f1;
goto _LL1017;} else{ goto _LL1018;} _LL1018: if(*(( int*) _temp1014) == Cyc_Tcpat_Pos){
_LL1025: _temp1024=(( struct Cyc_Tcpat_Pos_struct*) _temp1014)->f1; goto _LL1023;
_LL1023: _temp1022=(( struct Cyc_Tcpat_Pos_struct*) _temp1014)->f2; goto _LL1019;}
else{ goto _LL1015;} _LL1017: { void* any=( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp1029=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp1029[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp1030; _temp1030.tag= Cyc_Tcpat_Neg;
_temp1030.f1= Cyc_Tcpat_empty_con_set(); _temp1030;}); _temp1029;}); struct Cyc_List_List*
_temp1026= 0; goto _LL1027; _LL1027:{ int i= 0; for( 0; i < pcon->arity; ++ i){
_temp1026=({ struct Cyc_List_List* _temp1028=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1028->hd=( void*) any; _temp1028->tl=
_temp1026; _temp1028;});}} return _temp1026;} _LL1019: return _temp1022; _LL1015:;}
static struct Cyc_List_List* Cyc_Tcpat_getoarg( struct Cyc_List_List* obj, int i){
return({ struct Cyc_List_List* _temp1031=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1031->hd=( void*)( i + 1); _temp1031->tl=
obj; _temp1031;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s*
pcon, struct Cyc_List_List* obj){ return(( struct Cyc_List_List*(*)( int n,
struct Cyc_List_List*(* f)( struct Cyc_List_List*, int), struct Cyc_List_List*
env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static void*
Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp1032= p; struct Cyc_List_List* _temp1038; struct Cyc_Tcpat_Con_s*
_temp1040; _LL1034: if( _temp1032 ==( void*) Cyc_Tcpat_Any){ goto _LL1035;}
else{ goto _LL1036;} _LL1036: if(( unsigned int) _temp1032 > 1u?*(( int*)
_temp1032) == Cyc_Tcpat_Con: 0){ _LL1041: _temp1040=(( struct Cyc_Tcpat_Con_struct*)
_temp1032)->f1; goto _LL1039; _LL1039: _temp1038=(( struct Cyc_Tcpat_Con_struct*)
_temp1032)->f2; goto _LL1037;} else{ goto _LL1033;} _LL1035: return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc),
work, right_hand_side, rules); _LL1037: { void* _temp1042= Cyc_Tcpat_static_match(
_temp1040, dsc); _LL1044: if( _temp1042 ==( void*) Cyc_Tcpat_Yes){ goto _LL1045;}
else{ goto _LL1046;} _LL1046: if( _temp1042 ==( void*) Cyc_Tcpat_No){ goto
_LL1047;} else{ goto _LL1048;} _LL1048: if( _temp1042 ==( void*) Cyc_Tcpat_Maybe){
goto _LL1049;} else{ goto _LL1043;} _LL1045: { struct Cyc_List_List* _temp1052=({
struct Cyc_List_List* _temp1050=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1050->hd=( void*)({ struct _tuple10* _temp1051=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1051->f1= _temp1040;
_temp1051->f2= 0; _temp1051;}); _temp1050->tl= ctx; _temp1050;}); goto _LL1053;
_LL1053: { struct _tuple9* _temp1055=({ struct _tuple9* _temp1054=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp1054->f1= _temp1038;
_temp1054->f2= Cyc_Tcpat_getoargs( _temp1040, obj); _temp1054->f3= Cyc_Tcpat_getdargs(
_temp1040, dsc); _temp1054;}); goto _LL1056; _LL1056: { struct Cyc_List_List*
_temp1058=({ struct Cyc_List_List* _temp1057=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1057->hd=( void*) _temp1055; _temp1057->tl=
work; _temp1057;}); goto _LL1059; _LL1059: return(( void*(*)( struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_and_match)( _temp1052, _temp1058, right_hand_side, rules);}}}
_LL1047: return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)(
Cyc_Tcpat_build_desc( ctx, dsc, work), rules); _LL1049: { struct Cyc_List_List*
_temp1062=({ struct Cyc_List_List* _temp1060=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1060->hd=( void*)({ struct _tuple10*
_temp1061=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1061->f1=
_temp1040; _temp1061->f2= 0; _temp1061;}); _temp1060->tl= ctx; _temp1060;});
goto _LL1063; _LL1063: { struct _tuple9* _temp1065=({ struct _tuple9* _temp1064=(
struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp1064->f1= _temp1038;
_temp1064->f2= Cyc_Tcpat_getoargs( _temp1040, obj); _temp1064->f3= Cyc_Tcpat_getdargs(
_temp1040, dsc); _temp1064;}); goto _LL1066; _LL1066: { struct Cyc_List_List*
_temp1068=({ struct Cyc_List_List* _temp1067=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1067->hd=( void*) _temp1065; _temp1067->tl=
work; _temp1067;}); goto _LL1069; _LL1069: { void* _temp1070=(( void*(*)( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules)) Cyc_Tcpat_and_match)( _temp1062, _temp1068,
right_hand_side, rules); goto _LL1071; _LL1071: { void* _temp1072=(( void*(*)(
void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc(
ctx, Cyc_Tcpat_add_neg( dsc, _temp1040), work), rules); goto _LL1073; _LL1073:
return( void*)({ struct Cyc_Tcpat_IfEq_struct* _temp1074=( struct Cyc_Tcpat_IfEq_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct)); _temp1074[ 0]=({ struct Cyc_Tcpat_IfEq_struct
_temp1075; _temp1075.tag= Cyc_Tcpat_IfEq; _temp1075.f1= obj; _temp1075.f2=
_temp1040; _temp1075.f3=( void*) _temp1070; _temp1075.f4=( void*) _temp1072;
_temp1075;}); _temp1074;});}}}}} _LL1043:;} _LL1033:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp1076= d; void* _temp1084; void* _temp1086; void*
_temp1088; struct Cyc_Tcpat_Con_s* _temp1090; struct Cyc_List_List* _temp1092;
_LL1078: if( _temp1076 ==( void*) Cyc_Tcpat_Failure){ goto _LL1079;} else{ goto
_LL1080;} _LL1080: if(( unsigned int) _temp1076 > 1u?*(( int*) _temp1076) == Cyc_Tcpat_Success:
0){ _LL1085: _temp1084=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1076)->f1;
goto _LL1081;} else{ goto _LL1082;} _LL1082: if(( unsigned int) _temp1076 > 1u?*((
int*) _temp1076) == Cyc_Tcpat_IfEq: 0){ _LL1093: _temp1092=(( struct Cyc_Tcpat_IfEq_struct*)
_temp1076)->f1; goto _LL1091; _LL1091: _temp1090=(( struct Cyc_Tcpat_IfEq_struct*)
_temp1076)->f2; goto _LL1089; _LL1089: _temp1088=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1076)->f3; goto _LL1087; _LL1087: _temp1086=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1076)->f4; goto _LL1083;} else{ goto _LL1077;} _LL1079: not_exhaust( env1);
goto _LL1077; _LL1081: rhs_appears( env2, _temp1084); goto _LL1077; _LL1083:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1088,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1086, not_exhaust, env1, rhs_appears, env2); goto _LL1077; _LL1077:;}
struct _tuple11{ int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple12{
void* f1; struct _tuple11* f2; } ; static struct _tuple12* Cyc_Tcpat_get_match(
int* r, struct Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat(
swc->pattern); struct _tuple11* rhs=({ struct _tuple11* _temp1099=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1099->f1= 0; _temp1099->f2=(
swc->pattern)->loc; _temp1099;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp1094=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1094->hd=( void*) sp0; _temp1094->tl=({ struct Cyc_List_List*
_temp1095=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1095->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp1095->tl= 0; _temp1095;});
_temp1094;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1096=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1096->hd=( void*) sp0; _temp1096->tl=({ struct Cyc_List_List* _temp1097=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1097->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp1097->tl= 0; _temp1097;}); _temp1096;}));}
return({ struct _tuple12* _temp1098=( struct _tuple12*) GC_malloc( sizeof(
struct _tuple12)); _temp1098->f1= sp; _temp1098->f2= rhs; _temp1098;});} static
void Cyc_Tcpat_not_exhaust_err( struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr(
loc,({ char* _temp1100="patterns may not be exhaustive"; struct _tagged_string
_temp1101; _temp1101.curr= _temp1100; _temp1101.base= _temp1100; _temp1101.last_plus_one=
_temp1100 + 31; _temp1101;}));} static void Cyc_Tcpat_rule_occurs( int dummy,
struct _tuple11* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int _temp1102= 0;
goto _LL1103; _LL1103: { struct Cyc_List_List* _temp1104=(( struct Cyc_List_List*(*)(
struct _tuple12*(* f)( int*, struct Cyc_Absyn_Switch_clause*), int* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcpat_get_match,& _temp1102, swcs); goto
_LL1105; _LL1105: { void* _temp1106=(( void*(*)( struct Cyc_List_List* allmrules))
Cyc_Tcpat_match_compile)( _temp1104); goto _LL1107; _LL1107:(( void(*)( void* d,
void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple11*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1106, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp1104 != 0; _temp1104=(( struct Cyc_List_List*) _check_null( _temp1104))->tl){
struct _tuple12 _temp1110; struct _tuple11* _temp1111; struct _tuple11 _temp1113;
struct Cyc_Position_Segment* _temp1114; int _temp1116; void* _temp1118; struct
_tuple12* _temp1108=( struct _tuple12*)(( struct Cyc_List_List*) _check_null(
_temp1104))->hd; _temp1110=* _temp1108; _LL1119: _temp1118= _temp1110.f1; goto
_LL1112; _LL1112: _temp1111= _temp1110.f2; _temp1113=* _temp1111; _LL1117:
_temp1116= _temp1113.f1; goto _LL1115; _LL1115: _temp1114= _temp1113.f2; goto
_LL1109; _LL1109: if( ! _temp1116){ Cyc_Tcutil_terr( _temp1114,({ char*
_temp1120="redundant pattern"; struct _tagged_string _temp1121; _temp1121.curr=
_temp1120; _temp1121.base= _temp1120; _temp1121.last_plus_one= _temp1120 + 18;
_temp1121;}));}}}}} struct _tuple13{ struct Cyc_Position_Segment* f1; int f2; }
; static void Cyc_Tcpat_not_exhaust_warn( struct _tuple13* pr){(* pr).f2= 0; Cyc_Tcutil_warn((*
pr).f1,({ char* _temp1122="pattern not exhaustive"; struct _tagged_string
_temp1123; _temp1123.curr= _temp1122; _temp1123.base= _temp1122; _temp1123.last_plus_one=
_temp1122 + 23; _temp1123;}));} static void Cyc_Tcpat_dummy_fn( int i, int j){
return;} struct _tuple14{ void* f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
_temp1126=({ struct Cyc_List_List* _temp1124=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1124->hd=( void*)({ struct _tuple14*
_temp1125=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1125->f1=
Cyc_Tcpat_compile_pat( p); _temp1125->f2= 0; _temp1125;}); _temp1124->tl= 0;
_temp1124;}); goto _LL1127; _LL1127: { void* _temp1128=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( _temp1126); goto _LL1129; _LL1129: {
struct _tuple13* _temp1131=({ struct _tuple13* _temp1130=( struct _tuple13*)
GC_malloc( sizeof( struct _tuple13)); _temp1130->f1= loc; _temp1130->f2= 1;
_temp1130;}); goto _LL1132; _LL1132:(( void(*)( void* d, void(* not_exhaust)(
struct _tuple13*), struct _tuple13* env1, void(* rhs_appears)( int, int), int
env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1128, Cyc_Tcpat_not_exhaust_warn,
_temp1131, Cyc_Tcpat_dummy_fn, 0); return(* _temp1131).f2;}}} static struct
_tuple12* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause* swc){ void* sp0=
Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({ struct _tuple11*
_temp1134=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1134->f1=
0; _temp1134->f2=( swc->pattern)->loc; _temp1134;}); return({ struct _tuple12*
_temp1133=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1133->f1=
sp0; _temp1133->f2= rhs; _temp1133;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
_temp1135=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); goto
_LL1136; _LL1136: { void* _temp1137=(( void*(*)( struct Cyc_List_List* allmrules))
Cyc_Tcpat_match_compile)( _temp1135); goto _LL1138; _LL1138:(( void(*)( void* d,
void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple11*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1137, Cyc_Tcpat_not_exhaust_err2, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp1135 != 0; _temp1135=(( struct Cyc_List_List*) _check_null( _temp1135))->tl){
struct _tuple12 _temp1141; struct _tuple11* _temp1142; struct _tuple11 _temp1144;
struct Cyc_Position_Segment* _temp1145; int _temp1147; void* _temp1149; struct
_tuple12* _temp1139=( struct _tuple12*)(( struct Cyc_List_List*) _check_null(
_temp1135))->hd; _temp1141=* _temp1139; _LL1150: _temp1149= _temp1141.f1; goto
_LL1143; _LL1143: _temp1142= _temp1141.f2; _temp1144=* _temp1142; _LL1148:
_temp1147= _temp1144.f1; goto _LL1146; _LL1146: _temp1145= _temp1144.f2; goto
_LL1140; _LL1140: if( ! _temp1147){ Cyc_Tcutil_terr( _temp1145,({ char*
_temp1151="redundant pattern"; struct _tagged_string _temp1152; _temp1152.curr=
_temp1151; _temp1152.base= _temp1151; _temp1152.last_plus_one= _temp1151 + 18;
_temp1152;}));}}}}