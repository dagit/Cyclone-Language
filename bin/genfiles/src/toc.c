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
tag; struct _tagged_string f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t; extern int Cyc_Stdio_fflush(
struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; struct
_tagged_ptr0{ void** curr; void** base; void** last_plus_one; } ; extern struct
Cyc_List_List* Cyc_List_list( struct _tagged_ptr0); extern struct Cyc_List_List*
Cyc_List_rlist( struct _RegionHandle*, struct _tagged_ptr0); extern int Cyc_List_length(
struct Cyc_List_List* x); extern char Cyc_List_List_empty[ 15u]; extern struct
Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c(
struct _RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter_c( void(*
f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_rrevappend( struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_rrev( struct _RegionHandle*, struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rflatten(
struct _RegionHandle*, struct Cyc_List_List* x); extern char Cyc_List_Nth[ 8u];
extern int Cyc_List_forall( int(* pred)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rfrom_array( struct _RegionHandle* r2,
struct _tagged_ptr0 arr); extern int Cyc_String_zstrptrcmp( struct
_tagged_string*, struct _tagged_string*); extern struct _tagged_string Cyc_String_strconcat(
struct _tagged_string, struct _tagged_string); extern struct _tagged_string Cyc_String_implode(
struct Cyc_List_List* c); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern
int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_gdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u]; extern char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key,
void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key);
extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void*
key); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; extern struct
_tagged_string Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp( struct
_tuple0*, struct _tuple0*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void*
Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_star_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_cstar_typ( void* t,
struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ(); extern
void* Cyc_Absyn_strct( struct _tagged_string* name); extern void* Cyc_Absyn_strctq(
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
_temp204;});}} struct _tagged_ptr1{ struct Cyc_Absyn_Structfield** curr; struct
Cyc_Absyn_Structfield** base; struct Cyc_Absyn_Structfield** last_plus_one; } ;
static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x, void* t){
void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
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
_LL215; _LL215: { struct Cyc_List_List* _temp222=({ struct Cyc_List_List*(*
_temp216)( struct _tagged_ptr1)=( struct Cyc_List_List*(*)( struct _tagged_ptr1))
Cyc_List_list; struct Cyc_Absyn_Structfield* _temp219= _temp208; struct Cyc_Absyn_Structfield*
_temp220= _temp211; struct Cyc_Absyn_Structfield* _temp221= _temp214; struct Cyc_Absyn_Structfield*
_temp217[ 3u]={ _temp219, _temp220, _temp221}; struct _tagged_ptr1 _temp218={
_temp217, _temp217, _temp217 + 3u}; _temp216( _temp218);}); goto _LL223; _LL223: {
struct Cyc_Absyn_Structdecl* _temp228=({ struct Cyc_Absyn_Structdecl* _temp224=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp224->sc=( void*)(( void*) Cyc_Absyn_Public); _temp224->name=({ struct Cyc_Core_Opt*
_temp226=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp226->v=( void*)({ struct _tuple0* _temp227=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp227->f1= Cyc_Toc_abs_ns; _temp227->f2= x;
_temp227;}); _temp226;}); _temp224->tvs= 0; _temp224->fields=({ struct Cyc_Core_Opt*
_temp225=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp225->v=( void*)(( struct Cyc_List_List*) _temp222); _temp225;}); _temp224->attributes=
0; _temp224;}); goto _LL229; _LL229: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp230=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp230->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp231=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp231[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp232; _temp232.tag= Cyc_Absyn_Struct_d;
_temp232.f1= _temp228; _temp232;}); _temp231;}), 0); _temp230->tl= Cyc_Toc_result_decls;
_temp230;});}}}}} static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static
int Cyc_Toc_tuple_type_counter= 0; struct _tuple5{ struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static struct _tagged_string* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp233= Cyc_Toc_tuple_types;
goto _LL234; _LL234: for( 0; _temp233 != 0; _temp233=(( struct Cyc_List_List*)
_check_null( _temp233))->tl){ struct _tuple5 _temp237; struct Cyc_List_List*
_temp238; struct _tagged_string* _temp240; struct _tuple5* _temp235=( struct
_tuple5*)(( struct Cyc_List_List*) _check_null( _temp233))->hd; _temp237=*
_temp235; _LL241: _temp240= _temp237.f1; goto _LL239; _LL239: _temp238= _temp237.f2;
goto _LL236; _LL236: { int okay= 1; struct Cyc_List_List* _temp242= tqs0; goto
_LL243; _LL243: for( 0; _temp242 != 0? _temp238 != 0: 0;( _temp242=(( struct Cyc_List_List*)
_check_null( _temp242))->tl, _temp238=(( struct Cyc_List_List*) _check_null(
_temp238))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp242))->hd)).f2,( void*)(( struct Cyc_List_List*) _check_null(
_temp238))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp242 != 0)? 1: _temp238
!= 0){ continue;} return _temp240;}}}{ struct _tagged_string* x=({ struct
_tagged_string* _temp246=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp246[ 0]= xprintf("_tuple%d", Cyc_Toc_tuple_type_counter
++); _temp246;}); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt,
tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({ struct Cyc_List_List*
_temp244=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp244->hd=( void*)({ struct _tuple5* _temp245=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp245->f1= x; _temp245->f2= ts; _temp245;});
_temp244->tl= Cyc_Toc_tuple_types; _temp244;}); return x;}} static struct Cyc_List_List*
Cyc_Toc_tagged_ptr_types= 0; static int Cyc_Toc_tagged_ptr_counter= 0; static
void* Cyc_Toc_tagged_string_type_v=( void*) 0u; static void* Cyc_Toc_tagged_string_type(){
if( Cyc_Toc_tagged_string_type_v ==( void*) Cyc_Absyn_VoidType){ Cyc_Toc_tagged_string_type_v=
Cyc_Absyn_strct( Cyc_Toc__tagged_string_sp);} return Cyc_Toc_tagged_string_type_v;}
struct _tuple6{ struct _tagged_string* f1; void* f2; } ; static void* Cyc_Toc_add_tagged_ptr_type(
void* t, struct Cyc_Absyn_Tqual tq){{ void* _temp247= t; void* _temp253; void*
_temp255; _LL249: if(( unsigned int) _temp247 > 4u?*(( int*) _temp247) == Cyc_Absyn_IntType:
0){ _LL256: _temp255=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp247)->f1;
if( _temp255 ==( void*) Cyc_Absyn_Unsigned){ goto _LL254;} else{ goto _LL251;}
_LL254: _temp253=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp247)->f2; if(
_temp253 ==( void*) Cyc_Absyn_B1){ goto _LL250;} else{ goto _LL251;}} else{ goto
_LL251;} _LL251: goto _LL252; _LL250: return Cyc_Toc_tagged_string_type();
_LL252: goto _LL248; _LL248:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts=(( struct Cyc_List_List*) _check_null( tts))->tl){ struct
_tuple6 _temp259; void* _temp260; struct _tagged_string* _temp262; struct
_tuple6* _temp257=( struct _tuple6*)(( struct Cyc_List_List*) _check_null( tts))->hd;
_temp259=* _temp257; _LL263: _temp262= _temp259.f1; goto _LL261; _LL261:
_temp260= _temp259.f2; goto _LL258; _LL258: if( ! Cyc_Tcutil_unify( t, _temp260)){
continue;} return Cyc_Absyn_strct( _temp262);}}{ struct _tagged_string* x=({
struct _tagged_string* _temp266=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp266[ 0]= xprintf("_tagged_ptr%d", Cyc_Toc_tagged_ptr_counter
++); _temp266;}); Cyc_Toc_add_tagged_ptr_decl( x, t); Cyc_Toc_tagged_ptr_types=({
struct Cyc_List_List* _temp264=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp264->hd=( void*)({ struct _tuple6* _temp265=(
struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp265->f1= x; _temp265->f2=
t; _temp265;}); _temp264->tl= Cyc_Toc_tagged_ptr_types; _temp264;}); return Cyc_Absyn_strct(
x);}} static int Cyc_Toc_temp_var_counter= 0; struct _tuple0* Cyc_Toc_temp_var(){
return({ struct _tuple0* _temp267=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp267->f1=( void*) Cyc_Absyn_Loc_n; _temp267->f2=({ struct
_tagged_string* _temp268=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp268[ 0]= xprintf("_temp%d", Cyc_Toc_temp_var_counter
++); _temp268;}); _temp267;});} static int Cyc_Toc_fresh_label_counter= 0;
static struct _tagged_string* Cyc_Toc_fresh_label(){ return({ struct
_tagged_string* _temp269=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp269[ 0]= xprintf("_LL%d", Cyc_Toc_temp_var_counter ++);
_temp269;});} static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple1*
Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp272; struct Cyc_Absyn_Tqual
_temp274; struct Cyc_Core_Opt* _temp276; struct _tuple1 _temp270=* a; _LL277:
_temp276= _temp270.f1; goto _LL275; _LL275: _temp274= _temp270.f2; goto _LL273;
_LL273: _temp272= _temp270.f3; goto _LL271; _LL271: return({ struct _tuple1*
_temp278=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp278->f1=
_temp276; _temp278->f2= _temp274; _temp278->f3= Cyc_Toc_typ_to_c( _temp272);
_temp278;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp281; struct Cyc_Absyn_Tqual _temp283; struct _tuple3 _temp279=* x;
_LL284: _temp283= _temp279.f1; goto _LL282; _LL282: _temp281= _temp279.f2; goto
_LL280; _LL280: return({ struct _tuple3* _temp285=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp285->f1= _temp283; _temp285->f2= Cyc_Toc_typ_to_c(
_temp281); _temp285;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp286= t; struct Cyc_Absyn_Exp* _temp294; struct Cyc_Absyn_Tqual _temp296;
void* _temp298; int _temp300; struct Cyc_Core_Opt* _temp302; struct Cyc_Core_Opt
_temp304; void* _temp305; void* _temp307; _LL288: if(( unsigned int) _temp286 >
4u?*(( int*) _temp286) == Cyc_Absyn_ArrayType: 0){ _LL299: _temp298=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp286)->f1; goto _LL297; _LL297: _temp296=((
struct Cyc_Absyn_ArrayType_struct*) _temp286)->f2; goto _LL295; _LL295: _temp294=((
struct Cyc_Absyn_ArrayType_struct*) _temp286)->f3; goto _LL289;} else{ goto
_LL290;} _LL290: if(( unsigned int) _temp286 > 4u?*(( int*) _temp286) == Cyc_Absyn_Evar:
0){ _LL308: _temp307=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp286)->f1;
goto _LL303; _LL303: _temp302=(( struct Cyc_Absyn_Evar_struct*) _temp286)->f2;
if( _temp302 == 0){ goto _LL292;} else{ _temp304=* _temp302; _LL306: _temp305=(
void*) _temp304.v; goto _LL301;} _LL301: _temp300=(( struct Cyc_Absyn_Evar_struct*)
_temp286)->f3; goto _LL291;} else{ goto _LL292;} _LL292: goto _LL293; _LL289:
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp309=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp309[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp310; _temp310.tag= Cyc_Absyn_ArrayType; _temp310.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp298); _temp310.f2= _temp296; _temp310.f3=
_temp294; _temp310;}); _temp309;}); _LL291: return Cyc_Toc_typ_to_c_array(
_temp305); _LL293: return Cyc_Toc_typ_to_c( t); _LL287:;} static void* Cyc_Toc_typ_to_c(
void* t); static struct Cyc_Absyn_Structfield* Cyc_Toc_structfield_to_c( struct
Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield* _temp311=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp311->name= f->name; _temp311->tq= f->tq; _temp311->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp311->width= f->width; _temp311->attributes= f->attributes;
_temp311;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp312= t; int
_temp364; struct Cyc_Core_Opt* _temp366; void* _temp368; int _temp370; struct
Cyc_Core_Opt* _temp372; struct Cyc_Core_Opt _temp374; void* _temp375; void*
_temp377; struct Cyc_Absyn_Tvar* _temp379; struct Cyc_Absyn_TunionInfo _temp381;
void* _temp383; struct Cyc_List_List* _temp385; void* _temp387; struct Cyc_Absyn_Tuniondecl*
_temp389; struct Cyc_Absyn_TunionInfo _temp391; struct Cyc_Absyn_TunionFieldInfo
_temp393; struct Cyc_List_List* _temp395; void* _temp397; struct Cyc_Absyn_Tunionfield*
_temp399; struct Cyc_Absyn_Tuniondecl* _temp401; struct Cyc_Absyn_TunionFieldInfo
_temp403; struct Cyc_Absyn_PtrInfo _temp405; struct Cyc_Absyn_Conref* _temp407;
struct Cyc_Absyn_Tqual _temp409; struct Cyc_Absyn_Conref* _temp411; void*
_temp413; void* _temp415; void* _temp417; void* _temp419; struct Cyc_Absyn_Exp*
_temp421; struct Cyc_Absyn_Tqual _temp423; void* _temp425; struct Cyc_Absyn_FnInfo
_temp427; struct Cyc_List_List* _temp429; struct Cyc_Absyn_VarargInfo* _temp431;
int _temp433; struct Cyc_List_List* _temp435; void* _temp437; struct Cyc_Core_Opt*
_temp439; struct Cyc_List_List* _temp441; struct Cyc_List_List* _temp443; struct
Cyc_List_List* _temp445; struct Cyc_List_List* _temp447; struct Cyc_Absyn_Uniondecl**
_temp449; struct Cyc_List_List* _temp451; struct _tuple0* _temp453; struct Cyc_Absyn_Structdecl**
_temp455; struct Cyc_List_List* _temp457; struct _tuple0* _temp459; struct Cyc_Absyn_Enumdecl*
_temp461; struct _tuple0* _temp463; struct Cyc_Core_Opt* _temp465; struct Cyc_List_List*
_temp467; struct _tuple0* _temp469; void* _temp471; void* _temp473; struct Cyc_List_List*
_temp475; _LL314: if( _temp312 ==( void*) Cyc_Absyn_VoidType){ goto _LL315;}
else{ goto _LL316;} _LL316: if(( unsigned int) _temp312 > 4u?*(( int*) _temp312)
== Cyc_Absyn_Evar: 0){ _LL369: _temp368=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp312)->f1; goto _LL367; _LL367: _temp366=(( struct Cyc_Absyn_Evar_struct*)
_temp312)->f2; if( _temp366 == 0){ goto _LL365;} else{ goto _LL318;} _LL365:
_temp364=(( struct Cyc_Absyn_Evar_struct*) _temp312)->f3; goto _LL317;} else{
goto _LL318;} _LL318: if(( unsigned int) _temp312 > 4u?*(( int*) _temp312) ==
Cyc_Absyn_Evar: 0){ _LL378: _temp377=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp312)->f1; goto _LL373; _LL373: _temp372=(( struct Cyc_Absyn_Evar_struct*)
_temp312)->f2; if( _temp372 == 0){ goto _LL320;} else{ _temp374=* _temp372;
_LL376: _temp375=( void*) _temp374.v; goto _LL371;} _LL371: _temp370=(( struct
Cyc_Absyn_Evar_struct*) _temp312)->f3; goto _LL319;} else{ goto _LL320;} _LL320:
if(( unsigned int) _temp312 > 4u?*(( int*) _temp312) == Cyc_Absyn_VarType: 0){
_LL380: _temp379=(( struct Cyc_Absyn_VarType_struct*) _temp312)->f1; goto _LL321;}
else{ goto _LL322;} _LL322: if(( unsigned int) _temp312 > 4u?*(( int*) _temp312)
== Cyc_Absyn_TunionType: 0){ _LL382: _temp381=(( struct Cyc_Absyn_TunionType_struct*)
_temp312)->f1; _LL388: _temp387=( void*) _temp381.tunion_info; if(*(( int*)
_temp387) == Cyc_Absyn_KnownTunion){ _LL390: _temp389=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp387)->f1; goto _LL386;} else{ goto _LL324;} _LL386: _temp385= _temp381.targs;
goto _LL384; _LL384: _temp383=( void*) _temp381.rgn; goto _LL323;} else{ goto
_LL324;} _LL324: if(( unsigned int) _temp312 > 4u?*(( int*) _temp312) == Cyc_Absyn_TunionType:
0){ _LL392: _temp391=(( struct Cyc_Absyn_TunionType_struct*) _temp312)->f1; goto
_LL325;} else{ goto _LL326;} _LL326: if(( unsigned int) _temp312 > 4u?*(( int*)
_temp312) == Cyc_Absyn_TunionFieldType: 0){ _LL394: _temp393=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp312)->f1; _LL398: _temp397=( void*) _temp393.field_info; if(*(( int*)
_temp397) == Cyc_Absyn_KnownTunionfield){ _LL402: _temp401=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp397)->f1; goto _LL400; _LL400: _temp399=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp397)->f2; goto _LL396;} else{ goto _LL328;} _LL396: _temp395= _temp393.targs;
goto _LL327;} else{ goto _LL328;} _LL328: if(( unsigned int) _temp312 > 4u?*((
int*) _temp312) == Cyc_Absyn_TunionFieldType: 0){ _LL404: _temp403=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp312)->f1; goto _LL329;} else{ goto _LL330;} _LL330: if(( unsigned int)
_temp312 > 4u?*(( int*) _temp312) == Cyc_Absyn_PointerType: 0){ _LL406: _temp405=((
struct Cyc_Absyn_PointerType_struct*) _temp312)->f1; _LL416: _temp415=( void*)
_temp405.elt_typ; goto _LL414; _LL414: _temp413=( void*) _temp405.rgn_typ; goto
_LL412; _LL412: _temp411= _temp405.nullable; goto _LL410; _LL410: _temp409=
_temp405.tq; goto _LL408; _LL408: _temp407= _temp405.bounds; goto _LL331;} else{
goto _LL332;} _LL332: if(( unsigned int) _temp312 > 4u?*(( int*) _temp312) ==
Cyc_Absyn_IntType: 0){ _LL420: _temp419=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp312)->f1; goto _LL418; _LL418: _temp417=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp312)->f2; goto _LL333;} else{ goto _LL334;} _LL334: if( _temp312 ==( void*)
Cyc_Absyn_FloatType){ goto _LL335;} else{ goto _LL336;} _LL336: if( _temp312 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL337;} else{ goto _LL338;} _LL338: if((
unsigned int) _temp312 > 4u?*(( int*) _temp312) == Cyc_Absyn_ArrayType: 0){
_LL426: _temp425=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp312)->f1;
goto _LL424; _LL424: _temp423=(( struct Cyc_Absyn_ArrayType_struct*) _temp312)->f2;
goto _LL422; _LL422: _temp421=(( struct Cyc_Absyn_ArrayType_struct*) _temp312)->f3;
goto _LL339;} else{ goto _LL340;} _LL340: if(( unsigned int) _temp312 > 4u?*((
int*) _temp312) == Cyc_Absyn_FnType: 0){ _LL428: _temp427=(( struct Cyc_Absyn_FnType_struct*)
_temp312)->f1; _LL442: _temp441= _temp427.tvars; goto _LL440; _LL440: _temp439=
_temp427.effect; goto _LL438; _LL438: _temp437=( void*) _temp427.ret_typ; goto
_LL436; _LL436: _temp435= _temp427.args; goto _LL434; _LL434: _temp433= _temp427.c_varargs;
goto _LL432; _LL432: _temp431= _temp427.cyc_varargs; goto _LL430; _LL430:
_temp429= _temp427.attributes; goto _LL341;} else{ goto _LL342;} _LL342: if((
unsigned int) _temp312 > 4u?*(( int*) _temp312) == Cyc_Absyn_TupleType: 0){
_LL444: _temp443=(( struct Cyc_Absyn_TupleType_struct*) _temp312)->f1; goto
_LL343;} else{ goto _LL344;} _LL344: if(( unsigned int) _temp312 > 4u?*(( int*)
_temp312) == Cyc_Absyn_AnonStructType: 0){ _LL446: _temp445=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp312)->f1; goto _LL345;} else{ goto _LL346;} _LL346: if(( unsigned int)
_temp312 > 4u?*(( int*) _temp312) == Cyc_Absyn_AnonUnionType: 0){ _LL448:
_temp447=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp312)->f1; goto _LL347;}
else{ goto _LL348;} _LL348: if(( unsigned int) _temp312 > 4u?*(( int*) _temp312)
== Cyc_Absyn_UnionType: 0){ _LL454: _temp453=(( struct Cyc_Absyn_UnionType_struct*)
_temp312)->f1; goto _LL452; _LL452: _temp451=(( struct Cyc_Absyn_UnionType_struct*)
_temp312)->f2; goto _LL450; _LL450: _temp449=(( struct Cyc_Absyn_UnionType_struct*)
_temp312)->f3; goto _LL349;} else{ goto _LL350;} _LL350: if(( unsigned int)
_temp312 > 4u?*(( int*) _temp312) == Cyc_Absyn_StructType: 0){ _LL460: _temp459=((
struct Cyc_Absyn_StructType_struct*) _temp312)->f1; goto _LL458; _LL458:
_temp457=(( struct Cyc_Absyn_StructType_struct*) _temp312)->f2; goto _LL456;
_LL456: _temp455=(( struct Cyc_Absyn_StructType_struct*) _temp312)->f3; goto
_LL351;} else{ goto _LL352;} _LL352: if(( unsigned int) _temp312 > 4u?*(( int*)
_temp312) == Cyc_Absyn_EnumType: 0){ _LL464: _temp463=(( struct Cyc_Absyn_EnumType_struct*)
_temp312)->f1; goto _LL462; _LL462: _temp461=(( struct Cyc_Absyn_EnumType_struct*)
_temp312)->f2; goto _LL353;} else{ goto _LL354;} _LL354: if(( unsigned int)
_temp312 > 4u?*(( int*) _temp312) == Cyc_Absyn_TypedefType: 0){ _LL470: _temp469=((
struct Cyc_Absyn_TypedefType_struct*) _temp312)->f1; goto _LL468; _LL468:
_temp467=(( struct Cyc_Absyn_TypedefType_struct*) _temp312)->f2; goto _LL466;
_LL466: _temp465=(( struct Cyc_Absyn_TypedefType_struct*) _temp312)->f3; goto
_LL355;} else{ goto _LL356;} _LL356: if(( unsigned int) _temp312 > 4u?*(( int*)
_temp312) == Cyc_Absyn_RgnHandleType: 0){ _LL472: _temp471=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp312)->f1; goto _LL357;} else{ goto _LL358;} _LL358: if( _temp312 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL359;} else{ goto _LL360;} _LL360: if(( unsigned int)
_temp312 > 4u?*(( int*) _temp312) == Cyc_Absyn_AccessEff: 0){ _LL474: _temp473=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp312)->f1; goto _LL361;} else{
goto _LL362;} _LL362: if(( unsigned int) _temp312 > 4u?*(( int*) _temp312) ==
Cyc_Absyn_JoinEff: 0){ _LL476: _temp475=(( struct Cyc_Absyn_JoinEff_struct*)
_temp312)->f1; goto _LL363;} else{ goto _LL313;} _LL315: return t; _LL317:
return( void*) Cyc_Absyn_VoidType; _LL319: return Cyc_Toc_typ_to_c( _temp375);
_LL321: return Cyc_Absyn_void_star_typ(); _LL323: return Cyc_Absyn_void_star_typ();
_LL325:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
char* _temp477="unresolved TunionType"; struct _tagged_string _temp478; _temp478.curr=
_temp477; _temp478.base= _temp477; _temp478.last_plus_one= _temp477 + 22;
_temp478;}))); _LL327: if( _temp399->typs == 0){ if( _temp401->is_xtunion){
return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);}
else{ return Cyc_Absyn_uint_t;}} else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp399->name,({ char* _temp479="_struct"; struct _tagged_string _temp480;
_temp480.curr= _temp479; _temp480.base= _temp479; _temp480.last_plus_one=
_temp479 + 8; _temp480;})));} _LL329:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ char* _temp481="unresolved TunionFieldType";
struct _tagged_string _temp482; _temp482.curr= _temp481; _temp482.base= _temp481;
_temp482.last_plus_one= _temp481 + 27; _temp482;}))); _LL331: _temp415= Cyc_Toc_typ_to_c_array(
_temp415);{ void* _temp483=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp407))->v; void* _temp489; _LL485: if((
unsigned int) _temp483 > 1u?*(( int*) _temp483) == Cyc_Absyn_Eq_constr: 0){
_LL490: _temp489=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp483)->f1;
if( _temp489 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL486;} else{ goto _LL487;}}
else{ goto _LL487;} _LL487: goto _LL488; _LL486: return Cyc_Toc_add_tagged_ptr_type(
_temp415, _temp409); _LL488: return Cyc_Absyn_star_typ( _temp415,( void*) Cyc_Absyn_HeapRgn,
_temp409); _LL484:;} _LL333: return t; _LL335: return t; _LL337: return t;
_LL339: return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp491=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp491[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp492; _temp492.tag= Cyc_Absyn_ArrayType;
_temp492.f1=( void*) Cyc_Toc_typ_to_c_array( _temp425); _temp492.f2= _temp423;
_temp492.f3= _temp421; _temp492;}); _temp491;}); _LL341: { struct Cyc_List_List*
_temp493= _temp429; goto _LL494; _LL494: { struct Cyc_List_List* _temp495= 0;
goto _LL496; _LL496: for( 0; _temp429 != 0; _temp429=(( struct Cyc_List_List*)
_check_null( _temp429))->tl){ void* _temp497=( void*)(( struct Cyc_List_List*)
_check_null( _temp429))->hd; _LL499: if( _temp497 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL500;} else{ goto _LL501;} _LL501: if( _temp497 ==( void*) Cyc_Absyn_Const_att){
goto _LL502;} else{ goto _LL503;} _LL503: goto _LL504; _LL500: goto _LL502;
_LL502: continue; _LL504: _temp495=({ struct Cyc_List_List* _temp505=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp505->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( _temp429))->hd); _temp505->tl=
_temp495; _temp505;}); goto _LL498; _LL498:;}{ struct Cyc_List_List* _temp506=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp435); goto _LL507; _LL507: if(
_temp431 != 0){ void* _temp508= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp431))->type,( void*)(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp431))->rgn, Cyc_Toc_mt_tq)); goto
_LL509; _LL509: { struct _tuple1* _temp511=({ struct _tuple1* _temp510=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp510->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp431))->name; _temp510->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp431))->tq; _temp510->f3= _temp508; _temp510;}); goto _LL512;
_LL512: _temp506=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_imp_append)( _temp506,({ struct Cyc_List_List*
_temp513=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp513->hd=( void*) _temp511; _temp513->tl= 0; _temp513;}));}} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp514=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp514[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp515; _temp515.tag= Cyc_Absyn_FnType; _temp515.f1=({ struct Cyc_Absyn_FnInfo
_temp516; _temp516.tvars= 0; _temp516.effect= 0; _temp516.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp437); _temp516.args= _temp506; _temp516.c_varargs= _temp433; _temp516.cyc_varargs=
0; _temp516.attributes= _temp495; _temp516;}); _temp515;}); _temp514;});}}}
_LL343: _temp443=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp443);{
struct _tagged_string* _temp517= Cyc_Toc_add_tuple_type( _temp443); goto _LL518;
_LL518: return Cyc_Absyn_strct( _temp517);} _LL345: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp519=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp519[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp520; _temp520.tag= Cyc_Absyn_AnonStructType; _temp520.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp445); _temp520;}); _temp519;});
_LL347: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp521=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp521[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp522; _temp522.tag=
Cyc_Absyn_AnonUnionType; _temp522.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp447); _temp522;}); _temp521;}); _LL349: if( _temp453 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ char* _temp523="anonymous union";
struct _tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 16; _temp524;}));} return Cyc_Absyn_unionq_typ((
struct _tuple0*) _check_null( _temp453)); _LL351: if( _temp459 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ char* _temp525="anonymous struct";
struct _tagged_string _temp526; _temp526.curr= _temp525; _temp526.base= _temp525;
_temp526.last_plus_one= _temp525 + 17; _temp526;}));} return Cyc_Absyn_strctq((
struct _tuple0*) _check_null( _temp459)); _LL353: return t; _LL355: if( _temp465
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp527="Toc::typ_to_c: unresolved TypedefType"; struct _tagged_string _temp528;
_temp528.curr= _temp527; _temp528.base= _temp527; _temp528.last_plus_one=
_temp527 + 38; _temp528;}));} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp529=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp529[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp530; _temp530.tag= Cyc_Absyn_TypedefType;
_temp530.f1= _temp469; _temp530.f2= 0; _temp530.f3=({ struct Cyc_Core_Opt*
_temp531=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp531->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp465))->v); _temp531;}); _temp530;}); _temp529;}); _LL357:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL359: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp532="Toc::typ_to_c: type translation passed the heap region"; struct
_tagged_string _temp533; _temp533.curr= _temp532; _temp533.base= _temp532;
_temp533.last_plus_one= _temp532 + 55; _temp533;})); _LL361: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp534="Toc::typ_to_c: type translation passed the access eff";
struct _tagged_string _temp535; _temp535.curr= _temp534; _temp535.base= _temp534;
_temp535.last_plus_one= _temp534 + 54; _temp535;})); _LL363: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp536="Toc::typ_to_c: type translation passed the join eff";
struct _tagged_string _temp537; _temp537.curr= _temp536; _temp537.base= _temp536;
_temp537.last_plus_one= _temp536 + 52; _temp537;})); _LL313:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp538= t; struct Cyc_Absyn_Exp* _temp544; struct Cyc_Absyn_Tqual
_temp546; void* _temp548; _LL540: if(( unsigned int) _temp538 > 4u?*(( int*)
_temp538) == Cyc_Absyn_ArrayType: 0){ _LL549: _temp548=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp538)->f1; goto _LL547; _LL547: _temp546=(( struct Cyc_Absyn_ArrayType_struct*)
_temp538)->f2; goto _LL545; _LL545: _temp544=(( struct Cyc_Absyn_ArrayType_struct*)
_temp538)->f3; goto _LL541;} else{ goto _LL542;} _LL542: goto _LL543; _LL541:
return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ( _temp548,( void*) Cyc_Absyn_HeapRgn,
_temp546), e, l); _LL543: return Cyc_Absyn_cast_exp( t, e, l); _LL539:;} static
int Cyc_Toc_atomic_typ( void* t){ void* _temp550= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp586; void* _temp588; void* _temp590; struct Cyc_Absyn_Exp*
_temp592; struct Cyc_Absyn_Tqual _temp594; void* _temp596; struct Cyc_Absyn_FnInfo
_temp598; struct Cyc_List_List* _temp600; struct Cyc_Absyn_Structdecl** _temp602;
struct Cyc_List_List* _temp604; struct _tuple0* _temp606; struct Cyc_Absyn_TunionInfo
_temp608; struct Cyc_List_List* _temp610; struct Cyc_List_List* _temp612; struct
Cyc_Absyn_TunionFieldInfo _temp614; struct Cyc_List_List* _temp616; void*
_temp618; struct Cyc_Absyn_Tunionfield* _temp620; struct Cyc_Absyn_Tuniondecl*
_temp622; struct Cyc_Absyn_Uniondecl** _temp624; struct Cyc_List_List* _temp626;
struct _tuple0* _temp628; struct Cyc_Absyn_PtrInfo _temp630; void* _temp632;
_LL552: if( _temp550 ==( void*) Cyc_Absyn_VoidType){ goto _LL553;} else{ goto
_LL554;} _LL554: if(( unsigned int) _temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_VarType:
0){ _LL587: _temp586=(( struct Cyc_Absyn_VarType_struct*) _temp550)->f1; goto
_LL555;} else{ goto _LL556;} _LL556: if(( unsigned int) _temp550 > 4u?*(( int*)
_temp550) == Cyc_Absyn_IntType: 0){ _LL591: _temp590=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp550)->f1; goto _LL589; _LL589: _temp588=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp550)->f2; goto _LL557;} else{ goto _LL558;} _LL558: if( _temp550 ==( void*)
Cyc_Absyn_FloatType){ goto _LL559;} else{ goto _LL560;} _LL560: if( _temp550 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL561;} else{ goto _LL562;} _LL562: if((
unsigned int) _temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_ArrayType: 0){
_LL597: _temp596=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp550)->f1;
goto _LL595; _LL595: _temp594=(( struct Cyc_Absyn_ArrayType_struct*) _temp550)->f2;
goto _LL593; _LL593: _temp592=(( struct Cyc_Absyn_ArrayType_struct*) _temp550)->f3;
goto _LL563;} else{ goto _LL564;} _LL564: if(( unsigned int) _temp550 > 4u?*((
int*) _temp550) == Cyc_Absyn_FnType: 0){ _LL599: _temp598=(( struct Cyc_Absyn_FnType_struct*)
_temp550)->f1; goto _LL565;} else{ goto _LL566;} _LL566: if(( unsigned int)
_temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_TupleType: 0){ _LL601: _temp600=((
struct Cyc_Absyn_TupleType_struct*) _temp550)->f1; goto _LL567;} else{ goto
_LL568;} _LL568: if(( unsigned int) _temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_StructType:
0){ _LL607: _temp606=(( struct Cyc_Absyn_StructType_struct*) _temp550)->f1; goto
_LL605; _LL605: _temp604=(( struct Cyc_Absyn_StructType_struct*) _temp550)->f2;
goto _LL603; _LL603: _temp602=(( struct Cyc_Absyn_StructType_struct*) _temp550)->f3;
goto _LL569;} else{ goto _LL570;} _LL570: if(( unsigned int) _temp550 > 4u?*((
int*) _temp550) == Cyc_Absyn_TunionType: 0){ _LL609: _temp608=(( struct Cyc_Absyn_TunionType_struct*)
_temp550)->f1; goto _LL571;} else{ goto _LL572;} _LL572: if(( unsigned int)
_temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_AnonStructType: 0){ _LL611:
_temp610=(( struct Cyc_Absyn_AnonStructType_struct*) _temp550)->f1; goto _LL573;}
else{ goto _LL574;} _LL574: if(( unsigned int) _temp550 > 4u?*(( int*) _temp550)
== Cyc_Absyn_AnonUnionType: 0){ _LL613: _temp612=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp550)->f1; goto _LL575;} else{ goto _LL576;} _LL576: if(( unsigned int)
_temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_TunionFieldType: 0){ _LL615:
_temp614=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp550)->f1; _LL619:
_temp618=( void*) _temp614.field_info; if(*(( int*) _temp618) == Cyc_Absyn_KnownTunionfield){
_LL623: _temp622=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp618)->f1;
goto _LL621; _LL621: _temp620=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp618)->f2; goto _LL617;} else{ goto _LL578;} _LL617: _temp616= _temp614.targs;
goto _LL577;} else{ goto _LL578;} _LL578: if(( unsigned int) _temp550 > 4u?*((
int*) _temp550) == Cyc_Absyn_UnionType: 0){ _LL629: _temp628=(( struct Cyc_Absyn_UnionType_struct*)
_temp550)->f1; goto _LL627; _LL627: _temp626=(( struct Cyc_Absyn_UnionType_struct*)
_temp550)->f2; goto _LL625; _LL625: _temp624=(( struct Cyc_Absyn_UnionType_struct*)
_temp550)->f3; goto _LL579;} else{ goto _LL580;} _LL580: if(( unsigned int)
_temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_PointerType: 0){ _LL631: _temp630=((
struct Cyc_Absyn_PointerType_struct*) _temp550)->f1; goto _LL581;} else{ goto
_LL582;} _LL582: if(( unsigned int) _temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_RgnHandleType:
0){ _LL633: _temp632=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp550)->f1;
goto _LL583;} else{ goto _LL584;} _LL584: goto _LL585; _LL553: return 1; _LL555:
return 0; _LL557: return 1; _LL559: return 1; _LL561: return 0; _LL563: return
Cyc_Toc_atomic_typ( _temp596); _LL565: return 1; _LL567: for( 0; _temp600 != 0;
_temp600=(( struct Cyc_List_List*) _check_null( _temp600))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp600))->hd)).f2)){
return 0;}} return 1; _LL569: return 0; _LL571: return 0; _LL573: _temp612=
_temp610; goto _LL575; _LL575: for( 0; _temp612 != 0; _temp612=(( struct Cyc_List_List*)
_check_null( _temp612))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp612))->hd)->type)){ return 0;}} return
1; _LL577: return _temp620->typs == 0; _LL579: return 0; _LL581: return 0;
_LL583: return 0; _LL585:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp634= Cyc_Absynpp_typ2string( t); xprintf("atomic_typ:  bad type %.*s",
_temp634.last_plus_one - _temp634.curr, _temp634.curr);})); return 0; _LL551:;}
static int Cyc_Toc_is_void_star( void* t){ void* _temp635= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp641; struct Cyc_Absyn_Conref* _temp643; struct
Cyc_Absyn_Tqual _temp645; struct Cyc_Absyn_Conref* _temp647; void* _temp649;
void* _temp651; _LL637: if(( unsigned int) _temp635 > 4u?*(( int*) _temp635) ==
Cyc_Absyn_PointerType: 0){ _LL642: _temp641=(( struct Cyc_Absyn_PointerType_struct*)
_temp635)->f1; _LL652: _temp651=( void*) _temp641.elt_typ; goto _LL650; _LL650:
_temp649=( void*) _temp641.rgn_typ; goto _LL648; _LL648: _temp647= _temp641.nullable;
goto _LL646; _LL646: _temp645= _temp641.tq; goto _LL644; _LL644: _temp643=
_temp641.bounds; goto _LL638;} else{ goto _LL639;} _LL639: goto _LL640; _LL638: {
void* _temp653= Cyc_Tcutil_compress( _temp651); _LL655: if( _temp653 ==( void*)
Cyc_Absyn_VoidType){ goto _LL656;} else{ goto _LL657;} _LL657: goto _LL658;
_LL656: return 1; _LL658: return 0; _LL654:;} _LL640: return 0; _LL636:;} static
int Cyc_Toc_is_poly_field( void* t, struct _tagged_string* f){ void* _temp659=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl** _temp671; struct Cyc_Absyn_Structdecl*
_temp673; struct Cyc_List_List* _temp674; struct _tuple0* _temp676; struct Cyc_List_List*
_temp678; struct Cyc_List_List* _temp680; struct Cyc_Absyn_Uniondecl** _temp682;
struct Cyc_List_List* _temp684; struct _tuple0* _temp686; _LL661: if((
unsigned int) _temp659 > 4u?*(( int*) _temp659) == Cyc_Absyn_StructType: 0){
_LL677: _temp676=(( struct Cyc_Absyn_StructType_struct*) _temp659)->f1; goto
_LL675; _LL675: _temp674=(( struct Cyc_Absyn_StructType_struct*) _temp659)->f2;
goto _LL672; _LL672: _temp671=(( struct Cyc_Absyn_StructType_struct*) _temp659)->f3;
if( _temp671 == 0){ goto _LL663;} else{ _temp673=* _temp671; goto _LL662;}}
else{ goto _LL663;} _LL663: if(( unsigned int) _temp659 > 4u?*(( int*) _temp659)
== Cyc_Absyn_AnonStructType: 0){ _LL679: _temp678=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp659)->f1; goto _LL664;} else{ goto _LL665;} _LL665: if(( unsigned int)
_temp659 > 4u?*(( int*) _temp659) == Cyc_Absyn_AnonUnionType: 0){ _LL681:
_temp680=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp659)->f1; goto _LL666;}
else{ goto _LL667;} _LL667: if(( unsigned int) _temp659 > 4u?*(( int*) _temp659)
== Cyc_Absyn_UnionType: 0){ _LL687: _temp686=(( struct Cyc_Absyn_UnionType_struct*)
_temp659)->f1; goto _LL685; _LL685: _temp684=(( struct Cyc_Absyn_UnionType_struct*)
_temp659)->f2; goto _LL683; _LL683: _temp682=(( struct Cyc_Absyn_UnionType_struct*)
_temp659)->f3; goto _LL668;} else{ goto _LL669;} _LL669: goto _LL670; _LL662:
if( _temp673->fields == 0){( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ char* _temp688="is_poly_field: struct missing fields";
struct _tagged_string _temp689; _temp689.curr= _temp688; _temp689.base= _temp688;
_temp689.last_plus_one= _temp688 + 37; _temp689;})));} _temp678=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp673->fields))->v; goto _LL664; _LL664: {
struct Cyc_Absyn_Structfield* _temp690= Cyc_Absyn_lookup_field( _temp678, f);
goto _LL691; _LL691: if( _temp690 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp692=*(( struct Cyc_Absyn_Structfield*)
_check_null( _temp690))->name; xprintf("is_poly_field: bad field %.*s", _temp692.last_plus_one
- _temp692.curr, _temp692.curr);}));} return Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp690))->type);} _LL666: return 0;
_LL668: return 0; _LL670:( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp693= Cyc_Absynpp_typ2string( t);
xprintf("is_poly_field: bad type %.*s", _temp693.last_plus_one - _temp693.curr,
_temp693.curr);}))); _LL660:;} static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp*
e){ void* _temp694=( void*) e->r; struct _tagged_string* _temp702; struct Cyc_Absyn_Exp*
_temp704; struct _tagged_string* _temp706; struct Cyc_Absyn_Exp* _temp708;
_LL696: if(*(( int*) _temp694) == Cyc_Absyn_StructMember_e){ _LL705: _temp704=((
struct Cyc_Absyn_StructMember_e_struct*) _temp694)->f1; goto _LL703; _LL703:
_temp702=(( struct Cyc_Absyn_StructMember_e_struct*) _temp694)->f2; goto _LL697;}
else{ goto _LL698;} _LL698: if(*(( int*) _temp694) == Cyc_Absyn_StructArrow_e){
_LL709: _temp708=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp694)->f1; goto
_LL707; _LL707: _temp706=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp694)->f2;
goto _LL699;} else{ goto _LL700;} _LL700: goto _LL701; _LL697: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp704->topt))->v, _temp702);
_LL699: { void* _temp710= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp708->topt))->v); struct Cyc_Absyn_PtrInfo _temp716; struct Cyc_Absyn_Conref*
_temp718; struct Cyc_Absyn_Tqual _temp720; struct Cyc_Absyn_Conref* _temp722;
void* _temp724; void* _temp726; _LL712: if(( unsigned int) _temp710 > 4u?*(( int*)
_temp710) == Cyc_Absyn_PointerType: 0){ _LL717: _temp716=(( struct Cyc_Absyn_PointerType_struct*)
_temp710)->f1; _LL727: _temp726=( void*) _temp716.elt_typ; goto _LL725; _LL725:
_temp724=( void*) _temp716.rgn_typ; goto _LL723; _LL723: _temp722= _temp716.nullable;
goto _LL721; _LL721: _temp720= _temp716.tq; goto _LL719; _LL719: _temp718=
_temp716.bounds; goto _LL713;} else{ goto _LL714;} _LL714: goto _LL715; _LL713:
return Cyc_Toc_is_poly_field( _temp726, _temp706); _LL715:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp728= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp708->topt))->v); xprintf("is_poly_project:  bad type %.*s",
_temp728.last_plus_one - _temp728.curr, _temp728.curr);})); return 0; _LL711:;}
_LL701: return 0; _LL695:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_qv,
0),({ struct Cyc_List_List* _temp729=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp729->hd=( void*) s; _temp729->tl= 0; _temp729;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_atomic_qv, 0),({
struct Cyc_List_List* _temp730=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp730->hd=( void*) s; _temp730->tl= 0; _temp730;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__region_malloc_qv,
0),({ struct Cyc_List_List* _temp731=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp731->hd=( void*) rgn; _temp731->tl=({ struct Cyc_List_List*
_temp732=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp732->hd=( void*) s; _temp732->tl= 0; _temp732;}); _temp731;}), 0);} static
struct _tagged_string* Cyc_Toc_field_names_v[ 0u]={}; struct _tagged_ptr2{
struct _tagged_string** curr; struct _tagged_string** base; struct
_tagged_string** last_plus_one; } ; static struct _tagged_ptr2 Cyc_Toc_field_names={(
struct _tagged_string**) Cyc_Toc_field_names_v,( struct _tagged_string**) Cyc_Toc_field_names_v,(
struct _tagged_string**) Cyc_Toc_field_names_v + 0u}; static struct
_tagged_string* Cyc_Toc_fieldname( int i){ unsigned int fsz=({ struct
_tagged_ptr2 _temp740= Cyc_Toc_field_names;( unsigned int)( _temp740.last_plus_one
- _temp740.curr);}); if( i >= fsz){ Cyc_Toc_field_names=({ unsigned int _temp735=(
unsigned int)( i + 1); struct _tagged_string** _temp736=( struct _tagged_string**)
GC_malloc( sizeof( struct _tagged_string*) * _temp735); struct _tagged_ptr2
_temp739={ _temp736, _temp736, _temp736 + _temp735};{ unsigned int _temp737=
_temp735; unsigned int j; for( j= 0; j < _temp737; j ++){ _temp736[ j]= j < fsz?*((
struct _tagged_string**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Toc_field_names, sizeof( struct _tagged_string*),(
int) j):({ struct _tagged_string* _temp738=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string)); _temp738[ 0]= xprintf("f%d",( int) j); _temp738;});}};
_temp739;});} return*(( struct _tagged_string**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Toc_field_names,
sizeof( struct _tagged_string*), i);} static const int Cyc_Toc_NullCheck= 0;
struct Cyc_Toc_NullCheck_struct{ int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr=
1; struct Cyc_Toc_UntagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
int f3; } ; static const int Cyc_Toc_TagPtr= 2; struct Cyc_Toc_TagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Toc_NoConv=
0; typedef void* Cyc_Toc_conv; struct _tuple7{ void* f1; void* f2; } ; static
void* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2=
Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp742=({ struct _tuple7 _temp741;
_temp741.f1= t1; _temp741.f2= t2; _temp741;}); void* _temp748; struct Cyc_Absyn_PtrInfo
_temp750; void* _temp752; struct Cyc_Absyn_PtrInfo _temp754; _LL744: _LL753:
_temp752= _temp742.f1; if(( unsigned int) _temp752 > 4u?*(( int*) _temp752) ==
Cyc_Absyn_PointerType: 0){ _LL755: _temp754=(( struct Cyc_Absyn_PointerType_struct*)
_temp752)->f1; goto _LL749;} else{ goto _LL746;} _LL749: _temp748= _temp742.f2;
if(( unsigned int) _temp748 > 4u?*(( int*) _temp748) == Cyc_Absyn_PointerType: 0){
_LL751: _temp750=(( struct Cyc_Absyn_PointerType_struct*) _temp748)->f1; goto
_LL745;} else{ goto _LL746;} _LL746: goto _LL747; _LL745: { int _temp756=(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp754.nullable); goto
_LL757; _LL757: { int _temp758=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp750.nullable); goto _LL759; _LL759: { void* _temp760=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp754.bounds); goto _LL761;
_LL761: { void* _temp762=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp750.bounds); goto _LL763; _LL763: { struct _tuple7 _temp765=({ struct
_tuple7 _temp764; _temp764.f1= _temp760; _temp764.f2= _temp762; _temp764;});
void* _temp775; struct Cyc_Absyn_Exp* _temp777; void* _temp779; struct Cyc_Absyn_Exp*
_temp781; void* _temp783; void* _temp785; void* _temp787; void* _temp789; struct
Cyc_Absyn_Exp* _temp791; void* _temp793; struct Cyc_Absyn_Exp* _temp795; void*
_temp797; _LL767: _LL780: _temp779= _temp765.f1; if(( unsigned int) _temp779 > 1u?*((
int*) _temp779) == Cyc_Absyn_Upper_b: 0){ _LL782: _temp781=(( struct Cyc_Absyn_Upper_b_struct*)
_temp779)->f1; goto _LL776;} else{ goto _LL769;} _LL776: _temp775= _temp765.f2;
if(( unsigned int) _temp775 > 1u?*(( int*) _temp775) == Cyc_Absyn_Upper_b: 0){
_LL778: _temp777=(( struct Cyc_Absyn_Upper_b_struct*) _temp775)->f1; goto _LL768;}
else{ goto _LL769;} _LL769: _LL786: _temp785= _temp765.f1; if( _temp785 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL784;} else{ goto _LL771;} _LL784: _temp783=
_temp765.f2; if( _temp783 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL770;} else{
goto _LL771;} _LL771: _LL790: _temp789= _temp765.f1; if(( unsigned int) _temp789
> 1u?*(( int*) _temp789) == Cyc_Absyn_Upper_b: 0){ _LL792: _temp791=(( struct
Cyc_Absyn_Upper_b_struct*) _temp789)->f1; goto _LL788;} else{ goto _LL773;}
_LL788: _temp787= _temp765.f2; if( _temp787 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL772;} else{ goto _LL773;} _LL773: _LL798: _temp797= _temp765.f1; if(
_temp797 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL794;} else{ goto _LL766;}
_LL794: _temp793= _temp765.f2; if(( unsigned int) _temp793 > 1u?*(( int*)
_temp793) == Cyc_Absyn_Upper_b: 0){ _LL796: _temp795=(( struct Cyc_Absyn_Upper_b_struct*)
_temp793)->f1; goto _LL774;} else{ goto _LL766;} _LL768: if( _temp756? !
_temp758: 0){ return( void*)({ struct Cyc_Toc_NullCheck_struct* _temp799=(
struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp799[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp800; _temp800.tag= Cyc_Toc_NullCheck;
_temp800.f1=( void*) t2; _temp800;}); _temp799;});} return( void*) Cyc_Toc_NoConv;
_LL770: return( void*) Cyc_Toc_NoConv; _LL772: return( void*)({ struct Cyc_Toc_TagPtr_struct*
_temp801=( struct Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp801[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp802; _temp802.tag= Cyc_Toc_TagPtr;
_temp802.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp802.f2= _temp791; _temp802;});
_temp801;}); _LL774: return( void*)({ struct Cyc_Toc_UntagPtr_struct* _temp803=(
struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp803[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp804; _temp804.tag= Cyc_Toc_UntagPtr;
_temp804.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp804.f2= _temp795; _temp804.f3=
_temp758; _temp804;}); _temp803;}); _LL766:;}}}}} _LL747: return( void*) Cyc_Toc_NoConv;
_LL743:;}} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp*
e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__throw_qv, 0),({
struct Cyc_List_List* _temp805=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp805->hd=( void*) e; _temp805->tl= 0; _temp805;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ char* _temp806="can't do null-check conversion at top-level";
struct _tagged_string _temp807; _temp807.curr= _temp806; _temp807.base= _temp806;
_temp807.last_plus_one= _temp806 + 44; _temp807;}));} if( Cyc_Toc_use_runtimecheck_funs){
return Cyc_Absyn_cast_exp( new_typ, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp808=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp808->hd=( void*) e; _temp808->tl= 0; _temp808;}), 0),
0);} else{ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0),
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0),
Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
x, 0), 0), 0); return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, new_typ,(
struct Cyc_Absyn_Exp*) e, s, 0), 0);}} struct _tagged_ptr3{ struct _tuple4**
curr; struct _tuple4** base; struct _tuple4** last_plus_one; } ; static struct
Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct Cyc_Absyn_Exp*
sz, struct Cyc_Absyn_Exp* e){ struct _tuple0* a= Cyc_Toc_temp_var(); struct
_tuple0* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp809= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp815; struct Cyc_Absyn_Conref* _temp817; struct Cyc_Absyn_Tqual
_temp819; struct Cyc_Absyn_Conref* _temp821; void* _temp823; void* _temp825;
_LL811: if(( unsigned int) _temp809 > 4u?*(( int*) _temp809) == Cyc_Absyn_PointerType:
0){ _LL816: _temp815=(( struct Cyc_Absyn_PointerType_struct*) _temp809)->f1;
_LL826: _temp825=( void*) _temp815.elt_typ; goto _LL824; _LL824: _temp823=( void*)
_temp815.rgn_typ; goto _LL822; _LL822: _temp821= _temp815.nullable; goto _LL820;
_LL820: _temp819= _temp815.tq; goto _LL818; _LL818: _temp817= _temp815.bounds;
goto _LL812;} else{ goto _LL813;} _LL813: goto _LL814; _LL812: elt_typ= _temp825;
strt_typ= Cyc_Toc_add_tagged_ptr_type( _temp825, Cyc_Toc_mt_tq); goto _LL810;
_LL814: return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp827="tag_ptr_conv: not a pointer"; struct _tagged_string _temp828;
_temp828.curr= _temp827; _temp828.base= _temp827; _temp828.last_plus_one=
_temp827 + 28; _temp828;})); _LL810:;} if( toplevel){ int is_string= 0;{ void*
_temp829=( void*) e->r; void* _temp835; struct _tagged_string _temp837; _LL831:
if(*(( int*) _temp829) == Cyc_Absyn_Const_e){ _LL836: _temp835=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp829)->f1; if(( unsigned int) _temp835 > 1u?*((
int*) _temp835) == Cyc_Absyn_String_c: 0){ _LL838: _temp837=(( struct Cyc_Absyn_String_c_struct*)
_temp835)->f1; goto _LL832;} else{ goto _LL833;}} else{ goto _LL833;} _LL833:
goto _LL834; _LL832: is_string= 1; goto _LL830; _LL834: goto _LL830; _LL830:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp842=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp842[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp843; _temp843.tag= Cyc_Absyn_ArrayType; _temp843.f1=(
void*) elt_typ; _temp843.f2= Cyc_Toc_mt_tq; _temp843.f3=( struct Cyc_Absyn_Exp*)
sz; _temp843;}); _temp842;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp839=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp839->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp840=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp840[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp841; _temp841.tag= Cyc_Absyn_Var_d;
_temp841.f1= vd; _temp841;}); _temp840;}), 0); _temp839->tl= Cyc_Toc_result_decls;
_temp839;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List*(* _temp844)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)(
struct _tagged_ptr3)) Cyc_List_list; struct _tuple4* _temp847=({ struct _tuple4*
_temp852=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp852->f1= 0;
_temp852->f2= xexp; _temp852;}); struct _tuple4* _temp848=({ struct _tuple4*
_temp851=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp851->f1= 0;
_temp851->f2= xexp; _temp851;}); struct _tuple4* _temp849=({ struct _tuple4*
_temp850=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp850->f1= 0;
_temp850->f2= xplussz; _temp850;}); struct _tuple4* _temp845[ 3u]={ _temp847,
_temp848, _temp849}; struct _tagged_ptr3 _temp846={ _temp845, _temp845, _temp845
+ 3u}; _temp844( _temp846);}), 0); return urm_exp;}}} else{ struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( b, 0), Cyc_Toc_last_plus_one_sp,
0), Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp(
szvalue, 0), 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( b, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_var_exp( a, 0), 0), s,
0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( b, 0), Cyc_Toc_curr_sp, 0), Cyc_Absyn_var_exp( a, 0), 0), s,
0); s= Cyc_Absyn_declare_stmt( b, strt_typ, 0, s, 0); s= Cyc_Absyn_declare_stmt(
a, Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq),( struct Cyc_Absyn_Exp*) e, s, 0);
return Cyc_Absyn_stmt_exp( s, 0);}} static struct Cyc_Absyn_Exp* Cyc_Toc_untag_ptr_conv(
int toplevel, void* t1, struct Cyc_Absyn_Exp* sz, int nullable, struct Cyc_Absyn_Exp*
e){ if( toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ char*
_temp853="can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp854; _temp854.curr= _temp853; _temp854.base= _temp853; _temp854.last_plus_one=
_temp853 + 45; _temp854;}));}{ struct _tuple0* x= Cyc_Toc_temp_var(); int
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
_temp855=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct Cyc_Toc_Env));
_temp855->break_lab= 0; _temp855->continue_lab= 0; _temp855->fallthru_info= 0;
_temp855->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp855->toplevel= 1;
_temp855;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp856=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp856->break_lab= e->break_lab; _temp856->continue_lab=
e->continue_lab; _temp856->fallthru_info= e->fallthru_info; _temp856->varmap= e->varmap;
_temp856->toplevel= e->toplevel; _temp856;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp857=(* x).f1; struct
Cyc_List_List* _temp863; _LL859: if(( unsigned int) _temp857 > 1u?*(( int*)
_temp857) == Cyc_Absyn_Rel_n: 0){ _LL864: _temp863=(( struct Cyc_Absyn_Rel_n_struct*)
_temp857)->f1; goto _LL860;} else{ goto _LL861;} _LL861: goto _LL862; _LL860:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp865=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp865[ 0]=({ struct
Cyc_Core_Impossible_struct _temp866; _temp866.tag= Cyc_Core_Impossible; _temp866.f1=({
struct _tagged_string _temp867= Cyc_Absynpp_qvar2string( x); xprintf("Toc::add_varmap on Rel_n: %.*s\n",
_temp867.last_plus_one - _temp867.curr, _temp867.curr);}); _temp866;}); _temp865;}));
_LL862: goto _LL858; _LL858:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap, x, y); return
ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple8{ struct _tagged_string* f1; struct Cyc_List_List*
f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l, struct _tagged_string*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp868= 0; goto _LL869; _LL869: for( 0;
fallthru_binders != 0; fallthru_binders=(( struct Cyc_List_List*) _check_null(
fallthru_binders))->tl){ _temp868=({ struct Cyc_List_List* _temp870=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp870->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp870->tl=
_temp868; _temp870;});} _temp868=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp868);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp871=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp871->v=( void*) break_l; _temp871;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp872=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp872->v=( void*)({ struct _tuple8*
_temp873=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp873->f1=
fallthru_l; _temp873->f2= _temp868; _temp873->f3= next_case_env->varmap;
_temp873;}); _temp872;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp874=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp874->v=( void*)
break_l; _temp874;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_string*
next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp875=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp875->v=( void*)({ struct _tuple8* _temp876=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp876->f1= next_l; _temp876->f2= 0;
_temp876->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp876;}); _temp875;});
return ans;} struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; char f5; int f6; } ; struct
_tagged_ptr4{ struct Cyc_List_List** curr; struct Cyc_List_List** base; struct
Cyc_List_List** last_plus_one; } ; struct _tuple10{ struct _tuple0* f1; void* f2;
struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(({ char* _temp877="expecting a literal format string";
struct _tagged_string _temp878; _temp878.curr= _temp877; _temp878.base= _temp877;
_temp878.last_plus_one= _temp877 + 34; _temp878;}));}{ struct _tagged_string
fmt_str;{ void* _temp879=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd)->r; void* _temp885; struct _tagged_string _temp887;
_LL881: if(*(( int*) _temp879) == Cyc_Absyn_Const_e){ _LL886: _temp885=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp879)->f1; if(( unsigned int) _temp885 > 1u?*((
int*) _temp885) == Cyc_Absyn_String_c: 0){ _LL888: _temp887=(( struct Cyc_Absyn_String_c_struct*)
_temp885)->f1; goto _LL882;} else{ goto _LL883;}} else{ goto _LL883;} _LL883:
goto _LL884; _LL882: fmt_str= _temp887; goto _LL880; _LL884: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp889="expecting a literal format string";
struct _tagged_string _temp890; _temp890.curr= _temp889; _temp890.base= _temp889;
_temp890.last_plus_one= _temp889 + 34; _temp890;})); _LL880:;}{ int len=( int)({
struct _tagged_string _temp986= fmt_str;( unsigned int)( _temp986.last_plus_one
- _temp986.curr);}); int i= 0; for( i= 0; i < len; i ++){ if(*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
fmt_str, sizeof( char), i) =='%'){ i ++; if( i < len?*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str,
sizeof( char), i) =='s': 0){ break;}}} if( i == len){ if( aopt != 0){ args=({
struct Cyc_List_List* _temp891=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp891->hd=( void*)(( struct Cyc_Absyn_Exp*)(( struct
Cyc_Core_Opt*) _check_null( aopt))->v); _temp891->tl= args; _temp891;});} return
Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*) _check_null(
args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{ struct Cyc_Absyn_Stmt*
s;{ struct _RegionHandle _temp892= _new_region(); struct _RegionHandle* r=&
_temp892; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0; struct Cyc_List_List*
rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i= 0; i < len; i ++){
char c=*(( char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( fmt_str, sizeof( char), i); rev_fmt=({ struct Cyc_List_List*
_temp893=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp893->hd=( void*)(( int) c); _temp893->tl= rev_fmt; _temp893;}); if( c !='%'){
continue;}{ struct Cyc_Core_Opt* _temp894= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); goto _LL895; _LL895: if( _temp894 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp896="bad format string";
struct _tagged_string _temp897; _temp897.curr= _temp896; _temp897.base= _temp896;
_temp897.last_plus_one= _temp896 + 18; _temp897;}));}{ struct _tuple9 _temp900;
int _temp901; char _temp903; struct Cyc_List_List* _temp905; struct Cyc_List_List*
_temp907; struct Cyc_List_List* _temp909; struct Cyc_List_List* _temp911; struct
_tuple9* _temp898=( struct _tuple9*)(( struct Cyc_Core_Opt*) _check_null(
_temp894))->v; _temp900=* _temp898; _LL912: _temp911= _temp900.f1; goto _LL910;
_LL910: _temp909= _temp900.f2; goto _LL908; _LL908: _temp907= _temp900.f3; goto
_LL906; _LL906: _temp905= _temp900.f4; goto _LL904; _LL904: _temp903= _temp900.f5;
goto _LL902; _LL902: _temp901= _temp900.f6; goto _LL899; _LL899: i= _temp901 - 1;
if( _temp903 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp913)( struct _RegionHandle*, struct
_tagged_ptr4)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_ptr4)) Cyc_List_rlist; struct _RegionHandle* _temp920= r; struct Cyc_List_List*
_temp916= _temp911; struct Cyc_List_List* _temp917= _temp909; struct Cyc_List_List*
_temp918= _temp907; struct Cyc_List_List* _temp919= _temp905; struct Cyc_List_List*
_temp914[ 4u]={ _temp916, _temp917, _temp918, _temp919}; struct _tagged_ptr4
_temp915={ _temp914, _temp914, _temp914 + 4u}; _temp913( _temp920, _temp915);})),
rev_fmt); rev_fmt=({ struct Cyc_List_List* _temp921=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp921->hd=( void*)(( int)
_temp903); _temp921->tl= rev_fmt; _temp921;});{ struct Cyc_List_List* _temp922=
_temp909; struct Cyc_List_List _temp929; struct Cyc_List_List* _temp930; int
_temp932; _LL924: if( _temp922 == 0){ goto _LL926;} else{ _temp929=* _temp922;
_LL933: _temp932=( int) _temp929.hd; goto _LL931; _LL931: _temp930= _temp929.tl;
if( _temp930 == 0){ goto _LL928;} else{ goto _LL926;}} _LL928: if( _temp932 =='*'){
goto _LL925;} else{ goto _LL926;} _LL926: goto _LL927; _LL925: { struct _tuple0*
_temp934= Cyc_Toc_temp_var(); goto _LL935; _LL935: rev_temps=({ struct Cyc_List_List*
_temp936=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp936->hd=( void*)({ struct _tuple10* _temp937=( struct _tuple10*)
_region_malloc( r, sizeof( struct _tuple10)); _temp937->f1= _temp934; _temp937->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp937->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp937;});
_temp936->tl= rev_temps; _temp936;}); rev_result=({ struct Cyc_List_List*
_temp938=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp938->hd=( void*) Cyc_Absyn_var_exp( _temp934, 0); _temp938->tl= rev_result;
_temp938;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL923;} _LL927: goto
_LL923; _LL923:;}{ struct Cyc_List_List* _temp939= _temp907; struct Cyc_List_List
_temp946; struct Cyc_List_List* _temp947; struct Cyc_List_List _temp949; struct
Cyc_List_List* _temp950; int _temp952; int _temp954; _LL941: if( _temp939 == 0){
goto _LL943;} else{ _temp946=* _temp939; _LL955: _temp954=( int) _temp946.hd;
goto _LL948; _LL948: _temp947= _temp946.tl; if( _temp947 == 0){ goto _LL943;}
else{ _temp949=* _temp947; _LL953: _temp952=( int) _temp949.hd; goto _LL951;
_LL951: _temp950= _temp949.tl; if( _temp950 == 0){ goto _LL945;} else{ goto
_LL943;}}} _LL945: if( _temp954 =='.'? _temp952 =='*': 0){ goto _LL942;} else{
goto _LL943;} _LL943: goto _LL944; _LL942: { struct _tuple0* _temp956= Cyc_Toc_temp_var();
goto _LL957; _LL957: rev_temps=({ struct Cyc_List_List* _temp958=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp958->hd=( void*)({
struct _tuple10* _temp959=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp959->f1= _temp956; _temp959->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp959->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp959;}); _temp958->tl= rev_temps; _temp958;});
rev_result=({ struct Cyc_List_List* _temp960=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp960->hd=( void*) Cyc_Absyn_var_exp(
_temp956, 0); _temp960->tl= rev_result; _temp960;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL940;} _LL944: goto _LL940; _LL940:;} if( _temp903 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp961= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp961.last_plus_one - _temp961.curr, _temp961.curr);}));}{ struct _tuple0*
_temp962= Cyc_Toc_temp_var(); goto _LL963; _LL963: rev_temps=({ struct Cyc_List_List*
_temp964=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp964->hd=( void*)({ struct _tuple10* _temp965=( struct _tuple10*)
_region_malloc( r, sizeof( struct _tuple10)); _temp965->f1= _temp962; _temp965->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp965->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp965;});
_temp964->tl= rev_temps; _temp964;}); rev_result=({ struct Cyc_List_List*
_temp966=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp966->hd=( void*) Cyc_Absyn_var_exp( _temp962, 0); _temp966->tl= rev_result;
_temp966;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl;}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp967=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp967->hd=( void*)(( int)'.'); _temp967->tl= rev_fmt; _temp967;}); rev_fmt=({
struct Cyc_List_List* _temp968=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp968->hd=( void*)(( int)'*'); _temp968->tl=
rev_fmt; _temp968;}); rev_fmt=({ struct Cyc_List_List* _temp969=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp969->hd=( void*)(( int)'s');
_temp969->tl= rev_fmt; _temp969;});{ struct _tuple0* _temp970= Cyc_Toc_temp_var();
goto _LL971; _LL971: rev_temps=({ struct Cyc_List_List* _temp972=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp972->hd=( void*)({
struct _tuple10* _temp973=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp973->f1= _temp970; _temp973->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp973->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp973;}); _temp972->tl= rev_temps; _temp972;});{
struct Cyc_Absyn_Exp* _temp974= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp970, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp970, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL975; _LL975: {
struct Cyc_Absyn_Exp* _temp976= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp970, 0), Cyc_Toc_curr_sp, 0); goto _LL977; _LL977: rev_result=({ struct Cyc_List_List*
_temp978=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp978->hd=( void*) _temp974; _temp978->tl= rev_result; _temp978;});
rev_result=({ struct Cyc_List_List* _temp979=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp979->hd=( void*)
_temp976; _temp979->tl= rev_result; _temp979;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}}{
struct Cyc_List_List* _temp981=({ struct Cyc_List_List* _temp980=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp980->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp980->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( rev_result); _temp980;}); goto _LL982; _LL982: if( aopt != 0){
_temp981=({ struct Cyc_List_List* _temp983=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp983->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp983->tl= _temp981; _temp983;});}{
struct Cyc_Absyn_Exp* _temp984= Cyc_Absyn_primop_exp( p, _temp981, 0); goto
_LL985; _LL985: s= Cyc_Absyn_exp_stmt( _temp984, 0); for( 0; rev_temps != 0;
rev_temps=(( struct Cyc_List_List*) _check_null( rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f1,(*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f2,(
struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f3, s, 0);}}}}; _pop_region(& _temp892);} return
Cyc_Absyn_stmt_exp( s, 0);}}}} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp987="Missing type in primop "; struct _tagged_string _temp988;
_temp988.curr= _temp987; _temp988.base= _temp987; _temp988.last_plus_one=
_temp987 + 24; _temp988;}));} return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp989="Missing type in primop "; struct _tagged_string _temp990;
_temp990.curr= _temp989; _temp990.base= _temp989; _temp990.last_plus_one=
_temp989 + 24; _temp990;}));} return( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp* e){
return({ struct _tuple3* _temp991=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp991->f1= Cyc_Toc_mt_tq; _temp991->f2= Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp991;});} static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e); static struct _tuple4* Cyc_Toc_add_designator(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e);
return({ struct _tuple4* _temp992=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4) * 1); _temp992[ 0]=({ struct _tuple4 _temp993; _temp993.f1= 0; _temp993.f2=
e; _temp993;}); _temp992;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct(
struct Cyc_Toc_Env* nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt*
s, int pointer, struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp*
eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{
struct Cyc_Absyn_Exp* _temp994= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); goto
_LL995; _LL995: if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp994): Cyc_Toc_malloc_ptr( _temp994), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp994), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp996=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); goto _LL997; _LL997: for( 0; _temp996
!= 0; _temp996=(( struct Cyc_List_List*) _check_null( _temp996))->tl){ struct
_tuple4 _temp1000; struct Cyc_Absyn_Exp* _temp1001; struct Cyc_List_List*
_temp1003; struct _tuple4* _temp998=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp996))->hd; _temp1000=* _temp998; _LL1004: _temp1003= _temp1000.f1;
goto _LL1002; _LL1002: _temp1001= _temp1000.f2; goto _LL999; _LL999: { struct
Cyc_Absyn_Exp* e_index; if( _temp1003 == 0){ e_index= Cyc_Absyn_signed_int_exp(
count --, 0);} else{ if((( struct Cyc_List_List*) _check_null( _temp1003))->tl
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ char* _temp1005="multiple designators in array";
struct _tagged_string _temp1006; _temp1006.curr= _temp1005; _temp1006.base=
_temp1005; _temp1006.last_plus_one= _temp1005 + 30; _temp1006;}));}{ void*
_temp1007=( void*)(( struct Cyc_List_List*) _check_null( _temp1003))->hd; goto
_LL1008; _LL1008: { void* _temp1009= _temp1007; struct Cyc_Absyn_Exp* _temp1015;
struct _tagged_string* _temp1017; _LL1011: if(*(( int*) _temp1009) == Cyc_Absyn_ArrayElement){
_LL1016: _temp1015=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1009)->f1;
goto _LL1012;} else{ goto _LL1013;} _LL1013: if(*(( int*) _temp1009) == Cyc_Absyn_FieldName){
_LL1018: _temp1017=(( struct Cyc_Absyn_FieldName_struct*) _temp1009)->f1; goto
_LL1014;} else{ goto _LL1010;} _LL1012: Cyc_Toc_exp_to_c( nv, _temp1015);
e_index= _temp1015; goto _LL1010; _LL1014: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ char* _temp1019="field name designators in array";
struct _tagged_string _temp1020; _temp1020.curr= _temp1019; _temp1020.base=
_temp1019; _temp1020.last_plus_one= _temp1019 + 32; _temp1020;})); goto _LL1010;
_LL1010:;}}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index,
0); void* _temp1021=( void*) _temp1001->r; struct Cyc_List_List* _temp1031;
struct Cyc_Absyn_Exp* _temp1033; struct Cyc_Absyn_Exp* _temp1035; struct Cyc_Absyn_Vardecl*
_temp1037; struct Cyc_List_List* _temp1039; void* _temp1041; _LL1023: if(*(( int*)
_temp1021) == Cyc_Absyn_Array_e){ _LL1032: _temp1031=(( struct Cyc_Absyn_Array_e_struct*)
_temp1021)->f1; goto _LL1024;} else{ goto _LL1025;} _LL1025: if(*(( int*)
_temp1021) == Cyc_Absyn_Comprehension_e){ _LL1038: _temp1037=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1021)->f1; goto _LL1036; _LL1036: _temp1035=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1021)->f2; goto _LL1034; _LL1034: _temp1033=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1021)->f3; goto _LL1026;} else{ goto _LL1027;} _LL1027: if(*(( int*)
_temp1021) == Cyc_Absyn_AnonStruct_e){ _LL1042: _temp1041=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1021)->f1; goto _LL1040; _LL1040: _temp1039=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1021)->f2; goto _LL1028;} else{ goto _LL1029;} _LL1029: goto _LL1030;
_LL1024: s= Cyc_Toc_init_array( nv, lval, _temp1031, s); goto _LL1022; _LL1026:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1037, _temp1035, _temp1033, s, 0);
goto _LL1022; _LL1028: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1041,
_temp1039, s); goto _LL1022; _LL1030: Cyc_Toc_exp_to_c( nv, _temp1001); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp1001, 0),
s, 0); goto _LL1022; _LL1022:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* _temp1043= vd->name; goto _LL1044;
_LL1044: if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp1043, Cyc_Absyn_varb_exp( _temp1043,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1067=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1067[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1068; _temp1068.tag= Cyc_Absyn_Local_b; _temp1068.f1=
vd; _temp1068;}); _temp1067;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp1043, 0),
Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( _temp1043, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp*
ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( _temp1043, 0), 0); struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( _temp1043, 0), 0); struct
Cyc_Absyn_Stmt* body;{ void* _temp1045=( void*) e2->r; struct Cyc_List_List*
_temp1055; struct Cyc_Absyn_Exp* _temp1057; struct Cyc_Absyn_Exp* _temp1059;
struct Cyc_Absyn_Vardecl* _temp1061; struct Cyc_List_List* _temp1063; void*
_temp1065; _LL1047: if(*(( int*) _temp1045) == Cyc_Absyn_Array_e){ _LL1056:
_temp1055=(( struct Cyc_Absyn_Array_e_struct*) _temp1045)->f1; goto _LL1048;}
else{ goto _LL1049;} _LL1049: if(*(( int*) _temp1045) == Cyc_Absyn_Comprehension_e){
_LL1062: _temp1061=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1045)->f1;
goto _LL1060; _LL1060: _temp1059=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1045)->f2; goto _LL1058; _LL1058: _temp1057=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1045)->f3; goto _LL1050;} else{ goto _LL1051;} _LL1051: if(*(( int*)
_temp1045) == Cyc_Absyn_AnonStruct_e){ _LL1066: _temp1065=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1045)->f1; goto _LL1064; _LL1064: _temp1063=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1045)->f2; goto _LL1052;} else{ goto _LL1053;} _LL1053: goto _LL1054;
_LL1048: body= Cyc_Toc_init_array( nv2, lval, _temp1055, Cyc_Absyn_skip_stmt( 0));
goto _LL1046; _LL1050: body= Cyc_Toc_init_comprehension( nv2, lval, _temp1061,
_temp1059, _temp1057, Cyc_Absyn_skip_stmt( 0), 0); goto _LL1046; _LL1052: body=
Cyc_Toc_init_anon_struct( nv, lval, _temp1065, _temp1063, Cyc_Absyn_skip_stmt( 0));
goto _LL1046; _LL1054: Cyc_Toc_exp_to_c( nv2, e2); body= Cyc_Absyn_assign_stmt(
lval, e2, 0); goto _LL1046; _LL1046:;}{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt(
ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( _temp1043, Cyc_Absyn_uint_t,
0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s){{ struct Cyc_List_List* _temp1069=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto _LL1070; _LL1070: for( 0;
_temp1069 != 0; _temp1069=(( struct Cyc_List_List*) _check_null( _temp1069))->tl){
struct _tuple4 _temp1073; struct Cyc_Absyn_Exp* _temp1074; struct Cyc_List_List*
_temp1076; struct _tuple4* _temp1071=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1069))->hd; _temp1073=* _temp1071; _LL1077: _temp1076=
_temp1073.f1; goto _LL1075; _LL1075: _temp1074= _temp1073.f2; goto _LL1072;
_LL1072: if( _temp1076 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp1078="empty designator list"; struct _tagged_string _temp1079;
_temp1079.curr= _temp1078; _temp1079.base= _temp1078; _temp1079.last_plus_one=
_temp1078 + 22; _temp1079;}));} if((( struct Cyc_List_List*) _check_null(
_temp1076))->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp1080="too many designators in anonymous struct"; struct
_tagged_string _temp1081; _temp1081.curr= _temp1080; _temp1081.base= _temp1080;
_temp1081.last_plus_one= _temp1080 + 41; _temp1081;}));}{ void* _temp1082=( void*)((
struct Cyc_List_List*) _check_null( _temp1076))->hd; struct _tagged_string*
_temp1088; _LL1084: if(*(( int*) _temp1082) == Cyc_Absyn_FieldName){ _LL1089:
_temp1088=(( struct Cyc_Absyn_FieldName_struct*) _temp1082)->f1; goto _LL1085;}
else{ goto _LL1086;} _LL1086: goto _LL1087; _LL1085: { struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_structmember_exp( lhs, _temp1088, 0);{ void* _temp1090=( void*)
_temp1074->r; struct Cyc_List_List* _temp1100; struct Cyc_Absyn_Exp* _temp1102;
struct Cyc_Absyn_Exp* _temp1104; struct Cyc_Absyn_Vardecl* _temp1106; struct Cyc_List_List*
_temp1108; void* _temp1110; _LL1092: if(*(( int*) _temp1090) == Cyc_Absyn_Array_e){
_LL1101: _temp1100=(( struct Cyc_Absyn_Array_e_struct*) _temp1090)->f1; goto
_LL1093;} else{ goto _LL1094;} _LL1094: if(*(( int*) _temp1090) == Cyc_Absyn_Comprehension_e){
_LL1107: _temp1106=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1090)->f1;
goto _LL1105; _LL1105: _temp1104=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1090)->f2; goto _LL1103; _LL1103: _temp1102=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1090)->f3; goto _LL1095;} else{ goto _LL1096;} _LL1096: if(*(( int*)
_temp1090) == Cyc_Absyn_AnonStruct_e){ _LL1111: _temp1110=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1090)->f1; goto _LL1109; _LL1109: _temp1108=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1090)->f2; goto _LL1097;} else{ goto _LL1098;} _LL1098: goto _LL1099;
_LL1093: s= Cyc_Toc_init_array( nv, lval, _temp1100, s); goto _LL1091; _LL1095:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1106, _temp1104, _temp1102, s, 0);
goto _LL1091; _LL1097: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1110,
_temp1108, s); goto _LL1091; _LL1099: Cyc_Toc_exp_to_c( nv, _temp1074); if( Cyc_Toc_is_poly_field(
struct_type, _temp1088)){ _temp1074= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1074, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1074, 0), 0), s, 0); goto _LL1091; _LL1091:;} goto _LL1083;} _LL1087:(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp1112="array designator in struct"; struct _tagged_string _temp1113;
_temp1113.curr= _temp1112; _temp1113.base= _temp1112; _temp1113.last_plus_one=
_temp1112 + 27; _temp1113;}))); _LL1083:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp1114= _new_region();
struct _RegionHandle* r=& _temp1114; _push_region( r);{ struct Cyc_List_List*
_temp1115=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(*
f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); goto _LL1116; _LL1116: { struct _tagged_string* _temp1117= Cyc_Toc_add_tuple_type(
_temp1115); goto _LL1118; _LL1118: { struct _tuple0* _temp1119= Cyc_Toc_temp_var();
goto _LL1120; _LL1120: { struct Cyc_Absyn_Exp* _temp1121= Cyc_Absyn_var_exp(
_temp1119, 0); goto _LL1122; _LL1122: { struct Cyc_Absyn_Stmt* _temp1123= Cyc_Absyn_exp_stmt(
_temp1121, 0); goto _LL1124; _LL1124: { struct Cyc_Absyn_Exp*(* _temp1125)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1126;
_LL1126: { int is_atomic= 1; struct Cyc_List_List* _temp1127=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es); goto
_LL1128; _LL1128:{ int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1127); for( 0; _temp1127 != 0;( _temp1127=(( struct Cyc_List_List*)
_check_null( _temp1127))->tl, -- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1127))->hd; struct Cyc_Absyn_Exp* lval=
_temp1125( _temp1121, Cyc_Toc_fieldname( i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v): 0;{ void* _temp1129=(
void*) e->r; struct Cyc_List_List* _temp1137; struct Cyc_Absyn_Exp* _temp1139;
struct Cyc_Absyn_Exp* _temp1141; struct Cyc_Absyn_Vardecl* _temp1143; _LL1131:
if(*(( int*) _temp1129) == Cyc_Absyn_Array_e){ _LL1138: _temp1137=(( struct Cyc_Absyn_Array_e_struct*)
_temp1129)->f1; goto _LL1132;} else{ goto _LL1133;} _LL1133: if(*(( int*)
_temp1129) == Cyc_Absyn_Comprehension_e){ _LL1144: _temp1143=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1129)->f1; goto _LL1142; _LL1142: _temp1141=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1129)->f2; goto _LL1140; _LL1140: _temp1139=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1129)->f3; goto _LL1134;} else{ goto _LL1135;} _LL1135: goto _LL1136;
_LL1132: _temp1123= Cyc_Toc_init_array( nv, lval, _temp1137, _temp1123); goto
_LL1130; _LL1134: _temp1123= Cyc_Toc_init_comprehension( nv, lval, _temp1143,
_temp1141, _temp1139, _temp1123, 0); goto _LL1130; _LL1136: Cyc_Toc_exp_to_c( nv,
e); _temp1123= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1125( _temp1121, Cyc_Toc_fieldname( i), 0), e, 0), 0), _temp1123, 0); goto
_LL1130; _LL1130:;}}}{ struct Cyc_Absyn_Exp* _temp1145= Cyc_Toc_make_struct( nv,
_temp1119, Cyc_Absyn_strct( _temp1117), _temp1123, pointer, rgnopt, is_atomic);
_npop_handler( 0u); return _temp1145;}}}}}}}}; _pop_region(& _temp1114);} static
struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env* nv, void*
struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp1146= Cyc_Toc_temp_var(); goto
_LL1147; _LL1147: { struct Cyc_Absyn_Exp* _temp1148= Cyc_Absyn_var_exp(
_temp1146, 0); goto _LL1149; _LL1149: { struct Cyc_Absyn_Stmt* _temp1150= Cyc_Absyn_exp_stmt(
_temp1148, 0); goto _LL1151; _LL1151: { struct Cyc_Absyn_Exp*(* _temp1152)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1153;
_LL1153: { int is_atomic= 1; struct _RegionHandle _temp1154= _new_region();
struct _RegionHandle* r=& _temp1154; _push_region( r);{ struct Cyc_List_List*
_temp1155=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, dles); goto _LL1156; _LL1156: for( 0; _temp1155 != 0;
_temp1155=(( struct Cyc_List_List*) _check_null( _temp1155))->tl){ struct
_tuple4 _temp1159; struct Cyc_Absyn_Exp* _temp1160; struct Cyc_List_List*
_temp1162; struct _tuple4* _temp1157=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1155))->hd; _temp1159=* _temp1157; _LL1163: _temp1162=
_temp1159.f1; goto _LL1161; _LL1161: _temp1160= _temp1159.f2; goto _LL1158;
_LL1158: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1160->topt))->v): 0; if( _temp1162 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp1164="empty designator list";
struct _tagged_string _temp1165; _temp1165.curr= _temp1164; _temp1165.base=
_temp1164; _temp1165.last_plus_one= _temp1164 + 22; _temp1165;}));} if((( struct
Cyc_List_List*) _check_null( _temp1162))->tl != 0){ struct _tuple0* _temp1166=
Cyc_Toc_temp_var(); goto _LL1167; _LL1167: { struct Cyc_Absyn_Exp* _temp1168=
Cyc_Absyn_var_exp( _temp1166, 0); goto _LL1169; _LL1169: for( 0; _temp1162 != 0;
_temp1162=(( struct Cyc_List_List*) _check_null( _temp1162))->tl){ void*
_temp1170=( void*)(( struct Cyc_List_List*) _check_null( _temp1162))->hd; struct
_tagged_string* _temp1176; _LL1172: if(*(( int*) _temp1170) == Cyc_Absyn_FieldName){
_LL1177: _temp1176=(( struct Cyc_Absyn_FieldName_struct*) _temp1170)->f1; goto
_LL1173;} else{ goto _LL1174;} _LL1174: goto _LL1175; _LL1173: if( Cyc_Toc_is_poly_field(
struct_type, _temp1176)){ _temp1168= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1168, 0);} _temp1150= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1152( _temp1148, _temp1176, 0), _temp1168, 0), 0), _temp1150, 0); goto
_LL1171; _LL1175:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp1178="array designator in struct"; struct _tagged_string _temp1179;
_temp1179.curr= _temp1178; _temp1179.base= _temp1178; _temp1179.last_plus_one=
_temp1178 + 27; _temp1179;}))); _LL1171:;} Cyc_Toc_exp_to_c( nv, _temp1160);
_temp1150= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1168, _temp1160, 0), 0), _temp1150, 0);}} else{ void* _temp1180=( void*)((
struct Cyc_List_List*) _check_null( _temp1162))->hd; struct _tagged_string*
_temp1186; _LL1182: if(*(( int*) _temp1180) == Cyc_Absyn_FieldName){ _LL1187:
_temp1186=(( struct Cyc_Absyn_FieldName_struct*) _temp1180)->f1; goto _LL1183;}
else{ goto _LL1184;} _LL1184: goto _LL1185; _LL1183: { struct Cyc_Absyn_Exp*
lval= _temp1152( _temp1148, _temp1186, 0);{ void* _temp1188=( void*) _temp1160->r;
struct Cyc_List_List* _temp1198; struct Cyc_Absyn_Exp* _temp1200; struct Cyc_Absyn_Exp*
_temp1202; struct Cyc_Absyn_Vardecl* _temp1204; struct Cyc_List_List* _temp1206;
void* _temp1208; _LL1190: if(*(( int*) _temp1188) == Cyc_Absyn_Array_e){ _LL1199:
_temp1198=(( struct Cyc_Absyn_Array_e_struct*) _temp1188)->f1; goto _LL1191;}
else{ goto _LL1192;} _LL1192: if(*(( int*) _temp1188) == Cyc_Absyn_Comprehension_e){
_LL1205: _temp1204=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1188)->f1;
goto _LL1203; _LL1203: _temp1202=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1188)->f2; goto _LL1201; _LL1201: _temp1200=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1188)->f3; goto _LL1193;} else{ goto _LL1194;} _LL1194: if(*(( int*)
_temp1188) == Cyc_Absyn_AnonStruct_e){ _LL1209: _temp1208=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1188)->f1; goto _LL1207; _LL1207: _temp1206=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1188)->f2; goto _LL1195;} else{ goto _LL1196;} _LL1196: goto _LL1197;
_LL1191: _temp1150= Cyc_Toc_init_array( nv, lval, _temp1198, _temp1150); goto
_LL1189; _LL1193: _temp1150= Cyc_Toc_init_comprehension( nv, lval, _temp1204,
_temp1202, _temp1200, _temp1150, 0); goto _LL1189; _LL1195: _temp1150= Cyc_Toc_init_anon_struct(
nv, lval, _temp1208, _temp1206, _temp1150); goto _LL1189; _LL1197: Cyc_Toc_exp_to_c(
nv, _temp1160); if( Cyc_Toc_is_poly_field( struct_type, _temp1186)){ _temp1160=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1160, 0);} _temp1150= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval, _temp1160, 0), 0), _temp1150, 0);
goto _LL1189; _LL1189:;} goto _LL1181;} _LL1185:( void) _throw((( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp1210="array designator in struct";
struct _tagged_string _temp1211; _temp1211.curr= _temp1210; _temp1211.base=
_temp1210; _temp1211.last_plus_one= _temp1210 + 27; _temp1211;}))); _LL1181:;}}}{
struct Cyc_Absyn_Exp* _temp1212= Cyc_Toc_make_struct( nv, _temp1146, Cyc_Absyn_strctq(
tdn), _temp1150, pointer, rgnopt, is_atomic); _npop_handler( 0u); return
_temp1212;}; _pop_region(& _temp1154);}}}}} struct _tuple11{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp(
el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp1213=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp1213[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp1214; _temp1214.tag=
Cyc_Absyn_Increment_e; _temp1214.f1= e1; _temp1214.f2=( void*) incr; _temp1214;});
_temp1213;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1215=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp1225; struct Cyc_Absyn_Exp* _temp1227; void* _temp1229; struct
_tagged_string* _temp1231; struct Cyc_Absyn_Exp* _temp1233; _LL1217: if(*(( int*)
_temp1215) == Cyc_Absyn_StmtExp_e){ _LL1226: _temp1225=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1215)->f1; goto _LL1218;} else{ goto _LL1219;} _LL1219: if(*(( int*)
_temp1215) == Cyc_Absyn_Cast_e){ _LL1230: _temp1229=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1215)->f1; goto _LL1228; _LL1228: _temp1227=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1215)->f2; goto _LL1220;} else{ goto _LL1221;} _LL1221: if(*(( int*)
_temp1215) == Cyc_Absyn_StructMember_e){ _LL1234: _temp1233=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1215)->f1; goto _LL1232; _LL1232: _temp1231=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1215)->f2; goto _LL1222;} else{ goto _LL1223;} _LL1223: goto _LL1224;
_LL1218:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1225, fs, f, f_env); goto _LL1216; _LL1220:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1227, fs, f, f_env); goto
_LL1216; _LL1222:( void*)( e1->r=( void*)(( void*) _temp1233->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1235=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1235->hd=( void*) _temp1231; _temp1235->tl= fs;
_temp1235;}), f, f_env); goto _LL1216; _LL1224: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1236= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1236.last_plus_one - _temp1236.curr, _temp1236.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1216;}
_LL1216:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1237=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1247; struct Cyc_Absyn_Stmt* _temp1249; struct Cyc_Absyn_Decl* _temp1251;
struct Cyc_Absyn_Stmt* _temp1253; struct Cyc_Absyn_Stmt* _temp1255; _LL1239: if((
unsigned int) _temp1237 > 1u?*(( int*) _temp1237) == Cyc_Absyn_Exp_s: 0){
_LL1248: _temp1247=(( struct Cyc_Absyn_Exp_s_struct*) _temp1237)->f1; goto
_LL1240;} else{ goto _LL1241;} _LL1241: if(( unsigned int) _temp1237 > 1u?*((
int*) _temp1237) == Cyc_Absyn_Decl_s: 0){ _LL1252: _temp1251=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1237)->f1; goto _LL1250; _LL1250: _temp1249=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1237)->f2; goto _LL1242;} else{ goto _LL1243;} _LL1243: if(( unsigned int)
_temp1237 > 1u?*(( int*) _temp1237) == Cyc_Absyn_Seq_s: 0){ _LL1256: _temp1255=((
struct Cyc_Absyn_Seq_s_struct*) _temp1237)->f1; goto _LL1254; _LL1254: _temp1253=((
struct Cyc_Absyn_Seq_s_struct*) _temp1237)->f2; goto _LL1244;} else{ goto
_LL1245;} _LL1245: goto _LL1246; _LL1240:(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1247, fs, f, f_env); goto
_LL1238; _LL1242:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1249, fs, f, f_env); goto _LL1238; _LL1244:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1253, fs, f, f_env);
goto _LL1238; _LL1246:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1257= Cyc_Absynpp_stmt2string( s); xprintf("lvalue_assign_stmt: %.*s",
_temp1257.last_plus_one - _temp1257.curr, _temp1257.curr);})); goto _LL1238;
_LL1238:;} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
s); static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct _RegionHandle* r2, void*(*
f)( void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List*
result; struct Cyc_List_List* prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List*
_temp1258=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp1258->hd=( void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,
env); _temp1258->tl= 0; _temp1258;}); prev= result; for( x=(( struct Cyc_List_List*)
_check_null( x))->tl; x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){((
struct Cyc_List_List*) _check_null( prev))->tl=({ struct Cyc_List_List*
_temp1259=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp1259->hd=( void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,
env); _temp1259->tl= 0; _temp1259;}); prev=(( struct Cyc_List_List*) _check_null(
prev))->tl;} return result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(*
f)( void*, void*), void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4*
Cyc_Toc_make_dle( struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1260=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1260->f1= 0;
_temp1260->f2= e; _temp1260;});} struct _tagged_ptr5{ struct _tuple0** curr;
struct _tuple0** base; struct _tuple0** last_plus_one; } ; struct _tagged_ptr6{
struct Cyc_Absyn_Exp** curr; struct Cyc_Absyn_Exp** base; struct Cyc_Absyn_Exp**
last_plus_one; } ; struct _tagged_ptr7{ struct _tuple1** curr; struct _tuple1**
base; struct _tuple1** last_plus_one; } ; struct _tuple12{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp1261=( void*) e->r; goto _LL1262;
_LL1262: if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1263= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp1263.last_plus_one - _temp1263.curr, _temp1263.curr);})); return;}{ void*
old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void*
_temp1264= _temp1261; void* _temp1342; void* _temp1344; void* _temp1346; struct
_tuple0* _temp1348; struct _tuple0* _temp1350; struct Cyc_List_List* _temp1352;
void* _temp1354; void* _temp1356; struct Cyc_Absyn_Exp* _temp1358; struct Cyc_Absyn_Exp*
_temp1360; struct Cyc_Core_Opt* _temp1362; struct Cyc_Absyn_Exp* _temp1364;
struct Cyc_Absyn_Exp* _temp1366; struct Cyc_Absyn_Exp* _temp1368; struct Cyc_Absyn_Exp*
_temp1370; struct Cyc_Absyn_Exp* _temp1372; struct Cyc_Absyn_Exp* _temp1374;
struct Cyc_List_List* _temp1376; struct Cyc_Absyn_Exp* _temp1378; struct Cyc_Absyn_VarargCallInfo*
_temp1380; struct Cyc_List_List* _temp1382; struct Cyc_Absyn_Exp* _temp1384;
struct Cyc_Absyn_VarargCallInfo* _temp1386; struct Cyc_Absyn_VarargCallInfo
_temp1388; struct Cyc_Absyn_VarargInfo* _temp1389; struct Cyc_List_List*
_temp1391; int _temp1393; struct Cyc_List_List* _temp1395; struct Cyc_Absyn_Exp*
_temp1397; struct Cyc_Absyn_Exp* _temp1399; struct Cyc_Absyn_Exp* _temp1401;
struct Cyc_List_List* _temp1403; struct Cyc_Absyn_Exp* _temp1405; struct Cyc_Absyn_Exp*
_temp1407; void* _temp1409; void** _temp1411; struct Cyc_Absyn_Exp* _temp1412;
struct Cyc_Absyn_Exp* _temp1414; struct Cyc_Absyn_Exp* _temp1416; void*
_temp1418; struct Cyc_Absyn_Exp* _temp1420; struct Cyc_Absyn_Exp* _temp1422;
struct _tagged_string* _temp1424; struct Cyc_Absyn_Exp* _temp1426; struct
_tagged_string* _temp1428; struct Cyc_Absyn_Exp* _temp1430; struct Cyc_Absyn_Exp*
_temp1432; struct Cyc_Absyn_Exp* _temp1434; struct Cyc_List_List* _temp1436;
struct Cyc_List_List* _temp1438; struct _tuple1* _temp1440; struct Cyc_List_List*
_temp1442; struct Cyc_Absyn_Exp* _temp1444; struct Cyc_Absyn_Exp* _temp1446;
struct Cyc_Absyn_Vardecl* _temp1448; struct Cyc_Absyn_Structdecl* _temp1450;
struct Cyc_List_List* _temp1452; struct Cyc_Core_Opt* _temp1454; struct _tuple0*
_temp1456; struct Cyc_List_List* _temp1458; void* _temp1460; struct Cyc_Absyn_Tunionfield*
_temp1462; struct Cyc_Absyn_Tuniondecl* _temp1464; struct Cyc_List_List*
_temp1466; struct Cyc_Core_Opt* _temp1468; struct Cyc_Core_Opt* _temp1470;
struct Cyc_Absyn_Tunionfield* _temp1472; struct Cyc_Absyn_Tuniondecl* _temp1474;
struct Cyc_List_List* _temp1476; struct Cyc_Core_Opt* _temp1478; struct Cyc_Core_Opt*
_temp1480; struct Cyc_Absyn_Enumfield* _temp1482; struct Cyc_Absyn_Enumdecl*
_temp1484; struct _tuple0* _temp1486; void* _temp1488; struct Cyc_Absyn_Exp*
_temp1490; struct Cyc_Absyn_Stmt* _temp1492; struct Cyc_List_List* _temp1494;
struct Cyc_Core_Opt* _temp1496; struct Cyc_Absyn_Fndecl* _temp1498; struct Cyc_Absyn_Exp*
_temp1500; _LL1266: if(*(( int*) _temp1264) == Cyc_Absyn_Const_e){ _LL1343:
_temp1342=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1264)->f1; if(
_temp1342 ==( void*) Cyc_Absyn_Null_c){ goto _LL1267;} else{ goto _LL1268;}}
else{ goto _LL1268;} _LL1268: if(*(( int*) _temp1264) == Cyc_Absyn_Const_e){
_LL1345: _temp1344=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1264)->f1;
goto _LL1269;} else{ goto _LL1270;} _LL1270: if(*(( int*) _temp1264) == Cyc_Absyn_Var_e){
_LL1349: _temp1348=(( struct Cyc_Absyn_Var_e_struct*) _temp1264)->f1; goto
_LL1347; _LL1347: _temp1346=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1264)->f2;
goto _LL1271;} else{ goto _LL1272;} _LL1272: if(*(( int*) _temp1264) == Cyc_Absyn_UnknownId_e){
_LL1351: _temp1350=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1264)->f1; goto
_LL1273;} else{ goto _LL1274;} _LL1274: if(*(( int*) _temp1264) == Cyc_Absyn_Primop_e){
_LL1355: _temp1354=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1264)->f1;
goto _LL1353; _LL1353: _temp1352=(( struct Cyc_Absyn_Primop_e_struct*) _temp1264)->f2;
goto _LL1275;} else{ goto _LL1276;} _LL1276: if(*(( int*) _temp1264) == Cyc_Absyn_Increment_e){
_LL1359: _temp1358=(( struct Cyc_Absyn_Increment_e_struct*) _temp1264)->f1; goto
_LL1357; _LL1357: _temp1356=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1264)->f2; goto _LL1277;} else{ goto _LL1278;} _LL1278: if(*(( int*)
_temp1264) == Cyc_Absyn_AssignOp_e){ _LL1365: _temp1364=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1264)->f1; goto _LL1363; _LL1363: _temp1362=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1264)->f2; goto _LL1361; _LL1361: _temp1360=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1264)->f3; goto _LL1279;} else{ goto _LL1280;} _LL1280: if(*(( int*)
_temp1264) == Cyc_Absyn_Conditional_e){ _LL1371: _temp1370=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1264)->f1; goto _LL1369; _LL1369: _temp1368=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1264)->f2; goto _LL1367; _LL1367: _temp1366=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1264)->f3; goto _LL1281;} else{ goto _LL1282;} _LL1282: if(*(( int*)
_temp1264) == Cyc_Absyn_SeqExp_e){ _LL1375: _temp1374=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1264)->f1; goto _LL1373; _LL1373: _temp1372=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1264)->f2; goto _LL1283;} else{ goto _LL1284;} _LL1284: if(*(( int*)
_temp1264) == Cyc_Absyn_UnknownCall_e){ _LL1379: _temp1378=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1264)->f1; goto _LL1377; _LL1377: _temp1376=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1264)->f2; goto _LL1285;} else{ goto _LL1286;} _LL1286: if(*(( int*)
_temp1264) == Cyc_Absyn_FnCall_e){ _LL1385: _temp1384=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1264)->f1; goto _LL1383; _LL1383: _temp1382=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1264)->f2; goto _LL1381; _LL1381: _temp1380=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1264)->f3; if( _temp1380 == 0){ goto _LL1287;} else{ goto _LL1288;}} else{
goto _LL1288;} _LL1288: if(*(( int*) _temp1264) == Cyc_Absyn_FnCall_e){ _LL1398:
_temp1397=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1264)->f1; goto _LL1396;
_LL1396: _temp1395=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1264)->f2; goto
_LL1387; _LL1387: _temp1386=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1264)->f3;
if( _temp1386 == 0){ goto _LL1290;} else{ _temp1388=* _temp1386; _LL1394:
_temp1393= _temp1388.num_varargs; goto _LL1392; _LL1392: _temp1391= _temp1388.injectors;
goto _LL1390; _LL1390: _temp1389= _temp1388.vai; goto _LL1289;}} else{ goto
_LL1290;} _LL1290: if(*(( int*) _temp1264) == Cyc_Absyn_Throw_e){ _LL1400:
_temp1399=(( struct Cyc_Absyn_Throw_e_struct*) _temp1264)->f1; goto _LL1291;}
else{ goto _LL1292;} _LL1292: if(*(( int*) _temp1264) == Cyc_Absyn_NoInstantiate_e){
_LL1402: _temp1401=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1264)->f1;
goto _LL1293;} else{ goto _LL1294;} _LL1294: if(*(( int*) _temp1264) == Cyc_Absyn_Instantiate_e){
_LL1406: _temp1405=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1264)->f1;
goto _LL1404; _LL1404: _temp1403=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1264)->f2; goto _LL1295;} else{ goto _LL1296;} _LL1296: if(*(( int*)
_temp1264) == Cyc_Absyn_Cast_e){ _LL1410: _temp1409=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1264)->f1; _temp1411=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1264)->f1); goto _LL1408; _LL1408: _temp1407=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1264)->f2; goto _LL1297;} else{ goto _LL1298;} _LL1298: if(*(( int*)
_temp1264) == Cyc_Absyn_Address_e){ _LL1413: _temp1412=(( struct Cyc_Absyn_Address_e_struct*)
_temp1264)->f1; goto _LL1299;} else{ goto _LL1300;} _LL1300: if(*(( int*)
_temp1264) == Cyc_Absyn_New_e){ _LL1417: _temp1416=(( struct Cyc_Absyn_New_e_struct*)
_temp1264)->f1; goto _LL1415; _LL1415: _temp1414=(( struct Cyc_Absyn_New_e_struct*)
_temp1264)->f2; goto _LL1301;} else{ goto _LL1302;} _LL1302: if(*(( int*)
_temp1264) == Cyc_Absyn_Sizeoftyp_e){ _LL1419: _temp1418=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1264)->f1; goto _LL1303;} else{ goto _LL1304;} _LL1304: if(*(( int*)
_temp1264) == Cyc_Absyn_Sizeofexp_e){ _LL1421: _temp1420=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1264)->f1; goto _LL1305;} else{ goto _LL1306;} _LL1306: if(*(( int*)
_temp1264) == Cyc_Absyn_Deref_e){ _LL1423: _temp1422=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1264)->f1; goto _LL1307;} else{ goto _LL1308;} _LL1308: if(*(( int*)
_temp1264) == Cyc_Absyn_StructMember_e){ _LL1427: _temp1426=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1264)->f1; goto _LL1425; _LL1425: _temp1424=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1264)->f2; goto _LL1309;} else{ goto _LL1310;} _LL1310: if(*(( int*)
_temp1264) == Cyc_Absyn_StructArrow_e){ _LL1431: _temp1430=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1264)->f1; goto _LL1429; _LL1429: _temp1428=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1264)->f2; goto _LL1311;} else{ goto _LL1312;} _LL1312: if(*(( int*)
_temp1264) == Cyc_Absyn_Subscript_e){ _LL1435: _temp1434=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1264)->f1; goto _LL1433; _LL1433: _temp1432=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1264)->f2; goto _LL1313;} else{ goto _LL1314;} _LL1314: if(*(( int*)
_temp1264) == Cyc_Absyn_Tuple_e){ _LL1437: _temp1436=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1264)->f1; goto _LL1315;} else{ goto _LL1316;} _LL1316: if(*(( int*)
_temp1264) == Cyc_Absyn_CompoundLit_e){ _LL1441: _temp1440=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1264)->f1; goto _LL1439; _LL1439: _temp1438=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1264)->f2; goto _LL1317;} else{ goto _LL1318;} _LL1318: if(*(( int*)
_temp1264) == Cyc_Absyn_Array_e){ _LL1443: _temp1442=(( struct Cyc_Absyn_Array_e_struct*)
_temp1264)->f1; goto _LL1319;} else{ goto _LL1320;} _LL1320: if(*(( int*)
_temp1264) == Cyc_Absyn_Comprehension_e){ _LL1449: _temp1448=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1264)->f1; goto _LL1447; _LL1447: _temp1446=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1264)->f2; goto _LL1445; _LL1445: _temp1444=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1264)->f3; goto _LL1321;} else{ goto _LL1322;} _LL1322: if(*(( int*)
_temp1264) == Cyc_Absyn_Struct_e){ _LL1457: _temp1456=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1264)->f1; goto _LL1455; _LL1455: _temp1454=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1264)->f2; goto _LL1453; _LL1453: _temp1452=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1264)->f3; goto _LL1451; _LL1451: _temp1450=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1264)->f4; goto _LL1323;} else{ goto _LL1324;} _LL1324: if(*(( int*)
_temp1264) == Cyc_Absyn_AnonStruct_e){ _LL1461: _temp1460=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1264)->f1; goto _LL1459; _LL1459: _temp1458=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1264)->f2; goto _LL1325;} else{ goto _LL1326;} _LL1326: if(*(( int*)
_temp1264) == Cyc_Absyn_Tunion_e){ _LL1471: _temp1470=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1264)->f1; goto _LL1469; _LL1469: _temp1468=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1264)->f2; goto _LL1467; _LL1467: _temp1466=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1264)->f3; if( _temp1466 == 0){ goto _LL1465;} else{ goto _LL1328;} _LL1465:
_temp1464=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1264)->f4; goto _LL1463;
_LL1463: _temp1462=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1264)->f5; goto
_LL1327;} else{ goto _LL1328;} _LL1328: if(*(( int*) _temp1264) == Cyc_Absyn_Tunion_e){
_LL1481: _temp1480=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1264)->f1; goto
_LL1479; _LL1479: _temp1478=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1264)->f2;
goto _LL1477; _LL1477: _temp1476=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1264)->f3;
goto _LL1475; _LL1475: _temp1474=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1264)->f4;
goto _LL1473; _LL1473: _temp1472=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1264)->f5;
goto _LL1329;} else{ goto _LL1330;} _LL1330: if(*(( int*) _temp1264) == Cyc_Absyn_Enum_e){
_LL1487: _temp1486=(( struct Cyc_Absyn_Enum_e_struct*) _temp1264)->f1; goto
_LL1485; _LL1485: _temp1484=(( struct Cyc_Absyn_Enum_e_struct*) _temp1264)->f2;
goto _LL1483; _LL1483: _temp1482=(( struct Cyc_Absyn_Enum_e_struct*) _temp1264)->f3;
goto _LL1331;} else{ goto _LL1332;} _LL1332: if(*(( int*) _temp1264) == Cyc_Absyn_Malloc_e){
_LL1491: _temp1490=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1264)->f1; goto
_LL1489; _LL1489: _temp1488=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1264)->f2; goto _LL1333;} else{ goto _LL1334;} _LL1334: if(*(( int*)
_temp1264) == Cyc_Absyn_StmtExp_e){ _LL1493: _temp1492=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1264)->f1; goto _LL1335;} else{ goto _LL1336;} _LL1336: if(*(( int*)
_temp1264) == Cyc_Absyn_UnresolvedMem_e){ _LL1497: _temp1496=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1264)->f1; goto _LL1495; _LL1495: _temp1494=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1264)->f2; goto _LL1337;} else{ goto _LL1338;} _LL1338: if(*(( int*)
_temp1264) == Cyc_Absyn_Codegen_e){ _LL1499: _temp1498=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1264)->f1; goto _LL1339;} else{ goto _LL1340;} _LL1340: if(*(( int*)
_temp1264) == Cyc_Absyn_Fill_e){ _LL1501: _temp1500=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1264)->f1; goto _LL1341;} else{ goto _LL1265;} _LL1267:{ void* _temp1502=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1508; struct Cyc_Absyn_Conref*
_temp1510; struct Cyc_Absyn_Tqual _temp1512; struct Cyc_Absyn_Conref* _temp1514;
void* _temp1516; void* _temp1518; _LL1504: if(( unsigned int) _temp1502 > 4u?*((
int*) _temp1502) == Cyc_Absyn_PointerType: 0){ _LL1509: _temp1508=(( struct Cyc_Absyn_PointerType_struct*)
_temp1502)->f1; _LL1519: _temp1518=( void*) _temp1508.elt_typ; goto _LL1517;
_LL1517: _temp1516=( void*) _temp1508.rgn_typ; goto _LL1515; _LL1515: _temp1514=
_temp1508.nullable; goto _LL1513; _LL1513: _temp1512= _temp1508.tq; goto _LL1511;
_LL1511: _temp1510= _temp1508.bounds; goto _LL1505;} else{ goto _LL1506;}
_LL1506: goto _LL1507; _LL1505: if(( void*) _temp1510->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1503;}{ void* _temp1520=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1510); struct Cyc_Absyn_Exp* _temp1526; _LL1522: if( _temp1520 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1523;} else{ goto _LL1524;} _LL1524: if((
unsigned int) _temp1520 > 1u?*(( int*) _temp1520) == Cyc_Absyn_Upper_b: 0){
_LL1527: _temp1526=(( struct Cyc_Absyn_Upper_b_struct*) _temp1520)->f1; goto
_LL1525;} else{ goto _LL1521;} _LL1523: { struct _tuple4* _temp1529=({ struct
_tuple4* _temp1528=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1528->f1= 0; _temp1528->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1528;});
goto _LL1530; _LL1530: { struct Cyc_Absyn_Exp* _temp1537= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List*(* _temp1531)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)(
struct _tagged_ptr3)) Cyc_List_list; struct _tuple4* _temp1534= _temp1529;
struct _tuple4* _temp1535= _temp1529; struct _tuple4* _temp1536= _temp1529;
struct _tuple4* _temp1532[ 3u]={ _temp1534, _temp1535, _temp1536}; struct
_tagged_ptr3 _temp1533={ _temp1532, _temp1532, _temp1532 + 3u}; _temp1531(
_temp1533);}), 0); goto _LL1538; _LL1538: if( nv->toplevel){( void*)( e->r=(
void*)(( void*) _temp1537->r));} else{ void* _temp1539= Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1518), Cyc_Toc_mt_tq); goto _LL1540; _LL1540: { struct
_tuple0* _temp1541= Cyc_Toc_temp_var(); goto _LL1542; _LL1542: { struct Cyc_Absyn_Exp*
_temp1543= Cyc_Absyn_var_exp( _temp1541, 0); goto _LL1544; _LL1544: { struct Cyc_Absyn_Stmt*
_temp1545= Cyc_Absyn_declare_stmt( _temp1541, _temp1539,( struct Cyc_Absyn_Exp*)
_temp1537, Cyc_Absyn_exp_stmt( _temp1543, 0), 0); goto _LL1546; _LL1546:( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1545, 0))->r));}}}} goto
_LL1521;}} _LL1525:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0,
0))->r)); goto _LL1521; _LL1521:;} goto _LL1503; _LL1507:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1547= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1547.last_plus_one -
_temp1547.curr, _temp1547.curr);})); goto _LL1503; _LL1503:;} goto _LL1265;
_LL1269: goto _LL1265; _LL1271:{ struct _handler_cons _temp1548; _push_handler(&
_temp1548);{ int _temp1550= 0; if( setjmp( _temp1548.handler)){ _temp1550= 1;}
if( ! _temp1550){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1348))->r));; _pop_handler();} else{ void* _temp1549=( void*) _exn_thrown;
void* _temp1552= _temp1549; _LL1554: if( _temp1552 == Cyc_Dict_Absent){ goto
_LL1555;} else{ goto _LL1556;} _LL1556: goto _LL1557; _LL1555:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1558= Cyc_Absynpp_qvar2string(
_temp1348); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1558.last_plus_one
- _temp1558.curr, _temp1558.curr);})); return; _LL1557:( void) _throw( _temp1552);
_LL1553:;}}} goto _LL1265; _LL1273:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp1559="unknownid"; struct _tagged_string _temp1560; _temp1560.curr=
_temp1559; _temp1560.base= _temp1559; _temp1560.last_plus_one= _temp1559 + 10;
_temp1560;})); goto _LL1265; _LL1275: { struct Cyc_List_List* _temp1561=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp1352); goto _LL1562; _LL1562: {
struct Cyc_List_List* _temp1563=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1352); goto _LL1564; _LL1564:(( void(*)( void(* f)( struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)(
Cyc_Toc_exp_to_c, nv, _temp1352);{ void* _temp1565= _temp1354; _LL1567: if(
_temp1565 ==( void*) Cyc_Absyn_Size){ goto _LL1568;} else{ goto _LL1569;}
_LL1569: if( _temp1565 ==( void*) Cyc_Absyn_Printf){ goto _LL1570;} else{ goto
_LL1571;} _LL1571: if( _temp1565 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1572;}
else{ goto _LL1573;} _LL1573: if( _temp1565 ==( void*) Cyc_Absyn_Fprintf){ goto
_LL1574;} else{ goto _LL1575;} _LL1575: if( _temp1565 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1576;} else{ goto _LL1577;} _LL1577: if( _temp1565 ==( void*) Cyc_Absyn_Plus){
goto _LL1578;} else{ goto _LL1579;} _LL1579: if( _temp1565 ==( void*) Cyc_Absyn_Minus){
goto _LL1580;} else{ goto _LL1581;} _LL1581: if( _temp1565 ==( void*) Cyc_Absyn_Eq){
goto _LL1582;} else{ goto _LL1583;} _LL1583: if( _temp1565 ==( void*) Cyc_Absyn_Neq){
goto _LL1584;} else{ goto _LL1585;} _LL1585: if( _temp1565 ==( void*) Cyc_Absyn_Gt){
goto _LL1586;} else{ goto _LL1587;} _LL1587: if( _temp1565 ==( void*) Cyc_Absyn_Gte){
goto _LL1588;} else{ goto _LL1589;} _LL1589: if( _temp1565 ==( void*) Cyc_Absyn_Lt){
goto _LL1590;} else{ goto _LL1591;} _LL1591: if( _temp1565 ==( void*) Cyc_Absyn_Lte){
goto _LL1592;} else{ goto _LL1593;} _LL1593: goto _LL1594; _LL1568: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1352))->hd;{
void* _temp1595=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1603; struct Cyc_Absyn_Tqual _temp1605; void*
_temp1607; struct Cyc_Absyn_PtrInfo _temp1609; struct Cyc_Absyn_Conref*
_temp1611; struct Cyc_Absyn_Tqual _temp1613; struct Cyc_Absyn_Conref* _temp1615;
void* _temp1617; void* _temp1619; _LL1597: if(( unsigned int) _temp1595 > 4u?*((
int*) _temp1595) == Cyc_Absyn_ArrayType: 0){ _LL1608: _temp1607=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1595)->f1; goto _LL1606; _LL1606:
_temp1605=(( struct Cyc_Absyn_ArrayType_struct*) _temp1595)->f2; goto _LL1604;
_LL1604: _temp1603=(( struct Cyc_Absyn_ArrayType_struct*) _temp1595)->f3; goto
_LL1598;} else{ goto _LL1599;} _LL1599: if(( unsigned int) _temp1595 > 4u?*((
int*) _temp1595) == Cyc_Absyn_PointerType: 0){ _LL1610: _temp1609=(( struct Cyc_Absyn_PointerType_struct*)
_temp1595)->f1; _LL1620: _temp1619=( void*) _temp1609.elt_typ; goto _LL1618;
_LL1618: _temp1617=( void*) _temp1609.rgn_typ; goto _LL1616; _LL1616: _temp1615=
_temp1609.nullable; goto _LL1614; _LL1614: _temp1613= _temp1609.tq; goto _LL1612;
_LL1612: _temp1611= _temp1609.bounds; goto _LL1600;} else{ goto _LL1601;}
_LL1601: goto _LL1602; _LL1598:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1603))->r)); goto _LL1596; _LL1600: { struct _tuple0*
_temp1621= Cyc_Toc_temp_var(); goto _LL1622; _LL1622: { void* _temp1623= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); goto _LL1624;
_LL1624: { struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1621, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1621, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4=
Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp( e2, e3, 0), 0);
struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1621, _temp1623,(
struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1596;}}} _LL1602:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1625= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*) _check_null(
_temp1563))->hd); struct _tagged_string _temp1626= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_temp1625.last_plus_one - _temp1625.curr, _temp1625.curr, _temp1626.last_plus_one
- _temp1626.curr, _temp1626.curr);})); return; _LL1596:;} goto _LL1566;} _LL1570:(
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1354, 0, _temp1352,
_temp1563))->r)); goto _LL1566; _LL1572:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1354, 0, _temp1352, _temp1563))->r)); goto _LL1566; _LL1574: if( _temp1352
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp1627="fprintf without arguments"; struct _tagged_string _temp1628;
_temp1628.curr= _temp1627; _temp1628.base= _temp1627; _temp1628.last_plus_one=
_temp1627 + 26; _temp1628;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1354,({ struct Cyc_Core_Opt* _temp1629=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1629->v=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1352))->hd); _temp1629;}),(( struct Cyc_List_List*)
_check_null( _temp1352))->tl,(( struct Cyc_List_List*) _check_null( _temp1563))->tl))->r));
goto _LL1566; _LL1576: if( _temp1352 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ char* _temp1630="sscanf without arguments"; struct
_tagged_string _temp1631; _temp1631.curr= _temp1630; _temp1631.base= _temp1630;
_temp1631.last_plus_one= _temp1630 + 25; _temp1631;}));}( void*)((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1352))->hd)->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1352))->hd)->r,(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1352))->hd)->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1566;
_LL1578: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1352))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1352))->tl))->hd; struct
_tuple0* _temp1632= Cyc_Toc_temp_var(); goto _LL1633; _LL1633: { void* _temp1634=
Cyc_Toc_typ_to_c( old_typ); goto _LL1635; _LL1635: { struct Cyc_Absyn_Stmt*
_temp1636= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1632, 0), 0); goto
_LL1637; _LL1637: { struct Cyc_Absyn_Exp* _temp1638= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1632, 0), Cyc_Toc_curr_sp, 0); goto _LL1639; _LL1639: {
struct Cyc_Absyn_Exp* _temp1641= Cyc_Absyn_assignop_exp( _temp1638,({ struct Cyc_Core_Opt*
_temp1640=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1640->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1640;}), e2, 0); goto
_LL1642; _LL1642: _temp1636= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1641,
0), _temp1636, 0); _temp1636= Cyc_Absyn_declare_stmt( _temp1632, _temp1634,(
struct Cyc_Absyn_Exp*) e1, _temp1636, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1636, 0))->r));}}}}} goto _LL1566; _LL1580: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null( _temp1561))->hd)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1352))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1352))->tl))->hd; void*
old_typ_e2=( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1561))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* _temp1643= Cyc_Toc_temp_var(); goto _LL1644;
_LL1644: { void* _temp1645= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_List_List*)
_check_null( _temp1561))->hd); goto _LL1646; _LL1646: { struct Cyc_Absyn_Stmt*
_temp1647= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1643, 0), 0); goto
_LL1648; _LL1648: { struct Cyc_Absyn_Exp* _temp1649= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1643, 0), Cyc_Toc_curr_sp, 0); goto _LL1650; _LL1650: {
struct Cyc_Absyn_Exp* _temp1652= Cyc_Absyn_assignop_exp( _temp1649,({ struct Cyc_Core_Opt*
_temp1651=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1651->v=( void*)(( void*) Cyc_Absyn_Minus); _temp1651;}), e2, 0); goto
_LL1653; _LL1653: _temp1647= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1652,
0), _temp1647, 0); _temp1647= Cyc_Absyn_declare_stmt( _temp1643, _temp1645,(
struct Cyc_Absyn_Exp*) e1, _temp1647, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1647, 0))->r));}}}}}} goto _LL1566; _LL1582: goto _LL1584; _LL1584: goto
_LL1586; _LL1586: goto _LL1588; _LL1588: goto _LL1590; _LL1590: goto _LL1592;
_LL1592: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1352))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1352))->tl))->hd; void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp1561))->hd; void* t2=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1561))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1566;} _LL1594: goto
_LL1566; _LL1566:;} goto _LL1265;}} _LL1277: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1358->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1358); if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if( _temp1356 ==(
void*) Cyc_Absyn_PreInc? 1: _temp1356 ==( void*) Cyc_Absyn_PreDec){ struct
_tuple0* _temp1654= Cyc_Toc_temp_var(); goto _LL1655; _LL1655: { void* _temp1656=
Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1657; _LL1657: { struct Cyc_Absyn_Stmt*
_temp1658= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp1654,
0), 0), 0); goto _LL1659; _LL1659: { struct Cyc_Absyn_Exp* _temp1660= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1654, 0), Cyc_Toc_curr_sp, 0),
_temp1356, 0); goto _LL1661; _LL1661: _temp1658= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1660, 0), _temp1658, 0); _temp1658= Cyc_Absyn_declare_stmt( _temp1654,
_temp1656,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1358, 0),
_temp1658, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1658, 0))->r));}}}}
else{ struct _tuple0* _temp1662= Cyc_Toc_temp_var(); goto _LL1663; _LL1663: {
struct _tuple0* _temp1664= Cyc_Toc_temp_var(); goto _LL1665; _LL1665: { void*
_temp1666= Cyc_Toc_typ_to_c( e2_cyc_typ); goto _LL1667; _LL1667: { void*
_temp1668= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1669; _LL1669: { struct Cyc_Absyn_Stmt*
_temp1670= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1664, 0), 0); goto
_LL1671; _LL1671: { struct Cyc_Absyn_Exp* _temp1672= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1662, 0), Cyc_Toc_curr_sp, 0),
_temp1356, 0); goto _LL1673; _LL1673: _temp1670= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1672, 0), _temp1670, 0); _temp1670= Cyc_Absyn_declare_stmt( _temp1664,
_temp1666,( struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp1662, 0), 0), _temp1670, 0); _temp1670= Cyc_Absyn_declare_stmt( _temp1662,
_temp1668,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1358, 0),
_temp1670, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1670, 0))->r));}}}}}}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1358)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1358, 0, Cyc_Toc_incr_lvalue,
_temp1356);( void*)( e->r=( void*)(( void*) _temp1358->r));}} goto _LL1265;}
_LL1279: { int e1_poly= Cyc_Toc_is_poly_project( _temp1364); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1364->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1360->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1364); Cyc_Toc_exp_to_c( nv, _temp1360); if(
_temp1362 != 0?( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1362))->v ==(
void*) Cyc_Absyn_Plus? 1:( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1362))->v
==( void*) Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){(
void*)( _temp1364->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1364->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1360->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1360->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1360->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1360->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1364)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1364, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1674=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1674[ 0]=({ struct _tuple11 _temp1675; _temp1675.f1=
_temp1362; _temp1675.f2= _temp1360; _temp1675;}); _temp1674;}));( void*)( e->r=(
void*)(( void*) _temp1364->r));} goto _LL1265;} _LL1281: Cyc_Toc_exp_to_c( nv,
_temp1370); Cyc_Toc_exp_to_c( nv, _temp1368); Cyc_Toc_exp_to_c( nv, _temp1366);
goto _LL1265; _LL1283: Cyc_Toc_exp_to_c( nv, _temp1374); Cyc_Toc_exp_to_c( nv,
_temp1372); goto _LL1265; _LL1285: _temp1384= _temp1378; _temp1382= _temp1376;
goto _LL1287; _LL1287: Cyc_Toc_exp_to_c( nv, _temp1384);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1382); goto _LL1265; _LL1289:{
struct _RegionHandle _temp1676= _new_region(); struct _RegionHandle* r=&
_temp1676; _push_region( r);{ void* _temp1677=( void*) _temp1389->type; goto
_LL1678; _LL1678: { void* _temp1679= Cyc_Toc_typ_to_c( _temp1677); goto _LL1680;
_LL1680: { void* _temp1681= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1397->topt))->v); goto _LL1682; _LL1682: { struct Cyc_Absyn_Exp*
_temp1683= Cyc_Absyn_uint_exp(( unsigned int) _temp1393, 0); goto _LL1684;
_LL1684: { void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1756=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1756[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1757; _temp1757.tag= Cyc_Absyn_ArrayType;
_temp1757.f1=( void*) _temp1677; _temp1757.f2= Cyc_Toc_mt_tq; _temp1757.f3=(
struct Cyc_Absyn_Exp*) _temp1683; _temp1757;}); _temp1756;})); struct _tuple0*
_temp1685= Cyc_Toc_temp_var(); goto _LL1686; _LL1686: { struct _tuple0*
_temp1687= Cyc_Toc_temp_var(); goto _LL1688; _LL1688: { struct Cyc_Absyn_Exp*
_temp1689= Cyc_Absyn_var_exp( _temp1687, 0); goto _LL1690; _LL1690: { struct
_tuple0* _temp1691= Cyc_Toc_temp_var(); goto _LL1692; _LL1692: { int _temp1693=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1395); goto _LL1694;
_LL1694: { int _temp1695= _temp1693 - _temp1393; goto _LL1696; _LL1696: { struct
_tagged_ptr5 vs=({ unsigned int _temp1752=( unsigned int) _temp1393; struct
_tuple0** _temp1753=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1752); struct _tagged_ptr5 _temp1755={ _temp1753, _temp1753,
_temp1753 + _temp1752};{ unsigned int _temp1754= _temp1752; unsigned int i; for(
i= 0; i < _temp1754; i ++){ _temp1753[ i]= Cyc_Toc_temp_var();}}; _temp1755;});
struct _tagged_ptr5 xs=({ unsigned int _temp1748=( unsigned int) _temp1695;
struct _tuple0** _temp1749=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1748); struct _tagged_ptr5 _temp1751={ _temp1749, _temp1749,
_temp1749 + _temp1748};{ unsigned int _temp1750= _temp1748; unsigned int i; for(
i= 0; i < _temp1750; i ++){ _temp1749[ i]= Cyc_Toc_temp_var();}}; _temp1751;});
struct Cyc_List_List* xexps=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(*
f)( struct _tuple0*, struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,( struct Cyc_Position_Segment*)
0,(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct _tagged_ptr5 arr))
Cyc_List_rfrom_array)( r, xs)); xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List*
_temp1697=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1697->hd=( void*) Cyc_Absyn_var_exp( _temp1691, 0); _temp1697->tl= 0;
_temp1697;}));{ struct Cyc_Absyn_Stmt* _temp1698= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( _temp1685, 0), xexps, 0), 0); goto _LL1699; _LL1699: { struct
Cyc_Absyn_Exp* _temp1700= Cyc_Absyn_add_exp( _temp1689, _temp1683, 0); goto
_LL1701; _LL1701: { struct Cyc_List_List* _temp1711=({ struct Cyc_List_List*(*
_temp1702)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)( struct _tagged_ptr3))
Cyc_List_list; struct _tuple4* _temp1705=({ struct _tuple4* _temp1710=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1710->f1= 0; _temp1710->f2=
_temp1689; _temp1710;}); struct _tuple4* _temp1706=({ struct _tuple4* _temp1709=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1709->f1= 0;
_temp1709->f2= _temp1689; _temp1709;}); struct _tuple4* _temp1707=({ struct
_tuple4* _temp1708=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1708->f1= 0; _temp1708->f2= _temp1700; _temp1708;}); struct _tuple4*
_temp1703[ 3u]={ _temp1705, _temp1706, _temp1707}; struct _tagged_ptr3 _temp1704={
_temp1703, _temp1703, _temp1703 + 3u}; _temp1702( _temp1704);}); goto _LL1712;
_LL1712: _temp1698= Cyc_Absyn_declare_stmt( _temp1691, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp1677,( void*) _temp1389->rgn, Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp(
0, _temp1711, 0), _temp1698, 0);{ struct Cyc_List_List* _temp1713=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_ptr5 arr)) Cyc_List_rfrom_array)( r, vs));
goto _LL1714; _LL1714: { struct Cyc_List_List* _temp1715=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_make_dle, _temp1713); goto _LL1716; _LL1716: _temp1698= Cyc_Absyn_declare_stmt(
_temp1687, arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1715, 0), _temp1698, 0);{ struct Cyc_List_List* _temp1717=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1395);
goto _LL1718; _LL1718: { struct Cyc_List_List* _temp1719=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1391);
goto _LL1720; _LL1720:{ int i= _temp1393 - 1; for( 0; i >= 0;( i --, _temp1717=((
struct Cyc_List_List*) _check_null( _temp1717))->tl)){ struct Cyc_Absyn_Exp* arg=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1717))->hd;
void* arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c(
nv, arg); if( _temp1389->inject){ void* _temp1723; struct Cyc_Position_Segment*
_temp1725; struct Cyc_List_List* _temp1727; struct Cyc_List_List* _temp1729;
struct _tuple0* _temp1731; struct Cyc_Absyn_Tunionfield _temp1721=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1719))->hd); _LL1732: _temp1731=
_temp1721.name; goto _LL1730; _LL1730: _temp1729= _temp1721.tvs; goto _LL1728;
_LL1728: _temp1727= _temp1721.typs; goto _LL1726; _LL1726: _temp1725= _temp1721.loc;
goto _LL1724; _LL1724: _temp1723=( void*) _temp1721.sc; goto _LL1722; _LL1722:
_temp1719=(( struct Cyc_List_List*) _check_null( _temp1719))->tl;{ struct
_tuple0* _temp1733= Cyc_Toc_temp_var(); goto _LL1734; _LL1734: { struct Cyc_Absyn_Exp*
_temp1735= Cyc_Absyn_var_exp( _temp1733, 0); goto _LL1736; _LL1736: { void*
_temp1739= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1731,({ char*
_temp1737="_struct"; struct _tagged_string _temp1738; _temp1738.curr= _temp1737;
_temp1738.base= _temp1737; _temp1738.last_plus_one= _temp1737 + 8; _temp1738;})));
goto _LL1740; _LL1740: _temp1698= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)(
struct _tagged_ptr5, unsigned int, unsigned int)) _check_unknown_subscript)( vs,
sizeof( struct _tuple0*), i), _temp1679,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
_temp1679, Cyc_Absyn_address_exp( _temp1735, 0), 0), _temp1698, 0);{ void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1727))->hd)).f2); if( Cyc_Toc_is_void_star( field_typ)){ arg=
Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt* _temp1744= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1735,({ struct _tagged_string* _temp1741=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp1741[ 0]=({
char* _temp1742="f1"; struct _tagged_string _temp1743; _temp1743.curr= _temp1742;
_temp1743.base= _temp1742; _temp1743.last_plus_one= _temp1742 + 3; _temp1743;});
_temp1741;}), 0), arg, 0); goto _LL1745; _LL1745: _temp1698= Cyc_Absyn_seq_stmt(
_temp1744, _temp1698, 0);{ struct Cyc_Absyn_Stmt* _temp1746= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1735, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp1731, 0), 0); goto _LL1747; _LL1747: _temp1698= Cyc_Absyn_seq_stmt(
_temp1746, _temp1698, 0); _temp1698= Cyc_Absyn_declare_stmt( _temp1733,
_temp1739, 0, _temp1698, 0);}}}}}}} else{ _temp1698= Cyc_Absyn_declare_stmt(*((
struct _tuple0**(*)( struct _tagged_ptr5, unsigned int, unsigned int))
_check_unknown_subscript)( vs, sizeof( struct _tuple0*), i), _temp1679,( struct
Cyc_Absyn_Exp*) arg, _temp1698, 0);}}}{ int i= _temp1695 - 1; for( 0; i >= 0;( i
--, _temp1717=(( struct Cyc_List_List*) _check_null( _temp1717))->tl)){ struct
Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1717))->hd; void* argtype= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); Cyc_Toc_exp_to_c( nv, arg); _temp1698= Cyc_Absyn_declare_stmt(*((
struct _tuple0**(*)( struct _tagged_ptr5, unsigned int, unsigned int))
_check_unknown_subscript)( xs, sizeof( struct _tuple0*), i), argtype,( struct
Cyc_Absyn_Exp*) arg, _temp1698, 0);}} Cyc_Toc_exp_to_c( nv, _temp1397);
_temp1698= Cyc_Absyn_declare_stmt( _temp1685, _temp1681,( struct Cyc_Absyn_Exp*)
_temp1397, _temp1698, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1698, 0))->r));}}}}}}}}}}}}}}}}}}; _pop_region(& _temp1676);} goto _LL1265;
_LL1291: Cyc_Toc_exp_to_c( nv, _temp1399);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1399), 0))->r)); goto
_LL1265; _LL1293: Cyc_Toc_exp_to_c( nv, _temp1401); goto _LL1265; _LL1295: Cyc_Toc_exp_to_c(
nv, _temp1405);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); for( 0; _temp1403 != 0; _temp1403=(( struct Cyc_List_List*)
_check_null( _temp1403))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct
Cyc_List_List*) _check_null( _temp1403))->hd); if( k !=( void*) Cyc_Absyn_EffKind?
k !=( void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1405, 0))->r)); break;}} goto _LL1265;} _LL1297: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1407->topt))->v; void* new_typ=*
_temp1411;* _temp1411= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1407);{ int noconv= 0;{ void* _temp1758= Cyc_Toc_conversion( old_t2,
new_typ); void* _temp1768; struct Cyc_Absyn_Exp* _temp1770; void* _temp1772; int
_temp1774; struct Cyc_Absyn_Exp* _temp1776; void* _temp1778; _LL1760: if(
_temp1758 ==( void*) Cyc_Toc_NoConv){ goto _LL1761;} else{ goto _LL1762;}
_LL1762: if(( unsigned int) _temp1758 > 1u?*(( int*) _temp1758) == Cyc_Toc_NullCheck:
0){ _LL1769: _temp1768=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp1758)->f1;
goto _LL1763;} else{ goto _LL1764;} _LL1764: if(( unsigned int) _temp1758 > 1u?*((
int*) _temp1758) == Cyc_Toc_TagPtr: 0){ _LL1773: _temp1772=( void*)(( struct Cyc_Toc_TagPtr_struct*)
_temp1758)->f1; goto _LL1771; _LL1771: _temp1770=(( struct Cyc_Toc_TagPtr_struct*)
_temp1758)->f2; goto _LL1765;} else{ goto _LL1766;} _LL1766: if(( unsigned int)
_temp1758 > 1u?*(( int*) _temp1758) == Cyc_Toc_UntagPtr: 0){ _LL1779: _temp1778=(
void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp1758)->f1; goto _LL1777; _LL1777:
_temp1776=(( struct Cyc_Toc_UntagPtr_struct*) _temp1758)->f2; goto _LL1775;
_LL1775: _temp1774=(( struct Cyc_Toc_UntagPtr_struct*) _temp1758)->f3; goto
_LL1767;} else{ goto _LL1759;} _LL1761: goto _LL1759; _LL1763:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_null_check_conv( nv->toplevel, _temp1768, _temp1407))->r));
goto _LL1759; _LL1765:( void*)( e->r=( void*)(( void*)( Cyc_Toc_tag_ptr_conv( nv->toplevel,
_temp1772, _temp1770, _temp1407))->r)); goto _LL1759; _LL1767:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_untag_ptr_conv( nv->toplevel, _temp1778, _temp1776,
_temp1774, _temp1407))->r)); goto _LL1759; _LL1759:;} goto _LL1265;}} _LL1299:{
void* _temp1780=( void*) _temp1412->r; struct Cyc_Absyn_Structdecl* _temp1788;
struct Cyc_List_List* _temp1790; struct Cyc_Core_Opt* _temp1792; struct _tuple0*
_temp1794; struct Cyc_List_List* _temp1796; _LL1782: if(*(( int*) _temp1780) ==
Cyc_Absyn_Struct_e){ _LL1795: _temp1794=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1780)->f1; goto _LL1793; _LL1793: _temp1792=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1780)->f2; goto _LL1791; _LL1791: _temp1790=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1780)->f3; goto _LL1789; _LL1789: _temp1788=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1780)->f4; goto _LL1783;} else{ goto _LL1784;} _LL1784: if(*(( int*)
_temp1780) == Cyc_Absyn_Tuple_e){ _LL1797: _temp1796=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1780)->f1; goto _LL1785;} else{ goto _LL1786;} _LL1786: goto _LL1787;
_LL1783: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1798= Cyc_Position_string_of_segment( _temp1412->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1798.last_plus_one -
_temp1798.curr, _temp1798.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1412->topt))->v, 1, 0,
_temp1790, _temp1794))->r)); goto _LL1781; _LL1785: if( nv->toplevel){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1799= Cyc_Position_string_of_segment(
_temp1412->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1799.last_plus_one - _temp1799.curr, _temp1799.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1796))->r)); goto _LL1781;
_LL1787: Cyc_Toc_exp_to_c( nv, _temp1412); if( ! Cyc_Absyn_is_lvalue( _temp1412)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1412,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1412, 0))->r));} goto _LL1781; _LL1781:;} goto _LL1265; _LL1301: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string
_temp1800= Cyc_Position_string_of_segment( _temp1414->loc); xprintf("%.*s: new at top-level",
_temp1800.last_plus_one - _temp1800.curr, _temp1800.curr);}));}{ void* _temp1801=(
void*) _temp1414->r; struct Cyc_List_List* _temp1815; void* _temp1817; struct
_tagged_string _temp1819; struct Cyc_Absyn_Exp* _temp1821; struct Cyc_Absyn_Exp*
_temp1823; struct Cyc_Absyn_Vardecl* _temp1825; struct Cyc_Absyn_Structdecl*
_temp1827; struct Cyc_List_List* _temp1829; struct Cyc_Core_Opt* _temp1831;
struct _tuple0* _temp1833; struct Cyc_List_List* _temp1835; _LL1803: if(*(( int*)
_temp1801) == Cyc_Absyn_Array_e){ _LL1816: _temp1815=(( struct Cyc_Absyn_Array_e_struct*)
_temp1801)->f1; goto _LL1804;} else{ goto _LL1805;} _LL1805: if(*(( int*)
_temp1801) == Cyc_Absyn_Const_e){ _LL1818: _temp1817=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1801)->f1; if(( unsigned int) _temp1817 > 1u?*(( int*) _temp1817) == Cyc_Absyn_String_c:
0){ _LL1820: _temp1819=(( struct Cyc_Absyn_String_c_struct*) _temp1817)->f1;
goto _LL1806;} else{ goto _LL1807;}} else{ goto _LL1807;} _LL1807: if(*(( int*)
_temp1801) == Cyc_Absyn_Comprehension_e){ _LL1826: _temp1825=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1801)->f1; goto _LL1824; _LL1824: _temp1823=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1801)->f2; goto _LL1822; _LL1822: _temp1821=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1801)->f3; goto _LL1808;} else{ goto _LL1809;} _LL1809: if(*(( int*)
_temp1801) == Cyc_Absyn_Struct_e){ _LL1834: _temp1833=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1801)->f1; goto _LL1832; _LL1832: _temp1831=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1801)->f2; goto _LL1830; _LL1830: _temp1829=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1801)->f3; goto _LL1828; _LL1828: _temp1827=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1801)->f4; goto _LL1810;} else{ goto _LL1811;} _LL1811: if(*(( int*)
_temp1801) == Cyc_Absyn_Tuple_e){ _LL1836: _temp1835=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1801)->f1; goto _LL1812;} else{ goto _LL1813;} _LL1813: goto _LL1814;
_LL1804: { struct _tuple0* _temp1837= Cyc_Toc_temp_var(); goto _LL1838; _LL1838: {
struct Cyc_Absyn_Exp* _temp1839= Cyc_Absyn_var_exp( _temp1837, 0); goto _LL1840;
_LL1840: { struct Cyc_Absyn_Stmt* _temp1841= Cyc_Toc_init_array( nv, _temp1839,
_temp1815, Cyc_Absyn_exp_stmt( _temp1839, 0)); goto _LL1842; _LL1842: { void*
old_elt_typ;{ void* _temp1843= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1849; struct Cyc_Absyn_Conref* _temp1851; struct Cyc_Absyn_Tqual _temp1853;
struct Cyc_Absyn_Conref* _temp1855; void* _temp1857; void* _temp1859; _LL1845:
if(( unsigned int) _temp1843 > 4u?*(( int*) _temp1843) == Cyc_Absyn_PointerType:
0){ _LL1850: _temp1849=(( struct Cyc_Absyn_PointerType_struct*) _temp1843)->f1;
_LL1860: _temp1859=( void*) _temp1849.elt_typ; goto _LL1858; _LL1858: _temp1857=(
void*) _temp1849.rgn_typ; goto _LL1856; _LL1856: _temp1855= _temp1849.nullable;
goto _LL1854; _LL1854: _temp1853= _temp1849.tq; goto _LL1852; _LL1852: _temp1851=
_temp1849.bounds; goto _LL1846;} else{ goto _LL1847;} _LL1847: goto _LL1848;
_LL1846: old_elt_typ= _temp1859; goto _LL1844; _LL1848: old_elt_typ=(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp1861="exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1862; _temp1862.curr= _temp1861; _temp1862.base=
_temp1861; _temp1862.last_plus_one= _temp1861 + 52; _temp1862;})); goto _LL1844;
_LL1844:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* _temp1863= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1864; _LL1864: { struct Cyc_Absyn_Exp*
_temp1865= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1815), 0), 0); goto
_LL1866; _LL1866: { struct Cyc_Absyn_Exp* e1; if( _temp1416 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1865);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1416); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1865);}{ struct Cyc_Absyn_Exp* _temp1867= Cyc_Absyn_cast_exp( _temp1863, e1,
0); goto _LL1868; _LL1868:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1837, _temp1863,( struct Cyc_Absyn_Exp*) _temp1867,
_temp1841, 0), 0))->r)); goto _LL1802;}}}}}}}} _LL1806: if( _temp1416 == 0){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1869=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1869->f1= Cyc_Toc_abs_ns; _temp1869->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1869;}), 0),({ struct Cyc_List_List* _temp1870=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1870->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1871=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1871[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1872; _temp1872.tag= Cyc_Absyn_Const_e; _temp1872.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1873=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1873[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1874; _temp1874.tag= Cyc_Absyn_String_c;
_temp1874.f1= _temp1819; _temp1874;}); _temp1873;})); _temp1872;}); _temp1871;}),
0); _temp1870->tl= 0; _temp1870;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( _temp1416); Cyc_Toc_exp_to_c( nv, r);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({ struct
_tuple0* _temp1875=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1875->f1= Cyc_Toc_abs_ns; _temp1875->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1875;}), 0),({ struct Cyc_List_List* _temp1876=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1876->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1877=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1877[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1878; _temp1878.tag= Cyc_Absyn_Const_e; _temp1878.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1879=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1879[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1880; _temp1880.tag= Cyc_Absyn_String_c;
_temp1880.f1= _temp1819; _temp1880;}); _temp1879;})); _temp1878;}); _temp1877;}),
0); _temp1876->tl= 0; _temp1876;}), 0))->r));} goto _LL1802; _LL1808: { int
is_tagged_ptr= 0;{ void* _temp1881= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1887; struct Cyc_Absyn_Conref* _temp1889; struct Cyc_Absyn_Tqual _temp1891;
struct Cyc_Absyn_Conref* _temp1893; void* _temp1895; void* _temp1897; _LL1883:
if(( unsigned int) _temp1881 > 4u?*(( int*) _temp1881) == Cyc_Absyn_PointerType:
0){ _LL1888: _temp1887=(( struct Cyc_Absyn_PointerType_struct*) _temp1881)->f1;
_LL1898: _temp1897=( void*) _temp1887.elt_typ; goto _LL1896; _LL1896: _temp1895=(
void*) _temp1887.rgn_typ; goto _LL1894; _LL1894: _temp1893= _temp1887.nullable;
goto _LL1892; _LL1892: _temp1891= _temp1887.tq; goto _LL1890; _LL1890: _temp1889=
_temp1887.bounds; goto _LL1884;} else{ goto _LL1885;} _LL1885: goto _LL1886;
_LL1884:{ void* _temp1899=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1889))->v; void* _temp1905; _LL1901: if((
unsigned int) _temp1899 > 1u?*(( int*) _temp1899) == Cyc_Absyn_Eq_constr: 0){
_LL1906: _temp1905=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1899)->f1;
if( _temp1905 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1902;} else{ goto _LL1903;}}
else{ goto _LL1903;} _LL1903: goto _LL1904; _LL1902: is_tagged_ptr= 1; goto
_LL1900; _LL1904: goto _LL1900; _LL1900:;} goto _LL1882; _LL1886:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp1907="exp_to_c: comprehension not an array type";
struct _tagged_string _temp1908; _temp1908.curr= _temp1907; _temp1908.base=
_temp1907; _temp1908.last_plus_one= _temp1907 + 42; _temp1908;})); goto _LL1882;
_LL1882:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1821->topt))->v;
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1823);{ struct Cyc_Absyn_Stmt*
s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( a, 0), _temp1825, Cyc_Absyn_var_exp(
max, 0), _temp1821, Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1909=
_new_region(); struct _RegionHandle* r=& _temp1909; _push_region( r);{ struct
Cyc_List_List* decls=({ struct Cyc_List_List* _temp1939=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1939->hd=( void*)({
struct _tuple10* _temp1940=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp1940->f1= max; _temp1940->f2= Cyc_Absyn_uint_t; _temp1940->f3=(
struct Cyc_Absyn_Exp*) _temp1823; _temp1940;}); _temp1939->tl= 0; _temp1939;});
struct Cyc_Absyn_Exp* ai; if( _temp1416 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1416); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1910=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1910->hd=( void*)({ struct _tuple10*
_temp1911=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1911->f1= a; _temp1911->f2= ptr_typ; _temp1911->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1911;}); _temp1910->tl= decls; _temp1910;}); if( is_tagged_ptr){
struct _tuple0* _temp1912= Cyc_Toc_temp_var(); goto _LL1913; _LL1913: { void*
_temp1914= Cyc_Toc_typ_to_c( old_typ); goto _LL1915; _LL1915: { struct Cyc_Absyn_Exp*
_temp1925= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(* _temp1916)(
struct _tagged_ptr3)=( struct Cyc_List_List*(*)( struct _tagged_ptr3)) Cyc_List_list;
struct _tuple4* _temp1919=({ struct _tuple4* _temp1924=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1924->f1= 0; _temp1924->f2= Cyc_Absyn_var_exp(
a, 0); _temp1924;}); struct _tuple4* _temp1920=({ struct _tuple4* _temp1923=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1923->f1= 0;
_temp1923->f2= Cyc_Absyn_var_exp( a, 0); _temp1923;}); struct _tuple4* _temp1921=({
struct _tuple4* _temp1922=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1922->f1= 0; _temp1922->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1922;}); struct _tuple4* _temp1917[ 3u]={
_temp1919, _temp1920, _temp1921}; struct _tagged_ptr3 _temp1918={ _temp1917,
_temp1917, _temp1917 + 3u}; _temp1916( _temp1918);}), 0); goto _LL1926; _LL1926:
decls=({ struct Cyc_List_List* _temp1927=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1927->hd=( void*)({ struct _tuple10*
_temp1928=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1928->f1= _temp1912; _temp1928->f2= _temp1914; _temp1928->f3=( struct Cyc_Absyn_Exp*)
_temp1925; _temp1928;}); _temp1927->tl= decls; _temp1927;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1912, 0), 0), 0);}}} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
_temp1929= decls; goto _LL1930; _LL1930: for( 0; _temp1929 != 0; _temp1929=((
struct Cyc_List_List*) _check_null( _temp1929))->tl){ struct Cyc_Absyn_Exp*
_temp1933; void* _temp1935; struct _tuple0* _temp1937; struct _tuple10 _temp1931=*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1929))->hd);
_LL1938: _temp1937= _temp1931.f1; goto _LL1936; _LL1936: _temp1935= _temp1931.f2;
goto _LL1934; _LL1934: _temp1933= _temp1931.f3; goto _LL1932; _LL1932: s= Cyc_Absyn_declare_stmt(
_temp1937, _temp1935, _temp1933, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region(& _temp1909);} goto _LL1802;}}} _LL1810:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1414->topt))->v, 1, _temp1416, _temp1829, _temp1833))->r));
goto _LL1802; _LL1812:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv,
1, _temp1416, _temp1835))->r)); goto _LL1802; _LL1814: { void* _temp1941=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1414->topt))->v; goto _LL1942; _LL1942: {
struct _tuple0* _temp1943= Cyc_Toc_temp_var(); goto _LL1944; _LL1944: { struct
Cyc_Absyn_Stmt* _temp1945= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1943, 0),
0); goto _LL1946; _LL1946: { struct Cyc_Absyn_Exp* _temp1947= Cyc_Absyn_signed_int_exp(
0, 0); goto _LL1948; _LL1948: Cyc_Toc_exp_to_c( nv, _temp1414); _temp1945= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1943, 0),
_temp1947, 0), _temp1414, 0), _temp1945, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
_temp1941); void* _temp1949= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); goto
_LL1950; _LL1950: { struct Cyc_Absyn_Exp* _temp1951= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0); goto _LL1952; _LL1952: { struct Cyc_Absyn_Exp* e1; if( _temp1416 ==
0){ e1= Cyc_Toc_malloc_exp( _temp1941, _temp1951);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp1416); Cyc_Toc_exp_to_c( nv, r); e1=
Cyc_Toc_rmalloc_exp( r, _temp1951);}{ struct Cyc_Absyn_Exp* _temp1953= Cyc_Absyn_cast_exp(
_temp1949, e1, 0); goto _LL1954; _LL1954:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1943, _temp1949,( struct Cyc_Absyn_Exp*) _temp1953,
_temp1945, 0), 0))->r)); goto _LL1802;}}}}}}}} _LL1802:;} goto _LL1265; _LL1303:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct* _temp1955=(
struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1955[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1956; _temp1956.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1956.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1418);
_temp1956;}); _temp1955;}))); goto _LL1265; _LL1305: Cyc_Toc_exp_to_c( nv,
_temp1420); goto _LL1265; _LL1307: { void* e1_typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1422->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1422);{ void* _temp1957= e1_typ; struct Cyc_Absyn_PtrInfo _temp1963; struct
Cyc_Absyn_Conref* _temp1965; struct Cyc_Absyn_Tqual _temp1967; struct Cyc_Absyn_Conref*
_temp1969; void* _temp1971; void* _temp1973; _LL1959: if(( unsigned int)
_temp1957 > 4u?*(( int*) _temp1957) == Cyc_Absyn_PointerType: 0){ _LL1964:
_temp1963=(( struct Cyc_Absyn_PointerType_struct*) _temp1957)->f1; _LL1974:
_temp1973=( void*) _temp1963.elt_typ; goto _LL1972; _LL1972: _temp1971=( void*)
_temp1963.rgn_typ; goto _LL1970; _LL1970: _temp1969= _temp1963.nullable; goto
_LL1968; _LL1968: _temp1967= _temp1963.tq; goto _LL1966; _LL1966: _temp1965=
_temp1963.bounds; goto _LL1960;} else{ goto _LL1961;} _LL1961: goto _LL1962;
_LL1960:{ void* _temp1975=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1965); struct Cyc_Absyn_Exp* _temp1981; _LL1977: if( _temp1975 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1978;} else{ goto _LL1979;} _LL1979: if((
unsigned int) _temp1975 > 1u?*(( int*) _temp1975) == Cyc_Absyn_Upper_b: 0){
_LL1982: _temp1981=(( struct Cyc_Absyn_Upper_b_struct*) _temp1975)->f1; goto
_LL1980;} else{ goto _LL1976;} _LL1978: { struct _tuple0* x= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); void* _temp1983= Cyc_Toc_typ_to_c_array(
_temp1973); goto _LL1984; _LL1984: { void* tx= Cyc_Toc_add_tagged_ptr_type(
_temp1983, _temp1967); void* ta2= Cyc_Absyn_cstar_typ( _temp1983, _temp1967);
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
struct Cyc_Absyn_Exp*) _temp1422, s1, 0), 0))->r)); goto _LL1976;}} _LL1980: if(
Cyc_Toc_is_nullable( e1_typ)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1422->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1985=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1985->hd=(
void*) _temp1422; _temp1985->tl= 0; _temp1985;}), 0), 0), 0))->r));} else{
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, e1_typ,(
struct Cyc_Absyn_Exp*) _temp1422, s3, 0), 0))->r));}} goto _LL1976; _LL1976:;}
goto _LL1958; _LL1962:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp1986="exp_to_c: Deref: non-pointer"; struct _tagged_string _temp1987;
_temp1987.curr= _temp1986; _temp1987.base= _temp1986; _temp1987.last_plus_one=
_temp1986 + 29; _temp1987;})); goto _LL1958; _LL1958:;} goto _LL1265;} _LL1309:
Cyc_Toc_exp_to_c( nv, _temp1426); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL1265; _LL1311: Cyc_Toc_exp_to_c( nv, _temp1430);{ int is_poly=
Cyc_Toc_is_poly_project( e); if( Cyc_Toc_is_nullable(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1430->topt))->v)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1430->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1988=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1988->hd=(
void*) _temp1430; _temp1988->tl= 0; _temp1988;}), 0), 0), _temp1428, 0))->r));}
else{ struct _tuple0* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1430->topt))->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp1428, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1430, s3, 0), 0))->r));}} if( is_poly){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto
_LL1265;} _LL1313: { void* _temp1989= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1434->topt))->v); goto _LL1990; _LL1990: Cyc_Toc_exp_to_c( nv,
_temp1434); Cyc_Toc_exp_to_c( nv, _temp1432);{ void* _temp1991= _temp1989;
struct Cyc_List_List* _temp1999; struct Cyc_Absyn_PtrInfo _temp2001; struct Cyc_Absyn_Conref*
_temp2003; struct Cyc_Absyn_Tqual _temp2005; struct Cyc_Absyn_Conref* _temp2007;
void* _temp2009; void* _temp2011; _LL1993: if(( unsigned int) _temp1991 > 4u?*((
int*) _temp1991) == Cyc_Absyn_TupleType: 0){ _LL2000: _temp1999=(( struct Cyc_Absyn_TupleType_struct*)
_temp1991)->f1; goto _LL1994;} else{ goto _LL1995;} _LL1995: if(( unsigned int)
_temp1991 > 4u?*(( int*) _temp1991) == Cyc_Absyn_PointerType: 0){ _LL2002:
_temp2001=(( struct Cyc_Absyn_PointerType_struct*) _temp1991)->f1; _LL2012:
_temp2011=( void*) _temp2001.elt_typ; goto _LL2010; _LL2010: _temp2009=( void*)
_temp2001.rgn_typ; goto _LL2008; _LL2008: _temp2007= _temp2001.nullable; goto
_LL2006; _LL2006: _temp2005= _temp2001.tq; goto _LL2004; _LL2004: _temp2003=
_temp2001.bounds; goto _LL1996;} else{ goto _LL1997;} _LL1997: goto _LL1998;
_LL1994: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1432) + 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp1434, Cyc_Toc_fieldname((
int) i), 0))->r)); goto _LL1992;} _LL1996:{ void* _temp2013=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2003); struct Cyc_Absyn_Exp*
_temp2019; _LL2015: if(( unsigned int) _temp2013 > 1u?*(( int*) _temp2013) ==
Cyc_Absyn_Upper_b: 0){ _LL2020: _temp2019=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2013)->f1; goto _LL2016;} else{ goto _LL2017;} _LL2017: if( _temp2013 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL2018;} else{ goto _LL2014;} _LL2016:
_temp2019= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp( _temp2019), 0);{
int possibly_null;{ void* _temp2021=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2007))->v; int
_temp2027; _LL2023: if(( unsigned int) _temp2021 > 1u?*(( int*) _temp2021) ==
Cyc_Absyn_Eq_constr: 0){ _LL2028: _temp2027=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2021)->f1; goto _LL2024;} else{ goto _LL2025;} _LL2025: goto _LL2026;
_LL2024: possibly_null= _temp2027; goto _LL2022; _LL2026: possibly_null= 0; goto
_LL2022; _LL2022:;}{ void* ta1= Cyc_Toc_typ_to_c( _temp2011); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp2005); if( Cyc_Toc_use_runtimecheck_funs){ if( possibly_null){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv, 0),({ struct Cyc_List_List*(*
_temp2029)( struct _tagged_ptr6)=( struct Cyc_List_List*(*)( struct _tagged_ptr6))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp2032= _temp1434; struct Cyc_Absyn_Exp*
_temp2033= _temp2019; struct Cyc_Absyn_Exp* _temp2034= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp2035= _temp1432; struct Cyc_Absyn_Exp*
_temp2030[ 4u]={ _temp2032, _temp2033, _temp2034, _temp2035}; struct
_tagged_ptr6 _temp2031={ _temp2030, _temp2030, _temp2030 + 4u}; _temp2029(
_temp2031);}), 0), 0), 0))->r));} else{( void*)( _temp1432->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List*(* _temp2036)( struct _tagged_ptr6)=( struct Cyc_List_List*(*)(
struct _tagged_ptr6)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2039= _temp2019;
struct Cyc_Absyn_Exp* _temp2040= Cyc_Absyn_copy_exp( _temp1432); struct Cyc_Absyn_Exp*
_temp2037[ 2u]={ _temp2039, _temp2040}; struct _tagged_ptr6 _temp2038={
_temp2037, _temp2037, _temp2037 + 2u}; _temp2036( _temp2038);}), 0))->r));}}
else{ struct _tuple0* a= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte, Cyc_Absyn_var_exp(
i, 0), _temp2019, 0); if( possibly_null){ e3= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_new_exp((
void*) e3->r, 0), 0);}{ struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt(
i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) _temp1432, s3, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*)
_temp1434, s1, 0), 0))->r));}} goto _LL2014;}} _LL2018: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2011); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2005); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2050=({ struct Cyc_List_List*(* _temp2041)( struct
_tagged_ptr7)=( struct Cyc_List_List*(*)( struct _tagged_ptr7)) Cyc_List_list;
struct _tuple1* _temp2044=({ struct _tuple1* _temp2049=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2049->f1= 0; _temp2049->f2= Cyc_Toc_mt_tq;
_temp2049->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1434->topt))->v); _temp2049;}); struct _tuple1* _temp2045=({ struct _tuple1*
_temp2048=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2048->f1=
0; _temp2048->f2= Cyc_Toc_mt_tq; _temp2048->f3= Cyc_Absyn_uint_t; _temp2048;});
struct _tuple1* _temp2046=({ struct _tuple1* _temp2047=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2047->f1= 0; _temp2047->f2= Cyc_Toc_mt_tq;
_temp2047->f3= Cyc_Absyn_uint_t; _temp2047;}); struct _tuple1* _temp2042[ 3u]={
_temp2044, _temp2045, _temp2046}; struct _tagged_ptr7 _temp2043={ _temp2042,
_temp2042, _temp2042 + 3u}; _temp2041( _temp2043);}); goto _LL2051; _LL2051: {
void* _temp2055= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2052=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2052[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2053; _temp2053.tag= Cyc_Absyn_FnType;
_temp2053.f1=({ struct Cyc_Absyn_FnInfo _temp2054; _temp2054.tvars= 0; _temp2054.effect=
0; _temp2054.ret_typ=( void*) ta2; _temp2054.args= _temp2050; _temp2054.c_varargs=
0; _temp2054.cyc_varargs= 0; _temp2054.attributes= 0; _temp2054;}); _temp2053;});
_temp2052;}), Cyc_Toc_mt_tq); goto _LL2056; _LL2056: { struct Cyc_Absyn_Exp*
fnexp= Cyc_Absyn_cast_exp( _temp2055, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv,
0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_fncall_exp(
fnexp,({ struct Cyc_List_List*(* _temp2057)( struct _tagged_ptr6)=( struct Cyc_List_List*(*)(
struct _tagged_ptr6)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2060= _temp1434;
struct Cyc_Absyn_Exp* _temp2061= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2062= _temp1432; struct Cyc_Absyn_Exp* _temp2058[ 3u]={ _temp2060,
_temp2061, _temp2062}; struct _tagged_ptr6 _temp2059={ _temp2058, _temp2058,
_temp2058 + 3u}; _temp2057( _temp2059);}), 0), 0))->r));}}} else{ struct _tuple0*
x= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2005);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1432, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1434, s1, 0), 0))->r));} goto _LL2014;} _LL2014:;}
goto _LL1992; _LL1998:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp2063="exp_to_c: Subscript on non-tuple/array/tuple ptr"; struct
_tagged_string _temp2064; _temp2064.curr= _temp2063; _temp2064.base= _temp2063;
_temp2064.last_plus_one= _temp2063 + 49; _temp2064;})); goto _LL1992; _LL1992:;}
goto _LL1265;} _LL1315: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_tuple( nv, 0, 0, _temp1436))->r));} else{ struct Cyc_List_List*
_temp2065=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1436); goto
_LL2066; _LL2066: { struct _tagged_string* _temp2067= Cyc_Toc_add_tuple_type(
_temp2065); goto _LL2068; _LL2068: { struct Cyc_List_List* dles= 0;{ int i= 1;
for( 0; _temp1436 != 0;( _temp1436=(( struct Cyc_List_List*) _check_null(
_temp1436))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1436))->hd); dles=({ struct Cyc_List_List*
_temp2069=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2069->hd=( void*)({ struct _tuple4* _temp2070=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2070->f1= 0; _temp2070->f2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1436))->hd; _temp2070;}); _temp2069->tl=
dles; _temp2069;});}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, dles, 0))->r));}}} goto _LL1265; _LL1317:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ char* _temp2071="compoundlit"; struct _tagged_string _temp2072;
_temp2072.curr= _temp2071; _temp2072.base= _temp2071; _temp2072.last_plus_one=
_temp2071 + 12; _temp2072;})); goto _LL1265; _LL1319:( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1442, 0))->r));{ struct Cyc_List_List*
_temp2073= _temp1442; goto _LL2074; _LL2074: for( 0; _temp2073 != 0; _temp2073=((
struct Cyc_List_List*) _check_null( _temp2073))->tl){ struct _tuple4 _temp2077;
struct Cyc_Absyn_Exp* _temp2078; struct Cyc_List_List* _temp2080; struct _tuple4*
_temp2075=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp2073))->hd;
_temp2077=* _temp2075; _LL2081: _temp2080= _temp2077.f1; goto _LL2079; _LL2079:
_temp2078= _temp2077.f2; goto _LL2076; _LL2076: Cyc_Toc_exp_to_c( nv, _temp2078);}}
goto _LL1265; _LL1321: { unsigned int _temp2082= Cyc_Evexp_eval_const_uint_exp(
_temp1446); goto _LL2083; _LL2083: Cyc_Toc_exp_to_c( nv, _temp1444);{ struct Cyc_List_List*
es= 0; if( ! Cyc_Toc_is_zero( _temp1444)){ unsigned int i= 0; for( 0; i <
_temp2082; i ++){ es=({ struct Cyc_List_List* _temp2084=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2084->hd=( void*)({ struct
_tuple4* _temp2085=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp2085->f1= 0; _temp2085->f2= _temp1444; _temp2085;}); _temp2084->tl= es;
_temp2084;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
es, 0))->r)); goto _LL1265;}} _LL1323: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1452, _temp1456))->r));}
else{ if( _temp1450 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp2086="Struct_e: missing structdecl pointer"; struct _tagged_string
_temp2087; _temp2087.curr= _temp2086; _temp2087.base= _temp2086; _temp2087.last_plus_one=
_temp2086 + 37; _temp2087;}));}{ struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp1450); struct _RegionHandle _temp2088= _new_region(); struct
_RegionHandle* rgn=& _temp2088; _push_region( rgn);{ struct Cyc_List_List*
_temp2089=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1452,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); goto _LL2090; _LL2090: { struct Cyc_List_List*
_temp2091= 0; goto _LL2092; _LL2092:{ struct Cyc_List_List* _temp2093=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; goto
_LL2094; _LL2094: for( 0; _temp2093 != 0; _temp2093=(( struct Cyc_List_List*)
_check_null( _temp2093))->tl){ struct Cyc_List_List* _temp2095= _temp2089; goto
_LL2096; _LL2096: for( 0; _temp2095 != 0; _temp2095=(( struct Cyc_List_List*)
_check_null( _temp2095))->tl){ if((*(( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( _temp2095))->hd)).f1 ==( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2093))->hd){ struct _tuple12 _temp2099; struct
Cyc_Absyn_Exp* _temp2100; struct Cyc_Absyn_Structfield* _temp2102; struct
_tuple12* _temp2097=( struct _tuple12*)(( struct Cyc_List_List*) _check_null(
_temp2095))->hd; _temp2099=* _temp2097; _LL2103: _temp2102= _temp2099.f1; goto
_LL2101; _LL2101: _temp2100= _temp2099.f2; goto _LL2098; _LL2098: { void*
_temp2104=( void*) _temp2102->type; goto _LL2105; _LL2105: Cyc_Toc_exp_to_c( nv,
_temp2100); if( Cyc_Toc_is_void_star( _temp2104)){( void*)( _temp2100->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp2100->r, 0), 0))->r));} _temp2091=({ struct Cyc_List_List* _temp2106=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2106->hd=(
void*)({ struct _tuple4* _temp2107=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp2107->f1= 0; _temp2107->f2= _temp2100; _temp2107;}); _temp2106->tl=
_temp2091; _temp2106;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2091), 0))->r));}}; _pop_region(& _temp2088);}} goto _LL1265; _LL1325: {
struct Cyc_List_List* fs;{ void* _temp2108= Cyc_Tcutil_compress( _temp1460);
struct Cyc_List_List* _temp2114; _LL2110: if(( unsigned int) _temp2108 > 4u?*((
int*) _temp2108) == Cyc_Absyn_AnonStructType: 0){ _LL2115: _temp2114=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2108)->f1; goto _LL2111;} else{ goto
_LL2112;} _LL2112: goto _LL2113; _LL2111: fs= _temp2114; goto _LL2109; _LL2113:
fs=(( struct Cyc_List_List*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp2116= Cyc_Absynpp_typ2string( _temp1460); xprintf("anon struct has type %.*s",
_temp2116.last_plus_one - _temp2116.curr, _temp2116.curr);})); goto _LL2109;
_LL2109:;}{ struct _RegionHandle _temp2117= _new_region(); struct _RegionHandle*
rgn=& _temp2117; _push_region( rgn);{ struct Cyc_List_List* _temp2118=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1458, fs); goto _LL2119; _LL2119: for( 0; _temp2118 != 0;
_temp2118=(( struct Cyc_List_List*) _check_null( _temp2118))->tl){ struct
_tuple12 _temp2122; struct Cyc_Absyn_Exp* _temp2123; struct Cyc_Absyn_Structfield*
_temp2125; struct _tuple12* _temp2120=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( _temp2118))->hd; _temp2122=* _temp2120; _LL2126: _temp2125=
_temp2122.f1; goto _LL2124; _LL2124: _temp2123= _temp2122.f2; goto _LL2121;
_LL2121: { void* _temp2127=( void*) _temp2125->type; goto _LL2128; _LL2128: Cyc_Toc_exp_to_c(
nv, _temp2123); if( Cyc_Toc_is_void_star( _temp2127)){( void*)( _temp2123->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp2123->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1458, 0))->r));}; _pop_region(& _temp2117);} goto _LL1265;} _LL1327: {
struct _tuple0* qv= _temp1462->name; if( _temp1464->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp2129= _temp1464->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1464->fields))->v;
goto _LL2130; _LL2130: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2129))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp2129))->hd)->typs
== 0){ tag_count ++;} _temp2129=(( struct Cyc_List_List*) _check_null( _temp2129))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1265;} _LL1329: { struct _tuple0* _temp2131= _temp1472->name; goto
_LL2132; _LL2132: { struct Cyc_List_List* _temp2133= _temp1472->typs; goto
_LL2134; _LL2134: { struct _tuple0* _temp2135= Cyc_Toc_temp_var(); goto _LL2136;
_LL2136: { struct Cyc_Absyn_Exp* _temp2137= Cyc_Absyn_var_exp( _temp2135, 0);
goto _LL2138; _LL2138: { void* _temp2141= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2131,({ char* _temp2139="_struct"; struct _tagged_string _temp2140;
_temp2140.curr= _temp2139; _temp2140.base= _temp2139; _temp2140.last_plus_one=
_temp2139 + 8; _temp2140;}))); goto _LL2142; _LL2142: if( nv->toplevel){ struct
Cyc_Absyn_Exp* tag_exp; if( _temp1474->is_xtunion){ tag_exp= Cyc_Absyn_var_exp(
_temp2131, 0);} else{ int tag_count= 0; struct Cyc_List_List* _temp2143=
_temp1474->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1474->fields))->v; goto _LL2144; _LL2144: while( Cyc_Absyn_qvar_cmp(
_temp2131,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2143))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2143))->hd)->typs != 0){ tag_count ++;}
_temp2143=(( struct Cyc_List_List*) _check_null( _temp2143))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2145= 0; goto _LL2146;
_LL2146: for( 0; _temp1476 != 0;( _temp1476=(( struct Cyc_List_List*)
_check_null( _temp1476))->tl, _temp2133=(( struct Cyc_List_List*) _check_null(
_temp2133))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1476))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2133))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2145=({ struct Cyc_List_List* _temp2147=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2147[ 0]=({ struct Cyc_List_List
_temp2148; _temp2148.hd=( void*)({ struct _tuple4* _temp2149=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2149[ 0]=({ struct _tuple4
_temp2150; _temp2150.f1= 0; _temp2150.f2= cur_e; _temp2150;}); _temp2149;});
_temp2148.tl= _temp2145; _temp2148;}); _temp2147;});} _temp2145=({ struct Cyc_List_List*
_temp2151=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2151[ 0]=({ struct Cyc_List_List _temp2152; _temp2152.hd=( void*)({ struct
_tuple4* _temp2153=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2153[ 0]=({ struct _tuple4 _temp2154; _temp2154.f1= 0; _temp2154.f2=
tag_exp; _temp2154;}); _temp2153;}); _temp2152.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2145); _temp2152;}); _temp2151;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2155=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2155->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2131,({ char* _temp2156="_struct";
struct _tagged_string _temp2157; _temp2157.curr= _temp2156; _temp2157.base=
_temp2156; _temp2157.last_plus_one= _temp2156 + 8; _temp2157;})); _temp2155;}),
_temp2145, 0))->r));}} else{ struct Cyc_List_List* _temp2159=({ struct Cyc_List_List*
_temp2158=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2158->hd=( void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp2137, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp2131, 0), 0); _temp2158->tl=
0; _temp2158;}); goto _LL2160; _LL2160:{ int i= 1; for( 0; _temp1476 != 0;(((
_temp1476=(( struct Cyc_List_List*) _check_null( _temp1476))->tl, i ++)),
_temp2133=(( struct Cyc_List_List*) _check_null( _temp2133))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1476))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2133))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp2161= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp2137, Cyc_Toc_fieldname(
i), 0), cur_e, 0); goto _LL2162; _LL2162: _temp2159=({ struct Cyc_List_List*
_temp2163=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2163->hd=( void*) _temp2161; _temp2163->tl= _temp2159; _temp2163;});}}}{
struct Cyc_Absyn_Stmt* _temp2164= Cyc_Absyn_exp_stmt( _temp2137, 0); goto
_LL2165; _LL2165: { struct Cyc_Absyn_Stmt* _temp2167= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp2166=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2166->hd=( void*) _temp2164; _temp2166->tl= _temp2159; _temp2166;})), 0);
goto _LL2168; _LL2168:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp2135, _temp2141, 0, _temp2167, 0), 0))->r));}}} goto _LL1265;}}}}} _LL1331:
goto _LL1265; _LL1333: { void* t_c= Cyc_Toc_typ_to_c( _temp1488); if( _temp1490
!= 0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1490);
Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp(
rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} else{( void*)( e->r=( void*)((
void*)( Cyc_Toc_malloc_exp( _temp1488, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
goto _LL1265;} _LL1335: Cyc_Toc_stmt_to_c( nv, _temp1492); goto _LL1265; _LL1337:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp2169="UnresolvedMem";
struct _tagged_string _temp2170; _temp2170.curr= _temp2169; _temp2170.base=
_temp2169; _temp2170.last_plus_one= _temp2169 + 14; _temp2170;})); goto _LL1265;
_LL1339:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ char* _temp2171="codegen";
struct _tagged_string _temp2172; _temp2172.curr= _temp2171; _temp2172.base=
_temp2171; _temp2172.last_plus_one= _temp2171 + 8; _temp2172;})); goto _LL1265;
_LL1341:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ char* _temp2173="fill";
struct _tagged_string _temp2174; _temp2174.curr= _temp2173; _temp2174.base=
_temp2173; _temp2174.last_plus_one= _temp2173 + 5; _temp2174;})); goto _LL1265;
_LL1265:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct _tagged_string* succ_lab, struct
_tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
0);} struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct
Cyc_Absyn_Stmt* f3; } ; struct _tuple14{ struct _tuple0* f1; void* f2; } ;
struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static
struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct _RegionHandle*
rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp2175=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp2211; struct Cyc_Absyn_Vardecl _temp2213; struct Cyc_List_List* _temp2214;
struct Cyc_Core_Opt* _temp2216; struct Cyc_Absyn_Exp* _temp2218; void* _temp2220;
struct Cyc_Absyn_Tqual _temp2222; struct _tuple0* _temp2224; void* _temp2226;
struct Cyc_Absyn_Vardecl* _temp2228; int _temp2230; void* _temp2232; char
_temp2234; struct _tagged_string _temp2236; struct Cyc_Absyn_Enumfield*
_temp2238; struct Cyc_Absyn_Enumdecl* _temp2240; struct Cyc_List_List* _temp2242;
struct Cyc_List_List* _temp2244; struct Cyc_Absyn_Tunionfield* _temp2246; struct
Cyc_Absyn_Tuniondecl* _temp2248; struct Cyc_Absyn_Pat* _temp2251; struct Cyc_Absyn_Pat
_temp2253; struct Cyc_Position_Segment* _temp2254; struct Cyc_Core_Opt*
_temp2256; void* _temp2258; struct Cyc_List_List* _temp2260; struct Cyc_List_List*
_temp2262; struct Cyc_Absyn_Tunionfield* _temp2264; struct Cyc_Absyn_Tuniondecl*
_temp2266; struct Cyc_List_List* _temp2268; struct Cyc_List_List* _temp2270;
struct Cyc_Absyn_Tunionfield* _temp2272; struct Cyc_Absyn_Tuniondecl* _temp2274;
struct Cyc_List_List* _temp2276; struct Cyc_List_List* _temp2278; struct Cyc_List_List*
_temp2280; struct Cyc_Core_Opt* _temp2282; struct Cyc_Absyn_Structdecl*
_temp2284; struct Cyc_Absyn_Pat* _temp2286; struct _tuple0* _temp2288; struct
Cyc_List_List* _temp2290; struct Cyc_List_List* _temp2292; struct _tuple0*
_temp2294; struct Cyc_List_List* _temp2296; struct Cyc_List_List* _temp2298;
struct _tuple0* _temp2300; _LL2177: if(( unsigned int) _temp2175 > 2u?*(( int*)
_temp2175) == Cyc_Absyn_Var_p: 0){ _LL2212: _temp2211=(( struct Cyc_Absyn_Var_p_struct*)
_temp2175)->f1; _temp2213=* _temp2211; _LL2227: _temp2226=( void*) _temp2213.sc;
goto _LL2225; _LL2225: _temp2224= _temp2213.name; goto _LL2223; _LL2223:
_temp2222= _temp2213.tq; goto _LL2221; _LL2221: _temp2220=( void*) _temp2213.type;
goto _LL2219; _LL2219: _temp2218= _temp2213.initializer; goto _LL2217; _LL2217:
_temp2216= _temp2213.rgn; goto _LL2215; _LL2215: _temp2214= _temp2213.attributes;
goto _LL2178;} else{ goto _LL2179;} _LL2179: if( _temp2175 ==( void*) Cyc_Absyn_Wild_p){
goto _LL2180;} else{ goto _LL2181;} _LL2181: if(( unsigned int) _temp2175 > 2u?*((
int*) _temp2175) == Cyc_Absyn_Reference_p: 0){ _LL2229: _temp2228=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2175)->f1; goto _LL2182;} else{ goto _LL2183;} _LL2183: if( _temp2175 ==(
void*) Cyc_Absyn_Null_p){ goto _LL2184;} else{ goto _LL2185;} _LL2185: if((
unsigned int) _temp2175 > 2u?*(( int*) _temp2175) == Cyc_Absyn_Int_p: 0){
_LL2233: _temp2232=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp2175)->f1;
goto _LL2231; _LL2231: _temp2230=(( struct Cyc_Absyn_Int_p_struct*) _temp2175)->f2;
goto _LL2186;} else{ goto _LL2187;} _LL2187: if(( unsigned int) _temp2175 > 2u?*((
int*) _temp2175) == Cyc_Absyn_Char_p: 0){ _LL2235: _temp2234=(( struct Cyc_Absyn_Char_p_struct*)
_temp2175)->f1; goto _LL2188;} else{ goto _LL2189;} _LL2189: if(( unsigned int)
_temp2175 > 2u?*(( int*) _temp2175) == Cyc_Absyn_Float_p: 0){ _LL2237: _temp2236=((
struct Cyc_Absyn_Float_p_struct*) _temp2175)->f1; goto _LL2190;} else{ goto
_LL2191;} _LL2191: if(( unsigned int) _temp2175 > 2u?*(( int*) _temp2175) == Cyc_Absyn_Enum_p:
0){ _LL2241: _temp2240=(( struct Cyc_Absyn_Enum_p_struct*) _temp2175)->f1; goto
_LL2239; _LL2239: _temp2238=(( struct Cyc_Absyn_Enum_p_struct*) _temp2175)->f2;
goto _LL2192;} else{ goto _LL2193;} _LL2193: if(( unsigned int) _temp2175 > 2u?*((
int*) _temp2175) == Cyc_Absyn_Tunion_p: 0){ _LL2249: _temp2248=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2175)->f1; goto _LL2247; _LL2247: _temp2246=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2175)->f2; goto _LL2245; _LL2245: _temp2244=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2175)->f3; goto _LL2243; _LL2243: _temp2242=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2175)->f4; if( _temp2242 == 0){ goto _LL2194;} else{ goto _LL2195;}} else{
goto _LL2195;} _LL2195: if(( unsigned int) _temp2175 > 2u?*(( int*) _temp2175)
== Cyc_Absyn_Pointer_p: 0){ _LL2252: _temp2251=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2175)->f1; _temp2253=* _temp2251; _LL2259: _temp2258=( void*) _temp2253.r;
if(( unsigned int) _temp2258 > 2u?*(( int*) _temp2258) == Cyc_Absyn_Tunion_p: 0){
_LL2267: _temp2266=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2258)->f1; goto
_LL2265; _LL2265: _temp2264=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2258)->f2;
goto _LL2263; _LL2263: _temp2262=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2258)->f3;
goto _LL2261; _LL2261: _temp2260=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2258)->f4;
goto _LL2257;} else{ goto _LL2197;} _LL2257: _temp2256= _temp2253.topt; goto
_LL2255; _LL2255: _temp2254= _temp2253.loc; goto _LL2250;} else{ goto _LL2197;}
_LL2250: if( _temp2260 != 0){ goto _LL2196;} else{ goto _LL2197;} _LL2197: if((
unsigned int) _temp2175 > 2u?*(( int*) _temp2175) == Cyc_Absyn_Tunion_p: 0){
_LL2275: _temp2274=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2175)->f1; goto
_LL2273; _LL2273: _temp2272=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2175)->f2;
goto _LL2271; _LL2271: _temp2270=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2175)->f3;
goto _LL2269; _LL2269: _temp2268=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2175)->f4;
goto _LL2198;} else{ goto _LL2199;} _LL2199: if(( unsigned int) _temp2175 > 2u?*((
int*) _temp2175) == Cyc_Absyn_Tuple_p: 0){ _LL2277: _temp2276=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp2175)->f1; goto _LL2200;} else{ goto _LL2201;} _LL2201: if(( unsigned int)
_temp2175 > 2u?*(( int*) _temp2175) == Cyc_Absyn_Struct_p: 0){ _LL2285:
_temp2284=(( struct Cyc_Absyn_Struct_p_struct*) _temp2175)->f1; goto _LL2283;
_LL2283: _temp2282=(( struct Cyc_Absyn_Struct_p_struct*) _temp2175)->f2; goto
_LL2281; _LL2281: _temp2280=(( struct Cyc_Absyn_Struct_p_struct*) _temp2175)->f3;
goto _LL2279; _LL2279: _temp2278=(( struct Cyc_Absyn_Struct_p_struct*) _temp2175)->f4;
goto _LL2202;} else{ goto _LL2203;} _LL2203: if(( unsigned int) _temp2175 > 2u?*((
int*) _temp2175) == Cyc_Absyn_Pointer_p: 0){ _LL2287: _temp2286=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2175)->f1; goto _LL2204;} else{ goto _LL2205;} _LL2205: if(( unsigned int)
_temp2175 > 2u?*(( int*) _temp2175) == Cyc_Absyn_UnknownId_p: 0){ _LL2289:
_temp2288=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2175)->f1; goto _LL2206;}
else{ goto _LL2207;} _LL2207: if(( unsigned int) _temp2175 > 2u?*(( int*)
_temp2175) == Cyc_Absyn_UnknownCall_p: 0){ _LL2295: _temp2294=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2175)->f1; goto _LL2293; _LL2293: _temp2292=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2175)->f2; goto _LL2291; _LL2291: _temp2290=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2175)->f3; goto _LL2208;} else{ goto _LL2209;} _LL2209: if(( unsigned int)
_temp2175 > 2u?*(( int*) _temp2175) == Cyc_Absyn_UnknownFields_p: 0){ _LL2301:
_temp2300=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2175)->f1; goto
_LL2299; _LL2299: _temp2298=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2175)->f2; goto _LL2297; _LL2297: _temp2296=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2175)->f3; goto _LL2210;} else{ goto _LL2176;} _LL2178: nv= Cyc_Toc_add_varmap(
nv, _temp2224, r); goto _LL2180; _LL2180: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL2176; _LL2182: { struct _tuple0* _temp2302= Cyc_Toc_temp_var(); goto
_LL2303; _LL2303: decls=({ struct Cyc_List_List* _temp2304=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2304->hd=( void*)({
struct _tuple14* _temp2305=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2305->f1= _temp2302; _temp2305->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2305;}); _temp2304->tl= decls;
_temp2304;}); nv= Cyc_Toc_add_varmap( nv, _temp2228->name, Cyc_Absyn_var_exp(
_temp2302, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2302, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2176;} _LL2184: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2176; _LL2186: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2232, _temp2230, 0), succ_lab, fail_lab); goto _LL2176;
_LL2188: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2234, 0),
succ_lab, fail_lab); goto _LL2176; _LL2190: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2236, 0), succ_lab, fail_lab); goto _LL2176; _LL2192: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2306=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2306[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2307; _temp2307.tag= Cyc_Absyn_Enum_e;
_temp2307.f1= _temp2238->name; _temp2307.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2240; _temp2307.f3=( struct Cyc_Absyn_Enumfield*) _temp2238; _temp2307;});
_temp2306;}), 0), succ_lab, fail_lab); goto _LL2176; _LL2194: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2246->name, 0); if( ! _temp2248->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2176;} _LL2196: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2260);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2264->name,({ char*
_temp2342="_struct"; struct _tagged_string _temp2343; _temp2343.curr= _temp2342;
_temp2343.base= _temp2342; _temp2343.last_plus_one= _temp2342 + 8; _temp2343;}));
struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq(
tufstrct), Cyc_Toc_mt_tq), r, 0); struct Cyc_List_List* _temp2308=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2264->typs);
goto _LL2309; _LL2309: { struct Cyc_List_List* _temp2310=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2260);
goto _LL2311; _LL2311: for( 0; _temp2310 != 0;((( _temp2310=(( struct Cyc_List_List*)
_check_null( _temp2310))->tl, _temp2308=(( struct Cyc_List_List*) _check_null(
_temp2308))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp2312=( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp2310))->hd; goto _LL2313; _LL2313: {
void* _temp2314=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2308))->hd)).f2; goto _LL2315; _LL2315: { struct _tuple0* _temp2316= Cyc_Toc_temp_var();
goto _LL2317; _LL2317: { void* _temp2318=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2312->topt))->v; goto _LL2319; _LL2319: { void* _temp2320= Cyc_Toc_typ_to_c(
_temp2318); goto _LL2321; _LL2321: { struct _tagged_string* _temp2322= Cyc_Toc_fresh_label();
goto _LL2323; _LL2323: { struct Cyc_Absyn_Exp* _temp2325= Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp2324=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2324[ 0]= xprintf("f%d", cnt);
_temp2324;}), 0); goto _LL2326; _LL2326: if( Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c(
_temp2314))){ _temp2325= Cyc_Absyn_cast_exp( _temp2320, _temp2325, 0);} decls=({
struct Cyc_List_List* _temp2327=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2327->hd=( void*)({ struct _tuple14*
_temp2328=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2328->f1= _temp2316; _temp2328->f2= _temp2320; _temp2328;}); _temp2327->tl=
decls; _temp2327;});{ struct _tuple13 _temp2329= Cyc_Toc_xlate_pat( nv, rgn,
_temp2318, Cyc_Absyn_var_exp( _temp2316, 0), _temp2325, _temp2312, succ_lab,
fail_lab, decls); goto _LL2330; _LL2330: nv= _temp2329.f1; decls= _temp2329.f2;{
struct Cyc_Absyn_Stmt* _temp2331= _temp2329.f3; goto _LL2332; _LL2332: { struct
Cyc_Absyn_Stmt* _temp2333= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2316,
0), _temp2325, 0); goto _LL2334; _LL2334: { struct Cyc_Absyn_Stmt* _temp2335=
Cyc_Absyn_seq_stmt( _temp2333, _temp2331, 0); goto _LL2336; _LL2336: ss=({
struct Cyc_List_List* _temp2337=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2337->hd=( void*) Cyc_Absyn_label_stmt( _temp2322,
_temp2335, 0); _temp2337->tl= ss; _temp2337;}); succ_lab= _temp2322;}}}}}}}}}}}{
struct Cyc_Absyn_Exp* test_exp; if( _temp2266->is_xtunion){ struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0); struct Cyc_Absyn_Exp*
e= Cyc_Absyn_var_exp( _temp2264->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0);
s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp2338=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2266->fields))->v; goto _LL2339; _LL2339: for( 0; _temp2338 !=
0; _temp2338=(( struct Cyc_List_List*) _check_null( _temp2338))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2340=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2338))->hd; goto _LL2341; _LL2341: if( _temp2340->typs == 0){ max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r,
0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp,
0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2264->name, 0); test_exp=
max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2176;}}} _LL2198: _temp2276=
_temp2268; goto _LL2200; _LL2200: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2276);{ struct Cyc_List_List*
_temp2344=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2276); goto _LL2345; _LL2345: for( 0; _temp2344 !=
0;( _temp2344=(( struct Cyc_List_List*) _check_null( _temp2344))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2346=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2344))->hd; goto _LL2347; _LL2347: { struct _tuple0* _temp2348=
Cyc_Toc_temp_var(); goto _LL2349; _LL2349: { void* _temp2350=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2346->topt))->v; goto _LL2351; _LL2351: {
struct _tagged_string* _temp2352= Cyc_Toc_fresh_label(); goto _LL2353; _LL2353:
decls=({ struct Cyc_List_List* _temp2354=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2354->hd=( void*)({ struct _tuple14*
_temp2355=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2355->f1= _temp2348; _temp2355->f2= Cyc_Toc_typ_to_c( _temp2350); _temp2355;});
_temp2354->tl= decls; _temp2354;});{ struct _tuple13 _temp2356= Cyc_Toc_xlate_pat(
nv, rgn, _temp2350, Cyc_Absyn_var_exp( _temp2348, 0), Cyc_Absyn_structmember_exp(
path, Cyc_Toc_fieldname( cnt), 0), _temp2346, succ_lab, fail_lab, decls); goto
_LL2357; _LL2357: nv= _temp2356.f1; decls= _temp2356.f2;{ struct Cyc_Absyn_Stmt*
_temp2358= _temp2356.f3; goto _LL2359; _LL2359: { struct Cyc_Absyn_Stmt*
_temp2360= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2348, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2358, 0); goto _LL2361; _LL2361: ss=({ struct Cyc_List_List* _temp2362=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2362->hd=(
void*) Cyc_Absyn_label_stmt( _temp2352, _temp2360, 0); _temp2362->tl= ss;
_temp2362;}); succ_lab= _temp2352;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2176;} _LL2202: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2363=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2278); goto _LL2364; _LL2364: for( 0; _temp2363 !=
0; _temp2363=(( struct Cyc_List_List*) _check_null( _temp2363))->tl){ struct
_tuple15* _temp2365=( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2363))->hd; goto _LL2366; _LL2366: { struct Cyc_Absyn_Pat* _temp2367=(*
_temp2365).f2; goto _LL2368; _LL2368: { struct _tagged_string* f;{ void*
_temp2369=( void*)(( struct Cyc_List_List*) _check_null((* _temp2365).f1))->hd;
struct _tagged_string* _temp2375; _LL2371: if(*(( int*) _temp2369) == Cyc_Absyn_FieldName){
_LL2376: _temp2375=(( struct Cyc_Absyn_FieldName_struct*) _temp2369)->f1; goto
_LL2372;} else{ goto _LL2373;} _LL2373: goto _LL2374; _LL2372: f= _temp2375;
goto _LL2370; _LL2374:( void) _throw(( void*) Cyc_Toc_Match_error); _LL2370:;}{
struct _tuple0* _temp2377= Cyc_Toc_temp_var(); goto _LL2378; _LL2378: { void*
_temp2379=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2367->topt))->v;
goto _LL2380; _LL2380: { void* _temp2381= Cyc_Toc_typ_to_c( _temp2379); goto
_LL2382; _LL2382: { struct _tagged_string* _temp2383= Cyc_Toc_fresh_label();
goto _LL2384; _LL2384: decls=({ struct Cyc_List_List* _temp2385=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2385->hd=( void*)({
struct _tuple14* _temp2386=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2386->f1= _temp2377; _temp2386->f2= _temp2381; _temp2386;});
_temp2385->tl= decls; _temp2385;});{ struct _tuple13 _temp2387= Cyc_Toc_xlate_pat(
nv, rgn, _temp2379, Cyc_Absyn_var_exp( _temp2377, 0), Cyc_Absyn_structmember_exp(
path, f, 0), _temp2367, succ_lab, fail_lab, decls); goto _LL2388; _LL2388: nv=
_temp2387.f1; decls= _temp2387.f2;{ struct Cyc_Absyn_Exp* _temp2389= Cyc_Absyn_structmember_exp(
r, f, 0); goto _LL2390; _LL2390: if( Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2284->fields))->v, f)))->type)){ _temp2389= Cyc_Absyn_cast_exp(
_temp2381, _temp2389, 0);}{ struct Cyc_Absyn_Stmt* _temp2391= _temp2387.f3; goto
_LL2392; _LL2392: { struct Cyc_Absyn_Stmt* _temp2393= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2377, 0), _temp2389, 0), _temp2391, 0); goto _LL2394;
_LL2394: ss=({ struct Cyc_List_List* _temp2395=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2395->hd=( void*) Cyc_Absyn_label_stmt(
_temp2383, _temp2393, 0); _temp2395->tl= ss; _temp2395;}); succ_lab= _temp2383;}}}}}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2176;} _LL2204: { struct _tuple0*
_temp2396= Cyc_Toc_temp_var(); goto _LL2397; _LL2397: { void* _temp2398=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2286->topt))->v; goto _LL2399; _LL2399:
decls=({ struct Cyc_List_List* _temp2400=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2400->hd=( void*)({ struct _tuple14*
_temp2401=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2401->f1= _temp2396; _temp2401->f2= Cyc_Toc_typ_to_c( _temp2398); _temp2401;});
_temp2400->tl= decls; _temp2400;});{ struct _tuple13 _temp2402= Cyc_Toc_xlate_pat(
nv, rgn, _temp2398, Cyc_Absyn_var_exp( _temp2396, 0), Cyc_Absyn_deref_exp( path,
0), _temp2286, succ_lab, fail_lab, decls); goto _LL2403; _LL2403: nv= _temp2402.f1;
decls= _temp2402.f2;{ struct Cyc_Absyn_Stmt* _temp2404= _temp2402.f3; goto
_LL2405; _LL2405: { struct Cyc_Absyn_Stmt* _temp2406= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2396, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2404, 0);
goto _LL2407; _LL2407: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2406, 0);} else{ s= _temp2406;} goto _LL2176;}}}}} _LL2206: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp2408="unknownid"; struct _tagged_string _temp2409; _temp2409.curr=
_temp2408; _temp2409.base= _temp2408; _temp2409.last_plus_one= _temp2408 + 10;
_temp2409;})); goto _LL2176; _LL2208: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ char* _temp2410="unknowncall"; struct
_tagged_string _temp2411; _temp2411.curr= _temp2410; _temp2411.base= _temp2410;
_temp2411.last_plus_one= _temp2410 + 12; _temp2411;})); goto _LL2176; _LL2210: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp2412="unknownfields"; struct _tagged_string _temp2413; _temp2413.curr=
_temp2412; _temp2413.base= _temp2412; _temp2413.last_plus_one= _temp2412 + 14;
_temp2413;})); goto _LL2176; _LL2176:;} return({ struct _tuple13 _temp2414;
_temp2414.f1= nv; _temp2414.f2= decls; _temp2414.f3= s; _temp2414;});} struct
_tuple16{ struct _tagged_string* f1; struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple16* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple16* _temp2415=(
struct _tuple16*) _region_malloc( r, sizeof( struct _tuple16)); _temp2415->f1=
Cyc_Toc_fresh_label(); _temp2415->f2= Cyc_Toc_fresh_label(); _temp2415->f3= sc;
_temp2415;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp2416=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2417; _LL2417: { int leave_as_switch;{ void*
_temp2418= Cyc_Tcutil_compress( _temp2416); void* _temp2424; void* _temp2426;
_LL2420: if(( unsigned int) _temp2418 > 4u?*(( int*) _temp2418) == Cyc_Absyn_IntType:
0){ _LL2427: _temp2426=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2418)->f1;
goto _LL2425; _LL2425: _temp2424=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2418)->f2; goto _LL2421;} else{ goto _LL2422;} _LL2422: goto _LL2423;
_LL2421: leave_as_switch= 1; goto _LL2419; _LL2423: leave_as_switch= 0; goto
_LL2419; _LL2419:;}{ struct Cyc_List_List* _temp2428= scs; goto _LL2429; _LL2429:
for( 0; _temp2428 != 0; _temp2428=(( struct Cyc_List_List*) _check_null(
_temp2428))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2428))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2428))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2430= scs; goto
_LL2431; _LL2431: for( 0; _temp2430 != 0; _temp2430=(( struct Cyc_List_List*)
_check_null( _temp2430))->tl){ struct Cyc_Absyn_Stmt* _temp2432=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2430))->hd)->body; goto _LL2433;
_LL2433:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2430))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2432, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2432);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct
_tagged_string* end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp2434=
_new_region(); struct _RegionHandle* rgn=& _temp2434; _push_region( rgn);{
struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2435= lscs; goto
_LL2436; _LL2436: for( 0; _temp2435 != 0; _temp2435=(( struct Cyc_List_List*)
_check_null( _temp2435))->tl){ struct Cyc_Absyn_Switch_clause* sc=(*(( struct
_tuple16*)(( struct Cyc_List_List*) _check_null( _temp2435))->hd)).f3; struct
_tagged_string* fail_lab=(( struct Cyc_List_List*) _check_null( _temp2435))->tl
== 0? end_l:(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp2435))->tl))->hd)).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2435))->hd)).f2; if( sc->where_clause == 0){ struct _tuple13
_temp2437= Cyc_Toc_xlate_pat( nv, rgn, _temp2416, r, path, sc->pattern, succ_lab,
fail_lab, decls); goto _LL2438; _LL2438: nvs=({ struct Cyc_List_List* _temp2439=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2439->hd=( void*) _temp2437.f1; _temp2439->tl= nvs; _temp2439;}); decls=
_temp2437.f2; test_stmts=({ struct Cyc_List_List* _temp2440=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2440->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2435))->hd)).f1,
_temp2437.f3, 0); _temp2440->tl= test_stmts; _temp2440;});} else{ struct Cyc_Absyn_Exp*
_temp2441=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); goto _LL2442;
_LL2442: { struct _tagged_string* _temp2443= Cyc_Toc_fresh_label(); goto _LL2444;
_LL2444: { struct _tuple13 _temp2445= Cyc_Toc_xlate_pat( nv, rgn, _temp2416, r,
path, sc->pattern, _temp2443, fail_lab, decls); goto _LL2446; _LL2446: Cyc_Toc_exp_to_c(
_temp2445.f1, _temp2441); nvs=({ struct Cyc_List_List* _temp2447=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2447->hd=( void*)
_temp2445.f1; _temp2447->tl= nvs; _temp2447;}); decls= _temp2445.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2441, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2443, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2448=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2448->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2435))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2445.f3, s3, 0), 0); _temp2448->tl= test_stmts;
_temp2448;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2449= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2450; _LL2450: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0;( lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2451=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2452; _LL2452: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2451, end_l,(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2453=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2454; _LL2454: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2453, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2449, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple14 _temp2457;
void* _temp2458; struct _tuple0* _temp2460; struct _tuple14* _temp2455=( struct
_tuple14*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2457=*
_temp2455; _LL2461: _temp2460= _temp2457.f1; goto _LL2459; _LL2459: _temp2458=
_temp2457.f2; goto _LL2456; _LL2456: res= Cyc_Absyn_declare_stmt( _temp2460,
_temp2458, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region(& _temp2434);}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2462=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2462[ 0]=({ struct Cyc_List_List _temp2463; _temp2463.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2463.tl= 0; _temp2463;}); _temp2462;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2464=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2504; struct Cyc_Absyn_Stmt* _temp2506; struct Cyc_Absyn_Stmt*
_temp2508; struct Cyc_Absyn_Exp* _temp2510; struct Cyc_Absyn_Stmt* _temp2512;
struct Cyc_Absyn_Stmt* _temp2514; struct Cyc_Absyn_Exp* _temp2516; struct Cyc_Absyn_Stmt*
_temp2518; struct _tuple2 _temp2520; struct Cyc_Absyn_Stmt* _temp2522; struct
Cyc_Absyn_Exp* _temp2524; struct Cyc_Absyn_Stmt* _temp2526; struct Cyc_Absyn_Stmt*
_temp2528; struct Cyc_Absyn_Stmt* _temp2530; struct _tagged_string* _temp2532;
struct Cyc_Absyn_Stmt* _temp2534; struct _tuple2 _temp2536; struct Cyc_Absyn_Stmt*
_temp2538; struct Cyc_Absyn_Exp* _temp2540; struct _tuple2 _temp2542; struct Cyc_Absyn_Stmt*
_temp2544; struct Cyc_Absyn_Exp* _temp2546; struct Cyc_Absyn_Exp* _temp2548;
struct Cyc_List_List* _temp2550; struct Cyc_Absyn_Exp* _temp2552; struct Cyc_Absyn_Switch_clause**
_temp2554; struct Cyc_List_List* _temp2556; struct Cyc_Absyn_Stmt* _temp2558;
struct Cyc_Absyn_Decl* _temp2560; struct Cyc_Absyn_Stmt* _temp2562; struct Cyc_Absyn_Stmt*
_temp2564; struct Cyc_Absyn_Stmt* _temp2566; struct _tagged_string* _temp2568;
struct _tuple2 _temp2570; struct Cyc_Absyn_Stmt* _temp2572; struct Cyc_Absyn_Exp*
_temp2574; struct Cyc_Absyn_Stmt* _temp2576; struct Cyc_List_List* _temp2578;
struct Cyc_Absyn_Stmt* _temp2580; struct Cyc_Absyn_Stmt* _temp2582; struct Cyc_Absyn_Vardecl*
_temp2584; struct Cyc_Absyn_Tvar* _temp2586; _LL2466: if( _temp2464 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2467;} else{ goto _LL2468;} _LL2468: if((
unsigned int) _temp2464 > 1u?*(( int*) _temp2464) == Cyc_Absyn_Exp_s: 0){
_LL2505: _temp2504=(( struct Cyc_Absyn_Exp_s_struct*) _temp2464)->f1; goto
_LL2469;} else{ goto _LL2470;} _LL2470: if(( unsigned int) _temp2464 > 1u?*((
int*) _temp2464) == Cyc_Absyn_Seq_s: 0){ _LL2509: _temp2508=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2464)->f1; goto _LL2507; _LL2507: _temp2506=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2464)->f2; goto _LL2471;} else{ goto _LL2472;} _LL2472: if(( unsigned int)
_temp2464 > 1u?*(( int*) _temp2464) == Cyc_Absyn_Return_s: 0){ _LL2511:
_temp2510=(( struct Cyc_Absyn_Return_s_struct*) _temp2464)->f1; goto _LL2473;}
else{ goto _LL2474;} _LL2474: if(( unsigned int) _temp2464 > 1u?*(( int*)
_temp2464) == Cyc_Absyn_IfThenElse_s: 0){ _LL2517: _temp2516=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2464)->f1; goto _LL2515; _LL2515: _temp2514=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2464)->f2; goto _LL2513; _LL2513: _temp2512=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2464)->f3; goto _LL2475;} else{ goto _LL2476;} _LL2476: if(( unsigned int)
_temp2464 > 1u?*(( int*) _temp2464) == Cyc_Absyn_While_s: 0){ _LL2521: _temp2520=((
struct Cyc_Absyn_While_s_struct*) _temp2464)->f1; _LL2525: _temp2524= _temp2520.f1;
goto _LL2523; _LL2523: _temp2522= _temp2520.f2; goto _LL2519; _LL2519: _temp2518=((
struct Cyc_Absyn_While_s_struct*) _temp2464)->f2; goto _LL2477;} else{ goto
_LL2478;} _LL2478: if(( unsigned int) _temp2464 > 1u?*(( int*) _temp2464) == Cyc_Absyn_Break_s:
0){ _LL2527: _temp2526=(( struct Cyc_Absyn_Break_s_struct*) _temp2464)->f1; goto
_LL2479;} else{ goto _LL2480;} _LL2480: if(( unsigned int) _temp2464 > 1u?*((
int*) _temp2464) == Cyc_Absyn_Continue_s: 0){ _LL2529: _temp2528=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2464)->f1; goto _LL2481;} else{ goto _LL2482;} _LL2482: if(( unsigned int)
_temp2464 > 1u?*(( int*) _temp2464) == Cyc_Absyn_Goto_s: 0){ _LL2533: _temp2532=((
struct Cyc_Absyn_Goto_s_struct*) _temp2464)->f1; goto _LL2531; _LL2531:
_temp2530=(( struct Cyc_Absyn_Goto_s_struct*) _temp2464)->f2; goto _LL2483;}
else{ goto _LL2484;} _LL2484: if(( unsigned int) _temp2464 > 1u?*(( int*)
_temp2464) == Cyc_Absyn_For_s: 0){ _LL2549: _temp2548=(( struct Cyc_Absyn_For_s_struct*)
_temp2464)->f1; goto _LL2543; _LL2543: _temp2542=(( struct Cyc_Absyn_For_s_struct*)
_temp2464)->f2; _LL2547: _temp2546= _temp2542.f1; goto _LL2545; _LL2545:
_temp2544= _temp2542.f2; goto _LL2537; _LL2537: _temp2536=(( struct Cyc_Absyn_For_s_struct*)
_temp2464)->f3; _LL2541: _temp2540= _temp2536.f1; goto _LL2539; _LL2539:
_temp2538= _temp2536.f2; goto _LL2535; _LL2535: _temp2534=(( struct Cyc_Absyn_For_s_struct*)
_temp2464)->f4; goto _LL2485;} else{ goto _LL2486;} _LL2486: if(( unsigned int)
_temp2464 > 1u?*(( int*) _temp2464) == Cyc_Absyn_Switch_s: 0){ _LL2553:
_temp2552=(( struct Cyc_Absyn_Switch_s_struct*) _temp2464)->f1; goto _LL2551;
_LL2551: _temp2550=(( struct Cyc_Absyn_Switch_s_struct*) _temp2464)->f2; goto
_LL2487;} else{ goto _LL2488;} _LL2488: if(( unsigned int) _temp2464 > 1u?*((
int*) _temp2464) == Cyc_Absyn_Fallthru_s: 0){ _LL2557: _temp2556=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2464)->f1; goto _LL2555; _LL2555: _temp2554=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2464)->f2; goto _LL2489;} else{ goto _LL2490;} _LL2490: if(( unsigned int)
_temp2464 > 1u?*(( int*) _temp2464) == Cyc_Absyn_Decl_s: 0){ _LL2561: _temp2560=((
struct Cyc_Absyn_Decl_s_struct*) _temp2464)->f1; goto _LL2559; _LL2559:
_temp2558=(( struct Cyc_Absyn_Decl_s_struct*) _temp2464)->f2; goto _LL2491;}
else{ goto _LL2492;} _LL2492: if(( unsigned int) _temp2464 > 1u?*(( int*)
_temp2464) == Cyc_Absyn_Cut_s: 0){ _LL2563: _temp2562=(( struct Cyc_Absyn_Cut_s_struct*)
_temp2464)->f1; goto _LL2493;} else{ goto _LL2494;} _LL2494: if(( unsigned int)
_temp2464 > 1u?*(( int*) _temp2464) == Cyc_Absyn_Splice_s: 0){ _LL2565:
_temp2564=(( struct Cyc_Absyn_Splice_s_struct*) _temp2464)->f1; goto _LL2495;}
else{ goto _LL2496;} _LL2496: if(( unsigned int) _temp2464 > 1u?*(( int*)
_temp2464) == Cyc_Absyn_Label_s: 0){ _LL2569: _temp2568=(( struct Cyc_Absyn_Label_s_struct*)
_temp2464)->f1; goto _LL2567; _LL2567: _temp2566=(( struct Cyc_Absyn_Label_s_struct*)
_temp2464)->f2; goto _LL2497;} else{ goto _LL2498;} _LL2498: if(( unsigned int)
_temp2464 > 1u?*(( int*) _temp2464) == Cyc_Absyn_Do_s: 0){ _LL2577: _temp2576=((
struct Cyc_Absyn_Do_s_struct*) _temp2464)->f1; goto _LL2571; _LL2571: _temp2570=((
struct Cyc_Absyn_Do_s_struct*) _temp2464)->f2; _LL2575: _temp2574= _temp2570.f1;
goto _LL2573; _LL2573: _temp2572= _temp2570.f2; goto _LL2499;} else{ goto
_LL2500;} _LL2500: if(( unsigned int) _temp2464 > 1u?*(( int*) _temp2464) == Cyc_Absyn_TryCatch_s:
0){ _LL2581: _temp2580=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2464)->f1;
goto _LL2579; _LL2579: _temp2578=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2464)->f2; goto _LL2501;} else{ goto _LL2502;} _LL2502: if(( unsigned int)
_temp2464 > 1u?*(( int*) _temp2464) == Cyc_Absyn_Region_s: 0){ _LL2587:
_temp2586=(( struct Cyc_Absyn_Region_s_struct*) _temp2464)->f1; goto _LL2585;
_LL2585: _temp2584=(( struct Cyc_Absyn_Region_s_struct*) _temp2464)->f2; goto
_LL2583; _LL2583: _temp2582=(( struct Cyc_Absyn_Region_s_struct*) _temp2464)->f3;
goto _LL2503;} else{ goto _LL2465;} _LL2467: return; _LL2469: Cyc_Toc_exp_to_c(
nv, _temp2504); return; _LL2471: Cyc_Toc_stmt_to_c( nv, _temp2508); s= _temp2506;
continue; _LL2473: { struct Cyc_Core_Opt* topt= 0; if( _temp2510 != 0){ topt=({
struct Cyc_Core_Opt* _temp2588=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2588->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2510))->topt))->v);
_temp2588;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2510));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2589=
Cyc_Toc_temp_var(); goto _LL2590; _LL2590: { struct Cyc_Absyn_Stmt* _temp2591=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2589, 0),
0); goto _LL2592; _LL2592:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2589,( void*)(( struct Cyc_Core_Opt*) _check_null( topt))->v, _temp2510,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), _temp2591, 0), 0))->r));}}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2475: Cyc_Toc_exp_to_c(
nv, _temp2516); Cyc_Toc_stmt_to_c( nv, _temp2514); s= _temp2512; continue;
_LL2477: Cyc_Toc_exp_to_c( nv, _temp2524); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2518); return; _LL2479: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null( nv->break_lab))->v, 0))->r));}{ int dest_depth= _temp2526 == 0? 0:((
struct Cyc_Absyn_Stmt*) _check_null( _temp2526))->try_depth; Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2481: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -(( struct Cyc_Absyn_Stmt*) _check_null( _temp2528))->try_depth, s);
return; _LL2483: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2530))->try_depth, s); return; _LL2485: Cyc_Toc_exp_to_c( nv,
_temp2548); Cyc_Toc_exp_to_c( nv, _temp2546); Cyc_Toc_exp_to_c( nv, _temp2540);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2534); return; _LL2487: Cyc_Toc_xlate_switch(
nv, s, _temp2552, _temp2550); return; _LL2489: if( nv->fallthru_info == 0){(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ char*
_temp2593="fallthru in unexpected place"; struct _tagged_string _temp2594;
_temp2594.curr= _temp2593; _temp2594.base= _temp2593; _temp2594.last_plus_one=
_temp2593 + 29; _temp2594;})));}{ struct _tuple8 _temp2597; struct Cyc_Dict_Dict*
_temp2598; struct Cyc_List_List* _temp2600; struct _tagged_string* _temp2602;
struct _tuple8* _temp2595=( struct _tuple8*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp2597=* _temp2595; _LL2603: _temp2602= _temp2597.f1;
goto _LL2601; _LL2601: _temp2600= _temp2597.f2; goto _LL2599; _LL2599: _temp2598=
_temp2597.f3; goto _LL2596; _LL2596: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp2602, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp2554)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp2604=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2600); goto _LL2605; _LL2605: { struct Cyc_List_List* _temp2606=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp2556); goto
_LL2607; _LL2607: for( 0; _temp2604 != 0;( _temp2604=(( struct Cyc_List_List*)
_check_null( _temp2604))->tl, _temp2606=(( struct Cyc_List_List*) _check_null(
_temp2606))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp2606))->hd); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2598,( struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp2604))->hd),(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp2606))->hd, 0),
s2, 0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}}} _LL2491:{ void*
_temp2608=( void*) _temp2560->r; int _temp2618; struct Cyc_Absyn_Exp* _temp2620;
struct Cyc_Core_Opt* _temp2622; struct Cyc_Core_Opt* _temp2624; struct Cyc_Absyn_Pat*
_temp2626; struct Cyc_List_List* _temp2628; struct Cyc_Absyn_Vardecl* _temp2630;
_LL2610: if(*(( int*) _temp2608) == Cyc_Absyn_Let_d){ _LL2627: _temp2626=((
struct Cyc_Absyn_Let_d_struct*) _temp2608)->f1; goto _LL2625; _LL2625: _temp2624=((
struct Cyc_Absyn_Let_d_struct*) _temp2608)->f2; goto _LL2623; _LL2623: _temp2622=((
struct Cyc_Absyn_Let_d_struct*) _temp2608)->f3; goto _LL2621; _LL2621: _temp2620=((
struct Cyc_Absyn_Let_d_struct*) _temp2608)->f4; goto _LL2619; _LL2619: _temp2618=((
struct Cyc_Absyn_Let_d_struct*) _temp2608)->f5; goto _LL2611;} else{ goto
_LL2612;} _LL2612: if(*(( int*) _temp2608) == Cyc_Absyn_Letv_d){ _LL2629:
_temp2628=(( struct Cyc_Absyn_Letv_d_struct*) _temp2608)->f1; goto _LL2613;}
else{ goto _LL2614;} _LL2614: if(*(( int*) _temp2608) == Cyc_Absyn_Var_d){
_LL2631: _temp2630=(( struct Cyc_Absyn_Var_d_struct*) _temp2608)->f1; goto
_LL2615;} else{ goto _LL2616;} _LL2616: goto _LL2617; _LL2611:( void*)( s->r=(
void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2626, _temp2622, _temp2620,
_temp2618, _temp2558))->r)); goto _LL2609; _LL2613: { struct Cyc_List_List*
_temp2632=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2628); goto _LL2633; _LL2633: if( _temp2632 == 0){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ char* _temp2634="empty Letv_d"; struct
_tagged_string _temp2635; _temp2635.curr= _temp2634; _temp2635.base= _temp2634;
_temp2635.last_plus_one= _temp2634 + 13; _temp2635;}));}( void*)( _temp2560->r=(
void*)(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2636=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2636[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2637; _temp2637.tag= Cyc_Absyn_Var_d; _temp2637.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2632))->hd; _temp2637;}); _temp2636;})));
_temp2632=(( struct Cyc_List_List*) _check_null( _temp2632))->tl; for( 0;
_temp2632 != 0; _temp2632=(( struct Cyc_List_List*) _check_null( _temp2632))->tl){
struct Cyc_Absyn_Decl* _temp2640= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2638=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2638[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2639; _temp2639.tag= Cyc_Absyn_Var_d;
_temp2639.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2632))->hd; _temp2639;}); _temp2638;}), 0); goto _LL2641; _LL2641:( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp2640, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL2609;} _LL2615: {
struct _tuple0* _temp2642= _temp2630->name; goto _LL2643; _LL2643:( void*)(
_temp2630->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2630->type));{
struct Cyc_Toc_Env* _temp2646= Cyc_Toc_add_varmap( nv, _temp2642, Cyc_Absyn_varb_exp(
_temp2642,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp2644=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2644[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2645; _temp2645.tag= Cyc_Absyn_Local_b; _temp2645.f1=
_temp2630; _temp2645;}); _temp2644;}), 0)); goto _LL2647; _LL2647: Cyc_Toc_stmt_to_c(
_temp2646, _temp2558); if( _temp2630->initializer != 0){ struct Cyc_Absyn_Exp*
init=( struct Cyc_Absyn_Exp*) _check_null( _temp2630->initializer); void*
_temp2648=( void*) init->r; struct Cyc_Absyn_Exp* _temp2654; struct Cyc_Absyn_Exp*
_temp2656; struct Cyc_Absyn_Vardecl* _temp2658; _LL2650: if(*(( int*) _temp2648)
== Cyc_Absyn_Comprehension_e){ _LL2659: _temp2658=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2648)->f1; goto _LL2657; _LL2657: _temp2656=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2648)->f2; goto _LL2655; _LL2655: _temp2654=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2648)->f3; goto _LL2651;} else{ goto _LL2652;} _LL2652: goto _LL2653;
_LL2651: _temp2630->initializer= 0;( void*)( _temp2558->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2642, 0), _temp2658, _temp2656, _temp2654, Cyc_Absyn_new_stmt((
void*) _temp2558->r, 0), 0))->r)); goto _LL2649; _LL2653: Cyc_Toc_exp_to_c( nv,
init); goto _LL2649; _LL2649:;} goto _LL2609;}} _LL2617:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ char* _temp2660="bad nested declaration within function";
struct _tagged_string _temp2661; _temp2661.curr= _temp2660; _temp2661.base=
_temp2660; _temp2661.last_plus_one= _temp2660 + 39; _temp2661;})); goto _LL2609;
_LL2609:;} return; _LL2493:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
char* _temp2662="cut"; struct _tagged_string _temp2663; _temp2663.curr=
_temp2662; _temp2663.base= _temp2662; _temp2663.last_plus_one= _temp2662 + 4;
_temp2663;})); return; _LL2495:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
char* _temp2664="splice"; struct _tagged_string _temp2665; _temp2665.curr=
_temp2664; _temp2665.base= _temp2664; _temp2665.last_plus_one= _temp2664 + 7;
_temp2665;})); return; _LL2497: s= _temp2566; continue; _LL2499: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp2576); Cyc_Toc_exp_to_c( nv, _temp2574); return;
_LL2501: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__push_handler_qv,
0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp( Cyc_Toc_setjmp_qv, 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__pop_handler_qv,
0); struct Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp( Cyc_Toc__exn_thrown_qv,
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2666=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2666->v=( void*) e_typ; _temp2666;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2580);{
struct Cyc_Absyn_Stmt* _temp2667= Cyc_Absyn_seq_stmt( _temp2580, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); goto _LL2668; _LL2668: {
struct _tuple0* _temp2669= Cyc_Toc_temp_var(); goto _LL2670; _LL2670: { struct
Cyc_Absyn_Exp* _temp2671= Cyc_Absyn_var_exp( _temp2669, 0); goto _LL2672;
_LL2672: { struct Cyc_Absyn_Vardecl* _temp2673= Cyc_Absyn_new_vardecl( _temp2669,
Cyc_Absyn_exn_typ, 0); goto _LL2674; _LL2674: _temp2671->topt=({ struct Cyc_Core_Opt*
_temp2675=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2675->v=( void*) Cyc_Absyn_exn_typ; _temp2675;});{ struct Cyc_Absyn_Pat*
_temp2680=({ struct Cyc_Absyn_Pat* _temp2676=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2676->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2678=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2678[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2679; _temp2679.tag= Cyc_Absyn_Var_p;
_temp2679.f1= _temp2673; _temp2679;}); _temp2678;})); _temp2676->topt=({ struct
Cyc_Core_Opt* _temp2677=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2677->v=( void*) Cyc_Absyn_exn_typ; _temp2677;}); _temp2676->loc= 0;
_temp2676;}); goto _LL2681; _LL2681: { struct Cyc_Absyn_Exp* _temp2682= Cyc_Absyn_throw_exp(
_temp2671, 0); goto _LL2683; _LL2683: _temp2682->topt=({ struct Cyc_Core_Opt*
_temp2684=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2684->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2684;});{ struct Cyc_Absyn_Stmt*
_temp2685= Cyc_Absyn_exp_stmt( _temp2682, 0); goto _LL2686; _LL2686: { struct
Cyc_Absyn_Switch_clause* _temp2690=({ struct Cyc_Absyn_Switch_clause* _temp2687=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2687->pattern= _temp2680; _temp2687->pat_vars=({ struct Cyc_Core_Opt*
_temp2688=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2688->v=( void*)({ struct Cyc_List_List* _temp2689=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2689->hd=( void*) _temp2673;
_temp2689->tl= 0; _temp2689;}); _temp2688;}); _temp2687->where_clause= 0;
_temp2687->body= _temp2685; _temp2687->loc= 0; _temp2687;}); goto _LL2691;
_LL2691: { struct Cyc_Absyn_Stmt* _temp2693= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2578,({ struct Cyc_List_List* _temp2692=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2692->hd=( void*) _temp2690; _temp2692->tl=
0; _temp2692;})), 0); goto _LL2694; _LL2694: Cyc_Toc_stmt_to_c( nv, _temp2693);{
struct Cyc_Absyn_Exp* _temp2696= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2695=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2695->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2695->tl= 0; _temp2695;}), 0); goto _LL2697; _LL2697: { struct Cyc_Absyn_Stmt*
_temp2699= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2698=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2698->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2698->tl= 0;
_temp2698;}), 0), 0); goto _LL2700; _LL2700: { struct Cyc_Absyn_Exp* _temp2701=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2702; _LL2702: {
struct Cyc_Absyn_Exp* _temp2703= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2704; _LL2704:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2699, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2701, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2696, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2703, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2667, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2693, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2503: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2584->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2710=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2710[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2711; _temp2711.tag= Cyc_Absyn_Local_b; _temp2711.f1=
_temp2584; _temp2711;}); _temp2710;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2709=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2709->f1= Cyc_Toc_abs_ns; _temp2709->f2= Cyc_Toc__push_region_sp;
_temp2709;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2708=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2708->f1= Cyc_Toc_abs_ns; _temp2708->f2= Cyc_Toc__pop_region_sp; _temp2708;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2707=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2707->f1=
Cyc_Toc_abs_ns; _temp2707->f2= Cyc_Toc__new_region_sp; _temp2707;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2706=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2706->hd=( void*) addr_rh_exp; _temp2706->tl= 0; _temp2706;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2705=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2705->hd=( void*) x_exp; _temp2705->tl= 0; _temp2705;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2582);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2582, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2465:;}} struct _tuple17{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0;(
void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{ struct
Cyc_List_List* _temp2712= f->args; goto _LL2713; _LL2713: for( 0; _temp2712 != 0;
_temp2712=(( struct Cyc_List_List*) _check_null( _temp2712))->tl){(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp2712))->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp2712))->hd)).f3);{
struct _tuple0* _temp2715=({ struct _tuple0* _temp2714=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2714->f1=( void*) Cyc_Absyn_Loc_n;
_temp2714->f2=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp2712))->hd)).f1; _temp2714;}); goto _LL2716; _LL2716: nv= Cyc_Toc_add_varmap(
nv, _temp2715, Cyc_Absyn_var_exp( _temp2715, 0));}}} if(( unsigned int) f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs))->name != 0: 0){ int
_temp2719; void* _temp2721; void* _temp2723; struct Cyc_Absyn_Tqual _temp2725;
struct Cyc_Core_Opt* _temp2727; struct Cyc_Absyn_VarargInfo _temp2717=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs)); _LL2728: _temp2727=
_temp2717.name; goto _LL2726; _LL2726: _temp2725= _temp2717.tq; goto _LL2724;
_LL2724: _temp2723=( void*) _temp2717.type; goto _LL2722; _LL2722: _temp2721=(
void*) _temp2717.rgn; goto _LL2720; _LL2720: _temp2719= _temp2717.inject; goto
_LL2718; _LL2718: { void* _temp2729= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp2723, _temp2721, _temp2725)); goto _LL2730; _LL2730: { struct _tuple0*
_temp2732=({ struct _tuple0* _temp2731=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2731->f1=( void*) Cyc_Absyn_Loc_n; _temp2731->f2=( struct
_tagged_string*)(( struct Cyc_Core_Opt*) _check_null( _temp2727))->v; _temp2731;});
goto _LL2733; _LL2733: f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2734=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2734->hd=( void*)({ struct _tuple17* _temp2735=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp2735->f1=( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( _temp2727))->v; _temp2735->f2= _temp2725;
_temp2735->f3= _temp2729; _temp2735;}); _temp2734->tl= 0; _temp2734;})); nv= Cyc_Toc_add_varmap(
nv, _temp2732, Cyc_Absyn_var_exp( _temp2732, 0)); f->cyc_varargs= 0;}}}{ struct
Cyc_List_List* _temp2736=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; goto _LL2737; _LL2737: for( 0; _temp2736 !=
0; _temp2736=(( struct Cyc_List_List*) _check_null( _temp2736))->tl){( void*)(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp2736))->hd)->type=(
void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp2736))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2738= s;
_LL2740: if( _temp2738 ==( void*) Cyc_Absyn_Abstract){ goto _LL2741;} else{ goto
_LL2742;} _LL2742: if( _temp2738 ==( void*) Cyc_Absyn_ExternC){ goto _LL2743;}
else{ goto _LL2744;} _LL2744: goto _LL2745; _LL2741: return( void*) Cyc_Absyn_Public;
_LL2743: return( void*) Cyc_Absyn_Extern; _LL2745: return s; _LL2739:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2746=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2747; _LL2747: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2748=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2748->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2748;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2749=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2746).f2); goto _LL2750;
_LL2750: if( _temp2749 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2746).f2, s));}
else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null(
_temp2749))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2746).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2751=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2751->v=( void*)
_temp2746; _temp2751;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2752=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2753; _LL2753: for( 0; _temp2752 != 0;
_temp2752=(( struct Cyc_List_List*) _check_null( _temp2752))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2752))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2752))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2754=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2755; _LL2755: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2756=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2756->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2756;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2757=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2754).f2); goto _LL2758; _LL2758:
if( _temp2757 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2754).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2757))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2754).f2, u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)(
u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==(
void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?( void*)
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2759=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2759->v=( void*) _temp2754; _temp2759;}); if( u->fields !=
0){ if( seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* _temp2760=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( u->fields))->v; goto
_LL2761; _LL2761: for( 0; _temp2760 != 0; _temp2760=(( struct Cyc_List_List*)
_check_null( _temp2760))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2760))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2760))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2762=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2762->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2762;});}{ struct
_tuple0* _temp2763= tud->name; goto _LL2764; _LL2764: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2763)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2763));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2765=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2766; _LL2766: for( 0;
_temp2765 != 0; _temp2765=(( struct Cyc_List_List*) _check_null( _temp2765))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2765))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2767; _temp2767.q_const= 1; _temp2767.q_volatile= 0;
_temp2767.q_restrict= 0; _temp2767;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2768=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2768->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2769=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2769[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2770; _temp2770.tag= Cyc_Absyn_Var_d;
_temp2770.f1= vd; _temp2770;}); _temp2769;}), 0); _temp2768->tl= Cyc_Toc_result_decls;
_temp2768;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2771; _temp2771.q_const=
1; _temp2771.q_volatile= 0; _temp2771.q_restrict= 0; _temp2771;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2772=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2772->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2773=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2773[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2774; _temp2774.tag= Cyc_Absyn_Var_d; _temp2774.f1= vd; _temp2774;});
_temp2773;}), 0); _temp2772->tl= Cyc_Toc_result_decls; _temp2772;});{ struct Cyc_List_List*
_temp2775= 0; goto _LL2776; _LL2776: { int i= 1;{ struct Cyc_List_List*
_temp2777= f->typs; goto _LL2778; _LL2778: for( 0; _temp2777 != 0;( _temp2777=((
struct Cyc_List_List*) _check_null( _temp2777))->tl, i ++)){ struct
_tagged_string* _temp2779= Cyc_Toc_fieldname( i); goto _LL2780; _LL2780: {
struct Cyc_Absyn_Structfield* _temp2782=({ struct Cyc_Absyn_Structfield*
_temp2781=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2781->name= _temp2779; _temp2781->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2777))->hd)).f1; _temp2781->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2777))->hd)).f2);
_temp2781->width= 0; _temp2781->attributes= 0; _temp2781;}); goto _LL2783;
_LL2783: _temp2775=({ struct Cyc_List_List* _temp2784=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2784->hd=( void*) _temp2782;
_temp2784->tl= _temp2775; _temp2784;});}}} _temp2775=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2775); _temp2775=({ struct Cyc_List_List*
_temp2785=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2785->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2786=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2786->name= Cyc_Toc_tag_sp;
_temp2786->tq= Cyc_Toc_mt_tq; _temp2786->type=( void*) Cyc_Absyn_sint_t;
_temp2786->width= 0; _temp2786->attributes= 0; _temp2786;}); _temp2785->tl=
_temp2775; _temp2785;});{ struct Cyc_Absyn_Structdecl* _temp2792=({ struct Cyc_Absyn_Structdecl*
_temp2787=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2787->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2787->name=({ struct Cyc_Core_Opt*
_temp2789=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2789->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ char* _temp2790="_struct";
struct _tagged_string _temp2791; _temp2791.curr= _temp2790; _temp2791.base=
_temp2790; _temp2791.last_plus_one= _temp2790 + 8; _temp2791;})); _temp2789;});
_temp2787->tvs= 0; _temp2787->fields=({ struct Cyc_Core_Opt* _temp2788=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2788->v=( void*)
_temp2775; _temp2788;}); _temp2787->attributes= 0; _temp2787;}); goto _LL2793;
_LL2793: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2794=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2794->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2795=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2795[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2796; _temp2796.tag= Cyc_Absyn_Struct_d;
_temp2796.f1= _temp2792; _temp2796;}); _temp2795;}), 0); _temp2794->tl= Cyc_Toc_result_decls;
_temp2794;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2797=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2797->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2797;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2798= xd->name; goto _LL2799; _LL2799: {
struct Cyc_List_List* _temp2800=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2801; _LL2801: for( 0; _temp2800 != 0;
_temp2800=(( struct Cyc_List_List*) _check_null( _temp2800))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2800))->hd; struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2803= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2802=* fn;(
unsigned int)( _temp2802.last_plus_one - _temp2802.curr);}) + 4, 0); goto
_LL2804; _LL2804: { struct Cyc_Absyn_ArrayType_struct* _temp2807=({ struct Cyc_Absyn_ArrayType_struct*
_temp2805=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2805[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2806; _temp2806.tag= Cyc_Absyn_ArrayType;
_temp2806.f1=( void*) Cyc_Absyn_uchar_t; _temp2806.f2= Cyc_Toc_mt_tq; _temp2806.f3=(
struct Cyc_Absyn_Exp*) _temp2803; _temp2806;}); _temp2805;}); goto _LL2808;
_LL2808: { struct Cyc_Core_Opt* _temp2809=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2817; int _temp2818; _LL2811: if( _temp2809 == 0){ goto
_LL2812;} else{ goto _LL2813;} _LL2813: if( _temp2809 == 0){ goto _LL2815;}
else{ _temp2817=* _temp2809; _LL2819: _temp2818=( int) _temp2817.v; if(
_temp2818 == 0){ goto _LL2814;} else{ goto _LL2815;}} _LL2815: goto _LL2816;
_LL2812: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(({ int _temp2820= 0; int
_temp2821= 0; int _temp2822= 0; int _temp2823= 0; struct _tagged_string
_temp2824=* fn; xprintf("%c%c%c%c%.*s", _temp2820, _temp2821, _temp2822,
_temp2823, _temp2824.last_plus_one - _temp2824.curr, _temp2824.curr);}), 0);}{
struct Cyc_Absyn_Vardecl* _temp2825= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2807, initopt); goto _LL2826; _LL2826:( void*)( _temp2825->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2827=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2827->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2828=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2828[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2829; _temp2829.tag= Cyc_Absyn_Var_d;
_temp2829.f1= _temp2825; _temp2829;}); _temp2828;}), 0); _temp2827->tl= Cyc_Toc_result_decls;
_temp2827;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2830= f->typs; goto _LL2831; _LL2831: for( 0;
_temp2830 != 0;( _temp2830=(( struct Cyc_List_List*) _check_null( _temp2830))->tl,
i ++)){ struct _tagged_string* _temp2833=( struct _tagged_string*)({ struct
_tagged_string* _temp2832=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2832[ 0]= xprintf("f%d", i); _temp2832;}); goto
_LL2834; _LL2834: { struct Cyc_Absyn_Structfield* _temp2836=({ struct Cyc_Absyn_Structfield*
_temp2835=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2835->name= _temp2833; _temp2835->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2830))->hd)).f1; _temp2835->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2830))->hd)).f2);
_temp2835->width= 0; _temp2835->attributes= 0; _temp2835;}); goto _LL2837;
_LL2837: fields=({ struct Cyc_List_List* _temp2838=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2838->hd=( void*) _temp2836;
_temp2838->tl= fields; _temp2838;});}}} fields=({ struct Cyc_List_List*
_temp2839=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2839->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2840=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2840->name= Cyc_Toc_tag_sp;
_temp2840->tq= Cyc_Toc_mt_tq; _temp2840->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2840->width= 0; _temp2840->attributes= 0; _temp2840;});
_temp2839->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2839;});{ struct Cyc_Absyn_Structdecl* _temp2846=({ struct Cyc_Absyn_Structdecl*
_temp2841=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2841->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2841->name=({ struct Cyc_Core_Opt*
_temp2843=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2843->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ char* _temp2844="_struct";
struct _tagged_string _temp2845; _temp2845.curr= _temp2844; _temp2845.base=
_temp2844; _temp2845.last_plus_one= _temp2844 + 8; _temp2845;})); _temp2843;});
_temp2841->tvs= 0; _temp2841->fields=({ struct Cyc_Core_Opt* _temp2842=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2842->v=( void*)
fields; _temp2842;}); _temp2841->attributes= 0; _temp2841;}); goto _LL2847;
_LL2847: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2848=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2848->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2849=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2849[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2850; _temp2850.tag= Cyc_Absyn_Struct_d;
_temp2850.f1= _temp2846; _temp2850;}); _temp2849;}), 0); _temp2848->tl= Cyc_Toc_result_decls;
_temp2848;});}} goto _LL2810;}} _LL2814: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2856= Cyc_Absyn_string_exp(({ int _temp2851= 0; int
_temp2852= 0; int _temp2853= 0; int _temp2854= 0; struct _tagged_string
_temp2855=* fn; xprintf("%c%c%c%c%.*s", _temp2851, _temp2852, _temp2853,
_temp2854, _temp2855.last_plus_one - _temp2855.curr, _temp2855.curr);}), 0);
goto _LL2857; _LL2857: { struct Cyc_Absyn_Vardecl* _temp2858= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2807,( struct Cyc_Absyn_Exp*) _temp2856); goto _LL2859;
_LL2859:( void*)( _temp2858->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2860=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2860->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2861=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2861[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2862; _temp2862.tag= Cyc_Absyn_Var_d; _temp2862.f1= _temp2858; _temp2862;});
_temp2861;}), 0); _temp2860->tl= Cyc_Toc_result_decls; _temp2860;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2810; _LL2816: goto
_LL2810; _LL2810:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2863= ed->name; goto
_LL2864; _LL2864:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2865=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; goto _LL2866; _LL2866: for( 0; _temp2865 != 0;
_temp2865=(( struct Cyc_List_List*) _check_null( _temp2865))->tl){ if((( struct
Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( _temp2865))->hd)->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp2865))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2865))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct _RegionHandle
_temp2867= _new_region(); struct _RegionHandle* prgn=& _temp2867; _push_region(
prgn);{ struct Cyc_Absyn_Stmt* _temp2870; struct Cyc_List_List* _temp2872;
struct Cyc_Toc_Env* _temp2874; struct _tuple13 _temp2868= Cyc_Toc_xlate_pat( nv,
prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2875: _temp2874= _temp2868.f1; goto _LL2873; _LL2873: _temp2872=
_temp2868.f2; goto _LL2871; _LL2871: _temp2870= _temp2868.f3; goto _LL2869;
_LL2869: Cyc_Toc_stmt_to_c( _temp2874, s);{ struct Cyc_Absyn_Stmt* _temp2876=
Cyc_Absyn_label_stmt( succ_lab, s, 0); goto _LL2877; _LL2877: if( exhaust){ s=
Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2870, _temp2876, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2879= Cyc_Toc_newthrow_exp(
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2878=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2878->f1= Cyc_Toc_abs_ns; _temp2878->f2= Cyc_Toc_Match_Exception_sp;
_temp2878;}), 0)); goto _LL2880; _LL2880: { struct Cyc_Absyn_Stmt* _temp2881=
Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2879, 0), 0); goto
_LL2882; _LL2882: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2870, Cyc_Absyn_seq_stmt( _temp2881,
_temp2876, 0), 0), 0);}} for( 0; _temp2872 != 0; _temp2872=(( struct Cyc_List_List*)
_check_null( _temp2872))->tl){ struct _tuple14 _temp2885; void* _temp2886;
struct _tuple0* _temp2888; struct _tuple14* _temp2883=( struct _tuple14*)((
struct Cyc_List_List*) _check_null( _temp2872))->hd; _temp2885=* _temp2883;
_LL2889: _temp2888= _temp2885.f1; goto _LL2887; _LL2887: _temp2886= _temp2885.f2;
goto _LL2884; _LL2884: s= Cyc_Absyn_declare_stmt( _temp2888, _temp2886, 0, s, 0);}}};
_pop_region(& _temp2867);} return s;}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=((
struct Cyc_List_List*) _check_null( ds))->tl){ struct Cyc_Absyn_Decl* d=( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2890=(
void*) d->r; struct Cyc_Absyn_Vardecl* _temp2916; struct Cyc_Absyn_Fndecl*
_temp2918; int _temp2920; struct Cyc_Absyn_Exp* _temp2922; struct Cyc_Core_Opt*
_temp2924; struct Cyc_Core_Opt* _temp2926; struct Cyc_Absyn_Pat* _temp2928;
struct Cyc_List_List* _temp2930; struct Cyc_Absyn_Structdecl* _temp2932; struct
Cyc_Absyn_Uniondecl* _temp2934; struct Cyc_Absyn_Tuniondecl* _temp2936; struct
Cyc_Absyn_Enumdecl* _temp2938; struct Cyc_Absyn_Typedefdecl* _temp2940; struct
Cyc_List_List* _temp2942; struct _tagged_string* _temp2944; struct Cyc_List_List*
_temp2946; struct _tuple0* _temp2948; struct Cyc_List_List* _temp2950; _LL2892:
if(*(( int*) _temp2890) == Cyc_Absyn_Var_d){ _LL2917: _temp2916=(( struct Cyc_Absyn_Var_d_struct*)
_temp2890)->f1; goto _LL2893;} else{ goto _LL2894;} _LL2894: if(*(( int*)
_temp2890) == Cyc_Absyn_Fn_d){ _LL2919: _temp2918=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2890)->f1; goto _LL2895;} else{ goto _LL2896;} _LL2896: if(*(( int*)
_temp2890) == Cyc_Absyn_Let_d){ _LL2929: _temp2928=(( struct Cyc_Absyn_Let_d_struct*)
_temp2890)->f1; goto _LL2927; _LL2927: _temp2926=(( struct Cyc_Absyn_Let_d_struct*)
_temp2890)->f2; goto _LL2925; _LL2925: _temp2924=(( struct Cyc_Absyn_Let_d_struct*)
_temp2890)->f3; goto _LL2923; _LL2923: _temp2922=(( struct Cyc_Absyn_Let_d_struct*)
_temp2890)->f4; goto _LL2921; _LL2921: _temp2920=(( struct Cyc_Absyn_Let_d_struct*)
_temp2890)->f5; goto _LL2897;} else{ goto _LL2898;} _LL2898: if(*(( int*)
_temp2890) == Cyc_Absyn_Letv_d){ _LL2931: _temp2930=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2890)->f1; goto _LL2899;} else{ goto _LL2900;} _LL2900: if(*(( int*)
_temp2890) == Cyc_Absyn_Struct_d){ _LL2933: _temp2932=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2890)->f1; goto _LL2901;} else{ goto _LL2902;} _LL2902: if(*(( int*)
_temp2890) == Cyc_Absyn_Union_d){ _LL2935: _temp2934=(( struct Cyc_Absyn_Union_d_struct*)
_temp2890)->f1; goto _LL2903;} else{ goto _LL2904;} _LL2904: if(*(( int*)
_temp2890) == Cyc_Absyn_Tunion_d){ _LL2937: _temp2936=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2890)->f1; goto _LL2905;} else{ goto _LL2906;} _LL2906: if(*(( int*)
_temp2890) == Cyc_Absyn_Enum_d){ _LL2939: _temp2938=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2890)->f1; goto _LL2907;} else{ goto _LL2908;} _LL2908: if(*(( int*)
_temp2890) == Cyc_Absyn_Typedef_d){ _LL2941: _temp2940=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2890)->f1; goto _LL2909;} else{ goto _LL2910;} _LL2910: if(*(( int*)
_temp2890) == Cyc_Absyn_Namespace_d){ _LL2945: _temp2944=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2890)->f1; goto _LL2943; _LL2943: _temp2942=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2890)->f2; goto _LL2911;} else{ goto _LL2912;} _LL2912: if(*(( int*)
_temp2890) == Cyc_Absyn_Using_d){ _LL2949: _temp2948=(( struct Cyc_Absyn_Using_d_struct*)
_temp2890)->f1; goto _LL2947; _LL2947: _temp2946=(( struct Cyc_Absyn_Using_d_struct*)
_temp2890)->f2; goto _LL2913;} else{ goto _LL2914;} _LL2914: if(*(( int*)
_temp2890) == Cyc_Absyn_ExternC_d){ _LL2951: _temp2950=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2890)->f1; goto _LL2915;} else{ goto _LL2891;} _LL2893: { struct _tuple0*
_temp2952= _temp2916->name; goto _LL2953; _LL2953: if(( void*) _temp2916->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2952=({ struct _tuple0* _temp2954=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2954->f1= Cyc_Toc_abs_ns;
_temp2954->f2=(* _temp2952).f2; _temp2954;});} if( _temp2916->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null( _temp2916->initializer));}
if( ! nv->toplevel){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
char* _temp2955="decls_to_c: not at toplevel!"; struct _tagged_string _temp2956;
_temp2956.curr= _temp2955; _temp2956.base= _temp2955; _temp2956.last_plus_one=
_temp2955 + 29; _temp2956;}));} nv= Cyc_Toc_add_varmap( nv, _temp2916->name, Cyc_Absyn_varb_exp(
_temp2952,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp2957=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp2957[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp2958; _temp2958.tag= Cyc_Absyn_Global_b;
_temp2958.f1= _temp2916; _temp2958;}); _temp2957;}), 0)); _temp2916->name=
_temp2952;( void*)( _temp2916->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2916->sc));(
void*)( _temp2916->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2916->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2959=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2959->hd=( void*) d; _temp2959->tl=
Cyc_Toc_result_decls; _temp2959;}); goto _LL2891;} _LL2895: { struct _tuple0*
_temp2960= _temp2918->name; goto _LL2961; _LL2961: if(( void*) _temp2918->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2960=({ struct _tuple0* _temp2962=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2962->f1= Cyc_Toc_abs_ns;
_temp2962->f2=(* _temp2960).f2; _temp2962;});} nv= Cyc_Toc_add_varmap( nv,
_temp2918->name, Cyc_Absyn_var_exp( _temp2960, 0)); _temp2918->name= _temp2960;
Cyc_Toc_fndecl_to_c( nv, _temp2918); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2963=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2963->hd=( void*) d; _temp2963->tl= Cyc_Toc_result_decls; _temp2963;});
goto _LL2891;} _LL2897:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp2964="letdecl"; struct _tagged_string _temp2965; _temp2965.curr=
_temp2964; _temp2965.base= _temp2964; _temp2965.last_plus_one= _temp2964 + 8;
_temp2965;})); goto _LL2891; _LL2899:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
char* _temp2966="letdecl"; struct _tagged_string _temp2967; _temp2967.curr=
_temp2966; _temp2967.base= _temp2966; _temp2967.last_plus_one= _temp2966 + 8;
_temp2967;})); goto _LL2891; _LL2901: Cyc_Toc_structdecl_to_c( _temp2932); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2968=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2968->hd=( void*) d; _temp2968->tl= Cyc_Toc_result_decls;
_temp2968;}); goto _LL2891; _LL2903: Cyc_Toc_uniondecl_to_c( _temp2934); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2969=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2969->hd=( void*) d; _temp2969->tl= Cyc_Toc_result_decls;
_temp2969;}); goto _LL2891; _LL2905: if( _temp2936->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2936);} else{ Cyc_Toc_tuniondecl_to_c( _temp2936);} goto _LL2891; _LL2907:
Cyc_Toc_enumdecl_to_c( nv, _temp2938); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2970=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2970->hd=( void*) d; _temp2970->tl= Cyc_Toc_result_decls; _temp2970;});
goto _LL2891; _LL2909: _temp2940->name= _temp2940->name; _temp2940->tvs= 0;(
void*)( _temp2940->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2940->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2971=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2971->hd=( void*) d; _temp2971->tl=
Cyc_Toc_result_decls; _temp2971;}); goto _LL2891; _LL2911: _temp2946= _temp2942;
goto _LL2913; _LL2913: _temp2950= _temp2946; goto _LL2915; _LL2915: nv= Cyc_Toc_decls_to_c(
nv, _temp2950, top); goto _LL2891; _LL2891:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}