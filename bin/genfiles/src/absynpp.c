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
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType= 11;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 12; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 13; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
14; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 15; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
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
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_Tunion_e= 27;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 28; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 29; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
30; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 31; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 32; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 33; struct Cyc_Absyn_Fill_e_struct{
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
_temp1->tl;}) == 0){* l= 0;} else{(( void(*)( struct Cyc_List_List** l)) Cyc_Absynpp_suppr_last)(({
struct Cyc_List_List* _temp2=* l; if( _temp2 == 0){ _throw( Null_Exception);}&
_temp2->tl;}));}} static void Cyc_Absynpp_curr_namespace_drop(){(( void(*)(
struct Cyc_List_List** l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
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
_temp442= t; struct Cyc_Absyn_Exp* _temp484; struct Cyc_Absyn_Tqual _temp486;
void* _temp488; struct Cyc_Absyn_FnInfo _temp490; struct Cyc_Absyn_PtrInfo
_temp492; int _temp494; struct Cyc_Core_Opt* _temp496; void* _temp498; struct
Cyc_Absyn_Tvar* _temp500; struct Cyc_Absyn_TunionInfo _temp502; void* _temp504;
struct Cyc_List_List* _temp506; void* _temp508; struct Cyc_Absyn_TunionFieldInfo
_temp510; struct Cyc_List_List* _temp512; void* _temp514; void* _temp516; void*
_temp518; struct Cyc_List_List* _temp520; struct Cyc_Absyn_Structdecl** _temp522;
struct Cyc_List_List* _temp524; struct _tuple0* _temp526; struct Cyc_Absyn_Uniondecl**
_temp528; struct Cyc_List_List* _temp530; struct _tuple0* _temp532; struct Cyc_Absyn_Enumdecl*
_temp534; struct _tuple0* _temp536; struct Cyc_Core_Opt* _temp538; struct Cyc_List_List*
_temp540; struct _tuple0* _temp542; void* _temp544; void* _temp546; struct Cyc_List_List*
_temp548; _LL444: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_ArrayType:
0){ _LL489: _temp488=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp442)->f1;
goto _LL487; _LL487: _temp486=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp442)->f2; goto _LL485; _LL485: _temp484=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp442)->f3; goto _LL445;} else{ goto _LL446;}
_LL446: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_FnType:
0){ _LL491: _temp490=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp442)->f1; goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int)
_temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_PointerType: 0){ _LL493: _temp492=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp442)->f1;
goto _LL449;} else{ goto _LL450;} _LL450: if( _temp442 ==( void*) Cyc_Absyn_VoidType){
goto _LL451;} else{ goto _LL452;} _LL452: if(( unsigned int) _temp442 > 4u?*((
int*) _temp442) == Cyc_Absyn_Evar: 0){ _LL499: _temp498=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp442)->f1; goto _LL497; _LL497: _temp496=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp442)->f2; goto _LL495; _LL495: _temp494=( int)((
struct Cyc_Absyn_Evar_struct*) _temp442)->f3; goto _LL453;} else{ goto _LL454;}
_LL454: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_VarType:
0){ _LL501: _temp500=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp442)->f1; goto _LL455;} else{ goto _LL456;} _LL456: if(( unsigned int)
_temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_TunionType: 0){ _LL503: _temp502=(
struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*) _temp442)->f1;
_LL509: _temp508=( void*) _temp502.tunion_info; goto _LL507; _LL507: _temp506=(
struct Cyc_List_List*) _temp502.targs; goto _LL505; _LL505: _temp504=( void*)
_temp502.rgn; goto _LL457;} else{ goto _LL458;} _LL458: if(( unsigned int)
_temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_TunionFieldType: 0){ _LL511:
_temp510=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp442)->f1; _LL515: _temp514=( void*) _temp510.field_info; goto _LL513;
_LL513: _temp512=( struct Cyc_List_List*) _temp510.targs; goto _LL459;} else{
goto _LL460;} _LL460: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) ==
Cyc_Absyn_IntType: 0){ _LL519: _temp518=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp442)->f1; goto _LL517; _LL517: _temp516=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp442)->f2; goto _LL461;} else{ goto _LL462;} _LL462: if( _temp442 ==( void*)
Cyc_Absyn_FloatType){ goto _LL463;} else{ goto _LL464;} _LL464: if( _temp442 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL465;} else{ goto _LL466;} _LL466: if((
unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_TupleType: 0){
_LL521: _temp520=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp442)->f1; goto _LL467;} else{ goto _LL468;} _LL468: if(( unsigned int)
_temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_StructType: 0){ _LL527: _temp526=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp442)->f1; goto
_LL525; _LL525: _temp524=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp442)->f2; goto _LL523; _LL523: _temp522=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp442)->f3; goto _LL469;} else{ goto
_LL470;} _LL470: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_UnionType:
0){ _LL533: _temp532=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp442)->f1; goto _LL531; _LL531: _temp530=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp442)->f2; goto _LL529; _LL529: _temp528=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp442)->f3;
goto _LL471;} else{ goto _LL472;} _LL472: if(( unsigned int) _temp442 > 4u?*((
int*) _temp442) == Cyc_Absyn_EnumType: 0){ _LL537: _temp536=( struct _tuple0*)((
struct Cyc_Absyn_EnumType_struct*) _temp442)->f1; goto _LL535; _LL535: _temp534=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp442)->f2;
goto _LL473;} else{ goto _LL474;} _LL474: if(( unsigned int) _temp442 > 4u?*((
int*) _temp442) == Cyc_Absyn_TypedefType: 0){ _LL543: _temp542=( struct _tuple0*)((
struct Cyc_Absyn_TypedefType_struct*) _temp442)->f1; goto _LL541; _LL541:
_temp540=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp442)->f2; goto _LL539; _LL539: _temp538=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp442)->f3; goto _LL475;} else{ goto _LL476;}
_LL476: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_RgnHandleType:
0){ _LL545: _temp544=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp442)->f1;
goto _LL477;} else{ goto _LL478;} _LL478: if( _temp442 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL479;} else{ goto _LL480;} _LL480: if(( unsigned int) _temp442 > 4u?*((
int*) _temp442) == Cyc_Absyn_AccessEff: 0){ _LL547: _temp546=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp442)->f1; goto _LL481;} else{ goto _LL482;}
_LL482: if(( unsigned int) _temp442 > 4u?*(( int*) _temp442) == Cyc_Absyn_JoinEff:
0){ _LL549: _temp548=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp442)->f1; goto _LL483;} else{ goto _LL443;} _LL445: return Cyc_PP_text((
struct _tagged_string)({ char* _temp550=( char*)"[[[array]]]"; struct
_tagged_string _temp551; _temp551.curr= _temp550; _temp551.base= _temp550;
_temp551.last_plus_one= _temp550 + 12; _temp551;})); _LL447: return Cyc_PP_nil_doc();
_LL449: return Cyc_PP_nil_doc(); _LL451: s= Cyc_PP_text(( struct _tagged_string)({
char* _temp552=( char*)"void"; struct _tagged_string _temp553; _temp553.curr=
_temp552; _temp553.base= _temp552; _temp553.last_plus_one= _temp552 + 5;
_temp553;})); goto _LL443; _LL453: if( _temp496 != 0){ return Cyc_Absynpp_ntyp2doc((
void*)({ struct Cyc_Core_Opt* _temp554= _temp496; if( _temp554 == 0){ _throw(
Null_Exception);} _temp554->v;}));} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp555=( char*)"%"; struct _tagged_string
_temp556; _temp556.curr= _temp555; _temp556.base= _temp555; _temp556.last_plus_one=
_temp555 + 2; _temp556;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp557=( char*)"("; struct _tagged_string _temp558; _temp558.curr= _temp557;
_temp558.base= _temp557; _temp558.last_plus_one= _temp557 + 2; _temp558;})), Cyc_PP_text(
xprintf("%d", _temp494)), Cyc_PP_text(( struct _tagged_string)({ char* _temp559=(
char*)")::"; struct _tagged_string _temp560; _temp560.curr= _temp559; _temp560.base=
_temp559; _temp560.last_plus_one= _temp559 + 4; _temp560;})), Cyc_Absynpp_kind2doc(
_temp498));} goto _LL443; _LL455: s= Cyc_Absynpp_textptr( _temp500->name); if(
Cyc_Absynpp_print_all_kinds){ s= Cyc_Absynpp_cat3( s, Cyc_PP_text(( struct
_tagged_string)({ char* _temp561=( char*)"::"; struct _tagged_string _temp562;
_temp562.curr= _temp561; _temp562.base= _temp561; _temp562.last_plus_one=
_temp561 + 3; _temp562;})), Cyc_Absynpp_ckind2doc( _temp500->kind));} if( ! Cyc_Absynpp_dont_rewrite_temp_tvars?
Cyc_Tcutil_is_temp_tvar( _temp500): 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp563=( char*)"_ /* "; struct _tagged_string
_temp564; _temp564.curr= _temp563; _temp564.base= _temp563; _temp564.last_plus_one=
_temp563 + 6; _temp564;})), s, Cyc_PP_text(( struct _tagged_string)({ char*
_temp565=( char*)" */"; struct _tagged_string _temp566; _temp566.curr= _temp565;
_temp566.base= _temp565; _temp566.last_plus_one= _temp565 + 4; _temp566;})));}
goto _LL443; _LL457:{ void* _temp567= _temp508; struct Cyc_Absyn_UnknownTunionInfo
_temp573; int _temp575; struct _tuple0* _temp577; struct Cyc_Absyn_Tuniondecl*
_temp579; struct Cyc_Absyn_Tuniondecl _temp581; int _temp582; struct Cyc_Core_Opt*
_temp584; struct Cyc_List_List* _temp586; struct _tuple0* _temp588; void*
_temp590; _LL569: if(*(( int*) _temp567) == Cyc_Absyn_UnknownTunion){ _LL574:
_temp573=( struct Cyc_Absyn_UnknownTunionInfo)(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp567)->f1; _LL578: _temp577=( struct _tuple0*) _temp573.name; goto _LL576;
_LL576: _temp575=( int) _temp573.is_xtunion; goto _LL570;} else{ goto _LL571;}
_LL571: if(*(( int*) _temp567) == Cyc_Absyn_KnownTunion){ _LL580: _temp579=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*) _temp567)->f1;
_temp581=* _temp579; _LL591: _temp590=( void*) _temp581.sc; goto _LL589; _LL589:
_temp588=( struct _tuple0*) _temp581.name; goto _LL587; _LL587: _temp586=(
struct Cyc_List_List*) _temp581.tvs; goto _LL585; _LL585: _temp584=( struct Cyc_Core_Opt*)
_temp581.fields; goto _LL583; _LL583: _temp582=( int) _temp581.is_xtunion; goto
_LL572;} else{ goto _LL568;} _LL570: _temp588= _temp577; _temp582= _temp575;
goto _LL572; _LL572: { struct Cyc_PP_Doc* kw= Cyc_PP_text( _temp582?( struct
_tagged_string)({ char* _temp600=( char*)"xtunion "; struct _tagged_string
_temp601; _temp601.curr= _temp600; _temp601.base= _temp600; _temp601.last_plus_one=
_temp600 + 9; _temp601;}):( struct _tagged_string)({ char* _temp602=( char*)"tunion ";
struct _tagged_string _temp603; _temp603.curr= _temp602; _temp603.base= _temp602;
_temp603.last_plus_one= _temp602 + 8; _temp603;}));{ void* _temp592= Cyc_Tcutil_compress(
_temp504); _LL594: if( _temp592 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL595;}
else{ goto _LL596;} _LL596: goto _LL597; _LL595: s= Cyc_Absynpp_cat3( kw, Cyc_Absynpp_qvar2doc(
_temp588), Cyc_Absynpp_tps2doc( _temp506)); goto _LL593; _LL597: s= Cyc_Absynpp_cat5(
kw, Cyc_Absynpp_typ2doc( _temp504), Cyc_PP_text(( struct _tagged_string)({ char*
_temp598=( char*)" "; struct _tagged_string _temp599; _temp599.curr= _temp598;
_temp599.base= _temp598; _temp599.last_plus_one= _temp598 + 2; _temp599;})), Cyc_Absynpp_qvar2doc(
_temp588), Cyc_Absynpp_tps2doc( _temp506)); goto _LL593; _LL593:;} goto _LL568;}
_LL568:;} goto _LL443; _LL459:{ void* _temp604= _temp514; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp610; int _temp612; struct _tuple0* _temp614; struct _tuple0* _temp616;
struct Cyc_Absyn_Tunionfield* _temp618; struct Cyc_Absyn_Tunionfield _temp620;
struct Cyc_Position_Segment* _temp621; struct Cyc_List_List* _temp623; struct
Cyc_List_List* _temp625; struct _tuple0* _temp627; struct Cyc_Absyn_Tuniondecl*
_temp629; struct Cyc_Absyn_Tuniondecl _temp631; int _temp632; struct Cyc_Core_Opt*
_temp634; struct Cyc_List_List* _temp636; struct _tuple0* _temp638; void*
_temp640; _LL606: if(*(( int*) _temp604) == Cyc_Absyn_UnknownTunionfield){
_LL611: _temp610=( struct Cyc_Absyn_UnknownTunionFieldInfo)(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp604)->f1; _LL617: _temp616=( struct _tuple0*) _temp610.tunion_name; goto
_LL615; _LL615: _temp614=( struct _tuple0*) _temp610.field_name; goto _LL613;
_LL613: _temp612=( int) _temp610.is_xtunion; goto _LL607;} else{ goto _LL608;}
_LL608: if(*(( int*) _temp604) == Cyc_Absyn_KnownTunionfield){ _LL630: _temp629=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp604)->f1; _temp631=* _temp629; _LL641: _temp640=( void*) _temp631.sc; goto
_LL639; _LL639: _temp638=( struct _tuple0*) _temp631.name; goto _LL637; _LL637:
_temp636=( struct Cyc_List_List*) _temp631.tvs; goto _LL635; _LL635: _temp634=(
struct Cyc_Core_Opt*) _temp631.fields; goto _LL633; _LL633: _temp632=( int)
_temp631.is_xtunion; goto _LL619; _LL619: _temp618=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp604)->f2; _temp620=* _temp618;
_LL628: _temp627=( struct _tuple0*) _temp620.name; goto _LL626; _LL626: _temp625=(
struct Cyc_List_List*) _temp620.tvs; goto _LL624; _LL624: _temp623=( struct Cyc_List_List*)
_temp620.typs; goto _LL622; _LL622: _temp621=( struct Cyc_Position_Segment*)
_temp620.loc; goto _LL609;} else{ goto _LL605;} _LL607: _temp638= _temp616;
_temp632= _temp612; _temp627= _temp614; goto _LL609; _LL609: { struct Cyc_PP_Doc*
kw= Cyc_PP_text( _temp632?( struct _tagged_string)({ char* _temp644=( char*)"xtunion ";
struct _tagged_string _temp645; _temp645.curr= _temp644; _temp645.base= _temp644;
_temp645.last_plus_one= _temp644 + 9; _temp645;}):( struct _tagged_string)({
char* _temp646=( char*)"tunion "; struct _tagged_string _temp647; _temp647.curr=
_temp646; _temp647.base= _temp646; _temp647.last_plus_one= _temp646 + 8;
_temp647;})); s= Cyc_Absynpp_cat4( kw, Cyc_Absynpp_qvar2doc( _temp638), Cyc_PP_text((
struct _tagged_string)({ char* _temp642=( char*)"."; struct _tagged_string
_temp643; _temp643.curr= _temp642; _temp643.base= _temp642; _temp643.last_plus_one=
_temp642 + 2; _temp643;})), Cyc_Absynpp_textptr((* _temp627).f2)); goto _LL605;}
_LL605:;} goto _LL443; _LL461: { struct _tagged_string sns; struct
_tagged_string ts;{ void* _temp648= _temp518; _LL650: if( _temp648 ==( void*)
Cyc_Absyn_Signed){ goto _LL651;} else{ goto _LL652;} _LL652: if( _temp648 ==(
void*) Cyc_Absyn_Unsigned){ goto _LL653;} else{ goto _LL649;} _LL651: sns=(
struct _tagged_string)({ char* _temp654=( char*)""; struct _tagged_string
_temp655; _temp655.curr= _temp654; _temp655.base= _temp654; _temp655.last_plus_one=
_temp654 + 1; _temp655;}); goto _LL649; _LL653: sns=( struct _tagged_string)({
char* _temp656=( char*)"unsigned "; struct _tagged_string _temp657; _temp657.curr=
_temp656; _temp657.base= _temp656; _temp657.last_plus_one= _temp656 + 10;
_temp657;}); goto _LL649; _LL649:;}{ void* _temp658= _temp516; _LL660: if(
_temp658 ==( void*) Cyc_Absyn_B1){ goto _LL661;} else{ goto _LL662;} _LL662: if(
_temp658 ==( void*) Cyc_Absyn_B2){ goto _LL663;} else{ goto _LL664;} _LL664: if(
_temp658 ==( void*) Cyc_Absyn_B4){ goto _LL665;} else{ goto _LL666;} _LL666: if(
_temp658 ==( void*) Cyc_Absyn_B8){ goto _LL667;} else{ goto _LL659;} _LL661:{
void* _temp668= _temp518; _LL670: if( _temp668 ==( void*) Cyc_Absyn_Signed){
goto _LL671;} else{ goto _LL672;} _LL672: if( _temp668 ==( void*) Cyc_Absyn_Unsigned){
goto _LL673;} else{ goto _LL669;} _LL671: sns=( struct _tagged_string)({ char*
_temp674=( char*)"signed "; struct _tagged_string _temp675; _temp675.curr=
_temp674; _temp675.base= _temp674; _temp675.last_plus_one= _temp674 + 8;
_temp675;}); goto _LL669; _LL673: sns=( struct _tagged_string)({ char* _temp676=(
char*)""; struct _tagged_string _temp677; _temp677.curr= _temp676; _temp677.base=
_temp676; _temp677.last_plus_one= _temp676 + 1; _temp677;}); goto _LL669; _LL669:;}
ts=( struct _tagged_string)({ char* _temp678=( char*)"char"; struct
_tagged_string _temp679; _temp679.curr= _temp678; _temp679.base= _temp678;
_temp679.last_plus_one= _temp678 + 5; _temp679;}); goto _LL659; _LL663: ts=(
struct _tagged_string)({ char* _temp680=( char*)"short"; struct _tagged_string
_temp681; _temp681.curr= _temp680; _temp681.base= _temp680; _temp681.last_plus_one=
_temp680 + 6; _temp681;}); goto _LL659; _LL665: ts=( struct _tagged_string)({
char* _temp682=( char*)"int"; struct _tagged_string _temp683; _temp683.curr=
_temp682; _temp683.base= _temp682; _temp683.last_plus_one= _temp682 + 4;
_temp683;}); goto _LL659; _LL667: ts=( struct _tagged_string)({ char* _temp684=(
char*)"long long"; struct _tagged_string _temp685; _temp685.curr= _temp684;
_temp685.base= _temp684; _temp685.last_plus_one= _temp684 + 10; _temp685;});
goto _LL659; _LL659:;} s= Cyc_PP_text(({ struct _tagged_string _temp686= sns;
struct _tagged_string _temp687= ts; xprintf("%.*s%.*s", _temp686.last_plus_one -
_temp686.curr, _temp686.curr, _temp687.last_plus_one - _temp687.curr, _temp687.curr);}));
goto _LL443;} _LL463: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp688=(
char*)"float"; struct _tagged_string _temp689; _temp689.curr= _temp688; _temp689.base=
_temp688; _temp689.last_plus_one= _temp688 + 6; _temp689;})); goto _LL443;
_LL465: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp690=( char*)"double";
struct _tagged_string _temp691; _temp691.curr= _temp690; _temp691.base= _temp690;
_temp691.last_plus_one= _temp690 + 7; _temp691;})); goto _LL443; _LL467: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp692=( char*)"$"; struct
_tagged_string _temp693; _temp693.curr= _temp692; _temp693.base= _temp692;
_temp693.last_plus_one= _temp692 + 2; _temp693;})), Cyc_Absynpp_args2doc(
_temp520)); goto _LL443; _LL469: if( _temp526 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp694=( char*)"struct "; struct _tagged_string
_temp695; _temp695.curr= _temp694; _temp695.base= _temp694; _temp695.last_plus_one=
_temp694 + 8; _temp695;})), Cyc_Absynpp_tps2doc( _temp524));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp696=( char*)"struct "; struct
_tagged_string _temp697; _temp697.curr= _temp696; _temp697.base= _temp696;
_temp697.last_plus_one= _temp696 + 8; _temp697;})), _temp526 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp698= _temp526;
if( _temp698 == 0){ _throw( Null_Exception);} _temp698;})), Cyc_Absynpp_tps2doc(
_temp524));} goto _LL443; _LL471: if( _temp532 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp699=( char*)"union "; struct _tagged_string
_temp700; _temp700.curr= _temp699; _temp700.base= _temp699; _temp700.last_plus_one=
_temp699 + 7; _temp700;})), Cyc_Absynpp_tps2doc( _temp530));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp701=( char*)"union "; struct
_tagged_string _temp702; _temp702.curr= _temp701; _temp702.base= _temp701;
_temp702.last_plus_one= _temp701 + 7; _temp702;})), _temp532 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp703= _temp532;
if( _temp703 == 0){ _throw( Null_Exception);} _temp703;})), Cyc_Absynpp_tps2doc(
_temp530));} goto _LL443; _LL473: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp704=( char*)"enum "; struct _tagged_string _temp705;
_temp705.curr= _temp704; _temp705.base= _temp704; _temp705.last_plus_one=
_temp704 + 6; _temp705;})), Cyc_Absynpp_qvar2doc( _temp536)); goto _LL443;
_LL475: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp542), Cyc_Absynpp_tps2doc(
_temp540)); goto _LL443; _LL477: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp706=( char*)"region_t<"; struct _tagged_string
_temp707; _temp707.curr= _temp706; _temp707.base= _temp706; _temp707.last_plus_one=
_temp706 + 10; _temp707;})), Cyc_Absynpp_rgn2doc( _temp544), Cyc_PP_text((
struct _tagged_string)({ char* _temp708=( char*)">"; struct _tagged_string
_temp709; _temp709.curr= _temp708; _temp709.base= _temp708; _temp709.last_plus_one=
_temp708 + 2; _temp709;}))); goto _LL443; _LL479: s= Cyc_Absynpp_rgn2doc( t);
goto _LL443; _LL481: s= Cyc_Absynpp_eff2doc( t); goto _LL443; _LL483: s= Cyc_Absynpp_eff2doc(
t); goto _LL443; _LL443:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp710= vo; if( _temp710 == 0){
_throw( Null_Exception);} _temp710->v;})));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc(
struct _tuple1* t){ struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt*
_temp711=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp711->v=( void*) Cyc_PP_text(*(( struct _tagged_string*)({ struct Cyc_Core_Opt*
_temp712=(* t).f1; if( _temp712 == 0){ _throw( Null_Exception);} _temp712->v;})));
_temp711;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt*
effopt){ if( effopt == 0){ if( varargs){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp713=( char*)"("; struct _tagged_string _temp714;
_temp714.curr= _temp713; _temp714.base= _temp713; _temp714.last_plus_one=
_temp713 + 2; _temp714;}),( struct _tagged_string)({ char* _temp715=( char*)")";
struct _tagged_string _temp716; _temp716.curr= _temp715; _temp716.base= _temp715;
_temp716.last_plus_one= _temp715 + 2; _temp716;}),( struct _tagged_string)({
char* _temp717=( char*)","; struct _tagged_string _temp718; _temp718.curr=
_temp717; _temp718.base= _temp717; _temp718.last_plus_one= _temp717 + 2;
_temp718;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)((( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args),({ struct Cyc_List_List* _temp719=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp719->hd=( void*) Cyc_PP_text(( struct
_tagged_string)({ char* _temp720=( char*)"..."; struct _tagged_string _temp721;
_temp721.curr= _temp720; _temp721.base= _temp720; _temp721.last_plus_one=
_temp720 + 4; _temp721;})); _temp719->tl= 0; _temp719;})));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp722=( char*)"("; struct _tagged_string
_temp723; _temp723.curr= _temp722; _temp723.base= _temp722; _temp723.last_plus_one=
_temp722 + 2; _temp723;}),( struct _tagged_string)({ char* _temp724=( char*)")";
struct _tagged_string _temp725; _temp725.curr= _temp724; _temp725.base= _temp724;
_temp725.last_plus_one= _temp724 + 2; _temp725;}),( struct _tagged_string)({
char* _temp726=( char*)","; struct _tagged_string _temp727; _temp727.curr=
_temp726; _temp727.base= _temp726; _temp727.last_plus_one= _temp726 + 2;
_temp727;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));}} else{
if( varargs){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp728=(
char*)"("; struct _tagged_string _temp729; _temp729.curr= _temp728; _temp729.base=
_temp728; _temp729.last_plus_one= _temp728 + 2; _temp729;}),( struct
_tagged_string)({ char* _temp730=( char*)")"; struct _tagged_string _temp731;
_temp731.curr= _temp730; _temp731.base= _temp730; _temp731.last_plus_one=
_temp730 + 2; _temp731;}),( struct _tagged_string)({ char* _temp732=( char*)";";
struct _tagged_string _temp733; _temp733.curr= _temp732; _temp733.base= _temp732;
_temp733.last_plus_one= _temp732 + 2; _temp733;}),({ struct Cyc_List_List*
_temp734=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp734->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp737=( char*)""; struct _tagged_string _temp738; _temp738.curr= _temp737;
_temp738.base= _temp737; _temp738.last_plus_one= _temp737 + 1; _temp738;}),(
struct _tagged_string)({ char* _temp739=( char*)""; struct _tagged_string
_temp740; _temp740.curr= _temp739; _temp740.base= _temp739; _temp740.last_plus_one=
_temp739 + 1; _temp740;}),( struct _tagged_string)({ char* _temp741=( char*)",";
struct _tagged_string _temp742; _temp742.curr= _temp741; _temp742.base= _temp741;
_temp742.last_plus_one= _temp741 + 2; _temp742;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args),({ struct Cyc_List_List*
_temp743=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp743->hd=( void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp744=(
char*)"..."; struct _tagged_string _temp745; _temp745.curr= _temp744; _temp745.base=
_temp744; _temp745.last_plus_one= _temp744 + 4; _temp745;})); _temp743->tl= 0;
_temp743;}))); _temp734->tl=({ struct Cyc_List_List* _temp735=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp735->hd=( void*) Cyc_Absynpp_eff2doc((
void*)({ struct Cyc_Core_Opt* _temp736= effopt; if( _temp736 == 0){ _throw(
Null_Exception);} _temp736->v;})); _temp735->tl= 0; _temp735;}); _temp734;}));}
else{ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp746=( char*)"(";
struct _tagged_string _temp747; _temp747.curr= _temp746; _temp747.base= _temp746;
_temp747.last_plus_one= _temp746 + 2; _temp747;}),( struct _tagged_string)({
char* _temp748=( char*)")"; struct _tagged_string _temp749; _temp749.curr=
_temp748; _temp749.base= _temp748; _temp749.last_plus_one= _temp748 + 2;
_temp749;}),( struct _tagged_string)({ char* _temp750=( char*)";"; struct
_tagged_string _temp751; _temp751.curr= _temp750; _temp751.base= _temp750;
_temp751.last_plus_one= _temp750 + 2; _temp751;}),({ struct Cyc_List_List*
_temp752=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp752->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp755=( char*)""; struct _tagged_string _temp756; _temp756.curr= _temp755;
_temp756.base= _temp755; _temp756.last_plus_one= _temp755 + 1; _temp756;}),(
struct _tagged_string)({ char* _temp757=( char*)""; struct _tagged_string
_temp758; _temp758.curr= _temp757; _temp758.base= _temp757; _temp758.last_plus_one=
_temp757 + 1; _temp758;}),( struct _tagged_string)({ char* _temp759=( char*)",";
struct _tagged_string _temp760; _temp760.curr= _temp759; _temp760.base= _temp759;
_temp760.last_plus_one= _temp759 + 2; _temp760;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_funarg2doc, args)); _temp752->tl=({ struct Cyc_List_List* _temp753=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp753->hd=(
void*) Cyc_Absynpp_eff2doc(( void*)({ struct Cyc_Core_Opt* _temp754= effopt; if(
_temp754 == 0){ _throw( Null_Exception);} _temp754->v;})); _temp753->tl= 0;
_temp753;}); _temp752;}));}}} struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct
_tuple3* arg){ return({ struct _tuple1* _temp761=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp761->f1=({ struct Cyc_Core_Opt* _temp762=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp762->v=( void*)(*
arg).f1; _temp762;}); _temp761->f2=(* arg).f2; _temp761->f3=(* arg).f3; _temp761;});}
struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct _tagged_string* v){ return Cyc_PP_text(*
v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
prefix= 0;{ void* _temp763=(* q).f1; struct Cyc_List_List* _temp775; struct Cyc_List_List*
_temp777; struct Cyc_List_List* _temp779; struct Cyc_List_List* _temp781; _LL765:
if( _temp763 ==( void*) Cyc_Absyn_Loc_n){ goto _LL766;} else{ goto _LL767;}
_LL767: if(( unsigned int) _temp763 > 1u?*(( int*) _temp763) == Cyc_Absyn_Rel_n:
0){ _LL776: _temp775=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp763)->f1; if( _temp775 == 0){ goto _LL768;} else{ goto _LL769;}} else{ goto
_LL769;} _LL769: if(( unsigned int) _temp763 > 1u?*(( int*) _temp763) == Cyc_Absyn_Abs_n:
0){ _LL778: _temp777=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp763)->f1; if( _temp777 == 0){ goto _LL770;} else{ goto _LL771;}} else{ goto
_LL771;} _LL771: if(( unsigned int) _temp763 > 1u?*(( int*) _temp763) == Cyc_Absyn_Rel_n:
0){ _LL780: _temp779=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp763)->f1; goto _LL772;} else{ goto _LL773;} _LL773: if(( unsigned int)
_temp763 > 1u?*(( int*) _temp763) == Cyc_Absyn_Abs_n: 0){ _LL782: _temp781=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp763)->f1; goto
_LL774;} else{ goto _LL764;} _LL766: goto _LL768; _LL768: goto _LL770; _LL770:
return Cyc_Absynpp_var2doc((* q).f2); _LL772: _temp781= _temp779; goto _LL774;
_LL774: prefix= _temp781; goto _LL764; _LL764:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( prefix,({ struct
Cyc_List_List* _temp783=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp783->hd=( void*)(* q).f2; _temp783->tl= 0; _temp783;})),( struct
_tagged_string)({ char* _temp784=( char*)"_"; struct _tagged_string _temp785;
_temp785.curr= _temp784; _temp785.base= _temp784; _temp785.last_plus_one=
_temp784 + 2; _temp785;})));} else{ if((( int(*)( int(* cmp)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, prefix, Cyc_Absynpp_curr_namespace)){
return Cyc_Absynpp_var2doc((* q).f2);} else{ return Cyc_PP_text( Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
prefix,({ struct Cyc_List_List* _temp786=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp786->hd=( void*)(* q).f2; _temp786->tl= 0;
_temp786;})),( struct _tagged_string)({ char* _temp787=( char*)"::"; struct
_tagged_string _temp788; _temp788.curr= _temp787; _temp788.base= _temp787;
_temp788.last_plus_one= _temp787 + 3; _temp788;})));}}} struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc(
struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){ return Cyc_Absynpp_qvar2doc(
v);}{ void* _temp789=(* v).f1; struct Cyc_List_List* _temp795; _LL791: if((
unsigned int) _temp789 > 1u?*(( int*) _temp789) == Cyc_Absyn_Abs_n: 0){ _LL796:
_temp795=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp789)->f1;
goto _LL792;} else{ goto _LL793;} _LL793: goto _LL794; _LL792: if((( int(*)( int(*
cmp)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp795,
Cyc_Absynpp_curr_namespace) == 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{
return Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp797=(
char*)"/* wrong namespace : */ "; struct _tagged_string _temp798; _temp798.curr=
_temp797; _temp798.base= _temp797; _temp798.last_plus_one= _temp797 + 25;
_temp798;})), Cyc_Absynpp_qvar2doc( v));} _LL794: return Cyc_Absynpp_qvar2doc( v);
_LL790:;}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*
e){ void* _temp799=( void*) e->r; void* _temp869; void* _temp871; struct _tuple0*
_temp873; struct _tuple0* _temp875; struct Cyc_List_List* _temp877; void*
_temp879; struct Cyc_Absyn_Exp* _temp881; struct Cyc_Core_Opt* _temp883; struct
Cyc_Absyn_Exp* _temp885; void* _temp887; struct Cyc_Absyn_Exp* _temp889; struct
Cyc_Absyn_Exp* _temp891; struct Cyc_Absyn_Exp* _temp893; struct Cyc_Absyn_Exp*
_temp895; struct Cyc_Absyn_Exp* _temp897; struct Cyc_Absyn_Exp* _temp899; struct
Cyc_List_List* _temp901; struct Cyc_Absyn_Exp* _temp903; struct Cyc_List_List*
_temp905; struct Cyc_Absyn_Exp* _temp907; struct Cyc_Absyn_Exp* _temp909; struct
Cyc_Absyn_Exp* _temp911; struct Cyc_List_List* _temp913; struct Cyc_Absyn_Exp*
_temp915; struct Cyc_Absyn_Exp* _temp917; void* _temp919; struct Cyc_Absyn_Exp*
_temp921; struct Cyc_Absyn_Exp* _temp923; struct Cyc_Absyn_Exp* _temp925; void*
_temp927; struct Cyc_Absyn_Exp* _temp929; struct Cyc_Absyn_Exp* _temp931; struct
_tagged_string* _temp933; struct Cyc_Absyn_Exp* _temp935; struct _tagged_string*
_temp937; struct Cyc_Absyn_Exp* _temp939; struct Cyc_Absyn_Exp* _temp941; struct
Cyc_Absyn_Exp* _temp943; struct Cyc_List_List* _temp945; struct Cyc_List_List*
_temp947; struct _tuple1* _temp949; struct Cyc_List_List* _temp951; struct Cyc_Absyn_Exp*
_temp953; struct Cyc_Absyn_Exp* _temp955; struct Cyc_Absyn_Vardecl* _temp957;
struct Cyc_Absyn_Structdecl* _temp959; struct Cyc_List_List* _temp961; struct
Cyc_Core_Opt* _temp963; struct _tuple0* _temp965; struct Cyc_Absyn_Tunionfield*
_temp967; struct Cyc_Absyn_Tuniondecl* _temp969; struct Cyc_List_List* _temp971;
struct Cyc_Core_Opt* _temp973; struct Cyc_Core_Opt* _temp975; struct Cyc_Absyn_Enumfield*
_temp977; struct Cyc_Absyn_Enumdecl* _temp979; struct _tuple0* _temp981; void*
_temp983; struct Cyc_Absyn_Exp* _temp985; struct Cyc_List_List* _temp987; struct
Cyc_Core_Opt* _temp989; struct Cyc_Absyn_Stmt* _temp991; struct Cyc_Absyn_Fndecl*
_temp993; struct Cyc_Absyn_Exp* _temp995; _LL801: if(*(( int*) _temp799) == Cyc_Absyn_Const_e){
_LL870: _temp869=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp799)->f1;
goto _LL802;} else{ goto _LL803;} _LL803: if(*(( int*) _temp799) == Cyc_Absyn_Var_e){
_LL874: _temp873=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp799)->f1;
goto _LL872; _LL872: _temp871=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp799)->f2; goto _LL804;} else{ goto _LL805;} _LL805: if(*(( int*) _temp799)
== Cyc_Absyn_UnknownId_e){ _LL876: _temp875=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp799)->f1; goto _LL806;} else{ goto _LL807;} _LL807: if(*(( int*) _temp799)
== Cyc_Absyn_Primop_e){ _LL880: _temp879=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp799)->f1; goto _LL878; _LL878: _temp877=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp799)->f2; goto _LL808;} else{ goto _LL809;}
_LL809: if(*(( int*) _temp799) == Cyc_Absyn_AssignOp_e){ _LL886: _temp885=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp799)->f1;
goto _LL884; _LL884: _temp883=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp799)->f2; goto _LL882; _LL882: _temp881=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp799)->f3; goto _LL810;} else{ goto _LL811;}
_LL811: if(*(( int*) _temp799) == Cyc_Absyn_Increment_e){ _LL890: _temp889=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp799)->f1;
goto _LL888; _LL888: _temp887=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp799)->f2; goto _LL812;} else{ goto _LL813;} _LL813: if(*(( int*) _temp799)
== Cyc_Absyn_Conditional_e){ _LL896: _temp895=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp799)->f1; goto _LL894; _LL894: _temp893=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp799)->f2;
goto _LL892; _LL892: _temp891=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp799)->f3; goto _LL814;} else{ goto _LL815;} _LL815: if(*(( int*) _temp799)
== Cyc_Absyn_SeqExp_e){ _LL900: _temp899=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp799)->f1; goto _LL898; _LL898: _temp897=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp799)->f2; goto _LL816;} else{ goto _LL817;}
_LL817: if(*(( int*) _temp799) == Cyc_Absyn_UnknownCall_e){ _LL904: _temp903=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp799)->f1;
goto _LL902; _LL902: _temp901=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp799)->f2; goto _LL818;} else{ goto _LL819;} _LL819: if(*(( int*) _temp799)
== Cyc_Absyn_FnCall_e){ _LL908: _temp907=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp799)->f1; goto _LL906; _LL906: _temp905=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp799)->f2; goto _LL820;} else{ goto _LL821;}
_LL821: if(*(( int*) _temp799) == Cyc_Absyn_Throw_e){ _LL910: _temp909=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp799)->f1; goto _LL822;}
else{ goto _LL823;} _LL823: if(*(( int*) _temp799) == Cyc_Absyn_NoInstantiate_e){
_LL912: _temp911=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp799)->f1; goto _LL824;} else{ goto _LL825;} _LL825: if(*(( int*) _temp799)
== Cyc_Absyn_Instantiate_e){ _LL916: _temp915=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp799)->f1; goto _LL914; _LL914: _temp913=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp799)->f2;
goto _LL826;} else{ goto _LL827;} _LL827: if(*(( int*) _temp799) == Cyc_Absyn_Cast_e){
_LL920: _temp919=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp799)->f1; goto
_LL918; _LL918: _temp917=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp799)->f2; goto _LL828;} else{ goto _LL829;} _LL829: if(*(( int*) _temp799)
== Cyc_Absyn_New_e){ _LL924: _temp923=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp799)->f1; goto _LL922; _LL922: _temp921=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_New_e_struct*) _temp799)->f2; goto _LL830;} else{ goto _LL831;} _LL831:
if(*(( int*) _temp799) == Cyc_Absyn_Address_e){ _LL926: _temp925=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp799)->f1; goto _LL832;} else{ goto
_LL833;} _LL833: if(*(( int*) _temp799) == Cyc_Absyn_Sizeoftyp_e){ _LL928:
_temp927=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp799)->f1; goto
_LL834;} else{ goto _LL835;} _LL835: if(*(( int*) _temp799) == Cyc_Absyn_Sizeofexp_e){
_LL930: _temp929=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp799)->f1; goto _LL836;} else{ goto _LL837;} _LL837: if(*(( int*) _temp799)
== Cyc_Absyn_Deref_e){ _LL932: _temp931=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp799)->f1; goto _LL838;} else{ goto _LL839;} _LL839: if(*(( int*) _temp799)
== Cyc_Absyn_StructMember_e){ _LL936: _temp935=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp799)->f1; goto _LL934; _LL934: _temp933=(
struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp799)->f2;
goto _LL840;} else{ goto _LL841;} _LL841: if(*(( int*) _temp799) == Cyc_Absyn_StructArrow_e){
_LL940: _temp939=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp799)->f1; goto _LL938; _LL938: _temp937=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp799)->f2; goto _LL842;} else{ goto _LL843;}
_LL843: if(*(( int*) _temp799) == Cyc_Absyn_Subscript_e){ _LL944: _temp943=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp799)->f1;
goto _LL942; _LL942: _temp941=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp799)->f2; goto _LL844;} else{ goto _LL845;} _LL845: if(*(( int*) _temp799)
== Cyc_Absyn_Tuple_e){ _LL946: _temp945=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp799)->f1; goto _LL846;} else{ goto _LL847;} _LL847: if(*(( int*) _temp799)
== Cyc_Absyn_CompoundLit_e){ _LL950: _temp949=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp799)->f1; goto _LL948; _LL948: _temp947=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp799)->f2; goto _LL848;} else{ goto _LL849;}
_LL849: if(*(( int*) _temp799) == Cyc_Absyn_Array_e){ _LL952: _temp951=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp799)->f1; goto _LL850;}
else{ goto _LL851;} _LL851: if(*(( int*) _temp799) == Cyc_Absyn_Comprehension_e){
_LL958: _temp957=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp799)->f1; goto _LL956; _LL956: _temp955=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp799)->f2; goto _LL954; _LL954: _temp953=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp799)->f3;
goto _LL852;} else{ goto _LL853;} _LL853: if(*(( int*) _temp799) == Cyc_Absyn_Struct_e){
_LL966: _temp965=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp799)->f1; goto _LL964; _LL964: _temp963=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp799)->f2; goto _LL962; _LL962: _temp961=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp799)->f3; goto
_LL960; _LL960: _temp959=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp799)->f4; goto _LL854;} else{ goto _LL855;} _LL855: if(*(( int*) _temp799)
== Cyc_Absyn_Tunion_e){ _LL976: _temp975=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp799)->f1; goto _LL974; _LL974: _temp973=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp799)->f2; goto _LL972; _LL972: _temp971=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp799)->f3; goto
_LL970; _LL970: _temp969=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp799)->f4; goto _LL968; _LL968: _temp967=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp799)->f5; goto _LL856;} else{ goto
_LL857;} _LL857: if(*(( int*) _temp799) == Cyc_Absyn_Enum_e){ _LL982: _temp981=(
struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp799)->f1; goto _LL980;
_LL980: _temp979=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp799)->f2; goto _LL978; _LL978: _temp977=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp799)->f3; goto _LL858;} else{ goto _LL859;}
_LL859: if(*(( int*) _temp799) == Cyc_Absyn_Malloc_e){ _LL986: _temp985=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp799)->f1; goto _LL984;
_LL984: _temp983=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp799)->f2;
goto _LL860;} else{ goto _LL861;} _LL861: if(*(( int*) _temp799) == Cyc_Absyn_UnresolvedMem_e){
_LL990: _temp989=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp799)->f1; goto _LL988; _LL988: _temp987=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp799)->f2; goto _LL862;} else{ goto
_LL863;} _LL863: if(*(( int*) _temp799) == Cyc_Absyn_StmtExp_e){ _LL992:
_temp991=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp799)->f1; goto _LL864;} else{ goto _LL865;} _LL865: if(*(( int*) _temp799)
== Cyc_Absyn_Codegen_e){ _LL994: _temp993=( struct Cyc_Absyn_Fndecl*)(( struct
Cyc_Absyn_Codegen_e_struct*) _temp799)->f1; goto _LL866;} else{ goto _LL867;}
_LL867: if(*(( int*) _temp799) == Cyc_Absyn_Fill_e){ _LL996: _temp995=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp799)->f1; goto _LL868;}
else{ goto _LL800;} _LL802: return 10000; _LL804: return 10000; _LL806: return
10000; _LL808: { void* _temp997= _temp879; _LL999: if( _temp997 ==( void*) Cyc_Absyn_Plus){
goto _LL1000;} else{ goto _LL1001;} _LL1001: if( _temp997 ==( void*) Cyc_Absyn_Times){
goto _LL1002;} else{ goto _LL1003;} _LL1003: if( _temp997 ==( void*) Cyc_Absyn_Minus){
goto _LL1004;} else{ goto _LL1005;} _LL1005: if( _temp997 ==( void*) Cyc_Absyn_Div){
goto _LL1006;} else{ goto _LL1007;} _LL1007: if( _temp997 ==( void*) Cyc_Absyn_Mod){
goto _LL1008;} else{ goto _LL1009;} _LL1009: if( _temp997 ==( void*) Cyc_Absyn_Eq){
goto _LL1010;} else{ goto _LL1011;} _LL1011: if( _temp997 ==( void*) Cyc_Absyn_Neq){
goto _LL1012;} else{ goto _LL1013;} _LL1013: if( _temp997 ==( void*) Cyc_Absyn_Gt){
goto _LL1014;} else{ goto _LL1015;} _LL1015: if( _temp997 ==( void*) Cyc_Absyn_Lt){
goto _LL1016;} else{ goto _LL1017;} _LL1017: if( _temp997 ==( void*) Cyc_Absyn_Gte){
goto _LL1018;} else{ goto _LL1019;} _LL1019: if( _temp997 ==( void*) Cyc_Absyn_Lte){
goto _LL1020;} else{ goto _LL1021;} _LL1021: if( _temp997 ==( void*) Cyc_Absyn_Not){
goto _LL1022;} else{ goto _LL1023;} _LL1023: if( _temp997 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1024;} else{ goto _LL1025;} _LL1025: if( _temp997 ==( void*) Cyc_Absyn_Bitand){
goto _LL1026;} else{ goto _LL1027;} _LL1027: if( _temp997 ==( void*) Cyc_Absyn_Bitor){
goto _LL1028;} else{ goto _LL1029;} _LL1029: if( _temp997 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1030;} else{ goto _LL1031;} _LL1031: if( _temp997 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1032;} else{ goto _LL1033;} _LL1033: if( _temp997 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1034;} else{ goto _LL1035;} _LL1035: if( _temp997 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1036;} else{ goto _LL1037;} _LL1037: if( _temp997 ==( void*) Cyc_Absyn_Size){
goto _LL1038;} else{ goto _LL1039;} _LL1039: if( _temp997 ==( void*) Cyc_Absyn_Printf){
goto _LL1040;} else{ goto _LL1041;} _LL1041: if( _temp997 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1042;} else{ goto _LL1043;} _LL1043: if( _temp997 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1044;} else{ goto _LL1045;} _LL1045: if( _temp997 ==( void*) Cyc_Absyn_Scanf){
goto _LL1046;} else{ goto _LL1047;} _LL1047: if( _temp997 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1048;} else{ goto _LL1049;} _LL1049: if( _temp997 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1050;} else{ goto _LL998;} _LL1000: return 100; _LL1002: return 110;
_LL1004: return 100; _LL1006: return 110; _LL1008: return 110; _LL1010: return
70; _LL1012: return 70; _LL1014: return 80; _LL1016: return 80; _LL1018: return
80; _LL1020: return 80; _LL1022: return 130; _LL1024: return 130; _LL1026:
return 60; _LL1028: return 40; _LL1030: return 50; _LL1032: return 90; _LL1034:
return 80; _LL1036: return 80; _LL1038: return 140; _LL1040: return 130; _LL1042:
return 130; _LL1044: return 130; _LL1046: return 130; _LL1048: return 130;
_LL1050: return 130; _LL998:;} _LL810: return 20; _LL812: return 20; _LL814:
return 30; _LL816: return 10; _LL818: return 140; _LL820: return 140; _LL822:
return 130; _LL824: return Cyc_Absynpp_exp_prec( _temp911); _LL826: return Cyc_Absynpp_exp_prec(
_temp915); _LL828: return 120; _LL830: return 15; _LL832: return 130; _LL834:
return 130; _LL836: return 130; _LL838: return 130; _LL840: return 140; _LL842:
return 140; _LL844: return 140; _LL846: return 150; _LL848: goto _LL850; _LL850:
goto _LL852; _LL852: goto _LL854; _LL854: goto _LL856; _LL856: goto _LL858;
_LL858: goto _LL860; _LL860: return 140; _LL862: return 140; _LL864: return
10000; _LL866: return 140; _LL868: return 140; _LL800:;} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec( 0, e);} struct
_tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e);
struct Cyc_PP_Doc* s;{ void* _temp1051=( void*) e->r; void* _temp1121; void*
_temp1123; struct _tuple0* _temp1125; struct _tuple0* _temp1127; struct Cyc_List_List*
_temp1129; void* _temp1131; struct Cyc_Absyn_Exp* _temp1133; struct Cyc_Core_Opt*
_temp1135; struct Cyc_Absyn_Exp* _temp1137; void* _temp1139; struct Cyc_Absyn_Exp*
_temp1141; struct Cyc_Absyn_Exp* _temp1143; struct Cyc_Absyn_Exp* _temp1145;
struct Cyc_Absyn_Exp* _temp1147; struct Cyc_Absyn_Exp* _temp1149; struct Cyc_Absyn_Exp*
_temp1151; struct Cyc_List_List* _temp1153; struct Cyc_Absyn_Exp* _temp1155;
struct Cyc_List_List* _temp1157; struct Cyc_Absyn_Exp* _temp1159; struct Cyc_Absyn_Exp*
_temp1161; struct Cyc_Absyn_Exp* _temp1163; struct Cyc_List_List* _temp1165;
struct Cyc_Absyn_Exp* _temp1167; struct Cyc_Absyn_Exp* _temp1169; void*
_temp1171; struct Cyc_Absyn_Exp* _temp1173; struct Cyc_Absyn_Exp* _temp1175;
struct Cyc_Absyn_Exp* _temp1177; void* _temp1179; struct Cyc_Absyn_Exp*
_temp1181; struct Cyc_Absyn_Exp* _temp1183; struct _tagged_string* _temp1185;
struct Cyc_Absyn_Exp* _temp1187; struct _tagged_string* _temp1189; struct Cyc_Absyn_Exp*
_temp1191; struct Cyc_Absyn_Exp* _temp1193; struct Cyc_Absyn_Exp* _temp1195;
struct Cyc_List_List* _temp1197; struct Cyc_List_List* _temp1199; struct _tuple1*
_temp1201; struct Cyc_List_List* _temp1203; struct Cyc_Absyn_Exp* _temp1205;
struct Cyc_Absyn_Exp* _temp1207; struct Cyc_Absyn_Vardecl* _temp1209; struct Cyc_Absyn_Structdecl*
_temp1211; struct Cyc_List_List* _temp1213; struct Cyc_Core_Opt* _temp1215;
struct _tuple0* _temp1217; struct Cyc_Absyn_Tunionfield* _temp1219; struct Cyc_Absyn_Tuniondecl*
_temp1221; struct Cyc_List_List* _temp1223; struct Cyc_Core_Opt* _temp1225;
struct Cyc_Core_Opt* _temp1227; struct Cyc_Absyn_Enumfield* _temp1229; struct
Cyc_Absyn_Enumdecl* _temp1231; struct _tuple0* _temp1233; void* _temp1235;
struct Cyc_Absyn_Exp* _temp1237; struct Cyc_List_List* _temp1239; struct Cyc_Core_Opt*
_temp1241; struct Cyc_Absyn_Stmt* _temp1243; struct Cyc_Absyn_Fndecl* _temp1245;
struct Cyc_Absyn_Exp* _temp1247; _LL1053: if(*(( int*) _temp1051) == Cyc_Absyn_Const_e){
_LL1122: _temp1121=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1051)->f1;
goto _LL1054;} else{ goto _LL1055;} _LL1055: if(*(( int*) _temp1051) == Cyc_Absyn_Var_e){
_LL1126: _temp1125=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1051)->f1; goto _LL1124; _LL1124: _temp1123=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1051)->f2; goto _LL1056;} else{ goto _LL1057;} _LL1057: if(*(( int*)
_temp1051) == Cyc_Absyn_UnknownId_e){ _LL1128: _temp1127=( struct _tuple0*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1051)->f1; goto _LL1058;} else{ goto
_LL1059;} _LL1059: if(*(( int*) _temp1051) == Cyc_Absyn_Primop_e){ _LL1132:
_temp1131=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1051)->f1; goto
_LL1130; _LL1130: _temp1129=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1051)->f2; goto _LL1060;} else{ goto _LL1061;} _LL1061: if(*(( int*)
_temp1051) == Cyc_Absyn_AssignOp_e){ _LL1138: _temp1137=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1051)->f1; goto _LL1136; _LL1136:
_temp1135=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1051)->f2; goto _LL1134; _LL1134: _temp1133=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1051)->f3; goto _LL1062;} else{ goto
_LL1063;} _LL1063: if(*(( int*) _temp1051) == Cyc_Absyn_Increment_e){ _LL1142:
_temp1141=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1051)->f1; goto _LL1140; _LL1140: _temp1139=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1051)->f2; goto _LL1064;} else{ goto _LL1065;} _LL1065: if(*(( int*)
_temp1051) == Cyc_Absyn_Conditional_e){ _LL1148: _temp1147=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1051)->f1; goto _LL1146; _LL1146:
_temp1145=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1051)->f2; goto _LL1144; _LL1144: _temp1143=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1051)->f3; goto _LL1066;} else{
goto _LL1067;} _LL1067: if(*(( int*) _temp1051) == Cyc_Absyn_SeqExp_e){ _LL1152:
_temp1151=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1051)->f1; goto _LL1150; _LL1150: _temp1149=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1051)->f2; goto _LL1068;} else{ goto
_LL1069;} _LL1069: if(*(( int*) _temp1051) == Cyc_Absyn_UnknownCall_e){ _LL1156:
_temp1155=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1051)->f1; goto _LL1154; _LL1154: _temp1153=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1051)->f2; goto _LL1070;} else{
goto _LL1071;} _LL1071: if(*(( int*) _temp1051) == Cyc_Absyn_FnCall_e){ _LL1160:
_temp1159=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1051)->f1; goto _LL1158; _LL1158: _temp1157=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1051)->f2; goto _LL1072;} else{ goto
_LL1073;} _LL1073: if(*(( int*) _temp1051) == Cyc_Absyn_Throw_e){ _LL1162:
_temp1161=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1051)->f1;
goto _LL1074;} else{ goto _LL1075;} _LL1075: if(*(( int*) _temp1051) == Cyc_Absyn_NoInstantiate_e){
_LL1164: _temp1163=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1051)->f1; goto _LL1076;} else{ goto _LL1077;} _LL1077: if(*(( int*)
_temp1051) == Cyc_Absyn_Instantiate_e){ _LL1168: _temp1167=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1051)->f1; goto _LL1166; _LL1166:
_temp1165=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1051)->f2; goto _LL1078;} else{ goto _LL1079;} _LL1079: if(*(( int*)
_temp1051) == Cyc_Absyn_Cast_e){ _LL1172: _temp1171=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1051)->f1; goto _LL1170; _LL1170: _temp1169=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Cast_e_struct*) _temp1051)->f2; goto _LL1080;} else{ goto
_LL1081;} _LL1081: if(*(( int*) _temp1051) == Cyc_Absyn_Address_e){ _LL1174:
_temp1173=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp1051)->f1; goto _LL1082;} else{ goto _LL1083;} _LL1083: if(*(( int*)
_temp1051) == Cyc_Absyn_New_e){ _LL1178: _temp1177=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1051)->f1; goto _LL1176; _LL1176: _temp1175=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1051)->f2; goto
_LL1084;} else{ goto _LL1085;} _LL1085: if(*(( int*) _temp1051) == Cyc_Absyn_Sizeoftyp_e){
_LL1180: _temp1179=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1051)->f1;
goto _LL1086;} else{ goto _LL1087;} _LL1087: if(*(( int*) _temp1051) == Cyc_Absyn_Sizeofexp_e){
_LL1182: _temp1181=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1051)->f1; goto _LL1088;} else{ goto _LL1089;} _LL1089: if(*(( int*)
_temp1051) == Cyc_Absyn_Deref_e){ _LL1184: _temp1183=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp1051)->f1; goto _LL1090;} else{ goto
_LL1091;} _LL1091: if(*(( int*) _temp1051) == Cyc_Absyn_StructMember_e){ _LL1188:
_temp1187=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1051)->f1; goto _LL1186; _LL1186: _temp1185=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1051)->f2; goto _LL1092;} else{
goto _LL1093;} _LL1093: if(*(( int*) _temp1051) == Cyc_Absyn_StructArrow_e){
_LL1192: _temp1191=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1051)->f1; goto _LL1190; _LL1190: _temp1189=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1051)->f2; goto _LL1094;} else{
goto _LL1095;} _LL1095: if(*(( int*) _temp1051) == Cyc_Absyn_Subscript_e){
_LL1196: _temp1195=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1051)->f1; goto _LL1194; _LL1194: _temp1193=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1051)->f2; goto _LL1096;} else{ goto
_LL1097;} _LL1097: if(*(( int*) _temp1051) == Cyc_Absyn_Tuple_e){ _LL1198:
_temp1197=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1051)->f1;
goto _LL1098;} else{ goto _LL1099;} _LL1099: if(*(( int*) _temp1051) == Cyc_Absyn_CompoundLit_e){
_LL1202: _temp1201=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1051)->f1; goto _LL1200; _LL1200: _temp1199=( struct Cyc_List_List*)((
struct Cyc_Absyn_CompoundLit_e_struct*) _temp1051)->f2; goto _LL1100;} else{
goto _LL1101;} _LL1101: if(*(( int*) _temp1051) == Cyc_Absyn_Array_e){ _LL1204:
_temp1203=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1051)->f1;
goto _LL1102;} else{ goto _LL1103;} _LL1103: if(*(( int*) _temp1051) == Cyc_Absyn_Comprehension_e){
_LL1210: _temp1209=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1051)->f1; goto _LL1208; _LL1208: _temp1207=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1051)->f2; goto _LL1206; _LL1206:
_temp1205=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1051)->f3; goto _LL1104;} else{ goto _LL1105;} _LL1105: if(*(( int*)
_temp1051) == Cyc_Absyn_Struct_e){ _LL1218: _temp1217=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1051)->f1; goto _LL1216; _LL1216:
_temp1215=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1051)->f2;
goto _LL1214; _LL1214: _temp1213=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1051)->f3; goto _LL1212; _LL1212: _temp1211=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1051)->f4; goto _LL1106;} else{ goto
_LL1107;} _LL1107: if(*(( int*) _temp1051) == Cyc_Absyn_Tunion_e){ _LL1228:
_temp1227=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1051)->f1;
goto _LL1226; _LL1226: _temp1225=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1051)->f2; goto _LL1224; _LL1224: _temp1223=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1051)->f3; goto _LL1222; _LL1222:
_temp1221=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1051)->f4; goto _LL1220; _LL1220: _temp1219=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1051)->f5; goto _LL1108;} else{ goto
_LL1109;} _LL1109: if(*(( int*) _temp1051) == Cyc_Absyn_Enum_e){ _LL1234:
_temp1233=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1051)->f1;
goto _LL1232; _LL1232: _temp1231=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1051)->f2; goto _LL1230; _LL1230: _temp1229=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1051)->f3; goto _LL1110;} else{ goto
_LL1111;} _LL1111: if(*(( int*) _temp1051) == Cyc_Absyn_Malloc_e){ _LL1238:
_temp1237=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1051)->f1; goto _LL1236; _LL1236: _temp1235=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1051)->f2; goto _LL1112;} else{ goto _LL1113;} _LL1113: if(*(( int*)
_temp1051) == Cyc_Absyn_UnresolvedMem_e){ _LL1242: _temp1241=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1051)->f1; goto _LL1240; _LL1240:
_temp1239=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1051)->f2; goto _LL1114;} else{ goto _LL1115;} _LL1115: if(*(( int*)
_temp1051) == Cyc_Absyn_StmtExp_e){ _LL1244: _temp1243=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1051)->f1; goto _LL1116;} else{ goto
_LL1117;} _LL1117: if(*(( int*) _temp1051) == Cyc_Absyn_Codegen_e){ _LL1246:
_temp1245=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1051)->f1; goto _LL1118;} else{ goto _LL1119;} _LL1119: if(*(( int*)
_temp1051) == Cyc_Absyn_Fill_e){ _LL1248: _temp1247=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp1051)->f1; goto _LL1120;} else{ goto
_LL1052;} _LL1054: s= Cyc_Absynpp_cnst2doc( _temp1121); goto _LL1052; _LL1056:
_temp1127= _temp1125; goto _LL1058; _LL1058: s= Cyc_Absynpp_qvar2doc( _temp1127);
goto _LL1052; _LL1060: s= Cyc_Absynpp_primapp2doc( myprec, _temp1131, _temp1129);
goto _LL1052; _LL1062: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1137), Cyc_PP_text(( struct _tagged_string)({ char* _temp1249=( char*)" ";
struct _tagged_string _temp1250; _temp1250.curr= _temp1249; _temp1250.base=
_temp1249; _temp1250.last_plus_one= _temp1249 + 2; _temp1250;})), _temp1135 == 0?
Cyc_PP_text(( struct _tagged_string)({ char* _temp1251=( char*)""; struct
_tagged_string _temp1252; _temp1252.curr= _temp1251; _temp1252.base= _temp1251;
_temp1252.last_plus_one= _temp1251 + 1; _temp1252;})): Cyc_Absynpp_prim2doc((
void*)({ struct Cyc_Core_Opt* _temp1253= _temp1135; if( _temp1253 == 0){ _throw(
Null_Exception);} _temp1253->v;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1254=( char*)"= "; struct _tagged_string _temp1255; _temp1255.curr=
_temp1254; _temp1255.base= _temp1254; _temp1255.last_plus_one= _temp1254 + 3;
_temp1255;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1133)); goto _LL1052;
_LL1064: { struct Cyc_PP_Doc* es= Cyc_Absynpp_exp2doc_prec( myprec, _temp1141);{
void* _temp1256= _temp1139; _LL1258: if( _temp1256 ==( void*) Cyc_Absyn_PreInc){
goto _LL1259;} else{ goto _LL1260;} _LL1260: if( _temp1256 ==( void*) Cyc_Absyn_PreDec){
goto _LL1261;} else{ goto _LL1262;} _LL1262: if( _temp1256 ==( void*) Cyc_Absyn_PostInc){
goto _LL1263;} else{ goto _LL1264;} _LL1264: if( _temp1256 ==( void*) Cyc_Absyn_PostDec){
goto _LL1265;} else{ goto _LL1257;} _LL1259: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1266=( char*)"++"; struct _tagged_string
_temp1267; _temp1267.curr= _temp1266; _temp1267.base= _temp1266; _temp1267.last_plus_one=
_temp1266 + 3; _temp1267;})), es); goto _LL1257; _LL1261: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1268=( char*)"--"; struct
_tagged_string _temp1269; _temp1269.curr= _temp1268; _temp1269.base= _temp1268;
_temp1269.last_plus_one= _temp1268 + 3; _temp1269;})), es); goto _LL1257;
_LL1263: s= Cyc_Absynpp_cat2( es, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1270=( char*)"++"; struct _tagged_string _temp1271; _temp1271.curr=
_temp1270; _temp1271.base= _temp1270; _temp1271.last_plus_one= _temp1270 + 3;
_temp1271;}))); goto _LL1257; _LL1265: s= Cyc_Absynpp_cat2( es, Cyc_PP_text((
struct _tagged_string)({ char* _temp1272=( char*)"--"; struct _tagged_string
_temp1273; _temp1273.curr= _temp1272; _temp1273.base= _temp1272; _temp1273.last_plus_one=
_temp1272 + 3; _temp1273;}))); goto _LL1257; _LL1257:;} goto _LL1052;} _LL1066:{
struct _tuple8 _temp1275=({ struct _tuple8 _temp1274; _temp1274.f1=( void*)
_temp1145->r; _temp1274.f2=( void*) _temp1143->r; _temp1274;}); _LL1277: goto
_LL1278; _LL1278: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1147), Cyc_PP_text(( struct _tagged_string)({ char* _temp1279=( char*)" ? ";
struct _tagged_string _temp1280; _temp1280.curr= _temp1279; _temp1280.base=
_temp1279; _temp1280.last_plus_one= _temp1279 + 4; _temp1280;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1145), Cyc_PP_text(( struct _tagged_string)({ char* _temp1281=( char*)" : ";
struct _tagged_string _temp1282; _temp1282.curr= _temp1281; _temp1282.base=
_temp1281; _temp1282.last_plus_one= _temp1281 + 4; _temp1282;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1143)); goto _LL1276; _LL1276:;} goto _LL1052; _LL1068: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1151), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1283=( char*)", "; struct _tagged_string _temp1284;
_temp1284.curr= _temp1283; _temp1284.base= _temp1283; _temp1284.last_plus_one=
_temp1283 + 3; _temp1284;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1149));
goto _LL1052; _LL1070: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1155), Cyc_PP_text(( struct _tagged_string)({ char* _temp1285=( char*)"(";
struct _tagged_string _temp1286; _temp1286.curr= _temp1285; _temp1286.base=
_temp1285; _temp1286.last_plus_one= _temp1285 + 2; _temp1286;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1153), Cyc_PP_text(( struct _tagged_string)({ char* _temp1287=( char*)")";
struct _tagged_string _temp1288; _temp1288.curr= _temp1287; _temp1288.base=
_temp1287; _temp1288.last_plus_one= _temp1287 + 2; _temp1288;}))); goto _LL1052;
_LL1072: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1159), Cyc_PP_text((
struct _tagged_string)({ char* _temp1289=( char*)"("; struct _tagged_string
_temp1290; _temp1290.curr= _temp1289; _temp1290.base= _temp1289; _temp1290.last_plus_one=
_temp1289 + 2; _temp1290;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1157), Cyc_PP_text((
struct _tagged_string)({ char* _temp1291=( char*)")"; struct _tagged_string
_temp1292; _temp1292.curr= _temp1291; _temp1292.base= _temp1291; _temp1292.last_plus_one=
_temp1291 + 2; _temp1292;}))); goto _LL1052; _LL1074: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1293=( char*)"throw "; struct _tagged_string
_temp1294; _temp1294.curr= _temp1293; _temp1294.base= _temp1293; _temp1294.last_plus_one=
_temp1293 + 7; _temp1294;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1161));
goto _LL1052; _LL1076: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1163); goto
_LL1052; _LL1078: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1167); goto _LL1052;
_LL1080: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1295=( char*)"("; struct _tagged_string _temp1296; _temp1296.curr=
_temp1295; _temp1296.base= _temp1295; _temp1296.last_plus_one= _temp1295 + 2;
_temp1296;})), Cyc_Absynpp_typ2doc( _temp1171), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1297=( char*)")"; struct _tagged_string _temp1298;
_temp1298.curr= _temp1297; _temp1298.base= _temp1297; _temp1298.last_plus_one=
_temp1297 + 2; _temp1298;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1169));
goto _LL1052; _LL1082: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({
char* _temp1299=( char*)"&"; struct _tagged_string _temp1300; _temp1300.curr=
_temp1299; _temp1300.base= _temp1299; _temp1300.last_plus_one= _temp1299 + 2;
_temp1300;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1173)); goto _LL1052;
_LL1084: if( _temp1177 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1301=( char*)"new "; struct _tagged_string
_temp1302; _temp1302.curr= _temp1301; _temp1302.base= _temp1301; _temp1302.last_plus_one=
_temp1301 + 5; _temp1302;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1175));}
else{ s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1303=( char*)"rnew("; struct _tagged_string _temp1304; _temp1304.curr=
_temp1303; _temp1304.base= _temp1303; _temp1304.last_plus_one= _temp1303 + 6;
_temp1304;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1305= _temp1177; if( _temp1305 == 0){ _throw( Null_Exception);} _temp1305;})),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1306=( char*)") "; struct
_tagged_string _temp1307; _temp1307.curr= _temp1306; _temp1307.base= _temp1306;
_temp1307.last_plus_one= _temp1306 + 3; _temp1307;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1175));} goto _LL1052; _LL1086: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1308=( char*)"sizeof("; struct
_tagged_string _temp1309; _temp1309.curr= _temp1308; _temp1309.base= _temp1308;
_temp1309.last_plus_one= _temp1308 + 8; _temp1309;})), Cyc_Absynpp_typ2doc(
_temp1179), Cyc_PP_text(( struct _tagged_string)({ char* _temp1310=( char*)")";
struct _tagged_string _temp1311; _temp1311.curr= _temp1310; _temp1311.base=
_temp1310; _temp1311.last_plus_one= _temp1310 + 2; _temp1311;}))); goto _LL1052;
_LL1088: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1312=( char*)"sizeof("; struct _tagged_string _temp1313; _temp1313.curr=
_temp1312; _temp1313.base= _temp1312; _temp1313.last_plus_one= _temp1312 + 8;
_temp1313;})), Cyc_Absynpp_exp2doc( _temp1181), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1314=( char*)")"; struct _tagged_string _temp1315;
_temp1315.curr= _temp1314; _temp1315.base= _temp1314; _temp1315.last_plus_one=
_temp1314 + 2; _temp1315;}))); goto _LL1052; _LL1090: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1316=( char*)"*"; struct _tagged_string
_temp1317; _temp1317.curr= _temp1316; _temp1317.base= _temp1316; _temp1317.last_plus_one=
_temp1316 + 2; _temp1317;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1183));
goto _LL1052; _LL1092: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1187), Cyc_PP_text(( struct _tagged_string)({ char* _temp1318=( char*)".";
struct _tagged_string _temp1319; _temp1319.curr= _temp1318; _temp1319.base=
_temp1318; _temp1319.last_plus_one= _temp1318 + 2; _temp1319;})), Cyc_Absynpp_textptr(
_temp1185)); goto _LL1052; _LL1094: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1191), Cyc_PP_text(( struct _tagged_string)({ char* _temp1320=(
char*)"->"; struct _tagged_string _temp1321; _temp1321.curr= _temp1320;
_temp1321.base= _temp1320; _temp1321.last_plus_one= _temp1320 + 3; _temp1321;})),
Cyc_Absynpp_textptr( _temp1189)); goto _LL1052; _LL1096: s= Cyc_Absynpp_cat4(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1195), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1322=( char*)"["; struct _tagged_string _temp1323;
_temp1323.curr= _temp1322; _temp1323.base= _temp1322; _temp1323.last_plus_one=
_temp1322 + 2; _temp1323;})), Cyc_Absynpp_exp2doc( _temp1193), Cyc_PP_text((
struct _tagged_string)({ char* _temp1324=( char*)"]"; struct _tagged_string
_temp1325; _temp1325.curr= _temp1324; _temp1325.base= _temp1324; _temp1325.last_plus_one=
_temp1324 + 2; _temp1325;}))); goto _LL1052; _LL1098: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1326=( char*)"$("; struct _tagged_string
_temp1327; _temp1327.curr= _temp1326; _temp1327.base= _temp1326; _temp1327.last_plus_one=
_temp1326 + 3; _temp1327;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1197), Cyc_PP_text((
struct _tagged_string)({ char* _temp1328=( char*)")"; struct _tagged_string
_temp1329; _temp1329.curr= _temp1328; _temp1329.base= _temp1328; _temp1329.last_plus_one=
_temp1328 + 2; _temp1329;}))); goto _LL1052; _LL1100: s= Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1330=( char*)"("; struct _tagged_string
_temp1331; _temp1331.curr= _temp1330; _temp1331.base= _temp1330; _temp1331.last_plus_one=
_temp1330 + 2; _temp1331;})), Cyc_Absynpp_typ2doc((* _temp1201).f3), Cyc_PP_text((
struct _tagged_string)({ char* _temp1332=( char*)")"; struct _tagged_string
_temp1333; _temp1333.curr= _temp1332; _temp1333.base= _temp1332; _temp1333.last_plus_one=
_temp1332 + 2; _temp1333;})), Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp1334=( char*)"{"; struct _tagged_string _temp1335; _temp1335.curr=
_temp1334; _temp1335.base= _temp1334; _temp1335.last_plus_one= _temp1334 + 2;
_temp1335;}),( struct _tagged_string)({ char* _temp1336=( char*)"}"; struct
_tagged_string _temp1337; _temp1337.curr= _temp1336; _temp1337.base= _temp1336;
_temp1337.last_plus_one= _temp1336 + 2; _temp1337;}),( struct _tagged_string)({
char* _temp1338=( char*)","; struct _tagged_string _temp1339; _temp1339.curr=
_temp1338; _temp1339.base= _temp1338; _temp1339.last_plus_one= _temp1338 + 2;
_temp1339;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1199))); goto _LL1052; _LL1102: s= Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1340=( char*)"{"; struct _tagged_string _temp1341;
_temp1341.curr= _temp1340; _temp1341.base= _temp1340; _temp1341.last_plus_one=
_temp1340 + 2; _temp1341;}),( struct _tagged_string)({ char* _temp1342=( char*)"}";
struct _tagged_string _temp1343; _temp1343.curr= _temp1342; _temp1343.base=
_temp1342; _temp1343.last_plus_one= _temp1342 + 2; _temp1343;}),( struct
_tagged_string)({ char* _temp1344=( char*)","; struct _tagged_string _temp1345;
_temp1345.curr= _temp1344; _temp1345.base= _temp1344; _temp1345.last_plus_one=
_temp1344 + 2; _temp1345;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1203)); goto _LL1052; _LL1104: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1346=( char*)"{for "; struct _tagged_string
_temp1347; _temp1347.curr= _temp1346; _temp1347.base= _temp1346; _temp1347.last_plus_one=
_temp1346 + 6; _temp1347;})), Cyc_PP_text(*(* _temp1209->name).f2), Cyc_PP_text((
struct _tagged_string)({ char* _temp1348=( char*)" < "; struct _tagged_string
_temp1349; _temp1349.curr= _temp1348; _temp1349.base= _temp1348; _temp1349.last_plus_one=
_temp1348 + 4; _temp1349;})), Cyc_Absynpp_exp2doc( _temp1207), Cyc_PP_text((
struct _tagged_string)({ char* _temp1350=( char*)" : "; struct _tagged_string
_temp1351; _temp1351.curr= _temp1350; _temp1351.base= _temp1350; _temp1351.last_plus_one=
_temp1350 + 4; _temp1351;})), Cyc_Absynpp_exp2doc( _temp1205), Cyc_PP_text((
struct _tagged_string)({ char* _temp1352=( char*)"}"; struct _tagged_string
_temp1353; _temp1353.curr= _temp1352; _temp1353.base= _temp1352; _temp1353.last_plus_one=
_temp1352 + 2; _temp1353;}))); goto _LL1052; _LL1106: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1217), Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1354=( char*)"{";
struct _tagged_string _temp1355; _temp1355.curr= _temp1354; _temp1355.base=
_temp1354; _temp1355.last_plus_one= _temp1354 + 2; _temp1355;}),( struct
_tagged_string)({ char* _temp1356=( char*)"}"; struct _tagged_string _temp1357;
_temp1357.curr= _temp1356; _temp1357.base= _temp1356; _temp1357.last_plus_one=
_temp1356 + 2; _temp1357;}),( struct _tagged_string)({ char* _temp1358=( char*)",";
struct _tagged_string _temp1359; _temp1359.curr= _temp1358; _temp1359.base=
_temp1358; _temp1359.last_plus_one= _temp1358 + 2; _temp1359;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1213))); goto _LL1052; _LL1108: if( _temp1223 == 0){ s=
Cyc_Absynpp_qvar2doc( _temp1219->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1219->name), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1360=(
char*)"("; struct _tagged_string _temp1361; _temp1361.curr= _temp1360; _temp1361.base=
_temp1360; _temp1361.last_plus_one= _temp1360 + 2; _temp1361;}),( struct
_tagged_string)({ char* _temp1362=( char*)")"; struct _tagged_string _temp1363;
_temp1363.curr= _temp1362; _temp1363.base= _temp1362; _temp1363.last_plus_one=
_temp1362 + 2; _temp1363;}),( struct _tagged_string)({ char* _temp1364=( char*)",";
struct _tagged_string _temp1365; _temp1365.curr= _temp1364; _temp1365.base=
_temp1364; _temp1365.last_plus_one= _temp1364 + 2; _temp1365;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp1223)));} goto _LL1052; _LL1110: s= Cyc_Absynpp_qvar2doc(
_temp1233); goto _LL1052; _LL1112: if( _temp1237 == 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1366=( char*)"malloc(sizeof("; struct
_tagged_string _temp1367; _temp1367.curr= _temp1366; _temp1367.base= _temp1366;
_temp1367.last_plus_one= _temp1366 + 15; _temp1367;})), Cyc_Absynpp_typ2doc(
_temp1235), Cyc_PP_text(( struct _tagged_string)({ char* _temp1368=( char*)"))";
struct _tagged_string _temp1369; _temp1369.curr= _temp1368; _temp1369.base=
_temp1368; _temp1369.last_plus_one= _temp1368 + 3; _temp1369;})));} else{ s= Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1370=( char*)"rmalloc(";
struct _tagged_string _temp1371; _temp1371.curr= _temp1370; _temp1371.base=
_temp1370; _temp1371.last_plus_one= _temp1370 + 9; _temp1371;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1372= _temp1237; if(
_temp1372 == 0){ _throw( Null_Exception);} _temp1372;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1373=( char*)","; struct _tagged_string _temp1374;
_temp1374.curr= _temp1373; _temp1374.base= _temp1373; _temp1374.last_plus_one=
_temp1373 + 2; _temp1374;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1375=( char*)"sizeof("; struct _tagged_string _temp1376; _temp1376.curr=
_temp1375; _temp1376.base= _temp1375; _temp1376.last_plus_one= _temp1375 + 8;
_temp1376;})), Cyc_Absynpp_typ2doc( _temp1235), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1377=( char*)"))"; struct _tagged_string _temp1378;
_temp1378.curr= _temp1377; _temp1378.base= _temp1377; _temp1378.last_plus_one=
_temp1377 + 3; _temp1378;})));} goto _LL1052; _LL1114: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1379=( char*)"{"; struct _tagged_string
_temp1380; _temp1380.curr= _temp1379; _temp1380.base= _temp1379; _temp1380.last_plus_one=
_temp1379 + 2; _temp1380;}),( struct _tagged_string)({ char* _temp1381=( char*)"}";
struct _tagged_string _temp1382; _temp1382.curr= _temp1381; _temp1382.base=
_temp1381; _temp1382.last_plus_one= _temp1381 + 2; _temp1382;}),( struct
_tagged_string)({ char* _temp1383=( char*)","; struct _tagged_string _temp1384;
_temp1384.curr= _temp1383; _temp1384.base= _temp1383; _temp1384.last_plus_one=
_temp1383 + 2; _temp1384;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1239)); goto _LL1052; _LL1116: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1385=( char*)"({ "; struct _tagged_string _temp1386;
_temp1386.curr= _temp1385; _temp1386.base= _temp1385; _temp1386.last_plus_one=
_temp1385 + 4; _temp1386;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1243)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1387=( char*)" })"; struct
_tagged_string _temp1388; _temp1388.curr= _temp1387; _temp1388.base= _temp1387;
_temp1388.last_plus_one= _temp1387 + 4; _temp1388;}))); goto _LL1052; _LL1118: s=
Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1389=( char*)"codegen(";
struct _tagged_string _temp1390; _temp1390.curr= _temp1389; _temp1390.base=
_temp1389; _temp1390.last_plus_one= _temp1389 + 9; _temp1390;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1391=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1391->r=( void*)(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp1392=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1392[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1393; _temp1393.tag= Cyc_Absyn_Fn_d; _temp1393.f1= _temp1245; _temp1393;});
_temp1392;})); _temp1391->loc= e->loc; _temp1391;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1394=( char*)")"; struct _tagged_string _temp1395;
_temp1395.curr= _temp1394; _temp1395.base= _temp1394; _temp1395.last_plus_one=
_temp1394 + 2; _temp1395;}))); goto _LL1052; _LL1120: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1396=( char*)"fill("; struct _tagged_string
_temp1397; _temp1397.curr= _temp1396; _temp1397.base= _temp1396; _temp1397.last_plus_one=
_temp1396 + 6; _temp1397;})), Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1247)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1398=( char*)")"; struct
_tagged_string _temp1399; _temp1399.curr= _temp1398; _temp1399.base= _temp1398;
_temp1399.last_plus_one= _temp1398 + 2; _temp1399;}))); goto _LL1052; _LL1052:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp1400=( char*)"("; struct _tagged_string _temp1401; _temp1401.curr=
_temp1400; _temp1401.base= _temp1400; _temp1401.last_plus_one= _temp1400 + 2;
_temp1401;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp1402=( char*)")";
struct _tagged_string _temp1403; _temp1403.curr= _temp1402; _temp1403.base=
_temp1402; _temp1403.last_plus_one= _temp1402 + 2; _temp1403;})));} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1404= d;
struct Cyc_Absyn_Exp* _temp1410; struct _tagged_string* _temp1412; _LL1406: if(*((
int*) _temp1404) == Cyc_Absyn_ArrayElement){ _LL1411: _temp1410=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1404)->f1; goto _LL1407;} else{ goto
_LL1408;} _LL1408: if(*(( int*) _temp1404) == Cyc_Absyn_FieldName){ _LL1413:
_temp1412=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1404)->f1; goto _LL1409;} else{ goto _LL1405;} _LL1407: return Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1414=( char*)".["; struct
_tagged_string _temp1415; _temp1415.curr= _temp1414; _temp1415.base= _temp1414;
_temp1415.last_plus_one= _temp1414 + 3; _temp1415;})), Cyc_Absynpp_exp2doc(
_temp1410), Cyc_PP_text(( struct _tagged_string)({ char* _temp1416=( char*)"]";
struct _tagged_string _temp1417; _temp1417.curr= _temp1416; _temp1417.base=
_temp1416; _temp1417.last_plus_one= _temp1416 + 2; _temp1417;}))); _LL1409:
return Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp1418=(
char*)"."; struct _tagged_string _temp1419; _temp1419.curr= _temp1418; _temp1419.base=
_temp1418; _temp1419.last_plus_one= _temp1418 + 2; _temp1419;})), Cyc_Absynpp_textptr(
_temp1412)); _LL1405:;} struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6*
de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((* de).f2);} else{ return
Cyc_Absynpp_cat2( Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1420=(
char*)""; struct _tagged_string _temp1421; _temp1421.curr= _temp1420; _temp1421.base=
_temp1420; _temp1421.last_plus_one= _temp1420 + 1; _temp1421;}),( struct
_tagged_string)({ char* _temp1422=( char*)"="; struct _tagged_string _temp1423;
_temp1423.curr= _temp1422; _temp1423.base= _temp1422; _temp1423.last_plus_one=
_temp1422 + 2; _temp1423;}),( struct _tagged_string)({ char* _temp1424=( char*)"=";
struct _tagged_string _temp1425; _temp1425.curr= _temp1424; _temp1425.base=
_temp1424; _temp1425.last_plus_one= _temp1424 + 2; _temp1425;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)), Cyc_Absynpp_exp2doc((* de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1426=( char*)""; struct _tagged_string _temp1427;
_temp1427.curr= _temp1426; _temp1427.base= _temp1426; _temp1427.last_plus_one=
_temp1426 + 1; _temp1427;}),( struct _tagged_string)({ char* _temp1428=( char*)"";
struct _tagged_string _temp1429; _temp1429.curr= _temp1428; _temp1429.base=
_temp1428; _temp1429.last_plus_one= _temp1428 + 1; _temp1429;}),( struct
_tagged_string)({ char* _temp1430=( char*)","; struct _tagged_string _temp1431;
_temp1431.curr= _temp1430; _temp1431.base= _temp1430; _temp1431.last_plus_one=
_temp1430 + 2; _temp1431;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1432= c; char _temp1450; void* _temp1452; short _temp1454;
void* _temp1456; int _temp1458; void* _temp1460; int _temp1462; void* _temp1464;
long long _temp1466; void* _temp1468; struct _tagged_string _temp1470; struct
_tagged_string _temp1472; _LL1434: if(( unsigned int) _temp1432 > 1u?*(( int*)
_temp1432) == Cyc_Absyn_Char_c: 0){ _LL1453: _temp1452=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp1432)->f1; goto _LL1451; _LL1451: _temp1450=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp1432)->f2; goto _LL1435;} else{ goto _LL1436;} _LL1436: if(( unsigned int)
_temp1432 > 1u?*(( int*) _temp1432) == Cyc_Absyn_Short_c: 0){ _LL1457: _temp1456=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1432)->f1; goto _LL1455; _LL1455:
_temp1454=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp1432)->f2; goto
_LL1437;} else{ goto _LL1438;} _LL1438: if(( unsigned int) _temp1432 > 1u?*((
int*) _temp1432) == Cyc_Absyn_Int_c: 0){ _LL1461: _temp1460=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1432)->f1; if( _temp1460 ==( void*) Cyc_Absyn_Signed){
goto _LL1459;} else{ goto _LL1440;} _LL1459: _temp1458=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp1432)->f2; goto _LL1439;} else{ goto _LL1440;} _LL1440: if(( unsigned int)
_temp1432 > 1u?*(( int*) _temp1432) == Cyc_Absyn_Int_c: 0){ _LL1465: _temp1464=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1432)->f1; if( _temp1464 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL1463;} else{ goto _LL1442;} _LL1463: _temp1462=(
int)(( struct Cyc_Absyn_Int_c_struct*) _temp1432)->f2; goto _LL1441;} else{ goto
_LL1442;} _LL1442: if(( unsigned int) _temp1432 > 1u?*(( int*) _temp1432) == Cyc_Absyn_LongLong_c:
0){ _LL1469: _temp1468=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1432)->f1;
goto _LL1467; _LL1467: _temp1466=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1432)->f2; goto _LL1443;} else{ goto _LL1444;} _LL1444: if(( unsigned int)
_temp1432 > 1u?*(( int*) _temp1432) == Cyc_Absyn_Float_c: 0){ _LL1471: _temp1470=(
struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*) _temp1432)->f1; goto
_LL1445;} else{ goto _LL1446;} _LL1446: if( _temp1432 ==( void*) Cyc_Absyn_Null_c){
goto _LL1447;} else{ goto _LL1448;} _LL1448: if(( unsigned int) _temp1432 > 1u?*((
int*) _temp1432) == Cyc_Absyn_String_c: 0){ _LL1473: _temp1472=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp1432)->f1; goto
_LL1449;} else{ goto _LL1433;} _LL1435: return Cyc_PP_text(({ struct
_tagged_string _temp1474= Cyc_Absynpp_char_escape( _temp1450); xprintf("'%.*s'",
_temp1474.last_plus_one - _temp1474.curr, _temp1474.curr);})); _LL1437: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1454)); _LL1439: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1458)); _LL1441: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1462)); _LL1443: return Cyc_PP_text(( struct _tagged_string)({
char* _temp1475=( char*)"<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp1476; _temp1476.curr= _temp1475; _temp1476.base= _temp1475; _temp1476.last_plus_one=
_temp1475 + 27; _temp1476;})); _LL1445: return Cyc_PP_text( _temp1470); _LL1447:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp1477=( char*)"null";
struct _tagged_string _temp1478; _temp1478.curr= _temp1477; _temp1478.base=
_temp1477; _temp1478.last_plus_one= _temp1477 + 5; _temp1478;})); _LL1449:
return Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1479=(
char*)"\""; struct _tagged_string _temp1480; _temp1480.curr= _temp1479;
_temp1480.base= _temp1479; _temp1480.last_plus_one= _temp1479 + 2; _temp1480;})),
Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1472)), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1481=( char*)"\""; struct _tagged_string _temp1482;
_temp1482.curr= _temp1481; _temp1482.base= _temp1481; _temp1482.last_plus_one=
_temp1481 + 2; _temp1482;}))); _LL1433:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:({ struct Cyc_List_List*
_temp1483= es; if( _temp1483 == 0){ _throw( Null_Exception);} _temp1483->tl;})
!= 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp1484=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1484[ 0]=({ struct Cyc_Core_Failure_struct _temp1485; _temp1485.tag= Cyc_Core_Failure;
_temp1485.f1=({ struct _tagged_string _temp1486= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1486.last_plus_one
- _temp1486.curr, _temp1486.curr);}); _temp1485;}); _temp1484;}));}{ struct Cyc_PP_Doc*
d= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1489= es; if( _temp1489 == 0){ _throw( Null_Exception);} _temp1489->hd;}));
return Cyc_Absynpp_cat2( d, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1487=( char*)".size"; struct _tagged_string _temp1488; _temp1488.curr=
_temp1487; _temp1488.base= _temp1487; _temp1488.last_plus_one= _temp1487 + 6;
_temp1488;})));}} else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct Cyc_List_List*
ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1490=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1490[ 0]=({ struct Cyc_Core_Failure_struct _temp1491; _temp1491.tag= Cyc_Core_Failure;
_temp1491.f1=({ struct _tagged_string _temp1492= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with no args", _temp1492.last_plus_one -
_temp1492.curr, _temp1492.curr);}); _temp1491;}); _temp1490;}));} else{ if(({
struct Cyc_List_List* _temp1493= ds; if( _temp1493 == 0){ _throw( Null_Exception);}
_temp1493->tl;}) == 0){ return Cyc_Absynpp_cat3( ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1494=( char*)" "; struct _tagged_string _temp1495;
_temp1495.curr= _temp1494; _temp1495.base= _temp1494; _temp1495.last_plus_one=
_temp1494 + 2; _temp1495;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1496= ds; if( _temp1496 == 0){ _throw( Null_Exception);} _temp1496->hd;}));}
else{ if(({ struct Cyc_List_List* _temp1498=({ struct Cyc_List_List* _temp1497=
ds; if( _temp1497 == 0){ _throw( Null_Exception);} _temp1497->tl;}); if(
_temp1498 == 0){ _throw( Null_Exception);} _temp1498->tl;}) != 0){( void) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp1499=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1499[ 0]=({ struct Cyc_Core_Failure_struct
_temp1500; _temp1500.tag= Cyc_Core_Failure; _temp1500.f1=({ struct
_tagged_string _temp1501= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_temp1501.last_plus_one - _temp1501.curr, _temp1501.curr);}); _temp1500;});
_temp1499;}));} else{ return Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1502= ds; if( _temp1502 == 0){ _throw( Null_Exception);} _temp1502->hd;}),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1503=( char*)" "; struct
_tagged_string _temp1504; _temp1504.curr= _temp1503; _temp1504.base= _temp1503;
_temp1504.last_plus_one= _temp1503 + 2; _temp1504;})), ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1505=( char*)" "; struct _tagged_string _temp1506;
_temp1506.curr= _temp1505; _temp1506.base= _temp1505; _temp1506.last_plus_one=
_temp1505 + 2; _temp1506;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1508=({ struct Cyc_List_List* _temp1507= ds; if( _temp1507 == 0){ _throw(
Null_Exception);} _temp1507->tl;}); if( _temp1508 == 0){ _throw( Null_Exception);}
_temp1508->hd;}));}}}} else{ return Cyc_Absynpp_cat4( ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1509=( char*)"("; struct _tagged_string _temp1510;
_temp1510.curr= _temp1509; _temp1510.base= _temp1509; _temp1510.last_plus_one=
_temp1509 + 2; _temp1510;})), Cyc_Absynpp_exps2doc_prec( 20, es), Cyc_PP_text((
struct _tagged_string)({ char* _temp1511=( char*)")"; struct _tagged_string
_temp1512; _temp1512.curr= _temp1511; _temp1512.base= _temp1511; _temp1512.last_plus_one=
_temp1511 + 2; _temp1512;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1513= p; _LL1515: if( _temp1513 ==( void*) Cyc_Absyn_Plus){
goto _LL1516;} else{ goto _LL1517;} _LL1517: if( _temp1513 ==( void*) Cyc_Absyn_Times){
goto _LL1518;} else{ goto _LL1519;} _LL1519: if( _temp1513 ==( void*) Cyc_Absyn_Minus){
goto _LL1520;} else{ goto _LL1521;} _LL1521: if( _temp1513 ==( void*) Cyc_Absyn_Div){
goto _LL1522;} else{ goto _LL1523;} _LL1523: if( _temp1513 ==( void*) Cyc_Absyn_Mod){
goto _LL1524;} else{ goto _LL1525;} _LL1525: if( _temp1513 ==( void*) Cyc_Absyn_Eq){
goto _LL1526;} else{ goto _LL1527;} _LL1527: if( _temp1513 ==( void*) Cyc_Absyn_Neq){
goto _LL1528;} else{ goto _LL1529;} _LL1529: if( _temp1513 ==( void*) Cyc_Absyn_Gt){
goto _LL1530;} else{ goto _LL1531;} _LL1531: if( _temp1513 ==( void*) Cyc_Absyn_Lt){
goto _LL1532;} else{ goto _LL1533;} _LL1533: if( _temp1513 ==( void*) Cyc_Absyn_Gte){
goto _LL1534;} else{ goto _LL1535;} _LL1535: if( _temp1513 ==( void*) Cyc_Absyn_Lte){
goto _LL1536;} else{ goto _LL1537;} _LL1537: if( _temp1513 ==( void*) Cyc_Absyn_Not){
goto _LL1538;} else{ goto _LL1539;} _LL1539: if( _temp1513 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1540;} else{ goto _LL1541;} _LL1541: if( _temp1513 ==( void*) Cyc_Absyn_Bitand){
goto _LL1542;} else{ goto _LL1543;} _LL1543: if( _temp1513 ==( void*) Cyc_Absyn_Bitor){
goto _LL1544;} else{ goto _LL1545;} _LL1545: if( _temp1513 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1546;} else{ goto _LL1547;} _LL1547: if( _temp1513 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1548;} else{ goto _LL1549;} _LL1549: if( _temp1513 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1550;} else{ goto _LL1551;} _LL1551: if( _temp1513 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1552;} else{ goto _LL1553;} _LL1553: if( _temp1513 ==( void*) Cyc_Absyn_Size){
goto _LL1554;} else{ goto _LL1555;} _LL1555: if( _temp1513 ==( void*) Cyc_Absyn_Printf){
goto _LL1556;} else{ goto _LL1557;} _LL1557: if( _temp1513 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1558;} else{ goto _LL1559;} _LL1559: if( _temp1513 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1560;} else{ goto _LL1561;} _LL1561: if( _temp1513 ==( void*) Cyc_Absyn_Scanf){
goto _LL1562;} else{ goto _LL1563;} _LL1563: if( _temp1513 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1564;} else{ goto _LL1565;} _LL1565: if( _temp1513 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1566;} else{ goto _LL1514;} _LL1516: return( struct _tagged_string)({
char* _temp1567=( char*)"+"; struct _tagged_string _temp1568; _temp1568.curr=
_temp1567; _temp1568.base= _temp1567; _temp1568.last_plus_one= _temp1567 + 2;
_temp1568;}); _LL1518: return( struct _tagged_string)({ char* _temp1569=( char*)"*";
struct _tagged_string _temp1570; _temp1570.curr= _temp1569; _temp1570.base=
_temp1569; _temp1570.last_plus_one= _temp1569 + 2; _temp1570;}); _LL1520: return(
struct _tagged_string)({ char* _temp1571=( char*)"-"; struct _tagged_string
_temp1572; _temp1572.curr= _temp1571; _temp1572.base= _temp1571; _temp1572.last_plus_one=
_temp1571 + 2; _temp1572;}); _LL1522: return( struct _tagged_string)({ char*
_temp1573=( char*)"/"; struct _tagged_string _temp1574; _temp1574.curr=
_temp1573; _temp1574.base= _temp1573; _temp1574.last_plus_one= _temp1573 + 2;
_temp1574;}); _LL1524: return( struct _tagged_string)({ char* _temp1575=( char*)"%";
struct _tagged_string _temp1576; _temp1576.curr= _temp1575; _temp1576.base=
_temp1575; _temp1576.last_plus_one= _temp1575 + 2; _temp1576;}); _LL1526: return(
struct _tagged_string)({ char* _temp1577=( char*)"=="; struct _tagged_string
_temp1578; _temp1578.curr= _temp1577; _temp1578.base= _temp1577; _temp1578.last_plus_one=
_temp1577 + 3; _temp1578;}); _LL1528: return( struct _tagged_string)({ char*
_temp1579=( char*)"!="; struct _tagged_string _temp1580; _temp1580.curr=
_temp1579; _temp1580.base= _temp1579; _temp1580.last_plus_one= _temp1579 + 3;
_temp1580;}); _LL1530: return( struct _tagged_string)({ char* _temp1581=( char*)">";
struct _tagged_string _temp1582; _temp1582.curr= _temp1581; _temp1582.base=
_temp1581; _temp1582.last_plus_one= _temp1581 + 2; _temp1582;}); _LL1532: return(
struct _tagged_string)({ char* _temp1583=( char*)"<"; struct _tagged_string
_temp1584; _temp1584.curr= _temp1583; _temp1584.base= _temp1583; _temp1584.last_plus_one=
_temp1583 + 2; _temp1584;}); _LL1534: return( struct _tagged_string)({ char*
_temp1585=( char*)">="; struct _tagged_string _temp1586; _temp1586.curr=
_temp1585; _temp1586.base= _temp1585; _temp1586.last_plus_one= _temp1585 + 3;
_temp1586;}); _LL1536: return( struct _tagged_string)({ char* _temp1587=( char*)"<=";
struct _tagged_string _temp1588; _temp1588.curr= _temp1587; _temp1588.base=
_temp1587; _temp1588.last_plus_one= _temp1587 + 3; _temp1588;}); _LL1538: return(
struct _tagged_string)({ char* _temp1589=( char*)"!"; struct _tagged_string
_temp1590; _temp1590.curr= _temp1589; _temp1590.base= _temp1589; _temp1590.last_plus_one=
_temp1589 + 2; _temp1590;}); _LL1540: return( struct _tagged_string)({ char*
_temp1591=( char*)"~"; struct _tagged_string _temp1592; _temp1592.curr=
_temp1591; _temp1592.base= _temp1591; _temp1592.last_plus_one= _temp1591 + 2;
_temp1592;}); _LL1542: return( struct _tagged_string)({ char* _temp1593=( char*)"&";
struct _tagged_string _temp1594; _temp1594.curr= _temp1593; _temp1594.base=
_temp1593; _temp1594.last_plus_one= _temp1593 + 2; _temp1594;}); _LL1544: return(
struct _tagged_string)({ char* _temp1595=( char*)"|"; struct _tagged_string
_temp1596; _temp1596.curr= _temp1595; _temp1596.base= _temp1595; _temp1596.last_plus_one=
_temp1595 + 2; _temp1596;}); _LL1546: return( struct _tagged_string)({ char*
_temp1597=( char*)"^"; struct _tagged_string _temp1598; _temp1598.curr=
_temp1597; _temp1598.base= _temp1597; _temp1598.last_plus_one= _temp1597 + 2;
_temp1598;}); _LL1548: return( struct _tagged_string)({ char* _temp1599=( char*)"<<";
struct _tagged_string _temp1600; _temp1600.curr= _temp1599; _temp1600.base=
_temp1599; _temp1600.last_plus_one= _temp1599 + 3; _temp1600;}); _LL1550: return(
struct _tagged_string)({ char* _temp1601=( char*)">>"; struct _tagged_string
_temp1602; _temp1602.curr= _temp1601; _temp1602.base= _temp1601; _temp1602.last_plus_one=
_temp1601 + 3; _temp1602;}); _LL1552: return( struct _tagged_string)({ char*
_temp1603=( char*)">>>"; struct _tagged_string _temp1604; _temp1604.curr=
_temp1603; _temp1604.base= _temp1603; _temp1604.last_plus_one= _temp1603 + 4;
_temp1604;}); _LL1554: return( struct _tagged_string)({ char* _temp1605=( char*)"size";
struct _tagged_string _temp1606; _temp1606.curr= _temp1605; _temp1606.base=
_temp1605; _temp1606.last_plus_one= _temp1605 + 5; _temp1606;}); _LL1556: return(
struct _tagged_string)({ char* _temp1607=( char*)"printf"; struct _tagged_string
_temp1608; _temp1608.curr= _temp1607; _temp1608.base= _temp1607; _temp1608.last_plus_one=
_temp1607 + 7; _temp1608;}); _LL1558: return( struct _tagged_string)({ char*
_temp1609=( char*)"fprintf"; struct _tagged_string _temp1610; _temp1610.curr=
_temp1609; _temp1610.base= _temp1609; _temp1610.last_plus_one= _temp1609 + 8;
_temp1610;}); _LL1560: return( struct _tagged_string)({ char* _temp1611=( char*)"xprintf";
struct _tagged_string _temp1612; _temp1612.curr= _temp1611; _temp1612.base=
_temp1611; _temp1612.last_plus_one= _temp1611 + 8; _temp1612;}); _LL1562: return(
struct _tagged_string)({ char* _temp1613=( char*)"scanf"; struct _tagged_string
_temp1614; _temp1614.curr= _temp1613; _temp1614.base= _temp1613; _temp1614.last_plus_one=
_temp1613 + 6; _temp1614;}); _LL1564: return( struct _tagged_string)({ char*
_temp1615=( char*)"fscanf"; struct _tagged_string _temp1616; _temp1616.curr=
_temp1615; _temp1616.base= _temp1615; _temp1616.last_plus_one= _temp1615 + 7;
_temp1616;}); _LL1566: return( struct _tagged_string)({ char* _temp1617=( char*)"sscanf";
struct _tagged_string _temp1618; _temp1618.curr= _temp1617; _temp1618.base=
_temp1617; _temp1618.last_plus_one= _temp1617 + 7; _temp1618;}); _LL1514:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 ==
0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp1619=(* t).f1; if( _temp1619
== 0){ _throw( Null_Exception);} _temp1619->v;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1620=( char*)" = "; struct _tagged_string _temp1621;
_temp1621.curr= _temp1620; _temp1621.base= _temp1620; _temp1621.last_plus_one=
_temp1620 + 4; _temp1621;})), Cyc_Absynpp_exp2doc((* t).f2));}} struct Cyc_PP_Doc*
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
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1622=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1628; struct Cyc_Absyn_Decl* _temp1630;
_LL1624: if(( unsigned int) _temp1622 > 1u?*(( int*) _temp1622) == Cyc_Absyn_Decl_s:
0){ _LL1631: _temp1630=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1622)->f1; goto _LL1629; _LL1629: _temp1628=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1622)->f2; goto _LL1625;} else{ goto
_LL1626;} _LL1626: goto _LL1627; _LL1625: return 1; _LL1627: return 0; _LL1623:;}
struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc*
s;{ void* _temp1632=( void*) st->r; struct Cyc_Absyn_Exp* _temp1674; struct Cyc_Absyn_Stmt*
_temp1676; struct Cyc_Absyn_Stmt* _temp1678; struct Cyc_Absyn_Exp* _temp1680;
struct Cyc_Absyn_Stmt* _temp1682; struct Cyc_Absyn_Stmt* _temp1684; struct Cyc_Absyn_Exp*
_temp1686; struct Cyc_Absyn_Stmt* _temp1688; struct _tuple2 _temp1690; struct
Cyc_Absyn_Stmt* _temp1692; struct Cyc_Absyn_Exp* _temp1694; struct Cyc_Absyn_Stmt*
_temp1696; struct Cyc_Absyn_Stmt* _temp1698; struct Cyc_Absyn_Stmt* _temp1700;
struct _tagged_string* _temp1702; struct Cyc_Absyn_Stmt* _temp1704; struct
_tuple2 _temp1706; struct Cyc_Absyn_Stmt* _temp1708; struct Cyc_Absyn_Exp*
_temp1710; struct _tuple2 _temp1712; struct Cyc_Absyn_Stmt* _temp1714; struct
Cyc_Absyn_Exp* _temp1716; struct Cyc_Absyn_Exp* _temp1718; struct Cyc_List_List*
_temp1720; struct Cyc_Absyn_Exp* _temp1722; struct Cyc_Absyn_Switch_clause**
_temp1724; struct Cyc_List_List* _temp1726; struct Cyc_Absyn_Switch_clause**
_temp1728; struct Cyc_List_List* _temp1730; struct Cyc_Absyn_Stmt* _temp1732;
struct Cyc_Absyn_Decl* _temp1734; struct Cyc_Absyn_Stmt* _temp1736; struct Cyc_Absyn_Stmt*
_temp1738; struct Cyc_Absyn_Stmt* _temp1740; struct _tagged_string* _temp1742;
struct _tuple2 _temp1744; struct Cyc_Absyn_Stmt* _temp1746; struct Cyc_Absyn_Exp*
_temp1748; struct Cyc_Absyn_Stmt* _temp1750; struct Cyc_List_List* _temp1752;
struct Cyc_Absyn_Stmt* _temp1754; struct Cyc_Absyn_Stmt* _temp1756; struct Cyc_Absyn_Vardecl*
_temp1758; struct Cyc_Absyn_Tvar* _temp1760; _LL1634: if( _temp1632 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1635;} else{ goto _LL1636;} _LL1636: if((
unsigned int) _temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Exp_s: 0){
_LL1675: _temp1674=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1632)->f1; goto _LL1637;} else{ goto _LL1638;} _LL1638: if(( unsigned int)
_temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Seq_s: 0){ _LL1679: _temp1678=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1632)->f1; goto
_LL1677; _LL1677: _temp1676=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1632)->f2; goto _LL1639;} else{ goto _LL1640;} _LL1640: if(( unsigned int)
_temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Return_s: 0){ _LL1681:
_temp1680=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1632)->f1; goto _LL1641;} else{ goto _LL1642;} _LL1642: if(( unsigned int)
_temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_IfThenElse_s: 0){ _LL1687:
_temp1686=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1632)->f1; goto _LL1685; _LL1685: _temp1684=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1632)->f2; goto _LL1683; _LL1683:
_temp1682=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1632)->f3; goto _LL1643;} else{ goto _LL1644;} _LL1644: if(( unsigned int)
_temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_While_s: 0){ _LL1691: _temp1690=(
struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp1632)->f1; _LL1695:
_temp1694= _temp1690.f1; goto _LL1693; _LL1693: _temp1692= _temp1690.f2; goto
_LL1689; _LL1689: _temp1688=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1632)->f2; goto _LL1645;} else{ goto _LL1646;} _LL1646: if(( unsigned int)
_temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Break_s: 0){ _LL1697: _temp1696=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp1632)->f1; goto
_LL1647;} else{ goto _LL1648;} _LL1648: if(( unsigned int) _temp1632 > 1u?*((
int*) _temp1632) == Cyc_Absyn_Continue_s: 0){ _LL1699: _temp1698=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1632)->f1; goto _LL1649;} else{ goto
_LL1650;} _LL1650: if(( unsigned int) _temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Goto_s:
0){ _LL1703: _temp1702=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1632)->f1; goto _LL1701; _LL1701: _temp1700=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp1632)->f2; goto _LL1651;} else{ goto
_LL1652;} _LL1652: if(( unsigned int) _temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_For_s:
0){ _LL1719: _temp1718=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1632)->f1; goto _LL1713; _LL1713: _temp1712=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp1632)->f2; _LL1717: _temp1716= _temp1712.f1; goto _LL1715; _LL1715:
_temp1714= _temp1712.f2; goto _LL1707; _LL1707: _temp1706=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp1632)->f3; _LL1711: _temp1710= _temp1706.f1;
goto _LL1709; _LL1709: _temp1708= _temp1706.f2; goto _LL1705; _LL1705: _temp1704=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1632)->f4; goto
_LL1653;} else{ goto _LL1654;} _LL1654: if(( unsigned int) _temp1632 > 1u?*((
int*) _temp1632) == Cyc_Absyn_Switch_s: 0){ _LL1723: _temp1722=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1632)->f1; goto _LL1721; _LL1721:
_temp1720=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1632)->f2; goto _LL1655;} else{ goto _LL1656;} _LL1656: if(( unsigned int)
_temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Fallthru_s: 0){ _LL1727:
_temp1726=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1632)->f1; if( _temp1726 == 0){ goto _LL1725;} else{ goto _LL1658;} _LL1725:
_temp1724=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1632)->f2; goto _LL1657;} else{ goto _LL1658;} _LL1658: if(( unsigned int)
_temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Fallthru_s: 0){ _LL1731:
_temp1730=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1632)->f1; goto _LL1729; _LL1729: _temp1728=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1632)->f2; goto _LL1659;} else{ goto
_LL1660;} _LL1660: if(( unsigned int) _temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Decl_s:
0){ _LL1735: _temp1734=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1632)->f1; goto _LL1733; _LL1733: _temp1732=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1632)->f2; goto _LL1661;} else{ goto
_LL1662;} _LL1662: if(( unsigned int) _temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Cut_s:
0){ _LL1737: _temp1736=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp1632)->f1; goto _LL1663;} else{ goto _LL1664;} _LL1664: if(( unsigned int)
_temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Splice_s: 0){ _LL1739:
_temp1738=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp1632)->f1; goto _LL1665;} else{ goto _LL1666;} _LL1666: if(( unsigned int)
_temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Label_s: 0){ _LL1743: _temp1742=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1632)->f1; goto
_LL1741; _LL1741: _temp1740=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1632)->f2; goto _LL1667;} else{ goto _LL1668;} _LL1668: if(( unsigned int)
_temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Do_s: 0){ _LL1751: _temp1750=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp1632)->f1; goto
_LL1745; _LL1745: _temp1744=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp1632)->f2; _LL1749: _temp1748= _temp1744.f1; goto _LL1747; _LL1747:
_temp1746= _temp1744.f2; goto _LL1669;} else{ goto _LL1670;} _LL1670: if((
unsigned int) _temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_TryCatch_s: 0){
_LL1755: _temp1754=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1632)->f1; goto _LL1753; _LL1753: _temp1752=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1632)->f2; goto _LL1671;} else{ goto
_LL1672;} _LL1672: if(( unsigned int) _temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Region_s:
0){ _LL1761: _temp1760=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1632)->f1; goto _LL1759; _LL1759: _temp1758=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1632)->f2; goto _LL1757; _LL1757:
_temp1756=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1632)->f3; goto _LL1673;} else{ goto _LL1633;} _LL1635: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp1762=( char*)";"; struct _tagged_string
_temp1763; _temp1763.curr= _temp1762; _temp1763.base= _temp1762; _temp1763.last_plus_one=
_temp1762 + 2; _temp1763;})); goto _LL1633; _LL1637: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc(
_temp1674), Cyc_PP_text(( struct _tagged_string)({ char* _temp1764=( char*)";";
struct _tagged_string _temp1765; _temp1765.curr= _temp1764; _temp1765.base=
_temp1764; _temp1765.last_plus_one= _temp1764 + 2; _temp1765;}))); goto _LL1633;
_LL1639: if( Cyc_Absynpp_decls_first){ if( Cyc_Absynpp_is_declaration( _temp1678)){
s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({ char* _temp1766=(
char*)"{ "; struct _tagged_string _temp1767; _temp1767.curr= _temp1766;
_temp1767.base= _temp1766; _temp1767.last_plus_one= _temp1766 + 3; _temp1767;})),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1678)), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1768=( char*)"}"; struct _tagged_string
_temp1769; _temp1769.curr= _temp1768; _temp1769.base= _temp1768; _temp1769.last_plus_one=
_temp1768 + 2; _temp1769;})), Cyc_PP_line_doc(), Cyc_Absynpp_is_declaration(
_temp1676)? Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1770=( char*)"{ "; struct _tagged_string _temp1771; _temp1771.curr=
_temp1770; _temp1771.base= _temp1770; _temp1771.last_plus_one= _temp1770 + 3;
_temp1771;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1676)), Cyc_PP_text((
struct _tagged_string)({ char* _temp1772=( char*)"}"; struct _tagged_string
_temp1773; _temp1773.curr= _temp1772; _temp1773.base= _temp1772; _temp1773.last_plus_one=
_temp1772 + 2; _temp1773;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1676));} else{ if( Cyc_Absynpp_is_declaration( _temp1676)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1678), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1774=( char*)"{ "; struct _tagged_string _temp1775;
_temp1775.curr= _temp1774; _temp1775.base= _temp1774; _temp1775.last_plus_one=
_temp1774 + 3; _temp1775;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1676)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1776=( char*)"}"; struct
_tagged_string _temp1777; _temp1777.curr= _temp1776; _temp1777.base= _temp1776;
_temp1777.last_plus_one= _temp1776 + 2; _temp1777;})), Cyc_PP_line_doc());}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,(
struct _tagged_string)({ char* _temp1778=( char*)""; struct _tagged_string
_temp1779; _temp1779.curr= _temp1778; _temp1779.base= _temp1778; _temp1779.last_plus_one=
_temp1778 + 1; _temp1779;}),({ struct Cyc_List_List* _temp1780=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1780->hd=( void*) _temp1678;
_temp1780->tl=({ struct Cyc_List_List* _temp1781=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1781->hd=( void*) _temp1676;
_temp1781->tl= 0; _temp1781;}); _temp1780;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,( struct
_tagged_string)({ char* _temp1782=( char*)""; struct _tagged_string _temp1783;
_temp1783.curr= _temp1782; _temp1783.base= _temp1782; _temp1783.last_plus_one=
_temp1782 + 1; _temp1783;}),({ struct Cyc_List_List* _temp1784=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1784->hd=( void*) _temp1678;
_temp1784->tl=({ struct Cyc_List_List* _temp1785=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1785->hd=( void*) _temp1676;
_temp1785->tl= 0; _temp1785;}); _temp1784;}));} goto _LL1633; _LL1641: if(
_temp1680 == 0){ s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1786=(
char*)"return;"; struct _tagged_string _temp1787; _temp1787.curr= _temp1786;
_temp1787.base= _temp1786; _temp1787.last_plus_one= _temp1786 + 8; _temp1787;}));}
else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1788=( char*)"return "; struct _tagged_string _temp1789; _temp1789.curr=
_temp1788; _temp1789.base= _temp1788; _temp1789.last_plus_one= _temp1788 + 8;
_temp1789;})), _temp1680 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1790= _temp1680; if( _temp1790 == 0){
_throw( Null_Exception);} _temp1790;})), Cyc_PP_text(( struct _tagged_string)({
char* _temp1791=( char*)";"; struct _tagged_string _temp1792; _temp1792.curr=
_temp1791; _temp1792.base= _temp1791; _temp1792.last_plus_one= _temp1791 + 2;
_temp1792;})));} goto _LL1633; _LL1643: { int print_else;{ void* _temp1793=(
void*) _temp1682->r; _LL1795: if( _temp1793 ==( void*) Cyc_Absyn_Skip_s){ goto
_LL1796;} else{ goto _LL1797;} _LL1797: goto _LL1798; _LL1796: print_else= 0;
goto _LL1794; _LL1798: print_else= 1; goto _LL1794; _LL1794:;} s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1799=( char*)"if ("; struct
_tagged_string _temp1800; _temp1800.curr= _temp1799; _temp1800.base= _temp1799;
_temp1800.last_plus_one= _temp1799 + 5; _temp1800;})), Cyc_Absynpp_exp2doc(
_temp1686), Cyc_PP_text(( struct _tagged_string)({ char* _temp1801=( char*)") {";
struct _tagged_string _temp1802; _temp1802.curr= _temp1801; _temp1802.base=
_temp1801; _temp1802.last_plus_one= _temp1801 + 4; _temp1802;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1684))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1803=( char*)"}"; struct
_tagged_string _temp1804; _temp1804.curr= _temp1803; _temp1804.base= _temp1803;
_temp1804.last_plus_one= _temp1803 + 2; _temp1804;})), print_else? Cyc_Absynpp_cat5(
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1805=( char*)"else {";
struct _tagged_string _temp1806; _temp1806.curr= _temp1805; _temp1806.base=
_temp1805; _temp1806.last_plus_one= _temp1805 + 7; _temp1806;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1682))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1807=( char*)"}"; struct
_tagged_string _temp1808; _temp1808.curr= _temp1807; _temp1808.base= _temp1807;
_temp1808.last_plus_one= _temp1807 + 2; _temp1808;}))): Cyc_PP_nil_doc()); goto
_LL1633;} _LL1645: s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({
char* _temp1809=( char*)"while ("; struct _tagged_string _temp1810; _temp1810.curr=
_temp1809; _temp1810.base= _temp1809; _temp1810.last_plus_one= _temp1809 + 8;
_temp1810;})), Cyc_Absynpp_exp2doc( _temp1694), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1811=( char*)") {"; struct _tagged_string _temp1812;
_temp1812.curr= _temp1811; _temp1812.base= _temp1811; _temp1812.last_plus_one=
_temp1811 + 4; _temp1812;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1688))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1813=( char*)"}"; struct _tagged_string _temp1814;
_temp1814.curr= _temp1813; _temp1814.base= _temp1813; _temp1814.last_plus_one=
_temp1813 + 2; _temp1814;}))); goto _LL1633; _LL1647: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1815=( char*)"break;"; struct _tagged_string
_temp1816; _temp1816.curr= _temp1815; _temp1816.base= _temp1815; _temp1816.last_plus_one=
_temp1815 + 7; _temp1816;})); goto _LL1633; _LL1649: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1817=( char*)"continue;"; struct _tagged_string
_temp1818; _temp1818.curr= _temp1817; _temp1818.base= _temp1817; _temp1818.last_plus_one=
_temp1817 + 10; _temp1818;})); goto _LL1633; _LL1651: s= Cyc_PP_text(({ struct
_tagged_string _temp1819=* _temp1702; xprintf("goto %.*s;", _temp1819.last_plus_one
- _temp1819.curr, _temp1819.curr);})); goto _LL1633; _LL1653: s= Cyc_Absynpp_cat10(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1820=( char*)"for("; struct
_tagged_string _temp1821; _temp1821.curr= _temp1820; _temp1821.base= _temp1820;
_temp1821.last_plus_one= _temp1820 + 5; _temp1821;})), Cyc_Absynpp_exp2doc(
_temp1718), Cyc_PP_text(( struct _tagged_string)({ char* _temp1822=( char*)"; ";
struct _tagged_string _temp1823; _temp1823.curr= _temp1822; _temp1823.base=
_temp1822; _temp1823.last_plus_one= _temp1822 + 3; _temp1823;})), Cyc_Absynpp_exp2doc(
_temp1716), Cyc_PP_text(( struct _tagged_string)({ char* _temp1824=( char*)"; ";
struct _tagged_string _temp1825; _temp1825.curr= _temp1824; _temp1825.base=
_temp1824; _temp1825.last_plus_one= _temp1824 + 3; _temp1825;})), Cyc_Absynpp_exp2doc(
_temp1710), Cyc_PP_text(( struct _tagged_string)({ char* _temp1826=( char*)") {";
struct _tagged_string _temp1827; _temp1827.curr= _temp1826; _temp1827.base=
_temp1826; _temp1827.last_plus_one= _temp1826 + 4; _temp1827;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1704))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1828=( char*)"}"; struct
_tagged_string _temp1829; _temp1829.curr= _temp1828; _temp1829.base= _temp1828;
_temp1829.last_plus_one= _temp1828 + 2; _temp1829;}))); goto _LL1633; _LL1655: s=
Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char* _temp1830=( char*)"switch (";
struct _tagged_string _temp1831; _temp1831.curr= _temp1830; _temp1831.base=
_temp1830; _temp1831.last_plus_one= _temp1830 + 9; _temp1831;})), Cyc_Absynpp_exp2doc(
_temp1722), Cyc_PP_text(( struct _tagged_string)({ char* _temp1832=( char*)") {";
struct _tagged_string _temp1833; _temp1833.curr= _temp1832; _temp1833.base=
_temp1832; _temp1833.last_plus_one= _temp1832 + 4; _temp1833;})), Cyc_PP_line_doc(),
Cyc_Absynpp_switchclauses2doc( _temp1720), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1834=( char*)"}"; struct _tagged_string
_temp1835; _temp1835.curr= _temp1834; _temp1835.base= _temp1834; _temp1835.last_plus_one=
_temp1834 + 2; _temp1835;}))); goto _LL1633; _LL1657: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1836=( char*)"fallthru;"; struct _tagged_string
_temp1837; _temp1837.curr= _temp1836; _temp1837.base= _temp1836; _temp1837.last_plus_one=
_temp1836 + 10; _temp1837;})); goto _LL1633; _LL1659: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1838=( char*)"fallthru("; struct
_tagged_string _temp1839; _temp1839.curr= _temp1838; _temp1839.base= _temp1838;
_temp1839.last_plus_one= _temp1838 + 10; _temp1839;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1730), Cyc_PP_text(( struct _tagged_string)({ char* _temp1840=( char*)");";
struct _tagged_string _temp1841; _temp1841.curr= _temp1840; _temp1841.base=
_temp1840; _temp1841.last_plus_one= _temp1840 + 3; _temp1841;}))); goto _LL1633;
_LL1661: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1734), Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1732)); goto _LL1633; _LL1663: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1842=( char*)"cut "; struct
_tagged_string _temp1843; _temp1843.curr= _temp1842; _temp1843.base= _temp1842;
_temp1843.last_plus_one= _temp1842 + 5; _temp1843;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1736))); goto _LL1633; _LL1665: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1844=( char*)"splice "; struct _tagged_string
_temp1845; _temp1845.curr= _temp1844; _temp1845.base= _temp1844; _temp1845.last_plus_one=
_temp1844 + 8; _temp1845;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1738)));
goto _LL1633; _LL1667: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1740): 0){ s= Cyc_Absynpp_cat6( Cyc_Absynpp_textptr( _temp1742), Cyc_PP_text((
struct _tagged_string)({ char* _temp1846=( char*)": {"; struct _tagged_string
_temp1847; _temp1847.curr= _temp1846; _temp1847.base= _temp1846; _temp1847.last_plus_one=
_temp1846 + 4; _temp1847;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1740)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1848=( char*)"}"; struct _tagged_string _temp1849; _temp1849.curr=
_temp1848; _temp1849.base= _temp1848; _temp1849.last_plus_one= _temp1848 + 2;
_temp1849;})));} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( _temp1742), Cyc_PP_text((
struct _tagged_string)({ char* _temp1850=( char*)": "; struct _tagged_string
_temp1851; _temp1851.curr= _temp1850; _temp1851.base= _temp1850; _temp1851.last_plus_one=
_temp1850 + 3; _temp1851;})), Cyc_Absynpp_stmt2doc( _temp1740));} goto _LL1633;
_LL1669: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1852=( char*)"do {"; struct _tagged_string _temp1853; _temp1853.curr=
_temp1852; _temp1853.base= _temp1852; _temp1853.last_plus_one= _temp1852 + 5;
_temp1853;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1750)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1854=( char*)"} while ("; struct _tagged_string _temp1855; _temp1855.curr=
_temp1854; _temp1855.base= _temp1854; _temp1855.last_plus_one= _temp1854 + 10;
_temp1855;})), Cyc_Absynpp_exp2doc( _temp1748), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1856=( char*)");"; struct _tagged_string _temp1857;
_temp1857.curr= _temp1856; _temp1857.base= _temp1856; _temp1857.last_plus_one=
_temp1856 + 3; _temp1857;}))); goto _LL1633; _LL1671: s= Cyc_Absynpp_cat9( Cyc_PP_text((
struct _tagged_string)({ char* _temp1858=( char*)"try {"; struct _tagged_string
_temp1859; _temp1859.curr= _temp1858; _temp1859.base= _temp1858; _temp1859.last_plus_one=
_temp1858 + 6; _temp1859;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1754)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1860=( char*)"} catch {"; struct _tagged_string _temp1861; _temp1861.curr=
_temp1860; _temp1861.base= _temp1860; _temp1861.last_plus_one= _temp1860 + 10;
_temp1861;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1752)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1862=( char*)"}"; struct _tagged_string _temp1863; _temp1863.curr=
_temp1862; _temp1863.base= _temp1862; _temp1863.last_plus_one= _temp1862 + 2;
_temp1863;}))); goto _LL1633; _LL1673: s= Cyc_Absynpp_cat9( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1864=( char*)"region<"; struct _tagged_string
_temp1865; _temp1865.curr= _temp1864; _temp1865.base= _temp1864; _temp1865.last_plus_one=
_temp1864 + 8; _temp1865;})), Cyc_Absynpp_textptr( Cyc_Absynpp_get_name(
_temp1760)), Cyc_PP_text(( struct _tagged_string)({ char* _temp1866=( char*)">";
struct _tagged_string _temp1867; _temp1867.curr= _temp1866; _temp1867.base=
_temp1866; _temp1867.last_plus_one= _temp1866 + 2; _temp1867;})), Cyc_Absynpp_qvar2doc(
_temp1758->name), Cyc_PP_text(( struct _tagged_string)({ char* _temp1868=( char*)"{";
struct _tagged_string _temp1869; _temp1869.curr= _temp1868; _temp1869.base=
_temp1868; _temp1869.last_plus_one= _temp1868 + 2; _temp1869;})), Cyc_PP_line_doc(),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1756)), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1870=( char*)"}"; struct _tagged_string
_temp1871; _temp1871.curr= _temp1870; _temp1871.base= _temp1870; _temp1871.last_plus_one=
_temp1870 + 2; _temp1871;}))); goto _LL1633; _LL1633:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_pat2doc( struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void*
_temp1872=( void*) p->r; int _temp1904; void* _temp1906; char _temp1908; struct
_tagged_string _temp1910; struct Cyc_Absyn_Vardecl* _temp1912; struct Cyc_List_List*
_temp1914; struct Cyc_Absyn_Pat* _temp1916; struct Cyc_Absyn_Vardecl* _temp1918;
struct _tuple0* _temp1920; struct Cyc_List_List* _temp1922; struct Cyc_List_List*
_temp1924; struct _tuple0* _temp1926; struct Cyc_List_List* _temp1928; struct
Cyc_List_List* _temp1930; struct _tuple0* _temp1932; struct Cyc_List_List*
_temp1934; struct Cyc_List_List* _temp1936; struct Cyc_Core_Opt* _temp1938;
struct Cyc_Absyn_Structdecl* _temp1940; struct Cyc_Absyn_Enumfield* _temp1942;
struct Cyc_Absyn_Enumdecl* _temp1944; struct Cyc_List_List* _temp1946; struct
Cyc_List_List* _temp1948; struct Cyc_Absyn_Tunionfield* _temp1950; struct Cyc_Absyn_Tuniondecl*
_temp1952; _LL1874: if( _temp1872 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1875;}
else{ goto _LL1876;} _LL1876: if( _temp1872 ==( void*) Cyc_Absyn_Null_p){ goto
_LL1877;} else{ goto _LL1878;} _LL1878: if(( unsigned int) _temp1872 > 2u?*((
int*) _temp1872) == Cyc_Absyn_Int_p: 0){ _LL1907: _temp1906=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1872)->f1; goto _LL1905; _LL1905: _temp1904=( int)((
struct Cyc_Absyn_Int_p_struct*) _temp1872)->f2; goto _LL1879;} else{ goto
_LL1880;} _LL1880: if(( unsigned int) _temp1872 > 2u?*(( int*) _temp1872) == Cyc_Absyn_Char_p:
0){ _LL1909: _temp1908=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1872)->f1;
goto _LL1881;} else{ goto _LL1882;} _LL1882: if(( unsigned int) _temp1872 > 2u?*((
int*) _temp1872) == Cyc_Absyn_Float_p: 0){ _LL1911: _temp1910=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1872)->f1; goto _LL1883;}
else{ goto _LL1884;} _LL1884: if(( unsigned int) _temp1872 > 2u?*(( int*)
_temp1872) == Cyc_Absyn_Var_p: 0){ _LL1913: _temp1912=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1872)->f1; goto _LL1885;} else{ goto
_LL1886;} _LL1886: if(( unsigned int) _temp1872 > 2u?*(( int*) _temp1872) == Cyc_Absyn_Tuple_p:
0){ _LL1915: _temp1914=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1872)->f1; goto _LL1887;} else{ goto _LL1888;} _LL1888: if(( unsigned int)
_temp1872 > 2u?*(( int*) _temp1872) == Cyc_Absyn_Pointer_p: 0){ _LL1917:
_temp1916=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1872)->f1; goto _LL1889;} else{ goto _LL1890;} _LL1890: if(( unsigned int)
_temp1872 > 2u?*(( int*) _temp1872) == Cyc_Absyn_Reference_p: 0){ _LL1919:
_temp1918=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1872)->f1; goto _LL1891;} else{ goto _LL1892;} _LL1892: if(( unsigned int)
_temp1872 > 2u?*(( int*) _temp1872) == Cyc_Absyn_UnknownId_p: 0){ _LL1921:
_temp1920=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1872)->f1;
goto _LL1893;} else{ goto _LL1894;} _LL1894: if(( unsigned int) _temp1872 > 2u?*((
int*) _temp1872) == Cyc_Absyn_UnknownCall_p: 0){ _LL1927: _temp1926=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1872)->f1; goto _LL1925;
_LL1925: _temp1924=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1872)->f2; goto _LL1923; _LL1923: _temp1922=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1872)->f3; goto _LL1895;} else{
goto _LL1896;} _LL1896: if(( unsigned int) _temp1872 > 2u?*(( int*) _temp1872)
== Cyc_Absyn_UnknownFields_p: 0){ _LL1933: _temp1932=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp1872)->f1; goto _LL1931; _LL1931:
_temp1930=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1872)->f2; goto _LL1929; _LL1929: _temp1928=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1872)->f3; goto _LL1897;} else{
goto _LL1898;} _LL1898: if(( unsigned int) _temp1872 > 2u?*(( int*) _temp1872)
== Cyc_Absyn_Struct_p: 0){ _LL1941: _temp1940=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1872)->f1; goto _LL1939; _LL1939:
_temp1938=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1872)->f2;
goto _LL1937; _LL1937: _temp1936=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1872)->f3; goto _LL1935; _LL1935: _temp1934=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1872)->f4; goto _LL1899;} else{ goto
_LL1900;} _LL1900: if(( unsigned int) _temp1872 > 2u?*(( int*) _temp1872) == Cyc_Absyn_Enum_p:
0){ _LL1945: _temp1944=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1872)->f1; goto _LL1943; _LL1943: _temp1942=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1872)->f2; goto _LL1901;} else{ goto
_LL1902;} _LL1902: if(( unsigned int) _temp1872 > 2u?*(( int*) _temp1872) == Cyc_Absyn_Tunion_p:
0){ _LL1953: _temp1952=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1872)->f1; goto _LL1951; _LL1951: _temp1950=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1872)->f2; goto _LL1949; _LL1949:
_temp1948=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1872)->f3; goto _LL1947; _LL1947: _temp1946=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1872)->f4; goto _LL1903;} else{ goto
_LL1873;} _LL1875: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1954=(
char*)"_"; struct _tagged_string _temp1955; _temp1955.curr= _temp1954; _temp1955.base=
_temp1954; _temp1955.last_plus_one= _temp1954 + 2; _temp1955;})); goto _LL1873;
_LL1877: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1956=( char*)"null";
struct _tagged_string _temp1957; _temp1957.curr= _temp1956; _temp1957.base=
_temp1956; _temp1957.last_plus_one= _temp1956 + 5; _temp1957;})); goto _LL1873;
_LL1879: if( _temp1906 ==( void*) Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d",
_temp1904));} else{ s= Cyc_PP_text( xprintf("%u",( unsigned int) _temp1904));}
goto _LL1873; _LL1881: s= Cyc_PP_text(({ struct _tagged_string _temp1958= Cyc_Absynpp_char_escape(
_temp1908); xprintf("'%.*s'", _temp1958.last_plus_one - _temp1958.curr,
_temp1958.curr);})); goto _LL1873; _LL1883: s= Cyc_PP_text( _temp1910); goto
_LL1873; _LL1885: s= Cyc_Absynpp_qvar2doc( _temp1912->name); goto _LL1873;
_LL1887: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1959=( char*)"$("; struct _tagged_string _temp1960; _temp1960.curr=
_temp1959; _temp1960.base= _temp1959; _temp1960.last_plus_one= _temp1959 + 3;
_temp1960;})),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,(
struct _tagged_string)({ char* _temp1961=( char*)","; struct _tagged_string
_temp1962; _temp1962.curr= _temp1961; _temp1962.base= _temp1961; _temp1962.last_plus_one=
_temp1961 + 2; _temp1962;}), _temp1914), Cyc_PP_text(( struct _tagged_string)({
char* _temp1963=( char*)")"; struct _tagged_string _temp1964; _temp1964.curr=
_temp1963; _temp1964.base= _temp1963; _temp1964.last_plus_one= _temp1963 + 2;
_temp1964;}))); goto _LL1873; _LL1889: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1965=( char*)"&"; struct _tagged_string _temp1966;
_temp1966.curr= _temp1965; _temp1966.base= _temp1965; _temp1966.last_plus_one=
_temp1965 + 2; _temp1966;})), Cyc_Absynpp_pat2doc( _temp1916)); goto _LL1873;
_LL1891: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1967=( char*)"*"; struct _tagged_string _temp1968; _temp1968.curr=
_temp1967; _temp1968.base= _temp1967; _temp1968.last_plus_one= _temp1967 + 2;
_temp1968;})), Cyc_Absynpp_qvar2doc( _temp1918->name)); goto _LL1873; _LL1893: s=
Cyc_Absynpp_qvar2doc( _temp1920); goto _LL1873; _LL1895: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1926), Cyc_Absynpp_tvars2doc( _temp1924), Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1969=( char*)"("; struct _tagged_string
_temp1970; _temp1970.curr= _temp1969; _temp1970.base= _temp1969; _temp1970.last_plus_one=
_temp1969 + 2; _temp1970;}),( struct _tagged_string)({ char* _temp1971=( char*)")";
struct _tagged_string _temp1972; _temp1972.curr= _temp1971; _temp1972.base=
_temp1971; _temp1972.last_plus_one= _temp1971 + 2; _temp1972;}),( struct
_tagged_string)({ char* _temp1973=( char*)","; struct _tagged_string _temp1974;
_temp1974.curr= _temp1973; _temp1974.base= _temp1973; _temp1974.last_plus_one=
_temp1973 + 2; _temp1974;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1922))); goto _LL1873; _LL1897: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1932), Cyc_Absynpp_tvars2doc( _temp1930), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1975=( char*)"{"; struct _tagged_string _temp1976;
_temp1976.curr= _temp1975; _temp1976.base= _temp1975; _temp1976.last_plus_one=
_temp1975 + 2; _temp1976;}),( struct _tagged_string)({ char* _temp1977=( char*)"}";
struct _tagged_string _temp1978; _temp1978.curr= _temp1977; _temp1978.base=
_temp1977; _temp1978.last_plus_one= _temp1977 + 2; _temp1978;}),( struct
_tagged_string)({ char* _temp1979=( char*)","; struct _tagged_string _temp1980;
_temp1980.curr= _temp1979; _temp1980.base= _temp1979; _temp1980.last_plus_one=
_temp1979 + 2; _temp1980;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1928))); goto _LL1873; _LL1899: s= Cyc_Absynpp_cat3( _temp1940->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp1981= _temp1940->name; if( _temp1981 == 0){ _throw( Null_Exception);}
_temp1981->v;})), Cyc_Absynpp_tvars2doc( _temp1936), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1982=( char*)"{"; struct _tagged_string _temp1983;
_temp1983.curr= _temp1982; _temp1983.base= _temp1982; _temp1983.last_plus_one=
_temp1982 + 2; _temp1983;}),( struct _tagged_string)({ char* _temp1984=( char*)"}";
struct _tagged_string _temp1985; _temp1985.curr= _temp1984; _temp1985.base=
_temp1984; _temp1985.last_plus_one= _temp1984 + 2; _temp1985;}),( struct
_tagged_string)({ char* _temp1986=( char*)","; struct _tagged_string _temp1987;
_temp1987.curr= _temp1986; _temp1987.base= _temp1986; _temp1987.last_plus_one=
_temp1986 + 2; _temp1987;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1934))); goto _LL1873; _LL1901: s= Cyc_Absynpp_qvar2doc( _temp1942->name);
goto _LL1873; _LL1903: if( _temp1946 == 0){ s= Cyc_Absynpp_qvar2doc( _temp1950->name);}
else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1950->name), Cyc_Absynpp_tvars2doc(
_temp1948), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1988=( char*)"(";
struct _tagged_string _temp1989; _temp1989.curr= _temp1988; _temp1989.base=
_temp1988; _temp1989.last_plus_one= _temp1988 + 2; _temp1989;}),( struct
_tagged_string)({ char* _temp1990=( char*)")"; struct _tagged_string _temp1991;
_temp1991.curr= _temp1990; _temp1991.base= _temp1990; _temp1991.last_plus_one=
_temp1990 + 2; _temp1991;}),( struct _tagged_string)({ char* _temp1992=( char*)",";
struct _tagged_string _temp1993; _temp1993.curr= _temp1992; _temp1993.base=
_temp1992; _temp1993.last_plus_one= _temp1992 + 2; _temp1993;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1946)));} goto _LL1873; _LL1873:;} return s;} struct
Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return Cyc_Absynpp_cat2(
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1994=( char*)""; struct
_tagged_string _temp1995; _temp1995.curr= _temp1994; _temp1995.base= _temp1994;
_temp1995.last_plus_one= _temp1994 + 1; _temp1995;}),( struct _tagged_string)({
char* _temp1996=( char*)"="; struct _tagged_string _temp1997; _temp1997.curr=
_temp1996; _temp1997.base= _temp1996; _temp1997.last_plus_one= _temp1996 + 2;
_temp1997;}),( struct _tagged_string)({ char* _temp1998=( char*)"="; struct
_tagged_string _temp1999; _temp1999.curr= _temp1998; _temp1999.base= _temp1998;
_temp1999.last_plus_one= _temp1998 + 2; _temp1999;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)), Cyc_Absynpp_pat2doc((* dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
==( void*) Cyc_Absyn_Wild_p: 0){ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2000=( char*)"default: "; struct _tagged_string
_temp2001; _temp2001.curr= _temp2000; _temp2001.base= _temp2000; _temp2001.last_plus_one=
_temp2000 + 10; _temp2001;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause == 0){ return Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2002=( char*)"case "; struct
_tagged_string _temp2003; _temp2003.curr= _temp2002; _temp2003.base= _temp2002;
_temp2003.last_plus_one= _temp2002 + 6; _temp2003;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2004=( char*)": "; struct
_tagged_string _temp2005; _temp2005.curr= _temp2004; _temp2005.base= _temp2004;
_temp2005.last_plus_one= _temp2004 + 3; _temp2005;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ return Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2006=( char*)"case "; struct
_tagged_string _temp2007; _temp2007.curr= _temp2006; _temp2007.base= _temp2006;
_temp2007.last_plus_one= _temp2006 + 6; _temp2007;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2008=( char*)" && "; struct
_tagged_string _temp2009; _temp2009.curr= _temp2008; _temp2009.base= _temp2008;
_temp2009.last_plus_one= _temp2008 + 5; _temp2009;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2010= c->where_clause; if(
_temp2010 == 0){ _throw( Null_Exception);} _temp2010;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2011=( char*)": "; struct _tagged_string _temp2012;
_temp2012.curr= _temp2011; _temp2012.base= _temp2011; _temp2012.last_plus_one=
_temp2011 + 3; _temp2012;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,( struct _tagged_string)({
char* _temp2013=( char*)""; struct _tagged_string _temp2014; _temp2014.curr=
_temp2013; _temp2014.base= _temp2013; _temp2014.last_plus_one= _temp2013 + 1;
_temp2014;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield*
f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);} else{ return Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( f->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2015=( char*)" = "; struct _tagged_string _temp2016; _temp2016.curr=
_temp2015; _temp2016.base= _temp2015; _temp2016.last_plus_one= _temp2015 + 4;
_temp2016;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2017= f->tag; if( _temp2017 == 0){ _throw( Null_Exception);} _temp2017;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,(
struct _tagged_string)({ char* _temp2018=( char*)","; struct _tagged_string
_temp2019; _temp2019.curr= _temp2018; _temp2019.base= _temp2018; _temp2019.last_plus_one=
_temp2018 + 2; _temp2019;}), fs);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2020=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2044; struct Cyc_Absyn_Structdecl* _temp2046;
struct Cyc_Absyn_Uniondecl* _temp2048; struct Cyc_Absyn_Vardecl* _temp2050;
struct Cyc_Absyn_Vardecl _temp2052; struct Cyc_List_List* _temp2053; struct Cyc_Core_Opt*
_temp2055; struct Cyc_Absyn_Exp* _temp2057; void* _temp2059; struct Cyc_Absyn_Tqual
_temp2061; struct _tuple0* _temp2063; void* _temp2065; struct Cyc_Absyn_Tuniondecl*
_temp2067; struct Cyc_Absyn_Tuniondecl _temp2069; int _temp2070; struct Cyc_Core_Opt*
_temp2072; struct Cyc_List_List* _temp2074; struct _tuple0* _temp2076; void*
_temp2078; int _temp2080; struct Cyc_Absyn_Exp* _temp2082; struct Cyc_Core_Opt*
_temp2084; struct Cyc_Core_Opt* _temp2086; struct Cyc_Absyn_Pat* _temp2088;
struct Cyc_Absyn_Enumdecl* _temp2090; struct Cyc_Absyn_Enumdecl _temp2092;
struct Cyc_List_List* _temp2093; struct _tuple0* _temp2095; void* _temp2097;
struct Cyc_Absyn_Typedefdecl* _temp2099; struct Cyc_List_List* _temp2101; struct
_tagged_string* _temp2103; struct Cyc_List_List* _temp2105; struct _tuple0*
_temp2107; struct Cyc_List_List* _temp2109; _LL2022: if(*(( int*) _temp2020) ==
Cyc_Absyn_Fn_d){ _LL2045: _temp2044=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp2020)->f1; goto _LL2023;} else{ goto _LL2024;} _LL2024: if(*(( int*)
_temp2020) == Cyc_Absyn_Struct_d){ _LL2047: _temp2046=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp2020)->f1; goto _LL2025;} else{ goto
_LL2026;} _LL2026: if(*(( int*) _temp2020) == Cyc_Absyn_Union_d){ _LL2049:
_temp2048=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp2020)->f1; goto _LL2027;} else{ goto _LL2028;} _LL2028: if(*(( int*)
_temp2020) == Cyc_Absyn_Var_d){ _LL2051: _temp2050=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp2020)->f1; _temp2052=* _temp2050; _LL2066:
_temp2065=( void*) _temp2052.sc; goto _LL2064; _LL2064: _temp2063=( struct
_tuple0*) _temp2052.name; goto _LL2062; _LL2062: _temp2061=( struct Cyc_Absyn_Tqual)
_temp2052.tq; goto _LL2060; _LL2060: _temp2059=( void*) _temp2052.type; goto
_LL2058; _LL2058: _temp2057=( struct Cyc_Absyn_Exp*) _temp2052.initializer; goto
_LL2056; _LL2056: _temp2055=( struct Cyc_Core_Opt*) _temp2052.rgn; goto _LL2054;
_LL2054: _temp2053=( struct Cyc_List_List*) _temp2052.attributes; goto _LL2029;}
else{ goto _LL2030;} _LL2030: if(*(( int*) _temp2020) == Cyc_Absyn_Tunion_d){
_LL2068: _temp2067=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2020)->f1; _temp2069=* _temp2067; _LL2079: _temp2078=( void*) _temp2069.sc;
goto _LL2077; _LL2077: _temp2076=( struct _tuple0*) _temp2069.name; goto _LL2075;
_LL2075: _temp2074=( struct Cyc_List_List*) _temp2069.tvs; goto _LL2073; _LL2073:
_temp2072=( struct Cyc_Core_Opt*) _temp2069.fields; goto _LL2071; _LL2071:
_temp2070=( int) _temp2069.is_xtunion; goto _LL2031;} else{ goto _LL2032;}
_LL2032: if(*(( int*) _temp2020) == Cyc_Absyn_Let_d){ _LL2089: _temp2088=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2020)->f1; goto
_LL2087; _LL2087: _temp2086=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2020)->f2; goto _LL2085; _LL2085: _temp2084=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2020)->f3; goto _LL2083; _LL2083: _temp2082=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2020)->f4; goto
_LL2081; _LL2081: _temp2080=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2020)->f5;
goto _LL2033;} else{ goto _LL2034;} _LL2034: if(*(( int*) _temp2020) == Cyc_Absyn_Enum_d){
_LL2091: _temp2090=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp2020)->f1; _temp2092=* _temp2090; _LL2098: _temp2097=( void*) _temp2092.sc;
goto _LL2096; _LL2096: _temp2095=( struct _tuple0*) _temp2092.name; goto _LL2094;
_LL2094: _temp2093=( struct Cyc_List_List*) _temp2092.fields; goto _LL2035;}
else{ goto _LL2036;} _LL2036: if(*(( int*) _temp2020) == Cyc_Absyn_Typedef_d){
_LL2100: _temp2099=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2020)->f1; goto _LL2037;} else{ goto _LL2038;} _LL2038: if(*(( int*)
_temp2020) == Cyc_Absyn_Namespace_d){ _LL2104: _temp2103=( struct _tagged_string*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2020)->f1; goto _LL2102; _LL2102:
_temp2101=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2020)->f2; goto _LL2039;} else{ goto _LL2040;} _LL2040: if(*(( int*)
_temp2020) == Cyc_Absyn_Using_d){ _LL2108: _temp2107=( struct _tuple0*)(( struct
Cyc_Absyn_Using_d_struct*) _temp2020)->f1; goto _LL2106; _LL2106: _temp2105=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp2020)->f2; goto
_LL2041;} else{ goto _LL2042;} _LL2042: if(*(( int*) _temp2020) == Cyc_Absyn_ExternC_d){
_LL2110: _temp2109=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2020)->f1; goto _LL2043;} else{ goto _LL2021;} _LL2023: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2121=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2121[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2122; _temp2122.tag= Cyc_Absyn_FnType; _temp2122.f1=({ struct Cyc_Absyn_FnInfo
_temp2123; _temp2123.tvars= _temp2044->tvs; _temp2123.effect= _temp2044->effect;
_temp2123.ret_typ=( void*)(( void*) _temp2044->ret_type); _temp2123.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2044->args); _temp2123.varargs=
_temp2044->varargs; _temp2123.attributes= 0; _temp2123;}); _temp2122;});
_temp2121;}); s= Cyc_Absynpp_cat7( Cyc_PP_text( _temp2044->is_inline?( struct
_tagged_string)({ char* _temp2111=( char*)"inline "; struct _tagged_string
_temp2112; _temp2112.curr= _temp2111; _temp2112.base= _temp2111; _temp2112.last_plus_one=
_temp2111 + 8; _temp2112;}):( struct _tagged_string)({ char* _temp2113=( char*)"";
struct _tagged_string _temp2114; _temp2114.curr= _temp2113; _temp2114.base=
_temp2113; _temp2114.last_plus_one= _temp2113 + 1; _temp2114;})), Cyc_Absynpp_scope2doc((
void*) _temp2044->sc), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual _temp2115;
_temp2115.q_const= 0; _temp2115.q_volatile= 0; _temp2115.q_restrict= 0;
_temp2115;}), t,({ struct Cyc_Core_Opt* _temp2116=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2116->v=( void*) Cyc_Absynpp_cat2(
Cyc_Absynpp_atts2doc( _temp2044->attributes), Cyc_Absynpp_typedef_name2doc(
_temp2044->name)); _temp2116;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2117=( char*)" {"; struct _tagged_string _temp2118; _temp2118.curr=
_temp2117; _temp2118.base= _temp2117; _temp2118.last_plus_one= _temp2117 + 3;
_temp2118;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp2044->body))), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2119=( char*)"}"; struct _tagged_string _temp2120; _temp2120.curr=
_temp2119; _temp2120.base= _temp2119; _temp2120.last_plus_one= _temp2119 + 2;
_temp2120;}))); goto _LL2021;} _LL2025: if( _temp2046->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp2046->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2124=( char*)"struct "; struct _tagged_string
_temp2125; _temp2125.curr= _temp2124; _temp2125.base= _temp2124; _temp2125.last_plus_one=
_temp2124 + 8; _temp2125;})), _temp2046->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2126=( char*)""; struct _tagged_string _temp2127;
_temp2127.curr= _temp2126; _temp2127.base= _temp2126; _temp2127.last_plus_one=
_temp2126 + 1; _temp2127;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp2128= _temp2046->name; if( _temp2128 == 0){ _throw(
Null_Exception);} _temp2128->v;})), Cyc_Absynpp_ktvars2doc( _temp2046->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2129=( char*)";"; struct _tagged_string
_temp2130; _temp2130.curr= _temp2129; _temp2130.base= _temp2129; _temp2130.last_plus_one=
_temp2129 + 2; _temp2130;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2046->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2131=(
char*)"struct "; struct _tagged_string _temp2132; _temp2132.curr= _temp2131;
_temp2132.base= _temp2131; _temp2132.last_plus_one= _temp2131 + 8; _temp2132;})),
_temp2046->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2133=(
char*)""; struct _tagged_string _temp2134; _temp2134.curr= _temp2133; _temp2134.base=
_temp2133; _temp2134.last_plus_one= _temp2133 + 1; _temp2134;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2135= _temp2046->name; if(
_temp2135 == 0){ _throw( Null_Exception);} _temp2135->v;})), Cyc_Absynpp_ktvars2doc(
_temp2046->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2136=( char*)" {";
struct _tagged_string _temp2137; _temp2137.curr= _temp2136; _temp2137.base=
_temp2136; _temp2137.last_plus_one= _temp2136 + 3; _temp2137;})), Cyc_PP_nest( 2,
_temp2046->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2138=(
char*)""; struct _tagged_string _temp2139; _temp2139.curr= _temp2138; _temp2139.base=
_temp2138; _temp2139.last_plus_one= _temp2138 + 1; _temp2139;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2140= _temp2046->fields; if( _temp2140 == 0){ _throw(
Null_Exception);} _temp2140->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2141=( char*)"}"; struct _tagged_string _temp2142;
_temp2142.curr= _temp2141; _temp2142.base= _temp2141; _temp2142.last_plus_one=
_temp2141 + 2; _temp2142;})), Cyc_Absynpp_atts2doc( _temp2046->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2143=( char*)";"; struct _tagged_string
_temp2144; _temp2144.curr= _temp2143; _temp2144.base= _temp2143; _temp2144.last_plus_one=
_temp2143 + 2; _temp2144;})));} goto _LL2021; _LL2027: if( _temp2048->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp2048->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2145=( char*)"union "; struct _tagged_string
_temp2146; _temp2146.curr= _temp2145; _temp2146.base= _temp2145; _temp2146.last_plus_one=
_temp2145 + 7; _temp2146;})), _temp2048->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2147=( char*)""; struct _tagged_string _temp2148;
_temp2148.curr= _temp2147; _temp2148.base= _temp2147; _temp2148.last_plus_one=
_temp2147 + 1; _temp2148;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2149= _temp2048->name; if( _temp2149 == 0){ _throw(
Null_Exception);} _temp2149->v;})), Cyc_Absynpp_tvars2doc( _temp2048->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2150=( char*)";"; struct _tagged_string
_temp2151; _temp2151.curr= _temp2150; _temp2151.base= _temp2150; _temp2151.last_plus_one=
_temp2150 + 2; _temp2151;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2048->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2152=(
char*)"union "; struct _tagged_string _temp2153; _temp2153.curr= _temp2152;
_temp2153.base= _temp2152; _temp2153.last_plus_one= _temp2152 + 7; _temp2153;})),
_temp2048->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2154=(
char*)""; struct _tagged_string _temp2155; _temp2155.curr= _temp2154; _temp2155.base=
_temp2154; _temp2155.last_plus_one= _temp2154 + 1; _temp2155;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2156= _temp2048->name; if(
_temp2156 == 0){ _throw( Null_Exception);} _temp2156->v;})), Cyc_Absynpp_tvars2doc(
_temp2048->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2157=( char*)" {";
struct _tagged_string _temp2158; _temp2158.curr= _temp2157; _temp2158.base=
_temp2157; _temp2158.last_plus_one= _temp2157 + 3; _temp2158;})), Cyc_PP_nest( 2,
_temp2048->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2159=(
char*)""; struct _tagged_string _temp2160; _temp2160.curr= _temp2159; _temp2160.base=
_temp2159; _temp2160.last_plus_one= _temp2159 + 1; _temp2160;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2161= _temp2048->fields; if( _temp2161 == 0){ _throw(
Null_Exception);} _temp2161->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2162=( char*)"}"; struct _tagged_string _temp2163;
_temp2163.curr= _temp2162; _temp2163.base= _temp2162; _temp2163.last_plus_one=
_temp2162 + 2; _temp2163;})), Cyc_Absynpp_atts2doc( _temp2048->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2164=( char*)";"; struct _tagged_string
_temp2165; _temp2165.curr= _temp2164; _temp2165.base= _temp2164; _temp2165.last_plus_one=
_temp2164 + 2; _temp2165;})));} goto _LL2021; _LL2029: { struct Cyc_PP_Doc* sn=
Cyc_Absynpp_typedef_name2doc( _temp2063); s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(
_temp2065), Cyc_Absynpp_tqtd2doc( _temp2061, _temp2059,({ struct Cyc_Core_Opt*
_temp2166=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2166->v=( void*) sn; _temp2166;})), Cyc_Absynpp_atts2doc( _temp2053),
_temp2057 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2167=( char*)"";
struct _tagged_string _temp2168; _temp2168.curr= _temp2167; _temp2168.base=
_temp2167; _temp2168.last_plus_one= _temp2167 + 1; _temp2168;})): Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2169=( char*)" = "; struct
_tagged_string _temp2170; _temp2170.curr= _temp2169; _temp2170.base= _temp2169;
_temp2170.last_plus_one= _temp2169 + 4; _temp2170;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2171= _temp2057; if(
_temp2171 == 0){ _throw( Null_Exception);} _temp2171;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2172=( char*)";"; struct _tagged_string _temp2173;
_temp2173.curr= _temp2172; _temp2173.base= _temp2172; _temp2173.last_plus_one=
_temp2172 + 2; _temp2173;}))); goto _LL2021;} _LL2031: if( _temp2072 == 0){ s=
Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc( _temp2078), _temp2070? Cyc_PP_text((
struct _tagged_string)({ char* _temp2174=( char*)"xtunion"; struct
_tagged_string _temp2175; _temp2175.curr= _temp2174; _temp2175.base= _temp2174;
_temp2175.last_plus_one= _temp2174 + 8; _temp2175;})): Cyc_PP_text(( struct
_tagged_string)({ char* _temp2176=( char*)"tunion "; struct _tagged_string
_temp2177; _temp2177.curr= _temp2176; _temp2177.base= _temp2176; _temp2177.last_plus_one=
_temp2176 + 8; _temp2177;})), Cyc_Absynpp_typedef_name2doc( _temp2076), Cyc_Absynpp_ktvars2doc(
_temp2074), Cyc_PP_text(( struct _tagged_string)({ char* _temp2178=( char*)";";
struct _tagged_string _temp2179; _temp2179.curr= _temp2178; _temp2179.base=
_temp2178; _temp2179.last_plus_one= _temp2178 + 2; _temp2179;})));} else{ s= Cyc_Absynpp_cat8(
Cyc_Absynpp_scope2doc( _temp2078), _temp2070? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2180=( char*)"xtunion"; struct _tagged_string
_temp2181; _temp2181.curr= _temp2180; _temp2181.base= _temp2180; _temp2181.last_plus_one=
_temp2180 + 8; _temp2181;})): Cyc_PP_text(( struct _tagged_string)({ char*
_temp2182=( char*)"tunion "; struct _tagged_string _temp2183; _temp2183.curr=
_temp2182; _temp2183.base= _temp2182; _temp2183.last_plus_one= _temp2182 + 8;
_temp2183;})), Cyc_Absynpp_typedef_name2doc( _temp2076), Cyc_Absynpp_ktvars2doc(
_temp2074), Cyc_PP_text(( struct _tagged_string)({ char* _temp2184=( char*)" {";
struct _tagged_string _temp2185; _temp2185.curr= _temp2184; _temp2185.base=
_temp2184; _temp2185.last_plus_one= _temp2184 + 3; _temp2185;})), Cyc_PP_nest( 2,
_temp2072 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2186=( char*)"";
struct _tagged_string _temp2187; _temp2187.curr= _temp2186; _temp2187.base=
_temp2186; _temp2187.last_plus_one= _temp2186 + 1; _temp2187;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2188= _temp2072; if( _temp2188 == 0){ _throw(
Null_Exception);} _temp2188->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2189=( char*)"};"; struct _tagged_string _temp2190;
_temp2190.curr= _temp2189; _temp2190.base= _temp2189; _temp2190.last_plus_one=
_temp2189 + 3; _temp2190;})));} goto _LL2021; _LL2033: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp2191=( char*)"let "; struct _tagged_string
_temp2192; _temp2192.curr= _temp2191; _temp2192.base= _temp2191; _temp2192.last_plus_one=
_temp2191 + 5; _temp2192;})), Cyc_Absynpp_pat2doc( _temp2088), Cyc_PP_text((
struct _tagged_string)({ char* _temp2193=( char*)" = "; struct _tagged_string
_temp2194; _temp2194.curr= _temp2193; _temp2194.base= _temp2193; _temp2194.last_plus_one=
_temp2193 + 4; _temp2194;})), Cyc_Absynpp_exp2doc( _temp2082), Cyc_PP_text((
struct _tagged_string)({ char* _temp2195=( char*)";"; struct _tagged_string
_temp2196; _temp2196.curr= _temp2195; _temp2196.base= _temp2195; _temp2196.last_plus_one=
_temp2195 + 2; _temp2196;}))); goto _LL2021; _LL2035: s= Cyc_Absynpp_cat7( Cyc_Absynpp_scope2doc(
_temp2097), Cyc_PP_text(( struct _tagged_string)({ char* _temp2197=( char*)"enum ";
struct _tagged_string _temp2198; _temp2198.curr= _temp2197; _temp2198.base=
_temp2197; _temp2198.last_plus_one= _temp2197 + 6; _temp2198;})), Cyc_Absynpp_qvar2doc(
_temp2095), Cyc_PP_text(( struct _tagged_string)({ char* _temp2199=( char*)" {";
struct _tagged_string _temp2200; _temp2200.curr= _temp2199; _temp2200.base=
_temp2199; _temp2200.last_plus_one= _temp2199 + 3; _temp2200;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_enumfields2doc( _temp2093))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp2201=( char*)"};";
struct _tagged_string _temp2202; _temp2202.curr= _temp2201; _temp2202.base=
_temp2201; _temp2202.last_plus_one= _temp2201 + 3; _temp2202;}))); goto _LL2021;
_LL2037: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2203=( char*)"typedef "; struct _tagged_string _temp2204; _temp2204.curr=
_temp2203; _temp2204.base= _temp2203; _temp2204.last_plus_one= _temp2203 + 9;
_temp2204;})), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual _temp2205;
_temp2205.q_const= 0; _temp2205.q_volatile= 0; _temp2205.q_restrict= 0;
_temp2205;}),( void*) _temp2099->defn,({ struct Cyc_Core_Opt* _temp2206=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2206->v=( void*)
Cyc_Absynpp_cat2( Cyc_Absynpp_typedef_name2doc( _temp2099->name), Cyc_Absynpp_tvars2doc(
_temp2099->tvs)); _temp2206;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2207=( char*)";"; struct _tagged_string _temp2208; _temp2208.curr=
_temp2207; _temp2208.base= _temp2207; _temp2208.last_plus_one= _temp2207 + 2;
_temp2208;}))); goto _LL2021; _LL2039: if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_add(
_temp2103);} s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2209=( char*)"namespace "; struct _tagged_string _temp2210; _temp2210.curr=
_temp2209; _temp2210.base= _temp2209; _temp2210.last_plus_one= _temp2209 + 11;
_temp2210;})), Cyc_Absynpp_textptr( _temp2103), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2211=( char*)" {"; struct _tagged_string _temp2212;
_temp2212.curr= _temp2211; _temp2212.base= _temp2211; _temp2212.last_plus_one=
_temp2211 + 3; _temp2212;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2213=( char*)""; struct _tagged_string _temp2214; _temp2214.curr=
_temp2213; _temp2214.base= _temp2213; _temp2214.last_plus_one= _temp2213 + 1;
_temp2214;}), _temp2101), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2215=( char*)"}"; struct _tagged_string _temp2216; _temp2216.curr=
_temp2215; _temp2216.base= _temp2215; _temp2216.last_plus_one= _temp2215 + 2;
_temp2216;}))); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2021; _LL2041: if( Cyc_Absynpp_print_using_stmts){ s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2217=( char*)"using "; struct
_tagged_string _temp2218; _temp2218.curr= _temp2217; _temp2218.base= _temp2217;
_temp2218.last_plus_one= _temp2217 + 7; _temp2218;})), Cyc_Absynpp_qvar2doc(
_temp2107), Cyc_PP_text(( struct _tagged_string)({ char* _temp2219=( char*)" {";
struct _tagged_string _temp2220; _temp2220.curr= _temp2219; _temp2220.base=
_temp2219; _temp2220.last_plus_one= _temp2219 + 3; _temp2220;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2221=( char*)""; struct _tagged_string
_temp2222; _temp2222.curr= _temp2221; _temp2222.base= _temp2221; _temp2222.last_plus_one=
_temp2221 + 1; _temp2222;}), _temp2105), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2223=( char*)"}"; struct _tagged_string _temp2224;
_temp2224.curr= _temp2223; _temp2224.base= _temp2223; _temp2224.last_plus_one=
_temp2223 + 2; _temp2224;})));} else{ s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2225=( char*)"/* using "; struct _tagged_string
_temp2226; _temp2226.curr= _temp2225; _temp2226.base= _temp2225; _temp2226.last_plus_one=
_temp2225 + 10; _temp2226;})), Cyc_Absynpp_qvar2doc( _temp2107), Cyc_PP_text((
struct _tagged_string)({ char* _temp2227=( char*)" { */"; struct _tagged_string
_temp2228; _temp2228.curr= _temp2227; _temp2228.base= _temp2227; _temp2228.last_plus_one=
_temp2227 + 6; _temp2228;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2229=( char*)""; struct _tagged_string _temp2230; _temp2230.curr=
_temp2229; _temp2230.base= _temp2229; _temp2230.last_plus_one= _temp2229 + 1;
_temp2230;}), _temp2105), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2231=( char*)"/* } */"; struct _tagged_string _temp2232; _temp2232.curr=
_temp2231; _temp2232.base= _temp2231; _temp2232.last_plus_one= _temp2231 + 8;
_temp2232;})));} goto _LL2021; _LL2043: if( Cyc_Absynpp_print_externC_stmts){ s=
Cyc_Absynpp_cat5( Cyc_PP_text(( struct _tagged_string)({ char* _temp2233=( char*)"extern \"C\" {";
struct _tagged_string _temp2234; _temp2234.curr= _temp2233; _temp2234.base=
_temp2233; _temp2234.last_plus_one= _temp2233 + 13; _temp2234;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2235=( char*)""; struct _tagged_string
_temp2236; _temp2236.curr= _temp2235; _temp2236.base= _temp2235; _temp2236.last_plus_one=
_temp2235 + 1; _temp2236;}), _temp2109), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2237=( char*)"}"; struct _tagged_string _temp2238;
_temp2238.curr= _temp2237; _temp2238.base= _temp2237; _temp2238.last_plus_one=
_temp2237 + 2; _temp2238;})));} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2239=( char*)"/* extern \"C\" { */"; struct
_tagged_string _temp2240; _temp2240.curr= _temp2239; _temp2240.base= _temp2239;
_temp2240.last_plus_one= _temp2239 + 19; _temp2240;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2241=( char*)""; struct _tagged_string
_temp2242; _temp2242.curr= _temp2241; _temp2242.base= _temp2241; _temp2242.last_plus_one=
_temp2241 + 1; _temp2242;}), _temp2109), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2243=( char*)"/* } */"; struct _tagged_string
_temp2244; _temp2244.curr= _temp2243; _temp2244.base= _temp2243; _temp2244.last_plus_one=
_temp2243 + 8; _temp2244;})));} goto _LL2021; _LL2021:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void* sc){ void* _temp2245= sc; _LL2247: if( _temp2245 ==(
void*) Cyc_Absyn_Static){ goto _LL2248;} else{ goto _LL2249;} _LL2249: if(
_temp2245 ==( void*) Cyc_Absyn_Public){ goto _LL2250;} else{ goto _LL2251;}
_LL2251: if( _temp2245 ==( void*) Cyc_Absyn_Extern){ goto _LL2252;} else{ goto
_LL2253;} _LL2253: if( _temp2245 ==( void*) Cyc_Absyn_ExternC){ goto _LL2254;}
else{ goto _LL2255;} _LL2255: if( _temp2245 ==( void*) Cyc_Absyn_Abstract){ goto
_LL2256;} else{ goto _LL2246;} _LL2248: return Cyc_PP_text(( struct
_tagged_string)({ char* _temp2257=( char*)"static "; struct _tagged_string
_temp2258; _temp2258.curr= _temp2257; _temp2258.base= _temp2257; _temp2258.last_plus_one=
_temp2257 + 8; _temp2258;})); _LL2250: return Cyc_PP_nil_doc(); _LL2252: return
Cyc_PP_text(( struct _tagged_string)({ char* _temp2259=( char*)"extern "; struct
_tagged_string _temp2260; _temp2260.curr= _temp2259; _temp2260.base= _temp2259;
_temp2260.last_plus_one= _temp2259 + 8; _temp2260;})); _LL2254: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2261=( char*)"extern \"C\" "; struct
_tagged_string _temp2262; _temp2262.curr= _temp2261; _temp2262.base= _temp2261;
_temp2262.last_plus_one= _temp2261 + 12; _temp2262;})); _LL2256: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2263=( char*)"abstract "; struct
_tagged_string _temp2264; _temp2264.curr= _temp2263; _temp2264.base= _temp2263;
_temp2264.last_plus_one= _temp2263 + 10; _temp2264;})); _LL2246:;} int Cyc_Absynpp_exists_temp_tvar_in_effect(
void* t){ void* _temp2265= t; struct Cyc_Absyn_Tvar* _temp2273; struct Cyc_List_List*
_temp2275; _LL2267: if(( unsigned int) _temp2265 > 4u?*(( int*) _temp2265) ==
Cyc_Absyn_VarType: 0){ _LL2274: _temp2273=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp2265)->f1; goto _LL2268;} else{ goto _LL2269;} _LL2269: if(( unsigned int)
_temp2265 > 4u?*(( int*) _temp2265) == Cyc_Absyn_JoinEff: 0){ _LL2276: _temp2275=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp2265)->f1; goto
_LL2270;} else{ goto _LL2271;} _LL2271: goto _LL2272; _LL2268: return Cyc_Tcutil_is_temp_tvar(
_temp2273); _LL2270: return(( int(*)( int(* pred)( void*), struct Cyc_List_List*
x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect, _temp2275);
_LL2272: return 0; _LL2266:;} struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual
tq, void* t){ void* _temp2277= t; struct Cyc_Absyn_Exp* _temp2291; struct Cyc_Absyn_Tqual
_temp2293; void* _temp2295; struct Cyc_Absyn_PtrInfo _temp2297; struct Cyc_Absyn_Conref*
_temp2299; struct Cyc_Absyn_Tqual _temp2301; struct Cyc_Absyn_Conref* _temp2303;
void* _temp2305; void* _temp2307; struct Cyc_Absyn_FnInfo _temp2309; struct Cyc_List_List*
_temp2311; int _temp2313; struct Cyc_List_List* _temp2315; void* _temp2317;
struct Cyc_Core_Opt* _temp2319; struct Cyc_List_List* _temp2321; int _temp2323;
struct Cyc_Core_Opt* _temp2325; void* _temp2327; struct Cyc_Core_Opt* _temp2329;
struct Cyc_List_List* _temp2331; struct _tuple0* _temp2333; _LL2279: if((
unsigned int) _temp2277 > 4u?*(( int*) _temp2277) == Cyc_Absyn_ArrayType: 0){
_LL2296: _temp2295=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2277)->f1;
goto _LL2294; _LL2294: _temp2293=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2277)->f2; goto _LL2292; _LL2292: _temp2291=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2277)->f3; goto _LL2280;} else{ goto
_LL2281;} _LL2281: if(( unsigned int) _temp2277 > 4u?*(( int*) _temp2277) == Cyc_Absyn_PointerType:
0){ _LL2298: _temp2297=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2277)->f1; _LL2308: _temp2307=( void*) _temp2297.elt_typ; goto _LL2306;
_LL2306: _temp2305=( void*) _temp2297.rgn_typ; goto _LL2304; _LL2304: _temp2303=(
struct Cyc_Absyn_Conref*) _temp2297.nullable; goto _LL2302; _LL2302: _temp2301=(
struct Cyc_Absyn_Tqual) _temp2297.tq; goto _LL2300; _LL2300: _temp2299=( struct
Cyc_Absyn_Conref*) _temp2297.bounds; goto _LL2282;} else{ goto _LL2283;} _LL2283:
if(( unsigned int) _temp2277 > 4u?*(( int*) _temp2277) == Cyc_Absyn_FnType: 0){
_LL2310: _temp2309=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2277)->f1; _LL2322: _temp2321=( struct Cyc_List_List*) _temp2309.tvars;
goto _LL2320; _LL2320: _temp2319=( struct Cyc_Core_Opt*) _temp2309.effect; goto
_LL2318; _LL2318: _temp2317=( void*) _temp2309.ret_typ; goto _LL2316; _LL2316:
_temp2315=( struct Cyc_List_List*) _temp2309.args; goto _LL2314; _LL2314:
_temp2313=( int) _temp2309.varargs; goto _LL2312; _LL2312: _temp2311=( struct
Cyc_List_List*) _temp2309.attributes; goto _LL2284;} else{ goto _LL2285;}
_LL2285: if(( unsigned int) _temp2277 > 4u?*(( int*) _temp2277) == Cyc_Absyn_Evar:
0){ _LL2328: _temp2327=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp2277)->f1;
goto _LL2326; _LL2326: _temp2325=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp2277)->f2; goto _LL2324; _LL2324: _temp2323=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp2277)->f3; goto _LL2286;} else{ goto _LL2287;} _LL2287: if(( unsigned int)
_temp2277 > 4u?*(( int*) _temp2277) == Cyc_Absyn_TypedefType: 0){ _LL2334:
_temp2333=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2277)->f1;
goto _LL2332; _LL2332: _temp2331=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2277)->f2; goto _LL2330; _LL2330: _temp2329=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2277)->f3; goto _LL2288;} else{ goto
_LL2289;} _LL2289: goto _LL2290; _LL2280: { struct Cyc_List_List* _temp2337;
void* _temp2339; struct Cyc_Absyn_Tqual _temp2341; struct _tuple4 _temp2335= Cyc_Absynpp_to_tms(
_temp2293, _temp2295); _LL2342: _temp2341= _temp2335.f1; goto _LL2340; _LL2340:
_temp2339= _temp2335.f2; goto _LL2338; _LL2338: _temp2337= _temp2335.f3; goto
_LL2336; _LL2336: { void* tm; if( _temp2291 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;}
else{ tm=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct* _temp2343=( struct
Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct));
_temp2343[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct _temp2344; _temp2344.tag=
Cyc_Absyn_ConstArray_mod; _temp2344.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2345= _temp2291; if( _temp2345 == 0){ _throw( Null_Exception);} _temp2345;});
_temp2344;}); _temp2343;});} return({ struct _tuple4 _temp2346; _temp2346.f1=
_temp2341; _temp2346.f2= _temp2339; _temp2346.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2347=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2347->hd=( void*) tm; _temp2347->tl= _temp2337;
_temp2347;}); _temp2346;});}} _LL2282: { struct Cyc_List_List* _temp2350; void*
_temp2352; struct Cyc_Absyn_Tqual _temp2354; struct _tuple4 _temp2348= Cyc_Absynpp_to_tms(
_temp2301, _temp2307); _LL2355: _temp2354= _temp2348.f1; goto _LL2353; _LL2353:
_temp2352= _temp2348.f2; goto _LL2351; _LL2351: _temp2350= _temp2348.f3; goto
_LL2349; _LL2349: { void* ps;{ struct _tuple8 _temp2357=({ struct _tuple8
_temp2356; _temp2356.f1=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2303))->v; _temp2356.f2=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2299))->v; _temp2356;}); void* _temp2367; void* _temp2369; struct Cyc_Absyn_Exp*
_temp2371; void* _temp2373; int _temp2375; void* _temp2377; void* _temp2379;
struct Cyc_Absyn_Exp* _temp2381; void* _temp2383; int _temp2385; void* _temp2387;
void* _temp2389; void* _temp2391; _LL2359: _LL2374: _temp2373= _temp2357.f1; if((
unsigned int) _temp2373 > 1u?*(( int*) _temp2373) == Cyc_Absyn_Eq_constr: 0){
_LL2376: _temp2375=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2373)->f1;
if( _temp2375 == 1){ goto _LL2368;} else{ goto _LL2361;}} else{ goto _LL2361;}
_LL2368: _temp2367= _temp2357.f2; if(( unsigned int) _temp2367 > 1u?*(( int*)
_temp2367) == Cyc_Absyn_Eq_constr: 0){ _LL2370: _temp2369=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2367)->f1; if(( unsigned int) _temp2369 > 1u?*(( int*) _temp2369) == Cyc_Absyn_Upper_b:
0){ _LL2372: _temp2371=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2369)->f1; goto _LL2360;} else{ goto _LL2361;}} else{ goto _LL2361;}
_LL2361: _LL2384: _temp2383= _temp2357.f1; if(( unsigned int) _temp2383 > 1u?*((
int*) _temp2383) == Cyc_Absyn_Eq_constr: 0){ _LL2386: _temp2385=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp2383)->f1; if( _temp2385 == 0){ goto _LL2378;}
else{ goto _LL2363;}} else{ goto _LL2363;} _LL2378: _temp2377= _temp2357.f2; if((
unsigned int) _temp2377 > 1u?*(( int*) _temp2377) == Cyc_Absyn_Eq_constr: 0){
_LL2380: _temp2379=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2377)->f1;
if(( unsigned int) _temp2379 > 1u?*(( int*) _temp2379) == Cyc_Absyn_Upper_b: 0){
_LL2382: _temp2381=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2379)->f1; goto _LL2362;} else{ goto _LL2363;}} else{ goto _LL2363;}
_LL2363: _LL2392: _temp2391= _temp2357.f1; goto _LL2388; _LL2388: _temp2387=
_temp2357.f2; if(( unsigned int) _temp2387 > 1u?*(( int*) _temp2387) == Cyc_Absyn_Eq_constr:
0){ _LL2390: _temp2389=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2387)->f1;
if( _temp2389 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2364;} else{ goto _LL2365;}}
else{ goto _LL2365;} _LL2365: goto _LL2366; _LL2360: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2393=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2393[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2394; _temp2394.tag=
Cyc_Absyn_Nullable_ps; _temp2394.f1= _temp2371; _temp2394;}); _temp2393;}); goto
_LL2358; _LL2362: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2395=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2395[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2396; _temp2396.tag= Cyc_Absyn_NonNullable_ps; _temp2396.f1= _temp2381;
_temp2396;}); _temp2395;}); goto _LL2358; _LL2364: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2358; _LL2366: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2358;
_LL2358:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2399=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2399[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2400; _temp2400.tag=
Cyc_Absyn_Pointer_mod; _temp2400.f1=( void*) ps; _temp2400.f2=( void*) _temp2305;
_temp2400.f3= tq; _temp2400;}); _temp2399;}); return({ struct _tuple4 _temp2397;
_temp2397.f1= _temp2354; _temp2397.f2= _temp2352; _temp2397.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2398=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2398->hd=( void*) tm; _temp2398->tl= _temp2350;
_temp2398;}); _temp2397;});}}} _LL2284: if( ! Cyc_Absynpp_print_all_tvars){ if(
_temp2319 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)({ struct Cyc_Core_Opt*
_temp2401= _temp2319; if( _temp2401 == 0){ _throw( Null_Exception);} _temp2401->v;}))){
_temp2319= 0; _temp2321= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2321);}}{ struct Cyc_List_List* _temp2404;
void* _temp2406; struct Cyc_Absyn_Tqual _temp2408; struct _tuple4 _temp2402= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp2317); _LL2409: _temp2408= _temp2402.f1; goto
_LL2407; _LL2407: _temp2406= _temp2402.f2; goto _LL2405; _LL2405: _temp2404=
_temp2402.f3; goto _LL2403; _LL2403: { struct Cyc_List_List* tms= _temp2404; if(
_temp2311 != 0){ tms=({ struct Cyc_List_List* _temp2410=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2410->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2411=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2411[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2412; _temp2412.tag= Cyc_Absyn_Attributes_mod;
_temp2412.f1= 0; _temp2412.f2= _temp2311; _temp2412;}); _temp2411;})); _temp2410->tl=
tms; _temp2410;});} tms=({ struct Cyc_List_List* _temp2413=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2413->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2414=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2414[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2415; _temp2415.tag= Cyc_Absyn_Function_mod;
_temp2415.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2416=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2416[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2417; _temp2417.tag= Cyc_Absyn_WithTypes;
_temp2417.f1= _temp2315; _temp2417.f2= _temp2313; _temp2417.f3= _temp2319;
_temp2417;}); _temp2416;})); _temp2415;}); _temp2414;})); _temp2413->tl= tms;
_temp2413;}); if( _temp2321 != 0){ tms=({ struct Cyc_List_List* _temp2418=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2418->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2419=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2419[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2420; _temp2420.tag= Cyc_Absyn_TypeParams_mod;
_temp2420.f1= _temp2321; _temp2420.f2= 0; _temp2420.f3= 1; _temp2420;});
_temp2419;})); _temp2418->tl= tms; _temp2418;});} return({ struct _tuple4
_temp2421; _temp2421.f1= _temp2408; _temp2421.f2= _temp2406; _temp2421.f3= tms;
_temp2421;});}} _LL2286: if( _temp2325 == 0){ return({ struct _tuple4 _temp2422;
_temp2422.f1= tq; _temp2422.f2= t; _temp2422.f3= 0; _temp2422;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2423= _temp2325; if(
_temp2423 == 0){ _throw( Null_Exception);} _temp2423->v;}));} _LL2288: if(
_temp2329 == 0? 1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4
_temp2424; _temp2424.f1= tq; _temp2424.f2= t; _temp2424.f3= 0; _temp2424;});}
else{ return Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2425=
_temp2329; if( _temp2425 == 0){ _throw( Null_Exception);} _temp2425->v;}));}
_LL2290: return({ struct _tuple4 _temp2426; _temp2426.f1= tq; _temp2426.f2= t;
_temp2426.f3= 0; _temp2426;}); _LL2278:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp2429; void* _temp2431; struct Cyc_Absyn_Tqual _temp2433; struct _tuple4
_temp2427= Cyc_Absynpp_to_tms( tq, t); _LL2434: _temp2433= _temp2427.f1; goto
_LL2432; _LL2432: _temp2431= _temp2427.f2; goto _LL2430; _LL2430: _temp2429=
_temp2427.f3; goto _LL2428; _LL2428: _temp2429=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2429); if( _temp2429 == 0?
dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc( _temp2433), Cyc_Absynpp_ntyp2doc(
_temp2431));} else{ return Cyc_Absynpp_cat4( Cyc_Absynpp_tqual2doc( _temp2433),
Cyc_Absynpp_ntyp2doc( _temp2431), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2435=( char*)" "; struct _tagged_string _temp2436; _temp2436.curr=
_temp2435; _temp2436.base= _temp2435; _temp2436.last_plus_one= _temp2435 + 2;
_temp2436;})), Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)({
struct Cyc_Core_Opt* _temp2437= dopt; if( _temp2437 == 0){ _throw(
Null_Exception);} _temp2437->v;}), _temp2429));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return Cyc_Absynpp_cat5(
Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2438=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2438->v=(
void*) Cyc_Absynpp_textptr( f->name); _temp2438;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2439=( char*)":"; struct _tagged_string _temp2440;
_temp2440.curr= _temp2439; _temp2440.base= _temp2439; _temp2440.last_plus_one=
_temp2439 + 2; _temp2440;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp2441= f->width; if( _temp2441 == 0){ _throw(
Null_Exception);} _temp2441->v;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2442=( char*)";"; struct _tagged_string
_temp2443; _temp2443.curr= _temp2442; _temp2443.base= _temp2442; _temp2443.last_plus_one=
_temp2442 + 2; _temp2443;})));} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2444=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2444->v=( void*) Cyc_Absynpp_textptr(
f->name); _temp2444;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2445=( char*)";"; struct _tagged_string
_temp2446; _temp2446.curr= _temp2445; _temp2446.base= _temp2445; _temp2446.last_plus_one=
_temp2445 + 2; _temp2446;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,( struct _tagged_string)({ char*
_temp2447=( char*)""; struct _tagged_string _temp2448; _temp2448.curr= _temp2447;
_temp2448.base= _temp2447; _temp2448.last_plus_one= _temp2447 + 1; _temp2448;}),
fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc( struct Cyc_Absyn_Tunionfield*
f){ return Cyc_Absynpp_cat3( Cyc_Absynpp_typedef_name2doc( f->name), f->tvs == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs), f->typs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,( struct _tagged_string)({ char*
_temp2449=( char*)","; struct _tagged_string _temp2450; _temp2450.curr=
_temp2449; _temp2450.base= _temp2449; _temp2450.last_plus_one= _temp2449 + 2;
_temp2450;}), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl=({ struct Cyc_List_List*
_temp2451= tdl; if( _temp2451 == 0){ _throw( Null_Exception);} _temp2451->tl;})){
Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp2452= tdl; if( _temp2452 == 0){ _throw( Null_Exception);} _temp2452->hd;})),
72, f); fprintf( f,"\n");}} struct _tagged_string Cyc_Absynpp_decllist2string(
struct Cyc_List_List* tdl){ return Cyc_PP_string_of_doc( Cyc_PP_seql(( struct
_tagged_string)({ char* _temp2453=( char*)""; struct _tagged_string _temp2454;
_temp2454.curr= _temp2453; _temp2454.base= _temp2453; _temp2454.last_plus_one=
_temp2453 + 1; _temp2454;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
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