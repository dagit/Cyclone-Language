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
Cyc_Core_opt_t; extern int Cyc_Core_intcmp( int, int); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern
unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
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
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length(
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_map2( void*(* f)( void*, void*),
struct Cyc_List_List* x, struct Cyc_List_List* y); extern void Cyc_List_iter2(
void(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_exists_c(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y); struct
_tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; extern struct
_tuple0 Cyc_List_rsplit( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x); extern int Cyc_List_mem( int(* compare)( void*, void*),
struct Cyc_List_List* l, void* x); extern void* Cyc_List_assoc_cmp( int(*
compare)( void*, void*), struct Cyc_List_List* l, void* x); struct Cyc_Lineno_Pos{
struct _tagged_string logical_file; struct _tagged_string line; int line_no; int
col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern unsigned char
Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment*
Cyc_Position_seg_t; extern struct Cyc_List_List* Cyc_Position_strings_of_segments(
struct Cyc_List_List*); static const int Cyc_Position_Lex= 0; static const int
Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_string); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); typedef
struct _tagged_string* Cyc_Absyn_field_name_t; typedef struct _tagged_string*
Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef
void* Cyc_Absyn_nmspace_t; struct _tuple1{ void* f1; struct _tagged_string* f2;
} ; typedef struct _tuple1* Cyc_Absyn_qvar_t; typedef struct _tuple1* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1* Cyc_Absyn_typedef_name_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t;
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
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp(
struct _tuple1*, struct _tuple1*); extern int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern
void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
void*); extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t; extern
void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern void* Cyc_Absyn_bounds_one; extern void*
Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*,
struct _tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_string*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_string*);
struct _tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern struct _tuple4*
Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List*, int); extern struct
_tagged_string Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_kind2string( void*); extern
struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); struct
Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set*
Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern unsigned char
Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_gdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern void* Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Uniondecl**
Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); extern struct _tagged_string Cyc_String_strconcat(
struct _tagged_string, struct _tagged_string); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern
void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern void* Cyc_Tcutil_typ_kind(
void* t); extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_is_function_type( void* t); extern void* Cyc_Tcutil_max_arithmetic_type(
void*, void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_rsubstitute( struct
_RegionHandle*, struct Cyc_List_List*, void*); extern int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2); extern int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_Position_Segment*); struct _tuple5{ struct Cyc_Absyn_Tvar* f1;
void* f2; } ; extern struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*);
extern struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _RegionHandle*, struct
Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*);
extern struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*, void* k, void*); extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct
_tagged_string err_msg); extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*,
struct Cyc_List_List*); extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*, void* t,
struct Cyc_Absyn_Exp* e); struct _tuple6{ int f1; void* f2; } ; extern struct
_tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e); extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k); extern int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*);
extern int Cyc_Tcutil_same_atts( struct Cyc_List_List*, struct Cyc_List_List*);
extern int Cyc_Tcutil_bits_only( void* t); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern void* Cyc_Tcutil_snd_tqt(
struct _tuple4*); extern int Cyc_Tcutil_supports_default( void*); extern void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
unsigned char Cyc_Tcutil_TypeErr[ 12u]="\000\000\000\000TypeErr"; extern void
Cyc_Tcutil_unify_it( void* t1, void* t2); void Cyc_Tcutil_terr( struct Cyc_Position_Segment*
loc, struct _tagged_string s){ Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc, s));} void* Cyc_Tcutil_impos( struct _tagged_string msg){({ struct
_tagged_string _temp0= msg; fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: %.*s\n",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(( void*) Cyc_Tcutil_TypeErr);}
static struct _tagged_string Cyc_Tcutil_tvar2string( struct Cyc_Absyn_Tvar* tv){
return* tv->name;} void Cyc_Tcutil_print_tvars( struct Cyc_List_List* tvs){ for(
0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){({ struct
_tagged_string _temp1= Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs))->hd); struct _tagged_string _temp2= Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s::%.*s ", _temp1.last_plus_one -
_temp1.curr, _temp1.curr, _temp2.last_plus_one - _temp2.curr, _temp2.curr);});}
fprintf( sfile_to_file( Cyc_Stdio_stderr),"\n"); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);} static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0;
static struct Cyc_List_List* Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn(
struct Cyc_Position_Segment* sg, struct _tagged_string msg){ Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp3=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp3->hd=( void*) sg; _temp3->tl= Cyc_Tcutil_warning_segs;
_temp3;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp4=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4->hd=( void*)(( struct
_tagged_string*)({ struct _tagged_string* _temp5=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5[ 0]= msg; _temp5;}));
_temp4->tl= Cyc_Tcutil_warning_msgs; _temp4;});} void Cyc_Tcutil_flush_warnings(){
if( Cyc_Tcutil_warning_segs == 0){ return;} fprintf( sfile_to_file( Cyc_Stdio_stderr),"***Warnings***\n");{
struct Cyc_List_List* seg_strs= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while(
Cyc_Tcutil_warning_msgs != 0) {({ struct _tagged_string _temp6=*(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( seg_strs))->hd); struct
_tagged_string _temp7=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( Cyc_Tcutil_warning_msgs))->hd); fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: %.*s\n",
_temp6.last_plus_one - _temp6.curr, _temp6.curr, _temp7.last_plus_one - _temp7.curr,
_temp7.curr);}); seg_strs=(( struct Cyc_List_List*) _check_null( seg_strs))->tl;
Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->tl;}
fprintf( sfile_to_file( Cyc_Stdio_stderr),"**************\n"); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set=
0; void* Cyc_Tcutil_compress( void* t){ void* _temp8= t; int _temp20; struct Cyc_Core_Opt*
_temp22; void* _temp24; int _temp26; struct Cyc_Core_Opt* _temp28; struct Cyc_Core_Opt**
_temp30; void* _temp31; struct Cyc_Core_Opt* _temp33; struct Cyc_List_List*
_temp35; struct _tuple1* _temp37; struct Cyc_Core_Opt* _temp39; struct Cyc_Core_Opt**
_temp41; struct Cyc_List_List* _temp42; struct _tuple1* _temp44; _LL10: if((
unsigned int) _temp8 > 4u?*(( int*) _temp8) == Cyc_Absyn_Evar: 0){ _LL25:
_temp24=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp8)->f1; goto _LL23; _LL23:
_temp22=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f2; if( _temp22 == 0){ goto
_LL21;} else{ goto _LL12;} _LL21: _temp20=(( struct Cyc_Absyn_Evar_struct*)
_temp8)->f3; goto _LL11;} else{ goto _LL12;} _LL12: if(( unsigned int) _temp8 >
4u?*(( int*) _temp8) == Cyc_Absyn_Evar: 0){ _LL32: _temp31=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp8)->f1; goto _LL29; _LL29: _temp28=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f2;
_temp30=&(( struct Cyc_Absyn_Evar_struct*) _temp8)->f2; goto _LL27; _LL27:
_temp26=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f3; goto _LL13;} else{ goto
_LL14;} _LL14: if(( unsigned int) _temp8 > 4u?*(( int*) _temp8) == Cyc_Absyn_TypedefType:
0){ _LL38: _temp37=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f1; goto
_LL36; _LL36: _temp35=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f2; goto
_LL34; _LL34: _temp33=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f3; if(
_temp33 == 0){ goto _LL15;} else{ goto _LL16;}} else{ goto _LL16;} _LL16: if((
unsigned int) _temp8 > 4u?*(( int*) _temp8) == Cyc_Absyn_TypedefType: 0){ _LL45:
_temp44=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f1; goto _LL43; _LL43:
_temp42=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f2; goto _LL40; _LL40:
_temp39=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f3; _temp41=&(( struct
Cyc_Absyn_TypedefType_struct*) _temp8)->f3; goto _LL17;} else{ goto _LL18;}
_LL18: goto _LL19; _LL11: return t; _LL13: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp30))->v); if( t2 !=( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp30))->v){* _temp30=({ struct Cyc_Core_Opt*
_temp46=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp46->v=( void*) t2; _temp46;});} return t2;} _LL15: return t; _LL17: { void*
t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp41))->v);
if( t2 !=( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp41))->v){* _temp41=({
struct Cyc_Core_Opt* _temp47=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp47->v=( void*) t2; _temp47;});} return t2;} _LL19: return t;
_LL9:;} struct _tuple7{ void* f1; void* f2; } ; static int Cyc_Tcutil_kind_leq(
void* k1, void* k2){ if( k1 == k2){ return 1;}{ struct _tuple7 _temp49=({ struct
_tuple7 _temp48; _temp48.f1= k1; _temp48.f2= k2; _temp48;}); void* _temp59; void*
_temp61; void* _temp63; void* _temp65; void* _temp67; void* _temp69; _LL51:
_LL62: _temp61= _temp49.f1; if( _temp61 ==( void*) Cyc_Absyn_BoxKind){ goto
_LL60;} else{ goto _LL53;} _LL60: _temp59= _temp49.f2; if( _temp59 ==( void*)
Cyc_Absyn_MemKind){ goto _LL52;} else{ goto _LL53;} _LL53: _LL66: _temp65=
_temp49.f1; if( _temp65 ==( void*) Cyc_Absyn_BoxKind){ goto _LL64;} else{ goto
_LL55;} _LL64: _temp63= _temp49.f2; if( _temp63 ==( void*) Cyc_Absyn_AnyKind){
goto _LL54;} else{ goto _LL55;} _LL55: _LL70: _temp69= _temp49.f1; if( _temp69
==( void*) Cyc_Absyn_MemKind){ goto _LL68;} else{ goto _LL57;} _LL68: _temp67=
_temp49.f2; if( _temp67 ==( void*) Cyc_Absyn_AnyKind){ goto _LL56;} else{ goto
_LL57;} _LL57: goto _LL58; _LL52: return 1; _LL54: return 1; _LL56: return 1;
_LL58: return 0; _LL50:;}} void* Cyc_Tcutil_typ_kind( void* t){ void* _temp71=
Cyc_Tcutil_compress( t); int _temp123; struct Cyc_Core_Opt* _temp125; void*
_temp127; struct Cyc_Absyn_Tvar* _temp129; void* _temp131; void* _temp133;
struct Cyc_Absyn_FnInfo _temp135; void* _temp137; struct Cyc_Absyn_TunionInfo
_temp139; struct Cyc_Absyn_TunionFieldInfo _temp141; struct Cyc_List_List*
_temp143; void* _temp145; struct Cyc_Absyn_Tunionfield* _temp147; struct Cyc_Absyn_Tuniondecl*
_temp149; struct Cyc_Absyn_TunionFieldInfo _temp151; struct Cyc_List_List*
_temp153; void* _temp155; struct Cyc_Absyn_UnknownTunionFieldInfo _temp157;
struct Cyc_Absyn_Structdecl** _temp159; struct Cyc_List_List* _temp161; struct
_tuple1* _temp163; struct Cyc_Absyn_Uniondecl** _temp165; struct Cyc_List_List*
_temp167; struct _tuple1* _temp169; struct Cyc_Absyn_Structdecl** _temp171;
struct Cyc_List_List* _temp173; struct _tuple1* _temp175; struct Cyc_Absyn_Uniondecl**
_temp177; struct Cyc_List_List* _temp179; struct _tuple1* _temp181; struct Cyc_List_List*
_temp183; struct Cyc_List_List* _temp185; struct Cyc_Absyn_Enumdecl* _temp187;
struct _tuple1* _temp189; struct Cyc_Absyn_PtrInfo _temp191; struct Cyc_Absyn_Exp*
_temp193; struct Cyc_Absyn_Tqual _temp195; void* _temp197; struct Cyc_List_List*
_temp199; struct Cyc_Core_Opt* _temp201; struct Cyc_List_List* _temp203; struct
_tuple1* _temp205; void* _temp207; struct Cyc_List_List* _temp209; _LL73: if((
unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_Evar: 0){ _LL128:
_temp127=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp71)->f1; goto _LL126;
_LL126: _temp125=(( struct Cyc_Absyn_Evar_struct*) _temp71)->f2; goto _LL124;
_LL124: _temp123=(( struct Cyc_Absyn_Evar_struct*) _temp71)->f3; goto _LL74;}
else{ goto _LL75;} _LL75: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) ==
Cyc_Absyn_VarType: 0){ _LL130: _temp129=(( struct Cyc_Absyn_VarType_struct*)
_temp71)->f1; goto _LL76;} else{ goto _LL77;} _LL77: if( _temp71 ==( void*) Cyc_Absyn_VoidType){
goto _LL78;} else{ goto _LL79;} _LL79: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_IntType: 0){ _LL134: _temp133=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp71)->f1; goto _LL132; _LL132: _temp131=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp71)->f2; goto _LL80;} else{ goto _LL81;} _LL81: if( _temp71 ==( void*) Cyc_Absyn_FloatType){
goto _LL82;} else{ goto _LL83;} _LL83: if( _temp71 ==( void*) Cyc_Absyn_DoubleType){
goto _LL84;} else{ goto _LL85;} _LL85: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_FnType: 0){ _LL136: _temp135=(( struct Cyc_Absyn_FnType_struct*)
_temp71)->f1; goto _LL86;} else{ goto _LL87;} _LL87: if(( unsigned int) _temp71
> 4u?*(( int*) _temp71) == Cyc_Absyn_RgnHandleType: 0){ _LL138: _temp137=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp71)->f1; goto _LL88;} else{ goto
_LL89;} _LL89: if( _temp71 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL90;} else{
goto _LL91;} _LL91: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_TunionType:
0){ _LL140: _temp139=(( struct Cyc_Absyn_TunionType_struct*) _temp71)->f1; goto
_LL92;} else{ goto _LL93;} _LL93: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_TunionFieldType: 0){ _LL142: _temp141=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp71)->f1; _LL146: _temp145=( void*) _temp141.field_info; if(*(( int*)
_temp145) == Cyc_Absyn_KnownTunionfield){ _LL150: _temp149=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp145)->f1; goto _LL148; _LL148: _temp147=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp145)->f2; goto _LL144;} else{ goto _LL95;} _LL144: _temp143= _temp141.targs;
goto _LL94;} else{ goto _LL95;} _LL95: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_TunionFieldType: 0){ _LL152: _temp151=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp71)->f1; _LL156: _temp155=( void*) _temp151.field_info; if(*(( int*)
_temp155) == Cyc_Absyn_UnknownTunionfield){ _LL158: _temp157=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp155)->f1; goto _LL154;} else{ goto _LL97;} _LL154: _temp153= _temp151.targs;
goto _LL96;} else{ goto _LL97;} _LL97: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_StructType: 0){ _LL164: _temp163=(( struct Cyc_Absyn_StructType_struct*)
_temp71)->f1; goto _LL162; _LL162: _temp161=(( struct Cyc_Absyn_StructType_struct*)
_temp71)->f2; goto _LL160; _LL160: _temp159=(( struct Cyc_Absyn_StructType_struct*)
_temp71)->f3; if( _temp159 == 0){ goto _LL98;} else{ goto _LL99;}} else{ goto
_LL99;} _LL99: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_UnionType:
0){ _LL170: _temp169=(( struct Cyc_Absyn_UnionType_struct*) _temp71)->f1; goto
_LL168; _LL168: _temp167=(( struct Cyc_Absyn_UnionType_struct*) _temp71)->f2;
goto _LL166; _LL166: _temp165=(( struct Cyc_Absyn_UnionType_struct*) _temp71)->f3;
if( _temp165 == 0){ goto _LL100;} else{ goto _LL101;}} else{ goto _LL101;}
_LL101: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_StructType:
0){ _LL176: _temp175=(( struct Cyc_Absyn_StructType_struct*) _temp71)->f1; goto
_LL174; _LL174: _temp173=(( struct Cyc_Absyn_StructType_struct*) _temp71)->f2;
goto _LL172; _LL172: _temp171=(( struct Cyc_Absyn_StructType_struct*) _temp71)->f3;
goto _LL102;} else{ goto _LL103;} _LL103: if(( unsigned int) _temp71 > 4u?*((
int*) _temp71) == Cyc_Absyn_UnionType: 0){ _LL182: _temp181=(( struct Cyc_Absyn_UnionType_struct*)
_temp71)->f1; goto _LL180; _LL180: _temp179=(( struct Cyc_Absyn_UnionType_struct*)
_temp71)->f2; goto _LL178; _LL178: _temp177=(( struct Cyc_Absyn_UnionType_struct*)
_temp71)->f3; goto _LL104;} else{ goto _LL105;} _LL105: if(( unsigned int)
_temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_AnonStructType: 0){ _LL184:
_temp183=(( struct Cyc_Absyn_AnonStructType_struct*) _temp71)->f1; goto _LL106;}
else{ goto _LL107;} _LL107: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71)
== Cyc_Absyn_AnonUnionType: 0){ _LL186: _temp185=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp71)->f1; goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int)
_temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_EnumType: 0){ _LL190: _temp189=((
struct Cyc_Absyn_EnumType_struct*) _temp71)->f1; goto _LL188; _LL188: _temp187=((
struct Cyc_Absyn_EnumType_struct*) _temp71)->f2; goto _LL110;} else{ goto _LL111;}
_LL111: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_PointerType:
0){ _LL192: _temp191=(( struct Cyc_Absyn_PointerType_struct*) _temp71)->f1; goto
_LL112;} else{ goto _LL113;} _LL113: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_ArrayType: 0){ _LL198: _temp197=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f1; goto _LL196; _LL196: _temp195=(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f2; goto _LL194; _LL194: _temp193=(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f3; goto _LL114;} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_TupleType: 0){ _LL200: _temp199=((
struct Cyc_Absyn_TupleType_struct*) _temp71)->f1; goto _LL116;} else{ goto
_LL117;} _LL117: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_TypedefType:
0){ _LL206: _temp205=(( struct Cyc_Absyn_TypedefType_struct*) _temp71)->f1; goto
_LL204; _LL204: _temp203=(( struct Cyc_Absyn_TypedefType_struct*) _temp71)->f2;
goto _LL202; _LL202: _temp201=(( struct Cyc_Absyn_TypedefType_struct*) _temp71)->f3;
goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int) _temp71 > 4u?*((
int*) _temp71) == Cyc_Absyn_AccessEff: 0){ _LL208: _temp207=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp71)->f1; goto _LL120;} else{ goto _LL121;}
_LL121: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_JoinEff:
0){ _LL210: _temp209=(( struct Cyc_Absyn_JoinEff_struct*) _temp71)->f1; goto
_LL122;} else{ goto _LL72;} _LL74: return _temp127; _LL76: return(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp129->kind); _LL78:
return( void*) Cyc_Absyn_MemKind; _LL80: return _temp131 ==( void*) Cyc_Absyn_B4?(
void*) Cyc_Absyn_BoxKind:( void*) Cyc_Absyn_MemKind; _LL82: return( void*) Cyc_Absyn_MemKind;
_LL84: return( void*) Cyc_Absyn_MemKind; _LL86: return( void*) Cyc_Absyn_MemKind;
_LL88: return( void*) Cyc_Absyn_BoxKind; _LL90: return( void*) Cyc_Absyn_RgnKind;
_LL92: return( void*) Cyc_Absyn_BoxKind; _LL94: if( _temp147->typs == 0){ return(
void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;} _LL96:
return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp211="typ_kind: Unresolved TunionFieldType"; struct
_tagged_string _temp212; _temp212.curr= _temp211; _temp212.base= _temp211;
_temp212.last_plus_one= _temp211 + 37; _temp212;})); _LL98: return( void*) Cyc_Absyn_AnyKind;
_LL100: return( void*) Cyc_Absyn_AnyKind; _LL102: return( void*) Cyc_Absyn_MemKind;
_LL104: return( void*) Cyc_Absyn_MemKind; _LL106: return( void*) Cyc_Absyn_MemKind;
_LL108: return( void*) Cyc_Absyn_MemKind; _LL110: return( void*) Cyc_Absyn_BoxKind;
_LL112: { void* _temp213=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp191.bounds))->v; void* _temp223; void*
_temp225; struct Cyc_Absyn_Exp* _temp227; struct Cyc_Absyn_Conref* _temp229;
_LL215: if(( unsigned int) _temp213 > 1u?*(( int*) _temp213) == Cyc_Absyn_Eq_constr:
0){ _LL224: _temp223=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp213)->f1;
if( _temp223 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL216;} else{ goto _LL217;}}
else{ goto _LL217;} _LL217: if(( unsigned int) _temp213 > 1u?*(( int*) _temp213)
== Cyc_Absyn_Eq_constr: 0){ _LL226: _temp225=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp213)->f1; if(( unsigned int) _temp225 > 1u?*(( int*) _temp225) == Cyc_Absyn_Upper_b:
0){ _LL228: _temp227=(( struct Cyc_Absyn_Upper_b_struct*) _temp225)->f1; goto
_LL218;} else{ goto _LL219;}} else{ goto _LL219;} _LL219: if( _temp213 ==( void*)
Cyc_Absyn_No_constr){ goto _LL220;} else{ goto _LL221;} _LL221: if((
unsigned int) _temp213 > 1u?*(( int*) _temp213) == Cyc_Absyn_Forward_constr: 0){
_LL230: _temp229=(( struct Cyc_Absyn_Forward_constr_struct*) _temp213)->f1; goto
_LL222;} else{ goto _LL214;} _LL216: return( void*) Cyc_Absyn_MemKind; _LL218:
return( void*) Cyc_Absyn_BoxKind; _LL220: return( void*) Cyc_Absyn_MemKind;
_LL222: return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp231="typ_kind: forward constraint in ptr bounds"; struct
_tagged_string _temp232; _temp232.curr= _temp231; _temp232.base= _temp231;
_temp232.last_plus_one= _temp231 + 43; _temp232;})); _LL214:;} _LL114: return(
void*) Cyc_Absyn_MemKind; _LL116: return( void*) Cyc_Absyn_MemKind; _LL118:
return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct
_tagged_string _temp233= Cyc_Absynpp_typ2string( t); xprintf("typ_kind: typedef found: %.*s",
_temp233.last_plus_one - _temp233.curr, _temp233.curr);})); _LL120: return( void*)
Cyc_Absyn_EffKind; _LL122: return( void*) Cyc_Absyn_EffKind; _LL72:;}
unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify(
void* t1, void* t2){ struct _handler_cons _temp234; _push_handler(& _temp234);{
int _temp236= 0; if( setjmp( _temp234.handler)){ _temp236= 1;} if( ! _temp236){
Cyc_Tcutil_unify_it( t1, t2);{ int _temp237= 1; _npop_handler( 0u); return
_temp237;}; _pop_handler();} else{ void* _temp235=( void*) _exn_thrown; void*
_temp239= _temp235; _LL241: if( _temp239 == Cyc_Tcutil_Unify){ goto _LL242;}
else{ goto _LL243;} _LL243: goto _LL244; _LL242: return 0; _LL244:( void) _throw(
_temp239); _LL240:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
ts); static void Cyc_Tcutil_occurs( void* evar, void* t){ void* _temp245= Cyc_Tcutil_compress(
t); int _temp277; struct Cyc_Core_Opt* _temp279; void* _temp281; struct Cyc_Absyn_PtrInfo
_temp283; struct Cyc_Absyn_Exp* _temp285; struct Cyc_Absyn_Tqual _temp287; void*
_temp289; struct Cyc_Absyn_FnInfo _temp291; struct Cyc_List_List* _temp293;
struct Cyc_Absyn_VarargInfo* _temp295; int _temp297; struct Cyc_List_List*
_temp299; void* _temp301; struct Cyc_Core_Opt* _temp303; struct Cyc_List_List*
_temp305; struct Cyc_List_List* _temp307; void* _temp309; struct Cyc_Absyn_TunionInfo
_temp311; void* _temp313; struct Cyc_List_List* _temp315; void* _temp317; struct
Cyc_Core_Opt* _temp319; struct Cyc_List_List* _temp321; struct _tuple1* _temp323;
struct Cyc_Absyn_TunionFieldInfo _temp325; struct Cyc_List_List* _temp327; void*
_temp329; struct Cyc_Absyn_Structdecl** _temp331; struct Cyc_List_List* _temp333;
struct _tuple1* _temp335; struct Cyc_List_List* _temp337; struct Cyc_List_List*
_temp339; void* _temp341; struct Cyc_List_List* _temp343; _LL247: if((
unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_Evar: 0){ _LL282:
_temp281=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp245)->f1; goto _LL280;
_LL280: _temp279=(( struct Cyc_Absyn_Evar_struct*) _temp245)->f2; goto _LL278;
_LL278: _temp277=(( struct Cyc_Absyn_Evar_struct*) _temp245)->f3; goto _LL248;}
else{ goto _LL249;} _LL249: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245)
== Cyc_Absyn_PointerType: 0){ _LL284: _temp283=(( struct Cyc_Absyn_PointerType_struct*)
_temp245)->f1; goto _LL250;} else{ goto _LL251;} _LL251: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_ArrayType: 0){ _LL290: _temp289=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp245)->f1; goto _LL288; _LL288:
_temp287=(( struct Cyc_Absyn_ArrayType_struct*) _temp245)->f2; goto _LL286;
_LL286: _temp285=(( struct Cyc_Absyn_ArrayType_struct*) _temp245)->f3; goto
_LL252;} else{ goto _LL253;} _LL253: if(( unsigned int) _temp245 > 4u?*(( int*)
_temp245) == Cyc_Absyn_FnType: 0){ _LL292: _temp291=(( struct Cyc_Absyn_FnType_struct*)
_temp245)->f1; _LL306: _temp305= _temp291.tvars; goto _LL304; _LL304: _temp303=
_temp291.effect; goto _LL302; _LL302: _temp301=( void*) _temp291.ret_typ; goto
_LL300; _LL300: _temp299= _temp291.args; goto _LL298; _LL298: _temp297= _temp291.c_varargs;
goto _LL296; _LL296: _temp295= _temp291.cyc_varargs; goto _LL294; _LL294:
_temp293= _temp291.attributes; goto _LL254;} else{ goto _LL255;} _LL255: if((
unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_TupleType: 0){
_LL308: _temp307=(( struct Cyc_Absyn_TupleType_struct*) _temp245)->f1; goto
_LL256;} else{ goto _LL257;} _LL257: if(( unsigned int) _temp245 > 4u?*(( int*)
_temp245) == Cyc_Absyn_RgnHandleType: 0){ _LL310: _temp309=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp245)->f1; goto _LL258;} else{ goto _LL259;} _LL259: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_TunionType: 0){ _LL312: _temp311=((
struct Cyc_Absyn_TunionType_struct*) _temp245)->f1; _LL318: _temp317=( void*)
_temp311.tunion_info; goto _LL316; _LL316: _temp315= _temp311.targs; goto _LL314;
_LL314: _temp313=( void*) _temp311.rgn; goto _LL260;} else{ goto _LL261;} _LL261:
if(( unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_TypedefType: 0){
_LL324: _temp323=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f1; goto
_LL322; _LL322: _temp321=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f2;
goto _LL320; _LL320: _temp319=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f3;
goto _LL262;} else{ goto _LL263;} _LL263: if(( unsigned int) _temp245 > 4u?*((
int*) _temp245) == Cyc_Absyn_TunionFieldType: 0){ _LL326: _temp325=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp245)->f1; _LL330: _temp329=( void*) _temp325.field_info; goto _LL328;
_LL328: _temp327= _temp325.targs; goto _LL264;} else{ goto _LL265;} _LL265: if((
unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_StructType: 0){
_LL336: _temp335=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f1; goto
_LL334; _LL334: _temp333=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f2;
goto _LL332; _LL332: _temp331=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f3;
goto _LL266;} else{ goto _LL267;} _LL267: if(( unsigned int) _temp245 > 4u?*((
int*) _temp245) == Cyc_Absyn_AnonStructType: 0){ _LL338: _temp337=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp245)->f1; goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_AnonUnionType: 0){ _LL340:
_temp339=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp245)->f1; goto _LL270;}
else{ goto _LL271;} _LL271: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245)
== Cyc_Absyn_AccessEff: 0){ _LL342: _temp341=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp245)->f1; goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_JoinEff: 0){ _LL344: _temp343=((
struct Cyc_Absyn_JoinEff_struct*) _temp245)->f1; goto _LL274;} else{ goto _LL275;}
_LL275: goto _LL276; _LL248: if( t == evar){( void) _throw(( void*) Cyc_Tcutil_Unify);}
else{ if( _temp279 != 0){ Cyc_Tcutil_occurs( evar,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp279))->v);}} goto _LL246; _LL250: Cyc_Tcutil_occurs( evar,(
void*) _temp283.elt_typ); Cyc_Tcutil_occurs( evar,( void*) _temp283.rgn_typ);
goto _LL246; _LL252: Cyc_Tcutil_occurs( evar, _temp289); goto _LL246; _LL254:
if( _temp303 != 0){ Cyc_Tcutil_occurs( evar,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp303))->v);} Cyc_Tcutil_occurs( evar, _temp301); for( 0;
_temp299 != 0; _temp299=(( struct Cyc_List_List*) _check_null( _temp299))->tl){
Cyc_Tcutil_occurs( evar,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp299))->hd)).f3);} if( _temp295 != 0){ int _temp347; void*
_temp349; void* _temp351; struct Cyc_Absyn_Tqual _temp353; struct Cyc_Core_Opt*
_temp355; struct Cyc_Absyn_VarargInfo _temp345=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp295)); _LL356: _temp355= _temp345.name; goto _LL354; _LL354:
_temp353= _temp345.tq; goto _LL352; _LL352: _temp351=( void*) _temp345.type;
goto _LL350; _LL350: _temp349=( void*) _temp345.rgn; goto _LL348; _LL348:
_temp347= _temp345.inject; goto _LL346; _LL346: Cyc_Tcutil_occurs( evar,
_temp351); Cyc_Tcutil_occurs( evar, _temp349);} goto _LL246; _LL256: for( 0;
_temp307 != 0; _temp307=(( struct Cyc_List_List*) _check_null( _temp307))->tl){
Cyc_Tcutil_occurs( evar,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp307))->hd)).f2);} goto _LL246; _LL258: Cyc_Tcutil_occurs( evar,
_temp309); goto _LL246; _LL260: Cyc_Tcutil_occurs( evar, _temp313); Cyc_Tcutil_occurslist(
evar, _temp315); goto _LL246; _LL262: _temp327= _temp321; goto _LL264; _LL264:
_temp333= _temp327; goto _LL266; _LL266: Cyc_Tcutil_occurslist( evar, _temp333);
goto _LL246; _LL268: _temp339= _temp337; goto _LL270; _LL270: for( 0; _temp339
!= 0; _temp339=(( struct Cyc_List_List*) _check_null( _temp339))->tl){ Cyc_Tcutil_occurs(
evar,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp339))->hd)->type);} goto _LL246; _LL272: Cyc_Tcutil_occurs(
evar, _temp341); goto _LL246; _LL274: Cyc_Tcutil_occurslist( evar, _temp343);
goto _LL246; _LL276: goto _LL246; _LL246:;} static void Cyc_Tcutil_occurslist(
void* evar, struct Cyc_List_List* ts){ for( 0; ts != 0; ts=(( struct Cyc_List_List*)
_check_null( ts))->tl){ Cyc_Tcutil_occurs( evar,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd);}} static void Cyc_Tcutil_unify_list( struct Cyc_List_List*
t1, struct Cyc_List_List* t2){ struct _handler_cons _temp357; _push_handler(&
_temp357);{ int _temp359= 0; if( setjmp( _temp357.handler)){ _temp359= 1;} if( !
_temp359){(( void(*)( void(* f)( void*, void*), struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_iter2)( Cyc_Tcutil_unify_it, t1, t2);; _pop_handler();}
else{ void* _temp358=( void*) _exn_thrown; void* _temp361= _temp358; _LL363: if(
_temp361 == Cyc_List_List_mismatch){ goto _LL364;} else{ goto _LL365;} _LL365:
goto _LL366; _LL364:( void) _throw(( void*) Cyc_Tcutil_Unify); _LL366:( void)
_throw( _temp361); _LL362:;}}} static void Cyc_Tcutil_unify_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != tq2.q_const? 1: tq1.q_volatile
!= tq2.q_volatile)? 1: tq1.q_restrict != tq2.q_restrict){( void) _throw(( void*)
Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == tq2.q_const? tq1.q_volatile
== tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
y); if( x == y){ return;}{ void* _temp367=( void*) x->v; struct Cyc_Absyn_Conref*
_temp375; void* _temp377; _LL369: if( _temp367 ==( void*) Cyc_Absyn_No_constr){
goto _LL370;} else{ goto _LL371;} _LL371: if(( unsigned int) _temp367 > 1u?*((
int*) _temp367) == Cyc_Absyn_Forward_constr: 0){ _LL376: _temp375=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp367)->f1; goto _LL372;} else{ goto _LL373;} _LL373: if(( unsigned int)
_temp367 > 1u?*(( int*) _temp367) == Cyc_Absyn_Eq_constr: 0){ _LL378: _temp377=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp367)->f1; goto _LL374;} else{
goto _LL368;} _LL370:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp379=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp379[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp380; _temp380.tag= Cyc_Absyn_Forward_constr; _temp380.f1= y; _temp380;});
_temp379;}))); return; _LL372:( void) _throw((( void*(*)( struct _tagged_string
msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp381="unify_conref: forward after compress";
struct _tagged_string _temp382; _temp382.curr= _temp381; _temp382.base= _temp381;
_temp382.last_plus_one= _temp381 + 37; _temp382;}))); _LL374: { void* _temp383=(
void*) y->v; struct Cyc_Absyn_Conref* _temp391; void* _temp393; _LL385: if(
_temp383 ==( void*) Cyc_Absyn_No_constr){ goto _LL386;} else{ goto _LL387;}
_LL387: if(( unsigned int) _temp383 > 1u?*(( int*) _temp383) == Cyc_Absyn_Forward_constr:
0){ _LL392: _temp391=(( struct Cyc_Absyn_Forward_constr_struct*) _temp383)->f1;
goto _LL388;} else{ goto _LL389;} _LL389: if(( unsigned int) _temp383 > 1u?*((
int*) _temp383) == Cyc_Absyn_Eq_constr: 0){ _LL394: _temp393=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp383)->f1; goto _LL390;} else{ goto _LL384;}
_LL386:( void*)( y->v=( void*)(( void*) x->v)); return; _LL388:( void) _throw(((
void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char*
_temp395="unify_conref: forward after compress(2)"; struct _tagged_string
_temp396; _temp396.curr= _temp395; _temp396.base= _temp395; _temp396.last_plus_one=
_temp395 + 40; _temp396;}))); _LL390: if( cmp( _temp377, _temp393) != 0){( void)
_throw(( void*) Cyc_Tcutil_Unify);} return; _LL384:;} _LL368:;}} static int Cyc_Tcutil_unify_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ struct _handler_cons _temp397; _push_handler(& _temp397);{ int _temp399= 0;
if( setjmp( _temp397.handler)){ _temp399= 1;} if( ! _temp399){(( void(*)( int(*
cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int _temp400= 1; _npop_handler( 0u);
return _temp400;}; _pop_handler();} else{ void* _temp398=( void*) _exn_thrown;
void* _temp402= _temp398; _LL404: if( _temp402 == Cyc_Tcutil_Unify){ goto _LL405;}
else{ goto _LL406;} _LL406: goto _LL407; _LL405: return 0; _LL407:( void) _throw(
_temp402); _LL403:;}}} static int Cyc_Tcutil_boundscmp( void* b1, void* b2){
struct _tuple7 _temp409=({ struct _tuple7 _temp408; _temp408.f1= b1; _temp408.f2=
b2; _temp408;}); void* _temp419; void* _temp421; void* _temp423; void* _temp425;
void* _temp427; void* _temp429; void* _temp431; struct Cyc_Absyn_Exp* _temp433;
void* _temp435; struct Cyc_Absyn_Exp* _temp437; _LL411: _LL422: _temp421=
_temp409.f1; if( _temp421 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL420;} else{
goto _LL413;} _LL420: _temp419= _temp409.f2; if( _temp419 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL412;} else{ goto _LL413;} _LL413: _LL426: _temp425= _temp409.f1; if(
_temp425 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL424;} else{ goto _LL415;}
_LL424: _temp423= _temp409.f2; goto _LL414; _LL415: _LL430: _temp429= _temp409.f1;
goto _LL428; _LL428: _temp427= _temp409.f2; if( _temp427 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL416;} else{ goto _LL417;} _LL417: _LL436: _temp435= _temp409.f1; if((
unsigned int) _temp435 > 1u?*(( int*) _temp435) == Cyc_Absyn_Upper_b: 0){ _LL438:
_temp437=(( struct Cyc_Absyn_Upper_b_struct*) _temp435)->f1; goto _LL432;} else{
goto _LL410;} _LL432: _temp431= _temp409.f2; if(( unsigned int) _temp431 > 1u?*((
int*) _temp431) == Cyc_Absyn_Upper_b: 0){ _LL434: _temp433=(( struct Cyc_Absyn_Upper_b_struct*)
_temp431)->f1; goto _LL418;} else{ goto _LL410;} _LL412: return 0; _LL414:
return - 1; _LL416: return 1; _LL418: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp437); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp433); if( i1 == i2){
return 0;} if( i1 < i2){ return - 1;} return 1;} _LL410:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 == a2){ return 1;}{ struct _tuple7 _temp440=({
struct _tuple7 _temp439; _temp439.f1= a1; _temp439.f2= a2; _temp439;}); void*
_temp450; int _temp452; void* _temp454; int _temp456; void* _temp458; int
_temp460; void* _temp462; int _temp464; void* _temp466; struct _tagged_string
_temp468; void* _temp470; struct _tagged_string _temp472; _LL442: _LL455:
_temp454= _temp440.f1; if(( unsigned int) _temp454 > 16u?*(( int*) _temp454) ==
Cyc_Absyn_Regparm_att: 0){ _LL457: _temp456=(( struct Cyc_Absyn_Regparm_att_struct*)
_temp454)->f1; goto _LL451;} else{ goto _LL444;} _LL451: _temp450= _temp440.f2;
if(( unsigned int) _temp450 > 16u?*(( int*) _temp450) == Cyc_Absyn_Regparm_att:
0){ _LL453: _temp452=(( struct Cyc_Absyn_Regparm_att_struct*) _temp450)->f1;
goto _LL443;} else{ goto _LL444;} _LL444: _LL463: _temp462= _temp440.f1; if((
unsigned int) _temp462 > 16u?*(( int*) _temp462) == Cyc_Absyn_Aligned_att: 0){
_LL465: _temp464=(( struct Cyc_Absyn_Aligned_att_struct*) _temp462)->f1; goto
_LL459;} else{ goto _LL446;} _LL459: _temp458= _temp440.f2; if(( unsigned int)
_temp458 > 16u?*(( int*) _temp458) == Cyc_Absyn_Aligned_att: 0){ _LL461:
_temp460=(( struct Cyc_Absyn_Aligned_att_struct*) _temp458)->f1; goto _LL445;}
else{ goto _LL446;} _LL446: _LL471: _temp470= _temp440.f1; if(( unsigned int)
_temp470 > 16u?*(( int*) _temp470) == Cyc_Absyn_Section_att: 0){ _LL473:
_temp472=(( struct Cyc_Absyn_Section_att_struct*) _temp470)->f1; goto _LL467;}
else{ goto _LL448;} _LL467: _temp466= _temp440.f2; if(( unsigned int) _temp466 >
16u?*(( int*) _temp466) == Cyc_Absyn_Section_att: 0){ _LL469: _temp468=(( struct
Cyc_Absyn_Section_att_struct*) _temp466)->f1; goto _LL447;} else{ goto _LL448;}
_LL448: goto _LL449; _LL443: _temp464= _temp456; _temp460= _temp452; goto _LL445;
_LL445: return _temp464 == _temp460; _LL447: return Cyc_String_strcmp( _temp472,
_temp468) == 0; _LL449: return 0; _LL441:;}} int Cyc_Tcutil_same_atts( struct
Cyc_List_List* a1, struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for(
0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( !(( int(*)(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){
return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( !(( int(*)( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a1)){ return 0;}}} return 1;}
static void Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp474= e; struct Cyc_List_List* _temp480; struct Cyc_List_List**
_temp482; _LL476: if(( unsigned int) _temp474 > 4u?*(( int*) _temp474) == Cyc_Absyn_JoinEff:
0){ _LL481: _temp480=(( struct Cyc_Absyn_JoinEff_struct*) _temp474)->f1;
_temp482=&(( struct Cyc_Absyn_JoinEff_struct*) _temp474)->f1; goto _LL477;}
else{ goto _LL478;} _LL478: goto _LL479; _LL477: { int nested_join= 0;{ struct
Cyc_List_List* effs=* _temp482; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp483=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; goto _LL484; _LL484: Cyc_Tcutil_normalize_effect(
_temp483);{ void* _temp485= Cyc_Tcutil_compress( _temp483); struct Cyc_List_List*
_temp491; _LL487: if(( unsigned int) _temp485 > 4u?*(( int*) _temp485) == Cyc_Absyn_JoinEff:
0){ _LL492: _temp491=(( struct Cyc_Absyn_JoinEff_struct*) _temp485)->f1; goto
_LL488;} else{ goto _LL489;} _LL489: goto _LL490; _LL488: nested_join= 1; goto
_LL486; _LL490: goto _LL486; _LL486:;}}} if( ! nested_join){ return;}{ struct
Cyc_List_List* effects= 0;{ struct Cyc_List_List* effs=* _temp482; for( 0; effs
!= 0; effs=(( struct Cyc_List_List*) _check_null( effs))->tl){ void* _temp493=
Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null( effs))->hd);
struct Cyc_List_List* _temp499; _LL495: if(( unsigned int) _temp493 > 4u?*(( int*)
_temp493) == Cyc_Absyn_JoinEff: 0){ _LL500: _temp499=(( struct Cyc_Absyn_JoinEff_struct*)
_temp493)->f1; goto _LL496;} else{ goto _LL497;} _LL497: goto _LL498; _LL496:
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_revappend)( _temp499, effects); goto _LL494; _LL498: effects=({
struct Cyc_List_List* _temp501=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp501->hd=( void*) _temp493; _temp501->tl= effects;
_temp501;}); goto _LL494; _LL494:;}}* _temp482=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effects); goto _LL475;}} _LL479:
goto _LL475; _LL475:;}} struct _tuple8{ void* f1; struct Cyc_List_List* f2; } ;
static struct _tuple8* Cyc_Tcutil_get_effect_evar( void* t){ void* _temp502= Cyc_Tcutil_compress(
t); struct Cyc_List_List* _temp510; struct Cyc_List_List _temp512; struct Cyc_List_List*
_temp513; void* _temp515; int _temp517; struct Cyc_Core_Opt* _temp519; struct
Cyc_Core_Opt** _temp521; void* _temp522; void** _temp524; _LL504: if((
unsigned int) _temp502 > 4u?*(( int*) _temp502) == Cyc_Absyn_JoinEff: 0){ _LL511:
_temp510=(( struct Cyc_Absyn_JoinEff_struct*) _temp502)->f1; if( _temp510 == 0){
goto _LL506;} else{ _temp512=* _temp510; _LL516: _temp515=( void*) _temp512.hd;
goto _LL514; _LL514: _temp513= _temp512.tl; goto _LL505;}} else{ goto _LL506;}
_LL506: if(( unsigned int) _temp502 > 4u?*(( int*) _temp502) == Cyc_Absyn_Evar:
0){ _LL523: _temp522=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp502)->f1;
_temp524=&(( void*)(( struct Cyc_Absyn_Evar_struct*) _temp502)->f1); goto _LL520;
_LL520: _temp519=(( struct Cyc_Absyn_Evar_struct*) _temp502)->f2; _temp521=&((
struct Cyc_Absyn_Evar_struct*) _temp502)->f2; goto _LL518; _LL518: _temp517=((
struct Cyc_Absyn_Evar_struct*) _temp502)->f3; goto _LL507;} else{ goto _LL508;}
_LL508: goto _LL509; _LL505: { void* _temp525= Cyc_Tcutil_compress( _temp515);
int _temp531; struct Cyc_Core_Opt* _temp533; void* _temp535; _LL527: if((
unsigned int) _temp525 > 4u?*(( int*) _temp525) == Cyc_Absyn_Evar: 0){ _LL536:
_temp535=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp525)->f1; goto _LL534;
_LL534: _temp533=(( struct Cyc_Absyn_Evar_struct*) _temp525)->f2; goto _LL532;
_LL532: _temp531=(( struct Cyc_Absyn_Evar_struct*) _temp525)->f3; goto _LL528;}
else{ goto _LL529;} _LL529: goto _LL530; _LL528: return({ struct _tuple8*
_temp537=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp537->f1=
_temp515; _temp537->f2= _temp513; _temp537;}); _LL530: return 0; _LL526:;}
_LL507: if(* _temp524 !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp538="effect evar has wrong kind";
struct _tagged_string _temp539; _temp539.curr= _temp538; _temp539.base= _temp538;
_temp539.last_plus_one= _temp538 + 27; _temp539;}));} return({ struct _tuple8*
_temp540=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp540->f1= t;
_temp540->f2= 0; _temp540;}); _LL509: return 0; _LL503:;} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp541= Cyc_Tcutil_compress( e); void*
_temp551; struct Cyc_List_List* _temp553; int _temp555; struct Cyc_Core_Opt*
_temp557; struct Cyc_Core_Opt** _temp559; void* _temp560; void** _temp562;
_LL543: if(( unsigned int) _temp541 > 4u?*(( int*) _temp541) == Cyc_Absyn_AccessEff:
0){ _LL552: _temp551=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp541)->f1;
goto _LL544;} else{ goto _LL545;} _LL545: if(( unsigned int) _temp541 > 4u?*((
int*) _temp541) == Cyc_Absyn_JoinEff: 0){ _LL554: _temp553=(( struct Cyc_Absyn_JoinEff_struct*)
_temp541)->f1; goto _LL546;} else{ goto _LL547;} _LL547: if(( unsigned int)
_temp541 > 4u?*(( int*) _temp541) == Cyc_Absyn_Evar: 0){ _LL561: _temp560=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp541)->f1; _temp562=&(( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp541)->f1); goto _LL558; _LL558: _temp557=(( struct Cyc_Absyn_Evar_struct*)
_temp541)->f2; _temp559=&(( struct Cyc_Absyn_Evar_struct*) _temp541)->f2; goto
_LL556; _LL556: _temp555=(( struct Cyc_Absyn_Evar_struct*) _temp541)->f3; goto
_LL548;} else{ goto _LL549;} _LL549: goto _LL550; _LL544: if( constrain){ return
Cyc_Tcutil_unify( r, _temp551);} Cyc_Tcutil_compress( _temp551); if( r ==
_temp551){ return 1;}{ struct _tuple7 _temp564=({ struct _tuple7 _temp563;
_temp563.f1= r; _temp563.f2= _temp551; _temp563;}); void* _temp570; struct Cyc_Absyn_Tvar*
_temp572; void* _temp574; struct Cyc_Absyn_Tvar* _temp576; _LL566: _LL575:
_temp574= _temp564.f1; if(( unsigned int) _temp574 > 4u?*(( int*) _temp574) ==
Cyc_Absyn_VarType: 0){ _LL577: _temp576=(( struct Cyc_Absyn_VarType_struct*)
_temp574)->f1; goto _LL571;} else{ goto _LL568;} _LL571: _temp570= _temp564.f2;
if(( unsigned int) _temp570 > 4u?*(( int*) _temp570) == Cyc_Absyn_VarType: 0){
_LL573: _temp572=(( struct Cyc_Absyn_VarType_struct*) _temp570)->f1; goto _LL567;}
else{ goto _LL568;} _LL568: goto _LL569; _LL567: return Cyc_Absyn_tvar_cmp(
_temp576, _temp572) == 0; _LL569: return 0; _LL565:;} _LL546: for( 0; _temp553
!= 0; _temp553=(( struct Cyc_List_List*) _check_null( _temp553))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp553))->hd)){
return 1;}} return 0; _LL548: if(* _temp562 !=( void*) Cyc_Absyn_EffKind){((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char*
_temp578="effect evar has wrong kind"; struct _tagged_string _temp579; _temp579.curr=
_temp578; _temp579.base= _temp578; _temp579.last_plus_one= _temp578 + 27;
_temp579;}));} if( ! constrain){ return 0;}{ void* _temp580= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_EffKind); goto _LL581; _LL581: { struct Cyc_Absyn_JoinEff_struct*
_temp588=({ struct Cyc_Absyn_JoinEff_struct* _temp582=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp582[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp583; _temp583.tag= Cyc_Absyn_JoinEff; _temp583.f1=({ struct Cyc_List_List*
_temp584=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp584->hd=( void*) _temp580; _temp584->tl=({ struct Cyc_List_List* _temp585=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp585->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp586=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp586[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp587; _temp587.tag= Cyc_Absyn_AccessEff; _temp587.f1=(
void*) r; _temp587;}); _temp586;})); _temp585->tl= 0; _temp585;}); _temp584;});
_temp583;}); _temp582;}); goto _LL589; _LL589:* _temp559=({ struct Cyc_Core_Opt*
_temp590=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp590->v=( void*)(( void*) _temp588); _temp590;}); return 1;}} _LL550: return
0; _LL542:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp591= e;
struct Cyc_Absyn_Tvar* _temp601; struct Cyc_List_List* _temp603; int _temp605;
struct Cyc_Core_Opt* _temp607; struct Cyc_Core_Opt** _temp609; void* _temp610;
void** _temp612; _LL593: if(( unsigned int) _temp591 > 4u?*(( int*) _temp591) ==
Cyc_Absyn_VarType: 0){ _LL602: _temp601=(( struct Cyc_Absyn_VarType_struct*)
_temp591)->f1; goto _LL594;} else{ goto _LL595;} _LL595: if(( unsigned int)
_temp591 > 4u?*(( int*) _temp591) == Cyc_Absyn_JoinEff: 0){ _LL604: _temp603=((
struct Cyc_Absyn_JoinEff_struct*) _temp591)->f1; goto _LL596;} else{ goto _LL597;}
_LL597: if(( unsigned int) _temp591 > 4u?*(( int*) _temp591) == Cyc_Absyn_Evar:
0){ _LL611: _temp610=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp591)->f1;
_temp612=&(( void*)(( struct Cyc_Absyn_Evar_struct*) _temp591)->f1); goto _LL608;
_LL608: _temp607=(( struct Cyc_Absyn_Evar_struct*) _temp591)->f2; _temp609=&((
struct Cyc_Absyn_Evar_struct*) _temp591)->f2; goto _LL606; _LL606: _temp605=((
struct Cyc_Absyn_Evar_struct*) _temp591)->f3; goto _LL598;} else{ goto _LL599;}
_LL599: goto _LL600; _LL594: return Cyc_Absyn_tvar_cmp( v, _temp601) == 0;
_LL596: for( 0; _temp603 != 0; _temp603=(( struct Cyc_List_List*) _check_null(
_temp603))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,( void*)((
struct Cyc_List_List*) _check_null( _temp603))->hd)){ return 1;}} return 0;
_LL598: if(* _temp612 !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp613="effect evar has wrong kind";
struct _tagged_string _temp614; _temp614.curr= _temp613; _temp614.base= _temp613;
_temp614.last_plus_one= _temp613 + 27; _temp614;}));}{ void* _temp615= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_EffKind); goto _LL616; _LL616: { struct Cyc_Absyn_JoinEff_struct*
_temp623=({ struct Cyc_Absyn_JoinEff_struct* _temp617=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp617[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp618; _temp618.tag= Cyc_Absyn_JoinEff; _temp618.f1=({ struct Cyc_List_List*
_temp619=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp619->hd=( void*) _temp615; _temp619->tl=({ struct Cyc_List_List* _temp620=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp620->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp621=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp621[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp622; _temp622.tag= Cyc_Absyn_VarType; _temp622.f1= v; _temp622;}); _temp621;}));
_temp620->tl= 0; _temp620;}); _temp619;}); _temp618;}); _temp617;}); goto _LL624;
_LL624:* _temp609=({ struct Cyc_Core_Opt* _temp625=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp625->v=( void*)(( void*) _temp623);
_temp625;}); return 1;}} _LL600: return 0; _LL592:;}} static int Cyc_Tcutil_evar_in_effect(
void* evar, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp626= e; struct Cyc_List_List*
_temp634; int _temp636; struct Cyc_Core_Opt* _temp638; void* _temp640; _LL628:
if(( unsigned int) _temp626 > 4u?*(( int*) _temp626) == Cyc_Absyn_JoinEff: 0){
_LL635: _temp634=(( struct Cyc_Absyn_JoinEff_struct*) _temp626)->f1; goto _LL629;}
else{ goto _LL630;} _LL630: if(( unsigned int) _temp626 > 4u?*(( int*) _temp626)
== Cyc_Absyn_Evar: 0){ _LL641: _temp640=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp626)->f1; goto _LL639; _LL639: _temp638=(( struct Cyc_Absyn_Evar_struct*)
_temp626)->f2; goto _LL637; _LL637: _temp636=(( struct Cyc_Absyn_Evar_struct*)
_temp626)->f3; goto _LL631;} else{ goto _LL632;} _LL632: goto _LL633; _LL629:
for( 0; _temp634 != 0; _temp634=(( struct Cyc_List_List*) _check_null( _temp634))->tl){
if( Cyc_Tcutil_evar_in_effect( evar,( void*)(( struct Cyc_List_List*)
_check_null( _temp634))->hd)){ return 1;}} return 0; _LL631: return evar == e;
_LL633: return 0; _LL627:;}} int Cyc_Tcutil_subset_effect( int set_to_empty,
void* e1, void* e2){{ struct _tuple8* _temp642= Cyc_Tcutil_get_effect_evar( e2);
struct _tuple8 _temp648; struct Cyc_List_List* _temp649; void* _temp651; _LL644:
if( _temp642 == 0){ goto _LL646;} else{ _temp648=* _temp642; _LL652: _temp651=
_temp648.f1; goto _LL650; _LL650: _temp649= _temp648.f2; goto _LL645;} _LL646:
goto _LL647; _LL645: { void* _temp653= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
goto _LL654; _LL654: { struct Cyc_Absyn_JoinEff_struct* _temp659=({ struct Cyc_Absyn_JoinEff_struct*
_temp655=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp655[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp656; _temp656.tag= Cyc_Absyn_JoinEff;
_temp656.f1=({ struct Cyc_List_List* _temp657=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp657->hd=( void*) _temp653; _temp657->tl=({
struct Cyc_List_List* _temp658=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp658->hd=( void*) e1; _temp658->tl= 0; _temp658;});
_temp657;}); _temp656;}); _temp655;}); goto _LL660; _LL660: return Cyc_Tcutil_unify(
_temp651,( void*) _temp659);}} _LL647: goto _LL643; _LL643:;}{ void* _temp661=
Cyc_Tcutil_compress( e1); struct Cyc_List_List* _temp673; void* _temp675; struct
Cyc_Absyn_Tvar* _temp677; int _temp679; struct Cyc_Core_Opt* _temp681; struct
Cyc_Core_Opt** _temp683; void* _temp684; _LL663: if(( unsigned int) _temp661 > 4u?*((
int*) _temp661) == Cyc_Absyn_JoinEff: 0){ _LL674: _temp673=(( struct Cyc_Absyn_JoinEff_struct*)
_temp661)->f1; goto _LL664;} else{ goto _LL665;} _LL665: if(( unsigned int)
_temp661 > 4u?*(( int*) _temp661) == Cyc_Absyn_AccessEff: 0){ _LL676: _temp675=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp661)->f1; goto _LL666;} else{
goto _LL667;} _LL667: if(( unsigned int) _temp661 > 4u?*(( int*) _temp661) ==
Cyc_Absyn_VarType: 0){ _LL678: _temp677=(( struct Cyc_Absyn_VarType_struct*)
_temp661)->f1; goto _LL668;} else{ goto _LL669;} _LL669: if(( unsigned int)
_temp661 > 4u?*(( int*) _temp661) == Cyc_Absyn_Evar: 0){ _LL685: _temp684=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp661)->f1; goto _LL682; _LL682: _temp681=((
struct Cyc_Absyn_Evar_struct*) _temp661)->f2; _temp683=&(( struct Cyc_Absyn_Evar_struct*)
_temp661)->f2; goto _LL680; _LL680: _temp679=(( struct Cyc_Absyn_Evar_struct*)
_temp661)->f3; goto _LL670;} else{ goto _LL671;} _LL671: goto _LL672; _LL664:
for( 0; _temp673 != 0; _temp673=(( struct Cyc_List_List*) _check_null( _temp673))->tl){
if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*)(( struct Cyc_List_List*)
_check_null( _temp673))->hd, e2)){ return 0;}} return 1; _LL666: return( Cyc_Tcutil_region_in_effect(
0, _temp675, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp675, e2))? 1: Cyc_Tcutil_unify(
_temp675,( void*) Cyc_Absyn_HeapRgn); _LL668: return Cyc_Tcutil_variable_in_effect(
0, _temp677, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp677, e2); _LL670:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp683=({
struct Cyc_Core_Opt* _temp686=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp686->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp687=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp687[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp688; _temp688.tag= Cyc_Absyn_JoinEff;
_temp688.f1= 0; _temp688;}); _temp687;})); _temp686;});} else{* _temp683=({
struct Cyc_Core_Opt* _temp689=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp689->v=( void*) e2; _temp689;});}} return 1; _LL672: return((
int(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string
_temp690= Cyc_Absynpp_typ2string( e1); xprintf("subset_effect: bad effect: %.*s",
_temp690.last_plus_one - _temp690.curr, _temp690.curr);})); _LL662:;}} struct
_tuple9{ struct _tuple8* f1; struct _tuple8* f2; } ; static int Cyc_Tcutil_unify_effect(
void* e1, void* e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2);
Cyc_Tcutil_normalize_effect( e1); Cyc_Tcutil_normalize_effect( e2);{ struct
_tuple9 _temp692=({ struct _tuple9 _temp691; _temp691.f1= Cyc_Tcutil_get_effect_evar(
e1); _temp691.f2= Cyc_Tcutil_get_effect_evar( e2); _temp691;}); struct _tuple8*
_temp698; struct _tuple8 _temp700; struct Cyc_List_List* _temp701; void*
_temp703; struct _tuple8* _temp705; struct _tuple8 _temp707; struct Cyc_List_List*
_temp708; void* _temp710; _LL694: _LL706: _temp705= _temp692.f1; if( _temp705 ==
0){ goto _LL696;} else{ _temp707=* _temp705; _LL711: _temp710= _temp707.f1; goto
_LL709; _LL709: _temp708= _temp707.f2; goto _LL699;} _LL699: _temp698= _temp692.f2;
if( _temp698 == 0){ goto _LL696;} else{ _temp700=* _temp698; _LL704: _temp703=
_temp700.f1; goto _LL702; _LL702: _temp701= _temp700.f2; goto _LL695;} _LL696:
goto _LL697; _LL695: { void* _temp712= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
goto _LL713; _LL713: { struct Cyc_Absyn_JoinEff_struct* _temp717=({ struct Cyc_Absyn_JoinEff_struct*
_temp714=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp714[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp715; _temp715.tag= Cyc_Absyn_JoinEff;
_temp715.f1=({ struct Cyc_List_List* _temp716=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp716->hd=( void*) _temp712; _temp716->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp708, _temp701); _temp716;}); _temp715;}); _temp714;}); goto _LL718; _LL718:
return Cyc_Tcutil_unify( _temp710,( void*) _temp717)? Cyc_Tcutil_unify( _temp703,(
void*) _temp717): 0;}} _LL697: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect(
0, e2, e1): 0; _LL693:;}} struct _tuple10{ struct Cyc_Absyn_VarargInfo* f1;
struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple11{ struct Cyc_Core_Opt* f1;
struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1=
Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{
void* _temp719= t1; int _temp725; struct Cyc_Core_Opt* _temp727; struct Cyc_Core_Opt**
_temp729; void* _temp730; _LL721: if(( unsigned int) _temp719 > 4u?*(( int*)
_temp719) == Cyc_Absyn_Evar: 0){ _LL731: _temp730=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp719)->f1; goto _LL728; _LL728: _temp727=(( struct Cyc_Absyn_Evar_struct*)
_temp719)->f2; _temp729=&(( struct Cyc_Absyn_Evar_struct*) _temp719)->f2; goto
_LL726; _LL726: _temp725=(( struct Cyc_Absyn_Evar_struct*) _temp719)->f3; goto
_LL722;} else{ goto _LL723;} _LL723: goto _LL724; _LL722: Cyc_Tcutil_occurs( t1,
t2);{ void* _temp732= Cyc_Tcutil_typ_kind( t2); goto _LL733; _LL733: if( Cyc_Tcutil_kind_leq(
_temp732, _temp730)){* _temp729=({ struct Cyc_Core_Opt* _temp734=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp734->v=( void*) t2; _temp734;});
return;} else{{ void* _temp735= t2; int _temp743; struct Cyc_Core_Opt* _temp745;
struct Cyc_Core_Opt** _temp747; void* _temp748; struct Cyc_Absyn_PtrInfo
_temp751; _LL737: if(( unsigned int) _temp735 > 4u?*(( int*) _temp735) == Cyc_Absyn_Evar:
0){ _LL749: _temp748=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp735)->f1;
goto _LL746; _LL746: _temp745=(( struct Cyc_Absyn_Evar_struct*) _temp735)->f2;
_temp747=&(( struct Cyc_Absyn_Evar_struct*) _temp735)->f2; goto _LL744; _LL744:
_temp743=(( struct Cyc_Absyn_Evar_struct*) _temp735)->f3; goto _LL738;} else{
goto _LL739;} _LL739: if(( unsigned int) _temp735 > 4u?*(( int*) _temp735) ==
Cyc_Absyn_PointerType: 0){ _LL752: _temp751=(( struct Cyc_Absyn_PointerType_struct*)
_temp735)->f1; goto _LL750;} else{ goto _LL741;} _LL750: if( _temp730 ==( void*)
Cyc_Absyn_BoxKind){ goto _LL740;} else{ goto _LL741;} _LL741: goto _LL742;
_LL738: if( Cyc_Tcutil_kind_leq( _temp730, _temp732)){* _temp747=({ struct Cyc_Core_Opt*
_temp753=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp753->v=( void*) t1; _temp753;}); return;} goto _LL736; _LL740: { struct Cyc_Absyn_Conref*
_temp754=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp751.bounds); goto _LL755; _LL755:{ void* _temp756=( void*) _temp754->v;
_LL758: if( _temp756 ==( void*) Cyc_Absyn_No_constr){ goto _LL759;} else{ goto
_LL760;} _LL760: goto _LL761; _LL759:( void*)( _temp754->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp762=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp762[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp763; _temp763.tag= Cyc_Absyn_Eq_constr; _temp763.f1=(
void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp764=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp764[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp765; _temp765.tag= Cyc_Absyn_Upper_b; _temp765.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp765;}); _temp764;})); _temp763;}); _temp762;})));* _temp729=({
struct Cyc_Core_Opt* _temp766=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp766->v=( void*) t2; _temp766;}); return; _LL761: goto
_LL757; _LL757:;} goto _LL736;} _LL742: goto _LL736; _LL736:;}( void) _throw((
void*) Cyc_Tcutil_Unify);}} _LL724: goto _LL720; _LL720:;}{ struct _tuple7
_temp768=({ struct _tuple7 _temp767; _temp767.f1= t2; _temp767.f2= t1; _temp767;});
void* _temp818; void* _temp820; int _temp822; struct Cyc_Core_Opt* _temp824;
void* _temp826; void* _temp828; void* _temp830; void* _temp832; struct Cyc_Absyn_Tvar*
_temp834; void* _temp836; struct Cyc_Absyn_Tvar* _temp838; void* _temp840;
struct Cyc_Absyn_Structdecl** _temp842; struct Cyc_List_List* _temp844; struct
_tuple1* _temp846; void* _temp848; struct Cyc_Absyn_Structdecl** _temp850;
struct Cyc_List_List* _temp852; struct _tuple1* _temp854; void* _temp856; struct
Cyc_Absyn_Enumdecl* _temp858; struct _tuple1* _temp860; void* _temp862; struct
Cyc_Absyn_Enumdecl* _temp864; struct _tuple1* _temp866; void* _temp868; struct
Cyc_Absyn_Uniondecl** _temp870; struct Cyc_List_List* _temp872; struct _tuple1*
_temp874; void* _temp876; struct Cyc_Absyn_Uniondecl** _temp878; struct Cyc_List_List*
_temp880; struct _tuple1* _temp882; void* _temp884; struct Cyc_Absyn_TunionInfo
_temp886; void* _temp888; struct Cyc_List_List* _temp890; void* _temp892; struct
Cyc_Absyn_Tuniondecl* _temp894; void* _temp896; struct Cyc_Absyn_TunionInfo
_temp898; void* _temp900; struct Cyc_List_List* _temp902; void* _temp904; struct
Cyc_Absyn_Tuniondecl* _temp906; void* _temp908; struct Cyc_Absyn_TunionFieldInfo
_temp910; struct Cyc_List_List* _temp912; void* _temp914; struct Cyc_Absyn_Tunionfield*
_temp916; struct Cyc_Absyn_Tuniondecl* _temp918; void* _temp920; struct Cyc_Absyn_TunionFieldInfo
_temp922; struct Cyc_List_List* _temp924; void* _temp926; struct Cyc_Absyn_Tunionfield*
_temp928; struct Cyc_Absyn_Tuniondecl* _temp930; void* _temp932; struct Cyc_Absyn_PtrInfo
_temp934; struct Cyc_Absyn_Conref* _temp936; struct Cyc_Absyn_Tqual _temp938;
struct Cyc_Absyn_Conref* _temp940; void* _temp942; void* _temp944; void*
_temp946; struct Cyc_Absyn_PtrInfo _temp948; struct Cyc_Absyn_Conref* _temp950;
struct Cyc_Absyn_Tqual _temp952; struct Cyc_Absyn_Conref* _temp954; void*
_temp956; void* _temp958; void* _temp960; void* _temp962; void* _temp964; void*
_temp966; void* _temp968; void* _temp970; void* _temp972; void* _temp974; void*
_temp976; void* _temp978; void* _temp980; struct Cyc_Absyn_Exp* _temp982; struct
Cyc_Absyn_Tqual _temp984; void* _temp986; void* _temp988; struct Cyc_Absyn_Exp*
_temp990; struct Cyc_Absyn_Tqual _temp992; void* _temp994; void* _temp996;
struct Cyc_Absyn_FnInfo _temp998; struct Cyc_List_List* _temp1000; struct Cyc_Absyn_VarargInfo*
_temp1002; int _temp1004; struct Cyc_List_List* _temp1006; void* _temp1008;
struct Cyc_Core_Opt* _temp1010; struct Cyc_List_List* _temp1012; void* _temp1014;
struct Cyc_Absyn_FnInfo _temp1016; struct Cyc_List_List* _temp1018; struct Cyc_Absyn_VarargInfo*
_temp1020; int _temp1022; struct Cyc_List_List* _temp1024; void* _temp1026;
struct Cyc_Core_Opt* _temp1028; struct Cyc_List_List* _temp1030; void* _temp1032;
struct Cyc_List_List* _temp1034; void* _temp1036; struct Cyc_List_List*
_temp1038; void* _temp1040; struct Cyc_List_List* _temp1042; void* _temp1044;
struct Cyc_List_List* _temp1046; void* _temp1048; struct Cyc_List_List*
_temp1050; void* _temp1052; struct Cyc_List_List* _temp1054; void* _temp1056;
void* _temp1058; void* _temp1060; void* _temp1062; void* _temp1064; void*
_temp1066; void* _temp1068; void* _temp1070; struct Cyc_List_List* _temp1072;
void* _temp1074; struct Cyc_List_List* _temp1076; void* _temp1078; void*
_temp1080; void* _temp1082; void* _temp1084; void* _temp1086; void* _temp1088;
void* _temp1090; _LL770: _LL821: _temp820= _temp768.f1; if(( unsigned int)
_temp820 > 4u?*(( int*) _temp820) == Cyc_Absyn_Evar: 0){ _LL827: _temp826=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp820)->f1; goto _LL825; _LL825: _temp824=((
struct Cyc_Absyn_Evar_struct*) _temp820)->f2; goto _LL823; _LL823: _temp822=((
struct Cyc_Absyn_Evar_struct*) _temp820)->f3; goto _LL819;} else{ goto _LL772;}
_LL819: _temp818= _temp768.f2; goto _LL771; _LL772: _LL831: _temp830= _temp768.f1;
if( _temp830 ==( void*) Cyc_Absyn_VoidType){ goto _LL829;} else{ goto _LL774;}
_LL829: _temp828= _temp768.f2; if( _temp828 ==( void*) Cyc_Absyn_VoidType){ goto
_LL773;} else{ goto _LL774;} _LL774: _LL837: _temp836= _temp768.f1; if((
unsigned int) _temp836 > 4u?*(( int*) _temp836) == Cyc_Absyn_VarType: 0){ _LL839:
_temp838=(( struct Cyc_Absyn_VarType_struct*) _temp836)->f1; goto _LL833;} else{
goto _LL776;} _LL833: _temp832= _temp768.f2; if(( unsigned int) _temp832 > 4u?*((
int*) _temp832) == Cyc_Absyn_VarType: 0){ _LL835: _temp834=(( struct Cyc_Absyn_VarType_struct*)
_temp832)->f1; goto _LL775;} else{ goto _LL776;} _LL776: _LL849: _temp848=
_temp768.f1; if(( unsigned int) _temp848 > 4u?*(( int*) _temp848) == Cyc_Absyn_StructType:
0){ _LL855: _temp854=(( struct Cyc_Absyn_StructType_struct*) _temp848)->f1; goto
_LL853; _LL853: _temp852=(( struct Cyc_Absyn_StructType_struct*) _temp848)->f2;
goto _LL851; _LL851: _temp850=(( struct Cyc_Absyn_StructType_struct*) _temp848)->f3;
goto _LL841;} else{ goto _LL778;} _LL841: _temp840= _temp768.f2; if((
unsigned int) _temp840 > 4u?*(( int*) _temp840) == Cyc_Absyn_StructType: 0){
_LL847: _temp846=(( struct Cyc_Absyn_StructType_struct*) _temp840)->f1; goto
_LL845; _LL845: _temp844=(( struct Cyc_Absyn_StructType_struct*) _temp840)->f2;
goto _LL843; _LL843: _temp842=(( struct Cyc_Absyn_StructType_struct*) _temp840)->f3;
goto _LL777;} else{ goto _LL778;} _LL778: _LL863: _temp862= _temp768.f1; if((
unsigned int) _temp862 > 4u?*(( int*) _temp862) == Cyc_Absyn_EnumType: 0){
_LL867: _temp866=(( struct Cyc_Absyn_EnumType_struct*) _temp862)->f1; goto
_LL865; _LL865: _temp864=(( struct Cyc_Absyn_EnumType_struct*) _temp862)->f2;
goto _LL857;} else{ goto _LL780;} _LL857: _temp856= _temp768.f2; if((
unsigned int) _temp856 > 4u?*(( int*) _temp856) == Cyc_Absyn_EnumType: 0){
_LL861: _temp860=(( struct Cyc_Absyn_EnumType_struct*) _temp856)->f1; goto
_LL859; _LL859: _temp858=(( struct Cyc_Absyn_EnumType_struct*) _temp856)->f2;
goto _LL779;} else{ goto _LL780;} _LL780: _LL877: _temp876= _temp768.f1; if((
unsigned int) _temp876 > 4u?*(( int*) _temp876) == Cyc_Absyn_UnionType: 0){
_LL883: _temp882=(( struct Cyc_Absyn_UnionType_struct*) _temp876)->f1; goto
_LL881; _LL881: _temp880=(( struct Cyc_Absyn_UnionType_struct*) _temp876)->f2;
goto _LL879; _LL879: _temp878=(( struct Cyc_Absyn_UnionType_struct*) _temp876)->f3;
goto _LL869;} else{ goto _LL782;} _LL869: _temp868= _temp768.f2; if((
unsigned int) _temp868 > 4u?*(( int*) _temp868) == Cyc_Absyn_UnionType: 0){
_LL875: _temp874=(( struct Cyc_Absyn_UnionType_struct*) _temp868)->f1; goto
_LL873; _LL873: _temp872=(( struct Cyc_Absyn_UnionType_struct*) _temp868)->f2;
goto _LL871; _LL871: _temp870=(( struct Cyc_Absyn_UnionType_struct*) _temp868)->f3;
goto _LL781;} else{ goto _LL782;} _LL782: _LL897: _temp896= _temp768.f1; if((
unsigned int) _temp896 > 4u?*(( int*) _temp896) == Cyc_Absyn_TunionType: 0){
_LL899: _temp898=(( struct Cyc_Absyn_TunionType_struct*) _temp896)->f1; _LL905:
_temp904=( void*) _temp898.tunion_info; if(*(( int*) _temp904) == Cyc_Absyn_KnownTunion){
_LL907: _temp906=(( struct Cyc_Absyn_KnownTunion_struct*) _temp904)->f1; goto
_LL903;} else{ goto _LL784;} _LL903: _temp902= _temp898.targs; goto _LL901;
_LL901: _temp900=( void*) _temp898.rgn; goto _LL885;} else{ goto _LL784;} _LL885:
_temp884= _temp768.f2; if(( unsigned int) _temp884 > 4u?*(( int*) _temp884) ==
Cyc_Absyn_TunionType: 0){ _LL887: _temp886=(( struct Cyc_Absyn_TunionType_struct*)
_temp884)->f1; _LL893: _temp892=( void*) _temp886.tunion_info; if(*(( int*)
_temp892) == Cyc_Absyn_KnownTunion){ _LL895: _temp894=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp892)->f1; goto _LL891;} else{ goto _LL784;} _LL891: _temp890= _temp886.targs;
goto _LL889; _LL889: _temp888=( void*) _temp886.rgn; goto _LL783;} else{ goto
_LL784;} _LL784: _LL921: _temp920= _temp768.f1; if(( unsigned int) _temp920 > 4u?*((
int*) _temp920) == Cyc_Absyn_TunionFieldType: 0){ _LL923: _temp922=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp920)->f1; _LL927: _temp926=( void*) _temp922.field_info; if(*(( int*)
_temp926) == Cyc_Absyn_KnownTunionfield){ _LL931: _temp930=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp926)->f1; goto _LL929; _LL929: _temp928=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp926)->f2; goto _LL925;} else{ goto _LL786;} _LL925: _temp924= _temp922.targs;
goto _LL909;} else{ goto _LL786;} _LL909: _temp908= _temp768.f2; if((
unsigned int) _temp908 > 4u?*(( int*) _temp908) == Cyc_Absyn_TunionFieldType: 0){
_LL911: _temp910=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp908)->f1;
_LL915: _temp914=( void*) _temp910.field_info; if(*(( int*) _temp914) == Cyc_Absyn_KnownTunionfield){
_LL919: _temp918=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp914)->f1;
goto _LL917; _LL917: _temp916=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp914)->f2; goto _LL913;} else{ goto _LL786;} _LL913: _temp912= _temp910.targs;
goto _LL785;} else{ goto _LL786;} _LL786: _LL947: _temp946= _temp768.f1; if((
unsigned int) _temp946 > 4u?*(( int*) _temp946) == Cyc_Absyn_PointerType: 0){
_LL949: _temp948=(( struct Cyc_Absyn_PointerType_struct*) _temp946)->f1; _LL959:
_temp958=( void*) _temp948.elt_typ; goto _LL957; _LL957: _temp956=( void*)
_temp948.rgn_typ; goto _LL955; _LL955: _temp954= _temp948.nullable; goto _LL953;
_LL953: _temp952= _temp948.tq; goto _LL951; _LL951: _temp950= _temp948.bounds;
goto _LL933;} else{ goto _LL788;} _LL933: _temp932= _temp768.f2; if((
unsigned int) _temp932 > 4u?*(( int*) _temp932) == Cyc_Absyn_PointerType: 0){
_LL935: _temp934=(( struct Cyc_Absyn_PointerType_struct*) _temp932)->f1; _LL945:
_temp944=( void*) _temp934.elt_typ; goto _LL943; _LL943: _temp942=( void*)
_temp934.rgn_typ; goto _LL941; _LL941: _temp940= _temp934.nullable; goto _LL939;
_LL939: _temp938= _temp934.tq; goto _LL937; _LL937: _temp936= _temp934.bounds;
goto _LL787;} else{ goto _LL788;} _LL788: _LL967: _temp966= _temp768.f1; if((
unsigned int) _temp966 > 4u?*(( int*) _temp966) == Cyc_Absyn_IntType: 0){ _LL971:
_temp970=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp966)->f1; goto _LL969;
_LL969: _temp968=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp966)->f2;
goto _LL961;} else{ goto _LL790;} _LL961: _temp960= _temp768.f2; if((
unsigned int) _temp960 > 4u?*(( int*) _temp960) == Cyc_Absyn_IntType: 0){ _LL965:
_temp964=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp960)->f1; goto _LL963;
_LL963: _temp962=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp960)->f2;
goto _LL789;} else{ goto _LL790;} _LL790: _LL975: _temp974= _temp768.f1; if(
_temp974 ==( void*) Cyc_Absyn_FloatType){ goto _LL973;} else{ goto _LL792;}
_LL973: _temp972= _temp768.f2; if( _temp972 ==( void*) Cyc_Absyn_FloatType){
goto _LL791;} else{ goto _LL792;} _LL792: _LL979: _temp978= _temp768.f1; if(
_temp978 ==( void*) Cyc_Absyn_DoubleType){ goto _LL977;} else{ goto _LL794;}
_LL977: _temp976= _temp768.f2; if( _temp976 ==( void*) Cyc_Absyn_DoubleType){
goto _LL793;} else{ goto _LL794;} _LL794: _LL989: _temp988= _temp768.f1; if((
unsigned int) _temp988 > 4u?*(( int*) _temp988) == Cyc_Absyn_ArrayType: 0){
_LL995: _temp994=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp988)->f1;
goto _LL993; _LL993: _temp992=(( struct Cyc_Absyn_ArrayType_struct*) _temp988)->f2;
goto _LL991; _LL991: _temp990=(( struct Cyc_Absyn_ArrayType_struct*) _temp988)->f3;
goto _LL981;} else{ goto _LL796;} _LL981: _temp980= _temp768.f2; if((
unsigned int) _temp980 > 4u?*(( int*) _temp980) == Cyc_Absyn_ArrayType: 0){
_LL987: _temp986=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp980)->f1;
goto _LL985; _LL985: _temp984=(( struct Cyc_Absyn_ArrayType_struct*) _temp980)->f2;
goto _LL983; _LL983: _temp982=(( struct Cyc_Absyn_ArrayType_struct*) _temp980)->f3;
goto _LL795;} else{ goto _LL796;} _LL796: _LL1015: _temp1014= _temp768.f1; if((
unsigned int) _temp1014 > 4u?*(( int*) _temp1014) == Cyc_Absyn_FnType: 0){
_LL1017: _temp1016=(( struct Cyc_Absyn_FnType_struct*) _temp1014)->f1; _LL1031:
_temp1030= _temp1016.tvars; goto _LL1029; _LL1029: _temp1028= _temp1016.effect;
goto _LL1027; _LL1027: _temp1026=( void*) _temp1016.ret_typ; goto _LL1025;
_LL1025: _temp1024= _temp1016.args; goto _LL1023; _LL1023: _temp1022= _temp1016.c_varargs;
goto _LL1021; _LL1021: _temp1020= _temp1016.cyc_varargs; goto _LL1019; _LL1019:
_temp1018= _temp1016.attributes; goto _LL997;} else{ goto _LL798;} _LL997:
_temp996= _temp768.f2; if(( unsigned int) _temp996 > 4u?*(( int*) _temp996) ==
Cyc_Absyn_FnType: 0){ _LL999: _temp998=(( struct Cyc_Absyn_FnType_struct*)
_temp996)->f1; _LL1013: _temp1012= _temp998.tvars; goto _LL1011; _LL1011:
_temp1010= _temp998.effect; goto _LL1009; _LL1009: _temp1008=( void*) _temp998.ret_typ;
goto _LL1007; _LL1007: _temp1006= _temp998.args; goto _LL1005; _LL1005:
_temp1004= _temp998.c_varargs; goto _LL1003; _LL1003: _temp1002= _temp998.cyc_varargs;
goto _LL1001; _LL1001: _temp1000= _temp998.attributes; goto _LL797;} else{ goto
_LL798;} _LL798: _LL1037: _temp1036= _temp768.f1; if(( unsigned int) _temp1036 >
4u?*(( int*) _temp1036) == Cyc_Absyn_TupleType: 0){ _LL1039: _temp1038=(( struct
Cyc_Absyn_TupleType_struct*) _temp1036)->f1; goto _LL1033;} else{ goto _LL800;}
_LL1033: _temp1032= _temp768.f2; if(( unsigned int) _temp1032 > 4u?*(( int*)
_temp1032) == Cyc_Absyn_TupleType: 0){ _LL1035: _temp1034=(( struct Cyc_Absyn_TupleType_struct*)
_temp1032)->f1; goto _LL799;} else{ goto _LL800;} _LL800: _LL1045: _temp1044=
_temp768.f1; if(( unsigned int) _temp1044 > 4u?*(( int*) _temp1044) == Cyc_Absyn_AnonStructType:
0){ _LL1047: _temp1046=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1044)->f1;
goto _LL1041;} else{ goto _LL802;} _LL1041: _temp1040= _temp768.f2; if((
unsigned int) _temp1040 > 4u?*(( int*) _temp1040) == Cyc_Absyn_AnonStructType: 0){
_LL1043: _temp1042=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1040)->f1;
goto _LL801;} else{ goto _LL802;} _LL802: _LL1053: _temp1052= _temp768.f1; if((
unsigned int) _temp1052 > 4u?*(( int*) _temp1052) == Cyc_Absyn_AnonUnionType: 0){
_LL1055: _temp1054=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1052)->f1;
goto _LL1049;} else{ goto _LL804;} _LL1049: _temp1048= _temp768.f2; if((
unsigned int) _temp1048 > 4u?*(( int*) _temp1048) == Cyc_Absyn_AnonUnionType: 0){
_LL1051: _temp1050=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1048)->f1;
goto _LL803;} else{ goto _LL804;} _LL804: _LL1059: _temp1058= _temp768.f1; if(
_temp1058 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1057;} else{ goto _LL806;}
_LL1057: _temp1056= _temp768.f2; if( _temp1056 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL805;} else{ goto _LL806;} _LL806: _LL1065: _temp1064= _temp768.f1; if((
unsigned int) _temp1064 > 4u?*(( int*) _temp1064) == Cyc_Absyn_RgnHandleType: 0){
_LL1067: _temp1066=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1064)->f1;
goto _LL1061;} else{ goto _LL808;} _LL1061: _temp1060= _temp768.f2; if((
unsigned int) _temp1060 > 4u?*(( int*) _temp1060) == Cyc_Absyn_RgnHandleType: 0){
_LL1063: _temp1062=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1060)->f1;
goto _LL807;} else{ goto _LL808;} _LL808: _LL1071: _temp1070= _temp768.f1; if((
unsigned int) _temp1070 > 4u?*(( int*) _temp1070) == Cyc_Absyn_JoinEff: 0){
_LL1073: _temp1072=(( struct Cyc_Absyn_JoinEff_struct*) _temp1070)->f1; goto
_LL1069;} else{ goto _LL810;} _LL1069: _temp1068= _temp768.f2; goto _LL809;
_LL810: _LL1079: _temp1078= _temp768.f1; goto _LL1075; _LL1075: _temp1074=
_temp768.f2; if(( unsigned int) _temp1074 > 4u?*(( int*) _temp1074) == Cyc_Absyn_JoinEff:
0){ _LL1077: _temp1076=(( struct Cyc_Absyn_JoinEff_struct*) _temp1074)->f1; goto
_LL811;} else{ goto _LL812;} _LL812: _LL1083: _temp1082= _temp768.f1; if((
unsigned int) _temp1082 > 4u?*(( int*) _temp1082) == Cyc_Absyn_AccessEff: 0){
_LL1085: _temp1084=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1082)->f1;
goto _LL1081;} else{ goto _LL814;} _LL1081: _temp1080= _temp768.f2; goto _LL813;
_LL814: _LL1091: _temp1090= _temp768.f1; goto _LL1087; _LL1087: _temp1086=
_temp768.f2; if(( unsigned int) _temp1086 > 4u?*(( int*) _temp1086) == Cyc_Absyn_AccessEff:
0){ _LL1089: _temp1088=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1086)->f1;
goto _LL815;} else{ goto _LL816;} _LL816: goto _LL817; _LL771: Cyc_Tcutil_unify_it(
t2, t1); return; _LL773: return; _LL775: { struct _tagged_string* _temp1092=
_temp838->name; goto _LL1093; _LL1093: { struct _tagged_string* _temp1094=
_temp834->name; goto _LL1095; _LL1095: { void* _temp1096=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp838->kind); goto _LL1097; _LL1097: { void*
_temp1098=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp834->kind); goto _LL1099; _LL1099: if( Cyc_String_zstrptrcmp( _temp1092,
_temp1094) == 0){ if( _temp1096 != _temp1098){(( void(*)( struct _tagged_string
msg)) Cyc_Tcutil_impos)(({ struct _tagged_string _temp1100=* _temp1092; struct
_tagged_string _temp1101= Cyc_Absynpp_kind2string( _temp1096); struct
_tagged_string _temp1102= Cyc_Absynpp_kind2string( _temp1098); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp1100.last_plus_one - _temp1100.curr, _temp1100.curr, _temp1101.last_plus_one
- _temp1101.curr, _temp1101.curr, _temp1102.last_plus_one - _temp1102.curr,
_temp1102.curr);}));} return;} goto _LL769;}}}} _LL777: if((( _temp846 != 0?
_temp854 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null( _temp846),(
struct _tuple1*) _check_null( _temp854)) == 0: 0)? 1:( _temp846 == 0? _temp854
== 0: 0)){ Cyc_Tcutil_unify_list( _temp844, _temp852); return;} goto _LL769;
_LL779: if( Cyc_Absyn_qvar_cmp( _temp866, _temp860) == 0){ return;} goto _LL769;
_LL781: if((( _temp874 != 0? _temp882 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct
_tuple1*) _check_null( _temp874),( struct _tuple1*) _check_null( _temp882)) == 0:
0)? 1:( _temp874 == 0? _temp882 == 0: 0)){ Cyc_Tcutil_unify_list( _temp872,
_temp880); return;} goto _LL769; _LL783: if( _temp906 == _temp894? 1: Cyc_Absyn_qvar_cmp(
_temp906->name, _temp894->name) == 0){ Cyc_Tcutil_unify_it( _temp888, _temp900);
Cyc_Tcutil_unify_list( _temp890, _temp902); return;} goto _LL769; _LL785: if((
_temp930 == _temp918? 1: Cyc_Absyn_qvar_cmp( _temp930->name, _temp918->name) ==
0)? _temp928 == _temp916? 1: Cyc_Absyn_qvar_cmp( _temp928->name, _temp916->name)
== 0: 0){ Cyc_Tcutil_unify_list( _temp912, _temp924); return;} goto _LL769;
_LL787: Cyc_Tcutil_unify_it( _temp944, _temp958); Cyc_Tcutil_unify_it( _temp956,
_temp942); Cyc_Tcutil_unify_tqual( _temp938, _temp952);(( void(*)( int(* cmp)(
void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Tcutil_boundscmp, _temp936, _temp950);{ void* _temp1103=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp936))->v; void* _temp1109; _LL1105: if(( unsigned int) _temp1103 > 1u?*((
int*) _temp1103) == Cyc_Absyn_Eq_constr: 0){ _LL1110: _temp1109=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1103)->f1; if( _temp1109 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1106;} else{ goto _LL1107;}} else{ goto _LL1107;} _LL1107: goto _LL1108;
_LL1106: return; _LL1108: goto _LL1104; _LL1104:;}(( void(*)( int(* cmp)( int,
int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp940, _temp954); return; _LL789: if( _temp964 == _temp970?
_temp962 == _temp968: 0){ return;} goto _LL769; _LL791: return; _LL793: return;
_LL795: Cyc_Tcutil_unify_it( _temp986, _temp994); Cyc_Tcutil_unify_tqual(
_temp984, _temp992); if( _temp990 == _temp982){ return;} if( _temp990 == 0? 1:
_temp982 == 0){ goto _LL769;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp990)) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp982))){ return;} goto _LL769; _LL797: { int done= 0;{ struct
_RegionHandle _temp1111= _new_region(); struct _RegionHandle* rgn=& _temp1111;
_push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1012 != 0) { if(
_temp1030 == 0){ break;} inst=({ struct Cyc_List_List* _temp1112=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1112->hd=( void*)({
struct _tuple5* _temp1113=( struct _tuple5*) _region_malloc( rgn, sizeof( struct
_tuple5)); _temp1113->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1030))->hd; _temp1113->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1114=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1114[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1115; _temp1115.tag= Cyc_Absyn_VarType;
_temp1115.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp1012))->hd; _temp1115;}); _temp1114;}); _temp1113;}); _temp1112->tl= inst;
_temp1112;}); _temp1012=(( struct Cyc_List_List*) _check_null( _temp1012))->tl;
_temp1030=(( struct Cyc_List_List*) _check_null( _temp1030))->tl;} if( _temp1030
!= 0){ _npop_handler( 0u); goto _LL769;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1116=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1116[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1117; _temp1117.tag= Cyc_Absyn_FnType; _temp1117.f1=({ struct Cyc_Absyn_FnInfo
_temp1118; _temp1118.tvars= 0; _temp1118.effect= _temp1010; _temp1118.ret_typ=(
void*) _temp1008; _temp1118.args= _temp1006; _temp1118.c_varargs= _temp1004;
_temp1118.cyc_varargs= _temp1002; _temp1118.attributes= _temp1000; _temp1118;});
_temp1117;}); _temp1116;}), Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct
Cyc_Absyn_FnType_struct* _temp1119=( struct Cyc_Absyn_FnType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_FnType_struct)); _temp1119[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1120; _temp1120.tag= Cyc_Absyn_FnType; _temp1120.f1=({ struct Cyc_Absyn_FnInfo
_temp1121; _temp1121.tvars= 0; _temp1121.effect= _temp1028; _temp1121.ret_typ=(
void*) _temp1026; _temp1121.args= _temp1024; _temp1121.c_varargs= _temp1022;
_temp1121.cyc_varargs= _temp1020; _temp1121.attributes= _temp1018; _temp1121;});
_temp1120;}); _temp1119;}))); done= 1;}}; _pop_region(& _temp1111);} if( done){
return;} Cyc_Tcutil_unify_it( _temp1008, _temp1026); for( 0; _temp1006 != 0?
_temp1024 != 0: 0;( _temp1006=(( struct Cyc_List_List*) _check_null( _temp1006))->tl,
_temp1024=(( struct Cyc_List_List*) _check_null( _temp1024))->tl)){ Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1006))->hd)).f2,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1024))->hd)).f2);
Cyc_Tcutil_unify_it((*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null(
_temp1006))->hd)).f3,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null(
_temp1024))->hd)).f3);} if( _temp1006 != 0? 1: _temp1024 != 0){ goto _LL769;}
if( _temp1004 != _temp1022){ goto _LL769;}{ int bad_cyc_vararg= 0;{ struct
_tuple10 _temp1123=({ struct _tuple10 _temp1122; _temp1122.f1= _temp1002;
_temp1122.f2= _temp1020; _temp1122;}); struct Cyc_Absyn_VarargInfo* _temp1133;
struct Cyc_Absyn_VarargInfo* _temp1135; struct Cyc_Absyn_VarargInfo* _temp1137;
struct Cyc_Absyn_VarargInfo* _temp1139; struct Cyc_Absyn_VarargInfo* _temp1141;
struct Cyc_Absyn_VarargInfo* _temp1143; struct Cyc_Absyn_VarargInfo* _temp1145;
struct Cyc_Absyn_VarargInfo _temp1147; int _temp1148; void* _temp1150; void*
_temp1152; struct Cyc_Absyn_Tqual _temp1154; struct Cyc_Core_Opt* _temp1156;
struct Cyc_Absyn_VarargInfo* _temp1158; struct Cyc_Absyn_VarargInfo _temp1160;
int _temp1161; void* _temp1163; void* _temp1165; struct Cyc_Absyn_Tqual
_temp1167; struct Cyc_Core_Opt* _temp1169; _LL1125: _LL1136: _temp1135=
_temp1123.f1; if( _temp1135 == 0){ goto _LL1134;} else{ goto _LL1127;} _LL1134:
_temp1133= _temp1123.f2; if( _temp1133 == 0){ goto _LL1126;} else{ goto _LL1127;}
_LL1127: _LL1140: _temp1139= _temp1123.f1; if( _temp1139 == 0){ goto _LL1138;}
else{ goto _LL1129;} _LL1138: _temp1137= _temp1123.f2; goto _LL1128; _LL1129:
_LL1144: _temp1143= _temp1123.f1; goto _LL1142; _LL1142: _temp1141= _temp1123.f2;
if( _temp1141 == 0){ goto _LL1130;} else{ goto _LL1131;} _LL1131: _LL1159:
_temp1158= _temp1123.f1; if( _temp1158 == 0){ goto _LL1124;} else{ _temp1160=*
_temp1158; _LL1170: _temp1169= _temp1160.name; goto _LL1168; _LL1168: _temp1167=
_temp1160.tq; goto _LL1166; _LL1166: _temp1165=( void*) _temp1160.type; goto
_LL1164; _LL1164: _temp1163=( void*) _temp1160.rgn; goto _LL1162; _LL1162:
_temp1161= _temp1160.inject; goto _LL1146;} _LL1146: _temp1145= _temp1123.f2;
if( _temp1145 == 0){ goto _LL1124;} else{ _temp1147=* _temp1145; _LL1157:
_temp1156= _temp1147.name; goto _LL1155; _LL1155: _temp1154= _temp1147.tq; goto
_LL1153; _LL1153: _temp1152=( void*) _temp1147.type; goto _LL1151; _LL1151:
_temp1150=( void*) _temp1147.rgn; goto _LL1149; _LL1149: _temp1148= _temp1147.inject;
goto _LL1132;} _LL1126: goto _LL1124; _LL1128: goto _LL1130; _LL1130:
bad_cyc_vararg= 1; goto _LL1124; _LL1132: Cyc_Tcutil_unify_tqual( _temp1167,
_temp1154); Cyc_Tcutil_unify_it( _temp1165, _temp1152); Cyc_Tcutil_unify_it(
_temp1163, _temp1150); if( _temp1161 != _temp1148){ bad_cyc_vararg= 1;} goto
_LL1124; _LL1124:;} if( bad_cyc_vararg){ goto _LL769;}{ int bad_effect= 0;{
struct _tuple11 _temp1172=({ struct _tuple11 _temp1171; _temp1171.f1= _temp1010;
_temp1171.f2= _temp1028; _temp1171;}); struct Cyc_Core_Opt* _temp1182; struct
Cyc_Core_Opt* _temp1184; struct Cyc_Core_Opt* _temp1186; struct Cyc_Core_Opt*
_temp1188; struct Cyc_Core_Opt* _temp1190; struct Cyc_Core_Opt* _temp1192;
_LL1174: _LL1185: _temp1184= _temp1172.f1; if( _temp1184 == 0){ goto _LL1183;}
else{ goto _LL1176;} _LL1183: _temp1182= _temp1172.f2; if( _temp1182 == 0){ goto
_LL1175;} else{ goto _LL1176;} _LL1176: _LL1189: _temp1188= _temp1172.f1; if(
_temp1188 == 0){ goto _LL1187;} else{ goto _LL1178;} _LL1187: _temp1186=
_temp1172.f2; goto _LL1177; _LL1178: _LL1193: _temp1192= _temp1172.f1; goto
_LL1191; _LL1191: _temp1190= _temp1172.f2; if( _temp1190 == 0){ goto _LL1179;}
else{ goto _LL1180;} _LL1180: goto _LL1181; _LL1175: goto _LL1173; _LL1177: goto
_LL1179; _LL1179: bad_effect= 1; goto _LL1173; _LL1181: bad_effect= ! Cyc_Tcutil_unify_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1010))->v,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1028))->v); goto _LL1173; _LL1173:;} if( bad_effect){ goto
_LL769;} if( ! Cyc_Tcutil_same_atts( _temp1018, _temp1000)){ goto _LL769;}
return;}}} _LL799: for( 0; _temp1034 != 0? _temp1038 != 0: 0;( _temp1034=((
struct Cyc_List_List*) _check_null( _temp1034))->tl, _temp1038=(( struct Cyc_List_List*)
_check_null( _temp1038))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1034))->hd)).f1,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1038))->hd)).f1); Cyc_Tcutil_unify_it((*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1034))->hd)).f2,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1038))->hd)).f2);}
if( _temp1034 == 0? _temp1038 == 0: 0){ return;} goto _LL769; _LL801: _temp1054=
_temp1046; _temp1050= _temp1042; goto _LL803; _LL803: for( 0; _temp1050 != 0?
_temp1054 != 0: 0;( _temp1050=(( struct Cyc_List_List*) _check_null( _temp1050))->tl,
_temp1054=(( struct Cyc_List_List*) _check_null( _temp1054))->tl)){ struct Cyc_Absyn_Structfield*
_temp1194=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1050))->hd; goto _LL1195; _LL1195: { struct Cyc_Absyn_Structfield*
_temp1196=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1054))->hd; goto _LL1197; _LL1197: if( Cyc_String_zstrptrcmp( _temp1194->name,
_temp1196->name) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1194->tq, _temp1196->tq); Cyc_Tcutil_unify_it(( void*) _temp1194->type,(
void*) _temp1196->type); if((( ! Cyc_Tcutil_same_atts( _temp1194->attributes,
_temp1196->attributes)? 1:( _temp1194->width != 0? _temp1196->width == 0: 0))? 1:(
_temp1196->width != 0? _temp1194->width == 0: 0))? 1:(( _temp1194->width != 0?
_temp1196->width != 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp1194->width))->v) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1196->width))->v):
0)){( void) _throw(( void*) Cyc_Tcutil_Unify);}}} if( _temp1050 == 0? _temp1054
== 0: 0){ return;} goto _LL769; _LL805: return; _LL807: Cyc_Tcutil_unify_it(
_temp1066, _temp1062); return; _LL809: goto _LL811; _LL811: goto _LL813; _LL813:
goto _LL815; _LL815: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL769;} _LL817: goto _LL769; _LL769:;}( void) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1198= Cyc_Tcutil_compress(
t); void* _temp1210; void* _temp1212; struct Cyc_Absyn_Enumdecl* _temp1214;
struct _tuple1* _temp1216; _LL1200: if(( unsigned int) _temp1198 > 4u?*(( int*)
_temp1198) == Cyc_Absyn_IntType: 0){ _LL1213: _temp1212=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1198)->f1; goto _LL1211; _LL1211: _temp1210=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1198)->f2; goto _LL1201;} else{ goto _LL1202;} _LL1202: if( _temp1198 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1203;} else{ goto _LL1204;} _LL1204: if(
_temp1198 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1205;} else{ goto _LL1206;}
_LL1206: if(( unsigned int) _temp1198 > 4u?*(( int*) _temp1198) == Cyc_Absyn_EnumType:
0){ _LL1217: _temp1216=(( struct Cyc_Absyn_EnumType_struct*) _temp1198)->f1;
goto _LL1215; _LL1215: _temp1214=(( struct Cyc_Absyn_EnumType_struct*) _temp1198)->f2;
goto _LL1207;} else{ goto _LL1208;} _LL1208: goto _LL1209; _LL1201: return 1;
_LL1203: return 1; _LL1205: return 1; _LL1207: return 1; _LL1209: return 0;
_LL1199:;} int Cyc_Tcutil_will_lose_precision( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp1219=({ struct _tuple7
_temp1218; _temp1218.f1= t1; _temp1218.f2= t2; _temp1218;}); void* _temp1241;
void* _temp1243; void* _temp1245; void* _temp1247; void* _temp1249; void*
_temp1251; void* _temp1253; void* _temp1255; void* _temp1257; void* _temp1259;
void* _temp1261; void* _temp1263; void* _temp1265; void* _temp1267; void*
_temp1269; void* _temp1271; void* _temp1273; void* _temp1275; void* _temp1277;
void* _temp1279; void* _temp1281; void* _temp1283; void* _temp1285; void*
_temp1287; void* _temp1289; void* _temp1291; void* _temp1293; void* _temp1295;
void* _temp1297; void* _temp1299; void* _temp1301; void* _temp1303; void*
_temp1305; void* _temp1307; void* _temp1309; void* _temp1311; void* _temp1313;
void* _temp1315; void* _temp1317; void* _temp1319; void* _temp1321; void*
_temp1323; _LL1221: _LL1244: _temp1243= _temp1219.f1; if( _temp1243 ==( void*)
Cyc_Absyn_DoubleType){ goto _LL1242;} else{ goto _LL1223;} _LL1242: _temp1241=
_temp1219.f2; if( _temp1241 ==( void*) Cyc_Absyn_FloatType){ goto _LL1222;}
else{ goto _LL1223;} _LL1223: _LL1252: _temp1251= _temp1219.f1; if( _temp1251 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL1246;} else{ goto _LL1225;} _LL1246:
_temp1245= _temp1219.f2; if(( unsigned int) _temp1245 > 4u?*(( int*) _temp1245)
== Cyc_Absyn_IntType: 0){ _LL1250: _temp1249=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1245)->f1; goto _LL1248; _LL1248: _temp1247=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1245)->f2; goto _LL1224;} else{ goto _LL1225;} _LL1225: _LL1260: _temp1259=
_temp1219.f1; if( _temp1259 ==( void*) Cyc_Absyn_FloatType){ goto _LL1254;}
else{ goto _LL1227;} _LL1254: _temp1253= _temp1219.f2; if(( unsigned int)
_temp1253 > 4u?*(( int*) _temp1253) == Cyc_Absyn_IntType: 0){ _LL1258: _temp1257=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1253)->f1; goto _LL1256; _LL1256:
_temp1255=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1253)->f2; goto
_LL1226;} else{ goto _LL1227;} _LL1227: _LL1268: _temp1267= _temp1219.f1; if((
unsigned int) _temp1267 > 4u?*(( int*) _temp1267) == Cyc_Absyn_IntType: 0){
_LL1272: _temp1271=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1267)->f1;
goto _LL1270; _LL1270: _temp1269=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1267)->f2; if( _temp1269 ==( void*) Cyc_Absyn_B8){ goto _LL1262;} else{
goto _LL1229;}} else{ goto _LL1229;} _LL1262: _temp1261= _temp1219.f2; if((
unsigned int) _temp1261 > 4u?*(( int*) _temp1261) == Cyc_Absyn_IntType: 0){
_LL1266: _temp1265=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1261)->f1;
goto _LL1264; _LL1264: _temp1263=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1261)->f2; if( _temp1263 ==( void*) Cyc_Absyn_B8){ goto _LL1228;} else{
goto _LL1229;}} else{ goto _LL1229;} _LL1229: _LL1276: _temp1275= _temp1219.f1;
if(( unsigned int) _temp1275 > 4u?*(( int*) _temp1275) == Cyc_Absyn_IntType: 0){
_LL1280: _temp1279=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1275)->f1;
goto _LL1278; _LL1278: _temp1277=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1275)->f2; if( _temp1277 ==( void*) Cyc_Absyn_B8){ goto _LL1274;} else{
goto _LL1231;}} else{ goto _LL1231;} _LL1274: _temp1273= _temp1219.f2; goto
_LL1230; _LL1231: _LL1284: _temp1283= _temp1219.f1; if(( unsigned int) _temp1283
> 4u?*(( int*) _temp1283) == Cyc_Absyn_IntType: 0){ _LL1288: _temp1287=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1283)->f1; goto _LL1286; _LL1286:
_temp1285=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1283)->f2; if(
_temp1285 ==( void*) Cyc_Absyn_B4){ goto _LL1282;} else{ goto _LL1233;}} else{
goto _LL1233;} _LL1282: _temp1281= _temp1219.f2; if( _temp1281 ==( void*) Cyc_Absyn_FloatType){
goto _LL1232;} else{ goto _LL1233;} _LL1233: _LL1296: _temp1295= _temp1219.f1;
if(( unsigned int) _temp1295 > 4u?*(( int*) _temp1295) == Cyc_Absyn_IntType: 0){
_LL1300: _temp1299=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1295)->f1;
goto _LL1298; _LL1298: _temp1297=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1295)->f2; if( _temp1297 ==( void*) Cyc_Absyn_B4){ goto _LL1290;} else{
goto _LL1235;}} else{ goto _LL1235;} _LL1290: _temp1289= _temp1219.f2; if((
unsigned int) _temp1289 > 4u?*(( int*) _temp1289) == Cyc_Absyn_IntType: 0){
_LL1294: _temp1293=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1289)->f1;
goto _LL1292; _LL1292: _temp1291=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1289)->f2; if( _temp1291 ==( void*) Cyc_Absyn_B2){ goto _LL1234;} else{
goto _LL1235;}} else{ goto _LL1235;} _LL1235: _LL1308: _temp1307= _temp1219.f1;
if(( unsigned int) _temp1307 > 4u?*(( int*) _temp1307) == Cyc_Absyn_IntType: 0){
_LL1312: _temp1311=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1307)->f1;
goto _LL1310; _LL1310: _temp1309=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1307)->f2; if( _temp1309 ==( void*) Cyc_Absyn_B4){ goto _LL1302;} else{
goto _LL1237;}} else{ goto _LL1237;} _LL1302: _temp1301= _temp1219.f2; if((
unsigned int) _temp1301 > 4u?*(( int*) _temp1301) == Cyc_Absyn_IntType: 0){
_LL1306: _temp1305=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1301)->f1;
goto _LL1304; _LL1304: _temp1303=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1301)->f2; if( _temp1303 ==( void*) Cyc_Absyn_B1){ goto _LL1236;} else{
goto _LL1237;}} else{ goto _LL1237;} _LL1237: _LL1320: _temp1319= _temp1219.f1;
if(( unsigned int) _temp1319 > 4u?*(( int*) _temp1319) == Cyc_Absyn_IntType: 0){
_LL1324: _temp1323=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1319)->f1;
goto _LL1322; _LL1322: _temp1321=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1319)->f2; if( _temp1321 ==( void*) Cyc_Absyn_B2){ goto _LL1314;} else{
goto _LL1239;}} else{ goto _LL1239;} _LL1314: _temp1313= _temp1219.f2; if((
unsigned int) _temp1313 > 4u?*(( int*) _temp1313) == Cyc_Absyn_IntType: 0){
_LL1318: _temp1317=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1313)->f1;
goto _LL1316; _LL1316: _temp1315=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1313)->f2; if( _temp1315 ==( void*) Cyc_Absyn_B1){ goto _LL1238;} else{
goto _LL1239;}} else{ goto _LL1239;} _LL1239: goto _LL1240; _LL1222: return 1;
_LL1224: return 1; _LL1226: return 1; _LL1228: return 0; _LL1230: return 1;
_LL1232: return 1; _LL1234: return 1; _LL1236: return 1; _LL1238: return 1;
_LL1240: return 0; _LL1220:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1325=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1325->v=( void*) t1; _temp1325;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
el))->hd)->loc,({ struct _tagged_string _temp1326= Cyc_Absynpp_typ2string( t);
struct _tagged_string _temp1327= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
xprintf("type mismatch: expecting %.*s but found %.*s", _temp1326.last_plus_one
- _temp1326.curr, _temp1326.curr, _temp1327.last_plus_one - _temp1327.curr,
_temp1327.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1328= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1334; _LL1330: if((
unsigned int) _temp1328 > 4u?*(( int*) _temp1328) == Cyc_Absyn_PointerType: 0){
_LL1335: _temp1334=(( struct Cyc_Absyn_PointerType_struct*) _temp1328)->f1; goto
_LL1331;} else{ goto _LL1332;} _LL1332: goto _LL1333; _LL1331: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1329; _LL1333: return 0; _LL1329:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1336= Cyc_Tcutil_compress(
t); void* _temp1344; void* _temp1346; struct Cyc_Absyn_Enumdecl* _temp1348;
struct _tuple1* _temp1350; _LL1338: if(( unsigned int) _temp1336 > 4u?*(( int*)
_temp1336) == Cyc_Absyn_IntType: 0){ _LL1347: _temp1346=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1336)->f1; goto _LL1345; _LL1345: _temp1344=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1336)->f2; goto _LL1339;} else{ goto _LL1340;} _LL1340: if(( unsigned int)
_temp1336 > 4u?*(( int*) _temp1336) == Cyc_Absyn_EnumType: 0){ _LL1351:
_temp1350=(( struct Cyc_Absyn_EnumType_struct*) _temp1336)->f1; goto _LL1349;
_LL1349: _temp1348=(( struct Cyc_Absyn_EnumType_struct*) _temp1336)->f2; goto
_LL1341;} else{ goto _LL1342;} _LL1342: goto _LL1343; _LL1339: return 1; _LL1341:
return 1; _LL1343: return 0; _LL1337:;} int Cyc_Tcutil_coerce_uint_typ( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,({ unsigned char*
_temp1352="integral size mismatch; conversion supplied"; struct _tagged_string
_temp1353; _temp1353.curr= _temp1352; _temp1353.base= _temp1352; _temp1353.last_plus_one=
_temp1352 + 44; _temp1353;}));} Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_uint_t);
return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){
Cyc_Tcutil_warn( e->loc,({ unsigned char* _temp1354="integral size mismatch; conversion supplied";
struct _tagged_string _temp1355; _temp1355.curr= _temp1354; _temp1355.base=
_temp1354; _temp1355.last_plus_one= _temp1354 + 44; _temp1355;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7
_temp1357=({ struct _tuple7 _temp1356; _temp1356.f1= t1; _temp1356.f2= t2;
_temp1356;}); void* _temp1369; struct Cyc_Absyn_PtrInfo _temp1371; void*
_temp1373; struct Cyc_Absyn_PtrInfo _temp1375; void* _temp1377; struct Cyc_Absyn_Exp*
_temp1379; struct Cyc_Absyn_Tqual _temp1381; void* _temp1383; void* _temp1385;
struct Cyc_Absyn_Exp* _temp1387; struct Cyc_Absyn_Tqual _temp1389; void*
_temp1391; void* _temp1393; struct Cyc_Absyn_TunionInfo _temp1395; void*
_temp1397; struct Cyc_List_List* _temp1399; void* _temp1401; struct Cyc_Absyn_Tuniondecl*
_temp1403; void* _temp1405; struct Cyc_Absyn_TunionFieldInfo _temp1407; struct
Cyc_List_List* _temp1409; void* _temp1411; struct Cyc_Absyn_Tunionfield*
_temp1413; struct Cyc_Absyn_Tuniondecl* _temp1415; void* _temp1417; struct Cyc_Absyn_TunionInfo
_temp1419; void* _temp1421; struct Cyc_List_List* _temp1423; void* _temp1425;
struct Cyc_Absyn_Tuniondecl* _temp1427; void* _temp1429; struct Cyc_Absyn_PtrInfo
_temp1431; struct Cyc_Absyn_Conref* _temp1433; struct Cyc_Absyn_Tqual _temp1435;
struct Cyc_Absyn_Conref* _temp1437; void* _temp1439; void* _temp1441; _LL1359:
_LL1374: _temp1373= _temp1357.f1; if(( unsigned int) _temp1373 > 4u?*(( int*)
_temp1373) == Cyc_Absyn_PointerType: 0){ _LL1376: _temp1375=(( struct Cyc_Absyn_PointerType_struct*)
_temp1373)->f1; goto _LL1370;} else{ goto _LL1361;} _LL1370: _temp1369=
_temp1357.f2; if(( unsigned int) _temp1369 > 4u?*(( int*) _temp1369) == Cyc_Absyn_PointerType:
0){ _LL1372: _temp1371=(( struct Cyc_Absyn_PointerType_struct*) _temp1369)->f1;
goto _LL1360;} else{ goto _LL1361;} _LL1361: _LL1386: _temp1385= _temp1357.f1;
if(( unsigned int) _temp1385 > 4u?*(( int*) _temp1385) == Cyc_Absyn_ArrayType: 0){
_LL1392: _temp1391=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1385)->f1;
goto _LL1390; _LL1390: _temp1389=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1385)->f2; goto _LL1388; _LL1388: _temp1387=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1385)->f3; goto _LL1378;} else{ goto _LL1363;} _LL1378: _temp1377=
_temp1357.f2; if(( unsigned int) _temp1377 > 4u?*(( int*) _temp1377) == Cyc_Absyn_ArrayType:
0){ _LL1384: _temp1383=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1377)->f1;
goto _LL1382; _LL1382: _temp1381=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1377)->f2; goto _LL1380; _LL1380: _temp1379=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1377)->f3; goto _LL1362;} else{ goto _LL1363;} _LL1363: _LL1406: _temp1405=
_temp1357.f1; if(( unsigned int) _temp1405 > 4u?*(( int*) _temp1405) == Cyc_Absyn_TunionFieldType:
0){ _LL1408: _temp1407=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1405)->f1;
_LL1412: _temp1411=( void*) _temp1407.field_info; if(*(( int*) _temp1411) == Cyc_Absyn_KnownTunionfield){
_LL1416: _temp1415=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1411)->f1;
goto _LL1414; _LL1414: _temp1413=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1411)->f2; goto _LL1410;} else{ goto _LL1365;} _LL1410: _temp1409=
_temp1407.targs; goto _LL1394;} else{ goto _LL1365;} _LL1394: _temp1393=
_temp1357.f2; if(( unsigned int) _temp1393 > 4u?*(( int*) _temp1393) == Cyc_Absyn_TunionType:
0){ _LL1396: _temp1395=(( struct Cyc_Absyn_TunionType_struct*) _temp1393)->f1;
_LL1402: _temp1401=( void*) _temp1395.tunion_info; if(*(( int*) _temp1401) ==
Cyc_Absyn_KnownTunion){ _LL1404: _temp1403=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1401)->f1; goto _LL1400;} else{ goto _LL1365;} _LL1400: _temp1399=
_temp1395.targs; goto _LL1398; _LL1398: _temp1397=( void*) _temp1395.rgn; goto
_LL1364;} else{ goto _LL1365;} _LL1365: _LL1430: _temp1429= _temp1357.f1; if((
unsigned int) _temp1429 > 4u?*(( int*) _temp1429) == Cyc_Absyn_PointerType: 0){
_LL1432: _temp1431=(( struct Cyc_Absyn_PointerType_struct*) _temp1429)->f1;
_LL1442: _temp1441=( void*) _temp1431.elt_typ; goto _LL1440; _LL1440: _temp1439=(
void*) _temp1431.rgn_typ; goto _LL1438; _LL1438: _temp1437= _temp1431.nullable;
goto _LL1436; _LL1436: _temp1435= _temp1431.tq; goto _LL1434; _LL1434: _temp1433=
_temp1431.bounds; goto _LL1418;} else{ goto _LL1367;} _LL1418: _temp1417=
_temp1357.f2; if(( unsigned int) _temp1417 > 4u?*(( int*) _temp1417) == Cyc_Absyn_TunionType:
0){ _LL1420: _temp1419=(( struct Cyc_Absyn_TunionType_struct*) _temp1417)->f1;
_LL1426: _temp1425=( void*) _temp1419.tunion_info; if(*(( int*) _temp1425) ==
Cyc_Absyn_KnownTunion){ _LL1428: _temp1427=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1425)->f1; goto _LL1424;} else{ goto _LL1367;} _LL1424: _temp1423=
_temp1419.targs; goto _LL1422; _LL1422: _temp1421=( void*) _temp1419.rgn; goto
_LL1366;} else{ goto _LL1367;} _LL1367: goto _LL1368; _LL1360: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1375.nullable, _temp1371.nullable)){
void* _temp1443=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1375.nullable))->v; int _temp1449; _LL1445:
if(( unsigned int) _temp1443 > 1u?*(( int*) _temp1443) == Cyc_Absyn_Eq_constr: 0){
_LL1450: _temp1449=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1443)->f1;
goto _LL1446;} else{ goto _LL1447;} _LL1447: goto _LL1448; _LL1446: okay= !
_temp1449; goto _LL1444; _LL1448:( void) _throw((( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp1451="silent_castable conref not eq";
struct _tagged_string _temp1452; _temp1452.curr= _temp1451; _temp1452.base=
_temp1451; _temp1452.last_plus_one= _temp1451 + 30; _temp1452;}))); _LL1444:;}
if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct
Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp, _temp1375.bounds,
_temp1371.bounds)){ struct _tuple7 _temp1454=({ struct _tuple7 _temp1453;
_temp1453.f1=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1375.bounds))->v; _temp1453.f2=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1371.bounds))->v; _temp1453;}); void* _temp1462; void* _temp1464; void*
_temp1466; void* _temp1468; struct Cyc_Absyn_Exp* _temp1470; void* _temp1472;
void* _temp1474; void* _temp1476; void* _temp1478; _LL1456: _LL1467: _temp1466=
_temp1454.f1; if(( unsigned int) _temp1466 > 1u?*(( int*) _temp1466) == Cyc_Absyn_Eq_constr:
0){ _LL1469: _temp1468=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1466)->f1;
if(( unsigned int) _temp1468 > 1u?*(( int*) _temp1468) == Cyc_Absyn_Upper_b: 0){
_LL1471: _temp1470=(( struct Cyc_Absyn_Upper_b_struct*) _temp1468)->f1; goto
_LL1463;} else{ goto _LL1458;}} else{ goto _LL1458;} _LL1463: _temp1462=
_temp1454.f2; if(( unsigned int) _temp1462 > 1u?*(( int*) _temp1462) == Cyc_Absyn_Eq_constr:
0){ _LL1465: _temp1464=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1462)->f1;
if( _temp1464 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1457;} else{ goto _LL1458;}}
else{ goto _LL1458;} _LL1458: _LL1477: _temp1476= _temp1454.f1; if((
unsigned int) _temp1476 > 1u?*(( int*) _temp1476) == Cyc_Absyn_Eq_constr: 0){
_LL1479: _temp1478=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1476)->f1;
if( _temp1478 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1473;} else{ goto _LL1460;}}
else{ goto _LL1460;} _LL1473: _temp1472= _temp1454.f2; if(( unsigned int)
_temp1472 > 1u?*(( int*) _temp1472) == Cyc_Absyn_Eq_constr: 0){ _LL1475:
_temp1474=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1472)->f1; if(
_temp1474 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1459;} else{ goto _LL1460;}}
else{ goto _LL1460;} _LL1460: goto _LL1461; _LL1457: okay= 1; goto _LL1455;
_LL1459: okay= 1; goto _LL1455; _LL1461: okay= 0; goto _LL1455; _LL1455:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1375.elt_typ,( void*) _temp1371.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1375.rgn_typ,( void*) _temp1371.rgn_typ): 0)?
!( _temp1375.tq).q_const? 1:( _temp1371.tq).q_const: 0;} _LL1362: { int okay;
okay=( _temp1387 != 0? _temp1379 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1387)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1379)): 0; return( okay? Cyc_Tcutil_unify(
_temp1391, _temp1383): 0)? ! _temp1389.q_const? 1: _temp1381.q_const: 0;}
_LL1364: if(( _temp1415 == _temp1403? 1: Cyc_Absyn_qvar_cmp( _temp1415->name,
_temp1403->name) == 0)? _temp1413->typs == 0: 0){ for( 0; _temp1409 != 0?
_temp1399 != 0: 0;( _temp1409=(( struct Cyc_List_List*) _check_null( _temp1409))->tl,
_temp1399=(( struct Cyc_List_List*) _check_null( _temp1399))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1409))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1399))->hd)){ break;}} if( _temp1409 == 0?
_temp1399 == 0: 0){ return 1;}} return 0; _LL1366:{ void* _temp1480= Cyc_Tcutil_compress(
_temp1441); struct Cyc_Absyn_TunionFieldInfo _temp1486; struct Cyc_List_List*
_temp1488; void* _temp1490; struct Cyc_Absyn_Tunionfield* _temp1492; struct Cyc_Absyn_Tuniondecl*
_temp1494; _LL1482: if(( unsigned int) _temp1480 > 4u?*(( int*) _temp1480) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1487: _temp1486=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1480)->f1; _LL1491: _temp1490=( void*) _temp1486.field_info; if(*(( int*)
_temp1490) == Cyc_Absyn_KnownTunionfield){ _LL1495: _temp1494=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1490)->f1; goto _LL1493; _LL1493: _temp1492=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1490)->f2; goto _LL1489;} else{ goto _LL1484;} _LL1489: _temp1488=
_temp1486.targs; goto _LL1483;} else{ goto _LL1484;} _LL1484: goto _LL1485;
_LL1483: if( ! Cyc_Tcutil_unify( _temp1439, _temp1421)? _temp1439 !=( void*) Cyc_Absyn_HeapRgn:
0){ return 0;} if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1437,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0))){
return 0;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1433,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_Absyn_bounds_one))){
return 0;} if( Cyc_Absyn_qvar_cmp( _temp1427->name, _temp1494->name) == 0?
_temp1492->typs != 0: 0){ int okay= 1; for( 0; _temp1488 != 0? _temp1423 != 0: 0;(
_temp1488=(( struct Cyc_List_List*) _check_null( _temp1488))->tl, _temp1423=((
struct Cyc_List_List*) _check_null( _temp1423))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1488))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1423))->hd)){ okay= 0; break;}} if(( ! okay? 1:
_temp1488 != 0)? 1: _temp1423 != 0){ return 0;} return 1;} goto _LL1481; _LL1485:
goto _LL1481; _LL1481:;} return 0; _LL1368: return Cyc_Tcutil_unify( t1, t2);
_LL1358:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){ Cyc_Tcutil_warn( e->loc,({ unsigned char* _temp1496="integral size mismatch; conversion supplied";
struct _tagged_string _temp1497; _temp1497.curr= _temp1496; _temp1497.base=
_temp1496; _temp1497.last_plus_one= _temp1496 + 44; _temp1497;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1498= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1499= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1498.last_plus_one - _temp1498.curr, _temp1498.curr, _temp1499.last_plus_one
- _temp1499.curr, _temp1499.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1500= old_typ; struct Cyc_Absyn_Enumdecl*
_temp1512; struct _tuple1* _temp1514; void* _temp1516; void* _temp1518; _LL1502:
if(( unsigned int) _temp1500 > 4u?*(( int*) _temp1500) == Cyc_Absyn_EnumType: 0){
_LL1515: _temp1514=(( struct Cyc_Absyn_EnumType_struct*) _temp1500)->f1; goto
_LL1513; _LL1513: _temp1512=(( struct Cyc_Absyn_EnumType_struct*) _temp1500)->f2;
goto _LL1503;} else{ goto _LL1504;} _LL1504: if(( unsigned int) _temp1500 > 4u?*((
int*) _temp1500) == Cyc_Absyn_IntType: 0){ _LL1519: _temp1518=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1500)->f1; goto _LL1517; _LL1517: _temp1516=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1500)->f2; goto _LL1505;} else{
goto _LL1506;} _LL1506: if( _temp1500 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1507;} else{ goto _LL1508;} _LL1508: if( _temp1500 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1509;} else{ goto _LL1510;} _LL1510: goto _LL1511; _LL1503: t= Cyc_Absyn_sint_t;
goto _LL1501; _LL1505:{ void* _temp1520= _temp1516; _LL1522: if( _temp1520 ==(
void*) Cyc_Absyn_B1){ goto _LL1523;} else{ goto _LL1524;} _LL1524: if( _temp1520
==( void*) Cyc_Absyn_B2){ goto _LL1525;} else{ goto _LL1526;} _LL1526: if(
_temp1520 ==( void*) Cyc_Absyn_B4){ goto _LL1527;} else{ goto _LL1528;} _LL1528:
if( _temp1520 ==( void*) Cyc_Absyn_B8){ goto _LL1529;} else{ goto _LL1521;}
_LL1523: t= Cyc_Absyn_sint_t; goto _LL1521; _LL1525: t= Cyc_Absyn_sint_t; goto
_LL1521; _LL1527: t= _temp1518 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1521; _LL1529: t= _temp1518 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1521; _LL1521:;} goto _LL1501;
_LL1507: t= Cyc_Absyn_float_t; goto _LL1501; _LL1509: t= Cyc_Absyn_double_t;
goto _LL1501; _LL1511: return 0; _LL1501:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote(
te, e); if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,
t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,({ struct _tagged_string _temp1530= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp1531= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1530.last_plus_one - _temp1530.curr, _temp1530.curr, _temp1531.last_plus_one
- _temp1531.curr, _temp1531.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1532= Cyc_Tcutil_compress( t); void* _temp1542; void*
_temp1544; _LL1534: if(( unsigned int) _temp1532 > 4u?*(( int*) _temp1532) ==
Cyc_Absyn_IntType: 0){ _LL1545: _temp1544=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1532)->f1; goto _LL1543; _LL1543: _temp1542=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1532)->f2; goto _LL1535;} else{ goto _LL1536;} _LL1536: if( _temp1532 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1537;} else{ goto _LL1538;} _LL1538: if(
_temp1532 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1539;} else{ goto _LL1540;}
_LL1540: goto _LL1541; _LL1535: return 1; _LL1537: return 1; _LL1539: return 1;
_LL1541: return 0; _LL1533:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp1546=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1546->f1= x->tq; _temp1546->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp1546;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp1547= t1; struct Cyc_List_List*
_temp1559; struct Cyc_Absyn_Structdecl** _temp1561; struct Cyc_List_List*
_temp1563; struct _tuple1* _temp1565; struct Cyc_List_List* _temp1567; _LL1549:
if( _temp1547 ==( void*) Cyc_Absyn_VoidType){ goto _LL1550;} else{ goto _LL1551;}
_LL1551: if(( unsigned int) _temp1547 > 4u?*(( int*) _temp1547) == Cyc_Absyn_TupleType:
0){ _LL1560: _temp1559=(( struct Cyc_Absyn_TupleType_struct*) _temp1547)->f1;
goto _LL1552;} else{ goto _LL1553;} _LL1553: if(( unsigned int) _temp1547 > 4u?*((
int*) _temp1547) == Cyc_Absyn_StructType: 0){ _LL1566: _temp1565=(( struct Cyc_Absyn_StructType_struct*)
_temp1547)->f1; goto _LL1564; _LL1564: _temp1563=(( struct Cyc_Absyn_StructType_struct*)
_temp1547)->f2; goto _LL1562; _LL1562: _temp1561=(( struct Cyc_Absyn_StructType_struct*)
_temp1547)->f3; goto _LL1554;} else{ goto _LL1555;} _LL1555: if(( unsigned int)
_temp1547 > 4u?*(( int*) _temp1547) == Cyc_Absyn_AnonStructType: 0){ _LL1568:
_temp1567=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1547)->f1; goto
_LL1556;} else{ goto _LL1557;} _LL1557: goto _LL1558; _LL1550: return 0; _LL1552:
return _temp1559; _LL1554: if( _temp1561 == 0? 1: _temp1565 == 0){ return({
struct Cyc_List_List* _temp1569=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1569->hd=( void*)({ struct _tuple4* _temp1570=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1570->f1= Cyc_Absyn_empty_tqual();
_temp1570->f2= t1; _temp1570;}); _temp1569->tl= 0; _temp1569;});}{ struct Cyc_Absyn_Structdecl*
_temp1571=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1561)); goto
_LL1572; _LL1572: { struct _tuple1 _temp1573=*(( struct _tuple1*) _check_null(
_temp1565)); goto _LL1574; _LL1574: { struct Cyc_List_List* _temp1575=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1571->tvs, _temp1563); goto _LL1576; _LL1576: if( _temp1571->fields == 0){
return({ struct Cyc_List_List* _temp1577=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1577->hd=( void*)({ struct _tuple4*
_temp1578=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1578->f1=
Cyc_Absyn_empty_tqual(); _temp1578->f2= t1; _temp1578;}); _temp1577->tl= 0;
_temp1577;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1575,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1571->fields))->v);}}}
_LL1556: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1567); _LL1558: return({
struct Cyc_List_List* _temp1579=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1579->hd=( void*)({ struct _tuple4* _temp1580=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1580->f1= Cyc_Absyn_empty_tqual();
_temp1580->f2= t1; _temp1580;}); _temp1579->tl= 0; _temp1579;}); _LL1548:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple7*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple7 _temp1582=({ struct _tuple7 _temp1581; _temp1581.f1= t1; _temp1581.f2=
t2; _temp1581;}); void* _temp1588; struct Cyc_Absyn_PtrInfo _temp1590; struct
Cyc_Absyn_Conref* _temp1592; struct Cyc_Absyn_Tqual _temp1594; struct Cyc_Absyn_Conref*
_temp1596; void* _temp1598; void* _temp1600; void* _temp1602; struct Cyc_Absyn_PtrInfo
_temp1604; struct Cyc_Absyn_Conref* _temp1606; struct Cyc_Absyn_Tqual _temp1608;
struct Cyc_Absyn_Conref* _temp1610; void* _temp1612; void* _temp1614; _LL1584:
_LL1603: _temp1602= _temp1582.f1; if(( unsigned int) _temp1602 > 4u?*(( int*)
_temp1602) == Cyc_Absyn_PointerType: 0){ _LL1605: _temp1604=(( struct Cyc_Absyn_PointerType_struct*)
_temp1602)->f1; _LL1615: _temp1614=( void*) _temp1604.elt_typ; goto _LL1613;
_LL1613: _temp1612=( void*) _temp1604.rgn_typ; goto _LL1611; _LL1611: _temp1610=
_temp1604.nullable; goto _LL1609; _LL1609: _temp1608= _temp1604.tq; goto _LL1607;
_LL1607: _temp1606= _temp1604.bounds; goto _LL1589;} else{ goto _LL1586;}
_LL1589: _temp1588= _temp1582.f2; if(( unsigned int) _temp1588 > 4u?*(( int*)
_temp1588) == Cyc_Absyn_PointerType: 0){ _LL1591: _temp1590=(( struct Cyc_Absyn_PointerType_struct*)
_temp1588)->f1; _LL1601: _temp1600=( void*) _temp1590.elt_typ; goto _LL1599;
_LL1599: _temp1598=( void*) _temp1590.rgn_typ; goto _LL1597; _LL1597: _temp1596=
_temp1590.nullable; goto _LL1595; _LL1595: _temp1594= _temp1590.tq; goto _LL1593;
_LL1593: _temp1592= _temp1590.bounds; goto _LL1585;} else{ goto _LL1586;}
_LL1586: goto _LL1587; _LL1585: if( _temp1608.q_const? ! _temp1594.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1610, _temp1596)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1610): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1596): 0){ return 0;}{ void* _temp1616= _temp1612; _LL1618: if( _temp1616
==( void*) Cyc_Absyn_HeapRgn){ goto _LL1619;} else{ goto _LL1620;} _LL1620: goto
_LL1621; _LL1619: goto _LL1617; _LL1621: Cyc_Tcutil_unify( _temp1612, _temp1598);
goto _LL1617; _LL1617:;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1606, _temp1592)){ struct _tuple7 _temp1623=({ struct _tuple7 _temp1622;
_temp1622.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1606); _temp1622.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1592); _temp1622;}); void* _temp1631; void* _temp1633; struct Cyc_Absyn_Exp*
_temp1635; void* _temp1637; struct Cyc_Absyn_Exp* _temp1639; void* _temp1641;
struct Cyc_Absyn_Exp* _temp1643; _LL1625: _LL1634: _temp1633= _temp1623.f1; if((
unsigned int) _temp1633 > 1u?*(( int*) _temp1633) == Cyc_Absyn_Upper_b: 0){
_LL1636: _temp1635=(( struct Cyc_Absyn_Upper_b_struct*) _temp1633)->f1; goto
_LL1632;} else{ goto _LL1627;} _LL1632: _temp1631= _temp1623.f2; if( _temp1631
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1626;} else{ goto _LL1627;} _LL1627:
_LL1642: _temp1641= _temp1623.f1; if(( unsigned int) _temp1641 > 1u?*(( int*)
_temp1641) == Cyc_Absyn_Upper_b: 0){ _LL1644: _temp1643=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1641)->f1; goto _LL1638;} else{ goto _LL1629;} _LL1638: _temp1637=
_temp1623.f2; if(( unsigned int) _temp1637 > 1u?*(( int*) _temp1637) == Cyc_Absyn_Upper_b:
0){ _LL1640: _temp1639=(( struct Cyc_Absyn_Upper_b_struct*) _temp1637)->f1; goto
_LL1628;} else{ goto _LL1629;} _LL1629: goto _LL1630; _LL1626: goto _LL1624;
_LL1628: if( Cyc_Evexp_eval_const_uint_exp( _temp1643) < Cyc_Evexp_eval_const_uint_exp(
_temp1639)){ return 0;} goto _LL1624; _LL1630: return 0; _LL1624:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1645=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1645->hd=( void*)({ struct _tuple7*
_temp1646=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1646->f1=
t1; _temp1646->f2= t2; _temp1646;}); _temp1645->tl= assume; _temp1645;}),
_temp1614, _temp1600); _LL1587: return 0; _LL1583:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2){
struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1); struct Cyc_List_List*
tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;( tqs2=(( struct Cyc_List_List*)
_check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*) _check_null( tqs1))->tl)){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1649; void* _temp1650; struct
Cyc_Absyn_Tqual _temp1652; struct _tuple4* _temp1647=( struct _tuple4*)(( struct
Cyc_List_List*) _check_null( tqs1))->hd; _temp1649=* _temp1647; _LL1653:
_temp1652= _temp1649.f1; goto _LL1651; _LL1651: _temp1650= _temp1649.f2; goto
_LL1648; _LL1648: { struct _tuple4 _temp1656; void* _temp1657; struct Cyc_Absyn_Tqual
_temp1659; struct _tuple4* _temp1654=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs2))->hd; _temp1656=* _temp1654; _LL1660: _temp1659= _temp1656.f1;
goto _LL1658; _LL1658: _temp1657= _temp1656.f2; goto _LL1655; _LL1655: if(
_temp1659.q_const? Cyc_Tcutil_subtype( te, assume, _temp1650, _temp1657): 0){
continue;} else{ if( Cyc_Tcutil_unify( _temp1650, _temp1657)){ continue;} else{
return 0;}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp1661= t2; struct Cyc_Absyn_Uniondecl** _temp1669; struct Cyc_List_List*
_temp1671; struct _tuple1* _temp1673; void* _temp1675; void* _temp1677; _LL1663:
if(( unsigned int) _temp1661 > 4u?*(( int*) _temp1661) == Cyc_Absyn_UnionType: 0){
_LL1674: _temp1673=(( struct Cyc_Absyn_UnionType_struct*) _temp1661)->f1; goto
_LL1672; _LL1672: _temp1671=(( struct Cyc_Absyn_UnionType_struct*) _temp1661)->f2;
goto _LL1670; _LL1670: _temp1669=(( struct Cyc_Absyn_UnionType_struct*)
_temp1661)->f3; goto _LL1664;} else{ goto _LL1665;} _LL1665: if(( unsigned int)
_temp1661 > 4u?*(( int*) _temp1661) == Cyc_Absyn_IntType: 0){ _LL1678: _temp1677=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1661)->f1; goto _LL1676; _LL1676:
_temp1675=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1661)->f2; if(
_temp1675 ==( void*) Cyc_Absyn_B4){ goto _LL1666;} else{ goto _LL1667;}} else{
goto _LL1667;} _LL1667: goto _LL1668; _LL1664: if( _temp1669 == 0){ goto _LL1662;}{
struct Cyc_Absyn_Uniondecl* _temp1679=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1669)); goto _LL1680; _LL1680: if( _temp1679->fields == 0){
goto _LL1662;}{ struct Cyc_List_List* _temp1681=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1679->tvs,
_temp1671); goto _LL1682; _LL1682:{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1679->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp1681,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ return 1;}}} goto _LL1662;}} _LL1666: if( Cyc_Tcutil_typ_kind(
t2) ==( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL1662; _LL1668: goto
_LL1662; _LL1662:;}{ void* _temp1683= t1; struct Cyc_Absyn_PtrInfo _temp1699;
struct Cyc_Absyn_Conref* _temp1701; struct Cyc_Absyn_Tqual _temp1703; struct Cyc_Absyn_Conref*
_temp1705; void* _temp1707; void* _temp1709; struct Cyc_Absyn_Exp* _temp1711;
struct Cyc_Absyn_Tqual _temp1713; void* _temp1715; struct Cyc_Absyn_Enumdecl*
_temp1717; struct _tuple1* _temp1719; void* _temp1721; void* _temp1723; _LL1685:
if(( unsigned int) _temp1683 > 4u?*(( int*) _temp1683) == Cyc_Absyn_PointerType:
0){ _LL1700: _temp1699=(( struct Cyc_Absyn_PointerType_struct*) _temp1683)->f1;
_LL1710: _temp1709=( void*) _temp1699.elt_typ; goto _LL1708; _LL1708: _temp1707=(
void*) _temp1699.rgn_typ; goto _LL1706; _LL1706: _temp1705= _temp1699.nullable;
goto _LL1704; _LL1704: _temp1703= _temp1699.tq; goto _LL1702; _LL1702: _temp1701=
_temp1699.bounds; goto _LL1686;} else{ goto _LL1687;} _LL1687: if(( unsigned int)
_temp1683 > 4u?*(( int*) _temp1683) == Cyc_Absyn_ArrayType: 0){ _LL1716:
_temp1715=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1683)->f1; goto
_LL1714; _LL1714: _temp1713=(( struct Cyc_Absyn_ArrayType_struct*) _temp1683)->f2;
goto _LL1712; _LL1712: _temp1711=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1683)->f3; goto _LL1688;} else{ goto _LL1689;} _LL1689: if(( unsigned int)
_temp1683 > 4u?*(( int*) _temp1683) == Cyc_Absyn_EnumType: 0){ _LL1720:
_temp1719=(( struct Cyc_Absyn_EnumType_struct*) _temp1683)->f1; goto _LL1718;
_LL1718: _temp1717=(( struct Cyc_Absyn_EnumType_struct*) _temp1683)->f2; goto
_LL1690;} else{ goto _LL1691;} _LL1691: if(( unsigned int) _temp1683 > 4u?*((
int*) _temp1683) == Cyc_Absyn_IntType: 0){ _LL1724: _temp1723=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1683)->f1; goto _LL1722; _LL1722: _temp1721=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1683)->f2; goto _LL1692;} else{
goto _LL1693;} _LL1693: if( _temp1683 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1694;} else{ goto _LL1695;} _LL1695: if( _temp1683 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1696;} else{ goto _LL1697;} _LL1697: goto _LL1698; _LL1686:{ void*
_temp1725= t2; struct Cyc_Absyn_PtrInfo _temp1731; struct Cyc_Absyn_Conref*
_temp1733; struct Cyc_Absyn_Tqual _temp1735; struct Cyc_Absyn_Conref* _temp1737;
void* _temp1739; void* _temp1741; _LL1727: if(( unsigned int) _temp1725 > 4u?*((
int*) _temp1725) == Cyc_Absyn_PointerType: 0){ _LL1732: _temp1731=(( struct Cyc_Absyn_PointerType_struct*)
_temp1725)->f1; _LL1742: _temp1741=( void*) _temp1731.elt_typ; goto _LL1740;
_LL1740: _temp1739=( void*) _temp1731.rgn_typ; goto _LL1738; _LL1738: _temp1737=
_temp1731.nullable; goto _LL1736; _LL1736: _temp1735= _temp1731.tq; goto _LL1734;
_LL1734: _temp1733= _temp1731.bounds; goto _LL1728;} else{ goto _LL1729;}
_LL1729: goto _LL1730; _LL1728: { struct Cyc_List_List* _temp1745=({ struct Cyc_List_List*
_temp1743=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1743->hd=( void*)({ struct _tuple7* _temp1744=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1744->f1= t1; _temp1744->f2= t2; _temp1744;});
_temp1743->tl= 0; _temp1743;}); goto _LL1746; _LL1746: { int _temp1747= Cyc_Tcutil_ptrsubtype(
te, _temp1745, _temp1709, _temp1741)? ! _temp1703.q_const? 1: _temp1735.q_const:
0; goto _LL1748; _LL1748: { int bounds_ok=(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1701, _temp1733); if( ! bounds_ok){ struct _tuple7
_temp1750=({ struct _tuple7 _temp1749; _temp1749.f1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1701); _temp1749.f2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1733); _temp1749;}); void* _temp1756; struct Cyc_Absyn_Exp*
_temp1758; void* _temp1760; struct Cyc_Absyn_Exp* _temp1762; _LL1752: _LL1761:
_temp1760= _temp1750.f1; if(( unsigned int) _temp1760 > 1u?*(( int*) _temp1760)
== Cyc_Absyn_Upper_b: 0){ _LL1763: _temp1762=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1760)->f1; goto _LL1757;} else{ goto _LL1754;} _LL1757: _temp1756=
_temp1750.f2; if(( unsigned int) _temp1756 > 1u?*(( int*) _temp1756) == Cyc_Absyn_Upper_b:
0){ _LL1759: _temp1758=(( struct Cyc_Absyn_Upper_b_struct*) _temp1756)->f1; goto
_LL1753;} else{ goto _LL1754;} _LL1754: goto _LL1755; _LL1753: if( Cyc_Evexp_eval_const_uint_exp(
_temp1762) >= Cyc_Evexp_eval_const_uint_exp( _temp1758)){ bounds_ok= 1;} goto
_LL1751; _LL1755: bounds_ok= 1; goto _LL1751; _LL1751:;} return( bounds_ok?
_temp1747: 0)? _temp1707 ==( void*) Cyc_Absyn_HeapRgn? 1: Cyc_Tcutil_unify(
_temp1707, _temp1739): 0;}}} _LL1730: goto _LL1726; _LL1726:;} return 0; _LL1688:{
void* _temp1764= t2; struct Cyc_Absyn_Exp* _temp1770; struct Cyc_Absyn_Tqual
_temp1772; void* _temp1774; _LL1766: if(( unsigned int) _temp1764 > 4u?*(( int*)
_temp1764) == Cyc_Absyn_ArrayType: 0){ _LL1775: _temp1774=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1764)->f1; goto _LL1773; _LL1773: _temp1772=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1764)->f2; goto _LL1771; _LL1771: _temp1770=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1764)->f3; goto _LL1767;} else{ goto _LL1768;} _LL1768: goto _LL1769;
_LL1767: { int okay; okay=( _temp1711 != 0? _temp1770 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1711)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1770)): 0; return( okay? Cyc_Tcutil_unify(
_temp1715, _temp1774): 0)? ! _temp1713.q_const? 1: _temp1772.q_const: 0;}
_LL1769: return 0; _LL1765:;} return 0; _LL1690: goto _LL1692; _LL1692: goto
_LL1694; _LL1694: goto _LL1696; _LL1696: return Cyc_Tcutil_coerceable( t2);
_LL1698: return 0; _LL1684:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp1776=
Cyc_Absyn_copy_exp( e); goto _LL1777; _LL1777:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Cast_e_struct* _temp1778=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1778[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp1779; _temp1779.tag= Cyc_Absyn_Cast_e; _temp1779.f1=( void*) t; _temp1779.f2=
_temp1776; _temp1779;}); _temp1778;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1780=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1780->v=( void*) t; _temp1780;});}} int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*
e){ void* _temp1781= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_Enumdecl* _temp1791; struct _tuple1*
_temp1793; void* _temp1795; void* _temp1797; int _temp1799; struct Cyc_Core_Opt*
_temp1801; void* _temp1803; _LL1783: if(( unsigned int) _temp1781 > 4u?*(( int*)
_temp1781) == Cyc_Absyn_EnumType: 0){ _LL1794: _temp1793=(( struct Cyc_Absyn_EnumType_struct*)
_temp1781)->f1; goto _LL1792; _LL1792: _temp1791=(( struct Cyc_Absyn_EnumType_struct*)
_temp1781)->f2; goto _LL1784;} else{ goto _LL1785;} _LL1785: if(( unsigned int)
_temp1781 > 4u?*(( int*) _temp1781) == Cyc_Absyn_IntType: 0){ _LL1798: _temp1797=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1781)->f1; goto _LL1796; _LL1796:
_temp1795=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1781)->f2; goto
_LL1786;} else{ goto _LL1787;} _LL1787: if(( unsigned int) _temp1781 > 4u?*((
int*) _temp1781) == Cyc_Absyn_Evar: 0){ _LL1804: _temp1803=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp1781)->f1; goto _LL1802; _LL1802: _temp1801=(( struct Cyc_Absyn_Evar_struct*)
_temp1781)->f2; goto _LL1800; _LL1800: _temp1799=(( struct Cyc_Absyn_Evar_struct*)
_temp1781)->f3; goto _LL1788;} else{ goto _LL1789;} _LL1789: goto _LL1790;
_LL1784: goto _LL1786; _LL1786: return 1; _LL1788: if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
return 0; _LL1790: return 0; _LL1782:;} int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*
e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void* _temp1805= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL1807: if(
_temp1805 ==( void*) Cyc_Absyn_FloatType){ goto _LL1808;} else{ goto _LL1809;}
_LL1809: if( _temp1805 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1810;} else{
goto _LL1811;} _LL1811: goto _LL1812; _LL1808: goto _LL1810; _LL1810: return 1;
_LL1812: return 0; _LL1806:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp1813= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp1819; _LL1815:
if(( unsigned int) _temp1813 > 4u?*(( int*) _temp1813) == Cyc_Absyn_FnType: 0){
_LL1820: _temp1819=(( struct Cyc_Absyn_FnType_struct*) _temp1813)->f1; goto
_LL1816;} else{ goto _LL1817;} _LL1817: goto _LL1818; _LL1816: return 1; _LL1818:
return 0; _LL1814:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){
struct _tuple7 _temp1822=({ struct _tuple7 _temp1821; _temp1821.f1= t1;
_temp1821.f2= t2; _temp1821;}); void* _temp1846; void* _temp1848; void*
_temp1850; void* _temp1852; void* _temp1854; void* _temp1856; void* _temp1858;
void* _temp1860; void* _temp1862; void* _temp1864; void* _temp1866; void*
_temp1868; void* _temp1870; void* _temp1872; void* _temp1874; void* _temp1876;
void* _temp1878; void* _temp1880; void* _temp1882; void* _temp1884; void*
_temp1886; void* _temp1888; void* _temp1890; void* _temp1892; void* _temp1894;
void* _temp1896; void* _temp1898; void* _temp1900; void* _temp1902; void*
_temp1904; void* _temp1906; void* _temp1908; _LL1824: _LL1849: _temp1848=
_temp1822.f1; if( _temp1848 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1847;}
else{ goto _LL1826;} _LL1847: _temp1846= _temp1822.f2; goto _LL1825; _LL1826:
_LL1853: _temp1852= _temp1822.f1; goto _LL1851; _LL1851: _temp1850= _temp1822.f2;
if( _temp1850 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1827;} else{ goto
_LL1828;} _LL1828: _LL1857: _temp1856= _temp1822.f1; if( _temp1856 ==( void*)
Cyc_Absyn_FloatType){ goto _LL1855;} else{ goto _LL1830;} _LL1855: _temp1854=
_temp1822.f2; goto _LL1829; _LL1830: _LL1861: _temp1860= _temp1822.f1; goto
_LL1859; _LL1859: _temp1858= _temp1822.f2; if( _temp1858 ==( void*) Cyc_Absyn_FloatType){
goto _LL1831;} else{ goto _LL1832;} _LL1832: _LL1865: _temp1864= _temp1822.f1;
if(( unsigned int) _temp1864 > 4u?*(( int*) _temp1864) == Cyc_Absyn_IntType: 0){
_LL1869: _temp1868=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1864)->f1;
if( _temp1868 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1867;} else{ goto _LL1834;}
_LL1867: _temp1866=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1864)->f2;
if( _temp1866 ==( void*) Cyc_Absyn_B8){ goto _LL1863;} else{ goto _LL1834;}}
else{ goto _LL1834;} _LL1863: _temp1862= _temp1822.f2; goto _LL1833; _LL1834:
_LL1877: _temp1876= _temp1822.f1; goto _LL1871; _LL1871: _temp1870= _temp1822.f2;
if(( unsigned int) _temp1870 > 4u?*(( int*) _temp1870) == Cyc_Absyn_IntType: 0){
_LL1875: _temp1874=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1870)->f1;
if( _temp1874 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1873;} else{ goto _LL1836;}
_LL1873: _temp1872=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1870)->f2;
if( _temp1872 ==( void*) Cyc_Absyn_B8){ goto _LL1835;} else{ goto _LL1836;}}
else{ goto _LL1836;} _LL1836: _LL1881: _temp1880= _temp1822.f1; if((
unsigned int) _temp1880 > 4u?*(( int*) _temp1880) == Cyc_Absyn_IntType: 0){
_LL1885: _temp1884=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1880)->f1;
if( _temp1884 ==( void*) Cyc_Absyn_Signed){ goto _LL1883;} else{ goto _LL1838;}
_LL1883: _temp1882=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1880)->f2;
if( _temp1882 ==( void*) Cyc_Absyn_B8){ goto _LL1879;} else{ goto _LL1838;}}
else{ goto _LL1838;} _LL1879: _temp1878= _temp1822.f2; goto _LL1837; _LL1838:
_LL1893: _temp1892= _temp1822.f1; goto _LL1887; _LL1887: _temp1886= _temp1822.f2;
if(( unsigned int) _temp1886 > 4u?*(( int*) _temp1886) == Cyc_Absyn_IntType: 0){
_LL1891: _temp1890=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1886)->f1;
if( _temp1890 ==( void*) Cyc_Absyn_Signed){ goto _LL1889;} else{ goto _LL1840;}
_LL1889: _temp1888=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1886)->f2;
if( _temp1888 ==( void*) Cyc_Absyn_B8){ goto _LL1839;} else{ goto _LL1840;}}
else{ goto _LL1840;} _LL1840: _LL1897: _temp1896= _temp1822.f1; if((
unsigned int) _temp1896 > 4u?*(( int*) _temp1896) == Cyc_Absyn_IntType: 0){
_LL1901: _temp1900=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1896)->f1;
if( _temp1900 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1899;} else{ goto _LL1842;}
_LL1899: _temp1898=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1896)->f2;
if( _temp1898 ==( void*) Cyc_Absyn_B4){ goto _LL1895;} else{ goto _LL1842;}}
else{ goto _LL1842;} _LL1895: _temp1894= _temp1822.f2; goto _LL1841; _LL1842:
_LL1909: _temp1908= _temp1822.f1; goto _LL1903; _LL1903: _temp1902= _temp1822.f2;
if(( unsigned int) _temp1902 > 4u?*(( int*) _temp1902) == Cyc_Absyn_IntType: 0){
_LL1907: _temp1906=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1902)->f1;
if( _temp1906 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1905;} else{ goto _LL1844;}
_LL1905: _temp1904=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1902)->f2;
if( _temp1904 ==( void*) Cyc_Absyn_B4){ goto _LL1843;} else{ goto _LL1844;}}
else{ goto _LL1844;} _LL1844: goto _LL1845; _LL1825: goto _LL1827; _LL1827:
return( void*) Cyc_Absyn_DoubleType; _LL1829: goto _LL1831; _LL1831: return(
void*) Cyc_Absyn_FloatType; _LL1833: goto _LL1835; _LL1835: return Cyc_Absyn_ulong_t;
_LL1837: goto _LL1839; _LL1839: return Cyc_Absyn_slong_t; _LL1841: goto _LL1843;
_LL1843: return Cyc_Absyn_uint_t; _LL1845: return Cyc_Absyn_sint_t; _LL1823:;}
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void* _temp1910=(
void*) e->r; struct Cyc_Absyn_Exp* _temp1916; struct Cyc_Core_Opt* _temp1918;
struct Cyc_Absyn_Exp* _temp1920; _LL1912: if(*(( int*) _temp1910) == Cyc_Absyn_AssignOp_e){
_LL1921: _temp1920=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1910)->f1; goto
_LL1919; _LL1919: _temp1918=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1910)->f2;
if( _temp1918 == 0){ goto _LL1917;} else{ goto _LL1914;} _LL1917: _temp1916=((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1910)->f3; goto _LL1913;} else{ goto
_LL1914;} _LL1914: goto _LL1915; _LL1913: Cyc_Tcutil_warn( e->loc,({
unsigned char* _temp1922="assignment in test"; struct _tagged_string _temp1923;
_temp1923.curr= _temp1922; _temp1923.base= _temp1922; _temp1923.last_plus_one=
_temp1922 + 19; _temp1923;})); goto _LL1911; _LL1915: goto _LL1911; _LL1911:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
c2); if( c1 == c2){ return 1;} if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1924=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1924[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1925; _temp1925.tag= Cyc_Absyn_Forward_constr; _temp1925.f1= c2; _temp1925;});
_temp1924;}))); return 1;} else{ if(( void*) c2->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1926=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1926[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1927; _temp1927.tag= Cyc_Absyn_Forward_constr; _temp1927.f1= c1; _temp1927;});
_temp1926;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1928=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1928[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1929; _temp1929.tag= Cyc_Absyn_Forward_constr;
_temp1929.f1= c1; _temp1929;}); _temp1928;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1930=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1930[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1931; _temp1931.tag= Cyc_Absyn_Forward_constr; _temp1931.f1= c2; _temp1931;});
_temp1930;}))); return 1;} else{ return 0;}}}}} static struct Cyc_List_List* Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tvs == 0){ return({ struct Cyc_List_List* _temp1932=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1932->hd=( void*) tv; _temp1932->tl=
0; _temp1932;});} else{{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0;
tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){ if( Cyc_String_zstrptrcmp(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->name,
tv->name) == 0){ struct Cyc_Absyn_Conref* k1=(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs2))->hd)->kind; struct Cyc_Absyn_Conref* k2= tv->kind;
if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1933=* tv->name; struct _tagged_string _temp1934= Cyc_Absynpp_ckind2string(
k1); struct _tagged_string _temp1935= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp1933.last_plus_one - _temp1933.curr, _temp1933.curr, _temp1934.last_plus_one
- _temp1934.curr, _temp1934.curr, _temp1935.last_plus_one - _temp1935.curr,
_temp1935.curr);}));} return tvs;} else{ if((( struct Cyc_List_List*)
_check_null( tvs2))->tl == 0){(( struct Cyc_List_List*) _check_null( tvs2))->tl=({
struct Cyc_List_List* _temp1936=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1936->hd=( void*) tv; _temp1936->tl= 0; _temp1936;});
return tvs;}}}} return(( struct Cyc_List_List*(*)( struct _tagged_string msg))
Cyc_Tcutil_impos)(({ unsigned char* _temp1937="Tcutil::add_free_tvar:expecting to have returned.";
struct _tagged_string _temp1938; _temp1938.curr= _temp1937; _temp1938.base=
_temp1937; _temp1938.last_plus_one= _temp1937 + 50; _temp1938;}));}} static
struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs,
struct Cyc_List_List* btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl){ int present= 0;{ struct Cyc_List_List*
b= btvs; for( 0; b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){ if(
Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->name,(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name) == 0){ present= 1; break;}}} if( ! present){ r=({
struct Cyc_List_List* _temp1939=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1939->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd); _temp1939->tl= r; _temp1939;});}}
r=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ struct _tagged_string s= xprintf("#%d", Cyc_Tcutil_tvar_counter ++);
return({ struct Cyc_Absyn_Tvar* _temp1940=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp1940->name=({ struct _tagged_string*
_temp1941=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1941[ 0]= s; _temp1941;}); _temp1940->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)( k); _temp1940;});} int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar* t){ struct _tagged_string _temp1942=* t->name; goto
_LL1943; _LL1943: return*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp1942, sizeof(
unsigned char), 0) =='#';} void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*
t){ if( ! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_string
_temp1946= Cyc_String_strconcat(({ unsigned char* _temp1944="`"; struct
_tagged_string _temp1945; _temp1945.curr= _temp1944; _temp1945.base= _temp1944;
_temp1945.last_plus_one= _temp1944 + 2; _temp1945;}),* t->name); goto _LL1947;
_LL1947:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp1946, sizeof( unsigned char), 1)='t'; t->name=({
struct _tagged_string* _temp1948=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp1948[ 0]= _temp1946; _temp1948;});}} struct
_tuple12{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple12* x){ return({
struct _tuple2* _temp1949=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp1949->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1950=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1950->v=( void*)(*
x).f1; _temp1950;}); _temp1949->f2=(* x).f2; _temp1949->f3=(* x).f3; _temp1949;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ ==
0){ struct Cyc_List_List* _temp1951= 0; goto _LL1952; _LL1952:{ struct Cyc_List_List*
atts= fd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ if( Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd)){ _temp1951=({ struct Cyc_List_List* _temp1953=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1953->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); _temp1953->tl=
_temp1951; _temp1953;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1954=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1954[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1955; _temp1955.tag= Cyc_Absyn_FnType;
_temp1955.f1=({ struct Cyc_Absyn_FnInfo _temp1956; _temp1956.tvars= fd->tvs;
_temp1956.effect= fd->effect; _temp1956.ret_typ=( void*)(( void*) fd->ret_type);
_temp1956.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple12*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp1956.c_varargs= fd->c_varargs; _temp1956.cyc_varargs= fd->cyc_varargs;
_temp1956.attributes= _temp1951; _temp1956;}); _temp1955;}); _temp1954;});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct
_tuple13{ void* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple13* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp1957=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1957->f1=(* pr).f1; _temp1957->f2= t; _temp1957;});}
void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp1958= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp1964; struct Cyc_List_List* _temp1966; struct
Cyc_Absyn_VarargInfo* _temp1968; int _temp1970; struct Cyc_List_List* _temp1972;
void* _temp1974; struct Cyc_Core_Opt* _temp1976; struct Cyc_List_List* _temp1978;
_LL1960: if(( unsigned int) _temp1958 > 4u?*(( int*) _temp1958) == Cyc_Absyn_FnType:
0){ _LL1965: _temp1964=(( struct Cyc_Absyn_FnType_struct*) _temp1958)->f1;
_LL1979: _temp1978= _temp1964.tvars; goto _LL1977; _LL1977: _temp1976= _temp1964.effect;
goto _LL1975; _LL1975: _temp1974=( void*) _temp1964.ret_typ; goto _LL1973;
_LL1973: _temp1972= _temp1964.args; goto _LL1971; _LL1971: _temp1970= _temp1964.c_varargs;
goto _LL1969; _LL1969: _temp1968= _temp1964.cyc_varargs; goto _LL1967; _LL1967:
_temp1966= _temp1964.attributes; goto _LL1961;} else{ goto _LL1962;} _LL1962:
goto _LL1963; _LL1961: fd->tvs= _temp1978; fd->effect= _temp1976; goto _LL1959;
_LL1963:(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp1980="check_fndecl_valid_type: not a FnType"; struct
_tagged_string _temp1981; _temp1981.curr= _temp1980; _temp1981.base= _temp1980;
_temp1981.last_plus_one= _temp1980 + 38; _temp1981;})); return; _LL1959:;}{
struct _RegionHandle _temp1982= _new_region(); struct _RegionHandle* r=&
_temp1982; _push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_string*(* f)( struct _tuple12*), struct
Cyc_List_List* x)) Cyc_List_rmap)( r,( struct _tagged_string*(*)( struct
_tuple12* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,({ unsigned char* _temp1983="function declaration has repeated parameter";
struct _tagged_string _temp1984; _temp1984.curr= _temp1983; _temp1984.base=
_temp1983; _temp1984.last_plus_one= _temp1983 + 44; _temp1984;}));; _pop_region(&
_temp1982);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp1985=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1985->v=( void*) t; _temp1985;});}
struct _tuple14{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple15{ struct _tuple14* f1; void* f2; } ; static struct _tuple15* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple15*
_temp1986=( struct _tuple15*) _region_malloc( rgn, sizeof( struct _tuple15));
_temp1986->f1=({ struct _tuple14* _temp1987=( struct _tuple14*) _region_malloc(
rgn, sizeof( struct _tuple14)); _temp1987->f1=(* y).f1; _temp1987->f2=(* y).f2;
_temp1987;}); _temp1986->f2=(* y).f3; _temp1986;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple15* w){ void* _temp1990; struct _tuple14* _temp1992; struct
_tuple15 _temp1988=* w; _LL1993: _temp1992= _temp1988.f1; goto _LL1991; _LL1991:
_temp1990= _temp1988.f2; goto _LL1989; _LL1989: { struct Cyc_Absyn_Tqual
_temp1996; struct Cyc_Core_Opt* _temp1998; struct _tuple14 _temp1994=* _temp1992;
_LL1999: _temp1998= _temp1994.f1; goto _LL1997; _LL1997: _temp1996= _temp1994.f2;
goto _LL1995; _LL1995: return({ struct _tuple2* _temp2000=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp2000->f1= _temp1998; _temp2000->f2=
_temp1996; _temp2000->f3= _temp1990; _temp2000;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2001=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2001->name= f->name;
_temp2001->tq= f->tq; _temp2001->type=( void*) t; _temp2001->width= f->width;
_temp2001->attributes= f->attributes; _temp2001;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2002= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2048; struct Cyc_Absyn_Structdecl** _temp2050;
struct Cyc_List_List* _temp2052; struct _tuple1* _temp2054; struct Cyc_Absyn_Uniondecl**
_temp2056; struct Cyc_List_List* _temp2058; struct _tuple1* _temp2060; struct
Cyc_Absyn_TunionInfo _temp2062; void* _temp2064; struct Cyc_List_List* _temp2066;
void* _temp2068; struct Cyc_Absyn_TunionFieldInfo _temp2070; struct Cyc_List_List*
_temp2072; void* _temp2074; struct Cyc_Core_Opt* _temp2076; struct Cyc_List_List*
_temp2078; struct _tuple1* _temp2080; struct Cyc_Absyn_Exp* _temp2082; struct
Cyc_Absyn_Tqual _temp2084; void* _temp2086; struct Cyc_Absyn_PtrInfo _temp2088;
struct Cyc_Absyn_Conref* _temp2090; struct Cyc_Absyn_Tqual _temp2092; struct Cyc_Absyn_Conref*
_temp2094; void* _temp2096; void* _temp2098; struct Cyc_Absyn_FnInfo _temp2100;
struct Cyc_List_List* _temp2102; struct Cyc_Absyn_VarargInfo* _temp2104; int
_temp2106; struct Cyc_List_List* _temp2108; void* _temp2110; struct Cyc_Core_Opt*
_temp2112; struct Cyc_List_List* _temp2114; struct Cyc_List_List* _temp2116;
struct Cyc_List_List* _temp2118; struct Cyc_List_List* _temp2120; int _temp2122;
struct Cyc_Core_Opt* _temp2124; void* _temp2126; void* _temp2128; struct Cyc_Absyn_Enumdecl*
_temp2130; struct _tuple1* _temp2132; void* _temp2134; void* _temp2136; void*
_temp2138; struct Cyc_List_List* _temp2140; _LL2004: if(( unsigned int)
_temp2002 > 4u?*(( int*) _temp2002) == Cyc_Absyn_VarType: 0){ _LL2049: _temp2048=((
struct Cyc_Absyn_VarType_struct*) _temp2002)->f1; goto _LL2005;} else{ goto
_LL2006;} _LL2006: if(( unsigned int) _temp2002 > 4u?*(( int*) _temp2002) == Cyc_Absyn_StructType:
0){ _LL2055: _temp2054=(( struct Cyc_Absyn_StructType_struct*) _temp2002)->f1;
goto _LL2053; _LL2053: _temp2052=(( struct Cyc_Absyn_StructType_struct*)
_temp2002)->f2; goto _LL2051; _LL2051: _temp2050=(( struct Cyc_Absyn_StructType_struct*)
_temp2002)->f3; goto _LL2007;} else{ goto _LL2008;} _LL2008: if(( unsigned int)
_temp2002 > 4u?*(( int*) _temp2002) == Cyc_Absyn_UnionType: 0){ _LL2061:
_temp2060=(( struct Cyc_Absyn_UnionType_struct*) _temp2002)->f1; goto _LL2059;
_LL2059: _temp2058=(( struct Cyc_Absyn_UnionType_struct*) _temp2002)->f2; goto
_LL2057; _LL2057: _temp2056=(( struct Cyc_Absyn_UnionType_struct*) _temp2002)->f3;
goto _LL2009;} else{ goto _LL2010;} _LL2010: if(( unsigned int) _temp2002 > 4u?*((
int*) _temp2002) == Cyc_Absyn_TunionType: 0){ _LL2063: _temp2062=(( struct Cyc_Absyn_TunionType_struct*)
_temp2002)->f1; _LL2069: _temp2068=( void*) _temp2062.tunion_info; goto _LL2067;
_LL2067: _temp2066= _temp2062.targs; goto _LL2065; _LL2065: _temp2064=( void*)
_temp2062.rgn; goto _LL2011;} else{ goto _LL2012;} _LL2012: if(( unsigned int)
_temp2002 > 4u?*(( int*) _temp2002) == Cyc_Absyn_TunionFieldType: 0){ _LL2071:
_temp2070=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2002)->f1; _LL2075:
_temp2074=( void*) _temp2070.field_info; goto _LL2073; _LL2073: _temp2072=
_temp2070.targs; goto _LL2013;} else{ goto _LL2014;} _LL2014: if(( unsigned int)
_temp2002 > 4u?*(( int*) _temp2002) == Cyc_Absyn_TypedefType: 0){ _LL2081:
_temp2080=(( struct Cyc_Absyn_TypedefType_struct*) _temp2002)->f1; goto _LL2079;
_LL2079: _temp2078=(( struct Cyc_Absyn_TypedefType_struct*) _temp2002)->f2; goto
_LL2077; _LL2077: _temp2076=(( struct Cyc_Absyn_TypedefType_struct*) _temp2002)->f3;
goto _LL2015;} else{ goto _LL2016;} _LL2016: if(( unsigned int) _temp2002 > 4u?*((
int*) _temp2002) == Cyc_Absyn_ArrayType: 0){ _LL2087: _temp2086=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2002)->f1; goto _LL2085; _LL2085:
_temp2084=(( struct Cyc_Absyn_ArrayType_struct*) _temp2002)->f2; goto _LL2083;
_LL2083: _temp2082=(( struct Cyc_Absyn_ArrayType_struct*) _temp2002)->f3; goto
_LL2017;} else{ goto _LL2018;} _LL2018: if(( unsigned int) _temp2002 > 4u?*((
int*) _temp2002) == Cyc_Absyn_PointerType: 0){ _LL2089: _temp2088=(( struct Cyc_Absyn_PointerType_struct*)
_temp2002)->f1; _LL2099: _temp2098=( void*) _temp2088.elt_typ; goto _LL2097;
_LL2097: _temp2096=( void*) _temp2088.rgn_typ; goto _LL2095; _LL2095: _temp2094=
_temp2088.nullable; goto _LL2093; _LL2093: _temp2092= _temp2088.tq; goto _LL2091;
_LL2091: _temp2090= _temp2088.bounds; goto _LL2019;} else{ goto _LL2020;}
_LL2020: if(( unsigned int) _temp2002 > 4u?*(( int*) _temp2002) == Cyc_Absyn_FnType:
0){ _LL2101: _temp2100=(( struct Cyc_Absyn_FnType_struct*) _temp2002)->f1;
_LL2115: _temp2114= _temp2100.tvars; goto _LL2113; _LL2113: _temp2112= _temp2100.effect;
goto _LL2111; _LL2111: _temp2110=( void*) _temp2100.ret_typ; goto _LL2109;
_LL2109: _temp2108= _temp2100.args; goto _LL2107; _LL2107: _temp2106= _temp2100.c_varargs;
goto _LL2105; _LL2105: _temp2104= _temp2100.cyc_varargs; goto _LL2103; _LL2103:
_temp2102= _temp2100.attributes; goto _LL2021;} else{ goto _LL2022;} _LL2022:
if(( unsigned int) _temp2002 > 4u?*(( int*) _temp2002) == Cyc_Absyn_TupleType: 0){
_LL2117: _temp2116=(( struct Cyc_Absyn_TupleType_struct*) _temp2002)->f1; goto
_LL2023;} else{ goto _LL2024;} _LL2024: if(( unsigned int) _temp2002 > 4u?*((
int*) _temp2002) == Cyc_Absyn_AnonStructType: 0){ _LL2119: _temp2118=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2002)->f1; goto _LL2025;} else{ goto
_LL2026;} _LL2026: if(( unsigned int) _temp2002 > 4u?*(( int*) _temp2002) == Cyc_Absyn_AnonUnionType:
0){ _LL2121: _temp2120=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2002)->f1;
goto _LL2027;} else{ goto _LL2028;} _LL2028: if(( unsigned int) _temp2002 > 4u?*((
int*) _temp2002) == Cyc_Absyn_Evar: 0){ _LL2127: _temp2126=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2002)->f1; goto _LL2125; _LL2125: _temp2124=(( struct Cyc_Absyn_Evar_struct*)
_temp2002)->f2; goto _LL2123; _LL2123: _temp2122=(( struct Cyc_Absyn_Evar_struct*)
_temp2002)->f3; goto _LL2029;} else{ goto _LL2030;} _LL2030: if(( unsigned int)
_temp2002 > 4u?*(( int*) _temp2002) == Cyc_Absyn_RgnHandleType: 0){ _LL2129:
_temp2128=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2002)->f1;
goto _LL2031;} else{ goto _LL2032;} _LL2032: if(( unsigned int) _temp2002 > 4u?*((
int*) _temp2002) == Cyc_Absyn_EnumType: 0){ _LL2133: _temp2132=(( struct Cyc_Absyn_EnumType_struct*)
_temp2002)->f1; goto _LL2131; _LL2131: _temp2130=(( struct Cyc_Absyn_EnumType_struct*)
_temp2002)->f2; goto _LL2033;} else{ goto _LL2034;} _LL2034: if( _temp2002 ==(
void*) Cyc_Absyn_VoidType){ goto _LL2035;} else{ goto _LL2036;} _LL2036: if((
unsigned int) _temp2002 > 4u?*(( int*) _temp2002) == Cyc_Absyn_IntType: 0){
_LL2137: _temp2136=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2002)->f1;
goto _LL2135; _LL2135: _temp2134=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2002)->f2; goto _LL2037;} else{ goto _LL2038;} _LL2038: if( _temp2002 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2039;} else{ goto _LL2040;} _LL2040: if(
_temp2002 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2041;} else{ goto _LL2042;}
_LL2042: if( _temp2002 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2043;} else{ goto
_LL2044;} _LL2044: if(( unsigned int) _temp2002 > 4u?*(( int*) _temp2002) == Cyc_Absyn_AccessEff:
0){ _LL2139: _temp2138=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2002)->f1;
goto _LL2045;} else{ goto _LL2046;} _LL2046: if(( unsigned int) _temp2002 > 4u?*((
int*) _temp2002) == Cyc_Absyn_JoinEff: 0){ _LL2141: _temp2140=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2002)->f1; goto _LL2047;} else{ goto _LL2003;} _LL2005: { struct
_handler_cons _temp2142; _push_handler(& _temp2142);{ int _temp2144= 0; if(
setjmp( _temp2142.handler)){ _temp2144= 1;} if( ! _temp2144){{ void* _temp2145=((
void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2048); _npop_handler( 0u); return _temp2145;}; _pop_handler();} else{
void* _temp2143=( void*) _exn_thrown; void* _temp2147= _temp2143; _LL2149: if(
_temp2147 == Cyc_Core_Not_found){ goto _LL2150;} else{ goto _LL2151;} _LL2151:
goto _LL2152; _LL2150: return t; _LL2152:( void) _throw( _temp2147); _LL2148:;}}}
_LL2007: { struct Cyc_List_List* _temp2153= Cyc_Tcutil_substs( rgn, inst,
_temp2052); goto _LL2154; _LL2154: return _temp2153 == _temp2052? t:( void*)({
struct Cyc_Absyn_StructType_struct* _temp2155=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2155[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2156; _temp2156.tag= Cyc_Absyn_StructType;
_temp2156.f1= _temp2054; _temp2156.f2= _temp2153; _temp2156.f3= _temp2050;
_temp2156;}); _temp2155;});} _LL2009: { struct Cyc_List_List* _temp2157= Cyc_Tcutil_substs(
rgn, inst, _temp2058); goto _LL2158; _LL2158: return _temp2157 == _temp2058? t:(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp2159=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2159[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2160; _temp2160.tag= Cyc_Absyn_UnionType;
_temp2160.f1= _temp2060; _temp2160.f2= _temp2157; _temp2160.f3= _temp2056;
_temp2160;}); _temp2159;});} _LL2011: { struct Cyc_List_List* _temp2161= Cyc_Tcutil_substs(
rgn, inst, _temp2066); goto _LL2162; _LL2162: { void* _temp2163= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2064); goto _LL2164; _LL2164: return( _temp2161 == _temp2066?
_temp2163 == _temp2064: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2165=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2165[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2166; _temp2166.tag=
Cyc_Absyn_TunionType; _temp2166.f1=({ struct Cyc_Absyn_TunionInfo _temp2167;
_temp2167.tunion_info=( void*) _temp2068; _temp2167.targs= _temp2161; _temp2167.rgn=(
void*) _temp2163; _temp2167;}); _temp2166;}); _temp2165;});}} _LL2013: { struct
Cyc_List_List* _temp2168= Cyc_Tcutil_substs( rgn, inst, _temp2072); goto _LL2169;
_LL2169: return _temp2168 == _temp2072? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2170=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2170[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2171; _temp2171.tag= Cyc_Absyn_TunionFieldType; _temp2171.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2172; _temp2172.field_info=( void*) _temp2074; _temp2172.targs= _temp2168;
_temp2172;}); _temp2171;}); _temp2170;});} _LL2015: { struct Cyc_List_List*
_temp2173= Cyc_Tcutil_substs( rgn, inst, _temp2078); goto _LL2174; _LL2174:
return _temp2173 == _temp2078? t:( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp2175=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2175[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2176; _temp2176.tag=
Cyc_Absyn_TypedefType; _temp2176.f1= _temp2080; _temp2176.f2= _temp2173;
_temp2176.f3= _temp2076; _temp2176;}); _temp2175;});} _LL2017: { void* _temp2177=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2086); goto _LL2178; _LL2178: return
_temp2177 == _temp2086? t:( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp2179=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2179[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2180; _temp2180.tag= Cyc_Absyn_ArrayType;
_temp2180.f1=( void*) _temp2177; _temp2180.f2= _temp2084; _temp2180.f3=
_temp2082; _temp2180;}); _temp2179;});} _LL2019: { void* _temp2181= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2098); goto _LL2182; _LL2182: { void* _temp2183= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2096); goto _LL2184; _LL2184: if( _temp2181 == _temp2098?
_temp2183 == _temp2096: 0){ return t;} return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp2185=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp2185[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp2186; _temp2186.tag=
Cyc_Absyn_PointerType; _temp2186.f1=({ struct Cyc_Absyn_PtrInfo _temp2187;
_temp2187.elt_typ=( void*) _temp2181; _temp2187.rgn_typ=( void*) _temp2183;
_temp2187.nullable= _temp2094; _temp2187.tq= _temp2092; _temp2187.bounds=
_temp2090; _temp2187;}); _temp2186;}); _temp2185;});}} _LL2021:{ struct Cyc_List_List*
_temp2188= _temp2114; goto _LL2189; _LL2189: for( 0; _temp2188 != 0; _temp2188=((
struct Cyc_List_List*) _check_null( _temp2188))->tl){ inst=({ struct Cyc_List_List*
_temp2190=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2190->hd=( void*)({ struct _tuple5* _temp2191=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp2191->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp2188))->hd; _temp2191->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp2192=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2192[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2193; _temp2193.tag= Cyc_Absyn_VarType; _temp2193.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp2188))->hd;
_temp2193;}); _temp2192;}); _temp2191;}); _temp2190->tl= inst; _temp2190;});}}{
struct Cyc_List_List* _temp2196; struct Cyc_List_List* _temp2198; struct _tuple0
_temp2194=(( struct _tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple15*(* f)( struct _RegionHandle*, struct
_tuple2*), struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2108)); _LL2199: _temp2198= _temp2194.f1;
goto _LL2197; _LL2197: _temp2196= _temp2194.f2; goto _LL2195; _LL2195: { struct
Cyc_List_List* _temp2200= Cyc_Tcutil_substs( rgn, inst, _temp2196); goto _LL2201;
_LL2201: { struct Cyc_List_List* _temp2202=(( struct Cyc_List_List*(*)( struct
_tuple2*(* f)( struct _tuple15*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp2198, _temp2200)); goto _LL2203; _LL2203: { struct Cyc_Core_Opt* eff2; if(
_temp2112 == 0){ eff2= 0;} else{ void* _temp2204= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2112))->v); goto _LL2205;
_LL2205: if( _temp2204 ==( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2112))->v){
eff2= _temp2112;} else{ eff2=({ struct Cyc_Core_Opt* _temp2206=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2206->v=( void*) _temp2204;
_temp2206;});}}{ struct Cyc_Absyn_VarargInfo* cyc_varargs2; if( _temp2104 == 0){
cyc_varargs2= 0;} else{ int _temp2209; void* _temp2211; void* _temp2213; struct
Cyc_Absyn_Tqual _temp2215; struct Cyc_Core_Opt* _temp2217; struct Cyc_Absyn_VarargInfo
_temp2207=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2104)); _LL2218:
_temp2217= _temp2207.name; goto _LL2216; _LL2216: _temp2215= _temp2207.tq; goto
_LL2214; _LL2214: _temp2213=( void*) _temp2207.type; goto _LL2212; _LL2212:
_temp2211=( void*) _temp2207.rgn; goto _LL2210; _LL2210: _temp2209= _temp2207.inject;
goto _LL2208; _LL2208: { void* _temp2219= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2213); goto _LL2220; _LL2220: { void* _temp2221= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2211); goto _LL2222; _LL2222: if( _temp2219 == _temp2213?
_temp2211 == _temp2221: 0){ cyc_varargs2= _temp2104;} else{ cyc_varargs2=({
struct Cyc_Absyn_VarargInfo* _temp2223=( struct Cyc_Absyn_VarargInfo*) GC_malloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp2223->name= _temp2217; _temp2223->tq=
_temp2215; _temp2223->type=( void*) _temp2219; _temp2223->rgn=( void*) _temp2221;
_temp2223->inject= _temp2209; _temp2223;});}}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2224=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2224[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2225; _temp2225.tag= Cyc_Absyn_FnType;
_temp2225.f1=({ struct Cyc_Absyn_FnInfo _temp2226; _temp2226.tvars= _temp2114;
_temp2226.effect= eff2; _temp2226.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2110); _temp2226.args= _temp2202; _temp2226.c_varargs= _temp2106;
_temp2226.cyc_varargs= cyc_varargs2; _temp2226.attributes= _temp2102; _temp2226;});
_temp2225;}); _temp2224;});}}}}} _LL2023: { struct Cyc_List_List* _temp2227=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt, _temp2116);
goto _LL2228; _LL2228: { struct Cyc_List_List* _temp2229= Cyc_Tcutil_substs( rgn,
inst, _temp2227); goto _LL2230; _LL2230: if( _temp2229 == _temp2227){ return t;}{
struct Cyc_List_List* _temp2231=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct _tuple4*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)(
Cyc_Tcutil_map2_tq, _temp2116, _temp2229); goto _LL2232; _LL2232: return( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2233=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2233[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2234; _temp2234.tag= Cyc_Absyn_TupleType;
_temp2234.f1= _temp2231; _temp2234;}); _temp2233;});}}} _LL2025: { struct Cyc_List_List*
_temp2235=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2118); goto _LL2236; _LL2236: { struct Cyc_List_List* _temp2237= Cyc_Tcutil_substs(
rgn, inst, _temp2235); goto _LL2238; _LL2238: if( _temp2237 == _temp2235){
return t;}{ struct Cyc_List_List* _temp2239=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type,
_temp2118, _temp2237); goto _LL2240; _LL2240: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2241=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2241[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2242; _temp2242.tag= Cyc_Absyn_AnonStructType; _temp2242.f1= _temp2239;
_temp2242;}); _temp2241;});}}} _LL2027: { struct Cyc_List_List* _temp2243=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type, _temp2120);
goto _LL2244; _LL2244: { struct Cyc_List_List* _temp2245= Cyc_Tcutil_substs( rgn,
inst, _temp2243); goto _LL2246; _LL2246: if( _temp2245 == _temp2243){ return t;}{
struct Cyc_List_List* _temp2247=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2120, _temp2245); goto
_LL2248; _LL2248: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2249=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2249[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2250; _temp2250.tag= Cyc_Absyn_AnonStructType; _temp2250.f1= _temp2247;
_temp2250;}); _temp2249;});}}} _LL2029: if( _temp2124 != 0){ return Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2124))->v);} else{
return t;} _LL2031: { void* _temp2251= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2128); goto _LL2252; _LL2252: return _temp2251 == _temp2128? t:( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2253=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2253[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2254; _temp2254.tag= Cyc_Absyn_RgnHandleType;
_temp2254.f1=( void*) _temp2251; _temp2254;}); _temp2253;});} _LL2033: return t;
_LL2035: return t; _LL2037: return t; _LL2039: return t; _LL2041: return t;
_LL2043: return t; _LL2045: { void* _temp2255= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2138); goto _LL2256; _LL2256: return _temp2255 == _temp2138? t:( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2257=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2257[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2258; _temp2258.tag= Cyc_Absyn_AccessEff;
_temp2258.f1=( void*) _temp2255; _temp2258;}); _temp2257;});} _LL2047: { struct
Cyc_List_List* _temp2259= Cyc_Tcutil_substs( rgn, inst, _temp2140); goto _LL2260;
_LL2260: return _temp2259 == _temp2140? t:( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2261=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2261[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2262; _temp2262.tag= Cyc_Absyn_JoinEff;
_temp2262.f1= _temp2259; _temp2262;}); _temp2261;});} _LL2003:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2263=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; goto _LL2264; _LL2264: {
struct Cyc_List_List* _temp2265=(( struct Cyc_List_List*) _check_null( ts))->tl;
goto _LL2266; _LL2266: { void* _temp2267= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2263); goto _LL2268; _LL2268: { struct Cyc_List_List* _temp2269= Cyc_Tcutil_substs(
rgn, inst, _temp2265); goto _LL2270; _LL2270: if( _temp2263 == _temp2267?
_temp2265 == _temp2269: 0){ return ts;} return( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp2271=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2271->hd=( void*) _temp2267; _temp2271->tl= _temp2269; _temp2271;});}}}}}
extern void* Cyc_Tcutil_substitute( struct Cyc_List_List* inst, void* t){ return
Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region, inst, t);} struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_Position_Segment*
loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2321=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2321[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2322; _temp2322.tag= Cyc_Absyn_Const_e;
_temp2322.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp2322;}); _temp2321;}),
loc);{ void* _temp2272= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp2286; void* _temp2288; void* _temp2290; void* _temp2292; void* _temp2294;
_LL2274: if(( unsigned int) _temp2272 > 4u?*(( int*) _temp2272) == Cyc_Absyn_PointerType:
0){ _LL2287: _temp2286=(( struct Cyc_Absyn_PointerType_struct*) _temp2272)->f1;
goto _LL2275;} else{ goto _LL2276;} _LL2276: if(( unsigned int) _temp2272 > 4u?*((
int*) _temp2272) == Cyc_Absyn_IntType: 0){ _LL2291: _temp2290=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2272)->f1; goto _LL2289; _LL2289: _temp2288=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2272)->f2; if( _temp2288 ==(
void*) Cyc_Absyn_B1){ goto _LL2277;} else{ goto _LL2278;}} else{ goto _LL2278;}
_LL2278: if(( unsigned int) _temp2272 > 4u?*(( int*) _temp2272) == Cyc_Absyn_IntType:
0){ _LL2295: _temp2294=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2272)->f1;
goto _LL2293; _LL2293: _temp2292=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2272)->f2; goto _LL2279;} else{ goto _LL2280;} _LL2280: if( _temp2272 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2281;} else{ goto _LL2282;} _LL2282: if(
_temp2272 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2283;} else{ goto _LL2284;}
_LL2284: goto _LL2285; _LL2275: goto _LL2273; _LL2277:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2296=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2296[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2297; _temp2297.tag= Cyc_Absyn_Const_e; _temp2297.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2298=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp2298[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp2299; _temp2299.tag= Cyc_Absyn_Char_c; _temp2299.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp2299.f2='\000'; _temp2299;}); _temp2298;})); _temp2297;}); _temp2296;})));
goto _LL2273; _LL2279:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2300=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2300[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2301; _temp2301.tag= Cyc_Absyn_Const_e;
_temp2301.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2302=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2302[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2303; _temp2303.tag= Cyc_Absyn_Int_c;
_temp2303.f1=( void*) _temp2294; _temp2303.f2= 0; _temp2303;}); _temp2302;}));
_temp2301;}); _temp2300;}))); if( _temp2292 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2304=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2304[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2305; _temp2305.tag= Cyc_Absyn_Cast_e; _temp2305.f1=( void*) t; _temp2305.f2=
e; _temp2305;}); _temp2304;}), loc);} goto _LL2273; _LL2281:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2306=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2306[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2307; _temp2307.tag= Cyc_Absyn_Const_e; _temp2307.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2308=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp2308[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp2309; _temp2309.tag= Cyc_Absyn_Float_c; _temp2309.f1=({
unsigned char* _temp2310="0.0"; struct _tagged_string _temp2311; _temp2311.curr=
_temp2310; _temp2311.base= _temp2310; _temp2311.last_plus_one= _temp2310 + 4;
_temp2311;}); _temp2309;}); _temp2308;})); _temp2307;}); _temp2306;}))); goto
_LL2273; _LL2283:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2312=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2312[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2313; _temp2313.tag= Cyc_Absyn_Cast_e;
_temp2313.f1=( void*) t; _temp2313.f2= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2314=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2314[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2315; _temp2315.tag= Cyc_Absyn_Const_e;
_temp2315.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2316=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2316[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2317; _temp2317.tag= Cyc_Absyn_Float_c;
_temp2317.f1=({ unsigned char* _temp2318="0.0"; struct _tagged_string _temp2319;
_temp2319.curr= _temp2318; _temp2319.base= _temp2318; _temp2319.last_plus_one=
_temp2318 + 4; _temp2319;}); _temp2317;}); _temp2316;})); _temp2315;});
_temp2314;}), loc); _temp2313;}); _temp2312;}))); goto _LL2273; _LL2285: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2320= Cyc_Absynpp_typ2string( t); xprintf("declaration of type %.*s requires initializer",
_temp2320.last_plus_one - _temp2320.curr, _temp2320.curr);})); goto _LL2273;
_LL2273:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*
tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind);
return({ struct _tuple5* _temp2323=( struct _tuple5*) GC_malloc( sizeof( struct
_tuple5)); _temp2323->f1= tv; _temp2323->f2= Cyc_Absyn_new_evar( k); _temp2323;});}
struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _RegionHandle* rgn, struct
Cyc_Absyn_Tvar* tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
tv->kind); return({ struct _tuple5* _temp2324=( struct _tuple5*) _region_malloc(
rgn, sizeof( struct _tuple5)); _temp2324->f1= tv; _temp2324->f2= Cyc_Absyn_new_evar(
k); _temp2324;});} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* free_vars, void*
expected_kind, void* t){{ void* _temp2325= Cyc_Tcutil_compress( t); int
_temp2371; struct Cyc_Core_Opt* _temp2373; void* _temp2375; struct Cyc_Absyn_Tvar*
_temp2377; struct Cyc_Absyn_Enumdecl* _temp2379; struct Cyc_Absyn_Enumdecl**
_temp2381; struct _tuple1* _temp2382; struct Cyc_Absyn_TunionInfo _temp2384;
void* _temp2386; struct Cyc_List_List* _temp2388; void* _temp2390; void**
_temp2392; struct Cyc_Absyn_TunionFieldInfo _temp2393; struct Cyc_List_List*
_temp2395; void* _temp2397; void** _temp2399; struct Cyc_Absyn_PtrInfo _temp2400;
struct Cyc_Absyn_Conref* _temp2402; struct Cyc_Absyn_Tqual _temp2404; struct Cyc_Absyn_Conref*
_temp2406; void* _temp2408; void* _temp2410; void* _temp2412; void* _temp2414;
struct Cyc_Absyn_Exp* _temp2416; struct Cyc_Absyn_Tqual _temp2418; void*
_temp2420; struct Cyc_Absyn_FnInfo _temp2422; struct Cyc_List_List* _temp2424;
struct Cyc_Absyn_VarargInfo* _temp2426; int _temp2428; struct Cyc_List_List*
_temp2430; void* _temp2432; struct Cyc_Core_Opt* _temp2434; struct Cyc_Core_Opt**
_temp2436; struct Cyc_List_List* _temp2437; struct Cyc_List_List** _temp2439;
struct Cyc_List_List* _temp2440; struct Cyc_List_List* _temp2442; struct Cyc_List_List*
_temp2444; struct Cyc_Absyn_Structdecl** _temp2446; struct Cyc_Absyn_Structdecl***
_temp2448; struct Cyc_List_List* _temp2449; struct _tuple1* _temp2451; struct
Cyc_Absyn_Uniondecl** _temp2453; struct Cyc_Absyn_Uniondecl*** _temp2455; struct
Cyc_List_List* _temp2456; struct _tuple1* _temp2458; struct Cyc_Core_Opt*
_temp2460; struct Cyc_Core_Opt** _temp2462; struct Cyc_List_List* _temp2463;
struct _tuple1* _temp2465; void* _temp2467; void* _temp2469; struct Cyc_List_List*
_temp2471; _LL2327: if( _temp2325 ==( void*) Cyc_Absyn_VoidType){ goto _LL2328;}
else{ goto _LL2329;} _LL2329: if(( unsigned int) _temp2325 > 4u?*(( int*)
_temp2325) == Cyc_Absyn_Evar: 0){ _LL2376: _temp2375=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2325)->f1; goto _LL2374; _LL2374: _temp2373=(( struct Cyc_Absyn_Evar_struct*)
_temp2325)->f2; goto _LL2372; _LL2372: _temp2371=(( struct Cyc_Absyn_Evar_struct*)
_temp2325)->f3; goto _LL2330;} else{ goto _LL2331;} _LL2331: if(( unsigned int)
_temp2325 > 4u?*(( int*) _temp2325) == Cyc_Absyn_VarType: 0){ _LL2378: _temp2377=((
struct Cyc_Absyn_VarType_struct*) _temp2325)->f1; goto _LL2332;} else{ goto
_LL2333;} _LL2333: if(( unsigned int) _temp2325 > 4u?*(( int*) _temp2325) == Cyc_Absyn_EnumType:
0){ _LL2383: _temp2382=(( struct Cyc_Absyn_EnumType_struct*) _temp2325)->f1;
goto _LL2380; _LL2380: _temp2379=(( struct Cyc_Absyn_EnumType_struct*) _temp2325)->f2;
_temp2381=&(( struct Cyc_Absyn_EnumType_struct*) _temp2325)->f2; goto _LL2334;}
else{ goto _LL2335;} _LL2335: if(( unsigned int) _temp2325 > 4u?*(( int*)
_temp2325) == Cyc_Absyn_TunionType: 0){ _LL2385: _temp2384=(( struct Cyc_Absyn_TunionType_struct*)
_temp2325)->f1; _LL2391: _temp2390=( void*) _temp2384.tunion_info; _temp2392=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2325)->f1).tunion_info; goto _LL2389;
_LL2389: _temp2388= _temp2384.targs; goto _LL2387; _LL2387: _temp2386=( void*)
_temp2384.rgn; goto _LL2336;} else{ goto _LL2337;} _LL2337: if(( unsigned int)
_temp2325 > 4u?*(( int*) _temp2325) == Cyc_Absyn_TunionFieldType: 0){ _LL2394:
_temp2393=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2325)->f1; _LL2398:
_temp2397=( void*) _temp2393.field_info; _temp2399=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2325)->f1).field_info; goto _LL2396; _LL2396: _temp2395= _temp2393.targs;
goto _LL2338;} else{ goto _LL2339;} _LL2339: if(( unsigned int) _temp2325 > 4u?*((
int*) _temp2325) == Cyc_Absyn_PointerType: 0){ _LL2401: _temp2400=(( struct Cyc_Absyn_PointerType_struct*)
_temp2325)->f1; _LL2411: _temp2410=( void*) _temp2400.elt_typ; goto _LL2409;
_LL2409: _temp2408=( void*) _temp2400.rgn_typ; goto _LL2407; _LL2407: _temp2406=
_temp2400.nullable; goto _LL2405; _LL2405: _temp2404= _temp2400.tq; goto _LL2403;
_LL2403: _temp2402= _temp2400.bounds; goto _LL2340;} else{ goto _LL2341;}
_LL2341: if(( unsigned int) _temp2325 > 4u?*(( int*) _temp2325) == Cyc_Absyn_IntType:
0){ _LL2415: _temp2414=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2325)->f1;
goto _LL2413; _LL2413: _temp2412=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2325)->f2; goto _LL2342;} else{ goto _LL2343;} _LL2343: if( _temp2325 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2344;} else{ goto _LL2345;} _LL2345: if(
_temp2325 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2346;} else{ goto _LL2347;}
_LL2347: if(( unsigned int) _temp2325 > 4u?*(( int*) _temp2325) == Cyc_Absyn_ArrayType:
0){ _LL2421: _temp2420=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2325)->f1;
goto _LL2419; _LL2419: _temp2418=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2325)->f2; goto _LL2417; _LL2417: _temp2416=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2325)->f3; goto _LL2348;} else{ goto _LL2349;} _LL2349: if(( unsigned int)
_temp2325 > 4u?*(( int*) _temp2325) == Cyc_Absyn_FnType: 0){ _LL2423: _temp2422=((
struct Cyc_Absyn_FnType_struct*) _temp2325)->f1; _LL2438: _temp2437= _temp2422.tvars;
_temp2439=&((( struct Cyc_Absyn_FnType_struct*) _temp2325)->f1).tvars; goto
_LL2435; _LL2435: _temp2434= _temp2422.effect; _temp2436=&((( struct Cyc_Absyn_FnType_struct*)
_temp2325)->f1).effect; goto _LL2433; _LL2433: _temp2432=( void*) _temp2422.ret_typ;
goto _LL2431; _LL2431: _temp2430= _temp2422.args; goto _LL2429; _LL2429:
_temp2428= _temp2422.c_varargs; goto _LL2427; _LL2427: _temp2426= _temp2422.cyc_varargs;
goto _LL2425; _LL2425: _temp2424= _temp2422.attributes; goto _LL2350;} else{
goto _LL2351;} _LL2351: if(( unsigned int) _temp2325 > 4u?*(( int*) _temp2325)
== Cyc_Absyn_TupleType: 0){ _LL2441: _temp2440=(( struct Cyc_Absyn_TupleType_struct*)
_temp2325)->f1; goto _LL2352;} else{ goto _LL2353;} _LL2353: if(( unsigned int)
_temp2325 > 4u?*(( int*) _temp2325) == Cyc_Absyn_AnonStructType: 0){ _LL2443:
_temp2442=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2325)->f1; goto
_LL2354;} else{ goto _LL2355;} _LL2355: if(( unsigned int) _temp2325 > 4u?*((
int*) _temp2325) == Cyc_Absyn_AnonUnionType: 0){ _LL2445: _temp2444=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2325)->f1; goto _LL2356;} else{ goto
_LL2357;} _LL2357: if(( unsigned int) _temp2325 > 4u?*(( int*) _temp2325) == Cyc_Absyn_StructType:
0){ _LL2452: _temp2451=(( struct Cyc_Absyn_StructType_struct*) _temp2325)->f1;
goto _LL2450; _LL2450: _temp2449=(( struct Cyc_Absyn_StructType_struct*)
_temp2325)->f2; goto _LL2447; _LL2447: _temp2446=(( struct Cyc_Absyn_StructType_struct*)
_temp2325)->f3; _temp2448=&(( struct Cyc_Absyn_StructType_struct*) _temp2325)->f3;
goto _LL2358;} else{ goto _LL2359;} _LL2359: if(( unsigned int) _temp2325 > 4u?*((
int*) _temp2325) == Cyc_Absyn_UnionType: 0){ _LL2459: _temp2458=(( struct Cyc_Absyn_UnionType_struct*)
_temp2325)->f1; goto _LL2457; _LL2457: _temp2456=(( struct Cyc_Absyn_UnionType_struct*)
_temp2325)->f2; goto _LL2454; _LL2454: _temp2453=(( struct Cyc_Absyn_UnionType_struct*)
_temp2325)->f3; _temp2455=&(( struct Cyc_Absyn_UnionType_struct*) _temp2325)->f3;
goto _LL2360;} else{ goto _LL2361;} _LL2361: if(( unsigned int) _temp2325 > 4u?*((
int*) _temp2325) == Cyc_Absyn_TypedefType: 0){ _LL2466: _temp2465=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2325)->f1; goto _LL2464; _LL2464: _temp2463=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2325)->f2; goto _LL2461; _LL2461: _temp2460=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2325)->f3; _temp2462=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2325)->f3;
goto _LL2362;} else{ goto _LL2363;} _LL2363: if( _temp2325 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2364;} else{ goto _LL2365;} _LL2365: if(( unsigned int) _temp2325 > 4u?*((
int*) _temp2325) == Cyc_Absyn_RgnHandleType: 0){ _LL2468: _temp2467=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2325)->f1; goto _LL2366;} else{
goto _LL2367;} _LL2367: if(( unsigned int) _temp2325 > 4u?*(( int*) _temp2325)
== Cyc_Absyn_AccessEff: 0){ _LL2470: _temp2469=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2325)->f1; goto _LL2368;} else{ goto _LL2369;} _LL2369: if(( unsigned int)
_temp2325 > 4u?*(( int*) _temp2325) == Cyc_Absyn_JoinEff: 0){ _LL2472: _temp2471=((
struct Cyc_Absyn_JoinEff_struct*) _temp2325)->f1; goto _LL2370;} else{ goto
_LL2326;} _LL2328: goto _LL2326; _LL2330: goto _LL2326; _LL2332: { struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2377->kind); if(( void*) c->v ==( void*) Cyc_Absyn_No_constr){( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2473=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2473[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp2474; _temp2474.tag= Cyc_Absyn_Eq_constr;
_temp2474.f1=( void*) expected_kind; _temp2474;}); _temp2473;})));} free_vars=
Cyc_Tcutil_add_free_tvar( loc, free_vars, _temp2377); goto _LL2326;} _LL2334:
if(* _temp2381 == 0){ struct _handler_cons _temp2475; _push_handler(& _temp2475);{
int _temp2477= 0; if( setjmp( _temp2475.handler)){ _temp2477= 1;} if( !
_temp2477){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te, loc,
_temp2382);* _temp2381=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2476=( void*) _exn_thrown; void* _temp2479= _temp2476; _LL2481:
if( _temp2479 == Cyc_Dict_Absent){ goto _LL2482;} else{ goto _LL2483;} _LL2483:
goto _LL2484; _LL2482: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2485=
Cyc_Absynpp_qvar2string( _temp2382); xprintf("unbound type enum %.*s", _temp2485.last_plus_one
- _temp2485.curr, _temp2485.curr);})); return free_vars; _LL2484:( void) _throw(
_temp2479); _LL2480:;}}}{ struct Cyc_Absyn_Enumdecl* ed=( struct Cyc_Absyn_Enumdecl*)
_check_null(* _temp2381);* _temp2382=( ed->name)[ _check_known_subscript_notnull(
1u, 0)]; goto _LL2326;} _LL2336:{ void* _temp2486=* _temp2392; struct Cyc_Absyn_UnknownTunionInfo
_temp2492; int _temp2494; struct _tuple1* _temp2496; struct Cyc_Absyn_Tuniondecl*
_temp2498; _LL2488: if(*(( int*) _temp2486) == Cyc_Absyn_UnknownTunion){ _LL2493:
_temp2492=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp2486)->f1; _LL2497:
_temp2496= _temp2492.name; goto _LL2495; _LL2495: _temp2494= _temp2492.is_xtunion;
goto _LL2489;} else{ goto _LL2490;} _LL2490: if(*(( int*) _temp2486) == Cyc_Absyn_KnownTunion){
_LL2499: _temp2498=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2486)->f1; goto
_LL2491;} else{ goto _LL2487;} _LL2489: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2500; _push_handler(& _temp2500);{ int _temp2502= 0;
if( setjmp( _temp2500.handler)){ _temp2502= 1;} if( ! _temp2502){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2496);; _pop_handler();} else{ void* _temp2501=( void*)
_exn_thrown; void* _temp2504= _temp2501; _LL2506: if( _temp2504 == Cyc_Dict_Absent){
goto _LL2507;} else{ goto _LL2508;} _LL2508: goto _LL2509; _LL2507: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2510= Cyc_Absynpp_qvar2string( _temp2496);
xprintf("unbound type [x]tunion %.*s", _temp2510.last_plus_one - _temp2510.curr,
_temp2510.curr);})); return free_vars; _LL2509:( void) _throw( _temp2504);
_LL2505:;}}} if( tud->is_xtunion != _temp2494){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2511= Cyc_Absynpp_qvar2string( _temp2496); xprintf("[x]tunion is different from type declaration %.*s",
_temp2511.last_plus_one - _temp2511.curr, _temp2511.curr);}));}* _temp2392=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2512=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2512[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2513; _temp2513.tag= Cyc_Absyn_KnownTunion;
_temp2513.f1= tud; _temp2513;}); _temp2512;}); _temp2498= tud; goto _LL2491;}
_LL2491: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2386);{ struct Cyc_List_List* tvs= _temp2498->tvs; for( 0; _temp2388 != 0?
tvs != 0: 0;( _temp2388=(( struct Cyc_List_List*) _check_null( _temp2388))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2388))->hd; void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2388 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2514= Cyc_Absynpp_qvar2string( _temp2498->name);
xprintf("too many type arguments for tunion %.*s", _temp2514.last_plus_one -
_temp2514.curr, _temp2514.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2515= Cyc_Absynpp_qvar2string( _temp2498->name);
xprintf("too few type arguments for tunion %.*s", _temp2515.last_plus_one -
_temp2515.curr, _temp2515.curr);}));} goto _LL2487;} _LL2487:;} goto _LL2326;
_LL2338:{ void* _temp2516=* _temp2399; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2522; int _temp2524; struct _tuple1* _temp2526; struct _tuple1* _temp2528;
struct Cyc_Absyn_Tunionfield* _temp2530; struct Cyc_Absyn_Tuniondecl* _temp2532;
_LL2518: if(*(( int*) _temp2516) == Cyc_Absyn_UnknownTunionfield){ _LL2523:
_temp2522=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2516)->f1;
_LL2529: _temp2528= _temp2522.tunion_name; goto _LL2527; _LL2527: _temp2526=
_temp2522.field_name; goto _LL2525; _LL2525: _temp2524= _temp2522.is_xtunion;
goto _LL2519;} else{ goto _LL2520;} _LL2520: if(*(( int*) _temp2516) == Cyc_Absyn_KnownTunionfield){
_LL2533: _temp2532=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2516)->f1;
goto _LL2531; _LL2531: _temp2530=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2516)->f2; goto _LL2521;} else{ goto _LL2517;} _LL2519: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2534;
_push_handler(& _temp2534);{ int _temp2536= 0; if( setjmp( _temp2534.handler)){
_temp2536= 1;} if( ! _temp2536){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2528);; _pop_handler();} else{ void* _temp2535=( void*) _exn_thrown; void*
_temp2538= _temp2535; _LL2540: if( _temp2538 == Cyc_Dict_Absent){ goto _LL2541;}
else{ goto _LL2542;} _LL2542: goto _LL2543; _LL2541: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2544= Cyc_Absynpp_qvar2string( _temp2528); xprintf("unbound type tunion %.*s",
_temp2544.last_plus_one - _temp2544.curr, _temp2544.curr);})); return free_vars;
_LL2543:( void) _throw( _temp2538); _LL2539:;}}}{ struct _handler_cons _temp2545;
_push_handler(& _temp2545);{ int _temp2547= 0; if( setjmp( _temp2545.handler)){
_temp2547= 1;} if( ! _temp2547){{ void* _temp2548= Cyc_Tcenv_lookup_ordinary( te,
loc, _temp2526); struct Cyc_Absyn_Tunionfield* _temp2554; struct Cyc_Absyn_Tuniondecl*
_temp2556; _LL2550: if(*(( int*) _temp2548) == Cyc_Tcenv_TunionRes){ _LL2557:
_temp2556=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2548)->f1; goto _LL2555;
_LL2555: _temp2554=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2548)->f2; goto
_LL2551;} else{ goto _LL2552;} _LL2552: goto _LL2553; _LL2551: tuf= _temp2554;
tud= _temp2556; if( tud->is_xtunion != _temp2524){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2558= Cyc_Absynpp_qvar2string( _temp2528); xprintf("[x]tunion is different from type declaration %.*s",
_temp2558.last_plus_one - _temp2558.curr, _temp2558.curr);}));} goto _LL2549;
_LL2553: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2559= Cyc_Absynpp_qvar2string(
_temp2526); struct _tagged_string _temp2560= Cyc_Absynpp_qvar2string( _temp2528);
xprintf("unbound field %.*s in type tunion %.*s", _temp2559.last_plus_one -
_temp2559.curr, _temp2559.curr, _temp2560.last_plus_one - _temp2560.curr,
_temp2560.curr);}));{ struct Cyc_List_List* _temp2561= free_vars; _npop_handler(
0u); return _temp2561;} _LL2549:;}; _pop_handler();} else{ void* _temp2546=(
void*) _exn_thrown; void* _temp2563= _temp2546; _LL2565: if( _temp2563 == Cyc_Dict_Absent){
goto _LL2566;} else{ goto _LL2567;} _LL2567: goto _LL2568; _LL2566: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2569= Cyc_Absynpp_qvar2string( _temp2526);
struct _tagged_string _temp2570= Cyc_Absynpp_qvar2string( _temp2528); xprintf("unbound field %.*s in type tunion %.*s",
_temp2569.last_plus_one - _temp2569.curr, _temp2569.curr, _temp2570.last_plus_one
- _temp2570.curr, _temp2570.curr);})); return free_vars; _LL2568:( void) _throw(
_temp2563); _LL2564:;}}}* _temp2399=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2571=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2571[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2572; _temp2572.tag= Cyc_Absyn_KnownTunionfield; _temp2572.f1= tud;
_temp2572.f2= tuf; _temp2572;}); _temp2571;}); _temp2532= tud; _temp2530= tuf;
goto _LL2521;} _LL2521: { struct Cyc_List_List* tvs= _temp2532->tvs; for( 0;
_temp2395 != 0? tvs != 0: 0;( _temp2395=(( struct Cyc_List_List*) _check_null(
_temp2395))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2395))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2395 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2573= Cyc_Absynpp_qvar2string( _temp2532->name);
struct _tagged_string _temp2574= Cyc_Absynpp_qvar2string( _temp2530->name);
xprintf("too many type arguments for tunion %.*s.%.*s", _temp2573.last_plus_one
- _temp2573.curr, _temp2573.curr, _temp2574.last_plus_one - _temp2574.curr,
_temp2574.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2575= Cyc_Absynpp_qvar2string( _temp2532->name); struct
_tagged_string _temp2576= Cyc_Absynpp_qvar2string( _temp2530->name); xprintf("too few type arguments for tunion %.*s.%.*s",
_temp2575.last_plus_one - _temp2575.curr, _temp2575.curr, _temp2576.last_plus_one
- _temp2576.curr, _temp2576.curr);}));} goto _LL2517;} _LL2517:;} goto _LL2326;
_LL2340: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_AnyKind,
_temp2410); free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*)
Cyc_Absyn_RgnKind, _temp2408);{ void* _temp2577=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2402))->v; void*
_temp2583; struct Cyc_Absyn_Exp* _temp2585; _LL2579: if(( unsigned int)
_temp2577 > 1u?*(( int*) _temp2577) == Cyc_Absyn_Eq_constr: 0){ _LL2584:
_temp2583=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2577)->f1; if((
unsigned int) _temp2583 > 1u?*(( int*) _temp2583) == Cyc_Absyn_Upper_b: 0){
_LL2586: _temp2585=(( struct Cyc_Absyn_Upper_b_struct*) _temp2583)->f1; goto
_LL2580;} else{ goto _LL2581;}} else{ goto _LL2581;} _LL2581: goto _LL2582;
_LL2580: if( ! Cyc_Tcutil_is_const_exp( te, _temp2585)){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2587="pointer bounds expression is not a constant"; struct
_tagged_string _temp2588; _temp2588.curr= _temp2587; _temp2588.base= _temp2587;
_temp2588.last_plus_one= _temp2587 + 44; _temp2588;}));} Cyc_Tcexp_tcExp( te, 0,
_temp2585); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2585)){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2589="pointer bounds expression is not an unsigned int";
struct _tagged_string _temp2590; _temp2590.curr= _temp2589; _temp2590.base=
_temp2589; _temp2590.last_plus_one= _temp2589 + 49; _temp2590;}));} Cyc_Evexp_eval_const_uint_exp(
_temp2585); goto _LL2578; _LL2582: goto _LL2578; _LL2578:;} goto _LL2326;
_LL2342: goto _LL2326; _LL2344: goto _LL2326; _LL2346: goto _LL2326; _LL2348:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,
_temp2420); if( _temp2416 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2416))){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2591="array bounds expression is not constant";
struct _tagged_string _temp2592; _temp2592.curr= _temp2591; _temp2592.base=
_temp2591; _temp2592.last_plus_one= _temp2591 + 40; _temp2592;}));} Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*) _check_null( _temp2416)); if( ! Cyc_Tcutil_coerce_uint_typ(
te,( struct Cyc_Absyn_Exp*) _check_null( _temp2416))){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2593="array bounds expression is not an unsigned int";
struct _tagged_string _temp2594; _temp2594.curr= _temp2593; _temp2594.base=
_temp2593; _temp2594.last_plus_one= _temp2593 + 47; _temp2594;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2416)); goto _LL2326; _LL2350: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; for(
0; _temp2424 != 0; _temp2424=(( struct Cyc_List_List*) _check_null( _temp2424))->tl){
if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null(
_temp2424))->hd)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2595= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2424))->hd); xprintf("bad function type attribute %.*s",
_temp2595.last_plus_one - _temp2595.curr, _temp2595.curr);}));}{ void* _temp2596=(
void*)(( struct Cyc_List_List*) _check_null( _temp2424))->hd; _LL2598: if(
_temp2596 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL2599;} else{ goto _LL2600;}
_LL2600: if( _temp2596 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL2601;} else{
goto _LL2602;} _LL2602: if( _temp2596 ==( void*) Cyc_Absyn_Fastcall_att){ goto
_LL2603;} else{ goto _LL2604;} _LL2604: goto _LL2605; _LL2599: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2597; _LL2601: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2597; _LL2603: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2597; _LL2605: goto
_LL2597; _LL2597:;}} if( num_convs > 1){ Cyc_Tcutil_terr( loc, xprintf("function can't have multiple calling conventions"));}
Cyc_Tcutil_check_unique_tvars( loc,* _temp2439);{ struct Cyc_List_List* b=*
_temp2439; for( 0; b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){
void* _temp2606=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->kind))->v; void* _temp2612; _LL2608: if(( unsigned int)
_temp2606 > 1u?*(( int*) _temp2606) == Cyc_Absyn_Eq_constr: 0){ _LL2613:
_temp2612=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2606)->f1; if(
_temp2612 ==( void*) Cyc_Absyn_MemKind){ goto _LL2609;} else{ goto _LL2610;}}
else{ goto _LL2610;} _LL2610: goto _LL2611; _LL2609: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2614=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_temp2614.last_plus_one - _temp2614.curr, _temp2614.curr);})); goto _LL2607;
_LL2611: goto _LL2607; _LL2607:;}}{ struct Cyc_List_List* new_free_vars= 0;
new_free_vars= Cyc_Tcutil_check_valid_type( loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind,
_temp2432); for( 0; _temp2430 != 0; _temp2430=(( struct Cyc_List_List*)
_check_null( _temp2430))->tl){ new_free_vars= Cyc_Tcutil_check_valid_type( loc,
te, new_free_vars,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp2430))->hd)).f3);} if( _temp2426 != 0){ if( _temp2428){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp2615="both c_vararg and cyc_vararg";
struct _tagged_string _temp2616; _temp2616.curr= _temp2615; _temp2616.base=
_temp2615; _temp2616.last_plus_one= _temp2615 + 29; _temp2616;}));}{ int
_temp2619; void* _temp2621; void* _temp2623; struct Cyc_Absyn_Tqual _temp2625;
struct Cyc_Core_Opt* _temp2627; struct Cyc_Absyn_VarargInfo _temp2617=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp2426)); _LL2628: _temp2627= _temp2617.name;
goto _LL2626; _LL2626: _temp2625= _temp2617.tq; goto _LL2624; _LL2624: _temp2623=(
void*) _temp2617.type; goto _LL2622; _LL2622: _temp2621=( void*) _temp2617.rgn;
goto _LL2620; _LL2620: _temp2619= _temp2617.inject; goto _LL2618; _LL2618:
new_free_vars= Cyc_Tcutil_check_valid_type( loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind,
_temp2623);{ void* _temp2629= Cyc_Tcutil_compress( _temp2621); int _temp2639;
struct Cyc_Core_Opt* _temp2641; void* _temp2643; struct Cyc_Absyn_Tvar*
_temp2645; _LL2631: if(( unsigned int) _temp2629 > 4u?*(( int*) _temp2629) ==
Cyc_Absyn_Evar: 0){ _LL2644: _temp2643=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2629)->f1; goto _LL2642; _LL2642: _temp2641=(( struct Cyc_Absyn_Evar_struct*)
_temp2629)->f2; goto _LL2640; _LL2640: _temp2639=(( struct Cyc_Absyn_Evar_struct*)
_temp2629)->f3; goto _LL2632;} else{ goto _LL2633;} _LL2633: if(( unsigned int)
_temp2629 > 4u?*(( int*) _temp2629) == Cyc_Absyn_VarType: 0){ _LL2646: _temp2645=((
struct Cyc_Absyn_VarType_struct*) _temp2629)->f1; goto _LL2634;} else{ goto
_LL2635;} _LL2635: if( _temp2629 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2636;}
else{ goto _LL2637;} _LL2637: goto _LL2638; _LL2632: { void* vr_tvar=( void*)({
struct Cyc_Absyn_VarType_struct* _temp2647=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2647[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2648; _temp2648.tag= Cyc_Absyn_VarType; _temp2648.f1=
Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); _temp2648;}); _temp2647;}); Cyc_Tcutil_unify(
_temp2621, vr_tvar); goto _LL2630;} _LL2634: goto _LL2630; _LL2636: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2649="varargs should be in a region, not heap-allocated";
struct _tagged_string _temp2650; _temp2650.curr= _temp2649; _temp2650.base=
_temp2649; _temp2650.last_plus_one= _temp2649 + 50; _temp2650;})); goto _LL2630;
_LL2638: goto _LL2630; _LL2630:;} new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_RgnKind, _temp2621); if( _temp2619){
void* _temp2651= Cyc_Tcutil_compress( _temp2623); struct Cyc_Absyn_TunionInfo
_temp2657; void* _temp2659; struct Cyc_List_List* _temp2661; void* _temp2663;
struct Cyc_Absyn_Tuniondecl* _temp2665; _LL2653: if(( unsigned int) _temp2651 >
4u?*(( int*) _temp2651) == Cyc_Absyn_TunionType: 0){ _LL2658: _temp2657=((
struct Cyc_Absyn_TunionType_struct*) _temp2651)->f1; _LL2664: _temp2663=( void*)
_temp2657.tunion_info; if(*(( int*) _temp2663) == Cyc_Absyn_KnownTunion){
_LL2666: _temp2665=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2663)->f1; goto
_LL2662;} else{ goto _LL2655;} _LL2662: _temp2661= _temp2657.targs; goto _LL2660;
_LL2660: _temp2659=( void*) _temp2657.rgn; goto _LL2654;} else{ goto _LL2655;}
_LL2655: goto _LL2656; _LL2654:{ void* _temp2667= Cyc_Tcutil_compress( _temp2659);
int _temp2677; struct Cyc_Core_Opt* _temp2679; void* _temp2681; struct Cyc_Absyn_Tvar*
_temp2683; _LL2669: if(( unsigned int) _temp2667 > 4u?*(( int*) _temp2667) ==
Cyc_Absyn_Evar: 0){ _LL2682: _temp2681=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2667)->f1; goto _LL2680; _LL2680: _temp2679=(( struct Cyc_Absyn_Evar_struct*)
_temp2667)->f2; goto _LL2678; _LL2678: _temp2677=(( struct Cyc_Absyn_Evar_struct*)
_temp2667)->f3; goto _LL2670;} else{ goto _LL2671;} _LL2671: if(( unsigned int)
_temp2667 > 4u?*(( int*) _temp2667) == Cyc_Absyn_VarType: 0){ _LL2684: _temp2683=((
struct Cyc_Absyn_VarType_struct*) _temp2667)->f1; goto _LL2672;} else{ goto
_LL2673;} _LL2673: if( _temp2667 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2674;}
else{ goto _LL2675;} _LL2675: goto _LL2676; _LL2670: Cyc_Tcutil_unify( _temp2621,
_temp2659); goto _LL2668; _LL2672: goto _LL2668; _LL2674: Cyc_Tcutil_terr( loc,({
unsigned char* _temp2685="injected vararg [x]tunion should be in a region, not heap-allocated";
struct _tagged_string _temp2686; _temp2686.curr= _temp2685; _temp2686.base=
_temp2685; _temp2686.last_plus_one= _temp2685 + 68; _temp2686;})); goto _LL2668;
_LL2676: goto _LL2668; _LL2668:;} goto _LL2652; _LL2656: Cyc_Tcutil_terr( loc,({
unsigned char* _temp2687="can't inject a non-[x]tunion type"; struct
_tagged_string _temp2688; _temp2688.curr= _temp2687; _temp2688.base= _temp2687;
_temp2688.last_plus_one= _temp2687 + 34; _temp2688;})); goto _LL2652; _LL2652:;}}}
if(* _temp2436 != 0){ new_free_vars= Cyc_Tcutil_check_valid_type( loc, te,
new_free_vars,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2436))->v);} else{ struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar((
void*) Cyc_Absyn_EffKind); new_free_vars=({ struct Cyc_List_List* _temp2689=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2689->hd=(
void*) e; _temp2689->tl= new_free_vars; _temp2689;});{ struct Cyc_List_List*
effect= 0;{ struct Cyc_List_List* tvs= new_free_vars; for( 0; tvs != 0; tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl){ void* _temp2690=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v;
void* _temp2698; void* _temp2700; _LL2692: if(( unsigned int) _temp2690 > 1u?*((
int*) _temp2690) == Cyc_Absyn_Eq_constr: 0){ _LL2699: _temp2698=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2690)->f1; if( _temp2698 ==( void*) Cyc_Absyn_RgnKind){
goto _LL2693;} else{ goto _LL2694;}} else{ goto _LL2694;} _LL2694: if((
unsigned int) _temp2690 > 1u?*(( int*) _temp2690) == Cyc_Absyn_Eq_constr: 0){
_LL2701: _temp2700=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2690)->f1;
if( _temp2700 ==( void*) Cyc_Absyn_EffKind){ goto _LL2695;} else{ goto _LL2696;}}
else{ goto _LL2696;} _LL2696: goto _LL2697; _LL2693: effect=({ struct Cyc_List_List*
_temp2702=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2702->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2703=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2703[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2704; _temp2704.tag= Cyc_Absyn_AccessEff;
_temp2704.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2705=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2705[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2706; _temp2706.tag= Cyc_Absyn_VarType;
_temp2706.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2706;}); _temp2705;})); _temp2704;}); _temp2703;})); _temp2702->tl= effect;
_temp2702;}); goto _LL2691; _LL2695: effect=({ struct Cyc_List_List* _temp2707=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2707->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2708=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2708[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2709; _temp2709.tag= Cyc_Absyn_VarType; _temp2709.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2709;}); _temp2708;})); _temp2707->tl= effect; _temp2707;}); goto _LL2691;
_LL2697: goto _LL2691; _LL2691:;}}* _temp2436=({ struct Cyc_Core_Opt* _temp2710=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2710->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2711=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2711[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2712; _temp2712.tag= Cyc_Absyn_JoinEff; _temp2712.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( effect);
_temp2712;}); _temp2711;})); _temp2710;});}} if(* _temp2439 != 0){ struct Cyc_List_List*
new_btvs= 0;{ struct Cyc_List_List* bs=* _temp2439; for( 0; bs != 0; bs=((
struct Cyc_List_List*) _check_null( bs))->tl){ new_free_vars= Cyc_Tcutil_add_free_tvar(
loc, new_free_vars,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd);{ struct Cyc_Absyn_Conref* _temp2713=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->kind); goto _LL2714; _LL2714: {
void* _temp2715=( void*) _temp2713->v; void* _temp2723; _LL2717: if( _temp2715
==( void*) Cyc_Absyn_No_constr){ goto _LL2718;} else{ goto _LL2719;} _LL2719:
if(( unsigned int) _temp2715 > 1u?*(( int*) _temp2715) == Cyc_Absyn_Eq_constr: 0){
_LL2724: _temp2723=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2715)->f1;
if( _temp2723 ==( void*) Cyc_Absyn_MemKind){ goto _LL2720;} else{ goto _LL2721;}}
else{ goto _LL2721;} _LL2721: goto _LL2722; _LL2718: Cyc_Tcutil_warn( loc,({
struct _tagged_string _temp2725=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp2725.last_plus_one - _temp2725.curr, _temp2725.curr);})); goto _LL2720;
_LL2720:( void*)( _temp2713->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2726=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2726[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2727; _temp2727.tag= Cyc_Absyn_Eq_constr;
_temp2727.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2727;}); _temp2726;})));
goto _LL2716; _LL2722: goto _LL2716; _LL2716:;}}}} new_free_vars= Cyc_Tcutil_remove_bound_tvars(
new_free_vars,* _temp2439);} for( 0; new_free_vars != 0; new_free_vars=(( struct
Cyc_List_List*) _check_null( new_free_vars))->tl){ free_vars= Cyc_Tcutil_add_free_tvar(
loc, free_vars,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
new_free_vars))->hd);} goto _LL2326;}} _LL2352: for( 0; _temp2440 != 0;
_temp2440=(( struct Cyc_List_List*) _check_null( _temp2440))->tl){ free_vars=
Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp2440))->hd)).f2);}
goto _LL2326; _LL2354:{ struct _RegionHandle _temp2728= _new_region(); struct
_RegionHandle* sprev_rgn=& _temp2728; _push_region( sprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp2442 != 0; _temp2442=(( struct Cyc_List_List*)
_check_null( _temp2442))->tl){ struct Cyc_Absyn_Structfield _temp2731; struct
Cyc_List_List* _temp2732; struct Cyc_Core_Opt* _temp2734; void* _temp2736;
struct Cyc_Absyn_Tqual _temp2738; struct _tagged_string* _temp2740; struct Cyc_Absyn_Structfield*
_temp2729=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2442))->hd; _temp2731=* _temp2729; _LL2741: _temp2740= _temp2731.name; goto
_LL2739; _LL2739: _temp2738= _temp2731.tq; goto _LL2737; _LL2737: _temp2736=(
void*) _temp2731.type; goto _LL2735; _LL2735: _temp2734= _temp2731.width; goto
_LL2733; _LL2733: _temp2732= _temp2731.attributes; goto _LL2730; _LL2730: if(((
int(*)( int(* compare)( struct _tagged_string*, struct _tagged_string*), struct
Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields, _temp2740)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2742=* _temp2740; xprintf("duplicate field %.*s in struct", _temp2742.last_plus_one
- _temp2742.curr, _temp2742.curr);}));} if( Cyc_String_strcmp(* _temp2740,({
unsigned char* _temp2743=""; struct _tagged_string _temp2744; _temp2744.curr=
_temp2743; _temp2744.base= _temp2743; _temp2744.last_plus_one= _temp2743 + 1;
_temp2744;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp2745=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp2745->hd=( void*)
_temp2740; _temp2745->tl= prev_fields; _temp2745;});} Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_MemKind, _temp2736); if( _temp2734 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2734))->v)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2746=* _temp2740; xprintf("bitfield %.*s does not have constant width",
_temp2746.last_plus_one - _temp2746.curr, _temp2746.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2734))->v);}{
void* _temp2747= Cyc_Tcutil_compress( _temp2736); void* _temp2753; void*
_temp2755; _LL2749: if(( unsigned int) _temp2747 > 4u?*(( int*) _temp2747) ==
Cyc_Absyn_IntType: 0){ _LL2756: _temp2755=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2747)->f1; goto _LL2754; _LL2754: _temp2753=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2747)->f2; goto _LL2750;} else{ goto _LL2751;} _LL2751: goto _LL2752;
_LL2750:{ void* _temp2757= _temp2753; _LL2759: if( _temp2757 ==( void*) Cyc_Absyn_B1){
goto _LL2760;} else{ goto _LL2761;} _LL2761: if( _temp2757 ==( void*) Cyc_Absyn_B2){
goto _LL2762;} else{ goto _LL2763;} _LL2763: if( _temp2757 ==( void*) Cyc_Absyn_B4){
goto _LL2764;} else{ goto _LL2765;} _LL2765: if( _temp2757 ==( void*) Cyc_Absyn_B8){
goto _LL2766;} else{ goto _LL2758;} _LL2760: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2767="bitfield larger than type"; struct _tagged_string
_temp2768; _temp2768.curr= _temp2767; _temp2768.base= _temp2767; _temp2768.last_plus_one=
_temp2767 + 26; _temp2768;}));} goto _LL2758; _LL2762: if( w > 16){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2769="bitfield larger than type"; struct
_tagged_string _temp2770; _temp2770.curr= _temp2769; _temp2770.base= _temp2769;
_temp2770.last_plus_one= _temp2769 + 26; _temp2770;}));} goto _LL2758; _LL2764:
if( w > 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2771="bitfield larger than type";
struct _tagged_string _temp2772; _temp2772.curr= _temp2771; _temp2772.base=
_temp2771; _temp2772.last_plus_one= _temp2771 + 26; _temp2772;}));} goto _LL2758;
_LL2766: if( w > 64){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2773="bitfield larger than type";
struct _tagged_string _temp2774; _temp2774.curr= _temp2773; _temp2774.base=
_temp2773; _temp2774.last_plus_one= _temp2773 + 26; _temp2774;}));} goto _LL2758;
_LL2758:;} goto _LL2748; _LL2752: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2775=* _temp2740; struct _tagged_string _temp2776= Cyc_Absynpp_typ2string(
_temp2736); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2775.last_plus_one - _temp2775.curr, _temp2775.curr, _temp2776.last_plus_one
- _temp2776.curr, _temp2776.curr);})); goto _LL2748; _LL2748:;}} for( 0;
_temp2732 != 0; _temp2732=(( struct Cyc_List_List*) _check_null( _temp2732))->tl){
void* _temp2777=( void*)(( struct Cyc_List_List*) _check_null( _temp2732))->hd;
int _temp2785; _LL2779: if( _temp2777 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL2780;} else{ goto _LL2781;} _LL2781: if(( unsigned int) _temp2777 > 16u?*((
int*) _temp2777) == Cyc_Absyn_Aligned_att: 0){ _LL2786: _temp2785=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp2777)->f1; goto _LL2782;} else{ goto _LL2783;} _LL2783: goto _LL2784;
_LL2780: continue; _LL2782: continue; _LL2784: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2787= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp2732))->hd); struct _tagged_string _temp2788=* _temp2740;
xprintf("bad attribute %.*s on struct member %.*s", _temp2787.last_plus_one -
_temp2787.curr, _temp2787.curr, _temp2788.last_plus_one - _temp2788.curr,
_temp2788.curr);})); goto _LL2778; _LL2778:;}}}; _pop_region(& _temp2728);} goto
_LL2326; _LL2356:{ struct _RegionHandle _temp2789= _new_region(); struct
_RegionHandle* uprev_rgn=& _temp2789; _push_region( uprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp2444 != 0; _temp2444=(( struct Cyc_List_List*)
_check_null( _temp2444))->tl){ struct Cyc_Absyn_Structfield _temp2792; struct
Cyc_List_List* _temp2793; struct Cyc_Core_Opt* _temp2795; void* _temp2797;
struct Cyc_Absyn_Tqual _temp2799; struct _tagged_string* _temp2801; struct Cyc_Absyn_Structfield*
_temp2790=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2444))->hd; _temp2792=* _temp2790; _LL2802: _temp2801= _temp2792.name; goto
_LL2800; _LL2800: _temp2799= _temp2792.tq; goto _LL2798; _LL2798: _temp2797=(
void*) _temp2792.type; goto _LL2796; _LL2796: _temp2795= _temp2792.width; goto
_LL2794; _LL2794: _temp2793= _temp2792.attributes; goto _LL2791; _LL2791: if(((
int(*)( int(* compare)( struct _tagged_string*, struct _tagged_string*), struct
Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields, _temp2801)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2803=* _temp2801; xprintf("duplicate field %.*s in struct", _temp2803.last_plus_one
- _temp2803.curr, _temp2803.curr);}));} if( Cyc_String_strcmp(* _temp2801,({
unsigned char* _temp2804=""; struct _tagged_string _temp2805; _temp2805.curr=
_temp2804; _temp2805.base= _temp2804; _temp2805.last_plus_one= _temp2804 + 1;
_temp2805;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp2806=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp2806->hd=( void*)
_temp2801; _temp2806->tl= prev_fields; _temp2806;});} Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_MemKind, _temp2797); if( ! Cyc_Tcutil_bits_only(
_temp2797)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2807=* _temp2801;
xprintf("%.*s has a type that is possibly incompatible with other members of union",
_temp2807.last_plus_one - _temp2807.curr, _temp2807.curr);}));} if( _temp2795 !=
0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2795))->v)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2808=* _temp2801; xprintf("bitfield %.*s does not have constant width",
_temp2808.last_plus_one - _temp2808.curr, _temp2808.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2795))->v);}{
void* _temp2809= Cyc_Tcutil_compress( _temp2797); void* _temp2815; void*
_temp2817; _LL2811: if(( unsigned int) _temp2809 > 4u?*(( int*) _temp2809) ==
Cyc_Absyn_IntType: 0){ _LL2818: _temp2817=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2809)->f1; goto _LL2816; _LL2816: _temp2815=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2809)->f2; goto _LL2812;} else{ goto _LL2813;} _LL2813: goto _LL2814;
_LL2812:{ void* _temp2819= _temp2815; _LL2821: if( _temp2819 ==( void*) Cyc_Absyn_B1){
goto _LL2822;} else{ goto _LL2823;} _LL2823: if( _temp2819 ==( void*) Cyc_Absyn_B2){
goto _LL2824;} else{ goto _LL2825;} _LL2825: if( _temp2819 ==( void*) Cyc_Absyn_B4){
goto _LL2826;} else{ goto _LL2827;} _LL2827: if( _temp2819 ==( void*) Cyc_Absyn_B8){
goto _LL2828;} else{ goto _LL2820;} _LL2822: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2829="bitfield larger than type"; struct _tagged_string
_temp2830; _temp2830.curr= _temp2829; _temp2830.base= _temp2829; _temp2830.last_plus_one=
_temp2829 + 26; _temp2830;}));} goto _LL2820; _LL2824: if( w > 16){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2831="bitfield larger than type"; struct
_tagged_string _temp2832; _temp2832.curr= _temp2831; _temp2832.base= _temp2831;
_temp2832.last_plus_one= _temp2831 + 26; _temp2832;}));} goto _LL2820; _LL2826:
if( w > 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2833="bitfield larger than type";
struct _tagged_string _temp2834; _temp2834.curr= _temp2833; _temp2834.base=
_temp2833; _temp2834.last_plus_one= _temp2833 + 26; _temp2834;}));} goto _LL2820;
_LL2828: if( w > 64){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2835="bitfield larger than type";
struct _tagged_string _temp2836; _temp2836.curr= _temp2835; _temp2836.base=
_temp2835; _temp2836.last_plus_one= _temp2835 + 26; _temp2836;}));} goto _LL2820;
_LL2820:;} goto _LL2810; _LL2814: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2837=* _temp2801; struct _tagged_string _temp2838= Cyc_Absynpp_typ2string(
_temp2797); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2837.last_plus_one - _temp2837.curr, _temp2837.curr, _temp2838.last_plus_one
- _temp2838.curr, _temp2838.curr);})); goto _LL2810; _LL2810:;}} for( 0;
_temp2793 != 0; _temp2793=(( struct Cyc_List_List*) _check_null( _temp2793))->tl){
void* _temp2839=( void*)(( struct Cyc_List_List*) _check_null( _temp2793))->hd;
int _temp2847; _LL2841: if( _temp2839 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL2842;} else{ goto _LL2843;} _LL2843: if(( unsigned int) _temp2839 > 16u?*((
int*) _temp2839) == Cyc_Absyn_Aligned_att: 0){ _LL2848: _temp2847=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp2839)->f1; goto _LL2844;} else{ goto _LL2845;} _LL2845: goto _LL2846;
_LL2842: continue; _LL2844: continue; _LL2846: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2849= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp2793))->hd); struct _tagged_string _temp2850=* _temp2801;
xprintf("bad attribute %.*s on struct member %.*s", _temp2849.last_plus_one -
_temp2849.curr, _temp2849.curr, _temp2850.last_plus_one - _temp2850.curr,
_temp2850.curr);})); goto _LL2840; _LL2840:;}}}; _pop_region(& _temp2789);} goto
_LL2326; _LL2358: if( _temp2451 == 0){ if(* _temp2448 == 0){ Cyc_Tcutil_terr(
loc, xprintf("bad anonymous struct")); return free_vars;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2448));;}} if(*
_temp2448 == 0){ struct _handler_cons _temp2851; _push_handler(& _temp2851);{
int _temp2853= 0; if( setjmp( _temp2851.handler)){ _temp2853= 1;} if( !
_temp2853){* _temp2448=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2451));; _pop_handler();} else{
void* _temp2852=( void*) _exn_thrown; void* _temp2855= _temp2852; _LL2857: if(
_temp2855 == Cyc_Dict_Absent){ goto _LL2858;} else{ goto _LL2859;} _LL2859: goto
_LL2860; _LL2858: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2861= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2451)); xprintf("unbound type struct %.*s",
_temp2861.last_plus_one - _temp2861.curr, _temp2861.curr);})); return free_vars;
_LL2860:( void) _throw( _temp2855); _LL2856:;}}}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2448)); if( sd->name !=
0){*(( struct _tuple1*) _check_null( _temp2451))=(( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( sd->name))->v)[ _check_known_subscript_notnull( 1u,
0)];}{ struct Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2449); if( lvs != largs){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2862= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2451));
int _temp2863= lvs; int _temp2864= largs; xprintf("struct %.*s expects %d type arguments but was given %d",
_temp2862.last_plus_one - _temp2862.curr, _temp2862.curr, _temp2863, _temp2864);}));}
for( 0; _temp2449 != 0;( _temp2449=(( struct Cyc_List_List*) _check_null(
_temp2449))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2449))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2326;}} _LL2360: if( _temp2458 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2865="missing union name"; struct _tagged_string
_temp2866; _temp2866.curr= _temp2865; _temp2866.base= _temp2865; _temp2866.last_plus_one=
_temp2865 + 19; _temp2866;})); return free_vars;} if(* _temp2455 == 0){ struct
_handler_cons _temp2867; _push_handler(& _temp2867);{ int _temp2869= 0; if(
setjmp( _temp2867.handler)){ _temp2869= 1;} if( ! _temp2869){* _temp2455=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp2458));; _pop_handler();} else{ void* _temp2868=(
void*) _exn_thrown; void* _temp2871= _temp2868; _LL2873: if( _temp2871 == Cyc_Dict_Absent){
goto _LL2874;} else{ goto _LL2875;} _LL2875: goto _LL2876; _LL2874: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2877= Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp2458)); xprintf("unbound type union %.*s", _temp2877.last_plus_one
- _temp2877.curr, _temp2877.curr);})); return free_vars; _LL2876:( void) _throw(
_temp2871); _LL2872:;}}}{ struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**)
_check_null(* _temp2455)); if( ud->name != 0){*(( struct _tuple1*) _check_null(
_temp2458))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2456); if( lvs != largs){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2878= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2458)); int _temp2879= lvs; int _temp2880=
largs; xprintf("union %.*s expects %d type arguments but was given %d",
_temp2878.last_plus_one - _temp2878.curr, _temp2878.curr, _temp2879, _temp2880);}));}
for( 0; _temp2456 != 0;( _temp2456=(( struct Cyc_List_List*) _check_null(
_temp2456))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2456))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2326;}} _LL2362: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp2881; _push_handler(& _temp2881);{ int _temp2883=
0; if( setjmp( _temp2881.handler)){ _temp2883= 1;} if( ! _temp2883){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2465);; _pop_handler();} else{ void* _temp2882=( void*)
_exn_thrown; void* _temp2885= _temp2882; _LL2887: if( _temp2885 == Cyc_Dict_Absent){
goto _LL2888;} else{ goto _LL2889;} _LL2889: goto _LL2890; _LL2888: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2891= Cyc_Absynpp_qvar2string( _temp2465);
xprintf("unbound typedef name %.*s", _temp2891.last_plus_one - _temp2891.curr,
_temp2891.curr);})); return free_vars; _LL2890:( void) _throw( _temp2885);
_LL2886:;}}} _temp2465[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2463; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type( loc, te,
free_vars, k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); inst=({
struct Cyc_List_List* _temp2892=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2892->hd=( void*)({ struct _tuple5* _temp2893=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp2893->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; _temp2893->f2=( void*)(( struct
Cyc_List_List*) _check_null( ts))->hd; _temp2893;}); _temp2892->tl= inst;
_temp2892;});} if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2894= Cyc_Absynpp_qvar2string( _temp2465); xprintf("typedef %.*s expects a different number of arguments",
_temp2894.last_plus_one - _temp2894.curr, _temp2894.curr);})); return free_vars;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2462=({
struct Cyc_Core_Opt* _temp2895=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2895->v=( void*) new_typ; _temp2895;}); goto _LL2326;}}}
_LL2364: goto _LL2326; _LL2366: _temp2469= _temp2467; goto _LL2368; _LL2368:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2469); goto _LL2326; _LL2370: for( 0; _temp2471 != 0; _temp2471=(( struct
Cyc_List_List*) _check_null( _temp2471))->tl){ free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2471))->hd);} goto _LL2326; _LL2326:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2896= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp2897= Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t)); struct
_tagged_string _temp2898= Cyc_Absynpp_kind2string( expected_kind); xprintf("type %.*s has kind %.*s but as used here needs kind %.*s",
_temp2896.last_plus_one - _temp2896.curr, _temp2896.curr, _temp2897.last_plus_one
- _temp2897.curr, _temp2897.curr, _temp2898.last_plus_one - _temp2898.curr,
_temp2898.curr);}));} return free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t);{ struct Cyc_List_List* x= free_tvars; for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( x))->hd)->kind); void* _temp2899=( void*) c->v;
void* _temp2907; _LL2901: if( _temp2899 ==( void*) Cyc_Absyn_No_constr){ goto
_LL2902;} else{ goto _LL2903;} _LL2903: if(( unsigned int) _temp2899 > 1u?*((
int*) _temp2899) == Cyc_Absyn_Eq_constr: 0){ _LL2908: _temp2907=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2899)->f1; if( _temp2907 ==( void*) Cyc_Absyn_MemKind){
goto _LL2904;} else{ goto _LL2905;}} else{ goto _LL2905;} _LL2905: goto _LL2906;
_LL2902: goto _LL2904; _LL2904:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2909=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2909[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2910; _temp2910.tag= Cyc_Absyn_Eq_constr;
_temp2910.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2910;}); _temp2909;})));
goto _LL2900; _LL2906: goto _LL2900; _LL2900:;}} if( free_tvars != 0){ void*
_temp2911= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2917; struct
Cyc_List_List* _temp2919; struct Cyc_Absyn_VarargInfo* _temp2921; int _temp2923;
struct Cyc_List_List* _temp2925; void* _temp2927; struct Cyc_Core_Opt* _temp2929;
struct Cyc_List_List* _temp2931; struct Cyc_List_List** _temp2933; _LL2913: if((
unsigned int) _temp2911 > 4u?*(( int*) _temp2911) == Cyc_Absyn_FnType: 0){
_LL2918: _temp2917=(( struct Cyc_Absyn_FnType_struct*) _temp2911)->f1; _LL2932:
_temp2931= _temp2917.tvars; _temp2933=&((( struct Cyc_Absyn_FnType_struct*)
_temp2911)->f1).tvars; goto _LL2930; _LL2930: _temp2929= _temp2917.effect; goto
_LL2928; _LL2928: _temp2927=( void*) _temp2917.ret_typ; goto _LL2926; _LL2926:
_temp2925= _temp2917.args; goto _LL2924; _LL2924: _temp2923= _temp2917.c_varargs;
goto _LL2922; _LL2922: _temp2921= _temp2917.cyc_varargs; goto _LL2920; _LL2920:
_temp2919= _temp2917.attributes; goto _LL2914;} else{ goto _LL2915;} _LL2915:
goto _LL2916; _LL2914: if(* _temp2933 == 0){* _temp2933= free_tvars;} else{ if(
free_tvars != 0){ struct _tagged_string s=*(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( free_tvars))->hd)->name; if(*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), 0) =='?'){ Cyc_Tcutil_terr( loc, xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2934= s; xprintf("unbound type variable %.*s",
_temp2934.last_plus_one - _temp2934.curr, _temp2934.curr);}));}}} goto _LL2912;
_LL2916: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2935=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( free_tvars))->hd)->name; xprintf("unbound type variable %.*s",
_temp2935.last_plus_one - _temp2935.curr, _temp2935.curr);})); return; _LL2912:;}}
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0, expected_kind, t); struct
Cyc_List_List* fs= free_tvars; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Tvar* ftv=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( fs))->hd; struct _tagged_string* _temp2936=
ftv->name; goto _LL2937; _LL2937: { struct Cyc_Absyn_Conref* _temp2938= ftv->kind;
goto _LL2939; _LL2939: { int present= 0;{ struct Cyc_List_List* bs= bound_tvars;
for( 0; bs != 0; bs=(( struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Tvar*
btv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( bs))->hd;
struct _tagged_string* _temp2940= btv->name; goto _LL2941; _LL2941: { struct Cyc_Absyn_Conref*
_temp2942= btv->kind; goto _LL2943; _LL2943: if( Cyc_String_zstrptrcmp(
_temp2936, _temp2940) == 0){ present= 1; if( ! Cyc_Tcutil_constrain_kinds(
_temp2942, _temp2938)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2944=*
_temp2936; struct _tagged_string _temp2945= Cyc_Absynpp_ckind2string( _temp2942);
struct _tagged_string _temp2946= Cyc_Absynpp_ckind2string( _temp2938); xprintf("type variable %.*s has kind %.*s but is used here atkind %.*s",
_temp2944.last_plus_one - _temp2944.curr, _temp2944.curr, _temp2945.last_plus_one
- _temp2945.curr, _temp2945.curr, _temp2946.last_plus_one - _temp2946.curr,
_temp2946.curr);}));} break;}}}} if( ! present){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2947=* _temp2936; struct _tagged_string _temp2948= Cyc_Absynpp_typ2string(
t); xprintf("unbound type variable %.*s in type %.*s", _temp2947.last_plus_one -
_temp2947.curr, _temp2947.curr, _temp2948.last_plus_one - _temp2948.curr,
_temp2948.curr);}));}}}}} static void Cyc_Tcutil_check_unique_unsorted( int(*
cmp)( void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for( 0;
vs != 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=((
struct Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2949= msg; struct
_tagged_string _temp2950= a2string(( void*)(( struct Cyc_List_List*) _check_null(
vs))->hd); xprintf("%.*s: %.*s", _temp2949.last_plus_one - _temp2949.curr,
_temp2949.curr, _temp2950.last_plus_one - _temp2950.curr, _temp2950.curr);}));}}}}
static struct _tagged_string Cyc_Tcutil_strptr2string( struct _tagged_string* s){
return* s;} void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct
Cyc_Position_Segment* loc, struct _tagged_string msg){(( void(*)( int(* cmp)(
struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* vs,
struct Cyc_Position_Segment* loc, struct _tagged_string(* a2string)( struct
_tagged_string*), struct _tagged_string msg)) Cyc_Tcutil_check_unique_unsorted)(
Cyc_String_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string, msg);} void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs){(( void(*)( int(*
cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* vs,
struct Cyc_Position_Segment* loc, struct _tagged_string(* a2string)( struct Cyc_Absyn_Tvar*),
struct _tagged_string msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_Absyn_tvar_cmp,
tvs, loc, Cyc_Tcutil_tvar2string,({ unsigned char* _temp2951="duplicate type variable";
struct _tagged_string _temp2952; _temp2952.curr= _temp2951; _temp2952.base=
_temp2951; _temp2952.last_plus_one= _temp2951 + 24; _temp2952;}));} struct
_tuple16{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple17{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple18{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_String_strcmp(*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( sd_fields))->hd)->name,({
unsigned char* _temp2953=""; struct _tagged_string _temp2954; _temp2954.curr=
_temp2953; _temp2954.base= _temp2953; _temp2954.last_plus_one= _temp2953 + 1;
_temp2954;})) != 0){ fields=({ struct Cyc_List_List* _temp2955=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2955->hd=( void*)({ struct
_tuple16* _temp2956=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp2956->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp2956->f2= 0; _temp2956;}); _temp2955->tl=
fields; _temp2955;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des != 0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple17 _temp2959;
void* _temp2960; struct Cyc_List_List* _temp2962; struct _tuple17* _temp2957=(
struct _tuple17*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp2959=*
_temp2957; _LL2963: _temp2962= _temp2959.f1; goto _LL2961; _LL2961: _temp2960=
_temp2959.f2; goto _LL2958; _LL2958: if( _temp2962 == 0){ struct Cyc_List_List*
_temp2964= fields; goto _LL2965; _LL2965: for( 0; _temp2964 != 0; _temp2964=((
struct Cyc_List_List*) _check_null( _temp2964))->tl){ if( !(*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( _temp2964))->hd)).f2){(*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( _temp2964))->hd)).f2= 1;(*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( des))->hd)).f1=({ struct Cyc_List_List*
_temp2966=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2966->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2967=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2967[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2968; _temp2968.tag= Cyc_Absyn_FieldName;
_temp2968.f1=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2964))->hd)).f1)->name; _temp2968;}); _temp2967;})); _temp2966->tl= 0;
_temp2966;}); ans=({ struct Cyc_List_List* _temp2969=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2969->hd=( void*)({
struct _tuple18* _temp2970=( struct _tuple18*) _region_malloc( rgn, sizeof(
struct _tuple18)); _temp2970->f1=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2964))->hd)).f1; _temp2970->f2= _temp2960; _temp2970;});
_temp2969->tl= ans; _temp2969;}); break;}} if( _temp2964 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2971="too many arguments to struct"; struct
_tagged_string _temp2972; _temp2972.curr= _temp2971; _temp2972.base= _temp2971;
_temp2972.last_plus_one= _temp2971 + 29; _temp2972;}));}} else{ if((( struct Cyc_List_List*)
_check_null( _temp2962))->tl != 0){ Cyc_Tcutil_terr( loc,({ unsigned char*
_temp2973="multiple designators are not supported"; struct _tagged_string
_temp2974; _temp2974.curr= _temp2973; _temp2974.base= _temp2973; _temp2974.last_plus_one=
_temp2973 + 39; _temp2974;}));} else{ void* _temp2975=( void*)(( struct Cyc_List_List*)
_check_null( _temp2962))->hd; struct Cyc_Absyn_Exp* _temp2981; struct
_tagged_string* _temp2983; _LL2977: if(*(( int*) _temp2975) == Cyc_Absyn_ArrayElement){
_LL2982: _temp2981=(( struct Cyc_Absyn_ArrayElement_struct*) _temp2975)->f1;
goto _LL2978;} else{ goto _LL2979;} _LL2979: if(*(( int*) _temp2975) == Cyc_Absyn_FieldName){
_LL2984: _temp2983=(( struct Cyc_Absyn_FieldName_struct*) _temp2975)->f1; goto
_LL2980;} else{ goto _LL2976;} _LL2978: Cyc_Tcutil_terr( loc,({ unsigned char*
_temp2985="array designator used in argument to struct"; struct _tagged_string
_temp2986; _temp2986.curr= _temp2985; _temp2986.base= _temp2985; _temp2986.last_plus_one=
_temp2985 + 44; _temp2986;})); goto _LL2976; _LL2980: { struct Cyc_List_List*
_temp2987= fields; goto _LL2988; _LL2988: for( 0; _temp2987 != 0; _temp2987=((
struct Cyc_List_List*) _check_null( _temp2987))->tl){ if( Cyc_String_zstrptrcmp(
_temp2983,((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2987))->hd)).f1)->name) == 0){ if((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2987))->hd)).f2){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2989=* _temp2983; xprintf("field %.*s has already been used as an argument",
_temp2989.last_plus_one - _temp2989.curr, _temp2989.curr);}));}(*(( struct
_tuple16*)(( struct Cyc_List_List*) _check_null( _temp2987))->hd)).f2= 1; ans=({
struct Cyc_List_List* _temp2990=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2990->hd=( void*)({ struct _tuple18*
_temp2991=( struct _tuple18*) _region_malloc( rgn, sizeof( struct _tuple18));
_temp2991->f1=(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2987))->hd)).f1; _temp2991->f2= _temp2960; _temp2991;}); _temp2990->tl= ans;
_temp2990;}); break;}} if( _temp2987 == 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2992=* _temp2983; xprintf("bad field designator %.*s",
_temp2992.last_plus_one - _temp2992.curr, _temp2992.curr);}));} goto _LL2976;}
_LL2976:;}}} for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ if( !(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
fields))->hd)).f2){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2993="too few arguments to struct";
struct _tagged_string _temp2994; _temp2994.curr= _temp2993; _temp2994.base=
_temp2993; _temp2994.last_plus_one= _temp2993 + 28; _temp2994;})); break;}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);}} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* _temp2995= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp3001; struct Cyc_Absyn_Conref* _temp3003;
struct Cyc_Absyn_Tqual _temp3005; struct Cyc_Absyn_Conref* _temp3007; void*
_temp3009; void* _temp3011; _LL2997: if(( unsigned int) _temp2995 > 4u?*(( int*)
_temp2995) == Cyc_Absyn_PointerType: 0){ _LL3002: _temp3001=(( struct Cyc_Absyn_PointerType_struct*)
_temp2995)->f1; _LL3012: _temp3011=( void*) _temp3001.elt_typ; goto _LL3010;
_LL3010: _temp3009=( void*) _temp3001.rgn_typ; goto _LL3008; _LL3008: _temp3007=
_temp3001.nullable; goto _LL3006; _LL3006: _temp3005= _temp3001.tq; goto _LL3004;
_LL3004: _temp3003= _temp3001.bounds; goto _LL2998;} else{ goto _LL2999;}
_LL2999: goto _LL3000; _LL2998: { struct Cyc_Absyn_Conref* _temp3013=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp3003); goto _LL3014; _LL3014: { void* _temp3015=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3013))->v; void*
_temp3023; _LL3017: if(( unsigned int) _temp3015 > 1u?*(( int*) _temp3015) ==
Cyc_Absyn_Eq_constr: 0){ _LL3024: _temp3023=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3015)->f1; if( _temp3023 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3018;}
else{ goto _LL3019;}} else{ goto _LL3019;} _LL3019: if( _temp3015 ==( void*) Cyc_Absyn_No_constr){
goto _LL3020;} else{ goto _LL3021;} _LL3021: goto _LL3022; _LL3018: return 1;
_LL3020:( void*)( _temp3013->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3025=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3025[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3026; _temp3026.tag= Cyc_Absyn_Eq_constr;
_temp3026.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3026;}); _temp3025;})));
return 1; _LL3022: return 0; _LL3016:;}} _LL3000: return 0; _LL2996:;} struct
_tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple6 bogus_ans=({ struct _tuple6 _temp3265; _temp3265.f1= 0;
_temp3265.f2=( void*) Cyc_Absyn_HeapRgn; _temp3265;}); void* _temp3027=( void*)
e->r; void* _temp3041; struct _tuple1* _temp3043; struct _tagged_string*
_temp3045; struct Cyc_Absyn_Exp* _temp3047; struct _tagged_string* _temp3049;
struct Cyc_Absyn_Exp* _temp3051; struct Cyc_Absyn_Exp* _temp3053; struct Cyc_Absyn_Exp*
_temp3055; struct Cyc_Absyn_Exp* _temp3057; _LL3029: if(*(( int*) _temp3027) ==
Cyc_Absyn_Var_e){ _LL3044: _temp3043=(( struct Cyc_Absyn_Var_e_struct*)
_temp3027)->f1; goto _LL3042; _LL3042: _temp3041=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3027)->f2; goto _LL3030;} else{ goto _LL3031;} _LL3031: if(*(( int*)
_temp3027) == Cyc_Absyn_StructMember_e){ _LL3048: _temp3047=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3027)->f1; goto _LL3046; _LL3046: _temp3045=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3027)->f2; goto _LL3032;} else{ goto _LL3033;} _LL3033: if(*(( int*)
_temp3027) == Cyc_Absyn_StructArrow_e){ _LL3052: _temp3051=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3027)->f1; goto _LL3050; _LL3050: _temp3049=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3027)->f2; goto _LL3034;} else{ goto _LL3035;} _LL3035: if(*(( int*)
_temp3027) == Cyc_Absyn_Deref_e){ _LL3054: _temp3053=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3027)->f1; goto _LL3036;} else{ goto _LL3037;} _LL3037: if(*(( int*)
_temp3027) == Cyc_Absyn_Subscript_e){ _LL3058: _temp3057=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3027)->f1; goto _LL3056; _LL3056: _temp3055=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3027)->f2; goto _LL3038;} else{ goto _LL3039;} _LL3039: goto _LL3040;
_LL3030: { void* _temp3059= _temp3041; struct Cyc_Absyn_Fndecl* _temp3073;
struct Cyc_Absyn_Vardecl* _temp3075; struct Cyc_Absyn_Vardecl* _temp3077; struct
Cyc_Absyn_Vardecl* _temp3079; struct Cyc_Absyn_Vardecl* _temp3081; _LL3061: if(
_temp3059 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL3062;} else{ goto _LL3063;}
_LL3063: if(( unsigned int) _temp3059 > 1u?*(( int*) _temp3059) == Cyc_Absyn_Funname_b:
0){ _LL3074: _temp3073=(( struct Cyc_Absyn_Funname_b_struct*) _temp3059)->f1;
goto _LL3064;} else{ goto _LL3065;} _LL3065: if(( unsigned int) _temp3059 > 1u?*((
int*) _temp3059) == Cyc_Absyn_Global_b: 0){ _LL3076: _temp3075=(( struct Cyc_Absyn_Global_b_struct*)
_temp3059)->f1; goto _LL3066;} else{ goto _LL3067;} _LL3067: if(( unsigned int)
_temp3059 > 1u?*(( int*) _temp3059) == Cyc_Absyn_Local_b: 0){ _LL3078: _temp3077=((
struct Cyc_Absyn_Local_b_struct*) _temp3059)->f1; goto _LL3068;} else{ goto
_LL3069;} _LL3069: if(( unsigned int) _temp3059 > 1u?*(( int*) _temp3059) == Cyc_Absyn_Pat_b:
0){ _LL3080: _temp3079=(( struct Cyc_Absyn_Pat_b_struct*) _temp3059)->f1; goto
_LL3070;} else{ goto _LL3071;} _LL3071: if(( unsigned int) _temp3059 > 1u?*((
int*) _temp3059) == Cyc_Absyn_Param_b: 0){ _LL3082: _temp3081=(( struct Cyc_Absyn_Param_b_struct*)
_temp3059)->f1; goto _LL3072;} else{ goto _LL3060;} _LL3062: return bogus_ans;
_LL3064: return({ struct _tuple6 _temp3083; _temp3083.f1= 1; _temp3083.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3083;}); _LL3066: { void* _temp3084= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3090; struct Cyc_Absyn_Tqual _temp3092; void* _temp3094; _LL3086: if((
unsigned int) _temp3084 > 4u?*(( int*) _temp3084) == Cyc_Absyn_ArrayType: 0){
_LL3095: _temp3094=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3084)->f1;
goto _LL3093; _LL3093: _temp3092=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3084)->f2; goto _LL3091; _LL3091: _temp3090=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3084)->f3; goto _LL3087;} else{ goto _LL3088;} _LL3088: goto _LL3089;
_LL3087: return({ struct _tuple6 _temp3096; _temp3096.f1= 1; _temp3096.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3096;}); _LL3089: return({ struct _tuple6 _temp3097;
_temp3097.f1=( _temp3075->tq).q_const; _temp3097.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3097;}); _LL3085:;} _LL3068: { void* _temp3098= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3104; struct Cyc_Absyn_Tqual _temp3106; void* _temp3108; _LL3100: if((
unsigned int) _temp3098 > 4u?*(( int*) _temp3098) == Cyc_Absyn_ArrayType: 0){
_LL3109: _temp3108=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3098)->f1;
goto _LL3107; _LL3107: _temp3106=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3098)->f2; goto _LL3105; _LL3105: _temp3104=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3098)->f3; goto _LL3101;} else{ goto _LL3102;} _LL3102: goto _LL3103;
_LL3101: return({ struct _tuple6 _temp3110; _temp3110.f1= 1; _temp3110.f2=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3077->rgn))->v; _temp3110;}); _LL3103:
return({ struct _tuple6 _temp3111; _temp3111.f1=( _temp3077->tq).q_const;
_temp3111.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3077->rgn))->v;
_temp3111;}); _LL3099:;} _LL3070: _temp3081= _temp3079; goto _LL3072; _LL3072:
return({ struct _tuple6 _temp3112; _temp3112.f1=( _temp3081->tq).q_const;
_temp3112.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3081->rgn))->v;
_temp3112;}); _LL3060:;} _LL3032: { void* _temp3113= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3047->topt))->v); struct Cyc_List_List*
_temp3125; struct Cyc_List_List* _temp3127; struct Cyc_Absyn_Structdecl**
_temp3129; struct Cyc_Absyn_Structdecl* _temp3131; struct Cyc_List_List*
_temp3132; struct _tuple1* _temp3134; struct Cyc_Absyn_Uniondecl** _temp3136;
struct Cyc_Absyn_Uniondecl* _temp3138; struct Cyc_List_List* _temp3139; struct
_tuple1* _temp3141; _LL3115: if(( unsigned int) _temp3113 > 4u?*(( int*)
_temp3113) == Cyc_Absyn_AnonStructType: 0){ _LL3126: _temp3125=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3113)->f1; goto _LL3116;} else{ goto _LL3117;} _LL3117: if(( unsigned int)
_temp3113 > 4u?*(( int*) _temp3113) == Cyc_Absyn_AnonUnionType: 0){ _LL3128:
_temp3127=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3113)->f1; goto
_LL3118;} else{ goto _LL3119;} _LL3119: if(( unsigned int) _temp3113 > 4u?*((
int*) _temp3113) == Cyc_Absyn_StructType: 0){ _LL3135: _temp3134=(( struct Cyc_Absyn_StructType_struct*)
_temp3113)->f1; goto _LL3133; _LL3133: _temp3132=(( struct Cyc_Absyn_StructType_struct*)
_temp3113)->f2; goto _LL3130; _LL3130: _temp3129=(( struct Cyc_Absyn_StructType_struct*)
_temp3113)->f3; if( _temp3129 == 0){ goto _LL3121;} else{ _temp3131=* _temp3129;
goto _LL3120;}} else{ goto _LL3121;} _LL3121: if(( unsigned int) _temp3113 > 4u?*((
int*) _temp3113) == Cyc_Absyn_UnionType: 0){ _LL3142: _temp3141=(( struct Cyc_Absyn_UnionType_struct*)
_temp3113)->f1; goto _LL3140; _LL3140: _temp3139=(( struct Cyc_Absyn_UnionType_struct*)
_temp3113)->f2; goto _LL3137; _LL3137: _temp3136=(( struct Cyc_Absyn_UnionType_struct*)
_temp3113)->f3; if( _temp3136 == 0){ goto _LL3123;} else{ _temp3138=* _temp3136;
goto _LL3122;}} else{ goto _LL3123;} _LL3123: goto _LL3124; _LL3116: _temp3127=
_temp3125; goto _LL3118; _LL3118: { struct Cyc_Absyn_Structfield* _temp3143= Cyc_Absyn_lookup_field(
_temp3127, _temp3045); goto _LL3144; _LL3144: if( _temp3143 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3143))->width != 0: 0){ return({ struct _tuple6 _temp3145;
_temp3145.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3143))->tq).q_const;
_temp3145.f2=( Cyc_Tcutil_addressof_props( te, _temp3047)).f2; _temp3145;});}
return bogus_ans;} _LL3120: { struct Cyc_Absyn_Structfield* _temp3146= Cyc_Absyn_lookup_struct_field(
_temp3131, _temp3045); goto _LL3147; _LL3147: if( _temp3146 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3146))->width != 0: 0){ return({ struct _tuple6 _temp3148;
_temp3148.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3146))->tq).q_const;
_temp3148.f2=( Cyc_Tcutil_addressof_props( te, _temp3047)).f2; _temp3148;});}
return bogus_ans;} _LL3122: { struct Cyc_Absyn_Structfield* _temp3149= Cyc_Absyn_lookup_union_field(
_temp3138, _temp3045); goto _LL3150; _LL3150: if( _temp3149 != 0){ return({
struct _tuple6 _temp3151; _temp3151.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3149))->tq).q_const; _temp3151.f2=( Cyc_Tcutil_addressof_props(
te, _temp3047)).f2; _temp3151;});} goto _LL3124;} _LL3124: return bogus_ans;
_LL3114:;} _LL3034: { void* _temp3152= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3051->topt))->v); struct Cyc_Absyn_PtrInfo _temp3158; struct
Cyc_Absyn_Conref* _temp3160; struct Cyc_Absyn_Tqual _temp3162; struct Cyc_Absyn_Conref*
_temp3164; void* _temp3166; void* _temp3168; _LL3154: if(( unsigned int)
_temp3152 > 4u?*(( int*) _temp3152) == Cyc_Absyn_PointerType: 0){ _LL3159:
_temp3158=(( struct Cyc_Absyn_PointerType_struct*) _temp3152)->f1; _LL3169:
_temp3168=( void*) _temp3158.elt_typ; goto _LL3167; _LL3167: _temp3166=( void*)
_temp3158.rgn_typ; goto _LL3165; _LL3165: _temp3164= _temp3158.nullable; goto
_LL3163; _LL3163: _temp3162= _temp3158.tq; goto _LL3161; _LL3161: _temp3160=
_temp3158.bounds; goto _LL3155;} else{ goto _LL3156;} _LL3156: goto _LL3157;
_LL3155: { void* _temp3170= Cyc_Tcutil_compress( _temp3168); struct Cyc_List_List*
_temp3182; struct Cyc_List_List* _temp3184; struct Cyc_Absyn_Structdecl**
_temp3186; struct Cyc_Absyn_Structdecl* _temp3188; struct Cyc_List_List*
_temp3189; struct _tuple1* _temp3191; struct Cyc_Absyn_Uniondecl** _temp3193;
struct Cyc_Absyn_Uniondecl* _temp3195; struct Cyc_List_List* _temp3196; struct
_tuple1* _temp3198; _LL3172: if(( unsigned int) _temp3170 > 4u?*(( int*)
_temp3170) == Cyc_Absyn_AnonStructType: 0){ _LL3183: _temp3182=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3170)->f1; goto _LL3173;} else{ goto _LL3174;} _LL3174: if(( unsigned int)
_temp3170 > 4u?*(( int*) _temp3170) == Cyc_Absyn_AnonUnionType: 0){ _LL3185:
_temp3184=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3170)->f1; goto
_LL3175;} else{ goto _LL3176;} _LL3176: if(( unsigned int) _temp3170 > 4u?*((
int*) _temp3170) == Cyc_Absyn_StructType: 0){ _LL3192: _temp3191=(( struct Cyc_Absyn_StructType_struct*)
_temp3170)->f1; goto _LL3190; _LL3190: _temp3189=(( struct Cyc_Absyn_StructType_struct*)
_temp3170)->f2; goto _LL3187; _LL3187: _temp3186=(( struct Cyc_Absyn_StructType_struct*)
_temp3170)->f3; if( _temp3186 == 0){ goto _LL3178;} else{ _temp3188=* _temp3186;
goto _LL3177;}} else{ goto _LL3178;} _LL3178: if(( unsigned int) _temp3170 > 4u?*((
int*) _temp3170) == Cyc_Absyn_UnionType: 0){ _LL3199: _temp3198=(( struct Cyc_Absyn_UnionType_struct*)
_temp3170)->f1; goto _LL3197; _LL3197: _temp3196=(( struct Cyc_Absyn_UnionType_struct*)
_temp3170)->f2; goto _LL3194; _LL3194: _temp3193=(( struct Cyc_Absyn_UnionType_struct*)
_temp3170)->f3; if( _temp3193 == 0){ goto _LL3180;} else{ _temp3195=* _temp3193;
goto _LL3179;}} else{ goto _LL3180;} _LL3180: goto _LL3181; _LL3173: _temp3184=
_temp3182; goto _LL3175; _LL3175: { struct Cyc_Absyn_Structfield* _temp3200= Cyc_Absyn_lookup_field(
_temp3184, _temp3049); goto _LL3201; _LL3201: if( _temp3200 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3200))->width != 0: 0){ return({ struct _tuple6 _temp3202;
_temp3202.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3200))->tq).q_const;
_temp3202.f2= _temp3166; _temp3202;});} return bogus_ans;} _LL3177: { struct Cyc_Absyn_Structfield*
_temp3203= Cyc_Absyn_lookup_struct_field( _temp3188, _temp3049); goto _LL3204;
_LL3204: if( _temp3203 != 0?(( struct Cyc_Absyn_Structfield*) _check_null(
_temp3203))->width != 0: 0){ return({ struct _tuple6 _temp3205; _temp3205.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3203))->tq).q_const; _temp3205.f2=
_temp3166; _temp3205;});} return bogus_ans;} _LL3179: { struct Cyc_Absyn_Structfield*
_temp3206= Cyc_Absyn_lookup_union_field( _temp3195, _temp3049); goto _LL3207;
_LL3207: if( _temp3206 != 0){ return({ struct _tuple6 _temp3208; _temp3208.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3206))->tq).q_const; _temp3208.f2=
_temp3166; _temp3208;});} return bogus_ans;} _LL3181: return bogus_ans; _LL3171:;}
_LL3157: return bogus_ans; _LL3153:;} _LL3036: { void* _temp3209= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp3053->topt))->v); struct Cyc_Absyn_PtrInfo
_temp3215; struct Cyc_Absyn_Conref* _temp3217; struct Cyc_Absyn_Tqual _temp3219;
struct Cyc_Absyn_Conref* _temp3221; void* _temp3223; void* _temp3225; _LL3211:
if(( unsigned int) _temp3209 > 4u?*(( int*) _temp3209) == Cyc_Absyn_PointerType:
0){ _LL3216: _temp3215=(( struct Cyc_Absyn_PointerType_struct*) _temp3209)->f1;
_LL3226: _temp3225=( void*) _temp3215.elt_typ; goto _LL3224; _LL3224: _temp3223=(
void*) _temp3215.rgn_typ; goto _LL3222; _LL3222: _temp3221= _temp3215.nullable;
goto _LL3220; _LL3220: _temp3219= _temp3215.tq; goto _LL3218; _LL3218: _temp3217=
_temp3215.bounds; goto _LL3212;} else{ goto _LL3213;} _LL3213: goto _LL3214;
_LL3212: return({ struct _tuple6 _temp3227; _temp3227.f1= _temp3219.q_const;
_temp3227.f2= _temp3223; _temp3227;}); _LL3214: return bogus_ans; _LL3210:;}
_LL3038: { void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3057->topt))->v); void* _temp3228= t; struct Cyc_Absyn_Exp*
_temp3238; struct Cyc_Absyn_Tqual _temp3240; void* _temp3242; struct Cyc_List_List*
_temp3244; struct Cyc_Absyn_PtrInfo _temp3246; struct Cyc_Absyn_Conref*
_temp3248; struct Cyc_Absyn_Tqual _temp3250; struct Cyc_Absyn_Conref* _temp3252;
void* _temp3254; void* _temp3256; _LL3230: if(( unsigned int) _temp3228 > 4u?*((
int*) _temp3228) == Cyc_Absyn_ArrayType: 0){ _LL3243: _temp3242=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3228)->f1; goto _LL3241; _LL3241:
_temp3240=(( struct Cyc_Absyn_ArrayType_struct*) _temp3228)->f2; goto _LL3239;
_LL3239: _temp3238=(( struct Cyc_Absyn_ArrayType_struct*) _temp3228)->f3; goto
_LL3231;} else{ goto _LL3232;} _LL3232: if(( unsigned int) _temp3228 > 4u?*((
int*) _temp3228) == Cyc_Absyn_TupleType: 0){ _LL3245: _temp3244=(( struct Cyc_Absyn_TupleType_struct*)
_temp3228)->f1; goto _LL3233;} else{ goto _LL3234;} _LL3234: if(( unsigned int)
_temp3228 > 4u?*(( int*) _temp3228) == Cyc_Absyn_PointerType: 0){ _LL3247:
_temp3246=(( struct Cyc_Absyn_PointerType_struct*) _temp3228)->f1; _LL3257:
_temp3256=( void*) _temp3246.elt_typ; goto _LL3255; _LL3255: _temp3254=( void*)
_temp3246.rgn_typ; goto _LL3253; _LL3253: _temp3252= _temp3246.nullable; goto
_LL3251; _LL3251: _temp3250= _temp3246.tq; goto _LL3249; _LL3249: _temp3248=
_temp3246.bounds; goto _LL3235;} else{ goto _LL3236;} _LL3236: goto _LL3237;
_LL3231: return({ struct _tuple6 _temp3258; _temp3258.f1= _temp3240.q_const;
_temp3258.f2=( Cyc_Tcutil_addressof_props( te, _temp3057)).f2; _temp3258;});
_LL3233: { struct _tuple4* _temp3259= Cyc_Absyn_lookup_tuple_field( _temp3244,(
int) Cyc_Evexp_eval_const_uint_exp( _temp3055)); goto _LL3260; _LL3260: if(
_temp3259 != 0){ return({ struct _tuple6 _temp3261; _temp3261.f1=((*(( struct
_tuple4*) _check_null( _temp3259))).f1).q_const; _temp3261.f2=( Cyc_Tcutil_addressof_props(
te, _temp3057)).f2; _temp3261;});} return bogus_ans;} _LL3235: return({ struct
_tuple6 _temp3262; _temp3262.f1= _temp3250.q_const; _temp3262.f2= _temp3254;
_temp3262;}); _LL3237: return bogus_ans; _LL3229:;} _LL3040: Cyc_Tcutil_terr( e->loc,({
unsigned char* _temp3263="unary & applied to non-lvalue"; struct _tagged_string
_temp3264; _temp3264.curr= _temp3263; _temp3264.base= _temp3263; _temp3264.last_plus_one=
_temp3263 + 30; _temp3264;})); return bogus_ans; _LL3028:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp3266= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Exp* _temp3272; struct
Cyc_Absyn_Tqual _temp3274; void* _temp3276; _LL3268: if(( unsigned int)
_temp3266 > 4u?*(( int*) _temp3266) == Cyc_Absyn_ArrayType: 0){ _LL3277:
_temp3276=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3266)->f1; goto
_LL3275; _LL3275: _temp3274=(( struct Cyc_Absyn_ArrayType_struct*) _temp3266)->f2;
goto _LL3273; _LL3273: _temp3272=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3266)->f3; goto _LL3269;} else{ goto _LL3270;} _LL3270: goto _LL3271;
_LL3269: { void* _temp3280; int _temp3282; struct _tuple6 _temp3278= Cyc_Tcutil_addressof_props(
te, e); _LL3283: _temp3282= _temp3278.f1; goto _LL3281; _LL3281: _temp3280=
_temp3278.f2; goto _LL3279; _LL3279: return Cyc_Absyn_atb_typ( _temp3276,
_temp3280, _temp3274,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3284=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3284[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3285; _temp3285.tag= Cyc_Absyn_Upper_b;
_temp3285.f1= e; _temp3285;}); _temp3284;}));} _LL3271: return e_typ; _LL3267:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( b);{ void* _temp3286=( void*) b->v; void*
_temp3294; void* _temp3296; struct Cyc_Absyn_Exp* _temp3298; _LL3288: if((
unsigned int) _temp3286 > 1u?*(( int*) _temp3286) == Cyc_Absyn_Eq_constr: 0){
_LL3295: _temp3294=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3286)->f1;
if( _temp3294 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3289;} else{ goto _LL3290;}}
else{ goto _LL3290;} _LL3290: if(( unsigned int) _temp3286 > 1u?*(( int*)
_temp3286) == Cyc_Absyn_Eq_constr: 0){ _LL3297: _temp3296=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3286)->f1; if(( unsigned int) _temp3296 > 1u?*(( int*) _temp3296) == Cyc_Absyn_Upper_b:
0){ _LL3299: _temp3298=(( struct Cyc_Absyn_Upper_b_struct*) _temp3296)->f1; goto
_LL3291;} else{ goto _LL3292;}} else{ goto _LL3292;} _LL3292: goto _LL3293;
_LL3289: return; _LL3291: if( Cyc_Evexp_eval_const_uint_exp( _temp3298) <= i){
Cyc_Tcutil_terr( loc,({ unsigned char* _temp3300="dereference is out of bounds";
struct _tagged_string _temp3301; _temp3301.curr= _temp3300; _temp3301.base=
_temp3300; _temp3301.last_plus_one= _temp3300 + 29; _temp3301;}));} return;
_LL3293:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3302=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3302[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3303; _temp3303.tag= Cyc_Absyn_Eq_constr;
_temp3303.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3304=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3304[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3305; _temp3305.tag= Cyc_Absyn_Upper_b;
_temp3305.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3305;}); _temp3304;}));
_temp3303;}); _temp3302;}))); return; _LL3287:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3306= Cyc_Tcutil_compress(
t); void* _temp3332; void* _temp3334; struct Cyc_Absyn_Enumdecl* _temp3336;
struct _tuple1* _temp3338; struct Cyc_Absyn_Exp* _temp3340; struct Cyc_Absyn_Tqual
_temp3342; void* _temp3344; struct Cyc_List_List* _temp3346; struct Cyc_Absyn_Structdecl**
_temp3348; struct Cyc_List_List* _temp3350; struct _tuple1* _temp3352; struct
Cyc_Absyn_Uniondecl** _temp3354; struct Cyc_List_List* _temp3356; struct _tuple1*
_temp3358; struct Cyc_List_List* _temp3360; struct Cyc_List_List* _temp3362;
_LL3308: if( _temp3306 ==( void*) Cyc_Absyn_VoidType){ goto _LL3309;} else{ goto
_LL3310;} _LL3310: if(( unsigned int) _temp3306 > 4u?*(( int*) _temp3306) == Cyc_Absyn_IntType:
0){ _LL3335: _temp3334=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3306)->f1;
goto _LL3333; _LL3333: _temp3332=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3306)->f2; goto _LL3311;} else{ goto _LL3312;} _LL3312: if( _temp3306 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3313;} else{ goto _LL3314;} _LL3314: if(
_temp3306 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3315;} else{ goto _LL3316;}
_LL3316: if(( unsigned int) _temp3306 > 4u?*(( int*) _temp3306) == Cyc_Absyn_EnumType:
0){ _LL3339: _temp3338=(( struct Cyc_Absyn_EnumType_struct*) _temp3306)->f1;
goto _LL3337; _LL3337: _temp3336=(( struct Cyc_Absyn_EnumType_struct*) _temp3306)->f2;
goto _LL3317;} else{ goto _LL3318;} _LL3318: if(( unsigned int) _temp3306 > 4u?*((
int*) _temp3306) == Cyc_Absyn_ArrayType: 0){ _LL3345: _temp3344=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3306)->f1; goto _LL3343; _LL3343:
_temp3342=(( struct Cyc_Absyn_ArrayType_struct*) _temp3306)->f2; goto _LL3341;
_LL3341: _temp3340=(( struct Cyc_Absyn_ArrayType_struct*) _temp3306)->f3; goto
_LL3319;} else{ goto _LL3320;} _LL3320: if(( unsigned int) _temp3306 > 4u?*((
int*) _temp3306) == Cyc_Absyn_TupleType: 0){ _LL3347: _temp3346=(( struct Cyc_Absyn_TupleType_struct*)
_temp3306)->f1; goto _LL3321;} else{ goto _LL3322;} _LL3322: if(( unsigned int)
_temp3306 > 4u?*(( int*) _temp3306) == Cyc_Absyn_StructType: 0){ _LL3353:
_temp3352=(( struct Cyc_Absyn_StructType_struct*) _temp3306)->f1; goto _LL3351;
_LL3351: _temp3350=(( struct Cyc_Absyn_StructType_struct*) _temp3306)->f2; goto
_LL3349; _LL3349: _temp3348=(( struct Cyc_Absyn_StructType_struct*) _temp3306)->f3;
goto _LL3323;} else{ goto _LL3324;} _LL3324: if(( unsigned int) _temp3306 > 4u?*((
int*) _temp3306) == Cyc_Absyn_UnionType: 0){ _LL3359: _temp3358=(( struct Cyc_Absyn_UnionType_struct*)
_temp3306)->f1; goto _LL3357; _LL3357: _temp3356=(( struct Cyc_Absyn_UnionType_struct*)
_temp3306)->f2; goto _LL3355; _LL3355: _temp3354=(( struct Cyc_Absyn_UnionType_struct*)
_temp3306)->f3; goto _LL3325;} else{ goto _LL3326;} _LL3326: if(( unsigned int)
_temp3306 > 4u?*(( int*) _temp3306) == Cyc_Absyn_AnonStructType: 0){ _LL3361:
_temp3360=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3306)->f1; goto
_LL3327;} else{ goto _LL3328;} _LL3328: if(( unsigned int) _temp3306 > 4u?*((
int*) _temp3306) == Cyc_Absyn_AnonUnionType: 0){ _LL3363: _temp3362=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3306)->f1; goto _LL3329;} else{ goto
_LL3330;} _LL3330: goto _LL3331; _LL3309: return 1; _LL3311: return 1; _LL3313:
return 1; _LL3315: return 1; _LL3317: return 1; _LL3319: return Cyc_Tcutil_bits_only(
_temp3344); _LL3321: for( 0; _temp3346 != 0; _temp3346=(( struct Cyc_List_List*)
_check_null( _temp3346))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3346))->hd)).f2)){ return 0;}} return 1;
_LL3323: if( _temp3348 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3364=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3348)); goto _LL3365; _LL3365:
if( _temp3364->fields == 0){ return 0;}{ struct _RegionHandle _temp3366=
_new_region(); struct _RegionHandle* rgn=& _temp3366; _push_region( rgn);{
struct Cyc_List_List* _temp3367=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3364->tvs, _temp3350); goto _LL3368; _LL3368:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3364->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3367,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3369= 0; _npop_handler( 0u); return
_temp3369;}}}{ int _temp3370= 1; _npop_handler( 0u); return _temp3370;}};
_pop_region(& _temp3366);}} _LL3325: if( _temp3354 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3371=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3354)); goto
_LL3372; _LL3372: if( _temp3371->fields == 0){ return 0;}{ struct _RegionHandle
_temp3373= _new_region(); struct _RegionHandle* rgn=& _temp3373; _push_region(
rgn);{ struct Cyc_List_List* _temp3374=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3371->tvs, _temp3356); goto _LL3375; _LL3375:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3371->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3374,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3376= 0; _npop_handler( 0u); return
_temp3376;}}}{ int _temp3377= 1; _npop_handler( 0u); return _temp3377;}};
_pop_region(& _temp3373);}} _LL3327: _temp3362= _temp3360; goto _LL3329; _LL3329:
for( 0; _temp3362 != 0; _temp3362=(( struct Cyc_List_List*) _check_null(
_temp3362))->tl){ if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp3362))->hd)->type)){ return 0;}} return
1; _LL3331: return 0; _LL3307:;} struct _tuple19{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3378=( void*) e->r; void*
_temp3418; void* _temp3420; struct Cyc_Absyn_Exp* _temp3422; struct Cyc_Absyn_Enumfield*
_temp3424; struct Cyc_Absyn_Enumdecl* _temp3426; struct _tuple1* _temp3428; void*
_temp3430; struct _tuple1* _temp3432; struct Cyc_Absyn_Exp* _temp3434; struct
Cyc_Absyn_Exp* _temp3436; struct Cyc_Absyn_Exp* _temp3438; struct Cyc_Absyn_Exp*
_temp3440; struct Cyc_Absyn_Exp* _temp3442; struct Cyc_Absyn_Exp* _temp3444;
struct Cyc_List_List* _temp3446; struct Cyc_Absyn_Exp* _temp3448; struct Cyc_Absyn_Exp*
_temp3450; void* _temp3452; struct Cyc_Absyn_Exp* _temp3454; struct Cyc_Absyn_Exp*
_temp3456; struct Cyc_Absyn_Exp* _temp3458; struct Cyc_Absyn_Vardecl* _temp3460;
struct Cyc_List_List* _temp3462; struct Cyc_List_List* _temp3464; void*
_temp3466; struct Cyc_Absyn_Structdecl* _temp3468; struct Cyc_List_List*
_temp3470; struct Cyc_Core_Opt* _temp3472; struct _tuple1* _temp3474; struct Cyc_List_List*
_temp3476; void* _temp3478; struct Cyc_List_List* _temp3480; struct Cyc_Absyn_Tunionfield*
_temp3482; struct Cyc_Absyn_Tuniondecl* _temp3484; struct Cyc_List_List*
_temp3486; struct Cyc_Core_Opt* _temp3488; struct Cyc_Core_Opt* _temp3490;
_LL3380: if(*(( int*) _temp3378) == Cyc_Absyn_Const_e){ _LL3419: _temp3418=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp3378)->f1; goto _LL3381;} else{
goto _LL3382;} _LL3382: if(*(( int*) _temp3378) == Cyc_Absyn_Sizeoftyp_e){
_LL3421: _temp3420=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp3378)->f1;
goto _LL3383;} else{ goto _LL3384;} _LL3384: if(*(( int*) _temp3378) == Cyc_Absyn_Sizeofexp_e){
_LL3423: _temp3422=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp3378)->f1; goto
_LL3385;} else{ goto _LL3386;} _LL3386: if(*(( int*) _temp3378) == Cyc_Absyn_Enum_e){
_LL3429: _temp3428=(( struct Cyc_Absyn_Enum_e_struct*) _temp3378)->f1; goto
_LL3427; _LL3427: _temp3426=(( struct Cyc_Absyn_Enum_e_struct*) _temp3378)->f2;
goto _LL3425; _LL3425: _temp3424=(( struct Cyc_Absyn_Enum_e_struct*) _temp3378)->f3;
goto _LL3387;} else{ goto _LL3388;} _LL3388: if(*(( int*) _temp3378) == Cyc_Absyn_Var_e){
_LL3433: _temp3432=(( struct Cyc_Absyn_Var_e_struct*) _temp3378)->f1; goto
_LL3431; _LL3431: _temp3430=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp3378)->f2;
goto _LL3389;} else{ goto _LL3390;} _LL3390: if(*(( int*) _temp3378) == Cyc_Absyn_Conditional_e){
_LL3439: _temp3438=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3378)->f1;
goto _LL3437; _LL3437: _temp3436=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3378)->f2; goto _LL3435; _LL3435: _temp3434=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3378)->f3; goto _LL3391;} else{ goto _LL3392;} _LL3392: if(*(( int*)
_temp3378) == Cyc_Absyn_SeqExp_e){ _LL3443: _temp3442=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3378)->f1; goto _LL3441; _LL3441: _temp3440=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3378)->f2; goto _LL3393;} else{ goto _LL3394;} _LL3394: if(*(( int*)
_temp3378) == Cyc_Absyn_NoInstantiate_e){ _LL3445: _temp3444=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3378)->f1; goto _LL3395;} else{ goto _LL3396;} _LL3396: if(*(( int*)
_temp3378) == Cyc_Absyn_Instantiate_e){ _LL3449: _temp3448=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3378)->f1; goto _LL3447; _LL3447: _temp3446=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3378)->f2; goto _LL3397;} else{ goto _LL3398;} _LL3398: if(*(( int*)
_temp3378) == Cyc_Absyn_Cast_e){ _LL3453: _temp3452=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3378)->f1; goto _LL3451; _LL3451: _temp3450=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3378)->f2; goto _LL3399;} else{ goto _LL3400;} _LL3400: if(*(( int*)
_temp3378) == Cyc_Absyn_Address_e){ _LL3455: _temp3454=(( struct Cyc_Absyn_Address_e_struct*)
_temp3378)->f1; goto _LL3401;} else{ goto _LL3402;} _LL3402: if(*(( int*)
_temp3378) == Cyc_Absyn_Comprehension_e){ _LL3461: _temp3460=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3378)->f1; goto _LL3459; _LL3459: _temp3458=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3378)->f2; goto _LL3457; _LL3457: _temp3456=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3378)->f3; goto _LL3403;} else{ goto _LL3404;} _LL3404: if(*(( int*)
_temp3378) == Cyc_Absyn_Array_e){ _LL3463: _temp3462=(( struct Cyc_Absyn_Array_e_struct*)
_temp3378)->f1; goto _LL3405;} else{ goto _LL3406;} _LL3406: if(*(( int*)
_temp3378) == Cyc_Absyn_AnonStruct_e){ _LL3467: _temp3466=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3378)->f1; goto _LL3465; _LL3465: _temp3464=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3378)->f2; goto _LL3407;} else{ goto _LL3408;} _LL3408: if(*(( int*)
_temp3378) == Cyc_Absyn_Struct_e){ _LL3475: _temp3474=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3378)->f1; goto _LL3473; _LL3473: _temp3472=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3378)->f2; goto _LL3471; _LL3471: _temp3470=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3378)->f3; goto _LL3469; _LL3469: _temp3468=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3378)->f4; goto _LL3409;} else{ goto _LL3410;} _LL3410: if(*(( int*)
_temp3378) == Cyc_Absyn_Primop_e){ _LL3479: _temp3478=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3378)->f1; goto _LL3477; _LL3477: _temp3476=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3378)->f2; goto _LL3411;} else{ goto _LL3412;} _LL3412: if(*(( int*)
_temp3378) == Cyc_Absyn_Tuple_e){ _LL3481: _temp3480=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3378)->f1; goto _LL3413;} else{ goto _LL3414;} _LL3414: if(*(( int*)
_temp3378) == Cyc_Absyn_Tunion_e){ _LL3491: _temp3490=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3378)->f1; goto _LL3489; _LL3489: _temp3488=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3378)->f2; goto _LL3487; _LL3487: _temp3486=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3378)->f3; goto _LL3485; _LL3485: _temp3484=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3378)->f4; goto _LL3483; _LL3483: _temp3482=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3378)->f5; goto _LL3415;} else{ goto _LL3416;} _LL3416: goto _LL3417;
_LL3381: return 1; _LL3383: return 1; _LL3385: return Cyc_Tcutil_cnst_exp( te, 1,
_temp3422); _LL3387: return 1; _LL3389: { struct _handler_cons _temp3492;
_push_handler(& _temp3492);{ int _temp3494= 0; if( setjmp( _temp3492.handler)){
_temp3494= 1;} if( ! _temp3494){{ void* _temp3495= Cyc_Tcenv_lookup_ordinary( te,
e->loc, _temp3432); void* _temp3503; struct Cyc_Absyn_Fndecl* _temp3505; void*
_temp3507; struct Cyc_Absyn_Vardecl* _temp3509; _LL3497: if(*(( int*) _temp3495)
== Cyc_Tcenv_VarRes){ _LL3504: _temp3503=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3495)->f1; if(( unsigned int) _temp3503 > 1u?*(( int*) _temp3503) == Cyc_Absyn_Funname_b:
0){ _LL3506: _temp3505=(( struct Cyc_Absyn_Funname_b_struct*) _temp3503)->f1;
goto _LL3498;} else{ goto _LL3499;}} else{ goto _LL3499;} _LL3499: if(*(( int*)
_temp3495) == Cyc_Tcenv_VarRes){ _LL3508: _temp3507=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3495)->f1; if(( unsigned int) _temp3507 > 1u?*(( int*) _temp3507) == Cyc_Absyn_Global_b:
0){ _LL3510: _temp3509=(( struct Cyc_Absyn_Global_b_struct*) _temp3507)->f1;
goto _LL3500;} else{ goto _LL3501;}} else{ goto _LL3501;} _LL3501: goto _LL3502;
_LL3498: { int _temp3511= 1; _npop_handler( 0u); return _temp3511;} _LL3500: {
void* _temp3512= Cyc_Tcutil_compress(( void*) _temp3509->type); struct Cyc_Absyn_Exp*
_temp3518; struct Cyc_Absyn_Tqual _temp3520; void* _temp3522; _LL3514: if((
unsigned int) _temp3512 > 4u?*(( int*) _temp3512) == Cyc_Absyn_ArrayType: 0){
_LL3523: _temp3522=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3512)->f1;
goto _LL3521; _LL3521: _temp3520=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3512)->f2; goto _LL3519; _LL3519: _temp3518=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3512)->f3; goto _LL3515;} else{ goto _LL3516;} _LL3516: goto _LL3517;
_LL3515: { int _temp3524= 1; _npop_handler( 0u); return _temp3524;} _LL3517: {
int _temp3525= var_okay; _npop_handler( 0u); return _temp3525;} _LL3513:;}
_LL3502: { int _temp3526= var_okay; _npop_handler( 0u); return _temp3526;}
_LL3496:;}; _pop_handler();} else{ void* _temp3493=( void*) _exn_thrown; void*
_temp3528= _temp3493; _LL3530: if( _temp3528 == Cyc_Dict_Absent){ goto _LL3531;}
else{ goto _LL3532;} _LL3532: goto _LL3533; _LL3531: return 0; _LL3533:( void)
_throw( _temp3528); _LL3529:;}}} _LL3391: return( Cyc_Tcutil_cnst_exp( te, 0,
_temp3438)? Cyc_Tcutil_cnst_exp( te, 0, _temp3436): 0)? Cyc_Tcutil_cnst_exp( te,
0, _temp3434): 0; _LL3393: return Cyc_Tcutil_cnst_exp( te, 0, _temp3442)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3440): 0; _LL3395: _temp3448= _temp3444; goto _LL3397; _LL3397:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3448); _LL3399: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3450); _LL3401: return Cyc_Tcutil_cnst_exp( te, 1, _temp3454);
_LL3403: return Cyc_Tcutil_cnst_exp( te, 0, _temp3458)? Cyc_Tcutil_cnst_exp( te,
0, _temp3456): 0; _LL3405: _temp3464= _temp3462; goto _LL3407; _LL3407:
_temp3470= _temp3464; goto _LL3409; _LL3409: for( 0; _temp3470 != 0; _temp3470=((
struct Cyc_List_List*) _check_null( _temp3470))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple19*)(( struct Cyc_List_List*) _check_null( _temp3470))->hd)).f2)){
return 0;}} return 1; _LL3411: _temp3480= _temp3476; goto _LL3413; _LL3413:
_temp3486= _temp3480; goto _LL3415; _LL3415: for( 0; _temp3486 != 0; _temp3486=((
struct Cyc_List_List*) _check_null( _temp3486))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3486))->hd)){
return 0;}} return 1; _LL3417: return 0; _LL3379:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3534= Cyc_Tcutil_compress( t); int _temp3572; struct Cyc_Core_Opt*
_temp3574; void* _temp3576; struct Cyc_Absyn_Tvar* _temp3578; struct Cyc_Absyn_TunionInfo
_temp3580; struct Cyc_Absyn_TunionFieldInfo _temp3582; struct Cyc_Absyn_PtrInfo
_temp3584; struct Cyc_Absyn_Conref* _temp3586; struct Cyc_Absyn_Tqual _temp3588;
struct Cyc_Absyn_Conref* _temp3590; void* _temp3592; void* _temp3594; void*
_temp3596; void* _temp3598; struct Cyc_Absyn_Exp* _temp3600; struct Cyc_Absyn_Tqual
_temp3602; void* _temp3604; struct Cyc_Absyn_FnInfo _temp3606; struct Cyc_List_List*
_temp3608; struct Cyc_Absyn_Structdecl** _temp3610; struct Cyc_List_List*
_temp3612; struct _tuple1* _temp3614; struct Cyc_Absyn_Uniondecl** _temp3616;
struct Cyc_List_List* _temp3618; struct _tuple1* _temp3620; struct Cyc_List_List*
_temp3622; struct Cyc_List_List* _temp3624; struct Cyc_Absyn_Enumdecl* _temp3626;
struct _tuple1* _temp3628; _LL3536: if( _temp3534 ==( void*) Cyc_Absyn_VoidType){
goto _LL3537;} else{ goto _LL3538;} _LL3538: if(( unsigned int) _temp3534 > 4u?*((
int*) _temp3534) == Cyc_Absyn_Evar: 0){ _LL3577: _temp3576=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp3534)->f1; goto _LL3575; _LL3575: _temp3574=(( struct Cyc_Absyn_Evar_struct*)
_temp3534)->f2; goto _LL3573; _LL3573: _temp3572=(( struct Cyc_Absyn_Evar_struct*)
_temp3534)->f3; goto _LL3539;} else{ goto _LL3540;} _LL3540: if(( unsigned int)
_temp3534 > 4u?*(( int*) _temp3534) == Cyc_Absyn_VarType: 0){ _LL3579: _temp3578=((
struct Cyc_Absyn_VarType_struct*) _temp3534)->f1; goto _LL3541;} else{ goto
_LL3542;} _LL3542: if(( unsigned int) _temp3534 > 4u?*(( int*) _temp3534) == Cyc_Absyn_TunionType:
0){ _LL3581: _temp3580=(( struct Cyc_Absyn_TunionType_struct*) _temp3534)->f1;
goto _LL3543;} else{ goto _LL3544;} _LL3544: if(( unsigned int) _temp3534 > 4u?*((
int*) _temp3534) == Cyc_Absyn_TunionFieldType: 0){ _LL3583: _temp3582=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp3534)->f1; goto _LL3545;} else{ goto
_LL3546;} _LL3546: if(( unsigned int) _temp3534 > 4u?*(( int*) _temp3534) == Cyc_Absyn_PointerType:
0){ _LL3585: _temp3584=(( struct Cyc_Absyn_PointerType_struct*) _temp3534)->f1;
_LL3595: _temp3594=( void*) _temp3584.elt_typ; goto _LL3593; _LL3593: _temp3592=(
void*) _temp3584.rgn_typ; goto _LL3591; _LL3591: _temp3590= _temp3584.nullable;
goto _LL3589; _LL3589: _temp3588= _temp3584.tq; goto _LL3587; _LL3587: _temp3586=
_temp3584.bounds; goto _LL3547;} else{ goto _LL3548;} _LL3548: if(( unsigned int)
_temp3534 > 4u?*(( int*) _temp3534) == Cyc_Absyn_IntType: 0){ _LL3599: _temp3598=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp3534)->f1; goto _LL3597; _LL3597:
_temp3596=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3534)->f2; goto
_LL3549;} else{ goto _LL3550;} _LL3550: if( _temp3534 ==( void*) Cyc_Absyn_FloatType){
goto _LL3551;} else{ goto _LL3552;} _LL3552: if( _temp3534 ==( void*) Cyc_Absyn_DoubleType){
goto _LL3553;} else{ goto _LL3554;} _LL3554: if(( unsigned int) _temp3534 > 4u?*((
int*) _temp3534) == Cyc_Absyn_ArrayType: 0){ _LL3605: _temp3604=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3534)->f1; goto _LL3603; _LL3603:
_temp3602=(( struct Cyc_Absyn_ArrayType_struct*) _temp3534)->f2; goto _LL3601;
_LL3601: _temp3600=(( struct Cyc_Absyn_ArrayType_struct*) _temp3534)->f3; goto
_LL3555;} else{ goto _LL3556;} _LL3556: if(( unsigned int) _temp3534 > 4u?*((
int*) _temp3534) == Cyc_Absyn_FnType: 0){ _LL3607: _temp3606=(( struct Cyc_Absyn_FnType_struct*)
_temp3534)->f1; goto _LL3557;} else{ goto _LL3558;} _LL3558: if(( unsigned int)
_temp3534 > 4u?*(( int*) _temp3534) == Cyc_Absyn_TupleType: 0){ _LL3609:
_temp3608=(( struct Cyc_Absyn_TupleType_struct*) _temp3534)->f1; goto _LL3559;}
else{ goto _LL3560;} _LL3560: if(( unsigned int) _temp3534 > 4u?*(( int*)
_temp3534) == Cyc_Absyn_StructType: 0){ _LL3615: _temp3614=(( struct Cyc_Absyn_StructType_struct*)
_temp3534)->f1; goto _LL3613; _LL3613: _temp3612=(( struct Cyc_Absyn_StructType_struct*)
_temp3534)->f2; goto _LL3611; _LL3611: _temp3610=(( struct Cyc_Absyn_StructType_struct*)
_temp3534)->f3; goto _LL3561;} else{ goto _LL3562;} _LL3562: if(( unsigned int)
_temp3534 > 4u?*(( int*) _temp3534) == Cyc_Absyn_UnionType: 0){ _LL3621:
_temp3620=(( struct Cyc_Absyn_UnionType_struct*) _temp3534)->f1; goto _LL3619;
_LL3619: _temp3618=(( struct Cyc_Absyn_UnionType_struct*) _temp3534)->f2; goto
_LL3617; _LL3617: _temp3616=(( struct Cyc_Absyn_UnionType_struct*) _temp3534)->f3;
goto _LL3563;} else{ goto _LL3564;} _LL3564: if(( unsigned int) _temp3534 > 4u?*((
int*) _temp3534) == Cyc_Absyn_AnonStructType: 0){ _LL3623: _temp3622=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3534)->f1; goto _LL3565;} else{ goto
_LL3566;} _LL3566: if(( unsigned int) _temp3534 > 4u?*(( int*) _temp3534) == Cyc_Absyn_AnonUnionType:
0){ _LL3625: _temp3624=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3534)->f1;
goto _LL3567;} else{ goto _LL3568;} _LL3568: if(( unsigned int) _temp3534 > 4u?*((
int*) _temp3534) == Cyc_Absyn_EnumType: 0){ _LL3629: _temp3628=(( struct Cyc_Absyn_EnumType_struct*)
_temp3534)->f1; goto _LL3627; _LL3627: _temp3626=(( struct Cyc_Absyn_EnumType_struct*)
_temp3534)->f2; goto _LL3569;} else{ goto _LL3570;} _LL3570: goto _LL3571;
_LL3537: return 1; _LL3539: return 0; _LL3541: return 0; _LL3543: return 0;
_LL3545: return 0; _LL3547: { void* _temp3630=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3586))->v; void*
_temp3638; void* _temp3640; struct Cyc_Absyn_Exp* _temp3642; _LL3632: if((
unsigned int) _temp3630 > 1u?*(( int*) _temp3630) == Cyc_Absyn_Eq_constr: 0){
_LL3639: _temp3638=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3630)->f1;
if( _temp3638 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3633;} else{ goto _LL3634;}}
else{ goto _LL3634;} _LL3634: if(( unsigned int) _temp3630 > 1u?*(( int*)
_temp3630) == Cyc_Absyn_Eq_constr: 0){ _LL3641: _temp3640=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3630)->f1; if(( unsigned int) _temp3640 > 1u?*(( int*) _temp3640) == Cyc_Absyn_Upper_b:
0){ _LL3643: _temp3642=(( struct Cyc_Absyn_Upper_b_struct*) _temp3640)->f1; goto
_LL3635;} else{ goto _LL3636;}} else{ goto _LL3636;} _LL3636: goto _LL3637;
_LL3633: return 1; _LL3635: { void* _temp3644=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3590))->v; int
_temp3650; _LL3646: if(( unsigned int) _temp3644 > 1u?*(( int*) _temp3644) ==
Cyc_Absyn_Eq_constr: 0){ _LL3651: _temp3650=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3644)->f1; goto _LL3647;} else{ goto _LL3648;} _LL3648: goto _LL3649;
_LL3647: return _temp3650; _LL3649: return 0; _LL3645:;} _LL3637: return 0;
_LL3631:;} _LL3549: return 1; _LL3551: return 1; _LL3553: return 1; _LL3555:
return Cyc_Tcutil_supports_default( _temp3604); _LL3557: return 0; _LL3559: for(
0; _temp3608 != 0; _temp3608=(( struct Cyc_List_List*) _check_null( _temp3608))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3608))->hd)).f2)){ return 0;}} return 1; _LL3561: if(
_temp3610 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3610)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp3612,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3563: if( _temp3616 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3616)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3618,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3565: _temp3624= _temp3622; goto _LL3567; _LL3567: return Cyc_Tcutil_fields_support_default(
0, 0, _temp3624); _LL3569: return 1; _LL3571: return 0; _LL3535:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp3652= _new_region();
struct _RegionHandle* rgn=& _temp3652; _push_region( rgn);{ struct Cyc_List_List*
_temp3653=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); goto _LL3654; _LL3654: for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3653,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fs))->hd)->type);
if( ! Cyc_Tcutil_supports_default( t)){ int _temp3655= 0; _npop_handler( 0u);
return _temp3655;}}}; _pop_region(& _temp3652);} return 1;}