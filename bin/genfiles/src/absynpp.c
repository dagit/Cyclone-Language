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
struct Cyc_PP_Doc* d, int w); extern struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern
struct Cyc_PP_Doc* Cyc_PP_line_doc(); extern struct Cyc_PP_Doc* Cyc_PP_text(
struct _tagged_string s); extern struct Cyc_PP_Doc* Cyc_PP_textptr( struct
_tagged_string* s); extern struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc*
d); struct _tagged_ptr0{ struct Cyc_PP_Doc** curr; struct Cyc_PP_Doc** base;
struct Cyc_PP_Doc** last_plus_one; } ; extern struct Cyc_PP_Doc* Cyc_PP_cat(
struct _tagged_ptr0); extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l0); extern struct
Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_seql(
struct _tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc*
Cyc_PP_group( struct _tagged_string start, struct _tagged_string stop, struct
_tagged_string sep, struct Cyc_List_List* ss); extern struct Cyc_PP_Doc* Cyc_PP_egroup(
struct _tagged_string start, struct _tagged_string stop, struct _tagged_string
sep, struct Cyc_List_List* ss); struct Cyc_Lineno_Pos{ struct _tagged_string
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
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t); struct _tuple5{
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
_temp60 & 7));} goto _LL61; _LL61:;}} return t;}}}} static char _temp85[ 9u]="restrict";
static struct _tagged_string Cyc_Absynpp_restrict_string={ _temp85, _temp85,
_temp85 + 9u}; static char _temp88[ 9u]="volatile"; static struct _tagged_string
Cyc_Absynpp_volatile_string={ _temp88, _temp88, _temp88 + 9u}; static char
_temp91[ 6u]="const"; static struct _tagged_string Cyc_Absynpp_const_string={
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
void*) Cyc_Absynpp_const_sp; _temp94->tl= l; _temp94;});} return Cyc_PP_egroup(({
char* _temp95=""; struct _tagged_string _temp96; _temp96.curr= _temp95; _temp96.base=
_temp95; _temp96.last_plus_one= _temp95 + 1; _temp96;}),({ char* _temp97=" ";
struct _tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97;
_temp98.last_plus_one= _temp97 + 2; _temp98;}),({ char* _temp99=" "; struct
_tagged_string _temp100; _temp100.curr= _temp99; _temp100.base= _temp99;
_temp100.last_plus_one= _temp99 + 2; _temp100;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_PP_textptr, l));} struct _tagged_string Cyc_Absynpp_kind2string( void* k){
void* _temp101= k; _LL103: if( _temp101 ==( void*) Cyc_Absyn_AnyKind){ goto
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
c));} struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc( struct Cyc_List_List* ts){ return
Cyc_PP_egroup(({ char* _temp133="<"; struct _tagged_string _temp134; _temp134.curr=
_temp133; _temp134.base= _temp133; _temp134.last_plus_one= _temp133 + 2;
_temp134;}),({ char* _temp135=">"; struct _tagged_string _temp136; _temp136.curr=
_temp135; _temp136.base= _temp135; _temp136.last_plus_one= _temp135 + 2;
_temp136;}),({ char* _temp137=","; struct _tagged_string _temp138; _temp138.curr=
_temp137; _temp138.base= _temp137; _temp138.last_plus_one= _temp137 + 2;
_temp138;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc, ts));} struct Cyc_PP_Doc*
Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){ void* _temp139=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind))->v; void* _temp147; void* _temp149; _LL141: if(( unsigned int)
_temp139 > 1u?*(( int*) _temp139) == Cyc_Absyn_Eq_constr: 0){ _LL148: _temp147=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp139)->f1; if( _temp147 ==(
void*) Cyc_Absyn_BoxKind){ goto _LL142;} else{ goto _LL143;}} else{ goto _LL143;}
_LL143: if(( unsigned int) _temp139 > 1u?*(( int*) _temp139) == Cyc_Absyn_Eq_constr:
0){ _LL150: _temp149=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp139)->f1;
goto _LL144;} else{ goto _LL145;} _LL145: goto _LL146; _LL142: return Cyc_PP_textptr(
tv->name); _LL144: return({ struct Cyc_PP_Doc*(* _temp151)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp154= Cyc_PP_textptr( tv->name); struct Cyc_PP_Doc*
_temp155= Cyc_PP_text(({ char* _temp157="::"; struct _tagged_string _temp158;
_temp158.curr= _temp157; _temp158.base= _temp157; _temp158.last_plus_one=
_temp157 + 3; _temp158;})); struct Cyc_PP_Doc* _temp156= Cyc_Absynpp_kind2doc(
_temp149); struct Cyc_PP_Doc* _temp152[ 3u]={ _temp154, _temp155, _temp156};
struct _tagged_ptr0 _temp153={ _temp152, _temp152, _temp152 + 3u}; _temp151(
_temp153);}); _LL146: return({ struct Cyc_PP_Doc*(* _temp159)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp162= Cyc_PP_textptr( tv->name);
struct Cyc_PP_Doc* _temp163= Cyc_PP_text(({ char* _temp164="/*::?*/"; struct
_tagged_string _temp165; _temp165.curr= _temp164; _temp165.base= _temp164;
_temp165.last_plus_one= _temp164 + 8; _temp165;})); struct Cyc_PP_Doc* _temp160[
2u]={ _temp162, _temp163}; struct _tagged_ptr0 _temp161={ _temp160, _temp160,
_temp160 + 2u}; _temp159( _temp161);}); _LL140:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List* tvs){ return Cyc_PP_egroup(({ char* _temp166="<"; struct
_tagged_string _temp167; _temp167.curr= _temp166; _temp167.base= _temp166;
_temp167.last_plus_one= _temp166 + 2; _temp167;}),({ char* _temp168=">"; struct
_tagged_string _temp169; _temp169.curr= _temp168; _temp169.base= _temp168;
_temp169.last_plus_one= _temp168 + 2; _temp169;}),({ char* _temp170=","; struct
_tagged_string _temp171; _temp171.curr= _temp170; _temp171.base= _temp170;
_temp171.last_plus_one= _temp170 + 2; _temp171;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_string* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc(
tvs);} return Cyc_PP_egroup(({ char* _temp172="<"; struct _tagged_string
_temp173; _temp173.curr= _temp172; _temp173.base= _temp172; _temp173.last_plus_one=
_temp172 + 2; _temp173;}),({ char* _temp174=">"; struct _tagged_string _temp175;
_temp175.curr= _temp174; _temp175.base= _temp174; _temp175.last_plus_one=
_temp174 + 2; _temp175;}),({ char* _temp176=","; struct _tagged_string _temp177;
_temp177.curr= _temp176; _temp177.base= _temp176; _temp177.last_plus_one=
_temp176 + 2; _temp177;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_PP_textptr,((
struct Cyc_List_List*(*)( struct _tagged_string*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_get_name, tvs)));} struct
_tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc(
struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((* t).f1,(* t).f2, 0);} struct
Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List* ts){ return Cyc_PP_group(({
char* _temp178="("; struct _tagged_string _temp179; _temp179.curr= _temp178;
_temp179.base= _temp178; _temp179.last_plus_one= _temp178 + 2; _temp179;}),({
char* _temp180=")"; struct _tagged_string _temp181; _temp181.curr= _temp180;
_temp181.base= _temp180; _temp181.last_plus_one= _temp180 + 2; _temp181;}),({
char* _temp182=","; struct _tagged_string _temp183; _temp183.curr= _temp182;
_temp183.base= _temp182; _temp183.last_plus_one= _temp182 + 2; _temp183;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc(
void* a){ return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if( atts == 0){ return Cyc_PP_nil_doc();}
return({ struct Cyc_PP_Doc*(* _temp184)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp187= Cyc_PP_text(({ char* _temp195=" __attribute__";
struct _tagged_string _temp196; _temp196.curr= _temp195; _temp196.base= _temp195;
_temp196.last_plus_one= _temp195 + 15; _temp196;})); struct Cyc_PP_Doc* _temp188=
Cyc_PP_group(({ char* _temp189="(("; struct _tagged_string _temp190; _temp190.curr=
_temp189; _temp190.base= _temp189; _temp190.last_plus_one= _temp189 + 3;
_temp190;}),({ char* _temp191="))"; struct _tagged_string _temp192; _temp192.curr=
_temp191; _temp192.base= _temp191; _temp192.last_plus_one= _temp191 + 3;
_temp192;}),({ char* _temp193=","; struct _tagged_string _temp194; _temp194.curr=
_temp193; _temp194.base= _temp193; _temp194.last_plus_one= _temp193 + 2;
_temp194;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc, atts)); struct Cyc_PP_Doc*
_temp185[ 2u]={ _temp187, _temp188}; struct _tagged_ptr0 _temp186={ _temp185,
_temp185, _temp185 + 2u}; _temp184( _temp186);});} int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List* tms){ if( tms == 0){ return 0;}{ void* _temp197=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp203;
void* _temp205; void* _temp207; _LL199: if(( unsigned int) _temp197 > 1u?*(( int*)
_temp197) == Cyc_Absyn_Pointer_mod: 0){ _LL208: _temp207=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp197)->f1; goto _LL206; _LL206: _temp205=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp197)->f2; goto _LL204; _LL204: _temp203=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp197)->f3; goto _LL200;} else{ goto _LL201;} _LL201: goto _LL202; _LL200:
return 1; _LL202: return 0; _LL198:;}} extern struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc(
void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc( struct Cyc_PP_Doc* d, struct
Cyc_List_List* tms){ if( tms == 0){ return d;}{ struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc(
d,(( struct Cyc_List_List*) _check_null( tms))->tl); struct Cyc_PP_Doc* p_rest=({
struct Cyc_PP_Doc*(* _temp374)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp377= Cyc_PP_text(({ char* _temp382="("; struct _tagged_string _temp383;
_temp383.curr= _temp382; _temp383.base= _temp382; _temp383.last_plus_one=
_temp382 + 2; _temp383;})); struct Cyc_PP_Doc* _temp378= rest; struct Cyc_PP_Doc*
_temp379= Cyc_PP_text(({ char* _temp380=")"; struct _tagged_string _temp381;
_temp381.curr= _temp380; _temp381.base= _temp380; _temp381.last_plus_one=
_temp380 + 2; _temp381;})); struct Cyc_PP_Doc* _temp375[ 3u]={ _temp377,
_temp378, _temp379}; struct _tagged_ptr0 _temp376={ _temp375, _temp375, _temp375
+ 3u}; _temp374( _temp376);}); void* _temp209=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp223; void* _temp225; struct
Cyc_List_List* _temp227; struct Cyc_Position_Segment* _temp229; int _temp231;
struct Cyc_Position_Segment* _temp233; struct Cyc_List_List* _temp235; struct
Cyc_Absyn_Tqual _temp237; void* _temp239; void* _temp241; _LL211: if( _temp209
==( void*) Cyc_Absyn_Carray_mod){ goto _LL212;} else{ goto _LL213;} _LL213: if((
unsigned int) _temp209 > 1u?*(( int*) _temp209) == Cyc_Absyn_ConstArray_mod: 0){
_LL224: _temp223=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp209)->f1; goto
_LL214;} else{ goto _LL215;} _LL215: if(( unsigned int) _temp209 > 1u?*(( int*)
_temp209) == Cyc_Absyn_Function_mod: 0){ _LL226: _temp225=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp209)->f1; goto _LL216;} else{ goto _LL217;} _LL217: if(( unsigned int)
_temp209 > 1u?*(( int*) _temp209) == Cyc_Absyn_Attributes_mod: 0){ _LL230:
_temp229=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp209)->f1; goto _LL228;
_LL228: _temp227=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp209)->f2; goto
_LL218;} else{ goto _LL219;} _LL219: if(( unsigned int) _temp209 > 1u?*(( int*)
_temp209) == Cyc_Absyn_TypeParams_mod: 0){ _LL236: _temp235=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp209)->f1; goto _LL234; _LL234: _temp233=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp209)->f2; goto _LL232; _LL232: _temp231=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp209)->f3; goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int)
_temp209 > 1u?*(( int*) _temp209) == Cyc_Absyn_Pointer_mod: 0){ _LL242: _temp241=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp209)->f1; goto _LL240;
_LL240: _temp239=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp209)->f2;
goto _LL238; _LL238: _temp237=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp209)->f3;
goto _LL222;} else{ goto _LL210;} _LL212: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct
Cyc_PP_Doc*(* _temp243)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp246= rest; struct Cyc_PP_Doc* _temp247= Cyc_PP_text(({ char* _temp248="[]";
struct _tagged_string _temp249; _temp249.curr= _temp248; _temp249.base= _temp248;
_temp249.last_plus_one= _temp248 + 3; _temp249;})); struct Cyc_PP_Doc* _temp244[
2u]={ _temp246, _temp247}; struct _tagged_ptr0 _temp245={ _temp244, _temp244,
_temp244 + 2u}; _temp243( _temp245);}); _LL214: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct
Cyc_PP_Doc*(* _temp250)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp253= rest; struct Cyc_PP_Doc* _temp254= Cyc_PP_text(({ char* _temp259="[";
struct _tagged_string _temp260; _temp260.curr= _temp259; _temp260.base= _temp259;
_temp260.last_plus_one= _temp259 + 2; _temp260;})); struct Cyc_PP_Doc* _temp255=
Cyc_Absynpp_exp2doc( _temp223); struct Cyc_PP_Doc* _temp256= Cyc_PP_text(({ char*
_temp257="]"; struct _tagged_string _temp258; _temp258.curr= _temp257; _temp258.base=
_temp257; _temp258.last_plus_one= _temp257 + 2; _temp258;})); struct Cyc_PP_Doc*
_temp251[ 4u]={ _temp253, _temp254, _temp255, _temp256}; struct _tagged_ptr0
_temp252={ _temp251, _temp251, _temp251 + 4u}; _temp250( _temp252);}); _LL216:
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;}{ void* _temp261= _temp225; struct Cyc_Core_Opt* _temp267; struct
Cyc_Absyn_VarargInfo* _temp269; int _temp271; struct Cyc_List_List* _temp273;
struct Cyc_Position_Segment* _temp275; struct Cyc_List_List* _temp277; _LL263:
if(*(( int*) _temp261) == Cyc_Absyn_WithTypes){ _LL274: _temp273=(( struct Cyc_Absyn_WithTypes_struct*)
_temp261)->f1; goto _LL272; _LL272: _temp271=(( struct Cyc_Absyn_WithTypes_struct*)
_temp261)->f2; goto _LL270; _LL270: _temp269=(( struct Cyc_Absyn_WithTypes_struct*)
_temp261)->f3; goto _LL268; _LL268: _temp267=(( struct Cyc_Absyn_WithTypes_struct*)
_temp261)->f4; goto _LL264;} else{ goto _LL265;} _LL265: if(*(( int*) _temp261)
== Cyc_Absyn_NoTypes){ _LL278: _temp277=(( struct Cyc_Absyn_NoTypes_struct*)
_temp261)->f1; goto _LL276; _LL276: _temp275=(( struct Cyc_Absyn_NoTypes_struct*)
_temp261)->f2; goto _LL266;} else{ goto _LL262;} _LL264: return({ struct Cyc_PP_Doc*(*
_temp279)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp282= rest;
struct Cyc_PP_Doc* _temp283= Cyc_Absynpp_funargs2doc( _temp273, _temp271,
_temp269, _temp267); struct Cyc_PP_Doc* _temp280[ 2u]={ _temp282, _temp283};
struct _tagged_ptr0 _temp281={ _temp280, _temp280, _temp280 + 2u}; _temp279(
_temp281);}); _LL266: return({ struct Cyc_PP_Doc*(* _temp284)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp287= rest; struct Cyc_PP_Doc*
_temp288= Cyc_PP_group(({ char* _temp289="("; struct _tagged_string _temp290;
_temp290.curr= _temp289; _temp290.base= _temp289; _temp290.last_plus_one=
_temp289 + 2; _temp290;}),({ char* _temp291=")"; struct _tagged_string _temp292;
_temp292.curr= _temp291; _temp292.base= _temp291; _temp292.last_plus_one=
_temp291 + 2; _temp292;}),({ char* _temp293=","; struct _tagged_string _temp294;
_temp294.curr= _temp293; _temp294.base= _temp293; _temp294.last_plus_one=
_temp293 + 2; _temp294;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_PP_textptr,
_temp277)); struct Cyc_PP_Doc* _temp285[ 2u]={ _temp287, _temp288}; struct
_tagged_ptr0 _temp286={ _temp285, _temp285, _temp285 + 2u}; _temp284( _temp286);});
_LL262:;} _LL218: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp295)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp298= rest; struct Cyc_PP_Doc*
_temp299= Cyc_Absynpp_atts2doc( _temp227); struct Cyc_PP_Doc* _temp296[ 2u]={
_temp298, _temp299}; struct _tagged_ptr0 _temp297={ _temp296, _temp296, _temp296
+ 2u}; _temp295( _temp297);}); _LL220: if( Cyc_Absynpp_next_is_pointer((( struct
Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} if( _temp231){ return({
struct Cyc_PP_Doc*(* _temp300)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp303= rest; struct Cyc_PP_Doc* _temp304= Cyc_Absynpp_ktvars2doc( _temp235);
struct Cyc_PP_Doc* _temp301[ 2u]={ _temp303, _temp304}; struct _tagged_ptr0
_temp302={ _temp301, _temp301, _temp301 + 2u}; _temp300( _temp302);});} else{
return({ struct Cyc_PP_Doc*(* _temp305)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp308= rest; struct Cyc_PP_Doc* _temp309= Cyc_Absynpp_tvars2doc(
_temp235); struct Cyc_PP_Doc* _temp306[ 2u]={ _temp308, _temp309}; struct
_tagged_ptr0 _temp307={ _temp306, _temp306, _temp306 + 2u}; _temp305( _temp307);});}
_LL222: { struct Cyc_PP_Doc* ptr;{ void* _temp310= _temp241; struct Cyc_Absyn_Exp*
_temp318; struct Cyc_Absyn_Exp* _temp320; _LL312: if(( unsigned int) _temp310 >
1u?*(( int*) _temp310) == Cyc_Absyn_Nullable_ps: 0){ _LL319: _temp318=(( struct
Cyc_Absyn_Nullable_ps_struct*) _temp310)->f1; goto _LL313;} else{ goto _LL314;}
_LL314: if(( unsigned int) _temp310 > 1u?*(( int*) _temp310) == Cyc_Absyn_NonNullable_ps:
0){ _LL321: _temp320=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp310)->f1;
goto _LL315;} else{ goto _LL316;} _LL316: if( _temp310 ==( void*) Cyc_Absyn_TaggedArray_ps){
goto _LL317;} else{ goto _LL311;} _LL313: if( Cyc_Evexp_eval_const_uint_exp(
_temp318) == 1){ ptr= Cyc_PP_text(({ char* _temp322="*"; struct _tagged_string
_temp323; _temp323.curr= _temp322; _temp323.base= _temp322; _temp323.last_plus_one=
_temp322 + 2; _temp323;}));} else{ ptr=({ struct Cyc_PP_Doc*(* _temp324)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp327= Cyc_PP_text(({ char*
_temp335="*"; struct _tagged_string _temp336; _temp336.curr= _temp335; _temp336.base=
_temp335; _temp336.last_plus_one= _temp335 + 2; _temp336;})); struct Cyc_PP_Doc*
_temp328= Cyc_PP_text(({ char* _temp333="{"; struct _tagged_string _temp334;
_temp334.curr= _temp333; _temp334.base= _temp333; _temp334.last_plus_one=
_temp333 + 2; _temp334;})); struct Cyc_PP_Doc* _temp329= Cyc_Absynpp_exp2doc(
_temp318); struct Cyc_PP_Doc* _temp330= Cyc_PP_text(({ char* _temp331="}";
struct _tagged_string _temp332; _temp332.curr= _temp331; _temp332.base= _temp331;
_temp332.last_plus_one= _temp331 + 2; _temp332;})); struct Cyc_PP_Doc* _temp325[
4u]={ _temp327, _temp328, _temp329, _temp330}; struct _tagged_ptr0 _temp326={
_temp325, _temp325, _temp325 + 4u}; _temp324( _temp326);});} goto _LL311; _LL315:
if( Cyc_Evexp_eval_const_uint_exp( _temp320) == 1){ ptr= Cyc_PP_text(({ char*
_temp337="@"; struct _tagged_string _temp338; _temp338.curr= _temp337; _temp338.base=
_temp337; _temp338.last_plus_one= _temp337 + 2; _temp338;}));} else{ ptr=({
struct Cyc_PP_Doc*(* _temp339)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp342= Cyc_PP_text(({ char* _temp350="@"; struct _tagged_string _temp351;
_temp351.curr= _temp350; _temp351.base= _temp350; _temp351.last_plus_one=
_temp350 + 2; _temp351;})); struct Cyc_PP_Doc* _temp343= Cyc_PP_text(({ char*
_temp348="{"; struct _tagged_string _temp349; _temp349.curr= _temp348; _temp349.base=
_temp348; _temp349.last_plus_one= _temp348 + 2; _temp349;})); struct Cyc_PP_Doc*
_temp344= Cyc_Absynpp_exp2doc( _temp320); struct Cyc_PP_Doc* _temp345= Cyc_PP_text(({
char* _temp346="}"; struct _tagged_string _temp347; _temp347.curr= _temp346;
_temp347.base= _temp346; _temp347.last_plus_one= _temp346 + 2; _temp347;}));
struct Cyc_PP_Doc* _temp340[ 4u]={ _temp342, _temp343, _temp344, _temp345};
struct _tagged_ptr0 _temp341={ _temp340, _temp340, _temp340 + 4u}; _temp339(
_temp341);});} goto _LL311; _LL317: ptr= Cyc_PP_text(({ char* _temp352="?";
struct _tagged_string _temp353; _temp353.curr= _temp352; _temp353.base= _temp352;
_temp353.last_plus_one= _temp352 + 2; _temp353;})); goto _LL311; _LL311:;}{ void*
_temp354= Cyc_Tcutil_compress( _temp239); _LL356: if( _temp354 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL357;} else{ goto _LL358;} _LL358: goto _LL359; _LL357: return({ struct
Cyc_PP_Doc*(* _temp360)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp363= ptr; struct Cyc_PP_Doc* _temp364= rest; struct Cyc_PP_Doc* _temp361[ 2u]={
_temp363, _temp364}; struct _tagged_ptr0 _temp362={ _temp361, _temp361, _temp361
+ 2u}; _temp360( _temp362);}); _LL359: return({ struct Cyc_PP_Doc*(* _temp365)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp368= ptr; struct Cyc_PP_Doc*
_temp369= Cyc_Absynpp_ntyp2doc( _temp239); struct Cyc_PP_Doc* _temp370= Cyc_PP_text(({
char* _temp372=" "; struct _tagged_string _temp373; _temp373.curr= _temp372;
_temp373.base= _temp372; _temp373.last_plus_one= _temp372 + 2; _temp373;}));
struct Cyc_PP_Doc* _temp371= rest; struct Cyc_PP_Doc* _temp366[ 4u]={ _temp368,
_temp369, _temp370, _temp371}; struct _tagged_ptr0 _temp367={ _temp366, _temp366,
_temp366 + 4u}; _temp365( _temp367);}); _LL355:;}} _LL210:;}} struct Cyc_PP_Doc*
Cyc_Absynpp_rgn2doc( void* t){ void* _temp384= Cyc_Tcutil_compress( t); _LL386:
if( _temp384 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL387;} else{ goto _LL388;}
_LL388: goto _LL389; _LL387: return Cyc_PP_text(({ char* _temp390="`H"; struct
_tagged_string _temp391; _temp391.curr= _temp390; _temp391.base= _temp390;
_temp391.last_plus_one= _temp390 + 3; _temp391;})); _LL389: return Cyc_Absynpp_ntyp2doc(
t); _LL385:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
regions, struct Cyc_List_List** effects, void* t){ void* _temp392= Cyc_Tcutil_compress(
t); void* _temp400; struct Cyc_List_List* _temp402; _LL394: if(( unsigned int)
_temp392 > 4u?*(( int*) _temp392) == Cyc_Absyn_AccessEff: 0){ _LL401: _temp400=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp392)->f1; goto _LL395;} else{
goto _LL396;} _LL396: if(( unsigned int) _temp392 > 4u?*(( int*) _temp392) ==
Cyc_Absyn_JoinEff: 0){ _LL403: _temp402=(( struct Cyc_Absyn_JoinEff_struct*)
_temp392)->f1; goto _LL397;} else{ goto _LL398;} _LL398: goto _LL399; _LL395:*
regions=({ struct Cyc_List_List* _temp404=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp404->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp400); _temp404->tl=* regions; _temp404;}); goto _LL393; _LL397: for( 0;
_temp402 != 0; _temp402=(( struct Cyc_List_List*) _check_null( _temp402))->tl){
Cyc_Absynpp_effects2docs( regions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp402))->hd);} goto _LL393; _LL399:* effects=({ struct Cyc_List_List*
_temp405=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp405->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp405->tl=* effects; _temp405;});
goto _LL393; _LL393:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_PP_group(({
char* _temp406=""; struct _tagged_string _temp407; _temp407.curr= _temp406;
_temp407.base= _temp406; _temp407.last_plus_one= _temp406 + 1; _temp407;}),({
char* _temp408=""; struct _tagged_string _temp409; _temp409.curr= _temp408;
_temp409.base= _temp408; _temp409.last_plus_one= _temp408 + 1; _temp409;}),({
char* _temp410="+"; struct _tagged_string _temp411; _temp411.curr= _temp410;
_temp411.base= _temp410; _temp411.last_plus_one= _temp410 + 2; _temp411;}),
effects);} else{ struct Cyc_PP_Doc* _temp418= Cyc_PP_group(({ char* _temp412="{";
struct _tagged_string _temp413; _temp413.curr= _temp412; _temp413.base= _temp412;
_temp413.last_plus_one= _temp412 + 2; _temp413;}),({ char* _temp414="}"; struct
_tagged_string _temp415; _temp415.curr= _temp414; _temp415.base= _temp414;
_temp415.last_plus_one= _temp414 + 2; _temp415;}),({ char* _temp416=","; struct
_tagged_string _temp417; _temp417.curr= _temp416; _temp417.base= _temp416;
_temp417.last_plus_one= _temp416 + 2; _temp417;}), regions); goto _LL419; _LL419:
return Cyc_PP_group(({ char* _temp420=""; struct _tagged_string _temp421;
_temp421.curr= _temp420; _temp421.base= _temp420; _temp421.last_plus_one=
_temp420 + 1; _temp421;}),({ char* _temp422=""; struct _tagged_string _temp423;
_temp423.curr= _temp422; _temp423.base= _temp422; _temp423.last_plus_one=
_temp422 + 1; _temp423;}),({ char* _temp424="+"; struct _tagged_string _temp425;
_temp425.curr= _temp424; _temp425.base= _temp424; _temp425.last_plus_one=
_temp424 + 2; _temp425;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( effects,({ struct Cyc_List_List*
_temp426=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp426->hd=( void*) _temp418; _temp426->tl= 0; _temp426;})));}} struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc* s;{ void* _temp427= t; struct
Cyc_Absyn_Exp* _temp473; struct Cyc_Absyn_Tqual _temp475; void* _temp477; struct
Cyc_Absyn_FnInfo _temp479; struct Cyc_Absyn_PtrInfo _temp481; int _temp483;
struct Cyc_Core_Opt* _temp485; void* _temp487; struct Cyc_Absyn_Tvar* _temp489;
struct Cyc_Absyn_TunionInfo _temp491; void* _temp493; struct Cyc_List_List*
_temp495; void* _temp497; struct Cyc_Absyn_TunionFieldInfo _temp499; struct Cyc_List_List*
_temp501; void* _temp503; void* _temp505; void* _temp507; struct Cyc_List_List*
_temp509; struct Cyc_Absyn_Structdecl** _temp511; struct Cyc_List_List* _temp513;
struct _tuple0* _temp515; struct Cyc_Absyn_Uniondecl** _temp517; struct Cyc_List_List*
_temp519; struct _tuple0* _temp521; struct Cyc_List_List* _temp523; struct Cyc_List_List*
_temp525; struct Cyc_Absyn_Enumdecl* _temp527; struct _tuple0* _temp529; struct
Cyc_Core_Opt* _temp531; struct Cyc_List_List* _temp533; struct _tuple0* _temp535;
void* _temp537; void* _temp539; struct Cyc_List_List* _temp541; _LL429: if((
unsigned int) _temp427 > 4u?*(( int*) _temp427) == Cyc_Absyn_ArrayType: 0){
_LL478: _temp477=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp427)->f1;
goto _LL476; _LL476: _temp475=(( struct Cyc_Absyn_ArrayType_struct*) _temp427)->f2;
goto _LL474; _LL474: _temp473=(( struct Cyc_Absyn_ArrayType_struct*) _temp427)->f3;
goto _LL430;} else{ goto _LL431;} _LL431: if(( unsigned int) _temp427 > 4u?*((
int*) _temp427) == Cyc_Absyn_FnType: 0){ _LL480: _temp479=(( struct Cyc_Absyn_FnType_struct*)
_temp427)->f1; goto _LL432;} else{ goto _LL433;} _LL433: if(( unsigned int)
_temp427 > 4u?*(( int*) _temp427) == Cyc_Absyn_PointerType: 0){ _LL482: _temp481=((
struct Cyc_Absyn_PointerType_struct*) _temp427)->f1; goto _LL434;} else{ goto
_LL435;} _LL435: if( _temp427 ==( void*) Cyc_Absyn_VoidType){ goto _LL436;}
else{ goto _LL437;} _LL437: if(( unsigned int) _temp427 > 4u?*(( int*) _temp427)
== Cyc_Absyn_Evar: 0){ _LL488: _temp487=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp427)->f1; goto _LL486; _LL486: _temp485=(( struct Cyc_Absyn_Evar_struct*)
_temp427)->f2; goto _LL484; _LL484: _temp483=(( struct Cyc_Absyn_Evar_struct*)
_temp427)->f3; goto _LL438;} else{ goto _LL439;} _LL439: if(( unsigned int)
_temp427 > 4u?*(( int*) _temp427) == Cyc_Absyn_VarType: 0){ _LL490: _temp489=((
struct Cyc_Absyn_VarType_struct*) _temp427)->f1; goto _LL440;} else{ goto _LL441;}
_LL441: if(( unsigned int) _temp427 > 4u?*(( int*) _temp427) == Cyc_Absyn_TunionType:
0){ _LL492: _temp491=(( struct Cyc_Absyn_TunionType_struct*) _temp427)->f1;
_LL498: _temp497=( void*) _temp491.tunion_info; goto _LL496; _LL496: _temp495=
_temp491.targs; goto _LL494; _LL494: _temp493=( void*) _temp491.rgn; goto _LL442;}
else{ goto _LL443;} _LL443: if(( unsigned int) _temp427 > 4u?*(( int*) _temp427)
== Cyc_Absyn_TunionFieldType: 0){ _LL500: _temp499=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp427)->f1; _LL504: _temp503=( void*) _temp499.field_info; goto _LL502;
_LL502: _temp501= _temp499.targs; goto _LL444;} else{ goto _LL445;} _LL445: if((
unsigned int) _temp427 > 4u?*(( int*) _temp427) == Cyc_Absyn_IntType: 0){ _LL508:
_temp507=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp427)->f1; goto _LL506;
_LL506: _temp505=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp427)->f2;
goto _LL446;} else{ goto _LL447;} _LL447: if( _temp427 ==( void*) Cyc_Absyn_FloatType){
goto _LL448;} else{ goto _LL449;} _LL449: if( _temp427 ==( void*) Cyc_Absyn_DoubleType){
goto _LL450;} else{ goto _LL451;} _LL451: if(( unsigned int) _temp427 > 4u?*((
int*) _temp427) == Cyc_Absyn_TupleType: 0){ _LL510: _temp509=(( struct Cyc_Absyn_TupleType_struct*)
_temp427)->f1; goto _LL452;} else{ goto _LL453;} _LL453: if(( unsigned int)
_temp427 > 4u?*(( int*) _temp427) == Cyc_Absyn_StructType: 0){ _LL516: _temp515=((
struct Cyc_Absyn_StructType_struct*) _temp427)->f1; goto _LL514; _LL514:
_temp513=(( struct Cyc_Absyn_StructType_struct*) _temp427)->f2; goto _LL512;
_LL512: _temp511=(( struct Cyc_Absyn_StructType_struct*) _temp427)->f3; goto
_LL454;} else{ goto _LL455;} _LL455: if(( unsigned int) _temp427 > 4u?*(( int*)
_temp427) == Cyc_Absyn_UnionType: 0){ _LL522: _temp521=(( struct Cyc_Absyn_UnionType_struct*)
_temp427)->f1; goto _LL520; _LL520: _temp519=(( struct Cyc_Absyn_UnionType_struct*)
_temp427)->f2; goto _LL518; _LL518: _temp517=(( struct Cyc_Absyn_UnionType_struct*)
_temp427)->f3; goto _LL456;} else{ goto _LL457;} _LL457: if(( unsigned int)
_temp427 > 4u?*(( int*) _temp427) == Cyc_Absyn_AnonStructType: 0){ _LL524:
_temp523=(( struct Cyc_Absyn_AnonStructType_struct*) _temp427)->f1; goto _LL458;}
else{ goto _LL459;} _LL459: if(( unsigned int) _temp427 > 4u?*(( int*) _temp427)
== Cyc_Absyn_AnonUnionType: 0){ _LL526: _temp525=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp427)->f1; goto _LL460;} else{ goto _LL461;} _LL461: if(( unsigned int)
_temp427 > 4u?*(( int*) _temp427) == Cyc_Absyn_EnumType: 0){ _LL530: _temp529=((
struct Cyc_Absyn_EnumType_struct*) _temp427)->f1; goto _LL528; _LL528: _temp527=((
struct Cyc_Absyn_EnumType_struct*) _temp427)->f2; goto _LL462;} else{ goto
_LL463;} _LL463: if(( unsigned int) _temp427 > 4u?*(( int*) _temp427) == Cyc_Absyn_TypedefType:
0){ _LL536: _temp535=(( struct Cyc_Absyn_TypedefType_struct*) _temp427)->f1;
goto _LL534; _LL534: _temp533=(( struct Cyc_Absyn_TypedefType_struct*) _temp427)->f2;
goto _LL532; _LL532: _temp531=(( struct Cyc_Absyn_TypedefType_struct*) _temp427)->f3;
goto _LL464;} else{ goto _LL465;} _LL465: if(( unsigned int) _temp427 > 4u?*((
int*) _temp427) == Cyc_Absyn_RgnHandleType: 0){ _LL538: _temp537=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp427)->f1; goto _LL466;} else{ goto
_LL467;} _LL467: if( _temp427 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL468;} else{
goto _LL469;} _LL469: if(( unsigned int) _temp427 > 4u?*(( int*) _temp427) ==
Cyc_Absyn_AccessEff: 0){ _LL540: _temp539=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp427)->f1; goto _LL470;} else{ goto _LL471;} _LL471: if(( unsigned int)
_temp427 > 4u?*(( int*) _temp427) == Cyc_Absyn_JoinEff: 0){ _LL542: _temp541=((
struct Cyc_Absyn_JoinEff_struct*) _temp427)->f1; goto _LL472;} else{ goto _LL428;}
_LL430: return Cyc_PP_text(({ char* _temp543="[[[array]]]"; struct
_tagged_string _temp544; _temp544.curr= _temp543; _temp544.base= _temp543;
_temp544.last_plus_one= _temp543 + 12; _temp544;})); _LL432: return Cyc_PP_nil_doc();
_LL434: return Cyc_PP_nil_doc(); _LL436: s= Cyc_PP_text(({ char* _temp545="void";
struct _tagged_string _temp546; _temp546.curr= _temp545; _temp546.base= _temp545;
_temp546.last_plus_one= _temp545 + 5; _temp546;})); goto _LL428; _LL438: if(
_temp485 != 0){ return Cyc_Absynpp_ntyp2doc(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp485))->v);} else{ s=({ struct Cyc_PP_Doc*(* _temp547)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp550= Cyc_PP_text(({ char*
_temp559="%"; struct _tagged_string _temp560; _temp560.curr= _temp559; _temp560.base=
_temp559; _temp560.last_plus_one= _temp559 + 2; _temp560;})); struct Cyc_PP_Doc*
_temp551= Cyc_PP_text(({ char* _temp557="("; struct _tagged_string _temp558;
_temp558.curr= _temp557; _temp558.base= _temp557; _temp558.last_plus_one=
_temp557 + 2; _temp558;})); struct Cyc_PP_Doc* _temp552= Cyc_PP_text( xprintf("%d",
_temp483)); struct Cyc_PP_Doc* _temp553= Cyc_PP_text(({ char* _temp555=")::";
struct _tagged_string _temp556; _temp556.curr= _temp555; _temp556.base= _temp555;
_temp556.last_plus_one= _temp555 + 4; _temp556;})); struct Cyc_PP_Doc* _temp554=
Cyc_Absynpp_kind2doc( _temp487); struct Cyc_PP_Doc* _temp548[ 5u]={ _temp550,
_temp551, _temp552, _temp553, _temp554}; struct _tagged_ptr0 _temp549={ _temp548,
_temp548, _temp548 + 5u}; _temp547( _temp549);});} goto _LL428; _LL440: s= Cyc_PP_textptr(
_temp489->name); if( Cyc_Absynpp_print_all_kinds){ s=({ struct Cyc_PP_Doc*(*
_temp561)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp564= s;
struct Cyc_PP_Doc* _temp565= Cyc_PP_text(({ char* _temp567="::"; struct
_tagged_string _temp568; _temp568.curr= _temp567; _temp568.base= _temp567;
_temp568.last_plus_one= _temp567 + 3; _temp568;})); struct Cyc_PP_Doc* _temp566=
Cyc_Absynpp_ckind2doc( _temp489->kind); struct Cyc_PP_Doc* _temp562[ 3u]={
_temp564, _temp565, _temp566}; struct _tagged_ptr0 _temp563={ _temp562, _temp562,
_temp562 + 3u}; _temp561( _temp563);});} if( ! Cyc_Absynpp_dont_rewrite_temp_tvars?
Cyc_Tcutil_is_temp_tvar( _temp489): 0){ s=({ struct Cyc_PP_Doc*(* _temp569)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp572= Cyc_PP_text(({
char* _temp577="_ /* "; struct _tagged_string _temp578; _temp578.curr= _temp577;
_temp578.base= _temp577; _temp578.last_plus_one= _temp577 + 6; _temp578;}));
struct Cyc_PP_Doc* _temp573= s; struct Cyc_PP_Doc* _temp574= Cyc_PP_text(({ char*
_temp575=" */"; struct _tagged_string _temp576; _temp576.curr= _temp575;
_temp576.base= _temp575; _temp576.last_plus_one= _temp575 + 4; _temp576;}));
struct Cyc_PP_Doc* _temp570[ 3u]={ _temp572, _temp573, _temp574}; struct
_tagged_ptr0 _temp571={ _temp570, _temp570, _temp570 + 3u}; _temp569( _temp571);});}
goto _LL428; _LL442:{ void* _temp579= _temp497; struct Cyc_Absyn_UnknownTunionInfo
_temp585; int _temp587; struct _tuple0* _temp589; struct Cyc_Absyn_Tuniondecl*
_temp591; struct Cyc_Absyn_Tuniondecl _temp593; int _temp594; struct Cyc_Core_Opt*
_temp596; struct Cyc_List_List* _temp598; struct _tuple0* _temp600; void*
_temp602; _LL581: if(*(( int*) _temp579) == Cyc_Absyn_UnknownTunion){ _LL586:
_temp585=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp579)->f1; _LL590:
_temp589= _temp585.name; goto _LL588; _LL588: _temp587= _temp585.is_xtunion;
goto _LL582;} else{ goto _LL583;} _LL583: if(*(( int*) _temp579) == Cyc_Absyn_KnownTunion){
_LL592: _temp591=(( struct Cyc_Absyn_KnownTunion_struct*) _temp579)->f1;
_temp593=* _temp591; _LL603: _temp602=( void*) _temp593.sc; goto _LL601; _LL601:
_temp600= _temp593.name; goto _LL599; _LL599: _temp598= _temp593.tvs; goto
_LL597; _LL597: _temp596= _temp593.fields; goto _LL595; _LL595: _temp594=
_temp593.is_xtunion; goto _LL584;} else{ goto _LL580;} _LL582: _temp600=
_temp589; _temp594= _temp587; goto _LL584; _LL584: { struct Cyc_PP_Doc* _temp608=
Cyc_PP_text( _temp594?({ char* _temp604="xtunion "; struct _tagged_string
_temp605; _temp605.curr= _temp604; _temp605.base= _temp604; _temp605.last_plus_one=
_temp604 + 9; _temp605;}):({ char* _temp606="tunion "; struct _tagged_string
_temp607; _temp607.curr= _temp606; _temp607.base= _temp606; _temp607.last_plus_one=
_temp606 + 8; _temp607;})); goto _LL609; _LL609:{ void* _temp610= Cyc_Tcutil_compress(
_temp493); _LL612: if( _temp610 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL613;}
else{ goto _LL614;} _LL614: goto _LL615; _LL613: s=({ struct Cyc_PP_Doc*(*
_temp616)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp619=
_temp608; struct Cyc_PP_Doc* _temp620= Cyc_Absynpp_qvar2doc( _temp600); struct
Cyc_PP_Doc* _temp621= Cyc_Absynpp_tps2doc( _temp495); struct Cyc_PP_Doc*
_temp617[ 3u]={ _temp619, _temp620, _temp621}; struct _tagged_ptr0 _temp618={
_temp617, _temp617, _temp617 + 3u}; _temp616( _temp618);}); goto _LL611; _LL615:
s=({ struct Cyc_PP_Doc*(* _temp622)( struct _tagged_ptr0)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp625= _temp608; struct Cyc_PP_Doc* _temp626= Cyc_Absynpp_typ2doc(
_temp493); struct Cyc_PP_Doc* _temp627= Cyc_PP_text(({ char* _temp630=" ";
struct _tagged_string _temp631; _temp631.curr= _temp630; _temp631.base= _temp630;
_temp631.last_plus_one= _temp630 + 2; _temp631;})); struct Cyc_PP_Doc* _temp628=
Cyc_Absynpp_qvar2doc( _temp600); struct Cyc_PP_Doc* _temp629= Cyc_Absynpp_tps2doc(
_temp495); struct Cyc_PP_Doc* _temp623[ 5u]={ _temp625, _temp626, _temp627,
_temp628, _temp629}; struct _tagged_ptr0 _temp624={ _temp623, _temp623, _temp623
+ 5u}; _temp622( _temp624);}); goto _LL611; _LL611:;} goto _LL580;} _LL580:;}
goto _LL428; _LL444:{ void* _temp632= _temp503; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp638; int _temp640; struct _tuple0* _temp642; struct _tuple0* _temp644;
struct Cyc_Absyn_Tunionfield* _temp646; struct Cyc_Absyn_Tunionfield _temp648;
void* _temp649; struct Cyc_Position_Segment* _temp651; struct Cyc_List_List*
_temp653; struct Cyc_List_List* _temp655; struct _tuple0* _temp657; struct Cyc_Absyn_Tuniondecl*
_temp659; struct Cyc_Absyn_Tuniondecl _temp661; int _temp662; struct Cyc_Core_Opt*
_temp664; struct Cyc_List_List* _temp666; struct _tuple0* _temp668; void*
_temp670; _LL634: if(*(( int*) _temp632) == Cyc_Absyn_UnknownTunionfield){
_LL639: _temp638=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp632)->f1;
_LL645: _temp644= _temp638.tunion_name; goto _LL643; _LL643: _temp642= _temp638.field_name;
goto _LL641; _LL641: _temp640= _temp638.is_xtunion; goto _LL635;} else{ goto
_LL636;} _LL636: if(*(( int*) _temp632) == Cyc_Absyn_KnownTunionfield){ _LL660:
_temp659=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp632)->f1; _temp661=*
_temp659; _LL671: _temp670=( void*) _temp661.sc; goto _LL669; _LL669: _temp668=
_temp661.name; goto _LL667; _LL667: _temp666= _temp661.tvs; goto _LL665; _LL665:
_temp664= _temp661.fields; goto _LL663; _LL663: _temp662= _temp661.is_xtunion;
goto _LL647; _LL647: _temp646=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp632)->f2; _temp648=* _temp646; _LL658: _temp657= _temp648.name; goto _LL656;
_LL656: _temp655= _temp648.tvs; goto _LL654; _LL654: _temp653= _temp648.typs;
goto _LL652; _LL652: _temp651= _temp648.loc; goto _LL650; _LL650: _temp649=(
void*) _temp648.sc; goto _LL637;} else{ goto _LL633;} _LL635: _temp668= _temp644;
_temp662= _temp640; _temp657= _temp642; goto _LL637; _LL637: { struct Cyc_PP_Doc*
_temp676= Cyc_PP_text( _temp662?({ char* _temp672="xtunion "; struct
_tagged_string _temp673; _temp673.curr= _temp672; _temp673.base= _temp672;
_temp673.last_plus_one= _temp672 + 9; _temp673;}):({ char* _temp674="tunion ";
struct _tagged_string _temp675; _temp675.curr= _temp674; _temp675.base= _temp674;
_temp675.last_plus_one= _temp674 + 8; _temp675;})); goto _LL677; _LL677: s=({
struct Cyc_PP_Doc*(* _temp678)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp681= _temp676; struct Cyc_PP_Doc* _temp682= Cyc_Absynpp_qvar2doc( _temp668);
struct Cyc_PP_Doc* _temp683= Cyc_PP_text(({ char* _temp685="."; struct
_tagged_string _temp686; _temp686.curr= _temp685; _temp686.base= _temp685;
_temp686.last_plus_one= _temp685 + 2; _temp686;})); struct Cyc_PP_Doc* _temp684=
Cyc_PP_textptr((* _temp657).f2); struct Cyc_PP_Doc* _temp679[ 4u]={ _temp681,
_temp682, _temp683, _temp684}; struct _tagged_ptr0 _temp680={ _temp679, _temp679,
_temp679 + 4u}; _temp678( _temp680);}); goto _LL633;} _LL633:;} goto _LL428;
_LL446: { struct _tagged_string sns; struct _tagged_string ts;{ void* _temp687=
_temp507; _LL689: if( _temp687 ==( void*) Cyc_Absyn_Signed){ goto _LL690;} else{
goto _LL691;} _LL691: if( _temp687 ==( void*) Cyc_Absyn_Unsigned){ goto _LL692;}
else{ goto _LL688;} _LL690: sns=({ char* _temp693=""; struct _tagged_string
_temp694; _temp694.curr= _temp693; _temp694.base= _temp693; _temp694.last_plus_one=
_temp693 + 1; _temp694;}); goto _LL688; _LL692: sns=({ char* _temp695="unsigned ";
struct _tagged_string _temp696; _temp696.curr= _temp695; _temp696.base= _temp695;
_temp696.last_plus_one= _temp695 + 10; _temp696;}); goto _LL688; _LL688:;}{ void*
_temp697= _temp505; _LL699: if( _temp697 ==( void*) Cyc_Absyn_B1){ goto _LL700;}
else{ goto _LL701;} _LL701: if( _temp697 ==( void*) Cyc_Absyn_B2){ goto _LL702;}
else{ goto _LL703;} _LL703: if( _temp697 ==( void*) Cyc_Absyn_B4){ goto _LL704;}
else{ goto _LL705;} _LL705: if( _temp697 ==( void*) Cyc_Absyn_B8){ goto _LL706;}
else{ goto _LL698;} _LL700:{ void* _temp707= _temp507; _LL709: if( _temp707 ==(
void*) Cyc_Absyn_Signed){ goto _LL710;} else{ goto _LL711;} _LL711: if( _temp707
==( void*) Cyc_Absyn_Unsigned){ goto _LL712;} else{ goto _LL708;} _LL710: sns=({
char* _temp713="signed "; struct _tagged_string _temp714; _temp714.curr=
_temp713; _temp714.base= _temp713; _temp714.last_plus_one= _temp713 + 8;
_temp714;}); goto _LL708; _LL712: sns=({ char* _temp715=""; struct
_tagged_string _temp716; _temp716.curr= _temp715; _temp716.base= _temp715;
_temp716.last_plus_one= _temp715 + 1; _temp716;}); goto _LL708; _LL708:;} ts=({
char* _temp717="char"; struct _tagged_string _temp718; _temp718.curr= _temp717;
_temp718.base= _temp717; _temp718.last_plus_one= _temp717 + 5; _temp718;}); goto
_LL698; _LL702: ts=({ char* _temp719="short"; struct _tagged_string _temp720;
_temp720.curr= _temp719; _temp720.base= _temp719; _temp720.last_plus_one=
_temp719 + 6; _temp720;}); goto _LL698; _LL704: ts=({ char* _temp721="int";
struct _tagged_string _temp722; _temp722.curr= _temp721; _temp722.base= _temp721;
_temp722.last_plus_one= _temp721 + 4; _temp722;}); goto _LL698; _LL706: ts= Cyc_Absynpp_to_VC?({
char* _temp723="__int64"; struct _tagged_string _temp724; _temp724.curr=
_temp723; _temp724.base= _temp723; _temp724.last_plus_one= _temp723 + 8;
_temp724;}):({ char* _temp725="long long"; struct _tagged_string _temp726;
_temp726.curr= _temp725; _temp726.base= _temp725; _temp726.last_plus_one=
_temp725 + 10; _temp726;}); goto _LL698; _LL698:;} s= Cyc_PP_text(({ struct
_tagged_string _temp727= sns; struct _tagged_string _temp728= ts; xprintf("%.*s%.*s",
_temp727.last_plus_one - _temp727.curr, _temp727.curr, _temp728.last_plus_one -
_temp728.curr, _temp728.curr);})); goto _LL428;} _LL448: s= Cyc_PP_text(({ char*
_temp729="float"; struct _tagged_string _temp730; _temp730.curr= _temp729;
_temp730.base= _temp729; _temp730.last_plus_one= _temp729 + 6; _temp730;}));
goto _LL428; _LL450: s= Cyc_PP_text(({ char* _temp731="double"; struct
_tagged_string _temp732; _temp732.curr= _temp731; _temp732.base= _temp731;
_temp732.last_plus_one= _temp731 + 7; _temp732;})); goto _LL428; _LL452: s=({
struct Cyc_PP_Doc*(* _temp733)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp736= Cyc_PP_text(({ char* _temp738="$"; struct _tagged_string _temp739;
_temp739.curr= _temp738; _temp739.base= _temp738; _temp739.last_plus_one=
_temp738 + 2; _temp739;})); struct Cyc_PP_Doc* _temp737= Cyc_Absynpp_args2doc(
_temp509); struct Cyc_PP_Doc* _temp734[ 2u]={ _temp736, _temp737}; struct
_tagged_ptr0 _temp735={ _temp734, _temp734, _temp734 + 2u}; _temp733( _temp735);});
goto _LL428; _LL454: if( _temp515 == 0){ s=({ struct Cyc_PP_Doc*(* _temp740)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp743= Cyc_PP_text(({
char* _temp745="struct "; struct _tagged_string _temp746; _temp746.curr=
_temp745; _temp746.base= _temp745; _temp746.last_plus_one= _temp745 + 8;
_temp746;})); struct Cyc_PP_Doc* _temp744= Cyc_Absynpp_tps2doc( _temp513);
struct Cyc_PP_Doc* _temp741[ 2u]={ _temp743, _temp744}; struct _tagged_ptr0
_temp742={ _temp741, _temp741, _temp741 + 2u}; _temp740( _temp742);});} else{ s=({
struct Cyc_PP_Doc*(* _temp747)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp750= Cyc_PP_text(({ char* _temp753="struct "; struct _tagged_string
_temp754; _temp754.curr= _temp753; _temp754.base= _temp753; _temp754.last_plus_one=
_temp753 + 8; _temp754;})); struct Cyc_PP_Doc* _temp751= _temp515 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp515)); struct Cyc_PP_Doc*
_temp752= Cyc_Absynpp_tps2doc( _temp513); struct Cyc_PP_Doc* _temp748[ 3u]={
_temp750, _temp751, _temp752}; struct _tagged_ptr0 _temp749={ _temp748, _temp748,
_temp748 + 3u}; _temp747( _temp749);});} goto _LL428; _LL456: if( _temp521 == 0){
s=({ struct Cyc_PP_Doc*(* _temp755)( struct _tagged_ptr0)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp758= Cyc_PP_text(({ char* _temp760="union "; struct
_tagged_string _temp761; _temp761.curr= _temp760; _temp761.base= _temp760;
_temp761.last_plus_one= _temp760 + 7; _temp761;})); struct Cyc_PP_Doc* _temp759=
Cyc_Absynpp_tps2doc( _temp519); struct Cyc_PP_Doc* _temp756[ 2u]={ _temp758,
_temp759}; struct _tagged_ptr0 _temp757={ _temp756, _temp756, _temp756 + 2u};
_temp755( _temp757);});} else{ s=({ struct Cyc_PP_Doc*(* _temp762)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp765= Cyc_PP_text(({ char*
_temp768="union "; struct _tagged_string _temp769; _temp769.curr= _temp768;
_temp769.base= _temp768; _temp769.last_plus_one= _temp768 + 7; _temp769;}));
struct Cyc_PP_Doc* _temp766= _temp521 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*) _check_null( _temp521)); struct Cyc_PP_Doc* _temp767= Cyc_Absynpp_tps2doc(
_temp519); struct Cyc_PP_Doc* _temp763[ 3u]={ _temp765, _temp766, _temp767};
struct _tagged_ptr0 _temp764={ _temp763, _temp763, _temp763 + 3u}; _temp762(
_temp764);});} goto _LL428; _LL458: s=({ struct Cyc_PP_Doc*(* _temp770)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp773= Cyc_PP_text(({ char*
_temp778="struct {"; struct _tagged_string _temp779; _temp779.curr= _temp778;
_temp779.base= _temp778; _temp779.last_plus_one= _temp778 + 9; _temp779;}));
struct Cyc_PP_Doc* _temp774= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp523)); struct Cyc_PP_Doc* _temp775= Cyc_PP_text(({ char* _temp776="}";
struct _tagged_string _temp777; _temp777.curr= _temp776; _temp777.base= _temp776;
_temp777.last_plus_one= _temp776 + 2; _temp777;})); struct Cyc_PP_Doc* _temp771[
3u]={ _temp773, _temp774, _temp775}; struct _tagged_ptr0 _temp772={ _temp771,
_temp771, _temp771 + 3u}; _temp770( _temp772);}); goto _LL428; _LL460: s=({
struct Cyc_PP_Doc*(* _temp780)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp783= Cyc_PP_text(({ char* _temp788="union {"; struct _tagged_string
_temp789; _temp789.curr= _temp788; _temp789.base= _temp788; _temp789.last_plus_one=
_temp788 + 8; _temp789;})); struct Cyc_PP_Doc* _temp784= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp525)); struct Cyc_PP_Doc* _temp785= Cyc_PP_text(({ char* _temp786="}";
struct _tagged_string _temp787; _temp787.curr= _temp786; _temp787.base= _temp786;
_temp787.last_plus_one= _temp786 + 2; _temp787;})); struct Cyc_PP_Doc* _temp781[
3u]={ _temp783, _temp784, _temp785}; struct _tagged_ptr0 _temp782={ _temp781,
_temp781, _temp781 + 3u}; _temp780( _temp782);}); goto _LL428; _LL462: s=({
struct Cyc_PP_Doc*(* _temp790)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp793= Cyc_PP_text(({ char* _temp795="enum "; struct _tagged_string _temp796;
_temp796.curr= _temp795; _temp796.base= _temp795; _temp796.last_plus_one=
_temp795 + 6; _temp796;})); struct Cyc_PP_Doc* _temp794= Cyc_Absynpp_qvar2doc(
_temp529); struct Cyc_PP_Doc* _temp791[ 2u]={ _temp793, _temp794}; struct
_tagged_ptr0 _temp792={ _temp791, _temp791, _temp791 + 2u}; _temp790( _temp792);});
goto _LL428; _LL464: s=({ struct Cyc_PP_Doc*(* _temp797)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp800= Cyc_Absynpp_qvar2doc( _temp535); struct
Cyc_PP_Doc* _temp801= Cyc_Absynpp_tps2doc( _temp533); struct Cyc_PP_Doc*
_temp798[ 2u]={ _temp800, _temp801}; struct _tagged_ptr0 _temp799={ _temp798,
_temp798, _temp798 + 2u}; _temp797( _temp799);}); goto _LL428; _LL466: s=({
struct Cyc_PP_Doc*(* _temp802)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp805= Cyc_PP_text(({ char* _temp810="region_t<"; struct _tagged_string
_temp811; _temp811.curr= _temp810; _temp811.base= _temp810; _temp811.last_plus_one=
_temp810 + 10; _temp811;})); struct Cyc_PP_Doc* _temp806= Cyc_Absynpp_rgn2doc(
_temp537); struct Cyc_PP_Doc* _temp807= Cyc_PP_text(({ char* _temp808=">";
struct _tagged_string _temp809; _temp809.curr= _temp808; _temp809.base= _temp808;
_temp809.last_plus_one= _temp808 + 2; _temp809;})); struct Cyc_PP_Doc* _temp803[
3u]={ _temp805, _temp806, _temp807}; struct _tagged_ptr0 _temp804={ _temp803,
_temp803, _temp803 + 3u}; _temp802( _temp804);}); goto _LL428; _LL468: s= Cyc_Absynpp_rgn2doc(
t); goto _LL428; _LL470: s= Cyc_Absynpp_eff2doc( t); goto _LL428; _LL472: s= Cyc_Absynpp_eff2doc(
t); goto _LL428; _LL428:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null( vo))->v));} struct
Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt* _temp812=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp812->v=( void*) Cyc_PP_text(*((
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp812;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt){ struct Cyc_List_List* _temp813=((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args); goto _LL814; _LL814: if(
effopt == 0){ if( c_varargs){ return Cyc_PP_group(({ char* _temp815="("; struct
_tagged_string _temp816; _temp816.curr= _temp815; _temp816.base= _temp815;
_temp816.last_plus_one= _temp815 + 2; _temp816;}),({ char* _temp817=")"; struct
_tagged_string _temp818; _temp818.curr= _temp817; _temp818.base= _temp817;
_temp818.last_plus_one= _temp817 + 2; _temp818;}),({ char* _temp819=","; struct
_tagged_string _temp820; _temp820.curr= _temp819; _temp820.base= _temp819;
_temp820.last_plus_one= _temp819 + 2; _temp820;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp813,({
struct Cyc_List_List* _temp821=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp821->hd=( void*) Cyc_PP_text(({ char* _temp822="...";
struct _tagged_string _temp823; _temp823.curr= _temp822; _temp823.base= _temp822;
_temp823.last_plus_one= _temp822 + 4; _temp823;})); _temp821->tl= 0; _temp821;})));}
else{ if( cyc_varargs != 0){ struct Cyc_PP_Doc* _temp825= Cyc_Absynpp_funarg2doc(({
struct _tuple1* _temp824=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp824->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->name;
_temp824->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->tq;
_temp824->f3=( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->type;
_temp824;})); goto _LL826; _LL826: { struct Cyc_PP_Doc* _temp840=({ struct Cyc_PP_Doc*(*
_temp827)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp830= Cyc_PP_text(({
char* _temp838="..."; struct _tagged_string _temp839; _temp839.curr= _temp838;
_temp839.base= _temp838; _temp839.last_plus_one= _temp838 + 4; _temp839;}));
struct Cyc_PP_Doc* _temp831= Cyc_Absynpp_typ2doc(( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->rgn); struct Cyc_PP_Doc* _temp832=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject? Cyc_PP_text(({ char* _temp834=" inject ";
struct _tagged_string _temp835; _temp835.curr= _temp834; _temp835.base= _temp834;
_temp835.last_plus_one= _temp834 + 9; _temp835;})): Cyc_PP_text(({ char*
_temp836=""; struct _tagged_string _temp837; _temp837.curr= _temp836; _temp837.base=
_temp836; _temp837.last_plus_one= _temp836 + 1; _temp837;})); struct Cyc_PP_Doc*
_temp833= _temp825; struct Cyc_PP_Doc* _temp828[ 4u]={ _temp830, _temp831,
_temp832, _temp833}; struct _tagged_ptr0 _temp829={ _temp828, _temp828, _temp828
+ 4u}; _temp827( _temp829);}); goto _LL841; _LL841: return Cyc_PP_group(({ char*
_temp842="("; struct _tagged_string _temp843; _temp843.curr= _temp842; _temp843.base=
_temp842; _temp843.last_plus_one= _temp842 + 2; _temp843;}),({ char* _temp844=")";
struct _tagged_string _temp845; _temp845.curr= _temp844; _temp845.base= _temp844;
_temp845.last_plus_one= _temp844 + 2; _temp845;}),({ char* _temp846=","; struct
_tagged_string _temp847; _temp847.curr= _temp846; _temp847.base= _temp846;
_temp847.last_plus_one= _temp846 + 2; _temp847;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp813,({
struct Cyc_List_List* _temp848=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp848->hd=( void*) _temp840; _temp848->tl= 0;
_temp848;})));}} else{ return Cyc_PP_group(({ char* _temp849="("; struct
_tagged_string _temp850; _temp850.curr= _temp849; _temp850.base= _temp849;
_temp850.last_plus_one= _temp849 + 2; _temp850;}),({ char* _temp851=")"; struct
_tagged_string _temp852; _temp852.curr= _temp851; _temp852.base= _temp851;
_temp852.last_plus_one= _temp851 + 2; _temp852;}),({ char* _temp853=","; struct
_tagged_string _temp854; _temp854.curr= _temp853; _temp854.base= _temp853;
_temp854.last_plus_one= _temp853 + 2; _temp854;}), _temp813);}}} else{ if(
c_varargs){ return Cyc_PP_group(({ char* _temp855="("; struct _tagged_string
_temp856; _temp856.curr= _temp855; _temp856.base= _temp855; _temp856.last_plus_one=
_temp855 + 2; _temp856;}),({ char* _temp857=")"; struct _tagged_string _temp858;
_temp858.curr= _temp857; _temp858.base= _temp857; _temp858.last_plus_one=
_temp857 + 2; _temp858;}),({ char* _temp859=";"; struct _tagged_string _temp860;
_temp860.curr= _temp859; _temp860.base= _temp859; _temp860.last_plus_one=
_temp859 + 2; _temp860;}),({ struct Cyc_List_List* _temp861=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp861->hd=( void*) Cyc_PP_group(({
char* _temp863=""; struct _tagged_string _temp864; _temp864.curr= _temp863;
_temp864.base= _temp863; _temp864.last_plus_one= _temp863 + 1; _temp864;}),({
char* _temp865=""; struct _tagged_string _temp866; _temp866.curr= _temp865;
_temp866.base= _temp865; _temp866.last_plus_one= _temp865 + 1; _temp866;}),({
char* _temp867=","; struct _tagged_string _temp868; _temp868.curr= _temp867;
_temp868.base= _temp867; _temp868.last_plus_one= _temp867 + 2; _temp868;}),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp813,({ struct Cyc_List_List* _temp869=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp869->hd=( void*) Cyc_PP_text(({ char*
_temp870="..."; struct _tagged_string _temp871; _temp871.curr= _temp870;
_temp871.base= _temp870; _temp871.last_plus_one= _temp870 + 4; _temp871;}));
_temp869->tl= 0; _temp869;}))); _temp861->tl=({ struct Cyc_List_List* _temp862=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp862->hd=(
void*) Cyc_Absynpp_eff2doc(( void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v);
_temp862->tl= 0; _temp862;}); _temp861;}));} else{ if( cyc_varargs != 0){ struct
Cyc_PP_Doc* _temp873= Cyc_Absynpp_funarg2doc(({ struct _tuple1* _temp872=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp872->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->name; _temp872->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->tq; _temp872->f3=( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->type; _temp872;})); goto _LL874; _LL874: { struct
Cyc_PP_Doc* _temp888=({ struct Cyc_PP_Doc*(* _temp875)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp878= Cyc_PP_text(({ char* _temp886="...";
struct _tagged_string _temp887; _temp887.curr= _temp886; _temp887.base= _temp886;
_temp887.last_plus_one= _temp886 + 4; _temp887;})); struct Cyc_PP_Doc* _temp879=
Cyc_Absynpp_typ2doc(( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null(
cyc_varargs))->rgn); struct Cyc_PP_Doc* _temp880=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject? Cyc_PP_text(({ char* _temp882=" inject ";
struct _tagged_string _temp883; _temp883.curr= _temp882; _temp883.base= _temp882;
_temp883.last_plus_one= _temp882 + 9; _temp883;})): Cyc_PP_text(({ char*
_temp884=""; struct _tagged_string _temp885; _temp885.curr= _temp884; _temp885.base=
_temp884; _temp885.last_plus_one= _temp884 + 1; _temp885;})); struct Cyc_PP_Doc*
_temp881= _temp873; struct Cyc_PP_Doc* _temp876[ 4u]={ _temp878, _temp879,
_temp880, _temp881}; struct _tagged_ptr0 _temp877={ _temp876, _temp876, _temp876
+ 4u}; _temp875( _temp877);}); goto _LL889; _LL889: return Cyc_PP_group(({ char*
_temp890="("; struct _tagged_string _temp891; _temp891.curr= _temp890; _temp891.base=
_temp890; _temp891.last_plus_one= _temp890 + 2; _temp891;}),({ char* _temp892=")";
struct _tagged_string _temp893; _temp893.curr= _temp892; _temp893.base= _temp892;
_temp893.last_plus_one= _temp892 + 2; _temp893;}),({ char* _temp894=";"; struct
_tagged_string _temp895; _temp895.curr= _temp894; _temp895.base= _temp894;
_temp895.last_plus_one= _temp894 + 2; _temp895;}),({ struct Cyc_List_List*
_temp896=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp896->hd=( void*) Cyc_PP_group(({ char* _temp898=""; struct _tagged_string
_temp899; _temp899.curr= _temp898; _temp899.base= _temp898; _temp899.last_plus_one=
_temp898 + 1; _temp899;}),({ char* _temp900=""; struct _tagged_string _temp901;
_temp901.curr= _temp900; _temp901.base= _temp900; _temp901.last_plus_one=
_temp900 + 1; _temp901;}),({ char* _temp902=","; struct _tagged_string _temp903;
_temp903.curr= _temp902; _temp903.base= _temp902; _temp903.last_plus_one=
_temp902 + 2; _temp903;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp813,({ struct Cyc_List_List*
_temp904=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp904->hd=( void*) _temp888; _temp904->tl= 0; _temp904;}))); _temp896->tl=({
struct Cyc_List_List* _temp897=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp897->hd=( void*) Cyc_Absynpp_eff2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( effopt))->v); _temp897->tl= 0; _temp897;});
_temp896;}));}} else{ return Cyc_PP_group(({ char* _temp905="("; struct
_tagged_string _temp906; _temp906.curr= _temp905; _temp906.base= _temp905;
_temp906.last_plus_one= _temp905 + 2; _temp906;}),({ char* _temp907=")"; struct
_tagged_string _temp908; _temp908.curr= _temp907; _temp908.base= _temp907;
_temp908.last_plus_one= _temp907 + 2; _temp908;}),({ char* _temp909=";"; struct
_tagged_string _temp910; _temp910.curr= _temp909; _temp910.base= _temp909;
_temp910.last_plus_one= _temp909 + 2; _temp910;}),({ struct Cyc_List_List*
_temp911=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp911->hd=( void*) Cyc_PP_group(({ char* _temp913=""; struct _tagged_string
_temp914; _temp914.curr= _temp913; _temp914.base= _temp913; _temp914.last_plus_one=
_temp913 + 1; _temp914;}),({ char* _temp915=""; struct _tagged_string _temp916;
_temp916.curr= _temp915; _temp916.base= _temp915; _temp916.last_plus_one=
_temp915 + 1; _temp916;}),({ char* _temp917=","; struct _tagged_string _temp918;
_temp918.curr= _temp917; _temp918.base= _temp917; _temp918.last_plus_one=
_temp917 + 2; _temp918;}), _temp813); _temp911->tl=({ struct Cyc_List_List*
_temp912=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp912->hd=( void*) Cyc_Absynpp_eff2doc(( void*)(( struct Cyc_Core_Opt*)
_check_null( effopt))->v); _temp912->tl= 0; _temp912;}); _temp911;}));}}}}
struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg){ return({ struct
_tuple1* _temp919=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp919->f1=({ struct Cyc_Core_Opt* _temp920=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp920->v=( void*)(* arg).f1; _temp920;});
_temp919->f2=(* arg).f2; _temp919->f3=(* arg).f3; _temp919;});} struct Cyc_PP_Doc*
Cyc_Absynpp_var2doc( struct _tagged_string* v){ return Cyc_PP_text(* v);} struct
Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
_temp921= 0; goto _LL922; _LL922:{ void* _temp923=(* q).f1; struct Cyc_List_List*
_temp935; struct Cyc_List_List* _temp937; struct Cyc_List_List* _temp939; struct
Cyc_List_List* _temp941; _LL925: if( _temp923 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL926;} else{ goto _LL927;} _LL927: if(( unsigned int) _temp923 > 1u?*(( int*)
_temp923) == Cyc_Absyn_Rel_n: 0){ _LL936: _temp935=(( struct Cyc_Absyn_Rel_n_struct*)
_temp923)->f1; if( _temp935 == 0){ goto _LL928;} else{ goto _LL929;}} else{ goto
_LL929;} _LL929: if(( unsigned int) _temp923 > 1u?*(( int*) _temp923) == Cyc_Absyn_Abs_n:
0){ _LL938: _temp937=(( struct Cyc_Absyn_Abs_n_struct*) _temp923)->f1; if(
_temp937 == 0){ goto _LL930;} else{ goto _LL931;}} else{ goto _LL931;} _LL931:
if(( unsigned int) _temp923 > 1u?*(( int*) _temp923) == Cyc_Absyn_Rel_n: 0){
_LL940: _temp939=(( struct Cyc_Absyn_Rel_n_struct*) _temp923)->f1; goto _LL932;}
else{ goto _LL933;} _LL933: if(( unsigned int) _temp923 > 1u?*(( int*) _temp923)
== Cyc_Absyn_Abs_n: 0){ _LL942: _temp941=(( struct Cyc_Absyn_Abs_n_struct*)
_temp923)->f1; goto _LL934;} else{ goto _LL924;} _LL926: goto _LL928; _LL928:
goto _LL930; _LL930: return Cyc_Absynpp_var2doc((* q).f2); _LL932: _temp941=
_temp939; goto _LL934; _LL934: _temp921= _temp941; goto _LL924; _LL924:;} if(
Cyc_Absynpp_qvar_to_Cids){ return Cyc_PP_text( Cyc_String_str_sepstr((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp921,({ struct Cyc_List_List* _temp943=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp943->hd=( void*)(* q).f2; _temp943->tl= 0;
_temp943;})),({ char* _temp944="_"; struct _tagged_string _temp945; _temp945.curr=
_temp944; _temp945.base= _temp944; _temp945.last_plus_one= _temp944 + 2;
_temp945;})));} else{ if((( int(*)( int(* cmp)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_prefix)(
Cyc_String_strptrcmp, _temp921, Cyc_Absynpp_curr_namespace)){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp921,({
struct Cyc_List_List* _temp946=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp946->hd=( void*)(* q).f2; _temp946->tl= 0; _temp946;})),({
char* _temp947="::"; struct _tagged_string _temp948; _temp948.curr= _temp947;
_temp948.base= _temp947; _temp948.last_plus_one= _temp947 + 3; _temp948;})));}}}
struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);}{ void* _temp949=(* v).f1; struct Cyc_List_List*
_temp955; _LL951: if(( unsigned int) _temp949 > 1u?*(( int*) _temp949) == Cyc_Absyn_Abs_n:
0){ _LL956: _temp955=(( struct Cyc_Absyn_Abs_n_struct*) _temp949)->f1; goto
_LL952;} else{ goto _LL953;} _LL953: goto _LL954; _LL952: if((( int(*)( int(*
cmp)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp955,
Cyc_Absynpp_curr_namespace) == 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{
return({ struct Cyc_PP_Doc*(* _temp957)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp960= Cyc_PP_text(({ char* _temp962="/* wrong namespace : */ ";
struct _tagged_string _temp963; _temp963.curr= _temp962; _temp963.base= _temp962;
_temp963.last_plus_one= _temp962 + 25; _temp963;})); struct Cyc_PP_Doc* _temp961=
Cyc_Absynpp_qvar2doc( v); struct Cyc_PP_Doc* _temp958[ 2u]={ _temp960, _temp961};
struct _tagged_ptr0 _temp959={ _temp958, _temp958, _temp958 + 2u}; _temp957(
_temp959);});} _LL954: return Cyc_Absynpp_qvar2doc( v); _LL950:;}} struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(),
t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp* e){ void* _temp964=(
void*) e->r; void* _temp1036; void* _temp1038; struct _tuple0* _temp1040; struct
_tuple0* _temp1042; struct Cyc_List_List* _temp1044; void* _temp1046; struct Cyc_Absyn_Exp*
_temp1048; struct Cyc_Core_Opt* _temp1050; struct Cyc_Absyn_Exp* _temp1052; void*
_temp1054; struct Cyc_Absyn_Exp* _temp1056; struct Cyc_Absyn_Exp* _temp1058;
struct Cyc_Absyn_Exp* _temp1060; struct Cyc_Absyn_Exp* _temp1062; struct Cyc_Absyn_Exp*
_temp1064; struct Cyc_Absyn_Exp* _temp1066; struct Cyc_List_List* _temp1068;
struct Cyc_Absyn_Exp* _temp1070; struct Cyc_Absyn_VarargCallInfo* _temp1072;
struct Cyc_List_List* _temp1074; struct Cyc_Absyn_Exp* _temp1076; struct Cyc_Absyn_Exp*
_temp1078; struct Cyc_Absyn_Exp* _temp1080; struct Cyc_List_List* _temp1082;
struct Cyc_Absyn_Exp* _temp1084; struct Cyc_Absyn_Exp* _temp1086; void*
_temp1088; struct Cyc_Absyn_Exp* _temp1090; struct Cyc_Absyn_Exp* _temp1092;
struct Cyc_Absyn_Exp* _temp1094; void* _temp1096; struct Cyc_Absyn_Exp*
_temp1098; struct Cyc_Absyn_Exp* _temp1100; struct _tagged_string* _temp1102;
struct Cyc_Absyn_Exp* _temp1104; struct _tagged_string* _temp1106; struct Cyc_Absyn_Exp*
_temp1108; struct Cyc_Absyn_Exp* _temp1110; struct Cyc_Absyn_Exp* _temp1112;
struct Cyc_List_List* _temp1114; struct Cyc_List_List* _temp1116; struct _tuple1*
_temp1118; struct Cyc_List_List* _temp1120; struct Cyc_Absyn_Exp* _temp1122;
struct Cyc_Absyn_Exp* _temp1124; struct Cyc_Absyn_Vardecl* _temp1126; struct Cyc_Absyn_Structdecl*
_temp1128; struct Cyc_List_List* _temp1130; struct Cyc_Core_Opt* _temp1132;
struct _tuple0* _temp1134; struct Cyc_List_List* _temp1136; void* _temp1138;
struct Cyc_Absyn_Tunionfield* _temp1140; struct Cyc_Absyn_Tuniondecl* _temp1142;
struct Cyc_List_List* _temp1144; struct Cyc_Core_Opt* _temp1146; struct Cyc_Core_Opt*
_temp1148; struct Cyc_Absyn_Enumfield* _temp1150; struct Cyc_Absyn_Enumdecl*
_temp1152; struct _tuple0* _temp1154; void* _temp1156; struct Cyc_Absyn_Exp*
_temp1158; struct Cyc_List_List* _temp1160; struct Cyc_Core_Opt* _temp1162;
struct Cyc_Absyn_Stmt* _temp1164; struct Cyc_Absyn_Fndecl* _temp1166; struct Cyc_Absyn_Exp*
_temp1168; _LL966: if(*(( int*) _temp964) == Cyc_Absyn_Const_e){ _LL1037:
_temp1036=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp964)->f1; goto
_LL967;} else{ goto _LL968;} _LL968: if(*(( int*) _temp964) == Cyc_Absyn_Var_e){
_LL1041: _temp1040=(( struct Cyc_Absyn_Var_e_struct*) _temp964)->f1; goto
_LL1039; _LL1039: _temp1038=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp964)->f2;
goto _LL969;} else{ goto _LL970;} _LL970: if(*(( int*) _temp964) == Cyc_Absyn_UnknownId_e){
_LL1043: _temp1042=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp964)->f1; goto
_LL971;} else{ goto _LL972;} _LL972: if(*(( int*) _temp964) == Cyc_Absyn_Primop_e){
_LL1047: _temp1046=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp964)->f1;
goto _LL1045; _LL1045: _temp1044=(( struct Cyc_Absyn_Primop_e_struct*) _temp964)->f2;
goto _LL973;} else{ goto _LL974;} _LL974: if(*(( int*) _temp964) == Cyc_Absyn_AssignOp_e){
_LL1053: _temp1052=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp964)->f1; goto
_LL1051; _LL1051: _temp1050=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp964)->f2;
goto _LL1049; _LL1049: _temp1048=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp964)->f3; goto _LL975;} else{ goto _LL976;} _LL976: if(*(( int*) _temp964)
== Cyc_Absyn_Increment_e){ _LL1057: _temp1056=(( struct Cyc_Absyn_Increment_e_struct*)
_temp964)->f1; goto _LL1055; _LL1055: _temp1054=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp964)->f2; goto _LL977;} else{ goto _LL978;} _LL978: if(*(( int*) _temp964)
== Cyc_Absyn_Conditional_e){ _LL1063: _temp1062=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp964)->f1; goto _LL1061; _LL1061: _temp1060=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp964)->f2; goto _LL1059; _LL1059: _temp1058=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp964)->f3; goto _LL979;} else{ goto _LL980;} _LL980: if(*(( int*) _temp964)
== Cyc_Absyn_SeqExp_e){ _LL1067: _temp1066=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp964)->f1; goto _LL1065; _LL1065: _temp1064=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp964)->f2; goto _LL981;} else{ goto _LL982;} _LL982: if(*(( int*) _temp964)
== Cyc_Absyn_UnknownCall_e){ _LL1071: _temp1070=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp964)->f1; goto _LL1069; _LL1069: _temp1068=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp964)->f2; goto _LL983;} else{ goto _LL984;} _LL984: if(*(( int*) _temp964)
== Cyc_Absyn_FnCall_e){ _LL1077: _temp1076=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp964)->f1; goto _LL1075; _LL1075: _temp1074=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp964)->f2; goto _LL1073; _LL1073: _temp1072=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp964)->f3; goto _LL985;} else{ goto _LL986;} _LL986: if(*(( int*) _temp964)
== Cyc_Absyn_Throw_e){ _LL1079: _temp1078=(( struct Cyc_Absyn_Throw_e_struct*)
_temp964)->f1; goto _LL987;} else{ goto _LL988;} _LL988: if(*(( int*) _temp964)
== Cyc_Absyn_NoInstantiate_e){ _LL1081: _temp1080=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp964)->f1; goto _LL989;} else{ goto _LL990;} _LL990: if(*(( int*) _temp964)
== Cyc_Absyn_Instantiate_e){ _LL1085: _temp1084=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp964)->f1; goto _LL1083; _LL1083: _temp1082=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp964)->f2; goto _LL991;} else{ goto _LL992;} _LL992: if(*(( int*) _temp964)
== Cyc_Absyn_Cast_e){ _LL1089: _temp1088=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp964)->f1; goto _LL1087; _LL1087: _temp1086=(( struct Cyc_Absyn_Cast_e_struct*)
_temp964)->f2; goto _LL993;} else{ goto _LL994;} _LL994: if(*(( int*) _temp964)
== Cyc_Absyn_New_e){ _LL1093: _temp1092=(( struct Cyc_Absyn_New_e_struct*)
_temp964)->f1; goto _LL1091; _LL1091: _temp1090=(( struct Cyc_Absyn_New_e_struct*)
_temp964)->f2; goto _LL995;} else{ goto _LL996;} _LL996: if(*(( int*) _temp964)
== Cyc_Absyn_Address_e){ _LL1095: _temp1094=(( struct Cyc_Absyn_Address_e_struct*)
_temp964)->f1; goto _LL997;} else{ goto _LL998;} _LL998: if(*(( int*) _temp964)
== Cyc_Absyn_Sizeoftyp_e){ _LL1097: _temp1096=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp964)->f1; goto _LL999;} else{ goto _LL1000;} _LL1000: if(*(( int*) _temp964)
== Cyc_Absyn_Sizeofexp_e){ _LL1099: _temp1098=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp964)->f1; goto _LL1001;} else{ goto _LL1002;} _LL1002: if(*(( int*)
_temp964) == Cyc_Absyn_Deref_e){ _LL1101: _temp1100=(( struct Cyc_Absyn_Deref_e_struct*)
_temp964)->f1; goto _LL1003;} else{ goto _LL1004;} _LL1004: if(*(( int*)
_temp964) == Cyc_Absyn_StructMember_e){ _LL1105: _temp1104=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp964)->f1; goto _LL1103; _LL1103: _temp1102=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp964)->f2; goto _LL1005;} else{ goto _LL1006;} _LL1006: if(*(( int*)
_temp964) == Cyc_Absyn_StructArrow_e){ _LL1109: _temp1108=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp964)->f1; goto _LL1107; _LL1107: _temp1106=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp964)->f2; goto _LL1007;} else{ goto _LL1008;} _LL1008: if(*(( int*)
_temp964) == Cyc_Absyn_Subscript_e){ _LL1113: _temp1112=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp964)->f1; goto _LL1111; _LL1111: _temp1110=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp964)->f2; goto _LL1009;} else{ goto _LL1010;} _LL1010: if(*(( int*)
_temp964) == Cyc_Absyn_Tuple_e){ _LL1115: _temp1114=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp964)->f1; goto _LL1011;} else{ goto _LL1012;} _LL1012: if(*(( int*)
_temp964) == Cyc_Absyn_CompoundLit_e){ _LL1119: _temp1118=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp964)->f1; goto _LL1117; _LL1117: _temp1116=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp964)->f2; goto _LL1013;} else{ goto _LL1014;} _LL1014: if(*(( int*)
_temp964) == Cyc_Absyn_Array_e){ _LL1121: _temp1120=(( struct Cyc_Absyn_Array_e_struct*)
_temp964)->f1; goto _LL1015;} else{ goto _LL1016;} _LL1016: if(*(( int*)
_temp964) == Cyc_Absyn_Comprehension_e){ _LL1127: _temp1126=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp964)->f1; goto _LL1125; _LL1125: _temp1124=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp964)->f2; goto _LL1123; _LL1123: _temp1122=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp964)->f3; goto _LL1017;} else{ goto _LL1018;} _LL1018: if(*(( int*)
_temp964) == Cyc_Absyn_Struct_e){ _LL1135: _temp1134=(( struct Cyc_Absyn_Struct_e_struct*)
_temp964)->f1; goto _LL1133; _LL1133: _temp1132=(( struct Cyc_Absyn_Struct_e_struct*)
_temp964)->f2; goto _LL1131; _LL1131: _temp1130=(( struct Cyc_Absyn_Struct_e_struct*)
_temp964)->f3; goto _LL1129; _LL1129: _temp1128=(( struct Cyc_Absyn_Struct_e_struct*)
_temp964)->f4; goto _LL1019;} else{ goto _LL1020;} _LL1020: if(*(( int*)
_temp964) == Cyc_Absyn_AnonStruct_e){ _LL1139: _temp1138=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp964)->f1; goto _LL1137; _LL1137: _temp1136=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp964)->f2; goto _LL1021;} else{ goto _LL1022;} _LL1022: if(*(( int*)
_temp964) == Cyc_Absyn_Tunion_e){ _LL1149: _temp1148=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp964)->f1; goto _LL1147; _LL1147: _temp1146=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp964)->f2; goto _LL1145; _LL1145: _temp1144=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp964)->f3; goto _LL1143; _LL1143: _temp1142=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp964)->f4; goto _LL1141; _LL1141: _temp1140=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp964)->f5; goto _LL1023;} else{ goto _LL1024;} _LL1024: if(*(( int*)
_temp964) == Cyc_Absyn_Enum_e){ _LL1155: _temp1154=(( struct Cyc_Absyn_Enum_e_struct*)
_temp964)->f1; goto _LL1153; _LL1153: _temp1152=(( struct Cyc_Absyn_Enum_e_struct*)
_temp964)->f2; goto _LL1151; _LL1151: _temp1150=(( struct Cyc_Absyn_Enum_e_struct*)
_temp964)->f3; goto _LL1025;} else{ goto _LL1026;} _LL1026: if(*(( int*)
_temp964) == Cyc_Absyn_Malloc_e){ _LL1159: _temp1158=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp964)->f1; goto _LL1157; _LL1157: _temp1156=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp964)->f2; goto _LL1027;} else{ goto _LL1028;} _LL1028: if(*(( int*)
_temp964) == Cyc_Absyn_UnresolvedMem_e){ _LL1163: _temp1162=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp964)->f1; goto _LL1161; _LL1161: _temp1160=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp964)->f2; goto _LL1029;} else{ goto _LL1030;} _LL1030: if(*(( int*)
_temp964) == Cyc_Absyn_StmtExp_e){ _LL1165: _temp1164=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp964)->f1; goto _LL1031;} else{ goto _LL1032;} _LL1032: if(*(( int*)
_temp964) == Cyc_Absyn_Codegen_e){ _LL1167: _temp1166=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp964)->f1; goto _LL1033;} else{ goto _LL1034;} _LL1034: if(*(( int*)
_temp964) == Cyc_Absyn_Fill_e){ _LL1169: _temp1168=(( struct Cyc_Absyn_Fill_e_struct*)
_temp964)->f1; goto _LL1035;} else{ goto _LL965;} _LL967: return 10000; _LL969:
return 10000; _LL971: return 10000; _LL973: { void* _temp1170= _temp1046;
_LL1172: if( _temp1170 ==( void*) Cyc_Absyn_Plus){ goto _LL1173;} else{ goto
_LL1174;} _LL1174: if( _temp1170 ==( void*) Cyc_Absyn_Times){ goto _LL1175;}
else{ goto _LL1176;} _LL1176: if( _temp1170 ==( void*) Cyc_Absyn_Minus){ goto
_LL1177;} else{ goto _LL1178;} _LL1178: if( _temp1170 ==( void*) Cyc_Absyn_Div){
goto _LL1179;} else{ goto _LL1180;} _LL1180: if( _temp1170 ==( void*) Cyc_Absyn_Mod){
goto _LL1181;} else{ goto _LL1182;} _LL1182: if( _temp1170 ==( void*) Cyc_Absyn_Eq){
goto _LL1183;} else{ goto _LL1184;} _LL1184: if( _temp1170 ==( void*) Cyc_Absyn_Neq){
goto _LL1185;} else{ goto _LL1186;} _LL1186: if( _temp1170 ==( void*) Cyc_Absyn_Gt){
goto _LL1187;} else{ goto _LL1188;} _LL1188: if( _temp1170 ==( void*) Cyc_Absyn_Lt){
goto _LL1189;} else{ goto _LL1190;} _LL1190: if( _temp1170 ==( void*) Cyc_Absyn_Gte){
goto _LL1191;} else{ goto _LL1192;} _LL1192: if( _temp1170 ==( void*) Cyc_Absyn_Lte){
goto _LL1193;} else{ goto _LL1194;} _LL1194: if( _temp1170 ==( void*) Cyc_Absyn_Not){
goto _LL1195;} else{ goto _LL1196;} _LL1196: if( _temp1170 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1197;} else{ goto _LL1198;} _LL1198: if( _temp1170 ==( void*) Cyc_Absyn_Bitand){
goto _LL1199;} else{ goto _LL1200;} _LL1200: if( _temp1170 ==( void*) Cyc_Absyn_Bitor){
goto _LL1201;} else{ goto _LL1202;} _LL1202: if( _temp1170 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1203;} else{ goto _LL1204;} _LL1204: if( _temp1170 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1205;} else{ goto _LL1206;} _LL1206: if( _temp1170 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1207;} else{ goto _LL1208;} _LL1208: if( _temp1170 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1209;} else{ goto _LL1210;} _LL1210: if( _temp1170 ==( void*) Cyc_Absyn_Size){
goto _LL1211;} else{ goto _LL1212;} _LL1212: if( _temp1170 ==( void*) Cyc_Absyn_Printf){
goto _LL1213;} else{ goto _LL1214;} _LL1214: if( _temp1170 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1215;} else{ goto _LL1216;} _LL1216: if( _temp1170 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1217;} else{ goto _LL1218;} _LL1218: if( _temp1170 ==( void*) Cyc_Absyn_Scanf){
goto _LL1219;} else{ goto _LL1220;} _LL1220: if( _temp1170 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1221;} else{ goto _LL1222;} _LL1222: if( _temp1170 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1223;} else{ goto _LL1171;} _LL1173: return 100; _LL1175: return 110;
_LL1177: return 100; _LL1179: return 110; _LL1181: return 110; _LL1183: return
70; _LL1185: return 70; _LL1187: return 80; _LL1189: return 80; _LL1191: return
80; _LL1193: return 80; _LL1195: return 130; _LL1197: return 130; _LL1199:
return 60; _LL1201: return 40; _LL1203: return 50; _LL1205: return 90; _LL1207:
return 80; _LL1209: return 80; _LL1211: return 140; _LL1213: return 130; _LL1215:
return 130; _LL1217: return 130; _LL1219: return 130; _LL1221: return 130;
_LL1223: return 130; _LL1171:;} _LL975: return 20; _LL977: return 130; _LL979:
return 30; _LL981: return 10; _LL983: return 140; _LL985: return 140; _LL987:
return 130; _LL989: return Cyc_Absynpp_exp_prec( _temp1080); _LL991: return Cyc_Absynpp_exp_prec(
_temp1084); _LL993: return 120; _LL995: return 15; _LL997: return 130; _LL999:
return 130; _LL1001: return 130; _LL1003: return 130; _LL1005: return 140;
_LL1007: return 140; _LL1009: return 140; _LL1011: return 150; _LL1013: goto
_LL1015; _LL1015: goto _LL1017; _LL1017: goto _LL1019; _LL1019: goto _LL1021;
_LL1021: goto _LL1023; _LL1023: goto _LL1025; _LL1025: goto _LL1027; _LL1027:
return 140; _LL1029: return 140; _LL1031: return 10000; _LL1033: return 140;
_LL1035: return 140; _LL965:;} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct
Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec( 0, e);} struct _tuple8{ void*
f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec,
struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc*
s;{ void* _temp1224=( void*) e->r; void* _temp1296; void* _temp1298; struct
_tuple0* _temp1300; struct _tuple0* _temp1302; struct Cyc_List_List* _temp1304;
void* _temp1306; struct Cyc_Absyn_Exp* _temp1308; struct Cyc_Core_Opt* _temp1310;
struct Cyc_Absyn_Exp* _temp1312; void* _temp1314; struct Cyc_Absyn_Exp*
_temp1316; struct Cyc_Absyn_Exp* _temp1318; struct Cyc_Absyn_Exp* _temp1320;
struct Cyc_Absyn_Exp* _temp1322; struct Cyc_Absyn_Exp* _temp1324; struct Cyc_Absyn_Exp*
_temp1326; struct Cyc_List_List* _temp1328; struct Cyc_Absyn_Exp* _temp1330;
struct Cyc_Absyn_VarargCallInfo* _temp1332; struct Cyc_List_List* _temp1334;
struct Cyc_Absyn_Exp* _temp1336; struct Cyc_Absyn_Exp* _temp1338; struct Cyc_Absyn_Exp*
_temp1340; struct Cyc_List_List* _temp1342; struct Cyc_Absyn_Exp* _temp1344;
struct Cyc_Absyn_Exp* _temp1346; void* _temp1348; struct Cyc_Absyn_Exp*
_temp1350; struct Cyc_Absyn_Exp* _temp1352; struct Cyc_Absyn_Exp* _temp1354;
void* _temp1356; struct Cyc_Absyn_Exp* _temp1358; struct Cyc_Absyn_Exp*
_temp1360; struct _tagged_string* _temp1362; struct Cyc_Absyn_Exp* _temp1364;
struct _tagged_string* _temp1366; struct Cyc_Absyn_Exp* _temp1368; struct Cyc_Absyn_Exp*
_temp1370; struct Cyc_Absyn_Exp* _temp1372; struct Cyc_List_List* _temp1374;
struct Cyc_List_List* _temp1376; struct _tuple1* _temp1378; struct Cyc_List_List*
_temp1380; struct Cyc_Absyn_Exp* _temp1382; struct Cyc_Absyn_Exp* _temp1384;
struct Cyc_Absyn_Vardecl* _temp1386; struct Cyc_Absyn_Structdecl* _temp1388;
struct Cyc_List_List* _temp1390; struct Cyc_Core_Opt* _temp1392; struct _tuple0*
_temp1394; struct Cyc_List_List* _temp1396; void* _temp1398; struct Cyc_Absyn_Tunionfield*
_temp1400; struct Cyc_Absyn_Tuniondecl* _temp1402; struct Cyc_List_List*
_temp1404; struct Cyc_Core_Opt* _temp1406; struct Cyc_Core_Opt* _temp1408;
struct Cyc_Absyn_Enumfield* _temp1410; struct Cyc_Absyn_Enumdecl* _temp1412;
struct _tuple0* _temp1414; void* _temp1416; struct Cyc_Absyn_Exp* _temp1418;
struct Cyc_List_List* _temp1420; struct Cyc_Core_Opt* _temp1422; struct Cyc_Absyn_Stmt*
_temp1424; struct Cyc_Absyn_Fndecl* _temp1426; struct Cyc_Absyn_Exp* _temp1428;
_LL1226: if(*(( int*) _temp1224) == Cyc_Absyn_Const_e){ _LL1297: _temp1296=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1224)->f1; goto _LL1227;} else{
goto _LL1228;} _LL1228: if(*(( int*) _temp1224) == Cyc_Absyn_Var_e){ _LL1301:
_temp1300=(( struct Cyc_Absyn_Var_e_struct*) _temp1224)->f1; goto _LL1299;
_LL1299: _temp1298=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1224)->f2;
goto _LL1229;} else{ goto _LL1230;} _LL1230: if(*(( int*) _temp1224) == Cyc_Absyn_UnknownId_e){
_LL1303: _temp1302=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1224)->f1; goto
_LL1231;} else{ goto _LL1232;} _LL1232: if(*(( int*) _temp1224) == Cyc_Absyn_Primop_e){
_LL1307: _temp1306=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1224)->f1;
goto _LL1305; _LL1305: _temp1304=(( struct Cyc_Absyn_Primop_e_struct*) _temp1224)->f2;
goto _LL1233;} else{ goto _LL1234;} _LL1234: if(*(( int*) _temp1224) == Cyc_Absyn_AssignOp_e){
_LL1313: _temp1312=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1224)->f1; goto
_LL1311; _LL1311: _temp1310=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1224)->f2;
goto _LL1309; _LL1309: _temp1308=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1224)->f3; goto _LL1235;} else{ goto _LL1236;} _LL1236: if(*(( int*)
_temp1224) == Cyc_Absyn_Increment_e){ _LL1317: _temp1316=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1224)->f1; goto _LL1315; _LL1315: _temp1314=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1224)->f2; goto _LL1237;} else{ goto _LL1238;} _LL1238: if(*(( int*)
_temp1224) == Cyc_Absyn_Conditional_e){ _LL1323: _temp1322=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1224)->f1; goto _LL1321; _LL1321: _temp1320=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1224)->f2; goto _LL1319; _LL1319: _temp1318=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1224)->f3; goto _LL1239;} else{ goto _LL1240;} _LL1240: if(*(( int*)
_temp1224) == Cyc_Absyn_SeqExp_e){ _LL1327: _temp1326=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1224)->f1; goto _LL1325; _LL1325: _temp1324=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1224)->f2; goto _LL1241;} else{ goto _LL1242;} _LL1242: if(*(( int*)
_temp1224) == Cyc_Absyn_UnknownCall_e){ _LL1331: _temp1330=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1224)->f1; goto _LL1329; _LL1329: _temp1328=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1224)->f2; goto _LL1243;} else{ goto _LL1244;} _LL1244: if(*(( int*)
_temp1224) == Cyc_Absyn_FnCall_e){ _LL1337: _temp1336=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1224)->f1; goto _LL1335; _LL1335: _temp1334=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1224)->f2; goto _LL1333; _LL1333: _temp1332=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1224)->f3; goto _LL1245;} else{ goto _LL1246;} _LL1246: if(*(( int*)
_temp1224) == Cyc_Absyn_Throw_e){ _LL1339: _temp1338=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1224)->f1; goto _LL1247;} else{ goto _LL1248;} _LL1248: if(*(( int*)
_temp1224) == Cyc_Absyn_NoInstantiate_e){ _LL1341: _temp1340=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1224)->f1; goto _LL1249;} else{ goto _LL1250;} _LL1250: if(*(( int*)
_temp1224) == Cyc_Absyn_Instantiate_e){ _LL1345: _temp1344=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1224)->f1; goto _LL1343; _LL1343: _temp1342=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1224)->f2; goto _LL1251;} else{ goto _LL1252;} _LL1252: if(*(( int*)
_temp1224) == Cyc_Absyn_Cast_e){ _LL1349: _temp1348=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1224)->f1; goto _LL1347; _LL1347: _temp1346=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1224)->f2; goto _LL1253;} else{ goto _LL1254;} _LL1254: if(*(( int*)
_temp1224) == Cyc_Absyn_Address_e){ _LL1351: _temp1350=(( struct Cyc_Absyn_Address_e_struct*)
_temp1224)->f1; goto _LL1255;} else{ goto _LL1256;} _LL1256: if(*(( int*)
_temp1224) == Cyc_Absyn_New_e){ _LL1355: _temp1354=(( struct Cyc_Absyn_New_e_struct*)
_temp1224)->f1; goto _LL1353; _LL1353: _temp1352=(( struct Cyc_Absyn_New_e_struct*)
_temp1224)->f2; goto _LL1257;} else{ goto _LL1258;} _LL1258: if(*(( int*)
_temp1224) == Cyc_Absyn_Sizeoftyp_e){ _LL1357: _temp1356=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1224)->f1; goto _LL1259;} else{ goto _LL1260;} _LL1260: if(*(( int*)
_temp1224) == Cyc_Absyn_Sizeofexp_e){ _LL1359: _temp1358=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1224)->f1; goto _LL1261;} else{ goto _LL1262;} _LL1262: if(*(( int*)
_temp1224) == Cyc_Absyn_Deref_e){ _LL1361: _temp1360=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1224)->f1; goto _LL1263;} else{ goto _LL1264;} _LL1264: if(*(( int*)
_temp1224) == Cyc_Absyn_StructMember_e){ _LL1365: _temp1364=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1224)->f1; goto _LL1363; _LL1363: _temp1362=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1224)->f2; goto _LL1265;} else{ goto _LL1266;} _LL1266: if(*(( int*)
_temp1224) == Cyc_Absyn_StructArrow_e){ _LL1369: _temp1368=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1224)->f1; goto _LL1367; _LL1367: _temp1366=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1224)->f2; goto _LL1267;} else{ goto _LL1268;} _LL1268: if(*(( int*)
_temp1224) == Cyc_Absyn_Subscript_e){ _LL1373: _temp1372=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1224)->f1; goto _LL1371; _LL1371: _temp1370=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1224)->f2; goto _LL1269;} else{ goto _LL1270;} _LL1270: if(*(( int*)
_temp1224) == Cyc_Absyn_Tuple_e){ _LL1375: _temp1374=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1224)->f1; goto _LL1271;} else{ goto _LL1272;} _LL1272: if(*(( int*)
_temp1224) == Cyc_Absyn_CompoundLit_e){ _LL1379: _temp1378=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1224)->f1; goto _LL1377; _LL1377: _temp1376=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1224)->f2; goto _LL1273;} else{ goto _LL1274;} _LL1274: if(*(( int*)
_temp1224) == Cyc_Absyn_Array_e){ _LL1381: _temp1380=(( struct Cyc_Absyn_Array_e_struct*)
_temp1224)->f1; goto _LL1275;} else{ goto _LL1276;} _LL1276: if(*(( int*)
_temp1224) == Cyc_Absyn_Comprehension_e){ _LL1387: _temp1386=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1224)->f1; goto _LL1385; _LL1385: _temp1384=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1224)->f2; goto _LL1383; _LL1383: _temp1382=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1224)->f3; goto _LL1277;} else{ goto _LL1278;} _LL1278: if(*(( int*)
_temp1224) == Cyc_Absyn_Struct_e){ _LL1395: _temp1394=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1224)->f1; goto _LL1393; _LL1393: _temp1392=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1224)->f2; goto _LL1391; _LL1391: _temp1390=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1224)->f3; goto _LL1389; _LL1389: _temp1388=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1224)->f4; goto _LL1279;} else{ goto _LL1280;} _LL1280: if(*(( int*)
_temp1224) == Cyc_Absyn_AnonStruct_e){ _LL1399: _temp1398=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1224)->f1; goto _LL1397; _LL1397: _temp1396=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1224)->f2; goto _LL1281;} else{ goto _LL1282;} _LL1282: if(*(( int*)
_temp1224) == Cyc_Absyn_Tunion_e){ _LL1409: _temp1408=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1224)->f1; goto _LL1407; _LL1407: _temp1406=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1224)->f2; goto _LL1405; _LL1405: _temp1404=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1224)->f3; goto _LL1403; _LL1403: _temp1402=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1224)->f4; goto _LL1401; _LL1401: _temp1400=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1224)->f5; goto _LL1283;} else{ goto _LL1284;} _LL1284: if(*(( int*)
_temp1224) == Cyc_Absyn_Enum_e){ _LL1415: _temp1414=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1224)->f1; goto _LL1413; _LL1413: _temp1412=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1224)->f2; goto _LL1411; _LL1411: _temp1410=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1224)->f3; goto _LL1285;} else{ goto _LL1286;} _LL1286: if(*(( int*)
_temp1224) == Cyc_Absyn_Malloc_e){ _LL1419: _temp1418=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1224)->f1; goto _LL1417; _LL1417: _temp1416=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1224)->f2; goto _LL1287;} else{ goto _LL1288;} _LL1288: if(*(( int*)
_temp1224) == Cyc_Absyn_UnresolvedMem_e){ _LL1423: _temp1422=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1224)->f1; goto _LL1421; _LL1421: _temp1420=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1224)->f2; goto _LL1289;} else{ goto _LL1290;} _LL1290: if(*(( int*)
_temp1224) == Cyc_Absyn_StmtExp_e){ _LL1425: _temp1424=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1224)->f1; goto _LL1291;} else{ goto _LL1292;} _LL1292: if(*(( int*)
_temp1224) == Cyc_Absyn_Codegen_e){ _LL1427: _temp1426=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1224)->f1; goto _LL1293;} else{ goto _LL1294;} _LL1294: if(*(( int*)
_temp1224) == Cyc_Absyn_Fill_e){ _LL1429: _temp1428=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1224)->f1; goto _LL1295;} else{ goto _LL1225;} _LL1227: s= Cyc_Absynpp_cnst2doc(
_temp1296); goto _LL1225; _LL1229: _temp1302= _temp1300; goto _LL1231; _LL1231:
s= Cyc_Absynpp_qvar2doc( _temp1302); goto _LL1225; _LL1233: s= Cyc_Absynpp_primapp2doc(
myprec, _temp1306, _temp1304); goto _LL1225; _LL1235: s=({ struct Cyc_PP_Doc*(*
_temp1430)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1433= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1312); struct Cyc_PP_Doc* _temp1434= Cyc_PP_text(({ char* _temp1442=" ";
struct _tagged_string _temp1443; _temp1443.curr= _temp1442; _temp1443.base=
_temp1442; _temp1443.last_plus_one= _temp1442 + 2; _temp1443;})); struct Cyc_PP_Doc*
_temp1435= _temp1310 == 0? Cyc_PP_text(({ char* _temp1440=""; struct
_tagged_string _temp1441; _temp1441.curr= _temp1440; _temp1441.base= _temp1440;
_temp1441.last_plus_one= _temp1440 + 1; _temp1441;})): Cyc_Absynpp_prim2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1310))->v); struct Cyc_PP_Doc*
_temp1436= Cyc_PP_text(({ char* _temp1438="= "; struct _tagged_string _temp1439;
_temp1439.curr= _temp1438; _temp1439.base= _temp1438; _temp1439.last_plus_one=
_temp1438 + 3; _temp1439;})); struct Cyc_PP_Doc* _temp1437= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1308); struct Cyc_PP_Doc* _temp1431[ 5u]={ _temp1433, _temp1434,
_temp1435, _temp1436, _temp1437}; struct _tagged_ptr0 _temp1432={ _temp1431,
_temp1431, _temp1431 + 5u}; _temp1430( _temp1432);}); goto _LL1225; _LL1237: {
struct Cyc_PP_Doc* _temp1444= Cyc_Absynpp_exp2doc_prec( myprec, _temp1316); goto
_LL1445; _LL1445:{ void* _temp1446= _temp1314; _LL1448: if( _temp1446 ==( void*)
Cyc_Absyn_PreInc){ goto _LL1449;} else{ goto _LL1450;} _LL1450: if( _temp1446 ==(
void*) Cyc_Absyn_PreDec){ goto _LL1451;} else{ goto _LL1452;} _LL1452: if(
_temp1446 ==( void*) Cyc_Absyn_PostInc){ goto _LL1453;} else{ goto _LL1454;}
_LL1454: if( _temp1446 ==( void*) Cyc_Absyn_PostDec){ goto _LL1455;} else{ goto
_LL1447;} _LL1449: s=({ struct Cyc_PP_Doc*(* _temp1456)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1459= Cyc_PP_text(({ char* _temp1461="++";
struct _tagged_string _temp1462; _temp1462.curr= _temp1461; _temp1462.base=
_temp1461; _temp1462.last_plus_one= _temp1461 + 3; _temp1462;})); struct Cyc_PP_Doc*
_temp1460= _temp1444; struct Cyc_PP_Doc* _temp1457[ 2u]={ _temp1459, _temp1460};
struct _tagged_ptr0 _temp1458={ _temp1457, _temp1457, _temp1457 + 2u}; _temp1456(
_temp1458);}); goto _LL1447; _LL1451: s=({ struct Cyc_PP_Doc*(* _temp1463)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1466= Cyc_PP_text(({
char* _temp1468="--"; struct _tagged_string _temp1469; _temp1469.curr= _temp1468;
_temp1469.base= _temp1468; _temp1469.last_plus_one= _temp1468 + 3; _temp1469;}));
struct Cyc_PP_Doc* _temp1467= _temp1444; struct Cyc_PP_Doc* _temp1464[ 2u]={
_temp1466, _temp1467}; struct _tagged_ptr0 _temp1465={ _temp1464, _temp1464,
_temp1464 + 2u}; _temp1463( _temp1465);}); goto _LL1447; _LL1453: s=({ struct
Cyc_PP_Doc*(* _temp1470)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1473= _temp1444; struct Cyc_PP_Doc* _temp1474= Cyc_PP_text(({ char*
_temp1475="++"; struct _tagged_string _temp1476; _temp1476.curr= _temp1475;
_temp1476.base= _temp1475; _temp1476.last_plus_one= _temp1475 + 3; _temp1476;}));
struct Cyc_PP_Doc* _temp1471[ 2u]={ _temp1473, _temp1474}; struct _tagged_ptr0
_temp1472={ _temp1471, _temp1471, _temp1471 + 2u}; _temp1470( _temp1472);});
goto _LL1447; _LL1455: s=({ struct Cyc_PP_Doc*(* _temp1477)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1480= _temp1444; struct Cyc_PP_Doc*
_temp1481= Cyc_PP_text(({ char* _temp1482="--"; struct _tagged_string _temp1483;
_temp1483.curr= _temp1482; _temp1483.base= _temp1482; _temp1483.last_plus_one=
_temp1482 + 3; _temp1483;})); struct Cyc_PP_Doc* _temp1478[ 2u]={ _temp1480,
_temp1481}; struct _tagged_ptr0 _temp1479={ _temp1478, _temp1478, _temp1478 + 2u};
_temp1477( _temp1479);}); goto _LL1447; _LL1447:;} goto _LL1225;} _LL1239:{
struct _tuple8 _temp1485=({ struct _tuple8 _temp1484; _temp1484.f1=( void*)
_temp1320->r; _temp1484.f2=( void*) _temp1318->r; _temp1484;}); _LL1487: goto
_LL1488; _LL1488: s=({ struct Cyc_PP_Doc*(* _temp1489)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1492= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1322); struct Cyc_PP_Doc* _temp1493= Cyc_PP_text(({ char* _temp1499=" ? ";
struct _tagged_string _temp1500; _temp1500.curr= _temp1499; _temp1500.base=
_temp1499; _temp1500.last_plus_one= _temp1499 + 4; _temp1500;})); struct Cyc_PP_Doc*
_temp1494= Cyc_Absynpp_exp2doc_prec( 0, _temp1320); struct Cyc_PP_Doc* _temp1495=
Cyc_PP_text(({ char* _temp1497=" : "; struct _tagged_string _temp1498; _temp1498.curr=
_temp1497; _temp1498.base= _temp1497; _temp1498.last_plus_one= _temp1497 + 4;
_temp1498;})); struct Cyc_PP_Doc* _temp1496= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1318); struct Cyc_PP_Doc* _temp1490[ 5u]={ _temp1492, _temp1493, _temp1494,
_temp1495, _temp1496}; struct _tagged_ptr0 _temp1491={ _temp1490, _temp1490,
_temp1490 + 5u}; _temp1489( _temp1491);}); goto _LL1486; _LL1486:;} goto _LL1225;
_LL1241: s=({ struct Cyc_PP_Doc*(* _temp1501)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1504= Cyc_PP_text(({ char* _temp1513="("; struct
_tagged_string _temp1514; _temp1514.curr= _temp1513; _temp1514.base= _temp1513;
_temp1514.last_plus_one= _temp1513 + 2; _temp1514;})); struct Cyc_PP_Doc*
_temp1505= Cyc_Absynpp_exp2doc( _temp1326); struct Cyc_PP_Doc* _temp1506= Cyc_PP_text(({
char* _temp1511=", "; struct _tagged_string _temp1512; _temp1512.curr= _temp1511;
_temp1512.base= _temp1511; _temp1512.last_plus_one= _temp1511 + 3; _temp1512;}));
struct Cyc_PP_Doc* _temp1507= Cyc_Absynpp_exp2doc( _temp1324); struct Cyc_PP_Doc*
_temp1508= Cyc_PP_text(({ char* _temp1509=")"; struct _tagged_string _temp1510;
_temp1510.curr= _temp1509; _temp1510.base= _temp1509; _temp1510.last_plus_one=
_temp1509 + 2; _temp1510;})); struct Cyc_PP_Doc* _temp1502[ 5u]={ _temp1504,
_temp1505, _temp1506, _temp1507, _temp1508}; struct _tagged_ptr0 _temp1503={
_temp1502, _temp1502, _temp1502 + 5u}; _temp1501( _temp1503);}); goto _LL1225;
_LL1243: s=({ struct Cyc_PP_Doc*(* _temp1515)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1518= Cyc_Absynpp_exp2doc_prec( myprec, _temp1330);
struct Cyc_PP_Doc* _temp1519= Cyc_PP_text(({ char* _temp1524="("; struct
_tagged_string _temp1525; _temp1525.curr= _temp1524; _temp1525.base= _temp1524;
_temp1525.last_plus_one= _temp1524 + 2; _temp1525;})); struct Cyc_PP_Doc*
_temp1520= Cyc_Absynpp_exps2doc_prec( 20, _temp1328); struct Cyc_PP_Doc*
_temp1521= Cyc_PP_text(({ char* _temp1522=")"; struct _tagged_string _temp1523;
_temp1523.curr= _temp1522; _temp1523.base= _temp1522; _temp1523.last_plus_one=
_temp1522 + 2; _temp1523;})); struct Cyc_PP_Doc* _temp1516[ 4u]={ _temp1518,
_temp1519, _temp1520, _temp1521}; struct _tagged_ptr0 _temp1517={ _temp1516,
_temp1516, _temp1516 + 4u}; _temp1515( _temp1517);}); goto _LL1225; _LL1245: s=({
struct Cyc_PP_Doc*(* _temp1526)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1529= Cyc_Absynpp_exp2doc_prec( myprec, _temp1336); struct Cyc_PP_Doc*
_temp1530= Cyc_PP_text(({ char* _temp1535="("; struct _tagged_string _temp1536;
_temp1536.curr= _temp1535; _temp1536.base= _temp1535; _temp1536.last_plus_one=
_temp1535 + 2; _temp1536;})); struct Cyc_PP_Doc* _temp1531= Cyc_Absynpp_exps2doc_prec(
20, _temp1334); struct Cyc_PP_Doc* _temp1532= Cyc_PP_text(({ char* _temp1533=")";
struct _tagged_string _temp1534; _temp1534.curr= _temp1533; _temp1534.base=
_temp1533; _temp1534.last_plus_one= _temp1533 + 2; _temp1534;})); struct Cyc_PP_Doc*
_temp1527[ 4u]={ _temp1529, _temp1530, _temp1531, _temp1532}; struct
_tagged_ptr0 _temp1528={ _temp1527, _temp1527, _temp1527 + 4u}; _temp1526(
_temp1528);}); goto _LL1225; _LL1247: s=({ struct Cyc_PP_Doc*(* _temp1537)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1540= Cyc_PP_text(({
char* _temp1542="throw "; struct _tagged_string _temp1543; _temp1543.curr=
_temp1542; _temp1543.base= _temp1542; _temp1543.last_plus_one= _temp1542 + 7;
_temp1543;})); struct Cyc_PP_Doc* _temp1541= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1338); struct Cyc_PP_Doc* _temp1538[ 2u]={ _temp1540, _temp1541}; struct
_tagged_ptr0 _temp1539={ _temp1538, _temp1538, _temp1538 + 2u}; _temp1537(
_temp1539);}); goto _LL1225; _LL1249: s= Cyc_Absynpp_exp2doc_prec( inprec,
_temp1340); goto _LL1225; _LL1251: s= Cyc_Absynpp_exp2doc_prec( inprec,
_temp1344); goto _LL1225; _LL1253: s=({ struct Cyc_PP_Doc*(* _temp1544)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1547= Cyc_PP_text(({ char*
_temp1553="("; struct _tagged_string _temp1554; _temp1554.curr= _temp1553;
_temp1554.base= _temp1553; _temp1554.last_plus_one= _temp1553 + 2; _temp1554;}));
struct Cyc_PP_Doc* _temp1548= Cyc_Absynpp_typ2doc( _temp1348); struct Cyc_PP_Doc*
_temp1549= Cyc_PP_text(({ char* _temp1551=")"; struct _tagged_string _temp1552;
_temp1552.curr= _temp1551; _temp1552.base= _temp1551; _temp1552.last_plus_one=
_temp1551 + 2; _temp1552;})); struct Cyc_PP_Doc* _temp1550= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1346); struct Cyc_PP_Doc* _temp1545[ 4u]={ _temp1547, _temp1548,
_temp1549, _temp1550}; struct _tagged_ptr0 _temp1546={ _temp1545, _temp1545,
_temp1545 + 4u}; _temp1544( _temp1546);}); goto _LL1225; _LL1255: s=({ struct
Cyc_PP_Doc*(* _temp1555)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1558= Cyc_PP_text(({ char* _temp1560="&"; struct _tagged_string _temp1561;
_temp1561.curr= _temp1560; _temp1561.base= _temp1560; _temp1561.last_plus_one=
_temp1560 + 2; _temp1561;})); struct Cyc_PP_Doc* _temp1559= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1350); struct Cyc_PP_Doc* _temp1556[ 2u]={ _temp1558, _temp1559};
struct _tagged_ptr0 _temp1557={ _temp1556, _temp1556, _temp1556 + 2u}; _temp1555(
_temp1557);}); goto _LL1225; _LL1257: if( _temp1354 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp1562)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1565= Cyc_PP_text(({
char* _temp1567="new "; struct _tagged_string _temp1568; _temp1568.curr=
_temp1567; _temp1568.base= _temp1567; _temp1568.last_plus_one= _temp1567 + 5;
_temp1568;})); struct Cyc_PP_Doc* _temp1566= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1352); struct Cyc_PP_Doc* _temp1563[ 2u]={ _temp1565, _temp1566}; struct
_tagged_ptr0 _temp1564={ _temp1563, _temp1563, _temp1563 + 2u}; _temp1562(
_temp1564);});} else{ s=({ struct Cyc_PP_Doc*(* _temp1569)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1572= Cyc_PP_text(({ char* _temp1578="rnew(";
struct _tagged_string _temp1579; _temp1579.curr= _temp1578; _temp1579.base=
_temp1578; _temp1579.last_plus_one= _temp1578 + 6; _temp1579;})); struct Cyc_PP_Doc*
_temp1573= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1354));
struct Cyc_PP_Doc* _temp1574= Cyc_PP_text(({ char* _temp1576=") "; struct
_tagged_string _temp1577; _temp1577.curr= _temp1576; _temp1577.base= _temp1576;
_temp1577.last_plus_one= _temp1576 + 3; _temp1577;})); struct Cyc_PP_Doc*
_temp1575= Cyc_Absynpp_exp2doc_prec( myprec, _temp1352); struct Cyc_PP_Doc*
_temp1570[ 4u]={ _temp1572, _temp1573, _temp1574, _temp1575}; struct
_tagged_ptr0 _temp1571={ _temp1570, _temp1570, _temp1570 + 4u}; _temp1569(
_temp1571);});} goto _LL1225; _LL1259: s=({ struct Cyc_PP_Doc*(* _temp1580)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1583= Cyc_PP_text(({
char* _temp1588="sizeof("; struct _tagged_string _temp1589; _temp1589.curr=
_temp1588; _temp1589.base= _temp1588; _temp1589.last_plus_one= _temp1588 + 8;
_temp1589;})); struct Cyc_PP_Doc* _temp1584= Cyc_Absynpp_typ2doc( _temp1356);
struct Cyc_PP_Doc* _temp1585= Cyc_PP_text(({ char* _temp1586=")"; struct
_tagged_string _temp1587; _temp1587.curr= _temp1586; _temp1587.base= _temp1586;
_temp1587.last_plus_one= _temp1586 + 2; _temp1587;})); struct Cyc_PP_Doc*
_temp1581[ 3u]={ _temp1583, _temp1584, _temp1585}; struct _tagged_ptr0 _temp1582={
_temp1581, _temp1581, _temp1581 + 3u}; _temp1580( _temp1582);}); goto _LL1225;
_LL1261: s=({ struct Cyc_PP_Doc*(* _temp1590)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1593= Cyc_PP_text(({ char* _temp1598="sizeof("; struct
_tagged_string _temp1599; _temp1599.curr= _temp1598; _temp1599.base= _temp1598;
_temp1599.last_plus_one= _temp1598 + 8; _temp1599;})); struct Cyc_PP_Doc*
_temp1594= Cyc_Absynpp_exp2doc( _temp1358); struct Cyc_PP_Doc* _temp1595= Cyc_PP_text(({
char* _temp1596=")"; struct _tagged_string _temp1597; _temp1597.curr= _temp1596;
_temp1597.base= _temp1596; _temp1597.last_plus_one= _temp1596 + 2; _temp1597;}));
struct Cyc_PP_Doc* _temp1591[ 3u]={ _temp1593, _temp1594, _temp1595}; struct
_tagged_ptr0 _temp1592={ _temp1591, _temp1591, _temp1591 + 3u}; _temp1590(
_temp1592);}); goto _LL1225; _LL1263: s=({ struct Cyc_PP_Doc*(* _temp1600)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1603= Cyc_PP_text(({
char* _temp1605="*"; struct _tagged_string _temp1606; _temp1606.curr= _temp1605;
_temp1606.base= _temp1605; _temp1606.last_plus_one= _temp1605 + 2; _temp1606;}));
struct Cyc_PP_Doc* _temp1604= Cyc_Absynpp_exp2doc_prec( myprec, _temp1360);
struct Cyc_PP_Doc* _temp1601[ 2u]={ _temp1603, _temp1604}; struct _tagged_ptr0
_temp1602={ _temp1601, _temp1601, _temp1601 + 2u}; _temp1600( _temp1602);});
goto _LL1225; _LL1265: s=({ struct Cyc_PP_Doc*(* _temp1607)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1610= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1364); struct Cyc_PP_Doc* _temp1611= Cyc_PP_text(({ char* _temp1613=".";
struct _tagged_string _temp1614; _temp1614.curr= _temp1613; _temp1614.base=
_temp1613; _temp1614.last_plus_one= _temp1613 + 2; _temp1614;})); struct Cyc_PP_Doc*
_temp1612= Cyc_PP_textptr( _temp1362); struct Cyc_PP_Doc* _temp1608[ 3u]={
_temp1610, _temp1611, _temp1612}; struct _tagged_ptr0 _temp1609={ _temp1608,
_temp1608, _temp1608 + 3u}; _temp1607( _temp1609);}); goto _LL1225; _LL1267: s=({
struct Cyc_PP_Doc*(* _temp1615)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1618= Cyc_Absynpp_exp2doc_prec( myprec, _temp1368); struct Cyc_PP_Doc*
_temp1619= Cyc_PP_text(({ char* _temp1621="->"; struct _tagged_string _temp1622;
_temp1622.curr= _temp1621; _temp1622.base= _temp1621; _temp1622.last_plus_one=
_temp1621 + 3; _temp1622;})); struct Cyc_PP_Doc* _temp1620= Cyc_PP_textptr(
_temp1366); struct Cyc_PP_Doc* _temp1616[ 3u]={ _temp1618, _temp1619, _temp1620};
struct _tagged_ptr0 _temp1617={ _temp1616, _temp1616, _temp1616 + 3u}; _temp1615(
_temp1617);}); goto _LL1225; _LL1269: s=({ struct Cyc_PP_Doc*(* _temp1623)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1626= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1372); struct Cyc_PP_Doc* _temp1627= Cyc_PP_text(({ char* _temp1632="[";
struct _tagged_string _temp1633; _temp1633.curr= _temp1632; _temp1633.base=
_temp1632; _temp1633.last_plus_one= _temp1632 + 2; _temp1633;})); struct Cyc_PP_Doc*
_temp1628= Cyc_Absynpp_exp2doc( _temp1370); struct Cyc_PP_Doc* _temp1629= Cyc_PP_text(({
char* _temp1630="]"; struct _tagged_string _temp1631; _temp1631.curr= _temp1630;
_temp1631.base= _temp1630; _temp1631.last_plus_one= _temp1630 + 2; _temp1631;}));
struct Cyc_PP_Doc* _temp1624[ 4u]={ _temp1626, _temp1627, _temp1628, _temp1629};
struct _tagged_ptr0 _temp1625={ _temp1624, _temp1624, _temp1624 + 4u}; _temp1623(
_temp1625);}); goto _LL1225; _LL1271: s=({ struct Cyc_PP_Doc*(* _temp1634)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1637= Cyc_PP_text(({
char* _temp1642="$("; struct _tagged_string _temp1643; _temp1643.curr= _temp1642;
_temp1643.base= _temp1642; _temp1643.last_plus_one= _temp1642 + 3; _temp1643;}));
struct Cyc_PP_Doc* _temp1638= Cyc_Absynpp_exps2doc_prec( 20, _temp1374); struct
Cyc_PP_Doc* _temp1639= Cyc_PP_text(({ char* _temp1640=")"; struct _tagged_string
_temp1641; _temp1641.curr= _temp1640; _temp1641.base= _temp1640; _temp1641.last_plus_one=
_temp1640 + 2; _temp1641;})); struct Cyc_PP_Doc* _temp1635[ 3u]={ _temp1637,
_temp1638, _temp1639}; struct _tagged_ptr0 _temp1636={ _temp1635, _temp1635,
_temp1635 + 3u}; _temp1634( _temp1636);}); goto _LL1225; _LL1273: s=({ struct
Cyc_PP_Doc*(* _temp1644)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1647= Cyc_PP_text(({ char* _temp1659="("; struct _tagged_string _temp1660;
_temp1660.curr= _temp1659; _temp1660.base= _temp1659; _temp1660.last_plus_one=
_temp1659 + 2; _temp1660;})); struct Cyc_PP_Doc* _temp1648= Cyc_Absynpp_typ2doc((*
_temp1378).f3); struct Cyc_PP_Doc* _temp1649= Cyc_PP_text(({ char* _temp1657=")";
struct _tagged_string _temp1658; _temp1658.curr= _temp1657; _temp1658.base=
_temp1657; _temp1658.last_plus_one= _temp1657 + 2; _temp1658;})); struct Cyc_PP_Doc*
_temp1650= Cyc_PP_group(({ char* _temp1651="{"; struct _tagged_string _temp1652;
_temp1652.curr= _temp1651; _temp1652.base= _temp1651; _temp1652.last_plus_one=
_temp1651 + 2; _temp1652;}),({ char* _temp1653="}"; struct _tagged_string
_temp1654; _temp1654.curr= _temp1653; _temp1654.base= _temp1653; _temp1654.last_plus_one=
_temp1653 + 2; _temp1654;}),({ char* _temp1655=","; struct _tagged_string
_temp1656; _temp1656.curr= _temp1655; _temp1656.base= _temp1655; _temp1656.last_plus_one=
_temp1655 + 2; _temp1656;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1376)); struct Cyc_PP_Doc* _temp1645[ 4u]={ _temp1647, _temp1648, _temp1649,
_temp1650}; struct _tagged_ptr0 _temp1646={ _temp1645, _temp1645, _temp1645 + 4u};
_temp1644( _temp1646);}); goto _LL1225; _LL1275: s= Cyc_PP_group(({ char*
_temp1661="{"; struct _tagged_string _temp1662; _temp1662.curr= _temp1661;
_temp1662.base= _temp1661; _temp1662.last_plus_one= _temp1661 + 2; _temp1662;}),({
char* _temp1663="}"; struct _tagged_string _temp1664; _temp1664.curr= _temp1663;
_temp1664.base= _temp1663; _temp1664.last_plus_one= _temp1663 + 2; _temp1664;}),({
char* _temp1665=","; struct _tagged_string _temp1666; _temp1666.curr= _temp1665;
_temp1666.base= _temp1665; _temp1666.last_plus_one= _temp1665 + 2; _temp1666;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1380)); goto _LL1225; _LL1277: s=({
struct Cyc_PP_Doc*(* _temp1667)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1670= Cyc_PP_text(({ char* _temp1683="{for "; struct _tagged_string
_temp1684; _temp1684.curr= _temp1683; _temp1684.base= _temp1683; _temp1684.last_plus_one=
_temp1683 + 6; _temp1684;})); struct Cyc_PP_Doc* _temp1671= Cyc_PP_text(*(*
_temp1386->name).f2); struct Cyc_PP_Doc* _temp1672= Cyc_PP_text(({ char*
_temp1681=" < "; struct _tagged_string _temp1682; _temp1682.curr= _temp1681;
_temp1682.base= _temp1681; _temp1682.last_plus_one= _temp1681 + 4; _temp1682;}));
struct Cyc_PP_Doc* _temp1673= Cyc_Absynpp_exp2doc( _temp1384); struct Cyc_PP_Doc*
_temp1674= Cyc_PP_text(({ char* _temp1679=" : "; struct _tagged_string _temp1680;
_temp1680.curr= _temp1679; _temp1680.base= _temp1679; _temp1680.last_plus_one=
_temp1679 + 4; _temp1680;})); struct Cyc_PP_Doc* _temp1675= Cyc_Absynpp_exp2doc(
_temp1382); struct Cyc_PP_Doc* _temp1676= Cyc_PP_text(({ char* _temp1677="}";
struct _tagged_string _temp1678; _temp1678.curr= _temp1677; _temp1678.base=
_temp1677; _temp1678.last_plus_one= _temp1677 + 2; _temp1678;})); struct Cyc_PP_Doc*
_temp1668[ 7u]={ _temp1670, _temp1671, _temp1672, _temp1673, _temp1674,
_temp1675, _temp1676}; struct _tagged_ptr0 _temp1669={ _temp1668, _temp1668,
_temp1668 + 7u}; _temp1667( _temp1669);}); goto _LL1225; _LL1279: s=({ struct
Cyc_PP_Doc*(* _temp1685)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1688= Cyc_Absynpp_qvar2doc( _temp1394); struct Cyc_PP_Doc* _temp1689= Cyc_PP_group(({
char* _temp1690="{"; struct _tagged_string _temp1691; _temp1691.curr= _temp1690;
_temp1691.base= _temp1690; _temp1691.last_plus_one= _temp1690 + 2; _temp1691;}),({
char* _temp1692="}"; struct _tagged_string _temp1693; _temp1693.curr= _temp1692;
_temp1693.base= _temp1692; _temp1693.last_plus_one= _temp1692 + 2; _temp1693;}),({
char* _temp1694=","; struct _tagged_string _temp1695; _temp1695.curr= _temp1694;
_temp1695.base= _temp1694; _temp1695.last_plus_one= _temp1694 + 2; _temp1695;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1390)); struct Cyc_PP_Doc* _temp1686[
2u]={ _temp1688, _temp1689}; struct _tagged_ptr0 _temp1687={ _temp1686,
_temp1686, _temp1686 + 2u}; _temp1685( _temp1687);}); goto _LL1225; _LL1281: s=
Cyc_PP_group(({ char* _temp1696="{"; struct _tagged_string _temp1697; _temp1697.curr=
_temp1696; _temp1697.base= _temp1696; _temp1697.last_plus_one= _temp1696 + 2;
_temp1697;}),({ char* _temp1698="}"; struct _tagged_string _temp1699; _temp1699.curr=
_temp1698; _temp1699.base= _temp1698; _temp1699.last_plus_one= _temp1698 + 2;
_temp1699;}),({ char* _temp1700=","; struct _tagged_string _temp1701; _temp1701.curr=
_temp1700; _temp1701.base= _temp1700; _temp1701.last_plus_one= _temp1700 + 2;
_temp1701;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1396)); goto _LL1225; _LL1283: if( _temp1404 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1400->name);} else{ s=({ struct Cyc_PP_Doc*(* _temp1702)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1705= Cyc_Absynpp_qvar2doc(
_temp1400->name); struct Cyc_PP_Doc* _temp1706= Cyc_PP_egroup(({ char* _temp1707="(";
struct _tagged_string _temp1708; _temp1708.curr= _temp1707; _temp1708.base=
_temp1707; _temp1708.last_plus_one= _temp1707 + 2; _temp1708;}),({ char*
_temp1709=")"; struct _tagged_string _temp1710; _temp1710.curr= _temp1709;
_temp1710.base= _temp1709; _temp1710.last_plus_one= _temp1709 + 2; _temp1710;}),({
char* _temp1711=","; struct _tagged_string _temp1712; _temp1712.curr= _temp1711;
_temp1712.base= _temp1711; _temp1712.last_plus_one= _temp1711 + 2; _temp1712;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_exp2doc, _temp1404));
struct Cyc_PP_Doc* _temp1703[ 2u]={ _temp1705, _temp1706}; struct _tagged_ptr0
_temp1704={ _temp1703, _temp1703, _temp1703 + 2u}; _temp1702( _temp1704);});}
goto _LL1225; _LL1285: s= Cyc_Absynpp_qvar2doc( _temp1414); goto _LL1225;
_LL1287: if( _temp1418 == 0){ s=({ struct Cyc_PP_Doc*(* _temp1713)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1716= Cyc_PP_text(({ char*
_temp1721="malloc(sizeof("; struct _tagged_string _temp1722; _temp1722.curr=
_temp1721; _temp1722.base= _temp1721; _temp1722.last_plus_one= _temp1721 + 15;
_temp1722;})); struct Cyc_PP_Doc* _temp1717= Cyc_Absynpp_typ2doc( _temp1416);
struct Cyc_PP_Doc* _temp1718= Cyc_PP_text(({ char* _temp1719="))"; struct
_tagged_string _temp1720; _temp1720.curr= _temp1719; _temp1720.base= _temp1719;
_temp1720.last_plus_one= _temp1719 + 3; _temp1720;})); struct Cyc_PP_Doc*
_temp1714[ 3u]={ _temp1716, _temp1717, _temp1718}; struct _tagged_ptr0 _temp1715={
_temp1714, _temp1714, _temp1714 + 3u}; _temp1713( _temp1715);});} else{ s=({
struct Cyc_PP_Doc*(* _temp1723)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1726= Cyc_PP_text(({ char* _temp1738="rmalloc("; struct _tagged_string
_temp1739; _temp1739.curr= _temp1738; _temp1739.base= _temp1738; _temp1739.last_plus_one=
_temp1738 + 9; _temp1739;})); struct Cyc_PP_Doc* _temp1727= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1418)); struct Cyc_PP_Doc* _temp1728=
Cyc_PP_text(({ char* _temp1736=","; struct _tagged_string _temp1737; _temp1737.curr=
_temp1736; _temp1737.base= _temp1736; _temp1737.last_plus_one= _temp1736 + 2;
_temp1737;})); struct Cyc_PP_Doc* _temp1729= Cyc_PP_text(({ char* _temp1734="sizeof(";
struct _tagged_string _temp1735; _temp1735.curr= _temp1734; _temp1735.base=
_temp1734; _temp1735.last_plus_one= _temp1734 + 8; _temp1735;})); struct Cyc_PP_Doc*
_temp1730= Cyc_Absynpp_typ2doc( _temp1416); struct Cyc_PP_Doc* _temp1731= Cyc_PP_text(({
char* _temp1732="))"; struct _tagged_string _temp1733; _temp1733.curr= _temp1732;
_temp1733.base= _temp1732; _temp1733.last_plus_one= _temp1732 + 3; _temp1733;}));
struct Cyc_PP_Doc* _temp1724[ 6u]={ _temp1726, _temp1727, _temp1728, _temp1729,
_temp1730, _temp1731}; struct _tagged_ptr0 _temp1725={ _temp1724, _temp1724,
_temp1724 + 6u}; _temp1723( _temp1725);});} goto _LL1225; _LL1289: s= Cyc_PP_group(({
char* _temp1740="{"; struct _tagged_string _temp1741; _temp1741.curr= _temp1740;
_temp1741.base= _temp1740; _temp1741.last_plus_one= _temp1740 + 2; _temp1741;}),({
char* _temp1742="}"; struct _tagged_string _temp1743; _temp1743.curr= _temp1742;
_temp1743.base= _temp1742; _temp1743.last_plus_one= _temp1742 + 2; _temp1743;}),({
char* _temp1744=","; struct _tagged_string _temp1745; _temp1745.curr= _temp1744;
_temp1745.base= _temp1744; _temp1745.last_plus_one= _temp1744 + 2; _temp1745;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1420)); goto _LL1225; _LL1291: s=({
struct Cyc_PP_Doc*(* _temp1746)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1749= Cyc_PP_text(({ char* _temp1754="({ "; struct _tagged_string _temp1755;
_temp1755.curr= _temp1754; _temp1755.base= _temp1754; _temp1755.last_plus_one=
_temp1754 + 4; _temp1755;})); struct Cyc_PP_Doc* _temp1750= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1424)); struct Cyc_PP_Doc* _temp1751= Cyc_PP_text(({ char* _temp1752=" })";
struct _tagged_string _temp1753; _temp1753.curr= _temp1752; _temp1753.base=
_temp1752; _temp1753.last_plus_one= _temp1752 + 4; _temp1753;})); struct Cyc_PP_Doc*
_temp1747[ 3u]={ _temp1749, _temp1750, _temp1751}; struct _tagged_ptr0 _temp1748={
_temp1747, _temp1747, _temp1747 + 3u}; _temp1746( _temp1748);}); goto _LL1225;
_LL1293: s=({ struct Cyc_PP_Doc*(* _temp1756)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1759= Cyc_PP_text(({ char* _temp1767="codegen("; struct
_tagged_string _temp1768; _temp1768.curr= _temp1767; _temp1768.base= _temp1767;
_temp1768.last_plus_one= _temp1767 + 9; _temp1768;})); struct Cyc_PP_Doc*
_temp1760= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl*
_temp1764=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1764->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp1765=(
struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1765[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1766; _temp1766.tag= Cyc_Absyn_Fn_d;
_temp1766.f1= _temp1426; _temp1766;}); _temp1765;})); _temp1764->loc= e->loc;
_temp1764;}))); struct Cyc_PP_Doc* _temp1761= Cyc_PP_text(({ char* _temp1762=")";
struct _tagged_string _temp1763; _temp1763.curr= _temp1762; _temp1763.base=
_temp1762; _temp1763.last_plus_one= _temp1762 + 2; _temp1763;})); struct Cyc_PP_Doc*
_temp1757[ 3u]={ _temp1759, _temp1760, _temp1761}; struct _tagged_ptr0 _temp1758={
_temp1757, _temp1757, _temp1757 + 3u}; _temp1756( _temp1758);}); goto _LL1225;
_LL1295: s=({ struct Cyc_PP_Doc*(* _temp1769)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1772= Cyc_PP_text(({ char* _temp1777="fill("; struct
_tagged_string _temp1778; _temp1778.curr= _temp1777; _temp1778.base= _temp1777;
_temp1778.last_plus_one= _temp1777 + 6; _temp1778;})); struct Cyc_PP_Doc*
_temp1773= Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1428)); struct Cyc_PP_Doc*
_temp1774= Cyc_PP_text(({ char* _temp1775=")"; struct _tagged_string _temp1776;
_temp1776.curr= _temp1775; _temp1776.base= _temp1775; _temp1776.last_plus_one=
_temp1775 + 2; _temp1776;})); struct Cyc_PP_Doc* _temp1770[ 3u]={ _temp1772,
_temp1773, _temp1774}; struct _tagged_ptr0 _temp1771={ _temp1770, _temp1770,
_temp1770 + 3u}; _temp1769( _temp1771);}); goto _LL1225; _LL1225:;} if( inprec
>= myprec){ s=({ struct Cyc_PP_Doc*(* _temp1779)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1782= Cyc_PP_text(({ char* _temp1787="("; struct
_tagged_string _temp1788; _temp1788.curr= _temp1787; _temp1788.base= _temp1787;
_temp1788.last_plus_one= _temp1787 + 2; _temp1788;})); struct Cyc_PP_Doc*
_temp1783= s; struct Cyc_PP_Doc* _temp1784= Cyc_PP_text(({ char* _temp1785=")";
struct _tagged_string _temp1786; _temp1786.curr= _temp1785; _temp1786.base=
_temp1785; _temp1786.last_plus_one= _temp1785 + 2; _temp1786;})); struct Cyc_PP_Doc*
_temp1780[ 3u]={ _temp1782, _temp1783, _temp1784}; struct _tagged_ptr0 _temp1781={
_temp1780, _temp1780, _temp1780 + 3u}; _temp1779( _temp1781);});} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1789= d;
struct Cyc_Absyn_Exp* _temp1795; struct _tagged_string* _temp1797; _LL1791: if(*((
int*) _temp1789) == Cyc_Absyn_ArrayElement){ _LL1796: _temp1795=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1789)->f1; goto _LL1792;} else{ goto _LL1793;} _LL1793: if(*(( int*)
_temp1789) == Cyc_Absyn_FieldName){ _LL1798: _temp1797=(( struct Cyc_Absyn_FieldName_struct*)
_temp1789)->f1; goto _LL1794;} else{ goto _LL1790;} _LL1792: return({ struct Cyc_PP_Doc*(*
_temp1799)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1802= Cyc_PP_text(({
char* _temp1807=".["; struct _tagged_string _temp1808; _temp1808.curr= _temp1807;
_temp1808.base= _temp1807; _temp1808.last_plus_one= _temp1807 + 3; _temp1808;}));
struct Cyc_PP_Doc* _temp1803= Cyc_Absynpp_exp2doc( _temp1795); struct Cyc_PP_Doc*
_temp1804= Cyc_PP_text(({ char* _temp1805="]"; struct _tagged_string _temp1806;
_temp1806.curr= _temp1805; _temp1806.base= _temp1805; _temp1806.last_plus_one=
_temp1805 + 2; _temp1806;})); struct Cyc_PP_Doc* _temp1800[ 3u]={ _temp1802,
_temp1803, _temp1804}; struct _tagged_ptr0 _temp1801={ _temp1800, _temp1800,
_temp1800 + 3u}; _temp1799( _temp1801);}); _LL1794: return({ struct Cyc_PP_Doc*(*
_temp1809)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1812= Cyc_PP_text(({
char* _temp1814="."; struct _tagged_string _temp1815; _temp1815.curr= _temp1814;
_temp1815.base= _temp1814; _temp1815.last_plus_one= _temp1814 + 2; _temp1815;}));
struct Cyc_PP_Doc* _temp1813= Cyc_PP_textptr( _temp1797); struct Cyc_PP_Doc*
_temp1810[ 2u]={ _temp1812, _temp1813}; struct _tagged_ptr0 _temp1811={
_temp1810, _temp1810, _temp1810 + 2u}; _temp1809( _temp1811);}); _LL1790:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 == 0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return({ struct Cyc_PP_Doc*(*
_temp1816)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1819= Cyc_PP_egroup(({
char* _temp1821=""; struct _tagged_string _temp1822; _temp1822.curr= _temp1821;
_temp1822.base= _temp1821; _temp1822.last_plus_one= _temp1821 + 1; _temp1822;}),({
char* _temp1823="="; struct _tagged_string _temp1824; _temp1824.curr= _temp1823;
_temp1824.base= _temp1823; _temp1824.last_plus_one= _temp1823 + 2; _temp1824;}),({
char* _temp1825="="; struct _tagged_string _temp1826; _temp1826.curr= _temp1825;
_temp1826.base= _temp1825; _temp1826.last_plus_one= _temp1825 + 2; _temp1826;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* de).f1)); struct Cyc_PP_Doc*
_temp1820= Cyc_Absynpp_exp2doc((* de).f2); struct Cyc_PP_Doc* _temp1817[ 2u]={
_temp1819, _temp1820}; struct _tagged_ptr0 _temp1818={ _temp1817, _temp1817,
_temp1817 + 2u}; _temp1816( _temp1818);});}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_PP_group(({ char* _temp1827="";
struct _tagged_string _temp1828; _temp1828.curr= _temp1827; _temp1828.base=
_temp1827; _temp1828.last_plus_one= _temp1827 + 1; _temp1828;}),({ char*
_temp1829=""; struct _tagged_string _temp1830; _temp1830.curr= _temp1829;
_temp1830.base= _temp1829; _temp1830.last_plus_one= _temp1829 + 1; _temp1830;}),({
char* _temp1831=","; struct _tagged_string _temp1832; _temp1832.curr= _temp1831;
_temp1832.base= _temp1831; _temp1832.last_plus_one= _temp1831 + 2; _temp1832;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void* c){ void*
_temp1833= c; char _temp1851; void* _temp1853; short _temp1855; void* _temp1857;
int _temp1859; void* _temp1861; int _temp1863; void* _temp1865; long long
_temp1867; void* _temp1869; struct _tagged_string _temp1871; struct
_tagged_string _temp1873; _LL1835: if(( unsigned int) _temp1833 > 1u?*(( int*)
_temp1833) == Cyc_Absyn_Char_c: 0){ _LL1854: _temp1853=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp1833)->f1; goto _LL1852; _LL1852: _temp1851=(( struct Cyc_Absyn_Char_c_struct*)
_temp1833)->f2; goto _LL1836;} else{ goto _LL1837;} _LL1837: if(( unsigned int)
_temp1833 > 1u?*(( int*) _temp1833) == Cyc_Absyn_Short_c: 0){ _LL1858: _temp1857=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1833)->f1; goto _LL1856; _LL1856:
_temp1855=(( struct Cyc_Absyn_Short_c_struct*) _temp1833)->f2; goto _LL1838;}
else{ goto _LL1839;} _LL1839: if(( unsigned int) _temp1833 > 1u?*(( int*)
_temp1833) == Cyc_Absyn_Int_c: 0){ _LL1862: _temp1861=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp1833)->f1; if( _temp1861 ==( void*) Cyc_Absyn_Signed){ goto _LL1860;} else{
goto _LL1841;} _LL1860: _temp1859=(( struct Cyc_Absyn_Int_c_struct*) _temp1833)->f2;
goto _LL1840;} else{ goto _LL1841;} _LL1841: if(( unsigned int) _temp1833 > 1u?*((
int*) _temp1833) == Cyc_Absyn_Int_c: 0){ _LL1866: _temp1865=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1833)->f1; if( _temp1865 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1864;} else{ goto _LL1843;} _LL1864: _temp1863=(( struct Cyc_Absyn_Int_c_struct*)
_temp1833)->f2; goto _LL1842;} else{ goto _LL1843;} _LL1843: if(( unsigned int)
_temp1833 > 1u?*(( int*) _temp1833) == Cyc_Absyn_LongLong_c: 0){ _LL1870:
_temp1869=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1833)->f1; goto
_LL1868; _LL1868: _temp1867=(( struct Cyc_Absyn_LongLong_c_struct*) _temp1833)->f2;
goto _LL1844;} else{ goto _LL1845;} _LL1845: if(( unsigned int) _temp1833 > 1u?*((
int*) _temp1833) == Cyc_Absyn_Float_c: 0){ _LL1872: _temp1871=(( struct Cyc_Absyn_Float_c_struct*)
_temp1833)->f1; goto _LL1846;} else{ goto _LL1847;} _LL1847: if( _temp1833 ==(
void*) Cyc_Absyn_Null_c){ goto _LL1848;} else{ goto _LL1849;} _LL1849: if((
unsigned int) _temp1833 > 1u?*(( int*) _temp1833) == Cyc_Absyn_String_c: 0){
_LL1874: _temp1873=(( struct Cyc_Absyn_String_c_struct*) _temp1833)->f1; goto
_LL1850;} else{ goto _LL1834;} _LL1836: return Cyc_PP_text(({ struct
_tagged_string _temp1875= Cyc_Absynpp_char_escape( _temp1851); xprintf("'%.*s'",
_temp1875.last_plus_one - _temp1875.curr, _temp1875.curr);})); _LL1838: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1855)); _LL1840: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1859)); _LL1842: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1863)); _LL1844: return Cyc_PP_text(({ char* _temp1876="<<FIX LONG LONG CONSTANT>>";
struct _tagged_string _temp1877; _temp1877.curr= _temp1876; _temp1877.base=
_temp1876; _temp1877.last_plus_one= _temp1876 + 27; _temp1877;})); _LL1846:
return Cyc_PP_text( _temp1871); _LL1848: return Cyc_PP_text(({ char* _temp1878="null";
struct _tagged_string _temp1879; _temp1879.curr= _temp1878; _temp1879.base=
_temp1878; _temp1879.last_plus_one= _temp1878 + 5; _temp1879;})); _LL1850:
return({ struct Cyc_PP_Doc*(* _temp1880)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1883= Cyc_PP_text(({ char* _temp1888="\""; struct
_tagged_string _temp1889; _temp1889.curr= _temp1888; _temp1889.base= _temp1888;
_temp1889.last_plus_one= _temp1888 + 2; _temp1889;})); struct Cyc_PP_Doc*
_temp1884= Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1873)); struct Cyc_PP_Doc*
_temp1885= Cyc_PP_text(({ char* _temp1886="\""; struct _tagged_string _temp1887;
_temp1887.curr= _temp1886; _temp1887.base= _temp1886; _temp1887.last_plus_one=
_temp1886 + 2; _temp1887;})); struct Cyc_PP_Doc* _temp1881[ 3u]={ _temp1883,
_temp1884, _temp1885}; struct _tagged_ptr0 _temp1882={ _temp1881, _temp1881,
_temp1881 + 3u}; _temp1880( _temp1882);}); _LL1834:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl != 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1890=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1890[ 0]=({ struct Cyc_Core_Failure_struct _temp1891; _temp1891.tag= Cyc_Core_Failure;
_temp1891.f1=({ struct _tagged_string _temp1892= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1892.last_plus_one
- _temp1892.curr, _temp1892.curr);}); _temp1891;}); _temp1890;}));}{ struct Cyc_PP_Doc*
_temp1893= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd); goto _LL1894; _LL1894: return({ struct
Cyc_PP_Doc*(* _temp1895)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1898= _temp1893; struct Cyc_PP_Doc* _temp1899= Cyc_PP_text(({ char*
_temp1900=".size"; struct _tagged_string _temp1901; _temp1901.curr= _temp1900;
_temp1901.base= _temp1900; _temp1901.last_plus_one= _temp1900 + 6; _temp1901;}));
struct Cyc_PP_Doc* _temp1896[ 2u]={ _temp1898, _temp1899}; struct _tagged_ptr0
_temp1897={ _temp1896, _temp1896, _temp1896 + 2u}; _temp1895( _temp1897);});}}
else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct Cyc_List_List* ds=(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es);
if( ds == 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp1902=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1902[ 0]=({ struct Cyc_Core_Failure_struct _temp1903; _temp1903.tag= Cyc_Core_Failure;
_temp1903.f1=({ struct _tagged_string _temp1904= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with no args", _temp1904.last_plus_one -
_temp1904.curr, _temp1904.curr);}); _temp1903;}); _temp1902;}));} else{ if(((
struct Cyc_List_List*) _check_null( ds))->tl == 0){ return({ struct Cyc_PP_Doc*(*
_temp1905)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1908= ps;
struct Cyc_PP_Doc* _temp1909= Cyc_PP_text(({ char* _temp1911=" "; struct
_tagged_string _temp1912; _temp1912.curr= _temp1911; _temp1912.base= _temp1911;
_temp1912.last_plus_one= _temp1911 + 2; _temp1912;})); struct Cyc_PP_Doc*
_temp1910=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct Cyc_PP_Doc* _temp1906[ 3u]={ _temp1908, _temp1909, _temp1910}; struct
_tagged_ptr0 _temp1907={ _temp1906, _temp1906, _temp1906 + 3u}; _temp1905(
_temp1907);});} else{ if((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( ds))->tl))->tl != 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1913=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1913[ 0]=({ struct Cyc_Core_Failure_struct _temp1914; _temp1914.tag= Cyc_Core_Failure;
_temp1914.f1=({ struct _tagged_string _temp1915= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with more than 2 args", _temp1915.last_plus_one
- _temp1915.curr, _temp1915.curr);}); _temp1914;}); _temp1913;}));} else{ return({
struct Cyc_PP_Doc*(* _temp1916)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1919=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct Cyc_PP_Doc* _temp1920= Cyc_PP_text(({ char* _temp1926=" "; struct
_tagged_string _temp1927; _temp1927.curr= _temp1926; _temp1927.base= _temp1926;
_temp1927.last_plus_one= _temp1926 + 2; _temp1927;})); struct Cyc_PP_Doc*
_temp1921= ps; struct Cyc_PP_Doc* _temp1922= Cyc_PP_text(({ char* _temp1924=" ";
struct _tagged_string _temp1925; _temp1925.curr= _temp1924; _temp1925.base=
_temp1924; _temp1925.last_plus_one= _temp1924 + 2; _temp1925;})); struct Cyc_PP_Doc*
_temp1923=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( ds))->tl))->hd; struct Cyc_PP_Doc* _temp1917[ 5u]={
_temp1919, _temp1920, _temp1921, _temp1922, _temp1923}; struct _tagged_ptr0
_temp1918={ _temp1917, _temp1917, _temp1917 + 5u}; _temp1916( _temp1918);});}}}}
else{ return({ struct Cyc_PP_Doc*(* _temp1928)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1931= ps; struct Cyc_PP_Doc* _temp1932= Cyc_PP_text(({
char* _temp1937="("; struct _tagged_string _temp1938; _temp1938.curr= _temp1937;
_temp1938.base= _temp1937; _temp1938.last_plus_one= _temp1937 + 2; _temp1938;}));
struct Cyc_PP_Doc* _temp1933= Cyc_Absynpp_exps2doc_prec( 20, es); struct Cyc_PP_Doc*
_temp1934= Cyc_PP_text(({ char* _temp1935=")"; struct _tagged_string _temp1936;
_temp1936.curr= _temp1935; _temp1936.base= _temp1935; _temp1936.last_plus_one=
_temp1935 + 2; _temp1936;})); struct Cyc_PP_Doc* _temp1929[ 4u]={ _temp1931,
_temp1932, _temp1933, _temp1934}; struct _tagged_ptr0 _temp1930={ _temp1929,
_temp1929, _temp1929 + 4u}; _temp1928( _temp1930);});}}} struct _tagged_string
Cyc_Absynpp_prim2str( void* p){ void* _temp1939= p; _LL1941: if( _temp1939 ==(
void*) Cyc_Absyn_Plus){ goto _LL1942;} else{ goto _LL1943;} _LL1943: if(
_temp1939 ==( void*) Cyc_Absyn_Times){ goto _LL1944;} else{ goto _LL1945;}
_LL1945: if( _temp1939 ==( void*) Cyc_Absyn_Minus){ goto _LL1946;} else{ goto
_LL1947;} _LL1947: if( _temp1939 ==( void*) Cyc_Absyn_Div){ goto _LL1948;} else{
goto _LL1949;} _LL1949: if( _temp1939 ==( void*) Cyc_Absyn_Mod){ goto _LL1950;}
else{ goto _LL1951;} _LL1951: if( _temp1939 ==( void*) Cyc_Absyn_Eq){ goto
_LL1952;} else{ goto _LL1953;} _LL1953: if( _temp1939 ==( void*) Cyc_Absyn_Neq){
goto _LL1954;} else{ goto _LL1955;} _LL1955: if( _temp1939 ==( void*) Cyc_Absyn_Gt){
goto _LL1956;} else{ goto _LL1957;} _LL1957: if( _temp1939 ==( void*) Cyc_Absyn_Lt){
goto _LL1958;} else{ goto _LL1959;} _LL1959: if( _temp1939 ==( void*) Cyc_Absyn_Gte){
goto _LL1960;} else{ goto _LL1961;} _LL1961: if( _temp1939 ==( void*) Cyc_Absyn_Lte){
goto _LL1962;} else{ goto _LL1963;} _LL1963: if( _temp1939 ==( void*) Cyc_Absyn_Not){
goto _LL1964;} else{ goto _LL1965;} _LL1965: if( _temp1939 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1966;} else{ goto _LL1967;} _LL1967: if( _temp1939 ==( void*) Cyc_Absyn_Bitand){
goto _LL1968;} else{ goto _LL1969;} _LL1969: if( _temp1939 ==( void*) Cyc_Absyn_Bitor){
goto _LL1970;} else{ goto _LL1971;} _LL1971: if( _temp1939 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1972;} else{ goto _LL1973;} _LL1973: if( _temp1939 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1974;} else{ goto _LL1975;} _LL1975: if( _temp1939 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1976;} else{ goto _LL1977;} _LL1977: if( _temp1939 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1978;} else{ goto _LL1979;} _LL1979: if( _temp1939 ==( void*) Cyc_Absyn_Size){
goto _LL1980;} else{ goto _LL1981;} _LL1981: if( _temp1939 ==( void*) Cyc_Absyn_Printf){
goto _LL1982;} else{ goto _LL1983;} _LL1983: if( _temp1939 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1984;} else{ goto _LL1985;} _LL1985: if( _temp1939 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1986;} else{ goto _LL1987;} _LL1987: if( _temp1939 ==( void*) Cyc_Absyn_Scanf){
goto _LL1988;} else{ goto _LL1989;} _LL1989: if( _temp1939 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1990;} else{ goto _LL1991;} _LL1991: if( _temp1939 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1992;} else{ goto _LL1940;} _LL1942: return({ char* _temp1993="+";
struct _tagged_string _temp1994; _temp1994.curr= _temp1993; _temp1994.base=
_temp1993; _temp1994.last_plus_one= _temp1993 + 2; _temp1994;}); _LL1944: return({
char* _temp1995="*"; struct _tagged_string _temp1996; _temp1996.curr= _temp1995;
_temp1996.base= _temp1995; _temp1996.last_plus_one= _temp1995 + 2; _temp1996;});
_LL1946: return({ char* _temp1997="-"; struct _tagged_string _temp1998;
_temp1998.curr= _temp1997; _temp1998.base= _temp1997; _temp1998.last_plus_one=
_temp1997 + 2; _temp1998;}); _LL1948: return({ char* _temp1999="/"; struct
_tagged_string _temp2000; _temp2000.curr= _temp1999; _temp2000.base= _temp1999;
_temp2000.last_plus_one= _temp1999 + 2; _temp2000;}); _LL1950: return({ char*
_temp2001="%"; struct _tagged_string _temp2002; _temp2002.curr= _temp2001;
_temp2002.base= _temp2001; _temp2002.last_plus_one= _temp2001 + 2; _temp2002;});
_LL1952: return({ char* _temp2003="=="; struct _tagged_string _temp2004;
_temp2004.curr= _temp2003; _temp2004.base= _temp2003; _temp2004.last_plus_one=
_temp2003 + 3; _temp2004;}); _LL1954: return({ char* _temp2005="!="; struct
_tagged_string _temp2006; _temp2006.curr= _temp2005; _temp2006.base= _temp2005;
_temp2006.last_plus_one= _temp2005 + 3; _temp2006;}); _LL1956: return({ char*
_temp2007=">"; struct _tagged_string _temp2008; _temp2008.curr= _temp2007;
_temp2008.base= _temp2007; _temp2008.last_plus_one= _temp2007 + 2; _temp2008;});
_LL1958: return({ char* _temp2009="<"; struct _tagged_string _temp2010;
_temp2010.curr= _temp2009; _temp2010.base= _temp2009; _temp2010.last_plus_one=
_temp2009 + 2; _temp2010;}); _LL1960: return({ char* _temp2011=">="; struct
_tagged_string _temp2012; _temp2012.curr= _temp2011; _temp2012.base= _temp2011;
_temp2012.last_plus_one= _temp2011 + 3; _temp2012;}); _LL1962: return({ char*
_temp2013="<="; struct _tagged_string _temp2014; _temp2014.curr= _temp2013;
_temp2014.base= _temp2013; _temp2014.last_plus_one= _temp2013 + 3; _temp2014;});
_LL1964: return({ char* _temp2015="!"; struct _tagged_string _temp2016;
_temp2016.curr= _temp2015; _temp2016.base= _temp2015; _temp2016.last_plus_one=
_temp2015 + 2; _temp2016;}); _LL1966: return({ char* _temp2017="~"; struct
_tagged_string _temp2018; _temp2018.curr= _temp2017; _temp2018.base= _temp2017;
_temp2018.last_plus_one= _temp2017 + 2; _temp2018;}); _LL1968: return({ char*
_temp2019="&"; struct _tagged_string _temp2020; _temp2020.curr= _temp2019;
_temp2020.base= _temp2019; _temp2020.last_plus_one= _temp2019 + 2; _temp2020;});
_LL1970: return({ char* _temp2021="|"; struct _tagged_string _temp2022;
_temp2022.curr= _temp2021; _temp2022.base= _temp2021; _temp2022.last_plus_one=
_temp2021 + 2; _temp2022;}); _LL1972: return({ char* _temp2023="^"; struct
_tagged_string _temp2024; _temp2024.curr= _temp2023; _temp2024.base= _temp2023;
_temp2024.last_plus_one= _temp2023 + 2; _temp2024;}); _LL1974: return({ char*
_temp2025="<<"; struct _tagged_string _temp2026; _temp2026.curr= _temp2025;
_temp2026.base= _temp2025; _temp2026.last_plus_one= _temp2025 + 3; _temp2026;});
_LL1976: return({ char* _temp2027=">>"; struct _tagged_string _temp2028;
_temp2028.curr= _temp2027; _temp2028.base= _temp2027; _temp2028.last_plus_one=
_temp2027 + 3; _temp2028;}); _LL1978: return({ char* _temp2029=">>>"; struct
_tagged_string _temp2030; _temp2030.curr= _temp2029; _temp2030.base= _temp2029;
_temp2030.last_plus_one= _temp2029 + 4; _temp2030;}); _LL1980: return({ char*
_temp2031="size"; struct _tagged_string _temp2032; _temp2032.curr= _temp2031;
_temp2032.base= _temp2031; _temp2032.last_plus_one= _temp2031 + 5; _temp2032;});
_LL1982: return({ char* _temp2033="printf"; struct _tagged_string _temp2034;
_temp2034.curr= _temp2033; _temp2034.base= _temp2033; _temp2034.last_plus_one=
_temp2033 + 7; _temp2034;}); _LL1984: return({ char* _temp2035="fprintf"; struct
_tagged_string _temp2036; _temp2036.curr= _temp2035; _temp2036.base= _temp2035;
_temp2036.last_plus_one= _temp2035 + 8; _temp2036;}); _LL1986: return({ char*
_temp2037="xprintf"; struct _tagged_string _temp2038; _temp2038.curr= _temp2037;
_temp2038.base= _temp2037; _temp2038.last_plus_one= _temp2037 + 8; _temp2038;});
_LL1988: return({ char* _temp2039="scanf"; struct _tagged_string _temp2040;
_temp2040.curr= _temp2039; _temp2040.base= _temp2039; _temp2040.last_plus_one=
_temp2039 + 6; _temp2040;}); _LL1990: return({ char* _temp2041="fscanf"; struct
_tagged_string _temp2042; _temp2042.curr= _temp2041; _temp2042.base= _temp2041;
_temp2042.last_plus_one= _temp2041 + 7; _temp2042;}); _LL1992: return({ char*
_temp2043="sscanf"; struct _tagged_string _temp2044; _temp2044.curr= _temp2043;
_temp2044.base= _temp2043; _temp2044.last_plus_one= _temp2043 + 7; _temp2044;});
_LL1940:;} struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text(
Cyc_Absynpp_prim2str( p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp*
f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((*
t).f1 == 0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return({ struct Cyc_PP_Doc*(*
_temp2045)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2048= Cyc_PP_textptr((
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v);
struct Cyc_PP_Doc* _temp2049= Cyc_PP_text(({ char* _temp2051=" = "; struct
_tagged_string _temp2052; _temp2052.curr= _temp2051; _temp2052.base= _temp2051;
_temp2052.last_plus_one= _temp2051 + 4; _temp2052;})); struct Cyc_PP_Doc*
_temp2050= Cyc_Absynpp_exp2doc((* t).f2); struct Cyc_PP_Doc* _temp2046[ 3u]={
_temp2048, _temp2049, _temp2050}; struct _tagged_ptr0 _temp2047={ _temp2046,
_temp2046, _temp2046 + 3u}; _temp2045( _temp2047);});}} int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt* s){ void* _temp2053=( void*) s->r; struct Cyc_Absyn_Stmt*
_temp2059; struct Cyc_Absyn_Decl* _temp2061; _LL2055: if(( unsigned int)
_temp2053 > 1u?*(( int*) _temp2053) == Cyc_Absyn_Decl_s: 0){ _LL2062: _temp2061=((
struct Cyc_Absyn_Decl_s_struct*) _temp2053)->f1; goto _LL2060; _LL2060:
_temp2059=(( struct Cyc_Absyn_Decl_s_struct*) _temp2053)->f2; goto _LL2056;}
else{ goto _LL2057;} _LL2057: goto _LL2058; _LL2056: return 1; _LL2058: return 0;
_LL2054:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){
struct Cyc_PP_Doc* s;{ void* _temp2063=( void*) st->r; struct Cyc_Absyn_Exp*
_temp2105; struct Cyc_Absyn_Stmt* _temp2107; struct Cyc_Absyn_Stmt* _temp2109;
struct Cyc_Absyn_Exp* _temp2111; struct Cyc_Absyn_Stmt* _temp2113; struct Cyc_Absyn_Stmt*
_temp2115; struct Cyc_Absyn_Exp* _temp2117; struct Cyc_Absyn_Stmt* _temp2119;
struct _tuple2 _temp2121; struct Cyc_Absyn_Stmt* _temp2123; struct Cyc_Absyn_Exp*
_temp2125; struct Cyc_Absyn_Stmt* _temp2127; struct Cyc_Absyn_Stmt* _temp2129;
struct Cyc_Absyn_Stmt* _temp2131; struct _tagged_string* _temp2133; struct Cyc_Absyn_Stmt*
_temp2135; struct _tuple2 _temp2137; struct Cyc_Absyn_Stmt* _temp2139; struct
Cyc_Absyn_Exp* _temp2141; struct _tuple2 _temp2143; struct Cyc_Absyn_Stmt*
_temp2145; struct Cyc_Absyn_Exp* _temp2147; struct Cyc_Absyn_Exp* _temp2149;
struct Cyc_List_List* _temp2151; struct Cyc_Absyn_Exp* _temp2153; struct Cyc_Absyn_Switch_clause**
_temp2155; struct Cyc_List_List* _temp2157; struct Cyc_Absyn_Switch_clause**
_temp2159; struct Cyc_List_List* _temp2161; struct Cyc_Absyn_Stmt* _temp2163;
struct Cyc_Absyn_Decl* _temp2165; struct Cyc_Absyn_Stmt* _temp2167; struct Cyc_Absyn_Stmt*
_temp2169; struct Cyc_Absyn_Stmt* _temp2171; struct _tagged_string* _temp2173;
struct _tuple2 _temp2175; struct Cyc_Absyn_Stmt* _temp2177; struct Cyc_Absyn_Exp*
_temp2179; struct Cyc_Absyn_Stmt* _temp2181; struct Cyc_List_List* _temp2183;
struct Cyc_Absyn_Stmt* _temp2185; struct Cyc_Absyn_Stmt* _temp2187; struct Cyc_Absyn_Vardecl*
_temp2189; struct Cyc_Absyn_Tvar* _temp2191; _LL2065: if( _temp2063 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2066;} else{ goto _LL2067;} _LL2067: if((
unsigned int) _temp2063 > 1u?*(( int*) _temp2063) == Cyc_Absyn_Exp_s: 0){
_LL2106: _temp2105=(( struct Cyc_Absyn_Exp_s_struct*) _temp2063)->f1; goto
_LL2068;} else{ goto _LL2069;} _LL2069: if(( unsigned int) _temp2063 > 1u?*((
int*) _temp2063) == Cyc_Absyn_Seq_s: 0){ _LL2110: _temp2109=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2063)->f1; goto _LL2108; _LL2108: _temp2107=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2063)->f2; goto _LL2070;} else{ goto _LL2071;} _LL2071: if(( unsigned int)
_temp2063 > 1u?*(( int*) _temp2063) == Cyc_Absyn_Return_s: 0){ _LL2112:
_temp2111=(( struct Cyc_Absyn_Return_s_struct*) _temp2063)->f1; goto _LL2072;}
else{ goto _LL2073;} _LL2073: if(( unsigned int) _temp2063 > 1u?*(( int*)
_temp2063) == Cyc_Absyn_IfThenElse_s: 0){ _LL2118: _temp2117=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2063)->f1; goto _LL2116; _LL2116: _temp2115=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2063)->f2; goto _LL2114; _LL2114: _temp2113=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2063)->f3; goto _LL2074;} else{ goto _LL2075;} _LL2075: if(( unsigned int)
_temp2063 > 1u?*(( int*) _temp2063) == Cyc_Absyn_While_s: 0){ _LL2122: _temp2121=((
struct Cyc_Absyn_While_s_struct*) _temp2063)->f1; _LL2126: _temp2125= _temp2121.f1;
goto _LL2124; _LL2124: _temp2123= _temp2121.f2; goto _LL2120; _LL2120: _temp2119=((
struct Cyc_Absyn_While_s_struct*) _temp2063)->f2; goto _LL2076;} else{ goto
_LL2077;} _LL2077: if(( unsigned int) _temp2063 > 1u?*(( int*) _temp2063) == Cyc_Absyn_Break_s:
0){ _LL2128: _temp2127=(( struct Cyc_Absyn_Break_s_struct*) _temp2063)->f1; goto
_LL2078;} else{ goto _LL2079;} _LL2079: if(( unsigned int) _temp2063 > 1u?*((
int*) _temp2063) == Cyc_Absyn_Continue_s: 0){ _LL2130: _temp2129=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2063)->f1; goto _LL2080;} else{ goto _LL2081;} _LL2081: if(( unsigned int)
_temp2063 > 1u?*(( int*) _temp2063) == Cyc_Absyn_Goto_s: 0){ _LL2134: _temp2133=((
struct Cyc_Absyn_Goto_s_struct*) _temp2063)->f1; goto _LL2132; _LL2132:
_temp2131=(( struct Cyc_Absyn_Goto_s_struct*) _temp2063)->f2; goto _LL2082;}
else{ goto _LL2083;} _LL2083: if(( unsigned int) _temp2063 > 1u?*(( int*)
_temp2063) == Cyc_Absyn_For_s: 0){ _LL2150: _temp2149=(( struct Cyc_Absyn_For_s_struct*)
_temp2063)->f1; goto _LL2144; _LL2144: _temp2143=(( struct Cyc_Absyn_For_s_struct*)
_temp2063)->f2; _LL2148: _temp2147= _temp2143.f1; goto _LL2146; _LL2146:
_temp2145= _temp2143.f2; goto _LL2138; _LL2138: _temp2137=(( struct Cyc_Absyn_For_s_struct*)
_temp2063)->f3; _LL2142: _temp2141= _temp2137.f1; goto _LL2140; _LL2140:
_temp2139= _temp2137.f2; goto _LL2136; _LL2136: _temp2135=(( struct Cyc_Absyn_For_s_struct*)
_temp2063)->f4; goto _LL2084;} else{ goto _LL2085;} _LL2085: if(( unsigned int)
_temp2063 > 1u?*(( int*) _temp2063) == Cyc_Absyn_Switch_s: 0){ _LL2154:
_temp2153=(( struct Cyc_Absyn_Switch_s_struct*) _temp2063)->f1; goto _LL2152;
_LL2152: _temp2151=(( struct Cyc_Absyn_Switch_s_struct*) _temp2063)->f2; goto
_LL2086;} else{ goto _LL2087;} _LL2087: if(( unsigned int) _temp2063 > 1u?*((
int*) _temp2063) == Cyc_Absyn_Fallthru_s: 0){ _LL2158: _temp2157=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2063)->f1; if( _temp2157 == 0){ goto _LL2156;} else{ goto _LL2089;} _LL2156:
_temp2155=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp2063)->f2; goto _LL2088;}
else{ goto _LL2089;} _LL2089: if(( unsigned int) _temp2063 > 1u?*(( int*)
_temp2063) == Cyc_Absyn_Fallthru_s: 0){ _LL2162: _temp2161=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2063)->f1; goto _LL2160; _LL2160: _temp2159=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2063)->f2; goto _LL2090;} else{ goto _LL2091;} _LL2091: if(( unsigned int)
_temp2063 > 1u?*(( int*) _temp2063) == Cyc_Absyn_Decl_s: 0){ _LL2166: _temp2165=((
struct Cyc_Absyn_Decl_s_struct*) _temp2063)->f1; goto _LL2164; _LL2164:
_temp2163=(( struct Cyc_Absyn_Decl_s_struct*) _temp2063)->f2; goto _LL2092;}
else{ goto _LL2093;} _LL2093: if(( unsigned int) _temp2063 > 1u?*(( int*)
_temp2063) == Cyc_Absyn_Cut_s: 0){ _LL2168: _temp2167=(( struct Cyc_Absyn_Cut_s_struct*)
_temp2063)->f1; goto _LL2094;} else{ goto _LL2095;} _LL2095: if(( unsigned int)
_temp2063 > 1u?*(( int*) _temp2063) == Cyc_Absyn_Splice_s: 0){ _LL2170:
_temp2169=(( struct Cyc_Absyn_Splice_s_struct*) _temp2063)->f1; goto _LL2096;}
else{ goto _LL2097;} _LL2097: if(( unsigned int) _temp2063 > 1u?*(( int*)
_temp2063) == Cyc_Absyn_Label_s: 0){ _LL2174: _temp2173=(( struct Cyc_Absyn_Label_s_struct*)
_temp2063)->f1; goto _LL2172; _LL2172: _temp2171=(( struct Cyc_Absyn_Label_s_struct*)
_temp2063)->f2; goto _LL2098;} else{ goto _LL2099;} _LL2099: if(( unsigned int)
_temp2063 > 1u?*(( int*) _temp2063) == Cyc_Absyn_Do_s: 0){ _LL2182: _temp2181=((
struct Cyc_Absyn_Do_s_struct*) _temp2063)->f1; goto _LL2176; _LL2176: _temp2175=((
struct Cyc_Absyn_Do_s_struct*) _temp2063)->f2; _LL2180: _temp2179= _temp2175.f1;
goto _LL2178; _LL2178: _temp2177= _temp2175.f2; goto _LL2100;} else{ goto
_LL2101;} _LL2101: if(( unsigned int) _temp2063 > 1u?*(( int*) _temp2063) == Cyc_Absyn_TryCatch_s:
0){ _LL2186: _temp2185=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2063)->f1;
goto _LL2184; _LL2184: _temp2183=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2063)->f2; goto _LL2102;} else{ goto _LL2103;} _LL2103: if(( unsigned int)
_temp2063 > 1u?*(( int*) _temp2063) == Cyc_Absyn_Region_s: 0){ _LL2192:
_temp2191=(( struct Cyc_Absyn_Region_s_struct*) _temp2063)->f1; goto _LL2190;
_LL2190: _temp2189=(( struct Cyc_Absyn_Region_s_struct*) _temp2063)->f2; goto
_LL2188; _LL2188: _temp2187=(( struct Cyc_Absyn_Region_s_struct*) _temp2063)->f3;
goto _LL2104;} else{ goto _LL2064;} _LL2066: s= Cyc_PP_text(({ char* _temp2193=";";
struct _tagged_string _temp2194; _temp2194.curr= _temp2193; _temp2194.base=
_temp2193; _temp2194.last_plus_one= _temp2193 + 2; _temp2194;})); goto _LL2064;
_LL2068: s=({ struct Cyc_PP_Doc*(* _temp2195)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2198= Cyc_Absynpp_exp2doc( _temp2105); struct Cyc_PP_Doc*
_temp2199= Cyc_PP_text(({ char* _temp2200=";"; struct _tagged_string _temp2201;
_temp2201.curr= _temp2200; _temp2201.base= _temp2200; _temp2201.last_plus_one=
_temp2200 + 2; _temp2201;})); struct Cyc_PP_Doc* _temp2196[ 2u]={ _temp2198,
_temp2199}; struct _tagged_ptr0 _temp2197={ _temp2196, _temp2196, _temp2196 + 2u};
_temp2195( _temp2197);}); goto _LL2064; _LL2070: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp2109)){ s=({ struct Cyc_PP_Doc*(* _temp2202)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2205= Cyc_PP_text(({
char* _temp2224="{ "; struct _tagged_string _temp2225; _temp2225.curr= _temp2224;
_temp2225.base= _temp2224; _temp2225.last_plus_one= _temp2224 + 3; _temp2225;}));
struct Cyc_PP_Doc* _temp2206= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2109));
struct Cyc_PP_Doc* _temp2207= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2208=
Cyc_PP_text(({ char* _temp2222="}"; struct _tagged_string _temp2223; _temp2223.curr=
_temp2222; _temp2223.base= _temp2222; _temp2223.last_plus_one= _temp2222 + 2;
_temp2223;})); struct Cyc_PP_Doc* _temp2209= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2210= Cyc_Absynpp_is_declaration( _temp2107)?({ struct Cyc_PP_Doc*(*
_temp2211)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2214= Cyc_PP_text(({
char* _temp2220="{ "; struct _tagged_string _temp2221; _temp2221.curr= _temp2220;
_temp2221.base= _temp2220; _temp2221.last_plus_one= _temp2220 + 3; _temp2221;}));
struct Cyc_PP_Doc* _temp2215= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2107));
struct Cyc_PP_Doc* _temp2216= Cyc_PP_text(({ char* _temp2218="}"; struct
_tagged_string _temp2219; _temp2219.curr= _temp2218; _temp2219.base= _temp2218;
_temp2219.last_plus_one= _temp2218 + 2; _temp2219;})); struct Cyc_PP_Doc*
_temp2217= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2212[ 4u]={ _temp2214,
_temp2215, _temp2216, _temp2217}; struct _tagged_ptr0 _temp2213={ _temp2212,
_temp2212, _temp2212 + 4u}; _temp2211( _temp2213);}): Cyc_Absynpp_stmt2doc(
_temp2107); struct Cyc_PP_Doc* _temp2203[ 6u]={ _temp2205, _temp2206, _temp2207,
_temp2208, _temp2209, _temp2210}; struct _tagged_ptr0 _temp2204={ _temp2203,
_temp2203, _temp2203 + 6u}; _temp2202( _temp2204);});} else{ if( Cyc_Absynpp_is_declaration(
_temp2107)){ s=({ struct Cyc_PP_Doc*(* _temp2226)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2229= Cyc_Absynpp_stmt2doc( _temp2109); struct Cyc_PP_Doc*
_temp2230= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2231= Cyc_PP_text(({ char*
_temp2237="{ "; struct _tagged_string _temp2238; _temp2238.curr= _temp2237;
_temp2238.base= _temp2237; _temp2238.last_plus_one= _temp2237 + 3; _temp2238;}));
struct Cyc_PP_Doc* _temp2232= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2107));
struct Cyc_PP_Doc* _temp2233= Cyc_PP_text(({ char* _temp2235="}"; struct
_tagged_string _temp2236; _temp2236.curr= _temp2235; _temp2236.base= _temp2235;
_temp2236.last_plus_one= _temp2235 + 2; _temp2236;})); struct Cyc_PP_Doc*
_temp2234= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2227[ 6u]={ _temp2229,
_temp2230, _temp2231, _temp2232, _temp2233, _temp2234}; struct _tagged_ptr0
_temp2228={ _temp2227, _temp2227, _temp2227 + 6u}; _temp2226( _temp2228);});}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({
char* _temp2239=""; struct _tagged_string _temp2240; _temp2240.curr= _temp2239;
_temp2240.base= _temp2239; _temp2240.last_plus_one= _temp2239 + 1; _temp2240;}),({
struct Cyc_List_List* _temp2241=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2241->hd=( void*) _temp2109; _temp2241->tl=({
struct Cyc_List_List* _temp2242=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2242->hd=( void*) _temp2107; _temp2242->tl= 0;
_temp2242;}); _temp2241;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({ char* _temp2243=""; struct
_tagged_string _temp2244; _temp2244.curr= _temp2243; _temp2244.base= _temp2243;
_temp2244.last_plus_one= _temp2243 + 1; _temp2244;}),({ struct Cyc_List_List*
_temp2245=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2245->hd=( void*) _temp2109; _temp2245->tl=({ struct Cyc_List_List*
_temp2246=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2246->hd=( void*) _temp2107; _temp2246->tl= 0; _temp2246;}); _temp2245;}));}
goto _LL2064; _LL2072: if( _temp2111 == 0){ s= Cyc_PP_text(({ char* _temp2247="return;";
struct _tagged_string _temp2248; _temp2248.curr= _temp2247; _temp2248.base=
_temp2247; _temp2248.last_plus_one= _temp2247 + 8; _temp2248;}));} else{ s=({
struct Cyc_PP_Doc*(* _temp2249)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2252= Cyc_PP_text(({ char* _temp2257="return "; struct _tagged_string
_temp2258; _temp2258.curr= _temp2257; _temp2258.base= _temp2257; _temp2258.last_plus_one=
_temp2257 + 8; _temp2258;})); struct Cyc_PP_Doc* _temp2253= _temp2111 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp2111)); struct
Cyc_PP_Doc* _temp2254= Cyc_PP_text(({ char* _temp2255=";"; struct _tagged_string
_temp2256; _temp2256.curr= _temp2255; _temp2256.base= _temp2255; _temp2256.last_plus_one=
_temp2255 + 2; _temp2256;})); struct Cyc_PP_Doc* _temp2250[ 3u]={ _temp2252,
_temp2253, _temp2254}; struct _tagged_ptr0 _temp2251={ _temp2250, _temp2250,
_temp2250 + 3u}; _temp2249( _temp2251);});} goto _LL2064; _LL2074: { int
print_else;{ void* _temp2259=( void*) _temp2113->r; _LL2261: if( _temp2259 ==(
void*) Cyc_Absyn_Skip_s){ goto _LL2262;} else{ goto _LL2263;} _LL2263: goto
_LL2264; _LL2262: print_else= 0; goto _LL2260; _LL2264: print_else= 1; goto
_LL2260; _LL2260:;} s=({ struct Cyc_PP_Doc*(* _temp2265)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2268= Cyc_PP_text(({ char* _temp2301="if (";
struct _tagged_string _temp2302; _temp2302.curr= _temp2301; _temp2302.base=
_temp2301; _temp2302.last_plus_one= _temp2301 + 5; _temp2302;})); struct Cyc_PP_Doc*
_temp2269= Cyc_Absynpp_exp2doc( _temp2117); struct Cyc_PP_Doc* _temp2270= Cyc_PP_text(({
char* _temp2299=") {"; struct _tagged_string _temp2300; _temp2300.curr=
_temp2299; _temp2300.base= _temp2299; _temp2300.last_plus_one= _temp2299 + 4;
_temp2300;})); struct Cyc_PP_Doc* _temp2271= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2294)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2297= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2298= Cyc_Absynpp_stmt2doc( _temp2115); struct Cyc_PP_Doc*
_temp2295[ 2u]={ _temp2297, _temp2298}; struct _tagged_ptr0 _temp2296={
_temp2295, _temp2295, _temp2295 + 2u}; _temp2294( _temp2296);})); struct Cyc_PP_Doc*
_temp2272= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2273= Cyc_PP_text(({ char*
_temp2292="}"; struct _tagged_string _temp2293; _temp2293.curr= _temp2292;
_temp2293.base= _temp2292; _temp2293.last_plus_one= _temp2292 + 2; _temp2293;}));
struct Cyc_PP_Doc* _temp2274= print_else?({ struct Cyc_PP_Doc*(* _temp2275)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2278= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2279= Cyc_PP_text(({ char* _temp2290="else {"; struct
_tagged_string _temp2291; _temp2291.curr= _temp2290; _temp2291.base= _temp2290;
_temp2291.last_plus_one= _temp2290 + 7; _temp2291;})); struct Cyc_PP_Doc*
_temp2280= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2285)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2288= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2289= Cyc_Absynpp_stmt2doc( _temp2113); struct Cyc_PP_Doc*
_temp2286[ 2u]={ _temp2288, _temp2289}; struct _tagged_ptr0 _temp2287={
_temp2286, _temp2286, _temp2286 + 2u}; _temp2285( _temp2287);})); struct Cyc_PP_Doc*
_temp2281= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2282= Cyc_PP_text(({ char*
_temp2283="}"; struct _tagged_string _temp2284; _temp2284.curr= _temp2283;
_temp2284.base= _temp2283; _temp2284.last_plus_one= _temp2283 + 2; _temp2284;}));
struct Cyc_PP_Doc* _temp2276[ 5u]={ _temp2278, _temp2279, _temp2280, _temp2281,
_temp2282}; struct _tagged_ptr0 _temp2277={ _temp2276, _temp2276, _temp2276 + 5u};
_temp2275( _temp2277);}): Cyc_PP_nil_doc(); struct Cyc_PP_Doc* _temp2266[ 7u]={
_temp2268, _temp2269, _temp2270, _temp2271, _temp2272, _temp2273, _temp2274};
struct _tagged_ptr0 _temp2267={ _temp2266, _temp2266, _temp2266 + 7u}; _temp2265(
_temp2267);}); goto _LL2064;} _LL2076: s=({ struct Cyc_PP_Doc*(* _temp2303)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2306= Cyc_PP_text(({
char* _temp2321="while ("; struct _tagged_string _temp2322; _temp2322.curr=
_temp2321; _temp2322.base= _temp2321; _temp2322.last_plus_one= _temp2321 + 8;
_temp2322;})); struct Cyc_PP_Doc* _temp2307= Cyc_Absynpp_exp2doc( _temp2125);
struct Cyc_PP_Doc* _temp2308= Cyc_PP_text(({ char* _temp2319=") {"; struct
_tagged_string _temp2320; _temp2320.curr= _temp2319; _temp2320.base= _temp2319;
_temp2320.last_plus_one= _temp2319 + 4; _temp2320;})); struct Cyc_PP_Doc*
_temp2309= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2314)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2317= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2318= Cyc_Absynpp_stmt2doc( _temp2119); struct Cyc_PP_Doc*
_temp2315[ 2u]={ _temp2317, _temp2318}; struct _tagged_ptr0 _temp2316={
_temp2315, _temp2315, _temp2315 + 2u}; _temp2314( _temp2316);})); struct Cyc_PP_Doc*
_temp2310= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2311= Cyc_PP_text(({ char*
_temp2312="}"; struct _tagged_string _temp2313; _temp2313.curr= _temp2312;
_temp2313.base= _temp2312; _temp2313.last_plus_one= _temp2312 + 2; _temp2313;}));
struct Cyc_PP_Doc* _temp2304[ 6u]={ _temp2306, _temp2307, _temp2308, _temp2309,
_temp2310, _temp2311}; struct _tagged_ptr0 _temp2305={ _temp2304, _temp2304,
_temp2304 + 6u}; _temp2303( _temp2305);}); goto _LL2064; _LL2078: s= Cyc_PP_text(({
char* _temp2323="break;"; struct _tagged_string _temp2324; _temp2324.curr=
_temp2323; _temp2324.base= _temp2323; _temp2324.last_plus_one= _temp2323 + 7;
_temp2324;})); goto _LL2064; _LL2080: s= Cyc_PP_text(({ char* _temp2325="continue;";
struct _tagged_string _temp2326; _temp2326.curr= _temp2325; _temp2326.base=
_temp2325; _temp2326.last_plus_one= _temp2325 + 10; _temp2326;})); goto _LL2064;
_LL2082: s= Cyc_PP_text(({ struct _tagged_string _temp2327=* _temp2133; xprintf("goto %.*s;",
_temp2327.last_plus_one - _temp2327.curr, _temp2327.curr);})); goto _LL2064;
_LL2084: s=({ struct Cyc_PP_Doc*(* _temp2328)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2331= Cyc_PP_text(({ char* _temp2354="for("; struct
_tagged_string _temp2355; _temp2355.curr= _temp2354; _temp2355.base= _temp2354;
_temp2355.last_plus_one= _temp2354 + 5; _temp2355;})); struct Cyc_PP_Doc*
_temp2332= Cyc_Absynpp_exp2doc( _temp2149); struct Cyc_PP_Doc* _temp2333= Cyc_PP_text(({
char* _temp2352="; "; struct _tagged_string _temp2353; _temp2353.curr= _temp2352;
_temp2353.base= _temp2352; _temp2353.last_plus_one= _temp2352 + 3; _temp2353;}));
struct Cyc_PP_Doc* _temp2334= Cyc_Absynpp_exp2doc( _temp2147); struct Cyc_PP_Doc*
_temp2335= Cyc_PP_text(({ char* _temp2350="; "; struct _tagged_string _temp2351;
_temp2351.curr= _temp2350; _temp2351.base= _temp2350; _temp2351.last_plus_one=
_temp2350 + 3; _temp2351;})); struct Cyc_PP_Doc* _temp2336= Cyc_Absynpp_exp2doc(
_temp2141); struct Cyc_PP_Doc* _temp2337= Cyc_PP_text(({ char* _temp2348=") {";
struct _tagged_string _temp2349; _temp2349.curr= _temp2348; _temp2349.base=
_temp2348; _temp2349.last_plus_one= _temp2348 + 4; _temp2349;})); struct Cyc_PP_Doc*
_temp2338= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2343)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2346= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2347= Cyc_Absynpp_stmt2doc( _temp2135); struct Cyc_PP_Doc*
_temp2344[ 2u]={ _temp2346, _temp2347}; struct _tagged_ptr0 _temp2345={
_temp2344, _temp2344, _temp2344 + 2u}; _temp2343( _temp2345);})); struct Cyc_PP_Doc*
_temp2339= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2340= Cyc_PP_text(({ char*
_temp2341="}"; struct _tagged_string _temp2342; _temp2342.curr= _temp2341;
_temp2342.base= _temp2341; _temp2342.last_plus_one= _temp2341 + 2; _temp2342;}));
struct Cyc_PP_Doc* _temp2329[ 10u]={ _temp2331, _temp2332, _temp2333, _temp2334,
_temp2335, _temp2336, _temp2337, _temp2338, _temp2339, _temp2340}; struct
_tagged_ptr0 _temp2330={ _temp2329, _temp2329, _temp2329 + 10u}; _temp2328(
_temp2330);}); goto _LL2064; _LL2086: s=({ struct Cyc_PP_Doc*(* _temp2356)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2359= Cyc_PP_text(({
char* _temp2370="switch ("; struct _tagged_string _temp2371; _temp2371.curr=
_temp2370; _temp2371.base= _temp2370; _temp2371.last_plus_one= _temp2370 + 9;
_temp2371;})); struct Cyc_PP_Doc* _temp2360= Cyc_Absynpp_exp2doc( _temp2153);
struct Cyc_PP_Doc* _temp2361= Cyc_PP_text(({ char* _temp2368=") {"; struct
_tagged_string _temp2369; _temp2369.curr= _temp2368; _temp2369.base= _temp2368;
_temp2369.last_plus_one= _temp2368 + 4; _temp2369;})); struct Cyc_PP_Doc*
_temp2362= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2363= Cyc_Absynpp_switchclauses2doc(
_temp2151); struct Cyc_PP_Doc* _temp2364= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2365= Cyc_PP_text(({ char* _temp2366="}"; struct _tagged_string _temp2367;
_temp2367.curr= _temp2366; _temp2367.base= _temp2366; _temp2367.last_plus_one=
_temp2366 + 2; _temp2367;})); struct Cyc_PP_Doc* _temp2357[ 7u]={ _temp2359,
_temp2360, _temp2361, _temp2362, _temp2363, _temp2364, _temp2365}; struct
_tagged_ptr0 _temp2358={ _temp2357, _temp2357, _temp2357 + 7u}; _temp2356(
_temp2358);}); goto _LL2064; _LL2088: s= Cyc_PP_text(({ char* _temp2372="fallthru;";
struct _tagged_string _temp2373; _temp2373.curr= _temp2372; _temp2373.base=
_temp2372; _temp2373.last_plus_one= _temp2372 + 10; _temp2373;})); goto _LL2064;
_LL2090: s=({ struct Cyc_PP_Doc*(* _temp2374)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2377= Cyc_PP_text(({ char* _temp2382="fallthru("; struct
_tagged_string _temp2383; _temp2383.curr= _temp2382; _temp2383.base= _temp2382;
_temp2383.last_plus_one= _temp2382 + 10; _temp2383;})); struct Cyc_PP_Doc*
_temp2378= Cyc_Absynpp_exps2doc_prec( 20, _temp2161); struct Cyc_PP_Doc*
_temp2379= Cyc_PP_text(({ char* _temp2380=");"; struct _tagged_string _temp2381;
_temp2381.curr= _temp2380; _temp2381.base= _temp2380; _temp2381.last_plus_one=
_temp2380 + 3; _temp2381;})); struct Cyc_PP_Doc* _temp2375[ 3u]={ _temp2377,
_temp2378, _temp2379}; struct _tagged_ptr0 _temp2376={ _temp2375, _temp2375,
_temp2375 + 3u}; _temp2374( _temp2376);}); goto _LL2064; _LL2092: s=({ struct
Cyc_PP_Doc*(* _temp2384)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2387= Cyc_Absynpp_decl2doc( _temp2165); struct Cyc_PP_Doc* _temp2388= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2389= Cyc_Absynpp_stmt2doc( _temp2163); struct Cyc_PP_Doc*
_temp2385[ 3u]={ _temp2387, _temp2388, _temp2389}; struct _tagged_ptr0 _temp2386={
_temp2385, _temp2385, _temp2385 + 3u}; _temp2384( _temp2386);}); goto _LL2064;
_LL2094: s=({ struct Cyc_PP_Doc*(* _temp2390)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2393= Cyc_PP_text(({ char* _temp2395="cut "; struct
_tagged_string _temp2396; _temp2396.curr= _temp2395; _temp2396.base= _temp2395;
_temp2396.last_plus_one= _temp2395 + 5; _temp2396;})); struct Cyc_PP_Doc*
_temp2394= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2167)); struct Cyc_PP_Doc*
_temp2391[ 2u]={ _temp2393, _temp2394}; struct _tagged_ptr0 _temp2392={
_temp2391, _temp2391, _temp2391 + 2u}; _temp2390( _temp2392);}); goto _LL2064;
_LL2096: s=({ struct Cyc_PP_Doc*(* _temp2397)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2400= Cyc_PP_text(({ char* _temp2402="splice "; struct
_tagged_string _temp2403; _temp2403.curr= _temp2402; _temp2403.base= _temp2402;
_temp2403.last_plus_one= _temp2402 + 8; _temp2403;})); struct Cyc_PP_Doc*
_temp2401= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2169)); struct Cyc_PP_Doc*
_temp2398[ 2u]={ _temp2400, _temp2401}; struct _tagged_ptr0 _temp2399={
_temp2398, _temp2398, _temp2398 + 2u}; _temp2397( _temp2399);}); goto _LL2064;
_LL2098: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration( _temp2171): 0){
s=({ struct Cyc_PP_Doc*(* _temp2404)( struct _tagged_ptr0)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2407= Cyc_PP_textptr( _temp2173); struct Cyc_PP_Doc* _temp2408=
Cyc_PP_text(({ char* _temp2415=": {"; struct _tagged_string _temp2416; _temp2416.curr=
_temp2415; _temp2416.base= _temp2415; _temp2416.last_plus_one= _temp2415 + 4;
_temp2416;})); struct Cyc_PP_Doc* _temp2409= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2410= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2171)); struct Cyc_PP_Doc*
_temp2411= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2412= Cyc_PP_text(({ char*
_temp2413="}"; struct _tagged_string _temp2414; _temp2414.curr= _temp2413;
_temp2414.base= _temp2413; _temp2414.last_plus_one= _temp2413 + 2; _temp2414;}));
struct Cyc_PP_Doc* _temp2405[ 6u]={ _temp2407, _temp2408, _temp2409, _temp2410,
_temp2411, _temp2412}; struct _tagged_ptr0 _temp2406={ _temp2405, _temp2405,
_temp2405 + 6u}; _temp2404( _temp2406);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp2417)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2420= Cyc_PP_textptr(
_temp2173); struct Cyc_PP_Doc* _temp2421= Cyc_PP_text(({ char* _temp2423=": ";
struct _tagged_string _temp2424; _temp2424.curr= _temp2423; _temp2424.base=
_temp2423; _temp2424.last_plus_one= _temp2423 + 3; _temp2424;})); struct Cyc_PP_Doc*
_temp2422= Cyc_Absynpp_stmt2doc( _temp2171); struct Cyc_PP_Doc* _temp2418[ 3u]={
_temp2420, _temp2421, _temp2422}; struct _tagged_ptr0 _temp2419={ _temp2418,
_temp2418, _temp2418 + 3u}; _temp2417( _temp2419);});} goto _LL2064; _LL2100: s=({
struct Cyc_PP_Doc*(* _temp2425)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2428= Cyc_PP_text(({ char* _temp2439="do {"; struct _tagged_string
_temp2440; _temp2440.curr= _temp2439; _temp2440.base= _temp2439; _temp2440.last_plus_one=
_temp2439 + 5; _temp2440;})); struct Cyc_PP_Doc* _temp2429= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2430= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2181));
struct Cyc_PP_Doc* _temp2431= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2432=
Cyc_PP_text(({ char* _temp2437="} while ("; struct _tagged_string _temp2438;
_temp2438.curr= _temp2437; _temp2438.base= _temp2437; _temp2438.last_plus_one=
_temp2437 + 10; _temp2438;})); struct Cyc_PP_Doc* _temp2433= Cyc_Absynpp_exp2doc(
_temp2179); struct Cyc_PP_Doc* _temp2434= Cyc_PP_text(({ char* _temp2435=");";
struct _tagged_string _temp2436; _temp2436.curr= _temp2435; _temp2436.base=
_temp2435; _temp2436.last_plus_one= _temp2435 + 3; _temp2436;})); struct Cyc_PP_Doc*
_temp2426[ 7u]={ _temp2428, _temp2429, _temp2430, _temp2431, _temp2432,
_temp2433, _temp2434}; struct _tagged_ptr0 _temp2427={ _temp2426, _temp2426,
_temp2426 + 7u}; _temp2425( _temp2427);}); goto _LL2064; _LL2102: s=({ struct
Cyc_PP_Doc*(* _temp2441)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2444= Cyc_PP_text(({ char* _temp2457="try {"; struct _tagged_string
_temp2458; _temp2458.curr= _temp2457; _temp2458.base= _temp2457; _temp2458.last_plus_one=
_temp2457 + 6; _temp2458;})); struct Cyc_PP_Doc* _temp2445= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2446= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2185));
struct Cyc_PP_Doc* _temp2447= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2448=
Cyc_PP_text(({ char* _temp2455="} catch {"; struct _tagged_string _temp2456;
_temp2456.curr= _temp2455; _temp2456.base= _temp2455; _temp2456.last_plus_one=
_temp2455 + 10; _temp2456;})); struct Cyc_PP_Doc* _temp2449= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2450= Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp2183)); struct Cyc_PP_Doc* _temp2451= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2452= Cyc_PP_text(({ char* _temp2453="}"; struct _tagged_string _temp2454;
_temp2454.curr= _temp2453; _temp2454.base= _temp2453; _temp2454.last_plus_one=
_temp2453 + 2; _temp2454;})); struct Cyc_PP_Doc* _temp2442[ 9u]={ _temp2444,
_temp2445, _temp2446, _temp2447, _temp2448, _temp2449, _temp2450, _temp2451,
_temp2452}; struct _tagged_ptr0 _temp2443={ _temp2442, _temp2442, _temp2442 + 9u};
_temp2441( _temp2443);}); goto _LL2064; _LL2104: s=({ struct Cyc_PP_Doc*(*
_temp2459)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2462= Cyc_PP_text(({
char* _temp2477="region<"; struct _tagged_string _temp2478; _temp2478.curr=
_temp2477; _temp2478.base= _temp2477; _temp2478.last_plus_one= _temp2477 + 8;
_temp2478;})); struct Cyc_PP_Doc* _temp2463= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp2191)); struct Cyc_PP_Doc* _temp2464= Cyc_PP_text(({ char* _temp2475=">";
struct _tagged_string _temp2476; _temp2476.curr= _temp2475; _temp2476.base=
_temp2475; _temp2476.last_plus_one= _temp2475 + 2; _temp2476;})); struct Cyc_PP_Doc*
_temp2465= Cyc_Absynpp_qvar2doc( _temp2189->name); struct Cyc_PP_Doc* _temp2466=
Cyc_PP_text(({ char* _temp2473="{"; struct _tagged_string _temp2474; _temp2474.curr=
_temp2473; _temp2474.base= _temp2473; _temp2474.last_plus_one= _temp2473 + 2;
_temp2474;})); struct Cyc_PP_Doc* _temp2467= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2468= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2187)); struct Cyc_PP_Doc*
_temp2469= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2470= Cyc_PP_text(({ char*
_temp2471="}"; struct _tagged_string _temp2472; _temp2472.curr= _temp2471;
_temp2472.base= _temp2471; _temp2472.last_plus_one= _temp2471 + 2; _temp2472;}));
struct Cyc_PP_Doc* _temp2460[ 9u]={ _temp2462, _temp2463, _temp2464, _temp2465,
_temp2466, _temp2467, _temp2468, _temp2469, _temp2470}; struct _tagged_ptr0
_temp2461={ _temp2460, _temp2460, _temp2460 + 9u}; _temp2459( _temp2461);});
goto _LL2064; _LL2064:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp2479=( void*) p->r;
int _temp2511; void* _temp2513; char _temp2515; struct _tagged_string _temp2517;
struct Cyc_Absyn_Vardecl* _temp2519; struct Cyc_List_List* _temp2521; struct Cyc_Absyn_Pat*
_temp2523; struct Cyc_Absyn_Vardecl* _temp2525; struct _tuple0* _temp2527;
struct Cyc_List_List* _temp2529; struct Cyc_List_List* _temp2531; struct _tuple0*
_temp2533; struct Cyc_List_List* _temp2535; struct Cyc_List_List* _temp2537;
struct _tuple0* _temp2539; struct Cyc_List_List* _temp2541; struct Cyc_List_List*
_temp2543; struct Cyc_Core_Opt* _temp2545; struct Cyc_Absyn_Structdecl*
_temp2547; struct Cyc_Absyn_Enumfield* _temp2549; struct Cyc_Absyn_Enumdecl*
_temp2551; struct Cyc_List_List* _temp2553; struct Cyc_List_List* _temp2555;
struct Cyc_Absyn_Tunionfield* _temp2557; struct Cyc_Absyn_Tuniondecl* _temp2559;
_LL2481: if( _temp2479 ==( void*) Cyc_Absyn_Wild_p){ goto _LL2482;} else{ goto
_LL2483;} _LL2483: if( _temp2479 ==( void*) Cyc_Absyn_Null_p){ goto _LL2484;}
else{ goto _LL2485;} _LL2485: if(( unsigned int) _temp2479 > 2u?*(( int*)
_temp2479) == Cyc_Absyn_Int_p: 0){ _LL2514: _temp2513=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp2479)->f1; goto _LL2512; _LL2512: _temp2511=(( struct Cyc_Absyn_Int_p_struct*)
_temp2479)->f2; goto _LL2486;} else{ goto _LL2487;} _LL2487: if(( unsigned int)
_temp2479 > 2u?*(( int*) _temp2479) == Cyc_Absyn_Char_p: 0){ _LL2516: _temp2515=((
struct Cyc_Absyn_Char_p_struct*) _temp2479)->f1; goto _LL2488;} else{ goto
_LL2489;} _LL2489: if(( unsigned int) _temp2479 > 2u?*(( int*) _temp2479) == Cyc_Absyn_Float_p:
0){ _LL2518: _temp2517=(( struct Cyc_Absyn_Float_p_struct*) _temp2479)->f1; goto
_LL2490;} else{ goto _LL2491;} _LL2491: if(( unsigned int) _temp2479 > 2u?*((
int*) _temp2479) == Cyc_Absyn_Var_p: 0){ _LL2520: _temp2519=(( struct Cyc_Absyn_Var_p_struct*)
_temp2479)->f1; goto _LL2492;} else{ goto _LL2493;} _LL2493: if(( unsigned int)
_temp2479 > 2u?*(( int*) _temp2479) == Cyc_Absyn_Tuple_p: 0){ _LL2522: _temp2521=((
struct Cyc_Absyn_Tuple_p_struct*) _temp2479)->f1; goto _LL2494;} else{ goto
_LL2495;} _LL2495: if(( unsigned int) _temp2479 > 2u?*(( int*) _temp2479) == Cyc_Absyn_Pointer_p:
0){ _LL2524: _temp2523=(( struct Cyc_Absyn_Pointer_p_struct*) _temp2479)->f1;
goto _LL2496;} else{ goto _LL2497;} _LL2497: if(( unsigned int) _temp2479 > 2u?*((
int*) _temp2479) == Cyc_Absyn_Reference_p: 0){ _LL2526: _temp2525=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2479)->f1; goto _LL2498;} else{ goto _LL2499;} _LL2499: if(( unsigned int)
_temp2479 > 2u?*(( int*) _temp2479) == Cyc_Absyn_UnknownId_p: 0){ _LL2528:
_temp2527=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2479)->f1; goto _LL2500;}
else{ goto _LL2501;} _LL2501: if(( unsigned int) _temp2479 > 2u?*(( int*)
_temp2479) == Cyc_Absyn_UnknownCall_p: 0){ _LL2534: _temp2533=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2479)->f1; goto _LL2532; _LL2532: _temp2531=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2479)->f2; goto _LL2530; _LL2530: _temp2529=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2479)->f3; goto _LL2502;} else{ goto _LL2503;} _LL2503: if(( unsigned int)
_temp2479 > 2u?*(( int*) _temp2479) == Cyc_Absyn_UnknownFields_p: 0){ _LL2540:
_temp2539=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2479)->f1; goto
_LL2538; _LL2538: _temp2537=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2479)->f2; goto _LL2536; _LL2536: _temp2535=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2479)->f3; goto _LL2504;} else{ goto _LL2505;} _LL2505: if(( unsigned int)
_temp2479 > 2u?*(( int*) _temp2479) == Cyc_Absyn_Struct_p: 0){ _LL2548:
_temp2547=(( struct Cyc_Absyn_Struct_p_struct*) _temp2479)->f1; goto _LL2546;
_LL2546: _temp2545=(( struct Cyc_Absyn_Struct_p_struct*) _temp2479)->f2; goto
_LL2544; _LL2544: _temp2543=(( struct Cyc_Absyn_Struct_p_struct*) _temp2479)->f3;
goto _LL2542; _LL2542: _temp2541=(( struct Cyc_Absyn_Struct_p_struct*) _temp2479)->f4;
goto _LL2506;} else{ goto _LL2507;} _LL2507: if(( unsigned int) _temp2479 > 2u?*((
int*) _temp2479) == Cyc_Absyn_Enum_p: 0){ _LL2552: _temp2551=(( struct Cyc_Absyn_Enum_p_struct*)
_temp2479)->f1; goto _LL2550; _LL2550: _temp2549=(( struct Cyc_Absyn_Enum_p_struct*)
_temp2479)->f2; goto _LL2508;} else{ goto _LL2509;} _LL2509: if(( unsigned int)
_temp2479 > 2u?*(( int*) _temp2479) == Cyc_Absyn_Tunion_p: 0){ _LL2560:
_temp2559=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2479)->f1; goto _LL2558;
_LL2558: _temp2557=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2479)->f2; goto
_LL2556; _LL2556: _temp2555=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2479)->f3;
goto _LL2554; _LL2554: _temp2553=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2479)->f4;
goto _LL2510;} else{ goto _LL2480;} _LL2482: s= Cyc_PP_text(({ char* _temp2561="_";
struct _tagged_string _temp2562; _temp2562.curr= _temp2561; _temp2562.base=
_temp2561; _temp2562.last_plus_one= _temp2561 + 2; _temp2562;})); goto _LL2480;
_LL2484: s= Cyc_PP_text(({ char* _temp2563="null"; struct _tagged_string
_temp2564; _temp2564.curr= _temp2563; _temp2564.base= _temp2563; _temp2564.last_plus_one=
_temp2563 + 5; _temp2564;})); goto _LL2480; _LL2486: if( _temp2513 ==( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d", _temp2511));} else{ s= Cyc_PP_text(
xprintf("%u",( unsigned int) _temp2511));} goto _LL2480; _LL2488: s= Cyc_PP_text(({
struct _tagged_string _temp2565= Cyc_Absynpp_char_escape( _temp2515); xprintf("'%.*s'",
_temp2565.last_plus_one - _temp2565.curr, _temp2565.curr);})); goto _LL2480;
_LL2490: s= Cyc_PP_text( _temp2517); goto _LL2480; _LL2492: s= Cyc_Absynpp_qvar2doc(
_temp2519->name); goto _LL2480; _LL2494: s=({ struct Cyc_PP_Doc*(* _temp2566)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2569= Cyc_PP_text(({
char* _temp2576="$("; struct _tagged_string _temp2577; _temp2577.curr= _temp2576;
_temp2577.base= _temp2576; _temp2577.last_plus_one= _temp2576 + 3; _temp2577;}));
struct Cyc_PP_Doc* _temp2570=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Pat*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,({ char* _temp2574=","; struct _tagged_string
_temp2575; _temp2575.curr= _temp2574; _temp2575.base= _temp2574; _temp2575.last_plus_one=
_temp2574 + 2; _temp2575;}), _temp2521); struct Cyc_PP_Doc* _temp2571= Cyc_PP_text(({
char* _temp2572=")"; struct _tagged_string _temp2573; _temp2573.curr= _temp2572;
_temp2573.base= _temp2572; _temp2573.last_plus_one= _temp2572 + 2; _temp2573;}));
struct Cyc_PP_Doc* _temp2567[ 3u]={ _temp2569, _temp2570, _temp2571}; struct
_tagged_ptr0 _temp2568={ _temp2567, _temp2567, _temp2567 + 3u}; _temp2566(
_temp2568);}); goto _LL2480; _LL2496: s=({ struct Cyc_PP_Doc*(* _temp2578)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2581= Cyc_PP_text(({
char* _temp2583="&"; struct _tagged_string _temp2584; _temp2584.curr= _temp2583;
_temp2584.base= _temp2583; _temp2584.last_plus_one= _temp2583 + 2; _temp2584;}));
struct Cyc_PP_Doc* _temp2582= Cyc_Absynpp_pat2doc( _temp2523); struct Cyc_PP_Doc*
_temp2579[ 2u]={ _temp2581, _temp2582}; struct _tagged_ptr0 _temp2580={
_temp2579, _temp2579, _temp2579 + 2u}; _temp2578( _temp2580);}); goto _LL2480;
_LL2498: s=({ struct Cyc_PP_Doc*(* _temp2585)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2588= Cyc_PP_text(({ char* _temp2590="*"; struct
_tagged_string _temp2591; _temp2591.curr= _temp2590; _temp2591.base= _temp2590;
_temp2591.last_plus_one= _temp2590 + 2; _temp2591;})); struct Cyc_PP_Doc*
_temp2589= Cyc_Absynpp_qvar2doc( _temp2525->name); struct Cyc_PP_Doc* _temp2586[
2u]={ _temp2588, _temp2589}; struct _tagged_ptr0 _temp2587={ _temp2586,
_temp2586, _temp2586 + 2u}; _temp2585( _temp2587);}); goto _LL2480; _LL2500: s=
Cyc_Absynpp_qvar2doc( _temp2527); goto _LL2480; _LL2502: s=({ struct Cyc_PP_Doc*(*
_temp2592)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2595= Cyc_Absynpp_qvar2doc(
_temp2533); struct Cyc_PP_Doc* _temp2596= Cyc_Absynpp_tvars2doc( _temp2531);
struct Cyc_PP_Doc* _temp2597= Cyc_PP_group(({ char* _temp2598="("; struct
_tagged_string _temp2599; _temp2599.curr= _temp2598; _temp2599.base= _temp2598;
_temp2599.last_plus_one= _temp2598 + 2; _temp2599;}),({ char* _temp2600=")";
struct _tagged_string _temp2601; _temp2601.curr= _temp2600; _temp2601.base=
_temp2600; _temp2601.last_plus_one= _temp2600 + 2; _temp2601;}),({ char*
_temp2602=","; struct _tagged_string _temp2603; _temp2603.curr= _temp2602;
_temp2603.base= _temp2602; _temp2603.last_plus_one= _temp2602 + 2; _temp2603;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp2529));
struct Cyc_PP_Doc* _temp2593[ 3u]={ _temp2595, _temp2596, _temp2597}; struct
_tagged_ptr0 _temp2594={ _temp2593, _temp2593, _temp2593 + 3u}; _temp2592(
_temp2594);}); goto _LL2480; _LL2504: s=({ struct Cyc_PP_Doc*(* _temp2604)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2607= Cyc_Absynpp_qvar2doc(
_temp2539); struct Cyc_PP_Doc* _temp2608= Cyc_Absynpp_tvars2doc( _temp2537);
struct Cyc_PP_Doc* _temp2609= Cyc_PP_group(({ char* _temp2610="{"; struct
_tagged_string _temp2611; _temp2611.curr= _temp2610; _temp2611.base= _temp2610;
_temp2611.last_plus_one= _temp2610 + 2; _temp2611;}),({ char* _temp2612="}";
struct _tagged_string _temp2613; _temp2613.curr= _temp2612; _temp2613.base=
_temp2612; _temp2613.last_plus_one= _temp2612 + 2; _temp2613;}),({ char*
_temp2614=","; struct _tagged_string _temp2615; _temp2615.curr= _temp2614;
_temp2615.base= _temp2614; _temp2615.last_plus_one= _temp2614 + 2; _temp2615;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_dp2doc, _temp2535)); struct Cyc_PP_Doc* _temp2605[
3u]={ _temp2607, _temp2608, _temp2609}; struct _tagged_ptr0 _temp2606={
_temp2605, _temp2605, _temp2605 + 3u}; _temp2604( _temp2606);}); goto _LL2480;
_LL2506: s=({ struct Cyc_PP_Doc*(* _temp2616)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2619= _temp2547->name == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2547->name))->v);
struct Cyc_PP_Doc* _temp2620= Cyc_Absynpp_tvars2doc( _temp2543); struct Cyc_PP_Doc*
_temp2621= Cyc_PP_group(({ char* _temp2622="{"; struct _tagged_string _temp2623;
_temp2623.curr= _temp2622; _temp2623.base= _temp2622; _temp2623.last_plus_one=
_temp2622 + 2; _temp2623;}),({ char* _temp2624="}"; struct _tagged_string
_temp2625; _temp2625.curr= _temp2624; _temp2625.base= _temp2624; _temp2625.last_plus_one=
_temp2624 + 2; _temp2625;}),({ char* _temp2626=","; struct _tagged_string
_temp2627; _temp2627.curr= _temp2626; _temp2627.base= _temp2626; _temp2627.last_plus_one=
_temp2626 + 2; _temp2627;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp2541)); struct Cyc_PP_Doc* _temp2617[ 3u]={ _temp2619, _temp2620, _temp2621};
struct _tagged_ptr0 _temp2618={ _temp2617, _temp2617, _temp2617 + 3u}; _temp2616(
_temp2618);}); goto _LL2480; _LL2508: s= Cyc_Absynpp_qvar2doc( _temp2549->name);
goto _LL2480; _LL2510: if( _temp2553 == 0){ s= Cyc_Absynpp_qvar2doc( _temp2557->name);}
else{ s=({ struct Cyc_PP_Doc*(* _temp2628)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2631= Cyc_Absynpp_qvar2doc( _temp2557->name); struct Cyc_PP_Doc*
_temp2632= Cyc_Absynpp_tvars2doc( _temp2555); struct Cyc_PP_Doc* _temp2633= Cyc_PP_egroup(({
char* _temp2634="("; struct _tagged_string _temp2635; _temp2635.curr= _temp2634;
_temp2635.base= _temp2634; _temp2635.last_plus_one= _temp2634 + 2; _temp2635;}),({
char* _temp2636=")"; struct _tagged_string _temp2637; _temp2637.curr= _temp2636;
_temp2637.base= _temp2636; _temp2637.last_plus_one= _temp2636 + 2; _temp2637;}),({
char* _temp2638=","; struct _tagged_string _temp2639; _temp2639.curr= _temp2638;
_temp2639.base= _temp2638; _temp2639.last_plus_one= _temp2638 + 2; _temp2639;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp2553));
struct Cyc_PP_Doc* _temp2629[ 3u]={ _temp2631, _temp2632, _temp2633}; struct
_tagged_ptr0 _temp2630={ _temp2629, _temp2629, _temp2629 + 3u}; _temp2628(
_temp2630);});} goto _LL2480; _LL2480:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc(
struct _tuple5* dp){ return({ struct Cyc_PP_Doc*(* _temp2640)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2643= Cyc_PP_egroup(({ char*
_temp2645=""; struct _tagged_string _temp2646; _temp2646.curr= _temp2645;
_temp2646.base= _temp2645; _temp2646.last_plus_one= _temp2645 + 1; _temp2646;}),({
char* _temp2647="="; struct _tagged_string _temp2648; _temp2648.curr= _temp2647;
_temp2648.base= _temp2647; _temp2648.last_plus_one= _temp2647 + 2; _temp2648;}),({
char* _temp2649="="; struct _tagged_string _temp2650; _temp2650.curr= _temp2649;
_temp2650.base= _temp2649; _temp2650.last_plus_one= _temp2649 + 2; _temp2650;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1)); struct Cyc_PP_Doc*
_temp2644= Cyc_Absynpp_pat2doc((* dp).f2); struct Cyc_PP_Doc* _temp2641[ 2u]={
_temp2643, _temp2644}; struct _tagged_ptr0 _temp2642={ _temp2641, _temp2641,
_temp2641 + 2u}; _temp2640( _temp2642);});} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
==( void*) Cyc_Absyn_Wild_p: 0){ return({ struct Cyc_PP_Doc*(* _temp2651)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2654= Cyc_PP_text(({
char* _temp2661="default: "; struct _tagged_string _temp2662; _temp2662.curr=
_temp2661; _temp2662.base= _temp2661; _temp2662.last_plus_one= _temp2661 + 10;
_temp2662;})); struct Cyc_PP_Doc* _temp2655= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2656)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2659= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2660= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp2657[ 2u]={ _temp2659, _temp2660}; struct _tagged_ptr0 _temp2658={
_temp2657, _temp2657, _temp2657 + 2u}; _temp2656( _temp2658);})); struct Cyc_PP_Doc*
_temp2652[ 2u]={ _temp2654, _temp2655}; struct _tagged_ptr0 _temp2653={
_temp2652, _temp2652, _temp2652 + 2u}; _temp2651( _temp2653);});} else{ if( c->where_clause
== 0){ return({ struct Cyc_PP_Doc*(* _temp2663)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2666= Cyc_PP_text(({ char* _temp2677="case "; struct
_tagged_string _temp2678; _temp2678.curr= _temp2677; _temp2678.base= _temp2677;
_temp2678.last_plus_one= _temp2677 + 6; _temp2678;})); struct Cyc_PP_Doc*
_temp2667= Cyc_Absynpp_pat2doc( c->pattern); struct Cyc_PP_Doc* _temp2668= Cyc_PP_text(({
char* _temp2675=": "; struct _tagged_string _temp2676; _temp2676.curr= _temp2675;
_temp2676.base= _temp2675; _temp2676.last_plus_one= _temp2675 + 3; _temp2676;}));
struct Cyc_PP_Doc* _temp2669= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2670)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2673= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2674= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp2671[ 2u]={ _temp2673, _temp2674}; struct _tagged_ptr0 _temp2672={
_temp2671, _temp2671, _temp2671 + 2u}; _temp2670( _temp2672);})); struct Cyc_PP_Doc*
_temp2664[ 4u]={ _temp2666, _temp2667, _temp2668, _temp2669}; struct
_tagged_ptr0 _temp2665={ _temp2664, _temp2664, _temp2664 + 4u}; _temp2663(
_temp2665);});} else{ return({ struct Cyc_PP_Doc*(* _temp2679)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2682= Cyc_PP_text(({ char*
_temp2697="case "; struct _tagged_string _temp2698; _temp2698.curr= _temp2697;
_temp2698.base= _temp2697; _temp2698.last_plus_one= _temp2697 + 6; _temp2698;}));
struct Cyc_PP_Doc* _temp2683= Cyc_Absynpp_pat2doc( c->pattern); struct Cyc_PP_Doc*
_temp2684= Cyc_PP_text(({ char* _temp2695=" && "; struct _tagged_string
_temp2696; _temp2696.curr= _temp2695; _temp2696.base= _temp2695; _temp2696.last_plus_one=
_temp2695 + 5; _temp2696;})); struct Cyc_PP_Doc* _temp2685= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( c->where_clause)); struct Cyc_PP_Doc*
_temp2686= Cyc_PP_text(({ char* _temp2693=": "; struct _tagged_string _temp2694;
_temp2694.curr= _temp2693; _temp2694.base= _temp2693; _temp2694.last_plus_one=
_temp2693 + 3; _temp2694;})); struct Cyc_PP_Doc* _temp2687= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2688)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2691= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2692= Cyc_Absynpp_stmt2doc(
c->body); struct Cyc_PP_Doc* _temp2689[ 2u]={ _temp2691, _temp2692}; struct
_tagged_ptr0 _temp2690={ _temp2689, _temp2689, _temp2689 + 2u}; _temp2688(
_temp2690);})); struct Cyc_PP_Doc* _temp2680[ 6u]={ _temp2682, _temp2683,
_temp2684, _temp2685, _temp2686, _temp2687}; struct _tagged_ptr0 _temp2681={
_temp2680, _temp2680, _temp2680 + 6u}; _temp2679( _temp2681);});}}} struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc( struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Switch_clause*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,({
char* _temp2699=""; struct _tagged_string _temp2700; _temp2700.curr= _temp2699;
_temp2700.base= _temp2699; _temp2700.last_plus_one= _temp2699 + 1; _temp2700;}),
cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield*
f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);} else{ return({
struct Cyc_PP_Doc*(* _temp2701)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2704= Cyc_Absynpp_qvar2doc( f->name); struct Cyc_PP_Doc* _temp2705= Cyc_PP_text(({
char* _temp2707=" = "; struct _tagged_string _temp2708; _temp2708.curr=
_temp2707; _temp2708.base= _temp2707; _temp2708.last_plus_one= _temp2707 + 4;
_temp2708;})); struct Cyc_PP_Doc* _temp2706= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( f->tag)); struct Cyc_PP_Doc* _temp2702[ 3u]={ _temp2704, _temp2705,
_temp2706}; struct _tagged_ptr0 _temp2703={ _temp2702, _temp2702, _temp2702 + 3u};
_temp2701( _temp2703);});}} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List* fs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Enumfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,({ char* _temp2709=","; struct
_tagged_string _temp2710; _temp2710.curr= _temp2709; _temp2710.base= _temp2709;
_temp2710.last_plus_one= _temp2709 + 2; _temp2710;}), fs);} static struct Cyc_PP_Doc*
Cyc_Absynpp_id2doc( struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);}
static struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){
return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,({
char* _temp2711=","; struct _tagged_string _temp2712; _temp2712.curr= _temp2711;
_temp2712.base= _temp2711; _temp2712.last_plus_one= _temp2711 + 2; _temp2712;}),
vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl* d){
struct Cyc_PP_Doc* s;{ void* _temp2713=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp2739; struct Cyc_Absyn_Structdecl* _temp2741; struct Cyc_Absyn_Uniondecl*
_temp2743; struct Cyc_Absyn_Vardecl* _temp2745; struct Cyc_Absyn_Vardecl
_temp2747; struct Cyc_List_List* _temp2748; struct Cyc_Core_Opt* _temp2750;
struct Cyc_Absyn_Exp* _temp2752; void* _temp2754; struct Cyc_Absyn_Tqual
_temp2756; struct _tuple0* _temp2758; void* _temp2760; struct Cyc_Absyn_Tuniondecl*
_temp2762; struct Cyc_Absyn_Tuniondecl _temp2764; int _temp2765; struct Cyc_Core_Opt*
_temp2767; struct Cyc_List_List* _temp2769; struct _tuple0* _temp2771; void*
_temp2773; int _temp2775; struct Cyc_Absyn_Exp* _temp2777; struct Cyc_Core_Opt*
_temp2779; struct Cyc_Core_Opt* _temp2781; struct Cyc_Absyn_Pat* _temp2783;
struct Cyc_List_List* _temp2785; struct Cyc_Absyn_Enumdecl* _temp2787; struct
Cyc_Absyn_Enumdecl _temp2789; struct Cyc_Core_Opt* _temp2790; struct _tuple0*
_temp2792; void* _temp2794; struct Cyc_Absyn_Typedefdecl* _temp2796; struct Cyc_List_List*
_temp2798; struct _tagged_string* _temp2800; struct Cyc_List_List* _temp2802;
struct _tuple0* _temp2804; struct Cyc_List_List* _temp2806; _LL2715: if(*(( int*)
_temp2713) == Cyc_Absyn_Fn_d){ _LL2740: _temp2739=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2713)->f1; goto _LL2716;} else{ goto _LL2717;} _LL2717: if(*(( int*)
_temp2713) == Cyc_Absyn_Struct_d){ _LL2742: _temp2741=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2713)->f1; goto _LL2718;} else{ goto _LL2719;} _LL2719: if(*(( int*)
_temp2713) == Cyc_Absyn_Union_d){ _LL2744: _temp2743=(( struct Cyc_Absyn_Union_d_struct*)
_temp2713)->f1; goto _LL2720;} else{ goto _LL2721;} _LL2721: if(*(( int*)
_temp2713) == Cyc_Absyn_Var_d){ _LL2746: _temp2745=(( struct Cyc_Absyn_Var_d_struct*)
_temp2713)->f1; _temp2747=* _temp2745; _LL2761: _temp2760=( void*) _temp2747.sc;
goto _LL2759; _LL2759: _temp2758= _temp2747.name; goto _LL2757; _LL2757:
_temp2756= _temp2747.tq; goto _LL2755; _LL2755: _temp2754=( void*) _temp2747.type;
goto _LL2753; _LL2753: _temp2752= _temp2747.initializer; goto _LL2751; _LL2751:
_temp2750= _temp2747.rgn; goto _LL2749; _LL2749: _temp2748= _temp2747.attributes;
goto _LL2722;} else{ goto _LL2723;} _LL2723: if(*(( int*) _temp2713) == Cyc_Absyn_Tunion_d){
_LL2763: _temp2762=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2713)->f1;
_temp2764=* _temp2762; _LL2774: _temp2773=( void*) _temp2764.sc; goto _LL2772;
_LL2772: _temp2771= _temp2764.name; goto _LL2770; _LL2770: _temp2769= _temp2764.tvs;
goto _LL2768; _LL2768: _temp2767= _temp2764.fields; goto _LL2766; _LL2766:
_temp2765= _temp2764.is_xtunion; goto _LL2724;} else{ goto _LL2725;} _LL2725:
if(*(( int*) _temp2713) == Cyc_Absyn_Let_d){ _LL2784: _temp2783=(( struct Cyc_Absyn_Let_d_struct*)
_temp2713)->f1; goto _LL2782; _LL2782: _temp2781=(( struct Cyc_Absyn_Let_d_struct*)
_temp2713)->f2; goto _LL2780; _LL2780: _temp2779=(( struct Cyc_Absyn_Let_d_struct*)
_temp2713)->f3; goto _LL2778; _LL2778: _temp2777=(( struct Cyc_Absyn_Let_d_struct*)
_temp2713)->f4; goto _LL2776; _LL2776: _temp2775=(( struct Cyc_Absyn_Let_d_struct*)
_temp2713)->f5; goto _LL2726;} else{ goto _LL2727;} _LL2727: if(*(( int*)
_temp2713) == Cyc_Absyn_Letv_d){ _LL2786: _temp2785=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2713)->f1; goto _LL2728;} else{ goto _LL2729;} _LL2729: if(*(( int*)
_temp2713) == Cyc_Absyn_Enum_d){ _LL2788: _temp2787=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2713)->f1; _temp2789=* _temp2787; _LL2795: _temp2794=( void*) _temp2789.sc;
goto _LL2793; _LL2793: _temp2792= _temp2789.name; goto _LL2791; _LL2791:
_temp2790= _temp2789.fields; goto _LL2730;} else{ goto _LL2731;} _LL2731: if(*((
int*) _temp2713) == Cyc_Absyn_Typedef_d){ _LL2797: _temp2796=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2713)->f1; goto _LL2732;} else{ goto _LL2733;} _LL2733: if(*(( int*)
_temp2713) == Cyc_Absyn_Namespace_d){ _LL2801: _temp2800=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2713)->f1; goto _LL2799; _LL2799: _temp2798=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2713)->f2; goto _LL2734;} else{ goto _LL2735;} _LL2735: if(*(( int*)
_temp2713) == Cyc_Absyn_Using_d){ _LL2805: _temp2804=(( struct Cyc_Absyn_Using_d_struct*)
_temp2713)->f1; goto _LL2803; _LL2803: _temp2802=(( struct Cyc_Absyn_Using_d_struct*)
_temp2713)->f2; goto _LL2736;} else{ goto _LL2737;} _LL2737: if(*(( int*)
_temp2713) == Cyc_Absyn_ExternC_d){ _LL2807: _temp2806=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2713)->f1; goto _LL2738;} else{ goto _LL2714;} _LL2716: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2840=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2840[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2841; _temp2841.tag= Cyc_Absyn_FnType; _temp2841.f1=({ struct Cyc_Absyn_FnInfo
_temp2842; _temp2842.tvars= _temp2739->tvs; _temp2842.effect= _temp2739->effect;
_temp2842.ret_typ=( void*)(( void*) _temp2739->ret_type); _temp2842.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2739->args); _temp2842.c_varargs=
_temp2739->c_varargs; _temp2842.cyc_varargs= _temp2739->cyc_varargs; _temp2842.attributes=
0; _temp2842;}); _temp2841;}); _temp2840;}); s=({ struct Cyc_PP_Doc*(* _temp2808)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2811= Cyc_PP_text(
_temp2739->is_inline? Cyc_Absynpp_to_VC?({ char* _temp2834="__inline "; struct
_tagged_string _temp2835; _temp2835.curr= _temp2834; _temp2835.base= _temp2834;
_temp2835.last_plus_one= _temp2834 + 10; _temp2835;}):({ char* _temp2836="inline ";
struct _tagged_string _temp2837; _temp2837.curr= _temp2836; _temp2837.base=
_temp2836; _temp2837.last_plus_one= _temp2836 + 8; _temp2837;}):({ char*
_temp2838=""; struct _tagged_string _temp2839; _temp2839.curr= _temp2838;
_temp2839.base= _temp2838; _temp2839.last_plus_one= _temp2838 + 1; _temp2839;}));
struct Cyc_PP_Doc* _temp2812= Cyc_Absynpp_scope2doc(( void*) _temp2739->sc);
struct Cyc_PP_Doc* _temp2813= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp2827; _temp2827.q_const= 0; _temp2827.q_volatile= 0; _temp2827.q_restrict=
0; _temp2827;}), t,({ struct Cyc_Core_Opt* _temp2828=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2828->v=( void*)({ struct Cyc_PP_Doc*(*
_temp2829)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2832= Cyc_Absynpp_atts2doc(
_temp2739->attributes); struct Cyc_PP_Doc* _temp2833= Cyc_Absynpp_typedef_name2doc(
_temp2739->name); struct Cyc_PP_Doc* _temp2830[ 2u]={ _temp2832, _temp2833};
struct _tagged_ptr0 _temp2831={ _temp2830, _temp2830, _temp2830 + 2u}; _temp2829(
_temp2831);}); _temp2828;})); struct Cyc_PP_Doc* _temp2814= Cyc_PP_text(({ char*
_temp2825=" {"; struct _tagged_string _temp2826; _temp2826.curr= _temp2825;
_temp2826.base= _temp2825; _temp2826.last_plus_one= _temp2825 + 3; _temp2826;}));
struct Cyc_PP_Doc* _temp2815= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2820)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2823= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2824= Cyc_Absynpp_stmt2doc( _temp2739->body); struct Cyc_PP_Doc*
_temp2821[ 2u]={ _temp2823, _temp2824}; struct _tagged_ptr0 _temp2822={
_temp2821, _temp2821, _temp2821 + 2u}; _temp2820( _temp2822);})); struct Cyc_PP_Doc*
_temp2816= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2817= Cyc_PP_text(({ char*
_temp2818="}"; struct _tagged_string _temp2819; _temp2819.curr= _temp2818;
_temp2819.base= _temp2818; _temp2819.last_plus_one= _temp2818 + 2; _temp2819;}));
struct Cyc_PP_Doc* _temp2809[ 7u]={ _temp2811, _temp2812, _temp2813, _temp2814,
_temp2815, _temp2816, _temp2817}; struct _tagged_ptr0 _temp2810={ _temp2809,
_temp2809, _temp2809 + 7u}; _temp2808( _temp2810);}); goto _LL2714;} _LL2718:
if( _temp2741->fields == 0){ s=({ struct Cyc_PP_Doc*(* _temp2843)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2846= Cyc_Absynpp_scope2doc((
void*) _temp2741->sc); struct Cyc_PP_Doc* _temp2847= Cyc_PP_text(({ char*
_temp2855="struct "; struct _tagged_string _temp2856; _temp2856.curr= _temp2855;
_temp2856.base= _temp2855; _temp2856.last_plus_one= _temp2855 + 8; _temp2856;}));
struct Cyc_PP_Doc* _temp2848= _temp2741->name == 0? Cyc_PP_text(({ char*
_temp2853=""; struct _tagged_string _temp2854; _temp2854.curr= _temp2853;
_temp2854.base= _temp2853; _temp2854.last_plus_one= _temp2853 + 1; _temp2854;})):
Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp2741->name))->v); struct Cyc_PP_Doc* _temp2849= Cyc_Absynpp_ktvars2doc(
_temp2741->tvs); struct Cyc_PP_Doc* _temp2850= Cyc_PP_text(({ char* _temp2851=";";
struct _tagged_string _temp2852; _temp2852.curr= _temp2851; _temp2852.base=
_temp2851; _temp2852.last_plus_one= _temp2851 + 2; _temp2852;})); struct Cyc_PP_Doc*
_temp2844[ 5u]={ _temp2846, _temp2847, _temp2848, _temp2849, _temp2850}; struct
_tagged_ptr0 _temp2845={ _temp2844, _temp2844, _temp2844 + 5u}; _temp2843(
_temp2845);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2857)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2860= Cyc_Absynpp_scope2doc(( void*)
_temp2741->sc); struct Cyc_PP_Doc* _temp2861= Cyc_PP_text(({ char* _temp2883="struct ";
struct _tagged_string _temp2884; _temp2884.curr= _temp2883; _temp2884.base=
_temp2883; _temp2884.last_plus_one= _temp2883 + 8; _temp2884;})); struct Cyc_PP_Doc*
_temp2862= _temp2741->name == 0? Cyc_PP_text(({ char* _temp2881=""; struct
_tagged_string _temp2882; _temp2882.curr= _temp2881; _temp2882.base= _temp2881;
_temp2882.last_plus_one= _temp2881 + 1; _temp2882;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2741->name))->v);
struct Cyc_PP_Doc* _temp2863= Cyc_Absynpp_ktvars2doc( _temp2741->tvs); struct
Cyc_PP_Doc* _temp2864= Cyc_PP_text(({ char* _temp2879=" {"; struct
_tagged_string _temp2880; _temp2880.curr= _temp2879; _temp2880.base= _temp2879;
_temp2880.last_plus_one= _temp2879 + 3; _temp2880;})); struct Cyc_PP_Doc*
_temp2865= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2874)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2877= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2878= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2741->fields))->v); struct Cyc_PP_Doc*
_temp2875[ 2u]={ _temp2877, _temp2878}; struct _tagged_ptr0 _temp2876={
_temp2875, _temp2875, _temp2875 + 2u}; _temp2874( _temp2876);})); struct Cyc_PP_Doc*
_temp2866= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2867= Cyc_PP_text(({ char*
_temp2872="}"; struct _tagged_string _temp2873; _temp2873.curr= _temp2872;
_temp2873.base= _temp2872; _temp2873.last_plus_one= _temp2872 + 2; _temp2873;}));
struct Cyc_PP_Doc* _temp2868= Cyc_Absynpp_atts2doc( _temp2741->attributes);
struct Cyc_PP_Doc* _temp2869= Cyc_PP_text(({ char* _temp2870=";"; struct
_tagged_string _temp2871; _temp2871.curr= _temp2870; _temp2871.base= _temp2870;
_temp2871.last_plus_one= _temp2870 + 2; _temp2871;})); struct Cyc_PP_Doc*
_temp2858[ 10u]={ _temp2860, _temp2861, _temp2862, _temp2863, _temp2864,
_temp2865, _temp2866, _temp2867, _temp2868, _temp2869}; struct _tagged_ptr0
_temp2859={ _temp2858, _temp2858, _temp2858 + 10u}; _temp2857( _temp2859);});}
goto _LL2714; _LL2720: if( _temp2743->fields == 0){ s=({ struct Cyc_PP_Doc*(*
_temp2885)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2888= Cyc_Absynpp_scope2doc((
void*) _temp2743->sc); struct Cyc_PP_Doc* _temp2889= Cyc_PP_text(({ char*
_temp2897="union "; struct _tagged_string _temp2898; _temp2898.curr= _temp2897;
_temp2898.base= _temp2897; _temp2898.last_plus_one= _temp2897 + 7; _temp2898;}));
struct Cyc_PP_Doc* _temp2890= _temp2743->name == 0? Cyc_PP_text(({ char*
_temp2895=""; struct _tagged_string _temp2896; _temp2896.curr= _temp2895;
_temp2896.base= _temp2895; _temp2896.last_plus_one= _temp2895 + 1; _temp2896;})):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp2743->name))->v); struct Cyc_PP_Doc* _temp2891= Cyc_Absynpp_tvars2doc(
_temp2743->tvs); struct Cyc_PP_Doc* _temp2892= Cyc_PP_text(({ char* _temp2893=";";
struct _tagged_string _temp2894; _temp2894.curr= _temp2893; _temp2894.base=
_temp2893; _temp2894.last_plus_one= _temp2893 + 2; _temp2894;})); struct Cyc_PP_Doc*
_temp2886[ 5u]={ _temp2888, _temp2889, _temp2890, _temp2891, _temp2892}; struct
_tagged_ptr0 _temp2887={ _temp2886, _temp2886, _temp2886 + 5u}; _temp2885(
_temp2887);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2899)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2902= Cyc_Absynpp_scope2doc(( void*)
_temp2743->sc); struct Cyc_PP_Doc* _temp2903= Cyc_PP_text(({ char* _temp2925="union ";
struct _tagged_string _temp2926; _temp2926.curr= _temp2925; _temp2926.base=
_temp2925; _temp2926.last_plus_one= _temp2925 + 7; _temp2926;})); struct Cyc_PP_Doc*
_temp2904= _temp2743->name == 0? Cyc_PP_text(({ char* _temp2923=""; struct
_tagged_string _temp2924; _temp2924.curr= _temp2923; _temp2924.base= _temp2923;
_temp2924.last_plus_one= _temp2923 + 1; _temp2924;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2743->name))->v);
struct Cyc_PP_Doc* _temp2905= Cyc_Absynpp_tvars2doc( _temp2743->tvs); struct Cyc_PP_Doc*
_temp2906= Cyc_PP_text(({ char* _temp2921=" {"; struct _tagged_string _temp2922;
_temp2922.curr= _temp2921; _temp2922.base= _temp2921; _temp2922.last_plus_one=
_temp2921 + 3; _temp2922;})); struct Cyc_PP_Doc* _temp2907= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2916)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2919= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2920= Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2743->fields))->v);
struct Cyc_PP_Doc* _temp2917[ 2u]={ _temp2919, _temp2920}; struct _tagged_ptr0
_temp2918={ _temp2917, _temp2917, _temp2917 + 2u}; _temp2916( _temp2918);}));
struct Cyc_PP_Doc* _temp2908= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2909=
Cyc_PP_text(({ char* _temp2914="}"; struct _tagged_string _temp2915; _temp2915.curr=
_temp2914; _temp2915.base= _temp2914; _temp2915.last_plus_one= _temp2914 + 2;
_temp2915;})); struct Cyc_PP_Doc* _temp2910= Cyc_Absynpp_atts2doc( _temp2743->attributes);
struct Cyc_PP_Doc* _temp2911= Cyc_PP_text(({ char* _temp2912=";"; struct
_tagged_string _temp2913; _temp2913.curr= _temp2912; _temp2913.base= _temp2912;
_temp2913.last_plus_one= _temp2912 + 2; _temp2913;})); struct Cyc_PP_Doc*
_temp2900[ 10u]={ _temp2902, _temp2903, _temp2904, _temp2905, _temp2906,
_temp2907, _temp2908, _temp2909, _temp2910, _temp2911}; struct _tagged_ptr0
_temp2901={ _temp2900, _temp2900, _temp2900 + 10u}; _temp2899( _temp2901);});}
goto _LL2714; _LL2722: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc(
_temp2758); s=({ struct Cyc_PP_Doc*(* _temp2927)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2930= Cyc_Absynpp_scope2doc( _temp2760); struct Cyc_PP_Doc*
_temp2931= Cyc_Absynpp_tqtd2doc( _temp2756, _temp2754,({ struct Cyc_Core_Opt*
_temp2946=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2946->v=( void*) sn; _temp2946;})); struct Cyc_PP_Doc* _temp2932= Cyc_Absynpp_atts2doc(
_temp2748); struct Cyc_PP_Doc* _temp2933= _temp2752 == 0? Cyc_PP_text(({ char*
_temp2937=""; struct _tagged_string _temp2938; _temp2938.curr= _temp2937;
_temp2938.base= _temp2937; _temp2938.last_plus_one= _temp2937 + 1; _temp2938;})):({
struct Cyc_PP_Doc*(* _temp2939)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2942= Cyc_PP_text(({ char* _temp2944=" = "; struct _tagged_string _temp2945;
_temp2945.curr= _temp2944; _temp2945.base= _temp2944; _temp2945.last_plus_one=
_temp2944 + 4; _temp2945;})); struct Cyc_PP_Doc* _temp2943= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp2752)); struct Cyc_PP_Doc* _temp2940[ 2u]={
_temp2942, _temp2943}; struct _tagged_ptr0 _temp2941={ _temp2940, _temp2940,
_temp2940 + 2u}; _temp2939( _temp2941);}); struct Cyc_PP_Doc* _temp2934= Cyc_PP_text(({
char* _temp2935=";"; struct _tagged_string _temp2936; _temp2936.curr= _temp2935;
_temp2936.base= _temp2935; _temp2936.last_plus_one= _temp2935 + 2; _temp2936;}));
struct Cyc_PP_Doc* _temp2928[ 5u]={ _temp2930, _temp2931, _temp2932, _temp2933,
_temp2934}; struct _tagged_ptr0 _temp2929={ _temp2928, _temp2928, _temp2928 + 5u};
_temp2927( _temp2929);}); goto _LL2714;} _LL2724: if( _temp2767 == 0){ s=({
struct Cyc_PP_Doc*(* _temp2947)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2950= Cyc_Absynpp_scope2doc( _temp2773); struct Cyc_PP_Doc* _temp2951=
_temp2765? Cyc_PP_text(({ char* _temp2957="xtunion "; struct _tagged_string
_temp2958; _temp2958.curr= _temp2957; _temp2958.base= _temp2957; _temp2958.last_plus_one=
_temp2957 + 9; _temp2958;})): Cyc_PP_text(({ char* _temp2959="tunion "; struct
_tagged_string _temp2960; _temp2960.curr= _temp2959; _temp2960.base= _temp2959;
_temp2960.last_plus_one= _temp2959 + 8; _temp2960;})); struct Cyc_PP_Doc*
_temp2952= _temp2765? Cyc_Absynpp_qvar2doc( _temp2771): Cyc_Absynpp_typedef_name2doc(
_temp2771); struct Cyc_PP_Doc* _temp2953= Cyc_Absynpp_ktvars2doc( _temp2769);
struct Cyc_PP_Doc* _temp2954= Cyc_PP_text(({ char* _temp2955=";"; struct
_tagged_string _temp2956; _temp2956.curr= _temp2955; _temp2956.base= _temp2955;
_temp2956.last_plus_one= _temp2955 + 2; _temp2956;})); struct Cyc_PP_Doc*
_temp2948[ 5u]={ _temp2950, _temp2951, _temp2952, _temp2953, _temp2954}; struct
_tagged_ptr0 _temp2949={ _temp2948, _temp2948, _temp2948 + 5u}; _temp2947(
_temp2949);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2961)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2964= Cyc_Absynpp_scope2doc( _temp2773);
struct Cyc_PP_Doc* _temp2965= _temp2765? Cyc_PP_text(({ char* _temp2981="xtunion ";
struct _tagged_string _temp2982; _temp2982.curr= _temp2981; _temp2982.base=
_temp2981; _temp2982.last_plus_one= _temp2981 + 9; _temp2982;})): Cyc_PP_text(({
char* _temp2983="tunion "; struct _tagged_string _temp2984; _temp2984.curr=
_temp2983; _temp2984.base= _temp2983; _temp2984.last_plus_one= _temp2983 + 8;
_temp2984;})); struct Cyc_PP_Doc* _temp2966= _temp2765? Cyc_Absynpp_qvar2doc(
_temp2771): Cyc_Absynpp_typedef_name2doc( _temp2771); struct Cyc_PP_Doc*
_temp2967= Cyc_Absynpp_ktvars2doc( _temp2769); struct Cyc_PP_Doc* _temp2968= Cyc_PP_text(({
char* _temp2979=" {"; struct _tagged_string _temp2980; _temp2980.curr= _temp2979;
_temp2980.base= _temp2979; _temp2980.last_plus_one= _temp2979 + 3; _temp2980;}));
struct Cyc_PP_Doc* _temp2969= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2974)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2977= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2978= Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2767))->v); struct Cyc_PP_Doc* _temp2975[
2u]={ _temp2977, _temp2978}; struct _tagged_ptr0 _temp2976={ _temp2975,
_temp2975, _temp2975 + 2u}; _temp2974( _temp2976);})); struct Cyc_PP_Doc*
_temp2970= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2971= Cyc_PP_text(({ char*
_temp2972="};"; struct _tagged_string _temp2973; _temp2973.curr= _temp2972;
_temp2973.base= _temp2972; _temp2973.last_plus_one= _temp2972 + 3; _temp2973;}));
struct Cyc_PP_Doc* _temp2962[ 8u]={ _temp2964, _temp2965, _temp2966, _temp2967,
_temp2968, _temp2969, _temp2970, _temp2971}; struct _tagged_ptr0 _temp2963={
_temp2962, _temp2962, _temp2962 + 8u}; _temp2961( _temp2963);});} goto _LL2714;
_LL2726: s=({ struct Cyc_PP_Doc*(* _temp2985)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2988= Cyc_PP_text(({ char* _temp2997="let "; struct
_tagged_string _temp2998; _temp2998.curr= _temp2997; _temp2998.base= _temp2997;
_temp2998.last_plus_one= _temp2997 + 5; _temp2998;})); struct Cyc_PP_Doc*
_temp2989= Cyc_Absynpp_pat2doc( _temp2783); struct Cyc_PP_Doc* _temp2990= Cyc_PP_text(({
char* _temp2995=" = "; struct _tagged_string _temp2996; _temp2996.curr=
_temp2995; _temp2996.base= _temp2995; _temp2996.last_plus_one= _temp2995 + 4;
_temp2996;})); struct Cyc_PP_Doc* _temp2991= Cyc_Absynpp_exp2doc( _temp2777);
struct Cyc_PP_Doc* _temp2992= Cyc_PP_text(({ char* _temp2993=";"; struct
_tagged_string _temp2994; _temp2994.curr= _temp2993; _temp2994.base= _temp2993;
_temp2994.last_plus_one= _temp2993 + 2; _temp2994;})); struct Cyc_PP_Doc*
_temp2986[ 5u]={ _temp2988, _temp2989, _temp2990, _temp2991, _temp2992}; struct
_tagged_ptr0 _temp2987={ _temp2986, _temp2986, _temp2986 + 5u}; _temp2985(
_temp2987);}); goto _LL2714; _LL2728: s=({ struct Cyc_PP_Doc*(* _temp2999)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3002= Cyc_PP_text(({
char* _temp3007="let "; struct _tagged_string _temp3008; _temp3008.curr=
_temp3007; _temp3008.base= _temp3007; _temp3008.last_plus_one= _temp3007 + 5;
_temp3008;})); struct Cyc_PP_Doc* _temp3003= Cyc_Absynpp_ids2doc( _temp2785);
struct Cyc_PP_Doc* _temp3004= Cyc_PP_text(({ char* _temp3005=";"; struct
_tagged_string _temp3006; _temp3006.curr= _temp3005; _temp3006.base= _temp3005;
_temp3006.last_plus_one= _temp3005 + 2; _temp3006;})); struct Cyc_PP_Doc*
_temp3000[ 3u]={ _temp3002, _temp3003, _temp3004}; struct _tagged_ptr0 _temp3001={
_temp3000, _temp3000, _temp3000 + 3u}; _temp2999( _temp3001);}); goto _LL2714;
_LL2730: if( _temp2790 == 0){ s=({ struct Cyc_PP_Doc*(* _temp3009)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3012= Cyc_Absynpp_scope2doc(
_temp2794); struct Cyc_PP_Doc* _temp3013= Cyc_PP_text(({ char* _temp3018="enum ";
struct _tagged_string _temp3019; _temp3019.curr= _temp3018; _temp3019.base=
_temp3018; _temp3019.last_plus_one= _temp3018 + 6; _temp3019;})); struct Cyc_PP_Doc*
_temp3014= Cyc_Absynpp_typedef_name2doc( _temp2792); struct Cyc_PP_Doc*
_temp3015= Cyc_PP_text(({ char* _temp3016=";"; struct _tagged_string _temp3017;
_temp3017.curr= _temp3016; _temp3017.base= _temp3016; _temp3017.last_plus_one=
_temp3016 + 2; _temp3017;})); struct Cyc_PP_Doc* _temp3010[ 4u]={ _temp3012,
_temp3013, _temp3014, _temp3015}; struct _tagged_ptr0 _temp3011={ _temp3010,
_temp3010, _temp3010 + 4u}; _temp3009( _temp3011);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp3020)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3023= Cyc_Absynpp_scope2doc(
_temp2794); struct Cyc_PP_Doc* _temp3024= Cyc_PP_text(({ char* _temp3039="enum ";
struct _tagged_string _temp3040; _temp3040.curr= _temp3039; _temp3040.base=
_temp3039; _temp3040.last_plus_one= _temp3039 + 6; _temp3040;})); struct Cyc_PP_Doc*
_temp3025= Cyc_Absynpp_qvar2doc( _temp2792); struct Cyc_PP_Doc* _temp3026= Cyc_PP_text(({
char* _temp3037=" {"; struct _tagged_string _temp3038; _temp3038.curr= _temp3037;
_temp3038.base= _temp3037; _temp3038.last_plus_one= _temp3037 + 3; _temp3038;}));
struct Cyc_PP_Doc* _temp3027= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp3032)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3035= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3036= Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2790))->v); struct Cyc_PP_Doc* _temp3033[
2u]={ _temp3035, _temp3036}; struct _tagged_ptr0 _temp3034={ _temp3033,
_temp3033, _temp3033 + 2u}; _temp3032( _temp3034);})); struct Cyc_PP_Doc*
_temp3028= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3029= Cyc_PP_text(({ char*
_temp3030="};"; struct _tagged_string _temp3031; _temp3031.curr= _temp3030;
_temp3031.base= _temp3030; _temp3031.last_plus_one= _temp3030 + 3; _temp3031;}));
struct Cyc_PP_Doc* _temp3021[ 7u]={ _temp3023, _temp3024, _temp3025, _temp3026,
_temp3027, _temp3028, _temp3029}; struct _tagged_ptr0 _temp3022={ _temp3021,
_temp3021, _temp3021 + 7u}; _temp3020( _temp3022);});} goto _LL2714; _LL2732: s=({
struct Cyc_PP_Doc*(* _temp3041)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3044= Cyc_PP_text(({ char* _temp3056="typedef "; struct _tagged_string
_temp3057; _temp3057.curr= _temp3056; _temp3057.base= _temp3056; _temp3057.last_plus_one=
_temp3056 + 9; _temp3057;})); struct Cyc_PP_Doc* _temp3045= Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual _temp3049; _temp3049.q_const= 0; _temp3049.q_volatile= 0;
_temp3049.q_restrict= 0; _temp3049;}),( void*) _temp2796->defn,({ struct Cyc_Core_Opt*
_temp3050=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3050->v=( void*)({ struct Cyc_PP_Doc*(* _temp3051)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3054= Cyc_Absynpp_typedef_name2doc(
_temp2796->name); struct Cyc_PP_Doc* _temp3055= Cyc_Absynpp_tvars2doc( _temp2796->tvs);
struct Cyc_PP_Doc* _temp3052[ 2u]={ _temp3054, _temp3055}; struct _tagged_ptr0
_temp3053={ _temp3052, _temp3052, _temp3052 + 2u}; _temp3051( _temp3053);});
_temp3050;})); struct Cyc_PP_Doc* _temp3046= Cyc_PP_text(({ char* _temp3047=";";
struct _tagged_string _temp3048; _temp3048.curr= _temp3047; _temp3048.base=
_temp3047; _temp3048.last_plus_one= _temp3047 + 2; _temp3048;})); struct Cyc_PP_Doc*
_temp3042[ 3u]={ _temp3044, _temp3045, _temp3046}; struct _tagged_ptr0 _temp3043={
_temp3042, _temp3042, _temp3042 + 3u}; _temp3041( _temp3043);}); goto _LL2714;
_LL2734: if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_add(
_temp2800);} s=({ struct Cyc_PP_Doc*(* _temp3058)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3061= Cyc_PP_text(({ char* _temp3074="namespace ";
struct _tagged_string _temp3075; _temp3075.curr= _temp3074; _temp3075.base=
_temp3074; _temp3075.last_plus_one= _temp3074 + 11; _temp3075;})); struct Cyc_PP_Doc*
_temp3062= Cyc_PP_textptr( _temp2800); struct Cyc_PP_Doc* _temp3063= Cyc_PP_text(({
char* _temp3072=" {"; struct _tagged_string _temp3073; _temp3073.curr= _temp3072;
_temp3073.base= _temp3072; _temp3073.last_plus_one= _temp3072 + 3; _temp3073;}));
struct Cyc_PP_Doc* _temp3064= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3065=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({
char* _temp3070=""; struct _tagged_string _temp3071; _temp3071.curr= _temp3070;
_temp3071.base= _temp3070; _temp3071.last_plus_one= _temp3070 + 1; _temp3071;}),
_temp2798); struct Cyc_PP_Doc* _temp3066= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3067= Cyc_PP_text(({ char* _temp3068="}"; struct _tagged_string _temp3069;
_temp3069.curr= _temp3068; _temp3069.base= _temp3068; _temp3069.last_plus_one=
_temp3068 + 2; _temp3069;})); struct Cyc_PP_Doc* _temp3059[ 7u]={ _temp3061,
_temp3062, _temp3063, _temp3064, _temp3065, _temp3066, _temp3067}; struct
_tagged_ptr0 _temp3060={ _temp3059, _temp3059, _temp3059 + 7u}; _temp3058(
_temp3060);}); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2714; _LL2736: if( Cyc_Absynpp_print_using_stmts){ s=({ struct Cyc_PP_Doc*(*
_temp3076)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3079= Cyc_PP_text(({
char* _temp3092="using "; struct _tagged_string _temp3093; _temp3093.curr=
_temp3092; _temp3093.base= _temp3092; _temp3093.last_plus_one= _temp3092 + 7;
_temp3093;})); struct Cyc_PP_Doc* _temp3080= Cyc_Absynpp_qvar2doc( _temp2804);
struct Cyc_PP_Doc* _temp3081= Cyc_PP_text(({ char* _temp3090=" {"; struct
_tagged_string _temp3091; _temp3091.curr= _temp3090; _temp3091.base= _temp3090;
_temp3091.last_plus_one= _temp3090 + 3; _temp3091;})); struct Cyc_PP_Doc*
_temp3082= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3083=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ char*
_temp3088=""; struct _tagged_string _temp3089; _temp3089.curr= _temp3088;
_temp3089.base= _temp3088; _temp3089.last_plus_one= _temp3088 + 1; _temp3089;}),
_temp2802); struct Cyc_PP_Doc* _temp3084= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3085= Cyc_PP_text(({ char* _temp3086="}"; struct _tagged_string _temp3087;
_temp3087.curr= _temp3086; _temp3087.base= _temp3086; _temp3087.last_plus_one=
_temp3086 + 2; _temp3087;})); struct Cyc_PP_Doc* _temp3077[ 7u]={ _temp3079,
_temp3080, _temp3081, _temp3082, _temp3083, _temp3084, _temp3085}; struct
_tagged_ptr0 _temp3078={ _temp3077, _temp3077, _temp3077 + 7u}; _temp3076(
_temp3078);});} else{ s=({ struct Cyc_PP_Doc*(* _temp3094)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3097= Cyc_PP_text(({ char* _temp3110="/* using ";
struct _tagged_string _temp3111; _temp3111.curr= _temp3110; _temp3111.base=
_temp3110; _temp3111.last_plus_one= _temp3110 + 10; _temp3111;})); struct Cyc_PP_Doc*
_temp3098= Cyc_Absynpp_qvar2doc( _temp2804); struct Cyc_PP_Doc* _temp3099= Cyc_PP_text(({
char* _temp3108=" { */"; struct _tagged_string _temp3109; _temp3109.curr=
_temp3108; _temp3109.base= _temp3108; _temp3109.last_plus_one= _temp3108 + 6;
_temp3109;})); struct Cyc_PP_Doc* _temp3100= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3101=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({
char* _temp3106=""; struct _tagged_string _temp3107; _temp3107.curr= _temp3106;
_temp3107.base= _temp3106; _temp3107.last_plus_one= _temp3106 + 1; _temp3107;}),
_temp2802); struct Cyc_PP_Doc* _temp3102= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3103= Cyc_PP_text(({ char* _temp3104="/* } */"; struct _tagged_string
_temp3105; _temp3105.curr= _temp3104; _temp3105.base= _temp3104; _temp3105.last_plus_one=
_temp3104 + 8; _temp3105;})); struct Cyc_PP_Doc* _temp3095[ 7u]={ _temp3097,
_temp3098, _temp3099, _temp3100, _temp3101, _temp3102, _temp3103}; struct
_tagged_ptr0 _temp3096={ _temp3095, _temp3095, _temp3095 + 7u}; _temp3094(
_temp3096);});} goto _LL2714; _LL2738: if( Cyc_Absynpp_print_externC_stmts){ s=({
struct Cyc_PP_Doc*(* _temp3112)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3115= Cyc_PP_text(({ char* _temp3124="extern \"C\" {"; struct
_tagged_string _temp3125; _temp3125.curr= _temp3124; _temp3125.base= _temp3124;
_temp3125.last_plus_one= _temp3124 + 13; _temp3125;})); struct Cyc_PP_Doc*
_temp3116= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3117=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ char*
_temp3122=""; struct _tagged_string _temp3123; _temp3123.curr= _temp3122;
_temp3123.base= _temp3122; _temp3123.last_plus_one= _temp3122 + 1; _temp3123;}),
_temp2806); struct Cyc_PP_Doc* _temp3118= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3119= Cyc_PP_text(({ char* _temp3120="}"; struct _tagged_string _temp3121;
_temp3121.curr= _temp3120; _temp3121.base= _temp3120; _temp3121.last_plus_one=
_temp3120 + 2; _temp3121;})); struct Cyc_PP_Doc* _temp3113[ 5u]={ _temp3115,
_temp3116, _temp3117, _temp3118, _temp3119}; struct _tagged_ptr0 _temp3114={
_temp3113, _temp3113, _temp3113 + 5u}; _temp3112( _temp3114);});} else{ s=({
struct Cyc_PP_Doc*(* _temp3126)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3129= Cyc_PP_text(({ char* _temp3138="/* extern \"C\" { */"; struct
_tagged_string _temp3139; _temp3139.curr= _temp3138; _temp3139.base= _temp3138;
_temp3139.last_plus_one= _temp3138 + 19; _temp3139;})); struct Cyc_PP_Doc*
_temp3130= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3131=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ char*
_temp3136=""; struct _tagged_string _temp3137; _temp3137.curr= _temp3136;
_temp3137.base= _temp3136; _temp3137.last_plus_one= _temp3136 + 1; _temp3137;}),
_temp2806); struct Cyc_PP_Doc* _temp3132= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3133= Cyc_PP_text(({ char* _temp3134="/* } */"; struct _tagged_string
_temp3135; _temp3135.curr= _temp3134; _temp3135.base= _temp3134; _temp3135.last_plus_one=
_temp3134 + 8; _temp3135;})); struct Cyc_PP_Doc* _temp3127[ 5u]={ _temp3129,
_temp3130, _temp3131, _temp3132, _temp3133}; struct _tagged_ptr0 _temp3128={
_temp3127, _temp3127, _temp3127 + 5u}; _temp3126( _temp3128);});} goto _LL2714;
_LL2714:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void* sc){ void*
_temp3140= sc; _LL3142: if( _temp3140 ==( void*) Cyc_Absyn_Static){ goto _LL3143;}
else{ goto _LL3144;} _LL3144: if( _temp3140 ==( void*) Cyc_Absyn_Public){ goto
_LL3145;} else{ goto _LL3146;} _LL3146: if( _temp3140 ==( void*) Cyc_Absyn_Extern){
goto _LL3147;} else{ goto _LL3148;} _LL3148: if( _temp3140 ==( void*) Cyc_Absyn_ExternC){
goto _LL3149;} else{ goto _LL3150;} _LL3150: if( _temp3140 ==( void*) Cyc_Absyn_Abstract){
goto _LL3151;} else{ goto _LL3141;} _LL3143: return Cyc_PP_text(({ char*
_temp3152="static "; struct _tagged_string _temp3153; _temp3153.curr= _temp3152;
_temp3153.base= _temp3152; _temp3153.last_plus_one= _temp3152 + 8; _temp3153;}));
_LL3145: return Cyc_PP_nil_doc(); _LL3147: return Cyc_PP_text(({ char* _temp3154="extern ";
struct _tagged_string _temp3155; _temp3155.curr= _temp3154; _temp3155.base=
_temp3154; _temp3155.last_plus_one= _temp3154 + 8; _temp3155;})); _LL3149:
return Cyc_PP_text(({ char* _temp3156="extern \"C\" "; struct _tagged_string
_temp3157; _temp3157.curr= _temp3156; _temp3157.base= _temp3156; _temp3157.last_plus_one=
_temp3156 + 12; _temp3157;})); _LL3151: return Cyc_PP_text(({ char* _temp3158="abstract ";
struct _tagged_string _temp3159; _temp3159.curr= _temp3158; _temp3159.base=
_temp3158; _temp3159.last_plus_one= _temp3158 + 10; _temp3159;})); _LL3141:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp3160= t; struct
Cyc_Absyn_Tvar* _temp3168; struct Cyc_List_List* _temp3170; _LL3162: if((
unsigned int) _temp3160 > 4u?*(( int*) _temp3160) == Cyc_Absyn_VarType: 0){
_LL3169: _temp3168=(( struct Cyc_Absyn_VarType_struct*) _temp3160)->f1; goto
_LL3163;} else{ goto _LL3164;} _LL3164: if(( unsigned int) _temp3160 > 4u?*((
int*) _temp3160) == Cyc_Absyn_JoinEff: 0){ _LL3171: _temp3170=(( struct Cyc_Absyn_JoinEff_struct*)
_temp3160)->f1; goto _LL3165;} else{ goto _LL3166;} _LL3166: goto _LL3167;
_LL3163: return Cyc_Tcutil_is_temp_tvar( _temp3168); _LL3165: return(( int(*)(
int(* pred)( void*), struct Cyc_List_List* x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect,
_temp3170); _LL3167: return 0; _LL3161:;} struct _tuple4 Cyc_Absynpp_to_tms(
struct Cyc_Absyn_Tqual tq, void* t){ void* _temp3172= t; struct Cyc_Absyn_Exp*
_temp3186; struct Cyc_Absyn_Tqual _temp3188; void* _temp3190; struct Cyc_Absyn_PtrInfo
_temp3192; struct Cyc_Absyn_Conref* _temp3194; struct Cyc_Absyn_Tqual _temp3196;
struct Cyc_Absyn_Conref* _temp3198; void* _temp3200; void* _temp3202; struct Cyc_Absyn_FnInfo
_temp3204; struct Cyc_List_List* _temp3206; struct Cyc_Absyn_VarargInfo*
_temp3208; int _temp3210; struct Cyc_List_List* _temp3212; void* _temp3214;
struct Cyc_Core_Opt* _temp3216; struct Cyc_List_List* _temp3218; int _temp3220;
struct Cyc_Core_Opt* _temp3222; void* _temp3224; struct Cyc_Core_Opt* _temp3226;
struct Cyc_List_List* _temp3228; struct _tuple0* _temp3230; _LL3174: if((
unsigned int) _temp3172 > 4u?*(( int*) _temp3172) == Cyc_Absyn_ArrayType: 0){
_LL3191: _temp3190=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3172)->f1;
goto _LL3189; _LL3189: _temp3188=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3172)->f2; goto _LL3187; _LL3187: _temp3186=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3172)->f3; goto _LL3175;} else{ goto _LL3176;} _LL3176: if(( unsigned int)
_temp3172 > 4u?*(( int*) _temp3172) == Cyc_Absyn_PointerType: 0){ _LL3193:
_temp3192=(( struct Cyc_Absyn_PointerType_struct*) _temp3172)->f1; _LL3203:
_temp3202=( void*) _temp3192.elt_typ; goto _LL3201; _LL3201: _temp3200=( void*)
_temp3192.rgn_typ; goto _LL3199; _LL3199: _temp3198= _temp3192.nullable; goto
_LL3197; _LL3197: _temp3196= _temp3192.tq; goto _LL3195; _LL3195: _temp3194=
_temp3192.bounds; goto _LL3177;} else{ goto _LL3178;} _LL3178: if(( unsigned int)
_temp3172 > 4u?*(( int*) _temp3172) == Cyc_Absyn_FnType: 0){ _LL3205: _temp3204=((
struct Cyc_Absyn_FnType_struct*) _temp3172)->f1; _LL3219: _temp3218= _temp3204.tvars;
goto _LL3217; _LL3217: _temp3216= _temp3204.effect; goto _LL3215; _LL3215:
_temp3214=( void*) _temp3204.ret_typ; goto _LL3213; _LL3213: _temp3212=
_temp3204.args; goto _LL3211; _LL3211: _temp3210= _temp3204.c_varargs; goto
_LL3209; _LL3209: _temp3208= _temp3204.cyc_varargs; goto _LL3207; _LL3207:
_temp3206= _temp3204.attributes; goto _LL3179;} else{ goto _LL3180;} _LL3180:
if(( unsigned int) _temp3172 > 4u?*(( int*) _temp3172) == Cyc_Absyn_Evar: 0){
_LL3225: _temp3224=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp3172)->f1;
goto _LL3223; _LL3223: _temp3222=(( struct Cyc_Absyn_Evar_struct*) _temp3172)->f2;
goto _LL3221; _LL3221: _temp3220=(( struct Cyc_Absyn_Evar_struct*) _temp3172)->f3;
goto _LL3181;} else{ goto _LL3182;} _LL3182: if(( unsigned int) _temp3172 > 4u?*((
int*) _temp3172) == Cyc_Absyn_TypedefType: 0){ _LL3231: _temp3230=(( struct Cyc_Absyn_TypedefType_struct*)
_temp3172)->f1; goto _LL3229; _LL3229: _temp3228=(( struct Cyc_Absyn_TypedefType_struct*)
_temp3172)->f2; goto _LL3227; _LL3227: _temp3226=(( struct Cyc_Absyn_TypedefType_struct*)
_temp3172)->f3; goto _LL3183;} else{ goto _LL3184;} _LL3184: goto _LL3185;
_LL3175: { struct Cyc_List_List* _temp3234; void* _temp3236; struct Cyc_Absyn_Tqual
_temp3238; struct _tuple4 _temp3232= Cyc_Absynpp_to_tms( _temp3188, _temp3190);
_LL3239: _temp3238= _temp3232.f1; goto _LL3237; _LL3237: _temp3236= _temp3232.f2;
goto _LL3235; _LL3235: _temp3234= _temp3232.f3; goto _LL3233; _LL3233: { void*
tm; if( _temp3186 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3240=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3240[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3241; _temp3241.tag= Cyc_Absyn_ConstArray_mod;
_temp3241.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp3186); _temp3241;});
_temp3240;});} return({ struct _tuple4 _temp3242; _temp3242.f1= _temp3238;
_temp3242.f2= _temp3236; _temp3242.f3=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3243=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3243->hd=( void*) tm; _temp3243->tl= _temp3234; _temp3243;}); _temp3242;});}}
_LL3177: { struct Cyc_List_List* _temp3246; void* _temp3248; struct Cyc_Absyn_Tqual
_temp3250; struct _tuple4 _temp3244= Cyc_Absynpp_to_tms( _temp3196, _temp3202);
_LL3251: _temp3250= _temp3244.f1; goto _LL3249; _LL3249: _temp3248= _temp3244.f2;
goto _LL3247; _LL3247: _temp3246= _temp3244.f3; goto _LL3245; _LL3245: { void*
ps;{ struct _tuple8 _temp3253=({ struct _tuple8 _temp3252; _temp3252.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp3198))->v; _temp3252.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp3194))->v; _temp3252;}); void* _temp3263;
void* _temp3265; struct Cyc_Absyn_Exp* _temp3267; void* _temp3269; int _temp3271;
void* _temp3273; void* _temp3275; struct Cyc_Absyn_Exp* _temp3277; void*
_temp3279; int _temp3281; void* _temp3283; void* _temp3285; void* _temp3287;
_LL3255: _LL3270: _temp3269= _temp3253.f1; if(( unsigned int) _temp3269 > 1u?*((
int*) _temp3269) == Cyc_Absyn_Eq_constr: 0){ _LL3272: _temp3271=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp3269)->f1; if( _temp3271 == 1){ goto _LL3264;}
else{ goto _LL3257;}} else{ goto _LL3257;} _LL3264: _temp3263= _temp3253.f2; if((
unsigned int) _temp3263 > 1u?*(( int*) _temp3263) == Cyc_Absyn_Eq_constr: 0){
_LL3266: _temp3265=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3263)->f1;
if(( unsigned int) _temp3265 > 1u?*(( int*) _temp3265) == Cyc_Absyn_Upper_b: 0){
_LL3268: _temp3267=(( struct Cyc_Absyn_Upper_b_struct*) _temp3265)->f1; goto
_LL3256;} else{ goto _LL3257;}} else{ goto _LL3257;} _LL3257: _LL3280: _temp3279=
_temp3253.f1; if(( unsigned int) _temp3279 > 1u?*(( int*) _temp3279) == Cyc_Absyn_Eq_constr:
0){ _LL3282: _temp3281=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3279)->f1;
if( _temp3281 == 0){ goto _LL3274;} else{ goto _LL3259;}} else{ goto _LL3259;}
_LL3274: _temp3273= _temp3253.f2; if(( unsigned int) _temp3273 > 1u?*(( int*)
_temp3273) == Cyc_Absyn_Eq_constr: 0){ _LL3276: _temp3275=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3273)->f1; if(( unsigned int) _temp3275 > 1u?*(( int*) _temp3275) == Cyc_Absyn_Upper_b:
0){ _LL3278: _temp3277=(( struct Cyc_Absyn_Upper_b_struct*) _temp3275)->f1; goto
_LL3258;} else{ goto _LL3259;}} else{ goto _LL3259;} _LL3259: _LL3288: _temp3287=
_temp3253.f1; goto _LL3284; _LL3284: _temp3283= _temp3253.f2; if(( unsigned int)
_temp3283 > 1u?*(( int*) _temp3283) == Cyc_Absyn_Eq_constr: 0){ _LL3286:
_temp3285=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3283)->f1; if(
_temp3285 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3260;} else{ goto _LL3261;}}
else{ goto _LL3261;} _LL3261: goto _LL3262; _LL3256: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3289=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3289[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3290; _temp3290.tag=
Cyc_Absyn_Nullable_ps; _temp3290.f1= _temp3267; _temp3290;}); _temp3289;}); goto
_LL3254; _LL3258: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3291=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3291[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3292; _temp3292.tag= Cyc_Absyn_NonNullable_ps; _temp3292.f1= _temp3277;
_temp3292;}); _temp3291;}); goto _LL3254; _LL3260: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL3254; _LL3262: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL3254;
_LL3254:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3295=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3295[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3296; _temp3296.tag=
Cyc_Absyn_Pointer_mod; _temp3296.f1=( void*) ps; _temp3296.f2=( void*) _temp3200;
_temp3296.f3= tq; _temp3296;}); _temp3295;}); return({ struct _tuple4 _temp3293;
_temp3293.f1= _temp3250; _temp3293.f2= _temp3248; _temp3293.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3294=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3294->hd=( void*) tm; _temp3294->tl= _temp3246;
_temp3294;}); _temp3293;});}}} _LL3179: if( ! Cyc_Absynpp_print_all_tvars){ if(
_temp3216 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3216))->v)){ _temp3216= 0; _temp3218= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp3218);}}{ struct Cyc_List_List* _temp3299;
void* _temp3301; struct Cyc_Absyn_Tqual _temp3303; struct _tuple4 _temp3297= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp3214); _LL3304: _temp3303= _temp3297.f1; goto
_LL3302; _LL3302: _temp3301= _temp3297.f2; goto _LL3300; _LL3300: _temp3299=
_temp3297.f3; goto _LL3298; _LL3298: { struct Cyc_List_List* tms= _temp3299; if(
_temp3206 != 0){ tms=({ struct Cyc_List_List* _temp3305=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3305->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3306=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3306[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3307; _temp3307.tag= Cyc_Absyn_Attributes_mod;
_temp3307.f1= 0; _temp3307.f2= _temp3206; _temp3307;}); _temp3306;})); _temp3305->tl=
tms; _temp3305;});} tms=({ struct Cyc_List_List* _temp3308=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3308->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3309=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3309[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3310; _temp3310.tag= Cyc_Absyn_Function_mod;
_temp3310.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3311=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3311[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3312; _temp3312.tag= Cyc_Absyn_WithTypes;
_temp3312.f1= _temp3212; _temp3312.f2= _temp3210; _temp3312.f3= _temp3208;
_temp3312.f4= _temp3216; _temp3312;}); _temp3311;})); _temp3310;}); _temp3309;}));
_temp3308->tl= tms; _temp3308;}); if( _temp3218 != 0){ tms=({ struct Cyc_List_List*
_temp3313=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3313->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp3314=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp3314[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp3315; _temp3315.tag= Cyc_Absyn_TypeParams_mod; _temp3315.f1= _temp3218;
_temp3315.f2= 0; _temp3315.f3= 1; _temp3315;}); _temp3314;})); _temp3313->tl=
tms; _temp3313;});} return({ struct _tuple4 _temp3316; _temp3316.f1= _temp3303;
_temp3316.f2= _temp3301; _temp3316.f3= tms; _temp3316;});}} _LL3181: if(
_temp3222 == 0){ return({ struct _tuple4 _temp3317; _temp3317.f1= tq; _temp3317.f2=
t; _temp3317.f3= 0; _temp3317;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3222))->v);} _LL3183: if( _temp3226 == 0?
1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp3318; _temp3318.f1=
tq; _temp3318.f2= t; _temp3318.f3= 0; _temp3318;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3226))->v);} _LL3185:
return({ struct _tuple4 _temp3319; _temp3319.f1= tq; _temp3319.f2= t; _temp3319.f3=
0; _temp3319;}); _LL3173:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp3322; void*
_temp3324; struct Cyc_Absyn_Tqual _temp3326; struct _tuple4 _temp3320= Cyc_Absynpp_to_tms(
tq, t); _LL3327: _temp3326= _temp3320.f1; goto _LL3325; _LL3325: _temp3324=
_temp3320.f2; goto _LL3323; _LL3323: _temp3322= _temp3320.f3; goto _LL3321;
_LL3321: _temp3322=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp3322); if( _temp3322 == 0? dopt == 0: 0){ return({ struct Cyc_PP_Doc*(*
_temp3328)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3331= Cyc_Absynpp_tqual2doc(
_temp3326); struct Cyc_PP_Doc* _temp3332= Cyc_Absynpp_ntyp2doc( _temp3324);
struct Cyc_PP_Doc* _temp3329[ 2u]={ _temp3331, _temp3332}; struct _tagged_ptr0
_temp3330={ _temp3329, _temp3329, _temp3329 + 2u}; _temp3328( _temp3330);});}
else{ return({ struct Cyc_PP_Doc*(* _temp3333)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3336= Cyc_Absynpp_tqual2doc( _temp3326); struct Cyc_PP_Doc*
_temp3337= Cyc_Absynpp_ntyp2doc( _temp3324); struct Cyc_PP_Doc* _temp3338= Cyc_PP_text(({
char* _temp3340=" "; struct _tagged_string _temp3341; _temp3341.curr= _temp3340;
_temp3341.base= _temp3340; _temp3341.last_plus_one= _temp3340 + 2; _temp3341;}));
struct Cyc_PP_Doc* _temp3339= Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null( dopt))->v, _temp3322);
struct Cyc_PP_Doc* _temp3334[ 4u]={ _temp3336, _temp3337, _temp3338, _temp3339};
struct _tagged_ptr0 _temp3335={ _temp3334, _temp3334, _temp3334 + 4u}; _temp3333(
_temp3335);});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc( struct Cyc_Absyn_Structfield*
f){ if( f->width != 0){ return({ struct Cyc_PP_Doc*(* _temp3342)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3345= Cyc_Absynpp_tqtd2doc( f->tq,(
void*) f->type,({ struct Cyc_Core_Opt* _temp3354=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3354->v=( void*) Cyc_PP_textptr(
f->name); _temp3354;})); struct Cyc_PP_Doc* _temp3346= Cyc_PP_text(({ char*
_temp3352=":"; struct _tagged_string _temp3353; _temp3353.curr= _temp3352;
_temp3353.base= _temp3352; _temp3353.last_plus_one= _temp3352 + 2; _temp3353;}));
struct Cyc_PP_Doc* _temp3347= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( f->width))->v); struct Cyc_PP_Doc* _temp3348=
Cyc_Absynpp_atts2doc( f->attributes); struct Cyc_PP_Doc* _temp3349= Cyc_PP_text(({
char* _temp3350=";"; struct _tagged_string _temp3351; _temp3351.curr= _temp3350;
_temp3351.base= _temp3350; _temp3351.last_plus_one= _temp3350 + 2; _temp3351;}));
struct Cyc_PP_Doc* _temp3343[ 5u]={ _temp3345, _temp3346, _temp3347, _temp3348,
_temp3349}; struct _tagged_ptr0 _temp3344={ _temp3343, _temp3343, _temp3343 + 5u};
_temp3342( _temp3344);});} else{ return({ struct Cyc_PP_Doc*(* _temp3355)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3358= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp3363=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3363->v=( void*) Cyc_PP_textptr(
f->name); _temp3363;})); struct Cyc_PP_Doc* _temp3359= Cyc_Absynpp_atts2doc( f->attributes);
struct Cyc_PP_Doc* _temp3360= Cyc_PP_text(({ char* _temp3361=";"; struct
_tagged_string _temp3362; _temp3362.curr= _temp3361; _temp3362.base= _temp3361;
_temp3362.last_plus_one= _temp3361 + 2; _temp3362;})); struct Cyc_PP_Doc*
_temp3356[ 3u]={ _temp3358, _temp3359, _temp3360}; struct _tagged_ptr0 _temp3357={
_temp3356, _temp3356, _temp3356 + 3u}; _temp3355( _temp3357);});}} struct Cyc_PP_Doc*
Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string
sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,({
char* _temp3364=""; struct _tagged_string _temp3365; _temp3365.curr= _temp3364;
_temp3365.base= _temp3364; _temp3365.last_plus_one= _temp3364 + 1; _temp3365;}),
fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc( struct Cyc_Absyn_Tunionfield*
f){ return({ struct Cyc_PP_Doc*(* _temp3366)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3369= Cyc_Absynpp_scope2doc(( void*) f->sc); struct Cyc_PP_Doc*
_temp3370= Cyc_Absynpp_typedef_name2doc( f->name); struct Cyc_PP_Doc* _temp3371=
f->tvs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs); struct Cyc_PP_Doc*
_temp3372= f->typs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc( f->typs);
struct Cyc_PP_Doc* _temp3367[ 4u]={ _temp3369, _temp3370, _temp3371, _temp3372};
struct _tagged_ptr0 _temp3368={ _temp3367, _temp3367, _temp3367 + 4u}; _temp3366(
_temp3368);});} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List*
fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Tunionfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,({
char* _temp3373=","; struct _tagged_string _temp3374; _temp3374.curr= _temp3373;
_temp3374.base= _temp3373; _temp3374.last_plus_one= _temp3373 + 2; _temp3374;}),
fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f){ for( 0; tdl != 0; tdl=(( struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( tdl))->hd), 72, f); fprintf( f,"\n");}} struct _tagged_string Cyc_Absynpp_decllist2string(
struct Cyc_List_List* tdl){ return Cyc_PP_string_of_doc( Cyc_PP_seql(({ char*
_temp3375=""; struct _tagged_string _temp3376; _temp3376.curr= _temp3375;
_temp3376.base= _temp3375; _temp3376.last_plus_one= _temp3375 + 1; _temp3376;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_decl2doc, tdl)), 72);}
struct _tagged_string Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp* e){ return
Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e), 72);} struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt* s){ return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s),
72);} struct _tagged_string Cyc_Absynpp_typ2string( void* t){ Cyc_Absynpp_dont_rewrite_temp_tvars=
1;{ struct _tagged_string _temp3377= Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc(
t), 72); goto _LL3378; _LL3378: Cyc_Absynpp_dont_rewrite_temp_tvars= 0; return
_temp3377;}} struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0* v){
return Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);} struct
_tagged_string Cyc_Absynpp_prim2string( void* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc( p), 72);} struct _tagged_string Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_pat2doc( p),
72);} struct _tagged_string Cyc_Absynpp_scope2string( void* sc){ void* _temp3379=
sc; _LL3381: if( _temp3379 ==( void*) Cyc_Absyn_Static){ goto _LL3382;} else{
goto _LL3383;} _LL3383: if( _temp3379 ==( void*) Cyc_Absyn_Public){ goto _LL3384;}
else{ goto _LL3385;} _LL3385: if( _temp3379 ==( void*) Cyc_Absyn_Extern){ goto
_LL3386;} else{ goto _LL3387;} _LL3387: if( _temp3379 ==( void*) Cyc_Absyn_ExternC){
goto _LL3388;} else{ goto _LL3389;} _LL3389: if( _temp3379 ==( void*) Cyc_Absyn_Abstract){
goto _LL3390;} else{ goto _LL3380;} _LL3382: return({ char* _temp3391="static";
struct _tagged_string _temp3392; _temp3392.curr= _temp3391; _temp3392.base=
_temp3391; _temp3392.last_plus_one= _temp3391 + 7; _temp3392;}); _LL3384: return({
char* _temp3393="public"; struct _tagged_string _temp3394; _temp3394.curr=
_temp3393; _temp3394.base= _temp3393; _temp3394.last_plus_one= _temp3393 + 7;
_temp3394;}); _LL3386: return({ char* _temp3395="extern"; struct _tagged_string
_temp3396; _temp3396.curr= _temp3395; _temp3396.base= _temp3395; _temp3396.last_plus_one=
_temp3395 + 7; _temp3396;}); _LL3388: return({ char* _temp3397="extern \"C\"";
struct _tagged_string _temp3398; _temp3398.curr= _temp3397; _temp3398.base=
_temp3397; _temp3398.last_plus_one= _temp3397 + 11; _temp3398;}); _LL3390:
return({ char* _temp3399="abstract"; struct _tagged_string _temp3400; _temp3400.curr=
_temp3399; _temp3400.base= _temp3399; _temp3400.last_plus_one= _temp3399 + 9;
_temp3400;}); _LL3380:;}