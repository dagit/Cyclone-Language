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
int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef unsigned short Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
unsigned int Cyc_vm_size_t; typedef char Cyc_int8_t; typedef char Cyc_u_int8_t;
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
int); extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern struct _tagged_string Cyc_Core_string_of_int( int); extern char*
string_to_Cstring( struct _tagged_string); extern char* underlying_Cstring(
struct _tagged_string); extern struct _tagged_string Cstring_to_string( char*);
extern int system( char*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[
14u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_List_exists( int(* pred)( void*), struct Cyc_List_List*
x); extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); extern int Cyc_List_list_prefix( int(* cmp)( void*,
void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w,
struct Cyc_Stdio___sFILE* f); extern struct _tagged_string Cyc_PP_string_of_doc(
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
0; static void Cyc_Absynpp_curr_namespace_add( struct _tagged_string* v){ Cyc_Absynpp_curr_namespace=({
struct Cyc_List_List* _temp0=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp0->hd=( void*) v; _temp0->tl= Cyc_Absynpp_curr_namespace;
_temp0;});} static void Cyc_Absynpp_curr_namespace_drop(){ Cyc_Absynpp_curr_namespace=({
struct Cyc_List_List* _temp1= Cyc_Absynpp_curr_namespace; if( _temp1 == 0){
_throw( Null_Exception);} _temp1->tl;});} struct _tagged_string Cyc_Absynpp_char_escape(
char c){ switch( c){ case '\a': _LL2: return( struct _tagged_string)({ char*
_temp4=( char*)"\\a"; struct _tagged_string _temp5; _temp5.curr= _temp4; _temp5.base=
_temp4; _temp5.last_plus_one= _temp4 + 3; _temp5;}); case '\b': _LL3: return(
struct _tagged_string)({ char* _temp7=( char*)"\\b"; struct _tagged_string
_temp8; _temp8.curr= _temp7; _temp8.base= _temp7; _temp8.last_plus_one= _temp7 +
3; _temp8;}); case '\f': _LL6: return( struct _tagged_string)({ char* _temp10=(
char*)"\\f"; struct _tagged_string _temp11; _temp11.curr= _temp10; _temp11.base=
_temp10; _temp11.last_plus_one= _temp10 + 3; _temp11;}); case '\n': _LL9: return(
struct _tagged_string)({ char* _temp13=( char*)"\\n"; struct _tagged_string
_temp14; _temp14.curr= _temp13; _temp14.base= _temp13; _temp14.last_plus_one=
_temp13 + 3; _temp14;}); case '\r': _LL12: return( struct _tagged_string)({ char*
_temp16=( char*)"\\r"; struct _tagged_string _temp17; _temp17.curr= _temp16;
_temp17.base= _temp16; _temp17.last_plus_one= _temp16 + 3; _temp17;}); case '\t':
_LL15: return( struct _tagged_string)({ char* _temp19=( char*)"\\t"; struct
_tagged_string _temp20; _temp20.curr= _temp19; _temp20.base= _temp19; _temp20.last_plus_one=
_temp19 + 3; _temp20;}); case '\v': _LL18: return( struct _tagged_string)({ char*
_temp22=( char*)"\\v"; struct _tagged_string _temp23; _temp23.curr= _temp22;
_temp23.base= _temp22; _temp23.last_plus_one= _temp22 + 3; _temp23;}); case '\\':
_LL21: return( struct _tagged_string)({ char* _temp25=( char*)"\\\\"; struct
_tagged_string _temp26; _temp26.curr= _temp25; _temp26.base= _temp25; _temp26.last_plus_one=
_temp25 + 3; _temp26;}); case '"': _LL24: return( struct _tagged_string)({ char*
_temp28=( char*)"\""; struct _tagged_string _temp29; _temp29.curr= _temp28;
_temp29.base= _temp28; _temp29.last_plus_one= _temp28 + 2; _temp29;}); case '\'':
_LL27: return( struct _tagged_string)({ char* _temp31=( char*)"\\'"; struct
_tagged_string _temp32; _temp32.curr= _temp31; _temp32.base= _temp31; _temp32.last_plus_one=
_temp31 + 3; _temp32;}); default: _LL30: if(( int) c >=( int)' '?( int) c <=(
int)'~': 0){ struct _tagged_string t= Cyc_Core_new_string( 1);({ struct
_tagged_string _temp34= t; char* _temp36= _temp34.curr + 0; if( _temp34.base ==
0? 1:( _temp36 < _temp34.base? 1: _temp36 >= _temp34.last_plus_one)){ _throw(
Null_Exception);}* _temp36= c;}); return t;} else{ struct _tagged_string t= Cyc_Core_new_string(
4); int j= 0;({ struct _tagged_string _temp37= t; char* _temp39= _temp37.curr +(
j ++); if( _temp37.base == 0? 1:( _temp39 < _temp37.base? 1: _temp39 >= _temp37.last_plus_one)){
_throw( Null_Exception);}* _temp39='\\';});({ struct _tagged_string _temp40= t;
char* _temp42= _temp40.curr +( j ++); if( _temp40.base == 0? 1:( _temp42 <
_temp40.base? 1: _temp42 >= _temp40.last_plus_one)){ _throw( Null_Exception);}*
_temp42=( char)(( int)'0' +(( int) c >> 6 & 7));});({ struct _tagged_string
_temp43= t; char* _temp45= _temp43.curr +( j ++); if( _temp43.base == 0? 1:(
_temp45 < _temp43.base? 1: _temp45 >= _temp43.last_plus_one)){ _throw(
Null_Exception);}* _temp45=( char)(( int)'0' +(( int) c >> 3 & 7));});({ struct
_tagged_string _temp46= t; char* _temp48= _temp46.curr +( j ++); if( _temp46.base
== 0? 1:( _temp48 < _temp46.base? 1: _temp48 >= _temp46.last_plus_one)){ _throw(
Null_Exception);}* _temp48=( char)(( int)'0' +(( int) c & 7));}); return t;}}}
static int Cyc_Absynpp_special( struct _tagged_string s){ int sz=( int)(({
struct _tagged_string _temp49= s;( unsigned int)( _temp49.last_plus_one -
_temp49.curr);}) -( unsigned int) 1);{ int i= 0; for( 0; i < sz; i ++){ char c=({
struct _tagged_string _temp50= s; char* _temp52= _temp50.curr + i; if( _temp50.base
== 0? 1:( _temp52 < _temp50.base? 1: _temp52 >= _temp50.last_plus_one)){ _throw(
Null_Exception);}* _temp52;}); if(((( int) c <=( int)' '? 1:( int) c >=( int)'~')?
1:( int) c ==( int)'"')? 1:( int) c ==( int)'\\'){ return 1;}}} return 0;}
struct _tagged_string Cyc_Absynpp_string_escape( struct _tagged_string s){ if( !
Cyc_Absynpp_special( s)){ return s;}{ int n=( int)(({ struct _tagged_string
_temp53= s;( unsigned int)( _temp53.last_plus_one - _temp53.curr);}) -(
unsigned int) 1); if( n > 0?( int)({ struct _tagged_string _temp54= s; char*
_temp56= _temp54.curr + n; if( _temp54.base == 0? 1:( _temp56 < _temp54.base? 1:
_temp56 >= _temp54.last_plus_one)){ _throw( Null_Exception);}* _temp56;}) ==(
int)'\000': 0){ n --;}{ int len= 0;{ int i= 0; for( 0; i <= n; i ++){ char
_temp60=({ struct _tagged_string _temp57= s; char* _temp59= _temp57.curr + i;
if( _temp57.base == 0? 1:( _temp59 < _temp57.base? 1: _temp59 >= _temp57.last_plus_one)){
_throw( Null_Exception);}* _temp59;}); _LL62: if( _temp60 =='\a'){ goto _LL63;}
else{ goto _LL64;} _LL64: if( _temp60 =='\b'){ goto _LL65;} else{ goto _LL66;}
_LL66: if( _temp60 =='\f'){ goto _LL67;} else{ goto _LL68;} _LL68: if( _temp60
=='\n'){ goto _LL69;} else{ goto _LL70;} _LL70: if( _temp60 =='\r'){ goto _LL71;}
else{ goto _LL72;} _LL72: if( _temp60 =='\t'){ goto _LL73;} else{ goto _LL74;}
_LL74: if( _temp60 =='\v'){ goto _LL75;} else{ goto _LL76;} _LL76: if( _temp60
=='\\'){ goto _LL77;} else{ goto _LL78;} _LL78: if( _temp60 =='"'){ goto _LL79;}
else{ goto _LL80;} _LL80: goto _LL81; _LL63: goto _LL65; _LL65: goto _LL67;
_LL67: goto _LL69; _LL69: goto _LL71; _LL71: goto _LL73; _LL73: goto _LL75;
_LL75: goto _LL77; _LL77: goto _LL79; _LL79: len += 2; goto _LL61; _LL81: if((
int) _temp60 >=( int)' '?( int) _temp60 <=( int)'~': 0){ len ++;} else{ len += 4;}
goto _LL61; _LL61:;}}{ struct _tagged_string t= Cyc_Core_new_string( len); int j=
0;{ int i= 0; for( 0; i <= n; i ++){ char _temp85=({ struct _tagged_string
_temp82= s; char* _temp84= _temp82.curr + i; if( _temp82.base == 0? 1:( _temp84
< _temp82.base? 1: _temp84 >= _temp82.last_plus_one)){ _throw( Null_Exception);}*
_temp84;}); _LL87: if( _temp85 =='\a'){ goto _LL88;} else{ goto _LL89;} _LL89:
if( _temp85 =='\b'){ goto _LL90;} else{ goto _LL91;} _LL91: if( _temp85 =='\f'){
goto _LL92;} else{ goto _LL93;} _LL93: if( _temp85 =='\n'){ goto _LL94;} else{
goto _LL95;} _LL95: if( _temp85 =='\r'){ goto _LL96;} else{ goto _LL97;} _LL97:
if( _temp85 =='\t'){ goto _LL98;} else{ goto _LL99;} _LL99: if( _temp85 =='\v'){
goto _LL100;} else{ goto _LL101;} _LL101: if( _temp85 =='\\'){ goto _LL102;}
else{ goto _LL103;} _LL103: if( _temp85 =='"'){ goto _LL104;} else{ goto _LL105;}
_LL105: goto _LL106; _LL88:({ struct _tagged_string _temp107= t; char* _temp109=
_temp107.curr +( j ++); if( _temp107.base == 0? 1:( _temp109 < _temp107.base? 1:
_temp109 >= _temp107.last_plus_one)){ _throw( Null_Exception);}* _temp109='\\';});({
struct _tagged_string _temp110= t; char* _temp112= _temp110.curr +( j ++); if(
_temp110.base == 0? 1:( _temp112 < _temp110.base? 1: _temp112 >= _temp110.last_plus_one)){
_throw( Null_Exception);}* _temp112='a';}); goto _LL86; _LL90:({ struct
_tagged_string _temp113= t; char* _temp115= _temp113.curr +( j ++); if( _temp113.base
== 0? 1:( _temp115 < _temp113.base? 1: _temp115 >= _temp113.last_plus_one)){
_throw( Null_Exception);}* _temp115='\\';});({ struct _tagged_string _temp116= t;
char* _temp118= _temp116.curr +( j ++); if( _temp116.base == 0? 1:( _temp118 <
_temp116.base? 1: _temp118 >= _temp116.last_plus_one)){ _throw( Null_Exception);}*
_temp118='b';}); goto _LL86; _LL92:({ struct _tagged_string _temp119= t; char*
_temp121= _temp119.curr +( j ++); if( _temp119.base == 0? 1:( _temp121 <
_temp119.base? 1: _temp121 >= _temp119.last_plus_one)){ _throw( Null_Exception);}*
_temp121='\\';});({ struct _tagged_string _temp122= t; char* _temp124= _temp122.curr
+( j ++); if( _temp122.base == 0? 1:( _temp124 < _temp122.base? 1: _temp124 >=
_temp122.last_plus_one)){ _throw( Null_Exception);}* _temp124='f';}); goto _LL86;
_LL94:({ struct _tagged_string _temp125= t; char* _temp127= _temp125.curr +( j
++); if( _temp125.base == 0? 1:( _temp127 < _temp125.base? 1: _temp127 >=
_temp125.last_plus_one)){ _throw( Null_Exception);}* _temp127='\\';});({ struct
_tagged_string _temp128= t; char* _temp130= _temp128.curr +( j ++); if( _temp128.base
== 0? 1:( _temp130 < _temp128.base? 1: _temp130 >= _temp128.last_plus_one)){
_throw( Null_Exception);}* _temp130='n';}); goto _LL86; _LL96:({ struct
_tagged_string _temp131= t; char* _temp133= _temp131.curr +( j ++); if( _temp131.base
== 0? 1:( _temp133 < _temp131.base? 1: _temp133 >= _temp131.last_plus_one)){
_throw( Null_Exception);}* _temp133='\\';});({ struct _tagged_string _temp134= t;
char* _temp136= _temp134.curr +( j ++); if( _temp134.base == 0? 1:( _temp136 <
_temp134.base? 1: _temp136 >= _temp134.last_plus_one)){ _throw( Null_Exception);}*
_temp136='r';}); goto _LL86; _LL98:({ struct _tagged_string _temp137= t; char*
_temp139= _temp137.curr +( j ++); if( _temp137.base == 0? 1:( _temp139 <
_temp137.base? 1: _temp139 >= _temp137.last_plus_one)){ _throw( Null_Exception);}*
_temp139='\\';});({ struct _tagged_string _temp140= t; char* _temp142= _temp140.curr
+( j ++); if( _temp140.base == 0? 1:( _temp142 < _temp140.base? 1: _temp142 >=
_temp140.last_plus_one)){ _throw( Null_Exception);}* _temp142='t';}); goto _LL86;
_LL100:({ struct _tagged_string _temp143= t; char* _temp145= _temp143.curr +( j
++); if( _temp143.base == 0? 1:( _temp145 < _temp143.base? 1: _temp145 >=
_temp143.last_plus_one)){ _throw( Null_Exception);}* _temp145='\\';});({ struct
_tagged_string _temp146= t; char* _temp148= _temp146.curr +( j ++); if( _temp146.base
== 0? 1:( _temp148 < _temp146.base? 1: _temp148 >= _temp146.last_plus_one)){
_throw( Null_Exception);}* _temp148='v';}); goto _LL86; _LL102:({ struct
_tagged_string _temp149= t; char* _temp151= _temp149.curr +( j ++); if( _temp149.base
== 0? 1:( _temp151 < _temp149.base? 1: _temp151 >= _temp149.last_plus_one)){
_throw( Null_Exception);}* _temp151='\\';});({ struct _tagged_string _temp152= t;
char* _temp154= _temp152.curr +( j ++); if( _temp152.base == 0? 1:( _temp154 <
_temp152.base? 1: _temp154 >= _temp152.last_plus_one)){ _throw( Null_Exception);}*
_temp154='\\';}); goto _LL86; _LL104:({ struct _tagged_string _temp155= t; char*
_temp157= _temp155.curr +( j ++); if( _temp155.base == 0? 1:( _temp157 <
_temp155.base? 1: _temp157 >= _temp155.last_plus_one)){ _throw( Null_Exception);}*
_temp157='\\';});({ struct _tagged_string _temp158= t; char* _temp160= _temp158.curr
+( j ++); if( _temp158.base == 0? 1:( _temp160 < _temp158.base? 1: _temp160 >=
_temp158.last_plus_one)){ _throw( Null_Exception);}* _temp160='"';}); goto _LL86;
_LL106: if(( int) _temp85 >=( int)' '?( int) _temp85 <=( int)'~': 0){({ struct
_tagged_string _temp161= t; char* _temp163= _temp161.curr +( j ++); if( _temp161.base
== 0? 1:( _temp163 < _temp161.base? 1: _temp163 >= _temp161.last_plus_one)){
_throw( Null_Exception);}* _temp163= _temp85;});} else{({ struct _tagged_string
_temp164= t; char* _temp166= _temp164.curr +( j ++); if( _temp164.base == 0? 1:(
_temp166 < _temp164.base? 1: _temp166 >= _temp164.last_plus_one)){ _throw(
Null_Exception);}* _temp166='\\';});({ struct _tagged_string _temp167= t; char*
_temp169= _temp167.curr +( j ++); if( _temp167.base == 0? 1:( _temp169 <
_temp167.base? 1: _temp169 >= _temp167.last_plus_one)){ _throw( Null_Exception);}*
_temp169=( char)(( int)'0' +(( int) _temp85 >> 6 & 7));});({ struct
_tagged_string _temp170= t; char* _temp172= _temp170.curr +( j ++); if( _temp170.base
== 0? 1:( _temp172 < _temp170.base? 1: _temp172 >= _temp170.last_plus_one)){
_throw( Null_Exception);}* _temp172=( char)(( int)'0' +(( int) _temp85 >> 3 & 7));});({
struct _tagged_string _temp173= t; char* _temp175= _temp173.curr +( j ++); if(
_temp173.base == 0? 1:( _temp175 < _temp173.base? 1: _temp175 >= _temp173.last_plus_one)){
_throw( Null_Exception);}* _temp175=( char)(( int)'0' +(( int) _temp85 & 7));});}
goto _LL86; _LL86:;}} return t;}}}} static struct Cyc_PP_Doc* Cyc_Absynpp_textptr(
struct _tagged_string* s){ return Cyc_PP_text(* s);} static char _temp178[ 9u]="restrict";
static struct _tagged_string Cyc_Absynpp_restrict_string=( struct _tagged_string){
_temp178, _temp178, _temp178 + 9u}; static char _temp181[ 9u]="volatile"; static
struct _tagged_string Cyc_Absynpp_volatile_string=( struct _tagged_string){
_temp181, _temp181, _temp181 + 9u}; static char _temp184[ 6u]="const"; static
struct _tagged_string Cyc_Absynpp_const_string=( struct _tagged_string){
_temp184, _temp184, _temp184 + 6u}; static struct _tagged_string* Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string; static struct _tagged_string* Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string; static struct _tagged_string* Cyc_Absynpp_const_sp=&
Cyc_Absynpp_const_string; struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual*
tq){ struct Cyc_List_List* l= 0; if( tq->q_restrict){ l=({ struct Cyc_List_List*
_temp185=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp185->hd=( void*) Cyc_Absynpp_restrict_sp; _temp185->tl= l; _temp185;});}
if( tq->q_volatile){ l=({ struct Cyc_List_List* _temp186=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp186->hd=( void*) Cyc_Absynpp_volatile_sp;
_temp186->tl= l; _temp186;});} if( tq->q_const){ l=({ struct Cyc_List_List*
_temp187=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp187->hd=( void*) Cyc_Absynpp_const_sp; _temp187->tl= l; _temp187;});}
return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp188=( char*)"";
struct _tagged_string _temp189; _temp189.curr= _temp188; _temp189.base= _temp188;
_temp189.last_plus_one= _temp188 + 1; _temp189;}),( struct _tagged_string)({
char* _temp190=( char*)" "; struct _tagged_string _temp191; _temp191.curr=
_temp190; _temp191.base= _temp190; _temp191.last_plus_one= _temp190 + 2;
_temp191;}),( struct _tagged_string)({ char* _temp192=( char*)" "; struct
_tagged_string _temp193; _temp193.curr= _temp192; _temp193.base= _temp192;
_temp193.last_plus_one= _temp192 + 2; _temp193;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr, l));} struct _tagged_string Cyc_Absynpp_kind2string( void*
k){ void* _temp194= k; _LL196: if(( int) _temp194 == Cyc_Absyn_AnyKind){ goto
_LL197;} else{ goto _LL198;} _LL198: if(( int) _temp194 == Cyc_Absyn_MemKind){
goto _LL199;} else{ goto _LL200;} _LL200: if(( int) _temp194 == Cyc_Absyn_BoxKind){
goto _LL201;} else{ goto _LL202;} _LL202: if(( int) _temp194 == Cyc_Absyn_RgnKind){
goto _LL203;} else{ goto _LL204;} _LL204: if(( int) _temp194 == Cyc_Absyn_EffKind){
goto _LL205;} else{ goto _LL195;} _LL197: return( struct _tagged_string)({ char*
_temp206=( char*)"A"; struct _tagged_string _temp207; _temp207.curr= _temp206;
_temp207.base= _temp206; _temp207.last_plus_one= _temp206 + 2; _temp207;});
_LL199: return( struct _tagged_string)({ char* _temp208=( char*)"M"; struct
_tagged_string _temp209; _temp209.curr= _temp208; _temp209.base= _temp208;
_temp209.last_plus_one= _temp208 + 2; _temp209;}); _LL201: return( struct
_tagged_string)({ char* _temp210=( char*)"B"; struct _tagged_string _temp211;
_temp211.curr= _temp210; _temp211.base= _temp210; _temp211.last_plus_one=
_temp210 + 2; _temp211;}); _LL203: return( struct _tagged_string)({ char*
_temp212=( char*)"R"; struct _tagged_string _temp213; _temp213.curr= _temp212;
_temp213.base= _temp212; _temp213.last_plus_one= _temp212 + 2; _temp213;});
_LL205: return( struct _tagged_string)({ char* _temp214=( char*)"E"; struct
_tagged_string _temp215; _temp215.curr= _temp214; _temp215.base= _temp214;
_temp215.last_plus_one= _temp214 + 2; _temp215;}); _LL195:;} struct
_tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref* c){ void*
_temp216=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( c))->v; void* _temp222; _LL218: if(( unsigned int)
_temp216 > 1u?(( struct _tunion_struct*) _temp216)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL223: _temp222=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp216)->f1;
goto _LL219;} else{ goto _LL220;} _LL220: goto _LL221; _LL219: return Cyc_Absynpp_kind2string(
_temp222); _LL221: return( struct _tagged_string)({ char* _temp224=( char*)"?";
struct _tagged_string _temp225; _temp225.curr= _temp224; _temp225.base= _temp224;
_temp225.last_plus_one= _temp224 + 2; _temp225;}); _LL217:;} struct Cyc_PP_Doc*
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
return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp226=( char*)"<";
struct _tagged_string _temp227; _temp227.curr= _temp226; _temp227.base= _temp226;
_temp227.last_plus_one= _temp226 + 2; _temp227;}),( struct _tagged_string)({
char* _temp228=( char*)">"; struct _tagged_string _temp229; _temp229.curr=
_temp228; _temp229.base= _temp228; _temp229.last_plus_one= _temp228 + 2;
_temp229;}),( struct _tagged_string)({ char* _temp230=( char*)","; struct
_tagged_string _temp231; _temp231.curr= _temp230; _temp231.base= _temp230;
_temp231.last_plus_one= _temp230 + 2; _temp231;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc,
ts));} struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){
void* _temp232=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp240; void* _temp242;
_LL234: if(( unsigned int) _temp232 > 1u?(( struct _tunion_struct*) _temp232)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL241: _temp240=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp232)->f1; if(( int) _temp240 == Cyc_Absyn_BoxKind){ goto _LL235;} else{
goto _LL236;}} else{ goto _LL236;} _LL236: if(( unsigned int) _temp232 > 1u?((
struct _tunion_struct*) _temp232)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL243:
_temp242=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp232)->f1; goto
_LL237;} else{ goto _LL238;} _LL238: goto _LL239; _LL235: return Cyc_Absynpp_textptr(
tv->name); _LL237: return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( tv->name), Cyc_PP_text((
struct _tagged_string)({ char* _temp244=( char*)"::"; struct _tagged_string
_temp245; _temp245.curr= _temp244; _temp245.base= _temp244; _temp245.last_plus_one=
_temp244 + 3; _temp245;})), Cyc_Absynpp_kind2doc( _temp242)); _LL239: return Cyc_Absynpp_cat2(
Cyc_Absynpp_textptr( tv->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp246=( char*)"/*::?*/"; struct _tagged_string _temp247; _temp247.curr=
_temp246; _temp247.base= _temp246; _temp247.last_plus_one= _temp246 + 8;
_temp247;}))); _LL233:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc( struct Cyc_List_List*
tvs){ return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp248=( char*)"<";
struct _tagged_string _temp249; _temp249.curr= _temp248; _temp249.base= _temp248;
_temp249.last_plus_one= _temp248 + 2; _temp249;}),( struct _tagged_string)({
char* _temp250=( char*)">"; struct _tagged_string _temp251; _temp251.curr=
_temp250; _temp251.base= _temp250; _temp251.last_plus_one= _temp250 + 2;
_temp251;}),( struct _tagged_string)({ char* _temp252=( char*)","; struct
_tagged_string _temp253; _temp253.curr= _temp252; _temp253.base= _temp252;
_temp253.last_plus_one= _temp252 + 2; _temp253;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_string* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc(
tvs);} return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp254=(
char*)"<"; struct _tagged_string _temp255; _temp255.curr= _temp254; _temp255.base=
_temp254; _temp255.last_plus_one= _temp254 + 2; _temp255;}),( struct
_tagged_string)({ char* _temp256=( char*)">"; struct _tagged_string _temp257;
_temp257.curr= _temp256; _temp257.base= _temp256; _temp257.last_plus_one=
_temp256 + 2; _temp257;}),( struct _tagged_string)({ char* _temp258=( char*)",";
struct _tagged_string _temp259; _temp259.curr= _temp258; _temp259.base= _temp258;
_temp259.last_plus_one= _temp258 + 2; _temp259;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr,(( struct Cyc_List_List*(*)( struct _tagged_string*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_get_name,
tvs)));} struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc( struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((*
t).f1,(* t).f2, 0);} struct Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List*
ts){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp260=( char*)"(";
struct _tagged_string _temp261; _temp261.curr= _temp260; _temp261.base= _temp260;
_temp261.last_plus_one= _temp260 + 2; _temp261;}),( struct _tagged_string)({
char* _temp262=( char*)")"; struct _tagged_string _temp263; _temp263.curr=
_temp262; _temp263.base= _temp262; _temp263.last_plus_one= _temp262 + 2;
_temp263;}),( struct _tagged_string)({ char* _temp264=( char*)","; struct
_tagged_string _temp265; _temp265.curr= _temp264; _temp265.base= _temp264;
_temp265.last_plus_one= _temp264 + 2; _temp265;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc( void* a){
return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc* Cyc_Absynpp_atts2doc(
struct Cyc_List_List* atts){ if( atts == 0){ return Cyc_PP_nil_doc();} return
Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp266=( char*)" __attribute__";
struct _tagged_string _temp267; _temp267.curr= _temp266; _temp267.base= _temp266;
_temp267.last_plus_one= _temp266 + 15; _temp267;})), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp268=( char*)"(("; struct _tagged_string _temp269;
_temp269.curr= _temp268; _temp269.base= _temp268; _temp269.last_plus_one=
_temp268 + 3; _temp269;}),( struct _tagged_string)({ char* _temp270=( char*)"))";
struct _tagged_string _temp271; _temp271.curr= _temp270; _temp271.base= _temp270;
_temp271.last_plus_one= _temp270 + 3; _temp271;}),( struct _tagged_string)({
char* _temp272=( char*)","; struct _tagged_string _temp273; _temp273.curr=
_temp272; _temp273.base= _temp272; _temp273.last_plus_one= _temp272 + 2;
_temp273;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc, atts)));} int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List* tms){ if( tms == 0){ return 0;}{ void* _temp275=( void*)({
struct Cyc_List_List* _temp274= tms; if( _temp274 == 0){ _throw( Null_Exception);}
_temp274->hd;}); struct Cyc_Absyn_Tqual* _temp281; void* _temp283; void*
_temp285; _LL277: if(( unsigned int) _temp275 > 1u?(( struct _tunion_struct*)
_temp275)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL286: _temp285=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp275)->f1; goto _LL284; _LL284:
_temp283=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp275)->f2; goto
_LL282; _LL282: _temp281=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp275)->f3; goto _LL278;} else{ goto _LL279;} _LL279: goto _LL280; _LL278:
return 1; _LL280: return 0; _LL276:;}} extern struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc(
void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc( struct Cyc_PP_Doc* d, struct
Cyc_List_List* tms){ if( tms == 0){ return d;}{ struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc(
d,({ struct Cyc_List_List* _temp287= tms; if( _temp287 == 0){ _throw(
Null_Exception);} _temp287->tl;})); struct Cyc_PP_Doc* p_rest= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp288=( char*)"("; struct
_tagged_string _temp289; _temp289.curr= _temp288; _temp289.base= _temp288;
_temp289.last_plus_one= _temp288 + 2; _temp289;})), rest, Cyc_PP_text(( struct
_tagged_string)({ char* _temp290=( char*)")"; struct _tagged_string _temp291;
_temp291.curr= _temp290; _temp291.base= _temp290; _temp291.last_plus_one=
_temp290 + 2; _temp291;}))); void* _temp293=( void*)({ struct Cyc_List_List*
_temp292= tms; if( _temp292 == 0){ _throw( Null_Exception);} _temp292->hd;});
struct Cyc_Absyn_Exp* _temp307; void* _temp309; struct Cyc_List_List* _temp311;
struct Cyc_Position_Segment* _temp313; int _temp315; struct Cyc_Position_Segment*
_temp317; struct Cyc_List_List* _temp319; struct Cyc_Absyn_Tqual* _temp321; void*
_temp323; void* _temp325; _LL295: if(( int) _temp293 == Cyc_Absyn_Carray_mod){
goto _LL296;} else{ goto _LL297;} _LL297: if(( unsigned int) _temp293 > 1u?((
struct _tunion_struct*) _temp293)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){
_LL308: _temp307=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp293)->f1; goto _LL298;} else{ goto _LL299;} _LL299: if(( unsigned int)
_temp293 > 1u?(( struct _tunion_struct*) _temp293)->tag == Cyc_Absyn_Function_mod_tag:
0){ _LL310: _temp309=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp293)->f1;
goto _LL300;} else{ goto _LL301;} _LL301: if(( unsigned int) _temp293 > 1u?((
struct _tunion_struct*) _temp293)->tag == Cyc_Absyn_Attributes_mod_tag: 0){
_LL314: _temp313=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp293)->f1; goto _LL312; _LL312: _temp311=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Attributes_mod_struct*) _temp293)->f2; goto _LL302;} else{ goto _LL303;}
_LL303: if(( unsigned int) _temp293 > 1u?(( struct _tunion_struct*) _temp293)->tag
== Cyc_Absyn_TypeParams_mod_tag: 0){ _LL320: _temp319=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp293)->f1; goto _LL318; _LL318:
_temp317=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp293)->f2; goto _LL316; _LL316: _temp315=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp293)->f3; goto _LL304;} else{ goto _LL305;} _LL305: if(( unsigned int)
_temp293 > 1u?(( struct _tunion_struct*) _temp293)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL326: _temp325=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp293)->f1;
goto _LL324; _LL324: _temp323=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp293)->f2; goto _LL322; _LL322: _temp321=( struct Cyc_Absyn_Tqual*)(( struct
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
_temp348; struct Cyc_List_List* _temp350; _LL338: if((( struct _tunion_struct*)
_temp336)->tag == Cyc_Absyn_WithTypes_tag){ _LL347: _temp346=( struct Cyc_List_List*)((
struct Cyc_Absyn_WithTypes_struct*) _temp336)->f1; goto _LL345; _LL345: _temp344=(
int)(( struct Cyc_Absyn_WithTypes_struct*) _temp336)->f2; goto _LL343; _LL343:
_temp342=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp336)->f3;
goto _LL339;} else{ goto _LL340;} _LL340: if((( struct _tunion_struct*) _temp336)->tag
== Cyc_Absyn_NoTypes_tag){ _LL351: _temp350=( struct Cyc_List_List*)(( struct
Cyc_Absyn_NoTypes_struct*) _temp336)->f1; goto _LL349; _LL349: _temp348=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*) _temp336)->f2; goto
_LL341;} else{ goto _LL337;} _LL339: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_funargs2doc(
_temp346, _temp344, _temp342)); _LL341: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp352=( char*)"("; struct _tagged_string
_temp353; _temp353.curr= _temp352; _temp353.base= _temp352; _temp353.last_plus_one=
_temp352 + 2; _temp353;}),( struct _tagged_string)({ char* _temp354=( char*)")";
struct _tagged_string _temp355; _temp355.curr= _temp354; _temp355.base= _temp354;
_temp355.last_plus_one= _temp354 + 2; _temp355;}),( struct _tagged_string)({
char* _temp356=( char*)","; struct _tagged_string _temp357; _temp357.curr=
_temp356; _temp357.base= _temp356; _temp357.last_plus_one= _temp356 + 2;
_temp357;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr,
_temp350))); _LL337:;} _LL302: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List*
_temp358= tms; if( _temp358 == 0){ _throw( Null_Exception);} _temp358->tl;}))){
rest= p_rest;} return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_atts2doc( _temp311));
_LL304: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List* _temp359= tms;
if( _temp359 == 0){ _throw( Null_Exception);} _temp359->tl;}))){ rest= p_rest;}
if( _temp315){ return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_ktvars2doc( _temp319));}
else{ return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_tvars2doc( _temp319));} _LL306: {
struct Cyc_PP_Doc* ptr;{ void* _temp360= _temp325; struct Cyc_Absyn_Exp*
_temp368; struct Cyc_Absyn_Exp* _temp370; _LL362: if(( unsigned int) _temp360 >
1u?(( struct _tunion_struct*) _temp360)->tag == Cyc_Absyn_Nullable_ps_tag: 0){
_LL369: _temp368=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp360)->f1; goto _LL363;} else{ goto _LL364;} _LL364: if(( unsigned int)
_temp360 > 1u?(( struct _tunion_struct*) _temp360)->tag == Cyc_Absyn_NonNullable_ps_tag:
0){ _LL371: _temp370=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp360)->f1; goto _LL365;} else{ goto _LL366;} _LL366: if(( int) _temp360 ==
Cyc_Absyn_TaggedArray_ps){ goto _LL367;} else{ goto _LL361;} _LL363: if( Cyc_Evexp_eval_const_uint_exp(
_temp368) ==( unsigned int) 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char*
_temp372=( char*)"*"; struct _tagged_string _temp373; _temp373.curr= _temp372;
_temp373.base= _temp372; _temp373.last_plus_one= _temp372 + 2; _temp373;}));}
else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp374=( char*)"*"; struct _tagged_string _temp375; _temp375.curr= _temp374;
_temp375.base= _temp374; _temp375.last_plus_one= _temp374 + 2; _temp375;})), Cyc_PP_text((
struct _tagged_string)({ char* _temp376=( char*)"{"; struct _tagged_string
_temp377; _temp377.curr= _temp376; _temp377.base= _temp376; _temp377.last_plus_one=
_temp376 + 2; _temp377;})), Cyc_Absynpp_exp2doc( _temp368), Cyc_PP_text(( struct
_tagged_string)({ char* _temp378=( char*)"}"; struct _tagged_string _temp379;
_temp379.curr= _temp378; _temp379.base= _temp378; _temp379.last_plus_one=
_temp378 + 2; _temp379;})));} goto _LL361; _LL365: if( Cyc_Evexp_eval_const_uint_exp(
_temp370) ==( unsigned int) 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char*
_temp380=( char*)"@"; struct _tagged_string _temp381; _temp381.curr= _temp380;
_temp381.base= _temp380; _temp381.last_plus_one= _temp380 + 2; _temp381;}));}
else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp382=( char*)"@"; struct _tagged_string _temp383; _temp383.curr= _temp382;
_temp383.base= _temp382; _temp383.last_plus_one= _temp382 + 2; _temp383;})), Cyc_PP_text((
struct _tagged_string)({ char* _temp384=( char*)"{"; struct _tagged_string
_temp385; _temp385.curr= _temp384; _temp385.base= _temp384; _temp385.last_plus_one=
_temp384 + 2; _temp385;})), Cyc_Absynpp_exp2doc( _temp370), Cyc_PP_text(( struct
_tagged_string)({ char* _temp386=( char*)"}"; struct _tagged_string _temp387;
_temp387.curr= _temp386; _temp387.base= _temp386; _temp387.last_plus_one=
_temp386 + 2; _temp387;})));} goto _LL361; _LL367: ptr= Cyc_PP_text(( struct
_tagged_string)({ char* _temp388=( char*)"?"; struct _tagged_string _temp389;
_temp389.curr= _temp388; _temp389.base= _temp388; _temp389.last_plus_one=
_temp388 + 2; _temp389;})); goto _LL361; _LL361:;}{ void* _temp390= Cyc_Tcutil_compress(
_temp323); _LL392: if(( int) _temp390 == Cyc_Absyn_HeapRgn){ goto _LL393;} else{
goto _LL394;} _LL394: goto _LL395; _LL393: return Cyc_Absynpp_cat2( ptr, rest);
_LL395: return Cyc_Absynpp_cat4( ptr, Cyc_Absynpp_ntyp2doc( _temp323), Cyc_PP_text((
struct _tagged_string)({ char* _temp396=( char*)" "; struct _tagged_string
_temp397; _temp397.curr= _temp396; _temp397.base= _temp396; _temp397.last_plus_one=
_temp396 + 2; _temp397;})), rest); _LL391:;}} _LL294:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc(
void* t){ void* _temp398= Cyc_Tcutil_compress( t); _LL400: if(( int) _temp398 ==
Cyc_Absyn_HeapRgn){ goto _LL401;} else{ goto _LL402;} _LL402: goto _LL403;
_LL401: return Cyc_PP_text(( struct _tagged_string)({ char* _temp404=( char*)"`H";
struct _tagged_string _temp405; _temp405.curr= _temp404; _temp405.base= _temp404;
_temp405.last_plus_one= _temp404 + 3; _temp405;})); _LL403: return Cyc_Absynpp_ntyp2doc(
t); _LL399:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
regions, struct Cyc_List_List** effects, void* t){ void* _temp406= Cyc_Tcutil_compress(
t); void* _temp414; struct Cyc_List_List* _temp416; _LL408: if(( unsigned int)
_temp406 > 4u?(( struct _tunion_struct*) _temp406)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL415: _temp414=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp406)->f1;
goto _LL409;} else{ goto _LL410;} _LL410: if(( unsigned int) _temp406 > 4u?((
struct _tunion_struct*) _temp406)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL417:
_temp416=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp406)->f1;
goto _LL411;} else{ goto _LL412;} _LL412: goto _LL413; _LL409:* regions=({
struct Cyc_List_List* _temp418=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp418->hd=( void*) Cyc_Absynpp_rgn2doc( _temp414);
_temp418->tl=* regions; _temp418;}); goto _LL407; _LL411: for( 0; _temp416 != 0;
_temp416=({ struct Cyc_List_List* _temp419= _temp416; if( _temp419 == 0){ _throw(
Null_Exception);} _temp419->tl;})){ Cyc_Absynpp_effects2docs( regions, effects,(
void*)({ struct Cyc_List_List* _temp420= _temp416; if( _temp420 == 0){ _throw(
Null_Exception);} _temp420->hd;}));} goto _LL407; _LL413:* effects=({ struct Cyc_List_List*
_temp421=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp421->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp421->tl=* effects; _temp421;});
goto _LL407; _LL407:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp422=( char*)""; struct _tagged_string
_temp423; _temp423.curr= _temp422; _temp423.base= _temp422; _temp423.last_plus_one=
_temp422 + 1; _temp423;}),( struct _tagged_string)({ char* _temp424=( char*)"";
struct _tagged_string _temp425; _temp425.curr= _temp424; _temp425.base= _temp424;
_temp425.last_plus_one= _temp424 + 1; _temp425;}),( struct _tagged_string)({
char* _temp426=( char*)"+"; struct _tagged_string _temp427; _temp427.curr=
_temp426; _temp427.base= _temp426; _temp427.last_plus_one= _temp426 + 2;
_temp427;}), effects);} else{ struct Cyc_PP_Doc* doc1= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp428=( char*)"{"; struct _tagged_string
_temp429; _temp429.curr= _temp428; _temp429.base= _temp428; _temp429.last_plus_one=
_temp428 + 2; _temp429;}),( struct _tagged_string)({ char* _temp430=( char*)"}";
struct _tagged_string _temp431; _temp431.curr= _temp430; _temp431.base= _temp430;
_temp431.last_plus_one= _temp430 + 2; _temp431;}),( struct _tagged_string)({
char* _temp432=( char*)","; struct _tagged_string _temp433; _temp433.curr=
_temp432; _temp433.base= _temp432; _temp433.last_plus_one= _temp432 + 2;
_temp433;}), regions); return Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp434=( char*)""; struct _tagged_string _temp435; _temp435.curr= _temp434;
_temp435.base= _temp434; _temp435.last_plus_one= _temp434 + 1; _temp435;}),(
struct _tagged_string)({ char* _temp436=( char*)""; struct _tagged_string
_temp437; _temp437.curr= _temp436; _temp437.base= _temp436; _temp437.last_plus_one=
_temp436 + 1; _temp437;}),( struct _tagged_string)({ char* _temp438=( char*)"+";
struct _tagged_string _temp439; _temp439.curr= _temp438; _temp439.base= _temp438;
_temp439.last_plus_one= _temp438 + 2; _temp439;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( effects,({
struct Cyc_List_List* _temp440=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp440->hd=( void*) doc1; _temp440->tl= 0; _temp440;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc* s;{ void*
_temp441= t; struct Cyc_Absyn_Exp* _temp487; struct Cyc_Absyn_Tqual* _temp489;
void* _temp491; struct Cyc_Absyn_FnInfo _temp493; struct Cyc_Absyn_PtrInfo
_temp495; int _temp497; struct Cyc_Core_Opt* _temp499; void* _temp501; struct
Cyc_Absyn_Tvar* _temp503; struct Cyc_Absyn_TunionInfo _temp505; struct Cyc_Absyn_Tuniondecl*
_temp507; void* _temp509; struct Cyc_List_List* _temp511; struct _tuple0*
_temp513; struct Cyc_Absyn_XTunionInfo _temp515; struct Cyc_Absyn_XTuniondecl*
_temp517; void* _temp519; struct _tuple0* _temp521; struct Cyc_Absyn_TunionFieldInfo
_temp523; struct Cyc_Absyn_Tunionfield* _temp525; struct Cyc_Absyn_Tuniondecl*
_temp527; struct _tuple0* _temp529; struct Cyc_List_List* _temp531; struct
_tuple0* _temp533; struct Cyc_Absyn_XTunionFieldInfo _temp535; struct Cyc_Absyn_Tunionfield*
_temp537; struct Cyc_Absyn_XTuniondecl* _temp539; struct _tuple0* _temp541;
struct _tuple0* _temp543; void* _temp545; void* _temp547; struct Cyc_List_List*
_temp549; struct Cyc_Absyn_Structdecl** _temp551; struct Cyc_List_List* _temp553;
struct _tuple0* _temp555; struct Cyc_Absyn_Uniondecl** _temp557; struct Cyc_List_List*
_temp559; struct _tuple0* _temp561; struct Cyc_Absyn_Enumdecl* _temp563; struct
_tuple0* _temp565; struct Cyc_Core_Opt* _temp567; struct Cyc_List_List* _temp569;
struct _tuple0* _temp571; void* _temp573; void* _temp575; struct Cyc_List_List*
_temp577; _LL443: if(( unsigned int) _temp441 > 4u?(( struct _tunion_struct*)
_temp441)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL492: _temp491=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp441)->f1; goto _LL490; _LL490: _temp489=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp441)->f2;
goto _LL488; _LL488: _temp487=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp441)->f3; goto _LL444;} else{ goto _LL445;} _LL445: if(( unsigned int)
_temp441 > 4u?(( struct _tunion_struct*) _temp441)->tag == Cyc_Absyn_FnType_tag:
0){ _LL494: _temp493=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp441)->f1; goto _LL446;} else{ goto _LL447;} _LL447: if(( unsigned int)
_temp441 > 4u?(( struct _tunion_struct*) _temp441)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL496: _temp495=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp441)->f1; goto _LL448;} else{ goto _LL449;} _LL449: if(( int) _temp441 ==
Cyc_Absyn_VoidType){ goto _LL450;} else{ goto _LL451;} _LL451: if(( unsigned int)
_temp441 > 4u?(( struct _tunion_struct*) _temp441)->tag == Cyc_Absyn_Evar_tag: 0){
_LL502: _temp501=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp441)->f1; goto
_LL500; _LL500: _temp499=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp441)->f2; goto _LL498; _LL498: _temp497=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp441)->f3; goto _LL452;} else{ goto _LL453;} _LL453: if(( unsigned int)
_temp441 > 4u?(( struct _tunion_struct*) _temp441)->tag == Cyc_Absyn_VarType_tag:
0){ _LL504: _temp503=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp441)->f1; goto _LL454;} else{ goto _LL455;} _LL455: if(( unsigned int)
_temp441 > 4u?(( struct _tunion_struct*) _temp441)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL506: _temp505=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp441)->f1; _LL514: _temp513=( struct _tuple0*) _temp505.name; goto _LL512;
_LL512: _temp511=( struct Cyc_List_List*) _temp505.targs; goto _LL510; _LL510:
_temp509=( void*) _temp505.rgn; goto _LL508; _LL508: _temp507=( struct Cyc_Absyn_Tuniondecl*)
_temp505.tud; goto _LL456;} else{ goto _LL457;} _LL457: if(( unsigned int)
_temp441 > 4u?(( struct _tunion_struct*) _temp441)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL516: _temp515=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp441)->f1; _LL522: _temp521=( struct _tuple0*) _temp515.name; goto _LL520;
_LL520: _temp519=( void*) _temp515.rgn; goto _LL518; _LL518: _temp517=( struct
Cyc_Absyn_XTuniondecl*) _temp515.xtud; goto _LL458;} else{ goto _LL459;} _LL459:
if(( unsigned int) _temp441 > 4u?(( struct _tunion_struct*) _temp441)->tag ==
Cyc_Absyn_TunionFieldType_tag: 0){ _LL524: _temp523=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp441)->f1; _LL534: _temp533=(
struct _tuple0*) _temp523.name; goto _LL532; _LL532: _temp531=( struct Cyc_List_List*)
_temp523.targs; goto _LL530; _LL530: _temp529=( struct _tuple0*) _temp523.fname;
goto _LL528; _LL528: _temp527=( struct Cyc_Absyn_Tuniondecl*) _temp523.tud; goto
_LL526; _LL526: _temp525=( struct Cyc_Absyn_Tunionfield*) _temp523.tufd; goto
_LL460;} else{ goto _LL461;} _LL461: if(( unsigned int) _temp441 > 4u?(( struct
_tunion_struct*) _temp441)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL536:
_temp535=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp441)->f1; _LL544: _temp543=( struct _tuple0*) _temp535.name; goto _LL542;
_LL542: _temp541=( struct _tuple0*) _temp535.fname; goto _LL540; _LL540:
_temp539=( struct Cyc_Absyn_XTuniondecl*) _temp535.xtud; goto _LL538; _LL538:
_temp537=( struct Cyc_Absyn_Tunionfield*) _temp535.xtufd; goto _LL462;} else{
goto _LL463;} _LL463: if(( unsigned int) _temp441 > 4u?(( struct _tunion_struct*)
_temp441)->tag == Cyc_Absyn_IntType_tag: 0){ _LL548: _temp547=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp441)->f1; goto _LL546; _LL546: _temp545=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp441)->f2; goto _LL464;} else{ goto _LL465;}
_LL465: if(( int) _temp441 == Cyc_Absyn_FloatType){ goto _LL466;} else{ goto
_LL467;} _LL467: if(( int) _temp441 == Cyc_Absyn_DoubleType){ goto _LL468;}
else{ goto _LL469;} _LL469: if(( unsigned int) _temp441 > 4u?(( struct
_tunion_struct*) _temp441)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL550: _temp549=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp441)->f1; goto
_LL470;} else{ goto _LL471;} _LL471: if(( unsigned int) _temp441 > 4u?(( struct
_tunion_struct*) _temp441)->tag == Cyc_Absyn_StructType_tag: 0){ _LL556:
_temp555=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp441)->f1;
goto _LL554; _LL554: _temp553=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp441)->f2; goto _LL552; _LL552: _temp551=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp441)->f3; goto _LL472;} else{ goto
_LL473;} _LL473: if(( unsigned int) _temp441 > 4u?(( struct _tunion_struct*)
_temp441)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL562: _temp561=( struct _tuple0*)((
struct Cyc_Absyn_UnionType_struct*) _temp441)->f1; goto _LL560; _LL560: _temp559=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp441)->f2; goto
_LL558; _LL558: _temp557=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp441)->f3; goto _LL474;} else{ goto _LL475;} _LL475: if(( unsigned int)
_temp441 > 4u?(( struct _tunion_struct*) _temp441)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL566: _temp565=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*)
_temp441)->f1; goto _LL564; _LL564: _temp563=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp441)->f2; goto _LL476;} else{ goto
_LL477;} _LL477: if(( unsigned int) _temp441 > 4u?(( struct _tunion_struct*)
_temp441)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL572: _temp571=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp441)->f1; goto _LL570;
_LL570: _temp569=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp441)->f2; goto _LL568; _LL568: _temp567=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp441)->f3; goto _LL478;} else{ goto _LL479;}
_LL479: if(( unsigned int) _temp441 > 4u?(( struct _tunion_struct*) _temp441)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL574: _temp573=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp441)->f1; goto _LL480;} else{ goto _LL481;} _LL481: if(( int) _temp441 ==
Cyc_Absyn_HeapRgn){ goto _LL482;} else{ goto _LL483;} _LL483: if(( unsigned int)
_temp441 > 4u?(( struct _tunion_struct*) _temp441)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL576: _temp575=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp441)->f1;
goto _LL484;} else{ goto _LL485;} _LL485: if(( unsigned int) _temp441 > 4u?((
struct _tunion_struct*) _temp441)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL578:
_temp577=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp441)->f1;
goto _LL486;} else{ goto _LL442;} _LL444: return Cyc_PP_text(( struct
_tagged_string)({ char* _temp579=( char*)"[[[array]]]"; struct _tagged_string
_temp580; _temp580.curr= _temp579; _temp580.base= _temp579; _temp580.last_plus_one=
_temp579 + 12; _temp580;})); _LL446: return Cyc_PP_nil_doc(); _LL448: return Cyc_PP_nil_doc();
_LL450: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp581=( char*)"void";
struct _tagged_string _temp582; _temp582.curr= _temp581; _temp582.base= _temp581;
_temp582.last_plus_one= _temp581 + 5; _temp582;})); goto _LL442; _LL452: if(
_temp499 != 0){ return Cyc_Absynpp_ntyp2doc(( void*)({ struct Cyc_Core_Opt*
_temp583= _temp499; if( _temp583 == 0){ _throw( Null_Exception);} _temp583->v;}));}
else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct _tagged_string)({ char* _temp584=(
char*)"%"; struct _tagged_string _temp585; _temp585.curr= _temp584; _temp585.base=
_temp584; _temp585.last_plus_one= _temp584 + 2; _temp585;})), Cyc_PP_text((
struct _tagged_string)({ char* _temp586=( char*)"("; struct _tagged_string
_temp587; _temp587.curr= _temp586; _temp587.base= _temp586; _temp587.last_plus_one=
_temp586 + 2; _temp587;})), Cyc_PP_text( xprintf("%d", _temp497)), Cyc_PP_text((
struct _tagged_string)({ char* _temp588=( char*)")::"; struct _tagged_string
_temp589; _temp589.curr= _temp588; _temp589.base= _temp588; _temp589.last_plus_one=
_temp588 + 4; _temp589;})), Cyc_Absynpp_kind2doc( _temp501));} goto _LL442;
_LL454: s= Cyc_Absynpp_textptr( _temp503->name); if( Cyc_Absynpp_print_all_kinds){
s= Cyc_Absynpp_cat3( s, Cyc_PP_text(( struct _tagged_string)({ char* _temp590=(
char*)"::"; struct _tagged_string _temp591; _temp591.curr= _temp590; _temp591.base=
_temp590; _temp591.last_plus_one= _temp590 + 3; _temp591;})), Cyc_Absynpp_ckind2doc(
_temp503->kind));} if( ! Cyc_Absynpp_dont_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_temp503): 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp592=( char*)"_ /* "; struct _tagged_string _temp593; _temp593.curr=
_temp592; _temp593.base= _temp592; _temp593.last_plus_one= _temp592 + 6;
_temp593;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp594=( char*)" */";
struct _tagged_string _temp595; _temp595.curr= _temp594; _temp595.base= _temp594;
_temp595.last_plus_one= _temp594 + 4; _temp595;})));} goto _LL442; _LL456:{ void*
_temp596= Cyc_Tcutil_compress( _temp509); _LL598: if(( int) _temp596 == Cyc_Absyn_HeapRgn){
goto _LL599;} else{ goto _LL600;} _LL600: goto _LL601; _LL599: s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp602=( char*)"tunion "; struct
_tagged_string _temp603; _temp603.curr= _temp602; _temp603.base= _temp602;
_temp603.last_plus_one= _temp602 + 8; _temp603;})), Cyc_Absynpp_qvar2doc(
_temp513), Cyc_Absynpp_tps2doc( _temp511)); goto _LL597; _LL601: s= Cyc_Absynpp_cat5(
Cyc_PP_text(( struct _tagged_string)({ char* _temp604=( char*)"tunion "; struct
_tagged_string _temp605; _temp605.curr= _temp604; _temp605.base= _temp604;
_temp605.last_plus_one= _temp604 + 8; _temp605;})), Cyc_Absynpp_typ2doc(
_temp509), Cyc_PP_text(( struct _tagged_string)({ char* _temp606=( char*)" ";
struct _tagged_string _temp607; _temp607.curr= _temp606; _temp607.base= _temp606;
_temp607.last_plus_one= _temp606 + 2; _temp607;})), Cyc_Absynpp_qvar2doc(
_temp513), Cyc_Absynpp_tps2doc( _temp511)); goto _LL597; _LL597:;} goto _LL442;
_LL458:{ void* _temp608= Cyc_Tcutil_compress( _temp519); _LL610: if(( int)
_temp608 == Cyc_Absyn_HeapRgn){ goto _LL611;} else{ goto _LL612;} _LL612: goto
_LL613; _LL611: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp614=( char*)"xtunion "; struct _tagged_string _temp615; _temp615.curr=
_temp614; _temp615.base= _temp614; _temp615.last_plus_one= _temp614 + 9;
_temp615;})), Cyc_Absynpp_qvar2doc( _temp521)); goto _LL609; _LL613: s= Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp616=( char*)"xtunion "; struct
_tagged_string _temp617; _temp617.curr= _temp616; _temp617.base= _temp616;
_temp617.last_plus_one= _temp616 + 9; _temp617;})), Cyc_Absynpp_typ2doc(
_temp519), Cyc_PP_text(( struct _tagged_string)({ char* _temp618=( char*)" ";
struct _tagged_string _temp619; _temp619.curr= _temp618; _temp619.base= _temp618;
_temp619.last_plus_one= _temp618 + 2; _temp619;})), Cyc_Absynpp_qvar2doc(
_temp521)); goto _LL609; _LL609:;} goto _LL442; _LL460: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp620=( char*)"tunion "; struct _tagged_string
_temp621; _temp621.curr= _temp620; _temp621.base= _temp620; _temp621.last_plus_one=
_temp620 + 8; _temp621;})), Cyc_Absynpp_qvar2doc( _temp533), Cyc_PP_text((
struct _tagged_string)({ char* _temp622=( char*)"."; struct _tagged_string
_temp623; _temp623.curr= _temp622; _temp623.base= _temp622; _temp623.last_plus_one=
_temp622 + 2; _temp623;})), Cyc_Absynpp_textptr((* _temp529).f2), Cyc_Absynpp_tps2doc(
_temp531)); goto _LL442; _LL462: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct
_tagged_string)({ char* _temp624=( char*)"xtunion "; struct _tagged_string
_temp625; _temp625.curr= _temp624; _temp625.base= _temp624; _temp625.last_plus_one=
_temp624 + 9; _temp625;})), Cyc_Absynpp_qvar2doc( _temp543), Cyc_PP_text((
struct _tagged_string)({ char* _temp626=( char*)"."; struct _tagged_string
_temp627; _temp627.curr= _temp626; _temp627.base= _temp626; _temp627.last_plus_one=
_temp626 + 2; _temp627;})), Cyc_Absynpp_textptr((* _temp541).f2)); goto _LL442;
_LL464: { struct _tagged_string sns; struct _tagged_string ts;{ void* _temp628=
_temp547; _LL630: if(( int) _temp628 == Cyc_Absyn_Signed){ goto _LL631;} else{
goto _LL632;} _LL632: if(( int) _temp628 == Cyc_Absyn_Unsigned){ goto _LL633;}
else{ goto _LL629;} _LL631: sns=( struct _tagged_string)({ char* _temp634=( char*)"";
struct _tagged_string _temp635; _temp635.curr= _temp634; _temp635.base= _temp634;
_temp635.last_plus_one= _temp634 + 1; _temp635;}); goto _LL629; _LL633: sns=(
struct _tagged_string)({ char* _temp636=( char*)"unsigned "; struct
_tagged_string _temp637; _temp637.curr= _temp636; _temp637.base= _temp636;
_temp637.last_plus_one= _temp636 + 10; _temp637;}); goto _LL629; _LL629:;}{ void*
_temp638= _temp545; _LL640: if(( int) _temp638 == Cyc_Absyn_B1){ goto _LL641;}
else{ goto _LL642;} _LL642: if(( int) _temp638 == Cyc_Absyn_B2){ goto _LL643;}
else{ goto _LL644;} _LL644: if(( int) _temp638 == Cyc_Absyn_B4){ goto _LL645;}
else{ goto _LL646;} _LL646: if(( int) _temp638 == Cyc_Absyn_B8){ goto _LL647;}
else{ goto _LL639;} _LL641:{ void* _temp648= _temp547; _LL650: if(( int)
_temp648 == Cyc_Absyn_Signed){ goto _LL651;} else{ goto _LL652;} _LL652: if((
int) _temp648 == Cyc_Absyn_Unsigned){ goto _LL653;} else{ goto _LL649;} _LL651:
sns=( struct _tagged_string)({ char* _temp654=( char*)"signed "; struct
_tagged_string _temp655; _temp655.curr= _temp654; _temp655.base= _temp654;
_temp655.last_plus_one= _temp654 + 8; _temp655;}); goto _LL649; _LL653: sns=(
struct _tagged_string)({ char* _temp656=( char*)""; struct _tagged_string
_temp657; _temp657.curr= _temp656; _temp657.base= _temp656; _temp657.last_plus_one=
_temp656 + 1; _temp657;}); goto _LL649; _LL649:;} ts=( struct _tagged_string)({
char* _temp658=( char*)"char"; struct _tagged_string _temp659; _temp659.curr=
_temp658; _temp659.base= _temp658; _temp659.last_plus_one= _temp658 + 5;
_temp659;}); goto _LL639; _LL643: ts=( struct _tagged_string)({ char* _temp660=(
char*)"short"; struct _tagged_string _temp661; _temp661.curr= _temp660; _temp661.base=
_temp660; _temp661.last_plus_one= _temp660 + 6; _temp661;}); goto _LL639; _LL645:
ts=( struct _tagged_string)({ char* _temp662=( char*)"int"; struct
_tagged_string _temp663; _temp663.curr= _temp662; _temp663.base= _temp662;
_temp663.last_plus_one= _temp662 + 4; _temp663;}); goto _LL639; _LL647: ts=(
struct _tagged_string)({ char* _temp664=( char*)"long long"; struct
_tagged_string _temp665; _temp665.curr= _temp664; _temp665.base= _temp664;
_temp665.last_plus_one= _temp664 + 10; _temp665;}); goto _LL639; _LL639:;} s=
Cyc_PP_text(({ struct _tagged_string _temp666= sns; struct _tagged_string
_temp667= ts; xprintf("%.*s%.*s", _temp666.last_plus_one - _temp666.curr,
_temp666.curr, _temp667.last_plus_one - _temp667.curr, _temp667.curr);})); goto
_LL442;} _LL466: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp668=( char*)"float";
struct _tagged_string _temp669; _temp669.curr= _temp668; _temp669.base= _temp668;
_temp669.last_plus_one= _temp668 + 6; _temp669;})); goto _LL442; _LL468: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp670=( char*)"double"; struct _tagged_string
_temp671; _temp671.curr= _temp670; _temp671.base= _temp670; _temp671.last_plus_one=
_temp670 + 7; _temp671;})); goto _LL442; _LL470: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp672=( char*)"$"; struct _tagged_string
_temp673; _temp673.curr= _temp672; _temp673.base= _temp672; _temp673.last_plus_one=
_temp672 + 2; _temp673;})), Cyc_Absynpp_args2doc( _temp549)); goto _LL442;
_LL472: if( _temp555 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp674=( char*)"struct "; struct _tagged_string
_temp675; _temp675.curr= _temp674; _temp675.base= _temp674; _temp675.last_plus_one=
_temp674 + 8; _temp675;})), Cyc_Absynpp_tps2doc( _temp553));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp676=( char*)"struct "; struct
_tagged_string _temp677; _temp677.curr= _temp676; _temp677.base= _temp676;
_temp677.last_plus_one= _temp676 + 8; _temp677;})), _temp555 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp678= _temp555;
if( _temp678 == 0){ _throw( Null_Exception);} _temp678;})), Cyc_Absynpp_tps2doc(
_temp553));} goto _LL442; _LL474: if( _temp561 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp679=( char*)"union "; struct _tagged_string
_temp680; _temp680.curr= _temp679; _temp680.base= _temp679; _temp680.last_plus_one=
_temp679 + 7; _temp680;})), Cyc_Absynpp_tps2doc( _temp559));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp681=( char*)"union "; struct
_tagged_string _temp682; _temp682.curr= _temp681; _temp682.base= _temp681;
_temp682.last_plus_one= _temp681 + 7; _temp682;})), _temp561 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp683= _temp561;
if( _temp683 == 0){ _throw( Null_Exception);} _temp683;})), Cyc_Absynpp_tps2doc(
_temp559));} goto _LL442; _LL476: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp684=( char*)"enum "; struct _tagged_string _temp685;
_temp685.curr= _temp684; _temp685.base= _temp684; _temp685.last_plus_one=
_temp684 + 6; _temp685;})), Cyc_Absynpp_qvar2doc( _temp565)); goto _LL442;
_LL478: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp571), Cyc_Absynpp_tps2doc(
_temp569)); goto _LL442; _LL480: s= Cyc_Absynpp_rgn2doc( _temp573); goto _LL442;
_LL482: s= Cyc_Absynpp_rgn2doc( t); goto _LL442; _LL484: s= Cyc_Absynpp_eff2doc(
t); goto _LL442; _LL486: s= Cyc_Absynpp_eff2doc( t); goto _LL442; _LL442:;}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct Cyc_Core_Opt* vo){
return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct _tagged_string*)({
struct Cyc_Core_Opt* _temp686= vo; if( _temp686 == 0){ _throw( Null_Exception);}
_temp686->v;})));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){
struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt* _temp687=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp687->v=(
void*) Cyc_PP_text(*(( struct _tagged_string*)({ struct Cyc_Core_Opt* _temp688=(*
t).f1; if( _temp688 == 0){ _throw( Null_Exception);} _temp688->v;}))); _temp687;});
return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt){ if(
effopt == 0){ if( varargs){ return Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp689=( char*)"("; struct _tagged_string _temp690; _temp690.curr=
_temp689; _temp690.base= _temp689; _temp690.last_plus_one= _temp689 + 2;
_temp690;}),( struct _tagged_string)({ char* _temp691=( char*)")"; struct
_tagged_string _temp692; _temp692.curr= _temp691; _temp692.base= _temp691;
_temp692.last_plus_one= _temp691 + 2; _temp692;}),( struct _tagged_string)({
char* _temp693=( char*)","; struct _tagged_string _temp694; _temp694.curr=
_temp693; _temp694.base= _temp693; _temp694.last_plus_one= _temp693 + 2;
_temp694;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)((( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args),({ struct Cyc_List_List* _temp695=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp695->hd=( void*) Cyc_PP_text(( struct
_tagged_string)({ char* _temp696=( char*)"..."; struct _tagged_string _temp697;
_temp697.curr= _temp696; _temp697.base= _temp696; _temp697.last_plus_one=
_temp696 + 4; _temp697;})); _temp695->tl= 0; _temp695;})));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp698=( char*)"("; struct _tagged_string
_temp699; _temp699.curr= _temp698; _temp699.base= _temp698; _temp699.last_plus_one=
_temp698 + 2; _temp699;}),( struct _tagged_string)({ char* _temp700=( char*)")";
struct _tagged_string _temp701; _temp701.curr= _temp700; _temp701.base= _temp700;
_temp701.last_plus_one= _temp700 + 2; _temp701;}),( struct _tagged_string)({
char* _temp702=( char*)","; struct _tagged_string _temp703; _temp703.curr=
_temp702; _temp703.base= _temp702; _temp703.last_plus_one= _temp702 + 2;
_temp703;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));}} else{
if( varargs){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp704=(
char*)"("; struct _tagged_string _temp705; _temp705.curr= _temp704; _temp705.base=
_temp704; _temp705.last_plus_one= _temp704 + 2; _temp705;}),( struct
_tagged_string)({ char* _temp706=( char*)")"; struct _tagged_string _temp707;
_temp707.curr= _temp706; _temp707.base= _temp706; _temp707.last_plus_one=
_temp706 + 2; _temp707;}),( struct _tagged_string)({ char* _temp708=( char*)";";
struct _tagged_string _temp709; _temp709.curr= _temp708; _temp709.base= _temp708;
_temp709.last_plus_one= _temp708 + 2; _temp709;}),({ struct Cyc_List_List*
_temp710=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp710->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp713=( char*)""; struct _tagged_string _temp714; _temp714.curr= _temp713;
_temp714.base= _temp713; _temp714.last_plus_one= _temp713 + 1; _temp714;}),(
struct _tagged_string)({ char* _temp715=( char*)""; struct _tagged_string
_temp716; _temp716.curr= _temp715; _temp716.base= _temp715; _temp716.last_plus_one=
_temp715 + 1; _temp716;}),( struct _tagged_string)({ char* _temp717=( char*)",";
struct _tagged_string _temp718; _temp718.curr= _temp717; _temp718.base= _temp717;
_temp718.last_plus_one= _temp717 + 2; _temp718;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args),({ struct Cyc_List_List*
_temp719=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp719->hd=( void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp720=(
char*)"..."; struct _tagged_string _temp721; _temp721.curr= _temp720; _temp721.base=
_temp720; _temp721.last_plus_one= _temp720 + 4; _temp721;})); _temp719->tl= 0;
_temp719;}))); _temp710->tl=({ struct Cyc_List_List* _temp711=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp711->hd=( void*) Cyc_Absynpp_eff2doc((
void*)({ struct Cyc_Core_Opt* _temp712= effopt; if( _temp712 == 0){ _throw(
Null_Exception);} _temp712->v;})); _temp711->tl= 0; _temp711;}); _temp710;}));}
else{ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp722=( char*)"(";
struct _tagged_string _temp723; _temp723.curr= _temp722; _temp723.base= _temp722;
_temp723.last_plus_one= _temp722 + 2; _temp723;}),( struct _tagged_string)({
char* _temp724=( char*)")"; struct _tagged_string _temp725; _temp725.curr=
_temp724; _temp725.base= _temp724; _temp725.last_plus_one= _temp724 + 2;
_temp725;}),( struct _tagged_string)({ char* _temp726=( char*)";"; struct
_tagged_string _temp727; _temp727.curr= _temp726; _temp727.base= _temp726;
_temp727.last_plus_one= _temp726 + 2; _temp727;}),({ struct Cyc_List_List*
_temp728=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp728->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp731=( char*)""; struct _tagged_string _temp732; _temp732.curr= _temp731;
_temp732.base= _temp731; _temp732.last_plus_one= _temp731 + 1; _temp732;}),(
struct _tagged_string)({ char* _temp733=( char*)""; struct _tagged_string
_temp734; _temp734.curr= _temp733; _temp734.base= _temp733; _temp734.last_plus_one=
_temp733 + 1; _temp734;}),( struct _tagged_string)({ char* _temp735=( char*)",";
struct _tagged_string _temp736; _temp736.curr= _temp735; _temp736.base= _temp735;
_temp736.last_plus_one= _temp735 + 2; _temp736;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_funarg2doc, args)); _temp728->tl=({ struct Cyc_List_List* _temp729=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp729->hd=(
void*) Cyc_Absynpp_eff2doc(( void*)({ struct Cyc_Core_Opt* _temp730= effopt; if(
_temp730 == 0){ _throw( Null_Exception);} _temp730->v;})); _temp729->tl= 0;
_temp729;}); _temp728;}));}}} struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct
_tuple3* arg){ return({ struct _tuple1* _temp737=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp737->f1=({ struct Cyc_Core_Opt* _temp738=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp738->v=( void*)(*
arg).f1; _temp738;}); _temp737->f2=(* arg).f2; _temp737->f3=(* arg).f3; _temp737;});}
struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct _tagged_string* v){ return Cyc_PP_text(*
v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
prefix= 0;{ void* _temp739=(* q).f1; struct Cyc_List_List* _temp751; struct Cyc_List_List*
_temp753; struct Cyc_List_List* _temp755; struct Cyc_List_List* _temp757; _LL741:
if(( int) _temp739 == Cyc_Absyn_Loc_n){ goto _LL742;} else{ goto _LL743;} _LL743:
if(( unsigned int) _temp739 > 1u?(( struct _tunion_struct*) _temp739)->tag ==
Cyc_Absyn_Rel_n_tag: 0){ _LL752: _temp751=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp739)->f1; if( _temp751 == 0){ goto _LL744;} else{ goto _LL745;}} else{ goto
_LL745;} _LL745: if(( unsigned int) _temp739 > 1u?(( struct _tunion_struct*)
_temp739)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL754: _temp753=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp739)->f1; if( _temp753 == 0){ goto _LL746;}
else{ goto _LL747;}} else{ goto _LL747;} _LL747: if(( unsigned int) _temp739 > 1u?((
struct _tunion_struct*) _temp739)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL756:
_temp755=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp739)->f1;
goto _LL748;} else{ goto _LL749;} _LL749: if(( unsigned int) _temp739 > 1u?((
struct _tunion_struct*) _temp739)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL758:
_temp757=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp739)->f1;
goto _LL750;} else{ goto _LL740;} _LL742: goto _LL744; _LL744: goto _LL746;
_LL746: return Cyc_Absynpp_var2doc((* q).f2); _LL748: _temp757= _temp755; goto
_LL750; _LL750: prefix= _temp757; goto _LL740; _LL740:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( prefix,({ struct
Cyc_List_List* _temp759=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp759->hd=( void*)(* q).f2; _temp759->tl= 0; _temp759;})),( struct
_tagged_string)({ char* _temp760=( char*)"_"; struct _tagged_string _temp761;
_temp761.curr= _temp760; _temp761.base= _temp760; _temp761.last_plus_one=
_temp760 + 2; _temp761;})));} else{ if((( int(*)( int(* cmp)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, prefix, Cyc_Absynpp_curr_namespace)){
return Cyc_Absynpp_var2doc((* q).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp762=( char*)""; struct _tagged_string
_temp763; _temp763.curr= _temp762; _temp763.base= _temp762; _temp763.last_plus_one=
_temp762 + 1; _temp763;}),( struct _tagged_string)({ char* _temp764=( char*)"::";
struct _tagged_string _temp765; _temp765.curr= _temp764; _temp765.base= _temp764;
_temp765.last_plus_one= _temp764 + 3; _temp765;}),( struct _tagged_string)({
char* _temp766=( char*)"::"; struct _tagged_string _temp767; _temp767.curr=
_temp766; _temp767.base= _temp766; _temp767.last_plus_one= _temp766 + 3;
_temp767;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_var2doc,
prefix)), Cyc_Absynpp_var2doc((* q).f2));}}} struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc(
struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){ return Cyc_Absynpp_qvar2doc(
v);}{ void* _temp768=(* v).f1; struct Cyc_List_List* _temp774; _LL770: if((
unsigned int) _temp768 > 1u?(( struct _tunion_struct*) _temp768)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL775: _temp774=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp768)->f1; goto _LL771;} else{ goto _LL772;} _LL772: goto _LL773; _LL771:
if((( int(*)( int(* cmp)( struct _tagged_string*, struct _tagged_string*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp,
_temp774, Cyc_Absynpp_curr_namespace) == 0){ return Cyc_Absynpp_var2doc((* v).f2);}
else{ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp776=( char*)"/* wrong namespace : */ "; struct _tagged_string _temp777;
_temp777.curr= _temp776; _temp777.base= _temp776; _temp777.last_plus_one=
_temp776 + 25; _temp777;})), Cyc_Absynpp_qvar2doc( v));} _LL773: return Cyc_Absynpp_qvar2doc(
v); _LL769:;}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*
e){ void* _temp778=( void*) e->r; void* _temp850; void* _temp852; struct _tuple0*
_temp854; struct _tuple0* _temp856; struct Cyc_List_List* _temp858; void*
_temp860; struct Cyc_Absyn_Exp* _temp862; struct Cyc_Core_Opt* _temp864; struct
Cyc_Absyn_Exp* _temp866; void* _temp868; struct Cyc_Absyn_Exp* _temp870; struct
Cyc_Absyn_Exp* _temp872; struct Cyc_Absyn_Exp* _temp874; struct Cyc_Absyn_Exp*
_temp876; struct Cyc_Absyn_Exp* _temp878; struct Cyc_Absyn_Exp* _temp880; struct
Cyc_List_List* _temp882; struct Cyc_Absyn_Exp* _temp884; struct Cyc_List_List*
_temp886; struct Cyc_Absyn_Exp* _temp888; struct Cyc_Absyn_Exp* _temp890; struct
Cyc_Absyn_Exp* _temp892; struct Cyc_List_List* _temp894; struct Cyc_Absyn_Exp*
_temp896; struct Cyc_Absyn_Exp* _temp898; void* _temp900; struct Cyc_Absyn_Exp*
_temp902; struct Cyc_Absyn_Exp* _temp904; void* _temp906; struct Cyc_Absyn_Exp*
_temp908; struct Cyc_Absyn_Exp* _temp910; struct _tagged_string* _temp912;
struct Cyc_Absyn_Exp* _temp914; struct _tagged_string* _temp916; struct Cyc_Absyn_Exp*
_temp918; struct Cyc_Absyn_Exp* _temp920; struct Cyc_Absyn_Exp* _temp922; struct
Cyc_List_List* _temp924; struct Cyc_List_List* _temp926; struct _tuple1*
_temp928; struct Cyc_List_List* _temp930; struct Cyc_Absyn_Exp* _temp932; struct
Cyc_Absyn_Exp* _temp934; struct Cyc_Absyn_Vardecl* _temp936; struct Cyc_Absyn_Structdecl*
_temp938; struct Cyc_List_List* _temp940; struct Cyc_Core_Opt* _temp942; struct
_tuple0* _temp944; struct Cyc_Absyn_Tunionfield* _temp946; struct Cyc_Absyn_Tuniondecl*
_temp948; struct Cyc_List_List* _temp950; struct Cyc_Core_Opt* _temp952; struct
Cyc_Core_Opt* _temp954; struct Cyc_Absyn_Tunionfield* _temp956; struct Cyc_Absyn_XTuniondecl*
_temp958; struct Cyc_List_List* _temp960; struct Cyc_Core_Opt* _temp962; struct
Cyc_Absyn_Enumfield* _temp964; struct Cyc_Absyn_Enumdecl* _temp966; struct
_tuple0* _temp968; void* _temp970; struct Cyc_List_List* _temp972; struct Cyc_Core_Opt*
_temp974; struct Cyc_Absyn_Stmt* _temp976; struct Cyc_Absyn_Fndecl* _temp978;
struct Cyc_Absyn_Exp* _temp980; _LL780: if((( struct _tunion_struct*) _temp778)->tag
== Cyc_Absyn_Const_e_tag){ _LL851: _temp850=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp778)->f1; goto _LL781;} else{ goto _LL782;} _LL782: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_Var_e_tag){ _LL855: _temp854=(
struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp778)->f1; goto _LL853;
_LL853: _temp852=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp778)->f2; goto
_LL783;} else{ goto _LL784;} _LL784: if((( struct _tunion_struct*) _temp778)->tag
== Cyc_Absyn_UnknownId_e_tag){ _LL857: _temp856=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp778)->f1; goto _LL785;} else{ goto _LL786;} _LL786: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_Primop_e_tag){ _LL861: _temp860=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp778)->f1; goto _LL859; _LL859:
_temp858=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp778)->f2;
goto _LL787;} else{ goto _LL788;} _LL788: if((( struct _tunion_struct*) _temp778)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL867: _temp866=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp778)->f1; goto _LL865; _LL865: _temp864=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp778)->f2; goto
_LL863; _LL863: _temp862=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp778)->f3; goto _LL789;} else{ goto _LL790;} _LL790: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_Increment_e_tag){ _LL871: _temp870=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp778)->f1;
goto _LL869; _LL869: _temp868=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp778)->f2; goto _LL791;} else{ goto _LL792;} _LL792: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_Conditional_e_tag){ _LL877:
_temp876=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp778)->f1; goto _LL875; _LL875: _temp874=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp778)->f2; goto _LL873; _LL873: _temp872=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp778)->f3;
goto _LL793;} else{ goto _LL794;} _LL794: if((( struct _tunion_struct*) _temp778)->tag
== Cyc_Absyn_SeqExp_e_tag){ _LL881: _temp880=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp778)->f1; goto _LL879; _LL879: _temp878=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp778)->f2; goto
_LL795;} else{ goto _LL796;} _LL796: if((( struct _tunion_struct*) _temp778)->tag
== Cyc_Absyn_UnknownCall_e_tag){ _LL885: _temp884=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp778)->f1; goto _LL883; _LL883:
_temp882=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp778)->f2; goto _LL797;} else{ goto _LL798;} _LL798: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_FnCall_e_tag){ _LL889: _temp888=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp778)->f1; goto
_LL887; _LL887: _temp886=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp778)->f2; goto _LL799;} else{ goto _LL800;} _LL800: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_Throw_e_tag){ _LL891: _temp890=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp778)->f1; goto
_LL801;} else{ goto _LL802;} _LL802: if((( struct _tunion_struct*) _temp778)->tag
== Cyc_Absyn_NoInstantiate_e_tag){ _LL893: _temp892=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp778)->f1; goto _LL803;} else{
goto _LL804;} _LL804: if((( struct _tunion_struct*) _temp778)->tag == Cyc_Absyn_Instantiate_e_tag){
_LL897: _temp896=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp778)->f1; goto _LL895; _LL895: _temp894=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp778)->f2; goto _LL805;} else{ goto _LL806;}
_LL806: if((( struct _tunion_struct*) _temp778)->tag == Cyc_Absyn_Cast_e_tag){
_LL901: _temp900=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp778)->f1; goto
_LL899; _LL899: _temp898=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp778)->f2; goto _LL807;} else{ goto _LL808;} _LL808: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_New_e_tag){ _LL903: _temp902=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp778)->f1; goto
_LL809;} else{ goto _LL810;} _LL810: if((( struct _tunion_struct*) _temp778)->tag
== Cyc_Absyn_Address_e_tag){ _LL905: _temp904=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Address_e_struct*) _temp778)->f1; goto _LL811;} else{ goto _LL812;}
_LL812: if((( struct _tunion_struct*) _temp778)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL907: _temp906=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp778)->f1;
goto _LL813;} else{ goto _LL814;} _LL814: if((( struct _tunion_struct*) _temp778)->tag
== Cyc_Absyn_Sizeofexp_e_tag){ _LL909: _temp908=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp778)->f1; goto _LL815;} else{ goto
_LL816;} _LL816: if((( struct _tunion_struct*) _temp778)->tag == Cyc_Absyn_Deref_e_tag){
_LL911: _temp910=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp778)->f1; goto _LL817;} else{ goto _LL818;} _LL818: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_StructMember_e_tag){ _LL915:
_temp914=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp778)->f1; goto _LL913; _LL913: _temp912=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp778)->f2; goto _LL819;} else{ goto _LL820;}
_LL820: if((( struct _tunion_struct*) _temp778)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL919: _temp918=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp778)->f1; goto _LL917; _LL917: _temp916=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp778)->f2; goto _LL821;} else{ goto _LL822;}
_LL822: if((( struct _tunion_struct*) _temp778)->tag == Cyc_Absyn_Subscript_e_tag){
_LL923: _temp922=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp778)->f1; goto _LL921; _LL921: _temp920=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp778)->f2; goto _LL823;} else{ goto _LL824;}
_LL824: if((( struct _tunion_struct*) _temp778)->tag == Cyc_Absyn_Tuple_e_tag){
_LL925: _temp924=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp778)->f1; goto _LL825;} else{ goto _LL826;} _LL826: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL929:
_temp928=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp778)->f1;
goto _LL927; _LL927: _temp926=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp778)->f2; goto _LL827;} else{ goto _LL828;} _LL828: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_Array_e_tag){ _LL931: _temp930=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp778)->f1; goto
_LL829;} else{ goto _LL830;} _LL830: if((( struct _tunion_struct*) _temp778)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL937: _temp936=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp778)->f1; goto _LL935; _LL935:
_temp934=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp778)->f2; goto _LL933; _LL933: _temp932=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp778)->f3; goto _LL831;} else{ goto
_LL832;} _LL832: if((( struct _tunion_struct*) _temp778)->tag == Cyc_Absyn_Struct_e_tag){
_LL945: _temp944=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp778)->f1; goto _LL943; _LL943: _temp942=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp778)->f2; goto _LL941; _LL941: _temp940=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp778)->f3; goto
_LL939; _LL939: _temp938=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp778)->f4; goto _LL833;} else{ goto _LL834;} _LL834: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_Tunion_e_tag){ _LL955: _temp954=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp778)->f1; goto
_LL953; _LL953: _temp952=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp778)->f2; goto _LL951; _LL951: _temp950=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp778)->f3; goto _LL949; _LL949: _temp948=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp778)->f4;
goto _LL947; _LL947: _temp946=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp778)->f5; goto _LL835;} else{ goto _LL836;} _LL836: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_XTunion_e_tag){ _LL963: _temp962=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp778)->f1; goto
_LL961; _LL961: _temp960=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp778)->f2; goto _LL959; _LL959: _temp958=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp778)->f3; goto _LL957; _LL957: _temp956=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp778)->f4;
goto _LL837;} else{ goto _LL838;} _LL838: if((( struct _tunion_struct*) _temp778)->tag
== Cyc_Absyn_Enum_e_tag){ _LL969: _temp968=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp778)->f1; goto _LL967; _LL967: _temp966=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp778)->f2; goto _LL965; _LL965: _temp964=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp778)->f3;
goto _LL839;} else{ goto _LL840;} _LL840: if((( struct _tunion_struct*) _temp778)->tag
== Cyc_Absyn_Malloc_e_tag){ _LL971: _temp970=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp778)->f1; goto _LL841;} else{ goto _LL842;} _LL842: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL975:
_temp974=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp778)->f1; goto _LL973; _LL973: _temp972=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp778)->f2; goto _LL843;} else{ goto
_LL844;} _LL844: if((( struct _tunion_struct*) _temp778)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL977: _temp976=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp778)->f1; goto _LL845;} else{ goto _LL846;} _LL846: if((( struct
_tunion_struct*) _temp778)->tag == Cyc_Absyn_Codegen_e_tag){ _LL979: _temp978=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp778)->f1;
goto _LL847;} else{ goto _LL848;} _LL848: if((( struct _tunion_struct*) _temp778)->tag
== Cyc_Absyn_Fill_e_tag){ _LL981: _temp980=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp778)->f1; goto _LL849;} else{ goto _LL779;} _LL781: return 10000; _LL783:
return 10000; _LL785: return 10000; _LL787: { void* _temp982= _temp860; _LL984:
if(( int) _temp982 == Cyc_Absyn_Plus){ goto _LL985;} else{ goto _LL986;} _LL986:
if(( int) _temp982 == Cyc_Absyn_Times){ goto _LL987;} else{ goto _LL988;} _LL988:
if(( int) _temp982 == Cyc_Absyn_Minus){ goto _LL989;} else{ goto _LL990;} _LL990:
if(( int) _temp982 == Cyc_Absyn_Div){ goto _LL991;} else{ goto _LL992;} _LL992:
if(( int) _temp982 == Cyc_Absyn_Mod){ goto _LL993;} else{ goto _LL994;} _LL994:
if(( int) _temp982 == Cyc_Absyn_Eq){ goto _LL995;} else{ goto _LL996;} _LL996:
if(( int) _temp982 == Cyc_Absyn_Neq){ goto _LL997;} else{ goto _LL998;} _LL998:
if(( int) _temp982 == Cyc_Absyn_Gt){ goto _LL999;} else{ goto _LL1000;} _LL1000:
if(( int) _temp982 == Cyc_Absyn_Lt){ goto _LL1001;} else{ goto _LL1002;} _LL1002:
if(( int) _temp982 == Cyc_Absyn_Gte){ goto _LL1003;} else{ goto _LL1004;}
_LL1004: if(( int) _temp982 == Cyc_Absyn_Lte){ goto _LL1005;} else{ goto _LL1006;}
_LL1006: if(( int) _temp982 == Cyc_Absyn_Not){ goto _LL1007;} else{ goto _LL1008;}
_LL1008: if(( int) _temp982 == Cyc_Absyn_Bitnot){ goto _LL1009;} else{ goto
_LL1010;} _LL1010: if(( int) _temp982 == Cyc_Absyn_Bitand){ goto _LL1011;} else{
goto _LL1012;} _LL1012: if(( int) _temp982 == Cyc_Absyn_Bitor){ goto _LL1013;}
else{ goto _LL1014;} _LL1014: if(( int) _temp982 == Cyc_Absyn_Bitxor){ goto
_LL1015;} else{ goto _LL1016;} _LL1016: if(( int) _temp982 == Cyc_Absyn_Bitlshift){
goto _LL1017;} else{ goto _LL1018;} _LL1018: if(( int) _temp982 == Cyc_Absyn_Bitlrshift){
goto _LL1019;} else{ goto _LL1020;} _LL1020: if(( int) _temp982 == Cyc_Absyn_Bitarshift){
goto _LL1021;} else{ goto _LL1022;} _LL1022: if(( int) _temp982 == Cyc_Absyn_Size){
goto _LL1023;} else{ goto _LL1024;} _LL1024: if(( int) _temp982 == Cyc_Absyn_Printf){
goto _LL1025;} else{ goto _LL1026;} _LL1026: if(( int) _temp982 == Cyc_Absyn_Fprintf){
goto _LL1027;} else{ goto _LL1028;} _LL1028: if(( int) _temp982 == Cyc_Absyn_Xprintf){
goto _LL1029;} else{ goto _LL1030;} _LL1030: if(( int) _temp982 == Cyc_Absyn_Scanf){
goto _LL1031;} else{ goto _LL1032;} _LL1032: if(( int) _temp982 == Cyc_Absyn_Fscanf){
goto _LL1033;} else{ goto _LL1034;} _LL1034: if(( int) _temp982 == Cyc_Absyn_Sscanf){
goto _LL1035;} else{ goto _LL983;} _LL985: return 100; _LL987: return 110;
_LL989: return 100; _LL991: return 110; _LL993: return 110; _LL995: return 70;
_LL997: return 70; _LL999: return 80; _LL1001: return 80; _LL1003: return 80;
_LL1005: return 80; _LL1007: return 130; _LL1009: return 130; _LL1011: return 60;
_LL1013: return 40; _LL1015: return 50; _LL1017: return 90; _LL1019: return 80;
_LL1021: return 80; _LL1023: return 140; _LL1025: return 130; _LL1027: return
130; _LL1029: return 130; _LL1031: return 130; _LL1033: return 130; _LL1035:
return 130; _LL983:;} _LL789: return 20; _LL791: return 20; _LL793: return 30;
_LL795: return 10; _LL797: return 140; _LL799: return 140; _LL801: return 130;
_LL803: return Cyc_Absynpp_exp_prec( _temp892); _LL805: return Cyc_Absynpp_exp_prec(
_temp896); _LL807: return 120; _LL809: return 15; _LL811: return 130; _LL813:
return 130; _LL815: return 130; _LL817: return 130; _LL819: return 140; _LL821:
return 140; _LL823: return 140; _LL825: return 150; _LL827: goto _LL829; _LL829:
goto _LL831; _LL831: goto _LL833; _LL833: goto _LL835; _LL835: goto _LL837;
_LL837: goto _LL839; _LL839: goto _LL841; _LL841: return 140; _LL843: return 140;
_LL845: return 10000; _LL847: return 140; _LL849: return 140; _LL779:;} struct
Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp1036=(
void*) e->r; void* _temp1112; void* _temp1114; struct Cyc_Absyn_Vardecl*
_temp1116; struct _tuple0* _temp1118; void* _temp1120; struct Cyc_Absyn_Vardecl*
_temp1122; struct _tuple0* _temp1124; void* _temp1126; struct _tuple0* _temp1128;
struct _tuple0* _temp1130; struct Cyc_List_List* _temp1132; void* _temp1134;
struct Cyc_Absyn_Exp* _temp1136; struct Cyc_Core_Opt* _temp1138; struct Cyc_Absyn_Exp*
_temp1140; void* _temp1142; struct Cyc_Absyn_Exp* _temp1144; struct Cyc_Absyn_Exp*
_temp1146; struct Cyc_Absyn_Exp* _temp1148; struct Cyc_Absyn_Exp* _temp1150;
struct Cyc_Absyn_Exp* _temp1152; struct Cyc_Absyn_Exp* _temp1154; struct Cyc_List_List*
_temp1156; struct Cyc_Absyn_Exp* _temp1158; struct Cyc_List_List* _temp1160;
struct Cyc_Absyn_Exp* _temp1162; struct Cyc_Absyn_Exp* _temp1164; struct Cyc_Absyn_Exp*
_temp1166; struct Cyc_List_List* _temp1168; struct Cyc_Absyn_Exp* _temp1170;
struct Cyc_Absyn_Exp* _temp1172; void* _temp1174; struct Cyc_Absyn_Exp*
_temp1176; struct Cyc_Absyn_Exp* _temp1178; void* _temp1180; struct Cyc_Absyn_Exp*
_temp1182; struct Cyc_Absyn_Exp* _temp1184; struct _tagged_string* _temp1186;
struct Cyc_Absyn_Exp* _temp1188; struct _tagged_string* _temp1190; struct Cyc_Absyn_Exp*
_temp1192; struct Cyc_Absyn_Exp* _temp1194; struct Cyc_Absyn_Exp* _temp1196;
struct Cyc_List_List* _temp1198; struct Cyc_List_List* _temp1200; struct _tuple1*
_temp1202; struct Cyc_List_List* _temp1204; struct Cyc_Absyn_Exp* _temp1206;
struct Cyc_Absyn_Exp* _temp1208; struct Cyc_Absyn_Vardecl* _temp1210; struct Cyc_Absyn_Structdecl*
_temp1212; struct Cyc_List_List* _temp1214; struct Cyc_Core_Opt* _temp1216;
struct _tuple0* _temp1218; struct Cyc_Absyn_Tunionfield* _temp1220; struct Cyc_Absyn_Tuniondecl*
_temp1222; struct Cyc_List_List* _temp1224; struct Cyc_Core_Opt* _temp1226;
struct Cyc_Core_Opt* _temp1228; struct Cyc_Absyn_Tunionfield* _temp1230; struct
Cyc_Absyn_XTuniondecl* _temp1232; struct Cyc_List_List* _temp1234; struct Cyc_Core_Opt*
_temp1236; struct Cyc_Absyn_Enumfield* _temp1238; struct Cyc_Absyn_Enumdecl*
_temp1240; struct _tuple0* _temp1242; void* _temp1244; struct Cyc_List_List*
_temp1246; struct Cyc_Core_Opt* _temp1248; struct Cyc_Absyn_Stmt* _temp1250;
struct Cyc_Absyn_Fndecl* _temp1252; struct Cyc_Absyn_Exp* _temp1254; _LL1038:
if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_Const_e_tag){ _LL1113:
_temp1112=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1036)->f1; goto
_LL1039;} else{ goto _LL1040;} _LL1040: if((( struct _tunion_struct*) _temp1036)->tag
== Cyc_Absyn_Var_e_tag){ _LL1119: _temp1118=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1036)->f1; goto _LL1115; _LL1115: _temp1114=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1036)->f2; if(( unsigned int) _temp1114 > 1u?(( struct _tunion_struct*)
_temp1114)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL1117: _temp1116=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp1114)->f1; goto _LL1041;} else{ goto
_LL1042;}} else{ goto _LL1042;} _LL1042: if((( struct _tunion_struct*) _temp1036)->tag
== Cyc_Absyn_Var_e_tag){ _LL1125: _temp1124=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1036)->f1; goto _LL1121; _LL1121: _temp1120=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1036)->f2; if(( unsigned int) _temp1120 > 1u?(( struct _tunion_struct*)
_temp1120)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL1123: _temp1122=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp1120)->f1; goto _LL1043;} else{ goto
_LL1044;}} else{ goto _LL1044;} _LL1044: if((( struct _tunion_struct*) _temp1036)->tag
== Cyc_Absyn_Var_e_tag){ _LL1129: _temp1128=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1036)->f1; goto _LL1127; _LL1127: _temp1126=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1036)->f2; goto _LL1045;} else{ goto _LL1046;} _LL1046: if((( struct
_tunion_struct*) _temp1036)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1131:
_temp1130=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1036)->f1;
goto _LL1047;} else{ goto _LL1048;} _LL1048: if((( struct _tunion_struct*)
_temp1036)->tag == Cyc_Absyn_Primop_e_tag){ _LL1135: _temp1134=( void*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp1036)->f1; goto _LL1133; _LL1133: _temp1132=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1036)->f2; goto
_LL1049;} else{ goto _LL1050;} _LL1050: if((( struct _tunion_struct*) _temp1036)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL1141: _temp1140=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1036)->f1; goto _LL1139; _LL1139:
_temp1138=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1036)->f2; goto _LL1137; _LL1137: _temp1136=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1036)->f3; goto _LL1051;} else{ goto
_LL1052;} _LL1052: if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_Increment_e_tag){
_LL1145: _temp1144=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1036)->f1; goto _LL1143; _LL1143: _temp1142=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1036)->f2; goto _LL1053;} else{ goto _LL1054;} _LL1054: if((( struct
_tunion_struct*) _temp1036)->tag == Cyc_Absyn_Conditional_e_tag){ _LL1151:
_temp1150=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1036)->f1; goto _LL1149; _LL1149: _temp1148=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1036)->f2; goto _LL1147; _LL1147:
_temp1146=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1036)->f3; goto _LL1055;} else{ goto _LL1056;} _LL1056: if((( struct
_tunion_struct*) _temp1036)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1155: _temp1154=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1036)->f1; goto
_LL1153; _LL1153: _temp1152=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1036)->f2; goto _LL1057;} else{ goto _LL1058;} _LL1058: if((( struct
_tunion_struct*) _temp1036)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1159:
_temp1158=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1036)->f1; goto _LL1157; _LL1157: _temp1156=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1036)->f2; goto _LL1059;} else{
goto _LL1060;} _LL1060: if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1163: _temp1162=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1036)->f1; goto _LL1161; _LL1161: _temp1160=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1036)->f2; goto _LL1061;} else{ goto
_LL1062;} _LL1062: if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_Throw_e_tag){
_LL1165: _temp1164=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp1036)->f1; goto _LL1063;} else{ goto _LL1064;} _LL1064: if((( struct
_tunion_struct*) _temp1036)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1167:
_temp1166=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1036)->f1; goto _LL1065;} else{ goto _LL1066;} _LL1066: if((( struct
_tunion_struct*) _temp1036)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1171:
_temp1170=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1036)->f1; goto _LL1169; _LL1169: _temp1168=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1036)->f2; goto _LL1067;} else{
goto _LL1068;} _LL1068: if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_Cast_e_tag){
_LL1175: _temp1174=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1036)->f1;
goto _LL1173; _LL1173: _temp1172=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1036)->f2; goto _LL1069;} else{ goto _LL1070;} _LL1070: if((( struct
_tunion_struct*) _temp1036)->tag == Cyc_Absyn_Address_e_tag){ _LL1177: _temp1176=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp1036)->f1;
goto _LL1071;} else{ goto _LL1072;} _LL1072: if((( struct _tunion_struct*)
_temp1036)->tag == Cyc_Absyn_New_e_tag){ _LL1179: _temp1178=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1036)->f1; goto _LL1073;} else{ goto
_LL1074;} _LL1074: if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL1181: _temp1180=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1036)->f1;
goto _LL1075;} else{ goto _LL1076;} _LL1076: if((( struct _tunion_struct*)
_temp1036)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL1183: _temp1182=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp1036)->f1; goto _LL1077;} else{ goto
_LL1078;} _LL1078: if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_Deref_e_tag){
_LL1185: _temp1184=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp1036)->f1; goto _LL1079;} else{ goto _LL1080;} _LL1080: if((( struct
_tunion_struct*) _temp1036)->tag == Cyc_Absyn_StructMember_e_tag){ _LL1189:
_temp1188=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1036)->f1; goto _LL1187; _LL1187: _temp1186=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1036)->f2; goto _LL1081;} else{
goto _LL1082;} _LL1082: if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1193: _temp1192=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1036)->f1; goto _LL1191; _LL1191: _temp1190=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1036)->f2; goto _LL1083;} else{
goto _LL1084;} _LL1084: if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1197: _temp1196=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1036)->f1; goto _LL1195; _LL1195: _temp1194=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1036)->f2; goto _LL1085;} else{ goto
_LL1086;} _LL1086: if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1199: _temp1198=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1036)->f1; goto _LL1087;} else{ goto _LL1088;} _LL1088: if((( struct
_tunion_struct*) _temp1036)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1203:
_temp1202=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1036)->f1;
goto _LL1201; _LL1201: _temp1200=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1036)->f2; goto _LL1089;} else{ goto _LL1090;} _LL1090: if((( struct
_tunion_struct*) _temp1036)->tag == Cyc_Absyn_Array_e_tag){ _LL1205: _temp1204=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1036)->f1; goto
_LL1091;} else{ goto _LL1092;} _LL1092: if((( struct _tunion_struct*) _temp1036)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1211: _temp1210=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1036)->f1; goto _LL1209; _LL1209:
_temp1208=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1036)->f2; goto _LL1207; _LL1207: _temp1206=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1036)->f3; goto _LL1093;} else{
goto _LL1094;} _LL1094: if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_Struct_e_tag){
_LL1219: _temp1218=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1036)->f1; goto _LL1217; _LL1217: _temp1216=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1036)->f2; goto _LL1215; _LL1215:
_temp1214=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1036)->f3; goto _LL1213; _LL1213: _temp1212=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1036)->f4; goto _LL1095;} else{ goto
_LL1096;} _LL1096: if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_Tunion_e_tag){
_LL1229: _temp1228=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1036)->f1; goto _LL1227; _LL1227: _temp1226=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1036)->f2; goto _LL1225; _LL1225:
_temp1224=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1036)->f3; goto _LL1223; _LL1223: _temp1222=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1036)->f4; goto _LL1221; _LL1221:
_temp1220=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1036)->f5; goto _LL1097;} else{ goto _LL1098;} _LL1098: if((( struct
_tunion_struct*) _temp1036)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1237: _temp1236=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1036)->f1; goto
_LL1235; _LL1235: _temp1234=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1036)->f2; goto _LL1233; _LL1233: _temp1232=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1036)->f3; goto _LL1231; _LL1231:
_temp1230=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1036)->f4; goto _LL1099;} else{ goto _LL1100;} _LL1100: if((( struct
_tunion_struct*) _temp1036)->tag == Cyc_Absyn_Enum_e_tag){ _LL1243: _temp1242=(
struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1036)->f1; goto _LL1241;
_LL1241: _temp1240=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1036)->f2; goto _LL1239; _LL1239: _temp1238=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1036)->f3; goto _LL1101;} else{ goto
_LL1102;} _LL1102: if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1245: _temp1244=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp1036)->f1;
goto _LL1103;} else{ goto _LL1104;} _LL1104: if((( struct _tunion_struct*)
_temp1036)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1249: _temp1248=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1036)->f1; goto
_LL1247; _LL1247: _temp1246=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1036)->f2; goto _LL1105;} else{ goto _LL1106;} _LL1106: if((( struct
_tunion_struct*) _temp1036)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1251: _temp1250=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1036)->f1;
goto _LL1107;} else{ goto _LL1108;} _LL1108: if((( struct _tunion_struct*)
_temp1036)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1253: _temp1252=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp1036)->f1; goto _LL1109;} else{ goto
_LL1110;} _LL1110: if((( struct _tunion_struct*) _temp1036)->tag == Cyc_Absyn_Fill_e_tag){
_LL1255: _temp1254=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp1036)->f1; goto _LL1111;} else{ goto _LL1037;} _LL1039: s= Cyc_Absynpp_cnst2doc(
_temp1112); goto _LL1037; _LL1041: s= Cyc_Absynpp_qvar2doc( _temp1118); goto
_LL1037; _LL1043: s= Cyc_Absynpp_qvar2doc( _temp1124); goto _LL1037; _LL1045: s=
Cyc_Absynpp_qvar2doc( _temp1128); goto _LL1037; _LL1047: s= Cyc_Absynpp_qvar2doc(
_temp1130); goto _LL1037; _LL1049: s= Cyc_Absynpp_primapp2doc( myprec, _temp1134,
_temp1132); goto _LL1037; _LL1051: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1140), Cyc_PP_text(( struct _tagged_string)({ char* _temp1256=(
char*)" "; struct _tagged_string _temp1257; _temp1257.curr= _temp1256; _temp1257.base=
_temp1256; _temp1257.last_plus_one= _temp1256 + 2; _temp1257;})), _temp1138 == 0?
Cyc_PP_text(( struct _tagged_string)({ char* _temp1258=( char*)""; struct
_tagged_string _temp1259; _temp1259.curr= _temp1258; _temp1259.base= _temp1258;
_temp1259.last_plus_one= _temp1258 + 1; _temp1259;})): Cyc_Absynpp_prim2doc((
void*)({ struct Cyc_Core_Opt* _temp1260= _temp1138; if( _temp1260 == 0){ _throw(
Null_Exception);} _temp1260->v;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1261=( char*)"= "; struct _tagged_string _temp1262; _temp1262.curr=
_temp1261; _temp1262.base= _temp1261; _temp1262.last_plus_one= _temp1261 + 3;
_temp1262;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1136)); goto _LL1037;
_LL1053: { struct Cyc_PP_Doc* es= Cyc_Absynpp_exp2doc_prec( myprec, _temp1144);{
void* _temp1263= _temp1142; _LL1265: if(( int) _temp1263 == Cyc_Absyn_PreInc){
goto _LL1266;} else{ goto _LL1267;} _LL1267: if(( int) _temp1263 == Cyc_Absyn_PreDec){
goto _LL1268;} else{ goto _LL1269;} _LL1269: if(( int) _temp1263 == Cyc_Absyn_PostInc){
goto _LL1270;} else{ goto _LL1271;} _LL1271: if(( int) _temp1263 == Cyc_Absyn_PostDec){
goto _LL1272;} else{ goto _LL1264;} _LL1266: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1273=( char*)"++"; struct _tagged_string
_temp1274; _temp1274.curr= _temp1273; _temp1274.base= _temp1273; _temp1274.last_plus_one=
_temp1273 + 3; _temp1274;})), es); goto _LL1264; _LL1268: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1275=( char*)"--"; struct
_tagged_string _temp1276; _temp1276.curr= _temp1275; _temp1276.base= _temp1275;
_temp1276.last_plus_one= _temp1275 + 3; _temp1276;})), es); goto _LL1264;
_LL1270: s= Cyc_Absynpp_cat2( es, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1277=( char*)"++"; struct _tagged_string _temp1278; _temp1278.curr=
_temp1277; _temp1278.base= _temp1277; _temp1278.last_plus_one= _temp1277 + 3;
_temp1278;}))); goto _LL1264; _LL1272: s= Cyc_Absynpp_cat2( es, Cyc_PP_text((
struct _tagged_string)({ char* _temp1279=( char*)"--"; struct _tagged_string
_temp1280; _temp1280.curr= _temp1279; _temp1280.base= _temp1279; _temp1280.last_plus_one=
_temp1279 + 3; _temp1280;}))); goto _LL1264; _LL1264:;} goto _LL1037;} _LL1055:{
struct _tuple8 _temp1282=({ struct _tuple8 _temp1281; _temp1281.f1=( void*)
_temp1148->r; _temp1281.f2=( void*) _temp1146->r; _temp1281;}); _LL1284: goto
_LL1285; _LL1285: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1150), Cyc_PP_text(( struct _tagged_string)({ char* _temp1286=( char*)" ? ";
struct _tagged_string _temp1287; _temp1287.curr= _temp1286; _temp1287.base=
_temp1286; _temp1287.last_plus_one= _temp1286 + 4; _temp1287;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1148), Cyc_PP_text(( struct _tagged_string)({ char* _temp1288=( char*)" : ";
struct _tagged_string _temp1289; _temp1289.curr= _temp1288; _temp1289.base=
_temp1288; _temp1289.last_plus_one= _temp1288 + 4; _temp1289;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1146)); goto _LL1283; _LL1283:;} goto _LL1037; _LL1057: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1154), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1290=( char*)", "; struct _tagged_string _temp1291;
_temp1291.curr= _temp1290; _temp1291.base= _temp1290; _temp1291.last_plus_one=
_temp1290 + 3; _temp1291;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1152));
goto _LL1037; _LL1059: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1158), Cyc_PP_text(( struct _tagged_string)({ char* _temp1292=( char*)"(";
struct _tagged_string _temp1293; _temp1293.curr= _temp1292; _temp1293.base=
_temp1292; _temp1293.last_plus_one= _temp1292 + 2; _temp1293;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1156), Cyc_PP_text(( struct _tagged_string)({ char* _temp1294=( char*)")";
struct _tagged_string _temp1295; _temp1295.curr= _temp1294; _temp1295.base=
_temp1294; _temp1295.last_plus_one= _temp1294 + 2; _temp1295;}))); goto _LL1037;
_LL1061: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1162), Cyc_PP_text((
struct _tagged_string)({ char* _temp1296=( char*)"("; struct _tagged_string
_temp1297; _temp1297.curr= _temp1296; _temp1297.base= _temp1296; _temp1297.last_plus_one=
_temp1296 + 2; _temp1297;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1160), Cyc_PP_text((
struct _tagged_string)({ char* _temp1298=( char*)")"; struct _tagged_string
_temp1299; _temp1299.curr= _temp1298; _temp1299.base= _temp1298; _temp1299.last_plus_one=
_temp1298 + 2; _temp1299;}))); goto _LL1037; _LL1063: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1300=( char*)"throw "; struct _tagged_string
_temp1301; _temp1301.curr= _temp1300; _temp1301.base= _temp1300; _temp1301.last_plus_one=
_temp1300 + 7; _temp1301;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1164));
goto _LL1037; _LL1065: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1166); goto
_LL1037; _LL1067: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1170); goto _LL1037;
_LL1069: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1302=( char*)"("; struct _tagged_string _temp1303; _temp1303.curr=
_temp1302; _temp1303.base= _temp1302; _temp1303.last_plus_one= _temp1302 + 2;
_temp1303;})), Cyc_Absynpp_typ2doc( _temp1174), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1304=( char*)")"; struct _tagged_string _temp1305;
_temp1305.curr= _temp1304; _temp1305.base= _temp1304; _temp1305.last_plus_one=
_temp1304 + 2; _temp1305;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1172));
goto _LL1037; _LL1071: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({
char* _temp1306=( char*)"&"; struct _tagged_string _temp1307; _temp1307.curr=
_temp1306; _temp1307.base= _temp1306; _temp1307.last_plus_one= _temp1306 + 2;
_temp1307;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1176)); goto _LL1037;
_LL1073: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1308=( char*)"new "; struct _tagged_string _temp1309; _temp1309.curr=
_temp1308; _temp1309.base= _temp1308; _temp1309.last_plus_one= _temp1308 + 5;
_temp1309;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1178)); goto _LL1037;
_LL1075: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1310=( char*)"sizeof("; struct _tagged_string _temp1311; _temp1311.curr=
_temp1310; _temp1311.base= _temp1310; _temp1311.last_plus_one= _temp1310 + 8;
_temp1311;})), Cyc_Absynpp_typ2doc( _temp1180), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1312=( char*)")"; struct _tagged_string _temp1313;
_temp1313.curr= _temp1312; _temp1313.base= _temp1312; _temp1313.last_plus_one=
_temp1312 + 2; _temp1313;}))); goto _LL1037; _LL1077: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1314=( char*)"sizeof("; struct
_tagged_string _temp1315; _temp1315.curr= _temp1314; _temp1315.base= _temp1314;
_temp1315.last_plus_one= _temp1314 + 8; _temp1315;})), Cyc_Absynpp_exp2doc(
_temp1182), Cyc_PP_text(( struct _tagged_string)({ char* _temp1316=( char*)")";
struct _tagged_string _temp1317; _temp1317.curr= _temp1316; _temp1317.base=
_temp1316; _temp1317.last_plus_one= _temp1316 + 2; _temp1317;}))); goto _LL1037;
_LL1079: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1318=( char*)"*"; struct _tagged_string _temp1319; _temp1319.curr=
_temp1318; _temp1319.base= _temp1318; _temp1319.last_plus_one= _temp1318 + 2;
_temp1319;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1184)); goto _LL1037;
_LL1081: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec, _temp1188), Cyc_PP_text((
struct _tagged_string)({ char* _temp1320=( char*)"."; struct _tagged_string
_temp1321; _temp1321.curr= _temp1320; _temp1321.base= _temp1320; _temp1321.last_plus_one=
_temp1320 + 2; _temp1321;})), Cyc_Absynpp_textptr( _temp1186)); goto _LL1037;
_LL1083: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec, _temp1192), Cyc_PP_text((
struct _tagged_string)({ char* _temp1322=( char*)"->"; struct _tagged_string
_temp1323; _temp1323.curr= _temp1322; _temp1323.base= _temp1322; _temp1323.last_plus_one=
_temp1322 + 3; _temp1323;})), Cyc_Absynpp_textptr( _temp1190)); goto _LL1037;
_LL1085: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1196), Cyc_PP_text((
struct _tagged_string)({ char* _temp1324=( char*)"["; struct _tagged_string
_temp1325; _temp1325.curr= _temp1324; _temp1325.base= _temp1324; _temp1325.last_plus_one=
_temp1324 + 2; _temp1325;})), Cyc_Absynpp_exp2doc( _temp1194), Cyc_PP_text((
struct _tagged_string)({ char* _temp1326=( char*)"]"; struct _tagged_string
_temp1327; _temp1327.curr= _temp1326; _temp1327.base= _temp1326; _temp1327.last_plus_one=
_temp1326 + 2; _temp1327;}))); goto _LL1037; _LL1087: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1328=( char*)"$("; struct _tagged_string
_temp1329; _temp1329.curr= _temp1328; _temp1329.base= _temp1328; _temp1329.last_plus_one=
_temp1328 + 3; _temp1329;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1198), Cyc_PP_text((
struct _tagged_string)({ char* _temp1330=( char*)")"; struct _tagged_string
_temp1331; _temp1331.curr= _temp1330; _temp1331.base= _temp1330; _temp1331.last_plus_one=
_temp1330 + 2; _temp1331;}))); goto _LL1037; _LL1089: s= Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1332=( char*)"("; struct _tagged_string
_temp1333; _temp1333.curr= _temp1332; _temp1333.base= _temp1332; _temp1333.last_plus_one=
_temp1332 + 2; _temp1333;})), Cyc_Absynpp_typ2doc((* _temp1202).f3), Cyc_PP_text((
struct _tagged_string)({ char* _temp1334=( char*)")"; struct _tagged_string
_temp1335; _temp1335.curr= _temp1334; _temp1335.base= _temp1334; _temp1335.last_plus_one=
_temp1334 + 2; _temp1335;})), Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp1336=( char*)"{"; struct _tagged_string _temp1337; _temp1337.curr=
_temp1336; _temp1337.base= _temp1336; _temp1337.last_plus_one= _temp1336 + 2;
_temp1337;}),( struct _tagged_string)({ char* _temp1338=( char*)"}"; struct
_tagged_string _temp1339; _temp1339.curr= _temp1338; _temp1339.base= _temp1338;
_temp1339.last_plus_one= _temp1338 + 2; _temp1339;}),( struct _tagged_string)({
char* _temp1340=( char*)","; struct _tagged_string _temp1341; _temp1341.curr=
_temp1340; _temp1341.base= _temp1340; _temp1341.last_plus_one= _temp1340 + 2;
_temp1341;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1200))); goto _LL1037; _LL1091: s= Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1342=( char*)"{"; struct _tagged_string _temp1343;
_temp1343.curr= _temp1342; _temp1343.base= _temp1342; _temp1343.last_plus_one=
_temp1342 + 2; _temp1343;}),( struct _tagged_string)({ char* _temp1344=( char*)"}";
struct _tagged_string _temp1345; _temp1345.curr= _temp1344; _temp1345.base=
_temp1344; _temp1345.last_plus_one= _temp1344 + 2; _temp1345;}),( struct
_tagged_string)({ char* _temp1346=( char*)","; struct _tagged_string _temp1347;
_temp1347.curr= _temp1346; _temp1347.base= _temp1346; _temp1347.last_plus_one=
_temp1346 + 2; _temp1347;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1204)); goto _LL1037; _LL1093: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1348=( char*)"new {for "; struct _tagged_string
_temp1349; _temp1349.curr= _temp1348; _temp1349.base= _temp1348; _temp1349.last_plus_one=
_temp1348 + 10; _temp1349;})), Cyc_PP_text(*(* _temp1210->name).f2), Cyc_PP_text((
struct _tagged_string)({ char* _temp1350=( char*)" < "; struct _tagged_string
_temp1351; _temp1351.curr= _temp1350; _temp1351.base= _temp1350; _temp1351.last_plus_one=
_temp1350 + 4; _temp1351;})), Cyc_Absynpp_exp2doc( _temp1208), Cyc_PP_text((
struct _tagged_string)({ char* _temp1352=( char*)" : "; struct _tagged_string
_temp1353; _temp1353.curr= _temp1352; _temp1353.base= _temp1352; _temp1353.last_plus_one=
_temp1352 + 4; _temp1353;})), Cyc_Absynpp_exp2doc( _temp1206), Cyc_PP_text((
struct _tagged_string)({ char* _temp1354=( char*)"}"; struct _tagged_string
_temp1355; _temp1355.curr= _temp1354; _temp1355.base= _temp1354; _temp1355.last_plus_one=
_temp1354 + 2; _temp1355;}))); goto _LL1037; _LL1095: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1218), Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1356=( char*)"{";
struct _tagged_string _temp1357; _temp1357.curr= _temp1356; _temp1357.base=
_temp1356; _temp1357.last_plus_one= _temp1356 + 2; _temp1357;}),( struct
_tagged_string)({ char* _temp1358=( char*)"}"; struct _tagged_string _temp1359;
_temp1359.curr= _temp1358; _temp1359.base= _temp1358; _temp1359.last_plus_one=
_temp1358 + 2; _temp1359;}),( struct _tagged_string)({ char* _temp1360=( char*)",";
struct _tagged_string _temp1361; _temp1361.curr= _temp1360; _temp1361.base=
_temp1360; _temp1361.last_plus_one= _temp1360 + 2; _temp1361;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1214))); goto _LL1037; _LL1097: _temp1234= _temp1224;
_temp1230= _temp1220; goto _LL1099; _LL1099: if( _temp1234 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1230->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1230->name),
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1362=( char*)"(";
struct _tagged_string _temp1363; _temp1363.curr= _temp1362; _temp1363.base=
_temp1362; _temp1363.last_plus_one= _temp1362 + 2; _temp1363;}),( struct
_tagged_string)({ char* _temp1364=( char*)")"; struct _tagged_string _temp1365;
_temp1365.curr= _temp1364; _temp1365.base= _temp1364; _temp1365.last_plus_one=
_temp1364 + 2; _temp1365;}),( struct _tagged_string)({ char* _temp1366=( char*)",";
struct _tagged_string _temp1367; _temp1367.curr= _temp1366; _temp1367.base=
_temp1366; _temp1367.last_plus_one= _temp1366 + 2; _temp1367;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp1234)));} goto _LL1037; _LL1101: s= Cyc_Absynpp_qvar2doc(
_temp1242); goto _LL1037; _LL1103: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1368=( char*)"malloc(sizeof("; struct
_tagged_string _temp1369; _temp1369.curr= _temp1368; _temp1369.base= _temp1368;
_temp1369.last_plus_one= _temp1368 + 15; _temp1369;})), Cyc_Absynpp_typ2doc(
_temp1244), Cyc_PP_text(( struct _tagged_string)({ char* _temp1370=( char*)"))";
struct _tagged_string _temp1371; _temp1371.curr= _temp1370; _temp1371.base=
_temp1370; _temp1371.last_plus_one= _temp1370 + 3; _temp1371;}))); goto _LL1037;
_LL1105: s= Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1372=( char*)"{";
struct _tagged_string _temp1373; _temp1373.curr= _temp1372; _temp1373.base=
_temp1372; _temp1373.last_plus_one= _temp1372 + 2; _temp1373;}),( struct
_tagged_string)({ char* _temp1374=( char*)"}"; struct _tagged_string _temp1375;
_temp1375.curr= _temp1374; _temp1375.base= _temp1374; _temp1375.last_plus_one=
_temp1374 + 2; _temp1375;}),( struct _tagged_string)({ char* _temp1376=( char*)",";
struct _tagged_string _temp1377; _temp1377.curr= _temp1376; _temp1377.base=
_temp1376; _temp1377.last_plus_one= _temp1376 + 2; _temp1377;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1246)); goto _LL1037; _LL1107: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1378=( char*)"({ "; struct _tagged_string
_temp1379; _temp1379.curr= _temp1378; _temp1379.base= _temp1378; _temp1379.last_plus_one=
_temp1378 + 4; _temp1379;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1250)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1380=( char*)" })"; struct
_tagged_string _temp1381; _temp1381.curr= _temp1380; _temp1381.base= _temp1380;
_temp1381.last_plus_one= _temp1380 + 4; _temp1381;}))); goto _LL1037; _LL1109: s=
Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1382=( char*)"codegen(";
struct _tagged_string _temp1383; _temp1383.curr= _temp1382; _temp1383.base=
_temp1382; _temp1383.last_plus_one= _temp1382 + 9; _temp1383;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1384=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1384->r=( void*)(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp1385=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1385[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1386; _temp1386.tag= Cyc_Absyn_Fn_d_tag; _temp1386.f1= _temp1252; _temp1386;});
_temp1385;})); _temp1384->loc= e->loc; _temp1384;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1387=( char*)")"; struct _tagged_string _temp1388;
_temp1388.curr= _temp1387; _temp1388.base= _temp1387; _temp1388.last_plus_one=
_temp1387 + 2; _temp1388;}))); goto _LL1037; _LL1111: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1389=( char*)"fill("; struct _tagged_string
_temp1390; _temp1390.curr= _temp1389; _temp1390.base= _temp1389; _temp1390.last_plus_one=
_temp1389 + 6; _temp1390;})), Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1254)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1391=( char*)")"; struct
_tagged_string _temp1392; _temp1392.curr= _temp1391; _temp1392.base= _temp1391;
_temp1392.last_plus_one= _temp1391 + 2; _temp1392;}))); goto _LL1037; _LL1037:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp1393=( char*)"("; struct _tagged_string _temp1394; _temp1394.curr=
_temp1393; _temp1394.base= _temp1393; _temp1394.last_plus_one= _temp1393 + 2;
_temp1394;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp1395=( char*)")";
struct _tagged_string _temp1396; _temp1396.curr= _temp1395; _temp1396.base=
_temp1395; _temp1396.last_plus_one= _temp1395 + 2; _temp1396;})));} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1397= d;
struct Cyc_Absyn_Exp* _temp1403; struct _tagged_string* _temp1405; _LL1399: if(((
struct _tunion_struct*) _temp1397)->tag == Cyc_Absyn_ArrayElement_tag){ _LL1404:
_temp1403=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1397)->f1; goto _LL1400;} else{ goto _LL1401;} _LL1401: if((( struct
_tunion_struct*) _temp1397)->tag == Cyc_Absyn_FieldName_tag){ _LL1406: _temp1405=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1397)->f1;
goto _LL1402;} else{ goto _LL1398;} _LL1400: return Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1407=( char*)".["; struct _tagged_string
_temp1408; _temp1408.curr= _temp1407; _temp1408.base= _temp1407; _temp1408.last_plus_one=
_temp1407 + 3; _temp1408;})), Cyc_Absynpp_exp2doc( _temp1403), Cyc_PP_text((
struct _tagged_string)({ char* _temp1409=( char*)"]"; struct _tagged_string
_temp1410; _temp1410.curr= _temp1409; _temp1410.base= _temp1409; _temp1410.last_plus_one=
_temp1409 + 2; _temp1410;}))); _LL1402: return Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1411=( char*)"."; struct _tagged_string
_temp1412; _temp1412.curr= _temp1411; _temp1412.base= _temp1411; _temp1412.last_plus_one=
_temp1411 + 2; _temp1412;})), Cyc_Absynpp_textptr( _temp1405)); _LL1398:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 == 0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp1413=( char*)""; struct _tagged_string
_temp1414; _temp1414.curr= _temp1413; _temp1414.base= _temp1413; _temp1414.last_plus_one=
_temp1413 + 1; _temp1414;}),( struct _tagged_string)({ char* _temp1415=( char*)"=";
struct _tagged_string _temp1416; _temp1416.curr= _temp1415; _temp1416.base=
_temp1415; _temp1416.last_plus_one= _temp1415 + 2; _temp1416;}),( struct
_tagged_string)({ char* _temp1417=( char*)"="; struct _tagged_string _temp1418;
_temp1418.curr= _temp1417; _temp1418.base= _temp1417; _temp1418.last_plus_one=
_temp1417 + 2; _temp1418;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)), Cyc_Absynpp_exp2doc((* de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1419=( char*)""; struct _tagged_string _temp1420;
_temp1420.curr= _temp1419; _temp1420.base= _temp1419; _temp1420.last_plus_one=
_temp1419 + 1; _temp1420;}),( struct _tagged_string)({ char* _temp1421=( char*)"";
struct _tagged_string _temp1422; _temp1422.curr= _temp1421; _temp1422.base=
_temp1421; _temp1422.last_plus_one= _temp1421 + 1; _temp1422;}),( struct
_tagged_string)({ char* _temp1423=( char*)","; struct _tagged_string _temp1424;
_temp1424.curr= _temp1423; _temp1424.base= _temp1423; _temp1424.last_plus_one=
_temp1423 + 2; _temp1424;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1425= c; char _temp1443; void* _temp1445; short _temp1447;
void* _temp1449; int _temp1451; void* _temp1453; int _temp1455; void* _temp1457;
long long _temp1459; void* _temp1461; struct _tagged_string _temp1463; struct
_tagged_string _temp1465; _LL1427: if(( unsigned int) _temp1425 > 1u?(( struct
_tunion_struct*) _temp1425)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL1446: _temp1445=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp1425)->f1; goto _LL1444; _LL1444:
_temp1443=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp1425)->f2; goto
_LL1428;} else{ goto _LL1429;} _LL1429: if(( unsigned int) _temp1425 > 1u?((
struct _tunion_struct*) _temp1425)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL1450:
_temp1449=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1425)->f1; goto
_LL1448; _LL1448: _temp1447=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp1425)->f2; goto _LL1430;} else{ goto _LL1431;} _LL1431: if(( unsigned int)
_temp1425 > 1u?(( struct _tunion_struct*) _temp1425)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL1454: _temp1453=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1425)->f1;
if(( int) _temp1453 == Cyc_Absyn_Signed){ goto _LL1452;} else{ goto _LL1433;}
_LL1452: _temp1451=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1425)->f2; goto
_LL1432;} else{ goto _LL1433;} _LL1433: if(( unsigned int) _temp1425 > 1u?((
struct _tunion_struct*) _temp1425)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL1458:
_temp1457=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1425)->f1; if(( int)
_temp1457 == Cyc_Absyn_Unsigned){ goto _LL1456;} else{ goto _LL1435;} _LL1456:
_temp1455=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1425)->f2; goto _LL1434;}
else{ goto _LL1435;} _LL1435: if(( unsigned int) _temp1425 > 1u?(( struct
_tunion_struct*) _temp1425)->tag == Cyc_Absyn_LongLong_c_tag: 0){ _LL1462:
_temp1461=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1425)->f1; goto
_LL1460; _LL1460: _temp1459=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1425)->f2; goto _LL1436;} else{ goto _LL1437;} _LL1437: if(( unsigned int)
_temp1425 > 1u?(( struct _tunion_struct*) _temp1425)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL1464: _temp1463=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp1425)->f1; goto _LL1438;} else{ goto _LL1439;} _LL1439: if(( int) _temp1425
== Cyc_Absyn_Null_c){ goto _LL1440;} else{ goto _LL1441;} _LL1441: if((
unsigned int) _temp1425 > 1u?(( struct _tunion_struct*) _temp1425)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1466: _temp1465=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1425)->f1; goto _LL1442;} else{ goto _LL1426;} _LL1428: return Cyc_PP_text(({
struct _tagged_string _temp1467= Cyc_Absynpp_char_escape( _temp1443); xprintf("'%.*s'",
_temp1467.last_plus_one - _temp1467.curr, _temp1467.curr);})); _LL1430: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1447)); _LL1432: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1451)); _LL1434: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1455)); _LL1436: return Cyc_PP_text(( struct _tagged_string)({
char* _temp1468=( char*)"<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp1469; _temp1469.curr= _temp1468; _temp1469.base= _temp1468; _temp1469.last_plus_one=
_temp1468 + 27; _temp1469;})); _LL1438: return Cyc_PP_text( _temp1463); _LL1440:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp1470=( char*)"null";
struct _tagged_string _temp1471; _temp1471.curr= _temp1470; _temp1471.base=
_temp1470; _temp1471.last_plus_one= _temp1470 + 5; _temp1471;})); _LL1442:
return Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1472=(
char*)"\""; struct _tagged_string _temp1473; _temp1473.curr= _temp1472;
_temp1473.base= _temp1472; _temp1473.last_plus_one= _temp1472 + 2; _temp1473;})),
Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1465)), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1474=( char*)"\""; struct _tagged_string _temp1475;
_temp1475.curr= _temp1474; _temp1475.base= _temp1474; _temp1475.last_plus_one=
_temp1474 + 2; _temp1475;}))); _LL1426:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:({ struct Cyc_List_List*
_temp1476= es; if( _temp1476 == 0){ _throw( Null_Exception);} _temp1476->tl;})
!= 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct*
_temp1477=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1477[ 0]=({ struct Cyc_Core_Failure_struct _temp1478; _temp1478.tag= Cyc_Core_Failure_tag;
_temp1478.f1=({ struct _tagged_string _temp1479= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1479.last_plus_one
- _temp1479.curr, _temp1479.curr);}); _temp1478;}); _temp1477;}));}{ struct Cyc_PP_Doc*
d= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1480= es; if( _temp1480 == 0){ _throw( Null_Exception);} _temp1480->hd;}));
return Cyc_Absynpp_cat2( d, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1481=( char*)".size"; struct _tagged_string _temp1482; _temp1482.curr=
_temp1481; _temp1482.base= _temp1481; _temp1482.last_plus_one= _temp1481 + 6;
_temp1482;})));}} else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct Cyc_List_List*
ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct*
_temp1483=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1483[ 0]=({ struct Cyc_Core_Failure_struct _temp1484; _temp1484.tag= Cyc_Core_Failure_tag;
_temp1484.f1=({ struct _tagged_string _temp1485= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with no args", _temp1485.last_plus_one -
_temp1485.curr, _temp1485.curr);}); _temp1484;}); _temp1483;}));} else{ if(({
struct Cyc_List_List* _temp1486= ds; if( _temp1486 == 0){ _throw( Null_Exception);}
_temp1486->tl;}) == 0){ return Cyc_Absynpp_cat3( ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1487=( char*)" "; struct _tagged_string _temp1488;
_temp1488.curr= _temp1487; _temp1488.base= _temp1487; _temp1488.last_plus_one=
_temp1487 + 2; _temp1488;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1489= ds; if( _temp1489 == 0){ _throw( Null_Exception);} _temp1489->hd;}));}
else{ if(({ struct Cyc_List_List* _temp1491=({ struct Cyc_List_List* _temp1490=
ds; if( _temp1490 == 0){ _throw( Null_Exception);} _temp1490->tl;}); if(
_temp1491 == 0){ _throw( Null_Exception);} _temp1491->tl;}) != 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp1492=( struct
Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1492[ 0]=({ struct Cyc_Core_Failure_struct _temp1493; _temp1493.tag= Cyc_Core_Failure_tag;
_temp1493.f1=({ struct _tagged_string _temp1494= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with more than 2 args", _temp1494.last_plus_one
- _temp1494.curr, _temp1494.curr);}); _temp1493;}); _temp1492;}));} else{ return
Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp1495= ds;
if( _temp1495 == 0){ _throw( Null_Exception);} _temp1495->hd;}), Cyc_PP_text((
struct _tagged_string)({ char* _temp1496=( char*)" "; struct _tagged_string
_temp1497; _temp1497.curr= _temp1496; _temp1497.base= _temp1496; _temp1497.last_plus_one=
_temp1496 + 2; _temp1497;})), ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1498=( char*)" "; struct _tagged_string _temp1499; _temp1499.curr=
_temp1498; _temp1499.base= _temp1498; _temp1499.last_plus_one= _temp1498 + 2;
_temp1499;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp1501=({ struct
Cyc_List_List* _temp1500= ds; if( _temp1500 == 0){ _throw( Null_Exception);}
_temp1500->tl;}); if( _temp1501 == 0){ _throw( Null_Exception);} _temp1501->hd;}));}}}}
else{ return Cyc_Absynpp_cat4( ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1502=( char*)"("; struct _tagged_string _temp1503; _temp1503.curr=
_temp1502; _temp1503.base= _temp1502; _temp1503.last_plus_one= _temp1502 + 2;
_temp1503;})), Cyc_Absynpp_exps2doc_prec( 20, es), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1504=( char*)")"; struct _tagged_string _temp1505;
_temp1505.curr= _temp1504; _temp1505.base= _temp1504; _temp1505.last_plus_one=
_temp1504 + 2; _temp1505;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1506= p; _LL1508: if(( int) _temp1506 == Cyc_Absyn_Plus){
goto _LL1509;} else{ goto _LL1510;} _LL1510: if(( int) _temp1506 == Cyc_Absyn_Times){
goto _LL1511;} else{ goto _LL1512;} _LL1512: if(( int) _temp1506 == Cyc_Absyn_Minus){
goto _LL1513;} else{ goto _LL1514;} _LL1514: if(( int) _temp1506 == Cyc_Absyn_Div){
goto _LL1515;} else{ goto _LL1516;} _LL1516: if(( int) _temp1506 == Cyc_Absyn_Mod){
goto _LL1517;} else{ goto _LL1518;} _LL1518: if(( int) _temp1506 == Cyc_Absyn_Eq){
goto _LL1519;} else{ goto _LL1520;} _LL1520: if(( int) _temp1506 == Cyc_Absyn_Neq){
goto _LL1521;} else{ goto _LL1522;} _LL1522: if(( int) _temp1506 == Cyc_Absyn_Gt){
goto _LL1523;} else{ goto _LL1524;} _LL1524: if(( int) _temp1506 == Cyc_Absyn_Lt){
goto _LL1525;} else{ goto _LL1526;} _LL1526: if(( int) _temp1506 == Cyc_Absyn_Gte){
goto _LL1527;} else{ goto _LL1528;} _LL1528: if(( int) _temp1506 == Cyc_Absyn_Lte){
goto _LL1529;} else{ goto _LL1530;} _LL1530: if(( int) _temp1506 == Cyc_Absyn_Not){
goto _LL1531;} else{ goto _LL1532;} _LL1532: if(( int) _temp1506 == Cyc_Absyn_Bitnot){
goto _LL1533;} else{ goto _LL1534;} _LL1534: if(( int) _temp1506 == Cyc_Absyn_Bitand){
goto _LL1535;} else{ goto _LL1536;} _LL1536: if(( int) _temp1506 == Cyc_Absyn_Bitor){
goto _LL1537;} else{ goto _LL1538;} _LL1538: if(( int) _temp1506 == Cyc_Absyn_Bitxor){
goto _LL1539;} else{ goto _LL1540;} _LL1540: if(( int) _temp1506 == Cyc_Absyn_Bitlshift){
goto _LL1541;} else{ goto _LL1542;} _LL1542: if(( int) _temp1506 == Cyc_Absyn_Bitlrshift){
goto _LL1543;} else{ goto _LL1544;} _LL1544: if(( int) _temp1506 == Cyc_Absyn_Bitarshift){
goto _LL1545;} else{ goto _LL1546;} _LL1546: if(( int) _temp1506 == Cyc_Absyn_Size){
goto _LL1547;} else{ goto _LL1548;} _LL1548: if(( int) _temp1506 == Cyc_Absyn_Printf){
goto _LL1549;} else{ goto _LL1550;} _LL1550: if(( int) _temp1506 == Cyc_Absyn_Fprintf){
goto _LL1551;} else{ goto _LL1552;} _LL1552: if(( int) _temp1506 == Cyc_Absyn_Xprintf){
goto _LL1553;} else{ goto _LL1554;} _LL1554: if(( int) _temp1506 == Cyc_Absyn_Scanf){
goto _LL1555;} else{ goto _LL1556;} _LL1556: if(( int) _temp1506 == Cyc_Absyn_Fscanf){
goto _LL1557;} else{ goto _LL1558;} _LL1558: if(( int) _temp1506 == Cyc_Absyn_Sscanf){
goto _LL1559;} else{ goto _LL1507;} _LL1509: return( struct _tagged_string)({
char* _temp1560=( char*)"+"; struct _tagged_string _temp1561; _temp1561.curr=
_temp1560; _temp1561.base= _temp1560; _temp1561.last_plus_one= _temp1560 + 2;
_temp1561;}); _LL1511: return( struct _tagged_string)({ char* _temp1562=( char*)"*";
struct _tagged_string _temp1563; _temp1563.curr= _temp1562; _temp1563.base=
_temp1562; _temp1563.last_plus_one= _temp1562 + 2; _temp1563;}); _LL1513: return(
struct _tagged_string)({ char* _temp1564=( char*)"-"; struct _tagged_string
_temp1565; _temp1565.curr= _temp1564; _temp1565.base= _temp1564; _temp1565.last_plus_one=
_temp1564 + 2; _temp1565;}); _LL1515: return( struct _tagged_string)({ char*
_temp1566=( char*)"/"; struct _tagged_string _temp1567; _temp1567.curr=
_temp1566; _temp1567.base= _temp1566; _temp1567.last_plus_one= _temp1566 + 2;
_temp1567;}); _LL1517: return( struct _tagged_string)({ char* _temp1568=( char*)"%";
struct _tagged_string _temp1569; _temp1569.curr= _temp1568; _temp1569.base=
_temp1568; _temp1569.last_plus_one= _temp1568 + 2; _temp1569;}); _LL1519: return(
struct _tagged_string)({ char* _temp1570=( char*)"=="; struct _tagged_string
_temp1571; _temp1571.curr= _temp1570; _temp1571.base= _temp1570; _temp1571.last_plus_one=
_temp1570 + 3; _temp1571;}); _LL1521: return( struct _tagged_string)({ char*
_temp1572=( char*)"!="; struct _tagged_string _temp1573; _temp1573.curr=
_temp1572; _temp1573.base= _temp1572; _temp1573.last_plus_one= _temp1572 + 3;
_temp1573;}); _LL1523: return( struct _tagged_string)({ char* _temp1574=( char*)">";
struct _tagged_string _temp1575; _temp1575.curr= _temp1574; _temp1575.base=
_temp1574; _temp1575.last_plus_one= _temp1574 + 2; _temp1575;}); _LL1525: return(
struct _tagged_string)({ char* _temp1576=( char*)"<"; struct _tagged_string
_temp1577; _temp1577.curr= _temp1576; _temp1577.base= _temp1576; _temp1577.last_plus_one=
_temp1576 + 2; _temp1577;}); _LL1527: return( struct _tagged_string)({ char*
_temp1578=( char*)">="; struct _tagged_string _temp1579; _temp1579.curr=
_temp1578; _temp1579.base= _temp1578; _temp1579.last_plus_one= _temp1578 + 3;
_temp1579;}); _LL1529: return( struct _tagged_string)({ char* _temp1580=( char*)"<=";
struct _tagged_string _temp1581; _temp1581.curr= _temp1580; _temp1581.base=
_temp1580; _temp1581.last_plus_one= _temp1580 + 3; _temp1581;}); _LL1531: return(
struct _tagged_string)({ char* _temp1582=( char*)"!"; struct _tagged_string
_temp1583; _temp1583.curr= _temp1582; _temp1583.base= _temp1582; _temp1583.last_plus_one=
_temp1582 + 2; _temp1583;}); _LL1533: return( struct _tagged_string)({ char*
_temp1584=( char*)"~"; struct _tagged_string _temp1585; _temp1585.curr=
_temp1584; _temp1585.base= _temp1584; _temp1585.last_plus_one= _temp1584 + 2;
_temp1585;}); _LL1535: return( struct _tagged_string)({ char* _temp1586=( char*)"&";
struct _tagged_string _temp1587; _temp1587.curr= _temp1586; _temp1587.base=
_temp1586; _temp1587.last_plus_one= _temp1586 + 2; _temp1587;}); _LL1537: return(
struct _tagged_string)({ char* _temp1588=( char*)"|"; struct _tagged_string
_temp1589; _temp1589.curr= _temp1588; _temp1589.base= _temp1588; _temp1589.last_plus_one=
_temp1588 + 2; _temp1589;}); _LL1539: return( struct _tagged_string)({ char*
_temp1590=( char*)"^"; struct _tagged_string _temp1591; _temp1591.curr=
_temp1590; _temp1591.base= _temp1590; _temp1591.last_plus_one= _temp1590 + 2;
_temp1591;}); _LL1541: return( struct _tagged_string)({ char* _temp1592=( char*)"<<";
struct _tagged_string _temp1593; _temp1593.curr= _temp1592; _temp1593.base=
_temp1592; _temp1593.last_plus_one= _temp1592 + 3; _temp1593;}); _LL1543: return(
struct _tagged_string)({ char* _temp1594=( char*)">>"; struct _tagged_string
_temp1595; _temp1595.curr= _temp1594; _temp1595.base= _temp1594; _temp1595.last_plus_one=
_temp1594 + 3; _temp1595;}); _LL1545: return( struct _tagged_string)({ char*
_temp1596=( char*)">>>"; struct _tagged_string _temp1597; _temp1597.curr=
_temp1596; _temp1597.base= _temp1596; _temp1597.last_plus_one= _temp1596 + 4;
_temp1597;}); _LL1547: return( struct _tagged_string)({ char* _temp1598=( char*)"size";
struct _tagged_string _temp1599; _temp1599.curr= _temp1598; _temp1599.base=
_temp1598; _temp1599.last_plus_one= _temp1598 + 5; _temp1599;}); _LL1549: return(
struct _tagged_string)({ char* _temp1600=( char*)"printf"; struct _tagged_string
_temp1601; _temp1601.curr= _temp1600; _temp1601.base= _temp1600; _temp1601.last_plus_one=
_temp1600 + 7; _temp1601;}); _LL1551: return( struct _tagged_string)({ char*
_temp1602=( char*)"fprintf"; struct _tagged_string _temp1603; _temp1603.curr=
_temp1602; _temp1603.base= _temp1602; _temp1603.last_plus_one= _temp1602 + 8;
_temp1603;}); _LL1553: return( struct _tagged_string)({ char* _temp1604=( char*)"xprintf";
struct _tagged_string _temp1605; _temp1605.curr= _temp1604; _temp1605.base=
_temp1604; _temp1605.last_plus_one= _temp1604 + 8; _temp1605;}); _LL1555: return(
struct _tagged_string)({ char* _temp1606=( char*)"scanf"; struct _tagged_string
_temp1607; _temp1607.curr= _temp1606; _temp1607.base= _temp1606; _temp1607.last_plus_one=
_temp1606 + 6; _temp1607;}); _LL1557: return( struct _tagged_string)({ char*
_temp1608=( char*)"fscanf"; struct _tagged_string _temp1609; _temp1609.curr=
_temp1608; _temp1609.base= _temp1608; _temp1609.last_plus_one= _temp1608 + 7;
_temp1609;}); _LL1559: return( struct _tagged_string)({ char* _temp1610=( char*)"sscanf";
struct _tagged_string _temp1611; _temp1611.curr= _temp1610; _temp1611.base=
_temp1610; _temp1611.last_plus_one= _temp1610 + 7; _temp1611;}); _LL1507:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1
== 0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp1612=(* t).f1; if( _temp1612
== 0){ _throw( Null_Exception);} _temp1612->v;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1613=( char*)" = "; struct _tagged_string _temp1614;
_temp1614.curr= _temp1613; _temp1614.base= _temp1613; _temp1614.last_plus_one=
_temp1613 + 4; _temp1614;})), Cyc_Absynpp_exp2doc((* t).f2));}} struct Cyc_PP_Doc*
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
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1615=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1621; struct Cyc_Absyn_Decl* _temp1623;
_LL1617: if(( unsigned int) _temp1615 > 1u?(( struct _tunion_struct*) _temp1615)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL1624: _temp1623=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1615)->f1; goto _LL1622; _LL1622:
_temp1621=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1615)->f2;
goto _LL1618;} else{ goto _LL1619;} _LL1619: goto _LL1620; _LL1618: return 1;
_LL1620: return 0; _LL1616:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1625=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1665; struct Cyc_Absyn_Stmt* _temp1667; struct Cyc_Absyn_Stmt*
_temp1669; struct Cyc_Absyn_Exp* _temp1671; struct Cyc_Absyn_Stmt* _temp1673;
struct Cyc_Absyn_Stmt* _temp1675; struct Cyc_Absyn_Exp* _temp1677; struct Cyc_Absyn_Stmt*
_temp1679; struct _tuple2 _temp1681; struct Cyc_Absyn_Stmt* _temp1683; struct
Cyc_Absyn_Exp* _temp1685; struct Cyc_Absyn_Stmt* _temp1687; struct Cyc_Absyn_Stmt*
_temp1689; struct Cyc_Absyn_Stmt* _temp1691; struct _tagged_string* _temp1693;
struct Cyc_Absyn_Stmt* _temp1695; struct _tuple2 _temp1697; struct Cyc_Absyn_Stmt*
_temp1699; struct Cyc_Absyn_Exp* _temp1701; struct _tuple2 _temp1703; struct Cyc_Absyn_Stmt*
_temp1705; struct Cyc_Absyn_Exp* _temp1707; struct Cyc_Absyn_Exp* _temp1709;
struct Cyc_List_List* _temp1711; struct Cyc_Absyn_Exp* _temp1713; struct Cyc_Absyn_Switch_clause**
_temp1715; struct Cyc_List_List* _temp1717; struct Cyc_Absyn_Switch_clause**
_temp1719; struct Cyc_List_List* _temp1721; struct Cyc_Absyn_Stmt* _temp1723;
struct Cyc_Absyn_Decl* _temp1725; struct Cyc_Absyn_Stmt* _temp1727; struct Cyc_Absyn_Stmt*
_temp1729; struct Cyc_Absyn_Stmt* _temp1731; struct _tagged_string* _temp1733;
struct _tuple2 _temp1735; struct Cyc_Absyn_Stmt* _temp1737; struct Cyc_Absyn_Exp*
_temp1739; struct Cyc_Absyn_Stmt* _temp1741; struct Cyc_List_List* _temp1743;
struct Cyc_Absyn_Stmt* _temp1745; _LL1627: if(( int) _temp1625 == Cyc_Absyn_Skip_s){
goto _LL1628;} else{ goto _LL1629;} _LL1629: if(( unsigned int) _temp1625 > 1u?((
struct _tunion_struct*) _temp1625)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1666:
_temp1665=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1625)->f1;
goto _LL1630;} else{ goto _LL1631;} _LL1631: if(( unsigned int) _temp1625 > 1u?((
struct _tunion_struct*) _temp1625)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1670:
_temp1669=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1625)->f1;
goto _LL1668; _LL1668: _temp1667=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1625)->f2; goto _LL1632;} else{ goto _LL1633;} _LL1633: if(( unsigned int)
_temp1625 > 1u?(( struct _tunion_struct*) _temp1625)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1672: _temp1671=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1625)->f1; goto _LL1634;} else{ goto _LL1635;} _LL1635: if(( unsigned int)
_temp1625 > 1u?(( struct _tunion_struct*) _temp1625)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1678: _temp1677=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1625)->f1; goto _LL1676; _LL1676: _temp1675=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1625)->f2; goto _LL1674; _LL1674:
_temp1673=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1625)->f3; goto _LL1636;} else{ goto _LL1637;} _LL1637: if(( unsigned int)
_temp1625 > 1u?(( struct _tunion_struct*) _temp1625)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1682: _temp1681=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*)
_temp1625)->f1; _LL1686: _temp1685= _temp1681.f1; goto _LL1684; _LL1684:
_temp1683= _temp1681.f2; goto _LL1680; _LL1680: _temp1679=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1625)->f2; goto _LL1638;} else{ goto
_LL1639;} _LL1639: if(( unsigned int) _temp1625 > 1u?(( struct _tunion_struct*)
_temp1625)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1688: _temp1687=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1625)->f1; goto _LL1640;} else{ goto
_LL1641;} _LL1641: if(( unsigned int) _temp1625 > 1u?(( struct _tunion_struct*)
_temp1625)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1690: _temp1689=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1625)->f1; goto _LL1642;} else{ goto
_LL1643;} _LL1643: if(( unsigned int) _temp1625 > 1u?(( struct _tunion_struct*)
_temp1625)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1694: _temp1693=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1625)->f1; goto _LL1692;
_LL1692: _temp1691=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1625)->f2; goto _LL1644;} else{ goto _LL1645;} _LL1645: if(( unsigned int)
_temp1625 > 1u?(( struct _tunion_struct*) _temp1625)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1710: _temp1709=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1625)->f1; goto _LL1704; _LL1704: _temp1703=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp1625)->f2; _LL1708: _temp1707= _temp1703.f1; goto _LL1706; _LL1706:
_temp1705= _temp1703.f2; goto _LL1698; _LL1698: _temp1697=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp1625)->f3; _LL1702: _temp1701= _temp1697.f1;
goto _LL1700; _LL1700: _temp1699= _temp1697.f2; goto _LL1696; _LL1696: _temp1695=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1625)->f4; goto
_LL1646;} else{ goto _LL1647;} _LL1647: if(( unsigned int) _temp1625 > 1u?((
struct _tunion_struct*) _temp1625)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1714:
_temp1713=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1625)->f1; goto _LL1712; _LL1712: _temp1711=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1625)->f2; goto _LL1648;} else{ goto
_LL1649;} _LL1649: if(( unsigned int) _temp1625 > 1u?(( struct _tunion_struct*)
_temp1625)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1718: _temp1717=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1625)->f1; if( _temp1717 == 0){ goto
_LL1716;} else{ goto _LL1651;} _LL1716: _temp1715=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1625)->f2; goto _LL1650;} else{ goto
_LL1651;} _LL1651: if(( unsigned int) _temp1625 > 1u?(( struct _tunion_struct*)
_temp1625)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1722: _temp1721=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1625)->f1; goto _LL1720; _LL1720:
_temp1719=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1625)->f2; goto _LL1652;} else{ goto _LL1653;} _LL1653: if(( unsigned int)
_temp1625 > 1u?(( struct _tunion_struct*) _temp1625)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1726: _temp1725=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1625)->f1; goto _LL1724; _LL1724: _temp1723=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1625)->f2; goto _LL1654;} else{ goto
_LL1655;} _LL1655: if(( unsigned int) _temp1625 > 1u?(( struct _tunion_struct*)
_temp1625)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1728: _temp1727=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1625)->f1; goto _LL1656;} else{ goto
_LL1657;} _LL1657: if(( unsigned int) _temp1625 > 1u?(( struct _tunion_struct*)
_temp1625)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1730: _temp1729=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1625)->f1; goto _LL1658;} else{ goto
_LL1659;} _LL1659: if(( unsigned int) _temp1625 > 1u?(( struct _tunion_struct*)
_temp1625)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1734: _temp1733=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1625)->f1; goto
_LL1732; _LL1732: _temp1731=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1625)->f2; goto _LL1660;} else{ goto _LL1661;} _LL1661: if(( unsigned int)
_temp1625 > 1u?(( struct _tunion_struct*) _temp1625)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1742: _temp1741=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1625)->f1; goto _LL1736; _LL1736: _temp1735=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp1625)->f2; _LL1740: _temp1739= _temp1735.f1; goto _LL1738; _LL1738:
_temp1737= _temp1735.f2; goto _LL1662;} else{ goto _LL1663;} _LL1663: if((
unsigned int) _temp1625 > 1u?(( struct _tunion_struct*) _temp1625)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1746: _temp1745=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1625)->f1; goto _LL1744; _LL1744: _temp1743=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1625)->f2; goto _LL1664;} else{ goto
_LL1626;} _LL1628: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1747=(
char*)";"; struct _tagged_string _temp1748; _temp1748.curr= _temp1747; _temp1748.base=
_temp1747; _temp1748.last_plus_one= _temp1747 + 2; _temp1748;})); goto _LL1626;
_LL1630: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc( _temp1665), Cyc_PP_text((
struct _tagged_string)({ char* _temp1749=( char*)";"; struct _tagged_string
_temp1750; _temp1750.curr= _temp1749; _temp1750.base= _temp1749; _temp1750.last_plus_one=
_temp1749 + 2; _temp1750;}))); goto _LL1626; _LL1632: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1669)){ s= Cyc_Absynpp_cat6( Cyc_PP_text((
struct _tagged_string)({ char* _temp1751=( char*)"{ "; struct _tagged_string
_temp1752; _temp1752.curr= _temp1751; _temp1752.base= _temp1751; _temp1752.last_plus_one=
_temp1751 + 3; _temp1752;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1669)),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1753=( char*)"}";
struct _tagged_string _temp1754; _temp1754.curr= _temp1753; _temp1754.base=
_temp1753; _temp1754.last_plus_one= _temp1753 + 2; _temp1754;})), Cyc_PP_line_doc(),
Cyc_Absynpp_is_declaration( _temp1667)? Cyc_Absynpp_cat4( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1755=( char*)"{ "; struct _tagged_string _temp1756;
_temp1756.curr= _temp1755; _temp1756.base= _temp1755; _temp1756.last_plus_one=
_temp1755 + 3; _temp1756;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1667)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1757=( char*)"}"; struct
_tagged_string _temp1758; _temp1758.curr= _temp1757; _temp1758.base= _temp1757;
_temp1758.last_plus_one= _temp1757 + 2; _temp1758;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1667));} else{ if( Cyc_Absynpp_is_declaration( _temp1667)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1669), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1759=( char*)"{ "; struct _tagged_string _temp1760;
_temp1760.curr= _temp1759; _temp1760.base= _temp1759; _temp1760.last_plus_one=
_temp1759 + 3; _temp1760;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1667)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1761=( char*)"}"; struct
_tagged_string _temp1762; _temp1762.curr= _temp1761; _temp1762.base= _temp1761;
_temp1762.last_plus_one= _temp1761 + 2; _temp1762;})), Cyc_PP_line_doc());}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,(
struct _tagged_string)({ char* _temp1763=( char*)""; struct _tagged_string
_temp1764; _temp1764.curr= _temp1763; _temp1764.base= _temp1763; _temp1764.last_plus_one=
_temp1763 + 1; _temp1764;}),({ struct Cyc_List_List* _temp1765=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1765->hd=( void*) _temp1669;
_temp1765->tl=({ struct Cyc_List_List* _temp1766=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1766->hd=( void*) _temp1667;
_temp1766->tl= 0; _temp1766;}); _temp1765;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,( struct
_tagged_string)({ char* _temp1767=( char*)""; struct _tagged_string _temp1768;
_temp1768.curr= _temp1767; _temp1768.base= _temp1767; _temp1768.last_plus_one=
_temp1767 + 1; _temp1768;}),({ struct Cyc_List_List* _temp1769=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1769->hd=( void*) _temp1669;
_temp1769->tl=({ struct Cyc_List_List* _temp1770=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1770->hd=( void*) _temp1667;
_temp1770->tl= 0; _temp1770;}); _temp1769;}));} goto _LL1626; _LL1634: if(
_temp1671 == 0){ s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1771=(
char*)"return;"; struct _tagged_string _temp1772; _temp1772.curr= _temp1771;
_temp1772.base= _temp1771; _temp1772.last_plus_one= _temp1771 + 8; _temp1772;}));}
else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1773=( char*)"return "; struct _tagged_string _temp1774; _temp1774.curr=
_temp1773; _temp1774.base= _temp1773; _temp1774.last_plus_one= _temp1773 + 8;
_temp1774;})), _temp1671 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1775= _temp1671; if( _temp1775 == 0){
_throw( Null_Exception);} _temp1775;})), Cyc_PP_text(( struct _tagged_string)({
char* _temp1776=( char*)";"; struct _tagged_string _temp1777; _temp1777.curr=
_temp1776; _temp1777.base= _temp1776; _temp1777.last_plus_one= _temp1776 + 2;
_temp1777;})));} goto _LL1626; _LL1636: { int print_else;{ void* _temp1778=(
void*) _temp1673->r; _LL1780: if(( int) _temp1778 == Cyc_Absyn_Skip_s){ goto
_LL1781;} else{ goto _LL1782;} _LL1782: goto _LL1783; _LL1781: print_else= 0;
goto _LL1779; _LL1783: print_else= 1; goto _LL1779; _LL1779:;} s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1784=( char*)"if ("; struct
_tagged_string _temp1785; _temp1785.curr= _temp1784; _temp1785.base= _temp1784;
_temp1785.last_plus_one= _temp1784 + 5; _temp1785;})), Cyc_Absynpp_exp2doc(
_temp1677), Cyc_PP_text(( struct _tagged_string)({ char* _temp1786=( char*)") {";
struct _tagged_string _temp1787; _temp1787.curr= _temp1786; _temp1787.base=
_temp1786; _temp1787.last_plus_one= _temp1786 + 4; _temp1787;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1675))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1788=( char*)"}"; struct
_tagged_string _temp1789; _temp1789.curr= _temp1788; _temp1789.base= _temp1788;
_temp1789.last_plus_one= _temp1788 + 2; _temp1789;})), print_else? Cyc_Absynpp_cat5(
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1790=( char*)"else {";
struct _tagged_string _temp1791; _temp1791.curr= _temp1790; _temp1791.base=
_temp1790; _temp1791.last_plus_one= _temp1790 + 7; _temp1791;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1673))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1792=( char*)"}"; struct
_tagged_string _temp1793; _temp1793.curr= _temp1792; _temp1793.base= _temp1792;
_temp1793.last_plus_one= _temp1792 + 2; _temp1793;}))): Cyc_PP_nil_doc()); goto
_LL1626;} _LL1638: s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({
char* _temp1794=( char*)"while ("; struct _tagged_string _temp1795; _temp1795.curr=
_temp1794; _temp1795.base= _temp1794; _temp1795.last_plus_one= _temp1794 + 8;
_temp1795;})), Cyc_Absynpp_exp2doc( _temp1685), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1796=( char*)") {"; struct _tagged_string _temp1797;
_temp1797.curr= _temp1796; _temp1797.base= _temp1796; _temp1797.last_plus_one=
_temp1796 + 4; _temp1797;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1679))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1798=( char*)"}"; struct _tagged_string _temp1799;
_temp1799.curr= _temp1798; _temp1799.base= _temp1798; _temp1799.last_plus_one=
_temp1798 + 2; _temp1799;}))); goto _LL1626; _LL1640: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1800=( char*)"break;"; struct _tagged_string
_temp1801; _temp1801.curr= _temp1800; _temp1801.base= _temp1800; _temp1801.last_plus_one=
_temp1800 + 7; _temp1801;})); goto _LL1626; _LL1642: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1802=( char*)"continue;"; struct _tagged_string
_temp1803; _temp1803.curr= _temp1802; _temp1803.base= _temp1802; _temp1803.last_plus_one=
_temp1802 + 10; _temp1803;})); goto _LL1626; _LL1644: s= Cyc_PP_text(({ struct
_tagged_string _temp1804=* _temp1693; xprintf("goto %.*s;", _temp1804.last_plus_one
- _temp1804.curr, _temp1804.curr);})); goto _LL1626; _LL1646: s= Cyc_Absynpp_cat10(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1805=( char*)"for("; struct
_tagged_string _temp1806; _temp1806.curr= _temp1805; _temp1806.base= _temp1805;
_temp1806.last_plus_one= _temp1805 + 5; _temp1806;})), Cyc_Absynpp_exp2doc(
_temp1709), Cyc_PP_text(( struct _tagged_string)({ char* _temp1807=( char*)"; ";
struct _tagged_string _temp1808; _temp1808.curr= _temp1807; _temp1808.base=
_temp1807; _temp1808.last_plus_one= _temp1807 + 3; _temp1808;})), Cyc_Absynpp_exp2doc(
_temp1707), Cyc_PP_text(( struct _tagged_string)({ char* _temp1809=( char*)"; ";
struct _tagged_string _temp1810; _temp1810.curr= _temp1809; _temp1810.base=
_temp1809; _temp1810.last_plus_one= _temp1809 + 3; _temp1810;})), Cyc_Absynpp_exp2doc(
_temp1701), Cyc_PP_text(( struct _tagged_string)({ char* _temp1811=( char*)") {";
struct _tagged_string _temp1812; _temp1812.curr= _temp1811; _temp1812.base=
_temp1811; _temp1812.last_plus_one= _temp1811 + 4; _temp1812;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1695))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1813=( char*)"}"; struct
_tagged_string _temp1814; _temp1814.curr= _temp1813; _temp1814.base= _temp1813;
_temp1814.last_plus_one= _temp1813 + 2; _temp1814;}))); goto _LL1626; _LL1648: s=
Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char* _temp1815=( char*)"switch (";
struct _tagged_string _temp1816; _temp1816.curr= _temp1815; _temp1816.base=
_temp1815; _temp1816.last_plus_one= _temp1815 + 9; _temp1816;})), Cyc_Absynpp_exp2doc(
_temp1713), Cyc_PP_text(( struct _tagged_string)({ char* _temp1817=( char*)") {";
struct _tagged_string _temp1818; _temp1818.curr= _temp1817; _temp1818.base=
_temp1817; _temp1818.last_plus_one= _temp1817 + 4; _temp1818;})), Cyc_PP_line_doc(),
Cyc_Absynpp_switchclauses2doc( _temp1711), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1819=( char*)"}"; struct _tagged_string
_temp1820; _temp1820.curr= _temp1819; _temp1820.base= _temp1819; _temp1820.last_plus_one=
_temp1819 + 2; _temp1820;}))); goto _LL1626; _LL1650: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1821=( char*)"fallthru;"; struct _tagged_string
_temp1822; _temp1822.curr= _temp1821; _temp1822.base= _temp1821; _temp1822.last_plus_one=
_temp1821 + 10; _temp1822;})); goto _LL1626; _LL1652: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1823=( char*)"fallthru("; struct
_tagged_string _temp1824; _temp1824.curr= _temp1823; _temp1824.base= _temp1823;
_temp1824.last_plus_one= _temp1823 + 10; _temp1824;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1721), Cyc_PP_text(( struct _tagged_string)({ char* _temp1825=( char*)");";
struct _tagged_string _temp1826; _temp1826.curr= _temp1825; _temp1826.base=
_temp1825; _temp1826.last_plus_one= _temp1825 + 3; _temp1826;}))); goto _LL1626;
_LL1654: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1725), Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1723)); goto _LL1626; _LL1656: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1827=( char*)"cut "; struct
_tagged_string _temp1828; _temp1828.curr= _temp1827; _temp1828.base= _temp1827;
_temp1828.last_plus_one= _temp1827 + 5; _temp1828;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1727))); goto _LL1626; _LL1658: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1829=( char*)"splice "; struct _tagged_string
_temp1830; _temp1830.curr= _temp1829; _temp1830.base= _temp1829; _temp1830.last_plus_one=
_temp1829 + 8; _temp1830;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1729)));
goto _LL1626; _LL1660: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1731): 0){ s= Cyc_Absynpp_cat6( Cyc_Absynpp_textptr( _temp1733), Cyc_PP_text((
struct _tagged_string)({ char* _temp1831=( char*)": {"; struct _tagged_string
_temp1832; _temp1832.curr= _temp1831; _temp1832.base= _temp1831; _temp1832.last_plus_one=
_temp1831 + 4; _temp1832;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1731)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1833=( char*)"}"; struct _tagged_string _temp1834; _temp1834.curr=
_temp1833; _temp1834.base= _temp1833; _temp1834.last_plus_one= _temp1833 + 2;
_temp1834;})));} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( _temp1733), Cyc_PP_text((
struct _tagged_string)({ char* _temp1835=( char*)": "; struct _tagged_string
_temp1836; _temp1836.curr= _temp1835; _temp1836.base= _temp1835; _temp1836.last_plus_one=
_temp1835 + 3; _temp1836;})), Cyc_Absynpp_stmt2doc( _temp1731));} goto _LL1626;
_LL1662: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1837=( char*)"do {"; struct _tagged_string _temp1838; _temp1838.curr=
_temp1837; _temp1838.base= _temp1837; _temp1838.last_plus_one= _temp1837 + 5;
_temp1838;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1741)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1839=( char*)"} while ("; struct _tagged_string _temp1840; _temp1840.curr=
_temp1839; _temp1840.base= _temp1839; _temp1840.last_plus_one= _temp1839 + 10;
_temp1840;})), Cyc_Absynpp_exp2doc( _temp1739), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1841=( char*)");"; struct _tagged_string _temp1842;
_temp1842.curr= _temp1841; _temp1842.base= _temp1841; _temp1842.last_plus_one=
_temp1841 + 3; _temp1842;}))); goto _LL1626; _LL1664: s= Cyc_Absynpp_cat9( Cyc_PP_text((
struct _tagged_string)({ char* _temp1843=( char*)"try {"; struct _tagged_string
_temp1844; _temp1844.curr= _temp1843; _temp1844.base= _temp1843; _temp1844.last_plus_one=
_temp1843 + 6; _temp1844;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1745)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1845=( char*)"} catch {"; struct _tagged_string _temp1846; _temp1846.curr=
_temp1845; _temp1846.base= _temp1845; _temp1846.last_plus_one= _temp1845 + 10;
_temp1846;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1743)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1847=( char*)"}"; struct _tagged_string _temp1848; _temp1848.curr=
_temp1847; _temp1848.base= _temp1847; _temp1848.last_plus_one= _temp1847 + 2;
_temp1848;}))); goto _LL1626; _LL1626:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1849=( void*) p->r;
int _temp1883; void* _temp1885; char _temp1887; struct _tagged_string _temp1889;
struct Cyc_Absyn_Vardecl* _temp1891; struct Cyc_List_List* _temp1893; struct Cyc_Absyn_Pat*
_temp1895; struct Cyc_Absyn_Vardecl* _temp1897; struct _tuple0* _temp1899;
struct Cyc_List_List* _temp1901; struct Cyc_List_List* _temp1903; struct _tuple0*
_temp1905; struct Cyc_List_List* _temp1907; struct Cyc_List_List* _temp1909;
struct _tuple0* _temp1911; struct Cyc_List_List* _temp1913; struct Cyc_List_List*
_temp1915; struct Cyc_Core_Opt* _temp1917; struct Cyc_Absyn_Structdecl*
_temp1919; struct Cyc_Absyn_Enumfield* _temp1921; struct Cyc_Absyn_Enumdecl*
_temp1923; struct _tuple0* _temp1925; struct Cyc_Absyn_Tunionfield* _temp1927;
struct Cyc_Absyn_Tuniondecl* _temp1929; struct Cyc_List_List* _temp1931; struct
Cyc_List_List* _temp1933; struct Cyc_Core_Opt* _temp1935; struct _tuple0*
_temp1937; struct Cyc_Absyn_Tunionfield* _temp1939; struct Cyc_Absyn_XTuniondecl*
_temp1941; struct Cyc_List_List* _temp1943; struct Cyc_List_List* _temp1945;
struct _tuple0* _temp1947; _LL1851: if(( int) _temp1849 == Cyc_Absyn_Wild_p){
goto _LL1852;} else{ goto _LL1853;} _LL1853: if(( int) _temp1849 == Cyc_Absyn_Null_p){
goto _LL1854;} else{ goto _LL1855;} _LL1855: if(( unsigned int) _temp1849 > 2u?((
struct _tunion_struct*) _temp1849)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1886:
_temp1885=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1849)->f1; goto
_LL1884; _LL1884: _temp1883=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp1849)->f2;
goto _LL1856;} else{ goto _LL1857;} _LL1857: if(( unsigned int) _temp1849 > 2u?((
struct _tunion_struct*) _temp1849)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL1888:
_temp1887=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1849)->f1; goto
_LL1858;} else{ goto _LL1859;} _LL1859: if(( unsigned int) _temp1849 > 2u?((
struct _tunion_struct*) _temp1849)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1890:
_temp1889=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1849)->f1;
goto _LL1860;} else{ goto _LL1861;} _LL1861: if(( unsigned int) _temp1849 > 2u?((
struct _tunion_struct*) _temp1849)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1892:
_temp1891=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1849)->f1; goto _LL1862;} else{ goto _LL1863;} _LL1863: if(( unsigned int)
_temp1849 > 2u?(( struct _tunion_struct*) _temp1849)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1894: _temp1893=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1849)->f1; goto _LL1864;} else{ goto _LL1865;} _LL1865: if(( unsigned int)
_temp1849 > 2u?(( struct _tunion_struct*) _temp1849)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1896: _temp1895=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1849)->f1; goto _LL1866;} else{ goto _LL1867;} _LL1867: if(( unsigned int)
_temp1849 > 2u?(( struct _tunion_struct*) _temp1849)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1898: _temp1897=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1849)->f1; goto _LL1868;} else{ goto _LL1869;} _LL1869: if(( unsigned int)
_temp1849 > 2u?(( struct _tunion_struct*) _temp1849)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL1900: _temp1899=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1849)->f1; goto _LL1870;} else{ goto _LL1871;} _LL1871: if(( unsigned int)
_temp1849 > 2u?(( struct _tunion_struct*) _temp1849)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1906: _temp1905=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1849)->f1; goto _LL1904; _LL1904: _temp1903=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1849)->f2; goto _LL1902; _LL1902:
_temp1901=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1849)->f3; goto _LL1872;} else{ goto _LL1873;} _LL1873: if(( unsigned int)
_temp1849 > 2u?(( struct _tunion_struct*) _temp1849)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1912: _temp1911=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1849)->f1; goto _LL1910; _LL1910: _temp1909=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1849)->f2; goto _LL1908; _LL1908:
_temp1907=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1849)->f3; goto _LL1874;} else{ goto _LL1875;} _LL1875: if(( unsigned int)
_temp1849 > 2u?(( struct _tunion_struct*) _temp1849)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1920: _temp1919=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1849)->f1; goto _LL1918; _LL1918: _temp1917=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1849)->f2; goto _LL1916; _LL1916:
_temp1915=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1849)->f3; goto _LL1914; _LL1914: _temp1913=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1849)->f4; goto _LL1876;} else{ goto
_LL1877;} _LL1877: if(( unsigned int) _temp1849 > 2u?(( struct _tunion_struct*)
_temp1849)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1926: _temp1925=( struct _tuple0*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1849)->f1; goto _LL1924; _LL1924:
_temp1923=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1849)->f2; goto _LL1922; _LL1922: _temp1921=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1849)->f3; goto _LL1878;} else{ goto
_LL1879;} _LL1879: if(( unsigned int) _temp1849 > 2u?(( struct _tunion_struct*)
_temp1849)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1938: _temp1937=( struct
_tuple0*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1849)->f1; goto _LL1936;
_LL1936: _temp1935=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1849)->f2; goto _LL1934; _LL1934: _temp1933=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1849)->f3; goto _LL1932; _LL1932:
_temp1931=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1849)->f4; goto _LL1930; _LL1930: _temp1929=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1849)->f5; goto _LL1928; _LL1928:
_temp1927=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1849)->f6; goto _LL1880;} else{ goto _LL1881;} _LL1881: if(( unsigned int)
_temp1849 > 2u?(( struct _tunion_struct*) _temp1849)->tag == Cyc_Absyn_XTunion_p_tag:
0){ _LL1948: _temp1947=( struct _tuple0*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1849)->f1; goto _LL1946; _LL1946: _temp1945=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1849)->f2; goto _LL1944; _LL1944:
_temp1943=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1849)->f3; goto _LL1942; _LL1942: _temp1941=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1849)->f4; goto _LL1940; _LL1940:
_temp1939=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1849)->f5; goto _LL1882;} else{ goto _LL1850;} _LL1852: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp1949=( char*)"_"; struct _tagged_string
_temp1950; _temp1950.curr= _temp1949; _temp1950.base= _temp1949; _temp1950.last_plus_one=
_temp1949 + 2; _temp1950;})); goto _LL1850; _LL1854: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1951=( char*)"null"; struct _tagged_string
_temp1952; _temp1952.curr= _temp1951; _temp1952.base= _temp1951; _temp1952.last_plus_one=
_temp1951 + 5; _temp1952;})); goto _LL1850; _LL1856: if( _temp1885 ==( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d", _temp1883));} else{ s= Cyc_PP_text(
xprintf("%u",( unsigned int) _temp1883));} goto _LL1850; _LL1858: s= Cyc_PP_text(({
struct _tagged_string _temp1953= Cyc_Absynpp_char_escape( _temp1887); xprintf("'%.*s'",
_temp1953.last_plus_one - _temp1953.curr, _temp1953.curr);})); goto _LL1850;
_LL1860: s= Cyc_PP_text( _temp1889); goto _LL1850; _LL1862: s= Cyc_Absynpp_qvar2doc(
_temp1891->name); goto _LL1850; _LL1864: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1954=( char*)"$("; struct _tagged_string
_temp1955; _temp1955.curr= _temp1954; _temp1955.base= _temp1954; _temp1955.last_plus_one=
_temp1954 + 3; _temp1955;})),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Pat*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,( struct _tagged_string)({ char* _temp1956=(
char*)","; struct _tagged_string _temp1957; _temp1957.curr= _temp1956; _temp1957.base=
_temp1956; _temp1957.last_plus_one= _temp1956 + 2; _temp1957;}), _temp1893), Cyc_PP_text((
struct _tagged_string)({ char* _temp1958=( char*)")"; struct _tagged_string
_temp1959; _temp1959.curr= _temp1958; _temp1959.base= _temp1958; _temp1959.last_plus_one=
_temp1958 + 2; _temp1959;}))); goto _LL1850; _LL1866: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1960=( char*)"&"; struct _tagged_string
_temp1961; _temp1961.curr= _temp1960; _temp1961.base= _temp1960; _temp1961.last_plus_one=
_temp1960 + 2; _temp1961;})), Cyc_Absynpp_pat2doc( _temp1895)); goto _LL1850;
_LL1868: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1962=( char*)"*"; struct _tagged_string _temp1963; _temp1963.curr=
_temp1962; _temp1963.base= _temp1962; _temp1963.last_plus_one= _temp1962 + 2;
_temp1963;})), Cyc_Absynpp_qvar2doc( _temp1897->name)); goto _LL1850; _LL1870: s=
Cyc_Absynpp_qvar2doc( _temp1899); goto _LL1850; _LL1872: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1905), Cyc_Absynpp_tvars2doc( _temp1903), Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1964=( char*)"("; struct _tagged_string
_temp1965; _temp1965.curr= _temp1964; _temp1965.base= _temp1964; _temp1965.last_plus_one=
_temp1964 + 2; _temp1965;}),( struct _tagged_string)({ char* _temp1966=( char*)")";
struct _tagged_string _temp1967; _temp1967.curr= _temp1966; _temp1967.base=
_temp1966; _temp1967.last_plus_one= _temp1966 + 2; _temp1967;}),( struct
_tagged_string)({ char* _temp1968=( char*)","; struct _tagged_string _temp1969;
_temp1969.curr= _temp1968; _temp1969.base= _temp1968; _temp1969.last_plus_one=
_temp1968 + 2; _temp1969;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1901))); goto _LL1850; _LL1874: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1911), Cyc_Absynpp_tvars2doc( _temp1909), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1970=( char*)"{"; struct _tagged_string _temp1971;
_temp1971.curr= _temp1970; _temp1971.base= _temp1970; _temp1971.last_plus_one=
_temp1970 + 2; _temp1971;}),( struct _tagged_string)({ char* _temp1972=( char*)"}";
struct _tagged_string _temp1973; _temp1973.curr= _temp1972; _temp1973.base=
_temp1972; _temp1973.last_plus_one= _temp1972 + 2; _temp1973;}),( struct
_tagged_string)({ char* _temp1974=( char*)","; struct _tagged_string _temp1975;
_temp1975.curr= _temp1974; _temp1975.base= _temp1974; _temp1975.last_plus_one=
_temp1974 + 2; _temp1975;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1907))); goto _LL1850; _LL1876: s= Cyc_Absynpp_cat3( _temp1919->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp1976= _temp1919->name; if( _temp1976 == 0){ _throw( Null_Exception);}
_temp1976->v;})), Cyc_Absynpp_tvars2doc( _temp1915), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1977=( char*)"{"; struct _tagged_string _temp1978;
_temp1978.curr= _temp1977; _temp1978.base= _temp1977; _temp1978.last_plus_one=
_temp1977 + 2; _temp1978;}),( struct _tagged_string)({ char* _temp1979=( char*)"}";
struct _tagged_string _temp1980; _temp1980.curr= _temp1979; _temp1980.base=
_temp1979; _temp1980.last_plus_one= _temp1979 + 2; _temp1980;}),( struct
_tagged_string)({ char* _temp1981=( char*)","; struct _tagged_string _temp1982;
_temp1982.curr= _temp1981; _temp1982.base= _temp1981; _temp1982.last_plus_one=
_temp1981 + 2; _temp1982;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1913))); goto _LL1850; _LL1878: s= Cyc_Absynpp_qvar2doc( _temp1925); goto
_LL1850; _LL1880: _temp1947= _temp1937; _temp1945= _temp1933; _temp1943=
_temp1931; goto _LL1882; _LL1882: if( _temp1943 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1947);} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1947), Cyc_Absynpp_tvars2doc(
_temp1945), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1983=( char*)"(";
struct _tagged_string _temp1984; _temp1984.curr= _temp1983; _temp1984.base=
_temp1983; _temp1984.last_plus_one= _temp1983 + 2; _temp1984;}),( struct
_tagged_string)({ char* _temp1985=( char*)")"; struct _tagged_string _temp1986;
_temp1986.curr= _temp1985; _temp1986.base= _temp1985; _temp1986.last_plus_one=
_temp1985 + 2; _temp1986;}),( struct _tagged_string)({ char* _temp1987=( char*)",";
struct _tagged_string _temp1988; _temp1988.curr= _temp1987; _temp1988.base=
_temp1987; _temp1988.last_plus_one= _temp1987 + 2; _temp1988;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1943)));} goto _LL1850; _LL1850:;} return s;} struct
Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return Cyc_Absynpp_cat2(
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1989=( char*)""; struct
_tagged_string _temp1990; _temp1990.curr= _temp1989; _temp1990.base= _temp1989;
_temp1990.last_plus_one= _temp1989 + 1; _temp1990;}),( struct _tagged_string)({
char* _temp1991=( char*)"="; struct _tagged_string _temp1992; _temp1992.curr=
_temp1991; _temp1992.base= _temp1991; _temp1992.last_plus_one= _temp1991 + 2;
_temp1992;}),( struct _tagged_string)({ char* _temp1993=( char*)"="; struct
_tagged_string _temp1994; _temp1994.curr= _temp1993; _temp1994.base= _temp1993;
_temp1994.last_plus_one= _temp1993 + 2; _temp1994;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)), Cyc_Absynpp_pat2doc((* dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
==( void*) Cyc_Absyn_Wild_p: 0){ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1995=( char*)"default: "; struct _tagged_string
_temp1996; _temp1996.curr= _temp1995; _temp1996.base= _temp1995; _temp1996.last_plus_one=
_temp1995 + 10; _temp1996;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause == 0){ return Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1997=( char*)"case "; struct
_tagged_string _temp1998; _temp1998.curr= _temp1997; _temp1998.base= _temp1997;
_temp1998.last_plus_one= _temp1997 + 6; _temp1998;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1999=( char*)": "; struct
_tagged_string _temp2000; _temp2000.curr= _temp1999; _temp2000.base= _temp1999;
_temp2000.last_plus_one= _temp1999 + 3; _temp2000;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ return Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2001=( char*)"case "; struct
_tagged_string _temp2002; _temp2002.curr= _temp2001; _temp2002.base= _temp2001;
_temp2002.last_plus_one= _temp2001 + 6; _temp2002;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2003=( char*)" && "; struct
_tagged_string _temp2004; _temp2004.curr= _temp2003; _temp2004.base= _temp2003;
_temp2004.last_plus_one= _temp2003 + 5; _temp2004;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2005= c->where_clause; if(
_temp2005 == 0){ _throw( Null_Exception);} _temp2005;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2006=( char*)": "; struct _tagged_string _temp2007;
_temp2007.curr= _temp2006; _temp2007.base= _temp2006; _temp2007.last_plus_one=
_temp2006 + 3; _temp2007;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,( struct _tagged_string)({
char* _temp2008=( char*)""; struct _tagged_string _temp2009; _temp2009.curr=
_temp2008; _temp2009.base= _temp2008; _temp2009.last_plus_one= _temp2008 + 1;
_temp2009;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield*
f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);} else{ return Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( f->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2010=( char*)" = "; struct _tagged_string _temp2011; _temp2011.curr=
_temp2010; _temp2011.base= _temp2010; _temp2011.last_plus_one= _temp2010 + 4;
_temp2011;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2012= f->tag; if( _temp2012 == 0){ _throw( Null_Exception);} _temp2012;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,(
struct _tagged_string)({ char* _temp2013=( char*)","; struct _tagged_string
_temp2014; _temp2014.curr= _temp2013; _temp2014.base= _temp2013; _temp2014.last_plus_one=
_temp2013 + 2; _temp2014;}), fs);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2015=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2041; struct Cyc_Absyn_Structdecl* _temp2043;
struct Cyc_Absyn_Uniondecl* _temp2045; struct Cyc_Absyn_Vardecl* _temp2047;
struct Cyc_Absyn_Vardecl _temp2049; struct Cyc_List_List* _temp2050; struct Cyc_Core_Opt*
_temp2052; int _temp2054; struct Cyc_Absyn_Exp* _temp2056; void* _temp2058;
struct Cyc_Absyn_Tqual* _temp2060; struct _tuple0* _temp2062; void* _temp2064;
struct Cyc_Absyn_Tuniondecl* _temp2066; struct Cyc_Absyn_XTuniondecl* _temp2068;
int _temp2070; struct Cyc_Absyn_Exp* _temp2072; struct Cyc_Core_Opt* _temp2074;
struct Cyc_Core_Opt* _temp2076; struct Cyc_Absyn_Pat* _temp2078; struct Cyc_Absyn_Enumdecl*
_temp2080; struct Cyc_Absyn_Enumdecl _temp2082; struct Cyc_List_List* _temp2083;
struct _tuple0* _temp2085; void* _temp2087; struct Cyc_Absyn_Typedefdecl*
_temp2089; struct Cyc_List_List* _temp2091; struct _tagged_string* _temp2093;
struct Cyc_List_List* _temp2095; struct _tuple0* _temp2097; struct Cyc_List_List*
_temp2099; _LL2017: if((( struct _tunion_struct*) _temp2015)->tag == Cyc_Absyn_Fn_d_tag){
_LL2042: _temp2041=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp2015)->f1; goto _LL2018;} else{ goto _LL2019;} _LL2019: if((( struct
_tunion_struct*) _temp2015)->tag == Cyc_Absyn_Struct_d_tag){ _LL2044: _temp2043=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp2015)->f1;
goto _LL2020;} else{ goto _LL2021;} _LL2021: if((( struct _tunion_struct*)
_temp2015)->tag == Cyc_Absyn_Union_d_tag){ _LL2046: _temp2045=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp2015)->f1; goto _LL2022;} else{ goto
_LL2023;} _LL2023: if((( struct _tunion_struct*) _temp2015)->tag == Cyc_Absyn_Var_d_tag){
_LL2048: _temp2047=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2015)->f1; _temp2049=* _temp2047; _LL2065: _temp2064=( void*) _temp2049.sc;
goto _LL2063; _LL2063: _temp2062=( struct _tuple0*) _temp2049.name; goto _LL2061;
_LL2061: _temp2060=( struct Cyc_Absyn_Tqual*) _temp2049.tq; goto _LL2059;
_LL2059: _temp2058=( void*) _temp2049.type; goto _LL2057; _LL2057: _temp2056=(
struct Cyc_Absyn_Exp*) _temp2049.initializer; goto _LL2055; _LL2055: _temp2054=(
int) _temp2049.shadow; goto _LL2053; _LL2053: _temp2052=( struct Cyc_Core_Opt*)
_temp2049.region; goto _LL2051; _LL2051: _temp2050=( struct Cyc_List_List*)
_temp2049.attributes; goto _LL2024;} else{ goto _LL2025;} _LL2025: if((( struct
_tunion_struct*) _temp2015)->tag == Cyc_Absyn_Tunion_d_tag){ _LL2067: _temp2066=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp2015)->f1;
goto _LL2026;} else{ goto _LL2027;} _LL2027: if((( struct _tunion_struct*)
_temp2015)->tag == Cyc_Absyn_XTunion_d_tag){ _LL2069: _temp2068=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp2015)->f1; goto _LL2028;} else{ goto
_LL2029;} _LL2029: if((( struct _tunion_struct*) _temp2015)->tag == Cyc_Absyn_Let_d_tag){
_LL2079: _temp2078=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2015)->f1; goto _LL2077; _LL2077: _temp2076=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2015)->f2; goto _LL2075; _LL2075: _temp2074=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2015)->f3; goto
_LL2073; _LL2073: _temp2072=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2015)->f4; goto _LL2071; _LL2071: _temp2070=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp2015)->f5; goto _LL2030;} else{ goto _LL2031;} _LL2031: if((( struct
_tunion_struct*) _temp2015)->tag == Cyc_Absyn_Enum_d_tag){ _LL2081: _temp2080=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp2015)->f1;
_temp2082=* _temp2080; _LL2088: _temp2087=( void*) _temp2082.sc; goto _LL2086;
_LL2086: _temp2085=( struct _tuple0*) _temp2082.name; goto _LL2084; _LL2084:
_temp2083=( struct Cyc_List_List*) _temp2082.fields; goto _LL2032;} else{ goto
_LL2033;} _LL2033: if((( struct _tunion_struct*) _temp2015)->tag == Cyc_Absyn_Typedef_d_tag){
_LL2090: _temp2089=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2015)->f1; goto _LL2034;} else{ goto _LL2035;} _LL2035: if((( struct
_tunion_struct*) _temp2015)->tag == Cyc_Absyn_Namespace_d_tag){ _LL2094:
_temp2093=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2015)->f1; goto _LL2092; _LL2092: _temp2091=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2015)->f2; goto _LL2036;} else{ goto
_LL2037;} _LL2037: if((( struct _tunion_struct*) _temp2015)->tag == Cyc_Absyn_Using_d_tag){
_LL2098: _temp2097=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2015)->f1; goto _LL2096; _LL2096: _temp2095=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp2015)->f2; goto _LL2038;} else{ goto
_LL2039;} _LL2039: if((( struct _tunion_struct*) _temp2015)->tag == Cyc_Absyn_ExternC_d_tag){
_LL2100: _temp2099=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2015)->f1; goto _LL2040;} else{ goto _LL2016;} _LL2018: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2101=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2101[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2102; _temp2102.tag= Cyc_Absyn_FnType_tag; _temp2102.f1=({ struct Cyc_Absyn_FnInfo
_temp2103; _temp2103.tvars= _temp2041->tvs; _temp2103.effect= _temp2041->effect;
_temp2103.ret_typ=( void*)(( void*) _temp2041->ret_type); _temp2103.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2041->args); _temp2103.varargs=
_temp2041->varargs; _temp2103.attributes= 0; _temp2103;}); _temp2102;});
_temp2101;}); s= Cyc_Absynpp_cat7( Cyc_PP_text( _temp2041->is_inline?( struct
_tagged_string)({ char* _temp2104=( char*)"inline "; struct _tagged_string
_temp2105; _temp2105.curr= _temp2104; _temp2105.base= _temp2104; _temp2105.last_plus_one=
_temp2104 + 8; _temp2105;}):( struct _tagged_string)({ char* _temp2106=( char*)"";
struct _tagged_string _temp2107; _temp2107.curr= _temp2106; _temp2107.base=
_temp2106; _temp2107.last_plus_one= _temp2106 + 1; _temp2107;})), Cyc_Absynpp_scope2doc((
void*) _temp2041->sc), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual* _temp2108=(
struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp2108->q_const= 0; _temp2108->q_volatile= 0; _temp2108->q_restrict= 0;
_temp2108;}), t,({ struct Cyc_Core_Opt* _temp2109=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2109->v=( void*) Cyc_Absynpp_cat2(
Cyc_Absynpp_atts2doc( _temp2041->attributes), Cyc_Absynpp_typedef_name2doc(
_temp2041->name)); _temp2109;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2110=( char*)" {"; struct _tagged_string _temp2111; _temp2111.curr=
_temp2110; _temp2111.base= _temp2110; _temp2111.last_plus_one= _temp2110 + 3;
_temp2111;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp2041->body))), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2112=( char*)"}"; struct _tagged_string _temp2113; _temp2113.curr=
_temp2112; _temp2113.base= _temp2112; _temp2113.last_plus_one= _temp2112 + 2;
_temp2113;}))); goto _LL2016;} _LL2020: if( _temp2043->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp2043->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2114=( char*)"struct "; struct _tagged_string
_temp2115; _temp2115.curr= _temp2114; _temp2115.base= _temp2114; _temp2115.last_plus_one=
_temp2114 + 8; _temp2115;})), _temp2043->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2116=( char*)""; struct _tagged_string _temp2117;
_temp2117.curr= _temp2116; _temp2117.base= _temp2116; _temp2117.last_plus_one=
_temp2116 + 1; _temp2117;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp2118= _temp2043->name; if( _temp2118 == 0){ _throw(
Null_Exception);} _temp2118->v;})), Cyc_Absynpp_ktvars2doc( _temp2043->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2119=( char*)";"; struct _tagged_string
_temp2120; _temp2120.curr= _temp2119; _temp2120.base= _temp2119; _temp2120.last_plus_one=
_temp2119 + 2; _temp2120;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2043->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2121=(
char*)"struct "; struct _tagged_string _temp2122; _temp2122.curr= _temp2121;
_temp2122.base= _temp2121; _temp2122.last_plus_one= _temp2121 + 8; _temp2122;})),
_temp2043->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2123=(
char*)""; struct _tagged_string _temp2124; _temp2124.curr= _temp2123; _temp2124.base=
_temp2123; _temp2124.last_plus_one= _temp2123 + 1; _temp2124;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2125= _temp2043->name; if(
_temp2125 == 0){ _throw( Null_Exception);} _temp2125->v;})), Cyc_Absynpp_ktvars2doc(
_temp2043->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2126=( char*)" {";
struct _tagged_string _temp2127; _temp2127.curr= _temp2126; _temp2127.base=
_temp2126; _temp2127.last_plus_one= _temp2126 + 3; _temp2127;})), Cyc_PP_nest( 2,
_temp2043->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2128=(
char*)""; struct _tagged_string _temp2129; _temp2129.curr= _temp2128; _temp2129.base=
_temp2128; _temp2129.last_plus_one= _temp2128 + 1; _temp2129;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2130= _temp2043->fields; if( _temp2130 == 0){ _throw(
Null_Exception);} _temp2130->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2131=( char*)"}"; struct _tagged_string _temp2132;
_temp2132.curr= _temp2131; _temp2132.base= _temp2131; _temp2132.last_plus_one=
_temp2131 + 2; _temp2132;})), Cyc_Absynpp_atts2doc( _temp2043->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2133=( char*)";"; struct _tagged_string
_temp2134; _temp2134.curr= _temp2133; _temp2134.base= _temp2133; _temp2134.last_plus_one=
_temp2133 + 2; _temp2134;})));} goto _LL2016; _LL2022: if( _temp2045->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp2045->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2135=( char*)"union "; struct _tagged_string
_temp2136; _temp2136.curr= _temp2135; _temp2136.base= _temp2135; _temp2136.last_plus_one=
_temp2135 + 7; _temp2136;})), _temp2045->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2137=( char*)""; struct _tagged_string _temp2138;
_temp2138.curr= _temp2137; _temp2138.base= _temp2137; _temp2138.last_plus_one=
_temp2137 + 1; _temp2138;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2139= _temp2045->name; if( _temp2139 == 0){ _throw(
Null_Exception);} _temp2139->v;})), Cyc_Absynpp_tvars2doc( _temp2045->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2140=( char*)";"; struct _tagged_string
_temp2141; _temp2141.curr= _temp2140; _temp2141.base= _temp2140; _temp2141.last_plus_one=
_temp2140 + 2; _temp2141;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2045->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2142=(
char*)"union "; struct _tagged_string _temp2143; _temp2143.curr= _temp2142;
_temp2143.base= _temp2142; _temp2143.last_plus_one= _temp2142 + 7; _temp2143;})),
_temp2045->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2144=(
char*)""; struct _tagged_string _temp2145; _temp2145.curr= _temp2144; _temp2145.base=
_temp2144; _temp2145.last_plus_one= _temp2144 + 1; _temp2145;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2146= _temp2045->name; if(
_temp2146 == 0){ _throw( Null_Exception);} _temp2146->v;})), Cyc_Absynpp_tvars2doc(
_temp2045->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2147=( char*)" {";
struct _tagged_string _temp2148; _temp2148.curr= _temp2147; _temp2148.base=
_temp2147; _temp2148.last_plus_one= _temp2147 + 3; _temp2148;})), Cyc_PP_nest( 2,
_temp2045->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2149=(
char*)""; struct _tagged_string _temp2150; _temp2150.curr= _temp2149; _temp2150.base=
_temp2149; _temp2150.last_plus_one= _temp2149 + 1; _temp2150;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2151= _temp2045->fields; if( _temp2151 == 0){ _throw(
Null_Exception);} _temp2151->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2152=( char*)"}"; struct _tagged_string _temp2153;
_temp2153.curr= _temp2152; _temp2153.base= _temp2152; _temp2153.last_plus_one=
_temp2152 + 2; _temp2153;})), Cyc_Absynpp_atts2doc( _temp2045->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2154=( char*)";"; struct _tagged_string
_temp2155; _temp2155.curr= _temp2154; _temp2155.base= _temp2154; _temp2155.last_plus_one=
_temp2154 + 2; _temp2155;})));} goto _LL2016; _LL2024: { struct Cyc_PP_Doc* sn=
Cyc_Absynpp_typedef_name2doc( _temp2062); s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(
_temp2064), Cyc_Absynpp_tqtd2doc( _temp2060, _temp2058,({ struct Cyc_Core_Opt*
_temp2156=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2156->v=( void*) sn; _temp2156;})), Cyc_Absynpp_atts2doc( _temp2050),
_temp2056 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2157=( char*)"";
struct _tagged_string _temp2158; _temp2158.curr= _temp2157; _temp2158.base=
_temp2157; _temp2158.last_plus_one= _temp2157 + 1; _temp2158;})): Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2159=( char*)" = "; struct
_tagged_string _temp2160; _temp2160.curr= _temp2159; _temp2160.base= _temp2159;
_temp2160.last_plus_one= _temp2159 + 4; _temp2160;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2161= _temp2056; if(
_temp2161 == 0){ _throw( Null_Exception);} _temp2161;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2162=( char*)";"; struct _tagged_string _temp2163;
_temp2163.curr= _temp2162; _temp2163.base= _temp2162; _temp2163.last_plus_one=
_temp2162 + 2; _temp2163;}))); goto _LL2016;} _LL2026: if( _temp2066->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp2066->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2164=( char*)"tunion "; struct
_tagged_string _temp2165; _temp2165.curr= _temp2164; _temp2165.base= _temp2164;
_temp2165.last_plus_one= _temp2164 + 8; _temp2165;})), _temp2066->name == 0? Cyc_PP_text((
struct _tagged_string)({ char* _temp2166=( char*)""; struct _tagged_string
_temp2167; _temp2167.curr= _temp2166; _temp2167.base= _temp2166; _temp2167.last_plus_one=
_temp2166 + 1; _temp2167;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp2168= _temp2066->name; if( _temp2168 == 0){ _throw(
Null_Exception);} _temp2168->v;})), Cyc_Absynpp_ktvars2doc( _temp2066->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2169=( char*)";"; struct _tagged_string
_temp2170; _temp2170.curr= _temp2169; _temp2170.base= _temp2169; _temp2170.last_plus_one=
_temp2169 + 2; _temp2170;})));} else{ s= Cyc_Absynpp_cat8( Cyc_Absynpp_scope2doc((
void*) _temp2066->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2171=(
char*)"tunion "; struct _tagged_string _temp2172; _temp2172.curr= _temp2171;
_temp2172.base= _temp2171; _temp2172.last_plus_one= _temp2171 + 8; _temp2172;})),
_temp2066->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2173=(
char*)""; struct _tagged_string _temp2174; _temp2174.curr= _temp2173; _temp2174.base=
_temp2173; _temp2174.last_plus_one= _temp2173 + 1; _temp2174;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2175= _temp2066->name; if(
_temp2175 == 0){ _throw( Null_Exception);} _temp2175->v;})), Cyc_Absynpp_ktvars2doc(
_temp2066->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2176=( char*)" {";
struct _tagged_string _temp2177; _temp2177.curr= _temp2176; _temp2177.base=
_temp2176; _temp2177.last_plus_one= _temp2176 + 3; _temp2177;})), Cyc_PP_nest( 2,
_temp2066->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2178=(
char*)""; struct _tagged_string _temp2179; _temp2179.curr= _temp2178; _temp2179.base=
_temp2178; _temp2179.last_plus_one= _temp2178 + 1; _temp2179;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2180= _temp2066->fields; if( _temp2180 == 0){ _throw(
Null_Exception);} _temp2180->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2181=( char*)"};"; struct _tagged_string _temp2182;
_temp2182.curr= _temp2181; _temp2182.base= _temp2181; _temp2182.last_plus_one=
_temp2181 + 3; _temp2182;})));} goto _LL2016; _LL2028: s= Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc((
void*) _temp2068->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2183=(
char*)"xtunion "; struct _tagged_string _temp2184; _temp2184.curr= _temp2183;
_temp2184.base= _temp2183; _temp2184.last_plus_one= _temp2183 + 9; _temp2184;})),
Cyc_Absynpp_typedef_name2doc( _temp2068->name), _temp2068->fields == 0? Cyc_PP_text((
struct _tagged_string)({ char* _temp2185=( char*)";"; struct _tagged_string
_temp2186; _temp2186.curr= _temp2185; _temp2186.base= _temp2185; _temp2186.last_plus_one=
_temp2185 + 2; _temp2186;})): Cyc_Absynpp_cat4( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2187=( char*)" {"; struct _tagged_string _temp2188;
_temp2188.curr= _temp2187; _temp2188.base= _temp2187; _temp2188.last_plus_one=
_temp2187 + 3; _temp2188;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_tunionfields2doc( _temp2068->fields))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp2189=( char*)"};"; struct _tagged_string
_temp2190; _temp2190.curr= _temp2189; _temp2190.base= _temp2189; _temp2190.last_plus_one=
_temp2189 + 3; _temp2190;})))); goto _LL2016; _LL2030: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp2191=( char*)"let "; struct _tagged_string
_temp2192; _temp2192.curr= _temp2191; _temp2192.base= _temp2191; _temp2192.last_plus_one=
_temp2191 + 5; _temp2192;})), Cyc_Absynpp_pat2doc( _temp2078), Cyc_PP_text((
struct _tagged_string)({ char* _temp2193=( char*)" = "; struct _tagged_string
_temp2194; _temp2194.curr= _temp2193; _temp2194.base= _temp2193; _temp2194.last_plus_one=
_temp2193 + 4; _temp2194;})), Cyc_Absynpp_exp2doc( _temp2072), Cyc_PP_text((
struct _tagged_string)({ char* _temp2195=( char*)";"; struct _tagged_string
_temp2196; _temp2196.curr= _temp2195; _temp2196.base= _temp2195; _temp2196.last_plus_one=
_temp2195 + 2; _temp2196;}))); goto _LL2016; _LL2032: s= Cyc_Absynpp_cat7( Cyc_Absynpp_scope2doc(
_temp2087), Cyc_PP_text(( struct _tagged_string)({ char* _temp2197=( char*)"enum ";
struct _tagged_string _temp2198; _temp2198.curr= _temp2197; _temp2198.base=
_temp2197; _temp2198.last_plus_one= _temp2197 + 6; _temp2198;})), Cyc_Absynpp_qvar2doc(
_temp2085), Cyc_PP_text(( struct _tagged_string)({ char* _temp2199=( char*)" {";
struct _tagged_string _temp2200; _temp2200.curr= _temp2199; _temp2200.base=
_temp2199; _temp2200.last_plus_one= _temp2199 + 3; _temp2200;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_enumfields2doc( _temp2083))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp2201=( char*)"};";
struct _tagged_string _temp2202; _temp2202.curr= _temp2201; _temp2202.base=
_temp2201; _temp2202.last_plus_one= _temp2201 + 3; _temp2202;}))); goto _LL2016;
_LL2034: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2203=( char*)"typedef "; struct _tagged_string _temp2204; _temp2204.curr=
_temp2203; _temp2204.base= _temp2203; _temp2204.last_plus_one= _temp2203 + 9;
_temp2204;})), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual* _temp2205=(
struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp2205->q_const= 0; _temp2205->q_volatile= 0; _temp2205->q_restrict= 0;
_temp2205;}),( void*) _temp2089->defn,({ struct Cyc_Core_Opt* _temp2206=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2206->v=( void*)
Cyc_Absynpp_cat2( Cyc_Absynpp_typedef_name2doc( _temp2089->name), Cyc_Absynpp_tvars2doc(
_temp2089->tvs)); _temp2206;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2207=( char*)";"; struct _tagged_string _temp2208; _temp2208.curr=
_temp2207; _temp2208.base= _temp2207; _temp2208.last_plus_one= _temp2207 + 2;
_temp2208;}))); goto _LL2016; _LL2036: if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_add(
_temp2093);} s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2209=( char*)"namespace "; struct _tagged_string _temp2210; _temp2210.curr=
_temp2209; _temp2210.base= _temp2209; _temp2210.last_plus_one= _temp2209 + 11;
_temp2210;})), Cyc_Absynpp_textptr( _temp2093), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2211=( char*)" {"; struct _tagged_string _temp2212;
_temp2212.curr= _temp2211; _temp2212.base= _temp2211; _temp2212.last_plus_one=
_temp2211 + 3; _temp2212;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2213=( char*)""; struct _tagged_string _temp2214; _temp2214.curr=
_temp2213; _temp2214.base= _temp2213; _temp2214.last_plus_one= _temp2213 + 1;
_temp2214;}), _temp2091), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2215=( char*)"}"; struct _tagged_string _temp2216; _temp2216.curr=
_temp2215; _temp2216.base= _temp2215; _temp2216.last_plus_one= _temp2215 + 2;
_temp2216;}))); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2016; _LL2038: if( Cyc_Absynpp_print_using_stmts){ s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2217=( char*)"using "; struct
_tagged_string _temp2218; _temp2218.curr= _temp2217; _temp2218.base= _temp2217;
_temp2218.last_plus_one= _temp2217 + 7; _temp2218;})), Cyc_Absynpp_qvar2doc(
_temp2097), Cyc_PP_text(( struct _tagged_string)({ char* _temp2219=( char*)" {";
struct _tagged_string _temp2220; _temp2220.curr= _temp2219; _temp2220.base=
_temp2219; _temp2220.last_plus_one= _temp2219 + 3; _temp2220;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2221=( char*)""; struct _tagged_string
_temp2222; _temp2222.curr= _temp2221; _temp2222.base= _temp2221; _temp2222.last_plus_one=
_temp2221 + 1; _temp2222;}), _temp2095), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2223=( char*)"}"; struct _tagged_string _temp2224;
_temp2224.curr= _temp2223; _temp2224.base= _temp2223; _temp2224.last_plus_one=
_temp2223 + 2; _temp2224;})));} else{ s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2225=( char*)"/* using "; struct _tagged_string
_temp2226; _temp2226.curr= _temp2225; _temp2226.base= _temp2225; _temp2226.last_plus_one=
_temp2225 + 10; _temp2226;})), Cyc_Absynpp_qvar2doc( _temp2097), Cyc_PP_text((
struct _tagged_string)({ char* _temp2227=( char*)" { */"; struct _tagged_string
_temp2228; _temp2228.curr= _temp2227; _temp2228.base= _temp2227; _temp2228.last_plus_one=
_temp2227 + 6; _temp2228;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2229=( char*)""; struct _tagged_string _temp2230; _temp2230.curr=
_temp2229; _temp2230.base= _temp2229; _temp2230.last_plus_one= _temp2229 + 1;
_temp2230;}), _temp2095), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2231=( char*)"/* } */"; struct _tagged_string _temp2232; _temp2232.curr=
_temp2231; _temp2232.base= _temp2231; _temp2232.last_plus_one= _temp2231 + 8;
_temp2232;})));} goto _LL2016; _LL2040: if( Cyc_Absynpp_print_externC_stmts){ s=
Cyc_Absynpp_cat5( Cyc_PP_text(( struct _tagged_string)({ char* _temp2233=( char*)"extern \"C\" {";
struct _tagged_string _temp2234; _temp2234.curr= _temp2233; _temp2234.base=
_temp2233; _temp2234.last_plus_one= _temp2233 + 13; _temp2234;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2235=( char*)""; struct _tagged_string
_temp2236; _temp2236.curr= _temp2235; _temp2236.base= _temp2235; _temp2236.last_plus_one=
_temp2235 + 1; _temp2236;}), _temp2099), Cyc_PP_line_doc(), Cyc_PP_text(( struct
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
_temp2241 + 1; _temp2242;}), _temp2099), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2243=( char*)"/* } */"; struct _tagged_string
_temp2244; _temp2244.curr= _temp2243; _temp2244.base= _temp2243; _temp2244.last_plus_one=
_temp2243 + 8; _temp2244;})));} goto _LL2016; _LL2016:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void* sc){ void* _temp2245= sc; _LL2247: if(( int)
_temp2245 == Cyc_Absyn_Static){ goto _LL2248;} else{ goto _LL2249;} _LL2249: if((
int) _temp2245 == Cyc_Absyn_Public){ goto _LL2250;} else{ goto _LL2251;} _LL2251:
if(( int) _temp2245 == Cyc_Absyn_Extern){ goto _LL2252;} else{ goto _LL2253;}
_LL2253: if(( int) _temp2245 == Cyc_Absyn_ExternC){ goto _LL2254;} else{ goto
_LL2255;} _LL2255: if(( int) _temp2245 == Cyc_Absyn_Abstract){ goto _LL2256;}
else{ goto _LL2246;} _LL2248: return Cyc_PP_text(( struct _tagged_string)({ char*
_temp2257=( char*)"static "; struct _tagged_string _temp2258; _temp2258.curr=
_temp2257; _temp2258.base= _temp2257; _temp2258.last_plus_one= _temp2257 + 8;
_temp2258;})); _LL2250: return Cyc_PP_nil_doc(); _LL2252: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2259=( char*)"extern "; struct
_tagged_string _temp2260; _temp2260.curr= _temp2259; _temp2260.base= _temp2259;
_temp2260.last_plus_one= _temp2259 + 8; _temp2260;})); _LL2254: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2261=( char*)"extern \"C\" "; struct
_tagged_string _temp2262; _temp2262.curr= _temp2261; _temp2262.base= _temp2261;
_temp2262.last_plus_one= _temp2261 + 12; _temp2262;})); _LL2256: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2263=( char*)"abstract "; struct
_tagged_string _temp2264; _temp2264.curr= _temp2263; _temp2264.base= _temp2263;
_temp2264.last_plus_one= _temp2263 + 10; _temp2264;})); _LL2246:;} int Cyc_Absynpp_exists_temp_tvar_in_effect(
void* t){ void* _temp2265= t; struct Cyc_Absyn_Tvar* _temp2273; struct Cyc_List_List*
_temp2275; _LL2267: if(( unsigned int) _temp2265 > 4u?(( struct _tunion_struct*)
_temp2265)->tag == Cyc_Absyn_VarType_tag: 0){ _LL2274: _temp2273=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp2265)->f1; goto _LL2268;} else{ goto
_LL2269;} _LL2269: if(( unsigned int) _temp2265 > 4u?(( struct _tunion_struct*)
_temp2265)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL2276: _temp2275=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp2265)->f1; goto _LL2270;} else{ goto
_LL2271;} _LL2271: goto _LL2272; _LL2268: return Cyc_Tcutil_is_temp_tvar(
_temp2273); _LL2270: return(( int(*)( int(* pred)( void*), struct Cyc_List_List*
x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect, _temp2275);
_LL2272: return 0; _LL2266:;} struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual*
tq, void* t){ void* _temp2277= t; struct Cyc_Absyn_Exp* _temp2291; struct Cyc_Absyn_Tqual*
_temp2293; void* _temp2295; struct Cyc_Absyn_PtrInfo _temp2297; struct Cyc_Absyn_Conref*
_temp2299; struct Cyc_Absyn_Tqual* _temp2301; struct Cyc_Absyn_Conref* _temp2303;
void* _temp2305; void* _temp2307; struct Cyc_Absyn_FnInfo _temp2309; struct Cyc_List_List*
_temp2311; int _temp2313; struct Cyc_List_List* _temp2315; void* _temp2317;
struct Cyc_Core_Opt* _temp2319; struct Cyc_List_List* _temp2321; int _temp2323;
struct Cyc_Core_Opt* _temp2325; void* _temp2327; struct Cyc_Core_Opt* _temp2329;
struct Cyc_List_List* _temp2331; struct _tuple0* _temp2333; _LL2279: if((
unsigned int) _temp2277 > 4u?(( struct _tunion_struct*) _temp2277)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2296: _temp2295=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2277)->f1;
goto _LL2294; _LL2294: _temp2293=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2277)->f2; goto _LL2292; _LL2292: _temp2291=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2277)->f3; goto _LL2280;} else{ goto
_LL2281;} _LL2281: if(( unsigned int) _temp2277 > 4u?(( struct _tunion_struct*)
_temp2277)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2298: _temp2297=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2277)->f1;
_LL2308: _temp2307=( void*) _temp2297.elt_typ; goto _LL2306; _LL2306: _temp2305=(
void*) _temp2297.rgn_typ; goto _LL2304; _LL2304: _temp2303=( struct Cyc_Absyn_Conref*)
_temp2297.nullable; goto _LL2302; _LL2302: _temp2301=( struct Cyc_Absyn_Tqual*)
_temp2297.tq; goto _LL2300; _LL2300: _temp2299=( struct Cyc_Absyn_Conref*)
_temp2297.bounds; goto _LL2282;} else{ goto _LL2283;} _LL2283: if(( unsigned int)
_temp2277 > 4u?(( struct _tunion_struct*) _temp2277)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2310: _temp2309=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2277)->f1; _LL2322: _temp2321=( struct Cyc_List_List*) _temp2309.tvars;
goto _LL2320; _LL2320: _temp2319=( struct Cyc_Core_Opt*) _temp2309.effect; goto
_LL2318; _LL2318: _temp2317=( void*) _temp2309.ret_typ; goto _LL2316; _LL2316:
_temp2315=( struct Cyc_List_List*) _temp2309.args; goto _LL2314; _LL2314:
_temp2313=( int) _temp2309.varargs; goto _LL2312; _LL2312: _temp2311=( struct
Cyc_List_List*) _temp2309.attributes; goto _LL2284;} else{ goto _LL2285;}
_LL2285: if(( unsigned int) _temp2277 > 4u?(( struct _tunion_struct*) _temp2277)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL2328: _temp2327=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2277)->f1; goto _LL2326; _LL2326: _temp2325=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp2277)->f2; goto _LL2324; _LL2324: _temp2323=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp2277)->f3; goto _LL2286;} else{ goto
_LL2287;} _LL2287: if(( unsigned int) _temp2277 > 4u?(( struct _tunion_struct*)
_temp2277)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL2334: _temp2333=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2277)->f1; goto _LL2332;
_LL2332: _temp2331=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2277)->f2; goto _LL2330; _LL2330: _temp2329=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2277)->f3; goto _LL2288;} else{ goto
_LL2289;} _LL2289: goto _LL2290; _LL2280: { struct Cyc_List_List* _temp2337;
void* _temp2339; struct Cyc_Absyn_Tqual* _temp2341; struct _tuple4 _temp2335=
Cyc_Absynpp_to_tms( _temp2293, _temp2295); _LL2342: _temp2341= _temp2335.f1;
goto _LL2340; _LL2340: _temp2339= _temp2335.f2; goto _LL2338; _LL2338: _temp2337=
_temp2335.f3; goto _LL2336; _LL2336: { void* tm; if( _temp2291 == 0){ tm=( void*)
Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2343=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2343[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2344; _temp2344.tag= Cyc_Absyn_ConstArray_mod_tag; _temp2344.f1=( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2345= _temp2291; if( _temp2345 == 0){
_throw( Null_Exception);} _temp2345;}); _temp2344;}); _temp2343;});} return({
struct _tuple4 _temp2346; _temp2346.f1= _temp2341; _temp2346.f2= _temp2339;
_temp2346.f3=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2347=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2347->hd=( void*)
tm; _temp2347->tl= _temp2337; _temp2347;}); _temp2346;});}} _LL2282: { struct
Cyc_List_List* _temp2350; void* _temp2352; struct Cyc_Absyn_Tqual* _temp2354;
struct _tuple4 _temp2348= Cyc_Absynpp_to_tms( _temp2301, _temp2307); _LL2355:
_temp2354= _temp2348.f1; goto _LL2353; _LL2353: _temp2352= _temp2348.f2; goto
_LL2351; _LL2351: _temp2350= _temp2348.f3; goto _LL2349; _LL2349: { void* ps;{
struct _tuple8 _temp2357=({ struct _tuple8 _temp2356; _temp2356.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2303))->v; _temp2356.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2299))->v; _temp2356;}); void* _temp2367;
void* _temp2369; struct Cyc_Absyn_Exp* _temp2371; void* _temp2373; int _temp2375;
void* _temp2377; void* _temp2379; struct Cyc_Absyn_Exp* _temp2381; void*
_temp2383; int _temp2385; void* _temp2387; void* _temp2389; void* _temp2391;
_LL2359: _LL2374: _temp2373= _temp2357.f1; if(( unsigned int) _temp2373 > 1u?((
struct _tunion_struct*) _temp2373)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2376:
_temp2375=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2373)->f1; if(
_temp2375 == 1){ goto _LL2368;} else{ goto _LL2361;}} else{ goto _LL2361;}
_LL2368: _temp2367= _temp2357.f2; if(( unsigned int) _temp2367 > 1u?(( struct
_tunion_struct*) _temp2367)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2370:
_temp2369=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2367)->f1; if((
unsigned int) _temp2369 > 1u?(( struct _tunion_struct*) _temp2369)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2372: _temp2371=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2369)->f1; goto _LL2360;} else{ goto _LL2361;}} else{ goto _LL2361;}
_LL2361: _LL2384: _temp2383= _temp2357.f1; if(( unsigned int) _temp2383 > 1u?((
struct _tunion_struct*) _temp2383)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2386:
_temp2385=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2383)->f1; if(
_temp2385 == 0){ goto _LL2378;} else{ goto _LL2363;}} else{ goto _LL2363;}
_LL2378: _temp2377= _temp2357.f2; if(( unsigned int) _temp2377 > 1u?(( struct
_tunion_struct*) _temp2377)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2380:
_temp2379=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2377)->f1; if((
unsigned int) _temp2379 > 1u?(( struct _tunion_struct*) _temp2379)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2382: _temp2381=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2379)->f1; goto _LL2362;} else{ goto _LL2363;}} else{ goto _LL2363;}
_LL2363: _LL2392: _temp2391= _temp2357.f1; goto _LL2388; _LL2388: _temp2387=
_temp2357.f2; if(( unsigned int) _temp2387 > 1u?(( struct _tunion_struct*)
_temp2387)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2390: _temp2389=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2387)->f1; if(( int) _temp2389 == Cyc_Absyn_Unknown_b){
goto _LL2364;} else{ goto _LL2365;}} else{ goto _LL2365;} _LL2365: goto _LL2366;
_LL2360: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2393=( struct
Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2393[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2394; _temp2394.tag=
Cyc_Absyn_Nullable_ps_tag; _temp2394.f1= _temp2371; _temp2394;}); _temp2393;});
goto _LL2358; _LL2362: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2395=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2395[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2396; _temp2396.tag= Cyc_Absyn_NonNullable_ps_tag; _temp2396.f1= _temp2381;
_temp2396;}); _temp2395;}); goto _LL2358; _LL2364: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2358; _LL2366: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2358;
_LL2358:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2397=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2397[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2398; _temp2398.tag=
Cyc_Absyn_Pointer_mod_tag; _temp2398.f1=( void*) ps; _temp2398.f2=( void*)
_temp2305; _temp2398.f3= tq; _temp2398;}); _temp2397;}); return({ struct _tuple4
_temp2399; _temp2399.f1= _temp2354; _temp2399.f2= _temp2352; _temp2399.f3=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2400=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2400->hd=( void*) tm; _temp2400->tl=
_temp2350; _temp2400;}); _temp2399;});}}} _LL2284: if( ! Cyc_Absynpp_print_all_tvars){
if( _temp2319 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)({ struct
Cyc_Core_Opt* _temp2401= _temp2319; if( _temp2401 == 0){ _throw( Null_Exception);}
_temp2401->v;}))){ _temp2319= 0; _temp2321= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2321);}}{ struct Cyc_List_List* _temp2404;
void* _temp2406; struct Cyc_Absyn_Tqual* _temp2408; struct _tuple4 _temp2402=
Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(), _temp2317); _LL2409: _temp2408=
_temp2402.f1; goto _LL2407; _LL2407: _temp2406= _temp2402.f2; goto _LL2405;
_LL2405: _temp2404= _temp2402.f3; goto _LL2403; _LL2403: { struct Cyc_List_List*
tms= _temp2404; if( _temp2311 != 0){ tms=({ struct Cyc_List_List* _temp2410=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2410->hd=(
void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp2411=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2411[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2412; _temp2412.tag= Cyc_Absyn_Attributes_mod_tag;
_temp2412.f1= 0; _temp2412.f2= _temp2311; _temp2412;}); _temp2411;})); _temp2410->tl=
tms; _temp2410;});} tms=({ struct Cyc_List_List* _temp2413=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2413->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2414=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2414[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2415; _temp2415.tag= Cyc_Absyn_Function_mod_tag;
_temp2415.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2416=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2416[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2417; _temp2417.tag= Cyc_Absyn_WithTypes_tag;
_temp2417.f1= _temp2315; _temp2417.f2= _temp2313; _temp2417.f3= _temp2319;
_temp2417;}); _temp2416;})); _temp2415;}); _temp2414;})); _temp2413->tl= tms;
_temp2413;}); if( _temp2321 != 0){ tms=({ struct Cyc_List_List* _temp2418=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2418->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2419=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2419[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2420; _temp2420.tag= Cyc_Absyn_TypeParams_mod_tag;
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
struct Cyc_Absyn_Tqual* tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp2429; void* _temp2431; struct Cyc_Absyn_Tqual* _temp2433; struct _tuple4
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