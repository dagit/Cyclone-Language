#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t; typedef int Cyc_off_t;
typedef unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int
Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned int
Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct
Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct
_tagged_string Cyc_Core_new_string( int); extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern struct _tagged_string Cyc_Core_string_of_int(
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
void* _temp629; struct Cyc_Position_Segment* _temp631; struct Cyc_List_List*
_temp633; struct Cyc_List_List* _temp635; struct _tuple0* _temp637; struct Cyc_Absyn_Tuniondecl*
_temp639; struct Cyc_Absyn_Tuniondecl _temp641; int _temp642; struct Cyc_Core_Opt*
_temp644; struct Cyc_List_List* _temp646; struct _tuple0* _temp648; void*
_temp650; _LL614: if(*(( int*) _temp612) == Cyc_Absyn_UnknownTunionfield){
_LL619: _temp618=( struct Cyc_Absyn_UnknownTunionFieldInfo)(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp612)->f1; _LL625: _temp624=( struct _tuple0*) _temp618.tunion_name; goto
_LL623; _LL623: _temp622=( struct _tuple0*) _temp618.field_name; goto _LL621;
_LL621: _temp620=( int) _temp618.is_xtunion; goto _LL615;} else{ goto _LL616;}
_LL616: if(*(( int*) _temp612) == Cyc_Absyn_KnownTunionfield){ _LL640: _temp639=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp612)->f1; _temp641=* _temp639; _LL651: _temp650=( void*) _temp641.sc; goto
_LL649; _LL649: _temp648=( struct _tuple0*) _temp641.name; goto _LL647; _LL647:
_temp646=( struct Cyc_List_List*) _temp641.tvs; goto _LL645; _LL645: _temp644=(
struct Cyc_Core_Opt*) _temp641.fields; goto _LL643; _LL643: _temp642=( int)
_temp641.is_xtunion; goto _LL627; _LL627: _temp626=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp612)->f2; _temp628=* _temp626;
_LL638: _temp637=( struct _tuple0*) _temp628.name; goto _LL636; _LL636: _temp635=(
struct Cyc_List_List*) _temp628.tvs; goto _LL634; _LL634: _temp633=( struct Cyc_List_List*)
_temp628.typs; goto _LL632; _LL632: _temp631=( struct Cyc_Position_Segment*)
_temp628.loc; goto _LL630; _LL630: _temp629=( void*) _temp628.sc; goto _LL617;}
else{ goto _LL613;} _LL615: _temp648= _temp624; _temp642= _temp620; _temp637=
_temp622; goto _LL617; _LL617: { struct Cyc_PP_Doc* kw= Cyc_PP_text( _temp642?(
struct _tagged_string)({ char* _temp654=( char*)"xtunion "; struct
_tagged_string _temp655; _temp655.curr= _temp654; _temp655.base= _temp654;
_temp655.last_plus_one= _temp654 + 9; _temp655;}):( struct _tagged_string)({
char* _temp656=( char*)"tunion "; struct _tagged_string _temp657; _temp657.curr=
_temp656; _temp657.base= _temp656; _temp657.last_plus_one= _temp656 + 8;
_temp657;})); s= Cyc_Absynpp_cat4( kw, Cyc_Absynpp_qvar2doc( _temp648), Cyc_PP_text((
struct _tagged_string)({ char* _temp652=( char*)"."; struct _tagged_string
_temp653; _temp653.curr= _temp652; _temp653.base= _temp652; _temp653.last_plus_one=
_temp652 + 2; _temp653;})), Cyc_Absynpp_textptr((* _temp637).f2)); goto _LL613;}
_LL613:;} goto _LL443; _LL461: { struct _tagged_string sns; struct
_tagged_string ts;{ void* _temp658= _temp522; _LL660: if( _temp658 ==( void*)
Cyc_Absyn_Signed){ goto _LL661;} else{ goto _LL662;} _LL662: if( _temp658 ==(
void*) Cyc_Absyn_Unsigned){ goto _LL663;} else{ goto _LL659;} _LL661: sns=(
struct _tagged_string)({ char* _temp664=( char*)""; struct _tagged_string
_temp665; _temp665.curr= _temp664; _temp665.base= _temp664; _temp665.last_plus_one=
_temp664 + 1; _temp665;}); goto _LL659; _LL663: sns=( struct _tagged_string)({
char* _temp666=( char*)"unsigned "; struct _tagged_string _temp667; _temp667.curr=
_temp666; _temp667.base= _temp666; _temp667.last_plus_one= _temp666 + 10;
_temp667;}); goto _LL659; _LL659:;}{ void* _temp668= _temp520; _LL670: if(
_temp668 ==( void*) Cyc_Absyn_B1){ goto _LL671;} else{ goto _LL672;} _LL672: if(
_temp668 ==( void*) Cyc_Absyn_B2){ goto _LL673;} else{ goto _LL674;} _LL674: if(
_temp668 ==( void*) Cyc_Absyn_B4){ goto _LL675;} else{ goto _LL676;} _LL676: if(
_temp668 ==( void*) Cyc_Absyn_B8){ goto _LL677;} else{ goto _LL669;} _LL671:{
void* _temp678= _temp522; _LL680: if( _temp678 ==( void*) Cyc_Absyn_Signed){
goto _LL681;} else{ goto _LL682;} _LL682: if( _temp678 ==( void*) Cyc_Absyn_Unsigned){
goto _LL683;} else{ goto _LL679;} _LL681: sns=( struct _tagged_string)({ char*
_temp684=( char*)"signed "; struct _tagged_string _temp685; _temp685.curr=
_temp684; _temp685.base= _temp684; _temp685.last_plus_one= _temp684 + 8;
_temp685;}); goto _LL679; _LL683: sns=( struct _tagged_string)({ char* _temp686=(
char*)""; struct _tagged_string _temp687; _temp687.curr= _temp686; _temp687.base=
_temp686; _temp687.last_plus_one= _temp686 + 1; _temp687;}); goto _LL679; _LL679:;}
ts=( struct _tagged_string)({ char* _temp688=( char*)"char"; struct
_tagged_string _temp689; _temp689.curr= _temp688; _temp689.base= _temp688;
_temp689.last_plus_one= _temp688 + 5; _temp689;}); goto _LL669; _LL673: ts=(
struct _tagged_string)({ char* _temp690=( char*)"short"; struct _tagged_string
_temp691; _temp691.curr= _temp690; _temp691.base= _temp690; _temp691.last_plus_one=
_temp690 + 6; _temp691;}); goto _LL669; _LL675: ts=( struct _tagged_string)({
char* _temp692=( char*)"int"; struct _tagged_string _temp693; _temp693.curr=
_temp692; _temp693.base= _temp692; _temp693.last_plus_one= _temp692 + 4;
_temp693;}); goto _LL669; _LL677: ts=( struct _tagged_string)({ char* _temp694=(
char*)"long long"; struct _tagged_string _temp695; _temp695.curr= _temp694;
_temp695.base= _temp694; _temp695.last_plus_one= _temp694 + 10; _temp695;});
goto _LL669; _LL669:;} s= Cyc_PP_text(({ struct _tagged_string _temp696= sns;
struct _tagged_string _temp697= ts; xprintf("%.*s%.*s", _temp696.last_plus_one -
_temp696.curr, _temp696.curr, _temp697.last_plus_one - _temp697.curr, _temp697.curr);}));
goto _LL443;} _LL463: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp698=(
char*)"float"; struct _tagged_string _temp699; _temp699.curr= _temp698; _temp699.base=
_temp698; _temp699.last_plus_one= _temp698 + 6; _temp699;})); goto _LL443;
_LL465: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp700=( char*)"double";
struct _tagged_string _temp701; _temp701.curr= _temp700; _temp701.base= _temp700;
_temp701.last_plus_one= _temp700 + 7; _temp701;})); goto _LL443; _LL467: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp702=( char*)"$"; struct
_tagged_string _temp703; _temp703.curr= _temp702; _temp703.base= _temp702;
_temp703.last_plus_one= _temp702 + 2; _temp703;})), Cyc_Absynpp_args2doc(
_temp524)); goto _LL443; _LL469: if( _temp530 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp704=( char*)"struct "; struct _tagged_string
_temp705; _temp705.curr= _temp704; _temp705.base= _temp704; _temp705.last_plus_one=
_temp704 + 8; _temp705;})), Cyc_Absynpp_tps2doc( _temp528));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp706=( char*)"struct "; struct
_tagged_string _temp707; _temp707.curr= _temp706; _temp707.base= _temp706;
_temp707.last_plus_one= _temp706 + 8; _temp707;})), _temp530 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp708= _temp530;
if( _temp708 == 0){ _throw( Null_Exception);} _temp708;})), Cyc_Absynpp_tps2doc(
_temp528));} goto _LL443; _LL471: if( _temp536 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp709=( char*)"union "; struct _tagged_string
_temp710; _temp710.curr= _temp709; _temp710.base= _temp709; _temp710.last_plus_one=
_temp709 + 7; _temp710;})), Cyc_Absynpp_tps2doc( _temp534));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp711=( char*)"union "; struct
_tagged_string _temp712; _temp712.curr= _temp711; _temp712.base= _temp711;
_temp712.last_plus_one= _temp711 + 7; _temp712;})), _temp536 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp713= _temp536;
if( _temp713 == 0){ _throw( Null_Exception);} _temp713;})), Cyc_Absynpp_tps2doc(
_temp534));} goto _LL443; _LL473: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp714=( char*)"struct {"; struct _tagged_string
_temp715; _temp715.curr= _temp714; _temp715.base= _temp714; _temp715.last_plus_one=
_temp714 + 9; _temp715;})), Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp538)), Cyc_PP_text(( struct _tagged_string)({ char* _temp716=( char*)"}";
struct _tagged_string _temp717; _temp717.curr= _temp716; _temp717.base= _temp716;
_temp717.last_plus_one= _temp716 + 2; _temp717;}))); goto _LL443; _LL475: s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp718=( char*)"union {"; struct
_tagged_string _temp719; _temp719.curr= _temp718; _temp719.base= _temp718;
_temp719.last_plus_one= _temp718 + 8; _temp719;})), Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp540)), Cyc_PP_text(( struct _tagged_string)({ char* _temp720=( char*)"}";
struct _tagged_string _temp721; _temp721.curr= _temp720; _temp721.base= _temp720;
_temp721.last_plus_one= _temp720 + 2; _temp721;}))); goto _LL443; _LL477: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp722=( char*)"enum "; struct
_tagged_string _temp723; _temp723.curr= _temp722; _temp723.base= _temp722;
_temp723.last_plus_one= _temp722 + 6; _temp723;})), Cyc_Absynpp_qvar2doc(
_temp544)); goto _LL443; _LL479: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp550), Cyc_Absynpp_tps2doc( _temp548)); goto _LL443; _LL481: s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp724=( char*)"region_t<";
struct _tagged_string _temp725; _temp725.curr= _temp724; _temp725.base= _temp724;
_temp725.last_plus_one= _temp724 + 10; _temp725;})), Cyc_Absynpp_rgn2doc(
_temp552), Cyc_PP_text(( struct _tagged_string)({ char* _temp726=( char*)">";
struct _tagged_string _temp727; _temp727.curr= _temp726; _temp727.base= _temp726;
_temp727.last_plus_one= _temp726 + 2; _temp727;}))); goto _LL443; _LL483: s= Cyc_Absynpp_rgn2doc(
t); goto _LL443; _LL485: s= Cyc_Absynpp_eff2doc( t); goto _LL443; _LL487: s= Cyc_Absynpp_eff2doc(
t); goto _LL443; _LL443:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp728= vo; if( _temp728 == 0){
_throw( Null_Exception);} _temp728->v;})));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc(
struct _tuple1* t){ struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt*
_temp729=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp729->v=( void*) Cyc_PP_text(*(( struct _tagged_string*)({ struct Cyc_Core_Opt*
_temp730=(* t).f1; if( _temp730 == 0){ _throw( Null_Exception);} _temp730->v;})));
_temp729;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt*
effopt){ if( effopt == 0){ if( varargs){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp731=( char*)"("; struct _tagged_string _temp732;
_temp732.curr= _temp731; _temp732.base= _temp731; _temp732.last_plus_one=
_temp731 + 2; _temp732;}),( struct _tagged_string)({ char* _temp733=( char*)")";
struct _tagged_string _temp734; _temp734.curr= _temp733; _temp734.base= _temp733;
_temp734.last_plus_one= _temp733 + 2; _temp734;}),( struct _tagged_string)({
char* _temp735=( char*)","; struct _tagged_string _temp736; _temp736.curr=
_temp735; _temp736.base= _temp735; _temp736.last_plus_one= _temp735 + 2;
_temp736;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)((( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args),({ struct Cyc_List_List* _temp737=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp737->hd=( void*) Cyc_PP_text(( struct
_tagged_string)({ char* _temp738=( char*)"..."; struct _tagged_string _temp739;
_temp739.curr= _temp738; _temp739.base= _temp738; _temp739.last_plus_one=
_temp738 + 4; _temp739;})); _temp737->tl= 0; _temp737;})));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp740=( char*)"("; struct _tagged_string
_temp741; _temp741.curr= _temp740; _temp741.base= _temp740; _temp741.last_plus_one=
_temp740 + 2; _temp741;}),( struct _tagged_string)({ char* _temp742=( char*)")";
struct _tagged_string _temp743; _temp743.curr= _temp742; _temp743.base= _temp742;
_temp743.last_plus_one= _temp742 + 2; _temp743;}),( struct _tagged_string)({
char* _temp744=( char*)","; struct _tagged_string _temp745; _temp745.curr=
_temp744; _temp745.base= _temp744; _temp745.last_plus_one= _temp744 + 2;
_temp745;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));}} else{
if( varargs){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp746=(
char*)"("; struct _tagged_string _temp747; _temp747.curr= _temp746; _temp747.base=
_temp746; _temp747.last_plus_one= _temp746 + 2; _temp747;}),( struct
_tagged_string)({ char* _temp748=( char*)")"; struct _tagged_string _temp749;
_temp749.curr= _temp748; _temp749.base= _temp748; _temp749.last_plus_one=
_temp748 + 2; _temp749;}),( struct _tagged_string)({ char* _temp750=( char*)";";
struct _tagged_string _temp751; _temp751.curr= _temp750; _temp751.base= _temp750;
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
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args),({ struct Cyc_List_List*
_temp761=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp761->hd=( void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp762=(
char*)"..."; struct _tagged_string _temp763; _temp763.curr= _temp762; _temp763.base=
_temp762; _temp763.last_plus_one= _temp762 + 4; _temp763;})); _temp761->tl= 0;
_temp761;}))); _temp752->tl=({ struct Cyc_List_List* _temp753=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp753->hd=( void*) Cyc_Absynpp_eff2doc((
void*)({ struct Cyc_Core_Opt* _temp754= effopt; if( _temp754 == 0){ _throw(
Null_Exception);} _temp754->v;})); _temp753->tl= 0; _temp753;}); _temp752;}));}
else{ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp764=( char*)"(";
struct _tagged_string _temp765; _temp765.curr= _temp764; _temp765.base= _temp764;
_temp765.last_plus_one= _temp764 + 2; _temp765;}),( struct _tagged_string)({
char* _temp766=( char*)")"; struct _tagged_string _temp767; _temp767.curr=
_temp766; _temp767.base= _temp766; _temp767.last_plus_one= _temp766 + 2;
_temp767;}),( struct _tagged_string)({ char* _temp768=( char*)";"; struct
_tagged_string _temp769; _temp769.curr= _temp768; _temp769.base= _temp768;
_temp769.last_plus_one= _temp768 + 2; _temp769;}),({ struct Cyc_List_List*
_temp770=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp770->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp773=( char*)""; struct _tagged_string _temp774; _temp774.curr= _temp773;
_temp774.base= _temp773; _temp774.last_plus_one= _temp773 + 1; _temp774;}),(
struct _tagged_string)({ char* _temp775=( char*)""; struct _tagged_string
_temp776; _temp776.curr= _temp775; _temp776.base= _temp775; _temp776.last_plus_one=
_temp775 + 1; _temp776;}),( struct _tagged_string)({ char* _temp777=( char*)",";
struct _tagged_string _temp778; _temp778.curr= _temp777; _temp778.base= _temp777;
_temp778.last_plus_one= _temp777 + 2; _temp778;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_funarg2doc, args)); _temp770->tl=({ struct Cyc_List_List* _temp771=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp771->hd=(
void*) Cyc_Absynpp_eff2doc(( void*)({ struct Cyc_Core_Opt* _temp772= effopt; if(
_temp772 == 0){ _throw( Null_Exception);} _temp772->v;})); _temp771->tl= 0;
_temp771;}); _temp770;}));}}} struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct
_tuple3* arg){ return({ struct _tuple1* _temp779=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp779->f1=({ struct Cyc_Core_Opt* _temp780=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp780->v=( void*)(*
arg).f1; _temp780;}); _temp779->f2=(* arg).f2; _temp779->f3=(* arg).f3; _temp779;});}
struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct _tagged_string* v){ return Cyc_PP_text(*
v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
prefix= 0;{ void* _temp781=(* q).f1; struct Cyc_List_List* _temp793; struct Cyc_List_List*
_temp795; struct Cyc_List_List* _temp797; struct Cyc_List_List* _temp799; _LL783:
if( _temp781 ==( void*) Cyc_Absyn_Loc_n){ goto _LL784;} else{ goto _LL785;}
_LL785: if(( unsigned int) _temp781 > 1u?*(( int*) _temp781) == Cyc_Absyn_Rel_n:
0){ _LL794: _temp793=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp781)->f1; if( _temp793 == 0){ goto _LL786;} else{ goto _LL787;}} else{ goto
_LL787;} _LL787: if(( unsigned int) _temp781 > 1u?*(( int*) _temp781) == Cyc_Absyn_Abs_n:
0){ _LL796: _temp795=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp781)->f1; if( _temp795 == 0){ goto _LL788;} else{ goto _LL789;}} else{ goto
_LL789;} _LL789: if(( unsigned int) _temp781 > 1u?*(( int*) _temp781) == Cyc_Absyn_Rel_n:
0){ _LL798: _temp797=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp781)->f1; goto _LL790;} else{ goto _LL791;} _LL791: if(( unsigned int)
_temp781 > 1u?*(( int*) _temp781) == Cyc_Absyn_Abs_n: 0){ _LL800: _temp799=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp781)->f1; goto
_LL792;} else{ goto _LL782;} _LL784: goto _LL786; _LL786: goto _LL788; _LL788:
return Cyc_Absynpp_var2doc((* q).f2); _LL790: _temp799= _temp797; goto _LL792;
_LL792: prefix= _temp799; goto _LL782; _LL782:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( prefix,({ struct
Cyc_List_List* _temp801=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp801->hd=( void*)(* q).f2; _temp801->tl= 0; _temp801;})),( struct
_tagged_string)({ char* _temp802=( char*)"_"; struct _tagged_string _temp803;
_temp803.curr= _temp802; _temp803.base= _temp802; _temp803.last_plus_one=
_temp802 + 2; _temp803;})));} else{ if((( int(*)( int(* cmp)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, prefix, Cyc_Absynpp_curr_namespace)){
return Cyc_Absynpp_var2doc((* q).f2);} else{ return Cyc_PP_text( Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
prefix,({ struct Cyc_List_List* _temp804=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp804->hd=( void*)(* q).f2; _temp804->tl= 0;
_temp804;})),( struct _tagged_string)({ char* _temp805=( char*)"::"; struct
_tagged_string _temp806; _temp806.curr= _temp805; _temp806.base= _temp805;
_temp806.last_plus_one= _temp805 + 3; _temp806;})));}}} struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc(
struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){ return Cyc_Absynpp_qvar2doc(
v);}{ void* _temp807=(* v).f1; struct Cyc_List_List* _temp813; _LL809: if((
unsigned int) _temp807 > 1u?*(( int*) _temp807) == Cyc_Absyn_Abs_n: 0){ _LL814:
_temp813=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp807)->f1;
goto _LL810;} else{ goto _LL811;} _LL811: goto _LL812; _LL810: if((( int(*)( int(*
cmp)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp813,
Cyc_Absynpp_curr_namespace) == 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{
return Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp815=(
char*)"/* wrong namespace : */ "; struct _tagged_string _temp816; _temp816.curr=
_temp815; _temp816.base= _temp815; _temp816.last_plus_one= _temp815 + 25;
_temp816;})), Cyc_Absynpp_qvar2doc( v));} _LL812: return Cyc_Absynpp_qvar2doc( v);
_LL808:;}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*
e){ void* _temp817=( void*) e->r; void* _temp889; void* _temp891; struct _tuple0*
_temp893; struct _tuple0* _temp895; struct Cyc_List_List* _temp897; void*
_temp899; struct Cyc_Absyn_Exp* _temp901; struct Cyc_Core_Opt* _temp903; struct
Cyc_Absyn_Exp* _temp905; void* _temp907; struct Cyc_Absyn_Exp* _temp909; struct
Cyc_Absyn_Exp* _temp911; struct Cyc_Absyn_Exp* _temp913; struct Cyc_Absyn_Exp*
_temp915; struct Cyc_Absyn_Exp* _temp917; struct Cyc_Absyn_Exp* _temp919; struct
Cyc_List_List* _temp921; struct Cyc_Absyn_Exp* _temp923; struct Cyc_List_List*
_temp925; struct Cyc_Absyn_Exp* _temp927; struct Cyc_Absyn_Exp* _temp929; struct
Cyc_Absyn_Exp* _temp931; struct Cyc_List_List* _temp933; struct Cyc_Absyn_Exp*
_temp935; struct Cyc_Absyn_Exp* _temp937; void* _temp939; struct Cyc_Absyn_Exp*
_temp941; struct Cyc_Absyn_Exp* _temp943; struct Cyc_Absyn_Exp* _temp945; void*
_temp947; struct Cyc_Absyn_Exp* _temp949; struct Cyc_Absyn_Exp* _temp951; struct
_tagged_string* _temp953; struct Cyc_Absyn_Exp* _temp955; struct _tagged_string*
_temp957; struct Cyc_Absyn_Exp* _temp959; struct Cyc_Absyn_Exp* _temp961; struct
Cyc_Absyn_Exp* _temp963; struct Cyc_List_List* _temp965; struct Cyc_List_List*
_temp967; struct _tuple1* _temp969; struct Cyc_List_List* _temp971; struct Cyc_Absyn_Exp*
_temp973; struct Cyc_Absyn_Exp* _temp975; struct Cyc_Absyn_Vardecl* _temp977;
struct Cyc_Absyn_Structdecl* _temp979; struct Cyc_List_List* _temp981; struct
Cyc_Core_Opt* _temp983; struct _tuple0* _temp985; struct Cyc_List_List* _temp987;
void* _temp989; struct Cyc_Absyn_Tunionfield* _temp991; struct Cyc_Absyn_Tuniondecl*
_temp993; struct Cyc_List_List* _temp995; struct Cyc_Core_Opt* _temp997; struct
Cyc_Core_Opt* _temp999; struct Cyc_Absyn_Enumfield* _temp1001; struct Cyc_Absyn_Enumdecl*
_temp1003; struct _tuple0* _temp1005; void* _temp1007; struct Cyc_Absyn_Exp*
_temp1009; struct Cyc_List_List* _temp1011; struct Cyc_Core_Opt* _temp1013;
struct Cyc_Absyn_Stmt* _temp1015; struct Cyc_Absyn_Fndecl* _temp1017; struct Cyc_Absyn_Exp*
_temp1019; _LL819: if(*(( int*) _temp817) == Cyc_Absyn_Const_e){ _LL890:
_temp889=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp817)->f1; goto _LL820;}
else{ goto _LL821;} _LL821: if(*(( int*) _temp817) == Cyc_Absyn_Var_e){ _LL894:
_temp893=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp817)->f1;
goto _LL892; _LL892: _temp891=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp817)->f2; goto _LL822;} else{ goto _LL823;} _LL823: if(*(( int*) _temp817)
== Cyc_Absyn_UnknownId_e){ _LL896: _temp895=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp817)->f1; goto _LL824;} else{ goto _LL825;} _LL825: if(*(( int*) _temp817)
== Cyc_Absyn_Primop_e){ _LL900: _temp899=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp817)->f1; goto _LL898; _LL898: _temp897=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp817)->f2; goto _LL826;} else{ goto _LL827;}
_LL827: if(*(( int*) _temp817) == Cyc_Absyn_AssignOp_e){ _LL906: _temp905=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp817)->f1;
goto _LL904; _LL904: _temp903=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp817)->f2; goto _LL902; _LL902: _temp901=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp817)->f3; goto _LL828;} else{ goto _LL829;}
_LL829: if(*(( int*) _temp817) == Cyc_Absyn_Increment_e){ _LL910: _temp909=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp817)->f1;
goto _LL908; _LL908: _temp907=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp817)->f2; goto _LL830;} else{ goto _LL831;} _LL831: if(*(( int*) _temp817)
== Cyc_Absyn_Conditional_e){ _LL916: _temp915=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp817)->f1; goto _LL914; _LL914: _temp913=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp817)->f2;
goto _LL912; _LL912: _temp911=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp817)->f3; goto _LL832;} else{ goto _LL833;} _LL833: if(*(( int*) _temp817)
== Cyc_Absyn_SeqExp_e){ _LL920: _temp919=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp817)->f1; goto _LL918; _LL918: _temp917=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp817)->f2; goto _LL834;} else{ goto _LL835;}
_LL835: if(*(( int*) _temp817) == Cyc_Absyn_UnknownCall_e){ _LL924: _temp923=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp817)->f1;
goto _LL922; _LL922: _temp921=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp817)->f2; goto _LL836;} else{ goto _LL837;} _LL837: if(*(( int*) _temp817)
== Cyc_Absyn_FnCall_e){ _LL928: _temp927=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp817)->f1; goto _LL926; _LL926: _temp925=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp817)->f2; goto _LL838;} else{ goto _LL839;}
_LL839: if(*(( int*) _temp817) == Cyc_Absyn_Throw_e){ _LL930: _temp929=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp817)->f1; goto _LL840;}
else{ goto _LL841;} _LL841: if(*(( int*) _temp817) == Cyc_Absyn_NoInstantiate_e){
_LL932: _temp931=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp817)->f1; goto _LL842;} else{ goto _LL843;} _LL843: if(*(( int*) _temp817)
== Cyc_Absyn_Instantiate_e){ _LL936: _temp935=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp817)->f1; goto _LL934; _LL934: _temp933=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp817)->f2;
goto _LL844;} else{ goto _LL845;} _LL845: if(*(( int*) _temp817) == Cyc_Absyn_Cast_e){
_LL940: _temp939=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp817)->f1; goto
_LL938; _LL938: _temp937=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp817)->f2; goto _LL846;} else{ goto _LL847;} _LL847: if(*(( int*) _temp817)
== Cyc_Absyn_New_e){ _LL944: _temp943=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp817)->f1; goto _LL942; _LL942: _temp941=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_New_e_struct*) _temp817)->f2; goto _LL848;} else{ goto _LL849;} _LL849:
if(*(( int*) _temp817) == Cyc_Absyn_Address_e){ _LL946: _temp945=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp817)->f1; goto _LL850;} else{ goto
_LL851;} _LL851: if(*(( int*) _temp817) == Cyc_Absyn_Sizeoftyp_e){ _LL948:
_temp947=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp817)->f1; goto
_LL852;} else{ goto _LL853;} _LL853: if(*(( int*) _temp817) == Cyc_Absyn_Sizeofexp_e){
_LL950: _temp949=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp817)->f1; goto _LL854;} else{ goto _LL855;} _LL855: if(*(( int*) _temp817)
== Cyc_Absyn_Deref_e){ _LL952: _temp951=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp817)->f1; goto _LL856;} else{ goto _LL857;} _LL857: if(*(( int*) _temp817)
== Cyc_Absyn_StructMember_e){ _LL956: _temp955=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp817)->f1; goto _LL954; _LL954: _temp953=(
struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp817)->f2;
goto _LL858;} else{ goto _LL859;} _LL859: if(*(( int*) _temp817) == Cyc_Absyn_StructArrow_e){
_LL960: _temp959=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp817)->f1; goto _LL958; _LL958: _temp957=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp817)->f2; goto _LL860;} else{ goto _LL861;}
_LL861: if(*(( int*) _temp817) == Cyc_Absyn_Subscript_e){ _LL964: _temp963=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp817)->f1;
goto _LL962; _LL962: _temp961=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp817)->f2; goto _LL862;} else{ goto _LL863;} _LL863: if(*(( int*) _temp817)
== Cyc_Absyn_Tuple_e){ _LL966: _temp965=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp817)->f1; goto _LL864;} else{ goto _LL865;} _LL865: if(*(( int*) _temp817)
== Cyc_Absyn_CompoundLit_e){ _LL970: _temp969=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp817)->f1; goto _LL968; _LL968: _temp967=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp817)->f2; goto _LL866;} else{ goto _LL867;}
_LL867: if(*(( int*) _temp817) == Cyc_Absyn_Array_e){ _LL972: _temp971=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp817)->f1; goto _LL868;}
else{ goto _LL869;} _LL869: if(*(( int*) _temp817) == Cyc_Absyn_Comprehension_e){
_LL978: _temp977=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp817)->f1; goto _LL976; _LL976: _temp975=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp817)->f2; goto _LL974; _LL974: _temp973=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp817)->f3;
goto _LL870;} else{ goto _LL871;} _LL871: if(*(( int*) _temp817) == Cyc_Absyn_Struct_e){
_LL986: _temp985=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp817)->f1; goto _LL984; _LL984: _temp983=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp817)->f2; goto _LL982; _LL982: _temp981=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp817)->f3; goto
_LL980; _LL980: _temp979=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp817)->f4; goto _LL872;} else{ goto _LL873;} _LL873: if(*(( int*) _temp817)
== Cyc_Absyn_AnonStruct_e){ _LL990: _temp989=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp817)->f1; goto _LL988; _LL988: _temp987=( struct Cyc_List_List*)(( struct
Cyc_Absyn_AnonStruct_e_struct*) _temp817)->f2; goto _LL874;} else{ goto _LL875;}
_LL875: if(*(( int*) _temp817) == Cyc_Absyn_Tunion_e){ _LL1000: _temp999=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp817)->f1; goto
_LL998; _LL998: _temp997=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp817)->f2; goto _LL996; _LL996: _temp995=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp817)->f3; goto _LL994; _LL994: _temp993=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp817)->f4;
goto _LL992; _LL992: _temp991=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp817)->f5; goto _LL876;} else{ goto _LL877;} _LL877: if(*(( int*) _temp817)
== Cyc_Absyn_Enum_e){ _LL1006: _temp1005=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp817)->f1; goto _LL1004; _LL1004: _temp1003=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp817)->f2; goto _LL1002; _LL1002: _temp1001=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp817)->f3;
goto _LL878;} else{ goto _LL879;} _LL879: if(*(( int*) _temp817) == Cyc_Absyn_Malloc_e){
_LL1010: _temp1009=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp817)->f1; goto _LL1008; _LL1008: _temp1007=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp817)->f2; goto _LL880;} else{ goto _LL881;} _LL881: if(*(( int*) _temp817)
== Cyc_Absyn_UnresolvedMem_e){ _LL1014: _temp1013=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp817)->f1; goto _LL1012; _LL1012:
_temp1011=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp817)->f2; goto _LL882;} else{ goto _LL883;} _LL883: if(*(( int*) _temp817)
== Cyc_Absyn_StmtExp_e){ _LL1016: _temp1015=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_StmtExp_e_struct*) _temp817)->f1; goto _LL884;} else{ goto _LL885;}
_LL885: if(*(( int*) _temp817) == Cyc_Absyn_Codegen_e){ _LL1018: _temp1017=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp817)->f1;
goto _LL886;} else{ goto _LL887;} _LL887: if(*(( int*) _temp817) == Cyc_Absyn_Fill_e){
_LL1020: _temp1019=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp817)->f1; goto _LL888;} else{ goto _LL818;} _LL820: return 10000; _LL822:
return 10000; _LL824: return 10000; _LL826: { void* _temp1021= _temp899; _LL1023:
if( _temp1021 ==( void*) Cyc_Absyn_Plus){ goto _LL1024;} else{ goto _LL1025;}
_LL1025: if( _temp1021 ==( void*) Cyc_Absyn_Times){ goto _LL1026;} else{ goto
_LL1027;} _LL1027: if( _temp1021 ==( void*) Cyc_Absyn_Minus){ goto _LL1028;}
else{ goto _LL1029;} _LL1029: if( _temp1021 ==( void*) Cyc_Absyn_Div){ goto
_LL1030;} else{ goto _LL1031;} _LL1031: if( _temp1021 ==( void*) Cyc_Absyn_Mod){
goto _LL1032;} else{ goto _LL1033;} _LL1033: if( _temp1021 ==( void*) Cyc_Absyn_Eq){
goto _LL1034;} else{ goto _LL1035;} _LL1035: if( _temp1021 ==( void*) Cyc_Absyn_Neq){
goto _LL1036;} else{ goto _LL1037;} _LL1037: if( _temp1021 ==( void*) Cyc_Absyn_Gt){
goto _LL1038;} else{ goto _LL1039;} _LL1039: if( _temp1021 ==( void*) Cyc_Absyn_Lt){
goto _LL1040;} else{ goto _LL1041;} _LL1041: if( _temp1021 ==( void*) Cyc_Absyn_Gte){
goto _LL1042;} else{ goto _LL1043;} _LL1043: if( _temp1021 ==( void*) Cyc_Absyn_Lte){
goto _LL1044;} else{ goto _LL1045;} _LL1045: if( _temp1021 ==( void*) Cyc_Absyn_Not){
goto _LL1046;} else{ goto _LL1047;} _LL1047: if( _temp1021 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1048;} else{ goto _LL1049;} _LL1049: if( _temp1021 ==( void*) Cyc_Absyn_Bitand){
goto _LL1050;} else{ goto _LL1051;} _LL1051: if( _temp1021 ==( void*) Cyc_Absyn_Bitor){
goto _LL1052;} else{ goto _LL1053;} _LL1053: if( _temp1021 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1054;} else{ goto _LL1055;} _LL1055: if( _temp1021 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1056;} else{ goto _LL1057;} _LL1057: if( _temp1021 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1058;} else{ goto _LL1059;} _LL1059: if( _temp1021 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1060;} else{ goto _LL1061;} _LL1061: if( _temp1021 ==( void*) Cyc_Absyn_Size){
goto _LL1062;} else{ goto _LL1063;} _LL1063: if( _temp1021 ==( void*) Cyc_Absyn_Printf){
goto _LL1064;} else{ goto _LL1065;} _LL1065: if( _temp1021 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1066;} else{ goto _LL1067;} _LL1067: if( _temp1021 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1068;} else{ goto _LL1069;} _LL1069: if( _temp1021 ==( void*) Cyc_Absyn_Scanf){
goto _LL1070;} else{ goto _LL1071;} _LL1071: if( _temp1021 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1072;} else{ goto _LL1073;} _LL1073: if( _temp1021 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1074;} else{ goto _LL1022;} _LL1024: return 100; _LL1026: return 110;
_LL1028: return 100; _LL1030: return 110; _LL1032: return 110; _LL1034: return
70; _LL1036: return 70; _LL1038: return 80; _LL1040: return 80; _LL1042: return
80; _LL1044: return 80; _LL1046: return 130; _LL1048: return 130; _LL1050:
return 60; _LL1052: return 40; _LL1054: return 50; _LL1056: return 90; _LL1058:
return 80; _LL1060: return 80; _LL1062: return 140; _LL1064: return 130; _LL1066:
return 130; _LL1068: return 130; _LL1070: return 130; _LL1072: return 130;
_LL1074: return 130; _LL1022:;} _LL828: return 20; _LL830: return 20; _LL832:
return 30; _LL834: return 10; _LL836: return 140; _LL838: return 140; _LL840:
return 130; _LL842: return Cyc_Absynpp_exp_prec( _temp931); _LL844: return Cyc_Absynpp_exp_prec(
_temp935); _LL846: return 120; _LL848: return 15; _LL850: return 130; _LL852:
return 130; _LL854: return 130; _LL856: return 130; _LL858: return 140; _LL860:
return 140; _LL862: return 140; _LL864: return 150; _LL866: goto _LL868; _LL868:
goto _LL870; _LL870: goto _LL872; _LL872: goto _LL874; _LL874: goto _LL876;
_LL876: goto _LL878; _LL878: goto _LL880; _LL880: return 140; _LL882: return 140;
_LL884: return 10000; _LL886: return 140; _LL888: return 140; _LL818:;} struct
Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct _tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e);
struct Cyc_PP_Doc* s;{ void* _temp1075=( void*) e->r; void* _temp1147; void*
_temp1149; struct _tuple0* _temp1151; struct _tuple0* _temp1153; struct Cyc_List_List*
_temp1155; void* _temp1157; struct Cyc_Absyn_Exp* _temp1159; struct Cyc_Core_Opt*
_temp1161; struct Cyc_Absyn_Exp* _temp1163; void* _temp1165; struct Cyc_Absyn_Exp*
_temp1167; struct Cyc_Absyn_Exp* _temp1169; struct Cyc_Absyn_Exp* _temp1171;
struct Cyc_Absyn_Exp* _temp1173; struct Cyc_Absyn_Exp* _temp1175; struct Cyc_Absyn_Exp*
_temp1177; struct Cyc_List_List* _temp1179; struct Cyc_Absyn_Exp* _temp1181;
struct Cyc_List_List* _temp1183; struct Cyc_Absyn_Exp* _temp1185; struct Cyc_Absyn_Exp*
_temp1187; struct Cyc_Absyn_Exp* _temp1189; struct Cyc_List_List* _temp1191;
struct Cyc_Absyn_Exp* _temp1193; struct Cyc_Absyn_Exp* _temp1195; void*
_temp1197; struct Cyc_Absyn_Exp* _temp1199; struct Cyc_Absyn_Exp* _temp1201;
struct Cyc_Absyn_Exp* _temp1203; void* _temp1205; struct Cyc_Absyn_Exp*
_temp1207; struct Cyc_Absyn_Exp* _temp1209; struct _tagged_string* _temp1211;
struct Cyc_Absyn_Exp* _temp1213; struct _tagged_string* _temp1215; struct Cyc_Absyn_Exp*
_temp1217; struct Cyc_Absyn_Exp* _temp1219; struct Cyc_Absyn_Exp* _temp1221;
struct Cyc_List_List* _temp1223; struct Cyc_List_List* _temp1225; struct _tuple1*
_temp1227; struct Cyc_List_List* _temp1229; struct Cyc_Absyn_Exp* _temp1231;
struct Cyc_Absyn_Exp* _temp1233; struct Cyc_Absyn_Vardecl* _temp1235; struct Cyc_Absyn_Structdecl*
_temp1237; struct Cyc_List_List* _temp1239; struct Cyc_Core_Opt* _temp1241;
struct _tuple0* _temp1243; struct Cyc_List_List* _temp1245; void* _temp1247;
struct Cyc_Absyn_Tunionfield* _temp1249; struct Cyc_Absyn_Tuniondecl* _temp1251;
struct Cyc_List_List* _temp1253; struct Cyc_Core_Opt* _temp1255; struct Cyc_Core_Opt*
_temp1257; struct Cyc_Absyn_Enumfield* _temp1259; struct Cyc_Absyn_Enumdecl*
_temp1261; struct _tuple0* _temp1263; void* _temp1265; struct Cyc_Absyn_Exp*
_temp1267; struct Cyc_List_List* _temp1269; struct Cyc_Core_Opt* _temp1271;
struct Cyc_Absyn_Stmt* _temp1273; struct Cyc_Absyn_Fndecl* _temp1275; struct Cyc_Absyn_Exp*
_temp1277; _LL1077: if(*(( int*) _temp1075) == Cyc_Absyn_Const_e){ _LL1148:
_temp1147=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1075)->f1; goto
_LL1078;} else{ goto _LL1079;} _LL1079: if(*(( int*) _temp1075) == Cyc_Absyn_Var_e){
_LL1152: _temp1151=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1075)->f1; goto _LL1150; _LL1150: _temp1149=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1075)->f2; goto _LL1080;} else{ goto _LL1081;} _LL1081: if(*(( int*)
_temp1075) == Cyc_Absyn_UnknownId_e){ _LL1154: _temp1153=( struct _tuple0*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1075)->f1; goto _LL1082;} else{ goto
_LL1083;} _LL1083: if(*(( int*) _temp1075) == Cyc_Absyn_Primop_e){ _LL1158:
_temp1157=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1075)->f1; goto
_LL1156; _LL1156: _temp1155=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1075)->f2; goto _LL1084;} else{ goto _LL1085;} _LL1085: if(*(( int*)
_temp1075) == Cyc_Absyn_AssignOp_e){ _LL1164: _temp1163=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1075)->f1; goto _LL1162; _LL1162:
_temp1161=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1075)->f2; goto _LL1160; _LL1160: _temp1159=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1075)->f3; goto _LL1086;} else{ goto
_LL1087;} _LL1087: if(*(( int*) _temp1075) == Cyc_Absyn_Increment_e){ _LL1168:
_temp1167=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1075)->f1; goto _LL1166; _LL1166: _temp1165=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1075)->f2; goto _LL1088;} else{ goto _LL1089;} _LL1089: if(*(( int*)
_temp1075) == Cyc_Absyn_Conditional_e){ _LL1174: _temp1173=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1075)->f1; goto _LL1172; _LL1172:
_temp1171=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1075)->f2; goto _LL1170; _LL1170: _temp1169=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1075)->f3; goto _LL1090;} else{
goto _LL1091;} _LL1091: if(*(( int*) _temp1075) == Cyc_Absyn_SeqExp_e){ _LL1178:
_temp1177=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1075)->f1; goto _LL1176; _LL1176: _temp1175=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1075)->f2; goto _LL1092;} else{ goto
_LL1093;} _LL1093: if(*(( int*) _temp1075) == Cyc_Absyn_UnknownCall_e){ _LL1182:
_temp1181=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1075)->f1; goto _LL1180; _LL1180: _temp1179=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1075)->f2; goto _LL1094;} else{
goto _LL1095;} _LL1095: if(*(( int*) _temp1075) == Cyc_Absyn_FnCall_e){ _LL1186:
_temp1185=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1075)->f1; goto _LL1184; _LL1184: _temp1183=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1075)->f2; goto _LL1096;} else{ goto
_LL1097;} _LL1097: if(*(( int*) _temp1075) == Cyc_Absyn_Throw_e){ _LL1188:
_temp1187=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1075)->f1;
goto _LL1098;} else{ goto _LL1099;} _LL1099: if(*(( int*) _temp1075) == Cyc_Absyn_NoInstantiate_e){
_LL1190: _temp1189=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1075)->f1; goto _LL1100;} else{ goto _LL1101;} _LL1101: if(*(( int*)
_temp1075) == Cyc_Absyn_Instantiate_e){ _LL1194: _temp1193=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1075)->f1; goto _LL1192; _LL1192:
_temp1191=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1075)->f2; goto _LL1102;} else{ goto _LL1103;} _LL1103: if(*(( int*)
_temp1075) == Cyc_Absyn_Cast_e){ _LL1198: _temp1197=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1075)->f1; goto _LL1196; _LL1196: _temp1195=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Cast_e_struct*) _temp1075)->f2; goto _LL1104;} else{ goto
_LL1105;} _LL1105: if(*(( int*) _temp1075) == Cyc_Absyn_Address_e){ _LL1200:
_temp1199=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp1075)->f1; goto _LL1106;} else{ goto _LL1107;} _LL1107: if(*(( int*)
_temp1075) == Cyc_Absyn_New_e){ _LL1204: _temp1203=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1075)->f1; goto _LL1202; _LL1202: _temp1201=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1075)->f2; goto
_LL1108;} else{ goto _LL1109;} _LL1109: if(*(( int*) _temp1075) == Cyc_Absyn_Sizeoftyp_e){
_LL1206: _temp1205=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1075)->f1;
goto _LL1110;} else{ goto _LL1111;} _LL1111: if(*(( int*) _temp1075) == Cyc_Absyn_Sizeofexp_e){
_LL1208: _temp1207=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1075)->f1; goto _LL1112;} else{ goto _LL1113;} _LL1113: if(*(( int*)
_temp1075) == Cyc_Absyn_Deref_e){ _LL1210: _temp1209=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp1075)->f1; goto _LL1114;} else{ goto
_LL1115;} _LL1115: if(*(( int*) _temp1075) == Cyc_Absyn_StructMember_e){ _LL1214:
_temp1213=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1075)->f1; goto _LL1212; _LL1212: _temp1211=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1075)->f2; goto _LL1116;} else{
goto _LL1117;} _LL1117: if(*(( int*) _temp1075) == Cyc_Absyn_StructArrow_e){
_LL1218: _temp1217=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1075)->f1; goto _LL1216; _LL1216: _temp1215=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1075)->f2; goto _LL1118;} else{
goto _LL1119;} _LL1119: if(*(( int*) _temp1075) == Cyc_Absyn_Subscript_e){
_LL1222: _temp1221=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1075)->f1; goto _LL1220; _LL1220: _temp1219=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1075)->f2; goto _LL1120;} else{ goto
_LL1121;} _LL1121: if(*(( int*) _temp1075) == Cyc_Absyn_Tuple_e){ _LL1224:
_temp1223=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1075)->f1;
goto _LL1122;} else{ goto _LL1123;} _LL1123: if(*(( int*) _temp1075) == Cyc_Absyn_CompoundLit_e){
_LL1228: _temp1227=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1075)->f1; goto _LL1226; _LL1226: _temp1225=( struct Cyc_List_List*)((
struct Cyc_Absyn_CompoundLit_e_struct*) _temp1075)->f2; goto _LL1124;} else{
goto _LL1125;} _LL1125: if(*(( int*) _temp1075) == Cyc_Absyn_Array_e){ _LL1230:
_temp1229=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1075)->f1;
goto _LL1126;} else{ goto _LL1127;} _LL1127: if(*(( int*) _temp1075) == Cyc_Absyn_Comprehension_e){
_LL1236: _temp1235=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1075)->f1; goto _LL1234; _LL1234: _temp1233=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1075)->f2; goto _LL1232; _LL1232:
_temp1231=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1075)->f3; goto _LL1128;} else{ goto _LL1129;} _LL1129: if(*(( int*)
_temp1075) == Cyc_Absyn_Struct_e){ _LL1244: _temp1243=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1075)->f1; goto _LL1242; _LL1242:
_temp1241=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1075)->f2;
goto _LL1240; _LL1240: _temp1239=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1075)->f3; goto _LL1238; _LL1238: _temp1237=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1075)->f4; goto _LL1130;} else{ goto
_LL1131;} _LL1131: if(*(( int*) _temp1075) == Cyc_Absyn_AnonStruct_e){ _LL1248:
_temp1247=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1075)->f1; goto
_LL1246; _LL1246: _temp1245=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1075)->f2; goto _LL1132;} else{ goto _LL1133;} _LL1133: if(*(( int*)
_temp1075) == Cyc_Absyn_Tunion_e){ _LL1258: _temp1257=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1075)->f1; goto _LL1256; _LL1256:
_temp1255=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1075)->f2;
goto _LL1254; _LL1254: _temp1253=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1075)->f3; goto _LL1252; _LL1252: _temp1251=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1075)->f4; goto _LL1250; _LL1250:
_temp1249=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1075)->f5; goto _LL1134;} else{ goto _LL1135;} _LL1135: if(*(( int*)
_temp1075) == Cyc_Absyn_Enum_e){ _LL1264: _temp1263=( struct _tuple0*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp1075)->f1; goto _LL1262; _LL1262: _temp1261=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1075)->f2;
goto _LL1260; _LL1260: _temp1259=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1075)->f3; goto _LL1136;} else{ goto _LL1137;} _LL1137: if(*(( int*)
_temp1075) == Cyc_Absyn_Malloc_e){ _LL1268: _temp1267=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Malloc_e_struct*) _temp1075)->f1; goto _LL1266; _LL1266:
_temp1265=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp1075)->f2; goto
_LL1138;} else{ goto _LL1139;} _LL1139: if(*(( int*) _temp1075) == Cyc_Absyn_UnresolvedMem_e){
_LL1272: _temp1271=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1075)->f1; goto _LL1270; _LL1270: _temp1269=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1075)->f2; goto _LL1140;} else{
goto _LL1141;} _LL1141: if(*(( int*) _temp1075) == Cyc_Absyn_StmtExp_e){ _LL1274:
_temp1273=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1075)->f1; goto _LL1142;} else{ goto _LL1143;} _LL1143: if(*(( int*)
_temp1075) == Cyc_Absyn_Codegen_e){ _LL1276: _temp1275=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp1075)->f1; goto _LL1144;} else{ goto
_LL1145;} _LL1145: if(*(( int*) _temp1075) == Cyc_Absyn_Fill_e){ _LL1278:
_temp1277=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1075)->f1;
goto _LL1146;} else{ goto _LL1076;} _LL1078: s= Cyc_Absynpp_cnst2doc( _temp1147);
goto _LL1076; _LL1080: _temp1153= _temp1151; goto _LL1082; _LL1082: s= Cyc_Absynpp_qvar2doc(
_temp1153); goto _LL1076; _LL1084: s= Cyc_Absynpp_primapp2doc( myprec, _temp1157,
_temp1155); goto _LL1076; _LL1086: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1163), Cyc_PP_text(( struct _tagged_string)({ char* _temp1279=(
char*)" "; struct _tagged_string _temp1280; _temp1280.curr= _temp1279; _temp1280.base=
_temp1279; _temp1280.last_plus_one= _temp1279 + 2; _temp1280;})), _temp1161 == 0?
Cyc_PP_text(( struct _tagged_string)({ char* _temp1281=( char*)""; struct
_tagged_string _temp1282; _temp1282.curr= _temp1281; _temp1282.base= _temp1281;
_temp1282.last_plus_one= _temp1281 + 1; _temp1282;})): Cyc_Absynpp_prim2doc((
void*)({ struct Cyc_Core_Opt* _temp1283= _temp1161; if( _temp1283 == 0){ _throw(
Null_Exception);} _temp1283->v;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1284=( char*)"= "; struct _tagged_string _temp1285; _temp1285.curr=
_temp1284; _temp1285.base= _temp1284; _temp1285.last_plus_one= _temp1284 + 3;
_temp1285;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1159)); goto _LL1076;
_LL1088: { struct Cyc_PP_Doc* es= Cyc_Absynpp_exp2doc_prec( myprec, _temp1167);{
void* _temp1286= _temp1165; _LL1288: if( _temp1286 ==( void*) Cyc_Absyn_PreInc){
goto _LL1289;} else{ goto _LL1290;} _LL1290: if( _temp1286 ==( void*) Cyc_Absyn_PreDec){
goto _LL1291;} else{ goto _LL1292;} _LL1292: if( _temp1286 ==( void*) Cyc_Absyn_PostInc){
goto _LL1293;} else{ goto _LL1294;} _LL1294: if( _temp1286 ==( void*) Cyc_Absyn_PostDec){
goto _LL1295;} else{ goto _LL1287;} _LL1289: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1296=( char*)"++"; struct _tagged_string
_temp1297; _temp1297.curr= _temp1296; _temp1297.base= _temp1296; _temp1297.last_plus_one=
_temp1296 + 3; _temp1297;})), es); goto _LL1287; _LL1291: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1298=( char*)"--"; struct
_tagged_string _temp1299; _temp1299.curr= _temp1298; _temp1299.base= _temp1298;
_temp1299.last_plus_one= _temp1298 + 3; _temp1299;})), es); goto _LL1287;
_LL1293: s= Cyc_Absynpp_cat2( es, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1300=( char*)"++"; struct _tagged_string _temp1301; _temp1301.curr=
_temp1300; _temp1301.base= _temp1300; _temp1301.last_plus_one= _temp1300 + 3;
_temp1301;}))); goto _LL1287; _LL1295: s= Cyc_Absynpp_cat2( es, Cyc_PP_text((
struct _tagged_string)({ char* _temp1302=( char*)"--"; struct _tagged_string
_temp1303; _temp1303.curr= _temp1302; _temp1303.base= _temp1302; _temp1303.last_plus_one=
_temp1302 + 3; _temp1303;}))); goto _LL1287; _LL1287:;} goto _LL1076;} _LL1090:{
struct _tuple8 _temp1305=({ struct _tuple8 _temp1304; _temp1304.f1=( void*)
_temp1171->r; _temp1304.f2=( void*) _temp1169->r; _temp1304;}); _LL1307: goto
_LL1308; _LL1308: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1173), Cyc_PP_text(( struct _tagged_string)({ char* _temp1309=( char*)" ? ";
struct _tagged_string _temp1310; _temp1310.curr= _temp1309; _temp1310.base=
_temp1309; _temp1310.last_plus_one= _temp1309 + 4; _temp1310;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1171), Cyc_PP_text(( struct _tagged_string)({ char* _temp1311=( char*)" : ";
struct _tagged_string _temp1312; _temp1312.curr= _temp1311; _temp1312.base=
_temp1311; _temp1312.last_plus_one= _temp1311 + 4; _temp1312;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1169)); goto _LL1306; _LL1306:;} goto _LL1076; _LL1092: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1177), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1313=( char*)", "; struct _tagged_string _temp1314;
_temp1314.curr= _temp1313; _temp1314.base= _temp1313; _temp1314.last_plus_one=
_temp1313 + 3; _temp1314;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1175));
goto _LL1076; _LL1094: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1181), Cyc_PP_text(( struct _tagged_string)({ char* _temp1315=( char*)"(";
struct _tagged_string _temp1316; _temp1316.curr= _temp1315; _temp1316.base=
_temp1315; _temp1316.last_plus_one= _temp1315 + 2; _temp1316;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1179), Cyc_PP_text(( struct _tagged_string)({ char* _temp1317=( char*)")";
struct _tagged_string _temp1318; _temp1318.curr= _temp1317; _temp1318.base=
_temp1317; _temp1318.last_plus_one= _temp1317 + 2; _temp1318;}))); goto _LL1076;
_LL1096: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1185), Cyc_PP_text((
struct _tagged_string)({ char* _temp1319=( char*)"("; struct _tagged_string
_temp1320; _temp1320.curr= _temp1319; _temp1320.base= _temp1319; _temp1320.last_plus_one=
_temp1319 + 2; _temp1320;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1183), Cyc_PP_text((
struct _tagged_string)({ char* _temp1321=( char*)")"; struct _tagged_string
_temp1322; _temp1322.curr= _temp1321; _temp1322.base= _temp1321; _temp1322.last_plus_one=
_temp1321 + 2; _temp1322;}))); goto _LL1076; _LL1098: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1323=( char*)"throw "; struct _tagged_string
_temp1324; _temp1324.curr= _temp1323; _temp1324.base= _temp1323; _temp1324.last_plus_one=
_temp1323 + 7; _temp1324;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1187));
goto _LL1076; _LL1100: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1189); goto
_LL1076; _LL1102: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1193); goto _LL1076;
_LL1104: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1325=( char*)"("; struct _tagged_string _temp1326; _temp1326.curr=
_temp1325; _temp1326.base= _temp1325; _temp1326.last_plus_one= _temp1325 + 2;
_temp1326;})), Cyc_Absynpp_typ2doc( _temp1197), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1327=( char*)")"; struct _tagged_string _temp1328;
_temp1328.curr= _temp1327; _temp1328.base= _temp1327; _temp1328.last_plus_one=
_temp1327 + 2; _temp1328;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1195));
goto _LL1076; _LL1106: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({
char* _temp1329=( char*)"&"; struct _tagged_string _temp1330; _temp1330.curr=
_temp1329; _temp1330.base= _temp1329; _temp1330.last_plus_one= _temp1329 + 2;
_temp1330;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1199)); goto _LL1076;
_LL1108: if( _temp1203 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1331=( char*)"new "; struct _tagged_string
_temp1332; _temp1332.curr= _temp1331; _temp1332.base= _temp1331; _temp1332.last_plus_one=
_temp1331 + 5; _temp1332;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1201));}
else{ s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1333=( char*)"rnew("; struct _tagged_string _temp1334; _temp1334.curr=
_temp1333; _temp1334.base= _temp1333; _temp1334.last_plus_one= _temp1333 + 6;
_temp1334;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1335= _temp1203; if( _temp1335 == 0){ _throw( Null_Exception);} _temp1335;})),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1336=( char*)") "; struct
_tagged_string _temp1337; _temp1337.curr= _temp1336; _temp1337.base= _temp1336;
_temp1337.last_plus_one= _temp1336 + 3; _temp1337;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1201));} goto _LL1076; _LL1110: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1338=( char*)"sizeof("; struct
_tagged_string _temp1339; _temp1339.curr= _temp1338; _temp1339.base= _temp1338;
_temp1339.last_plus_one= _temp1338 + 8; _temp1339;})), Cyc_Absynpp_typ2doc(
_temp1205), Cyc_PP_text(( struct _tagged_string)({ char* _temp1340=( char*)")";
struct _tagged_string _temp1341; _temp1341.curr= _temp1340; _temp1341.base=
_temp1340; _temp1341.last_plus_one= _temp1340 + 2; _temp1341;}))); goto _LL1076;
_LL1112: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1342=( char*)"sizeof("; struct _tagged_string _temp1343; _temp1343.curr=
_temp1342; _temp1343.base= _temp1342; _temp1343.last_plus_one= _temp1342 + 8;
_temp1343;})), Cyc_Absynpp_exp2doc( _temp1207), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1344=( char*)")"; struct _tagged_string _temp1345;
_temp1345.curr= _temp1344; _temp1345.base= _temp1344; _temp1345.last_plus_one=
_temp1344 + 2; _temp1345;}))); goto _LL1076; _LL1114: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1346=( char*)"*"; struct _tagged_string
_temp1347; _temp1347.curr= _temp1346; _temp1347.base= _temp1346; _temp1347.last_plus_one=
_temp1346 + 2; _temp1347;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1209));
goto _LL1076; _LL1116: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1213), Cyc_PP_text(( struct _tagged_string)({ char* _temp1348=( char*)".";
struct _tagged_string _temp1349; _temp1349.curr= _temp1348; _temp1349.base=
_temp1348; _temp1349.last_plus_one= _temp1348 + 2; _temp1349;})), Cyc_Absynpp_textptr(
_temp1211)); goto _LL1076; _LL1118: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1217), Cyc_PP_text(( struct _tagged_string)({ char* _temp1350=(
char*)"->"; struct _tagged_string _temp1351; _temp1351.curr= _temp1350;
_temp1351.base= _temp1350; _temp1351.last_plus_one= _temp1350 + 3; _temp1351;})),
Cyc_Absynpp_textptr( _temp1215)); goto _LL1076; _LL1120: s= Cyc_Absynpp_cat4(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1221), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1352=( char*)"["; struct _tagged_string _temp1353;
_temp1353.curr= _temp1352; _temp1353.base= _temp1352; _temp1353.last_plus_one=
_temp1352 + 2; _temp1353;})), Cyc_Absynpp_exp2doc( _temp1219), Cyc_PP_text((
struct _tagged_string)({ char* _temp1354=( char*)"]"; struct _tagged_string
_temp1355; _temp1355.curr= _temp1354; _temp1355.base= _temp1354; _temp1355.last_plus_one=
_temp1354 + 2; _temp1355;}))); goto _LL1076; _LL1122: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1356=( char*)"$("; struct _tagged_string
_temp1357; _temp1357.curr= _temp1356; _temp1357.base= _temp1356; _temp1357.last_plus_one=
_temp1356 + 3; _temp1357;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1223), Cyc_PP_text((
struct _tagged_string)({ char* _temp1358=( char*)")"; struct _tagged_string
_temp1359; _temp1359.curr= _temp1358; _temp1359.base= _temp1358; _temp1359.last_plus_one=
_temp1358 + 2; _temp1359;}))); goto _LL1076; _LL1124: s= Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1360=( char*)"("; struct _tagged_string
_temp1361; _temp1361.curr= _temp1360; _temp1361.base= _temp1360; _temp1361.last_plus_one=
_temp1360 + 2; _temp1361;})), Cyc_Absynpp_typ2doc((* _temp1227).f3), Cyc_PP_text((
struct _tagged_string)({ char* _temp1362=( char*)")"; struct _tagged_string
_temp1363; _temp1363.curr= _temp1362; _temp1363.base= _temp1362; _temp1363.last_plus_one=
_temp1362 + 2; _temp1363;})), Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp1364=( char*)"{"; struct _tagged_string _temp1365; _temp1365.curr=
_temp1364; _temp1365.base= _temp1364; _temp1365.last_plus_one= _temp1364 + 2;
_temp1365;}),( struct _tagged_string)({ char* _temp1366=( char*)"}"; struct
_tagged_string _temp1367; _temp1367.curr= _temp1366; _temp1367.base= _temp1366;
_temp1367.last_plus_one= _temp1366 + 2; _temp1367;}),( struct _tagged_string)({
char* _temp1368=( char*)","; struct _tagged_string _temp1369; _temp1369.curr=
_temp1368; _temp1369.base= _temp1368; _temp1369.last_plus_one= _temp1368 + 2;
_temp1369;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1225))); goto _LL1076; _LL1126: s= Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1370=( char*)"{"; struct _tagged_string _temp1371;
_temp1371.curr= _temp1370; _temp1371.base= _temp1370; _temp1371.last_plus_one=
_temp1370 + 2; _temp1371;}),( struct _tagged_string)({ char* _temp1372=( char*)"}";
struct _tagged_string _temp1373; _temp1373.curr= _temp1372; _temp1373.base=
_temp1372; _temp1373.last_plus_one= _temp1372 + 2; _temp1373;}),( struct
_tagged_string)({ char* _temp1374=( char*)","; struct _tagged_string _temp1375;
_temp1375.curr= _temp1374; _temp1375.base= _temp1374; _temp1375.last_plus_one=
_temp1374 + 2; _temp1375;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1229)); goto _LL1076; _LL1128: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1376=( char*)"{for "; struct _tagged_string
_temp1377; _temp1377.curr= _temp1376; _temp1377.base= _temp1376; _temp1377.last_plus_one=
_temp1376 + 6; _temp1377;})), Cyc_PP_text(*(* _temp1235->name).f2), Cyc_PP_text((
struct _tagged_string)({ char* _temp1378=( char*)" < "; struct _tagged_string
_temp1379; _temp1379.curr= _temp1378; _temp1379.base= _temp1378; _temp1379.last_plus_one=
_temp1378 + 4; _temp1379;})), Cyc_Absynpp_exp2doc( _temp1233), Cyc_PP_text((
struct _tagged_string)({ char* _temp1380=( char*)" : "; struct _tagged_string
_temp1381; _temp1381.curr= _temp1380; _temp1381.base= _temp1380; _temp1381.last_plus_one=
_temp1380 + 4; _temp1381;})), Cyc_Absynpp_exp2doc( _temp1231), Cyc_PP_text((
struct _tagged_string)({ char* _temp1382=( char*)"}"; struct _tagged_string
_temp1383; _temp1383.curr= _temp1382; _temp1383.base= _temp1382; _temp1383.last_plus_one=
_temp1382 + 2; _temp1383;}))); goto _LL1076; _LL1130: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1243), Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1384=( char*)"{";
struct _tagged_string _temp1385; _temp1385.curr= _temp1384; _temp1385.base=
_temp1384; _temp1385.last_plus_one= _temp1384 + 2; _temp1385;}),( struct
_tagged_string)({ char* _temp1386=( char*)"}"; struct _tagged_string _temp1387;
_temp1387.curr= _temp1386; _temp1387.base= _temp1386; _temp1387.last_plus_one=
_temp1386 + 2; _temp1387;}),( struct _tagged_string)({ char* _temp1388=( char*)",";
struct _tagged_string _temp1389; _temp1389.curr= _temp1388; _temp1389.base=
_temp1388; _temp1389.last_plus_one= _temp1388 + 2; _temp1389;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1239))); goto _LL1076; _LL1132: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1390=( char*)"{"; struct _tagged_string
_temp1391; _temp1391.curr= _temp1390; _temp1391.base= _temp1390; _temp1391.last_plus_one=
_temp1390 + 2; _temp1391;}),( struct _tagged_string)({ char* _temp1392=( char*)"}";
struct _tagged_string _temp1393; _temp1393.curr= _temp1392; _temp1393.base=
_temp1392; _temp1393.last_plus_one= _temp1392 + 2; _temp1393;}),( struct
_tagged_string)({ char* _temp1394=( char*)","; struct _tagged_string _temp1395;
_temp1395.curr= _temp1394; _temp1395.base= _temp1394; _temp1395.last_plus_one=
_temp1394 + 2; _temp1395;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1245)); goto _LL1076; _LL1134: if( _temp1253 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1249->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1249->name),
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1396=( char*)"(";
struct _tagged_string _temp1397; _temp1397.curr= _temp1396; _temp1397.base=
_temp1396; _temp1397.last_plus_one= _temp1396 + 2; _temp1397;}),( struct
_tagged_string)({ char* _temp1398=( char*)")"; struct _tagged_string _temp1399;
_temp1399.curr= _temp1398; _temp1399.base= _temp1398; _temp1399.last_plus_one=
_temp1398 + 2; _temp1399;}),( struct _tagged_string)({ char* _temp1400=( char*)",";
struct _tagged_string _temp1401; _temp1401.curr= _temp1400; _temp1401.base=
_temp1400; _temp1401.last_plus_one= _temp1400 + 2; _temp1401;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp1253)));} goto _LL1076; _LL1136: s= Cyc_Absynpp_qvar2doc(
_temp1263); goto _LL1076; _LL1138: if( _temp1267 == 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1402=( char*)"malloc(sizeof("; struct
_tagged_string _temp1403; _temp1403.curr= _temp1402; _temp1403.base= _temp1402;
_temp1403.last_plus_one= _temp1402 + 15; _temp1403;})), Cyc_Absynpp_typ2doc(
_temp1265), Cyc_PP_text(( struct _tagged_string)({ char* _temp1404=( char*)"))";
struct _tagged_string _temp1405; _temp1405.curr= _temp1404; _temp1405.base=
_temp1404; _temp1405.last_plus_one= _temp1404 + 3; _temp1405;})));} else{ s= Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1406=( char*)"rmalloc(";
struct _tagged_string _temp1407; _temp1407.curr= _temp1406; _temp1407.base=
_temp1406; _temp1407.last_plus_one= _temp1406 + 9; _temp1407;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1408= _temp1267; if(
_temp1408 == 0){ _throw( Null_Exception);} _temp1408;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1409=( char*)","; struct _tagged_string _temp1410;
_temp1410.curr= _temp1409; _temp1410.base= _temp1409; _temp1410.last_plus_one=
_temp1409 + 2; _temp1410;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1411=( char*)"sizeof("; struct _tagged_string _temp1412; _temp1412.curr=
_temp1411; _temp1412.base= _temp1411; _temp1412.last_plus_one= _temp1411 + 8;
_temp1412;})), Cyc_Absynpp_typ2doc( _temp1265), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1413=( char*)"))"; struct _tagged_string _temp1414;
_temp1414.curr= _temp1413; _temp1414.base= _temp1413; _temp1414.last_plus_one=
_temp1413 + 3; _temp1414;})));} goto _LL1076; _LL1140: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1415=( char*)"{"; struct _tagged_string
_temp1416; _temp1416.curr= _temp1415; _temp1416.base= _temp1415; _temp1416.last_plus_one=
_temp1415 + 2; _temp1416;}),( struct _tagged_string)({ char* _temp1417=( char*)"}";
struct _tagged_string _temp1418; _temp1418.curr= _temp1417; _temp1418.base=
_temp1417; _temp1418.last_plus_one= _temp1417 + 2; _temp1418;}),( struct
_tagged_string)({ char* _temp1419=( char*)","; struct _tagged_string _temp1420;
_temp1420.curr= _temp1419; _temp1420.base= _temp1419; _temp1420.last_plus_one=
_temp1419 + 2; _temp1420;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1269)); goto _LL1076; _LL1142: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1421=( char*)"({ "; struct _tagged_string _temp1422;
_temp1422.curr= _temp1421; _temp1422.base= _temp1421; _temp1422.last_plus_one=
_temp1421 + 4; _temp1422;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1273)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1423=( char*)" })"; struct
_tagged_string _temp1424; _temp1424.curr= _temp1423; _temp1424.base= _temp1423;
_temp1424.last_plus_one= _temp1423 + 4; _temp1424;}))); goto _LL1076; _LL1144: s=
Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1425=( char*)"codegen(";
struct _tagged_string _temp1426; _temp1426.curr= _temp1425; _temp1426.base=
_temp1425; _temp1426.last_plus_one= _temp1425 + 9; _temp1426;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1427=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1427->r=( void*)(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp1428=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1428[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1429; _temp1429.tag= Cyc_Absyn_Fn_d; _temp1429.f1= _temp1275; _temp1429;});
_temp1428;})); _temp1427->loc= e->loc; _temp1427;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1430=( char*)")"; struct _tagged_string _temp1431;
_temp1431.curr= _temp1430; _temp1431.base= _temp1430; _temp1431.last_plus_one=
_temp1430 + 2; _temp1431;}))); goto _LL1076; _LL1146: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1432=( char*)"fill("; struct _tagged_string
_temp1433; _temp1433.curr= _temp1432; _temp1433.base= _temp1432; _temp1433.last_plus_one=
_temp1432 + 6; _temp1433;})), Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1277)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1434=( char*)")"; struct
_tagged_string _temp1435; _temp1435.curr= _temp1434; _temp1435.base= _temp1434;
_temp1435.last_plus_one= _temp1434 + 2; _temp1435;}))); goto _LL1076; _LL1076:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp1436=( char*)"("; struct _tagged_string _temp1437; _temp1437.curr=
_temp1436; _temp1437.base= _temp1436; _temp1437.last_plus_one= _temp1436 + 2;
_temp1437;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp1438=( char*)")";
struct _tagged_string _temp1439; _temp1439.curr= _temp1438; _temp1439.base=
_temp1438; _temp1439.last_plus_one= _temp1438 + 2; _temp1439;})));} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1440= d;
struct Cyc_Absyn_Exp* _temp1446; struct _tagged_string* _temp1448; _LL1442: if(*((
int*) _temp1440) == Cyc_Absyn_ArrayElement){ _LL1447: _temp1446=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1440)->f1; goto _LL1443;} else{ goto
_LL1444;} _LL1444: if(*(( int*) _temp1440) == Cyc_Absyn_FieldName){ _LL1449:
_temp1448=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1440)->f1; goto _LL1445;} else{ goto _LL1441;} _LL1443: return Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1450=( char*)".["; struct
_tagged_string _temp1451; _temp1451.curr= _temp1450; _temp1451.base= _temp1450;
_temp1451.last_plus_one= _temp1450 + 3; _temp1451;})), Cyc_Absynpp_exp2doc(
_temp1446), Cyc_PP_text(( struct _tagged_string)({ char* _temp1452=( char*)"]";
struct _tagged_string _temp1453; _temp1453.curr= _temp1452; _temp1453.base=
_temp1452; _temp1453.last_plus_one= _temp1452 + 2; _temp1453;}))); _LL1445:
return Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp1454=(
char*)"."; struct _tagged_string _temp1455; _temp1455.curr= _temp1454; _temp1455.base=
_temp1454; _temp1455.last_plus_one= _temp1454 + 2; _temp1455;})), Cyc_Absynpp_textptr(
_temp1448)); _LL1441:;} struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6*
de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((* de).f2);} else{ return
Cyc_Absynpp_cat2( Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1456=(
char*)""; struct _tagged_string _temp1457; _temp1457.curr= _temp1456; _temp1457.base=
_temp1456; _temp1457.last_plus_one= _temp1456 + 1; _temp1457;}),( struct
_tagged_string)({ char* _temp1458=( char*)"="; struct _tagged_string _temp1459;
_temp1459.curr= _temp1458; _temp1459.base= _temp1458; _temp1459.last_plus_one=
_temp1458 + 2; _temp1459;}),( struct _tagged_string)({ char* _temp1460=( char*)"=";
struct _tagged_string _temp1461; _temp1461.curr= _temp1460; _temp1461.base=
_temp1460; _temp1461.last_plus_one= _temp1460 + 2; _temp1461;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)), Cyc_Absynpp_exp2doc((* de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1462=( char*)""; struct _tagged_string _temp1463;
_temp1463.curr= _temp1462; _temp1463.base= _temp1462; _temp1463.last_plus_one=
_temp1462 + 1; _temp1463;}),( struct _tagged_string)({ char* _temp1464=( char*)"";
struct _tagged_string _temp1465; _temp1465.curr= _temp1464; _temp1465.base=
_temp1464; _temp1465.last_plus_one= _temp1464 + 1; _temp1465;}),( struct
_tagged_string)({ char* _temp1466=( char*)","; struct _tagged_string _temp1467;
_temp1467.curr= _temp1466; _temp1467.base= _temp1466; _temp1467.last_plus_one=
_temp1466 + 2; _temp1467;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1468= c; char _temp1486; void* _temp1488; short _temp1490;
void* _temp1492; int _temp1494; void* _temp1496; int _temp1498; void* _temp1500;
long long _temp1502; void* _temp1504; struct _tagged_string _temp1506; struct
_tagged_string _temp1508; _LL1470: if(( unsigned int) _temp1468 > 1u?*(( int*)
_temp1468) == Cyc_Absyn_Char_c: 0){ _LL1489: _temp1488=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp1468)->f1; goto _LL1487; _LL1487: _temp1486=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp1468)->f2; goto _LL1471;} else{ goto _LL1472;} _LL1472: if(( unsigned int)
_temp1468 > 1u?*(( int*) _temp1468) == Cyc_Absyn_Short_c: 0){ _LL1493: _temp1492=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1468)->f1; goto _LL1491; _LL1491:
_temp1490=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp1468)->f2; goto
_LL1473;} else{ goto _LL1474;} _LL1474: if(( unsigned int) _temp1468 > 1u?*((
int*) _temp1468) == Cyc_Absyn_Int_c: 0){ _LL1497: _temp1496=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1468)->f1; if( _temp1496 ==( void*) Cyc_Absyn_Signed){
goto _LL1495;} else{ goto _LL1476;} _LL1495: _temp1494=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp1468)->f2; goto _LL1475;} else{ goto _LL1476;} _LL1476: if(( unsigned int)
_temp1468 > 1u?*(( int*) _temp1468) == Cyc_Absyn_Int_c: 0){ _LL1501: _temp1500=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1468)->f1; if( _temp1500 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL1499;} else{ goto _LL1478;} _LL1499: _temp1498=(
int)(( struct Cyc_Absyn_Int_c_struct*) _temp1468)->f2; goto _LL1477;} else{ goto
_LL1478;} _LL1478: if(( unsigned int) _temp1468 > 1u?*(( int*) _temp1468) == Cyc_Absyn_LongLong_c:
0){ _LL1505: _temp1504=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1468)->f1;
goto _LL1503; _LL1503: _temp1502=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1468)->f2; goto _LL1479;} else{ goto _LL1480;} _LL1480: if(( unsigned int)
_temp1468 > 1u?*(( int*) _temp1468) == Cyc_Absyn_Float_c: 0){ _LL1507: _temp1506=(
struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*) _temp1468)->f1; goto
_LL1481;} else{ goto _LL1482;} _LL1482: if( _temp1468 ==( void*) Cyc_Absyn_Null_c){
goto _LL1483;} else{ goto _LL1484;} _LL1484: if(( unsigned int) _temp1468 > 1u?*((
int*) _temp1468) == Cyc_Absyn_String_c: 0){ _LL1509: _temp1508=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp1468)->f1; goto
_LL1485;} else{ goto _LL1469;} _LL1471: return Cyc_PP_text(({ struct
_tagged_string _temp1510= Cyc_Absynpp_char_escape( _temp1486); xprintf("'%.*s'",
_temp1510.last_plus_one - _temp1510.curr, _temp1510.curr);})); _LL1473: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1490)); _LL1475: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1494)); _LL1477: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1498)); _LL1479: return Cyc_PP_text(( struct _tagged_string)({
char* _temp1511=( char*)"<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp1512; _temp1512.curr= _temp1511; _temp1512.base= _temp1511; _temp1512.last_plus_one=
_temp1511 + 27; _temp1512;})); _LL1481: return Cyc_PP_text( _temp1506); _LL1483:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp1513=( char*)"null";
struct _tagged_string _temp1514; _temp1514.curr= _temp1513; _temp1514.base=
_temp1513; _temp1514.last_plus_one= _temp1513 + 5; _temp1514;})); _LL1485:
return Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1515=(
char*)"\""; struct _tagged_string _temp1516; _temp1516.curr= _temp1515;
_temp1516.base= _temp1515; _temp1516.last_plus_one= _temp1515 + 2; _temp1516;})),
Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1508)), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1517=( char*)"\""; struct _tagged_string _temp1518;
_temp1518.curr= _temp1517; _temp1518.base= _temp1517; _temp1518.last_plus_one=
_temp1517 + 2; _temp1518;}))); _LL1469:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:({ struct Cyc_List_List*
_temp1519= es; if( _temp1519 == 0){ _throw( Null_Exception);} _temp1519->tl;})
!= 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp1520=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1520[ 0]=({ struct Cyc_Core_Failure_struct _temp1521; _temp1521.tag= Cyc_Core_Failure;
_temp1521.f1=({ struct _tagged_string _temp1522= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1522.last_plus_one
- _temp1522.curr, _temp1522.curr);}); _temp1521;}); _temp1520;}));}{ struct Cyc_PP_Doc*
d= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1525= es; if( _temp1525 == 0){ _throw( Null_Exception);} _temp1525->hd;}));
return Cyc_Absynpp_cat2( d, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1523=( char*)".size"; struct _tagged_string _temp1524; _temp1524.curr=
_temp1523; _temp1524.base= _temp1523; _temp1524.last_plus_one= _temp1523 + 6;
_temp1524;})));}} else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct Cyc_List_List*
ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1526=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1526[ 0]=({ struct Cyc_Core_Failure_struct _temp1527; _temp1527.tag= Cyc_Core_Failure;
_temp1527.f1=({ struct _tagged_string _temp1528= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with no args", _temp1528.last_plus_one -
_temp1528.curr, _temp1528.curr);}); _temp1527;}); _temp1526;}));} else{ if(({
struct Cyc_List_List* _temp1529= ds; if( _temp1529 == 0){ _throw( Null_Exception);}
_temp1529->tl;}) == 0){ return Cyc_Absynpp_cat3( ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1530=( char*)" "; struct _tagged_string _temp1531;
_temp1531.curr= _temp1530; _temp1531.base= _temp1530; _temp1531.last_plus_one=
_temp1530 + 2; _temp1531;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1532= ds; if( _temp1532 == 0){ _throw( Null_Exception);} _temp1532->hd;}));}
else{ if(({ struct Cyc_List_List* _temp1534=({ struct Cyc_List_List* _temp1533=
ds; if( _temp1533 == 0){ _throw( Null_Exception);} _temp1533->tl;}); if(
_temp1534 == 0){ _throw( Null_Exception);} _temp1534->tl;}) != 0){( void) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp1535=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1535[ 0]=({ struct Cyc_Core_Failure_struct
_temp1536; _temp1536.tag= Cyc_Core_Failure; _temp1536.f1=({ struct
_tagged_string _temp1537= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_temp1537.last_plus_one - _temp1537.curr, _temp1537.curr);}); _temp1536;});
_temp1535;}));} else{ return Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1538= ds; if( _temp1538 == 0){ _throw( Null_Exception);} _temp1538->hd;}),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1539=( char*)" "; struct
_tagged_string _temp1540; _temp1540.curr= _temp1539; _temp1540.base= _temp1539;
_temp1540.last_plus_one= _temp1539 + 2; _temp1540;})), ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1541=( char*)" "; struct _tagged_string _temp1542;
_temp1542.curr= _temp1541; _temp1542.base= _temp1541; _temp1542.last_plus_one=
_temp1541 + 2; _temp1542;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1544=({ struct Cyc_List_List* _temp1543= ds; if( _temp1543 == 0){ _throw(
Null_Exception);} _temp1543->tl;}); if( _temp1544 == 0){ _throw( Null_Exception);}
_temp1544->hd;}));}}}} else{ return Cyc_Absynpp_cat4( ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1545=( char*)"("; struct _tagged_string _temp1546;
_temp1546.curr= _temp1545; _temp1546.base= _temp1545; _temp1546.last_plus_one=
_temp1545 + 2; _temp1546;})), Cyc_Absynpp_exps2doc_prec( 20, es), Cyc_PP_text((
struct _tagged_string)({ char* _temp1547=( char*)")"; struct _tagged_string
_temp1548; _temp1548.curr= _temp1547; _temp1548.base= _temp1547; _temp1548.last_plus_one=
_temp1547 + 2; _temp1548;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1549= p; _LL1551: if( _temp1549 ==( void*) Cyc_Absyn_Plus){
goto _LL1552;} else{ goto _LL1553;} _LL1553: if( _temp1549 ==( void*) Cyc_Absyn_Times){
goto _LL1554;} else{ goto _LL1555;} _LL1555: if( _temp1549 ==( void*) Cyc_Absyn_Minus){
goto _LL1556;} else{ goto _LL1557;} _LL1557: if( _temp1549 ==( void*) Cyc_Absyn_Div){
goto _LL1558;} else{ goto _LL1559;} _LL1559: if( _temp1549 ==( void*) Cyc_Absyn_Mod){
goto _LL1560;} else{ goto _LL1561;} _LL1561: if( _temp1549 ==( void*) Cyc_Absyn_Eq){
goto _LL1562;} else{ goto _LL1563;} _LL1563: if( _temp1549 ==( void*) Cyc_Absyn_Neq){
goto _LL1564;} else{ goto _LL1565;} _LL1565: if( _temp1549 ==( void*) Cyc_Absyn_Gt){
goto _LL1566;} else{ goto _LL1567;} _LL1567: if( _temp1549 ==( void*) Cyc_Absyn_Lt){
goto _LL1568;} else{ goto _LL1569;} _LL1569: if( _temp1549 ==( void*) Cyc_Absyn_Gte){
goto _LL1570;} else{ goto _LL1571;} _LL1571: if( _temp1549 ==( void*) Cyc_Absyn_Lte){
goto _LL1572;} else{ goto _LL1573;} _LL1573: if( _temp1549 ==( void*) Cyc_Absyn_Not){
goto _LL1574;} else{ goto _LL1575;} _LL1575: if( _temp1549 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1576;} else{ goto _LL1577;} _LL1577: if( _temp1549 ==( void*) Cyc_Absyn_Bitand){
goto _LL1578;} else{ goto _LL1579;} _LL1579: if( _temp1549 ==( void*) Cyc_Absyn_Bitor){
goto _LL1580;} else{ goto _LL1581;} _LL1581: if( _temp1549 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1582;} else{ goto _LL1583;} _LL1583: if( _temp1549 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1584;} else{ goto _LL1585;} _LL1585: if( _temp1549 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1586;} else{ goto _LL1587;} _LL1587: if( _temp1549 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1588;} else{ goto _LL1589;} _LL1589: if( _temp1549 ==( void*) Cyc_Absyn_Size){
goto _LL1590;} else{ goto _LL1591;} _LL1591: if( _temp1549 ==( void*) Cyc_Absyn_Printf){
goto _LL1592;} else{ goto _LL1593;} _LL1593: if( _temp1549 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1594;} else{ goto _LL1595;} _LL1595: if( _temp1549 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1596;} else{ goto _LL1597;} _LL1597: if( _temp1549 ==( void*) Cyc_Absyn_Scanf){
goto _LL1598;} else{ goto _LL1599;} _LL1599: if( _temp1549 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1600;} else{ goto _LL1601;} _LL1601: if( _temp1549 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1602;} else{ goto _LL1550;} _LL1552: return( struct _tagged_string)({
char* _temp1603=( char*)"+"; struct _tagged_string _temp1604; _temp1604.curr=
_temp1603; _temp1604.base= _temp1603; _temp1604.last_plus_one= _temp1603 + 2;
_temp1604;}); _LL1554: return( struct _tagged_string)({ char* _temp1605=( char*)"*";
struct _tagged_string _temp1606; _temp1606.curr= _temp1605; _temp1606.base=
_temp1605; _temp1606.last_plus_one= _temp1605 + 2; _temp1606;}); _LL1556: return(
struct _tagged_string)({ char* _temp1607=( char*)"-"; struct _tagged_string
_temp1608; _temp1608.curr= _temp1607; _temp1608.base= _temp1607; _temp1608.last_plus_one=
_temp1607 + 2; _temp1608;}); _LL1558: return( struct _tagged_string)({ char*
_temp1609=( char*)"/"; struct _tagged_string _temp1610; _temp1610.curr=
_temp1609; _temp1610.base= _temp1609; _temp1610.last_plus_one= _temp1609 + 2;
_temp1610;}); _LL1560: return( struct _tagged_string)({ char* _temp1611=( char*)"%";
struct _tagged_string _temp1612; _temp1612.curr= _temp1611; _temp1612.base=
_temp1611; _temp1612.last_plus_one= _temp1611 + 2; _temp1612;}); _LL1562: return(
struct _tagged_string)({ char* _temp1613=( char*)"=="; struct _tagged_string
_temp1614; _temp1614.curr= _temp1613; _temp1614.base= _temp1613; _temp1614.last_plus_one=
_temp1613 + 3; _temp1614;}); _LL1564: return( struct _tagged_string)({ char*
_temp1615=( char*)"!="; struct _tagged_string _temp1616; _temp1616.curr=
_temp1615; _temp1616.base= _temp1615; _temp1616.last_plus_one= _temp1615 + 3;
_temp1616;}); _LL1566: return( struct _tagged_string)({ char* _temp1617=( char*)">";
struct _tagged_string _temp1618; _temp1618.curr= _temp1617; _temp1618.base=
_temp1617; _temp1618.last_plus_one= _temp1617 + 2; _temp1618;}); _LL1568: return(
struct _tagged_string)({ char* _temp1619=( char*)"<"; struct _tagged_string
_temp1620; _temp1620.curr= _temp1619; _temp1620.base= _temp1619; _temp1620.last_plus_one=
_temp1619 + 2; _temp1620;}); _LL1570: return( struct _tagged_string)({ char*
_temp1621=( char*)">="; struct _tagged_string _temp1622; _temp1622.curr=
_temp1621; _temp1622.base= _temp1621; _temp1622.last_plus_one= _temp1621 + 3;
_temp1622;}); _LL1572: return( struct _tagged_string)({ char* _temp1623=( char*)"<=";
struct _tagged_string _temp1624; _temp1624.curr= _temp1623; _temp1624.base=
_temp1623; _temp1624.last_plus_one= _temp1623 + 3; _temp1624;}); _LL1574: return(
struct _tagged_string)({ char* _temp1625=( char*)"!"; struct _tagged_string
_temp1626; _temp1626.curr= _temp1625; _temp1626.base= _temp1625; _temp1626.last_plus_one=
_temp1625 + 2; _temp1626;}); _LL1576: return( struct _tagged_string)({ char*
_temp1627=( char*)"~"; struct _tagged_string _temp1628; _temp1628.curr=
_temp1627; _temp1628.base= _temp1627; _temp1628.last_plus_one= _temp1627 + 2;
_temp1628;}); _LL1578: return( struct _tagged_string)({ char* _temp1629=( char*)"&";
struct _tagged_string _temp1630; _temp1630.curr= _temp1629; _temp1630.base=
_temp1629; _temp1630.last_plus_one= _temp1629 + 2; _temp1630;}); _LL1580: return(
struct _tagged_string)({ char* _temp1631=( char*)"|"; struct _tagged_string
_temp1632; _temp1632.curr= _temp1631; _temp1632.base= _temp1631; _temp1632.last_plus_one=
_temp1631 + 2; _temp1632;}); _LL1582: return( struct _tagged_string)({ char*
_temp1633=( char*)"^"; struct _tagged_string _temp1634; _temp1634.curr=
_temp1633; _temp1634.base= _temp1633; _temp1634.last_plus_one= _temp1633 + 2;
_temp1634;}); _LL1584: return( struct _tagged_string)({ char* _temp1635=( char*)"<<";
struct _tagged_string _temp1636; _temp1636.curr= _temp1635; _temp1636.base=
_temp1635; _temp1636.last_plus_one= _temp1635 + 3; _temp1636;}); _LL1586: return(
struct _tagged_string)({ char* _temp1637=( char*)">>"; struct _tagged_string
_temp1638; _temp1638.curr= _temp1637; _temp1638.base= _temp1637; _temp1638.last_plus_one=
_temp1637 + 3; _temp1638;}); _LL1588: return( struct _tagged_string)({ char*
_temp1639=( char*)">>>"; struct _tagged_string _temp1640; _temp1640.curr=
_temp1639; _temp1640.base= _temp1639; _temp1640.last_plus_one= _temp1639 + 4;
_temp1640;}); _LL1590: return( struct _tagged_string)({ char* _temp1641=( char*)"size";
struct _tagged_string _temp1642; _temp1642.curr= _temp1641; _temp1642.base=
_temp1641; _temp1642.last_plus_one= _temp1641 + 5; _temp1642;}); _LL1592: return(
struct _tagged_string)({ char* _temp1643=( char*)"printf"; struct _tagged_string
_temp1644; _temp1644.curr= _temp1643; _temp1644.base= _temp1643; _temp1644.last_plus_one=
_temp1643 + 7; _temp1644;}); _LL1594: return( struct _tagged_string)({ char*
_temp1645=( char*)"fprintf"; struct _tagged_string _temp1646; _temp1646.curr=
_temp1645; _temp1646.base= _temp1645; _temp1646.last_plus_one= _temp1645 + 8;
_temp1646;}); _LL1596: return( struct _tagged_string)({ char* _temp1647=( char*)"xprintf";
struct _tagged_string _temp1648; _temp1648.curr= _temp1647; _temp1648.base=
_temp1647; _temp1648.last_plus_one= _temp1647 + 8; _temp1648;}); _LL1598: return(
struct _tagged_string)({ char* _temp1649=( char*)"scanf"; struct _tagged_string
_temp1650; _temp1650.curr= _temp1649; _temp1650.base= _temp1649; _temp1650.last_plus_one=
_temp1649 + 6; _temp1650;}); _LL1600: return( struct _tagged_string)({ char*
_temp1651=( char*)"fscanf"; struct _tagged_string _temp1652; _temp1652.curr=
_temp1651; _temp1652.base= _temp1651; _temp1652.last_plus_one= _temp1651 + 7;
_temp1652;}); _LL1602: return( struct _tagged_string)({ char* _temp1653=( char*)"sscanf";
struct _tagged_string _temp1654; _temp1654.curr= _temp1653; _temp1654.base=
_temp1653; _temp1654.last_plus_one= _temp1653 + 7; _temp1654;}); _LL1550:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 ==
0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp1655=(* t).f1; if( _temp1655
== 0){ _throw( Null_Exception);} _temp1655->v;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1656=( char*)" = "; struct _tagged_string _temp1657;
_temp1657.curr= _temp1656; _temp1657.base= _temp1656; _temp1657.last_plus_one=
_temp1656 + 4; _temp1657;})), Cyc_Absynpp_exp2doc((* t).f2));}} struct Cyc_PP_Doc*
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
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1658=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1664; struct Cyc_Absyn_Decl* _temp1666;
_LL1660: if(( unsigned int) _temp1658 > 1u?*(( int*) _temp1658) == Cyc_Absyn_Decl_s:
0){ _LL1667: _temp1666=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1658)->f1; goto _LL1665; _LL1665: _temp1664=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1658)->f2; goto _LL1661;} else{ goto
_LL1662;} _LL1662: goto _LL1663; _LL1661: return 1; _LL1663: return 0; _LL1659:;}
struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc*
s;{ void* _temp1668=( void*) st->r; struct Cyc_Absyn_Exp* _temp1710; struct Cyc_Absyn_Stmt*
_temp1712; struct Cyc_Absyn_Stmt* _temp1714; struct Cyc_Absyn_Exp* _temp1716;
struct Cyc_Absyn_Stmt* _temp1718; struct Cyc_Absyn_Stmt* _temp1720; struct Cyc_Absyn_Exp*
_temp1722; struct Cyc_Absyn_Stmt* _temp1724; struct _tuple2 _temp1726; struct
Cyc_Absyn_Stmt* _temp1728; struct Cyc_Absyn_Exp* _temp1730; struct Cyc_Absyn_Stmt*
_temp1732; struct Cyc_Absyn_Stmt* _temp1734; struct Cyc_Absyn_Stmt* _temp1736;
struct _tagged_string* _temp1738; struct Cyc_Absyn_Stmt* _temp1740; struct
_tuple2 _temp1742; struct Cyc_Absyn_Stmt* _temp1744; struct Cyc_Absyn_Exp*
_temp1746; struct _tuple2 _temp1748; struct Cyc_Absyn_Stmt* _temp1750; struct
Cyc_Absyn_Exp* _temp1752; struct Cyc_Absyn_Exp* _temp1754; struct Cyc_List_List*
_temp1756; struct Cyc_Absyn_Exp* _temp1758; struct Cyc_Absyn_Switch_clause**
_temp1760; struct Cyc_List_List* _temp1762; struct Cyc_Absyn_Switch_clause**
_temp1764; struct Cyc_List_List* _temp1766; struct Cyc_Absyn_Stmt* _temp1768;
struct Cyc_Absyn_Decl* _temp1770; struct Cyc_Absyn_Stmt* _temp1772; struct Cyc_Absyn_Stmt*
_temp1774; struct Cyc_Absyn_Stmt* _temp1776; struct _tagged_string* _temp1778;
struct _tuple2 _temp1780; struct Cyc_Absyn_Stmt* _temp1782; struct Cyc_Absyn_Exp*
_temp1784; struct Cyc_Absyn_Stmt* _temp1786; struct Cyc_List_List* _temp1788;
struct Cyc_Absyn_Stmt* _temp1790; struct Cyc_Absyn_Stmt* _temp1792; struct Cyc_Absyn_Vardecl*
_temp1794; struct Cyc_Absyn_Tvar* _temp1796; _LL1670: if( _temp1668 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1671;} else{ goto _LL1672;} _LL1672: if((
unsigned int) _temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_Exp_s: 0){
_LL1711: _temp1710=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1668)->f1; goto _LL1673;} else{ goto _LL1674;} _LL1674: if(( unsigned int)
_temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_Seq_s: 0){ _LL1715: _temp1714=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1668)->f1; goto
_LL1713; _LL1713: _temp1712=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1668)->f2; goto _LL1675;} else{ goto _LL1676;} _LL1676: if(( unsigned int)
_temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_Return_s: 0){ _LL1717:
_temp1716=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1668)->f1; goto _LL1677;} else{ goto _LL1678;} _LL1678: if(( unsigned int)
_temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_IfThenElse_s: 0){ _LL1723:
_temp1722=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1668)->f1; goto _LL1721; _LL1721: _temp1720=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1668)->f2; goto _LL1719; _LL1719:
_temp1718=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1668)->f3; goto _LL1679;} else{ goto _LL1680;} _LL1680: if(( unsigned int)
_temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_While_s: 0){ _LL1727: _temp1726=(
struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp1668)->f1; _LL1731:
_temp1730= _temp1726.f1; goto _LL1729; _LL1729: _temp1728= _temp1726.f2; goto
_LL1725; _LL1725: _temp1724=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1668)->f2; goto _LL1681;} else{ goto _LL1682;} _LL1682: if(( unsigned int)
_temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_Break_s: 0){ _LL1733: _temp1732=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp1668)->f1; goto
_LL1683;} else{ goto _LL1684;} _LL1684: if(( unsigned int) _temp1668 > 1u?*((
int*) _temp1668) == Cyc_Absyn_Continue_s: 0){ _LL1735: _temp1734=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1668)->f1; goto _LL1685;} else{ goto
_LL1686;} _LL1686: if(( unsigned int) _temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_Goto_s:
0){ _LL1739: _temp1738=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1668)->f1; goto _LL1737; _LL1737: _temp1736=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp1668)->f2; goto _LL1687;} else{ goto
_LL1688;} _LL1688: if(( unsigned int) _temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_For_s:
0){ _LL1755: _temp1754=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1668)->f1; goto _LL1749; _LL1749: _temp1748=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp1668)->f2; _LL1753: _temp1752= _temp1748.f1; goto _LL1751; _LL1751:
_temp1750= _temp1748.f2; goto _LL1743; _LL1743: _temp1742=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp1668)->f3; _LL1747: _temp1746= _temp1742.f1;
goto _LL1745; _LL1745: _temp1744= _temp1742.f2; goto _LL1741; _LL1741: _temp1740=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1668)->f4; goto
_LL1689;} else{ goto _LL1690;} _LL1690: if(( unsigned int) _temp1668 > 1u?*((
int*) _temp1668) == Cyc_Absyn_Switch_s: 0){ _LL1759: _temp1758=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1668)->f1; goto _LL1757; _LL1757:
_temp1756=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1668)->f2; goto _LL1691;} else{ goto _LL1692;} _LL1692: if(( unsigned int)
_temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_Fallthru_s: 0){ _LL1763:
_temp1762=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1668)->f1; if( _temp1762 == 0){ goto _LL1761;} else{ goto _LL1694;} _LL1761:
_temp1760=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1668)->f2; goto _LL1693;} else{ goto _LL1694;} _LL1694: if(( unsigned int)
_temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_Fallthru_s: 0){ _LL1767:
_temp1766=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1668)->f1; goto _LL1765; _LL1765: _temp1764=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1668)->f2; goto _LL1695;} else{ goto
_LL1696;} _LL1696: if(( unsigned int) _temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_Decl_s:
0){ _LL1771: _temp1770=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1668)->f1; goto _LL1769; _LL1769: _temp1768=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1668)->f2; goto _LL1697;} else{ goto
_LL1698;} _LL1698: if(( unsigned int) _temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_Cut_s:
0){ _LL1773: _temp1772=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp1668)->f1; goto _LL1699;} else{ goto _LL1700;} _LL1700: if(( unsigned int)
_temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_Splice_s: 0){ _LL1775:
_temp1774=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp1668)->f1; goto _LL1701;} else{ goto _LL1702;} _LL1702: if(( unsigned int)
_temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_Label_s: 0){ _LL1779: _temp1778=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1668)->f1; goto
_LL1777; _LL1777: _temp1776=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1668)->f2; goto _LL1703;} else{ goto _LL1704;} _LL1704: if(( unsigned int)
_temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_Do_s: 0){ _LL1787: _temp1786=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp1668)->f1; goto
_LL1781; _LL1781: _temp1780=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp1668)->f2; _LL1785: _temp1784= _temp1780.f1; goto _LL1783; _LL1783:
_temp1782= _temp1780.f2; goto _LL1705;} else{ goto _LL1706;} _LL1706: if((
unsigned int) _temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_TryCatch_s: 0){
_LL1791: _temp1790=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1668)->f1; goto _LL1789; _LL1789: _temp1788=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1668)->f2; goto _LL1707;} else{ goto
_LL1708;} _LL1708: if(( unsigned int) _temp1668 > 1u?*(( int*) _temp1668) == Cyc_Absyn_Region_s:
0){ _LL1797: _temp1796=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1668)->f1; goto _LL1795; _LL1795: _temp1794=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1668)->f2; goto _LL1793; _LL1793:
_temp1792=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1668)->f3; goto _LL1709;} else{ goto _LL1669;} _LL1671: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp1798=( char*)";"; struct _tagged_string
_temp1799; _temp1799.curr= _temp1798; _temp1799.base= _temp1798; _temp1799.last_plus_one=
_temp1798 + 2; _temp1799;})); goto _LL1669; _LL1673: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc(
_temp1710), Cyc_PP_text(( struct _tagged_string)({ char* _temp1800=( char*)";";
struct _tagged_string _temp1801; _temp1801.curr= _temp1800; _temp1801.base=
_temp1800; _temp1801.last_plus_one= _temp1800 + 2; _temp1801;}))); goto _LL1669;
_LL1675: if( Cyc_Absynpp_decls_first){ if( Cyc_Absynpp_is_declaration( _temp1714)){
s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({ char* _temp1802=(
char*)"{ "; struct _tagged_string _temp1803; _temp1803.curr= _temp1802;
_temp1803.base= _temp1802; _temp1803.last_plus_one= _temp1802 + 3; _temp1803;})),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1714)), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1804=( char*)"}"; struct _tagged_string
_temp1805; _temp1805.curr= _temp1804; _temp1805.base= _temp1804; _temp1805.last_plus_one=
_temp1804 + 2; _temp1805;})), Cyc_PP_line_doc(), Cyc_Absynpp_is_declaration(
_temp1712)? Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1806=( char*)"{ "; struct _tagged_string _temp1807; _temp1807.curr=
_temp1806; _temp1807.base= _temp1806; _temp1807.last_plus_one= _temp1806 + 3;
_temp1807;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1712)), Cyc_PP_text((
struct _tagged_string)({ char* _temp1808=( char*)"}"; struct _tagged_string
_temp1809; _temp1809.curr= _temp1808; _temp1809.base= _temp1808; _temp1809.last_plus_one=
_temp1808 + 2; _temp1809;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1712));} else{ if( Cyc_Absynpp_is_declaration( _temp1712)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1714), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1810=( char*)"{ "; struct _tagged_string _temp1811;
_temp1811.curr= _temp1810; _temp1811.base= _temp1810; _temp1811.last_plus_one=
_temp1810 + 3; _temp1811;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1712)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1812=( char*)"}"; struct
_tagged_string _temp1813; _temp1813.curr= _temp1812; _temp1813.base= _temp1812;
_temp1813.last_plus_one= _temp1812 + 2; _temp1813;})), Cyc_PP_line_doc());}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,(
struct _tagged_string)({ char* _temp1814=( char*)""; struct _tagged_string
_temp1815; _temp1815.curr= _temp1814; _temp1815.base= _temp1814; _temp1815.last_plus_one=
_temp1814 + 1; _temp1815;}),({ struct Cyc_List_List* _temp1816=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1816->hd=( void*) _temp1714;
_temp1816->tl=({ struct Cyc_List_List* _temp1817=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1817->hd=( void*) _temp1712;
_temp1817->tl= 0; _temp1817;}); _temp1816;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,( struct
_tagged_string)({ char* _temp1818=( char*)""; struct _tagged_string _temp1819;
_temp1819.curr= _temp1818; _temp1819.base= _temp1818; _temp1819.last_plus_one=
_temp1818 + 1; _temp1819;}),({ struct Cyc_List_List* _temp1820=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1820->hd=( void*) _temp1714;
_temp1820->tl=({ struct Cyc_List_List* _temp1821=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1821->hd=( void*) _temp1712;
_temp1821->tl= 0; _temp1821;}); _temp1820;}));} goto _LL1669; _LL1677: if(
_temp1716 == 0){ s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1822=(
char*)"return;"; struct _tagged_string _temp1823; _temp1823.curr= _temp1822;
_temp1823.base= _temp1822; _temp1823.last_plus_one= _temp1822 + 8; _temp1823;}));}
else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1824=( char*)"return "; struct _tagged_string _temp1825; _temp1825.curr=
_temp1824; _temp1825.base= _temp1824; _temp1825.last_plus_one= _temp1824 + 8;
_temp1825;})), _temp1716 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1826= _temp1716; if( _temp1826 == 0){
_throw( Null_Exception);} _temp1826;})), Cyc_PP_text(( struct _tagged_string)({
char* _temp1827=( char*)";"; struct _tagged_string _temp1828; _temp1828.curr=
_temp1827; _temp1828.base= _temp1827; _temp1828.last_plus_one= _temp1827 + 2;
_temp1828;})));} goto _LL1669; _LL1679: { int print_else;{ void* _temp1829=(
void*) _temp1718->r; _LL1831: if( _temp1829 ==( void*) Cyc_Absyn_Skip_s){ goto
_LL1832;} else{ goto _LL1833;} _LL1833: goto _LL1834; _LL1832: print_else= 0;
goto _LL1830; _LL1834: print_else= 1; goto _LL1830; _LL1830:;} s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1835=( char*)"if ("; struct
_tagged_string _temp1836; _temp1836.curr= _temp1835; _temp1836.base= _temp1835;
_temp1836.last_plus_one= _temp1835 + 5; _temp1836;})), Cyc_Absynpp_exp2doc(
_temp1722), Cyc_PP_text(( struct _tagged_string)({ char* _temp1837=( char*)") {";
struct _tagged_string _temp1838; _temp1838.curr= _temp1837; _temp1838.base=
_temp1837; _temp1838.last_plus_one= _temp1837 + 4; _temp1838;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1720))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1839=( char*)"}"; struct
_tagged_string _temp1840; _temp1840.curr= _temp1839; _temp1840.base= _temp1839;
_temp1840.last_plus_one= _temp1839 + 2; _temp1840;})), print_else? Cyc_Absynpp_cat5(
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1841=( char*)"else {";
struct _tagged_string _temp1842; _temp1842.curr= _temp1841; _temp1842.base=
_temp1841; _temp1842.last_plus_one= _temp1841 + 7; _temp1842;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1718))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1843=( char*)"}"; struct
_tagged_string _temp1844; _temp1844.curr= _temp1843; _temp1844.base= _temp1843;
_temp1844.last_plus_one= _temp1843 + 2; _temp1844;}))): Cyc_PP_nil_doc()); goto
_LL1669;} _LL1681: s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({
char* _temp1845=( char*)"while ("; struct _tagged_string _temp1846; _temp1846.curr=
_temp1845; _temp1846.base= _temp1845; _temp1846.last_plus_one= _temp1845 + 8;
_temp1846;})), Cyc_Absynpp_exp2doc( _temp1730), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1847=( char*)") {"; struct _tagged_string _temp1848;
_temp1848.curr= _temp1847; _temp1848.base= _temp1847; _temp1848.last_plus_one=
_temp1847 + 4; _temp1848;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1724))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1849=( char*)"}"; struct _tagged_string _temp1850;
_temp1850.curr= _temp1849; _temp1850.base= _temp1849; _temp1850.last_plus_one=
_temp1849 + 2; _temp1850;}))); goto _LL1669; _LL1683: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1851=( char*)"break;"; struct _tagged_string
_temp1852; _temp1852.curr= _temp1851; _temp1852.base= _temp1851; _temp1852.last_plus_one=
_temp1851 + 7; _temp1852;})); goto _LL1669; _LL1685: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1853=( char*)"continue;"; struct _tagged_string
_temp1854; _temp1854.curr= _temp1853; _temp1854.base= _temp1853; _temp1854.last_plus_one=
_temp1853 + 10; _temp1854;})); goto _LL1669; _LL1687: s= Cyc_PP_text(({ struct
_tagged_string _temp1855=* _temp1738; xprintf("goto %.*s;", _temp1855.last_plus_one
- _temp1855.curr, _temp1855.curr);})); goto _LL1669; _LL1689: s= Cyc_Absynpp_cat10(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1856=( char*)"for("; struct
_tagged_string _temp1857; _temp1857.curr= _temp1856; _temp1857.base= _temp1856;
_temp1857.last_plus_one= _temp1856 + 5; _temp1857;})), Cyc_Absynpp_exp2doc(
_temp1754), Cyc_PP_text(( struct _tagged_string)({ char* _temp1858=( char*)"; ";
struct _tagged_string _temp1859; _temp1859.curr= _temp1858; _temp1859.base=
_temp1858; _temp1859.last_plus_one= _temp1858 + 3; _temp1859;})), Cyc_Absynpp_exp2doc(
_temp1752), Cyc_PP_text(( struct _tagged_string)({ char* _temp1860=( char*)"; ";
struct _tagged_string _temp1861; _temp1861.curr= _temp1860; _temp1861.base=
_temp1860; _temp1861.last_plus_one= _temp1860 + 3; _temp1861;})), Cyc_Absynpp_exp2doc(
_temp1746), Cyc_PP_text(( struct _tagged_string)({ char* _temp1862=( char*)") {";
struct _tagged_string _temp1863; _temp1863.curr= _temp1862; _temp1863.base=
_temp1862; _temp1863.last_plus_one= _temp1862 + 4; _temp1863;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1740))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1864=( char*)"}"; struct
_tagged_string _temp1865; _temp1865.curr= _temp1864; _temp1865.base= _temp1864;
_temp1865.last_plus_one= _temp1864 + 2; _temp1865;}))); goto _LL1669; _LL1691: s=
Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char* _temp1866=( char*)"switch (";
struct _tagged_string _temp1867; _temp1867.curr= _temp1866; _temp1867.base=
_temp1866; _temp1867.last_plus_one= _temp1866 + 9; _temp1867;})), Cyc_Absynpp_exp2doc(
_temp1758), Cyc_PP_text(( struct _tagged_string)({ char* _temp1868=( char*)") {";
struct _tagged_string _temp1869; _temp1869.curr= _temp1868; _temp1869.base=
_temp1868; _temp1869.last_plus_one= _temp1868 + 4; _temp1869;})), Cyc_PP_line_doc(),
Cyc_Absynpp_switchclauses2doc( _temp1756), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1870=( char*)"}"; struct _tagged_string
_temp1871; _temp1871.curr= _temp1870; _temp1871.base= _temp1870; _temp1871.last_plus_one=
_temp1870 + 2; _temp1871;}))); goto _LL1669; _LL1693: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1872=( char*)"fallthru;"; struct _tagged_string
_temp1873; _temp1873.curr= _temp1872; _temp1873.base= _temp1872; _temp1873.last_plus_one=
_temp1872 + 10; _temp1873;})); goto _LL1669; _LL1695: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1874=( char*)"fallthru("; struct
_tagged_string _temp1875; _temp1875.curr= _temp1874; _temp1875.base= _temp1874;
_temp1875.last_plus_one= _temp1874 + 10; _temp1875;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1766), Cyc_PP_text(( struct _tagged_string)({ char* _temp1876=( char*)");";
struct _tagged_string _temp1877; _temp1877.curr= _temp1876; _temp1877.base=
_temp1876; _temp1877.last_plus_one= _temp1876 + 3; _temp1877;}))); goto _LL1669;
_LL1697: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1770), Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1768)); goto _LL1669; _LL1699: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1878=( char*)"cut "; struct
_tagged_string _temp1879; _temp1879.curr= _temp1878; _temp1879.base= _temp1878;
_temp1879.last_plus_one= _temp1878 + 5; _temp1879;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1772))); goto _LL1669; _LL1701: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1880=( char*)"splice "; struct _tagged_string
_temp1881; _temp1881.curr= _temp1880; _temp1881.base= _temp1880; _temp1881.last_plus_one=
_temp1880 + 8; _temp1881;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1774)));
goto _LL1669; _LL1703: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1776): 0){ s= Cyc_Absynpp_cat6( Cyc_Absynpp_textptr( _temp1778), Cyc_PP_text((
struct _tagged_string)({ char* _temp1882=( char*)": {"; struct _tagged_string
_temp1883; _temp1883.curr= _temp1882; _temp1883.base= _temp1882; _temp1883.last_plus_one=
_temp1882 + 4; _temp1883;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1776)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1884=( char*)"}"; struct _tagged_string _temp1885; _temp1885.curr=
_temp1884; _temp1885.base= _temp1884; _temp1885.last_plus_one= _temp1884 + 2;
_temp1885;})));} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( _temp1778), Cyc_PP_text((
struct _tagged_string)({ char* _temp1886=( char*)": "; struct _tagged_string
_temp1887; _temp1887.curr= _temp1886; _temp1887.base= _temp1886; _temp1887.last_plus_one=
_temp1886 + 3; _temp1887;})), Cyc_Absynpp_stmt2doc( _temp1776));} goto _LL1669;
_LL1705: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1888=( char*)"do {"; struct _tagged_string _temp1889; _temp1889.curr=
_temp1888; _temp1889.base= _temp1888; _temp1889.last_plus_one= _temp1888 + 5;
_temp1889;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1786)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1890=( char*)"} while ("; struct _tagged_string _temp1891; _temp1891.curr=
_temp1890; _temp1891.base= _temp1890; _temp1891.last_plus_one= _temp1890 + 10;
_temp1891;})), Cyc_Absynpp_exp2doc( _temp1784), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1892=( char*)");"; struct _tagged_string _temp1893;
_temp1893.curr= _temp1892; _temp1893.base= _temp1892; _temp1893.last_plus_one=
_temp1892 + 3; _temp1893;}))); goto _LL1669; _LL1707: s= Cyc_Absynpp_cat9( Cyc_PP_text((
struct _tagged_string)({ char* _temp1894=( char*)"try {"; struct _tagged_string
_temp1895; _temp1895.curr= _temp1894; _temp1895.base= _temp1894; _temp1895.last_plus_one=
_temp1894 + 6; _temp1895;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1790)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1896=( char*)"} catch {"; struct _tagged_string _temp1897; _temp1897.curr=
_temp1896; _temp1897.base= _temp1896; _temp1897.last_plus_one= _temp1896 + 10;
_temp1897;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1788)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1898=( char*)"}"; struct _tagged_string _temp1899; _temp1899.curr=
_temp1898; _temp1899.base= _temp1898; _temp1899.last_plus_one= _temp1898 + 2;
_temp1899;}))); goto _LL1669; _LL1709: s= Cyc_Absynpp_cat9( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1900=( char*)"region<"; struct _tagged_string
_temp1901; _temp1901.curr= _temp1900; _temp1901.base= _temp1900; _temp1901.last_plus_one=
_temp1900 + 8; _temp1901;})), Cyc_Absynpp_textptr( Cyc_Absynpp_get_name(
_temp1796)), Cyc_PP_text(( struct _tagged_string)({ char* _temp1902=( char*)">";
struct _tagged_string _temp1903; _temp1903.curr= _temp1902; _temp1903.base=
_temp1902; _temp1903.last_plus_one= _temp1902 + 2; _temp1903;})), Cyc_Absynpp_qvar2doc(
_temp1794->name), Cyc_PP_text(( struct _tagged_string)({ char* _temp1904=( char*)"{";
struct _tagged_string _temp1905; _temp1905.curr= _temp1904; _temp1905.base=
_temp1904; _temp1905.last_plus_one= _temp1904 + 2; _temp1905;})), Cyc_PP_line_doc(),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1792)), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1906=( char*)"}"; struct _tagged_string
_temp1907; _temp1907.curr= _temp1906; _temp1907.base= _temp1906; _temp1907.last_plus_one=
_temp1906 + 2; _temp1907;}))); goto _LL1669; _LL1669:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_pat2doc( struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void*
_temp1908=( void*) p->r; int _temp1940; void* _temp1942; char _temp1944; struct
_tagged_string _temp1946; struct Cyc_Absyn_Vardecl* _temp1948; struct Cyc_List_List*
_temp1950; struct Cyc_Absyn_Pat* _temp1952; struct Cyc_Absyn_Vardecl* _temp1954;
struct _tuple0* _temp1956; struct Cyc_List_List* _temp1958; struct Cyc_List_List*
_temp1960; struct _tuple0* _temp1962; struct Cyc_List_List* _temp1964; struct
Cyc_List_List* _temp1966; struct _tuple0* _temp1968; struct Cyc_List_List*
_temp1970; struct Cyc_List_List* _temp1972; struct Cyc_Core_Opt* _temp1974;
struct Cyc_Absyn_Structdecl* _temp1976; struct Cyc_Absyn_Enumfield* _temp1978;
struct Cyc_Absyn_Enumdecl* _temp1980; struct Cyc_List_List* _temp1982; struct
Cyc_List_List* _temp1984; struct Cyc_Absyn_Tunionfield* _temp1986; struct Cyc_Absyn_Tuniondecl*
_temp1988; _LL1910: if( _temp1908 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1911;}
else{ goto _LL1912;} _LL1912: if( _temp1908 ==( void*) Cyc_Absyn_Null_p){ goto
_LL1913;} else{ goto _LL1914;} _LL1914: if(( unsigned int) _temp1908 > 2u?*((
int*) _temp1908) == Cyc_Absyn_Int_p: 0){ _LL1943: _temp1942=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1908)->f1; goto _LL1941; _LL1941: _temp1940=( int)((
struct Cyc_Absyn_Int_p_struct*) _temp1908)->f2; goto _LL1915;} else{ goto
_LL1916;} _LL1916: if(( unsigned int) _temp1908 > 2u?*(( int*) _temp1908) == Cyc_Absyn_Char_p:
0){ _LL1945: _temp1944=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1908)->f1;
goto _LL1917;} else{ goto _LL1918;} _LL1918: if(( unsigned int) _temp1908 > 2u?*((
int*) _temp1908) == Cyc_Absyn_Float_p: 0){ _LL1947: _temp1946=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1908)->f1; goto _LL1919;}
else{ goto _LL1920;} _LL1920: if(( unsigned int) _temp1908 > 2u?*(( int*)
_temp1908) == Cyc_Absyn_Var_p: 0){ _LL1949: _temp1948=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1908)->f1; goto _LL1921;} else{ goto
_LL1922;} _LL1922: if(( unsigned int) _temp1908 > 2u?*(( int*) _temp1908) == Cyc_Absyn_Tuple_p:
0){ _LL1951: _temp1950=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1908)->f1; goto _LL1923;} else{ goto _LL1924;} _LL1924: if(( unsigned int)
_temp1908 > 2u?*(( int*) _temp1908) == Cyc_Absyn_Pointer_p: 0){ _LL1953:
_temp1952=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1908)->f1; goto _LL1925;} else{ goto _LL1926;} _LL1926: if(( unsigned int)
_temp1908 > 2u?*(( int*) _temp1908) == Cyc_Absyn_Reference_p: 0){ _LL1955:
_temp1954=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1908)->f1; goto _LL1927;} else{ goto _LL1928;} _LL1928: if(( unsigned int)
_temp1908 > 2u?*(( int*) _temp1908) == Cyc_Absyn_UnknownId_p: 0){ _LL1957:
_temp1956=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1908)->f1;
goto _LL1929;} else{ goto _LL1930;} _LL1930: if(( unsigned int) _temp1908 > 2u?*((
int*) _temp1908) == Cyc_Absyn_UnknownCall_p: 0){ _LL1963: _temp1962=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1908)->f1; goto _LL1961;
_LL1961: _temp1960=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1908)->f2; goto _LL1959; _LL1959: _temp1958=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1908)->f3; goto _LL1931;} else{
goto _LL1932;} _LL1932: if(( unsigned int) _temp1908 > 2u?*(( int*) _temp1908)
== Cyc_Absyn_UnknownFields_p: 0){ _LL1969: _temp1968=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp1908)->f1; goto _LL1967; _LL1967:
_temp1966=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1908)->f2; goto _LL1965; _LL1965: _temp1964=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1908)->f3; goto _LL1933;} else{
goto _LL1934;} _LL1934: if(( unsigned int) _temp1908 > 2u?*(( int*) _temp1908)
== Cyc_Absyn_Struct_p: 0){ _LL1977: _temp1976=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1908)->f1; goto _LL1975; _LL1975:
_temp1974=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1908)->f2;
goto _LL1973; _LL1973: _temp1972=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1908)->f3; goto _LL1971; _LL1971: _temp1970=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1908)->f4; goto _LL1935;} else{ goto
_LL1936;} _LL1936: if(( unsigned int) _temp1908 > 2u?*(( int*) _temp1908) == Cyc_Absyn_Enum_p:
0){ _LL1981: _temp1980=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1908)->f1; goto _LL1979; _LL1979: _temp1978=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1908)->f2; goto _LL1937;} else{ goto
_LL1938;} _LL1938: if(( unsigned int) _temp1908 > 2u?*(( int*) _temp1908) == Cyc_Absyn_Tunion_p:
0){ _LL1989: _temp1988=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1908)->f1; goto _LL1987; _LL1987: _temp1986=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1908)->f2; goto _LL1985; _LL1985:
_temp1984=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1908)->f3; goto _LL1983; _LL1983: _temp1982=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1908)->f4; goto _LL1939;} else{ goto
_LL1909;} _LL1911: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1990=(
char*)"_"; struct _tagged_string _temp1991; _temp1991.curr= _temp1990; _temp1991.base=
_temp1990; _temp1991.last_plus_one= _temp1990 + 2; _temp1991;})); goto _LL1909;
_LL1913: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1992=( char*)"null";
struct _tagged_string _temp1993; _temp1993.curr= _temp1992; _temp1993.base=
_temp1992; _temp1993.last_plus_one= _temp1992 + 5; _temp1993;})); goto _LL1909;
_LL1915: if( _temp1942 ==( void*) Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d",
_temp1940));} else{ s= Cyc_PP_text( xprintf("%u",( unsigned int) _temp1940));}
goto _LL1909; _LL1917: s= Cyc_PP_text(({ struct _tagged_string _temp1994= Cyc_Absynpp_char_escape(
_temp1944); xprintf("'%.*s'", _temp1994.last_plus_one - _temp1994.curr,
_temp1994.curr);})); goto _LL1909; _LL1919: s= Cyc_PP_text( _temp1946); goto
_LL1909; _LL1921: s= Cyc_Absynpp_qvar2doc( _temp1948->name); goto _LL1909;
_LL1923: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1995=( char*)"$("; struct _tagged_string _temp1996; _temp1996.curr=
_temp1995; _temp1996.base= _temp1995; _temp1996.last_plus_one= _temp1995 + 3;
_temp1996;})),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,(
struct _tagged_string)({ char* _temp1997=( char*)","; struct _tagged_string
_temp1998; _temp1998.curr= _temp1997; _temp1998.base= _temp1997; _temp1998.last_plus_one=
_temp1997 + 2; _temp1998;}), _temp1950), Cyc_PP_text(( struct _tagged_string)({
char* _temp1999=( char*)")"; struct _tagged_string _temp2000; _temp2000.curr=
_temp1999; _temp2000.base= _temp1999; _temp2000.last_plus_one= _temp1999 + 2;
_temp2000;}))); goto _LL1909; _LL1925: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2001=( char*)"&"; struct _tagged_string _temp2002;
_temp2002.curr= _temp2001; _temp2002.base= _temp2001; _temp2002.last_plus_one=
_temp2001 + 2; _temp2002;})), Cyc_Absynpp_pat2doc( _temp1952)); goto _LL1909;
_LL1927: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2003=( char*)"*"; struct _tagged_string _temp2004; _temp2004.curr=
_temp2003; _temp2004.base= _temp2003; _temp2004.last_plus_one= _temp2003 + 2;
_temp2004;})), Cyc_Absynpp_qvar2doc( _temp1954->name)); goto _LL1909; _LL1929: s=
Cyc_Absynpp_qvar2doc( _temp1956); goto _LL1909; _LL1931: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1962), Cyc_Absynpp_tvars2doc( _temp1960), Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp2005=( char*)"("; struct _tagged_string
_temp2006; _temp2006.curr= _temp2005; _temp2006.base= _temp2005; _temp2006.last_plus_one=
_temp2005 + 2; _temp2006;}),( struct _tagged_string)({ char* _temp2007=( char*)")";
struct _tagged_string _temp2008; _temp2008.curr= _temp2007; _temp2008.base=
_temp2007; _temp2008.last_plus_one= _temp2007 + 2; _temp2008;}),( struct
_tagged_string)({ char* _temp2009=( char*)","; struct _tagged_string _temp2010;
_temp2010.curr= _temp2009; _temp2010.base= _temp2009; _temp2010.last_plus_one=
_temp2009 + 2; _temp2010;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1958))); goto _LL1909; _LL1933: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1968), Cyc_Absynpp_tvars2doc( _temp1966), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp2011=( char*)"{"; struct _tagged_string _temp2012;
_temp2012.curr= _temp2011; _temp2012.base= _temp2011; _temp2012.last_plus_one=
_temp2011 + 2; _temp2012;}),( struct _tagged_string)({ char* _temp2013=( char*)"}";
struct _tagged_string _temp2014; _temp2014.curr= _temp2013; _temp2014.base=
_temp2013; _temp2014.last_plus_one= _temp2013 + 2; _temp2014;}),( struct
_tagged_string)({ char* _temp2015=( char*)","; struct _tagged_string _temp2016;
_temp2016.curr= _temp2015; _temp2016.base= _temp2015; _temp2016.last_plus_one=
_temp2015 + 2; _temp2016;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1964))); goto _LL1909; _LL1935: s= Cyc_Absynpp_cat3( _temp1976->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp2017= _temp1976->name; if( _temp2017 == 0){ _throw( Null_Exception);}
_temp2017->v;})), Cyc_Absynpp_tvars2doc( _temp1972), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp2018=( char*)"{"; struct _tagged_string _temp2019;
_temp2019.curr= _temp2018; _temp2019.base= _temp2018; _temp2019.last_plus_one=
_temp2018 + 2; _temp2019;}),( struct _tagged_string)({ char* _temp2020=( char*)"}";
struct _tagged_string _temp2021; _temp2021.curr= _temp2020; _temp2021.base=
_temp2020; _temp2021.last_plus_one= _temp2020 + 2; _temp2021;}),( struct
_tagged_string)({ char* _temp2022=( char*)","; struct _tagged_string _temp2023;
_temp2023.curr= _temp2022; _temp2023.base= _temp2022; _temp2023.last_plus_one=
_temp2022 + 2; _temp2023;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1970))); goto _LL1909; _LL1937: s= Cyc_Absynpp_qvar2doc( _temp1978->name);
goto _LL1909; _LL1939: if( _temp1982 == 0){ s= Cyc_Absynpp_qvar2doc( _temp1986->name);}
else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1986->name), Cyc_Absynpp_tvars2doc(
_temp1984), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp2024=( char*)"(";
struct _tagged_string _temp2025; _temp2025.curr= _temp2024; _temp2025.base=
_temp2024; _temp2025.last_plus_one= _temp2024 + 2; _temp2025;}),( struct
_tagged_string)({ char* _temp2026=( char*)")"; struct _tagged_string _temp2027;
_temp2027.curr= _temp2026; _temp2027.base= _temp2026; _temp2027.last_plus_one=
_temp2026 + 2; _temp2027;}),( struct _tagged_string)({ char* _temp2028=( char*)",";
struct _tagged_string _temp2029; _temp2029.curr= _temp2028; _temp2029.base=
_temp2028; _temp2029.last_plus_one= _temp2028 + 2; _temp2029;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1982)));} goto _LL1909; _LL1909:;} return s;} struct
Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return Cyc_Absynpp_cat2(
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp2030=( char*)""; struct
_tagged_string _temp2031; _temp2031.curr= _temp2030; _temp2031.base= _temp2030;
_temp2031.last_plus_one= _temp2030 + 1; _temp2031;}),( struct _tagged_string)({
char* _temp2032=( char*)"="; struct _tagged_string _temp2033; _temp2033.curr=
_temp2032; _temp2033.base= _temp2032; _temp2033.last_plus_one= _temp2032 + 2;
_temp2033;}),( struct _tagged_string)({ char* _temp2034=( char*)"="; struct
_tagged_string _temp2035; _temp2035.curr= _temp2034; _temp2035.base= _temp2034;
_temp2035.last_plus_one= _temp2034 + 2; _temp2035;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)), Cyc_Absynpp_pat2doc((* dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
==( void*) Cyc_Absyn_Wild_p: 0){ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2036=( char*)"default: "; struct _tagged_string
_temp2037; _temp2037.curr= _temp2036; _temp2037.base= _temp2036; _temp2037.last_plus_one=
_temp2036 + 10; _temp2037;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause == 0){ return Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2038=( char*)"case "; struct
_tagged_string _temp2039; _temp2039.curr= _temp2038; _temp2039.base= _temp2038;
_temp2039.last_plus_one= _temp2038 + 6; _temp2039;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2040=( char*)": "; struct
_tagged_string _temp2041; _temp2041.curr= _temp2040; _temp2041.base= _temp2040;
_temp2041.last_plus_one= _temp2040 + 3; _temp2041;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ return Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2042=( char*)"case "; struct
_tagged_string _temp2043; _temp2043.curr= _temp2042; _temp2043.base= _temp2042;
_temp2043.last_plus_one= _temp2042 + 6; _temp2043;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2044=( char*)" && "; struct
_tagged_string _temp2045; _temp2045.curr= _temp2044; _temp2045.base= _temp2044;
_temp2045.last_plus_one= _temp2044 + 5; _temp2045;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2046= c->where_clause; if(
_temp2046 == 0){ _throw( Null_Exception);} _temp2046;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2047=( char*)": "; struct _tagged_string _temp2048;
_temp2048.curr= _temp2047; _temp2048.base= _temp2047; _temp2048.last_plus_one=
_temp2047 + 3; _temp2048;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,( struct _tagged_string)({
char* _temp2049=( char*)""; struct _tagged_string _temp2050; _temp2050.curr=
_temp2049; _temp2050.base= _temp2049; _temp2050.last_plus_one= _temp2049 + 1;
_temp2050;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield*
f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);} else{ return Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( f->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2051=( char*)" = "; struct _tagged_string _temp2052; _temp2052.curr=
_temp2051; _temp2052.base= _temp2051; _temp2052.last_plus_one= _temp2051 + 4;
_temp2052;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2053= f->tag; if( _temp2053 == 0){ _throw( Null_Exception);} _temp2053;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,(
struct _tagged_string)({ char* _temp2054=( char*)","; struct _tagged_string
_temp2055; _temp2055.curr= _temp2054; _temp2055.base= _temp2054; _temp2055.last_plus_one=
_temp2054 + 2; _temp2055;}), fs);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2056=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2080; struct Cyc_Absyn_Structdecl* _temp2082;
struct Cyc_Absyn_Uniondecl* _temp2084; struct Cyc_Absyn_Vardecl* _temp2086;
struct Cyc_Absyn_Vardecl _temp2088; struct Cyc_List_List* _temp2089; struct Cyc_Core_Opt*
_temp2091; struct Cyc_Absyn_Exp* _temp2093; void* _temp2095; struct Cyc_Absyn_Tqual
_temp2097; struct _tuple0* _temp2099; void* _temp2101; struct Cyc_Absyn_Tuniondecl*
_temp2103; struct Cyc_Absyn_Tuniondecl _temp2105; int _temp2106; struct Cyc_Core_Opt*
_temp2108; struct Cyc_List_List* _temp2110; struct _tuple0* _temp2112; void*
_temp2114; int _temp2116; struct Cyc_Absyn_Exp* _temp2118; struct Cyc_Core_Opt*
_temp2120; struct Cyc_Core_Opt* _temp2122; struct Cyc_Absyn_Pat* _temp2124;
struct Cyc_Absyn_Enumdecl* _temp2126; struct Cyc_Absyn_Enumdecl _temp2128;
struct Cyc_Core_Opt* _temp2129; struct _tuple0* _temp2131; void* _temp2133;
struct Cyc_Absyn_Typedefdecl* _temp2135; struct Cyc_List_List* _temp2137; struct
_tagged_string* _temp2139; struct Cyc_List_List* _temp2141; struct _tuple0*
_temp2143; struct Cyc_List_List* _temp2145; _LL2058: if(*(( int*) _temp2056) ==
Cyc_Absyn_Fn_d){ _LL2081: _temp2080=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp2056)->f1; goto _LL2059;} else{ goto _LL2060;} _LL2060: if(*(( int*)
_temp2056) == Cyc_Absyn_Struct_d){ _LL2083: _temp2082=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp2056)->f1; goto _LL2061;} else{ goto
_LL2062;} _LL2062: if(*(( int*) _temp2056) == Cyc_Absyn_Union_d){ _LL2085:
_temp2084=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp2056)->f1; goto _LL2063;} else{ goto _LL2064;} _LL2064: if(*(( int*)
_temp2056) == Cyc_Absyn_Var_d){ _LL2087: _temp2086=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp2056)->f1; _temp2088=* _temp2086; _LL2102:
_temp2101=( void*) _temp2088.sc; goto _LL2100; _LL2100: _temp2099=( struct
_tuple0*) _temp2088.name; goto _LL2098; _LL2098: _temp2097=( struct Cyc_Absyn_Tqual)
_temp2088.tq; goto _LL2096; _LL2096: _temp2095=( void*) _temp2088.type; goto
_LL2094; _LL2094: _temp2093=( struct Cyc_Absyn_Exp*) _temp2088.initializer; goto
_LL2092; _LL2092: _temp2091=( struct Cyc_Core_Opt*) _temp2088.rgn; goto _LL2090;
_LL2090: _temp2089=( struct Cyc_List_List*) _temp2088.attributes; goto _LL2065;}
else{ goto _LL2066;} _LL2066: if(*(( int*) _temp2056) == Cyc_Absyn_Tunion_d){
_LL2104: _temp2103=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2056)->f1; _temp2105=* _temp2103; _LL2115: _temp2114=( void*) _temp2105.sc;
goto _LL2113; _LL2113: _temp2112=( struct _tuple0*) _temp2105.name; goto _LL2111;
_LL2111: _temp2110=( struct Cyc_List_List*) _temp2105.tvs; goto _LL2109; _LL2109:
_temp2108=( struct Cyc_Core_Opt*) _temp2105.fields; goto _LL2107; _LL2107:
_temp2106=( int) _temp2105.is_xtunion; goto _LL2067;} else{ goto _LL2068;}
_LL2068: if(*(( int*) _temp2056) == Cyc_Absyn_Let_d){ _LL2125: _temp2124=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2056)->f1; goto
_LL2123; _LL2123: _temp2122=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2056)->f2; goto _LL2121; _LL2121: _temp2120=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2056)->f3; goto _LL2119; _LL2119: _temp2118=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2056)->f4; goto
_LL2117; _LL2117: _temp2116=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2056)->f5;
goto _LL2069;} else{ goto _LL2070;} _LL2070: if(*(( int*) _temp2056) == Cyc_Absyn_Enum_d){
_LL2127: _temp2126=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp2056)->f1; _temp2128=* _temp2126; _LL2134: _temp2133=( void*) _temp2128.sc;
goto _LL2132; _LL2132: _temp2131=( struct _tuple0*) _temp2128.name; goto _LL2130;
_LL2130: _temp2129=( struct Cyc_Core_Opt*) _temp2128.fields; goto _LL2071;}
else{ goto _LL2072;} _LL2072: if(*(( int*) _temp2056) == Cyc_Absyn_Typedef_d){
_LL2136: _temp2135=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2056)->f1; goto _LL2073;} else{ goto _LL2074;} _LL2074: if(*(( int*)
_temp2056) == Cyc_Absyn_Namespace_d){ _LL2140: _temp2139=( struct _tagged_string*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2056)->f1; goto _LL2138; _LL2138:
_temp2137=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2056)->f2; goto _LL2075;} else{ goto _LL2076;} _LL2076: if(*(( int*)
_temp2056) == Cyc_Absyn_Using_d){ _LL2144: _temp2143=( struct _tuple0*)(( struct
Cyc_Absyn_Using_d_struct*) _temp2056)->f1; goto _LL2142; _LL2142: _temp2141=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp2056)->f2; goto
_LL2077;} else{ goto _LL2078;} _LL2078: if(*(( int*) _temp2056) == Cyc_Absyn_ExternC_d){
_LL2146: _temp2145=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2056)->f1; goto _LL2079;} else{ goto _LL2057;} _LL2059: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2157=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2157[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2158; _temp2158.tag= Cyc_Absyn_FnType; _temp2158.f1=({ struct Cyc_Absyn_FnInfo
_temp2159; _temp2159.tvars= _temp2080->tvs; _temp2159.effect= _temp2080->effect;
_temp2159.ret_typ=( void*)(( void*) _temp2080->ret_type); _temp2159.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2080->args); _temp2159.varargs=
_temp2080->varargs; _temp2159.attributes= 0; _temp2159;}); _temp2158;});
_temp2157;}); s= Cyc_Absynpp_cat7( Cyc_PP_text( _temp2080->is_inline?( struct
_tagged_string)({ char* _temp2147=( char*)"inline "; struct _tagged_string
_temp2148; _temp2148.curr= _temp2147; _temp2148.base= _temp2147; _temp2148.last_plus_one=
_temp2147 + 8; _temp2148;}):( struct _tagged_string)({ char* _temp2149=( char*)"";
struct _tagged_string _temp2150; _temp2150.curr= _temp2149; _temp2150.base=
_temp2149; _temp2150.last_plus_one= _temp2149 + 1; _temp2150;})), Cyc_Absynpp_scope2doc((
void*) _temp2080->sc), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual _temp2151;
_temp2151.q_const= 0; _temp2151.q_volatile= 0; _temp2151.q_restrict= 0;
_temp2151;}), t,({ struct Cyc_Core_Opt* _temp2152=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2152->v=( void*) Cyc_Absynpp_cat2(
Cyc_Absynpp_atts2doc( _temp2080->attributes), Cyc_Absynpp_typedef_name2doc(
_temp2080->name)); _temp2152;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2153=( char*)" {"; struct _tagged_string _temp2154; _temp2154.curr=
_temp2153; _temp2154.base= _temp2153; _temp2154.last_plus_one= _temp2153 + 3;
_temp2154;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp2080->body))), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2155=( char*)"}"; struct _tagged_string _temp2156; _temp2156.curr=
_temp2155; _temp2156.base= _temp2155; _temp2156.last_plus_one= _temp2155 + 2;
_temp2156;}))); goto _LL2057;} _LL2061: if( _temp2082->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp2082->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2160=( char*)"struct "; struct _tagged_string
_temp2161; _temp2161.curr= _temp2160; _temp2161.base= _temp2160; _temp2161.last_plus_one=
_temp2160 + 8; _temp2161;})), _temp2082->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2162=( char*)""; struct _tagged_string _temp2163;
_temp2163.curr= _temp2162; _temp2163.base= _temp2162; _temp2163.last_plus_one=
_temp2162 + 1; _temp2163;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp2164= _temp2082->name; if( _temp2164 == 0){ _throw(
Null_Exception);} _temp2164->v;})), Cyc_Absynpp_ktvars2doc( _temp2082->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2165=( char*)";"; struct _tagged_string
_temp2166; _temp2166.curr= _temp2165; _temp2166.base= _temp2165; _temp2166.last_plus_one=
_temp2165 + 2; _temp2166;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2082->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2167=(
char*)"struct "; struct _tagged_string _temp2168; _temp2168.curr= _temp2167;
_temp2168.base= _temp2167; _temp2168.last_plus_one= _temp2167 + 8; _temp2168;})),
_temp2082->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2169=(
char*)""; struct _tagged_string _temp2170; _temp2170.curr= _temp2169; _temp2170.base=
_temp2169; _temp2170.last_plus_one= _temp2169 + 1; _temp2170;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2171= _temp2082->name; if(
_temp2171 == 0){ _throw( Null_Exception);} _temp2171->v;})), Cyc_Absynpp_ktvars2doc(
_temp2082->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2172=( char*)" {";
struct _tagged_string _temp2173; _temp2173.curr= _temp2172; _temp2173.base=
_temp2172; _temp2173.last_plus_one= _temp2172 + 3; _temp2173;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2174= _temp2082->fields; if( _temp2174 == 0){ _throw(
Null_Exception);} _temp2174->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2175=( char*)"}"; struct _tagged_string _temp2176;
_temp2176.curr= _temp2175; _temp2176.base= _temp2175; _temp2176.last_plus_one=
_temp2175 + 2; _temp2176;})), Cyc_Absynpp_atts2doc( _temp2082->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2177=( char*)";"; struct _tagged_string
_temp2178; _temp2178.curr= _temp2177; _temp2178.base= _temp2177; _temp2178.last_plus_one=
_temp2177 + 2; _temp2178;})));} goto _LL2057; _LL2063: if( _temp2084->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp2084->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2179=( char*)"union "; struct _tagged_string
_temp2180; _temp2180.curr= _temp2179; _temp2180.base= _temp2179; _temp2180.last_plus_one=
_temp2179 + 7; _temp2180;})), _temp2084->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2181=( char*)""; struct _tagged_string _temp2182;
_temp2182.curr= _temp2181; _temp2182.base= _temp2181; _temp2182.last_plus_one=
_temp2181 + 1; _temp2182;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2183= _temp2084->name; if( _temp2183 == 0){ _throw(
Null_Exception);} _temp2183->v;})), Cyc_Absynpp_tvars2doc( _temp2084->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2184=( char*)";"; struct _tagged_string
_temp2185; _temp2185.curr= _temp2184; _temp2185.base= _temp2184; _temp2185.last_plus_one=
_temp2184 + 2; _temp2185;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2084->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2186=(
char*)"union "; struct _tagged_string _temp2187; _temp2187.curr= _temp2186;
_temp2187.base= _temp2186; _temp2187.last_plus_one= _temp2186 + 7; _temp2187;})),
_temp2084->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2188=(
char*)""; struct _tagged_string _temp2189; _temp2189.curr= _temp2188; _temp2189.base=
_temp2188; _temp2189.last_plus_one= _temp2188 + 1; _temp2189;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2190= _temp2084->name; if(
_temp2190 == 0){ _throw( Null_Exception);} _temp2190->v;})), Cyc_Absynpp_tvars2doc(
_temp2084->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2191=( char*)" {";
struct _tagged_string _temp2192; _temp2192.curr= _temp2191; _temp2192.base=
_temp2191; _temp2192.last_plus_one= _temp2191 + 3; _temp2192;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2193= _temp2084->fields; if( _temp2193 == 0){ _throw(
Null_Exception);} _temp2193->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2194=( char*)"}"; struct _tagged_string _temp2195;
_temp2195.curr= _temp2194; _temp2195.base= _temp2194; _temp2195.last_plus_one=
_temp2194 + 2; _temp2195;})), Cyc_Absynpp_atts2doc( _temp2084->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2196=( char*)";"; struct _tagged_string
_temp2197; _temp2197.curr= _temp2196; _temp2197.base= _temp2196; _temp2197.last_plus_one=
_temp2196 + 2; _temp2197;})));} goto _LL2057; _LL2065: { struct Cyc_PP_Doc* sn=
Cyc_Absynpp_typedef_name2doc( _temp2099); s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(
_temp2101), Cyc_Absynpp_tqtd2doc( _temp2097, _temp2095,({ struct Cyc_Core_Opt*
_temp2198=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2198->v=( void*) sn; _temp2198;})), Cyc_Absynpp_atts2doc( _temp2089),
_temp2093 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2199=( char*)"";
struct _tagged_string _temp2200; _temp2200.curr= _temp2199; _temp2200.base=
_temp2199; _temp2200.last_plus_one= _temp2199 + 1; _temp2200;})): Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2201=( char*)" = "; struct
_tagged_string _temp2202; _temp2202.curr= _temp2201; _temp2202.base= _temp2201;
_temp2202.last_plus_one= _temp2201 + 4; _temp2202;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2203= _temp2093; if(
_temp2203 == 0){ _throw( Null_Exception);} _temp2203;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2204=( char*)";"; struct _tagged_string _temp2205;
_temp2205.curr= _temp2204; _temp2205.base= _temp2204; _temp2205.last_plus_one=
_temp2204 + 2; _temp2205;}))); goto _LL2057;} _LL2067: if( _temp2108 == 0){ s=
Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc( _temp2114), _temp2106? Cyc_PP_text((
struct _tagged_string)({ char* _temp2206=( char*)"xtunion "; struct
_tagged_string _temp2207; _temp2207.curr= _temp2206; _temp2207.base= _temp2206;
_temp2207.last_plus_one= _temp2206 + 9; _temp2207;})): Cyc_PP_text(( struct
_tagged_string)({ char* _temp2208=( char*)"tunion "; struct _tagged_string
_temp2209; _temp2209.curr= _temp2208; _temp2209.base= _temp2208; _temp2209.last_plus_one=
_temp2208 + 8; _temp2209;})), _temp2106? Cyc_Absynpp_qvar2doc( _temp2112): Cyc_Absynpp_typedef_name2doc(
_temp2112), Cyc_Absynpp_ktvars2doc( _temp2110), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2210=( char*)";"; struct _tagged_string _temp2211;
_temp2211.curr= _temp2210; _temp2211.base= _temp2210; _temp2211.last_plus_one=
_temp2210 + 2; _temp2211;})));} else{ s= Cyc_Absynpp_cat8( Cyc_Absynpp_scope2doc(
_temp2114), _temp2106? Cyc_PP_text(( struct _tagged_string)({ char* _temp2212=(
char*)"xtunion "; struct _tagged_string _temp2213; _temp2213.curr= _temp2212;
_temp2213.base= _temp2212; _temp2213.last_plus_one= _temp2212 + 9; _temp2213;})):
Cyc_PP_text(( struct _tagged_string)({ char* _temp2214=( char*)"tunion "; struct
_tagged_string _temp2215; _temp2215.curr= _temp2214; _temp2215.base= _temp2214;
_temp2215.last_plus_one= _temp2214 + 8; _temp2215;})), _temp2106? Cyc_Absynpp_qvar2doc(
_temp2112): Cyc_Absynpp_typedef_name2doc( _temp2112), Cyc_Absynpp_ktvars2doc(
_temp2110), Cyc_PP_text(( struct _tagged_string)({ char* _temp2216=( char*)" {";
struct _tagged_string _temp2217; _temp2217.curr= _temp2216; _temp2217.base=
_temp2216; _temp2217.last_plus_one= _temp2216 + 3; _temp2217;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2218= _temp2108; if( _temp2218 == 0){ _throw(
Null_Exception);} _temp2218->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2219=( char*)"};"; struct _tagged_string _temp2220;
_temp2220.curr= _temp2219; _temp2220.base= _temp2219; _temp2220.last_plus_one=
_temp2219 + 3; _temp2220;})));} goto _LL2057; _LL2069: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp2221=( char*)"let "; struct _tagged_string
_temp2222; _temp2222.curr= _temp2221; _temp2222.base= _temp2221; _temp2222.last_plus_one=
_temp2221 + 5; _temp2222;})), Cyc_Absynpp_pat2doc( _temp2124), Cyc_PP_text((
struct _tagged_string)({ char* _temp2223=( char*)" = "; struct _tagged_string
_temp2224; _temp2224.curr= _temp2223; _temp2224.base= _temp2223; _temp2224.last_plus_one=
_temp2223 + 4; _temp2224;})), Cyc_Absynpp_exp2doc( _temp2118), Cyc_PP_text((
struct _tagged_string)({ char* _temp2225=( char*)";"; struct _tagged_string
_temp2226; _temp2226.curr= _temp2225; _temp2226.base= _temp2225; _temp2226.last_plus_one=
_temp2225 + 2; _temp2226;}))); goto _LL2057; _LL2071: if( _temp2129 == 0){ s=
Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc( _temp2133), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2227=( char*)"enum "; struct _tagged_string
_temp2228; _temp2228.curr= _temp2227; _temp2228.base= _temp2227; _temp2228.last_plus_one=
_temp2227 + 6; _temp2228;})), Cyc_Absynpp_typedef_name2doc( _temp2131), Cyc_PP_text((
struct _tagged_string)({ char* _temp2229=( char*)";"; struct _tagged_string
_temp2230; _temp2230.curr= _temp2229; _temp2230.base= _temp2229; _temp2230.last_plus_one=
_temp2229 + 2; _temp2230;})));} else{ s= Cyc_Absynpp_cat7( Cyc_Absynpp_scope2doc(
_temp2133), Cyc_PP_text(( struct _tagged_string)({ char* _temp2231=( char*)"enum ";
struct _tagged_string _temp2232; _temp2232.curr= _temp2231; _temp2232.base=
_temp2231; _temp2232.last_plus_one= _temp2231 + 6; _temp2232;})), Cyc_Absynpp_qvar2doc(
_temp2131), Cyc_PP_text(( struct _tagged_string)({ char* _temp2233=( char*)" {";
struct _tagged_string _temp2234; _temp2234.curr= _temp2233; _temp2234.base=
_temp2233; _temp2234.last_plus_one= _temp2233 + 3; _temp2234;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2235= _temp2129; if( _temp2235 == 0){ _throw(
Null_Exception);} _temp2235->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2236=( char*)"};"; struct _tagged_string _temp2237;
_temp2237.curr= _temp2236; _temp2237.base= _temp2236; _temp2237.last_plus_one=
_temp2236 + 3; _temp2237;})));} goto _LL2057; _LL2073: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp2238=( char*)"typedef "; struct
_tagged_string _temp2239; _temp2239.curr= _temp2238; _temp2239.base= _temp2238;
_temp2239.last_plus_one= _temp2238 + 9; _temp2239;})), Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual _temp2240; _temp2240.q_const= 0; _temp2240.q_volatile= 0;
_temp2240.q_restrict= 0; _temp2240;}),( void*) _temp2135->defn,({ struct Cyc_Core_Opt*
_temp2241=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2241->v=( void*) Cyc_Absynpp_cat2( Cyc_Absynpp_typedef_name2doc( _temp2135->name),
Cyc_Absynpp_tvars2doc( _temp2135->tvs)); _temp2241;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2242=( char*)";"; struct _tagged_string _temp2243;
_temp2243.curr= _temp2242; _temp2243.base= _temp2242; _temp2243.last_plus_one=
_temp2242 + 2; _temp2243;}))); goto _LL2057; _LL2075: if( ! Cyc_Absynpp_qvar_to_Cids){
Cyc_Absynpp_curr_namespace_add( _temp2139);} s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp2244=( char*)"namespace "; struct
_tagged_string _temp2245; _temp2245.curr= _temp2244; _temp2245.base= _temp2244;
_temp2245.last_plus_one= _temp2244 + 11; _temp2245;})), Cyc_Absynpp_textptr(
_temp2139), Cyc_PP_text(( struct _tagged_string)({ char* _temp2246=( char*)" {";
struct _tagged_string _temp2247; _temp2247.curr= _temp2246; _temp2247.base=
_temp2246; _temp2247.last_plus_one= _temp2246 + 3; _temp2247;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2248=( char*)""; struct _tagged_string
_temp2249; _temp2249.curr= _temp2248; _temp2249.base= _temp2248; _temp2249.last_plus_one=
_temp2248 + 1; _temp2249;}), _temp2137), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2250=( char*)"}"; struct _tagged_string _temp2251;
_temp2251.curr= _temp2250; _temp2251.base= _temp2250; _temp2251.last_plus_one=
_temp2250 + 2; _temp2251;}))); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2057; _LL2077: if( Cyc_Absynpp_print_using_stmts){ s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2252=( char*)"using "; struct
_tagged_string _temp2253; _temp2253.curr= _temp2252; _temp2253.base= _temp2252;
_temp2253.last_plus_one= _temp2252 + 7; _temp2253;})), Cyc_Absynpp_qvar2doc(
_temp2143), Cyc_PP_text(( struct _tagged_string)({ char* _temp2254=( char*)" {";
struct _tagged_string _temp2255; _temp2255.curr= _temp2254; _temp2255.base=
_temp2254; _temp2255.last_plus_one= _temp2254 + 3; _temp2255;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2256=( char*)""; struct _tagged_string
_temp2257; _temp2257.curr= _temp2256; _temp2257.base= _temp2256; _temp2257.last_plus_one=
_temp2256 + 1; _temp2257;}), _temp2141), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2258=( char*)"}"; struct _tagged_string _temp2259;
_temp2259.curr= _temp2258; _temp2259.base= _temp2258; _temp2259.last_plus_one=
_temp2258 + 2; _temp2259;})));} else{ s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2260=( char*)"/* using "; struct _tagged_string
_temp2261; _temp2261.curr= _temp2260; _temp2261.base= _temp2260; _temp2261.last_plus_one=
_temp2260 + 10; _temp2261;})), Cyc_Absynpp_qvar2doc( _temp2143), Cyc_PP_text((
struct _tagged_string)({ char* _temp2262=( char*)" { */"; struct _tagged_string
_temp2263; _temp2263.curr= _temp2262; _temp2263.base= _temp2262; _temp2263.last_plus_one=
_temp2262 + 6; _temp2263;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2264=( char*)""; struct _tagged_string _temp2265; _temp2265.curr=
_temp2264; _temp2265.base= _temp2264; _temp2265.last_plus_one= _temp2264 + 1;
_temp2265;}), _temp2141), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2266=( char*)"/* } */"; struct _tagged_string _temp2267; _temp2267.curr=
_temp2266; _temp2267.base= _temp2266; _temp2267.last_plus_one= _temp2266 + 8;
_temp2267;})));} goto _LL2057; _LL2079: if( Cyc_Absynpp_print_externC_stmts){ s=
Cyc_Absynpp_cat5( Cyc_PP_text(( struct _tagged_string)({ char* _temp2268=( char*)"extern \"C\" {";
struct _tagged_string _temp2269; _temp2269.curr= _temp2268; _temp2269.base=
_temp2268; _temp2269.last_plus_one= _temp2268 + 13; _temp2269;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2270=( char*)""; struct _tagged_string
_temp2271; _temp2271.curr= _temp2270; _temp2271.base= _temp2270; _temp2271.last_plus_one=
_temp2270 + 1; _temp2271;}), _temp2145), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2272=( char*)"}"; struct _tagged_string _temp2273;
_temp2273.curr= _temp2272; _temp2273.base= _temp2272; _temp2273.last_plus_one=
_temp2272 + 2; _temp2273;})));} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2274=( char*)"/* extern \"C\" { */"; struct
_tagged_string _temp2275; _temp2275.curr= _temp2274; _temp2275.base= _temp2274;
_temp2275.last_plus_one= _temp2274 + 19; _temp2275;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2276=( char*)""; struct _tagged_string
_temp2277; _temp2277.curr= _temp2276; _temp2277.base= _temp2276; _temp2277.last_plus_one=
_temp2276 + 1; _temp2277;}), _temp2145), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2278=( char*)"/* } */"; struct _tagged_string
_temp2279; _temp2279.curr= _temp2278; _temp2279.base= _temp2278; _temp2279.last_plus_one=
_temp2278 + 8; _temp2279;})));} goto _LL2057; _LL2057:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void* sc){ void* _temp2280= sc; _LL2282: if( _temp2280 ==(
void*) Cyc_Absyn_Static){ goto _LL2283;} else{ goto _LL2284;} _LL2284: if(
_temp2280 ==( void*) Cyc_Absyn_Public){ goto _LL2285;} else{ goto _LL2286;}
_LL2286: if( _temp2280 ==( void*) Cyc_Absyn_Extern){ goto _LL2287;} else{ goto
_LL2288;} _LL2288: if( _temp2280 ==( void*) Cyc_Absyn_ExternC){ goto _LL2289;}
else{ goto _LL2290;} _LL2290: if( _temp2280 ==( void*) Cyc_Absyn_Abstract){ goto
_LL2291;} else{ goto _LL2281;} _LL2283: return Cyc_PP_text(( struct
_tagged_string)({ char* _temp2292=( char*)"static "; struct _tagged_string
_temp2293; _temp2293.curr= _temp2292; _temp2293.base= _temp2292; _temp2293.last_plus_one=
_temp2292 + 8; _temp2293;})); _LL2285: return Cyc_PP_nil_doc(); _LL2287: return
Cyc_PP_text(( struct _tagged_string)({ char* _temp2294=( char*)"extern "; struct
_tagged_string _temp2295; _temp2295.curr= _temp2294; _temp2295.base= _temp2294;
_temp2295.last_plus_one= _temp2294 + 8; _temp2295;})); _LL2289: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2296=( char*)"extern \"C\" "; struct
_tagged_string _temp2297; _temp2297.curr= _temp2296; _temp2297.base= _temp2296;
_temp2297.last_plus_one= _temp2296 + 12; _temp2297;})); _LL2291: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2298=( char*)"abstract "; struct
_tagged_string _temp2299; _temp2299.curr= _temp2298; _temp2299.base= _temp2298;
_temp2299.last_plus_one= _temp2298 + 10; _temp2299;})); _LL2281:;} int Cyc_Absynpp_exists_temp_tvar_in_effect(
void* t){ void* _temp2300= t; struct Cyc_Absyn_Tvar* _temp2308; struct Cyc_List_List*
_temp2310; _LL2302: if(( unsigned int) _temp2300 > 4u?*(( int*) _temp2300) ==
Cyc_Absyn_VarType: 0){ _LL2309: _temp2308=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp2300)->f1; goto _LL2303;} else{ goto _LL2304;} _LL2304: if(( unsigned int)
_temp2300 > 4u?*(( int*) _temp2300) == Cyc_Absyn_JoinEff: 0){ _LL2311: _temp2310=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp2300)->f1; goto
_LL2305;} else{ goto _LL2306;} _LL2306: goto _LL2307; _LL2303: return Cyc_Tcutil_is_temp_tvar(
_temp2308); _LL2305: return(( int(*)( int(* pred)( void*), struct Cyc_List_List*
x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect, _temp2310);
_LL2307: return 0; _LL2301:;} struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual
tq, void* t){ void* _temp2312= t; struct Cyc_Absyn_Exp* _temp2326; struct Cyc_Absyn_Tqual
_temp2328; void* _temp2330; struct Cyc_Absyn_PtrInfo _temp2332; struct Cyc_Absyn_Conref*
_temp2334; struct Cyc_Absyn_Tqual _temp2336; struct Cyc_Absyn_Conref* _temp2338;
void* _temp2340; void* _temp2342; struct Cyc_Absyn_FnInfo _temp2344; struct Cyc_List_List*
_temp2346; int _temp2348; struct Cyc_List_List* _temp2350; void* _temp2352;
struct Cyc_Core_Opt* _temp2354; struct Cyc_List_List* _temp2356; int _temp2358;
struct Cyc_Core_Opt* _temp2360; void* _temp2362; struct Cyc_Core_Opt* _temp2364;
struct Cyc_List_List* _temp2366; struct _tuple0* _temp2368; _LL2314: if((
unsigned int) _temp2312 > 4u?*(( int*) _temp2312) == Cyc_Absyn_ArrayType: 0){
_LL2331: _temp2330=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2312)->f1;
goto _LL2329; _LL2329: _temp2328=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2312)->f2; goto _LL2327; _LL2327: _temp2326=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2312)->f3; goto _LL2315;} else{ goto
_LL2316;} _LL2316: if(( unsigned int) _temp2312 > 4u?*(( int*) _temp2312) == Cyc_Absyn_PointerType:
0){ _LL2333: _temp2332=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2312)->f1; _LL2343: _temp2342=( void*) _temp2332.elt_typ; goto _LL2341;
_LL2341: _temp2340=( void*) _temp2332.rgn_typ; goto _LL2339; _LL2339: _temp2338=(
struct Cyc_Absyn_Conref*) _temp2332.nullable; goto _LL2337; _LL2337: _temp2336=(
struct Cyc_Absyn_Tqual) _temp2332.tq; goto _LL2335; _LL2335: _temp2334=( struct
Cyc_Absyn_Conref*) _temp2332.bounds; goto _LL2317;} else{ goto _LL2318;} _LL2318:
if(( unsigned int) _temp2312 > 4u?*(( int*) _temp2312) == Cyc_Absyn_FnType: 0){
_LL2345: _temp2344=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2312)->f1; _LL2357: _temp2356=( struct Cyc_List_List*) _temp2344.tvars;
goto _LL2355; _LL2355: _temp2354=( struct Cyc_Core_Opt*) _temp2344.effect; goto
_LL2353; _LL2353: _temp2352=( void*) _temp2344.ret_typ; goto _LL2351; _LL2351:
_temp2350=( struct Cyc_List_List*) _temp2344.args; goto _LL2349; _LL2349:
_temp2348=( int) _temp2344.varargs; goto _LL2347; _LL2347: _temp2346=( struct
Cyc_List_List*) _temp2344.attributes; goto _LL2319;} else{ goto _LL2320;}
_LL2320: if(( unsigned int) _temp2312 > 4u?*(( int*) _temp2312) == Cyc_Absyn_Evar:
0){ _LL2363: _temp2362=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp2312)->f1;
goto _LL2361; _LL2361: _temp2360=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp2312)->f2; goto _LL2359; _LL2359: _temp2358=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp2312)->f3; goto _LL2321;} else{ goto _LL2322;} _LL2322: if(( unsigned int)
_temp2312 > 4u?*(( int*) _temp2312) == Cyc_Absyn_TypedefType: 0){ _LL2369:
_temp2368=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2312)->f1;
goto _LL2367; _LL2367: _temp2366=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2312)->f2; goto _LL2365; _LL2365: _temp2364=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2312)->f3; goto _LL2323;} else{ goto
_LL2324;} _LL2324: goto _LL2325; _LL2315: { struct Cyc_List_List* _temp2372;
void* _temp2374; struct Cyc_Absyn_Tqual _temp2376; struct _tuple4 _temp2370= Cyc_Absynpp_to_tms(
_temp2328, _temp2330); _LL2377: _temp2376= _temp2370.f1; goto _LL2375; _LL2375:
_temp2374= _temp2370.f2; goto _LL2373; _LL2373: _temp2372= _temp2370.f3; goto
_LL2371; _LL2371: { void* tm; if( _temp2326 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;}
else{ tm=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct* _temp2378=( struct
Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct));
_temp2378[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct _temp2379; _temp2379.tag=
Cyc_Absyn_ConstArray_mod; _temp2379.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2380= _temp2326; if( _temp2380 == 0){ _throw( Null_Exception);} _temp2380;});
_temp2379;}); _temp2378;});} return({ struct _tuple4 _temp2381; _temp2381.f1=
_temp2376; _temp2381.f2= _temp2374; _temp2381.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2382=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2382->hd=( void*) tm; _temp2382->tl= _temp2372;
_temp2382;}); _temp2381;});}} _LL2317: { struct Cyc_List_List* _temp2385; void*
_temp2387; struct Cyc_Absyn_Tqual _temp2389; struct _tuple4 _temp2383= Cyc_Absynpp_to_tms(
_temp2336, _temp2342); _LL2390: _temp2389= _temp2383.f1; goto _LL2388; _LL2388:
_temp2387= _temp2383.f2; goto _LL2386; _LL2386: _temp2385= _temp2383.f3; goto
_LL2384; _LL2384: { void* ps;{ struct _tuple8 _temp2392=({ struct _tuple8
_temp2391; _temp2391.f1=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2338))->v; _temp2391.f2=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2334))->v; _temp2391;}); void* _temp2402; void* _temp2404; struct Cyc_Absyn_Exp*
_temp2406; void* _temp2408; int _temp2410; void* _temp2412; void* _temp2414;
struct Cyc_Absyn_Exp* _temp2416; void* _temp2418; int _temp2420; void* _temp2422;
void* _temp2424; void* _temp2426; _LL2394: _LL2409: _temp2408= _temp2392.f1; if((
unsigned int) _temp2408 > 1u?*(( int*) _temp2408) == Cyc_Absyn_Eq_constr: 0){
_LL2411: _temp2410=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2408)->f1;
if( _temp2410 == 1){ goto _LL2403;} else{ goto _LL2396;}} else{ goto _LL2396;}
_LL2403: _temp2402= _temp2392.f2; if(( unsigned int) _temp2402 > 1u?*(( int*)
_temp2402) == Cyc_Absyn_Eq_constr: 0){ _LL2405: _temp2404=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2402)->f1; if(( unsigned int) _temp2404 > 1u?*(( int*) _temp2404) == Cyc_Absyn_Upper_b:
0){ _LL2407: _temp2406=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2404)->f1; goto _LL2395;} else{ goto _LL2396;}} else{ goto _LL2396;}
_LL2396: _LL2419: _temp2418= _temp2392.f1; if(( unsigned int) _temp2418 > 1u?*((
int*) _temp2418) == Cyc_Absyn_Eq_constr: 0){ _LL2421: _temp2420=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp2418)->f1; if( _temp2420 == 0){ goto _LL2413;}
else{ goto _LL2398;}} else{ goto _LL2398;} _LL2413: _temp2412= _temp2392.f2; if((
unsigned int) _temp2412 > 1u?*(( int*) _temp2412) == Cyc_Absyn_Eq_constr: 0){
_LL2415: _temp2414=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2412)->f1;
if(( unsigned int) _temp2414 > 1u?*(( int*) _temp2414) == Cyc_Absyn_Upper_b: 0){
_LL2417: _temp2416=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2414)->f1; goto _LL2397;} else{ goto _LL2398;}} else{ goto _LL2398;}
_LL2398: _LL2427: _temp2426= _temp2392.f1; goto _LL2423; _LL2423: _temp2422=
_temp2392.f2; if(( unsigned int) _temp2422 > 1u?*(( int*) _temp2422) == Cyc_Absyn_Eq_constr:
0){ _LL2425: _temp2424=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2422)->f1;
if( _temp2424 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2399;} else{ goto _LL2400;}}
else{ goto _LL2400;} _LL2400: goto _LL2401; _LL2395: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2428=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2428[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2429; _temp2429.tag=
Cyc_Absyn_Nullable_ps; _temp2429.f1= _temp2406; _temp2429;}); _temp2428;}); goto
_LL2393; _LL2397: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2430=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2430[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2431; _temp2431.tag= Cyc_Absyn_NonNullable_ps; _temp2431.f1= _temp2416;
_temp2431;}); _temp2430;}); goto _LL2393; _LL2399: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2393; _LL2401: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2393;
_LL2393:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2434=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2434[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2435; _temp2435.tag=
Cyc_Absyn_Pointer_mod; _temp2435.f1=( void*) ps; _temp2435.f2=( void*) _temp2340;
_temp2435.f3= tq; _temp2435;}); _temp2434;}); return({ struct _tuple4 _temp2432;
_temp2432.f1= _temp2389; _temp2432.f2= _temp2387; _temp2432.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2433=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2433->hd=( void*) tm; _temp2433->tl= _temp2385;
_temp2433;}); _temp2432;});}}} _LL2319: if( ! Cyc_Absynpp_print_all_tvars){ if(
_temp2354 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)({ struct Cyc_Core_Opt*
_temp2436= _temp2354; if( _temp2436 == 0){ _throw( Null_Exception);} _temp2436->v;}))){
_temp2354= 0; _temp2356= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2356);}}{ struct Cyc_List_List* _temp2439;
void* _temp2441; struct Cyc_Absyn_Tqual _temp2443; struct _tuple4 _temp2437= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp2352); _LL2444: _temp2443= _temp2437.f1; goto
_LL2442; _LL2442: _temp2441= _temp2437.f2; goto _LL2440; _LL2440: _temp2439=
_temp2437.f3; goto _LL2438; _LL2438: { struct Cyc_List_List* tms= _temp2439; if(
_temp2346 != 0){ tms=({ struct Cyc_List_List* _temp2445=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2445->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2446=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2446[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2447; _temp2447.tag= Cyc_Absyn_Attributes_mod;
_temp2447.f1= 0; _temp2447.f2= _temp2346; _temp2447;}); _temp2446;})); _temp2445->tl=
tms; _temp2445;});} tms=({ struct Cyc_List_List* _temp2448=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2448->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2449=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2449[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2450; _temp2450.tag= Cyc_Absyn_Function_mod;
_temp2450.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2451=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2451[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2452; _temp2452.tag= Cyc_Absyn_WithTypes;
_temp2452.f1= _temp2350; _temp2452.f2= _temp2348; _temp2452.f3= _temp2354;
_temp2452;}); _temp2451;})); _temp2450;}); _temp2449;})); _temp2448->tl= tms;
_temp2448;}); if( _temp2356 != 0){ tms=({ struct Cyc_List_List* _temp2453=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2453->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2454=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2454[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2455; _temp2455.tag= Cyc_Absyn_TypeParams_mod;
_temp2455.f1= _temp2356; _temp2455.f2= 0; _temp2455.f3= 1; _temp2455;});
_temp2454;})); _temp2453->tl= tms; _temp2453;});} return({ struct _tuple4
_temp2456; _temp2456.f1= _temp2443; _temp2456.f2= _temp2441; _temp2456.f3= tms;
_temp2456;});}} _LL2321: if( _temp2360 == 0){ return({ struct _tuple4 _temp2457;
_temp2457.f1= tq; _temp2457.f2= t; _temp2457.f3= 0; _temp2457;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2458= _temp2360; if(
_temp2458 == 0){ _throw( Null_Exception);} _temp2458->v;}));} _LL2323: if(
_temp2364 == 0? 1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4
_temp2459; _temp2459.f1= tq; _temp2459.f2= t; _temp2459.f3= 0; _temp2459;});}
else{ return Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2460=
_temp2364; if( _temp2460 == 0){ _throw( Null_Exception);} _temp2460->v;}));}
_LL2325: return({ struct _tuple4 _temp2461; _temp2461.f1= tq; _temp2461.f2= t;
_temp2461.f3= 0; _temp2461;}); _LL2313:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp2464; void* _temp2466; struct Cyc_Absyn_Tqual _temp2468; struct _tuple4
_temp2462= Cyc_Absynpp_to_tms( tq, t); _LL2469: _temp2468= _temp2462.f1; goto
_LL2467; _LL2467: _temp2466= _temp2462.f2; goto _LL2465; _LL2465: _temp2464=
_temp2462.f3; goto _LL2463; _LL2463: _temp2464=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2464); if( _temp2464 == 0?
dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc( _temp2468), Cyc_Absynpp_ntyp2doc(
_temp2466));} else{ return Cyc_Absynpp_cat4( Cyc_Absynpp_tqual2doc( _temp2468),
Cyc_Absynpp_ntyp2doc( _temp2466), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2470=( char*)" "; struct _tagged_string _temp2471; _temp2471.curr=
_temp2470; _temp2471.base= _temp2470; _temp2471.last_plus_one= _temp2470 + 2;
_temp2471;})), Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)({
struct Cyc_Core_Opt* _temp2472= dopt; if( _temp2472 == 0){ _throw(
Null_Exception);} _temp2472->v;}), _temp2464));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return Cyc_Absynpp_cat5(
Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2473=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2473->v=(
void*) Cyc_Absynpp_textptr( f->name); _temp2473;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2474=( char*)":"; struct _tagged_string _temp2475;
_temp2475.curr= _temp2474; _temp2475.base= _temp2474; _temp2475.last_plus_one=
_temp2474 + 2; _temp2475;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp2476= f->width; if( _temp2476 == 0){ _throw(
Null_Exception);} _temp2476->v;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2477=( char*)";"; struct _tagged_string
_temp2478; _temp2478.curr= _temp2477; _temp2478.base= _temp2477; _temp2478.last_plus_one=
_temp2477 + 2; _temp2478;})));} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2479=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2479->v=( void*) Cyc_Absynpp_textptr(
f->name); _temp2479;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2480=( char*)";"; struct _tagged_string
_temp2481; _temp2481.curr= _temp2480; _temp2481.base= _temp2480; _temp2481.last_plus_one=
_temp2480 + 2; _temp2481;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,( struct _tagged_string)({ char*
_temp2482=( char*)""; struct _tagged_string _temp2483; _temp2483.curr= _temp2482;
_temp2483.base= _temp2482; _temp2483.last_plus_one= _temp2482 + 1; _temp2483;}),
fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc( struct Cyc_Absyn_Tunionfield*
f){ return Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc(( void*) f->sc), Cyc_Absynpp_typedef_name2doc(
f->name), f->tvs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs), f->typs
== 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,( struct _tagged_string)({ char*
_temp2484=( char*)","; struct _tagged_string _temp2485; _temp2485.curr=
_temp2484; _temp2485.base= _temp2484; _temp2485.last_plus_one= _temp2484 + 2;
_temp2485;}), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl=({ struct Cyc_List_List*
_temp2486= tdl; if( _temp2486 == 0){ _throw( Null_Exception);} _temp2486->tl;})){
Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp2487= tdl; if( _temp2487 == 0){ _throw( Null_Exception);} _temp2487->hd;})),
72, f); fprintf( f,"\n");}} struct _tagged_string Cyc_Absynpp_decllist2string(
struct Cyc_List_List* tdl){ return Cyc_PP_string_of_doc( Cyc_PP_seql(( struct
_tagged_string)({ char* _temp2488=( char*)""; struct _tagged_string _temp2489;
_temp2489.curr= _temp2488; _temp2489.base= _temp2488; _temp2489.last_plus_one=
_temp2488 + 1; _temp2489;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
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
Cyc_Absynpp_pat2doc( p), 72);} struct _tagged_string Cyc_Absynpp_scope2string(
void* sc){ void* _temp2490= sc; _LL2492: if( _temp2490 ==( void*) Cyc_Absyn_Static){
goto _LL2493;} else{ goto _LL2494;} _LL2494: if( _temp2490 ==( void*) Cyc_Absyn_Public){
goto _LL2495;} else{ goto _LL2496;} _LL2496: if( _temp2490 ==( void*) Cyc_Absyn_Extern){
goto _LL2497;} else{ goto _LL2498;} _LL2498: if( _temp2490 ==( void*) Cyc_Absyn_ExternC){
goto _LL2499;} else{ goto _LL2500;} _LL2500: if( _temp2490 ==( void*) Cyc_Absyn_Abstract){
goto _LL2501;} else{ goto _LL2491;} _LL2493: return( struct _tagged_string)({
char* _temp2502=( char*)"static"; struct _tagged_string _temp2503; _temp2503.curr=
_temp2502; _temp2503.base= _temp2502; _temp2503.last_plus_one= _temp2502 + 7;
_temp2503;}); _LL2495: return( struct _tagged_string)({ char* _temp2504=( char*)"public";
struct _tagged_string _temp2505; _temp2505.curr= _temp2504; _temp2505.base=
_temp2504; _temp2505.last_plus_one= _temp2504 + 7; _temp2505;}); _LL2497: return(
struct _tagged_string)({ char* _temp2506=( char*)"extern"; struct _tagged_string
_temp2507; _temp2507.curr= _temp2506; _temp2507.base= _temp2506; _temp2507.last_plus_one=
_temp2506 + 7; _temp2507;}); _LL2499: return( struct _tagged_string)({ char*
_temp2508=( char*)"extern \"C\""; struct _tagged_string _temp2509; _temp2509.curr=
_temp2508; _temp2509.base= _temp2508; _temp2509.last_plus_one= _temp2508 + 11;
_temp2509;}); _LL2501: return( struct _tagged_string)({ char* _temp2510=( char*)"abstract";
struct _tagged_string _temp2511; _temp2511.curr= _temp2510; _temp2511.base=
_temp2510; _temp2511.last_plus_one= _temp2510 + 9; _temp2511;}); _LL2491:;}