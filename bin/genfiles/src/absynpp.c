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
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[
12u]; extern char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
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
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt); extern
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields);
int Cyc_Absynpp_expand_typedefs= 0; int Cyc_Absynpp_qvar_to_Cids= 0; int Cyc_Absynpp_to_VC=
0; int Cyc_Absynpp_decls_first= 0; int Cyc_Absynpp_dont_rewrite_temp_tvars= 0;
int Cyc_Absynpp_print_all_tvars= 0; int Cyc_Absynpp_print_all_kinds= 0; int Cyc_Absynpp_print_using_stmts=
0; int Cyc_Absynpp_print_externC_stmts= 0; static struct Cyc_List_List* Cyc_Absynpp_curr_namespace=
0; static void Cyc_Absynpp_curr_namespace_add( struct _tagged_string* v){ Cyc_Absynpp_curr_namespace=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_Absynpp_curr_namespace,({ struct Cyc_List_List* _temp0=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp0->hd=( void*) v; _temp0->tl= 0;
_temp0;}));} static void Cyc_Absynpp_suppr_last( struct Cyc_List_List** l){ if(((
struct Cyc_List_List*) _check_null(* l))->tl == 0){* l= 0;} else{(( void(*)(
struct Cyc_List_List** l)) Cyc_Absynpp_suppr_last)(&(( struct Cyc_List_List*)
_check_null(* l))->tl);}} static void Cyc_Absynpp_curr_namespace_drop(){(( void(*)(
struct Cyc_List_List** l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _tagged_string Cyc_Absynpp_char_escape( char c){ switch( c){ case '\a':
_LL1: return({ char* _temp3="\\a"; struct _tagged_string _temp4; _temp4.curr=
_temp3; _temp4.base= _temp3; _temp4.last_plus_one= _temp3 + 3; _temp4;}); case '\b':
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
Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp293="("; struct _tagged_string
_temp294; _temp294.curr= _temp293; _temp294.base= _temp293; _temp294.last_plus_one=
_temp293 + 2; _temp294;})), rest, Cyc_PP_text(({ char* _temp295=")"; struct
_tagged_string _temp296; _temp296.curr= _temp295; _temp296.base= _temp295;
_temp296.last_plus_one= _temp295 + 2; _temp296;}))); void* _temp193=( void*)((
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
_temp209; struct Cyc_Core_Opt* _temp239; int _temp241; struct Cyc_List_List*
_temp243; struct Cyc_Position_Segment* _temp245; struct Cyc_List_List* _temp247;
_LL235: if(*(( int*) _temp233) == Cyc_Absyn_WithTypes){ _LL244: _temp243=((
struct Cyc_Absyn_WithTypes_struct*) _temp233)->f1; goto _LL242; _LL242: _temp241=((
struct Cyc_Absyn_WithTypes_struct*) _temp233)->f2; goto _LL240; _LL240: _temp239=((
struct Cyc_Absyn_WithTypes_struct*) _temp233)->f3; goto _LL236;} else{ goto
_LL237;} _LL237: if(*(( int*) _temp233) == Cyc_Absyn_NoTypes){ _LL248: _temp247=((
struct Cyc_Absyn_NoTypes_struct*) _temp233)->f1; goto _LL246; _LL246: _temp245=((
struct Cyc_Absyn_NoTypes_struct*) _temp233)->f2; goto _LL238;} else{ goto _LL234;}
_LL236: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_funargs2doc( _temp243,
_temp241, _temp239)); _LL238: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_group(({
char* _temp249="("; struct _tagged_string _temp250; _temp250.curr= _temp249;
_temp250.base= _temp249; _temp250.last_plus_one= _temp249 + 2; _temp250;}),({
char* _temp251=")"; struct _tagged_string _temp252; _temp252.curr= _temp251;
_temp252.base= _temp251; _temp252.last_plus_one= _temp251 + 2; _temp252;}),({
char* _temp253=","; struct _tagged_string _temp254; _temp254.curr= _temp253;
_temp254.base= _temp253; _temp254.last_plus_one= _temp253 + 2; _temp254;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tagged_string*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr, _temp247)));
_LL234:;} _LL202: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_atts2doc(
_temp211)); _LL204: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} if( _temp215){ return Cyc_Absynpp_cat2(
rest, Cyc_Absynpp_ktvars2doc( _temp219));} else{ return Cyc_Absynpp_cat2( rest,
Cyc_Absynpp_tvars2doc( _temp219));} _LL206: { struct Cyc_PP_Doc* ptr;{ void*
_temp255= _temp225; struct Cyc_Absyn_Exp* _temp263; struct Cyc_Absyn_Exp*
_temp265; _LL257: if(( unsigned int) _temp255 > 1u?*(( int*) _temp255) == Cyc_Absyn_Nullable_ps:
0){ _LL264: _temp263=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp255)->f1;
goto _LL258;} else{ goto _LL259;} _LL259: if(( unsigned int) _temp255 > 1u?*((
int*) _temp255) == Cyc_Absyn_NonNullable_ps: 0){ _LL266: _temp265=(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp255)->f1; goto _LL260;} else{ goto _LL261;} _LL261: if( _temp255 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL262;} else{ goto _LL256;} _LL258: if( Cyc_Evexp_eval_const_uint_exp(
_temp263) == 1){ ptr= Cyc_PP_text(({ char* _temp267="*"; struct _tagged_string
_temp268; _temp268.curr= _temp267; _temp268.base= _temp267; _temp268.last_plus_one=
_temp267 + 2; _temp268;}));} else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(({ char*
_temp269="*"; struct _tagged_string _temp270; _temp270.curr= _temp269; _temp270.base=
_temp269; _temp270.last_plus_one= _temp269 + 2; _temp270;})), Cyc_PP_text(({
char* _temp271="{"; struct _tagged_string _temp272; _temp272.curr= _temp271;
_temp272.base= _temp271; _temp272.last_plus_one= _temp271 + 2; _temp272;})), Cyc_Absynpp_exp2doc(
_temp263), Cyc_PP_text(({ char* _temp273="}"; struct _tagged_string _temp274;
_temp274.curr= _temp273; _temp274.base= _temp273; _temp274.last_plus_one=
_temp273 + 2; _temp274;})));} goto _LL256; _LL260: if( Cyc_Evexp_eval_const_uint_exp(
_temp265) == 1){ ptr= Cyc_PP_text(({ char* _temp275="@"; struct _tagged_string
_temp276; _temp276.curr= _temp275; _temp276.base= _temp275; _temp276.last_plus_one=
_temp275 + 2; _temp276;}));} else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(({ char*
_temp277="@"; struct _tagged_string _temp278; _temp278.curr= _temp277; _temp278.base=
_temp277; _temp278.last_plus_one= _temp277 + 2; _temp278;})), Cyc_PP_text(({
char* _temp279="{"; struct _tagged_string _temp280; _temp280.curr= _temp279;
_temp280.base= _temp279; _temp280.last_plus_one= _temp279 + 2; _temp280;})), Cyc_Absynpp_exp2doc(
_temp265), Cyc_PP_text(({ char* _temp281="}"; struct _tagged_string _temp282;
_temp282.curr= _temp281; _temp282.base= _temp281; _temp282.last_plus_one=
_temp281 + 2; _temp282;})));} goto _LL256; _LL262: ptr= Cyc_PP_text(({ char*
_temp283="?"; struct _tagged_string _temp284; _temp284.curr= _temp283; _temp284.base=
_temp283; _temp284.last_plus_one= _temp283 + 2; _temp284;})); goto _LL256;
_LL256:;}{ void* _temp285= Cyc_Tcutil_compress( _temp223); _LL287: if( _temp285
==( void*) Cyc_Absyn_HeapRgn){ goto _LL288;} else{ goto _LL289;} _LL289: goto
_LL290; _LL288: return Cyc_Absynpp_cat2( ptr, rest); _LL290: return Cyc_Absynpp_cat4(
ptr, Cyc_Absynpp_ntyp2doc( _temp223), Cyc_PP_text(({ char* _temp291=" "; struct
_tagged_string _temp292; _temp292.curr= _temp291; _temp292.base= _temp291;
_temp292.last_plus_one= _temp291 + 2; _temp292;})), rest); _LL286:;}} _LL194:;}}
struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){ void* _temp297= Cyc_Tcutil_compress(
t); _LL299: if( _temp297 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL300;} else{ goto
_LL301;} _LL301: goto _LL302; _LL300: return Cyc_PP_text(({ char* _temp303="`H";
struct _tagged_string _temp304; _temp304.curr= _temp303; _temp304.base= _temp303;
_temp304.last_plus_one= _temp303 + 3; _temp304;})); _LL302: return Cyc_Absynpp_ntyp2doc(
t); _LL298:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
regions, struct Cyc_List_List** effects, void* t){ void* _temp305= Cyc_Tcutil_compress(
t); void* _temp313; struct Cyc_List_List* _temp315; _LL307: if(( unsigned int)
_temp305 > 4u?*(( int*) _temp305) == Cyc_Absyn_AccessEff: 0){ _LL314: _temp313=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp305)->f1; goto _LL308;} else{
goto _LL309;} _LL309: if(( unsigned int) _temp305 > 4u?*(( int*) _temp305) ==
Cyc_Absyn_JoinEff: 0){ _LL316: _temp315=(( struct Cyc_Absyn_JoinEff_struct*)
_temp305)->f1; goto _LL310;} else{ goto _LL311;} _LL311: goto _LL312; _LL308:*
regions=({ struct Cyc_List_List* _temp317=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp317->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp313); _temp317->tl=* regions; _temp317;}); goto _LL306; _LL310: for( 0;
_temp315 != 0; _temp315=(( struct Cyc_List_List*) _check_null( _temp315))->tl){
Cyc_Absynpp_effects2docs( regions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp315))->hd);} goto _LL306; _LL312:* effects=({ struct Cyc_List_List*
_temp318=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp318->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp318->tl=* effects; _temp318;});
goto _LL306; _LL306:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_Absynpp_group(({
char* _temp319=""; struct _tagged_string _temp320; _temp320.curr= _temp319;
_temp320.base= _temp319; _temp320.last_plus_one= _temp319 + 1; _temp320;}),({
char* _temp321=""; struct _tagged_string _temp322; _temp322.curr= _temp321;
_temp322.base= _temp321; _temp322.last_plus_one= _temp321 + 1; _temp322;}),({
char* _temp323="+"; struct _tagged_string _temp324; _temp324.curr= _temp323;
_temp324.base= _temp323; _temp324.last_plus_one= _temp323 + 2; _temp324;}),
effects);} else{ struct Cyc_PP_Doc* _temp331= Cyc_Absynpp_group(({ char*
_temp325="{"; struct _tagged_string _temp326; _temp326.curr= _temp325; _temp326.base=
_temp325; _temp326.last_plus_one= _temp325 + 2; _temp326;}),({ char* _temp327="}";
struct _tagged_string _temp328; _temp328.curr= _temp327; _temp328.base= _temp327;
_temp328.last_plus_one= _temp327 + 2; _temp328;}),({ char* _temp329=","; struct
_tagged_string _temp330; _temp330.curr= _temp329; _temp330.base= _temp329;
_temp330.last_plus_one= _temp329 + 2; _temp330;}), regions); goto _LL332; _LL332:
return Cyc_Absynpp_group(({ char* _temp333=""; struct _tagged_string _temp334;
_temp334.curr= _temp333; _temp334.base= _temp333; _temp334.last_plus_one=
_temp333 + 1; _temp334;}),({ char* _temp335=""; struct _tagged_string _temp336;
_temp336.curr= _temp335; _temp336.base= _temp335; _temp336.last_plus_one=
_temp335 + 1; _temp336;}),({ char* _temp337="+"; struct _tagged_string _temp338;
_temp338.curr= _temp337; _temp338.base= _temp337; _temp338.last_plus_one=
_temp337 + 2; _temp338;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( effects,({ struct Cyc_List_List*
_temp339=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp339->hd=( void*) _temp331; _temp339->tl= 0; _temp339;})));}} struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc* s;{ void* _temp340= t; struct
Cyc_Absyn_Exp* _temp386; struct Cyc_Absyn_Tqual _temp388; void* _temp390; struct
Cyc_Absyn_FnInfo _temp392; struct Cyc_Absyn_PtrInfo _temp394; int _temp396;
struct Cyc_Core_Opt* _temp398; void* _temp400; struct Cyc_Absyn_Tvar* _temp402;
struct Cyc_Absyn_TunionInfo _temp404; void* _temp406; struct Cyc_List_List*
_temp408; void* _temp410; struct Cyc_Absyn_TunionFieldInfo _temp412; struct Cyc_List_List*
_temp414; void* _temp416; void* _temp418; void* _temp420; struct Cyc_List_List*
_temp422; struct Cyc_Absyn_Structdecl** _temp424; struct Cyc_List_List* _temp426;
struct _tuple0* _temp428; struct Cyc_Absyn_Uniondecl** _temp430; struct Cyc_List_List*
_temp432; struct _tuple0* _temp434; struct Cyc_List_List* _temp436; struct Cyc_List_List*
_temp438; struct Cyc_Absyn_Enumdecl* _temp440; struct _tuple0* _temp442; struct
Cyc_Core_Opt* _temp444; struct Cyc_List_List* _temp446; struct _tuple0* _temp448;
void* _temp450; void* _temp452; struct Cyc_List_List* _temp454; _LL342: if((
unsigned int) _temp340 > 4u?*(( int*) _temp340) == Cyc_Absyn_ArrayType: 0){
_LL391: _temp390=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp340)->f1;
goto _LL389; _LL389: _temp388=(( struct Cyc_Absyn_ArrayType_struct*) _temp340)->f2;
goto _LL387; _LL387: _temp386=(( struct Cyc_Absyn_ArrayType_struct*) _temp340)->f3;
goto _LL343;} else{ goto _LL344;} _LL344: if(( unsigned int) _temp340 > 4u?*((
int*) _temp340) == Cyc_Absyn_FnType: 0){ _LL393: _temp392=(( struct Cyc_Absyn_FnType_struct*)
_temp340)->f1; goto _LL345;} else{ goto _LL346;} _LL346: if(( unsigned int)
_temp340 > 4u?*(( int*) _temp340) == Cyc_Absyn_PointerType: 0){ _LL395: _temp394=((
struct Cyc_Absyn_PointerType_struct*) _temp340)->f1; goto _LL347;} else{ goto
_LL348;} _LL348: if( _temp340 ==( void*) Cyc_Absyn_VoidType){ goto _LL349;}
else{ goto _LL350;} _LL350: if(( unsigned int) _temp340 > 4u?*(( int*) _temp340)
== Cyc_Absyn_Evar: 0){ _LL401: _temp400=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp340)->f1; goto _LL399; _LL399: _temp398=(( struct Cyc_Absyn_Evar_struct*)
_temp340)->f2; goto _LL397; _LL397: _temp396=(( struct Cyc_Absyn_Evar_struct*)
_temp340)->f3; goto _LL351;} else{ goto _LL352;} _LL352: if(( unsigned int)
_temp340 > 4u?*(( int*) _temp340) == Cyc_Absyn_VarType: 0){ _LL403: _temp402=((
struct Cyc_Absyn_VarType_struct*) _temp340)->f1; goto _LL353;} else{ goto _LL354;}
_LL354: if(( unsigned int) _temp340 > 4u?*(( int*) _temp340) == Cyc_Absyn_TunionType:
0){ _LL405: _temp404=(( struct Cyc_Absyn_TunionType_struct*) _temp340)->f1;
_LL411: _temp410=( void*) _temp404.tunion_info; goto _LL409; _LL409: _temp408=
_temp404.targs; goto _LL407; _LL407: _temp406=( void*) _temp404.rgn; goto _LL355;}
else{ goto _LL356;} _LL356: if(( unsigned int) _temp340 > 4u?*(( int*) _temp340)
== Cyc_Absyn_TunionFieldType: 0){ _LL413: _temp412=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp340)->f1; _LL417: _temp416=( void*) _temp412.field_info; goto _LL415;
_LL415: _temp414= _temp412.targs; goto _LL357;} else{ goto _LL358;} _LL358: if((
unsigned int) _temp340 > 4u?*(( int*) _temp340) == Cyc_Absyn_IntType: 0){ _LL421:
_temp420=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp340)->f1; goto _LL419;
_LL419: _temp418=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp340)->f2;
goto _LL359;} else{ goto _LL360;} _LL360: if( _temp340 ==( void*) Cyc_Absyn_FloatType){
goto _LL361;} else{ goto _LL362;} _LL362: if( _temp340 ==( void*) Cyc_Absyn_DoubleType){
goto _LL363;} else{ goto _LL364;} _LL364: if(( unsigned int) _temp340 > 4u?*((
int*) _temp340) == Cyc_Absyn_TupleType: 0){ _LL423: _temp422=(( struct Cyc_Absyn_TupleType_struct*)
_temp340)->f1; goto _LL365;} else{ goto _LL366;} _LL366: if(( unsigned int)
_temp340 > 4u?*(( int*) _temp340) == Cyc_Absyn_StructType: 0){ _LL429: _temp428=((
struct Cyc_Absyn_StructType_struct*) _temp340)->f1; goto _LL427; _LL427:
_temp426=(( struct Cyc_Absyn_StructType_struct*) _temp340)->f2; goto _LL425;
_LL425: _temp424=(( struct Cyc_Absyn_StructType_struct*) _temp340)->f3; goto
_LL367;} else{ goto _LL368;} _LL368: if(( unsigned int) _temp340 > 4u?*(( int*)
_temp340) == Cyc_Absyn_UnionType: 0){ _LL435: _temp434=(( struct Cyc_Absyn_UnionType_struct*)
_temp340)->f1; goto _LL433; _LL433: _temp432=(( struct Cyc_Absyn_UnionType_struct*)
_temp340)->f2; goto _LL431; _LL431: _temp430=(( struct Cyc_Absyn_UnionType_struct*)
_temp340)->f3; goto _LL369;} else{ goto _LL370;} _LL370: if(( unsigned int)
_temp340 > 4u?*(( int*) _temp340) == Cyc_Absyn_AnonStructType: 0){ _LL437:
_temp436=(( struct Cyc_Absyn_AnonStructType_struct*) _temp340)->f1; goto _LL371;}
else{ goto _LL372;} _LL372: if(( unsigned int) _temp340 > 4u?*(( int*) _temp340)
== Cyc_Absyn_AnonUnionType: 0){ _LL439: _temp438=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp340)->f1; goto _LL373;} else{ goto _LL374;} _LL374: if(( unsigned int)
_temp340 > 4u?*(( int*) _temp340) == Cyc_Absyn_EnumType: 0){ _LL443: _temp442=((
struct Cyc_Absyn_EnumType_struct*) _temp340)->f1; goto _LL441; _LL441: _temp440=((
struct Cyc_Absyn_EnumType_struct*) _temp340)->f2; goto _LL375;} else{ goto
_LL376;} _LL376: if(( unsigned int) _temp340 > 4u?*(( int*) _temp340) == Cyc_Absyn_TypedefType:
0){ _LL449: _temp448=(( struct Cyc_Absyn_TypedefType_struct*) _temp340)->f1;
goto _LL447; _LL447: _temp446=(( struct Cyc_Absyn_TypedefType_struct*) _temp340)->f2;
goto _LL445; _LL445: _temp444=(( struct Cyc_Absyn_TypedefType_struct*) _temp340)->f3;
goto _LL377;} else{ goto _LL378;} _LL378: if(( unsigned int) _temp340 > 4u?*((
int*) _temp340) == Cyc_Absyn_RgnHandleType: 0){ _LL451: _temp450=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp340)->f1; goto _LL379;} else{ goto
_LL380;} _LL380: if( _temp340 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL381;} else{
goto _LL382;} _LL382: if(( unsigned int) _temp340 > 4u?*(( int*) _temp340) ==
Cyc_Absyn_AccessEff: 0){ _LL453: _temp452=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp340)->f1; goto _LL383;} else{ goto _LL384;} _LL384: if(( unsigned int)
_temp340 > 4u?*(( int*) _temp340) == Cyc_Absyn_JoinEff: 0){ _LL455: _temp454=((
struct Cyc_Absyn_JoinEff_struct*) _temp340)->f1; goto _LL385;} else{ goto _LL341;}
_LL343: return Cyc_PP_text(({ char* _temp456="[[[array]]]"; struct
_tagged_string _temp457; _temp457.curr= _temp456; _temp457.base= _temp456;
_temp457.last_plus_one= _temp456 + 12; _temp457;})); _LL345: return Cyc_PP_nil_doc();
_LL347: return Cyc_PP_nil_doc(); _LL349: s= Cyc_PP_text(({ char* _temp458="void";
struct _tagged_string _temp459; _temp459.curr= _temp458; _temp459.base= _temp458;
_temp459.last_plus_one= _temp458 + 5; _temp459;})); goto _LL341; _LL351: if(
_temp398 != 0){ return Cyc_Absynpp_ntyp2doc(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp398))->v);} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(({ char*
_temp460="%"; struct _tagged_string _temp461; _temp461.curr= _temp460; _temp461.base=
_temp460; _temp461.last_plus_one= _temp460 + 2; _temp461;})), Cyc_PP_text(({
char* _temp462="("; struct _tagged_string _temp463; _temp463.curr= _temp462;
_temp463.base= _temp462; _temp463.last_plus_one= _temp462 + 2; _temp463;})), Cyc_PP_text(
xprintf("%d", _temp396)), Cyc_PP_text(({ char* _temp464=")::"; struct
_tagged_string _temp465; _temp465.curr= _temp464; _temp465.base= _temp464;
_temp465.last_plus_one= _temp464 + 4; _temp465;})), Cyc_Absynpp_kind2doc(
_temp400));} goto _LL341; _LL353: s= Cyc_Absynpp_textptr( _temp402->name); if(
Cyc_Absynpp_print_all_kinds){ s= Cyc_Absynpp_cat3( s, Cyc_PP_text(({ char*
_temp466="::"; struct _tagged_string _temp467; _temp467.curr= _temp466; _temp467.base=
_temp466; _temp467.last_plus_one= _temp466 + 3; _temp467;})), Cyc_Absynpp_ckind2doc(
_temp402->kind));} if( ! Cyc_Absynpp_dont_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_temp402): 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp468="_ /* ";
struct _tagged_string _temp469; _temp469.curr= _temp468; _temp469.base= _temp468;
_temp469.last_plus_one= _temp468 + 6; _temp469;})), s, Cyc_PP_text(({ char*
_temp470=" */"; struct _tagged_string _temp471; _temp471.curr= _temp470;
_temp471.base= _temp470; _temp471.last_plus_one= _temp470 + 4; _temp471;})));}
goto _LL341; _LL355:{ void* _temp472= _temp410; struct Cyc_Absyn_UnknownTunionInfo
_temp478; int _temp480; struct _tuple0* _temp482; struct Cyc_Absyn_Tuniondecl*
_temp484; struct Cyc_Absyn_Tuniondecl _temp486; int _temp487; struct Cyc_Core_Opt*
_temp489; struct Cyc_List_List* _temp491; struct _tuple0* _temp493; void*
_temp495; _LL474: if(*(( int*) _temp472) == Cyc_Absyn_UnknownTunion){ _LL479:
_temp478=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp472)->f1; _LL483:
_temp482= _temp478.name; goto _LL481; _LL481: _temp480= _temp478.is_xtunion;
goto _LL475;} else{ goto _LL476;} _LL476: if(*(( int*) _temp472) == Cyc_Absyn_KnownTunion){
_LL485: _temp484=(( struct Cyc_Absyn_KnownTunion_struct*) _temp472)->f1;
_temp486=* _temp484; _LL496: _temp495=( void*) _temp486.sc; goto _LL494; _LL494:
_temp493= _temp486.name; goto _LL492; _LL492: _temp491= _temp486.tvs; goto
_LL490; _LL490: _temp489= _temp486.fields; goto _LL488; _LL488: _temp487=
_temp486.is_xtunion; goto _LL477;} else{ goto _LL473;} _LL475: _temp493=
_temp482; _temp487= _temp480; goto _LL477; _LL477: { struct Cyc_PP_Doc* _temp501=
Cyc_PP_text( _temp487?({ char* _temp497="xtunion "; struct _tagged_string
_temp498; _temp498.curr= _temp497; _temp498.base= _temp497; _temp498.last_plus_one=
_temp497 + 9; _temp498;}):({ char* _temp499="tunion "; struct _tagged_string
_temp500; _temp500.curr= _temp499; _temp500.base= _temp499; _temp500.last_plus_one=
_temp499 + 8; _temp500;})); goto _LL502; _LL502:{ void* _temp503= Cyc_Tcutil_compress(
_temp406); _LL505: if( _temp503 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL506;}
else{ goto _LL507;} _LL507: goto _LL508; _LL506: s= Cyc_Absynpp_cat3( _temp501,
Cyc_Absynpp_qvar2doc( _temp493), Cyc_Absynpp_tps2doc( _temp408)); goto _LL504;
_LL508: s= Cyc_Absynpp_cat5( _temp501, Cyc_Absynpp_typ2doc( _temp406), Cyc_PP_text(({
char* _temp509=" "; struct _tagged_string _temp510; _temp510.curr= _temp509;
_temp510.base= _temp509; _temp510.last_plus_one= _temp509 + 2; _temp510;})), Cyc_Absynpp_qvar2doc(
_temp493), Cyc_Absynpp_tps2doc( _temp408)); goto _LL504; _LL504:;} goto _LL473;}
_LL473:;} goto _LL341; _LL357:{ void* _temp511= _temp416; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp517; int _temp519; struct _tuple0* _temp521; struct _tuple0* _temp523;
struct Cyc_Absyn_Tunionfield* _temp525; struct Cyc_Absyn_Tunionfield _temp527;
void* _temp528; struct Cyc_Position_Segment* _temp530; struct Cyc_List_List*
_temp532; struct Cyc_List_List* _temp534; struct _tuple0* _temp536; struct Cyc_Absyn_Tuniondecl*
_temp538; struct Cyc_Absyn_Tuniondecl _temp540; int _temp541; struct Cyc_Core_Opt*
_temp543; struct Cyc_List_List* _temp545; struct _tuple0* _temp547; void*
_temp549; _LL513: if(*(( int*) _temp511) == Cyc_Absyn_UnknownTunionfield){
_LL518: _temp517=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp511)->f1;
_LL524: _temp523= _temp517.tunion_name; goto _LL522; _LL522: _temp521= _temp517.field_name;
goto _LL520; _LL520: _temp519= _temp517.is_xtunion; goto _LL514;} else{ goto
_LL515;} _LL515: if(*(( int*) _temp511) == Cyc_Absyn_KnownTunionfield){ _LL539:
_temp538=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp511)->f1; _temp540=*
_temp538; _LL550: _temp549=( void*) _temp540.sc; goto _LL548; _LL548: _temp547=
_temp540.name; goto _LL546; _LL546: _temp545= _temp540.tvs; goto _LL544; _LL544:
_temp543= _temp540.fields; goto _LL542; _LL542: _temp541= _temp540.is_xtunion;
goto _LL526; _LL526: _temp525=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp511)->f2; _temp527=* _temp525; _LL537: _temp536= _temp527.name; goto _LL535;
_LL535: _temp534= _temp527.tvs; goto _LL533; _LL533: _temp532= _temp527.typs;
goto _LL531; _LL531: _temp530= _temp527.loc; goto _LL529; _LL529: _temp528=(
void*) _temp527.sc; goto _LL516;} else{ goto _LL512;} _LL514: _temp547= _temp523;
_temp541= _temp519; _temp536= _temp521; goto _LL516; _LL516: { struct Cyc_PP_Doc*
_temp555= Cyc_PP_text( _temp541?({ char* _temp551="xtunion "; struct
_tagged_string _temp552; _temp552.curr= _temp551; _temp552.base= _temp551;
_temp552.last_plus_one= _temp551 + 9; _temp552;}):({ char* _temp553="tunion ";
struct _tagged_string _temp554; _temp554.curr= _temp553; _temp554.base= _temp553;
_temp554.last_plus_one= _temp553 + 8; _temp554;})); goto _LL556; _LL556: s= Cyc_Absynpp_cat4(
_temp555, Cyc_Absynpp_qvar2doc( _temp547), Cyc_PP_text(({ char* _temp557=".";
struct _tagged_string _temp558; _temp558.curr= _temp557; _temp558.base= _temp557;
_temp558.last_plus_one= _temp557 + 2; _temp558;})), Cyc_Absynpp_textptr((*
_temp536).f2)); goto _LL512;} _LL512:;} goto _LL341; _LL359: { struct
_tagged_string sns; struct _tagged_string ts;{ void* _temp559= _temp420; _LL561:
if( _temp559 ==( void*) Cyc_Absyn_Signed){ goto _LL562;} else{ goto _LL563;}
_LL563: if( _temp559 ==( void*) Cyc_Absyn_Unsigned){ goto _LL564;} else{ goto
_LL560;} _LL562: sns=({ char* _temp565=""; struct _tagged_string _temp566;
_temp566.curr= _temp565; _temp566.base= _temp565; _temp566.last_plus_one=
_temp565 + 1; _temp566;}); goto _LL560; _LL564: sns=({ char* _temp567="unsigned ";
struct _tagged_string _temp568; _temp568.curr= _temp567; _temp568.base= _temp567;
_temp568.last_plus_one= _temp567 + 10; _temp568;}); goto _LL560; _LL560:;}{ void*
_temp569= _temp418; _LL571: if( _temp569 ==( void*) Cyc_Absyn_B1){ goto _LL572;}
else{ goto _LL573;} _LL573: if( _temp569 ==( void*) Cyc_Absyn_B2){ goto _LL574;}
else{ goto _LL575;} _LL575: if( _temp569 ==( void*) Cyc_Absyn_B4){ goto _LL576;}
else{ goto _LL577;} _LL577: if( _temp569 ==( void*) Cyc_Absyn_B8){ goto _LL578;}
else{ goto _LL570;} _LL572:{ void* _temp579= _temp420; _LL581: if( _temp579 ==(
void*) Cyc_Absyn_Signed){ goto _LL582;} else{ goto _LL583;} _LL583: if( _temp579
==( void*) Cyc_Absyn_Unsigned){ goto _LL584;} else{ goto _LL580;} _LL582: sns=({
char* _temp585="signed "; struct _tagged_string _temp586; _temp586.curr=
_temp585; _temp586.base= _temp585; _temp586.last_plus_one= _temp585 + 8;
_temp586;}); goto _LL580; _LL584: sns=({ char* _temp587=""; struct
_tagged_string _temp588; _temp588.curr= _temp587; _temp588.base= _temp587;
_temp588.last_plus_one= _temp587 + 1; _temp588;}); goto _LL580; _LL580:;} ts=({
char* _temp589="char"; struct _tagged_string _temp590; _temp590.curr= _temp589;
_temp590.base= _temp589; _temp590.last_plus_one= _temp589 + 5; _temp590;}); goto
_LL570; _LL574: ts=({ char* _temp591="short"; struct _tagged_string _temp592;
_temp592.curr= _temp591; _temp592.base= _temp591; _temp592.last_plus_one=
_temp591 + 6; _temp592;}); goto _LL570; _LL576: ts=({ char* _temp593="int";
struct _tagged_string _temp594; _temp594.curr= _temp593; _temp594.base= _temp593;
_temp594.last_plus_one= _temp593 + 4; _temp594;}); goto _LL570; _LL578: ts= Cyc_Absynpp_to_VC?({
char* _temp595="__int64"; struct _tagged_string _temp596; _temp596.curr=
_temp595; _temp596.base= _temp595; _temp596.last_plus_one= _temp595 + 8;
_temp596;}):({ char* _temp597="long long"; struct _tagged_string _temp598;
_temp598.curr= _temp597; _temp598.base= _temp597; _temp598.last_plus_one=
_temp597 + 10; _temp598;}); goto _LL570; _LL570:;} s= Cyc_PP_text(({ struct
_tagged_string _temp599= sns; struct _tagged_string _temp600= ts; xprintf("%.*s%.*s",
_temp599.last_plus_one - _temp599.curr, _temp599.curr, _temp600.last_plus_one -
_temp600.curr, _temp600.curr);})); goto _LL341;} _LL361: s= Cyc_PP_text(({ char*
_temp601="float"; struct _tagged_string _temp602; _temp602.curr= _temp601;
_temp602.base= _temp601; _temp602.last_plus_one= _temp601 + 6; _temp602;}));
goto _LL341; _LL363: s= Cyc_PP_text(({ char* _temp603="double"; struct
_tagged_string _temp604; _temp604.curr= _temp603; _temp604.base= _temp603;
_temp604.last_plus_one= _temp603 + 7; _temp604;})); goto _LL341; _LL365: s= Cyc_Absynpp_cat2(
Cyc_PP_text(({ char* _temp605="$"; struct _tagged_string _temp606; _temp606.curr=
_temp605; _temp606.base= _temp605; _temp606.last_plus_one= _temp605 + 2;
_temp606;})), Cyc_Absynpp_args2doc( _temp422)); goto _LL341; _LL367: if(
_temp428 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp607="struct ";
struct _tagged_string _temp608; _temp608.curr= _temp607; _temp608.base= _temp607;
_temp608.last_plus_one= _temp607 + 8; _temp608;})), Cyc_Absynpp_tps2doc(
_temp426));} else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp609="struct ";
struct _tagged_string _temp610; _temp610.curr= _temp609; _temp610.base= _temp609;
_temp610.last_plus_one= _temp609 + 8; _temp610;})), _temp428 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp428)), Cyc_Absynpp_tps2doc(
_temp426));} goto _LL341; _LL369: if( _temp434 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(({
char* _temp611="union "; struct _tagged_string _temp612; _temp612.curr= _temp611;
_temp612.base= _temp611; _temp612.last_plus_one= _temp611 + 7; _temp612;})), Cyc_Absynpp_tps2doc(
_temp432));} else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp613="union ";
struct _tagged_string _temp614; _temp614.curr= _temp613; _temp614.base= _temp613;
_temp614.last_plus_one= _temp613 + 7; _temp614;})), _temp434 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp434)), Cyc_Absynpp_tps2doc(
_temp432));} goto _LL341; _LL371: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char*
_temp615="struct {"; struct _tagged_string _temp616; _temp616.curr= _temp615;
_temp616.base= _temp615; _temp616.last_plus_one= _temp615 + 9; _temp616;})), Cyc_PP_nest(
2, Cyc_Absynpp_structfields2doc( _temp436)), Cyc_PP_text(({ char* _temp617="}";
struct _tagged_string _temp618; _temp618.curr= _temp617; _temp618.base= _temp617;
_temp618.last_plus_one= _temp617 + 2; _temp618;}))); goto _LL341; _LL373: s= Cyc_Absynpp_cat3(
Cyc_PP_text(({ char* _temp619="union {"; struct _tagged_string _temp620;
_temp620.curr= _temp619; _temp620.base= _temp619; _temp620.last_plus_one=
_temp619 + 8; _temp620;})), Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp438)), Cyc_PP_text(({ char* _temp621="}"; struct _tagged_string _temp622;
_temp622.curr= _temp621; _temp622.base= _temp621; _temp622.last_plus_one=
_temp621 + 2; _temp622;}))); goto _LL341; _LL375: s= Cyc_Absynpp_cat2( Cyc_PP_text(({
char* _temp623="enum "; struct _tagged_string _temp624; _temp624.curr= _temp623;
_temp624.base= _temp623; _temp624.last_plus_one= _temp623 + 6; _temp624;})), Cyc_Absynpp_qvar2doc(
_temp442)); goto _LL341; _LL377: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp448), Cyc_Absynpp_tps2doc( _temp446)); goto _LL341; _LL379: s= Cyc_Absynpp_cat3(
Cyc_PP_text(({ char* _temp625="region_t<"; struct _tagged_string _temp626;
_temp626.curr= _temp625; _temp626.base= _temp625; _temp626.last_plus_one=
_temp625 + 10; _temp626;})), Cyc_Absynpp_rgn2doc( _temp450), Cyc_PP_text(({ char*
_temp627=">"; struct _tagged_string _temp628; _temp628.curr= _temp627; _temp628.base=
_temp627; _temp628.last_plus_one= _temp627 + 2; _temp628;}))); goto _LL341;
_LL381: s= Cyc_Absynpp_rgn2doc( t); goto _LL341; _LL383: s= Cyc_Absynpp_eff2doc(
t); goto _LL341; _LL385: s= Cyc_Absynpp_eff2doc( t); goto _LL341; _LL341:;}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct Cyc_Core_Opt* vo){
return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( vo))->v));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc(
struct _tuple1* t){ struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt*
_temp629=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp629->v=( void*) Cyc_PP_text(*(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null((* t).f1))->v)); _temp629;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(*
t).f3, dopt);} struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc( struct Cyc_List_List*
args, int varargs, struct Cyc_Core_Opt* effopt){ if( effopt == 0){ if( varargs){
return Cyc_Absynpp_group(({ char* _temp630="("; struct _tagged_string _temp631;
_temp631.curr= _temp630; _temp631.base= _temp630; _temp631.last_plus_one=
_temp630 + 2; _temp631;}),({ char* _temp632=")"; struct _tagged_string _temp633;
_temp633.curr= _temp632; _temp633.base= _temp632; _temp633.last_plus_one=
_temp632 + 2; _temp633;}),({ char* _temp634=","; struct _tagged_string _temp635;
_temp635.curr= _temp634; _temp635.base= _temp634; _temp635.last_plus_one=
_temp634 + 2; _temp635;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)((( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args),({ struct Cyc_List_List* _temp636=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp636->hd=( void*) Cyc_PP_text(({ char*
_temp637="..."; struct _tagged_string _temp638; _temp638.curr= _temp637;
_temp638.base= _temp637; _temp638.last_plus_one= _temp637 + 4; _temp638;}));
_temp636->tl= 0; _temp636;})));} else{ return Cyc_Absynpp_group(({ char*
_temp639="("; struct _tagged_string _temp640; _temp640.curr= _temp639; _temp640.base=
_temp639; _temp640.last_plus_one= _temp639 + 2; _temp640;}),({ char* _temp641=")";
struct _tagged_string _temp642; _temp642.curr= _temp641; _temp642.base= _temp641;
_temp642.last_plus_one= _temp641 + 2; _temp642;}),({ char* _temp643=","; struct
_tagged_string _temp644; _temp644.curr= _temp643; _temp644.base= _temp643;
_temp644.last_plus_one= _temp643 + 2; _temp644;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_funarg2doc, args));}} else{ if( varargs){ return Cyc_Absynpp_group(({
char* _temp645="("; struct _tagged_string _temp646; _temp646.curr= _temp645;
_temp646.base= _temp645; _temp646.last_plus_one= _temp645 + 2; _temp646;}),({
char* _temp647=")"; struct _tagged_string _temp648; _temp648.curr= _temp647;
_temp648.base= _temp647; _temp648.last_plus_one= _temp647 + 2; _temp648;}),({
char* _temp649=";"; struct _tagged_string _temp650; _temp650.curr= _temp649;
_temp650.base= _temp649; _temp650.last_plus_one= _temp649 + 2; _temp650;}),({
struct Cyc_List_List* _temp651=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp651->hd=( void*) Cyc_Absynpp_group(({ char*
_temp653=""; struct _tagged_string _temp654; _temp654.curr= _temp653; _temp654.base=
_temp653; _temp654.last_plus_one= _temp653 + 1; _temp654;}),({ char* _temp655="";
struct _tagged_string _temp656; _temp656.curr= _temp655; _temp656.base= _temp655;
_temp656.last_plus_one= _temp655 + 1; _temp656;}),({ char* _temp657=","; struct
_tagged_string _temp658; _temp658.curr= _temp657; _temp658.base= _temp657;
_temp658.last_plus_one= _temp657 + 2; _temp658;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args),({ struct Cyc_List_List*
_temp659=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp659->hd=( void*) Cyc_PP_text(({ char* _temp660="..."; struct _tagged_string
_temp661; _temp661.curr= _temp660; _temp661.base= _temp660; _temp661.last_plus_one=
_temp660 + 4; _temp661;})); _temp659->tl= 0; _temp659;}))); _temp651->tl=({
struct Cyc_List_List* _temp652=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp652->hd=( void*) Cyc_Absynpp_eff2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( effopt))->v); _temp652->tl= 0; _temp652;});
_temp651;}));} else{ return Cyc_Absynpp_group(({ char* _temp662="("; struct
_tagged_string _temp663; _temp663.curr= _temp662; _temp663.base= _temp662;
_temp663.last_plus_one= _temp662 + 2; _temp663;}),({ char* _temp664=")"; struct
_tagged_string _temp665; _temp665.curr= _temp664; _temp665.base= _temp664;
_temp665.last_plus_one= _temp664 + 2; _temp665;}),({ char* _temp666=";"; struct
_tagged_string _temp667; _temp667.curr= _temp666; _temp667.base= _temp666;
_temp667.last_plus_one= _temp666 + 2; _temp667;}),({ struct Cyc_List_List*
_temp668=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp668->hd=( void*) Cyc_Absynpp_group(({ char* _temp670=""; struct
_tagged_string _temp671; _temp671.curr= _temp670; _temp671.base= _temp670;
_temp671.last_plus_one= _temp670 + 1; _temp671;}),({ char* _temp672=""; struct
_tagged_string _temp673; _temp673.curr= _temp672; _temp673.base= _temp672;
_temp673.last_plus_one= _temp672 + 1; _temp673;}),({ char* _temp674=","; struct
_tagged_string _temp675; _temp675.curr= _temp674; _temp675.base= _temp674;
_temp675.last_plus_one= _temp674 + 2; _temp675;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_funarg2doc, args)); _temp668->tl=({ struct Cyc_List_List* _temp669=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp669->hd=(
void*) Cyc_Absynpp_eff2doc(( void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v);
_temp669->tl= 0; _temp669;}); _temp668;}));}}} struct _tuple1* Cyc_Absynpp_arg_mk_opt(
struct _tuple3* arg){ return({ struct _tuple1* _temp676=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp676->f1=({ struct Cyc_Core_Opt*
_temp677=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp677->v=( void*)(* arg).f1; _temp677;}); _temp676->f2=(* arg).f2; _temp676->f3=(*
arg).f3; _temp676;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_string* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* _temp678= 0; goto _LL679; _LL679:{
void* _temp680=(* q).f1; struct Cyc_List_List* _temp692; struct Cyc_List_List*
_temp694; struct Cyc_List_List* _temp696; struct Cyc_List_List* _temp698; _LL682:
if( _temp680 ==( void*) Cyc_Absyn_Loc_n){ goto _LL683;} else{ goto _LL684;}
_LL684: if(( unsigned int) _temp680 > 1u?*(( int*) _temp680) == Cyc_Absyn_Rel_n:
0){ _LL693: _temp692=(( struct Cyc_Absyn_Rel_n_struct*) _temp680)->f1; if(
_temp692 == 0){ goto _LL685;} else{ goto _LL686;}} else{ goto _LL686;} _LL686:
if(( unsigned int) _temp680 > 1u?*(( int*) _temp680) == Cyc_Absyn_Abs_n: 0){
_LL695: _temp694=(( struct Cyc_Absyn_Abs_n_struct*) _temp680)->f1; if( _temp694
== 0){ goto _LL687;} else{ goto _LL688;}} else{ goto _LL688;} _LL688: if((
unsigned int) _temp680 > 1u?*(( int*) _temp680) == Cyc_Absyn_Rel_n: 0){ _LL697:
_temp696=(( struct Cyc_Absyn_Rel_n_struct*) _temp680)->f1; goto _LL689;} else{
goto _LL690;} _LL690: if(( unsigned int) _temp680 > 1u?*(( int*) _temp680) ==
Cyc_Absyn_Abs_n: 0){ _LL699: _temp698=(( struct Cyc_Absyn_Abs_n_struct*)
_temp680)->f1; goto _LL691;} else{ goto _LL681;} _LL683: goto _LL685; _LL685:
goto _LL687; _LL687: return Cyc_Absynpp_var2doc((* q).f2); _LL689: _temp698=
_temp696; goto _LL691; _LL691: _temp678= _temp698; goto _LL681; _LL681:;} if(
Cyc_Absynpp_qvar_to_Cids){ return Cyc_PP_text( Cyc_String_str_sepstr((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp678,({ struct Cyc_List_List* _temp700=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp700->hd=( void*)(* q).f2; _temp700->tl= 0;
_temp700;})),({ char* _temp701="_"; struct _tagged_string _temp702; _temp702.curr=
_temp701; _temp702.base= _temp701; _temp702.last_plus_one= _temp701 + 2;
_temp702;})));} else{ if((( int(*)( int(* cmp)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_prefix)(
Cyc_String_strptrcmp, _temp678, Cyc_Absynpp_curr_namespace)){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp678,({
struct Cyc_List_List* _temp703=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp703->hd=( void*)(* q).f2; _temp703->tl= 0; _temp703;})),({
char* _temp704="::"; struct _tagged_string _temp705; _temp705.curr= _temp704;
_temp705.base= _temp704; _temp705.last_plus_one= _temp704 + 3; _temp705;})));}}}
struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);}{ void* _temp706=(* v).f1; struct Cyc_List_List*
_temp712; _LL708: if(( unsigned int) _temp706 > 1u?*(( int*) _temp706) == Cyc_Absyn_Abs_n:
0){ _LL713: _temp712=(( struct Cyc_Absyn_Abs_n_struct*) _temp706)->f1; goto
_LL709;} else{ goto _LL710;} _LL710: goto _LL711; _LL709: if((( int(*)( int(*
cmp)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp712,
Cyc_Absynpp_curr_namespace) == 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{
return Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp714="/* wrong namespace : */ ";
struct _tagged_string _temp715; _temp715.curr= _temp714; _temp715.base= _temp714;
_temp715.last_plus_one= _temp714 + 25; _temp715;})), Cyc_Absynpp_qvar2doc( v));}
_LL711: return Cyc_Absynpp_qvar2doc( v); _LL707:;}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc(
void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp* e){ void* _temp716=( void*) e->r; void* _temp788; void*
_temp790; struct _tuple0* _temp792; struct _tuple0* _temp794; struct Cyc_List_List*
_temp796; void* _temp798; struct Cyc_Absyn_Exp* _temp800; struct Cyc_Core_Opt*
_temp802; struct Cyc_Absyn_Exp* _temp804; void* _temp806; struct Cyc_Absyn_Exp*
_temp808; struct Cyc_Absyn_Exp* _temp810; struct Cyc_Absyn_Exp* _temp812; struct
Cyc_Absyn_Exp* _temp814; struct Cyc_Absyn_Exp* _temp816; struct Cyc_Absyn_Exp*
_temp818; struct Cyc_List_List* _temp820; struct Cyc_Absyn_Exp* _temp822; struct
Cyc_List_List* _temp824; struct Cyc_Absyn_Exp* _temp826; struct Cyc_Absyn_Exp*
_temp828; struct Cyc_Absyn_Exp* _temp830; struct Cyc_List_List* _temp832; struct
Cyc_Absyn_Exp* _temp834; struct Cyc_Absyn_Exp* _temp836; void* _temp838; struct
Cyc_Absyn_Exp* _temp840; struct Cyc_Absyn_Exp* _temp842; struct Cyc_Absyn_Exp*
_temp844; void* _temp846; struct Cyc_Absyn_Exp* _temp848; struct Cyc_Absyn_Exp*
_temp850; struct _tagged_string* _temp852; struct Cyc_Absyn_Exp* _temp854;
struct _tagged_string* _temp856; struct Cyc_Absyn_Exp* _temp858; struct Cyc_Absyn_Exp*
_temp860; struct Cyc_Absyn_Exp* _temp862; struct Cyc_List_List* _temp864; struct
Cyc_List_List* _temp866; struct _tuple1* _temp868; struct Cyc_List_List*
_temp870; struct Cyc_Absyn_Exp* _temp872; struct Cyc_Absyn_Exp* _temp874; struct
Cyc_Absyn_Vardecl* _temp876; struct Cyc_Absyn_Structdecl* _temp878; struct Cyc_List_List*
_temp880; struct Cyc_Core_Opt* _temp882; struct _tuple0* _temp884; struct Cyc_List_List*
_temp886; void* _temp888; struct Cyc_Absyn_Tunionfield* _temp890; struct Cyc_Absyn_Tuniondecl*
_temp892; struct Cyc_List_List* _temp894; struct Cyc_Core_Opt* _temp896; struct
Cyc_Core_Opt* _temp898; struct Cyc_Absyn_Enumfield* _temp900; struct Cyc_Absyn_Enumdecl*
_temp902; struct _tuple0* _temp904; void* _temp906; struct Cyc_Absyn_Exp*
_temp908; struct Cyc_List_List* _temp910; struct Cyc_Core_Opt* _temp912; struct
Cyc_Absyn_Stmt* _temp914; struct Cyc_Absyn_Fndecl* _temp916; struct Cyc_Absyn_Exp*
_temp918; _LL718: if(*(( int*) _temp716) == Cyc_Absyn_Const_e){ _LL789: _temp788=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp716)->f1; goto _LL719;} else{
goto _LL720;} _LL720: if(*(( int*) _temp716) == Cyc_Absyn_Var_e){ _LL793:
_temp792=(( struct Cyc_Absyn_Var_e_struct*) _temp716)->f1; goto _LL791; _LL791:
_temp790=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp716)->f2; goto _LL721;}
else{ goto _LL722;} _LL722: if(*(( int*) _temp716) == Cyc_Absyn_UnknownId_e){
_LL795: _temp794=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp716)->f1; goto
_LL723;} else{ goto _LL724;} _LL724: if(*(( int*) _temp716) == Cyc_Absyn_Primop_e){
_LL799: _temp798=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp716)->f1;
goto _LL797; _LL797: _temp796=(( struct Cyc_Absyn_Primop_e_struct*) _temp716)->f2;
goto _LL725;} else{ goto _LL726;} _LL726: if(*(( int*) _temp716) == Cyc_Absyn_AssignOp_e){
_LL805: _temp804=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp716)->f1; goto
_LL803; _LL803: _temp802=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp716)->f2;
goto _LL801; _LL801: _temp800=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp716)->f3;
goto _LL727;} else{ goto _LL728;} _LL728: if(*(( int*) _temp716) == Cyc_Absyn_Increment_e){
_LL809: _temp808=(( struct Cyc_Absyn_Increment_e_struct*) _temp716)->f1; goto
_LL807; _LL807: _temp806=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp716)->f2; goto _LL729;} else{ goto _LL730;} _LL730: if(*(( int*) _temp716)
== Cyc_Absyn_Conditional_e){ _LL815: _temp814=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp716)->f1; goto _LL813; _LL813: _temp812=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp716)->f2; goto _LL811; _LL811: _temp810=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp716)->f3; goto _LL731;} else{ goto _LL732;} _LL732: if(*(( int*) _temp716)
== Cyc_Absyn_SeqExp_e){ _LL819: _temp818=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp716)->f1; goto _LL817; _LL817: _temp816=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp716)->f2; goto _LL733;} else{ goto _LL734;} _LL734: if(*(( int*) _temp716)
== Cyc_Absyn_UnknownCall_e){ _LL823: _temp822=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp716)->f1; goto _LL821; _LL821: _temp820=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp716)->f2; goto _LL735;} else{ goto _LL736;} _LL736: if(*(( int*) _temp716)
== Cyc_Absyn_FnCall_e){ _LL827: _temp826=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp716)->f1; goto _LL825; _LL825: _temp824=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp716)->f2; goto _LL737;} else{ goto _LL738;} _LL738: if(*(( int*) _temp716)
== Cyc_Absyn_Throw_e){ _LL829: _temp828=(( struct Cyc_Absyn_Throw_e_struct*)
_temp716)->f1; goto _LL739;} else{ goto _LL740;} _LL740: if(*(( int*) _temp716)
== Cyc_Absyn_NoInstantiate_e){ _LL831: _temp830=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp716)->f1; goto _LL741;} else{ goto _LL742;} _LL742: if(*(( int*) _temp716)
== Cyc_Absyn_Instantiate_e){ _LL835: _temp834=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp716)->f1; goto _LL833; _LL833: _temp832=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp716)->f2; goto _LL743;} else{ goto _LL744;} _LL744: if(*(( int*) _temp716)
== Cyc_Absyn_Cast_e){ _LL839: _temp838=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp716)->f1; goto _LL837; _LL837: _temp836=(( struct Cyc_Absyn_Cast_e_struct*)
_temp716)->f2; goto _LL745;} else{ goto _LL746;} _LL746: if(*(( int*) _temp716)
== Cyc_Absyn_New_e){ _LL843: _temp842=(( struct Cyc_Absyn_New_e_struct*)
_temp716)->f1; goto _LL841; _LL841: _temp840=(( struct Cyc_Absyn_New_e_struct*)
_temp716)->f2; goto _LL747;} else{ goto _LL748;} _LL748: if(*(( int*) _temp716)
== Cyc_Absyn_Address_e){ _LL845: _temp844=(( struct Cyc_Absyn_Address_e_struct*)
_temp716)->f1; goto _LL749;} else{ goto _LL750;} _LL750: if(*(( int*) _temp716)
== Cyc_Absyn_Sizeoftyp_e){ _LL847: _temp846=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp716)->f1; goto _LL751;} else{ goto _LL752;} _LL752: if(*(( int*) _temp716)
== Cyc_Absyn_Sizeofexp_e){ _LL849: _temp848=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp716)->f1; goto _LL753;} else{ goto _LL754;} _LL754: if(*(( int*) _temp716)
== Cyc_Absyn_Deref_e){ _LL851: _temp850=(( struct Cyc_Absyn_Deref_e_struct*)
_temp716)->f1; goto _LL755;} else{ goto _LL756;} _LL756: if(*(( int*) _temp716)
== Cyc_Absyn_StructMember_e){ _LL855: _temp854=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp716)->f1; goto _LL853; _LL853: _temp852=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp716)->f2; goto _LL757;} else{ goto _LL758;} _LL758: if(*(( int*) _temp716)
== Cyc_Absyn_StructArrow_e){ _LL859: _temp858=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp716)->f1; goto _LL857; _LL857: _temp856=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp716)->f2; goto _LL759;} else{ goto _LL760;} _LL760: if(*(( int*) _temp716)
== Cyc_Absyn_Subscript_e){ _LL863: _temp862=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp716)->f1; goto _LL861; _LL861: _temp860=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp716)->f2; goto _LL761;} else{ goto _LL762;} _LL762: if(*(( int*) _temp716)
== Cyc_Absyn_Tuple_e){ _LL865: _temp864=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp716)->f1; goto _LL763;} else{ goto _LL764;} _LL764: if(*(( int*) _temp716)
== Cyc_Absyn_CompoundLit_e){ _LL869: _temp868=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp716)->f1; goto _LL867; _LL867: _temp866=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp716)->f2; goto _LL765;} else{ goto _LL766;} _LL766: if(*(( int*) _temp716)
== Cyc_Absyn_Array_e){ _LL871: _temp870=(( struct Cyc_Absyn_Array_e_struct*)
_temp716)->f1; goto _LL767;} else{ goto _LL768;} _LL768: if(*(( int*) _temp716)
== Cyc_Absyn_Comprehension_e){ _LL877: _temp876=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp716)->f1; goto _LL875; _LL875: _temp874=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp716)->f2; goto _LL873; _LL873: _temp872=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp716)->f3; goto _LL769;} else{ goto _LL770;} _LL770: if(*(( int*) _temp716)
== Cyc_Absyn_Struct_e){ _LL885: _temp884=(( struct Cyc_Absyn_Struct_e_struct*)
_temp716)->f1; goto _LL883; _LL883: _temp882=(( struct Cyc_Absyn_Struct_e_struct*)
_temp716)->f2; goto _LL881; _LL881: _temp880=(( struct Cyc_Absyn_Struct_e_struct*)
_temp716)->f3; goto _LL879; _LL879: _temp878=(( struct Cyc_Absyn_Struct_e_struct*)
_temp716)->f4; goto _LL771;} else{ goto _LL772;} _LL772: if(*(( int*) _temp716)
== Cyc_Absyn_AnonStruct_e){ _LL889: _temp888=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp716)->f1; goto _LL887; _LL887: _temp886=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp716)->f2; goto _LL773;} else{ goto _LL774;} _LL774: if(*(( int*) _temp716)
== Cyc_Absyn_Tunion_e){ _LL899: _temp898=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp716)->f1; goto _LL897; _LL897: _temp896=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp716)->f2; goto _LL895; _LL895: _temp894=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp716)->f3; goto _LL893; _LL893: _temp892=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp716)->f4; goto _LL891; _LL891: _temp890=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp716)->f5; goto _LL775;} else{ goto _LL776;} _LL776: if(*(( int*) _temp716)
== Cyc_Absyn_Enum_e){ _LL905: _temp904=(( struct Cyc_Absyn_Enum_e_struct*)
_temp716)->f1; goto _LL903; _LL903: _temp902=(( struct Cyc_Absyn_Enum_e_struct*)
_temp716)->f2; goto _LL901; _LL901: _temp900=(( struct Cyc_Absyn_Enum_e_struct*)
_temp716)->f3; goto _LL777;} else{ goto _LL778;} _LL778: if(*(( int*) _temp716)
== Cyc_Absyn_Malloc_e){ _LL909: _temp908=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp716)->f1; goto _LL907; _LL907: _temp906=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp716)->f2; goto _LL779;} else{ goto _LL780;} _LL780: if(*(( int*) _temp716)
== Cyc_Absyn_UnresolvedMem_e){ _LL913: _temp912=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp716)->f1; goto _LL911; _LL911: _temp910=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp716)->f2; goto _LL781;} else{ goto _LL782;} _LL782: if(*(( int*) _temp716)
== Cyc_Absyn_StmtExp_e){ _LL915: _temp914=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp716)->f1; goto _LL783;} else{ goto _LL784;} _LL784: if(*(( int*) _temp716)
== Cyc_Absyn_Codegen_e){ _LL917: _temp916=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp716)->f1; goto _LL785;} else{ goto _LL786;} _LL786: if(*(( int*) _temp716)
== Cyc_Absyn_Fill_e){ _LL919: _temp918=(( struct Cyc_Absyn_Fill_e_struct*)
_temp716)->f1; goto _LL787;} else{ goto _LL717;} _LL719: return 10000; _LL721:
return 10000; _LL723: return 10000; _LL725: { void* _temp920= _temp798; _LL922:
if( _temp920 ==( void*) Cyc_Absyn_Plus){ goto _LL923;} else{ goto _LL924;}
_LL924: if( _temp920 ==( void*) Cyc_Absyn_Times){ goto _LL925;} else{ goto
_LL926;} _LL926: if( _temp920 ==( void*) Cyc_Absyn_Minus){ goto _LL927;} else{
goto _LL928;} _LL928: if( _temp920 ==( void*) Cyc_Absyn_Div){ goto _LL929;}
else{ goto _LL930;} _LL930: if( _temp920 ==( void*) Cyc_Absyn_Mod){ goto _LL931;}
else{ goto _LL932;} _LL932: if( _temp920 ==( void*) Cyc_Absyn_Eq){ goto _LL933;}
else{ goto _LL934;} _LL934: if( _temp920 ==( void*) Cyc_Absyn_Neq){ goto _LL935;}
else{ goto _LL936;} _LL936: if( _temp920 ==( void*) Cyc_Absyn_Gt){ goto _LL937;}
else{ goto _LL938;} _LL938: if( _temp920 ==( void*) Cyc_Absyn_Lt){ goto _LL939;}
else{ goto _LL940;} _LL940: if( _temp920 ==( void*) Cyc_Absyn_Gte){ goto _LL941;}
else{ goto _LL942;} _LL942: if( _temp920 ==( void*) Cyc_Absyn_Lte){ goto _LL943;}
else{ goto _LL944;} _LL944: if( _temp920 ==( void*) Cyc_Absyn_Not){ goto _LL945;}
else{ goto _LL946;} _LL946: if( _temp920 ==( void*) Cyc_Absyn_Bitnot){ goto
_LL947;} else{ goto _LL948;} _LL948: if( _temp920 ==( void*) Cyc_Absyn_Bitand){
goto _LL949;} else{ goto _LL950;} _LL950: if( _temp920 ==( void*) Cyc_Absyn_Bitor){
goto _LL951;} else{ goto _LL952;} _LL952: if( _temp920 ==( void*) Cyc_Absyn_Bitxor){
goto _LL953;} else{ goto _LL954;} _LL954: if( _temp920 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL955;} else{ goto _LL956;} _LL956: if( _temp920 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL957;} else{ goto _LL958;} _LL958: if( _temp920 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL959;} else{ goto _LL960;} _LL960: if( _temp920 ==( void*) Cyc_Absyn_Size){
goto _LL961;} else{ goto _LL962;} _LL962: if( _temp920 ==( void*) Cyc_Absyn_Printf){
goto _LL963;} else{ goto _LL964;} _LL964: if( _temp920 ==( void*) Cyc_Absyn_Fprintf){
goto _LL965;} else{ goto _LL966;} _LL966: if( _temp920 ==( void*) Cyc_Absyn_Xprintf){
goto _LL967;} else{ goto _LL968;} _LL968: if( _temp920 ==( void*) Cyc_Absyn_Scanf){
goto _LL969;} else{ goto _LL970;} _LL970: if( _temp920 ==( void*) Cyc_Absyn_Fscanf){
goto _LL971;} else{ goto _LL972;} _LL972: if( _temp920 ==( void*) Cyc_Absyn_Sscanf){
goto _LL973;} else{ goto _LL921;} _LL923: return 100; _LL925: return 110; _LL927:
return 100; _LL929: return 110; _LL931: return 110; _LL933: return 70; _LL935:
return 70; _LL937: return 80; _LL939: return 80; _LL941: return 80; _LL943:
return 80; _LL945: return 130; _LL947: return 130; _LL949: return 60; _LL951:
return 40; _LL953: return 50; _LL955: return 90; _LL957: return 80; _LL959:
return 80; _LL961: return 140; _LL963: return 130; _LL965: return 130; _LL967:
return 130; _LL969: return 130; _LL971: return 130; _LL973: return 130; _LL921:;}
_LL727: return 20; _LL729: return 130; _LL731: return 30; _LL733: return 10;
_LL735: return 140; _LL737: return 140; _LL739: return 130; _LL741: return Cyc_Absynpp_exp_prec(
_temp830); _LL743: return Cyc_Absynpp_exp_prec( _temp834); _LL745: return 120;
_LL747: return 15; _LL749: return 130; _LL751: return 130; _LL753: return 130;
_LL755: return 130; _LL757: return 140; _LL759: return 140; _LL761: return 140;
_LL763: return 150; _LL765: goto _LL767; _LL767: goto _LL769; _LL769: goto
_LL771; _LL771: goto _LL773; _LL773: goto _LL775; _LL775: goto _LL777; _LL777:
goto _LL779; _LL779: return 140; _LL781: return 140; _LL783: return 10000;
_LL785: return 140; _LL787: return 140; _LL717:;} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec( 0, e);} struct
_tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e);
struct Cyc_PP_Doc* s;{ void* _temp974=( void*) e->r; void* _temp1046; void*
_temp1048; struct _tuple0* _temp1050; struct _tuple0* _temp1052; struct Cyc_List_List*
_temp1054; void* _temp1056; struct Cyc_Absyn_Exp* _temp1058; struct Cyc_Core_Opt*
_temp1060; struct Cyc_Absyn_Exp* _temp1062; void* _temp1064; struct Cyc_Absyn_Exp*
_temp1066; struct Cyc_Absyn_Exp* _temp1068; struct Cyc_Absyn_Exp* _temp1070;
struct Cyc_Absyn_Exp* _temp1072; struct Cyc_Absyn_Exp* _temp1074; struct Cyc_Absyn_Exp*
_temp1076; struct Cyc_List_List* _temp1078; struct Cyc_Absyn_Exp* _temp1080;
struct Cyc_List_List* _temp1082; struct Cyc_Absyn_Exp* _temp1084; struct Cyc_Absyn_Exp*
_temp1086; struct Cyc_Absyn_Exp* _temp1088; struct Cyc_List_List* _temp1090;
struct Cyc_Absyn_Exp* _temp1092; struct Cyc_Absyn_Exp* _temp1094; void*
_temp1096; struct Cyc_Absyn_Exp* _temp1098; struct Cyc_Absyn_Exp* _temp1100;
struct Cyc_Absyn_Exp* _temp1102; void* _temp1104; struct Cyc_Absyn_Exp*
_temp1106; struct Cyc_Absyn_Exp* _temp1108; struct _tagged_string* _temp1110;
struct Cyc_Absyn_Exp* _temp1112; struct _tagged_string* _temp1114; struct Cyc_Absyn_Exp*
_temp1116; struct Cyc_Absyn_Exp* _temp1118; struct Cyc_Absyn_Exp* _temp1120;
struct Cyc_List_List* _temp1122; struct Cyc_List_List* _temp1124; struct _tuple1*
_temp1126; struct Cyc_List_List* _temp1128; struct Cyc_Absyn_Exp* _temp1130;
struct Cyc_Absyn_Exp* _temp1132; struct Cyc_Absyn_Vardecl* _temp1134; struct Cyc_Absyn_Structdecl*
_temp1136; struct Cyc_List_List* _temp1138; struct Cyc_Core_Opt* _temp1140;
struct _tuple0* _temp1142; struct Cyc_List_List* _temp1144; void* _temp1146;
struct Cyc_Absyn_Tunionfield* _temp1148; struct Cyc_Absyn_Tuniondecl* _temp1150;
struct Cyc_List_List* _temp1152; struct Cyc_Core_Opt* _temp1154; struct Cyc_Core_Opt*
_temp1156; struct Cyc_Absyn_Enumfield* _temp1158; struct Cyc_Absyn_Enumdecl*
_temp1160; struct _tuple0* _temp1162; void* _temp1164; struct Cyc_Absyn_Exp*
_temp1166; struct Cyc_List_List* _temp1168; struct Cyc_Core_Opt* _temp1170;
struct Cyc_Absyn_Stmt* _temp1172; struct Cyc_Absyn_Fndecl* _temp1174; struct Cyc_Absyn_Exp*
_temp1176; _LL976: if(*(( int*) _temp974) == Cyc_Absyn_Const_e){ _LL1047:
_temp1046=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp974)->f1; goto
_LL977;} else{ goto _LL978;} _LL978: if(*(( int*) _temp974) == Cyc_Absyn_Var_e){
_LL1051: _temp1050=(( struct Cyc_Absyn_Var_e_struct*) _temp974)->f1; goto
_LL1049; _LL1049: _temp1048=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp974)->f2;
goto _LL979;} else{ goto _LL980;} _LL980: if(*(( int*) _temp974) == Cyc_Absyn_UnknownId_e){
_LL1053: _temp1052=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp974)->f1; goto
_LL981;} else{ goto _LL982;} _LL982: if(*(( int*) _temp974) == Cyc_Absyn_Primop_e){
_LL1057: _temp1056=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp974)->f1;
goto _LL1055; _LL1055: _temp1054=(( struct Cyc_Absyn_Primop_e_struct*) _temp974)->f2;
goto _LL983;} else{ goto _LL984;} _LL984: if(*(( int*) _temp974) == Cyc_Absyn_AssignOp_e){
_LL1063: _temp1062=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp974)->f1; goto
_LL1061; _LL1061: _temp1060=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp974)->f2;
goto _LL1059; _LL1059: _temp1058=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp974)->f3; goto _LL985;} else{ goto _LL986;} _LL986: if(*(( int*) _temp974)
== Cyc_Absyn_Increment_e){ _LL1067: _temp1066=(( struct Cyc_Absyn_Increment_e_struct*)
_temp974)->f1; goto _LL1065; _LL1065: _temp1064=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp974)->f2; goto _LL987;} else{ goto _LL988;} _LL988: if(*(( int*) _temp974)
== Cyc_Absyn_Conditional_e){ _LL1073: _temp1072=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp974)->f1; goto _LL1071; _LL1071: _temp1070=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp974)->f2; goto _LL1069; _LL1069: _temp1068=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp974)->f3; goto _LL989;} else{ goto _LL990;} _LL990: if(*(( int*) _temp974)
== Cyc_Absyn_SeqExp_e){ _LL1077: _temp1076=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp974)->f1; goto _LL1075; _LL1075: _temp1074=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp974)->f2; goto _LL991;} else{ goto _LL992;} _LL992: if(*(( int*) _temp974)
== Cyc_Absyn_UnknownCall_e){ _LL1081: _temp1080=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp974)->f1; goto _LL1079; _LL1079: _temp1078=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp974)->f2; goto _LL993;} else{ goto _LL994;} _LL994: if(*(( int*) _temp974)
== Cyc_Absyn_FnCall_e){ _LL1085: _temp1084=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp974)->f1; goto _LL1083; _LL1083: _temp1082=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp974)->f2; goto _LL995;} else{ goto _LL996;} _LL996: if(*(( int*) _temp974)
== Cyc_Absyn_Throw_e){ _LL1087: _temp1086=(( struct Cyc_Absyn_Throw_e_struct*)
_temp974)->f1; goto _LL997;} else{ goto _LL998;} _LL998: if(*(( int*) _temp974)
== Cyc_Absyn_NoInstantiate_e){ _LL1089: _temp1088=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp974)->f1; goto _LL999;} else{ goto _LL1000;} _LL1000: if(*(( int*) _temp974)
== Cyc_Absyn_Instantiate_e){ _LL1093: _temp1092=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp974)->f1; goto _LL1091; _LL1091: _temp1090=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp974)->f2; goto _LL1001;} else{ goto _LL1002;} _LL1002: if(*(( int*)
_temp974) == Cyc_Absyn_Cast_e){ _LL1097: _temp1096=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp974)->f1; goto _LL1095; _LL1095: _temp1094=(( struct Cyc_Absyn_Cast_e_struct*)
_temp974)->f2; goto _LL1003;} else{ goto _LL1004;} _LL1004: if(*(( int*)
_temp974) == Cyc_Absyn_Address_e){ _LL1099: _temp1098=(( struct Cyc_Absyn_Address_e_struct*)
_temp974)->f1; goto _LL1005;} else{ goto _LL1006;} _LL1006: if(*(( int*)
_temp974) == Cyc_Absyn_New_e){ _LL1103: _temp1102=(( struct Cyc_Absyn_New_e_struct*)
_temp974)->f1; goto _LL1101; _LL1101: _temp1100=(( struct Cyc_Absyn_New_e_struct*)
_temp974)->f2; goto _LL1007;} else{ goto _LL1008;} _LL1008: if(*(( int*)
_temp974) == Cyc_Absyn_Sizeoftyp_e){ _LL1105: _temp1104=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp974)->f1; goto _LL1009;} else{ goto _LL1010;} _LL1010: if(*(( int*)
_temp974) == Cyc_Absyn_Sizeofexp_e){ _LL1107: _temp1106=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp974)->f1; goto _LL1011;} else{ goto _LL1012;} _LL1012: if(*(( int*)
_temp974) == Cyc_Absyn_Deref_e){ _LL1109: _temp1108=(( struct Cyc_Absyn_Deref_e_struct*)
_temp974)->f1; goto _LL1013;} else{ goto _LL1014;} _LL1014: if(*(( int*)
_temp974) == Cyc_Absyn_StructMember_e){ _LL1113: _temp1112=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp974)->f1; goto _LL1111; _LL1111: _temp1110=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp974)->f2; goto _LL1015;} else{ goto _LL1016;} _LL1016: if(*(( int*)
_temp974) == Cyc_Absyn_StructArrow_e){ _LL1117: _temp1116=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp974)->f1; goto _LL1115; _LL1115: _temp1114=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp974)->f2; goto _LL1017;} else{ goto _LL1018;} _LL1018: if(*(( int*)
_temp974) == Cyc_Absyn_Subscript_e){ _LL1121: _temp1120=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp974)->f1; goto _LL1119; _LL1119: _temp1118=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp974)->f2; goto _LL1019;} else{ goto _LL1020;} _LL1020: if(*(( int*)
_temp974) == Cyc_Absyn_Tuple_e){ _LL1123: _temp1122=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp974)->f1; goto _LL1021;} else{ goto _LL1022;} _LL1022: if(*(( int*)
_temp974) == Cyc_Absyn_CompoundLit_e){ _LL1127: _temp1126=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp974)->f1; goto _LL1125; _LL1125: _temp1124=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp974)->f2; goto _LL1023;} else{ goto _LL1024;} _LL1024: if(*(( int*)
_temp974) == Cyc_Absyn_Array_e){ _LL1129: _temp1128=(( struct Cyc_Absyn_Array_e_struct*)
_temp974)->f1; goto _LL1025;} else{ goto _LL1026;} _LL1026: if(*(( int*)
_temp974) == Cyc_Absyn_Comprehension_e){ _LL1135: _temp1134=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp974)->f1; goto _LL1133; _LL1133: _temp1132=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp974)->f2; goto _LL1131; _LL1131: _temp1130=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp974)->f3; goto _LL1027;} else{ goto _LL1028;} _LL1028: if(*(( int*)
_temp974) == Cyc_Absyn_Struct_e){ _LL1143: _temp1142=(( struct Cyc_Absyn_Struct_e_struct*)
_temp974)->f1; goto _LL1141; _LL1141: _temp1140=(( struct Cyc_Absyn_Struct_e_struct*)
_temp974)->f2; goto _LL1139; _LL1139: _temp1138=(( struct Cyc_Absyn_Struct_e_struct*)
_temp974)->f3; goto _LL1137; _LL1137: _temp1136=(( struct Cyc_Absyn_Struct_e_struct*)
_temp974)->f4; goto _LL1029;} else{ goto _LL1030;} _LL1030: if(*(( int*)
_temp974) == Cyc_Absyn_AnonStruct_e){ _LL1147: _temp1146=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp974)->f1; goto _LL1145; _LL1145: _temp1144=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp974)->f2; goto _LL1031;} else{ goto _LL1032;} _LL1032: if(*(( int*)
_temp974) == Cyc_Absyn_Tunion_e){ _LL1157: _temp1156=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp974)->f1; goto _LL1155; _LL1155: _temp1154=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp974)->f2; goto _LL1153; _LL1153: _temp1152=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp974)->f3; goto _LL1151; _LL1151: _temp1150=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp974)->f4; goto _LL1149; _LL1149: _temp1148=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp974)->f5; goto _LL1033;} else{ goto _LL1034;} _LL1034: if(*(( int*)
_temp974) == Cyc_Absyn_Enum_e){ _LL1163: _temp1162=(( struct Cyc_Absyn_Enum_e_struct*)
_temp974)->f1; goto _LL1161; _LL1161: _temp1160=(( struct Cyc_Absyn_Enum_e_struct*)
_temp974)->f2; goto _LL1159; _LL1159: _temp1158=(( struct Cyc_Absyn_Enum_e_struct*)
_temp974)->f3; goto _LL1035;} else{ goto _LL1036;} _LL1036: if(*(( int*)
_temp974) == Cyc_Absyn_Malloc_e){ _LL1167: _temp1166=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp974)->f1; goto _LL1165; _LL1165: _temp1164=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp974)->f2; goto _LL1037;} else{ goto _LL1038;} _LL1038: if(*(( int*)
_temp974) == Cyc_Absyn_UnresolvedMem_e){ _LL1171: _temp1170=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp974)->f1; goto _LL1169; _LL1169: _temp1168=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp974)->f2; goto _LL1039;} else{ goto _LL1040;} _LL1040: if(*(( int*)
_temp974) == Cyc_Absyn_StmtExp_e){ _LL1173: _temp1172=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp974)->f1; goto _LL1041;} else{ goto _LL1042;} _LL1042: if(*(( int*)
_temp974) == Cyc_Absyn_Codegen_e){ _LL1175: _temp1174=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp974)->f1; goto _LL1043;} else{ goto _LL1044;} _LL1044: if(*(( int*)
_temp974) == Cyc_Absyn_Fill_e){ _LL1177: _temp1176=(( struct Cyc_Absyn_Fill_e_struct*)
_temp974)->f1; goto _LL1045;} else{ goto _LL975;} _LL977: s= Cyc_Absynpp_cnst2doc(
_temp1046); goto _LL975; _LL979: _temp1052= _temp1050; goto _LL981; _LL981: s=
Cyc_Absynpp_qvar2doc( _temp1052); goto _LL975; _LL983: s= Cyc_Absynpp_primapp2doc(
myprec, _temp1056, _temp1054); goto _LL975; _LL985: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1062), Cyc_PP_text(({ char* _temp1178=" "; struct _tagged_string
_temp1179; _temp1179.curr= _temp1178; _temp1179.base= _temp1178; _temp1179.last_plus_one=
_temp1178 + 2; _temp1179;})), _temp1060 == 0? Cyc_PP_text(({ char* _temp1180="";
struct _tagged_string _temp1181; _temp1181.curr= _temp1180; _temp1181.base=
_temp1180; _temp1181.last_plus_one= _temp1180 + 1; _temp1181;})): Cyc_Absynpp_prim2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1060))->v), Cyc_PP_text(({ char*
_temp1182="= "; struct _tagged_string _temp1183; _temp1183.curr= _temp1182;
_temp1183.base= _temp1182; _temp1183.last_plus_one= _temp1182 + 3; _temp1183;})),
Cyc_Absynpp_exp2doc_prec( myprec, _temp1058)); goto _LL975; _LL987: { struct Cyc_PP_Doc*
_temp1184= Cyc_Absynpp_exp2doc_prec( myprec, _temp1066); goto _LL1185; _LL1185:{
void* _temp1186= _temp1064; _LL1188: if( _temp1186 ==( void*) Cyc_Absyn_PreInc){
goto _LL1189;} else{ goto _LL1190;} _LL1190: if( _temp1186 ==( void*) Cyc_Absyn_PreDec){
goto _LL1191;} else{ goto _LL1192;} _LL1192: if( _temp1186 ==( void*) Cyc_Absyn_PostInc){
goto _LL1193;} else{ goto _LL1194;} _LL1194: if( _temp1186 ==( void*) Cyc_Absyn_PostDec){
goto _LL1195;} else{ goto _LL1187;} _LL1189: s= Cyc_Absynpp_cat2( Cyc_PP_text(({
char* _temp1196="++"; struct _tagged_string _temp1197; _temp1197.curr= _temp1196;
_temp1197.base= _temp1196; _temp1197.last_plus_one= _temp1196 + 3; _temp1197;})),
_temp1184); goto _LL1187; _LL1191: s= Cyc_Absynpp_cat2( Cyc_PP_text(({ char*
_temp1198="--"; struct _tagged_string _temp1199; _temp1199.curr= _temp1198;
_temp1199.base= _temp1198; _temp1199.last_plus_one= _temp1198 + 3; _temp1199;})),
_temp1184); goto _LL1187; _LL1193: s= Cyc_Absynpp_cat2( _temp1184, Cyc_PP_text(({
char* _temp1200="++"; struct _tagged_string _temp1201; _temp1201.curr= _temp1200;
_temp1201.base= _temp1200; _temp1201.last_plus_one= _temp1200 + 3; _temp1201;})));
goto _LL1187; _LL1195: s= Cyc_Absynpp_cat2( _temp1184, Cyc_PP_text(({ char*
_temp1202="--"; struct _tagged_string _temp1203; _temp1203.curr= _temp1202;
_temp1203.base= _temp1202; _temp1203.last_plus_one= _temp1202 + 3; _temp1203;})));
goto _LL1187; _LL1187:;} goto _LL975;} _LL989:{ struct _tuple8 _temp1205=({
struct _tuple8 _temp1204; _temp1204.f1=( void*) _temp1070->r; _temp1204.f2=(
void*) _temp1068->r; _temp1204;}); _LL1207: goto _LL1208; _LL1208: s= Cyc_Absynpp_cat5(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1072), Cyc_PP_text(({ char* _temp1209=" ? ";
struct _tagged_string _temp1210; _temp1210.curr= _temp1209; _temp1210.base=
_temp1209; _temp1210.last_plus_one= _temp1209 + 4; _temp1210;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1070), Cyc_PP_text(({ char* _temp1211=" : "; struct _tagged_string
_temp1212; _temp1212.curr= _temp1211; _temp1212.base= _temp1211; _temp1212.last_plus_one=
_temp1211 + 4; _temp1212;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1068));
goto _LL1206; _LL1206:;} goto _LL975; _LL991: s= Cyc_Absynpp_cat5( Cyc_PP_text(({
char* _temp1213="("; struct _tagged_string _temp1214; _temp1214.curr= _temp1213;
_temp1214.base= _temp1213; _temp1214.last_plus_one= _temp1213 + 2; _temp1214;})),
Cyc_Absynpp_exp2doc( _temp1076), Cyc_PP_text(({ char* _temp1215=", "; struct
_tagged_string _temp1216; _temp1216.curr= _temp1215; _temp1216.base= _temp1215;
_temp1216.last_plus_one= _temp1215 + 3; _temp1216;})), Cyc_Absynpp_exp2doc(
_temp1074), Cyc_PP_text(({ char* _temp1217=")"; struct _tagged_string _temp1218;
_temp1218.curr= _temp1217; _temp1218.base= _temp1217; _temp1218.last_plus_one=
_temp1217 + 2; _temp1218;}))); goto _LL975; _LL993: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1080), Cyc_PP_text(({ char* _temp1219="("; struct _tagged_string
_temp1220; _temp1220.curr= _temp1219; _temp1220.base= _temp1219; _temp1220.last_plus_one=
_temp1219 + 2; _temp1220;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1078), Cyc_PP_text(({
char* _temp1221=")"; struct _tagged_string _temp1222; _temp1222.curr= _temp1221;
_temp1222.base= _temp1221; _temp1222.last_plus_one= _temp1221 + 2; _temp1222;})));
goto _LL975; _LL995: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1084), Cyc_PP_text(({ char* _temp1223="("; struct _tagged_string _temp1224;
_temp1224.curr= _temp1223; _temp1224.base= _temp1223; _temp1224.last_plus_one=
_temp1223 + 2; _temp1224;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1082), Cyc_PP_text(({
char* _temp1225=")"; struct _tagged_string _temp1226; _temp1226.curr= _temp1225;
_temp1226.base= _temp1225; _temp1226.last_plus_one= _temp1225 + 2; _temp1226;})));
goto _LL975; _LL997: s= Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp1227="throw ";
struct _tagged_string _temp1228; _temp1228.curr= _temp1227; _temp1228.base=
_temp1227; _temp1228.last_plus_one= _temp1227 + 7; _temp1228;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1086)); goto _LL975; _LL999: s= Cyc_Absynpp_exp2doc_prec( inprec,
_temp1088); goto _LL975; _LL1001: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1092);
goto _LL975; _LL1003: s= Cyc_Absynpp_cat4( Cyc_PP_text(({ char* _temp1229="(";
struct _tagged_string _temp1230; _temp1230.curr= _temp1229; _temp1230.base=
_temp1229; _temp1230.last_plus_one= _temp1229 + 2; _temp1230;})), Cyc_Absynpp_typ2doc(
_temp1096), Cyc_PP_text(({ char* _temp1231=")"; struct _tagged_string _temp1232;
_temp1232.curr= _temp1231; _temp1232.base= _temp1231; _temp1232.last_plus_one=
_temp1231 + 2; _temp1232;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1094));
goto _LL975; _LL1005: s= Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp1233="&";
struct _tagged_string _temp1234; _temp1234.curr= _temp1233; _temp1234.base=
_temp1233; _temp1234.last_plus_one= _temp1233 + 2; _temp1234;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1098)); goto _LL975; _LL1007: if( _temp1102 == 0){ s= Cyc_Absynpp_cat2(
Cyc_PP_text(({ char* _temp1235="new "; struct _tagged_string _temp1236;
_temp1236.curr= _temp1235; _temp1236.base= _temp1235; _temp1236.last_plus_one=
_temp1235 + 5; _temp1236;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1100));}
else{ s= Cyc_Absynpp_cat4( Cyc_PP_text(({ char* _temp1237="rnew("; struct
_tagged_string _temp1238; _temp1238.curr= _temp1237; _temp1238.base= _temp1237;
_temp1238.last_plus_one= _temp1237 + 6; _temp1238;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1102)), Cyc_PP_text(({ char* _temp1239=") ";
struct _tagged_string _temp1240; _temp1240.curr= _temp1239; _temp1240.base=
_temp1239; _temp1240.last_plus_one= _temp1239 + 3; _temp1240;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1100));} goto _LL975; _LL1009: s= Cyc_Absynpp_cat3( Cyc_PP_text(({
char* _temp1241="sizeof("; struct _tagged_string _temp1242; _temp1242.curr=
_temp1241; _temp1242.base= _temp1241; _temp1242.last_plus_one= _temp1241 + 8;
_temp1242;})), Cyc_Absynpp_typ2doc( _temp1104), Cyc_PP_text(({ char* _temp1243=")";
struct _tagged_string _temp1244; _temp1244.curr= _temp1243; _temp1244.base=
_temp1243; _temp1244.last_plus_one= _temp1243 + 2; _temp1244;}))); goto _LL975;
_LL1011: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1245="sizeof("; struct
_tagged_string _temp1246; _temp1246.curr= _temp1245; _temp1246.base= _temp1245;
_temp1246.last_plus_one= _temp1245 + 8; _temp1246;})), Cyc_Absynpp_exp2doc(
_temp1106), Cyc_PP_text(({ char* _temp1247=")"; struct _tagged_string _temp1248;
_temp1248.curr= _temp1247; _temp1248.base= _temp1247; _temp1248.last_plus_one=
_temp1247 + 2; _temp1248;}))); goto _LL975; _LL1013: s= Cyc_Absynpp_cat2( Cyc_PP_text(({
char* _temp1249="*"; struct _tagged_string _temp1250; _temp1250.curr= _temp1249;
_temp1250.base= _temp1249; _temp1250.last_plus_one= _temp1249 + 2; _temp1250;})),
Cyc_Absynpp_exp2doc_prec( myprec, _temp1108)); goto _LL975; _LL1015: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1112), Cyc_PP_text(({ char* _temp1251=".";
struct _tagged_string _temp1252; _temp1252.curr= _temp1251; _temp1252.base=
_temp1251; _temp1252.last_plus_one= _temp1251 + 2; _temp1252;})), Cyc_Absynpp_textptr(
_temp1110)); goto _LL975; _LL1017: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1116), Cyc_PP_text(({ char* _temp1253="->"; struct _tagged_string
_temp1254; _temp1254.curr= _temp1253; _temp1254.base= _temp1253; _temp1254.last_plus_one=
_temp1253 + 3; _temp1254;})), Cyc_Absynpp_textptr( _temp1114)); goto _LL975;
_LL1019: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1120), Cyc_PP_text(({
char* _temp1255="["; struct _tagged_string _temp1256; _temp1256.curr= _temp1255;
_temp1256.base= _temp1255; _temp1256.last_plus_one= _temp1255 + 2; _temp1256;})),
Cyc_Absynpp_exp2doc( _temp1118), Cyc_PP_text(({ char* _temp1257="]"; struct
_tagged_string _temp1258; _temp1258.curr= _temp1257; _temp1258.base= _temp1257;
_temp1258.last_plus_one= _temp1257 + 2; _temp1258;}))); goto _LL975; _LL1021: s=
Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1259="$("; struct _tagged_string
_temp1260; _temp1260.curr= _temp1259; _temp1260.base= _temp1259; _temp1260.last_plus_one=
_temp1259 + 3; _temp1260;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1122), Cyc_PP_text(({
char* _temp1261=")"; struct _tagged_string _temp1262; _temp1262.curr= _temp1261;
_temp1262.base= _temp1261; _temp1262.last_plus_one= _temp1261 + 2; _temp1262;})));
goto _LL975; _LL1023: s= Cyc_Absynpp_cat4( Cyc_PP_text(({ char* _temp1263="(";
struct _tagged_string _temp1264; _temp1264.curr= _temp1263; _temp1264.base=
_temp1263; _temp1264.last_plus_one= _temp1263 + 2; _temp1264;})), Cyc_Absynpp_typ2doc((*
_temp1126).f3), Cyc_PP_text(({ char* _temp1265=")"; struct _tagged_string
_temp1266; _temp1266.curr= _temp1265; _temp1266.base= _temp1265; _temp1266.last_plus_one=
_temp1265 + 2; _temp1266;})), Cyc_Absynpp_group(({ char* _temp1267="{"; struct
_tagged_string _temp1268; _temp1268.curr= _temp1267; _temp1268.base= _temp1267;
_temp1268.last_plus_one= _temp1267 + 2; _temp1268;}),({ char* _temp1269="}";
struct _tagged_string _temp1270; _temp1270.curr= _temp1269; _temp1270.base=
_temp1269; _temp1270.last_plus_one= _temp1269 + 2; _temp1270;}),({ char*
_temp1271=","; struct _tagged_string _temp1272; _temp1272.curr= _temp1271;
_temp1272.base= _temp1271; _temp1272.last_plus_one= _temp1271 + 2; _temp1272;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1124))); goto _LL975; _LL1025: s=
Cyc_Absynpp_group(({ char* _temp1273="{"; struct _tagged_string _temp1274;
_temp1274.curr= _temp1273; _temp1274.base= _temp1273; _temp1274.last_plus_one=
_temp1273 + 2; _temp1274;}),({ char* _temp1275="}"; struct _tagged_string
_temp1276; _temp1276.curr= _temp1275; _temp1276.base= _temp1275; _temp1276.last_plus_one=
_temp1275 + 2; _temp1276;}),({ char* _temp1277=","; struct _tagged_string
_temp1278; _temp1278.curr= _temp1277; _temp1278.base= _temp1277; _temp1278.last_plus_one=
_temp1277 + 2; _temp1278;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1128)); goto _LL975; _LL1027: s= Cyc_Absynpp_cat7( Cyc_PP_text(({ char*
_temp1279="{for "; struct _tagged_string _temp1280; _temp1280.curr= _temp1279;
_temp1280.base= _temp1279; _temp1280.last_plus_one= _temp1279 + 6; _temp1280;})),
Cyc_PP_text(*(* _temp1134->name).f2), Cyc_PP_text(({ char* _temp1281=" < ";
struct _tagged_string _temp1282; _temp1282.curr= _temp1281; _temp1282.base=
_temp1281; _temp1282.last_plus_one= _temp1281 + 4; _temp1282;})), Cyc_Absynpp_exp2doc(
_temp1132), Cyc_PP_text(({ char* _temp1283=" : "; struct _tagged_string
_temp1284; _temp1284.curr= _temp1283; _temp1284.base= _temp1283; _temp1284.last_plus_one=
_temp1283 + 4; _temp1284;})), Cyc_Absynpp_exp2doc( _temp1130), Cyc_PP_text(({
char* _temp1285="}"; struct _tagged_string _temp1286; _temp1286.curr= _temp1285;
_temp1286.base= _temp1285; _temp1286.last_plus_one= _temp1285 + 2; _temp1286;})));
goto _LL975; _LL1029: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1142), Cyc_Absynpp_group(({
char* _temp1287="{"; struct _tagged_string _temp1288; _temp1288.curr= _temp1287;
_temp1288.base= _temp1287; _temp1288.last_plus_one= _temp1287 + 2; _temp1288;}),({
char* _temp1289="}"; struct _tagged_string _temp1290; _temp1290.curr= _temp1289;
_temp1290.base= _temp1289; _temp1290.last_plus_one= _temp1289 + 2; _temp1290;}),({
char* _temp1291=","; struct _tagged_string _temp1292; _temp1292.curr= _temp1291;
_temp1292.base= _temp1291; _temp1292.last_plus_one= _temp1291 + 2; _temp1292;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1138))); goto _LL975; _LL1031: s=
Cyc_Absynpp_group(({ char* _temp1293="{"; struct _tagged_string _temp1294;
_temp1294.curr= _temp1293; _temp1294.base= _temp1293; _temp1294.last_plus_one=
_temp1293 + 2; _temp1294;}),({ char* _temp1295="}"; struct _tagged_string
_temp1296; _temp1296.curr= _temp1295; _temp1296.base= _temp1295; _temp1296.last_plus_one=
_temp1295 + 2; _temp1296;}),({ char* _temp1297=","; struct _tagged_string
_temp1298; _temp1298.curr= _temp1297; _temp1298.base= _temp1297; _temp1298.last_plus_one=
_temp1297 + 2; _temp1298;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1144)); goto _LL975; _LL1033: if( _temp1152 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1148->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1148->name),
Cyc_Absynpp_egroup(({ char* _temp1299="("; struct _tagged_string _temp1300;
_temp1300.curr= _temp1299; _temp1300.base= _temp1299; _temp1300.last_plus_one=
_temp1299 + 2; _temp1300;}),({ char* _temp1301=")"; struct _tagged_string
_temp1302; _temp1302.curr= _temp1301; _temp1302.base= _temp1301; _temp1302.last_plus_one=
_temp1301 + 2; _temp1302;}),({ char* _temp1303=","; struct _tagged_string
_temp1304; _temp1304.curr= _temp1303; _temp1304.base= _temp1303; _temp1304.last_plus_one=
_temp1303 + 2; _temp1304;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_exp2doc,
_temp1152)));} goto _LL975; _LL1035: s= Cyc_Absynpp_qvar2doc( _temp1162); goto
_LL975; _LL1037: if( _temp1166 == 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char*
_temp1305="malloc(sizeof("; struct _tagged_string _temp1306; _temp1306.curr=
_temp1305; _temp1306.base= _temp1305; _temp1306.last_plus_one= _temp1305 + 15;
_temp1306;})), Cyc_Absynpp_typ2doc( _temp1164), Cyc_PP_text(({ char* _temp1307="))";
struct _tagged_string _temp1308; _temp1308.curr= _temp1307; _temp1308.base=
_temp1307; _temp1308.last_plus_one= _temp1307 + 3; _temp1308;})));} else{ s= Cyc_Absynpp_cat6(
Cyc_PP_text(({ char* _temp1309="rmalloc("; struct _tagged_string _temp1310;
_temp1310.curr= _temp1309; _temp1310.base= _temp1309; _temp1310.last_plus_one=
_temp1309 + 9; _temp1310;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( _temp1166)), Cyc_PP_text(({ char* _temp1311=","; struct
_tagged_string _temp1312; _temp1312.curr= _temp1311; _temp1312.base= _temp1311;
_temp1312.last_plus_one= _temp1311 + 2; _temp1312;})), Cyc_PP_text(({ char*
_temp1313="sizeof("; struct _tagged_string _temp1314; _temp1314.curr= _temp1313;
_temp1314.base= _temp1313; _temp1314.last_plus_one= _temp1313 + 8; _temp1314;})),
Cyc_Absynpp_typ2doc( _temp1164), Cyc_PP_text(({ char* _temp1315="))"; struct
_tagged_string _temp1316; _temp1316.curr= _temp1315; _temp1316.base= _temp1315;
_temp1316.last_plus_one= _temp1315 + 3; _temp1316;})));} goto _LL975; _LL1039: s=
Cyc_Absynpp_group(({ char* _temp1317="{"; struct _tagged_string _temp1318;
_temp1318.curr= _temp1317; _temp1318.base= _temp1317; _temp1318.last_plus_one=
_temp1317 + 2; _temp1318;}),({ char* _temp1319="}"; struct _tagged_string
_temp1320; _temp1320.curr= _temp1319; _temp1320.base= _temp1319; _temp1320.last_plus_one=
_temp1319 + 2; _temp1320;}),({ char* _temp1321=","; struct _tagged_string
_temp1322; _temp1322.curr= _temp1321; _temp1322.base= _temp1321; _temp1322.last_plus_one=
_temp1321 + 2; _temp1322;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1168)); goto _LL975; _LL1041: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char*
_temp1323="({ "; struct _tagged_string _temp1324; _temp1324.curr= _temp1323;
_temp1324.base= _temp1323; _temp1324.last_plus_one= _temp1323 + 4; _temp1324;})),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1172)), Cyc_PP_text(({ char*
_temp1325=" })"; struct _tagged_string _temp1326; _temp1326.curr= _temp1325;
_temp1326.base= _temp1325; _temp1326.last_plus_one= _temp1325 + 4; _temp1326;})));
goto _LL975; _LL1043: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1327="codegen(";
struct _tagged_string _temp1328; _temp1328.curr= _temp1327; _temp1328.base=
_temp1327; _temp1328.last_plus_one= _temp1327 + 9; _temp1328;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1329=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1329->r=( void*)(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp1330=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1330[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1331; _temp1331.tag= Cyc_Absyn_Fn_d; _temp1331.f1= _temp1174; _temp1331;});
_temp1330;})); _temp1329->loc= e->loc; _temp1329;}))), Cyc_PP_text(({ char*
_temp1332=")"; struct _tagged_string _temp1333; _temp1333.curr= _temp1332;
_temp1333.base= _temp1332; _temp1333.last_plus_one= _temp1332 + 2; _temp1333;})));
goto _LL975; _LL1045: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1334="fill(";
struct _tagged_string _temp1335; _temp1335.curr= _temp1334; _temp1335.base=
_temp1334; _temp1335.last_plus_one= _temp1334 + 6; _temp1335;})), Cyc_PP_nest( 2,
Cyc_Absynpp_exp2doc( _temp1176)), Cyc_PP_text(({ char* _temp1336=")"; struct
_tagged_string _temp1337; _temp1337.curr= _temp1336; _temp1337.base= _temp1336;
_temp1337.last_plus_one= _temp1336 + 2; _temp1337;}))); goto _LL975; _LL975:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1338="(";
struct _tagged_string _temp1339; _temp1339.curr= _temp1338; _temp1339.base=
_temp1338; _temp1339.last_plus_one= _temp1338 + 2; _temp1339;})), s, Cyc_PP_text(({
char* _temp1340=")"; struct _tagged_string _temp1341; _temp1341.curr= _temp1340;
_temp1341.base= _temp1340; _temp1341.last_plus_one= _temp1340 + 2; _temp1341;})));}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void*
_temp1342= d; struct Cyc_Absyn_Exp* _temp1348; struct _tagged_string* _temp1350;
_LL1344: if(*(( int*) _temp1342) == Cyc_Absyn_ArrayElement){ _LL1349: _temp1348=((
struct Cyc_Absyn_ArrayElement_struct*) _temp1342)->f1; goto _LL1345;} else{ goto
_LL1346;} _LL1346: if(*(( int*) _temp1342) == Cyc_Absyn_FieldName){ _LL1351:
_temp1350=(( struct Cyc_Absyn_FieldName_struct*) _temp1342)->f1; goto _LL1347;}
else{ goto _LL1343;} _LL1345: return Cyc_Absynpp_cat3( Cyc_PP_text(({ char*
_temp1352=".["; struct _tagged_string _temp1353; _temp1353.curr= _temp1352;
_temp1353.base= _temp1352; _temp1353.last_plus_one= _temp1352 + 3; _temp1353;})),
Cyc_Absynpp_exp2doc( _temp1348), Cyc_PP_text(({ char* _temp1354="]"; struct
_tagged_string _temp1355; _temp1355.curr= _temp1354; _temp1355.base= _temp1354;
_temp1355.last_plus_one= _temp1354 + 2; _temp1355;}))); _LL1347: return Cyc_Absynpp_cat2(
Cyc_PP_text(({ char* _temp1356="."; struct _tagged_string _temp1357; _temp1357.curr=
_temp1356; _temp1357.base= _temp1356; _temp1357.last_plus_one= _temp1356 + 2;
_temp1357;})), Cyc_Absynpp_textptr( _temp1350)); _LL1343:;} struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((*
de).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup(({ char* _temp1358="";
struct _tagged_string _temp1359; _temp1359.curr= _temp1358; _temp1359.base=
_temp1358; _temp1359.last_plus_one= _temp1358 + 1; _temp1359;}),({ char*
_temp1360="="; struct _tagged_string _temp1361; _temp1361.curr= _temp1360;
_temp1361.base= _temp1360; _temp1361.last_plus_one= _temp1360 + 2; _temp1361;}),({
char* _temp1362="="; struct _tagged_string _temp1363; _temp1363.curr= _temp1362;
_temp1363.base= _temp1362; _temp1363.last_plus_one= _temp1362 + 2; _temp1363;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* de).f1)), Cyc_Absynpp_exp2doc((*
de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_Absynpp_group(({ char* _temp1364=""; struct _tagged_string
_temp1365; _temp1365.curr= _temp1364; _temp1365.base= _temp1364; _temp1365.last_plus_one=
_temp1364 + 1; _temp1365;}),({ char* _temp1366=""; struct _tagged_string
_temp1367; _temp1367.curr= _temp1366; _temp1367.base= _temp1366; _temp1367.last_plus_one=
_temp1366 + 1; _temp1367;}),({ char* _temp1368=","; struct _tagged_string
_temp1369; _temp1369.curr= _temp1368; _temp1369.base= _temp1368; _temp1369.last_plus_one=
_temp1368 + 2; _temp1369;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1370= c; char _temp1388; void* _temp1390; short _temp1392;
void* _temp1394; int _temp1396; void* _temp1398; int _temp1400; void* _temp1402;
long long _temp1404; void* _temp1406; struct _tagged_string _temp1408; struct
_tagged_string _temp1410; _LL1372: if(( unsigned int) _temp1370 > 1u?*(( int*)
_temp1370) == Cyc_Absyn_Char_c: 0){ _LL1391: _temp1390=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp1370)->f1; goto _LL1389; _LL1389: _temp1388=(( struct Cyc_Absyn_Char_c_struct*)
_temp1370)->f2; goto _LL1373;} else{ goto _LL1374;} _LL1374: if(( unsigned int)
_temp1370 > 1u?*(( int*) _temp1370) == Cyc_Absyn_Short_c: 0){ _LL1395: _temp1394=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1370)->f1; goto _LL1393; _LL1393:
_temp1392=(( struct Cyc_Absyn_Short_c_struct*) _temp1370)->f2; goto _LL1375;}
else{ goto _LL1376;} _LL1376: if(( unsigned int) _temp1370 > 1u?*(( int*)
_temp1370) == Cyc_Absyn_Int_c: 0){ _LL1399: _temp1398=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp1370)->f1; if( _temp1398 ==( void*) Cyc_Absyn_Signed){ goto _LL1397;} else{
goto _LL1378;} _LL1397: _temp1396=(( struct Cyc_Absyn_Int_c_struct*) _temp1370)->f2;
goto _LL1377;} else{ goto _LL1378;} _LL1378: if(( unsigned int) _temp1370 > 1u?*((
int*) _temp1370) == Cyc_Absyn_Int_c: 0){ _LL1403: _temp1402=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1370)->f1; if( _temp1402 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1401;} else{ goto _LL1380;} _LL1401: _temp1400=(( struct Cyc_Absyn_Int_c_struct*)
_temp1370)->f2; goto _LL1379;} else{ goto _LL1380;} _LL1380: if(( unsigned int)
_temp1370 > 1u?*(( int*) _temp1370) == Cyc_Absyn_LongLong_c: 0){ _LL1407:
_temp1406=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1370)->f1; goto
_LL1405; _LL1405: _temp1404=(( struct Cyc_Absyn_LongLong_c_struct*) _temp1370)->f2;
goto _LL1381;} else{ goto _LL1382;} _LL1382: if(( unsigned int) _temp1370 > 1u?*((
int*) _temp1370) == Cyc_Absyn_Float_c: 0){ _LL1409: _temp1408=(( struct Cyc_Absyn_Float_c_struct*)
_temp1370)->f1; goto _LL1383;} else{ goto _LL1384;} _LL1384: if( _temp1370 ==(
void*) Cyc_Absyn_Null_c){ goto _LL1385;} else{ goto _LL1386;} _LL1386: if((
unsigned int) _temp1370 > 1u?*(( int*) _temp1370) == Cyc_Absyn_String_c: 0){
_LL1411: _temp1410=(( struct Cyc_Absyn_String_c_struct*) _temp1370)->f1; goto
_LL1387;} else{ goto _LL1371;} _LL1373: return Cyc_PP_text(({ struct
_tagged_string _temp1412= Cyc_Absynpp_char_escape( _temp1388); xprintf("'%.*s'",
_temp1412.last_plus_one - _temp1412.curr, _temp1412.curr);})); _LL1375: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1392)); _LL1377: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1396)); _LL1379: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1400)); _LL1381: return Cyc_PP_text(({ char* _temp1413="<<FIX LONG LONG CONSTANT>>";
struct _tagged_string _temp1414; _temp1414.curr= _temp1413; _temp1414.base=
_temp1413; _temp1414.last_plus_one= _temp1413 + 27; _temp1414;})); _LL1383:
return Cyc_PP_text( _temp1408); _LL1385: return Cyc_PP_text(({ char* _temp1415="null";
struct _tagged_string _temp1416; _temp1416.curr= _temp1415; _temp1416.base=
_temp1415; _temp1416.last_plus_one= _temp1415 + 5; _temp1416;})); _LL1387:
return Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1417="\""; struct
_tagged_string _temp1418; _temp1418.curr= _temp1417; _temp1418.base= _temp1417;
_temp1418.last_plus_one= _temp1417 + 2; _temp1418;})), Cyc_PP_text( Cyc_Absynpp_string_escape(
_temp1410)), Cyc_PP_text(({ char* _temp1419="\""; struct _tagged_string
_temp1420; _temp1420.curr= _temp1419; _temp1420.base= _temp1419; _temp1420.last_plus_one=
_temp1419 + 2; _temp1420;}))); _LL1371:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl != 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1421=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1421[ 0]=({ struct Cyc_Core_Failure_struct _temp1422; _temp1422.tag= Cyc_Core_Failure;
_temp1422.f1=({ struct _tagged_string _temp1423= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1423.last_plus_one
- _temp1423.curr, _temp1423.curr);}); _temp1422;}); _temp1421;}));}{ struct Cyc_PP_Doc*
_temp1424= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd); goto _LL1425; _LL1425: return Cyc_Absynpp_cat2(
_temp1424, Cyc_PP_text(({ char* _temp1426=".size"; struct _tagged_string
_temp1427; _temp1427.curr= _temp1426; _temp1427.base= _temp1426; _temp1427.last_plus_one=
_temp1426 + 6; _temp1427;})));}} else{ if( ! Cyc_Absyn_is_format_prim( p)){
struct Cyc_List_List* ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es); if( ds == 0){( void) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp1428=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1428[ 0]=({ struct Cyc_Core_Failure_struct
_temp1429; _temp1429.tag= Cyc_Core_Failure; _temp1429.f1=({ struct
_tagged_string _temp1430= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with no args",
_temp1430.last_plus_one - _temp1430.curr, _temp1430.curr);}); _temp1429;});
_temp1428;}));} else{ if((( struct Cyc_List_List*) _check_null( ds))->tl == 0){
return Cyc_Absynpp_cat3( ps, Cyc_PP_text(({ char* _temp1431=" "; struct
_tagged_string _temp1432; _temp1432.curr= _temp1431; _temp1432.base= _temp1431;
_temp1432.last_plus_one= _temp1431 + 2; _temp1432;})),( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( ds))->hd);} else{ if((( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( ds))->tl))->tl != 0){( void)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp1433=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1433[ 0]=({ struct Cyc_Core_Failure_struct
_temp1434; _temp1434.tag= Cyc_Core_Failure; _temp1434.f1=({ struct
_tagged_string _temp1435= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_temp1435.last_plus_one - _temp1435.curr, _temp1435.curr);}); _temp1434;});
_temp1433;}));} else{ return Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( ds))->hd, Cyc_PP_text(({ char* _temp1436=" "; struct _tagged_string
_temp1437; _temp1437.curr= _temp1436; _temp1437.base= _temp1436; _temp1437.last_plus_one=
_temp1436 + 2; _temp1437;})), ps, Cyc_PP_text(({ char* _temp1438=" "; struct
_tagged_string _temp1439; _temp1439.curr= _temp1438; _temp1439.base= _temp1438;
_temp1439.last_plus_one= _temp1438 + 2; _temp1439;})),( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( ds))->tl))->hd);}}}}
else{ return Cyc_Absynpp_cat4( ps, Cyc_PP_text(({ char* _temp1440="("; struct
_tagged_string _temp1441; _temp1441.curr= _temp1440; _temp1441.base= _temp1440;
_temp1441.last_plus_one= _temp1440 + 2; _temp1441;})), Cyc_Absynpp_exps2doc_prec(
20, es), Cyc_PP_text(({ char* _temp1442=")"; struct _tagged_string _temp1443;
_temp1443.curr= _temp1442; _temp1443.base= _temp1442; _temp1443.last_plus_one=
_temp1442 + 2; _temp1443;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1444= p; _LL1446: if( _temp1444 ==( void*) Cyc_Absyn_Plus){
goto _LL1447;} else{ goto _LL1448;} _LL1448: if( _temp1444 ==( void*) Cyc_Absyn_Times){
goto _LL1449;} else{ goto _LL1450;} _LL1450: if( _temp1444 ==( void*) Cyc_Absyn_Minus){
goto _LL1451;} else{ goto _LL1452;} _LL1452: if( _temp1444 ==( void*) Cyc_Absyn_Div){
goto _LL1453;} else{ goto _LL1454;} _LL1454: if( _temp1444 ==( void*) Cyc_Absyn_Mod){
goto _LL1455;} else{ goto _LL1456;} _LL1456: if( _temp1444 ==( void*) Cyc_Absyn_Eq){
goto _LL1457;} else{ goto _LL1458;} _LL1458: if( _temp1444 ==( void*) Cyc_Absyn_Neq){
goto _LL1459;} else{ goto _LL1460;} _LL1460: if( _temp1444 ==( void*) Cyc_Absyn_Gt){
goto _LL1461;} else{ goto _LL1462;} _LL1462: if( _temp1444 ==( void*) Cyc_Absyn_Lt){
goto _LL1463;} else{ goto _LL1464;} _LL1464: if( _temp1444 ==( void*) Cyc_Absyn_Gte){
goto _LL1465;} else{ goto _LL1466;} _LL1466: if( _temp1444 ==( void*) Cyc_Absyn_Lte){
goto _LL1467;} else{ goto _LL1468;} _LL1468: if( _temp1444 ==( void*) Cyc_Absyn_Not){
goto _LL1469;} else{ goto _LL1470;} _LL1470: if( _temp1444 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1471;} else{ goto _LL1472;} _LL1472: if( _temp1444 ==( void*) Cyc_Absyn_Bitand){
goto _LL1473;} else{ goto _LL1474;} _LL1474: if( _temp1444 ==( void*) Cyc_Absyn_Bitor){
goto _LL1475;} else{ goto _LL1476;} _LL1476: if( _temp1444 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1477;} else{ goto _LL1478;} _LL1478: if( _temp1444 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1479;} else{ goto _LL1480;} _LL1480: if( _temp1444 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1481;} else{ goto _LL1482;} _LL1482: if( _temp1444 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1483;} else{ goto _LL1484;} _LL1484: if( _temp1444 ==( void*) Cyc_Absyn_Size){
goto _LL1485;} else{ goto _LL1486;} _LL1486: if( _temp1444 ==( void*) Cyc_Absyn_Printf){
goto _LL1487;} else{ goto _LL1488;} _LL1488: if( _temp1444 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1489;} else{ goto _LL1490;} _LL1490: if( _temp1444 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1491;} else{ goto _LL1492;} _LL1492: if( _temp1444 ==( void*) Cyc_Absyn_Scanf){
goto _LL1493;} else{ goto _LL1494;} _LL1494: if( _temp1444 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1495;} else{ goto _LL1496;} _LL1496: if( _temp1444 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1497;} else{ goto _LL1445;} _LL1447: return({ char* _temp1498="+";
struct _tagged_string _temp1499; _temp1499.curr= _temp1498; _temp1499.base=
_temp1498; _temp1499.last_plus_one= _temp1498 + 2; _temp1499;}); _LL1449: return({
char* _temp1500="*"; struct _tagged_string _temp1501; _temp1501.curr= _temp1500;
_temp1501.base= _temp1500; _temp1501.last_plus_one= _temp1500 + 2; _temp1501;});
_LL1451: return({ char* _temp1502="-"; struct _tagged_string _temp1503;
_temp1503.curr= _temp1502; _temp1503.base= _temp1502; _temp1503.last_plus_one=
_temp1502 + 2; _temp1503;}); _LL1453: return({ char* _temp1504="/"; struct
_tagged_string _temp1505; _temp1505.curr= _temp1504; _temp1505.base= _temp1504;
_temp1505.last_plus_one= _temp1504 + 2; _temp1505;}); _LL1455: return({ char*
_temp1506="%"; struct _tagged_string _temp1507; _temp1507.curr= _temp1506;
_temp1507.base= _temp1506; _temp1507.last_plus_one= _temp1506 + 2; _temp1507;});
_LL1457: return({ char* _temp1508="=="; struct _tagged_string _temp1509;
_temp1509.curr= _temp1508; _temp1509.base= _temp1508; _temp1509.last_plus_one=
_temp1508 + 3; _temp1509;}); _LL1459: return({ char* _temp1510="!="; struct
_tagged_string _temp1511; _temp1511.curr= _temp1510; _temp1511.base= _temp1510;
_temp1511.last_plus_one= _temp1510 + 3; _temp1511;}); _LL1461: return({ char*
_temp1512=">"; struct _tagged_string _temp1513; _temp1513.curr= _temp1512;
_temp1513.base= _temp1512; _temp1513.last_plus_one= _temp1512 + 2; _temp1513;});
_LL1463: return({ char* _temp1514="<"; struct _tagged_string _temp1515;
_temp1515.curr= _temp1514; _temp1515.base= _temp1514; _temp1515.last_plus_one=
_temp1514 + 2; _temp1515;}); _LL1465: return({ char* _temp1516=">="; struct
_tagged_string _temp1517; _temp1517.curr= _temp1516; _temp1517.base= _temp1516;
_temp1517.last_plus_one= _temp1516 + 3; _temp1517;}); _LL1467: return({ char*
_temp1518="<="; struct _tagged_string _temp1519; _temp1519.curr= _temp1518;
_temp1519.base= _temp1518; _temp1519.last_plus_one= _temp1518 + 3; _temp1519;});
_LL1469: return({ char* _temp1520="!"; struct _tagged_string _temp1521;
_temp1521.curr= _temp1520; _temp1521.base= _temp1520; _temp1521.last_plus_one=
_temp1520 + 2; _temp1521;}); _LL1471: return({ char* _temp1522="~"; struct
_tagged_string _temp1523; _temp1523.curr= _temp1522; _temp1523.base= _temp1522;
_temp1523.last_plus_one= _temp1522 + 2; _temp1523;}); _LL1473: return({ char*
_temp1524="&"; struct _tagged_string _temp1525; _temp1525.curr= _temp1524;
_temp1525.base= _temp1524; _temp1525.last_plus_one= _temp1524 + 2; _temp1525;});
_LL1475: return({ char* _temp1526="|"; struct _tagged_string _temp1527;
_temp1527.curr= _temp1526; _temp1527.base= _temp1526; _temp1527.last_plus_one=
_temp1526 + 2; _temp1527;}); _LL1477: return({ char* _temp1528="^"; struct
_tagged_string _temp1529; _temp1529.curr= _temp1528; _temp1529.base= _temp1528;
_temp1529.last_plus_one= _temp1528 + 2; _temp1529;}); _LL1479: return({ char*
_temp1530="<<"; struct _tagged_string _temp1531; _temp1531.curr= _temp1530;
_temp1531.base= _temp1530; _temp1531.last_plus_one= _temp1530 + 3; _temp1531;});
_LL1481: return({ char* _temp1532=">>"; struct _tagged_string _temp1533;
_temp1533.curr= _temp1532; _temp1533.base= _temp1532; _temp1533.last_plus_one=
_temp1532 + 3; _temp1533;}); _LL1483: return({ char* _temp1534=">>>"; struct
_tagged_string _temp1535; _temp1535.curr= _temp1534; _temp1535.base= _temp1534;
_temp1535.last_plus_one= _temp1534 + 4; _temp1535;}); _LL1485: return({ char*
_temp1536="size"; struct _tagged_string _temp1537; _temp1537.curr= _temp1536;
_temp1537.base= _temp1536; _temp1537.last_plus_one= _temp1536 + 5; _temp1537;});
_LL1487: return({ char* _temp1538="printf"; struct _tagged_string _temp1539;
_temp1539.curr= _temp1538; _temp1539.base= _temp1538; _temp1539.last_plus_one=
_temp1538 + 7; _temp1539;}); _LL1489: return({ char* _temp1540="fprintf"; struct
_tagged_string _temp1541; _temp1541.curr= _temp1540; _temp1541.base= _temp1540;
_temp1541.last_plus_one= _temp1540 + 8; _temp1541;}); _LL1491: return({ char*
_temp1542="xprintf"; struct _tagged_string _temp1543; _temp1543.curr= _temp1542;
_temp1543.base= _temp1542; _temp1543.last_plus_one= _temp1542 + 8; _temp1543;});
_LL1493: return({ char* _temp1544="scanf"; struct _tagged_string _temp1545;
_temp1545.curr= _temp1544; _temp1545.base= _temp1544; _temp1545.last_plus_one=
_temp1544 + 6; _temp1545;}); _LL1495: return({ char* _temp1546="fscanf"; struct
_tagged_string _temp1547; _temp1547.curr= _temp1546; _temp1547.base= _temp1546;
_temp1547.last_plus_one= _temp1546 + 7; _temp1547;}); _LL1497: return({ char*
_temp1548="sscanf"; struct _tagged_string _temp1549; _temp1549.curr= _temp1548;
_temp1549.base= _temp1548; _temp1549.last_plus_one= _temp1548 + 7; _temp1549;});
_LL1445:;} struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text(
Cyc_Absynpp_prim2str( p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp*
f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((*
t).f1 == 0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3(
Cyc_Absynpp_textptr(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null((* t).f1))->v), Cyc_PP_text(({ char* _temp1550=" = "; struct
_tagged_string _temp1551; _temp1551.curr= _temp1550; _temp1551.base= _temp1550;
_temp1551.last_plus_one= _temp1550 + 4; _temp1551;})), Cyc_Absynpp_exp2doc((* t).f2));}}
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
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1552=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1558; struct Cyc_Absyn_Decl* _temp1560;
_LL1554: if(( unsigned int) _temp1552 > 1u?*(( int*) _temp1552) == Cyc_Absyn_Decl_s:
0){ _LL1561: _temp1560=(( struct Cyc_Absyn_Decl_s_struct*) _temp1552)->f1; goto
_LL1559; _LL1559: _temp1558=(( struct Cyc_Absyn_Decl_s_struct*) _temp1552)->f2;
goto _LL1555;} else{ goto _LL1556;} _LL1556: goto _LL1557; _LL1555: return 1;
_LL1557: return 0; _LL1553:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1562=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1604; struct Cyc_Absyn_Stmt* _temp1606; struct Cyc_Absyn_Stmt*
_temp1608; struct Cyc_Absyn_Exp* _temp1610; struct Cyc_Absyn_Stmt* _temp1612;
struct Cyc_Absyn_Stmt* _temp1614; struct Cyc_Absyn_Exp* _temp1616; struct Cyc_Absyn_Stmt*
_temp1618; struct _tuple2 _temp1620; struct Cyc_Absyn_Stmt* _temp1622; struct
Cyc_Absyn_Exp* _temp1624; struct Cyc_Absyn_Stmt* _temp1626; struct Cyc_Absyn_Stmt*
_temp1628; struct Cyc_Absyn_Stmt* _temp1630; struct _tagged_string* _temp1632;
struct Cyc_Absyn_Stmt* _temp1634; struct _tuple2 _temp1636; struct Cyc_Absyn_Stmt*
_temp1638; struct Cyc_Absyn_Exp* _temp1640; struct _tuple2 _temp1642; struct Cyc_Absyn_Stmt*
_temp1644; struct Cyc_Absyn_Exp* _temp1646; struct Cyc_Absyn_Exp* _temp1648;
struct Cyc_List_List* _temp1650; struct Cyc_Absyn_Exp* _temp1652; struct Cyc_Absyn_Switch_clause**
_temp1654; struct Cyc_List_List* _temp1656; struct Cyc_Absyn_Switch_clause**
_temp1658; struct Cyc_List_List* _temp1660; struct Cyc_Absyn_Stmt* _temp1662;
struct Cyc_Absyn_Decl* _temp1664; struct Cyc_Absyn_Stmt* _temp1666; struct Cyc_Absyn_Stmt*
_temp1668; struct Cyc_Absyn_Stmt* _temp1670; struct _tagged_string* _temp1672;
struct _tuple2 _temp1674; struct Cyc_Absyn_Stmt* _temp1676; struct Cyc_Absyn_Exp*
_temp1678; struct Cyc_Absyn_Stmt* _temp1680; struct Cyc_List_List* _temp1682;
struct Cyc_Absyn_Stmt* _temp1684; struct Cyc_Absyn_Stmt* _temp1686; struct Cyc_Absyn_Vardecl*
_temp1688; struct Cyc_Absyn_Tvar* _temp1690; _LL1564: if( _temp1562 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1565;} else{ goto _LL1566;} _LL1566: if((
unsigned int) _temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_Exp_s: 0){
_LL1605: _temp1604=(( struct Cyc_Absyn_Exp_s_struct*) _temp1562)->f1; goto
_LL1567;} else{ goto _LL1568;} _LL1568: if(( unsigned int) _temp1562 > 1u?*((
int*) _temp1562) == Cyc_Absyn_Seq_s: 0){ _LL1609: _temp1608=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1562)->f1; goto _LL1607; _LL1607: _temp1606=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1562)->f2; goto _LL1569;} else{ goto _LL1570;} _LL1570: if(( unsigned int)
_temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_Return_s: 0){ _LL1611:
_temp1610=(( struct Cyc_Absyn_Return_s_struct*) _temp1562)->f1; goto _LL1571;}
else{ goto _LL1572;} _LL1572: if(( unsigned int) _temp1562 > 1u?*(( int*)
_temp1562) == Cyc_Absyn_IfThenElse_s: 0){ _LL1617: _temp1616=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1562)->f1; goto _LL1615; _LL1615: _temp1614=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1562)->f2; goto _LL1613; _LL1613: _temp1612=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1562)->f3; goto _LL1573;} else{ goto _LL1574;} _LL1574: if(( unsigned int)
_temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_While_s: 0){ _LL1621: _temp1620=((
struct Cyc_Absyn_While_s_struct*) _temp1562)->f1; _LL1625: _temp1624= _temp1620.f1;
goto _LL1623; _LL1623: _temp1622= _temp1620.f2; goto _LL1619; _LL1619: _temp1618=((
struct Cyc_Absyn_While_s_struct*) _temp1562)->f2; goto _LL1575;} else{ goto
_LL1576;} _LL1576: if(( unsigned int) _temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_Break_s:
0){ _LL1627: _temp1626=(( struct Cyc_Absyn_Break_s_struct*) _temp1562)->f1; goto
_LL1577;} else{ goto _LL1578;} _LL1578: if(( unsigned int) _temp1562 > 1u?*((
int*) _temp1562) == Cyc_Absyn_Continue_s: 0){ _LL1629: _temp1628=(( struct Cyc_Absyn_Continue_s_struct*)
_temp1562)->f1; goto _LL1579;} else{ goto _LL1580;} _LL1580: if(( unsigned int)
_temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_Goto_s: 0){ _LL1633: _temp1632=((
struct Cyc_Absyn_Goto_s_struct*) _temp1562)->f1; goto _LL1631; _LL1631:
_temp1630=(( struct Cyc_Absyn_Goto_s_struct*) _temp1562)->f2; goto _LL1581;}
else{ goto _LL1582;} _LL1582: if(( unsigned int) _temp1562 > 1u?*(( int*)
_temp1562) == Cyc_Absyn_For_s: 0){ _LL1649: _temp1648=(( struct Cyc_Absyn_For_s_struct*)
_temp1562)->f1; goto _LL1643; _LL1643: _temp1642=(( struct Cyc_Absyn_For_s_struct*)
_temp1562)->f2; _LL1647: _temp1646= _temp1642.f1; goto _LL1645; _LL1645:
_temp1644= _temp1642.f2; goto _LL1637; _LL1637: _temp1636=(( struct Cyc_Absyn_For_s_struct*)
_temp1562)->f3; _LL1641: _temp1640= _temp1636.f1; goto _LL1639; _LL1639:
_temp1638= _temp1636.f2; goto _LL1635; _LL1635: _temp1634=(( struct Cyc_Absyn_For_s_struct*)
_temp1562)->f4; goto _LL1583;} else{ goto _LL1584;} _LL1584: if(( unsigned int)
_temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_Switch_s: 0){ _LL1653:
_temp1652=(( struct Cyc_Absyn_Switch_s_struct*) _temp1562)->f1; goto _LL1651;
_LL1651: _temp1650=(( struct Cyc_Absyn_Switch_s_struct*) _temp1562)->f2; goto
_LL1585;} else{ goto _LL1586;} _LL1586: if(( unsigned int) _temp1562 > 1u?*((
int*) _temp1562) == Cyc_Absyn_Fallthru_s: 0){ _LL1657: _temp1656=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1562)->f1; if( _temp1656 == 0){ goto _LL1655;} else{ goto _LL1588;} _LL1655:
_temp1654=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1562)->f2; goto _LL1587;}
else{ goto _LL1588;} _LL1588: if(( unsigned int) _temp1562 > 1u?*(( int*)
_temp1562) == Cyc_Absyn_Fallthru_s: 0){ _LL1661: _temp1660=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1562)->f1; goto _LL1659; _LL1659: _temp1658=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1562)->f2; goto _LL1589;} else{ goto _LL1590;} _LL1590: if(( unsigned int)
_temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_Decl_s: 0){ _LL1665: _temp1664=((
struct Cyc_Absyn_Decl_s_struct*) _temp1562)->f1; goto _LL1663; _LL1663:
_temp1662=(( struct Cyc_Absyn_Decl_s_struct*) _temp1562)->f2; goto _LL1591;}
else{ goto _LL1592;} _LL1592: if(( unsigned int) _temp1562 > 1u?*(( int*)
_temp1562) == Cyc_Absyn_Cut_s: 0){ _LL1667: _temp1666=(( struct Cyc_Absyn_Cut_s_struct*)
_temp1562)->f1; goto _LL1593;} else{ goto _LL1594;} _LL1594: if(( unsigned int)
_temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_Splice_s: 0){ _LL1669:
_temp1668=(( struct Cyc_Absyn_Splice_s_struct*) _temp1562)->f1; goto _LL1595;}
else{ goto _LL1596;} _LL1596: if(( unsigned int) _temp1562 > 1u?*(( int*)
_temp1562) == Cyc_Absyn_Label_s: 0){ _LL1673: _temp1672=(( struct Cyc_Absyn_Label_s_struct*)
_temp1562)->f1; goto _LL1671; _LL1671: _temp1670=(( struct Cyc_Absyn_Label_s_struct*)
_temp1562)->f2; goto _LL1597;} else{ goto _LL1598;} _LL1598: if(( unsigned int)
_temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_Do_s: 0){ _LL1681: _temp1680=((
struct Cyc_Absyn_Do_s_struct*) _temp1562)->f1; goto _LL1675; _LL1675: _temp1674=((
struct Cyc_Absyn_Do_s_struct*) _temp1562)->f2; _LL1679: _temp1678= _temp1674.f1;
goto _LL1677; _LL1677: _temp1676= _temp1674.f2; goto _LL1599;} else{ goto
_LL1600;} _LL1600: if(( unsigned int) _temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_TryCatch_s:
0){ _LL1685: _temp1684=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1562)->f1;
goto _LL1683; _LL1683: _temp1682=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1562)->f2; goto _LL1601;} else{ goto _LL1602;} _LL1602: if(( unsigned int)
_temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_Region_s: 0){ _LL1691:
_temp1690=(( struct Cyc_Absyn_Region_s_struct*) _temp1562)->f1; goto _LL1689;
_LL1689: _temp1688=(( struct Cyc_Absyn_Region_s_struct*) _temp1562)->f2; goto
_LL1687; _LL1687: _temp1686=(( struct Cyc_Absyn_Region_s_struct*) _temp1562)->f3;
goto _LL1603;} else{ goto _LL1563;} _LL1565: s= Cyc_PP_text(({ char* _temp1692=";";
struct _tagged_string _temp1693; _temp1693.curr= _temp1692; _temp1693.base=
_temp1692; _temp1693.last_plus_one= _temp1692 + 2; _temp1693;})); goto _LL1563;
_LL1567: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc( _temp1604), Cyc_PP_text(({
char* _temp1694=";"; struct _tagged_string _temp1695; _temp1695.curr= _temp1694;
_temp1695.base= _temp1694; _temp1695.last_plus_one= _temp1694 + 2; _temp1695;})));
goto _LL1563; _LL1569: if( Cyc_Absynpp_decls_first){ if( Cyc_Absynpp_is_declaration(
_temp1608)){ s= Cyc_Absynpp_cat6( Cyc_PP_text(({ char* _temp1696="{ "; struct
_tagged_string _temp1697; _temp1697.curr= _temp1696; _temp1697.base= _temp1696;
_temp1697.last_plus_one= _temp1696 + 3; _temp1697;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1608)), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp1698="}"; struct
_tagged_string _temp1699; _temp1699.curr= _temp1698; _temp1699.base= _temp1698;
_temp1699.last_plus_one= _temp1698 + 2; _temp1699;})), Cyc_PP_line_doc(), Cyc_Absynpp_is_declaration(
_temp1606)? Cyc_Absynpp_cat4( Cyc_PP_text(({ char* _temp1700="{ "; struct
_tagged_string _temp1701; _temp1701.curr= _temp1700; _temp1701.base= _temp1700;
_temp1701.last_plus_one= _temp1700 + 3; _temp1701;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1606)), Cyc_PP_text(({ char* _temp1702="}"; struct _tagged_string _temp1703;
_temp1703.curr= _temp1702; _temp1703.base= _temp1702; _temp1703.last_plus_one=
_temp1702 + 2; _temp1703;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1606));} else{ if( Cyc_Absynpp_is_declaration( _temp1606)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1608), Cyc_PP_line_doc(), Cyc_PP_text(({ char*
_temp1704="{ "; struct _tagged_string _temp1705; _temp1705.curr= _temp1704;
_temp1705.base= _temp1704; _temp1705.last_plus_one= _temp1704 + 3; _temp1705;})),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1606)), Cyc_PP_text(({ char*
_temp1706="}"; struct _tagged_string _temp1707; _temp1707.curr= _temp1706;
_temp1707.base= _temp1706; _temp1707.last_plus_one= _temp1706 + 2; _temp1707;})),
Cyc_PP_line_doc());} else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Stmt*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({ char* _temp1708=""; struct
_tagged_string _temp1709; _temp1709.curr= _temp1708; _temp1709.base= _temp1708;
_temp1709.last_plus_one= _temp1708 + 1; _temp1709;}),({ struct Cyc_List_List*
_temp1710=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1710->hd=( void*) _temp1608; _temp1710->tl=({ struct Cyc_List_List*
_temp1711=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1711->hd=( void*) _temp1606; _temp1711->tl= 0; _temp1711;}); _temp1710;}));}}}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({
char* _temp1712=""; struct _tagged_string _temp1713; _temp1713.curr= _temp1712;
_temp1713.base= _temp1712; _temp1713.last_plus_one= _temp1712 + 1; _temp1713;}),({
struct Cyc_List_List* _temp1714=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1714->hd=( void*) _temp1608; _temp1714->tl=({
struct Cyc_List_List* _temp1715=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1715->hd=( void*) _temp1606; _temp1715->tl= 0;
_temp1715;}); _temp1714;}));} goto _LL1563; _LL1571: if( _temp1610 == 0){ s= Cyc_PP_text(({
char* _temp1716="return;"; struct _tagged_string _temp1717; _temp1717.curr=
_temp1716; _temp1717.base= _temp1716; _temp1717.last_plus_one= _temp1716 + 8;
_temp1717;}));} else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp1718="return ";
struct _tagged_string _temp1719; _temp1719.curr= _temp1718; _temp1719.base=
_temp1718; _temp1719.last_plus_one= _temp1718 + 8; _temp1719;})), _temp1610 == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null(
_temp1610)), Cyc_PP_text(({ char* _temp1720=";"; struct _tagged_string _temp1721;
_temp1721.curr= _temp1720; _temp1721.base= _temp1720; _temp1721.last_plus_one=
_temp1720 + 2; _temp1721;})));} goto _LL1563; _LL1573: { int print_else;{ void*
_temp1722=( void*) _temp1612->r; _LL1724: if( _temp1722 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1725;} else{ goto _LL1726;} _LL1726: goto _LL1727; _LL1725: print_else=
0; goto _LL1723; _LL1727: print_else= 1; goto _LL1723; _LL1723:;} s= Cyc_Absynpp_cat7(
Cyc_PP_text(({ char* _temp1728="if ("; struct _tagged_string _temp1729;
_temp1729.curr= _temp1728; _temp1729.base= _temp1728; _temp1729.last_plus_one=
_temp1728 + 5; _temp1729;})), Cyc_Absynpp_exp2doc( _temp1616), Cyc_PP_text(({
char* _temp1730=") {"; struct _tagged_string _temp1731; _temp1731.curr=
_temp1730; _temp1731.base= _temp1730; _temp1731.last_plus_one= _temp1730 + 4;
_temp1731;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp1614))), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp1732="}"; struct
_tagged_string _temp1733; _temp1733.curr= _temp1732; _temp1733.base= _temp1732;
_temp1733.last_plus_one= _temp1732 + 2; _temp1733;})), print_else? Cyc_Absynpp_cat5(
Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp1734="else {"; struct
_tagged_string _temp1735; _temp1735.curr= _temp1734; _temp1735.base= _temp1734;
_temp1735.last_plus_one= _temp1734 + 7; _temp1735;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1612))), Cyc_PP_line_doc(), Cyc_PP_text(({
char* _temp1736="}"; struct _tagged_string _temp1737; _temp1737.curr= _temp1736;
_temp1737.base= _temp1736; _temp1737.last_plus_one= _temp1736 + 2; _temp1737;}))):
Cyc_PP_nil_doc()); goto _LL1563;} _LL1575: s= Cyc_Absynpp_cat6( Cyc_PP_text(({
char* _temp1738="while ("; struct _tagged_string _temp1739; _temp1739.curr=
_temp1738; _temp1739.base= _temp1738; _temp1739.last_plus_one= _temp1738 + 8;
_temp1739;})), Cyc_Absynpp_exp2doc( _temp1624), Cyc_PP_text(({ char* _temp1740=") {";
struct _tagged_string _temp1741; _temp1741.curr= _temp1740; _temp1741.base=
_temp1740; _temp1741.last_plus_one= _temp1740 + 4; _temp1741;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1618))), Cyc_PP_line_doc(),
Cyc_PP_text(({ char* _temp1742="}"; struct _tagged_string _temp1743; _temp1743.curr=
_temp1742; _temp1743.base= _temp1742; _temp1743.last_plus_one= _temp1742 + 2;
_temp1743;}))); goto _LL1563; _LL1577: s= Cyc_PP_text(({ char* _temp1744="break;";
struct _tagged_string _temp1745; _temp1745.curr= _temp1744; _temp1745.base=
_temp1744; _temp1745.last_plus_one= _temp1744 + 7; _temp1745;})); goto _LL1563;
_LL1579: s= Cyc_PP_text(({ char* _temp1746="continue;"; struct _tagged_string
_temp1747; _temp1747.curr= _temp1746; _temp1747.base= _temp1746; _temp1747.last_plus_one=
_temp1746 + 10; _temp1747;})); goto _LL1563; _LL1581: s= Cyc_PP_text(({ struct
_tagged_string _temp1748=* _temp1632; xprintf("goto %.*s;", _temp1748.last_plus_one
- _temp1748.curr, _temp1748.curr);})); goto _LL1563; _LL1583: s= Cyc_Absynpp_cat10(
Cyc_PP_text(({ char* _temp1749="for("; struct _tagged_string _temp1750;
_temp1750.curr= _temp1749; _temp1750.base= _temp1749; _temp1750.last_plus_one=
_temp1749 + 5; _temp1750;})), Cyc_Absynpp_exp2doc( _temp1648), Cyc_PP_text(({
char* _temp1751="; "; struct _tagged_string _temp1752; _temp1752.curr= _temp1751;
_temp1752.base= _temp1751; _temp1752.last_plus_one= _temp1751 + 3; _temp1752;})),
Cyc_Absynpp_exp2doc( _temp1646), Cyc_PP_text(({ char* _temp1753="; "; struct
_tagged_string _temp1754; _temp1754.curr= _temp1753; _temp1754.base= _temp1753;
_temp1754.last_plus_one= _temp1753 + 3; _temp1754;})), Cyc_Absynpp_exp2doc(
_temp1640), Cyc_PP_text(({ char* _temp1755=") {"; struct _tagged_string
_temp1756; _temp1756.curr= _temp1755; _temp1756.base= _temp1755; _temp1756.last_plus_one=
_temp1755 + 4; _temp1756;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1634))), Cyc_PP_line_doc(), Cyc_PP_text(({ char*
_temp1757="}"; struct _tagged_string _temp1758; _temp1758.curr= _temp1757;
_temp1758.base= _temp1757; _temp1758.last_plus_one= _temp1757 + 2; _temp1758;})));
goto _LL1563; _LL1585: s= Cyc_Absynpp_cat7( Cyc_PP_text(({ char* _temp1759="switch (";
struct _tagged_string _temp1760; _temp1760.curr= _temp1759; _temp1760.base=
_temp1759; _temp1760.last_plus_one= _temp1759 + 9; _temp1760;})), Cyc_Absynpp_exp2doc(
_temp1652), Cyc_PP_text(({ char* _temp1761=") {"; struct _tagged_string
_temp1762; _temp1762.curr= _temp1761; _temp1762.base= _temp1761; _temp1762.last_plus_one=
_temp1761 + 4; _temp1762;})), Cyc_PP_line_doc(), Cyc_Absynpp_switchclauses2doc(
_temp1650), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp1763="}"; struct
_tagged_string _temp1764; _temp1764.curr= _temp1763; _temp1764.base= _temp1763;
_temp1764.last_plus_one= _temp1763 + 2; _temp1764;}))); goto _LL1563; _LL1587: s=
Cyc_PP_text(({ char* _temp1765="fallthru;"; struct _tagged_string _temp1766;
_temp1766.curr= _temp1765; _temp1766.base= _temp1765; _temp1766.last_plus_one=
_temp1765 + 10; _temp1766;})); goto _LL1563; _LL1589: s= Cyc_Absynpp_cat3( Cyc_PP_text(({
char* _temp1767="fallthru("; struct _tagged_string _temp1768; _temp1768.curr=
_temp1767; _temp1768.base= _temp1767; _temp1768.last_plus_one= _temp1767 + 10;
_temp1768;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1660), Cyc_PP_text(({ char*
_temp1769=");"; struct _tagged_string _temp1770; _temp1770.curr= _temp1769;
_temp1770.base= _temp1769; _temp1770.last_plus_one= _temp1769 + 3; _temp1770;})));
goto _LL1563; _LL1591: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1664),
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1662)); goto _LL1563; _LL1593: s=
Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp1771="cut "; struct _tagged_string
_temp1772; _temp1772.curr= _temp1771; _temp1772.base= _temp1771; _temp1772.last_plus_one=
_temp1771 + 5; _temp1772;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1666)));
goto _LL1563; _LL1595: s= Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp1773="splice ";
struct _tagged_string _temp1774; _temp1774.curr= _temp1773; _temp1774.base=
_temp1773; _temp1774.last_plus_one= _temp1773 + 8; _temp1774;})), Cyc_PP_nest( 2,
Cyc_Absynpp_stmt2doc( _temp1668))); goto _LL1563; _LL1597: if( Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration( _temp1670): 0){ s= Cyc_Absynpp_cat6( Cyc_Absynpp_textptr(
_temp1672), Cyc_PP_text(({ char* _temp1775=": {"; struct _tagged_string
_temp1776; _temp1776.curr= _temp1775; _temp1776.base= _temp1775; _temp1776.last_plus_one=
_temp1775 + 4; _temp1776;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1670)), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp1777="}"; struct
_tagged_string _temp1778; _temp1778.curr= _temp1777; _temp1778.base= _temp1777;
_temp1778.last_plus_one= _temp1777 + 2; _temp1778;})));} else{ s= Cyc_Absynpp_cat3(
Cyc_Absynpp_textptr( _temp1672), Cyc_PP_text(({ char* _temp1779=": "; struct
_tagged_string _temp1780; _temp1780.curr= _temp1779; _temp1780.base= _temp1779;
_temp1780.last_plus_one= _temp1779 + 3; _temp1780;})), Cyc_Absynpp_stmt2doc(
_temp1670));} goto _LL1563; _LL1599: s= Cyc_Absynpp_cat7( Cyc_PP_text(({ char*
_temp1781="do {"; struct _tagged_string _temp1782; _temp1782.curr= _temp1781;
_temp1782.base= _temp1781; _temp1782.last_plus_one= _temp1781 + 5; _temp1782;})),
Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1680)), Cyc_PP_line_doc(),
Cyc_PP_text(({ char* _temp1783="} while ("; struct _tagged_string _temp1784;
_temp1784.curr= _temp1783; _temp1784.base= _temp1783; _temp1784.last_plus_one=
_temp1783 + 10; _temp1784;})), Cyc_Absynpp_exp2doc( _temp1678), Cyc_PP_text(({
char* _temp1785=");"; struct _tagged_string _temp1786; _temp1786.curr= _temp1785;
_temp1786.base= _temp1785; _temp1786.last_plus_one= _temp1785 + 3; _temp1786;})));
goto _LL1563; _LL1601: s= Cyc_Absynpp_cat9( Cyc_PP_text(({ char* _temp1787="try {";
struct _tagged_string _temp1788; _temp1788.curr= _temp1787; _temp1788.base=
_temp1787; _temp1788.last_plus_one= _temp1787 + 6; _temp1788;})), Cyc_PP_line_doc(),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1684)), Cyc_PP_line_doc(), Cyc_PP_text(({
char* _temp1789="} catch {"; struct _tagged_string _temp1790; _temp1790.curr=
_temp1789; _temp1790.base= _temp1789; _temp1790.last_plus_one= _temp1789 + 10;
_temp1790;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1682)), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp1791="}"; struct
_tagged_string _temp1792; _temp1792.curr= _temp1791; _temp1792.base= _temp1791;
_temp1792.last_plus_one= _temp1791 + 2; _temp1792;}))); goto _LL1563; _LL1603: s=
Cyc_Absynpp_cat9( Cyc_PP_text(({ char* _temp1793="region<"; struct
_tagged_string _temp1794; _temp1794.curr= _temp1793; _temp1794.base= _temp1793;
_temp1794.last_plus_one= _temp1793 + 8; _temp1794;})), Cyc_Absynpp_textptr( Cyc_Absynpp_get_name(
_temp1690)), Cyc_PP_text(({ char* _temp1795=">"; struct _tagged_string _temp1796;
_temp1796.curr= _temp1795; _temp1796.base= _temp1795; _temp1796.last_plus_one=
_temp1795 + 2; _temp1796;})), Cyc_Absynpp_qvar2doc( _temp1688->name), Cyc_PP_text(({
char* _temp1797="{"; struct _tagged_string _temp1798; _temp1798.curr= _temp1797;
_temp1798.base= _temp1797; _temp1798.last_plus_one= _temp1797 + 2; _temp1798;})),
Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1686)), Cyc_PP_line_doc(),
Cyc_PP_text(({ char* _temp1799="}"; struct _tagged_string _temp1800; _temp1800.curr=
_temp1799; _temp1800.base= _temp1799; _temp1800.last_plus_one= _temp1799 + 2;
_temp1800;}))); goto _LL1563; _LL1563:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1801=( void*) p->r;
int _temp1833; void* _temp1835; char _temp1837; struct _tagged_string _temp1839;
struct Cyc_Absyn_Vardecl* _temp1841; struct Cyc_List_List* _temp1843; struct Cyc_Absyn_Pat*
_temp1845; struct Cyc_Absyn_Vardecl* _temp1847; struct _tuple0* _temp1849;
struct Cyc_List_List* _temp1851; struct Cyc_List_List* _temp1853; struct _tuple0*
_temp1855; struct Cyc_List_List* _temp1857; struct Cyc_List_List* _temp1859;
struct _tuple0* _temp1861; struct Cyc_List_List* _temp1863; struct Cyc_List_List*
_temp1865; struct Cyc_Core_Opt* _temp1867; struct Cyc_Absyn_Structdecl*
_temp1869; struct Cyc_Absyn_Enumfield* _temp1871; struct Cyc_Absyn_Enumdecl*
_temp1873; struct Cyc_List_List* _temp1875; struct Cyc_List_List* _temp1877;
struct Cyc_Absyn_Tunionfield* _temp1879; struct Cyc_Absyn_Tuniondecl* _temp1881;
_LL1803: if( _temp1801 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1804;} else{ goto
_LL1805;} _LL1805: if( _temp1801 ==( void*) Cyc_Absyn_Null_p){ goto _LL1806;}
else{ goto _LL1807;} _LL1807: if(( unsigned int) _temp1801 > 2u?*(( int*)
_temp1801) == Cyc_Absyn_Int_p: 0){ _LL1836: _temp1835=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp1801)->f1; goto _LL1834; _LL1834: _temp1833=(( struct Cyc_Absyn_Int_p_struct*)
_temp1801)->f2; goto _LL1808;} else{ goto _LL1809;} _LL1809: if(( unsigned int)
_temp1801 > 2u?*(( int*) _temp1801) == Cyc_Absyn_Char_p: 0){ _LL1838: _temp1837=((
struct Cyc_Absyn_Char_p_struct*) _temp1801)->f1; goto _LL1810;} else{ goto
_LL1811;} _LL1811: if(( unsigned int) _temp1801 > 2u?*(( int*) _temp1801) == Cyc_Absyn_Float_p:
0){ _LL1840: _temp1839=(( struct Cyc_Absyn_Float_p_struct*) _temp1801)->f1; goto
_LL1812;} else{ goto _LL1813;} _LL1813: if(( unsigned int) _temp1801 > 2u?*((
int*) _temp1801) == Cyc_Absyn_Var_p: 0){ _LL1842: _temp1841=(( struct Cyc_Absyn_Var_p_struct*)
_temp1801)->f1; goto _LL1814;} else{ goto _LL1815;} _LL1815: if(( unsigned int)
_temp1801 > 2u?*(( int*) _temp1801) == Cyc_Absyn_Tuple_p: 0){ _LL1844: _temp1843=((
struct Cyc_Absyn_Tuple_p_struct*) _temp1801)->f1; goto _LL1816;} else{ goto
_LL1817;} _LL1817: if(( unsigned int) _temp1801 > 2u?*(( int*) _temp1801) == Cyc_Absyn_Pointer_p:
0){ _LL1846: _temp1845=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1801)->f1;
goto _LL1818;} else{ goto _LL1819;} _LL1819: if(( unsigned int) _temp1801 > 2u?*((
int*) _temp1801) == Cyc_Absyn_Reference_p: 0){ _LL1848: _temp1847=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1801)->f1; goto _LL1820;} else{ goto _LL1821;} _LL1821: if(( unsigned int)
_temp1801 > 2u?*(( int*) _temp1801) == Cyc_Absyn_UnknownId_p: 0){ _LL1850:
_temp1849=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1801)->f1; goto _LL1822;}
else{ goto _LL1823;} _LL1823: if(( unsigned int) _temp1801 > 2u?*(( int*)
_temp1801) == Cyc_Absyn_UnknownCall_p: 0){ _LL1856: _temp1855=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1801)->f1; goto _LL1854; _LL1854: _temp1853=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1801)->f2; goto _LL1852; _LL1852: _temp1851=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1801)->f3; goto _LL1824;} else{ goto _LL1825;} _LL1825: if(( unsigned int)
_temp1801 > 2u?*(( int*) _temp1801) == Cyc_Absyn_UnknownFields_p: 0){ _LL1862:
_temp1861=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1801)->f1; goto
_LL1860; _LL1860: _temp1859=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1801)->f2; goto _LL1858; _LL1858: _temp1857=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1801)->f3; goto _LL1826;} else{ goto _LL1827;} _LL1827: if(( unsigned int)
_temp1801 > 2u?*(( int*) _temp1801) == Cyc_Absyn_Struct_p: 0){ _LL1870:
_temp1869=(( struct Cyc_Absyn_Struct_p_struct*) _temp1801)->f1; goto _LL1868;
_LL1868: _temp1867=(( struct Cyc_Absyn_Struct_p_struct*) _temp1801)->f2; goto
_LL1866; _LL1866: _temp1865=(( struct Cyc_Absyn_Struct_p_struct*) _temp1801)->f3;
goto _LL1864; _LL1864: _temp1863=(( struct Cyc_Absyn_Struct_p_struct*) _temp1801)->f4;
goto _LL1828;} else{ goto _LL1829;} _LL1829: if(( unsigned int) _temp1801 > 2u?*((
int*) _temp1801) == Cyc_Absyn_Enum_p: 0){ _LL1874: _temp1873=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1801)->f1; goto _LL1872; _LL1872: _temp1871=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1801)->f2; goto _LL1830;} else{ goto _LL1831;} _LL1831: if(( unsigned int)
_temp1801 > 2u?*(( int*) _temp1801) == Cyc_Absyn_Tunion_p: 0){ _LL1882:
_temp1881=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1801)->f1; goto _LL1880;
_LL1880: _temp1879=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1801)->f2; goto
_LL1878; _LL1878: _temp1877=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1801)->f3;
goto _LL1876; _LL1876: _temp1875=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1801)->f4;
goto _LL1832;} else{ goto _LL1802;} _LL1804: s= Cyc_PP_text(({ char* _temp1883="_";
struct _tagged_string _temp1884; _temp1884.curr= _temp1883; _temp1884.base=
_temp1883; _temp1884.last_plus_one= _temp1883 + 2; _temp1884;})); goto _LL1802;
_LL1806: s= Cyc_PP_text(({ char* _temp1885="null"; struct _tagged_string
_temp1886; _temp1886.curr= _temp1885; _temp1886.base= _temp1885; _temp1886.last_plus_one=
_temp1885 + 5; _temp1886;})); goto _LL1802; _LL1808: if( _temp1835 ==( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d", _temp1833));} else{ s= Cyc_PP_text(
xprintf("%u",( unsigned int) _temp1833));} goto _LL1802; _LL1810: s= Cyc_PP_text(({
struct _tagged_string _temp1887= Cyc_Absynpp_char_escape( _temp1837); xprintf("'%.*s'",
_temp1887.last_plus_one - _temp1887.curr, _temp1887.curr);})); goto _LL1802;
_LL1812: s= Cyc_PP_text( _temp1839); goto _LL1802; _LL1814: s= Cyc_Absynpp_qvar2doc(
_temp1841->name); goto _LL1802; _LL1816: s= Cyc_Absynpp_cat3( Cyc_PP_text(({
char* _temp1888="$("; struct _tagged_string _temp1889; _temp1889.curr= _temp1888;
_temp1889.base= _temp1888; _temp1889.last_plus_one= _temp1888 + 3; _temp1889;})),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,({
char* _temp1890=","; struct _tagged_string _temp1891; _temp1891.curr= _temp1890;
_temp1891.base= _temp1890; _temp1891.last_plus_one= _temp1890 + 2; _temp1891;}),
_temp1843), Cyc_PP_text(({ char* _temp1892=")"; struct _tagged_string _temp1893;
_temp1893.curr= _temp1892; _temp1893.base= _temp1892; _temp1893.last_plus_one=
_temp1892 + 2; _temp1893;}))); goto _LL1802; _LL1818: s= Cyc_Absynpp_cat2( Cyc_PP_text(({
char* _temp1894="&"; struct _tagged_string _temp1895; _temp1895.curr= _temp1894;
_temp1895.base= _temp1894; _temp1895.last_plus_one= _temp1894 + 2; _temp1895;})),
Cyc_Absynpp_pat2doc( _temp1845)); goto _LL1802; _LL1820: s= Cyc_Absynpp_cat2(
Cyc_PP_text(({ char* _temp1896="*"; struct _tagged_string _temp1897; _temp1897.curr=
_temp1896; _temp1897.base= _temp1896; _temp1897.last_plus_one= _temp1896 + 2;
_temp1897;})), Cyc_Absynpp_qvar2doc( _temp1847->name)); goto _LL1802; _LL1822: s=
Cyc_Absynpp_qvar2doc( _temp1849); goto _LL1802; _LL1824: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1855), Cyc_Absynpp_tvars2doc( _temp1853), Cyc_Absynpp_group(({
char* _temp1898="("; struct _tagged_string _temp1899; _temp1899.curr= _temp1898;
_temp1899.base= _temp1898; _temp1899.last_plus_one= _temp1898 + 2; _temp1899;}),({
char* _temp1900=")"; struct _tagged_string _temp1901; _temp1901.curr= _temp1900;
_temp1901.base= _temp1900; _temp1901.last_plus_one= _temp1900 + 2; _temp1901;}),({
char* _temp1902=","; struct _tagged_string _temp1903; _temp1903.curr= _temp1902;
_temp1903.base= _temp1902; _temp1903.last_plus_one= _temp1902 + 2; _temp1903;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp1851))); goto
_LL1802; _LL1826: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1861), Cyc_Absynpp_tvars2doc(
_temp1859), Cyc_Absynpp_group(({ char* _temp1904="{"; struct _tagged_string
_temp1905; _temp1905.curr= _temp1904; _temp1905.base= _temp1904; _temp1905.last_plus_one=
_temp1904 + 2; _temp1905;}),({ char* _temp1906="}"; struct _tagged_string
_temp1907; _temp1907.curr= _temp1906; _temp1907.base= _temp1906; _temp1907.last_plus_one=
_temp1906 + 2; _temp1907;}),({ char* _temp1908=","; struct _tagged_string
_temp1909; _temp1909.curr= _temp1908; _temp1909.base= _temp1908; _temp1909.last_plus_one=
_temp1908 + 2; _temp1909;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1857))); goto _LL1802; _LL1828: s= Cyc_Absynpp_cat3( _temp1869->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp1869->name))->v), Cyc_Absynpp_tvars2doc( _temp1865), Cyc_Absynpp_group(({
char* _temp1910="{"; struct _tagged_string _temp1911; _temp1911.curr= _temp1910;
_temp1911.base= _temp1910; _temp1911.last_plus_one= _temp1910 + 2; _temp1911;}),({
char* _temp1912="}"; struct _tagged_string _temp1913; _temp1913.curr= _temp1912;
_temp1913.base= _temp1912; _temp1913.last_plus_one= _temp1912 + 2; _temp1913;}),({
char* _temp1914=","; struct _tagged_string _temp1915; _temp1915.curr= _temp1914;
_temp1915.base= _temp1914; _temp1915.last_plus_one= _temp1914 + 2; _temp1915;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_dp2doc, _temp1863))); goto _LL1802; _LL1830: s=
Cyc_Absynpp_qvar2doc( _temp1871->name); goto _LL1802; _LL1832: if( _temp1875 ==
0){ s= Cyc_Absynpp_qvar2doc( _temp1879->name);} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1879->name), Cyc_Absynpp_tvars2doc( _temp1877), Cyc_Absynpp_egroup(({ char*
_temp1916="("; struct _tagged_string _temp1917; _temp1917.curr= _temp1916;
_temp1917.base= _temp1916; _temp1917.last_plus_one= _temp1916 + 2; _temp1917;}),({
char* _temp1918=")"; struct _tagged_string _temp1919; _temp1919.curr= _temp1918;
_temp1919.base= _temp1918; _temp1919.last_plus_one= _temp1918 + 2; _temp1919;}),({
char* _temp1920=","; struct _tagged_string _temp1921; _temp1921.curr= _temp1920;
_temp1921.base= _temp1920; _temp1921.last_plus_one= _temp1920 + 2; _temp1921;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp1875)));}
goto _LL1802; _LL1802:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc(
struct _tuple5* dp){ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup(({ char*
_temp1922=""; struct _tagged_string _temp1923; _temp1923.curr= _temp1922;
_temp1923.base= _temp1922; _temp1923.last_plus_one= _temp1922 + 1; _temp1923;}),({
char* _temp1924="="; struct _tagged_string _temp1925; _temp1925.curr= _temp1924;
_temp1925.base= _temp1924; _temp1925.last_plus_one= _temp1924 + 2; _temp1925;}),({
char* _temp1926="="; struct _tagged_string _temp1927; _temp1927.curr= _temp1926;
_temp1927.base= _temp1926; _temp1927.last_plus_one= _temp1926 + 2; _temp1927;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1)), Cyc_Absynpp_pat2doc((*
dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc( struct Cyc_Absyn_Switch_clause*
c){ if( c->where_clause == 0?( void*)( c->pattern)->r ==( void*) Cyc_Absyn_Wild_p:
0){ return Cyc_Absynpp_cat2( Cyc_PP_text(({ char* _temp1928="default: "; struct
_tagged_string _temp1929; _temp1929.curr= _temp1928; _temp1929.base= _temp1928;
_temp1929.last_plus_one= _temp1928 + 10; _temp1929;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause
== 0){ return Cyc_Absynpp_cat4( Cyc_PP_text(({ char* _temp1930="case "; struct
_tagged_string _temp1931; _temp1931.curr= _temp1930; _temp1931.base= _temp1930;
_temp1931.last_plus_one= _temp1930 + 6; _temp1931;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(({ char* _temp1932=": "; struct _tagged_string _temp1933; _temp1933.curr=
_temp1932; _temp1933.base= _temp1932; _temp1933.last_plus_one= _temp1932 + 3;
_temp1933;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
c->body))));} else{ return Cyc_Absynpp_cat6( Cyc_PP_text(({ char* _temp1934="case ";
struct _tagged_string _temp1935; _temp1935.curr= _temp1934; _temp1935.base=
_temp1934; _temp1935.last_plus_one= _temp1934 + 6; _temp1935;})), Cyc_Absynpp_pat2doc(
c->pattern), Cyc_PP_text(({ char* _temp1936=" && "; struct _tagged_string
_temp1937; _temp1937.curr= _temp1936; _temp1937.base= _temp1936; _temp1937.last_plus_one=
_temp1936 + 5; _temp1937;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( c->where_clause)), Cyc_PP_text(({ char* _temp1938=": "; struct
_tagged_string _temp1939; _temp1939.curr= _temp1938; _temp1939.base= _temp1938;
_temp1939.last_plus_one= _temp1938 + 3; _temp1939;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,({ char* _temp1940=""; struct
_tagged_string _temp1941; _temp1941.curr= _temp1940; _temp1941.base= _temp1940;
_temp1941.last_plus_one= _temp1940 + 1; _temp1941;}), cs);} struct Cyc_PP_Doc*
Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield* f){ if( f->tag == 0){
return Cyc_Absynpp_qvar2doc( f->name);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
f->name), Cyc_PP_text(({ char* _temp1942=" = "; struct _tagged_string _temp1943;
_temp1943.curr= _temp1942; _temp1943.base= _temp1942; _temp1943.last_plus_one=
_temp1942 + 4; _temp1943;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( f->tag)));}} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct
Cyc_List_List* fs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Enumfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,({ char* _temp1944=","; struct
_tagged_string _temp1945; _temp1945.curr= _temp1944; _temp1945.base= _temp1944;
_temp1945.last_plus_one= _temp1944 + 2; _temp1945;}), fs);} static struct Cyc_PP_Doc*
Cyc_Absynpp_id2doc( struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);}
static struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){
return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,({
char* _temp1946=","; struct _tagged_string _temp1947; _temp1947.curr= _temp1946;
_temp1947.base= _temp1946; _temp1947.last_plus_one= _temp1946 + 2; _temp1947;}),
vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl* d){
struct Cyc_PP_Doc* s;{ void* _temp1948=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp1974; struct Cyc_Absyn_Structdecl* _temp1976; struct Cyc_Absyn_Uniondecl*
_temp1978; struct Cyc_Absyn_Vardecl* _temp1980; struct Cyc_Absyn_Vardecl
_temp1982; struct Cyc_List_List* _temp1983; struct Cyc_Core_Opt* _temp1985;
struct Cyc_Absyn_Exp* _temp1987; void* _temp1989; struct Cyc_Absyn_Tqual
_temp1991; struct _tuple0* _temp1993; void* _temp1995; struct Cyc_Absyn_Tuniondecl*
_temp1997; struct Cyc_Absyn_Tuniondecl _temp1999; int _temp2000; struct Cyc_Core_Opt*
_temp2002; struct Cyc_List_List* _temp2004; struct _tuple0* _temp2006; void*
_temp2008; int _temp2010; struct Cyc_Absyn_Exp* _temp2012; struct Cyc_Core_Opt*
_temp2014; struct Cyc_Core_Opt* _temp2016; struct Cyc_Absyn_Pat* _temp2018;
struct Cyc_List_List* _temp2020; struct Cyc_Absyn_Enumdecl* _temp2022; struct
Cyc_Absyn_Enumdecl _temp2024; struct Cyc_Core_Opt* _temp2025; struct _tuple0*
_temp2027; void* _temp2029; struct Cyc_Absyn_Typedefdecl* _temp2031; struct Cyc_List_List*
_temp2033; struct _tagged_string* _temp2035; struct Cyc_List_List* _temp2037;
struct _tuple0* _temp2039; struct Cyc_List_List* _temp2041; _LL1950: if(*(( int*)
_temp1948) == Cyc_Absyn_Fn_d){ _LL1975: _temp1974=(( struct Cyc_Absyn_Fn_d_struct*)
_temp1948)->f1; goto _LL1951;} else{ goto _LL1952;} _LL1952: if(*(( int*)
_temp1948) == Cyc_Absyn_Struct_d){ _LL1977: _temp1976=(( struct Cyc_Absyn_Struct_d_struct*)
_temp1948)->f1; goto _LL1953;} else{ goto _LL1954;} _LL1954: if(*(( int*)
_temp1948) == Cyc_Absyn_Union_d){ _LL1979: _temp1978=(( struct Cyc_Absyn_Union_d_struct*)
_temp1948)->f1; goto _LL1955;} else{ goto _LL1956;} _LL1956: if(*(( int*)
_temp1948) == Cyc_Absyn_Var_d){ _LL1981: _temp1980=(( struct Cyc_Absyn_Var_d_struct*)
_temp1948)->f1; _temp1982=* _temp1980; _LL1996: _temp1995=( void*) _temp1982.sc;
goto _LL1994; _LL1994: _temp1993= _temp1982.name; goto _LL1992; _LL1992:
_temp1991= _temp1982.tq; goto _LL1990; _LL1990: _temp1989=( void*) _temp1982.type;
goto _LL1988; _LL1988: _temp1987= _temp1982.initializer; goto _LL1986; _LL1986:
_temp1985= _temp1982.rgn; goto _LL1984; _LL1984: _temp1983= _temp1982.attributes;
goto _LL1957;} else{ goto _LL1958;} _LL1958: if(*(( int*) _temp1948) == Cyc_Absyn_Tunion_d){
_LL1998: _temp1997=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1948)->f1;
_temp1999=* _temp1997; _LL2009: _temp2008=( void*) _temp1999.sc; goto _LL2007;
_LL2007: _temp2006= _temp1999.name; goto _LL2005; _LL2005: _temp2004= _temp1999.tvs;
goto _LL2003; _LL2003: _temp2002= _temp1999.fields; goto _LL2001; _LL2001:
_temp2000= _temp1999.is_xtunion; goto _LL1959;} else{ goto _LL1960;} _LL1960:
if(*(( int*) _temp1948) == Cyc_Absyn_Let_d){ _LL2019: _temp2018=(( struct Cyc_Absyn_Let_d_struct*)
_temp1948)->f1; goto _LL2017; _LL2017: _temp2016=(( struct Cyc_Absyn_Let_d_struct*)
_temp1948)->f2; goto _LL2015; _LL2015: _temp2014=(( struct Cyc_Absyn_Let_d_struct*)
_temp1948)->f3; goto _LL2013; _LL2013: _temp2012=(( struct Cyc_Absyn_Let_d_struct*)
_temp1948)->f4; goto _LL2011; _LL2011: _temp2010=(( struct Cyc_Absyn_Let_d_struct*)
_temp1948)->f5; goto _LL1961;} else{ goto _LL1962;} _LL1962: if(*(( int*)
_temp1948) == Cyc_Absyn_Letv_d){ _LL2021: _temp2020=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1948)->f1; goto _LL1963;} else{ goto _LL1964;} _LL1964: if(*(( int*)
_temp1948) == Cyc_Absyn_Enum_d){ _LL2023: _temp2022=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1948)->f1; _temp2024=* _temp2022; _LL2030: _temp2029=( void*) _temp2024.sc;
goto _LL2028; _LL2028: _temp2027= _temp2024.name; goto _LL2026; _LL2026:
_temp2025= _temp2024.fields; goto _LL1965;} else{ goto _LL1966;} _LL1966: if(*((
int*) _temp1948) == Cyc_Absyn_Typedef_d){ _LL2032: _temp2031=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1948)->f1; goto _LL1967;} else{ goto _LL1968;} _LL1968: if(*(( int*)
_temp1948) == Cyc_Absyn_Namespace_d){ _LL2036: _temp2035=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1948)->f1; goto _LL2034; _LL2034: _temp2033=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1948)->f2; goto _LL1969;} else{ goto _LL1970;} _LL1970: if(*(( int*)
_temp1948) == Cyc_Absyn_Using_d){ _LL2040: _temp2039=(( struct Cyc_Absyn_Using_d_struct*)
_temp1948)->f1; goto _LL2038; _LL2038: _temp2037=(( struct Cyc_Absyn_Using_d_struct*)
_temp1948)->f2; goto _LL1971;} else{ goto _LL1972;} _LL1972: if(*(( int*)
_temp1948) == Cyc_Absyn_ExternC_d){ _LL2042: _temp2041=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1948)->f1; goto _LL1973;} else{ goto _LL1949;} _LL1951: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2055=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2055[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2056; _temp2056.tag= Cyc_Absyn_FnType; _temp2056.f1=({ struct Cyc_Absyn_FnInfo
_temp2057; _temp2057.tvars= _temp1974->tvs; _temp2057.effect= _temp1974->effect;
_temp2057.ret_typ=( void*)(( void*) _temp1974->ret_type); _temp2057.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1974->args); _temp2057.varargs=
_temp1974->varargs; _temp2057.attributes= 0; _temp2057;}); _temp2056;});
_temp2055;}); s= Cyc_Absynpp_cat7( Cyc_PP_text( _temp1974->is_inline? Cyc_Absynpp_to_VC?({
char* _temp2043="__inline "; struct _tagged_string _temp2044; _temp2044.curr=
_temp2043; _temp2044.base= _temp2043; _temp2044.last_plus_one= _temp2043 + 10;
_temp2044;}):({ char* _temp2045="inline "; struct _tagged_string _temp2046;
_temp2046.curr= _temp2045; _temp2046.base= _temp2045; _temp2046.last_plus_one=
_temp2045 + 8; _temp2046;}):({ char* _temp2047=""; struct _tagged_string
_temp2048; _temp2048.curr= _temp2047; _temp2048.base= _temp2047; _temp2048.last_plus_one=
_temp2047 + 1; _temp2048;})), Cyc_Absynpp_scope2doc(( void*) _temp1974->sc), Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual _temp2049; _temp2049.q_const= 0; _temp2049.q_volatile= 0;
_temp2049.q_restrict= 0; _temp2049;}), t,({ struct Cyc_Core_Opt* _temp2050=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2050->v=(
void*) Cyc_Absynpp_cat2( Cyc_Absynpp_atts2doc( _temp1974->attributes), Cyc_Absynpp_typedef_name2doc(
_temp1974->name)); _temp2050;})), Cyc_PP_text(({ char* _temp2051=" {"; struct
_tagged_string _temp2052; _temp2052.curr= _temp2051; _temp2052.base= _temp2051;
_temp2052.last_plus_one= _temp2051 + 3; _temp2052;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1974->body))), Cyc_PP_line_doc(),
Cyc_PP_text(({ char* _temp2053="}"; struct _tagged_string _temp2054; _temp2054.curr=
_temp2053; _temp2054.base= _temp2053; _temp2054.last_plus_one= _temp2053 + 2;
_temp2054;}))); goto _LL1949;} _LL1953: if( _temp1976->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp1976->sc), Cyc_PP_text(({ char* _temp2058="struct ";
struct _tagged_string _temp2059; _temp2059.curr= _temp2058; _temp2059.base=
_temp2058; _temp2059.last_plus_one= _temp2058 + 8; _temp2059;})), _temp1976->name
== 0? Cyc_PP_text(({ char* _temp2060=""; struct _tagged_string _temp2061;
_temp2061.curr= _temp2060; _temp2061.base= _temp2060; _temp2061.last_plus_one=
_temp2060 + 1; _temp2061;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1976->name))->v), Cyc_Absynpp_ktvars2doc(
_temp1976->tvs), Cyc_PP_text(({ char* _temp2062=";"; struct _tagged_string
_temp2063; _temp2063.curr= _temp2062; _temp2063.base= _temp2062; _temp2063.last_plus_one=
_temp2062 + 2; _temp2063;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp1976->sc), Cyc_PP_text(({ char* _temp2064="struct "; struct
_tagged_string _temp2065; _temp2065.curr= _temp2064; _temp2065.base= _temp2064;
_temp2065.last_plus_one= _temp2064 + 8; _temp2065;})), _temp1976->name == 0? Cyc_PP_text(({
char* _temp2066=""; struct _tagged_string _temp2067; _temp2067.curr= _temp2066;
_temp2067.base= _temp2066; _temp2067.last_plus_one= _temp2066 + 1; _temp2067;})):
Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp1976->name))->v), Cyc_Absynpp_ktvars2doc( _temp1976->tvs), Cyc_PP_text(({
char* _temp2068=" {"; struct _tagged_string _temp2069; _temp2069.curr= _temp2068;
_temp2069.base= _temp2068; _temp2069.last_plus_one= _temp2068 + 3; _temp2069;})),
Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1976->fields))->v))),
Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp2070="}"; struct _tagged_string
_temp2071; _temp2071.curr= _temp2070; _temp2071.base= _temp2070; _temp2071.last_plus_one=
_temp2070 + 2; _temp2071;})), Cyc_Absynpp_atts2doc( _temp1976->attributes), Cyc_PP_text(({
char* _temp2072=";"; struct _tagged_string _temp2073; _temp2073.curr= _temp2072;
_temp2073.base= _temp2072; _temp2073.last_plus_one= _temp2072 + 2; _temp2073;})));}
goto _LL1949; _LL1955: if( _temp1978->fields == 0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc((
void*) _temp1978->sc), Cyc_PP_text(({ char* _temp2074="union "; struct
_tagged_string _temp2075; _temp2075.curr= _temp2074; _temp2075.base= _temp2074;
_temp2075.last_plus_one= _temp2074 + 7; _temp2075;})), _temp1978->name == 0? Cyc_PP_text(({
char* _temp2076=""; struct _tagged_string _temp2077; _temp2077.curr= _temp2076;
_temp2077.base= _temp2076; _temp2077.last_plus_one= _temp2076 + 1; _temp2077;})):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1978->name))->v), Cyc_Absynpp_tvars2doc( _temp1978->tvs), Cyc_PP_text(({
char* _temp2078=";"; struct _tagged_string _temp2079; _temp2079.curr= _temp2078;
_temp2079.base= _temp2078; _temp2079.last_plus_one= _temp2078 + 2; _temp2079;})));}
else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc(( void*) _temp1978->sc), Cyc_PP_text(({
char* _temp2080="union "; struct _tagged_string _temp2081; _temp2081.curr=
_temp2080; _temp2081.base= _temp2080; _temp2081.last_plus_one= _temp2080 + 7;
_temp2081;})), _temp1978->name == 0? Cyc_PP_text(({ char* _temp2082=""; struct
_tagged_string _temp2083; _temp2083.curr= _temp2082; _temp2083.base= _temp2082;
_temp2083.last_plus_one= _temp2082 + 1; _temp2083;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1978->name))->v), Cyc_Absynpp_tvars2doc(
_temp1978->tvs), Cyc_PP_text(({ char* _temp2084=" {"; struct _tagged_string
_temp2085; _temp2085.curr= _temp2084; _temp2085.base= _temp2084; _temp2085.last_plus_one=
_temp2084 + 3; _temp2085;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1978->fields))->v))), Cyc_PP_line_doc(), Cyc_PP_text(({ char*
_temp2086="}"; struct _tagged_string _temp2087; _temp2087.curr= _temp2086;
_temp2087.base= _temp2086; _temp2087.last_plus_one= _temp2086 + 2; _temp2087;})),
Cyc_Absynpp_atts2doc( _temp1978->attributes), Cyc_PP_text(({ char* _temp2088=";";
struct _tagged_string _temp2089; _temp2089.curr= _temp2088; _temp2089.base=
_temp2088; _temp2089.last_plus_one= _temp2088 + 2; _temp2089;})));} goto _LL1949;
_LL1957: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc( _temp1993); s=
Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc( _temp1995), Cyc_Absynpp_tqtd2doc(
_temp1991, _temp1989,({ struct Cyc_Core_Opt* _temp2090=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2090->v=( void*) sn; _temp2090;})),
Cyc_Absynpp_atts2doc( _temp1983), _temp1987 == 0? Cyc_PP_text(({ char* _temp2091="";
struct _tagged_string _temp2092; _temp2092.curr= _temp2091; _temp2092.base=
_temp2091; _temp2092.last_plus_one= _temp2091 + 1; _temp2092;})): Cyc_Absynpp_cat2(
Cyc_PP_text(({ char* _temp2093=" = "; struct _tagged_string _temp2094; _temp2094.curr=
_temp2093; _temp2094.base= _temp2093; _temp2094.last_plus_one= _temp2093 + 4;
_temp2094;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null(
_temp1987))), Cyc_PP_text(({ char* _temp2095=";"; struct _tagged_string
_temp2096; _temp2096.curr= _temp2095; _temp2096.base= _temp2095; _temp2096.last_plus_one=
_temp2095 + 2; _temp2096;}))); goto _LL1949;} _LL1959: if( _temp2002 == 0){ s=
Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc( _temp2008), _temp2000? Cyc_PP_text(({
char* _temp2097="xtunion "; struct _tagged_string _temp2098; _temp2098.curr=
_temp2097; _temp2098.base= _temp2097; _temp2098.last_plus_one= _temp2097 + 9;
_temp2098;})): Cyc_PP_text(({ char* _temp2099="tunion "; struct _tagged_string
_temp2100; _temp2100.curr= _temp2099; _temp2100.base= _temp2099; _temp2100.last_plus_one=
_temp2099 + 8; _temp2100;})), _temp2000? Cyc_Absynpp_qvar2doc( _temp2006): Cyc_Absynpp_typedef_name2doc(
_temp2006), Cyc_Absynpp_ktvars2doc( _temp2004), Cyc_PP_text(({ char* _temp2101=";";
struct _tagged_string _temp2102; _temp2102.curr= _temp2101; _temp2102.base=
_temp2101; _temp2102.last_plus_one= _temp2101 + 2; _temp2102;})));} else{ s= Cyc_Absynpp_cat8(
Cyc_Absynpp_scope2doc( _temp2008), _temp2000? Cyc_PP_text(({ char* _temp2103="xtunion ";
struct _tagged_string _temp2104; _temp2104.curr= _temp2103; _temp2104.base=
_temp2103; _temp2104.last_plus_one= _temp2103 + 9; _temp2104;})): Cyc_PP_text(({
char* _temp2105="tunion "; struct _tagged_string _temp2106; _temp2106.curr=
_temp2105; _temp2106.base= _temp2105; _temp2106.last_plus_one= _temp2105 + 8;
_temp2106;})), _temp2000? Cyc_Absynpp_qvar2doc( _temp2006): Cyc_Absynpp_typedef_name2doc(
_temp2006), Cyc_Absynpp_ktvars2doc( _temp2004), Cyc_PP_text(({ char* _temp2107=" {";
struct _tagged_string _temp2108; _temp2108.curr= _temp2107; _temp2108.base=
_temp2107; _temp2108.last_plus_one= _temp2107 + 3; _temp2108;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2002))->v))), Cyc_PP_line_doc(), Cyc_PP_text(({
char* _temp2109="};"; struct _tagged_string _temp2110; _temp2110.curr= _temp2109;
_temp2110.base= _temp2109; _temp2110.last_plus_one= _temp2109 + 3; _temp2110;})));}
goto _LL1949; _LL1961: s= Cyc_Absynpp_cat5( Cyc_PP_text(({ char* _temp2111="let ";
struct _tagged_string _temp2112; _temp2112.curr= _temp2111; _temp2112.base=
_temp2111; _temp2112.last_plus_one= _temp2111 + 5; _temp2112;})), Cyc_Absynpp_pat2doc(
_temp2018), Cyc_PP_text(({ char* _temp2113=" = "; struct _tagged_string
_temp2114; _temp2114.curr= _temp2113; _temp2114.base= _temp2113; _temp2114.last_plus_one=
_temp2113 + 4; _temp2114;})), Cyc_Absynpp_exp2doc( _temp2012), Cyc_PP_text(({
char* _temp2115=";"; struct _tagged_string _temp2116; _temp2116.curr= _temp2115;
_temp2116.base= _temp2115; _temp2116.last_plus_one= _temp2115 + 2; _temp2116;})));
goto _LL1949; _LL1963: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp2117="let ";
struct _tagged_string _temp2118; _temp2118.curr= _temp2117; _temp2118.base=
_temp2117; _temp2118.last_plus_one= _temp2117 + 5; _temp2118;})), Cyc_Absynpp_ids2doc(
_temp2020), Cyc_PP_text(({ char* _temp2119=";"; struct _tagged_string _temp2120;
_temp2120.curr= _temp2119; _temp2120.base= _temp2119; _temp2120.last_plus_one=
_temp2119 + 2; _temp2120;}))); goto _LL1949; _LL1965: if( _temp2025 == 0){ s=
Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc( _temp2029), Cyc_PP_text(({ char*
_temp2121="enum "; struct _tagged_string _temp2122; _temp2122.curr= _temp2121;
_temp2122.base= _temp2121; _temp2122.last_plus_one= _temp2121 + 6; _temp2122;})),
Cyc_Absynpp_typedef_name2doc( _temp2027), Cyc_PP_text(({ char* _temp2123=";";
struct _tagged_string _temp2124; _temp2124.curr= _temp2123; _temp2124.base=
_temp2123; _temp2124.last_plus_one= _temp2123 + 2; _temp2124;})));} else{ s= Cyc_Absynpp_cat7(
Cyc_Absynpp_scope2doc( _temp2029), Cyc_PP_text(({ char* _temp2125="enum ";
struct _tagged_string _temp2126; _temp2126.curr= _temp2125; _temp2126.base=
_temp2125; _temp2126.last_plus_one= _temp2125 + 6; _temp2126;})), Cyc_Absynpp_qvar2doc(
_temp2027), Cyc_PP_text(({ char* _temp2127=" {"; struct _tagged_string _temp2128;
_temp2128.curr= _temp2127; _temp2128.base= _temp2127; _temp2128.last_plus_one=
_temp2127 + 3; _temp2128;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2025))->v))), Cyc_PP_line_doc(), Cyc_PP_text(({ char*
_temp2129="};"; struct _tagged_string _temp2130; _temp2130.curr= _temp2129;
_temp2130.base= _temp2129; _temp2130.last_plus_one= _temp2129 + 3; _temp2130;})));}
goto _LL1949; _LL1967: s= Cyc_Absynpp_cat3( Cyc_PP_text(({ char* _temp2131="typedef ";
struct _tagged_string _temp2132; _temp2132.curr= _temp2131; _temp2132.base=
_temp2131; _temp2132.last_plus_one= _temp2131 + 9; _temp2132;})), Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual _temp2133; _temp2133.q_const= 0; _temp2133.q_volatile= 0;
_temp2133.q_restrict= 0; _temp2133;}),( void*) _temp2031->defn,({ struct Cyc_Core_Opt*
_temp2134=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2134->v=( void*) Cyc_Absynpp_cat2( Cyc_Absynpp_typedef_name2doc( _temp2031->name),
Cyc_Absynpp_tvars2doc( _temp2031->tvs)); _temp2134;})), Cyc_PP_text(({ char*
_temp2135=";"; struct _tagged_string _temp2136; _temp2136.curr= _temp2135;
_temp2136.base= _temp2135; _temp2136.last_plus_one= _temp2135 + 2; _temp2136;})));
goto _LL1949; _LL1969: if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_add(
_temp2035);} s= Cyc_Absynpp_cat7( Cyc_PP_text(({ char* _temp2137="namespace ";
struct _tagged_string _temp2138; _temp2138.curr= _temp2137; _temp2138.base=
_temp2137; _temp2138.last_plus_one= _temp2137 + 11; _temp2138;})), Cyc_Absynpp_textptr(
_temp2035), Cyc_PP_text(({ char* _temp2139=" {"; struct _tagged_string _temp2140;
_temp2140.curr= _temp2139; _temp2140.base= _temp2139; _temp2140.last_plus_one=
_temp2139 + 3; _temp2140;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ char* _temp2141="";
struct _tagged_string _temp2142; _temp2142.curr= _temp2141; _temp2142.base=
_temp2141; _temp2142.last_plus_one= _temp2141 + 1; _temp2142;}), _temp2033), Cyc_PP_line_doc(),
Cyc_PP_text(({ char* _temp2143="}"; struct _tagged_string _temp2144; _temp2144.curr=
_temp2143; _temp2144.base= _temp2143; _temp2144.last_plus_one= _temp2143 + 2;
_temp2144;}))); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL1949; _LL1971: if( Cyc_Absynpp_print_using_stmts){ s= Cyc_Absynpp_cat7(
Cyc_PP_text(({ char* _temp2145="using "; struct _tagged_string _temp2146;
_temp2146.curr= _temp2145; _temp2146.base= _temp2145; _temp2146.last_plus_one=
_temp2145 + 7; _temp2146;})), Cyc_Absynpp_qvar2doc( _temp2039), Cyc_PP_text(({
char* _temp2147=" {"; struct _tagged_string _temp2148; _temp2148.curr= _temp2147;
_temp2148.base= _temp2147; _temp2148.last_plus_one= _temp2147 + 3; _temp2148;})),
Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({
char* _temp2149=""; struct _tagged_string _temp2150; _temp2150.curr= _temp2149;
_temp2150.base= _temp2149; _temp2150.last_plus_one= _temp2149 + 1; _temp2150;}),
_temp2037), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp2151="}"; struct
_tagged_string _temp2152; _temp2152.curr= _temp2151; _temp2152.base= _temp2151;
_temp2152.last_plus_one= _temp2151 + 2; _temp2152;})));} else{ s= Cyc_Absynpp_cat7(
Cyc_PP_text(({ char* _temp2153="/* using "; struct _tagged_string _temp2154;
_temp2154.curr= _temp2153; _temp2154.base= _temp2153; _temp2154.last_plus_one=
_temp2153 + 10; _temp2154;})), Cyc_Absynpp_qvar2doc( _temp2039), Cyc_PP_text(({
char* _temp2155=" { */"; struct _tagged_string _temp2156; _temp2156.curr=
_temp2155; _temp2156.base= _temp2155; _temp2156.last_plus_one= _temp2155 + 6;
_temp2156;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ char* _temp2157=""; struct
_tagged_string _temp2158; _temp2158.curr= _temp2157; _temp2158.base= _temp2157;
_temp2158.last_plus_one= _temp2157 + 1; _temp2158;}), _temp2037), Cyc_PP_line_doc(),
Cyc_PP_text(({ char* _temp2159="/* } */"; struct _tagged_string _temp2160;
_temp2160.curr= _temp2159; _temp2160.base= _temp2159; _temp2160.last_plus_one=
_temp2159 + 8; _temp2160;})));} goto _LL1949; _LL1973: if( Cyc_Absynpp_print_externC_stmts){
s= Cyc_Absynpp_cat5( Cyc_PP_text(({ char* _temp2161="extern \"C\" {"; struct
_tagged_string _temp2162; _temp2162.curr= _temp2161; _temp2162.base= _temp2161;
_temp2162.last_plus_one= _temp2161 + 13; _temp2162;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({
char* _temp2163=""; struct _tagged_string _temp2164; _temp2164.curr= _temp2163;
_temp2164.base= _temp2163; _temp2164.last_plus_one= _temp2163 + 1; _temp2164;}),
_temp2041), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp2165="}"; struct
_tagged_string _temp2166; _temp2166.curr= _temp2165; _temp2166.base= _temp2165;
_temp2166.last_plus_one= _temp2165 + 2; _temp2166;})));} else{ s= Cyc_Absynpp_cat5(
Cyc_PP_text(({ char* _temp2167="/* extern \"C\" { */"; struct _tagged_string
_temp2168; _temp2168.curr= _temp2167; _temp2168.base= _temp2167; _temp2168.last_plus_one=
_temp2167 + 19; _temp2168;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ char*
_temp2169=""; struct _tagged_string _temp2170; _temp2170.curr= _temp2169;
_temp2170.base= _temp2169; _temp2170.last_plus_one= _temp2169 + 1; _temp2170;}),
_temp2041), Cyc_PP_line_doc(), Cyc_PP_text(({ char* _temp2171="/* } */"; struct
_tagged_string _temp2172; _temp2172.curr= _temp2171; _temp2172.base= _temp2171;
_temp2172.last_plus_one= _temp2171 + 8; _temp2172;})));} goto _LL1949; _LL1949:;}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void* sc){ void* _temp2173=
sc; _LL2175: if( _temp2173 ==( void*) Cyc_Absyn_Static){ goto _LL2176;} else{
goto _LL2177;} _LL2177: if( _temp2173 ==( void*) Cyc_Absyn_Public){ goto _LL2178;}
else{ goto _LL2179;} _LL2179: if( _temp2173 ==( void*) Cyc_Absyn_Extern){ goto
_LL2180;} else{ goto _LL2181;} _LL2181: if( _temp2173 ==( void*) Cyc_Absyn_ExternC){
goto _LL2182;} else{ goto _LL2183;} _LL2183: if( _temp2173 ==( void*) Cyc_Absyn_Abstract){
goto _LL2184;} else{ goto _LL2174;} _LL2176: return Cyc_PP_text(({ char*
_temp2185="static "; struct _tagged_string _temp2186; _temp2186.curr= _temp2185;
_temp2186.base= _temp2185; _temp2186.last_plus_one= _temp2185 + 8; _temp2186;}));
_LL2178: return Cyc_PP_nil_doc(); _LL2180: return Cyc_PP_text(({ char* _temp2187="extern ";
struct _tagged_string _temp2188; _temp2188.curr= _temp2187; _temp2188.base=
_temp2187; _temp2188.last_plus_one= _temp2187 + 8; _temp2188;})); _LL2182:
return Cyc_PP_text(({ char* _temp2189="extern \"C\" "; struct _tagged_string
_temp2190; _temp2190.curr= _temp2189; _temp2190.base= _temp2189; _temp2190.last_plus_one=
_temp2189 + 12; _temp2190;})); _LL2184: return Cyc_PP_text(({ char* _temp2191="abstract ";
struct _tagged_string _temp2192; _temp2192.curr= _temp2191; _temp2192.base=
_temp2191; _temp2192.last_plus_one= _temp2191 + 10; _temp2192;})); _LL2174:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp2193= t; struct
Cyc_Absyn_Tvar* _temp2201; struct Cyc_List_List* _temp2203; _LL2195: if((
unsigned int) _temp2193 > 4u?*(( int*) _temp2193) == Cyc_Absyn_VarType: 0){
_LL2202: _temp2201=(( struct Cyc_Absyn_VarType_struct*) _temp2193)->f1; goto
_LL2196;} else{ goto _LL2197;} _LL2197: if(( unsigned int) _temp2193 > 4u?*((
int*) _temp2193) == Cyc_Absyn_JoinEff: 0){ _LL2204: _temp2203=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2193)->f1; goto _LL2198;} else{ goto _LL2199;} _LL2199: goto _LL2200;
_LL2196: return Cyc_Tcutil_is_temp_tvar( _temp2201); _LL2198: return(( int(*)(
int(* pred)( void*), struct Cyc_List_List* x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect,
_temp2203); _LL2200: return 0; _LL2194:;} struct _tuple4 Cyc_Absynpp_to_tms(
struct Cyc_Absyn_Tqual tq, void* t){ void* _temp2205= t; struct Cyc_Absyn_Exp*
_temp2219; struct Cyc_Absyn_Tqual _temp2221; void* _temp2223; struct Cyc_Absyn_PtrInfo
_temp2225; struct Cyc_Absyn_Conref* _temp2227; struct Cyc_Absyn_Tqual _temp2229;
struct Cyc_Absyn_Conref* _temp2231; void* _temp2233; void* _temp2235; struct Cyc_Absyn_FnInfo
_temp2237; struct Cyc_List_List* _temp2239; int _temp2241; struct Cyc_List_List*
_temp2243; void* _temp2245; struct Cyc_Core_Opt* _temp2247; struct Cyc_List_List*
_temp2249; int _temp2251; struct Cyc_Core_Opt* _temp2253; void* _temp2255;
struct Cyc_Core_Opt* _temp2257; struct Cyc_List_List* _temp2259; struct _tuple0*
_temp2261; _LL2207: if(( unsigned int) _temp2205 > 4u?*(( int*) _temp2205) ==
Cyc_Absyn_ArrayType: 0){ _LL2224: _temp2223=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2205)->f1; goto _LL2222; _LL2222: _temp2221=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2205)->f2; goto _LL2220; _LL2220: _temp2219=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2205)->f3; goto _LL2208;} else{ goto _LL2209;} _LL2209: if(( unsigned int)
_temp2205 > 4u?*(( int*) _temp2205) == Cyc_Absyn_PointerType: 0){ _LL2226:
_temp2225=(( struct Cyc_Absyn_PointerType_struct*) _temp2205)->f1; _LL2236:
_temp2235=( void*) _temp2225.elt_typ; goto _LL2234; _LL2234: _temp2233=( void*)
_temp2225.rgn_typ; goto _LL2232; _LL2232: _temp2231= _temp2225.nullable; goto
_LL2230; _LL2230: _temp2229= _temp2225.tq; goto _LL2228; _LL2228: _temp2227=
_temp2225.bounds; goto _LL2210;} else{ goto _LL2211;} _LL2211: if(( unsigned int)
_temp2205 > 4u?*(( int*) _temp2205) == Cyc_Absyn_FnType: 0){ _LL2238: _temp2237=((
struct Cyc_Absyn_FnType_struct*) _temp2205)->f1; _LL2250: _temp2249= _temp2237.tvars;
goto _LL2248; _LL2248: _temp2247= _temp2237.effect; goto _LL2246; _LL2246:
_temp2245=( void*) _temp2237.ret_typ; goto _LL2244; _LL2244: _temp2243=
_temp2237.args; goto _LL2242; _LL2242: _temp2241= _temp2237.varargs; goto
_LL2240; _LL2240: _temp2239= _temp2237.attributes; goto _LL2212;} else{ goto
_LL2213;} _LL2213: if(( unsigned int) _temp2205 > 4u?*(( int*) _temp2205) == Cyc_Absyn_Evar:
0){ _LL2256: _temp2255=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp2205)->f1;
goto _LL2254; _LL2254: _temp2253=(( struct Cyc_Absyn_Evar_struct*) _temp2205)->f2;
goto _LL2252; _LL2252: _temp2251=(( struct Cyc_Absyn_Evar_struct*) _temp2205)->f3;
goto _LL2214;} else{ goto _LL2215;} _LL2215: if(( unsigned int) _temp2205 > 4u?*((
int*) _temp2205) == Cyc_Absyn_TypedefType: 0){ _LL2262: _temp2261=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2205)->f1; goto _LL2260; _LL2260: _temp2259=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2205)->f2; goto _LL2258; _LL2258: _temp2257=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2205)->f3; goto _LL2216;} else{ goto _LL2217;} _LL2217: goto _LL2218;
_LL2208: { struct Cyc_List_List* _temp2265; void* _temp2267; struct Cyc_Absyn_Tqual
_temp2269; struct _tuple4 _temp2263= Cyc_Absynpp_to_tms( _temp2221, _temp2223);
_LL2270: _temp2269= _temp2263.f1; goto _LL2268; _LL2268: _temp2267= _temp2263.f2;
goto _LL2266; _LL2266: _temp2265= _temp2263.f3; goto _LL2264; _LL2264: { void*
tm; if( _temp2219 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2271=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2271[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2272; _temp2272.tag= Cyc_Absyn_ConstArray_mod;
_temp2272.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp2219); _temp2272;});
_temp2271;});} return({ struct _tuple4 _temp2273; _temp2273.f1= _temp2269;
_temp2273.f2= _temp2267; _temp2273.f3=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp2274=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2274->hd=( void*) tm; _temp2274->tl= _temp2265; _temp2274;}); _temp2273;});}}
_LL2210: { struct Cyc_List_List* _temp2277; void* _temp2279; struct Cyc_Absyn_Tqual
_temp2281; struct _tuple4 _temp2275= Cyc_Absynpp_to_tms( _temp2229, _temp2235);
_LL2282: _temp2281= _temp2275.f1; goto _LL2280; _LL2280: _temp2279= _temp2275.f2;
goto _LL2278; _LL2278: _temp2277= _temp2275.f3; goto _LL2276; _LL2276: { void*
ps;{ struct _tuple8 _temp2284=({ struct _tuple8 _temp2283; _temp2283.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2231))->v; _temp2283.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2227))->v; _temp2283;}); void* _temp2294;
void* _temp2296; struct Cyc_Absyn_Exp* _temp2298; void* _temp2300; int _temp2302;
void* _temp2304; void* _temp2306; struct Cyc_Absyn_Exp* _temp2308; void*
_temp2310; int _temp2312; void* _temp2314; void* _temp2316; void* _temp2318;
_LL2286: _LL2301: _temp2300= _temp2284.f1; if(( unsigned int) _temp2300 > 1u?*((
int*) _temp2300) == Cyc_Absyn_Eq_constr: 0){ _LL2303: _temp2302=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp2300)->f1; if( _temp2302 == 1){ goto _LL2295;}
else{ goto _LL2288;}} else{ goto _LL2288;} _LL2295: _temp2294= _temp2284.f2; if((
unsigned int) _temp2294 > 1u?*(( int*) _temp2294) == Cyc_Absyn_Eq_constr: 0){
_LL2297: _temp2296=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2294)->f1;
if(( unsigned int) _temp2296 > 1u?*(( int*) _temp2296) == Cyc_Absyn_Upper_b: 0){
_LL2299: _temp2298=(( struct Cyc_Absyn_Upper_b_struct*) _temp2296)->f1; goto
_LL2287;} else{ goto _LL2288;}} else{ goto _LL2288;} _LL2288: _LL2311: _temp2310=
_temp2284.f1; if(( unsigned int) _temp2310 > 1u?*(( int*) _temp2310) == Cyc_Absyn_Eq_constr:
0){ _LL2313: _temp2312=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2310)->f1;
if( _temp2312 == 0){ goto _LL2305;} else{ goto _LL2290;}} else{ goto _LL2290;}
_LL2305: _temp2304= _temp2284.f2; if(( unsigned int) _temp2304 > 1u?*(( int*)
_temp2304) == Cyc_Absyn_Eq_constr: 0){ _LL2307: _temp2306=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2304)->f1; if(( unsigned int) _temp2306 > 1u?*(( int*) _temp2306) == Cyc_Absyn_Upper_b:
0){ _LL2309: _temp2308=(( struct Cyc_Absyn_Upper_b_struct*) _temp2306)->f1; goto
_LL2289;} else{ goto _LL2290;}} else{ goto _LL2290;} _LL2290: _LL2319: _temp2318=
_temp2284.f1; goto _LL2315; _LL2315: _temp2314= _temp2284.f2; if(( unsigned int)
_temp2314 > 1u?*(( int*) _temp2314) == Cyc_Absyn_Eq_constr: 0){ _LL2317:
_temp2316=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2314)->f1; if(
_temp2316 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2291;} else{ goto _LL2292;}}
else{ goto _LL2292;} _LL2292: goto _LL2293; _LL2287: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2320=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2320[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2321; _temp2321.tag=
Cyc_Absyn_Nullable_ps; _temp2321.f1= _temp2298; _temp2321;}); _temp2320;}); goto
_LL2285; _LL2289: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2322=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2322[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2323; _temp2323.tag= Cyc_Absyn_NonNullable_ps; _temp2323.f1= _temp2308;
_temp2323;}); _temp2322;}); goto _LL2285; _LL2291: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2285; _LL2293: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2285;
_LL2285:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2326=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2326[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2327; _temp2327.tag=
Cyc_Absyn_Pointer_mod; _temp2327.f1=( void*) ps; _temp2327.f2=( void*) _temp2233;
_temp2327.f3= tq; _temp2327;}); _temp2326;}); return({ struct _tuple4 _temp2324;
_temp2324.f1= _temp2281; _temp2324.f2= _temp2279; _temp2324.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2325=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2325->hd=( void*) tm; _temp2325->tl= _temp2277;
_temp2325;}); _temp2324;});}}} _LL2212: if( ! Cyc_Absynpp_print_all_tvars){ if(
_temp2247 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2247))->v)){ _temp2247= 0; _temp2249= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2249);}}{ struct Cyc_List_List* _temp2330;
void* _temp2332; struct Cyc_Absyn_Tqual _temp2334; struct _tuple4 _temp2328= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp2245); _LL2335: _temp2334= _temp2328.f1; goto
_LL2333; _LL2333: _temp2332= _temp2328.f2; goto _LL2331; _LL2331: _temp2330=
_temp2328.f3; goto _LL2329; _LL2329: { struct Cyc_List_List* tms= _temp2330; if(
_temp2239 != 0){ tms=({ struct Cyc_List_List* _temp2336=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2336->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2337=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2337[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2338; _temp2338.tag= Cyc_Absyn_Attributes_mod;
_temp2338.f1= 0; _temp2338.f2= _temp2239; _temp2338;}); _temp2337;})); _temp2336->tl=
tms; _temp2336;});} tms=({ struct Cyc_List_List* _temp2339=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2339->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2340=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2340[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2341; _temp2341.tag= Cyc_Absyn_Function_mod;
_temp2341.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2342=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2342[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2343; _temp2343.tag= Cyc_Absyn_WithTypes;
_temp2343.f1= _temp2243; _temp2343.f2= _temp2241; _temp2343.f3= _temp2247;
_temp2343;}); _temp2342;})); _temp2341;}); _temp2340;})); _temp2339->tl= tms;
_temp2339;}); if( _temp2249 != 0){ tms=({ struct Cyc_List_List* _temp2344=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2344->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2345=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2345[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2346; _temp2346.tag= Cyc_Absyn_TypeParams_mod;
_temp2346.f1= _temp2249; _temp2346.f2= 0; _temp2346.f3= 1; _temp2346;});
_temp2345;})); _temp2344->tl= tms; _temp2344;});} return({ struct _tuple4
_temp2347; _temp2347.f1= _temp2334; _temp2347.f2= _temp2332; _temp2347.f3= tms;
_temp2347;});}} _LL2214: if( _temp2253 == 0){ return({ struct _tuple4 _temp2348;
_temp2348.f1= tq; _temp2348.f2= t; _temp2348.f3= 0; _temp2348;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2253))->v);}
_LL2216: if( _temp2257 == 0? 1: ! Cyc_Absynpp_expand_typedefs){ return({ struct
_tuple4 _temp2349; _temp2349.f1= tq; _temp2349.f2= t; _temp2349.f3= 0; _temp2349;});}
else{ return Cyc_Absynpp_to_tms( tq,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp2257))->v);} _LL2218: return({ struct _tuple4 _temp2350; _temp2350.f1= tq;
_temp2350.f2= t; _temp2350.f3= 0; _temp2350;}); _LL2206:;} struct Cyc_PP_Doc*
Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_Core_Opt*
dopt){ struct Cyc_List_List* _temp2353; void* _temp2355; struct Cyc_Absyn_Tqual
_temp2357; struct _tuple4 _temp2351= Cyc_Absynpp_to_tms( tq, t); _LL2358:
_temp2357= _temp2351.f1; goto _LL2356; _LL2356: _temp2355= _temp2351.f2; goto
_LL2354; _LL2354: _temp2353= _temp2351.f3; goto _LL2352; _LL2352: _temp2353=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2353);
if( _temp2353 == 0? dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc(
_temp2357), Cyc_Absynpp_ntyp2doc( _temp2355));} else{ return Cyc_Absynpp_cat4(
Cyc_Absynpp_tqual2doc( _temp2357), Cyc_Absynpp_ntyp2doc( _temp2355), Cyc_PP_text(({
char* _temp2359=" "; struct _tagged_string _temp2360; _temp2360.curr= _temp2359;
_temp2360.base= _temp2359; _temp2360.last_plus_one= _temp2359 + 2; _temp2360;})),
Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)(( struct
Cyc_Core_Opt*) _check_null( dopt))->v, _temp2353));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return Cyc_Absynpp_cat5(
Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2361=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2361->v=(
void*) Cyc_Absynpp_textptr( f->name); _temp2361;})), Cyc_PP_text(({ char*
_temp2362=":"; struct _tagged_string _temp2363; _temp2363.curr= _temp2362;
_temp2363.base= _temp2362; _temp2363.last_plus_one= _temp2362 + 2; _temp2363;})),
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null(
f->width))->v), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text(({ char*
_temp2364=";"; struct _tagged_string _temp2365; _temp2365.curr= _temp2364;
_temp2365.base= _temp2364; _temp2365.last_plus_one= _temp2364 + 2; _temp2365;})));}
else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp2366=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2366->v=( void*) Cyc_Absynpp_textptr( f->name); _temp2366;})),
Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text(({ char* _temp2367=";"; struct
_tagged_string _temp2368; _temp2368.curr= _temp2367; _temp2368.base= _temp2367;
_temp2368.last_plus_one= _temp2367 + 2; _temp2368;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,({ char* _temp2369=""; struct
_tagged_string _temp2370; _temp2370.curr= _temp2369; _temp2370.base= _temp2369;
_temp2370.last_plus_one= _temp2369 + 1; _temp2370;}), fields);} struct Cyc_PP_Doc*
Cyc_Absynpp_tunionfield2doc( struct Cyc_Absyn_Tunionfield* f){ return Cyc_Absynpp_cat4(
Cyc_Absynpp_scope2doc(( void*) f->sc), Cyc_Absynpp_typedef_name2doc( f->name), f->tvs
== 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs), f->typs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,({ char* _temp2371=","; struct
_tagged_string _temp2372; _temp2372.curr= _temp2371; _temp2372.base= _temp2371;
_temp2372.last_plus_one= _temp2371 + 2; _temp2372;}), fields);} void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl=((
struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd), 72, f);
fprintf( f,"\n");}} struct _tagged_string Cyc_Absynpp_decllist2string( struct
Cyc_List_List* tdl){ return Cyc_PP_string_of_doc( Cyc_PP_seql(({ char* _temp2373="";
struct _tagged_string _temp2374; _temp2374.curr= _temp2373; _temp2374.base=
_temp2373; _temp2374.last_plus_one= _temp2373 + 1; _temp2374;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_decl2doc, tdl)), 72);} struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp* e){ return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e),
72);} struct _tagged_string Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){
return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s), 72);} struct
_tagged_string Cyc_Absynpp_typ2string( void* t){ Cyc_Absynpp_dont_rewrite_temp_tvars=
1;{ struct _tagged_string _temp2375= Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc(
t), 72); goto _LL2376; _LL2376: Cyc_Absynpp_dont_rewrite_temp_tvars= 0; return
_temp2375;}} struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0* v){
return Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);} struct
_tagged_string Cyc_Absynpp_prim2string( void* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc( p), 72);} struct _tagged_string Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_pat2doc( p),
72);} struct _tagged_string Cyc_Absynpp_scope2string( void* sc){ void* _temp2377=
sc; _LL2379: if( _temp2377 ==( void*) Cyc_Absyn_Static){ goto _LL2380;} else{
goto _LL2381;} _LL2381: if( _temp2377 ==( void*) Cyc_Absyn_Public){ goto _LL2382;}
else{ goto _LL2383;} _LL2383: if( _temp2377 ==( void*) Cyc_Absyn_Extern){ goto
_LL2384;} else{ goto _LL2385;} _LL2385: if( _temp2377 ==( void*) Cyc_Absyn_ExternC){
goto _LL2386;} else{ goto _LL2387;} _LL2387: if( _temp2377 ==( void*) Cyc_Absyn_Abstract){
goto _LL2388;} else{ goto _LL2378;} _LL2380: return({ char* _temp2389="static";
struct _tagged_string _temp2390; _temp2390.curr= _temp2389; _temp2390.base=
_temp2389; _temp2390.last_plus_one= _temp2389 + 7; _temp2390;}); _LL2382: return({
char* _temp2391="public"; struct _tagged_string _temp2392; _temp2392.curr=
_temp2391; _temp2392.base= _temp2391; _temp2392.last_plus_one= _temp2391 + 7;
_temp2392;}); _LL2384: return({ char* _temp2393="extern"; struct _tagged_string
_temp2394; _temp2394.curr= _temp2393; _temp2394.base= _temp2393; _temp2394.last_plus_one=
_temp2393 + 7; _temp2394;}); _LL2386: return({ char* _temp2395="extern \"C\"";
struct _tagged_string _temp2396; _temp2396.curr= _temp2395; _temp2396.base=
_temp2395; _temp2396.last_plus_one= _temp2395 + 11; _temp2396;}); _LL2388:
return({ char* _temp2397="abstract"; struct _tagged_string _temp2398; _temp2398.curr=
_temp2397; _temp2398.base= _temp2397; _temp2398.last_plus_one= _temp2397 + 9;
_temp2398;}); _LL2378:;}