#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef
unsigned char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef
unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef unsigned short
Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec it_value; }
; typedef int Cyc_daddr_t; typedef unsigned char* Cyc_caddr_t; typedef
unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
unsigned int Cyc_vm_size_t; typedef unsigned char Cyc_int8_t; typedef
unsigned char Cyc_u_int8_t; typedef short Cyc_int16_t; typedef unsigned short
Cyc_u_int16_t; typedef int Cyc_int32_t; typedef unsigned int Cyc_u_int32_t;
typedef long long Cyc_int64_t; typedef unsigned long long Cyc_u_int64_t; typedef
int Cyc_register_t; typedef short Cyc_dev_t; typedef int Cyc_off_t; typedef
unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int Cyc_pid_t;
typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned char* Cyc_addr_t;
typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask;
struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef unsigned char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern struct _RegionHandle*
Cyc_Core_heap_region; extern unsigned char* string_to_Cstring( struct
_tagged_string); extern unsigned char* underlying_Cstring( struct _tagged_string);
extern struct _tagged_string Cstring_to_string( unsigned char*); extern int
system( unsigned char*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int
Cyc_Stdio_fpos_t; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; struct
_tagged_ptr0{ void** curr; void** base; void** last_plus_one; } ; extern struct
Cyc_List_List* Cyc_List_list( struct _tagged_ptr0); extern struct Cyc_List_List*
Cyc_List_rlist( struct _RegionHandle*, struct _tagged_ptr0); extern int Cyc_List_length(
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c(
struct _RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter_c(
void(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_rrevappend( struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_rrev( struct _RegionHandle*, struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rflatten(
struct _RegionHandle*, struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[
8u]; extern int Cyc_List_forall( int(* pred)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rfrom_array( struct _RegionHandle* r2,
struct _tagged_ptr0 arr); extern int Cyc_String_zstrptrcmp( struct
_tagged_string*, struct _tagged_string*); extern struct _tagged_string Cyc_String_strconcat(
struct _tagged_string, struct _tagged_string); extern struct _tagged_string Cyc_String_implode(
struct Cyc_List_List* c); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern
int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_gdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict* d, void* key); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
extern struct _tagged_string Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern unsigned char Cyc_Position_Nocontext[ 14u]; typedef
struct _tagged_string* Cyc_Absyn_field_name_t; typedef struct _tagged_string*
Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef
void* Cyc_Absyn_nmspace_t; struct _tuple0{ void* f1; struct _tagged_string* f2;
} ; typedef struct _tuple0* Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
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
unsigned char f2; } ; static const int Cyc_Absyn_Short_c= 1; struct Cyc_Absyn_Short_c_struct{
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
int tag; unsigned char f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
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
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp(
struct _tuple0*, struct _tuple0*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
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
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern unsigned char Cyc_Tcutil_TypeErr[
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
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={ 0, 0, 0}; unsigned char Cyc_Toc_Toc_Unimplemented[
22u]="\000\000\000\000Toc_Unimplemented"; unsigned char Cyc_Toc_Toc_Impossible[
19u]="\000\000\000\000Toc_Impossible"; static void* Cyc_Toc_unimp( struct
_tagged_string s){({ struct _tagged_string _temp1= s; fprintf( sfile_to_file(
Cyc_Stdio_stderr),"%.*s\n", _temp1.last_plus_one - _temp1.curr, _temp1.curr);});
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw((
void*) Cyc_Toc_Toc_Unimplemented);} static void* Cyc_Toc_toc_impos( struct
_tagged_string s){({ struct _tagged_string _temp2= s; fprintf( sfile_to_file(
Cyc_Stdio_stderr),"%.*s\n", _temp2.last_plus_one - _temp2.curr, _temp2.curr);});
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw((
void*) Cyc_Toc_Toc_Impossible);} unsigned char Cyc_Toc_Match_error[ 16u]="\000\000\000\000Match_error";
static unsigned char _temp5[ 5u]="curr"; static struct _tagged_string Cyc_Toc_curr_string={
_temp5, _temp5, _temp5 + 5u}; static struct _tagged_string* Cyc_Toc_curr_sp=&
Cyc_Toc_curr_string; static unsigned char _temp8[ 14u]="last_plus_one"; static
struct _tagged_string Cyc_Toc_last_plus_one_string={ _temp8, _temp8, _temp8 + 14u};
static struct _tagged_string* Cyc_Toc_last_plus_one_sp=& Cyc_Toc_last_plus_one_string;
static unsigned char _temp11[ 5u]="base"; static struct _tagged_string Cyc_Toc_base_string={
_temp11, _temp11, _temp11 + 5u}; static struct _tagged_string* Cyc_Toc_base_sp=&
Cyc_Toc_base_string; static unsigned char _temp14[ 15u]="_tagged_string"; static
struct _tagged_string Cyc_Toc__tagged_string_string={ _temp14, _temp14, _temp14
+ 15u}; static struct _tagged_string* Cyc_Toc__tagged_string_sp=& Cyc_Toc__tagged_string_string;
static unsigned char _temp17[ 8u]="xtunion"; static struct _tagged_string Cyc_Toc_xtunion_string={
_temp17, _temp17, _temp17 + 8u}; static struct _tagged_string* Cyc_Toc_xtunion_sp=&
Cyc_Toc_xtunion_string; static unsigned char _temp20[ 4u]="tag"; static struct
_tagged_string Cyc_Toc_tag_string={ _temp20, _temp20, _temp20 + 4u}; static
struct _tagged_string* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string; static unsigned char
_temp23[ 14u]="_handler_cons"; static struct _tagged_string Cyc_Toc__handler_cons_string={
_temp23, _temp23, _temp23 + 14u}; static struct _tagged_string* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static unsigned char _temp26[ 13u]="_push_region";
static struct _tagged_string Cyc_Toc__push_region_string={ _temp26, _temp26,
_temp26 + 13u}; static struct _tagged_string* Cyc_Toc__push_region_sp=& Cyc_Toc__push_region_string;
static unsigned char _temp29[ 12u]="_pop_region"; static struct _tagged_string
Cyc_Toc__pop_region_string={ _temp29, _temp29, _temp29 + 12u}; static struct
_tagged_string* Cyc_Toc__pop_region_sp=& Cyc_Toc__pop_region_string; static
unsigned char _temp32[ 8u]="handler"; static struct _tagged_string Cyc_Toc_handler_string={
_temp32, _temp32, _temp32 + 8u}; static struct _tagged_string* Cyc_Toc_handler_sp=&
Cyc_Toc_handler_string; static unsigned char _temp35[ 16u]="Match_Exception";
static struct _tagged_string Cyc_Toc_Match_Exception_string={ _temp35, _temp35,
_temp35 + 16u}; static struct _tagged_string* Cyc_Toc_Match_Exception_sp=& Cyc_Toc_Match_Exception_string;
static unsigned char _temp38[ 15u]="Cyc_new_string"; static struct
_tagged_string Cyc_Toc_Cyc_new_string_string={ _temp38, _temp38, _temp38 + 15u};
static struct _tagged_string* Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string;
static unsigned char _temp41[ 16u]="Cyc_rnew_string"; static struct
_tagged_string Cyc_Toc_Cyc_rnew_string_string={ _temp41, _temp41, _temp41 + 16u};
static struct _tagged_string* Cyc_Toc_Cyc_rnew_string_sp=& Cyc_Toc_Cyc_rnew_string_string;
static unsigned char _temp44[ 14u]="_RegionHandle"; static struct _tagged_string
Cyc_Toc__RegionHandle_string={ _temp44, _temp44, _temp44 + 14u}; static struct
_tagged_string* Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string; static
unsigned char _temp47[ 12u]="_new_region"; static struct _tagged_string Cyc_Toc__new_region_string={
_temp47, _temp47, _temp47 + 12u}; static struct _tagged_string* Cyc_Toc__new_region_sp=&
Cyc_Toc__new_region_string; static unsigned char _temp50[ 7u]="_throw"; static
struct _tagged_string Cyc_Toc__throw_string={ _temp50, _temp50, _temp50 + 7u};
static struct _tuple0 Cyc_Toc__throw_pr={( void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__throw_string};
static struct _tuple0* Cyc_Toc__throw_qv=& Cyc_Toc__throw_pr; static
unsigned char _temp53[ 7u]="setjmp"; static struct _tagged_string Cyc_Toc_setjmp_string={
_temp53, _temp53, _temp53 + 7u}; static struct _tuple0 Cyc_Toc_setjmp_pr={( void*)&
Cyc_Toc_abs_ns_value,& Cyc_Toc_setjmp_string}; static struct _tuple0* Cyc_Toc_setjmp_qv=&
Cyc_Toc_setjmp_pr; static unsigned char _temp56[ 14u]="_push_handler"; static
struct _tagged_string Cyc_Toc__push_handler_string={ _temp56, _temp56, _temp56 +
14u}; static struct _tuple0 Cyc_Toc__push_handler_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__push_handler_string}; static struct _tuple0* Cyc_Toc__push_handler_qv=&
Cyc_Toc__push_handler_pr; static unsigned char _temp59[ 13u]="_pop_handler";
static struct _tagged_string Cyc_Toc__pop_handler_string={ _temp59, _temp59,
_temp59 + 13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__pop_handler_string}; static struct _tuple0* Cyc_Toc__pop_handler_qv=&
Cyc_Toc__pop_handler_pr; static unsigned char _temp62[ 12u]="_exn_thrown";
static struct _tagged_string Cyc_Toc__exn_thrown_string={ _temp62, _temp62,
_temp62 + 12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__exn_thrown_string}; static struct _tuple0* Cyc_Toc__exn_thrown_qv=& Cyc_Toc__exn_thrown_pr;
static unsigned char _temp65[ 14u]="_npop_handler"; static struct _tagged_string
Cyc_Toc__npop_handler_string={ _temp65, _temp65, _temp65 + 14u}; static struct
_tuple0 Cyc_Toc__npop_handler_pr={( void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__npop_handler_string};
static struct _tuple0* Cyc_Toc__npop_handler_qv=& Cyc_Toc__npop_handler_pr;
static unsigned char _temp68[ 12u]="_check_null"; static struct _tagged_string
Cyc_Toc__check_null_string={ _temp68, _temp68, _temp68 + 12u}; static struct
_tuple0 Cyc_Toc__check_null_pr={( void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__check_null_string};
static struct _tuple0* Cyc_Toc__check_null_qv=& Cyc_Toc__check_null_pr; static
unsigned char _temp71[ 25u]="_check_unknown_subscript"; static struct
_tagged_string Cyc_Toc__check_unknown_subscript_string={ _temp71, _temp71,
_temp71 + 25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__check_unknown_subscript_string}; static
struct _tuple0* Cyc_Toc__check_unknown_subscript_qv=& Cyc_Toc__check_unknown_subscript_pr;
static unsigned char _temp74[ 28u]="_check_known_subscript_null"; static struct
_tagged_string Cyc_Toc__check_known_subscript_null_string={ _temp74, _temp74,
_temp74 + 28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__check_known_subscript_null_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_null_qv=& Cyc_Toc__check_known_subscript_null_pr;
static unsigned char _temp77[ 31u]="_check_known_subscript_notnull"; static
struct _tagged_string Cyc_Toc__check_known_subscript_notnull_string={ _temp77,
_temp77, _temp77 + 31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__check_known_subscript_notnull_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_notnull_qv=& Cyc_Toc__check_known_subscript_notnull_pr;
static unsigned char _temp80[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_GC_malloc_string={
_temp80, _temp80, _temp80 + 10u}; static struct _tuple0 Cyc_Toc_GC_malloc_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc_GC_malloc_string}; static struct _tuple0*
Cyc_Toc_GC_malloc_qv=& Cyc_Toc_GC_malloc_pr; static unsigned char _temp83[ 17u]="GC_malloc_atomic";
static struct _tagged_string Cyc_Toc_GC_malloc_atomic_string={ _temp83, _temp83,
_temp83 + 17u}; static struct _tuple0 Cyc_Toc_GC_malloc_atomic_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc_GC_malloc_atomic_string}; static struct _tuple0* Cyc_Toc_GC_malloc_atomic_qv=&
Cyc_Toc_GC_malloc_atomic_pr; static unsigned char _temp86[ 15u]="_region_malloc";
static struct _tagged_string Cyc_Toc__region_malloc_string={ _temp86, _temp86,
_temp86 + 15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__region_malloc_string}; static struct _tuple0* Cyc_Toc__region_malloc_qv=&
Cyc_Toc__region_malloc_pr; struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; static int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp87=(
void*) e->r; void* _temp113; unsigned char _temp115; void* _temp117; void*
_temp119; short _temp121; void* _temp123; void* _temp125; int _temp127; void*
_temp129; void* _temp131; long long _temp133; void* _temp135; void* _temp137;
struct Cyc_Absyn_Exp* _temp139; void* _temp141; struct Cyc_List_List* _temp143;
struct Cyc_List_List* _temp145; struct Cyc_Absyn_Structdecl* _temp147; struct
Cyc_List_List* _temp149; struct Cyc_Core_Opt* _temp151; struct _tuple0* _temp153;
struct Cyc_List_List* _temp155; struct _tuple1* _temp157; struct Cyc_List_List*
_temp159; struct Cyc_Core_Opt* _temp161; _LL89: if(*(( int*) _temp87) == Cyc_Absyn_Const_e){
_LL114: _temp113=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp87)->f1; if((
unsigned int) _temp113 > 1u?*(( int*) _temp113) == Cyc_Absyn_Char_c: 0){ _LL118:
_temp117=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp113)->f1; goto _LL116;
_LL116: _temp115=(( struct Cyc_Absyn_Char_c_struct*) _temp113)->f2; goto _LL90;}
else{ goto _LL91;}} else{ goto _LL91;} _LL91: if(*(( int*) _temp87) == Cyc_Absyn_Const_e){
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
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp189="is_nullable";
struct _tagged_string _temp190; _temp190.curr= _temp189; _temp190.base= _temp189;
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
unsigned char* _temp477="unresolved TunionType"; struct _tagged_string _temp478;
_temp478.curr= _temp477; _temp478.base= _temp477; _temp478.last_plus_one=
_temp477 + 22; _temp478;}))); _LL327: if( _temp399->typs == 0){ if( _temp401->is_xtunion){
return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);}
else{ return Cyc_Absyn_uint_t;}} else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp399->name,({ unsigned char* _temp479="_struct"; struct _tagged_string
_temp480; _temp480.curr= _temp479; _temp480.base= _temp479; _temp480.last_plus_one=
_temp479 + 8; _temp480;})));} _LL329:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp481="unresolved TunionFieldType";
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
struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp523="anonymous union";
struct _tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 16; _temp524;}));} return Cyc_Absyn_unionq_typ((
struct _tuple0*) _check_null( _temp453)); _LL351: if( _temp459 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp525="anonymous struct";
struct _tagged_string _temp526; _temp526.curr= _temp525; _temp526.base= _temp525;
_temp526.last_plus_one= _temp525 + 17; _temp526;}));} return Cyc_Absyn_strctq((
struct _tuple0*) _check_null( _temp459)); _LL353: return t; _LL355: if( _temp465
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp527="Toc::typ_to_c: unresolved TypedefType"; struct
_tagged_string _temp528; _temp528.curr= _temp527; _temp528.base= _temp527;
_temp528.last_plus_one= _temp527 + 38; _temp528;}));} return( void*)({ struct
Cyc_Absyn_TypedefType_struct* _temp529=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp529[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp530; _temp530.tag= Cyc_Absyn_TypedefType;
_temp530.f1= _temp469; _temp530.f2= 0; _temp530.f3=({ struct Cyc_Core_Opt*
_temp531=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp531->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp465))->v); _temp531;}); _temp530;}); _temp529;}); _LL357:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL359: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp532="Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp533; _temp533.curr= _temp532; _temp533.base= _temp532;
_temp533.last_plus_one= _temp532 + 55; _temp533;})); _LL361: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp534="Toc::typ_to_c: type translation passed the access eff";
struct _tagged_string _temp535; _temp535.curr= _temp534; _temp535.base= _temp534;
_temp535.last_plus_one= _temp534 + 54; _temp535;})); _LL363: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp536="Toc::typ_to_c: type translation passed the join eff";
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
Cyc_Toc_toc_impos)(({ unsigned char* _temp688="is_poly_field: struct missing fields";
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
unsigned char _temp735[ 3u]="f0"; static struct _tagged_string Cyc_Toc_f0={
_temp735, _temp735, _temp735 + 3u}; static struct _tagged_string* Cyc_Toc_field_names_v[
1u]={& Cyc_Toc_f0}; struct _tagged_ptr2{ struct _tagged_string** curr; struct
_tagged_string** base; struct _tagged_string** last_plus_one; } ; static struct
_tagged_ptr2 Cyc_Toc_field_names={( struct _tagged_string**) Cyc_Toc_field_names_v,(
struct _tagged_string**) Cyc_Toc_field_names_v,( struct _tagged_string**) Cyc_Toc_field_names_v
+ 1u}; static struct _tagged_string* Cyc_Toc_fieldname( int i){ return({ struct
_tagged_string* _temp738=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp738[ 0]= xprintf("f%d", i); _temp738;});{ unsigned int
fsz=({ struct _tagged_ptr2 _temp744= Cyc_Toc_field_names;( unsigned int)(
_temp744.last_plus_one - _temp744.curr);}); if( i >= fsz){ Cyc_Toc_field_names=({
unsigned int _temp739=( unsigned int)( i + 1); struct _tagged_string** _temp740=(
struct _tagged_string**) GC_malloc( sizeof( struct _tagged_string*) * _temp739);
struct _tagged_ptr2 _temp743={ _temp740, _temp740, _temp740 + _temp739};{
unsigned int _temp741= _temp739; unsigned int j; for( j= 0; j < _temp741; j ++){
_temp740[ j]= j < fsz?*(( struct _tagged_string**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Toc_field_names,
sizeof( struct _tagged_string*),( int) j):({ struct _tagged_string* _temp742=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp742[ 0]=
xprintf("f%d",( int) j); _temp742;});}}; _temp743;});} return*(( struct
_tagged_string**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Toc_field_names, sizeof( struct _tagged_string*),
i);}} static const int Cyc_Toc_NullCheck= 0; struct Cyc_Toc_NullCheck_struct{
int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr= 1; struct Cyc_Toc_UntagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; int f3; } ; static const int Cyc_Toc_TagPtr=
2; struct Cyc_Toc_TagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; }
; static const int Cyc_Toc_NoConv= 0; typedef void* Cyc_Toc_conv; struct _tuple7{
void* f1; void* f2; } ; static void* Cyc_Toc_conversion( void* t1, void* t2){ t1=
Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp746=({
struct _tuple7 _temp745; _temp745.f1= t1; _temp745.f2= t2; _temp745;}); void*
_temp752; struct Cyc_Absyn_PtrInfo _temp754; void* _temp756; struct Cyc_Absyn_PtrInfo
_temp758; _LL748: _LL757: _temp756= _temp746.f1; if(( unsigned int) _temp756 > 4u?*((
int*) _temp756) == Cyc_Absyn_PointerType: 0){ _LL759: _temp758=(( struct Cyc_Absyn_PointerType_struct*)
_temp756)->f1; goto _LL753;} else{ goto _LL750;} _LL753: _temp752= _temp746.f2;
if(( unsigned int) _temp752 > 4u?*(( int*) _temp752) == Cyc_Absyn_PointerType: 0){
_LL755: _temp754=(( struct Cyc_Absyn_PointerType_struct*) _temp752)->f1; goto
_LL749;} else{ goto _LL750;} _LL750: goto _LL751; _LL749: { int _temp760=(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp758.nullable); goto
_LL761; _LL761: { int _temp762=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp754.nullable); goto _LL763; _LL763: { void* _temp764=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp758.bounds); goto _LL765;
_LL765: { void* _temp766=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp754.bounds); goto _LL767; _LL767: { struct _tuple7 _temp769=({ struct
_tuple7 _temp768; _temp768.f1= _temp764; _temp768.f2= _temp766; _temp768;});
void* _temp779; struct Cyc_Absyn_Exp* _temp781; void* _temp783; struct Cyc_Absyn_Exp*
_temp785; void* _temp787; void* _temp789; void* _temp791; void* _temp793; struct
Cyc_Absyn_Exp* _temp795; void* _temp797; struct Cyc_Absyn_Exp* _temp799; void*
_temp801; _LL771: _LL784: _temp783= _temp769.f1; if(( unsigned int) _temp783 > 1u?*((
int*) _temp783) == Cyc_Absyn_Upper_b: 0){ _LL786: _temp785=(( struct Cyc_Absyn_Upper_b_struct*)
_temp783)->f1; goto _LL780;} else{ goto _LL773;} _LL780: _temp779= _temp769.f2;
if(( unsigned int) _temp779 > 1u?*(( int*) _temp779) == Cyc_Absyn_Upper_b: 0){
_LL782: _temp781=(( struct Cyc_Absyn_Upper_b_struct*) _temp779)->f1; goto _LL772;}
else{ goto _LL773;} _LL773: _LL790: _temp789= _temp769.f1; if( _temp789 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL788;} else{ goto _LL775;} _LL788: _temp787=
_temp769.f2; if( _temp787 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL774;} else{
goto _LL775;} _LL775: _LL794: _temp793= _temp769.f1; if(( unsigned int) _temp793
> 1u?*(( int*) _temp793) == Cyc_Absyn_Upper_b: 0){ _LL796: _temp795=(( struct
Cyc_Absyn_Upper_b_struct*) _temp793)->f1; goto _LL792;} else{ goto _LL777;}
_LL792: _temp791= _temp769.f2; if( _temp791 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL776;} else{ goto _LL777;} _LL777: _LL802: _temp801= _temp769.f1; if(
_temp801 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL798;} else{ goto _LL770;}
_LL798: _temp797= _temp769.f2; if(( unsigned int) _temp797 > 1u?*(( int*)
_temp797) == Cyc_Absyn_Upper_b: 0){ _LL800: _temp799=(( struct Cyc_Absyn_Upper_b_struct*)
_temp797)->f1; goto _LL778;} else{ goto _LL770;} _LL772: if( _temp760? !
_temp762: 0){ return( void*)({ struct Cyc_Toc_NullCheck_struct* _temp803=(
struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp803[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp804; _temp804.tag= Cyc_Toc_NullCheck;
_temp804.f1=( void*) t2; _temp804;}); _temp803;});} return( void*) Cyc_Toc_NoConv;
_LL774: return( void*) Cyc_Toc_NoConv; _LL776: return( void*)({ struct Cyc_Toc_TagPtr_struct*
_temp805=( struct Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp805[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp806; _temp806.tag= Cyc_Toc_TagPtr;
_temp806.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp806.f2= _temp795; _temp806;});
_temp805;}); _LL778: return( void*)({ struct Cyc_Toc_UntagPtr_struct* _temp807=(
struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp807[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp808; _temp808.tag= Cyc_Toc_UntagPtr;
_temp808.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp808.f2= _temp799; _temp808.f3=
_temp762; _temp808;}); _temp807;}); _LL770:;}}}}} _LL751: return( void*) Cyc_Toc_NoConv;
_LL747:;}} static unsigned char _temp811[ 14u]="sfile_to_file"; static struct
_tagged_string Cyc_Toc__sfile_to_file_string={ _temp811, _temp811, _temp811 + 14u};
static struct _tuple0 Cyc_Toc__sfile_to_file_var={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__sfile_to_file_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_fdconv(
struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__sfile_to_file_var,
0),({ struct Cyc_List_List* _temp812=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp812->hd=( void*) e; _temp812->tl= 0; _temp812;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__throw_qv, 0),({ struct
Cyc_List_List* _temp813=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp813->hd=( void*) e; _temp813->tl= 0; _temp813;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_null_check_conv( int toplevel, void* t, struct Cyc_Absyn_Exp* e){ void*
new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ unsigned char* _temp814="can't do null-check conversion at top-level";
struct _tagged_string _temp815; _temp815.curr= _temp814; _temp815.base= _temp814;
_temp815.last_plus_one= _temp814 + 44; _temp815;}));} if( Cyc_Toc_use_runtimecheck_funs){
return Cyc_Absyn_cast_exp( new_typ, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp816=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp816->hd=( void*) e; _temp816->tl= 0; _temp816;}), 0),
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
sz); void* strt_typ; void* elt_typ;{ void* _temp817= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp823; struct Cyc_Absyn_Conref* _temp825; struct Cyc_Absyn_Tqual
_temp827; struct Cyc_Absyn_Conref* _temp829; void* _temp831; void* _temp833;
_LL819: if(( unsigned int) _temp817 > 4u?*(( int*) _temp817) == Cyc_Absyn_PointerType:
0){ _LL824: _temp823=(( struct Cyc_Absyn_PointerType_struct*) _temp817)->f1;
_LL834: _temp833=( void*) _temp823.elt_typ; goto _LL832; _LL832: _temp831=( void*)
_temp823.rgn_typ; goto _LL830; _LL830: _temp829= _temp823.nullable; goto _LL828;
_LL828: _temp827= _temp823.tq; goto _LL826; _LL826: _temp825= _temp823.bounds;
goto _LL820;} else{ goto _LL821;} _LL821: goto _LL822; _LL820: elt_typ= _temp833;
strt_typ= Cyc_Toc_add_tagged_ptr_type( _temp833, Cyc_Toc_mt_tq); goto _LL818;
_LL822: return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp835="tag_ptr_conv: not a pointer"; struct _tagged_string
_temp836; _temp836.curr= _temp835; _temp836.base= _temp835; _temp836.last_plus_one=
_temp835 + 28; _temp836;})); _LL818:;} if( toplevel){ int is_string= 0;{ void*
_temp837=( void*) e->r; void* _temp843; struct _tagged_string _temp845; _LL839:
if(*(( int*) _temp837) == Cyc_Absyn_Const_e){ _LL844: _temp843=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp837)->f1; if(( unsigned int) _temp843 > 1u?*((
int*) _temp843) == Cyc_Absyn_String_c: 0){ _LL846: _temp845=(( struct Cyc_Absyn_String_c_struct*)
_temp843)->f1; goto _LL840;} else{ goto _LL841;}} else{ goto _LL841;} _LL841:
goto _LL842; _LL840: is_string= 1; goto _LL838; _LL842: goto _LL838; _LL838:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp850=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp850[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp851; _temp851.tag= Cyc_Absyn_ArrayType; _temp851.f1=(
void*) elt_typ; _temp851.f2= Cyc_Toc_mt_tq; _temp851.f3=( struct Cyc_Absyn_Exp*)
sz; _temp851;}); _temp850;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp847=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp847->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp848=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp848[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp849; _temp849.tag= Cyc_Absyn_Var_d;
_temp849.f1= vd; _temp849;}); _temp848;}), 0); _temp847->tl= Cyc_Toc_result_decls;
_temp847;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List*(* _temp852)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)(
struct _tagged_ptr3)) Cyc_List_list; struct _tuple4* _temp855=({ struct _tuple4*
_temp860=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp860->f1= 0;
_temp860->f2= xexp; _temp860;}); struct _tuple4* _temp856=({ struct _tuple4*
_temp859=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp859->f1= 0;
_temp859->f2= xexp; _temp859;}); struct _tuple4* _temp857=({ struct _tuple4*
_temp858=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp858->f1= 0;
_temp858->f2= xplussz; _temp858;}); struct _tuple4* _temp853[ 3u]={ _temp855,
_temp856, _temp857}; struct _tagged_ptr3 _temp854={ _temp853, _temp853, _temp853
+ 3u}; _temp852( _temp854);}), 0); return urm_exp;}}} else{ struct Cyc_Absyn_Stmt*
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
e){ if( toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp861="can't coerce t? to t* or t@ at the top-level"; struct
_tagged_string _temp862; _temp862.curr= _temp861; _temp862.base= _temp861;
_temp862.last_plus_one= _temp861 + 45; _temp862;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
int szvalue=( int) Cyc_Evexp_eval_const_uint_exp( sz); struct Cyc_Absyn_Exp*
xcurr= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0);
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
_temp863=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct Cyc_Toc_Env));
_temp863->break_lab= 0; _temp863->continue_lab= 0; _temp863->fallthru_info= 0;
_temp863->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp863->toplevel= 1;
_temp863;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp864=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp864->break_lab= e->break_lab; _temp864->continue_lab=
e->continue_lab; _temp864->fallthru_info= e->fallthru_info; _temp864->varmap= e->varmap;
_temp864->toplevel= e->toplevel; _temp864;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp865=(* x).f1; struct
Cyc_List_List* _temp871; _LL867: if(( unsigned int) _temp865 > 1u?*(( int*)
_temp865) == Cyc_Absyn_Rel_n: 0){ _LL872: _temp871=(( struct Cyc_Absyn_Rel_n_struct*)
_temp865)->f1; goto _LL868;} else{ goto _LL869;} _LL869: goto _LL870; _LL868:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp873=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp873[ 0]=({ struct
Cyc_Core_Impossible_struct _temp874; _temp874.tag= Cyc_Core_Impossible; _temp874.f1=({
struct _tagged_string _temp875= Cyc_Absynpp_qvar2string( x); xprintf("Toc::add_varmap on Rel_n: %.*s\n",
_temp875.last_plus_one - _temp875.curr, _temp875.curr);}); _temp874;}); _temp873;}));
_LL870: goto _LL866; _LL866:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap, x, y); return
ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple8{ struct _tagged_string* f1; struct Cyc_List_List*
f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l, struct _tagged_string*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp876= 0; goto _LL877; _LL877: for( 0;
fallthru_binders != 0; fallthru_binders=(( struct Cyc_List_List*) _check_null(
fallthru_binders))->tl){ _temp876=({ struct Cyc_List_List* _temp878=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp878->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp878->tl=
_temp876; _temp878;});} _temp876=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp876);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp879=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp879->v=( void*) break_l; _temp879;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp880=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp880->v=( void*)({ struct _tuple8*
_temp881=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp881->f1=
fallthru_l; _temp881->f2= _temp876; _temp881->f3= next_case_env->varmap;
_temp881;}); _temp880;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp882=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp882->v=( void*)
break_l; _temp882;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_string*
next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp883=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp883->v=( void*)({ struct _tuple8* _temp884=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp884->f1= next_l; _temp884->f2= 0;
_temp884->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp884;}); _temp883;});
return ans;} struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct _tagged_ptr4{ struct Cyc_List_List** curr; struct Cyc_List_List** base;
struct Cyc_List_List** last_plus_one; } ; struct _tuple10{ struct _tuple0* f1;
void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp885="expecting a literal format string";
struct _tagged_string _temp886; _temp886.curr= _temp885; _temp886.base= _temp885;
_temp886.last_plus_one= _temp885 + 34; _temp886;}));}{ struct _tagged_string
fmt_str;{ void* _temp887=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd)->r; void* _temp893; struct _tagged_string _temp895;
_LL889: if(*(( int*) _temp887) == Cyc_Absyn_Const_e){ _LL894: _temp893=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp887)->f1; if(( unsigned int) _temp893 > 1u?*((
int*) _temp893) == Cyc_Absyn_String_c: 0){ _LL896: _temp895=(( struct Cyc_Absyn_String_c_struct*)
_temp893)->f1; goto _LL890;} else{ goto _LL891;}} else{ goto _LL891;} _LL891:
goto _LL892; _LL890: fmt_str= _temp895; goto _LL888; _LL892: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp897="expecting a literal format string";
struct _tagged_string _temp898; _temp898.curr= _temp897; _temp898.base= _temp897;
_temp898.last_plus_one= _temp897 + 34; _temp898;})); _LL888:;}{ int len=( int)({
struct _tagged_string _temp994= fmt_str;( unsigned int)( _temp994.last_plus_one
- _temp994.curr);}); int i= 0; for( i= 0; i < len; i ++){ if(*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
fmt_str, sizeof( unsigned char), i) =='%'){ i ++; if( i < len?*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
fmt_str, sizeof( unsigned char), i) =='s': 0){ break;}}} if( i == len){ if( aopt
!= 0){ args=({ struct Cyc_List_List* _temp899=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp899->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp899->tl= args; _temp899;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp900= _new_region(); struct
_RegionHandle* r=& _temp900; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str, sizeof(
unsigned char), i); rev_fmt=({ struct Cyc_List_List* _temp901=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp901->hd=( void*)(( int)
c); _temp901->tl= rev_fmt; _temp901;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt*
_temp902= Cyc_Formatstr_parse_conversionspecification( fmt_str, i + 1); goto
_LL903; _LL903: if( _temp902 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp904="bad format string"; struct _tagged_string _temp905;
_temp905.curr= _temp904; _temp905.base= _temp904; _temp905.last_plus_one=
_temp904 + 18; _temp905;}));}{ struct _tuple9 _temp908; int _temp909;
unsigned char _temp911; struct Cyc_List_List* _temp913; struct Cyc_List_List*
_temp915; struct Cyc_List_List* _temp917; struct Cyc_List_List* _temp919; struct
_tuple9* _temp906=( struct _tuple9*)(( struct Cyc_Core_Opt*) _check_null(
_temp902))->v; _temp908=* _temp906; _LL920: _temp919= _temp908.f1; goto _LL918;
_LL918: _temp917= _temp908.f2; goto _LL916; _LL916: _temp915= _temp908.f3; goto
_LL914; _LL914: _temp913= _temp908.f4; goto _LL912; _LL912: _temp911= _temp908.f5;
goto _LL910; _LL910: _temp909= _temp908.f6; goto _LL907; _LL907: i= _temp909 - 1;
if( _temp911 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp921)( struct _RegionHandle*, struct
_tagged_ptr4)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_ptr4)) Cyc_List_rlist; struct _RegionHandle* _temp928= r; struct Cyc_List_List*
_temp924= _temp919; struct Cyc_List_List* _temp925= _temp917; struct Cyc_List_List*
_temp926= _temp915; struct Cyc_List_List* _temp927= _temp913; struct Cyc_List_List*
_temp922[ 4u]={ _temp924, _temp925, _temp926, _temp927}; struct _tagged_ptr4
_temp923={ _temp922, _temp922, _temp922 + 4u}; _temp921( _temp928, _temp923);})),
rev_fmt); rev_fmt=({ struct Cyc_List_List* _temp929=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp929->hd=( void*)(( int)
_temp911); _temp929->tl= rev_fmt; _temp929;});{ struct Cyc_List_List* _temp930=
_temp917; struct Cyc_List_List _temp937; struct Cyc_List_List* _temp938; int
_temp940; _LL932: if( _temp930 == 0){ goto _LL934;} else{ _temp937=* _temp930;
_LL941: _temp940=( int) _temp937.hd; goto _LL939; _LL939: _temp938= _temp937.tl;
if( _temp938 == 0){ goto _LL936;} else{ goto _LL934;}} _LL936: if( _temp940 =='*'){
goto _LL933;} else{ goto _LL934;} _LL934: goto _LL935; _LL933: { struct _tuple0*
_temp942= Cyc_Toc_temp_var(); goto _LL943; _LL943: rev_temps=({ struct Cyc_List_List*
_temp944=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp944->hd=( void*)({ struct _tuple10* _temp945=( struct _tuple10*)
_region_malloc( r, sizeof( struct _tuple10)); _temp945->f1= _temp942; _temp945->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp945->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp945;});
_temp944->tl= rev_temps; _temp944;}); rev_result=({ struct Cyc_List_List*
_temp946=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp946->hd=( void*) Cyc_Absyn_var_exp( _temp942, 0); _temp946->tl= rev_result;
_temp946;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL931;} _LL935: goto
_LL931; _LL931:;}{ struct Cyc_List_List* _temp947= _temp915; struct Cyc_List_List
_temp954; struct Cyc_List_List* _temp955; struct Cyc_List_List _temp957; struct
Cyc_List_List* _temp958; int _temp960; int _temp962; _LL949: if( _temp947 == 0){
goto _LL951;} else{ _temp954=* _temp947; _LL963: _temp962=( int) _temp954.hd;
goto _LL956; _LL956: _temp955= _temp954.tl; if( _temp955 == 0){ goto _LL951;}
else{ _temp957=* _temp955; _LL961: _temp960=( int) _temp957.hd; goto _LL959;
_LL959: _temp958= _temp957.tl; if( _temp958 == 0){ goto _LL953;} else{ goto
_LL951;}}} _LL953: if( _temp962 =='.'? _temp960 =='*': 0){ goto _LL950;} else{
goto _LL951;} _LL951: goto _LL952; _LL950: { struct _tuple0* _temp964= Cyc_Toc_temp_var();
goto _LL965; _LL965: rev_temps=({ struct Cyc_List_List* _temp966=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp966->hd=( void*)({
struct _tuple10* _temp967=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp967->f1= _temp964; _temp967->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp967->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp967;}); _temp966->tl= rev_temps; _temp966;});
rev_result=({ struct Cyc_List_List* _temp968=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp968->hd=( void*) Cyc_Absyn_var_exp(
_temp964, 0); _temp968->tl= rev_result; _temp968;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL948;} _LL952: goto _LL948; _LL948:;} if( _temp911 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp969= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp969.last_plus_one - _temp969.curr, _temp969.curr);}));}{ struct _tuple0*
_temp970= Cyc_Toc_temp_var(); goto _LL971; _LL971: rev_temps=({ struct Cyc_List_List*
_temp972=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp972->hd=( void*)({ struct _tuple10* _temp973=( struct _tuple10*)
_region_malloc( r, sizeof( struct _tuple10)); _temp973->f1= _temp970; _temp973->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp973->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp973;});
_temp972->tl= rev_temps; _temp972;}); rev_result=({ struct Cyc_List_List*
_temp974=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp974->hd=( void*) Cyc_Absyn_var_exp( _temp970, 0); _temp974->tl= rev_result;
_temp974;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl;}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp975=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp975->hd=( void*)(( int)'.'); _temp975->tl= rev_fmt; _temp975;}); rev_fmt=({
struct Cyc_List_List* _temp976=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp976->hd=( void*)(( int)'*'); _temp976->tl=
rev_fmt; _temp976;}); rev_fmt=({ struct Cyc_List_List* _temp977=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp977->hd=( void*)(( int)'s');
_temp977->tl= rev_fmt; _temp977;});{ struct _tuple0* _temp978= Cyc_Toc_temp_var();
goto _LL979; _LL979: rev_temps=({ struct Cyc_List_List* _temp980=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp980->hd=( void*)({
struct _tuple10* _temp981=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp981->f1= _temp978; _temp981->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp981->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp981;}); _temp980->tl= rev_temps; _temp980;});{
struct Cyc_Absyn_Exp* _temp982= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp978, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp978, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL983; _LL983: {
struct Cyc_Absyn_Exp* _temp984= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp978, 0), Cyc_Toc_curr_sp, 0); goto _LL985; _LL985: rev_result=({ struct Cyc_List_List*
_temp986=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp986->hd=( void*) _temp982; _temp986->tl= rev_result; _temp986;});
rev_result=({ struct Cyc_List_List* _temp987=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp987->hd=( void*)
_temp984; _temp987->tl= rev_result; _temp987;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}}{
struct Cyc_List_List* _temp989=({ struct Cyc_List_List* _temp988=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp988->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp988->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( rev_result); _temp988;}); goto _LL990; _LL990: if( aopt != 0){
_temp989=({ struct Cyc_List_List* _temp991=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp991->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp991->tl= _temp989; _temp991;});}{
struct Cyc_Absyn_Exp* _temp992= Cyc_Absyn_primop_exp( p, _temp989, 0); goto
_LL993; _LL993: s= Cyc_Absyn_exp_stmt( _temp992, 0); for( 0; rev_temps != 0;
rev_temps=(( struct Cyc_List_List*) _check_null( rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f1,(*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f2,(
struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f3, s, 0);}}}}; _pop_region(& _temp900);} return
Cyc_Absyn_stmt_exp( s, 0);}}}} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp995="Missing type in primop "; struct _tagged_string
_temp996; _temp996.curr= _temp995; _temp996.base= _temp995; _temp996.last_plus_one=
_temp995 + 24; _temp996;}));} return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp997="Missing type in primop "; struct _tagged_string
_temp998; _temp998.curr= _temp997; _temp998.base= _temp997; _temp998.last_plus_one=
_temp997 + 24; _temp998;}));} return( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp* e){
return({ struct _tuple3* _temp999=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp999->f1= Cyc_Toc_mt_tq; _temp999->f2= Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp999;});} static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e); static struct _tuple4* Cyc_Toc_add_designator(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e);
return({ struct _tuple4* _temp1000=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4) * 1); _temp1000[ 0]=({ struct _tuple4 _temp1001; _temp1001.f1= 0;
_temp1001.f2= e; _temp1001;}); _temp1000;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct(
struct Cyc_Toc_Env* nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt*
s, int pointer, struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp*
eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{
struct Cyc_Absyn_Exp* _temp1002= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); goto
_LL1003; _LL1003: if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp1002): Cyc_Toc_malloc_ptr( _temp1002),
0);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt);
Cyc_Toc_exp_to_c( nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp1002), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp1004=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); goto _LL1005; _LL1005: for( 0;
_temp1004 != 0; _temp1004=(( struct Cyc_List_List*) _check_null( _temp1004))->tl){
struct _tuple4 _temp1008; struct Cyc_Absyn_Exp* _temp1009; struct Cyc_List_List*
_temp1011; struct _tuple4* _temp1006=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1004))->hd; _temp1008=* _temp1006; _LL1012: _temp1011=
_temp1008.f1; goto _LL1010; _LL1010: _temp1009= _temp1008.f2; goto _LL1007;
_LL1007: { struct Cyc_Absyn_Exp* e_index; if( _temp1011 == 0){ e_index= Cyc_Absyn_signed_int_exp(
count --, 0);} else{ if((( struct Cyc_List_List*) _check_null( _temp1011))->tl
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp1013="multiple designators in array"; struct _tagged_string _temp1014;
_temp1014.curr= _temp1013; _temp1014.base= _temp1013; _temp1014.last_plus_one=
_temp1013 + 30; _temp1014;}));}{ void* _temp1015=( void*)(( struct Cyc_List_List*)
_check_null( _temp1011))->hd; goto _LL1016; _LL1016: { void* _temp1017=
_temp1015; struct Cyc_Absyn_Exp* _temp1023; struct _tagged_string* _temp1025;
_LL1019: if(*(( int*) _temp1017) == Cyc_Absyn_ArrayElement){ _LL1024: _temp1023=((
struct Cyc_Absyn_ArrayElement_struct*) _temp1017)->f1; goto _LL1020;} else{ goto
_LL1021;} _LL1021: if(*(( int*) _temp1017) == Cyc_Absyn_FieldName){ _LL1026:
_temp1025=(( struct Cyc_Absyn_FieldName_struct*) _temp1017)->f1; goto _LL1022;}
else{ goto _LL1018;} _LL1020: Cyc_Toc_exp_to_c( nv, _temp1023); e_index=
_temp1023; goto _LL1018; _LL1022: e_index=(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp1027="field name designators in array";
struct _tagged_string _temp1028; _temp1028.curr= _temp1027; _temp1028.base=
_temp1027; _temp1028.last_plus_one= _temp1027 + 32; _temp1028;})); goto _LL1018;
_LL1018:;}}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index,
0); void* _temp1029=( void*) _temp1009->r; struct Cyc_List_List* _temp1039;
struct Cyc_Absyn_Exp* _temp1041; struct Cyc_Absyn_Exp* _temp1043; struct Cyc_Absyn_Vardecl*
_temp1045; struct Cyc_List_List* _temp1047; void* _temp1049; _LL1031: if(*(( int*)
_temp1029) == Cyc_Absyn_Array_e){ _LL1040: _temp1039=(( struct Cyc_Absyn_Array_e_struct*)
_temp1029)->f1; goto _LL1032;} else{ goto _LL1033;} _LL1033: if(*(( int*)
_temp1029) == Cyc_Absyn_Comprehension_e){ _LL1046: _temp1045=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1029)->f1; goto _LL1044; _LL1044: _temp1043=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1029)->f2; goto _LL1042; _LL1042: _temp1041=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1029)->f3; goto _LL1034;} else{ goto _LL1035;} _LL1035: if(*(( int*)
_temp1029) == Cyc_Absyn_AnonStruct_e){ _LL1050: _temp1049=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1029)->f1; goto _LL1048; _LL1048: _temp1047=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1029)->f2; goto _LL1036;} else{ goto _LL1037;} _LL1037: goto _LL1038;
_LL1032: s= Cyc_Toc_init_array( nv, lval, _temp1039, s); goto _LL1030; _LL1034:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1045, _temp1043, _temp1041, s, 0);
goto _LL1030; _LL1036: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1049,
_temp1047, s); goto _LL1030; _LL1038: Cyc_Toc_exp_to_c( nv, _temp1009); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp1009, 0),
s, 0); goto _LL1030; _LL1030:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* _temp1051= vd->name; goto _LL1052;
_LL1052: if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp1051, Cyc_Absyn_varb_exp( _temp1051,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1075=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1075[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1076; _temp1076.tag= Cyc_Absyn_Local_b; _temp1076.f1=
vd; _temp1076;}); _temp1075;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp1051, 0),
Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( _temp1051, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp*
ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( _temp1051, 0), 0); struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( _temp1051, 0), 0); struct
Cyc_Absyn_Stmt* body;{ void* _temp1053=( void*) e2->r; struct Cyc_List_List*
_temp1063; struct Cyc_Absyn_Exp* _temp1065; struct Cyc_Absyn_Exp* _temp1067;
struct Cyc_Absyn_Vardecl* _temp1069; struct Cyc_List_List* _temp1071; void*
_temp1073; _LL1055: if(*(( int*) _temp1053) == Cyc_Absyn_Array_e){ _LL1064:
_temp1063=(( struct Cyc_Absyn_Array_e_struct*) _temp1053)->f1; goto _LL1056;}
else{ goto _LL1057;} _LL1057: if(*(( int*) _temp1053) == Cyc_Absyn_Comprehension_e){
_LL1070: _temp1069=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1053)->f1;
goto _LL1068; _LL1068: _temp1067=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1053)->f2; goto _LL1066; _LL1066: _temp1065=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1053)->f3; goto _LL1058;} else{ goto _LL1059;} _LL1059: if(*(( int*)
_temp1053) == Cyc_Absyn_AnonStruct_e){ _LL1074: _temp1073=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1053)->f1; goto _LL1072; _LL1072: _temp1071=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1053)->f2; goto _LL1060;} else{ goto _LL1061;} _LL1061: goto _LL1062;
_LL1056: body= Cyc_Toc_init_array( nv2, lval, _temp1063, Cyc_Absyn_skip_stmt( 0));
goto _LL1054; _LL1058: body= Cyc_Toc_init_comprehension( nv2, lval, _temp1069,
_temp1067, _temp1065, Cyc_Absyn_skip_stmt( 0), 0); goto _LL1054; _LL1060: body=
Cyc_Toc_init_anon_struct( nv, lval, _temp1073, _temp1071, Cyc_Absyn_skip_stmt( 0));
goto _LL1054; _LL1062: Cyc_Toc_exp_to_c( nv2, e2); body= Cyc_Absyn_assign_stmt(
lval, e2, 0); goto _LL1054; _LL1054:;}{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt(
ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( _temp1051, Cyc_Absyn_uint_t,
0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s){{ struct Cyc_List_List* _temp1077=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto _LL1078; _LL1078: for( 0;
_temp1077 != 0; _temp1077=(( struct Cyc_List_List*) _check_null( _temp1077))->tl){
struct _tuple4 _temp1081; struct Cyc_Absyn_Exp* _temp1082; struct Cyc_List_List*
_temp1084; struct _tuple4* _temp1079=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1077))->hd; _temp1081=* _temp1079; _LL1085: _temp1084=
_temp1081.f1; goto _LL1083; _LL1083: _temp1082= _temp1081.f2; goto _LL1080;
_LL1080: if( _temp1084 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1086="empty designator list"; struct _tagged_string
_temp1087; _temp1087.curr= _temp1086; _temp1087.base= _temp1086; _temp1087.last_plus_one=
_temp1086 + 22; _temp1087;}));} if((( struct Cyc_List_List*) _check_null(
_temp1084))->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1088="too many designators in anonymous struct"; struct
_tagged_string _temp1089; _temp1089.curr= _temp1088; _temp1089.base= _temp1088;
_temp1089.last_plus_one= _temp1088 + 41; _temp1089;}));}{ void* _temp1090=( void*)((
struct Cyc_List_List*) _check_null( _temp1084))->hd; struct _tagged_string*
_temp1096; _LL1092: if(*(( int*) _temp1090) == Cyc_Absyn_FieldName){ _LL1097:
_temp1096=(( struct Cyc_Absyn_FieldName_struct*) _temp1090)->f1; goto _LL1093;}
else{ goto _LL1094;} _LL1094: goto _LL1095; _LL1093: { struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_structmember_exp( lhs, _temp1096, 0);{ void* _temp1098=( void*)
_temp1082->r; struct Cyc_List_List* _temp1108; struct Cyc_Absyn_Exp* _temp1110;
struct Cyc_Absyn_Exp* _temp1112; struct Cyc_Absyn_Vardecl* _temp1114; struct Cyc_List_List*
_temp1116; void* _temp1118; _LL1100: if(*(( int*) _temp1098) == Cyc_Absyn_Array_e){
_LL1109: _temp1108=(( struct Cyc_Absyn_Array_e_struct*) _temp1098)->f1; goto
_LL1101;} else{ goto _LL1102;} _LL1102: if(*(( int*) _temp1098) == Cyc_Absyn_Comprehension_e){
_LL1115: _temp1114=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1098)->f1;
goto _LL1113; _LL1113: _temp1112=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1098)->f2; goto _LL1111; _LL1111: _temp1110=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1098)->f3; goto _LL1103;} else{ goto _LL1104;} _LL1104: if(*(( int*)
_temp1098) == Cyc_Absyn_AnonStruct_e){ _LL1119: _temp1118=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1098)->f1; goto _LL1117; _LL1117: _temp1116=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1098)->f2; goto _LL1105;} else{ goto _LL1106;} _LL1106: goto _LL1107;
_LL1101: s= Cyc_Toc_init_array( nv, lval, _temp1108, s); goto _LL1099; _LL1103:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1114, _temp1112, _temp1110, s, 0);
goto _LL1099; _LL1105: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1118,
_temp1116, s); goto _LL1099; _LL1107: Cyc_Toc_exp_to_c( nv, _temp1082); if( Cyc_Toc_is_poly_field(
struct_type, _temp1096)){ _temp1082= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1082, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1082, 0), 0), s, 0); goto _LL1099; _LL1099:;} goto _LL1091;} _LL1095:(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1120="array designator in struct"; struct _tagged_string
_temp1121; _temp1121.curr= _temp1120; _temp1121.base= _temp1120; _temp1121.last_plus_one=
_temp1120 + 27; _temp1121;}))); _LL1091:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp1122= _new_region();
struct _RegionHandle* r=& _temp1122; _push_region( r);{ struct Cyc_List_List*
_temp1123=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(*
f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); goto _LL1124; _LL1124: { struct _tagged_string* _temp1125= Cyc_Toc_add_tuple_type(
_temp1123); goto _LL1126; _LL1126: { struct _tuple0* _temp1127= Cyc_Toc_temp_var();
goto _LL1128; _LL1128: { struct Cyc_Absyn_Exp* _temp1129= Cyc_Absyn_var_exp(
_temp1127, 0); goto _LL1130; _LL1130: { struct Cyc_Absyn_Stmt* _temp1131= Cyc_Absyn_exp_stmt(
_temp1129, 0); goto _LL1132; _LL1132: { struct Cyc_Absyn_Exp*(* _temp1133)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1134;
_LL1134: { int is_atomic= 1; struct Cyc_List_List* _temp1135=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es); goto
_LL1136; _LL1136:{ int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1135); for( 0; _temp1135 != 0;( _temp1135=(( struct Cyc_List_List*)
_check_null( _temp1135))->tl, -- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1135))->hd; struct Cyc_Absyn_Exp* lval=
_temp1133( _temp1129, Cyc_Toc_fieldname( i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v): 0;{ void* _temp1137=(
void*) e->r; struct Cyc_List_List* _temp1145; struct Cyc_Absyn_Exp* _temp1147;
struct Cyc_Absyn_Exp* _temp1149; struct Cyc_Absyn_Vardecl* _temp1151; _LL1139:
if(*(( int*) _temp1137) == Cyc_Absyn_Array_e){ _LL1146: _temp1145=(( struct Cyc_Absyn_Array_e_struct*)
_temp1137)->f1; goto _LL1140;} else{ goto _LL1141;} _LL1141: if(*(( int*)
_temp1137) == Cyc_Absyn_Comprehension_e){ _LL1152: _temp1151=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1137)->f1; goto _LL1150; _LL1150: _temp1149=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1137)->f2; goto _LL1148; _LL1148: _temp1147=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1137)->f3; goto _LL1142;} else{ goto _LL1143;} _LL1143: goto _LL1144;
_LL1140: _temp1131= Cyc_Toc_init_array( nv, lval, _temp1145, _temp1131); goto
_LL1138; _LL1142: _temp1131= Cyc_Toc_init_comprehension( nv, lval, _temp1151,
_temp1149, _temp1147, _temp1131, 0); goto _LL1138; _LL1144: Cyc_Toc_exp_to_c( nv,
e); _temp1131= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1133( _temp1129, Cyc_Toc_fieldname( i), 0), e, 0), 0), _temp1131, 0); goto
_LL1138; _LL1138:;}}}{ struct Cyc_Absyn_Exp* _temp1153= Cyc_Toc_make_struct( nv,
_temp1127, Cyc_Absyn_strct( _temp1125), _temp1131, pointer, rgnopt, is_atomic);
_npop_handler( 0u); return _temp1153;}}}}}}}}; _pop_region(& _temp1122);} static
struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env* nv, void*
struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp1154= Cyc_Toc_temp_var(); goto
_LL1155; _LL1155: { struct Cyc_Absyn_Exp* _temp1156= Cyc_Absyn_var_exp(
_temp1154, 0); goto _LL1157; _LL1157: { struct Cyc_Absyn_Stmt* _temp1158= Cyc_Absyn_exp_stmt(
_temp1156, 0); goto _LL1159; _LL1159: { struct Cyc_Absyn_Exp*(* _temp1160)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1161;
_LL1161: { int is_atomic= 1; struct _RegionHandle _temp1162= _new_region();
struct _RegionHandle* r=& _temp1162; _push_region( r);{ struct Cyc_List_List*
_temp1163=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, dles); goto _LL1164; _LL1164: for( 0; _temp1163 != 0;
_temp1163=(( struct Cyc_List_List*) _check_null( _temp1163))->tl){ struct
_tuple4 _temp1167; struct Cyc_Absyn_Exp* _temp1168; struct Cyc_List_List*
_temp1170; struct _tuple4* _temp1165=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1163))->hd; _temp1167=* _temp1165; _LL1171: _temp1170=
_temp1167.f1; goto _LL1169; _LL1169: _temp1168= _temp1167.f2; goto _LL1166;
_LL1166: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1168->topt))->v): 0; if( _temp1170 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1172="empty designator list";
struct _tagged_string _temp1173; _temp1173.curr= _temp1172; _temp1173.base=
_temp1172; _temp1173.last_plus_one= _temp1172 + 22; _temp1173;}));} if((( struct
Cyc_List_List*) _check_null( _temp1170))->tl != 0){ struct _tuple0* _temp1174=
Cyc_Toc_temp_var(); goto _LL1175; _LL1175: { struct Cyc_Absyn_Exp* _temp1176=
Cyc_Absyn_var_exp( _temp1174, 0); goto _LL1177; _LL1177: for( 0; _temp1170 != 0;
_temp1170=(( struct Cyc_List_List*) _check_null( _temp1170))->tl){ void*
_temp1178=( void*)(( struct Cyc_List_List*) _check_null( _temp1170))->hd; struct
_tagged_string* _temp1184; _LL1180: if(*(( int*) _temp1178) == Cyc_Absyn_FieldName){
_LL1185: _temp1184=(( struct Cyc_Absyn_FieldName_struct*) _temp1178)->f1; goto
_LL1181;} else{ goto _LL1182;} _LL1182: goto _LL1183; _LL1181: if( Cyc_Toc_is_poly_field(
struct_type, _temp1184)){ _temp1176= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1176, 0);} _temp1158= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1160( _temp1156, _temp1184, 0), _temp1176, 0), 0), _temp1158, 0); goto
_LL1179; _LL1183:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1186="array designator in struct"; struct _tagged_string
_temp1187; _temp1187.curr= _temp1186; _temp1187.base= _temp1186; _temp1187.last_plus_one=
_temp1186 + 27; _temp1187;}))); _LL1179:;} Cyc_Toc_exp_to_c( nv, _temp1168);
_temp1158= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1176, _temp1168, 0), 0), _temp1158, 0);}} else{ void* _temp1188=( void*)((
struct Cyc_List_List*) _check_null( _temp1170))->hd; struct _tagged_string*
_temp1194; _LL1190: if(*(( int*) _temp1188) == Cyc_Absyn_FieldName){ _LL1195:
_temp1194=(( struct Cyc_Absyn_FieldName_struct*) _temp1188)->f1; goto _LL1191;}
else{ goto _LL1192;} _LL1192: goto _LL1193; _LL1191: { struct Cyc_Absyn_Exp*
lval= _temp1160( _temp1156, _temp1194, 0);{ void* _temp1196=( void*) _temp1168->r;
struct Cyc_List_List* _temp1206; struct Cyc_Absyn_Exp* _temp1208; struct Cyc_Absyn_Exp*
_temp1210; struct Cyc_Absyn_Vardecl* _temp1212; struct Cyc_List_List* _temp1214;
void* _temp1216; _LL1198: if(*(( int*) _temp1196) == Cyc_Absyn_Array_e){ _LL1207:
_temp1206=(( struct Cyc_Absyn_Array_e_struct*) _temp1196)->f1; goto _LL1199;}
else{ goto _LL1200;} _LL1200: if(*(( int*) _temp1196) == Cyc_Absyn_Comprehension_e){
_LL1213: _temp1212=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1196)->f1;
goto _LL1211; _LL1211: _temp1210=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1196)->f2; goto _LL1209; _LL1209: _temp1208=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1196)->f3; goto _LL1201;} else{ goto _LL1202;} _LL1202: if(*(( int*)
_temp1196) == Cyc_Absyn_AnonStruct_e){ _LL1217: _temp1216=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1196)->f1; goto _LL1215; _LL1215: _temp1214=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1196)->f2; goto _LL1203;} else{ goto _LL1204;} _LL1204: goto _LL1205;
_LL1199: _temp1158= Cyc_Toc_init_array( nv, lval, _temp1206, _temp1158); goto
_LL1197; _LL1201: _temp1158= Cyc_Toc_init_comprehension( nv, lval, _temp1212,
_temp1210, _temp1208, _temp1158, 0); goto _LL1197; _LL1203: _temp1158= Cyc_Toc_init_anon_struct(
nv, lval, _temp1216, _temp1214, _temp1158); goto _LL1197; _LL1205: Cyc_Toc_exp_to_c(
nv, _temp1168); if( Cyc_Toc_is_poly_field( struct_type, _temp1194)){ _temp1168=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1168, 0);} _temp1158= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval, _temp1168, 0), 0), _temp1158, 0);
goto _LL1197; _LL1197:;} goto _LL1189;} _LL1193:( void) _throw((( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1218="array designator in struct";
struct _tagged_string _temp1219; _temp1219.curr= _temp1218; _temp1219.base=
_temp1218; _temp1219.last_plus_one= _temp1218 + 27; _temp1219;}))); _LL1189:;}}}{
struct Cyc_Absyn_Exp* _temp1220= Cyc_Toc_make_struct( nv, _temp1154, Cyc_Absyn_strctq(
tdn), _temp1158, pointer, rgnopt, is_atomic); _npop_handler( 0u); return
_temp1220;}; _pop_region(& _temp1162);}}}}} struct _tuple11{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp(
el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp1221=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp1221[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp1222; _temp1222.tag=
Cyc_Absyn_Increment_e; _temp1222.f1= e1; _temp1222.f2=( void*) incr; _temp1222;});
_temp1221;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1223=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp1233; struct Cyc_Absyn_Exp* _temp1235; void* _temp1237; struct
_tagged_string* _temp1239; struct Cyc_Absyn_Exp* _temp1241; _LL1225: if(*(( int*)
_temp1223) == Cyc_Absyn_StmtExp_e){ _LL1234: _temp1233=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1223)->f1; goto _LL1226;} else{ goto _LL1227;} _LL1227: if(*(( int*)
_temp1223) == Cyc_Absyn_Cast_e){ _LL1238: _temp1237=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1223)->f1; goto _LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1223)->f2; goto _LL1228;} else{ goto _LL1229;} _LL1229: if(*(( int*)
_temp1223) == Cyc_Absyn_StructMember_e){ _LL1242: _temp1241=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1223)->f1; goto _LL1240; _LL1240: _temp1239=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1223)->f2; goto _LL1230;} else{ goto _LL1231;} _LL1231: goto _LL1232;
_LL1226:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1233, fs, f, f_env); goto _LL1224; _LL1228:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1235, fs, f, f_env); goto
_LL1224; _LL1230:( void*)( e1->r=( void*)(( void*) _temp1241->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1243=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1243->hd=( void*) _temp1239; _temp1243->tl= fs;
_temp1243;}), f, f_env); goto _LL1224; _LL1232: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1244= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1244.last_plus_one - _temp1244.curr, _temp1244.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1224;}
_LL1224:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1245=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1255; struct Cyc_Absyn_Stmt* _temp1257; struct Cyc_Absyn_Decl* _temp1259;
struct Cyc_Absyn_Stmt* _temp1261; struct Cyc_Absyn_Stmt* _temp1263; _LL1247: if((
unsigned int) _temp1245 > 1u?*(( int*) _temp1245) == Cyc_Absyn_Exp_s: 0){
_LL1256: _temp1255=(( struct Cyc_Absyn_Exp_s_struct*) _temp1245)->f1; goto
_LL1248;} else{ goto _LL1249;} _LL1249: if(( unsigned int) _temp1245 > 1u?*((
int*) _temp1245) == Cyc_Absyn_Decl_s: 0){ _LL1260: _temp1259=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1245)->f1; goto _LL1258; _LL1258: _temp1257=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1245)->f2; goto _LL1250;} else{ goto _LL1251;} _LL1251: if(( unsigned int)
_temp1245 > 1u?*(( int*) _temp1245) == Cyc_Absyn_Seq_s: 0){ _LL1264: _temp1263=((
struct Cyc_Absyn_Seq_s_struct*) _temp1245)->f1; goto _LL1262; _LL1262: _temp1261=((
struct Cyc_Absyn_Seq_s_struct*) _temp1245)->f2; goto _LL1252;} else{ goto
_LL1253;} _LL1253: goto _LL1254; _LL1248:(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1255, fs, f, f_env); goto
_LL1246; _LL1250:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1257, fs, f, f_env); goto _LL1246; _LL1252:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1261, fs, f, f_env);
goto _LL1246; _LL1254:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1265= Cyc_Absynpp_stmt2string( s); xprintf("lvalue_assign_stmt: %.*s",
_temp1265.last_plus_one - _temp1265.curr, _temp1265.curr);})); goto _LL1246;
_LL1246:;} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
s); static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct _RegionHandle* r2, void*(*
f)( void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List*
result; struct Cyc_List_List* prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List*
_temp1266=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp1266->hd=( void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,
env); _temp1266->tl= 0; _temp1266;}); prev= result; for( x=(( struct Cyc_List_List*)
_check_null( x))->tl; x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){((
struct Cyc_List_List*) _check_null( prev))->tl=({ struct Cyc_List_List*
_temp1267=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp1267->hd=( void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,
env); _temp1267->tl= 0; _temp1267;}); prev=(( struct Cyc_List_List*) _check_null(
prev))->tl;} return result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(*
f)( void*, void*), void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4*
Cyc_Toc_make_dle( struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1268=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1268->f1= 0;
_temp1268->f2= e; _temp1268;});} static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void* t){ void* _temp1269= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp1275; _LL1271: if(( unsigned int) _temp1269 > 4u?*(( int*) _temp1269) ==
Cyc_Absyn_PointerType: 0){ _LL1276: _temp1275=(( struct Cyc_Absyn_PointerType_struct*)
_temp1269)->f1; goto _LL1272;} else{ goto _LL1273;} _LL1273: goto _LL1274;
_LL1272: return _temp1275; _LL1274:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp1277="get_ptr_typ:  not a pointer!";
struct _tagged_string _temp1278; _temp1278.curr= _temp1277; _temp1278.base=
_temp1277; _temp1278.last_plus_one= _temp1277 + 29; _temp1278;}))); _LL1270:;}
struct _tagged_ptr5{ struct _tuple0** curr; struct _tuple0** base; struct
_tuple0** last_plus_one; } ; struct _tagged_ptr6{ struct _tuple1** curr; struct
_tuple1** base; struct _tuple1** last_plus_one; } ; struct _tagged_ptr7{ struct
Cyc_Absyn_Exp** curr; struct Cyc_Absyn_Exp** base; struct Cyc_Absyn_Exp**
last_plus_one; } ; struct _tuple12{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp*
f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ void* _temp1279=( void*) e->r; goto _LL1280; _LL1280: if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1281= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp1281.last_plus_one - _temp1281.curr, _temp1281.curr);})); return;}{ void*
old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void*
_temp1282= _temp1279; void* _temp1360; void* _temp1362; void* _temp1364; struct
_tuple0* _temp1366; struct _tuple0* _temp1368; struct Cyc_List_List* _temp1370;
void* _temp1372; void* _temp1374; struct Cyc_Absyn_Exp* _temp1376; struct Cyc_Absyn_Exp*
_temp1378; struct Cyc_Core_Opt* _temp1380; struct Cyc_Absyn_Exp* _temp1382;
struct Cyc_Absyn_Exp* _temp1384; struct Cyc_Absyn_Exp* _temp1386; struct Cyc_Absyn_Exp*
_temp1388; struct Cyc_Absyn_Exp* _temp1390; struct Cyc_Absyn_Exp* _temp1392;
struct Cyc_List_List* _temp1394; struct Cyc_Absyn_Exp* _temp1396; struct Cyc_Absyn_VarargCallInfo*
_temp1398; struct Cyc_List_List* _temp1400; struct Cyc_Absyn_Exp* _temp1402;
struct Cyc_Absyn_VarargCallInfo* _temp1404; struct Cyc_Absyn_VarargCallInfo
_temp1406; struct Cyc_Absyn_VarargInfo* _temp1407; struct Cyc_List_List*
_temp1409; int _temp1411; struct Cyc_List_List* _temp1413; struct Cyc_Absyn_Exp*
_temp1415; struct Cyc_Absyn_Exp* _temp1417; struct Cyc_Absyn_Exp* _temp1419;
struct Cyc_List_List* _temp1421; struct Cyc_Absyn_Exp* _temp1423; struct Cyc_Absyn_Exp*
_temp1425; void* _temp1427; void** _temp1429; struct Cyc_Absyn_Exp* _temp1430;
struct Cyc_Absyn_Exp* _temp1432; struct Cyc_Absyn_Exp* _temp1434; void*
_temp1436; struct Cyc_Absyn_Exp* _temp1438; struct Cyc_Absyn_Exp* _temp1440;
struct _tagged_string* _temp1442; struct Cyc_Absyn_Exp* _temp1444; struct
_tagged_string* _temp1446; struct Cyc_Absyn_Exp* _temp1448; struct Cyc_Absyn_Exp*
_temp1450; struct Cyc_Absyn_Exp* _temp1452; struct Cyc_List_List* _temp1454;
struct Cyc_List_List* _temp1456; struct _tuple1* _temp1458; struct Cyc_List_List*
_temp1460; struct Cyc_Absyn_Exp* _temp1462; struct Cyc_Absyn_Exp* _temp1464;
struct Cyc_Absyn_Vardecl* _temp1466; struct Cyc_Absyn_Structdecl* _temp1468;
struct Cyc_List_List* _temp1470; struct Cyc_Core_Opt* _temp1472; struct _tuple0*
_temp1474; struct Cyc_List_List* _temp1476; void* _temp1478; struct Cyc_Absyn_Tunionfield*
_temp1480; struct Cyc_Absyn_Tuniondecl* _temp1482; struct Cyc_List_List*
_temp1484; struct Cyc_Core_Opt* _temp1486; struct Cyc_Core_Opt* _temp1488;
struct Cyc_Absyn_Tunionfield* _temp1490; struct Cyc_Absyn_Tuniondecl* _temp1492;
struct Cyc_List_List* _temp1494; struct Cyc_Core_Opt* _temp1496; struct Cyc_Core_Opt*
_temp1498; struct Cyc_Absyn_Enumfield* _temp1500; struct Cyc_Absyn_Enumdecl*
_temp1502; struct _tuple0* _temp1504; void* _temp1506; struct Cyc_Absyn_Exp*
_temp1508; struct Cyc_Absyn_Stmt* _temp1510; struct Cyc_List_List* _temp1512;
struct Cyc_Core_Opt* _temp1514; struct Cyc_Absyn_Fndecl* _temp1516; struct Cyc_Absyn_Exp*
_temp1518; _LL1284: if(*(( int*) _temp1282) == Cyc_Absyn_Const_e){ _LL1361:
_temp1360=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1282)->f1; if(
_temp1360 ==( void*) Cyc_Absyn_Null_c){ goto _LL1285;} else{ goto _LL1286;}}
else{ goto _LL1286;} _LL1286: if(*(( int*) _temp1282) == Cyc_Absyn_Const_e){
_LL1363: _temp1362=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1282)->f1;
goto _LL1287;} else{ goto _LL1288;} _LL1288: if(*(( int*) _temp1282) == Cyc_Absyn_Var_e){
_LL1367: _temp1366=(( struct Cyc_Absyn_Var_e_struct*) _temp1282)->f1; goto
_LL1365; _LL1365: _temp1364=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1282)->f2;
goto _LL1289;} else{ goto _LL1290;} _LL1290: if(*(( int*) _temp1282) == Cyc_Absyn_UnknownId_e){
_LL1369: _temp1368=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1282)->f1; goto
_LL1291;} else{ goto _LL1292;} _LL1292: if(*(( int*) _temp1282) == Cyc_Absyn_Primop_e){
_LL1373: _temp1372=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1282)->f1;
goto _LL1371; _LL1371: _temp1370=(( struct Cyc_Absyn_Primop_e_struct*) _temp1282)->f2;
goto _LL1293;} else{ goto _LL1294;} _LL1294: if(*(( int*) _temp1282) == Cyc_Absyn_Increment_e){
_LL1377: _temp1376=(( struct Cyc_Absyn_Increment_e_struct*) _temp1282)->f1; goto
_LL1375; _LL1375: _temp1374=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1282)->f2; goto _LL1295;} else{ goto _LL1296;} _LL1296: if(*(( int*)
_temp1282) == Cyc_Absyn_AssignOp_e){ _LL1383: _temp1382=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1282)->f1; goto _LL1381; _LL1381: _temp1380=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1282)->f2; goto _LL1379; _LL1379: _temp1378=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1282)->f3; goto _LL1297;} else{ goto _LL1298;} _LL1298: if(*(( int*)
_temp1282) == Cyc_Absyn_Conditional_e){ _LL1389: _temp1388=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1282)->f1; goto _LL1387; _LL1387: _temp1386=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1282)->f2; goto _LL1385; _LL1385: _temp1384=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1282)->f3; goto _LL1299;} else{ goto _LL1300;} _LL1300: if(*(( int*)
_temp1282) == Cyc_Absyn_SeqExp_e){ _LL1393: _temp1392=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1282)->f1; goto _LL1391; _LL1391: _temp1390=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1282)->f2; goto _LL1301;} else{ goto _LL1302;} _LL1302: if(*(( int*)
_temp1282) == Cyc_Absyn_UnknownCall_e){ _LL1397: _temp1396=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1282)->f1; goto _LL1395; _LL1395: _temp1394=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1282)->f2; goto _LL1303;} else{ goto _LL1304;} _LL1304: if(*(( int*)
_temp1282) == Cyc_Absyn_FnCall_e){ _LL1403: _temp1402=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1282)->f1; goto _LL1401; _LL1401: _temp1400=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1282)->f2; goto _LL1399; _LL1399: _temp1398=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1282)->f3; if( _temp1398 == 0){ goto _LL1305;} else{ goto _LL1306;}} else{
goto _LL1306;} _LL1306: if(*(( int*) _temp1282) == Cyc_Absyn_FnCall_e){ _LL1416:
_temp1415=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1282)->f1; goto _LL1414;
_LL1414: _temp1413=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1282)->f2; goto
_LL1405; _LL1405: _temp1404=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1282)->f3;
if( _temp1404 == 0){ goto _LL1308;} else{ _temp1406=* _temp1404; _LL1412:
_temp1411= _temp1406.num_varargs; goto _LL1410; _LL1410: _temp1409= _temp1406.injectors;
goto _LL1408; _LL1408: _temp1407= _temp1406.vai; goto _LL1307;}} else{ goto
_LL1308;} _LL1308: if(*(( int*) _temp1282) == Cyc_Absyn_Throw_e){ _LL1418:
_temp1417=(( struct Cyc_Absyn_Throw_e_struct*) _temp1282)->f1; goto _LL1309;}
else{ goto _LL1310;} _LL1310: if(*(( int*) _temp1282) == Cyc_Absyn_NoInstantiate_e){
_LL1420: _temp1419=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1282)->f1;
goto _LL1311;} else{ goto _LL1312;} _LL1312: if(*(( int*) _temp1282) == Cyc_Absyn_Instantiate_e){
_LL1424: _temp1423=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1282)->f1;
goto _LL1422; _LL1422: _temp1421=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1282)->f2; goto _LL1313;} else{ goto _LL1314;} _LL1314: if(*(( int*)
_temp1282) == Cyc_Absyn_Cast_e){ _LL1428: _temp1427=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1282)->f1; _temp1429=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1282)->f1); goto _LL1426; _LL1426: _temp1425=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1282)->f2; goto _LL1315;} else{ goto _LL1316;} _LL1316: if(*(( int*)
_temp1282) == Cyc_Absyn_Address_e){ _LL1431: _temp1430=(( struct Cyc_Absyn_Address_e_struct*)
_temp1282)->f1; goto _LL1317;} else{ goto _LL1318;} _LL1318: if(*(( int*)
_temp1282) == Cyc_Absyn_New_e){ _LL1435: _temp1434=(( struct Cyc_Absyn_New_e_struct*)
_temp1282)->f1; goto _LL1433; _LL1433: _temp1432=(( struct Cyc_Absyn_New_e_struct*)
_temp1282)->f2; goto _LL1319;} else{ goto _LL1320;} _LL1320: if(*(( int*)
_temp1282) == Cyc_Absyn_Sizeoftyp_e){ _LL1437: _temp1436=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1282)->f1; goto _LL1321;} else{ goto _LL1322;} _LL1322: if(*(( int*)
_temp1282) == Cyc_Absyn_Sizeofexp_e){ _LL1439: _temp1438=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1282)->f1; goto _LL1323;} else{ goto _LL1324;} _LL1324: if(*(( int*)
_temp1282) == Cyc_Absyn_Deref_e){ _LL1441: _temp1440=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1282)->f1; goto _LL1325;} else{ goto _LL1326;} _LL1326: if(*(( int*)
_temp1282) == Cyc_Absyn_StructMember_e){ _LL1445: _temp1444=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1282)->f1; goto _LL1443; _LL1443: _temp1442=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1282)->f2; goto _LL1327;} else{ goto _LL1328;} _LL1328: if(*(( int*)
_temp1282) == Cyc_Absyn_StructArrow_e){ _LL1449: _temp1448=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1282)->f1; goto _LL1447; _LL1447: _temp1446=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1282)->f2; goto _LL1329;} else{ goto _LL1330;} _LL1330: if(*(( int*)
_temp1282) == Cyc_Absyn_Subscript_e){ _LL1453: _temp1452=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1282)->f1; goto _LL1451; _LL1451: _temp1450=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1282)->f2; goto _LL1331;} else{ goto _LL1332;} _LL1332: if(*(( int*)
_temp1282) == Cyc_Absyn_Tuple_e){ _LL1455: _temp1454=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1282)->f1; goto _LL1333;} else{ goto _LL1334;} _LL1334: if(*(( int*)
_temp1282) == Cyc_Absyn_CompoundLit_e){ _LL1459: _temp1458=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1282)->f1; goto _LL1457; _LL1457: _temp1456=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1282)->f2; goto _LL1335;} else{ goto _LL1336;} _LL1336: if(*(( int*)
_temp1282) == Cyc_Absyn_Array_e){ _LL1461: _temp1460=(( struct Cyc_Absyn_Array_e_struct*)
_temp1282)->f1; goto _LL1337;} else{ goto _LL1338;} _LL1338: if(*(( int*)
_temp1282) == Cyc_Absyn_Comprehension_e){ _LL1467: _temp1466=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1282)->f1; goto _LL1465; _LL1465: _temp1464=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1282)->f2; goto _LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1282)->f3; goto _LL1339;} else{ goto _LL1340;} _LL1340: if(*(( int*)
_temp1282) == Cyc_Absyn_Struct_e){ _LL1475: _temp1474=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1282)->f1; goto _LL1473; _LL1473: _temp1472=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1282)->f2; goto _LL1471; _LL1471: _temp1470=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1282)->f3; goto _LL1469; _LL1469: _temp1468=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1282)->f4; goto _LL1341;} else{ goto _LL1342;} _LL1342: if(*(( int*)
_temp1282) == Cyc_Absyn_AnonStruct_e){ _LL1479: _temp1478=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1282)->f1; goto _LL1477; _LL1477: _temp1476=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1282)->f2; goto _LL1343;} else{ goto _LL1344;} _LL1344: if(*(( int*)
_temp1282) == Cyc_Absyn_Tunion_e){ _LL1489: _temp1488=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1282)->f1; goto _LL1487; _LL1487: _temp1486=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1282)->f2; goto _LL1485; _LL1485: _temp1484=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1282)->f3; if( _temp1484 == 0){ goto _LL1483;} else{ goto _LL1346;} _LL1483:
_temp1482=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1282)->f4; goto _LL1481;
_LL1481: _temp1480=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1282)->f5; goto
_LL1345;} else{ goto _LL1346;} _LL1346: if(*(( int*) _temp1282) == Cyc_Absyn_Tunion_e){
_LL1499: _temp1498=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1282)->f1; goto
_LL1497; _LL1497: _temp1496=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1282)->f2;
goto _LL1495; _LL1495: _temp1494=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1282)->f3;
goto _LL1493; _LL1493: _temp1492=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1282)->f4;
goto _LL1491; _LL1491: _temp1490=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1282)->f5;
goto _LL1347;} else{ goto _LL1348;} _LL1348: if(*(( int*) _temp1282) == Cyc_Absyn_Enum_e){
_LL1505: _temp1504=(( struct Cyc_Absyn_Enum_e_struct*) _temp1282)->f1; goto
_LL1503; _LL1503: _temp1502=(( struct Cyc_Absyn_Enum_e_struct*) _temp1282)->f2;
goto _LL1501; _LL1501: _temp1500=(( struct Cyc_Absyn_Enum_e_struct*) _temp1282)->f3;
goto _LL1349;} else{ goto _LL1350;} _LL1350: if(*(( int*) _temp1282) == Cyc_Absyn_Malloc_e){
_LL1509: _temp1508=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1282)->f1; goto
_LL1507; _LL1507: _temp1506=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1282)->f2; goto _LL1351;} else{ goto _LL1352;} _LL1352: if(*(( int*)
_temp1282) == Cyc_Absyn_StmtExp_e){ _LL1511: _temp1510=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1282)->f1; goto _LL1353;} else{ goto _LL1354;} _LL1354: if(*(( int*)
_temp1282) == Cyc_Absyn_UnresolvedMem_e){ _LL1515: _temp1514=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1282)->f1; goto _LL1513; _LL1513: _temp1512=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1282)->f2; goto _LL1355;} else{ goto _LL1356;} _LL1356: if(*(( int*)
_temp1282) == Cyc_Absyn_Codegen_e){ _LL1517: _temp1516=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1282)->f1; goto _LL1357;} else{ goto _LL1358;} _LL1358: if(*(( int*)
_temp1282) == Cyc_Absyn_Fill_e){ _LL1519: _temp1518=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1282)->f1; goto _LL1359;} else{ goto _LL1283;} _LL1285:{ void* _temp1520=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1526; struct Cyc_Absyn_Conref*
_temp1528; struct Cyc_Absyn_Tqual _temp1530; struct Cyc_Absyn_Conref* _temp1532;
void* _temp1534; void* _temp1536; _LL1522: if(( unsigned int) _temp1520 > 4u?*((
int*) _temp1520) == Cyc_Absyn_PointerType: 0){ _LL1527: _temp1526=(( struct Cyc_Absyn_PointerType_struct*)
_temp1520)->f1; _LL1537: _temp1536=( void*) _temp1526.elt_typ; goto _LL1535;
_LL1535: _temp1534=( void*) _temp1526.rgn_typ; goto _LL1533; _LL1533: _temp1532=
_temp1526.nullable; goto _LL1531; _LL1531: _temp1530= _temp1526.tq; goto _LL1529;
_LL1529: _temp1528= _temp1526.bounds; goto _LL1523;} else{ goto _LL1524;}
_LL1524: goto _LL1525; _LL1523: if(( void*) _temp1528->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1521;}{ void* _temp1538=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1528); struct Cyc_Absyn_Exp* _temp1544; _LL1540: if( _temp1538 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1541;} else{ goto _LL1542;} _LL1542: if((
unsigned int) _temp1538 > 1u?*(( int*) _temp1538) == Cyc_Absyn_Upper_b: 0){
_LL1545: _temp1544=(( struct Cyc_Absyn_Upper_b_struct*) _temp1538)->f1; goto
_LL1543;} else{ goto _LL1539;} _LL1541: { struct _tuple4* _temp1547=({ struct
_tuple4* _temp1546=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1546->f1= 0; _temp1546->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1546;});
goto _LL1548; _LL1548: { struct Cyc_Absyn_Exp* _temp1555= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List*(* _temp1549)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)(
struct _tagged_ptr3)) Cyc_List_list; struct _tuple4* _temp1552= _temp1547;
struct _tuple4* _temp1553= _temp1547; struct _tuple4* _temp1554= _temp1547;
struct _tuple4* _temp1550[ 3u]={ _temp1552, _temp1553, _temp1554}; struct
_tagged_ptr3 _temp1551={ _temp1550, _temp1550, _temp1550 + 3u}; _temp1549(
_temp1551);}), 0); goto _LL1556; _LL1556: if( nv->toplevel){( void*)( e->r=(
void*)(( void*) _temp1555->r));} else{ void* _temp1557= Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1536), Cyc_Toc_mt_tq); goto _LL1558; _LL1558: { struct
_tuple0* _temp1559= Cyc_Toc_temp_var(); goto _LL1560; _LL1560: { struct Cyc_Absyn_Exp*
_temp1561= Cyc_Absyn_var_exp( _temp1559, 0); goto _LL1562; _LL1562: { struct Cyc_Absyn_Stmt*
_temp1563= Cyc_Absyn_declare_stmt( _temp1559, _temp1557,( struct Cyc_Absyn_Exp*)
_temp1555, Cyc_Absyn_exp_stmt( _temp1561, 0), 0); goto _LL1564; _LL1564:( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1563, 0))->r));}}}} goto
_LL1539;}} _LL1543:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0,
0))->r)); goto _LL1539; _LL1539:;} goto _LL1521; _LL1525:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1565= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1565.last_plus_one -
_temp1565.curr, _temp1565.curr);})); goto _LL1521; _LL1521:;} goto _LL1283;
_LL1287: goto _LL1283; _LL1289:{ struct _handler_cons _temp1566; _push_handler(&
_temp1566);{ int _temp1568= 0; if( setjmp( _temp1566.handler)){ _temp1568= 1;}
if( ! _temp1568){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1366))->r));; _pop_handler();} else{ void* _temp1567=( void*) _exn_thrown;
void* _temp1570= _temp1567; _LL1572: if( _temp1570 == Cyc_Dict_Absent){ goto
_LL1573;} else{ goto _LL1574;} _LL1574: goto _LL1575; _LL1573:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1576= Cyc_Absynpp_qvar2string(
_temp1366); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1576.last_plus_one
- _temp1576.curr, _temp1576.curr);})); return; _LL1575:( void) _throw( _temp1570);
_LL1571:;}}} goto _LL1283; _LL1291:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1577="unknownid"; struct _tagged_string _temp1578; _temp1578.curr=
_temp1577; _temp1578.base= _temp1577; _temp1578.last_plus_one= _temp1577 + 10;
_temp1578;})); goto _LL1283; _LL1293: { struct Cyc_List_List* _temp1579=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp1370); goto _LL1580; _LL1580: {
struct Cyc_List_List* _temp1581=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1370); goto _LL1582; _LL1582:(( void(*)( void(* f)( struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)(
Cyc_Toc_exp_to_c, nv, _temp1370);{ void* _temp1583= _temp1372; _LL1585: if(
_temp1583 ==( void*) Cyc_Absyn_Size){ goto _LL1586;} else{ goto _LL1587;}
_LL1587: if( _temp1583 ==( void*) Cyc_Absyn_Printf){ goto _LL1588;} else{ goto
_LL1589;} _LL1589: if( _temp1583 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1590;}
else{ goto _LL1591;} _LL1591: if( _temp1583 ==( void*) Cyc_Absyn_Fprintf){ goto
_LL1592;} else{ goto _LL1593;} _LL1593: if( _temp1583 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1594;} else{ goto _LL1595;} _LL1595: if( _temp1583 ==( void*) Cyc_Absyn_Plus){
goto _LL1596;} else{ goto _LL1597;} _LL1597: if( _temp1583 ==( void*) Cyc_Absyn_Minus){
goto _LL1598;} else{ goto _LL1599;} _LL1599: if( _temp1583 ==( void*) Cyc_Absyn_Eq){
goto _LL1600;} else{ goto _LL1601;} _LL1601: if( _temp1583 ==( void*) Cyc_Absyn_Neq){
goto _LL1602;} else{ goto _LL1603;} _LL1603: if( _temp1583 ==( void*) Cyc_Absyn_Gt){
goto _LL1604;} else{ goto _LL1605;} _LL1605: if( _temp1583 ==( void*) Cyc_Absyn_Gte){
goto _LL1606;} else{ goto _LL1607;} _LL1607: if( _temp1583 ==( void*) Cyc_Absyn_Lt){
goto _LL1608;} else{ goto _LL1609;} _LL1609: if( _temp1583 ==( void*) Cyc_Absyn_Lte){
goto _LL1610;} else{ goto _LL1611;} _LL1611: goto _LL1612; _LL1586: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1370))->hd;{
void* _temp1613=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1621; struct Cyc_Absyn_Tqual _temp1623; void*
_temp1625; struct Cyc_Absyn_PtrInfo _temp1627; struct Cyc_Absyn_Conref*
_temp1629; struct Cyc_Absyn_Tqual _temp1631; struct Cyc_Absyn_Conref* _temp1633;
void* _temp1635; void* _temp1637; _LL1615: if(( unsigned int) _temp1613 > 4u?*((
int*) _temp1613) == Cyc_Absyn_ArrayType: 0){ _LL1626: _temp1625=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1613)->f1; goto _LL1624; _LL1624:
_temp1623=(( struct Cyc_Absyn_ArrayType_struct*) _temp1613)->f2; goto _LL1622;
_LL1622: _temp1621=(( struct Cyc_Absyn_ArrayType_struct*) _temp1613)->f3; goto
_LL1616;} else{ goto _LL1617;} _LL1617: if(( unsigned int) _temp1613 > 4u?*((
int*) _temp1613) == Cyc_Absyn_PointerType: 0){ _LL1628: _temp1627=(( struct Cyc_Absyn_PointerType_struct*)
_temp1613)->f1; _LL1638: _temp1637=( void*) _temp1627.elt_typ; goto _LL1636;
_LL1636: _temp1635=( void*) _temp1627.rgn_typ; goto _LL1634; _LL1634: _temp1633=
_temp1627.nullable; goto _LL1632; _LL1632: _temp1631= _temp1627.tq; goto _LL1630;
_LL1630: _temp1629= _temp1627.bounds; goto _LL1618;} else{ goto _LL1619;}
_LL1619: goto _LL1620; _LL1616:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1621))->r)); goto _LL1614; _LL1618: { struct _tuple0*
_temp1639= Cyc_Toc_temp_var(); goto _LL1640; _LL1640: { void* _temp1641= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); goto _LL1642;
_LL1642: { struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1639, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1639, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4=
Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp( e2, e3, 0), 0);
struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1639, _temp1641,(
struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1614;}}} _LL1620:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1643= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*) _check_null(
_temp1581))->hd); struct _tagged_string _temp1644= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_temp1643.last_plus_one - _temp1643.curr, _temp1643.curr, _temp1644.last_plus_one
- _temp1644.curr, _temp1644.curr);})); return; _LL1614:;} goto _LL1584;} _LL1588:(
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1372, 0, _temp1370,
_temp1581))->r)); goto _LL1584; _LL1590:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1372, 0, _temp1370, _temp1581))->r)); goto _LL1584; _LL1592: if( _temp1370
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char*
_temp1645="fprintf without arguments"; struct _tagged_string _temp1646;
_temp1646.curr= _temp1645; _temp1646.base= _temp1645; _temp1646.last_plus_one=
_temp1645 + 26; _temp1646;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1372,({ struct Cyc_Core_Opt* _temp1647=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1647->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1370))->hd); _temp1647;}),(( struct Cyc_List_List*)
_check_null( _temp1370))->tl,(( struct Cyc_List_List*) _check_null( _temp1581))->tl))->r));
goto _LL1584; _LL1594: if( _temp1370 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1648="sscanf without arguments";
struct _tagged_string _temp1649; _temp1649.curr= _temp1648; _temp1649.base=
_temp1648; _temp1649.last_plus_one= _temp1648 + 25; _temp1649;}));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1370))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1370))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1370))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1584; _LL1596: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1370))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1370))->tl))->hd; struct _tuple0* _temp1650= Cyc_Toc_temp_var(); goto
_LL1651; _LL1651: { void* _temp1652= Cyc_Toc_typ_to_c( old_typ); goto _LL1653;
_LL1653: { struct Cyc_Absyn_Stmt* _temp1654= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1650, 0), 0); goto _LL1655; _LL1655: { struct Cyc_Absyn_Exp* _temp1656= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1650, 0), Cyc_Toc_curr_sp, 0); goto _LL1657; _LL1657: {
struct Cyc_Absyn_Exp* _temp1659= Cyc_Absyn_assignop_exp( _temp1656,({ struct Cyc_Core_Opt*
_temp1658=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1658->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1658;}), e2, 0); goto
_LL1660; _LL1660: _temp1654= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1659,
0), _temp1654, 0); _temp1654= Cyc_Absyn_declare_stmt( _temp1650, _temp1652,(
struct Cyc_Absyn_Exp*) e1, _temp1654, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1654, 0))->r));}}}}} goto _LL1584; _LL1598: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null( _temp1579))->hd)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1370))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1370))->tl))->hd; void*
old_typ_e2=( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1579))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* _temp1661= Cyc_Toc_temp_var(); goto _LL1662;
_LL1662: { void* _temp1663= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_List_List*)
_check_null( _temp1579))->hd); goto _LL1664; _LL1664: { struct Cyc_Absyn_Stmt*
_temp1665= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1661, 0), 0); goto
_LL1666; _LL1666: { struct Cyc_Absyn_Exp* _temp1667= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1661, 0), Cyc_Toc_curr_sp, 0); goto _LL1668; _LL1668: {
struct Cyc_Absyn_Exp* _temp1670= Cyc_Absyn_assignop_exp( _temp1667,({ struct Cyc_Core_Opt*
_temp1669=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1669->v=( void*)(( void*) Cyc_Absyn_Minus); _temp1669;}), e2, 0); goto
_LL1671; _LL1671: _temp1665= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1670,
0), _temp1665, 0); _temp1665= Cyc_Absyn_declare_stmt( _temp1661, _temp1663,(
struct Cyc_Absyn_Exp*) e1, _temp1665, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1665, 0))->r));}}}}}} goto _LL1584; _LL1600: goto _LL1602; _LL1602: goto
_LL1604; _LL1604: goto _LL1606; _LL1606: goto _LL1608; _LL1608: goto _LL1610;
_LL1610: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1370))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1370))->tl))->hd; void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp1579))->hd; void* t2=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1579))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1584;} _LL1612: goto
_LL1584; _LL1584:;} goto _LL1283;}} _LL1295: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1376->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1376); if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if( _temp1374 ==(
void*) Cyc_Absyn_PreInc? 1: _temp1374 ==( void*) Cyc_Absyn_PreDec){ struct
_tuple0* _temp1672= Cyc_Toc_temp_var(); goto _LL1673; _LL1673: { void* _temp1674=
Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1675; _LL1675: { struct Cyc_Absyn_Stmt*
_temp1676= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp1672,
0), 0), 0); goto _LL1677; _LL1677: { struct Cyc_Absyn_Exp* _temp1678= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1672, 0), Cyc_Toc_curr_sp, 0),
_temp1374, 0); goto _LL1679; _LL1679: _temp1676= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1678, 0), _temp1676, 0); _temp1676= Cyc_Absyn_declare_stmt( _temp1672,
_temp1674,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1376, 0),
_temp1676, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1676, 0))->r));}}}}
else{ struct _tuple0* _temp1680= Cyc_Toc_temp_var(); goto _LL1681; _LL1681: {
struct _tuple0* _temp1682= Cyc_Toc_temp_var(); goto _LL1683; _LL1683: { void*
_temp1684= Cyc_Toc_typ_to_c( e2_cyc_typ); goto _LL1685; _LL1685: { void*
_temp1686= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1687; _LL1687: { struct Cyc_Absyn_Stmt*
_temp1688= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1682, 0), 0); goto
_LL1689; _LL1689: { struct Cyc_Absyn_Exp* _temp1690= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1680, 0), Cyc_Toc_curr_sp, 0),
_temp1374, 0); goto _LL1691; _LL1691: _temp1688= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1690, 0), _temp1688, 0); _temp1688= Cyc_Absyn_declare_stmt( _temp1682,
_temp1684,( struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp1680, 0), 0), _temp1688, 0); _temp1688= Cyc_Absyn_declare_stmt( _temp1680,
_temp1686,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1376, 0),
_temp1688, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1688, 0))->r));}}}}}}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1376)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1376, 0, Cyc_Toc_incr_lvalue,
_temp1374);( void*)( e->r=( void*)(( void*) _temp1376->r));}} goto _LL1283;}
_LL1297: { int e1_poly= Cyc_Toc_is_poly_project( _temp1382); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1382->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1378->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1382); Cyc_Toc_exp_to_c( nv, _temp1378); if(
_temp1380 != 0?( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1380))->v ==(
void*) Cyc_Absyn_Plus? 1:( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1380))->v
==( void*) Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){(
void*)( _temp1382->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1382->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1378->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1378->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1378->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1378->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1382)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1382, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1692=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1692[ 0]=({ struct _tuple11 _temp1693; _temp1693.f1=
_temp1380; _temp1693.f2= _temp1378; _temp1693;}); _temp1692;}));( void*)( e->r=(
void*)(( void*) _temp1382->r));} goto _LL1283;} _LL1299: Cyc_Toc_exp_to_c( nv,
_temp1388); Cyc_Toc_exp_to_c( nv, _temp1386); Cyc_Toc_exp_to_c( nv, _temp1384);
goto _LL1283; _LL1301: Cyc_Toc_exp_to_c( nv, _temp1392); Cyc_Toc_exp_to_c( nv,
_temp1390); goto _LL1283; _LL1303: _temp1402= _temp1396; _temp1400= _temp1394;
goto _LL1305; _LL1305: Cyc_Toc_exp_to_c( nv, _temp1402);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1400); goto _LL1283; _LL1307:{
struct _RegionHandle _temp1694= _new_region(); struct _RegionHandle* r=&
_temp1694; _push_region( r);{ void* _temp1695=( void*) _temp1407->type; goto
_LL1696; _LL1696: { void* _temp1697= Cyc_Toc_typ_to_c( _temp1695); goto _LL1698;
_LL1698: { void* _temp1699= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1415->topt))->v); goto _LL1700; _LL1700: { struct Cyc_Absyn_Exp*
_temp1701= Cyc_Absyn_uint_exp(( unsigned int) _temp1411, 0); goto _LL1702;
_LL1702: { void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1774=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1774[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1775; _temp1775.tag= Cyc_Absyn_ArrayType;
_temp1775.f1=( void*) _temp1695; _temp1775.f2= Cyc_Toc_mt_tq; _temp1775.f3=(
struct Cyc_Absyn_Exp*) _temp1701; _temp1775;}); _temp1774;})); struct _tuple0*
_temp1703= Cyc_Toc_temp_var(); goto _LL1704; _LL1704: { struct _tuple0*
_temp1705= Cyc_Toc_temp_var(); goto _LL1706; _LL1706: { struct Cyc_Absyn_Exp*
_temp1707= Cyc_Absyn_var_exp( _temp1705, 0); goto _LL1708; _LL1708: { struct
_tuple0* _temp1709= Cyc_Toc_temp_var(); goto _LL1710; _LL1710: { int _temp1711=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1413); goto _LL1712;
_LL1712: { int _temp1713= _temp1711 - _temp1411; goto _LL1714; _LL1714: { struct
_tagged_ptr5 vs=({ unsigned int _temp1770=( unsigned int) _temp1411; struct
_tuple0** _temp1771=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1770); struct _tagged_ptr5 _temp1773={ _temp1771, _temp1771,
_temp1771 + _temp1770};{ unsigned int _temp1772= _temp1770; unsigned int i; for(
i= 0; i < _temp1772; i ++){ _temp1771[ i]= Cyc_Toc_temp_var();}}; _temp1773;});
struct _tagged_ptr5 xs=({ unsigned int _temp1766=( unsigned int) _temp1713;
struct _tuple0** _temp1767=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1766); struct _tagged_ptr5 _temp1769={ _temp1767, _temp1767,
_temp1767 + _temp1766};{ unsigned int _temp1768= _temp1766; unsigned int i; for(
i= 0; i < _temp1768; i ++){ _temp1767[ i]= Cyc_Toc_temp_var();}}; _temp1769;});
struct Cyc_List_List* xexps=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(*
f)( struct _tuple0*, struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,( struct Cyc_Position_Segment*)
0,(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct _tagged_ptr5 arr))
Cyc_List_rfrom_array)( r, xs)); xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List*
_temp1715=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1715->hd=( void*) Cyc_Absyn_var_exp( _temp1709, 0); _temp1715->tl= 0;
_temp1715;}));{ struct Cyc_Absyn_Stmt* _temp1716= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( _temp1703, 0), xexps, 0), 0); goto _LL1717; _LL1717: { struct
Cyc_Absyn_Exp* _temp1718= Cyc_Absyn_add_exp( _temp1707, _temp1701, 0); goto
_LL1719; _LL1719: { struct Cyc_List_List* _temp1729=({ struct Cyc_List_List*(*
_temp1720)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)( struct _tagged_ptr3))
Cyc_List_list; struct _tuple4* _temp1723=({ struct _tuple4* _temp1728=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1728->f1= 0; _temp1728->f2=
_temp1707; _temp1728;}); struct _tuple4* _temp1724=({ struct _tuple4* _temp1727=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1727->f1= 0;
_temp1727->f2= _temp1707; _temp1727;}); struct _tuple4* _temp1725=({ struct
_tuple4* _temp1726=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1726->f1= 0; _temp1726->f2= _temp1718; _temp1726;}); struct _tuple4*
_temp1721[ 3u]={ _temp1723, _temp1724, _temp1725}; struct _tagged_ptr3 _temp1722={
_temp1721, _temp1721, _temp1721 + 3u}; _temp1720( _temp1722);}); goto _LL1730;
_LL1730: _temp1716= Cyc_Absyn_declare_stmt( _temp1709, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp1695,( void*) _temp1407->rgn, Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp(
0, _temp1729, 0), _temp1716, 0);{ struct Cyc_List_List* _temp1731=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_ptr5 arr)) Cyc_List_rfrom_array)( r, vs));
goto _LL1732; _LL1732: { struct Cyc_List_List* _temp1733=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_make_dle, _temp1731); goto _LL1734; _LL1734: _temp1716= Cyc_Absyn_declare_stmt(
_temp1705, arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1733, 0), _temp1716, 0);{ struct Cyc_List_List* _temp1735=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1413);
goto _LL1736; _LL1736: { struct Cyc_List_List* _temp1737=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1409);
goto _LL1738; _LL1738:{ int i= _temp1411 - 1; for( 0; i >= 0;( i --, _temp1735=((
struct Cyc_List_List*) _check_null( _temp1735))->tl)){ struct Cyc_Absyn_Exp* arg=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1735))->hd;
void* arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c(
nv, arg); if( _temp1407->inject){ void* _temp1741; struct Cyc_Position_Segment*
_temp1743; struct Cyc_List_List* _temp1745; struct Cyc_List_List* _temp1747;
struct _tuple0* _temp1749; struct Cyc_Absyn_Tunionfield _temp1739=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1737))->hd); _LL1750: _temp1749=
_temp1739.name; goto _LL1748; _LL1748: _temp1747= _temp1739.tvs; goto _LL1746;
_LL1746: _temp1745= _temp1739.typs; goto _LL1744; _LL1744: _temp1743= _temp1739.loc;
goto _LL1742; _LL1742: _temp1741=( void*) _temp1739.sc; goto _LL1740; _LL1740:
_temp1737=(( struct Cyc_List_List*) _check_null( _temp1737))->tl;{ struct
_tuple0* _temp1751= Cyc_Toc_temp_var(); goto _LL1752; _LL1752: { struct Cyc_Absyn_Exp*
_temp1753= Cyc_Absyn_var_exp( _temp1751, 0); goto _LL1754; _LL1754: { void*
_temp1757= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1749,({
unsigned char* _temp1755="_struct"; struct _tagged_string _temp1756; _temp1756.curr=
_temp1755; _temp1756.base= _temp1755; _temp1756.last_plus_one= _temp1755 + 8;
_temp1756;}))); goto _LL1758; _LL1758: _temp1716= Cyc_Absyn_declare_stmt(*((
struct _tuple0**(*)( struct _tagged_ptr5, unsigned int, unsigned int))
_check_unknown_subscript)( vs, sizeof( struct _tuple0*), i), _temp1697,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1697, Cyc_Absyn_address_exp( _temp1753,
0), 0), _temp1716, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp1745))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1762= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1753,({
struct _tagged_string* _temp1759=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp1759[ 0]=({ unsigned char* _temp1760="f1"; struct
_tagged_string _temp1761; _temp1761.curr= _temp1760; _temp1761.base= _temp1760;
_temp1761.last_plus_one= _temp1760 + 3; _temp1761;}); _temp1759;}), 0), arg, 0);
goto _LL1763; _LL1763: _temp1716= Cyc_Absyn_seq_stmt( _temp1762, _temp1716, 0);{
struct Cyc_Absyn_Stmt* _temp1764= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1753, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1749, 0), 0); goto
_LL1765; _LL1765: _temp1716= Cyc_Absyn_seq_stmt( _temp1764, _temp1716, 0);
_temp1716= Cyc_Absyn_declare_stmt( _temp1751, _temp1757, 0, _temp1716, 0);}}}}}}}
else{ _temp1716= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr5, unsigned int, unsigned int)) _check_unknown_subscript)( vs,
sizeof( struct _tuple0*), i), _temp1697,( struct Cyc_Absyn_Exp*) arg, _temp1716,
0);}}}{ int i= _temp1713 - 1; for( 0; i >= 0;( i --, _temp1735=(( struct Cyc_List_List*)
_check_null( _temp1735))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1735))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1716= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr5, unsigned int, unsigned int)) _check_unknown_subscript)( xs,
sizeof( struct _tuple0*), i), argtype,( struct Cyc_Absyn_Exp*) arg, _temp1716, 0);}}
Cyc_Toc_exp_to_c( nv, _temp1415); _temp1716= Cyc_Absyn_declare_stmt( _temp1703,
_temp1699,( struct Cyc_Absyn_Exp*) _temp1415, _temp1716, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1716, 0))->r));}}}}}}}}}}}}}}}}}};
_pop_region(& _temp1694);} goto _LL1283; _LL1309: Cyc_Toc_exp_to_c( nv,
_temp1417);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1417), 0))->r)); goto _LL1283; _LL1311: Cyc_Toc_exp_to_c(
nv, _temp1419); goto _LL1283; _LL1313: Cyc_Toc_exp_to_c( nv, _temp1423);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
for( 0; _temp1421 != 0; _temp1421=(( struct Cyc_List_List*) _check_null(
_temp1421))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*)
_check_null( _temp1421))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*)
Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1423, 0))->r)); break;}} goto _LL1283;} _LL1315: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1425->topt))->v; void* new_typ=*
_temp1429;* _temp1429= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1425);{ int noconv= 0;{ void* _temp1776= Cyc_Toc_conversion( old_t2,
new_typ); void* _temp1786; struct Cyc_Absyn_Exp* _temp1788; void* _temp1790; int
_temp1792; struct Cyc_Absyn_Exp* _temp1794; void* _temp1796; _LL1778: if(
_temp1776 ==( void*) Cyc_Toc_NoConv){ goto _LL1779;} else{ goto _LL1780;}
_LL1780: if(( unsigned int) _temp1776 > 1u?*(( int*) _temp1776) == Cyc_Toc_NullCheck:
0){ _LL1787: _temp1786=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp1776)->f1;
goto _LL1781;} else{ goto _LL1782;} _LL1782: if(( unsigned int) _temp1776 > 1u?*((
int*) _temp1776) == Cyc_Toc_TagPtr: 0){ _LL1791: _temp1790=( void*)(( struct Cyc_Toc_TagPtr_struct*)
_temp1776)->f1; goto _LL1789; _LL1789: _temp1788=(( struct Cyc_Toc_TagPtr_struct*)
_temp1776)->f2; goto _LL1783;} else{ goto _LL1784;} _LL1784: if(( unsigned int)
_temp1776 > 1u?*(( int*) _temp1776) == Cyc_Toc_UntagPtr: 0){ _LL1797: _temp1796=(
void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp1776)->f1; goto _LL1795; _LL1795:
_temp1794=(( struct Cyc_Toc_UntagPtr_struct*) _temp1776)->f2; goto _LL1793;
_LL1793: _temp1792=(( struct Cyc_Toc_UntagPtr_struct*) _temp1776)->f3; goto
_LL1785;} else{ goto _LL1777;} _LL1779: goto _LL1777; _LL1781:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_null_check_conv( nv->toplevel, _temp1786, _temp1425))->r));
goto _LL1777; _LL1783:( void*)( e->r=( void*)(( void*)( Cyc_Toc_tag_ptr_conv( nv->toplevel,
_temp1790, _temp1788, _temp1425))->r)); goto _LL1777; _LL1785:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_untag_ptr_conv( nv->toplevel, _temp1796, _temp1794,
_temp1792, _temp1425))->r)); goto _LL1777; _LL1777:;} goto _LL1283;}} _LL1317:{
void* _temp1798=( void*) _temp1430->r; struct Cyc_Absyn_Structdecl* _temp1806;
struct Cyc_List_List* _temp1808; struct Cyc_Core_Opt* _temp1810; struct _tuple0*
_temp1812; struct Cyc_List_List* _temp1814; _LL1800: if(*(( int*) _temp1798) ==
Cyc_Absyn_Struct_e){ _LL1813: _temp1812=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1798)->f1; goto _LL1811; _LL1811: _temp1810=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1798)->f2; goto _LL1809; _LL1809: _temp1808=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1798)->f3; goto _LL1807; _LL1807: _temp1806=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1798)->f4; goto _LL1801;} else{ goto _LL1802;} _LL1802: if(*(( int*)
_temp1798) == Cyc_Absyn_Tuple_e){ _LL1815: _temp1814=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1798)->f1; goto _LL1803;} else{ goto _LL1804;} _LL1804: goto _LL1805;
_LL1801: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1816= Cyc_Position_string_of_segment( _temp1430->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1816.last_plus_one -
_temp1816.curr, _temp1816.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1430->topt))->v, 1, 0,
_temp1808, _temp1812))->r)); goto _LL1799; _LL1803: if( nv->toplevel){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1817= Cyc_Position_string_of_segment(
_temp1430->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1817.last_plus_one - _temp1817.curr, _temp1817.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1814))->r)); goto _LL1799;
_LL1805: Cyc_Toc_exp_to_c( nv, _temp1430); if( ! Cyc_Absyn_is_lvalue( _temp1430)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1430,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1430, 0))->r));} goto _LL1799; _LL1799:;} goto _LL1283; _LL1319: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string
_temp1818= Cyc_Position_string_of_segment( _temp1432->loc); xprintf("%.*s: new at top-level",
_temp1818.last_plus_one - _temp1818.curr, _temp1818.curr);}));}{ void* _temp1819=(
void*) _temp1432->r; struct Cyc_List_List* _temp1833; void* _temp1835; struct
_tagged_string _temp1837; struct Cyc_Absyn_Exp* _temp1839; struct Cyc_Absyn_Exp*
_temp1841; struct Cyc_Absyn_Vardecl* _temp1843; struct Cyc_Absyn_Structdecl*
_temp1845; struct Cyc_List_List* _temp1847; struct Cyc_Core_Opt* _temp1849;
struct _tuple0* _temp1851; struct Cyc_List_List* _temp1853; _LL1821: if(*(( int*)
_temp1819) == Cyc_Absyn_Array_e){ _LL1834: _temp1833=(( struct Cyc_Absyn_Array_e_struct*)
_temp1819)->f1; goto _LL1822;} else{ goto _LL1823;} _LL1823: if(*(( int*)
_temp1819) == Cyc_Absyn_Const_e){ _LL1836: _temp1835=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1819)->f1; if(( unsigned int) _temp1835 > 1u?*(( int*) _temp1835) == Cyc_Absyn_String_c:
0){ _LL1838: _temp1837=(( struct Cyc_Absyn_String_c_struct*) _temp1835)->f1;
goto _LL1824;} else{ goto _LL1825;}} else{ goto _LL1825;} _LL1825: if(*(( int*)
_temp1819) == Cyc_Absyn_Comprehension_e){ _LL1844: _temp1843=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1819)->f1; goto _LL1842; _LL1842: _temp1841=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1819)->f2; goto _LL1840; _LL1840: _temp1839=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1819)->f3; goto _LL1826;} else{ goto _LL1827;} _LL1827: if(*(( int*)
_temp1819) == Cyc_Absyn_Struct_e){ _LL1852: _temp1851=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1819)->f1; goto _LL1850; _LL1850: _temp1849=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1819)->f2; goto _LL1848; _LL1848: _temp1847=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1819)->f3; goto _LL1846; _LL1846: _temp1845=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1819)->f4; goto _LL1828;} else{ goto _LL1829;} _LL1829: if(*(( int*)
_temp1819) == Cyc_Absyn_Tuple_e){ _LL1854: _temp1853=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1819)->f1; goto _LL1830;} else{ goto _LL1831;} _LL1831: goto _LL1832;
_LL1822: { struct _tuple0* _temp1855= Cyc_Toc_temp_var(); goto _LL1856; _LL1856: {
struct Cyc_Absyn_Exp* _temp1857= Cyc_Absyn_var_exp( _temp1855, 0); goto _LL1858;
_LL1858: { struct Cyc_Absyn_Stmt* _temp1859= Cyc_Toc_init_array( nv, _temp1857,
_temp1833, Cyc_Absyn_exp_stmt( _temp1857, 0)); goto _LL1860; _LL1860: { void*
old_elt_typ;{ void* _temp1861= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1867; struct Cyc_Absyn_Conref* _temp1869; struct Cyc_Absyn_Tqual _temp1871;
struct Cyc_Absyn_Conref* _temp1873; void* _temp1875; void* _temp1877; _LL1863:
if(( unsigned int) _temp1861 > 4u?*(( int*) _temp1861) == Cyc_Absyn_PointerType:
0){ _LL1868: _temp1867=(( struct Cyc_Absyn_PointerType_struct*) _temp1861)->f1;
_LL1878: _temp1877=( void*) _temp1867.elt_typ; goto _LL1876; _LL1876: _temp1875=(
void*) _temp1867.rgn_typ; goto _LL1874; _LL1874: _temp1873= _temp1867.nullable;
goto _LL1872; _LL1872: _temp1871= _temp1867.tq; goto _LL1870; _LL1870: _temp1869=
_temp1867.bounds; goto _LL1864;} else{ goto _LL1865;} _LL1865: goto _LL1866;
_LL1864: old_elt_typ= _temp1877; goto _LL1862; _LL1866: old_elt_typ=(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1879="exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1880; _temp1880.curr= _temp1879; _temp1880.base=
_temp1879; _temp1880.last_plus_one= _temp1879 + 52; _temp1880;})); goto _LL1862;
_LL1862:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* _temp1881= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1882; _LL1882: { struct Cyc_Absyn_Exp*
_temp1883= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1833), 0), 0); goto
_LL1884; _LL1884: { struct Cyc_Absyn_Exp* e1; if( _temp1434 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1883);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1434); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1883);}{ struct Cyc_Absyn_Exp* _temp1885= Cyc_Absyn_cast_exp( _temp1881, e1,
0); goto _LL1886; _LL1886:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1855, _temp1881,( struct Cyc_Absyn_Exp*) _temp1885,
_temp1859, 0), 0))->r)); goto _LL1820;}}}}}}}} _LL1824: if( _temp1434 == 0){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1887=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1887->f1= Cyc_Toc_abs_ns; _temp1887->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1887;}), 0),({ struct Cyc_List_List* _temp1888=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1888->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1889=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1889[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1890; _temp1890.tag= Cyc_Absyn_Const_e; _temp1890.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1891=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1891[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1892; _temp1892.tag= Cyc_Absyn_String_c;
_temp1892.f1= _temp1837; _temp1892;}); _temp1891;})); _temp1890;}); _temp1889;}),
0); _temp1888->tl= 0; _temp1888;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( _temp1434); Cyc_Toc_exp_to_c( nv, r);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({ struct
_tuple0* _temp1893=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1893->f1= Cyc_Toc_abs_ns; _temp1893->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1893;}), 0),({ struct Cyc_List_List* _temp1894=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1894->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1895=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1895[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1896; _temp1896.tag= Cyc_Absyn_Const_e; _temp1896.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1897=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1897[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1898; _temp1898.tag= Cyc_Absyn_String_c;
_temp1898.f1= _temp1837; _temp1898;}); _temp1897;})); _temp1896;}); _temp1895;}),
0); _temp1894->tl= 0; _temp1894;}), 0))->r));} goto _LL1820; _LL1826: { int
is_tagged_ptr= 0;{ void* _temp1899= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1905; struct Cyc_Absyn_Conref* _temp1907; struct Cyc_Absyn_Tqual _temp1909;
struct Cyc_Absyn_Conref* _temp1911; void* _temp1913; void* _temp1915; _LL1901:
if(( unsigned int) _temp1899 > 4u?*(( int*) _temp1899) == Cyc_Absyn_PointerType:
0){ _LL1906: _temp1905=(( struct Cyc_Absyn_PointerType_struct*) _temp1899)->f1;
_LL1916: _temp1915=( void*) _temp1905.elt_typ; goto _LL1914; _LL1914: _temp1913=(
void*) _temp1905.rgn_typ; goto _LL1912; _LL1912: _temp1911= _temp1905.nullable;
goto _LL1910; _LL1910: _temp1909= _temp1905.tq; goto _LL1908; _LL1908: _temp1907=
_temp1905.bounds; goto _LL1902;} else{ goto _LL1903;} _LL1903: goto _LL1904;
_LL1902:{ void* _temp1917=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1907))->v; void* _temp1923; _LL1919: if((
unsigned int) _temp1917 > 1u?*(( int*) _temp1917) == Cyc_Absyn_Eq_constr: 0){
_LL1924: _temp1923=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1917)->f1;
if( _temp1923 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1920;} else{ goto _LL1921;}}
else{ goto _LL1921;} _LL1921: goto _LL1922; _LL1920: is_tagged_ptr= 1; goto
_LL1918; _LL1922: goto _LL1918; _LL1918:;} goto _LL1900; _LL1904:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1925="exp_to_c: comprehension not an array type";
struct _tagged_string _temp1926; _temp1926.curr= _temp1925; _temp1926.base=
_temp1925; _temp1926.last_plus_one= _temp1925 + 42; _temp1926;})); goto _LL1900;
_LL1900:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1839->topt))->v;
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1841);{ struct Cyc_Absyn_Stmt*
s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( a, 0), _temp1843, Cyc_Absyn_var_exp(
max, 0), _temp1839, Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1927=
_new_region(); struct _RegionHandle* r=& _temp1927; _push_region( r);{ struct
Cyc_List_List* decls=({ struct Cyc_List_List* _temp1957=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1957->hd=( void*)({
struct _tuple10* _temp1958=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp1958->f1= max; _temp1958->f2= Cyc_Absyn_uint_t; _temp1958->f3=(
struct Cyc_Absyn_Exp*) _temp1841; _temp1958;}); _temp1957->tl= 0; _temp1957;});
struct Cyc_Absyn_Exp* ai; if( _temp1434 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1434); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1928=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1928->hd=( void*)({ struct _tuple10*
_temp1929=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1929->f1= a; _temp1929->f2= ptr_typ; _temp1929->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1929;}); _temp1928->tl= decls; _temp1928;}); if( is_tagged_ptr){
struct _tuple0* _temp1930= Cyc_Toc_temp_var(); goto _LL1931; _LL1931: { void*
_temp1932= Cyc_Toc_typ_to_c( old_typ); goto _LL1933; _LL1933: { struct Cyc_Absyn_Exp*
_temp1943= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(* _temp1934)(
struct _tagged_ptr3)=( struct Cyc_List_List*(*)( struct _tagged_ptr3)) Cyc_List_list;
struct _tuple4* _temp1937=({ struct _tuple4* _temp1942=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1942->f1= 0; _temp1942->f2= Cyc_Absyn_var_exp(
a, 0); _temp1942;}); struct _tuple4* _temp1938=({ struct _tuple4* _temp1941=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1941->f1= 0;
_temp1941->f2= Cyc_Absyn_var_exp( a, 0); _temp1941;}); struct _tuple4* _temp1939=({
struct _tuple4* _temp1940=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1940->f1= 0; _temp1940->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1940;}); struct _tuple4* _temp1935[ 3u]={
_temp1937, _temp1938, _temp1939}; struct _tagged_ptr3 _temp1936={ _temp1935,
_temp1935, _temp1935 + 3u}; _temp1934( _temp1936);}), 0); goto _LL1944; _LL1944:
decls=({ struct Cyc_List_List* _temp1945=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1945->hd=( void*)({ struct _tuple10*
_temp1946=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1946->f1= _temp1930; _temp1946->f2= _temp1932; _temp1946->f3=( struct Cyc_Absyn_Exp*)
_temp1943; _temp1946;}); _temp1945->tl= decls; _temp1945;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1930, 0), 0), 0);}}} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
_temp1947= decls; goto _LL1948; _LL1948: for( 0; _temp1947 != 0; _temp1947=((
struct Cyc_List_List*) _check_null( _temp1947))->tl){ struct Cyc_Absyn_Exp*
_temp1951; void* _temp1953; struct _tuple0* _temp1955; struct _tuple10 _temp1949=*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1947))->hd);
_LL1956: _temp1955= _temp1949.f1; goto _LL1954; _LL1954: _temp1953= _temp1949.f2;
goto _LL1952; _LL1952: _temp1951= _temp1949.f3; goto _LL1950; _LL1950: s= Cyc_Absyn_declare_stmt(
_temp1955, _temp1953, _temp1951, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region(& _temp1927);} goto _LL1820;}}} _LL1828:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1432->topt))->v, 1, _temp1434, _temp1847, _temp1851))->r));
goto _LL1820; _LL1830:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv,
1, _temp1434, _temp1853))->r)); goto _LL1820; _LL1832: { void* _temp1959=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1432->topt))->v; goto _LL1960; _LL1960: {
struct _tuple0* _temp1961= Cyc_Toc_temp_var(); goto _LL1962; _LL1962: { struct
Cyc_Absyn_Stmt* _temp1963= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1961, 0),
0); goto _LL1964; _LL1964: { struct Cyc_Absyn_Exp* _temp1965= Cyc_Absyn_signed_int_exp(
0, 0); goto _LL1966; _LL1966: Cyc_Toc_exp_to_c( nv, _temp1432); _temp1963= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1961, 0),
_temp1965, 0), _temp1432, 0), _temp1963, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
_temp1959); void* _temp1967= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); goto
_LL1968; _LL1968: { struct Cyc_Absyn_Exp* _temp1969= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0); goto _LL1970; _LL1970: { struct Cyc_Absyn_Exp* e1; if( _temp1434 ==
0){ e1= Cyc_Toc_malloc_exp( _temp1959, _temp1969);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp1434); Cyc_Toc_exp_to_c( nv, r); e1=
Cyc_Toc_rmalloc_exp( r, _temp1969);}{ struct Cyc_Absyn_Exp* _temp1971= Cyc_Absyn_cast_exp(
_temp1967, e1, 0); goto _LL1972; _LL1972:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1961, _temp1967,( struct Cyc_Absyn_Exp*) _temp1971,
_temp1963, 0), 0))->r)); goto _LL1820;}}}}}}}} _LL1820:;} goto _LL1283; _LL1321:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct* _temp1973=(
struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1973[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1974; _temp1974.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1974.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1436);
_temp1974;}); _temp1973;}))); goto _LL1283; _LL1323: Cyc_Toc_exp_to_c( nv,
_temp1438); goto _LL1283; _LL1325: { void* e1_typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1440->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1440);{ void* _temp1975= e1_typ; struct Cyc_Absyn_PtrInfo _temp1981; struct
Cyc_Absyn_Conref* _temp1983; struct Cyc_Absyn_Tqual _temp1985; struct Cyc_Absyn_Conref*
_temp1987; void* _temp1989; void* _temp1991; _LL1977: if(( unsigned int)
_temp1975 > 4u?*(( int*) _temp1975) == Cyc_Absyn_PointerType: 0){ _LL1982:
_temp1981=(( struct Cyc_Absyn_PointerType_struct*) _temp1975)->f1; _LL1992:
_temp1991=( void*) _temp1981.elt_typ; goto _LL1990; _LL1990: _temp1989=( void*)
_temp1981.rgn_typ; goto _LL1988; _LL1988: _temp1987= _temp1981.nullable; goto
_LL1986; _LL1986: _temp1985= _temp1981.tq; goto _LL1984; _LL1984: _temp1983=
_temp1981.bounds; goto _LL1978;} else{ goto _LL1979;} _LL1979: goto _LL1980;
_LL1978:{ void* _temp1993=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1983); struct Cyc_Absyn_Exp* _temp1999; _LL1995: if( _temp1993 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1996;} else{ goto _LL1997;} _LL1997: if((
unsigned int) _temp1993 > 1u?*(( int*) _temp1993) == Cyc_Absyn_Upper_b: 0){
_LL2000: _temp1999=(( struct Cyc_Absyn_Upper_b_struct*) _temp1993)->f1; goto
_LL1998;} else{ goto _LL1994;} _LL1996: { struct _tuple0* x= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); void* _temp2001= Cyc_Toc_typ_to_c_array(
_temp1991); goto _LL2002; _LL2002: { void* tx= Cyc_Toc_add_tagged_ptr_type(
_temp2001, _temp1985); void* ta2= Cyc_Absyn_cstar_typ( _temp2001, _temp1985);
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
struct Cyc_Absyn_Exp*) _temp1440, s1, 0), 0))->r)); goto _LL1994;}} _LL1998: if(
Cyc_Toc_is_nullable( e1_typ)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1440->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp2003=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2003->hd=(
void*) _temp1440; _temp2003->tl= 0; _temp2003;}), 0), 0), 0))->r));} else{
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, e1_typ,(
struct Cyc_Absyn_Exp*) _temp1440, s3, 0), 0))->r));}} goto _LL1994; _LL1994:;}
goto _LL1976; _LL1980:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2004="exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp2005; _temp2005.curr= _temp2004; _temp2005.base= _temp2004; _temp2005.last_plus_one=
_temp2004 + 29; _temp2005;})); goto _LL1976; _LL1976:;} goto _LL1283;} _LL1327:
Cyc_Toc_exp_to_c( nv, _temp1444); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL1283; _LL1329: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1448->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1448);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp2008; struct Cyc_Absyn_Tqual _temp2010; struct Cyc_Absyn_Conref* _temp2012;
void* _temp2014; void* _temp2016; struct Cyc_Absyn_PtrInfo _temp2006= Cyc_Toc_get_ptr_type(
e1typ); _LL2017: _temp2016=( void*) _temp2006.elt_typ; goto _LL2015; _LL2015:
_temp2014=( void*) _temp2006.rgn_typ; goto _LL2013; _LL2013: _temp2012=
_temp2006.nullable; goto _LL2011; _LL2011: _temp2010= _temp2006.tq; goto _LL2009;
_LL2009: _temp2008= _temp2006.bounds; goto _LL2007; _LL2007:{ void* _temp2018=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2008); struct
Cyc_Absyn_Exp* _temp2024; _LL2020: if(( unsigned int) _temp2018 > 1u?*(( int*)
_temp2018) == Cyc_Absyn_Upper_b: 0){ _LL2025: _temp2024=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2018)->f1; goto _LL2021;} else{ goto _LL2022;} _LL2022: if( _temp2018 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL2023;} else{ goto _LL2019;} _LL2021: if(
Cyc_Evexp_eval_const_uint_exp( _temp2024) < 1){(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp2026="exp_to_c:  StructArrow_e on pointer of size 0";
struct _tagged_string _temp2027; _temp2027.curr= _temp2026; _temp2027.base=
_temp2026; _temp2027.last_plus_one= _temp2026 + 46; _temp2027;}));}{ int
possibly_null;{ void* _temp2028=( void*)((( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2012))->v; int _temp2034;
_LL2030: if(( unsigned int) _temp2028 > 1u?*(( int*) _temp2028) == Cyc_Absyn_Eq_constr:
0){ _LL2035: _temp2034=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2028)->f1;
goto _LL2031;} else{ goto _LL2032;} _LL2032: goto _LL2033; _LL2031:
possibly_null= _temp2034; goto _LL2029; _LL2033: possibly_null= 0; goto _LL2029;
_LL2029:;} if( Cyc_Toc_is_nullable(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1448->topt))->v)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1448->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp2036=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2036->hd=(
void*) _temp1448; _temp2036->tl= 0; _temp2036;}), 0), 0), _temp1446, 0))->r));}
else{ struct _tuple0* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1448->topt))->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp1446, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1448, s3, 0), 0))->r));}} goto _LL2019;} _LL2023: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2016); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2010); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2046=({ struct Cyc_List_List*(* _temp2037)( struct
_tagged_ptr6)=( struct Cyc_List_List*(*)( struct _tagged_ptr6)) Cyc_List_list;
struct _tuple1* _temp2040=({ struct _tuple1* _temp2045=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2045->f1= 0; _temp2045->f2= Cyc_Toc_mt_tq;
_temp2045->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1448->topt))->v); _temp2045;}); struct _tuple1* _temp2041=({ struct _tuple1*
_temp2044=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2044->f1=
0; _temp2044->f2= Cyc_Toc_mt_tq; _temp2044->f3= Cyc_Absyn_uint_t; _temp2044;});
struct _tuple1* _temp2042=({ struct _tuple1* _temp2043=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2043->f1= 0; _temp2043->f2= Cyc_Toc_mt_tq;
_temp2043->f3= Cyc_Absyn_uint_t; _temp2043;}); struct _tuple1* _temp2038[ 3u]={
_temp2040, _temp2041, _temp2042}; struct _tagged_ptr6 _temp2039={ _temp2038,
_temp2038, _temp2038 + 3u}; _temp2037( _temp2039);}); goto _LL2047; _LL2047: {
void* _temp2051= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2048=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2048[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2049; _temp2049.tag= Cyc_Absyn_FnType;
_temp2049.f1=({ struct Cyc_Absyn_FnInfo _temp2050; _temp2050.tvars= 0; _temp2050.effect=
0; _temp2050.ret_typ=( void*) ta2; _temp2050.args= _temp2046; _temp2050.c_varargs=
0; _temp2050.cyc_varargs= 0; _temp2050.attributes= 0; _temp2050;}); _temp2049;});
_temp2048;}), Cyc_Toc_mt_tq); goto _LL2052; _LL2052: { struct Cyc_Absyn_Exp*
fnexp= Cyc_Absyn_cast_exp( _temp2051, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv,
0), 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_uint_exp( 0, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_fncall_exp( fnexp,({
struct Cyc_List_List*(* _temp2053)( struct _tagged_ptr7)=( struct Cyc_List_List*(*)(
struct _tagged_ptr7)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2056= _temp1448;
struct Cyc_Absyn_Exp* _temp2057= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2058= e2; struct Cyc_Absyn_Exp* _temp2054[ 3u]={ _temp2056, _temp2057,
_temp2058}; struct _tagged_ptr7 _temp2055={ _temp2054, _temp2054, _temp2054 + 3u};
_temp2053( _temp2055);}), 0), _temp1446, 0))->r));}}} else{ struct _tuple0* x=
Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2010);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( a, 0), _temp1446, 0), 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt( 0), 0); struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt* s1=
Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,(
struct Cyc_Absyn_Exp*) _temp1448, s1, 0), 0))->r));} goto _LL2019;} _LL2019:;}
if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1283;}} _LL1331: { void* _temp2059= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1452->topt))->v); goto _LL2060;
_LL2060: Cyc_Toc_exp_to_c( nv, _temp1452); Cyc_Toc_exp_to_c( nv, _temp1450);{
void* _temp2061= _temp2059; struct Cyc_List_List* _temp2069; struct Cyc_Absyn_PtrInfo
_temp2071; struct Cyc_Absyn_Conref* _temp2073; struct Cyc_Absyn_Tqual _temp2075;
struct Cyc_Absyn_Conref* _temp2077; void* _temp2079; void* _temp2081; _LL2063:
if(( unsigned int) _temp2061 > 4u?*(( int*) _temp2061) == Cyc_Absyn_TupleType: 0){
_LL2070: _temp2069=(( struct Cyc_Absyn_TupleType_struct*) _temp2061)->f1; goto
_LL2064;} else{ goto _LL2065;} _LL2065: if(( unsigned int) _temp2061 > 4u?*((
int*) _temp2061) == Cyc_Absyn_PointerType: 0){ _LL2072: _temp2071=(( struct Cyc_Absyn_PointerType_struct*)
_temp2061)->f1; _LL2082: _temp2081=( void*) _temp2071.elt_typ; goto _LL2080;
_LL2080: _temp2079=( void*) _temp2071.rgn_typ; goto _LL2078; _LL2078: _temp2077=
_temp2071.nullable; goto _LL2076; _LL2076: _temp2075= _temp2071.tq; goto _LL2074;
_LL2074: _temp2073= _temp2071.bounds; goto _LL2066;} else{ goto _LL2067;}
_LL2067: goto _LL2068; _LL2064: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1450) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1452, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL2062;} _LL2066:{ void*
_temp2083=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp2073); struct Cyc_Absyn_Exp* _temp2089; _LL2085: if(( unsigned int)
_temp2083 > 1u?*(( int*) _temp2083) == Cyc_Absyn_Upper_b: 0){ _LL2090: _temp2089=((
struct Cyc_Absyn_Upper_b_struct*) _temp2083)->f1; goto _LL2086;} else{ goto
_LL2087;} _LL2087: if( _temp2083 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2088;}
else{ goto _LL2084;} _LL2086: _temp2089= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp2089), 0);{ int possibly_null;{ void* _temp2091=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2077))->v; int
_temp2097; _LL2093: if(( unsigned int) _temp2091 > 1u?*(( int*) _temp2091) ==
Cyc_Absyn_Eq_constr: 0){ _LL2098: _temp2097=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2091)->f1; goto _LL2094;} else{ goto _LL2095;} _LL2095: goto _LL2096;
_LL2094: possibly_null= _temp2097; goto _LL2092; _LL2096: possibly_null= 0; goto
_LL2092; _LL2092:;}{ void* ta1= Cyc_Toc_typ_to_c( _temp2081); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp2075); if( Cyc_Toc_use_runtimecheck_funs){ if( possibly_null){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv, 0),({ struct Cyc_List_List*(*
_temp2099)( struct _tagged_ptr7)=( struct Cyc_List_List*(*)( struct _tagged_ptr7))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp2102= _temp1452; struct Cyc_Absyn_Exp*
_temp2103= _temp2089; struct Cyc_Absyn_Exp* _temp2104= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp2105= _temp1450; struct Cyc_Absyn_Exp*
_temp2100[ 4u]={ _temp2102, _temp2103, _temp2104, _temp2105}; struct
_tagged_ptr7 _temp2101={ _temp2100, _temp2100, _temp2100 + 4u}; _temp2099(
_temp2101);}), 0), 0), 0))->r));} else{( void*)( _temp1450->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List*(* _temp2106)( struct _tagged_ptr7)=( struct Cyc_List_List*(*)(
struct _tagged_ptr7)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2109= _temp2089;
struct Cyc_Absyn_Exp* _temp2110= Cyc_Absyn_copy_exp( _temp1450); struct Cyc_Absyn_Exp*
_temp2107[ 2u]={ _temp2109, _temp2110}; struct _tagged_ptr7 _temp2108={
_temp2107, _temp2107, _temp2107 + 2u}; _temp2106( _temp2108);}), 0))->r));}}
else{ struct _tuple0* a= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte, Cyc_Absyn_var_exp(
i, 0), _temp2089, 0); if( possibly_null){ e3= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_new_exp((
void*) e3->r, 0), 0);}{ struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt(
i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) _temp1450, s3, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*)
_temp1452, s1, 0), 0))->r));}} goto _LL2084;}} _LL2088: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2081); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2075); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2120=({ struct Cyc_List_List*(* _temp2111)( struct
_tagged_ptr6)=( struct Cyc_List_List*(*)( struct _tagged_ptr6)) Cyc_List_list;
struct _tuple1* _temp2114=({ struct _tuple1* _temp2119=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2119->f1= 0; _temp2119->f2= Cyc_Toc_mt_tq;
_temp2119->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1452->topt))->v); _temp2119;}); struct _tuple1* _temp2115=({ struct _tuple1*
_temp2118=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2118->f1=
0; _temp2118->f2= Cyc_Toc_mt_tq; _temp2118->f3= Cyc_Absyn_uint_t; _temp2118;});
struct _tuple1* _temp2116=({ struct _tuple1* _temp2117=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2117->f1= 0; _temp2117->f2= Cyc_Toc_mt_tq;
_temp2117->f3= Cyc_Absyn_uint_t; _temp2117;}); struct _tuple1* _temp2112[ 3u]={
_temp2114, _temp2115, _temp2116}; struct _tagged_ptr6 _temp2113={ _temp2112,
_temp2112, _temp2112 + 3u}; _temp2111( _temp2113);}); goto _LL2121; _LL2121: {
void* _temp2125= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2122=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2122[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2123; _temp2123.tag= Cyc_Absyn_FnType;
_temp2123.f1=({ struct Cyc_Absyn_FnInfo _temp2124; _temp2124.tvars= 0; _temp2124.effect=
0; _temp2124.ret_typ=( void*) ta2; _temp2124.args= _temp2120; _temp2124.c_varargs=
0; _temp2124.cyc_varargs= 0; _temp2124.attributes= 0; _temp2124;}); _temp2123;});
_temp2122;}), Cyc_Toc_mt_tq); goto _LL2126; _LL2126: { struct Cyc_Absyn_Exp*
fnexp= Cyc_Absyn_cast_exp( _temp2125, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv,
0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_fncall_exp(
fnexp,({ struct Cyc_List_List*(* _temp2127)( struct _tagged_ptr7)=( struct Cyc_List_List*(*)(
struct _tagged_ptr7)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2130= _temp1452;
struct Cyc_Absyn_Exp* _temp2131= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2132= _temp1450; struct Cyc_Absyn_Exp* _temp2128[ 3u]={ _temp2130,
_temp2131, _temp2132}; struct _tagged_ptr7 _temp2129={ _temp2128, _temp2128,
_temp2128 + 3u}; _temp2127( _temp2129);}), 0), 0))->r));}}} else{ struct _tuple0*
x= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2075);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1450, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1452, s1, 0), 0))->r));} goto _LL2084;} _LL2084:;}
goto _LL2062; _LL2068:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2133="exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp2134; _temp2134.curr= _temp2133; _temp2134.base=
_temp2133; _temp2134.last_plus_one= _temp2133 + 49; _temp2134;})); goto _LL2062;
_LL2062:;} goto _LL1283;} _LL1333: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1454))->r));} else{ struct Cyc_List_List*
_temp2135=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1454); goto
_LL2136; _LL2136: { struct _tagged_string* _temp2137= Cyc_Toc_add_tuple_type(
_temp2135); goto _LL2138; _LL2138: { struct Cyc_List_List* dles= 0;{ int i= 1;
for( 0; _temp1454 != 0;( _temp1454=(( struct Cyc_List_List*) _check_null(
_temp1454))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1454))->hd); dles=({ struct Cyc_List_List*
_temp2139=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2139->hd=( void*)({ struct _tuple4* _temp2140=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2140->f1= 0; _temp2140->f2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1454))->hd; _temp2140;}); _temp2139->tl=
dles; _temp2139;});}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, dles, 0))->r));}}} goto _LL1283; _LL1335:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ unsigned char* _temp2141="compoundlit"; struct _tagged_string
_temp2142; _temp2142.curr= _temp2141; _temp2142.base= _temp2141; _temp2142.last_plus_one=
_temp2141 + 12; _temp2142;})); goto _LL1283; _LL1337:( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1460, 0))->r));{ struct Cyc_List_List*
_temp2143= _temp1460; goto _LL2144; _LL2144: for( 0; _temp2143 != 0; _temp2143=((
struct Cyc_List_List*) _check_null( _temp2143))->tl){ struct _tuple4 _temp2147;
struct Cyc_Absyn_Exp* _temp2148; struct Cyc_List_List* _temp2150; struct _tuple4*
_temp2145=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp2143))->hd;
_temp2147=* _temp2145; _LL2151: _temp2150= _temp2147.f1; goto _LL2149; _LL2149:
_temp2148= _temp2147.f2; goto _LL2146; _LL2146: Cyc_Toc_exp_to_c( nv, _temp2148);}}
goto _LL1283; _LL1339: { unsigned int _temp2152= Cyc_Evexp_eval_const_uint_exp(
_temp1464); goto _LL2153; _LL2153: Cyc_Toc_exp_to_c( nv, _temp1462);{ struct Cyc_List_List*
es= 0; if( ! Cyc_Toc_is_zero( _temp1462)){ unsigned int i= 0; for( 0; i <
_temp2152; i ++){ es=({ struct Cyc_List_List* _temp2154=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2154->hd=( void*)({ struct
_tuple4* _temp2155=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp2155->f1= 0; _temp2155->f2= _temp1462; _temp2155;}); _temp2154->tl= es;
_temp2154;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
es, 0))->r)); goto _LL1283;}} _LL1341: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1470, _temp1474))->r));}
else{ if( _temp1468 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2156="Struct_e: missing structdecl pointer"; struct
_tagged_string _temp2157; _temp2157.curr= _temp2156; _temp2157.base= _temp2156;
_temp2157.last_plus_one= _temp2156 + 37; _temp2157;}));}{ struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*) _check_null( _temp1468); struct
_RegionHandle _temp2158= _new_region(); struct _RegionHandle* rgn=& _temp2158;
_push_region( rgn);{ struct Cyc_List_List* _temp2159=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1470,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd2->fields))->v); goto _LL2160; _LL2160: { struct Cyc_List_List* _temp2161= 0;
goto _LL2162; _LL2162:{ struct Cyc_List_List* _temp2163=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; goto _LL2164; _LL2164: for(
0; _temp2163 != 0; _temp2163=(( struct Cyc_List_List*) _check_null( _temp2163))->tl){
struct Cyc_List_List* _temp2165= _temp2159; goto _LL2166; _LL2166: for( 0;
_temp2165 != 0; _temp2165=(( struct Cyc_List_List*) _check_null( _temp2165))->tl){
if((*(( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2165))->hd)).f1
==( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2163))->hd){ struct _tuple12 _temp2169; struct Cyc_Absyn_Exp* _temp2170;
struct Cyc_Absyn_Structfield* _temp2172; struct _tuple12* _temp2167=( struct
_tuple12*)(( struct Cyc_List_List*) _check_null( _temp2165))->hd; _temp2169=*
_temp2167; _LL2173: _temp2172= _temp2169.f1; goto _LL2171; _LL2171: _temp2170=
_temp2169.f2; goto _LL2168; _LL2168: { void* _temp2174=( void*) _temp2172->type;
goto _LL2175; _LL2175: Cyc_Toc_exp_to_c( nv, _temp2170); if( Cyc_Toc_is_void_star(
_temp2174)){( void*)( _temp2170->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp2170->r, 0), 0))->r));} _temp2161=({ struct Cyc_List_List*
_temp2176=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2176->hd=( void*)({ struct _tuple4* _temp2177=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2177->f1= 0; _temp2177->f2= _temp2170; _temp2177;});
_temp2176->tl= _temp2161; _temp2176;}); break;}}}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp2161), 0))->r));}}; _pop_region(& _temp2158);}} goto
_LL1283; _LL1343: { struct Cyc_List_List* fs;{ void* _temp2178= Cyc_Tcutil_compress(
_temp1478); struct Cyc_List_List* _temp2184; _LL2180: if(( unsigned int)
_temp2178 > 4u?*(( int*) _temp2178) == Cyc_Absyn_AnonStructType: 0){ _LL2185:
_temp2184=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2178)->f1; goto
_LL2181;} else{ goto _LL2182;} _LL2182: goto _LL2183; _LL2181: fs= _temp2184;
goto _LL2179; _LL2183: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp2186= Cyc_Absynpp_typ2string(
_temp1478); xprintf("anon struct has type %.*s", _temp2186.last_plus_one -
_temp2186.curr, _temp2186.curr);})); goto _LL2179; _LL2179:;}{ struct
_RegionHandle _temp2187= _new_region(); struct _RegionHandle* rgn=& _temp2187;
_push_region( rgn);{ struct Cyc_List_List* _temp2188=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1476, fs); goto _LL2189; _LL2189: for( 0; _temp2188 != 0; _temp2188=((
struct Cyc_List_List*) _check_null( _temp2188))->tl){ struct _tuple12 _temp2192;
struct Cyc_Absyn_Exp* _temp2193; struct Cyc_Absyn_Structfield* _temp2195; struct
_tuple12* _temp2190=( struct _tuple12*)(( struct Cyc_List_List*) _check_null(
_temp2188))->hd; _temp2192=* _temp2190; _LL2196: _temp2195= _temp2192.f1; goto
_LL2194; _LL2194: _temp2193= _temp2192.f2; goto _LL2191; _LL2191: { void*
_temp2197=( void*) _temp2195->type; goto _LL2198; _LL2198: Cyc_Toc_exp_to_c( nv,
_temp2193); if( Cyc_Toc_is_void_star( _temp2197)){( void*)( _temp2193->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp2193->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1476, 0))->r));}; _pop_region(& _temp2187);} goto _LL1283;} _LL1345: {
struct _tuple0* qv= _temp1480->name; if( _temp1482->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp2199= _temp1482->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1482->fields))->v;
goto _LL2200; _LL2200: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2199))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp2199))->hd)->typs
== 0){ tag_count ++;} _temp2199=(( struct Cyc_List_List*) _check_null( _temp2199))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1283;} _LL1347: { struct _tuple0* _temp2201= _temp1490->name; goto
_LL2202; _LL2202: { struct Cyc_List_List* _temp2203= _temp1490->typs; goto
_LL2204; _LL2204: { struct _tuple0* _temp2205= Cyc_Toc_temp_var(); goto _LL2206;
_LL2206: { struct Cyc_Absyn_Exp* _temp2207= Cyc_Absyn_var_exp( _temp2205, 0);
goto _LL2208; _LL2208: { void* _temp2211= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2201,({ unsigned char* _temp2209="_struct"; struct _tagged_string _temp2210;
_temp2210.curr= _temp2209; _temp2210.base= _temp2209; _temp2210.last_plus_one=
_temp2209 + 8; _temp2210;}))); goto _LL2212; _LL2212: if( nv->toplevel){ struct
Cyc_Absyn_Exp* tag_exp; if( _temp1492->is_xtunion){ tag_exp= Cyc_Absyn_var_exp(
_temp2201, 0);} else{ int tag_count= 0; struct Cyc_List_List* _temp2213=
_temp1492->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1492->fields))->v; goto _LL2214; _LL2214: while( Cyc_Absyn_qvar_cmp(
_temp2201,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2213))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2213))->hd)->typs != 0){ tag_count ++;}
_temp2213=(( struct Cyc_List_List*) _check_null( _temp2213))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2215= 0; goto _LL2216;
_LL2216: for( 0; _temp1494 != 0;( _temp1494=(( struct Cyc_List_List*)
_check_null( _temp1494))->tl, _temp2203=(( struct Cyc_List_List*) _check_null(
_temp2203))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1494))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2203))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2215=({ struct Cyc_List_List* _temp2217=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2217[ 0]=({ struct Cyc_List_List
_temp2218; _temp2218.hd=( void*)({ struct _tuple4* _temp2219=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2219[ 0]=({ struct _tuple4
_temp2220; _temp2220.f1= 0; _temp2220.f2= cur_e; _temp2220;}); _temp2219;});
_temp2218.tl= _temp2215; _temp2218;}); _temp2217;});} _temp2215=({ struct Cyc_List_List*
_temp2221=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2221[ 0]=({ struct Cyc_List_List _temp2222; _temp2222.hd=( void*)({ struct
_tuple4* _temp2223=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2223[ 0]=({ struct _tuple4 _temp2224; _temp2224.f1= 0; _temp2224.f2=
tag_exp; _temp2224;}); _temp2223;}); _temp2222.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2215); _temp2222;}); _temp2221;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2225=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2225->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2201,({ unsigned char*
_temp2226="_struct"; struct _tagged_string _temp2227; _temp2227.curr= _temp2226;
_temp2227.base= _temp2226; _temp2227.last_plus_one= _temp2226 + 8; _temp2227;}));
_temp2225;}), _temp2215, 0))->r));}} else{ struct Cyc_List_List* _temp2229=({
struct Cyc_List_List* _temp2228=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2228->hd=( void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp2207, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp2201, 0), 0); _temp2228->tl=
0; _temp2228;}); goto _LL2230; _LL2230:{ int i= 1; for( 0; _temp1494 != 0;(((
_temp1494=(( struct Cyc_List_List*) _check_null( _temp1494))->tl, i ++)),
_temp2203=(( struct Cyc_List_List*) _check_null( _temp2203))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1494))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2203))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp2231= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp2207, Cyc_Toc_fieldname(
i), 0), cur_e, 0); goto _LL2232; _LL2232: _temp2229=({ struct Cyc_List_List*
_temp2233=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2233->hd=( void*) _temp2231; _temp2233->tl= _temp2229; _temp2233;});}}}{
struct Cyc_Absyn_Stmt* _temp2234= Cyc_Absyn_exp_stmt( _temp2207, 0); goto
_LL2235; _LL2235: { struct Cyc_Absyn_Stmt* _temp2237= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp2236=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2236->hd=( void*) _temp2234; _temp2236->tl= _temp2229; _temp2236;})), 0);
goto _LL2238; _LL2238:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp2205, _temp2211, 0, _temp2237, 0), 0))->r));}}} goto _LL1283;}}}}} _LL1349:
goto _LL1283; _LL1351: { void* t_c= Cyc_Toc_typ_to_c( _temp1506); if( _temp1508
!= 0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1508);
Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp(
rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} else{( void*)( e->r=( void*)((
void*)( Cyc_Toc_malloc_exp( _temp1506, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
goto _LL1283;} _LL1353: Cyc_Toc_stmt_to_c( nv, _temp1510); goto _LL1283; _LL1355:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char*
_temp2239="UnresolvedMem"; struct _tagged_string _temp2240; _temp2240.curr=
_temp2239; _temp2240.base= _temp2239; _temp2240.last_plus_one= _temp2239 + 14;
_temp2240;})); goto _LL1283; _LL1357:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2241="codegen"; struct _tagged_string _temp2242; _temp2242.curr=
_temp2241; _temp2242.base= _temp2241; _temp2242.last_plus_one= _temp2241 + 8;
_temp2242;})); goto _LL1283; _LL1359:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2243="fill"; struct _tagged_string _temp2244; _temp2244.curr=
_temp2243; _temp2244.base= _temp2243; _temp2244.last_plus_one= _temp2243 + 5;
_temp2244;})); goto _LL1283; _LL1283:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_string*
succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple14{ struct _tuple0* f1; void* f2;
} ; struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct
_RegionHandle* rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path,
struct Cyc_Absyn_Pat* p, struct _tagged_string* succ_lab, struct _tagged_string*
fail_lab, struct Cyc_List_List* decls){ struct Cyc_Absyn_Stmt* s;{ void*
_temp2245=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp2281; struct Cyc_Absyn_Vardecl
_temp2283; struct Cyc_List_List* _temp2284; struct Cyc_Core_Opt* _temp2286;
struct Cyc_Absyn_Exp* _temp2288; void* _temp2290; struct Cyc_Absyn_Tqual
_temp2292; struct _tuple0* _temp2294; void* _temp2296; struct Cyc_Absyn_Vardecl*
_temp2298; int _temp2300; void* _temp2302; unsigned char _temp2304; struct
_tagged_string _temp2306; struct Cyc_Absyn_Enumfield* _temp2308; struct Cyc_Absyn_Enumdecl*
_temp2310; struct Cyc_List_List* _temp2312; struct Cyc_List_List* _temp2314;
struct Cyc_Absyn_Tunionfield* _temp2316; struct Cyc_Absyn_Tuniondecl* _temp2318;
struct Cyc_Absyn_Pat* _temp2321; struct Cyc_Absyn_Pat _temp2323; struct Cyc_Position_Segment*
_temp2324; struct Cyc_Core_Opt* _temp2326; void* _temp2328; struct Cyc_List_List*
_temp2330; struct Cyc_List_List* _temp2332; struct Cyc_Absyn_Tunionfield*
_temp2334; struct Cyc_Absyn_Tuniondecl* _temp2336; struct Cyc_List_List*
_temp2338; struct Cyc_List_List* _temp2340; struct Cyc_Absyn_Tunionfield*
_temp2342; struct Cyc_Absyn_Tuniondecl* _temp2344; struct Cyc_List_List*
_temp2346; struct Cyc_List_List* _temp2348; struct Cyc_List_List* _temp2350;
struct Cyc_Core_Opt* _temp2352; struct Cyc_Absyn_Structdecl* _temp2354; struct
Cyc_Absyn_Pat* _temp2356; struct _tuple0* _temp2358; struct Cyc_List_List*
_temp2360; struct Cyc_List_List* _temp2362; struct _tuple0* _temp2364; struct
Cyc_List_List* _temp2366; struct Cyc_List_List* _temp2368; struct _tuple0*
_temp2370; _LL2247: if(( unsigned int) _temp2245 > 2u?*(( int*) _temp2245) ==
Cyc_Absyn_Var_p: 0){ _LL2282: _temp2281=(( struct Cyc_Absyn_Var_p_struct*)
_temp2245)->f1; _temp2283=* _temp2281; _LL2297: _temp2296=( void*) _temp2283.sc;
goto _LL2295; _LL2295: _temp2294= _temp2283.name; goto _LL2293; _LL2293:
_temp2292= _temp2283.tq; goto _LL2291; _LL2291: _temp2290=( void*) _temp2283.type;
goto _LL2289; _LL2289: _temp2288= _temp2283.initializer; goto _LL2287; _LL2287:
_temp2286= _temp2283.rgn; goto _LL2285; _LL2285: _temp2284= _temp2283.attributes;
goto _LL2248;} else{ goto _LL2249;} _LL2249: if( _temp2245 ==( void*) Cyc_Absyn_Wild_p){
goto _LL2250;} else{ goto _LL2251;} _LL2251: if(( unsigned int) _temp2245 > 2u?*((
int*) _temp2245) == Cyc_Absyn_Reference_p: 0){ _LL2299: _temp2298=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2245)->f1; goto _LL2252;} else{ goto _LL2253;} _LL2253: if( _temp2245 ==(
void*) Cyc_Absyn_Null_p){ goto _LL2254;} else{ goto _LL2255;} _LL2255: if((
unsigned int) _temp2245 > 2u?*(( int*) _temp2245) == Cyc_Absyn_Int_p: 0){
_LL2303: _temp2302=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp2245)->f1;
goto _LL2301; _LL2301: _temp2300=(( struct Cyc_Absyn_Int_p_struct*) _temp2245)->f2;
goto _LL2256;} else{ goto _LL2257;} _LL2257: if(( unsigned int) _temp2245 > 2u?*((
int*) _temp2245) == Cyc_Absyn_Char_p: 0){ _LL2305: _temp2304=(( struct Cyc_Absyn_Char_p_struct*)
_temp2245)->f1; goto _LL2258;} else{ goto _LL2259;} _LL2259: if(( unsigned int)
_temp2245 > 2u?*(( int*) _temp2245) == Cyc_Absyn_Float_p: 0){ _LL2307: _temp2306=((
struct Cyc_Absyn_Float_p_struct*) _temp2245)->f1; goto _LL2260;} else{ goto
_LL2261;} _LL2261: if(( unsigned int) _temp2245 > 2u?*(( int*) _temp2245) == Cyc_Absyn_Enum_p:
0){ _LL2311: _temp2310=(( struct Cyc_Absyn_Enum_p_struct*) _temp2245)->f1; goto
_LL2309; _LL2309: _temp2308=(( struct Cyc_Absyn_Enum_p_struct*) _temp2245)->f2;
goto _LL2262;} else{ goto _LL2263;} _LL2263: if(( unsigned int) _temp2245 > 2u?*((
int*) _temp2245) == Cyc_Absyn_Tunion_p: 0){ _LL2319: _temp2318=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2245)->f1; goto _LL2317; _LL2317: _temp2316=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2245)->f2; goto _LL2315; _LL2315: _temp2314=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2245)->f3; goto _LL2313; _LL2313: _temp2312=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2245)->f4; if( _temp2312 == 0){ goto _LL2264;} else{ goto _LL2265;}} else{
goto _LL2265;} _LL2265: if(( unsigned int) _temp2245 > 2u?*(( int*) _temp2245)
== Cyc_Absyn_Pointer_p: 0){ _LL2322: _temp2321=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2245)->f1; _temp2323=* _temp2321; _LL2329: _temp2328=( void*) _temp2323.r;
if(( unsigned int) _temp2328 > 2u?*(( int*) _temp2328) == Cyc_Absyn_Tunion_p: 0){
_LL2337: _temp2336=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2328)->f1; goto
_LL2335; _LL2335: _temp2334=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2328)->f2;
goto _LL2333; _LL2333: _temp2332=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2328)->f3;
goto _LL2331; _LL2331: _temp2330=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2328)->f4;
goto _LL2327;} else{ goto _LL2267;} _LL2327: _temp2326= _temp2323.topt; goto
_LL2325; _LL2325: _temp2324= _temp2323.loc; goto _LL2320;} else{ goto _LL2267;}
_LL2320: if( _temp2330 != 0){ goto _LL2266;} else{ goto _LL2267;} _LL2267: if((
unsigned int) _temp2245 > 2u?*(( int*) _temp2245) == Cyc_Absyn_Tunion_p: 0){
_LL2345: _temp2344=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2245)->f1; goto
_LL2343; _LL2343: _temp2342=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2245)->f2;
goto _LL2341; _LL2341: _temp2340=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2245)->f3;
goto _LL2339; _LL2339: _temp2338=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2245)->f4;
goto _LL2268;} else{ goto _LL2269;} _LL2269: if(( unsigned int) _temp2245 > 2u?*((
int*) _temp2245) == Cyc_Absyn_Tuple_p: 0){ _LL2347: _temp2346=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp2245)->f1; goto _LL2270;} else{ goto _LL2271;} _LL2271: if(( unsigned int)
_temp2245 > 2u?*(( int*) _temp2245) == Cyc_Absyn_Struct_p: 0){ _LL2355:
_temp2354=(( struct Cyc_Absyn_Struct_p_struct*) _temp2245)->f1; goto _LL2353;
_LL2353: _temp2352=(( struct Cyc_Absyn_Struct_p_struct*) _temp2245)->f2; goto
_LL2351; _LL2351: _temp2350=(( struct Cyc_Absyn_Struct_p_struct*) _temp2245)->f3;
goto _LL2349; _LL2349: _temp2348=(( struct Cyc_Absyn_Struct_p_struct*) _temp2245)->f4;
goto _LL2272;} else{ goto _LL2273;} _LL2273: if(( unsigned int) _temp2245 > 2u?*((
int*) _temp2245) == Cyc_Absyn_Pointer_p: 0){ _LL2357: _temp2356=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2245)->f1; goto _LL2274;} else{ goto _LL2275;} _LL2275: if(( unsigned int)
_temp2245 > 2u?*(( int*) _temp2245) == Cyc_Absyn_UnknownId_p: 0){ _LL2359:
_temp2358=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2245)->f1; goto _LL2276;}
else{ goto _LL2277;} _LL2277: if(( unsigned int) _temp2245 > 2u?*(( int*)
_temp2245) == Cyc_Absyn_UnknownCall_p: 0){ _LL2365: _temp2364=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2245)->f1; goto _LL2363; _LL2363: _temp2362=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2245)->f2; goto _LL2361; _LL2361: _temp2360=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2245)->f3; goto _LL2278;} else{ goto _LL2279;} _LL2279: if(( unsigned int)
_temp2245 > 2u?*(( int*) _temp2245) == Cyc_Absyn_UnknownFields_p: 0){ _LL2371:
_temp2370=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2245)->f1; goto
_LL2369; _LL2369: _temp2368=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2245)->f2; goto _LL2367; _LL2367: _temp2366=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2245)->f3; goto _LL2280;} else{ goto _LL2246;} _LL2248: nv= Cyc_Toc_add_varmap(
nv, _temp2294, r); goto _LL2250; _LL2250: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL2246; _LL2252: { struct _tuple0* _temp2372= Cyc_Toc_temp_var(); goto
_LL2373; _LL2373: decls=({ struct Cyc_List_List* _temp2374=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2374->hd=( void*)({
struct _tuple14* _temp2375=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2375->f1= _temp2372; _temp2375->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2375;}); _temp2374->tl= decls;
_temp2374;}); nv= Cyc_Toc_add_varmap( nv, _temp2298->name, Cyc_Absyn_var_exp(
_temp2372, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2372, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2246;} _LL2254: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2246; _LL2256: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2302, _temp2300, 0), succ_lab, fail_lab); goto _LL2246;
_LL2258: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2304, 0),
succ_lab, fail_lab); goto _LL2246; _LL2260: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2306, 0), succ_lab, fail_lab); goto _LL2246; _LL2262: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2376=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2376[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2377; _temp2377.tag= Cyc_Absyn_Enum_e;
_temp2377.f1= _temp2308->name; _temp2377.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2310; _temp2377.f3=( struct Cyc_Absyn_Enumfield*) _temp2308; _temp2377;});
_temp2376;}), 0), succ_lab, fail_lab); goto _LL2246; _LL2264: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2316->name, 0); if( ! _temp2318->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2246;} _LL2266: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2330);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2334->name,({
unsigned char* _temp2412="_struct"; struct _tagged_string _temp2413; _temp2413.curr=
_temp2412; _temp2413.base= _temp2412; _temp2413.last_plus_one= _temp2412 + 8;
_temp2413;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct Cyc_List_List*
_temp2378=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2334->typs); goto _LL2379; _LL2379: { struct Cyc_List_List*
_temp2380=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2330); goto _LL2381; _LL2381: for( 0; _temp2380 !=
0;((( _temp2380=(( struct Cyc_List_List*) _check_null( _temp2380))->tl,
_temp2378=(( struct Cyc_List_List*) _check_null( _temp2378))->tl)), cnt --)){
struct Cyc_Absyn_Pat* _temp2382=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2380))->hd; goto _LL2383; _LL2383: { void* _temp2384=(*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2378))->hd)).f2;
goto _LL2385; _LL2385: { struct _tuple0* _temp2386= Cyc_Toc_temp_var(); goto
_LL2387; _LL2387: { void* _temp2388=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp2382->topt))->v; goto _LL2389; _LL2389: { void* _temp2390= Cyc_Toc_typ_to_c(
_temp2388); goto _LL2391; _LL2391: { struct _tagged_string* _temp2392= Cyc_Toc_fresh_label();
goto _LL2393; _LL2393: { struct Cyc_Absyn_Exp* _temp2395= Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp2394=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2394[ 0]= xprintf("f%d", cnt);
_temp2394;}), 0); goto _LL2396; _LL2396: if( Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c(
_temp2384))){ _temp2395= Cyc_Absyn_cast_exp( _temp2390, _temp2395, 0);} decls=({
struct Cyc_List_List* _temp2397=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2397->hd=( void*)({ struct _tuple14*
_temp2398=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2398->f1= _temp2386; _temp2398->f2= _temp2390; _temp2398;}); _temp2397->tl=
decls; _temp2397;});{ struct _tuple13 _temp2399= Cyc_Toc_xlate_pat( nv, rgn,
_temp2388, Cyc_Absyn_var_exp( _temp2386, 0), _temp2395, _temp2382, succ_lab,
fail_lab, decls); goto _LL2400; _LL2400: nv= _temp2399.f1; decls= _temp2399.f2;{
struct Cyc_Absyn_Stmt* _temp2401= _temp2399.f3; goto _LL2402; _LL2402: { struct
Cyc_Absyn_Stmt* _temp2403= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2386,
0), _temp2395, 0); goto _LL2404; _LL2404: { struct Cyc_Absyn_Stmt* _temp2405=
Cyc_Absyn_seq_stmt( _temp2403, _temp2401, 0); goto _LL2406; _LL2406: ss=({
struct Cyc_List_List* _temp2407=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2407->hd=( void*) Cyc_Absyn_label_stmt( _temp2392,
_temp2405, 0); _temp2407->tl= ss; _temp2407;}); succ_lab= _temp2392;}}}}}}}}}}}{
struct Cyc_Absyn_Exp* test_exp; if( _temp2336->is_xtunion){ struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0); struct Cyc_Absyn_Exp*
e= Cyc_Absyn_var_exp( _temp2334->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0);
s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp2408=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2336->fields))->v; goto _LL2409; _LL2409: for( 0; _temp2408 !=
0; _temp2408=(( struct Cyc_List_List*) _check_null( _temp2408))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2410=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2408))->hd; goto _LL2411; _LL2411: if( _temp2410->typs == 0){ max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r,
0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp,
0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2334->name, 0); test_exp=
max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2246;}}} _LL2268: _temp2346=
_temp2338; goto _LL2270; _LL2270: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2346);{ struct Cyc_List_List*
_temp2414=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2346); goto _LL2415; _LL2415: for( 0; _temp2414 !=
0;( _temp2414=(( struct Cyc_List_List*) _check_null( _temp2414))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2416=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2414))->hd; goto _LL2417; _LL2417: { struct _tuple0* _temp2418=
Cyc_Toc_temp_var(); goto _LL2419; _LL2419: { void* _temp2420=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2416->topt))->v; goto _LL2421; _LL2421: {
struct _tagged_string* _temp2422= Cyc_Toc_fresh_label(); goto _LL2423; _LL2423:
decls=({ struct Cyc_List_List* _temp2424=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2424->hd=( void*)({ struct _tuple14*
_temp2425=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2425->f1= _temp2418; _temp2425->f2= Cyc_Toc_typ_to_c( _temp2420); _temp2425;});
_temp2424->tl= decls; _temp2424;});{ struct _tuple13 _temp2426= Cyc_Toc_xlate_pat(
nv, rgn, _temp2420, Cyc_Absyn_var_exp( _temp2418, 0), Cyc_Absyn_structmember_exp(
path, Cyc_Toc_fieldname( cnt), 0), _temp2416, succ_lab, fail_lab, decls); goto
_LL2427; _LL2427: nv= _temp2426.f1; decls= _temp2426.f2;{ struct Cyc_Absyn_Stmt*
_temp2428= _temp2426.f3; goto _LL2429; _LL2429: { struct Cyc_Absyn_Stmt*
_temp2430= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2418, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2428, 0); goto _LL2431; _LL2431: ss=({ struct Cyc_List_List* _temp2432=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2432->hd=(
void*) Cyc_Absyn_label_stmt( _temp2422, _temp2430, 0); _temp2432->tl= ss;
_temp2432;}); succ_lab= _temp2422;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2246;} _LL2272: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2433=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2348); goto _LL2434; _LL2434: for( 0; _temp2433 !=
0; _temp2433=(( struct Cyc_List_List*) _check_null( _temp2433))->tl){ struct
_tuple15* _temp2435=( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2433))->hd; goto _LL2436; _LL2436: { struct Cyc_Absyn_Pat* _temp2437=(*
_temp2435).f2; goto _LL2438; _LL2438: { struct _tagged_string* f;{ void*
_temp2439=( void*)(( struct Cyc_List_List*) _check_null((* _temp2435).f1))->hd;
struct _tagged_string* _temp2445; _LL2441: if(*(( int*) _temp2439) == Cyc_Absyn_FieldName){
_LL2446: _temp2445=(( struct Cyc_Absyn_FieldName_struct*) _temp2439)->f1; goto
_LL2442;} else{ goto _LL2443;} _LL2443: goto _LL2444; _LL2442: f= _temp2445;
goto _LL2440; _LL2444:( void) _throw(( void*) Cyc_Toc_Match_error); _LL2440:;}{
struct _tuple0* _temp2447= Cyc_Toc_temp_var(); goto _LL2448; _LL2448: { void*
_temp2449=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2437->topt))->v;
goto _LL2450; _LL2450: { void* _temp2451= Cyc_Toc_typ_to_c( _temp2449); goto
_LL2452; _LL2452: { struct _tagged_string* _temp2453= Cyc_Toc_fresh_label();
goto _LL2454; _LL2454: decls=({ struct Cyc_List_List* _temp2455=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2455->hd=( void*)({
struct _tuple14* _temp2456=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2456->f1= _temp2447; _temp2456->f2= _temp2451; _temp2456;});
_temp2455->tl= decls; _temp2455;});{ struct _tuple13 _temp2457= Cyc_Toc_xlate_pat(
nv, rgn, _temp2449, Cyc_Absyn_var_exp( _temp2447, 0), Cyc_Absyn_structmember_exp(
path, f, 0), _temp2437, succ_lab, fail_lab, decls); goto _LL2458; _LL2458: nv=
_temp2457.f1; decls= _temp2457.f2;{ struct Cyc_Absyn_Exp* _temp2459= Cyc_Absyn_structmember_exp(
r, f, 0); goto _LL2460; _LL2460: if( Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2354->fields))->v, f)))->type)){ _temp2459= Cyc_Absyn_cast_exp(
_temp2451, _temp2459, 0);}{ struct Cyc_Absyn_Stmt* _temp2461= _temp2457.f3; goto
_LL2462; _LL2462: { struct Cyc_Absyn_Stmt* _temp2463= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2447, 0), _temp2459, 0), _temp2461, 0); goto _LL2464;
_LL2464: ss=({ struct Cyc_List_List* _temp2465=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2465->hd=( void*) Cyc_Absyn_label_stmt(
_temp2453, _temp2463, 0); _temp2465->tl= ss; _temp2465;}); succ_lab= _temp2453;}}}}}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2246;} _LL2274: { struct _tuple0*
_temp2466= Cyc_Toc_temp_var(); goto _LL2467; _LL2467: { void* _temp2468=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2356->topt))->v; goto _LL2469; _LL2469:
decls=({ struct Cyc_List_List* _temp2470=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2470->hd=( void*)({ struct _tuple14*
_temp2471=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2471->f1= _temp2466; _temp2471->f2= Cyc_Toc_typ_to_c( _temp2468); _temp2471;});
_temp2470->tl= decls; _temp2470;});{ struct _tuple13 _temp2472= Cyc_Toc_xlate_pat(
nv, rgn, _temp2468, Cyc_Absyn_var_exp( _temp2466, 0), Cyc_Absyn_deref_exp( path,
0), _temp2356, succ_lab, fail_lab, decls); goto _LL2473; _LL2473: nv= _temp2472.f1;
decls= _temp2472.f2;{ struct Cyc_Absyn_Stmt* _temp2474= _temp2472.f3; goto
_LL2475; _LL2475: { struct Cyc_Absyn_Stmt* _temp2476= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2466, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2474, 0);
goto _LL2477; _LL2477: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2476, 0);} else{ s= _temp2476;} goto _LL2246;}}}}} _LL2276: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2478="unknownid"; struct _tagged_string _temp2479; _temp2479.curr=
_temp2478; _temp2479.base= _temp2478; _temp2479.last_plus_one= _temp2478 + 10;
_temp2479;})); goto _LL2246; _LL2278: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2480="unknowncall";
struct _tagged_string _temp2481; _temp2481.curr= _temp2480; _temp2481.base=
_temp2480; _temp2481.last_plus_one= _temp2480 + 12; _temp2481;})); goto _LL2246;
_LL2280: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2482="unknownfields"; struct _tagged_string _temp2483;
_temp2483.curr= _temp2482; _temp2483.base= _temp2482; _temp2483.last_plus_one=
_temp2482 + 14; _temp2483;})); goto _LL2246; _LL2246:;} return({ struct _tuple13
_temp2484; _temp2484.f1= nv; _temp2484.f2= decls; _temp2484.f3= s; _temp2484;});}
struct _tuple16{ struct _tagged_string* f1; struct _tagged_string* f2; struct
Cyc_Absyn_Switch_clause* f3; } ; static struct _tuple16* Cyc_Toc_gen_label(
struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause* sc){ return({ struct
_tuple16* _temp2485=( struct _tuple16*) _region_malloc( r, sizeof( struct
_tuple16)); _temp2485->f1= Cyc_Toc_fresh_label(); _temp2485->f2= Cyc_Toc_fresh_label();
_temp2485->f3= sc; _temp2485;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp2486=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2487; _LL2487: { int leave_as_switch;{ void*
_temp2488= Cyc_Tcutil_compress( _temp2486); void* _temp2494; void* _temp2496;
_LL2490: if(( unsigned int) _temp2488 > 4u?*(( int*) _temp2488) == Cyc_Absyn_IntType:
0){ _LL2497: _temp2496=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2488)->f1;
goto _LL2495; _LL2495: _temp2494=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2488)->f2; goto _LL2491;} else{ goto _LL2492;} _LL2492: goto _LL2493;
_LL2491: leave_as_switch= 1; goto _LL2489; _LL2493: leave_as_switch= 0; goto
_LL2489; _LL2489:;}{ struct Cyc_List_List* _temp2498= scs; goto _LL2499; _LL2499:
for( 0; _temp2498 != 0; _temp2498=(( struct Cyc_List_List*) _check_null(
_temp2498))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2498))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2498))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2500= scs; goto
_LL2501; _LL2501: for( 0; _temp2500 != 0; _temp2500=(( struct Cyc_List_List*)
_check_null( _temp2500))->tl){ struct Cyc_Absyn_Stmt* _temp2502=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2500))->hd)->body; goto _LL2503;
_LL2503:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2500))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2502, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2502);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct
_tagged_string* end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp2504=
_new_region(); struct _RegionHandle* rgn=& _temp2504; _push_region( rgn);{
struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2505= lscs; goto
_LL2506; _LL2506: for( 0; _temp2505 != 0; _temp2505=(( struct Cyc_List_List*)
_check_null( _temp2505))->tl){ struct Cyc_Absyn_Switch_clause* sc=(*(( struct
_tuple16*)(( struct Cyc_List_List*) _check_null( _temp2505))->hd)).f3; struct
_tagged_string* fail_lab=(( struct Cyc_List_List*) _check_null( _temp2505))->tl
== 0? end_l:(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp2505))->tl))->hd)).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2505))->hd)).f2; if( sc->where_clause == 0){ struct _tuple13
_temp2507= Cyc_Toc_xlate_pat( nv, rgn, _temp2486, r, path, sc->pattern, succ_lab,
fail_lab, decls); goto _LL2508; _LL2508: nvs=({ struct Cyc_List_List* _temp2509=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2509->hd=( void*) _temp2507.f1; _temp2509->tl= nvs; _temp2509;}); decls=
_temp2507.f2; test_stmts=({ struct Cyc_List_List* _temp2510=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2510->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2505))->hd)).f1,
_temp2507.f3, 0); _temp2510->tl= test_stmts; _temp2510;});} else{ struct Cyc_Absyn_Exp*
_temp2511=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); goto _LL2512;
_LL2512: { struct _tagged_string* _temp2513= Cyc_Toc_fresh_label(); goto _LL2514;
_LL2514: { struct _tuple13 _temp2515= Cyc_Toc_xlate_pat( nv, rgn, _temp2486, r,
path, sc->pattern, _temp2513, fail_lab, decls); goto _LL2516; _LL2516: Cyc_Toc_exp_to_c(
_temp2515.f1, _temp2511); nvs=({ struct Cyc_List_List* _temp2517=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2517->hd=( void*)
_temp2515.f1; _temp2517->tl= nvs; _temp2517;}); decls= _temp2515.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2511, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2513, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2518=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2518->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2505))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2515.f3, s3, 0), 0); _temp2518->tl= test_stmts;
_temp2518;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2519= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2520; _LL2520: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0;( lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2521=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2522; _LL2522: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2521, end_l,(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2523=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2524; _LL2524: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2523, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2519, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple14 _temp2527;
void* _temp2528; struct _tuple0* _temp2530; struct _tuple14* _temp2525=( struct
_tuple14*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2527=*
_temp2525; _LL2531: _temp2530= _temp2527.f1; goto _LL2529; _LL2529: _temp2528=
_temp2527.f2; goto _LL2526; _LL2526: res= Cyc_Absyn_declare_stmt( _temp2530,
_temp2528, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region(& _temp2504);}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2532=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2532[ 0]=({ struct Cyc_List_List _temp2533; _temp2533.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2533.tl= 0; _temp2533;}); _temp2532;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2534=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2574; struct Cyc_Absyn_Stmt* _temp2576; struct Cyc_Absyn_Stmt*
_temp2578; struct Cyc_Absyn_Exp* _temp2580; struct Cyc_Absyn_Stmt* _temp2582;
struct Cyc_Absyn_Stmt* _temp2584; struct Cyc_Absyn_Exp* _temp2586; struct Cyc_Absyn_Stmt*
_temp2588; struct _tuple2 _temp2590; struct Cyc_Absyn_Stmt* _temp2592; struct
Cyc_Absyn_Exp* _temp2594; struct Cyc_Absyn_Stmt* _temp2596; struct Cyc_Absyn_Stmt*
_temp2598; struct Cyc_Absyn_Stmt* _temp2600; struct _tagged_string* _temp2602;
struct Cyc_Absyn_Stmt* _temp2604; struct _tuple2 _temp2606; struct Cyc_Absyn_Stmt*
_temp2608; struct Cyc_Absyn_Exp* _temp2610; struct _tuple2 _temp2612; struct Cyc_Absyn_Stmt*
_temp2614; struct Cyc_Absyn_Exp* _temp2616; struct Cyc_Absyn_Exp* _temp2618;
struct Cyc_List_List* _temp2620; struct Cyc_Absyn_Exp* _temp2622; struct Cyc_Absyn_Switch_clause**
_temp2624; struct Cyc_List_List* _temp2626; struct Cyc_Absyn_Stmt* _temp2628;
struct Cyc_Absyn_Decl* _temp2630; struct Cyc_Absyn_Stmt* _temp2632; struct Cyc_Absyn_Stmt*
_temp2634; struct Cyc_Absyn_Stmt* _temp2636; struct _tagged_string* _temp2638;
struct _tuple2 _temp2640; struct Cyc_Absyn_Stmt* _temp2642; struct Cyc_Absyn_Exp*
_temp2644; struct Cyc_Absyn_Stmt* _temp2646; struct Cyc_List_List* _temp2648;
struct Cyc_Absyn_Stmt* _temp2650; struct Cyc_Absyn_Stmt* _temp2652; struct Cyc_Absyn_Vardecl*
_temp2654; struct Cyc_Absyn_Tvar* _temp2656; _LL2536: if( _temp2534 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2537;} else{ goto _LL2538;} _LL2538: if((
unsigned int) _temp2534 > 1u?*(( int*) _temp2534) == Cyc_Absyn_Exp_s: 0){
_LL2575: _temp2574=(( struct Cyc_Absyn_Exp_s_struct*) _temp2534)->f1; goto
_LL2539;} else{ goto _LL2540;} _LL2540: if(( unsigned int) _temp2534 > 1u?*((
int*) _temp2534) == Cyc_Absyn_Seq_s: 0){ _LL2579: _temp2578=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2534)->f1; goto _LL2577; _LL2577: _temp2576=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2534)->f2; goto _LL2541;} else{ goto _LL2542;} _LL2542: if(( unsigned int)
_temp2534 > 1u?*(( int*) _temp2534) == Cyc_Absyn_Return_s: 0){ _LL2581:
_temp2580=(( struct Cyc_Absyn_Return_s_struct*) _temp2534)->f1; goto _LL2543;}
else{ goto _LL2544;} _LL2544: if(( unsigned int) _temp2534 > 1u?*(( int*)
_temp2534) == Cyc_Absyn_IfThenElse_s: 0){ _LL2587: _temp2586=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2534)->f1; goto _LL2585; _LL2585: _temp2584=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2534)->f2; goto _LL2583; _LL2583: _temp2582=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2534)->f3; goto _LL2545;} else{ goto _LL2546;} _LL2546: if(( unsigned int)
_temp2534 > 1u?*(( int*) _temp2534) == Cyc_Absyn_While_s: 0){ _LL2591: _temp2590=((
struct Cyc_Absyn_While_s_struct*) _temp2534)->f1; _LL2595: _temp2594= _temp2590.f1;
goto _LL2593; _LL2593: _temp2592= _temp2590.f2; goto _LL2589; _LL2589: _temp2588=((
struct Cyc_Absyn_While_s_struct*) _temp2534)->f2; goto _LL2547;} else{ goto
_LL2548;} _LL2548: if(( unsigned int) _temp2534 > 1u?*(( int*) _temp2534) == Cyc_Absyn_Break_s:
0){ _LL2597: _temp2596=(( struct Cyc_Absyn_Break_s_struct*) _temp2534)->f1; goto
_LL2549;} else{ goto _LL2550;} _LL2550: if(( unsigned int) _temp2534 > 1u?*((
int*) _temp2534) == Cyc_Absyn_Continue_s: 0){ _LL2599: _temp2598=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2534)->f1; goto _LL2551;} else{ goto _LL2552;} _LL2552: if(( unsigned int)
_temp2534 > 1u?*(( int*) _temp2534) == Cyc_Absyn_Goto_s: 0){ _LL2603: _temp2602=((
struct Cyc_Absyn_Goto_s_struct*) _temp2534)->f1; goto _LL2601; _LL2601:
_temp2600=(( struct Cyc_Absyn_Goto_s_struct*) _temp2534)->f2; goto _LL2553;}
else{ goto _LL2554;} _LL2554: if(( unsigned int) _temp2534 > 1u?*(( int*)
_temp2534) == Cyc_Absyn_For_s: 0){ _LL2619: _temp2618=(( struct Cyc_Absyn_For_s_struct*)
_temp2534)->f1; goto _LL2613; _LL2613: _temp2612=(( struct Cyc_Absyn_For_s_struct*)
_temp2534)->f2; _LL2617: _temp2616= _temp2612.f1; goto _LL2615; _LL2615:
_temp2614= _temp2612.f2; goto _LL2607; _LL2607: _temp2606=(( struct Cyc_Absyn_For_s_struct*)
_temp2534)->f3; _LL2611: _temp2610= _temp2606.f1; goto _LL2609; _LL2609:
_temp2608= _temp2606.f2; goto _LL2605; _LL2605: _temp2604=(( struct Cyc_Absyn_For_s_struct*)
_temp2534)->f4; goto _LL2555;} else{ goto _LL2556;} _LL2556: if(( unsigned int)
_temp2534 > 1u?*(( int*) _temp2534) == Cyc_Absyn_Switch_s: 0){ _LL2623:
_temp2622=(( struct Cyc_Absyn_Switch_s_struct*) _temp2534)->f1; goto _LL2621;
_LL2621: _temp2620=(( struct Cyc_Absyn_Switch_s_struct*) _temp2534)->f2; goto
_LL2557;} else{ goto _LL2558;} _LL2558: if(( unsigned int) _temp2534 > 1u?*((
int*) _temp2534) == Cyc_Absyn_Fallthru_s: 0){ _LL2627: _temp2626=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2534)->f1; goto _LL2625; _LL2625: _temp2624=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2534)->f2; goto _LL2559;} else{ goto _LL2560;} _LL2560: if(( unsigned int)
_temp2534 > 1u?*(( int*) _temp2534) == Cyc_Absyn_Decl_s: 0){ _LL2631: _temp2630=((
struct Cyc_Absyn_Decl_s_struct*) _temp2534)->f1; goto _LL2629; _LL2629:
_temp2628=(( struct Cyc_Absyn_Decl_s_struct*) _temp2534)->f2; goto _LL2561;}
else{ goto _LL2562;} _LL2562: if(( unsigned int) _temp2534 > 1u?*(( int*)
_temp2534) == Cyc_Absyn_Cut_s: 0){ _LL2633: _temp2632=(( struct Cyc_Absyn_Cut_s_struct*)
_temp2534)->f1; goto _LL2563;} else{ goto _LL2564;} _LL2564: if(( unsigned int)
_temp2534 > 1u?*(( int*) _temp2534) == Cyc_Absyn_Splice_s: 0){ _LL2635:
_temp2634=(( struct Cyc_Absyn_Splice_s_struct*) _temp2534)->f1; goto _LL2565;}
else{ goto _LL2566;} _LL2566: if(( unsigned int) _temp2534 > 1u?*(( int*)
_temp2534) == Cyc_Absyn_Label_s: 0){ _LL2639: _temp2638=(( struct Cyc_Absyn_Label_s_struct*)
_temp2534)->f1; goto _LL2637; _LL2637: _temp2636=(( struct Cyc_Absyn_Label_s_struct*)
_temp2534)->f2; goto _LL2567;} else{ goto _LL2568;} _LL2568: if(( unsigned int)
_temp2534 > 1u?*(( int*) _temp2534) == Cyc_Absyn_Do_s: 0){ _LL2647: _temp2646=((
struct Cyc_Absyn_Do_s_struct*) _temp2534)->f1; goto _LL2641; _LL2641: _temp2640=((
struct Cyc_Absyn_Do_s_struct*) _temp2534)->f2; _LL2645: _temp2644= _temp2640.f1;
goto _LL2643; _LL2643: _temp2642= _temp2640.f2; goto _LL2569;} else{ goto
_LL2570;} _LL2570: if(( unsigned int) _temp2534 > 1u?*(( int*) _temp2534) == Cyc_Absyn_TryCatch_s:
0){ _LL2651: _temp2650=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2534)->f1;
goto _LL2649; _LL2649: _temp2648=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2534)->f2; goto _LL2571;} else{ goto _LL2572;} _LL2572: if(( unsigned int)
_temp2534 > 1u?*(( int*) _temp2534) == Cyc_Absyn_Region_s: 0){ _LL2657:
_temp2656=(( struct Cyc_Absyn_Region_s_struct*) _temp2534)->f1; goto _LL2655;
_LL2655: _temp2654=(( struct Cyc_Absyn_Region_s_struct*) _temp2534)->f2; goto
_LL2653; _LL2653: _temp2652=(( struct Cyc_Absyn_Region_s_struct*) _temp2534)->f3;
goto _LL2573;} else{ goto _LL2535;} _LL2537: return; _LL2539: Cyc_Toc_exp_to_c(
nv, _temp2574); return; _LL2541: Cyc_Toc_stmt_to_c( nv, _temp2578); s= _temp2576;
continue; _LL2543: { struct Cyc_Core_Opt* topt= 0; if( _temp2580 != 0){ topt=({
struct Cyc_Core_Opt* _temp2658=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2658->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2580))->topt))->v);
_temp2658;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2580));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2659=
Cyc_Toc_temp_var(); goto _LL2660; _LL2660: { struct Cyc_Absyn_Stmt* _temp2661=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2659, 0),
0); goto _LL2662; _LL2662:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2659,( void*)(( struct Cyc_Core_Opt*) _check_null( topt))->v, _temp2580,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), _temp2661, 0), 0))->r));}}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2545: Cyc_Toc_exp_to_c(
nv, _temp2586); Cyc_Toc_stmt_to_c( nv, _temp2584); s= _temp2582; continue;
_LL2547: Cyc_Toc_exp_to_c( nv, _temp2594); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2588); return; _LL2549: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null( nv->break_lab))->v, 0))->r));}{ int dest_depth= _temp2596 == 0? 0:((
struct Cyc_Absyn_Stmt*) _check_null( _temp2596))->try_depth; Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2551: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -(( struct Cyc_Absyn_Stmt*) _check_null( _temp2598))->try_depth, s);
return; _LL2553: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2600))->try_depth, s); return; _LL2555: Cyc_Toc_exp_to_c( nv,
_temp2618); Cyc_Toc_exp_to_c( nv, _temp2616); Cyc_Toc_exp_to_c( nv, _temp2610);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2604); return; _LL2557: Cyc_Toc_xlate_switch(
nv, s, _temp2622, _temp2620); return; _LL2559: if( nv->fallthru_info == 0){(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2663="fallthru in unexpected place"; struct _tagged_string
_temp2664; _temp2664.curr= _temp2663; _temp2664.base= _temp2663; _temp2664.last_plus_one=
_temp2663 + 29; _temp2664;})));}{ struct _tuple8 _temp2667; struct Cyc_Dict_Dict*
_temp2668; struct Cyc_List_List* _temp2670; struct _tagged_string* _temp2672;
struct _tuple8* _temp2665=( struct _tuple8*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp2667=* _temp2665; _LL2673: _temp2672= _temp2667.f1;
goto _LL2671; _LL2671: _temp2670= _temp2667.f2; goto _LL2669; _LL2669: _temp2668=
_temp2667.f3; goto _LL2666; _LL2666: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp2672, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp2624)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp2674=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2670); goto _LL2675; _LL2675: { struct Cyc_List_List* _temp2676=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp2626); goto
_LL2677; _LL2677: for( 0; _temp2674 != 0;( _temp2674=(( struct Cyc_List_List*)
_check_null( _temp2674))->tl, _temp2676=(( struct Cyc_List_List*) _check_null(
_temp2676))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp2676))->hd); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2668,( struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp2674))->hd),(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp2676))->hd, 0),
s2, 0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}}} _LL2561:{ void*
_temp2678=( void*) _temp2630->r; int _temp2688; struct Cyc_Absyn_Exp* _temp2690;
struct Cyc_Core_Opt* _temp2692; struct Cyc_Core_Opt* _temp2694; struct Cyc_Absyn_Pat*
_temp2696; struct Cyc_List_List* _temp2698; struct Cyc_Absyn_Vardecl* _temp2700;
_LL2680: if(*(( int*) _temp2678) == Cyc_Absyn_Let_d){ _LL2697: _temp2696=((
struct Cyc_Absyn_Let_d_struct*) _temp2678)->f1; goto _LL2695; _LL2695: _temp2694=((
struct Cyc_Absyn_Let_d_struct*) _temp2678)->f2; goto _LL2693; _LL2693: _temp2692=((
struct Cyc_Absyn_Let_d_struct*) _temp2678)->f3; goto _LL2691; _LL2691: _temp2690=((
struct Cyc_Absyn_Let_d_struct*) _temp2678)->f4; goto _LL2689; _LL2689: _temp2688=((
struct Cyc_Absyn_Let_d_struct*) _temp2678)->f5; goto _LL2681;} else{ goto
_LL2682;} _LL2682: if(*(( int*) _temp2678) == Cyc_Absyn_Letv_d){ _LL2699:
_temp2698=(( struct Cyc_Absyn_Letv_d_struct*) _temp2678)->f1; goto _LL2683;}
else{ goto _LL2684;} _LL2684: if(*(( int*) _temp2678) == Cyc_Absyn_Var_d){
_LL2701: _temp2700=(( struct Cyc_Absyn_Var_d_struct*) _temp2678)->f1; goto
_LL2685;} else{ goto _LL2686;} _LL2686: goto _LL2687; _LL2681:( void*)( s->r=(
void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2696, _temp2692, _temp2690,
_temp2688, _temp2628))->r)); goto _LL2679; _LL2683: { struct Cyc_List_List*
_temp2702=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2698); goto _LL2703; _LL2703: if( _temp2702 == 0){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2704="empty Letv_d";
struct _tagged_string _temp2705; _temp2705.curr= _temp2704; _temp2705.base=
_temp2704; _temp2705.last_plus_one= _temp2704 + 13; _temp2705;}));}( void*)(
_temp2630->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2706=(
struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2706[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2707; _temp2707.tag= Cyc_Absyn_Var_d;
_temp2707.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2702))->hd; _temp2707;}); _temp2706;}))); _temp2702=(( struct Cyc_List_List*)
_check_null( _temp2702))->tl; for( 0; _temp2702 != 0; _temp2702=(( struct Cyc_List_List*)
_check_null( _temp2702))->tl){ struct Cyc_Absyn_Decl* _temp2710= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2708=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2708[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2709; _temp2709.tag= Cyc_Absyn_Var_d; _temp2709.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2702))->hd; _temp2709;}); _temp2708;}),
0); goto _LL2711; _LL2711:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp2710, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c(
nv, s); goto _LL2679;} _LL2685: { struct _tuple0* _temp2712= _temp2700->name;
goto _LL2713; _LL2713:( void*)( _temp2700->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2700->type));{ struct Cyc_Toc_Env* _temp2716= Cyc_Toc_add_varmap( nv,
_temp2712, Cyc_Absyn_varb_exp( _temp2712,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2714=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2714[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2715; _temp2715.tag= Cyc_Absyn_Local_b;
_temp2715.f1= _temp2700; _temp2715;}); _temp2714;}), 0)); goto _LL2717; _LL2717:
Cyc_Toc_stmt_to_c( _temp2716, _temp2628); if( _temp2700->initializer != 0){
struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*) _check_null( _temp2700->initializer);
void* _temp2718=( void*) init->r; struct Cyc_Absyn_Exp* _temp2724; struct Cyc_Absyn_Exp*
_temp2726; struct Cyc_Absyn_Vardecl* _temp2728; _LL2720: if(*(( int*) _temp2718)
== Cyc_Absyn_Comprehension_e){ _LL2729: _temp2728=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2718)->f1; goto _LL2727; _LL2727: _temp2726=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2718)->f2; goto _LL2725; _LL2725: _temp2724=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2718)->f3; goto _LL2721;} else{ goto _LL2722;} _LL2722: goto _LL2723;
_LL2721: _temp2700->initializer= 0;( void*)( _temp2628->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2712, 0), _temp2728, _temp2726, _temp2724, Cyc_Absyn_new_stmt((
void*) _temp2628->r, 0), 0))->r)); goto _LL2719; _LL2723: Cyc_Toc_exp_to_c( nv,
init); goto _LL2719; _LL2719:;} goto _LL2679;}} _LL2687:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2730="bad nested declaration within function";
struct _tagged_string _temp2731; _temp2731.curr= _temp2730; _temp2731.base=
_temp2730; _temp2731.last_plus_one= _temp2730 + 39; _temp2731;})); goto _LL2679;
_LL2679:;} return; _LL2563:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2732="cut"; struct _tagged_string _temp2733; _temp2733.curr=
_temp2732; _temp2733.base= _temp2732; _temp2733.last_plus_one= _temp2732 + 4;
_temp2733;})); return; _LL2565:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2734="splice"; struct _tagged_string _temp2735; _temp2735.curr=
_temp2734; _temp2735.base= _temp2734; _temp2735.last_plus_one= _temp2734 + 7;
_temp2735;})); return; _LL2567: s= _temp2636; continue; _LL2569: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp2646); Cyc_Toc_exp_to_c( nv, _temp2644); return;
_LL2571: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__push_handler_qv,
0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp( Cyc_Toc_setjmp_qv, 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__pop_handler_qv,
0); struct Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp( Cyc_Toc__exn_thrown_qv,
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2736=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2736->v=( void*) e_typ; _temp2736;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2650);{
struct Cyc_Absyn_Stmt* _temp2737= Cyc_Absyn_seq_stmt( _temp2650, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); goto _LL2738; _LL2738: {
struct _tuple0* _temp2739= Cyc_Toc_temp_var(); goto _LL2740; _LL2740: { struct
Cyc_Absyn_Exp* _temp2741= Cyc_Absyn_var_exp( _temp2739, 0); goto _LL2742;
_LL2742: { struct Cyc_Absyn_Vardecl* _temp2743= Cyc_Absyn_new_vardecl( _temp2739,
Cyc_Absyn_exn_typ, 0); goto _LL2744; _LL2744: _temp2741->topt=({ struct Cyc_Core_Opt*
_temp2745=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2745->v=( void*) Cyc_Absyn_exn_typ; _temp2745;});{ struct Cyc_Absyn_Pat*
_temp2750=({ struct Cyc_Absyn_Pat* _temp2746=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2746->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2748=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2748[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2749; _temp2749.tag= Cyc_Absyn_Var_p;
_temp2749.f1= _temp2743; _temp2749;}); _temp2748;})); _temp2746->topt=({ struct
Cyc_Core_Opt* _temp2747=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2747->v=( void*) Cyc_Absyn_exn_typ; _temp2747;}); _temp2746->loc= 0;
_temp2746;}); goto _LL2751; _LL2751: { struct Cyc_Absyn_Exp* _temp2752= Cyc_Absyn_throw_exp(
_temp2741, 0); goto _LL2753; _LL2753: _temp2752->topt=({ struct Cyc_Core_Opt*
_temp2754=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2754->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2754;});{ struct Cyc_Absyn_Stmt*
_temp2755= Cyc_Absyn_exp_stmt( _temp2752, 0); goto _LL2756; _LL2756: { struct
Cyc_Absyn_Switch_clause* _temp2760=({ struct Cyc_Absyn_Switch_clause* _temp2757=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2757->pattern= _temp2750; _temp2757->pat_vars=({ struct Cyc_Core_Opt*
_temp2758=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2758->v=( void*)({ struct Cyc_List_List* _temp2759=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2759->hd=( void*) _temp2743;
_temp2759->tl= 0; _temp2759;}); _temp2758;}); _temp2757->where_clause= 0;
_temp2757->body= _temp2755; _temp2757->loc= 0; _temp2757;}); goto _LL2761;
_LL2761: { struct Cyc_Absyn_Stmt* _temp2763= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2648,({ struct Cyc_List_List* _temp2762=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2762->hd=( void*) _temp2760; _temp2762->tl=
0; _temp2762;})), 0); goto _LL2764; _LL2764: Cyc_Toc_stmt_to_c( nv, _temp2763);{
struct Cyc_Absyn_Exp* _temp2766= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2765=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2765->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2765->tl= 0; _temp2765;}), 0); goto _LL2767; _LL2767: { struct Cyc_Absyn_Stmt*
_temp2769= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2768=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2768->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2768->tl= 0;
_temp2768;}), 0), 0); goto _LL2770; _LL2770: { struct Cyc_Absyn_Exp* _temp2771=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2772; _LL2772: {
struct Cyc_Absyn_Exp* _temp2773= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2774; _LL2774:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2769, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2771, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2766, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2773, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2737, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2763, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2573: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2654->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2780=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2780[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2781; _temp2781.tag= Cyc_Absyn_Local_b; _temp2781.f1=
_temp2654; _temp2781;}); _temp2780;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2779=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2779->f1= Cyc_Toc_abs_ns; _temp2779->f2= Cyc_Toc__push_region_sp;
_temp2779;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2778=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2778->f1= Cyc_Toc_abs_ns; _temp2778->f2= Cyc_Toc__pop_region_sp; _temp2778;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2777=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2777->f1=
Cyc_Toc_abs_ns; _temp2777->f2= Cyc_Toc__new_region_sp; _temp2777;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2776=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2776->hd=( void*) addr_rh_exp; _temp2776->tl= 0; _temp2776;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2775=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2775->hd=( void*) x_exp; _temp2775->tl= 0; _temp2775;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2652);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2652, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2535:;}} struct _tuple17{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0;(
void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{ struct
Cyc_List_List* _temp2782= f->args; goto _LL2783; _LL2783: for( 0; _temp2782 != 0;
_temp2782=(( struct Cyc_List_List*) _check_null( _temp2782))->tl){(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp2782))->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp2782))->hd)).f3);{
struct _tuple0* _temp2785=({ struct _tuple0* _temp2784=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2784->f1=( void*) Cyc_Absyn_Loc_n;
_temp2784->f2=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp2782))->hd)).f1; _temp2784;}); goto _LL2786; _LL2786: nv= Cyc_Toc_add_varmap(
nv, _temp2785, Cyc_Absyn_var_exp( _temp2785, 0));}}} if(( unsigned int) f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs))->name != 0: 0){ int
_temp2789; void* _temp2791; void* _temp2793; struct Cyc_Absyn_Tqual _temp2795;
struct Cyc_Core_Opt* _temp2797; struct Cyc_Absyn_VarargInfo _temp2787=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs)); _LL2798: _temp2797=
_temp2787.name; goto _LL2796; _LL2796: _temp2795= _temp2787.tq; goto _LL2794;
_LL2794: _temp2793=( void*) _temp2787.type; goto _LL2792; _LL2792: _temp2791=(
void*) _temp2787.rgn; goto _LL2790; _LL2790: _temp2789= _temp2787.inject; goto
_LL2788; _LL2788: { void* _temp2799= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp2793, _temp2791, _temp2795)); goto _LL2800; _LL2800: { struct _tuple0*
_temp2802=({ struct _tuple0* _temp2801=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2801->f1=( void*) Cyc_Absyn_Loc_n; _temp2801->f2=( struct
_tagged_string*)(( struct Cyc_Core_Opt*) _check_null( _temp2797))->v; _temp2801;});
goto _LL2803; _LL2803: f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2804=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2804->hd=( void*)({ struct _tuple17* _temp2805=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp2805->f1=( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( _temp2797))->v; _temp2805->f2= _temp2795;
_temp2805->f3= _temp2799; _temp2805;}); _temp2804->tl= 0; _temp2804;})); nv= Cyc_Toc_add_varmap(
nv, _temp2802, Cyc_Absyn_var_exp( _temp2802, 0)); f->cyc_varargs= 0;}}}{ struct
Cyc_List_List* _temp2806=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; goto _LL2807; _LL2807: for( 0; _temp2806 !=
0; _temp2806=(( struct Cyc_List_List*) _check_null( _temp2806))->tl){( void*)(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp2806))->hd)->type=(
void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp2806))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2808= s;
_LL2810: if( _temp2808 ==( void*) Cyc_Absyn_Abstract){ goto _LL2811;} else{ goto
_LL2812;} _LL2812: if( _temp2808 ==( void*) Cyc_Absyn_ExternC){ goto _LL2813;}
else{ goto _LL2814;} _LL2814: goto _LL2815; _LL2811: return( void*) Cyc_Absyn_Public;
_LL2813: return( void*) Cyc_Absyn_Extern; _LL2815: return s; _LL2809:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2816=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2817; _LL2817: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2818=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2818->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2818;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2819=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2816).f2); goto _LL2820;
_LL2820: if( _temp2819 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2816).f2, s));}
else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null(
_temp2819))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2816).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2821=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2821->v=( void*)
_temp2816; _temp2821;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2822=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2823; _LL2823: for( 0; _temp2822 != 0;
_temp2822=(( struct Cyc_List_List*) _check_null( _temp2822))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2822))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2822))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2824=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2825; _LL2825: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2826=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2826->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2826;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2827=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2824).f2); goto _LL2828; _LL2828:
if( _temp2827 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2824).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2827))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2824).f2, u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)(
u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==(
void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?( void*)
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2829=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2829->v=( void*) _temp2824; _temp2829;}); if( u->fields !=
0){ if( seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* _temp2830=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( u->fields))->v; goto
_LL2831; _LL2831: for( 0; _temp2830 != 0; _temp2830=(( struct Cyc_List_List*)
_check_null( _temp2830))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2830))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2830))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2832=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2832->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2832;});}{ struct
_tuple0* _temp2833= tud->name; goto _LL2834; _LL2834: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2833)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2833));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2835=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2836; _LL2836: for( 0;
_temp2835 != 0; _temp2835=(( struct Cyc_List_List*) _check_null( _temp2835))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2835))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2837; _temp2837.q_const= 1; _temp2837.q_volatile= 0;
_temp2837.q_restrict= 0; _temp2837;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2838=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2838->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2839=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2839[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2840; _temp2840.tag= Cyc_Absyn_Var_d;
_temp2840.f1= vd; _temp2840;}); _temp2839;}), 0); _temp2838->tl= Cyc_Toc_result_decls;
_temp2838;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2841; _temp2841.q_const=
1; _temp2841.q_volatile= 0; _temp2841.q_restrict= 0; _temp2841;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2842=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2842->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2843=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2843[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2844; _temp2844.tag= Cyc_Absyn_Var_d; _temp2844.f1= vd; _temp2844;});
_temp2843;}), 0); _temp2842->tl= Cyc_Toc_result_decls; _temp2842;});{ struct Cyc_List_List*
_temp2845= 0; goto _LL2846; _LL2846: { int i= 1;{ struct Cyc_List_List*
_temp2847= f->typs; goto _LL2848; _LL2848: for( 0; _temp2847 != 0;( _temp2847=((
struct Cyc_List_List*) _check_null( _temp2847))->tl, i ++)){ struct
_tagged_string* _temp2849= Cyc_Toc_fieldname( i); goto _LL2850; _LL2850: {
struct Cyc_Absyn_Structfield* _temp2852=({ struct Cyc_Absyn_Structfield*
_temp2851=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2851->name= _temp2849; _temp2851->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2847))->hd)).f1; _temp2851->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2847))->hd)).f2);
_temp2851->width= 0; _temp2851->attributes= 0; _temp2851;}); goto _LL2853;
_LL2853: _temp2845=({ struct Cyc_List_List* _temp2854=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2854->hd=( void*) _temp2852;
_temp2854->tl= _temp2845; _temp2854;});}}} _temp2845=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2845); _temp2845=({ struct Cyc_List_List*
_temp2855=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2855->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2856=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2856->name= Cyc_Toc_tag_sp;
_temp2856->tq= Cyc_Toc_mt_tq; _temp2856->type=( void*) Cyc_Absyn_sint_t;
_temp2856->width= 0; _temp2856->attributes= 0; _temp2856;}); _temp2855->tl=
_temp2845; _temp2855;});{ struct Cyc_Absyn_Structdecl* _temp2862=({ struct Cyc_Absyn_Structdecl*
_temp2857=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2857->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2857->name=({ struct Cyc_Core_Opt*
_temp2859=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2859->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2860="_struct"; struct _tagged_string _temp2861; _temp2861.curr= _temp2860;
_temp2861.base= _temp2860; _temp2861.last_plus_one= _temp2860 + 8; _temp2861;}));
_temp2859;}); _temp2857->tvs= 0; _temp2857->fields=({ struct Cyc_Core_Opt*
_temp2858=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2858->v=( void*) _temp2845; _temp2858;}); _temp2857->attributes= 0;
_temp2857;}); goto _LL2863; _LL2863: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2864=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2864->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2865=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2865[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2866; _temp2866.tag= Cyc_Absyn_Struct_d;
_temp2866.f1= _temp2862; _temp2866;}); _temp2865;}), 0); _temp2864->tl= Cyc_Toc_result_decls;
_temp2864;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2867=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2867->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2867;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2868= xd->name; goto _LL2869; _LL2869: {
struct Cyc_List_List* _temp2870=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2871; _LL2871: for( 0; _temp2870 != 0;
_temp2870=(( struct Cyc_List_List*) _check_null( _temp2870))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2870))->hd; struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2873= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2872=* fn;(
unsigned int)( _temp2872.last_plus_one - _temp2872.curr);}) + 4, 0); goto
_LL2874; _LL2874: { struct Cyc_Absyn_ArrayType_struct* _temp2877=({ struct Cyc_Absyn_ArrayType_struct*
_temp2875=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2875[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2876; _temp2876.tag= Cyc_Absyn_ArrayType;
_temp2876.f1=( void*) Cyc_Absyn_uchar_t; _temp2876.f2= Cyc_Toc_mt_tq; _temp2876.f3=(
struct Cyc_Absyn_Exp*) _temp2873; _temp2876;}); _temp2875;}); goto _LL2878;
_LL2878: { struct Cyc_Core_Opt* _temp2879=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2887; int _temp2888; _LL2881: if( _temp2879 == 0){ goto
_LL2882;} else{ goto _LL2883;} _LL2883: if( _temp2879 == 0){ goto _LL2885;}
else{ _temp2887=* _temp2879; _LL2889: _temp2888=( int) _temp2887.v; if(
_temp2888 == 0){ goto _LL2884;} else{ goto _LL2885;}} _LL2885: goto _LL2886;
_LL2882: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(({ int _temp2890= 0; int
_temp2891= 0; int _temp2892= 0; int _temp2893= 0; struct _tagged_string
_temp2894=* fn; xprintf("%c%c%c%c%.*s", _temp2890, _temp2891, _temp2892,
_temp2893, _temp2894.last_plus_one - _temp2894.curr, _temp2894.curr);}), 0);}{
struct Cyc_Absyn_Vardecl* _temp2895= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2877, initopt); goto _LL2896; _LL2896:( void*)( _temp2895->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2897=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2897->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2898=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2898[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2899; _temp2899.tag= Cyc_Absyn_Var_d;
_temp2899.f1= _temp2895; _temp2899;}); _temp2898;}), 0); _temp2897->tl= Cyc_Toc_result_decls;
_temp2897;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2900= f->typs; goto _LL2901; _LL2901: for( 0;
_temp2900 != 0;( _temp2900=(( struct Cyc_List_List*) _check_null( _temp2900))->tl,
i ++)){ struct _tagged_string* _temp2903=( struct _tagged_string*)({ struct
_tagged_string* _temp2902=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2902[ 0]= xprintf("f%d", i); _temp2902;}); goto
_LL2904; _LL2904: { struct Cyc_Absyn_Structfield* _temp2906=({ struct Cyc_Absyn_Structfield*
_temp2905=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2905->name= _temp2903; _temp2905->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2900))->hd)).f1; _temp2905->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2900))->hd)).f2);
_temp2905->width= 0; _temp2905->attributes= 0; _temp2905;}); goto _LL2907;
_LL2907: fields=({ struct Cyc_List_List* _temp2908=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2908->hd=( void*) _temp2906;
_temp2908->tl= fields; _temp2908;});}}} fields=({ struct Cyc_List_List*
_temp2909=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2909->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2910=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2910->name= Cyc_Toc_tag_sp;
_temp2910->tq= Cyc_Toc_mt_tq; _temp2910->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2910->width= 0; _temp2910->attributes= 0; _temp2910;});
_temp2909->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2909;});{ struct Cyc_Absyn_Structdecl* _temp2916=({ struct Cyc_Absyn_Structdecl*
_temp2911=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2911->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2911->name=({ struct Cyc_Core_Opt*
_temp2913=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2913->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2914="_struct"; struct _tagged_string _temp2915; _temp2915.curr= _temp2914;
_temp2915.base= _temp2914; _temp2915.last_plus_one= _temp2914 + 8; _temp2915;}));
_temp2913;}); _temp2911->tvs= 0; _temp2911->fields=({ struct Cyc_Core_Opt*
_temp2912=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2912->v=( void*) fields; _temp2912;}); _temp2911->attributes= 0; _temp2911;});
goto _LL2917; _LL2917: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2918=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2918->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2919=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2919[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2920; _temp2920.tag= Cyc_Absyn_Struct_d;
_temp2920.f1= _temp2916; _temp2920;}); _temp2919;}), 0); _temp2918->tl= Cyc_Toc_result_decls;
_temp2918;});}} goto _LL2880;}} _LL2884: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2926= Cyc_Absyn_string_exp(({ int _temp2921= 0; int
_temp2922= 0; int _temp2923= 0; int _temp2924= 0; struct _tagged_string
_temp2925=* fn; xprintf("%c%c%c%c%.*s", _temp2921, _temp2922, _temp2923,
_temp2924, _temp2925.last_plus_one - _temp2925.curr, _temp2925.curr);}), 0);
goto _LL2927; _LL2927: { struct Cyc_Absyn_Vardecl* _temp2928= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2877,( struct Cyc_Absyn_Exp*) _temp2926); goto _LL2929;
_LL2929:( void*)( _temp2928->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2930=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2930->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2931=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2931[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2932; _temp2932.tag= Cyc_Absyn_Var_d; _temp2932.f1= _temp2928; _temp2932;});
_temp2931;}), 0); _temp2930->tl= Cyc_Toc_result_decls; _temp2930;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2880; _LL2886: goto
_LL2880; _LL2880:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2933= ed->name; goto
_LL2934; _LL2934:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2935=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; goto _LL2936; _LL2936: for( 0; _temp2935 != 0;
_temp2935=(( struct Cyc_List_List*) _check_null( _temp2935))->tl){ if((( struct
Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( _temp2935))->hd)->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp2935))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2935))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct _RegionHandle
_temp2937= _new_region(); struct _RegionHandle* prgn=& _temp2937; _push_region(
prgn);{ struct Cyc_Absyn_Stmt* _temp2940; struct Cyc_List_List* _temp2942;
struct Cyc_Toc_Env* _temp2944; struct _tuple13 _temp2938= Cyc_Toc_xlate_pat( nv,
prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2945: _temp2944= _temp2938.f1; goto _LL2943; _LL2943: _temp2942=
_temp2938.f2; goto _LL2941; _LL2941: _temp2940= _temp2938.f3; goto _LL2939;
_LL2939: Cyc_Toc_stmt_to_c( _temp2944, s);{ struct Cyc_Absyn_Stmt* _temp2946=
Cyc_Absyn_label_stmt( succ_lab, s, 0); goto _LL2947; _LL2947: if( exhaust){ s=
Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2940, _temp2946, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2949= Cyc_Toc_newthrow_exp(
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2948=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2948->f1= Cyc_Toc_abs_ns; _temp2948->f2= Cyc_Toc_Match_Exception_sp;
_temp2948;}), 0)); goto _LL2950; _LL2950: { struct Cyc_Absyn_Stmt* _temp2951=
Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2949, 0), 0); goto
_LL2952; _LL2952: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2940, Cyc_Absyn_seq_stmt( _temp2951,
_temp2946, 0), 0), 0);}} for( 0; _temp2942 != 0; _temp2942=(( struct Cyc_List_List*)
_check_null( _temp2942))->tl){ struct _tuple14 _temp2955; void* _temp2956;
struct _tuple0* _temp2958; struct _tuple14* _temp2953=( struct _tuple14*)((
struct Cyc_List_List*) _check_null( _temp2942))->hd; _temp2955=* _temp2953;
_LL2959: _temp2958= _temp2955.f1; goto _LL2957; _LL2957: _temp2956= _temp2955.f2;
goto _LL2954; _LL2954: s= Cyc_Absyn_declare_stmt( _temp2958, _temp2956, 0, s, 0);}}};
_pop_region(& _temp2937);} return s;}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=((
struct Cyc_List_List*) _check_null( ds))->tl){ struct Cyc_Absyn_Decl* d=( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2960=(
void*) d->r; struct Cyc_Absyn_Vardecl* _temp2986; struct Cyc_Absyn_Fndecl*
_temp2988; int _temp2990; struct Cyc_Absyn_Exp* _temp2992; struct Cyc_Core_Opt*
_temp2994; struct Cyc_Core_Opt* _temp2996; struct Cyc_Absyn_Pat* _temp2998;
struct Cyc_List_List* _temp3000; struct Cyc_Absyn_Structdecl* _temp3002; struct
Cyc_Absyn_Uniondecl* _temp3004; struct Cyc_Absyn_Tuniondecl* _temp3006; struct
Cyc_Absyn_Enumdecl* _temp3008; struct Cyc_Absyn_Typedefdecl* _temp3010; struct
Cyc_List_List* _temp3012; struct _tagged_string* _temp3014; struct Cyc_List_List*
_temp3016; struct _tuple0* _temp3018; struct Cyc_List_List* _temp3020; _LL2962:
if(*(( int*) _temp2960) == Cyc_Absyn_Var_d){ _LL2987: _temp2986=(( struct Cyc_Absyn_Var_d_struct*)
_temp2960)->f1; goto _LL2963;} else{ goto _LL2964;} _LL2964: if(*(( int*)
_temp2960) == Cyc_Absyn_Fn_d){ _LL2989: _temp2988=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2960)->f1; goto _LL2965;} else{ goto _LL2966;} _LL2966: if(*(( int*)
_temp2960) == Cyc_Absyn_Let_d){ _LL2999: _temp2998=(( struct Cyc_Absyn_Let_d_struct*)
_temp2960)->f1; goto _LL2997; _LL2997: _temp2996=(( struct Cyc_Absyn_Let_d_struct*)
_temp2960)->f2; goto _LL2995; _LL2995: _temp2994=(( struct Cyc_Absyn_Let_d_struct*)
_temp2960)->f3; goto _LL2993; _LL2993: _temp2992=(( struct Cyc_Absyn_Let_d_struct*)
_temp2960)->f4; goto _LL2991; _LL2991: _temp2990=(( struct Cyc_Absyn_Let_d_struct*)
_temp2960)->f5; goto _LL2967;} else{ goto _LL2968;} _LL2968: if(*(( int*)
_temp2960) == Cyc_Absyn_Letv_d){ _LL3001: _temp3000=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2960)->f1; goto _LL2969;} else{ goto _LL2970;} _LL2970: if(*(( int*)
_temp2960) == Cyc_Absyn_Struct_d){ _LL3003: _temp3002=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2960)->f1; goto _LL2971;} else{ goto _LL2972;} _LL2972: if(*(( int*)
_temp2960) == Cyc_Absyn_Union_d){ _LL3005: _temp3004=(( struct Cyc_Absyn_Union_d_struct*)
_temp2960)->f1; goto _LL2973;} else{ goto _LL2974;} _LL2974: if(*(( int*)
_temp2960) == Cyc_Absyn_Tunion_d){ _LL3007: _temp3006=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2960)->f1; goto _LL2975;} else{ goto _LL2976;} _LL2976: if(*(( int*)
_temp2960) == Cyc_Absyn_Enum_d){ _LL3009: _temp3008=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2960)->f1; goto _LL2977;} else{ goto _LL2978;} _LL2978: if(*(( int*)
_temp2960) == Cyc_Absyn_Typedef_d){ _LL3011: _temp3010=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2960)->f1; goto _LL2979;} else{ goto _LL2980;} _LL2980: if(*(( int*)
_temp2960) == Cyc_Absyn_Namespace_d){ _LL3015: _temp3014=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2960)->f1; goto _LL3013; _LL3013: _temp3012=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2960)->f2; goto _LL2981;} else{ goto _LL2982;} _LL2982: if(*(( int*)
_temp2960) == Cyc_Absyn_Using_d){ _LL3019: _temp3018=(( struct Cyc_Absyn_Using_d_struct*)
_temp2960)->f1; goto _LL3017; _LL3017: _temp3016=(( struct Cyc_Absyn_Using_d_struct*)
_temp2960)->f2; goto _LL2983;} else{ goto _LL2984;} _LL2984: if(*(( int*)
_temp2960) == Cyc_Absyn_ExternC_d){ _LL3021: _temp3020=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2960)->f1; goto _LL2985;} else{ goto _LL2961;} _LL2963: { struct _tuple0*
_temp3022= _temp2986->name; goto _LL3023; _LL3023: if(( void*) _temp2986->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3022=({ struct _tuple0* _temp3024=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3024->f1= Cyc_Toc_abs_ns;
_temp3024->f2=(* _temp3022).f2; _temp3024;});} if( _temp2986->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null( _temp2986->initializer));}
if( ! nv->toplevel){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp3025="decls_to_c: not at toplevel!"; struct _tagged_string
_temp3026; _temp3026.curr= _temp3025; _temp3026.base= _temp3025; _temp3026.last_plus_one=
_temp3025 + 29; _temp3026;}));} nv= Cyc_Toc_add_varmap( nv, _temp2986->name, Cyc_Absyn_varb_exp(
_temp3022,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp3027=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp3027[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp3028; _temp3028.tag= Cyc_Absyn_Global_b;
_temp3028.f1= _temp2986; _temp3028;}); _temp3027;}), 0)); _temp2986->name=
_temp3022;( void*)( _temp2986->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2986->sc));(
void*)( _temp2986->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2986->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3029=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3029->hd=( void*) d; _temp3029->tl=
Cyc_Toc_result_decls; _temp3029;}); goto _LL2961;} _LL2965: { struct _tuple0*
_temp3030= _temp2988->name; goto _LL3031; _LL3031: if(( void*) _temp2988->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3030=({ struct _tuple0* _temp3032=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3032->f1= Cyc_Toc_abs_ns;
_temp3032->f2=(* _temp3030).f2; _temp3032;});} nv= Cyc_Toc_add_varmap( nv,
_temp2988->name, Cyc_Absyn_var_exp( _temp3030, 0)); _temp2988->name= _temp3030;
Cyc_Toc_fndecl_to_c( nv, _temp2988); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3033=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3033->hd=( void*) d; _temp3033->tl= Cyc_Toc_result_decls; _temp3033;});
goto _LL2961;} _LL2967:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp3034="letdecl"; struct _tagged_string _temp3035; _temp3035.curr=
_temp3034; _temp3035.base= _temp3034; _temp3035.last_plus_one= _temp3034 + 8;
_temp3035;})); goto _LL2961; _LL2969:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp3036="letdecl"; struct _tagged_string _temp3037; _temp3037.curr=
_temp3036; _temp3037.base= _temp3036; _temp3037.last_plus_one= _temp3036 + 8;
_temp3037;})); goto _LL2961; _LL2971: Cyc_Toc_structdecl_to_c( _temp3002); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3038=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3038->hd=( void*) d; _temp3038->tl= Cyc_Toc_result_decls;
_temp3038;}); goto _LL2961; _LL2973: Cyc_Toc_uniondecl_to_c( _temp3004); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3039=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3039->hd=( void*) d; _temp3039->tl= Cyc_Toc_result_decls;
_temp3039;}); goto _LL2961; _LL2975: if( _temp3006->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp3006);} else{ Cyc_Toc_tuniondecl_to_c( _temp3006);} goto _LL2961; _LL2977:
Cyc_Toc_enumdecl_to_c( nv, _temp3008); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3040=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3040->hd=( void*) d; _temp3040->tl= Cyc_Toc_result_decls; _temp3040;});
goto _LL2961; _LL2979: _temp3010->name= _temp3010->name; _temp3010->tvs= 0;(
void*)( _temp3010->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp3010->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3041=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3041->hd=( void*) d; _temp3041->tl=
Cyc_Toc_result_decls; _temp3041;}); goto _LL2961; _LL2981: _temp3016= _temp3012;
goto _LL2983; _LL2983: _temp3020= _temp3016; goto _LL2985; _LL2985: nv= Cyc_Toc_decls_to_c(
nv, _temp3020, top); goto _LL2961; _LL2961:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}