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
extern char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError[
19u]; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef
struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t;
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
extern int Cyc_Absynpp_decls_first; extern int Cyc_Absynpp_print_all_tvars;
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
_tagged_string Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p); extern int Cyc_Absynpp_exp_prec(
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
int Cyc_Absynpp_expand_typedefs= 0; int Cyc_Absynpp_qvar_to_Cids= 0; int Cyc_Absynpp_decls_first=
0; int Cyc_Absynpp_dont_rewrite_temp_tvars= 0; int Cyc_Absynpp_print_all_tvars=
0; int Cyc_Absynpp_print_all_kinds= 0; int Cyc_Absynpp_print_using_stmts= 0; int
Cyc_Absynpp_print_externC_stmts= 0; static struct Cyc_List_List* Cyc_Absynpp_curr_namespace=
0; static void Cyc_Absynpp_curr_namespace_add( struct _tagged_string* v){ Cyc_Absynpp_curr_namespace=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_Absynpp_curr_namespace,({ struct Cyc_List_List* _temp0=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp0->hd=( void*) v; _temp0->tl= 0;
_temp0;}));} static void Cyc_Absynpp_suppr_last( struct Cyc_List_List** l){ if(({
struct Cyc_List_List* _temp1=* l; if( _temp1 == 0){ _throw( Null_Exception);}
_temp1->tl;}) == 0){* l= 0;} else{(( void(*)( struct Cyc_List_List** l)) Cyc_Absynpp_suppr_last)((
struct Cyc_List_List**)({ struct Cyc_List_List* _temp2=* l; if( _temp2 == 0){
_throw( Null_Exception);}& _temp2->tl;}));}} static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)( struct Cyc_List_List** l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _tagged_string Cyc_Absynpp_char_escape( char c){ switch( c){ case '\a':
_LL3: return( struct _tagged_string)({ char* _temp5=( char*)"\\a"; struct
_tagged_string _temp6; _temp6.curr= _temp5; _temp6.base= _temp5; _temp6.last_plus_one=
_temp5 + 3; _temp6;}); case '\b': _LL4: return( struct _tagged_string)({ char*
_temp8=( char*)"\\b"; struct _tagged_string _temp9; _temp9.curr= _temp8; _temp9.base=
_temp8; _temp9.last_plus_one= _temp8 + 3; _temp9;}); case '\f': _LL7: return(
struct _tagged_string)({ char* _temp11=( char*)"\\f"; struct _tagged_string
_temp12; _temp12.curr= _temp11; _temp12.base= _temp11; _temp12.last_plus_one=
_temp11 + 3; _temp12;}); case '\n': _LL10: return( struct _tagged_string)({ char*
_temp14=( char*)"\\n"; struct _tagged_string _temp15; _temp15.curr= _temp14;
_temp15.base= _temp14; _temp15.last_plus_one= _temp14 + 3; _temp15;}); case '\r':
_LL13: return( struct _tagged_string)({ char* _temp17=( char*)"\\r"; struct
_tagged_string _temp18; _temp18.curr= _temp17; _temp18.base= _temp17; _temp18.last_plus_one=
_temp17 + 3; _temp18;}); case '\t': _LL16: return( struct _tagged_string)({ char*
_temp20=( char*)"\\t"; struct _tagged_string _temp21; _temp21.curr= _temp20;
_temp21.base= _temp20; _temp21.last_plus_one= _temp20 + 3; _temp21;}); case '\v':
_LL19: return( struct _tagged_string)({ char* _temp23=( char*)"\\v"; struct
_tagged_string _temp24; _temp24.curr= _temp23; _temp24.base= _temp23; _temp24.last_plus_one=
_temp23 + 3; _temp24;}); case '\\': _LL22: return( struct _tagged_string)({ char*
_temp26=( char*)"\\\\"; struct _tagged_string _temp27; _temp27.curr= _temp26;
_temp27.base= _temp26; _temp27.last_plus_one= _temp26 + 3; _temp27;}); case '"':
_LL25: return( struct _tagged_string)({ char* _temp29=( char*)"\""; struct
_tagged_string _temp30; _temp30.curr= _temp29; _temp30.base= _temp29; _temp30.last_plus_one=
_temp29 + 2; _temp30;}); case '\'': _LL28: return( struct _tagged_string)({ char*
_temp32=( char*)"\\'"; struct _tagged_string _temp33; _temp33.curr= _temp32;
_temp33.base= _temp32; _temp33.last_plus_one= _temp32 + 3; _temp33;}); default:
_LL31: if( c >=' '? c <='~': 0){ struct _tagged_string t= Cyc_Core_new_string( 1);({
struct _tagged_string _temp35= t; char* _temp37= _temp35.curr + 0; if( _temp35.base
== 0? 1:( _temp37 < _temp35.base? 1: _temp37 >= _temp35.last_plus_one)){ _throw(
Null_Exception);}* _temp37= c;}); return t;} else{ struct _tagged_string t= Cyc_Core_new_string(
4); int j= 0;({ struct _tagged_string _temp38= t; char* _temp40= _temp38.curr +(
j ++); if( _temp38.base == 0? 1:( _temp40 < _temp38.base? 1: _temp40 >= _temp38.last_plus_one)){
_throw( Null_Exception);}* _temp40='\\';});({ struct _tagged_string _temp41= t;
char* _temp43= _temp41.curr +( j ++); if( _temp41.base == 0? 1:( _temp43 <
_temp41.base? 1: _temp43 >= _temp41.last_plus_one)){ _throw( Null_Exception);}*
_temp43=( char)('0' +( c >> 6 & 7));});({ struct _tagged_string _temp44= t; char*
_temp46= _temp44.curr +( j ++); if( _temp44.base == 0? 1:( _temp46 < _temp44.base?
1: _temp46 >= _temp44.last_plus_one)){ _throw( Null_Exception);}* _temp46=( char)('0'
+( c >> 3 & 7));});({ struct _tagged_string _temp47= t; char* _temp49= _temp47.curr
+( j ++); if( _temp47.base == 0? 1:( _temp49 < _temp47.base? 1: _temp49 >=
_temp47.last_plus_one)){ _throw( Null_Exception);}* _temp49=( char)('0' +( c & 7));});
return t;}}} static int Cyc_Absynpp_special( struct _tagged_string s){ int sz=(
int)(({ struct _tagged_string _temp53= s;( unsigned int)( _temp53.last_plus_one
- _temp53.curr);}) - 1);{ int i= 0; for( 0; i < sz; i ++){ char c=({ struct
_tagged_string _temp50= s; char* _temp52= _temp50.curr + i; if( _temp50.base ==
0? 1:( _temp52 < _temp50.base? 1: _temp52 >= _temp50.last_plus_one)){ _throw(
Null_Exception);}* _temp52;}); if((( c <=' '? 1: c >='~')? 1: c =='"')? 1: c =='\\'){
return 1;}}} return 0;} struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string s){ if( ! Cyc_Absynpp_special( s)){ return s;}{ int n=( int)(({
struct _tagged_string _temp176= s;( unsigned int)( _temp176.last_plus_one -
_temp176.curr);}) - 1); if( n > 0?({ struct _tagged_string _temp54= s; char*
_temp56= _temp54.curr + n; if( _temp54.base == 0? 1:( _temp56 < _temp54.base? 1:
_temp56 >= _temp54.last_plus_one)){ _throw( Null_Exception);}* _temp56;}) =='\000':
0){ n --;}{ int len= 0;{ int i= 0; for( 0; i <= n; i ++){ char _temp60=({ struct
_tagged_string _temp57= s; char* _temp59= _temp57.curr + i; if( _temp57.base ==
0? 1:( _temp59 < _temp57.base? 1: _temp59 >= _temp57.last_plus_one)){ _throw(
Null_Exception);}* _temp59;}); _LL62: if( _temp60 =='\a'){ goto _LL63;} else{
goto _LL64;} _LL64: if( _temp60 =='\b'){ goto _LL65;} else{ goto _LL66;} _LL66:
if( _temp60 =='\f'){ goto _LL67;} else{ goto _LL68;} _LL68: if( _temp60 =='\n'){
goto _LL69;} else{ goto _LL70;} _LL70: if( _temp60 =='\r'){ goto _LL71;} else{
goto _LL72;} _LL72: if( _temp60 =='\t'){ goto _LL73;} else{ goto _LL74;} _LL74:
if( _temp60 =='\v'){ goto _LL75;} else{ goto _LL76;} _LL76: if( _temp60 =='\\'){
goto _LL77;} else{ goto _LL78;} _LL78: if( _temp60 =='"'){ goto _LL79;} else{
goto _LL80;} _LL80: goto _LL81; _LL63: goto _LL65; _LL65: goto _LL67; _LL67:
goto _LL69; _LL69: goto _LL71; _LL71: goto _LL73; _LL73: goto _LL75; _LL75: goto
_LL77; _LL77: goto _LL79; _LL79: len += 2; goto _LL61; _LL81: if( _temp60 >=' '?
_temp60 <='~': 0){ len ++;} else{ len += 4;} goto _LL61; _LL61:;}}{ struct
_tagged_string t= Cyc_Core_new_string( len); int j= 0;{ int i= 0; for( 0; i <= n;
i ++){ char _temp85=({ struct _tagged_string _temp82= s; char* _temp84= _temp82.curr
+ i; if( _temp82.base == 0? 1:( _temp84 < _temp82.base? 1: _temp84 >= _temp82.last_plus_one)){
_throw( Null_Exception);}* _temp84;}); _LL87: if( _temp85 =='\a'){ goto _LL88;}
else{ goto _LL89;} _LL89: if( _temp85 =='\b'){ goto _LL90;} else{ goto _LL91;}
_LL91: if( _temp85 =='\f'){ goto _LL92;} else{ goto _LL93;} _LL93: if( _temp85
=='\n'){ goto _LL94;} else{ goto _LL95;} _LL95: if( _temp85 =='\r'){ goto _LL96;}
else{ goto _LL97;} _LL97: if( _temp85 =='\t'){ goto _LL98;} else{ goto _LL99;}
_LL99: if( _temp85 =='\v'){ goto _LL100;} else{ goto _LL101;} _LL101: if(
_temp85 =='\\'){ goto _LL102;} else{ goto _LL103;} _LL103: if( _temp85 =='"'){
goto _LL104;} else{ goto _LL105;} _LL105: goto _LL106; _LL88:({ struct
_tagged_string _temp107= t; char* _temp109= _temp107.curr +( j ++); if( _temp107.base
== 0? 1:( _temp109 < _temp107.base? 1: _temp109 >= _temp107.last_plus_one)){
_throw( Null_Exception);}* _temp109='\\';});({ struct _tagged_string _temp110= t;
char* _temp112= _temp110.curr +( j ++); if( _temp110.base == 0? 1:( _temp112 <
_temp110.base? 1: _temp112 >= _temp110.last_plus_one)){ _throw( Null_Exception);}*
_temp112='a';}); goto _LL86; _LL90:({ struct _tagged_string _temp113= t; char*
_temp115= _temp113.curr +( j ++); if( _temp113.base == 0? 1:( _temp115 <
_temp113.base? 1: _temp115 >= _temp113.last_plus_one)){ _throw( Null_Exception);}*
_temp115='\\';});({ struct _tagged_string _temp116= t; char* _temp118= _temp116.curr
+( j ++); if( _temp116.base == 0? 1:( _temp118 < _temp116.base? 1: _temp118 >=
_temp116.last_plus_one)){ _throw( Null_Exception);}* _temp118='b';}); goto _LL86;
_LL92:({ struct _tagged_string _temp119= t; char* _temp121= _temp119.curr +( j
++); if( _temp119.base == 0? 1:( _temp121 < _temp119.base? 1: _temp121 >=
_temp119.last_plus_one)){ _throw( Null_Exception);}* _temp121='\\';});({ struct
_tagged_string _temp122= t; char* _temp124= _temp122.curr +( j ++); if( _temp122.base
== 0? 1:( _temp124 < _temp122.base? 1: _temp124 >= _temp122.last_plus_one)){
_throw( Null_Exception);}* _temp124='f';}); goto _LL86; _LL94:({ struct
_tagged_string _temp125= t; char* _temp127= _temp125.curr +( j ++); if( _temp125.base
== 0? 1:( _temp127 < _temp125.base? 1: _temp127 >= _temp125.last_plus_one)){
_throw( Null_Exception);}* _temp127='\\';});({ struct _tagged_string _temp128= t;
char* _temp130= _temp128.curr +( j ++); if( _temp128.base == 0? 1:( _temp130 <
_temp128.base? 1: _temp130 >= _temp128.last_plus_one)){ _throw( Null_Exception);}*
_temp130='n';}); goto _LL86; _LL96:({ struct _tagged_string _temp131= t; char*
_temp133= _temp131.curr +( j ++); if( _temp131.base == 0? 1:( _temp133 <
_temp131.base? 1: _temp133 >= _temp131.last_plus_one)){ _throw( Null_Exception);}*
_temp133='\\';});({ struct _tagged_string _temp134= t; char* _temp136= _temp134.curr
+( j ++); if( _temp134.base == 0? 1:( _temp136 < _temp134.base? 1: _temp136 >=
_temp134.last_plus_one)){ _throw( Null_Exception);}* _temp136='r';}); goto _LL86;
_LL98:({ struct _tagged_string _temp137= t; char* _temp139= _temp137.curr +( j
++); if( _temp137.base == 0? 1:( _temp139 < _temp137.base? 1: _temp139 >=
_temp137.last_plus_one)){ _throw( Null_Exception);}* _temp139='\\';});({ struct
_tagged_string _temp140= t; char* _temp142= _temp140.curr +( j ++); if( _temp140.base
== 0? 1:( _temp142 < _temp140.base? 1: _temp142 >= _temp140.last_plus_one)){
_throw( Null_Exception);}* _temp142='t';}); goto _LL86; _LL100:({ struct
_tagged_string _temp143= t; char* _temp145= _temp143.curr +( j ++); if( _temp143.base
== 0? 1:( _temp145 < _temp143.base? 1: _temp145 >= _temp143.last_plus_one)){
_throw( Null_Exception);}* _temp145='\\';});({ struct _tagged_string _temp146= t;
char* _temp148= _temp146.curr +( j ++); if( _temp146.base == 0? 1:( _temp148 <
_temp146.base? 1: _temp148 >= _temp146.last_plus_one)){ _throw( Null_Exception);}*
_temp148='v';}); goto _LL86; _LL102:({ struct _tagged_string _temp149= t; char*
_temp151= _temp149.curr +( j ++); if( _temp149.base == 0? 1:( _temp151 <
_temp149.base? 1: _temp151 >= _temp149.last_plus_one)){ _throw( Null_Exception);}*
_temp151='\\';});({ struct _tagged_string _temp152= t; char* _temp154= _temp152.curr
+( j ++); if( _temp152.base == 0? 1:( _temp154 < _temp152.base? 1: _temp154 >=
_temp152.last_plus_one)){ _throw( Null_Exception);}* _temp154='\\';}); goto
_LL86; _LL104:({ struct _tagged_string _temp155= t; char* _temp157= _temp155.curr
+( j ++); if( _temp155.base == 0? 1:( _temp157 < _temp155.base? 1: _temp157 >=
_temp155.last_plus_one)){ _throw( Null_Exception);}* _temp157='\\';});({ struct
_tagged_string _temp158= t; char* _temp160= _temp158.curr +( j ++); if( _temp158.base
== 0? 1:( _temp160 < _temp158.base? 1: _temp160 >= _temp158.last_plus_one)){
_throw( Null_Exception);}* _temp160='"';}); goto _LL86; _LL106: if( _temp85 >=' '?
_temp85 <='~': 0){({ struct _tagged_string _temp161= t; char* _temp163= _temp161.curr
+( j ++); if( _temp161.base == 0? 1:( _temp163 < _temp161.base? 1: _temp163 >=
_temp161.last_plus_one)){ _throw( Null_Exception);}* _temp163= _temp85;});}
else{({ struct _tagged_string _temp164= t; char* _temp166= _temp164.curr +( j ++);
if( _temp164.base == 0? 1:( _temp166 < _temp164.base? 1: _temp166 >= _temp164.last_plus_one)){
_throw( Null_Exception);}* _temp166='\\';});({ struct _tagged_string _temp167= t;
char* _temp169= _temp167.curr +( j ++); if( _temp167.base == 0? 1:( _temp169 <
_temp167.base? 1: _temp169 >= _temp167.last_plus_one)){ _throw( Null_Exception);}*
_temp169=( char)('0' +( _temp85 >> 6 & 7));});({ struct _tagged_string _temp170=
t; char* _temp172= _temp170.curr +( j ++); if( _temp170.base == 0? 1:( _temp172
< _temp170.base? 1: _temp172 >= _temp170.last_plus_one)){ _throw( Null_Exception);}*
_temp172=( char)('0' +( _temp85 >> 3 & 7));});({ struct _tagged_string _temp173=
t; char* _temp175= _temp173.curr +( j ++); if( _temp173.base == 0? 1:( _temp175
< _temp173.base? 1: _temp175 >= _temp173.last_plus_one)){ _throw( Null_Exception);}*
_temp175=( char)('0' +( _temp85 & 7));});} goto _LL86; _LL86:;}} return t;}}}}
static struct Cyc_PP_Doc* Cyc_Absynpp_textptr( struct _tagged_string* s){ return
Cyc_PP_text(* s);} static char _temp179[ 9u]="restrict"; static struct
_tagged_string Cyc_Absynpp_restrict_string=( struct _tagged_string){ _temp179,
_temp179, _temp179 + 9u}; static char _temp182[ 9u]="volatile"; static struct
_tagged_string Cyc_Absynpp_volatile_string=( struct _tagged_string){ _temp182,
_temp182, _temp182 + 9u}; static char _temp185[ 6u]="const"; static struct
_tagged_string Cyc_Absynpp_const_string=( struct _tagged_string){ _temp185,
_temp185, _temp185 + 6u}; static struct _tagged_string* Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string; static struct _tagged_string* Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string; static struct _tagged_string* Cyc_Absynpp_const_sp=&
Cyc_Absynpp_const_string; struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual
tq){ struct Cyc_List_List* l= 0; if( tq.q_restrict){ l=({ struct Cyc_List_List*
_temp186=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp186->hd=( void*) Cyc_Absynpp_restrict_sp; _temp186->tl= l; _temp186;});}
if( tq.q_volatile){ l=({ struct Cyc_List_List* _temp187=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp187->hd=( void*) Cyc_Absynpp_volatile_sp;
_temp187->tl= l; _temp187;});} if( tq.q_const){ l=({ struct Cyc_List_List*
_temp188=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp188->hd=( void*) Cyc_Absynpp_const_sp; _temp188->tl= l; _temp188;});}
return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp189=( char*)"";
struct _tagged_string _temp190; _temp190.curr= _temp189; _temp190.base= _temp189;
_temp190.last_plus_one= _temp189 + 1; _temp190;}),( struct _tagged_string)({
char* _temp191=( char*)" "; struct _tagged_string _temp192; _temp192.curr=
_temp191; _temp192.base= _temp191; _temp192.last_plus_one= _temp191 + 2;
_temp192;}),( struct _tagged_string)({ char* _temp193=( char*)" "; struct
_tagged_string _temp194; _temp194.curr= _temp193; _temp194.base= _temp193;
_temp194.last_plus_one= _temp193 + 2; _temp194;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr, l));} struct _tagged_string Cyc_Absynpp_kind2string( void*
k){ void* _temp195= k; _LL197: if( _temp195 ==( void*) Cyc_Absyn_AnyKind){ goto
_LL198;} else{ goto _LL199;} _LL199: if( _temp195 ==( void*) Cyc_Absyn_MemKind){
goto _LL200;} else{ goto _LL201;} _LL201: if( _temp195 ==( void*) Cyc_Absyn_BoxKind){
goto _LL202;} else{ goto _LL203;} _LL203: if( _temp195 ==( void*) Cyc_Absyn_RgnKind){
goto _LL204;} else{ goto _LL205;} _LL205: if( _temp195 ==( void*) Cyc_Absyn_EffKind){
goto _LL206;} else{ goto _LL196;} _LL198: return( struct _tagged_string)({ char*
_temp207=( char*)"A"; struct _tagged_string _temp208; _temp208.curr= _temp207;
_temp208.base= _temp207; _temp208.last_plus_one= _temp207 + 2; _temp208;});
_LL200: return( struct _tagged_string)({ char* _temp209=( char*)"M"; struct
_tagged_string _temp210; _temp210.curr= _temp209; _temp210.base= _temp209;
_temp210.last_plus_one= _temp209 + 2; _temp210;}); _LL202: return( struct
_tagged_string)({ char* _temp211=( char*)"B"; struct _tagged_string _temp212;
_temp212.curr= _temp211; _temp212.base= _temp211; _temp212.last_plus_one=
_temp211 + 2; _temp212;}); _LL204: return( struct _tagged_string)({ char*
_temp213=( char*)"R"; struct _tagged_string _temp214; _temp214.curr= _temp213;
_temp214.base= _temp213; _temp214.last_plus_one= _temp213 + 2; _temp214;});
_LL206: return( struct _tagged_string)({ char* _temp215=( char*)"E"; struct
_tagged_string _temp216; _temp216.curr= _temp215; _temp216.base= _temp215;
_temp216.last_plus_one= _temp215 + 2; _temp216;}); _LL196:;} struct
_tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref* c){ void*
_temp217=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( c))->v; void* _temp223; _LL219: if(( unsigned int)
_temp217 > 1u?*(( int*) _temp217) == Cyc_Absyn_Eq_constr: 0){ _LL224: _temp223=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp217)->f1; goto _LL220;} else{
goto _LL221;} _LL221: goto _LL222; _LL220: return Cyc_Absynpp_kind2string(
_temp223); _LL222: return( struct _tagged_string)({ char* _temp225=( char*)"?";
struct _tagged_string _temp226; _temp226.curr= _temp225; _temp226.base= _temp225;
_temp226.last_plus_one= _temp225 + 2; _temp226;}); _LL218:;} struct Cyc_PP_Doc*
Cyc_Absynpp_kind2doc( void* k){ return Cyc_PP_text( Cyc_Absynpp_kind2string( k));}
struct Cyc_PP_Doc* Cyc_Absynpp_ckind2doc( struct Cyc_Absyn_Conref* c){ return
Cyc_PP_text( Cyc_Absynpp_ckind2string( c));} struct Cyc_PP_Doc* Cyc_Absynpp_group(
struct _tagged_string start, struct _tagged_string stop, struct _tagged_string
sep, struct Cyc_List_List* ss){ return Cyc_Absynpp_cat3( Cyc_PP_text( start),
Cyc_PP_seq( sep, ss), Cyc_PP_text( stop));} struct Cyc_PP_Doc* Cyc_Absynpp_egroup(
struct _tagged_string start, struct _tagged_string stop, struct _tagged_string
sep, struct Cyc_List_List* ss){ if( ss == 0){ return Cyc_PP_nil_doc();} else{
return Cyc_Absynpp_cat3( Cyc_PP_text( start), Cyc_PP_seq( sep, ss), Cyc_PP_text(
stop));}} struct Cyc_PP_Doc* Cyc_Absynpp_groupl( struct _tagged_string start,
struct _tagged_string stop, struct _tagged_string sep, struct Cyc_List_List* ss){
return Cyc_Absynpp_cat3( Cyc_PP_text( start), Cyc_PP_seql( sep, ss), Cyc_PP_text(
stop));} struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc( struct Cyc_List_List* ts){
return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp227=( char*)"<";
struct _tagged_string _temp228; _temp228.curr= _temp227; _temp228.base= _temp227;
_temp228.last_plus_one= _temp227 + 2; _temp228;}),( struct _tagged_string)({
char* _temp229=( char*)">"; struct _tagged_string _temp230; _temp230.curr=
_temp229; _temp230.base= _temp229; _temp230.last_plus_one= _temp229 + 2;
_temp230;}),( struct _tagged_string)({ char* _temp231=( char*)","; struct
_tagged_string _temp232; _temp232.curr= _temp231; _temp232.base= _temp231;
_temp232.last_plus_one= _temp231 + 2; _temp232;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc,
ts));} struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){
void* _temp233=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp241; void* _temp243;
_LL235: if(( unsigned int) _temp233 > 1u?*(( int*) _temp233) == Cyc_Absyn_Eq_constr:
0){ _LL242: _temp241=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp233)->f1;
if( _temp241 ==( void*) Cyc_Absyn_BoxKind){ goto _LL236;} else{ goto _LL237;}}
else{ goto _LL237;} _LL237: if(( unsigned int) _temp233 > 1u?*(( int*) _temp233)
== Cyc_Absyn_Eq_constr: 0){ _LL244: _temp243=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp233)->f1; goto _LL238;} else{ goto _LL239;} _LL239: goto _LL240; _LL236:
return Cyc_Absynpp_textptr( tv->name); _LL238: return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr(
tv->name), Cyc_PP_text(( struct _tagged_string)({ char* _temp245=( char*)"::";
struct _tagged_string _temp246; _temp246.curr= _temp245; _temp246.base= _temp245;
_temp246.last_plus_one= _temp245 + 3; _temp246;})), Cyc_Absynpp_kind2doc(
_temp243)); _LL240: return Cyc_Absynpp_cat2( Cyc_Absynpp_textptr( tv->name), Cyc_PP_text((
struct _tagged_string)({ char* _temp247=( char*)"/*::?*/"; struct _tagged_string
_temp248; _temp248.curr= _temp247; _temp248.base= _temp247; _temp248.last_plus_one=
_temp247 + 8; _temp248;}))); _LL234:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List* tvs){ return Cyc_Absynpp_egroup(( struct _tagged_string)({
char* _temp249=( char*)"<"; struct _tagged_string _temp250; _temp250.curr=
_temp249; _temp250.base= _temp249; _temp250.last_plus_one= _temp249 + 2;
_temp250;}),( struct _tagged_string)({ char* _temp251=( char*)">"; struct
_tagged_string _temp252; _temp252.curr= _temp251; _temp252.base= _temp251;
_temp252.last_plus_one= _temp251 + 2; _temp252;}),( struct _tagged_string)({
char* _temp253=( char*)","; struct _tagged_string _temp254; _temp254.curr=
_temp253; _temp254.base= _temp253; _temp254.last_plus_one= _temp253 + 2;
_temp254;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_ktvar2doc, tvs));} static
struct _tagged_string* Cyc_Absynpp_get_name( struct Cyc_Absyn_Tvar* tv){ return
tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc( struct Cyc_List_List* tvs){
if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc( tvs);} return
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp255=( char*)"<"; struct
_tagged_string _temp256; _temp256.curr= _temp255; _temp256.base= _temp255;
_temp256.last_plus_one= _temp255 + 2; _temp256;}),( struct _tagged_string)({
char* _temp257=( char*)">"; struct _tagged_string _temp258; _temp258.curr=
_temp257; _temp258.base= _temp257; _temp258.last_plus_one= _temp257 + 2;
_temp258;}),( struct _tagged_string)({ char* _temp259=( char*)","; struct
_tagged_string _temp260; _temp260.curr= _temp259; _temp260.base= _temp259;
_temp260.last_plus_one= _temp259 + 2; _temp260;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr,(( struct Cyc_List_List*(*)( struct _tagged_string*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_get_name,
tvs)));} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; struct Cyc_PP_Doc*
Cyc_Absynpp_arg2doc( struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((* t).f1,(*
t).f2, 0);} struct Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List* ts){
return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp261=( char*)"(";
struct _tagged_string _temp262; _temp262.curr= _temp261; _temp262.base= _temp261;
_temp262.last_plus_one= _temp261 + 2; _temp262;}),( struct _tagged_string)({
char* _temp263=( char*)")"; struct _tagged_string _temp264; _temp264.curr=
_temp263; _temp264.base= _temp263; _temp264.last_plus_one= _temp263 + 2;
_temp264;}),( struct _tagged_string)({ char* _temp265=( char*)","; struct
_tagged_string _temp266; _temp266.curr= _temp265; _temp266.base= _temp265;
_temp266.last_plus_one= _temp265 + 2; _temp266;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc( void* a){
return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc* Cyc_Absynpp_atts2doc(
struct Cyc_List_List* atts){ if( atts == 0){ return Cyc_PP_nil_doc();} return
Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp267=( char*)" __attribute__";
struct _tagged_string _temp268; _temp268.curr= _temp267; _temp268.base= _temp267;
_temp268.last_plus_one= _temp267 + 15; _temp268;})), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp269=( char*)"(("; struct _tagged_string _temp270;
_temp270.curr= _temp269; _temp270.base= _temp269; _temp270.last_plus_one=
_temp269 + 3; _temp270;}),( struct _tagged_string)({ char* _temp271=( char*)"))";
struct _tagged_string _temp272; _temp272.curr= _temp271; _temp272.base= _temp271;
_temp272.last_plus_one= _temp271 + 3; _temp272;}),( struct _tagged_string)({
char* _temp273=( char*)","; struct _tagged_string _temp274; _temp274.curr=
_temp273; _temp274.base= _temp273; _temp274.last_plus_one= _temp273 + 2;
_temp274;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc, atts)));} int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List* tms){ if( tms == 0){ return 0;}{ void* _temp276=( void*)({
struct Cyc_List_List* _temp275= tms; if( _temp275 == 0){ _throw( Null_Exception);}
_temp275->hd;}); struct Cyc_Absyn_Tqual _temp282; void* _temp284; void* _temp286;
_LL278: if(( unsigned int) _temp276 > 1u?*(( int*) _temp276) == Cyc_Absyn_Pointer_mod:
0){ _LL287: _temp286=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp276)->f1;
goto _LL285; _LL285: _temp284=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp276)->f2; goto _LL283; _LL283: _temp282=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp276)->f3; goto _LL279;} else{ goto _LL280;}
_LL280: goto _LL281; _LL279: return 1; _LL281: return 0; _LL277:;}} extern
struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc* d, struct Cyc_List_List* tms){ if( tms == 0){ return d;}{
struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc( d,({ struct Cyc_List_List*
_temp398= tms; if( _temp398 == 0){ _throw( Null_Exception);} _temp398->tl;}));
struct Cyc_PP_Doc* p_rest= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp394=( char*)"("; struct _tagged_string _temp395; _temp395.curr=
_temp394; _temp395.base= _temp394; _temp395.last_plus_one= _temp394 + 2;
_temp395;})), rest, Cyc_PP_text(( struct _tagged_string)({ char* _temp396=( char*)")";
struct _tagged_string _temp397; _temp397.curr= _temp396; _temp397.base= _temp396;
_temp397.last_plus_one= _temp396 + 2; _temp397;}))); void* _temp289=( void*)({
struct Cyc_List_List* _temp288= tms; if( _temp288 == 0){ _throw( Null_Exception);}
_temp288->hd;}); struct Cyc_Absyn_Exp* _temp303; void* _temp305; struct Cyc_List_List*
_temp307; struct Cyc_Position_Segment* _temp309; int _temp311; struct Cyc_Position_Segment*
_temp313; struct Cyc_List_List* _temp315; struct Cyc_Absyn_Tqual _temp317; void*
_temp319; void* _temp321; _LL291: if( _temp289 ==( void*) Cyc_Absyn_Carray_mod){
goto _LL292;} else{ goto _LL293;} _LL293: if(( unsigned int) _temp289 > 1u?*((
int*) _temp289) == Cyc_Absyn_ConstArray_mod: 0){ _LL304: _temp303=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ConstArray_mod_struct*) _temp289)->f1; goto _LL294;} else{ goto
_LL295;} _LL295: if(( unsigned int) _temp289 > 1u?*(( int*) _temp289) == Cyc_Absyn_Function_mod:
0){ _LL306: _temp305=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp289)->f1;
goto _LL296;} else{ goto _LL297;} _LL297: if(( unsigned int) _temp289 > 1u?*((
int*) _temp289) == Cyc_Absyn_Attributes_mod: 0){ _LL310: _temp309=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_Attributes_mod_struct*) _temp289)->f1; goto _LL308; _LL308:
_temp307=( struct Cyc_List_List*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp289)->f2; goto _LL298;} else{ goto _LL299;} _LL299: if(( unsigned int)
_temp289 > 1u?*(( int*) _temp289) == Cyc_Absyn_TypeParams_mod: 0){ _LL316:
_temp315=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp289)->f1; goto _LL314; _LL314: _temp313=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp289)->f2; goto _LL312; _LL312:
_temp311=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp289)->f3; goto
_LL300;} else{ goto _LL301;} _LL301: if(( unsigned int) _temp289 > 1u?*(( int*)
_temp289) == Cyc_Absyn_Pointer_mod: 0){ _LL322: _temp321=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp289)->f1; goto _LL320; _LL320: _temp319=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp289)->f2; goto _LL318; _LL318: _temp317=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp289)->f3; goto _LL302;} else{ goto _LL290;}
_LL292: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List* _temp323= tms;
if( _temp323 == 0){ _throw( Null_Exception);} _temp323->tl;}))){ rest= p_rest;}
return Cyc_Absynpp_cat2( rest, Cyc_PP_text(( struct _tagged_string)({ char*
_temp324=( char*)"[]"; struct _tagged_string _temp325; _temp325.curr= _temp324;
_temp325.base= _temp324; _temp325.last_plus_one= _temp324 + 3; _temp325;})));
_LL294: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List* _temp326= tms;
if( _temp326 == 0){ _throw( Null_Exception);} _temp326->tl;}))){ rest= p_rest;}
return Cyc_Absynpp_cat4( rest, Cyc_PP_text(( struct _tagged_string)({ char*
_temp327=( char*)"["; struct _tagged_string _temp328; _temp328.curr= _temp327;
_temp328.base= _temp327; _temp328.last_plus_one= _temp327 + 2; _temp328;})), Cyc_Absynpp_exp2doc(
_temp303), Cyc_PP_text(( struct _tagged_string)({ char* _temp329=( char*)"]";
struct _tagged_string _temp330; _temp330.curr= _temp329; _temp330.base= _temp329;
_temp330.last_plus_one= _temp329 + 2; _temp330;}))); _LL296: if( Cyc_Absynpp_next_is_pointer(({
struct Cyc_List_List* _temp331= tms; if( _temp331 == 0){ _throw( Null_Exception);}
_temp331->tl;}))){ rest= p_rest;}{ void* _temp332= _temp305; struct Cyc_Core_Opt*
_temp338; int _temp340; struct Cyc_List_List* _temp342; struct Cyc_Position_Segment*
_temp344; struct Cyc_List_List* _temp346; _LL334: if(*(( int*) _temp332) == Cyc_Absyn_WithTypes){
_LL343: _temp342=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp332)->f1; goto _LL341; _LL341: _temp340=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp332)->f2; goto _LL339; _LL339: _temp338=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp332)->f3; goto _LL335;} else{ goto _LL336;}
_LL336: if(*(( int*) _temp332) == Cyc_Absyn_NoTypes){ _LL347: _temp346=( struct
Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp332)->f1; goto _LL345;
_LL345: _temp344=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp332)->f2; goto _LL337;} else{ goto _LL333;} _LL335: return Cyc_Absynpp_cat2(
rest, Cyc_Absynpp_funargs2doc( _temp342, _temp340, _temp338)); _LL337: return
Cyc_Absynpp_cat2( rest, Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp348=( char*)"("; struct _tagged_string _temp349; _temp349.curr= _temp348;
_temp349.base= _temp348; _temp349.last_plus_one= _temp348 + 2; _temp349;}),(
struct _tagged_string)({ char* _temp350=( char*)")"; struct _tagged_string
_temp351; _temp351.curr= _temp350; _temp351.base= _temp350; _temp351.last_plus_one=
_temp350 + 2; _temp351;}),( struct _tagged_string)({ char* _temp352=( char*)",";
struct _tagged_string _temp353; _temp353.curr= _temp352; _temp353.base= _temp352;
_temp353.last_plus_one= _temp352 + 2; _temp353;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr, _temp346))); _LL333:;} _LL298: if( Cyc_Absynpp_next_is_pointer(({
struct Cyc_List_List* _temp354= tms; if( _temp354 == 0){ _throw( Null_Exception);}
_temp354->tl;}))){ rest= p_rest;} return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_atts2doc(
_temp307)); _LL300: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List*
_temp355= tms; if( _temp355 == 0){ _throw( Null_Exception);} _temp355->tl;}))){
rest= p_rest;} if( _temp311){ return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_ktvars2doc(
_temp315));} else{ return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_tvars2doc(
_temp315));} _LL302: { struct Cyc_PP_Doc* ptr;{ void* _temp356= _temp321; struct
Cyc_Absyn_Exp* _temp364; struct Cyc_Absyn_Exp* _temp366; _LL358: if((
unsigned int) _temp356 > 1u?*(( int*) _temp356) == Cyc_Absyn_Nullable_ps: 0){
_LL365: _temp364=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp356)->f1; goto _LL359;} else{ goto _LL360;} _LL360: if(( unsigned int)
_temp356 > 1u?*(( int*) _temp356) == Cyc_Absyn_NonNullable_ps: 0){ _LL367:
_temp366=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp356)->f1; goto _LL361;} else{ goto _LL362;} _LL362: if( _temp356 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL363;} else{ goto _LL357;} _LL359: if( Cyc_Evexp_eval_const_uint_exp(
_temp364) == 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char* _temp368=(
char*)"*"; struct _tagged_string _temp369; _temp369.curr= _temp368; _temp369.base=
_temp368; _temp369.last_plus_one= _temp368 + 2; _temp369;}));} else{ ptr= Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp370=( char*)"*"; struct
_tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 2; _temp371;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp372=( char*)"{"; struct _tagged_string _temp373;
_temp373.curr= _temp372; _temp373.base= _temp372; _temp373.last_plus_one=
_temp372 + 2; _temp373;})), Cyc_Absynpp_exp2doc( _temp364), Cyc_PP_text(( struct
_tagged_string)({ char* _temp374=( char*)"}"; struct _tagged_string _temp375;
_temp375.curr= _temp374; _temp375.base= _temp374; _temp375.last_plus_one=
_temp374 + 2; _temp375;})));} goto _LL357; _LL361: if( Cyc_Evexp_eval_const_uint_exp(
_temp366) == 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char* _temp376=(
char*)"@"; struct _tagged_string _temp377; _temp377.curr= _temp376; _temp377.base=
_temp376; _temp377.last_plus_one= _temp376 + 2; _temp377;}));} else{ ptr= Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp378=( char*)"@"; struct
_tagged_string _temp379; _temp379.curr= _temp378; _temp379.base= _temp378;
_temp379.last_plus_one= _temp378 + 2; _temp379;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp380=( char*)"{"; struct _tagged_string _temp381;
_temp381.curr= _temp380; _temp381.base= _temp380; _temp381.last_plus_one=
_temp380 + 2; _temp381;})), Cyc_Absynpp_exp2doc( _temp366), Cyc_PP_text(( struct
_tagged_string)({ char* _temp382=( char*)"}"; struct _tagged_string _temp383;
_temp383.curr= _temp382; _temp383.base= _temp382; _temp383.last_plus_one=
_temp382 + 2; _temp383;})));} goto _LL357; _LL363: ptr= Cyc_PP_text(( struct
_tagged_string)({ char* _temp384=( char*)"?"; struct _tagged_string _temp385;
_temp385.curr= _temp384; _temp385.base= _temp384; _temp385.last_plus_one=
_temp384 + 2; _temp385;})); goto _LL357; _LL357:;}{ void* _temp386= Cyc_Tcutil_compress(
_temp319); _LL388: if( _temp386 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL389;}
else{ goto _LL390;} _LL390: goto _LL391; _LL389: return Cyc_Absynpp_cat2( ptr,
rest); _LL391: return Cyc_Absynpp_cat4( ptr, Cyc_Absynpp_ntyp2doc( _temp319),
Cyc_PP_text(( struct _tagged_string)({ char* _temp392=( char*)" "; struct
_tagged_string _temp393; _temp393.curr= _temp392; _temp393.base= _temp392;
_temp393.last_plus_one= _temp392 + 2; _temp393;})), rest); _LL387:;}} _LL290:;}}
struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){ void* _temp399= Cyc_Tcutil_compress(
t); _LL401: if( _temp399 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL402;} else{ goto
_LL403;} _LL403: goto _LL404; _LL402: return Cyc_PP_text(( struct _tagged_string)({
char* _temp405=( char*)"`H"; struct _tagged_string _temp406; _temp406.curr=
_temp405; _temp406.base= _temp405; _temp406.last_plus_one= _temp405 + 3;
_temp406;})); _LL404: return Cyc_Absynpp_ntyp2doc( t); _LL400:;} static void Cyc_Absynpp_effects2docs(
struct Cyc_List_List** regions, struct Cyc_List_List** effects, void* t){ void*
_temp407= Cyc_Tcutil_compress( t); void* _temp415; struct Cyc_List_List*
_temp417; _LL409: if(( unsigned int) _temp407 > 4u?*(( int*) _temp407) == Cyc_Absyn_AccessEff:
0){ _LL416: _temp415=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp407)->f1;
goto _LL410;} else{ goto _LL411;} _LL411: if(( unsigned int) _temp407 > 4u?*((
int*) _temp407) == Cyc_Absyn_JoinEff: 0){ _LL418: _temp417=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp407)->f1; goto _LL412;} else{ goto _LL413;}
_LL413: goto _LL414; _LL410:* regions=({ struct Cyc_List_List* _temp419=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp419->hd=( void*)
Cyc_Absynpp_rgn2doc( _temp415); _temp419->tl=* regions; _temp419;}); goto _LL408;
_LL412: for( 0; _temp417 != 0; _temp417=({ struct Cyc_List_List* _temp420=
_temp417; if( _temp420 == 0){ _throw( Null_Exception);} _temp420->tl;})){ Cyc_Absynpp_effects2docs(
regions, effects,( void*)({ struct Cyc_List_List* _temp421= _temp417; if(
_temp421 == 0){ _throw( Null_Exception);} _temp421->hd;}));} goto _LL408; _LL414:*
effects=({ struct Cyc_List_List* _temp422=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp422->hd=( void*) Cyc_Absynpp_ntyp2doc( t);
_temp422->tl=* effects; _temp422;}); goto _LL408; _LL408:;} struct Cyc_PP_Doc*
Cyc_Absynpp_eff2doc( void* t){ struct Cyc_List_List* regions= 0; struct Cyc_List_List*
effects= 0; Cyc_Absynpp_effects2docs(& regions,& effects, t); regions=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( regions);
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effects); if( regions == 0? effects != 0: 0){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp423=( char*)""; struct _tagged_string _temp424;
_temp424.curr= _temp423; _temp424.base= _temp423; _temp424.last_plus_one=
_temp423 + 1; _temp424;}),( struct _tagged_string)({ char* _temp425=( char*)"";
struct _tagged_string _temp426; _temp426.curr= _temp425; _temp426.base= _temp425;
_temp426.last_plus_one= _temp425 + 1; _temp426;}),( struct _tagged_string)({
char* _temp427=( char*)"+"; struct _tagged_string _temp428; _temp428.curr=
_temp427; _temp428.base= _temp427; _temp428.last_plus_one= _temp427 + 2;
_temp428;}), effects);} else{ struct Cyc_PP_Doc* doc1= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp436=( char*)"{"; struct _tagged_string
_temp437; _temp437.curr= _temp436; _temp437.base= _temp436; _temp437.last_plus_one=
_temp436 + 2; _temp437;}),( struct _tagged_string)({ char* _temp438=( char*)"}";
struct _tagged_string _temp439; _temp439.curr= _temp438; _temp439.base= _temp438;
_temp439.last_plus_one= _temp438 + 2; _temp439;}),( struct _tagged_string)({
char* _temp440=( char*)","; struct _tagged_string _temp441; _temp441.curr=
_temp440; _temp441.base= _temp440; _temp441.last_plus_one= _temp440 + 2;
_temp441;}), regions); return Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp429=( char*)""; struct _tagged_string _temp430; _temp430.curr= _temp429;
_temp430.base= _temp429; _temp430.last_plus_one= _temp429 + 1; _temp430;}),(
struct _tagged_string)({ char* _temp431=( char*)""; struct _tagged_string
_temp432; _temp432.curr= _temp431; _temp432.base= _temp431; _temp432.last_plus_one=
_temp431 + 1; _temp432;}),( struct _tagged_string)({ char* _temp433=( char*)"+";
struct _tagged_string _temp434; _temp434.curr= _temp433; _temp434.base= _temp433;
_temp434.last_plus_one= _temp433 + 2; _temp434;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( effects,({
struct Cyc_List_List* _temp435=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp435->hd=( void*) doc1; _temp435->tl= 0; _temp435;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc* s;{ void*
_temp442= t; struct Cyc_Absyn_Exp* _temp488; struct Cyc_Absyn_Tqual _temp490;
void* _temp492; struct Cyc_Absyn_FnInfo _temp494; struct Cyc_Absyn_PtrInfo
_temp496; int _temp498; struct Cyc_Core_Opt* _temp500; void* _temp502; struct
Cyc_Absyn_Tvar* _temp504; struct Cyc_Absyn_TunionInfo _temp506; void* _temp508;
struct Cyc_List_List* _temp510; void* _temp512; struct Cyc_Absyn_TunionFieldInfo
_temp514; struct Cyc_List_List* _temp516; void* _temp518; void* _temp520; void*
_temp522; struct Cyc_List_List* _temp524; struct Cyc_Absyn_Structdecl** _temp526;
struct Cyc_List_List* _temp528; struct _tuple0* _temp530; struct Cyc_Absyn_Uniondecl**
_temp532; struct Cyc_List_List* _temp534; struct _tuple0* _temp536; struct Cyc_List_List*
_temp538; struct Cyc_List_List* _temp540; struct Cyc_Absyn_Enumdecl* _temp542;
struct _tuple0* _temp544; struct Cyc_Core_Opt* _temp546; struct Cyc_List_List*
_temp548; struct _tuple0* _temp550; void* _temp552; void* _temp554; struct Cyc_List_List*
_temp556; _LL444: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_ArrayType:
0){ _LL493: _temp492=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp442)->f1;
goto _LL491; _LL491: _temp490=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp442)->f2; goto _LL489; _LL489: _temp488=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp442)->f3; goto _LL445;} else{ goto _LL446;}
_LL446: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_FnType:
0){ _LL495: _temp494=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp442)->f1; goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int)
_temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_PointerType: 0){ _LL497: _temp496=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp442)->f1;
goto _LL449;} else{ goto _LL450;} _LL450: if( _temp442 ==( void*) Cyc_Absyn_VoidType){
goto _LL451;} else{ goto _LL452;} _LL452: if(( unsigned int) _temp442 > 4u?*((
int*) _temp442) == Cyc_Absyn_Evar: 0){ _LL503: _temp502=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp442)->f1; goto _LL501; _LL501: _temp500=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp442)->f2; goto _LL499; _LL499: _temp498=( int)((
struct Cyc_Absyn_Evar_struct*) _temp442)->f3; goto _LL453;} else{ goto _LL454;}
_LL454: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_VarType:
0){ _LL505: _temp504=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp442)->f1; goto _LL455;} else{ goto _LL456;} _LL456: if(( unsigned int)
_temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_TunionType: 0){ _LL507: _temp506=(
struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*) _temp442)->f1;
_LL513: _temp512=( void*) _temp506.tunion_info; goto _LL511; _LL511: _temp510=(
struct Cyc_List_List*) _temp506.targs; goto _LL509; _LL509: _temp508=( void*)
_temp506.rgn; goto _LL457;} else{ goto _LL458;} _LL458: if(( unsigned int)
_temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_TunionFieldType: 0){ _LL515:
_temp514=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp442)->f1; _LL519: _temp518=( void*) _temp514.field_info; goto _LL517;
_LL517: _temp516=( struct Cyc_List_List*) _temp514.targs; goto _LL459;} else{
goto _LL460;} _LL460: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) ==
Cyc_Absyn_IntType: 0){ _LL523: _temp522=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp442)->f1; goto _LL521; _LL521: _temp520=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp442)->f2; goto _LL461;} else{ goto _LL462;} _LL462: if( _temp442 ==( void*)
Cyc_Absyn_FloatType){ goto _LL463;} else{ goto _LL464;} _LL464: if( _temp442 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL465;} else{ goto _LL466;} _LL466: if((
unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_TupleType: 0){
_LL525: _temp524=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp442)->f1; goto _LL467;} else{ goto _LL468;} _LL468: if(( unsigned int)
_temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_StructType: 0){ _LL531: _temp530=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp442)->f1; goto
_LL529; _LL529: _temp528=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp442)->f2; goto _LL527; _LL527: _temp526=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp442)->f3; goto _LL469;} else{ goto
_LL470;} _LL470: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_UnionType:
0){ _LL537: _temp536=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp442)->f1; goto _LL535; _LL535: _temp534=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp442)->f2; goto _LL533; _LL533: _temp532=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp442)->f3;
goto _LL471;} else{ goto _LL472;} _LL472: if(( unsigned int) _temp442 > 4u?*((
int*) _temp442) == Cyc_Absyn_AnonStructType: 0){ _LL539: _temp538=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp442)->f1; goto _LL473;} else{ goto
_LL474;} _LL474: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_AnonUnionType:
0){ _LL541: _temp540=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp442)->f1; goto _LL475;} else{ goto _LL476;} _LL476: if(( unsigned int)
_temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_EnumType: 0){ _LL545: _temp544=(
struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp442)->f1; goto _LL543;
_LL543: _temp542=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp442)->f2; goto _LL477;} else{ goto _LL478;} _LL478: if(( unsigned int)
_temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_TypedefType: 0){ _LL551: _temp550=(
struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp442)->f1; goto
_LL549; _LL549: _temp548=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp442)->f2; goto _LL547; _LL547: _temp546=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp442)->f3; goto _LL479;} else{ goto _LL480;}
_LL480: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_RgnHandleType:
0){ _LL553: _temp552=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp442)->f1;
goto _LL481;} else{ goto _LL482;} _LL482: if( _temp442 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL483;} else{ goto _LL484;} _LL484: if(( unsigned int) _temp442 > 4u?*((
int*) _temp442) == Cyc_Absyn_AccessEff: 0){ _LL555: _temp554=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp442)->f1; goto _LL485;} else{ goto _LL486;}
_LL486: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_JoinEff:
0){ _LL557: _temp556=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp442)->f1; goto _LL487;} else{ goto _LL443;} _LL445: return Cyc_PP_text((
struct _tagged_string)({ char* _temp558=( char*)"[[[array]]]"; struct
_tagged_string _temp559; _temp559.curr= _temp558; _temp559.base= _temp558;
_temp559.last_plus_one= _temp558 + 12; _temp559;})); _LL447: return Cyc_PP_nil_doc();
_LL449: return Cyc_PP_nil_doc(); _LL451: s= Cyc_PP_text(( struct _tagged_string)({
char* _temp560=( char*)"void"; struct _tagged_string _temp561; _temp561.curr=
_temp560; _temp561.base= _temp560; _temp561.last_plus_one= _temp560 + 5;
_temp561;})); goto _LL443; _LL453: if( _temp500 != 0){ return Cyc_Absynpp_ntyp2doc((
void*)({ struct Cyc_Core_Opt* _temp562= _temp500; if( _temp562 == 0){ _throw(
Null_Exception);} _temp562->v;}));} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp563=( char*)"%"; struct _tagged_string
_temp564; _temp564.curr= _temp563; _temp564.base= _temp563; _temp564.last_plus_one=
_temp563 + 2; _temp564;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp565=( char*)"("; struct _tagged_string _temp566; _temp566.curr= _temp565;
_temp566.base= _temp565; _temp566.last_plus_one= _temp565 + 2; _temp566;})), Cyc_PP_text(
xprintf("%d", _temp498)), Cyc_PP_text(( struct _tagged_string)({ char* _temp567=(
char*)")::"; struct _tagged_string _temp568; _temp568.curr= _temp567; _temp568.base=
_temp567; _temp568.last_plus_one= _temp567 + 4; _temp568;})), Cyc_Absynpp_kind2doc(
_temp502));} goto _LL443; _LL455: s= Cyc_Absynpp_textptr( _temp504->name); if(
Cyc_Absynpp_print_all_kinds){ s= Cyc_Absynpp_cat3( s, Cyc_PP_text(( struct
_tagged_string)({ char* _temp569=( char*)"::"; struct _tagged_string _temp570;
_temp570.curr= _temp569; _temp570.base= _temp569; _temp570.last_plus_one=
_temp569 + 3; _temp570;})), Cyc_Absynpp_ckind2doc( _temp504->kind));} if( ! Cyc_Absynpp_dont_rewrite_temp_tvars?
Cyc_Tcutil_is_temp_tvar( _temp504): 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp571=( char*)"_ /* "; struct _tagged_string
_temp572; _temp572.curr= _temp571; _temp572.base= _temp571; _temp572.last_plus_one=
_temp571 + 6; _temp572;})), s, Cyc_PP_text(( struct _tagged_string)({ char*
_temp573=( char*)" */"; struct _tagged_string _temp574; _temp574.curr= _temp573;
_temp574.base= _temp573; _temp574.last_plus_one= _temp573 + 4; _temp574;})));}
goto _LL443; _LL457:{ void* _temp575= _temp512; struct Cyc_Absyn_UnknownTunionInfo
_temp581; int _temp583; struct _tuple0* _temp585; struct Cyc_Absyn_Tuniondecl*
_temp587; struct Cyc_Absyn_Tuniondecl _temp589; int _temp590; struct Cyc_Core_Opt*
_temp592; struct Cyc_List_List* _temp594; struct _tuple0* _temp596; void*
_temp598; _LL577: if(*(( int*) _temp575) == Cyc_Absyn_UnknownTunion){ _LL582:
_temp581=( struct Cyc_Absyn_UnknownTunionInfo)(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp575)->f1; _LL586: _temp585=( struct _tuple0*) _temp581.name; goto _LL584;
_LL584: _temp583=( int) _temp581.is_xtunion; goto _LL578;} else{ goto _LL579;}
_LL579: if(*(( int*) _temp575) == Cyc_Absyn_KnownTunion){ _LL588: _temp587=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*) _temp575)->f1;
_temp589=* _temp587; _LL599: _temp598=( void*) _temp589.sc; goto _LL597; _LL597:
_temp596=( struct _tuple0*) _temp589.name; goto _LL595; _LL595: _temp594=(
struct Cyc_List_List*) _temp589.tvs; goto _LL593; _LL593: _temp592=( struct Cyc_Core_Opt*)
_temp589.fields; goto _LL591; _LL591: _temp590=( int) _temp589.is_xtunion; goto
_LL580;} else{ goto _LL576;} _LL578: _temp596= _temp585; _temp590= _temp583;
goto _LL580; _LL580: { struct Cyc_PP_Doc* kw= Cyc_PP_text( _temp590?( struct
_tagged_string)({ char* _temp608=( char*)"xtunion "; struct _tagged_string
_temp609; _temp609.curr= _temp608; _temp609.base= _temp608; _temp609.last_plus_one=
_temp608 + 9; _temp609;}):( struct _tagged_string)({ char* _temp610=( char*)"tunion ";
struct _tagged_string _temp611; _temp611.curr= _temp610; _temp611.base= _temp610;
_temp611.last_plus_one= _temp610 + 8; _temp611;}));{ void* _temp600= Cyc_Tcutil_compress(
_temp508); _LL602: if( _temp600 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL603;}
else{ goto _LL604;} _LL604: goto _LL605; _LL603: s= Cyc_Absynpp_cat3( kw, Cyc_Absynpp_qvar2doc(
_temp596), Cyc_Absynpp_tps2doc( _temp510)); goto _LL601; _LL605: s= Cyc_Absynpp_cat5(
kw, Cyc_Absynpp_typ2doc( _temp508), Cyc_PP_text(( struct _tagged_string)({ char*
_temp606=( char*)" "; struct _tagged_string _temp607; _temp607.curr= _temp606;
_temp607.base= _temp606; _temp607.last_plus_one= _temp606 + 2; _temp607;})), Cyc_Absynpp_qvar2doc(
_temp596), Cyc_Absynpp_tps2doc( _temp510)); goto _LL601; _LL601:;} goto _LL576;}
_LL576:;} goto _LL443; _LL459:{ void* _temp612= _temp518; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp618; int _temp620; struct _tuple0* _temp622; struct _tuple0* _temp624;
struct Cyc_Absyn_Tunionfield* _temp626; struct Cyc_Absyn_Tunionfield _temp628;
struct Cyc_Position_Segment* _temp629; struct Cyc_List_List* _temp631; struct
Cyc_List_List* _temp633; struct _tuple0* _temp635; struct Cyc_Absyn_Tuniondecl*
_temp637; struct Cyc_Absyn_Tuniondecl _temp639; int _temp640; struct Cyc_Core_Opt*
_temp642; struct Cyc_List_List* _temp644; struct _tuple0* _temp646; void*
_temp648; _LL614: if(*(( int*) _temp612) == Cyc_Absyn_UnknownTunionfield){
_LL619: _temp618=( struct Cyc_Absyn_UnknownTunionFieldInfo)(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp612)->f1; _LL625: _temp624=( struct _tuple0*) _temp618.tunion_name; goto
_LL623; _LL623: _temp622=( struct _tuple0*) _temp618.field_name; goto _LL621;
_LL621: _temp620=( int) _temp618.is_xtunion; goto _LL615;} else{ goto _LL616;}
_LL616: if(*(( int*) _temp612) == Cyc_Absyn_KnownTunionfield){ _LL638: _temp637=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp612)->f1; _temp639=* _temp637; _LL649: _temp648=( void*) _temp639.sc; goto
_LL647; _LL647: _temp646=( struct _tuple0*) _temp639.name; goto _LL645; _LL645:
_temp644=( struct Cyc_List_List*) _temp639.tvs; goto _LL643; _LL643: _temp642=(
struct Cyc_Core_Opt*) _temp639.fields; goto _LL641; _LL641: _temp640=( int)
_temp639.is_xtunion; goto _LL627; _LL627: _temp626=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp612)->f2; _temp628=* _temp626;
_LL636: _temp635=( struct _tuple0*) _temp628.name; goto _LL634; _LL634: _temp633=(
struct Cyc_List_List*) _temp628.tvs; goto _LL632; _LL632: _temp631=( struct Cyc_List_List*)
_temp628.typs; goto _LL630; _LL630: _temp629=( struct Cyc_Position_Segment*)
_temp628.loc; goto _LL617;} else{ goto _LL613;} _LL615: _temp646= _temp624;
_temp640= _temp620; _temp635= _temp622; goto _LL617; _LL617: { struct Cyc_PP_Doc*
kw= Cyc_PP_text( _temp640?( struct _tagged_string)({ char* _temp652=( char*)"xtunion ";
struct _tagged_string _temp653; _temp653.curr= _temp652; _temp653.base= _temp652;
_temp653.last_plus_one= _temp652 + 9; _temp653;}):( struct _tagged_string)({
char* _temp654=( char*)"tunion "; struct _tagged_string _temp655; _temp655.curr=
_temp654; _temp655.base= _temp654; _temp655.last_plus_one= _temp654 + 8;
_temp655;})); s= Cyc_Absynpp_cat4( kw, Cyc_Absynpp_qvar2doc( _temp646), Cyc_PP_text((
struct _tagged_string)({ char* _temp650=( char*)"."; struct _tagged_string
_temp651; _temp651.curr= _temp650; _temp651.base= _temp650; _temp651.last_plus_one=
_temp650 + 2; _temp651;})), Cyc_Absynpp_textptr((* _temp635).f2)); goto _LL613;}
_LL613:;} goto _LL443; _LL461: { struct _tagged_string sns; struct
_tagged_string ts;{ void* _temp656= _temp522; _LL658: if( _temp656 ==( void*)
Cyc_Absyn_Signed){ goto _LL659;} else{ goto _LL660;} _LL660: if( _temp656 ==(
void*) Cyc_Absyn_Unsigned){ goto _LL661;} else{ goto _LL657;} _LL659: sns=(
struct _tagged_string)({ char* _temp662=( char*)""; struct _tagged_string
_temp663; _temp663.curr= _temp662; _temp663.base= _temp662; _temp663.last_plus_one=
_temp662 + 1; _temp663;}); goto _LL657; _LL661: sns=( struct _tagged_string)({
char* _temp664=( char*)"unsigned "; struct _tagged_string _temp665; _temp665.curr=
_temp664; _temp665.base= _temp664; _temp665.last_plus_one= _temp664 + 10;
_temp665;}); goto _LL657; _LL657:;}{ void* _temp666= _temp520; _LL668: if(
_temp666 ==( void*) Cyc_Absyn_B1){ goto _LL669;} else{ goto _LL670;} _LL670: if(
_temp666 ==( void*) Cyc_Absyn_B2){ goto _LL671;} else{ goto _LL672;} _LL672: if(
_temp666 ==( void*) Cyc_Absyn_B4){ goto _LL673;} else{ goto _LL674;} _LL674: if(
_temp666 ==( void*) Cyc_Absyn_B8){ goto _LL675;} else{ goto _LL667;} _LL669:{
void* _temp676= _temp522; _LL678: if( _temp676 ==( void*) Cyc_Absyn_Signed){
goto _LL679;} else{ goto _LL680;} _LL680: if( _temp676 ==( void*) Cyc_Absyn_Unsigned){
goto _LL681;} else{ goto _LL677;} _LL679: sns=( struct _tagged_string)({ char*
_temp682=( char*)"signed "; struct _tagged_string _temp683; _temp683.curr=
_temp682; _temp683.base= _temp682; _temp683.last_plus_one= _temp682 + 8;
_temp683;}); goto _LL677; _LL681: sns=( struct _tagged_string)({ char* _temp684=(
char*)""; struct _tagged_string _temp685; _temp685.curr= _temp684; _temp685.base=
_temp684; _temp685.last_plus_one= _temp684 + 1; _temp685;}); goto _LL677; _LL677:;}
ts=( struct _tagged_string)({ char* _temp686=( char*)"char"; struct
_tagged_string _temp687; _temp687.curr= _temp686; _temp687.base= _temp686;
_temp687.last_plus_one= _temp686 + 5; _temp687;}); goto _LL667; _LL671: ts=(
struct _tagged_string)({ char* _temp688=( char*)"short"; struct _tagged_string
_temp689; _temp689.curr= _temp688; _temp689.base= _temp688; _temp689.last_plus_one=
_temp688 + 6; _temp689;}); goto _LL667; _LL673: ts=( struct _tagged_string)({
char* _temp690=( char*)"int"; struct _tagged_string _temp691; _temp691.curr=
_temp690; _temp691.base= _temp690; _temp691.last_plus_one= _temp690 + 4;
_temp691;}); goto _LL667; _LL675: ts=( struct _tagged_string)({ char* _temp692=(
char*)"long long"; struct _tagged_string _temp693; _temp693.curr= _temp692;
_temp693.base= _temp692; _temp693.last_plus_one= _temp692 + 10; _temp693;});
goto _LL667; _LL667:;} s= Cyc_PP_text(({ struct _tagged_string _temp694= sns;
struct _tagged_string _temp695= ts; xprintf("%.*s%.*s", _temp694.last_plus_one -
_temp694.curr, _temp694.curr, _temp695.last_plus_one - _temp695.curr, _temp695.curr);}));
goto _LL443;} _LL463: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp696=(
char*)"float"; struct _tagged_string _temp697; _temp697.curr= _temp696; _temp697.base=
_temp696; _temp697.last_plus_one= _temp696 + 6; _temp697;})); goto _LL443;
_LL465: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp698=( char*)"double";
struct _tagged_string _temp699; _temp699.curr= _temp698; _temp699.base= _temp698;
_temp699.last_plus_one= _temp698 + 7; _temp699;})); goto _LL443; _LL467: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp700=( char*)"$"; struct
_tagged_string _temp701; _temp701.curr= _temp700; _temp701.base= _temp700;
_temp701.last_plus_one= _temp700 + 2; _temp701;})), Cyc_Absynpp_args2doc(
_temp524)); goto _LL443; _LL469: if( _temp530 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp702=( char*)"struct "; struct _tagged_string
_temp703; _temp703.curr= _temp702; _temp703.base= _temp702; _temp703.last_plus_one=
_temp702 + 8; _temp703;})), Cyc_Absynpp_tps2doc( _temp528));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp704=( char*)"struct "; struct
_tagged_string _temp705; _temp705.curr= _temp704; _temp705.base= _temp704;
_temp705.last_plus_one= _temp704 + 8; _temp705;})), _temp530 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp706= _temp530;
if( _temp706 == 0){ _throw( Null_Exception);} _temp706;})), Cyc_Absynpp_tps2doc(
_temp528));} goto _LL443; _LL471: if( _temp536 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp707=( char*)"union "; struct _tagged_string
_temp708; _temp708.curr= _temp707; _temp708.base= _temp707; _temp708.last_plus_one=
_temp707 + 7; _temp708;})), Cyc_Absynpp_tps2doc( _temp534));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp709=( char*)"union "; struct
_tagged_string _temp710; _temp710.curr= _temp709; _temp710.base= _temp709;
_temp710.last_plus_one= _temp709 + 7; _temp710;})), _temp536 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp711= _temp536;
if( _temp711 == 0){ _throw( Null_Exception);} _temp711;})), Cyc_Absynpp_tps2doc(
_temp534));} goto _LL443; _LL473: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp712=( char*)"struct {"; struct _tagged_string
_temp713; _temp713.curr= _temp712; _temp713.base= _temp712; _temp713.last_plus_one=
_temp712 + 9; _temp713;})), Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp538)), Cyc_PP_text(( struct _tagged_string)({ char* _temp714=( char*)"}";
struct _tagged_string _temp715; _temp715.curr= _temp714; _temp715.base= _temp714;
_temp715.last_plus_one= _temp714 + 2; _temp715;}))); goto _LL443; _LL475: s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp716=( char*)"union {"; struct
_tagged_string _temp717; _temp717.curr= _temp716; _temp717.base= _temp716;
_temp717.last_plus_one= _temp716 + 8; _temp717;})), Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp540)), Cyc_PP_text(( struct _tagged_string)({ char* _temp718=( char*)"}";
struct _tagged_string _temp719; _temp719.curr= _temp718; _temp719.base= _temp718;
_temp719.last_plus_one= _temp718 + 2; _temp719;}))); goto _LL443; _LL477: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp720=( char*)"enum "; struct
_tagged_string _temp721; _temp721.curr= _temp720; _temp721.base= _temp720;
_temp721.last_plus_one= _temp720 + 6; _temp721;})), Cyc_Absynpp_qvar2doc(
_temp544)); goto _LL443; _LL479: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp550), Cyc_Absynpp_tps2doc( _temp548)); goto _LL443; _LL481: s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp722=( char*)"region_t<";
struct _tagged_string _temp723; _temp723.curr= _temp722; _temp723.base= _temp722;
_temp723.last_plus_one= _temp722 + 10; _temp723;})), Cyc_Absynpp_rgn2doc(
_temp552), Cyc_PP_text(( struct _tagged_string)({ char* _temp724=( char*)">";
struct _tagged_string _temp725; _temp725.curr= _temp724; _temp725.base= _temp724;
_temp725.last_plus_one= _temp724 + 2; _temp725;}))); goto _LL443; _LL483: s= Cyc_Absynpp_rgn2doc(
t); goto _LL443; _LL485: s= Cyc_Absynpp_eff2doc( t); goto _LL443; _LL487: s= Cyc_Absynpp_eff2doc(
t); goto _LL443; _LL443:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp726= vo; if( _temp726 == 0){
_throw( Null_Exception);} _temp726->v;})));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc(
struct _tuple1* t){ struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt*
_temp727=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp727->v=( void*) Cyc_PP_text(*(( struct _tagged_string*)({ struct Cyc_Core_Opt*
_temp728=(* t).f1; if( _temp728 == 0){ _throw( Null_Exception);} _temp728->v;})));
_temp727;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt*
effopt){ if( effopt == 0){ if( varargs){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp729=( char*)"("; struct _tagged_string _temp730;
_temp730.curr= _temp729; _temp730.base= _temp729; _temp730.last_plus_one=
_temp729 + 2; _temp730;}),( struct _tagged_string)({ char* _temp731=( char*)")";
struct _tagged_string _temp732; _temp732.curr= _temp731; _temp732.base= _temp731;
_temp732.last_plus_one= _temp731 + 2; _temp732;}),( struct _tagged_string)({
char* _temp733=( char*)","; struct _tagged_string _temp734; _temp734.curr=
_temp733; _temp734.base= _temp733; _temp734.last_plus_one= _temp733 + 2;
_temp734;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)((( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args),({ struct Cyc_List_List* _temp735=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp735->hd=( void*) Cyc_PP_text(( struct
_tagged_string)({ char* _temp736=( char*)"..."; struct _tagged_string _temp737;
_temp737.curr= _temp736; _temp737.base= _temp736; _temp737.last_plus_one=
_temp736 + 4; _temp737;})); _temp735->tl= 0; _temp735;})));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp738=( char*)"("; struct _tagged_string
_temp739; _temp739.curr= _temp738; _temp739.base= _temp738; _temp739.last_plus_one=
_temp738 + 2; _temp739;}),( struct _tagged_string)({ char* _temp740=( char*)")";
struct _tagged_string _temp741; _temp741.curr= _temp740; _temp741.base= _temp740;
_temp741.last_plus_one= _temp740 + 2; _temp741;}),( struct _tagged_string)({
char* _temp742=( char*)","; struct _tagged_string _temp743; _temp743.curr=
_temp742; _temp743.base= _temp742; _temp743.last_plus_one= _temp742 + 2;
_temp743;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));}} else{
if( varargs){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp744=(
char*)"("; struct _tagged_string _temp745; _temp745.curr= _temp744; _temp745.base=
_temp744; _temp745.last_plus_one= _temp744 + 2; _temp745;}),( struct
_tagged_string)({ char* _temp746=( char*)")"; struct _tagged_string _temp747;
_temp747.curr= _temp746; _temp747.base= _temp746; _temp747.last_plus_one=
_temp746 + 2; _temp747;}),( struct _tagged_string)({ char* _temp748=( char*)";";
struct _tagged_string _temp749; _temp749.curr= _temp748; _temp749.base= _temp748;
_temp749.last_plus_one= _temp748 + 2; _temp749;}),({ struct Cyc_List_List*
_temp750=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp750->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp753=( char*)""; struct _tagged_string _temp754; _temp754.curr= _temp753;
_temp754.base= _temp753; _temp754.last_plus_one= _temp753 + 1; _temp754;}),(
struct _tagged_string)({ char* _temp755=( char*)""; struct _tagged_string
_temp756; _temp756.curr= _temp755; _temp756.base= _temp755; _temp756.last_plus_one=
_temp755 + 1; _temp756;}),( struct _tagged_string)({ char* _temp757=( char*)",";
struct _tagged_string _temp758; _temp758.curr= _temp757; _temp758.base= _temp757;
_temp758.last_plus_one= _temp757 + 2; _temp758;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args),({ struct Cyc_List_List*
_temp759=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp759->hd=( void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp760=(
char*)"..."; struct _tagged_string _temp761; _temp761.curr= _temp760; _temp761.base=
_temp760; _temp761.last_plus_one= _temp760 + 4; _temp761;})); _temp759->tl= 0;
_temp759;}))); _temp750->tl=({ struct Cyc_List_List* _temp751=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp751->hd=( void*) Cyc_Absynpp_eff2doc((
void*)({ struct Cyc_Core_Opt* _temp752= effopt; if( _temp752 == 0){ _throw(
Null_Exception);} _temp752->v;})); _temp751->tl= 0; _temp751;}); _temp750;}));}
else{ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp762=( char*)"(";
struct _tagged_string _temp763; _temp763.curr= _temp762; _temp763.base= _temp762;
_temp763.last_plus_one= _temp762 + 2; _temp763;}),( struct _tagged_string)({
char* _temp764=( char*)")"; struct _tagged_string _temp765; _temp765.curr=
_temp764; _temp765.base= _temp764; _temp765.last_plus_one= _temp764 + 2;
_temp765;}),( struct _tagged_string)({ char* _temp766=( char*)";"; struct
_tagged_string _temp767; _temp767.curr= _temp766; _temp767.base= _temp766;
_temp767.last_plus_one= _temp766 + 2; _temp767;}),({ struct Cyc_List_List*
_temp768=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp768->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp771=( char*)""; struct _tagged_string _temp772; _temp772.curr= _temp771;
_temp772.base= _temp771; _temp772.last_plus_one= _temp771 + 1; _temp772;}),(
struct _tagged_string)({ char* _temp773=( char*)""; struct _tagged_string
_temp774; _temp774.curr= _temp773; _temp774.base= _temp773; _temp774.last_plus_one=
_temp773 + 1; _temp774;}),( struct _tagged_string)({ char* _temp775=( char*)",";
struct _tagged_string _temp776; _temp776.curr= _temp775; _temp776.base= _temp775;
_temp776.last_plus_one= _temp775 + 2; _temp776;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_funarg2doc, args)); _temp768->tl=({ struct Cyc_List_List* _temp769=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp769->hd=(
void*) Cyc_Absynpp_eff2doc(( void*)({ struct Cyc_Core_Opt* _temp770= effopt; if(
_temp770 == 0){ _throw( Null_Exception);} _temp770->v;})); _temp769->tl= 0;
_temp769;}); _temp768;}));}}} struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct
_tuple3* arg){ return({ struct _tuple1* _temp777=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp777->f1=({ struct Cyc_Core_Opt* _temp778=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp778->v=( void*)(*
arg).f1; _temp778;}); _temp777->f2=(* arg).f2; _temp777->f3=(* arg).f3; _temp777;});}
struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct _tagged_string* v){ return Cyc_PP_text(*
v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
prefix= 0;{ void* _temp779=(* q).f1; struct Cyc_List_List* _temp791; struct Cyc_List_List*
_temp793; struct Cyc_List_List* _temp795; struct Cyc_List_List* _temp797; _LL781:
if( _temp779 ==( void*) Cyc_Absyn_Loc_n){ goto _LL782;} else{ goto _LL783;}
_LL783: if(( unsigned int) _temp779 > 1u?*(( int*) _temp779) == Cyc_Absyn_Rel_n:
0){ _LL792: _temp791=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp779)->f1; if( _temp791 == 0){ goto _LL784;} else{ goto _LL785;}} else{ goto
_LL785;} _LL785: if(( unsigned int) _temp779 > 1u?*(( int*) _temp779) == Cyc_Absyn_Abs_n:
0){ _LL794: _temp793=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp779)->f1; if( _temp793 == 0){ goto _LL786;} else{ goto _LL787;}} else{ goto
_LL787;} _LL787: if(( unsigned int) _temp779 > 1u?*(( int*) _temp779) == Cyc_Absyn_Rel_n:
0){ _LL796: _temp795=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp779)->f1; goto _LL788;} else{ goto _LL789;} _LL789: if(( unsigned int)
_temp779 > 1u?*(( int*) _temp779) == Cyc_Absyn_Abs_n: 0){ _LL798: _temp797=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp779)->f1; goto
_LL790;} else{ goto _LL780;} _LL782: goto _LL784; _LL784: goto _LL786; _LL786:
return Cyc_Absynpp_var2doc((* q).f2); _LL788: _temp797= _temp795; goto _LL790;
_LL790: prefix= _temp797; goto _LL780; _LL780:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( prefix,({ struct
Cyc_List_List* _temp799=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp799->hd=( void*)(* q).f2; _temp799->tl= 0; _temp799;})),( struct
_tagged_string)({ char* _temp800=( char*)"_"; struct _tagged_string _temp801;
_temp801.curr= _temp800; _temp801.base= _temp800; _temp801.last_plus_one=
_temp800 + 2; _temp801;})));} else{ if((( int(*)( int(* cmp)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, prefix, Cyc_Absynpp_curr_namespace)){
return Cyc_Absynpp_var2doc((* q).f2);} else{ return Cyc_PP_text( Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
prefix,({ struct Cyc_List_List* _temp802=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp802->hd=( void*)(* q).f2; _temp802->tl= 0;
_temp802;})),( struct _tagged_string)({ char* _temp803=( char*)"::"; struct
_tagged_string _temp804; _temp804.curr= _temp803; _temp804.base= _temp803;
_temp804.last_plus_one= _temp803 + 3; _temp804;})));}}} struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc(
struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){ return Cyc_Absynpp_qvar2doc(
v);}{ void* _temp805=(* v).f1; struct Cyc_List_List* _temp811; _LL807: if((
unsigned int) _temp805 > 1u?*(( int*) _temp805) == Cyc_Absyn_Abs_n: 0){ _LL812:
_temp811=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp805)->f1;
goto _LL808;} else{ goto _LL809;} _LL809: goto _LL810; _LL808: if((( int(*)( int(*
cmp)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp811,
Cyc_Absynpp_curr_namespace) == 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{
return Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp813=(
char*)"/* wrong namespace : */ "; struct _tagged_string _temp814; _temp814.curr=
_temp813; _temp814.base= _temp813; _temp814.last_plus_one= _temp813 + 25;
_temp814;})), Cyc_Absynpp_qvar2doc( v));} _LL810: return Cyc_Absynpp_qvar2doc( v);
_LL806:;}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*
e){ void* _temp815=( void*) e->r; void* _temp887; void* _temp889; struct _tuple0*
_temp891; struct _tuple0* _temp893; struct Cyc_List_List* _temp895; void*
_temp897; struct Cyc_Absyn_Exp* _temp899; struct Cyc_Core_Opt* _temp901; struct
Cyc_Absyn_Exp* _temp903; void* _temp905; struct Cyc_Absyn_Exp* _temp907; struct
Cyc_Absyn_Exp* _temp909; struct Cyc_Absyn_Exp* _temp911; struct Cyc_Absyn_Exp*
_temp913; struct Cyc_Absyn_Exp* _temp915; struct Cyc_Absyn_Exp* _temp917; struct
Cyc_List_List* _temp919; struct Cyc_Absyn_Exp* _temp921; struct Cyc_List_List*
_temp923; struct Cyc_Absyn_Exp* _temp925; struct Cyc_Absyn_Exp* _temp927; struct
Cyc_Absyn_Exp* _temp929; struct Cyc_List_List* _temp931; struct Cyc_Absyn_Exp*
_temp933; struct Cyc_Absyn_Exp* _temp935; void* _temp937; struct Cyc_Absyn_Exp*
_temp939; struct Cyc_Absyn_Exp* _temp941; struct Cyc_Absyn_Exp* _temp943; void*
_temp945; struct Cyc_Absyn_Exp* _temp947; struct Cyc_Absyn_Exp* _temp949; struct
_tagged_string* _temp951; struct Cyc_Absyn_Exp* _temp953; struct _tagged_string*
_temp955; struct Cyc_Absyn_Exp* _temp957; struct Cyc_Absyn_Exp* _temp959; struct
Cyc_Absyn_Exp* _temp961; struct Cyc_List_List* _temp963; struct Cyc_List_List*
_temp965; struct _tuple1* _temp967; struct Cyc_List_List* _temp969; struct Cyc_Absyn_Exp*
_temp971; struct Cyc_Absyn_Exp* _temp973; struct Cyc_Absyn_Vardecl* _temp975;
struct Cyc_Absyn_Structdecl* _temp977; struct Cyc_List_List* _temp979; struct
Cyc_Core_Opt* _temp981; struct _tuple0* _temp983; struct Cyc_List_List* _temp985;
void* _temp987; struct Cyc_Absyn_Tunionfield* _temp989; struct Cyc_Absyn_Tuniondecl*
_temp991; struct Cyc_List_List* _temp993; struct Cyc_Core_Opt* _temp995; struct
Cyc_Core_Opt* _temp997; struct Cyc_Absyn_Enumfield* _temp999; struct Cyc_Absyn_Enumdecl*
_temp1001; struct _tuple0* _temp1003; void* _temp1005; struct Cyc_Absyn_Exp*
_temp1007; struct Cyc_List_List* _temp1009; struct Cyc_Core_Opt* _temp1011;
struct Cyc_Absyn_Stmt* _temp1013; struct Cyc_Absyn_Fndecl* _temp1015; struct Cyc_Absyn_Exp*
_temp1017; _LL817: if(*(( int*) _temp815) == Cyc_Absyn_Const_e){ _LL888:
_temp887=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp815)->f1; goto _LL818;}
else{ goto _LL819;} _LL819: if(*(( int*) _temp815) == Cyc_Absyn_Var_e){ _LL892:
_temp891=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp815)->f1;
goto _LL890; _LL890: _temp889=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp815)->f2; goto _LL820;} else{ goto _LL821;} _LL821: if(*(( int*) _temp815)
== Cyc_Absyn_UnknownId_e){ _LL894: _temp893=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp815)->f1; goto _LL822;} else{ goto _LL823;} _LL823: if(*(( int*) _temp815)
== Cyc_Absyn_Primop_e){ _LL898: _temp897=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp815)->f1; goto _LL896; _LL896: _temp895=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp815)->f2; goto _LL824;} else{ goto _LL825;}
_LL825: if(*(( int*) _temp815) == Cyc_Absyn_AssignOp_e){ _LL904: _temp903=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp815)->f1;
goto _LL902; _LL902: _temp901=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp815)->f2; goto _LL900; _LL900: _temp899=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp815)->f3; goto _LL826;} else{ goto _LL827;}
_LL827: if(*(( int*) _temp815) == Cyc_Absyn_Increment_e){ _LL908: _temp907=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp815)->f1;
goto _LL906; _LL906: _temp905=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp815)->f2; goto _LL828;} else{ goto _LL829;} _LL829: if(*(( int*) _temp815)
== Cyc_Absyn_Conditional_e){ _LL914: _temp913=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp815)->f1; goto _LL912; _LL912: _temp911=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp815)->f2;
goto _LL910; _LL910: _temp909=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp815)->f3; goto _LL830;} else{ goto _LL831;} _LL831: if(*(( int*) _temp815)
== Cyc_Absyn_SeqExp_e){ _LL918: _temp917=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp815)->f1; goto _LL916; _LL916: _temp915=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp815)->f2; goto _LL832;} else{ goto _LL833;}
_LL833: if(*(( int*) _temp815) == Cyc_Absyn_UnknownCall_e){ _LL922: _temp921=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp815)->f1;
goto _LL920; _LL920: _temp919=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp815)->f2; goto _LL834;} else{ goto _LL835;} _LL835: if(*(( int*) _temp815)
== Cyc_Absyn_FnCall_e){ _LL926: _temp925=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp815)->f1; goto _LL924; _LL924: _temp923=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp815)->f2; goto _LL836;} else{ goto _LL837;}
_LL837: if(*(( int*) _temp815) == Cyc_Absyn_Throw_e){ _LL928: _temp927=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp815)->f1; goto _LL838;}
else{ goto _LL839;} _LL839: if(*(( int*) _temp815) == Cyc_Absyn_NoInstantiate_e){
_LL930: _temp929=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp815)->f1; goto _LL840;} else{ goto _LL841;} _LL841: if(*(( int*) _temp815)
== Cyc_Absyn_Instantiate_e){ _LL934: _temp933=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp815)->f1; goto _LL932; _LL932: _temp931=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp815)->f2;
goto _LL842;} else{ goto _LL843;} _LL843: if(*(( int*) _temp815) == Cyc_Absyn_Cast_e){
_LL938: _temp937=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp815)->f1; goto
_LL936; _LL936: _temp935=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp815)->f2; goto _LL844;} else{ goto _LL845;} _LL845: if(*(( int*) _temp815)
== Cyc_Absyn_New_e){ _LL942: _temp941=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp815)->f1; goto _LL940; _LL940: _temp939=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_New_e_struct*) _temp815)->f2; goto _LL846;} else{ goto _LL847;} _LL847:
if(*(( int*) _temp815) == Cyc_Absyn_Address_e){ _LL944: _temp943=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp815)->f1; goto _LL848;} else{ goto
_LL849;} _LL849: if(*(( int*) _temp815) == Cyc_Absyn_Sizeoftyp_e){ _LL946:
_temp945=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp815)->f1; goto
_LL850;} else{ goto _LL851;} _LL851: if(*(( int*) _temp815) == Cyc_Absyn_Sizeofexp_e){
_LL948: _temp947=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp815)->f1; goto _LL852;} else{ goto _LL853;} _LL853: if(*(( int*) _temp815)
== Cyc_Absyn_Deref_e){ _LL950: _temp949=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp815)->f1; goto _LL854;} else{ goto _LL855;} _LL855: if(*(( int*) _temp815)
== Cyc_Absyn_StructMember_e){ _LL954: _temp953=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp815)->f1; goto _LL952; _LL952: _temp951=(
struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp815)->f2;
goto _LL856;} else{ goto _LL857;} _LL857: if(*(( int*) _temp815) == Cyc_Absyn_StructArrow_e){
_LL958: _temp957=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp815)->f1; goto _LL956; _LL956: _temp955=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp815)->f2; goto _LL858;} else{ goto _LL859;}
_LL859: if(*(( int*) _temp815) == Cyc_Absyn_Subscript_e){ _LL962: _temp961=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp815)->f1;
goto _LL960; _LL960: _temp959=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp815)->f2; goto _LL860;} else{ goto _LL861;} _LL861: if(*(( int*) _temp815)
== Cyc_Absyn_Tuple_e){ _LL964: _temp963=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp815)->f1; goto _LL862;} else{ goto _LL863;} _LL863: if(*(( int*) _temp815)
== Cyc_Absyn_CompoundLit_e){ _LL968: _temp967=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp815)->f1; goto _LL966; _LL966: _temp965=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp815)->f2; goto _LL864;} else{ goto _LL865;}
_LL865: if(*(( int*) _temp815) == Cyc_Absyn_Array_e){ _LL970: _temp969=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp815)->f1; goto _LL866;}
else{ goto _LL867;} _LL867: if(*(( int*) _temp815) == Cyc_Absyn_Comprehension_e){
_LL976: _temp975=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp815)->f1; goto _LL974; _LL974: _temp973=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp815)->f2; goto _LL972; _LL972: _temp971=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp815)->f3;
goto _LL868;} else{ goto _LL869;} _LL869: if(*(( int*) _temp815) == Cyc_Absyn_Struct_e){
_LL984: _temp983=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp815)->f1; goto _LL982; _LL982: _temp981=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp815)->f2; goto _LL980; _LL980: _temp979=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp815)->f3; goto
_LL978; _LL978: _temp977=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp815)->f4; goto _LL870;} else{ goto _LL871;} _LL871: if(*(( int*) _temp815)
== Cyc_Absyn_AnonStruct_e){ _LL988: _temp987=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp815)->f1; goto _LL986; _LL986: _temp985=( struct Cyc_List_List*)(( struct
Cyc_Absyn_AnonStruct_e_struct*) _temp815)->f2; goto _LL872;} else{ goto _LL873;}
_LL873: if(*(( int*) _temp815) == Cyc_Absyn_Tunion_e){ _LL998: _temp997=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp815)->f1; goto _LL996;
_LL996: _temp995=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp815)->f2; goto _LL994; _LL994: _temp993=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp815)->f3; goto _LL992; _LL992: _temp991=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp815)->f4;
goto _LL990; _LL990: _temp989=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp815)->f5; goto _LL874;} else{ goto _LL875;} _LL875: if(*(( int*) _temp815)
== Cyc_Absyn_Enum_e){ _LL1004: _temp1003=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp815)->f1; goto _LL1002; _LL1002: _temp1001=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp815)->f2; goto _LL1000; _LL1000: _temp999=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp815)->f3;
goto _LL876;} else{ goto _LL877;} _LL877: if(*(( int*) _temp815) == Cyc_Absyn_Malloc_e){
_LL1008: _temp1007=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp815)->f1; goto _LL1006; _LL1006: _temp1005=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp815)->f2; goto _LL878;} else{ goto _LL879;} _LL879: if(*(( int*) _temp815)
== Cyc_Absyn_UnresolvedMem_e){ _LL1012: _temp1011=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp815)->f1; goto _LL1010; _LL1010:
_temp1009=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp815)->f2; goto _LL880;} else{ goto _LL881;} _LL881: if(*(( int*) _temp815)
== Cyc_Absyn_StmtExp_e){ _LL1014: _temp1013=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_StmtExp_e_struct*) _temp815)->f1; goto _LL882;} else{ goto _LL883;}
_LL883: if(*(( int*) _temp815) == Cyc_Absyn_Codegen_e){ _LL1016: _temp1015=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp815)->f1;
goto _LL884;} else{ goto _LL885;} _LL885: if(*(( int*) _temp815) == Cyc_Absyn_Fill_e){
_LL1018: _temp1017=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp815)->f1; goto _LL886;} else{ goto _LL816;} _LL818: return 10000; _LL820:
return 10000; _LL822: return 10000; _LL824: { void* _temp1019= _temp897; _LL1021:
if( _temp1019 ==( void*) Cyc_Absyn_Plus){ goto _LL1022;} else{ goto _LL1023;}
_LL1023: if( _temp1019 ==( void*) Cyc_Absyn_Times){ goto _LL1024;} else{ goto
_LL1025;} _LL1025: if( _temp1019 ==( void*) Cyc_Absyn_Minus){ goto _LL1026;}
else{ goto _LL1027;} _LL1027: if( _temp1019 ==( void*) Cyc_Absyn_Div){ goto
_LL1028;} else{ goto _LL1029;} _LL1029: if( _temp1019 ==( void*) Cyc_Absyn_Mod){
goto _LL1030;} else{ goto _LL1031;} _LL1031: if( _temp1019 ==( void*) Cyc_Absyn_Eq){
goto _LL1032;} else{ goto _LL1033;} _LL1033: if( _temp1019 ==( void*) Cyc_Absyn_Neq){
goto _LL1034;} else{ goto _LL1035;} _LL1035: if( _temp1019 ==( void*) Cyc_Absyn_Gt){
goto _LL1036;} else{ goto _LL1037;} _LL1037: if( _temp1019 ==( void*) Cyc_Absyn_Lt){
goto _LL1038;} else{ goto _LL1039;} _LL1039: if( _temp1019 ==( void*) Cyc_Absyn_Gte){
goto _LL1040;} else{ goto _LL1041;} _LL1041: if( _temp1019 ==( void*) Cyc_Absyn_Lte){
goto _LL1042;} else{ goto _LL1043;} _LL1043: if( _temp1019 ==( void*) Cyc_Absyn_Not){
goto _LL1044;} else{ goto _LL1045;} _LL1045: if( _temp1019 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1046;} else{ goto _LL1047;} _LL1047: if( _temp1019 ==( void*) Cyc_Absyn_Bitand){
goto _LL1048;} else{ goto _LL1049;} _LL1049: if( _temp1019 ==( void*) Cyc_Absyn_Bitor){
goto _LL1050;} else{ goto _LL1051;} _LL1051: if( _temp1019 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1052;} else{ goto _LL1053;} _LL1053: if( _temp1019 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1054;} else{ goto _LL1055;} _LL1055: if( _temp1019 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1056;} else{ goto _LL1057;} _LL1057: if( _temp1019 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1058;} else{ goto _LL1059;} _LL1059: if( _temp1019 ==( void*) Cyc_Absyn_Size){
goto _LL1060;} else{ goto _LL1061;} _LL1061: if( _temp1019 ==( void*) Cyc_Absyn_Printf){
goto _LL1062;} else{ goto _LL1063;} _LL1063: if( _temp1019 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1064;} else{ goto _LL1065;} _LL1065: if( _temp1019 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1066;} else{ goto _LL1067;} _LL1067: if( _temp1019 ==( void*) Cyc_Absyn_Scanf){
goto _LL1068;} else{ goto _LL1069;} _LL1069: if( _temp1019 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1070;} else{ goto _LL1071;} _LL1071: if( _temp1019 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1072;} else{ goto _LL1020;} _LL1022: return 100; _LL1024: return 110;
_LL1026: return 100; _LL1028: return 110; _LL1030: return 110; _LL1032: return
70; _LL1034: return 70; _LL1036: return 80; _LL1038: return 80; _LL1040: return
80; _LL1042: return 80; _LL1044: return 130; _LL1046: return 130; _LL1048:
return 60; _LL1050: return 40; _LL1052: return 50; _LL1054: return 90; _LL1056:
return 80; _LL1058: return 80; _LL1060: return 140; _LL1062: return 130; _LL1064:
return 130; _LL1066: return 130; _LL1068: return 130; _LL1070: return 130;
_LL1072: return 130; _LL1020:;} _LL826: return 20; _LL828: return 20; _LL830:
return 30; _LL832: return 10; _LL834: return 140; _LL836: return 140; _LL838:
return 130; _LL840: return Cyc_Absynpp_exp_prec( _temp929); _LL842: return Cyc_Absynpp_exp_prec(
_temp933); _LL844: return 120; _LL846: return 15; _LL848: return 130; _LL850:
return 130; _LL852: return 130; _LL854: return 130; _LL856: return 140; _LL858:
return 140; _LL860: return 140; _LL862: return 150; _LL864: goto _LL866; _LL866:
goto _LL868; _LL868: goto _LL870; _LL870: goto _LL872; _LL872: goto _LL874;
_LL874: goto _LL876; _LL876: goto _LL878; _LL878: return 140; _LL880: return 140;
_LL882: return 10000; _LL884: return 140; _LL886: return 140; _LL816:;} struct
Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct _tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e);
struct Cyc_PP_Doc* s;{ void* _temp1073=( void*) e->r; void* _temp1145; void*
_temp1147; struct _tuple0* _temp1149; struct _tuple0* _temp1151; struct Cyc_List_List*
_temp1153; void* _temp1155; struct Cyc_Absyn_Exp* _temp1157; struct Cyc_Core_Opt*
_temp1159; struct Cyc_Absyn_Exp* _temp1161; void* _temp1163; struct Cyc_Absyn_Exp*
_temp1165; struct Cyc_Absyn_Exp* _temp1167; struct Cyc_Absyn_Exp* _temp1169;
struct Cyc_Absyn_Exp* _temp1171; struct Cyc_Absyn_Exp* _temp1173; struct Cyc_Absyn_Exp*
_temp1175; struct Cyc_List_List* _temp1177; struct Cyc_Absyn_Exp* _temp1179;
struct Cyc_List_List* _temp1181; struct Cyc_Absyn_Exp* _temp1183; struct Cyc_Absyn_Exp*
_temp1185; struct Cyc_Absyn_Exp* _temp1187; struct Cyc_List_List* _temp1189;
struct Cyc_Absyn_Exp* _temp1191; struct Cyc_Absyn_Exp* _temp1193; void*
_temp1195; struct Cyc_Absyn_Exp* _temp1197; struct Cyc_Absyn_Exp* _temp1199;
struct Cyc_Absyn_Exp* _temp1201; void* _temp1203; struct Cyc_Absyn_Exp*
_temp1205; struct Cyc_Absyn_Exp* _temp1207; struct _tagged_string* _temp1209;
struct Cyc_Absyn_Exp* _temp1211; struct _tagged_string* _temp1213; struct Cyc_Absyn_Exp*
_temp1215; struct Cyc_Absyn_Exp* _temp1217; struct Cyc_Absyn_Exp* _temp1219;
struct Cyc_List_List* _temp1221; struct Cyc_List_List* _temp1223; struct _tuple1*
_temp1225; struct Cyc_List_List* _temp1227; struct Cyc_Absyn_Exp* _temp1229;
struct Cyc_Absyn_Exp* _temp1231; struct Cyc_Absyn_Vardecl* _temp1233; struct Cyc_Absyn_Structdecl*
_temp1235; struct Cyc_List_List* _temp1237; struct Cyc_Core_Opt* _temp1239;
struct _tuple0* _temp1241; struct Cyc_List_List* _temp1243; void* _temp1245;
struct Cyc_Absyn_Tunionfield* _temp1247; struct Cyc_Absyn_Tuniondecl* _temp1249;
struct Cyc_List_List* _temp1251; struct Cyc_Core_Opt* _temp1253; struct Cyc_Core_Opt*
_temp1255; struct Cyc_Absyn_Enumfield* _temp1257; struct Cyc_Absyn_Enumdecl*
_temp1259; struct _tuple0* _temp1261; void* _temp1263; struct Cyc_Absyn_Exp*
_temp1265; struct Cyc_List_List* _temp1267; struct Cyc_Core_Opt* _temp1269;
struct Cyc_Absyn_Stmt* _temp1271; struct Cyc_Absyn_Fndecl* _temp1273; struct Cyc_Absyn_Exp*
_temp1275; _LL1075: if(*(( int*) _temp1073) == Cyc_Absyn_Const_e){ _LL1146:
_temp1145=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1073)->f1; goto
_LL1076;} else{ goto _LL1077;} _LL1077: if(*(( int*) _temp1073) == Cyc_Absyn_Var_e){
_LL1150: _temp1149=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1073)->f1; goto _LL1148; _LL1148: _temp1147=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1073)->f2; goto _LL1078;} else{ goto _LL1079;} _LL1079: if(*(( int*)
_temp1073) == Cyc_Absyn_UnknownId_e){ _LL1152: _temp1151=( struct _tuple0*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1073)->f1; goto _LL1080;} else{ goto
_LL1081;} _LL1081: if(*(( int*) _temp1073) == Cyc_Absyn_Primop_e){ _LL1156:
_temp1155=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1073)->f1; goto
_LL1154; _LL1154: _temp1153=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1073)->f2; goto _LL1082;} else{ goto _LL1083;} _LL1083: if(*(( int*)
_temp1073) == Cyc_Absyn_AssignOp_e){ _LL1162: _temp1161=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1073)->f1; goto _LL1160; _LL1160:
_temp1159=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1073)->f2; goto _LL1158; _LL1158: _temp1157=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1073)->f3; goto _LL1084;} else{ goto
_LL1085;} _LL1085: if(*(( int*) _temp1073) == Cyc_Absyn_Increment_e){ _LL1166:
_temp1165=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1073)->f1; goto _LL1164; _LL1164: _temp1163=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1073)->f2; goto _LL1086;} else{ goto _LL1087;} _LL1087: if(*(( int*)
_temp1073) == Cyc_Absyn_Conditional_e){ _LL1172: _temp1171=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1073)->f1; goto _LL1170; _LL1170:
_temp1169=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1073)->f2; goto _LL1168; _LL1168: _temp1167=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1073)->f3; goto _LL1088;} else{
goto _LL1089;} _LL1089: if(*(( int*) _temp1073) == Cyc_Absyn_SeqExp_e){ _LL1176:
_temp1175=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1073)->f1; goto _LL1174; _LL1174: _temp1173=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1073)->f2; goto _LL1090;} else{ goto
_LL1091;} _LL1091: if(*(( int*) _temp1073) == Cyc_Absyn_UnknownCall_e){ _LL1180:
_temp1179=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1073)->f1; goto _LL1178; _LL1178: _temp1177=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1073)->f2; goto _LL1092;} else{
goto _LL1093;} _LL1093: if(*(( int*) _temp1073) == Cyc_Absyn_FnCall_e){ _LL1184:
_temp1183=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1073)->f1; goto _LL1182; _LL1182: _temp1181=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1073)->f2; goto _LL1094;} else{ goto
_LL1095;} _LL1095: if(*(( int*) _temp1073) == Cyc_Absyn_Throw_e){ _LL1186:
_temp1185=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1073)->f1;
goto _LL1096;} else{ goto _LL1097;} _LL1097: if(*(( int*) _temp1073) == Cyc_Absyn_NoInstantiate_e){
_LL1188: _temp1187=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1073)->f1; goto _LL1098;} else{ goto _LL1099;} _LL1099: if(*(( int*)
_temp1073) == Cyc_Absyn_Instantiate_e){ _LL1192: _temp1191=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1073)->f1; goto _LL1190; _LL1190:
_temp1189=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1073)->f2; goto _LL1100;} else{ goto _LL1101;} _LL1101: if(*(( int*)
_temp1073) == Cyc_Absyn_Cast_e){ _LL1196: _temp1195=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1073)->f1; goto _LL1194; _LL1194: _temp1193=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Cast_e_struct*) _temp1073)->f2; goto _LL1102;} else{ goto
_LL1103;} _LL1103: if(*(( int*) _temp1073) == Cyc_Absyn_Address_e){ _LL1198:
_temp1197=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp1073)->f1; goto _LL1104;} else{ goto _LL1105;} _LL1105: if(*(( int*)
_temp1073) == Cyc_Absyn_New_e){ _LL1202: _temp1201=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1073)->f1; goto _LL1200; _LL1200: _temp1199=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1073)->f2; goto
_LL1106;} else{ goto _LL1107;} _LL1107: if(*(( int*) _temp1073) == Cyc_Absyn_Sizeoftyp_e){
_LL1204: _temp1203=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1073)->f1;
goto _LL1108;} else{ goto _LL1109;} _LL1109: if(*(( int*) _temp1073) == Cyc_Absyn_Sizeofexp_e){
_LL1206: _temp1205=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1073)->f1; goto _LL1110;} else{ goto _LL1111;} _LL1111: if(*(( int*)
_temp1073) == Cyc_Absyn_Deref_e){ _LL1208: _temp1207=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp1073)->f1; goto _LL1112;} else{ goto
_LL1113;} _LL1113: if(*(( int*) _temp1073) == Cyc_Absyn_StructMember_e){ _LL1212:
_temp1211=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1073)->f1; goto _LL1210; _LL1210: _temp1209=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1073)->f2; goto _LL1114;} else{
goto _LL1115;} _LL1115: if(*(( int*) _temp1073) == Cyc_Absyn_StructArrow_e){
_LL1216: _temp1215=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1073)->f1; goto _LL1214; _LL1214: _temp1213=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1073)->f2; goto _LL1116;} else{
goto _LL1117;} _LL1117: if(*(( int*) _temp1073) == Cyc_Absyn_Subscript_e){
_LL1220: _temp1219=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1073)->f1; goto _LL1218; _LL1218: _temp1217=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1073)->f2; goto _LL1118;} else{ goto
_LL1119;} _LL1119: if(*(( int*) _temp1073) == Cyc_Absyn_Tuple_e){ _LL1222:
_temp1221=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1073)->f1;
goto _LL1120;} else{ goto _LL1121;} _LL1121: if(*(( int*) _temp1073) == Cyc_Absyn_CompoundLit_e){
_LL1226: _temp1225=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1073)->f1; goto _LL1224; _LL1224: _temp1223=( struct Cyc_List_List*)((
struct Cyc_Absyn_CompoundLit_e_struct*) _temp1073)->f2; goto _LL1122;} else{
goto _LL1123;} _LL1123: if(*(( int*) _temp1073) == Cyc_Absyn_Array_e){ _LL1228:
_temp1227=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1073)->f1;
goto _LL1124;} else{ goto _LL1125;} _LL1125: if(*(( int*) _temp1073) == Cyc_Absyn_Comprehension_e){
_LL1234: _temp1233=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1073)->f1; goto _LL1232; _LL1232: _temp1231=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1073)->f2; goto _LL1230; _LL1230:
_temp1229=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1073)->f3; goto _LL1126;} else{ goto _LL1127;} _LL1127: if(*(( int*)
_temp1073) == Cyc_Absyn_Struct_e){ _LL1242: _temp1241=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1073)->f1; goto _LL1240; _LL1240:
_temp1239=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1073)->f2;
goto _LL1238; _LL1238: _temp1237=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1073)->f3; goto _LL1236; _LL1236: _temp1235=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1073)->f4; goto _LL1128;} else{ goto
_LL1129;} _LL1129: if(*(( int*) _temp1073) == Cyc_Absyn_AnonStruct_e){ _LL1246:
_temp1245=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1073)->f1; goto
_LL1244; _LL1244: _temp1243=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1073)->f2; goto _LL1130;} else{ goto _LL1131;} _LL1131: if(*(( int*)
_temp1073) == Cyc_Absyn_Tunion_e){ _LL1256: _temp1255=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1073)->f1; goto _LL1254; _LL1254:
_temp1253=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1073)->f2;
goto _LL1252; _LL1252: _temp1251=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1073)->f3; goto _LL1250; _LL1250: _temp1249=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1073)->f4; goto _LL1248; _LL1248:
_temp1247=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1073)->f5; goto _LL1132;} else{ goto _LL1133;} _LL1133: if(*(( int*)
_temp1073) == Cyc_Absyn_Enum_e){ _LL1262: _temp1261=( struct _tuple0*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp1073)->f1; goto _LL1260; _LL1260: _temp1259=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1073)->f2;
goto _LL1258; _LL1258: _temp1257=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1073)->f3; goto _LL1134;} else{ goto _LL1135;} _LL1135: if(*(( int*)
_temp1073) == Cyc_Absyn_Malloc_e){ _LL1266: _temp1265=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Malloc_e_struct*) _temp1073)->f1; goto _LL1264; _LL1264:
_temp1263=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp1073)->f2; goto
_LL1136;} else{ goto _LL1137;} _LL1137: if(*(( int*) _temp1073) == Cyc_Absyn_UnresolvedMem_e){
_LL1270: _temp1269=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1073)->f1; goto _LL1268; _LL1268: _temp1267=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1073)->f2; goto _LL1138;} else{
goto _LL1139;} _LL1139: if(*(( int*) _temp1073) == Cyc_Absyn_StmtExp_e){ _LL1272:
_temp1271=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1073)->f1; goto _LL1140;} else{ goto _LL1141;} _LL1141: if(*(( int*)
_temp1073) == Cyc_Absyn_Codegen_e){ _LL1274: _temp1273=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp1073)->f1; goto _LL1142;} else{ goto
_LL1143;} _LL1143: if(*(( int*) _temp1073) == Cyc_Absyn_Fill_e){ _LL1276:
_temp1275=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1073)->f1;
goto _LL1144;} else{ goto _LL1074;} _LL1076: s= Cyc_Absynpp_cnst2doc( _temp1145);
goto _LL1074; _LL1078: _temp1151= _temp1149; goto _LL1080; _LL1080: s= Cyc_Absynpp_qvar2doc(
_temp1151); goto _LL1074; _LL1082: s= Cyc_Absynpp_primapp2doc( myprec, _temp1155,
_temp1153); goto _LL1074; _LL1084: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1161), Cyc_PP_text(( struct _tagged_string)({ char* _temp1277=(
char*)" "; struct _tagged_string _temp1278; _temp1278.curr= _temp1277; _temp1278.base=
_temp1277; _temp1278.last_plus_one= _temp1277 + 2; _temp1278;})), _temp1159 == 0?
Cyc_PP_text(( struct _tagged_string)({ char* _temp1279=( char*)""; struct
_tagged_string _temp1280; _temp1280.curr= _temp1279; _temp1280.base= _temp1279;
_temp1280.last_plus_one= _temp1279 + 1; _temp1280;})): Cyc_Absynpp_prim2doc((
void*)({ struct Cyc_Core_Opt* _temp1281= _temp1159; if( _temp1281 == 0){ _throw(
Null_Exception);} _temp1281->v;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1282=( char*)"= "; struct _tagged_string _temp1283; _temp1283.curr=
_temp1282; _temp1283.base= _temp1282; _temp1283.last_plus_one= _temp1282 + 3;
_temp1283;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1157)); goto _LL1074;
_LL1086: { struct Cyc_PP_Doc* es= Cyc_Absynpp_exp2doc_prec( myprec, _temp1165);{
void* _temp1284= _temp1163; _LL1286: if( _temp1284 ==( void*) Cyc_Absyn_PreInc){
goto _LL1287;} else{ goto _LL1288;} _LL1288: if( _temp1284 ==( void*) Cyc_Absyn_PreDec){
goto _LL1289;} else{ goto _LL1290;} _LL1290: if( _temp1284 ==( void*) Cyc_Absyn_PostInc){
goto _LL1291;} else{ goto _LL1292;} _LL1292: if( _temp1284 ==( void*) Cyc_Absyn_PostDec){
goto _LL1293;} else{ goto _LL1285;} _LL1287: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1294=( char*)"++"; struct _tagged_string
_temp1295; _temp1295.curr= _temp1294; _temp1295.base= _temp1294; _temp1295.last_plus_one=
_temp1294 + 3; _temp1295;})), es); goto _LL1285; _LL1289: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1296=( char*)"--"; struct
_tagged_string _temp1297; _temp1297.curr= _temp1296; _temp1297.base= _temp1296;
_temp1297.last_plus_one= _temp1296 + 3; _temp1297;})), es); goto _LL1285;
_LL1291: s= Cyc_Absynpp_cat2( es, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1298=( char*)"++"; struct _tagged_string _temp1299; _temp1299.curr=
_temp1298; _temp1299.base= _temp1298; _temp1299.last_plus_one= _temp1298 + 3;
_temp1299;}))); goto _LL1285; _LL1293: s= Cyc_Absynpp_cat2( es, Cyc_PP_text((
struct _tagged_string)({ char* _temp1300=( char*)"--"; struct _tagged_string
_temp1301; _temp1301.curr= _temp1300; _temp1301.base= _temp1300; _temp1301.last_plus_one=
_temp1300 + 3; _temp1301;}))); goto _LL1285; _LL1285:;} goto _LL1074;} _LL1088:{
struct _tuple8 _temp1303=({ struct _tuple8 _temp1302; _temp1302.f1=( void*)
_temp1169->r; _temp1302.f2=( void*) _temp1167->r; _temp1302;}); _LL1305: goto
_LL1306; _LL1306: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1171), Cyc_PP_text(( struct _tagged_string)({ char* _temp1307=( char*)" ? ";
struct _tagged_string _temp1308; _temp1308.curr= _temp1307; _temp1308.base=
_temp1307; _temp1308.last_plus_one= _temp1307 + 4; _temp1308;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1169), Cyc_PP_text(( struct _tagged_string)({ char* _temp1309=( char*)" : ";
struct _tagged_string _temp1310; _temp1310.curr= _temp1309; _temp1310.base=
_temp1309; _temp1310.last_plus_one= _temp1309 + 4; _temp1310;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1167)); goto _LL1304; _LL1304:;} goto _LL1074; _LL1090: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1175), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1311=( char*)", "; struct _tagged_string _temp1312;
_temp1312.curr= _temp1311; _temp1312.base= _temp1311; _temp1312.last_plus_one=
_temp1311 + 3; _temp1312;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1173));
goto _LL1074; _LL1092: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1179), Cyc_PP_text(( struct _tagged_string)({ char* _temp1313=( char*)"(";
struct _tagged_string _temp1314; _temp1314.curr= _temp1313; _temp1314.base=
_temp1313; _temp1314.last_plus_one= _temp1313 + 2; _temp1314;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1177), Cyc_PP_text(( struct _tagged_string)({ char* _temp1315=( char*)")";
struct _tagged_string _temp1316; _temp1316.curr= _temp1315; _temp1316.base=
_temp1315; _temp1316.last_plus_one= _temp1315 + 2; _temp1316;}))); goto _LL1074;
_LL1094: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1183), Cyc_PP_text((
struct _tagged_string)({ char* _temp1317=( char*)"("; struct _tagged_string
_temp1318; _temp1318.curr= _temp1317; _temp1318.base= _temp1317; _temp1318.last_plus_one=
_temp1317 + 2; _temp1318;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1181), Cyc_PP_text((
struct _tagged_string)({ char* _temp1319=( char*)")"; struct _tagged_string
_temp1320; _temp1320.curr= _temp1319; _temp1320.base= _temp1319; _temp1320.last_plus_one=
_temp1319 + 2; _temp1320;}))); goto _LL1074; _LL1096: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1321=( char*)"throw "; struct _tagged_string
_temp1322; _temp1322.curr= _temp1321; _temp1322.base= _temp1321; _temp1322.last_plus_one=
_temp1321 + 7; _temp1322;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1185));
goto _LL1074; _LL1098: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1187); goto
_LL1074; _LL1100: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1191); goto _LL1074;
_LL1102: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1323=( char*)"("; struct _tagged_string _temp1324; _temp1324.curr=
_temp1323; _temp1324.base= _temp1323; _temp1324.last_plus_one= _temp1323 + 2;
_temp1324;})), Cyc_Absynpp_typ2doc( _temp1195), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1325=( char*)")"; struct _tagged_string _temp1326;
_temp1326.curr= _temp1325; _temp1326.base= _temp1325; _temp1326.last_plus_one=
_temp1325 + 2; _temp1326;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1193));
goto _LL1074; _LL1104: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({
char* _temp1327=( char*)"&"; struct _tagged_string _temp1328; _temp1328.curr=
_temp1327; _temp1328.base= _temp1327; _temp1328.last_plus_one= _temp1327 + 2;
_temp1328;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1197)); goto _LL1074;
_LL1106: if( _temp1201 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1329=( char*)"new "; struct _tagged_string
_temp1330; _temp1330.curr= _temp1329; _temp1330.base= _temp1329; _temp1330.last_plus_one=
_temp1329 + 5; _temp1330;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1199));}
else{ s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1331=( char*)"rnew("; struct _tagged_string _temp1332; _temp1332.curr=
_temp1331; _temp1332.base= _temp1331; _temp1332.last_plus_one= _temp1331 + 6;
_temp1332;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1333= _temp1201; if( _temp1333 == 0){ _throw( Null_Exception);} _temp1333;})),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1334=( char*)") "; struct
_tagged_string _temp1335; _temp1335.curr= _temp1334; _temp1335.base= _temp1334;
_temp1335.last_plus_one= _temp1334 + 3; _temp1335;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1199));} goto _LL1074; _LL1108: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1336=( char*)"sizeof("; struct
_tagged_string _temp1337; _temp1337.curr= _temp1336; _temp1337.base= _temp1336;
_temp1337.last_plus_one= _temp1336 + 8; _temp1337;})), Cyc_Absynpp_typ2doc(
_temp1203), Cyc_PP_text(( struct _tagged_string)({ char* _temp1338=( char*)")";
struct _tagged_string _temp1339; _temp1339.curr= _temp1338; _temp1339.base=
_temp1338; _temp1339.last_plus_one= _temp1338 + 2; _temp1339;}))); goto _LL1074;
_LL1110: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1340=( char*)"sizeof("; struct _tagged_string _temp1341; _temp1341.curr=
_temp1340; _temp1341.base= _temp1340; _temp1341.last_plus_one= _temp1340 + 8;
_temp1341;})), Cyc_Absynpp_exp2doc( _temp1205), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1342=( char*)")"; struct _tagged_string _temp1343;
_temp1343.curr= _temp1342; _temp1343.base= _temp1342; _temp1343.last_plus_one=
_temp1342 + 2; _temp1343;}))); goto _LL1074; _LL1112: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1344=( char*)"*"; struct _tagged_string
_temp1345; _temp1345.curr= _temp1344; _temp1345.base= _temp1344; _temp1345.last_plus_one=
_temp1344 + 2; _temp1345;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1207));
goto _LL1074; _LL1114: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1211), Cyc_PP_text(( struct _tagged_string)({ char* _temp1346=( char*)".";
struct _tagged_string _temp1347; _temp1347.curr= _temp1346; _temp1347.base=
_temp1346; _temp1347.last_plus_one= _temp1346 + 2; _temp1347;})), Cyc_Absynpp_textptr(
_temp1209)); goto _LL1074; _LL1116: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1215), Cyc_PP_text(( struct _tagged_string)({ char* _temp1348=(
char*)"->"; struct _tagged_string _temp1349; _temp1349.curr= _temp1348;
_temp1349.base= _temp1348; _temp1349.last_plus_one= _temp1348 + 3; _temp1349;})),
Cyc_Absynpp_textptr( _temp1213)); goto _LL1074; _LL1118: s= Cyc_Absynpp_cat4(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1219), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1350=( char*)"["; struct _tagged_string _temp1351;
_temp1351.curr= _temp1350; _temp1351.base= _temp1350; _temp1351.last_plus_one=
_temp1350 + 2; _temp1351;})), Cyc_Absynpp_exp2doc( _temp1217), Cyc_PP_text((
struct _tagged_string)({ char* _temp1352=( char*)"]"; struct _tagged_string
_temp1353; _temp1353.curr= _temp1352; _temp1353.base= _temp1352; _temp1353.last_plus_one=
_temp1352 + 2; _temp1353;}))); goto _LL1074; _LL1120: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1354=( char*)"$("; struct _tagged_string
_temp1355; _temp1355.curr= _temp1354; _temp1355.base= _temp1354; _temp1355.last_plus_one=
_temp1354 + 3; _temp1355;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1221), Cyc_PP_text((
struct _tagged_string)({ char* _temp1356=( char*)")"; struct _tagged_string
_temp1357; _temp1357.curr= _temp1356; _temp1357.base= _temp1356; _temp1357.last_plus_one=
_temp1356 + 2; _temp1357;}))); goto _LL1074; _LL1122: s= Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1358=( char*)"("; struct _tagged_string
_temp1359; _temp1359.curr= _temp1358; _temp1359.base= _temp1358; _temp1359.last_plus_one=
_temp1358 + 2; _temp1359;})), Cyc_Absynpp_typ2doc((* _temp1225).f3), Cyc_PP_text((
struct _tagged_string)({ char* _temp1360=( char*)")"; struct _tagged_string
_temp1361; _temp1361.curr= _temp1360; _temp1361.base= _temp1360; _temp1361.last_plus_one=
_temp1360 + 2; _temp1361;})), Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp1362=( char*)"{"; struct _tagged_string _temp1363; _temp1363.curr=
_temp1362; _temp1363.base= _temp1362; _temp1363.last_plus_one= _temp1362 + 2;
_temp1363;}),( struct _tagged_string)({ char* _temp1364=( char*)"}"; struct
_tagged_string _temp1365; _temp1365.curr= _temp1364; _temp1365.base= _temp1364;
_temp1365.last_plus_one= _temp1364 + 2; _temp1365;}),( struct _tagged_string)({
char* _temp1366=( char*)","; struct _tagged_string _temp1367; _temp1367.curr=
_temp1366; _temp1367.base= _temp1366; _temp1367.last_plus_one= _temp1366 + 2;
_temp1367;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1223))); goto _LL1074; _LL1124: s= Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1368=( char*)"{"; struct _tagged_string _temp1369;
_temp1369.curr= _temp1368; _temp1369.base= _temp1368; _temp1369.last_plus_one=
_temp1368 + 2; _temp1369;}),( struct _tagged_string)({ char* _temp1370=( char*)"}";
struct _tagged_string _temp1371; _temp1371.curr= _temp1370; _temp1371.base=
_temp1370; _temp1371.last_plus_one= _temp1370 + 2; _temp1371;}),( struct
_tagged_string)({ char* _temp1372=( char*)","; struct _tagged_string _temp1373;
_temp1373.curr= _temp1372; _temp1373.base= _temp1372; _temp1373.last_plus_one=
_temp1372 + 2; _temp1373;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1227)); goto _LL1074; _LL1126: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1374=( char*)"{for "; struct _tagged_string
_temp1375; _temp1375.curr= _temp1374; _temp1375.base= _temp1374; _temp1375.last_plus_one=
_temp1374 + 6; _temp1375;})), Cyc_PP_text(*(* _temp1233->name).f2), Cyc_PP_text((
struct _tagged_string)({ char* _temp1376=( char*)" < "; struct _tagged_string
_temp1377; _temp1377.curr= _temp1376; _temp1377.base= _temp1376; _temp1377.last_plus_one=
_temp1376 + 4; _temp1377;})), Cyc_Absynpp_exp2doc( _temp1231), Cyc_PP_text((
struct _tagged_string)({ char* _temp1378=( char*)" : "; struct _tagged_string
_temp1379; _temp1379.curr= _temp1378; _temp1379.base= _temp1378; _temp1379.last_plus_one=
_temp1378 + 4; _temp1379;})), Cyc_Absynpp_exp2doc( _temp1229), Cyc_PP_text((
struct _tagged_string)({ char* _temp1380=( char*)"}"; struct _tagged_string
_temp1381; _temp1381.curr= _temp1380; _temp1381.base= _temp1380; _temp1381.last_plus_one=
_temp1380 + 2; _temp1381;}))); goto _LL1074; _LL1128: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1241), Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1382=( char*)"{";
struct _tagged_string _temp1383; _temp1383.curr= _temp1382; _temp1383.base=
_temp1382; _temp1383.last_plus_one= _temp1382 + 2; _temp1383;}),( struct
_tagged_string)({ char* _temp1384=( char*)"}"; struct _tagged_string _temp1385;
_temp1385.curr= _temp1384; _temp1385.base= _temp1384; _temp1385.last_plus_one=
_temp1384 + 2; _temp1385;}),( struct _tagged_string)({ char* _temp1386=( char*)",";
struct _tagged_string _temp1387; _temp1387.curr= _temp1386; _temp1387.base=
_temp1386; _temp1387.last_plus_one= _temp1386 + 2; _temp1387;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1237))); goto _LL1074; _LL1130: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1388=( char*)"{"; struct _tagged_string
_temp1389; _temp1389.curr= _temp1388; _temp1389.base= _temp1388; _temp1389.last_plus_one=
_temp1388 + 2; _temp1389;}),( struct _tagged_string)({ char* _temp1390=( char*)"}";
struct _tagged_string _temp1391; _temp1391.curr= _temp1390; _temp1391.base=
_temp1390; _temp1391.last_plus_one= _temp1390 + 2; _temp1391;}),( struct
_tagged_string)({ char* _temp1392=( char*)","; struct _tagged_string _temp1393;
_temp1393.curr= _temp1392; _temp1393.base= _temp1392; _temp1393.last_plus_one=
_temp1392 + 2; _temp1393;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1243)); goto _LL1074; _LL1132: if( _temp1251 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1247->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1247->name),
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1394=( char*)"(";
struct _tagged_string _temp1395; _temp1395.curr= _temp1394; _temp1395.base=
_temp1394; _temp1395.last_plus_one= _temp1394 + 2; _temp1395;}),( struct
_tagged_string)({ char* _temp1396=( char*)")"; struct _tagged_string _temp1397;
_temp1397.curr= _temp1396; _temp1397.base= _temp1396; _temp1397.last_plus_one=
_temp1396 + 2; _temp1397;}),( struct _tagged_string)({ char* _temp1398=( char*)",";
struct _tagged_string _temp1399; _temp1399.curr= _temp1398; _temp1399.base=
_temp1398; _temp1399.last_plus_one= _temp1398 + 2; _temp1399;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp1251)));} goto _LL1074; _LL1134: s= Cyc_Absynpp_qvar2doc(
_temp1261); goto _LL1074; _LL1136: if( _temp1265 == 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1400=( char*)"malloc(sizeof("; struct
_tagged_string _temp1401; _temp1401.curr= _temp1400; _temp1401.base= _temp1400;
_temp1401.last_plus_one= _temp1400 + 15; _temp1401;})), Cyc_Absynpp_typ2doc(
_temp1263), Cyc_PP_text(( struct _tagged_string)({ char* _temp1402=( char*)"))";
struct _tagged_string _temp1403; _temp1403.curr= _temp1402; _temp1403.base=
_temp1402; _temp1403.last_plus_one= _temp1402 + 3; _temp1403;})));} else{ s= Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1404=( char*)"rmalloc(";
struct _tagged_string _temp1405; _temp1405.curr= _temp1404; _temp1405.base=
_temp1404; _temp1405.last_plus_one= _temp1404 + 9; _temp1405;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1406= _temp1265; if(
_temp1406 == 0){ _throw( Null_Exception);} _temp1406;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1407=( char*)","; struct _tagged_string _temp1408;
_temp1408.curr= _temp1407; _temp1408.base= _temp1407; _temp1408.last_plus_one=
_temp1407 + 2; _temp1408;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1409=( char*)"sizeof("; struct _tagged_string _temp1410; _temp1410.curr=
_temp1409; _temp1410.base= _temp1409; _temp1410.last_plus_one= _temp1409 + 8;
_temp1410;})), Cyc_Absynpp_typ2doc( _temp1263), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1411=( char*)"))"; struct _tagged_string _temp1412;
_temp1412.curr= _temp1411; _temp1412.base= _temp1411; _temp1412.last_plus_one=
_temp1411 + 3; _temp1412;})));} goto _LL1074; _LL1138: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1413=( char*)"{"; struct _tagged_string
_temp1414; _temp1414.curr= _temp1413; _temp1414.base= _temp1413; _temp1414.last_plus_one=
_temp1413 + 2; _temp1414;}),( struct _tagged_string)({ char* _temp1415=( char*)"}";
struct _tagged_string _temp1416; _temp1416.curr= _temp1415; _temp1416.base=
_temp1415; _temp1416.last_plus_one= _temp1415 + 2; _temp1416;}),( struct
_tagged_string)({ char* _temp1417=( char*)","; struct _tagged_string _temp1418;
_temp1418.curr= _temp1417; _temp1418.base= _temp1417; _temp1418.last_plus_one=
_temp1417 + 2; _temp1418;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1267)); goto _LL1074; _LL1140: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1419=( char*)"({ "; struct _tagged_string _temp1420;
_temp1420.curr= _temp1419; _temp1420.base= _temp1419; _temp1420.last_plus_one=
_temp1419 + 4; _temp1420;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1271)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1421=( char*)" })"; struct
_tagged_string _temp1422; _temp1422.curr= _temp1421; _temp1422.base= _temp1421;
_temp1422.last_plus_one= _temp1421 + 4; _temp1422;}))); goto _LL1074; _LL1142: s=
Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1423=( char*)"codegen(";
struct _tagged_string _temp1424; _temp1424.curr= _temp1423; _temp1424.base=
_temp1423; _temp1424.last_plus_one= _temp1423 + 9; _temp1424;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1425=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1425->r=( void*)(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp1426=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1426[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1427; _temp1427.tag= Cyc_Absyn_Fn_d; _temp1427.f1= _temp1273; _temp1427;});
_temp1426;})); _temp1425->loc= e->loc; _temp1425;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1428=( char*)")"; struct _tagged_string _temp1429;
_temp1429.curr= _temp1428; _temp1429.base= _temp1428; _temp1429.last_plus_one=
_temp1428 + 2; _temp1429;}))); goto _LL1074; _LL1144: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1430=( char*)"fill("; struct _tagged_string
_temp1431; _temp1431.curr= _temp1430; _temp1431.base= _temp1430; _temp1431.last_plus_one=
_temp1430 + 6; _temp1431;})), Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1275)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1432=( char*)")"; struct
_tagged_string _temp1433; _temp1433.curr= _temp1432; _temp1433.base= _temp1432;
_temp1433.last_plus_one= _temp1432 + 2; _temp1433;}))); goto _LL1074; _LL1074:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp1434=( char*)"("; struct _tagged_string _temp1435; _temp1435.curr=
_temp1434; _temp1435.base= _temp1434; _temp1435.last_plus_one= _temp1434 + 2;
_temp1435;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp1436=( char*)")";
struct _tagged_string _temp1437; _temp1437.curr= _temp1436; _temp1437.base=
_temp1436; _temp1437.last_plus_one= _temp1436 + 2; _temp1437;})));} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1438= d;
struct Cyc_Absyn_Exp* _temp1444; struct _tagged_string* _temp1446; _LL1440: if(*((
int*) _temp1438) == Cyc_Absyn_ArrayElement){ _LL1445: _temp1444=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1438)->f1; goto _LL1441;} else{ goto
_LL1442;} _LL1442: if(*(( int*) _temp1438) == Cyc_Absyn_FieldName){ _LL1447:
_temp1446=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1438)->f1; goto _LL1443;} else{ goto _LL1439;} _LL1441: return Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1448=( char*)".["; struct
_tagged_string _temp1449; _temp1449.curr= _temp1448; _temp1449.base= _temp1448;
_temp1449.last_plus_one= _temp1448 + 3; _temp1449;})), Cyc_Absynpp_exp2doc(
_temp1444), Cyc_PP_text(( struct _tagged_string)({ char* _temp1450=( char*)"]";
struct _tagged_string _temp1451; _temp1451.curr= _temp1450; _temp1451.base=
_temp1450; _temp1451.last_plus_one= _temp1450 + 2; _temp1451;}))); _LL1443:
return Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp1452=(
char*)"."; struct _tagged_string _temp1453; _temp1453.curr= _temp1452; _temp1453.base=
_temp1452; _temp1453.last_plus_one= _temp1452 + 2; _temp1453;})), Cyc_Absynpp_textptr(
_temp1446)); _LL1439:;} struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6*
de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((* de).f2);} else{ return
Cyc_Absynpp_cat2( Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1454=(
char*)""; struct _tagged_string _temp1455; _temp1455.curr= _temp1454; _temp1455.base=
_temp1454; _temp1455.last_plus_one= _temp1454 + 1; _temp1455;}),( struct
_tagged_string)({ char* _temp1456=( char*)"="; struct _tagged_string _temp1457;
_temp1457.curr= _temp1456; _temp1457.base= _temp1456; _temp1457.last_plus_one=
_temp1456 + 2; _temp1457;}),( struct _tagged_string)({ char* _temp1458=( char*)"=";
struct _tagged_string _temp1459; _temp1459.curr= _temp1458; _temp1459.base=
_temp1458; _temp1459.last_plus_one= _temp1458 + 2; _temp1459;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)), Cyc_Absynpp_exp2doc((* de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1460=( char*)""; struct _tagged_string _temp1461;
_temp1461.curr= _temp1460; _temp1461.base= _temp1460; _temp1461.last_plus_one=
_temp1460 + 1; _temp1461;}),( struct _tagged_string)({ char* _temp1462=( char*)"";
struct _tagged_string _temp1463; _temp1463.curr= _temp1462; _temp1463.base=
_temp1462; _temp1463.last_plus_one= _temp1462 + 1; _temp1463;}),( struct
_tagged_string)({ char* _temp1464=( char*)","; struct _tagged_string _temp1465;
_temp1465.curr= _temp1464; _temp1465.base= _temp1464; _temp1465.last_plus_one=
_temp1464 + 2; _temp1465;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1466= c; char _temp1484; void* _temp1486; short _temp1488;
void* _temp1490; int _temp1492; void* _temp1494; int _temp1496; void* _temp1498;
long long _temp1500; void* _temp1502; struct _tagged_string _temp1504; struct
_tagged_string _temp1506; _LL1468: if(( unsigned int) _temp1466 > 1u?*(( int*)
_temp1466) == Cyc_Absyn_Char_c: 0){ _LL1487: _temp1486=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp1466)->f1; goto _LL1485; _LL1485: _temp1484=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp1466)->f2; goto _LL1469;} else{ goto _LL1470;} _LL1470: if(( unsigned int)
_temp1466 > 1u?*(( int*) _temp1466) == Cyc_Absyn_Short_c: 0){ _LL1491: _temp1490=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1466)->f1; goto _LL1489; _LL1489:
_temp1488=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp1466)->f2; goto
_LL1471;} else{ goto _LL1472;} _LL1472: if(( unsigned int) _temp1466 > 1u?*((
int*) _temp1466) == Cyc_Absyn_Int_c: 0){ _LL1495: _temp1494=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1466)->f1; if( _temp1494 ==( void*) Cyc_Absyn_Signed){
goto _LL1493;} else{ goto _LL1474;} _LL1493: _temp1492=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp1466)->f2; goto _LL1473;} else{ goto _LL1474;} _LL1474: if(( unsigned int)
_temp1466 > 1u?*(( int*) _temp1466) == Cyc_Absyn_Int_c: 0){ _LL1499: _temp1498=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1466)->f1; if( _temp1498 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL1497;} else{ goto _LL1476;} _LL1497: _temp1496=(
int)(( struct Cyc_Absyn_Int_c_struct*) _temp1466)->f2; goto _LL1475;} else{ goto
_LL1476;} _LL1476: if(( unsigned int) _temp1466 > 1u?*(( int*) _temp1466) == Cyc_Absyn_LongLong_c:
0){ _LL1503: _temp1502=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1466)->f1;
goto _LL1501; _LL1501: _temp1500=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1466)->f2; goto _LL1477;} else{ goto _LL1478;} _LL1478: if(( unsigned int)
_temp1466 > 1u?*(( int*) _temp1466) == Cyc_Absyn_Float_c: 0){ _LL1505: _temp1504=(
struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*) _temp1466)->f1; goto
_LL1479;} else{ goto _LL1480;} _LL1480: if( _temp1466 ==( void*) Cyc_Absyn_Null_c){
goto _LL1481;} else{ goto _LL1482;} _LL1482: if(( unsigned int) _temp1466 > 1u?*((
int*) _temp1466) == Cyc_Absyn_String_c: 0){ _LL1507: _temp1506=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp1466)->f1; goto
_LL1483;} else{ goto _LL1467;} _LL1469: return Cyc_PP_text(({ struct
_tagged_string _temp1508= Cyc_Absynpp_char_escape( _temp1484); xprintf("'%.*s'",
_temp1508.last_plus_one - _temp1508.curr, _temp1508.curr);})); _LL1471: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1488)); _LL1473: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1492)); _LL1475: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1496)); _LL1477: return Cyc_PP_text(( struct _tagged_string)({
char* _temp1509=( char*)"<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp1510; _temp1510.curr= _temp1509; _temp1510.base= _temp1509; _temp1510.last_plus_one=
_temp1509 + 27; _temp1510;})); _LL1479: return Cyc_PP_text( _temp1504); _LL1481:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp1511=( char*)"null";
struct _tagged_string _temp1512; _temp1512.curr= _temp1511; _temp1512.base=
_temp1511; _temp1512.last_plus_one= _temp1511 + 5; _temp1512;})); _LL1483:
return Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1513=(
char*)"\""; struct _tagged_string _temp1514; _temp1514.curr= _temp1513;
_temp1514.base= _temp1513; _temp1514.last_plus_one= _temp1513 + 2; _temp1514;})),
Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1506)), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1515=( char*)"\""; struct _tagged_string _temp1516;
_temp1516.curr= _temp1515; _temp1516.base= _temp1515; _temp1516.last_plus_one=
_temp1515 + 2; _temp1516;}))); _LL1467:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:({ struct Cyc_List_List*
_temp1517= es; if( _temp1517 == 0){ _throw( Null_Exception);} _temp1517->tl;})
!= 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp1518=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1518[ 0]=({ struct Cyc_Core_Failure_struct _temp1519; _temp1519.tag= Cyc_Core_Failure;
_temp1519.f1=({ struct _tagged_string _temp1520= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1520.last_plus_one
- _temp1520.curr, _temp1520.curr);}); _temp1519;}); _temp1518;}));}{ struct Cyc_PP_Doc*
d= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1523= es; if( _temp1523 == 0){ _throw( Null_Exception);} _temp1523->hd;}));
return Cyc_Absynpp_cat2( d, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1521=( char*)".size"; struct _tagged_string _temp1522; _temp1522.curr=
_temp1521; _temp1522.base= _temp1521; _temp1522.last_plus_one= _temp1521 + 6;
_temp1522;})));}} else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct Cyc_List_List*
ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1524=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1524[ 0]=({ struct Cyc_Core_Failure_struct _temp1525; _temp1525.tag= Cyc_Core_Failure;
_temp1525.f1=({ struct _tagged_string _temp1526= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with no args", _temp1526.last_plus_one -
_temp1526.curr, _temp1526.curr);}); _temp1525;}); _temp1524;}));} else{ if(({
struct Cyc_List_List* _temp1527= ds; if( _temp1527 == 0){ _throw( Null_Exception);}
_temp1527->tl;}) == 0){ return Cyc_Absynpp_cat3( ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1528=( char*)" "; struct _tagged_string _temp1529;
_temp1529.curr= _temp1528; _temp1529.base= _temp1528; _temp1529.last_plus_one=
_temp1528 + 2; _temp1529;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1530= ds; if( _temp1530 == 0){ _throw( Null_Exception);} _temp1530->hd;}));}
else{ if(({ struct Cyc_List_List* _temp1532=({ struct Cyc_List_List* _temp1531=
ds; if( _temp1531 == 0){ _throw( Null_Exception);} _temp1531->tl;}); if(
_temp1532 == 0){ _throw( Null_Exception);} _temp1532->tl;}) != 0){( void) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp1533=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1533[ 0]=({ struct Cyc_Core_Failure_struct
_temp1534; _temp1534.tag= Cyc_Core_Failure; _temp1534.f1=({ struct
_tagged_string _temp1535= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_temp1535.last_plus_one - _temp1535.curr, _temp1535.curr);}); _temp1534;});
_temp1533;}));} else{ return Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1536= ds; if( _temp1536 == 0){ _throw( Null_Exception);} _temp1536->hd;}),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1537=( char*)" "; struct
_tagged_string _temp1538; _temp1538.curr= _temp1537; _temp1538.base= _temp1537;
_temp1538.last_plus_one= _temp1537 + 2; _temp1538;})), ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1539=( char*)" "; struct _tagged_string _temp1540;
_temp1540.curr= _temp1539; _temp1540.base= _temp1539; _temp1540.last_plus_one=
_temp1539 + 2; _temp1540;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1542=({ struct Cyc_List_List* _temp1541= ds; if( _temp1541 == 0){ _throw(
Null_Exception);} _temp1541->tl;}); if( _temp1542 == 0){ _throw( Null_Exception);}
_temp1542->hd;}));}}}} else{ return Cyc_Absynpp_cat4( ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1543=( char*)"("; struct _tagged_string _temp1544;
_temp1544.curr= _temp1543; _temp1544.base= _temp1543; _temp1544.last_plus_one=
_temp1543 + 2; _temp1544;})), Cyc_Absynpp_exps2doc_prec( 20, es), Cyc_PP_text((
struct _tagged_string)({ char* _temp1545=( char*)")"; struct _tagged_string
_temp1546; _temp1546.curr= _temp1545; _temp1546.base= _temp1545; _temp1546.last_plus_one=
_temp1545 + 2; _temp1546;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1547= p; _LL1549: if( _temp1547 ==( void*) Cyc_Absyn_Plus){
goto _LL1550;} else{ goto _LL1551;} _LL1551: if( _temp1547 ==( void*) Cyc_Absyn_Times){
goto _LL1552;} else{ goto _LL1553;} _LL1553: if( _temp1547 ==( void*) Cyc_Absyn_Minus){
goto _LL1554;} else{ goto _LL1555;} _LL1555: if( _temp1547 ==( void*) Cyc_Absyn_Div){
goto _LL1556;} else{ goto _LL1557;} _LL1557: if( _temp1547 ==( void*) Cyc_Absyn_Mod){
goto _LL1558;} else{ goto _LL1559;} _LL1559: if( _temp1547 ==( void*) Cyc_Absyn_Eq){
goto _LL1560;} else{ goto _LL1561;} _LL1561: if( _temp1547 ==( void*) Cyc_Absyn_Neq){
goto _LL1562;} else{ goto _LL1563;} _LL1563: if( _temp1547 ==( void*) Cyc_Absyn_Gt){
goto _LL1564;} else{ goto _LL1565;} _LL1565: if( _temp1547 ==( void*) Cyc_Absyn_Lt){
goto _LL1566;} else{ goto _LL1567;} _LL1567: if( _temp1547 ==( void*) Cyc_Absyn_Gte){
goto _LL1568;} else{ goto _LL1569;} _LL1569: if( _temp1547 ==( void*) Cyc_Absyn_Lte){
goto _LL1570;} else{ goto _LL1571;} _LL1571: if( _temp1547 ==( void*) Cyc_Absyn_Not){
goto _LL1572;} else{ goto _LL1573;} _LL1573: if( _temp1547 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1574;} else{ goto _LL1575;} _LL1575: if( _temp1547 ==( void*) Cyc_Absyn_Bitand){
goto _LL1576;} else{ goto _LL1577;} _LL1577: if( _temp1547 ==( void*) Cyc_Absyn_Bitor){
goto _LL1578;} else{ goto _LL1579;} _LL1579: if( _temp1547 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1580;} else{ goto _LL1581;} _LL1581: if( _temp1547 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1582;} else{ goto _LL1583;} _LL1583: if( _temp1547 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1584;} else{ goto _LL1585;} _LL1585: if( _temp1547 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1586;} else{ goto _LL1587;} _LL1587: if( _temp1547 ==( void*) Cyc_Absyn_Size){
goto _LL1588;} else{ goto _LL1589;} _LL1589: if( _temp1547 ==( void*) Cyc_Absyn_Printf){
goto _LL1590;} else{ goto _LL1591;} _LL1591: if( _temp1547 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1592;} else{ goto _LL1593;} _LL1593: if( _temp1547 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1594;} else{ goto _LL1595;} _LL1595: if( _temp1547 ==( void*) Cyc_Absyn_Scanf){
goto _LL1596;} else{ goto _LL1597;} _LL1597: if( _temp1547 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1598;} else{ goto _LL1599;} _LL1599: if( _temp1547 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1600;} else{ goto _LL1548;} _LL1550: return( struct _tagged_string)({
char* _temp1601=( char*)"+"; struct _tagged_string _temp1602; _temp1602.curr=
_temp1601; _temp1602.base= _temp1601; _temp1602.last_plus_one= _temp1601 + 2;
_temp1602;}); _LL1552: return( struct _tagged_string)({ char* _temp1603=( char*)"*";
struct _tagged_string _temp1604; _temp1604.curr= _temp1603; _temp1604.base=
_temp1603; _temp1604.last_plus_one= _temp1603 + 2; _temp1604;}); _LL1554: return(
struct _tagged_string)({ char* _temp1605=( char*)"-"; struct _tagged_string
_temp1606; _temp1606.curr= _temp1605; _temp1606.base= _temp1605; _temp1606.last_plus_one=
_temp1605 + 2; _temp1606;}); _LL1556: return( struct _tagged_string)({ char*
_temp1607=( char*)"/"; struct _tagged_string _temp1608; _temp1608.curr=
_temp1607; _temp1608.base= _temp1607; _temp1608.last_plus_one= _temp1607 + 2;
_temp1608;}); _LL1558: return( struct _tagged_string)({ char* _temp1609=( char*)"%";
struct _tagged_string _temp1610; _temp1610.curr= _temp1609; _temp1610.base=
_temp1609; _temp1610.last_plus_one= _temp1609 + 2; _temp1610;}); _LL1560: return(
struct _tagged_string)({ char* _temp1611=( char*)"=="; struct _tagged_string
_temp1612; _temp1612.curr= _temp1611; _temp1612.base= _temp1611; _temp1612.last_plus_one=
_temp1611 + 3; _temp1612;}); _LL1562: return( struct _tagged_string)({ char*
_temp1613=( char*)"!="; struct _tagged_string _temp1614; _temp1614.curr=
_temp1613; _temp1614.base= _temp1613; _temp1614.last_plus_one= _temp1613 + 3;
_temp1614;}); _LL1564: return( struct _tagged_string)({ char* _temp1615=( char*)">";
struct _tagged_string _temp1616; _temp1616.curr= _temp1615; _temp1616.base=
_temp1615; _temp1616.last_plus_one= _temp1615 + 2; _temp1616;}); _LL1566: return(
struct _tagged_string)({ char* _temp1617=( char*)"<"; struct _tagged_string
_temp1618; _temp1618.curr= _temp1617; _temp1618.base= _temp1617; _temp1618.last_plus_one=
_temp1617 + 2; _temp1618;}); _LL1568: return( struct _tagged_string)({ char*
_temp1619=( char*)">="; struct _tagged_string _temp1620; _temp1620.curr=
_temp1619; _temp1620.base= _temp1619; _temp1620.last_plus_one= _temp1619 + 3;
_temp1620;}); _LL1570: return( struct _tagged_string)({ char* _temp1621=( char*)"<=";
struct _tagged_string _temp1622; _temp1622.curr= _temp1621; _temp1622.base=
_temp1621; _temp1622.last_plus_one= _temp1621 + 3; _temp1622;}); _LL1572: return(
struct _tagged_string)({ char* _temp1623=( char*)"!"; struct _tagged_string
_temp1624; _temp1624.curr= _temp1623; _temp1624.base= _temp1623; _temp1624.last_plus_one=
_temp1623 + 2; _temp1624;}); _LL1574: return( struct _tagged_string)({ char*
_temp1625=( char*)"~"; struct _tagged_string _temp1626; _temp1626.curr=
_temp1625; _temp1626.base= _temp1625; _temp1626.last_plus_one= _temp1625 + 2;
_temp1626;}); _LL1576: return( struct _tagged_string)({ char* _temp1627=( char*)"&";
struct _tagged_string _temp1628; _temp1628.curr= _temp1627; _temp1628.base=
_temp1627; _temp1628.last_plus_one= _temp1627 + 2; _temp1628;}); _LL1578: return(
struct _tagged_string)({ char* _temp1629=( char*)"|"; struct _tagged_string
_temp1630; _temp1630.curr= _temp1629; _temp1630.base= _temp1629; _temp1630.last_plus_one=
_temp1629 + 2; _temp1630;}); _LL1580: return( struct _tagged_string)({ char*
_temp1631=( char*)"^"; struct _tagged_string _temp1632; _temp1632.curr=
_temp1631; _temp1632.base= _temp1631; _temp1632.last_plus_one= _temp1631 + 2;
_temp1632;}); _LL1582: return( struct _tagged_string)({ char* _temp1633=( char*)"<<";
struct _tagged_string _temp1634; _temp1634.curr= _temp1633; _temp1634.base=
_temp1633; _temp1634.last_plus_one= _temp1633 + 3; _temp1634;}); _LL1584: return(
struct _tagged_string)({ char* _temp1635=( char*)">>"; struct _tagged_string
_temp1636; _temp1636.curr= _temp1635; _temp1636.base= _temp1635; _temp1636.last_plus_one=
_temp1635 + 3; _temp1636;}); _LL1586: return( struct _tagged_string)({ char*
_temp1637=( char*)">>>"; struct _tagged_string _temp1638; _temp1638.curr=
_temp1637; _temp1638.base= _temp1637; _temp1638.last_plus_one= _temp1637 + 4;
_temp1638;}); _LL1588: return( struct _tagged_string)({ char* _temp1639=( char*)"size";
struct _tagged_string _temp1640; _temp1640.curr= _temp1639; _temp1640.base=
_temp1639; _temp1640.last_plus_one= _temp1639 + 5; _temp1640;}); _LL1590: return(
struct _tagged_string)({ char* _temp1641=( char*)"printf"; struct _tagged_string
_temp1642; _temp1642.curr= _temp1641; _temp1642.base= _temp1641; _temp1642.last_plus_one=
_temp1641 + 7; _temp1642;}); _LL1592: return( struct _tagged_string)({ char*
_temp1643=( char*)"fprintf"; struct _tagged_string _temp1644; _temp1644.curr=
_temp1643; _temp1644.base= _temp1643; _temp1644.last_plus_one= _temp1643 + 8;
_temp1644;}); _LL1594: return( struct _tagged_string)({ char* _temp1645=( char*)"xprintf";
struct _tagged_string _temp1646; _temp1646.curr= _temp1645; _temp1646.base=
_temp1645; _temp1646.last_plus_one= _temp1645 + 8; _temp1646;}); _LL1596: return(
struct _tagged_string)({ char* _temp1647=( char*)"scanf"; struct _tagged_string
_temp1648; _temp1648.curr= _temp1647; _temp1648.base= _temp1647; _temp1648.last_plus_one=
_temp1647 + 6; _temp1648;}); _LL1598: return( struct _tagged_string)({ char*
_temp1649=( char*)"fscanf"; struct _tagged_string _temp1650; _temp1650.curr=
_temp1649; _temp1650.base= _temp1649; _temp1650.last_plus_one= _temp1649 + 7;
_temp1650;}); _LL1600: return( struct _tagged_string)({ char* _temp1651=( char*)"sscanf";
struct _tagged_string _temp1652; _temp1652.curr= _temp1651; _temp1652.base=
_temp1651; _temp1652.last_plus_one= _temp1651 + 7; _temp1652;}); _LL1548:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 ==
0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp1653=(* t).f1; if( _temp1653
== 0){ _throw( Null_Exception);} _temp1653->v;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1654=( char*)" = "; struct _tagged_string _temp1655;
_temp1655.curr= _temp1654; _temp1655.base= _temp1654; _temp1655.last_plus_one=
_temp1654 + 4; _temp1655;})), Cyc_Absynpp_exp2doc((* t).f2));}} struct Cyc_PP_Doc*
Cyc_Absynpp_cat2( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2){ return Cyc_PP_concat(
d1, d2);} struct Cyc_PP_Doc* Cyc_Absynpp_cat3( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2, struct Cyc_PP_Doc* d3){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, d3));}
struct Cyc_PP_Doc* Cyc_Absynpp_cat4( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4){ return Cyc_PP_concat( d1, Cyc_PP_concat(
d2, Cyc_PP_concat( d3, d4)));} struct Cyc_PP_Doc* Cyc_Absynpp_cat5( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat( d3,
Cyc_PP_concat( d4, d5))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat6( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5, struct Cyc_PP_Doc* d6){ return Cyc_PP_concat( d1, Cyc_PP_concat(
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
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1656=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1662; struct Cyc_Absyn_Decl* _temp1664;
_LL1658: if(( unsigned int) _temp1656 > 1u?*(( int*) _temp1656) == Cyc_Absyn_Decl_s:
0){ _LL1665: _temp1664=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1656)->f1; goto _LL1663; _LL1663: _temp1662=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1656)->f2; goto _LL1659;} else{ goto
_LL1660;} _LL1660: goto _LL1661; _LL1659: return 1; _LL1661: return 0; _LL1657:;}
struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc*
s;{ void* _temp1666=( void*) st->r; struct Cyc_Absyn_Exp* _temp1708; struct Cyc_Absyn_Stmt*
_temp1710; struct Cyc_Absyn_Stmt* _temp1712; struct Cyc_Absyn_Exp* _temp1714;
struct Cyc_Absyn_Stmt* _temp1716; struct Cyc_Absyn_Stmt* _temp1718; struct Cyc_Absyn_Exp*
_temp1720; struct Cyc_Absyn_Stmt* _temp1722; struct _tuple2 _temp1724; struct
Cyc_Absyn_Stmt* _temp1726; struct Cyc_Absyn_Exp* _temp1728; struct Cyc_Absyn_Stmt*
_temp1730; struct Cyc_Absyn_Stmt* _temp1732; struct Cyc_Absyn_Stmt* _temp1734;
struct _tagged_string* _temp1736; struct Cyc_Absyn_Stmt* _temp1738; struct
_tuple2 _temp1740; struct Cyc_Absyn_Stmt* _temp1742; struct Cyc_Absyn_Exp*
_temp1744; struct _tuple2 _temp1746; struct Cyc_Absyn_Stmt* _temp1748; struct
Cyc_Absyn_Exp* _temp1750; struct Cyc_Absyn_Exp* _temp1752; struct Cyc_List_List*
_temp1754; struct Cyc_Absyn_Exp* _temp1756; struct Cyc_Absyn_Switch_clause**
_temp1758; struct Cyc_List_List* _temp1760; struct Cyc_Absyn_Switch_clause**
_temp1762; struct Cyc_List_List* _temp1764; struct Cyc_Absyn_Stmt* _temp1766;
struct Cyc_Absyn_Decl* _temp1768; struct Cyc_Absyn_Stmt* _temp1770; struct Cyc_Absyn_Stmt*
_temp1772; struct Cyc_Absyn_Stmt* _temp1774; struct _tagged_string* _temp1776;
struct _tuple2 _temp1778; struct Cyc_Absyn_Stmt* _temp1780; struct Cyc_Absyn_Exp*
_temp1782; struct Cyc_Absyn_Stmt* _temp1784; struct Cyc_List_List* _temp1786;
struct Cyc_Absyn_Stmt* _temp1788; struct Cyc_Absyn_Stmt* _temp1790; struct Cyc_Absyn_Vardecl*
_temp1792; struct Cyc_Absyn_Tvar* _temp1794; _LL1668: if( _temp1666 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1669;} else{ goto _LL1670;} _LL1670: if((
unsigned int) _temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_Exp_s: 0){
_LL1709: _temp1708=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1666)->f1; goto _LL1671;} else{ goto _LL1672;} _LL1672: if(( unsigned int)
_temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_Seq_s: 0){ _LL1713: _temp1712=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1666)->f1; goto
_LL1711; _LL1711: _temp1710=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1666)->f2; goto _LL1673;} else{ goto _LL1674;} _LL1674: if(( unsigned int)
_temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_Return_s: 0){ _LL1715:
_temp1714=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1666)->f1; goto _LL1675;} else{ goto _LL1676;} _LL1676: if(( unsigned int)
_temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_IfThenElse_s: 0){ _LL1721:
_temp1720=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1666)->f1; goto _LL1719; _LL1719: _temp1718=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1666)->f2; goto _LL1717; _LL1717:
_temp1716=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1666)->f3; goto _LL1677;} else{ goto _LL1678;} _LL1678: if(( unsigned int)
_temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_While_s: 0){ _LL1725: _temp1724=(
struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp1666)->f1; _LL1729:
_temp1728= _temp1724.f1; goto _LL1727; _LL1727: _temp1726= _temp1724.f2; goto
_LL1723; _LL1723: _temp1722=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1666)->f2; goto _LL1679;} else{ goto _LL1680;} _LL1680: if(( unsigned int)
_temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_Break_s: 0){ _LL1731: _temp1730=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp1666)->f1; goto
_LL1681;} else{ goto _LL1682;} _LL1682: if(( unsigned int) _temp1666 > 1u?*((
int*) _temp1666) == Cyc_Absyn_Continue_s: 0){ _LL1733: _temp1732=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1666)->f1; goto _LL1683;} else{ goto
_LL1684;} _LL1684: if(( unsigned int) _temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_Goto_s:
0){ _LL1737: _temp1736=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1666)->f1; goto _LL1735; _LL1735: _temp1734=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp1666)->f2; goto _LL1685;} else{ goto
_LL1686;} _LL1686: if(( unsigned int) _temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_For_s:
0){ _LL1753: _temp1752=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1666)->f1; goto _LL1747; _LL1747: _temp1746=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp1666)->f2; _LL1751: _temp1750= _temp1746.f1; goto _LL1749; _LL1749:
_temp1748= _temp1746.f2; goto _LL1741; _LL1741: _temp1740=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp1666)->f3; _LL1745: _temp1744= _temp1740.f1;
goto _LL1743; _LL1743: _temp1742= _temp1740.f2; goto _LL1739; _LL1739: _temp1738=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1666)->f4; goto
_LL1687;} else{ goto _LL1688;} _LL1688: if(( unsigned int) _temp1666 > 1u?*((
int*) _temp1666) == Cyc_Absyn_Switch_s: 0){ _LL1757: _temp1756=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1666)->f1; goto _LL1755; _LL1755:
_temp1754=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1666)->f2; goto _LL1689;} else{ goto _LL1690;} _LL1690: if(( unsigned int)
_temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_Fallthru_s: 0){ _LL1761:
_temp1760=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1666)->f1; if( _temp1760 == 0){ goto _LL1759;} else{ goto _LL1692;} _LL1759:
_temp1758=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1666)->f2; goto _LL1691;} else{ goto _LL1692;} _LL1692: if(( unsigned int)
_temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_Fallthru_s: 0){ _LL1765:
_temp1764=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1666)->f1; goto _LL1763; _LL1763: _temp1762=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1666)->f2; goto _LL1693;} else{ goto
_LL1694;} _LL1694: if(( unsigned int) _temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_Decl_s:
0){ _LL1769: _temp1768=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1666)->f1; goto _LL1767; _LL1767: _temp1766=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1666)->f2; goto _LL1695;} else{ goto
_LL1696;} _LL1696: if(( unsigned int) _temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_Cut_s:
0){ _LL1771: _temp1770=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp1666)->f1; goto _LL1697;} else{ goto _LL1698;} _LL1698: if(( unsigned int)
_temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_Splice_s: 0){ _LL1773:
_temp1772=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp1666)->f1; goto _LL1699;} else{ goto _LL1700;} _LL1700: if(( unsigned int)
_temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_Label_s: 0){ _LL1777: _temp1776=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1666)->f1; goto
_LL1775; _LL1775: _temp1774=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1666)->f2; goto _LL1701;} else{ goto _LL1702;} _LL1702: if(( unsigned int)
_temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_Do_s: 0){ _LL1785: _temp1784=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp1666)->f1; goto
_LL1779; _LL1779: _temp1778=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp1666)->f2; _LL1783: _temp1782= _temp1778.f1; goto _LL1781; _LL1781:
_temp1780= _temp1778.f2; goto _LL1703;} else{ goto _LL1704;} _LL1704: if((
unsigned int) _temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_TryCatch_s: 0){
_LL1789: _temp1788=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1666)->f1; goto _LL1787; _LL1787: _temp1786=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1666)->f2; goto _LL1705;} else{ goto
_LL1706;} _LL1706: if(( unsigned int) _temp1666 > 1u?*(( int*) _temp1666) == Cyc_Absyn_Region_s:
0){ _LL1795: _temp1794=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1666)->f1; goto _LL1793; _LL1793: _temp1792=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1666)->f2; goto _LL1791; _LL1791:
_temp1790=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1666)->f3; goto _LL1707;} else{ goto _LL1667;} _LL1669: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp1796=( char*)";"; struct _tagged_string
_temp1797; _temp1797.curr= _temp1796; _temp1797.base= _temp1796; _temp1797.last_plus_one=
_temp1796 + 2; _temp1797;})); goto _LL1667; _LL1671: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc(
_temp1708), Cyc_PP_text(( struct _tagged_string)({ char* _temp1798=( char*)";";
struct _tagged_string _temp1799; _temp1799.curr= _temp1798; _temp1799.base=
_temp1798; _temp1799.last_plus_one= _temp1798 + 2; _temp1799;}))); goto _LL1667;
_LL1673: if( Cyc_Absynpp_decls_first){ if( Cyc_Absynpp_is_declaration( _temp1712)){
s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({ char* _temp1800=(
char*)"{ "; struct _tagged_string _temp1801; _temp1801.curr= _temp1800;
_temp1801.base= _temp1800; _temp1801.last_plus_one= _temp1800 + 3; _temp1801;})),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1712)), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1802=( char*)"}"; struct _tagged_string
_temp1803; _temp1803.curr= _temp1802; _temp1803.base= _temp1802; _temp1803.last_plus_one=
_temp1802 + 2; _temp1803;})), Cyc_PP_line_doc(), Cyc_Absynpp_is_declaration(
_temp1710)? Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1804=( char*)"{ "; struct _tagged_string _temp1805; _temp1805.curr=
_temp1804; _temp1805.base= _temp1804; _temp1805.last_plus_one= _temp1804 + 3;
_temp1805;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1710)), Cyc_PP_text((
struct _tagged_string)({ char* _temp1806=( char*)"}"; struct _tagged_string
_temp1807; _temp1807.curr= _temp1806; _temp1807.base= _temp1806; _temp1807.last_plus_one=
_temp1806 + 2; _temp1807;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1710));} else{ if( Cyc_Absynpp_is_declaration( _temp1710)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1712), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1808=( char*)"{ "; struct _tagged_string _temp1809;
_temp1809.curr= _temp1808; _temp1809.base= _temp1808; _temp1809.last_plus_one=
_temp1808 + 3; _temp1809;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1710)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1810=( char*)"}"; struct
_tagged_string _temp1811; _temp1811.curr= _temp1810; _temp1811.base= _temp1810;
_temp1811.last_plus_one= _temp1810 + 2; _temp1811;})), Cyc_PP_line_doc());}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,(
struct _tagged_string)({ char* _temp1812=( char*)""; struct _tagged_string
_temp1813; _temp1813.curr= _temp1812; _temp1813.base= _temp1812; _temp1813.last_plus_one=
_temp1812 + 1; _temp1813;}),({ struct Cyc_List_List* _temp1814=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1814->hd=( void*) _temp1712;
_temp1814->tl=({ struct Cyc_List_List* _temp1815=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1815->hd=( void*) _temp1710;
_temp1815->tl= 0; _temp1815;}); _temp1814;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,( struct
_tagged_string)({ char* _temp1816=( char*)""; struct _tagged_string _temp1817;
_temp1817.curr= _temp1816; _temp1817.base= _temp1816; _temp1817.last_plus_one=
_temp1816 + 1; _temp1817;}),({ struct Cyc_List_List* _temp1818=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1818->hd=( void*) _temp1712;
_temp1818->tl=({ struct Cyc_List_List* _temp1819=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1819->hd=( void*) _temp1710;
_temp1819->tl= 0; _temp1819;}); _temp1818;}));} goto _LL1667; _LL1675: if(
_temp1714 == 0){ s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1820=(
char*)"return;"; struct _tagged_string _temp1821; _temp1821.curr= _temp1820;
_temp1821.base= _temp1820; _temp1821.last_plus_one= _temp1820 + 8; _temp1821;}));}
else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1822=( char*)"return "; struct _tagged_string _temp1823; _temp1823.curr=
_temp1822; _temp1823.base= _temp1822; _temp1823.last_plus_one= _temp1822 + 8;
_temp1823;})), _temp1714 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1824= _temp1714; if( _temp1824 == 0){
_throw( Null_Exception);} _temp1824;})), Cyc_PP_text(( struct _tagged_string)({
char* _temp1825=( char*)";"; struct _tagged_string _temp1826; _temp1826.curr=
_temp1825; _temp1826.base= _temp1825; _temp1826.last_plus_one= _temp1825 + 2;
_temp1826;})));} goto _LL1667; _LL1677: { int print_else;{ void* _temp1827=(
void*) _temp1716->r; _LL1829: if( _temp1827 ==( void*) Cyc_Absyn_Skip_s){ goto
_LL1830;} else{ goto _LL1831;} _LL1831: goto _LL1832; _LL1830: print_else= 0;
goto _LL1828; _LL1832: print_else= 1; goto _LL1828; _LL1828:;} s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1833=( char*)"if ("; struct
_tagged_string _temp1834; _temp1834.curr= _temp1833; _temp1834.base= _temp1833;
_temp1834.last_plus_one= _temp1833 + 5; _temp1834;})), Cyc_Absynpp_exp2doc(
_temp1720), Cyc_PP_text(( struct _tagged_string)({ char* _temp1835=( char*)") {";
struct _tagged_string _temp1836; _temp1836.curr= _temp1835; _temp1836.base=
_temp1835; _temp1836.last_plus_one= _temp1835 + 4; _temp1836;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1718))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1837=( char*)"}"; struct
_tagged_string _temp1838; _temp1838.curr= _temp1837; _temp1838.base= _temp1837;
_temp1838.last_plus_one= _temp1837 + 2; _temp1838;})), print_else? Cyc_Absynpp_cat5(
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1839=( char*)"else {";
struct _tagged_string _temp1840; _temp1840.curr= _temp1839; _temp1840.base=
_temp1839; _temp1840.last_plus_one= _temp1839 + 7; _temp1840;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1716))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1841=( char*)"}"; struct
_tagged_string _temp1842; _temp1842.curr= _temp1841; _temp1842.base= _temp1841;
_temp1842.last_plus_one= _temp1841 + 2; _temp1842;}))): Cyc_PP_nil_doc()); goto
_LL1667;} _LL1679: s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({
char* _temp1843=( char*)"while ("; struct _tagged_string _temp1844; _temp1844.curr=
_temp1843; _temp1844.base= _temp1843; _temp1844.last_plus_one= _temp1843 + 8;
_temp1844;})), Cyc_Absynpp_exp2doc( _temp1728), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1845=( char*)") {"; struct _tagged_string _temp1846;
_temp1846.curr= _temp1845; _temp1846.base= _temp1845; _temp1846.last_plus_one=
_temp1845 + 4; _temp1846;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1722))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1847=( char*)"}"; struct _tagged_string _temp1848;
_temp1848.curr= _temp1847; _temp1848.base= _temp1847; _temp1848.last_plus_one=
_temp1847 + 2; _temp1848;}))); goto _LL1667; _LL1681: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1849=( char*)"break;"; struct _tagged_string
_temp1850; _temp1850.curr= _temp1849; _temp1850.base= _temp1849; _temp1850.last_plus_one=
_temp1849 + 7; _temp1850;})); goto _LL1667; _LL1683: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1851=( char*)"continue;"; struct _tagged_string
_temp1852; _temp1852.curr= _temp1851; _temp1852.base= _temp1851; _temp1852.last_plus_one=
_temp1851 + 10; _temp1852;})); goto _LL1667; _LL1685: s= Cyc_PP_text(({ struct
_tagged_string _temp1853=* _temp1736; xprintf("goto %.*s;", _temp1853.last_plus_one
- _temp1853.curr, _temp1853.curr);})); goto _LL1667; _LL1687: s= Cyc_Absynpp_cat10(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1854=( char*)"for("; struct
_tagged_string _temp1855; _temp1855.curr= _temp1854; _temp1855.base= _temp1854;
_temp1855.last_plus_one= _temp1854 + 5; _temp1855;})), Cyc_Absynpp_exp2doc(
_temp1752), Cyc_PP_text(( struct _tagged_string)({ char* _temp1856=( char*)"; ";
struct _tagged_string _temp1857; _temp1857.curr= _temp1856; _temp1857.base=
_temp1856; _temp1857.last_plus_one= _temp1856 + 3; _temp1857;})), Cyc_Absynpp_exp2doc(
_temp1750), Cyc_PP_text(( struct _tagged_string)({ char* _temp1858=( char*)"; ";
struct _tagged_string _temp1859; _temp1859.curr= _temp1858; _temp1859.base=
_temp1858; _temp1859.last_plus_one= _temp1858 + 3; _temp1859;})), Cyc_Absynpp_exp2doc(
_temp1744), Cyc_PP_text(( struct _tagged_string)({ char* _temp1860=( char*)") {";
struct _tagged_string _temp1861; _temp1861.curr= _temp1860; _temp1861.base=
_temp1860; _temp1861.last_plus_one= _temp1860 + 4; _temp1861;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1738))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1862=( char*)"}"; struct
_tagged_string _temp1863; _temp1863.curr= _temp1862; _temp1863.base= _temp1862;
_temp1863.last_plus_one= _temp1862 + 2; _temp1863;}))); goto _LL1667; _LL1689: s=
Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char* _temp1864=( char*)"switch (";
struct _tagged_string _temp1865; _temp1865.curr= _temp1864; _temp1865.base=
_temp1864; _temp1865.last_plus_one= _temp1864 + 9; _temp1865;})), Cyc_Absynpp_exp2doc(
_temp1756), Cyc_PP_text(( struct _tagged_string)({ char* _temp1866=( char*)") {";
struct _tagged_string _temp1867; _temp1867.curr= _temp1866; _temp1867.base=
_temp1866; _temp1867.last_plus_one= _temp1866 + 4; _temp1867;})), Cyc_PP_line_doc(),
Cyc_Absynpp_switchclauses2doc( _temp1754), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1868=( char*)"}"; struct _tagged_string
_temp1869; _temp1869.curr= _temp1868; _temp1869.base= _temp1868; _temp1869.last_plus_one=
_temp1868 + 2; _temp1869;}))); goto _LL1667; _LL1691: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1870=( char*)"fallthru;"; struct _tagged_string
_temp1871; _temp1871.curr= _temp1870; _temp1871.base= _temp1870; _temp1871.last_plus_one=
_temp1870 + 10; _temp1871;})); goto _LL1667; _LL1693: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1872=( char*)"fallthru("; struct
_tagged_string _temp1873; _temp1873.curr= _temp1872; _temp1873.base= _temp1872;
_temp1873.last_plus_one= _temp1872 + 10; _temp1873;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1764), Cyc_PP_text(( struct _tagged_string)({ char* _temp1874=( char*)");";
struct _tagged_string _temp1875; _temp1875.curr= _temp1874; _temp1875.base=
_temp1874; _temp1875.last_plus_one= _temp1874 + 3; _temp1875;}))); goto _LL1667;
_LL1695: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1768), Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1766)); goto _LL1667; _LL1697: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1876=( char*)"cut "; struct
_tagged_string _temp1877; _temp1877.curr= _temp1876; _temp1877.base= _temp1876;
_temp1877.last_plus_one= _temp1876 + 5; _temp1877;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1770))); goto _LL1667; _LL1699: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1878=( char*)"splice "; struct _tagged_string
_temp1879; _temp1879.curr= _temp1878; _temp1879.base= _temp1878; _temp1879.last_plus_one=
_temp1878 + 8; _temp1879;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1772)));
goto _LL1667; _LL1701: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1774): 0){ s= Cyc_Absynpp_cat6( Cyc_Absynpp_textptr( _temp1776), Cyc_PP_text((
struct _tagged_string)({ char* _temp1880=( char*)": {"; struct _tagged_string
_temp1881; _temp1881.curr= _temp1880; _temp1881.base= _temp1880; _temp1881.last_plus_one=
_temp1880 + 4; _temp1881;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1774)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1882=( char*)"}"; struct _tagged_string _temp1883; _temp1883.curr=
_temp1882; _temp1883.base= _temp1882; _temp1883.last_plus_one= _temp1882 + 2;
_temp1883;})));} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( _temp1776), Cyc_PP_text((
struct _tagged_string)({ char* _temp1884=( char*)": "; struct _tagged_string
_temp1885; _temp1885.curr= _temp1884; _temp1885.base= _temp1884; _temp1885.last_plus_one=
_temp1884 + 3; _temp1885;})), Cyc_Absynpp_stmt2doc( _temp1774));} goto _LL1667;
_LL1703: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1886=( char*)"do {"; struct _tagged_string _temp1887; _temp1887.curr=
_temp1886; _temp1887.base= _temp1886; _temp1887.last_plus_one= _temp1886 + 5;
_temp1887;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1784)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1888=( char*)"} while ("; struct _tagged_string _temp1889; _temp1889.curr=
_temp1888; _temp1889.base= _temp1888; _temp1889.last_plus_one= _temp1888 + 10;
_temp1889;})), Cyc_Absynpp_exp2doc( _temp1782), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1890=( char*)");"; struct _tagged_string _temp1891;
_temp1891.curr= _temp1890; _temp1891.base= _temp1890; _temp1891.last_plus_one=
_temp1890 + 3; _temp1891;}))); goto _LL1667; _LL1705: s= Cyc_Absynpp_cat9( Cyc_PP_text((
struct _tagged_string)({ char* _temp1892=( char*)"try {"; struct _tagged_string
_temp1893; _temp1893.curr= _temp1892; _temp1893.base= _temp1892; _temp1893.last_plus_one=
_temp1892 + 6; _temp1893;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1788)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1894=( char*)"} catch {"; struct _tagged_string _temp1895; _temp1895.curr=
_temp1894; _temp1895.base= _temp1894; _temp1895.last_plus_one= _temp1894 + 10;
_temp1895;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1786)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1896=( char*)"}"; struct _tagged_string _temp1897; _temp1897.curr=
_temp1896; _temp1897.base= _temp1896; _temp1897.last_plus_one= _temp1896 + 2;
_temp1897;}))); goto _LL1667; _LL1707: s= Cyc_Absynpp_cat9( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1898=( char*)"region<"; struct _tagged_string
_temp1899; _temp1899.curr= _temp1898; _temp1899.base= _temp1898; _temp1899.last_plus_one=
_temp1898 + 8; _temp1899;})), Cyc_Absynpp_textptr( Cyc_Absynpp_get_name(
_temp1794)), Cyc_PP_text(( struct _tagged_string)({ char* _temp1900=( char*)">";
struct _tagged_string _temp1901; _temp1901.curr= _temp1900; _temp1901.base=
_temp1900; _temp1901.last_plus_one= _temp1900 + 2; _temp1901;})), Cyc_Absynpp_qvar2doc(
_temp1792->name), Cyc_PP_text(( struct _tagged_string)({ char* _temp1902=( char*)"{";
struct _tagged_string _temp1903; _temp1903.curr= _temp1902; _temp1903.base=
_temp1902; _temp1903.last_plus_one= _temp1902 + 2; _temp1903;})), Cyc_PP_line_doc(),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1790)), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1904=( char*)"}"; struct _tagged_string
_temp1905; _temp1905.curr= _temp1904; _temp1905.base= _temp1904; _temp1905.last_plus_one=
_temp1904 + 2; _temp1905;}))); goto _LL1667; _LL1667:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_pat2doc( struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void*
_temp1906=( void*) p->r; int _temp1938; void* _temp1940; char _temp1942; struct
_tagged_string _temp1944; struct Cyc_Absyn_Vardecl* _temp1946; struct Cyc_List_List*
_temp1948; struct Cyc_Absyn_Pat* _temp1950; struct Cyc_Absyn_Vardecl* _temp1952;
struct _tuple0* _temp1954; struct Cyc_List_List* _temp1956; struct Cyc_List_List*
_temp1958; struct _tuple0* _temp1960; struct Cyc_List_List* _temp1962; struct
Cyc_List_List* _temp1964; struct _tuple0* _temp1966; struct Cyc_List_List*
_temp1968; struct Cyc_List_List* _temp1970; struct Cyc_Core_Opt* _temp1972;
struct Cyc_Absyn_Structdecl* _temp1974; struct Cyc_Absyn_Enumfield* _temp1976;
struct Cyc_Absyn_Enumdecl* _temp1978; struct Cyc_List_List* _temp1980; struct
Cyc_List_List* _temp1982; struct Cyc_Absyn_Tunionfield* _temp1984; struct Cyc_Absyn_Tuniondecl*
_temp1986; _LL1908: if( _temp1906 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1909;}
else{ goto _LL1910;} _LL1910: if( _temp1906 ==( void*) Cyc_Absyn_Null_p){ goto
_LL1911;} else{ goto _LL1912;} _LL1912: if(( unsigned int) _temp1906 > 2u?*((
int*) _temp1906) == Cyc_Absyn_Int_p: 0){ _LL1941: _temp1940=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1906)->f1; goto _LL1939; _LL1939: _temp1938=( int)((
struct Cyc_Absyn_Int_p_struct*) _temp1906)->f2; goto _LL1913;} else{ goto
_LL1914;} _LL1914: if(( unsigned int) _temp1906 > 2u?*(( int*) _temp1906) == Cyc_Absyn_Char_p:
0){ _LL1943: _temp1942=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1906)->f1;
goto _LL1915;} else{ goto _LL1916;} _LL1916: if(( unsigned int) _temp1906 > 2u?*((
int*) _temp1906) == Cyc_Absyn_Float_p: 0){ _LL1945: _temp1944=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1906)->f1; goto _LL1917;}
else{ goto _LL1918;} _LL1918: if(( unsigned int) _temp1906 > 2u?*(( int*)
_temp1906) == Cyc_Absyn_Var_p: 0){ _LL1947: _temp1946=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1906)->f1; goto _LL1919;} else{ goto
_LL1920;} _LL1920: if(( unsigned int) _temp1906 > 2u?*(( int*) _temp1906) == Cyc_Absyn_Tuple_p:
0){ _LL1949: _temp1948=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1906)->f1; goto _LL1921;} else{ goto _LL1922;} _LL1922: if(( unsigned int)
_temp1906 > 2u?*(( int*) _temp1906) == Cyc_Absyn_Pointer_p: 0){ _LL1951:
_temp1950=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1906)->f1; goto _LL1923;} else{ goto _LL1924;} _LL1924: if(( unsigned int)
_temp1906 > 2u?*(( int*) _temp1906) == Cyc_Absyn_Reference_p: 0){ _LL1953:
_temp1952=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1906)->f1; goto _LL1925;} else{ goto _LL1926;} _LL1926: if(( unsigned int)
_temp1906 > 2u?*(( int*) _temp1906) == Cyc_Absyn_UnknownId_p: 0){ _LL1955:
_temp1954=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1906)->f1;
goto _LL1927;} else{ goto _LL1928;} _LL1928: if(( unsigned int) _temp1906 > 2u?*((
int*) _temp1906) == Cyc_Absyn_UnknownCall_p: 0){ _LL1961: _temp1960=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1906)->f1; goto _LL1959;
_LL1959: _temp1958=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1906)->f2; goto _LL1957; _LL1957: _temp1956=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1906)->f3; goto _LL1929;} else{
goto _LL1930;} _LL1930: if(( unsigned int) _temp1906 > 2u?*(( int*) _temp1906)
== Cyc_Absyn_UnknownFields_p: 0){ _LL1967: _temp1966=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp1906)->f1; goto _LL1965; _LL1965:
_temp1964=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1906)->f2; goto _LL1963; _LL1963: _temp1962=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1906)->f3; goto _LL1931;} else{
goto _LL1932;} _LL1932: if(( unsigned int) _temp1906 > 2u?*(( int*) _temp1906)
== Cyc_Absyn_Struct_p: 0){ _LL1975: _temp1974=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1906)->f1; goto _LL1973; _LL1973:
_temp1972=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1906)->f2;
goto _LL1971; _LL1971: _temp1970=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1906)->f3; goto _LL1969; _LL1969: _temp1968=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1906)->f4; goto _LL1933;} else{ goto
_LL1934;} _LL1934: if(( unsigned int) _temp1906 > 2u?*(( int*) _temp1906) == Cyc_Absyn_Enum_p:
0){ _LL1979: _temp1978=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1906)->f1; goto _LL1977; _LL1977: _temp1976=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1906)->f2; goto _LL1935;} else{ goto
_LL1936;} _LL1936: if(( unsigned int) _temp1906 > 2u?*(( int*) _temp1906) == Cyc_Absyn_Tunion_p:
0){ _LL1987: _temp1986=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1906)->f1; goto _LL1985; _LL1985: _temp1984=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1906)->f2; goto _LL1983; _LL1983:
_temp1982=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1906)->f3; goto _LL1981; _LL1981: _temp1980=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1906)->f4; goto _LL1937;} else{ goto
_LL1907;} _LL1909: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1988=(
char*)"_"; struct _tagged_string _temp1989; _temp1989.curr= _temp1988; _temp1989.base=
_temp1988; _temp1989.last_plus_one= _temp1988 + 2; _temp1989;})); goto _LL1907;
_LL1911: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1990=( char*)"null";
struct _tagged_string _temp1991; _temp1991.curr= _temp1990; _temp1991.base=
_temp1990; _temp1991.last_plus_one= _temp1990 + 5; _temp1991;})); goto _LL1907;
_LL1913: if( _temp1940 ==( void*) Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d",
_temp1938));} else{ s= Cyc_PP_text( xprintf("%u",( unsigned int) _temp1938));}
goto _LL1907; _LL1915: s= Cyc_PP_text(({ struct _tagged_string _temp1992= Cyc_Absynpp_char_escape(
_temp1942); xprintf("'%.*s'", _temp1992.last_plus_one - _temp1992.curr,
_temp1992.curr);})); goto _LL1907; _LL1917: s= Cyc_PP_text( _temp1944); goto
_LL1907; _LL1919: s= Cyc_Absynpp_qvar2doc( _temp1946->name); goto _LL1907;
_LL1921: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1993=( char*)"$("; struct _tagged_string _temp1994; _temp1994.curr=
_temp1993; _temp1994.base= _temp1993; _temp1994.last_plus_one= _temp1993 + 3;
_temp1994;})),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,(
struct _tagged_string)({ char* _temp1995=( char*)","; struct _tagged_string
_temp1996; _temp1996.curr= _temp1995; _temp1996.base= _temp1995; _temp1996.last_plus_one=
_temp1995 + 2; _temp1996;}), _temp1948), Cyc_PP_text(( struct _tagged_string)({
char* _temp1997=( char*)")"; struct _tagged_string _temp1998; _temp1998.curr=
_temp1997; _temp1998.base= _temp1997; _temp1998.last_plus_one= _temp1997 + 2;
_temp1998;}))); goto _LL1907; _LL1923: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1999=( char*)"&"; struct _tagged_string _temp2000;
_temp2000.curr= _temp1999; _temp2000.base= _temp1999; _temp2000.last_plus_one=
_temp1999 + 2; _temp2000;})), Cyc_Absynpp_pat2doc( _temp1950)); goto _LL1907;
_LL1925: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2001=( char*)"*"; struct _tagged_string _temp2002; _temp2002.curr=
_temp2001; _temp2002.base= _temp2001; _temp2002.last_plus_one= _temp2001 + 2;
_temp2002;})), Cyc_Absynpp_qvar2doc( _temp1952->name)); goto _LL1907; _LL1927: s=
Cyc_Absynpp_qvar2doc( _temp1954); goto _LL1907; _LL1929: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1960), Cyc_Absynpp_tvars2doc( _temp1958), Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp2003=( char*)"("; struct _tagged_string
_temp2004; _temp2004.curr= _temp2003; _temp2004.base= _temp2003; _temp2004.last_plus_one=
_temp2003 + 2; _temp2004;}),( struct _tagged_string)({ char* _temp2005=( char*)")";
struct _tagged_string _temp2006; _temp2006.curr= _temp2005; _temp2006.base=
_temp2005; _temp2006.last_plus_one= _temp2005 + 2; _temp2006;}),( struct
_tagged_string)({ char* _temp2007=( char*)","; struct _tagged_string _temp2008;
_temp2008.curr= _temp2007; _temp2008.base= _temp2007; _temp2008.last_plus_one=
_temp2007 + 2; _temp2008;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1956))); goto _LL1907; _LL1931: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1966), Cyc_Absynpp_tvars2doc( _temp1964), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp2009=( char*)"{"; struct _tagged_string _temp2010;
_temp2010.curr= _temp2009; _temp2010.base= _temp2009; _temp2010.last_plus_one=
_temp2009 + 2; _temp2010;}),( struct _tagged_string)({ char* _temp2011=( char*)"}";
struct _tagged_string _temp2012; _temp2012.curr= _temp2011; _temp2012.base=
_temp2011; _temp2012.last_plus_one= _temp2011 + 2; _temp2012;}),( struct
_tagged_string)({ char* _temp2013=( char*)","; struct _tagged_string _temp2014;
_temp2014.curr= _temp2013; _temp2014.base= _temp2013; _temp2014.last_plus_one=
_temp2013 + 2; _temp2014;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1962))); goto _LL1907; _LL1933: s= Cyc_Absynpp_cat3( _temp1974->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp2015= _temp1974->name; if( _temp2015 == 0){ _throw( Null_Exception);}
_temp2015->v;})), Cyc_Absynpp_tvars2doc( _temp1970), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp2016=( char*)"{"; struct _tagged_string _temp2017;
_temp2017.curr= _temp2016; _temp2017.base= _temp2016; _temp2017.last_plus_one=
_temp2016 + 2; _temp2017;}),( struct _tagged_string)({ char* _temp2018=( char*)"}";
struct _tagged_string _temp2019; _temp2019.curr= _temp2018; _temp2019.base=
_temp2018; _temp2019.last_plus_one= _temp2018 + 2; _temp2019;}),( struct
_tagged_string)({ char* _temp2020=( char*)","; struct _tagged_string _temp2021;
_temp2021.curr= _temp2020; _temp2021.base= _temp2020; _temp2021.last_plus_one=
_temp2020 + 2; _temp2021;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1968))); goto _LL1907; _LL1935: s= Cyc_Absynpp_qvar2doc( _temp1976->name);
goto _LL1907; _LL1937: if( _temp1980 == 0){ s= Cyc_Absynpp_qvar2doc( _temp1984->name);}
else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1984->name), Cyc_Absynpp_tvars2doc(
_temp1982), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp2022=( char*)"(";
struct _tagged_string _temp2023; _temp2023.curr= _temp2022; _temp2023.base=
_temp2022; _temp2023.last_plus_one= _temp2022 + 2; _temp2023;}),( struct
_tagged_string)({ char* _temp2024=( char*)")"; struct _tagged_string _temp2025;
_temp2025.curr= _temp2024; _temp2025.base= _temp2024; _temp2025.last_plus_one=
_temp2024 + 2; _temp2025;}),( struct _tagged_string)({ char* _temp2026=( char*)",";
struct _tagged_string _temp2027; _temp2027.curr= _temp2026; _temp2027.base=
_temp2026; _temp2027.last_plus_one= _temp2026 + 2; _temp2027;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1980)));} goto _LL1907; _LL1907:;} return s;} struct
Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return Cyc_Absynpp_cat2(
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp2028=( char*)""; struct
_tagged_string _temp2029; _temp2029.curr= _temp2028; _temp2029.base= _temp2028;
_temp2029.last_plus_one= _temp2028 + 1; _temp2029;}),( struct _tagged_string)({
char* _temp2030=( char*)"="; struct _tagged_string _temp2031; _temp2031.curr=
_temp2030; _temp2031.base= _temp2030; _temp2031.last_plus_one= _temp2030 + 2;
_temp2031;}),( struct _tagged_string)({ char* _temp2032=( char*)"="; struct
_tagged_string _temp2033; _temp2033.curr= _temp2032; _temp2033.base= _temp2032;
_temp2033.last_plus_one= _temp2032 + 2; _temp2033;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)), Cyc_Absynpp_pat2doc((* dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
==( void*) Cyc_Absyn_Wild_p: 0){ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2034=( char*)"default: "; struct _tagged_string
_temp2035; _temp2035.curr= _temp2034; _temp2035.base= _temp2034; _temp2035.last_plus_one=
_temp2034 + 10; _temp2035;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause == 0){ return Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2036=( char*)"case "; struct
_tagged_string _temp2037; _temp2037.curr= _temp2036; _temp2037.base= _temp2036;
_temp2037.last_plus_one= _temp2036 + 6; _temp2037;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2038=( char*)": "; struct
_tagged_string _temp2039; _temp2039.curr= _temp2038; _temp2039.base= _temp2038;
_temp2039.last_plus_one= _temp2038 + 3; _temp2039;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ return Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2040=( char*)"case "; struct
_tagged_string _temp2041; _temp2041.curr= _temp2040; _temp2041.base= _temp2040;
_temp2041.last_plus_one= _temp2040 + 6; _temp2041;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2042=( char*)" && "; struct
_tagged_string _temp2043; _temp2043.curr= _temp2042; _temp2043.base= _temp2042;
_temp2043.last_plus_one= _temp2042 + 5; _temp2043;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2044= c->where_clause; if(
_temp2044 == 0){ _throw( Null_Exception);} _temp2044;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2045=( char*)": "; struct _tagged_string _temp2046;
_temp2046.curr= _temp2045; _temp2046.base= _temp2045; _temp2046.last_plus_one=
_temp2045 + 3; _temp2046;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,( struct _tagged_string)({
char* _temp2047=( char*)""; struct _tagged_string _temp2048; _temp2048.curr=
_temp2047; _temp2048.base= _temp2047; _temp2048.last_plus_one= _temp2047 + 1;
_temp2048;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield*
f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);} else{ return Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( f->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2049=( char*)" = "; struct _tagged_string _temp2050; _temp2050.curr=
_temp2049; _temp2050.base= _temp2049; _temp2050.last_plus_one= _temp2049 + 4;
_temp2050;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2051= f->tag; if( _temp2051 == 0){ _throw( Null_Exception);} _temp2051;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,(
struct _tagged_string)({ char* _temp2052=( char*)","; struct _tagged_string
_temp2053; _temp2053.curr= _temp2052; _temp2053.base= _temp2052; _temp2053.last_plus_one=
_temp2052 + 2; _temp2053;}), fs);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2054=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2078; struct Cyc_Absyn_Structdecl* _temp2080;
struct Cyc_Absyn_Uniondecl* _temp2082; struct Cyc_Absyn_Vardecl* _temp2084;
struct Cyc_Absyn_Vardecl _temp2086; struct Cyc_List_List* _temp2087; struct Cyc_Core_Opt*
_temp2089; struct Cyc_Absyn_Exp* _temp2091; void* _temp2093; struct Cyc_Absyn_Tqual
_temp2095; struct _tuple0* _temp2097; void* _temp2099; struct Cyc_Absyn_Tuniondecl*
_temp2101; struct Cyc_Absyn_Tuniondecl _temp2103; int _temp2104; struct Cyc_Core_Opt*
_temp2106; struct Cyc_List_List* _temp2108; struct _tuple0* _temp2110; void*
_temp2112; int _temp2114; struct Cyc_Absyn_Exp* _temp2116; struct Cyc_Core_Opt*
_temp2118; struct Cyc_Core_Opt* _temp2120; struct Cyc_Absyn_Pat* _temp2122;
struct Cyc_Absyn_Enumdecl* _temp2124; struct Cyc_Absyn_Enumdecl _temp2126;
struct Cyc_List_List* _temp2127; struct _tuple0* _temp2129; void* _temp2131;
struct Cyc_Absyn_Typedefdecl* _temp2133; struct Cyc_List_List* _temp2135; struct
_tagged_string* _temp2137; struct Cyc_List_List* _temp2139; struct _tuple0*
_temp2141; struct Cyc_List_List* _temp2143; _LL2056: if(*(( int*) _temp2054) ==
Cyc_Absyn_Fn_d){ _LL2079: _temp2078=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp2054)->f1; goto _LL2057;} else{ goto _LL2058;} _LL2058: if(*(( int*)
_temp2054) == Cyc_Absyn_Struct_d){ _LL2081: _temp2080=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp2054)->f1; goto _LL2059;} else{ goto
_LL2060;} _LL2060: if(*(( int*) _temp2054) == Cyc_Absyn_Union_d){ _LL2083:
_temp2082=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp2054)->f1; goto _LL2061;} else{ goto _LL2062;} _LL2062: if(*(( int*)
_temp2054) == Cyc_Absyn_Var_d){ _LL2085: _temp2084=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp2054)->f1; _temp2086=* _temp2084; _LL2100:
_temp2099=( void*) _temp2086.sc; goto _LL2098; _LL2098: _temp2097=( struct
_tuple0*) _temp2086.name; goto _LL2096; _LL2096: _temp2095=( struct Cyc_Absyn_Tqual)
_temp2086.tq; goto _LL2094; _LL2094: _temp2093=( void*) _temp2086.type; goto
_LL2092; _LL2092: _temp2091=( struct Cyc_Absyn_Exp*) _temp2086.initializer; goto
_LL2090; _LL2090: _temp2089=( struct Cyc_Core_Opt*) _temp2086.rgn; goto _LL2088;
_LL2088: _temp2087=( struct Cyc_List_List*) _temp2086.attributes; goto _LL2063;}
else{ goto _LL2064;} _LL2064: if(*(( int*) _temp2054) == Cyc_Absyn_Tunion_d){
_LL2102: _temp2101=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2054)->f1; _temp2103=* _temp2101; _LL2113: _temp2112=( void*) _temp2103.sc;
goto _LL2111; _LL2111: _temp2110=( struct _tuple0*) _temp2103.name; goto _LL2109;
_LL2109: _temp2108=( struct Cyc_List_List*) _temp2103.tvs; goto _LL2107; _LL2107:
_temp2106=( struct Cyc_Core_Opt*) _temp2103.fields; goto _LL2105; _LL2105:
_temp2104=( int) _temp2103.is_xtunion; goto _LL2065;} else{ goto _LL2066;}
_LL2066: if(*(( int*) _temp2054) == Cyc_Absyn_Let_d){ _LL2123: _temp2122=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2054)->f1; goto
_LL2121; _LL2121: _temp2120=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2054)->f2; goto _LL2119; _LL2119: _temp2118=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2054)->f3; goto _LL2117; _LL2117: _temp2116=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2054)->f4; goto
_LL2115; _LL2115: _temp2114=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2054)->f5;
goto _LL2067;} else{ goto _LL2068;} _LL2068: if(*(( int*) _temp2054) == Cyc_Absyn_Enum_d){
_LL2125: _temp2124=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp2054)->f1; _temp2126=* _temp2124; _LL2132: _temp2131=( void*) _temp2126.sc;
goto _LL2130; _LL2130: _temp2129=( struct _tuple0*) _temp2126.name; goto _LL2128;
_LL2128: _temp2127=( struct Cyc_List_List*) _temp2126.fields; goto _LL2069;}
else{ goto _LL2070;} _LL2070: if(*(( int*) _temp2054) == Cyc_Absyn_Typedef_d){
_LL2134: _temp2133=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2054)->f1; goto _LL2071;} else{ goto _LL2072;} _LL2072: if(*(( int*)
_temp2054) == Cyc_Absyn_Namespace_d){ _LL2138: _temp2137=( struct _tagged_string*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2054)->f1; goto _LL2136; _LL2136:
_temp2135=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2054)->f2; goto _LL2073;} else{ goto _LL2074;} _LL2074: if(*(( int*)
_temp2054) == Cyc_Absyn_Using_d){ _LL2142: _temp2141=( struct _tuple0*)(( struct
Cyc_Absyn_Using_d_struct*) _temp2054)->f1; goto _LL2140; _LL2140: _temp2139=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp2054)->f2; goto
_LL2075;} else{ goto _LL2076;} _LL2076: if(*(( int*) _temp2054) == Cyc_Absyn_ExternC_d){
_LL2144: _temp2143=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2054)->f1; goto _LL2077;} else{ goto _LL2055;} _LL2057: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2155=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2155[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2156; _temp2156.tag= Cyc_Absyn_FnType; _temp2156.f1=({ struct Cyc_Absyn_FnInfo
_temp2157; _temp2157.tvars= _temp2078->tvs; _temp2157.effect= _temp2078->effect;
_temp2157.ret_typ=( void*)(( void*) _temp2078->ret_type); _temp2157.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2078->args); _temp2157.varargs=
_temp2078->varargs; _temp2157.attributes= 0; _temp2157;}); _temp2156;});
_temp2155;}); s= Cyc_Absynpp_cat7( Cyc_PP_text( _temp2078->is_inline?( struct
_tagged_string)({ char* _temp2145=( char*)"inline "; struct _tagged_string
_temp2146; _temp2146.curr= _temp2145; _temp2146.base= _temp2145; _temp2146.last_plus_one=
_temp2145 + 8; _temp2146;}):( struct _tagged_string)({ char* _temp2147=( char*)"";
struct _tagged_string _temp2148; _temp2148.curr= _temp2147; _temp2148.base=
_temp2147; _temp2148.last_plus_one= _temp2147 + 1; _temp2148;})), Cyc_Absynpp_scope2doc((
void*) _temp2078->sc), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual _temp2149;
_temp2149.q_const= 0; _temp2149.q_volatile= 0; _temp2149.q_restrict= 0;
_temp2149;}), t,({ struct Cyc_Core_Opt* _temp2150=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2150->v=( void*) Cyc_Absynpp_cat2(
Cyc_Absynpp_atts2doc( _temp2078->attributes), Cyc_Absynpp_typedef_name2doc(
_temp2078->name)); _temp2150;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2151=( char*)" {"; struct _tagged_string _temp2152; _temp2152.curr=
_temp2151; _temp2152.base= _temp2151; _temp2152.last_plus_one= _temp2151 + 3;
_temp2152;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp2078->body))), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2153=( char*)"}"; struct _tagged_string _temp2154; _temp2154.curr=
_temp2153; _temp2154.base= _temp2153; _temp2154.last_plus_one= _temp2153 + 2;
_temp2154;}))); goto _LL2055;} _LL2059: if( _temp2080->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp2080->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2158=( char*)"struct "; struct _tagged_string
_temp2159; _temp2159.curr= _temp2158; _temp2159.base= _temp2158; _temp2159.last_plus_one=
_temp2158 + 8; _temp2159;})), _temp2080->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2160=( char*)""; struct _tagged_string _temp2161;
_temp2161.curr= _temp2160; _temp2161.base= _temp2160; _temp2161.last_plus_one=
_temp2160 + 1; _temp2161;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp2162= _temp2080->name; if( _temp2162 == 0){ _throw(
Null_Exception);} _temp2162->v;})), Cyc_Absynpp_ktvars2doc( _temp2080->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2163=( char*)";"; struct _tagged_string
_temp2164; _temp2164.curr= _temp2163; _temp2164.base= _temp2163; _temp2164.last_plus_one=
_temp2163 + 2; _temp2164;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2080->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2165=(
char*)"struct "; struct _tagged_string _temp2166; _temp2166.curr= _temp2165;
_temp2166.base= _temp2165; _temp2166.last_plus_one= _temp2165 + 8; _temp2166;})),
_temp2080->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2167=(
char*)""; struct _tagged_string _temp2168; _temp2168.curr= _temp2167; _temp2168.base=
_temp2167; _temp2168.last_plus_one= _temp2167 + 1; _temp2168;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2169= _temp2080->name; if(
_temp2169 == 0){ _throw( Null_Exception);} _temp2169->v;})), Cyc_Absynpp_ktvars2doc(
_temp2080->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2170=( char*)" {";
struct _tagged_string _temp2171; _temp2171.curr= _temp2170; _temp2171.base=
_temp2170; _temp2171.last_plus_one= _temp2170 + 3; _temp2171;})), Cyc_PP_nest( 2,
_temp2080->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2172=(
char*)""; struct _tagged_string _temp2173; _temp2173.curr= _temp2172; _temp2173.base=
_temp2172; _temp2173.last_plus_one= _temp2172 + 1; _temp2173;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2174= _temp2080->fields; if( _temp2174 == 0){ _throw(
Null_Exception);} _temp2174->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2175=( char*)"}"; struct _tagged_string _temp2176;
_temp2176.curr= _temp2175; _temp2176.base= _temp2175; _temp2176.last_plus_one=
_temp2175 + 2; _temp2176;})), Cyc_Absynpp_atts2doc( _temp2080->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2177=( char*)";"; struct _tagged_string
_temp2178; _temp2178.curr= _temp2177; _temp2178.base= _temp2177; _temp2178.last_plus_one=
_temp2177 + 2; _temp2178;})));} goto _LL2055; _LL2061: if( _temp2082->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp2082->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2179=( char*)"union "; struct _tagged_string
_temp2180; _temp2180.curr= _temp2179; _temp2180.base= _temp2179; _temp2180.last_plus_one=
_temp2179 + 7; _temp2180;})), _temp2082->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2181=( char*)""; struct _tagged_string _temp2182;
_temp2182.curr= _temp2181; _temp2182.base= _temp2181; _temp2182.last_plus_one=
_temp2181 + 1; _temp2182;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2183= _temp2082->name; if( _temp2183 == 0){ _throw(
Null_Exception);} _temp2183->v;})), Cyc_Absynpp_tvars2doc( _temp2082->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2184=( char*)";"; struct _tagged_string
_temp2185; _temp2185.curr= _temp2184; _temp2185.base= _temp2184; _temp2185.last_plus_one=
_temp2184 + 2; _temp2185;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2082->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2186=(
char*)"union "; struct _tagged_string _temp2187; _temp2187.curr= _temp2186;
_temp2187.base= _temp2186; _temp2187.last_plus_one= _temp2186 + 7; _temp2187;})),
_temp2082->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2188=(
char*)""; struct _tagged_string _temp2189; _temp2189.curr= _temp2188; _temp2189.base=
_temp2188; _temp2189.last_plus_one= _temp2188 + 1; _temp2189;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2190= _temp2082->name; if(
_temp2190 == 0){ _throw( Null_Exception);} _temp2190->v;})), Cyc_Absynpp_tvars2doc(
_temp2082->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2191=( char*)" {";
struct _tagged_string _temp2192; _temp2192.curr= _temp2191; _temp2192.base=
_temp2191; _temp2192.last_plus_one= _temp2191 + 3; _temp2192;})), Cyc_PP_nest( 2,
_temp2082->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2193=(
char*)""; struct _tagged_string _temp2194; _temp2194.curr= _temp2193; _temp2194.base=
_temp2193; _temp2194.last_plus_one= _temp2193 + 1; _temp2194;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2195= _temp2082->fields; if( _temp2195 == 0){ _throw(
Null_Exception);} _temp2195->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2196=( char*)"}"; struct _tagged_string _temp2197;
_temp2197.curr= _temp2196; _temp2197.base= _temp2196; _temp2197.last_plus_one=
_temp2196 + 2; _temp2197;})), Cyc_Absynpp_atts2doc( _temp2082->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2198=( char*)";"; struct _tagged_string
_temp2199; _temp2199.curr= _temp2198; _temp2199.base= _temp2198; _temp2199.last_plus_one=
_temp2198 + 2; _temp2199;})));} goto _LL2055; _LL2063: { struct Cyc_PP_Doc* sn=
Cyc_Absynpp_typedef_name2doc( _temp2097); s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(
_temp2099), Cyc_Absynpp_tqtd2doc( _temp2095, _temp2093,({ struct Cyc_Core_Opt*
_temp2200=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2200->v=( void*) sn; _temp2200;})), Cyc_Absynpp_atts2doc( _temp2087),
_temp2091 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2201=( char*)"";
struct _tagged_string _temp2202; _temp2202.curr= _temp2201; _temp2202.base=
_temp2201; _temp2202.last_plus_one= _temp2201 + 1; _temp2202;})): Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2203=( char*)" = "; struct
_tagged_string _temp2204; _temp2204.curr= _temp2203; _temp2204.base= _temp2203;
_temp2204.last_plus_one= _temp2203 + 4; _temp2204;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2205= _temp2091; if(
_temp2205 == 0){ _throw( Null_Exception);} _temp2205;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2206=( char*)";"; struct _tagged_string _temp2207;
_temp2207.curr= _temp2206; _temp2207.base= _temp2206; _temp2207.last_plus_one=
_temp2206 + 2; _temp2207;}))); goto _LL2055;} _LL2065: if( _temp2106 == 0){ s=
Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc( _temp2112), _temp2104? Cyc_PP_text((
struct _tagged_string)({ char* _temp2208=( char*)"xtunion"; struct
_tagged_string _temp2209; _temp2209.curr= _temp2208; _temp2209.base= _temp2208;
_temp2209.last_plus_one= _temp2208 + 8; _temp2209;})): Cyc_PP_text(( struct
_tagged_string)({ char* _temp2210=( char*)"tunion "; struct _tagged_string
_temp2211; _temp2211.curr= _temp2210; _temp2211.base= _temp2210; _temp2211.last_plus_one=
_temp2210 + 8; _temp2211;})), Cyc_Absynpp_typedef_name2doc( _temp2110), Cyc_Absynpp_ktvars2doc(
_temp2108), Cyc_PP_text(( struct _tagged_string)({ char* _temp2212=( char*)";";
struct _tagged_string _temp2213; _temp2213.curr= _temp2212; _temp2213.base=
_temp2212; _temp2213.last_plus_one= _temp2212 + 2; _temp2213;})));} else{ s= Cyc_Absynpp_cat8(
Cyc_Absynpp_scope2doc( _temp2112), _temp2104? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2214=( char*)"xtunion"; struct _tagged_string
_temp2215; _temp2215.curr= _temp2214; _temp2215.base= _temp2214; _temp2215.last_plus_one=
_temp2214 + 8; _temp2215;})): Cyc_PP_text(( struct _tagged_string)({ char*
_temp2216=( char*)"tunion "; struct _tagged_string _temp2217; _temp2217.curr=
_temp2216; _temp2217.base= _temp2216; _temp2217.last_plus_one= _temp2216 + 8;
_temp2217;})), Cyc_Absynpp_typedef_name2doc( _temp2110), Cyc_Absynpp_ktvars2doc(
_temp2108), Cyc_PP_text(( struct _tagged_string)({ char* _temp2218=( char*)" {";
struct _tagged_string _temp2219; _temp2219.curr= _temp2218; _temp2219.base=
_temp2218; _temp2219.last_plus_one= _temp2218 + 3; _temp2219;})), Cyc_PP_nest( 2,
_temp2106 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2220=( char*)"";
struct _tagged_string _temp2221; _temp2221.curr= _temp2220; _temp2221.base=
_temp2220; _temp2221.last_plus_one= _temp2220 + 1; _temp2221;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2222= _temp2106; if( _temp2222 == 0){ _throw(
Null_Exception);} _temp2222->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2223=( char*)"};"; struct _tagged_string _temp2224;
_temp2224.curr= _temp2223; _temp2224.base= _temp2223; _temp2224.last_plus_one=
_temp2223 + 3; _temp2224;})));} goto _LL2055; _LL2067: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp2225=( char*)"let "; struct _tagged_string
_temp2226; _temp2226.curr= _temp2225; _temp2226.base= _temp2225; _temp2226.last_plus_one=
_temp2225 + 5; _temp2226;})), Cyc_Absynpp_pat2doc( _temp2122), Cyc_PP_text((
struct _tagged_string)({ char* _temp2227=( char*)" = "; struct _tagged_string
_temp2228; _temp2228.curr= _temp2227; _temp2228.base= _temp2227; _temp2228.last_plus_one=
_temp2227 + 4; _temp2228;})), Cyc_Absynpp_exp2doc( _temp2116), Cyc_PP_text((
struct _tagged_string)({ char* _temp2229=( char*)";"; struct _tagged_string
_temp2230; _temp2230.curr= _temp2229; _temp2230.base= _temp2229; _temp2230.last_plus_one=
_temp2229 + 2; _temp2230;}))); goto _LL2055; _LL2069: s= Cyc_Absynpp_cat7( Cyc_Absynpp_scope2doc(
_temp2131), Cyc_PP_text(( struct _tagged_string)({ char* _temp2231=( char*)"enum ";
struct _tagged_string _temp2232; _temp2232.curr= _temp2231; _temp2232.base=
_temp2231; _temp2232.last_plus_one= _temp2231 + 6; _temp2232;})), Cyc_Absynpp_qvar2doc(
_temp2129), Cyc_PP_text(( struct _tagged_string)({ char* _temp2233=( char*)" {";
struct _tagged_string _temp2234; _temp2234.curr= _temp2233; _temp2234.base=
_temp2233; _temp2234.last_plus_one= _temp2233 + 3; _temp2234;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_enumfields2doc( _temp2127))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp2235=( char*)"};";
struct _tagged_string _temp2236; _temp2236.curr= _temp2235; _temp2236.base=
_temp2235; _temp2236.last_plus_one= _temp2235 + 3; _temp2236;}))); goto _LL2055;
_LL2071: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2237=( char*)"typedef "; struct _tagged_string _temp2238; _temp2238.curr=
_temp2237; _temp2238.base= _temp2237; _temp2238.last_plus_one= _temp2237 + 9;
_temp2238;})), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual _temp2239;
_temp2239.q_const= 0; _temp2239.q_volatile= 0; _temp2239.q_restrict= 0;
_temp2239;}),( void*) _temp2133->defn,({ struct Cyc_Core_Opt* _temp2240=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2240->v=( void*)
Cyc_Absynpp_cat2( Cyc_Absynpp_typedef_name2doc( _temp2133->name), Cyc_Absynpp_tvars2doc(
_temp2133->tvs)); _temp2240;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2241=( char*)";"; struct _tagged_string _temp2242; _temp2242.curr=
_temp2241; _temp2242.base= _temp2241; _temp2242.last_plus_one= _temp2241 + 2;
_temp2242;}))); goto _LL2055; _LL2073: if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_add(
_temp2137);} s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2243=( char*)"namespace "; struct _tagged_string _temp2244; _temp2244.curr=
_temp2243; _temp2244.base= _temp2243; _temp2244.last_plus_one= _temp2243 + 11;
_temp2244;})), Cyc_Absynpp_textptr( _temp2137), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2245=( char*)" {"; struct _tagged_string _temp2246;
_temp2246.curr= _temp2245; _temp2246.base= _temp2245; _temp2246.last_plus_one=
_temp2245 + 3; _temp2246;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2247=( char*)""; struct _tagged_string _temp2248; _temp2248.curr=
_temp2247; _temp2248.base= _temp2247; _temp2248.last_plus_one= _temp2247 + 1;
_temp2248;}), _temp2135), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2249=( char*)"}"; struct _tagged_string _temp2250; _temp2250.curr=
_temp2249; _temp2250.base= _temp2249; _temp2250.last_plus_one= _temp2249 + 2;
_temp2250;}))); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2055; _LL2075: if( Cyc_Absynpp_print_using_stmts){ s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2251=( char*)"using "; struct
_tagged_string _temp2252; _temp2252.curr= _temp2251; _temp2252.base= _temp2251;
_temp2252.last_plus_one= _temp2251 + 7; _temp2252;})), Cyc_Absynpp_qvar2doc(
_temp2141), Cyc_PP_text(( struct _tagged_string)({ char* _temp2253=( char*)" {";
struct _tagged_string _temp2254; _temp2254.curr= _temp2253; _temp2254.base=
_temp2253; _temp2254.last_plus_one= _temp2253 + 3; _temp2254;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2255=( char*)""; struct _tagged_string
_temp2256; _temp2256.curr= _temp2255; _temp2256.base= _temp2255; _temp2256.last_plus_one=
_temp2255 + 1; _temp2256;}), _temp2139), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2257=( char*)"}"; struct _tagged_string _temp2258;
_temp2258.curr= _temp2257; _temp2258.base= _temp2257; _temp2258.last_plus_one=
_temp2257 + 2; _temp2258;})));} else{ s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2259=( char*)"/* using "; struct _tagged_string
_temp2260; _temp2260.curr= _temp2259; _temp2260.base= _temp2259; _temp2260.last_plus_one=
_temp2259 + 10; _temp2260;})), Cyc_Absynpp_qvar2doc( _temp2141), Cyc_PP_text((
struct _tagged_string)({ char* _temp2261=( char*)" { */"; struct _tagged_string
_temp2262; _temp2262.curr= _temp2261; _temp2262.base= _temp2261; _temp2262.last_plus_one=
_temp2261 + 6; _temp2262;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2263=( char*)""; struct _tagged_string _temp2264; _temp2264.curr=
_temp2263; _temp2264.base= _temp2263; _temp2264.last_plus_one= _temp2263 + 1;
_temp2264;}), _temp2139), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2265=( char*)"/* } */"; struct _tagged_string _temp2266; _temp2266.curr=
_temp2265; _temp2266.base= _temp2265; _temp2266.last_plus_one= _temp2265 + 8;
_temp2266;})));} goto _LL2055; _LL2077: if( Cyc_Absynpp_print_externC_stmts){ s=
Cyc_Absynpp_cat5( Cyc_PP_text(( struct _tagged_string)({ char* _temp2267=( char*)"extern \"C\" {";
struct _tagged_string _temp2268; _temp2268.curr= _temp2267; _temp2268.base=
_temp2267; _temp2268.last_plus_one= _temp2267 + 13; _temp2268;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2269=( char*)""; struct _tagged_string
_temp2270; _temp2270.curr= _temp2269; _temp2270.base= _temp2269; _temp2270.last_plus_one=
_temp2269 + 1; _temp2270;}), _temp2143), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2271=( char*)"}"; struct _tagged_string _temp2272;
_temp2272.curr= _temp2271; _temp2272.base= _temp2271; _temp2272.last_plus_one=
_temp2271 + 2; _temp2272;})));} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2273=( char*)"/* extern \"C\" { */"; struct
_tagged_string _temp2274; _temp2274.curr= _temp2273; _temp2274.base= _temp2273;
_temp2274.last_plus_one= _temp2273 + 19; _temp2274;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2275=( char*)""; struct _tagged_string
_temp2276; _temp2276.curr= _temp2275; _temp2276.base= _temp2275; _temp2276.last_plus_one=
_temp2275 + 1; _temp2276;}), _temp2143), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2277=( char*)"/* } */"; struct _tagged_string
_temp2278; _temp2278.curr= _temp2277; _temp2278.base= _temp2277; _temp2278.last_plus_one=
_temp2277 + 8; _temp2278;})));} goto _LL2055; _LL2055:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void* sc){ void* _temp2279= sc; _LL2281: if( _temp2279 ==(
void*) Cyc_Absyn_Static){ goto _LL2282;} else{ goto _LL2283;} _LL2283: if(
_temp2279 ==( void*) Cyc_Absyn_Public){ goto _LL2284;} else{ goto _LL2285;}
_LL2285: if( _temp2279 ==( void*) Cyc_Absyn_Extern){ goto _LL2286;} else{ goto
_LL2287;} _LL2287: if( _temp2279 ==( void*) Cyc_Absyn_ExternC){ goto _LL2288;}
else{ goto _LL2289;} _LL2289: if( _temp2279 ==( void*) Cyc_Absyn_Abstract){ goto
_LL2290;} else{ goto _LL2280;} _LL2282: return Cyc_PP_text(( struct
_tagged_string)({ char* _temp2291=( char*)"static "; struct _tagged_string
_temp2292; _temp2292.curr= _temp2291; _temp2292.base= _temp2291; _temp2292.last_plus_one=
_temp2291 + 8; _temp2292;})); _LL2284: return Cyc_PP_nil_doc(); _LL2286: return
Cyc_PP_text(( struct _tagged_string)({ char* _temp2293=( char*)"extern "; struct
_tagged_string _temp2294; _temp2294.curr= _temp2293; _temp2294.base= _temp2293;
_temp2294.last_plus_one= _temp2293 + 8; _temp2294;})); _LL2288: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2295=( char*)"extern \"C\" "; struct
_tagged_string _temp2296; _temp2296.curr= _temp2295; _temp2296.base= _temp2295;
_temp2296.last_plus_one= _temp2295 + 12; _temp2296;})); _LL2290: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2297=( char*)"abstract "; struct
_tagged_string _temp2298; _temp2298.curr= _temp2297; _temp2298.base= _temp2297;
_temp2298.last_plus_one= _temp2297 + 10; _temp2298;})); _LL2280:;} int Cyc_Absynpp_exists_temp_tvar_in_effect(
void* t){ void* _temp2299= t; struct Cyc_Absyn_Tvar* _temp2307; struct Cyc_List_List*
_temp2309; _LL2301: if(( unsigned int) _temp2299 > 4u?*(( int*) _temp2299) ==
Cyc_Absyn_VarType: 0){ _LL2308: _temp2307=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp2299)->f1; goto _LL2302;} else{ goto _LL2303;} _LL2303: if(( unsigned int)
_temp2299 > 4u?*(( int*) _temp2299) == Cyc_Absyn_JoinEff: 0){ _LL2310: _temp2309=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp2299)->f1; goto
_LL2304;} else{ goto _LL2305;} _LL2305: goto _LL2306; _LL2302: return Cyc_Tcutil_is_temp_tvar(
_temp2307); _LL2304: return(( int(*)( int(* pred)( void*), struct Cyc_List_List*
x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect, _temp2309);
_LL2306: return 0; _LL2300:;} struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual
tq, void* t){ void* _temp2311= t; struct Cyc_Absyn_Exp* _temp2325; struct Cyc_Absyn_Tqual
_temp2327; void* _temp2329; struct Cyc_Absyn_PtrInfo _temp2331; struct Cyc_Absyn_Conref*
_temp2333; struct Cyc_Absyn_Tqual _temp2335; struct Cyc_Absyn_Conref* _temp2337;
void* _temp2339; void* _temp2341; struct Cyc_Absyn_FnInfo _temp2343; struct Cyc_List_List*
_temp2345; int _temp2347; struct Cyc_List_List* _temp2349; void* _temp2351;
struct Cyc_Core_Opt* _temp2353; struct Cyc_List_List* _temp2355; int _temp2357;
struct Cyc_Core_Opt* _temp2359; void* _temp2361; struct Cyc_Core_Opt* _temp2363;
struct Cyc_List_List* _temp2365; struct _tuple0* _temp2367; _LL2313: if((
unsigned int) _temp2311 > 4u?*(( int*) _temp2311) == Cyc_Absyn_ArrayType: 0){
_LL2330: _temp2329=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2311)->f1;
goto _LL2328; _LL2328: _temp2327=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2311)->f2; goto _LL2326; _LL2326: _temp2325=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2311)->f3; goto _LL2314;} else{ goto
_LL2315;} _LL2315: if(( unsigned int) _temp2311 > 4u?*(( int*) _temp2311) == Cyc_Absyn_PointerType:
0){ _LL2332: _temp2331=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2311)->f1; _LL2342: _temp2341=( void*) _temp2331.elt_typ; goto _LL2340;
_LL2340: _temp2339=( void*) _temp2331.rgn_typ; goto _LL2338; _LL2338: _temp2337=(
struct Cyc_Absyn_Conref*) _temp2331.nullable; goto _LL2336; _LL2336: _temp2335=(
struct Cyc_Absyn_Tqual) _temp2331.tq; goto _LL2334; _LL2334: _temp2333=( struct
Cyc_Absyn_Conref*) _temp2331.bounds; goto _LL2316;} else{ goto _LL2317;} _LL2317:
if(( unsigned int) _temp2311 > 4u?*(( int*) _temp2311) == Cyc_Absyn_FnType: 0){
_LL2344: _temp2343=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2311)->f1; _LL2356: _temp2355=( struct Cyc_List_List*) _temp2343.tvars;
goto _LL2354; _LL2354: _temp2353=( struct Cyc_Core_Opt*) _temp2343.effect; goto
_LL2352; _LL2352: _temp2351=( void*) _temp2343.ret_typ; goto _LL2350; _LL2350:
_temp2349=( struct Cyc_List_List*) _temp2343.args; goto _LL2348; _LL2348:
_temp2347=( int) _temp2343.varargs; goto _LL2346; _LL2346: _temp2345=( struct
Cyc_List_List*) _temp2343.attributes; goto _LL2318;} else{ goto _LL2319;}
_LL2319: if(( unsigned int) _temp2311 > 4u?*(( int*) _temp2311) == Cyc_Absyn_Evar:
0){ _LL2362: _temp2361=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp2311)->f1;
goto _LL2360; _LL2360: _temp2359=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp2311)->f2; goto _LL2358; _LL2358: _temp2357=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp2311)->f3; goto _LL2320;} else{ goto _LL2321;} _LL2321: if(( unsigned int)
_temp2311 > 4u?*(( int*) _temp2311) == Cyc_Absyn_TypedefType: 0){ _LL2368:
_temp2367=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2311)->f1;
goto _LL2366; _LL2366: _temp2365=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2311)->f2; goto _LL2364; _LL2364: _temp2363=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2311)->f3; goto _LL2322;} else{ goto
_LL2323;} _LL2323: goto _LL2324; _LL2314: { struct Cyc_List_List* _temp2371;
void* _temp2373; struct Cyc_Absyn_Tqual _temp2375; struct _tuple4 _temp2369= Cyc_Absynpp_to_tms(
_temp2327, _temp2329); _LL2376: _temp2375= _temp2369.f1; goto _LL2374; _LL2374:
_temp2373= _temp2369.f2; goto _LL2372; _LL2372: _temp2371= _temp2369.f3; goto
_LL2370; _LL2370: { void* tm; if( _temp2325 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;}
else{ tm=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct* _temp2377=( struct
Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct));
_temp2377[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct _temp2378; _temp2378.tag=
Cyc_Absyn_ConstArray_mod; _temp2378.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2379= _temp2325; if( _temp2379 == 0){ _throw( Null_Exception);} _temp2379;});
_temp2378;}); _temp2377;});} return({ struct _tuple4 _temp2380; _temp2380.f1=
_temp2375; _temp2380.f2= _temp2373; _temp2380.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2381=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2381->hd=( void*) tm; _temp2381->tl= _temp2371;
_temp2381;}); _temp2380;});}} _LL2316: { struct Cyc_List_List* _temp2384; void*
_temp2386; struct Cyc_Absyn_Tqual _temp2388; struct _tuple4 _temp2382= Cyc_Absynpp_to_tms(
_temp2335, _temp2341); _LL2389: _temp2388= _temp2382.f1; goto _LL2387; _LL2387:
_temp2386= _temp2382.f2; goto _LL2385; _LL2385: _temp2384= _temp2382.f3; goto
_LL2383; _LL2383: { void* ps;{ struct _tuple8 _temp2391=({ struct _tuple8
_temp2390; _temp2390.f1=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2337))->v; _temp2390.f2=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2333))->v; _temp2390;}); void* _temp2401; void* _temp2403; struct Cyc_Absyn_Exp*
_temp2405; void* _temp2407; int _temp2409; void* _temp2411; void* _temp2413;
struct Cyc_Absyn_Exp* _temp2415; void* _temp2417; int _temp2419; void* _temp2421;
void* _temp2423; void* _temp2425; _LL2393: _LL2408: _temp2407= _temp2391.f1; if((
unsigned int) _temp2407 > 1u?*(( int*) _temp2407) == Cyc_Absyn_Eq_constr: 0){
_LL2410: _temp2409=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2407)->f1;
if( _temp2409 == 1){ goto _LL2402;} else{ goto _LL2395;}} else{ goto _LL2395;}
_LL2402: _temp2401= _temp2391.f2; if(( unsigned int) _temp2401 > 1u?*(( int*)
_temp2401) == Cyc_Absyn_Eq_constr: 0){ _LL2404: _temp2403=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2401)->f1; if(( unsigned int) _temp2403 > 1u?*(( int*) _temp2403) == Cyc_Absyn_Upper_b:
0){ _LL2406: _temp2405=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2403)->f1; goto _LL2394;} else{ goto _LL2395;}} else{ goto _LL2395;}
_LL2395: _LL2418: _temp2417= _temp2391.f1; if(( unsigned int) _temp2417 > 1u?*((
int*) _temp2417) == Cyc_Absyn_Eq_constr: 0){ _LL2420: _temp2419=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp2417)->f1; if( _temp2419 == 0){ goto _LL2412;}
else{ goto _LL2397;}} else{ goto _LL2397;} _LL2412: _temp2411= _temp2391.f2; if((
unsigned int) _temp2411 > 1u?*(( int*) _temp2411) == Cyc_Absyn_Eq_constr: 0){
_LL2414: _temp2413=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2411)->f1;
if(( unsigned int) _temp2413 > 1u?*(( int*) _temp2413) == Cyc_Absyn_Upper_b: 0){
_LL2416: _temp2415=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2413)->f1; goto _LL2396;} else{ goto _LL2397;}} else{ goto _LL2397;}
_LL2397: _LL2426: _temp2425= _temp2391.f1; goto _LL2422; _LL2422: _temp2421=
_temp2391.f2; if(( unsigned int) _temp2421 > 1u?*(( int*) _temp2421) == Cyc_Absyn_Eq_constr:
0){ _LL2424: _temp2423=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2421)->f1;
if( _temp2423 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2398;} else{ goto _LL2399;}}
else{ goto _LL2399;} _LL2399: goto _LL2400; _LL2394: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2427=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2427[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2428; _temp2428.tag=
Cyc_Absyn_Nullable_ps; _temp2428.f1= _temp2405; _temp2428;}); _temp2427;}); goto
_LL2392; _LL2396: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2429=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2429[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2430; _temp2430.tag= Cyc_Absyn_NonNullable_ps; _temp2430.f1= _temp2415;
_temp2430;}); _temp2429;}); goto _LL2392; _LL2398: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2392; _LL2400: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2392;
_LL2392:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2433=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2433[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2434; _temp2434.tag=
Cyc_Absyn_Pointer_mod; _temp2434.f1=( void*) ps; _temp2434.f2=( void*) _temp2339;
_temp2434.f3= tq; _temp2434;}); _temp2433;}); return({ struct _tuple4 _temp2431;
_temp2431.f1= _temp2388; _temp2431.f2= _temp2386; _temp2431.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2432=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2432->hd=( void*) tm; _temp2432->tl= _temp2384;
_temp2432;}); _temp2431;});}}} _LL2318: if( ! Cyc_Absynpp_print_all_tvars){ if(
_temp2353 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)({ struct Cyc_Core_Opt*
_temp2435= _temp2353; if( _temp2435 == 0){ _throw( Null_Exception);} _temp2435->v;}))){
_temp2353= 0; _temp2355= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2355);}}{ struct Cyc_List_List* _temp2438;
void* _temp2440; struct Cyc_Absyn_Tqual _temp2442; struct _tuple4 _temp2436= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp2351); _LL2443: _temp2442= _temp2436.f1; goto
_LL2441; _LL2441: _temp2440= _temp2436.f2; goto _LL2439; _LL2439: _temp2438=
_temp2436.f3; goto _LL2437; _LL2437: { struct Cyc_List_List* tms= _temp2438; if(
_temp2345 != 0){ tms=({ struct Cyc_List_List* _temp2444=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2444->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2445=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2445[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2446; _temp2446.tag= Cyc_Absyn_Attributes_mod;
_temp2446.f1= 0; _temp2446.f2= _temp2345; _temp2446;}); _temp2445;})); _temp2444->tl=
tms; _temp2444;});} tms=({ struct Cyc_List_List* _temp2447=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2447->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2448=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2448[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2449; _temp2449.tag= Cyc_Absyn_Function_mod;
_temp2449.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2450=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2450[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2451; _temp2451.tag= Cyc_Absyn_WithTypes;
_temp2451.f1= _temp2349; _temp2451.f2= _temp2347; _temp2451.f3= _temp2353;
_temp2451;}); _temp2450;})); _temp2449;}); _temp2448;})); _temp2447->tl= tms;
_temp2447;}); if( _temp2355 != 0){ tms=({ struct Cyc_List_List* _temp2452=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2452->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2453=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2453[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2454; _temp2454.tag= Cyc_Absyn_TypeParams_mod;
_temp2454.f1= _temp2355; _temp2454.f2= 0; _temp2454.f3= 1; _temp2454;});
_temp2453;})); _temp2452->tl= tms; _temp2452;});} return({ struct _tuple4
_temp2455; _temp2455.f1= _temp2442; _temp2455.f2= _temp2440; _temp2455.f3= tms;
_temp2455;});}} _LL2320: if( _temp2359 == 0){ return({ struct _tuple4 _temp2456;
_temp2456.f1= tq; _temp2456.f2= t; _temp2456.f3= 0; _temp2456;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2457= _temp2359; if(
_temp2457 == 0){ _throw( Null_Exception);} _temp2457->v;}));} _LL2322: if(
_temp2363 == 0? 1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4
_temp2458; _temp2458.f1= tq; _temp2458.f2= t; _temp2458.f3= 0; _temp2458;});}
else{ return Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2459=
_temp2363; if( _temp2459 == 0){ _throw( Null_Exception);} _temp2459->v;}));}
_LL2324: return({ struct _tuple4 _temp2460; _temp2460.f1= tq; _temp2460.f2= t;
_temp2460.f3= 0; _temp2460;}); _LL2312:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp2463; void* _temp2465; struct Cyc_Absyn_Tqual _temp2467; struct _tuple4
_temp2461= Cyc_Absynpp_to_tms( tq, t); _LL2468: _temp2467= _temp2461.f1; goto
_LL2466; _LL2466: _temp2465= _temp2461.f2; goto _LL2464; _LL2464: _temp2463=
_temp2461.f3; goto _LL2462; _LL2462: _temp2463=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2463); if( _temp2463 == 0?
dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc( _temp2467), Cyc_Absynpp_ntyp2doc(
_temp2465));} else{ return Cyc_Absynpp_cat4( Cyc_Absynpp_tqual2doc( _temp2467),
Cyc_Absynpp_ntyp2doc( _temp2465), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2469=( char*)" "; struct _tagged_string _temp2470; _temp2470.curr=
_temp2469; _temp2470.base= _temp2469; _temp2470.last_plus_one= _temp2469 + 2;
_temp2470;})), Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)({
struct Cyc_Core_Opt* _temp2471= dopt; if( _temp2471 == 0){ _throw(
Null_Exception);} _temp2471->v;}), _temp2463));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return Cyc_Absynpp_cat5(
Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2472=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2472->v=(
void*) Cyc_Absynpp_textptr( f->name); _temp2472;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2473=( char*)":"; struct _tagged_string _temp2474;
_temp2474.curr= _temp2473; _temp2474.base= _temp2473; _temp2474.last_plus_one=
_temp2473 + 2; _temp2474;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp2475= f->width; if( _temp2475 == 0){ _throw(
Null_Exception);} _temp2475->v;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2476=( char*)";"; struct _tagged_string
_temp2477; _temp2477.curr= _temp2476; _temp2477.base= _temp2476; _temp2477.last_plus_one=
_temp2476 + 2; _temp2477;})));} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2478=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2478->v=( void*) Cyc_Absynpp_textptr(
f->name); _temp2478;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2479=( char*)";"; struct _tagged_string
_temp2480; _temp2480.curr= _temp2479; _temp2480.base= _temp2479; _temp2480.last_plus_one=
_temp2479 + 2; _temp2480;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,( struct _tagged_string)({ char*
_temp2481=( char*)""; struct _tagged_string _temp2482; _temp2482.curr= _temp2481;
_temp2482.base= _temp2481; _temp2482.last_plus_one= _temp2481 + 1; _temp2482;}),
fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc( struct Cyc_Absyn_Tunionfield*
f){ return Cyc_Absynpp_cat3( Cyc_Absynpp_typedef_name2doc( f->name), f->tvs == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs), f->typs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,( struct _tagged_string)({ char*
_temp2483=( char*)","; struct _tagged_string _temp2484; _temp2484.curr=
_temp2483; _temp2484.base= _temp2483; _temp2484.last_plus_one= _temp2483 + 2;
_temp2484;}), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl=({ struct Cyc_List_List*
_temp2485= tdl; if( _temp2485 == 0){ _throw( Null_Exception);} _temp2485->tl;})){
Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp2486= tdl; if( _temp2486 == 0){ _throw( Null_Exception);} _temp2486->hd;})),
72, f); fprintf( f,"\n");}} struct _tagged_string Cyc_Absynpp_decllist2string(
struct Cyc_List_List* tdl){ return Cyc_PP_string_of_doc( Cyc_PP_seql(( struct
_tagged_string)({ char* _temp2487=( char*)""; struct _tagged_string _temp2488;
_temp2488.curr= _temp2487; _temp2488.base= _temp2487; _temp2488.last_plus_one=
_temp2487 + 1; _temp2488;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_decl2doc,
tdl)), 72);} struct _tagged_string Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*
e){ return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e), 72);} struct
_tagged_string Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_stmt2doc( s), 72);} struct _tagged_string Cyc_Absynpp_typ2string(
void* t){ Cyc_Absynpp_dont_rewrite_temp_tvars= 1;{ struct _tagged_string s= Cyc_PP_string_of_doc(
Cyc_Absynpp_typ2doc( t), 72); Cyc_Absynpp_dont_rewrite_temp_tvars= 0; return s;}}
struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0* v){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_qvar2doc( v), 72);} struct _tagged_string Cyc_Absynpp_prim2string(
void* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_prim2doc( p), 72);} struct
_tagged_string Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_pat2doc( p), 72);}