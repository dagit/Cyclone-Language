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
_tagged_string s){({ struct _tagged_string _temp1= s; fprintf( Cyc_Stdio_stderr,"%.*s\n",
_temp1.last_plus_one - _temp1.curr, _temp1.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(( void*) Cyc_Toc_Toc_Unimplemented);}
static void* Cyc_Toc_toc_impos( struct _tagged_string s){({ struct
_tagged_string _temp2= s; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp2.last_plus_one
- _temp2.curr, _temp2.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( void) _throw(( void*) Cyc_Toc_Toc_Impossible);}
unsigned char Cyc_Toc_Match_error[ 16u]="\000\000\000\000Match_error"; static
unsigned char _temp5[ 5u]="curr"; static struct _tagged_string Cyc_Toc_curr_string={
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
_LL747:;}} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp*
e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__throw_qv, 0),({
struct Cyc_List_List* _temp809=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp809->hd=( void*) e; _temp809->tl= 0; _temp809;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp810="can't do null-check conversion at top-level";
struct _tagged_string _temp811; _temp811.curr= _temp810; _temp811.base= _temp810;
_temp811.last_plus_one= _temp810 + 44; _temp811;}));} if( Cyc_Toc_use_runtimecheck_funs){
return Cyc_Absyn_cast_exp( new_typ, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp812=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp812->hd=( void*) e; _temp812->tl= 0; _temp812;}), 0),
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
sz); void* strt_typ; void* elt_typ;{ void* _temp813= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp819; struct Cyc_Absyn_Conref* _temp821; struct Cyc_Absyn_Tqual
_temp823; struct Cyc_Absyn_Conref* _temp825; void* _temp827; void* _temp829;
_LL815: if(( unsigned int) _temp813 > 4u?*(( int*) _temp813) == Cyc_Absyn_PointerType:
0){ _LL820: _temp819=(( struct Cyc_Absyn_PointerType_struct*) _temp813)->f1;
_LL830: _temp829=( void*) _temp819.elt_typ; goto _LL828; _LL828: _temp827=( void*)
_temp819.rgn_typ; goto _LL826; _LL826: _temp825= _temp819.nullable; goto _LL824;
_LL824: _temp823= _temp819.tq; goto _LL822; _LL822: _temp821= _temp819.bounds;
goto _LL816;} else{ goto _LL817;} _LL817: goto _LL818; _LL816: elt_typ= _temp829;
strt_typ= Cyc_Toc_add_tagged_ptr_type( _temp829, Cyc_Toc_mt_tq); goto _LL814;
_LL818: return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp831="tag_ptr_conv: not a pointer"; struct _tagged_string
_temp832; _temp832.curr= _temp831; _temp832.base= _temp831; _temp832.last_plus_one=
_temp831 + 28; _temp832;})); _LL814:;} if( toplevel){ int is_string= 0;{ void*
_temp833=( void*) e->r; void* _temp839; struct _tagged_string _temp841; _LL835:
if(*(( int*) _temp833) == Cyc_Absyn_Const_e){ _LL840: _temp839=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp833)->f1; if(( unsigned int) _temp839 > 1u?*((
int*) _temp839) == Cyc_Absyn_String_c: 0){ _LL842: _temp841=(( struct Cyc_Absyn_String_c_struct*)
_temp839)->f1; goto _LL836;} else{ goto _LL837;}} else{ goto _LL837;} _LL837:
goto _LL838; _LL836: is_string= 1; goto _LL834; _LL838: goto _LL834; _LL834:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp846=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp846[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp847; _temp847.tag= Cyc_Absyn_ArrayType; _temp847.f1=(
void*) elt_typ; _temp847.f2= Cyc_Toc_mt_tq; _temp847.f3=( struct Cyc_Absyn_Exp*)
sz; _temp847;}); _temp846;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp843=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp843->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp844=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp844[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp845; _temp845.tag= Cyc_Absyn_Var_d;
_temp845.f1= vd; _temp845;}); _temp844;}), 0); _temp843->tl= Cyc_Toc_result_decls;
_temp843;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List*(* _temp848)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)(
struct _tagged_ptr3)) Cyc_List_list; struct _tuple4* _temp851=({ struct _tuple4*
_temp856=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp856->f1= 0;
_temp856->f2= xexp; _temp856;}); struct _tuple4* _temp852=({ struct _tuple4*
_temp855=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp855->f1= 0;
_temp855->f2= xexp; _temp855;}); struct _tuple4* _temp853=({ struct _tuple4*
_temp854=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp854->f1= 0;
_temp854->f2= xplussz; _temp854;}); struct _tuple4* _temp849[ 3u]={ _temp851,
_temp852, _temp853}; struct _tagged_ptr3 _temp850={ _temp849, _temp849, _temp849
+ 3u}; _temp848( _temp850);}), 0); return urm_exp;}}} else{ struct Cyc_Absyn_Stmt*
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
unsigned char* _temp857="can't coerce t? to t* or t@ at the top-level"; struct
_tagged_string _temp858; _temp858.curr= _temp857; _temp858.base= _temp857;
_temp858.last_plus_one= _temp857 + 45; _temp858;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
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
_temp859=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct Cyc_Toc_Env));
_temp859->break_lab= 0; _temp859->continue_lab= 0; _temp859->fallthru_info= 0;
_temp859->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp859->toplevel= 1;
_temp859;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp860=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp860->break_lab= e->break_lab; _temp860->continue_lab=
e->continue_lab; _temp860->fallthru_info= e->fallthru_info; _temp860->varmap= e->varmap;
_temp860->toplevel= e->toplevel; _temp860;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp861=(* x).f1; struct
Cyc_List_List* _temp867; _LL863: if(( unsigned int) _temp861 > 1u?*(( int*)
_temp861) == Cyc_Absyn_Rel_n: 0){ _LL868: _temp867=(( struct Cyc_Absyn_Rel_n_struct*)
_temp861)->f1; goto _LL864;} else{ goto _LL865;} _LL865: goto _LL866; _LL864:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp869=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp869[ 0]=({ struct
Cyc_Core_Impossible_struct _temp870; _temp870.tag= Cyc_Core_Impossible; _temp870.f1=({
struct _tagged_string _temp871= Cyc_Absynpp_qvar2string( x); xprintf("Toc::add_varmap on Rel_n: %.*s\n",
_temp871.last_plus_one - _temp871.curr, _temp871.curr);}); _temp870;}); _temp869;}));
_LL866: goto _LL862; _LL862:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap, x, y); return
ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple8{ struct _tagged_string* f1; struct Cyc_List_List*
f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l, struct _tagged_string*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp872= 0; goto _LL873; _LL873: for( 0;
fallthru_binders != 0; fallthru_binders=(( struct Cyc_List_List*) _check_null(
fallthru_binders))->tl){ _temp872=({ struct Cyc_List_List* _temp874=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp874->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp874->tl=
_temp872; _temp874;});} _temp872=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp872);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp875=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp875->v=( void*) break_l; _temp875;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp876=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp876->v=( void*)({ struct _tuple8*
_temp877=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp877->f1=
fallthru_l; _temp877->f2= _temp872; _temp877->f3= next_case_env->varmap;
_temp877;}); _temp876;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp878=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp878->v=( void*)
break_l; _temp878;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_string*
next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp879=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp879->v=( void*)({ struct _tuple8* _temp880=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp880->f1= next_l; _temp880->f2= 0;
_temp880->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp880;}); _temp879;});
return ans;} struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct _tagged_ptr4{ struct Cyc_List_List** curr; struct Cyc_List_List** base;
struct Cyc_List_List** last_plus_one; } ; struct _tuple10{ struct _tuple0* f1;
void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp881="expecting a literal format string";
struct _tagged_string _temp882; _temp882.curr= _temp881; _temp882.base= _temp881;
_temp882.last_plus_one= _temp881 + 34; _temp882;}));}{ struct _tagged_string
fmt_str;{ void* _temp883=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd)->r; void* _temp889; struct _tagged_string _temp891;
_LL885: if(*(( int*) _temp883) == Cyc_Absyn_Const_e){ _LL890: _temp889=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp883)->f1; if(( unsigned int) _temp889 > 1u?*((
int*) _temp889) == Cyc_Absyn_String_c: 0){ _LL892: _temp891=(( struct Cyc_Absyn_String_c_struct*)
_temp889)->f1; goto _LL886;} else{ goto _LL887;}} else{ goto _LL887;} _LL887:
goto _LL888; _LL886: fmt_str= _temp891; goto _LL884; _LL888: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp893="expecting a literal format string";
struct _tagged_string _temp894; _temp894.curr= _temp893; _temp894.base= _temp893;
_temp894.last_plus_one= _temp893 + 34; _temp894;})); _LL884:;}{ int len=( int)({
struct _tagged_string _temp990= fmt_str;( unsigned int)( _temp990.last_plus_one
- _temp990.curr);}); int i= 0; for( i= 0; i < len; i ++){ if(*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
fmt_str, sizeof( unsigned char), i) =='%'){ i ++; if( i < len?*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
fmt_str, sizeof( unsigned char), i) =='s': 0){ break;}}} if( i == len){ if( aopt
!= 0){ args=({ struct Cyc_List_List* _temp895=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp895->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp895->tl= args; _temp895;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp896= _new_region(); struct
_RegionHandle* r=& _temp896; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str, sizeof(
unsigned char), i); rev_fmt=({ struct Cyc_List_List* _temp897=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp897->hd=( void*)(( int)
c); _temp897->tl= rev_fmt; _temp897;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt*
_temp898= Cyc_Formatstr_parse_conversionspecification( fmt_str, i + 1); goto
_LL899; _LL899: if( _temp898 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp900="bad format string"; struct _tagged_string _temp901;
_temp901.curr= _temp900; _temp901.base= _temp900; _temp901.last_plus_one=
_temp900 + 18; _temp901;}));}{ struct _tuple9 _temp904; int _temp905;
unsigned char _temp907; struct Cyc_List_List* _temp909; struct Cyc_List_List*
_temp911; struct Cyc_List_List* _temp913; struct Cyc_List_List* _temp915; struct
_tuple9* _temp902=( struct _tuple9*)(( struct Cyc_Core_Opt*) _check_null(
_temp898))->v; _temp904=* _temp902; _LL916: _temp915= _temp904.f1; goto _LL914;
_LL914: _temp913= _temp904.f2; goto _LL912; _LL912: _temp911= _temp904.f3; goto
_LL910; _LL910: _temp909= _temp904.f4; goto _LL908; _LL908: _temp907= _temp904.f5;
goto _LL906; _LL906: _temp905= _temp904.f6; goto _LL903; _LL903: i= _temp905 - 1;
if( _temp907 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp917)( struct _RegionHandle*, struct
_tagged_ptr4)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_ptr4)) Cyc_List_rlist; struct _RegionHandle* _temp924= r; struct Cyc_List_List*
_temp920= _temp915; struct Cyc_List_List* _temp921= _temp913; struct Cyc_List_List*
_temp922= _temp911; struct Cyc_List_List* _temp923= _temp909; struct Cyc_List_List*
_temp918[ 4u]={ _temp920, _temp921, _temp922, _temp923}; struct _tagged_ptr4
_temp919={ _temp918, _temp918, _temp918 + 4u}; _temp917( _temp924, _temp919);})),
rev_fmt); rev_fmt=({ struct Cyc_List_List* _temp925=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp925->hd=( void*)(( int)
_temp907); _temp925->tl= rev_fmt; _temp925;});{ struct Cyc_List_List* _temp926=
_temp913; struct Cyc_List_List _temp933; struct Cyc_List_List* _temp934; int
_temp936; _LL928: if( _temp926 == 0){ goto _LL930;} else{ _temp933=* _temp926;
_LL937: _temp936=( int) _temp933.hd; goto _LL935; _LL935: _temp934= _temp933.tl;
if( _temp934 == 0){ goto _LL932;} else{ goto _LL930;}} _LL932: if( _temp936 =='*'){
goto _LL929;} else{ goto _LL930;} _LL930: goto _LL931; _LL929: { struct _tuple0*
_temp938= Cyc_Toc_temp_var(); goto _LL939; _LL939: rev_temps=({ struct Cyc_List_List*
_temp940=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp940->hd=( void*)({ struct _tuple10* _temp941=( struct _tuple10*)
_region_malloc( r, sizeof( struct _tuple10)); _temp941->f1= _temp938; _temp941->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp941->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp941;});
_temp940->tl= rev_temps; _temp940;}); rev_result=({ struct Cyc_List_List*
_temp942=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp942->hd=( void*) Cyc_Absyn_var_exp( _temp938, 0); _temp942->tl= rev_result;
_temp942;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL927;} _LL931: goto
_LL927; _LL927:;}{ struct Cyc_List_List* _temp943= _temp911; struct Cyc_List_List
_temp950; struct Cyc_List_List* _temp951; struct Cyc_List_List _temp953; struct
Cyc_List_List* _temp954; int _temp956; int _temp958; _LL945: if( _temp943 == 0){
goto _LL947;} else{ _temp950=* _temp943; _LL959: _temp958=( int) _temp950.hd;
goto _LL952; _LL952: _temp951= _temp950.tl; if( _temp951 == 0){ goto _LL947;}
else{ _temp953=* _temp951; _LL957: _temp956=( int) _temp953.hd; goto _LL955;
_LL955: _temp954= _temp953.tl; if( _temp954 == 0){ goto _LL949;} else{ goto
_LL947;}}} _LL949: if( _temp958 =='.'? _temp956 =='*': 0){ goto _LL946;} else{
goto _LL947;} _LL947: goto _LL948; _LL946: { struct _tuple0* _temp960= Cyc_Toc_temp_var();
goto _LL961; _LL961: rev_temps=({ struct Cyc_List_List* _temp962=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp962->hd=( void*)({
struct _tuple10* _temp963=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp963->f1= _temp960; _temp963->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp963->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp963;}); _temp962->tl= rev_temps; _temp962;});
rev_result=({ struct Cyc_List_List* _temp964=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp964->hd=( void*) Cyc_Absyn_var_exp(
_temp960, 0); _temp964->tl= rev_result; _temp964;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL944;} _LL948: goto _LL944; _LL944:;} if( _temp907 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp965= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp965.last_plus_one - _temp965.curr, _temp965.curr);}));}{ struct _tuple0*
_temp966= Cyc_Toc_temp_var(); goto _LL967; _LL967: rev_temps=({ struct Cyc_List_List*
_temp968=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp968->hd=( void*)({ struct _tuple10* _temp969=( struct _tuple10*)
_region_malloc( r, sizeof( struct _tuple10)); _temp969->f1= _temp966; _temp969->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp969->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp969;});
_temp968->tl= rev_temps; _temp968;}); rev_result=({ struct Cyc_List_List*
_temp970=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp970->hd=( void*) Cyc_Absyn_var_exp( _temp966, 0); _temp970->tl= rev_result;
_temp970;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl;}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp971=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp971->hd=( void*)(( int)'.'); _temp971->tl= rev_fmt; _temp971;}); rev_fmt=({
struct Cyc_List_List* _temp972=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp972->hd=( void*)(( int)'*'); _temp972->tl=
rev_fmt; _temp972;}); rev_fmt=({ struct Cyc_List_List* _temp973=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp973->hd=( void*)(( int)'s');
_temp973->tl= rev_fmt; _temp973;});{ struct _tuple0* _temp974= Cyc_Toc_temp_var();
goto _LL975; _LL975: rev_temps=({ struct Cyc_List_List* _temp976=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp976->hd=( void*)({
struct _tuple10* _temp977=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp977->f1= _temp974; _temp977->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp977->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp977;}); _temp976->tl= rev_temps; _temp976;});{
struct Cyc_Absyn_Exp* _temp978= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp974, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp974, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL979; _LL979: {
struct Cyc_Absyn_Exp* _temp980= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp974, 0), Cyc_Toc_curr_sp, 0); goto _LL981; _LL981: rev_result=({ struct Cyc_List_List*
_temp982=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp982->hd=( void*) _temp978; _temp982->tl= rev_result; _temp982;});
rev_result=({ struct Cyc_List_List* _temp983=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp983->hd=( void*)
_temp980; _temp983->tl= rev_result; _temp983;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}}{
struct Cyc_List_List* _temp985=({ struct Cyc_List_List* _temp984=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp984->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp984->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( rev_result); _temp984;}); goto _LL986; _LL986: if( aopt != 0){
_temp985=({ struct Cyc_List_List* _temp987=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp987->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp987->tl= _temp985; _temp987;});}{
struct Cyc_Absyn_Exp* _temp988= Cyc_Absyn_primop_exp( p, _temp985, 0); goto
_LL989; _LL989: s= Cyc_Absyn_exp_stmt( _temp988, 0); for( 0; rev_temps != 0;
rev_temps=(( struct Cyc_List_List*) _check_null( rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f1,(*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f2,(
struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f3, s, 0);}}}}; _pop_region(& _temp896);} return
Cyc_Absyn_stmt_exp( s, 0);}}}} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp991="Missing type in primop "; struct _tagged_string
_temp992; _temp992.curr= _temp991; _temp992.base= _temp991; _temp992.last_plus_one=
_temp991 + 24; _temp992;}));} return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp993="Missing type in primop "; struct _tagged_string
_temp994; _temp994.curr= _temp993; _temp994.base= _temp993; _temp994.last_plus_one=
_temp993 + 24; _temp994;}));} return( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp* e){
return({ struct _tuple3* _temp995=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp995->f1= Cyc_Toc_mt_tq; _temp995->f2= Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp995;});} static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e); static struct _tuple4* Cyc_Toc_add_designator(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e);
return({ struct _tuple4* _temp996=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4) * 1); _temp996[ 0]=({ struct _tuple4 _temp997; _temp997.f1= 0; _temp997.f2=
e; _temp997;}); _temp996;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct(
struct Cyc_Toc_Env* nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt*
s, int pointer, struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp*
eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{
struct Cyc_Absyn_Exp* _temp998= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); goto
_LL999; _LL999: if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp998): Cyc_Toc_malloc_ptr( _temp998), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp998), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp1000=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); goto _LL1001; _LL1001: for( 0;
_temp1000 != 0; _temp1000=(( struct Cyc_List_List*) _check_null( _temp1000))->tl){
struct _tuple4 _temp1004; struct Cyc_Absyn_Exp* _temp1005; struct Cyc_List_List*
_temp1007; struct _tuple4* _temp1002=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1000))->hd; _temp1004=* _temp1002; _LL1008: _temp1007=
_temp1004.f1; goto _LL1006; _LL1006: _temp1005= _temp1004.f2; goto _LL1003;
_LL1003: { struct Cyc_Absyn_Exp* e_index; if( _temp1007 == 0){ e_index= Cyc_Absyn_signed_int_exp(
count --, 0);} else{ if((( struct Cyc_List_List*) _check_null( _temp1007))->tl
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp1009="multiple designators in array"; struct _tagged_string _temp1010;
_temp1010.curr= _temp1009; _temp1010.base= _temp1009; _temp1010.last_plus_one=
_temp1009 + 30; _temp1010;}));}{ void* _temp1011=( void*)(( struct Cyc_List_List*)
_check_null( _temp1007))->hd; goto _LL1012; _LL1012: { void* _temp1013=
_temp1011; struct Cyc_Absyn_Exp* _temp1019; struct _tagged_string* _temp1021;
_LL1015: if(*(( int*) _temp1013) == Cyc_Absyn_ArrayElement){ _LL1020: _temp1019=((
struct Cyc_Absyn_ArrayElement_struct*) _temp1013)->f1; goto _LL1016;} else{ goto
_LL1017;} _LL1017: if(*(( int*) _temp1013) == Cyc_Absyn_FieldName){ _LL1022:
_temp1021=(( struct Cyc_Absyn_FieldName_struct*) _temp1013)->f1; goto _LL1018;}
else{ goto _LL1014;} _LL1016: Cyc_Toc_exp_to_c( nv, _temp1019); e_index=
_temp1019; goto _LL1014; _LL1018: e_index=(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp1023="field name designators in array";
struct _tagged_string _temp1024; _temp1024.curr= _temp1023; _temp1024.base=
_temp1023; _temp1024.last_plus_one= _temp1023 + 32; _temp1024;})); goto _LL1014;
_LL1014:;}}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index,
0); void* _temp1025=( void*) _temp1005->r; struct Cyc_List_List* _temp1035;
struct Cyc_Absyn_Exp* _temp1037; struct Cyc_Absyn_Exp* _temp1039; struct Cyc_Absyn_Vardecl*
_temp1041; struct Cyc_List_List* _temp1043; void* _temp1045; _LL1027: if(*(( int*)
_temp1025) == Cyc_Absyn_Array_e){ _LL1036: _temp1035=(( struct Cyc_Absyn_Array_e_struct*)
_temp1025)->f1; goto _LL1028;} else{ goto _LL1029;} _LL1029: if(*(( int*)
_temp1025) == Cyc_Absyn_Comprehension_e){ _LL1042: _temp1041=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1025)->f1; goto _LL1040; _LL1040: _temp1039=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1025)->f2; goto _LL1038; _LL1038: _temp1037=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1025)->f3; goto _LL1030;} else{ goto _LL1031;} _LL1031: if(*(( int*)
_temp1025) == Cyc_Absyn_AnonStruct_e){ _LL1046: _temp1045=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1025)->f1; goto _LL1044; _LL1044: _temp1043=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1025)->f2; goto _LL1032;} else{ goto _LL1033;} _LL1033: goto _LL1034;
_LL1028: s= Cyc_Toc_init_array( nv, lval, _temp1035, s); goto _LL1026; _LL1030:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1041, _temp1039, _temp1037, s, 0);
goto _LL1026; _LL1032: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1045,
_temp1043, s); goto _LL1026; _LL1034: Cyc_Toc_exp_to_c( nv, _temp1005); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp1005, 0),
s, 0); goto _LL1026; _LL1026:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* _temp1047= vd->name; goto _LL1048;
_LL1048: if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp1047, Cyc_Absyn_varb_exp( _temp1047,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1071=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1071[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1072; _temp1072.tag= Cyc_Absyn_Local_b; _temp1072.f1=
vd; _temp1072;}); _temp1071;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp1047, 0),
Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( _temp1047, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp*
ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( _temp1047, 0), 0); struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( _temp1047, 0), 0); struct
Cyc_Absyn_Stmt* body;{ void* _temp1049=( void*) e2->r; struct Cyc_List_List*
_temp1059; struct Cyc_Absyn_Exp* _temp1061; struct Cyc_Absyn_Exp* _temp1063;
struct Cyc_Absyn_Vardecl* _temp1065; struct Cyc_List_List* _temp1067; void*
_temp1069; _LL1051: if(*(( int*) _temp1049) == Cyc_Absyn_Array_e){ _LL1060:
_temp1059=(( struct Cyc_Absyn_Array_e_struct*) _temp1049)->f1; goto _LL1052;}
else{ goto _LL1053;} _LL1053: if(*(( int*) _temp1049) == Cyc_Absyn_Comprehension_e){
_LL1066: _temp1065=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1049)->f1;
goto _LL1064; _LL1064: _temp1063=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1049)->f2; goto _LL1062; _LL1062: _temp1061=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1049)->f3; goto _LL1054;} else{ goto _LL1055;} _LL1055: if(*(( int*)
_temp1049) == Cyc_Absyn_AnonStruct_e){ _LL1070: _temp1069=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1049)->f1; goto _LL1068; _LL1068: _temp1067=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1049)->f2; goto _LL1056;} else{ goto _LL1057;} _LL1057: goto _LL1058;
_LL1052: body= Cyc_Toc_init_array( nv2, lval, _temp1059, Cyc_Absyn_skip_stmt( 0));
goto _LL1050; _LL1054: body= Cyc_Toc_init_comprehension( nv2, lval, _temp1065,
_temp1063, _temp1061, Cyc_Absyn_skip_stmt( 0), 0); goto _LL1050; _LL1056: body=
Cyc_Toc_init_anon_struct( nv, lval, _temp1069, _temp1067, Cyc_Absyn_skip_stmt( 0));
goto _LL1050; _LL1058: Cyc_Toc_exp_to_c( nv2, e2); body= Cyc_Absyn_assign_stmt(
lval, e2, 0); goto _LL1050; _LL1050:;}{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt(
ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( _temp1047, Cyc_Absyn_uint_t,
0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s){{ struct Cyc_List_List* _temp1073=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto _LL1074; _LL1074: for( 0;
_temp1073 != 0; _temp1073=(( struct Cyc_List_List*) _check_null( _temp1073))->tl){
struct _tuple4 _temp1077; struct Cyc_Absyn_Exp* _temp1078; struct Cyc_List_List*
_temp1080; struct _tuple4* _temp1075=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1073))->hd; _temp1077=* _temp1075; _LL1081: _temp1080=
_temp1077.f1; goto _LL1079; _LL1079: _temp1078= _temp1077.f2; goto _LL1076;
_LL1076: if( _temp1080 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1082="empty designator list"; struct _tagged_string
_temp1083; _temp1083.curr= _temp1082; _temp1083.base= _temp1082; _temp1083.last_plus_one=
_temp1082 + 22; _temp1083;}));} if((( struct Cyc_List_List*) _check_null(
_temp1080))->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1084="too many designators in anonymous struct"; struct
_tagged_string _temp1085; _temp1085.curr= _temp1084; _temp1085.base= _temp1084;
_temp1085.last_plus_one= _temp1084 + 41; _temp1085;}));}{ void* _temp1086=( void*)((
struct Cyc_List_List*) _check_null( _temp1080))->hd; struct _tagged_string*
_temp1092; _LL1088: if(*(( int*) _temp1086) == Cyc_Absyn_FieldName){ _LL1093:
_temp1092=(( struct Cyc_Absyn_FieldName_struct*) _temp1086)->f1; goto _LL1089;}
else{ goto _LL1090;} _LL1090: goto _LL1091; _LL1089: { struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_structmember_exp( lhs, _temp1092, 0);{ void* _temp1094=( void*)
_temp1078->r; struct Cyc_List_List* _temp1104; struct Cyc_Absyn_Exp* _temp1106;
struct Cyc_Absyn_Exp* _temp1108; struct Cyc_Absyn_Vardecl* _temp1110; struct Cyc_List_List*
_temp1112; void* _temp1114; _LL1096: if(*(( int*) _temp1094) == Cyc_Absyn_Array_e){
_LL1105: _temp1104=(( struct Cyc_Absyn_Array_e_struct*) _temp1094)->f1; goto
_LL1097;} else{ goto _LL1098;} _LL1098: if(*(( int*) _temp1094) == Cyc_Absyn_Comprehension_e){
_LL1111: _temp1110=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1094)->f1;
goto _LL1109; _LL1109: _temp1108=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1094)->f2; goto _LL1107; _LL1107: _temp1106=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1094)->f3; goto _LL1099;} else{ goto _LL1100;} _LL1100: if(*(( int*)
_temp1094) == Cyc_Absyn_AnonStruct_e){ _LL1115: _temp1114=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1094)->f1; goto _LL1113; _LL1113: _temp1112=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1094)->f2; goto _LL1101;} else{ goto _LL1102;} _LL1102: goto _LL1103;
_LL1097: s= Cyc_Toc_init_array( nv, lval, _temp1104, s); goto _LL1095; _LL1099:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1110, _temp1108, _temp1106, s, 0);
goto _LL1095; _LL1101: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1114,
_temp1112, s); goto _LL1095; _LL1103: Cyc_Toc_exp_to_c( nv, _temp1078); if( Cyc_Toc_is_poly_field(
struct_type, _temp1092)){ _temp1078= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1078, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1078, 0), 0), s, 0); goto _LL1095; _LL1095:;} goto _LL1087;} _LL1091:(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1116="array designator in struct"; struct _tagged_string
_temp1117; _temp1117.curr= _temp1116; _temp1117.base= _temp1116; _temp1117.last_plus_one=
_temp1116 + 27; _temp1117;}))); _LL1087:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp1118= _new_region();
struct _RegionHandle* r=& _temp1118; _push_region( r);{ struct Cyc_List_List*
_temp1119=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(*
f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); goto _LL1120; _LL1120: { struct _tagged_string* _temp1121= Cyc_Toc_add_tuple_type(
_temp1119); goto _LL1122; _LL1122: { struct _tuple0* _temp1123= Cyc_Toc_temp_var();
goto _LL1124; _LL1124: { struct Cyc_Absyn_Exp* _temp1125= Cyc_Absyn_var_exp(
_temp1123, 0); goto _LL1126; _LL1126: { struct Cyc_Absyn_Stmt* _temp1127= Cyc_Absyn_exp_stmt(
_temp1125, 0); goto _LL1128; _LL1128: { struct Cyc_Absyn_Exp*(* _temp1129)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1130;
_LL1130: { int is_atomic= 1; struct Cyc_List_List* _temp1131=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es); goto
_LL1132; _LL1132:{ int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1131); for( 0; _temp1131 != 0;( _temp1131=(( struct Cyc_List_List*)
_check_null( _temp1131))->tl, -- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1131))->hd; struct Cyc_Absyn_Exp* lval=
_temp1129( _temp1125, Cyc_Toc_fieldname( i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v): 0;{ void* _temp1133=(
void*) e->r; struct Cyc_List_List* _temp1141; struct Cyc_Absyn_Exp* _temp1143;
struct Cyc_Absyn_Exp* _temp1145; struct Cyc_Absyn_Vardecl* _temp1147; _LL1135:
if(*(( int*) _temp1133) == Cyc_Absyn_Array_e){ _LL1142: _temp1141=(( struct Cyc_Absyn_Array_e_struct*)
_temp1133)->f1; goto _LL1136;} else{ goto _LL1137;} _LL1137: if(*(( int*)
_temp1133) == Cyc_Absyn_Comprehension_e){ _LL1148: _temp1147=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1133)->f1; goto _LL1146; _LL1146: _temp1145=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1133)->f2; goto _LL1144; _LL1144: _temp1143=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1133)->f3; goto _LL1138;} else{ goto _LL1139;} _LL1139: goto _LL1140;
_LL1136: _temp1127= Cyc_Toc_init_array( nv, lval, _temp1141, _temp1127); goto
_LL1134; _LL1138: _temp1127= Cyc_Toc_init_comprehension( nv, lval, _temp1147,
_temp1145, _temp1143, _temp1127, 0); goto _LL1134; _LL1140: Cyc_Toc_exp_to_c( nv,
e); _temp1127= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1129( _temp1125, Cyc_Toc_fieldname( i), 0), e, 0), 0), _temp1127, 0); goto
_LL1134; _LL1134:;}}}{ struct Cyc_Absyn_Exp* _temp1149= Cyc_Toc_make_struct( nv,
_temp1123, Cyc_Absyn_strct( _temp1121), _temp1127, pointer, rgnopt, is_atomic);
_npop_handler( 0u); return _temp1149;}}}}}}}}; _pop_region(& _temp1118);} static
struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env* nv, void*
struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp1150= Cyc_Toc_temp_var(); goto
_LL1151; _LL1151: { struct Cyc_Absyn_Exp* _temp1152= Cyc_Absyn_var_exp(
_temp1150, 0); goto _LL1153; _LL1153: { struct Cyc_Absyn_Stmt* _temp1154= Cyc_Absyn_exp_stmt(
_temp1152, 0); goto _LL1155; _LL1155: { struct Cyc_Absyn_Exp*(* _temp1156)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1157;
_LL1157: { int is_atomic= 1; struct _RegionHandle _temp1158= _new_region();
struct _RegionHandle* r=& _temp1158; _push_region( r);{ struct Cyc_List_List*
_temp1159=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, dles); goto _LL1160; _LL1160: for( 0; _temp1159 != 0;
_temp1159=(( struct Cyc_List_List*) _check_null( _temp1159))->tl){ struct
_tuple4 _temp1163; struct Cyc_Absyn_Exp* _temp1164; struct Cyc_List_List*
_temp1166; struct _tuple4* _temp1161=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1159))->hd; _temp1163=* _temp1161; _LL1167: _temp1166=
_temp1163.f1; goto _LL1165; _LL1165: _temp1164= _temp1163.f2; goto _LL1162;
_LL1162: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1164->topt))->v): 0; if( _temp1166 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1168="empty designator list";
struct _tagged_string _temp1169; _temp1169.curr= _temp1168; _temp1169.base=
_temp1168; _temp1169.last_plus_one= _temp1168 + 22; _temp1169;}));} if((( struct
Cyc_List_List*) _check_null( _temp1166))->tl != 0){ struct _tuple0* _temp1170=
Cyc_Toc_temp_var(); goto _LL1171; _LL1171: { struct Cyc_Absyn_Exp* _temp1172=
Cyc_Absyn_var_exp( _temp1170, 0); goto _LL1173; _LL1173: for( 0; _temp1166 != 0;
_temp1166=(( struct Cyc_List_List*) _check_null( _temp1166))->tl){ void*
_temp1174=( void*)(( struct Cyc_List_List*) _check_null( _temp1166))->hd; struct
_tagged_string* _temp1180; _LL1176: if(*(( int*) _temp1174) == Cyc_Absyn_FieldName){
_LL1181: _temp1180=(( struct Cyc_Absyn_FieldName_struct*) _temp1174)->f1; goto
_LL1177;} else{ goto _LL1178;} _LL1178: goto _LL1179; _LL1177: if( Cyc_Toc_is_poly_field(
struct_type, _temp1180)){ _temp1172= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1172, 0);} _temp1154= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1156( _temp1152, _temp1180, 0), _temp1172, 0), 0), _temp1154, 0); goto
_LL1175; _LL1179:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1182="array designator in struct"; struct _tagged_string
_temp1183; _temp1183.curr= _temp1182; _temp1183.base= _temp1182; _temp1183.last_plus_one=
_temp1182 + 27; _temp1183;}))); _LL1175:;} Cyc_Toc_exp_to_c( nv, _temp1164);
_temp1154= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1172, _temp1164, 0), 0), _temp1154, 0);}} else{ void* _temp1184=( void*)((
struct Cyc_List_List*) _check_null( _temp1166))->hd; struct _tagged_string*
_temp1190; _LL1186: if(*(( int*) _temp1184) == Cyc_Absyn_FieldName){ _LL1191:
_temp1190=(( struct Cyc_Absyn_FieldName_struct*) _temp1184)->f1; goto _LL1187;}
else{ goto _LL1188;} _LL1188: goto _LL1189; _LL1187: { struct Cyc_Absyn_Exp*
lval= _temp1156( _temp1152, _temp1190, 0);{ void* _temp1192=( void*) _temp1164->r;
struct Cyc_List_List* _temp1202; struct Cyc_Absyn_Exp* _temp1204; struct Cyc_Absyn_Exp*
_temp1206; struct Cyc_Absyn_Vardecl* _temp1208; struct Cyc_List_List* _temp1210;
void* _temp1212; _LL1194: if(*(( int*) _temp1192) == Cyc_Absyn_Array_e){ _LL1203:
_temp1202=(( struct Cyc_Absyn_Array_e_struct*) _temp1192)->f1; goto _LL1195;}
else{ goto _LL1196;} _LL1196: if(*(( int*) _temp1192) == Cyc_Absyn_Comprehension_e){
_LL1209: _temp1208=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1192)->f1;
goto _LL1207; _LL1207: _temp1206=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1192)->f2; goto _LL1205; _LL1205: _temp1204=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1192)->f3; goto _LL1197;} else{ goto _LL1198;} _LL1198: if(*(( int*)
_temp1192) == Cyc_Absyn_AnonStruct_e){ _LL1213: _temp1212=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1192)->f1; goto _LL1211; _LL1211: _temp1210=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1192)->f2; goto _LL1199;} else{ goto _LL1200;} _LL1200: goto _LL1201;
_LL1195: _temp1154= Cyc_Toc_init_array( nv, lval, _temp1202, _temp1154); goto
_LL1193; _LL1197: _temp1154= Cyc_Toc_init_comprehension( nv, lval, _temp1208,
_temp1206, _temp1204, _temp1154, 0); goto _LL1193; _LL1199: _temp1154= Cyc_Toc_init_anon_struct(
nv, lval, _temp1212, _temp1210, _temp1154); goto _LL1193; _LL1201: Cyc_Toc_exp_to_c(
nv, _temp1164); if( Cyc_Toc_is_poly_field( struct_type, _temp1190)){ _temp1164=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1164, 0);} _temp1154= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval, _temp1164, 0), 0), _temp1154, 0);
goto _LL1193; _LL1193:;} goto _LL1185;} _LL1189:( void) _throw((( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1214="array designator in struct";
struct _tagged_string _temp1215; _temp1215.curr= _temp1214; _temp1215.base=
_temp1214; _temp1215.last_plus_one= _temp1214 + 27; _temp1215;}))); _LL1185:;}}}{
struct Cyc_Absyn_Exp* _temp1216= Cyc_Toc_make_struct( nv, _temp1150, Cyc_Absyn_strctq(
tdn), _temp1154, pointer, rgnopt, is_atomic); _npop_handler( 0u); return
_temp1216;}; _pop_region(& _temp1158);}}}}} struct _tuple11{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp(
el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp1217=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp1217[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp1218; _temp1218.tag=
Cyc_Absyn_Increment_e; _temp1218.f1= e1; _temp1218.f2=( void*) incr; _temp1218;});
_temp1217;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1219=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp1229; struct Cyc_Absyn_Exp* _temp1231; void* _temp1233; struct
_tagged_string* _temp1235; struct Cyc_Absyn_Exp* _temp1237; _LL1221: if(*(( int*)
_temp1219) == Cyc_Absyn_StmtExp_e){ _LL1230: _temp1229=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1219)->f1; goto _LL1222;} else{ goto _LL1223;} _LL1223: if(*(( int*)
_temp1219) == Cyc_Absyn_Cast_e){ _LL1234: _temp1233=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1219)->f1; goto _LL1232; _LL1232: _temp1231=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1219)->f2; goto _LL1224;} else{ goto _LL1225;} _LL1225: if(*(( int*)
_temp1219) == Cyc_Absyn_StructMember_e){ _LL1238: _temp1237=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1219)->f1; goto _LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1219)->f2; goto _LL1226;} else{ goto _LL1227;} _LL1227: goto _LL1228;
_LL1222:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1229, fs, f, f_env); goto _LL1220; _LL1224:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1231, fs, f, f_env); goto
_LL1220; _LL1226:( void*)( e1->r=( void*)(( void*) _temp1237->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1239=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1239->hd=( void*) _temp1235; _temp1239->tl= fs;
_temp1239;}), f, f_env); goto _LL1220; _LL1228: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1240= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1240.last_plus_one - _temp1240.curr, _temp1240.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1220;}
_LL1220:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1241=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1251; struct Cyc_Absyn_Stmt* _temp1253; struct Cyc_Absyn_Decl* _temp1255;
struct Cyc_Absyn_Stmt* _temp1257; struct Cyc_Absyn_Stmt* _temp1259; _LL1243: if((
unsigned int) _temp1241 > 1u?*(( int*) _temp1241) == Cyc_Absyn_Exp_s: 0){
_LL1252: _temp1251=(( struct Cyc_Absyn_Exp_s_struct*) _temp1241)->f1; goto
_LL1244;} else{ goto _LL1245;} _LL1245: if(( unsigned int) _temp1241 > 1u?*((
int*) _temp1241) == Cyc_Absyn_Decl_s: 0){ _LL1256: _temp1255=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1241)->f1; goto _LL1254; _LL1254: _temp1253=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1241)->f2; goto _LL1246;} else{ goto _LL1247;} _LL1247: if(( unsigned int)
_temp1241 > 1u?*(( int*) _temp1241) == Cyc_Absyn_Seq_s: 0){ _LL1260: _temp1259=((
struct Cyc_Absyn_Seq_s_struct*) _temp1241)->f1; goto _LL1258; _LL1258: _temp1257=((
struct Cyc_Absyn_Seq_s_struct*) _temp1241)->f2; goto _LL1248;} else{ goto
_LL1249;} _LL1249: goto _LL1250; _LL1244:(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1251, fs, f, f_env); goto
_LL1242; _LL1246:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1253, fs, f, f_env); goto _LL1242; _LL1248:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1257, fs, f, f_env);
goto _LL1242; _LL1250:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1261= Cyc_Absynpp_stmt2string( s); xprintf("lvalue_assign_stmt: %.*s",
_temp1261.last_plus_one - _temp1261.curr, _temp1261.curr);})); goto _LL1242;
_LL1242:;} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
s); static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct _RegionHandle* r2, void*(*
f)( void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List*
result; struct Cyc_List_List* prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List*
_temp1262=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp1262->hd=( void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,
env); _temp1262->tl= 0; _temp1262;}); prev= result; for( x=(( struct Cyc_List_List*)
_check_null( x))->tl; x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){((
struct Cyc_List_List*) _check_null( prev))->tl=({ struct Cyc_List_List*
_temp1263=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp1263->hd=( void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,
env); _temp1263->tl= 0; _temp1263;}); prev=(( struct Cyc_List_List*) _check_null(
prev))->tl;} return result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(*
f)( void*, void*), void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4*
Cyc_Toc_make_dle( struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1264=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1264->f1= 0;
_temp1264->f2= e; _temp1264;});} struct _tagged_ptr5{ struct _tuple0** curr;
struct _tuple0** base; struct _tuple0** last_plus_one; } ; struct _tagged_ptr6{
struct Cyc_Absyn_Exp** curr; struct Cyc_Absyn_Exp** base; struct Cyc_Absyn_Exp**
last_plus_one; } ; struct _tagged_ptr7{ struct _tuple1** curr; struct _tuple1**
base; struct _tuple1** last_plus_one; } ; struct _tuple12{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp1265=( void*) e->r; goto _LL1266;
_LL1266: if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1267= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp1267.last_plus_one - _temp1267.curr, _temp1267.curr);})); return;}{ void*
old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void*
_temp1268= _temp1265; void* _temp1346; void* _temp1348; void* _temp1350; struct
_tuple0* _temp1352; struct _tuple0* _temp1354; struct Cyc_List_List* _temp1356;
void* _temp1358; void* _temp1360; struct Cyc_Absyn_Exp* _temp1362; struct Cyc_Absyn_Exp*
_temp1364; struct Cyc_Core_Opt* _temp1366; struct Cyc_Absyn_Exp* _temp1368;
struct Cyc_Absyn_Exp* _temp1370; struct Cyc_Absyn_Exp* _temp1372; struct Cyc_Absyn_Exp*
_temp1374; struct Cyc_Absyn_Exp* _temp1376; struct Cyc_Absyn_Exp* _temp1378;
struct Cyc_List_List* _temp1380; struct Cyc_Absyn_Exp* _temp1382; struct Cyc_Absyn_VarargCallInfo*
_temp1384; struct Cyc_List_List* _temp1386; struct Cyc_Absyn_Exp* _temp1388;
struct Cyc_Absyn_VarargCallInfo* _temp1390; struct Cyc_Absyn_VarargCallInfo
_temp1392; struct Cyc_Absyn_VarargInfo* _temp1393; struct Cyc_List_List*
_temp1395; int _temp1397; struct Cyc_List_List* _temp1399; struct Cyc_Absyn_Exp*
_temp1401; struct Cyc_Absyn_Exp* _temp1403; struct Cyc_Absyn_Exp* _temp1405;
struct Cyc_List_List* _temp1407; struct Cyc_Absyn_Exp* _temp1409; struct Cyc_Absyn_Exp*
_temp1411; void* _temp1413; void** _temp1415; struct Cyc_Absyn_Exp* _temp1416;
struct Cyc_Absyn_Exp* _temp1418; struct Cyc_Absyn_Exp* _temp1420; void*
_temp1422; struct Cyc_Absyn_Exp* _temp1424; struct Cyc_Absyn_Exp* _temp1426;
struct _tagged_string* _temp1428; struct Cyc_Absyn_Exp* _temp1430; struct
_tagged_string* _temp1432; struct Cyc_Absyn_Exp* _temp1434; struct Cyc_Absyn_Exp*
_temp1436; struct Cyc_Absyn_Exp* _temp1438; struct Cyc_List_List* _temp1440;
struct Cyc_List_List* _temp1442; struct _tuple1* _temp1444; struct Cyc_List_List*
_temp1446; struct Cyc_Absyn_Exp* _temp1448; struct Cyc_Absyn_Exp* _temp1450;
struct Cyc_Absyn_Vardecl* _temp1452; struct Cyc_Absyn_Structdecl* _temp1454;
struct Cyc_List_List* _temp1456; struct Cyc_Core_Opt* _temp1458; struct _tuple0*
_temp1460; struct Cyc_List_List* _temp1462; void* _temp1464; struct Cyc_Absyn_Tunionfield*
_temp1466; struct Cyc_Absyn_Tuniondecl* _temp1468; struct Cyc_List_List*
_temp1470; struct Cyc_Core_Opt* _temp1472; struct Cyc_Core_Opt* _temp1474;
struct Cyc_Absyn_Tunionfield* _temp1476; struct Cyc_Absyn_Tuniondecl* _temp1478;
struct Cyc_List_List* _temp1480; struct Cyc_Core_Opt* _temp1482; struct Cyc_Core_Opt*
_temp1484; struct Cyc_Absyn_Enumfield* _temp1486; struct Cyc_Absyn_Enumdecl*
_temp1488; struct _tuple0* _temp1490; void* _temp1492; struct Cyc_Absyn_Exp*
_temp1494; struct Cyc_Absyn_Stmt* _temp1496; struct Cyc_List_List* _temp1498;
struct Cyc_Core_Opt* _temp1500; struct Cyc_Absyn_Fndecl* _temp1502; struct Cyc_Absyn_Exp*
_temp1504; _LL1270: if(*(( int*) _temp1268) == Cyc_Absyn_Const_e){ _LL1347:
_temp1346=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1268)->f1; if(
_temp1346 ==( void*) Cyc_Absyn_Null_c){ goto _LL1271;} else{ goto _LL1272;}}
else{ goto _LL1272;} _LL1272: if(*(( int*) _temp1268) == Cyc_Absyn_Const_e){
_LL1349: _temp1348=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1268)->f1;
goto _LL1273;} else{ goto _LL1274;} _LL1274: if(*(( int*) _temp1268) == Cyc_Absyn_Var_e){
_LL1353: _temp1352=(( struct Cyc_Absyn_Var_e_struct*) _temp1268)->f1; goto
_LL1351; _LL1351: _temp1350=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1268)->f2;
goto _LL1275;} else{ goto _LL1276;} _LL1276: if(*(( int*) _temp1268) == Cyc_Absyn_UnknownId_e){
_LL1355: _temp1354=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1268)->f1; goto
_LL1277;} else{ goto _LL1278;} _LL1278: if(*(( int*) _temp1268) == Cyc_Absyn_Primop_e){
_LL1359: _temp1358=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1268)->f1;
goto _LL1357; _LL1357: _temp1356=(( struct Cyc_Absyn_Primop_e_struct*) _temp1268)->f2;
goto _LL1279;} else{ goto _LL1280;} _LL1280: if(*(( int*) _temp1268) == Cyc_Absyn_Increment_e){
_LL1363: _temp1362=(( struct Cyc_Absyn_Increment_e_struct*) _temp1268)->f1; goto
_LL1361; _LL1361: _temp1360=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1268)->f2; goto _LL1281;} else{ goto _LL1282;} _LL1282: if(*(( int*)
_temp1268) == Cyc_Absyn_AssignOp_e){ _LL1369: _temp1368=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1268)->f1; goto _LL1367; _LL1367: _temp1366=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1268)->f2; goto _LL1365; _LL1365: _temp1364=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1268)->f3; goto _LL1283;} else{ goto _LL1284;} _LL1284: if(*(( int*)
_temp1268) == Cyc_Absyn_Conditional_e){ _LL1375: _temp1374=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1268)->f1; goto _LL1373; _LL1373: _temp1372=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1268)->f2; goto _LL1371; _LL1371: _temp1370=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1268)->f3; goto _LL1285;} else{ goto _LL1286;} _LL1286: if(*(( int*)
_temp1268) == Cyc_Absyn_SeqExp_e){ _LL1379: _temp1378=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1268)->f1; goto _LL1377; _LL1377: _temp1376=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1268)->f2; goto _LL1287;} else{ goto _LL1288;} _LL1288: if(*(( int*)
_temp1268) == Cyc_Absyn_UnknownCall_e){ _LL1383: _temp1382=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1268)->f1; goto _LL1381; _LL1381: _temp1380=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1268)->f2; goto _LL1289;} else{ goto _LL1290;} _LL1290: if(*(( int*)
_temp1268) == Cyc_Absyn_FnCall_e){ _LL1389: _temp1388=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1268)->f1; goto _LL1387; _LL1387: _temp1386=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1268)->f2; goto _LL1385; _LL1385: _temp1384=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1268)->f3; if( _temp1384 == 0){ goto _LL1291;} else{ goto _LL1292;}} else{
goto _LL1292;} _LL1292: if(*(( int*) _temp1268) == Cyc_Absyn_FnCall_e){ _LL1402:
_temp1401=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1268)->f1; goto _LL1400;
_LL1400: _temp1399=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1268)->f2; goto
_LL1391; _LL1391: _temp1390=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1268)->f3;
if( _temp1390 == 0){ goto _LL1294;} else{ _temp1392=* _temp1390; _LL1398:
_temp1397= _temp1392.num_varargs; goto _LL1396; _LL1396: _temp1395= _temp1392.injectors;
goto _LL1394; _LL1394: _temp1393= _temp1392.vai; goto _LL1293;}} else{ goto
_LL1294;} _LL1294: if(*(( int*) _temp1268) == Cyc_Absyn_Throw_e){ _LL1404:
_temp1403=(( struct Cyc_Absyn_Throw_e_struct*) _temp1268)->f1; goto _LL1295;}
else{ goto _LL1296;} _LL1296: if(*(( int*) _temp1268) == Cyc_Absyn_NoInstantiate_e){
_LL1406: _temp1405=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1268)->f1;
goto _LL1297;} else{ goto _LL1298;} _LL1298: if(*(( int*) _temp1268) == Cyc_Absyn_Instantiate_e){
_LL1410: _temp1409=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1268)->f1;
goto _LL1408; _LL1408: _temp1407=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1268)->f2; goto _LL1299;} else{ goto _LL1300;} _LL1300: if(*(( int*)
_temp1268) == Cyc_Absyn_Cast_e){ _LL1414: _temp1413=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1268)->f1; _temp1415=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1268)->f1); goto _LL1412; _LL1412: _temp1411=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1268)->f2; goto _LL1301;} else{ goto _LL1302;} _LL1302: if(*(( int*)
_temp1268) == Cyc_Absyn_Address_e){ _LL1417: _temp1416=(( struct Cyc_Absyn_Address_e_struct*)
_temp1268)->f1; goto _LL1303;} else{ goto _LL1304;} _LL1304: if(*(( int*)
_temp1268) == Cyc_Absyn_New_e){ _LL1421: _temp1420=(( struct Cyc_Absyn_New_e_struct*)
_temp1268)->f1; goto _LL1419; _LL1419: _temp1418=(( struct Cyc_Absyn_New_e_struct*)
_temp1268)->f2; goto _LL1305;} else{ goto _LL1306;} _LL1306: if(*(( int*)
_temp1268) == Cyc_Absyn_Sizeoftyp_e){ _LL1423: _temp1422=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1268)->f1; goto _LL1307;} else{ goto _LL1308;} _LL1308: if(*(( int*)
_temp1268) == Cyc_Absyn_Sizeofexp_e){ _LL1425: _temp1424=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1268)->f1; goto _LL1309;} else{ goto _LL1310;} _LL1310: if(*(( int*)
_temp1268) == Cyc_Absyn_Deref_e){ _LL1427: _temp1426=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1268)->f1; goto _LL1311;} else{ goto _LL1312;} _LL1312: if(*(( int*)
_temp1268) == Cyc_Absyn_StructMember_e){ _LL1431: _temp1430=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1268)->f1; goto _LL1429; _LL1429: _temp1428=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1268)->f2; goto _LL1313;} else{ goto _LL1314;} _LL1314: if(*(( int*)
_temp1268) == Cyc_Absyn_StructArrow_e){ _LL1435: _temp1434=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1268)->f1; goto _LL1433; _LL1433: _temp1432=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1268)->f2; goto _LL1315;} else{ goto _LL1316;} _LL1316: if(*(( int*)
_temp1268) == Cyc_Absyn_Subscript_e){ _LL1439: _temp1438=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1268)->f1; goto _LL1437; _LL1437: _temp1436=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1268)->f2; goto _LL1317;} else{ goto _LL1318;} _LL1318: if(*(( int*)
_temp1268) == Cyc_Absyn_Tuple_e){ _LL1441: _temp1440=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1268)->f1; goto _LL1319;} else{ goto _LL1320;} _LL1320: if(*(( int*)
_temp1268) == Cyc_Absyn_CompoundLit_e){ _LL1445: _temp1444=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1268)->f1; goto _LL1443; _LL1443: _temp1442=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1268)->f2; goto _LL1321;} else{ goto _LL1322;} _LL1322: if(*(( int*)
_temp1268) == Cyc_Absyn_Array_e){ _LL1447: _temp1446=(( struct Cyc_Absyn_Array_e_struct*)
_temp1268)->f1; goto _LL1323;} else{ goto _LL1324;} _LL1324: if(*(( int*)
_temp1268) == Cyc_Absyn_Comprehension_e){ _LL1453: _temp1452=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1268)->f1; goto _LL1451; _LL1451: _temp1450=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1268)->f2; goto _LL1449; _LL1449: _temp1448=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1268)->f3; goto _LL1325;} else{ goto _LL1326;} _LL1326: if(*(( int*)
_temp1268) == Cyc_Absyn_Struct_e){ _LL1461: _temp1460=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1268)->f1; goto _LL1459; _LL1459: _temp1458=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1268)->f2; goto _LL1457; _LL1457: _temp1456=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1268)->f3; goto _LL1455; _LL1455: _temp1454=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1268)->f4; goto _LL1327;} else{ goto _LL1328;} _LL1328: if(*(( int*)
_temp1268) == Cyc_Absyn_AnonStruct_e){ _LL1465: _temp1464=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1268)->f1; goto _LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1268)->f2; goto _LL1329;} else{ goto _LL1330;} _LL1330: if(*(( int*)
_temp1268) == Cyc_Absyn_Tunion_e){ _LL1475: _temp1474=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1268)->f1; goto _LL1473; _LL1473: _temp1472=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1268)->f2; goto _LL1471; _LL1471: _temp1470=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1268)->f3; if( _temp1470 == 0){ goto _LL1469;} else{ goto _LL1332;} _LL1469:
_temp1468=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1268)->f4; goto _LL1467;
_LL1467: _temp1466=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1268)->f5; goto
_LL1331;} else{ goto _LL1332;} _LL1332: if(*(( int*) _temp1268) == Cyc_Absyn_Tunion_e){
_LL1485: _temp1484=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1268)->f1; goto
_LL1483; _LL1483: _temp1482=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1268)->f2;
goto _LL1481; _LL1481: _temp1480=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1268)->f3;
goto _LL1479; _LL1479: _temp1478=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1268)->f4;
goto _LL1477; _LL1477: _temp1476=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1268)->f5;
goto _LL1333;} else{ goto _LL1334;} _LL1334: if(*(( int*) _temp1268) == Cyc_Absyn_Enum_e){
_LL1491: _temp1490=(( struct Cyc_Absyn_Enum_e_struct*) _temp1268)->f1; goto
_LL1489; _LL1489: _temp1488=(( struct Cyc_Absyn_Enum_e_struct*) _temp1268)->f2;
goto _LL1487; _LL1487: _temp1486=(( struct Cyc_Absyn_Enum_e_struct*) _temp1268)->f3;
goto _LL1335;} else{ goto _LL1336;} _LL1336: if(*(( int*) _temp1268) == Cyc_Absyn_Malloc_e){
_LL1495: _temp1494=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1268)->f1; goto
_LL1493; _LL1493: _temp1492=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1268)->f2; goto _LL1337;} else{ goto _LL1338;} _LL1338: if(*(( int*)
_temp1268) == Cyc_Absyn_StmtExp_e){ _LL1497: _temp1496=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1268)->f1; goto _LL1339;} else{ goto _LL1340;} _LL1340: if(*(( int*)
_temp1268) == Cyc_Absyn_UnresolvedMem_e){ _LL1501: _temp1500=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1268)->f1; goto _LL1499; _LL1499: _temp1498=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1268)->f2; goto _LL1341;} else{ goto _LL1342;} _LL1342: if(*(( int*)
_temp1268) == Cyc_Absyn_Codegen_e){ _LL1503: _temp1502=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1268)->f1; goto _LL1343;} else{ goto _LL1344;} _LL1344: if(*(( int*)
_temp1268) == Cyc_Absyn_Fill_e){ _LL1505: _temp1504=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1268)->f1; goto _LL1345;} else{ goto _LL1269;} _LL1271:{ void* _temp1506=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1512; struct Cyc_Absyn_Conref*
_temp1514; struct Cyc_Absyn_Tqual _temp1516; struct Cyc_Absyn_Conref* _temp1518;
void* _temp1520; void* _temp1522; _LL1508: if(( unsigned int) _temp1506 > 4u?*((
int*) _temp1506) == Cyc_Absyn_PointerType: 0){ _LL1513: _temp1512=(( struct Cyc_Absyn_PointerType_struct*)
_temp1506)->f1; _LL1523: _temp1522=( void*) _temp1512.elt_typ; goto _LL1521;
_LL1521: _temp1520=( void*) _temp1512.rgn_typ; goto _LL1519; _LL1519: _temp1518=
_temp1512.nullable; goto _LL1517; _LL1517: _temp1516= _temp1512.tq; goto _LL1515;
_LL1515: _temp1514= _temp1512.bounds; goto _LL1509;} else{ goto _LL1510;}
_LL1510: goto _LL1511; _LL1509: if(( void*) _temp1514->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1507;}{ void* _temp1524=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1514); struct Cyc_Absyn_Exp* _temp1530; _LL1526: if( _temp1524 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1527;} else{ goto _LL1528;} _LL1528: if((
unsigned int) _temp1524 > 1u?*(( int*) _temp1524) == Cyc_Absyn_Upper_b: 0){
_LL1531: _temp1530=(( struct Cyc_Absyn_Upper_b_struct*) _temp1524)->f1; goto
_LL1529;} else{ goto _LL1525;} _LL1527: { struct _tuple4* _temp1533=({ struct
_tuple4* _temp1532=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1532->f1= 0; _temp1532->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1532;});
goto _LL1534; _LL1534: { struct Cyc_Absyn_Exp* _temp1541= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List*(* _temp1535)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)(
struct _tagged_ptr3)) Cyc_List_list; struct _tuple4* _temp1538= _temp1533;
struct _tuple4* _temp1539= _temp1533; struct _tuple4* _temp1540= _temp1533;
struct _tuple4* _temp1536[ 3u]={ _temp1538, _temp1539, _temp1540}; struct
_tagged_ptr3 _temp1537={ _temp1536, _temp1536, _temp1536 + 3u}; _temp1535(
_temp1537);}), 0); goto _LL1542; _LL1542: if( nv->toplevel){( void*)( e->r=(
void*)(( void*) _temp1541->r));} else{ void* _temp1543= Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1522), Cyc_Toc_mt_tq); goto _LL1544; _LL1544: { struct
_tuple0* _temp1545= Cyc_Toc_temp_var(); goto _LL1546; _LL1546: { struct Cyc_Absyn_Exp*
_temp1547= Cyc_Absyn_var_exp( _temp1545, 0); goto _LL1548; _LL1548: { struct Cyc_Absyn_Stmt*
_temp1549= Cyc_Absyn_declare_stmt( _temp1545, _temp1543,( struct Cyc_Absyn_Exp*)
_temp1541, Cyc_Absyn_exp_stmt( _temp1547, 0), 0); goto _LL1550; _LL1550:( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1549, 0))->r));}}}} goto
_LL1525;}} _LL1529:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0,
0))->r)); goto _LL1525; _LL1525:;} goto _LL1507; _LL1511:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1551= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1551.last_plus_one -
_temp1551.curr, _temp1551.curr);})); goto _LL1507; _LL1507:;} goto _LL1269;
_LL1273: goto _LL1269; _LL1275:{ struct _handler_cons _temp1552; _push_handler(&
_temp1552);{ int _temp1554= 0; if( setjmp( _temp1552.handler)){ _temp1554= 1;}
if( ! _temp1554){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1352))->r));; _pop_handler();} else{ void* _temp1553=( void*) _exn_thrown;
void* _temp1556= _temp1553; _LL1558: if( _temp1556 == Cyc_Dict_Absent){ goto
_LL1559;} else{ goto _LL1560;} _LL1560: goto _LL1561; _LL1559:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1562= Cyc_Absynpp_qvar2string(
_temp1352); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1562.last_plus_one
- _temp1562.curr, _temp1562.curr);})); return; _LL1561:( void) _throw( _temp1556);
_LL1557:;}}} goto _LL1269; _LL1277:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1563="unknownid"; struct _tagged_string _temp1564; _temp1564.curr=
_temp1563; _temp1564.base= _temp1563; _temp1564.last_plus_one= _temp1563 + 10;
_temp1564;})); goto _LL1269; _LL1279: { struct Cyc_List_List* _temp1565=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp1356); goto _LL1566; _LL1566: {
struct Cyc_List_List* _temp1567=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1356); goto _LL1568; _LL1568:(( void(*)( void(* f)( struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)(
Cyc_Toc_exp_to_c, nv, _temp1356);{ void* _temp1569= _temp1358; _LL1571: if(
_temp1569 ==( void*) Cyc_Absyn_Size){ goto _LL1572;} else{ goto _LL1573;}
_LL1573: if( _temp1569 ==( void*) Cyc_Absyn_Printf){ goto _LL1574;} else{ goto
_LL1575;} _LL1575: if( _temp1569 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1576;}
else{ goto _LL1577;} _LL1577: if( _temp1569 ==( void*) Cyc_Absyn_Fprintf){ goto
_LL1578;} else{ goto _LL1579;} _LL1579: if( _temp1569 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1580;} else{ goto _LL1581;} _LL1581: if( _temp1569 ==( void*) Cyc_Absyn_Plus){
goto _LL1582;} else{ goto _LL1583;} _LL1583: if( _temp1569 ==( void*) Cyc_Absyn_Minus){
goto _LL1584;} else{ goto _LL1585;} _LL1585: if( _temp1569 ==( void*) Cyc_Absyn_Eq){
goto _LL1586;} else{ goto _LL1587;} _LL1587: if( _temp1569 ==( void*) Cyc_Absyn_Neq){
goto _LL1588;} else{ goto _LL1589;} _LL1589: if( _temp1569 ==( void*) Cyc_Absyn_Gt){
goto _LL1590;} else{ goto _LL1591;} _LL1591: if( _temp1569 ==( void*) Cyc_Absyn_Gte){
goto _LL1592;} else{ goto _LL1593;} _LL1593: if( _temp1569 ==( void*) Cyc_Absyn_Lt){
goto _LL1594;} else{ goto _LL1595;} _LL1595: if( _temp1569 ==( void*) Cyc_Absyn_Lte){
goto _LL1596;} else{ goto _LL1597;} _LL1597: goto _LL1598; _LL1572: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1356))->hd;{
void* _temp1599=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1607; struct Cyc_Absyn_Tqual _temp1609; void*
_temp1611; struct Cyc_Absyn_PtrInfo _temp1613; struct Cyc_Absyn_Conref*
_temp1615; struct Cyc_Absyn_Tqual _temp1617; struct Cyc_Absyn_Conref* _temp1619;
void* _temp1621; void* _temp1623; _LL1601: if(( unsigned int) _temp1599 > 4u?*((
int*) _temp1599) == Cyc_Absyn_ArrayType: 0){ _LL1612: _temp1611=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1599)->f1; goto _LL1610; _LL1610:
_temp1609=(( struct Cyc_Absyn_ArrayType_struct*) _temp1599)->f2; goto _LL1608;
_LL1608: _temp1607=(( struct Cyc_Absyn_ArrayType_struct*) _temp1599)->f3; goto
_LL1602;} else{ goto _LL1603;} _LL1603: if(( unsigned int) _temp1599 > 4u?*((
int*) _temp1599) == Cyc_Absyn_PointerType: 0){ _LL1614: _temp1613=(( struct Cyc_Absyn_PointerType_struct*)
_temp1599)->f1; _LL1624: _temp1623=( void*) _temp1613.elt_typ; goto _LL1622;
_LL1622: _temp1621=( void*) _temp1613.rgn_typ; goto _LL1620; _LL1620: _temp1619=
_temp1613.nullable; goto _LL1618; _LL1618: _temp1617= _temp1613.tq; goto _LL1616;
_LL1616: _temp1615= _temp1613.bounds; goto _LL1604;} else{ goto _LL1605;}
_LL1605: goto _LL1606; _LL1602:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1607))->r)); goto _LL1600; _LL1604: { struct _tuple0*
_temp1625= Cyc_Toc_temp_var(); goto _LL1626; _LL1626: { void* _temp1627= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); goto _LL1628;
_LL1628: { struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1625, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1625, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4=
Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp( e2, e3, 0), 0);
struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1625, _temp1627,(
struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1600;}}} _LL1606:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1629= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*) _check_null(
_temp1567))->hd); struct _tagged_string _temp1630= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_temp1629.last_plus_one - _temp1629.curr, _temp1629.curr, _temp1630.last_plus_one
- _temp1630.curr, _temp1630.curr);})); return; _LL1600:;} goto _LL1570;} _LL1574:(
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1358, 0, _temp1356,
_temp1567))->r)); goto _LL1570; _LL1576:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1358, 0, _temp1356, _temp1567))->r)); goto _LL1570; _LL1578: if( _temp1356
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char*
_temp1631="fprintf without arguments"; struct _tagged_string _temp1632;
_temp1632.curr= _temp1631; _temp1632.base= _temp1631; _temp1632.last_plus_one=
_temp1631 + 26; _temp1632;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1358,({ struct Cyc_Core_Opt* _temp1633=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1633->v=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1356))->hd); _temp1633;}),(( struct Cyc_List_List*)
_check_null( _temp1356))->tl,(( struct Cyc_List_List*) _check_null( _temp1567))->tl))->r));
goto _LL1570; _LL1580: if( _temp1356 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1634="sscanf without arguments";
struct _tagged_string _temp1635; _temp1635.curr= _temp1634; _temp1635.base=
_temp1634; _temp1635.last_plus_one= _temp1634 + 25; _temp1635;}));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1356))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1356))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1356))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1570; _LL1582: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1356))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1356))->tl))->hd; struct _tuple0* _temp1636= Cyc_Toc_temp_var(); goto
_LL1637; _LL1637: { void* _temp1638= Cyc_Toc_typ_to_c( old_typ); goto _LL1639;
_LL1639: { struct Cyc_Absyn_Stmt* _temp1640= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1636, 0), 0); goto _LL1641; _LL1641: { struct Cyc_Absyn_Exp* _temp1642= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1636, 0), Cyc_Toc_curr_sp, 0); goto _LL1643; _LL1643: {
struct Cyc_Absyn_Exp* _temp1645= Cyc_Absyn_assignop_exp( _temp1642,({ struct Cyc_Core_Opt*
_temp1644=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1644->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1644;}), e2, 0); goto
_LL1646; _LL1646: _temp1640= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1645,
0), _temp1640, 0); _temp1640= Cyc_Absyn_declare_stmt( _temp1636, _temp1638,(
struct Cyc_Absyn_Exp*) e1, _temp1640, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1640, 0))->r));}}}}} goto _LL1570; _LL1584: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null( _temp1565))->hd)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1356))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1356))->tl))->hd; void*
old_typ_e2=( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1565))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* _temp1647= Cyc_Toc_temp_var(); goto _LL1648;
_LL1648: { void* _temp1649= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_List_List*)
_check_null( _temp1565))->hd); goto _LL1650; _LL1650: { struct Cyc_Absyn_Stmt*
_temp1651= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1647, 0), 0); goto
_LL1652; _LL1652: { struct Cyc_Absyn_Exp* _temp1653= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1647, 0), Cyc_Toc_curr_sp, 0); goto _LL1654; _LL1654: {
struct Cyc_Absyn_Exp* _temp1656= Cyc_Absyn_assignop_exp( _temp1653,({ struct Cyc_Core_Opt*
_temp1655=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1655->v=( void*)(( void*) Cyc_Absyn_Minus); _temp1655;}), e2, 0); goto
_LL1657; _LL1657: _temp1651= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1656,
0), _temp1651, 0); _temp1651= Cyc_Absyn_declare_stmt( _temp1647, _temp1649,(
struct Cyc_Absyn_Exp*) e1, _temp1651, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1651, 0))->r));}}}}}} goto _LL1570; _LL1586: goto _LL1588; _LL1588: goto
_LL1590; _LL1590: goto _LL1592; _LL1592: goto _LL1594; _LL1594: goto _LL1596;
_LL1596: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1356))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1356))->tl))->hd; void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp1565))->hd; void* t2=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1565))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1570;} _LL1598: goto
_LL1570; _LL1570:;} goto _LL1269;}} _LL1281: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1362->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1362); if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if( _temp1360 ==(
void*) Cyc_Absyn_PreInc? 1: _temp1360 ==( void*) Cyc_Absyn_PreDec){ struct
_tuple0* _temp1658= Cyc_Toc_temp_var(); goto _LL1659; _LL1659: { void* _temp1660=
Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1661; _LL1661: { struct Cyc_Absyn_Stmt*
_temp1662= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp1658,
0), 0), 0); goto _LL1663; _LL1663: { struct Cyc_Absyn_Exp* _temp1664= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1658, 0), Cyc_Toc_curr_sp, 0),
_temp1360, 0); goto _LL1665; _LL1665: _temp1662= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1664, 0), _temp1662, 0); _temp1662= Cyc_Absyn_declare_stmt( _temp1658,
_temp1660,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1362, 0),
_temp1662, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1662, 0))->r));}}}}
else{ struct _tuple0* _temp1666= Cyc_Toc_temp_var(); goto _LL1667; _LL1667: {
struct _tuple0* _temp1668= Cyc_Toc_temp_var(); goto _LL1669; _LL1669: { void*
_temp1670= Cyc_Toc_typ_to_c( e2_cyc_typ); goto _LL1671; _LL1671: { void*
_temp1672= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1673; _LL1673: { struct Cyc_Absyn_Stmt*
_temp1674= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1668, 0), 0); goto
_LL1675; _LL1675: { struct Cyc_Absyn_Exp* _temp1676= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1666, 0), Cyc_Toc_curr_sp, 0),
_temp1360, 0); goto _LL1677; _LL1677: _temp1674= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1676, 0), _temp1674, 0); _temp1674= Cyc_Absyn_declare_stmt( _temp1668,
_temp1670,( struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp1666, 0), 0), _temp1674, 0); _temp1674= Cyc_Absyn_declare_stmt( _temp1666,
_temp1672,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1362, 0),
_temp1674, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1674, 0))->r));}}}}}}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1362)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1362, 0, Cyc_Toc_incr_lvalue,
_temp1360);( void*)( e->r=( void*)(( void*) _temp1362->r));}} goto _LL1269;}
_LL1283: { int e1_poly= Cyc_Toc_is_poly_project( _temp1368); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1368->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1364->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1368); Cyc_Toc_exp_to_c( nv, _temp1364); if(
_temp1366 != 0?( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1366))->v ==(
void*) Cyc_Absyn_Plus? 1:( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1366))->v
==( void*) Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){(
void*)( _temp1368->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1368->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1364->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1364->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1364->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1364->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1368)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1368, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1678=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1678[ 0]=({ struct _tuple11 _temp1679; _temp1679.f1=
_temp1366; _temp1679.f2= _temp1364; _temp1679;}); _temp1678;}));( void*)( e->r=(
void*)(( void*) _temp1368->r));} goto _LL1269;} _LL1285: Cyc_Toc_exp_to_c( nv,
_temp1374); Cyc_Toc_exp_to_c( nv, _temp1372); Cyc_Toc_exp_to_c( nv, _temp1370);
goto _LL1269; _LL1287: Cyc_Toc_exp_to_c( nv, _temp1378); Cyc_Toc_exp_to_c( nv,
_temp1376); goto _LL1269; _LL1289: _temp1388= _temp1382; _temp1386= _temp1380;
goto _LL1291; _LL1291: Cyc_Toc_exp_to_c( nv, _temp1388);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1386); goto _LL1269; _LL1293:{
struct _RegionHandle _temp1680= _new_region(); struct _RegionHandle* r=&
_temp1680; _push_region( r);{ void* _temp1681=( void*) _temp1393->type; goto
_LL1682; _LL1682: { void* _temp1683= Cyc_Toc_typ_to_c( _temp1681); goto _LL1684;
_LL1684: { void* _temp1685= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1401->topt))->v); goto _LL1686; _LL1686: { struct Cyc_Absyn_Exp*
_temp1687= Cyc_Absyn_uint_exp(( unsigned int) _temp1397, 0); goto _LL1688;
_LL1688: { void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1760=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1760[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1761; _temp1761.tag= Cyc_Absyn_ArrayType;
_temp1761.f1=( void*) _temp1681; _temp1761.f2= Cyc_Toc_mt_tq; _temp1761.f3=(
struct Cyc_Absyn_Exp*) _temp1687; _temp1761;}); _temp1760;})); struct _tuple0*
_temp1689= Cyc_Toc_temp_var(); goto _LL1690; _LL1690: { struct _tuple0*
_temp1691= Cyc_Toc_temp_var(); goto _LL1692; _LL1692: { struct Cyc_Absyn_Exp*
_temp1693= Cyc_Absyn_var_exp( _temp1691, 0); goto _LL1694; _LL1694: { struct
_tuple0* _temp1695= Cyc_Toc_temp_var(); goto _LL1696; _LL1696: { int _temp1697=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1399); goto _LL1698;
_LL1698: { int _temp1699= _temp1697 - _temp1397; goto _LL1700; _LL1700: { struct
_tagged_ptr5 vs=({ unsigned int _temp1756=( unsigned int) _temp1397; struct
_tuple0** _temp1757=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1756); struct _tagged_ptr5 _temp1759={ _temp1757, _temp1757,
_temp1757 + _temp1756};{ unsigned int _temp1758= _temp1756; unsigned int i; for(
i= 0; i < _temp1758; i ++){ _temp1757[ i]= Cyc_Toc_temp_var();}}; _temp1759;});
struct _tagged_ptr5 xs=({ unsigned int _temp1752=( unsigned int) _temp1699;
struct _tuple0** _temp1753=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1752); struct _tagged_ptr5 _temp1755={ _temp1753, _temp1753,
_temp1753 + _temp1752};{ unsigned int _temp1754= _temp1752; unsigned int i; for(
i= 0; i < _temp1754; i ++){ _temp1753[ i]= Cyc_Toc_temp_var();}}; _temp1755;});
struct Cyc_List_List* xexps=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(*
f)( struct _tuple0*, struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,( struct Cyc_Position_Segment*)
0,(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct _tagged_ptr5 arr))
Cyc_List_rfrom_array)( r, xs)); xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List*
_temp1701=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1701->hd=( void*) Cyc_Absyn_var_exp( _temp1695, 0); _temp1701->tl= 0;
_temp1701;}));{ struct Cyc_Absyn_Stmt* _temp1702= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( _temp1689, 0), xexps, 0), 0); goto _LL1703; _LL1703: { struct
Cyc_Absyn_Exp* _temp1704= Cyc_Absyn_add_exp( _temp1693, _temp1687, 0); goto
_LL1705; _LL1705: { struct Cyc_List_List* _temp1715=({ struct Cyc_List_List*(*
_temp1706)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)( struct _tagged_ptr3))
Cyc_List_list; struct _tuple4* _temp1709=({ struct _tuple4* _temp1714=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1714->f1= 0; _temp1714->f2=
_temp1693; _temp1714;}); struct _tuple4* _temp1710=({ struct _tuple4* _temp1713=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1713->f1= 0;
_temp1713->f2= _temp1693; _temp1713;}); struct _tuple4* _temp1711=({ struct
_tuple4* _temp1712=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1712->f1= 0; _temp1712->f2= _temp1704; _temp1712;}); struct _tuple4*
_temp1707[ 3u]={ _temp1709, _temp1710, _temp1711}; struct _tagged_ptr3 _temp1708={
_temp1707, _temp1707, _temp1707 + 3u}; _temp1706( _temp1708);}); goto _LL1716;
_LL1716: _temp1702= Cyc_Absyn_declare_stmt( _temp1695, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp1681,( void*) _temp1393->rgn, Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp(
0, _temp1715, 0), _temp1702, 0);{ struct Cyc_List_List* _temp1717=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_ptr5 arr)) Cyc_List_rfrom_array)( r, vs));
goto _LL1718; _LL1718: { struct Cyc_List_List* _temp1719=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_make_dle, _temp1717); goto _LL1720; _LL1720: _temp1702= Cyc_Absyn_declare_stmt(
_temp1691, arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1719, 0), _temp1702, 0);{ struct Cyc_List_List* _temp1721=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1399);
goto _LL1722; _LL1722: { struct Cyc_List_List* _temp1723=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1395);
goto _LL1724; _LL1724:{ int i= _temp1397 - 1; for( 0; i >= 0;( i --, _temp1721=((
struct Cyc_List_List*) _check_null( _temp1721))->tl)){ struct Cyc_Absyn_Exp* arg=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1721))->hd;
void* arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c(
nv, arg); if( _temp1393->inject){ void* _temp1727; struct Cyc_Position_Segment*
_temp1729; struct Cyc_List_List* _temp1731; struct Cyc_List_List* _temp1733;
struct _tuple0* _temp1735; struct Cyc_Absyn_Tunionfield _temp1725=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1723))->hd); _LL1736: _temp1735=
_temp1725.name; goto _LL1734; _LL1734: _temp1733= _temp1725.tvs; goto _LL1732;
_LL1732: _temp1731= _temp1725.typs; goto _LL1730; _LL1730: _temp1729= _temp1725.loc;
goto _LL1728; _LL1728: _temp1727=( void*) _temp1725.sc; goto _LL1726; _LL1726:
_temp1723=(( struct Cyc_List_List*) _check_null( _temp1723))->tl;{ struct
_tuple0* _temp1737= Cyc_Toc_temp_var(); goto _LL1738; _LL1738: { struct Cyc_Absyn_Exp*
_temp1739= Cyc_Absyn_var_exp( _temp1737, 0); goto _LL1740; _LL1740: { void*
_temp1743= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1735,({
unsigned char* _temp1741="_struct"; struct _tagged_string _temp1742; _temp1742.curr=
_temp1741; _temp1742.base= _temp1741; _temp1742.last_plus_one= _temp1741 + 8;
_temp1742;}))); goto _LL1744; _LL1744: _temp1702= Cyc_Absyn_declare_stmt(*((
struct _tuple0**(*)( struct _tagged_ptr5, unsigned int, unsigned int))
_check_unknown_subscript)( vs, sizeof( struct _tuple0*), i), _temp1683,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1683, Cyc_Absyn_address_exp( _temp1739,
0), 0), _temp1702, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp1731))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1748= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1739,({
struct _tagged_string* _temp1745=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp1745[ 0]=({ unsigned char* _temp1746="f1"; struct
_tagged_string _temp1747; _temp1747.curr= _temp1746; _temp1747.base= _temp1746;
_temp1747.last_plus_one= _temp1746 + 3; _temp1747;}); _temp1745;}), 0), arg, 0);
goto _LL1749; _LL1749: _temp1702= Cyc_Absyn_seq_stmt( _temp1748, _temp1702, 0);{
struct Cyc_Absyn_Stmt* _temp1750= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1739, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1735, 0), 0); goto
_LL1751; _LL1751: _temp1702= Cyc_Absyn_seq_stmt( _temp1750, _temp1702, 0);
_temp1702= Cyc_Absyn_declare_stmt( _temp1737, _temp1743, 0, _temp1702, 0);}}}}}}}
else{ _temp1702= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr5, unsigned int, unsigned int)) _check_unknown_subscript)( vs,
sizeof( struct _tuple0*), i), _temp1683,( struct Cyc_Absyn_Exp*) arg, _temp1702,
0);}}}{ int i= _temp1699 - 1; for( 0; i >= 0;( i --, _temp1721=(( struct Cyc_List_List*)
_check_null( _temp1721))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1721))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1702= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr5, unsigned int, unsigned int)) _check_unknown_subscript)( xs,
sizeof( struct _tuple0*), i), argtype,( struct Cyc_Absyn_Exp*) arg, _temp1702, 0);}}
Cyc_Toc_exp_to_c( nv, _temp1401); _temp1702= Cyc_Absyn_declare_stmt( _temp1689,
_temp1685,( struct Cyc_Absyn_Exp*) _temp1401, _temp1702, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1702, 0))->r));}}}}}}}}}}}}}}}}}};
_pop_region(& _temp1680);} goto _LL1269; _LL1295: Cyc_Toc_exp_to_c( nv,
_temp1403);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1403), 0))->r)); goto _LL1269; _LL1297: Cyc_Toc_exp_to_c(
nv, _temp1405); goto _LL1269; _LL1299: Cyc_Toc_exp_to_c( nv, _temp1409);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
for( 0; _temp1407 != 0; _temp1407=(( struct Cyc_List_List*) _check_null(
_temp1407))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*)
_check_null( _temp1407))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*)
Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1409, 0))->r)); break;}} goto _LL1269;} _LL1301: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1411->topt))->v; void* new_typ=*
_temp1415;* _temp1415= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1411);{ int noconv= 0;{ void* _temp1762= Cyc_Toc_conversion( old_t2,
new_typ); void* _temp1772; struct Cyc_Absyn_Exp* _temp1774; void* _temp1776; int
_temp1778; struct Cyc_Absyn_Exp* _temp1780; void* _temp1782; _LL1764: if(
_temp1762 ==( void*) Cyc_Toc_NoConv){ goto _LL1765;} else{ goto _LL1766;}
_LL1766: if(( unsigned int) _temp1762 > 1u?*(( int*) _temp1762) == Cyc_Toc_NullCheck:
0){ _LL1773: _temp1772=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp1762)->f1;
goto _LL1767;} else{ goto _LL1768;} _LL1768: if(( unsigned int) _temp1762 > 1u?*((
int*) _temp1762) == Cyc_Toc_TagPtr: 0){ _LL1777: _temp1776=( void*)(( struct Cyc_Toc_TagPtr_struct*)
_temp1762)->f1; goto _LL1775; _LL1775: _temp1774=(( struct Cyc_Toc_TagPtr_struct*)
_temp1762)->f2; goto _LL1769;} else{ goto _LL1770;} _LL1770: if(( unsigned int)
_temp1762 > 1u?*(( int*) _temp1762) == Cyc_Toc_UntagPtr: 0){ _LL1783: _temp1782=(
void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp1762)->f1; goto _LL1781; _LL1781:
_temp1780=(( struct Cyc_Toc_UntagPtr_struct*) _temp1762)->f2; goto _LL1779;
_LL1779: _temp1778=(( struct Cyc_Toc_UntagPtr_struct*) _temp1762)->f3; goto
_LL1771;} else{ goto _LL1763;} _LL1765: goto _LL1763; _LL1767:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_null_check_conv( nv->toplevel, _temp1772, _temp1411))->r));
goto _LL1763; _LL1769:( void*)( e->r=( void*)(( void*)( Cyc_Toc_tag_ptr_conv( nv->toplevel,
_temp1776, _temp1774, _temp1411))->r)); goto _LL1763; _LL1771:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_untag_ptr_conv( nv->toplevel, _temp1782, _temp1780,
_temp1778, _temp1411))->r)); goto _LL1763; _LL1763:;} goto _LL1269;}} _LL1303:{
void* _temp1784=( void*) _temp1416->r; struct Cyc_Absyn_Structdecl* _temp1792;
struct Cyc_List_List* _temp1794; struct Cyc_Core_Opt* _temp1796; struct _tuple0*
_temp1798; struct Cyc_List_List* _temp1800; _LL1786: if(*(( int*) _temp1784) ==
Cyc_Absyn_Struct_e){ _LL1799: _temp1798=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1784)->f1; goto _LL1797; _LL1797: _temp1796=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1784)->f2; goto _LL1795; _LL1795: _temp1794=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1784)->f3; goto _LL1793; _LL1793: _temp1792=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1784)->f4; goto _LL1787;} else{ goto _LL1788;} _LL1788: if(*(( int*)
_temp1784) == Cyc_Absyn_Tuple_e){ _LL1801: _temp1800=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1784)->f1; goto _LL1789;} else{ goto _LL1790;} _LL1790: goto _LL1791;
_LL1787: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1802= Cyc_Position_string_of_segment( _temp1416->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1802.last_plus_one -
_temp1802.curr, _temp1802.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1416->topt))->v, 1, 0,
_temp1794, _temp1798))->r)); goto _LL1785; _LL1789: if( nv->toplevel){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1803= Cyc_Position_string_of_segment(
_temp1416->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1803.last_plus_one - _temp1803.curr, _temp1803.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1800))->r)); goto _LL1785;
_LL1791: Cyc_Toc_exp_to_c( nv, _temp1416); if( ! Cyc_Absyn_is_lvalue( _temp1416)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1416,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1416, 0))->r));} goto _LL1785; _LL1785:;} goto _LL1269; _LL1305: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string
_temp1804= Cyc_Position_string_of_segment( _temp1418->loc); xprintf("%.*s: new at top-level",
_temp1804.last_plus_one - _temp1804.curr, _temp1804.curr);}));}{ void* _temp1805=(
void*) _temp1418->r; struct Cyc_List_List* _temp1819; void* _temp1821; struct
_tagged_string _temp1823; struct Cyc_Absyn_Exp* _temp1825; struct Cyc_Absyn_Exp*
_temp1827; struct Cyc_Absyn_Vardecl* _temp1829; struct Cyc_Absyn_Structdecl*
_temp1831; struct Cyc_List_List* _temp1833; struct Cyc_Core_Opt* _temp1835;
struct _tuple0* _temp1837; struct Cyc_List_List* _temp1839; _LL1807: if(*(( int*)
_temp1805) == Cyc_Absyn_Array_e){ _LL1820: _temp1819=(( struct Cyc_Absyn_Array_e_struct*)
_temp1805)->f1; goto _LL1808;} else{ goto _LL1809;} _LL1809: if(*(( int*)
_temp1805) == Cyc_Absyn_Const_e){ _LL1822: _temp1821=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1805)->f1; if(( unsigned int) _temp1821 > 1u?*(( int*) _temp1821) == Cyc_Absyn_String_c:
0){ _LL1824: _temp1823=(( struct Cyc_Absyn_String_c_struct*) _temp1821)->f1;
goto _LL1810;} else{ goto _LL1811;}} else{ goto _LL1811;} _LL1811: if(*(( int*)
_temp1805) == Cyc_Absyn_Comprehension_e){ _LL1830: _temp1829=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1805)->f1; goto _LL1828; _LL1828: _temp1827=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1805)->f2; goto _LL1826; _LL1826: _temp1825=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1805)->f3; goto _LL1812;} else{ goto _LL1813;} _LL1813: if(*(( int*)
_temp1805) == Cyc_Absyn_Struct_e){ _LL1838: _temp1837=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1805)->f1; goto _LL1836; _LL1836: _temp1835=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1805)->f2; goto _LL1834; _LL1834: _temp1833=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1805)->f3; goto _LL1832; _LL1832: _temp1831=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1805)->f4; goto _LL1814;} else{ goto _LL1815;} _LL1815: if(*(( int*)
_temp1805) == Cyc_Absyn_Tuple_e){ _LL1840: _temp1839=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1805)->f1; goto _LL1816;} else{ goto _LL1817;} _LL1817: goto _LL1818;
_LL1808: { struct _tuple0* _temp1841= Cyc_Toc_temp_var(); goto _LL1842; _LL1842: {
struct Cyc_Absyn_Exp* _temp1843= Cyc_Absyn_var_exp( _temp1841, 0); goto _LL1844;
_LL1844: { struct Cyc_Absyn_Stmt* _temp1845= Cyc_Toc_init_array( nv, _temp1843,
_temp1819, Cyc_Absyn_exp_stmt( _temp1843, 0)); goto _LL1846; _LL1846: { void*
old_elt_typ;{ void* _temp1847= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1853; struct Cyc_Absyn_Conref* _temp1855; struct Cyc_Absyn_Tqual _temp1857;
struct Cyc_Absyn_Conref* _temp1859; void* _temp1861; void* _temp1863; _LL1849:
if(( unsigned int) _temp1847 > 4u?*(( int*) _temp1847) == Cyc_Absyn_PointerType:
0){ _LL1854: _temp1853=(( struct Cyc_Absyn_PointerType_struct*) _temp1847)->f1;
_LL1864: _temp1863=( void*) _temp1853.elt_typ; goto _LL1862; _LL1862: _temp1861=(
void*) _temp1853.rgn_typ; goto _LL1860; _LL1860: _temp1859= _temp1853.nullable;
goto _LL1858; _LL1858: _temp1857= _temp1853.tq; goto _LL1856; _LL1856: _temp1855=
_temp1853.bounds; goto _LL1850;} else{ goto _LL1851;} _LL1851: goto _LL1852;
_LL1850: old_elt_typ= _temp1863; goto _LL1848; _LL1852: old_elt_typ=(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1865="exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1866; _temp1866.curr= _temp1865; _temp1866.base=
_temp1865; _temp1866.last_plus_one= _temp1865 + 52; _temp1866;})); goto _LL1848;
_LL1848:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* _temp1867= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1868; _LL1868: { struct Cyc_Absyn_Exp*
_temp1869= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1819), 0), 0); goto
_LL1870; _LL1870: { struct Cyc_Absyn_Exp* e1; if( _temp1420 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1869);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1420); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1869);}{ struct Cyc_Absyn_Exp* _temp1871= Cyc_Absyn_cast_exp( _temp1867, e1,
0); goto _LL1872; _LL1872:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1841, _temp1867,( struct Cyc_Absyn_Exp*) _temp1871,
_temp1845, 0), 0))->r)); goto _LL1806;}}}}}}}} _LL1810: if( _temp1420 == 0){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1873=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1873->f1= Cyc_Toc_abs_ns; _temp1873->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1873;}), 0),({ struct Cyc_List_List* _temp1874=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1874->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1875=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1875[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1876; _temp1876.tag= Cyc_Absyn_Const_e; _temp1876.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1877=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1877[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1878; _temp1878.tag= Cyc_Absyn_String_c;
_temp1878.f1= _temp1823; _temp1878;}); _temp1877;})); _temp1876;}); _temp1875;}),
0); _temp1874->tl= 0; _temp1874;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( _temp1420); Cyc_Toc_exp_to_c( nv, r);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({ struct
_tuple0* _temp1879=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1879->f1= Cyc_Toc_abs_ns; _temp1879->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1879;}), 0),({ struct Cyc_List_List* _temp1880=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1880->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1881=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1881[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1882; _temp1882.tag= Cyc_Absyn_Const_e; _temp1882.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1883=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1883[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1884; _temp1884.tag= Cyc_Absyn_String_c;
_temp1884.f1= _temp1823; _temp1884;}); _temp1883;})); _temp1882;}); _temp1881;}),
0); _temp1880->tl= 0; _temp1880;}), 0))->r));} goto _LL1806; _LL1812: { int
is_tagged_ptr= 0;{ void* _temp1885= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1891; struct Cyc_Absyn_Conref* _temp1893; struct Cyc_Absyn_Tqual _temp1895;
struct Cyc_Absyn_Conref* _temp1897; void* _temp1899; void* _temp1901; _LL1887:
if(( unsigned int) _temp1885 > 4u?*(( int*) _temp1885) == Cyc_Absyn_PointerType:
0){ _LL1892: _temp1891=(( struct Cyc_Absyn_PointerType_struct*) _temp1885)->f1;
_LL1902: _temp1901=( void*) _temp1891.elt_typ; goto _LL1900; _LL1900: _temp1899=(
void*) _temp1891.rgn_typ; goto _LL1898; _LL1898: _temp1897= _temp1891.nullable;
goto _LL1896; _LL1896: _temp1895= _temp1891.tq; goto _LL1894; _LL1894: _temp1893=
_temp1891.bounds; goto _LL1888;} else{ goto _LL1889;} _LL1889: goto _LL1890;
_LL1888:{ void* _temp1903=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1893))->v; void* _temp1909; _LL1905: if((
unsigned int) _temp1903 > 1u?*(( int*) _temp1903) == Cyc_Absyn_Eq_constr: 0){
_LL1910: _temp1909=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1903)->f1;
if( _temp1909 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1906;} else{ goto _LL1907;}}
else{ goto _LL1907;} _LL1907: goto _LL1908; _LL1906: is_tagged_ptr= 1; goto
_LL1904; _LL1908: goto _LL1904; _LL1904:;} goto _LL1886; _LL1890:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1911="exp_to_c: comprehension not an array type";
struct _tagged_string _temp1912; _temp1912.curr= _temp1911; _temp1912.base=
_temp1911; _temp1912.last_plus_one= _temp1911 + 42; _temp1912;})); goto _LL1886;
_LL1886:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1825->topt))->v;
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1827);{ struct Cyc_Absyn_Stmt*
s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( a, 0), _temp1829, Cyc_Absyn_var_exp(
max, 0), _temp1825, Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1913=
_new_region(); struct _RegionHandle* r=& _temp1913; _push_region( r);{ struct
Cyc_List_List* decls=({ struct Cyc_List_List* _temp1943=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1943->hd=( void*)({
struct _tuple10* _temp1944=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp1944->f1= max; _temp1944->f2= Cyc_Absyn_uint_t; _temp1944->f3=(
struct Cyc_Absyn_Exp*) _temp1827; _temp1944;}); _temp1943->tl= 0; _temp1943;});
struct Cyc_Absyn_Exp* ai; if( _temp1420 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1420); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1914=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1914->hd=( void*)({ struct _tuple10*
_temp1915=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1915->f1= a; _temp1915->f2= ptr_typ; _temp1915->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1915;}); _temp1914->tl= decls; _temp1914;}); if( is_tagged_ptr){
struct _tuple0* _temp1916= Cyc_Toc_temp_var(); goto _LL1917; _LL1917: { void*
_temp1918= Cyc_Toc_typ_to_c( old_typ); goto _LL1919; _LL1919: { struct Cyc_Absyn_Exp*
_temp1929= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(* _temp1920)(
struct _tagged_ptr3)=( struct Cyc_List_List*(*)( struct _tagged_ptr3)) Cyc_List_list;
struct _tuple4* _temp1923=({ struct _tuple4* _temp1928=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1928->f1= 0; _temp1928->f2= Cyc_Absyn_var_exp(
a, 0); _temp1928;}); struct _tuple4* _temp1924=({ struct _tuple4* _temp1927=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1927->f1= 0;
_temp1927->f2= Cyc_Absyn_var_exp( a, 0); _temp1927;}); struct _tuple4* _temp1925=({
struct _tuple4* _temp1926=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1926->f1= 0; _temp1926->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1926;}); struct _tuple4* _temp1921[ 3u]={
_temp1923, _temp1924, _temp1925}; struct _tagged_ptr3 _temp1922={ _temp1921,
_temp1921, _temp1921 + 3u}; _temp1920( _temp1922);}), 0); goto _LL1930; _LL1930:
decls=({ struct Cyc_List_List* _temp1931=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1931->hd=( void*)({ struct _tuple10*
_temp1932=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1932->f1= _temp1916; _temp1932->f2= _temp1918; _temp1932->f3=( struct Cyc_Absyn_Exp*)
_temp1929; _temp1932;}); _temp1931->tl= decls; _temp1931;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1916, 0), 0), 0);}}} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
_temp1933= decls; goto _LL1934; _LL1934: for( 0; _temp1933 != 0; _temp1933=((
struct Cyc_List_List*) _check_null( _temp1933))->tl){ struct Cyc_Absyn_Exp*
_temp1937; void* _temp1939; struct _tuple0* _temp1941; struct _tuple10 _temp1935=*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1933))->hd);
_LL1942: _temp1941= _temp1935.f1; goto _LL1940; _LL1940: _temp1939= _temp1935.f2;
goto _LL1938; _LL1938: _temp1937= _temp1935.f3; goto _LL1936; _LL1936: s= Cyc_Absyn_declare_stmt(
_temp1941, _temp1939, _temp1937, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region(& _temp1913);} goto _LL1806;}}} _LL1814:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1418->topt))->v, 1, _temp1420, _temp1833, _temp1837))->r));
goto _LL1806; _LL1816:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv,
1, _temp1420, _temp1839))->r)); goto _LL1806; _LL1818: { void* _temp1945=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1418->topt))->v; goto _LL1946; _LL1946: {
struct _tuple0* _temp1947= Cyc_Toc_temp_var(); goto _LL1948; _LL1948: { struct
Cyc_Absyn_Stmt* _temp1949= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1947, 0),
0); goto _LL1950; _LL1950: { struct Cyc_Absyn_Exp* _temp1951= Cyc_Absyn_signed_int_exp(
0, 0); goto _LL1952; _LL1952: Cyc_Toc_exp_to_c( nv, _temp1418); _temp1949= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1947, 0),
_temp1951, 0), _temp1418, 0), _temp1949, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
_temp1945); void* _temp1953= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); goto
_LL1954; _LL1954: { struct Cyc_Absyn_Exp* _temp1955= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0); goto _LL1956; _LL1956: { struct Cyc_Absyn_Exp* e1; if( _temp1420 ==
0){ e1= Cyc_Toc_malloc_exp( _temp1945, _temp1955);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp1420); Cyc_Toc_exp_to_c( nv, r); e1=
Cyc_Toc_rmalloc_exp( r, _temp1955);}{ struct Cyc_Absyn_Exp* _temp1957= Cyc_Absyn_cast_exp(
_temp1953, e1, 0); goto _LL1958; _LL1958:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1947, _temp1953,( struct Cyc_Absyn_Exp*) _temp1957,
_temp1949, 0), 0))->r)); goto _LL1806;}}}}}}}} _LL1806:;} goto _LL1269; _LL1307:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct* _temp1959=(
struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1959[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1960; _temp1960.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1960.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1422);
_temp1960;}); _temp1959;}))); goto _LL1269; _LL1309: Cyc_Toc_exp_to_c( nv,
_temp1424); goto _LL1269; _LL1311: { void* e1_typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1426->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1426);{ void* _temp1961= e1_typ; struct Cyc_Absyn_PtrInfo _temp1967; struct
Cyc_Absyn_Conref* _temp1969; struct Cyc_Absyn_Tqual _temp1971; struct Cyc_Absyn_Conref*
_temp1973; void* _temp1975; void* _temp1977; _LL1963: if(( unsigned int)
_temp1961 > 4u?*(( int*) _temp1961) == Cyc_Absyn_PointerType: 0){ _LL1968:
_temp1967=(( struct Cyc_Absyn_PointerType_struct*) _temp1961)->f1; _LL1978:
_temp1977=( void*) _temp1967.elt_typ; goto _LL1976; _LL1976: _temp1975=( void*)
_temp1967.rgn_typ; goto _LL1974; _LL1974: _temp1973= _temp1967.nullable; goto
_LL1972; _LL1972: _temp1971= _temp1967.tq; goto _LL1970; _LL1970: _temp1969=
_temp1967.bounds; goto _LL1964;} else{ goto _LL1965;} _LL1965: goto _LL1966;
_LL1964:{ void* _temp1979=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1969); struct Cyc_Absyn_Exp* _temp1985; _LL1981: if( _temp1979 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1982;} else{ goto _LL1983;} _LL1983: if((
unsigned int) _temp1979 > 1u?*(( int*) _temp1979) == Cyc_Absyn_Upper_b: 0){
_LL1986: _temp1985=(( struct Cyc_Absyn_Upper_b_struct*) _temp1979)->f1; goto
_LL1984;} else{ goto _LL1980;} _LL1982: { struct _tuple0* x= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); void* _temp1987= Cyc_Toc_typ_to_c_array(
_temp1977); goto _LL1988; _LL1988: { void* tx= Cyc_Toc_add_tagged_ptr_type(
_temp1987, _temp1971); void* ta2= Cyc_Absyn_cstar_typ( _temp1987, _temp1971);
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
struct Cyc_Absyn_Exp*) _temp1426, s1, 0), 0))->r)); goto _LL1980;}} _LL1984: if(
Cyc_Toc_is_nullable( e1_typ)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1426->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1989=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1989->hd=(
void*) _temp1426; _temp1989->tl= 0; _temp1989;}), 0), 0), 0))->r));} else{
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, e1_typ,(
struct Cyc_Absyn_Exp*) _temp1426, s3, 0), 0))->r));}} goto _LL1980; _LL1980:;}
goto _LL1962; _LL1966:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1990="exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1991; _temp1991.curr= _temp1990; _temp1991.base= _temp1990; _temp1991.last_plus_one=
_temp1990 + 29; _temp1991;})); goto _LL1962; _LL1962:;} goto _LL1269;} _LL1313:
Cyc_Toc_exp_to_c( nv, _temp1430); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL1269; _LL1315: Cyc_Toc_exp_to_c( nv, _temp1434);{ int is_poly=
Cyc_Toc_is_poly_project( e); if( Cyc_Toc_is_nullable(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1434->topt))->v)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1434->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1992=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1992->hd=(
void*) _temp1434; _temp1992->tl= 0; _temp1992;}), 0), 0), _temp1432, 0))->r));}
else{ struct _tuple0* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1434->topt))->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp1432, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1434, s3, 0), 0))->r));}} if( is_poly){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto
_LL1269;} _LL1317: { void* _temp1993= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1438->topt))->v); goto _LL1994; _LL1994: Cyc_Toc_exp_to_c( nv,
_temp1438); Cyc_Toc_exp_to_c( nv, _temp1436);{ void* _temp1995= _temp1993;
struct Cyc_List_List* _temp2003; struct Cyc_Absyn_PtrInfo _temp2005; struct Cyc_Absyn_Conref*
_temp2007; struct Cyc_Absyn_Tqual _temp2009; struct Cyc_Absyn_Conref* _temp2011;
void* _temp2013; void* _temp2015; _LL1997: if(( unsigned int) _temp1995 > 4u?*((
int*) _temp1995) == Cyc_Absyn_TupleType: 0){ _LL2004: _temp2003=(( struct Cyc_Absyn_TupleType_struct*)
_temp1995)->f1; goto _LL1998;} else{ goto _LL1999;} _LL1999: if(( unsigned int)
_temp1995 > 4u?*(( int*) _temp1995) == Cyc_Absyn_PointerType: 0){ _LL2006:
_temp2005=(( struct Cyc_Absyn_PointerType_struct*) _temp1995)->f1; _LL2016:
_temp2015=( void*) _temp2005.elt_typ; goto _LL2014; _LL2014: _temp2013=( void*)
_temp2005.rgn_typ; goto _LL2012; _LL2012: _temp2011= _temp2005.nullable; goto
_LL2010; _LL2010: _temp2009= _temp2005.tq; goto _LL2008; _LL2008: _temp2007=
_temp2005.bounds; goto _LL2000;} else{ goto _LL2001;} _LL2001: goto _LL2002;
_LL1998: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1436) + 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp1438, Cyc_Toc_fieldname((
int) i), 0))->r)); goto _LL1996;} _LL2000:{ void* _temp2017=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2007); struct Cyc_Absyn_Exp*
_temp2023; _LL2019: if(( unsigned int) _temp2017 > 1u?*(( int*) _temp2017) ==
Cyc_Absyn_Upper_b: 0){ _LL2024: _temp2023=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2017)->f1; goto _LL2020;} else{ goto _LL2021;} _LL2021: if( _temp2017 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL2022;} else{ goto _LL2018;} _LL2020:
_temp2023= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp( _temp2023), 0);{
int possibly_null;{ void* _temp2025=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2011))->v; int
_temp2031; _LL2027: if(( unsigned int) _temp2025 > 1u?*(( int*) _temp2025) ==
Cyc_Absyn_Eq_constr: 0){ _LL2032: _temp2031=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2025)->f1; goto _LL2028;} else{ goto _LL2029;} _LL2029: goto _LL2030;
_LL2028: possibly_null= _temp2031; goto _LL2026; _LL2030: possibly_null= 0; goto
_LL2026; _LL2026:;}{ void* ta1= Cyc_Toc_typ_to_c( _temp2015); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp2009); if( Cyc_Toc_use_runtimecheck_funs){ if( possibly_null){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv, 0),({ struct Cyc_List_List*(*
_temp2033)( struct _tagged_ptr6)=( struct Cyc_List_List*(*)( struct _tagged_ptr6))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp2036= _temp1438; struct Cyc_Absyn_Exp*
_temp2037= _temp2023; struct Cyc_Absyn_Exp* _temp2038= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp2039= _temp1436; struct Cyc_Absyn_Exp*
_temp2034[ 4u]={ _temp2036, _temp2037, _temp2038, _temp2039}; struct
_tagged_ptr6 _temp2035={ _temp2034, _temp2034, _temp2034 + 4u}; _temp2033(
_temp2035);}), 0), 0), 0))->r));} else{( void*)( _temp1436->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List*(* _temp2040)( struct _tagged_ptr6)=( struct Cyc_List_List*(*)(
struct _tagged_ptr6)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2043= _temp2023;
struct Cyc_Absyn_Exp* _temp2044= Cyc_Absyn_copy_exp( _temp1436); struct Cyc_Absyn_Exp*
_temp2041[ 2u]={ _temp2043, _temp2044}; struct _tagged_ptr6 _temp2042={
_temp2041, _temp2041, _temp2041 + 2u}; _temp2040( _temp2042);}), 0))->r));}}
else{ struct _tuple0* a= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte, Cyc_Absyn_var_exp(
i, 0), _temp2023, 0); if( possibly_null){ e3= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_new_exp((
void*) e3->r, 0), 0);}{ struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt(
i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) _temp1436, s3, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*)
_temp1438, s1, 0), 0))->r));}} goto _LL2018;}} _LL2022: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2015); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2009); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2054=({ struct Cyc_List_List*(* _temp2045)( struct
_tagged_ptr7)=( struct Cyc_List_List*(*)( struct _tagged_ptr7)) Cyc_List_list;
struct _tuple1* _temp2048=({ struct _tuple1* _temp2053=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2053->f1= 0; _temp2053->f2= Cyc_Toc_mt_tq;
_temp2053->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1438->topt))->v); _temp2053;}); struct _tuple1* _temp2049=({ struct _tuple1*
_temp2052=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2052->f1=
0; _temp2052->f2= Cyc_Toc_mt_tq; _temp2052->f3= Cyc_Absyn_uint_t; _temp2052;});
struct _tuple1* _temp2050=({ struct _tuple1* _temp2051=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2051->f1= 0; _temp2051->f2= Cyc_Toc_mt_tq;
_temp2051->f3= Cyc_Absyn_uint_t; _temp2051;}); struct _tuple1* _temp2046[ 3u]={
_temp2048, _temp2049, _temp2050}; struct _tagged_ptr7 _temp2047={ _temp2046,
_temp2046, _temp2046 + 3u}; _temp2045( _temp2047);}); goto _LL2055; _LL2055: {
void* _temp2059= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2056=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2056[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2057; _temp2057.tag= Cyc_Absyn_FnType;
_temp2057.f1=({ struct Cyc_Absyn_FnInfo _temp2058; _temp2058.tvars= 0; _temp2058.effect=
0; _temp2058.ret_typ=( void*) ta2; _temp2058.args= _temp2054; _temp2058.c_varargs=
0; _temp2058.cyc_varargs= 0; _temp2058.attributes= 0; _temp2058;}); _temp2057;});
_temp2056;}), Cyc_Toc_mt_tq); goto _LL2060; _LL2060: { struct Cyc_Absyn_Exp*
fnexp= Cyc_Absyn_cast_exp( _temp2059, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv,
0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_fncall_exp(
fnexp,({ struct Cyc_List_List*(* _temp2061)( struct _tagged_ptr6)=( struct Cyc_List_List*(*)(
struct _tagged_ptr6)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2064= _temp1438;
struct Cyc_Absyn_Exp* _temp2065= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2066= _temp1436; struct Cyc_Absyn_Exp* _temp2062[ 3u]={ _temp2064,
_temp2065, _temp2066}; struct _tagged_ptr6 _temp2063={ _temp2062, _temp2062,
_temp2062 + 3u}; _temp2061( _temp2063);}), 0), 0))->r));}}} else{ struct _tuple0*
x= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2009);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1436, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1438, s1, 0), 0))->r));} goto _LL2018;} _LL2018:;}
goto _LL1996; _LL2002:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2067="exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp2068; _temp2068.curr= _temp2067; _temp2068.base=
_temp2067; _temp2068.last_plus_one= _temp2067 + 49; _temp2068;})); goto _LL1996;
_LL1996:;} goto _LL1269;} _LL1319: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1440))->r));} else{ struct Cyc_List_List*
_temp2069=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1440); goto
_LL2070; _LL2070: { struct _tagged_string* _temp2071= Cyc_Toc_add_tuple_type(
_temp2069); goto _LL2072; _LL2072: { struct Cyc_List_List* dles= 0;{ int i= 1;
for( 0; _temp1440 != 0;( _temp1440=(( struct Cyc_List_List*) _check_null(
_temp1440))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1440))->hd); dles=({ struct Cyc_List_List*
_temp2073=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2073->hd=( void*)({ struct _tuple4* _temp2074=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2074->f1= 0; _temp2074->f2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1440))->hd; _temp2074;}); _temp2073->tl=
dles; _temp2073;});}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, dles, 0))->r));}}} goto _LL1269; _LL1321:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ unsigned char* _temp2075="compoundlit"; struct _tagged_string
_temp2076; _temp2076.curr= _temp2075; _temp2076.base= _temp2075; _temp2076.last_plus_one=
_temp2075 + 12; _temp2076;})); goto _LL1269; _LL1323:( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1446, 0))->r));{ struct Cyc_List_List*
_temp2077= _temp1446; goto _LL2078; _LL2078: for( 0; _temp2077 != 0; _temp2077=((
struct Cyc_List_List*) _check_null( _temp2077))->tl){ struct _tuple4 _temp2081;
struct Cyc_Absyn_Exp* _temp2082; struct Cyc_List_List* _temp2084; struct _tuple4*
_temp2079=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp2077))->hd;
_temp2081=* _temp2079; _LL2085: _temp2084= _temp2081.f1; goto _LL2083; _LL2083:
_temp2082= _temp2081.f2; goto _LL2080; _LL2080: Cyc_Toc_exp_to_c( nv, _temp2082);}}
goto _LL1269; _LL1325: { unsigned int _temp2086= Cyc_Evexp_eval_const_uint_exp(
_temp1450); goto _LL2087; _LL2087: Cyc_Toc_exp_to_c( nv, _temp1448);{ struct Cyc_List_List*
es= 0; if( ! Cyc_Toc_is_zero( _temp1448)){ unsigned int i= 0; for( 0; i <
_temp2086; i ++){ es=({ struct Cyc_List_List* _temp2088=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2088->hd=( void*)({ struct
_tuple4* _temp2089=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp2089->f1= 0; _temp2089->f2= _temp1448; _temp2089;}); _temp2088->tl= es;
_temp2088;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
es, 0))->r)); goto _LL1269;}} _LL1327: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1456, _temp1460))->r));}
else{ if( _temp1454 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2090="Struct_e: missing structdecl pointer"; struct
_tagged_string _temp2091; _temp2091.curr= _temp2090; _temp2091.base= _temp2090;
_temp2091.last_plus_one= _temp2090 + 37; _temp2091;}));}{ struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*) _check_null( _temp1454); struct
_RegionHandle _temp2092= _new_region(); struct _RegionHandle* rgn=& _temp2092;
_push_region( rgn);{ struct Cyc_List_List* _temp2093=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1456,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd2->fields))->v); goto _LL2094; _LL2094: { struct Cyc_List_List* _temp2095= 0;
goto _LL2096; _LL2096:{ struct Cyc_List_List* _temp2097=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; goto _LL2098; _LL2098: for(
0; _temp2097 != 0; _temp2097=(( struct Cyc_List_List*) _check_null( _temp2097))->tl){
struct Cyc_List_List* _temp2099= _temp2093; goto _LL2100; _LL2100: for( 0;
_temp2099 != 0; _temp2099=(( struct Cyc_List_List*) _check_null( _temp2099))->tl){
if((*(( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2099))->hd)).f1
==( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2097))->hd){ struct _tuple12 _temp2103; struct Cyc_Absyn_Exp* _temp2104;
struct Cyc_Absyn_Structfield* _temp2106; struct _tuple12* _temp2101=( struct
_tuple12*)(( struct Cyc_List_List*) _check_null( _temp2099))->hd; _temp2103=*
_temp2101; _LL2107: _temp2106= _temp2103.f1; goto _LL2105; _LL2105: _temp2104=
_temp2103.f2; goto _LL2102; _LL2102: { void* _temp2108=( void*) _temp2106->type;
goto _LL2109; _LL2109: Cyc_Toc_exp_to_c( nv, _temp2104); if( Cyc_Toc_is_void_star(
_temp2108)){( void*)( _temp2104->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp2104->r, 0), 0))->r));} _temp2095=({ struct Cyc_List_List*
_temp2110=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2110->hd=( void*)({ struct _tuple4* _temp2111=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2111->f1= 0; _temp2111->f2= _temp2104; _temp2111;});
_temp2110->tl= _temp2095; _temp2110;}); break;}}}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp2095), 0))->r));}}; _pop_region(& _temp2092);}} goto
_LL1269; _LL1329: { struct Cyc_List_List* fs;{ void* _temp2112= Cyc_Tcutil_compress(
_temp1464); struct Cyc_List_List* _temp2118; _LL2114: if(( unsigned int)
_temp2112 > 4u?*(( int*) _temp2112) == Cyc_Absyn_AnonStructType: 0){ _LL2119:
_temp2118=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2112)->f1; goto
_LL2115;} else{ goto _LL2116;} _LL2116: goto _LL2117; _LL2115: fs= _temp2118;
goto _LL2113; _LL2117: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp2120= Cyc_Absynpp_typ2string(
_temp1464); xprintf("anon struct has type %.*s", _temp2120.last_plus_one -
_temp2120.curr, _temp2120.curr);})); goto _LL2113; _LL2113:;}{ struct
_RegionHandle _temp2121= _new_region(); struct _RegionHandle* rgn=& _temp2121;
_push_region( rgn);{ struct Cyc_List_List* _temp2122=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1462, fs); goto _LL2123; _LL2123: for( 0; _temp2122 != 0; _temp2122=((
struct Cyc_List_List*) _check_null( _temp2122))->tl){ struct _tuple12 _temp2126;
struct Cyc_Absyn_Exp* _temp2127; struct Cyc_Absyn_Structfield* _temp2129; struct
_tuple12* _temp2124=( struct _tuple12*)(( struct Cyc_List_List*) _check_null(
_temp2122))->hd; _temp2126=* _temp2124; _LL2130: _temp2129= _temp2126.f1; goto
_LL2128; _LL2128: _temp2127= _temp2126.f2; goto _LL2125; _LL2125: { void*
_temp2131=( void*) _temp2129->type; goto _LL2132; _LL2132: Cyc_Toc_exp_to_c( nv,
_temp2127); if( Cyc_Toc_is_void_star( _temp2131)){( void*)( _temp2127->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp2127->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1462, 0))->r));}; _pop_region(& _temp2121);} goto _LL1269;} _LL1331: {
struct _tuple0* qv= _temp1466->name; if( _temp1468->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp2133= _temp1468->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1468->fields))->v;
goto _LL2134; _LL2134: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2133))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp2133))->hd)->typs
== 0){ tag_count ++;} _temp2133=(( struct Cyc_List_List*) _check_null( _temp2133))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1269;} _LL1333: { struct _tuple0* _temp2135= _temp1476->name; goto
_LL2136; _LL2136: { struct Cyc_List_List* _temp2137= _temp1476->typs; goto
_LL2138; _LL2138: { struct _tuple0* _temp2139= Cyc_Toc_temp_var(); goto _LL2140;
_LL2140: { struct Cyc_Absyn_Exp* _temp2141= Cyc_Absyn_var_exp( _temp2139, 0);
goto _LL2142; _LL2142: { void* _temp2145= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2135,({ unsigned char* _temp2143="_struct"; struct _tagged_string _temp2144;
_temp2144.curr= _temp2143; _temp2144.base= _temp2143; _temp2144.last_plus_one=
_temp2143 + 8; _temp2144;}))); goto _LL2146; _LL2146: if( nv->toplevel){ struct
Cyc_Absyn_Exp* tag_exp; if( _temp1478->is_xtunion){ tag_exp= Cyc_Absyn_var_exp(
_temp2135, 0);} else{ int tag_count= 0; struct Cyc_List_List* _temp2147=
_temp1478->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1478->fields))->v; goto _LL2148; _LL2148: while( Cyc_Absyn_qvar_cmp(
_temp2135,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2147))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2147))->hd)->typs != 0){ tag_count ++;}
_temp2147=(( struct Cyc_List_List*) _check_null( _temp2147))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2149= 0; goto _LL2150;
_LL2150: for( 0; _temp1480 != 0;( _temp1480=(( struct Cyc_List_List*)
_check_null( _temp1480))->tl, _temp2137=(( struct Cyc_List_List*) _check_null(
_temp2137))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1480))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2137))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2149=({ struct Cyc_List_List* _temp2151=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2151[ 0]=({ struct Cyc_List_List
_temp2152; _temp2152.hd=( void*)({ struct _tuple4* _temp2153=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2153[ 0]=({ struct _tuple4
_temp2154; _temp2154.f1= 0; _temp2154.f2= cur_e; _temp2154;}); _temp2153;});
_temp2152.tl= _temp2149; _temp2152;}); _temp2151;});} _temp2149=({ struct Cyc_List_List*
_temp2155=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2155[ 0]=({ struct Cyc_List_List _temp2156; _temp2156.hd=( void*)({ struct
_tuple4* _temp2157=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2157[ 0]=({ struct _tuple4 _temp2158; _temp2158.f1= 0; _temp2158.f2=
tag_exp; _temp2158;}); _temp2157;}); _temp2156.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2149); _temp2156;}); _temp2155;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2159=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2159->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2135,({ unsigned char*
_temp2160="_struct"; struct _tagged_string _temp2161; _temp2161.curr= _temp2160;
_temp2161.base= _temp2160; _temp2161.last_plus_one= _temp2160 + 8; _temp2161;}));
_temp2159;}), _temp2149, 0))->r));}} else{ struct Cyc_List_List* _temp2163=({
struct Cyc_List_List* _temp2162=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2162->hd=( void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp2141, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp2135, 0), 0); _temp2162->tl=
0; _temp2162;}); goto _LL2164; _LL2164:{ int i= 1; for( 0; _temp1480 != 0;(((
_temp1480=(( struct Cyc_List_List*) _check_null( _temp1480))->tl, i ++)),
_temp2137=(( struct Cyc_List_List*) _check_null( _temp2137))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1480))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2137))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp2165= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp2141, Cyc_Toc_fieldname(
i), 0), cur_e, 0); goto _LL2166; _LL2166: _temp2163=({ struct Cyc_List_List*
_temp2167=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2167->hd=( void*) _temp2165; _temp2167->tl= _temp2163; _temp2167;});}}}{
struct Cyc_Absyn_Stmt* _temp2168= Cyc_Absyn_exp_stmt( _temp2141, 0); goto
_LL2169; _LL2169: { struct Cyc_Absyn_Stmt* _temp2171= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp2170=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2170->hd=( void*) _temp2168; _temp2170->tl= _temp2163; _temp2170;})), 0);
goto _LL2172; _LL2172:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp2139, _temp2145, 0, _temp2171, 0), 0))->r));}}} goto _LL1269;}}}}} _LL1335:
goto _LL1269; _LL1337: { void* t_c= Cyc_Toc_typ_to_c( _temp1492); if( _temp1494
!= 0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1494);
Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp(
rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} else{( void*)( e->r=( void*)((
void*)( Cyc_Toc_malloc_exp( _temp1492, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
goto _LL1269;} _LL1339: Cyc_Toc_stmt_to_c( nv, _temp1496); goto _LL1269; _LL1341:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char*
_temp2173="UnresolvedMem"; struct _tagged_string _temp2174; _temp2174.curr=
_temp2173; _temp2174.base= _temp2173; _temp2174.last_plus_one= _temp2173 + 14;
_temp2174;})); goto _LL1269; _LL1343:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2175="codegen"; struct _tagged_string _temp2176; _temp2176.curr=
_temp2175; _temp2176.base= _temp2175; _temp2176.last_plus_one= _temp2175 + 8;
_temp2176;})); goto _LL1269; _LL1345:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2177="fill"; struct _tagged_string _temp2178; _temp2178.curr=
_temp2177; _temp2178.base= _temp2177; _temp2178.last_plus_one= _temp2177 + 5;
_temp2178;})); goto _LL1269; _LL1269:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
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
_temp2179=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp2215; struct Cyc_Absyn_Vardecl
_temp2217; struct Cyc_List_List* _temp2218; struct Cyc_Core_Opt* _temp2220;
struct Cyc_Absyn_Exp* _temp2222; void* _temp2224; struct Cyc_Absyn_Tqual
_temp2226; struct _tuple0* _temp2228; void* _temp2230; struct Cyc_Absyn_Vardecl*
_temp2232; int _temp2234; void* _temp2236; unsigned char _temp2238; struct
_tagged_string _temp2240; struct Cyc_Absyn_Enumfield* _temp2242; struct Cyc_Absyn_Enumdecl*
_temp2244; struct Cyc_List_List* _temp2246; struct Cyc_List_List* _temp2248;
struct Cyc_Absyn_Tunionfield* _temp2250; struct Cyc_Absyn_Tuniondecl* _temp2252;
struct Cyc_Absyn_Pat* _temp2255; struct Cyc_Absyn_Pat _temp2257; struct Cyc_Position_Segment*
_temp2258; struct Cyc_Core_Opt* _temp2260; void* _temp2262; struct Cyc_List_List*
_temp2264; struct Cyc_List_List* _temp2266; struct Cyc_Absyn_Tunionfield*
_temp2268; struct Cyc_Absyn_Tuniondecl* _temp2270; struct Cyc_List_List*
_temp2272; struct Cyc_List_List* _temp2274; struct Cyc_Absyn_Tunionfield*
_temp2276; struct Cyc_Absyn_Tuniondecl* _temp2278; struct Cyc_List_List*
_temp2280; struct Cyc_List_List* _temp2282; struct Cyc_List_List* _temp2284;
struct Cyc_Core_Opt* _temp2286; struct Cyc_Absyn_Structdecl* _temp2288; struct
Cyc_Absyn_Pat* _temp2290; struct _tuple0* _temp2292; struct Cyc_List_List*
_temp2294; struct Cyc_List_List* _temp2296; struct _tuple0* _temp2298; struct
Cyc_List_List* _temp2300; struct Cyc_List_List* _temp2302; struct _tuple0*
_temp2304; _LL2181: if(( unsigned int) _temp2179 > 2u?*(( int*) _temp2179) ==
Cyc_Absyn_Var_p: 0){ _LL2216: _temp2215=(( struct Cyc_Absyn_Var_p_struct*)
_temp2179)->f1; _temp2217=* _temp2215; _LL2231: _temp2230=( void*) _temp2217.sc;
goto _LL2229; _LL2229: _temp2228= _temp2217.name; goto _LL2227; _LL2227:
_temp2226= _temp2217.tq; goto _LL2225; _LL2225: _temp2224=( void*) _temp2217.type;
goto _LL2223; _LL2223: _temp2222= _temp2217.initializer; goto _LL2221; _LL2221:
_temp2220= _temp2217.rgn; goto _LL2219; _LL2219: _temp2218= _temp2217.attributes;
goto _LL2182;} else{ goto _LL2183;} _LL2183: if( _temp2179 ==( void*) Cyc_Absyn_Wild_p){
goto _LL2184;} else{ goto _LL2185;} _LL2185: if(( unsigned int) _temp2179 > 2u?*((
int*) _temp2179) == Cyc_Absyn_Reference_p: 0){ _LL2233: _temp2232=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2179)->f1; goto _LL2186;} else{ goto _LL2187;} _LL2187: if( _temp2179 ==(
void*) Cyc_Absyn_Null_p){ goto _LL2188;} else{ goto _LL2189;} _LL2189: if((
unsigned int) _temp2179 > 2u?*(( int*) _temp2179) == Cyc_Absyn_Int_p: 0){
_LL2237: _temp2236=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp2179)->f1;
goto _LL2235; _LL2235: _temp2234=(( struct Cyc_Absyn_Int_p_struct*) _temp2179)->f2;
goto _LL2190;} else{ goto _LL2191;} _LL2191: if(( unsigned int) _temp2179 > 2u?*((
int*) _temp2179) == Cyc_Absyn_Char_p: 0){ _LL2239: _temp2238=(( struct Cyc_Absyn_Char_p_struct*)
_temp2179)->f1; goto _LL2192;} else{ goto _LL2193;} _LL2193: if(( unsigned int)
_temp2179 > 2u?*(( int*) _temp2179) == Cyc_Absyn_Float_p: 0){ _LL2241: _temp2240=((
struct Cyc_Absyn_Float_p_struct*) _temp2179)->f1; goto _LL2194;} else{ goto
_LL2195;} _LL2195: if(( unsigned int) _temp2179 > 2u?*(( int*) _temp2179) == Cyc_Absyn_Enum_p:
0){ _LL2245: _temp2244=(( struct Cyc_Absyn_Enum_p_struct*) _temp2179)->f1; goto
_LL2243; _LL2243: _temp2242=(( struct Cyc_Absyn_Enum_p_struct*) _temp2179)->f2;
goto _LL2196;} else{ goto _LL2197;} _LL2197: if(( unsigned int) _temp2179 > 2u?*((
int*) _temp2179) == Cyc_Absyn_Tunion_p: 0){ _LL2253: _temp2252=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2179)->f1; goto _LL2251; _LL2251: _temp2250=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2179)->f2; goto _LL2249; _LL2249: _temp2248=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2179)->f3; goto _LL2247; _LL2247: _temp2246=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2179)->f4; if( _temp2246 == 0){ goto _LL2198;} else{ goto _LL2199;}} else{
goto _LL2199;} _LL2199: if(( unsigned int) _temp2179 > 2u?*(( int*) _temp2179)
== Cyc_Absyn_Pointer_p: 0){ _LL2256: _temp2255=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2179)->f1; _temp2257=* _temp2255; _LL2263: _temp2262=( void*) _temp2257.r;
if(( unsigned int) _temp2262 > 2u?*(( int*) _temp2262) == Cyc_Absyn_Tunion_p: 0){
_LL2271: _temp2270=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2262)->f1; goto
_LL2269; _LL2269: _temp2268=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2262)->f2;
goto _LL2267; _LL2267: _temp2266=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2262)->f3;
goto _LL2265; _LL2265: _temp2264=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2262)->f4;
goto _LL2261;} else{ goto _LL2201;} _LL2261: _temp2260= _temp2257.topt; goto
_LL2259; _LL2259: _temp2258= _temp2257.loc; goto _LL2254;} else{ goto _LL2201;}
_LL2254: if( _temp2264 != 0){ goto _LL2200;} else{ goto _LL2201;} _LL2201: if((
unsigned int) _temp2179 > 2u?*(( int*) _temp2179) == Cyc_Absyn_Tunion_p: 0){
_LL2279: _temp2278=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2179)->f1; goto
_LL2277; _LL2277: _temp2276=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2179)->f2;
goto _LL2275; _LL2275: _temp2274=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2179)->f3;
goto _LL2273; _LL2273: _temp2272=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2179)->f4;
goto _LL2202;} else{ goto _LL2203;} _LL2203: if(( unsigned int) _temp2179 > 2u?*((
int*) _temp2179) == Cyc_Absyn_Tuple_p: 0){ _LL2281: _temp2280=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp2179)->f1; goto _LL2204;} else{ goto _LL2205;} _LL2205: if(( unsigned int)
_temp2179 > 2u?*(( int*) _temp2179) == Cyc_Absyn_Struct_p: 0){ _LL2289:
_temp2288=(( struct Cyc_Absyn_Struct_p_struct*) _temp2179)->f1; goto _LL2287;
_LL2287: _temp2286=(( struct Cyc_Absyn_Struct_p_struct*) _temp2179)->f2; goto
_LL2285; _LL2285: _temp2284=(( struct Cyc_Absyn_Struct_p_struct*) _temp2179)->f3;
goto _LL2283; _LL2283: _temp2282=(( struct Cyc_Absyn_Struct_p_struct*) _temp2179)->f4;
goto _LL2206;} else{ goto _LL2207;} _LL2207: if(( unsigned int) _temp2179 > 2u?*((
int*) _temp2179) == Cyc_Absyn_Pointer_p: 0){ _LL2291: _temp2290=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2179)->f1; goto _LL2208;} else{ goto _LL2209;} _LL2209: if(( unsigned int)
_temp2179 > 2u?*(( int*) _temp2179) == Cyc_Absyn_UnknownId_p: 0){ _LL2293:
_temp2292=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2179)->f1; goto _LL2210;}
else{ goto _LL2211;} _LL2211: if(( unsigned int) _temp2179 > 2u?*(( int*)
_temp2179) == Cyc_Absyn_UnknownCall_p: 0){ _LL2299: _temp2298=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2179)->f1; goto _LL2297; _LL2297: _temp2296=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2179)->f2; goto _LL2295; _LL2295: _temp2294=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2179)->f3; goto _LL2212;} else{ goto _LL2213;} _LL2213: if(( unsigned int)
_temp2179 > 2u?*(( int*) _temp2179) == Cyc_Absyn_UnknownFields_p: 0){ _LL2305:
_temp2304=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2179)->f1; goto
_LL2303; _LL2303: _temp2302=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2179)->f2; goto _LL2301; _LL2301: _temp2300=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2179)->f3; goto _LL2214;} else{ goto _LL2180;} _LL2182: nv= Cyc_Toc_add_varmap(
nv, _temp2228, r); goto _LL2184; _LL2184: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL2180; _LL2186: { struct _tuple0* _temp2306= Cyc_Toc_temp_var(); goto
_LL2307; _LL2307: decls=({ struct Cyc_List_List* _temp2308=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2308->hd=( void*)({
struct _tuple14* _temp2309=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2309->f1= _temp2306; _temp2309->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2309;}); _temp2308->tl= decls;
_temp2308;}); nv= Cyc_Toc_add_varmap( nv, _temp2232->name, Cyc_Absyn_var_exp(
_temp2306, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2306, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2180;} _LL2188: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2180; _LL2190: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2236, _temp2234, 0), succ_lab, fail_lab); goto _LL2180;
_LL2192: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2238, 0),
succ_lab, fail_lab); goto _LL2180; _LL2194: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2240, 0), succ_lab, fail_lab); goto _LL2180; _LL2196: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2310=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2310[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2311; _temp2311.tag= Cyc_Absyn_Enum_e;
_temp2311.f1= _temp2242->name; _temp2311.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2244; _temp2311.f3=( struct Cyc_Absyn_Enumfield*) _temp2242; _temp2311;});
_temp2310;}), 0), succ_lab, fail_lab); goto _LL2180; _LL2198: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2250->name, 0); if( ! _temp2252->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2180;} _LL2200: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2264);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2268->name,({
unsigned char* _temp2346="_struct"; struct _tagged_string _temp2347; _temp2347.curr=
_temp2346; _temp2347.base= _temp2346; _temp2347.last_plus_one= _temp2346 + 8;
_temp2347;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct Cyc_List_List*
_temp2312=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2268->typs); goto _LL2313; _LL2313: { struct Cyc_List_List*
_temp2314=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2264); goto _LL2315; _LL2315: for( 0; _temp2314 !=
0;((( _temp2314=(( struct Cyc_List_List*) _check_null( _temp2314))->tl,
_temp2312=(( struct Cyc_List_List*) _check_null( _temp2312))->tl)), cnt --)){
struct Cyc_Absyn_Pat* _temp2316=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2314))->hd; goto _LL2317; _LL2317: { void* _temp2318=(*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2312))->hd)).f2;
goto _LL2319; _LL2319: { struct _tuple0* _temp2320= Cyc_Toc_temp_var(); goto
_LL2321; _LL2321: { void* _temp2322=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp2316->topt))->v; goto _LL2323; _LL2323: { void* _temp2324= Cyc_Toc_typ_to_c(
_temp2322); goto _LL2325; _LL2325: { struct _tagged_string* _temp2326= Cyc_Toc_fresh_label();
goto _LL2327; _LL2327: { struct Cyc_Absyn_Exp* _temp2329= Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp2328=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2328[ 0]= xprintf("f%d", cnt);
_temp2328;}), 0); goto _LL2330; _LL2330: if( Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c(
_temp2318))){ _temp2329= Cyc_Absyn_cast_exp( _temp2324, _temp2329, 0);} decls=({
struct Cyc_List_List* _temp2331=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2331->hd=( void*)({ struct _tuple14*
_temp2332=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2332->f1= _temp2320; _temp2332->f2= _temp2324; _temp2332;}); _temp2331->tl=
decls; _temp2331;});{ struct _tuple13 _temp2333= Cyc_Toc_xlate_pat( nv, rgn,
_temp2322, Cyc_Absyn_var_exp( _temp2320, 0), _temp2329, _temp2316, succ_lab,
fail_lab, decls); goto _LL2334; _LL2334: nv= _temp2333.f1; decls= _temp2333.f2;{
struct Cyc_Absyn_Stmt* _temp2335= _temp2333.f3; goto _LL2336; _LL2336: { struct
Cyc_Absyn_Stmt* _temp2337= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2320,
0), _temp2329, 0); goto _LL2338; _LL2338: { struct Cyc_Absyn_Stmt* _temp2339=
Cyc_Absyn_seq_stmt( _temp2337, _temp2335, 0); goto _LL2340; _LL2340: ss=({
struct Cyc_List_List* _temp2341=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2341->hd=( void*) Cyc_Absyn_label_stmt( _temp2326,
_temp2339, 0); _temp2341->tl= ss; _temp2341;}); succ_lab= _temp2326;}}}}}}}}}}}{
struct Cyc_Absyn_Exp* test_exp; if( _temp2270->is_xtunion){ struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0); struct Cyc_Absyn_Exp*
e= Cyc_Absyn_var_exp( _temp2268->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0);
s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp2342=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2270->fields))->v; goto _LL2343; _LL2343: for( 0; _temp2342 !=
0; _temp2342=(( struct Cyc_List_List*) _check_null( _temp2342))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2344=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2342))->hd; goto _LL2345; _LL2345: if( _temp2344->typs == 0){ max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r,
0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp,
0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2268->name, 0); test_exp=
max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2180;}}} _LL2202: _temp2280=
_temp2272; goto _LL2204; _LL2204: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2280);{ struct Cyc_List_List*
_temp2348=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2280); goto _LL2349; _LL2349: for( 0; _temp2348 !=
0;( _temp2348=(( struct Cyc_List_List*) _check_null( _temp2348))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2350=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2348))->hd; goto _LL2351; _LL2351: { struct _tuple0* _temp2352=
Cyc_Toc_temp_var(); goto _LL2353; _LL2353: { void* _temp2354=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2350->topt))->v; goto _LL2355; _LL2355: {
struct _tagged_string* _temp2356= Cyc_Toc_fresh_label(); goto _LL2357; _LL2357:
decls=({ struct Cyc_List_List* _temp2358=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2358->hd=( void*)({ struct _tuple14*
_temp2359=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2359->f1= _temp2352; _temp2359->f2= Cyc_Toc_typ_to_c( _temp2354); _temp2359;});
_temp2358->tl= decls; _temp2358;});{ struct _tuple13 _temp2360= Cyc_Toc_xlate_pat(
nv, rgn, _temp2354, Cyc_Absyn_var_exp( _temp2352, 0), Cyc_Absyn_structmember_exp(
path, Cyc_Toc_fieldname( cnt), 0), _temp2350, succ_lab, fail_lab, decls); goto
_LL2361; _LL2361: nv= _temp2360.f1; decls= _temp2360.f2;{ struct Cyc_Absyn_Stmt*
_temp2362= _temp2360.f3; goto _LL2363; _LL2363: { struct Cyc_Absyn_Stmt*
_temp2364= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2352, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2362, 0); goto _LL2365; _LL2365: ss=({ struct Cyc_List_List* _temp2366=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2366->hd=(
void*) Cyc_Absyn_label_stmt( _temp2356, _temp2364, 0); _temp2366->tl= ss;
_temp2366;}); succ_lab= _temp2356;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2180;} _LL2206: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2367=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2282); goto _LL2368; _LL2368: for( 0; _temp2367 !=
0; _temp2367=(( struct Cyc_List_List*) _check_null( _temp2367))->tl){ struct
_tuple15* _temp2369=( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2367))->hd; goto _LL2370; _LL2370: { struct Cyc_Absyn_Pat* _temp2371=(*
_temp2369).f2; goto _LL2372; _LL2372: { struct _tagged_string* f;{ void*
_temp2373=( void*)(( struct Cyc_List_List*) _check_null((* _temp2369).f1))->hd;
struct _tagged_string* _temp2379; _LL2375: if(*(( int*) _temp2373) == Cyc_Absyn_FieldName){
_LL2380: _temp2379=(( struct Cyc_Absyn_FieldName_struct*) _temp2373)->f1; goto
_LL2376;} else{ goto _LL2377;} _LL2377: goto _LL2378; _LL2376: f= _temp2379;
goto _LL2374; _LL2378:( void) _throw(( void*) Cyc_Toc_Match_error); _LL2374:;}{
struct _tuple0* _temp2381= Cyc_Toc_temp_var(); goto _LL2382; _LL2382: { void*
_temp2383=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2371->topt))->v;
goto _LL2384; _LL2384: { void* _temp2385= Cyc_Toc_typ_to_c( _temp2383); goto
_LL2386; _LL2386: { struct _tagged_string* _temp2387= Cyc_Toc_fresh_label();
goto _LL2388; _LL2388: decls=({ struct Cyc_List_List* _temp2389=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2389->hd=( void*)({
struct _tuple14* _temp2390=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2390->f1= _temp2381; _temp2390->f2= _temp2385; _temp2390;});
_temp2389->tl= decls; _temp2389;});{ struct _tuple13 _temp2391= Cyc_Toc_xlate_pat(
nv, rgn, _temp2383, Cyc_Absyn_var_exp( _temp2381, 0), Cyc_Absyn_structmember_exp(
path, f, 0), _temp2371, succ_lab, fail_lab, decls); goto _LL2392; _LL2392: nv=
_temp2391.f1; decls= _temp2391.f2;{ struct Cyc_Absyn_Exp* _temp2393= Cyc_Absyn_structmember_exp(
r, f, 0); goto _LL2394; _LL2394: if( Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2288->fields))->v, f)))->type)){ _temp2393= Cyc_Absyn_cast_exp(
_temp2385, _temp2393, 0);}{ struct Cyc_Absyn_Stmt* _temp2395= _temp2391.f3; goto
_LL2396; _LL2396: { struct Cyc_Absyn_Stmt* _temp2397= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2381, 0), _temp2393, 0), _temp2395, 0); goto _LL2398;
_LL2398: ss=({ struct Cyc_List_List* _temp2399=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2399->hd=( void*) Cyc_Absyn_label_stmt(
_temp2387, _temp2397, 0); _temp2399->tl= ss; _temp2399;}); succ_lab= _temp2387;}}}}}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2180;} _LL2208: { struct _tuple0*
_temp2400= Cyc_Toc_temp_var(); goto _LL2401; _LL2401: { void* _temp2402=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2290->topt))->v; goto _LL2403; _LL2403:
decls=({ struct Cyc_List_List* _temp2404=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2404->hd=( void*)({ struct _tuple14*
_temp2405=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2405->f1= _temp2400; _temp2405->f2= Cyc_Toc_typ_to_c( _temp2402); _temp2405;});
_temp2404->tl= decls; _temp2404;});{ struct _tuple13 _temp2406= Cyc_Toc_xlate_pat(
nv, rgn, _temp2402, Cyc_Absyn_var_exp( _temp2400, 0), Cyc_Absyn_deref_exp( path,
0), _temp2290, succ_lab, fail_lab, decls); goto _LL2407; _LL2407: nv= _temp2406.f1;
decls= _temp2406.f2;{ struct Cyc_Absyn_Stmt* _temp2408= _temp2406.f3; goto
_LL2409; _LL2409: { struct Cyc_Absyn_Stmt* _temp2410= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2400, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2408, 0);
goto _LL2411; _LL2411: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2410, 0);} else{ s= _temp2410;} goto _LL2180;}}}}} _LL2210: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2412="unknownid"; struct _tagged_string _temp2413; _temp2413.curr=
_temp2412; _temp2413.base= _temp2412; _temp2413.last_plus_one= _temp2412 + 10;
_temp2413;})); goto _LL2180; _LL2212: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2414="unknowncall";
struct _tagged_string _temp2415; _temp2415.curr= _temp2414; _temp2415.base=
_temp2414; _temp2415.last_plus_one= _temp2414 + 12; _temp2415;})); goto _LL2180;
_LL2214: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2416="unknownfields"; struct _tagged_string _temp2417;
_temp2417.curr= _temp2416; _temp2417.base= _temp2416; _temp2417.last_plus_one=
_temp2416 + 14; _temp2417;})); goto _LL2180; _LL2180:;} return({ struct _tuple13
_temp2418; _temp2418.f1= nv; _temp2418.f2= decls; _temp2418.f3= s; _temp2418;});}
struct _tuple16{ struct _tagged_string* f1; struct _tagged_string* f2; struct
Cyc_Absyn_Switch_clause* f3; } ; static struct _tuple16* Cyc_Toc_gen_label(
struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause* sc){ return({ struct
_tuple16* _temp2419=( struct _tuple16*) _region_malloc( r, sizeof( struct
_tuple16)); _temp2419->f1= Cyc_Toc_fresh_label(); _temp2419->f2= Cyc_Toc_fresh_label();
_temp2419->f3= sc; _temp2419;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp2420=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2421; _LL2421: { int leave_as_switch;{ void*
_temp2422= Cyc_Tcutil_compress( _temp2420); void* _temp2428; void* _temp2430;
_LL2424: if(( unsigned int) _temp2422 > 4u?*(( int*) _temp2422) == Cyc_Absyn_IntType:
0){ _LL2431: _temp2430=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2422)->f1;
goto _LL2429; _LL2429: _temp2428=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2422)->f2; goto _LL2425;} else{ goto _LL2426;} _LL2426: goto _LL2427;
_LL2425: leave_as_switch= 1; goto _LL2423; _LL2427: leave_as_switch= 0; goto
_LL2423; _LL2423:;}{ struct Cyc_List_List* _temp2432= scs; goto _LL2433; _LL2433:
for( 0; _temp2432 != 0; _temp2432=(( struct Cyc_List_List*) _check_null(
_temp2432))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2432))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2432))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2434= scs; goto
_LL2435; _LL2435: for( 0; _temp2434 != 0; _temp2434=(( struct Cyc_List_List*)
_check_null( _temp2434))->tl){ struct Cyc_Absyn_Stmt* _temp2436=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2434))->hd)->body; goto _LL2437;
_LL2437:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2434))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2436, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2436);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct
_tagged_string* end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp2438=
_new_region(); struct _RegionHandle* rgn=& _temp2438; _push_region( rgn);{
struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2439= lscs; goto
_LL2440; _LL2440: for( 0; _temp2439 != 0; _temp2439=(( struct Cyc_List_List*)
_check_null( _temp2439))->tl){ struct Cyc_Absyn_Switch_clause* sc=(*(( struct
_tuple16*)(( struct Cyc_List_List*) _check_null( _temp2439))->hd)).f3; struct
_tagged_string* fail_lab=(( struct Cyc_List_List*) _check_null( _temp2439))->tl
== 0? end_l:(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp2439))->tl))->hd)).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2439))->hd)).f2; if( sc->where_clause == 0){ struct _tuple13
_temp2441= Cyc_Toc_xlate_pat( nv, rgn, _temp2420, r, path, sc->pattern, succ_lab,
fail_lab, decls); goto _LL2442; _LL2442: nvs=({ struct Cyc_List_List* _temp2443=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2443->hd=( void*) _temp2441.f1; _temp2443->tl= nvs; _temp2443;}); decls=
_temp2441.f2; test_stmts=({ struct Cyc_List_List* _temp2444=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2444->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2439))->hd)).f1,
_temp2441.f3, 0); _temp2444->tl= test_stmts; _temp2444;});} else{ struct Cyc_Absyn_Exp*
_temp2445=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); goto _LL2446;
_LL2446: { struct _tagged_string* _temp2447= Cyc_Toc_fresh_label(); goto _LL2448;
_LL2448: { struct _tuple13 _temp2449= Cyc_Toc_xlate_pat( nv, rgn, _temp2420, r,
path, sc->pattern, _temp2447, fail_lab, decls); goto _LL2450; _LL2450: Cyc_Toc_exp_to_c(
_temp2449.f1, _temp2445); nvs=({ struct Cyc_List_List* _temp2451=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2451->hd=( void*)
_temp2449.f1; _temp2451->tl= nvs; _temp2451;}); decls= _temp2449.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2445, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2447, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2452=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2452->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2439))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2449.f3, s3, 0), 0); _temp2452->tl= test_stmts;
_temp2452;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2453= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2454; _LL2454: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0;( lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2455=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2456; _LL2456: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2455, end_l,(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2457=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2458; _LL2458: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2457, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2453, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple14 _temp2461;
void* _temp2462; struct _tuple0* _temp2464; struct _tuple14* _temp2459=( struct
_tuple14*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2461=*
_temp2459; _LL2465: _temp2464= _temp2461.f1; goto _LL2463; _LL2463: _temp2462=
_temp2461.f2; goto _LL2460; _LL2460: res= Cyc_Absyn_declare_stmt( _temp2464,
_temp2462, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region(& _temp2438);}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2466=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2466[ 0]=({ struct Cyc_List_List _temp2467; _temp2467.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2467.tl= 0; _temp2467;}); _temp2466;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2468=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2508; struct Cyc_Absyn_Stmt* _temp2510; struct Cyc_Absyn_Stmt*
_temp2512; struct Cyc_Absyn_Exp* _temp2514; struct Cyc_Absyn_Stmt* _temp2516;
struct Cyc_Absyn_Stmt* _temp2518; struct Cyc_Absyn_Exp* _temp2520; struct Cyc_Absyn_Stmt*
_temp2522; struct _tuple2 _temp2524; struct Cyc_Absyn_Stmt* _temp2526; struct
Cyc_Absyn_Exp* _temp2528; struct Cyc_Absyn_Stmt* _temp2530; struct Cyc_Absyn_Stmt*
_temp2532; struct Cyc_Absyn_Stmt* _temp2534; struct _tagged_string* _temp2536;
struct Cyc_Absyn_Stmt* _temp2538; struct _tuple2 _temp2540; struct Cyc_Absyn_Stmt*
_temp2542; struct Cyc_Absyn_Exp* _temp2544; struct _tuple2 _temp2546; struct Cyc_Absyn_Stmt*
_temp2548; struct Cyc_Absyn_Exp* _temp2550; struct Cyc_Absyn_Exp* _temp2552;
struct Cyc_List_List* _temp2554; struct Cyc_Absyn_Exp* _temp2556; struct Cyc_Absyn_Switch_clause**
_temp2558; struct Cyc_List_List* _temp2560; struct Cyc_Absyn_Stmt* _temp2562;
struct Cyc_Absyn_Decl* _temp2564; struct Cyc_Absyn_Stmt* _temp2566; struct Cyc_Absyn_Stmt*
_temp2568; struct Cyc_Absyn_Stmt* _temp2570; struct _tagged_string* _temp2572;
struct _tuple2 _temp2574; struct Cyc_Absyn_Stmt* _temp2576; struct Cyc_Absyn_Exp*
_temp2578; struct Cyc_Absyn_Stmt* _temp2580; struct Cyc_List_List* _temp2582;
struct Cyc_Absyn_Stmt* _temp2584; struct Cyc_Absyn_Stmt* _temp2586; struct Cyc_Absyn_Vardecl*
_temp2588; struct Cyc_Absyn_Tvar* _temp2590; _LL2470: if( _temp2468 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2471;} else{ goto _LL2472;} _LL2472: if((
unsigned int) _temp2468 > 1u?*(( int*) _temp2468) == Cyc_Absyn_Exp_s: 0){
_LL2509: _temp2508=(( struct Cyc_Absyn_Exp_s_struct*) _temp2468)->f1; goto
_LL2473;} else{ goto _LL2474;} _LL2474: if(( unsigned int) _temp2468 > 1u?*((
int*) _temp2468) == Cyc_Absyn_Seq_s: 0){ _LL2513: _temp2512=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2468)->f1; goto _LL2511; _LL2511: _temp2510=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2468)->f2; goto _LL2475;} else{ goto _LL2476;} _LL2476: if(( unsigned int)
_temp2468 > 1u?*(( int*) _temp2468) == Cyc_Absyn_Return_s: 0){ _LL2515:
_temp2514=(( struct Cyc_Absyn_Return_s_struct*) _temp2468)->f1; goto _LL2477;}
else{ goto _LL2478;} _LL2478: if(( unsigned int) _temp2468 > 1u?*(( int*)
_temp2468) == Cyc_Absyn_IfThenElse_s: 0){ _LL2521: _temp2520=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2468)->f1; goto _LL2519; _LL2519: _temp2518=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2468)->f2; goto _LL2517; _LL2517: _temp2516=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2468)->f3; goto _LL2479;} else{ goto _LL2480;} _LL2480: if(( unsigned int)
_temp2468 > 1u?*(( int*) _temp2468) == Cyc_Absyn_While_s: 0){ _LL2525: _temp2524=((
struct Cyc_Absyn_While_s_struct*) _temp2468)->f1; _LL2529: _temp2528= _temp2524.f1;
goto _LL2527; _LL2527: _temp2526= _temp2524.f2; goto _LL2523; _LL2523: _temp2522=((
struct Cyc_Absyn_While_s_struct*) _temp2468)->f2; goto _LL2481;} else{ goto
_LL2482;} _LL2482: if(( unsigned int) _temp2468 > 1u?*(( int*) _temp2468) == Cyc_Absyn_Break_s:
0){ _LL2531: _temp2530=(( struct Cyc_Absyn_Break_s_struct*) _temp2468)->f1; goto
_LL2483;} else{ goto _LL2484;} _LL2484: if(( unsigned int) _temp2468 > 1u?*((
int*) _temp2468) == Cyc_Absyn_Continue_s: 0){ _LL2533: _temp2532=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2468)->f1; goto _LL2485;} else{ goto _LL2486;} _LL2486: if(( unsigned int)
_temp2468 > 1u?*(( int*) _temp2468) == Cyc_Absyn_Goto_s: 0){ _LL2537: _temp2536=((
struct Cyc_Absyn_Goto_s_struct*) _temp2468)->f1; goto _LL2535; _LL2535:
_temp2534=(( struct Cyc_Absyn_Goto_s_struct*) _temp2468)->f2; goto _LL2487;}
else{ goto _LL2488;} _LL2488: if(( unsigned int) _temp2468 > 1u?*(( int*)
_temp2468) == Cyc_Absyn_For_s: 0){ _LL2553: _temp2552=(( struct Cyc_Absyn_For_s_struct*)
_temp2468)->f1; goto _LL2547; _LL2547: _temp2546=(( struct Cyc_Absyn_For_s_struct*)
_temp2468)->f2; _LL2551: _temp2550= _temp2546.f1; goto _LL2549; _LL2549:
_temp2548= _temp2546.f2; goto _LL2541; _LL2541: _temp2540=(( struct Cyc_Absyn_For_s_struct*)
_temp2468)->f3; _LL2545: _temp2544= _temp2540.f1; goto _LL2543; _LL2543:
_temp2542= _temp2540.f2; goto _LL2539; _LL2539: _temp2538=(( struct Cyc_Absyn_For_s_struct*)
_temp2468)->f4; goto _LL2489;} else{ goto _LL2490;} _LL2490: if(( unsigned int)
_temp2468 > 1u?*(( int*) _temp2468) == Cyc_Absyn_Switch_s: 0){ _LL2557:
_temp2556=(( struct Cyc_Absyn_Switch_s_struct*) _temp2468)->f1; goto _LL2555;
_LL2555: _temp2554=(( struct Cyc_Absyn_Switch_s_struct*) _temp2468)->f2; goto
_LL2491;} else{ goto _LL2492;} _LL2492: if(( unsigned int) _temp2468 > 1u?*((
int*) _temp2468) == Cyc_Absyn_Fallthru_s: 0){ _LL2561: _temp2560=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2468)->f1; goto _LL2559; _LL2559: _temp2558=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2468)->f2; goto _LL2493;} else{ goto _LL2494;} _LL2494: if(( unsigned int)
_temp2468 > 1u?*(( int*) _temp2468) == Cyc_Absyn_Decl_s: 0){ _LL2565: _temp2564=((
struct Cyc_Absyn_Decl_s_struct*) _temp2468)->f1; goto _LL2563; _LL2563:
_temp2562=(( struct Cyc_Absyn_Decl_s_struct*) _temp2468)->f2; goto _LL2495;}
else{ goto _LL2496;} _LL2496: if(( unsigned int) _temp2468 > 1u?*(( int*)
_temp2468) == Cyc_Absyn_Cut_s: 0){ _LL2567: _temp2566=(( struct Cyc_Absyn_Cut_s_struct*)
_temp2468)->f1; goto _LL2497;} else{ goto _LL2498;} _LL2498: if(( unsigned int)
_temp2468 > 1u?*(( int*) _temp2468) == Cyc_Absyn_Splice_s: 0){ _LL2569:
_temp2568=(( struct Cyc_Absyn_Splice_s_struct*) _temp2468)->f1; goto _LL2499;}
else{ goto _LL2500;} _LL2500: if(( unsigned int) _temp2468 > 1u?*(( int*)
_temp2468) == Cyc_Absyn_Label_s: 0){ _LL2573: _temp2572=(( struct Cyc_Absyn_Label_s_struct*)
_temp2468)->f1; goto _LL2571; _LL2571: _temp2570=(( struct Cyc_Absyn_Label_s_struct*)
_temp2468)->f2; goto _LL2501;} else{ goto _LL2502;} _LL2502: if(( unsigned int)
_temp2468 > 1u?*(( int*) _temp2468) == Cyc_Absyn_Do_s: 0){ _LL2581: _temp2580=((
struct Cyc_Absyn_Do_s_struct*) _temp2468)->f1; goto _LL2575; _LL2575: _temp2574=((
struct Cyc_Absyn_Do_s_struct*) _temp2468)->f2; _LL2579: _temp2578= _temp2574.f1;
goto _LL2577; _LL2577: _temp2576= _temp2574.f2; goto _LL2503;} else{ goto
_LL2504;} _LL2504: if(( unsigned int) _temp2468 > 1u?*(( int*) _temp2468) == Cyc_Absyn_TryCatch_s:
0){ _LL2585: _temp2584=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2468)->f1;
goto _LL2583; _LL2583: _temp2582=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2468)->f2; goto _LL2505;} else{ goto _LL2506;} _LL2506: if(( unsigned int)
_temp2468 > 1u?*(( int*) _temp2468) == Cyc_Absyn_Region_s: 0){ _LL2591:
_temp2590=(( struct Cyc_Absyn_Region_s_struct*) _temp2468)->f1; goto _LL2589;
_LL2589: _temp2588=(( struct Cyc_Absyn_Region_s_struct*) _temp2468)->f2; goto
_LL2587; _LL2587: _temp2586=(( struct Cyc_Absyn_Region_s_struct*) _temp2468)->f3;
goto _LL2507;} else{ goto _LL2469;} _LL2471: return; _LL2473: Cyc_Toc_exp_to_c(
nv, _temp2508); return; _LL2475: Cyc_Toc_stmt_to_c( nv, _temp2512); s= _temp2510;
continue; _LL2477: { struct Cyc_Core_Opt* topt= 0; if( _temp2514 != 0){ topt=({
struct Cyc_Core_Opt* _temp2592=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2592->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2514))->topt))->v);
_temp2592;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2514));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2593=
Cyc_Toc_temp_var(); goto _LL2594; _LL2594: { struct Cyc_Absyn_Stmt* _temp2595=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2593, 0),
0); goto _LL2596; _LL2596:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2593,( void*)(( struct Cyc_Core_Opt*) _check_null( topt))->v, _temp2514,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), _temp2595, 0), 0))->r));}}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2479: Cyc_Toc_exp_to_c(
nv, _temp2520); Cyc_Toc_stmt_to_c( nv, _temp2518); s= _temp2516; continue;
_LL2481: Cyc_Toc_exp_to_c( nv, _temp2528); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2522); return; _LL2483: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null( nv->break_lab))->v, 0))->r));}{ int dest_depth= _temp2530 == 0? 0:((
struct Cyc_Absyn_Stmt*) _check_null( _temp2530))->try_depth; Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2485: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -(( struct Cyc_Absyn_Stmt*) _check_null( _temp2532))->try_depth, s);
return; _LL2487: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2534))->try_depth, s); return; _LL2489: Cyc_Toc_exp_to_c( nv,
_temp2552); Cyc_Toc_exp_to_c( nv, _temp2550); Cyc_Toc_exp_to_c( nv, _temp2544);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2538); return; _LL2491: Cyc_Toc_xlate_switch(
nv, s, _temp2556, _temp2554); return; _LL2493: if( nv->fallthru_info == 0){(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2597="fallthru in unexpected place"; struct _tagged_string
_temp2598; _temp2598.curr= _temp2597; _temp2598.base= _temp2597; _temp2598.last_plus_one=
_temp2597 + 29; _temp2598;})));}{ struct _tuple8 _temp2601; struct Cyc_Dict_Dict*
_temp2602; struct Cyc_List_List* _temp2604; struct _tagged_string* _temp2606;
struct _tuple8* _temp2599=( struct _tuple8*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp2601=* _temp2599; _LL2607: _temp2606= _temp2601.f1;
goto _LL2605; _LL2605: _temp2604= _temp2601.f2; goto _LL2603; _LL2603: _temp2602=
_temp2601.f3; goto _LL2600; _LL2600: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp2606, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp2558)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp2608=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2604); goto _LL2609; _LL2609: { struct Cyc_List_List* _temp2610=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp2560); goto
_LL2611; _LL2611: for( 0; _temp2608 != 0;( _temp2608=(( struct Cyc_List_List*)
_check_null( _temp2608))->tl, _temp2610=(( struct Cyc_List_List*) _check_null(
_temp2610))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp2610))->hd); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2602,( struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp2608))->hd),(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp2610))->hd, 0),
s2, 0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}}} _LL2495:{ void*
_temp2612=( void*) _temp2564->r; int _temp2622; struct Cyc_Absyn_Exp* _temp2624;
struct Cyc_Core_Opt* _temp2626; struct Cyc_Core_Opt* _temp2628; struct Cyc_Absyn_Pat*
_temp2630; struct Cyc_List_List* _temp2632; struct Cyc_Absyn_Vardecl* _temp2634;
_LL2614: if(*(( int*) _temp2612) == Cyc_Absyn_Let_d){ _LL2631: _temp2630=((
struct Cyc_Absyn_Let_d_struct*) _temp2612)->f1; goto _LL2629; _LL2629: _temp2628=((
struct Cyc_Absyn_Let_d_struct*) _temp2612)->f2; goto _LL2627; _LL2627: _temp2626=((
struct Cyc_Absyn_Let_d_struct*) _temp2612)->f3; goto _LL2625; _LL2625: _temp2624=((
struct Cyc_Absyn_Let_d_struct*) _temp2612)->f4; goto _LL2623; _LL2623: _temp2622=((
struct Cyc_Absyn_Let_d_struct*) _temp2612)->f5; goto _LL2615;} else{ goto
_LL2616;} _LL2616: if(*(( int*) _temp2612) == Cyc_Absyn_Letv_d){ _LL2633:
_temp2632=(( struct Cyc_Absyn_Letv_d_struct*) _temp2612)->f1; goto _LL2617;}
else{ goto _LL2618;} _LL2618: if(*(( int*) _temp2612) == Cyc_Absyn_Var_d){
_LL2635: _temp2634=(( struct Cyc_Absyn_Var_d_struct*) _temp2612)->f1; goto
_LL2619;} else{ goto _LL2620;} _LL2620: goto _LL2621; _LL2615:( void*)( s->r=(
void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2630, _temp2626, _temp2624,
_temp2622, _temp2562))->r)); goto _LL2613; _LL2617: { struct Cyc_List_List*
_temp2636=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2632); goto _LL2637; _LL2637: if( _temp2636 == 0){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2638="empty Letv_d";
struct _tagged_string _temp2639; _temp2639.curr= _temp2638; _temp2639.base=
_temp2638; _temp2639.last_plus_one= _temp2638 + 13; _temp2639;}));}( void*)(
_temp2564->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2640=(
struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2640[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2641; _temp2641.tag= Cyc_Absyn_Var_d;
_temp2641.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2636))->hd; _temp2641;}); _temp2640;}))); _temp2636=(( struct Cyc_List_List*)
_check_null( _temp2636))->tl; for( 0; _temp2636 != 0; _temp2636=(( struct Cyc_List_List*)
_check_null( _temp2636))->tl){ struct Cyc_Absyn_Decl* _temp2644= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2642=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2642[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2643; _temp2643.tag= Cyc_Absyn_Var_d; _temp2643.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2636))->hd; _temp2643;}); _temp2642;}),
0); goto _LL2645; _LL2645:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp2644, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c(
nv, s); goto _LL2613;} _LL2619: { struct _tuple0* _temp2646= _temp2634->name;
goto _LL2647; _LL2647:( void*)( _temp2634->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2634->type));{ struct Cyc_Toc_Env* _temp2650= Cyc_Toc_add_varmap( nv,
_temp2646, Cyc_Absyn_varb_exp( _temp2646,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2648=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2648[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2649; _temp2649.tag= Cyc_Absyn_Local_b;
_temp2649.f1= _temp2634; _temp2649;}); _temp2648;}), 0)); goto _LL2651; _LL2651:
Cyc_Toc_stmt_to_c( _temp2650, _temp2562); if( _temp2634->initializer != 0){
struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*) _check_null( _temp2634->initializer);
void* _temp2652=( void*) init->r; struct Cyc_Absyn_Exp* _temp2658; struct Cyc_Absyn_Exp*
_temp2660; struct Cyc_Absyn_Vardecl* _temp2662; _LL2654: if(*(( int*) _temp2652)
== Cyc_Absyn_Comprehension_e){ _LL2663: _temp2662=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2652)->f1; goto _LL2661; _LL2661: _temp2660=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2652)->f2; goto _LL2659; _LL2659: _temp2658=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2652)->f3; goto _LL2655;} else{ goto _LL2656;} _LL2656: goto _LL2657;
_LL2655: _temp2634->initializer= 0;( void*)( _temp2562->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2646, 0), _temp2662, _temp2660, _temp2658, Cyc_Absyn_new_stmt((
void*) _temp2562->r, 0), 0))->r)); goto _LL2653; _LL2657: Cyc_Toc_exp_to_c( nv,
init); goto _LL2653; _LL2653:;} goto _LL2613;}} _LL2621:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2664="bad nested declaration within function";
struct _tagged_string _temp2665; _temp2665.curr= _temp2664; _temp2665.base=
_temp2664; _temp2665.last_plus_one= _temp2664 + 39; _temp2665;})); goto _LL2613;
_LL2613:;} return; _LL2497:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2666="cut"; struct _tagged_string _temp2667; _temp2667.curr=
_temp2666; _temp2667.base= _temp2666; _temp2667.last_plus_one= _temp2666 + 4;
_temp2667;})); return; _LL2499:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2668="splice"; struct _tagged_string _temp2669; _temp2669.curr=
_temp2668; _temp2669.base= _temp2668; _temp2669.last_plus_one= _temp2668 + 7;
_temp2669;})); return; _LL2501: s= _temp2570; continue; _LL2503: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp2580); Cyc_Toc_exp_to_c( nv, _temp2578); return;
_LL2505: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__push_handler_qv,
0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp( Cyc_Toc_setjmp_qv, 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__pop_handler_qv,
0); struct Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp( Cyc_Toc__exn_thrown_qv,
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2670=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2670->v=( void*) e_typ; _temp2670;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2584);{
struct Cyc_Absyn_Stmt* _temp2671= Cyc_Absyn_seq_stmt( _temp2584, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); goto _LL2672; _LL2672: {
struct _tuple0* _temp2673= Cyc_Toc_temp_var(); goto _LL2674; _LL2674: { struct
Cyc_Absyn_Exp* _temp2675= Cyc_Absyn_var_exp( _temp2673, 0); goto _LL2676;
_LL2676: { struct Cyc_Absyn_Vardecl* _temp2677= Cyc_Absyn_new_vardecl( _temp2673,
Cyc_Absyn_exn_typ, 0); goto _LL2678; _LL2678: _temp2675->topt=({ struct Cyc_Core_Opt*
_temp2679=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2679->v=( void*) Cyc_Absyn_exn_typ; _temp2679;});{ struct Cyc_Absyn_Pat*
_temp2684=({ struct Cyc_Absyn_Pat* _temp2680=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2680->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2682=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2682[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2683; _temp2683.tag= Cyc_Absyn_Var_p;
_temp2683.f1= _temp2677; _temp2683;}); _temp2682;})); _temp2680->topt=({ struct
Cyc_Core_Opt* _temp2681=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2681->v=( void*) Cyc_Absyn_exn_typ; _temp2681;}); _temp2680->loc= 0;
_temp2680;}); goto _LL2685; _LL2685: { struct Cyc_Absyn_Exp* _temp2686= Cyc_Absyn_throw_exp(
_temp2675, 0); goto _LL2687; _LL2687: _temp2686->topt=({ struct Cyc_Core_Opt*
_temp2688=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2688->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2688;});{ struct Cyc_Absyn_Stmt*
_temp2689= Cyc_Absyn_exp_stmt( _temp2686, 0); goto _LL2690; _LL2690: { struct
Cyc_Absyn_Switch_clause* _temp2694=({ struct Cyc_Absyn_Switch_clause* _temp2691=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2691->pattern= _temp2684; _temp2691->pat_vars=({ struct Cyc_Core_Opt*
_temp2692=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2692->v=( void*)({ struct Cyc_List_List* _temp2693=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2693->hd=( void*) _temp2677;
_temp2693->tl= 0; _temp2693;}); _temp2692;}); _temp2691->where_clause= 0;
_temp2691->body= _temp2689; _temp2691->loc= 0; _temp2691;}); goto _LL2695;
_LL2695: { struct Cyc_Absyn_Stmt* _temp2697= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2582,({ struct Cyc_List_List* _temp2696=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2696->hd=( void*) _temp2694; _temp2696->tl=
0; _temp2696;})), 0); goto _LL2698; _LL2698: Cyc_Toc_stmt_to_c( nv, _temp2697);{
struct Cyc_Absyn_Exp* _temp2700= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2699=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2699->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2699->tl= 0; _temp2699;}), 0); goto _LL2701; _LL2701: { struct Cyc_Absyn_Stmt*
_temp2703= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2702=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2702->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2702->tl= 0;
_temp2702;}), 0), 0); goto _LL2704; _LL2704: { struct Cyc_Absyn_Exp* _temp2705=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2706; _LL2706: {
struct Cyc_Absyn_Exp* _temp2707= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2708; _LL2708:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2703, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2705, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2700, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2707, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2671, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2697, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2507: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2588->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2714=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2714[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2715; _temp2715.tag= Cyc_Absyn_Local_b; _temp2715.f1=
_temp2588; _temp2715;}); _temp2714;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2713=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2713->f1= Cyc_Toc_abs_ns; _temp2713->f2= Cyc_Toc__push_region_sp;
_temp2713;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2712=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2712->f1= Cyc_Toc_abs_ns; _temp2712->f2= Cyc_Toc__pop_region_sp; _temp2712;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2711=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2711->f1=
Cyc_Toc_abs_ns; _temp2711->f2= Cyc_Toc__new_region_sp; _temp2711;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2710=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2710->hd=( void*) addr_rh_exp; _temp2710->tl= 0; _temp2710;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2709=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2709->hd=( void*) x_exp; _temp2709->tl= 0; _temp2709;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2586);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2586, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2469:;}} struct _tuple17{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0;(
void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{ struct
Cyc_List_List* _temp2716= f->args; goto _LL2717; _LL2717: for( 0; _temp2716 != 0;
_temp2716=(( struct Cyc_List_List*) _check_null( _temp2716))->tl){(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp2716))->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp2716))->hd)).f3);{
struct _tuple0* _temp2719=({ struct _tuple0* _temp2718=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2718->f1=( void*) Cyc_Absyn_Loc_n;
_temp2718->f2=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp2716))->hd)).f1; _temp2718;}); goto _LL2720; _LL2720: nv= Cyc_Toc_add_varmap(
nv, _temp2719, Cyc_Absyn_var_exp( _temp2719, 0));}}} if(( unsigned int) f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs))->name != 0: 0){ int
_temp2723; void* _temp2725; void* _temp2727; struct Cyc_Absyn_Tqual _temp2729;
struct Cyc_Core_Opt* _temp2731; struct Cyc_Absyn_VarargInfo _temp2721=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs)); _LL2732: _temp2731=
_temp2721.name; goto _LL2730; _LL2730: _temp2729= _temp2721.tq; goto _LL2728;
_LL2728: _temp2727=( void*) _temp2721.type; goto _LL2726; _LL2726: _temp2725=(
void*) _temp2721.rgn; goto _LL2724; _LL2724: _temp2723= _temp2721.inject; goto
_LL2722; _LL2722: { void* _temp2733= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp2727, _temp2725, _temp2729)); goto _LL2734; _LL2734: { struct _tuple0*
_temp2736=({ struct _tuple0* _temp2735=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2735->f1=( void*) Cyc_Absyn_Loc_n; _temp2735->f2=( struct
_tagged_string*)(( struct Cyc_Core_Opt*) _check_null( _temp2731))->v; _temp2735;});
goto _LL2737; _LL2737: f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2738=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2738->hd=( void*)({ struct _tuple17* _temp2739=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp2739->f1=( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( _temp2731))->v; _temp2739->f2= _temp2729;
_temp2739->f3= _temp2733; _temp2739;}); _temp2738->tl= 0; _temp2738;})); nv= Cyc_Toc_add_varmap(
nv, _temp2736, Cyc_Absyn_var_exp( _temp2736, 0)); f->cyc_varargs= 0;}}}{ struct
Cyc_List_List* _temp2740=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; goto _LL2741; _LL2741: for( 0; _temp2740 !=
0; _temp2740=(( struct Cyc_List_List*) _check_null( _temp2740))->tl){( void*)(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp2740))->hd)->type=(
void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp2740))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2742= s;
_LL2744: if( _temp2742 ==( void*) Cyc_Absyn_Abstract){ goto _LL2745;} else{ goto
_LL2746;} _LL2746: if( _temp2742 ==( void*) Cyc_Absyn_ExternC){ goto _LL2747;}
else{ goto _LL2748;} _LL2748: goto _LL2749; _LL2745: return( void*) Cyc_Absyn_Public;
_LL2747: return( void*) Cyc_Absyn_Extern; _LL2749: return s; _LL2743:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2750=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2751; _LL2751: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2752=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2752->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2752;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2753=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2750).f2); goto _LL2754;
_LL2754: if( _temp2753 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2750).f2, s));}
else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null(
_temp2753))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2750).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2755=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2755->v=( void*)
_temp2750; _temp2755;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2756=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2757; _LL2757: for( 0; _temp2756 != 0;
_temp2756=(( struct Cyc_List_List*) _check_null( _temp2756))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2756))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2756))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2758=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2759; _LL2759: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2760=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2760->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2760;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2761=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2758).f2); goto _LL2762; _LL2762:
if( _temp2761 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2758).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2761))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2758).f2, u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)(
u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==(
void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?( void*)
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2763=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2763->v=( void*) _temp2758; _temp2763;}); if( u->fields !=
0){ if( seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* _temp2764=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( u->fields))->v; goto
_LL2765; _LL2765: for( 0; _temp2764 != 0; _temp2764=(( struct Cyc_List_List*)
_check_null( _temp2764))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2764))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2764))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2766=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2766->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2766;});}{ struct
_tuple0* _temp2767= tud->name; goto _LL2768; _LL2768: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2767)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2767));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2769=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2770; _LL2770: for( 0;
_temp2769 != 0; _temp2769=(( struct Cyc_List_List*) _check_null( _temp2769))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2769))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2771; _temp2771.q_const= 1; _temp2771.q_volatile= 0;
_temp2771.q_restrict= 0; _temp2771;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2772=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2772->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2773=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2773[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2774; _temp2774.tag= Cyc_Absyn_Var_d;
_temp2774.f1= vd; _temp2774;}); _temp2773;}), 0); _temp2772->tl= Cyc_Toc_result_decls;
_temp2772;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2775; _temp2775.q_const=
1; _temp2775.q_volatile= 0; _temp2775.q_restrict= 0; _temp2775;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2776=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2776->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2777=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2777[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2778; _temp2778.tag= Cyc_Absyn_Var_d; _temp2778.f1= vd; _temp2778;});
_temp2777;}), 0); _temp2776->tl= Cyc_Toc_result_decls; _temp2776;});{ struct Cyc_List_List*
_temp2779= 0; goto _LL2780; _LL2780: { int i= 1;{ struct Cyc_List_List*
_temp2781= f->typs; goto _LL2782; _LL2782: for( 0; _temp2781 != 0;( _temp2781=((
struct Cyc_List_List*) _check_null( _temp2781))->tl, i ++)){ struct
_tagged_string* _temp2783= Cyc_Toc_fieldname( i); goto _LL2784; _LL2784: {
struct Cyc_Absyn_Structfield* _temp2786=({ struct Cyc_Absyn_Structfield*
_temp2785=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2785->name= _temp2783; _temp2785->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2781))->hd)).f1; _temp2785->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2781))->hd)).f2);
_temp2785->width= 0; _temp2785->attributes= 0; _temp2785;}); goto _LL2787;
_LL2787: _temp2779=({ struct Cyc_List_List* _temp2788=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2788->hd=( void*) _temp2786;
_temp2788->tl= _temp2779; _temp2788;});}}} _temp2779=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2779); _temp2779=({ struct Cyc_List_List*
_temp2789=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2789->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2790=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2790->name= Cyc_Toc_tag_sp;
_temp2790->tq= Cyc_Toc_mt_tq; _temp2790->type=( void*) Cyc_Absyn_sint_t;
_temp2790->width= 0; _temp2790->attributes= 0; _temp2790;}); _temp2789->tl=
_temp2779; _temp2789;});{ struct Cyc_Absyn_Structdecl* _temp2796=({ struct Cyc_Absyn_Structdecl*
_temp2791=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2791->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2791->name=({ struct Cyc_Core_Opt*
_temp2793=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2793->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2794="_struct"; struct _tagged_string _temp2795; _temp2795.curr= _temp2794;
_temp2795.base= _temp2794; _temp2795.last_plus_one= _temp2794 + 8; _temp2795;}));
_temp2793;}); _temp2791->tvs= 0; _temp2791->fields=({ struct Cyc_Core_Opt*
_temp2792=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2792->v=( void*) _temp2779; _temp2792;}); _temp2791->attributes= 0;
_temp2791;}); goto _LL2797; _LL2797: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2798=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2798->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2799=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2799[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2800; _temp2800.tag= Cyc_Absyn_Struct_d;
_temp2800.f1= _temp2796; _temp2800;}); _temp2799;}), 0); _temp2798->tl= Cyc_Toc_result_decls;
_temp2798;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2801=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2801->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2801;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2802= xd->name; goto _LL2803; _LL2803: {
struct Cyc_List_List* _temp2804=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2805; _LL2805: for( 0; _temp2804 != 0;
_temp2804=(( struct Cyc_List_List*) _check_null( _temp2804))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2804))->hd; struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2807= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2806=* fn;(
unsigned int)( _temp2806.last_plus_one - _temp2806.curr);}) + 4, 0); goto
_LL2808; _LL2808: { struct Cyc_Absyn_ArrayType_struct* _temp2811=({ struct Cyc_Absyn_ArrayType_struct*
_temp2809=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2809[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2810; _temp2810.tag= Cyc_Absyn_ArrayType;
_temp2810.f1=( void*) Cyc_Absyn_uchar_t; _temp2810.f2= Cyc_Toc_mt_tq; _temp2810.f3=(
struct Cyc_Absyn_Exp*) _temp2807; _temp2810;}); _temp2809;}); goto _LL2812;
_LL2812: { struct Cyc_Core_Opt* _temp2813=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2821; int _temp2822; _LL2815: if( _temp2813 == 0){ goto
_LL2816;} else{ goto _LL2817;} _LL2817: if( _temp2813 == 0){ goto _LL2819;}
else{ _temp2821=* _temp2813; _LL2823: _temp2822=( int) _temp2821.v; if(
_temp2822 == 0){ goto _LL2818;} else{ goto _LL2819;}} _LL2819: goto _LL2820;
_LL2816: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(({ int _temp2824= 0; int
_temp2825= 0; int _temp2826= 0; int _temp2827= 0; struct _tagged_string
_temp2828=* fn; xprintf("%c%c%c%c%.*s", _temp2824, _temp2825, _temp2826,
_temp2827, _temp2828.last_plus_one - _temp2828.curr, _temp2828.curr);}), 0);}{
struct Cyc_Absyn_Vardecl* _temp2829= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2811, initopt); goto _LL2830; _LL2830:( void*)( _temp2829->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2831=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2831->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2832=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2832[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2833; _temp2833.tag= Cyc_Absyn_Var_d;
_temp2833.f1= _temp2829; _temp2833;}); _temp2832;}), 0); _temp2831->tl= Cyc_Toc_result_decls;
_temp2831;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2834= f->typs; goto _LL2835; _LL2835: for( 0;
_temp2834 != 0;( _temp2834=(( struct Cyc_List_List*) _check_null( _temp2834))->tl,
i ++)){ struct _tagged_string* _temp2837=( struct _tagged_string*)({ struct
_tagged_string* _temp2836=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2836[ 0]= xprintf("f%d", i); _temp2836;}); goto
_LL2838; _LL2838: { struct Cyc_Absyn_Structfield* _temp2840=({ struct Cyc_Absyn_Structfield*
_temp2839=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2839->name= _temp2837; _temp2839->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2834))->hd)).f1; _temp2839->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2834))->hd)).f2);
_temp2839->width= 0; _temp2839->attributes= 0; _temp2839;}); goto _LL2841;
_LL2841: fields=({ struct Cyc_List_List* _temp2842=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2842->hd=( void*) _temp2840;
_temp2842->tl= fields; _temp2842;});}}} fields=({ struct Cyc_List_List*
_temp2843=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2843->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2844=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2844->name= Cyc_Toc_tag_sp;
_temp2844->tq= Cyc_Toc_mt_tq; _temp2844->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2844->width= 0; _temp2844->attributes= 0; _temp2844;});
_temp2843->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2843;});{ struct Cyc_Absyn_Structdecl* _temp2850=({ struct Cyc_Absyn_Structdecl*
_temp2845=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2845->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2845->name=({ struct Cyc_Core_Opt*
_temp2847=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2847->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2848="_struct"; struct _tagged_string _temp2849; _temp2849.curr= _temp2848;
_temp2849.base= _temp2848; _temp2849.last_plus_one= _temp2848 + 8; _temp2849;}));
_temp2847;}); _temp2845->tvs= 0; _temp2845->fields=({ struct Cyc_Core_Opt*
_temp2846=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2846->v=( void*) fields; _temp2846;}); _temp2845->attributes= 0; _temp2845;});
goto _LL2851; _LL2851: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2852=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2852->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2853=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2853[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2854; _temp2854.tag= Cyc_Absyn_Struct_d;
_temp2854.f1= _temp2850; _temp2854;}); _temp2853;}), 0); _temp2852->tl= Cyc_Toc_result_decls;
_temp2852;});}} goto _LL2814;}} _LL2818: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2860= Cyc_Absyn_string_exp(({ int _temp2855= 0; int
_temp2856= 0; int _temp2857= 0; int _temp2858= 0; struct _tagged_string
_temp2859=* fn; xprintf("%c%c%c%c%.*s", _temp2855, _temp2856, _temp2857,
_temp2858, _temp2859.last_plus_one - _temp2859.curr, _temp2859.curr);}), 0);
goto _LL2861; _LL2861: { struct Cyc_Absyn_Vardecl* _temp2862= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2811,( struct Cyc_Absyn_Exp*) _temp2860); goto _LL2863;
_LL2863:( void*)( _temp2862->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2864=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2864->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2865=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2865[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2866; _temp2866.tag= Cyc_Absyn_Var_d; _temp2866.f1= _temp2862; _temp2866;});
_temp2865;}), 0); _temp2864->tl= Cyc_Toc_result_decls; _temp2864;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2814; _LL2820: goto
_LL2814; _LL2814:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2867= ed->name; goto
_LL2868; _LL2868:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2869=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; goto _LL2870; _LL2870: for( 0; _temp2869 != 0;
_temp2869=(( struct Cyc_List_List*) _check_null( _temp2869))->tl){ if((( struct
Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( _temp2869))->hd)->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp2869))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2869))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct _RegionHandle
_temp2871= _new_region(); struct _RegionHandle* prgn=& _temp2871; _push_region(
prgn);{ struct Cyc_Absyn_Stmt* _temp2874; struct Cyc_List_List* _temp2876;
struct Cyc_Toc_Env* _temp2878; struct _tuple13 _temp2872= Cyc_Toc_xlate_pat( nv,
prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2879: _temp2878= _temp2872.f1; goto _LL2877; _LL2877: _temp2876=
_temp2872.f2; goto _LL2875; _LL2875: _temp2874= _temp2872.f3; goto _LL2873;
_LL2873: Cyc_Toc_stmt_to_c( _temp2878, s);{ struct Cyc_Absyn_Stmt* _temp2880=
Cyc_Absyn_label_stmt( succ_lab, s, 0); goto _LL2881; _LL2881: if( exhaust){ s=
Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2874, _temp2880, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2883= Cyc_Toc_newthrow_exp(
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2882=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2882->f1= Cyc_Toc_abs_ns; _temp2882->f2= Cyc_Toc_Match_Exception_sp;
_temp2882;}), 0)); goto _LL2884; _LL2884: { struct Cyc_Absyn_Stmt* _temp2885=
Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2883, 0), 0); goto
_LL2886; _LL2886: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2874, Cyc_Absyn_seq_stmt( _temp2885,
_temp2880, 0), 0), 0);}} for( 0; _temp2876 != 0; _temp2876=(( struct Cyc_List_List*)
_check_null( _temp2876))->tl){ struct _tuple14 _temp2889; void* _temp2890;
struct _tuple0* _temp2892; struct _tuple14* _temp2887=( struct _tuple14*)((
struct Cyc_List_List*) _check_null( _temp2876))->hd; _temp2889=* _temp2887;
_LL2893: _temp2892= _temp2889.f1; goto _LL2891; _LL2891: _temp2890= _temp2889.f2;
goto _LL2888; _LL2888: s= Cyc_Absyn_declare_stmt( _temp2892, _temp2890, 0, s, 0);}}};
_pop_region(& _temp2871);} return s;}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=((
struct Cyc_List_List*) _check_null( ds))->tl){ struct Cyc_Absyn_Decl* d=( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2894=(
void*) d->r; struct Cyc_Absyn_Vardecl* _temp2920; struct Cyc_Absyn_Fndecl*
_temp2922; int _temp2924; struct Cyc_Absyn_Exp* _temp2926; struct Cyc_Core_Opt*
_temp2928; struct Cyc_Core_Opt* _temp2930; struct Cyc_Absyn_Pat* _temp2932;
struct Cyc_List_List* _temp2934; struct Cyc_Absyn_Structdecl* _temp2936; struct
Cyc_Absyn_Uniondecl* _temp2938; struct Cyc_Absyn_Tuniondecl* _temp2940; struct
Cyc_Absyn_Enumdecl* _temp2942; struct Cyc_Absyn_Typedefdecl* _temp2944; struct
Cyc_List_List* _temp2946; struct _tagged_string* _temp2948; struct Cyc_List_List*
_temp2950; struct _tuple0* _temp2952; struct Cyc_List_List* _temp2954; _LL2896:
if(*(( int*) _temp2894) == Cyc_Absyn_Var_d){ _LL2921: _temp2920=(( struct Cyc_Absyn_Var_d_struct*)
_temp2894)->f1; goto _LL2897;} else{ goto _LL2898;} _LL2898: if(*(( int*)
_temp2894) == Cyc_Absyn_Fn_d){ _LL2923: _temp2922=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2894)->f1; goto _LL2899;} else{ goto _LL2900;} _LL2900: if(*(( int*)
_temp2894) == Cyc_Absyn_Let_d){ _LL2933: _temp2932=(( struct Cyc_Absyn_Let_d_struct*)
_temp2894)->f1; goto _LL2931; _LL2931: _temp2930=(( struct Cyc_Absyn_Let_d_struct*)
_temp2894)->f2; goto _LL2929; _LL2929: _temp2928=(( struct Cyc_Absyn_Let_d_struct*)
_temp2894)->f3; goto _LL2927; _LL2927: _temp2926=(( struct Cyc_Absyn_Let_d_struct*)
_temp2894)->f4; goto _LL2925; _LL2925: _temp2924=(( struct Cyc_Absyn_Let_d_struct*)
_temp2894)->f5; goto _LL2901;} else{ goto _LL2902;} _LL2902: if(*(( int*)
_temp2894) == Cyc_Absyn_Letv_d){ _LL2935: _temp2934=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2894)->f1; goto _LL2903;} else{ goto _LL2904;} _LL2904: if(*(( int*)
_temp2894) == Cyc_Absyn_Struct_d){ _LL2937: _temp2936=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2894)->f1; goto _LL2905;} else{ goto _LL2906;} _LL2906: if(*(( int*)
_temp2894) == Cyc_Absyn_Union_d){ _LL2939: _temp2938=(( struct Cyc_Absyn_Union_d_struct*)
_temp2894)->f1; goto _LL2907;} else{ goto _LL2908;} _LL2908: if(*(( int*)
_temp2894) == Cyc_Absyn_Tunion_d){ _LL2941: _temp2940=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2894)->f1; goto _LL2909;} else{ goto _LL2910;} _LL2910: if(*(( int*)
_temp2894) == Cyc_Absyn_Enum_d){ _LL2943: _temp2942=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2894)->f1; goto _LL2911;} else{ goto _LL2912;} _LL2912: if(*(( int*)
_temp2894) == Cyc_Absyn_Typedef_d){ _LL2945: _temp2944=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2894)->f1; goto _LL2913;} else{ goto _LL2914;} _LL2914: if(*(( int*)
_temp2894) == Cyc_Absyn_Namespace_d){ _LL2949: _temp2948=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2894)->f1; goto _LL2947; _LL2947: _temp2946=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2894)->f2; goto _LL2915;} else{ goto _LL2916;} _LL2916: if(*(( int*)
_temp2894) == Cyc_Absyn_Using_d){ _LL2953: _temp2952=(( struct Cyc_Absyn_Using_d_struct*)
_temp2894)->f1; goto _LL2951; _LL2951: _temp2950=(( struct Cyc_Absyn_Using_d_struct*)
_temp2894)->f2; goto _LL2917;} else{ goto _LL2918;} _LL2918: if(*(( int*)
_temp2894) == Cyc_Absyn_ExternC_d){ _LL2955: _temp2954=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2894)->f1; goto _LL2919;} else{ goto _LL2895;} _LL2897: { struct _tuple0*
_temp2956= _temp2920->name; goto _LL2957; _LL2957: if(( void*) _temp2920->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2956=({ struct _tuple0* _temp2958=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2958->f1= Cyc_Toc_abs_ns;
_temp2958->f2=(* _temp2956).f2; _temp2958;});} if( _temp2920->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null( _temp2920->initializer));}
if( ! nv->toplevel){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp2959="decls_to_c: not at toplevel!"; struct _tagged_string
_temp2960; _temp2960.curr= _temp2959; _temp2960.base= _temp2959; _temp2960.last_plus_one=
_temp2959 + 29; _temp2960;}));} nv= Cyc_Toc_add_varmap( nv, _temp2920->name, Cyc_Absyn_varb_exp(
_temp2956,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp2961=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp2961[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp2962; _temp2962.tag= Cyc_Absyn_Global_b;
_temp2962.f1= _temp2920; _temp2962;}); _temp2961;}), 0)); _temp2920->name=
_temp2956;( void*)( _temp2920->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2920->sc));(
void*)( _temp2920->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2920->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2963=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2963->hd=( void*) d; _temp2963->tl=
Cyc_Toc_result_decls; _temp2963;}); goto _LL2895;} _LL2899: { struct _tuple0*
_temp2964= _temp2922->name; goto _LL2965; _LL2965: if(( void*) _temp2922->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2964=({ struct _tuple0* _temp2966=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2966->f1= Cyc_Toc_abs_ns;
_temp2966->f2=(* _temp2964).f2; _temp2966;});} nv= Cyc_Toc_add_varmap( nv,
_temp2922->name, Cyc_Absyn_var_exp( _temp2964, 0)); _temp2922->name= _temp2964;
Cyc_Toc_fndecl_to_c( nv, _temp2922); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2967=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2967->hd=( void*) d; _temp2967->tl= Cyc_Toc_result_decls; _temp2967;});
goto _LL2895;} _LL2901:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2968="letdecl"; struct _tagged_string _temp2969; _temp2969.curr=
_temp2968; _temp2969.base= _temp2968; _temp2969.last_plus_one= _temp2968 + 8;
_temp2969;})); goto _LL2895; _LL2903:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2970="letdecl"; struct _tagged_string _temp2971; _temp2971.curr=
_temp2970; _temp2971.base= _temp2970; _temp2971.last_plus_one= _temp2970 + 8;
_temp2971;})); goto _LL2895; _LL2905: Cyc_Toc_structdecl_to_c( _temp2936); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2972=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2972->hd=( void*) d; _temp2972->tl= Cyc_Toc_result_decls;
_temp2972;}); goto _LL2895; _LL2907: Cyc_Toc_uniondecl_to_c( _temp2938); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2973=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2973->hd=( void*) d; _temp2973->tl= Cyc_Toc_result_decls;
_temp2973;}); goto _LL2895; _LL2909: if( _temp2940->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2940);} else{ Cyc_Toc_tuniondecl_to_c( _temp2940);} goto _LL2895; _LL2911:
Cyc_Toc_enumdecl_to_c( nv, _temp2942); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2974=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2974->hd=( void*) d; _temp2974->tl= Cyc_Toc_result_decls; _temp2974;});
goto _LL2895; _LL2913: _temp2944->name= _temp2944->name; _temp2944->tvs= 0;(
void*)( _temp2944->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2944->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2975=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2975->hd=( void*) d; _temp2975->tl=
Cyc_Toc_result_decls; _temp2975;}); goto _LL2895; _LL2915: _temp2950= _temp2946;
goto _LL2917; _LL2917: _temp2954= _temp2950; goto _LL2919; _LL2919: nv= Cyc_Toc_decls_to_c(
nv, _temp2954, top); goto _LL2895; _LL2895:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}