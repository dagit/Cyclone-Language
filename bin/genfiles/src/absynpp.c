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
struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp*
f2; } ; static const int Cyc_Absyn_Sizeoftyp_e_tag= 16; struct Cyc_Absyn_Sizeoftyp_e_struct{
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
30; struct Cyc_Absyn_Malloc_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void*
f2; } ; static const int Cyc_Absyn_UnresolvedMem_e_tag= 31; struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Absyn_StmtExp_e_tag= 32; struct Cyc_Absyn_StmtExp_e_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Codegen_e_tag= 33; struct Cyc_Absyn_Codegen_e_struct{
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
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Region_s_tag= 17;
struct Cyc_Absyn_Region_s_struct{ int tag; struct Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment*
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
rgn; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc;
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
d,({ struct Cyc_List_List* _temp398= tms; if( _temp398 == 0){ _throw(
Null_Exception);} _temp398->tl;})); struct Cyc_PP_Doc* p_rest= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp394=( char*)"("; struct
_tagged_string _temp395; _temp395.curr= _temp394; _temp395.base= _temp394;
_temp395.last_plus_one= _temp394 + 2; _temp395;})), rest, Cyc_PP_text(( struct
_tagged_string)({ char* _temp396=( char*)")"; struct _tagged_string _temp397;
_temp397.curr= _temp396; _temp397.base= _temp396; _temp397.last_plus_one=
_temp396 + 2; _temp397;}))); void* _temp289=( void*)({ struct Cyc_List_List*
_temp288= tms; if( _temp288 == 0){ _throw( Null_Exception);} _temp288->hd;});
struct Cyc_Absyn_Exp* _temp303; void* _temp305; struct Cyc_List_List* _temp307;
struct Cyc_Position_Segment* _temp309; int _temp311; struct Cyc_Position_Segment*
_temp313; struct Cyc_List_List* _temp315; struct Cyc_Absyn_Tqual* _temp317; void*
_temp319; void* _temp321; _LL291: if(( int) _temp289 == Cyc_Absyn_Carray_mod){
goto _LL292;} else{ goto _LL293;} _LL293: if(( unsigned int) _temp289 > 1u?((
struct _tunion_struct*) _temp289)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){
_LL304: _temp303=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp289)->f1; goto _LL294;} else{ goto _LL295;} _LL295: if(( unsigned int)
_temp289 > 1u?(( struct _tunion_struct*) _temp289)->tag == Cyc_Absyn_Function_mod_tag:
0){ _LL306: _temp305=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp289)->f1;
goto _LL296;} else{ goto _LL297;} _LL297: if(( unsigned int) _temp289 > 1u?((
struct _tunion_struct*) _temp289)->tag == Cyc_Absyn_Attributes_mod_tag: 0){
_LL310: _temp309=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp289)->f1; goto _LL308; _LL308: _temp307=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Attributes_mod_struct*) _temp289)->f2; goto _LL298;} else{ goto _LL299;}
_LL299: if(( unsigned int) _temp289 > 1u?(( struct _tunion_struct*) _temp289)->tag
== Cyc_Absyn_TypeParams_mod_tag: 0){ _LL316: _temp315=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp289)->f1; goto _LL314; _LL314:
_temp313=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp289)->f2; goto _LL312; _LL312: _temp311=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp289)->f3; goto _LL300;} else{ goto _LL301;} _LL301: if(( unsigned int)
_temp289 > 1u?(( struct _tunion_struct*) _temp289)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL322: _temp321=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp289)->f1;
goto _LL320; _LL320: _temp319=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp289)->f2; goto _LL318; _LL318: _temp317=( struct Cyc_Absyn_Tqual*)(( struct
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
_temp344; struct Cyc_List_List* _temp346; _LL334: if((( struct _tunion_struct*)
_temp332)->tag == Cyc_Absyn_WithTypes_tag){ _LL343: _temp342=( struct Cyc_List_List*)((
struct Cyc_Absyn_WithTypes_struct*) _temp332)->f1; goto _LL341; _LL341: _temp340=(
int)(( struct Cyc_Absyn_WithTypes_struct*) _temp332)->f2; goto _LL339; _LL339:
_temp338=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp332)->f3;
goto _LL335;} else{ goto _LL336;} _LL336: if((( struct _tunion_struct*) _temp332)->tag
== Cyc_Absyn_NoTypes_tag){ _LL347: _temp346=( struct Cyc_List_List*)(( struct
Cyc_Absyn_NoTypes_struct*) _temp332)->f1; goto _LL345; _LL345: _temp344=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*) _temp332)->f2; goto
_LL337;} else{ goto _LL333;} _LL335: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_funargs2doc(
_temp342, _temp340, _temp338)); _LL337: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp348=( char*)"("; struct _tagged_string
_temp349; _temp349.curr= _temp348; _temp349.base= _temp348; _temp349.last_plus_one=
_temp348 + 2; _temp349;}),( struct _tagged_string)({ char* _temp350=( char*)")";
struct _tagged_string _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 2; _temp351;}),( struct _tagged_string)({
char* _temp352=( char*)","; struct _tagged_string _temp353; _temp353.curr=
_temp352; _temp353.base= _temp352; _temp353.last_plus_one= _temp352 + 2;
_temp353;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr,
_temp346))); _LL333:;} _LL298: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List*
_temp354= tms; if( _temp354 == 0){ _throw( Null_Exception);} _temp354->tl;}))){
rest= p_rest;} return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_atts2doc( _temp307));
_LL300: if( Cyc_Absynpp_next_is_pointer(({ struct Cyc_List_List* _temp355= tms;
if( _temp355 == 0){ _throw( Null_Exception);} _temp355->tl;}))){ rest= p_rest;}
if( _temp311){ return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_ktvars2doc( _temp315));}
else{ return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_tvars2doc( _temp315));} _LL302: {
struct Cyc_PP_Doc* ptr;{ void* _temp356= _temp321; struct Cyc_Absyn_Exp*
_temp364; struct Cyc_Absyn_Exp* _temp366; _LL358: if(( unsigned int) _temp356 >
1u?(( struct _tunion_struct*) _temp356)->tag == Cyc_Absyn_Nullable_ps_tag: 0){
_LL365: _temp364=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp356)->f1; goto _LL359;} else{ goto _LL360;} _LL360: if(( unsigned int)
_temp356 > 1u?(( struct _tunion_struct*) _temp356)->tag == Cyc_Absyn_NonNullable_ps_tag:
0){ _LL367: _temp366=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp356)->f1; goto _LL361;} else{ goto _LL362;} _LL362: if(( int) _temp356 ==
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
_temp319); _LL388: if(( int) _temp386 == Cyc_Absyn_HeapRgn){ goto _LL389;} else{
goto _LL390;} _LL390: goto _LL391; _LL389: return Cyc_Absynpp_cat2( ptr, rest);
_LL391: return Cyc_Absynpp_cat4( ptr, Cyc_Absynpp_ntyp2doc( _temp319), Cyc_PP_text((
struct _tagged_string)({ char* _temp392=( char*)" "; struct _tagged_string
_temp393; _temp393.curr= _temp392; _temp393.base= _temp392; _temp393.last_plus_one=
_temp392 + 2; _temp393;})), rest); _LL387:;}} _LL290:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc(
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
_temp570)); goto _LL443; _LL481: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp687=( char*)"region_t<"; struct _tagged_string
_temp688; _temp688.curr= _temp687; _temp688.base= _temp687; _temp688.last_plus_one=
_temp687 + 10; _temp688;})), Cyc_Absynpp_rgn2doc( _temp574), Cyc_PP_text((
struct _tagged_string)({ char* _temp689=( char*)">"; struct _tagged_string
_temp690; _temp690.curr= _temp689; _temp690.base= _temp689; _temp690.last_plus_one=
_temp689 + 2; _temp690;}))); goto _LL443; _LL483: s= Cyc_Absynpp_rgn2doc( t);
goto _LL443; _LL485: s= Cyc_Absynpp_eff2doc( t); goto _LL443; _LL487: s= Cyc_Absynpp_eff2doc(
t); goto _LL443; _LL443:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp691= vo; if( _temp691 == 0){
_throw( Null_Exception);} _temp691->v;})));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc(
struct _tuple1* t){ struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt*
_temp692=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp692->v=( void*) Cyc_PP_text(*(( struct _tagged_string*)({ struct Cyc_Core_Opt*
_temp693=(* t).f1; if( _temp693 == 0){ _throw( Null_Exception);} _temp693->v;})));
_temp692;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt*
effopt){ if( effopt == 0){ if( varargs){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp694=( char*)"("; struct _tagged_string _temp695;
_temp695.curr= _temp694; _temp695.base= _temp694; _temp695.last_plus_one=
_temp694 + 2; _temp695;}),( struct _tagged_string)({ char* _temp696=( char*)")";
struct _tagged_string _temp697; _temp697.curr= _temp696; _temp697.base= _temp696;
_temp697.last_plus_one= _temp696 + 2; _temp697;}),( struct _tagged_string)({
char* _temp698=( char*)","; struct _tagged_string _temp699; _temp699.curr=
_temp698; _temp699.base= _temp698; _temp699.last_plus_one= _temp698 + 2;
_temp699;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)((( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args),({ struct Cyc_List_List* _temp700=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp700->hd=( void*) Cyc_PP_text(( struct
_tagged_string)({ char* _temp701=( char*)"..."; struct _tagged_string _temp702;
_temp702.curr= _temp701; _temp702.base= _temp701; _temp702.last_plus_one=
_temp701 + 4; _temp702;})); _temp700->tl= 0; _temp700;})));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp703=( char*)"("; struct _tagged_string
_temp704; _temp704.curr= _temp703; _temp704.base= _temp703; _temp704.last_plus_one=
_temp703 + 2; _temp704;}),( struct _tagged_string)({ char* _temp705=( char*)")";
struct _tagged_string _temp706; _temp706.curr= _temp705; _temp706.base= _temp705;
_temp706.last_plus_one= _temp705 + 2; _temp706;}),( struct _tagged_string)({
char* _temp707=( char*)","; struct _tagged_string _temp708; _temp708.curr=
_temp707; _temp708.base= _temp707; _temp708.last_plus_one= _temp707 + 2;
_temp708;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));}} else{
if( varargs){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp709=(
char*)"("; struct _tagged_string _temp710; _temp710.curr= _temp709; _temp710.base=
_temp709; _temp710.last_plus_one= _temp709 + 2; _temp710;}),( struct
_tagged_string)({ char* _temp711=( char*)")"; struct _tagged_string _temp712;
_temp712.curr= _temp711; _temp712.base= _temp711; _temp712.last_plus_one=
_temp711 + 2; _temp712;}),( struct _tagged_string)({ char* _temp713=( char*)";";
struct _tagged_string _temp714; _temp714.curr= _temp713; _temp714.base= _temp713;
_temp714.last_plus_one= _temp713 + 2; _temp714;}),({ struct Cyc_List_List*
_temp715=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp715->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp718=( char*)""; struct _tagged_string _temp719; _temp719.curr= _temp718;
_temp719.base= _temp718; _temp719.last_plus_one= _temp718 + 1; _temp719;}),(
struct _tagged_string)({ char* _temp720=( char*)""; struct _tagged_string
_temp721; _temp721.curr= _temp720; _temp721.base= _temp720; _temp721.last_plus_one=
_temp720 + 1; _temp721;}),( struct _tagged_string)({ char* _temp722=( char*)",";
struct _tagged_string _temp723; _temp723.curr= _temp722; _temp723.base= _temp722;
_temp723.last_plus_one= _temp722 + 2; _temp723;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args),({ struct Cyc_List_List*
_temp724=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp724->hd=( void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp725=(
char*)"..."; struct _tagged_string _temp726; _temp726.curr= _temp725; _temp726.base=
_temp725; _temp726.last_plus_one= _temp725 + 4; _temp726;})); _temp724->tl= 0;
_temp724;}))); _temp715->tl=({ struct Cyc_List_List* _temp716=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp716->hd=( void*) Cyc_Absynpp_eff2doc((
void*)({ struct Cyc_Core_Opt* _temp717= effopt; if( _temp717 == 0){ _throw(
Null_Exception);} _temp717->v;})); _temp716->tl= 0; _temp716;}); _temp715;}));}
else{ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp727=( char*)"(";
struct _tagged_string _temp728; _temp728.curr= _temp727; _temp728.base= _temp727;
_temp728.last_plus_one= _temp727 + 2; _temp728;}),( struct _tagged_string)({
char* _temp729=( char*)")"; struct _tagged_string _temp730; _temp730.curr=
_temp729; _temp730.base= _temp729; _temp730.last_plus_one= _temp729 + 2;
_temp730;}),( struct _tagged_string)({ char* _temp731=( char*)";"; struct
_tagged_string _temp732; _temp732.curr= _temp731; _temp732.base= _temp731;
_temp732.last_plus_one= _temp731 + 2; _temp732;}),({ struct Cyc_List_List*
_temp733=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp733->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp736=( char*)""; struct _tagged_string _temp737; _temp737.curr= _temp736;
_temp737.base= _temp736; _temp737.last_plus_one= _temp736 + 1; _temp737;}),(
struct _tagged_string)({ char* _temp738=( char*)""; struct _tagged_string
_temp739; _temp739.curr= _temp738; _temp739.base= _temp738; _temp739.last_plus_one=
_temp738 + 1; _temp739;}),( struct _tagged_string)({ char* _temp740=( char*)",";
struct _tagged_string _temp741; _temp741.curr= _temp740; _temp741.base= _temp740;
_temp741.last_plus_one= _temp740 + 2; _temp741;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_funarg2doc, args)); _temp733->tl=({ struct Cyc_List_List* _temp734=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp734->hd=(
void*) Cyc_Absynpp_eff2doc(( void*)({ struct Cyc_Core_Opt* _temp735= effopt; if(
_temp735 == 0){ _throw( Null_Exception);} _temp735->v;})); _temp734->tl= 0;
_temp734;}); _temp733;}));}}} struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct
_tuple3* arg){ return({ struct _tuple1* _temp742=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp742->f1=({ struct Cyc_Core_Opt* _temp743=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp743->v=( void*)(*
arg).f1; _temp743;}); _temp742->f2=(* arg).f2; _temp742->f3=(* arg).f3; _temp742;});}
struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct _tagged_string* v){ return Cyc_PP_text(*
v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
prefix= 0;{ void* _temp744=(* q).f1; struct Cyc_List_List* _temp756; struct Cyc_List_List*
_temp758; struct Cyc_List_List* _temp760; struct Cyc_List_List* _temp762; _LL746:
if(( int) _temp744 == Cyc_Absyn_Loc_n){ goto _LL747;} else{ goto _LL748;} _LL748:
if(( unsigned int) _temp744 > 1u?(( struct _tunion_struct*) _temp744)->tag ==
Cyc_Absyn_Rel_n_tag: 0){ _LL757: _temp756=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp744)->f1; if( _temp756 == 0){ goto _LL749;} else{ goto _LL750;}} else{ goto
_LL750;} _LL750: if(( unsigned int) _temp744 > 1u?(( struct _tunion_struct*)
_temp744)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL759: _temp758=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp744)->f1; if( _temp758 == 0){ goto _LL751;}
else{ goto _LL752;}} else{ goto _LL752;} _LL752: if(( unsigned int) _temp744 > 1u?((
struct _tunion_struct*) _temp744)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL761:
_temp760=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp744)->f1;
goto _LL753;} else{ goto _LL754;} _LL754: if(( unsigned int) _temp744 > 1u?((
struct _tunion_struct*) _temp744)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL763:
_temp762=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp744)->f1;
goto _LL755;} else{ goto _LL745;} _LL747: goto _LL749; _LL749: goto _LL751;
_LL751: return Cyc_Absynpp_var2doc((* q).f2); _LL753: _temp762= _temp760; goto
_LL755; _LL755: prefix= _temp762; goto _LL745; _LL745:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( prefix,({ struct
Cyc_List_List* _temp764=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp764->hd=( void*)(* q).f2; _temp764->tl= 0; _temp764;})),( struct
_tagged_string)({ char* _temp765=( char*)"_"; struct _tagged_string _temp766;
_temp766.curr= _temp765; _temp766.base= _temp765; _temp766.last_plus_one=
_temp765 + 2; _temp766;})));} else{ if((( int(*)( int(* cmp)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, prefix, Cyc_Absynpp_curr_namespace)){
return Cyc_Absynpp_var2doc((* q).f2);} else{ return Cyc_PP_text( Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
prefix,({ struct Cyc_List_List* _temp767=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp767->hd=( void*)(* q).f2; _temp767->tl= 0;
_temp767;})),( struct _tagged_string)({ char* _temp768=( char*)"::"; struct
_tagged_string _temp769; _temp769.curr= _temp768; _temp769.base= _temp768;
_temp769.last_plus_one= _temp768 + 3; _temp769;})));}}} struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc(
struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){ return Cyc_Absynpp_qvar2doc(
v);}{ void* _temp770=(* v).f1; struct Cyc_List_List* _temp776; _LL772: if((
unsigned int) _temp770 > 1u?(( struct _tunion_struct*) _temp770)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL777: _temp776=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp770)->f1; goto _LL773;} else{ goto _LL774;} _LL774: goto _LL775; _LL773:
if((( int(*)( int(* cmp)( struct _tagged_string*, struct _tagged_string*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp,
_temp776, Cyc_Absynpp_curr_namespace) == 0){ return Cyc_Absynpp_var2doc((* v).f2);}
else{ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp778=( char*)"/* wrong namespace : */ "; struct _tagged_string _temp779;
_temp779.curr= _temp778; _temp779.base= _temp778; _temp779.last_plus_one=
_temp778 + 25; _temp779;})), Cyc_Absynpp_qvar2doc( v));} _LL775: return Cyc_Absynpp_qvar2doc(
v); _LL771:;}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*
e){ void* _temp780=( void*) e->r; void* _temp852; void* _temp854; struct _tuple0*
_temp856; struct _tuple0* _temp858; struct Cyc_List_List* _temp860; void*
_temp862; struct Cyc_Absyn_Exp* _temp864; struct Cyc_Core_Opt* _temp866; struct
Cyc_Absyn_Exp* _temp868; void* _temp870; struct Cyc_Absyn_Exp* _temp872; struct
Cyc_Absyn_Exp* _temp874; struct Cyc_Absyn_Exp* _temp876; struct Cyc_Absyn_Exp*
_temp878; struct Cyc_Absyn_Exp* _temp880; struct Cyc_Absyn_Exp* _temp882; struct
Cyc_List_List* _temp884; struct Cyc_Absyn_Exp* _temp886; struct Cyc_List_List*
_temp888; struct Cyc_Absyn_Exp* _temp890; struct Cyc_Absyn_Exp* _temp892; struct
Cyc_Absyn_Exp* _temp894; struct Cyc_List_List* _temp896; struct Cyc_Absyn_Exp*
_temp898; struct Cyc_Absyn_Exp* _temp900; void* _temp902; struct Cyc_Absyn_Exp*
_temp904; struct Cyc_Absyn_Exp* _temp906; struct Cyc_Absyn_Exp* _temp908; void*
_temp910; struct Cyc_Absyn_Exp* _temp912; struct Cyc_Absyn_Exp* _temp914; struct
_tagged_string* _temp916; struct Cyc_Absyn_Exp* _temp918; struct _tagged_string*
_temp920; struct Cyc_Absyn_Exp* _temp922; struct Cyc_Absyn_Exp* _temp924; struct
Cyc_Absyn_Exp* _temp926; struct Cyc_List_List* _temp928; struct Cyc_List_List*
_temp930; struct _tuple1* _temp932; struct Cyc_List_List* _temp934; struct Cyc_Absyn_Exp*
_temp936; struct Cyc_Absyn_Exp* _temp938; struct Cyc_Absyn_Vardecl* _temp940;
struct Cyc_Absyn_Structdecl* _temp942; struct Cyc_List_List* _temp944; struct
Cyc_Core_Opt* _temp946; struct _tuple0* _temp948; struct Cyc_Absyn_Tunionfield*
_temp950; struct Cyc_Absyn_Tuniondecl* _temp952; struct Cyc_List_List* _temp954;
struct Cyc_Core_Opt* _temp956; struct Cyc_Core_Opt* _temp958; struct Cyc_Absyn_Tunionfield*
_temp960; struct Cyc_Absyn_XTuniondecl* _temp962; struct Cyc_List_List* _temp964;
struct Cyc_Core_Opt* _temp966; struct Cyc_Absyn_Enumfield* _temp968; struct Cyc_Absyn_Enumdecl*
_temp970; struct _tuple0* _temp972; void* _temp974; struct Cyc_Absyn_Exp*
_temp976; struct Cyc_List_List* _temp978; struct Cyc_Core_Opt* _temp980; struct
Cyc_Absyn_Stmt* _temp982; struct Cyc_Absyn_Fndecl* _temp984; struct Cyc_Absyn_Exp*
_temp986; _LL782: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_Const_e_tag){
_LL853: _temp852=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp780)->f1;
goto _LL783;} else{ goto _LL784;} _LL784: if((( struct _tunion_struct*) _temp780)->tag
== Cyc_Absyn_Var_e_tag){ _LL857: _temp856=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp780)->f1; goto _LL855; _LL855: _temp854=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp780)->f2; goto _LL785;} else{ goto _LL786;} _LL786: if((( struct
_tunion_struct*) _temp780)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL859: _temp858=(
struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp780)->f1; goto
_LL787;} else{ goto _LL788;} _LL788: if((( struct _tunion_struct*) _temp780)->tag
== Cyc_Absyn_Primop_e_tag){ _LL863: _temp862=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp780)->f1; goto _LL861; _LL861: _temp860=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp780)->f2; goto _LL789;} else{ goto _LL790;}
_LL790: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL869: _temp868=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp780)->f1; goto _LL867; _LL867: _temp866=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp780)->f2; goto _LL865; _LL865: _temp864=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp780)->f3;
goto _LL791;} else{ goto _LL792;} _LL792: if((( struct _tunion_struct*) _temp780)->tag
== Cyc_Absyn_Increment_e_tag){ _LL873: _temp872=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp780)->f1; goto _LL871; _LL871:
_temp870=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp780)->f2; goto
_LL793;} else{ goto _LL794;} _LL794: if((( struct _tunion_struct*) _temp780)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL879: _temp878=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp780)->f1; goto _LL877; _LL877:
_temp876=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp780)->f2; goto _LL875; _LL875: _temp874=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp780)->f3; goto _LL795;} else{ goto _LL796;}
_LL796: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL883: _temp882=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp780)->f1; goto _LL881; _LL881: _temp880=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp780)->f2; goto _LL797;} else{ goto _LL798;}
_LL798: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL887: _temp886=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp780)->f1; goto _LL885; _LL885: _temp884=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp780)->f2; goto _LL799;} else{ goto _LL800;}
_LL800: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_FnCall_e_tag){
_LL891: _temp890=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp780)->f1; goto _LL889; _LL889: _temp888=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp780)->f2; goto _LL801;} else{ goto _LL802;}
_LL802: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_Throw_e_tag){
_LL893: _temp892=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp780)->f1; goto _LL803;} else{ goto _LL804;} _LL804: if((( struct
_tunion_struct*) _temp780)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL895:
_temp894=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp780)->f1; goto _LL805;} else{ goto _LL806;} _LL806: if((( struct
_tunion_struct*) _temp780)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL899:
_temp898=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp780)->f1; goto _LL897; _LL897: _temp896=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp780)->f2; goto _LL807;} else{ goto _LL808;}
_LL808: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_Cast_e_tag){
_LL903: _temp902=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp780)->f1; goto
_LL901; _LL901: _temp900=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp780)->f2; goto _LL809;} else{ goto _LL810;} _LL810: if((( struct
_tunion_struct*) _temp780)->tag == Cyc_Absyn_New_e_tag){ _LL907: _temp906=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp780)->f1; goto
_LL905; _LL905: _temp904=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp780)->f2; goto _LL811;} else{ goto _LL812;} _LL812: if((( struct
_tunion_struct*) _temp780)->tag == Cyc_Absyn_Address_e_tag){ _LL909: _temp908=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp780)->f1; goto
_LL813;} else{ goto _LL814;} _LL814: if((( struct _tunion_struct*) _temp780)->tag
== Cyc_Absyn_Sizeoftyp_e_tag){ _LL911: _temp910=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp780)->f1; goto _LL815;} else{ goto _LL816;} _LL816: if((( struct
_tunion_struct*) _temp780)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL913: _temp912=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp780)->f1;
goto _LL817;} else{ goto _LL818;} _LL818: if((( struct _tunion_struct*) _temp780)->tag
== Cyc_Absyn_Deref_e_tag){ _LL915: _temp914=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp780)->f1; goto _LL819;} else{ goto _LL820;}
_LL820: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_StructMember_e_tag){
_LL919: _temp918=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp780)->f1; goto _LL917; _LL917: _temp916=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp780)->f2; goto _LL821;} else{ goto _LL822;}
_LL822: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL923: _temp922=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp780)->f1; goto _LL921; _LL921: _temp920=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp780)->f2; goto _LL823;} else{ goto _LL824;}
_LL824: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_Subscript_e_tag){
_LL927: _temp926=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp780)->f1; goto _LL925; _LL925: _temp924=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp780)->f2; goto _LL825;} else{ goto _LL826;}
_LL826: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_Tuple_e_tag){
_LL929: _temp928=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp780)->f1; goto _LL827;} else{ goto _LL828;} _LL828: if((( struct
_tunion_struct*) _temp780)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL933:
_temp932=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp780)->f1;
goto _LL931; _LL931: _temp930=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp780)->f2; goto _LL829;} else{ goto _LL830;} _LL830: if((( struct
_tunion_struct*) _temp780)->tag == Cyc_Absyn_Array_e_tag){ _LL935: _temp934=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp780)->f1; goto
_LL831;} else{ goto _LL832;} _LL832: if((( struct _tunion_struct*) _temp780)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL941: _temp940=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp780)->f1; goto _LL939; _LL939:
_temp938=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp780)->f2; goto _LL937; _LL937: _temp936=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp780)->f3; goto _LL833;} else{ goto
_LL834;} _LL834: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_Struct_e_tag){
_LL949: _temp948=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp780)->f1; goto _LL947; _LL947: _temp946=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp780)->f2; goto _LL945; _LL945: _temp944=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp780)->f3; goto
_LL943; _LL943: _temp942=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp780)->f4; goto _LL835;} else{ goto _LL836;} _LL836: if((( struct
_tunion_struct*) _temp780)->tag == Cyc_Absyn_Tunion_e_tag){ _LL959: _temp958=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp780)->f1; goto
_LL957; _LL957: _temp956=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp780)->f2; goto _LL955; _LL955: _temp954=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp780)->f3; goto _LL953; _LL953: _temp952=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp780)->f4;
goto _LL951; _LL951: _temp950=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp780)->f5; goto _LL837;} else{ goto _LL838;} _LL838: if((( struct
_tunion_struct*) _temp780)->tag == Cyc_Absyn_XTunion_e_tag){ _LL967: _temp966=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp780)->f1; goto
_LL965; _LL965: _temp964=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp780)->f2; goto _LL963; _LL963: _temp962=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp780)->f3; goto _LL961; _LL961: _temp960=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp780)->f4;
goto _LL839;} else{ goto _LL840;} _LL840: if((( struct _tunion_struct*) _temp780)->tag
== Cyc_Absyn_Enum_e_tag){ _LL973: _temp972=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp780)->f1; goto _LL971; _LL971: _temp970=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp780)->f2; goto _LL969; _LL969: _temp968=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp780)->f3;
goto _LL841;} else{ goto _LL842;} _LL842: if((( struct _tunion_struct*) _temp780)->tag
== Cyc_Absyn_Malloc_e_tag){ _LL977: _temp976=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Malloc_e_struct*) _temp780)->f1; goto _LL975; _LL975: _temp974=( void*)((
struct Cyc_Absyn_Malloc_e_struct*) _temp780)->f2; goto _LL843;} else{ goto
_LL844;} _LL844: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL981: _temp980=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp780)->f1; goto _LL979; _LL979: _temp978=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp780)->f2; goto _LL845;} else{ goto
_LL846;} _LL846: if((( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL983: _temp982=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp780)->f1; goto _LL847;} else{ goto _LL848;} _LL848: if((( struct
_tunion_struct*) _temp780)->tag == Cyc_Absyn_Codegen_e_tag){ _LL985: _temp984=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp780)->f1;
goto _LL849;} else{ goto _LL850;} _LL850: if((( struct _tunion_struct*) _temp780)->tag
== Cyc_Absyn_Fill_e_tag){ _LL987: _temp986=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp780)->f1; goto _LL851;} else{ goto _LL781;} _LL783: return 10000; _LL785:
return 10000; _LL787: return 10000; _LL789: { void* _temp988= _temp862; _LL990:
if(( int) _temp988 == Cyc_Absyn_Plus){ goto _LL991;} else{ goto _LL992;} _LL992:
if(( int) _temp988 == Cyc_Absyn_Times){ goto _LL993;} else{ goto _LL994;} _LL994:
if(( int) _temp988 == Cyc_Absyn_Minus){ goto _LL995;} else{ goto _LL996;} _LL996:
if(( int) _temp988 == Cyc_Absyn_Div){ goto _LL997;} else{ goto _LL998;} _LL998:
if(( int) _temp988 == Cyc_Absyn_Mod){ goto _LL999;} else{ goto _LL1000;} _LL1000:
if(( int) _temp988 == Cyc_Absyn_Eq){ goto _LL1001;} else{ goto _LL1002;} _LL1002:
if(( int) _temp988 == Cyc_Absyn_Neq){ goto _LL1003;} else{ goto _LL1004;}
_LL1004: if(( int) _temp988 == Cyc_Absyn_Gt){ goto _LL1005;} else{ goto _LL1006;}
_LL1006: if(( int) _temp988 == Cyc_Absyn_Lt){ goto _LL1007;} else{ goto _LL1008;}
_LL1008: if(( int) _temp988 == Cyc_Absyn_Gte){ goto _LL1009;} else{ goto _LL1010;}
_LL1010: if(( int) _temp988 == Cyc_Absyn_Lte){ goto _LL1011;} else{ goto _LL1012;}
_LL1012: if(( int) _temp988 == Cyc_Absyn_Not){ goto _LL1013;} else{ goto _LL1014;}
_LL1014: if(( int) _temp988 == Cyc_Absyn_Bitnot){ goto _LL1015;} else{ goto
_LL1016;} _LL1016: if(( int) _temp988 == Cyc_Absyn_Bitand){ goto _LL1017;} else{
goto _LL1018;} _LL1018: if(( int) _temp988 == Cyc_Absyn_Bitor){ goto _LL1019;}
else{ goto _LL1020;} _LL1020: if(( int) _temp988 == Cyc_Absyn_Bitxor){ goto
_LL1021;} else{ goto _LL1022;} _LL1022: if(( int) _temp988 == Cyc_Absyn_Bitlshift){
goto _LL1023;} else{ goto _LL1024;} _LL1024: if(( int) _temp988 == Cyc_Absyn_Bitlrshift){
goto _LL1025;} else{ goto _LL1026;} _LL1026: if(( int) _temp988 == Cyc_Absyn_Bitarshift){
goto _LL1027;} else{ goto _LL1028;} _LL1028: if(( int) _temp988 == Cyc_Absyn_Size){
goto _LL1029;} else{ goto _LL1030;} _LL1030: if(( int) _temp988 == Cyc_Absyn_Printf){
goto _LL1031;} else{ goto _LL1032;} _LL1032: if(( int) _temp988 == Cyc_Absyn_Fprintf){
goto _LL1033;} else{ goto _LL1034;} _LL1034: if(( int) _temp988 == Cyc_Absyn_Xprintf){
goto _LL1035;} else{ goto _LL1036;} _LL1036: if(( int) _temp988 == Cyc_Absyn_Scanf){
goto _LL1037;} else{ goto _LL1038;} _LL1038: if(( int) _temp988 == Cyc_Absyn_Fscanf){
goto _LL1039;} else{ goto _LL1040;} _LL1040: if(( int) _temp988 == Cyc_Absyn_Sscanf){
goto _LL1041;} else{ goto _LL989;} _LL991: return 100; _LL993: return 110;
_LL995: return 100; _LL997: return 110; _LL999: return 110; _LL1001: return 70;
_LL1003: return 70; _LL1005: return 80; _LL1007: return 80; _LL1009: return 80;
_LL1011: return 80; _LL1013: return 130; _LL1015: return 130; _LL1017: return 60;
_LL1019: return 40; _LL1021: return 50; _LL1023: return 90; _LL1025: return 80;
_LL1027: return 80; _LL1029: return 140; _LL1031: return 130; _LL1033: return
130; _LL1035: return 130; _LL1037: return 130; _LL1039: return 130; _LL1041:
return 130; _LL989:;} _LL791: return 20; _LL793: return 20; _LL795: return 30;
_LL797: return 10; _LL799: return 140; _LL801: return 140; _LL803: return 130;
_LL805: return Cyc_Absynpp_exp_prec( _temp894); _LL807: return Cyc_Absynpp_exp_prec(
_temp898); _LL809: return 120; _LL811: return 15; _LL813: return 130; _LL815:
return 130; _LL817: return 130; _LL819: return 130; _LL821: return 140; _LL823:
return 140; _LL825: return 140; _LL827: return 150; _LL829: goto _LL831; _LL831:
goto _LL833; _LL833: goto _LL835; _LL835: goto _LL837; _LL837: goto _LL839;
_LL839: goto _LL841; _LL841: goto _LL843; _LL843: return 140; _LL845: return 140;
_LL847: return 10000; _LL849: return 140; _LL851: return 140; _LL781:;} struct
Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp1042=(
void*) e->r; void* _temp1118; void* _temp1120; struct Cyc_Absyn_Vardecl*
_temp1122; struct _tuple0* _temp1124; void* _temp1126; struct Cyc_Absyn_Vardecl*
_temp1128; struct _tuple0* _temp1130; void* _temp1132; struct _tuple0* _temp1134;
struct _tuple0* _temp1136; struct Cyc_List_List* _temp1138; void* _temp1140;
struct Cyc_Absyn_Exp* _temp1142; struct Cyc_Core_Opt* _temp1144; struct Cyc_Absyn_Exp*
_temp1146; void* _temp1148; struct Cyc_Absyn_Exp* _temp1150; struct Cyc_Absyn_Exp*
_temp1152; struct Cyc_Absyn_Exp* _temp1154; struct Cyc_Absyn_Exp* _temp1156;
struct Cyc_Absyn_Exp* _temp1158; struct Cyc_Absyn_Exp* _temp1160; struct Cyc_List_List*
_temp1162; struct Cyc_Absyn_Exp* _temp1164; struct Cyc_List_List* _temp1166;
struct Cyc_Absyn_Exp* _temp1168; struct Cyc_Absyn_Exp* _temp1170; struct Cyc_Absyn_Exp*
_temp1172; struct Cyc_List_List* _temp1174; struct Cyc_Absyn_Exp* _temp1176;
struct Cyc_Absyn_Exp* _temp1178; void* _temp1180; struct Cyc_Absyn_Exp*
_temp1182; struct Cyc_Absyn_Exp* _temp1184; struct Cyc_Absyn_Exp* _temp1186;
void* _temp1188; struct Cyc_Absyn_Exp* _temp1190; struct Cyc_Absyn_Exp*
_temp1192; struct _tagged_string* _temp1194; struct Cyc_Absyn_Exp* _temp1196;
struct _tagged_string* _temp1198; struct Cyc_Absyn_Exp* _temp1200; struct Cyc_Absyn_Exp*
_temp1202; struct Cyc_Absyn_Exp* _temp1204; struct Cyc_List_List* _temp1206;
struct Cyc_List_List* _temp1208; struct _tuple1* _temp1210; struct Cyc_List_List*
_temp1212; struct Cyc_Absyn_Exp* _temp1214; struct Cyc_Absyn_Exp* _temp1216;
struct Cyc_Absyn_Vardecl* _temp1218; struct Cyc_Absyn_Structdecl* _temp1220;
struct Cyc_List_List* _temp1222; struct Cyc_Core_Opt* _temp1224; struct _tuple0*
_temp1226; struct Cyc_Absyn_Tunionfield* _temp1228; struct Cyc_Absyn_Tuniondecl*
_temp1230; struct Cyc_List_List* _temp1232; struct Cyc_Core_Opt* _temp1234;
struct Cyc_Core_Opt* _temp1236; struct Cyc_Absyn_Tunionfield* _temp1238; struct
Cyc_Absyn_XTuniondecl* _temp1240; struct Cyc_List_List* _temp1242; struct Cyc_Core_Opt*
_temp1244; struct Cyc_Absyn_Enumfield* _temp1246; struct Cyc_Absyn_Enumdecl*
_temp1248; struct _tuple0* _temp1250; void* _temp1252; struct Cyc_Absyn_Exp*
_temp1254; struct Cyc_List_List* _temp1256; struct Cyc_Core_Opt* _temp1258;
struct Cyc_Absyn_Stmt* _temp1260; struct Cyc_Absyn_Fndecl* _temp1262; struct Cyc_Absyn_Exp*
_temp1264; _LL1044: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Const_e_tag){
_LL1119: _temp1118=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1042)->f1;
goto _LL1045;} else{ goto _LL1046;} _LL1046: if((( struct _tunion_struct*)
_temp1042)->tag == Cyc_Absyn_Var_e_tag){ _LL1125: _temp1124=( struct _tuple0*)((
struct Cyc_Absyn_Var_e_struct*) _temp1042)->f1; goto _LL1121; _LL1121: _temp1120=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1042)->f2; if(( unsigned int)
_temp1120 > 1u?(( struct _tunion_struct*) _temp1120)->tag == Cyc_Absyn_Local_b_tag:
0){ _LL1123: _temp1122=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*)
_temp1120)->f1; goto _LL1047;} else{ goto _LL1048;}} else{ goto _LL1048;}
_LL1048: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Var_e_tag){
_LL1131: _temp1130=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1042)->f1; goto _LL1127; _LL1127: _temp1126=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1042)->f2; if(( unsigned int) _temp1126 > 1u?(( struct _tunion_struct*)
_temp1126)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL1129: _temp1128=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp1126)->f1; goto _LL1049;} else{ goto
_LL1050;}} else{ goto _LL1050;} _LL1050: if((( struct _tunion_struct*) _temp1042)->tag
== Cyc_Absyn_Var_e_tag){ _LL1135: _temp1134=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1042)->f1; goto _LL1133; _LL1133: _temp1132=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1042)->f2; goto _LL1051;} else{ goto _LL1052;} _LL1052: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1137:
_temp1136=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1042)->f1;
goto _LL1053;} else{ goto _LL1054;} _LL1054: if((( struct _tunion_struct*)
_temp1042)->tag == Cyc_Absyn_Primop_e_tag){ _LL1141: _temp1140=( void*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp1042)->f1; goto _LL1139; _LL1139: _temp1138=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1042)->f2; goto
_LL1055;} else{ goto _LL1056;} _LL1056: if((( struct _tunion_struct*) _temp1042)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL1147: _temp1146=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1042)->f1; goto _LL1145; _LL1145:
_temp1144=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1042)->f2; goto _LL1143; _LL1143: _temp1142=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1042)->f3; goto _LL1057;} else{ goto
_LL1058;} _LL1058: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Increment_e_tag){
_LL1151: _temp1150=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1042)->f1; goto _LL1149; _LL1149: _temp1148=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1042)->f2; goto _LL1059;} else{ goto _LL1060;} _LL1060: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Conditional_e_tag){ _LL1157:
_temp1156=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1042)->f1; goto _LL1155; _LL1155: _temp1154=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1042)->f2; goto _LL1153; _LL1153:
_temp1152=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1042)->f3; goto _LL1061;} else{ goto _LL1062;} _LL1062: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1161: _temp1160=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1042)->f1; goto
_LL1159; _LL1159: _temp1158=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1042)->f2; goto _LL1063;} else{ goto _LL1064;} _LL1064: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1165:
_temp1164=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1042)->f1; goto _LL1163; _LL1163: _temp1162=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1042)->f2; goto _LL1065;} else{
goto _LL1066;} _LL1066: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1169: _temp1168=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1042)->f1; goto _LL1167; _LL1167: _temp1166=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1042)->f2; goto _LL1067;} else{ goto
_LL1068;} _LL1068: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Throw_e_tag){
_LL1171: _temp1170=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp1042)->f1; goto _LL1069;} else{ goto _LL1070;} _LL1070: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1173:
_temp1172=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1042)->f1; goto _LL1071;} else{ goto _LL1072;} _LL1072: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1177:
_temp1176=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1042)->f1; goto _LL1175; _LL1175: _temp1174=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1042)->f2; goto _LL1073;} else{
goto _LL1074;} _LL1074: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Cast_e_tag){
_LL1181: _temp1180=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1042)->f1;
goto _LL1179; _LL1179: _temp1178=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1042)->f2; goto _LL1075;} else{ goto _LL1076;} _LL1076: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Address_e_tag){ _LL1183: _temp1182=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp1042)->f1;
goto _LL1077;} else{ goto _LL1078;} _LL1078: if((( struct _tunion_struct*)
_temp1042)->tag == Cyc_Absyn_New_e_tag){ _LL1187: _temp1186=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1042)->f1; goto _LL1185; _LL1185: _temp1184=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1042)->f2; goto
_LL1079;} else{ goto _LL1080;} _LL1080: if((( struct _tunion_struct*) _temp1042)->tag
== Cyc_Absyn_Sizeoftyp_e_tag){ _LL1189: _temp1188=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1042)->f1; goto _LL1081;} else{ goto _LL1082;} _LL1082: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL1191:
_temp1190=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1042)->f1; goto _LL1083;} else{ goto _LL1084;} _LL1084: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Deref_e_tag){ _LL1193: _temp1192=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp1042)->f1; goto
_LL1085;} else{ goto _LL1086;} _LL1086: if((( struct _tunion_struct*) _temp1042)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL1197: _temp1196=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1042)->f1; goto _LL1195; _LL1195:
_temp1194=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1042)->f2; goto _LL1087;} else{ goto _LL1088;} _LL1088: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL1201:
_temp1200=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1042)->f1; goto _LL1199; _LL1199: _temp1198=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1042)->f2; goto _LL1089;} else{
goto _LL1090;} _LL1090: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1205: _temp1204=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1042)->f1; goto _LL1203; _LL1203: _temp1202=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1042)->f2; goto _LL1091;} else{ goto
_LL1092;} _LL1092: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1207: _temp1206=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1042)->f1; goto _LL1093;} else{ goto _LL1094;} _LL1094: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1211:
_temp1210=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1042)->f1;
goto _LL1209; _LL1209: _temp1208=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1042)->f2; goto _LL1095;} else{ goto _LL1096;} _LL1096: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Array_e_tag){ _LL1213: _temp1212=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1042)->f1; goto
_LL1097;} else{ goto _LL1098;} _LL1098: if((( struct _tunion_struct*) _temp1042)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1219: _temp1218=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1042)->f1; goto _LL1217; _LL1217:
_temp1216=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1042)->f2; goto _LL1215; _LL1215: _temp1214=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1042)->f3; goto _LL1099;} else{
goto _LL1100;} _LL1100: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Struct_e_tag){
_LL1227: _temp1226=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1042)->f1; goto _LL1225; _LL1225: _temp1224=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1042)->f2; goto _LL1223; _LL1223:
_temp1222=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1042)->f3; goto _LL1221; _LL1221: _temp1220=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1042)->f4; goto _LL1101;} else{ goto
_LL1102;} _LL1102: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Tunion_e_tag){
_LL1237: _temp1236=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1042)->f1; goto _LL1235; _LL1235: _temp1234=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1042)->f2; goto _LL1233; _LL1233:
_temp1232=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1042)->f3; goto _LL1231; _LL1231: _temp1230=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1042)->f4; goto _LL1229; _LL1229:
_temp1228=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1042)->f5; goto _LL1103;} else{ goto _LL1104;} _LL1104: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1245: _temp1244=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1042)->f1; goto
_LL1243; _LL1243: _temp1242=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1042)->f2; goto _LL1241; _LL1241: _temp1240=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1042)->f3; goto _LL1239; _LL1239:
_temp1238=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1042)->f4; goto _LL1105;} else{ goto _LL1106;} _LL1106: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Enum_e_tag){ _LL1251: _temp1250=(
struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1042)->f1; goto _LL1249;
_LL1249: _temp1248=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1042)->f2; goto _LL1247; _LL1247: _temp1246=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1042)->f3; goto _LL1107;} else{ goto
_LL1108;} _LL1108: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1255: _temp1254=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1042)->f1; goto _LL1253; _LL1253: _temp1252=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1042)->f2; goto _LL1109;} else{ goto _LL1110;} _LL1110: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1259:
_temp1258=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1042)->f1; goto _LL1257; _LL1257: _temp1256=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1042)->f2; goto _LL1111;} else{
goto _LL1112;} _LL1112: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL1261: _temp1260=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1042)->f1; goto _LL1113;} else{ goto _LL1114;} _LL1114: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1263: _temp1262=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp1042)->f1;
goto _LL1115;} else{ goto _LL1116;} _LL1116: if((( struct _tunion_struct*)
_temp1042)->tag == Cyc_Absyn_Fill_e_tag){ _LL1265: _temp1264=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp1042)->f1; goto _LL1117;} else{ goto
_LL1043;} _LL1045: s= Cyc_Absynpp_cnst2doc( _temp1118); goto _LL1043; _LL1047: s=
Cyc_Absynpp_qvar2doc( _temp1124); goto _LL1043; _LL1049: s= Cyc_Absynpp_qvar2doc(
_temp1130); goto _LL1043; _LL1051: s= Cyc_Absynpp_qvar2doc( _temp1134); goto
_LL1043; _LL1053: s= Cyc_Absynpp_qvar2doc( _temp1136); goto _LL1043; _LL1055: s=
Cyc_Absynpp_primapp2doc( myprec, _temp1140, _temp1138); goto _LL1043; _LL1057: s=
Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec, _temp1146), Cyc_PP_text((
struct _tagged_string)({ char* _temp1266=( char*)" "; struct _tagged_string
_temp1267; _temp1267.curr= _temp1266; _temp1267.base= _temp1266; _temp1267.last_plus_one=
_temp1266 + 2; _temp1267;})), _temp1144 == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp1268=( char*)""; struct _tagged_string _temp1269;
_temp1269.curr= _temp1268; _temp1269.base= _temp1268; _temp1269.last_plus_one=
_temp1268 + 1; _temp1269;})): Cyc_Absynpp_prim2doc(( void*)({ struct Cyc_Core_Opt*
_temp1270= _temp1144; if( _temp1270 == 0){ _throw( Null_Exception);} _temp1270->v;})),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1271=( char*)"= "; struct
_tagged_string _temp1272; _temp1272.curr= _temp1271; _temp1272.base= _temp1271;
_temp1272.last_plus_one= _temp1271 + 3; _temp1272;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1142)); goto _LL1043; _LL1059: { struct Cyc_PP_Doc* es= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1150);{ void* _temp1273= _temp1148; _LL1275: if(( int) _temp1273 ==
Cyc_Absyn_PreInc){ goto _LL1276;} else{ goto _LL1277;} _LL1277: if(( int)
_temp1273 == Cyc_Absyn_PreDec){ goto _LL1278;} else{ goto _LL1279;} _LL1279: if((
int) _temp1273 == Cyc_Absyn_PostInc){ goto _LL1280;} else{ goto _LL1281;}
_LL1281: if(( int) _temp1273 == Cyc_Absyn_PostDec){ goto _LL1282;} else{ goto
_LL1274;} _LL1276: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({
char* _temp1283=( char*)"++"; struct _tagged_string _temp1284; _temp1284.curr=
_temp1283; _temp1284.base= _temp1283; _temp1284.last_plus_one= _temp1283 + 3;
_temp1284;})), es); goto _LL1274; _LL1278: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1285=( char*)"--"; struct _tagged_string
_temp1286; _temp1286.curr= _temp1285; _temp1286.base= _temp1285; _temp1286.last_plus_one=
_temp1285 + 3; _temp1286;})), es); goto _LL1274; _LL1280: s= Cyc_Absynpp_cat2(
es, Cyc_PP_text(( struct _tagged_string)({ char* _temp1287=( char*)"++"; struct
_tagged_string _temp1288; _temp1288.curr= _temp1287; _temp1288.base= _temp1287;
_temp1288.last_plus_one= _temp1287 + 3; _temp1288;}))); goto _LL1274; _LL1282: s=
Cyc_Absynpp_cat2( es, Cyc_PP_text(( struct _tagged_string)({ char* _temp1289=(
char*)"--"; struct _tagged_string _temp1290; _temp1290.curr= _temp1289;
_temp1290.base= _temp1289; _temp1290.last_plus_one= _temp1289 + 3; _temp1290;})));
goto _LL1274; _LL1274:;} goto _LL1043;} _LL1061:{ struct _tuple8 _temp1292=({
struct _tuple8 _temp1291; _temp1291.f1=( void*) _temp1154->r; _temp1291.f2=(
void*) _temp1152->r; _temp1291;}); _LL1294: goto _LL1295; _LL1295: s= Cyc_Absynpp_cat5(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1156), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1296=( char*)" ? "; struct _tagged_string _temp1297;
_temp1297.curr= _temp1296; _temp1297.base= _temp1296; _temp1297.last_plus_one=
_temp1296 + 4; _temp1297;})), Cyc_Absynpp_exp2doc_prec( 0, _temp1154), Cyc_PP_text((
struct _tagged_string)({ char* _temp1298=( char*)" : "; struct _tagged_string
_temp1299; _temp1299.curr= _temp1298; _temp1299.base= _temp1298; _temp1299.last_plus_one=
_temp1298 + 4; _temp1299;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1152));
goto _LL1293; _LL1293:;} goto _LL1043; _LL1063: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1160), Cyc_PP_text(( struct _tagged_string)({ char* _temp1300=(
char*)", "; struct _tagged_string _temp1301; _temp1301.curr= _temp1300;
_temp1301.base= _temp1300; _temp1301.last_plus_one= _temp1300 + 3; _temp1301;})),
Cyc_Absynpp_exp2doc_prec( myprec, _temp1158)); goto _LL1043; _LL1065: s= Cyc_Absynpp_cat4(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1164), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1302=( char*)"("; struct _tagged_string _temp1303;
_temp1303.curr= _temp1302; _temp1303.base= _temp1302; _temp1303.last_plus_one=
_temp1302 + 2; _temp1303;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1162), Cyc_PP_text((
struct _tagged_string)({ char* _temp1304=( char*)")"; struct _tagged_string
_temp1305; _temp1305.curr= _temp1304; _temp1305.base= _temp1304; _temp1305.last_plus_one=
_temp1304 + 2; _temp1305;}))); goto _LL1043; _LL1067: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1168), Cyc_PP_text(( struct _tagged_string)({ char* _temp1306=(
char*)"("; struct _tagged_string _temp1307; _temp1307.curr= _temp1306; _temp1307.base=
_temp1306; _temp1307.last_plus_one= _temp1306 + 2; _temp1307;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1166), Cyc_PP_text(( struct _tagged_string)({ char* _temp1308=( char*)")";
struct _tagged_string _temp1309; _temp1309.curr= _temp1308; _temp1309.base=
_temp1308; _temp1309.last_plus_one= _temp1308 + 2; _temp1309;}))); goto _LL1043;
_LL1069: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1310=( char*)"throw "; struct _tagged_string _temp1311; _temp1311.curr=
_temp1310; _temp1311.base= _temp1310; _temp1311.last_plus_one= _temp1310 + 7;
_temp1311;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1170)); goto _LL1043;
_LL1071: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1172); goto _LL1043; _LL1073:
s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1176); goto _LL1043; _LL1075: s= Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1312=( char*)"("; struct
_tagged_string _temp1313; _temp1313.curr= _temp1312; _temp1313.base= _temp1312;
_temp1313.last_plus_one= _temp1312 + 2; _temp1313;})), Cyc_Absynpp_typ2doc(
_temp1180), Cyc_PP_text(( struct _tagged_string)({ char* _temp1314=( char*)")";
struct _tagged_string _temp1315; _temp1315.curr= _temp1314; _temp1315.base=
_temp1314; _temp1315.last_plus_one= _temp1314 + 2; _temp1315;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1178)); goto _LL1043; _LL1077: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1316=( char*)"&"; struct _tagged_string
_temp1317; _temp1317.curr= _temp1316; _temp1317.base= _temp1316; _temp1317.last_plus_one=
_temp1316 + 2; _temp1317;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1182));
goto _LL1043; _LL1079: if( _temp1186 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1318=( char*)"new "; struct _tagged_string
_temp1319; _temp1319.curr= _temp1318; _temp1319.base= _temp1318; _temp1319.last_plus_one=
_temp1318 + 5; _temp1319;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1184));}
else{ s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1320=( char*)"rnew("; struct _tagged_string _temp1321; _temp1321.curr=
_temp1320; _temp1321.base= _temp1320; _temp1321.last_plus_one= _temp1320 + 6;
_temp1321;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1322= _temp1186; if( _temp1322 == 0){ _throw( Null_Exception);} _temp1322;})),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1323=( char*)") "; struct
_tagged_string _temp1324; _temp1324.curr= _temp1323; _temp1324.base= _temp1323;
_temp1324.last_plus_one= _temp1323 + 3; _temp1324;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1184));} goto _LL1043; _LL1081: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1325=( char*)"sizeof("; struct
_tagged_string _temp1326; _temp1326.curr= _temp1325; _temp1326.base= _temp1325;
_temp1326.last_plus_one= _temp1325 + 8; _temp1326;})), Cyc_Absynpp_typ2doc(
_temp1188), Cyc_PP_text(( struct _tagged_string)({ char* _temp1327=( char*)")";
struct _tagged_string _temp1328; _temp1328.curr= _temp1327; _temp1328.base=
_temp1327; _temp1328.last_plus_one= _temp1327 + 2; _temp1328;}))); goto _LL1043;
_LL1083: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1329=( char*)"sizeof("; struct _tagged_string _temp1330; _temp1330.curr=
_temp1329; _temp1330.base= _temp1329; _temp1330.last_plus_one= _temp1329 + 8;
_temp1330;})), Cyc_Absynpp_exp2doc( _temp1190), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1331=( char*)")"; struct _tagged_string _temp1332;
_temp1332.curr= _temp1331; _temp1332.base= _temp1331; _temp1332.last_plus_one=
_temp1331 + 2; _temp1332;}))); goto _LL1043; _LL1085: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1333=( char*)"*"; struct _tagged_string
_temp1334; _temp1334.curr= _temp1333; _temp1334.base= _temp1333; _temp1334.last_plus_one=
_temp1333 + 2; _temp1334;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1192));
goto _LL1043; _LL1087: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1196), Cyc_PP_text(( struct _tagged_string)({ char* _temp1335=( char*)".";
struct _tagged_string _temp1336; _temp1336.curr= _temp1335; _temp1336.base=
_temp1335; _temp1336.last_plus_one= _temp1335 + 2; _temp1336;})), Cyc_Absynpp_textptr(
_temp1194)); goto _LL1043; _LL1089: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1200), Cyc_PP_text(( struct _tagged_string)({ char* _temp1337=(
char*)"->"; struct _tagged_string _temp1338; _temp1338.curr= _temp1337;
_temp1338.base= _temp1337; _temp1338.last_plus_one= _temp1337 + 3; _temp1338;})),
Cyc_Absynpp_textptr( _temp1198)); goto _LL1043; _LL1091: s= Cyc_Absynpp_cat4(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1204), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1339=( char*)"["; struct _tagged_string _temp1340;
_temp1340.curr= _temp1339; _temp1340.base= _temp1339; _temp1340.last_plus_one=
_temp1339 + 2; _temp1340;})), Cyc_Absynpp_exp2doc( _temp1202), Cyc_PP_text((
struct _tagged_string)({ char* _temp1341=( char*)"]"; struct _tagged_string
_temp1342; _temp1342.curr= _temp1341; _temp1342.base= _temp1341; _temp1342.last_plus_one=
_temp1341 + 2; _temp1342;}))); goto _LL1043; _LL1093: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1343=( char*)"$("; struct _tagged_string
_temp1344; _temp1344.curr= _temp1343; _temp1344.base= _temp1343; _temp1344.last_plus_one=
_temp1343 + 3; _temp1344;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1206), Cyc_PP_text((
struct _tagged_string)({ char* _temp1345=( char*)")"; struct _tagged_string
_temp1346; _temp1346.curr= _temp1345; _temp1346.base= _temp1345; _temp1346.last_plus_one=
_temp1345 + 2; _temp1346;}))); goto _LL1043; _LL1095: s= Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1347=( char*)"("; struct _tagged_string
_temp1348; _temp1348.curr= _temp1347; _temp1348.base= _temp1347; _temp1348.last_plus_one=
_temp1347 + 2; _temp1348;})), Cyc_Absynpp_typ2doc((* _temp1210).f3), Cyc_PP_text((
struct _tagged_string)({ char* _temp1349=( char*)")"; struct _tagged_string
_temp1350; _temp1350.curr= _temp1349; _temp1350.base= _temp1349; _temp1350.last_plus_one=
_temp1349 + 2; _temp1350;})), Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp1351=( char*)"{"; struct _tagged_string _temp1352; _temp1352.curr=
_temp1351; _temp1352.base= _temp1351; _temp1352.last_plus_one= _temp1351 + 2;
_temp1352;}),( struct _tagged_string)({ char* _temp1353=( char*)"}"; struct
_tagged_string _temp1354; _temp1354.curr= _temp1353; _temp1354.base= _temp1353;
_temp1354.last_plus_one= _temp1353 + 2; _temp1354;}),( struct _tagged_string)({
char* _temp1355=( char*)","; struct _tagged_string _temp1356; _temp1356.curr=
_temp1355; _temp1356.base= _temp1355; _temp1356.last_plus_one= _temp1355 + 2;
_temp1356;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1208))); goto _LL1043; _LL1097: s= Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1357=( char*)"{"; struct _tagged_string _temp1358;
_temp1358.curr= _temp1357; _temp1358.base= _temp1357; _temp1358.last_plus_one=
_temp1357 + 2; _temp1358;}),( struct _tagged_string)({ char* _temp1359=( char*)"}";
struct _tagged_string _temp1360; _temp1360.curr= _temp1359; _temp1360.base=
_temp1359; _temp1360.last_plus_one= _temp1359 + 2; _temp1360;}),( struct
_tagged_string)({ char* _temp1361=( char*)","; struct _tagged_string _temp1362;
_temp1362.curr= _temp1361; _temp1362.base= _temp1361; _temp1362.last_plus_one=
_temp1361 + 2; _temp1362;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1212)); goto _LL1043; _LL1099: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1363=( char*)"{for "; struct _tagged_string
_temp1364; _temp1364.curr= _temp1363; _temp1364.base= _temp1363; _temp1364.last_plus_one=
_temp1363 + 6; _temp1364;})), Cyc_PP_text(*(* _temp1218->name).f2), Cyc_PP_text((
struct _tagged_string)({ char* _temp1365=( char*)" < "; struct _tagged_string
_temp1366; _temp1366.curr= _temp1365; _temp1366.base= _temp1365; _temp1366.last_plus_one=
_temp1365 + 4; _temp1366;})), Cyc_Absynpp_exp2doc( _temp1216), Cyc_PP_text((
struct _tagged_string)({ char* _temp1367=( char*)" : "; struct _tagged_string
_temp1368; _temp1368.curr= _temp1367; _temp1368.base= _temp1367; _temp1368.last_plus_one=
_temp1367 + 4; _temp1368;})), Cyc_Absynpp_exp2doc( _temp1214), Cyc_PP_text((
struct _tagged_string)({ char* _temp1369=( char*)"}"; struct _tagged_string
_temp1370; _temp1370.curr= _temp1369; _temp1370.base= _temp1369; _temp1370.last_plus_one=
_temp1369 + 2; _temp1370;}))); goto _LL1043; _LL1101: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1226), Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1371=( char*)"{";
struct _tagged_string _temp1372; _temp1372.curr= _temp1371; _temp1372.base=
_temp1371; _temp1372.last_plus_one= _temp1371 + 2; _temp1372;}),( struct
_tagged_string)({ char* _temp1373=( char*)"}"; struct _tagged_string _temp1374;
_temp1374.curr= _temp1373; _temp1374.base= _temp1373; _temp1374.last_plus_one=
_temp1373 + 2; _temp1374;}),( struct _tagged_string)({ char* _temp1375=( char*)",";
struct _tagged_string _temp1376; _temp1376.curr= _temp1375; _temp1376.base=
_temp1375; _temp1376.last_plus_one= _temp1375 + 2; _temp1376;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1222))); goto _LL1043; _LL1103: _temp1242= _temp1232;
_temp1238= _temp1228; goto _LL1105; _LL1105: if( _temp1242 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1238->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1238->name),
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1377=( char*)"(";
struct _tagged_string _temp1378; _temp1378.curr= _temp1377; _temp1378.base=
_temp1377; _temp1378.last_plus_one= _temp1377 + 2; _temp1378;}),( struct
_tagged_string)({ char* _temp1379=( char*)")"; struct _tagged_string _temp1380;
_temp1380.curr= _temp1379; _temp1380.base= _temp1379; _temp1380.last_plus_one=
_temp1379 + 2; _temp1380;}),( struct _tagged_string)({ char* _temp1381=( char*)",";
struct _tagged_string _temp1382; _temp1382.curr= _temp1381; _temp1382.base=
_temp1381; _temp1382.last_plus_one= _temp1381 + 2; _temp1382;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp1242)));} goto _LL1043; _LL1107: s= Cyc_Absynpp_qvar2doc(
_temp1250); goto _LL1043; _LL1109: if( _temp1254 == 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1383=( char*)"malloc(sizeof("; struct
_tagged_string _temp1384; _temp1384.curr= _temp1383; _temp1384.base= _temp1383;
_temp1384.last_plus_one= _temp1383 + 15; _temp1384;})), Cyc_Absynpp_typ2doc(
_temp1252), Cyc_PP_text(( struct _tagged_string)({ char* _temp1385=( char*)"))";
struct _tagged_string _temp1386; _temp1386.curr= _temp1385; _temp1386.base=
_temp1385; _temp1386.last_plus_one= _temp1385 + 3; _temp1386;})));} else{ s= Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1387=( char*)"rmalloc(";
struct _tagged_string _temp1388; _temp1388.curr= _temp1387; _temp1388.base=
_temp1387; _temp1388.last_plus_one= _temp1387 + 9; _temp1388;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1389= _temp1254; if(
_temp1389 == 0){ _throw( Null_Exception);} _temp1389;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1390=( char*)","; struct _tagged_string _temp1391;
_temp1391.curr= _temp1390; _temp1391.base= _temp1390; _temp1391.last_plus_one=
_temp1390 + 2; _temp1391;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1392=( char*)"sizeof("; struct _tagged_string _temp1393; _temp1393.curr=
_temp1392; _temp1393.base= _temp1392; _temp1393.last_plus_one= _temp1392 + 8;
_temp1393;})), Cyc_Absynpp_typ2doc( _temp1252), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1394=( char*)"))"; struct _tagged_string _temp1395;
_temp1395.curr= _temp1394; _temp1395.base= _temp1394; _temp1395.last_plus_one=
_temp1394 + 3; _temp1395;})));} goto _LL1043; _LL1111: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1396=( char*)"{"; struct _tagged_string
_temp1397; _temp1397.curr= _temp1396; _temp1397.base= _temp1396; _temp1397.last_plus_one=
_temp1396 + 2; _temp1397;}),( struct _tagged_string)({ char* _temp1398=( char*)"}";
struct _tagged_string _temp1399; _temp1399.curr= _temp1398; _temp1399.base=
_temp1398; _temp1399.last_plus_one= _temp1398 + 2; _temp1399;}),( struct
_tagged_string)({ char* _temp1400=( char*)","; struct _tagged_string _temp1401;
_temp1401.curr= _temp1400; _temp1401.base= _temp1400; _temp1401.last_plus_one=
_temp1400 + 2; _temp1401;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1256)); goto _LL1043; _LL1113: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1402=( char*)"({ "; struct _tagged_string _temp1403;
_temp1403.curr= _temp1402; _temp1403.base= _temp1402; _temp1403.last_plus_one=
_temp1402 + 4; _temp1403;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1260)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1404=( char*)" })"; struct
_tagged_string _temp1405; _temp1405.curr= _temp1404; _temp1405.base= _temp1404;
_temp1405.last_plus_one= _temp1404 + 4; _temp1405;}))); goto _LL1043; _LL1115: s=
Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1406=( char*)"codegen(";
struct _tagged_string _temp1407; _temp1407.curr= _temp1406; _temp1407.base=
_temp1406; _temp1407.last_plus_one= _temp1406 + 9; _temp1407;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1408=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1408->r=( void*)(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp1409=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1409[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1410; _temp1410.tag= Cyc_Absyn_Fn_d_tag; _temp1410.f1= _temp1262; _temp1410;});
_temp1409;})); _temp1408->loc= e->loc; _temp1408;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1411=( char*)")"; struct _tagged_string _temp1412;
_temp1412.curr= _temp1411; _temp1412.base= _temp1411; _temp1412.last_plus_one=
_temp1411 + 2; _temp1412;}))); goto _LL1043; _LL1117: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1413=( char*)"fill("; struct _tagged_string
_temp1414; _temp1414.curr= _temp1413; _temp1414.base= _temp1413; _temp1414.last_plus_one=
_temp1413 + 6; _temp1414;})), Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1264)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1415=( char*)")"; struct
_tagged_string _temp1416; _temp1416.curr= _temp1415; _temp1416.base= _temp1415;
_temp1416.last_plus_one= _temp1415 + 2; _temp1416;}))); goto _LL1043; _LL1043:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp1417=( char*)"("; struct _tagged_string _temp1418; _temp1418.curr=
_temp1417; _temp1418.base= _temp1417; _temp1418.last_plus_one= _temp1417 + 2;
_temp1418;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp1419=( char*)")";
struct _tagged_string _temp1420; _temp1420.curr= _temp1419; _temp1420.base=
_temp1419; _temp1420.last_plus_one= _temp1419 + 2; _temp1420;})));} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1421= d;
struct Cyc_Absyn_Exp* _temp1427; struct _tagged_string* _temp1429; _LL1423: if(((
struct _tunion_struct*) _temp1421)->tag == Cyc_Absyn_ArrayElement_tag){ _LL1428:
_temp1427=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1421)->f1; goto _LL1424;} else{ goto _LL1425;} _LL1425: if((( struct
_tunion_struct*) _temp1421)->tag == Cyc_Absyn_FieldName_tag){ _LL1430: _temp1429=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1421)->f1;
goto _LL1426;} else{ goto _LL1422;} _LL1424: return Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1431=( char*)".["; struct _tagged_string
_temp1432; _temp1432.curr= _temp1431; _temp1432.base= _temp1431; _temp1432.last_plus_one=
_temp1431 + 3; _temp1432;})), Cyc_Absynpp_exp2doc( _temp1427), Cyc_PP_text((
struct _tagged_string)({ char* _temp1433=( char*)"]"; struct _tagged_string
_temp1434; _temp1434.curr= _temp1433; _temp1434.base= _temp1433; _temp1434.last_plus_one=
_temp1433 + 2; _temp1434;}))); _LL1426: return Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1435=( char*)"."; struct _tagged_string
_temp1436; _temp1436.curr= _temp1435; _temp1436.base= _temp1435; _temp1436.last_plus_one=
_temp1435 + 2; _temp1436;})), Cyc_Absynpp_textptr( _temp1429)); _LL1422:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 == 0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp1437=( char*)""; struct _tagged_string
_temp1438; _temp1438.curr= _temp1437; _temp1438.base= _temp1437; _temp1438.last_plus_one=
_temp1437 + 1; _temp1438;}),( struct _tagged_string)({ char* _temp1439=( char*)"=";
struct _tagged_string _temp1440; _temp1440.curr= _temp1439; _temp1440.base=
_temp1439; _temp1440.last_plus_one= _temp1439 + 2; _temp1440;}),( struct
_tagged_string)({ char* _temp1441=( char*)"="; struct _tagged_string _temp1442;
_temp1442.curr= _temp1441; _temp1442.base= _temp1441; _temp1442.last_plus_one=
_temp1441 + 2; _temp1442;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)), Cyc_Absynpp_exp2doc((* de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1443=( char*)""; struct _tagged_string _temp1444;
_temp1444.curr= _temp1443; _temp1444.base= _temp1443; _temp1444.last_plus_one=
_temp1443 + 1; _temp1444;}),( struct _tagged_string)({ char* _temp1445=( char*)"";
struct _tagged_string _temp1446; _temp1446.curr= _temp1445; _temp1446.base=
_temp1445; _temp1446.last_plus_one= _temp1445 + 1; _temp1446;}),( struct
_tagged_string)({ char* _temp1447=( char*)","; struct _tagged_string _temp1448;
_temp1448.curr= _temp1447; _temp1448.base= _temp1447; _temp1448.last_plus_one=
_temp1447 + 2; _temp1448;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1449= c; char _temp1467; void* _temp1469; short _temp1471;
void* _temp1473; int _temp1475; void* _temp1477; int _temp1479; void* _temp1481;
long long _temp1483; void* _temp1485; struct _tagged_string _temp1487; struct
_tagged_string _temp1489; _LL1451: if(( unsigned int) _temp1449 > 1u?(( struct
_tunion_struct*) _temp1449)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL1470: _temp1469=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp1449)->f1; goto _LL1468; _LL1468:
_temp1467=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp1449)->f2; goto
_LL1452;} else{ goto _LL1453;} _LL1453: if(( unsigned int) _temp1449 > 1u?((
struct _tunion_struct*) _temp1449)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL1474:
_temp1473=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1449)->f1; goto
_LL1472; _LL1472: _temp1471=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp1449)->f2; goto _LL1454;} else{ goto _LL1455;} _LL1455: if(( unsigned int)
_temp1449 > 1u?(( struct _tunion_struct*) _temp1449)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL1478: _temp1477=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1449)->f1;
if(( int) _temp1477 == Cyc_Absyn_Signed){ goto _LL1476;} else{ goto _LL1457;}
_LL1476: _temp1475=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1449)->f2; goto
_LL1456;} else{ goto _LL1457;} _LL1457: if(( unsigned int) _temp1449 > 1u?((
struct _tunion_struct*) _temp1449)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL1482:
_temp1481=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1449)->f1; if(( int)
_temp1481 == Cyc_Absyn_Unsigned){ goto _LL1480;} else{ goto _LL1459;} _LL1480:
_temp1479=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1449)->f2; goto _LL1458;}
else{ goto _LL1459;} _LL1459: if(( unsigned int) _temp1449 > 1u?(( struct
_tunion_struct*) _temp1449)->tag == Cyc_Absyn_LongLong_c_tag: 0){ _LL1486:
_temp1485=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1449)->f1; goto
_LL1484; _LL1484: _temp1483=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1449)->f2; goto _LL1460;} else{ goto _LL1461;} _LL1461: if(( unsigned int)
_temp1449 > 1u?(( struct _tunion_struct*) _temp1449)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL1488: _temp1487=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp1449)->f1; goto _LL1462;} else{ goto _LL1463;} _LL1463: if(( int) _temp1449
== Cyc_Absyn_Null_c){ goto _LL1464;} else{ goto _LL1465;} _LL1465: if((
unsigned int) _temp1449 > 1u?(( struct _tunion_struct*) _temp1449)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1490: _temp1489=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1449)->f1; goto _LL1466;} else{ goto _LL1450;} _LL1452: return Cyc_PP_text(({
struct _tagged_string _temp1491= Cyc_Absynpp_char_escape( _temp1467); xprintf("'%.*s'",
_temp1491.last_plus_one - _temp1491.curr, _temp1491.curr);})); _LL1454: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1471)); _LL1456: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1475)); _LL1458: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1479)); _LL1460: return Cyc_PP_text(( struct _tagged_string)({
char* _temp1492=( char*)"<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp1493; _temp1493.curr= _temp1492; _temp1493.base= _temp1492; _temp1493.last_plus_one=
_temp1492 + 27; _temp1493;})); _LL1462: return Cyc_PP_text( _temp1487); _LL1464:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp1494=( char*)"null";
struct _tagged_string _temp1495; _temp1495.curr= _temp1494; _temp1495.base=
_temp1494; _temp1495.last_plus_one= _temp1494 + 5; _temp1495;})); _LL1466:
return Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1496=(
char*)"\""; struct _tagged_string _temp1497; _temp1497.curr= _temp1496;
_temp1497.base= _temp1496; _temp1497.last_plus_one= _temp1496 + 2; _temp1497;})),
Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1489)), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1498=( char*)"\""; struct _tagged_string _temp1499;
_temp1499.curr= _temp1498; _temp1499.base= _temp1498; _temp1499.last_plus_one=
_temp1498 + 2; _temp1499;}))); _LL1450:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:({ struct Cyc_List_List*
_temp1500= es; if( _temp1500 == 0){ _throw( Null_Exception);} _temp1500->tl;})
!= 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct*
_temp1501=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1501[ 0]=({ struct Cyc_Core_Failure_struct _temp1502; _temp1502.tag= Cyc_Core_Failure_tag;
_temp1502.f1=({ struct _tagged_string _temp1503= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1503.last_plus_one
- _temp1503.curr, _temp1503.curr);}); _temp1502;}); _temp1501;}));}{ struct Cyc_PP_Doc*
d= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1506= es; if( _temp1506 == 0){ _throw( Null_Exception);} _temp1506->hd;}));
return Cyc_Absynpp_cat2( d, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1504=( char*)".size"; struct _tagged_string _temp1505; _temp1505.curr=
_temp1504; _temp1505.base= _temp1504; _temp1505.last_plus_one= _temp1504 + 6;
_temp1505;})));}} else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct Cyc_List_List*
ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct*
_temp1507=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1507[ 0]=({ struct Cyc_Core_Failure_struct _temp1508; _temp1508.tag= Cyc_Core_Failure_tag;
_temp1508.f1=({ struct _tagged_string _temp1509= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with no args", _temp1509.last_plus_one -
_temp1509.curr, _temp1509.curr);}); _temp1508;}); _temp1507;}));} else{ if(({
struct Cyc_List_List* _temp1510= ds; if( _temp1510 == 0){ _throw( Null_Exception);}
_temp1510->tl;}) == 0){ return Cyc_Absynpp_cat3( ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1511=( char*)" "; struct _tagged_string _temp1512;
_temp1512.curr= _temp1511; _temp1512.base= _temp1511; _temp1512.last_plus_one=
_temp1511 + 2; _temp1512;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1513= ds; if( _temp1513 == 0){ _throw( Null_Exception);} _temp1513->hd;}));}
else{ if(({ struct Cyc_List_List* _temp1515=({ struct Cyc_List_List* _temp1514=
ds; if( _temp1514 == 0){ _throw( Null_Exception);} _temp1514->tl;}); if(
_temp1515 == 0){ _throw( Null_Exception);} _temp1515->tl;}) != 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp1516=( struct
Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1516[ 0]=({ struct Cyc_Core_Failure_struct _temp1517; _temp1517.tag= Cyc_Core_Failure_tag;
_temp1517.f1=({ struct _tagged_string _temp1518= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with more than 2 args", _temp1518.last_plus_one
- _temp1518.curr, _temp1518.curr);}); _temp1517;}); _temp1516;}));} else{ return
Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp1519= ds;
if( _temp1519 == 0){ _throw( Null_Exception);} _temp1519->hd;}), Cyc_PP_text((
struct _tagged_string)({ char* _temp1520=( char*)" "; struct _tagged_string
_temp1521; _temp1521.curr= _temp1520; _temp1521.base= _temp1520; _temp1521.last_plus_one=
_temp1520 + 2; _temp1521;})), ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1522=( char*)" "; struct _tagged_string _temp1523; _temp1523.curr=
_temp1522; _temp1523.base= _temp1522; _temp1523.last_plus_one= _temp1522 + 2;
_temp1523;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp1525=({ struct
Cyc_List_List* _temp1524= ds; if( _temp1524 == 0){ _throw( Null_Exception);}
_temp1524->tl;}); if( _temp1525 == 0){ _throw( Null_Exception);} _temp1525->hd;}));}}}}
else{ return Cyc_Absynpp_cat4( ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1526=( char*)"("; struct _tagged_string _temp1527; _temp1527.curr=
_temp1526; _temp1527.base= _temp1526; _temp1527.last_plus_one= _temp1526 + 2;
_temp1527;})), Cyc_Absynpp_exps2doc_prec( 20, es), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1528=( char*)")"; struct _tagged_string _temp1529;
_temp1529.curr= _temp1528; _temp1529.base= _temp1528; _temp1529.last_plus_one=
_temp1528 + 2; _temp1529;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1530= p; _LL1532: if(( int) _temp1530 == Cyc_Absyn_Plus){
goto _LL1533;} else{ goto _LL1534;} _LL1534: if(( int) _temp1530 == Cyc_Absyn_Times){
goto _LL1535;} else{ goto _LL1536;} _LL1536: if(( int) _temp1530 == Cyc_Absyn_Minus){
goto _LL1537;} else{ goto _LL1538;} _LL1538: if(( int) _temp1530 == Cyc_Absyn_Div){
goto _LL1539;} else{ goto _LL1540;} _LL1540: if(( int) _temp1530 == Cyc_Absyn_Mod){
goto _LL1541;} else{ goto _LL1542;} _LL1542: if(( int) _temp1530 == Cyc_Absyn_Eq){
goto _LL1543;} else{ goto _LL1544;} _LL1544: if(( int) _temp1530 == Cyc_Absyn_Neq){
goto _LL1545;} else{ goto _LL1546;} _LL1546: if(( int) _temp1530 == Cyc_Absyn_Gt){
goto _LL1547;} else{ goto _LL1548;} _LL1548: if(( int) _temp1530 == Cyc_Absyn_Lt){
goto _LL1549;} else{ goto _LL1550;} _LL1550: if(( int) _temp1530 == Cyc_Absyn_Gte){
goto _LL1551;} else{ goto _LL1552;} _LL1552: if(( int) _temp1530 == Cyc_Absyn_Lte){
goto _LL1553;} else{ goto _LL1554;} _LL1554: if(( int) _temp1530 == Cyc_Absyn_Not){
goto _LL1555;} else{ goto _LL1556;} _LL1556: if(( int) _temp1530 == Cyc_Absyn_Bitnot){
goto _LL1557;} else{ goto _LL1558;} _LL1558: if(( int) _temp1530 == Cyc_Absyn_Bitand){
goto _LL1559;} else{ goto _LL1560;} _LL1560: if(( int) _temp1530 == Cyc_Absyn_Bitor){
goto _LL1561;} else{ goto _LL1562;} _LL1562: if(( int) _temp1530 == Cyc_Absyn_Bitxor){
goto _LL1563;} else{ goto _LL1564;} _LL1564: if(( int) _temp1530 == Cyc_Absyn_Bitlshift){
goto _LL1565;} else{ goto _LL1566;} _LL1566: if(( int) _temp1530 == Cyc_Absyn_Bitlrshift){
goto _LL1567;} else{ goto _LL1568;} _LL1568: if(( int) _temp1530 == Cyc_Absyn_Bitarshift){
goto _LL1569;} else{ goto _LL1570;} _LL1570: if(( int) _temp1530 == Cyc_Absyn_Size){
goto _LL1571;} else{ goto _LL1572;} _LL1572: if(( int) _temp1530 == Cyc_Absyn_Printf){
goto _LL1573;} else{ goto _LL1574;} _LL1574: if(( int) _temp1530 == Cyc_Absyn_Fprintf){
goto _LL1575;} else{ goto _LL1576;} _LL1576: if(( int) _temp1530 == Cyc_Absyn_Xprintf){
goto _LL1577;} else{ goto _LL1578;} _LL1578: if(( int) _temp1530 == Cyc_Absyn_Scanf){
goto _LL1579;} else{ goto _LL1580;} _LL1580: if(( int) _temp1530 == Cyc_Absyn_Fscanf){
goto _LL1581;} else{ goto _LL1582;} _LL1582: if(( int) _temp1530 == Cyc_Absyn_Sscanf){
goto _LL1583;} else{ goto _LL1531;} _LL1533: return( struct _tagged_string)({
char* _temp1584=( char*)"+"; struct _tagged_string _temp1585; _temp1585.curr=
_temp1584; _temp1585.base= _temp1584; _temp1585.last_plus_one= _temp1584 + 2;
_temp1585;}); _LL1535: return( struct _tagged_string)({ char* _temp1586=( char*)"*";
struct _tagged_string _temp1587; _temp1587.curr= _temp1586; _temp1587.base=
_temp1586; _temp1587.last_plus_one= _temp1586 + 2; _temp1587;}); _LL1537: return(
struct _tagged_string)({ char* _temp1588=( char*)"-"; struct _tagged_string
_temp1589; _temp1589.curr= _temp1588; _temp1589.base= _temp1588; _temp1589.last_plus_one=
_temp1588 + 2; _temp1589;}); _LL1539: return( struct _tagged_string)({ char*
_temp1590=( char*)"/"; struct _tagged_string _temp1591; _temp1591.curr=
_temp1590; _temp1591.base= _temp1590; _temp1591.last_plus_one= _temp1590 + 2;
_temp1591;}); _LL1541: return( struct _tagged_string)({ char* _temp1592=( char*)"%";
struct _tagged_string _temp1593; _temp1593.curr= _temp1592; _temp1593.base=
_temp1592; _temp1593.last_plus_one= _temp1592 + 2; _temp1593;}); _LL1543: return(
struct _tagged_string)({ char* _temp1594=( char*)"=="; struct _tagged_string
_temp1595; _temp1595.curr= _temp1594; _temp1595.base= _temp1594; _temp1595.last_plus_one=
_temp1594 + 3; _temp1595;}); _LL1545: return( struct _tagged_string)({ char*
_temp1596=( char*)"!="; struct _tagged_string _temp1597; _temp1597.curr=
_temp1596; _temp1597.base= _temp1596; _temp1597.last_plus_one= _temp1596 + 3;
_temp1597;}); _LL1547: return( struct _tagged_string)({ char* _temp1598=( char*)">";
struct _tagged_string _temp1599; _temp1599.curr= _temp1598; _temp1599.base=
_temp1598; _temp1599.last_plus_one= _temp1598 + 2; _temp1599;}); _LL1549: return(
struct _tagged_string)({ char* _temp1600=( char*)"<"; struct _tagged_string
_temp1601; _temp1601.curr= _temp1600; _temp1601.base= _temp1600; _temp1601.last_plus_one=
_temp1600 + 2; _temp1601;}); _LL1551: return( struct _tagged_string)({ char*
_temp1602=( char*)">="; struct _tagged_string _temp1603; _temp1603.curr=
_temp1602; _temp1603.base= _temp1602; _temp1603.last_plus_one= _temp1602 + 3;
_temp1603;}); _LL1553: return( struct _tagged_string)({ char* _temp1604=( char*)"<=";
struct _tagged_string _temp1605; _temp1605.curr= _temp1604; _temp1605.base=
_temp1604; _temp1605.last_plus_one= _temp1604 + 3; _temp1605;}); _LL1555: return(
struct _tagged_string)({ char* _temp1606=( char*)"!"; struct _tagged_string
_temp1607; _temp1607.curr= _temp1606; _temp1607.base= _temp1606; _temp1607.last_plus_one=
_temp1606 + 2; _temp1607;}); _LL1557: return( struct _tagged_string)({ char*
_temp1608=( char*)"~"; struct _tagged_string _temp1609; _temp1609.curr=
_temp1608; _temp1609.base= _temp1608; _temp1609.last_plus_one= _temp1608 + 2;
_temp1609;}); _LL1559: return( struct _tagged_string)({ char* _temp1610=( char*)"&";
struct _tagged_string _temp1611; _temp1611.curr= _temp1610; _temp1611.base=
_temp1610; _temp1611.last_plus_one= _temp1610 + 2; _temp1611;}); _LL1561: return(
struct _tagged_string)({ char* _temp1612=( char*)"|"; struct _tagged_string
_temp1613; _temp1613.curr= _temp1612; _temp1613.base= _temp1612; _temp1613.last_plus_one=
_temp1612 + 2; _temp1613;}); _LL1563: return( struct _tagged_string)({ char*
_temp1614=( char*)"^"; struct _tagged_string _temp1615; _temp1615.curr=
_temp1614; _temp1615.base= _temp1614; _temp1615.last_plus_one= _temp1614 + 2;
_temp1615;}); _LL1565: return( struct _tagged_string)({ char* _temp1616=( char*)"<<";
struct _tagged_string _temp1617; _temp1617.curr= _temp1616; _temp1617.base=
_temp1616; _temp1617.last_plus_one= _temp1616 + 3; _temp1617;}); _LL1567: return(
struct _tagged_string)({ char* _temp1618=( char*)">>"; struct _tagged_string
_temp1619; _temp1619.curr= _temp1618; _temp1619.base= _temp1618; _temp1619.last_plus_one=
_temp1618 + 3; _temp1619;}); _LL1569: return( struct _tagged_string)({ char*
_temp1620=( char*)">>>"; struct _tagged_string _temp1621; _temp1621.curr=
_temp1620; _temp1621.base= _temp1620; _temp1621.last_plus_one= _temp1620 + 4;
_temp1621;}); _LL1571: return( struct _tagged_string)({ char* _temp1622=( char*)"size";
struct _tagged_string _temp1623; _temp1623.curr= _temp1622; _temp1623.base=
_temp1622; _temp1623.last_plus_one= _temp1622 + 5; _temp1623;}); _LL1573: return(
struct _tagged_string)({ char* _temp1624=( char*)"printf"; struct _tagged_string
_temp1625; _temp1625.curr= _temp1624; _temp1625.base= _temp1624; _temp1625.last_plus_one=
_temp1624 + 7; _temp1625;}); _LL1575: return( struct _tagged_string)({ char*
_temp1626=( char*)"fprintf"; struct _tagged_string _temp1627; _temp1627.curr=
_temp1626; _temp1627.base= _temp1626; _temp1627.last_plus_one= _temp1626 + 8;
_temp1627;}); _LL1577: return( struct _tagged_string)({ char* _temp1628=( char*)"xprintf";
struct _tagged_string _temp1629; _temp1629.curr= _temp1628; _temp1629.base=
_temp1628; _temp1629.last_plus_one= _temp1628 + 8; _temp1629;}); _LL1579: return(
struct _tagged_string)({ char* _temp1630=( char*)"scanf"; struct _tagged_string
_temp1631; _temp1631.curr= _temp1630; _temp1631.base= _temp1630; _temp1631.last_plus_one=
_temp1630 + 6; _temp1631;}); _LL1581: return( struct _tagged_string)({ char*
_temp1632=( char*)"fscanf"; struct _tagged_string _temp1633; _temp1633.curr=
_temp1632; _temp1633.base= _temp1632; _temp1633.last_plus_one= _temp1632 + 7;
_temp1633;}); _LL1583: return( struct _tagged_string)({ char* _temp1634=( char*)"sscanf";
struct _tagged_string _temp1635; _temp1635.curr= _temp1634; _temp1635.base=
_temp1634; _temp1635.last_plus_one= _temp1634 + 7; _temp1635;}); _LL1531:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1
== 0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp1636=(* t).f1; if( _temp1636
== 0){ _throw( Null_Exception);} _temp1636->v;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1637=( char*)" = "; struct _tagged_string _temp1638;
_temp1638.curr= _temp1637; _temp1638.base= _temp1637; _temp1638.last_plus_one=
_temp1637 + 4; _temp1638;})), Cyc_Absynpp_exp2doc((* t).f2));}} struct Cyc_PP_Doc*
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
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1639=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1645; struct Cyc_Absyn_Decl* _temp1647;
_LL1641: if(( unsigned int) _temp1639 > 1u?(( struct _tunion_struct*) _temp1639)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL1648: _temp1647=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1639)->f1; goto _LL1646; _LL1646:
_temp1645=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1639)->f2;
goto _LL1642;} else{ goto _LL1643;} _LL1643: goto _LL1644; _LL1642: return 1;
_LL1644: return 0; _LL1640:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1649=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1691; struct Cyc_Absyn_Stmt* _temp1693; struct Cyc_Absyn_Stmt*
_temp1695; struct Cyc_Absyn_Exp* _temp1697; struct Cyc_Absyn_Stmt* _temp1699;
struct Cyc_Absyn_Stmt* _temp1701; struct Cyc_Absyn_Exp* _temp1703; struct Cyc_Absyn_Stmt*
_temp1705; struct _tuple2 _temp1707; struct Cyc_Absyn_Stmt* _temp1709; struct
Cyc_Absyn_Exp* _temp1711; struct Cyc_Absyn_Stmt* _temp1713; struct Cyc_Absyn_Stmt*
_temp1715; struct Cyc_Absyn_Stmt* _temp1717; struct _tagged_string* _temp1719;
struct Cyc_Absyn_Stmt* _temp1721; struct _tuple2 _temp1723; struct Cyc_Absyn_Stmt*
_temp1725; struct Cyc_Absyn_Exp* _temp1727; struct _tuple2 _temp1729; struct Cyc_Absyn_Stmt*
_temp1731; struct Cyc_Absyn_Exp* _temp1733; struct Cyc_Absyn_Exp* _temp1735;
struct Cyc_List_List* _temp1737; struct Cyc_Absyn_Exp* _temp1739; struct Cyc_Absyn_Switch_clause**
_temp1741; struct Cyc_List_List* _temp1743; struct Cyc_Absyn_Switch_clause**
_temp1745; struct Cyc_List_List* _temp1747; struct Cyc_Absyn_Stmt* _temp1749;
struct Cyc_Absyn_Decl* _temp1751; struct Cyc_Absyn_Stmt* _temp1753; struct Cyc_Absyn_Stmt*
_temp1755; struct Cyc_Absyn_Stmt* _temp1757; struct _tagged_string* _temp1759;
struct _tuple2 _temp1761; struct Cyc_Absyn_Stmt* _temp1763; struct Cyc_Absyn_Exp*
_temp1765; struct Cyc_Absyn_Stmt* _temp1767; struct Cyc_List_List* _temp1769;
struct Cyc_Absyn_Stmt* _temp1771; struct Cyc_Absyn_Stmt* _temp1773; struct Cyc_Absyn_Vardecl*
_temp1775; struct Cyc_Absyn_Tvar* _temp1777; _LL1651: if(( int) _temp1649 == Cyc_Absyn_Skip_s){
goto _LL1652;} else{ goto _LL1653;} _LL1653: if(( unsigned int) _temp1649 > 1u?((
struct _tunion_struct*) _temp1649)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1692:
_temp1691=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1649)->f1;
goto _LL1654;} else{ goto _LL1655;} _LL1655: if(( unsigned int) _temp1649 > 1u?((
struct _tunion_struct*) _temp1649)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1696:
_temp1695=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1649)->f1;
goto _LL1694; _LL1694: _temp1693=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1649)->f2; goto _LL1656;} else{ goto _LL1657;} _LL1657: if(( unsigned int)
_temp1649 > 1u?(( struct _tunion_struct*) _temp1649)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1698: _temp1697=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1649)->f1; goto _LL1658;} else{ goto _LL1659;} _LL1659: if(( unsigned int)
_temp1649 > 1u?(( struct _tunion_struct*) _temp1649)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1704: _temp1703=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1649)->f1; goto _LL1702; _LL1702: _temp1701=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1649)->f2; goto _LL1700; _LL1700:
_temp1699=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1649)->f3; goto _LL1660;} else{ goto _LL1661;} _LL1661: if(( unsigned int)
_temp1649 > 1u?(( struct _tunion_struct*) _temp1649)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1708: _temp1707=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*)
_temp1649)->f1; _LL1712: _temp1711= _temp1707.f1; goto _LL1710; _LL1710:
_temp1709= _temp1707.f2; goto _LL1706; _LL1706: _temp1705=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1649)->f2; goto _LL1662;} else{ goto
_LL1663;} _LL1663: if(( unsigned int) _temp1649 > 1u?(( struct _tunion_struct*)
_temp1649)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1714: _temp1713=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1649)->f1; goto _LL1664;} else{ goto
_LL1665;} _LL1665: if(( unsigned int) _temp1649 > 1u?(( struct _tunion_struct*)
_temp1649)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1716: _temp1715=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1649)->f1; goto _LL1666;} else{ goto
_LL1667;} _LL1667: if(( unsigned int) _temp1649 > 1u?(( struct _tunion_struct*)
_temp1649)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1720: _temp1719=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1649)->f1; goto _LL1718;
_LL1718: _temp1717=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1649)->f2; goto _LL1668;} else{ goto _LL1669;} _LL1669: if(( unsigned int)
_temp1649 > 1u?(( struct _tunion_struct*) _temp1649)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1736: _temp1735=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1649)->f1; goto _LL1730; _LL1730: _temp1729=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp1649)->f2; _LL1734: _temp1733= _temp1729.f1; goto _LL1732; _LL1732:
_temp1731= _temp1729.f2; goto _LL1724; _LL1724: _temp1723=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp1649)->f3; _LL1728: _temp1727= _temp1723.f1;
goto _LL1726; _LL1726: _temp1725= _temp1723.f2; goto _LL1722; _LL1722: _temp1721=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1649)->f4; goto
_LL1670;} else{ goto _LL1671;} _LL1671: if(( unsigned int) _temp1649 > 1u?((
struct _tunion_struct*) _temp1649)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1740:
_temp1739=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1649)->f1; goto _LL1738; _LL1738: _temp1737=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1649)->f2; goto _LL1672;} else{ goto
_LL1673;} _LL1673: if(( unsigned int) _temp1649 > 1u?(( struct _tunion_struct*)
_temp1649)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1744: _temp1743=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1649)->f1; if( _temp1743 == 0){ goto
_LL1742;} else{ goto _LL1675;} _LL1742: _temp1741=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1649)->f2; goto _LL1674;} else{ goto
_LL1675;} _LL1675: if(( unsigned int) _temp1649 > 1u?(( struct _tunion_struct*)
_temp1649)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1748: _temp1747=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1649)->f1; goto _LL1746; _LL1746:
_temp1745=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1649)->f2; goto _LL1676;} else{ goto _LL1677;} _LL1677: if(( unsigned int)
_temp1649 > 1u?(( struct _tunion_struct*) _temp1649)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1752: _temp1751=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1649)->f1; goto _LL1750; _LL1750: _temp1749=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1649)->f2; goto _LL1678;} else{ goto
_LL1679;} _LL1679: if(( unsigned int) _temp1649 > 1u?(( struct _tunion_struct*)
_temp1649)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1754: _temp1753=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1649)->f1; goto _LL1680;} else{ goto
_LL1681;} _LL1681: if(( unsigned int) _temp1649 > 1u?(( struct _tunion_struct*)
_temp1649)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1756: _temp1755=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1649)->f1; goto _LL1682;} else{ goto
_LL1683;} _LL1683: if(( unsigned int) _temp1649 > 1u?(( struct _tunion_struct*)
_temp1649)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1760: _temp1759=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1649)->f1; goto
_LL1758; _LL1758: _temp1757=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1649)->f2; goto _LL1684;} else{ goto _LL1685;} _LL1685: if(( unsigned int)
_temp1649 > 1u?(( struct _tunion_struct*) _temp1649)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1768: _temp1767=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1649)->f1; goto _LL1762; _LL1762: _temp1761=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp1649)->f2; _LL1766: _temp1765= _temp1761.f1; goto _LL1764; _LL1764:
_temp1763= _temp1761.f2; goto _LL1686;} else{ goto _LL1687;} _LL1687: if((
unsigned int) _temp1649 > 1u?(( struct _tunion_struct*) _temp1649)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1772: _temp1771=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1649)->f1; goto _LL1770; _LL1770: _temp1769=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1649)->f2; goto _LL1688;} else{ goto
_LL1689;} _LL1689: if(( unsigned int) _temp1649 > 1u?(( struct _tunion_struct*)
_temp1649)->tag == Cyc_Absyn_Region_s_tag: 0){ _LL1778: _temp1777=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_Region_s_struct*) _temp1649)->f1; goto _LL1776; _LL1776:
_temp1775=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1649)->f2; goto _LL1774; _LL1774: _temp1773=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Region_s_struct*) _temp1649)->f3; goto _LL1690;} else{ goto
_LL1650;} _LL1652: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1779=(
char*)";"; struct _tagged_string _temp1780; _temp1780.curr= _temp1779; _temp1780.base=
_temp1779; _temp1780.last_plus_one= _temp1779 + 2; _temp1780;})); goto _LL1650;
_LL1654: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc( _temp1691), Cyc_PP_text((
struct _tagged_string)({ char* _temp1781=( char*)";"; struct _tagged_string
_temp1782; _temp1782.curr= _temp1781; _temp1782.base= _temp1781; _temp1782.last_plus_one=
_temp1781 + 2; _temp1782;}))); goto _LL1650; _LL1656: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1695)){ s= Cyc_Absynpp_cat6( Cyc_PP_text((
struct _tagged_string)({ char* _temp1783=( char*)"{ "; struct _tagged_string
_temp1784; _temp1784.curr= _temp1783; _temp1784.base= _temp1783; _temp1784.last_plus_one=
_temp1783 + 3; _temp1784;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1695)),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1785=( char*)"}";
struct _tagged_string _temp1786; _temp1786.curr= _temp1785; _temp1786.base=
_temp1785; _temp1786.last_plus_one= _temp1785 + 2; _temp1786;})), Cyc_PP_line_doc(),
Cyc_Absynpp_is_declaration( _temp1693)? Cyc_Absynpp_cat4( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1787=( char*)"{ "; struct _tagged_string _temp1788;
_temp1788.curr= _temp1787; _temp1788.base= _temp1787; _temp1788.last_plus_one=
_temp1787 + 3; _temp1788;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1693)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1789=( char*)"}"; struct
_tagged_string _temp1790; _temp1790.curr= _temp1789; _temp1790.base= _temp1789;
_temp1790.last_plus_one= _temp1789 + 2; _temp1790;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1693));} else{ if( Cyc_Absynpp_is_declaration( _temp1693)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1695), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1791=( char*)"{ "; struct _tagged_string _temp1792;
_temp1792.curr= _temp1791; _temp1792.base= _temp1791; _temp1792.last_plus_one=
_temp1791 + 3; _temp1792;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1693)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1793=( char*)"}"; struct
_tagged_string _temp1794; _temp1794.curr= _temp1793; _temp1794.base= _temp1793;
_temp1794.last_plus_one= _temp1793 + 2; _temp1794;})), Cyc_PP_line_doc());}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,(
struct _tagged_string)({ char* _temp1795=( char*)""; struct _tagged_string
_temp1796; _temp1796.curr= _temp1795; _temp1796.base= _temp1795; _temp1796.last_plus_one=
_temp1795 + 1; _temp1796;}),({ struct Cyc_List_List* _temp1797=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1797->hd=( void*) _temp1695;
_temp1797->tl=({ struct Cyc_List_List* _temp1798=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1798->hd=( void*) _temp1693;
_temp1798->tl= 0; _temp1798;}); _temp1797;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,( struct
_tagged_string)({ char* _temp1799=( char*)""; struct _tagged_string _temp1800;
_temp1800.curr= _temp1799; _temp1800.base= _temp1799; _temp1800.last_plus_one=
_temp1799 + 1; _temp1800;}),({ struct Cyc_List_List* _temp1801=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1801->hd=( void*) _temp1695;
_temp1801->tl=({ struct Cyc_List_List* _temp1802=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1802->hd=( void*) _temp1693;
_temp1802->tl= 0; _temp1802;}); _temp1801;}));} goto _LL1650; _LL1658: if(
_temp1697 == 0){ s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1803=(
char*)"return;"; struct _tagged_string _temp1804; _temp1804.curr= _temp1803;
_temp1804.base= _temp1803; _temp1804.last_plus_one= _temp1803 + 8; _temp1804;}));}
else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1805=( char*)"return "; struct _tagged_string _temp1806; _temp1806.curr=
_temp1805; _temp1806.base= _temp1805; _temp1806.last_plus_one= _temp1805 + 8;
_temp1806;})), _temp1697 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1807= _temp1697; if( _temp1807 == 0){
_throw( Null_Exception);} _temp1807;})), Cyc_PP_text(( struct _tagged_string)({
char* _temp1808=( char*)";"; struct _tagged_string _temp1809; _temp1809.curr=
_temp1808; _temp1809.base= _temp1808; _temp1809.last_plus_one= _temp1808 + 2;
_temp1809;})));} goto _LL1650; _LL1660: { int print_else;{ void* _temp1810=(
void*) _temp1699->r; _LL1812: if(( int) _temp1810 == Cyc_Absyn_Skip_s){ goto
_LL1813;} else{ goto _LL1814;} _LL1814: goto _LL1815; _LL1813: print_else= 0;
goto _LL1811; _LL1815: print_else= 1; goto _LL1811; _LL1811:;} s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1816=( char*)"if ("; struct
_tagged_string _temp1817; _temp1817.curr= _temp1816; _temp1817.base= _temp1816;
_temp1817.last_plus_one= _temp1816 + 5; _temp1817;})), Cyc_Absynpp_exp2doc(
_temp1703), Cyc_PP_text(( struct _tagged_string)({ char* _temp1818=( char*)") {";
struct _tagged_string _temp1819; _temp1819.curr= _temp1818; _temp1819.base=
_temp1818; _temp1819.last_plus_one= _temp1818 + 4; _temp1819;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1701))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1820=( char*)"}"; struct
_tagged_string _temp1821; _temp1821.curr= _temp1820; _temp1821.base= _temp1820;
_temp1821.last_plus_one= _temp1820 + 2; _temp1821;})), print_else? Cyc_Absynpp_cat5(
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1822=( char*)"else {";
struct _tagged_string _temp1823; _temp1823.curr= _temp1822; _temp1823.base=
_temp1822; _temp1823.last_plus_one= _temp1822 + 7; _temp1823;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1699))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1824=( char*)"}"; struct
_tagged_string _temp1825; _temp1825.curr= _temp1824; _temp1825.base= _temp1824;
_temp1825.last_plus_one= _temp1824 + 2; _temp1825;}))): Cyc_PP_nil_doc()); goto
_LL1650;} _LL1662: s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({
char* _temp1826=( char*)"while ("; struct _tagged_string _temp1827; _temp1827.curr=
_temp1826; _temp1827.base= _temp1826; _temp1827.last_plus_one= _temp1826 + 8;
_temp1827;})), Cyc_Absynpp_exp2doc( _temp1711), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1828=( char*)") {"; struct _tagged_string _temp1829;
_temp1829.curr= _temp1828; _temp1829.base= _temp1828; _temp1829.last_plus_one=
_temp1828 + 4; _temp1829;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1705))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1830=( char*)"}"; struct _tagged_string _temp1831;
_temp1831.curr= _temp1830; _temp1831.base= _temp1830; _temp1831.last_plus_one=
_temp1830 + 2; _temp1831;}))); goto _LL1650; _LL1664: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1832=( char*)"break;"; struct _tagged_string
_temp1833; _temp1833.curr= _temp1832; _temp1833.base= _temp1832; _temp1833.last_plus_one=
_temp1832 + 7; _temp1833;})); goto _LL1650; _LL1666: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1834=( char*)"continue;"; struct _tagged_string
_temp1835; _temp1835.curr= _temp1834; _temp1835.base= _temp1834; _temp1835.last_plus_one=
_temp1834 + 10; _temp1835;})); goto _LL1650; _LL1668: s= Cyc_PP_text(({ struct
_tagged_string _temp1836=* _temp1719; xprintf("goto %.*s;", _temp1836.last_plus_one
- _temp1836.curr, _temp1836.curr);})); goto _LL1650; _LL1670: s= Cyc_Absynpp_cat10(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1837=( char*)"for("; struct
_tagged_string _temp1838; _temp1838.curr= _temp1837; _temp1838.base= _temp1837;
_temp1838.last_plus_one= _temp1837 + 5; _temp1838;})), Cyc_Absynpp_exp2doc(
_temp1735), Cyc_PP_text(( struct _tagged_string)({ char* _temp1839=( char*)"; ";
struct _tagged_string _temp1840; _temp1840.curr= _temp1839; _temp1840.base=
_temp1839; _temp1840.last_plus_one= _temp1839 + 3; _temp1840;})), Cyc_Absynpp_exp2doc(
_temp1733), Cyc_PP_text(( struct _tagged_string)({ char* _temp1841=( char*)"; ";
struct _tagged_string _temp1842; _temp1842.curr= _temp1841; _temp1842.base=
_temp1841; _temp1842.last_plus_one= _temp1841 + 3; _temp1842;})), Cyc_Absynpp_exp2doc(
_temp1727), Cyc_PP_text(( struct _tagged_string)({ char* _temp1843=( char*)") {";
struct _tagged_string _temp1844; _temp1844.curr= _temp1843; _temp1844.base=
_temp1843; _temp1844.last_plus_one= _temp1843 + 4; _temp1844;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1721))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1845=( char*)"}"; struct
_tagged_string _temp1846; _temp1846.curr= _temp1845; _temp1846.base= _temp1845;
_temp1846.last_plus_one= _temp1845 + 2; _temp1846;}))); goto _LL1650; _LL1672: s=
Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char* _temp1847=( char*)"switch (";
struct _tagged_string _temp1848; _temp1848.curr= _temp1847; _temp1848.base=
_temp1847; _temp1848.last_plus_one= _temp1847 + 9; _temp1848;})), Cyc_Absynpp_exp2doc(
_temp1739), Cyc_PP_text(( struct _tagged_string)({ char* _temp1849=( char*)") {";
struct _tagged_string _temp1850; _temp1850.curr= _temp1849; _temp1850.base=
_temp1849; _temp1850.last_plus_one= _temp1849 + 4; _temp1850;})), Cyc_PP_line_doc(),
Cyc_Absynpp_switchclauses2doc( _temp1737), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1851=( char*)"}"; struct _tagged_string
_temp1852; _temp1852.curr= _temp1851; _temp1852.base= _temp1851; _temp1852.last_plus_one=
_temp1851 + 2; _temp1852;}))); goto _LL1650; _LL1674: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1853=( char*)"fallthru;"; struct _tagged_string
_temp1854; _temp1854.curr= _temp1853; _temp1854.base= _temp1853; _temp1854.last_plus_one=
_temp1853 + 10; _temp1854;})); goto _LL1650; _LL1676: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1855=( char*)"fallthru("; struct
_tagged_string _temp1856; _temp1856.curr= _temp1855; _temp1856.base= _temp1855;
_temp1856.last_plus_one= _temp1855 + 10; _temp1856;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1747), Cyc_PP_text(( struct _tagged_string)({ char* _temp1857=( char*)");";
struct _tagged_string _temp1858; _temp1858.curr= _temp1857; _temp1858.base=
_temp1857; _temp1858.last_plus_one= _temp1857 + 3; _temp1858;}))); goto _LL1650;
_LL1678: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1751), Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1749)); goto _LL1650; _LL1680: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1859=( char*)"cut "; struct
_tagged_string _temp1860; _temp1860.curr= _temp1859; _temp1860.base= _temp1859;
_temp1860.last_plus_one= _temp1859 + 5; _temp1860;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1753))); goto _LL1650; _LL1682: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1861=( char*)"splice "; struct _tagged_string
_temp1862; _temp1862.curr= _temp1861; _temp1862.base= _temp1861; _temp1862.last_plus_one=
_temp1861 + 8; _temp1862;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1755)));
goto _LL1650; _LL1684: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1757): 0){ s= Cyc_Absynpp_cat6( Cyc_Absynpp_textptr( _temp1759), Cyc_PP_text((
struct _tagged_string)({ char* _temp1863=( char*)": {"; struct _tagged_string
_temp1864; _temp1864.curr= _temp1863; _temp1864.base= _temp1863; _temp1864.last_plus_one=
_temp1863 + 4; _temp1864;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1757)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1865=( char*)"}"; struct _tagged_string _temp1866; _temp1866.curr=
_temp1865; _temp1866.base= _temp1865; _temp1866.last_plus_one= _temp1865 + 2;
_temp1866;})));} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( _temp1759), Cyc_PP_text((
struct _tagged_string)({ char* _temp1867=( char*)": "; struct _tagged_string
_temp1868; _temp1868.curr= _temp1867; _temp1868.base= _temp1867; _temp1868.last_plus_one=
_temp1867 + 3; _temp1868;})), Cyc_Absynpp_stmt2doc( _temp1757));} goto _LL1650;
_LL1686: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1869=( char*)"do {"; struct _tagged_string _temp1870; _temp1870.curr=
_temp1869; _temp1870.base= _temp1869; _temp1870.last_plus_one= _temp1869 + 5;
_temp1870;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1767)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1871=( char*)"} while ("; struct _tagged_string _temp1872; _temp1872.curr=
_temp1871; _temp1872.base= _temp1871; _temp1872.last_plus_one= _temp1871 + 10;
_temp1872;})), Cyc_Absynpp_exp2doc( _temp1765), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1873=( char*)");"; struct _tagged_string _temp1874;
_temp1874.curr= _temp1873; _temp1874.base= _temp1873; _temp1874.last_plus_one=
_temp1873 + 3; _temp1874;}))); goto _LL1650; _LL1688: s= Cyc_Absynpp_cat9( Cyc_PP_text((
struct _tagged_string)({ char* _temp1875=( char*)"try {"; struct _tagged_string
_temp1876; _temp1876.curr= _temp1875; _temp1876.base= _temp1875; _temp1876.last_plus_one=
_temp1875 + 6; _temp1876;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1771)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1877=( char*)"} catch {"; struct _tagged_string _temp1878; _temp1878.curr=
_temp1877; _temp1878.base= _temp1877; _temp1878.last_plus_one= _temp1877 + 10;
_temp1878;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1769)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1879=( char*)"}"; struct _tagged_string _temp1880; _temp1880.curr=
_temp1879; _temp1880.base= _temp1879; _temp1880.last_plus_one= _temp1879 + 2;
_temp1880;}))); goto _LL1650; _LL1690: s= Cyc_Absynpp_cat9( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1881=( char*)"region<"; struct _tagged_string
_temp1882; _temp1882.curr= _temp1881; _temp1882.base= _temp1881; _temp1882.last_plus_one=
_temp1881 + 8; _temp1882;})), Cyc_Absynpp_textptr( Cyc_Absynpp_get_name(
_temp1777)), Cyc_PP_text(( struct _tagged_string)({ char* _temp1883=( char*)">";
struct _tagged_string _temp1884; _temp1884.curr= _temp1883; _temp1884.base=
_temp1883; _temp1884.last_plus_one= _temp1883 + 2; _temp1884;})), Cyc_Absynpp_qvar2doc(
_temp1775->name), Cyc_PP_text(( struct _tagged_string)({ char* _temp1885=( char*)"{";
struct _tagged_string _temp1886; _temp1886.curr= _temp1885; _temp1886.base=
_temp1885; _temp1886.last_plus_one= _temp1885 + 2; _temp1886;})), Cyc_PP_line_doc(),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1773)), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1887=( char*)"}"; struct _tagged_string
_temp1888; _temp1888.curr= _temp1887; _temp1888.base= _temp1887; _temp1888.last_plus_one=
_temp1887 + 2; _temp1888;}))); goto _LL1650; _LL1650:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_pat2doc( struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void*
_temp1889=( void*) p->r; int _temp1923; void* _temp1925; char _temp1927; struct
_tagged_string _temp1929; struct Cyc_Absyn_Vardecl* _temp1931; struct Cyc_List_List*
_temp1933; struct Cyc_Absyn_Pat* _temp1935; struct Cyc_Absyn_Vardecl* _temp1937;
struct _tuple0* _temp1939; struct Cyc_List_List* _temp1941; struct Cyc_List_List*
_temp1943; struct _tuple0* _temp1945; struct Cyc_List_List* _temp1947; struct
Cyc_List_List* _temp1949; struct _tuple0* _temp1951; struct Cyc_List_List*
_temp1953; struct Cyc_List_List* _temp1955; struct Cyc_Core_Opt* _temp1957;
struct Cyc_Absyn_Structdecl* _temp1959; struct Cyc_Absyn_Enumfield* _temp1961;
struct Cyc_Absyn_Enumdecl* _temp1963; struct _tuple0* _temp1965; struct Cyc_Absyn_Tunionfield*
_temp1967; struct Cyc_Absyn_Tuniondecl* _temp1969; struct Cyc_List_List*
_temp1971; struct Cyc_List_List* _temp1973; struct Cyc_Core_Opt* _temp1975;
struct _tuple0* _temp1977; struct Cyc_Absyn_Tunionfield* _temp1979; struct Cyc_Absyn_XTuniondecl*
_temp1981; struct Cyc_List_List* _temp1983; struct Cyc_List_List* _temp1985;
struct _tuple0* _temp1987; _LL1891: if(( int) _temp1889 == Cyc_Absyn_Wild_p){
goto _LL1892;} else{ goto _LL1893;} _LL1893: if(( int) _temp1889 == Cyc_Absyn_Null_p){
goto _LL1894;} else{ goto _LL1895;} _LL1895: if(( unsigned int) _temp1889 > 2u?((
struct _tunion_struct*) _temp1889)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1926:
_temp1925=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1889)->f1; goto
_LL1924; _LL1924: _temp1923=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp1889)->f2;
goto _LL1896;} else{ goto _LL1897;} _LL1897: if(( unsigned int) _temp1889 > 2u?((
struct _tunion_struct*) _temp1889)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL1928:
_temp1927=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1889)->f1; goto
_LL1898;} else{ goto _LL1899;} _LL1899: if(( unsigned int) _temp1889 > 2u?((
struct _tunion_struct*) _temp1889)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1930:
_temp1929=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1889)->f1;
goto _LL1900;} else{ goto _LL1901;} _LL1901: if(( unsigned int) _temp1889 > 2u?((
struct _tunion_struct*) _temp1889)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1932:
_temp1931=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1889)->f1; goto _LL1902;} else{ goto _LL1903;} _LL1903: if(( unsigned int)
_temp1889 > 2u?(( struct _tunion_struct*) _temp1889)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1934: _temp1933=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1889)->f1; goto _LL1904;} else{ goto _LL1905;} _LL1905: if(( unsigned int)
_temp1889 > 2u?(( struct _tunion_struct*) _temp1889)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1936: _temp1935=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1889)->f1; goto _LL1906;} else{ goto _LL1907;} _LL1907: if(( unsigned int)
_temp1889 > 2u?(( struct _tunion_struct*) _temp1889)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1938: _temp1937=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1889)->f1; goto _LL1908;} else{ goto _LL1909;} _LL1909: if(( unsigned int)
_temp1889 > 2u?(( struct _tunion_struct*) _temp1889)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL1940: _temp1939=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1889)->f1; goto _LL1910;} else{ goto _LL1911;} _LL1911: if(( unsigned int)
_temp1889 > 2u?(( struct _tunion_struct*) _temp1889)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1946: _temp1945=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1889)->f1; goto _LL1944; _LL1944: _temp1943=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1889)->f2; goto _LL1942; _LL1942:
_temp1941=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1889)->f3; goto _LL1912;} else{ goto _LL1913;} _LL1913: if(( unsigned int)
_temp1889 > 2u?(( struct _tunion_struct*) _temp1889)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1952: _temp1951=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1889)->f1; goto _LL1950; _LL1950: _temp1949=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1889)->f2; goto _LL1948; _LL1948:
_temp1947=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1889)->f3; goto _LL1914;} else{ goto _LL1915;} _LL1915: if(( unsigned int)
_temp1889 > 2u?(( struct _tunion_struct*) _temp1889)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1960: _temp1959=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1889)->f1; goto _LL1958; _LL1958: _temp1957=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1889)->f2; goto _LL1956; _LL1956:
_temp1955=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1889)->f3; goto _LL1954; _LL1954: _temp1953=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1889)->f4; goto _LL1916;} else{ goto
_LL1917;} _LL1917: if(( unsigned int) _temp1889 > 2u?(( struct _tunion_struct*)
_temp1889)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1966: _temp1965=( struct _tuple0*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1889)->f1; goto _LL1964; _LL1964:
_temp1963=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1889)->f2; goto _LL1962; _LL1962: _temp1961=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1889)->f3; goto _LL1918;} else{ goto
_LL1919;} _LL1919: if(( unsigned int) _temp1889 > 2u?(( struct _tunion_struct*)
_temp1889)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1978: _temp1977=( struct
_tuple0*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1889)->f1; goto _LL1976;
_LL1976: _temp1975=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1889)->f2; goto _LL1974; _LL1974: _temp1973=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1889)->f3; goto _LL1972; _LL1972:
_temp1971=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1889)->f4; goto _LL1970; _LL1970: _temp1969=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1889)->f5; goto _LL1968; _LL1968:
_temp1967=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1889)->f6; goto _LL1920;} else{ goto _LL1921;} _LL1921: if(( unsigned int)
_temp1889 > 2u?(( struct _tunion_struct*) _temp1889)->tag == Cyc_Absyn_XTunion_p_tag:
0){ _LL1988: _temp1987=( struct _tuple0*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1889)->f1; goto _LL1986; _LL1986: _temp1985=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1889)->f2; goto _LL1984; _LL1984:
_temp1983=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1889)->f3; goto _LL1982; _LL1982: _temp1981=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1889)->f4; goto _LL1980; _LL1980:
_temp1979=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1889)->f5; goto _LL1922;} else{ goto _LL1890;} _LL1892: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp1989=( char*)"_"; struct _tagged_string
_temp1990; _temp1990.curr= _temp1989; _temp1990.base= _temp1989; _temp1990.last_plus_one=
_temp1989 + 2; _temp1990;})); goto _LL1890; _LL1894: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1991=( char*)"null"; struct _tagged_string
_temp1992; _temp1992.curr= _temp1991; _temp1992.base= _temp1991; _temp1992.last_plus_one=
_temp1991 + 5; _temp1992;})); goto _LL1890; _LL1896: if( _temp1925 ==( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d", _temp1923));} else{ s= Cyc_PP_text(
xprintf("%u",( unsigned int) _temp1923));} goto _LL1890; _LL1898: s= Cyc_PP_text(({
struct _tagged_string _temp1993= Cyc_Absynpp_char_escape( _temp1927); xprintf("'%.*s'",
_temp1993.last_plus_one - _temp1993.curr, _temp1993.curr);})); goto _LL1890;
_LL1900: s= Cyc_PP_text( _temp1929); goto _LL1890; _LL1902: s= Cyc_Absynpp_qvar2doc(
_temp1931->name); goto _LL1890; _LL1904: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1994=( char*)"$("; struct _tagged_string
_temp1995; _temp1995.curr= _temp1994; _temp1995.base= _temp1994; _temp1995.last_plus_one=
_temp1994 + 3; _temp1995;})),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Pat*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,( struct _tagged_string)({ char* _temp1996=(
char*)","; struct _tagged_string _temp1997; _temp1997.curr= _temp1996; _temp1997.base=
_temp1996; _temp1997.last_plus_one= _temp1996 + 2; _temp1997;}), _temp1933), Cyc_PP_text((
struct _tagged_string)({ char* _temp1998=( char*)")"; struct _tagged_string
_temp1999; _temp1999.curr= _temp1998; _temp1999.base= _temp1998; _temp1999.last_plus_one=
_temp1998 + 2; _temp1999;}))); goto _LL1890; _LL1906: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp2000=( char*)"&"; struct _tagged_string
_temp2001; _temp2001.curr= _temp2000; _temp2001.base= _temp2000; _temp2001.last_plus_one=
_temp2000 + 2; _temp2001;})), Cyc_Absynpp_pat2doc( _temp1935)); goto _LL1890;
_LL1908: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2002=( char*)"*"; struct _tagged_string _temp2003; _temp2003.curr=
_temp2002; _temp2003.base= _temp2002; _temp2003.last_plus_one= _temp2002 + 2;
_temp2003;})), Cyc_Absynpp_qvar2doc( _temp1937->name)); goto _LL1890; _LL1910: s=
Cyc_Absynpp_qvar2doc( _temp1939); goto _LL1890; _LL1912: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1945), Cyc_Absynpp_tvars2doc( _temp1943), Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp2004=( char*)"("; struct _tagged_string
_temp2005; _temp2005.curr= _temp2004; _temp2005.base= _temp2004; _temp2005.last_plus_one=
_temp2004 + 2; _temp2005;}),( struct _tagged_string)({ char* _temp2006=( char*)")";
struct _tagged_string _temp2007; _temp2007.curr= _temp2006; _temp2007.base=
_temp2006; _temp2007.last_plus_one= _temp2006 + 2; _temp2007;}),( struct
_tagged_string)({ char* _temp2008=( char*)","; struct _tagged_string _temp2009;
_temp2009.curr= _temp2008; _temp2009.base= _temp2008; _temp2009.last_plus_one=
_temp2008 + 2; _temp2009;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1941))); goto _LL1890; _LL1914: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1951), Cyc_Absynpp_tvars2doc( _temp1949), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp2010=( char*)"{"; struct _tagged_string _temp2011;
_temp2011.curr= _temp2010; _temp2011.base= _temp2010; _temp2011.last_plus_one=
_temp2010 + 2; _temp2011;}),( struct _tagged_string)({ char* _temp2012=( char*)"}";
struct _tagged_string _temp2013; _temp2013.curr= _temp2012; _temp2013.base=
_temp2012; _temp2013.last_plus_one= _temp2012 + 2; _temp2013;}),( struct
_tagged_string)({ char* _temp2014=( char*)","; struct _tagged_string _temp2015;
_temp2015.curr= _temp2014; _temp2015.base= _temp2014; _temp2015.last_plus_one=
_temp2014 + 2; _temp2015;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1947))); goto _LL1890; _LL1916: s= Cyc_Absynpp_cat3( _temp1959->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp2016= _temp1959->name; if( _temp2016 == 0){ _throw( Null_Exception);}
_temp2016->v;})), Cyc_Absynpp_tvars2doc( _temp1955), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp2017=( char*)"{"; struct _tagged_string _temp2018;
_temp2018.curr= _temp2017; _temp2018.base= _temp2017; _temp2018.last_plus_one=
_temp2017 + 2; _temp2018;}),( struct _tagged_string)({ char* _temp2019=( char*)"}";
struct _tagged_string _temp2020; _temp2020.curr= _temp2019; _temp2020.base=
_temp2019; _temp2020.last_plus_one= _temp2019 + 2; _temp2020;}),( struct
_tagged_string)({ char* _temp2021=( char*)","; struct _tagged_string _temp2022;
_temp2022.curr= _temp2021; _temp2022.base= _temp2021; _temp2022.last_plus_one=
_temp2021 + 2; _temp2022;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1953))); goto _LL1890; _LL1918: s= Cyc_Absynpp_qvar2doc( _temp1965); goto
_LL1890; _LL1920: _temp1987= _temp1977; _temp1985= _temp1973; _temp1983=
_temp1971; goto _LL1922; _LL1922: if( _temp1983 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1987);} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1987), Cyc_Absynpp_tvars2doc(
_temp1985), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp2023=( char*)"(";
struct _tagged_string _temp2024; _temp2024.curr= _temp2023; _temp2024.base=
_temp2023; _temp2024.last_plus_one= _temp2023 + 2; _temp2024;}),( struct
_tagged_string)({ char* _temp2025=( char*)")"; struct _tagged_string _temp2026;
_temp2026.curr= _temp2025; _temp2026.base= _temp2025; _temp2026.last_plus_one=
_temp2025 + 2; _temp2026;}),( struct _tagged_string)({ char* _temp2027=( char*)",";
struct _tagged_string _temp2028; _temp2028.curr= _temp2027; _temp2028.base=
_temp2027; _temp2028.last_plus_one= _temp2027 + 2; _temp2028;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1983)));} goto _LL1890; _LL1890:;} return s;} struct
Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return Cyc_Absynpp_cat2(
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp2029=( char*)""; struct
_tagged_string _temp2030; _temp2030.curr= _temp2029; _temp2030.base= _temp2029;
_temp2030.last_plus_one= _temp2029 + 1; _temp2030;}),( struct _tagged_string)({
char* _temp2031=( char*)"="; struct _tagged_string _temp2032; _temp2032.curr=
_temp2031; _temp2032.base= _temp2031; _temp2032.last_plus_one= _temp2031 + 2;
_temp2032;}),( struct _tagged_string)({ char* _temp2033=( char*)"="; struct
_tagged_string _temp2034; _temp2034.curr= _temp2033; _temp2034.base= _temp2033;
_temp2034.last_plus_one= _temp2033 + 2; _temp2034;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)), Cyc_Absynpp_pat2doc((* dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
==( void*) Cyc_Absyn_Wild_p: 0){ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2035=( char*)"default: "; struct _tagged_string
_temp2036; _temp2036.curr= _temp2035; _temp2036.base= _temp2035; _temp2036.last_plus_one=
_temp2035 + 10; _temp2036;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause == 0){ return Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2037=( char*)"case "; struct
_tagged_string _temp2038; _temp2038.curr= _temp2037; _temp2038.base= _temp2037;
_temp2038.last_plus_one= _temp2037 + 6; _temp2038;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2039=( char*)": "; struct
_tagged_string _temp2040; _temp2040.curr= _temp2039; _temp2040.base= _temp2039;
_temp2040.last_plus_one= _temp2039 + 3; _temp2040;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ return Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2041=( char*)"case "; struct
_tagged_string _temp2042; _temp2042.curr= _temp2041; _temp2042.base= _temp2041;
_temp2042.last_plus_one= _temp2041 + 6; _temp2042;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2043=( char*)" && "; struct
_tagged_string _temp2044; _temp2044.curr= _temp2043; _temp2044.base= _temp2043;
_temp2044.last_plus_one= _temp2043 + 5; _temp2044;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2045= c->where_clause; if(
_temp2045 == 0){ _throw( Null_Exception);} _temp2045;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2046=( char*)": "; struct _tagged_string _temp2047;
_temp2047.curr= _temp2046; _temp2047.base= _temp2046; _temp2047.last_plus_one=
_temp2046 + 3; _temp2047;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,( struct _tagged_string)({
char* _temp2048=( char*)""; struct _tagged_string _temp2049; _temp2049.curr=
_temp2048; _temp2049.base= _temp2048; _temp2049.last_plus_one= _temp2048 + 1;
_temp2049;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield*
f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);} else{ return Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( f->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2050=( char*)" = "; struct _tagged_string _temp2051; _temp2051.curr=
_temp2050; _temp2051.base= _temp2050; _temp2051.last_plus_one= _temp2050 + 4;
_temp2051;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2052= f->tag; if( _temp2052 == 0){ _throw( Null_Exception);} _temp2052;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,(
struct _tagged_string)({ char* _temp2053=( char*)","; struct _tagged_string
_temp2054; _temp2054.curr= _temp2053; _temp2054.base= _temp2053; _temp2054.last_plus_one=
_temp2053 + 2; _temp2054;}), fs);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2055=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2081; struct Cyc_Absyn_Structdecl* _temp2083;
struct Cyc_Absyn_Uniondecl* _temp2085; struct Cyc_Absyn_Vardecl* _temp2087;
struct Cyc_Absyn_Vardecl _temp2089; struct Cyc_List_List* _temp2090; struct Cyc_Core_Opt*
_temp2092; int _temp2094; struct Cyc_Absyn_Exp* _temp2096; void* _temp2098;
struct Cyc_Absyn_Tqual* _temp2100; struct _tuple0* _temp2102; void* _temp2104;
struct Cyc_Absyn_Tuniondecl* _temp2106; struct Cyc_Absyn_XTuniondecl* _temp2108;
int _temp2110; struct Cyc_Absyn_Exp* _temp2112; struct Cyc_Core_Opt* _temp2114;
struct Cyc_Core_Opt* _temp2116; struct Cyc_Absyn_Pat* _temp2118; struct Cyc_Absyn_Enumdecl*
_temp2120; struct Cyc_Absyn_Enumdecl _temp2122; struct Cyc_List_List* _temp2123;
struct _tuple0* _temp2125; void* _temp2127; struct Cyc_Absyn_Typedefdecl*
_temp2129; struct Cyc_List_List* _temp2131; struct _tagged_string* _temp2133;
struct Cyc_List_List* _temp2135; struct _tuple0* _temp2137; struct Cyc_List_List*
_temp2139; _LL2057: if((( struct _tunion_struct*) _temp2055)->tag == Cyc_Absyn_Fn_d_tag){
_LL2082: _temp2081=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp2055)->f1; goto _LL2058;} else{ goto _LL2059;} _LL2059: if((( struct
_tunion_struct*) _temp2055)->tag == Cyc_Absyn_Struct_d_tag){ _LL2084: _temp2083=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp2055)->f1;
goto _LL2060;} else{ goto _LL2061;} _LL2061: if((( struct _tunion_struct*)
_temp2055)->tag == Cyc_Absyn_Union_d_tag){ _LL2086: _temp2085=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp2055)->f1; goto _LL2062;} else{ goto
_LL2063;} _LL2063: if((( struct _tunion_struct*) _temp2055)->tag == Cyc_Absyn_Var_d_tag){
_LL2088: _temp2087=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2055)->f1; _temp2089=* _temp2087; _LL2105: _temp2104=( void*) _temp2089.sc;
goto _LL2103; _LL2103: _temp2102=( struct _tuple0*) _temp2089.name; goto _LL2101;
_LL2101: _temp2100=( struct Cyc_Absyn_Tqual*) _temp2089.tq; goto _LL2099;
_LL2099: _temp2098=( void*) _temp2089.type; goto _LL2097; _LL2097: _temp2096=(
struct Cyc_Absyn_Exp*) _temp2089.initializer; goto _LL2095; _LL2095: _temp2094=(
int) _temp2089.shadow; goto _LL2093; _LL2093: _temp2092=( struct Cyc_Core_Opt*)
_temp2089.rgn; goto _LL2091; _LL2091: _temp2090=( struct Cyc_List_List*)
_temp2089.attributes; goto _LL2064;} else{ goto _LL2065;} _LL2065: if((( struct
_tunion_struct*) _temp2055)->tag == Cyc_Absyn_Tunion_d_tag){ _LL2107: _temp2106=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp2055)->f1;
goto _LL2066;} else{ goto _LL2067;} _LL2067: if((( struct _tunion_struct*)
_temp2055)->tag == Cyc_Absyn_XTunion_d_tag){ _LL2109: _temp2108=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp2055)->f1; goto _LL2068;} else{ goto
_LL2069;} _LL2069: if((( struct _tunion_struct*) _temp2055)->tag == Cyc_Absyn_Let_d_tag){
_LL2119: _temp2118=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2055)->f1; goto _LL2117; _LL2117: _temp2116=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2055)->f2; goto _LL2115; _LL2115: _temp2114=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2055)->f3; goto
_LL2113; _LL2113: _temp2112=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2055)->f4; goto _LL2111; _LL2111: _temp2110=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp2055)->f5; goto _LL2070;} else{ goto _LL2071;} _LL2071: if((( struct
_tunion_struct*) _temp2055)->tag == Cyc_Absyn_Enum_d_tag){ _LL2121: _temp2120=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp2055)->f1;
_temp2122=* _temp2120; _LL2128: _temp2127=( void*) _temp2122.sc; goto _LL2126;
_LL2126: _temp2125=( struct _tuple0*) _temp2122.name; goto _LL2124; _LL2124:
_temp2123=( struct Cyc_List_List*) _temp2122.fields; goto _LL2072;} else{ goto
_LL2073;} _LL2073: if((( struct _tunion_struct*) _temp2055)->tag == Cyc_Absyn_Typedef_d_tag){
_LL2130: _temp2129=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2055)->f1; goto _LL2074;} else{ goto _LL2075;} _LL2075: if((( struct
_tunion_struct*) _temp2055)->tag == Cyc_Absyn_Namespace_d_tag){ _LL2134:
_temp2133=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2055)->f1; goto _LL2132; _LL2132: _temp2131=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2055)->f2; goto _LL2076;} else{ goto
_LL2077;} _LL2077: if((( struct _tunion_struct*) _temp2055)->tag == Cyc_Absyn_Using_d_tag){
_LL2138: _temp2137=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2055)->f1; goto _LL2136; _LL2136: _temp2135=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp2055)->f2; goto _LL2078;} else{ goto
_LL2079;} _LL2079: if((( struct _tunion_struct*) _temp2055)->tag == Cyc_Absyn_ExternC_d_tag){
_LL2140: _temp2139=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2055)->f1; goto _LL2080;} else{ goto _LL2056;} _LL2058: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2151=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2151[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2152; _temp2152.tag= Cyc_Absyn_FnType_tag; _temp2152.f1=({ struct Cyc_Absyn_FnInfo
_temp2153; _temp2153.tvars= _temp2081->tvs; _temp2153.effect= _temp2081->effect;
_temp2153.ret_typ=( void*)(( void*) _temp2081->ret_type); _temp2153.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2081->args); _temp2153.varargs=
_temp2081->varargs; _temp2153.attributes= 0; _temp2153;}); _temp2152;});
_temp2151;}); s= Cyc_Absynpp_cat7( Cyc_PP_text( _temp2081->is_inline?( struct
_tagged_string)({ char* _temp2141=( char*)"inline "; struct _tagged_string
_temp2142; _temp2142.curr= _temp2141; _temp2142.base= _temp2141; _temp2142.last_plus_one=
_temp2141 + 8; _temp2142;}):( struct _tagged_string)({ char* _temp2143=( char*)"";
struct _tagged_string _temp2144; _temp2144.curr= _temp2143; _temp2144.base=
_temp2143; _temp2144.last_plus_one= _temp2143 + 1; _temp2144;})), Cyc_Absynpp_scope2doc((
void*) _temp2081->sc), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual* _temp2145=(
struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp2145->q_const= 0; _temp2145->q_volatile= 0; _temp2145->q_restrict= 0;
_temp2145;}), t,({ struct Cyc_Core_Opt* _temp2146=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2146->v=( void*) Cyc_Absynpp_cat2(
Cyc_Absynpp_atts2doc( _temp2081->attributes), Cyc_Absynpp_typedef_name2doc(
_temp2081->name)); _temp2146;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2147=( char*)" {"; struct _tagged_string _temp2148; _temp2148.curr=
_temp2147; _temp2148.base= _temp2147; _temp2148.last_plus_one= _temp2147 + 3;
_temp2148;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp2081->body))), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2149=( char*)"}"; struct _tagged_string _temp2150; _temp2150.curr=
_temp2149; _temp2150.base= _temp2149; _temp2150.last_plus_one= _temp2149 + 2;
_temp2150;}))); goto _LL2056;} _LL2060: if( _temp2083->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp2083->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2154=( char*)"struct "; struct _tagged_string
_temp2155; _temp2155.curr= _temp2154; _temp2155.base= _temp2154; _temp2155.last_plus_one=
_temp2154 + 8; _temp2155;})), _temp2083->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2156=( char*)""; struct _tagged_string _temp2157;
_temp2157.curr= _temp2156; _temp2157.base= _temp2156; _temp2157.last_plus_one=
_temp2156 + 1; _temp2157;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp2158= _temp2083->name; if( _temp2158 == 0){ _throw(
Null_Exception);} _temp2158->v;})), Cyc_Absynpp_ktvars2doc( _temp2083->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2159=( char*)";"; struct _tagged_string
_temp2160; _temp2160.curr= _temp2159; _temp2160.base= _temp2159; _temp2160.last_plus_one=
_temp2159 + 2; _temp2160;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2083->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2161=(
char*)"struct "; struct _tagged_string _temp2162; _temp2162.curr= _temp2161;
_temp2162.base= _temp2161; _temp2162.last_plus_one= _temp2161 + 8; _temp2162;})),
_temp2083->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2163=(
char*)""; struct _tagged_string _temp2164; _temp2164.curr= _temp2163; _temp2164.base=
_temp2163; _temp2164.last_plus_one= _temp2163 + 1; _temp2164;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2165= _temp2083->name; if(
_temp2165 == 0){ _throw( Null_Exception);} _temp2165->v;})), Cyc_Absynpp_ktvars2doc(
_temp2083->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2166=( char*)" {";
struct _tagged_string _temp2167; _temp2167.curr= _temp2166; _temp2167.base=
_temp2166; _temp2167.last_plus_one= _temp2166 + 3; _temp2167;})), Cyc_PP_nest( 2,
_temp2083->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2168=(
char*)""; struct _tagged_string _temp2169; _temp2169.curr= _temp2168; _temp2169.base=
_temp2168; _temp2169.last_plus_one= _temp2168 + 1; _temp2169;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2170= _temp2083->fields; if( _temp2170 == 0){ _throw(
Null_Exception);} _temp2170->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2171=( char*)"}"; struct _tagged_string _temp2172;
_temp2172.curr= _temp2171; _temp2172.base= _temp2171; _temp2172.last_plus_one=
_temp2171 + 2; _temp2172;})), Cyc_Absynpp_atts2doc( _temp2083->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2173=( char*)";"; struct _tagged_string
_temp2174; _temp2174.curr= _temp2173; _temp2174.base= _temp2173; _temp2174.last_plus_one=
_temp2173 + 2; _temp2174;})));} goto _LL2056; _LL2062: if( _temp2085->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp2085->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2175=( char*)"union "; struct _tagged_string
_temp2176; _temp2176.curr= _temp2175; _temp2176.base= _temp2175; _temp2176.last_plus_one=
_temp2175 + 7; _temp2176;})), _temp2085->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2177=( char*)""; struct _tagged_string _temp2178;
_temp2178.curr= _temp2177; _temp2178.base= _temp2177; _temp2178.last_plus_one=
_temp2177 + 1; _temp2178;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2179= _temp2085->name; if( _temp2179 == 0){ _throw(
Null_Exception);} _temp2179->v;})), Cyc_Absynpp_tvars2doc( _temp2085->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2180=( char*)";"; struct _tagged_string
_temp2181; _temp2181.curr= _temp2180; _temp2181.base= _temp2180; _temp2181.last_plus_one=
_temp2180 + 2; _temp2181;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2085->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2182=(
char*)"union "; struct _tagged_string _temp2183; _temp2183.curr= _temp2182;
_temp2183.base= _temp2182; _temp2183.last_plus_one= _temp2182 + 7; _temp2183;})),
_temp2085->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2184=(
char*)""; struct _tagged_string _temp2185; _temp2185.curr= _temp2184; _temp2185.base=
_temp2184; _temp2185.last_plus_one= _temp2184 + 1; _temp2185;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2186= _temp2085->name; if(
_temp2186 == 0){ _throw( Null_Exception);} _temp2186->v;})), Cyc_Absynpp_tvars2doc(
_temp2085->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2187=( char*)" {";
struct _tagged_string _temp2188; _temp2188.curr= _temp2187; _temp2188.base=
_temp2187; _temp2188.last_plus_one= _temp2187 + 3; _temp2188;})), Cyc_PP_nest( 2,
_temp2085->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2189=(
char*)""; struct _tagged_string _temp2190; _temp2190.curr= _temp2189; _temp2190.base=
_temp2189; _temp2190.last_plus_one= _temp2189 + 1; _temp2190;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2191= _temp2085->fields; if( _temp2191 == 0){ _throw(
Null_Exception);} _temp2191->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2192=( char*)"}"; struct _tagged_string _temp2193;
_temp2193.curr= _temp2192; _temp2193.base= _temp2192; _temp2193.last_plus_one=
_temp2192 + 2; _temp2193;})), Cyc_Absynpp_atts2doc( _temp2085->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2194=( char*)";"; struct _tagged_string
_temp2195; _temp2195.curr= _temp2194; _temp2195.base= _temp2194; _temp2195.last_plus_one=
_temp2194 + 2; _temp2195;})));} goto _LL2056; _LL2064: { struct Cyc_PP_Doc* sn=
Cyc_Absynpp_typedef_name2doc( _temp2102); s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(
_temp2104), Cyc_Absynpp_tqtd2doc( _temp2100, _temp2098,({ struct Cyc_Core_Opt*
_temp2196=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2196->v=( void*) sn; _temp2196;})), Cyc_Absynpp_atts2doc( _temp2090),
_temp2096 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2197=( char*)"";
struct _tagged_string _temp2198; _temp2198.curr= _temp2197; _temp2198.base=
_temp2197; _temp2198.last_plus_one= _temp2197 + 1; _temp2198;})): Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2199=( char*)" = "; struct
_tagged_string _temp2200; _temp2200.curr= _temp2199; _temp2200.base= _temp2199;
_temp2200.last_plus_one= _temp2199 + 4; _temp2200;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2201= _temp2096; if(
_temp2201 == 0){ _throw( Null_Exception);} _temp2201;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2202=( char*)";"; struct _tagged_string _temp2203;
_temp2203.curr= _temp2202; _temp2203.base= _temp2202; _temp2203.last_plus_one=
_temp2202 + 2; _temp2203;}))); goto _LL2056;} _LL2066: if( _temp2106->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp2106->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2204=( char*)"tunion "; struct
_tagged_string _temp2205; _temp2205.curr= _temp2204; _temp2205.base= _temp2204;
_temp2205.last_plus_one= _temp2204 + 8; _temp2205;})), _temp2106->name == 0? Cyc_PP_text((
struct _tagged_string)({ char* _temp2206=( char*)""; struct _tagged_string
_temp2207; _temp2207.curr= _temp2206; _temp2207.base= _temp2206; _temp2207.last_plus_one=
_temp2206 + 1; _temp2207;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp2208= _temp2106->name; if( _temp2208 == 0){ _throw(
Null_Exception);} _temp2208->v;})), Cyc_Absynpp_ktvars2doc( _temp2106->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2209=( char*)";"; struct _tagged_string
_temp2210; _temp2210.curr= _temp2209; _temp2210.base= _temp2209; _temp2210.last_plus_one=
_temp2209 + 2; _temp2210;})));} else{ s= Cyc_Absynpp_cat8( Cyc_Absynpp_scope2doc((
void*) _temp2106->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2211=(
char*)"tunion "; struct _tagged_string _temp2212; _temp2212.curr= _temp2211;
_temp2212.base= _temp2211; _temp2212.last_plus_one= _temp2211 + 8; _temp2212;})),
_temp2106->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2213=(
char*)""; struct _tagged_string _temp2214; _temp2214.curr= _temp2213; _temp2214.base=
_temp2213; _temp2214.last_plus_one= _temp2213 + 1; _temp2214;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2215= _temp2106->name; if(
_temp2215 == 0){ _throw( Null_Exception);} _temp2215->v;})), Cyc_Absynpp_ktvars2doc(
_temp2106->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2216=( char*)" {";
struct _tagged_string _temp2217; _temp2217.curr= _temp2216; _temp2217.base=
_temp2216; _temp2217.last_plus_one= _temp2216 + 3; _temp2217;})), Cyc_PP_nest( 2,
_temp2106->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2218=(
char*)""; struct _tagged_string _temp2219; _temp2219.curr= _temp2218; _temp2219.base=
_temp2218; _temp2219.last_plus_one= _temp2218 + 1; _temp2219;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2220= _temp2106->fields; if( _temp2220 == 0){ _throw(
Null_Exception);} _temp2220->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2221=( char*)"};"; struct _tagged_string _temp2222;
_temp2222.curr= _temp2221; _temp2222.base= _temp2221; _temp2222.last_plus_one=
_temp2221 + 3; _temp2222;})));} goto _LL2056; _LL2068: s= Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc((
void*) _temp2108->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2223=(
char*)"xtunion "; struct _tagged_string _temp2224; _temp2224.curr= _temp2223;
_temp2224.base= _temp2223; _temp2224.last_plus_one= _temp2223 + 9; _temp2224;})),
Cyc_Absynpp_typedef_name2doc( _temp2108->name), _temp2108->fields == 0? Cyc_PP_text((
struct _tagged_string)({ char* _temp2225=( char*)";"; struct _tagged_string
_temp2226; _temp2226.curr= _temp2225; _temp2226.base= _temp2225; _temp2226.last_plus_one=
_temp2225 + 2; _temp2226;})): Cyc_Absynpp_cat4( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2227=( char*)" {"; struct _tagged_string _temp2228;
_temp2228.curr= _temp2227; _temp2228.base= _temp2227; _temp2228.last_plus_one=
_temp2227 + 3; _temp2228;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_tunionfields2doc( _temp2108->fields))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp2229=( char*)"};"; struct _tagged_string
_temp2230; _temp2230.curr= _temp2229; _temp2230.base= _temp2229; _temp2230.last_plus_one=
_temp2229 + 3; _temp2230;})))); goto _LL2056; _LL2070: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp2231=( char*)"let "; struct _tagged_string
_temp2232; _temp2232.curr= _temp2231; _temp2232.base= _temp2231; _temp2232.last_plus_one=
_temp2231 + 5; _temp2232;})), Cyc_Absynpp_pat2doc( _temp2118), Cyc_PP_text((
struct _tagged_string)({ char* _temp2233=( char*)" = "; struct _tagged_string
_temp2234; _temp2234.curr= _temp2233; _temp2234.base= _temp2233; _temp2234.last_plus_one=
_temp2233 + 4; _temp2234;})), Cyc_Absynpp_exp2doc( _temp2112), Cyc_PP_text((
struct _tagged_string)({ char* _temp2235=( char*)";"; struct _tagged_string
_temp2236; _temp2236.curr= _temp2235; _temp2236.base= _temp2235; _temp2236.last_plus_one=
_temp2235 + 2; _temp2236;}))); goto _LL2056; _LL2072: s= Cyc_Absynpp_cat7( Cyc_Absynpp_scope2doc(
_temp2127), Cyc_PP_text(( struct _tagged_string)({ char* _temp2237=( char*)"enum ";
struct _tagged_string _temp2238; _temp2238.curr= _temp2237; _temp2238.base=
_temp2237; _temp2238.last_plus_one= _temp2237 + 6; _temp2238;})), Cyc_Absynpp_qvar2doc(
_temp2125), Cyc_PP_text(( struct _tagged_string)({ char* _temp2239=( char*)" {";
struct _tagged_string _temp2240; _temp2240.curr= _temp2239; _temp2240.base=
_temp2239; _temp2240.last_plus_one= _temp2239 + 3; _temp2240;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_enumfields2doc( _temp2123))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp2241=( char*)"};";
struct _tagged_string _temp2242; _temp2242.curr= _temp2241; _temp2242.base=
_temp2241; _temp2242.last_plus_one= _temp2241 + 3; _temp2242;}))); goto _LL2056;
_LL2074: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2243=( char*)"typedef "; struct _tagged_string _temp2244; _temp2244.curr=
_temp2243; _temp2244.base= _temp2243; _temp2244.last_plus_one= _temp2243 + 9;
_temp2244;})), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual* _temp2245=(
struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp2245->q_const= 0; _temp2245->q_volatile= 0; _temp2245->q_restrict= 0;
_temp2245;}),( void*) _temp2129->defn,({ struct Cyc_Core_Opt* _temp2246=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2246->v=( void*)
Cyc_Absynpp_cat2( Cyc_Absynpp_typedef_name2doc( _temp2129->name), Cyc_Absynpp_tvars2doc(
_temp2129->tvs)); _temp2246;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2247=( char*)";"; struct _tagged_string _temp2248; _temp2248.curr=
_temp2247; _temp2248.base= _temp2247; _temp2248.last_plus_one= _temp2247 + 2;
_temp2248;}))); goto _LL2056; _LL2076: if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_add(
_temp2133);} s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2249=( char*)"namespace "; struct _tagged_string _temp2250; _temp2250.curr=
_temp2249; _temp2250.base= _temp2249; _temp2250.last_plus_one= _temp2249 + 11;
_temp2250;})), Cyc_Absynpp_textptr( _temp2133), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2251=( char*)" {"; struct _tagged_string _temp2252;
_temp2252.curr= _temp2251; _temp2252.base= _temp2251; _temp2252.last_plus_one=
_temp2251 + 3; _temp2252;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2253=( char*)""; struct _tagged_string _temp2254; _temp2254.curr=
_temp2253; _temp2254.base= _temp2253; _temp2254.last_plus_one= _temp2253 + 1;
_temp2254;}), _temp2131), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2255=( char*)"}"; struct _tagged_string _temp2256; _temp2256.curr=
_temp2255; _temp2256.base= _temp2255; _temp2256.last_plus_one= _temp2255 + 2;
_temp2256;}))); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2056; _LL2078: if( Cyc_Absynpp_print_using_stmts){ s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2257=( char*)"using "; struct
_tagged_string _temp2258; _temp2258.curr= _temp2257; _temp2258.base= _temp2257;
_temp2258.last_plus_one= _temp2257 + 7; _temp2258;})), Cyc_Absynpp_qvar2doc(
_temp2137), Cyc_PP_text(( struct _tagged_string)({ char* _temp2259=( char*)" {";
struct _tagged_string _temp2260; _temp2260.curr= _temp2259; _temp2260.base=
_temp2259; _temp2260.last_plus_one= _temp2259 + 3; _temp2260;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2261=( char*)""; struct _tagged_string
_temp2262; _temp2262.curr= _temp2261; _temp2262.base= _temp2261; _temp2262.last_plus_one=
_temp2261 + 1; _temp2262;}), _temp2135), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2263=( char*)"}"; struct _tagged_string _temp2264;
_temp2264.curr= _temp2263; _temp2264.base= _temp2263; _temp2264.last_plus_one=
_temp2263 + 2; _temp2264;})));} else{ s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2265=( char*)"/* using "; struct _tagged_string
_temp2266; _temp2266.curr= _temp2265; _temp2266.base= _temp2265; _temp2266.last_plus_one=
_temp2265 + 10; _temp2266;})), Cyc_Absynpp_qvar2doc( _temp2137), Cyc_PP_text((
struct _tagged_string)({ char* _temp2267=( char*)" { */"; struct _tagged_string
_temp2268; _temp2268.curr= _temp2267; _temp2268.base= _temp2267; _temp2268.last_plus_one=
_temp2267 + 6; _temp2268;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2269=( char*)""; struct _tagged_string _temp2270; _temp2270.curr=
_temp2269; _temp2270.base= _temp2269; _temp2270.last_plus_one= _temp2269 + 1;
_temp2270;}), _temp2135), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2271=( char*)"/* } */"; struct _tagged_string _temp2272; _temp2272.curr=
_temp2271; _temp2272.base= _temp2271; _temp2272.last_plus_one= _temp2271 + 8;
_temp2272;})));} goto _LL2056; _LL2080: if( Cyc_Absynpp_print_externC_stmts){ s=
Cyc_Absynpp_cat5( Cyc_PP_text(( struct _tagged_string)({ char* _temp2273=( char*)"extern \"C\" {";
struct _tagged_string _temp2274; _temp2274.curr= _temp2273; _temp2274.base=
_temp2273; _temp2274.last_plus_one= _temp2273 + 13; _temp2274;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2275=( char*)""; struct _tagged_string
_temp2276; _temp2276.curr= _temp2275; _temp2276.base= _temp2275; _temp2276.last_plus_one=
_temp2275 + 1; _temp2276;}), _temp2139), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2277=( char*)"}"; struct _tagged_string _temp2278;
_temp2278.curr= _temp2277; _temp2278.base= _temp2277; _temp2278.last_plus_one=
_temp2277 + 2; _temp2278;})));} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2279=( char*)"/* extern \"C\" { */"; struct
_tagged_string _temp2280; _temp2280.curr= _temp2279; _temp2280.base= _temp2279;
_temp2280.last_plus_one= _temp2279 + 19; _temp2280;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2281=( char*)""; struct _tagged_string
_temp2282; _temp2282.curr= _temp2281; _temp2282.base= _temp2281; _temp2282.last_plus_one=
_temp2281 + 1; _temp2282;}), _temp2139), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2283=( char*)"/* } */"; struct _tagged_string
_temp2284; _temp2284.curr= _temp2283; _temp2284.base= _temp2283; _temp2284.last_plus_one=
_temp2283 + 8; _temp2284;})));} goto _LL2056; _LL2056:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void* sc){ void* _temp2285= sc; _LL2287: if(( int)
_temp2285 == Cyc_Absyn_Static){ goto _LL2288;} else{ goto _LL2289;} _LL2289: if((
int) _temp2285 == Cyc_Absyn_Public){ goto _LL2290;} else{ goto _LL2291;} _LL2291:
if(( int) _temp2285 == Cyc_Absyn_Extern){ goto _LL2292;} else{ goto _LL2293;}
_LL2293: if(( int) _temp2285 == Cyc_Absyn_ExternC){ goto _LL2294;} else{ goto
_LL2295;} _LL2295: if(( int) _temp2285 == Cyc_Absyn_Abstract){ goto _LL2296;}
else{ goto _LL2286;} _LL2288: return Cyc_PP_text(( struct _tagged_string)({ char*
_temp2297=( char*)"static "; struct _tagged_string _temp2298; _temp2298.curr=
_temp2297; _temp2298.base= _temp2297; _temp2298.last_plus_one= _temp2297 + 8;
_temp2298;})); _LL2290: return Cyc_PP_nil_doc(); _LL2292: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2299=( char*)"extern "; struct
_tagged_string _temp2300; _temp2300.curr= _temp2299; _temp2300.base= _temp2299;
_temp2300.last_plus_one= _temp2299 + 8; _temp2300;})); _LL2294: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2301=( char*)"extern \"C\" "; struct
_tagged_string _temp2302; _temp2302.curr= _temp2301; _temp2302.base= _temp2301;
_temp2302.last_plus_one= _temp2301 + 12; _temp2302;})); _LL2296: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2303=( char*)"abstract "; struct
_tagged_string _temp2304; _temp2304.curr= _temp2303; _temp2304.base= _temp2303;
_temp2304.last_plus_one= _temp2303 + 10; _temp2304;})); _LL2286:;} int Cyc_Absynpp_exists_temp_tvar_in_effect(
void* t){ void* _temp2305= t; struct Cyc_Absyn_Tvar* _temp2313; struct Cyc_List_List*
_temp2315; _LL2307: if(( unsigned int) _temp2305 > 4u?(( struct _tunion_struct*)
_temp2305)->tag == Cyc_Absyn_VarType_tag: 0){ _LL2314: _temp2313=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp2305)->f1; goto _LL2308;} else{ goto
_LL2309;} _LL2309: if(( unsigned int) _temp2305 > 4u?(( struct _tunion_struct*)
_temp2305)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL2316: _temp2315=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp2305)->f1; goto _LL2310;} else{ goto
_LL2311;} _LL2311: goto _LL2312; _LL2308: return Cyc_Tcutil_is_temp_tvar(
_temp2313); _LL2310: return(( int(*)( int(* pred)( void*), struct Cyc_List_List*
x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect, _temp2315);
_LL2312: return 0; _LL2306:;} struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual*
tq, void* t){ void* _temp2317= t; struct Cyc_Absyn_Exp* _temp2331; struct Cyc_Absyn_Tqual*
_temp2333; void* _temp2335; struct Cyc_Absyn_PtrInfo _temp2337; struct Cyc_Absyn_Conref*
_temp2339; struct Cyc_Absyn_Tqual* _temp2341; struct Cyc_Absyn_Conref* _temp2343;
void* _temp2345; void* _temp2347; struct Cyc_Absyn_FnInfo _temp2349; struct Cyc_List_List*
_temp2351; int _temp2353; struct Cyc_List_List* _temp2355; void* _temp2357;
struct Cyc_Core_Opt* _temp2359; struct Cyc_List_List* _temp2361; int _temp2363;
struct Cyc_Core_Opt* _temp2365; void* _temp2367; struct Cyc_Core_Opt* _temp2369;
struct Cyc_List_List* _temp2371; struct _tuple0* _temp2373; _LL2319: if((
unsigned int) _temp2317 > 4u?(( struct _tunion_struct*) _temp2317)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2336: _temp2335=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2317)->f1;
goto _LL2334; _LL2334: _temp2333=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2317)->f2; goto _LL2332; _LL2332: _temp2331=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2317)->f3; goto _LL2320;} else{ goto
_LL2321;} _LL2321: if(( unsigned int) _temp2317 > 4u?(( struct _tunion_struct*)
_temp2317)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2338: _temp2337=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2317)->f1;
_LL2348: _temp2347=( void*) _temp2337.elt_typ; goto _LL2346; _LL2346: _temp2345=(
void*) _temp2337.rgn_typ; goto _LL2344; _LL2344: _temp2343=( struct Cyc_Absyn_Conref*)
_temp2337.nullable; goto _LL2342; _LL2342: _temp2341=( struct Cyc_Absyn_Tqual*)
_temp2337.tq; goto _LL2340; _LL2340: _temp2339=( struct Cyc_Absyn_Conref*)
_temp2337.bounds; goto _LL2322;} else{ goto _LL2323;} _LL2323: if(( unsigned int)
_temp2317 > 4u?(( struct _tunion_struct*) _temp2317)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2350: _temp2349=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2317)->f1; _LL2362: _temp2361=( struct Cyc_List_List*) _temp2349.tvars;
goto _LL2360; _LL2360: _temp2359=( struct Cyc_Core_Opt*) _temp2349.effect; goto
_LL2358; _LL2358: _temp2357=( void*) _temp2349.ret_typ; goto _LL2356; _LL2356:
_temp2355=( struct Cyc_List_List*) _temp2349.args; goto _LL2354; _LL2354:
_temp2353=( int) _temp2349.varargs; goto _LL2352; _LL2352: _temp2351=( struct
Cyc_List_List*) _temp2349.attributes; goto _LL2324;} else{ goto _LL2325;}
_LL2325: if(( unsigned int) _temp2317 > 4u?(( struct _tunion_struct*) _temp2317)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL2368: _temp2367=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2317)->f1; goto _LL2366; _LL2366: _temp2365=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp2317)->f2; goto _LL2364; _LL2364: _temp2363=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp2317)->f3; goto _LL2326;} else{ goto
_LL2327;} _LL2327: if(( unsigned int) _temp2317 > 4u?(( struct _tunion_struct*)
_temp2317)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL2374: _temp2373=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2317)->f1; goto _LL2372;
_LL2372: _temp2371=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2317)->f2; goto _LL2370; _LL2370: _temp2369=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2317)->f3; goto _LL2328;} else{ goto
_LL2329;} _LL2329: goto _LL2330; _LL2320: { struct Cyc_List_List* _temp2377;
void* _temp2379; struct Cyc_Absyn_Tqual* _temp2381; struct _tuple4 _temp2375=
Cyc_Absynpp_to_tms( _temp2333, _temp2335); _LL2382: _temp2381= _temp2375.f1;
goto _LL2380; _LL2380: _temp2379= _temp2375.f2; goto _LL2378; _LL2378: _temp2377=
_temp2375.f3; goto _LL2376; _LL2376: { void* tm; if( _temp2331 == 0){ tm=( void*)
Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2383=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2383[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2384; _temp2384.tag= Cyc_Absyn_ConstArray_mod_tag; _temp2384.f1=( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2385= _temp2331; if( _temp2385 == 0){
_throw( Null_Exception);} _temp2385;}); _temp2384;}); _temp2383;});} return({
struct _tuple4 _temp2386; _temp2386.f1= _temp2381; _temp2386.f2= _temp2379;
_temp2386.f3=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2387=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2387->hd=( void*)
tm; _temp2387->tl= _temp2377; _temp2387;}); _temp2386;});}} _LL2322: { struct
Cyc_List_List* _temp2390; void* _temp2392; struct Cyc_Absyn_Tqual* _temp2394;
struct _tuple4 _temp2388= Cyc_Absynpp_to_tms( _temp2341, _temp2347); _LL2395:
_temp2394= _temp2388.f1; goto _LL2393; _LL2393: _temp2392= _temp2388.f2; goto
_LL2391; _LL2391: _temp2390= _temp2388.f3; goto _LL2389; _LL2389: { void* ps;{
struct _tuple8 _temp2397=({ struct _tuple8 _temp2396; _temp2396.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2343))->v; _temp2396.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2339))->v; _temp2396;}); void* _temp2407;
void* _temp2409; struct Cyc_Absyn_Exp* _temp2411; void* _temp2413; int _temp2415;
void* _temp2417; void* _temp2419; struct Cyc_Absyn_Exp* _temp2421; void*
_temp2423; int _temp2425; void* _temp2427; void* _temp2429; void* _temp2431;
_LL2399: _LL2414: _temp2413= _temp2397.f1; if(( unsigned int) _temp2413 > 1u?((
struct _tunion_struct*) _temp2413)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2416:
_temp2415=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2413)->f1; if(
_temp2415 == 1){ goto _LL2408;} else{ goto _LL2401;}} else{ goto _LL2401;}
_LL2408: _temp2407= _temp2397.f2; if(( unsigned int) _temp2407 > 1u?(( struct
_tunion_struct*) _temp2407)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2410:
_temp2409=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2407)->f1; if((
unsigned int) _temp2409 > 1u?(( struct _tunion_struct*) _temp2409)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2412: _temp2411=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2409)->f1; goto _LL2400;} else{ goto _LL2401;}} else{ goto _LL2401;}
_LL2401: _LL2424: _temp2423= _temp2397.f1; if(( unsigned int) _temp2423 > 1u?((
struct _tunion_struct*) _temp2423)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2426:
_temp2425=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2423)->f1; if(
_temp2425 == 0){ goto _LL2418;} else{ goto _LL2403;}} else{ goto _LL2403;}
_LL2418: _temp2417= _temp2397.f2; if(( unsigned int) _temp2417 > 1u?(( struct
_tunion_struct*) _temp2417)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2420:
_temp2419=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2417)->f1; if((
unsigned int) _temp2419 > 1u?(( struct _tunion_struct*) _temp2419)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2422: _temp2421=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2419)->f1; goto _LL2402;} else{ goto _LL2403;}} else{ goto _LL2403;}
_LL2403: _LL2432: _temp2431= _temp2397.f1; goto _LL2428; _LL2428: _temp2427=
_temp2397.f2; if(( unsigned int) _temp2427 > 1u?(( struct _tunion_struct*)
_temp2427)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2430: _temp2429=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2427)->f1; if(( int) _temp2429 == Cyc_Absyn_Unknown_b){
goto _LL2404;} else{ goto _LL2405;}} else{ goto _LL2405;} _LL2405: goto _LL2406;
_LL2400: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2433=( struct
Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2433[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2434; _temp2434.tag=
Cyc_Absyn_Nullable_ps_tag; _temp2434.f1= _temp2411; _temp2434;}); _temp2433;});
goto _LL2398; _LL2402: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2435=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2435[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2436; _temp2436.tag= Cyc_Absyn_NonNullable_ps_tag; _temp2436.f1= _temp2421;
_temp2436;}); _temp2435;}); goto _LL2398; _LL2404: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2398; _LL2406: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2398;
_LL2398:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2439=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2439[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2440; _temp2440.tag=
Cyc_Absyn_Pointer_mod_tag; _temp2440.f1=( void*) ps; _temp2440.f2=( void*)
_temp2345; _temp2440.f3= tq; _temp2440;}); _temp2439;}); return({ struct _tuple4
_temp2437; _temp2437.f1= _temp2394; _temp2437.f2= _temp2392; _temp2437.f3=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2438=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2438->hd=( void*) tm; _temp2438->tl=
_temp2390; _temp2438;}); _temp2437;});}}} _LL2324: if( ! Cyc_Absynpp_print_all_tvars){
if( _temp2359 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)({ struct
Cyc_Core_Opt* _temp2441= _temp2359; if( _temp2441 == 0){ _throw( Null_Exception);}
_temp2441->v;}))){ _temp2359= 0; _temp2361= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2361);}}{ struct Cyc_List_List* _temp2444;
void* _temp2446; struct Cyc_Absyn_Tqual* _temp2448; struct _tuple4 _temp2442=
Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(), _temp2357); _LL2449: _temp2448=
_temp2442.f1; goto _LL2447; _LL2447: _temp2446= _temp2442.f2; goto _LL2445;
_LL2445: _temp2444= _temp2442.f3; goto _LL2443; _LL2443: { struct Cyc_List_List*
tms= _temp2444; if( _temp2351 != 0){ tms=({ struct Cyc_List_List* _temp2450=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2450->hd=(
void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp2451=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2451[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2452; _temp2452.tag= Cyc_Absyn_Attributes_mod_tag;
_temp2452.f1= 0; _temp2452.f2= _temp2351; _temp2452;}); _temp2451;})); _temp2450->tl=
tms; _temp2450;});} tms=({ struct Cyc_List_List* _temp2453=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2453->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2454=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2454[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2455; _temp2455.tag= Cyc_Absyn_Function_mod_tag;
_temp2455.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2456=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2456[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2457; _temp2457.tag= Cyc_Absyn_WithTypes_tag;
_temp2457.f1= _temp2355; _temp2457.f2= _temp2353; _temp2457.f3= _temp2359;
_temp2457;}); _temp2456;})); _temp2455;}); _temp2454;})); _temp2453->tl= tms;
_temp2453;}); if( _temp2361 != 0){ tms=({ struct Cyc_List_List* _temp2458=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2458->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2459=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2459[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2460; _temp2460.tag= Cyc_Absyn_TypeParams_mod_tag;
_temp2460.f1= _temp2361; _temp2460.f2= 0; _temp2460.f3= 1; _temp2460;});
_temp2459;})); _temp2458->tl= tms; _temp2458;});} return({ struct _tuple4
_temp2461; _temp2461.f1= _temp2448; _temp2461.f2= _temp2446; _temp2461.f3= tms;
_temp2461;});}} _LL2326: if( _temp2365 == 0){ return({ struct _tuple4 _temp2462;
_temp2462.f1= tq; _temp2462.f2= t; _temp2462.f3= 0; _temp2462;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2463= _temp2365; if(
_temp2463 == 0){ _throw( Null_Exception);} _temp2463->v;}));} _LL2328: if(
_temp2369 == 0? 1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4
_temp2464; _temp2464.f1= tq; _temp2464.f2= t; _temp2464.f3= 0; _temp2464;});}
else{ return Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2465=
_temp2369; if( _temp2465 == 0){ _throw( Null_Exception);} _temp2465->v;}));}
_LL2330: return({ struct _tuple4 _temp2466; _temp2466.f1= tq; _temp2466.f2= t;
_temp2466.f3= 0; _temp2466;}); _LL2318:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual* tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp2469; void* _temp2471; struct Cyc_Absyn_Tqual* _temp2473; struct _tuple4
_temp2467= Cyc_Absynpp_to_tms( tq, t); _LL2474: _temp2473= _temp2467.f1; goto
_LL2472; _LL2472: _temp2471= _temp2467.f2; goto _LL2470; _LL2470: _temp2469=
_temp2467.f3; goto _LL2468; _LL2468: _temp2469=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2469); if( _temp2469 == 0?
dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc( _temp2473), Cyc_Absynpp_ntyp2doc(
_temp2471));} else{ return Cyc_Absynpp_cat4( Cyc_Absynpp_tqual2doc( _temp2473),
Cyc_Absynpp_ntyp2doc( _temp2471), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2475=( char*)" "; struct _tagged_string _temp2476; _temp2476.curr=
_temp2475; _temp2476.base= _temp2475; _temp2476.last_plus_one= _temp2475 + 2;
_temp2476;})), Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)({
struct Cyc_Core_Opt* _temp2477= dopt; if( _temp2477 == 0){ _throw(
Null_Exception);} _temp2477->v;}), _temp2469));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return Cyc_Absynpp_cat5(
Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2478=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2478->v=(
void*) Cyc_Absynpp_textptr( f->name); _temp2478;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2479=( char*)":"; struct _tagged_string _temp2480;
_temp2480.curr= _temp2479; _temp2480.base= _temp2479; _temp2480.last_plus_one=
_temp2479 + 2; _temp2480;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp2481= f->width; if( _temp2481 == 0){ _throw(
Null_Exception);} _temp2481->v;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2482=( char*)";"; struct _tagged_string
_temp2483; _temp2483.curr= _temp2482; _temp2483.base= _temp2482; _temp2483.last_plus_one=
_temp2482 + 2; _temp2483;})));} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2484=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2484->v=( void*) Cyc_Absynpp_textptr(
f->name); _temp2484;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2485=( char*)";"; struct _tagged_string
_temp2486; _temp2486.curr= _temp2485; _temp2486.base= _temp2485; _temp2486.last_plus_one=
_temp2485 + 2; _temp2486;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,( struct _tagged_string)({ char*
_temp2487=( char*)""; struct _tagged_string _temp2488; _temp2488.curr= _temp2487;
_temp2488.base= _temp2487; _temp2488.last_plus_one= _temp2487 + 1; _temp2488;}),
fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc( struct Cyc_Absyn_Tunionfield*
f){ return Cyc_Absynpp_cat3( Cyc_Absynpp_typedef_name2doc( f->name), f->tvs == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs), f->typs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,( struct _tagged_string)({ char*
_temp2489=( char*)","; struct _tagged_string _temp2490; _temp2490.curr=
_temp2489; _temp2490.base= _temp2489; _temp2490.last_plus_one= _temp2489 + 2;
_temp2490;}), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl=({ struct Cyc_List_List*
_temp2491= tdl; if( _temp2491 == 0){ _throw( Null_Exception);} _temp2491->tl;})){
Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp2492= tdl; if( _temp2492 == 0){ _throw( Null_Exception);} _temp2492->hd;})),
72, f); fprintf( f,"\n");}} struct _tagged_string Cyc_Absynpp_decllist2string(
struct Cyc_List_List* tdl){ return Cyc_PP_string_of_doc( Cyc_PP_seql(( struct
_tagged_string)({ char* _temp2493=( char*)""; struct _tagged_string _temp2494;
_temp2494.curr= _temp2493; _temp2494.base= _temp2493; _temp2494.last_plus_one=
_temp2493 + 1; _temp2494;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
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