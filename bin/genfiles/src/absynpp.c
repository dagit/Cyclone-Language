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
_LL31: if( c >=' '? c <='~': 0){ struct _tagged_string _temp35= Cyc_Core_new_string(
1); goto _LL36; _LL36:({ struct _tagged_string _temp37= _temp35; char* _temp39=
_temp37.curr + 0; if( _temp37.base == 0? 1:( _temp39 < _temp37.base? 1: _temp39
>= _temp37.last_plus_one)){ _throw( Null_Exception);}* _temp39= c;}); return
_temp35;} else{ struct _tagged_string _temp40= Cyc_Core_new_string( 4); goto
_LL41; _LL41: { int j= 0;({ struct _tagged_string _temp42= _temp40; char*
_temp44= _temp42.curr +( j ++); if( _temp42.base == 0? 1:( _temp44 < _temp42.base?
1: _temp44 >= _temp42.last_plus_one)){ _throw( Null_Exception);}* _temp44='\\';});({
struct _tagged_string _temp45= _temp40; char* _temp47= _temp45.curr +( j ++);
if( _temp45.base == 0? 1:( _temp47 < _temp45.base? 1: _temp47 >= _temp45.last_plus_one)){
_throw( Null_Exception);}* _temp47=( char)('0' +( c >> 6 & 7));});({ struct
_tagged_string _temp48= _temp40; char* _temp50= _temp48.curr +( j ++); if(
_temp48.base == 0? 1:( _temp50 < _temp48.base? 1: _temp50 >= _temp48.last_plus_one)){
_throw( Null_Exception);}* _temp50=( char)('0' +( c >> 3 & 7));});({ struct
_tagged_string _temp51= _temp40; char* _temp53= _temp51.curr +( j ++); if(
_temp51.base == 0? 1:( _temp53 < _temp51.base? 1: _temp53 >= _temp51.last_plus_one)){
_throw( Null_Exception);}* _temp53=( char)('0' +( c & 7));}); return _temp40;}}}}
static int Cyc_Absynpp_special( struct _tagged_string s){ int sz=( int)(({
struct _tagged_string _temp57= s;( unsigned int)( _temp57.last_plus_one -
_temp57.curr);}) - 1);{ int i= 0; for( 0; i < sz; i ++){ char c=({ struct
_tagged_string _temp54= s; char* _temp56= _temp54.curr + i; if( _temp54.base ==
0? 1:( _temp56 < _temp54.base? 1: _temp56 >= _temp54.last_plus_one)){ _throw(
Null_Exception);}* _temp56;}); if((( c <=' '? 1: c >='~')? 1: c =='"')? 1: c =='\\'){
return 1;}}} return 0;} struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string s){ if( ! Cyc_Absynpp_special( s)){ return s;}{ int n=( int)(({
struct _tagged_string _temp180= s;( unsigned int)( _temp180.last_plus_one -
_temp180.curr);}) - 1); if( n > 0?({ struct _tagged_string _temp58= s; char*
_temp60= _temp58.curr + n; if( _temp58.base == 0? 1:( _temp60 < _temp58.base? 1:
_temp60 >= _temp58.last_plus_one)){ _throw( Null_Exception);}* _temp60;}) =='\000':
0){ n --;}{ int len= 0;{ int i= 0; for( 0; i <= n; i ++){ char _temp64=({ struct
_tagged_string _temp61= s; char* _temp63= _temp61.curr + i; if( _temp61.base ==
0? 1:( _temp63 < _temp61.base? 1: _temp63 >= _temp61.last_plus_one)){ _throw(
Null_Exception);}* _temp63;}); _LL66: if( _temp64 =='\a'){ goto _LL67;} else{
goto _LL68;} _LL68: if( _temp64 =='\b'){ goto _LL69;} else{ goto _LL70;} _LL70:
if( _temp64 =='\f'){ goto _LL71;} else{ goto _LL72;} _LL72: if( _temp64 =='\n'){
goto _LL73;} else{ goto _LL74;} _LL74: if( _temp64 =='\r'){ goto _LL75;} else{
goto _LL76;} _LL76: if( _temp64 =='\t'){ goto _LL77;} else{ goto _LL78;} _LL78:
if( _temp64 =='\v'){ goto _LL79;} else{ goto _LL80;} _LL80: if( _temp64 =='\\'){
goto _LL81;} else{ goto _LL82;} _LL82: if( _temp64 =='"'){ goto _LL83;} else{
goto _LL84;} _LL84: goto _LL85; _LL67: goto _LL69; _LL69: goto _LL71; _LL71:
goto _LL73; _LL73: goto _LL75; _LL75: goto _LL77; _LL77: goto _LL79; _LL79: goto
_LL81; _LL81: goto _LL83; _LL83: len += 2; goto _LL65; _LL85: if( _temp64 >=' '?
_temp64 <='~': 0){ len ++;} else{ len += 4;} goto _LL65; _LL65:;}}{ struct
_tagged_string t= Cyc_Core_new_string( len); int j= 0;{ int i= 0; for( 0; i <= n;
i ++){ char _temp89=({ struct _tagged_string _temp86= s; char* _temp88= _temp86.curr
+ i; if( _temp86.base == 0? 1:( _temp88 < _temp86.base? 1: _temp88 >= _temp86.last_plus_one)){
_throw( Null_Exception);}* _temp88;}); _LL91: if( _temp89 =='\a'){ goto _LL92;}
else{ goto _LL93;} _LL93: if( _temp89 =='\b'){ goto _LL94;} else{ goto _LL95;}
_LL95: if( _temp89 =='\f'){ goto _LL96;} else{ goto _LL97;} _LL97: if( _temp89
=='\n'){ goto _LL98;} else{ goto _LL99;} _LL99: if( _temp89 =='\r'){ goto _LL100;}
else{ goto _LL101;} _LL101: if( _temp89 =='\t'){ goto _LL102;} else{ goto _LL103;}
_LL103: if( _temp89 =='\v'){ goto _LL104;} else{ goto _LL105;} _LL105: if(
_temp89 =='\\'){ goto _LL106;} else{ goto _LL107;} _LL107: if( _temp89 =='"'){
goto _LL108;} else{ goto _LL109;} _LL109: goto _LL110; _LL92:({ struct
_tagged_string _temp111= t; char* _temp113= _temp111.curr +( j ++); if( _temp111.base
== 0? 1:( _temp113 < _temp111.base? 1: _temp113 >= _temp111.last_plus_one)){
_throw( Null_Exception);}* _temp113='\\';});({ struct _tagged_string _temp114= t;
char* _temp116= _temp114.curr +( j ++); if( _temp114.base == 0? 1:( _temp116 <
_temp114.base? 1: _temp116 >= _temp114.last_plus_one)){ _throw( Null_Exception);}*
_temp116='a';}); goto _LL90; _LL94:({ struct _tagged_string _temp117= t; char*
_temp119= _temp117.curr +( j ++); if( _temp117.base == 0? 1:( _temp119 <
_temp117.base? 1: _temp119 >= _temp117.last_plus_one)){ _throw( Null_Exception);}*
_temp119='\\';});({ struct _tagged_string _temp120= t; char* _temp122= _temp120.curr
+( j ++); if( _temp120.base == 0? 1:( _temp122 < _temp120.base? 1: _temp122 >=
_temp120.last_plus_one)){ _throw( Null_Exception);}* _temp122='b';}); goto _LL90;
_LL96:({ struct _tagged_string _temp123= t; char* _temp125= _temp123.curr +( j
++); if( _temp123.base == 0? 1:( _temp125 < _temp123.base? 1: _temp125 >=
_temp123.last_plus_one)){ _throw( Null_Exception);}* _temp125='\\';});({ struct
_tagged_string _temp126= t; char* _temp128= _temp126.curr +( j ++); if( _temp126.base
== 0? 1:( _temp128 < _temp126.base? 1: _temp128 >= _temp126.last_plus_one)){
_throw( Null_Exception);}* _temp128='f';}); goto _LL90; _LL98:({ struct
_tagged_string _temp129= t; char* _temp131= _temp129.curr +( j ++); if( _temp129.base
== 0? 1:( _temp131 < _temp129.base? 1: _temp131 >= _temp129.last_plus_one)){
_throw( Null_Exception);}* _temp131='\\';});({ struct _tagged_string _temp132= t;
char* _temp134= _temp132.curr +( j ++); if( _temp132.base == 0? 1:( _temp134 <
_temp132.base? 1: _temp134 >= _temp132.last_plus_one)){ _throw( Null_Exception);}*
_temp134='n';}); goto _LL90; _LL100:({ struct _tagged_string _temp135= t; char*
_temp137= _temp135.curr +( j ++); if( _temp135.base == 0? 1:( _temp137 <
_temp135.base? 1: _temp137 >= _temp135.last_plus_one)){ _throw( Null_Exception);}*
_temp137='\\';});({ struct _tagged_string _temp138= t; char* _temp140= _temp138.curr
+( j ++); if( _temp138.base == 0? 1:( _temp140 < _temp138.base? 1: _temp140 >=
_temp138.last_plus_one)){ _throw( Null_Exception);}* _temp140='r';}); goto _LL90;
_LL102:({ struct _tagged_string _temp141= t; char* _temp143= _temp141.curr +( j
++); if( _temp141.base == 0? 1:( _temp143 < _temp141.base? 1: _temp143 >=
_temp141.last_plus_one)){ _throw( Null_Exception);}* _temp143='\\';});({ struct
_tagged_string _temp144= t; char* _temp146= _temp144.curr +( j ++); if( _temp144.base
== 0? 1:( _temp146 < _temp144.base? 1: _temp146 >= _temp144.last_plus_one)){
_throw( Null_Exception);}* _temp146='t';}); goto _LL90; _LL104:({ struct
_tagged_string _temp147= t; char* _temp149= _temp147.curr +( j ++); if( _temp147.base
== 0? 1:( _temp149 < _temp147.base? 1: _temp149 >= _temp147.last_plus_one)){
_throw( Null_Exception);}* _temp149='\\';});({ struct _tagged_string _temp150= t;
char* _temp152= _temp150.curr +( j ++); if( _temp150.base == 0? 1:( _temp152 <
_temp150.base? 1: _temp152 >= _temp150.last_plus_one)){ _throw( Null_Exception);}*
_temp152='v';}); goto _LL90; _LL106:({ struct _tagged_string _temp153= t; char*
_temp155= _temp153.curr +( j ++); if( _temp153.base == 0? 1:( _temp155 <
_temp153.base? 1: _temp155 >= _temp153.last_plus_one)){ _throw( Null_Exception);}*
_temp155='\\';});({ struct _tagged_string _temp156= t; char* _temp158= _temp156.curr
+( j ++); if( _temp156.base == 0? 1:( _temp158 < _temp156.base? 1: _temp158 >=
_temp156.last_plus_one)){ _throw( Null_Exception);}* _temp158='\\';}); goto
_LL90; _LL108:({ struct _tagged_string _temp159= t; char* _temp161= _temp159.curr
+( j ++); if( _temp159.base == 0? 1:( _temp161 < _temp159.base? 1: _temp161 >=
_temp159.last_plus_one)){ _throw( Null_Exception);}* _temp161='\\';});({ struct
_tagged_string _temp162= t; char* _temp164= _temp162.curr +( j ++); if( _temp162.base
== 0? 1:( _temp164 < _temp162.base? 1: _temp164 >= _temp162.last_plus_one)){
_throw( Null_Exception);}* _temp164='"';}); goto _LL90; _LL110: if( _temp89 >=' '?
_temp89 <='~': 0){({ struct _tagged_string _temp165= t; char* _temp167= _temp165.curr
+( j ++); if( _temp165.base == 0? 1:( _temp167 < _temp165.base? 1: _temp167 >=
_temp165.last_plus_one)){ _throw( Null_Exception);}* _temp167= _temp89;});}
else{({ struct _tagged_string _temp168= t; char* _temp170= _temp168.curr +( j ++);
if( _temp168.base == 0? 1:( _temp170 < _temp168.base? 1: _temp170 >= _temp168.last_plus_one)){
_throw( Null_Exception);}* _temp170='\\';});({ struct _tagged_string _temp171= t;
char* _temp173= _temp171.curr +( j ++); if( _temp171.base == 0? 1:( _temp173 <
_temp171.base? 1: _temp173 >= _temp171.last_plus_one)){ _throw( Null_Exception);}*
_temp173=( char)('0' +( _temp89 >> 6 & 7));});({ struct _tagged_string _temp174=
t; char* _temp176= _temp174.curr +( j ++); if( _temp174.base == 0? 1:( _temp176
< _temp174.base? 1: _temp176 >= _temp174.last_plus_one)){ _throw( Null_Exception);}*
_temp176=( char)('0' +( _temp89 >> 3 & 7));});({ struct _tagged_string _temp177=
t; char* _temp179= _temp177.curr +( j ++); if( _temp177.base == 0? 1:( _temp179
< _temp177.base? 1: _temp179 >= _temp177.last_plus_one)){ _throw( Null_Exception);}*
_temp179=( char)('0' +( _temp89 & 7));});} goto _LL90; _LL90:;}} return t;}}}}
static struct Cyc_PP_Doc* Cyc_Absynpp_textptr( struct _tagged_string* s){ return
Cyc_PP_text(* s);} static char _temp183[ 9u]="restrict"; static struct
_tagged_string Cyc_Absynpp_restrict_string=( struct _tagged_string){ _temp183,
_temp183, _temp183 + 9u}; static char _temp186[ 9u]="volatile"; static struct
_tagged_string Cyc_Absynpp_volatile_string=( struct _tagged_string){ _temp186,
_temp186, _temp186 + 9u}; static char _temp189[ 6u]="const"; static struct
_tagged_string Cyc_Absynpp_const_string=( struct _tagged_string){ _temp189,
_temp189, _temp189 + 6u}; static struct _tagged_string* Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string; static struct _tagged_string* Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string; static struct _tagged_string* Cyc_Absynpp_const_sp=&
Cyc_Absynpp_const_string; struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual
tq){ struct Cyc_List_List* l= 0; if( tq.q_restrict){ l=({ struct Cyc_List_List*
_temp190=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp190->hd=( void*) Cyc_Absynpp_restrict_sp; _temp190->tl= l; _temp190;});}
if( tq.q_volatile){ l=({ struct Cyc_List_List* _temp191=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp191->hd=( void*) Cyc_Absynpp_volatile_sp;
_temp191->tl= l; _temp191;});} if( tq.q_const){ l=({ struct Cyc_List_List*
_temp192=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp192->hd=( void*) Cyc_Absynpp_const_sp; _temp192->tl= l; _temp192;});}
return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp193=( char*)"";
struct _tagged_string _temp194; _temp194.curr= _temp193; _temp194.base= _temp193;
_temp194.last_plus_one= _temp193 + 1; _temp194;}),( struct _tagged_string)({
char* _temp195=( char*)" "; struct _tagged_string _temp196; _temp196.curr=
_temp195; _temp196.base= _temp195; _temp196.last_plus_one= _temp195 + 2;
_temp196;}),( struct _tagged_string)({ char* _temp197=( char*)" "; struct
_tagged_string _temp198; _temp198.curr= _temp197; _temp198.base= _temp197;
_temp198.last_plus_one= _temp197 + 2; _temp198;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr, l));} struct _tagged_string Cyc_Absynpp_kind2string( void*
k){ void* _temp199= k; _LL201: if( _temp199 ==( void*) Cyc_Absyn_AnyKind){ goto
_LL202;} else{ goto _LL203;} _LL203: if( _temp199 ==( void*) Cyc_Absyn_MemKind){
goto _LL204;} else{ goto _LL205;} _LL205: if( _temp199 ==( void*) Cyc_Absyn_BoxKind){
goto _LL206;} else{ goto _LL207;} _LL207: if( _temp199 ==( void*) Cyc_Absyn_RgnKind){
goto _LL208;} else{ goto _LL209;} _LL209: if( _temp199 ==( void*) Cyc_Absyn_EffKind){
goto _LL210;} else{ goto _LL200;} _LL202: return( struct _tagged_string)({ char*
_temp211=( char*)"A"; struct _tagged_string _temp212; _temp212.curr= _temp211;
_temp212.base= _temp211; _temp212.last_plus_one= _temp211 + 2; _temp212;});
_LL204: return( struct _tagged_string)({ char* _temp213=( char*)"M"; struct
_tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 2; _temp214;}); _LL206: return( struct
_tagged_string)({ char* _temp215=( char*)"B"; struct _tagged_string _temp216;
_temp216.curr= _temp215; _temp216.base= _temp215; _temp216.last_plus_one=
_temp215 + 2; _temp216;}); _LL208: return( struct _tagged_string)({ char*
_temp217=( char*)"R"; struct _tagged_string _temp218; _temp218.curr= _temp217;
_temp218.base= _temp217; _temp218.last_plus_one= _temp217 + 2; _temp218;});
_LL210: return( struct _tagged_string)({ char* _temp219=( char*)"E"; struct
_tagged_string _temp220; _temp220.curr= _temp219; _temp220.base= _temp219;
_temp220.last_plus_one= _temp219 + 2; _temp220;}); _LL200:;} struct
_tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref* c){ void*
_temp221=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( c))->v; void* _temp227; _LL223: if(( unsigned int)
_temp221 > 1u?*(( int*) _temp221) == Cyc_Absyn_Eq_constr: 0){ _LL228: _temp227=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp221)->f1; goto _LL224;} else{
goto _LL225;} _LL225: goto _LL226; _LL224: return Cyc_Absynpp_kind2string(
_temp227); _LL226: return( struct _tagged_string)({ char* _temp229=( char*)"?";
struct _tagged_string _temp230; _temp230.curr= _temp229; _temp230.base= _temp229;
_temp230.last_plus_one= _temp229 + 2; _temp230;}); _LL222:;} struct Cyc_PP_Doc*
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
return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp231=( char*)"<";
struct _tagged_string _temp232; _temp232.curr= _temp231; _temp232.base= _temp231;
_temp232.last_plus_one= _temp231 + 2; _temp232;}),( struct _tagged_string)({
char* _temp233=( char*)">"; struct _tagged_string _temp234; _temp234.curr=
_temp233; _temp234.base= _temp233; _temp234.last_plus_one= _temp233 + 2;
_temp234;}),( struct _tagged_string)({ char* _temp235=( char*)","; struct
_tagged_string _temp236; _temp236.curr= _temp235; _temp236.base= _temp235;
_temp236.last_plus_one= _temp235 + 2; _temp236;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc,
ts));} struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){
void* _temp237=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp245; void* _temp247;
_LL239: if(( unsigned int) _temp237 > 1u?*(( int*) _temp237) == Cyc_Absyn_Eq_constr:
0){ _LL246: _temp245=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp237)->f1;
if( _temp245 ==( void*) Cyc_Absyn_BoxKind){ goto _LL240;} else{ goto _LL241;}}
else{ goto _LL241;} _LL241: if(( unsigned int) _temp237 > 1u?*(( int*) _temp237)
== Cyc_Absyn_Eq_constr: 0){ _LL248: _temp247=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp237)->f1; goto _LL242;} else{ goto _LL243;} _LL243: goto _LL244; _LL240:
return Cyc_Absynpp_textptr( tv->name); _LL242: return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr(
tv->name), Cyc_PP_text(( struct _tagged_string)({ char* _temp249=( char*)"::";
struct _tagged_string _temp250; _temp250.curr= _temp249; _temp250.base= _temp249;
_temp250.last_plus_one= _temp249 + 3; _temp250;})), Cyc_Absynpp_kind2doc(
_temp247)); _LL244: return Cyc_Absynpp_cat2( Cyc_Absynpp_textptr( tv->name), Cyc_PP_text((
struct _tagged_string)({ char* _temp251=( char*)"/*::?*/"; struct _tagged_string
_temp252; _temp252.curr= _temp251; _temp252.base= _temp251; _temp252.last_plus_one=
_temp251 + 8; _temp252;}))); _LL238:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List* tvs){ return Cyc_Absynpp_egroup(( struct _tagged_string)({
char* _temp253=( char*)"<"; struct _tagged_string _temp254; _temp254.curr=
_temp253; _temp254.base= _temp253; _temp254.last_plus_one= _temp253 + 2;
_temp254;}),( struct _tagged_string)({ char* _temp255=( char*)">"; struct
_tagged_string _temp256; _temp256.curr= _temp255; _temp256.base= _temp255;
_temp256.last_plus_one= _temp255 + 2; _temp256;}),( struct _tagged_string)({
char* _temp257=( char*)","; struct _tagged_string _temp258; _temp258.curr=
_temp257; _temp258.base= _temp257; _temp258.last_plus_one= _temp257 + 2;
_temp258;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_ktvar2doc, tvs));} static
struct _tagged_string* Cyc_Absynpp_get_name( struct Cyc_Absyn_Tvar* tv){ return
tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc( struct Cyc_List_List* tvs){
if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc( tvs);} return
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp259=( char*)"<"; struct
_tagged_string _temp260; _temp260.curr= _temp259; _temp260.base= _temp259;
_temp260.last_plus_one= _temp259 + 2; _temp260;}),( struct _tagged_string)({
char* _temp261=( char*)">"; struct _tagged_string _temp262; _temp262.curr=
_temp261; _temp262.base= _temp261; _temp262.last_plus_one= _temp261 + 2;
_temp262;}),( struct _tagged_string)({ char* _temp263=( char*)","; struct
_tagged_string _temp264; _temp264.curr= _temp263; _temp264.base= _temp263;
_temp264.last_plus_one= _temp263 + 2; _temp264;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr,(( struct Cyc_List_List*(*)( struct _tagged_string*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_get_name,
tvs)));} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; struct Cyc_PP_Doc*
Cyc_Absynpp_arg2doc( struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((* t).f1,(*
t).f2, 0);} struct Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List* ts){
return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp265=( char*)"(";
struct _tagged_string _temp266; _temp266.curr= _temp265; _temp266.base= _temp265;
_temp266.last_plus_one= _temp265 + 2; _temp266;}),( struct _tagged_string)({
char* _temp267=( char*)")"; struct _tagged_string _temp268; _temp268.curr=
_temp267; _temp268.base= _temp267; _temp268.last_plus_one= _temp267 + 2;
_temp268;}),( struct _tagged_string)({ char* _temp269=( char*)","; struct
_tagged_string _temp270; _temp270.curr= _temp269; _temp270.base= _temp269;
_temp270.last_plus_one= _temp269 + 2; _temp270;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc( void* a){
return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc* Cyc_Absynpp_atts2doc(
struct Cyc_List_List* atts){ if( atts == 0){ return Cyc_PP_nil_doc();} return
Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp271=( char*)" __attribute__";
struct _tagged_string _temp272; _temp272.curr= _temp271; _temp272.base= _temp271;
_temp272.last_plus_one= _temp271 + 15; _temp272;})), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp273=( char*)"(("; struct _tagged_string _temp274;
_temp274.curr= _temp273; _temp274.base= _temp273; _temp274.last_plus_one=
_temp273 + 3; _temp274;}),( struct _tagged_string)({ char* _temp275=( char*)"))";
struct _tagged_string _temp276; _temp276.curr= _temp275; _temp276.base= _temp275;
_temp276.last_plus_one= _temp275 + 3; _temp276;}),( struct _tagged_string)({
char* _temp277=( char*)","; struct _tagged_string _temp278; _temp278.curr=
_temp277; _temp278.base= _temp277; _temp278.last_plus_one= _temp277 + 2;
_temp278;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc, atts)));} int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List* tms){ if( tms == 0){ return 0;}{ void* _temp280=( void*)({
struct Cyc_List_List* _temp279= tms; if( _temp279 == 0){ _throw( Null_Exception);}
_temp279->hd;}); struct Cyc_Absyn_Tqual _temp286; void* _temp288; void* _temp290;
_LL282: if(( unsigned int) _temp280 > 1u?*(( int*) _temp280) == Cyc_Absyn_Pointer_mod:
0){ _LL291: _temp290=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp280)->f1;
goto _LL289; _LL289: _temp288=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp280)->f2; goto _LL287; _LL287: _temp286=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp280)->f3; goto _LL283;} else{ goto _LL284;}
_LL284: goto _LL285; _LL283: return 1; _LL285: return 0; _LL281:;}} extern
struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc* d, struct Cyc_List_List* tms){ if( tms == 0){ return d;}{
struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc( d,({ struct Cyc_List_List*
_temp402= tms; if( _temp402 == 0){ _throw( Null_Exception);} _temp402->tl;}));
struct Cyc_PP_Doc* p_rest= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp398=( char*)"("; struct _tagged_string _temp399; _temp399.curr=
_temp398; _temp399.base= _temp398; _temp399.last_plus_one= _temp398 + 2;
_temp399;})), rest, Cyc_PP_text(( struct _tagged_string)({ char* _temp400=( char*)")";
struct _tagged_string _temp401; _temp401.curr= _temp400; _temp401.base= _temp400;
_temp401.last_plus_one= _temp400 + 2; _temp401;}))); void* _temp293=( void*)({
struct Cyc_List_List* _temp292= tms; if( _temp292 == 0){ _throw( Null_Exception);}
_temp292->hd;}); struct Cyc_Absyn_Exp* _temp307; void* _temp309; struct Cyc_List_List*
_temp311; struct Cyc_Position_Segment* _temp313; int _temp315; struct Cyc_Position_Segment*
_temp317; struct Cyc_List_List* _temp319; struct Cyc_Absyn_Tqual _temp321; void*
_temp323; void* _temp325; _LL295: if( _temp293 ==( void*) Cyc_Absyn_Carray_mod){
goto _LL296;} else{ goto _LL297;} _LL297: if(( unsigned int) _temp293 > 1u?*((
int*) _temp293) == Cyc_Absyn_ConstArray_mod: 0){ _LL308: _temp307=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ConstArray_mod_struct*) _temp293)->f1; goto _LL298;} else{ goto
_LL299;} _LL299: if(( unsigned int) _temp293 > 1u?*(( int*) _temp293) == Cyc_Absyn_Function_mod:
0){ _LL310: _temp309=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp293)->f1;
goto _LL300;} else{ goto _LL301;} _LL301: if(( unsigned int) _temp293 > 1u?*((
int*) _temp293) == Cyc_Absyn_Attributes_mod: 0){ _LL314: _temp313=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_Attributes_mod_struct*) _temp293)->f1; goto _LL312; _LL312:
_temp311=( struct Cyc_List_List*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp293)->f2; goto _LL302;} else{ goto _LL303;} _LL303: if(( unsigned int)
_temp293 > 1u?*(( int*) _temp293) == Cyc_Absyn_TypeParams_mod: 0){ _LL320:
_temp319=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp293)->f1; goto _LL318; _LL318: _temp317=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp293)->f2; goto _LL316; _LL316:
_temp315=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp293)->f3; goto
_LL304;} else{ goto _LL305;} _LL305: if(( unsigned int) _temp293 > 1u?*(( int*)
_temp293) == Cyc_Absyn_Pointer_mod: 0){ _LL326: _temp325=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp293)->f1; goto _LL324; _LL324: _temp323=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp293)->f2; goto _LL322; _LL322: _temp321=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp293)->f3; goto _LL306;} else{ goto _LL294;}
_LL296: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List* _temp327= tms;
if( _temp327 == 0){ _throw( Null_Exception);} _temp327->tl;}))){ rest= p_rest;}
return Cyc_Absynpp_cat2( rest, Cyc_PP_text(( struct _tagged_string)({ char*
_temp328=( char*)"[]"; struct _tagged_string _temp329; _temp329.curr= _temp328;
_temp329.base= _temp328; _temp329.last_plus_one= _temp328 + 3; _temp329;})));
_LL298: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List* _temp330= tms;
if( _temp330 == 0){ _throw( Null_Exception);} _temp330->tl;}))){ rest= p_rest;}
return Cyc_Absynpp_cat4( rest, Cyc_PP_text(( struct _tagged_string)({ char*
_temp331=( char*)"["; struct _tagged_string _temp332; _temp332.curr= _temp331;
_temp332.base= _temp331; _temp332.last_plus_one= _temp331 + 2; _temp332;})), Cyc_Absynpp_exp2doc(
_temp307), Cyc_PP_text(( struct _tagged_string)({ char* _temp333=( char*)"]";
struct _tagged_string _temp334; _temp334.curr= _temp333; _temp334.base= _temp333;
_temp334.last_plus_one= _temp333 + 2; _temp334;}))); _LL300: if( Cyc_Absynpp_next_is_pointer(({
struct Cyc_List_List* _temp335= tms; if( _temp335 == 0){ _throw( Null_Exception);}
_temp335->tl;}))){ rest= p_rest;}{ void* _temp336= _temp309; struct Cyc_Core_Opt*
_temp342; int _temp344; struct Cyc_List_List* _temp346; struct Cyc_Position_Segment*
_temp348; struct Cyc_List_List* _temp350; _LL338: if(*(( int*) _temp336) == Cyc_Absyn_WithTypes){
_LL347: _temp346=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp336)->f1; goto _LL345; _LL345: _temp344=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp336)->f2; goto _LL343; _LL343: _temp342=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp336)->f3; goto _LL339;} else{ goto _LL340;}
_LL340: if(*(( int*) _temp336) == Cyc_Absyn_NoTypes){ _LL351: _temp350=( struct
Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp336)->f1; goto _LL349;
_LL349: _temp348=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp336)->f2; goto _LL341;} else{ goto _LL337;} _LL339: return Cyc_Absynpp_cat2(
rest, Cyc_Absynpp_funargs2doc( _temp346, _temp344, _temp342)); _LL341: return
Cyc_Absynpp_cat2( rest, Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp352=( char*)"("; struct _tagged_string _temp353; _temp353.curr= _temp352;
_temp353.base= _temp352; _temp353.last_plus_one= _temp352 + 2; _temp353;}),(
struct _tagged_string)({ char* _temp354=( char*)")"; struct _tagged_string
_temp355; _temp355.curr= _temp354; _temp355.base= _temp354; _temp355.last_plus_one=
_temp354 + 2; _temp355;}),( struct _tagged_string)({ char* _temp356=( char*)",";
struct _tagged_string _temp357; _temp357.curr= _temp356; _temp357.base= _temp356;
_temp357.last_plus_one= _temp356 + 2; _temp357;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr, _temp350))); _LL337:;} _LL302: if( Cyc_Absynpp_next_is_pointer(({
struct Cyc_List_List* _temp358= tms; if( _temp358 == 0){ _throw( Null_Exception);}
_temp358->tl;}))){ rest= p_rest;} return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_atts2doc(
_temp311)); _LL304: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List*
_temp359= tms; if( _temp359 == 0){ _throw( Null_Exception);} _temp359->tl;}))){
rest= p_rest;} if( _temp315){ return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_ktvars2doc(
_temp319));} else{ return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_tvars2doc(
_temp319));} _LL306: { struct Cyc_PP_Doc* ptr;{ void* _temp360= _temp325; struct
Cyc_Absyn_Exp* _temp368; struct Cyc_Absyn_Exp* _temp370; _LL362: if((
unsigned int) _temp360 > 1u?*(( int*) _temp360) == Cyc_Absyn_Nullable_ps: 0){
_LL369: _temp368=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp360)->f1; goto _LL363;} else{ goto _LL364;} _LL364: if(( unsigned int)
_temp360 > 1u?*(( int*) _temp360) == Cyc_Absyn_NonNullable_ps: 0){ _LL371:
_temp370=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp360)->f1; goto _LL365;} else{ goto _LL366;} _LL366: if( _temp360 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL367;} else{ goto _LL361;} _LL363: if( Cyc_Evexp_eval_const_uint_exp(
_temp368) == 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char* _temp372=(
char*)"*"; struct _tagged_string _temp373; _temp373.curr= _temp372; _temp373.base=
_temp372; _temp373.last_plus_one= _temp372 + 2; _temp373;}));} else{ ptr= Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp374=( char*)"*"; struct
_tagged_string _temp375; _temp375.curr= _temp374; _temp375.base= _temp374;
_temp375.last_plus_one= _temp374 + 2; _temp375;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp376=( char*)"{"; struct _tagged_string _temp377;
_temp377.curr= _temp376; _temp377.base= _temp376; _temp377.last_plus_one=
_temp376 + 2; _temp377;})), Cyc_Absynpp_exp2doc( _temp368), Cyc_PP_text(( struct
_tagged_string)({ char* _temp378=( char*)"}"; struct _tagged_string _temp379;
_temp379.curr= _temp378; _temp379.base= _temp378; _temp379.last_plus_one=
_temp378 + 2; _temp379;})));} goto _LL361; _LL365: if( Cyc_Evexp_eval_const_uint_exp(
_temp370) == 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char* _temp380=(
char*)"@"; struct _tagged_string _temp381; _temp381.curr= _temp380; _temp381.base=
_temp380; _temp381.last_plus_one= _temp380 + 2; _temp381;}));} else{ ptr= Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp382=( char*)"@"; struct
_tagged_string _temp383; _temp383.curr= _temp382; _temp383.base= _temp382;
_temp383.last_plus_one= _temp382 + 2; _temp383;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp384=( char*)"{"; struct _tagged_string _temp385;
_temp385.curr= _temp384; _temp385.base= _temp384; _temp385.last_plus_one=
_temp384 + 2; _temp385;})), Cyc_Absynpp_exp2doc( _temp370), Cyc_PP_text(( struct
_tagged_string)({ char* _temp386=( char*)"}"; struct _tagged_string _temp387;
_temp387.curr= _temp386; _temp387.base= _temp386; _temp387.last_plus_one=
_temp386 + 2; _temp387;})));} goto _LL361; _LL367: ptr= Cyc_PP_text(( struct
_tagged_string)({ char* _temp388=( char*)"?"; struct _tagged_string _temp389;
_temp389.curr= _temp388; _temp389.base= _temp388; _temp389.last_plus_one=
_temp388 + 2; _temp389;})); goto _LL361; _LL361:;}{ void* _temp390= Cyc_Tcutil_compress(
_temp323); _LL392: if( _temp390 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL393;}
else{ goto _LL394;} _LL394: goto _LL395; _LL393: return Cyc_Absynpp_cat2( ptr,
rest); _LL395: return Cyc_Absynpp_cat4( ptr, Cyc_Absynpp_ntyp2doc( _temp323),
Cyc_PP_text(( struct _tagged_string)({ char* _temp396=( char*)" "; struct
_tagged_string _temp397; _temp397.curr= _temp396; _temp397.base= _temp396;
_temp397.last_plus_one= _temp396 + 2; _temp397;})), rest); _LL391:;}} _LL294:;}}
struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){ void* _temp403= Cyc_Tcutil_compress(
t); _LL405: if( _temp403 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL406;} else{ goto
_LL407;} _LL407: goto _LL408; _LL406: return Cyc_PP_text(( struct _tagged_string)({
char* _temp409=( char*)"`H"; struct _tagged_string _temp410; _temp410.curr=
_temp409; _temp410.base= _temp409; _temp410.last_plus_one= _temp409 + 3;
_temp410;})); _LL408: return Cyc_Absynpp_ntyp2doc( t); _LL404:;} static void Cyc_Absynpp_effects2docs(
struct Cyc_List_List** regions, struct Cyc_List_List** effects, void* t){ void*
_temp411= Cyc_Tcutil_compress( t); void* _temp419; struct Cyc_List_List*
_temp421; _LL413: if(( unsigned int) _temp411 > 4u?*(( int*) _temp411) == Cyc_Absyn_AccessEff:
0){ _LL420: _temp419=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp411)->f1;
goto _LL414;} else{ goto _LL415;} _LL415: if(( unsigned int) _temp411 > 4u?*((
int*) _temp411) == Cyc_Absyn_JoinEff: 0){ _LL422: _temp421=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp411)->f1; goto _LL416;} else{ goto _LL417;}
_LL417: goto _LL418; _LL414:* regions=({ struct Cyc_List_List* _temp423=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp423->hd=( void*)
Cyc_Absynpp_rgn2doc( _temp419); _temp423->tl=* regions; _temp423;}); goto _LL412;
_LL416: for( 0; _temp421 != 0; _temp421=({ struct Cyc_List_List* _temp424=
_temp421; if( _temp424 == 0){ _throw( Null_Exception);} _temp424->tl;})){ Cyc_Absynpp_effects2docs(
regions, effects,( void*)({ struct Cyc_List_List* _temp425= _temp421; if(
_temp425 == 0){ _throw( Null_Exception);} _temp425->hd;}));} goto _LL412; _LL418:*
effects=({ struct Cyc_List_List* _temp426=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp426->hd=( void*) Cyc_Absynpp_ntyp2doc( t);
_temp426->tl=* effects; _temp426;}); goto _LL412; _LL412:;} struct Cyc_PP_Doc*
Cyc_Absynpp_eff2doc( void* t){ struct Cyc_List_List* regions= 0; struct Cyc_List_List*
effects= 0; Cyc_Absynpp_effects2docs(& regions,& effects, t); regions=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( regions);
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effects); if( regions == 0? effects != 0: 0){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp427=( char*)""; struct _tagged_string _temp428;
_temp428.curr= _temp427; _temp428.base= _temp427; _temp428.last_plus_one=
_temp427 + 1; _temp428;}),( struct _tagged_string)({ char* _temp429=( char*)"";
struct _tagged_string _temp430; _temp430.curr= _temp429; _temp430.base= _temp429;
_temp430.last_plus_one= _temp429 + 1; _temp430;}),( struct _tagged_string)({
char* _temp431=( char*)"+"; struct _tagged_string _temp432; _temp432.curr=
_temp431; _temp432.base= _temp431; _temp432.last_plus_one= _temp431 + 2;
_temp432;}), effects);} else{ struct Cyc_PP_Doc* _temp439= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp433=( char*)"{"; struct _tagged_string
_temp434; _temp434.curr= _temp433; _temp434.base= _temp433; _temp434.last_plus_one=
_temp433 + 2; _temp434;}),( struct _tagged_string)({ char* _temp435=( char*)"}";
struct _tagged_string _temp436; _temp436.curr= _temp435; _temp436.base= _temp435;
_temp436.last_plus_one= _temp435 + 2; _temp436;}),( struct _tagged_string)({
char* _temp437=( char*)","; struct _tagged_string _temp438; _temp438.curr=
_temp437; _temp438.base= _temp437; _temp438.last_plus_one= _temp437 + 2;
_temp438;}), regions); goto _LL440; _LL440: return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp441=( char*)""; struct _tagged_string _temp442;
_temp442.curr= _temp441; _temp442.base= _temp441; _temp442.last_plus_one=
_temp441 + 1; _temp442;}),( struct _tagged_string)({ char* _temp443=( char*)"";
struct _tagged_string _temp444; _temp444.curr= _temp443; _temp444.base= _temp443;
_temp444.last_plus_one= _temp443 + 1; _temp444;}),( struct _tagged_string)({
char* _temp445=( char*)"+"; struct _tagged_string _temp446; _temp446.curr=
_temp445; _temp446.base= _temp445; _temp446.last_plus_one= _temp445 + 2;
_temp446;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( effects,({ struct Cyc_List_List* _temp447=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp447->hd=( void*) _temp439;
_temp447->tl= 0; _temp447;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void*
t){ struct Cyc_PP_Doc* s;{ void* _temp448= t; struct Cyc_Absyn_Exp* _temp494;
struct Cyc_Absyn_Tqual _temp496; void* _temp498; struct Cyc_Absyn_FnInfo
_temp500; struct Cyc_Absyn_PtrInfo _temp502; int _temp504; struct Cyc_Core_Opt*
_temp506; void* _temp508; struct Cyc_Absyn_Tvar* _temp510; struct Cyc_Absyn_TunionInfo
_temp512; void* _temp514; struct Cyc_List_List* _temp516; void* _temp518; struct
Cyc_Absyn_TunionFieldInfo _temp520; struct Cyc_List_List* _temp522; void*
_temp524; void* _temp526; void* _temp528; struct Cyc_List_List* _temp530; struct
Cyc_Absyn_Structdecl** _temp532; struct Cyc_List_List* _temp534; struct _tuple0*
_temp536; struct Cyc_Absyn_Uniondecl** _temp538; struct Cyc_List_List* _temp540;
struct _tuple0* _temp542; struct Cyc_List_List* _temp544; struct Cyc_List_List*
_temp546; struct Cyc_Absyn_Enumdecl* _temp548; struct _tuple0* _temp550; struct
Cyc_Core_Opt* _temp552; struct Cyc_List_List* _temp554; struct _tuple0* _temp556;
void* _temp558; void* _temp560; struct Cyc_List_List* _temp562; _LL450: if((
unsigned int) _temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_ArrayType: 0){
_LL499: _temp498=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp448)->f1;
goto _LL497; _LL497: _temp496=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp448)->f2; goto _LL495; _LL495: _temp494=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp448)->f3; goto _LL451;} else{ goto _LL452;}
_LL452: if(( unsigned int) _temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_FnType:
0){ _LL501: _temp500=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp448)->f1; goto _LL453;} else{ goto _LL454;} _LL454: if(( unsigned int)
_temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_PointerType: 0){ _LL503: _temp502=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp448)->f1;
goto _LL455;} else{ goto _LL456;} _LL456: if( _temp448 ==( void*) Cyc_Absyn_VoidType){
goto _LL457;} else{ goto _LL458;} _LL458: if(( unsigned int) _temp448 > 4u?*((
int*) _temp448) == Cyc_Absyn_Evar: 0){ _LL509: _temp508=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp448)->f1; goto _LL507; _LL507: _temp506=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp448)->f2; goto _LL505; _LL505: _temp504=( int)((
struct Cyc_Absyn_Evar_struct*) _temp448)->f3; goto _LL459;} else{ goto _LL460;}
_LL460: if(( unsigned int) _temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_VarType:
0){ _LL511: _temp510=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp448)->f1; goto _LL461;} else{ goto _LL462;} _LL462: if(( unsigned int)
_temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_TunionType: 0){ _LL513: _temp512=(
struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*) _temp448)->f1;
_LL519: _temp518=( void*) _temp512.tunion_info; goto _LL517; _LL517: _temp516=(
struct Cyc_List_List*) _temp512.targs; goto _LL515; _LL515: _temp514=( void*)
_temp512.rgn; goto _LL463;} else{ goto _LL464;} _LL464: if(( unsigned int)
_temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_TunionFieldType: 0){ _LL521:
_temp520=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp448)->f1; _LL525: _temp524=( void*) _temp520.field_info; goto _LL523;
_LL523: _temp522=( struct Cyc_List_List*) _temp520.targs; goto _LL465;} else{
goto _LL466;} _LL466: if(( unsigned int) _temp448 > 4u?*(( int*) _temp448) ==
Cyc_Absyn_IntType: 0){ _LL529: _temp528=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp448)->f1; goto _LL527; _LL527: _temp526=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp448)->f2; goto _LL467;} else{ goto _LL468;} _LL468: if( _temp448 ==( void*)
Cyc_Absyn_FloatType){ goto _LL469;} else{ goto _LL470;} _LL470: if( _temp448 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL471;} else{ goto _LL472;} _LL472: if((
unsigned int) _temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_TupleType: 0){
_LL531: _temp530=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp448)->f1; goto _LL473;} else{ goto _LL474;} _LL474: if(( unsigned int)
_temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_StructType: 0){ _LL537: _temp536=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp448)->f1; goto
_LL535; _LL535: _temp534=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp448)->f2; goto _LL533; _LL533: _temp532=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp448)->f3; goto _LL475;} else{ goto
_LL476;} _LL476: if(( unsigned int) _temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_UnionType:
0){ _LL543: _temp542=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp448)->f1; goto _LL541; _LL541: _temp540=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp448)->f2; goto _LL539; _LL539: _temp538=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp448)->f3;
goto _LL477;} else{ goto _LL478;} _LL478: if(( unsigned int) _temp448 > 4u?*((
int*) _temp448) == Cyc_Absyn_AnonStructType: 0){ _LL545: _temp544=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp448)->f1; goto _LL479;} else{ goto
_LL480;} _LL480: if(( unsigned int) _temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_AnonUnionType:
0){ _LL547: _temp546=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp448)->f1; goto _LL481;} else{ goto _LL482;} _LL482: if(( unsigned int)
_temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_EnumType: 0){ _LL551: _temp550=(
struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp448)->f1; goto _LL549;
_LL549: _temp548=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp448)->f2; goto _LL483;} else{ goto _LL484;} _LL484: if(( unsigned int)
_temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_TypedefType: 0){ _LL557: _temp556=(
struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp448)->f1; goto
_LL555; _LL555: _temp554=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp448)->f2; goto _LL553; _LL553: _temp552=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp448)->f3; goto _LL485;} else{ goto _LL486;}
_LL486: if(( unsigned int) _temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_RgnHandleType:
0){ _LL559: _temp558=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp448)->f1;
goto _LL487;} else{ goto _LL488;} _LL488: if( _temp448 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL489;} else{ goto _LL490;} _LL490: if(( unsigned int) _temp448 > 4u?*((
int*) _temp448) == Cyc_Absyn_AccessEff: 0){ _LL561: _temp560=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp448)->f1; goto _LL491;} else{ goto _LL492;}
_LL492: if(( unsigned int) _temp448 > 4u?*(( int*) _temp448) == Cyc_Absyn_JoinEff:
0){ _LL563: _temp562=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp448)->f1; goto _LL493;} else{ goto _LL449;} _LL451: return Cyc_PP_text((
struct _tagged_string)({ char* _temp564=( char*)"[[[array]]]"; struct
_tagged_string _temp565; _temp565.curr= _temp564; _temp565.base= _temp564;
_temp565.last_plus_one= _temp564 + 12; _temp565;})); _LL453: return Cyc_PP_nil_doc();
_LL455: return Cyc_PP_nil_doc(); _LL457: s= Cyc_PP_text(( struct _tagged_string)({
char* _temp566=( char*)"void"; struct _tagged_string _temp567; _temp567.curr=
_temp566; _temp567.base= _temp566; _temp567.last_plus_one= _temp566 + 5;
_temp567;})); goto _LL449; _LL459: if( _temp506 != 0){ return Cyc_Absynpp_ntyp2doc((
void*)({ struct Cyc_Core_Opt* _temp568= _temp506; if( _temp568 == 0){ _throw(
Null_Exception);} _temp568->v;}));} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp569=( char*)"%"; struct _tagged_string
_temp570; _temp570.curr= _temp569; _temp570.base= _temp569; _temp570.last_plus_one=
_temp569 + 2; _temp570;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp571=( char*)"("; struct _tagged_string _temp572; _temp572.curr= _temp571;
_temp572.base= _temp571; _temp572.last_plus_one= _temp571 + 2; _temp572;})), Cyc_PP_text(
xprintf("%d", _temp504)), Cyc_PP_text(( struct _tagged_string)({ char* _temp573=(
char*)")::"; struct _tagged_string _temp574; _temp574.curr= _temp573; _temp574.base=
_temp573; _temp574.last_plus_one= _temp573 + 4; _temp574;})), Cyc_Absynpp_kind2doc(
_temp508));} goto _LL449; _LL461: s= Cyc_Absynpp_textptr( _temp510->name); if(
Cyc_Absynpp_print_all_kinds){ s= Cyc_Absynpp_cat3( s, Cyc_PP_text(( struct
_tagged_string)({ char* _temp575=( char*)"::"; struct _tagged_string _temp576;
_temp576.curr= _temp575; _temp576.base= _temp575; _temp576.last_plus_one=
_temp575 + 3; _temp576;})), Cyc_Absynpp_ckind2doc( _temp510->kind));} if( ! Cyc_Absynpp_dont_rewrite_temp_tvars?
Cyc_Tcutil_is_temp_tvar( _temp510): 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp577=( char*)"_ /* "; struct _tagged_string
_temp578; _temp578.curr= _temp577; _temp578.base= _temp577; _temp578.last_plus_one=
_temp577 + 6; _temp578;})), s, Cyc_PP_text(( struct _tagged_string)({ char*
_temp579=( char*)" */"; struct _tagged_string _temp580; _temp580.curr= _temp579;
_temp580.base= _temp579; _temp580.last_plus_one= _temp579 + 4; _temp580;})));}
goto _LL449; _LL463:{ void* _temp581= _temp518; struct Cyc_Absyn_UnknownTunionInfo
_temp587; int _temp589; struct _tuple0* _temp591; struct Cyc_Absyn_Tuniondecl*
_temp593; struct Cyc_Absyn_Tuniondecl _temp595; int _temp596; struct Cyc_Core_Opt*
_temp598; struct Cyc_List_List* _temp600; struct _tuple0* _temp602; void*
_temp604; _LL583: if(*(( int*) _temp581) == Cyc_Absyn_UnknownTunion){ _LL588:
_temp587=( struct Cyc_Absyn_UnknownTunionInfo)(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp581)->f1; _LL592: _temp591=( struct _tuple0*) _temp587.name; goto _LL590;
_LL590: _temp589=( int) _temp587.is_xtunion; goto _LL584;} else{ goto _LL585;}
_LL585: if(*(( int*) _temp581) == Cyc_Absyn_KnownTunion){ _LL594: _temp593=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*) _temp581)->f1;
_temp595=* _temp593; _LL605: _temp604=( void*) _temp595.sc; goto _LL603; _LL603:
_temp602=( struct _tuple0*) _temp595.name; goto _LL601; _LL601: _temp600=(
struct Cyc_List_List*) _temp595.tvs; goto _LL599; _LL599: _temp598=( struct Cyc_Core_Opt*)
_temp595.fields; goto _LL597; _LL597: _temp596=( int) _temp595.is_xtunion; goto
_LL586;} else{ goto _LL582;} _LL584: _temp602= _temp591; _temp596= _temp589;
goto _LL586; _LL586: { struct Cyc_PP_Doc* _temp610= Cyc_PP_text( _temp596?(
struct _tagged_string)({ char* _temp606=( char*)"xtunion "; struct
_tagged_string _temp607; _temp607.curr= _temp606; _temp607.base= _temp606;
_temp607.last_plus_one= _temp606 + 9; _temp607;}):( struct _tagged_string)({
char* _temp608=( char*)"tunion "; struct _tagged_string _temp609; _temp609.curr=
_temp608; _temp609.base= _temp608; _temp609.last_plus_one= _temp608 + 8;
_temp609;})); goto _LL611; _LL611:{ void* _temp612= Cyc_Tcutil_compress(
_temp514); _LL614: if( _temp612 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL615;}
else{ goto _LL616;} _LL616: goto _LL617; _LL615: s= Cyc_Absynpp_cat3( _temp610,
Cyc_Absynpp_qvar2doc( _temp602), Cyc_Absynpp_tps2doc( _temp516)); goto _LL613;
_LL617: s= Cyc_Absynpp_cat5( _temp610, Cyc_Absynpp_typ2doc( _temp514), Cyc_PP_text((
struct _tagged_string)({ char* _temp618=( char*)" "; struct _tagged_string
_temp619; _temp619.curr= _temp618; _temp619.base= _temp618; _temp619.last_plus_one=
_temp618 + 2; _temp619;})), Cyc_Absynpp_qvar2doc( _temp602), Cyc_Absynpp_tps2doc(
_temp516)); goto _LL613; _LL613:;} goto _LL582;} _LL582:;} goto _LL449; _LL465:{
void* _temp620= _temp524; struct Cyc_Absyn_UnknownTunionFieldInfo _temp626; int
_temp628; struct _tuple0* _temp630; struct _tuple0* _temp632; struct Cyc_Absyn_Tunionfield*
_temp634; struct Cyc_Absyn_Tunionfield _temp636; void* _temp637; struct Cyc_Position_Segment*
_temp639; struct Cyc_List_List* _temp641; struct Cyc_List_List* _temp643; struct
_tuple0* _temp645; struct Cyc_Absyn_Tuniondecl* _temp647; struct Cyc_Absyn_Tuniondecl
_temp649; int _temp650; struct Cyc_Core_Opt* _temp652; struct Cyc_List_List*
_temp654; struct _tuple0* _temp656; void* _temp658; _LL622: if(*(( int*)
_temp620) == Cyc_Absyn_UnknownTunionfield){ _LL627: _temp626=( struct Cyc_Absyn_UnknownTunionFieldInfo)((
struct Cyc_Absyn_UnknownTunionfield_struct*) _temp620)->f1; _LL633: _temp632=(
struct _tuple0*) _temp626.tunion_name; goto _LL631; _LL631: _temp630=( struct
_tuple0*) _temp626.field_name; goto _LL629; _LL629: _temp628=( int) _temp626.is_xtunion;
goto _LL623;} else{ goto _LL624;} _LL624: if(*(( int*) _temp620) == Cyc_Absyn_KnownTunionfield){
_LL648: _temp647=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp620)->f1; _temp649=* _temp647; _LL659: _temp658=( void*) _temp649.sc; goto
_LL657; _LL657: _temp656=( struct _tuple0*) _temp649.name; goto _LL655; _LL655:
_temp654=( struct Cyc_List_List*) _temp649.tvs; goto _LL653; _LL653: _temp652=(
struct Cyc_Core_Opt*) _temp649.fields; goto _LL651; _LL651: _temp650=( int)
_temp649.is_xtunion; goto _LL635; _LL635: _temp634=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp620)->f2; _temp636=* _temp634;
_LL646: _temp645=( struct _tuple0*) _temp636.name; goto _LL644; _LL644: _temp643=(
struct Cyc_List_List*) _temp636.tvs; goto _LL642; _LL642: _temp641=( struct Cyc_List_List*)
_temp636.typs; goto _LL640; _LL640: _temp639=( struct Cyc_Position_Segment*)
_temp636.loc; goto _LL638; _LL638: _temp637=( void*) _temp636.sc; goto _LL625;}
else{ goto _LL621;} _LL623: _temp656= _temp632; _temp650= _temp628; _temp645=
_temp630; goto _LL625; _LL625: { struct Cyc_PP_Doc* _temp664= Cyc_PP_text(
_temp650?( struct _tagged_string)({ char* _temp660=( char*)"xtunion "; struct
_tagged_string _temp661; _temp661.curr= _temp660; _temp661.base= _temp660;
_temp661.last_plus_one= _temp660 + 9; _temp661;}):( struct _tagged_string)({
char* _temp662=( char*)"tunion "; struct _tagged_string _temp663; _temp663.curr=
_temp662; _temp663.base= _temp662; _temp663.last_plus_one= _temp662 + 8;
_temp663;})); goto _LL665; _LL665: s= Cyc_Absynpp_cat4( _temp664, Cyc_Absynpp_qvar2doc(
_temp656), Cyc_PP_text(( struct _tagged_string)({ char* _temp666=( char*)".";
struct _tagged_string _temp667; _temp667.curr= _temp666; _temp667.base= _temp666;
_temp667.last_plus_one= _temp666 + 2; _temp667;})), Cyc_Absynpp_textptr((*
_temp645).f2)); goto _LL621;} _LL621:;} goto _LL449; _LL467: { struct
_tagged_string sns; struct _tagged_string ts;{ void* _temp668= _temp528; _LL670:
if( _temp668 ==( void*) Cyc_Absyn_Signed){ goto _LL671;} else{ goto _LL672;}
_LL672: if( _temp668 ==( void*) Cyc_Absyn_Unsigned){ goto _LL673;} else{ goto
_LL669;} _LL671: sns=( struct _tagged_string)({ char* _temp674=( char*)"";
struct _tagged_string _temp675; _temp675.curr= _temp674; _temp675.base= _temp674;
_temp675.last_plus_one= _temp674 + 1; _temp675;}); goto _LL669; _LL673: sns=(
struct _tagged_string)({ char* _temp676=( char*)"unsigned "; struct
_tagged_string _temp677; _temp677.curr= _temp676; _temp677.base= _temp676;
_temp677.last_plus_one= _temp676 + 10; _temp677;}); goto _LL669; _LL669:;}{ void*
_temp678= _temp526; _LL680: if( _temp678 ==( void*) Cyc_Absyn_B1){ goto _LL681;}
else{ goto _LL682;} _LL682: if( _temp678 ==( void*) Cyc_Absyn_B2){ goto _LL683;}
else{ goto _LL684;} _LL684: if( _temp678 ==( void*) Cyc_Absyn_B4){ goto _LL685;}
else{ goto _LL686;} _LL686: if( _temp678 ==( void*) Cyc_Absyn_B8){ goto _LL687;}
else{ goto _LL679;} _LL681:{ void* _temp688= _temp528; _LL690: if( _temp688 ==(
void*) Cyc_Absyn_Signed){ goto _LL691;} else{ goto _LL692;} _LL692: if( _temp688
==( void*) Cyc_Absyn_Unsigned){ goto _LL693;} else{ goto _LL689;} _LL691: sns=(
struct _tagged_string)({ char* _temp694=( char*)"signed "; struct _tagged_string
_temp695; _temp695.curr= _temp694; _temp695.base= _temp694; _temp695.last_plus_one=
_temp694 + 8; _temp695;}); goto _LL689; _LL693: sns=( struct _tagged_string)({
char* _temp696=( char*)""; struct _tagged_string _temp697; _temp697.curr=
_temp696; _temp697.base= _temp696; _temp697.last_plus_one= _temp696 + 1;
_temp697;}); goto _LL689; _LL689:;} ts=( struct _tagged_string)({ char* _temp698=(
char*)"char"; struct _tagged_string _temp699; _temp699.curr= _temp698; _temp699.base=
_temp698; _temp699.last_plus_one= _temp698 + 5; _temp699;}); goto _LL679; _LL683:
ts=( struct _tagged_string)({ char* _temp700=( char*)"short"; struct
_tagged_string _temp701; _temp701.curr= _temp700; _temp701.base= _temp700;
_temp701.last_plus_one= _temp700 + 6; _temp701;}); goto _LL679; _LL685: ts=(
struct _tagged_string)({ char* _temp702=( char*)"int"; struct _tagged_string
_temp703; _temp703.curr= _temp702; _temp703.base= _temp702; _temp703.last_plus_one=
_temp702 + 4; _temp703;}); goto _LL679; _LL687: ts=( struct _tagged_string)({
char* _temp704=( char*)"long long"; struct _tagged_string _temp705; _temp705.curr=
_temp704; _temp705.base= _temp704; _temp705.last_plus_one= _temp704 + 10;
_temp705;}); goto _LL679; _LL679:;} s= Cyc_PP_text(({ struct _tagged_string
_temp706= sns; struct _tagged_string _temp707= ts; xprintf("%.*s%.*s", _temp706.last_plus_one
- _temp706.curr, _temp706.curr, _temp707.last_plus_one - _temp707.curr, _temp707.curr);}));
goto _LL449;} _LL469: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp708=(
char*)"float"; struct _tagged_string _temp709; _temp709.curr= _temp708; _temp709.base=
_temp708; _temp709.last_plus_one= _temp708 + 6; _temp709;})); goto _LL449;
_LL471: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp710=( char*)"double";
struct _tagged_string _temp711; _temp711.curr= _temp710; _temp711.base= _temp710;
_temp711.last_plus_one= _temp710 + 7; _temp711;})); goto _LL449; _LL473: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp712=( char*)"$"; struct
_tagged_string _temp713; _temp713.curr= _temp712; _temp713.base= _temp712;
_temp713.last_plus_one= _temp712 + 2; _temp713;})), Cyc_Absynpp_args2doc(
_temp530)); goto _LL449; _LL475: if( _temp536 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp714=( char*)"struct "; struct _tagged_string
_temp715; _temp715.curr= _temp714; _temp715.base= _temp714; _temp715.last_plus_one=
_temp714 + 8; _temp715;})), Cyc_Absynpp_tps2doc( _temp534));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp716=( char*)"struct "; struct
_tagged_string _temp717; _temp717.curr= _temp716; _temp717.base= _temp716;
_temp717.last_plus_one= _temp716 + 8; _temp717;})), _temp536 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp718= _temp536;
if( _temp718 == 0){ _throw( Null_Exception);} _temp718;})), Cyc_Absynpp_tps2doc(
_temp534));} goto _LL449; _LL477: if( _temp542 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp719=( char*)"union "; struct _tagged_string
_temp720; _temp720.curr= _temp719; _temp720.base= _temp719; _temp720.last_plus_one=
_temp719 + 7; _temp720;})), Cyc_Absynpp_tps2doc( _temp540));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp721=( char*)"union "; struct
_tagged_string _temp722; _temp722.curr= _temp721; _temp722.base= _temp721;
_temp722.last_plus_one= _temp721 + 7; _temp722;})), _temp542 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp723= _temp542;
if( _temp723 == 0){ _throw( Null_Exception);} _temp723;})), Cyc_Absynpp_tps2doc(
_temp540));} goto _LL449; _LL479: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp724=( char*)"struct {"; struct _tagged_string
_temp725; _temp725.curr= _temp724; _temp725.base= _temp724; _temp725.last_plus_one=
_temp724 + 9; _temp725;})), Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp544)), Cyc_PP_text(( struct _tagged_string)({ char* _temp726=( char*)"}";
struct _tagged_string _temp727; _temp727.curr= _temp726; _temp727.base= _temp726;
_temp727.last_plus_one= _temp726 + 2; _temp727;}))); goto _LL449; _LL481: s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp728=( char*)"union {"; struct
_tagged_string _temp729; _temp729.curr= _temp728; _temp729.base= _temp728;
_temp729.last_plus_one= _temp728 + 8; _temp729;})), Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp546)), Cyc_PP_text(( struct _tagged_string)({ char* _temp730=( char*)"}";
struct _tagged_string _temp731; _temp731.curr= _temp730; _temp731.base= _temp730;
_temp731.last_plus_one= _temp730 + 2; _temp731;}))); goto _LL449; _LL483: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp732=( char*)"enum "; struct
_tagged_string _temp733; _temp733.curr= _temp732; _temp733.base= _temp732;
_temp733.last_plus_one= _temp732 + 6; _temp733;})), Cyc_Absynpp_qvar2doc(
_temp550)); goto _LL449; _LL485: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp556), Cyc_Absynpp_tps2doc( _temp554)); goto _LL449; _LL487: s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp734=( char*)"region_t<";
struct _tagged_string _temp735; _temp735.curr= _temp734; _temp735.base= _temp734;
_temp735.last_plus_one= _temp734 + 10; _temp735;})), Cyc_Absynpp_rgn2doc(
_temp558), Cyc_PP_text(( struct _tagged_string)({ char* _temp736=( char*)">";
struct _tagged_string _temp737; _temp737.curr= _temp736; _temp737.base= _temp736;
_temp737.last_plus_one= _temp736 + 2; _temp737;}))); goto _LL449; _LL489: s= Cyc_Absynpp_rgn2doc(
t); goto _LL449; _LL491: s= Cyc_Absynpp_eff2doc( t); goto _LL449; _LL493: s= Cyc_Absynpp_eff2doc(
t); goto _LL449; _LL449:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp738= vo; if( _temp738 == 0){
_throw( Null_Exception);} _temp738->v;})));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc(
struct _tuple1* t){ struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt*
_temp739=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp739->v=( void*) Cyc_PP_text(*(( struct _tagged_string*)({ struct Cyc_Core_Opt*
_temp740=(* t).f1; if( _temp740 == 0){ _throw( Null_Exception);} _temp740->v;})));
_temp739;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt*
effopt){ if( effopt == 0){ if( varargs){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp741=( char*)"("; struct _tagged_string _temp742;
_temp742.curr= _temp741; _temp742.base= _temp741; _temp742.last_plus_one=
_temp741 + 2; _temp742;}),( struct _tagged_string)({ char* _temp743=( char*)")";
struct _tagged_string _temp744; _temp744.curr= _temp743; _temp744.base= _temp743;
_temp744.last_plus_one= _temp743 + 2; _temp744;}),( struct _tagged_string)({
char* _temp745=( char*)","; struct _tagged_string _temp746; _temp746.curr=
_temp745; _temp746.base= _temp745; _temp746.last_plus_one= _temp745 + 2;
_temp746;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)((( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args),({ struct Cyc_List_List* _temp747=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp747->hd=( void*) Cyc_PP_text(( struct
_tagged_string)({ char* _temp748=( char*)"..."; struct _tagged_string _temp749;
_temp749.curr= _temp748; _temp749.base= _temp748; _temp749.last_plus_one=
_temp748 + 4; _temp749;})); _temp747->tl= 0; _temp747;})));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp750=( char*)"("; struct _tagged_string
_temp751; _temp751.curr= _temp750; _temp751.base= _temp750; _temp751.last_plus_one=
_temp750 + 2; _temp751;}),( struct _tagged_string)({ char* _temp752=( char*)")";
struct _tagged_string _temp753; _temp753.curr= _temp752; _temp753.base= _temp752;
_temp753.last_plus_one= _temp752 + 2; _temp753;}),( struct _tagged_string)({
char* _temp754=( char*)","; struct _tagged_string _temp755; _temp755.curr=
_temp754; _temp755.base= _temp754; _temp755.last_plus_one= _temp754 + 2;
_temp755;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));}} else{
if( varargs){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp756=(
char*)"("; struct _tagged_string _temp757; _temp757.curr= _temp756; _temp757.base=
_temp756; _temp757.last_plus_one= _temp756 + 2; _temp757;}),( struct
_tagged_string)({ char* _temp758=( char*)")"; struct _tagged_string _temp759;
_temp759.curr= _temp758; _temp759.base= _temp758; _temp759.last_plus_one=
_temp758 + 2; _temp759;}),( struct _tagged_string)({ char* _temp760=( char*)";";
struct _tagged_string _temp761; _temp761.curr= _temp760; _temp761.base= _temp760;
_temp761.last_plus_one= _temp760 + 2; _temp761;}),({ struct Cyc_List_List*
_temp762=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp762->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp765=( char*)""; struct _tagged_string _temp766; _temp766.curr= _temp765;
_temp766.base= _temp765; _temp766.last_plus_one= _temp765 + 1; _temp766;}),(
struct _tagged_string)({ char* _temp767=( char*)""; struct _tagged_string
_temp768; _temp768.curr= _temp767; _temp768.base= _temp767; _temp768.last_plus_one=
_temp767 + 1; _temp768;}),( struct _tagged_string)({ char* _temp769=( char*)",";
struct _tagged_string _temp770; _temp770.curr= _temp769; _temp770.base= _temp769;
_temp770.last_plus_one= _temp769 + 2; _temp770;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args),({ struct Cyc_List_List*
_temp771=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp771->hd=( void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp772=(
char*)"..."; struct _tagged_string _temp773; _temp773.curr= _temp772; _temp773.base=
_temp772; _temp773.last_plus_one= _temp772 + 4; _temp773;})); _temp771->tl= 0;
_temp771;}))); _temp762->tl=({ struct Cyc_List_List* _temp763=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp763->hd=( void*) Cyc_Absynpp_eff2doc((
void*)({ struct Cyc_Core_Opt* _temp764= effopt; if( _temp764 == 0){ _throw(
Null_Exception);} _temp764->v;})); _temp763->tl= 0; _temp763;}); _temp762;}));}
else{ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp774=( char*)"(";
struct _tagged_string _temp775; _temp775.curr= _temp774; _temp775.base= _temp774;
_temp775.last_plus_one= _temp774 + 2; _temp775;}),( struct _tagged_string)({
char* _temp776=( char*)")"; struct _tagged_string _temp777; _temp777.curr=
_temp776; _temp777.base= _temp776; _temp777.last_plus_one= _temp776 + 2;
_temp777;}),( struct _tagged_string)({ char* _temp778=( char*)";"; struct
_tagged_string _temp779; _temp779.curr= _temp778; _temp779.base= _temp778;
_temp779.last_plus_one= _temp778 + 2; _temp779;}),({ struct Cyc_List_List*
_temp780=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp780->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp783=( char*)""; struct _tagged_string _temp784; _temp784.curr= _temp783;
_temp784.base= _temp783; _temp784.last_plus_one= _temp783 + 1; _temp784;}),(
struct _tagged_string)({ char* _temp785=( char*)""; struct _tagged_string
_temp786; _temp786.curr= _temp785; _temp786.base= _temp785; _temp786.last_plus_one=
_temp785 + 1; _temp786;}),( struct _tagged_string)({ char* _temp787=( char*)",";
struct _tagged_string _temp788; _temp788.curr= _temp787; _temp788.base= _temp787;
_temp788.last_plus_one= _temp787 + 2; _temp788;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_funarg2doc, args)); _temp780->tl=({ struct Cyc_List_List* _temp781=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp781->hd=(
void*) Cyc_Absynpp_eff2doc(( void*)({ struct Cyc_Core_Opt* _temp782= effopt; if(
_temp782 == 0){ _throw( Null_Exception);} _temp782->v;})); _temp781->tl= 0;
_temp781;}); _temp780;}));}}} struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct
_tuple3* arg){ return({ struct _tuple1* _temp789=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp789->f1=({ struct Cyc_Core_Opt* _temp790=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp790->v=( void*)(*
arg).f1; _temp790;}); _temp789->f2=(* arg).f2; _temp789->f3=(* arg).f3; _temp789;});}
struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct _tagged_string* v){ return Cyc_PP_text(*
v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
_temp791= 0; goto _LL792; _LL792:{ void* _temp793=(* q).f1; struct Cyc_List_List*
_temp805; struct Cyc_List_List* _temp807; struct Cyc_List_List* _temp809; struct
Cyc_List_List* _temp811; _LL795: if( _temp793 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL796;} else{ goto _LL797;} _LL797: if(( unsigned int) _temp793 > 1u?*(( int*)
_temp793) == Cyc_Absyn_Rel_n: 0){ _LL806: _temp805=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp793)->f1; if( _temp805 == 0){ goto _LL798;}
else{ goto _LL799;}} else{ goto _LL799;} _LL799: if(( unsigned int) _temp793 > 1u?*((
int*) _temp793) == Cyc_Absyn_Abs_n: 0){ _LL808: _temp807=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp793)->f1; if( _temp807 == 0){ goto _LL800;}
else{ goto _LL801;}} else{ goto _LL801;} _LL801: if(( unsigned int) _temp793 > 1u?*((
int*) _temp793) == Cyc_Absyn_Rel_n: 0){ _LL810: _temp809=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp793)->f1; goto _LL802;} else{ goto _LL803;}
_LL803: if(( unsigned int) _temp793 > 1u?*(( int*) _temp793) == Cyc_Absyn_Abs_n:
0){ _LL812: _temp811=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp793)->f1; goto _LL804;} else{ goto _LL794;} _LL796: goto _LL798; _LL798:
goto _LL800; _LL800: return Cyc_Absynpp_var2doc((* q).f2); _LL802: _temp811=
_temp809; goto _LL804; _LL804: _temp791= _temp811; goto _LL794; _LL794:;} if(
Cyc_Absynpp_qvar_to_Cids){ return Cyc_PP_text( Cyc_String_str_sepstr((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp791,({ struct Cyc_List_List* _temp813=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp813->hd=( void*)(* q).f2; _temp813->tl= 0;
_temp813;})),( struct _tagged_string)({ char* _temp814=( char*)"_"; struct
_tagged_string _temp815; _temp815.curr= _temp814; _temp815.base= _temp814;
_temp815.last_plus_one= _temp814 + 2; _temp815;})));} else{ if((( int(*)( int(*
cmp)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, _temp791,
Cyc_Absynpp_curr_namespace)){ return Cyc_Absynpp_var2doc((* q).f2);} else{
return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp791,({ struct
Cyc_List_List* _temp816=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp816->hd=( void*)(* q).f2; _temp816->tl= 0; _temp816;})),( struct
_tagged_string)({ char* _temp817=( char*)"::"; struct _tagged_string _temp818;
_temp818.curr= _temp817; _temp818.base= _temp817; _temp818.last_plus_one=
_temp817 + 3; _temp818;})));}}} struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc(
struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){ return Cyc_Absynpp_qvar2doc(
v);}{ void* _temp819=(* v).f1; struct Cyc_List_List* _temp825; _LL821: if((
unsigned int) _temp819 > 1u?*(( int*) _temp819) == Cyc_Absyn_Abs_n: 0){ _LL826:
_temp825=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp819)->f1;
goto _LL822;} else{ goto _LL823;} _LL823: goto _LL824; _LL822: if((( int(*)( int(*
cmp)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp825,
Cyc_Absynpp_curr_namespace) == 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{
return Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp827=(
char*)"/* wrong namespace : */ "; struct _tagged_string _temp828; _temp828.curr=
_temp827; _temp828.base= _temp827; _temp828.last_plus_one= _temp827 + 25;
_temp828;})), Cyc_Absynpp_qvar2doc( v));} _LL824: return Cyc_Absynpp_qvar2doc( v);
_LL820:;}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*
e){ void* _temp829=( void*) e->r; void* _temp901; void* _temp903; struct _tuple0*
_temp905; struct _tuple0* _temp907; struct Cyc_List_List* _temp909; void*
_temp911; struct Cyc_Absyn_Exp* _temp913; struct Cyc_Core_Opt* _temp915; struct
Cyc_Absyn_Exp* _temp917; void* _temp919; struct Cyc_Absyn_Exp* _temp921; struct
Cyc_Absyn_Exp* _temp923; struct Cyc_Absyn_Exp* _temp925; struct Cyc_Absyn_Exp*
_temp927; struct Cyc_Absyn_Exp* _temp929; struct Cyc_Absyn_Exp* _temp931; struct
Cyc_List_List* _temp933; struct Cyc_Absyn_Exp* _temp935; struct Cyc_List_List*
_temp937; struct Cyc_Absyn_Exp* _temp939; struct Cyc_Absyn_Exp* _temp941; struct
Cyc_Absyn_Exp* _temp943; struct Cyc_List_List* _temp945; struct Cyc_Absyn_Exp*
_temp947; struct Cyc_Absyn_Exp* _temp949; void* _temp951; struct Cyc_Absyn_Exp*
_temp953; struct Cyc_Absyn_Exp* _temp955; struct Cyc_Absyn_Exp* _temp957; void*
_temp959; struct Cyc_Absyn_Exp* _temp961; struct Cyc_Absyn_Exp* _temp963; struct
_tagged_string* _temp965; struct Cyc_Absyn_Exp* _temp967; struct _tagged_string*
_temp969; struct Cyc_Absyn_Exp* _temp971; struct Cyc_Absyn_Exp* _temp973; struct
Cyc_Absyn_Exp* _temp975; struct Cyc_List_List* _temp977; struct Cyc_List_List*
_temp979; struct _tuple1* _temp981; struct Cyc_List_List* _temp983; struct Cyc_Absyn_Exp*
_temp985; struct Cyc_Absyn_Exp* _temp987; struct Cyc_Absyn_Vardecl* _temp989;
struct Cyc_Absyn_Structdecl* _temp991; struct Cyc_List_List* _temp993; struct
Cyc_Core_Opt* _temp995; struct _tuple0* _temp997; struct Cyc_List_List* _temp999;
void* _temp1001; struct Cyc_Absyn_Tunionfield* _temp1003; struct Cyc_Absyn_Tuniondecl*
_temp1005; struct Cyc_List_List* _temp1007; struct Cyc_Core_Opt* _temp1009;
struct Cyc_Core_Opt* _temp1011; struct Cyc_Absyn_Enumfield* _temp1013; struct
Cyc_Absyn_Enumdecl* _temp1015; struct _tuple0* _temp1017; void* _temp1019;
struct Cyc_Absyn_Exp* _temp1021; struct Cyc_List_List* _temp1023; struct Cyc_Core_Opt*
_temp1025; struct Cyc_Absyn_Stmt* _temp1027; struct Cyc_Absyn_Fndecl* _temp1029;
struct Cyc_Absyn_Exp* _temp1031; _LL831: if(*(( int*) _temp829) == Cyc_Absyn_Const_e){
_LL902: _temp901=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp829)->f1;
goto _LL832;} else{ goto _LL833;} _LL833: if(*(( int*) _temp829) == Cyc_Absyn_Var_e){
_LL906: _temp905=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp829)->f1;
goto _LL904; _LL904: _temp903=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp829)->f2; goto _LL834;} else{ goto _LL835;} _LL835: if(*(( int*) _temp829)
== Cyc_Absyn_UnknownId_e){ _LL908: _temp907=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp829)->f1; goto _LL836;} else{ goto _LL837;} _LL837: if(*(( int*) _temp829)
== Cyc_Absyn_Primop_e){ _LL912: _temp911=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp829)->f1; goto _LL910; _LL910: _temp909=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp829)->f2; goto _LL838;} else{ goto _LL839;}
_LL839: if(*(( int*) _temp829) == Cyc_Absyn_AssignOp_e){ _LL918: _temp917=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp829)->f1;
goto _LL916; _LL916: _temp915=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp829)->f2; goto _LL914; _LL914: _temp913=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp829)->f3; goto _LL840;} else{ goto _LL841;}
_LL841: if(*(( int*) _temp829) == Cyc_Absyn_Increment_e){ _LL922: _temp921=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp829)->f1;
goto _LL920; _LL920: _temp919=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp829)->f2; goto _LL842;} else{ goto _LL843;} _LL843: if(*(( int*) _temp829)
== Cyc_Absyn_Conditional_e){ _LL928: _temp927=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp829)->f1; goto _LL926; _LL926: _temp925=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp829)->f2;
goto _LL924; _LL924: _temp923=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp829)->f3; goto _LL844;} else{ goto _LL845;} _LL845: if(*(( int*) _temp829)
== Cyc_Absyn_SeqExp_e){ _LL932: _temp931=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp829)->f1; goto _LL930; _LL930: _temp929=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp829)->f2; goto _LL846;} else{ goto _LL847;}
_LL847: if(*(( int*) _temp829) == Cyc_Absyn_UnknownCall_e){ _LL936: _temp935=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp829)->f1;
goto _LL934; _LL934: _temp933=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp829)->f2; goto _LL848;} else{ goto _LL849;} _LL849: if(*(( int*) _temp829)
== Cyc_Absyn_FnCall_e){ _LL940: _temp939=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp829)->f1; goto _LL938; _LL938: _temp937=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp829)->f2; goto _LL850;} else{ goto _LL851;}
_LL851: if(*(( int*) _temp829) == Cyc_Absyn_Throw_e){ _LL942: _temp941=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp829)->f1; goto _LL852;}
else{ goto _LL853;} _LL853: if(*(( int*) _temp829) == Cyc_Absyn_NoInstantiate_e){
_LL944: _temp943=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp829)->f1; goto _LL854;} else{ goto _LL855;} _LL855: if(*(( int*) _temp829)
== Cyc_Absyn_Instantiate_e){ _LL948: _temp947=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp829)->f1; goto _LL946; _LL946: _temp945=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp829)->f2;
goto _LL856;} else{ goto _LL857;} _LL857: if(*(( int*) _temp829) == Cyc_Absyn_Cast_e){
_LL952: _temp951=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp829)->f1; goto
_LL950; _LL950: _temp949=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp829)->f2; goto _LL858;} else{ goto _LL859;} _LL859: if(*(( int*) _temp829)
== Cyc_Absyn_New_e){ _LL956: _temp955=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp829)->f1; goto _LL954; _LL954: _temp953=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_New_e_struct*) _temp829)->f2; goto _LL860;} else{ goto _LL861;} _LL861:
if(*(( int*) _temp829) == Cyc_Absyn_Address_e){ _LL958: _temp957=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp829)->f1; goto _LL862;} else{ goto
_LL863;} _LL863: if(*(( int*) _temp829) == Cyc_Absyn_Sizeoftyp_e){ _LL960:
_temp959=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp829)->f1; goto
_LL864;} else{ goto _LL865;} _LL865: if(*(( int*) _temp829) == Cyc_Absyn_Sizeofexp_e){
_LL962: _temp961=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp829)->f1; goto _LL866;} else{ goto _LL867;} _LL867: if(*(( int*) _temp829)
== Cyc_Absyn_Deref_e){ _LL964: _temp963=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp829)->f1; goto _LL868;} else{ goto _LL869;} _LL869: if(*(( int*) _temp829)
== Cyc_Absyn_StructMember_e){ _LL968: _temp967=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp829)->f1; goto _LL966; _LL966: _temp965=(
struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp829)->f2;
goto _LL870;} else{ goto _LL871;} _LL871: if(*(( int*) _temp829) == Cyc_Absyn_StructArrow_e){
_LL972: _temp971=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp829)->f1; goto _LL970; _LL970: _temp969=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp829)->f2; goto _LL872;} else{ goto _LL873;}
_LL873: if(*(( int*) _temp829) == Cyc_Absyn_Subscript_e){ _LL976: _temp975=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp829)->f1;
goto _LL974; _LL974: _temp973=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp829)->f2; goto _LL874;} else{ goto _LL875;} _LL875: if(*(( int*) _temp829)
== Cyc_Absyn_Tuple_e){ _LL978: _temp977=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp829)->f1; goto _LL876;} else{ goto _LL877;} _LL877: if(*(( int*) _temp829)
== Cyc_Absyn_CompoundLit_e){ _LL982: _temp981=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp829)->f1; goto _LL980; _LL980: _temp979=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp829)->f2; goto _LL878;} else{ goto _LL879;}
_LL879: if(*(( int*) _temp829) == Cyc_Absyn_Array_e){ _LL984: _temp983=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp829)->f1; goto _LL880;}
else{ goto _LL881;} _LL881: if(*(( int*) _temp829) == Cyc_Absyn_Comprehension_e){
_LL990: _temp989=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp829)->f1; goto _LL988; _LL988: _temp987=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp829)->f2; goto _LL986; _LL986: _temp985=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp829)->f3;
goto _LL882;} else{ goto _LL883;} _LL883: if(*(( int*) _temp829) == Cyc_Absyn_Struct_e){
_LL998: _temp997=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp829)->f1; goto _LL996; _LL996: _temp995=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp829)->f2; goto _LL994; _LL994: _temp993=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp829)->f3; goto
_LL992; _LL992: _temp991=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp829)->f4; goto _LL884;} else{ goto _LL885;} _LL885: if(*(( int*) _temp829)
== Cyc_Absyn_AnonStruct_e){ _LL1002: _temp1001=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp829)->f1; goto _LL1000; _LL1000: _temp999=( struct Cyc_List_List*)(( struct
Cyc_Absyn_AnonStruct_e_struct*) _temp829)->f2; goto _LL886;} else{ goto _LL887;}
_LL887: if(*(( int*) _temp829) == Cyc_Absyn_Tunion_e){ _LL1012: _temp1011=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp829)->f1; goto
_LL1010; _LL1010: _temp1009=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp829)->f2; goto _LL1008; _LL1008: _temp1007=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp829)->f3; goto _LL1006; _LL1006:
_temp1005=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp829)->f4; goto _LL1004; _LL1004: _temp1003=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp829)->f5; goto _LL888;} else{ goto
_LL889;} _LL889: if(*(( int*) _temp829) == Cyc_Absyn_Enum_e){ _LL1018: _temp1017=(
struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp829)->f1; goto _LL1016;
_LL1016: _temp1015=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp829)->f2; goto _LL1014; _LL1014: _temp1013=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp829)->f3; goto _LL890;} else{ goto _LL891;}
_LL891: if(*(( int*) _temp829) == Cyc_Absyn_Malloc_e){ _LL1022: _temp1021=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp829)->f1; goto
_LL1020; _LL1020: _temp1019=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp829)->f2; goto _LL892;} else{ goto _LL893;} _LL893: if(*(( int*) _temp829)
== Cyc_Absyn_UnresolvedMem_e){ _LL1026: _temp1025=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp829)->f1; goto _LL1024; _LL1024:
_temp1023=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp829)->f2; goto _LL894;} else{ goto _LL895;} _LL895: if(*(( int*) _temp829)
== Cyc_Absyn_StmtExp_e){ _LL1028: _temp1027=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_StmtExp_e_struct*) _temp829)->f1; goto _LL896;} else{ goto _LL897;}
_LL897: if(*(( int*) _temp829) == Cyc_Absyn_Codegen_e){ _LL1030: _temp1029=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp829)->f1;
goto _LL898;} else{ goto _LL899;} _LL899: if(*(( int*) _temp829) == Cyc_Absyn_Fill_e){
_LL1032: _temp1031=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp829)->f1; goto _LL900;} else{ goto _LL830;} _LL832: return 10000; _LL834:
return 10000; _LL836: return 10000; _LL838: { void* _temp1033= _temp911; _LL1035:
if( _temp1033 ==( void*) Cyc_Absyn_Plus){ goto _LL1036;} else{ goto _LL1037;}
_LL1037: if( _temp1033 ==( void*) Cyc_Absyn_Times){ goto _LL1038;} else{ goto
_LL1039;} _LL1039: if( _temp1033 ==( void*) Cyc_Absyn_Minus){ goto _LL1040;}
else{ goto _LL1041;} _LL1041: if( _temp1033 ==( void*) Cyc_Absyn_Div){ goto
_LL1042;} else{ goto _LL1043;} _LL1043: if( _temp1033 ==( void*) Cyc_Absyn_Mod){
goto _LL1044;} else{ goto _LL1045;} _LL1045: if( _temp1033 ==( void*) Cyc_Absyn_Eq){
goto _LL1046;} else{ goto _LL1047;} _LL1047: if( _temp1033 ==( void*) Cyc_Absyn_Neq){
goto _LL1048;} else{ goto _LL1049;} _LL1049: if( _temp1033 ==( void*) Cyc_Absyn_Gt){
goto _LL1050;} else{ goto _LL1051;} _LL1051: if( _temp1033 ==( void*) Cyc_Absyn_Lt){
goto _LL1052;} else{ goto _LL1053;} _LL1053: if( _temp1033 ==( void*) Cyc_Absyn_Gte){
goto _LL1054;} else{ goto _LL1055;} _LL1055: if( _temp1033 ==( void*) Cyc_Absyn_Lte){
goto _LL1056;} else{ goto _LL1057;} _LL1057: if( _temp1033 ==( void*) Cyc_Absyn_Not){
goto _LL1058;} else{ goto _LL1059;} _LL1059: if( _temp1033 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1060;} else{ goto _LL1061;} _LL1061: if( _temp1033 ==( void*) Cyc_Absyn_Bitand){
goto _LL1062;} else{ goto _LL1063;} _LL1063: if( _temp1033 ==( void*) Cyc_Absyn_Bitor){
goto _LL1064;} else{ goto _LL1065;} _LL1065: if( _temp1033 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1066;} else{ goto _LL1067;} _LL1067: if( _temp1033 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1068;} else{ goto _LL1069;} _LL1069: if( _temp1033 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1070;} else{ goto _LL1071;} _LL1071: if( _temp1033 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1072;} else{ goto _LL1073;} _LL1073: if( _temp1033 ==( void*) Cyc_Absyn_Size){
goto _LL1074;} else{ goto _LL1075;} _LL1075: if( _temp1033 ==( void*) Cyc_Absyn_Printf){
goto _LL1076;} else{ goto _LL1077;} _LL1077: if( _temp1033 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1078;} else{ goto _LL1079;} _LL1079: if( _temp1033 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1080;} else{ goto _LL1081;} _LL1081: if( _temp1033 ==( void*) Cyc_Absyn_Scanf){
goto _LL1082;} else{ goto _LL1083;} _LL1083: if( _temp1033 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1084;} else{ goto _LL1085;} _LL1085: if( _temp1033 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1086;} else{ goto _LL1034;} _LL1036: return 100; _LL1038: return 110;
_LL1040: return 100; _LL1042: return 110; _LL1044: return 110; _LL1046: return
70; _LL1048: return 70; _LL1050: return 80; _LL1052: return 80; _LL1054: return
80; _LL1056: return 80; _LL1058: return 130; _LL1060: return 130; _LL1062:
return 60; _LL1064: return 40; _LL1066: return 50; _LL1068: return 90; _LL1070:
return 80; _LL1072: return 80; _LL1074: return 140; _LL1076: return 130; _LL1078:
return 130; _LL1080: return 130; _LL1082: return 130; _LL1084: return 130;
_LL1086: return 130; _LL1034:;} _LL840: return 20; _LL842: return 20; _LL844:
return 30; _LL846: return 10; _LL848: return 140; _LL850: return 140; _LL852:
return 130; _LL854: return Cyc_Absynpp_exp_prec( _temp943); _LL856: return Cyc_Absynpp_exp_prec(
_temp947); _LL858: return 120; _LL860: return 15; _LL862: return 130; _LL864:
return 130; _LL866: return 130; _LL868: return 130; _LL870: return 140; _LL872:
return 140; _LL874: return 140; _LL876: return 150; _LL878: goto _LL880; _LL880:
goto _LL882; _LL882: goto _LL884; _LL884: goto _LL886; _LL886: goto _LL888;
_LL888: goto _LL890; _LL890: goto _LL892; _LL892: return 140; _LL894: return 140;
_LL896: return 10000; _LL898: return 140; _LL900: return 140; _LL830:;} struct
Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct _tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e);
struct Cyc_PP_Doc* s;{ void* _temp1087=( void*) e->r; void* _temp1159; void*
_temp1161; struct _tuple0* _temp1163; struct _tuple0* _temp1165; struct Cyc_List_List*
_temp1167; void* _temp1169; struct Cyc_Absyn_Exp* _temp1171; struct Cyc_Core_Opt*
_temp1173; struct Cyc_Absyn_Exp* _temp1175; void* _temp1177; struct Cyc_Absyn_Exp*
_temp1179; struct Cyc_Absyn_Exp* _temp1181; struct Cyc_Absyn_Exp* _temp1183;
struct Cyc_Absyn_Exp* _temp1185; struct Cyc_Absyn_Exp* _temp1187; struct Cyc_Absyn_Exp*
_temp1189; struct Cyc_List_List* _temp1191; struct Cyc_Absyn_Exp* _temp1193;
struct Cyc_List_List* _temp1195; struct Cyc_Absyn_Exp* _temp1197; struct Cyc_Absyn_Exp*
_temp1199; struct Cyc_Absyn_Exp* _temp1201; struct Cyc_List_List* _temp1203;
struct Cyc_Absyn_Exp* _temp1205; struct Cyc_Absyn_Exp* _temp1207; void*
_temp1209; struct Cyc_Absyn_Exp* _temp1211; struct Cyc_Absyn_Exp* _temp1213;
struct Cyc_Absyn_Exp* _temp1215; void* _temp1217; struct Cyc_Absyn_Exp*
_temp1219; struct Cyc_Absyn_Exp* _temp1221; struct _tagged_string* _temp1223;
struct Cyc_Absyn_Exp* _temp1225; struct _tagged_string* _temp1227; struct Cyc_Absyn_Exp*
_temp1229; struct Cyc_Absyn_Exp* _temp1231; struct Cyc_Absyn_Exp* _temp1233;
struct Cyc_List_List* _temp1235; struct Cyc_List_List* _temp1237; struct _tuple1*
_temp1239; struct Cyc_List_List* _temp1241; struct Cyc_Absyn_Exp* _temp1243;
struct Cyc_Absyn_Exp* _temp1245; struct Cyc_Absyn_Vardecl* _temp1247; struct Cyc_Absyn_Structdecl*
_temp1249; struct Cyc_List_List* _temp1251; struct Cyc_Core_Opt* _temp1253;
struct _tuple0* _temp1255; struct Cyc_List_List* _temp1257; void* _temp1259;
struct Cyc_Absyn_Tunionfield* _temp1261; struct Cyc_Absyn_Tuniondecl* _temp1263;
struct Cyc_List_List* _temp1265; struct Cyc_Core_Opt* _temp1267; struct Cyc_Core_Opt*
_temp1269; struct Cyc_Absyn_Enumfield* _temp1271; struct Cyc_Absyn_Enumdecl*
_temp1273; struct _tuple0* _temp1275; void* _temp1277; struct Cyc_Absyn_Exp*
_temp1279; struct Cyc_List_List* _temp1281; struct Cyc_Core_Opt* _temp1283;
struct Cyc_Absyn_Stmt* _temp1285; struct Cyc_Absyn_Fndecl* _temp1287; struct Cyc_Absyn_Exp*
_temp1289; _LL1089: if(*(( int*) _temp1087) == Cyc_Absyn_Const_e){ _LL1160:
_temp1159=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1087)->f1; goto
_LL1090;} else{ goto _LL1091;} _LL1091: if(*(( int*) _temp1087) == Cyc_Absyn_Var_e){
_LL1164: _temp1163=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1087)->f1; goto _LL1162; _LL1162: _temp1161=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1087)->f2; goto _LL1092;} else{ goto _LL1093;} _LL1093: if(*(( int*)
_temp1087) == Cyc_Absyn_UnknownId_e){ _LL1166: _temp1165=( struct _tuple0*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1087)->f1; goto _LL1094;} else{ goto
_LL1095;} _LL1095: if(*(( int*) _temp1087) == Cyc_Absyn_Primop_e){ _LL1170:
_temp1169=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1087)->f1; goto
_LL1168; _LL1168: _temp1167=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1087)->f2; goto _LL1096;} else{ goto _LL1097;} _LL1097: if(*(( int*)
_temp1087) == Cyc_Absyn_AssignOp_e){ _LL1176: _temp1175=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1087)->f1; goto _LL1174; _LL1174:
_temp1173=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1087)->f2; goto _LL1172; _LL1172: _temp1171=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1087)->f3; goto _LL1098;} else{ goto
_LL1099;} _LL1099: if(*(( int*) _temp1087) == Cyc_Absyn_Increment_e){ _LL1180:
_temp1179=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1087)->f1; goto _LL1178; _LL1178: _temp1177=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1087)->f2; goto _LL1100;} else{ goto _LL1101;} _LL1101: if(*(( int*)
_temp1087) == Cyc_Absyn_Conditional_e){ _LL1186: _temp1185=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1087)->f1; goto _LL1184; _LL1184:
_temp1183=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1087)->f2; goto _LL1182; _LL1182: _temp1181=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1087)->f3; goto _LL1102;} else{
goto _LL1103;} _LL1103: if(*(( int*) _temp1087) == Cyc_Absyn_SeqExp_e){ _LL1190:
_temp1189=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1087)->f1; goto _LL1188; _LL1188: _temp1187=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1087)->f2; goto _LL1104;} else{ goto
_LL1105;} _LL1105: if(*(( int*) _temp1087) == Cyc_Absyn_UnknownCall_e){ _LL1194:
_temp1193=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1087)->f1; goto _LL1192; _LL1192: _temp1191=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1087)->f2; goto _LL1106;} else{
goto _LL1107;} _LL1107: if(*(( int*) _temp1087) == Cyc_Absyn_FnCall_e){ _LL1198:
_temp1197=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1087)->f1; goto _LL1196; _LL1196: _temp1195=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1087)->f2; goto _LL1108;} else{ goto
_LL1109;} _LL1109: if(*(( int*) _temp1087) == Cyc_Absyn_Throw_e){ _LL1200:
_temp1199=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1087)->f1;
goto _LL1110;} else{ goto _LL1111;} _LL1111: if(*(( int*) _temp1087) == Cyc_Absyn_NoInstantiate_e){
_LL1202: _temp1201=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1087)->f1; goto _LL1112;} else{ goto _LL1113;} _LL1113: if(*(( int*)
_temp1087) == Cyc_Absyn_Instantiate_e){ _LL1206: _temp1205=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1087)->f1; goto _LL1204; _LL1204:
_temp1203=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1087)->f2; goto _LL1114;} else{ goto _LL1115;} _LL1115: if(*(( int*)
_temp1087) == Cyc_Absyn_Cast_e){ _LL1210: _temp1209=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1087)->f1; goto _LL1208; _LL1208: _temp1207=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Cast_e_struct*) _temp1087)->f2; goto _LL1116;} else{ goto
_LL1117;} _LL1117: if(*(( int*) _temp1087) == Cyc_Absyn_Address_e){ _LL1212:
_temp1211=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp1087)->f1; goto _LL1118;} else{ goto _LL1119;} _LL1119: if(*(( int*)
_temp1087) == Cyc_Absyn_New_e){ _LL1216: _temp1215=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1087)->f1; goto _LL1214; _LL1214: _temp1213=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1087)->f2; goto
_LL1120;} else{ goto _LL1121;} _LL1121: if(*(( int*) _temp1087) == Cyc_Absyn_Sizeoftyp_e){
_LL1218: _temp1217=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1087)->f1;
goto _LL1122;} else{ goto _LL1123;} _LL1123: if(*(( int*) _temp1087) == Cyc_Absyn_Sizeofexp_e){
_LL1220: _temp1219=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1087)->f1; goto _LL1124;} else{ goto _LL1125;} _LL1125: if(*(( int*)
_temp1087) == Cyc_Absyn_Deref_e){ _LL1222: _temp1221=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp1087)->f1; goto _LL1126;} else{ goto
_LL1127;} _LL1127: if(*(( int*) _temp1087) == Cyc_Absyn_StructMember_e){ _LL1226:
_temp1225=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1087)->f1; goto _LL1224; _LL1224: _temp1223=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1087)->f2; goto _LL1128;} else{
goto _LL1129;} _LL1129: if(*(( int*) _temp1087) == Cyc_Absyn_StructArrow_e){
_LL1230: _temp1229=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1087)->f1; goto _LL1228; _LL1228: _temp1227=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1087)->f2; goto _LL1130;} else{
goto _LL1131;} _LL1131: if(*(( int*) _temp1087) == Cyc_Absyn_Subscript_e){
_LL1234: _temp1233=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1087)->f1; goto _LL1232; _LL1232: _temp1231=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1087)->f2; goto _LL1132;} else{ goto
_LL1133;} _LL1133: if(*(( int*) _temp1087) == Cyc_Absyn_Tuple_e){ _LL1236:
_temp1235=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1087)->f1;
goto _LL1134;} else{ goto _LL1135;} _LL1135: if(*(( int*) _temp1087) == Cyc_Absyn_CompoundLit_e){
_LL1240: _temp1239=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1087)->f1; goto _LL1238; _LL1238: _temp1237=( struct Cyc_List_List*)((
struct Cyc_Absyn_CompoundLit_e_struct*) _temp1087)->f2; goto _LL1136;} else{
goto _LL1137;} _LL1137: if(*(( int*) _temp1087) == Cyc_Absyn_Array_e){ _LL1242:
_temp1241=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1087)->f1;
goto _LL1138;} else{ goto _LL1139;} _LL1139: if(*(( int*) _temp1087) == Cyc_Absyn_Comprehension_e){
_LL1248: _temp1247=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1087)->f1; goto _LL1246; _LL1246: _temp1245=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1087)->f2; goto _LL1244; _LL1244:
_temp1243=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1087)->f3; goto _LL1140;} else{ goto _LL1141;} _LL1141: if(*(( int*)
_temp1087) == Cyc_Absyn_Struct_e){ _LL1256: _temp1255=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1087)->f1; goto _LL1254; _LL1254:
_temp1253=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1087)->f2;
goto _LL1252; _LL1252: _temp1251=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1087)->f3; goto _LL1250; _LL1250: _temp1249=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1087)->f4; goto _LL1142;} else{ goto
_LL1143;} _LL1143: if(*(( int*) _temp1087) == Cyc_Absyn_AnonStruct_e){ _LL1260:
_temp1259=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1087)->f1; goto
_LL1258; _LL1258: _temp1257=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1087)->f2; goto _LL1144;} else{ goto _LL1145;} _LL1145: if(*(( int*)
_temp1087) == Cyc_Absyn_Tunion_e){ _LL1270: _temp1269=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1087)->f1; goto _LL1268; _LL1268:
_temp1267=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1087)->f2;
goto _LL1266; _LL1266: _temp1265=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1087)->f3; goto _LL1264; _LL1264: _temp1263=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1087)->f4; goto _LL1262; _LL1262:
_temp1261=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1087)->f5; goto _LL1146;} else{ goto _LL1147;} _LL1147: if(*(( int*)
_temp1087) == Cyc_Absyn_Enum_e){ _LL1276: _temp1275=( struct _tuple0*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp1087)->f1; goto _LL1274; _LL1274: _temp1273=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1087)->f2;
goto _LL1272; _LL1272: _temp1271=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1087)->f3; goto _LL1148;} else{ goto _LL1149;} _LL1149: if(*(( int*)
_temp1087) == Cyc_Absyn_Malloc_e){ _LL1280: _temp1279=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Malloc_e_struct*) _temp1087)->f1; goto _LL1278; _LL1278:
_temp1277=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp1087)->f2; goto
_LL1150;} else{ goto _LL1151;} _LL1151: if(*(( int*) _temp1087) == Cyc_Absyn_UnresolvedMem_e){
_LL1284: _temp1283=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1087)->f1; goto _LL1282; _LL1282: _temp1281=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1087)->f2; goto _LL1152;} else{
goto _LL1153;} _LL1153: if(*(( int*) _temp1087) == Cyc_Absyn_StmtExp_e){ _LL1286:
_temp1285=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1087)->f1; goto _LL1154;} else{ goto _LL1155;} _LL1155: if(*(( int*)
_temp1087) == Cyc_Absyn_Codegen_e){ _LL1288: _temp1287=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp1087)->f1; goto _LL1156;} else{ goto
_LL1157;} _LL1157: if(*(( int*) _temp1087) == Cyc_Absyn_Fill_e){ _LL1290:
_temp1289=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1087)->f1;
goto _LL1158;} else{ goto _LL1088;} _LL1090: s= Cyc_Absynpp_cnst2doc( _temp1159);
goto _LL1088; _LL1092: _temp1165= _temp1163; goto _LL1094; _LL1094: s= Cyc_Absynpp_qvar2doc(
_temp1165); goto _LL1088; _LL1096: s= Cyc_Absynpp_primapp2doc( myprec, _temp1169,
_temp1167); goto _LL1088; _LL1098: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1175), Cyc_PP_text(( struct _tagged_string)({ char* _temp1291=(
char*)" "; struct _tagged_string _temp1292; _temp1292.curr= _temp1291; _temp1292.base=
_temp1291; _temp1292.last_plus_one= _temp1291 + 2; _temp1292;})), _temp1173 == 0?
Cyc_PP_text(( struct _tagged_string)({ char* _temp1293=( char*)""; struct
_tagged_string _temp1294; _temp1294.curr= _temp1293; _temp1294.base= _temp1293;
_temp1294.last_plus_one= _temp1293 + 1; _temp1294;})): Cyc_Absynpp_prim2doc((
void*)({ struct Cyc_Core_Opt* _temp1295= _temp1173; if( _temp1295 == 0){ _throw(
Null_Exception);} _temp1295->v;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1296=( char*)"= "; struct _tagged_string _temp1297; _temp1297.curr=
_temp1296; _temp1297.base= _temp1296; _temp1297.last_plus_one= _temp1296 + 3;
_temp1297;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1171)); goto _LL1088;
_LL1100: { struct Cyc_PP_Doc* _temp1298= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1179); goto _LL1299; _LL1299:{ void* _temp1300= _temp1177; _LL1302: if(
_temp1300 ==( void*) Cyc_Absyn_PreInc){ goto _LL1303;} else{ goto _LL1304;}
_LL1304: if( _temp1300 ==( void*) Cyc_Absyn_PreDec){ goto _LL1305;} else{ goto
_LL1306;} _LL1306: if( _temp1300 ==( void*) Cyc_Absyn_PostInc){ goto _LL1307;}
else{ goto _LL1308;} _LL1308: if( _temp1300 ==( void*) Cyc_Absyn_PostDec){ goto
_LL1309;} else{ goto _LL1301;} _LL1303: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1310=( char*)"++"; struct _tagged_string
_temp1311; _temp1311.curr= _temp1310; _temp1311.base= _temp1310; _temp1311.last_plus_one=
_temp1310 + 3; _temp1311;})), _temp1298); goto _LL1301; _LL1305: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1312=( char*)"--"; struct
_tagged_string _temp1313; _temp1313.curr= _temp1312; _temp1313.base= _temp1312;
_temp1313.last_plus_one= _temp1312 + 3; _temp1313;})), _temp1298); goto _LL1301;
_LL1307: s= Cyc_Absynpp_cat2( _temp1298, Cyc_PP_text(( struct _tagged_string)({
char* _temp1314=( char*)"++"; struct _tagged_string _temp1315; _temp1315.curr=
_temp1314; _temp1315.base= _temp1314; _temp1315.last_plus_one= _temp1314 + 3;
_temp1315;}))); goto _LL1301; _LL1309: s= Cyc_Absynpp_cat2( _temp1298, Cyc_PP_text((
struct _tagged_string)({ char* _temp1316=( char*)"--"; struct _tagged_string
_temp1317; _temp1317.curr= _temp1316; _temp1317.base= _temp1316; _temp1317.last_plus_one=
_temp1316 + 3; _temp1317;}))); goto _LL1301; _LL1301:;} goto _LL1088;} _LL1102:{
struct _tuple8 _temp1319=({ struct _tuple8 _temp1318; _temp1318.f1=( void*)
_temp1183->r; _temp1318.f2=( void*) _temp1181->r; _temp1318;}); _LL1321: goto
_LL1322; _LL1322: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1185), Cyc_PP_text(( struct _tagged_string)({ char* _temp1323=( char*)" ? ";
struct _tagged_string _temp1324; _temp1324.curr= _temp1323; _temp1324.base=
_temp1323; _temp1324.last_plus_one= _temp1323 + 4; _temp1324;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1183), Cyc_PP_text(( struct _tagged_string)({ char* _temp1325=( char*)" : ";
struct _tagged_string _temp1326; _temp1326.curr= _temp1325; _temp1326.base=
_temp1325; _temp1326.last_plus_one= _temp1325 + 4; _temp1326;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1181)); goto _LL1320; _LL1320:;} goto _LL1088; _LL1104: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1189), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1327=( char*)", "; struct _tagged_string _temp1328;
_temp1328.curr= _temp1327; _temp1328.base= _temp1327; _temp1328.last_plus_one=
_temp1327 + 3; _temp1328;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1187));
goto _LL1088; _LL1106: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1193), Cyc_PP_text(( struct _tagged_string)({ char* _temp1329=( char*)"(";
struct _tagged_string _temp1330; _temp1330.curr= _temp1329; _temp1330.base=
_temp1329; _temp1330.last_plus_one= _temp1329 + 2; _temp1330;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1191), Cyc_PP_text(( struct _tagged_string)({ char* _temp1331=( char*)")";
struct _tagged_string _temp1332; _temp1332.curr= _temp1331; _temp1332.base=
_temp1331; _temp1332.last_plus_one= _temp1331 + 2; _temp1332;}))); goto _LL1088;
_LL1108: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1197), Cyc_PP_text((
struct _tagged_string)({ char* _temp1333=( char*)"("; struct _tagged_string
_temp1334; _temp1334.curr= _temp1333; _temp1334.base= _temp1333; _temp1334.last_plus_one=
_temp1333 + 2; _temp1334;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1195), Cyc_PP_text((
struct _tagged_string)({ char* _temp1335=( char*)")"; struct _tagged_string
_temp1336; _temp1336.curr= _temp1335; _temp1336.base= _temp1335; _temp1336.last_plus_one=
_temp1335 + 2; _temp1336;}))); goto _LL1088; _LL1110: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1337=( char*)"throw "; struct _tagged_string
_temp1338; _temp1338.curr= _temp1337; _temp1338.base= _temp1337; _temp1338.last_plus_one=
_temp1337 + 7; _temp1338;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1199));
goto _LL1088; _LL1112: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1201); goto
_LL1088; _LL1114: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1205); goto _LL1088;
_LL1116: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1339=( char*)"("; struct _tagged_string _temp1340; _temp1340.curr=
_temp1339; _temp1340.base= _temp1339; _temp1340.last_plus_one= _temp1339 + 2;
_temp1340;})), Cyc_Absynpp_typ2doc( _temp1209), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1341=( char*)")"; struct _tagged_string _temp1342;
_temp1342.curr= _temp1341; _temp1342.base= _temp1341; _temp1342.last_plus_one=
_temp1341 + 2; _temp1342;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1207));
goto _LL1088; _LL1118: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({
char* _temp1343=( char*)"&"; struct _tagged_string _temp1344; _temp1344.curr=
_temp1343; _temp1344.base= _temp1343; _temp1344.last_plus_one= _temp1343 + 2;
_temp1344;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1211)); goto _LL1088;
_LL1120: if( _temp1215 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1345=( char*)"new "; struct _tagged_string
_temp1346; _temp1346.curr= _temp1345; _temp1346.base= _temp1345; _temp1346.last_plus_one=
_temp1345 + 5; _temp1346;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1213));}
else{ s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1347=( char*)"rnew("; struct _tagged_string _temp1348; _temp1348.curr=
_temp1347; _temp1348.base= _temp1347; _temp1348.last_plus_one= _temp1347 + 6;
_temp1348;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1349= _temp1215; if( _temp1349 == 0){ _throw( Null_Exception);} _temp1349;})),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1350=( char*)") "; struct
_tagged_string _temp1351; _temp1351.curr= _temp1350; _temp1351.base= _temp1350;
_temp1351.last_plus_one= _temp1350 + 3; _temp1351;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1213));} goto _LL1088; _LL1122: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1352=( char*)"sizeof("; struct
_tagged_string _temp1353; _temp1353.curr= _temp1352; _temp1353.base= _temp1352;
_temp1353.last_plus_one= _temp1352 + 8; _temp1353;})), Cyc_Absynpp_typ2doc(
_temp1217), Cyc_PP_text(( struct _tagged_string)({ char* _temp1354=( char*)")";
struct _tagged_string _temp1355; _temp1355.curr= _temp1354; _temp1355.base=
_temp1354; _temp1355.last_plus_one= _temp1354 + 2; _temp1355;}))); goto _LL1088;
_LL1124: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1356=( char*)"sizeof("; struct _tagged_string _temp1357; _temp1357.curr=
_temp1356; _temp1357.base= _temp1356; _temp1357.last_plus_one= _temp1356 + 8;
_temp1357;})), Cyc_Absynpp_exp2doc( _temp1219), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1358=( char*)")"; struct _tagged_string _temp1359;
_temp1359.curr= _temp1358; _temp1359.base= _temp1358; _temp1359.last_plus_one=
_temp1358 + 2; _temp1359;}))); goto _LL1088; _LL1126: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1360=( char*)"*"; struct _tagged_string
_temp1361; _temp1361.curr= _temp1360; _temp1361.base= _temp1360; _temp1361.last_plus_one=
_temp1360 + 2; _temp1361;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1221));
goto _LL1088; _LL1128: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1225), Cyc_PP_text(( struct _tagged_string)({ char* _temp1362=( char*)".";
struct _tagged_string _temp1363; _temp1363.curr= _temp1362; _temp1363.base=
_temp1362; _temp1363.last_plus_one= _temp1362 + 2; _temp1363;})), Cyc_Absynpp_textptr(
_temp1223)); goto _LL1088; _LL1130: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1229), Cyc_PP_text(( struct _tagged_string)({ char* _temp1364=(
char*)"->"; struct _tagged_string _temp1365; _temp1365.curr= _temp1364;
_temp1365.base= _temp1364; _temp1365.last_plus_one= _temp1364 + 3; _temp1365;})),
Cyc_Absynpp_textptr( _temp1227)); goto _LL1088; _LL1132: s= Cyc_Absynpp_cat4(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1233), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1366=( char*)"["; struct _tagged_string _temp1367;
_temp1367.curr= _temp1366; _temp1367.base= _temp1366; _temp1367.last_plus_one=
_temp1366 + 2; _temp1367;})), Cyc_Absynpp_exp2doc( _temp1231), Cyc_PP_text((
struct _tagged_string)({ char* _temp1368=( char*)"]"; struct _tagged_string
_temp1369; _temp1369.curr= _temp1368; _temp1369.base= _temp1368; _temp1369.last_plus_one=
_temp1368 + 2; _temp1369;}))); goto _LL1088; _LL1134: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1370=( char*)"$("; struct _tagged_string
_temp1371; _temp1371.curr= _temp1370; _temp1371.base= _temp1370; _temp1371.last_plus_one=
_temp1370 + 3; _temp1371;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1235), Cyc_PP_text((
struct _tagged_string)({ char* _temp1372=( char*)")"; struct _tagged_string
_temp1373; _temp1373.curr= _temp1372; _temp1373.base= _temp1372; _temp1373.last_plus_one=
_temp1372 + 2; _temp1373;}))); goto _LL1088; _LL1136: s= Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1374=( char*)"("; struct _tagged_string
_temp1375; _temp1375.curr= _temp1374; _temp1375.base= _temp1374; _temp1375.last_plus_one=
_temp1374 + 2; _temp1375;})), Cyc_Absynpp_typ2doc((* _temp1239).f3), Cyc_PP_text((
struct _tagged_string)({ char* _temp1376=( char*)")"; struct _tagged_string
_temp1377; _temp1377.curr= _temp1376; _temp1377.base= _temp1376; _temp1377.last_plus_one=
_temp1376 + 2; _temp1377;})), Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp1378=( char*)"{"; struct _tagged_string _temp1379; _temp1379.curr=
_temp1378; _temp1379.base= _temp1378; _temp1379.last_plus_one= _temp1378 + 2;
_temp1379;}),( struct _tagged_string)({ char* _temp1380=( char*)"}"; struct
_tagged_string _temp1381; _temp1381.curr= _temp1380; _temp1381.base= _temp1380;
_temp1381.last_plus_one= _temp1380 + 2; _temp1381;}),( struct _tagged_string)({
char* _temp1382=( char*)","; struct _tagged_string _temp1383; _temp1383.curr=
_temp1382; _temp1383.base= _temp1382; _temp1383.last_plus_one= _temp1382 + 2;
_temp1383;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1237))); goto _LL1088; _LL1138: s= Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1384=( char*)"{"; struct _tagged_string _temp1385;
_temp1385.curr= _temp1384; _temp1385.base= _temp1384; _temp1385.last_plus_one=
_temp1384 + 2; _temp1385;}),( struct _tagged_string)({ char* _temp1386=( char*)"}";
struct _tagged_string _temp1387; _temp1387.curr= _temp1386; _temp1387.base=
_temp1386; _temp1387.last_plus_one= _temp1386 + 2; _temp1387;}),( struct
_tagged_string)({ char* _temp1388=( char*)","; struct _tagged_string _temp1389;
_temp1389.curr= _temp1388; _temp1389.base= _temp1388; _temp1389.last_plus_one=
_temp1388 + 2; _temp1389;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1241)); goto _LL1088; _LL1140: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1390=( char*)"{for "; struct _tagged_string
_temp1391; _temp1391.curr= _temp1390; _temp1391.base= _temp1390; _temp1391.last_plus_one=
_temp1390 + 6; _temp1391;})), Cyc_PP_text(*(* _temp1247->name).f2), Cyc_PP_text((
struct _tagged_string)({ char* _temp1392=( char*)" < "; struct _tagged_string
_temp1393; _temp1393.curr= _temp1392; _temp1393.base= _temp1392; _temp1393.last_plus_one=
_temp1392 + 4; _temp1393;})), Cyc_Absynpp_exp2doc( _temp1245), Cyc_PP_text((
struct _tagged_string)({ char* _temp1394=( char*)" : "; struct _tagged_string
_temp1395; _temp1395.curr= _temp1394; _temp1395.base= _temp1394; _temp1395.last_plus_one=
_temp1394 + 4; _temp1395;})), Cyc_Absynpp_exp2doc( _temp1243), Cyc_PP_text((
struct _tagged_string)({ char* _temp1396=( char*)"}"; struct _tagged_string
_temp1397; _temp1397.curr= _temp1396; _temp1397.base= _temp1396; _temp1397.last_plus_one=
_temp1396 + 2; _temp1397;}))); goto _LL1088; _LL1142: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1255), Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1398=( char*)"{";
struct _tagged_string _temp1399; _temp1399.curr= _temp1398; _temp1399.base=
_temp1398; _temp1399.last_plus_one= _temp1398 + 2; _temp1399;}),( struct
_tagged_string)({ char* _temp1400=( char*)"}"; struct _tagged_string _temp1401;
_temp1401.curr= _temp1400; _temp1401.base= _temp1400; _temp1401.last_plus_one=
_temp1400 + 2; _temp1401;}),( struct _tagged_string)({ char* _temp1402=( char*)",";
struct _tagged_string _temp1403; _temp1403.curr= _temp1402; _temp1403.base=
_temp1402; _temp1403.last_plus_one= _temp1402 + 2; _temp1403;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1251))); goto _LL1088; _LL1144: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1404=( char*)"{"; struct _tagged_string
_temp1405; _temp1405.curr= _temp1404; _temp1405.base= _temp1404; _temp1405.last_plus_one=
_temp1404 + 2; _temp1405;}),( struct _tagged_string)({ char* _temp1406=( char*)"}";
struct _tagged_string _temp1407; _temp1407.curr= _temp1406; _temp1407.base=
_temp1406; _temp1407.last_plus_one= _temp1406 + 2; _temp1407;}),( struct
_tagged_string)({ char* _temp1408=( char*)","; struct _tagged_string _temp1409;
_temp1409.curr= _temp1408; _temp1409.base= _temp1408; _temp1409.last_plus_one=
_temp1408 + 2; _temp1409;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1257)); goto _LL1088; _LL1146: if( _temp1265 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1261->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1261->name),
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1410=( char*)"(";
struct _tagged_string _temp1411; _temp1411.curr= _temp1410; _temp1411.base=
_temp1410; _temp1411.last_plus_one= _temp1410 + 2; _temp1411;}),( struct
_tagged_string)({ char* _temp1412=( char*)")"; struct _tagged_string _temp1413;
_temp1413.curr= _temp1412; _temp1413.base= _temp1412; _temp1413.last_plus_one=
_temp1412 + 2; _temp1413;}),( struct _tagged_string)({ char* _temp1414=( char*)",";
struct _tagged_string _temp1415; _temp1415.curr= _temp1414; _temp1415.base=
_temp1414; _temp1415.last_plus_one= _temp1414 + 2; _temp1415;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp1265)));} goto _LL1088; _LL1148: s= Cyc_Absynpp_qvar2doc(
_temp1275); goto _LL1088; _LL1150: if( _temp1279 == 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1416=( char*)"malloc(sizeof("; struct
_tagged_string _temp1417; _temp1417.curr= _temp1416; _temp1417.base= _temp1416;
_temp1417.last_plus_one= _temp1416 + 15; _temp1417;})), Cyc_Absynpp_typ2doc(
_temp1277), Cyc_PP_text(( struct _tagged_string)({ char* _temp1418=( char*)"))";
struct _tagged_string _temp1419; _temp1419.curr= _temp1418; _temp1419.base=
_temp1418; _temp1419.last_plus_one= _temp1418 + 3; _temp1419;})));} else{ s= Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1420=( char*)"rmalloc(";
struct _tagged_string _temp1421; _temp1421.curr= _temp1420; _temp1421.base=
_temp1420; _temp1421.last_plus_one= _temp1420 + 9; _temp1421;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1422= _temp1279; if(
_temp1422 == 0){ _throw( Null_Exception);} _temp1422;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1423=( char*)","; struct _tagged_string _temp1424;
_temp1424.curr= _temp1423; _temp1424.base= _temp1423; _temp1424.last_plus_one=
_temp1423 + 2; _temp1424;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1425=( char*)"sizeof("; struct _tagged_string _temp1426; _temp1426.curr=
_temp1425; _temp1426.base= _temp1425; _temp1426.last_plus_one= _temp1425 + 8;
_temp1426;})), Cyc_Absynpp_typ2doc( _temp1277), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1427=( char*)"))"; struct _tagged_string _temp1428;
_temp1428.curr= _temp1427; _temp1428.base= _temp1427; _temp1428.last_plus_one=
_temp1427 + 3; _temp1428;})));} goto _LL1088; _LL1152: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1429=( char*)"{"; struct _tagged_string
_temp1430; _temp1430.curr= _temp1429; _temp1430.base= _temp1429; _temp1430.last_plus_one=
_temp1429 + 2; _temp1430;}),( struct _tagged_string)({ char* _temp1431=( char*)"}";
struct _tagged_string _temp1432; _temp1432.curr= _temp1431; _temp1432.base=
_temp1431; _temp1432.last_plus_one= _temp1431 + 2; _temp1432;}),( struct
_tagged_string)({ char* _temp1433=( char*)","; struct _tagged_string _temp1434;
_temp1434.curr= _temp1433; _temp1434.base= _temp1433; _temp1434.last_plus_one=
_temp1433 + 2; _temp1434;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1281)); goto _LL1088; _LL1154: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1435=( char*)"({ "; struct _tagged_string _temp1436;
_temp1436.curr= _temp1435; _temp1436.base= _temp1435; _temp1436.last_plus_one=
_temp1435 + 4; _temp1436;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1285)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1437=( char*)" })"; struct
_tagged_string _temp1438; _temp1438.curr= _temp1437; _temp1438.base= _temp1437;
_temp1438.last_plus_one= _temp1437 + 4; _temp1438;}))); goto _LL1088; _LL1156: s=
Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1439=( char*)"codegen(";
struct _tagged_string _temp1440; _temp1440.curr= _temp1439; _temp1440.base=
_temp1439; _temp1440.last_plus_one= _temp1439 + 9; _temp1440;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1441=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1441->r=( void*)(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp1442=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1442[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1443; _temp1443.tag= Cyc_Absyn_Fn_d; _temp1443.f1= _temp1287; _temp1443;});
_temp1442;})); _temp1441->loc= e->loc; _temp1441;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1444=( char*)")"; struct _tagged_string _temp1445;
_temp1445.curr= _temp1444; _temp1445.base= _temp1444; _temp1445.last_plus_one=
_temp1444 + 2; _temp1445;}))); goto _LL1088; _LL1158: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1446=( char*)"fill("; struct _tagged_string
_temp1447; _temp1447.curr= _temp1446; _temp1447.base= _temp1446; _temp1447.last_plus_one=
_temp1446 + 6; _temp1447;})), Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1289)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1448=( char*)")"; struct
_tagged_string _temp1449; _temp1449.curr= _temp1448; _temp1449.base= _temp1448;
_temp1449.last_plus_one= _temp1448 + 2; _temp1449;}))); goto _LL1088; _LL1088:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp1450=( char*)"("; struct _tagged_string _temp1451; _temp1451.curr=
_temp1450; _temp1451.base= _temp1450; _temp1451.last_plus_one= _temp1450 + 2;
_temp1451;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp1452=( char*)")";
struct _tagged_string _temp1453; _temp1453.curr= _temp1452; _temp1453.base=
_temp1452; _temp1453.last_plus_one= _temp1452 + 2; _temp1453;})));} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1454= d;
struct Cyc_Absyn_Exp* _temp1460; struct _tagged_string* _temp1462; _LL1456: if(*((
int*) _temp1454) == Cyc_Absyn_ArrayElement){ _LL1461: _temp1460=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1454)->f1; goto _LL1457;} else{ goto
_LL1458;} _LL1458: if(*(( int*) _temp1454) == Cyc_Absyn_FieldName){ _LL1463:
_temp1462=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1454)->f1; goto _LL1459;} else{ goto _LL1455;} _LL1457: return Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1464=( char*)".["; struct
_tagged_string _temp1465; _temp1465.curr= _temp1464; _temp1465.base= _temp1464;
_temp1465.last_plus_one= _temp1464 + 3; _temp1465;})), Cyc_Absynpp_exp2doc(
_temp1460), Cyc_PP_text(( struct _tagged_string)({ char* _temp1466=( char*)"]";
struct _tagged_string _temp1467; _temp1467.curr= _temp1466; _temp1467.base=
_temp1466; _temp1467.last_plus_one= _temp1466 + 2; _temp1467;}))); _LL1459:
return Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp1468=(
char*)"."; struct _tagged_string _temp1469; _temp1469.curr= _temp1468; _temp1469.base=
_temp1468; _temp1469.last_plus_one= _temp1468 + 2; _temp1469;})), Cyc_Absynpp_textptr(
_temp1462)); _LL1455:;} struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6*
de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((* de).f2);} else{ return
Cyc_Absynpp_cat2( Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1470=(
char*)""; struct _tagged_string _temp1471; _temp1471.curr= _temp1470; _temp1471.base=
_temp1470; _temp1471.last_plus_one= _temp1470 + 1; _temp1471;}),( struct
_tagged_string)({ char* _temp1472=( char*)"="; struct _tagged_string _temp1473;
_temp1473.curr= _temp1472; _temp1473.base= _temp1472; _temp1473.last_plus_one=
_temp1472 + 2; _temp1473;}),( struct _tagged_string)({ char* _temp1474=( char*)"=";
struct _tagged_string _temp1475; _temp1475.curr= _temp1474; _temp1475.base=
_temp1474; _temp1475.last_plus_one= _temp1474 + 2; _temp1475;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)), Cyc_Absynpp_exp2doc((* de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1476=( char*)""; struct _tagged_string _temp1477;
_temp1477.curr= _temp1476; _temp1477.base= _temp1476; _temp1477.last_plus_one=
_temp1476 + 1; _temp1477;}),( struct _tagged_string)({ char* _temp1478=( char*)"";
struct _tagged_string _temp1479; _temp1479.curr= _temp1478; _temp1479.base=
_temp1478; _temp1479.last_plus_one= _temp1478 + 1; _temp1479;}),( struct
_tagged_string)({ char* _temp1480=( char*)","; struct _tagged_string _temp1481;
_temp1481.curr= _temp1480; _temp1481.base= _temp1480; _temp1481.last_plus_one=
_temp1480 + 2; _temp1481;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1482= c; char _temp1500; void* _temp1502; short _temp1504;
void* _temp1506; int _temp1508; void* _temp1510; int _temp1512; void* _temp1514;
long long _temp1516; void* _temp1518; struct _tagged_string _temp1520; struct
_tagged_string _temp1522; _LL1484: if(( unsigned int) _temp1482 > 1u?*(( int*)
_temp1482) == Cyc_Absyn_Char_c: 0){ _LL1503: _temp1502=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp1482)->f1; goto _LL1501; _LL1501: _temp1500=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp1482)->f2; goto _LL1485;} else{ goto _LL1486;} _LL1486: if(( unsigned int)
_temp1482 > 1u?*(( int*) _temp1482) == Cyc_Absyn_Short_c: 0){ _LL1507: _temp1506=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1482)->f1; goto _LL1505; _LL1505:
_temp1504=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp1482)->f2; goto
_LL1487;} else{ goto _LL1488;} _LL1488: if(( unsigned int) _temp1482 > 1u?*((
int*) _temp1482) == Cyc_Absyn_Int_c: 0){ _LL1511: _temp1510=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1482)->f1; if( _temp1510 ==( void*) Cyc_Absyn_Signed){
goto _LL1509;} else{ goto _LL1490;} _LL1509: _temp1508=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp1482)->f2; goto _LL1489;} else{ goto _LL1490;} _LL1490: if(( unsigned int)
_temp1482 > 1u?*(( int*) _temp1482) == Cyc_Absyn_Int_c: 0){ _LL1515: _temp1514=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1482)->f1; if( _temp1514 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL1513;} else{ goto _LL1492;} _LL1513: _temp1512=(
int)(( struct Cyc_Absyn_Int_c_struct*) _temp1482)->f2; goto _LL1491;} else{ goto
_LL1492;} _LL1492: if(( unsigned int) _temp1482 > 1u?*(( int*) _temp1482) == Cyc_Absyn_LongLong_c:
0){ _LL1519: _temp1518=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1482)->f1;
goto _LL1517; _LL1517: _temp1516=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1482)->f2; goto _LL1493;} else{ goto _LL1494;} _LL1494: if(( unsigned int)
_temp1482 > 1u?*(( int*) _temp1482) == Cyc_Absyn_Float_c: 0){ _LL1521: _temp1520=(
struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*) _temp1482)->f1; goto
_LL1495;} else{ goto _LL1496;} _LL1496: if( _temp1482 ==( void*) Cyc_Absyn_Null_c){
goto _LL1497;} else{ goto _LL1498;} _LL1498: if(( unsigned int) _temp1482 > 1u?*((
int*) _temp1482) == Cyc_Absyn_String_c: 0){ _LL1523: _temp1522=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp1482)->f1; goto
_LL1499;} else{ goto _LL1483;} _LL1485: return Cyc_PP_text(({ struct
_tagged_string _temp1524= Cyc_Absynpp_char_escape( _temp1500); xprintf("'%.*s'",
_temp1524.last_plus_one - _temp1524.curr, _temp1524.curr);})); _LL1487: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1504)); _LL1489: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1508)); _LL1491: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1512)); _LL1493: return Cyc_PP_text(( struct _tagged_string)({
char* _temp1525=( char*)"<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp1526; _temp1526.curr= _temp1525; _temp1526.base= _temp1525; _temp1526.last_plus_one=
_temp1525 + 27; _temp1526;})); _LL1495: return Cyc_PP_text( _temp1520); _LL1497:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp1527=( char*)"null";
struct _tagged_string _temp1528; _temp1528.curr= _temp1527; _temp1528.base=
_temp1527; _temp1528.last_plus_one= _temp1527 + 5; _temp1528;})); _LL1499:
return Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1529=(
char*)"\""; struct _tagged_string _temp1530; _temp1530.curr= _temp1529;
_temp1530.base= _temp1529; _temp1530.last_plus_one= _temp1529 + 2; _temp1530;})),
Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1522)), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1531=( char*)"\""; struct _tagged_string _temp1532;
_temp1532.curr= _temp1531; _temp1532.base= _temp1531; _temp1532.last_plus_one=
_temp1531 + 2; _temp1532;}))); _LL1483:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:({ struct Cyc_List_List*
_temp1533= es; if( _temp1533 == 0){ _throw( Null_Exception);} _temp1533->tl;})
!= 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp1534=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1534[ 0]=({ struct Cyc_Core_Failure_struct _temp1535; _temp1535.tag= Cyc_Core_Failure;
_temp1535.f1=({ struct _tagged_string _temp1536= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1536.last_plus_one
- _temp1536.curr, _temp1536.curr);}); _temp1535;}); _temp1534;}));}{ struct Cyc_PP_Doc*
_temp1538= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)({ struct
Cyc_List_List* _temp1537= es; if( _temp1537 == 0){ _throw( Null_Exception);}
_temp1537->hd;})); goto _LL1539; _LL1539: return Cyc_Absynpp_cat2( _temp1538,
Cyc_PP_text(( struct _tagged_string)({ char* _temp1540=( char*)".size"; struct
_tagged_string _temp1541; _temp1541.curr= _temp1540; _temp1541.base= _temp1540;
_temp1541.last_plus_one= _temp1540 + 6; _temp1541;})));}} else{ if( ! Cyc_Absyn_is_format_prim(
p)){ struct Cyc_List_List* ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(*
f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es); if( ds == 0){( void) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp1542=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1542[ 0]=({ struct Cyc_Core_Failure_struct
_temp1543; _temp1543.tag= Cyc_Core_Failure; _temp1543.f1=({ struct
_tagged_string _temp1544= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with no args",
_temp1544.last_plus_one - _temp1544.curr, _temp1544.curr);}); _temp1543;});
_temp1542;}));} else{ if(({ struct Cyc_List_List* _temp1545= ds; if( _temp1545
== 0){ _throw( Null_Exception);} _temp1545->tl;}) == 0){ return Cyc_Absynpp_cat3(
ps, Cyc_PP_text(( struct _tagged_string)({ char* _temp1546=( char*)" "; struct
_tagged_string _temp1547; _temp1547.curr= _temp1546; _temp1547.base= _temp1546;
_temp1547.last_plus_one= _temp1546 + 2; _temp1547;})),( struct Cyc_PP_Doc*)({
struct Cyc_List_List* _temp1548= ds; if( _temp1548 == 0){ _throw( Null_Exception);}
_temp1548->hd;}));} else{ if(({ struct Cyc_List_List* _temp1550=({ struct Cyc_List_List*
_temp1549= ds; if( _temp1549 == 0){ _throw( Null_Exception);} _temp1549->tl;});
if( _temp1550 == 0){ _throw( Null_Exception);} _temp1550->tl;}) != 0){( void)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp1551=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1551[ 0]=({ struct Cyc_Core_Failure_struct
_temp1552; _temp1552.tag= Cyc_Core_Failure; _temp1552.f1=({ struct
_tagged_string _temp1553= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_temp1553.last_plus_one - _temp1553.curr, _temp1553.curr);}); _temp1552;});
_temp1551;}));} else{ return Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1554= ds; if( _temp1554 == 0){ _throw( Null_Exception);} _temp1554->hd;}),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1555=( char*)" "; struct
_tagged_string _temp1556; _temp1556.curr= _temp1555; _temp1556.base= _temp1555;
_temp1556.last_plus_one= _temp1555 + 2; _temp1556;})), ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1557=( char*)" "; struct _tagged_string _temp1558;
_temp1558.curr= _temp1557; _temp1558.base= _temp1557; _temp1558.last_plus_one=
_temp1557 + 2; _temp1558;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1560=({ struct Cyc_List_List* _temp1559= ds; if( _temp1559 == 0){ _throw(
Null_Exception);} _temp1559->tl;}); if( _temp1560 == 0){ _throw( Null_Exception);}
_temp1560->hd;}));}}}} else{ return Cyc_Absynpp_cat4( ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1561=( char*)"("; struct _tagged_string _temp1562;
_temp1562.curr= _temp1561; _temp1562.base= _temp1561; _temp1562.last_plus_one=
_temp1561 + 2; _temp1562;})), Cyc_Absynpp_exps2doc_prec( 20, es), Cyc_PP_text((
struct _tagged_string)({ char* _temp1563=( char*)")"; struct _tagged_string
_temp1564; _temp1564.curr= _temp1563; _temp1564.base= _temp1563; _temp1564.last_plus_one=
_temp1563 + 2; _temp1564;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1565= p; _LL1567: if( _temp1565 ==( void*) Cyc_Absyn_Plus){
goto _LL1568;} else{ goto _LL1569;} _LL1569: if( _temp1565 ==( void*) Cyc_Absyn_Times){
goto _LL1570;} else{ goto _LL1571;} _LL1571: if( _temp1565 ==( void*) Cyc_Absyn_Minus){
goto _LL1572;} else{ goto _LL1573;} _LL1573: if( _temp1565 ==( void*) Cyc_Absyn_Div){
goto _LL1574;} else{ goto _LL1575;} _LL1575: if( _temp1565 ==( void*) Cyc_Absyn_Mod){
goto _LL1576;} else{ goto _LL1577;} _LL1577: if( _temp1565 ==( void*) Cyc_Absyn_Eq){
goto _LL1578;} else{ goto _LL1579;} _LL1579: if( _temp1565 ==( void*) Cyc_Absyn_Neq){
goto _LL1580;} else{ goto _LL1581;} _LL1581: if( _temp1565 ==( void*) Cyc_Absyn_Gt){
goto _LL1582;} else{ goto _LL1583;} _LL1583: if( _temp1565 ==( void*) Cyc_Absyn_Lt){
goto _LL1584;} else{ goto _LL1585;} _LL1585: if( _temp1565 ==( void*) Cyc_Absyn_Gte){
goto _LL1586;} else{ goto _LL1587;} _LL1587: if( _temp1565 ==( void*) Cyc_Absyn_Lte){
goto _LL1588;} else{ goto _LL1589;} _LL1589: if( _temp1565 ==( void*) Cyc_Absyn_Not){
goto _LL1590;} else{ goto _LL1591;} _LL1591: if( _temp1565 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1592;} else{ goto _LL1593;} _LL1593: if( _temp1565 ==( void*) Cyc_Absyn_Bitand){
goto _LL1594;} else{ goto _LL1595;} _LL1595: if( _temp1565 ==( void*) Cyc_Absyn_Bitor){
goto _LL1596;} else{ goto _LL1597;} _LL1597: if( _temp1565 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1598;} else{ goto _LL1599;} _LL1599: if( _temp1565 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1600;} else{ goto _LL1601;} _LL1601: if( _temp1565 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1602;} else{ goto _LL1603;} _LL1603: if( _temp1565 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1604;} else{ goto _LL1605;} _LL1605: if( _temp1565 ==( void*) Cyc_Absyn_Size){
goto _LL1606;} else{ goto _LL1607;} _LL1607: if( _temp1565 ==( void*) Cyc_Absyn_Printf){
goto _LL1608;} else{ goto _LL1609;} _LL1609: if( _temp1565 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1610;} else{ goto _LL1611;} _LL1611: if( _temp1565 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1612;} else{ goto _LL1613;} _LL1613: if( _temp1565 ==( void*) Cyc_Absyn_Scanf){
goto _LL1614;} else{ goto _LL1615;} _LL1615: if( _temp1565 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1616;} else{ goto _LL1617;} _LL1617: if( _temp1565 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1618;} else{ goto _LL1566;} _LL1568: return( struct _tagged_string)({
char* _temp1619=( char*)"+"; struct _tagged_string _temp1620; _temp1620.curr=
_temp1619; _temp1620.base= _temp1619; _temp1620.last_plus_one= _temp1619 + 2;
_temp1620;}); _LL1570: return( struct _tagged_string)({ char* _temp1621=( char*)"*";
struct _tagged_string _temp1622; _temp1622.curr= _temp1621; _temp1622.base=
_temp1621; _temp1622.last_plus_one= _temp1621 + 2; _temp1622;}); _LL1572: return(
struct _tagged_string)({ char* _temp1623=( char*)"-"; struct _tagged_string
_temp1624; _temp1624.curr= _temp1623; _temp1624.base= _temp1623; _temp1624.last_plus_one=
_temp1623 + 2; _temp1624;}); _LL1574: return( struct _tagged_string)({ char*
_temp1625=( char*)"/"; struct _tagged_string _temp1626; _temp1626.curr=
_temp1625; _temp1626.base= _temp1625; _temp1626.last_plus_one= _temp1625 + 2;
_temp1626;}); _LL1576: return( struct _tagged_string)({ char* _temp1627=( char*)"%";
struct _tagged_string _temp1628; _temp1628.curr= _temp1627; _temp1628.base=
_temp1627; _temp1628.last_plus_one= _temp1627 + 2; _temp1628;}); _LL1578: return(
struct _tagged_string)({ char* _temp1629=( char*)"=="; struct _tagged_string
_temp1630; _temp1630.curr= _temp1629; _temp1630.base= _temp1629; _temp1630.last_plus_one=
_temp1629 + 3; _temp1630;}); _LL1580: return( struct _tagged_string)({ char*
_temp1631=( char*)"!="; struct _tagged_string _temp1632; _temp1632.curr=
_temp1631; _temp1632.base= _temp1631; _temp1632.last_plus_one= _temp1631 + 3;
_temp1632;}); _LL1582: return( struct _tagged_string)({ char* _temp1633=( char*)">";
struct _tagged_string _temp1634; _temp1634.curr= _temp1633; _temp1634.base=
_temp1633; _temp1634.last_plus_one= _temp1633 + 2; _temp1634;}); _LL1584: return(
struct _tagged_string)({ char* _temp1635=( char*)"<"; struct _tagged_string
_temp1636; _temp1636.curr= _temp1635; _temp1636.base= _temp1635; _temp1636.last_plus_one=
_temp1635 + 2; _temp1636;}); _LL1586: return( struct _tagged_string)({ char*
_temp1637=( char*)">="; struct _tagged_string _temp1638; _temp1638.curr=
_temp1637; _temp1638.base= _temp1637; _temp1638.last_plus_one= _temp1637 + 3;
_temp1638;}); _LL1588: return( struct _tagged_string)({ char* _temp1639=( char*)"<=";
struct _tagged_string _temp1640; _temp1640.curr= _temp1639; _temp1640.base=
_temp1639; _temp1640.last_plus_one= _temp1639 + 3; _temp1640;}); _LL1590: return(
struct _tagged_string)({ char* _temp1641=( char*)"!"; struct _tagged_string
_temp1642; _temp1642.curr= _temp1641; _temp1642.base= _temp1641; _temp1642.last_plus_one=
_temp1641 + 2; _temp1642;}); _LL1592: return( struct _tagged_string)({ char*
_temp1643=( char*)"~"; struct _tagged_string _temp1644; _temp1644.curr=
_temp1643; _temp1644.base= _temp1643; _temp1644.last_plus_one= _temp1643 + 2;
_temp1644;}); _LL1594: return( struct _tagged_string)({ char* _temp1645=( char*)"&";
struct _tagged_string _temp1646; _temp1646.curr= _temp1645; _temp1646.base=
_temp1645; _temp1646.last_plus_one= _temp1645 + 2; _temp1646;}); _LL1596: return(
struct _tagged_string)({ char* _temp1647=( char*)"|"; struct _tagged_string
_temp1648; _temp1648.curr= _temp1647; _temp1648.base= _temp1647; _temp1648.last_plus_one=
_temp1647 + 2; _temp1648;}); _LL1598: return( struct _tagged_string)({ char*
_temp1649=( char*)"^"; struct _tagged_string _temp1650; _temp1650.curr=
_temp1649; _temp1650.base= _temp1649; _temp1650.last_plus_one= _temp1649 + 2;
_temp1650;}); _LL1600: return( struct _tagged_string)({ char* _temp1651=( char*)"<<";
struct _tagged_string _temp1652; _temp1652.curr= _temp1651; _temp1652.base=
_temp1651; _temp1652.last_plus_one= _temp1651 + 3; _temp1652;}); _LL1602: return(
struct _tagged_string)({ char* _temp1653=( char*)">>"; struct _tagged_string
_temp1654; _temp1654.curr= _temp1653; _temp1654.base= _temp1653; _temp1654.last_plus_one=
_temp1653 + 3; _temp1654;}); _LL1604: return( struct _tagged_string)({ char*
_temp1655=( char*)">>>"; struct _tagged_string _temp1656; _temp1656.curr=
_temp1655; _temp1656.base= _temp1655; _temp1656.last_plus_one= _temp1655 + 4;
_temp1656;}); _LL1606: return( struct _tagged_string)({ char* _temp1657=( char*)"size";
struct _tagged_string _temp1658; _temp1658.curr= _temp1657; _temp1658.base=
_temp1657; _temp1658.last_plus_one= _temp1657 + 5; _temp1658;}); _LL1608: return(
struct _tagged_string)({ char* _temp1659=( char*)"printf"; struct _tagged_string
_temp1660; _temp1660.curr= _temp1659; _temp1660.base= _temp1659; _temp1660.last_plus_one=
_temp1659 + 7; _temp1660;}); _LL1610: return( struct _tagged_string)({ char*
_temp1661=( char*)"fprintf"; struct _tagged_string _temp1662; _temp1662.curr=
_temp1661; _temp1662.base= _temp1661; _temp1662.last_plus_one= _temp1661 + 8;
_temp1662;}); _LL1612: return( struct _tagged_string)({ char* _temp1663=( char*)"xprintf";
struct _tagged_string _temp1664; _temp1664.curr= _temp1663; _temp1664.base=
_temp1663; _temp1664.last_plus_one= _temp1663 + 8; _temp1664;}); _LL1614: return(
struct _tagged_string)({ char* _temp1665=( char*)"scanf"; struct _tagged_string
_temp1666; _temp1666.curr= _temp1665; _temp1666.base= _temp1665; _temp1666.last_plus_one=
_temp1665 + 6; _temp1666;}); _LL1616: return( struct _tagged_string)({ char*
_temp1667=( char*)"fscanf"; struct _tagged_string _temp1668; _temp1668.curr=
_temp1667; _temp1668.base= _temp1667; _temp1668.last_plus_one= _temp1667 + 7;
_temp1668;}); _LL1618: return( struct _tagged_string)({ char* _temp1669=( char*)"sscanf";
struct _tagged_string _temp1670; _temp1670.curr= _temp1669; _temp1670.base=
_temp1669; _temp1670.last_plus_one= _temp1669 + 7; _temp1670;}); _LL1566:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 ==
0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp1671=(* t).f1; if( _temp1671
== 0){ _throw( Null_Exception);} _temp1671->v;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1672=( char*)" = "; struct _tagged_string _temp1673;
_temp1673.curr= _temp1672; _temp1673.base= _temp1672; _temp1673.last_plus_one=
_temp1672 + 4; _temp1673;})), Cyc_Absynpp_exp2doc((* t).f2));}} struct Cyc_PP_Doc*
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
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1674=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1680; struct Cyc_Absyn_Decl* _temp1682;
_LL1676: if(( unsigned int) _temp1674 > 1u?*(( int*) _temp1674) == Cyc_Absyn_Decl_s:
0){ _LL1683: _temp1682=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1674)->f1; goto _LL1681; _LL1681: _temp1680=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1674)->f2; goto _LL1677;} else{ goto
_LL1678;} _LL1678: goto _LL1679; _LL1677: return 1; _LL1679: return 0; _LL1675:;}
struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc*
s;{ void* _temp1684=( void*) st->r; struct Cyc_Absyn_Exp* _temp1726; struct Cyc_Absyn_Stmt*
_temp1728; struct Cyc_Absyn_Stmt* _temp1730; struct Cyc_Absyn_Exp* _temp1732;
struct Cyc_Absyn_Stmt* _temp1734; struct Cyc_Absyn_Stmt* _temp1736; struct Cyc_Absyn_Exp*
_temp1738; struct Cyc_Absyn_Stmt* _temp1740; struct _tuple2 _temp1742; struct
Cyc_Absyn_Stmt* _temp1744; struct Cyc_Absyn_Exp* _temp1746; struct Cyc_Absyn_Stmt*
_temp1748; struct Cyc_Absyn_Stmt* _temp1750; struct Cyc_Absyn_Stmt* _temp1752;
struct _tagged_string* _temp1754; struct Cyc_Absyn_Stmt* _temp1756; struct
_tuple2 _temp1758; struct Cyc_Absyn_Stmt* _temp1760; struct Cyc_Absyn_Exp*
_temp1762; struct _tuple2 _temp1764; struct Cyc_Absyn_Stmt* _temp1766; struct
Cyc_Absyn_Exp* _temp1768; struct Cyc_Absyn_Exp* _temp1770; struct Cyc_List_List*
_temp1772; struct Cyc_Absyn_Exp* _temp1774; struct Cyc_Absyn_Switch_clause**
_temp1776; struct Cyc_List_List* _temp1778; struct Cyc_Absyn_Switch_clause**
_temp1780; struct Cyc_List_List* _temp1782; struct Cyc_Absyn_Stmt* _temp1784;
struct Cyc_Absyn_Decl* _temp1786; struct Cyc_Absyn_Stmt* _temp1788; struct Cyc_Absyn_Stmt*
_temp1790; struct Cyc_Absyn_Stmt* _temp1792; struct _tagged_string* _temp1794;
struct _tuple2 _temp1796; struct Cyc_Absyn_Stmt* _temp1798; struct Cyc_Absyn_Exp*
_temp1800; struct Cyc_Absyn_Stmt* _temp1802; struct Cyc_List_List* _temp1804;
struct Cyc_Absyn_Stmt* _temp1806; struct Cyc_Absyn_Stmt* _temp1808; struct Cyc_Absyn_Vardecl*
_temp1810; struct Cyc_Absyn_Tvar* _temp1812; _LL1686: if( _temp1684 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1687;} else{ goto _LL1688;} _LL1688: if((
unsigned int) _temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Exp_s: 0){
_LL1727: _temp1726=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1684)->f1; goto _LL1689;} else{ goto _LL1690;} _LL1690: if(( unsigned int)
_temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Seq_s: 0){ _LL1731: _temp1730=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1684)->f1; goto
_LL1729; _LL1729: _temp1728=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1684)->f2; goto _LL1691;} else{ goto _LL1692;} _LL1692: if(( unsigned int)
_temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Return_s: 0){ _LL1733:
_temp1732=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1684)->f1; goto _LL1693;} else{ goto _LL1694;} _LL1694: if(( unsigned int)
_temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_IfThenElse_s: 0){ _LL1739:
_temp1738=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1684)->f1; goto _LL1737; _LL1737: _temp1736=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1684)->f2; goto _LL1735; _LL1735:
_temp1734=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1684)->f3; goto _LL1695;} else{ goto _LL1696;} _LL1696: if(( unsigned int)
_temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_While_s: 0){ _LL1743: _temp1742=(
struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp1684)->f1; _LL1747:
_temp1746= _temp1742.f1; goto _LL1745; _LL1745: _temp1744= _temp1742.f2; goto
_LL1741; _LL1741: _temp1740=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1684)->f2; goto _LL1697;} else{ goto _LL1698;} _LL1698: if(( unsigned int)
_temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Break_s: 0){ _LL1749: _temp1748=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp1684)->f1; goto
_LL1699;} else{ goto _LL1700;} _LL1700: if(( unsigned int) _temp1684 > 1u?*((
int*) _temp1684) == Cyc_Absyn_Continue_s: 0){ _LL1751: _temp1750=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1684)->f1; goto _LL1701;} else{ goto
_LL1702;} _LL1702: if(( unsigned int) _temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Goto_s:
0){ _LL1755: _temp1754=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1684)->f1; goto _LL1753; _LL1753: _temp1752=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp1684)->f2; goto _LL1703;} else{ goto
_LL1704;} _LL1704: if(( unsigned int) _temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_For_s:
0){ _LL1771: _temp1770=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1684)->f1; goto _LL1765; _LL1765: _temp1764=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp1684)->f2; _LL1769: _temp1768= _temp1764.f1; goto _LL1767; _LL1767:
_temp1766= _temp1764.f2; goto _LL1759; _LL1759: _temp1758=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp1684)->f3; _LL1763: _temp1762= _temp1758.f1;
goto _LL1761; _LL1761: _temp1760= _temp1758.f2; goto _LL1757; _LL1757: _temp1756=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1684)->f4; goto
_LL1705;} else{ goto _LL1706;} _LL1706: if(( unsigned int) _temp1684 > 1u?*((
int*) _temp1684) == Cyc_Absyn_Switch_s: 0){ _LL1775: _temp1774=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1684)->f1; goto _LL1773; _LL1773:
_temp1772=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1684)->f2; goto _LL1707;} else{ goto _LL1708;} _LL1708: if(( unsigned int)
_temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Fallthru_s: 0){ _LL1779:
_temp1778=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1684)->f1; if( _temp1778 == 0){ goto _LL1777;} else{ goto _LL1710;} _LL1777:
_temp1776=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1684)->f2; goto _LL1709;} else{ goto _LL1710;} _LL1710: if(( unsigned int)
_temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Fallthru_s: 0){ _LL1783:
_temp1782=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1684)->f1; goto _LL1781; _LL1781: _temp1780=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1684)->f2; goto _LL1711;} else{ goto
_LL1712;} _LL1712: if(( unsigned int) _temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Decl_s:
0){ _LL1787: _temp1786=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1684)->f1; goto _LL1785; _LL1785: _temp1784=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1684)->f2; goto _LL1713;} else{ goto
_LL1714;} _LL1714: if(( unsigned int) _temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Cut_s:
0){ _LL1789: _temp1788=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp1684)->f1; goto _LL1715;} else{ goto _LL1716;} _LL1716: if(( unsigned int)
_temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Splice_s: 0){ _LL1791:
_temp1790=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp1684)->f1; goto _LL1717;} else{ goto _LL1718;} _LL1718: if(( unsigned int)
_temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Label_s: 0){ _LL1795: _temp1794=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1684)->f1; goto
_LL1793; _LL1793: _temp1792=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1684)->f2; goto _LL1719;} else{ goto _LL1720;} _LL1720: if(( unsigned int)
_temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Do_s: 0){ _LL1803: _temp1802=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp1684)->f1; goto
_LL1797; _LL1797: _temp1796=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp1684)->f2; _LL1801: _temp1800= _temp1796.f1; goto _LL1799; _LL1799:
_temp1798= _temp1796.f2; goto _LL1721;} else{ goto _LL1722;} _LL1722: if((
unsigned int) _temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_TryCatch_s: 0){
_LL1807: _temp1806=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1684)->f1; goto _LL1805; _LL1805: _temp1804=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1684)->f2; goto _LL1723;} else{ goto
_LL1724;} _LL1724: if(( unsigned int) _temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Region_s:
0){ _LL1813: _temp1812=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1684)->f1; goto _LL1811; _LL1811: _temp1810=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1684)->f2; goto _LL1809; _LL1809:
_temp1808=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1684)->f3; goto _LL1725;} else{ goto _LL1685;} _LL1687: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp1814=( char*)";"; struct _tagged_string
_temp1815; _temp1815.curr= _temp1814; _temp1815.base= _temp1814; _temp1815.last_plus_one=
_temp1814 + 2; _temp1815;})); goto _LL1685; _LL1689: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc(
_temp1726), Cyc_PP_text(( struct _tagged_string)({ char* _temp1816=( char*)";";
struct _tagged_string _temp1817; _temp1817.curr= _temp1816; _temp1817.base=
_temp1816; _temp1817.last_plus_one= _temp1816 + 2; _temp1817;}))); goto _LL1685;
_LL1691: if( Cyc_Absynpp_decls_first){ if( Cyc_Absynpp_is_declaration( _temp1730)){
s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({ char* _temp1818=(
char*)"{ "; struct _tagged_string _temp1819; _temp1819.curr= _temp1818;
_temp1819.base= _temp1818; _temp1819.last_plus_one= _temp1818 + 3; _temp1819;})),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1730)), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1820=( char*)"}"; struct _tagged_string
_temp1821; _temp1821.curr= _temp1820; _temp1821.base= _temp1820; _temp1821.last_plus_one=
_temp1820 + 2; _temp1821;})), Cyc_PP_line_doc(), Cyc_Absynpp_is_declaration(
_temp1728)? Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1822=( char*)"{ "; struct _tagged_string _temp1823; _temp1823.curr=
_temp1822; _temp1823.base= _temp1822; _temp1823.last_plus_one= _temp1822 + 3;
_temp1823;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1728)), Cyc_PP_text((
struct _tagged_string)({ char* _temp1824=( char*)"}"; struct _tagged_string
_temp1825; _temp1825.curr= _temp1824; _temp1825.base= _temp1824; _temp1825.last_plus_one=
_temp1824 + 2; _temp1825;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1728));} else{ if( Cyc_Absynpp_is_declaration( _temp1728)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1730), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1826=( char*)"{ "; struct _tagged_string _temp1827;
_temp1827.curr= _temp1826; _temp1827.base= _temp1826; _temp1827.last_plus_one=
_temp1826 + 3; _temp1827;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1728)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1828=( char*)"}"; struct
_tagged_string _temp1829; _temp1829.curr= _temp1828; _temp1829.base= _temp1828;
_temp1829.last_plus_one= _temp1828 + 2; _temp1829;})), Cyc_PP_line_doc());}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,(
struct _tagged_string)({ char* _temp1830=( char*)""; struct _tagged_string
_temp1831; _temp1831.curr= _temp1830; _temp1831.base= _temp1830; _temp1831.last_plus_one=
_temp1830 + 1; _temp1831;}),({ struct Cyc_List_List* _temp1832=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1832->hd=( void*) _temp1730;
_temp1832->tl=({ struct Cyc_List_List* _temp1833=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1833->hd=( void*) _temp1728;
_temp1833->tl= 0; _temp1833;}); _temp1832;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,( struct
_tagged_string)({ char* _temp1834=( char*)""; struct _tagged_string _temp1835;
_temp1835.curr= _temp1834; _temp1835.base= _temp1834; _temp1835.last_plus_one=
_temp1834 + 1; _temp1835;}),({ struct Cyc_List_List* _temp1836=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1836->hd=( void*) _temp1730;
_temp1836->tl=({ struct Cyc_List_List* _temp1837=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1837->hd=( void*) _temp1728;
_temp1837->tl= 0; _temp1837;}); _temp1836;}));} goto _LL1685; _LL1693: if(
_temp1732 == 0){ s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1838=(
char*)"return;"; struct _tagged_string _temp1839; _temp1839.curr= _temp1838;
_temp1839.base= _temp1838; _temp1839.last_plus_one= _temp1838 + 8; _temp1839;}));}
else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1840=( char*)"return "; struct _tagged_string _temp1841; _temp1841.curr=
_temp1840; _temp1841.base= _temp1840; _temp1841.last_plus_one= _temp1840 + 8;
_temp1841;})), _temp1732 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1842= _temp1732; if( _temp1842 == 0){
_throw( Null_Exception);} _temp1842;})), Cyc_PP_text(( struct _tagged_string)({
char* _temp1843=( char*)";"; struct _tagged_string _temp1844; _temp1844.curr=
_temp1843; _temp1844.base= _temp1843; _temp1844.last_plus_one= _temp1843 + 2;
_temp1844;})));} goto _LL1685; _LL1695: { int print_else;{ void* _temp1845=(
void*) _temp1734->r; _LL1847: if( _temp1845 ==( void*) Cyc_Absyn_Skip_s){ goto
_LL1848;} else{ goto _LL1849;} _LL1849: goto _LL1850; _LL1848: print_else= 0;
goto _LL1846; _LL1850: print_else= 1; goto _LL1846; _LL1846:;} s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1851=( char*)"if ("; struct
_tagged_string _temp1852; _temp1852.curr= _temp1851; _temp1852.base= _temp1851;
_temp1852.last_plus_one= _temp1851 + 5; _temp1852;})), Cyc_Absynpp_exp2doc(
_temp1738), Cyc_PP_text(( struct _tagged_string)({ char* _temp1853=( char*)") {";
struct _tagged_string _temp1854; _temp1854.curr= _temp1853; _temp1854.base=
_temp1853; _temp1854.last_plus_one= _temp1853 + 4; _temp1854;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1736))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1855=( char*)"}"; struct
_tagged_string _temp1856; _temp1856.curr= _temp1855; _temp1856.base= _temp1855;
_temp1856.last_plus_one= _temp1855 + 2; _temp1856;})), print_else? Cyc_Absynpp_cat5(
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1857=( char*)"else {";
struct _tagged_string _temp1858; _temp1858.curr= _temp1857; _temp1858.base=
_temp1857; _temp1858.last_plus_one= _temp1857 + 7; _temp1858;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1734))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1859=( char*)"}"; struct
_tagged_string _temp1860; _temp1860.curr= _temp1859; _temp1860.base= _temp1859;
_temp1860.last_plus_one= _temp1859 + 2; _temp1860;}))): Cyc_PP_nil_doc()); goto
_LL1685;} _LL1697: s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({
char* _temp1861=( char*)"while ("; struct _tagged_string _temp1862; _temp1862.curr=
_temp1861; _temp1862.base= _temp1861; _temp1862.last_plus_one= _temp1861 + 8;
_temp1862;})), Cyc_Absynpp_exp2doc( _temp1746), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1863=( char*)") {"; struct _tagged_string _temp1864;
_temp1864.curr= _temp1863; _temp1864.base= _temp1863; _temp1864.last_plus_one=
_temp1863 + 4; _temp1864;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1740))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1865=( char*)"}"; struct _tagged_string _temp1866;
_temp1866.curr= _temp1865; _temp1866.base= _temp1865; _temp1866.last_plus_one=
_temp1865 + 2; _temp1866;}))); goto _LL1685; _LL1699: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1867=( char*)"break;"; struct _tagged_string
_temp1868; _temp1868.curr= _temp1867; _temp1868.base= _temp1867; _temp1868.last_plus_one=
_temp1867 + 7; _temp1868;})); goto _LL1685; _LL1701: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1869=( char*)"continue;"; struct _tagged_string
_temp1870; _temp1870.curr= _temp1869; _temp1870.base= _temp1869; _temp1870.last_plus_one=
_temp1869 + 10; _temp1870;})); goto _LL1685; _LL1703: s= Cyc_PP_text(({ struct
_tagged_string _temp1871=* _temp1754; xprintf("goto %.*s;", _temp1871.last_plus_one
- _temp1871.curr, _temp1871.curr);})); goto _LL1685; _LL1705: s= Cyc_Absynpp_cat10(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1872=( char*)"for("; struct
_tagged_string _temp1873; _temp1873.curr= _temp1872; _temp1873.base= _temp1872;
_temp1873.last_plus_one= _temp1872 + 5; _temp1873;})), Cyc_Absynpp_exp2doc(
_temp1770), Cyc_PP_text(( struct _tagged_string)({ char* _temp1874=( char*)"; ";
struct _tagged_string _temp1875; _temp1875.curr= _temp1874; _temp1875.base=
_temp1874; _temp1875.last_plus_one= _temp1874 + 3; _temp1875;})), Cyc_Absynpp_exp2doc(
_temp1768), Cyc_PP_text(( struct _tagged_string)({ char* _temp1876=( char*)"; ";
struct _tagged_string _temp1877; _temp1877.curr= _temp1876; _temp1877.base=
_temp1876; _temp1877.last_plus_one= _temp1876 + 3; _temp1877;})), Cyc_Absynpp_exp2doc(
_temp1762), Cyc_PP_text(( struct _tagged_string)({ char* _temp1878=( char*)") {";
struct _tagged_string _temp1879; _temp1879.curr= _temp1878; _temp1879.base=
_temp1878; _temp1879.last_plus_one= _temp1878 + 4; _temp1879;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1756))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1880=( char*)"}"; struct
_tagged_string _temp1881; _temp1881.curr= _temp1880; _temp1881.base= _temp1880;
_temp1881.last_plus_one= _temp1880 + 2; _temp1881;}))); goto _LL1685; _LL1707: s=
Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char* _temp1882=( char*)"switch (";
struct _tagged_string _temp1883; _temp1883.curr= _temp1882; _temp1883.base=
_temp1882; _temp1883.last_plus_one= _temp1882 + 9; _temp1883;})), Cyc_Absynpp_exp2doc(
_temp1774), Cyc_PP_text(( struct _tagged_string)({ char* _temp1884=( char*)") {";
struct _tagged_string _temp1885; _temp1885.curr= _temp1884; _temp1885.base=
_temp1884; _temp1885.last_plus_one= _temp1884 + 4; _temp1885;})), Cyc_PP_line_doc(),
Cyc_Absynpp_switchclauses2doc( _temp1772), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1886=( char*)"}"; struct _tagged_string
_temp1887; _temp1887.curr= _temp1886; _temp1887.base= _temp1886; _temp1887.last_plus_one=
_temp1886 + 2; _temp1887;}))); goto _LL1685; _LL1709: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1888=( char*)"fallthru;"; struct _tagged_string
_temp1889; _temp1889.curr= _temp1888; _temp1889.base= _temp1888; _temp1889.last_plus_one=
_temp1888 + 10; _temp1889;})); goto _LL1685; _LL1711: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1890=( char*)"fallthru("; struct
_tagged_string _temp1891; _temp1891.curr= _temp1890; _temp1891.base= _temp1890;
_temp1891.last_plus_one= _temp1890 + 10; _temp1891;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1782), Cyc_PP_text(( struct _tagged_string)({ char* _temp1892=( char*)");";
struct _tagged_string _temp1893; _temp1893.curr= _temp1892; _temp1893.base=
_temp1892; _temp1893.last_plus_one= _temp1892 + 3; _temp1893;}))); goto _LL1685;
_LL1713: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1786), Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1784)); goto _LL1685; _LL1715: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1894=( char*)"cut "; struct
_tagged_string _temp1895; _temp1895.curr= _temp1894; _temp1895.base= _temp1894;
_temp1895.last_plus_one= _temp1894 + 5; _temp1895;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1788))); goto _LL1685; _LL1717: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1896=( char*)"splice "; struct _tagged_string
_temp1897; _temp1897.curr= _temp1896; _temp1897.base= _temp1896; _temp1897.last_plus_one=
_temp1896 + 8; _temp1897;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1790)));
goto _LL1685; _LL1719: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1792): 0){ s= Cyc_Absynpp_cat6( Cyc_Absynpp_textptr( _temp1794), Cyc_PP_text((
struct _tagged_string)({ char* _temp1898=( char*)": {"; struct _tagged_string
_temp1899; _temp1899.curr= _temp1898; _temp1899.base= _temp1898; _temp1899.last_plus_one=
_temp1898 + 4; _temp1899;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1792)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1900=( char*)"}"; struct _tagged_string _temp1901; _temp1901.curr=
_temp1900; _temp1901.base= _temp1900; _temp1901.last_plus_one= _temp1900 + 2;
_temp1901;})));} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( _temp1794), Cyc_PP_text((
struct _tagged_string)({ char* _temp1902=( char*)": "; struct _tagged_string
_temp1903; _temp1903.curr= _temp1902; _temp1903.base= _temp1902; _temp1903.last_plus_one=
_temp1902 + 3; _temp1903;})), Cyc_Absynpp_stmt2doc( _temp1792));} goto _LL1685;
_LL1721: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1904=( char*)"do {"; struct _tagged_string _temp1905; _temp1905.curr=
_temp1904; _temp1905.base= _temp1904; _temp1905.last_plus_one= _temp1904 + 5;
_temp1905;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1802)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1906=( char*)"} while ("; struct _tagged_string _temp1907; _temp1907.curr=
_temp1906; _temp1907.base= _temp1906; _temp1907.last_plus_one= _temp1906 + 10;
_temp1907;})), Cyc_Absynpp_exp2doc( _temp1800), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1908=( char*)");"; struct _tagged_string _temp1909;
_temp1909.curr= _temp1908; _temp1909.base= _temp1908; _temp1909.last_plus_one=
_temp1908 + 3; _temp1909;}))); goto _LL1685; _LL1723: s= Cyc_Absynpp_cat9( Cyc_PP_text((
struct _tagged_string)({ char* _temp1910=( char*)"try {"; struct _tagged_string
_temp1911; _temp1911.curr= _temp1910; _temp1911.base= _temp1910; _temp1911.last_plus_one=
_temp1910 + 6; _temp1911;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1806)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1912=( char*)"} catch {"; struct _tagged_string _temp1913; _temp1913.curr=
_temp1912; _temp1913.base= _temp1912; _temp1913.last_plus_one= _temp1912 + 10;
_temp1913;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1804)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1914=( char*)"}"; struct _tagged_string _temp1915; _temp1915.curr=
_temp1914; _temp1915.base= _temp1914; _temp1915.last_plus_one= _temp1914 + 2;
_temp1915;}))); goto _LL1685; _LL1725: s= Cyc_Absynpp_cat9( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1916=( char*)"region<"; struct _tagged_string
_temp1917; _temp1917.curr= _temp1916; _temp1917.base= _temp1916; _temp1917.last_plus_one=
_temp1916 + 8; _temp1917;})), Cyc_Absynpp_textptr( Cyc_Absynpp_get_name(
_temp1812)), Cyc_PP_text(( struct _tagged_string)({ char* _temp1918=( char*)">";
struct _tagged_string _temp1919; _temp1919.curr= _temp1918; _temp1919.base=
_temp1918; _temp1919.last_plus_one= _temp1918 + 2; _temp1919;})), Cyc_Absynpp_qvar2doc(
_temp1810->name), Cyc_PP_text(( struct _tagged_string)({ char* _temp1920=( char*)"{";
struct _tagged_string _temp1921; _temp1921.curr= _temp1920; _temp1921.base=
_temp1920; _temp1921.last_plus_one= _temp1920 + 2; _temp1921;})), Cyc_PP_line_doc(),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1808)), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1922=( char*)"}"; struct _tagged_string
_temp1923; _temp1923.curr= _temp1922; _temp1923.base= _temp1922; _temp1923.last_plus_one=
_temp1922 + 2; _temp1923;}))); goto _LL1685; _LL1685:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_pat2doc( struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void*
_temp1924=( void*) p->r; int _temp1956; void* _temp1958; char _temp1960; struct
_tagged_string _temp1962; struct Cyc_Absyn_Vardecl* _temp1964; struct Cyc_List_List*
_temp1966; struct Cyc_Absyn_Pat* _temp1968; struct Cyc_Absyn_Vardecl* _temp1970;
struct _tuple0* _temp1972; struct Cyc_List_List* _temp1974; struct Cyc_List_List*
_temp1976; struct _tuple0* _temp1978; struct Cyc_List_List* _temp1980; struct
Cyc_List_List* _temp1982; struct _tuple0* _temp1984; struct Cyc_List_List*
_temp1986; struct Cyc_List_List* _temp1988; struct Cyc_Core_Opt* _temp1990;
struct Cyc_Absyn_Structdecl* _temp1992; struct Cyc_Absyn_Enumfield* _temp1994;
struct Cyc_Absyn_Enumdecl* _temp1996; struct Cyc_List_List* _temp1998; struct
Cyc_List_List* _temp2000; struct Cyc_Absyn_Tunionfield* _temp2002; struct Cyc_Absyn_Tuniondecl*
_temp2004; _LL1926: if( _temp1924 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1927;}
else{ goto _LL1928;} _LL1928: if( _temp1924 ==( void*) Cyc_Absyn_Null_p){ goto
_LL1929;} else{ goto _LL1930;} _LL1930: if(( unsigned int) _temp1924 > 2u?*((
int*) _temp1924) == Cyc_Absyn_Int_p: 0){ _LL1959: _temp1958=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1924)->f1; goto _LL1957; _LL1957: _temp1956=( int)((
struct Cyc_Absyn_Int_p_struct*) _temp1924)->f2; goto _LL1931;} else{ goto
_LL1932;} _LL1932: if(( unsigned int) _temp1924 > 2u?*(( int*) _temp1924) == Cyc_Absyn_Char_p:
0){ _LL1961: _temp1960=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1924)->f1;
goto _LL1933;} else{ goto _LL1934;} _LL1934: if(( unsigned int) _temp1924 > 2u?*((
int*) _temp1924) == Cyc_Absyn_Float_p: 0){ _LL1963: _temp1962=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1924)->f1; goto _LL1935;}
else{ goto _LL1936;} _LL1936: if(( unsigned int) _temp1924 > 2u?*(( int*)
_temp1924) == Cyc_Absyn_Var_p: 0){ _LL1965: _temp1964=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1924)->f1; goto _LL1937;} else{ goto
_LL1938;} _LL1938: if(( unsigned int) _temp1924 > 2u?*(( int*) _temp1924) == Cyc_Absyn_Tuple_p:
0){ _LL1967: _temp1966=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1924)->f1; goto _LL1939;} else{ goto _LL1940;} _LL1940: if(( unsigned int)
_temp1924 > 2u?*(( int*) _temp1924) == Cyc_Absyn_Pointer_p: 0){ _LL1969:
_temp1968=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1924)->f1; goto _LL1941;} else{ goto _LL1942;} _LL1942: if(( unsigned int)
_temp1924 > 2u?*(( int*) _temp1924) == Cyc_Absyn_Reference_p: 0){ _LL1971:
_temp1970=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1924)->f1; goto _LL1943;} else{ goto _LL1944;} _LL1944: if(( unsigned int)
_temp1924 > 2u?*(( int*) _temp1924) == Cyc_Absyn_UnknownId_p: 0){ _LL1973:
_temp1972=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1924)->f1;
goto _LL1945;} else{ goto _LL1946;} _LL1946: if(( unsigned int) _temp1924 > 2u?*((
int*) _temp1924) == Cyc_Absyn_UnknownCall_p: 0){ _LL1979: _temp1978=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1924)->f1; goto _LL1977;
_LL1977: _temp1976=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1924)->f2; goto _LL1975; _LL1975: _temp1974=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1924)->f3; goto _LL1947;} else{
goto _LL1948;} _LL1948: if(( unsigned int) _temp1924 > 2u?*(( int*) _temp1924)
== Cyc_Absyn_UnknownFields_p: 0){ _LL1985: _temp1984=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp1924)->f1; goto _LL1983; _LL1983:
_temp1982=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1924)->f2; goto _LL1981; _LL1981: _temp1980=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1924)->f3; goto _LL1949;} else{
goto _LL1950;} _LL1950: if(( unsigned int) _temp1924 > 2u?*(( int*) _temp1924)
== Cyc_Absyn_Struct_p: 0){ _LL1993: _temp1992=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1924)->f1; goto _LL1991; _LL1991:
_temp1990=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1924)->f2;
goto _LL1989; _LL1989: _temp1988=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1924)->f3; goto _LL1987; _LL1987: _temp1986=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1924)->f4; goto _LL1951;} else{ goto
_LL1952;} _LL1952: if(( unsigned int) _temp1924 > 2u?*(( int*) _temp1924) == Cyc_Absyn_Enum_p:
0){ _LL1997: _temp1996=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1924)->f1; goto _LL1995; _LL1995: _temp1994=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1924)->f2; goto _LL1953;} else{ goto
_LL1954;} _LL1954: if(( unsigned int) _temp1924 > 2u?*(( int*) _temp1924) == Cyc_Absyn_Tunion_p:
0){ _LL2005: _temp2004=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1924)->f1; goto _LL2003; _LL2003: _temp2002=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1924)->f2; goto _LL2001; _LL2001:
_temp2000=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1924)->f3; goto _LL1999; _LL1999: _temp1998=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1924)->f4; goto _LL1955;} else{ goto
_LL1925;} _LL1927: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp2006=(
char*)"_"; struct _tagged_string _temp2007; _temp2007.curr= _temp2006; _temp2007.base=
_temp2006; _temp2007.last_plus_one= _temp2006 + 2; _temp2007;})); goto _LL1925;
_LL1929: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp2008=( char*)"null";
struct _tagged_string _temp2009; _temp2009.curr= _temp2008; _temp2009.base=
_temp2008; _temp2009.last_plus_one= _temp2008 + 5; _temp2009;})); goto _LL1925;
_LL1931: if( _temp1958 ==( void*) Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d",
_temp1956));} else{ s= Cyc_PP_text( xprintf("%u",( unsigned int) _temp1956));}
goto _LL1925; _LL1933: s= Cyc_PP_text(({ struct _tagged_string _temp2010= Cyc_Absynpp_char_escape(
_temp1960); xprintf("'%.*s'", _temp2010.last_plus_one - _temp2010.curr,
_temp2010.curr);})); goto _LL1925; _LL1935: s= Cyc_PP_text( _temp1962); goto
_LL1925; _LL1937: s= Cyc_Absynpp_qvar2doc( _temp1964->name); goto _LL1925;
_LL1939: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2011=( char*)"$("; struct _tagged_string _temp2012; _temp2012.curr=
_temp2011; _temp2012.base= _temp2011; _temp2012.last_plus_one= _temp2011 + 3;
_temp2012;})),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,(
struct _tagged_string)({ char* _temp2013=( char*)","; struct _tagged_string
_temp2014; _temp2014.curr= _temp2013; _temp2014.base= _temp2013; _temp2014.last_plus_one=
_temp2013 + 2; _temp2014;}), _temp1966), Cyc_PP_text(( struct _tagged_string)({
char* _temp2015=( char*)")"; struct _tagged_string _temp2016; _temp2016.curr=
_temp2015; _temp2016.base= _temp2015; _temp2016.last_plus_one= _temp2015 + 2;
_temp2016;}))); goto _LL1925; _LL1941: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2017=( char*)"&"; struct _tagged_string _temp2018;
_temp2018.curr= _temp2017; _temp2018.base= _temp2017; _temp2018.last_plus_one=
_temp2017 + 2; _temp2018;})), Cyc_Absynpp_pat2doc( _temp1968)); goto _LL1925;
_LL1943: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2019=( char*)"*"; struct _tagged_string _temp2020; _temp2020.curr=
_temp2019; _temp2020.base= _temp2019; _temp2020.last_plus_one= _temp2019 + 2;
_temp2020;})), Cyc_Absynpp_qvar2doc( _temp1970->name)); goto _LL1925; _LL1945: s=
Cyc_Absynpp_qvar2doc( _temp1972); goto _LL1925; _LL1947: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1978), Cyc_Absynpp_tvars2doc( _temp1976), Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp2021=( char*)"("; struct _tagged_string
_temp2022; _temp2022.curr= _temp2021; _temp2022.base= _temp2021; _temp2022.last_plus_one=
_temp2021 + 2; _temp2022;}),( struct _tagged_string)({ char* _temp2023=( char*)")";
struct _tagged_string _temp2024; _temp2024.curr= _temp2023; _temp2024.base=
_temp2023; _temp2024.last_plus_one= _temp2023 + 2; _temp2024;}),( struct
_tagged_string)({ char* _temp2025=( char*)","; struct _tagged_string _temp2026;
_temp2026.curr= _temp2025; _temp2026.base= _temp2025; _temp2026.last_plus_one=
_temp2025 + 2; _temp2026;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1974))); goto _LL1925; _LL1949: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1984), Cyc_Absynpp_tvars2doc( _temp1982), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp2027=( char*)"{"; struct _tagged_string _temp2028;
_temp2028.curr= _temp2027; _temp2028.base= _temp2027; _temp2028.last_plus_one=
_temp2027 + 2; _temp2028;}),( struct _tagged_string)({ char* _temp2029=( char*)"}";
struct _tagged_string _temp2030; _temp2030.curr= _temp2029; _temp2030.base=
_temp2029; _temp2030.last_plus_one= _temp2029 + 2; _temp2030;}),( struct
_tagged_string)({ char* _temp2031=( char*)","; struct _tagged_string _temp2032;
_temp2032.curr= _temp2031; _temp2032.base= _temp2031; _temp2032.last_plus_one=
_temp2031 + 2; _temp2032;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1980))); goto _LL1925; _LL1951: s= Cyc_Absynpp_cat3( _temp1992->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp2033= _temp1992->name; if( _temp2033 == 0){ _throw( Null_Exception);}
_temp2033->v;})), Cyc_Absynpp_tvars2doc( _temp1988), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp2034=( char*)"{"; struct _tagged_string _temp2035;
_temp2035.curr= _temp2034; _temp2035.base= _temp2034; _temp2035.last_plus_one=
_temp2034 + 2; _temp2035;}),( struct _tagged_string)({ char* _temp2036=( char*)"}";
struct _tagged_string _temp2037; _temp2037.curr= _temp2036; _temp2037.base=
_temp2036; _temp2037.last_plus_one= _temp2036 + 2; _temp2037;}),( struct
_tagged_string)({ char* _temp2038=( char*)","; struct _tagged_string _temp2039;
_temp2039.curr= _temp2038; _temp2039.base= _temp2038; _temp2039.last_plus_one=
_temp2038 + 2; _temp2039;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1986))); goto _LL1925; _LL1953: s= Cyc_Absynpp_qvar2doc( _temp1994->name);
goto _LL1925; _LL1955: if( _temp1998 == 0){ s= Cyc_Absynpp_qvar2doc( _temp2002->name);}
else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp2002->name), Cyc_Absynpp_tvars2doc(
_temp2000), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp2040=( char*)"(";
struct _tagged_string _temp2041; _temp2041.curr= _temp2040; _temp2041.base=
_temp2040; _temp2041.last_plus_one= _temp2040 + 2; _temp2041;}),( struct
_tagged_string)({ char* _temp2042=( char*)")"; struct _tagged_string _temp2043;
_temp2043.curr= _temp2042; _temp2043.base= _temp2042; _temp2043.last_plus_one=
_temp2042 + 2; _temp2043;}),( struct _tagged_string)({ char* _temp2044=( char*)",";
struct _tagged_string _temp2045; _temp2045.curr= _temp2044; _temp2045.base=
_temp2044; _temp2045.last_plus_one= _temp2044 + 2; _temp2045;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1998)));} goto _LL1925; _LL1925:;} return s;} struct
Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return Cyc_Absynpp_cat2(
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp2046=( char*)""; struct
_tagged_string _temp2047; _temp2047.curr= _temp2046; _temp2047.base= _temp2046;
_temp2047.last_plus_one= _temp2046 + 1; _temp2047;}),( struct _tagged_string)({
char* _temp2048=( char*)"="; struct _tagged_string _temp2049; _temp2049.curr=
_temp2048; _temp2049.base= _temp2048; _temp2049.last_plus_one= _temp2048 + 2;
_temp2049;}),( struct _tagged_string)({ char* _temp2050=( char*)"="; struct
_tagged_string _temp2051; _temp2051.curr= _temp2050; _temp2051.base= _temp2050;
_temp2051.last_plus_one= _temp2050 + 2; _temp2051;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)), Cyc_Absynpp_pat2doc((* dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
==( void*) Cyc_Absyn_Wild_p: 0){ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2052=( char*)"default: "; struct _tagged_string
_temp2053; _temp2053.curr= _temp2052; _temp2053.base= _temp2052; _temp2053.last_plus_one=
_temp2052 + 10; _temp2053;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause == 0){ return Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2054=( char*)"case "; struct
_tagged_string _temp2055; _temp2055.curr= _temp2054; _temp2055.base= _temp2054;
_temp2055.last_plus_one= _temp2054 + 6; _temp2055;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2056=( char*)": "; struct
_tagged_string _temp2057; _temp2057.curr= _temp2056; _temp2057.base= _temp2056;
_temp2057.last_plus_one= _temp2056 + 3; _temp2057;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ return Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2058=( char*)"case "; struct
_tagged_string _temp2059; _temp2059.curr= _temp2058; _temp2059.base= _temp2058;
_temp2059.last_plus_one= _temp2058 + 6; _temp2059;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2060=( char*)" && "; struct
_tagged_string _temp2061; _temp2061.curr= _temp2060; _temp2061.base= _temp2060;
_temp2061.last_plus_one= _temp2060 + 5; _temp2061;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2062= c->where_clause; if(
_temp2062 == 0){ _throw( Null_Exception);} _temp2062;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2063=( char*)": "; struct _tagged_string _temp2064;
_temp2064.curr= _temp2063; _temp2064.base= _temp2063; _temp2064.last_plus_one=
_temp2063 + 3; _temp2064;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,( struct _tagged_string)({
char* _temp2065=( char*)""; struct _tagged_string _temp2066; _temp2066.curr=
_temp2065; _temp2066.base= _temp2065; _temp2066.last_plus_one= _temp2065 + 1;
_temp2066;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield*
f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);} else{ return Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( f->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2067=( char*)" = "; struct _tagged_string _temp2068; _temp2068.curr=
_temp2067; _temp2068.base= _temp2067; _temp2068.last_plus_one= _temp2067 + 4;
_temp2068;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2069= f->tag; if( _temp2069 == 0){ _throw( Null_Exception);} _temp2069;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,(
struct _tagged_string)({ char* _temp2070=( char*)","; struct _tagged_string
_temp2071; _temp2071.curr= _temp2070; _temp2071.base= _temp2070; _temp2071.last_plus_one=
_temp2070 + 2; _temp2071;}), fs);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2072=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2096; struct Cyc_Absyn_Structdecl* _temp2098;
struct Cyc_Absyn_Uniondecl* _temp2100; struct Cyc_Absyn_Vardecl* _temp2102;
struct Cyc_Absyn_Vardecl _temp2104; struct Cyc_List_List* _temp2105; struct Cyc_Core_Opt*
_temp2107; struct Cyc_Absyn_Exp* _temp2109; void* _temp2111; struct Cyc_Absyn_Tqual
_temp2113; struct _tuple0* _temp2115; void* _temp2117; struct Cyc_Absyn_Tuniondecl*
_temp2119; struct Cyc_Absyn_Tuniondecl _temp2121; int _temp2122; struct Cyc_Core_Opt*
_temp2124; struct Cyc_List_List* _temp2126; struct _tuple0* _temp2128; void*
_temp2130; int _temp2132; struct Cyc_Absyn_Exp* _temp2134; struct Cyc_Core_Opt*
_temp2136; struct Cyc_Core_Opt* _temp2138; struct Cyc_Absyn_Pat* _temp2140;
struct Cyc_Absyn_Enumdecl* _temp2142; struct Cyc_Absyn_Enumdecl _temp2144;
struct Cyc_Core_Opt* _temp2145; struct _tuple0* _temp2147; void* _temp2149;
struct Cyc_Absyn_Typedefdecl* _temp2151; struct Cyc_List_List* _temp2153; struct
_tagged_string* _temp2155; struct Cyc_List_List* _temp2157; struct _tuple0*
_temp2159; struct Cyc_List_List* _temp2161; _LL2074: if(*(( int*) _temp2072) ==
Cyc_Absyn_Fn_d){ _LL2097: _temp2096=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp2072)->f1; goto _LL2075;} else{ goto _LL2076;} _LL2076: if(*(( int*)
_temp2072) == Cyc_Absyn_Struct_d){ _LL2099: _temp2098=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp2072)->f1; goto _LL2077;} else{ goto
_LL2078;} _LL2078: if(*(( int*) _temp2072) == Cyc_Absyn_Union_d){ _LL2101:
_temp2100=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp2072)->f1; goto _LL2079;} else{ goto _LL2080;} _LL2080: if(*(( int*)
_temp2072) == Cyc_Absyn_Var_d){ _LL2103: _temp2102=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp2072)->f1; _temp2104=* _temp2102; _LL2118:
_temp2117=( void*) _temp2104.sc; goto _LL2116; _LL2116: _temp2115=( struct
_tuple0*) _temp2104.name; goto _LL2114; _LL2114: _temp2113=( struct Cyc_Absyn_Tqual)
_temp2104.tq; goto _LL2112; _LL2112: _temp2111=( void*) _temp2104.type; goto
_LL2110; _LL2110: _temp2109=( struct Cyc_Absyn_Exp*) _temp2104.initializer; goto
_LL2108; _LL2108: _temp2107=( struct Cyc_Core_Opt*) _temp2104.rgn; goto _LL2106;
_LL2106: _temp2105=( struct Cyc_List_List*) _temp2104.attributes; goto _LL2081;}
else{ goto _LL2082;} _LL2082: if(*(( int*) _temp2072) == Cyc_Absyn_Tunion_d){
_LL2120: _temp2119=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2072)->f1; _temp2121=* _temp2119; _LL2131: _temp2130=( void*) _temp2121.sc;
goto _LL2129; _LL2129: _temp2128=( struct _tuple0*) _temp2121.name; goto _LL2127;
_LL2127: _temp2126=( struct Cyc_List_List*) _temp2121.tvs; goto _LL2125; _LL2125:
_temp2124=( struct Cyc_Core_Opt*) _temp2121.fields; goto _LL2123; _LL2123:
_temp2122=( int) _temp2121.is_xtunion; goto _LL2083;} else{ goto _LL2084;}
_LL2084: if(*(( int*) _temp2072) == Cyc_Absyn_Let_d){ _LL2141: _temp2140=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2072)->f1; goto
_LL2139; _LL2139: _temp2138=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2072)->f2; goto _LL2137; _LL2137: _temp2136=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2072)->f3; goto _LL2135; _LL2135: _temp2134=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2072)->f4; goto
_LL2133; _LL2133: _temp2132=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2072)->f5;
goto _LL2085;} else{ goto _LL2086;} _LL2086: if(*(( int*) _temp2072) == Cyc_Absyn_Enum_d){
_LL2143: _temp2142=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp2072)->f1; _temp2144=* _temp2142; _LL2150: _temp2149=( void*) _temp2144.sc;
goto _LL2148; _LL2148: _temp2147=( struct _tuple0*) _temp2144.name; goto _LL2146;
_LL2146: _temp2145=( struct Cyc_Core_Opt*) _temp2144.fields; goto _LL2087;}
else{ goto _LL2088;} _LL2088: if(*(( int*) _temp2072) == Cyc_Absyn_Typedef_d){
_LL2152: _temp2151=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2072)->f1; goto _LL2089;} else{ goto _LL2090;} _LL2090: if(*(( int*)
_temp2072) == Cyc_Absyn_Namespace_d){ _LL2156: _temp2155=( struct _tagged_string*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2072)->f1; goto _LL2154; _LL2154:
_temp2153=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2072)->f2; goto _LL2091;} else{ goto _LL2092;} _LL2092: if(*(( int*)
_temp2072) == Cyc_Absyn_Using_d){ _LL2160: _temp2159=( struct _tuple0*)(( struct
Cyc_Absyn_Using_d_struct*) _temp2072)->f1; goto _LL2158; _LL2158: _temp2157=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp2072)->f2; goto
_LL2093;} else{ goto _LL2094;} _LL2094: if(*(( int*) _temp2072) == Cyc_Absyn_ExternC_d){
_LL2162: _temp2161=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2072)->f1; goto _LL2095;} else{ goto _LL2073;} _LL2075: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2173=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2173[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2174; _temp2174.tag= Cyc_Absyn_FnType; _temp2174.f1=({ struct Cyc_Absyn_FnInfo
_temp2175; _temp2175.tvars= _temp2096->tvs; _temp2175.effect= _temp2096->effect;
_temp2175.ret_typ=( void*)(( void*) _temp2096->ret_type); _temp2175.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2096->args); _temp2175.varargs=
_temp2096->varargs; _temp2175.attributes= 0; _temp2175;}); _temp2174;});
_temp2173;}); s= Cyc_Absynpp_cat7( Cyc_PP_text( _temp2096->is_inline?( struct
_tagged_string)({ char* _temp2163=( char*)"inline "; struct _tagged_string
_temp2164; _temp2164.curr= _temp2163; _temp2164.base= _temp2163; _temp2164.last_plus_one=
_temp2163 + 8; _temp2164;}):( struct _tagged_string)({ char* _temp2165=( char*)"";
struct _tagged_string _temp2166; _temp2166.curr= _temp2165; _temp2166.base=
_temp2165; _temp2166.last_plus_one= _temp2165 + 1; _temp2166;})), Cyc_Absynpp_scope2doc((
void*) _temp2096->sc), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual _temp2167;
_temp2167.q_const= 0; _temp2167.q_volatile= 0; _temp2167.q_restrict= 0;
_temp2167;}), t,({ struct Cyc_Core_Opt* _temp2168=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2168->v=( void*) Cyc_Absynpp_cat2(
Cyc_Absynpp_atts2doc( _temp2096->attributes), Cyc_Absynpp_typedef_name2doc(
_temp2096->name)); _temp2168;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2169=( char*)" {"; struct _tagged_string _temp2170; _temp2170.curr=
_temp2169; _temp2170.base= _temp2169; _temp2170.last_plus_one= _temp2169 + 3;
_temp2170;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp2096->body))), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2171=( char*)"}"; struct _tagged_string _temp2172; _temp2172.curr=
_temp2171; _temp2172.base= _temp2171; _temp2172.last_plus_one= _temp2171 + 2;
_temp2172;}))); goto _LL2073;} _LL2077: if( _temp2098->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp2098->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2176=( char*)"struct "; struct _tagged_string
_temp2177; _temp2177.curr= _temp2176; _temp2177.base= _temp2176; _temp2177.last_plus_one=
_temp2176 + 8; _temp2177;})), _temp2098->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2178=( char*)""; struct _tagged_string _temp2179;
_temp2179.curr= _temp2178; _temp2179.base= _temp2178; _temp2179.last_plus_one=
_temp2178 + 1; _temp2179;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp2180= _temp2098->name; if( _temp2180 == 0){ _throw(
Null_Exception);} _temp2180->v;})), Cyc_Absynpp_ktvars2doc( _temp2098->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2181=( char*)";"; struct _tagged_string
_temp2182; _temp2182.curr= _temp2181; _temp2182.base= _temp2181; _temp2182.last_plus_one=
_temp2181 + 2; _temp2182;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2098->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2183=(
char*)"struct "; struct _tagged_string _temp2184; _temp2184.curr= _temp2183;
_temp2184.base= _temp2183; _temp2184.last_plus_one= _temp2183 + 8; _temp2184;})),
_temp2098->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2185=(
char*)""; struct _tagged_string _temp2186; _temp2186.curr= _temp2185; _temp2186.base=
_temp2185; _temp2186.last_plus_one= _temp2185 + 1; _temp2186;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2187= _temp2098->name; if(
_temp2187 == 0){ _throw( Null_Exception);} _temp2187->v;})), Cyc_Absynpp_ktvars2doc(
_temp2098->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2188=( char*)" {";
struct _tagged_string _temp2189; _temp2189.curr= _temp2188; _temp2189.base=
_temp2188; _temp2189.last_plus_one= _temp2188 + 3; _temp2189;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2190= _temp2098->fields; if( _temp2190 == 0){ _throw(
Null_Exception);} _temp2190->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2191=( char*)"}"; struct _tagged_string _temp2192;
_temp2192.curr= _temp2191; _temp2192.base= _temp2191; _temp2192.last_plus_one=
_temp2191 + 2; _temp2192;})), Cyc_Absynpp_atts2doc( _temp2098->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2193=( char*)";"; struct _tagged_string
_temp2194; _temp2194.curr= _temp2193; _temp2194.base= _temp2193; _temp2194.last_plus_one=
_temp2193 + 2; _temp2194;})));} goto _LL2073; _LL2079: if( _temp2100->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp2100->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2195=( char*)"union "; struct _tagged_string
_temp2196; _temp2196.curr= _temp2195; _temp2196.base= _temp2195; _temp2196.last_plus_one=
_temp2195 + 7; _temp2196;})), _temp2100->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2197=( char*)""; struct _tagged_string _temp2198;
_temp2198.curr= _temp2197; _temp2198.base= _temp2197; _temp2198.last_plus_one=
_temp2197 + 1; _temp2198;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2199= _temp2100->name; if( _temp2199 == 0){ _throw(
Null_Exception);} _temp2199->v;})), Cyc_Absynpp_tvars2doc( _temp2100->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2200=( char*)";"; struct _tagged_string
_temp2201; _temp2201.curr= _temp2200; _temp2201.base= _temp2200; _temp2201.last_plus_one=
_temp2200 + 2; _temp2201;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2100->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2202=(
char*)"union "; struct _tagged_string _temp2203; _temp2203.curr= _temp2202;
_temp2203.base= _temp2202; _temp2203.last_plus_one= _temp2202 + 7; _temp2203;})),
_temp2100->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2204=(
char*)""; struct _tagged_string _temp2205; _temp2205.curr= _temp2204; _temp2205.base=
_temp2204; _temp2205.last_plus_one= _temp2204 + 1; _temp2205;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2206= _temp2100->name; if(
_temp2206 == 0){ _throw( Null_Exception);} _temp2206->v;})), Cyc_Absynpp_tvars2doc(
_temp2100->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2207=( char*)" {";
struct _tagged_string _temp2208; _temp2208.curr= _temp2207; _temp2208.base=
_temp2207; _temp2208.last_plus_one= _temp2207 + 3; _temp2208;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2209= _temp2100->fields; if( _temp2209 == 0){ _throw(
Null_Exception);} _temp2209->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2210=( char*)"}"; struct _tagged_string _temp2211;
_temp2211.curr= _temp2210; _temp2211.base= _temp2210; _temp2211.last_plus_one=
_temp2210 + 2; _temp2211;})), Cyc_Absynpp_atts2doc( _temp2100->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2212=( char*)";"; struct _tagged_string
_temp2213; _temp2213.curr= _temp2212; _temp2213.base= _temp2212; _temp2213.last_plus_one=
_temp2212 + 2; _temp2213;})));} goto _LL2073; _LL2081: { struct Cyc_PP_Doc* sn=
Cyc_Absynpp_typedef_name2doc( _temp2115); s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(
_temp2117), Cyc_Absynpp_tqtd2doc( _temp2113, _temp2111,({ struct Cyc_Core_Opt*
_temp2214=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2214->v=( void*) sn; _temp2214;})), Cyc_Absynpp_atts2doc( _temp2105),
_temp2109 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2215=( char*)"";
struct _tagged_string _temp2216; _temp2216.curr= _temp2215; _temp2216.base=
_temp2215; _temp2216.last_plus_one= _temp2215 + 1; _temp2216;})): Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2217=( char*)" = "; struct
_tagged_string _temp2218; _temp2218.curr= _temp2217; _temp2218.base= _temp2217;
_temp2218.last_plus_one= _temp2217 + 4; _temp2218;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2219= _temp2109; if(
_temp2219 == 0){ _throw( Null_Exception);} _temp2219;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2220=( char*)";"; struct _tagged_string _temp2221;
_temp2221.curr= _temp2220; _temp2221.base= _temp2220; _temp2221.last_plus_one=
_temp2220 + 2; _temp2221;}))); goto _LL2073;} _LL2083: if( _temp2124 == 0){ s=
Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc( _temp2130), _temp2122? Cyc_PP_text((
struct _tagged_string)({ char* _temp2222=( char*)"xtunion "; struct
_tagged_string _temp2223; _temp2223.curr= _temp2222; _temp2223.base= _temp2222;
_temp2223.last_plus_one= _temp2222 + 9; _temp2223;})): Cyc_PP_text(( struct
_tagged_string)({ char* _temp2224=( char*)"tunion "; struct _tagged_string
_temp2225; _temp2225.curr= _temp2224; _temp2225.base= _temp2224; _temp2225.last_plus_one=
_temp2224 + 8; _temp2225;})), Cyc_Absynpp_typedef_name2doc( _temp2128), Cyc_Absynpp_ktvars2doc(
_temp2126), Cyc_PP_text(( struct _tagged_string)({ char* _temp2226=( char*)";";
struct _tagged_string _temp2227; _temp2227.curr= _temp2226; _temp2227.base=
_temp2226; _temp2227.last_plus_one= _temp2226 + 2; _temp2227;})));} else{ s= Cyc_Absynpp_cat8(
Cyc_Absynpp_scope2doc( _temp2130), _temp2122? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2228=( char*)"xtunion "; struct _tagged_string
_temp2229; _temp2229.curr= _temp2228; _temp2229.base= _temp2228; _temp2229.last_plus_one=
_temp2228 + 9; _temp2229;})): Cyc_PP_text(( struct _tagged_string)({ char*
_temp2230=( char*)"tunion "; struct _tagged_string _temp2231; _temp2231.curr=
_temp2230; _temp2231.base= _temp2230; _temp2231.last_plus_one= _temp2230 + 8;
_temp2231;})), Cyc_Absynpp_typedef_name2doc( _temp2128), Cyc_Absynpp_ktvars2doc(
_temp2126), Cyc_PP_text(( struct _tagged_string)({ char* _temp2232=( char*)" {";
struct _tagged_string _temp2233; _temp2233.curr= _temp2232; _temp2233.base=
_temp2232; _temp2233.last_plus_one= _temp2232 + 3; _temp2233;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2234= _temp2124; if( _temp2234 == 0){ _throw(
Null_Exception);} _temp2234->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2235=( char*)"};"; struct _tagged_string _temp2236;
_temp2236.curr= _temp2235; _temp2236.base= _temp2235; _temp2236.last_plus_one=
_temp2235 + 3; _temp2236;})));} goto _LL2073; _LL2085: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp2237=( char*)"let "; struct _tagged_string
_temp2238; _temp2238.curr= _temp2237; _temp2238.base= _temp2237; _temp2238.last_plus_one=
_temp2237 + 5; _temp2238;})), Cyc_Absynpp_pat2doc( _temp2140), Cyc_PP_text((
struct _tagged_string)({ char* _temp2239=( char*)" = "; struct _tagged_string
_temp2240; _temp2240.curr= _temp2239; _temp2240.base= _temp2239; _temp2240.last_plus_one=
_temp2239 + 4; _temp2240;})), Cyc_Absynpp_exp2doc( _temp2134), Cyc_PP_text((
struct _tagged_string)({ char* _temp2241=( char*)";"; struct _tagged_string
_temp2242; _temp2242.curr= _temp2241; _temp2242.base= _temp2241; _temp2242.last_plus_one=
_temp2241 + 2; _temp2242;}))); goto _LL2073; _LL2087: if( _temp2145 == 0){ s=
Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc( _temp2149), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2243=( char*)"enum "; struct _tagged_string
_temp2244; _temp2244.curr= _temp2243; _temp2244.base= _temp2243; _temp2244.last_plus_one=
_temp2243 + 6; _temp2244;})), Cyc_Absynpp_typedef_name2doc( _temp2147), Cyc_PP_text((
struct _tagged_string)({ char* _temp2245=( char*)";"; struct _tagged_string
_temp2246; _temp2246.curr= _temp2245; _temp2246.base= _temp2245; _temp2246.last_plus_one=
_temp2245 + 2; _temp2246;})));} else{ s= Cyc_Absynpp_cat7( Cyc_Absynpp_scope2doc(
_temp2149), Cyc_PP_text(( struct _tagged_string)({ char* _temp2247=( char*)"enum ";
struct _tagged_string _temp2248; _temp2248.curr= _temp2247; _temp2248.base=
_temp2247; _temp2248.last_plus_one= _temp2247 + 6; _temp2248;})), Cyc_Absynpp_qvar2doc(
_temp2147), Cyc_PP_text(( struct _tagged_string)({ char* _temp2249=( char*)" {";
struct _tagged_string _temp2250; _temp2250.curr= _temp2249; _temp2250.base=
_temp2249; _temp2250.last_plus_one= _temp2249 + 3; _temp2250;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2251= _temp2145; if( _temp2251 == 0){ _throw(
Null_Exception);} _temp2251->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2252=( char*)"};"; struct _tagged_string _temp2253;
_temp2253.curr= _temp2252; _temp2253.base= _temp2252; _temp2253.last_plus_one=
_temp2252 + 3; _temp2253;})));} goto _LL2073; _LL2089: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp2254=( char*)"typedef "; struct
_tagged_string _temp2255; _temp2255.curr= _temp2254; _temp2255.base= _temp2254;
_temp2255.last_plus_one= _temp2254 + 9; _temp2255;})), Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual _temp2256; _temp2256.q_const= 0; _temp2256.q_volatile= 0;
_temp2256.q_restrict= 0; _temp2256;}),( void*) _temp2151->defn,({ struct Cyc_Core_Opt*
_temp2257=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2257->v=( void*) Cyc_Absynpp_cat2( Cyc_Absynpp_typedef_name2doc( _temp2151->name),
Cyc_Absynpp_tvars2doc( _temp2151->tvs)); _temp2257;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2258=( char*)";"; struct _tagged_string _temp2259;
_temp2259.curr= _temp2258; _temp2259.base= _temp2258; _temp2259.last_plus_one=
_temp2258 + 2; _temp2259;}))); goto _LL2073; _LL2091: if( ! Cyc_Absynpp_qvar_to_Cids){
Cyc_Absynpp_curr_namespace_add( _temp2155);} s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp2260=( char*)"namespace "; struct
_tagged_string _temp2261; _temp2261.curr= _temp2260; _temp2261.base= _temp2260;
_temp2261.last_plus_one= _temp2260 + 11; _temp2261;})), Cyc_Absynpp_textptr(
_temp2155), Cyc_PP_text(( struct _tagged_string)({ char* _temp2262=( char*)" {";
struct _tagged_string _temp2263; _temp2263.curr= _temp2262; _temp2263.base=
_temp2262; _temp2263.last_plus_one= _temp2262 + 3; _temp2263;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2264=( char*)""; struct _tagged_string
_temp2265; _temp2265.curr= _temp2264; _temp2265.base= _temp2264; _temp2265.last_plus_one=
_temp2264 + 1; _temp2265;}), _temp2153), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2266=( char*)"}"; struct _tagged_string _temp2267;
_temp2267.curr= _temp2266; _temp2267.base= _temp2266; _temp2267.last_plus_one=
_temp2266 + 2; _temp2267;}))); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2073; _LL2093: if( Cyc_Absynpp_print_using_stmts){ s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2268=( char*)"using "; struct
_tagged_string _temp2269; _temp2269.curr= _temp2268; _temp2269.base= _temp2268;
_temp2269.last_plus_one= _temp2268 + 7; _temp2269;})), Cyc_Absynpp_qvar2doc(
_temp2159), Cyc_PP_text(( struct _tagged_string)({ char* _temp2270=( char*)" {";
struct _tagged_string _temp2271; _temp2271.curr= _temp2270; _temp2271.base=
_temp2270; _temp2271.last_plus_one= _temp2270 + 3; _temp2271;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2272=( char*)""; struct _tagged_string
_temp2273; _temp2273.curr= _temp2272; _temp2273.base= _temp2272; _temp2273.last_plus_one=
_temp2272 + 1; _temp2273;}), _temp2157), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2274=( char*)"}"; struct _tagged_string _temp2275;
_temp2275.curr= _temp2274; _temp2275.base= _temp2274; _temp2275.last_plus_one=
_temp2274 + 2; _temp2275;})));} else{ s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2276=( char*)"/* using "; struct _tagged_string
_temp2277; _temp2277.curr= _temp2276; _temp2277.base= _temp2276; _temp2277.last_plus_one=
_temp2276 + 10; _temp2277;})), Cyc_Absynpp_qvar2doc( _temp2159), Cyc_PP_text((
struct _tagged_string)({ char* _temp2278=( char*)" { */"; struct _tagged_string
_temp2279; _temp2279.curr= _temp2278; _temp2279.base= _temp2278; _temp2279.last_plus_one=
_temp2278 + 6; _temp2279;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2280=( char*)""; struct _tagged_string _temp2281; _temp2281.curr=
_temp2280; _temp2281.base= _temp2280; _temp2281.last_plus_one= _temp2280 + 1;
_temp2281;}), _temp2157), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2282=( char*)"/* } */"; struct _tagged_string _temp2283; _temp2283.curr=
_temp2282; _temp2283.base= _temp2282; _temp2283.last_plus_one= _temp2282 + 8;
_temp2283;})));} goto _LL2073; _LL2095: if( Cyc_Absynpp_print_externC_stmts){ s=
Cyc_Absynpp_cat5( Cyc_PP_text(( struct _tagged_string)({ char* _temp2284=( char*)"extern \"C\" {";
struct _tagged_string _temp2285; _temp2285.curr= _temp2284; _temp2285.base=
_temp2284; _temp2285.last_plus_one= _temp2284 + 13; _temp2285;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2286=( char*)""; struct _tagged_string
_temp2287; _temp2287.curr= _temp2286; _temp2287.base= _temp2286; _temp2287.last_plus_one=
_temp2286 + 1; _temp2287;}), _temp2161), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2288=( char*)"}"; struct _tagged_string _temp2289;
_temp2289.curr= _temp2288; _temp2289.base= _temp2288; _temp2289.last_plus_one=
_temp2288 + 2; _temp2289;})));} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2290=( char*)"/* extern \"C\" { */"; struct
_tagged_string _temp2291; _temp2291.curr= _temp2290; _temp2291.base= _temp2290;
_temp2291.last_plus_one= _temp2290 + 19; _temp2291;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2292=( char*)""; struct _tagged_string
_temp2293; _temp2293.curr= _temp2292; _temp2293.base= _temp2292; _temp2293.last_plus_one=
_temp2292 + 1; _temp2293;}), _temp2161), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2294=( char*)"/* } */"; struct _tagged_string
_temp2295; _temp2295.curr= _temp2294; _temp2295.base= _temp2294; _temp2295.last_plus_one=
_temp2294 + 8; _temp2295;})));} goto _LL2073; _LL2073:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void* sc){ void* _temp2296= sc; _LL2298: if( _temp2296 ==(
void*) Cyc_Absyn_Static){ goto _LL2299;} else{ goto _LL2300;} _LL2300: if(
_temp2296 ==( void*) Cyc_Absyn_Public){ goto _LL2301;} else{ goto _LL2302;}
_LL2302: if( _temp2296 ==( void*) Cyc_Absyn_Extern){ goto _LL2303;} else{ goto
_LL2304;} _LL2304: if( _temp2296 ==( void*) Cyc_Absyn_ExternC){ goto _LL2305;}
else{ goto _LL2306;} _LL2306: if( _temp2296 ==( void*) Cyc_Absyn_Abstract){ goto
_LL2307;} else{ goto _LL2297;} _LL2299: return Cyc_PP_text(( struct
_tagged_string)({ char* _temp2308=( char*)"static "; struct _tagged_string
_temp2309; _temp2309.curr= _temp2308; _temp2309.base= _temp2308; _temp2309.last_plus_one=
_temp2308 + 8; _temp2309;})); _LL2301: return Cyc_PP_nil_doc(); _LL2303: return
Cyc_PP_text(( struct _tagged_string)({ char* _temp2310=( char*)"extern "; struct
_tagged_string _temp2311; _temp2311.curr= _temp2310; _temp2311.base= _temp2310;
_temp2311.last_plus_one= _temp2310 + 8; _temp2311;})); _LL2305: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2312=( char*)"extern \"C\" "; struct
_tagged_string _temp2313; _temp2313.curr= _temp2312; _temp2313.base= _temp2312;
_temp2313.last_plus_one= _temp2312 + 12; _temp2313;})); _LL2307: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2314=( char*)"abstract "; struct
_tagged_string _temp2315; _temp2315.curr= _temp2314; _temp2315.base= _temp2314;
_temp2315.last_plus_one= _temp2314 + 10; _temp2315;})); _LL2297:;} int Cyc_Absynpp_exists_temp_tvar_in_effect(
void* t){ void* _temp2316= t; struct Cyc_Absyn_Tvar* _temp2324; struct Cyc_List_List*
_temp2326; _LL2318: if(( unsigned int) _temp2316 > 4u?*(( int*) _temp2316) ==
Cyc_Absyn_VarType: 0){ _LL2325: _temp2324=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp2316)->f1; goto _LL2319;} else{ goto _LL2320;} _LL2320: if(( unsigned int)
_temp2316 > 4u?*(( int*) _temp2316) == Cyc_Absyn_JoinEff: 0){ _LL2327: _temp2326=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp2316)->f1; goto
_LL2321;} else{ goto _LL2322;} _LL2322: goto _LL2323; _LL2319: return Cyc_Tcutil_is_temp_tvar(
_temp2324); _LL2321: return(( int(*)( int(* pred)( void*), struct Cyc_List_List*
x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect, _temp2326);
_LL2323: return 0; _LL2317:;} struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual
tq, void* t){ void* _temp2328= t; struct Cyc_Absyn_Exp* _temp2342; struct Cyc_Absyn_Tqual
_temp2344; void* _temp2346; struct Cyc_Absyn_PtrInfo _temp2348; struct Cyc_Absyn_Conref*
_temp2350; struct Cyc_Absyn_Tqual _temp2352; struct Cyc_Absyn_Conref* _temp2354;
void* _temp2356; void* _temp2358; struct Cyc_Absyn_FnInfo _temp2360; struct Cyc_List_List*
_temp2362; int _temp2364; struct Cyc_List_List* _temp2366; void* _temp2368;
struct Cyc_Core_Opt* _temp2370; struct Cyc_List_List* _temp2372; int _temp2374;
struct Cyc_Core_Opt* _temp2376; void* _temp2378; struct Cyc_Core_Opt* _temp2380;
struct Cyc_List_List* _temp2382; struct _tuple0* _temp2384; _LL2330: if((
unsigned int) _temp2328 > 4u?*(( int*) _temp2328) == Cyc_Absyn_ArrayType: 0){
_LL2347: _temp2346=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2328)->f1;
goto _LL2345; _LL2345: _temp2344=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2328)->f2; goto _LL2343; _LL2343: _temp2342=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2328)->f3; goto _LL2331;} else{ goto
_LL2332;} _LL2332: if(( unsigned int) _temp2328 > 4u?*(( int*) _temp2328) == Cyc_Absyn_PointerType:
0){ _LL2349: _temp2348=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2328)->f1; _LL2359: _temp2358=( void*) _temp2348.elt_typ; goto _LL2357;
_LL2357: _temp2356=( void*) _temp2348.rgn_typ; goto _LL2355; _LL2355: _temp2354=(
struct Cyc_Absyn_Conref*) _temp2348.nullable; goto _LL2353; _LL2353: _temp2352=(
struct Cyc_Absyn_Tqual) _temp2348.tq; goto _LL2351; _LL2351: _temp2350=( struct
Cyc_Absyn_Conref*) _temp2348.bounds; goto _LL2333;} else{ goto _LL2334;} _LL2334:
if(( unsigned int) _temp2328 > 4u?*(( int*) _temp2328) == Cyc_Absyn_FnType: 0){
_LL2361: _temp2360=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2328)->f1; _LL2373: _temp2372=( struct Cyc_List_List*) _temp2360.tvars;
goto _LL2371; _LL2371: _temp2370=( struct Cyc_Core_Opt*) _temp2360.effect; goto
_LL2369; _LL2369: _temp2368=( void*) _temp2360.ret_typ; goto _LL2367; _LL2367:
_temp2366=( struct Cyc_List_List*) _temp2360.args; goto _LL2365; _LL2365:
_temp2364=( int) _temp2360.varargs; goto _LL2363; _LL2363: _temp2362=( struct
Cyc_List_List*) _temp2360.attributes; goto _LL2335;} else{ goto _LL2336;}
_LL2336: if(( unsigned int) _temp2328 > 4u?*(( int*) _temp2328) == Cyc_Absyn_Evar:
0){ _LL2379: _temp2378=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp2328)->f1;
goto _LL2377; _LL2377: _temp2376=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp2328)->f2; goto _LL2375; _LL2375: _temp2374=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp2328)->f3; goto _LL2337;} else{ goto _LL2338;} _LL2338: if(( unsigned int)
_temp2328 > 4u?*(( int*) _temp2328) == Cyc_Absyn_TypedefType: 0){ _LL2385:
_temp2384=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2328)->f1;
goto _LL2383; _LL2383: _temp2382=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2328)->f2; goto _LL2381; _LL2381: _temp2380=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2328)->f3; goto _LL2339;} else{ goto
_LL2340;} _LL2340: goto _LL2341; _LL2331: { struct Cyc_List_List* _temp2388;
void* _temp2390; struct Cyc_Absyn_Tqual _temp2392; struct _tuple4 _temp2386= Cyc_Absynpp_to_tms(
_temp2344, _temp2346); _LL2393: _temp2392= _temp2386.f1; goto _LL2391; _LL2391:
_temp2390= _temp2386.f2; goto _LL2389; _LL2389: _temp2388= _temp2386.f3; goto
_LL2387; _LL2387: { void* tm; if( _temp2342 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;}
else{ tm=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct* _temp2394=( struct
Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct));
_temp2394[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct _temp2395; _temp2395.tag=
Cyc_Absyn_ConstArray_mod; _temp2395.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2396= _temp2342; if( _temp2396 == 0){ _throw( Null_Exception);} _temp2396;});
_temp2395;}); _temp2394;});} return({ struct _tuple4 _temp2397; _temp2397.f1=
_temp2392; _temp2397.f2= _temp2390; _temp2397.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2398=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2398->hd=( void*) tm; _temp2398->tl= _temp2388;
_temp2398;}); _temp2397;});}} _LL2333: { struct Cyc_List_List* _temp2401; void*
_temp2403; struct Cyc_Absyn_Tqual _temp2405; struct _tuple4 _temp2399= Cyc_Absynpp_to_tms(
_temp2352, _temp2358); _LL2406: _temp2405= _temp2399.f1; goto _LL2404; _LL2404:
_temp2403= _temp2399.f2; goto _LL2402; _LL2402: _temp2401= _temp2399.f3; goto
_LL2400; _LL2400: { void* ps;{ struct _tuple8 _temp2408=({ struct _tuple8
_temp2407; _temp2407.f1=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2354))->v; _temp2407.f2=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2350))->v; _temp2407;}); void* _temp2418; void* _temp2420; struct Cyc_Absyn_Exp*
_temp2422; void* _temp2424; int _temp2426; void* _temp2428; void* _temp2430;
struct Cyc_Absyn_Exp* _temp2432; void* _temp2434; int _temp2436; void* _temp2438;
void* _temp2440; void* _temp2442; _LL2410: _LL2425: _temp2424= _temp2408.f1; if((
unsigned int) _temp2424 > 1u?*(( int*) _temp2424) == Cyc_Absyn_Eq_constr: 0){
_LL2427: _temp2426=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2424)->f1;
if( _temp2426 == 1){ goto _LL2419;} else{ goto _LL2412;}} else{ goto _LL2412;}
_LL2419: _temp2418= _temp2408.f2; if(( unsigned int) _temp2418 > 1u?*(( int*)
_temp2418) == Cyc_Absyn_Eq_constr: 0){ _LL2421: _temp2420=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2418)->f1; if(( unsigned int) _temp2420 > 1u?*(( int*) _temp2420) == Cyc_Absyn_Upper_b:
0){ _LL2423: _temp2422=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2420)->f1; goto _LL2411;} else{ goto _LL2412;}} else{ goto _LL2412;}
_LL2412: _LL2435: _temp2434= _temp2408.f1; if(( unsigned int) _temp2434 > 1u?*((
int*) _temp2434) == Cyc_Absyn_Eq_constr: 0){ _LL2437: _temp2436=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp2434)->f1; if( _temp2436 == 0){ goto _LL2429;}
else{ goto _LL2414;}} else{ goto _LL2414;} _LL2429: _temp2428= _temp2408.f2; if((
unsigned int) _temp2428 > 1u?*(( int*) _temp2428) == Cyc_Absyn_Eq_constr: 0){
_LL2431: _temp2430=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2428)->f1;
if(( unsigned int) _temp2430 > 1u?*(( int*) _temp2430) == Cyc_Absyn_Upper_b: 0){
_LL2433: _temp2432=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2430)->f1; goto _LL2413;} else{ goto _LL2414;}} else{ goto _LL2414;}
_LL2414: _LL2443: _temp2442= _temp2408.f1; goto _LL2439; _LL2439: _temp2438=
_temp2408.f2; if(( unsigned int) _temp2438 > 1u?*(( int*) _temp2438) == Cyc_Absyn_Eq_constr:
0){ _LL2441: _temp2440=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2438)->f1;
if( _temp2440 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2415;} else{ goto _LL2416;}}
else{ goto _LL2416;} _LL2416: goto _LL2417; _LL2411: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2444=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2444[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2445; _temp2445.tag=
Cyc_Absyn_Nullable_ps; _temp2445.f1= _temp2422; _temp2445;}); _temp2444;}); goto
_LL2409; _LL2413: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2446=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2446[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2447; _temp2447.tag= Cyc_Absyn_NonNullable_ps; _temp2447.f1= _temp2432;
_temp2447;}); _temp2446;}); goto _LL2409; _LL2415: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2409; _LL2417: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2409;
_LL2409:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2450=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2450[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2451; _temp2451.tag=
Cyc_Absyn_Pointer_mod; _temp2451.f1=( void*) ps; _temp2451.f2=( void*) _temp2356;
_temp2451.f3= tq; _temp2451;}); _temp2450;}); return({ struct _tuple4 _temp2448;
_temp2448.f1= _temp2405; _temp2448.f2= _temp2403; _temp2448.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2449=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2449->hd=( void*) tm; _temp2449->tl= _temp2401;
_temp2449;}); _temp2448;});}}} _LL2335: if( ! Cyc_Absynpp_print_all_tvars){ if(
_temp2370 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)({ struct Cyc_Core_Opt*
_temp2452= _temp2370; if( _temp2452 == 0){ _throw( Null_Exception);} _temp2452->v;}))){
_temp2370= 0; _temp2372= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2372);}}{ struct Cyc_List_List* _temp2455;
void* _temp2457; struct Cyc_Absyn_Tqual _temp2459; struct _tuple4 _temp2453= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp2368); _LL2460: _temp2459= _temp2453.f1; goto
_LL2458; _LL2458: _temp2457= _temp2453.f2; goto _LL2456; _LL2456: _temp2455=
_temp2453.f3; goto _LL2454; _LL2454: { struct Cyc_List_List* tms= _temp2455; if(
_temp2362 != 0){ tms=({ struct Cyc_List_List* _temp2461=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2461->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2462=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2462[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2463; _temp2463.tag= Cyc_Absyn_Attributes_mod;
_temp2463.f1= 0; _temp2463.f2= _temp2362; _temp2463;}); _temp2462;})); _temp2461->tl=
tms; _temp2461;});} tms=({ struct Cyc_List_List* _temp2464=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2464->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2465=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2465[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2466; _temp2466.tag= Cyc_Absyn_Function_mod;
_temp2466.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2467=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2467[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2468; _temp2468.tag= Cyc_Absyn_WithTypes;
_temp2468.f1= _temp2366; _temp2468.f2= _temp2364; _temp2468.f3= _temp2370;
_temp2468;}); _temp2467;})); _temp2466;}); _temp2465;})); _temp2464->tl= tms;
_temp2464;}); if( _temp2372 != 0){ tms=({ struct Cyc_List_List* _temp2469=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2469->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2470=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2470[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2471; _temp2471.tag= Cyc_Absyn_TypeParams_mod;
_temp2471.f1= _temp2372; _temp2471.f2= 0; _temp2471.f3= 1; _temp2471;});
_temp2470;})); _temp2469->tl= tms; _temp2469;});} return({ struct _tuple4
_temp2472; _temp2472.f1= _temp2459; _temp2472.f2= _temp2457; _temp2472.f3= tms;
_temp2472;});}} _LL2337: if( _temp2376 == 0){ return({ struct _tuple4 _temp2473;
_temp2473.f1= tq; _temp2473.f2= t; _temp2473.f3= 0; _temp2473;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2474= _temp2376; if(
_temp2474 == 0){ _throw( Null_Exception);} _temp2474->v;}));} _LL2339: if(
_temp2380 == 0? 1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4
_temp2475; _temp2475.f1= tq; _temp2475.f2= t; _temp2475.f3= 0; _temp2475;});}
else{ return Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2476=
_temp2380; if( _temp2476 == 0){ _throw( Null_Exception);} _temp2476->v;}));}
_LL2341: return({ struct _tuple4 _temp2477; _temp2477.f1= tq; _temp2477.f2= t;
_temp2477.f3= 0; _temp2477;}); _LL2329:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp2480; void* _temp2482; struct Cyc_Absyn_Tqual _temp2484; struct _tuple4
_temp2478= Cyc_Absynpp_to_tms( tq, t); _LL2485: _temp2484= _temp2478.f1; goto
_LL2483; _LL2483: _temp2482= _temp2478.f2; goto _LL2481; _LL2481: _temp2480=
_temp2478.f3; goto _LL2479; _LL2479: _temp2480=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2480); if( _temp2480 == 0?
dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc( _temp2484), Cyc_Absynpp_ntyp2doc(
_temp2482));} else{ return Cyc_Absynpp_cat4( Cyc_Absynpp_tqual2doc( _temp2484),
Cyc_Absynpp_ntyp2doc( _temp2482), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2486=( char*)" "; struct _tagged_string _temp2487; _temp2487.curr=
_temp2486; _temp2487.base= _temp2486; _temp2487.last_plus_one= _temp2486 + 2;
_temp2487;})), Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)({
struct Cyc_Core_Opt* _temp2488= dopt; if( _temp2488 == 0){ _throw(
Null_Exception);} _temp2488->v;}), _temp2480));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return Cyc_Absynpp_cat5(
Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2489=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2489->v=(
void*) Cyc_Absynpp_textptr( f->name); _temp2489;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2490=( char*)":"; struct _tagged_string _temp2491;
_temp2491.curr= _temp2490; _temp2491.base= _temp2490; _temp2491.last_plus_one=
_temp2490 + 2; _temp2491;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp2492= f->width; if( _temp2492 == 0){ _throw(
Null_Exception);} _temp2492->v;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2493=( char*)";"; struct _tagged_string
_temp2494; _temp2494.curr= _temp2493; _temp2494.base= _temp2493; _temp2494.last_plus_one=
_temp2493 + 2; _temp2494;})));} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2495=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2495->v=( void*) Cyc_Absynpp_textptr(
f->name); _temp2495;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2496=( char*)";"; struct _tagged_string
_temp2497; _temp2497.curr= _temp2496; _temp2497.base= _temp2496; _temp2497.last_plus_one=
_temp2496 + 2; _temp2497;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,( struct _tagged_string)({ char*
_temp2498=( char*)""; struct _tagged_string _temp2499; _temp2499.curr= _temp2498;
_temp2499.base= _temp2498; _temp2499.last_plus_one= _temp2498 + 1; _temp2499;}),
fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc( struct Cyc_Absyn_Tunionfield*
f){ return Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc(( void*) f->sc), Cyc_Absynpp_typedef_name2doc(
f->name), f->tvs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs), f->typs
== 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,( struct _tagged_string)({ char*
_temp2500=( char*)","; struct _tagged_string _temp2501; _temp2501.curr=
_temp2500; _temp2501.base= _temp2500; _temp2501.last_plus_one= _temp2500 + 2;
_temp2501;}), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl=({ struct Cyc_List_List*
_temp2502= tdl; if( _temp2502 == 0){ _throw( Null_Exception);} _temp2502->tl;})){
Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp2503= tdl; if( _temp2503 == 0){ _throw( Null_Exception);} _temp2503->hd;})),
72, f); fprintf( f,"\n");}} struct _tagged_string Cyc_Absynpp_decllist2string(
struct Cyc_List_List* tdl){ return Cyc_PP_string_of_doc( Cyc_PP_seql(( struct
_tagged_string)({ char* _temp2504=( char*)""; struct _tagged_string _temp2505;
_temp2505.curr= _temp2504; _temp2505.base= _temp2504; _temp2505.last_plus_one=
_temp2504 + 1; _temp2505;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_decl2doc,
tdl)), 72);} struct _tagged_string Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*
e){ return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e), 72);} struct
_tagged_string Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_stmt2doc( s), 72);} struct _tagged_string Cyc_Absynpp_typ2string(
void* t){ Cyc_Absynpp_dont_rewrite_temp_tvars= 1;{ struct _tagged_string
_temp2506= Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc( t), 72); goto _LL2507;
_LL2507: Cyc_Absynpp_dont_rewrite_temp_tvars= 0; return _temp2506;}} struct
_tagged_string Cyc_Absynpp_qvar2string( struct _tuple0* v){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_qvar2doc( v), 72);} struct _tagged_string Cyc_Absynpp_prim2string(
void* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_prim2doc( p), 72);} struct
_tagged_string Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_pat2doc( p), 72);} struct _tagged_string Cyc_Absynpp_scope2string(
void* sc){ void* _temp2508= sc; _LL2510: if( _temp2508 ==( void*) Cyc_Absyn_Static){
goto _LL2511;} else{ goto _LL2512;} _LL2512: if( _temp2508 ==( void*) Cyc_Absyn_Public){
goto _LL2513;} else{ goto _LL2514;} _LL2514: if( _temp2508 ==( void*) Cyc_Absyn_Extern){
goto _LL2515;} else{ goto _LL2516;} _LL2516: if( _temp2508 ==( void*) Cyc_Absyn_ExternC){
goto _LL2517;} else{ goto _LL2518;} _LL2518: if( _temp2508 ==( void*) Cyc_Absyn_Abstract){
goto _LL2519;} else{ goto _LL2509;} _LL2511: return( struct _tagged_string)({
char* _temp2520=( char*)"static"; struct _tagged_string _temp2521; _temp2521.curr=
_temp2520; _temp2521.base= _temp2520; _temp2521.last_plus_one= _temp2520 + 7;
_temp2521;}); _LL2513: return( struct _tagged_string)({ char* _temp2522=( char*)"public";
struct _tagged_string _temp2523; _temp2523.curr= _temp2522; _temp2523.base=
_temp2522; _temp2523.last_plus_one= _temp2522 + 7; _temp2523;}); _LL2515: return(
struct _tagged_string)({ char* _temp2524=( char*)"extern"; struct _tagged_string
_temp2525; _temp2525.curr= _temp2524; _temp2525.base= _temp2524; _temp2525.last_plus_one=
_temp2524 + 7; _temp2525;}); _LL2517: return( struct _tagged_string)({ char*
_temp2526=( char*)"extern \"C\""; struct _tagged_string _temp2527; _temp2527.curr=
_temp2526; _temp2527.base= _temp2526; _temp2527.last_plus_one= _temp2526 + 11;
_temp2527;}); _LL2519: return( struct _tagged_string)({ char* _temp2528=( char*)"abstract";
struct _tagged_string _temp2529; _temp2529.curr= _temp2528; _temp2529.base=
_temp2528; _temp2529.last_plus_one= _temp2528 + 9; _temp2529;}); _LL2509:;}