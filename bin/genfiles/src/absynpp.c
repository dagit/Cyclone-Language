#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple4{
struct Cyc_Absyn_Tqual* f1; void* f2; struct Cyc_List_List* f3; } ; struct
_tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct _tuple6{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; struct _tuple7{ struct
Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple8{ void* f1; void* f2; } ;
struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; typedef
int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
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
_tagged_string Cyc_Core_new_string( int); extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern struct _tagged_string Cyc_Core_string_of_int(
int); extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern void Cyc_List_iter(
void(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; extern int Cyc_List_exists( int(*
pred)( void*), struct Cyc_List_List* x); extern int Cyc_List_list_cmp( int(* cmp)(
void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); extern int
Cyc_List_list_prefix( int(* cmp)( void*, void*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate*
Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t;
struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern void Cyc_PP_file_of_doc(
struct Cyc_PP_Doc* d, int w, struct Cyc_Stdio___sFILE* f); extern struct
_tagged_string Cyc_PP_string_of_doc( struct Cyc_PP_Doc* d, int w); extern struct
Cyc_PP_Doc* Cyc_PP_text( struct _tagged_string s); extern struct Cyc_PP_Doc* Cyc_PP_nil_doc();
extern struct Cyc_PP_Doc* Cyc_PP_line_doc(); extern struct Cyc_PP_Doc* Cyc_PP_nest(
int k, struct Cyc_PP_Doc* d); extern struct Cyc_PP_Doc* Cyc_PP_concat( struct
Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2); extern struct Cyc_PP_Doc* Cyc_PP_ppseq(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l0); extern struct Cyc_PP_Doc* Cyc_PP_seq( struct _tagged_string sep, struct Cyc_List_List*
l0); extern struct Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc*
Cyc_PP_seql( struct _tagged_string sep, struct Cyc_List_List* l0); struct Cyc_Lineno_Pos{
struct _tagged_string logical_file; struct _tagged_string line; int line_no; int
col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
static const unsigned int Cyc_Position_Lex= 0; static const unsigned int Cyc_Position_Parse=
1; static const unsigned int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; typedef struct _tagged_string* Cyc_Absyn_field_name_t; typedef
struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t; typedef
struct _tuple0* Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; typedef void* Cyc_Absyn_Scope; struct Cyc_Absyn_Tqual;
typedef void* Cyc_Absyn_Size_of; typedef void* Cyc_Absyn_Kind; typedef void* Cyc_Absyn_Array_kind;
typedef void* Cyc_Absyn_Sign; struct Cyc_Absyn_Conref; typedef void* Cyc_Absyn_Constraint;
typedef void* Cyc_Absyn_Bounds; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_FnInfo;
struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_XTunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_XTunionFieldInfo; typedef void* Cyc_Absyn_Type; typedef void*
Cyc_Absyn_Funcparams; typedef void* Cyc_Absyn_Type_modifier; typedef void* Cyc_Absyn_Cnst;
typedef void* Cyc_Absyn_Primop; typedef void* Cyc_Absyn_Incrementor; typedef
void* Cyc_Absyn_Raw_exp; struct Cyc_Absyn_Exp; typedef void* Cyc_Absyn_Raw_stmt;
struct Cyc_Absyn_Stmt; typedef void* Cyc_Absyn_Raw_pat; struct Cyc_Absyn_Pat;
typedef void* Cyc_Absyn_Binding; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_Fndecl;
struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl;
struct Cyc_Absyn_XTuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
typedef void* Cyc_Absyn_Raw_decl; struct Cyc_Absyn_Decl; typedef void* Cyc_Absyn_Designator;
typedef struct _xtunion_struct* Cyc_Absyn_StmtAnnot; typedef void* Cyc_Absyn_Attribute;
struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t; typedef struct
Cyc_Absyn_Tqual* Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t; typedef
void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t; typedef
void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t;
typedef struct Cyc_Absyn_XTunionInfo Cyc_Absyn_xtunion_info_t; typedef struct
Cyc_Absyn_TunionFieldInfo Cyc_Absyn_tunion_field_info_t; typedef struct Cyc_Absyn_XTunionFieldInfo
Cyc_Absyn_xtunion_field_info_t; typedef void* Cyc_Absyn_type_t; typedef void*
Cyc_Absyn_funcparams_t; typedef void* Cyc_Absyn_type_modifier_t; typedef void*
Cyc_Absyn_cnst_t; typedef void* Cyc_Absyn_primop_t; typedef void* Cyc_Absyn_incrementor_t;
typedef void* Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_t; typedef struct Cyc_Absyn_Stmt*
Cyc_Absyn_stmt_opt_t; typedef void* Cyc_Absyn_raw_pat_t; typedef struct Cyc_Absyn_Pat*
Cyc_Absyn_pat_t; typedef void* Cyc_Absyn_binding_t; typedef struct Cyc_Absyn_Switch_clause*
Cyc_Absyn_switch_clause_t; typedef struct Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl_t; typedef struct Cyc_Absyn_Uniondecl*
Cyc_Absyn_uniondecl_t; typedef struct Cyc_Absyn_Tunionfield* Cyc_Absyn_tunionfield_t;
typedef struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_tuniondecl_t; typedef struct Cyc_Absyn_XTuniondecl*
Cyc_Absyn_xtuniondecl_t; typedef struct Cyc_Absyn_Typedefdecl* Cyc_Absyn_typedefdecl_t;
typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield_t; typedef struct Cyc_Absyn_Enumdecl*
Cyc_Absyn_enumdecl_t; typedef struct Cyc_Absyn_Vardecl* Cyc_Absyn_vardecl_t;
typedef void* Cyc_Absyn_raw_decl_t; typedef struct Cyc_Absyn_Decl* Cyc_Absyn_decl_t;
typedef void* Cyc_Absyn_designator_t; typedef struct _xtunion_struct* Cyc_Absyn_stmt_annot_t;
typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List* Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t; static const
unsigned int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n_tag= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n_tag= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const unsigned int Cyc_Absyn_Static= 0; static const unsigned int
Cyc_Absyn_Abstract= 1; static const unsigned int Cyc_Absyn_Public= 2; static
const unsigned int Cyc_Absyn_Extern= 3; static const unsigned int Cyc_Absyn_ExternC=
4; struct Cyc_Absyn_Tqual{ int q_const: 1; int q_volatile: 1; int q_restrict: 1;
} ; static const unsigned int Cyc_Absyn_B1= 0; static const unsigned int Cyc_Absyn_B2=
1; static const unsigned int Cyc_Absyn_B4= 2; static const unsigned int Cyc_Absyn_B8=
3; static const unsigned int Cyc_Absyn_AnyKind= 0; static const unsigned int Cyc_Absyn_MemKind=
1; static const unsigned int Cyc_Absyn_BoxKind= 2; static const unsigned int Cyc_Absyn_RgnKind=
3; static const unsigned int Cyc_Absyn_EffKind= 4; static const unsigned int Cyc_Absyn_Signed=
0; static const unsigned int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{
void* v; } ; static const int Cyc_Absyn_Eq_constr_tag= 0; struct Cyc_Absyn_Eq_constr_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr_tag= 1; struct
Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1; } ;
static const unsigned int Cyc_Absyn_No_constr= 0; struct Cyc_Absyn_Tvar{ struct
_tagged_string* name; struct Cyc_Absyn_Conref* kind; } ; static const
unsigned int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b_tag= 0;
struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual* tq; struct Cyc_Absyn_Conref* bounds; } ;
struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt*
effect; void* ret_typ; struct Cyc_List_List* args; int varargs; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_TunionInfo{ struct _tuple0* name; struct Cyc_List_List*
targs; void* rgn; struct Cyc_Absyn_Tuniondecl* tud; } ; struct Cyc_Absyn_XTunionInfo{
struct _tuple0* name; void* rgn; struct Cyc_Absyn_XTuniondecl* xtud; } ; struct
Cyc_Absyn_TunionFieldInfo{ struct _tuple0* name; struct Cyc_List_List* targs;
struct _tuple0* fname; struct Cyc_Absyn_Tuniondecl* tud; struct Cyc_Absyn_Tunionfield*
tufd; } ; struct Cyc_Absyn_XTunionFieldInfo{ struct _tuple0* name; struct
_tuple0* fname; struct Cyc_Absyn_XTuniondecl* xtud; struct Cyc_Absyn_Tunionfield*
xtufd; } ; static const unsigned int Cyc_Absyn_VoidType= 0; static const int Cyc_Absyn_Evar_tag=
0; struct Cyc_Absyn_Evar_struct{ int tag; void* f1; struct Cyc_Core_Opt* f2; int
f3; } ; static const int Cyc_Absyn_VarType_tag= 1; struct Cyc_Absyn_VarType_struct{
int tag; struct Cyc_Absyn_Tvar* f1; } ; static const int Cyc_Absyn_TunionType_tag=
2; struct Cyc_Absyn_TunionType_struct{ int tag; struct Cyc_Absyn_TunionInfo f1;
} ; static const int Cyc_Absyn_XTunionType_tag= 3; struct Cyc_Absyn_XTunionType_struct{
int tag; struct Cyc_Absyn_XTunionInfo f1; } ; static const int Cyc_Absyn_TunionFieldType_tag=
4; struct Cyc_Absyn_TunionFieldType_struct{ int tag; struct Cyc_Absyn_TunionFieldInfo
f1; } ; static const int Cyc_Absyn_XTunionFieldType_tag= 5; struct Cyc_Absyn_XTunionFieldType_struct{
int tag; struct Cyc_Absyn_XTunionFieldInfo f1; } ; static const int Cyc_Absyn_PointerType_tag=
6; struct Cyc_Absyn_PointerType_struct{ int tag; struct Cyc_Absyn_PtrInfo f1; }
; static const int Cyc_Absyn_IntType_tag= 7; struct Cyc_Absyn_IntType_struct{
int tag; void* f1; void* f2; } ; static const unsigned int Cyc_Absyn_FloatType=
1; static const unsigned int Cyc_Absyn_DoubleType= 2; static const int Cyc_Absyn_ArrayType_tag=
8; struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual*
f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_FnType_tag= 9;
struct Cyc_Absyn_FnType_struct{ int tag; struct Cyc_Absyn_FnInfo f1; } ; static
const int Cyc_Absyn_TupleType_tag= 10; struct Cyc_Absyn_TupleType_struct{ int
tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_StructType_tag= 11;
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType_tag=
12; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType_tag=
13; struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType_tag= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType_tag= 15; struct
Cyc_Absyn_TypedefType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Core_Opt* f3; } ; static const unsigned int Cyc_Absyn_HeapRgn= 3;
static const int Cyc_Absyn_AccessEff_tag= 16; struct Cyc_Absyn_AccessEff_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff_tag= 17; struct Cyc_Absyn_JoinEff_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_NoTypes_tag= 0;
struct Cyc_Absyn_NoTypes_struct{ int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment*
f2; } ; static const int Cyc_Absyn_WithTypes_tag= 1; struct Cyc_Absyn_WithTypes_struct{
int tag; struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ; typedef
void* Cyc_Absyn_Pointer_Sort; static const int Cyc_Absyn_NonNullable_ps_tag= 0;
struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Nullable_ps_tag= 1; struct Cyc_Absyn_Nullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const unsigned int Cyc_Absyn_TaggedArray_ps=
0; static const int Cyc_Absyn_Regparm_att_tag= 0; struct Cyc_Absyn_Regparm_att_struct{
int tag; int f1; } ; static const unsigned int Cyc_Absyn_Stdcall_att= 0; static
const unsigned int Cyc_Absyn_Cdecl_att= 1; static const unsigned int Cyc_Absyn_Noreturn_att=
2; static const unsigned int Cyc_Absyn_Const_att= 3; static const int Cyc_Absyn_Aligned_att_tag=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const
unsigned int Cyc_Absyn_Packed_att= 4; static const int Cyc_Absyn_Section_att_tag=
2; struct Cyc_Absyn_Section_att_struct{ int tag; struct _tagged_string f1; } ;
static const unsigned int Cyc_Absyn_Nocommon_att= 5; static const unsigned int
Cyc_Absyn_Shared_att= 6; static const unsigned int Cyc_Absyn_Unused_att= 7;
static const unsigned int Cyc_Absyn_Weak_att= 8; static const unsigned int Cyc_Absyn_Dllimport_att=
9; static const unsigned int Cyc_Absyn_Dllexport_att= 10; static const
unsigned int Cyc_Absyn_No_instrument_function_att= 11; static const unsigned int
Cyc_Absyn_Constructor_att= 12; static const unsigned int Cyc_Absyn_Destructor_att=
13; static const unsigned int Cyc_Absyn_No_check_memory_usage_att= 14; static
const unsigned int Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod_tag=
0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; }
; static const int Cyc_Absyn_Pointer_mod_tag= 1; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual* f3; } ; static const int
Cyc_Absyn_Function_mod_tag= 2; struct Cyc_Absyn_Function_mod_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_TypeParams_mod_tag= 3; struct Cyc_Absyn_TypeParams_mod_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ;
static const int Cyc_Absyn_Attributes_mod_tag= 4; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; static
const int Cyc_Absyn_Char_c_tag= 0; struct Cyc_Absyn_Char_c_struct{ int tag; void*
f1; char f2; } ; static const int Cyc_Absyn_Short_c_tag= 1; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; static const int Cyc_Absyn_Int_c_tag= 2; struct
Cyc_Absyn_Int_c_struct{ int tag; void* f1; int f2; } ; static const int Cyc_Absyn_LongLong_c_tag=
3; struct Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ;
static const int Cyc_Absyn_Float_c_tag= 4; struct Cyc_Absyn_Float_c_struct{ int
tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_String_c_tag= 5;
struct Cyc_Absyn_String_c_struct{ int tag; struct _tagged_string f1; } ; static
const unsigned int Cyc_Absyn_Null_c= 0; static const unsigned int Cyc_Absyn_Plus=
0; static const unsigned int Cyc_Absyn_Times= 1; static const unsigned int Cyc_Absyn_Minus=
2; static const unsigned int Cyc_Absyn_Div= 3; static const unsigned int Cyc_Absyn_Mod=
4; static const unsigned int Cyc_Absyn_Eq= 5; static const unsigned int Cyc_Absyn_Neq=
6; static const unsigned int Cyc_Absyn_Gt= 7; static const unsigned int Cyc_Absyn_Lt=
8; static const unsigned int Cyc_Absyn_Gte= 9; static const unsigned int Cyc_Absyn_Lte=
10; static const unsigned int Cyc_Absyn_Not= 11; static const unsigned int Cyc_Absyn_Bitnot=
12; static const unsigned int Cyc_Absyn_Bitand= 13; static const unsigned int
Cyc_Absyn_Bitor= 14; static const unsigned int Cyc_Absyn_Bitxor= 15; static
const unsigned int Cyc_Absyn_Bitlshift= 16; static const unsigned int Cyc_Absyn_Bitlrshift=
17; static const unsigned int Cyc_Absyn_Bitarshift= 18; static const
unsigned int Cyc_Absyn_Size= 19; static const unsigned int Cyc_Absyn_Printf= 20;
static const unsigned int Cyc_Absyn_Fprintf= 21; static const unsigned int Cyc_Absyn_Xprintf=
22; static const unsigned int Cyc_Absyn_Scanf= 23; static const unsigned int Cyc_Absyn_Fscanf=
24; static const unsigned int Cyc_Absyn_Sscanf= 25; static const unsigned int
Cyc_Absyn_PreInc= 0; static const unsigned int Cyc_Absyn_PostInc= 1; static
const unsigned int Cyc_Absyn_PreDec= 2; static const unsigned int Cyc_Absyn_PostDec=
3; static const int Cyc_Absyn_Const_e_tag= 0; struct Cyc_Absyn_Const_e_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_Var_e_tag= 1; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple0* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e_tag=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_Primop_e_tag= 3; struct Cyc_Absyn_Primop_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_AssignOp_e_tag=
4; struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Increment_e_tag=
5; struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void*
f2; } ; static const int Cyc_Absyn_Conditional_e_tag= 6; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_SeqExp_e_tag= 7; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_UnknownCall_e_tag= 8; struct Cyc_Absyn_UnknownCall_e_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Absyn_FnCall_e_tag= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Throw_e_tag= 10;
struct Cyc_Absyn_Throw_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_NoInstantiate_e_tag= 11; struct Cyc_Absyn_NoInstantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Instantiate_e_tag=
12; struct Cyc_Absyn_Instantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Cast_e_tag= 13; struct
Cyc_Absyn_Cast_e_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; static
const int Cyc_Absyn_Address_e_tag= 14; struct Cyc_Absyn_Address_e_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_New_e_tag= 15;
struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Sizeoftyp_e_tag= 16; struct Cyc_Absyn_Sizeoftyp_e_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_Sizeofexp_e_tag= 17; struct
Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Deref_e_tag= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e_tag= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e_tag= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e_tag= 21; struct
Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp*
f2; } ; static const int Cyc_Absyn_Tuple_e_tag= 22; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e_tag=
23; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e_tag= 24; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e_tag=
25; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e_tag=
26; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_Tunion_e_tag= 27; struct Cyc_Absyn_Tunion_e_struct{ int tag;
struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ; static
const int Cyc_Absyn_XTunion_e_tag= 28; struct Cyc_Absyn_XTunion_e_struct{ int
tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_XTuniondecl*
f3; struct Cyc_Absyn_Tunionfield* f4; } ; static const int Cyc_Absyn_Enum_e_tag=
29; struct Cyc_Absyn_Enum_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; struct Cyc_Absyn_Enumfield* f3; } ; static const int Cyc_Absyn_Malloc_e_tag=
30; struct Cyc_Absyn_Malloc_e_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_UnresolvedMem_e_tag= 31; struct Cyc_Absyn_UnresolvedMem_e_struct{ int
tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e_tag=
32; struct Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Codegen_e_tag= 33; struct Cyc_Absyn_Codegen_e_struct{
int tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e_tag=
34; struct Cyc_Absyn_Fill_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt* topt; void* r; struct Cyc_Position_Segment*
loc; } ; static const unsigned int Cyc_Absyn_Skip_s= 0; static const int Cyc_Absyn_Exp_s_tag=
0; struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Seq_s_tag= 1; struct Cyc_Absyn_Seq_s_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Return_s_tag=
2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_IfThenElse_s_tag= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s_tag= 4; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s_tag=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s_tag= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s_tag= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s_tag= 8; struct Cyc_Absyn_For_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct
Cyc_Absyn_Stmt* f4; } ; static const int Cyc_Absyn_Switch_s_tag= 9; struct Cyc_Absyn_Switch_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Fallthru_s_tag= 10; struct Cyc_Absyn_Fallthru_s_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2; } ; static const
int Cyc_Absyn_Decl_s_tag= 11; struct Cyc_Absyn_Decl_s_struct{ int tag; struct
Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Cut_s_tag=
12; struct Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Splice_s_tag= 13; struct Cyc_Absyn_Splice_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Label_s_tag=
14; struct Cyc_Absyn_Label_s_struct{ int tag; struct _tagged_string* f1; struct
Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s_tag= 15; struct Cyc_Absyn_Do_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct _tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s_tag=
16; struct Cyc_Absyn_TryCatch_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;
struct Cyc_List_List* f2; } ; struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment*
loc; struct Cyc_List_List* non_local_preds; int try_depth; struct
_xtunion_struct* annot; } ; static const unsigned int Cyc_Absyn_Wild_p= 0;
static const int Cyc_Absyn_Var_p_tag= 0; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const unsigned int Cyc_Absyn_Null_p= 1;
static const int Cyc_Absyn_Int_p_tag= 1; struct Cyc_Absyn_Int_p_struct{ int tag;
void* f1; int f2; } ; static const int Cyc_Absyn_Char_p_tag= 2; struct Cyc_Absyn_Char_p_struct{
int tag; char f1; } ; static const int Cyc_Absyn_Float_p_tag= 3; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Tuple_p_tag= 4;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Pointer_p_tag= 5; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; static const int Cyc_Absyn_Reference_p_tag= 6;
struct Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Struct_p_tag= 7; struct Cyc_Absyn_Struct_p_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Tunion_p_tag= 8;
struct Cyc_Absyn_Tunion_p_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Tuniondecl*
f5; struct Cyc_Absyn_Tunionfield* f6; } ; static const int Cyc_Absyn_XTunion_p_tag=
9; struct Cyc_Absyn_XTunion_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_XTuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_p_tag= 10; struct Cyc_Absyn_Enum_p_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_UnknownId_p_tag= 11; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_UnknownCall_p_tag=
12; struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple0* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; static const int Cyc_Absyn_UnknownFields_p_tag=
13; struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple0* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_Absyn_Pat{ void* r;
struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat* pattern; struct Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp*
where_clause; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment* loc; } ;
static const unsigned int Cyc_Absyn_Unresolved_b= 0; static const int Cyc_Absyn_Global_b_tag=
0; struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Funname_b_tag= 1; struct Cyc_Absyn_Funname_b_struct{
int tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Param_b_tag=
2; struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Local_b_tag= 3; struct Cyc_Absyn_Local_b_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Pat_b_tag= 4;
struct Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct
Cyc_Absyn_Vardecl{ void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual* tq;
void* type; struct Cyc_Absyn_Exp* initializer; int shadow; struct Cyc_Core_Opt*
region; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc;
int is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_Core_Opt*
width; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structdecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Uniondecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Tunionfield{
struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_List_List* typs;
struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{ void* sc;
struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_XTuniondecl{ void* sc; struct _tuple0* name; struct
Cyc_List_List* fields; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; static const
int Cyc_Absyn_Var_d_tag= 0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Fn_d_tag= 1; struct Cyc_Absyn_Fn_d_struct{
int tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Let_d_tag=
2; struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; static const
int Cyc_Absyn_Struct_d_tag= 3; struct Cyc_Absyn_Struct_d_struct{ int tag; struct
Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Absyn_Union_d_tag= 4; struct
Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ; static
const int Cyc_Absyn_Tunion_d_tag= 5; struct Cyc_Absyn_Tunion_d_struct{ int tag;
struct Cyc_Absyn_Tuniondecl* f1; } ; static const int Cyc_Absyn_XTunion_d_tag= 6;
struct Cyc_Absyn_XTunion_d_struct{ int tag; struct Cyc_Absyn_XTuniondecl* f1; }
; static const int Cyc_Absyn_Enum_d_tag= 7; struct Cyc_Absyn_Enum_d_struct{ int
tag; struct Cyc_Absyn_Enumdecl* f1; } ; static const int Cyc_Absyn_Typedef_d_tag=
8; struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1;
} ; static const int Cyc_Absyn_Namespace_d_tag= 9; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d_tag= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d_tag=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement_tag= 0; struct Cyc_Absyn_ArrayElement_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName_tag=
1; struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot_tag[ 11u]; struct Cyc_Absyn_EmptyAnnot_struct{
char* tag; } ; extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern int Cyc_Absyn_is_format_prim( void* p); extern struct _tagged_string Cyc_Absyn_attribute2string(
void*); struct Cyc_Buffer_t; typedef struct Cyc_Buffer_t* Cyc_Buffer_T; extern
int Cyc_String_strptrcmp( struct _tagged_string* s1, struct _tagged_string* s2);
extern struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*,
struct _tagged_string); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; }
; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u];
struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; typedef void* Cyc_Tcenv_Resolved; static const int Cyc_Tcenv_VarRes_tag= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes_tag=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes_tag= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_XTunionRes_tag= 3; struct Cyc_Tcenv_XTunionRes_struct{
int tag; struct Cyc_Absyn_XTuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes_tag= 4; struct Cyc_Tcenv_EnumRes_struct{ int
tag; struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; typedef
void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* uniondecls;
struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* xtuniondecls; struct
Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict*
ordinaries; struct Cyc_List_List* availables; } ; typedef struct Cyc_Tcenv_Genv*
Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv; typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t;
typedef void* Cyc_Tcenv_Jumpee; static const unsigned int Cyc_Tcenv_NotLoop_j= 0;
static const unsigned int Cyc_Tcenv_CaseEnd_j= 1; static const unsigned int Cyc_Tcenv_FnEnd_j=
2; static const int Cyc_Tcenv_Stmt_j_tag= 0; struct Cyc_Tcenv_Stmt_j_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; typedef void* Cyc_Tcenv_jumpee_t; typedef
void* Cyc_Tcenv_Frames; static const int Cyc_Tcenv_Outermost_tag= 0; struct Cyc_Tcenv_Outermost_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_Frame_tag= 1; struct Cyc_Tcenv_Frame_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden_tag= 2;
struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void* f2; } ; typedef void*
Cyc_Tcenv_frames_t; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List* ns; struct Cyc_Dict_Dict*
ae; struct Cyc_Core_Opt* le; } ; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t;
extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{ char*
tag; } ; extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_is_temp_tvar(
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
extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); extern struct
_tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); extern struct _tuple4 Cyc_Absynpp_to_tms(
struct Cyc_Absyn_Tqual* tq, void* t); extern struct Cyc_PP_Doc* Cyc_Absynpp_egroup(
struct _tagged_string start, struct _tagged_string stop, struct _tagged_string
sep, struct Cyc_List_List* ss); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat12(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat11( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat10( struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat9( struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat8( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat7(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_cat6( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_cat5( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat4(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat3( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat2( struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct
_tuple5* dp); extern struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc( struct
Cyc_List_List* cs); extern struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List*
fields); extern struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*); extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e); extern struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct
_tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de); extern struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual*
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
int)(({ struct _tagged_string _temp50= s;( unsigned int)( _temp50.last_plus_one
- _temp50.curr);}) - 1);{ int i= 0; for( 0; i < sz; i ++){ char c=({ struct
_tagged_string _temp51= s; char* _temp53= _temp51.curr + i; if( _temp51.base ==
0? 1:( _temp53 < _temp51.base? 1: _temp53 >= _temp51.last_plus_one)){ _throw(
Null_Exception);}* _temp53;}); if((( c <=' '? 1: c >='~')? 1: c =='"')? 1: c =='\\'){
return 1;}}} return 0;} struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string s){ if( ! Cyc_Absynpp_special( s)){ return s;}{ int n=( int)(({
struct _tagged_string _temp54= s;( unsigned int)( _temp54.last_plus_one -
_temp54.curr);}) - 1); if( n > 0?({ struct _tagged_string _temp55= s; char*
_temp57= _temp55.curr + n; if( _temp55.base == 0? 1:( _temp57 < _temp55.base? 1:
_temp57 >= _temp55.last_plus_one)){ _throw( Null_Exception);}* _temp57;}) =='\000':
0){ n --;}{ int len= 0;{ int i= 0; for( 0; i <= n; i ++){ char _temp61=({ struct
_tagged_string _temp58= s; char* _temp60= _temp58.curr + i; if( _temp58.base ==
0? 1:( _temp60 < _temp58.base? 1: _temp60 >= _temp58.last_plus_one)){ _throw(
Null_Exception);}* _temp60;}); _LL63: if( _temp61 =='\a'){ goto _LL64;} else{
goto _LL65;} _LL65: if( _temp61 =='\b'){ goto _LL66;} else{ goto _LL67;} _LL67:
if( _temp61 =='\f'){ goto _LL68;} else{ goto _LL69;} _LL69: if( _temp61 =='\n'){
goto _LL70;} else{ goto _LL71;} _LL71: if( _temp61 =='\r'){ goto _LL72;} else{
goto _LL73;} _LL73: if( _temp61 =='\t'){ goto _LL74;} else{ goto _LL75;} _LL75:
if( _temp61 =='\v'){ goto _LL76;} else{ goto _LL77;} _LL77: if( _temp61 =='\\'){
goto _LL78;} else{ goto _LL79;} _LL79: if( _temp61 =='"'){ goto _LL80;} else{
goto _LL81;} _LL81: goto _LL82; _LL64: goto _LL66; _LL66: goto _LL68; _LL68:
goto _LL70; _LL70: goto _LL72; _LL72: goto _LL74; _LL74: goto _LL76; _LL76: goto
_LL78; _LL78: goto _LL80; _LL80: len += 2; goto _LL62; _LL82: if( _temp61 >=' '?
_temp61 <='~': 0){ len ++;} else{ len += 4;} goto _LL62; _LL62:;}}{ struct
_tagged_string t= Cyc_Core_new_string( len); int j= 0;{ int i= 0; for( 0; i <= n;
i ++){ char _temp86=({ struct _tagged_string _temp83= s; char* _temp85= _temp83.curr
+ i; if( _temp83.base == 0? 1:( _temp85 < _temp83.base? 1: _temp85 >= _temp83.last_plus_one)){
_throw( Null_Exception);}* _temp85;}); _LL88: if( _temp86 =='\a'){ goto _LL89;}
else{ goto _LL90;} _LL90: if( _temp86 =='\b'){ goto _LL91;} else{ goto _LL92;}
_LL92: if( _temp86 =='\f'){ goto _LL93;} else{ goto _LL94;} _LL94: if( _temp86
=='\n'){ goto _LL95;} else{ goto _LL96;} _LL96: if( _temp86 =='\r'){ goto _LL97;}
else{ goto _LL98;} _LL98: if( _temp86 =='\t'){ goto _LL99;} else{ goto _LL100;}
_LL100: if( _temp86 =='\v'){ goto _LL101;} else{ goto _LL102;} _LL102: if(
_temp86 =='\\'){ goto _LL103;} else{ goto _LL104;} _LL104: if( _temp86 =='"'){
goto _LL105;} else{ goto _LL106;} _LL106: goto _LL107; _LL89:({ struct
_tagged_string _temp108= t; char* _temp110= _temp108.curr +( j ++); if( _temp108.base
== 0? 1:( _temp110 < _temp108.base? 1: _temp110 >= _temp108.last_plus_one)){
_throw( Null_Exception);}* _temp110='\\';});({ struct _tagged_string _temp111= t;
char* _temp113= _temp111.curr +( j ++); if( _temp111.base == 0? 1:( _temp113 <
_temp111.base? 1: _temp113 >= _temp111.last_plus_one)){ _throw( Null_Exception);}*
_temp113='a';}); goto _LL87; _LL91:({ struct _tagged_string _temp114= t; char*
_temp116= _temp114.curr +( j ++); if( _temp114.base == 0? 1:( _temp116 <
_temp114.base? 1: _temp116 >= _temp114.last_plus_one)){ _throw( Null_Exception);}*
_temp116='\\';});({ struct _tagged_string _temp117= t; char* _temp119= _temp117.curr
+( j ++); if( _temp117.base == 0? 1:( _temp119 < _temp117.base? 1: _temp119 >=
_temp117.last_plus_one)){ _throw( Null_Exception);}* _temp119='b';}); goto _LL87;
_LL93:({ struct _tagged_string _temp120= t; char* _temp122= _temp120.curr +( j
++); if( _temp120.base == 0? 1:( _temp122 < _temp120.base? 1: _temp122 >=
_temp120.last_plus_one)){ _throw( Null_Exception);}* _temp122='\\';});({ struct
_tagged_string _temp123= t; char* _temp125= _temp123.curr +( j ++); if( _temp123.base
== 0? 1:( _temp125 < _temp123.base? 1: _temp125 >= _temp123.last_plus_one)){
_throw( Null_Exception);}* _temp125='f';}); goto _LL87; _LL95:({ struct
_tagged_string _temp126= t; char* _temp128= _temp126.curr +( j ++); if( _temp126.base
== 0? 1:( _temp128 < _temp126.base? 1: _temp128 >= _temp126.last_plus_one)){
_throw( Null_Exception);}* _temp128='\\';});({ struct _tagged_string _temp129= t;
char* _temp131= _temp129.curr +( j ++); if( _temp129.base == 0? 1:( _temp131 <
_temp129.base? 1: _temp131 >= _temp129.last_plus_one)){ _throw( Null_Exception);}*
_temp131='n';}); goto _LL87; _LL97:({ struct _tagged_string _temp132= t; char*
_temp134= _temp132.curr +( j ++); if( _temp132.base == 0? 1:( _temp134 <
_temp132.base? 1: _temp134 >= _temp132.last_plus_one)){ _throw( Null_Exception);}*
_temp134='\\';});({ struct _tagged_string _temp135= t; char* _temp137= _temp135.curr
+( j ++); if( _temp135.base == 0? 1:( _temp137 < _temp135.base? 1: _temp137 >=
_temp135.last_plus_one)){ _throw( Null_Exception);}* _temp137='r';}); goto _LL87;
_LL99:({ struct _tagged_string _temp138= t; char* _temp140= _temp138.curr +( j
++); if( _temp138.base == 0? 1:( _temp140 < _temp138.base? 1: _temp140 >=
_temp138.last_plus_one)){ _throw( Null_Exception);}* _temp140='\\';});({ struct
_tagged_string _temp141= t; char* _temp143= _temp141.curr +( j ++); if( _temp141.base
== 0? 1:( _temp143 < _temp141.base? 1: _temp143 >= _temp141.last_plus_one)){
_throw( Null_Exception);}* _temp143='t';}); goto _LL87; _LL101:({ struct
_tagged_string _temp144= t; char* _temp146= _temp144.curr +( j ++); if( _temp144.base
== 0? 1:( _temp146 < _temp144.base? 1: _temp146 >= _temp144.last_plus_one)){
_throw( Null_Exception);}* _temp146='\\';});({ struct _tagged_string _temp147= t;
char* _temp149= _temp147.curr +( j ++); if( _temp147.base == 0? 1:( _temp149 <
_temp147.base? 1: _temp149 >= _temp147.last_plus_one)){ _throw( Null_Exception);}*
_temp149='v';}); goto _LL87; _LL103:({ struct _tagged_string _temp150= t; char*
_temp152= _temp150.curr +( j ++); if( _temp150.base == 0? 1:( _temp152 <
_temp150.base? 1: _temp152 >= _temp150.last_plus_one)){ _throw( Null_Exception);}*
_temp152='\\';});({ struct _tagged_string _temp153= t; char* _temp155= _temp153.curr
+( j ++); if( _temp153.base == 0? 1:( _temp155 < _temp153.base? 1: _temp155 >=
_temp153.last_plus_one)){ _throw( Null_Exception);}* _temp155='\\';}); goto
_LL87; _LL105:({ struct _tagged_string _temp156= t; char* _temp158= _temp156.curr
+( j ++); if( _temp156.base == 0? 1:( _temp158 < _temp156.base? 1: _temp158 >=
_temp156.last_plus_one)){ _throw( Null_Exception);}* _temp158='\\';});({ struct
_tagged_string _temp159= t; char* _temp161= _temp159.curr +( j ++); if( _temp159.base
== 0? 1:( _temp161 < _temp159.base? 1: _temp161 >= _temp159.last_plus_one)){
_throw( Null_Exception);}* _temp161='"';}); goto _LL87; _LL107: if( _temp86 >=' '?
_temp86 <='~': 0){({ struct _tagged_string _temp162= t; char* _temp164= _temp162.curr
+( j ++); if( _temp162.base == 0? 1:( _temp164 < _temp162.base? 1: _temp164 >=
_temp162.last_plus_one)){ _throw( Null_Exception);}* _temp164= _temp86;});}
else{({ struct _tagged_string _temp165= t; char* _temp167= _temp165.curr +( j ++);
if( _temp165.base == 0? 1:( _temp167 < _temp165.base? 1: _temp167 >= _temp165.last_plus_one)){
_throw( Null_Exception);}* _temp167='\\';});({ struct _tagged_string _temp168= t;
char* _temp170= _temp168.curr +( j ++); if( _temp168.base == 0? 1:( _temp170 <
_temp168.base? 1: _temp170 >= _temp168.last_plus_one)){ _throw( Null_Exception);}*
_temp170=( char)('0' +( _temp86 >> 6 & 7));});({ struct _tagged_string _temp171=
t; char* _temp173= _temp171.curr +( j ++); if( _temp171.base == 0? 1:( _temp173
< _temp171.base? 1: _temp173 >= _temp171.last_plus_one)){ _throw( Null_Exception);}*
_temp173=( char)('0' +( _temp86 >> 3 & 7));});({ struct _tagged_string _temp174=
t; char* _temp176= _temp174.curr +( j ++); if( _temp174.base == 0? 1:( _temp176
< _temp174.base? 1: _temp176 >= _temp174.last_plus_one)){ _throw( Null_Exception);}*
_temp176=( char)('0' +( _temp86 & 7));});} goto _LL87; _LL87:;}} return t;}}}}
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
Cyc_Absynpp_const_string; struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual*
tq){ struct Cyc_List_List* l= 0; if( tq->q_restrict){ l=({ struct Cyc_List_List*
_temp186=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp186->hd=( void*) Cyc_Absynpp_restrict_sp; _temp186->tl= l; _temp186;});}
if( tq->q_volatile){ l=({ struct Cyc_List_List* _temp187=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp187->hd=( void*) Cyc_Absynpp_volatile_sp;
_temp187->tl= l; _temp187;});} if( tq->q_const){ l=({ struct Cyc_List_List*
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
k){ void* _temp195= k; _LL197: if(( int) _temp195 == Cyc_Absyn_AnyKind){ goto
_LL198;} else{ goto _LL199;} _LL199: if(( int) _temp195 == Cyc_Absyn_MemKind){
goto _LL200;} else{ goto _LL201;} _LL201: if(( int) _temp195 == Cyc_Absyn_BoxKind){
goto _LL202;} else{ goto _LL203;} _LL203: if(( int) _temp195 == Cyc_Absyn_RgnKind){
goto _LL204;} else{ goto _LL205;} _LL205: if(( int) _temp195 == Cyc_Absyn_EffKind){
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
_temp217 > 1u?(( struct _tunion_struct*) _temp217)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL224: _temp223=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp217)->f1;
goto _LL220;} else{ goto _LL221;} _LL221: goto _LL222; _LL220: return Cyc_Absynpp_kind2string(
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
_LL235: if(( unsigned int) _temp233 > 1u?(( struct _tunion_struct*) _temp233)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL242: _temp241=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp233)->f1; if(( int) _temp241 == Cyc_Absyn_BoxKind){ goto _LL236;} else{
goto _LL237;}} else{ goto _LL237;} _LL237: if(( unsigned int) _temp233 > 1u?((
struct _tunion_struct*) _temp233)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL244:
_temp243=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp233)->f1; goto
_LL238;} else{ goto _LL239;} _LL239: goto _LL240; _LL236: return Cyc_Absynpp_textptr(
tv->name); _LL238: return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( tv->name), Cyc_PP_text((
struct _tagged_string)({ char* _temp245=( char*)"::"; struct _tagged_string
_temp246; _temp246.curr= _temp245; _temp246.base= _temp245; _temp246.last_plus_one=
_temp245 + 3; _temp246;})), Cyc_Absynpp_kind2doc( _temp243)); _LL240: return Cyc_Absynpp_cat2(
Cyc_Absynpp_textptr( tv->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp247=( char*)"/*::?*/"; struct _tagged_string _temp248; _temp248.curr=
_temp247; _temp248.base= _temp247; _temp248.last_plus_one= _temp247 + 8;
_temp248;}))); _LL234:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc( struct Cyc_List_List*
tvs){ return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp249=( char*)"<";
struct _tagged_string _temp250; _temp250.curr= _temp249; _temp250.base= _temp249;
_temp250.last_plus_one= _temp249 + 2; _temp250;}),( struct _tagged_string)({
char* _temp251=( char*)">"; struct _tagged_string _temp252; _temp252.curr=
_temp251; _temp252.base= _temp251; _temp252.last_plus_one= _temp251 + 2;
_temp252;}),( struct _tagged_string)({ char* _temp253=( char*)","; struct
_tagged_string _temp254; _temp254.curr= _temp253; _temp254.base= _temp253;
_temp254.last_plus_one= _temp253 + 2; _temp254;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_string* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc(
tvs);} return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp255=(
char*)"<"; struct _tagged_string _temp256; _temp256.curr= _temp255; _temp256.base=
_temp255; _temp256.last_plus_one= _temp255 + 2; _temp256;}),( struct
_tagged_string)({ char* _temp257=( char*)">"; struct _tagged_string _temp258;
_temp258.curr= _temp257; _temp258.base= _temp257; _temp258.last_plus_one=
_temp257 + 2; _temp258;}),( struct _tagged_string)({ char* _temp259=( char*)",";
struct _tagged_string _temp260; _temp260.curr= _temp259; _temp260.base= _temp259;
_temp260.last_plus_one= _temp259 + 2; _temp260;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr,(( struct Cyc_List_List*(*)( struct _tagged_string*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_get_name,
tvs)));} struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc( struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((*
t).f1,(* t).f2, 0);} struct Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List*
ts){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp261=( char*)"(";
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
_temp275->hd;}); struct Cyc_Absyn_Tqual* _temp282; void* _temp284; void*
_temp286; _LL278: if(( unsigned int) _temp276 > 1u?(( struct _tunion_struct*)
_temp276)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL287: _temp286=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp276)->f1; goto _LL285; _LL285:
_temp284=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp276)->f2; goto
_LL283; _LL283: _temp282=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp276)->f3; goto _LL279;} else{ goto _LL280;} _LL280: goto _LL281; _LL279:
return 1; _LL281: return 0; _LL277:;}} extern struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc(
void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc( struct Cyc_PP_Doc* d, struct
Cyc_List_List* tms){ if( tms == 0){ return d;}{ struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc(
d,({ struct Cyc_List_List* _temp288= tms; if( _temp288 == 0){ _throw(
Null_Exception);} _temp288->tl;})); struct Cyc_PP_Doc* p_rest= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp289=( char*)"("; struct
_tagged_string _temp290; _temp290.curr= _temp289; _temp290.base= _temp289;
_temp290.last_plus_one= _temp289 + 2; _temp290;})), rest, Cyc_PP_text(( struct
_tagged_string)({ char* _temp291=( char*)")"; struct _tagged_string _temp292;
_temp292.curr= _temp291; _temp292.base= _temp291; _temp292.last_plus_one=
_temp291 + 2; _temp292;}))); void* _temp294=( void*)({ struct Cyc_List_List*
_temp293= tms; if( _temp293 == 0){ _throw( Null_Exception);} _temp293->hd;});
struct Cyc_Absyn_Exp* _temp308; void* _temp310; struct Cyc_List_List* _temp312;
struct Cyc_Position_Segment* _temp314; int _temp316; struct Cyc_Position_Segment*
_temp318; struct Cyc_List_List* _temp320; struct Cyc_Absyn_Tqual* _temp322; void*
_temp324; void* _temp326; _LL296: if(( int) _temp294 == Cyc_Absyn_Carray_mod){
goto _LL297;} else{ goto _LL298;} _LL298: if(( unsigned int) _temp294 > 1u?((
struct _tunion_struct*) _temp294)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){
_LL309: _temp308=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp294)->f1; goto _LL299;} else{ goto _LL300;} _LL300: if(( unsigned int)
_temp294 > 1u?(( struct _tunion_struct*) _temp294)->tag == Cyc_Absyn_Function_mod_tag:
0){ _LL311: _temp310=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp294)->f1;
goto _LL301;} else{ goto _LL302;} _LL302: if(( unsigned int) _temp294 > 1u?((
struct _tunion_struct*) _temp294)->tag == Cyc_Absyn_Attributes_mod_tag: 0){
_LL315: _temp314=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp294)->f1; goto _LL313; _LL313: _temp312=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Attributes_mod_struct*) _temp294)->f2; goto _LL303;} else{ goto _LL304;}
_LL304: if(( unsigned int) _temp294 > 1u?(( struct _tunion_struct*) _temp294)->tag
== Cyc_Absyn_TypeParams_mod_tag: 0){ _LL321: _temp320=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp294)->f1; goto _LL319; _LL319:
_temp318=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp294)->f2; goto _LL317; _LL317: _temp316=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp294)->f3; goto _LL305;} else{ goto _LL306;} _LL306: if(( unsigned int)
_temp294 > 1u?(( struct _tunion_struct*) _temp294)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL327: _temp326=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp294)->f1;
goto _LL325; _LL325: _temp324=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp294)->f2; goto _LL323; _LL323: _temp322=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp294)->f3; goto _LL307;} else{ goto _LL295;}
_LL297: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List* _temp328= tms;
if( _temp328 == 0){ _throw( Null_Exception);} _temp328->tl;}))){ rest= p_rest;}
return Cyc_Absynpp_cat2( rest, Cyc_PP_text(( struct _tagged_string)({ char*
_temp329=( char*)"[]"; struct _tagged_string _temp330; _temp330.curr= _temp329;
_temp330.base= _temp329; _temp330.last_plus_one= _temp329 + 3; _temp330;})));
_LL299: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List* _temp331= tms;
if( _temp331 == 0){ _throw( Null_Exception);} _temp331->tl;}))){ rest= p_rest;}
return Cyc_Absynpp_cat4( rest, Cyc_PP_text(( struct _tagged_string)({ char*
_temp332=( char*)"["; struct _tagged_string _temp333; _temp333.curr= _temp332;
_temp333.base= _temp332; _temp333.last_plus_one= _temp332 + 2; _temp333;})), Cyc_Absynpp_exp2doc(
_temp308), Cyc_PP_text(( struct _tagged_string)({ char* _temp334=( char*)"]";
struct _tagged_string _temp335; _temp335.curr= _temp334; _temp335.base= _temp334;
_temp335.last_plus_one= _temp334 + 2; _temp335;}))); _LL301: if( Cyc_Absynpp_next_is_pointer(({
struct Cyc_List_List* _temp336= tms; if( _temp336 == 0){ _throw( Null_Exception);}
_temp336->tl;}))){ rest= p_rest;}{ void* _temp337= _temp310; struct Cyc_Core_Opt*
_temp343; int _temp345; struct Cyc_List_List* _temp347; struct Cyc_Position_Segment*
_temp349; struct Cyc_List_List* _temp351; _LL339: if((( struct _tunion_struct*)
_temp337)->tag == Cyc_Absyn_WithTypes_tag){ _LL348: _temp347=( struct Cyc_List_List*)((
struct Cyc_Absyn_WithTypes_struct*) _temp337)->f1; goto _LL346; _LL346: _temp345=(
int)(( struct Cyc_Absyn_WithTypes_struct*) _temp337)->f2; goto _LL344; _LL344:
_temp343=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp337)->f3;
goto _LL340;} else{ goto _LL341;} _LL341: if((( struct _tunion_struct*) _temp337)->tag
== Cyc_Absyn_NoTypes_tag){ _LL352: _temp351=( struct Cyc_List_List*)(( struct
Cyc_Absyn_NoTypes_struct*) _temp337)->f1; goto _LL350; _LL350: _temp349=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*) _temp337)->f2; goto
_LL342;} else{ goto _LL338;} _LL340: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_funargs2doc(
_temp347, _temp345, _temp343)); _LL342: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp353=( char*)"("; struct _tagged_string
_temp354; _temp354.curr= _temp353; _temp354.base= _temp353; _temp354.last_plus_one=
_temp353 + 2; _temp354;}),( struct _tagged_string)({ char* _temp355=( char*)")";
struct _tagged_string _temp356; _temp356.curr= _temp355; _temp356.base= _temp355;
_temp356.last_plus_one= _temp355 + 2; _temp356;}),( struct _tagged_string)({
char* _temp357=( char*)","; struct _tagged_string _temp358; _temp358.curr=
_temp357; _temp358.base= _temp357; _temp358.last_plus_one= _temp357 + 2;
_temp358;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr,
_temp351))); _LL338:;} _LL303: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List*
_temp359= tms; if( _temp359 == 0){ _throw( Null_Exception);} _temp359->tl;}))){
rest= p_rest;} return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_atts2doc( _temp312));
_LL305: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List* _temp360= tms;
if( _temp360 == 0){ _throw( Null_Exception);} _temp360->tl;}))){ rest= p_rest;}
if( _temp316){ return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_ktvars2doc( _temp320));}
else{ return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_tvars2doc( _temp320));} _LL307: {
struct Cyc_PP_Doc* ptr;{ void* _temp361= _temp326; struct Cyc_Absyn_Exp*
_temp369; struct Cyc_Absyn_Exp* _temp371; _LL363: if(( unsigned int) _temp361 >
1u?(( struct _tunion_struct*) _temp361)->tag == Cyc_Absyn_Nullable_ps_tag: 0){
_LL370: _temp369=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp361)->f1; goto _LL364;} else{ goto _LL365;} _LL365: if(( unsigned int)
_temp361 > 1u?(( struct _tunion_struct*) _temp361)->tag == Cyc_Absyn_NonNullable_ps_tag:
0){ _LL372: _temp371=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp361)->f1; goto _LL366;} else{ goto _LL367;} _LL367: if(( int) _temp361 ==
Cyc_Absyn_TaggedArray_ps){ goto _LL368;} else{ goto _LL362;} _LL364: if( Cyc_Evexp_eval_const_uint_exp(
_temp369) == 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char* _temp373=(
char*)"*"; struct _tagged_string _temp374; _temp374.curr= _temp373; _temp374.base=
_temp373; _temp374.last_plus_one= _temp373 + 2; _temp374;}));} else{ ptr= Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp375=( char*)"*"; struct
_tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 2; _temp376;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp377=( char*)"{"; struct _tagged_string _temp378;
_temp378.curr= _temp377; _temp378.base= _temp377; _temp378.last_plus_one=
_temp377 + 2; _temp378;})), Cyc_Absynpp_exp2doc( _temp369), Cyc_PP_text(( struct
_tagged_string)({ char* _temp379=( char*)"}"; struct _tagged_string _temp380;
_temp380.curr= _temp379; _temp380.base= _temp379; _temp380.last_plus_one=
_temp379 + 2; _temp380;})));} goto _LL362; _LL366: if( Cyc_Evexp_eval_const_uint_exp(
_temp371) == 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char* _temp381=(
char*)"@"; struct _tagged_string _temp382; _temp382.curr= _temp381; _temp382.base=
_temp381; _temp382.last_plus_one= _temp381 + 2; _temp382;}));} else{ ptr= Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp383=( char*)"@"; struct
_tagged_string _temp384; _temp384.curr= _temp383; _temp384.base= _temp383;
_temp384.last_plus_one= _temp383 + 2; _temp384;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp385=( char*)"{"; struct _tagged_string _temp386;
_temp386.curr= _temp385; _temp386.base= _temp385; _temp386.last_plus_one=
_temp385 + 2; _temp386;})), Cyc_Absynpp_exp2doc( _temp371), Cyc_PP_text(( struct
_tagged_string)({ char* _temp387=( char*)"}"; struct _tagged_string _temp388;
_temp388.curr= _temp387; _temp388.base= _temp387; _temp388.last_plus_one=
_temp387 + 2; _temp388;})));} goto _LL362; _LL368: ptr= Cyc_PP_text(( struct
_tagged_string)({ char* _temp389=( char*)"?"; struct _tagged_string _temp390;
_temp390.curr= _temp389; _temp390.base= _temp389; _temp390.last_plus_one=
_temp389 + 2; _temp390;})); goto _LL362; _LL362:;}{ void* _temp391= Cyc_Tcutil_compress(
_temp324); _LL393: if(( int) _temp391 == Cyc_Absyn_HeapRgn){ goto _LL394;} else{
goto _LL395;} _LL395: goto _LL396; _LL394: return Cyc_Absynpp_cat2( ptr, rest);
_LL396: return Cyc_Absynpp_cat4( ptr, Cyc_Absynpp_ntyp2doc( _temp324), Cyc_PP_text((
struct _tagged_string)({ char* _temp397=( char*)" "; struct _tagged_string
_temp398; _temp398.curr= _temp397; _temp398.base= _temp397; _temp398.last_plus_one=
_temp397 + 2; _temp398;})), rest); _LL392:;}} _LL295:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc(
void* t){ void* _temp399= Cyc_Tcutil_compress( t); _LL401: if(( int) _temp399 ==
Cyc_Absyn_HeapRgn){ goto _LL402;} else{ goto _LL403;} _LL403: goto _LL404;
_LL402: return Cyc_PP_text(( struct _tagged_string)({ char* _temp405=( char*)"`H";
struct _tagged_string _temp406; _temp406.curr= _temp405; _temp406.base= _temp405;
_temp406.last_plus_one= _temp405 + 3; _temp406;})); _LL404: return Cyc_Absynpp_ntyp2doc(
t); _LL400:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
regions, struct Cyc_List_List** effects, void* t){ void* _temp407= Cyc_Tcutil_compress(
t); void* _temp415; struct Cyc_List_List* _temp417; _LL409: if(( unsigned int)
_temp407 > 4u?(( struct _tunion_struct*) _temp407)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL416: _temp415=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp407)->f1;
goto _LL410;} else{ goto _LL411;} _LL411: if(( unsigned int) _temp407 > 4u?((
struct _tunion_struct*) _temp407)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL418:
_temp417=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp407)->f1;
goto _LL412;} else{ goto _LL413;} _LL413: goto _LL414; _LL410:* regions=({
struct Cyc_List_List* _temp419=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp419->hd=( void*) Cyc_Absynpp_rgn2doc( _temp415);
_temp419->tl=* regions; _temp419;}); goto _LL408; _LL412: for( 0; _temp417 != 0;
_temp417=({ struct Cyc_List_List* _temp420= _temp417; if( _temp420 == 0){ _throw(
Null_Exception);} _temp420->tl;})){ Cyc_Absynpp_effects2docs( regions, effects,(
void*)({ struct Cyc_List_List* _temp421= _temp417; if( _temp421 == 0){ _throw(
Null_Exception);} _temp421->hd;}));} goto _LL408; _LL414:* effects=({ struct Cyc_List_List*
_temp422=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp422->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp422->tl=* effects; _temp422;});
goto _LL408; _LL408:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp423=( char*)""; struct _tagged_string
_temp424; _temp424.curr= _temp423; _temp424.base= _temp423; _temp424.last_plus_one=
_temp423 + 1; _temp424;}),( struct _tagged_string)({ char* _temp425=( char*)"";
struct _tagged_string _temp426; _temp426.curr= _temp425; _temp426.base= _temp425;
_temp426.last_plus_one= _temp425 + 1; _temp426;}),( struct _tagged_string)({
char* _temp427=( char*)"+"; struct _tagged_string _temp428; _temp428.curr=
_temp427; _temp428.base= _temp427; _temp428.last_plus_one= _temp427 + 2;
_temp428;}), effects);} else{ struct Cyc_PP_Doc* doc1= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp429=( char*)"{"; struct _tagged_string
_temp430; _temp430.curr= _temp429; _temp430.base= _temp429; _temp430.last_plus_one=
_temp429 + 2; _temp430;}),( struct _tagged_string)({ char* _temp431=( char*)"}";
struct _tagged_string _temp432; _temp432.curr= _temp431; _temp432.base= _temp431;
_temp432.last_plus_one= _temp431 + 2; _temp432;}),( struct _tagged_string)({
char* _temp433=( char*)","; struct _tagged_string _temp434; _temp434.curr=
_temp433; _temp434.base= _temp433; _temp434.last_plus_one= _temp433 + 2;
_temp434;}), regions); return Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp435=( char*)""; struct _tagged_string _temp436; _temp436.curr= _temp435;
_temp436.base= _temp435; _temp436.last_plus_one= _temp435 + 1; _temp436;}),(
struct _tagged_string)({ char* _temp437=( char*)""; struct _tagged_string
_temp438; _temp438.curr= _temp437; _temp438.base= _temp437; _temp438.last_plus_one=
_temp437 + 1; _temp438;}),( struct _tagged_string)({ char* _temp439=( char*)"+";
struct _tagged_string _temp440; _temp440.curr= _temp439; _temp440.base= _temp439;
_temp440.last_plus_one= _temp439 + 2; _temp440;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( effects,({
struct Cyc_List_List* _temp441=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp441->hd=( void*) doc1; _temp441->tl= 0; _temp441;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc* s;{ void*
_temp442= t; struct Cyc_Absyn_Exp* _temp488; struct Cyc_Absyn_Tqual* _temp490;
void* _temp492; struct Cyc_Absyn_FnInfo _temp494; struct Cyc_Absyn_PtrInfo
_temp496; int _temp498; struct Cyc_Core_Opt* _temp500; void* _temp502; struct
Cyc_Absyn_Tvar* _temp504; struct Cyc_Absyn_TunionInfo _temp506; struct Cyc_Absyn_Tuniondecl*
_temp508; void* _temp510; struct Cyc_List_List* _temp512; struct _tuple0*
_temp514; struct Cyc_Absyn_XTunionInfo _temp516; struct Cyc_Absyn_XTuniondecl*
_temp518; void* _temp520; struct _tuple0* _temp522; struct Cyc_Absyn_TunionFieldInfo
_temp524; struct Cyc_Absyn_Tunionfield* _temp526; struct Cyc_Absyn_Tuniondecl*
_temp528; struct _tuple0* _temp530; struct Cyc_List_List* _temp532; struct
_tuple0* _temp534; struct Cyc_Absyn_XTunionFieldInfo _temp536; struct Cyc_Absyn_Tunionfield*
_temp538; struct Cyc_Absyn_XTuniondecl* _temp540; struct _tuple0* _temp542;
struct _tuple0* _temp544; void* _temp546; void* _temp548; struct Cyc_List_List*
_temp550; struct Cyc_Absyn_Structdecl** _temp552; struct Cyc_List_List* _temp554;
struct _tuple0* _temp556; struct Cyc_Absyn_Uniondecl** _temp558; struct Cyc_List_List*
_temp560; struct _tuple0* _temp562; struct Cyc_Absyn_Enumdecl* _temp564; struct
_tuple0* _temp566; struct Cyc_Core_Opt* _temp568; struct Cyc_List_List* _temp570;
struct _tuple0* _temp572; void* _temp574; void* _temp576; struct Cyc_List_List*
_temp578; _LL444: if(( unsigned int) _temp442 > 4u?(( struct _tunion_struct*)
_temp442)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL493: _temp492=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp442)->f1; goto _LL491; _LL491: _temp490=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp442)->f2;
goto _LL489; _LL489: _temp488=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp442)->f3; goto _LL445;} else{ goto _LL446;} _LL446: if(( unsigned int)
_temp442 > 4u?(( struct _tunion_struct*) _temp442)->tag == Cyc_Absyn_FnType_tag:
0){ _LL495: _temp494=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp442)->f1; goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int)
_temp442 > 4u?(( struct _tunion_struct*) _temp442)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL497: _temp496=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp442)->f1; goto _LL449;} else{ goto _LL450;} _LL450: if(( int) _temp442 ==
Cyc_Absyn_VoidType){ goto _LL451;} else{ goto _LL452;} _LL452: if(( unsigned int)
_temp442 > 4u?(( struct _tunion_struct*) _temp442)->tag == Cyc_Absyn_Evar_tag: 0){
_LL503: _temp502=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp442)->f1; goto
_LL501; _LL501: _temp500=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp442)->f2; goto _LL499; _LL499: _temp498=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp442)->f3; goto _LL453;} else{ goto _LL454;} _LL454: if(( unsigned int)
_temp442 > 4u?(( struct _tunion_struct*) _temp442)->tag == Cyc_Absyn_VarType_tag:
0){ _LL505: _temp504=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp442)->f1; goto _LL455;} else{ goto _LL456;} _LL456: if(( unsigned int)
_temp442 > 4u?(( struct _tunion_struct*) _temp442)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL507: _temp506=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp442)->f1; _LL515: _temp514=( struct _tuple0*) _temp506.name; goto _LL513;
_LL513: _temp512=( struct Cyc_List_List*) _temp506.targs; goto _LL511; _LL511:
_temp510=( void*) _temp506.rgn; goto _LL509; _LL509: _temp508=( struct Cyc_Absyn_Tuniondecl*)
_temp506.tud; goto _LL457;} else{ goto _LL458;} _LL458: if(( unsigned int)
_temp442 > 4u?(( struct _tunion_struct*) _temp442)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL517: _temp516=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp442)->f1; _LL523: _temp522=( struct _tuple0*) _temp516.name; goto _LL521;
_LL521: _temp520=( void*) _temp516.rgn; goto _LL519; _LL519: _temp518=( struct
Cyc_Absyn_XTuniondecl*) _temp516.xtud; goto _LL459;} else{ goto _LL460;} _LL460:
if(( unsigned int) _temp442 > 4u?(( struct _tunion_struct*) _temp442)->tag ==
Cyc_Absyn_TunionFieldType_tag: 0){ _LL525: _temp524=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp442)->f1; _LL535: _temp534=(
struct _tuple0*) _temp524.name; goto _LL533; _LL533: _temp532=( struct Cyc_List_List*)
_temp524.targs; goto _LL531; _LL531: _temp530=( struct _tuple0*) _temp524.fname;
goto _LL529; _LL529: _temp528=( struct Cyc_Absyn_Tuniondecl*) _temp524.tud; goto
_LL527; _LL527: _temp526=( struct Cyc_Absyn_Tunionfield*) _temp524.tufd; goto
_LL461;} else{ goto _LL462;} _LL462: if(( unsigned int) _temp442 > 4u?(( struct
_tunion_struct*) _temp442)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL537:
_temp536=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp442)->f1; _LL545: _temp544=( struct _tuple0*) _temp536.name; goto _LL543;
_LL543: _temp542=( struct _tuple0*) _temp536.fname; goto _LL541; _LL541:
_temp540=( struct Cyc_Absyn_XTuniondecl*) _temp536.xtud; goto _LL539; _LL539:
_temp538=( struct Cyc_Absyn_Tunionfield*) _temp536.xtufd; goto _LL463;} else{
goto _LL464;} _LL464: if(( unsigned int) _temp442 > 4u?(( struct _tunion_struct*)
_temp442)->tag == Cyc_Absyn_IntType_tag: 0){ _LL549: _temp548=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp442)->f1; goto _LL547; _LL547: _temp546=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp442)->f2; goto _LL465;} else{ goto _LL466;}
_LL466: if(( int) _temp442 == Cyc_Absyn_FloatType){ goto _LL467;} else{ goto
_LL468;} _LL468: if(( int) _temp442 == Cyc_Absyn_DoubleType){ goto _LL469;}
else{ goto _LL470;} _LL470: if(( unsigned int) _temp442 > 4u?(( struct
_tunion_struct*) _temp442)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL551: _temp550=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp442)->f1; goto
_LL471;} else{ goto _LL472;} _LL472: if(( unsigned int) _temp442 > 4u?(( struct
_tunion_struct*) _temp442)->tag == Cyc_Absyn_StructType_tag: 0){ _LL557:
_temp556=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp442)->f1;
goto _LL555; _LL555: _temp554=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp442)->f2; goto _LL553; _LL553: _temp552=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp442)->f3; goto _LL473;} else{ goto
_LL474;} _LL474: if(( unsigned int) _temp442 > 4u?(( struct _tunion_struct*)
_temp442)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL563: _temp562=( struct _tuple0*)((
struct Cyc_Absyn_UnionType_struct*) _temp442)->f1; goto _LL561; _LL561: _temp560=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp442)->f2; goto
_LL559; _LL559: _temp558=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp442)->f3; goto _LL475;} else{ goto _LL476;} _LL476: if(( unsigned int)
_temp442 > 4u?(( struct _tunion_struct*) _temp442)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL567: _temp566=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*)
_temp442)->f1; goto _LL565; _LL565: _temp564=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp442)->f2; goto _LL477;} else{ goto
_LL478;} _LL478: if(( unsigned int) _temp442 > 4u?(( struct _tunion_struct*)
_temp442)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL573: _temp572=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp442)->f1; goto _LL571;
_LL571: _temp570=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp442)->f2; goto _LL569; _LL569: _temp568=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp442)->f3; goto _LL479;} else{ goto _LL480;}
_LL480: if(( unsigned int) _temp442 > 4u?(( struct _tunion_struct*) _temp442)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL575: _temp574=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp442)->f1; goto _LL481;} else{ goto _LL482;} _LL482: if(( int) _temp442 ==
Cyc_Absyn_HeapRgn){ goto _LL483;} else{ goto _LL484;} _LL484: if(( unsigned int)
_temp442 > 4u?(( struct _tunion_struct*) _temp442)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL577: _temp576=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp442)->f1;
goto _LL485;} else{ goto _LL486;} _LL486: if(( unsigned int) _temp442 > 4u?((
struct _tunion_struct*) _temp442)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL579:
_temp578=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp442)->f1;
goto _LL487;} else{ goto _LL443;} _LL445: return Cyc_PP_text(( struct
_tagged_string)({ char* _temp580=( char*)"[[[array]]]"; struct _tagged_string
_temp581; _temp581.curr= _temp580; _temp581.base= _temp580; _temp581.last_plus_one=
_temp580 + 12; _temp581;})); _LL447: return Cyc_PP_nil_doc(); _LL449: return Cyc_PP_nil_doc();
_LL451: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp582=( char*)"void";
struct _tagged_string _temp583; _temp583.curr= _temp582; _temp583.base= _temp582;
_temp583.last_plus_one= _temp582 + 5; _temp583;})); goto _LL443; _LL453: if(
_temp500 != 0){ return Cyc_Absynpp_ntyp2doc(( void*)({ struct Cyc_Core_Opt*
_temp584= _temp500; if( _temp584 == 0){ _throw( Null_Exception);} _temp584->v;}));}
else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct _tagged_string)({ char* _temp585=(
char*)"%"; struct _tagged_string _temp586; _temp586.curr= _temp585; _temp586.base=
_temp585; _temp586.last_plus_one= _temp585 + 2; _temp586;})), Cyc_PP_text((
struct _tagged_string)({ char* _temp587=( char*)"("; struct _tagged_string
_temp588; _temp588.curr= _temp587; _temp588.base= _temp587; _temp588.last_plus_one=
_temp587 + 2; _temp588;})), Cyc_PP_text( xprintf("%d", _temp498)), Cyc_PP_text((
struct _tagged_string)({ char* _temp589=( char*)")::"; struct _tagged_string
_temp590; _temp590.curr= _temp589; _temp590.base= _temp589; _temp590.last_plus_one=
_temp589 + 4; _temp590;})), Cyc_Absynpp_kind2doc( _temp502));} goto _LL443;
_LL455: s= Cyc_Absynpp_textptr( _temp504->name); if( Cyc_Absynpp_print_all_kinds){
s= Cyc_Absynpp_cat3( s, Cyc_PP_text(( struct _tagged_string)({ char* _temp591=(
char*)"::"; struct _tagged_string _temp592; _temp592.curr= _temp591; _temp592.base=
_temp591; _temp592.last_plus_one= _temp591 + 3; _temp592;})), Cyc_Absynpp_ckind2doc(
_temp504->kind));} if( ! Cyc_Absynpp_dont_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_temp504): 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp593=( char*)"_ /* "; struct _tagged_string _temp594; _temp594.curr=
_temp593; _temp594.base= _temp593; _temp594.last_plus_one= _temp593 + 6;
_temp594;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp595=( char*)" */";
struct _tagged_string _temp596; _temp596.curr= _temp595; _temp596.base= _temp595;
_temp596.last_plus_one= _temp595 + 4; _temp596;})));} goto _LL443; _LL457:{ void*
_temp597= Cyc_Tcutil_compress( _temp510); _LL599: if(( int) _temp597 == Cyc_Absyn_HeapRgn){
goto _LL600;} else{ goto _LL601;} _LL601: goto _LL602; _LL600: s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp603=( char*)"tunion "; struct
_tagged_string _temp604; _temp604.curr= _temp603; _temp604.base= _temp603;
_temp604.last_plus_one= _temp603 + 8; _temp604;})), Cyc_Absynpp_qvar2doc(
_temp514), Cyc_Absynpp_tps2doc( _temp512)); goto _LL598; _LL602: s= Cyc_Absynpp_cat5(
Cyc_PP_text(( struct _tagged_string)({ char* _temp605=( char*)"tunion "; struct
_tagged_string _temp606; _temp606.curr= _temp605; _temp606.base= _temp605;
_temp606.last_plus_one= _temp605 + 8; _temp606;})), Cyc_Absynpp_typ2doc(
_temp510), Cyc_PP_text(( struct _tagged_string)({ char* _temp607=( char*)" ";
struct _tagged_string _temp608; _temp608.curr= _temp607; _temp608.base= _temp607;
_temp608.last_plus_one= _temp607 + 2; _temp608;})), Cyc_Absynpp_qvar2doc(
_temp514), Cyc_Absynpp_tps2doc( _temp512)); goto _LL598; _LL598:;} goto _LL443;
_LL459:{ void* _temp609= Cyc_Tcutil_compress( _temp520); _LL611: if(( int)
_temp609 == Cyc_Absyn_HeapRgn){ goto _LL612;} else{ goto _LL613;} _LL613: goto
_LL614; _LL612: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp615=( char*)"xtunion "; struct _tagged_string _temp616; _temp616.curr=
_temp615; _temp616.base= _temp615; _temp616.last_plus_one= _temp615 + 9;
_temp616;})), Cyc_Absynpp_qvar2doc( _temp522)); goto _LL610; _LL614: s= Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp617=( char*)"xtunion "; struct
_tagged_string _temp618; _temp618.curr= _temp617; _temp618.base= _temp617;
_temp618.last_plus_one= _temp617 + 9; _temp618;})), Cyc_Absynpp_typ2doc(
_temp520), Cyc_PP_text(( struct _tagged_string)({ char* _temp619=( char*)" ";
struct _tagged_string _temp620; _temp620.curr= _temp619; _temp620.base= _temp619;
_temp620.last_plus_one= _temp619 + 2; _temp620;})), Cyc_Absynpp_qvar2doc(
_temp522)); goto _LL610; _LL610:;} goto _LL443; _LL461: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp621=( char*)"tunion "; struct _tagged_string
_temp622; _temp622.curr= _temp621; _temp622.base= _temp621; _temp622.last_plus_one=
_temp621 + 8; _temp622;})), Cyc_Absynpp_qvar2doc( _temp534), Cyc_PP_text((
struct _tagged_string)({ char* _temp623=( char*)"."; struct _tagged_string
_temp624; _temp624.curr= _temp623; _temp624.base= _temp623; _temp624.last_plus_one=
_temp623 + 2; _temp624;})), Cyc_Absynpp_textptr((* _temp530).f2), Cyc_Absynpp_tps2doc(
_temp532)); goto _LL443; _LL463: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct
_tagged_string)({ char* _temp625=( char*)"xtunion "; struct _tagged_string
_temp626; _temp626.curr= _temp625; _temp626.base= _temp625; _temp626.last_plus_one=
_temp625 + 9; _temp626;})), Cyc_Absynpp_qvar2doc( _temp544), Cyc_PP_text((
struct _tagged_string)({ char* _temp627=( char*)"."; struct _tagged_string
_temp628; _temp628.curr= _temp627; _temp628.base= _temp627; _temp628.last_plus_one=
_temp627 + 2; _temp628;})), Cyc_Absynpp_textptr((* _temp542).f2)); goto _LL443;
_LL465: { struct _tagged_string sns; struct _tagged_string ts;{ void* _temp629=
_temp548; _LL631: if(( int) _temp629 == Cyc_Absyn_Signed){ goto _LL632;} else{
goto _LL633;} _LL633: if(( int) _temp629 == Cyc_Absyn_Unsigned){ goto _LL634;}
else{ goto _LL630;} _LL632: sns=( struct _tagged_string)({ char* _temp635=( char*)"";
struct _tagged_string _temp636; _temp636.curr= _temp635; _temp636.base= _temp635;
_temp636.last_plus_one= _temp635 + 1; _temp636;}); goto _LL630; _LL634: sns=(
struct _tagged_string)({ char* _temp637=( char*)"unsigned "; struct
_tagged_string _temp638; _temp638.curr= _temp637; _temp638.base= _temp637;
_temp638.last_plus_one= _temp637 + 10; _temp638;}); goto _LL630; _LL630:;}{ void*
_temp639= _temp546; _LL641: if(( int) _temp639 == Cyc_Absyn_B1){ goto _LL642;}
else{ goto _LL643;} _LL643: if(( int) _temp639 == Cyc_Absyn_B2){ goto _LL644;}
else{ goto _LL645;} _LL645: if(( int) _temp639 == Cyc_Absyn_B4){ goto _LL646;}
else{ goto _LL647;} _LL647: if(( int) _temp639 == Cyc_Absyn_B8){ goto _LL648;}
else{ goto _LL640;} _LL642:{ void* _temp649= _temp548; _LL651: if(( int)
_temp649 == Cyc_Absyn_Signed){ goto _LL652;} else{ goto _LL653;} _LL653: if((
int) _temp649 == Cyc_Absyn_Unsigned){ goto _LL654;} else{ goto _LL650;} _LL652:
sns=( struct _tagged_string)({ char* _temp655=( char*)"signed "; struct
_tagged_string _temp656; _temp656.curr= _temp655; _temp656.base= _temp655;
_temp656.last_plus_one= _temp655 + 8; _temp656;}); goto _LL650; _LL654: sns=(
struct _tagged_string)({ char* _temp657=( char*)""; struct _tagged_string
_temp658; _temp658.curr= _temp657; _temp658.base= _temp657; _temp658.last_plus_one=
_temp657 + 1; _temp658;}); goto _LL650; _LL650:;} ts=( struct _tagged_string)({
char* _temp659=( char*)"char"; struct _tagged_string _temp660; _temp660.curr=
_temp659; _temp660.base= _temp659; _temp660.last_plus_one= _temp659 + 5;
_temp660;}); goto _LL640; _LL644: ts=( struct _tagged_string)({ char* _temp661=(
char*)"short"; struct _tagged_string _temp662; _temp662.curr= _temp661; _temp662.base=
_temp661; _temp662.last_plus_one= _temp661 + 6; _temp662;}); goto _LL640; _LL646:
ts=( struct _tagged_string)({ char* _temp663=( char*)"int"; struct
_tagged_string _temp664; _temp664.curr= _temp663; _temp664.base= _temp663;
_temp664.last_plus_one= _temp663 + 4; _temp664;}); goto _LL640; _LL648: ts=(
struct _tagged_string)({ char* _temp665=( char*)"long long"; struct
_tagged_string _temp666; _temp666.curr= _temp665; _temp666.base= _temp665;
_temp666.last_plus_one= _temp665 + 10; _temp666;}); goto _LL640; _LL640:;} s=
Cyc_PP_text(({ struct _tagged_string _temp667= sns; struct _tagged_string
_temp668= ts; xprintf("%.*s%.*s", _temp667.last_plus_one - _temp667.curr,
_temp667.curr, _temp668.last_plus_one - _temp668.curr, _temp668.curr);})); goto
_LL443;} _LL467: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp669=( char*)"float";
struct _tagged_string _temp670; _temp670.curr= _temp669; _temp670.base= _temp669;
_temp670.last_plus_one= _temp669 + 6; _temp670;})); goto _LL443; _LL469: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp671=( char*)"double"; struct _tagged_string
_temp672; _temp672.curr= _temp671; _temp672.base= _temp671; _temp672.last_plus_one=
_temp671 + 7; _temp672;})); goto _LL443; _LL471: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp673=( char*)"$"; struct _tagged_string
_temp674; _temp674.curr= _temp673; _temp674.base= _temp673; _temp674.last_plus_one=
_temp673 + 2; _temp674;})), Cyc_Absynpp_args2doc( _temp550)); goto _LL443;
_LL473: if( _temp556 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp675=( char*)"struct "; struct _tagged_string
_temp676; _temp676.curr= _temp675; _temp676.base= _temp675; _temp676.last_plus_one=
_temp675 + 8; _temp676;})), Cyc_Absynpp_tps2doc( _temp554));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp677=( char*)"struct "; struct
_tagged_string _temp678; _temp678.curr= _temp677; _temp678.base= _temp677;
_temp678.last_plus_one= _temp677 + 8; _temp678;})), _temp556 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp679= _temp556;
if( _temp679 == 0){ _throw( Null_Exception);} _temp679;})), Cyc_Absynpp_tps2doc(
_temp554));} goto _LL443; _LL475: if( _temp562 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp680=( char*)"union "; struct _tagged_string
_temp681; _temp681.curr= _temp680; _temp681.base= _temp680; _temp681.last_plus_one=
_temp680 + 7; _temp681;})), Cyc_Absynpp_tps2doc( _temp560));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp682=( char*)"union "; struct
_tagged_string _temp683; _temp683.curr= _temp682; _temp683.base= _temp682;
_temp683.last_plus_one= _temp682 + 7; _temp683;})), _temp562 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp684= _temp562;
if( _temp684 == 0){ _throw( Null_Exception);} _temp684;})), Cyc_Absynpp_tps2doc(
_temp560));} goto _LL443; _LL477: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp685=( char*)"enum "; struct _tagged_string _temp686;
_temp686.curr= _temp685; _temp686.base= _temp685; _temp686.last_plus_one=
_temp685 + 6; _temp686;})), Cyc_Absynpp_qvar2doc( _temp566)); goto _LL443;
_LL479: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp572), Cyc_Absynpp_tps2doc(
_temp570)); goto _LL443; _LL481: s= Cyc_Absynpp_rgn2doc( _temp574); goto _LL443;
_LL483: s= Cyc_Absynpp_rgn2doc( t); goto _LL443; _LL485: s= Cyc_Absynpp_eff2doc(
t); goto _LL443; _LL487: s= Cyc_Absynpp_eff2doc( t); goto _LL443; _LL443:;}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct Cyc_Core_Opt* vo){
return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct _tagged_string*)({
struct Cyc_Core_Opt* _temp687= vo; if( _temp687 == 0){ _throw( Null_Exception);}
_temp687->v;})));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){
struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt* _temp688=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp688->v=(
void*) Cyc_PP_text(*(( struct _tagged_string*)({ struct Cyc_Core_Opt* _temp689=(*
t).f1; if( _temp689 == 0){ _throw( Null_Exception);} _temp689->v;}))); _temp688;});
return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt){ if(
effopt == 0){ if( varargs){ return Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp690=( char*)"("; struct _tagged_string _temp691; _temp691.curr=
_temp690; _temp691.base= _temp690; _temp691.last_plus_one= _temp690 + 2;
_temp691;}),( struct _tagged_string)({ char* _temp692=( char*)")"; struct
_tagged_string _temp693; _temp693.curr= _temp692; _temp693.base= _temp692;
_temp693.last_plus_one= _temp692 + 2; _temp693;}),( struct _tagged_string)({
char* _temp694=( char*)","; struct _tagged_string _temp695; _temp695.curr=
_temp694; _temp695.base= _temp694; _temp695.last_plus_one= _temp694 + 2;
_temp695;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)((( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args),({ struct Cyc_List_List* _temp696=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp696->hd=( void*) Cyc_PP_text(( struct
_tagged_string)({ char* _temp697=( char*)"..."; struct _tagged_string _temp698;
_temp698.curr= _temp697; _temp698.base= _temp697; _temp698.last_plus_one=
_temp697 + 4; _temp698;})); _temp696->tl= 0; _temp696;})));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp699=( char*)"("; struct _tagged_string
_temp700; _temp700.curr= _temp699; _temp700.base= _temp699; _temp700.last_plus_one=
_temp699 + 2; _temp700;}),( struct _tagged_string)({ char* _temp701=( char*)")";
struct _tagged_string _temp702; _temp702.curr= _temp701; _temp702.base= _temp701;
_temp702.last_plus_one= _temp701 + 2; _temp702;}),( struct _tagged_string)({
char* _temp703=( char*)","; struct _tagged_string _temp704; _temp704.curr=
_temp703; _temp704.base= _temp703; _temp704.last_plus_one= _temp703 + 2;
_temp704;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));}} else{
if( varargs){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp705=(
char*)"("; struct _tagged_string _temp706; _temp706.curr= _temp705; _temp706.base=
_temp705; _temp706.last_plus_one= _temp705 + 2; _temp706;}),( struct
_tagged_string)({ char* _temp707=( char*)")"; struct _tagged_string _temp708;
_temp708.curr= _temp707; _temp708.base= _temp707; _temp708.last_plus_one=
_temp707 + 2; _temp708;}),( struct _tagged_string)({ char* _temp709=( char*)";";
struct _tagged_string _temp710; _temp710.curr= _temp709; _temp710.base= _temp709;
_temp710.last_plus_one= _temp709 + 2; _temp710;}),({ struct Cyc_List_List*
_temp711=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp711->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp714=( char*)""; struct _tagged_string _temp715; _temp715.curr= _temp714;
_temp715.base= _temp714; _temp715.last_plus_one= _temp714 + 1; _temp715;}),(
struct _tagged_string)({ char* _temp716=( char*)""; struct _tagged_string
_temp717; _temp717.curr= _temp716; _temp717.base= _temp716; _temp717.last_plus_one=
_temp716 + 1; _temp717;}),( struct _tagged_string)({ char* _temp718=( char*)",";
struct _tagged_string _temp719; _temp719.curr= _temp718; _temp719.base= _temp718;
_temp719.last_plus_one= _temp718 + 2; _temp719;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args),({ struct Cyc_List_List*
_temp720=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp720->hd=( void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp721=(
char*)"..."; struct _tagged_string _temp722; _temp722.curr= _temp721; _temp722.base=
_temp721; _temp722.last_plus_one= _temp721 + 4; _temp722;})); _temp720->tl= 0;
_temp720;}))); _temp711->tl=({ struct Cyc_List_List* _temp712=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp712->hd=( void*) Cyc_Absynpp_eff2doc((
void*)({ struct Cyc_Core_Opt* _temp713= effopt; if( _temp713 == 0){ _throw(
Null_Exception);} _temp713->v;})); _temp712->tl= 0; _temp712;}); _temp711;}));}
else{ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp723=( char*)"(";
struct _tagged_string _temp724; _temp724.curr= _temp723; _temp724.base= _temp723;
_temp724.last_plus_one= _temp723 + 2; _temp724;}),( struct _tagged_string)({
char* _temp725=( char*)")"; struct _tagged_string _temp726; _temp726.curr=
_temp725; _temp726.base= _temp725; _temp726.last_plus_one= _temp725 + 2;
_temp726;}),( struct _tagged_string)({ char* _temp727=( char*)";"; struct
_tagged_string _temp728; _temp728.curr= _temp727; _temp728.base= _temp727;
_temp728.last_plus_one= _temp727 + 2; _temp728;}),({ struct Cyc_List_List*
_temp729=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp729->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp732=( char*)""; struct _tagged_string _temp733; _temp733.curr= _temp732;
_temp733.base= _temp732; _temp733.last_plus_one= _temp732 + 1; _temp733;}),(
struct _tagged_string)({ char* _temp734=( char*)""; struct _tagged_string
_temp735; _temp735.curr= _temp734; _temp735.base= _temp734; _temp735.last_plus_one=
_temp734 + 1; _temp735;}),( struct _tagged_string)({ char* _temp736=( char*)",";
struct _tagged_string _temp737; _temp737.curr= _temp736; _temp737.base= _temp736;
_temp737.last_plus_one= _temp736 + 2; _temp737;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_funarg2doc, args)); _temp729->tl=({ struct Cyc_List_List* _temp730=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp730->hd=(
void*) Cyc_Absynpp_eff2doc(( void*)({ struct Cyc_Core_Opt* _temp731= effopt; if(
_temp731 == 0){ _throw( Null_Exception);} _temp731->v;})); _temp730->tl= 0;
_temp730;}); _temp729;}));}}} struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct
_tuple3* arg){ return({ struct _tuple1* _temp738=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp738->f1=({ struct Cyc_Core_Opt* _temp739=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp739->v=( void*)(*
arg).f1; _temp739;}); _temp738->f2=(* arg).f2; _temp738->f3=(* arg).f3; _temp738;});}
struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct _tagged_string* v){ return Cyc_PP_text(*
v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
prefix= 0;{ void* _temp740=(* q).f1; struct Cyc_List_List* _temp752; struct Cyc_List_List*
_temp754; struct Cyc_List_List* _temp756; struct Cyc_List_List* _temp758; _LL742:
if(( int) _temp740 == Cyc_Absyn_Loc_n){ goto _LL743;} else{ goto _LL744;} _LL744:
if(( unsigned int) _temp740 > 1u?(( struct _tunion_struct*) _temp740)->tag ==
Cyc_Absyn_Rel_n_tag: 0){ _LL753: _temp752=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp740)->f1; if( _temp752 == 0){ goto _LL745;} else{ goto _LL746;}} else{ goto
_LL746;} _LL746: if(( unsigned int) _temp740 > 1u?(( struct _tunion_struct*)
_temp740)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL755: _temp754=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp740)->f1; if( _temp754 == 0){ goto _LL747;}
else{ goto _LL748;}} else{ goto _LL748;} _LL748: if(( unsigned int) _temp740 > 1u?((
struct _tunion_struct*) _temp740)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL757:
_temp756=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp740)->f1;
goto _LL749;} else{ goto _LL750;} _LL750: if(( unsigned int) _temp740 > 1u?((
struct _tunion_struct*) _temp740)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL759:
_temp758=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp740)->f1;
goto _LL751;} else{ goto _LL741;} _LL743: goto _LL745; _LL745: goto _LL747;
_LL747: return Cyc_Absynpp_var2doc((* q).f2); _LL749: _temp758= _temp756; goto
_LL751; _LL751: prefix= _temp758; goto _LL741; _LL741:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( prefix,({ struct
Cyc_List_List* _temp760=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp760->hd=( void*)(* q).f2; _temp760->tl= 0; _temp760;})),( struct
_tagged_string)({ char* _temp761=( char*)"_"; struct _tagged_string _temp762;
_temp762.curr= _temp761; _temp762.base= _temp761; _temp762.last_plus_one=
_temp761 + 2; _temp762;})));} else{ if((( int(*)( int(* cmp)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, prefix, Cyc_Absynpp_curr_namespace)){
return Cyc_Absynpp_var2doc((* q).f2);} else{ return Cyc_PP_text( Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
prefix,({ struct Cyc_List_List* _temp763=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp763->hd=( void*)(* q).f2; _temp763->tl= 0;
_temp763;})),( struct _tagged_string)({ char* _temp764=( char*)"::"; struct
_tagged_string _temp765; _temp765.curr= _temp764; _temp765.base= _temp764;
_temp765.last_plus_one= _temp764 + 3; _temp765;})));}}} struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc(
struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){ return Cyc_Absynpp_qvar2doc(
v);}{ void* _temp766=(* v).f1; struct Cyc_List_List* _temp772; _LL768: if((
unsigned int) _temp766 > 1u?(( struct _tunion_struct*) _temp766)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL773: _temp772=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp766)->f1; goto _LL769;} else{ goto _LL770;} _LL770: goto _LL771; _LL769:
if((( int(*)( int(* cmp)( struct _tagged_string*, struct _tagged_string*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp,
_temp772, Cyc_Absynpp_curr_namespace) == 0){ return Cyc_Absynpp_var2doc((* v).f2);}
else{ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp774=( char*)"/* wrong namespace : */ "; struct _tagged_string _temp775;
_temp775.curr= _temp774; _temp775.base= _temp774; _temp775.last_plus_one=
_temp774 + 25; _temp775;})), Cyc_Absynpp_qvar2doc( v));} _LL771: return Cyc_Absynpp_qvar2doc(
v); _LL767:;}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*
e){ void* _temp776=( void*) e->r; void* _temp848; void* _temp850; struct _tuple0*
_temp852; struct _tuple0* _temp854; struct Cyc_List_List* _temp856; void*
_temp858; struct Cyc_Absyn_Exp* _temp860; struct Cyc_Core_Opt* _temp862; struct
Cyc_Absyn_Exp* _temp864; void* _temp866; struct Cyc_Absyn_Exp* _temp868; struct
Cyc_Absyn_Exp* _temp870; struct Cyc_Absyn_Exp* _temp872; struct Cyc_Absyn_Exp*
_temp874; struct Cyc_Absyn_Exp* _temp876; struct Cyc_Absyn_Exp* _temp878; struct
Cyc_List_List* _temp880; struct Cyc_Absyn_Exp* _temp882; struct Cyc_List_List*
_temp884; struct Cyc_Absyn_Exp* _temp886; struct Cyc_Absyn_Exp* _temp888; struct
Cyc_Absyn_Exp* _temp890; struct Cyc_List_List* _temp892; struct Cyc_Absyn_Exp*
_temp894; struct Cyc_Absyn_Exp* _temp896; void* _temp898; struct Cyc_Absyn_Exp*
_temp900; struct Cyc_Absyn_Exp* _temp902; void* _temp904; struct Cyc_Absyn_Exp*
_temp906; struct Cyc_Absyn_Exp* _temp908; struct _tagged_string* _temp910;
struct Cyc_Absyn_Exp* _temp912; struct _tagged_string* _temp914; struct Cyc_Absyn_Exp*
_temp916; struct Cyc_Absyn_Exp* _temp918; struct Cyc_Absyn_Exp* _temp920; struct
Cyc_List_List* _temp922; struct Cyc_List_List* _temp924; struct _tuple1*
_temp926; struct Cyc_List_List* _temp928; struct Cyc_Absyn_Exp* _temp930; struct
Cyc_Absyn_Exp* _temp932; struct Cyc_Absyn_Vardecl* _temp934; struct Cyc_Absyn_Structdecl*
_temp936; struct Cyc_List_List* _temp938; struct Cyc_Core_Opt* _temp940; struct
_tuple0* _temp942; struct Cyc_Absyn_Tunionfield* _temp944; struct Cyc_Absyn_Tuniondecl*
_temp946; struct Cyc_List_List* _temp948; struct Cyc_Core_Opt* _temp950; struct
Cyc_Core_Opt* _temp952; struct Cyc_Absyn_Tunionfield* _temp954; struct Cyc_Absyn_XTuniondecl*
_temp956; struct Cyc_List_List* _temp958; struct Cyc_Core_Opt* _temp960; struct
Cyc_Absyn_Enumfield* _temp962; struct Cyc_Absyn_Enumdecl* _temp964; struct
_tuple0* _temp966; void* _temp968; struct Cyc_List_List* _temp970; struct Cyc_Core_Opt*
_temp972; struct Cyc_Absyn_Stmt* _temp974; struct Cyc_Absyn_Fndecl* _temp976;
struct Cyc_Absyn_Exp* _temp978; _LL778: if((( struct _tunion_struct*) _temp776)->tag
== Cyc_Absyn_Const_e_tag){ _LL849: _temp848=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp776)->f1; goto _LL779;} else{ goto _LL780;} _LL780: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_Var_e_tag){ _LL853: _temp852=(
struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp776)->f1; goto _LL851;
_LL851: _temp850=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp776)->f2; goto
_LL781;} else{ goto _LL782;} _LL782: if((( struct _tunion_struct*) _temp776)->tag
== Cyc_Absyn_UnknownId_e_tag){ _LL855: _temp854=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp776)->f1; goto _LL783;} else{ goto _LL784;} _LL784: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_Primop_e_tag){ _LL859: _temp858=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp776)->f1; goto _LL857; _LL857:
_temp856=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp776)->f2;
goto _LL785;} else{ goto _LL786;} _LL786: if((( struct _tunion_struct*) _temp776)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL865: _temp864=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp776)->f1; goto _LL863; _LL863: _temp862=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp776)->f2; goto
_LL861; _LL861: _temp860=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp776)->f3; goto _LL787;} else{ goto _LL788;} _LL788: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_Increment_e_tag){ _LL869: _temp868=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp776)->f1;
goto _LL867; _LL867: _temp866=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp776)->f2; goto _LL789;} else{ goto _LL790;} _LL790: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_Conditional_e_tag){ _LL875:
_temp874=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp776)->f1; goto _LL873; _LL873: _temp872=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp776)->f2; goto _LL871; _LL871: _temp870=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp776)->f3;
goto _LL791;} else{ goto _LL792;} _LL792: if((( struct _tunion_struct*) _temp776)->tag
== Cyc_Absyn_SeqExp_e_tag){ _LL879: _temp878=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp776)->f1; goto _LL877; _LL877: _temp876=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp776)->f2; goto
_LL793;} else{ goto _LL794;} _LL794: if((( struct _tunion_struct*) _temp776)->tag
== Cyc_Absyn_UnknownCall_e_tag){ _LL883: _temp882=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp776)->f1; goto _LL881; _LL881:
_temp880=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp776)->f2; goto _LL795;} else{ goto _LL796;} _LL796: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_FnCall_e_tag){ _LL887: _temp886=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp776)->f1; goto
_LL885; _LL885: _temp884=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp776)->f2; goto _LL797;} else{ goto _LL798;} _LL798: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_Throw_e_tag){ _LL889: _temp888=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp776)->f1; goto
_LL799;} else{ goto _LL800;} _LL800: if((( struct _tunion_struct*) _temp776)->tag
== Cyc_Absyn_NoInstantiate_e_tag){ _LL891: _temp890=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp776)->f1; goto _LL801;} else{
goto _LL802;} _LL802: if((( struct _tunion_struct*) _temp776)->tag == Cyc_Absyn_Instantiate_e_tag){
_LL895: _temp894=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp776)->f1; goto _LL893; _LL893: _temp892=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp776)->f2; goto _LL803;} else{ goto _LL804;}
_LL804: if((( struct _tunion_struct*) _temp776)->tag == Cyc_Absyn_Cast_e_tag){
_LL899: _temp898=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp776)->f1; goto
_LL897; _LL897: _temp896=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp776)->f2; goto _LL805;} else{ goto _LL806;} _LL806: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_New_e_tag){ _LL901: _temp900=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp776)->f1; goto
_LL807;} else{ goto _LL808;} _LL808: if((( struct _tunion_struct*) _temp776)->tag
== Cyc_Absyn_Address_e_tag){ _LL903: _temp902=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Address_e_struct*) _temp776)->f1; goto _LL809;} else{ goto _LL810;}
_LL810: if((( struct _tunion_struct*) _temp776)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL905: _temp904=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp776)->f1;
goto _LL811;} else{ goto _LL812;} _LL812: if((( struct _tunion_struct*) _temp776)->tag
== Cyc_Absyn_Sizeofexp_e_tag){ _LL907: _temp906=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp776)->f1; goto _LL813;} else{ goto
_LL814;} _LL814: if((( struct _tunion_struct*) _temp776)->tag == Cyc_Absyn_Deref_e_tag){
_LL909: _temp908=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp776)->f1; goto _LL815;} else{ goto _LL816;} _LL816: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_StructMember_e_tag){ _LL913:
_temp912=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp776)->f1; goto _LL911; _LL911: _temp910=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp776)->f2; goto _LL817;} else{ goto _LL818;}
_LL818: if((( struct _tunion_struct*) _temp776)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL917: _temp916=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp776)->f1; goto _LL915; _LL915: _temp914=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp776)->f2; goto _LL819;} else{ goto _LL820;}
_LL820: if((( struct _tunion_struct*) _temp776)->tag == Cyc_Absyn_Subscript_e_tag){
_LL921: _temp920=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp776)->f1; goto _LL919; _LL919: _temp918=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp776)->f2; goto _LL821;} else{ goto _LL822;}
_LL822: if((( struct _tunion_struct*) _temp776)->tag == Cyc_Absyn_Tuple_e_tag){
_LL923: _temp922=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp776)->f1; goto _LL823;} else{ goto _LL824;} _LL824: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL927:
_temp926=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp776)->f1;
goto _LL925; _LL925: _temp924=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp776)->f2; goto _LL825;} else{ goto _LL826;} _LL826: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_Array_e_tag){ _LL929: _temp928=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp776)->f1; goto
_LL827;} else{ goto _LL828;} _LL828: if((( struct _tunion_struct*) _temp776)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL935: _temp934=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp776)->f1; goto _LL933; _LL933:
_temp932=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp776)->f2; goto _LL931; _LL931: _temp930=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp776)->f3; goto _LL829;} else{ goto
_LL830;} _LL830: if((( struct _tunion_struct*) _temp776)->tag == Cyc_Absyn_Struct_e_tag){
_LL943: _temp942=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp776)->f1; goto _LL941; _LL941: _temp940=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp776)->f2; goto _LL939; _LL939: _temp938=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp776)->f3; goto
_LL937; _LL937: _temp936=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp776)->f4; goto _LL831;} else{ goto _LL832;} _LL832: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_Tunion_e_tag){ _LL953: _temp952=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp776)->f1; goto
_LL951; _LL951: _temp950=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp776)->f2; goto _LL949; _LL949: _temp948=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp776)->f3; goto _LL947; _LL947: _temp946=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp776)->f4;
goto _LL945; _LL945: _temp944=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp776)->f5; goto _LL833;} else{ goto _LL834;} _LL834: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_XTunion_e_tag){ _LL961: _temp960=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp776)->f1; goto
_LL959; _LL959: _temp958=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp776)->f2; goto _LL957; _LL957: _temp956=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp776)->f3; goto _LL955; _LL955: _temp954=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp776)->f4;
goto _LL835;} else{ goto _LL836;} _LL836: if((( struct _tunion_struct*) _temp776)->tag
== Cyc_Absyn_Enum_e_tag){ _LL967: _temp966=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp776)->f1; goto _LL965; _LL965: _temp964=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp776)->f2; goto _LL963; _LL963: _temp962=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp776)->f3;
goto _LL837;} else{ goto _LL838;} _LL838: if((( struct _tunion_struct*) _temp776)->tag
== Cyc_Absyn_Malloc_e_tag){ _LL969: _temp968=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp776)->f1; goto _LL839;} else{ goto _LL840;} _LL840: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL973:
_temp972=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp776)->f1; goto _LL971; _LL971: _temp970=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp776)->f2; goto _LL841;} else{ goto
_LL842;} _LL842: if((( struct _tunion_struct*) _temp776)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL975: _temp974=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp776)->f1; goto _LL843;} else{ goto _LL844;} _LL844: if((( struct
_tunion_struct*) _temp776)->tag == Cyc_Absyn_Codegen_e_tag){ _LL977: _temp976=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp776)->f1;
goto _LL845;} else{ goto _LL846;} _LL846: if((( struct _tunion_struct*) _temp776)->tag
== Cyc_Absyn_Fill_e_tag){ _LL979: _temp978=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp776)->f1; goto _LL847;} else{ goto _LL777;} _LL779: return 10000; _LL781:
return 10000; _LL783: return 10000; _LL785: { void* _temp980= _temp858; _LL982:
if(( int) _temp980 == Cyc_Absyn_Plus){ goto _LL983;} else{ goto _LL984;} _LL984:
if(( int) _temp980 == Cyc_Absyn_Times){ goto _LL985;} else{ goto _LL986;} _LL986:
if(( int) _temp980 == Cyc_Absyn_Minus){ goto _LL987;} else{ goto _LL988;} _LL988:
if(( int) _temp980 == Cyc_Absyn_Div){ goto _LL989;} else{ goto _LL990;} _LL990:
if(( int) _temp980 == Cyc_Absyn_Mod){ goto _LL991;} else{ goto _LL992;} _LL992:
if(( int) _temp980 == Cyc_Absyn_Eq){ goto _LL993;} else{ goto _LL994;} _LL994:
if(( int) _temp980 == Cyc_Absyn_Neq){ goto _LL995;} else{ goto _LL996;} _LL996:
if(( int) _temp980 == Cyc_Absyn_Gt){ goto _LL997;} else{ goto _LL998;} _LL998:
if(( int) _temp980 == Cyc_Absyn_Lt){ goto _LL999;} else{ goto _LL1000;} _LL1000:
if(( int) _temp980 == Cyc_Absyn_Gte){ goto _LL1001;} else{ goto _LL1002;}
_LL1002: if(( int) _temp980 == Cyc_Absyn_Lte){ goto _LL1003;} else{ goto _LL1004;}
_LL1004: if(( int) _temp980 == Cyc_Absyn_Not){ goto _LL1005;} else{ goto _LL1006;}
_LL1006: if(( int) _temp980 == Cyc_Absyn_Bitnot){ goto _LL1007;} else{ goto
_LL1008;} _LL1008: if(( int) _temp980 == Cyc_Absyn_Bitand){ goto _LL1009;} else{
goto _LL1010;} _LL1010: if(( int) _temp980 == Cyc_Absyn_Bitor){ goto _LL1011;}
else{ goto _LL1012;} _LL1012: if(( int) _temp980 == Cyc_Absyn_Bitxor){ goto
_LL1013;} else{ goto _LL1014;} _LL1014: if(( int) _temp980 == Cyc_Absyn_Bitlshift){
goto _LL1015;} else{ goto _LL1016;} _LL1016: if(( int) _temp980 == Cyc_Absyn_Bitlrshift){
goto _LL1017;} else{ goto _LL1018;} _LL1018: if(( int) _temp980 == Cyc_Absyn_Bitarshift){
goto _LL1019;} else{ goto _LL1020;} _LL1020: if(( int) _temp980 == Cyc_Absyn_Size){
goto _LL1021;} else{ goto _LL1022;} _LL1022: if(( int) _temp980 == Cyc_Absyn_Printf){
goto _LL1023;} else{ goto _LL1024;} _LL1024: if(( int) _temp980 == Cyc_Absyn_Fprintf){
goto _LL1025;} else{ goto _LL1026;} _LL1026: if(( int) _temp980 == Cyc_Absyn_Xprintf){
goto _LL1027;} else{ goto _LL1028;} _LL1028: if(( int) _temp980 == Cyc_Absyn_Scanf){
goto _LL1029;} else{ goto _LL1030;} _LL1030: if(( int) _temp980 == Cyc_Absyn_Fscanf){
goto _LL1031;} else{ goto _LL1032;} _LL1032: if(( int) _temp980 == Cyc_Absyn_Sscanf){
goto _LL1033;} else{ goto _LL981;} _LL983: return 100; _LL985: return 110;
_LL987: return 100; _LL989: return 110; _LL991: return 110; _LL993: return 70;
_LL995: return 70; _LL997: return 80; _LL999: return 80; _LL1001: return 80;
_LL1003: return 80; _LL1005: return 130; _LL1007: return 130; _LL1009: return 60;
_LL1011: return 40; _LL1013: return 50; _LL1015: return 90; _LL1017: return 80;
_LL1019: return 80; _LL1021: return 140; _LL1023: return 130; _LL1025: return
130; _LL1027: return 130; _LL1029: return 130; _LL1031: return 130; _LL1033:
return 130; _LL981:;} _LL787: return 20; _LL789: return 20; _LL791: return 30;
_LL793: return 10; _LL795: return 140; _LL797: return 140; _LL799: return 130;
_LL801: return Cyc_Absynpp_exp_prec( _temp890); _LL803: return Cyc_Absynpp_exp_prec(
_temp894); _LL805: return 120; _LL807: return 15; _LL809: return 130; _LL811:
return 130; _LL813: return 130; _LL815: return 130; _LL817: return 140; _LL819:
return 140; _LL821: return 140; _LL823: return 150; _LL825: goto _LL827; _LL827:
goto _LL829; _LL829: goto _LL831; _LL831: goto _LL833; _LL833: goto _LL835;
_LL835: goto _LL837; _LL837: goto _LL839; _LL839: return 140; _LL841: return 140;
_LL843: return 10000; _LL845: return 140; _LL847: return 140; _LL777:;} struct
Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp1034=(
void*) e->r; void* _temp1110; void* _temp1112; struct Cyc_Absyn_Vardecl*
_temp1114; struct _tuple0* _temp1116; void* _temp1118; struct Cyc_Absyn_Vardecl*
_temp1120; struct _tuple0* _temp1122; void* _temp1124; struct _tuple0* _temp1126;
struct _tuple0* _temp1128; struct Cyc_List_List* _temp1130; void* _temp1132;
struct Cyc_Absyn_Exp* _temp1134; struct Cyc_Core_Opt* _temp1136; struct Cyc_Absyn_Exp*
_temp1138; void* _temp1140; struct Cyc_Absyn_Exp* _temp1142; struct Cyc_Absyn_Exp*
_temp1144; struct Cyc_Absyn_Exp* _temp1146; struct Cyc_Absyn_Exp* _temp1148;
struct Cyc_Absyn_Exp* _temp1150; struct Cyc_Absyn_Exp* _temp1152; struct Cyc_List_List*
_temp1154; struct Cyc_Absyn_Exp* _temp1156; struct Cyc_List_List* _temp1158;
struct Cyc_Absyn_Exp* _temp1160; struct Cyc_Absyn_Exp* _temp1162; struct Cyc_Absyn_Exp*
_temp1164; struct Cyc_List_List* _temp1166; struct Cyc_Absyn_Exp* _temp1168;
struct Cyc_Absyn_Exp* _temp1170; void* _temp1172; struct Cyc_Absyn_Exp*
_temp1174; struct Cyc_Absyn_Exp* _temp1176; void* _temp1178; struct Cyc_Absyn_Exp*
_temp1180; struct Cyc_Absyn_Exp* _temp1182; struct _tagged_string* _temp1184;
struct Cyc_Absyn_Exp* _temp1186; struct _tagged_string* _temp1188; struct Cyc_Absyn_Exp*
_temp1190; struct Cyc_Absyn_Exp* _temp1192; struct Cyc_Absyn_Exp* _temp1194;
struct Cyc_List_List* _temp1196; struct Cyc_List_List* _temp1198; struct _tuple1*
_temp1200; struct Cyc_List_List* _temp1202; struct Cyc_Absyn_Exp* _temp1204;
struct Cyc_Absyn_Exp* _temp1206; struct Cyc_Absyn_Vardecl* _temp1208; struct Cyc_Absyn_Structdecl*
_temp1210; struct Cyc_List_List* _temp1212; struct Cyc_Core_Opt* _temp1214;
struct _tuple0* _temp1216; struct Cyc_Absyn_Tunionfield* _temp1218; struct Cyc_Absyn_Tuniondecl*
_temp1220; struct Cyc_List_List* _temp1222; struct Cyc_Core_Opt* _temp1224;
struct Cyc_Core_Opt* _temp1226; struct Cyc_Absyn_Tunionfield* _temp1228; struct
Cyc_Absyn_XTuniondecl* _temp1230; struct Cyc_List_List* _temp1232; struct Cyc_Core_Opt*
_temp1234; struct Cyc_Absyn_Enumfield* _temp1236; struct Cyc_Absyn_Enumdecl*
_temp1238; struct _tuple0* _temp1240; void* _temp1242; struct Cyc_List_List*
_temp1244; struct Cyc_Core_Opt* _temp1246; struct Cyc_Absyn_Stmt* _temp1248;
struct Cyc_Absyn_Fndecl* _temp1250; struct Cyc_Absyn_Exp* _temp1252; _LL1036:
if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_Const_e_tag){ _LL1111:
_temp1110=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1034)->f1; goto
_LL1037;} else{ goto _LL1038;} _LL1038: if((( struct _tunion_struct*) _temp1034)->tag
== Cyc_Absyn_Var_e_tag){ _LL1117: _temp1116=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1034)->f1; goto _LL1113; _LL1113: _temp1112=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1034)->f2; if(( unsigned int) _temp1112 > 1u?(( struct _tunion_struct*)
_temp1112)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL1115: _temp1114=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp1112)->f1; goto _LL1039;} else{ goto
_LL1040;}} else{ goto _LL1040;} _LL1040: if((( struct _tunion_struct*) _temp1034)->tag
== Cyc_Absyn_Var_e_tag){ _LL1123: _temp1122=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1034)->f1; goto _LL1119; _LL1119: _temp1118=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1034)->f2; if(( unsigned int) _temp1118 > 1u?(( struct _tunion_struct*)
_temp1118)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL1121: _temp1120=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp1118)->f1; goto _LL1041;} else{ goto
_LL1042;}} else{ goto _LL1042;} _LL1042: if((( struct _tunion_struct*) _temp1034)->tag
== Cyc_Absyn_Var_e_tag){ _LL1127: _temp1126=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1034)->f1; goto _LL1125; _LL1125: _temp1124=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1034)->f2; goto _LL1043;} else{ goto _LL1044;} _LL1044: if((( struct
_tunion_struct*) _temp1034)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1129:
_temp1128=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1034)->f1;
goto _LL1045;} else{ goto _LL1046;} _LL1046: if((( struct _tunion_struct*)
_temp1034)->tag == Cyc_Absyn_Primop_e_tag){ _LL1133: _temp1132=( void*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp1034)->f1; goto _LL1131; _LL1131: _temp1130=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1034)->f2; goto
_LL1047;} else{ goto _LL1048;} _LL1048: if((( struct _tunion_struct*) _temp1034)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL1139: _temp1138=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1034)->f1; goto _LL1137; _LL1137:
_temp1136=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1034)->f2; goto _LL1135; _LL1135: _temp1134=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1034)->f3; goto _LL1049;} else{ goto
_LL1050;} _LL1050: if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_Increment_e_tag){
_LL1143: _temp1142=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1034)->f1; goto _LL1141; _LL1141: _temp1140=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1034)->f2; goto _LL1051;} else{ goto _LL1052;} _LL1052: if((( struct
_tunion_struct*) _temp1034)->tag == Cyc_Absyn_Conditional_e_tag){ _LL1149:
_temp1148=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1034)->f1; goto _LL1147; _LL1147: _temp1146=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1034)->f2; goto _LL1145; _LL1145:
_temp1144=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1034)->f3; goto _LL1053;} else{ goto _LL1054;} _LL1054: if((( struct
_tunion_struct*) _temp1034)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1153: _temp1152=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1034)->f1; goto
_LL1151; _LL1151: _temp1150=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1034)->f2; goto _LL1055;} else{ goto _LL1056;} _LL1056: if((( struct
_tunion_struct*) _temp1034)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1157:
_temp1156=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1034)->f1; goto _LL1155; _LL1155: _temp1154=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1034)->f2; goto _LL1057;} else{
goto _LL1058;} _LL1058: if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1161: _temp1160=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1034)->f1; goto _LL1159; _LL1159: _temp1158=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1034)->f2; goto _LL1059;} else{ goto
_LL1060;} _LL1060: if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_Throw_e_tag){
_LL1163: _temp1162=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp1034)->f1; goto _LL1061;} else{ goto _LL1062;} _LL1062: if((( struct
_tunion_struct*) _temp1034)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1165:
_temp1164=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1034)->f1; goto _LL1063;} else{ goto _LL1064;} _LL1064: if((( struct
_tunion_struct*) _temp1034)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1169:
_temp1168=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1034)->f1; goto _LL1167; _LL1167: _temp1166=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1034)->f2; goto _LL1065;} else{
goto _LL1066;} _LL1066: if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_Cast_e_tag){
_LL1173: _temp1172=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1034)->f1;
goto _LL1171; _LL1171: _temp1170=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1034)->f2; goto _LL1067;} else{ goto _LL1068;} _LL1068: if((( struct
_tunion_struct*) _temp1034)->tag == Cyc_Absyn_Address_e_tag){ _LL1175: _temp1174=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp1034)->f1;
goto _LL1069;} else{ goto _LL1070;} _LL1070: if((( struct _tunion_struct*)
_temp1034)->tag == Cyc_Absyn_New_e_tag){ _LL1177: _temp1176=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1034)->f1; goto _LL1071;} else{ goto
_LL1072;} _LL1072: if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL1179: _temp1178=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1034)->f1;
goto _LL1073;} else{ goto _LL1074;} _LL1074: if((( struct _tunion_struct*)
_temp1034)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL1181: _temp1180=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp1034)->f1; goto _LL1075;} else{ goto
_LL1076;} _LL1076: if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_Deref_e_tag){
_LL1183: _temp1182=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp1034)->f1; goto _LL1077;} else{ goto _LL1078;} _LL1078: if((( struct
_tunion_struct*) _temp1034)->tag == Cyc_Absyn_StructMember_e_tag){ _LL1187:
_temp1186=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1034)->f1; goto _LL1185; _LL1185: _temp1184=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1034)->f2; goto _LL1079;} else{
goto _LL1080;} _LL1080: if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1191: _temp1190=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1034)->f1; goto _LL1189; _LL1189: _temp1188=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1034)->f2; goto _LL1081;} else{
goto _LL1082;} _LL1082: if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1195: _temp1194=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1034)->f1; goto _LL1193; _LL1193: _temp1192=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1034)->f2; goto _LL1083;} else{ goto
_LL1084;} _LL1084: if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1197: _temp1196=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1034)->f1; goto _LL1085;} else{ goto _LL1086;} _LL1086: if((( struct
_tunion_struct*) _temp1034)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1201:
_temp1200=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1034)->f1;
goto _LL1199; _LL1199: _temp1198=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1034)->f2; goto _LL1087;} else{ goto _LL1088;} _LL1088: if((( struct
_tunion_struct*) _temp1034)->tag == Cyc_Absyn_Array_e_tag){ _LL1203: _temp1202=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1034)->f1; goto
_LL1089;} else{ goto _LL1090;} _LL1090: if((( struct _tunion_struct*) _temp1034)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1209: _temp1208=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1034)->f1; goto _LL1207; _LL1207:
_temp1206=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1034)->f2; goto _LL1205; _LL1205: _temp1204=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1034)->f3; goto _LL1091;} else{
goto _LL1092;} _LL1092: if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_Struct_e_tag){
_LL1217: _temp1216=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1034)->f1; goto _LL1215; _LL1215: _temp1214=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1034)->f2; goto _LL1213; _LL1213:
_temp1212=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1034)->f3; goto _LL1211; _LL1211: _temp1210=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1034)->f4; goto _LL1093;} else{ goto
_LL1094;} _LL1094: if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_Tunion_e_tag){
_LL1227: _temp1226=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1034)->f1; goto _LL1225; _LL1225: _temp1224=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1034)->f2; goto _LL1223; _LL1223:
_temp1222=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1034)->f3; goto _LL1221; _LL1221: _temp1220=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1034)->f4; goto _LL1219; _LL1219:
_temp1218=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1034)->f5; goto _LL1095;} else{ goto _LL1096;} _LL1096: if((( struct
_tunion_struct*) _temp1034)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1235: _temp1234=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1034)->f1; goto
_LL1233; _LL1233: _temp1232=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1034)->f2; goto _LL1231; _LL1231: _temp1230=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1034)->f3; goto _LL1229; _LL1229:
_temp1228=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1034)->f4; goto _LL1097;} else{ goto _LL1098;} _LL1098: if((( struct
_tunion_struct*) _temp1034)->tag == Cyc_Absyn_Enum_e_tag){ _LL1241: _temp1240=(
struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1034)->f1; goto _LL1239;
_LL1239: _temp1238=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1034)->f2; goto _LL1237; _LL1237: _temp1236=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1034)->f3; goto _LL1099;} else{ goto
_LL1100;} _LL1100: if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1243: _temp1242=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp1034)->f1;
goto _LL1101;} else{ goto _LL1102;} _LL1102: if((( struct _tunion_struct*)
_temp1034)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1247: _temp1246=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1034)->f1; goto
_LL1245; _LL1245: _temp1244=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1034)->f2; goto _LL1103;} else{ goto _LL1104;} _LL1104: if((( struct
_tunion_struct*) _temp1034)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1249: _temp1248=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1034)->f1;
goto _LL1105;} else{ goto _LL1106;} _LL1106: if((( struct _tunion_struct*)
_temp1034)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1251: _temp1250=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp1034)->f1; goto _LL1107;} else{ goto
_LL1108;} _LL1108: if((( struct _tunion_struct*) _temp1034)->tag == Cyc_Absyn_Fill_e_tag){
_LL1253: _temp1252=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp1034)->f1; goto _LL1109;} else{ goto _LL1035;} _LL1037: s= Cyc_Absynpp_cnst2doc(
_temp1110); goto _LL1035; _LL1039: s= Cyc_Absynpp_qvar2doc( _temp1116); goto
_LL1035; _LL1041: s= Cyc_Absynpp_qvar2doc( _temp1122); goto _LL1035; _LL1043: s=
Cyc_Absynpp_qvar2doc( _temp1126); goto _LL1035; _LL1045: s= Cyc_Absynpp_qvar2doc(
_temp1128); goto _LL1035; _LL1047: s= Cyc_Absynpp_primapp2doc( myprec, _temp1132,
_temp1130); goto _LL1035; _LL1049: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1138), Cyc_PP_text(( struct _tagged_string)({ char* _temp1254=(
char*)" "; struct _tagged_string _temp1255; _temp1255.curr= _temp1254; _temp1255.base=
_temp1254; _temp1255.last_plus_one= _temp1254 + 2; _temp1255;})), _temp1136 == 0?
Cyc_PP_text(( struct _tagged_string)({ char* _temp1256=( char*)""; struct
_tagged_string _temp1257; _temp1257.curr= _temp1256; _temp1257.base= _temp1256;
_temp1257.last_plus_one= _temp1256 + 1; _temp1257;})): Cyc_Absynpp_prim2doc((
void*)({ struct Cyc_Core_Opt* _temp1258= _temp1136; if( _temp1258 == 0){ _throw(
Null_Exception);} _temp1258->v;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1259=( char*)"= "; struct _tagged_string _temp1260; _temp1260.curr=
_temp1259; _temp1260.base= _temp1259; _temp1260.last_plus_one= _temp1259 + 3;
_temp1260;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1134)); goto _LL1035;
_LL1051: { struct Cyc_PP_Doc* es= Cyc_Absynpp_exp2doc_prec( myprec, _temp1142);{
void* _temp1261= _temp1140; _LL1263: if(( int) _temp1261 == Cyc_Absyn_PreInc){
goto _LL1264;} else{ goto _LL1265;} _LL1265: if(( int) _temp1261 == Cyc_Absyn_PreDec){
goto _LL1266;} else{ goto _LL1267;} _LL1267: if(( int) _temp1261 == Cyc_Absyn_PostInc){
goto _LL1268;} else{ goto _LL1269;} _LL1269: if(( int) _temp1261 == Cyc_Absyn_PostDec){
goto _LL1270;} else{ goto _LL1262;} _LL1264: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1271=( char*)"++"; struct _tagged_string
_temp1272; _temp1272.curr= _temp1271; _temp1272.base= _temp1271; _temp1272.last_plus_one=
_temp1271 + 3; _temp1272;})), es); goto _LL1262; _LL1266: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1273=( char*)"--"; struct
_tagged_string _temp1274; _temp1274.curr= _temp1273; _temp1274.base= _temp1273;
_temp1274.last_plus_one= _temp1273 + 3; _temp1274;})), es); goto _LL1262;
_LL1268: s= Cyc_Absynpp_cat2( es, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1275=( char*)"++"; struct _tagged_string _temp1276; _temp1276.curr=
_temp1275; _temp1276.base= _temp1275; _temp1276.last_plus_one= _temp1275 + 3;
_temp1276;}))); goto _LL1262; _LL1270: s= Cyc_Absynpp_cat2( es, Cyc_PP_text((
struct _tagged_string)({ char* _temp1277=( char*)"--"; struct _tagged_string
_temp1278; _temp1278.curr= _temp1277; _temp1278.base= _temp1277; _temp1278.last_plus_one=
_temp1277 + 3; _temp1278;}))); goto _LL1262; _LL1262:;} goto _LL1035;} _LL1053:{
struct _tuple8 _temp1280=({ struct _tuple8 _temp1279; _temp1279.f1=( void*)
_temp1146->r; _temp1279.f2=( void*) _temp1144->r; _temp1279;}); _LL1282: goto
_LL1283; _LL1283: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1148), Cyc_PP_text(( struct _tagged_string)({ char* _temp1284=( char*)" ? ";
struct _tagged_string _temp1285; _temp1285.curr= _temp1284; _temp1285.base=
_temp1284; _temp1285.last_plus_one= _temp1284 + 4; _temp1285;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1146), Cyc_PP_text(( struct _tagged_string)({ char* _temp1286=( char*)" : ";
struct _tagged_string _temp1287; _temp1287.curr= _temp1286; _temp1287.base=
_temp1286; _temp1287.last_plus_one= _temp1286 + 4; _temp1287;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1144)); goto _LL1281; _LL1281:;} goto _LL1035; _LL1055: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1152), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1288=( char*)", "; struct _tagged_string _temp1289;
_temp1289.curr= _temp1288; _temp1289.base= _temp1288; _temp1289.last_plus_one=
_temp1288 + 3; _temp1289;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1150));
goto _LL1035; _LL1057: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1156), Cyc_PP_text(( struct _tagged_string)({ char* _temp1290=( char*)"(";
struct _tagged_string _temp1291; _temp1291.curr= _temp1290; _temp1291.base=
_temp1290; _temp1291.last_plus_one= _temp1290 + 2; _temp1291;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1154), Cyc_PP_text(( struct _tagged_string)({ char* _temp1292=( char*)")";
struct _tagged_string _temp1293; _temp1293.curr= _temp1292; _temp1293.base=
_temp1292; _temp1293.last_plus_one= _temp1292 + 2; _temp1293;}))); goto _LL1035;
_LL1059: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1160), Cyc_PP_text((
struct _tagged_string)({ char* _temp1294=( char*)"("; struct _tagged_string
_temp1295; _temp1295.curr= _temp1294; _temp1295.base= _temp1294; _temp1295.last_plus_one=
_temp1294 + 2; _temp1295;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1158), Cyc_PP_text((
struct _tagged_string)({ char* _temp1296=( char*)")"; struct _tagged_string
_temp1297; _temp1297.curr= _temp1296; _temp1297.base= _temp1296; _temp1297.last_plus_one=
_temp1296 + 2; _temp1297;}))); goto _LL1035; _LL1061: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1298=( char*)"throw "; struct _tagged_string
_temp1299; _temp1299.curr= _temp1298; _temp1299.base= _temp1298; _temp1299.last_plus_one=
_temp1298 + 7; _temp1299;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1162));
goto _LL1035; _LL1063: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1164); goto
_LL1035; _LL1065: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1168); goto _LL1035;
_LL1067: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1300=( char*)"("; struct _tagged_string _temp1301; _temp1301.curr=
_temp1300; _temp1301.base= _temp1300; _temp1301.last_plus_one= _temp1300 + 2;
_temp1301;})), Cyc_Absynpp_typ2doc( _temp1172), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1302=( char*)")"; struct _tagged_string _temp1303;
_temp1303.curr= _temp1302; _temp1303.base= _temp1302; _temp1303.last_plus_one=
_temp1302 + 2; _temp1303;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1170));
goto _LL1035; _LL1069: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({
char* _temp1304=( char*)"&"; struct _tagged_string _temp1305; _temp1305.curr=
_temp1304; _temp1305.base= _temp1304; _temp1305.last_plus_one= _temp1304 + 2;
_temp1305;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1174)); goto _LL1035;
_LL1071: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1306=( char*)"new "; struct _tagged_string _temp1307; _temp1307.curr=
_temp1306; _temp1307.base= _temp1306; _temp1307.last_plus_one= _temp1306 + 5;
_temp1307;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1176)); goto _LL1035;
_LL1073: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1308=( char*)"sizeof("; struct _tagged_string _temp1309; _temp1309.curr=
_temp1308; _temp1309.base= _temp1308; _temp1309.last_plus_one= _temp1308 + 8;
_temp1309;})), Cyc_Absynpp_typ2doc( _temp1178), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1310=( char*)")"; struct _tagged_string _temp1311;
_temp1311.curr= _temp1310; _temp1311.base= _temp1310; _temp1311.last_plus_one=
_temp1310 + 2; _temp1311;}))); goto _LL1035; _LL1075: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1312=( char*)"sizeof("; struct
_tagged_string _temp1313; _temp1313.curr= _temp1312; _temp1313.base= _temp1312;
_temp1313.last_plus_one= _temp1312 + 8; _temp1313;})), Cyc_Absynpp_exp2doc(
_temp1180), Cyc_PP_text(( struct _tagged_string)({ char* _temp1314=( char*)")";
struct _tagged_string _temp1315; _temp1315.curr= _temp1314; _temp1315.base=
_temp1314; _temp1315.last_plus_one= _temp1314 + 2; _temp1315;}))); goto _LL1035;
_LL1077: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1316=( char*)"*"; struct _tagged_string _temp1317; _temp1317.curr=
_temp1316; _temp1317.base= _temp1316; _temp1317.last_plus_one= _temp1316 + 2;
_temp1317;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1182)); goto _LL1035;
_LL1079: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec, _temp1186), Cyc_PP_text((
struct _tagged_string)({ char* _temp1318=( char*)"."; struct _tagged_string
_temp1319; _temp1319.curr= _temp1318; _temp1319.base= _temp1318; _temp1319.last_plus_one=
_temp1318 + 2; _temp1319;})), Cyc_Absynpp_textptr( _temp1184)); goto _LL1035;
_LL1081: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec, _temp1190), Cyc_PP_text((
struct _tagged_string)({ char* _temp1320=( char*)"->"; struct _tagged_string
_temp1321; _temp1321.curr= _temp1320; _temp1321.base= _temp1320; _temp1321.last_plus_one=
_temp1320 + 3; _temp1321;})), Cyc_Absynpp_textptr( _temp1188)); goto _LL1035;
_LL1083: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1194), Cyc_PP_text((
struct _tagged_string)({ char* _temp1322=( char*)"["; struct _tagged_string
_temp1323; _temp1323.curr= _temp1322; _temp1323.base= _temp1322; _temp1323.last_plus_one=
_temp1322 + 2; _temp1323;})), Cyc_Absynpp_exp2doc( _temp1192), Cyc_PP_text((
struct _tagged_string)({ char* _temp1324=( char*)"]"; struct _tagged_string
_temp1325; _temp1325.curr= _temp1324; _temp1325.base= _temp1324; _temp1325.last_plus_one=
_temp1324 + 2; _temp1325;}))); goto _LL1035; _LL1085: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1326=( char*)"$("; struct _tagged_string
_temp1327; _temp1327.curr= _temp1326; _temp1327.base= _temp1326; _temp1327.last_plus_one=
_temp1326 + 3; _temp1327;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1196), Cyc_PP_text((
struct _tagged_string)({ char* _temp1328=( char*)")"; struct _tagged_string
_temp1329; _temp1329.curr= _temp1328; _temp1329.base= _temp1328; _temp1329.last_plus_one=
_temp1328 + 2; _temp1329;}))); goto _LL1035; _LL1087: s= Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1330=( char*)"("; struct _tagged_string
_temp1331; _temp1331.curr= _temp1330; _temp1331.base= _temp1330; _temp1331.last_plus_one=
_temp1330 + 2; _temp1331;})), Cyc_Absynpp_typ2doc((* _temp1200).f3), Cyc_PP_text((
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
_temp1198))); goto _LL1035; _LL1089: s= Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1340=( char*)"{"; struct _tagged_string _temp1341;
_temp1341.curr= _temp1340; _temp1341.base= _temp1340; _temp1341.last_plus_one=
_temp1340 + 2; _temp1341;}),( struct _tagged_string)({ char* _temp1342=( char*)"}";
struct _tagged_string _temp1343; _temp1343.curr= _temp1342; _temp1343.base=
_temp1342; _temp1343.last_plus_one= _temp1342 + 2; _temp1343;}),( struct
_tagged_string)({ char* _temp1344=( char*)","; struct _tagged_string _temp1345;
_temp1345.curr= _temp1344; _temp1345.base= _temp1344; _temp1345.last_plus_one=
_temp1344 + 2; _temp1345;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1202)); goto _LL1035; _LL1091: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1346=( char*)"{for "; struct _tagged_string
_temp1347; _temp1347.curr= _temp1346; _temp1347.base= _temp1346; _temp1347.last_plus_one=
_temp1346 + 6; _temp1347;})), Cyc_PP_text(*(* _temp1208->name).f2), Cyc_PP_text((
struct _tagged_string)({ char* _temp1348=( char*)" < "; struct _tagged_string
_temp1349; _temp1349.curr= _temp1348; _temp1349.base= _temp1348; _temp1349.last_plus_one=
_temp1348 + 4; _temp1349;})), Cyc_Absynpp_exp2doc( _temp1206), Cyc_PP_text((
struct _tagged_string)({ char* _temp1350=( char*)" : "; struct _tagged_string
_temp1351; _temp1351.curr= _temp1350; _temp1351.base= _temp1350; _temp1351.last_plus_one=
_temp1350 + 4; _temp1351;})), Cyc_Absynpp_exp2doc( _temp1204), Cyc_PP_text((
struct _tagged_string)({ char* _temp1352=( char*)"}"; struct _tagged_string
_temp1353; _temp1353.curr= _temp1352; _temp1353.base= _temp1352; _temp1353.last_plus_one=
_temp1352 + 2; _temp1353;}))); goto _LL1035; _LL1093: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1216), Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1354=( char*)"{";
struct _tagged_string _temp1355; _temp1355.curr= _temp1354; _temp1355.base=
_temp1354; _temp1355.last_plus_one= _temp1354 + 2; _temp1355;}),( struct
_tagged_string)({ char* _temp1356=( char*)"}"; struct _tagged_string _temp1357;
_temp1357.curr= _temp1356; _temp1357.base= _temp1356; _temp1357.last_plus_one=
_temp1356 + 2; _temp1357;}),( struct _tagged_string)({ char* _temp1358=( char*)",";
struct _tagged_string _temp1359; _temp1359.curr= _temp1358; _temp1359.base=
_temp1358; _temp1359.last_plus_one= _temp1358 + 2; _temp1359;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1212))); goto _LL1035; _LL1095: _temp1232= _temp1222;
_temp1228= _temp1218; goto _LL1097; _LL1097: if( _temp1232 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1228->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1228->name),
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1360=( char*)"(";
struct _tagged_string _temp1361; _temp1361.curr= _temp1360; _temp1361.base=
_temp1360; _temp1361.last_plus_one= _temp1360 + 2; _temp1361;}),( struct
_tagged_string)({ char* _temp1362=( char*)")"; struct _tagged_string _temp1363;
_temp1363.curr= _temp1362; _temp1363.base= _temp1362; _temp1363.last_plus_one=
_temp1362 + 2; _temp1363;}),( struct _tagged_string)({ char* _temp1364=( char*)",";
struct _tagged_string _temp1365; _temp1365.curr= _temp1364; _temp1365.base=
_temp1364; _temp1365.last_plus_one= _temp1364 + 2; _temp1365;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp1232)));} goto _LL1035; _LL1099: s= Cyc_Absynpp_qvar2doc(
_temp1240); goto _LL1035; _LL1101: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1366=( char*)"malloc(sizeof("; struct
_tagged_string _temp1367; _temp1367.curr= _temp1366; _temp1367.base= _temp1366;
_temp1367.last_plus_one= _temp1366 + 15; _temp1367;})), Cyc_Absynpp_typ2doc(
_temp1242), Cyc_PP_text(( struct _tagged_string)({ char* _temp1368=( char*)"))";
struct _tagged_string _temp1369; _temp1369.curr= _temp1368; _temp1369.base=
_temp1368; _temp1369.last_plus_one= _temp1368 + 3; _temp1369;}))); goto _LL1035;
_LL1103: s= Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1370=( char*)"{";
struct _tagged_string _temp1371; _temp1371.curr= _temp1370; _temp1371.base=
_temp1370; _temp1371.last_plus_one= _temp1370 + 2; _temp1371;}),( struct
_tagged_string)({ char* _temp1372=( char*)"}"; struct _tagged_string _temp1373;
_temp1373.curr= _temp1372; _temp1373.base= _temp1372; _temp1373.last_plus_one=
_temp1372 + 2; _temp1373;}),( struct _tagged_string)({ char* _temp1374=( char*)",";
struct _tagged_string _temp1375; _temp1375.curr= _temp1374; _temp1375.base=
_temp1374; _temp1375.last_plus_one= _temp1374 + 2; _temp1375;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1244)); goto _LL1035; _LL1105: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1376=( char*)"({ "; struct _tagged_string
_temp1377; _temp1377.curr= _temp1376; _temp1377.base= _temp1376; _temp1377.last_plus_one=
_temp1376 + 4; _temp1377;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1248)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1378=( char*)" })"; struct
_tagged_string _temp1379; _temp1379.curr= _temp1378; _temp1379.base= _temp1378;
_temp1379.last_plus_one= _temp1378 + 4; _temp1379;}))); goto _LL1035; _LL1107: s=
Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1380=( char*)"codegen(";
struct _tagged_string _temp1381; _temp1381.curr= _temp1380; _temp1381.base=
_temp1380; _temp1381.last_plus_one= _temp1380 + 9; _temp1381;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1382=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1382->r=( void*)(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp1383=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1383[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1384; _temp1384.tag= Cyc_Absyn_Fn_d_tag; _temp1384.f1= _temp1250; _temp1384;});
_temp1383;})); _temp1382->loc= e->loc; _temp1382;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1385=( char*)")"; struct _tagged_string _temp1386;
_temp1386.curr= _temp1385; _temp1386.base= _temp1385; _temp1386.last_plus_one=
_temp1385 + 2; _temp1386;}))); goto _LL1035; _LL1109: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1387=( char*)"fill("; struct _tagged_string
_temp1388; _temp1388.curr= _temp1387; _temp1388.base= _temp1387; _temp1388.last_plus_one=
_temp1387 + 6; _temp1388;})), Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1252)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1389=( char*)")"; struct
_tagged_string _temp1390; _temp1390.curr= _temp1389; _temp1390.base= _temp1389;
_temp1390.last_plus_one= _temp1389 + 2; _temp1390;}))); goto _LL1035; _LL1035:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp1391=( char*)"("; struct _tagged_string _temp1392; _temp1392.curr=
_temp1391; _temp1392.base= _temp1391; _temp1392.last_plus_one= _temp1391 + 2;
_temp1392;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp1393=( char*)")";
struct _tagged_string _temp1394; _temp1394.curr= _temp1393; _temp1394.base=
_temp1393; _temp1394.last_plus_one= _temp1393 + 2; _temp1394;})));} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1395= d;
struct Cyc_Absyn_Exp* _temp1401; struct _tagged_string* _temp1403; _LL1397: if(((
struct _tunion_struct*) _temp1395)->tag == Cyc_Absyn_ArrayElement_tag){ _LL1402:
_temp1401=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1395)->f1; goto _LL1398;} else{ goto _LL1399;} _LL1399: if((( struct
_tunion_struct*) _temp1395)->tag == Cyc_Absyn_FieldName_tag){ _LL1404: _temp1403=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1395)->f1;
goto _LL1400;} else{ goto _LL1396;} _LL1398: return Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1405=( char*)".["; struct _tagged_string
_temp1406; _temp1406.curr= _temp1405; _temp1406.base= _temp1405; _temp1406.last_plus_one=
_temp1405 + 3; _temp1406;})), Cyc_Absynpp_exp2doc( _temp1401), Cyc_PP_text((
struct _tagged_string)({ char* _temp1407=( char*)"]"; struct _tagged_string
_temp1408; _temp1408.curr= _temp1407; _temp1408.base= _temp1407; _temp1408.last_plus_one=
_temp1407 + 2; _temp1408;}))); _LL1400: return Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1409=( char*)"."; struct _tagged_string
_temp1410; _temp1410.curr= _temp1409; _temp1410.base= _temp1409; _temp1410.last_plus_one=
_temp1409 + 2; _temp1410;})), Cyc_Absynpp_textptr( _temp1403)); _LL1396:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 == 0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp1411=( char*)""; struct _tagged_string
_temp1412; _temp1412.curr= _temp1411; _temp1412.base= _temp1411; _temp1412.last_plus_one=
_temp1411 + 1; _temp1412;}),( struct _tagged_string)({ char* _temp1413=( char*)"=";
struct _tagged_string _temp1414; _temp1414.curr= _temp1413; _temp1414.base=
_temp1413; _temp1414.last_plus_one= _temp1413 + 2; _temp1414;}),( struct
_tagged_string)({ char* _temp1415=( char*)"="; struct _tagged_string _temp1416;
_temp1416.curr= _temp1415; _temp1416.base= _temp1415; _temp1416.last_plus_one=
_temp1415 + 2; _temp1416;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)), Cyc_Absynpp_exp2doc((* de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1417=( char*)""; struct _tagged_string _temp1418;
_temp1418.curr= _temp1417; _temp1418.base= _temp1417; _temp1418.last_plus_one=
_temp1417 + 1; _temp1418;}),( struct _tagged_string)({ char* _temp1419=( char*)"";
struct _tagged_string _temp1420; _temp1420.curr= _temp1419; _temp1420.base=
_temp1419; _temp1420.last_plus_one= _temp1419 + 1; _temp1420;}),( struct
_tagged_string)({ char* _temp1421=( char*)","; struct _tagged_string _temp1422;
_temp1422.curr= _temp1421; _temp1422.base= _temp1421; _temp1422.last_plus_one=
_temp1421 + 2; _temp1422;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1423= c; char _temp1441; void* _temp1443; short _temp1445;
void* _temp1447; int _temp1449; void* _temp1451; int _temp1453; void* _temp1455;
long long _temp1457; void* _temp1459; struct _tagged_string _temp1461; struct
_tagged_string _temp1463; _LL1425: if(( unsigned int) _temp1423 > 1u?(( struct
_tunion_struct*) _temp1423)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL1444: _temp1443=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp1423)->f1; goto _LL1442; _LL1442:
_temp1441=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp1423)->f2; goto
_LL1426;} else{ goto _LL1427;} _LL1427: if(( unsigned int) _temp1423 > 1u?((
struct _tunion_struct*) _temp1423)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL1448:
_temp1447=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1423)->f1; goto
_LL1446; _LL1446: _temp1445=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp1423)->f2; goto _LL1428;} else{ goto _LL1429;} _LL1429: if(( unsigned int)
_temp1423 > 1u?(( struct _tunion_struct*) _temp1423)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL1452: _temp1451=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1423)->f1;
if(( int) _temp1451 == Cyc_Absyn_Signed){ goto _LL1450;} else{ goto _LL1431;}
_LL1450: _temp1449=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1423)->f2; goto
_LL1430;} else{ goto _LL1431;} _LL1431: if(( unsigned int) _temp1423 > 1u?((
struct _tunion_struct*) _temp1423)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL1456:
_temp1455=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1423)->f1; if(( int)
_temp1455 == Cyc_Absyn_Unsigned){ goto _LL1454;} else{ goto _LL1433;} _LL1454:
_temp1453=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1423)->f2; goto _LL1432;}
else{ goto _LL1433;} _LL1433: if(( unsigned int) _temp1423 > 1u?(( struct
_tunion_struct*) _temp1423)->tag == Cyc_Absyn_LongLong_c_tag: 0){ _LL1460:
_temp1459=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1423)->f1; goto
_LL1458; _LL1458: _temp1457=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1423)->f2; goto _LL1434;} else{ goto _LL1435;} _LL1435: if(( unsigned int)
_temp1423 > 1u?(( struct _tunion_struct*) _temp1423)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL1462: _temp1461=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp1423)->f1; goto _LL1436;} else{ goto _LL1437;} _LL1437: if(( int) _temp1423
== Cyc_Absyn_Null_c){ goto _LL1438;} else{ goto _LL1439;} _LL1439: if((
unsigned int) _temp1423 > 1u?(( struct _tunion_struct*) _temp1423)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1464: _temp1463=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1423)->f1; goto _LL1440;} else{ goto _LL1424;} _LL1426: return Cyc_PP_text(({
struct _tagged_string _temp1465= Cyc_Absynpp_char_escape( _temp1441); xprintf("'%.*s'",
_temp1465.last_plus_one - _temp1465.curr, _temp1465.curr);})); _LL1428: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1445)); _LL1430: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1449)); _LL1432: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1453)); _LL1434: return Cyc_PP_text(( struct _tagged_string)({
char* _temp1466=( char*)"<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp1467; _temp1467.curr= _temp1466; _temp1467.base= _temp1466; _temp1467.last_plus_one=
_temp1466 + 27; _temp1467;})); _LL1436: return Cyc_PP_text( _temp1461); _LL1438:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp1468=( char*)"null";
struct _tagged_string _temp1469; _temp1469.curr= _temp1468; _temp1469.base=
_temp1468; _temp1469.last_plus_one= _temp1468 + 5; _temp1469;})); _LL1440:
return Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1470=(
char*)"\""; struct _tagged_string _temp1471; _temp1471.curr= _temp1470;
_temp1471.base= _temp1470; _temp1471.last_plus_one= _temp1470 + 2; _temp1471;})),
Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1463)), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1472=( char*)"\""; struct _tagged_string _temp1473;
_temp1473.curr= _temp1472; _temp1473.base= _temp1472; _temp1473.last_plus_one=
_temp1472 + 2; _temp1473;}))); _LL1424:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:({ struct Cyc_List_List*
_temp1474= es; if( _temp1474 == 0){ _throw( Null_Exception);} _temp1474->tl;})
!= 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct*
_temp1475=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1475[ 0]=({ struct Cyc_Core_Failure_struct _temp1476; _temp1476.tag= Cyc_Core_Failure_tag;
_temp1476.f1=({ struct _tagged_string _temp1477= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1477.last_plus_one
- _temp1477.curr, _temp1477.curr);}); _temp1476;}); _temp1475;}));}{ struct Cyc_PP_Doc*
d= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1478= es; if( _temp1478 == 0){ _throw( Null_Exception);} _temp1478->hd;}));
return Cyc_Absynpp_cat2( d, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1479=( char*)".size"; struct _tagged_string _temp1480; _temp1480.curr=
_temp1479; _temp1480.base= _temp1479; _temp1480.last_plus_one= _temp1479 + 6;
_temp1480;})));}} else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct Cyc_List_List*
ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct*
_temp1481=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1481[ 0]=({ struct Cyc_Core_Failure_struct _temp1482; _temp1482.tag= Cyc_Core_Failure_tag;
_temp1482.f1=({ struct _tagged_string _temp1483= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with no args", _temp1483.last_plus_one -
_temp1483.curr, _temp1483.curr);}); _temp1482;}); _temp1481;}));} else{ if(({
struct Cyc_List_List* _temp1484= ds; if( _temp1484 == 0){ _throw( Null_Exception);}
_temp1484->tl;}) == 0){ return Cyc_Absynpp_cat3( ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1485=( char*)" "; struct _tagged_string _temp1486;
_temp1486.curr= _temp1485; _temp1486.base= _temp1485; _temp1486.last_plus_one=
_temp1485 + 2; _temp1486;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1487= ds; if( _temp1487 == 0){ _throw( Null_Exception);} _temp1487->hd;}));}
else{ if(({ struct Cyc_List_List* _temp1489=({ struct Cyc_List_List* _temp1488=
ds; if( _temp1488 == 0){ _throw( Null_Exception);} _temp1488->tl;}); if(
_temp1489 == 0){ _throw( Null_Exception);} _temp1489->tl;}) != 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp1490=( struct
Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1490[ 0]=({ struct Cyc_Core_Failure_struct _temp1491; _temp1491.tag= Cyc_Core_Failure_tag;
_temp1491.f1=({ struct _tagged_string _temp1492= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with more than 2 args", _temp1492.last_plus_one
- _temp1492.curr, _temp1492.curr);}); _temp1491;}); _temp1490;}));} else{ return
Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp1493= ds;
if( _temp1493 == 0){ _throw( Null_Exception);} _temp1493->hd;}), Cyc_PP_text((
struct _tagged_string)({ char* _temp1494=( char*)" "; struct _tagged_string
_temp1495; _temp1495.curr= _temp1494; _temp1495.base= _temp1494; _temp1495.last_plus_one=
_temp1494 + 2; _temp1495;})), ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1496=( char*)" "; struct _tagged_string _temp1497; _temp1497.curr=
_temp1496; _temp1497.base= _temp1496; _temp1497.last_plus_one= _temp1496 + 2;
_temp1497;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp1499=({ struct
Cyc_List_List* _temp1498= ds; if( _temp1498 == 0){ _throw( Null_Exception);}
_temp1498->tl;}); if( _temp1499 == 0){ _throw( Null_Exception);} _temp1499->hd;}));}}}}
else{ return Cyc_Absynpp_cat4( ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1500=( char*)"("; struct _tagged_string _temp1501; _temp1501.curr=
_temp1500; _temp1501.base= _temp1500; _temp1501.last_plus_one= _temp1500 + 2;
_temp1501;})), Cyc_Absynpp_exps2doc_prec( 20, es), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1502=( char*)")"; struct _tagged_string _temp1503;
_temp1503.curr= _temp1502; _temp1503.base= _temp1502; _temp1503.last_plus_one=
_temp1502 + 2; _temp1503;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1504= p; _LL1506: if(( int) _temp1504 == Cyc_Absyn_Plus){
goto _LL1507;} else{ goto _LL1508;} _LL1508: if(( int) _temp1504 == Cyc_Absyn_Times){
goto _LL1509;} else{ goto _LL1510;} _LL1510: if(( int) _temp1504 == Cyc_Absyn_Minus){
goto _LL1511;} else{ goto _LL1512;} _LL1512: if(( int) _temp1504 == Cyc_Absyn_Div){
goto _LL1513;} else{ goto _LL1514;} _LL1514: if(( int) _temp1504 == Cyc_Absyn_Mod){
goto _LL1515;} else{ goto _LL1516;} _LL1516: if(( int) _temp1504 == Cyc_Absyn_Eq){
goto _LL1517;} else{ goto _LL1518;} _LL1518: if(( int) _temp1504 == Cyc_Absyn_Neq){
goto _LL1519;} else{ goto _LL1520;} _LL1520: if(( int) _temp1504 == Cyc_Absyn_Gt){
goto _LL1521;} else{ goto _LL1522;} _LL1522: if(( int) _temp1504 == Cyc_Absyn_Lt){
goto _LL1523;} else{ goto _LL1524;} _LL1524: if(( int) _temp1504 == Cyc_Absyn_Gte){
goto _LL1525;} else{ goto _LL1526;} _LL1526: if(( int) _temp1504 == Cyc_Absyn_Lte){
goto _LL1527;} else{ goto _LL1528;} _LL1528: if(( int) _temp1504 == Cyc_Absyn_Not){
goto _LL1529;} else{ goto _LL1530;} _LL1530: if(( int) _temp1504 == Cyc_Absyn_Bitnot){
goto _LL1531;} else{ goto _LL1532;} _LL1532: if(( int) _temp1504 == Cyc_Absyn_Bitand){
goto _LL1533;} else{ goto _LL1534;} _LL1534: if(( int) _temp1504 == Cyc_Absyn_Bitor){
goto _LL1535;} else{ goto _LL1536;} _LL1536: if(( int) _temp1504 == Cyc_Absyn_Bitxor){
goto _LL1537;} else{ goto _LL1538;} _LL1538: if(( int) _temp1504 == Cyc_Absyn_Bitlshift){
goto _LL1539;} else{ goto _LL1540;} _LL1540: if(( int) _temp1504 == Cyc_Absyn_Bitlrshift){
goto _LL1541;} else{ goto _LL1542;} _LL1542: if(( int) _temp1504 == Cyc_Absyn_Bitarshift){
goto _LL1543;} else{ goto _LL1544;} _LL1544: if(( int) _temp1504 == Cyc_Absyn_Size){
goto _LL1545;} else{ goto _LL1546;} _LL1546: if(( int) _temp1504 == Cyc_Absyn_Printf){
goto _LL1547;} else{ goto _LL1548;} _LL1548: if(( int) _temp1504 == Cyc_Absyn_Fprintf){
goto _LL1549;} else{ goto _LL1550;} _LL1550: if(( int) _temp1504 == Cyc_Absyn_Xprintf){
goto _LL1551;} else{ goto _LL1552;} _LL1552: if(( int) _temp1504 == Cyc_Absyn_Scanf){
goto _LL1553;} else{ goto _LL1554;} _LL1554: if(( int) _temp1504 == Cyc_Absyn_Fscanf){
goto _LL1555;} else{ goto _LL1556;} _LL1556: if(( int) _temp1504 == Cyc_Absyn_Sscanf){
goto _LL1557;} else{ goto _LL1505;} _LL1507: return( struct _tagged_string)({
char* _temp1558=( char*)"+"; struct _tagged_string _temp1559; _temp1559.curr=
_temp1558; _temp1559.base= _temp1558; _temp1559.last_plus_one= _temp1558 + 2;
_temp1559;}); _LL1509: return( struct _tagged_string)({ char* _temp1560=( char*)"*";
struct _tagged_string _temp1561; _temp1561.curr= _temp1560; _temp1561.base=
_temp1560; _temp1561.last_plus_one= _temp1560 + 2; _temp1561;}); _LL1511: return(
struct _tagged_string)({ char* _temp1562=( char*)"-"; struct _tagged_string
_temp1563; _temp1563.curr= _temp1562; _temp1563.base= _temp1562; _temp1563.last_plus_one=
_temp1562 + 2; _temp1563;}); _LL1513: return( struct _tagged_string)({ char*
_temp1564=( char*)"/"; struct _tagged_string _temp1565; _temp1565.curr=
_temp1564; _temp1565.base= _temp1564; _temp1565.last_plus_one= _temp1564 + 2;
_temp1565;}); _LL1515: return( struct _tagged_string)({ char* _temp1566=( char*)"%";
struct _tagged_string _temp1567; _temp1567.curr= _temp1566; _temp1567.base=
_temp1566; _temp1567.last_plus_one= _temp1566 + 2; _temp1567;}); _LL1517: return(
struct _tagged_string)({ char* _temp1568=( char*)"=="; struct _tagged_string
_temp1569; _temp1569.curr= _temp1568; _temp1569.base= _temp1568; _temp1569.last_plus_one=
_temp1568 + 3; _temp1569;}); _LL1519: return( struct _tagged_string)({ char*
_temp1570=( char*)"!="; struct _tagged_string _temp1571; _temp1571.curr=
_temp1570; _temp1571.base= _temp1570; _temp1571.last_plus_one= _temp1570 + 3;
_temp1571;}); _LL1521: return( struct _tagged_string)({ char* _temp1572=( char*)">";
struct _tagged_string _temp1573; _temp1573.curr= _temp1572; _temp1573.base=
_temp1572; _temp1573.last_plus_one= _temp1572 + 2; _temp1573;}); _LL1523: return(
struct _tagged_string)({ char* _temp1574=( char*)"<"; struct _tagged_string
_temp1575; _temp1575.curr= _temp1574; _temp1575.base= _temp1574; _temp1575.last_plus_one=
_temp1574 + 2; _temp1575;}); _LL1525: return( struct _tagged_string)({ char*
_temp1576=( char*)">="; struct _tagged_string _temp1577; _temp1577.curr=
_temp1576; _temp1577.base= _temp1576; _temp1577.last_plus_one= _temp1576 + 3;
_temp1577;}); _LL1527: return( struct _tagged_string)({ char* _temp1578=( char*)"<=";
struct _tagged_string _temp1579; _temp1579.curr= _temp1578; _temp1579.base=
_temp1578; _temp1579.last_plus_one= _temp1578 + 3; _temp1579;}); _LL1529: return(
struct _tagged_string)({ char* _temp1580=( char*)"!"; struct _tagged_string
_temp1581; _temp1581.curr= _temp1580; _temp1581.base= _temp1580; _temp1581.last_plus_one=
_temp1580 + 2; _temp1581;}); _LL1531: return( struct _tagged_string)({ char*
_temp1582=( char*)"~"; struct _tagged_string _temp1583; _temp1583.curr=
_temp1582; _temp1583.base= _temp1582; _temp1583.last_plus_one= _temp1582 + 2;
_temp1583;}); _LL1533: return( struct _tagged_string)({ char* _temp1584=( char*)"&";
struct _tagged_string _temp1585; _temp1585.curr= _temp1584; _temp1585.base=
_temp1584; _temp1585.last_plus_one= _temp1584 + 2; _temp1585;}); _LL1535: return(
struct _tagged_string)({ char* _temp1586=( char*)"|"; struct _tagged_string
_temp1587; _temp1587.curr= _temp1586; _temp1587.base= _temp1586; _temp1587.last_plus_one=
_temp1586 + 2; _temp1587;}); _LL1537: return( struct _tagged_string)({ char*
_temp1588=( char*)"^"; struct _tagged_string _temp1589; _temp1589.curr=
_temp1588; _temp1589.base= _temp1588; _temp1589.last_plus_one= _temp1588 + 2;
_temp1589;}); _LL1539: return( struct _tagged_string)({ char* _temp1590=( char*)"<<";
struct _tagged_string _temp1591; _temp1591.curr= _temp1590; _temp1591.base=
_temp1590; _temp1591.last_plus_one= _temp1590 + 3; _temp1591;}); _LL1541: return(
struct _tagged_string)({ char* _temp1592=( char*)">>"; struct _tagged_string
_temp1593; _temp1593.curr= _temp1592; _temp1593.base= _temp1592; _temp1593.last_plus_one=
_temp1592 + 3; _temp1593;}); _LL1543: return( struct _tagged_string)({ char*
_temp1594=( char*)">>>"; struct _tagged_string _temp1595; _temp1595.curr=
_temp1594; _temp1595.base= _temp1594; _temp1595.last_plus_one= _temp1594 + 4;
_temp1595;}); _LL1545: return( struct _tagged_string)({ char* _temp1596=( char*)"size";
struct _tagged_string _temp1597; _temp1597.curr= _temp1596; _temp1597.base=
_temp1596; _temp1597.last_plus_one= _temp1596 + 5; _temp1597;}); _LL1547: return(
struct _tagged_string)({ char* _temp1598=( char*)"printf"; struct _tagged_string
_temp1599; _temp1599.curr= _temp1598; _temp1599.base= _temp1598; _temp1599.last_plus_one=
_temp1598 + 7; _temp1599;}); _LL1549: return( struct _tagged_string)({ char*
_temp1600=( char*)"fprintf"; struct _tagged_string _temp1601; _temp1601.curr=
_temp1600; _temp1601.base= _temp1600; _temp1601.last_plus_one= _temp1600 + 8;
_temp1601;}); _LL1551: return( struct _tagged_string)({ char* _temp1602=( char*)"xprintf";
struct _tagged_string _temp1603; _temp1603.curr= _temp1602; _temp1603.base=
_temp1602; _temp1603.last_plus_one= _temp1602 + 8; _temp1603;}); _LL1553: return(
struct _tagged_string)({ char* _temp1604=( char*)"scanf"; struct _tagged_string
_temp1605; _temp1605.curr= _temp1604; _temp1605.base= _temp1604; _temp1605.last_plus_one=
_temp1604 + 6; _temp1605;}); _LL1555: return( struct _tagged_string)({ char*
_temp1606=( char*)"fscanf"; struct _tagged_string _temp1607; _temp1607.curr=
_temp1606; _temp1607.base= _temp1606; _temp1607.last_plus_one= _temp1606 + 7;
_temp1607;}); _LL1557: return( struct _tagged_string)({ char* _temp1608=( char*)"sscanf";
struct _tagged_string _temp1609; _temp1609.curr= _temp1608; _temp1609.base=
_temp1608; _temp1609.last_plus_one= _temp1608 + 7; _temp1609;}); _LL1505:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1
== 0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp1610=(* t).f1; if( _temp1610
== 0){ _throw( Null_Exception);} _temp1610->v;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1611=( char*)" = "; struct _tagged_string _temp1612;
_temp1612.curr= _temp1611; _temp1612.base= _temp1611; _temp1612.last_plus_one=
_temp1611 + 4; _temp1612;})), Cyc_Absynpp_exp2doc((* t).f2));}} struct Cyc_PP_Doc*
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
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1613=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1619; struct Cyc_Absyn_Decl* _temp1621;
_LL1615: if(( unsigned int) _temp1613 > 1u?(( struct _tunion_struct*) _temp1613)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL1622: _temp1621=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1613)->f1; goto _LL1620; _LL1620:
_temp1619=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1613)->f2;
goto _LL1616;} else{ goto _LL1617;} _LL1617: goto _LL1618; _LL1616: return 1;
_LL1618: return 0; _LL1614:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1623=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1663; struct Cyc_Absyn_Stmt* _temp1665; struct Cyc_Absyn_Stmt*
_temp1667; struct Cyc_Absyn_Exp* _temp1669; struct Cyc_Absyn_Stmt* _temp1671;
struct Cyc_Absyn_Stmt* _temp1673; struct Cyc_Absyn_Exp* _temp1675; struct Cyc_Absyn_Stmt*
_temp1677; struct _tuple2 _temp1679; struct Cyc_Absyn_Stmt* _temp1681; struct
Cyc_Absyn_Exp* _temp1683; struct Cyc_Absyn_Stmt* _temp1685; struct Cyc_Absyn_Stmt*
_temp1687; struct Cyc_Absyn_Stmt* _temp1689; struct _tagged_string* _temp1691;
struct Cyc_Absyn_Stmt* _temp1693; struct _tuple2 _temp1695; struct Cyc_Absyn_Stmt*
_temp1697; struct Cyc_Absyn_Exp* _temp1699; struct _tuple2 _temp1701; struct Cyc_Absyn_Stmt*
_temp1703; struct Cyc_Absyn_Exp* _temp1705; struct Cyc_Absyn_Exp* _temp1707;
struct Cyc_List_List* _temp1709; struct Cyc_Absyn_Exp* _temp1711; struct Cyc_Absyn_Switch_clause**
_temp1713; struct Cyc_List_List* _temp1715; struct Cyc_Absyn_Switch_clause**
_temp1717; struct Cyc_List_List* _temp1719; struct Cyc_Absyn_Stmt* _temp1721;
struct Cyc_Absyn_Decl* _temp1723; struct Cyc_Absyn_Stmt* _temp1725; struct Cyc_Absyn_Stmt*
_temp1727; struct Cyc_Absyn_Stmt* _temp1729; struct _tagged_string* _temp1731;
struct _tuple2 _temp1733; struct Cyc_Absyn_Stmt* _temp1735; struct Cyc_Absyn_Exp*
_temp1737; struct Cyc_Absyn_Stmt* _temp1739; struct Cyc_List_List* _temp1741;
struct Cyc_Absyn_Stmt* _temp1743; _LL1625: if(( int) _temp1623 == Cyc_Absyn_Skip_s){
goto _LL1626;} else{ goto _LL1627;} _LL1627: if(( unsigned int) _temp1623 > 1u?((
struct _tunion_struct*) _temp1623)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1664:
_temp1663=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1623)->f1;
goto _LL1628;} else{ goto _LL1629;} _LL1629: if(( unsigned int) _temp1623 > 1u?((
struct _tunion_struct*) _temp1623)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1668:
_temp1667=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1623)->f1;
goto _LL1666; _LL1666: _temp1665=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1623)->f2; goto _LL1630;} else{ goto _LL1631;} _LL1631: if(( unsigned int)
_temp1623 > 1u?(( struct _tunion_struct*) _temp1623)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1670: _temp1669=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1623)->f1; goto _LL1632;} else{ goto _LL1633;} _LL1633: if(( unsigned int)
_temp1623 > 1u?(( struct _tunion_struct*) _temp1623)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1676: _temp1675=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1623)->f1; goto _LL1674; _LL1674: _temp1673=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1623)->f2; goto _LL1672; _LL1672:
_temp1671=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1623)->f3; goto _LL1634;} else{ goto _LL1635;} _LL1635: if(( unsigned int)
_temp1623 > 1u?(( struct _tunion_struct*) _temp1623)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1680: _temp1679=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*)
_temp1623)->f1; _LL1684: _temp1683= _temp1679.f1; goto _LL1682; _LL1682:
_temp1681= _temp1679.f2; goto _LL1678; _LL1678: _temp1677=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1623)->f2; goto _LL1636;} else{ goto
_LL1637;} _LL1637: if(( unsigned int) _temp1623 > 1u?(( struct _tunion_struct*)
_temp1623)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1686: _temp1685=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1623)->f1; goto _LL1638;} else{ goto
_LL1639;} _LL1639: if(( unsigned int) _temp1623 > 1u?(( struct _tunion_struct*)
_temp1623)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1688: _temp1687=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1623)->f1; goto _LL1640;} else{ goto
_LL1641;} _LL1641: if(( unsigned int) _temp1623 > 1u?(( struct _tunion_struct*)
_temp1623)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1692: _temp1691=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1623)->f1; goto _LL1690;
_LL1690: _temp1689=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1623)->f2; goto _LL1642;} else{ goto _LL1643;} _LL1643: if(( unsigned int)
_temp1623 > 1u?(( struct _tunion_struct*) _temp1623)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1708: _temp1707=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1623)->f1; goto _LL1702; _LL1702: _temp1701=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp1623)->f2; _LL1706: _temp1705= _temp1701.f1; goto _LL1704; _LL1704:
_temp1703= _temp1701.f2; goto _LL1696; _LL1696: _temp1695=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp1623)->f3; _LL1700: _temp1699= _temp1695.f1;
goto _LL1698; _LL1698: _temp1697= _temp1695.f2; goto _LL1694; _LL1694: _temp1693=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1623)->f4; goto
_LL1644;} else{ goto _LL1645;} _LL1645: if(( unsigned int) _temp1623 > 1u?((
struct _tunion_struct*) _temp1623)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1712:
_temp1711=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1623)->f1; goto _LL1710; _LL1710: _temp1709=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1623)->f2; goto _LL1646;} else{ goto
_LL1647;} _LL1647: if(( unsigned int) _temp1623 > 1u?(( struct _tunion_struct*)
_temp1623)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1716: _temp1715=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1623)->f1; if( _temp1715 == 0){ goto
_LL1714;} else{ goto _LL1649;} _LL1714: _temp1713=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1623)->f2; goto _LL1648;} else{ goto
_LL1649;} _LL1649: if(( unsigned int) _temp1623 > 1u?(( struct _tunion_struct*)
_temp1623)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1720: _temp1719=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1623)->f1; goto _LL1718; _LL1718:
_temp1717=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1623)->f2; goto _LL1650;} else{ goto _LL1651;} _LL1651: if(( unsigned int)
_temp1623 > 1u?(( struct _tunion_struct*) _temp1623)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1724: _temp1723=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1623)->f1; goto _LL1722; _LL1722: _temp1721=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1623)->f2; goto _LL1652;} else{ goto
_LL1653;} _LL1653: if(( unsigned int) _temp1623 > 1u?(( struct _tunion_struct*)
_temp1623)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1726: _temp1725=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1623)->f1; goto _LL1654;} else{ goto
_LL1655;} _LL1655: if(( unsigned int) _temp1623 > 1u?(( struct _tunion_struct*)
_temp1623)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1728: _temp1727=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1623)->f1; goto _LL1656;} else{ goto
_LL1657;} _LL1657: if(( unsigned int) _temp1623 > 1u?(( struct _tunion_struct*)
_temp1623)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1732: _temp1731=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1623)->f1; goto
_LL1730; _LL1730: _temp1729=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1623)->f2; goto _LL1658;} else{ goto _LL1659;} _LL1659: if(( unsigned int)
_temp1623 > 1u?(( struct _tunion_struct*) _temp1623)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1740: _temp1739=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1623)->f1; goto _LL1734; _LL1734: _temp1733=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp1623)->f2; _LL1738: _temp1737= _temp1733.f1; goto _LL1736; _LL1736:
_temp1735= _temp1733.f2; goto _LL1660;} else{ goto _LL1661;} _LL1661: if((
unsigned int) _temp1623 > 1u?(( struct _tunion_struct*) _temp1623)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1744: _temp1743=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1623)->f1; goto _LL1742; _LL1742: _temp1741=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1623)->f2; goto _LL1662;} else{ goto
_LL1624;} _LL1626: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1745=(
char*)";"; struct _tagged_string _temp1746; _temp1746.curr= _temp1745; _temp1746.base=
_temp1745; _temp1746.last_plus_one= _temp1745 + 2; _temp1746;})); goto _LL1624;
_LL1628: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc( _temp1663), Cyc_PP_text((
struct _tagged_string)({ char* _temp1747=( char*)";"; struct _tagged_string
_temp1748; _temp1748.curr= _temp1747; _temp1748.base= _temp1747; _temp1748.last_plus_one=
_temp1747 + 2; _temp1748;}))); goto _LL1624; _LL1630: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1667)){ s= Cyc_Absynpp_cat6( Cyc_PP_text((
struct _tagged_string)({ char* _temp1749=( char*)"{ "; struct _tagged_string
_temp1750; _temp1750.curr= _temp1749; _temp1750.base= _temp1749; _temp1750.last_plus_one=
_temp1749 + 3; _temp1750;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1667)),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1751=( char*)"}";
struct _tagged_string _temp1752; _temp1752.curr= _temp1751; _temp1752.base=
_temp1751; _temp1752.last_plus_one= _temp1751 + 2; _temp1752;})), Cyc_PP_line_doc(),
Cyc_Absynpp_is_declaration( _temp1665)? Cyc_Absynpp_cat4( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1753=( char*)"{ "; struct _tagged_string _temp1754;
_temp1754.curr= _temp1753; _temp1754.base= _temp1753; _temp1754.last_plus_one=
_temp1753 + 3; _temp1754;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1665)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1755=( char*)"}"; struct
_tagged_string _temp1756; _temp1756.curr= _temp1755; _temp1756.base= _temp1755;
_temp1756.last_plus_one= _temp1755 + 2; _temp1756;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1665));} else{ if( Cyc_Absynpp_is_declaration( _temp1665)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1667), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1757=( char*)"{ "; struct _tagged_string _temp1758;
_temp1758.curr= _temp1757; _temp1758.base= _temp1757; _temp1758.last_plus_one=
_temp1757 + 3; _temp1758;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1665)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1759=( char*)"}"; struct
_tagged_string _temp1760; _temp1760.curr= _temp1759; _temp1760.base= _temp1759;
_temp1760.last_plus_one= _temp1759 + 2; _temp1760;})), Cyc_PP_line_doc());}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,(
struct _tagged_string)({ char* _temp1761=( char*)""; struct _tagged_string
_temp1762; _temp1762.curr= _temp1761; _temp1762.base= _temp1761; _temp1762.last_plus_one=
_temp1761 + 1; _temp1762;}),({ struct Cyc_List_List* _temp1763=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1763->hd=( void*) _temp1667;
_temp1763->tl=({ struct Cyc_List_List* _temp1764=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1764->hd=( void*) _temp1665;
_temp1764->tl= 0; _temp1764;}); _temp1763;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,( struct
_tagged_string)({ char* _temp1765=( char*)""; struct _tagged_string _temp1766;
_temp1766.curr= _temp1765; _temp1766.base= _temp1765; _temp1766.last_plus_one=
_temp1765 + 1; _temp1766;}),({ struct Cyc_List_List* _temp1767=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1767->hd=( void*) _temp1667;
_temp1767->tl=({ struct Cyc_List_List* _temp1768=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1768->hd=( void*) _temp1665;
_temp1768->tl= 0; _temp1768;}); _temp1767;}));} goto _LL1624; _LL1632: if(
_temp1669 == 0){ s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1769=(
char*)"return;"; struct _tagged_string _temp1770; _temp1770.curr= _temp1769;
_temp1770.base= _temp1769; _temp1770.last_plus_one= _temp1769 + 8; _temp1770;}));}
else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1771=( char*)"return "; struct _tagged_string _temp1772; _temp1772.curr=
_temp1771; _temp1772.base= _temp1771; _temp1772.last_plus_one= _temp1771 + 8;
_temp1772;})), _temp1669 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1773= _temp1669; if( _temp1773 == 0){
_throw( Null_Exception);} _temp1773;})), Cyc_PP_text(( struct _tagged_string)({
char* _temp1774=( char*)";"; struct _tagged_string _temp1775; _temp1775.curr=
_temp1774; _temp1775.base= _temp1774; _temp1775.last_plus_one= _temp1774 + 2;
_temp1775;})));} goto _LL1624; _LL1634: { int print_else;{ void* _temp1776=(
void*) _temp1671->r; _LL1778: if(( int) _temp1776 == Cyc_Absyn_Skip_s){ goto
_LL1779;} else{ goto _LL1780;} _LL1780: goto _LL1781; _LL1779: print_else= 0;
goto _LL1777; _LL1781: print_else= 1; goto _LL1777; _LL1777:;} s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1782=( char*)"if ("; struct
_tagged_string _temp1783; _temp1783.curr= _temp1782; _temp1783.base= _temp1782;
_temp1783.last_plus_one= _temp1782 + 5; _temp1783;})), Cyc_Absynpp_exp2doc(
_temp1675), Cyc_PP_text(( struct _tagged_string)({ char* _temp1784=( char*)") {";
struct _tagged_string _temp1785; _temp1785.curr= _temp1784; _temp1785.base=
_temp1784; _temp1785.last_plus_one= _temp1784 + 4; _temp1785;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1673))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1786=( char*)"}"; struct
_tagged_string _temp1787; _temp1787.curr= _temp1786; _temp1787.base= _temp1786;
_temp1787.last_plus_one= _temp1786 + 2; _temp1787;})), print_else? Cyc_Absynpp_cat5(
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1788=( char*)"else {";
struct _tagged_string _temp1789; _temp1789.curr= _temp1788; _temp1789.base=
_temp1788; _temp1789.last_plus_one= _temp1788 + 7; _temp1789;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1671))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1790=( char*)"}"; struct
_tagged_string _temp1791; _temp1791.curr= _temp1790; _temp1791.base= _temp1790;
_temp1791.last_plus_one= _temp1790 + 2; _temp1791;}))): Cyc_PP_nil_doc()); goto
_LL1624;} _LL1636: s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({
char* _temp1792=( char*)"while ("; struct _tagged_string _temp1793; _temp1793.curr=
_temp1792; _temp1793.base= _temp1792; _temp1793.last_plus_one= _temp1792 + 8;
_temp1793;})), Cyc_Absynpp_exp2doc( _temp1683), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1794=( char*)") {"; struct _tagged_string _temp1795;
_temp1795.curr= _temp1794; _temp1795.base= _temp1794; _temp1795.last_plus_one=
_temp1794 + 4; _temp1795;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1677))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1796=( char*)"}"; struct _tagged_string _temp1797;
_temp1797.curr= _temp1796; _temp1797.base= _temp1796; _temp1797.last_plus_one=
_temp1796 + 2; _temp1797;}))); goto _LL1624; _LL1638: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1798=( char*)"break;"; struct _tagged_string
_temp1799; _temp1799.curr= _temp1798; _temp1799.base= _temp1798; _temp1799.last_plus_one=
_temp1798 + 7; _temp1799;})); goto _LL1624; _LL1640: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1800=( char*)"continue;"; struct _tagged_string
_temp1801; _temp1801.curr= _temp1800; _temp1801.base= _temp1800; _temp1801.last_plus_one=
_temp1800 + 10; _temp1801;})); goto _LL1624; _LL1642: s= Cyc_PP_text(({ struct
_tagged_string _temp1802=* _temp1691; xprintf("goto %.*s;", _temp1802.last_plus_one
- _temp1802.curr, _temp1802.curr);})); goto _LL1624; _LL1644: s= Cyc_Absynpp_cat10(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1803=( char*)"for("; struct
_tagged_string _temp1804; _temp1804.curr= _temp1803; _temp1804.base= _temp1803;
_temp1804.last_plus_one= _temp1803 + 5; _temp1804;})), Cyc_Absynpp_exp2doc(
_temp1707), Cyc_PP_text(( struct _tagged_string)({ char* _temp1805=( char*)"; ";
struct _tagged_string _temp1806; _temp1806.curr= _temp1805; _temp1806.base=
_temp1805; _temp1806.last_plus_one= _temp1805 + 3; _temp1806;})), Cyc_Absynpp_exp2doc(
_temp1705), Cyc_PP_text(( struct _tagged_string)({ char* _temp1807=( char*)"; ";
struct _tagged_string _temp1808; _temp1808.curr= _temp1807; _temp1808.base=
_temp1807; _temp1808.last_plus_one= _temp1807 + 3; _temp1808;})), Cyc_Absynpp_exp2doc(
_temp1699), Cyc_PP_text(( struct _tagged_string)({ char* _temp1809=( char*)") {";
struct _tagged_string _temp1810; _temp1810.curr= _temp1809; _temp1810.base=
_temp1809; _temp1810.last_plus_one= _temp1809 + 4; _temp1810;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1693))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1811=( char*)"}"; struct
_tagged_string _temp1812; _temp1812.curr= _temp1811; _temp1812.base= _temp1811;
_temp1812.last_plus_one= _temp1811 + 2; _temp1812;}))); goto _LL1624; _LL1646: s=
Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char* _temp1813=( char*)"switch (";
struct _tagged_string _temp1814; _temp1814.curr= _temp1813; _temp1814.base=
_temp1813; _temp1814.last_plus_one= _temp1813 + 9; _temp1814;})), Cyc_Absynpp_exp2doc(
_temp1711), Cyc_PP_text(( struct _tagged_string)({ char* _temp1815=( char*)") {";
struct _tagged_string _temp1816; _temp1816.curr= _temp1815; _temp1816.base=
_temp1815; _temp1816.last_plus_one= _temp1815 + 4; _temp1816;})), Cyc_PP_line_doc(),
Cyc_Absynpp_switchclauses2doc( _temp1709), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1817=( char*)"}"; struct _tagged_string
_temp1818; _temp1818.curr= _temp1817; _temp1818.base= _temp1817; _temp1818.last_plus_one=
_temp1817 + 2; _temp1818;}))); goto _LL1624; _LL1648: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1819=( char*)"fallthru;"; struct _tagged_string
_temp1820; _temp1820.curr= _temp1819; _temp1820.base= _temp1819; _temp1820.last_plus_one=
_temp1819 + 10; _temp1820;})); goto _LL1624; _LL1650: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1821=( char*)"fallthru("; struct
_tagged_string _temp1822; _temp1822.curr= _temp1821; _temp1822.base= _temp1821;
_temp1822.last_plus_one= _temp1821 + 10; _temp1822;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1719), Cyc_PP_text(( struct _tagged_string)({ char* _temp1823=( char*)");";
struct _tagged_string _temp1824; _temp1824.curr= _temp1823; _temp1824.base=
_temp1823; _temp1824.last_plus_one= _temp1823 + 3; _temp1824;}))); goto _LL1624;
_LL1652: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1723), Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1721)); goto _LL1624; _LL1654: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1825=( char*)"cut "; struct
_tagged_string _temp1826; _temp1826.curr= _temp1825; _temp1826.base= _temp1825;
_temp1826.last_plus_one= _temp1825 + 5; _temp1826;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1725))); goto _LL1624; _LL1656: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1827=( char*)"splice "; struct _tagged_string
_temp1828; _temp1828.curr= _temp1827; _temp1828.base= _temp1827; _temp1828.last_plus_one=
_temp1827 + 8; _temp1828;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1727)));
goto _LL1624; _LL1658: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1729): 0){ s= Cyc_Absynpp_cat6( Cyc_Absynpp_textptr( _temp1731), Cyc_PP_text((
struct _tagged_string)({ char* _temp1829=( char*)": {"; struct _tagged_string
_temp1830; _temp1830.curr= _temp1829; _temp1830.base= _temp1829; _temp1830.last_plus_one=
_temp1829 + 4; _temp1830;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1729)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1831=( char*)"}"; struct _tagged_string _temp1832; _temp1832.curr=
_temp1831; _temp1832.base= _temp1831; _temp1832.last_plus_one= _temp1831 + 2;
_temp1832;})));} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( _temp1731), Cyc_PP_text((
struct _tagged_string)({ char* _temp1833=( char*)": "; struct _tagged_string
_temp1834; _temp1834.curr= _temp1833; _temp1834.base= _temp1833; _temp1834.last_plus_one=
_temp1833 + 3; _temp1834;})), Cyc_Absynpp_stmt2doc( _temp1729));} goto _LL1624;
_LL1660: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1835=( char*)"do {"; struct _tagged_string _temp1836; _temp1836.curr=
_temp1835; _temp1836.base= _temp1835; _temp1836.last_plus_one= _temp1835 + 5;
_temp1836;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1739)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1837=( char*)"} while ("; struct _tagged_string _temp1838; _temp1838.curr=
_temp1837; _temp1838.base= _temp1837; _temp1838.last_plus_one= _temp1837 + 10;
_temp1838;})), Cyc_Absynpp_exp2doc( _temp1737), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1839=( char*)");"; struct _tagged_string _temp1840;
_temp1840.curr= _temp1839; _temp1840.base= _temp1839; _temp1840.last_plus_one=
_temp1839 + 3; _temp1840;}))); goto _LL1624; _LL1662: s= Cyc_Absynpp_cat9( Cyc_PP_text((
struct _tagged_string)({ char* _temp1841=( char*)"try {"; struct _tagged_string
_temp1842; _temp1842.curr= _temp1841; _temp1842.base= _temp1841; _temp1842.last_plus_one=
_temp1841 + 6; _temp1842;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1743)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1843=( char*)"} catch {"; struct _tagged_string _temp1844; _temp1844.curr=
_temp1843; _temp1844.base= _temp1843; _temp1844.last_plus_one= _temp1843 + 10;
_temp1844;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1741)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1845=( char*)"}"; struct _tagged_string _temp1846; _temp1846.curr=
_temp1845; _temp1846.base= _temp1845; _temp1846.last_plus_one= _temp1845 + 2;
_temp1846;}))); goto _LL1624; _LL1624:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1847=( void*) p->r;
int _temp1881; void* _temp1883; char _temp1885; struct _tagged_string _temp1887;
struct Cyc_Absyn_Vardecl* _temp1889; struct Cyc_List_List* _temp1891; struct Cyc_Absyn_Pat*
_temp1893; struct Cyc_Absyn_Vardecl* _temp1895; struct _tuple0* _temp1897;
struct Cyc_List_List* _temp1899; struct Cyc_List_List* _temp1901; struct _tuple0*
_temp1903; struct Cyc_List_List* _temp1905; struct Cyc_List_List* _temp1907;
struct _tuple0* _temp1909; struct Cyc_List_List* _temp1911; struct Cyc_List_List*
_temp1913; struct Cyc_Core_Opt* _temp1915; struct Cyc_Absyn_Structdecl*
_temp1917; struct Cyc_Absyn_Enumfield* _temp1919; struct Cyc_Absyn_Enumdecl*
_temp1921; struct _tuple0* _temp1923; struct Cyc_Absyn_Tunionfield* _temp1925;
struct Cyc_Absyn_Tuniondecl* _temp1927; struct Cyc_List_List* _temp1929; struct
Cyc_List_List* _temp1931; struct Cyc_Core_Opt* _temp1933; struct _tuple0*
_temp1935; struct Cyc_Absyn_Tunionfield* _temp1937; struct Cyc_Absyn_XTuniondecl*
_temp1939; struct Cyc_List_List* _temp1941; struct Cyc_List_List* _temp1943;
struct _tuple0* _temp1945; _LL1849: if(( int) _temp1847 == Cyc_Absyn_Wild_p){
goto _LL1850;} else{ goto _LL1851;} _LL1851: if(( int) _temp1847 == Cyc_Absyn_Null_p){
goto _LL1852;} else{ goto _LL1853;} _LL1853: if(( unsigned int) _temp1847 > 2u?((
struct _tunion_struct*) _temp1847)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1884:
_temp1883=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1847)->f1; goto
_LL1882; _LL1882: _temp1881=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp1847)->f2;
goto _LL1854;} else{ goto _LL1855;} _LL1855: if(( unsigned int) _temp1847 > 2u?((
struct _tunion_struct*) _temp1847)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL1886:
_temp1885=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1847)->f1; goto
_LL1856;} else{ goto _LL1857;} _LL1857: if(( unsigned int) _temp1847 > 2u?((
struct _tunion_struct*) _temp1847)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1888:
_temp1887=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1847)->f1;
goto _LL1858;} else{ goto _LL1859;} _LL1859: if(( unsigned int) _temp1847 > 2u?((
struct _tunion_struct*) _temp1847)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1890:
_temp1889=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1847)->f1; goto _LL1860;} else{ goto _LL1861;} _LL1861: if(( unsigned int)
_temp1847 > 2u?(( struct _tunion_struct*) _temp1847)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1892: _temp1891=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1847)->f1; goto _LL1862;} else{ goto _LL1863;} _LL1863: if(( unsigned int)
_temp1847 > 2u?(( struct _tunion_struct*) _temp1847)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1894: _temp1893=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1847)->f1; goto _LL1864;} else{ goto _LL1865;} _LL1865: if(( unsigned int)
_temp1847 > 2u?(( struct _tunion_struct*) _temp1847)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1896: _temp1895=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1847)->f1; goto _LL1866;} else{ goto _LL1867;} _LL1867: if(( unsigned int)
_temp1847 > 2u?(( struct _tunion_struct*) _temp1847)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL1898: _temp1897=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1847)->f1; goto _LL1868;} else{ goto _LL1869;} _LL1869: if(( unsigned int)
_temp1847 > 2u?(( struct _tunion_struct*) _temp1847)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1904: _temp1903=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1847)->f1; goto _LL1902; _LL1902: _temp1901=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1847)->f2; goto _LL1900; _LL1900:
_temp1899=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1847)->f3; goto _LL1870;} else{ goto _LL1871;} _LL1871: if(( unsigned int)
_temp1847 > 2u?(( struct _tunion_struct*) _temp1847)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1910: _temp1909=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1847)->f1; goto _LL1908; _LL1908: _temp1907=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1847)->f2; goto _LL1906; _LL1906:
_temp1905=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1847)->f3; goto _LL1872;} else{ goto _LL1873;} _LL1873: if(( unsigned int)
_temp1847 > 2u?(( struct _tunion_struct*) _temp1847)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1918: _temp1917=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1847)->f1; goto _LL1916; _LL1916: _temp1915=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1847)->f2; goto _LL1914; _LL1914:
_temp1913=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1847)->f3; goto _LL1912; _LL1912: _temp1911=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1847)->f4; goto _LL1874;} else{ goto
_LL1875;} _LL1875: if(( unsigned int) _temp1847 > 2u?(( struct _tunion_struct*)
_temp1847)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1924: _temp1923=( struct _tuple0*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1847)->f1; goto _LL1922; _LL1922:
_temp1921=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1847)->f2; goto _LL1920; _LL1920: _temp1919=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1847)->f3; goto _LL1876;} else{ goto
_LL1877;} _LL1877: if(( unsigned int) _temp1847 > 2u?(( struct _tunion_struct*)
_temp1847)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1936: _temp1935=( struct
_tuple0*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1847)->f1; goto _LL1934;
_LL1934: _temp1933=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1847)->f2; goto _LL1932; _LL1932: _temp1931=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1847)->f3; goto _LL1930; _LL1930:
_temp1929=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1847)->f4; goto _LL1928; _LL1928: _temp1927=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1847)->f5; goto _LL1926; _LL1926:
_temp1925=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1847)->f6; goto _LL1878;} else{ goto _LL1879;} _LL1879: if(( unsigned int)
_temp1847 > 2u?(( struct _tunion_struct*) _temp1847)->tag == Cyc_Absyn_XTunion_p_tag:
0){ _LL1946: _temp1945=( struct _tuple0*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1847)->f1; goto _LL1944; _LL1944: _temp1943=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1847)->f2; goto _LL1942; _LL1942:
_temp1941=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1847)->f3; goto _LL1940; _LL1940: _temp1939=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1847)->f4; goto _LL1938; _LL1938:
_temp1937=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1847)->f5; goto _LL1880;} else{ goto _LL1848;} _LL1850: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp1947=( char*)"_"; struct _tagged_string
_temp1948; _temp1948.curr= _temp1947; _temp1948.base= _temp1947; _temp1948.last_plus_one=
_temp1947 + 2; _temp1948;})); goto _LL1848; _LL1852: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1949=( char*)"null"; struct _tagged_string
_temp1950; _temp1950.curr= _temp1949; _temp1950.base= _temp1949; _temp1950.last_plus_one=
_temp1949 + 5; _temp1950;})); goto _LL1848; _LL1854: if( _temp1883 ==( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d", _temp1881));} else{ s= Cyc_PP_text(
xprintf("%u",( unsigned int) _temp1881));} goto _LL1848; _LL1856: s= Cyc_PP_text(({
struct _tagged_string _temp1951= Cyc_Absynpp_char_escape( _temp1885); xprintf("'%.*s'",
_temp1951.last_plus_one - _temp1951.curr, _temp1951.curr);})); goto _LL1848;
_LL1858: s= Cyc_PP_text( _temp1887); goto _LL1848; _LL1860: s= Cyc_Absynpp_qvar2doc(
_temp1889->name); goto _LL1848; _LL1862: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1952=( char*)"$("; struct _tagged_string
_temp1953; _temp1953.curr= _temp1952; _temp1953.base= _temp1952; _temp1953.last_plus_one=
_temp1952 + 3; _temp1953;})),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Pat*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,( struct _tagged_string)({ char* _temp1954=(
char*)","; struct _tagged_string _temp1955; _temp1955.curr= _temp1954; _temp1955.base=
_temp1954; _temp1955.last_plus_one= _temp1954 + 2; _temp1955;}), _temp1891), Cyc_PP_text((
struct _tagged_string)({ char* _temp1956=( char*)")"; struct _tagged_string
_temp1957; _temp1957.curr= _temp1956; _temp1957.base= _temp1956; _temp1957.last_plus_one=
_temp1956 + 2; _temp1957;}))); goto _LL1848; _LL1864: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1958=( char*)"&"; struct _tagged_string
_temp1959; _temp1959.curr= _temp1958; _temp1959.base= _temp1958; _temp1959.last_plus_one=
_temp1958 + 2; _temp1959;})), Cyc_Absynpp_pat2doc( _temp1893)); goto _LL1848;
_LL1866: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1960=( char*)"*"; struct _tagged_string _temp1961; _temp1961.curr=
_temp1960; _temp1961.base= _temp1960; _temp1961.last_plus_one= _temp1960 + 2;
_temp1961;})), Cyc_Absynpp_qvar2doc( _temp1895->name)); goto _LL1848; _LL1868: s=
Cyc_Absynpp_qvar2doc( _temp1897); goto _LL1848; _LL1870: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1903), Cyc_Absynpp_tvars2doc( _temp1901), Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1962=( char*)"("; struct _tagged_string
_temp1963; _temp1963.curr= _temp1962; _temp1963.base= _temp1962; _temp1963.last_plus_one=
_temp1962 + 2; _temp1963;}),( struct _tagged_string)({ char* _temp1964=( char*)")";
struct _tagged_string _temp1965; _temp1965.curr= _temp1964; _temp1965.base=
_temp1964; _temp1965.last_plus_one= _temp1964 + 2; _temp1965;}),( struct
_tagged_string)({ char* _temp1966=( char*)","; struct _tagged_string _temp1967;
_temp1967.curr= _temp1966; _temp1967.base= _temp1966; _temp1967.last_plus_one=
_temp1966 + 2; _temp1967;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1899))); goto _LL1848; _LL1872: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1909), Cyc_Absynpp_tvars2doc( _temp1907), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1968=( char*)"{"; struct _tagged_string _temp1969;
_temp1969.curr= _temp1968; _temp1969.base= _temp1968; _temp1969.last_plus_one=
_temp1968 + 2; _temp1969;}),( struct _tagged_string)({ char* _temp1970=( char*)"}";
struct _tagged_string _temp1971; _temp1971.curr= _temp1970; _temp1971.base=
_temp1970; _temp1971.last_plus_one= _temp1970 + 2; _temp1971;}),( struct
_tagged_string)({ char* _temp1972=( char*)","; struct _tagged_string _temp1973;
_temp1973.curr= _temp1972; _temp1973.base= _temp1972; _temp1973.last_plus_one=
_temp1972 + 2; _temp1973;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1905))); goto _LL1848; _LL1874: s= Cyc_Absynpp_cat3( _temp1917->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp1974= _temp1917->name; if( _temp1974 == 0){ _throw( Null_Exception);}
_temp1974->v;})), Cyc_Absynpp_tvars2doc( _temp1913), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1975=( char*)"{"; struct _tagged_string _temp1976;
_temp1976.curr= _temp1975; _temp1976.base= _temp1975; _temp1976.last_plus_one=
_temp1975 + 2; _temp1976;}),( struct _tagged_string)({ char* _temp1977=( char*)"}";
struct _tagged_string _temp1978; _temp1978.curr= _temp1977; _temp1978.base=
_temp1977; _temp1978.last_plus_one= _temp1977 + 2; _temp1978;}),( struct
_tagged_string)({ char* _temp1979=( char*)","; struct _tagged_string _temp1980;
_temp1980.curr= _temp1979; _temp1980.base= _temp1979; _temp1980.last_plus_one=
_temp1979 + 2; _temp1980;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1911))); goto _LL1848; _LL1876: s= Cyc_Absynpp_qvar2doc( _temp1923); goto
_LL1848; _LL1878: _temp1945= _temp1935; _temp1943= _temp1931; _temp1941=
_temp1929; goto _LL1880; _LL1880: if( _temp1941 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1945);} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1945), Cyc_Absynpp_tvars2doc(
_temp1943), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1981=( char*)"(";
struct _tagged_string _temp1982; _temp1982.curr= _temp1981; _temp1982.base=
_temp1981; _temp1982.last_plus_one= _temp1981 + 2; _temp1982;}),( struct
_tagged_string)({ char* _temp1983=( char*)")"; struct _tagged_string _temp1984;
_temp1984.curr= _temp1983; _temp1984.base= _temp1983; _temp1984.last_plus_one=
_temp1983 + 2; _temp1984;}),( struct _tagged_string)({ char* _temp1985=( char*)",";
struct _tagged_string _temp1986; _temp1986.curr= _temp1985; _temp1986.base=
_temp1985; _temp1986.last_plus_one= _temp1985 + 2; _temp1986;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1941)));} goto _LL1848; _LL1848:;} return s;} struct
Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return Cyc_Absynpp_cat2(
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1987=( char*)""; struct
_tagged_string _temp1988; _temp1988.curr= _temp1987; _temp1988.base= _temp1987;
_temp1988.last_plus_one= _temp1987 + 1; _temp1988;}),( struct _tagged_string)({
char* _temp1989=( char*)"="; struct _tagged_string _temp1990; _temp1990.curr=
_temp1989; _temp1990.base= _temp1989; _temp1990.last_plus_one= _temp1989 + 2;
_temp1990;}),( struct _tagged_string)({ char* _temp1991=( char*)"="; struct
_tagged_string _temp1992; _temp1992.curr= _temp1991; _temp1992.base= _temp1991;
_temp1992.last_plus_one= _temp1991 + 2; _temp1992;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)), Cyc_Absynpp_pat2doc((* dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
==( void*) Cyc_Absyn_Wild_p: 0){ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1993=( char*)"default: "; struct _tagged_string
_temp1994; _temp1994.curr= _temp1993; _temp1994.base= _temp1993; _temp1994.last_plus_one=
_temp1993 + 10; _temp1994;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause == 0){ return Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1995=( char*)"case "; struct
_tagged_string _temp1996; _temp1996.curr= _temp1995; _temp1996.base= _temp1995;
_temp1996.last_plus_one= _temp1995 + 6; _temp1996;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1997=( char*)": "; struct
_tagged_string _temp1998; _temp1998.curr= _temp1997; _temp1998.base= _temp1997;
_temp1998.last_plus_one= _temp1997 + 3; _temp1998;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ return Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1999=( char*)"case "; struct
_tagged_string _temp2000; _temp2000.curr= _temp1999; _temp2000.base= _temp1999;
_temp2000.last_plus_one= _temp1999 + 6; _temp2000;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2001=( char*)" && "; struct
_tagged_string _temp2002; _temp2002.curr= _temp2001; _temp2002.base= _temp2001;
_temp2002.last_plus_one= _temp2001 + 5; _temp2002;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2003= c->where_clause; if(
_temp2003 == 0){ _throw( Null_Exception);} _temp2003;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2004=( char*)": "; struct _tagged_string _temp2005;
_temp2005.curr= _temp2004; _temp2005.base= _temp2004; _temp2005.last_plus_one=
_temp2004 + 3; _temp2005;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,( struct _tagged_string)({
char* _temp2006=( char*)""; struct _tagged_string _temp2007; _temp2007.curr=
_temp2006; _temp2007.base= _temp2006; _temp2007.last_plus_one= _temp2006 + 1;
_temp2007;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield*
f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);} else{ return Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( f->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2008=( char*)" = "; struct _tagged_string _temp2009; _temp2009.curr=
_temp2008; _temp2009.base= _temp2008; _temp2009.last_plus_one= _temp2008 + 4;
_temp2009;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2010= f->tag; if( _temp2010 == 0){ _throw( Null_Exception);} _temp2010;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,(
struct _tagged_string)({ char* _temp2011=( char*)","; struct _tagged_string
_temp2012; _temp2012.curr= _temp2011; _temp2012.base= _temp2011; _temp2012.last_plus_one=
_temp2011 + 2; _temp2012;}), fs);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2013=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2039; struct Cyc_Absyn_Structdecl* _temp2041;
struct Cyc_Absyn_Uniondecl* _temp2043; struct Cyc_Absyn_Vardecl* _temp2045;
struct Cyc_Absyn_Vardecl _temp2047; struct Cyc_List_List* _temp2048; struct Cyc_Core_Opt*
_temp2050; int _temp2052; struct Cyc_Absyn_Exp* _temp2054; void* _temp2056;
struct Cyc_Absyn_Tqual* _temp2058; struct _tuple0* _temp2060; void* _temp2062;
struct Cyc_Absyn_Tuniondecl* _temp2064; struct Cyc_Absyn_XTuniondecl* _temp2066;
int _temp2068; struct Cyc_Absyn_Exp* _temp2070; struct Cyc_Core_Opt* _temp2072;
struct Cyc_Core_Opt* _temp2074; struct Cyc_Absyn_Pat* _temp2076; struct Cyc_Absyn_Enumdecl*
_temp2078; struct Cyc_Absyn_Enumdecl _temp2080; struct Cyc_List_List* _temp2081;
struct _tuple0* _temp2083; void* _temp2085; struct Cyc_Absyn_Typedefdecl*
_temp2087; struct Cyc_List_List* _temp2089; struct _tagged_string* _temp2091;
struct Cyc_List_List* _temp2093; struct _tuple0* _temp2095; struct Cyc_List_List*
_temp2097; _LL2015: if((( struct _tunion_struct*) _temp2013)->tag == Cyc_Absyn_Fn_d_tag){
_LL2040: _temp2039=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp2013)->f1; goto _LL2016;} else{ goto _LL2017;} _LL2017: if((( struct
_tunion_struct*) _temp2013)->tag == Cyc_Absyn_Struct_d_tag){ _LL2042: _temp2041=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp2013)->f1;
goto _LL2018;} else{ goto _LL2019;} _LL2019: if((( struct _tunion_struct*)
_temp2013)->tag == Cyc_Absyn_Union_d_tag){ _LL2044: _temp2043=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp2013)->f1; goto _LL2020;} else{ goto
_LL2021;} _LL2021: if((( struct _tunion_struct*) _temp2013)->tag == Cyc_Absyn_Var_d_tag){
_LL2046: _temp2045=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2013)->f1; _temp2047=* _temp2045; _LL2063: _temp2062=( void*) _temp2047.sc;
goto _LL2061; _LL2061: _temp2060=( struct _tuple0*) _temp2047.name; goto _LL2059;
_LL2059: _temp2058=( struct Cyc_Absyn_Tqual*) _temp2047.tq; goto _LL2057;
_LL2057: _temp2056=( void*) _temp2047.type; goto _LL2055; _LL2055: _temp2054=(
struct Cyc_Absyn_Exp*) _temp2047.initializer; goto _LL2053; _LL2053: _temp2052=(
int) _temp2047.shadow; goto _LL2051; _LL2051: _temp2050=( struct Cyc_Core_Opt*)
_temp2047.region; goto _LL2049; _LL2049: _temp2048=( struct Cyc_List_List*)
_temp2047.attributes; goto _LL2022;} else{ goto _LL2023;} _LL2023: if((( struct
_tunion_struct*) _temp2013)->tag == Cyc_Absyn_Tunion_d_tag){ _LL2065: _temp2064=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp2013)->f1;
goto _LL2024;} else{ goto _LL2025;} _LL2025: if((( struct _tunion_struct*)
_temp2013)->tag == Cyc_Absyn_XTunion_d_tag){ _LL2067: _temp2066=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp2013)->f1; goto _LL2026;} else{ goto
_LL2027;} _LL2027: if((( struct _tunion_struct*) _temp2013)->tag == Cyc_Absyn_Let_d_tag){
_LL2077: _temp2076=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2013)->f1; goto _LL2075; _LL2075: _temp2074=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2013)->f2; goto _LL2073; _LL2073: _temp2072=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2013)->f3; goto
_LL2071; _LL2071: _temp2070=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2013)->f4; goto _LL2069; _LL2069: _temp2068=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp2013)->f5; goto _LL2028;} else{ goto _LL2029;} _LL2029: if((( struct
_tunion_struct*) _temp2013)->tag == Cyc_Absyn_Enum_d_tag){ _LL2079: _temp2078=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp2013)->f1;
_temp2080=* _temp2078; _LL2086: _temp2085=( void*) _temp2080.sc; goto _LL2084;
_LL2084: _temp2083=( struct _tuple0*) _temp2080.name; goto _LL2082; _LL2082:
_temp2081=( struct Cyc_List_List*) _temp2080.fields; goto _LL2030;} else{ goto
_LL2031;} _LL2031: if((( struct _tunion_struct*) _temp2013)->tag == Cyc_Absyn_Typedef_d_tag){
_LL2088: _temp2087=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2013)->f1; goto _LL2032;} else{ goto _LL2033;} _LL2033: if((( struct
_tunion_struct*) _temp2013)->tag == Cyc_Absyn_Namespace_d_tag){ _LL2092:
_temp2091=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2013)->f1; goto _LL2090; _LL2090: _temp2089=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2013)->f2; goto _LL2034;} else{ goto
_LL2035;} _LL2035: if((( struct _tunion_struct*) _temp2013)->tag == Cyc_Absyn_Using_d_tag){
_LL2096: _temp2095=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2013)->f1; goto _LL2094; _LL2094: _temp2093=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp2013)->f2; goto _LL2036;} else{ goto
_LL2037;} _LL2037: if((( struct _tunion_struct*) _temp2013)->tag == Cyc_Absyn_ExternC_d_tag){
_LL2098: _temp2097=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2013)->f1; goto _LL2038;} else{ goto _LL2014;} _LL2016: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2099=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2099[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2100; _temp2100.tag= Cyc_Absyn_FnType_tag; _temp2100.f1=({ struct Cyc_Absyn_FnInfo
_temp2101; _temp2101.tvars= _temp2039->tvs; _temp2101.effect= _temp2039->effect;
_temp2101.ret_typ=( void*)(( void*) _temp2039->ret_type); _temp2101.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2039->args); _temp2101.varargs=
_temp2039->varargs; _temp2101.attributes= 0; _temp2101;}); _temp2100;});
_temp2099;}); s= Cyc_Absynpp_cat7( Cyc_PP_text( _temp2039->is_inline?( struct
_tagged_string)({ char* _temp2102=( char*)"inline "; struct _tagged_string
_temp2103; _temp2103.curr= _temp2102; _temp2103.base= _temp2102; _temp2103.last_plus_one=
_temp2102 + 8; _temp2103;}):( struct _tagged_string)({ char* _temp2104=( char*)"";
struct _tagged_string _temp2105; _temp2105.curr= _temp2104; _temp2105.base=
_temp2104; _temp2105.last_plus_one= _temp2104 + 1; _temp2105;})), Cyc_Absynpp_scope2doc((
void*) _temp2039->sc), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual* _temp2106=(
struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp2106->q_const= 0; _temp2106->q_volatile= 0; _temp2106->q_restrict= 0;
_temp2106;}), t,({ struct Cyc_Core_Opt* _temp2107=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2107->v=( void*) Cyc_Absynpp_cat2(
Cyc_Absynpp_atts2doc( _temp2039->attributes), Cyc_Absynpp_typedef_name2doc(
_temp2039->name)); _temp2107;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2108=( char*)" {"; struct _tagged_string _temp2109; _temp2109.curr=
_temp2108; _temp2109.base= _temp2108; _temp2109.last_plus_one= _temp2108 + 3;
_temp2109;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp2039->body))), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2110=( char*)"}"; struct _tagged_string _temp2111; _temp2111.curr=
_temp2110; _temp2111.base= _temp2110; _temp2111.last_plus_one= _temp2110 + 2;
_temp2111;}))); goto _LL2014;} _LL2018: if( _temp2041->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp2041->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2112=( char*)"struct "; struct _tagged_string
_temp2113; _temp2113.curr= _temp2112; _temp2113.base= _temp2112; _temp2113.last_plus_one=
_temp2112 + 8; _temp2113;})), _temp2041->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2114=( char*)""; struct _tagged_string _temp2115;
_temp2115.curr= _temp2114; _temp2115.base= _temp2114; _temp2115.last_plus_one=
_temp2114 + 1; _temp2115;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp2116= _temp2041->name; if( _temp2116 == 0){ _throw(
Null_Exception);} _temp2116->v;})), Cyc_Absynpp_ktvars2doc( _temp2041->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2117=( char*)";"; struct _tagged_string
_temp2118; _temp2118.curr= _temp2117; _temp2118.base= _temp2117; _temp2118.last_plus_one=
_temp2117 + 2; _temp2118;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2041->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2119=(
char*)"struct "; struct _tagged_string _temp2120; _temp2120.curr= _temp2119;
_temp2120.base= _temp2119; _temp2120.last_plus_one= _temp2119 + 8; _temp2120;})),
_temp2041->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2121=(
char*)""; struct _tagged_string _temp2122; _temp2122.curr= _temp2121; _temp2122.base=
_temp2121; _temp2122.last_plus_one= _temp2121 + 1; _temp2122;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2123= _temp2041->name; if(
_temp2123 == 0){ _throw( Null_Exception);} _temp2123->v;})), Cyc_Absynpp_ktvars2doc(
_temp2041->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2124=( char*)" {";
struct _tagged_string _temp2125; _temp2125.curr= _temp2124; _temp2125.base=
_temp2124; _temp2125.last_plus_one= _temp2124 + 3; _temp2125;})), Cyc_PP_nest( 2,
_temp2041->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2126=(
char*)""; struct _tagged_string _temp2127; _temp2127.curr= _temp2126; _temp2127.base=
_temp2126; _temp2127.last_plus_one= _temp2126 + 1; _temp2127;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2128= _temp2041->fields; if( _temp2128 == 0){ _throw(
Null_Exception);} _temp2128->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2129=( char*)"}"; struct _tagged_string _temp2130;
_temp2130.curr= _temp2129; _temp2130.base= _temp2129; _temp2130.last_plus_one=
_temp2129 + 2; _temp2130;})), Cyc_Absynpp_atts2doc( _temp2041->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2131=( char*)";"; struct _tagged_string
_temp2132; _temp2132.curr= _temp2131; _temp2132.base= _temp2131; _temp2132.last_plus_one=
_temp2131 + 2; _temp2132;})));} goto _LL2014; _LL2020: if( _temp2043->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp2043->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2133=( char*)"union "; struct _tagged_string
_temp2134; _temp2134.curr= _temp2133; _temp2134.base= _temp2133; _temp2134.last_plus_one=
_temp2133 + 7; _temp2134;})), _temp2043->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2135=( char*)""; struct _tagged_string _temp2136;
_temp2136.curr= _temp2135; _temp2136.base= _temp2135; _temp2136.last_plus_one=
_temp2135 + 1; _temp2136;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2137= _temp2043->name; if( _temp2137 == 0){ _throw(
Null_Exception);} _temp2137->v;})), Cyc_Absynpp_tvars2doc( _temp2043->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2138=( char*)";"; struct _tagged_string
_temp2139; _temp2139.curr= _temp2138; _temp2139.base= _temp2138; _temp2139.last_plus_one=
_temp2138 + 2; _temp2139;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2043->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2140=(
char*)"union "; struct _tagged_string _temp2141; _temp2141.curr= _temp2140;
_temp2141.base= _temp2140; _temp2141.last_plus_one= _temp2140 + 7; _temp2141;})),
_temp2043->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2142=(
char*)""; struct _tagged_string _temp2143; _temp2143.curr= _temp2142; _temp2143.base=
_temp2142; _temp2143.last_plus_one= _temp2142 + 1; _temp2143;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2144= _temp2043->name; if(
_temp2144 == 0){ _throw( Null_Exception);} _temp2144->v;})), Cyc_Absynpp_tvars2doc(
_temp2043->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2145=( char*)" {";
struct _tagged_string _temp2146; _temp2146.curr= _temp2145; _temp2146.base=
_temp2145; _temp2146.last_plus_one= _temp2145 + 3; _temp2146;})), Cyc_PP_nest( 2,
_temp2043->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2147=(
char*)""; struct _tagged_string _temp2148; _temp2148.curr= _temp2147; _temp2148.base=
_temp2147; _temp2148.last_plus_one= _temp2147 + 1; _temp2148;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2149= _temp2043->fields; if( _temp2149 == 0){ _throw(
Null_Exception);} _temp2149->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2150=( char*)"}"; struct _tagged_string _temp2151;
_temp2151.curr= _temp2150; _temp2151.base= _temp2150; _temp2151.last_plus_one=
_temp2150 + 2; _temp2151;})), Cyc_Absynpp_atts2doc( _temp2043->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2152=( char*)";"; struct _tagged_string
_temp2153; _temp2153.curr= _temp2152; _temp2153.base= _temp2152; _temp2153.last_plus_one=
_temp2152 + 2; _temp2153;})));} goto _LL2014; _LL2022: { struct Cyc_PP_Doc* sn=
Cyc_Absynpp_typedef_name2doc( _temp2060); s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(
_temp2062), Cyc_Absynpp_tqtd2doc( _temp2058, _temp2056,({ struct Cyc_Core_Opt*
_temp2154=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2154->v=( void*) sn; _temp2154;})), Cyc_Absynpp_atts2doc( _temp2048),
_temp2054 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2155=( char*)"";
struct _tagged_string _temp2156; _temp2156.curr= _temp2155; _temp2156.base=
_temp2155; _temp2156.last_plus_one= _temp2155 + 1; _temp2156;})): Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2157=( char*)" = "; struct
_tagged_string _temp2158; _temp2158.curr= _temp2157; _temp2158.base= _temp2157;
_temp2158.last_plus_one= _temp2157 + 4; _temp2158;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2159= _temp2054; if(
_temp2159 == 0){ _throw( Null_Exception);} _temp2159;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2160=( char*)";"; struct _tagged_string _temp2161;
_temp2161.curr= _temp2160; _temp2161.base= _temp2160; _temp2161.last_plus_one=
_temp2160 + 2; _temp2161;}))); goto _LL2014;} _LL2024: if( _temp2064->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp2064->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2162=( char*)"tunion "; struct
_tagged_string _temp2163; _temp2163.curr= _temp2162; _temp2163.base= _temp2162;
_temp2163.last_plus_one= _temp2162 + 8; _temp2163;})), _temp2064->name == 0? Cyc_PP_text((
struct _tagged_string)({ char* _temp2164=( char*)""; struct _tagged_string
_temp2165; _temp2165.curr= _temp2164; _temp2165.base= _temp2164; _temp2165.last_plus_one=
_temp2164 + 1; _temp2165;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp2166= _temp2064->name; if( _temp2166 == 0){ _throw(
Null_Exception);} _temp2166->v;})), Cyc_Absynpp_ktvars2doc( _temp2064->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2167=( char*)";"; struct _tagged_string
_temp2168; _temp2168.curr= _temp2167; _temp2168.base= _temp2167; _temp2168.last_plus_one=
_temp2167 + 2; _temp2168;})));} else{ s= Cyc_Absynpp_cat8( Cyc_Absynpp_scope2doc((
void*) _temp2064->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2169=(
char*)"tunion "; struct _tagged_string _temp2170; _temp2170.curr= _temp2169;
_temp2170.base= _temp2169; _temp2170.last_plus_one= _temp2169 + 8; _temp2170;})),
_temp2064->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2171=(
char*)""; struct _tagged_string _temp2172; _temp2172.curr= _temp2171; _temp2172.base=
_temp2171; _temp2172.last_plus_one= _temp2171 + 1; _temp2172;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2173= _temp2064->name; if(
_temp2173 == 0){ _throw( Null_Exception);} _temp2173->v;})), Cyc_Absynpp_ktvars2doc(
_temp2064->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2174=( char*)" {";
struct _tagged_string _temp2175; _temp2175.curr= _temp2174; _temp2175.base=
_temp2174; _temp2175.last_plus_one= _temp2174 + 3; _temp2175;})), Cyc_PP_nest( 2,
_temp2064->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2176=(
char*)""; struct _tagged_string _temp2177; _temp2177.curr= _temp2176; _temp2177.base=
_temp2176; _temp2177.last_plus_one= _temp2176 + 1; _temp2177;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2178= _temp2064->fields; if( _temp2178 == 0){ _throw(
Null_Exception);} _temp2178->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2179=( char*)"};"; struct _tagged_string _temp2180;
_temp2180.curr= _temp2179; _temp2180.base= _temp2179; _temp2180.last_plus_one=
_temp2179 + 3; _temp2180;})));} goto _LL2014; _LL2026: s= Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc((
void*) _temp2066->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2181=(
char*)"xtunion "; struct _tagged_string _temp2182; _temp2182.curr= _temp2181;
_temp2182.base= _temp2181; _temp2182.last_plus_one= _temp2181 + 9; _temp2182;})),
Cyc_Absynpp_typedef_name2doc( _temp2066->name), _temp2066->fields == 0? Cyc_PP_text((
struct _tagged_string)({ char* _temp2183=( char*)";"; struct _tagged_string
_temp2184; _temp2184.curr= _temp2183; _temp2184.base= _temp2183; _temp2184.last_plus_one=
_temp2183 + 2; _temp2184;})): Cyc_Absynpp_cat4( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2185=( char*)" {"; struct _tagged_string _temp2186;
_temp2186.curr= _temp2185; _temp2186.base= _temp2185; _temp2186.last_plus_one=
_temp2185 + 3; _temp2186;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_tunionfields2doc( _temp2066->fields))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp2187=( char*)"};"; struct _tagged_string
_temp2188; _temp2188.curr= _temp2187; _temp2188.base= _temp2187; _temp2188.last_plus_one=
_temp2187 + 3; _temp2188;})))); goto _LL2014; _LL2028: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp2189=( char*)"let "; struct _tagged_string
_temp2190; _temp2190.curr= _temp2189; _temp2190.base= _temp2189; _temp2190.last_plus_one=
_temp2189 + 5; _temp2190;})), Cyc_Absynpp_pat2doc( _temp2076), Cyc_PP_text((
struct _tagged_string)({ char* _temp2191=( char*)" = "; struct _tagged_string
_temp2192; _temp2192.curr= _temp2191; _temp2192.base= _temp2191; _temp2192.last_plus_one=
_temp2191 + 4; _temp2192;})), Cyc_Absynpp_exp2doc( _temp2070), Cyc_PP_text((
struct _tagged_string)({ char* _temp2193=( char*)";"; struct _tagged_string
_temp2194; _temp2194.curr= _temp2193; _temp2194.base= _temp2193; _temp2194.last_plus_one=
_temp2193 + 2; _temp2194;}))); goto _LL2014; _LL2030: s= Cyc_Absynpp_cat7( Cyc_Absynpp_scope2doc(
_temp2085), Cyc_PP_text(( struct _tagged_string)({ char* _temp2195=( char*)"enum ";
struct _tagged_string _temp2196; _temp2196.curr= _temp2195; _temp2196.base=
_temp2195; _temp2196.last_plus_one= _temp2195 + 6; _temp2196;})), Cyc_Absynpp_qvar2doc(
_temp2083), Cyc_PP_text(( struct _tagged_string)({ char* _temp2197=( char*)" {";
struct _tagged_string _temp2198; _temp2198.curr= _temp2197; _temp2198.base=
_temp2197; _temp2198.last_plus_one= _temp2197 + 3; _temp2198;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_enumfields2doc( _temp2081))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp2199=( char*)"};";
struct _tagged_string _temp2200; _temp2200.curr= _temp2199; _temp2200.base=
_temp2199; _temp2200.last_plus_one= _temp2199 + 3; _temp2200;}))); goto _LL2014;
_LL2032: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2201=( char*)"typedef "; struct _tagged_string _temp2202; _temp2202.curr=
_temp2201; _temp2202.base= _temp2201; _temp2202.last_plus_one= _temp2201 + 9;
_temp2202;})), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual* _temp2203=(
struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp2203->q_const= 0; _temp2203->q_volatile= 0; _temp2203->q_restrict= 0;
_temp2203;}),( void*) _temp2087->defn,({ struct Cyc_Core_Opt* _temp2204=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2204->v=( void*)
Cyc_Absynpp_cat2( Cyc_Absynpp_typedef_name2doc( _temp2087->name), Cyc_Absynpp_tvars2doc(
_temp2087->tvs)); _temp2204;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2205=( char*)";"; struct _tagged_string _temp2206; _temp2206.curr=
_temp2205; _temp2206.base= _temp2205; _temp2206.last_plus_one= _temp2205 + 2;
_temp2206;}))); goto _LL2014; _LL2034: if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_add(
_temp2091);} s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2207=( char*)"namespace "; struct _tagged_string _temp2208; _temp2208.curr=
_temp2207; _temp2208.base= _temp2207; _temp2208.last_plus_one= _temp2207 + 11;
_temp2208;})), Cyc_Absynpp_textptr( _temp2091), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2209=( char*)" {"; struct _tagged_string _temp2210;
_temp2210.curr= _temp2209; _temp2210.base= _temp2209; _temp2210.last_plus_one=
_temp2209 + 3; _temp2210;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2211=( char*)""; struct _tagged_string _temp2212; _temp2212.curr=
_temp2211; _temp2212.base= _temp2211; _temp2212.last_plus_one= _temp2211 + 1;
_temp2212;}), _temp2089), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2213=( char*)"}"; struct _tagged_string _temp2214; _temp2214.curr=
_temp2213; _temp2214.base= _temp2213; _temp2214.last_plus_one= _temp2213 + 2;
_temp2214;}))); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2014; _LL2036: if( Cyc_Absynpp_print_using_stmts){ s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2215=( char*)"using "; struct
_tagged_string _temp2216; _temp2216.curr= _temp2215; _temp2216.base= _temp2215;
_temp2216.last_plus_one= _temp2215 + 7; _temp2216;})), Cyc_Absynpp_qvar2doc(
_temp2095), Cyc_PP_text(( struct _tagged_string)({ char* _temp2217=( char*)" {";
struct _tagged_string _temp2218; _temp2218.curr= _temp2217; _temp2218.base=
_temp2217; _temp2218.last_plus_one= _temp2217 + 3; _temp2218;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2219=( char*)""; struct _tagged_string
_temp2220; _temp2220.curr= _temp2219; _temp2220.base= _temp2219; _temp2220.last_plus_one=
_temp2219 + 1; _temp2220;}), _temp2093), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2221=( char*)"}"; struct _tagged_string _temp2222;
_temp2222.curr= _temp2221; _temp2222.base= _temp2221; _temp2222.last_plus_one=
_temp2221 + 2; _temp2222;})));} else{ s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2223=( char*)"/* using "; struct _tagged_string
_temp2224; _temp2224.curr= _temp2223; _temp2224.base= _temp2223; _temp2224.last_plus_one=
_temp2223 + 10; _temp2224;})), Cyc_Absynpp_qvar2doc( _temp2095), Cyc_PP_text((
struct _tagged_string)({ char* _temp2225=( char*)" { */"; struct _tagged_string
_temp2226; _temp2226.curr= _temp2225; _temp2226.base= _temp2225; _temp2226.last_plus_one=
_temp2225 + 6; _temp2226;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2227=( char*)""; struct _tagged_string _temp2228; _temp2228.curr=
_temp2227; _temp2228.base= _temp2227; _temp2228.last_plus_one= _temp2227 + 1;
_temp2228;}), _temp2093), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2229=( char*)"/* } */"; struct _tagged_string _temp2230; _temp2230.curr=
_temp2229; _temp2230.base= _temp2229; _temp2230.last_plus_one= _temp2229 + 8;
_temp2230;})));} goto _LL2014; _LL2038: if( Cyc_Absynpp_print_externC_stmts){ s=
Cyc_Absynpp_cat5( Cyc_PP_text(( struct _tagged_string)({ char* _temp2231=( char*)"extern \"C\" {";
struct _tagged_string _temp2232; _temp2232.curr= _temp2231; _temp2232.base=
_temp2231; _temp2232.last_plus_one= _temp2231 + 13; _temp2232;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2233=( char*)""; struct _tagged_string
_temp2234; _temp2234.curr= _temp2233; _temp2234.base= _temp2233; _temp2234.last_plus_one=
_temp2233 + 1; _temp2234;}), _temp2097), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2235=( char*)"}"; struct _tagged_string _temp2236;
_temp2236.curr= _temp2235; _temp2236.base= _temp2235; _temp2236.last_plus_one=
_temp2235 + 2; _temp2236;})));} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2237=( char*)"/* extern \"C\" { */"; struct
_tagged_string _temp2238; _temp2238.curr= _temp2237; _temp2238.base= _temp2237;
_temp2238.last_plus_one= _temp2237 + 19; _temp2238;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2239=( char*)""; struct _tagged_string
_temp2240; _temp2240.curr= _temp2239; _temp2240.base= _temp2239; _temp2240.last_plus_one=
_temp2239 + 1; _temp2240;}), _temp2097), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2241=( char*)"/* } */"; struct _tagged_string
_temp2242; _temp2242.curr= _temp2241; _temp2242.base= _temp2241; _temp2242.last_plus_one=
_temp2241 + 8; _temp2242;})));} goto _LL2014; _LL2014:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void* sc){ void* _temp2243= sc; _LL2245: if(( int)
_temp2243 == Cyc_Absyn_Static){ goto _LL2246;} else{ goto _LL2247;} _LL2247: if((
int) _temp2243 == Cyc_Absyn_Public){ goto _LL2248;} else{ goto _LL2249;} _LL2249:
if(( int) _temp2243 == Cyc_Absyn_Extern){ goto _LL2250;} else{ goto _LL2251;}
_LL2251: if(( int) _temp2243 == Cyc_Absyn_ExternC){ goto _LL2252;} else{ goto
_LL2253;} _LL2253: if(( int) _temp2243 == Cyc_Absyn_Abstract){ goto _LL2254;}
else{ goto _LL2244;} _LL2246: return Cyc_PP_text(( struct _tagged_string)({ char*
_temp2255=( char*)"static "; struct _tagged_string _temp2256; _temp2256.curr=
_temp2255; _temp2256.base= _temp2255; _temp2256.last_plus_one= _temp2255 + 8;
_temp2256;})); _LL2248: return Cyc_PP_nil_doc(); _LL2250: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2257=( char*)"extern "; struct
_tagged_string _temp2258; _temp2258.curr= _temp2257; _temp2258.base= _temp2257;
_temp2258.last_plus_one= _temp2257 + 8; _temp2258;})); _LL2252: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2259=( char*)"extern \"C\" "; struct
_tagged_string _temp2260; _temp2260.curr= _temp2259; _temp2260.base= _temp2259;
_temp2260.last_plus_one= _temp2259 + 12; _temp2260;})); _LL2254: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2261=( char*)"abstract "; struct
_tagged_string _temp2262; _temp2262.curr= _temp2261; _temp2262.base= _temp2261;
_temp2262.last_plus_one= _temp2261 + 10; _temp2262;})); _LL2244:;} int Cyc_Absynpp_exists_temp_tvar_in_effect(
void* t){ void* _temp2263= t; struct Cyc_Absyn_Tvar* _temp2271; struct Cyc_List_List*
_temp2273; _LL2265: if(( unsigned int) _temp2263 > 4u?(( struct _tunion_struct*)
_temp2263)->tag == Cyc_Absyn_VarType_tag: 0){ _LL2272: _temp2271=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp2263)->f1; goto _LL2266;} else{ goto
_LL2267;} _LL2267: if(( unsigned int) _temp2263 > 4u?(( struct _tunion_struct*)
_temp2263)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL2274: _temp2273=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp2263)->f1; goto _LL2268;} else{ goto
_LL2269;} _LL2269: goto _LL2270; _LL2266: return Cyc_Tcutil_is_temp_tvar(
_temp2271); _LL2268: return(( int(*)( int(* pred)( void*), struct Cyc_List_List*
x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect, _temp2273);
_LL2270: return 0; _LL2264:;} struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual*
tq, void* t){ void* _temp2275= t; struct Cyc_Absyn_Exp* _temp2289; struct Cyc_Absyn_Tqual*
_temp2291; void* _temp2293; struct Cyc_Absyn_PtrInfo _temp2295; struct Cyc_Absyn_Conref*
_temp2297; struct Cyc_Absyn_Tqual* _temp2299; struct Cyc_Absyn_Conref* _temp2301;
void* _temp2303; void* _temp2305; struct Cyc_Absyn_FnInfo _temp2307; struct Cyc_List_List*
_temp2309; int _temp2311; struct Cyc_List_List* _temp2313; void* _temp2315;
struct Cyc_Core_Opt* _temp2317; struct Cyc_List_List* _temp2319; int _temp2321;
struct Cyc_Core_Opt* _temp2323; void* _temp2325; struct Cyc_Core_Opt* _temp2327;
struct Cyc_List_List* _temp2329; struct _tuple0* _temp2331; _LL2277: if((
unsigned int) _temp2275 > 4u?(( struct _tunion_struct*) _temp2275)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2294: _temp2293=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2275)->f1;
goto _LL2292; _LL2292: _temp2291=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2275)->f2; goto _LL2290; _LL2290: _temp2289=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2275)->f3; goto _LL2278;} else{ goto
_LL2279;} _LL2279: if(( unsigned int) _temp2275 > 4u?(( struct _tunion_struct*)
_temp2275)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2296: _temp2295=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2275)->f1;
_LL2306: _temp2305=( void*) _temp2295.elt_typ; goto _LL2304; _LL2304: _temp2303=(
void*) _temp2295.rgn_typ; goto _LL2302; _LL2302: _temp2301=( struct Cyc_Absyn_Conref*)
_temp2295.nullable; goto _LL2300; _LL2300: _temp2299=( struct Cyc_Absyn_Tqual*)
_temp2295.tq; goto _LL2298; _LL2298: _temp2297=( struct Cyc_Absyn_Conref*)
_temp2295.bounds; goto _LL2280;} else{ goto _LL2281;} _LL2281: if(( unsigned int)
_temp2275 > 4u?(( struct _tunion_struct*) _temp2275)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2308: _temp2307=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2275)->f1; _LL2320: _temp2319=( struct Cyc_List_List*) _temp2307.tvars;
goto _LL2318; _LL2318: _temp2317=( struct Cyc_Core_Opt*) _temp2307.effect; goto
_LL2316; _LL2316: _temp2315=( void*) _temp2307.ret_typ; goto _LL2314; _LL2314:
_temp2313=( struct Cyc_List_List*) _temp2307.args; goto _LL2312; _LL2312:
_temp2311=( int) _temp2307.varargs; goto _LL2310; _LL2310: _temp2309=( struct
Cyc_List_List*) _temp2307.attributes; goto _LL2282;} else{ goto _LL2283;}
_LL2283: if(( unsigned int) _temp2275 > 4u?(( struct _tunion_struct*) _temp2275)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL2326: _temp2325=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2275)->f1; goto _LL2324; _LL2324: _temp2323=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp2275)->f2; goto _LL2322; _LL2322: _temp2321=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp2275)->f3; goto _LL2284;} else{ goto
_LL2285;} _LL2285: if(( unsigned int) _temp2275 > 4u?(( struct _tunion_struct*)
_temp2275)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL2332: _temp2331=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2275)->f1; goto _LL2330;
_LL2330: _temp2329=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2275)->f2; goto _LL2328; _LL2328: _temp2327=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2275)->f3; goto _LL2286;} else{ goto
_LL2287;} _LL2287: goto _LL2288; _LL2278: { struct Cyc_List_List* _temp2335;
void* _temp2337; struct Cyc_Absyn_Tqual* _temp2339; struct _tuple4 _temp2333=
Cyc_Absynpp_to_tms( _temp2291, _temp2293); _LL2340: _temp2339= _temp2333.f1;
goto _LL2338; _LL2338: _temp2337= _temp2333.f2; goto _LL2336; _LL2336: _temp2335=
_temp2333.f3; goto _LL2334; _LL2334: { void* tm; if( _temp2289 == 0){ tm=( void*)
Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2341=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2341[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2342; _temp2342.tag= Cyc_Absyn_ConstArray_mod_tag; _temp2342.f1=( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2343= _temp2289; if( _temp2343 == 0){
_throw( Null_Exception);} _temp2343;}); _temp2342;}); _temp2341;});} return({
struct _tuple4 _temp2344; _temp2344.f1= _temp2339; _temp2344.f2= _temp2337;
_temp2344.f3=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2345=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2345->hd=( void*)
tm; _temp2345->tl= _temp2335; _temp2345;}); _temp2344;});}} _LL2280: { struct
Cyc_List_List* _temp2348; void* _temp2350; struct Cyc_Absyn_Tqual* _temp2352;
struct _tuple4 _temp2346= Cyc_Absynpp_to_tms( _temp2299, _temp2305); _LL2353:
_temp2352= _temp2346.f1; goto _LL2351; _LL2351: _temp2350= _temp2346.f2; goto
_LL2349; _LL2349: _temp2348= _temp2346.f3; goto _LL2347; _LL2347: { void* ps;{
struct _tuple8 _temp2355=({ struct _tuple8 _temp2354; _temp2354.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2301))->v; _temp2354.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2297))->v; _temp2354;}); void* _temp2365;
void* _temp2367; struct Cyc_Absyn_Exp* _temp2369; void* _temp2371; int _temp2373;
void* _temp2375; void* _temp2377; struct Cyc_Absyn_Exp* _temp2379; void*
_temp2381; int _temp2383; void* _temp2385; void* _temp2387; void* _temp2389;
_LL2357: _LL2372: _temp2371= _temp2355.f1; if(( unsigned int) _temp2371 > 1u?((
struct _tunion_struct*) _temp2371)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2374:
_temp2373=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2371)->f1; if(
_temp2373 == 1){ goto _LL2366;} else{ goto _LL2359;}} else{ goto _LL2359;}
_LL2366: _temp2365= _temp2355.f2; if(( unsigned int) _temp2365 > 1u?(( struct
_tunion_struct*) _temp2365)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2368:
_temp2367=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2365)->f1; if((
unsigned int) _temp2367 > 1u?(( struct _tunion_struct*) _temp2367)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2370: _temp2369=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2367)->f1; goto _LL2358;} else{ goto _LL2359;}} else{ goto _LL2359;}
_LL2359: _LL2382: _temp2381= _temp2355.f1; if(( unsigned int) _temp2381 > 1u?((
struct _tunion_struct*) _temp2381)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2384:
_temp2383=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2381)->f1; if(
_temp2383 == 0){ goto _LL2376;} else{ goto _LL2361;}} else{ goto _LL2361;}
_LL2376: _temp2375= _temp2355.f2; if(( unsigned int) _temp2375 > 1u?(( struct
_tunion_struct*) _temp2375)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2378:
_temp2377=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2375)->f1; if((
unsigned int) _temp2377 > 1u?(( struct _tunion_struct*) _temp2377)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2380: _temp2379=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2377)->f1; goto _LL2360;} else{ goto _LL2361;}} else{ goto _LL2361;}
_LL2361: _LL2390: _temp2389= _temp2355.f1; goto _LL2386; _LL2386: _temp2385=
_temp2355.f2; if(( unsigned int) _temp2385 > 1u?(( struct _tunion_struct*)
_temp2385)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2388: _temp2387=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2385)->f1; if(( int) _temp2387 == Cyc_Absyn_Unknown_b){
goto _LL2362;} else{ goto _LL2363;}} else{ goto _LL2363;} _LL2363: goto _LL2364;
_LL2358: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2391=( struct
Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2391[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2392; _temp2392.tag=
Cyc_Absyn_Nullable_ps_tag; _temp2392.f1= _temp2369; _temp2392;}); _temp2391;});
goto _LL2356; _LL2360: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2393=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2393[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2394; _temp2394.tag= Cyc_Absyn_NonNullable_ps_tag; _temp2394.f1= _temp2379;
_temp2394;}); _temp2393;}); goto _LL2356; _LL2362: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2356; _LL2364: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2356;
_LL2356:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2395=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2395[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2396; _temp2396.tag=
Cyc_Absyn_Pointer_mod_tag; _temp2396.f1=( void*) ps; _temp2396.f2=( void*)
_temp2303; _temp2396.f3= tq; _temp2396;}); _temp2395;}); return({ struct _tuple4
_temp2397; _temp2397.f1= _temp2352; _temp2397.f2= _temp2350; _temp2397.f3=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2398=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2398->hd=( void*) tm; _temp2398->tl=
_temp2348; _temp2398;}); _temp2397;});}}} _LL2282: if( ! Cyc_Absynpp_print_all_tvars){
if( _temp2317 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)({ struct
Cyc_Core_Opt* _temp2399= _temp2317; if( _temp2399 == 0){ _throw( Null_Exception);}
_temp2399->v;}))){ _temp2317= 0; _temp2319= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2319);}}{ struct Cyc_List_List* _temp2402;
void* _temp2404; struct Cyc_Absyn_Tqual* _temp2406; struct _tuple4 _temp2400=
Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(), _temp2315); _LL2407: _temp2406=
_temp2400.f1; goto _LL2405; _LL2405: _temp2404= _temp2400.f2; goto _LL2403;
_LL2403: _temp2402= _temp2400.f3; goto _LL2401; _LL2401: { struct Cyc_List_List*
tms= _temp2402; if( _temp2309 != 0){ tms=({ struct Cyc_List_List* _temp2408=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2408->hd=(
void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp2409=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2409[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2410; _temp2410.tag= Cyc_Absyn_Attributes_mod_tag;
_temp2410.f1= 0; _temp2410.f2= _temp2309; _temp2410;}); _temp2409;})); _temp2408->tl=
tms; _temp2408;});} tms=({ struct Cyc_List_List* _temp2411=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2411->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2412=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2412[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2413; _temp2413.tag= Cyc_Absyn_Function_mod_tag;
_temp2413.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2414=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2414[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2415; _temp2415.tag= Cyc_Absyn_WithTypes_tag;
_temp2415.f1= _temp2313; _temp2415.f2= _temp2311; _temp2415.f3= _temp2317;
_temp2415;}); _temp2414;})); _temp2413;}); _temp2412;})); _temp2411->tl= tms;
_temp2411;}); if( _temp2319 != 0){ tms=({ struct Cyc_List_List* _temp2416=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2416->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2417=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2417[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2418; _temp2418.tag= Cyc_Absyn_TypeParams_mod_tag;
_temp2418.f1= _temp2319; _temp2418.f2= 0; _temp2418.f3= 1; _temp2418;});
_temp2417;})); _temp2416->tl= tms; _temp2416;});} return({ struct _tuple4
_temp2419; _temp2419.f1= _temp2406; _temp2419.f2= _temp2404; _temp2419.f3= tms;
_temp2419;});}} _LL2284: if( _temp2323 == 0){ return({ struct _tuple4 _temp2420;
_temp2420.f1= tq; _temp2420.f2= t; _temp2420.f3= 0; _temp2420;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2421= _temp2323; if(
_temp2421 == 0){ _throw( Null_Exception);} _temp2421->v;}));} _LL2286: if(
_temp2327 == 0? 1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4
_temp2422; _temp2422.f1= tq; _temp2422.f2= t; _temp2422.f3= 0; _temp2422;});}
else{ return Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2423=
_temp2327; if( _temp2423 == 0){ _throw( Null_Exception);} _temp2423->v;}));}
_LL2288: return({ struct _tuple4 _temp2424; _temp2424.f1= tq; _temp2424.f2= t;
_temp2424.f3= 0; _temp2424;}); _LL2276:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual* tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp2427; void* _temp2429; struct Cyc_Absyn_Tqual* _temp2431; struct _tuple4
_temp2425= Cyc_Absynpp_to_tms( tq, t); _LL2432: _temp2431= _temp2425.f1; goto
_LL2430; _LL2430: _temp2429= _temp2425.f2; goto _LL2428; _LL2428: _temp2427=
_temp2425.f3; goto _LL2426; _LL2426: _temp2427=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2427); if( _temp2427 == 0?
dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc( _temp2431), Cyc_Absynpp_ntyp2doc(
_temp2429));} else{ return Cyc_Absynpp_cat4( Cyc_Absynpp_tqual2doc( _temp2431),
Cyc_Absynpp_ntyp2doc( _temp2429), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2433=( char*)" "; struct _tagged_string _temp2434; _temp2434.curr=
_temp2433; _temp2434.base= _temp2433; _temp2434.last_plus_one= _temp2433 + 2;
_temp2434;})), Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)({
struct Cyc_Core_Opt* _temp2435= dopt; if( _temp2435 == 0){ _throw(
Null_Exception);} _temp2435->v;}), _temp2427));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return Cyc_Absynpp_cat5(
Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2436=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2436->v=(
void*) Cyc_Absynpp_textptr( f->name); _temp2436;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2437=( char*)":"; struct _tagged_string _temp2438;
_temp2438.curr= _temp2437; _temp2438.base= _temp2437; _temp2438.last_plus_one=
_temp2437 + 2; _temp2438;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp2439= f->width; if( _temp2439 == 0){ _throw(
Null_Exception);} _temp2439->v;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2440=( char*)";"; struct _tagged_string
_temp2441; _temp2441.curr= _temp2440; _temp2441.base= _temp2440; _temp2441.last_plus_one=
_temp2440 + 2; _temp2441;})));} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2442=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2442->v=( void*) Cyc_Absynpp_textptr(
f->name); _temp2442;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2443=( char*)";"; struct _tagged_string
_temp2444; _temp2444.curr= _temp2443; _temp2444.base= _temp2443; _temp2444.last_plus_one=
_temp2443 + 2; _temp2444;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,( struct _tagged_string)({ char*
_temp2445=( char*)""; struct _tagged_string _temp2446; _temp2446.curr= _temp2445;
_temp2446.base= _temp2445; _temp2446.last_plus_one= _temp2445 + 1; _temp2446;}),
fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc( struct Cyc_Absyn_Tunionfield*
f){ return Cyc_Absynpp_cat3( Cyc_Absynpp_typedef_name2doc( f->name), f->tvs == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs), f->typs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,( struct _tagged_string)({ char*
_temp2447=( char*)","; struct _tagged_string _temp2448; _temp2448.curr=
_temp2447; _temp2448.base= _temp2447; _temp2448.last_plus_one= _temp2447 + 2;
_temp2448;}), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl=({ struct Cyc_List_List*
_temp2449= tdl; if( _temp2449 == 0){ _throw( Null_Exception);} _temp2449->tl;})){
Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp2450= tdl; if( _temp2450 == 0){ _throw( Null_Exception);} _temp2450->hd;})),
72, f); fprintf( f,"\n");}} struct _tagged_string Cyc_Absynpp_decllist2string(
struct Cyc_List_List* tdl){ return Cyc_PP_string_of_doc( Cyc_PP_seql(( struct
_tagged_string)({ char* _temp2451=( char*)""; struct _tagged_string _temp2452;
_temp2452.curr= _temp2451; _temp2452.base= _temp2451; _temp2452.last_plus_one=
_temp2451 + 1; _temp2452;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
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