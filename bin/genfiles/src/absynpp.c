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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string(
int); extern char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure[ 12u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u]; extern char
Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char* tag;
struct _tagged_string f1; } ; extern struct _tagged_string Cyc_Core_string_of_int(
int); extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct
Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t;
typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void*
env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch[ 18u]; extern
void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern char Cyc_List_Nth[ 8u]; extern int Cyc_List_exists( int(* pred)( void*),
struct Cyc_List_List* x); extern int Cyc_List_list_cmp( int(* cmp)( void*, void*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2); extern int Cyc_List_list_prefix(
int(* cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2);
struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct
Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef
struct Cyc_PP_Doc* Cyc_PP_doc_t; extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc*
d, int w, struct Cyc_Stdio___sFILE* f); extern struct _tagged_string Cyc_PP_string_of_doc(
struct Cyc_PP_Doc* d, int w); extern struct Cyc_PP_Doc* Cyc_PP_text( struct
_tagged_string s); extern struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct Cyc_PP_Doc*
Cyc_PP_line_doc(); extern struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc*
d); extern struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2); extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc*
Cyc_PP_seq( struct _tagged_string sep, struct Cyc_List_List* l0); extern struct
Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_seql(
struct _tagged_string sep, struct Cyc_List_List* l0); struct Cyc_Lineno_Pos{
struct _tagged_string logical_file; struct _tagged_string line; int line_no; int
col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern int Cyc_Absyn_is_format_prim( void* p); extern struct _tagged_string
Cyc_Absyn_attribute2string( void*); struct Cyc_Buffer_t; typedef struct Cyc_Buffer_t*
Cyc_Buffer_T; extern int Cyc_String_strptrcmp( struct _tagged_string* s1, struct
_tagged_string* s2); extern struct _tagged_string Cyc_String_str_sepstr( struct
Cyc_List_List*, struct _tagged_string); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict*
Cyc_Dict_gdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct
Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u]; extern char
Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr[
12u]; extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*);
extern int Cyc_Absynpp_expand_typedefs; extern int Cyc_Absynpp_qvar_to_Cids;
extern int Cyc_Absynpp_to_VC; extern int Cyc_Absynpp_decls_first; extern int Cyc_Absynpp_print_all_tvars;
extern int Cyc_Absynpp_print_all_kinds; extern int Cyc_Absynpp_print_using_stmts;
extern int Cyc_Absynpp_print_externC_stmts; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); extern struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl* d); extern struct _tagged_string
Cyc_Absynpp_typ2string( void*); extern struct _tagged_string Cyc_Absynpp_kind2string(
void*); extern struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*);
extern struct _tagged_string Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); extern
struct _tagged_string Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl);
extern struct _tagged_string Cyc_Absynpp_prim2string( void* p); extern struct
_tagged_string Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p); extern struct
_tagged_string Cyc_Absynpp_scope2string( void* sc); extern int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_char_escape(
char); extern struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void* p);
extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); struct _tuple3{
struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; extern
struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct _tuple4{
struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ; extern struct
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t); extern struct
Cyc_PP_Doc* Cyc_Absynpp_egroup( struct _tagged_string start, struct
_tagged_string stop, struct _tagged_string sep, struct Cyc_List_List* ss);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat12( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat11(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_cat10( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_cat9( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat8(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat7( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat6( struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat5( struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat4( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat3(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_cat2( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; extern struct Cyc_PP_Doc*
Cyc_Absynpp_dp2doc( struct _tuple5* dp); extern struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs); extern struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List*
fields); extern struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*); extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e); extern struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct
_tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es); struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; extern struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de); extern struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields); int Cyc_Absynpp_expand_typedefs= 0; int Cyc_Absynpp_qvar_to_Cids=
0; int Cyc_Absynpp_to_VC= 0; int Cyc_Absynpp_decls_first= 0; int Cyc_Absynpp_dont_rewrite_temp_tvars=
0; int Cyc_Absynpp_print_all_tvars= 0; int Cyc_Absynpp_print_all_kinds= 0; int
Cyc_Absynpp_print_using_stmts= 0; int Cyc_Absynpp_print_externC_stmts= 0; static
struct Cyc_List_List* Cyc_Absynpp_curr_namespace= 0; static void Cyc_Absynpp_curr_namespace_add(
struct _tagged_string* v){ Cyc_Absynpp_curr_namespace=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List* _temp0=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp0->hd=( void*) v; _temp0->tl= 0; _temp0;}));} static void
Cyc_Absynpp_suppr_last( struct Cyc_List_List** l){ if((( struct Cyc_List_List*)
_check_null(* l))->tl == 0){* l= 0;} else{(( void(*)( struct Cyc_List_List** l))
Cyc_Absynpp_suppr_last)(&(( struct Cyc_List_List*) _check_null(* l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop(){(( void(*)( struct Cyc_List_List**
l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);} struct
_tagged_string Cyc_Absynpp_char_escape( char c){ switch( c){ case '\a': _LL1:
return({ char* _temp3="\\a"; struct _tagged_string _temp4; _temp4.curr= _temp3;
_temp4.base= _temp3; _temp4.last_plus_one= _temp3 + 3; _temp4;}); case '\b':
_LL2: return({ char* _temp6="\\b"; struct _tagged_string _temp7; _temp7.curr=
_temp6; _temp7.base= _temp6; _temp7.last_plus_one= _temp6 + 3; _temp7;}); case '\f':
_LL5: return({ char* _temp9="\\f"; struct _tagged_string _temp10; _temp10.curr=
_temp9; _temp10.base= _temp9; _temp10.last_plus_one= _temp9 + 3; _temp10;});
case '\n': _LL8: return({ char* _temp12="\\n"; struct _tagged_string _temp13;
_temp13.curr= _temp12; _temp13.base= _temp12; _temp13.last_plus_one= _temp12 + 3;
_temp13;}); case '\r': _LL11: return({ char* _temp15="\\r"; struct
_tagged_string _temp16; _temp16.curr= _temp15; _temp16.base= _temp15; _temp16.last_plus_one=
_temp15 + 3; _temp16;}); case '\t': _LL14: return({ char* _temp18="\\t"; struct
_tagged_string _temp19; _temp19.curr= _temp18; _temp19.base= _temp18; _temp19.last_plus_one=
_temp18 + 3; _temp19;}); case '\v': _LL17: return({ char* _temp21="\\v"; struct
_tagged_string _temp22; _temp22.curr= _temp21; _temp22.base= _temp21; _temp22.last_plus_one=
_temp21 + 3; _temp22;}); case '\\': _LL20: return({ char* _temp24="\\\\"; struct
_tagged_string _temp25; _temp25.curr= _temp24; _temp25.base= _temp24; _temp25.last_plus_one=
_temp24 + 3; _temp25;}); case '"': _LL23: return({ char* _temp27="\""; struct
_tagged_string _temp28; _temp28.curr= _temp27; _temp28.base= _temp27; _temp28.last_plus_one=
_temp27 + 2; _temp28;}); case '\'': _LL26: return({ char* _temp30="\\'"; struct
_tagged_string _temp31; _temp31.curr= _temp30; _temp31.base= _temp30; _temp31.last_plus_one=
_temp30 + 3; _temp31;}); default: _LL29: if( c >=' '? c <='~': 0){ struct
_tagged_string _temp33= Cyc_Core_new_string( 1); goto _LL34; _LL34:*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
_temp33, sizeof( char), 0)= c; return _temp33;} else{ struct _tagged_string
_temp35= Cyc_Core_new_string( 4); goto _LL36; _LL36: { int j= 0;*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
_temp35, sizeof( char), j ++)='\\';*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp35, sizeof( char),
j ++)=( char)('0' +( c >> 6 & 7));*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp35, sizeof( char),
j ++)=( char)('0' +( c >> 3 & 7));*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp35, sizeof( char),
j ++)=( char)('0' +( c & 7)); return _temp35;}}}} static int Cyc_Absynpp_special(
struct _tagged_string s){ int sz=( int)(({ struct _tagged_string _temp37= s;(
unsigned int)( _temp37.last_plus_one - _temp37.curr);}) - 1);{ int i= 0; for( 0;
i < sz; i ++){ char c=*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( char), i); if((( c <=' '? 1:
c >='~')? 1: c =='"')? 1: c =='\\'){ return 1;}}} return 0;} struct
_tagged_string Cyc_Absynpp_string_escape( struct _tagged_string s){ if( ! Cyc_Absynpp_special(
s)){ return s;}{ int n=( int)(({ struct _tagged_string _temp82= s;( unsigned int)(
_temp82.last_plus_one - _temp82.curr);}) - 1); if( n > 0?*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), n) =='\000': 0){ n --;}{ int len= 0;{ int i= 0; for( 0; i <= n; i
++){ char _temp38=*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( char), i); _LL40: if(
_temp38 =='\a'){ goto _LL41;} else{ goto _LL42;} _LL42: if( _temp38 =='\b'){
goto _LL43;} else{ goto _LL44;} _LL44: if( _temp38 =='\f'){ goto _LL45;} else{
goto _LL46;} _LL46: if( _temp38 =='\n'){ goto _LL47;} else{ goto _LL48;} _LL48:
if( _temp38 =='\r'){ goto _LL49;} else{ goto _LL50;} _LL50: if( _temp38 =='\t'){
goto _LL51;} else{ goto _LL52;} _LL52: if( _temp38 =='\v'){ goto _LL53;} else{
goto _LL54;} _LL54: if( _temp38 =='\\'){ goto _LL55;} else{ goto _LL56;} _LL56:
if( _temp38 =='"'){ goto _LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL41:
goto _LL43; _LL43: goto _LL45; _LL45: goto _LL47; _LL47: goto _LL49; _LL49: goto
_LL51; _LL51: goto _LL53; _LL53: goto _LL55; _LL55: goto _LL57; _LL57: len += 2;
goto _LL39; _LL59: if( _temp38 >=' '? _temp38 <='~': 0){ len ++;} else{ len += 4;}
goto _LL39; _LL39:;}}{ struct _tagged_string t= Cyc_Core_new_string( len); int j=
0;{ int i= 0; for( 0; i <= n; i ++){ char _temp60=*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), i); _LL62: if( _temp60 =='\a'){ goto _LL63;} else{ goto _LL64;}
_LL64: if( _temp60 =='\b'){ goto _LL65;} else{ goto _LL66;} _LL66: if( _temp60
=='\f'){ goto _LL67;} else{ goto _LL68;} _LL68: if( _temp60 =='\n'){ goto _LL69;}
else{ goto _LL70;} _LL70: if( _temp60 =='\r'){ goto _LL71;} else{ goto _LL72;}
_LL72: if( _temp60 =='\t'){ goto _LL73;} else{ goto _LL74;} _LL74: if( _temp60
=='\v'){ goto _LL75;} else{ goto _LL76;} _LL76: if( _temp60 =='\\'){ goto _LL77;}
else{ goto _LL78;} _LL78: if( _temp60 =='"'){ goto _LL79;} else{ goto _LL80;}
_LL80: goto _LL81; _LL63:*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( t, sizeof( char), j ++)='\\';*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( char), j ++)='a'; goto _LL61; _LL65:*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( t, sizeof( char), j ++)='\\';*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( char), j ++)='b'; goto _LL61; _LL67:*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( char), j ++)='\\';*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( char), j ++)='f'; goto _LL61; _LL69:*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( t, sizeof( char), j ++)='\\';*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( char), j ++)='n'; goto _LL61; _LL71:*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( char), j ++)='\\';*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( char), j ++)='r'; goto _LL61; _LL73:*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( t, sizeof( char), j ++)='\\';*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( char), j ++)='t'; goto _LL61; _LL75:*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( char), j ++)='\\';*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( char), j ++)='v'; goto _LL61; _LL77:*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( t, sizeof( char), j ++)='\\';*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( char), j ++)='\\'; goto _LL61; _LL79:*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( char), j ++)='\\';*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( char), j ++)='"'; goto _LL61; _LL81: if( _temp60 >=' '? _temp60 <='~': 0){*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( char), j ++)= _temp60;} else{*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( char), j ++)='\\';*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( t, sizeof( char), j ++)=( char)('0' +(
_temp60 >> 6 & 7));*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( t, sizeof( char), j ++)=( char)('0' +(
_temp60 >> 3 & 7));*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( t, sizeof( char), j ++)=( char)('0' +(
_temp60 & 7));} goto _LL61; _LL61:;}} return t;}}}} static struct Cyc_PP_Doc*
Cyc_Absynpp_textptr( struct _tagged_string* s){ return Cyc_PP_text(* s);} static
char _temp85[ 9u]="restrict"; static struct _tagged_string Cyc_Absynpp_restrict_string={
_temp85, _temp85, _temp85 + 9u}; static char _temp88[ 9u]="volatile"; static
struct _tagged_string Cyc_Absynpp_volatile_string={ _temp88, _temp88, _temp88 +
9u}; static char _temp91[ 6u]="const"; static struct _tagged_string Cyc_Absynpp_const_string={
_temp91, _temp91, _temp91 + 6u}; static struct _tagged_string* Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string; static struct _tagged_string* Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string; static struct _tagged_string* Cyc_Absynpp_const_sp=&
Cyc_Absynpp_const_string; struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual
tq){ struct Cyc_List_List* l= 0; if( tq.q_restrict){ l=({ struct Cyc_List_List*
_temp92=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp92->hd=( void*) Cyc_Absynpp_restrict_sp; _temp92->tl= l; _temp92;});} if(
tq.q_volatile){ l=({ struct Cyc_List_List* _temp93=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp93->hd=( void*) Cyc_Absynpp_volatile_sp;
_temp93->tl= l; _temp93;});} if( tq.q_const){ l=({ struct Cyc_List_List* _temp94=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp94->hd=(
void*) Cyc_Absynpp_const_sp; _temp94->tl= l; _temp94;});} return Cyc_Absynpp_egroup(({
char* _temp95=""; struct _tagged_string _temp96; _temp96.curr= _temp95; _temp96.base=
_temp95; _temp96.last_plus_one= _temp95 + 1; _temp96;}),({ char* _temp97=" ";
struct _tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97;
_temp98.last_plus_one= _temp97 + 2; _temp98;}),({ char* _temp99=" "; struct
_tagged_string _temp100; _temp100.curr= _temp99; _temp100.base= _temp99;
_temp100.last_plus_one= _temp99 + 2; _temp100;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr, l));} struct _tagged_string Cyc_Absynpp_kind2string( void*
k){ void* _temp101= k; _LL103: if( _temp101 ==( void*) Cyc_Absyn_AnyKind){ goto
_LL104;} else{ goto _LL105;} _LL105: if( _temp101 ==( void*) Cyc_Absyn_MemKind){
goto _LL106;} else{ goto _LL107;} _LL107: if( _temp101 ==( void*) Cyc_Absyn_BoxKind){
goto _LL108;} else{ goto _LL109;} _LL109: if( _temp101 ==( void*) Cyc_Absyn_RgnKind){
goto _LL110;} else{ goto _LL111;} _LL111: if( _temp101 ==( void*) Cyc_Absyn_EffKind){
goto _LL112;} else{ goto _LL102;} _LL104: return({ char* _temp113="A"; struct
_tagged_string _temp114; _temp114.curr= _temp113; _temp114.base= _temp113;
_temp114.last_plus_one= _temp113 + 2; _temp114;}); _LL106: return({ char*
_temp115="M"; struct _tagged_string _temp116; _temp116.curr= _temp115; _temp116.base=
_temp115; _temp116.last_plus_one= _temp115 + 2; _temp116;}); _LL108: return({
char* _temp117="B"; struct _tagged_string _temp118; _temp118.curr= _temp117;
_temp118.base= _temp117; _temp118.last_plus_one= _temp117 + 2; _temp118;});
_LL110: return({ char* _temp119="R"; struct _tagged_string _temp120; _temp120.curr=
_temp119; _temp120.base= _temp119; _temp120.last_plus_one= _temp119 + 2;
_temp120;}); _LL112: return({ char* _temp121="E"; struct _tagged_string _temp122;
_temp122.curr= _temp121; _temp122.base= _temp121; _temp122.last_plus_one=
_temp121 + 2; _temp122;}); _LL102:;} struct _tagged_string Cyc_Absynpp_ckind2string(
struct Cyc_Absyn_Conref* c){ void* _temp123=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c))->v; void* _temp129;
_LL125: if(( unsigned int) _temp123 > 1u?*(( int*) _temp123) == Cyc_Absyn_Eq_constr:
0){ _LL130: _temp129=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp123)->f1;
goto _LL126;} else{ goto _LL127;} _LL127: goto _LL128; _LL126: return Cyc_Absynpp_kind2string(
_temp129); _LL128: return({ char* _temp131="?"; struct _tagged_string _temp132;
_temp132.curr= _temp131; _temp132.base= _temp131; _temp132.last_plus_one=
_temp131 + 2; _temp132;}); _LL124:;} struct Cyc_PP_Doc* Cyc_Absynpp_kind2doc(
void* k){ return Cyc_PP_text( Cyc_Absynpp_kind2string( k));} struct Cyc_PP_Doc*
Cyc_Absynpp_ckind2doc( struct Cyc_Absyn_Conref* c){ return Cyc_PP_text( Cyc_Absynpp_ckind2string(
c));} struct Cyc_PP_Doc* Cyc_Absynpp_group( struct _tagged_string start, struct
_tagged_string stop, struct _tagged_string sep, struct Cyc_List_List* ss){
return Cyc_Absynpp_cat3( Cyc_PP_text( start), Cyc_PP_seq( sep, ss), Cyc_PP_text(
stop));} struct Cyc_PP_Doc* Cyc_Absynpp_egroup( struct _tagged_string start,
struct _tagged_string stop, struct _tagged_string sep, struct Cyc_List_List* ss){
if( ss == 0){ return Cyc_PP_nil_doc();} else{ return Cyc_Absynpp_cat3( Cyc_PP_text(
start), Cyc_PP_seq( sep, ss), Cyc_PP_text( stop));}} struct Cyc_PP_Doc* Cyc_Absynpp_groupl(
struct _tagged_string start, struct _tagged_string stop, struct _tagged_string
sep, struct Cyc_List_List* ss){ return Cyc_Absynpp_cat3( Cyc_PP_text( start),
Cyc_PP_seql( sep, ss), Cyc_PP_text( stop));} struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc(
struct Cyc_List_List* ts){ return Cyc_Absynpp_egroup(({ char* _temp133="<";
struct _tagged_string _temp134; _temp134.curr= _temp133; _temp134.base= _temp133;
_temp134.last_plus_one= _temp133 + 2; _temp134;}),({ char* _temp135=">"; struct
_tagged_string _temp136; _temp136.curr= _temp135; _temp136.base= _temp135;
_temp136.last_plus_one= _temp135 + 2; _temp136;}),({ char* _temp137=","; struct
_tagged_string _temp138; _temp138.curr= _temp137; _temp138.base= _temp137;
_temp138.last_plus_one= _temp137 + 2; _temp138;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc,
ts));} struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){
void* _temp139=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp147; void* _temp149;
_LL141: if(( unsigned int) _temp139 > 1u?*(( int*) _temp139) == Cyc_Absyn_Eq_constr:
0){ _LL148: _temp147=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp139)->f1;
if( _temp147 ==( void*) Cyc_Absyn_BoxKind){ goto _LL142;} else{ goto _LL143;}}
else{ goto _LL143;} _LL143: if(( unsigned int) _temp139 > 1u?*(( int*) _temp139)
== Cyc_Absyn_Eq_constr: 0){ _LL150: _temp149=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp139)->f1; goto _LL144;} else{ goto _LL145;} _LL145: goto _LL146; _LL142:
return Cyc_Absynpp_textptr( tv->name); _LL144: return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr(
tv->name), Cyc_PP_text(({ char* _temp151="::"; struct _tagged_string _temp152;
_temp152.curr= _temp151; _temp152.base= _temp151; _temp152.last_plus_one=
_temp151 + 3; _temp152;})), Cyc_Absynpp_kind2doc( _temp149)); _LL146: return Cyc_Absynpp_cat2(
Cyc_Absynpp_textptr( tv->name), Cyc_PP_text(({ char* _temp153="/*::?*/"; struct
_tagged_string _temp154; _temp154.curr= _temp153; _temp154.base= _temp153;
_temp154.last_plus_one= _temp153 + 8; _temp154;}))); _LL140:;} struct Cyc_PP_Doc*
Cyc_Absynpp_ktvars2doc( struct Cyc_List_List* tvs){ return Cyc_Absynpp_egroup(({
char* _temp155="<"; struct _tagged_string _temp156; _temp156.curr= _temp155;
_temp156.base= _temp155; _temp156.last_plus_one= _temp155 + 2; _temp156;}),({
char* _temp157=">"; struct _tagged_string _temp158; _temp158.curr= _temp157;
_temp158.base= _temp157; _temp158.last_plus_one= _temp157 + 2; _temp158;}),({
char* _temp159=","; struct _tagged_string _temp160; _temp160.curr= _temp159;
_temp160.base= _temp159; _temp160.last_plus_one= _temp159 + 2; _temp160;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_ktvar2doc, tvs));} static
struct _tagged_string* Cyc_Absynpp_get_name( struct Cyc_Absyn_Tvar* tv){ return
tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc( struct Cyc_List_List* tvs){
if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc( tvs);} return
Cyc_Absynpp_egroup(({ char* _temp161="<"; struct _tagged_string _temp162;
_temp162.curr= _temp161; _temp162.base= _temp161; _temp162.last_plus_one=
_temp161 + 2; _temp162;}),({ char* _temp163=">"; struct _tagged_string _temp164;
_temp164.curr= _temp163; _temp164.base= _temp163; _temp164.last_plus_one=
_temp163 + 2; _temp164;}),({ char* _temp165=","; struct _tagged_string _temp166;
_temp166.curr= _temp165; _temp166.base= _temp165; _temp166.last_plus_one=
_temp165 + 2; _temp166;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr,((
struct Cyc_List_List*(*)( struct _tagged_string*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_get_name, tvs)));} struct
_tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc(
struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((* t).f1,(* t).f2, 0);} struct
Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List* ts){ return Cyc_Absynpp_group(({
char* _temp167="("; struct _tagged_string _temp168; _temp168.curr= _temp167;
_temp168.base= _temp167; _temp168.last_plus_one= _temp167 + 2; _temp168;}),({
char* _temp169=")"; struct _tagged_string _temp170; _temp170.curr= _temp169;
_temp170.base= _temp169; _temp170.last_plus_one= _temp169 + 2; _temp170;}),({
char* _temp171=","; struct _tagged_string _temp172; _temp172.curr= _temp171;
_temp172.base= _temp171; _temp172.last_plus_one= _temp171 + 2; _temp172;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc(
void* a){ return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if( atts == 0){ return Cyc_PP_nil_doc();}
return Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp173=" __attribute__"; struct
_tagged_string _temp174; _temp174.curr= _temp173; _temp174.base= _temp173;
_temp174.last_plus_one= _temp173 + 15; _temp174;})), Cyc_Absynpp_group(({ char*
_temp175="(("; struct _tagged_string _temp176; _temp176.curr= _temp175; _temp176.base=
_temp175; _temp176.last_plus_one= _temp175 + 3; _temp176;}),({ char* _temp177="))";
struct _tagged_string _temp178; _temp178.curr= _temp177; _temp178.base= _temp177;
_temp178.last_plus_one= _temp177 + 3; _temp178;}),({ char* _temp179=","; struct
_tagged_string _temp180; _temp180.curr= _temp179; _temp180.base= _temp179;
_temp180.last_plus_one= _temp179 + 2; _temp180;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc,
atts)));} int Cyc_Absynpp_next_is_pointer( struct Cyc_List_List* tms){ if( tms
== 0){ return 0;}{ void* _temp181=( void*)(( struct Cyc_List_List*) _check_null(
tms))->hd; struct Cyc_Absyn_Tqual _temp187; void* _temp189; void* _temp191;
_LL183: if(( unsigned int) _temp181 > 1u?*(( int*) _temp181) == Cyc_Absyn_Pointer_mod:
0){ _LL192: _temp191=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp181)->f1;
goto _LL190; _LL190: _temp189=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp181)->f2; goto _LL188; _LL188: _temp187=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp181)->f3; goto _LL184;} else{ goto _LL185;} _LL185: goto _LL186; _LL184:
return 1; _LL186: return 0; _LL182:;}} extern struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc(
void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc( struct Cyc_PP_Doc* d, struct
Cyc_List_List* tms){ if( tms == 0){ return d;}{ struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc(
d,(( struct Cyc_List_List*) _check_null( tms))->tl); struct Cyc_PP_Doc* p_rest=
Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp295="("; struct _tagged_string
_temp296; _temp296.curr= _temp295; _temp296.base= _temp295; _temp296.last_plus_one=
_temp295 + 2; _temp296;})), rest, Cyc_PP_text(({ char* _temp297=")"; struct
_tagged_string _temp298; _temp298.curr= _temp297; _temp298.base= _temp297;
_temp298.last_plus_one= _temp297 + 2; _temp298;}))); void* _temp193=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp207;
void* _temp209; struct Cyc_List_List* _temp211; struct Cyc_Position_Segment*
_temp213; int _temp215; struct Cyc_Position_Segment* _temp217; struct Cyc_List_List*
_temp219; struct Cyc_Absyn_Tqual _temp221; void* _temp223; void* _temp225;
_LL195: if( _temp193 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL196;} else{ goto
_LL197;} _LL197: if(( unsigned int) _temp193 > 1u?*(( int*) _temp193) == Cyc_Absyn_ConstArray_mod:
0){ _LL208: _temp207=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp193)->f1;
goto _LL198;} else{ goto _LL199;} _LL199: if(( unsigned int) _temp193 > 1u?*((
int*) _temp193) == Cyc_Absyn_Function_mod: 0){ _LL210: _temp209=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp193)->f1; goto _LL200;} else{ goto
_LL201;} _LL201: if(( unsigned int) _temp193 > 1u?*(( int*) _temp193) == Cyc_Absyn_Attributes_mod:
0){ _LL214: _temp213=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp193)->f1;
goto _LL212; _LL212: _temp211=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp193)->f2; goto _LL202;} else{ goto _LL203;} _LL203: if(( unsigned int)
_temp193 > 1u?*(( int*) _temp193) == Cyc_Absyn_TypeParams_mod: 0){ _LL220:
_temp219=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp193)->f1; goto _LL218;
_LL218: _temp217=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp193)->f2; goto
_LL216; _LL216: _temp215=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp193)->f3;
goto _LL204;} else{ goto _LL205;} _LL205: if(( unsigned int) _temp193 > 1u?*((
int*) _temp193) == Cyc_Absyn_Pointer_mod: 0){ _LL226: _temp225=( void*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp193)->f1; goto _LL224; _LL224: _temp223=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp193)->f2; goto _LL222;
_LL222: _temp221=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp193)->f3; goto
_LL206;} else{ goto _LL194;} _LL196: if( Cyc_Absynpp_next_is_pointer((( struct
Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return Cyc_Absynpp_cat2(
rest, Cyc_PP_text(({ char* _temp227="[]"; struct _tagged_string _temp228;
_temp228.curr= _temp227; _temp228.base= _temp227; _temp228.last_plus_one=
_temp227 + 3; _temp228;}))); _LL198: if( Cyc_Absynpp_next_is_pointer((( struct
Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return Cyc_Absynpp_cat4(
rest, Cyc_PP_text(({ char* _temp229="["; struct _tagged_string _temp230;
_temp230.curr= _temp229; _temp230.base= _temp229; _temp230.last_plus_one=
_temp229 + 2; _temp230;})), Cyc_Absynpp_exp2doc( _temp207), Cyc_PP_text(({ char*
_temp231="]"; struct _tagged_string _temp232; _temp232.curr= _temp231; _temp232.base=
_temp231; _temp232.last_plus_one= _temp231 + 2; _temp232;}))); _LL200: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;}{ void* _temp233=
_temp209; struct Cyc_Core_Opt* _temp239; struct Cyc_Absyn_VarargInfo* _temp241;
int _temp243; struct Cyc_List_List* _temp245; struct Cyc_Position_Segment*
_temp247; struct Cyc_List_List* _temp249; _LL235: if(*(( int*) _temp233) == Cyc_Absyn_WithTypes){
_LL246: _temp245=(( struct Cyc_Absyn_WithTypes_struct*) _temp233)->f1; goto
_LL244; _LL244: _temp243=(( struct Cyc_Absyn_WithTypes_struct*) _temp233)->f2;
goto _LL242; _LL242: _temp241=(( struct Cyc_Absyn_WithTypes_struct*) _temp233)->f3;
goto _LL240; _LL240: _temp239=(( struct Cyc_Absyn_WithTypes_struct*) _temp233)->f4;
goto _LL236;} else{ goto _LL237;} _LL237: if(*(( int*) _temp233) == Cyc_Absyn_NoTypes){
_LL250: _temp249=(( struct Cyc_Absyn_NoTypes_struct*) _temp233)->f1; goto _LL248;
_LL248: _temp247=(( struct Cyc_Absyn_NoTypes_struct*) _temp233)->f2; goto _LL238;}
else{ goto _LL234;} _LL236: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_funargs2doc(
_temp245, _temp243, _temp241, _temp239)); _LL238: return Cyc_Absynpp_cat2( rest,
Cyc_Absynpp_group(({ char* _temp251="("; struct _tagged_string _temp252;
_temp252.curr= _temp251; _temp252.base= _temp251; _temp252.last_plus_one=
_temp251 + 2; _temp252;}),({ char* _temp253=")"; struct _tagged_string _temp254;
_temp254.curr= _temp253; _temp254.base= _temp253; _temp254.last_plus_one=
_temp253 + 2; _temp254;}),({ char* _temp255=","; struct _tagged_string _temp256;
_temp256.curr= _temp255; _temp256.base= _temp255; _temp256.last_plus_one=
_temp255 + 2; _temp256;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr,
_temp249))); _LL234:;} _LL202: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_atts2doc(
_temp211)); _LL204: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} if( _temp215){ return Cyc_Absynpp_cat2(
rest, Cyc_Absynpp_ktvars2doc( _temp219));} else{ return Cyc_Absynpp_cat2( rest,
Cyc_Absynpp_tvars2doc( _temp219));} _LL206: { struct Cyc_PP_Doc* ptr;{ void*
_temp257= _temp225; struct Cyc_Absyn_Exp* _temp265; struct Cyc_Absyn_Exp*
_temp267; _LL259: if(( unsigned int) _temp257 > 1u?*(( int*) _temp257) == Cyc_Absyn_Nullable_ps:
0){ _LL266: _temp265=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp257)->f1;
goto _LL260;} else{ goto _LL261;} _LL261: if(( unsigned int) _temp257 > 1u?*((
int*) _temp257) == Cyc_Absyn_NonNullable_ps: 0){ _LL268: _temp267=(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp257)->f1; goto _LL262;} else{ goto _LL263;} _LL263: if( _temp257 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL264;} else{ goto _LL258;} _LL260: if( Cyc_Evexp_eval_const_uint_exp(
_temp265) == 1){ ptr= Cyc_PP_text(({ char* _temp269="*"; struct _tagged_string
_temp270; _temp270.curr= _temp269; _temp270.base= _temp269; _temp270.last_plus_one=
_temp269 + 2; _temp270;}));} else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(({ char*
_temp271="*"; struct _tagged_string _temp272; _temp272.curr= _temp271; _temp272.base=
_temp271; _temp272.last_plus_one= _temp271 + 2; _temp272;})), Cyc_PP_text(({
char* _temp273="{"; struct _tagged_string _temp274; _temp274.curr= _temp273;
_temp274.base= _temp273; _temp274.last_plus_one= _temp273 + 2; _temp274;})), Cyc_Absynpp_exp2doc(
_temp265), Cyc_PP_text(({ char* _temp275="}"; struct _tagged_string _temp276;
_temp276.curr= _temp275; _temp276.base= _temp275; _temp276.last_plus_one=
_temp275 + 2; _temp276;})));} goto _LL258; _LL262: if( Cyc_Evexp_eval_const_uint_exp(
_temp267) == 1){ ptr= Cyc_PP_text(({ char* _temp277="@"; struct _tagged_string
_temp278; _temp278.curr= _temp277; _temp278.base= _temp277; _temp278.last_plus_one=
_temp277 + 2; _temp278;}));} else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(({ char*
_temp279="@"; struct _tagged_string _temp280; _temp280.curr= _temp279; _temp280.base=
_temp279; _temp280.last_plus_one= _temp279 + 2; _temp280;})), Cyc_PP_text(({
char* _temp281="{"; struct _tagged_string _temp282; _temp282.curr= _temp281;
_temp282.base= _temp281; _temp282.last_plus_one= _temp281 + 2; _temp282;})), Cyc_Absynpp_exp2doc(
_temp267), Cyc_PP_text(({ char* _temp283="}"; struct _tagged_string _temp284;
_temp284.curr= _temp283; _temp284.base= _temp283; _temp284.last_plus_one=
_temp283 + 2; _temp284;})));} goto _LL258; _LL264: ptr= Cyc_PP_text(({ char*
_temp285="?"; struct _tagged_string _temp286; _temp286.curr= _temp285; _temp286.base=
_temp285; _temp286.last_plus_one= _temp285 + 2; _temp286;})); goto _LL258;
_LL258:;}{ void* _temp287= Cyc_Tcutil_compress( _temp223); _LL289: if( _temp287
==( void*) Cyc_Absyn_HeapRgn){ goto _LL290;} else{ goto _LL291;} _LL291: goto
_LL292; _LL290: return Cyc_Absynpp_cat2( ptr, rest); _LL292: return Cyc_Absynpp_cat4(
ptr, Cyc_Absynpp_ntyp2doc( _temp223), Cyc_PP_text(({ char* _temp293=" "; struct
_tagged_string _temp294; _temp294.curr= _temp293; _temp294.base= _temp293;
_temp294.last_plus_one= _temp293 + 2; _temp294;})), rest); _LL288:;}} _LL194:;}}
struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){ void* _temp299= Cyc_Tcutil_compress(
t); _LL301: if( _temp299 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL302;} else{ goto
_LL303;} _LL303: goto _LL304; _LL302: return Cyc_PP_text(({ char* _temp305="`H";
struct _tagged_string _temp306; _temp306.curr= _temp305; _temp306.base= _temp305;
_temp306.last_plus_one= _temp305 + 3; _temp306;})); _LL304: return Cyc_Absynpp_ntyp2doc(
t); _LL300:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
regions, struct Cyc_List_List** effects, void* t){ void* _temp307= Cyc_Tcutil_compress(
t); void* _temp315; struct Cyc_List_List* _temp317; _LL309: if(( unsigned int)
_temp307 > 4u?*(( int*) _temp307) == Cyc_Absyn_AccessEff: 0){ _LL316: _temp315=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp307)->f1; goto _LL310;} else{
goto _LL311;} _LL311: if(( unsigned int) _temp307 > 4u?*(( int*) _temp307) ==
Cyc_Absyn_JoinEff: 0){ _LL318: _temp317=(( struct Cyc_Absyn_JoinEff_struct*)
_temp307)->f1; goto _LL312;} else{ goto _LL313;} _LL313: goto _LL314; _LL310:*
regions=({ struct Cyc_List_List* _temp319=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp319->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp315); _temp319->tl=* regions; _temp319;}); goto _LL308; _LL312: for( 0;
_temp317 != 0; _temp317=(( struct Cyc_List_List*) _check_null( _temp317))->tl){
Cyc_Absynpp_effects2docs( regions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp317))->hd);} goto _LL308; _LL314:* effects=({ struct Cyc_List_List*
_temp320=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp320->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp320->tl=* effects; _temp320;});
goto _LL308; _LL308:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_Absynpp_group(({
char* _temp321=""; struct _tagged_string _temp322; _temp322.curr= _temp321;
_temp322.base= _temp321; _temp322.last_plus_one= _temp321 + 1; _temp322;}),({
char* _temp323=""; struct _tagged_string _temp324; _temp324.curr= _temp323;
_temp324.base= _temp323; _temp324.last_plus_one= _temp323 + 1; _temp324;}),({
char* _temp325="+"; struct _tagged_string _temp326; _temp326.curr= _temp325;
_temp326.base= _temp325; _temp326.last_plus_one= _temp325 + 2; _temp326;}),
effects);} else{ struct Cyc_PP_Doc* _temp333= Cyc_Absynpp_group(({ char*
_temp327="{"; struct _tagged_string _temp328; _temp328.curr= _temp327; _temp328.base=
_temp327; _temp328.last_plus_one= _temp327 + 2; _temp328;}),({ char* _temp329="}";
struct _tagged_string _temp330; _temp330.curr= _temp329; _temp330.base= _temp329;
_temp330.last_plus_one= _temp329 + 2; _temp330;}),({ char* _temp331=","; struct
_tagged_string _temp332; _temp332.curr= _temp331; _temp332.base= _temp331;
_temp332.last_plus_one= _temp331 + 2; _temp332;}), regions); goto _LL334; _LL334:
return Cyc_Absynpp_group(({ char* _temp335=""; struct _tagged_string _temp336;
_temp336.curr= _temp335; _temp336.base= _temp335; _temp336.last_plus_one=
_temp335 + 1; _temp336;}),({ char* _temp337=""; struct _tagged_string _temp338;
_temp338.curr= _temp337; _temp338.base= _temp337; _temp338.last_plus_one=
_temp337 + 1; _temp338;}),({ char* _temp339="+"; struct _tagged_string _temp340;
_temp340.curr= _temp339; _temp340.base= _temp339; _temp340.last_plus_one=
_temp339 + 2; _temp340;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( effects,({ struct Cyc_List_List*
_temp341=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp341->hd=( void*) _temp333; _temp341->tl= 0; _temp341;})));}} struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc* s;{ void* _temp342= t; struct
Cyc_Absyn_Exp* _temp388; struct Cyc_Absyn_Tqual _temp390; void* _temp392; struct
Cyc_Absyn_FnInfo _temp394; struct Cyc_Absyn_PtrInfo _temp396; int _temp398;
struct Cyc_Core_Opt* _temp400; void* _temp402; struct Cyc_Absyn_Tvar* _temp404;
struct Cyc_Absyn_TunionInfo _temp406; void* _temp408; struct Cyc_List_List*
_temp410; void* _temp412; struct Cyc_Absyn_TunionFieldInfo _temp414; struct Cyc_List_List*
_temp416; void* _temp418; void* _temp420; void* _temp422; struct Cyc_List_List*
_temp424; struct Cyc_Absyn_Structdecl** _temp426; struct Cyc_List_List* _temp428;
struct _tuple0* _temp430; struct Cyc_Absyn_Uniondecl** _temp432; struct Cyc_List_List*
_temp434; struct _tuple0* _temp436; struct Cyc_List_List* _temp438; struct Cyc_List_List*
_temp440; struct Cyc_Absyn_Enumdecl* _temp442; struct _tuple0* _temp444; struct
Cyc_Core_Opt* _temp446; struct Cyc_List_List* _temp448; struct _tuple0* _temp450;
void* _temp452; void* _temp454; struct Cyc_List_List* _temp456; _LL344: if((
unsigned int) _temp342 > 4u?*(( int*) _temp342) == Cyc_Absyn_ArrayType: 0){
_LL393: _temp392=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp342)->f1;
goto _LL391; _LL391: _temp390=(( struct Cyc_Absyn_ArrayType_struct*) _temp342)->f2;
goto _LL389; _LL389: _temp388=(( struct Cyc_Absyn_ArrayType_struct*) _temp342)->f3;
goto _LL345;} else{ goto _LL346;} _LL346: if(( unsigned int) _temp342 > 4u?*((
int*) _temp342) == Cyc_Absyn_FnType: 0){ _LL395: _temp394=(( struct Cyc_Absyn_FnType_struct*)
_temp342)->f1; goto _LL347;} else{ goto _LL348;} _LL348: if(( unsigned int)
_temp342 > 4u?*(( int*) _temp342) == Cyc_Absyn_PointerType: 0){ _LL397: _temp396=((
struct Cyc_Absyn_PointerType_struct*) _temp342)->f1; goto _LL349;} else{ goto
_LL350;} _LL350: if( _temp342 ==( void*) Cyc_Absyn_VoidType){ goto _LL351;}
else{ goto _LL352;} _LL352: if(( unsigned int) _temp342 > 4u?*(( int*) _temp342)
== Cyc_Absyn_Evar: 0){ _LL403: _temp402=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp342)->f1; goto _LL401; _LL401: _temp400=(( struct Cyc_Absyn_Evar_struct*)
_temp342)->f2; goto _LL399; _LL399: _temp398=(( struct Cyc_Absyn_Evar_struct*)
_temp342)->f3; goto _LL353;} else{ goto _LL354;} _LL354: if(( unsigned int)
_temp342 > 4u?*(( int*) _temp342) == Cyc_Absyn_VarType: 0){ _LL405: _temp404=((
struct Cyc_Absyn_VarType_struct*) _temp342)->f1; goto _LL355;} else{ goto _LL356;}
_LL356: if(( unsigned int) _temp342 > 4u?*(( int*) _temp342) == Cyc_Absyn_TunionType:
0){ _LL407: _temp406=(( struct Cyc_Absyn_TunionType_struct*) _temp342)->f1;
_LL413: _temp412=( void*) _temp406.tunion_info; goto _LL411; _LL411: _temp410=
_temp406.targs; goto _LL409; _LL409: _temp408=( void*) _temp406.rgn; goto _LL357;}
else{ goto _LL358;} _LL358: if(( unsigned int) _temp342 > 4u?*(( int*) _temp342)
== Cyc_Absyn_TunionFieldType: 0){ _LL415: _temp414=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp342)->f1; _LL419: _temp418=( void*) _temp414.field_info; goto _LL417;
_LL417: _temp416= _temp414.targs; goto _LL359;} else{ goto _LL360;} _LL360: if((
unsigned int) _temp342 > 4u?*(( int*) _temp342) == Cyc_Absyn_IntType: 0){ _LL423:
_temp422=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp342)->f1; goto _LL421;
_LL421: _temp420=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp342)->f2;
goto _LL361;} else{ goto _LL362;} _LL362: if( _temp342 ==( void*) Cyc_Absyn_FloatType){
goto _LL363;} else{ goto _LL364;} _LL364: if( _temp342 ==( void*) Cyc_Absyn_DoubleType){
goto _LL365;} else{ goto _LL366;} _LL366: if(( unsigned int) _temp342 > 4u?*((
int*) _temp342) == Cyc_Absyn_TupleType: 0){ _LL425: _temp424=(( struct Cyc_Absyn_TupleType_struct*)
_temp342)->f1; goto _LL367;} else{ goto _LL368;} _LL368: if(( unsigned int)
_temp342 > 4u?*(( int*) _temp342) == Cyc_Absyn_StructType: 0){ _LL431: _temp430=((
struct Cyc_Absyn_StructType_struct*) _temp342)->f1; goto _LL429; _LL429:
_temp428=(( struct Cyc_Absyn_StructType_struct*) _temp342)->f2; goto _LL427;
_LL427: _temp426=(( struct Cyc_Absyn_StructType_struct*) _temp342)->f3; goto
_LL369;} else{ goto _LL370;} _LL370: if(( unsigned int) _temp342 > 4u?*(( int*)
_temp342) == Cyc_Absyn_UnionType: 0){ _LL437: _temp436=(( struct Cyc_Absyn_UnionType_struct*)
_temp342)->f1; goto _LL435; _LL435: _temp434=(( struct Cyc_Absyn_UnionType_struct*)
_temp342)->f2; goto _LL433; _LL433: _temp432=(( struct Cyc_Absyn_UnionType_struct*)
_temp342)->f3; goto _LL371;} else{ goto _LL372;} _LL372: if(( unsigned int)
_temp342 > 4u?*(( int*) _temp342) == Cyc_Absyn_AnonStructType: 0){ _LL439:
_temp438=(( struct Cyc_Absyn_AnonStructType_struct*) _temp342)->f1; goto _LL373;}
else{ goto _LL374;} _LL374: if(( unsigned int) _temp342 > 4u?*(( int*) _temp342)
== Cyc_Absyn_AnonUnionType: 0){ _LL441: _temp440=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp342)->f1; goto _LL375;} else{ goto _LL376;} _LL376: if(( unsigned int)
_temp342 > 4u?*(( int*) _temp342) == Cyc_Absyn_EnumType: 0){ _LL445: _temp444=((
struct Cyc_Absyn_EnumType_struct*) _temp342)->f1; goto _LL443; _LL443: _temp442=((
struct Cyc_Absyn_EnumType_struct*) _temp342)->f2; goto _LL377;} else{ goto
_LL378;} _LL378: if(( unsigned int) _temp342 > 4u?*(( int*) _temp342) == Cyc_Absyn_TypedefType:
0){ _LL451: _temp450=(( struct Cyc_Absyn_TypedefType_struct*) _temp342)->f1;
goto _LL449; _LL449: _temp448=(( struct Cyc_Absyn_TypedefType_struct*) _temp342)->f2;
goto _LL447; _LL447: _temp446=(( struct Cyc_Absyn_TypedefType_struct*) _temp342)->f3;
goto _LL379;} else{ goto _LL380;} _LL380: if(( unsigned int) _temp342 > 4u?*((
int*) _temp342) == Cyc_Absyn_RgnHandleType: 0){ _LL453: _temp452=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp342)->f1; goto _LL381;} else{ goto
_LL382;} _LL382: if( _temp342 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL383;} else{
goto _LL384;} _LL384: if(( unsigned int) _temp342 > 4u?*(( int*) _temp342) ==
Cyc_Absyn_AccessEff: 0){ _LL455: _temp454=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp342)->f1; goto _LL385;} else{ goto _LL386;} _LL386: if(( unsigned int)
_temp342 > 4u?*(( int*) _temp342) == Cyc_Absyn_JoinEff: 0){ _LL457: _temp456=((
struct Cyc_Absyn_JoinEff_struct*) _temp342)->f1; goto _LL387;} else{ goto _LL343;}
_LL345: return Cyc_PP_text(({ char* _temp458="[[[array]]]"; struct
_tagged_string _temp459; _temp459.curr= _temp458; _temp459.base= _temp458;
_temp459.last_plus_one= _temp458 + 12; _temp459;})); _LL347: return Cyc_PP_nil_doc();
_LL349: return Cyc_PP_nil_doc(); _LL351: s= Cyc_PP_text(({ char* _temp460="void";
struct _tagged_string _temp461; _temp461.curr= _temp460; _temp461.base= _temp460;
_temp461.last_plus_one= _temp460 + 5; _temp461;})); goto _LL343; _LL353: if(
_temp400 != 0){ return Cyc_Absynpp_ntyp2doc(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp400))->v);} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(({ char*
_temp462="%"; struct _tagged_string _temp463; _temp463.curr= _temp462; _temp463.base=
_temp462; _temp463.last_plus_one= _temp462 + 2; _temp463;})), Cyc_PP_text(({
char* _temp464="("; struct _tagged_string _temp465; _temp465.curr= _temp464;
_temp465.base= _temp464; _temp465.last_plus_one= _temp464 + 2; _temp465;})), Cyc_PP_text(
xprintf("%d", _temp398)), Cyc_PP_text(({ char* _temp466=")::"; struct
_tagged_string _temp467; _temp467.curr= _temp466; _temp467.base= _temp466;
_temp467.last_plus_one= _temp466 + 4; _temp467;})), Cyc_Absynpp_kind2doc(
_temp402));} goto _LL343; _LL355: s= Cyc_Absynpp_textptr( _temp404->name); if(
Cyc_Absynpp_print_all_kinds){ s= Cyc_Absynpp_cat3( s, Cyc_PP_text(({ char*
_temp468="::"; struct _tagged_string _temp469; _temp469.curr= _temp468; _temp469.base=
_temp468; _temp469.last_plus_one= _temp468 + 3; _temp469;})), Cyc_Absynpp_ckind2doc(
_temp404->kind));} if( ! Cyc_Absynpp_dont_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_temp404): 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp470="_ /* ";
struct _tagged_string _temp471; _temp471.curr= _temp470; _temp471.base= _temp470;
_temp471.last_plus_one= _temp470 + 6; _temp471;})), s, Cyc_PP_text(({ char*
_temp472=" */"; struct _tagged_string _temp473; _temp473.curr= _temp472;
_temp473.base= _temp472; _temp473.last_plus_one= _temp472 + 4; _temp473;})));}
goto _LL343; _LL357:{ void* _temp474= _temp412; struct Cyc_Absyn_UnknownTunionInfo
_temp480; int _temp482; struct _tuple0* _temp484; struct Cyc_Absyn_Tuniondecl*
_temp486; struct Cyc_Absyn_Tuniondecl _temp488; int _temp489; struct Cyc_Core_Opt*
_temp491; struct Cyc_List_List* _temp493; struct _tuple0* _temp495; void*
_temp497; _LL476: if(*(( int*) _temp474) == Cyc_Absyn_UnknownTunion){ _LL481:
_temp480=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp474)->f1; _LL485:
_temp484= _temp480.name; goto _LL483; _LL483: _temp482= _temp480.is_xtunion;
goto _LL477;} else{ goto _LL478;} _LL478: if(*(( int*) _temp474) == Cyc_Absyn_KnownTunion){
_LL487: _temp486=(( struct Cyc_Absyn_KnownTunion_struct*) _temp474)->f1;
_temp488=* _temp486; _LL498: _temp497=( void*) _temp488.sc; goto _LL496; _LL496:
_temp495= _temp488.name; goto _LL494; _LL494: _temp493= _temp488.tvs; goto
_LL492; _LL492: _temp491= _temp488.fields; goto _LL490; _LL490: _temp489=
_temp488.is_xtunion; goto _LL479;} else{ goto _LL475;} _LL477: _temp495=
_temp484; _temp489= _temp482; goto _LL479; _LL479: { struct Cyc_PP_Doc* _temp503=
Cyc_PP_text( _temp489?({ char* _temp499="xtunion "; struct _tagged_string
_temp500; _temp500.curr= _temp499; _temp500.base= _temp499; _temp500.last_plus_one=
_temp499 + 9; _temp500;}):({ char* _temp501="tunion "; struct _tagged_string
_temp502; _temp502.curr= _temp501; _temp502.base= _temp501; _temp502.last_plus_one=
_temp501 + 8; _temp502;})); goto _LL504; _LL504:{ void* _temp505= Cyc_Tcutil_compress(
_temp408); _LL507: if( _temp505 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL508;}
else{ goto _LL509;} _LL509: goto _LL510; _LL508: s= Cyc_Absynpp_cat3( _temp503,
Cyc_Absynpp_qvar2doc( _temp495), Cyc_Absynpp_tps2doc( _temp410)); goto _LL506;
_LL510: s= Cyc_Absynpp_cat5( _temp503, Cyc_Absynpp_typ2doc( _temp408), Cyc_PP_text(({
char* _temp511=" "; struct _tagged_string _temp512; _temp512.curr= _temp511;
_temp512.base= _temp511; _temp512.last_plus_one= _temp511 + 2; _temp512;})), Cyc_Absynpp_qvar2doc(
_temp495), Cyc_Absynpp_tps2doc( _temp410)); goto _LL506; _LL506:;} goto _LL475;}
_LL475:;} goto _LL343; _LL359:{ void* _temp513= _temp418; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp519; int _temp521; struct _tuple0* _temp523; struct _tuple0* _temp525;
struct Cyc_Absyn_Tunionfield* _temp527; struct Cyc_Absyn_Tunionfield _temp529;
void* _temp530; struct Cyc_Position_Segment* _temp532; struct Cyc_List_List*
_temp534; struct Cyc_List_List* _temp536; struct _tuple0* _temp538; struct Cyc_Absyn_Tuniondecl*
_temp540; struct Cyc_Absyn_Tuniondecl _temp542; int _temp543; struct Cyc_Core_Opt*
_temp545; struct Cyc_List_List* _temp547; struct _tuple0* _temp549; void*
_temp551; _LL515: if(*(( int*) _temp513) == Cyc_Absyn_UnknownTunionfield){
_LL520: _temp519=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp513)->f1;
_LL526: _temp525= _temp519.tunion_name; goto _LL524; _LL524: _temp523= _temp519.field_name;
goto _LL522; _LL522: _temp521= _temp519.is_xtunion; goto _LL516;} else{ goto
_LL517;} _LL517: if(*(( int*) _temp513) == Cyc_Absyn_KnownTunionfield){ _LL541:
_temp540=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp513)->f1; _temp542=*
_temp540; _LL552: _temp551=( void*) _temp542.sc; goto _LL550; _LL550: _temp549=
_temp542.name; goto _LL548; _LL548: _temp547= _temp542.tvs; goto _LL546; _LL546:
_temp545= _temp542.fields; goto _LL544; _LL544: _temp543= _temp542.is_xtunion;
goto _LL528; _LL528: _temp527=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp513)->f2; _temp529=* _temp527; _LL539: _temp538= _temp529.name; goto _LL537;
_LL537: _temp536= _temp529.tvs; goto _LL535; _LL535: _temp534= _temp529.typs;
goto _LL533; _LL533: _temp532= _temp529.loc; goto _LL531; _LL531: _temp530=(
void*) _temp529.sc; goto _LL518;} else{ goto _LL514;} _LL516: _temp549= _temp525;
_temp543= _temp521; _temp538= _temp523; goto _LL518; _LL518: { struct Cyc_PP_Doc*
_temp557= Cyc_PP_text( _temp543?({ char* _temp553="xtunion "; struct
_tagged_string _temp554; _temp554.curr= _temp553; _temp554.base= _temp553;
_temp554.last_plus_one= _temp553 + 9; _temp554;}):({ char* _temp555="tunion ";
struct _tagged_string _temp556; _temp556.curr= _temp555; _temp556.base= _temp555;
_temp556.last_plus_one= _temp555 + 8; _temp556;})); goto _LL558; _LL558: s= Cyc_Absynpp_cat4(
_temp557, Cyc_Absynpp_qvar2doc( _temp549), Cyc_PP_text(({ char* _temp559=".";
struct _tagged_string _temp560; _temp560.curr= _temp559; _temp560.base= _temp559;
_temp560.last_plus_one= _temp559 + 2; _temp560;})), Cyc_Absynpp_textptr((*
_temp538).f2)); goto _LL514;} _LL514:;} goto _LL343; _LL361: { struct
_tagged_string sns; struct _tagged_string ts;{ void* _temp561= _temp422; _LL563:
if( _temp561 ==( void*) Cyc_Absyn_Signed){ goto _LL564;} else{ goto _LL565;}
_LL565: if( _temp561 ==( void*) Cyc_Absyn_Unsigned){ goto _LL566;} else{ goto
_LL562;} _LL564: sns=({ char* _temp567=""; struct _tagged_string _temp568;
_temp568.curr= _temp567; _temp568.base= _temp567; _temp568.last_plus_one=
_temp567 + 1; _temp568;}); goto _LL562; _LL566: sns=({ char* _temp569="unsigned ";
struct _tagged_string _temp570; _temp570.curr= _temp569; _temp570.base= _temp569;
_temp570.last_plus_one= _temp569 + 10; _temp570;}); goto _LL562; _LL562:;}{ void*
_temp571= _temp420; _LL573: if( _temp571 ==( void*) Cyc_Absyn_B1){ goto _LL574;}
else{ goto _LL575;} _LL575: if( _temp571 ==( void*) Cyc_Absyn_B2){ goto _LL576;}
else{ goto _LL577;} _LL577: if( _temp571 ==( void*) Cyc_Absyn_B4){ goto _LL578;}
else{ goto _LL579;} _LL579: if( _temp571 ==( void*) Cyc_Absyn_B8){ goto _LL580;}
else{ goto _LL572;} _LL574:{ void* _temp581= _temp422; _LL583: if( _temp581 ==(
void*) Cyc_Absyn_Signed){ goto _LL584;} else{ goto _LL585;} _LL585: if( _temp581
==( void*) Cyc_Absyn_Unsigned){ goto _LL586;} else{ goto _LL582;} _LL584: sns=({
char* _temp587="signed "; struct _tagged_string _temp588; _temp588.curr=
_temp587; _temp588.base= _temp587; _temp588.last_plus_one= _temp587 + 8;
_temp588;}); goto _LL582; _LL586: sns=({ char* _temp589=""; struct
_tagged_string _temp590; _temp590.curr= _temp589; _temp590.base= _temp589;
_temp590.last_plus_one= _temp589 + 1; _temp590;}); goto _LL582; _LL582:;} ts=({
char* _temp591="char"; struct _tagged_string _temp592; _temp592.curr= _temp591;
_temp592.base= _temp591; _temp592.last_plus_one= _temp591 + 5; _temp592;}); goto
_LL572; _LL576: ts=({ char* _temp593="short"; struct _tagged_string _temp594;
_temp594.curr= _temp593; _temp594.base= _temp593; _temp594.last_plus_one=
_temp593 + 6; _temp594;}); goto _LL572; _LL578: ts=({ char* _temp595="int";
struct _tagged_string _temp596; _temp596.curr= _temp595; _temp596.base= _temp595;
_temp596.last_plus_one= _temp595 + 4; _temp596;}); goto _LL572; _LL580: ts= Cyc_Absynpp_to_VC?({
char* _temp597="__int64"; struct _tagged_string _temp598; _temp598.curr=
_temp597; _temp598.base= _temp597; _temp598.last_plus_one= _temp597 + 8;
_temp598;}):({ char* _temp599="long long"; struct _tagged_string _temp600;
_temp600.curr= _temp599; _temp600.base= _temp599; _temp600.last_plus_one=
_temp599 + 10; _temp600;}); goto _LL572; _LL572:;} s= Cyc_PP_text(({ struct
_tagged_string _temp601= sns; struct _tagged_string _temp602= ts; xprintf("%.*s%.*s",
_temp601.last_plus_one - _temp601.curr, _temp601.curr, _temp602.last_plus_one -
_temp602.curr, _temp602.curr);})); goto _LL343;} _LL363: s= Cyc_PP_text(({ char*
_temp603="float"; struct _tagged_string _temp604; _temp604.curr= _temp603;
_temp604.base= _temp603; _temp604.last_plus_one= _temp603 + 6; _temp604;}));
goto _LL343; _LL365: s= Cyc_PP_text(({ char* _temp605="double"; struct
_tagged_string _temp606; _temp606.curr= _temp605; _temp606.base= _temp605;
_temp606.last_plus_one= _temp605 + 7; _temp606;})); goto _LL343; _LL367: s= Cyc_Absynpp_cat2(
Cyc_PP_text(({ char* _temp607="$"; struct _tagged_string _temp608; _temp608.curr=
_temp607; _temp608.base= _temp607; _temp608.last_plus_one= _temp607 + 2;
_temp608;})), Cyc_Absynpp_args2doc( _temp424)); goto _LL343; _LL369: if(
_temp430 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp609="struct ";
struct _tagged_string _temp610; _temp610.curr= _temp609; _temp610.base= _temp609;
_temp610.last_plus_one= _temp609 + 8; _temp610;})), Cyc_Absynpp_tps2doc(
_temp428));} else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp611="struct ";
struct _tagged_string _temp612; _temp612.curr= _temp611; _temp612.base= _temp611;
_temp612.last_plus_one= _temp611 + 8; _temp612;})), _temp430 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp430)), Cyc_Absynpp_tps2doc(
_temp428));} goto _LL343; _LL371: if( _temp436 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(({
char* _temp613="union "; struct _tagged_string _temp614; _temp614.curr= _temp613;
_temp614.base= _temp613; _temp614.last_plus_one= _temp613 + 7; _temp614;})), Cyc_Absynpp_tps2doc(
_temp434));} else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp615="union ";
struct _tagged_string _temp616; _temp616.curr= _temp615; _temp616.base= _temp615;
_temp616.last_plus_one= _temp615 + 7; _temp616;})), _temp436 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp436)), Cyc_Absynpp_tps2doc(
_temp434));} goto _LL343; _LL373: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char*
_temp617="struct {"; struct _tagged_string _temp618; _temp618.curr= _temp617;
_temp618.base= _temp617; _temp618.last_plus_one= _temp617 + 9; _temp618;})), Cyc_PP_nest(
2, Cyc_Absynpp_structfields2doc( _temp438)), Cyc_PP_text(({ char* _temp619="}";
struct _tagged_string _temp620; _temp620.curr= _temp619; _temp620.base= _temp619;
_temp620.last_plus_one= _temp619 + 2; _temp620;}))); goto _LL343; _LL375: s= Cyc_Absynpp_cat3(
Cyc_PP_text(({ char* _temp621="union {"; struct _tagged_string _temp622;
_temp622.curr= _temp621; _temp622.base= _temp621; _temp622.last_plus_one=
_temp621 + 8; _temp622;})), Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp440)), Cyc_PP_text(({ char* _temp623="}"; struct _tagged_string _temp624;
_temp624.curr= _temp623; _temp624.base= _temp623; _temp624.last_plus_one=
_temp623 + 2; _temp624;}))); goto _LL343; _LL377: s= Cyc_Absynpp_cat2( Cyc_PP_text(({
char* _temp625="enum "; struct _tagged_string _temp626; _temp626.curr= _temp625;
_temp626.base= _temp625; _temp626.last_plus_one= _temp625 + 6; _temp626;})), Cyc_Absynpp_qvar2doc(
_temp444)); goto _LL343; _LL379: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp450), Cyc_Absynpp_tps2doc( _temp448)); goto _LL343; _LL381: s= Cyc_Absynpp_cat3(
Cyc_PP_text(({ char* _temp627="region_t<"; struct _tagged_string _temp628;
_temp628.curr= _temp627; _temp628.base= _temp627; _temp628.last_plus_one=
_temp627 + 10; _temp628;})), Cyc_Absynpp_rgn2doc( _temp452), Cyc_PP_text(({ char*
_temp629=">"; struct _tagged_string _temp630; _temp630.curr= _temp629; _temp630.base=
_temp629; _temp630.last_plus_one= _temp629 + 2; _temp630;}))); goto _LL343;
_LL383: s= Cyc_Absynpp_rgn2doc( t); goto _LL343; _LL385: s= Cyc_Absynpp_eff2doc(
t); goto _LL343; _LL387: s= Cyc_Absynpp_eff2doc( t); goto _LL343; _LL343:;}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct Cyc_Core_Opt* vo){
return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( vo))->v));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc(
struct _tuple1* t){ struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt*
_temp631=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp631->v=( void*) Cyc_PP_text(*(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null((* t).f1))->v)); _temp631;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(*
t).f3, dopt);} struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc( struct Cyc_List_List*
args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_Core_Opt*
effopt){ struct Cyc_List_List* _temp632=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(*
f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); goto _LL633; _LL633: if( effopt == 0){ if( c_varargs){ return Cyc_Absynpp_group(({
char* _temp634="("; struct _tagged_string _temp635; _temp635.curr= _temp634;
_temp635.base= _temp634; _temp635.last_plus_one= _temp634 + 2; _temp635;}),({
char* _temp636=")"; struct _tagged_string _temp637; _temp637.curr= _temp636;
_temp637.base= _temp636; _temp637.last_plus_one= _temp636 + 2; _temp637;}),({
char* _temp638=","; struct _tagged_string _temp639; _temp639.curr= _temp638;
_temp639.base= _temp638; _temp639.last_plus_one= _temp638 + 2; _temp639;}),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp632,({ struct Cyc_List_List* _temp640=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp640->hd=( void*) Cyc_PP_text(({ char*
_temp641="..."; struct _tagged_string _temp642; _temp642.curr= _temp641;
_temp642.base= _temp641; _temp642.last_plus_one= _temp641 + 4; _temp642;}));
_temp640->tl= 0; _temp640;})));} else{ if( cyc_varargs != 0){ struct Cyc_PP_Doc*
_temp644= Cyc_Absynpp_funarg2doc(({ struct _tuple1* _temp643=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp643->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->name; _temp643->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->tq; _temp643->f3=( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->type; _temp643;})); goto _LL645; _LL645: { struct
Cyc_PP_Doc* _temp652= Cyc_Absynpp_cat4( Cyc_PP_text(({ char* _temp646="...";
struct _tagged_string _temp647; _temp647.curr= _temp646; _temp647.base= _temp646;
_temp647.last_plus_one= _temp646 + 4; _temp647;})), Cyc_Absynpp_typ2doc(( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->rgn),(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject? Cyc_PP_text(({ char* _temp648=" inject ";
struct _tagged_string _temp649; _temp649.curr= _temp648; _temp649.base= _temp648;
_temp649.last_plus_one= _temp648 + 9; _temp649;})): Cyc_PP_text(({ char*
_temp650=""; struct _tagged_string _temp651; _temp651.curr= _temp650; _temp651.base=
_temp650; _temp651.last_plus_one= _temp650 + 1; _temp651;})), _temp644); goto
_LL653; _LL653: return Cyc_Absynpp_group(({ char* _temp654="("; struct
_tagged_string _temp655; _temp655.curr= _temp654; _temp655.base= _temp654;
_temp655.last_plus_one= _temp654 + 2; _temp655;}),({ char* _temp656=")"; struct
_tagged_string _temp657; _temp657.curr= _temp656; _temp657.base= _temp656;
_temp657.last_plus_one= _temp656 + 2; _temp657;}),({ char* _temp658=","; struct
_tagged_string _temp659; _temp659.curr= _temp658; _temp659.base= _temp658;
_temp659.last_plus_one= _temp658 + 2; _temp659;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp632,({
struct Cyc_List_List* _temp660=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp660->hd=( void*) _temp652; _temp660->tl= 0;
_temp660;})));}} else{ return Cyc_Absynpp_group(({ char* _temp661="("; struct
_tagged_string _temp662; _temp662.curr= _temp661; _temp662.base= _temp661;
_temp662.last_plus_one= _temp661 + 2; _temp662;}),({ char* _temp663=")"; struct
_tagged_string _temp664; _temp664.curr= _temp663; _temp664.base= _temp663;
_temp664.last_plus_one= _temp663 + 2; _temp664;}),({ char* _temp665=","; struct
_tagged_string _temp666; _temp666.curr= _temp665; _temp666.base= _temp665;
_temp666.last_plus_one= _temp665 + 2; _temp666;}), _temp632);}}} else{ if(
c_varargs){ return Cyc_Absynpp_group(({ char* _temp667="("; struct
_tagged_string _temp668; _temp668.curr= _temp667; _temp668.base= _temp667;
_temp668.last_plus_one= _temp667 + 2; _temp668;}),({ char* _temp669=")"; struct
_tagged_string _temp670; _temp670.curr= _temp669; _temp670.base= _temp669;
_temp670.last_plus_one= _temp669 + 2; _temp670;}),({ char* _temp671=";"; struct
_tagged_string _temp672; _temp672.curr= _temp671; _temp672.base= _temp671;
_temp672.last_plus_one= _temp671 + 2; _temp672;}),({ struct Cyc_List_List*
_temp673=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp673->hd=( void*) Cyc_Absynpp_group(({ char* _temp675=""; struct
_tagged_string _temp676; _temp676.curr= _temp675; _temp676.base= _temp675;
_temp676.last_plus_one= _temp675 + 1; _temp676;}),({ char* _temp677=""; struct
_tagged_string _temp678; _temp678.curr= _temp677; _temp678.base= _temp677;
_temp678.last_plus_one= _temp677 + 1; _temp678;}),({ char* _temp679=","; struct
_tagged_string _temp680; _temp680.curr= _temp679; _temp680.base= _temp679;
_temp680.last_plus_one= _temp679 + 2; _temp680;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp632,({
struct Cyc_List_List* _temp681=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp681->hd=( void*) Cyc_PP_text(({ char* _temp682="...";
struct _tagged_string _temp683; _temp683.curr= _temp682; _temp683.base= _temp682;
_temp683.last_plus_one= _temp682 + 4; _temp683;})); _temp681->tl= 0; _temp681;})));
_temp673->tl=({ struct Cyc_List_List* _temp674=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp674->hd=( void*) Cyc_Absynpp_eff2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v); _temp674->tl= 0;
_temp674;}); _temp673;}));} else{ if( cyc_varargs != 0){ struct Cyc_PP_Doc*
_temp685= Cyc_Absynpp_funarg2doc(({ struct _tuple1* _temp684=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp684->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->name; _temp684->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->tq; _temp684->f3=( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->type; _temp684;})); goto _LL686; _LL686: { struct
Cyc_PP_Doc* _temp693= Cyc_Absynpp_cat4( Cyc_PP_text(({ char* _temp687="...";
struct _tagged_string _temp688; _temp688.curr= _temp687; _temp688.base= _temp687;
_temp688.last_plus_one= _temp687 + 4; _temp688;})), Cyc_Absynpp_typ2doc(( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->rgn),(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject? Cyc_PP_text(({ char* _temp689=" inject ";
struct _tagged_string _temp690; _temp690.curr= _temp689; _temp690.base= _temp689;
_temp690.last_plus_one= _temp689 + 9; _temp690;})): Cyc_PP_text(({ char*
_temp691=""; struct _tagged_string _temp692; _temp692.curr= _temp691; _temp692.base=
_temp691; _temp692.last_plus_one= _temp691 + 1; _temp692;})), _temp685); goto
_LL694; _LL694: return Cyc_Absynpp_group(({ char* _temp695="("; struct
_tagged_string _temp696; _temp696.curr= _temp695; _temp696.base= _temp695;
_temp696.last_plus_one= _temp695 + 2; _temp696;}),({ char* _temp697=")"; struct
_tagged_string _temp698; _temp698.curr= _temp697; _temp698.base= _temp697;
_temp698.last_plus_one= _temp697 + 2; _temp698;}),({ char* _temp699=";"; struct
_tagged_string _temp700; _temp700.curr= _temp699; _temp700.base= _temp699;
_temp700.last_plus_one= _temp699 + 2; _temp700;}),({ struct Cyc_List_List*
_temp701=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp701->hd=( void*) Cyc_Absynpp_group(({ char* _temp703=""; struct
_tagged_string _temp704; _temp704.curr= _temp703; _temp704.base= _temp703;
_temp704.last_plus_one= _temp703 + 1; _temp704;}),({ char* _temp705=""; struct
_tagged_string _temp706; _temp706.curr= _temp705; _temp706.base= _temp705;
_temp706.last_plus_one= _temp705 + 1; _temp706;}),({ char* _temp707=","; struct
_tagged_string _temp708; _temp708.curr= _temp707; _temp708.base= _temp707;
_temp708.last_plus_one= _temp707 + 2; _temp708;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp632,({
struct Cyc_List_List* _temp709=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp709->hd=( void*) _temp693; _temp709->tl= 0;
_temp709;}))); _temp701->tl=({ struct Cyc_List_List* _temp702=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp702->hd=( void*) Cyc_Absynpp_eff2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v); _temp702->tl= 0;
_temp702;}); _temp701;}));}} else{ return Cyc_Absynpp_group(({ char* _temp710="(";
struct _tagged_string _temp711; _temp711.curr= _temp710; _temp711.base= _temp710;
_temp711.last_plus_one= _temp710 + 2; _temp711;}),({ char* _temp712=")"; struct
_tagged_string _temp713; _temp713.curr= _temp712; _temp713.base= _temp712;
_temp713.last_plus_one= _temp712 + 2; _temp713;}),({ char* _temp714=";"; struct
_tagged_string _temp715; _temp715.curr= _temp714; _temp715.base= _temp714;
_temp715.last_plus_one= _temp714 + 2; _temp715;}),({ struct Cyc_List_List*
_temp716=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp716->hd=( void*) Cyc_Absynpp_group(({ char* _temp718=""; struct
_tagged_string _temp719; _temp719.curr= _temp718; _temp719.base= _temp718;
_temp719.last_plus_one= _temp718 + 1; _temp719;}),({ char* _temp720=""; struct
_tagged_string _temp721; _temp721.curr= _temp720; _temp721.base= _temp720;
_temp721.last_plus_one= _temp720 + 1; _temp721;}),({ char* _temp722=","; struct
_tagged_string _temp723; _temp723.curr= _temp722; _temp723.base= _temp722;
_temp723.last_plus_one= _temp722 + 2; _temp723;}), _temp632); _temp716->tl=({
struct Cyc_List_List* _temp717=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp717->hd=( void*) Cyc_Absynpp_eff2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( effopt))->v); _temp717->tl= 0; _temp717;});
_temp716;}));}}}} struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg){
return({ struct _tuple1* _temp724=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp724->f1=({ struct Cyc_Core_Opt* _temp725=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp725->v=( void*)(* arg).f1;
_temp725;}); _temp724->f2=(* arg).f2; _temp724->f3=(* arg).f3; _temp724;});}
struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct _tagged_string* v){ return Cyc_PP_text(*
v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
_temp726= 0; goto _LL727; _LL727:{ void* _temp728=(* q).f1; struct Cyc_List_List*
_temp740; struct Cyc_List_List* _temp742; struct Cyc_List_List* _temp744; struct
Cyc_List_List* _temp746; _LL730: if( _temp728 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL731;} else{ goto _LL732;} _LL732: if(( unsigned int) _temp728 > 1u?*(( int*)
_temp728) == Cyc_Absyn_Rel_n: 0){ _LL741: _temp740=(( struct Cyc_Absyn_Rel_n_struct*)
_temp728)->f1; if( _temp740 == 0){ goto _LL733;} else{ goto _LL734;}} else{ goto
_LL734;} _LL734: if(( unsigned int) _temp728 > 1u?*(( int*) _temp728) == Cyc_Absyn_Abs_n:
0){ _LL743: _temp742=(( struct Cyc_Absyn_Abs_n_struct*) _temp728)->f1; if(
_temp742 == 0){ goto _LL735;} else{ goto _LL736;}} else{ goto _LL736;} _LL736:
if(( unsigned int) _temp728 > 1u?*(( int*) _temp728) == Cyc_Absyn_Rel_n: 0){
_LL745: _temp744=(( struct Cyc_Absyn_Rel_n_struct*) _temp728)->f1; goto _LL737;}
else{ goto _LL738;} _LL738: if(( unsigned int) _temp728 > 1u?*(( int*) _temp728)
== Cyc_Absyn_Abs_n: 0){ _LL747: _temp746=(( struct Cyc_Absyn_Abs_n_struct*)
_temp728)->f1; goto _LL739;} else{ goto _LL729;} _LL731: goto _LL733; _LL733:
goto _LL735; _LL735: return Cyc_Absynpp_var2doc((* q).f2); _LL737: _temp746=
_temp744; goto _LL739; _LL739: _temp726= _temp746; goto _LL729; _LL729:;} if(
Cyc_Absynpp_qvar_to_Cids){ return Cyc_PP_text( Cyc_String_str_sepstr((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp726,({ struct Cyc_List_List* _temp748=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp748->hd=( void*)(* q).f2; _temp748->tl= 0;
_temp748;})),({ char* _temp749="_"; struct _tagged_string _temp750; _temp750.curr=
_temp749; _temp750.base= _temp749; _temp750.last_plus_one= _temp749 + 2;
_temp750;})));} else{ if((( int(*)( int(* cmp)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_prefix)(
Cyc_String_strptrcmp, _temp726, Cyc_Absynpp_curr_namespace)){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp726,({
struct Cyc_List_List* _temp751=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp751->hd=( void*)(* q).f2; _temp751->tl= 0; _temp751;})),({
char* _temp752="::"; struct _tagged_string _temp753; _temp753.curr= _temp752;
_temp753.base= _temp752; _temp753.last_plus_one= _temp752 + 3; _temp753;})));}}}
struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);}{ void* _temp754=(* v).f1; struct Cyc_List_List*
_temp760; _LL756: if(( unsigned int) _temp754 > 1u?*(( int*) _temp754) == Cyc_Absyn_Abs_n:
0){ _LL761: _temp760=(( struct Cyc_Absyn_Abs_n_struct*) _temp754)->f1; goto
_LL757;} else{ goto _LL758;} _LL758: goto _LL759; _LL757: if((( int(*)( int(*
cmp)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp760,
Cyc_Absynpp_curr_namespace) == 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{
return Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp762="/* wrong namespace : */ ";
struct _tagged_string _temp763; _temp763.curr= _temp762; _temp763.base= _temp762;
_temp763.last_plus_one= _temp762 + 25; _temp763;})), Cyc_Absynpp_qvar2doc( v));}
_LL759: return Cyc_Absynpp_qvar2doc( v); _LL755:;}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc(
void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp* e){ void* _temp764=( void*) e->r; void* _temp836; void*
_temp838; struct _tuple0* _temp840; struct _tuple0* _temp842; struct Cyc_List_List*
_temp844; void* _temp846; struct Cyc_Absyn_Exp* _temp848; struct Cyc_Core_Opt*
_temp850; struct Cyc_Absyn_Exp* _temp852; void* _temp854; struct Cyc_Absyn_Exp*
_temp856; struct Cyc_Absyn_Exp* _temp858; struct Cyc_Absyn_Exp* _temp860; struct
Cyc_Absyn_Exp* _temp862; struct Cyc_Absyn_Exp* _temp864; struct Cyc_Absyn_Exp*
_temp866; struct Cyc_List_List* _temp868; struct Cyc_Absyn_Exp* _temp870; struct
Cyc_Absyn_VarargCallInfo* _temp872; struct Cyc_List_List* _temp874; struct Cyc_Absyn_Exp*
_temp876; struct Cyc_Absyn_Exp* _temp878; struct Cyc_Absyn_Exp* _temp880; struct
Cyc_List_List* _temp882; struct Cyc_Absyn_Exp* _temp884; struct Cyc_Absyn_Exp*
_temp886; void* _temp888; struct Cyc_Absyn_Exp* _temp890; struct Cyc_Absyn_Exp*
_temp892; struct Cyc_Absyn_Exp* _temp894; void* _temp896; struct Cyc_Absyn_Exp*
_temp898; struct Cyc_Absyn_Exp* _temp900; struct _tagged_string* _temp902;
struct Cyc_Absyn_Exp* _temp904; struct _tagged_string* _temp906; struct Cyc_Absyn_Exp*
_temp908; struct Cyc_Absyn_Exp* _temp910; struct Cyc_Absyn_Exp* _temp912; struct
Cyc_List_List* _temp914; struct Cyc_List_List* _temp916; struct _tuple1*
_temp918; struct Cyc_List_List* _temp920; struct Cyc_Absyn_Exp* _temp922; struct
Cyc_Absyn_Exp* _temp924; struct Cyc_Absyn_Vardecl* _temp926; struct Cyc_Absyn_Structdecl*
_temp928; struct Cyc_List_List* _temp930; struct Cyc_Core_Opt* _temp932; struct
_tuple0* _temp934; struct Cyc_List_List* _temp936; void* _temp938; struct Cyc_Absyn_Tunionfield*
_temp940; struct Cyc_Absyn_Tuniondecl* _temp942; struct Cyc_List_List* _temp944;
struct Cyc_Core_Opt* _temp946; struct Cyc_Core_Opt* _temp948; struct Cyc_Absyn_Enumfield*
_temp950; struct Cyc_Absyn_Enumdecl* _temp952; struct _tuple0* _temp954; void*
_temp956; struct Cyc_Absyn_Exp* _temp958; struct Cyc_List_List* _temp960; struct
Cyc_Core_Opt* _temp962; struct Cyc_Absyn_Stmt* _temp964; struct Cyc_Absyn_Fndecl*
_temp966; struct Cyc_Absyn_Exp* _temp968; _LL766: if(*(( int*) _temp764) == Cyc_Absyn_Const_e){
_LL837: _temp836=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp764)->f1;
goto _LL767;} else{ goto _LL768;} _LL768: if(*(( int*) _temp764) == Cyc_Absyn_Var_e){
_LL841: _temp840=(( struct Cyc_Absyn_Var_e_struct*) _temp764)->f1; goto _LL839;
_LL839: _temp838=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp764)->f2; goto
_LL769;} else{ goto _LL770;} _LL770: if(*(( int*) _temp764) == Cyc_Absyn_UnknownId_e){
_LL843: _temp842=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp764)->f1; goto
_LL771;} else{ goto _LL772;} _LL772: if(*(( int*) _temp764) == Cyc_Absyn_Primop_e){
_LL847: _temp846=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp764)->f1;
goto _LL845; _LL845: _temp844=(( struct Cyc_Absyn_Primop_e_struct*) _temp764)->f2;
goto _LL773;} else{ goto _LL774;} _LL774: if(*(( int*) _temp764) == Cyc_Absyn_AssignOp_e){
_LL853: _temp852=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp764)->f1; goto
_LL851; _LL851: _temp850=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp764)->f2;
goto _LL849; _LL849: _temp848=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp764)->f3;
goto _LL775;} else{ goto _LL776;} _LL776: if(*(( int*) _temp764) == Cyc_Absyn_Increment_e){
_LL857: _temp856=(( struct Cyc_Absyn_Increment_e_struct*) _temp764)->f1; goto
_LL855; _LL855: _temp854=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp764)->f2; goto _LL777;} else{ goto _LL778;} _LL778: if(*(( int*) _temp764)
== Cyc_Absyn_Conditional_e){ _LL863: _temp862=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp764)->f1; goto _LL861; _LL861: _temp860=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp764)->f2; goto _LL859; _LL859: _temp858=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp764)->f3; goto _LL779;} else{ goto _LL780;} _LL780: if(*(( int*) _temp764)
== Cyc_Absyn_SeqExp_e){ _LL867: _temp866=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp764)->f1; goto _LL865; _LL865: _temp864=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp764)->f2; goto _LL781;} else{ goto _LL782;} _LL782: if(*(( int*) _temp764)
== Cyc_Absyn_UnknownCall_e){ _LL871: _temp870=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp764)->f1; goto _LL869; _LL869: _temp868=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp764)->f2; goto _LL783;} else{ goto _LL784;} _LL784: if(*(( int*) _temp764)
== Cyc_Absyn_FnCall_e){ _LL877: _temp876=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp764)->f1; goto _LL875; _LL875: _temp874=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp764)->f2; goto _LL873; _LL873: _temp872=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp764)->f3; goto _LL785;} else{ goto _LL786;} _LL786: if(*(( int*) _temp764)
== Cyc_Absyn_Throw_e){ _LL879: _temp878=(( struct Cyc_Absyn_Throw_e_struct*)
_temp764)->f1; goto _LL787;} else{ goto _LL788;} _LL788: if(*(( int*) _temp764)
== Cyc_Absyn_NoInstantiate_e){ _LL881: _temp880=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp764)->f1; goto _LL789;} else{ goto _LL790;} _LL790: if(*(( int*) _temp764)
== Cyc_Absyn_Instantiate_e){ _LL885: _temp884=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp764)->f1; goto _LL883; _LL883: _temp882=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp764)->f2; goto _LL791;} else{ goto _LL792;} _LL792: if(*(( int*) _temp764)
== Cyc_Absyn_Cast_e){ _LL889: _temp888=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp764)->f1; goto _LL887; _LL887: _temp886=(( struct Cyc_Absyn_Cast_e_struct*)
_temp764)->f2; goto _LL793;} else{ goto _LL794;} _LL794: if(*(( int*) _temp764)
== Cyc_Absyn_New_e){ _LL893: _temp892=(( struct Cyc_Absyn_New_e_struct*)
_temp764)->f1; goto _LL891; _LL891: _temp890=(( struct Cyc_Absyn_New_e_struct*)
_temp764)->f2; goto _LL795;} else{ goto _LL796;} _LL796: if(*(( int*) _temp764)
== Cyc_Absyn_Address_e){ _LL895: _temp894=(( struct Cyc_Absyn_Address_e_struct*)
_temp764)->f1; goto _LL797;} else{ goto _LL798;} _LL798: if(*(( int*) _temp764)
== Cyc_Absyn_Sizeoftyp_e){ _LL897: _temp896=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp764)->f1; goto _LL799;} else{ goto _LL800;} _LL800: if(*(( int*) _temp764)
== Cyc_Absyn_Sizeofexp_e){ _LL899: _temp898=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp764)->f1; goto _LL801;} else{ goto _LL802;} _LL802: if(*(( int*) _temp764)
== Cyc_Absyn_Deref_e){ _LL901: _temp900=(( struct Cyc_Absyn_Deref_e_struct*)
_temp764)->f1; goto _LL803;} else{ goto _LL804;} _LL804: if(*(( int*) _temp764)
== Cyc_Absyn_StructMember_e){ _LL905: _temp904=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp764)->f1; goto _LL903; _LL903: _temp902=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp764)->f2; goto _LL805;} else{ goto _LL806;} _LL806: if(*(( int*) _temp764)
== Cyc_Absyn_StructArrow_e){ _LL909: _temp908=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp764)->f1; goto _LL907; _LL907: _temp906=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp764)->f2; goto _LL807;} else{ goto _LL808;} _LL808: if(*(( int*) _temp764)
== Cyc_Absyn_Subscript_e){ _LL913: _temp912=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp764)->f1; goto _LL911; _LL911: _temp910=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp764)->f2; goto _LL809;} else{ goto _LL810;} _LL810: if(*(( int*) _temp764)
== Cyc_Absyn_Tuple_e){ _LL915: _temp914=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp764)->f1; goto _LL811;} else{ goto _LL812;} _LL812: if(*(( int*) _temp764)
== Cyc_Absyn_CompoundLit_e){ _LL919: _temp918=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp764)->f1; goto _LL917; _LL917: _temp916=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp764)->f2; goto _LL813;} else{ goto _LL814;} _LL814: if(*(( int*) _temp764)
== Cyc_Absyn_Array_e){ _LL921: _temp920=(( struct Cyc_Absyn_Array_e_struct*)
_temp764)->f1; goto _LL815;} else{ goto _LL816;} _LL816: if(*(( int*) _temp764)
== Cyc_Absyn_Comprehension_e){ _LL927: _temp926=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp764)->f1; goto _LL925; _LL925: _temp924=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp764)->f2; goto _LL923; _LL923: _temp922=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp764)->f3; goto _LL817;} else{ goto _LL818;} _LL818: if(*(( int*) _temp764)
== Cyc_Absyn_Struct_e){ _LL935: _temp934=(( struct Cyc_Absyn_Struct_e_struct*)
_temp764)->f1; goto _LL933; _LL933: _temp932=(( struct Cyc_Absyn_Struct_e_struct*)
_temp764)->f2; goto _LL931; _LL931: _temp930=(( struct Cyc_Absyn_Struct_e_struct*)
_temp764)->f3; goto _LL929; _LL929: _temp928=(( struct Cyc_Absyn_Struct_e_struct*)
_temp764)->f4; goto _LL819;} else{ goto _LL820;} _LL820: if(*(( int*) _temp764)
== Cyc_Absyn_AnonStruct_e){ _LL939: _temp938=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp764)->f1; goto _LL937; _LL937: _temp936=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp764)->f2; goto _LL821;} else{ goto _LL822;} _LL822: if(*(( int*) _temp764)
== Cyc_Absyn_Tunion_e){ _LL949: _temp948=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp764)->f1; goto _LL947; _LL947: _temp946=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp764)->f2; goto _LL945; _LL945: _temp944=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp764)->f3; goto _LL943; _LL943: _temp942=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp764)->f4; goto _LL941; _LL941: _temp940=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp764)->f5; goto _LL823;} else{ goto _LL824;} _LL824: if(*(( int*) _temp764)
== Cyc_Absyn_Enum_e){ _LL955: _temp954=(( struct Cyc_Absyn_Enum_e_struct*)
_temp764)->f1; goto _LL953; _LL953: _temp952=(( struct Cyc_Absyn_Enum_e_struct*)
_temp764)->f2; goto _LL951; _LL951: _temp950=(( struct Cyc_Absyn_Enum_e_struct*)
_temp764)->f3; goto _LL825;} else{ goto _LL826;} _LL826: if(*(( int*) _temp764)
== Cyc_Absyn_Malloc_e){ _LL959: _temp958=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp764)->f1; goto _LL957; _LL957: _temp956=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp764)->f2; goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp764)
== Cyc_Absyn_UnresolvedMem_e){ _LL963: _temp962=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp764)->f1; goto _LL961; _LL961: _temp960=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp764)->f2; goto _LL829;} else{ goto _LL830;} _LL830: if(*(( int*) _temp764)
== Cyc_Absyn_StmtExp_e){ _LL965: _temp964=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp764)->f1; goto _LL831;} else{ goto _LL832;} _LL832: if(*(( int*) _temp764)
== Cyc_Absyn_Codegen_e){ _LL967: _temp966=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp764)->f1; goto _LL833;} else{ goto _LL834;} _LL834: if(*(( int*) _temp764)
== Cyc_Absyn_Fill_e){ _LL969: _temp968=(( struct Cyc_Absyn_Fill_e_struct*)
_temp764)->f1; goto _LL835;} else{ goto _LL765;} _LL767: return 10000; _LL769:
return 10000; _LL771: return 10000; _LL773: { void* _temp970= _temp846; _LL972:
if( _temp970 ==( void*) Cyc_Absyn_Plus){ goto _LL973;} else{ goto _LL974;}
_LL974: if( _temp970 ==( void*) Cyc_Absyn_Times){ goto _LL975;} else{ goto
_LL976;} _LL976: if( _temp970 ==( void*) Cyc_Absyn_Minus){ goto _LL977;} else{
goto _LL978;} _LL978: if( _temp970 ==( void*) Cyc_Absyn_Div){ goto _LL979;}
else{ goto _LL980;} _LL980: if( _temp970 ==( void*) Cyc_Absyn_Mod){ goto _LL981;}
else{ goto _LL982;} _LL982: if( _temp970 ==( void*) Cyc_Absyn_Eq){ goto _LL983;}
else{ goto _LL984;} _LL984: if( _temp970 ==( void*) Cyc_Absyn_Neq){ goto _LL985;}
else{ goto _LL986;} _LL986: if( _temp970 ==( void*) Cyc_Absyn_Gt){ goto _LL987;}
else{ goto _LL988;} _LL988: if( _temp970 ==( void*) Cyc_Absyn_Lt){ goto _LL989;}
else{ goto _LL990;} _LL990: if( _temp970 ==( void*) Cyc_Absyn_Gte){ goto _LL991;}
else{ goto _LL992;} _LL992: if( _temp970 ==( void*) Cyc_Absyn_Lte){ goto _LL993;}
else{ goto _LL994;} _LL994: if( _temp970 ==( void*) Cyc_Absyn_Not){ goto _LL995;}
else{ goto _LL996;} _LL996: if( _temp970 ==( void*) Cyc_Absyn_Bitnot){ goto
_LL997;} else{ goto _LL998;} _LL998: if( _temp970 ==( void*) Cyc_Absyn_Bitand){
goto _LL999;} else{ goto _LL1000;} _LL1000: if( _temp970 ==( void*) Cyc_Absyn_Bitor){
goto _LL1001;} else{ goto _LL1002;} _LL1002: if( _temp970 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1003;} else{ goto _LL1004;} _LL1004: if( _temp970 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1005;} else{ goto _LL1006;} _LL1006: if( _temp970 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1007;} else{ goto _LL1008;} _LL1008: if( _temp970 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1009;} else{ goto _LL1010;} _LL1010: if( _temp970 ==( void*) Cyc_Absyn_Size){
goto _LL1011;} else{ goto _LL1012;} _LL1012: if( _temp970 ==( void*) Cyc_Absyn_Printf){
goto _LL1013;} else{ goto _LL1014;} _LL1014: if( _temp970 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1015;} else{ goto _LL1016;} _LL1016: if( _temp970 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1017;} else{ goto _LL1018;} _LL1018: if( _temp970 ==( void*) Cyc_Absyn_Scanf){
goto _LL1019;} else{ goto _LL1020;} _LL1020: if( _temp970 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1021;} else{ goto _LL1022;} _LL1022: if( _temp970 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1023;} else{ goto _LL971;} _LL973: return 100; _LL975: return 110;
_LL977: return 100; _LL979: return 110; _LL981: return 110; _LL983: return 70;
_LL985: return 70; _LL987: return 80; _LL989: return 80; _LL991: return 80;
_LL993: return 80; _LL995: return 130; _LL997: return 130; _LL999: return 60;
_LL1001: return 40; _LL1003: return 50; _LL1005: return 90; _LL1007: return 80;
_LL1009: return 80; _LL1011: return 140; _LL1013: return 130; _LL1015: return
130; _LL1017: return 130; _LL1019: return 130; _LL1021: return 130; _LL1023:
return 130; _LL971:;} _LL775: return 20; _LL777: return 130; _LL779: return 30;
_LL781: return 10; _LL783: return 140; _LL785: return 140; _LL787: return 130;
_LL789: return Cyc_Absynpp_exp_prec( _temp880); _LL791: return Cyc_Absynpp_exp_prec(
_temp884); _LL793: return 120; _LL795: return 15; _LL797: return 130; _LL799:
return 130; _LL801: return 130; _LL803: return 130; _LL805: return 140; _LL807:
return 140; _LL809: return 140; _LL811: return 150; _LL813: goto _LL815; _LL815:
goto _LL817; _LL817: goto _LL819; _LL819: goto _LL821; _LL821: goto _LL823;
_LL823: goto _LL825; _LL825: goto _LL827; _LL827: return 140; _LL829: return 140;
_LL831: return 10000; _LL833: return 140; _LL835: return 140; _LL765:;} struct
Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct _tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e);
struct Cyc_PP_Doc* s;{ void* _temp1024=( void*) e->r; void* _temp1096; void*
_temp1098; struct _tuple0* _temp1100; struct _tuple0* _temp1102; struct Cyc_List_List*
_temp1104; void* _temp1106; struct Cyc_Absyn_Exp* _temp1108; struct Cyc_Core_Opt*
_temp1110; struct Cyc_Absyn_Exp* _temp1112; void* _temp1114; struct Cyc_Absyn_Exp*
_temp1116; struct Cyc_Absyn_Exp* _temp1118; struct Cyc_Absyn_Exp* _temp1120;
struct Cyc_Absyn_Exp* _temp1122; struct Cyc_Absyn_Exp* _temp1124; struct Cyc_Absyn_Exp*
_temp1126; struct Cyc_List_List* _temp1128; struct Cyc_Absyn_Exp* _temp1130;
struct Cyc_Absyn_VarargCallInfo* _temp1132; struct Cyc_List_List* _temp1134;
struct Cyc_Absyn_Exp* _temp1136; struct Cyc_Absyn_Exp* _temp1138; struct Cyc_Absyn_Exp*
_temp1140; struct Cyc_List_List* _temp1142; struct Cyc_Absyn_Exp* _temp1144;
struct Cyc_Absyn_Exp* _temp1146; void* _temp1148; struct Cyc_Absyn_Exp*
_temp1150; struct Cyc_Absyn_Exp* _temp1152; struct Cyc_Absyn_Exp* _temp1154;
void* _temp1156; struct Cyc_Absyn_Exp* _temp1158; struct Cyc_Absyn_Exp*
_temp1160; struct _tagged_string* _temp1162; struct Cyc_Absyn_Exp* _temp1164;
struct _tagged_string* _temp1166; struct Cyc_Absyn_Exp* _temp1168; struct Cyc_Absyn_Exp*
_temp1170; struct Cyc_Absyn_Exp* _temp1172; struct Cyc_List_List* _temp1174;
struct Cyc_List_List* _temp1176; struct _tuple1* _temp1178; struct Cyc_List_List*
_temp1180; struct Cyc_Absyn_Exp* _temp1182; struct Cyc_Absyn_Exp* _temp1184;
struct Cyc_Absyn_Vardecl* _temp1186; struct Cyc_Absyn_Structdecl* _temp1188;
struct Cyc_List_List* _temp1190; struct Cyc_Core_Opt* _temp1192; struct _tuple0*
_temp1194; struct Cyc_List_List* _temp1196; void* _temp1198; struct Cyc_Absyn_Tunionfield*
_temp1200; struct Cyc_Absyn_Tuniondecl* _temp1202; struct Cyc_List_List*
_temp1204; struct Cyc_Core_Opt* _temp1206; struct Cyc_Core_Opt* _temp1208;
struct Cyc_Absyn_Enumfield* _temp1210; struct Cyc_Absyn_Enumdecl* _temp1212;
struct _tuple0* _temp1214; void* _temp1216; struct Cyc_Absyn_Exp* _temp1218;
struct Cyc_List_List* _temp1220; struct Cyc_Core_Opt* _temp1222; struct Cyc_Absyn_Stmt*
_temp1224; struct Cyc_Absyn_Fndecl* _temp1226; struct Cyc_Absyn_Exp* _temp1228;
_LL1026: if(*(( int*) _temp1024) == Cyc_Absyn_Const_e){ _LL1097: _temp1096=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1024)->f1; goto _LL1027;} else{
goto _LL1028;} _LL1028: if(*(( int*) _temp1024) == Cyc_Absyn_Var_e){ _LL1101:
_temp1100=(( struct Cyc_Absyn_Var_e_struct*) _temp1024)->f1; goto _LL1099;
_LL1099: _temp1098=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1024)->f2;
goto _LL1029;} else{ goto _LL1030;} _LL1030: if(*(( int*) _temp1024) == Cyc_Absyn_UnknownId_e){
_LL1103: _temp1102=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1024)->f1; goto
_LL1031;} else{ goto _LL1032;} _LL1032: if(*(( int*) _temp1024) == Cyc_Absyn_Primop_e){
_LL1107: _temp1106=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1024)->f1;
goto _LL1105; _LL1105: _temp1104=(( struct Cyc_Absyn_Primop_e_struct*) _temp1024)->f2;
goto _LL1033;} else{ goto _LL1034;} _LL1034: if(*(( int*) _temp1024) == Cyc_Absyn_AssignOp_e){
_LL1113: _temp1112=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1024)->f1; goto
_LL1111; _LL1111: _temp1110=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1024)->f2;
goto _LL1109; _LL1109: _temp1108=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1024)->f3; goto _LL1035;} else{ goto _LL1036;} _LL1036: if(*(( int*)
_temp1024) == Cyc_Absyn_Increment_e){ _LL1117: _temp1116=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1024)->f1; goto _LL1115; _LL1115: _temp1114=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1024)->f2; goto _LL1037;} else{ goto _LL1038;} _LL1038: if(*(( int*)
_temp1024) == Cyc_Absyn_Conditional_e){ _LL1123: _temp1122=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1024)->f1; goto _LL1121; _LL1121: _temp1120=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1024)->f2; goto _LL1119; _LL1119: _temp1118=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1024)->f3; goto _LL1039;} else{ goto _LL1040;} _LL1040: if(*(( int*)
_temp1024) == Cyc_Absyn_SeqExp_e){ _LL1127: _temp1126=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1024)->f1; goto _LL1125; _LL1125: _temp1124=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1024)->f2; goto _LL1041;} else{ goto _LL1042;} _LL1042: if(*(( int*)
_temp1024) == Cyc_Absyn_UnknownCall_e){ _LL1131: _temp1130=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1024)->f1; goto _LL1129; _LL1129: _temp1128=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1024)->f2; goto _LL1043;} else{ goto _LL1044;} _LL1044: if(*(( int*)
_temp1024) == Cyc_Absyn_FnCall_e){ _LL1137: _temp1136=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1024)->f1; goto _LL1135; _LL1135: _temp1134=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1024)->f2; goto _LL1133; _LL1133: _temp1132=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1024)->f3; goto _LL1045;} else{ goto _LL1046;} _LL1046: if(*(( int*)
_temp1024) == Cyc_Absyn_Throw_e){ _LL1139: _temp1138=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1024)->f1; goto _LL1047;} else{ goto _LL1048;} _LL1048: if(*(( int*)
_temp1024) == Cyc_Absyn_NoInstantiate_e){ _LL1141: _temp1140=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1024)->f1; goto _LL1049;} else{ goto _LL1050;} _LL1050: if(*(( int*)
_temp1024) == Cyc_Absyn_Instantiate_e){ _LL1145: _temp1144=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1024)->f1; goto _LL1143; _LL1143: _temp1142=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1024)->f2; goto _LL1051;} else{ goto _LL1052;} _LL1052: if(*(( int*)
_temp1024) == Cyc_Absyn_Cast_e){ _LL1149: _temp1148=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1024)->f1; goto _LL1147; _LL1147: _temp1146=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1024)->f2; goto _LL1053;} else{ goto _LL1054;} _LL1054: if(*(( int*)
_temp1024) == Cyc_Absyn_Address_e){ _LL1151: _temp1150=(( struct Cyc_Absyn_Address_e_struct*)
_temp1024)->f1; goto _LL1055;} else{ goto _LL1056;} _LL1056: if(*(( int*)
_temp1024) == Cyc_Absyn_New_e){ _LL1155: _temp1154=(( struct Cyc_Absyn_New_e_struct*)
_temp1024)->f1; goto _LL1153; _LL1153: _temp1152=(( struct Cyc_Absyn_New_e_struct*)
_temp1024)->f2; goto _LL1057;} else{ goto _LL1058;} _LL1058: if(*(( int*)
_temp1024) == Cyc_Absyn_Sizeoftyp_e){ _LL1157: _temp1156=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1024)->f1; goto _LL1059;} else{ goto _LL1060;} _LL1060: if(*(( int*)
_temp1024) == Cyc_Absyn_Sizeofexp_e){ _LL1159: _temp1158=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1024)->f1; goto _LL1061;} else{ goto _LL1062;} _LL1062: if(*(( int*)
_temp1024) == Cyc_Absyn_Deref_e){ _LL1161: _temp1160=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1024)->f1; goto _LL1063;} else{ goto _LL1064;} _LL1064: if(*(( int*)
_temp1024) == Cyc_Absyn_StructMember_e){ _LL1165: _temp1164=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1024)->f1; goto _LL1163; _LL1163: _temp1162=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1024)->f2; goto _LL1065;} else{ goto _LL1066;} _LL1066: if(*(( int*)
_temp1024) == Cyc_Absyn_StructArrow_e){ _LL1169: _temp1168=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1024)->f1; goto _LL1167; _LL1167: _temp1166=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1024)->f2; goto _LL1067;} else{ goto _LL1068;} _LL1068: if(*(( int*)
_temp1024) == Cyc_Absyn_Subscript_e){ _LL1173: _temp1172=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1024)->f1; goto _LL1171; _LL1171: _temp1170=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1024)->f2; goto _LL1069;} else{ goto _LL1070;} _LL1070: if(*(( int*)
_temp1024) == Cyc_Absyn_Tuple_e){ _LL1175: _temp1174=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1024)->f1; goto _LL1071;} else{ goto _LL1072;} _LL1072: if(*(( int*)
_temp1024) == Cyc_Absyn_CompoundLit_e){ _LL1179: _temp1178=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1024)->f1; goto _LL1177; _LL1177: _temp1176=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1024)->f2; goto _LL1073;} else{ goto _LL1074;} _LL1074: if(*(( int*)
_temp1024) == Cyc_Absyn_Array_e){ _LL1181: _temp1180=(( struct Cyc_Absyn_Array_e_struct*)
_temp1024)->f1; goto _LL1075;} else{ goto _LL1076;} _LL1076: if(*(( int*)
_temp1024) == Cyc_Absyn_Comprehension_e){ _LL1187: _temp1186=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1024)->f1; goto _LL1185; _LL1185: _temp1184=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1024)->f2; goto _LL1183; _LL1183: _temp1182=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1024)->f3; goto _LL1077;} else{ goto _LL1078;} _LL1078: if(*(( int*)
_temp1024) == Cyc_Absyn_Struct_e){ _LL1195: _temp1194=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1024)->f1; goto _LL1193; _LL1193: _temp1192=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1024)->f2; goto _LL1191; _LL1191: _temp1190=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1024)->f3; goto _LL1189; _LL1189: _temp1188=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1024)->f4; goto _LL1079;} else{ goto _LL1080;} _LL1080: if(*(( int*)
_temp1024) == Cyc_Absyn_AnonStruct_e){ _LL1199: _temp1198=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1024)->f1; goto _LL1197; _LL1197: _temp1196=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1024)->f2; goto _LL1081;} else{ goto _LL1082;} _LL1082: if(*(( int*)
_temp1024) == Cyc_Absyn_Tunion_e){ _LL1209: _temp1208=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1024)->f1; goto _LL1207; _LL1207: _temp1206=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1024)->f2; goto _LL1205; _LL1205: _temp1204=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1024)->f3; goto _LL1203; _LL1203: _temp1202=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1024)->f4; goto _LL1201; _LL1201: _temp1200=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1024)->f5; goto _LL1083;} else{ goto _LL1084;} _LL1084: if(*(( int*)
_temp1024) == Cyc_Absyn_Enum_e){ _LL1215: _temp1214=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1024)->f1; goto _LL1213; _LL1213: _temp1212=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1024)->f2; goto _LL1211; _LL1211: _temp1210=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1024)->f3; goto _LL1085;} else{ goto _LL1086;} _LL1086: if(*(( int*)
_temp1024) == Cyc_Absyn_Malloc_e){ _LL1219: _temp1218=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1024)->f1; goto _LL1217; _LL1217: _temp1216=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1024)->f2; goto _LL1087;} else{ goto _LL1088;} _LL1088: if(*(( int*)
_temp1024) == Cyc_Absyn_UnresolvedMem_e){ _LL1223: _temp1222=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1024)->f1; goto _LL1221; _LL1221: _temp1220=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1024)->f2; goto _LL1089;} else{ goto _LL1090;} _LL1090: if(*(( int*)
_temp1024) == Cyc_Absyn_StmtExp_e){ _LL1225: _temp1224=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1024)->f1; goto _LL1091;} else{ goto _LL1092;} _LL1092: if(*(( int*)
_temp1024) == Cyc_Absyn_Codegen_e){ _LL1227: _temp1226=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1024)->f1; goto _LL1093;} else{ goto _LL1094;} _LL1094: if(*(( int*)
_temp1024) == Cyc_Absyn_Fill_e){ _LL1229: _temp1228=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1024)->f1; goto _LL1095;} else{ goto _LL1025;} _LL1027: s= Cyc_Absynpp_cnst2doc(
_temp1096); goto _LL1025; _LL1029: _temp1102= _temp1100; goto _LL1031; _LL1031:
s= Cyc_Absynpp_qvar2doc( _temp1102); goto _LL1025; _LL1033: s= Cyc_Absynpp_primapp2doc(
myprec, _temp1106, _temp1104); goto _LL1025; _LL1035: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1112), Cyc_PP_text(({ char* _temp1230=" "; struct _tagged_string
_temp1231; _temp1231.curr= _temp1230; _temp1231.base= _temp1230; _temp1231.last_plus_one=
_temp1230 + 2; _temp1231;})), _temp1110 == 0? Cyc_PP_text(({ char* _temp1232="";
struct _tagged_string _temp1233; _temp1233.curr= _temp1232; _temp1233.base=
_temp1232; _temp1233.last_plus_one= _temp1232 + 1; _temp1233;})): Cyc_Absynpp_prim2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1110))->v), Cyc_PP_text(({ char*
_temp1234="= "; struct _tagged_string _temp1235; _temp1235.curr= _temp1234;
_temp1235.base= _temp1234; _temp1235.last_plus_one= _temp1234 + 3; _temp1235;})),
Cyc_Absynpp_exp2doc_prec( myprec, _temp1108)); goto _LL1025; _LL1037: { struct
Cyc_PP_Doc* _temp1236= Cyc_Absynpp_exp2doc_prec( myprec, _temp1116); goto
_LL1237; _LL1237:{ void* _temp1238= _temp1114; _LL1240: if( _temp1238 ==( void*)
Cyc_Absyn_PreInc){ goto _LL1241;} else{ goto _LL1242;} _LL1242: if( _temp1238 ==(
void*) Cyc_Absyn_PreDec){ goto _LL1243;} else{ goto _LL1244;} _LL1244: if(
_temp1238 ==( void*) Cyc_Absyn_PostInc){ goto _LL1245;} else{ goto _LL1246;}
_LL1246: if( _temp1238 ==( void*) Cyc_Absyn_PostDec){ goto _LL1247;} else{ goto
_LL1239;} _LL1241: s= Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp1248="++";
struct _tagged_string _temp1249; _temp1249.curr= _temp1248; _temp1249.base=
_temp1248; _temp1249.last_plus_one= _temp1248 + 3; _temp1249;})), _temp1236);
goto _LL1239; _LL1243: s= Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp1250="--";
struct _tagged_string _temp1251; _temp1251.curr= _temp1250; _temp1251.base=
_temp1250; _temp1251.last_plus_one= _temp1250 + 3; _temp1251;})), _temp1236);
goto _LL1239; _LL1245: s= Cyc_Absynpp_cat2( _temp1236, Cyc_PP_text(({ char*
_temp1252="++"; struct _tagged_string _temp1253; _temp1253.curr= _temp1252;
_temp1253.base= _temp1252; _temp1253.last_plus_one= _temp1252 + 3; _temp1253;})));
goto _LL1239; _LL1247: s= Cyc_Absynpp_cat2( _temp1236, Cyc_PP_text(({ char*
_temp1254="--"; struct _tagged_string _temp1255; _temp1255.curr= _temp1254;
_temp1255.base= _temp1254; _temp1255.last_plus_one= _temp1254 + 3; _temp1255;})));
goto _LL1239; _LL1239:;} goto _LL1025;} _LL1039:{ struct _tuple8 _temp1257=({
struct _tuple8 _temp1256; _temp1256.f1=( void*) _temp1120->r; _temp1256.f2=(
void*) _temp1118->r; _temp1256;}); _LL1259: goto _LL1260; _LL1260: s= Cyc_Absynpp_cat5(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1122), Cyc_PP_text(({ char* _temp1261=" ? ";
struct _tagged_string _temp1262; _temp1262.curr= _temp1261; _temp1262.base=
_temp1261; _temp1262.last_plus_one= _temp1261 + 4; _temp1262;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1120), Cyc_PP_text(({ char* _temp1263=" : "; struct _tagged_string
_temp1264; _temp1264.curr= _temp1263; _temp1264.base= _temp1263; _temp1264.last_plus_one=
_temp1263 + 4; _temp1264;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1118));
goto _LL1258; _LL1258:;} goto _LL1025; _LL1041: s= Cyc_Absynpp_cat5( Cyc_PP_text(({
char* _temp1265="("; struct _tagged_string _temp1266; _temp1266.curr= _temp1265;
_temp1266.base= _temp1265; _temp1266.last_plus_one= _temp1265 + 2; _temp1266;})),
Cyc_Absynpp_exp2doc( _temp1126), Cyc_PP_text(({ char* _temp1267=", "; struct
_tagged_string _temp1268; _temp1268.curr= _temp1267; _temp1268.base= _temp1267;
_temp1268.last_plus_one= _temp1267 + 3; _temp1268;})), Cyc_Absynpp_exp2doc(
_temp1124), Cyc_PP_text(({ char* _temp1269=")"; struct _tagged_string _temp1270;
_temp1270.curr= _temp1269; _temp1270.base= _temp1269; _temp1270.last_plus_one=
_temp1269 + 2; _temp1270;}))); goto _LL1025; _LL1043: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1130), Cyc_PP_text(({ char* _temp1271="("; struct _tagged_string
_temp1272; _temp1272.curr= _temp1271; _temp1272.base= _temp1271; _temp1272.last_plus_one=
_temp1271 + 2; _temp1272;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1128), Cyc_PP_text(({
char* _temp1273=")"; struct _tagged_string _temp1274; _temp1274.curr= _temp1273;
_temp1274.base= _temp1273; _temp1274.last_plus_one= _temp1273 + 2; _temp1274;})));
goto _LL1025; _LL1045: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1136), Cyc_PP_text(({ char* _temp1275="("; struct _tagged_string _temp1276;
_temp1276.curr= _temp1275; _temp1276.base= _temp1275; _temp1276.last_plus_one=
_temp1275 + 2; _temp1276;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1134), Cyc_PP_text(({
char* _temp1277=")"; struct _tagged_string _temp1278; _temp1278.curr= _temp1277;
_temp1278.base= _temp1277; _temp1278.last_plus_one= _temp1277 + 2; _temp1278;})));
goto _LL1025; _LL1047: s= Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp1279="throw ";
struct _tagged_string _temp1280; _temp1280.curr= _temp1279; _temp1280.base=
_temp1279; _temp1280.last_plus_one= _temp1279 + 7; _temp1280;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1138)); goto _LL1025; _LL1049: s= Cyc_Absynpp_exp2doc_prec( inprec,
_temp1140); goto _LL1025; _LL1051: s= Cyc_Absynpp_exp2doc_prec( inprec,
_temp1144); goto _LL1025; _LL1053: s= Cyc_Absynpp_cat4( Cyc_PP_text(({ char*
_temp1281="("; struct _tagged_string _temp1282; _temp1282.curr= _temp1281;
_temp1282.base= _temp1281; _temp1282.last_plus_one= _temp1281 + 2; _temp1282;})),
Cyc_Absynpp_typ2doc( _temp1148), Cyc_PP_text(({ char* _temp1283=")"; struct
_tagged_string _temp1284; _temp1284.curr= _temp1283; _temp1284.base= _temp1283;
_temp1284.last_plus_one= _temp1283 + 2; _temp1284;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1146)); goto _LL1025; _LL1055: s= Cyc_Absynpp_cat2( Cyc_PP_text(({
char* _temp1285="&"; struct _tagged_string _temp1286; _temp1286.curr= _temp1285;
_temp1286.base= _temp1285; _temp1286.last_plus_one= _temp1285 + 2; _temp1286;})),
Cyc_Absynpp_exp2doc_prec( myprec, _temp1150)); goto _LL1025; _LL1057: if(
_temp1154 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp1287="new ";
struct _tagged_string _temp1288; _temp1288.curr= _temp1287; _temp1288.base=
_temp1287; _temp1288.last_plus_one= _temp1287 + 5; _temp1288;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1152));} else{ s= Cyc_Absynpp_cat4( Cyc_PP_text(({ char* _temp1289="rnew(";
struct _tagged_string _temp1290; _temp1290.curr= _temp1289; _temp1290.base=
_temp1289; _temp1290.last_plus_one= _temp1289 + 6; _temp1290;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1154)), Cyc_PP_text(({ char* _temp1291=") ";
struct _tagged_string _temp1292; _temp1292.curr= _temp1291; _temp1292.base=
_temp1291; _temp1292.last_plus_one= _temp1291 + 3; _temp1292;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1152));} goto _LL1025; _LL1059: s= Cyc_Absynpp_cat3( Cyc_PP_text(({
char* _temp1293="sizeof("; struct _tagged_string _temp1294; _temp1294.curr=
_temp1293; _temp1294.base= _temp1293; _temp1294.last_plus_one= _temp1293 + 8;
_temp1294;})), Cyc_Absynpp_typ2doc( _temp1156), Cyc_PP_text(({ char* _temp1295=")";
struct _tagged_string _temp1296; _temp1296.curr= _temp1295; _temp1296.base=
_temp1295; _temp1296.last_plus_one= _temp1295 + 2; _temp1296;}))); goto _LL1025;
_LL1061: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1297="sizeof("; struct
_tagged_string _temp1298; _temp1298.curr= _temp1297; _temp1298.base= _temp1297;
_temp1298.last_plus_one= _temp1297 + 8; _temp1298;})), Cyc_Absynpp_exp2doc(
_temp1158), Cyc_PP_text(({ char* _temp1299=")"; struct _tagged_string _temp1300;
_temp1300.curr= _temp1299; _temp1300.base= _temp1299; _temp1300.last_plus_one=
_temp1299 + 2; _temp1300;}))); goto _LL1025; _LL1063: s= Cyc_Absynpp_cat2( Cyc_PP_text(({
char* _temp1301="*"; struct _tagged_string _temp1302; _temp1302.curr= _temp1301;
_temp1302.base= _temp1301; _temp1302.last_plus_one= _temp1301 + 2; _temp1302;})),
Cyc_Absynpp_exp2doc_prec( myprec, _temp1160)); goto _LL1025; _LL1065: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1164), Cyc_PP_text(({ char* _temp1303=".";
struct _tagged_string _temp1304; _temp1304.curr= _temp1303; _temp1304.base=
_temp1303; _temp1304.last_plus_one= _temp1303 + 2; _temp1304;})), Cyc_Absynpp_textptr(
_temp1162)); goto _LL1025; _LL1067: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1168), Cyc_PP_text(({ char* _temp1305="->"; struct _tagged_string
_temp1306; _temp1306.curr= _temp1305; _temp1306.base= _temp1305; _temp1306.last_plus_one=
_temp1305 + 3; _temp1306;})), Cyc_Absynpp_textptr( _temp1166)); goto _LL1025;
_LL1069: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1172), Cyc_PP_text(({
char* _temp1307="["; struct _tagged_string _temp1308; _temp1308.curr= _temp1307;
_temp1308.base= _temp1307; _temp1308.last_plus_one= _temp1307 + 2; _temp1308;})),
Cyc_Absynpp_exp2doc( _temp1170), Cyc_PP_text(({ char* _temp1309="]"; struct
_tagged_string _temp1310; _temp1310.curr= _temp1309; _temp1310.base= _temp1309;
_temp1310.last_plus_one= _temp1309 + 2; _temp1310;}))); goto _LL1025; _LL1071: s=
Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1311="$("; struct _tagged_string
_temp1312; _temp1312.curr= _temp1311; _temp1312.base= _temp1311; _temp1312.last_plus_one=
_temp1311 + 3; _temp1312;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1174), Cyc_PP_text(({
char* _temp1313=")"; struct _tagged_string _temp1314; _temp1314.curr= _temp1313;
_temp1314.base= _temp1313; _temp1314.last_plus_one= _temp1313 + 2; _temp1314;})));
goto _LL1025; _LL1073: s= Cyc_Absynpp_cat4( Cyc_PP_text(({ char* _temp1315="(";
struct _tagged_string _temp1316; _temp1316.curr= _temp1315; _temp1316.base=
_temp1315; _temp1316.last_plus_one= _temp1315 + 2; _temp1316;})), Cyc_Absynpp_typ2doc((*
_temp1178).f3), Cyc_PP_text(({ char* _temp1317=")"; struct _tagged_string
_temp1318; _temp1318.curr= _temp1317; _temp1318.base= _temp1317; _temp1318.last_plus_one=
_temp1317 + 2; _temp1318;})), Cyc_Absynpp_group(({ char* _temp1319="{"; struct
_tagged_string _temp1320; _temp1320.curr= _temp1319; _temp1320.base= _temp1319;
_temp1320.last_plus_one= _temp1319 + 2; _temp1320;}),({ char* _temp1321="}";
struct _tagged_string _temp1322; _temp1322.curr= _temp1321; _temp1322.base=
_temp1321; _temp1322.last_plus_one= _temp1321 + 2; _temp1322;}),({ char*
_temp1323=","; struct _tagged_string _temp1324; _temp1324.curr= _temp1323;
_temp1324.base= _temp1323; _temp1324.last_plus_one= _temp1323 + 2; _temp1324;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1176))); goto _LL1025; _LL1075: s=
Cyc_Absynpp_group(({ char* _temp1325="{"; struct _tagged_string _temp1326;
_temp1326.curr= _temp1325; _temp1326.base= _temp1325; _temp1326.last_plus_one=
_temp1325 + 2; _temp1326;}),({ char* _temp1327="}"; struct _tagged_string
_temp1328; _temp1328.curr= _temp1327; _temp1328.base= _temp1327; _temp1328.last_plus_one=
_temp1327 + 2; _temp1328;}),({ char* _temp1329=","; struct _tagged_string
_temp1330; _temp1330.curr= _temp1329; _temp1330.base= _temp1329; _temp1330.last_plus_one=
_temp1329 + 2; _temp1330;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1180)); goto _LL1025; _LL1077: s= Cyc_Absynpp_cat7( Cyc_PP_text(({ char*
_temp1331="{for "; struct _tagged_string _temp1332; _temp1332.curr= _temp1331;
_temp1332.base= _temp1331; _temp1332.last_plus_one= _temp1331 + 6; _temp1332;})),
Cyc_PP_text(*(* _temp1186->name).f2), Cyc_PP_text(({ char* _temp1333=" < ";
struct _tagged_string _temp1334; _temp1334.curr= _temp1333; _temp1334.base=
_temp1333; _temp1334.last_plus_one= _temp1333 + 4; _temp1334;})), Cyc_Absynpp_exp2doc(
_temp1184), Cyc_PP_text(({ char* _temp1335=" : "; struct _tagged_string
_temp1336; _temp1336.curr= _temp1335; _temp1336.base= _temp1335; _temp1336.last_plus_one=
_temp1335 + 4; _temp1336;})), Cyc_Absynpp_exp2doc( _temp1182), Cyc_PP_text(({
char* _temp1337="}"; struct _tagged_string _temp1338; _temp1338.curr= _temp1337;
_temp1338.base= _temp1337; _temp1338.last_plus_one= _temp1337 + 2; _temp1338;})));
goto _LL1025; _LL1079: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1194),
Cyc_Absynpp_group(({ char* _temp1339="{"; struct _tagged_string _temp1340;
_temp1340.curr= _temp1339; _temp1340.base= _temp1339; _temp1340.last_plus_one=
_temp1339 + 2; _temp1340;}),({ char* _temp1341="}"; struct _tagged_string
_temp1342; _temp1342.curr= _temp1341; _temp1342.base= _temp1341; _temp1342.last_plus_one=
_temp1341 + 2; _temp1342;}),({ char* _temp1343=","; struct _tagged_string
_temp1344; _temp1344.curr= _temp1343; _temp1344.base= _temp1343; _temp1344.last_plus_one=
_temp1343 + 2; _temp1344;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1190))); goto _LL1025; _LL1081: s= Cyc_Absynpp_group(({ char* _temp1345="{";
struct _tagged_string _temp1346; _temp1346.curr= _temp1345; _temp1346.base=
_temp1345; _temp1346.last_plus_one= _temp1345 + 2; _temp1346;}),({ char*
_temp1347="}"; struct _tagged_string _temp1348; _temp1348.curr= _temp1347;
_temp1348.base= _temp1347; _temp1348.last_plus_one= _temp1347 + 2; _temp1348;}),({
char* _temp1349=","; struct _tagged_string _temp1350; _temp1350.curr= _temp1349;
_temp1350.base= _temp1349; _temp1350.last_plus_one= _temp1349 + 2; _temp1350;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1196)); goto _LL1025; _LL1083: if(
_temp1204 == 0){ s= Cyc_Absynpp_qvar2doc( _temp1200->name);} else{ s= Cyc_Absynpp_cat2(
Cyc_Absynpp_qvar2doc( _temp1200->name), Cyc_Absynpp_egroup(({ char* _temp1351="(";
struct _tagged_string _temp1352; _temp1352.curr= _temp1351; _temp1352.base=
_temp1351; _temp1352.last_plus_one= _temp1351 + 2; _temp1352;}),({ char*
_temp1353=")"; struct _tagged_string _temp1354; _temp1354.curr= _temp1353;
_temp1354.base= _temp1353; _temp1354.last_plus_one= _temp1353 + 2; _temp1354;}),({
char* _temp1355=","; struct _tagged_string _temp1356; _temp1356.curr= _temp1355;
_temp1356.base= _temp1355; _temp1356.last_plus_one= _temp1355 + 2; _temp1356;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_exp2doc, _temp1204)));}
goto _LL1025; _LL1085: s= Cyc_Absynpp_qvar2doc( _temp1214); goto _LL1025;
_LL1087: if( _temp1218 == 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char*
_temp1357="malloc(sizeof("; struct _tagged_string _temp1358; _temp1358.curr=
_temp1357; _temp1358.base= _temp1357; _temp1358.last_plus_one= _temp1357 + 15;
_temp1358;})), Cyc_Absynpp_typ2doc( _temp1216), Cyc_PP_text(({ char* _temp1359="))";
struct _tagged_string _temp1360; _temp1360.curr= _temp1359; _temp1360.base=
_temp1359; _temp1360.last_plus_one= _temp1359 + 3; _temp1360;})));} else{ s= Cyc_Absynpp_cat6(
Cyc_PP_text(({ char* _temp1361="rmalloc("; struct _tagged_string _temp1362;
_temp1362.curr= _temp1361; _temp1362.base= _temp1361; _temp1362.last_plus_one=
_temp1361 + 9; _temp1362;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( _temp1218)), Cyc_PP_text(({ char* _temp1363=","; struct
_tagged_string _temp1364; _temp1364.curr= _temp1363; _temp1364.base= _temp1363;
_temp1364.last_plus_one= _temp1363 + 2; _temp1364;})), Cyc_PP_text(({ char*
_temp1365="sizeof("; struct _tagged_string _temp1366; _temp1366.curr= _temp1365;
_temp1366.base= _temp1365; _temp1366.last_plus_one= _temp1365 + 8; _temp1366;})),
Cyc_Absynpp_typ2doc( _temp1216), Cyc_PP_text(({ char* _temp1367="))"; struct
_tagged_string _temp1368; _temp1368.curr= _temp1367; _temp1368.base= _temp1367;
_temp1368.last_plus_one= _temp1367 + 3; _temp1368;})));} goto _LL1025; _LL1089:
s= Cyc_Absynpp_group(({ char* _temp1369="{"; struct _tagged_string _temp1370;
_temp1370.curr= _temp1369; _temp1370.base= _temp1369; _temp1370.last_plus_one=
_temp1369 + 2; _temp1370;}),({ char* _temp1371="}"; struct _tagged_string
_temp1372; _temp1372.curr= _temp1371; _temp1372.base= _temp1371; _temp1372.last_plus_one=
_temp1371 + 2; _temp1372;}),({ char* _temp1373=","; struct _tagged_string
_temp1374; _temp1374.curr= _temp1373; _temp1374.base= _temp1373; _temp1374.last_plus_one=
_temp1373 + 2; _temp1374;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1220)); goto _LL1025; _LL1091: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char*
_temp1375="({ "; struct _tagged_string _temp1376; _temp1376.curr= _temp1375;
_temp1376.base= _temp1375; _temp1376.last_plus_one= _temp1375 + 4; _temp1376;})),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1224)), Cyc_PP_text(({ char*
_temp1377=" })"; struct _tagged_string _temp1378; _temp1378.curr= _temp1377;
_temp1378.base= _temp1377; _temp1378.last_plus_one= _temp1377 + 4; _temp1378;})));
goto _LL1025; _LL1093: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1379="codegen(";
struct _tagged_string _temp1380; _temp1380.curr= _temp1379; _temp1380.base=
_temp1379; _temp1380.last_plus_one= _temp1379 + 9; _temp1380;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1381=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1381->r=( void*)(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp1382=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1382[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1383; _temp1383.tag= Cyc_Absyn_Fn_d; _temp1383.f1= _temp1226; _temp1383;});
_temp1382;})); _temp1381->loc= e->loc; _temp1381;}))), Cyc_PP_text(({ char*
_temp1384=")"; struct _tagged_string _temp1385; _temp1385.curr= _temp1384;
_temp1385.base= _temp1384; _temp1385.last_plus_one= _temp1384 + 2; _temp1385;})));
goto _LL1025; _LL1095: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1386="fill(";
struct _tagged_string _temp1387; _temp1387.curr= _temp1386; _temp1387.base=
_temp1386; _temp1387.last_plus_one= _temp1386 + 6; _temp1387;})), Cyc_PP_nest( 2,
Cyc_Absynpp_exp2doc( _temp1228)), Cyc_PP_text(({ char* _temp1388=")"; struct
_tagged_string _temp1389; _temp1389.curr= _temp1388; _temp1389.base= _temp1388;
_temp1389.last_plus_one= _temp1388 + 2; _temp1389;}))); goto _LL1025; _LL1025:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1390="(";
struct _tagged_string _temp1391; _temp1391.curr= _temp1390; _temp1391.base=
_temp1390; _temp1391.last_plus_one= _temp1390 + 2; _temp1391;})), s, Cyc_PP_text(({
char* _temp1392=")"; struct _tagged_string _temp1393; _temp1393.curr= _temp1392;
_temp1393.base= _temp1392; _temp1393.last_plus_one= _temp1392 + 2; _temp1393;})));}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void*
_temp1394= d; struct Cyc_Absyn_Exp* _temp1400; struct _tagged_string* _temp1402;
_LL1396: if(*(( int*) _temp1394) == Cyc_Absyn_ArrayElement){ _LL1401: _temp1400=((
struct Cyc_Absyn_ArrayElement_struct*) _temp1394)->f1; goto _LL1397;} else{ goto
_LL1398;} _LL1398: if(*(( int*) _temp1394) == Cyc_Absyn_FieldName){ _LL1403:
_temp1402=(( struct Cyc_Absyn_FieldName_struct*) _temp1394)->f1; goto _LL1399;}
else{ goto _LL1395;} _LL1397: return Cyc_Absynpp_cat3( Cyc_PP_text(({ char*
_temp1404=".["; struct _tagged_string _temp1405; _temp1405.curr= _temp1404;
_temp1405.base= _temp1404; _temp1405.last_plus_one= _temp1404 + 3; _temp1405;})),
Cyc_Absynpp_exp2doc( _temp1400), Cyc_PP_text(({ char* _temp1406="]"; struct
_tagged_string _temp1407; _temp1407.curr= _temp1406; _temp1407.base= _temp1406;
_temp1407.last_plus_one= _temp1406 + 2; _temp1407;}))); _LL1399: return Cyc_Absynpp_cat2(
Cyc_PP_text(({ char* _temp1408="."; struct _tagged_string _temp1409; _temp1409.curr=
_temp1408; _temp1409.base= _temp1408; _temp1409.last_plus_one= _temp1408 + 2;
_temp1409;})), Cyc_Absynpp_textptr( _temp1402)); _LL1395:;} struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((*
de).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup(({ char* _temp1410="";
struct _tagged_string _temp1411; _temp1411.curr= _temp1410; _temp1411.base=
_temp1410; _temp1411.last_plus_one= _temp1410 + 1; _temp1411;}),({ char*
_temp1412="="; struct _tagged_string _temp1413; _temp1413.curr= _temp1412;
_temp1413.base= _temp1412; _temp1413.last_plus_one= _temp1412 + 2; _temp1413;}),({
char* _temp1414="="; struct _tagged_string _temp1415; _temp1415.curr= _temp1414;
_temp1415.base= _temp1414; _temp1415.last_plus_one= _temp1414 + 2; _temp1415;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* de).f1)), Cyc_Absynpp_exp2doc((*
de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_Absynpp_group(({ char* _temp1416=""; struct _tagged_string
_temp1417; _temp1417.curr= _temp1416; _temp1417.base= _temp1416; _temp1417.last_plus_one=
_temp1416 + 1; _temp1417;}),({ char* _temp1418=""; struct _tagged_string
_temp1419; _temp1419.curr= _temp1418; _temp1419.base= _temp1418; _temp1419.last_plus_one=
_temp1418 + 1; _temp1419;}),({ char* _temp1420=","; struct _tagged_string
_temp1421; _temp1421.curr= _temp1420; _temp1421.base= _temp1420; _temp1421.last_plus_one=
_temp1420 + 2; _temp1421;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1422= c; char _temp1440; void* _temp1442; short _temp1444;
void* _temp1446; int _temp1448; void* _temp1450; int _temp1452; void* _temp1454;
long long _temp1456; void* _temp1458; struct _tagged_string _temp1460; struct
_tagged_string _temp1462; _LL1424: if(( unsigned int) _temp1422 > 1u?*(( int*)
_temp1422) == Cyc_Absyn_Char_c: 0){ _LL1443: _temp1442=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp1422)->f1; goto _LL1441; _LL1441: _temp1440=(( struct Cyc_Absyn_Char_c_struct*)
_temp1422)->f2; goto _LL1425;} else{ goto _LL1426;} _LL1426: if(( unsigned int)
_temp1422 > 1u?*(( int*) _temp1422) == Cyc_Absyn_Short_c: 0){ _LL1447: _temp1446=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1422)->f1; goto _LL1445; _LL1445:
_temp1444=(( struct Cyc_Absyn_Short_c_struct*) _temp1422)->f2; goto _LL1427;}
else{ goto _LL1428;} _LL1428: if(( unsigned int) _temp1422 > 1u?*(( int*)
_temp1422) == Cyc_Absyn_Int_c: 0){ _LL1451: _temp1450=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp1422)->f1; if( _temp1450 ==( void*) Cyc_Absyn_Signed){ goto _LL1449;} else{
goto _LL1430;} _LL1449: _temp1448=(( struct Cyc_Absyn_Int_c_struct*) _temp1422)->f2;
goto _LL1429;} else{ goto _LL1430;} _LL1430: if(( unsigned int) _temp1422 > 1u?*((
int*) _temp1422) == Cyc_Absyn_Int_c: 0){ _LL1455: _temp1454=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1422)->f1; if( _temp1454 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1453;} else{ goto _LL1432;} _LL1453: _temp1452=(( struct Cyc_Absyn_Int_c_struct*)
_temp1422)->f2; goto _LL1431;} else{ goto _LL1432;} _LL1432: if(( unsigned int)
_temp1422 > 1u?*(( int*) _temp1422) == Cyc_Absyn_LongLong_c: 0){ _LL1459:
_temp1458=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1422)->f1; goto
_LL1457; _LL1457: _temp1456=(( struct Cyc_Absyn_LongLong_c_struct*) _temp1422)->f2;
goto _LL1433;} else{ goto _LL1434;} _LL1434: if(( unsigned int) _temp1422 > 1u?*((
int*) _temp1422) == Cyc_Absyn_Float_c: 0){ _LL1461: _temp1460=(( struct Cyc_Absyn_Float_c_struct*)
_temp1422)->f1; goto _LL1435;} else{ goto _LL1436;} _LL1436: if( _temp1422 ==(
void*) Cyc_Absyn_Null_c){ goto _LL1437;} else{ goto _LL1438;} _LL1438: if((
unsigned int) _temp1422 > 1u?*(( int*) _temp1422) == Cyc_Absyn_String_c: 0){
_LL1463: _temp1462=(( struct Cyc_Absyn_String_c_struct*) _temp1422)->f1; goto
_LL1439;} else{ goto _LL1423;} _LL1425: return Cyc_PP_text(({ struct
_tagged_string _temp1464= Cyc_Absynpp_char_escape( _temp1440); xprintf("'%.*s'",
_temp1464.last_plus_one - _temp1464.curr, _temp1464.curr);})); _LL1427: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1444)); _LL1429: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1448)); _LL1431: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1452)); _LL1433: return Cyc_PP_text(({ char* _temp1465="<<FIX LONG LONG CONSTANT>>";
struct _tagged_string _temp1466; _temp1466.curr= _temp1465; _temp1466.base=
_temp1465; _temp1466.last_plus_one= _temp1465 + 27; _temp1466;})); _LL1435:
return Cyc_PP_text( _temp1460); _LL1437: return Cyc_PP_text(({ char* _temp1467="null";
struct _tagged_string _temp1468; _temp1468.curr= _temp1467; _temp1468.base=
_temp1467; _temp1468.last_plus_one= _temp1467 + 5; _temp1468;})); _LL1439:
return Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1469="\""; struct
_tagged_string _temp1470; _temp1470.curr= _temp1469; _temp1470.base= _temp1469;
_temp1470.last_plus_one= _temp1469 + 2; _temp1470;})), Cyc_PP_text( Cyc_Absynpp_string_escape(
_temp1462)), Cyc_PP_text(({ char* _temp1471="\""; struct _tagged_string
_temp1472; _temp1472.curr= _temp1471; _temp1472.base= _temp1471; _temp1472.last_plus_one=
_temp1471 + 2; _temp1472;}))); _LL1423:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl != 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1473=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1473[ 0]=({ struct Cyc_Core_Failure_struct _temp1474; _temp1474.tag= Cyc_Core_Failure;
_temp1474.f1=({ struct _tagged_string _temp1475= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1475.last_plus_one
- _temp1475.curr, _temp1475.curr);}); _temp1474;}); _temp1473;}));}{ struct Cyc_PP_Doc*
_temp1476= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd); goto _LL1477; _LL1477: return Cyc_Absynpp_cat2(
_temp1476, Cyc_PP_text(({ char* _temp1478=".size"; struct _tagged_string
_temp1479; _temp1479.curr= _temp1478; _temp1479.base= _temp1478; _temp1479.last_plus_one=
_temp1478 + 6; _temp1479;})));}} else{ if( ! Cyc_Absyn_is_format_prim( p)){
struct Cyc_List_List* ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es); if( ds == 0){( void) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp1480=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1480[ 0]=({ struct Cyc_Core_Failure_struct
_temp1481; _temp1481.tag= Cyc_Core_Failure; _temp1481.f1=({ struct
_tagged_string _temp1482= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with no args",
_temp1482.last_plus_one - _temp1482.curr, _temp1482.curr);}); _temp1481;});
_temp1480;}));} else{ if((( struct Cyc_List_List*) _check_null( ds))->tl == 0){
return Cyc_Absynpp_cat3( ps, Cyc_PP_text(({ char* _temp1483=" "; struct
_tagged_string _temp1484; _temp1484.curr= _temp1483; _temp1484.base= _temp1483;
_temp1484.last_plus_one= _temp1483 + 2; _temp1484;})),( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( ds))->hd);} else{ if((( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( ds))->tl))->tl != 0){( void)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp1485=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1485[ 0]=({ struct Cyc_Core_Failure_struct
_temp1486; _temp1486.tag= Cyc_Core_Failure; _temp1486.f1=({ struct
_tagged_string _temp1487= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_temp1487.last_plus_one - _temp1487.curr, _temp1487.curr);}); _temp1486;});
_temp1485;}));} else{ return Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( ds))->hd, Cyc_PP_text(({ char* _temp1488=" "; struct _tagged_string
_temp1489; _temp1489.curr= _temp1488; _temp1489.base= _temp1488; _temp1489.last_plus_one=
_temp1488 + 2; _temp1489;})), ps, Cyc_PP_text(({ char* _temp1490=" "; struct
_tagged_string _temp1491; _temp1491.curr= _temp1490; _temp1491.base= _temp1490;
_temp1491.last_plus_one= _temp1490 + 2; _temp1491;})),( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( ds))->tl))->hd);}}}}
else{ return Cyc_Absynpp_cat4( ps, Cyc_PP_text(({ char* _temp1492="("; struct
_tagged_string _temp1493; _temp1493.curr= _temp1492; _temp1493.base= _temp1492;
_temp1493.last_plus_one= _temp1492 + 2; _temp1493;})), Cyc_Absynpp_exps2doc_prec(
20, es), Cyc_PP_text(({ char* _temp1494=")"; struct _tagged_string _temp1495;
_temp1495.curr= _temp1494; _temp1495.base= _temp1494; _temp1495.last_plus_one=
_temp1494 + 2; _temp1495;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1496= p; _LL1498: if( _temp1496 ==( void*) Cyc_Absyn_Plus){
goto _LL1499;} else{ goto _LL1500;} _LL1500: if( _temp1496 ==( void*) Cyc_Absyn_Times){
goto _LL1501;} else{ goto _LL1502;} _LL1502: if( _temp1496 ==( void*) Cyc_Absyn_Minus){
goto _LL1503;} else{ goto _LL1504;} _LL1504: if( _temp1496 ==( void*) Cyc_Absyn_Div){
goto _LL1505;} else{ goto _LL1506;} _LL1506: if( _temp1496 ==( void*) Cyc_Absyn_Mod){
goto _LL1507;} else{ goto _LL1508;} _LL1508: if( _temp1496 ==( void*) Cyc_Absyn_Eq){
goto _LL1509;} else{ goto _LL1510;} _LL1510: if( _temp1496 ==( void*) Cyc_Absyn_Neq){
goto _LL1511;} else{ goto _LL1512;} _LL1512: if( _temp1496 ==( void*) Cyc_Absyn_Gt){
goto _LL1513;} else{ goto _LL1514;} _LL1514: if( _temp1496 ==( void*) Cyc_Absyn_Lt){
goto _LL1515;} else{ goto _LL1516;} _LL1516: if( _temp1496 ==( void*) Cyc_Absyn_Gte){
goto _LL1517;} else{ goto _LL1518;} _LL1518: if( _temp1496 ==( void*) Cyc_Absyn_Lte){
goto _LL1519;} else{ goto _LL1520;} _LL1520: if( _temp1496 ==( void*) Cyc_Absyn_Not){
goto _LL1521;} else{ goto _LL1522;} _LL1522: if( _temp1496 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1523;} else{ goto _LL1524;} _LL1524: if( _temp1496 ==( void*) Cyc_Absyn_Bitand){
goto _LL1525;} else{ goto _LL1526;} _LL1526: if( _temp1496 ==( void*) Cyc_Absyn_Bitor){
goto _LL1527;} else{ goto _LL1528;} _LL1528: if( _temp1496 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1529;} else{ goto _LL1530;} _LL1530: if( _temp1496 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1531;} else{ goto _LL1532;} _LL1532: if( _temp1496 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1533;} else{ goto _LL1534;} _LL1534: if( _temp1496 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1535;} else{ goto _LL1536;} _LL1536: if( _temp1496 ==( void*) Cyc_Absyn_Size){
goto _LL1537;} else{ goto _LL1538;} _LL1538: if( _temp1496 ==( void*) Cyc_Absyn_Printf){
goto _LL1539;} else{ goto _LL1540;} _LL1540: if( _temp1496 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1541;} else{ goto _LL1542;} _LL1542: if( _temp1496 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1543;} else{ goto _LL1544;} _LL1544: if( _temp1496 ==( void*) Cyc_Absyn_Scanf){
goto _LL1545;} else{ goto _LL1546;} _LL1546: if( _temp1496 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1547;} else{ goto _LL1548;} _LL1548: if( _temp1496 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1549;} else{ goto _LL1497;} _LL1499: return({ char* _temp1550="+";
struct _tagged_string _temp1551; _temp1551.curr= _temp1550; _temp1551.base=
_temp1550; _temp1551.last_plus_one= _temp1550 + 2; _temp1551;}); _LL1501: return({
char* _temp1552="*"; struct _tagged_string _temp1553; _temp1553.curr= _temp1552;
_temp1553.base= _temp1552; _temp1553.last_plus_one= _temp1552 + 2; _temp1553;});
_LL1503: return({ char* _temp1554="-"; struct _tagged_string _temp1555;
_temp1555.curr= _temp1554; _temp1555.base= _temp1554; _temp1555.last_plus_one=
_temp1554 + 2; _temp1555;}); _LL1505: return({ char* _temp1556="/"; struct
_tagged_string _temp1557; _temp1557.curr= _temp1556; _temp1557.base= _temp1556;
_temp1557.last_plus_one= _temp1556 + 2; _temp1557;}); _LL1507: return({ char*
_temp1558="%"; struct _tagged_string _temp1559; _temp1559.curr= _temp1558;
_temp1559.base= _temp1558; _temp1559.last_plus_one= _temp1558 + 2; _temp1559;});
_LL1509: return({ char* _temp1560="=="; struct _tagged_string _temp1561;
_temp1561.curr= _temp1560; _temp1561.base= _temp1560; _temp1561.last_plus_one=
_temp1560 + 3; _temp1561;}); _LL1511: return({ char* _temp1562="!="; struct
_tagged_string _temp1563; _temp1563.curr= _temp1562; _temp1563.base= _temp1562;
_temp1563.last_plus_one= _temp1562 + 3; _temp1563;}); _LL1513: return({ char*
_temp1564=">"; struct _tagged_string _temp1565; _temp1565.curr= _temp1564;
_temp1565.base= _temp1564; _temp1565.last_plus_one= _temp1564 + 2; _temp1565;});
_LL1515: return({ char* _temp1566="<"; struct _tagged_string _temp1567;
_temp1567.curr= _temp1566; _temp1567.base= _temp1566; _temp1567.last_plus_one=
_temp1566 + 2; _temp1567;}); _LL1517: return({ char* _temp1568=">="; struct
_tagged_string _temp1569; _temp1569.curr= _temp1568; _temp1569.base= _temp1568;
_temp1569.last_plus_one= _temp1568 + 3; _temp1569;}); _LL1519: return({ char*
_temp1570="<="; struct _tagged_string _temp1571; _temp1571.curr= _temp1570;
_temp1571.base= _temp1570; _temp1571.last_plus_one= _temp1570 + 3; _temp1571;});
_LL1521: return({ char* _temp1572="!"; struct _tagged_string _temp1573;
_temp1573.curr= _temp1572; _temp1573.base= _temp1572; _temp1573.last_plus_one=
_temp1572 + 2; _temp1573;}); _LL1523: return({ char* _temp1574="~"; struct
_tagged_string _temp1575; _temp1575.curr= _temp1574; _temp1575.base= _temp1574;
_temp1575.last_plus_one= _temp1574 + 2; _temp1575;}); _LL1525: return({ char*
_temp1576="&"; struct _tagged_string _temp1577; _temp1577.curr= _temp1576;
_temp1577.base= _temp1576; _temp1577.last_plus_one= _temp1576 + 2; _temp1577;});
_LL1527: return({ char* _temp1578="|"; struct _tagged_string _temp1579;
_temp1579.curr= _temp1578; _temp1579.base= _temp1578; _temp1579.last_plus_one=
_temp1578 + 2; _temp1579;}); _LL1529: return({ char* _temp1580="^"; struct
_tagged_string _temp1581; _temp1581.curr= _temp1580; _temp1581.base= _temp1580;
_temp1581.last_plus_one= _temp1580 + 2; _temp1581;}); _LL1531: return({ char*
_temp1582="<<"; struct _tagged_string _temp1583; _temp1583.curr= _temp1582;
_temp1583.base= _temp1582; _temp1583.last_plus_one= _temp1582 + 3; _temp1583;});
_LL1533: return({ char* _temp1584=">>"; struct _tagged_string _temp1585;
_temp1585.curr= _temp1584; _temp1585.base= _temp1584; _temp1585.last_plus_one=
_temp1584 + 3; _temp1585;}); _LL1535: return({ char* _temp1586=">>>"; struct
_tagged_string _temp1587; _temp1587.curr= _temp1586; _temp1587.base= _temp1586;
_temp1587.last_plus_one= _temp1586 + 4; _temp1587;}); _LL1537: return({ char*
_temp1588="size"; struct _tagged_string _temp1589; _temp1589.curr= _temp1588;
_temp1589.base= _temp1588; _temp1589.last_plus_one= _temp1588 + 5; _temp1589;});
_LL1539: return({ char* _temp1590="printf"; struct _tagged_string _temp1591;
_temp1591.curr= _temp1590; _temp1591.base= _temp1590; _temp1591.last_plus_one=
_temp1590 + 7; _temp1591;}); _LL1541: return({ char* _temp1592="fprintf"; struct
_tagged_string _temp1593; _temp1593.curr= _temp1592; _temp1593.base= _temp1592;
_temp1593.last_plus_one= _temp1592 + 8; _temp1593;}); _LL1543: return({ char*
_temp1594="xprintf"; struct _tagged_string _temp1595; _temp1595.curr= _temp1594;
_temp1595.base= _temp1594; _temp1595.last_plus_one= _temp1594 + 8; _temp1595;});
_LL1545: return({ char* _temp1596="scanf"; struct _tagged_string _temp1597;
_temp1597.curr= _temp1596; _temp1597.base= _temp1596; _temp1597.last_plus_one=
_temp1596 + 6; _temp1597;}); _LL1547: return({ char* _temp1598="fscanf"; struct
_tagged_string _temp1599; _temp1599.curr= _temp1598; _temp1599.base= _temp1598;
_temp1599.last_plus_one= _temp1598 + 7; _temp1599;}); _LL1549: return({ char*
_temp1600="sscanf"; struct _tagged_string _temp1601; _temp1601.curr= _temp1600;
_temp1601.base= _temp1600; _temp1601.last_plus_one= _temp1600 + 7; _temp1601;});
_LL1497:;} struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text(
Cyc_Absynpp_prim2str( p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp*
f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((*
t).f1 == 0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3(
Cyc_Absynpp_textptr(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null((* t).f1))->v), Cyc_PP_text(({ char* _temp1602=" = "; struct
_tagged_string _temp1603; _temp1603.curr= _temp1602; _temp1603.base= _temp1602;
_temp1603.last_plus_one= _temp1602 + 4; _temp1603;})), Cyc_Absynpp_exp2doc((* t).f2));}}
struct Cyc_PP_Doc* Cyc_Absynpp_cat2( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2){ return Cyc_PP_concat( d1, d2);} struct Cyc_PP_Doc* Cyc_Absynpp_cat3( struct
Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3){ return Cyc_PP_concat(
d1, Cyc_PP_concat( d2, d3));} struct Cyc_PP_Doc* Cyc_Absynpp_cat4( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4){ return
Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat( d3, d4)));} struct Cyc_PP_Doc*
Cyc_Absynpp_cat5( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc*
d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc* d5){ return Cyc_PP_concat( d1, Cyc_PP_concat(
d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, d5))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat6(
struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc*
d4, struct Cyc_PP_Doc* d5, struct Cyc_PP_Doc* d6){ return Cyc_PP_concat( d1, Cyc_PP_concat(
d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, d6)))));} struct
Cyc_PP_Doc* Cyc_Absynpp_cat7( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2,
struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc* d5, struct Cyc_PP_Doc*
d6, struct Cyc_PP_Doc* d7){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat(
d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6, d7))))));} struct
Cyc_PP_Doc* Cyc_Absynpp_cat8( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2,
struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc* d5, struct Cyc_PP_Doc*
d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc* d8){ return Cyc_PP_concat( d1, Cyc_PP_concat(
d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6,
Cyc_PP_concat( d7, d8)))))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat9( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5, struct Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc*
d8, struct Cyc_PP_Doc* d9){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat(
d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6, Cyc_PP_concat( d7,
Cyc_PP_concat( d8, d9))))))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat10( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5, struct Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc*
d8, struct Cyc_PP_Doc* d9, struct Cyc_PP_Doc* d10){ return Cyc_PP_concat( d1,
Cyc_PP_concat( d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat(
d6, Cyc_PP_concat( d7, Cyc_PP_concat( d8, Cyc_PP_concat( d9, d10)))))))));}
struct Cyc_PP_Doc* Cyc_Absynpp_cat11( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc* d5, struct
Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc* d8, struct Cyc_PP_Doc*
d9, struct Cyc_PP_Doc* d10, struct Cyc_PP_Doc* d11){ return Cyc_PP_concat( d1,
Cyc_PP_concat( d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat(
d6, Cyc_PP_concat( d7, Cyc_PP_concat( d8, Cyc_PP_concat( d9, Cyc_PP_concat( d10,
d11))))))))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat12( struct Cyc_PP_Doc* d1,
struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc*
d5, struct Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc* d8, struct
Cyc_PP_Doc* d9, struct Cyc_PP_Doc* d10, struct Cyc_PP_Doc* d11, struct Cyc_PP_Doc*
d12){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat( d3, Cyc_PP_concat(
d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6, Cyc_PP_concat( d7, Cyc_PP_concat( d8,
Cyc_PP_concat( d9, Cyc_PP_concat( d10, Cyc_PP_concat( d11, d12)))))))))));} int
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1604=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1610; struct Cyc_Absyn_Decl* _temp1612;
_LL1606: if(( unsigned int) _temp1604 > 1u?*(( int*) _temp1604) == Cyc_Absyn_Decl_s:
0){ _LL1613: _temp1612=(( struct Cyc_Absyn_Decl_s_struct*) _temp1604)->f1; goto
_LL1611; _LL1611: _temp1610=(( struct Cyc_Absyn_Decl_s_struct*) _temp1604)->f2;
goto _LL1607;} else{ goto _LL1608;} _LL1608: goto _LL1609; _LL1607: return 1;
_LL1609: return 0; _LL1605:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1614=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1656; struct Cyc_Absyn_Stmt* _temp1658; struct Cyc_Absyn_Stmt*
_temp1660; struct Cyc_Absyn_Exp* _temp1662; struct Cyc_Absyn_Stmt* _temp1664;
struct Cyc_Absyn_Stmt* _temp1666; struct Cyc_Absyn_Exp* _temp1668; struct Cyc_Absyn_Stmt*
_temp1670; struct _tuple2 _temp1672; struct Cyc_Absyn_Stmt* _temp1674; struct
Cyc_Absyn_Exp* _temp1676; struct Cyc_Absyn_Stmt* _temp1678; struct Cyc_Absyn_Stmt*
_temp1680; struct Cyc_Absyn_Stmt* _temp1682; struct _tagged_string* _temp1684;
struct Cyc_Absyn_Stmt* _temp1686; struct _tuple2 _temp1688; struct Cyc_Absyn_Stmt*
_temp1690; struct Cyc_Absyn_Exp* _temp1692; struct _tuple2 _temp1694; struct Cyc_Absyn_Stmt*
_temp1696; struct Cyc_Absyn_Exp* _temp1698; struct Cyc_Absyn_Exp* _temp1700;
struct Cyc_List_List* _temp1702; struct Cyc_Absyn_Exp* _temp1704; struct Cyc_Absyn_Switch_clause**
_temp1706; struct Cyc_List_List* _temp1708; struct Cyc_Absyn_Switch_clause**
_temp1710; struct Cyc_List_List* _temp1712; struct Cyc_Absyn_Stmt* _temp1714;
struct Cyc_Absyn_Decl* _temp1716; struct Cyc_Absyn_Stmt* _temp1718; struct Cyc_Absyn_Stmt*
_temp1720; struct Cyc_Absyn_Stmt* _temp1722; struct _tagged_string* _temp1724;
struct _tuple2 _temp1726; struct Cyc_Absyn_Stmt* _temp1728; struct Cyc_Absyn_Exp*
_temp1730; struct Cyc_Absyn_Stmt* _temp1732; struct Cyc_List_List* _temp1734;
struct Cyc_Absyn_Stmt* _temp1736; struct Cyc_Absyn_Stmt* _temp1738; struct Cyc_Absyn_Vardecl*
_temp1740; struct Cyc_Absyn_Tvar* _temp1742; _LL1616: if( _temp1614 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1617;} else{ goto _LL1618;} _LL1618: if((
unsigned int) _temp1614 > 1u?*(( int*) _temp1614) == Cyc_Absyn_Exp_s: 0){
_LL1657: _temp1656=(( struct Cyc_Absyn_Exp_s_struct*) _temp1614)->f1; goto
_LL1619;} else{ goto _LL1620;} _LL1620: if(( unsigned int) _temp1614 > 1u?*((
int*) _temp1614) == Cyc_Absyn_Seq_s: 0){ _LL1661: _temp1660=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1614)->f1; goto _LL1659; _LL1659: _temp1658=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1614)->f2; goto _LL1621;} else{ goto _LL1622;} _LL1622: if(( unsigned int)
_temp1614 > 1u?*(( int*) _temp1614) == Cyc_Absyn_Return_s: 0){ _LL1663:
_temp1662=(( struct Cyc_Absyn_Return_s_struct*) _temp1614)->f1; goto _LL1623;}
else{ goto _LL1624;} _LL1624: if(( unsigned int) _temp1614 > 1u?*(( int*)
_temp1614) == Cyc_Absyn_IfThenElse_s: 0){ _LL1669: _temp1668=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1614)->f1; goto _LL1667; _LL1667: _temp1666=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1614)->f2; goto _LL1665; _LL1665: _temp1664=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1614)->f3; goto _LL1625;} else{ goto _LL1626;} _LL1626: if(( unsigned int)
_temp1614 > 1u?*(( int*) _temp1614) == Cyc_Absyn_While_s: 0){ _LL1673: _temp1672=((
struct Cyc_Absyn_While_s_struct*) _temp1614)->f1; _LL1677: _temp1676= _temp1672.f1;
goto _LL1675; _LL1675: _temp1674= _temp1672.f2; goto _LL1671; _LL1671: _temp1670=((
struct Cyc_Absyn_While_s_struct*) _temp1614)->f2; goto _LL1627;} else{ goto
_LL1628;} _LL1628: if(( unsigned int) _temp1614 > 1u?*(( int*) _temp1614) == Cyc_Absyn_Break_s:
0){ _LL1679: _temp1678=(( struct Cyc_Absyn_Break_s_struct*) _temp1614)->f1; goto
_LL1629;} else{ goto _LL1630;} _LL1630: if(( unsigned int) _temp1614 > 1u?*((
int*) _temp1614) == Cyc_Absyn_Continue_s: 0){ _LL1681: _temp1680=(( struct Cyc_Absyn_Continue_s_struct*)
_temp1614)->f1; goto _LL1631;} else{ goto _LL1632;} _LL1632: if(( unsigned int)
_temp1614 > 1u?*(( int*) _temp1614) == Cyc_Absyn_Goto_s: 0){ _LL1685: _temp1684=((
struct Cyc_Absyn_Goto_s_struct*) _temp1614)->f1; goto _LL1683; _LL1683:
_temp1682=(( struct Cyc_Absyn_Goto_s_struct*) _temp1614)->f2; goto _LL1633;}
else{ goto _LL1634;} _LL1634: if(( unsigned int) _temp1614 > 1u?*(( int*)
_temp1614) == Cyc_Absyn_For_s: 0){ _LL1701: _temp1700=(( struct Cyc_Absyn_For_s_struct*)
_temp1614)->f1; goto _LL1695; _LL1695: _temp1694=(( struct Cyc_Absyn_For_s_struct*)
_temp1614)->f2; _LL1699: _temp1698= _temp1694.f1; goto _LL1697; _LL1697:
_temp1696= _temp1694.f2; goto _LL1689; _LL1689: _temp1688=(( struct Cyc_Absyn_For_s_struct*)
_temp1614)->f3; _LL1693: _temp1692= _temp1688.f1; goto _LL1691; _LL1691:
_temp1690= _temp1688.f2; goto _LL1687; _LL1687: _temp1686=(( struct Cyc_Absyn_For_s_struct*)
_temp1614)->f4; goto _LL1635;} else{ goto _LL1636;} _LL1636: if(( unsigned int)
_temp1614 > 1u?*(( int*) _temp1614) == Cyc_Absyn_Switch_s: 0){ _LL1705:
_temp1704=(( struct Cyc_Absyn_Switch_s_struct*) _temp1614)->f1; goto _LL1703;
_LL1703: _temp1702=(( struct Cyc_Absyn_Switch_s_struct*) _temp1614)->f2; goto
_LL1637;} else{ goto _LL1638;} _LL1638: if(( unsigned int) _temp1614 > 1u?*((
int*) _temp1614) == Cyc_Absyn_Fallthru_s: 0){ _LL1709: _temp1708=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1614)->f1; if( _temp1708 == 0){ goto _LL1707;} else{ goto _LL1640;} _LL1707:
_temp1706=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1614)->f2; goto _LL1639;}
else{ goto _LL1640;} _LL1640: if(( unsigned int) _temp1614 > 1u?*(( int*)
_temp1614) == Cyc_Absyn_Fallthru_s: 0){ _LL1713: _temp1712=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1614)->f1; goto _LL1711; _LL1711: _temp1710=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1614)->f2; goto _LL1641;} else{ goto _LL1642;} _LL1642: if(( unsigned int)
_temp1614 > 1u?*(( int*) _temp1614) == Cyc_Absyn_Decl_s: 0){ _LL1717: _temp1716=((
struct Cyc_Absyn_Decl_s_struct*) _temp1614)->f1; goto _LL1715; _LL1715:
_temp1714=(( struct Cyc_Absyn_Decl_s_struct*) _temp1614)->f2; goto _LL1643;}
else{ goto _LL1644;} _LL1644: if(( unsigned int) _temp1614 > 1u?*(( int*)
_temp1614) == Cyc_Absyn_Cut_s: 0){ _LL1719: _temp1718=(( struct Cyc_Absyn_Cut_s_struct*)
_temp1614)->f1; goto _LL1645;} else{ goto _LL1646;} _LL1646: if(( unsigned int)
_temp1614 > 1u?*(( int*) _temp1614) == Cyc_Absyn_Splice_s: 0){ _LL1721:
_temp1720=(( struct Cyc_Absyn_Splice_s_struct*) _temp1614)->f1; goto _LL1647;}
else{ goto _LL1648;} _LL1648: if(( unsigned int) _temp1614 > 1u?*(( int*)
_temp1614) == Cyc_Absyn_Label_s: 0){ _LL1725: _temp1724=(( struct Cyc_Absyn_Label_s_struct*)
_temp1614)->f1; goto _LL1723; _LL1723: _temp1722=(( struct Cyc_Absyn_Label_s_struct*)
_temp1614)->f2; goto _LL1649;} else{ goto _LL1650;} _LL1650: if(( unsigned int)
_temp1614 > 1u?*(( int*) _temp1614) == Cyc_Absyn_Do_s: 0){ _LL1733: _temp1732=((
struct Cyc_Absyn_Do_s_struct*) _temp1614)->f1; goto _LL1727; _LL1727: _temp1726=((
struct Cyc_Absyn_Do_s_struct*) _temp1614)->f2; _LL1731: _temp1730= _temp1726.f1;
goto _LL1729; _LL1729: _temp1728= _temp1726.f2; goto _LL1651;} else{ goto
_LL1652;} _LL1652: if(( unsigned int) _temp1614 > 1u?*(( int*) _temp1614) == Cyc_Absyn_TryCatch_s:
0){ _LL1737: _temp1736=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1614)->f1;
goto _LL1735; _LL1735: _temp1734=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1614)->f2; goto _LL1653;} else{ goto _LL1654;} _LL1654: if(( unsigned int)
_temp1614 > 1u?*(( int*) _temp1614) == Cyc_Absyn_Region_s: 0){ _LL1743:
_temp1742=(( struct Cyc_Absyn_Region_s_struct*) _temp1614)->f1; goto _LL1741;
_LL1741: _temp1740=(( struct Cyc_Absyn_Region_s_struct*) _temp1614)->f2; goto
_LL1739; _LL1739: _temp1738=(( struct Cyc_Absyn_Region_s_struct*) _temp1614)->f3;
goto _LL1655;} else{ goto _LL1615;} _LL1617: s= Cyc_PP_text(({ char* _temp1744=";";
struct _tagged_string _temp1745; _temp1745.curr= _temp1744; _temp1745.base=
_temp1744; _temp1745.last_plus_one= _temp1744 + 2; _temp1745;})); goto _LL1615;
_LL1619: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc( _temp1656), Cyc_PP_text(({
char* _temp1746=";"; struct _tagged_string _temp1747; _temp1747.curr= _temp1746;
_temp1747.base= _temp1746; _temp1747.last_plus_one= _temp1746 + 2; _temp1747;})));
goto _LL1615; _LL1621: if( Cyc_Absynpp_decls_first){ if( Cyc_Absynpp_is_declaration(
_temp1660)){ s= Cyc_Absynpp_cat6( Cyc_PP_text(({ char* _temp1748="{ "; struct
_tagged_string _temp1749; _temp1749.curr= _temp1748; _temp1749.base= _temp1748;
_temp1749.last_plus_one= _temp1748 + 3; _temp1749;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1660)), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp1750="}"; struct
_tagged_string _temp1751; _temp1751.curr= _temp1750; _temp1751.base= _temp1750;
_temp1751.last_plus_one= _temp1750 + 2; _temp1751;})), Cyc_PP_line_doc(), Cyc_Absynpp_is_declaration(
_temp1658)? Cyc_Absynpp_cat4( Cyc_PP_text(({ char* _temp1752="{ "; struct
_tagged_string _temp1753; _temp1753.curr= _temp1752; _temp1753.base= _temp1752;
_temp1753.last_plus_one= _temp1752 + 3; _temp1753;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1658)), Cyc_PP_text(({ char* _temp1754="}"; struct _tagged_string _temp1755;
_temp1755.curr= _temp1754; _temp1755.base= _temp1754; _temp1755.last_plus_one=
_temp1754 + 2; _temp1755;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1658));} else{ if( Cyc_Absynpp_is_declaration( _temp1658)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1660), Cyc_PP_line_doc(), Cyc_PP_text(({ char*
_temp1756="{ "; struct _tagged_string _temp1757; _temp1757.curr= _temp1756;
_temp1757.base= _temp1756; _temp1757.last_plus_one= _temp1756 + 3; _temp1757;})),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1658)), Cyc_PP_text(({ char*
_temp1758="}"; struct _tagged_string _temp1759; _temp1759.curr= _temp1758;
_temp1759.base= _temp1758; _temp1759.last_plus_one= _temp1758 + 2; _temp1759;})),
Cyc_PP_line_doc());} else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Stmt*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({ char* _temp1760=""; struct
_tagged_string _temp1761; _temp1761.curr= _temp1760; _temp1761.base= _temp1760;
_temp1761.last_plus_one= _temp1760 + 1; _temp1761;}),({ struct Cyc_List_List*
_temp1762=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1762->hd=( void*) _temp1660; _temp1762->tl=({ struct Cyc_List_List*
_temp1763=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1763->hd=( void*) _temp1658; _temp1763->tl= 0; _temp1763;}); _temp1762;}));}}}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({
char* _temp1764=""; struct _tagged_string _temp1765; _temp1765.curr= _temp1764;
_temp1765.base= _temp1764; _temp1765.last_plus_one= _temp1764 + 1; _temp1765;}),({
struct Cyc_List_List* _temp1766=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1766->hd=( void*) _temp1660; _temp1766->tl=({
struct Cyc_List_List* _temp1767=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1767->hd=( void*) _temp1658; _temp1767->tl= 0;
_temp1767;}); _temp1766;}));} goto _LL1615; _LL1623: if( _temp1662 == 0){ s= Cyc_PP_text(({
char* _temp1768="return;"; struct _tagged_string _temp1769; _temp1769.curr=
_temp1768; _temp1769.base= _temp1768; _temp1769.last_plus_one= _temp1768 + 8;
_temp1769;}));} else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1770="return ";
struct _tagged_string _temp1771; _temp1771.curr= _temp1770; _temp1771.base=
_temp1770; _temp1771.last_plus_one= _temp1770 + 8; _temp1771;})), _temp1662 == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null(
_temp1662)), Cyc_PP_text(({ char* _temp1772=";"; struct _tagged_string _temp1773;
_temp1773.curr= _temp1772; _temp1773.base= _temp1772; _temp1773.last_plus_one=
_temp1772 + 2; _temp1773;})));} goto _LL1615; _LL1625: { int print_else;{ void*
_temp1774=( void*) _temp1664->r; _LL1776: if( _temp1774 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1777;} else{ goto _LL1778;} _LL1778: goto _LL1779; _LL1777: print_else=
0; goto _LL1775; _LL1779: print_else= 1; goto _LL1775; _LL1775:;} s= Cyc_Absynpp_cat7(
Cyc_PP_text(({ char* _temp1780="if ("; struct _tagged_string _temp1781;
_temp1781.curr= _temp1780; _temp1781.base= _temp1780; _temp1781.last_plus_one=
_temp1780 + 5; _temp1781;})), Cyc_Absynpp_exp2doc( _temp1668), Cyc_PP_text(({
char* _temp1782=") {"; struct _tagged_string _temp1783; _temp1783.curr=
_temp1782; _temp1783.base= _temp1782; _temp1783.last_plus_one= _temp1782 + 4;
_temp1783;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp1666))), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp1784="}"; struct
_tagged_string _temp1785; _temp1785.curr= _temp1784; _temp1785.base= _temp1784;
_temp1785.last_plus_one= _temp1784 + 2; _temp1785;})), print_else? Cyc_Absynpp_cat5(
Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp1786="else {"; struct
_tagged_string _temp1787; _temp1787.curr= _temp1786; _temp1787.base= _temp1786;
_temp1787.last_plus_one= _temp1786 + 7; _temp1787;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1664))), Cyc_PP_line_doc(), Cyc_PP_text(({
char* _temp1788="}"; struct _tagged_string _temp1789; _temp1789.curr= _temp1788;
_temp1789.base= _temp1788; _temp1789.last_plus_one= _temp1788 + 2; _temp1789;}))):
Cyc_PP_nil_doc()); goto _LL1615;} _LL1627: s= Cyc_Absynpp_cat6( Cyc_PP_text(({
char* _temp1790="while ("; struct _tagged_string _temp1791; _temp1791.curr=
_temp1790; _temp1791.base= _temp1790; _temp1791.last_plus_one= _temp1790 + 8;
_temp1791;})), Cyc_Absynpp_exp2doc( _temp1676), Cyc_PP_text(({ char* _temp1792=") {";
struct _tagged_string _temp1793; _temp1793.curr= _temp1792; _temp1793.base=
_temp1792; _temp1793.last_plus_one= _temp1792 + 4; _temp1793;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1670))), Cyc_PP_line_doc(),
Cyc_PP_text(({ char* _temp1794="}"; struct _tagged_string _temp1795; _temp1795.curr=
_temp1794; _temp1795.base= _temp1794; _temp1795.last_plus_one= _temp1794 + 2;
_temp1795;}))); goto _LL1615; _LL1629: s= Cyc_PP_text(({ char* _temp1796="break;";
struct _tagged_string _temp1797; _temp1797.curr= _temp1796; _temp1797.base=
_temp1796; _temp1797.last_plus_one= _temp1796 + 7; _temp1797;})); goto _LL1615;
_LL1631: s= Cyc_PP_text(({ char* _temp1798="continue;"; struct _tagged_string
_temp1799; _temp1799.curr= _temp1798; _temp1799.base= _temp1798; _temp1799.last_plus_one=
_temp1798 + 10; _temp1799;})); goto _LL1615; _LL1633: s= Cyc_PP_text(({ struct
_tagged_string _temp1800=* _temp1684; xprintf("goto %.*s;", _temp1800.last_plus_one
- _temp1800.curr, _temp1800.curr);})); goto _LL1615; _LL1635: s= Cyc_Absynpp_cat10(
Cyc_PP_text(({ char* _temp1801="for("; struct _tagged_string _temp1802;
_temp1802.curr= _temp1801; _temp1802.base= _temp1801; _temp1802.last_plus_one=
_temp1801 + 5; _temp1802;})), Cyc_Absynpp_exp2doc( _temp1700), Cyc_PP_text(({
char* _temp1803="; "; struct _tagged_string _temp1804; _temp1804.curr= _temp1803;
_temp1804.base= _temp1803; _temp1804.last_plus_one= _temp1803 + 3; _temp1804;})),
Cyc_Absynpp_exp2doc( _temp1698), Cyc_PP_text(({ char* _temp1805="; "; struct
_tagged_string _temp1806; _temp1806.curr= _temp1805; _temp1806.base= _temp1805;
_temp1806.last_plus_one= _temp1805 + 3; _temp1806;})), Cyc_Absynpp_exp2doc(
_temp1692), Cyc_PP_text(({ char* _temp1807=") {"; struct _tagged_string
_temp1808; _temp1808.curr= _temp1807; _temp1808.base= _temp1807; _temp1808.last_plus_one=
_temp1807 + 4; _temp1808;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1686))), Cyc_PP_line_doc(), Cyc_PP_text(({ char*
_temp1809="}"; struct _tagged_string _temp1810; _temp1810.curr= _temp1809;
_temp1810.base= _temp1809; _temp1810.last_plus_one= _temp1809 + 2; _temp1810;})));
goto _LL1615; _LL1637: s= Cyc_Absynpp_cat7( Cyc_PP_text(({ char* _temp1811="switch (";
struct _tagged_string _temp1812; _temp1812.curr= _temp1811; _temp1812.base=
_temp1811; _temp1812.last_plus_one= _temp1811 + 9; _temp1812;})), Cyc_Absynpp_exp2doc(
_temp1704), Cyc_PP_text(({ char* _temp1813=") {"; struct _tagged_string
_temp1814; _temp1814.curr= _temp1813; _temp1814.base= _temp1813; _temp1814.last_plus_one=
_temp1813 + 4; _temp1814;})), Cyc_PP_line_doc(), Cyc_Absynpp_switchclauses2doc(
_temp1702), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp1815="}"; struct
_tagged_string _temp1816; _temp1816.curr= _temp1815; _temp1816.base= _temp1815;
_temp1816.last_plus_one= _temp1815 + 2; _temp1816;}))); goto _LL1615; _LL1639: s=
Cyc_PP_text(({ char* _temp1817="fallthru;"; struct _tagged_string _temp1818;
_temp1818.curr= _temp1817; _temp1818.base= _temp1817; _temp1818.last_plus_one=
_temp1817 + 10; _temp1818;})); goto _LL1615; _LL1641: s= Cyc_Absynpp_cat3( Cyc_PP_text(({
char* _temp1819="fallthru("; struct _tagged_string _temp1820; _temp1820.curr=
_temp1819; _temp1820.base= _temp1819; _temp1820.last_plus_one= _temp1819 + 10;
_temp1820;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1712), Cyc_PP_text(({ char*
_temp1821=");"; struct _tagged_string _temp1822; _temp1822.curr= _temp1821;
_temp1822.base= _temp1821; _temp1822.last_plus_one= _temp1821 + 3; _temp1822;})));
goto _LL1615; _LL1643: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1716),
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1714)); goto _LL1615; _LL1645: s=
Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp1823="cut "; struct _tagged_string
_temp1824; _temp1824.curr= _temp1823; _temp1824.base= _temp1823; _temp1824.last_plus_one=
_temp1823 + 5; _temp1824;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1718)));
goto _LL1615; _LL1647: s= Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp1825="splice ";
struct _tagged_string _temp1826; _temp1826.curr= _temp1825; _temp1826.base=
_temp1825; _temp1826.last_plus_one= _temp1825 + 8; _temp1826;})), Cyc_PP_nest( 2,
Cyc_Absynpp_stmt2doc( _temp1720))); goto _LL1615; _LL1649: if( Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration( _temp1722): 0){ s= Cyc_Absynpp_cat6( Cyc_Absynpp_textptr(
_temp1724), Cyc_PP_text(({ char* _temp1827=": {"; struct _tagged_string
_temp1828; _temp1828.curr= _temp1827; _temp1828.base= _temp1827; _temp1828.last_plus_one=
_temp1827 + 4; _temp1828;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1722)), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp1829="}"; struct
_tagged_string _temp1830; _temp1830.curr= _temp1829; _temp1830.base= _temp1829;
_temp1830.last_plus_one= _temp1829 + 2; _temp1830;})));} else{ s= Cyc_Absynpp_cat3(
Cyc_Absynpp_textptr( _temp1724), Cyc_PP_text(({ char* _temp1831=": "; struct
_tagged_string _temp1832; _temp1832.curr= _temp1831; _temp1832.base= _temp1831;
_temp1832.last_plus_one= _temp1831 + 3; _temp1832;})), Cyc_Absynpp_stmt2doc(
_temp1722));} goto _LL1615; _LL1651: s= Cyc_Absynpp_cat7( Cyc_PP_text(({ char*
_temp1833="do {"; struct _tagged_string _temp1834; _temp1834.curr= _temp1833;
_temp1834.base= _temp1833; _temp1834.last_plus_one= _temp1833 + 5; _temp1834;})),
Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1732)), Cyc_PP_line_doc(),
Cyc_PP_text(({ char* _temp1835="} while ("; struct _tagged_string _temp1836;
_temp1836.curr= _temp1835; _temp1836.base= _temp1835; _temp1836.last_plus_one=
_temp1835 + 10; _temp1836;})), Cyc_Absynpp_exp2doc( _temp1730), Cyc_PP_text(({
char* _temp1837=");"; struct _tagged_string _temp1838; _temp1838.curr= _temp1837;
_temp1838.base= _temp1837; _temp1838.last_plus_one= _temp1837 + 3; _temp1838;})));
goto _LL1615; _LL1653: s= Cyc_Absynpp_cat9( Cyc_PP_text(({ char* _temp1839="try {";
struct _tagged_string _temp1840; _temp1840.curr= _temp1839; _temp1840.base=
_temp1839; _temp1840.last_plus_one= _temp1839 + 6; _temp1840;})), Cyc_PP_line_doc(),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1736)), Cyc_PP_line_doc(), Cyc_PP_text(({
char* _temp1841="} catch {"; struct _tagged_string _temp1842; _temp1842.curr=
_temp1841; _temp1842.base= _temp1841; _temp1842.last_plus_one= _temp1841 + 10;
_temp1842;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1734)), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp1843="}"; struct
_tagged_string _temp1844; _temp1844.curr= _temp1843; _temp1844.base= _temp1843;
_temp1844.last_plus_one= _temp1843 + 2; _temp1844;}))); goto _LL1615; _LL1655: s=
Cyc_Absynpp_cat9( Cyc_PP_text(({ char* _temp1845="region<"; struct
_tagged_string _temp1846; _temp1846.curr= _temp1845; _temp1846.base= _temp1845;
_temp1846.last_plus_one= _temp1845 + 8; _temp1846;})), Cyc_Absynpp_textptr( Cyc_Absynpp_get_name(
_temp1742)), Cyc_PP_text(({ char* _temp1847=">"; struct _tagged_string _temp1848;
_temp1848.curr= _temp1847; _temp1848.base= _temp1847; _temp1848.last_plus_one=
_temp1847 + 2; _temp1848;})), Cyc_Absynpp_qvar2doc( _temp1740->name), Cyc_PP_text(({
char* _temp1849="{"; struct _tagged_string _temp1850; _temp1850.curr= _temp1849;
_temp1850.base= _temp1849; _temp1850.last_plus_one= _temp1849 + 2; _temp1850;})),
Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1738)), Cyc_PP_line_doc(),
Cyc_PP_text(({ char* _temp1851="}"; struct _tagged_string _temp1852; _temp1852.curr=
_temp1851; _temp1852.base= _temp1851; _temp1852.last_plus_one= _temp1851 + 2;
_temp1852;}))); goto _LL1615; _LL1615:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1853=( void*) p->r;
int _temp1885; void* _temp1887; char _temp1889; struct _tagged_string _temp1891;
struct Cyc_Absyn_Vardecl* _temp1893; struct Cyc_List_List* _temp1895; struct Cyc_Absyn_Pat*
_temp1897; struct Cyc_Absyn_Vardecl* _temp1899; struct _tuple0* _temp1901;
struct Cyc_List_List* _temp1903; struct Cyc_List_List* _temp1905; struct _tuple0*
_temp1907; struct Cyc_List_List* _temp1909; struct Cyc_List_List* _temp1911;
struct _tuple0* _temp1913; struct Cyc_List_List* _temp1915; struct Cyc_List_List*
_temp1917; struct Cyc_Core_Opt* _temp1919; struct Cyc_Absyn_Structdecl*
_temp1921; struct Cyc_Absyn_Enumfield* _temp1923; struct Cyc_Absyn_Enumdecl*
_temp1925; struct Cyc_List_List* _temp1927; struct Cyc_List_List* _temp1929;
struct Cyc_Absyn_Tunionfield* _temp1931; struct Cyc_Absyn_Tuniondecl* _temp1933;
_LL1855: if( _temp1853 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1856;} else{ goto
_LL1857;} _LL1857: if( _temp1853 ==( void*) Cyc_Absyn_Null_p){ goto _LL1858;}
else{ goto _LL1859;} _LL1859: if(( unsigned int) _temp1853 > 2u?*(( int*)
_temp1853) == Cyc_Absyn_Int_p: 0){ _LL1888: _temp1887=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp1853)->f1; goto _LL1886; _LL1886: _temp1885=(( struct Cyc_Absyn_Int_p_struct*)
_temp1853)->f2; goto _LL1860;} else{ goto _LL1861;} _LL1861: if(( unsigned int)
_temp1853 > 2u?*(( int*) _temp1853) == Cyc_Absyn_Char_p: 0){ _LL1890: _temp1889=((
struct Cyc_Absyn_Char_p_struct*) _temp1853)->f1; goto _LL1862;} else{ goto
_LL1863;} _LL1863: if(( unsigned int) _temp1853 > 2u?*(( int*) _temp1853) == Cyc_Absyn_Float_p:
0){ _LL1892: _temp1891=(( struct Cyc_Absyn_Float_p_struct*) _temp1853)->f1; goto
_LL1864;} else{ goto _LL1865;} _LL1865: if(( unsigned int) _temp1853 > 2u?*((
int*) _temp1853) == Cyc_Absyn_Var_p: 0){ _LL1894: _temp1893=(( struct Cyc_Absyn_Var_p_struct*)
_temp1853)->f1; goto _LL1866;} else{ goto _LL1867;} _LL1867: if(( unsigned int)
_temp1853 > 2u?*(( int*) _temp1853) == Cyc_Absyn_Tuple_p: 0){ _LL1896: _temp1895=((
struct Cyc_Absyn_Tuple_p_struct*) _temp1853)->f1; goto _LL1868;} else{ goto
_LL1869;} _LL1869: if(( unsigned int) _temp1853 > 2u?*(( int*) _temp1853) == Cyc_Absyn_Pointer_p:
0){ _LL1898: _temp1897=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1853)->f1;
goto _LL1870;} else{ goto _LL1871;} _LL1871: if(( unsigned int) _temp1853 > 2u?*((
int*) _temp1853) == Cyc_Absyn_Reference_p: 0){ _LL1900: _temp1899=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1853)->f1; goto _LL1872;} else{ goto _LL1873;} _LL1873: if(( unsigned int)
_temp1853 > 2u?*(( int*) _temp1853) == Cyc_Absyn_UnknownId_p: 0){ _LL1902:
_temp1901=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1853)->f1; goto _LL1874;}
else{ goto _LL1875;} _LL1875: if(( unsigned int) _temp1853 > 2u?*(( int*)
_temp1853) == Cyc_Absyn_UnknownCall_p: 0){ _LL1908: _temp1907=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1853)->f1; goto _LL1906; _LL1906: _temp1905=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1853)->f2; goto _LL1904; _LL1904: _temp1903=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1853)->f3; goto _LL1876;} else{ goto _LL1877;} _LL1877: if(( unsigned int)
_temp1853 > 2u?*(( int*) _temp1853) == Cyc_Absyn_UnknownFields_p: 0){ _LL1914:
_temp1913=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1853)->f1; goto
_LL1912; _LL1912: _temp1911=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1853)->f2; goto _LL1910; _LL1910: _temp1909=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1853)->f3; goto _LL1878;} else{ goto _LL1879;} _LL1879: if(( unsigned int)
_temp1853 > 2u?*(( int*) _temp1853) == Cyc_Absyn_Struct_p: 0){ _LL1922:
_temp1921=(( struct Cyc_Absyn_Struct_p_struct*) _temp1853)->f1; goto _LL1920;
_LL1920: _temp1919=(( struct Cyc_Absyn_Struct_p_struct*) _temp1853)->f2; goto
_LL1918; _LL1918: _temp1917=(( struct Cyc_Absyn_Struct_p_struct*) _temp1853)->f3;
goto _LL1916; _LL1916: _temp1915=(( struct Cyc_Absyn_Struct_p_struct*) _temp1853)->f4;
goto _LL1880;} else{ goto _LL1881;} _LL1881: if(( unsigned int) _temp1853 > 2u?*((
int*) _temp1853) == Cyc_Absyn_Enum_p: 0){ _LL1926: _temp1925=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1853)->f1; goto _LL1924; _LL1924: _temp1923=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1853)->f2; goto _LL1882;} else{ goto _LL1883;} _LL1883: if(( unsigned int)
_temp1853 > 2u?*(( int*) _temp1853) == Cyc_Absyn_Tunion_p: 0){ _LL1934:
_temp1933=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1853)->f1; goto _LL1932;
_LL1932: _temp1931=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1853)->f2; goto
_LL1930; _LL1930: _temp1929=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1853)->f3;
goto _LL1928; _LL1928: _temp1927=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1853)->f4;
goto _LL1884;} else{ goto _LL1854;} _LL1856: s= Cyc_PP_text(({ char* _temp1935="_";
struct _tagged_string _temp1936; _temp1936.curr= _temp1935; _temp1936.base=
_temp1935; _temp1936.last_plus_one= _temp1935 + 2; _temp1936;})); goto _LL1854;
_LL1858: s= Cyc_PP_text(({ char* _temp1937="null"; struct _tagged_string
_temp1938; _temp1938.curr= _temp1937; _temp1938.base= _temp1937; _temp1938.last_plus_one=
_temp1937 + 5; _temp1938;})); goto _LL1854; _LL1860: if( _temp1887 ==( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d", _temp1885));} else{ s= Cyc_PP_text(
xprintf("%u",( unsigned int) _temp1885));} goto _LL1854; _LL1862: s= Cyc_PP_text(({
struct _tagged_string _temp1939= Cyc_Absynpp_char_escape( _temp1889); xprintf("'%.*s'",
_temp1939.last_plus_one - _temp1939.curr, _temp1939.curr);})); goto _LL1854;
_LL1864: s= Cyc_PP_text( _temp1891); goto _LL1854; _LL1866: s= Cyc_Absynpp_qvar2doc(
_temp1893->name); goto _LL1854; _LL1868: s= Cyc_Absynpp_cat3( Cyc_PP_text(({
char* _temp1940="$("; struct _tagged_string _temp1941; _temp1941.curr= _temp1940;
_temp1941.base= _temp1940; _temp1941.last_plus_one= _temp1940 + 3; _temp1941;})),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,({
char* _temp1942=","; struct _tagged_string _temp1943; _temp1943.curr= _temp1942;
_temp1943.base= _temp1942; _temp1943.last_plus_one= _temp1942 + 2; _temp1943;}),
_temp1895), Cyc_PP_text(({ char* _temp1944=")"; struct _tagged_string _temp1945;
_temp1945.curr= _temp1944; _temp1945.base= _temp1944; _temp1945.last_plus_one=
_temp1944 + 2; _temp1945;}))); goto _LL1854; _LL1870: s= Cyc_Absynpp_cat2( Cyc_PP_text(({
char* _temp1946="&"; struct _tagged_string _temp1947; _temp1947.curr= _temp1946;
_temp1947.base= _temp1946; _temp1947.last_plus_one= _temp1946 + 2; _temp1947;})),
Cyc_Absynpp_pat2doc( _temp1897)); goto _LL1854; _LL1872: s= Cyc_Absynpp_cat2(
Cyc_PP_text(({ char* _temp1948="*"; struct _tagged_string _temp1949; _temp1949.curr=
_temp1948; _temp1949.base= _temp1948; _temp1949.last_plus_one= _temp1948 + 2;
_temp1949;})), Cyc_Absynpp_qvar2doc( _temp1899->name)); goto _LL1854; _LL1874: s=
Cyc_Absynpp_qvar2doc( _temp1901); goto _LL1854; _LL1876: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1907), Cyc_Absynpp_tvars2doc( _temp1905), Cyc_Absynpp_group(({
char* _temp1950="("; struct _tagged_string _temp1951; _temp1951.curr= _temp1950;
_temp1951.base= _temp1950; _temp1951.last_plus_one= _temp1950 + 2; _temp1951;}),({
char* _temp1952=")"; struct _tagged_string _temp1953; _temp1953.curr= _temp1952;
_temp1953.base= _temp1952; _temp1953.last_plus_one= _temp1952 + 2; _temp1953;}),({
char* _temp1954=","; struct _tagged_string _temp1955; _temp1955.curr= _temp1954;
_temp1955.base= _temp1954; _temp1955.last_plus_one= _temp1954 + 2; _temp1955;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp1903))); goto
_LL1854; _LL1878: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1913), Cyc_Absynpp_tvars2doc(
_temp1911), Cyc_Absynpp_group(({ char* _temp1956="{"; struct _tagged_string
_temp1957; _temp1957.curr= _temp1956; _temp1957.base= _temp1956; _temp1957.last_plus_one=
_temp1956 + 2; _temp1957;}),({ char* _temp1958="}"; struct _tagged_string
_temp1959; _temp1959.curr= _temp1958; _temp1959.base= _temp1958; _temp1959.last_plus_one=
_temp1958 + 2; _temp1959;}),({ char* _temp1960=","; struct _tagged_string
_temp1961; _temp1961.curr= _temp1960; _temp1961.base= _temp1960; _temp1961.last_plus_one=
_temp1960 + 2; _temp1961;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1909))); goto _LL1854; _LL1880: s= Cyc_Absynpp_cat3( _temp1921->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp1921->name))->v), Cyc_Absynpp_tvars2doc( _temp1917), Cyc_Absynpp_group(({
char* _temp1962="{"; struct _tagged_string _temp1963; _temp1963.curr= _temp1962;
_temp1963.base= _temp1962; _temp1963.last_plus_one= _temp1962 + 2; _temp1963;}),({
char* _temp1964="}"; struct _tagged_string _temp1965; _temp1965.curr= _temp1964;
_temp1965.base= _temp1964; _temp1965.last_plus_one= _temp1964 + 2; _temp1965;}),({
char* _temp1966=","; struct _tagged_string _temp1967; _temp1967.curr= _temp1966;
_temp1967.base= _temp1966; _temp1967.last_plus_one= _temp1966 + 2; _temp1967;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_dp2doc, _temp1915))); goto _LL1854; _LL1882: s=
Cyc_Absynpp_qvar2doc( _temp1923->name); goto _LL1854; _LL1884: if( _temp1927 ==
0){ s= Cyc_Absynpp_qvar2doc( _temp1931->name);} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1931->name), Cyc_Absynpp_tvars2doc( _temp1929), Cyc_Absynpp_egroup(({ char*
_temp1968="("; struct _tagged_string _temp1969; _temp1969.curr= _temp1968;
_temp1969.base= _temp1968; _temp1969.last_plus_one= _temp1968 + 2; _temp1969;}),({
char* _temp1970=")"; struct _tagged_string _temp1971; _temp1971.curr= _temp1970;
_temp1971.base= _temp1970; _temp1971.last_plus_one= _temp1970 + 2; _temp1971;}),({
char* _temp1972=","; struct _tagged_string _temp1973; _temp1973.curr= _temp1972;
_temp1973.base= _temp1972; _temp1973.last_plus_one= _temp1972 + 2; _temp1973;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp1927)));}
goto _LL1854; _LL1854:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc(
struct _tuple5* dp){ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup(({ char*
_temp1974=""; struct _tagged_string _temp1975; _temp1975.curr= _temp1974;
_temp1975.base= _temp1974; _temp1975.last_plus_one= _temp1974 + 1; _temp1975;}),({
char* _temp1976="="; struct _tagged_string _temp1977; _temp1977.curr= _temp1976;
_temp1977.base= _temp1976; _temp1977.last_plus_one= _temp1976 + 2; _temp1977;}),({
char* _temp1978="="; struct _tagged_string _temp1979; _temp1979.curr= _temp1978;
_temp1979.base= _temp1978; _temp1979.last_plus_one= _temp1978 + 2; _temp1979;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1)), Cyc_Absynpp_pat2doc((*
dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc( struct Cyc_Absyn_Switch_clause*
c){ if( c->where_clause == 0?( void*)( c->pattern)->r ==( void*) Cyc_Absyn_Wild_p:
0){ return Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp1980="default: "; struct
_tagged_string _temp1981; _temp1981.curr= _temp1980; _temp1981.base= _temp1980;
_temp1981.last_plus_one= _temp1980 + 10; _temp1981;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause
== 0){ return Cyc_Absynpp_cat4( Cyc_PP_text(({ char* _temp1982="case "; struct
_tagged_string _temp1983; _temp1983.curr= _temp1982; _temp1983.base= _temp1982;
_temp1983.last_plus_one= _temp1982 + 6; _temp1983;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(({ char* _temp1984=": "; struct _tagged_string _temp1985; _temp1985.curr=
_temp1984; _temp1985.base= _temp1984; _temp1985.last_plus_one= _temp1984 + 3;
_temp1985;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
c->body))));} else{ return Cyc_Absynpp_cat6( Cyc_PP_text(({ char* _temp1986="case ";
struct _tagged_string _temp1987; _temp1987.curr= _temp1986; _temp1987.base=
_temp1986; _temp1987.last_plus_one= _temp1986 + 6; _temp1987;})), Cyc_Absynpp_pat2doc(
c->pattern), Cyc_PP_text(({ char* _temp1988=" && "; struct _tagged_string
_temp1989; _temp1989.curr= _temp1988; _temp1989.base= _temp1988; _temp1989.last_plus_one=
_temp1988 + 5; _temp1989;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( c->where_clause)), Cyc_PP_text(({ char* _temp1990=": "; struct
_tagged_string _temp1991; _temp1991.curr= _temp1990; _temp1991.base= _temp1990;
_temp1991.last_plus_one= _temp1990 + 3; _temp1991;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,({ char* _temp1992=""; struct
_tagged_string _temp1993; _temp1993.curr= _temp1992; _temp1993.base= _temp1992;
_temp1993.last_plus_one= _temp1992 + 1; _temp1993;}), cs);} struct Cyc_PP_Doc*
Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield* f){ if( f->tag == 0){
return Cyc_Absynpp_qvar2doc( f->name);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
f->name), Cyc_PP_text(({ char* _temp1994=" = "; struct _tagged_string _temp1995;
_temp1995.curr= _temp1994; _temp1995.base= _temp1994; _temp1995.last_plus_one=
_temp1994 + 4; _temp1995;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( f->tag)));}} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct
Cyc_List_List* fs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Enumfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,({ char* _temp1996=","; struct
_tagged_string _temp1997; _temp1997.curr= _temp1996; _temp1997.base= _temp1996;
_temp1997.last_plus_one= _temp1996 + 2; _temp1997;}), fs);} static struct Cyc_PP_Doc*
Cyc_Absynpp_id2doc( struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);}
static struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){
return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,({
char* _temp1998=","; struct _tagged_string _temp1999; _temp1999.curr= _temp1998;
_temp1999.base= _temp1998; _temp1999.last_plus_one= _temp1998 + 2; _temp1999;}),
vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl* d){
struct Cyc_PP_Doc* s;{ void* _temp2000=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp2026; struct Cyc_Absyn_Structdecl* _temp2028; struct Cyc_Absyn_Uniondecl*
_temp2030; struct Cyc_Absyn_Vardecl* _temp2032; struct Cyc_Absyn_Vardecl
_temp2034; struct Cyc_List_List* _temp2035; struct Cyc_Core_Opt* _temp2037;
struct Cyc_Absyn_Exp* _temp2039; void* _temp2041; struct Cyc_Absyn_Tqual
_temp2043; struct _tuple0* _temp2045; void* _temp2047; struct Cyc_Absyn_Tuniondecl*
_temp2049; struct Cyc_Absyn_Tuniondecl _temp2051; int _temp2052; struct Cyc_Core_Opt*
_temp2054; struct Cyc_List_List* _temp2056; struct _tuple0* _temp2058; void*
_temp2060; int _temp2062; struct Cyc_Absyn_Exp* _temp2064; struct Cyc_Core_Opt*
_temp2066; struct Cyc_Core_Opt* _temp2068; struct Cyc_Absyn_Pat* _temp2070;
struct Cyc_List_List* _temp2072; struct Cyc_Absyn_Enumdecl* _temp2074; struct
Cyc_Absyn_Enumdecl _temp2076; struct Cyc_Core_Opt* _temp2077; struct _tuple0*
_temp2079; void* _temp2081; struct Cyc_Absyn_Typedefdecl* _temp2083; struct Cyc_List_List*
_temp2085; struct _tagged_string* _temp2087; struct Cyc_List_List* _temp2089;
struct _tuple0* _temp2091; struct Cyc_List_List* _temp2093; _LL2002: if(*(( int*)
_temp2000) == Cyc_Absyn_Fn_d){ _LL2027: _temp2026=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2000)->f1; goto _LL2003;} else{ goto _LL2004;} _LL2004: if(*(( int*)
_temp2000) == Cyc_Absyn_Struct_d){ _LL2029: _temp2028=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2000)->f1; goto _LL2005;} else{ goto _LL2006;} _LL2006: if(*(( int*)
_temp2000) == Cyc_Absyn_Union_d){ _LL2031: _temp2030=(( struct Cyc_Absyn_Union_d_struct*)
_temp2000)->f1; goto _LL2007;} else{ goto _LL2008;} _LL2008: if(*(( int*)
_temp2000) == Cyc_Absyn_Var_d){ _LL2033: _temp2032=(( struct Cyc_Absyn_Var_d_struct*)
_temp2000)->f1; _temp2034=* _temp2032; _LL2048: _temp2047=( void*) _temp2034.sc;
goto _LL2046; _LL2046: _temp2045= _temp2034.name; goto _LL2044; _LL2044:
_temp2043= _temp2034.tq; goto _LL2042; _LL2042: _temp2041=( void*) _temp2034.type;
goto _LL2040; _LL2040: _temp2039= _temp2034.initializer; goto _LL2038; _LL2038:
_temp2037= _temp2034.rgn; goto _LL2036; _LL2036: _temp2035= _temp2034.attributes;
goto _LL2009;} else{ goto _LL2010;} _LL2010: if(*(( int*) _temp2000) == Cyc_Absyn_Tunion_d){
_LL2050: _temp2049=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2000)->f1;
_temp2051=* _temp2049; _LL2061: _temp2060=( void*) _temp2051.sc; goto _LL2059;
_LL2059: _temp2058= _temp2051.name; goto _LL2057; _LL2057: _temp2056= _temp2051.tvs;
goto _LL2055; _LL2055: _temp2054= _temp2051.fields; goto _LL2053; _LL2053:
_temp2052= _temp2051.is_xtunion; goto _LL2011;} else{ goto _LL2012;} _LL2012:
if(*(( int*) _temp2000) == Cyc_Absyn_Let_d){ _LL2071: _temp2070=(( struct Cyc_Absyn_Let_d_struct*)
_temp2000)->f1; goto _LL2069; _LL2069: _temp2068=(( struct Cyc_Absyn_Let_d_struct*)
_temp2000)->f2; goto _LL2067; _LL2067: _temp2066=(( struct Cyc_Absyn_Let_d_struct*)
_temp2000)->f3; goto _LL2065; _LL2065: _temp2064=(( struct Cyc_Absyn_Let_d_struct*)
_temp2000)->f4; goto _LL2063; _LL2063: _temp2062=(( struct Cyc_Absyn_Let_d_struct*)
_temp2000)->f5; goto _LL2013;} else{ goto _LL2014;} _LL2014: if(*(( int*)
_temp2000) == Cyc_Absyn_Letv_d){ _LL2073: _temp2072=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2000)->f1; goto _LL2015;} else{ goto _LL2016;} _LL2016: if(*(( int*)
_temp2000) == Cyc_Absyn_Enum_d){ _LL2075: _temp2074=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2000)->f1; _temp2076=* _temp2074; _LL2082: _temp2081=( void*) _temp2076.sc;
goto _LL2080; _LL2080: _temp2079= _temp2076.name; goto _LL2078; _LL2078:
_temp2077= _temp2076.fields; goto _LL2017;} else{ goto _LL2018;} _LL2018: if(*((
int*) _temp2000) == Cyc_Absyn_Typedef_d){ _LL2084: _temp2083=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2000)->f1; goto _LL2019;} else{ goto _LL2020;} _LL2020: if(*(( int*)
_temp2000) == Cyc_Absyn_Namespace_d){ _LL2088: _temp2087=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2000)->f1; goto _LL2086; _LL2086: _temp2085=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2000)->f2; goto _LL2021;} else{ goto _LL2022;} _LL2022: if(*(( int*)
_temp2000) == Cyc_Absyn_Using_d){ _LL2092: _temp2091=(( struct Cyc_Absyn_Using_d_struct*)
_temp2000)->f1; goto _LL2090; _LL2090: _temp2089=(( struct Cyc_Absyn_Using_d_struct*)
_temp2000)->f2; goto _LL2023;} else{ goto _LL2024;} _LL2024: if(*(( int*)
_temp2000) == Cyc_Absyn_ExternC_d){ _LL2094: _temp2093=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2000)->f1; goto _LL2025;} else{ goto _LL2001;} _LL2003: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2107=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2107[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2108; _temp2108.tag= Cyc_Absyn_FnType; _temp2108.f1=({ struct Cyc_Absyn_FnInfo
_temp2109; _temp2109.tvars= _temp2026->tvs; _temp2109.effect= _temp2026->effect;
_temp2109.ret_typ=( void*)(( void*) _temp2026->ret_type); _temp2109.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2026->args); _temp2109.c_varargs=
_temp2026->c_varargs; _temp2109.cyc_varargs= _temp2026->cyc_varargs; _temp2109.attributes=
0; _temp2109;}); _temp2108;}); _temp2107;}); s= Cyc_Absynpp_cat7( Cyc_PP_text(
_temp2026->is_inline? Cyc_Absynpp_to_VC?({ char* _temp2095="__inline "; struct
_tagged_string _temp2096; _temp2096.curr= _temp2095; _temp2096.base= _temp2095;
_temp2096.last_plus_one= _temp2095 + 10; _temp2096;}):({ char* _temp2097="inline ";
struct _tagged_string _temp2098; _temp2098.curr= _temp2097; _temp2098.base=
_temp2097; _temp2098.last_plus_one= _temp2097 + 8; _temp2098;}):({ char*
_temp2099=""; struct _tagged_string _temp2100; _temp2100.curr= _temp2099;
_temp2100.base= _temp2099; _temp2100.last_plus_one= _temp2099 + 1; _temp2100;})),
Cyc_Absynpp_scope2doc(( void*) _temp2026->sc), Cyc_Absynpp_tqtd2doc(({ struct
Cyc_Absyn_Tqual _temp2101; _temp2101.q_const= 0; _temp2101.q_volatile= 0;
_temp2101.q_restrict= 0; _temp2101;}), t,({ struct Cyc_Core_Opt* _temp2102=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2102->v=(
void*) Cyc_Absynpp_cat2( Cyc_Absynpp_atts2doc( _temp2026->attributes), Cyc_Absynpp_typedef_name2doc(
_temp2026->name)); _temp2102;})), Cyc_PP_text(({ char* _temp2103=" {"; struct
_tagged_string _temp2104; _temp2104.curr= _temp2103; _temp2104.base= _temp2103;
_temp2104.last_plus_one= _temp2103 + 3; _temp2104;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp2026->body))), Cyc_PP_line_doc(),
Cyc_PP_text(({ char* _temp2105="}"; struct _tagged_string _temp2106; _temp2106.curr=
_temp2105; _temp2106.base= _temp2105; _temp2106.last_plus_one= _temp2105 + 2;
_temp2106;}))); goto _LL2001;} _LL2005: if( _temp2028->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp2028->sc), Cyc_PP_text(({ char* _temp2110="struct ";
struct _tagged_string _temp2111; _temp2111.curr= _temp2110; _temp2111.base=
_temp2110; _temp2111.last_plus_one= _temp2110 + 8; _temp2111;})), _temp2028->name
== 0? Cyc_PP_text(({ char* _temp2112=""; struct _tagged_string _temp2113;
_temp2113.curr= _temp2112; _temp2113.base= _temp2112; _temp2113.last_plus_one=
_temp2112 + 1; _temp2113;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp2028->name))->v), Cyc_Absynpp_ktvars2doc(
_temp2028->tvs), Cyc_PP_text(({ char* _temp2114=";"; struct _tagged_string
_temp2115; _temp2115.curr= _temp2114; _temp2115.base= _temp2114; _temp2115.last_plus_one=
_temp2114 + 2; _temp2115;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2028->sc), Cyc_PP_text(({ char* _temp2116="struct "; struct
_tagged_string _temp2117; _temp2117.curr= _temp2116; _temp2117.base= _temp2116;
_temp2117.last_plus_one= _temp2116 + 8; _temp2117;})), _temp2028->name == 0? Cyc_PP_text(({
char* _temp2118=""; struct _tagged_string _temp2119; _temp2119.curr= _temp2118;
_temp2119.base= _temp2118; _temp2119.last_plus_one= _temp2118 + 1; _temp2119;})):
Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp2028->name))->v), Cyc_Absynpp_ktvars2doc( _temp2028->tvs), Cyc_PP_text(({
char* _temp2120=" {"; struct _tagged_string _temp2121; _temp2121.curr= _temp2120;
_temp2121.base= _temp2120; _temp2121.last_plus_one= _temp2120 + 3; _temp2121;})),
Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2028->fields))->v))),
Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp2122="}"; struct _tagged_string
_temp2123; _temp2123.curr= _temp2122; _temp2123.base= _temp2122; _temp2123.last_plus_one=
_temp2122 + 2; _temp2123;})), Cyc_Absynpp_atts2doc( _temp2028->attributes), Cyc_PP_text(({
char* _temp2124=";"; struct _tagged_string _temp2125; _temp2125.curr= _temp2124;
_temp2125.base= _temp2124; _temp2125.last_plus_one= _temp2124 + 2; _temp2125;})));}
goto _LL2001; _LL2007: if( _temp2030->fields == 0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc((
void*) _temp2030->sc), Cyc_PP_text(({ char* _temp2126="union "; struct
_tagged_string _temp2127; _temp2127.curr= _temp2126; _temp2127.base= _temp2126;
_temp2127.last_plus_one= _temp2126 + 7; _temp2127;})), _temp2030->name == 0? Cyc_PP_text(({
char* _temp2128=""; struct _tagged_string _temp2129; _temp2129.curr= _temp2128;
_temp2129.base= _temp2128; _temp2129.last_plus_one= _temp2128 + 1; _temp2129;})):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp2030->name))->v), Cyc_Absynpp_tvars2doc( _temp2030->tvs), Cyc_PP_text(({
char* _temp2130=";"; struct _tagged_string _temp2131; _temp2131.curr= _temp2130;
_temp2131.base= _temp2130; _temp2131.last_plus_one= _temp2130 + 2; _temp2131;})));}
else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc(( void*) _temp2030->sc), Cyc_PP_text(({
char* _temp2132="union "; struct _tagged_string _temp2133; _temp2133.curr=
_temp2132; _temp2133.base= _temp2132; _temp2133.last_plus_one= _temp2132 + 7;
_temp2133;})), _temp2030->name == 0? Cyc_PP_text(({ char* _temp2134=""; struct
_tagged_string _temp2135; _temp2135.curr= _temp2134; _temp2135.base= _temp2134;
_temp2135.last_plus_one= _temp2134 + 1; _temp2135;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2030->name))->v), Cyc_Absynpp_tvars2doc(
_temp2030->tvs), Cyc_PP_text(({ char* _temp2136=" {"; struct _tagged_string
_temp2137; _temp2137.curr= _temp2136; _temp2137.base= _temp2136; _temp2137.last_plus_one=
_temp2136 + 3; _temp2137;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2030->fields))->v))), Cyc_PP_line_doc(), Cyc_PP_text(({ char*
_temp2138="}"; struct _tagged_string _temp2139; _temp2139.curr= _temp2138;
_temp2139.base= _temp2138; _temp2139.last_plus_one= _temp2138 + 2; _temp2139;})),
Cyc_Absynpp_atts2doc( _temp2030->attributes), Cyc_PP_text(({ char* _temp2140=";";
struct _tagged_string _temp2141; _temp2141.curr= _temp2140; _temp2141.base=
_temp2140; _temp2141.last_plus_one= _temp2140 + 2; _temp2141;})));} goto _LL2001;
_LL2009: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc( _temp2045); s=
Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc( _temp2047), Cyc_Absynpp_tqtd2doc(
_temp2043, _temp2041,({ struct Cyc_Core_Opt* _temp2142=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2142->v=( void*) sn; _temp2142;})),
Cyc_Absynpp_atts2doc( _temp2035), _temp2039 == 0? Cyc_PP_text(({ char* _temp2143="";
struct _tagged_string _temp2144; _temp2144.curr= _temp2143; _temp2144.base=
_temp2143; _temp2144.last_plus_one= _temp2143 + 1; _temp2144;})): Cyc_Absynpp_cat2(
Cyc_PP_text(({ char* _temp2145=" = "; struct _tagged_string _temp2146; _temp2146.curr=
_temp2145; _temp2146.base= _temp2145; _temp2146.last_plus_one= _temp2145 + 4;
_temp2146;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null(
_temp2039))), Cyc_PP_text(({ char* _temp2147=";"; struct _tagged_string
_temp2148; _temp2148.curr= _temp2147; _temp2148.base= _temp2147; _temp2148.last_plus_one=
_temp2147 + 2; _temp2148;}))); goto _LL2001;} _LL2011: if( _temp2054 == 0){ s=
Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc( _temp2060), _temp2052? Cyc_PP_text(({
char* _temp2149="xtunion "; struct _tagged_string _temp2150; _temp2150.curr=
_temp2149; _temp2150.base= _temp2149; _temp2150.last_plus_one= _temp2149 + 9;
_temp2150;})): Cyc_PP_text(({ char* _temp2151="tunion "; struct _tagged_string
_temp2152; _temp2152.curr= _temp2151; _temp2152.base= _temp2151; _temp2152.last_plus_one=
_temp2151 + 8; _temp2152;})), _temp2052? Cyc_Absynpp_qvar2doc( _temp2058): Cyc_Absynpp_typedef_name2doc(
_temp2058), Cyc_Absynpp_ktvars2doc( _temp2056), Cyc_PP_text(({ char* _temp2153=";";
struct _tagged_string _temp2154; _temp2154.curr= _temp2153; _temp2154.base=
_temp2153; _temp2154.last_plus_one= _temp2153 + 2; _temp2154;})));} else{ s= Cyc_Absynpp_cat8(
Cyc_Absynpp_scope2doc( _temp2060), _temp2052? Cyc_PP_text(({ char* _temp2155="xtunion ";
struct _tagged_string _temp2156; _temp2156.curr= _temp2155; _temp2156.base=
_temp2155; _temp2156.last_plus_one= _temp2155 + 9; _temp2156;})): Cyc_PP_text(({
char* _temp2157="tunion "; struct _tagged_string _temp2158; _temp2158.curr=
_temp2157; _temp2158.base= _temp2157; _temp2158.last_plus_one= _temp2157 + 8;
_temp2158;})), _temp2052? Cyc_Absynpp_qvar2doc( _temp2058): Cyc_Absynpp_typedef_name2doc(
_temp2058), Cyc_Absynpp_ktvars2doc( _temp2056), Cyc_PP_text(({ char* _temp2159=" {";
struct _tagged_string _temp2160; _temp2160.curr= _temp2159; _temp2160.base=
_temp2159; _temp2160.last_plus_one= _temp2159 + 3; _temp2160;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2054))->v))), Cyc_PP_line_doc(), Cyc_PP_text(({
char* _temp2161="};"; struct _tagged_string _temp2162; _temp2162.curr= _temp2161;
_temp2162.base= _temp2161; _temp2162.last_plus_one= _temp2161 + 3; _temp2162;})));}
goto _LL2001; _LL2013: s= Cyc_Absynpp_cat5( Cyc_PP_text(({ char* _temp2163="let ";
struct _tagged_string _temp2164; _temp2164.curr= _temp2163; _temp2164.base=
_temp2163; _temp2164.last_plus_one= _temp2163 + 5; _temp2164;})), Cyc_Absynpp_pat2doc(
_temp2070), Cyc_PP_text(({ char* _temp2165=" = "; struct _tagged_string
_temp2166; _temp2166.curr= _temp2165; _temp2166.base= _temp2165; _temp2166.last_plus_one=
_temp2165 + 4; _temp2166;})), Cyc_Absynpp_exp2doc( _temp2064), Cyc_PP_text(({
char* _temp2167=";"; struct _tagged_string _temp2168; _temp2168.curr= _temp2167;
_temp2168.base= _temp2167; _temp2168.last_plus_one= _temp2167 + 2; _temp2168;})));
goto _LL2001; _LL2015: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp2169="let ";
struct _tagged_string _temp2170; _temp2170.curr= _temp2169; _temp2170.base=
_temp2169; _temp2170.last_plus_one= _temp2169 + 5; _temp2170;})), Cyc_Absynpp_ids2doc(
_temp2072), Cyc_PP_text(({ char* _temp2171=";"; struct _tagged_string _temp2172;
_temp2172.curr= _temp2171; _temp2172.base= _temp2171; _temp2172.last_plus_one=
_temp2171 + 2; _temp2172;}))); goto _LL2001; _LL2017: if( _temp2077 == 0){ s=
Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc( _temp2081), Cyc_PP_text(({ char*
_temp2173="enum "; struct _tagged_string _temp2174; _temp2174.curr= _temp2173;
_temp2174.base= _temp2173; _temp2174.last_plus_one= _temp2173 + 6; _temp2174;})),
Cyc_Absynpp_typedef_name2doc( _temp2079), Cyc_PP_text(({ char* _temp2175=";";
struct _tagged_string _temp2176; _temp2176.curr= _temp2175; _temp2176.base=
_temp2175; _temp2176.last_plus_one= _temp2175 + 2; _temp2176;})));} else{ s= Cyc_Absynpp_cat7(
Cyc_Absynpp_scope2doc( _temp2081), Cyc_PP_text(({ char* _temp2177="enum ";
struct _tagged_string _temp2178; _temp2178.curr= _temp2177; _temp2178.base=
_temp2177; _temp2178.last_plus_one= _temp2177 + 6; _temp2178;})), Cyc_Absynpp_qvar2doc(
_temp2079), Cyc_PP_text(({ char* _temp2179=" {"; struct _tagged_string _temp2180;
_temp2180.curr= _temp2179; _temp2180.base= _temp2179; _temp2180.last_plus_one=
_temp2179 + 3; _temp2180;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2077))->v))), Cyc_PP_line_doc(), Cyc_PP_text(({ char*
_temp2181="};"; struct _tagged_string _temp2182; _temp2182.curr= _temp2181;
_temp2182.base= _temp2181; _temp2182.last_plus_one= _temp2181 + 3; _temp2182;})));}
goto _LL2001; _LL2019: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp2183="typedef ";
struct _tagged_string _temp2184; _temp2184.curr= _temp2183; _temp2184.base=
_temp2183; _temp2184.last_plus_one= _temp2183 + 9; _temp2184;})), Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual _temp2185; _temp2185.q_const= 0; _temp2185.q_volatile= 0;
_temp2185.q_restrict= 0; _temp2185;}),( void*) _temp2083->defn,({ struct Cyc_Core_Opt*
_temp2186=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2186->v=( void*) Cyc_Absynpp_cat2( Cyc_Absynpp_typedef_name2doc( _temp2083->name),
Cyc_Absynpp_tvars2doc( _temp2083->tvs)); _temp2186;})), Cyc_PP_text(({ char*
_temp2187=";"; struct _tagged_string _temp2188; _temp2188.curr= _temp2187;
_temp2188.base= _temp2187; _temp2188.last_plus_one= _temp2187 + 2; _temp2188;})));
goto _LL2001; _LL2021: if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_add(
_temp2087);} s= Cyc_Absynpp_cat7( Cyc_PP_text(({ char* _temp2189="namespace ";
struct _tagged_string _temp2190; _temp2190.curr= _temp2189; _temp2190.base=
_temp2189; _temp2190.last_plus_one= _temp2189 + 11; _temp2190;})), Cyc_Absynpp_textptr(
_temp2087), Cyc_PP_text(({ char* _temp2191=" {"; struct _tagged_string _temp2192;
_temp2192.curr= _temp2191; _temp2192.base= _temp2191; _temp2192.last_plus_one=
_temp2191 + 3; _temp2192;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ char* _temp2193="";
struct _tagged_string _temp2194; _temp2194.curr= _temp2193; _temp2194.base=
_temp2193; _temp2194.last_plus_one= _temp2193 + 1; _temp2194;}), _temp2085), Cyc_PP_line_doc(),
Cyc_PP_text(({ char* _temp2195="}"; struct _tagged_string _temp2196; _temp2196.curr=
_temp2195; _temp2196.base= _temp2195; _temp2196.last_plus_one= _temp2195 + 2;
_temp2196;}))); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2001; _LL2023: if( Cyc_Absynpp_print_using_stmts){ s= Cyc_Absynpp_cat7(
Cyc_PP_text(({ char* _temp2197="using "; struct _tagged_string _temp2198;
_temp2198.curr= _temp2197; _temp2198.base= _temp2197; _temp2198.last_plus_one=
_temp2197 + 7; _temp2198;})), Cyc_Absynpp_qvar2doc( _temp2091), Cyc_PP_text(({
char* _temp2199=" {"; struct _tagged_string _temp2200; _temp2200.curr= _temp2199;
_temp2200.base= _temp2199; _temp2200.last_plus_one= _temp2199 + 3; _temp2200;})),
Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({
char* _temp2201=""; struct _tagged_string _temp2202; _temp2202.curr= _temp2201;
_temp2202.base= _temp2201; _temp2202.last_plus_one= _temp2201 + 1; _temp2202;}),
_temp2089), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp2203="}"; struct
_tagged_string _temp2204; _temp2204.curr= _temp2203; _temp2204.base= _temp2203;
_temp2204.last_plus_one= _temp2203 + 2; _temp2204;})));} else{ s= Cyc_Absynpp_cat7(
Cyc_PP_text(({ char* _temp2205="/* using "; struct _tagged_string _temp2206;
_temp2206.curr= _temp2205; _temp2206.base= _temp2205; _temp2206.last_plus_one=
_temp2205 + 10; _temp2206;})), Cyc_Absynpp_qvar2doc( _temp2091), Cyc_PP_text(({
char* _temp2207=" { */"; struct _tagged_string _temp2208; _temp2208.curr=
_temp2207; _temp2208.base= _temp2207; _temp2208.last_plus_one= _temp2207 + 6;
_temp2208;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ char* _temp2209=""; struct
_tagged_string _temp2210; _temp2210.curr= _temp2209; _temp2210.base= _temp2209;
_temp2210.last_plus_one= _temp2209 + 1; _temp2210;}), _temp2089), Cyc_PP_line_doc(),
Cyc_PP_text(({ char* _temp2211="/* } */"; struct _tagged_string _temp2212;
_temp2212.curr= _temp2211; _temp2212.base= _temp2211; _temp2212.last_plus_one=
_temp2211 + 8; _temp2212;})));} goto _LL2001; _LL2025: if( Cyc_Absynpp_print_externC_stmts){
s= Cyc_Absynpp_cat5( Cyc_PP_text(({ char* _temp2213="extern \"C\" {"; struct
_tagged_string _temp2214; _temp2214.curr= _temp2213; _temp2214.base= _temp2213;
_temp2214.last_plus_one= _temp2213 + 13; _temp2214;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({
char* _temp2215=""; struct _tagged_string _temp2216; _temp2216.curr= _temp2215;
_temp2216.base= _temp2215; _temp2216.last_plus_one= _temp2215 + 1; _temp2216;}),
_temp2093), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp2217="}"; struct
_tagged_string _temp2218; _temp2218.curr= _temp2217; _temp2218.base= _temp2217;
_temp2218.last_plus_one= _temp2217 + 2; _temp2218;})));} else{ s= Cyc_Absynpp_cat5(
Cyc_PP_text(({ char* _temp2219="/* extern \"C\" { */"; struct _tagged_string
_temp2220; _temp2220.curr= _temp2219; _temp2220.base= _temp2219; _temp2220.last_plus_one=
_temp2219 + 19; _temp2220;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ char*
_temp2221=""; struct _tagged_string _temp2222; _temp2222.curr= _temp2221;
_temp2222.base= _temp2221; _temp2222.last_plus_one= _temp2221 + 1; _temp2222;}),
_temp2093), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp2223="/* } */"; struct
_tagged_string _temp2224; _temp2224.curr= _temp2223; _temp2224.base= _temp2223;
_temp2224.last_plus_one= _temp2223 + 8; _temp2224;})));} goto _LL2001; _LL2001:;}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void* sc){ void* _temp2225=
sc; _LL2227: if( _temp2225 ==( void*) Cyc_Absyn_Static){ goto _LL2228;} else{
goto _LL2229;} _LL2229: if( _temp2225 ==( void*) Cyc_Absyn_Public){ goto _LL2230;}
else{ goto _LL2231;} _LL2231: if( _temp2225 ==( void*) Cyc_Absyn_Extern){ goto
_LL2232;} else{ goto _LL2233;} _LL2233: if( _temp2225 ==( void*) Cyc_Absyn_ExternC){
goto _LL2234;} else{ goto _LL2235;} _LL2235: if( _temp2225 ==( void*) Cyc_Absyn_Abstract){
goto _LL2236;} else{ goto _LL2226;} _LL2228: return Cyc_PP_text(({ char*
_temp2237="static "; struct _tagged_string _temp2238; _temp2238.curr= _temp2237;
_temp2238.base= _temp2237; _temp2238.last_plus_one= _temp2237 + 8; _temp2238;}));
_LL2230: return Cyc_PP_nil_doc(); _LL2232: return Cyc_PP_text(({ char* _temp2239="extern ";
struct _tagged_string _temp2240; _temp2240.curr= _temp2239; _temp2240.base=
_temp2239; _temp2240.last_plus_one= _temp2239 + 8; _temp2240;})); _LL2234:
return Cyc_PP_text(({ char* _temp2241="extern \"C\" "; struct _tagged_string
_temp2242; _temp2242.curr= _temp2241; _temp2242.base= _temp2241; _temp2242.last_plus_one=
_temp2241 + 12; _temp2242;})); _LL2236: return Cyc_PP_text(({ char* _temp2243="abstract ";
struct _tagged_string _temp2244; _temp2244.curr= _temp2243; _temp2244.base=
_temp2243; _temp2244.last_plus_one= _temp2243 + 10; _temp2244;})); _LL2226:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp2245= t; struct
Cyc_Absyn_Tvar* _temp2253; struct Cyc_List_List* _temp2255; _LL2247: if((
unsigned int) _temp2245 > 4u?*(( int*) _temp2245) == Cyc_Absyn_VarType: 0){
_LL2254: _temp2253=(( struct Cyc_Absyn_VarType_struct*) _temp2245)->f1; goto
_LL2248;} else{ goto _LL2249;} _LL2249: if(( unsigned int) _temp2245 > 4u?*((
int*) _temp2245) == Cyc_Absyn_JoinEff: 0){ _LL2256: _temp2255=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2245)->f1; goto _LL2250;} else{ goto _LL2251;} _LL2251: goto _LL2252;
_LL2248: return Cyc_Tcutil_is_temp_tvar( _temp2253); _LL2250: return(( int(*)(
int(* pred)( void*), struct Cyc_List_List* x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect,
_temp2255); _LL2252: return 0; _LL2246:;} struct _tuple4 Cyc_Absynpp_to_tms(
struct Cyc_Absyn_Tqual tq, void* t){ void* _temp2257= t; struct Cyc_Absyn_Exp*
_temp2271; struct Cyc_Absyn_Tqual _temp2273; void* _temp2275; struct Cyc_Absyn_PtrInfo
_temp2277; struct Cyc_Absyn_Conref* _temp2279; struct Cyc_Absyn_Tqual _temp2281;
struct Cyc_Absyn_Conref* _temp2283; void* _temp2285; void* _temp2287; struct Cyc_Absyn_FnInfo
_temp2289; struct Cyc_List_List* _temp2291; struct Cyc_Absyn_VarargInfo*
_temp2293; int _temp2295; struct Cyc_List_List* _temp2297; void* _temp2299;
struct Cyc_Core_Opt* _temp2301; struct Cyc_List_List* _temp2303; int _temp2305;
struct Cyc_Core_Opt* _temp2307; void* _temp2309; struct Cyc_Core_Opt* _temp2311;
struct Cyc_List_List* _temp2313; struct _tuple0* _temp2315; _LL2259: if((
unsigned int) _temp2257 > 4u?*(( int*) _temp2257) == Cyc_Absyn_ArrayType: 0){
_LL2276: _temp2275=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2257)->f1;
goto _LL2274; _LL2274: _temp2273=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2257)->f2; goto _LL2272; _LL2272: _temp2271=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2257)->f3; goto _LL2260;} else{ goto _LL2261;} _LL2261: if(( unsigned int)
_temp2257 > 4u?*(( int*) _temp2257) == Cyc_Absyn_PointerType: 0){ _LL2278:
_temp2277=(( struct Cyc_Absyn_PointerType_struct*) _temp2257)->f1; _LL2288:
_temp2287=( void*) _temp2277.elt_typ; goto _LL2286; _LL2286: _temp2285=( void*)
_temp2277.rgn_typ; goto _LL2284; _LL2284: _temp2283= _temp2277.nullable; goto
_LL2282; _LL2282: _temp2281= _temp2277.tq; goto _LL2280; _LL2280: _temp2279=
_temp2277.bounds; goto _LL2262;} else{ goto _LL2263;} _LL2263: if(( unsigned int)
_temp2257 > 4u?*(( int*) _temp2257) == Cyc_Absyn_FnType: 0){ _LL2290: _temp2289=((
struct Cyc_Absyn_FnType_struct*) _temp2257)->f1; _LL2304: _temp2303= _temp2289.tvars;
goto _LL2302; _LL2302: _temp2301= _temp2289.effect; goto _LL2300; _LL2300:
_temp2299=( void*) _temp2289.ret_typ; goto _LL2298; _LL2298: _temp2297=
_temp2289.args; goto _LL2296; _LL2296: _temp2295= _temp2289.c_varargs; goto
_LL2294; _LL2294: _temp2293= _temp2289.cyc_varargs; goto _LL2292; _LL2292:
_temp2291= _temp2289.attributes; goto _LL2264;} else{ goto _LL2265;} _LL2265:
if(( unsigned int) _temp2257 > 4u?*(( int*) _temp2257) == Cyc_Absyn_Evar: 0){
_LL2310: _temp2309=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp2257)->f1;
goto _LL2308; _LL2308: _temp2307=(( struct Cyc_Absyn_Evar_struct*) _temp2257)->f2;
goto _LL2306; _LL2306: _temp2305=(( struct Cyc_Absyn_Evar_struct*) _temp2257)->f3;
goto _LL2266;} else{ goto _LL2267;} _LL2267: if(( unsigned int) _temp2257 > 4u?*((
int*) _temp2257) == Cyc_Absyn_TypedefType: 0){ _LL2316: _temp2315=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2257)->f1; goto _LL2314; _LL2314: _temp2313=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2257)->f2; goto _LL2312; _LL2312: _temp2311=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2257)->f3; goto _LL2268;} else{ goto _LL2269;} _LL2269: goto _LL2270;
_LL2260: { struct Cyc_List_List* _temp2319; void* _temp2321; struct Cyc_Absyn_Tqual
_temp2323; struct _tuple4 _temp2317= Cyc_Absynpp_to_tms( _temp2273, _temp2275);
_LL2324: _temp2323= _temp2317.f1; goto _LL2322; _LL2322: _temp2321= _temp2317.f2;
goto _LL2320; _LL2320: _temp2319= _temp2317.f3; goto _LL2318; _LL2318: { void*
tm; if( _temp2271 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2325=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2325[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2326; _temp2326.tag= Cyc_Absyn_ConstArray_mod;
_temp2326.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp2271); _temp2326;});
_temp2325;});} return({ struct _tuple4 _temp2327; _temp2327.f1= _temp2323;
_temp2327.f2= _temp2321; _temp2327.f3=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp2328=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2328->hd=( void*) tm; _temp2328->tl= _temp2319; _temp2328;}); _temp2327;});}}
_LL2262: { struct Cyc_List_List* _temp2331; void* _temp2333; struct Cyc_Absyn_Tqual
_temp2335; struct _tuple4 _temp2329= Cyc_Absynpp_to_tms( _temp2281, _temp2287);
_LL2336: _temp2335= _temp2329.f1; goto _LL2334; _LL2334: _temp2333= _temp2329.f2;
goto _LL2332; _LL2332: _temp2331= _temp2329.f3; goto _LL2330; _LL2330: { void*
ps;{ struct _tuple8 _temp2338=({ struct _tuple8 _temp2337; _temp2337.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2283))->v; _temp2337.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2279))->v; _temp2337;}); void* _temp2348;
void* _temp2350; struct Cyc_Absyn_Exp* _temp2352; void* _temp2354; int _temp2356;
void* _temp2358; void* _temp2360; struct Cyc_Absyn_Exp* _temp2362; void*
_temp2364; int _temp2366; void* _temp2368; void* _temp2370; void* _temp2372;
_LL2340: _LL2355: _temp2354= _temp2338.f1; if(( unsigned int) _temp2354 > 1u?*((
int*) _temp2354) == Cyc_Absyn_Eq_constr: 0){ _LL2357: _temp2356=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp2354)->f1; if( _temp2356 == 1){ goto _LL2349;}
else{ goto _LL2342;}} else{ goto _LL2342;} _LL2349: _temp2348= _temp2338.f2; if((
unsigned int) _temp2348 > 1u?*(( int*) _temp2348) == Cyc_Absyn_Eq_constr: 0){
_LL2351: _temp2350=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2348)->f1;
if(( unsigned int) _temp2350 > 1u?*(( int*) _temp2350) == Cyc_Absyn_Upper_b: 0){
_LL2353: _temp2352=(( struct Cyc_Absyn_Upper_b_struct*) _temp2350)->f1; goto
_LL2341;} else{ goto _LL2342;}} else{ goto _LL2342;} _LL2342: _LL2365: _temp2364=
_temp2338.f1; if(( unsigned int) _temp2364 > 1u?*(( int*) _temp2364) == Cyc_Absyn_Eq_constr:
0){ _LL2367: _temp2366=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2364)->f1;
if( _temp2366 == 0){ goto _LL2359;} else{ goto _LL2344;}} else{ goto _LL2344;}
_LL2359: _temp2358= _temp2338.f2; if(( unsigned int) _temp2358 > 1u?*(( int*)
_temp2358) == Cyc_Absyn_Eq_constr: 0){ _LL2361: _temp2360=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2358)->f1; if(( unsigned int) _temp2360 > 1u?*(( int*) _temp2360) == Cyc_Absyn_Upper_b:
0){ _LL2363: _temp2362=(( struct Cyc_Absyn_Upper_b_struct*) _temp2360)->f1; goto
_LL2343;} else{ goto _LL2344;}} else{ goto _LL2344;} _LL2344: _LL2373: _temp2372=
_temp2338.f1; goto _LL2369; _LL2369: _temp2368= _temp2338.f2; if(( unsigned int)
_temp2368 > 1u?*(( int*) _temp2368) == Cyc_Absyn_Eq_constr: 0){ _LL2371:
_temp2370=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2368)->f1; if(
_temp2370 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2345;} else{ goto _LL2346;}}
else{ goto _LL2346;} _LL2346: goto _LL2347; _LL2341: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2374=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2374[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2375; _temp2375.tag=
Cyc_Absyn_Nullable_ps; _temp2375.f1= _temp2352; _temp2375;}); _temp2374;}); goto
_LL2339; _LL2343: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2376=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2376[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2377; _temp2377.tag= Cyc_Absyn_NonNullable_ps; _temp2377.f1= _temp2362;
_temp2377;}); _temp2376;}); goto _LL2339; _LL2345: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2339; _LL2347: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2339;
_LL2339:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2380=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2380[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2381; _temp2381.tag=
Cyc_Absyn_Pointer_mod; _temp2381.f1=( void*) ps; _temp2381.f2=( void*) _temp2285;
_temp2381.f3= tq; _temp2381;}); _temp2380;}); return({ struct _tuple4 _temp2378;
_temp2378.f1= _temp2335; _temp2378.f2= _temp2333; _temp2378.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2379=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2379->hd=( void*) tm; _temp2379->tl= _temp2331;
_temp2379;}); _temp2378;});}}} _LL2264: if( ! Cyc_Absynpp_print_all_tvars){ if(
_temp2301 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2301))->v)){ _temp2301= 0; _temp2303= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2303);}}{ struct Cyc_List_List* _temp2384;
void* _temp2386; struct Cyc_Absyn_Tqual _temp2388; struct _tuple4 _temp2382= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp2299); _LL2389: _temp2388= _temp2382.f1; goto
_LL2387; _LL2387: _temp2386= _temp2382.f2; goto _LL2385; _LL2385: _temp2384=
_temp2382.f3; goto _LL2383; _LL2383: { struct Cyc_List_List* tms= _temp2384; if(
_temp2291 != 0){ tms=({ struct Cyc_List_List* _temp2390=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2390->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2391=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2391[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2392; _temp2392.tag= Cyc_Absyn_Attributes_mod;
_temp2392.f1= 0; _temp2392.f2= _temp2291; _temp2392;}); _temp2391;})); _temp2390->tl=
tms; _temp2390;});} tms=({ struct Cyc_List_List* _temp2393=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2393->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2394=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2394[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2395; _temp2395.tag= Cyc_Absyn_Function_mod;
_temp2395.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2396=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2396[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2397; _temp2397.tag= Cyc_Absyn_WithTypes;
_temp2397.f1= _temp2297; _temp2397.f2= _temp2295; _temp2397.f3= _temp2293;
_temp2397.f4= _temp2301; _temp2397;}); _temp2396;})); _temp2395;}); _temp2394;}));
_temp2393->tl= tms; _temp2393;}); if( _temp2303 != 0){ tms=({ struct Cyc_List_List*
_temp2398=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2398->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp2399=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp2399[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp2400; _temp2400.tag= Cyc_Absyn_TypeParams_mod; _temp2400.f1= _temp2303;
_temp2400.f2= 0; _temp2400.f3= 1; _temp2400;}); _temp2399;})); _temp2398->tl=
tms; _temp2398;});} return({ struct _tuple4 _temp2401; _temp2401.f1= _temp2388;
_temp2401.f2= _temp2386; _temp2401.f3= tms; _temp2401;});}} _LL2266: if(
_temp2307 == 0){ return({ struct _tuple4 _temp2402; _temp2402.f1= tq; _temp2402.f2=
t; _temp2402.f3= 0; _temp2402;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2307))->v);} _LL2268: if( _temp2311 == 0?
1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp2403; _temp2403.f1=
tq; _temp2403.f2= t; _temp2403.f3= 0; _temp2403;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2311))->v);} _LL2270:
return({ struct _tuple4 _temp2404; _temp2404.f1= tq; _temp2404.f2= t; _temp2404.f3=
0; _temp2404;}); _LL2258:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp2407; void*
_temp2409; struct Cyc_Absyn_Tqual _temp2411; struct _tuple4 _temp2405= Cyc_Absynpp_to_tms(
tq, t); _LL2412: _temp2411= _temp2405.f1; goto _LL2410; _LL2410: _temp2409=
_temp2405.f2; goto _LL2408; _LL2408: _temp2407= _temp2405.f3; goto _LL2406;
_LL2406: _temp2407=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2407); if( _temp2407 == 0? dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc(
_temp2411), Cyc_Absynpp_ntyp2doc( _temp2409));} else{ return Cyc_Absynpp_cat4(
Cyc_Absynpp_tqual2doc( _temp2411), Cyc_Absynpp_ntyp2doc( _temp2409), Cyc_PP_text(({
char* _temp2413=" "; struct _tagged_string _temp2414; _temp2414.curr= _temp2413;
_temp2414.base= _temp2413; _temp2414.last_plus_one= _temp2413 + 2; _temp2414;})),
Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)(( struct
Cyc_Core_Opt*) _check_null( dopt))->v, _temp2407));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return Cyc_Absynpp_cat5(
Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2415=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2415->v=(
void*) Cyc_Absynpp_textptr( f->name); _temp2415;})), Cyc_PP_text(({ char*
_temp2416=":"; struct _tagged_string _temp2417; _temp2417.curr= _temp2416;
_temp2417.base= _temp2416; _temp2417.last_plus_one= _temp2416 + 2; _temp2417;})),
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null(
f->width))->v), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text(({ char*
_temp2418=";"; struct _tagged_string _temp2419; _temp2419.curr= _temp2418;
_temp2419.base= _temp2418; _temp2419.last_plus_one= _temp2418 + 2; _temp2419;})));}
else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp2420=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2420->v=( void*) Cyc_Absynpp_textptr( f->name); _temp2420;})),
Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text(({ char* _temp2421=";"; struct
_tagged_string _temp2422; _temp2422.curr= _temp2421; _temp2422.base= _temp2421;
_temp2422.last_plus_one= _temp2421 + 2; _temp2422;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,({ char* _temp2423=""; struct
_tagged_string _temp2424; _temp2424.curr= _temp2423; _temp2424.base= _temp2423;
_temp2424.last_plus_one= _temp2423 + 1; _temp2424;}), fields);} struct Cyc_PP_Doc*
Cyc_Absynpp_tunionfield2doc( struct Cyc_Absyn_Tunionfield* f){ return Cyc_Absynpp_cat4(
Cyc_Absynpp_scope2doc(( void*) f->sc), Cyc_Absynpp_typedef_name2doc( f->name), f->tvs
== 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs), f->typs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,({ char* _temp2425=","; struct
_tagged_string _temp2426; _temp2426.curr= _temp2425; _temp2426.base= _temp2425;
_temp2426.last_plus_one= _temp2425 + 2; _temp2426;}), fields);} void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl=((
struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd), 72, f);
fprintf( f,"\n");}} struct _tagged_string Cyc_Absynpp_decllist2string( struct
Cyc_List_List* tdl){ return Cyc_PP_string_of_doc( Cyc_PP_seql(({ char* _temp2427="";
struct _tagged_string _temp2428; _temp2428.curr= _temp2427; _temp2428.base=
_temp2427; _temp2428.last_plus_one= _temp2427 + 1; _temp2428;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_decl2doc, tdl)), 72);} struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp* e){ return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e),
72);} struct _tagged_string Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){
return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s), 72);} struct
_tagged_string Cyc_Absynpp_typ2string( void* t){ Cyc_Absynpp_dont_rewrite_temp_tvars=
1;{ struct _tagged_string _temp2429= Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc(
t), 72); goto _LL2430; _LL2430: Cyc_Absynpp_dont_rewrite_temp_tvars= 0; return
_temp2429;}} struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0* v){
return Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);} struct
_tagged_string Cyc_Absynpp_prim2string( void* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc( p), 72);} struct _tagged_string Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_pat2doc( p),
72);} struct _tagged_string Cyc_Absynpp_scope2string( void* sc){ void* _temp2431=
sc; _LL2433: if( _temp2431 ==( void*) Cyc_Absyn_Static){ goto _LL2434;} else{
goto _LL2435;} _LL2435: if( _temp2431 ==( void*) Cyc_Absyn_Public){ goto _LL2436;}
else{ goto _LL2437;} _LL2437: if( _temp2431 ==( void*) Cyc_Absyn_Extern){ goto
_LL2438;} else{ goto _LL2439;} _LL2439: if( _temp2431 ==( void*) Cyc_Absyn_ExternC){
goto _LL2440;} else{ goto _LL2441;} _LL2441: if( _temp2431 ==( void*) Cyc_Absyn_Abstract){
goto _LL2442;} else{ goto _LL2432;} _LL2434: return({ char* _temp2443="static";
struct _tagged_string _temp2444; _temp2444.curr= _temp2443; _temp2444.base=
_temp2443; _temp2444.last_plus_one= _temp2443 + 7; _temp2444;}); _LL2436: return({
char* _temp2445="public"; struct _tagged_string _temp2446; _temp2446.curr=
_temp2445; _temp2446.base= _temp2445; _temp2446.last_plus_one= _temp2445 + 7;
_temp2446;}); _LL2438: return({ char* _temp2447="extern"; struct _tagged_string
_temp2448; _temp2448.curr= _temp2447; _temp2448.base= _temp2447; _temp2448.last_plus_one=
_temp2447 + 7; _temp2448;}); _LL2440: return({ char* _temp2449="extern \"C\"";
struct _tagged_string _temp2450; _temp2450.curr= _temp2449; _temp2450.base=
_temp2449; _temp2450.last_plus_one= _temp2449 + 11; _temp2450;}); _LL2442:
return({ char* _temp2451="abstract"; struct _tagged_string _temp2452; _temp2452.curr=
_temp2451; _temp2452.base= _temp2451; _temp2452.last_plus_one= _temp2451 + 9;
_temp2452;}); _LL2432:;}