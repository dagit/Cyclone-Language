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
typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t; struct
_tuple0{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple0* Cyc_Absyn_qvar_t;
typedef struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
typedef void* Cyc_Absyn_Scope; struct Cyc_Absyn_Tqual; typedef void* Cyc_Absyn_Size_of;
typedef void* Cyc_Absyn_Kind; typedef void* Cyc_Absyn_Array_kind; typedef void*
Cyc_Absyn_Sign; struct Cyc_Absyn_Conref; typedef void* Cyc_Absyn_Constraint;
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
Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t; typedef
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
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int varargs; struct Cyc_List_List*
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
8; struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual
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
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod_tag=
2; struct Cyc_Absyn_Function_mod_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_TypeParams_mod_tag= 3; struct Cyc_Absyn_TypeParams_mod_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ; static
const int Cyc_Absyn_Attributes_mod_tag= 4; struct Cyc_Absyn_Attributes_mod_struct{
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
23; struct _tuple1{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct
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
f3; } ; static const int Cyc_Absyn_While_s_tag= 4; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s_tag=
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
Cyc_Absyn_Vardecl{ void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual tq;
void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct
Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline;
struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect;
void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
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
char* tag; } ; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern
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
_LL278: if(( unsigned int) _temp276 > 1u?(( struct _tunion_struct*) _temp276)->tag
== Cyc_Absyn_Pointer_mod_tag: 0){ _LL287: _temp286=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp276)->f1; goto _LL285; _LL285: _temp284=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
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
_temp442= t; struct Cyc_Absyn_Exp* _temp488; struct Cyc_Absyn_Tqual _temp490;
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
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp442)->f2;
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
0, e);} struct _tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e);
struct Cyc_PP_Doc* s;{ void* _temp1042=( void*) e->r; void* _temp1114; void*
_temp1116; struct _tuple0* _temp1118; struct _tuple0* _temp1120; struct Cyc_List_List*
_temp1122; void* _temp1124; struct Cyc_Absyn_Exp* _temp1126; struct Cyc_Core_Opt*
_temp1128; struct Cyc_Absyn_Exp* _temp1130; void* _temp1132; struct Cyc_Absyn_Exp*
_temp1134; struct Cyc_Absyn_Exp* _temp1136; struct Cyc_Absyn_Exp* _temp1138;
struct Cyc_Absyn_Exp* _temp1140; struct Cyc_Absyn_Exp* _temp1142; struct Cyc_Absyn_Exp*
_temp1144; struct Cyc_List_List* _temp1146; struct Cyc_Absyn_Exp* _temp1148;
struct Cyc_List_List* _temp1150; struct Cyc_Absyn_Exp* _temp1152; struct Cyc_Absyn_Exp*
_temp1154; struct Cyc_Absyn_Exp* _temp1156; struct Cyc_List_List* _temp1158;
struct Cyc_Absyn_Exp* _temp1160; struct Cyc_Absyn_Exp* _temp1162; void*
_temp1164; struct Cyc_Absyn_Exp* _temp1166; struct Cyc_Absyn_Exp* _temp1168;
struct Cyc_Absyn_Exp* _temp1170; void* _temp1172; struct Cyc_Absyn_Exp*
_temp1174; struct Cyc_Absyn_Exp* _temp1176; struct _tagged_string* _temp1178;
struct Cyc_Absyn_Exp* _temp1180; struct _tagged_string* _temp1182; struct Cyc_Absyn_Exp*
_temp1184; struct Cyc_Absyn_Exp* _temp1186; struct Cyc_Absyn_Exp* _temp1188;
struct Cyc_List_List* _temp1190; struct Cyc_List_List* _temp1192; struct _tuple1*
_temp1194; struct Cyc_List_List* _temp1196; struct Cyc_Absyn_Exp* _temp1198;
struct Cyc_Absyn_Exp* _temp1200; struct Cyc_Absyn_Vardecl* _temp1202; struct Cyc_Absyn_Structdecl*
_temp1204; struct Cyc_List_List* _temp1206; struct Cyc_Core_Opt* _temp1208;
struct _tuple0* _temp1210; struct Cyc_Absyn_Tunionfield* _temp1212; struct Cyc_Absyn_Tuniondecl*
_temp1214; struct Cyc_List_List* _temp1216; struct Cyc_Core_Opt* _temp1218;
struct Cyc_Core_Opt* _temp1220; struct Cyc_Absyn_Tunionfield* _temp1222; struct
Cyc_Absyn_XTuniondecl* _temp1224; struct Cyc_List_List* _temp1226; struct Cyc_Core_Opt*
_temp1228; struct Cyc_Absyn_Enumfield* _temp1230; struct Cyc_Absyn_Enumdecl*
_temp1232; struct _tuple0* _temp1234; void* _temp1236; struct Cyc_Absyn_Exp*
_temp1238; struct Cyc_List_List* _temp1240; struct Cyc_Core_Opt* _temp1242;
struct Cyc_Absyn_Stmt* _temp1244; struct Cyc_Absyn_Fndecl* _temp1246; struct Cyc_Absyn_Exp*
_temp1248; _LL1044: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Const_e_tag){
_LL1115: _temp1114=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1042)->f1;
goto _LL1045;} else{ goto _LL1046;} _LL1046: if((( struct _tunion_struct*)
_temp1042)->tag == Cyc_Absyn_Var_e_tag){ _LL1119: _temp1118=( struct _tuple0*)((
struct Cyc_Absyn_Var_e_struct*) _temp1042)->f1; goto _LL1117; _LL1117: _temp1116=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1042)->f2; goto _LL1047;} else{
goto _LL1048;} _LL1048: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL1121: _temp1120=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1042)->f1; goto _LL1049;} else{ goto _LL1050;} _LL1050: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Primop_e_tag){ _LL1125: _temp1124=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1042)->f1; goto _LL1123;
_LL1123: _temp1122=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1042)->f2; goto _LL1051;} else{ goto _LL1052;} _LL1052: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL1131:
_temp1130=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1042)->f1; goto _LL1129; _LL1129: _temp1128=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1042)->f2; goto _LL1127; _LL1127:
_temp1126=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1042)->f3; goto _LL1053;} else{ goto _LL1054;} _LL1054: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Increment_e_tag){ _LL1135:
_temp1134=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1042)->f1; goto _LL1133; _LL1133: _temp1132=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1042)->f2; goto _LL1055;} else{ goto _LL1056;} _LL1056: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Conditional_e_tag){ _LL1141:
_temp1140=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1042)->f1; goto _LL1139; _LL1139: _temp1138=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1042)->f2; goto _LL1137; _LL1137:
_temp1136=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1042)->f3; goto _LL1057;} else{ goto _LL1058;} _LL1058: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1145: _temp1144=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1042)->f1; goto
_LL1143; _LL1143: _temp1142=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1042)->f2; goto _LL1059;} else{ goto _LL1060;} _LL1060: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1149:
_temp1148=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1042)->f1; goto _LL1147; _LL1147: _temp1146=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1042)->f2; goto _LL1061;} else{
goto _LL1062;} _LL1062: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1153: _temp1152=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1042)->f1; goto _LL1151; _LL1151: _temp1150=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1042)->f2; goto _LL1063;} else{ goto
_LL1064;} _LL1064: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Throw_e_tag){
_LL1155: _temp1154=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp1042)->f1; goto _LL1065;} else{ goto _LL1066;} _LL1066: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1157:
_temp1156=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1042)->f1; goto _LL1067;} else{ goto _LL1068;} _LL1068: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1161:
_temp1160=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1042)->f1; goto _LL1159; _LL1159: _temp1158=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1042)->f2; goto _LL1069;} else{
goto _LL1070;} _LL1070: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Cast_e_tag){
_LL1165: _temp1164=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1042)->f1;
goto _LL1163; _LL1163: _temp1162=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1042)->f2; goto _LL1071;} else{ goto _LL1072;} _LL1072: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Address_e_tag){ _LL1167: _temp1166=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp1042)->f1;
goto _LL1073;} else{ goto _LL1074;} _LL1074: if((( struct _tunion_struct*)
_temp1042)->tag == Cyc_Absyn_New_e_tag){ _LL1171: _temp1170=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1042)->f1; goto _LL1169; _LL1169: _temp1168=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1042)->f2; goto
_LL1075;} else{ goto _LL1076;} _LL1076: if((( struct _tunion_struct*) _temp1042)->tag
== Cyc_Absyn_Sizeoftyp_e_tag){ _LL1173: _temp1172=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1042)->f1; goto _LL1077;} else{ goto _LL1078;} _LL1078: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL1175:
_temp1174=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1042)->f1; goto _LL1079;} else{ goto _LL1080;} _LL1080: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Deref_e_tag){ _LL1177: _temp1176=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp1042)->f1; goto
_LL1081;} else{ goto _LL1082;} _LL1082: if((( struct _tunion_struct*) _temp1042)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL1181: _temp1180=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1042)->f1; goto _LL1179; _LL1179:
_temp1178=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1042)->f2; goto _LL1083;} else{ goto _LL1084;} _LL1084: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL1185:
_temp1184=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1042)->f1; goto _LL1183; _LL1183: _temp1182=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1042)->f2; goto _LL1085;} else{
goto _LL1086;} _LL1086: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1189: _temp1188=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1042)->f1; goto _LL1187; _LL1187: _temp1186=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1042)->f2; goto _LL1087;} else{ goto
_LL1088;} _LL1088: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1191: _temp1190=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1042)->f1; goto _LL1089;} else{ goto _LL1090;} _LL1090: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1195:
_temp1194=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1042)->f1;
goto _LL1193; _LL1193: _temp1192=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1042)->f2; goto _LL1091;} else{ goto _LL1092;} _LL1092: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Array_e_tag){ _LL1197: _temp1196=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1042)->f1; goto
_LL1093;} else{ goto _LL1094;} _LL1094: if((( struct _tunion_struct*) _temp1042)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1203: _temp1202=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1042)->f1; goto _LL1201; _LL1201:
_temp1200=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1042)->f2; goto _LL1199; _LL1199: _temp1198=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1042)->f3; goto _LL1095;} else{
goto _LL1096;} _LL1096: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Struct_e_tag){
_LL1211: _temp1210=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1042)->f1; goto _LL1209; _LL1209: _temp1208=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1042)->f2; goto _LL1207; _LL1207:
_temp1206=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1042)->f3; goto _LL1205; _LL1205: _temp1204=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1042)->f4; goto _LL1097;} else{ goto
_LL1098;} _LL1098: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Tunion_e_tag){
_LL1221: _temp1220=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1042)->f1; goto _LL1219; _LL1219: _temp1218=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1042)->f2; goto _LL1217; _LL1217:
_temp1216=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1042)->f3; goto _LL1215; _LL1215: _temp1214=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1042)->f4; goto _LL1213; _LL1213:
_temp1212=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1042)->f5; goto _LL1099;} else{ goto _LL1100;} _LL1100: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1229: _temp1228=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1042)->f1; goto
_LL1227; _LL1227: _temp1226=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1042)->f2; goto _LL1225; _LL1225: _temp1224=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1042)->f3; goto _LL1223; _LL1223:
_temp1222=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1042)->f4; goto _LL1101;} else{ goto _LL1102;} _LL1102: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Enum_e_tag){ _LL1235: _temp1234=(
struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1042)->f1; goto _LL1233;
_LL1233: _temp1232=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1042)->f2; goto _LL1231; _LL1231: _temp1230=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1042)->f3; goto _LL1103;} else{ goto
_LL1104;} _LL1104: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1239: _temp1238=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1042)->f1; goto _LL1237; _LL1237: _temp1236=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1042)->f2; goto _LL1105;} else{ goto _LL1106;} _LL1106: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1243:
_temp1242=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1042)->f1; goto _LL1241; _LL1241: _temp1240=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1042)->f2; goto _LL1107;} else{
goto _LL1108;} _LL1108: if((( struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL1245: _temp1244=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1042)->f1; goto _LL1109;} else{ goto _LL1110;} _LL1110: if((( struct
_tunion_struct*) _temp1042)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1247: _temp1246=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp1042)->f1;
goto _LL1111;} else{ goto _LL1112;} _LL1112: if((( struct _tunion_struct*)
_temp1042)->tag == Cyc_Absyn_Fill_e_tag){ _LL1249: _temp1248=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp1042)->f1; goto _LL1113;} else{ goto
_LL1043;} _LL1045: s= Cyc_Absynpp_cnst2doc( _temp1114); goto _LL1043; _LL1047:
_temp1120= _temp1118; goto _LL1049; _LL1049: s= Cyc_Absynpp_qvar2doc( _temp1120);
goto _LL1043; _LL1051: s= Cyc_Absynpp_primapp2doc( myprec, _temp1124, _temp1122);
goto _LL1043; _LL1053: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1130), Cyc_PP_text(( struct _tagged_string)({ char* _temp1250=( char*)" ";
struct _tagged_string _temp1251; _temp1251.curr= _temp1250; _temp1251.base=
_temp1250; _temp1251.last_plus_one= _temp1250 + 2; _temp1251;})), _temp1128 == 0?
Cyc_PP_text(( struct _tagged_string)({ char* _temp1252=( char*)""; struct
_tagged_string _temp1253; _temp1253.curr= _temp1252; _temp1253.base= _temp1252;
_temp1253.last_plus_one= _temp1252 + 1; _temp1253;})): Cyc_Absynpp_prim2doc((
void*)({ struct Cyc_Core_Opt* _temp1254= _temp1128; if( _temp1254 == 0){ _throw(
Null_Exception);} _temp1254->v;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1255=( char*)"= "; struct _tagged_string _temp1256; _temp1256.curr=
_temp1255; _temp1256.base= _temp1255; _temp1256.last_plus_one= _temp1255 + 3;
_temp1256;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1126)); goto _LL1043;
_LL1055: { struct Cyc_PP_Doc* es= Cyc_Absynpp_exp2doc_prec( myprec, _temp1134);{
void* _temp1257= _temp1132; _LL1259: if(( int) _temp1257 == Cyc_Absyn_PreInc){
goto _LL1260;} else{ goto _LL1261;} _LL1261: if(( int) _temp1257 == Cyc_Absyn_PreDec){
goto _LL1262;} else{ goto _LL1263;} _LL1263: if(( int) _temp1257 == Cyc_Absyn_PostInc){
goto _LL1264;} else{ goto _LL1265;} _LL1265: if(( int) _temp1257 == Cyc_Absyn_PostDec){
goto _LL1266;} else{ goto _LL1258;} _LL1260: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1267=( char*)"++"; struct _tagged_string
_temp1268; _temp1268.curr= _temp1267; _temp1268.base= _temp1267; _temp1268.last_plus_one=
_temp1267 + 3; _temp1268;})), es); goto _LL1258; _LL1262: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1269=( char*)"--"; struct
_tagged_string _temp1270; _temp1270.curr= _temp1269; _temp1270.base= _temp1269;
_temp1270.last_plus_one= _temp1269 + 3; _temp1270;})), es); goto _LL1258;
_LL1264: s= Cyc_Absynpp_cat2( es, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1271=( char*)"++"; struct _tagged_string _temp1272; _temp1272.curr=
_temp1271; _temp1272.base= _temp1271; _temp1272.last_plus_one= _temp1271 + 3;
_temp1272;}))); goto _LL1258; _LL1266: s= Cyc_Absynpp_cat2( es, Cyc_PP_text((
struct _tagged_string)({ char* _temp1273=( char*)"--"; struct _tagged_string
_temp1274; _temp1274.curr= _temp1273; _temp1274.base= _temp1273; _temp1274.last_plus_one=
_temp1273 + 3; _temp1274;}))); goto _LL1258; _LL1258:;} goto _LL1043;} _LL1057:{
struct _tuple8 _temp1276=({ struct _tuple8 _temp1275; _temp1275.f1=( void*)
_temp1138->r; _temp1275.f2=( void*) _temp1136->r; _temp1275;}); _LL1278: goto
_LL1279; _LL1279: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1140), Cyc_PP_text(( struct _tagged_string)({ char* _temp1280=( char*)" ? ";
struct _tagged_string _temp1281; _temp1281.curr= _temp1280; _temp1281.base=
_temp1280; _temp1281.last_plus_one= _temp1280 + 4; _temp1281;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1138), Cyc_PP_text(( struct _tagged_string)({ char* _temp1282=( char*)" : ";
struct _tagged_string _temp1283; _temp1283.curr= _temp1282; _temp1283.base=
_temp1282; _temp1283.last_plus_one= _temp1282 + 4; _temp1283;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1136)); goto _LL1277; _LL1277:;} goto _LL1043; _LL1059: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1144), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1284=( char*)", "; struct _tagged_string _temp1285;
_temp1285.curr= _temp1284; _temp1285.base= _temp1284; _temp1285.last_plus_one=
_temp1284 + 3; _temp1285;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1142));
goto _LL1043; _LL1061: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1148), Cyc_PP_text(( struct _tagged_string)({ char* _temp1286=( char*)"(";
struct _tagged_string _temp1287; _temp1287.curr= _temp1286; _temp1287.base=
_temp1286; _temp1287.last_plus_one= _temp1286 + 2; _temp1287;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1146), Cyc_PP_text(( struct _tagged_string)({ char* _temp1288=( char*)")";
struct _tagged_string _temp1289; _temp1289.curr= _temp1288; _temp1289.base=
_temp1288; _temp1289.last_plus_one= _temp1288 + 2; _temp1289;}))); goto _LL1043;
_LL1063: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1152), Cyc_PP_text((
struct _tagged_string)({ char* _temp1290=( char*)"("; struct _tagged_string
_temp1291; _temp1291.curr= _temp1290; _temp1291.base= _temp1290; _temp1291.last_plus_one=
_temp1290 + 2; _temp1291;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1150), Cyc_PP_text((
struct _tagged_string)({ char* _temp1292=( char*)")"; struct _tagged_string
_temp1293; _temp1293.curr= _temp1292; _temp1293.base= _temp1292; _temp1293.last_plus_one=
_temp1292 + 2; _temp1293;}))); goto _LL1043; _LL1065: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1294=( char*)"throw "; struct _tagged_string
_temp1295; _temp1295.curr= _temp1294; _temp1295.base= _temp1294; _temp1295.last_plus_one=
_temp1294 + 7; _temp1295;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1154));
goto _LL1043; _LL1067: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1156); goto
_LL1043; _LL1069: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1160); goto _LL1043;
_LL1071: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1296=( char*)"("; struct _tagged_string _temp1297; _temp1297.curr=
_temp1296; _temp1297.base= _temp1296; _temp1297.last_plus_one= _temp1296 + 2;
_temp1297;})), Cyc_Absynpp_typ2doc( _temp1164), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1298=( char*)")"; struct _tagged_string _temp1299;
_temp1299.curr= _temp1298; _temp1299.base= _temp1298; _temp1299.last_plus_one=
_temp1298 + 2; _temp1299;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1162));
goto _LL1043; _LL1073: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({
char* _temp1300=( char*)"&"; struct _tagged_string _temp1301; _temp1301.curr=
_temp1300; _temp1301.base= _temp1300; _temp1301.last_plus_one= _temp1300 + 2;
_temp1301;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1166)); goto _LL1043;
_LL1075: if( _temp1170 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1302=( char*)"new "; struct _tagged_string
_temp1303; _temp1303.curr= _temp1302; _temp1303.base= _temp1302; _temp1303.last_plus_one=
_temp1302 + 5; _temp1303;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1168));}
else{ s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1304=( char*)"rnew("; struct _tagged_string _temp1305; _temp1305.curr=
_temp1304; _temp1305.base= _temp1304; _temp1305.last_plus_one= _temp1304 + 6;
_temp1305;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1306= _temp1170; if( _temp1306 == 0){ _throw( Null_Exception);} _temp1306;})),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1307=( char*)") "; struct
_tagged_string _temp1308; _temp1308.curr= _temp1307; _temp1308.base= _temp1307;
_temp1308.last_plus_one= _temp1307 + 3; _temp1308;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1168));} goto _LL1043; _LL1077: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1309=( char*)"sizeof("; struct
_tagged_string _temp1310; _temp1310.curr= _temp1309; _temp1310.base= _temp1309;
_temp1310.last_plus_one= _temp1309 + 8; _temp1310;})), Cyc_Absynpp_typ2doc(
_temp1172), Cyc_PP_text(( struct _tagged_string)({ char* _temp1311=( char*)")";
struct _tagged_string _temp1312; _temp1312.curr= _temp1311; _temp1312.base=
_temp1311; _temp1312.last_plus_one= _temp1311 + 2; _temp1312;}))); goto _LL1043;
_LL1079: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1313=( char*)"sizeof("; struct _tagged_string _temp1314; _temp1314.curr=
_temp1313; _temp1314.base= _temp1313; _temp1314.last_plus_one= _temp1313 + 8;
_temp1314;})), Cyc_Absynpp_exp2doc( _temp1174), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1315=( char*)")"; struct _tagged_string _temp1316;
_temp1316.curr= _temp1315; _temp1316.base= _temp1315; _temp1316.last_plus_one=
_temp1315 + 2; _temp1316;}))); goto _LL1043; _LL1081: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1317=( char*)"*"; struct _tagged_string
_temp1318; _temp1318.curr= _temp1317; _temp1318.base= _temp1317; _temp1318.last_plus_one=
_temp1317 + 2; _temp1318;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1176));
goto _LL1043; _LL1083: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1180), Cyc_PP_text(( struct _tagged_string)({ char* _temp1319=( char*)".";
struct _tagged_string _temp1320; _temp1320.curr= _temp1319; _temp1320.base=
_temp1319; _temp1320.last_plus_one= _temp1319 + 2; _temp1320;})), Cyc_Absynpp_textptr(
_temp1178)); goto _LL1043; _LL1085: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1184), Cyc_PP_text(( struct _tagged_string)({ char* _temp1321=(
char*)"->"; struct _tagged_string _temp1322; _temp1322.curr= _temp1321;
_temp1322.base= _temp1321; _temp1322.last_plus_one= _temp1321 + 3; _temp1322;})),
Cyc_Absynpp_textptr( _temp1182)); goto _LL1043; _LL1087: s= Cyc_Absynpp_cat4(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1188), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1323=( char*)"["; struct _tagged_string _temp1324;
_temp1324.curr= _temp1323; _temp1324.base= _temp1323; _temp1324.last_plus_one=
_temp1323 + 2; _temp1324;})), Cyc_Absynpp_exp2doc( _temp1186), Cyc_PP_text((
struct _tagged_string)({ char* _temp1325=( char*)"]"; struct _tagged_string
_temp1326; _temp1326.curr= _temp1325; _temp1326.base= _temp1325; _temp1326.last_plus_one=
_temp1325 + 2; _temp1326;}))); goto _LL1043; _LL1089: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1327=( char*)"$("; struct _tagged_string
_temp1328; _temp1328.curr= _temp1327; _temp1328.base= _temp1327; _temp1328.last_plus_one=
_temp1327 + 3; _temp1328;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1190), Cyc_PP_text((
struct _tagged_string)({ char* _temp1329=( char*)")"; struct _tagged_string
_temp1330; _temp1330.curr= _temp1329; _temp1330.base= _temp1329; _temp1330.last_plus_one=
_temp1329 + 2; _temp1330;}))); goto _LL1043; _LL1091: s= Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1331=( char*)"("; struct _tagged_string
_temp1332; _temp1332.curr= _temp1331; _temp1332.base= _temp1331; _temp1332.last_plus_one=
_temp1331 + 2; _temp1332;})), Cyc_Absynpp_typ2doc((* _temp1194).f3), Cyc_PP_text((
struct _tagged_string)({ char* _temp1333=( char*)")"; struct _tagged_string
_temp1334; _temp1334.curr= _temp1333; _temp1334.base= _temp1333; _temp1334.last_plus_one=
_temp1333 + 2; _temp1334;})), Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp1335=( char*)"{"; struct _tagged_string _temp1336; _temp1336.curr=
_temp1335; _temp1336.base= _temp1335; _temp1336.last_plus_one= _temp1335 + 2;
_temp1336;}),( struct _tagged_string)({ char* _temp1337=( char*)"}"; struct
_tagged_string _temp1338; _temp1338.curr= _temp1337; _temp1338.base= _temp1337;
_temp1338.last_plus_one= _temp1337 + 2; _temp1338;}),( struct _tagged_string)({
char* _temp1339=( char*)","; struct _tagged_string _temp1340; _temp1340.curr=
_temp1339; _temp1340.base= _temp1339; _temp1340.last_plus_one= _temp1339 + 2;
_temp1340;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1192))); goto _LL1043; _LL1093: s= Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1341=( char*)"{"; struct _tagged_string _temp1342;
_temp1342.curr= _temp1341; _temp1342.base= _temp1341; _temp1342.last_plus_one=
_temp1341 + 2; _temp1342;}),( struct _tagged_string)({ char* _temp1343=( char*)"}";
struct _tagged_string _temp1344; _temp1344.curr= _temp1343; _temp1344.base=
_temp1343; _temp1344.last_plus_one= _temp1343 + 2; _temp1344;}),( struct
_tagged_string)({ char* _temp1345=( char*)","; struct _tagged_string _temp1346;
_temp1346.curr= _temp1345; _temp1346.base= _temp1345; _temp1346.last_plus_one=
_temp1345 + 2; _temp1346;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1196)); goto _LL1043; _LL1095: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1347=( char*)"{for "; struct _tagged_string
_temp1348; _temp1348.curr= _temp1347; _temp1348.base= _temp1347; _temp1348.last_plus_one=
_temp1347 + 6; _temp1348;})), Cyc_PP_text(*(* _temp1202->name).f2), Cyc_PP_text((
struct _tagged_string)({ char* _temp1349=( char*)" < "; struct _tagged_string
_temp1350; _temp1350.curr= _temp1349; _temp1350.base= _temp1349; _temp1350.last_plus_one=
_temp1349 + 4; _temp1350;})), Cyc_Absynpp_exp2doc( _temp1200), Cyc_PP_text((
struct _tagged_string)({ char* _temp1351=( char*)" : "; struct _tagged_string
_temp1352; _temp1352.curr= _temp1351; _temp1352.base= _temp1351; _temp1352.last_plus_one=
_temp1351 + 4; _temp1352;})), Cyc_Absynpp_exp2doc( _temp1198), Cyc_PP_text((
struct _tagged_string)({ char* _temp1353=( char*)"}"; struct _tagged_string
_temp1354; _temp1354.curr= _temp1353; _temp1354.base= _temp1353; _temp1354.last_plus_one=
_temp1353 + 2; _temp1354;}))); goto _LL1043; _LL1097: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1210), Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1355=( char*)"{";
struct _tagged_string _temp1356; _temp1356.curr= _temp1355; _temp1356.base=
_temp1355; _temp1356.last_plus_one= _temp1355 + 2; _temp1356;}),( struct
_tagged_string)({ char* _temp1357=( char*)"}"; struct _tagged_string _temp1358;
_temp1358.curr= _temp1357; _temp1358.base= _temp1357; _temp1358.last_plus_one=
_temp1357 + 2; _temp1358;}),( struct _tagged_string)({ char* _temp1359=( char*)",";
struct _tagged_string _temp1360; _temp1360.curr= _temp1359; _temp1360.base=
_temp1359; _temp1360.last_plus_one= _temp1359 + 2; _temp1360;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1206))); goto _LL1043; _LL1099: _temp1226= _temp1216;
_temp1222= _temp1212; goto _LL1101; _LL1101: if( _temp1226 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1222->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1222->name),
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1361=( char*)"(";
struct _tagged_string _temp1362; _temp1362.curr= _temp1361; _temp1362.base=
_temp1361; _temp1362.last_plus_one= _temp1361 + 2; _temp1362;}),( struct
_tagged_string)({ char* _temp1363=( char*)")"; struct _tagged_string _temp1364;
_temp1364.curr= _temp1363; _temp1364.base= _temp1363; _temp1364.last_plus_one=
_temp1363 + 2; _temp1364;}),( struct _tagged_string)({ char* _temp1365=( char*)",";
struct _tagged_string _temp1366; _temp1366.curr= _temp1365; _temp1366.base=
_temp1365; _temp1366.last_plus_one= _temp1365 + 2; _temp1366;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp1226)));} goto _LL1043; _LL1103: s= Cyc_Absynpp_qvar2doc(
_temp1234); goto _LL1043; _LL1105: if( _temp1238 == 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1367=( char*)"malloc(sizeof("; struct
_tagged_string _temp1368; _temp1368.curr= _temp1367; _temp1368.base= _temp1367;
_temp1368.last_plus_one= _temp1367 + 15; _temp1368;})), Cyc_Absynpp_typ2doc(
_temp1236), Cyc_PP_text(( struct _tagged_string)({ char* _temp1369=( char*)"))";
struct _tagged_string _temp1370; _temp1370.curr= _temp1369; _temp1370.base=
_temp1369; _temp1370.last_plus_one= _temp1369 + 3; _temp1370;})));} else{ s= Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1371=( char*)"rmalloc(";
struct _tagged_string _temp1372; _temp1372.curr= _temp1371; _temp1372.base=
_temp1371; _temp1372.last_plus_one= _temp1371 + 9; _temp1372;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1373= _temp1238; if(
_temp1373 == 0){ _throw( Null_Exception);} _temp1373;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1374=( char*)","; struct _tagged_string _temp1375;
_temp1375.curr= _temp1374; _temp1375.base= _temp1374; _temp1375.last_plus_one=
_temp1374 + 2; _temp1375;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1376=( char*)"sizeof("; struct _tagged_string _temp1377; _temp1377.curr=
_temp1376; _temp1377.base= _temp1376; _temp1377.last_plus_one= _temp1376 + 8;
_temp1377;})), Cyc_Absynpp_typ2doc( _temp1236), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1378=( char*)"))"; struct _tagged_string _temp1379;
_temp1379.curr= _temp1378; _temp1379.base= _temp1378; _temp1379.last_plus_one=
_temp1378 + 3; _temp1379;})));} goto _LL1043; _LL1107: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1380=( char*)"{"; struct _tagged_string
_temp1381; _temp1381.curr= _temp1380; _temp1381.base= _temp1380; _temp1381.last_plus_one=
_temp1380 + 2; _temp1381;}),( struct _tagged_string)({ char* _temp1382=( char*)"}";
struct _tagged_string _temp1383; _temp1383.curr= _temp1382; _temp1383.base=
_temp1382; _temp1383.last_plus_one= _temp1382 + 2; _temp1383;}),( struct
_tagged_string)({ char* _temp1384=( char*)","; struct _tagged_string _temp1385;
_temp1385.curr= _temp1384; _temp1385.base= _temp1384; _temp1385.last_plus_one=
_temp1384 + 2; _temp1385;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1240)); goto _LL1043; _LL1109: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1386=( char*)"({ "; struct _tagged_string _temp1387;
_temp1387.curr= _temp1386; _temp1387.base= _temp1386; _temp1387.last_plus_one=
_temp1386 + 4; _temp1387;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1244)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1388=( char*)" })"; struct
_tagged_string _temp1389; _temp1389.curr= _temp1388; _temp1389.base= _temp1388;
_temp1389.last_plus_one= _temp1388 + 4; _temp1389;}))); goto _LL1043; _LL1111: s=
Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1390=( char*)"codegen(";
struct _tagged_string _temp1391; _temp1391.curr= _temp1390; _temp1391.base=
_temp1390; _temp1391.last_plus_one= _temp1390 + 9; _temp1391;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1392=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1392->r=( void*)(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp1393=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1393[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1394; _temp1394.tag= Cyc_Absyn_Fn_d_tag; _temp1394.f1= _temp1246; _temp1394;});
_temp1393;})); _temp1392->loc= e->loc; _temp1392;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1395=( char*)")"; struct _tagged_string _temp1396;
_temp1396.curr= _temp1395; _temp1396.base= _temp1395; _temp1396.last_plus_one=
_temp1395 + 2; _temp1396;}))); goto _LL1043; _LL1113: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1397=( char*)"fill("; struct _tagged_string
_temp1398; _temp1398.curr= _temp1397; _temp1398.base= _temp1397; _temp1398.last_plus_one=
_temp1397 + 6; _temp1398;})), Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1248)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1399=( char*)")"; struct
_tagged_string _temp1400; _temp1400.curr= _temp1399; _temp1400.base= _temp1399;
_temp1400.last_plus_one= _temp1399 + 2; _temp1400;}))); goto _LL1043; _LL1043:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp1401=( char*)"("; struct _tagged_string _temp1402; _temp1402.curr=
_temp1401; _temp1402.base= _temp1401; _temp1402.last_plus_one= _temp1401 + 2;
_temp1402;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp1403=( char*)")";
struct _tagged_string _temp1404; _temp1404.curr= _temp1403; _temp1404.base=
_temp1403; _temp1404.last_plus_one= _temp1403 + 2; _temp1404;})));} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1405= d;
struct Cyc_Absyn_Exp* _temp1411; struct _tagged_string* _temp1413; _LL1407: if(((
struct _tunion_struct*) _temp1405)->tag == Cyc_Absyn_ArrayElement_tag){ _LL1412:
_temp1411=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1405)->f1; goto _LL1408;} else{ goto _LL1409;} _LL1409: if((( struct
_tunion_struct*) _temp1405)->tag == Cyc_Absyn_FieldName_tag){ _LL1414: _temp1413=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1405)->f1;
goto _LL1410;} else{ goto _LL1406;} _LL1408: return Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1415=( char*)".["; struct _tagged_string
_temp1416; _temp1416.curr= _temp1415; _temp1416.base= _temp1415; _temp1416.last_plus_one=
_temp1415 + 3; _temp1416;})), Cyc_Absynpp_exp2doc( _temp1411), Cyc_PP_text((
struct _tagged_string)({ char* _temp1417=( char*)"]"; struct _tagged_string
_temp1418; _temp1418.curr= _temp1417; _temp1418.base= _temp1417; _temp1418.last_plus_one=
_temp1417 + 2; _temp1418;}))); _LL1410: return Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1419=( char*)"."; struct _tagged_string
_temp1420; _temp1420.curr= _temp1419; _temp1420.base= _temp1419; _temp1420.last_plus_one=
_temp1419 + 2; _temp1420;})), Cyc_Absynpp_textptr( _temp1413)); _LL1406:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 == 0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp1421=( char*)""; struct _tagged_string
_temp1422; _temp1422.curr= _temp1421; _temp1422.base= _temp1421; _temp1422.last_plus_one=
_temp1421 + 1; _temp1422;}),( struct _tagged_string)({ char* _temp1423=( char*)"=";
struct _tagged_string _temp1424; _temp1424.curr= _temp1423; _temp1424.base=
_temp1423; _temp1424.last_plus_one= _temp1423 + 2; _temp1424;}),( struct
_tagged_string)({ char* _temp1425=( char*)"="; struct _tagged_string _temp1426;
_temp1426.curr= _temp1425; _temp1426.base= _temp1425; _temp1426.last_plus_one=
_temp1425 + 2; _temp1426;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)), Cyc_Absynpp_exp2doc((* de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1427=( char*)""; struct _tagged_string _temp1428;
_temp1428.curr= _temp1427; _temp1428.base= _temp1427; _temp1428.last_plus_one=
_temp1427 + 1; _temp1428;}),( struct _tagged_string)({ char* _temp1429=( char*)"";
struct _tagged_string _temp1430; _temp1430.curr= _temp1429; _temp1430.base=
_temp1429; _temp1430.last_plus_one= _temp1429 + 1; _temp1430;}),( struct
_tagged_string)({ char* _temp1431=( char*)","; struct _tagged_string _temp1432;
_temp1432.curr= _temp1431; _temp1432.base= _temp1431; _temp1432.last_plus_one=
_temp1431 + 2; _temp1432;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1433= c; char _temp1451; void* _temp1453; short _temp1455;
void* _temp1457; int _temp1459; void* _temp1461; int _temp1463; void* _temp1465;
long long _temp1467; void* _temp1469; struct _tagged_string _temp1471; struct
_tagged_string _temp1473; _LL1435: if(( unsigned int) _temp1433 > 1u?(( struct
_tunion_struct*) _temp1433)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL1454: _temp1453=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp1433)->f1; goto _LL1452; _LL1452:
_temp1451=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp1433)->f2; goto
_LL1436;} else{ goto _LL1437;} _LL1437: if(( unsigned int) _temp1433 > 1u?((
struct _tunion_struct*) _temp1433)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL1458:
_temp1457=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1433)->f1; goto
_LL1456; _LL1456: _temp1455=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp1433)->f2; goto _LL1438;} else{ goto _LL1439;} _LL1439: if(( unsigned int)
_temp1433 > 1u?(( struct _tunion_struct*) _temp1433)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL1462: _temp1461=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1433)->f1;
if(( int) _temp1461 == Cyc_Absyn_Signed){ goto _LL1460;} else{ goto _LL1441;}
_LL1460: _temp1459=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1433)->f2; goto
_LL1440;} else{ goto _LL1441;} _LL1441: if(( unsigned int) _temp1433 > 1u?((
struct _tunion_struct*) _temp1433)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL1466:
_temp1465=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1433)->f1; if(( int)
_temp1465 == Cyc_Absyn_Unsigned){ goto _LL1464;} else{ goto _LL1443;} _LL1464:
_temp1463=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1433)->f2; goto _LL1442;}
else{ goto _LL1443;} _LL1443: if(( unsigned int) _temp1433 > 1u?(( struct
_tunion_struct*) _temp1433)->tag == Cyc_Absyn_LongLong_c_tag: 0){ _LL1470:
_temp1469=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1433)->f1; goto
_LL1468; _LL1468: _temp1467=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1433)->f2; goto _LL1444;} else{ goto _LL1445;} _LL1445: if(( unsigned int)
_temp1433 > 1u?(( struct _tunion_struct*) _temp1433)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL1472: _temp1471=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp1433)->f1; goto _LL1446;} else{ goto _LL1447;} _LL1447: if(( int) _temp1433
== Cyc_Absyn_Null_c){ goto _LL1448;} else{ goto _LL1449;} _LL1449: if((
unsigned int) _temp1433 > 1u?(( struct _tunion_struct*) _temp1433)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1474: _temp1473=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1433)->f1; goto _LL1450;} else{ goto _LL1434;} _LL1436: return Cyc_PP_text(({
struct _tagged_string _temp1475= Cyc_Absynpp_char_escape( _temp1451); xprintf("'%.*s'",
_temp1475.last_plus_one - _temp1475.curr, _temp1475.curr);})); _LL1438: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1455)); _LL1440: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1459)); _LL1442: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1463)); _LL1444: return Cyc_PP_text(( struct _tagged_string)({
char* _temp1476=( char*)"<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp1477; _temp1477.curr= _temp1476; _temp1477.base= _temp1476; _temp1477.last_plus_one=
_temp1476 + 27; _temp1477;})); _LL1446: return Cyc_PP_text( _temp1471); _LL1448:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp1478=( char*)"null";
struct _tagged_string _temp1479; _temp1479.curr= _temp1478; _temp1479.base=
_temp1478; _temp1479.last_plus_one= _temp1478 + 5; _temp1479;})); _LL1450:
return Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1480=(
char*)"\""; struct _tagged_string _temp1481; _temp1481.curr= _temp1480;
_temp1481.base= _temp1480; _temp1481.last_plus_one= _temp1480 + 2; _temp1481;})),
Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1473)), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1482=( char*)"\""; struct _tagged_string _temp1483;
_temp1483.curr= _temp1482; _temp1483.base= _temp1482; _temp1483.last_plus_one=
_temp1482 + 2; _temp1483;}))); _LL1434:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:({ struct Cyc_List_List*
_temp1484= es; if( _temp1484 == 0){ _throw( Null_Exception);} _temp1484->tl;})
!= 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct*
_temp1485=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1485[ 0]=({ struct Cyc_Core_Failure_struct _temp1486; _temp1486.tag= Cyc_Core_Failure_tag;
_temp1486.f1=({ struct _tagged_string _temp1487= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1487.last_plus_one
- _temp1487.curr, _temp1487.curr);}); _temp1486;}); _temp1485;}));}{ struct Cyc_PP_Doc*
d= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1490= es; if( _temp1490 == 0){ _throw( Null_Exception);} _temp1490->hd;}));
return Cyc_Absynpp_cat2( d, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1488=( char*)".size"; struct _tagged_string _temp1489; _temp1489.curr=
_temp1488; _temp1489.base= _temp1488; _temp1489.last_plus_one= _temp1488 + 6;
_temp1489;})));}} else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct Cyc_List_List*
ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct*
_temp1491=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1491[ 0]=({ struct Cyc_Core_Failure_struct _temp1492; _temp1492.tag= Cyc_Core_Failure_tag;
_temp1492.f1=({ struct _tagged_string _temp1493= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with no args", _temp1493.last_plus_one -
_temp1493.curr, _temp1493.curr);}); _temp1492;}); _temp1491;}));} else{ if(({
struct Cyc_List_List* _temp1494= ds; if( _temp1494 == 0){ _throw( Null_Exception);}
_temp1494->tl;}) == 0){ return Cyc_Absynpp_cat3( ps, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1495=( char*)" "; struct _tagged_string _temp1496;
_temp1496.curr= _temp1495; _temp1496.base= _temp1495; _temp1496.last_plus_one=
_temp1495 + 2; _temp1496;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp1497= ds; if( _temp1497 == 0){ _throw( Null_Exception);} _temp1497->hd;}));}
else{ if(({ struct Cyc_List_List* _temp1499=({ struct Cyc_List_List* _temp1498=
ds; if( _temp1498 == 0){ _throw( Null_Exception);} _temp1498->tl;}); if(
_temp1499 == 0){ _throw( Null_Exception);} _temp1499->tl;}) != 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp1500=( struct
Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1500[ 0]=({ struct Cyc_Core_Failure_struct _temp1501; _temp1501.tag= Cyc_Core_Failure_tag;
_temp1501.f1=({ struct _tagged_string _temp1502= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with more than 2 args", _temp1502.last_plus_one
- _temp1502.curr, _temp1502.curr);}); _temp1501;}); _temp1500;}));} else{ return
Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp1503= ds;
if( _temp1503 == 0){ _throw( Null_Exception);} _temp1503->hd;}), Cyc_PP_text((
struct _tagged_string)({ char* _temp1504=( char*)" "; struct _tagged_string
_temp1505; _temp1505.curr= _temp1504; _temp1505.base= _temp1504; _temp1505.last_plus_one=
_temp1504 + 2; _temp1505;})), ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1506=( char*)" "; struct _tagged_string _temp1507; _temp1507.curr=
_temp1506; _temp1507.base= _temp1506; _temp1507.last_plus_one= _temp1506 + 2;
_temp1507;})),( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp1509=({ struct
Cyc_List_List* _temp1508= ds; if( _temp1508 == 0){ _throw( Null_Exception);}
_temp1508->tl;}); if( _temp1509 == 0){ _throw( Null_Exception);} _temp1509->hd;}));}}}}
else{ return Cyc_Absynpp_cat4( ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1510=( char*)"("; struct _tagged_string _temp1511; _temp1511.curr=
_temp1510; _temp1511.base= _temp1510; _temp1511.last_plus_one= _temp1510 + 2;
_temp1511;})), Cyc_Absynpp_exps2doc_prec( 20, es), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1512=( char*)")"; struct _tagged_string _temp1513;
_temp1513.curr= _temp1512; _temp1513.base= _temp1512; _temp1513.last_plus_one=
_temp1512 + 2; _temp1513;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1514= p; _LL1516: if(( int) _temp1514 == Cyc_Absyn_Plus){
goto _LL1517;} else{ goto _LL1518;} _LL1518: if(( int) _temp1514 == Cyc_Absyn_Times){
goto _LL1519;} else{ goto _LL1520;} _LL1520: if(( int) _temp1514 == Cyc_Absyn_Minus){
goto _LL1521;} else{ goto _LL1522;} _LL1522: if(( int) _temp1514 == Cyc_Absyn_Div){
goto _LL1523;} else{ goto _LL1524;} _LL1524: if(( int) _temp1514 == Cyc_Absyn_Mod){
goto _LL1525;} else{ goto _LL1526;} _LL1526: if(( int) _temp1514 == Cyc_Absyn_Eq){
goto _LL1527;} else{ goto _LL1528;} _LL1528: if(( int) _temp1514 == Cyc_Absyn_Neq){
goto _LL1529;} else{ goto _LL1530;} _LL1530: if(( int) _temp1514 == Cyc_Absyn_Gt){
goto _LL1531;} else{ goto _LL1532;} _LL1532: if(( int) _temp1514 == Cyc_Absyn_Lt){
goto _LL1533;} else{ goto _LL1534;} _LL1534: if(( int) _temp1514 == Cyc_Absyn_Gte){
goto _LL1535;} else{ goto _LL1536;} _LL1536: if(( int) _temp1514 == Cyc_Absyn_Lte){
goto _LL1537;} else{ goto _LL1538;} _LL1538: if(( int) _temp1514 == Cyc_Absyn_Not){
goto _LL1539;} else{ goto _LL1540;} _LL1540: if(( int) _temp1514 == Cyc_Absyn_Bitnot){
goto _LL1541;} else{ goto _LL1542;} _LL1542: if(( int) _temp1514 == Cyc_Absyn_Bitand){
goto _LL1543;} else{ goto _LL1544;} _LL1544: if(( int) _temp1514 == Cyc_Absyn_Bitor){
goto _LL1545;} else{ goto _LL1546;} _LL1546: if(( int) _temp1514 == Cyc_Absyn_Bitxor){
goto _LL1547;} else{ goto _LL1548;} _LL1548: if(( int) _temp1514 == Cyc_Absyn_Bitlshift){
goto _LL1549;} else{ goto _LL1550;} _LL1550: if(( int) _temp1514 == Cyc_Absyn_Bitlrshift){
goto _LL1551;} else{ goto _LL1552;} _LL1552: if(( int) _temp1514 == Cyc_Absyn_Bitarshift){
goto _LL1553;} else{ goto _LL1554;} _LL1554: if(( int) _temp1514 == Cyc_Absyn_Size){
goto _LL1555;} else{ goto _LL1556;} _LL1556: if(( int) _temp1514 == Cyc_Absyn_Printf){
goto _LL1557;} else{ goto _LL1558;} _LL1558: if(( int) _temp1514 == Cyc_Absyn_Fprintf){
goto _LL1559;} else{ goto _LL1560;} _LL1560: if(( int) _temp1514 == Cyc_Absyn_Xprintf){
goto _LL1561;} else{ goto _LL1562;} _LL1562: if(( int) _temp1514 == Cyc_Absyn_Scanf){
goto _LL1563;} else{ goto _LL1564;} _LL1564: if(( int) _temp1514 == Cyc_Absyn_Fscanf){
goto _LL1565;} else{ goto _LL1566;} _LL1566: if(( int) _temp1514 == Cyc_Absyn_Sscanf){
goto _LL1567;} else{ goto _LL1515;} _LL1517: return( struct _tagged_string)({
char* _temp1568=( char*)"+"; struct _tagged_string _temp1569; _temp1569.curr=
_temp1568; _temp1569.base= _temp1568; _temp1569.last_plus_one= _temp1568 + 2;
_temp1569;}); _LL1519: return( struct _tagged_string)({ char* _temp1570=( char*)"*";
struct _tagged_string _temp1571; _temp1571.curr= _temp1570; _temp1571.base=
_temp1570; _temp1571.last_plus_one= _temp1570 + 2; _temp1571;}); _LL1521: return(
struct _tagged_string)({ char* _temp1572=( char*)"-"; struct _tagged_string
_temp1573; _temp1573.curr= _temp1572; _temp1573.base= _temp1572; _temp1573.last_plus_one=
_temp1572 + 2; _temp1573;}); _LL1523: return( struct _tagged_string)({ char*
_temp1574=( char*)"/"; struct _tagged_string _temp1575; _temp1575.curr=
_temp1574; _temp1575.base= _temp1574; _temp1575.last_plus_one= _temp1574 + 2;
_temp1575;}); _LL1525: return( struct _tagged_string)({ char* _temp1576=( char*)"%";
struct _tagged_string _temp1577; _temp1577.curr= _temp1576; _temp1577.base=
_temp1576; _temp1577.last_plus_one= _temp1576 + 2; _temp1577;}); _LL1527: return(
struct _tagged_string)({ char* _temp1578=( char*)"=="; struct _tagged_string
_temp1579; _temp1579.curr= _temp1578; _temp1579.base= _temp1578; _temp1579.last_plus_one=
_temp1578 + 3; _temp1579;}); _LL1529: return( struct _tagged_string)({ char*
_temp1580=( char*)"!="; struct _tagged_string _temp1581; _temp1581.curr=
_temp1580; _temp1581.base= _temp1580; _temp1581.last_plus_one= _temp1580 + 3;
_temp1581;}); _LL1531: return( struct _tagged_string)({ char* _temp1582=( char*)">";
struct _tagged_string _temp1583; _temp1583.curr= _temp1582; _temp1583.base=
_temp1582; _temp1583.last_plus_one= _temp1582 + 2; _temp1583;}); _LL1533: return(
struct _tagged_string)({ char* _temp1584=( char*)"<"; struct _tagged_string
_temp1585; _temp1585.curr= _temp1584; _temp1585.base= _temp1584; _temp1585.last_plus_one=
_temp1584 + 2; _temp1585;}); _LL1535: return( struct _tagged_string)({ char*
_temp1586=( char*)">="; struct _tagged_string _temp1587; _temp1587.curr=
_temp1586; _temp1587.base= _temp1586; _temp1587.last_plus_one= _temp1586 + 3;
_temp1587;}); _LL1537: return( struct _tagged_string)({ char* _temp1588=( char*)"<=";
struct _tagged_string _temp1589; _temp1589.curr= _temp1588; _temp1589.base=
_temp1588; _temp1589.last_plus_one= _temp1588 + 3; _temp1589;}); _LL1539: return(
struct _tagged_string)({ char* _temp1590=( char*)"!"; struct _tagged_string
_temp1591; _temp1591.curr= _temp1590; _temp1591.base= _temp1590; _temp1591.last_plus_one=
_temp1590 + 2; _temp1591;}); _LL1541: return( struct _tagged_string)({ char*
_temp1592=( char*)"~"; struct _tagged_string _temp1593; _temp1593.curr=
_temp1592; _temp1593.base= _temp1592; _temp1593.last_plus_one= _temp1592 + 2;
_temp1593;}); _LL1543: return( struct _tagged_string)({ char* _temp1594=( char*)"&";
struct _tagged_string _temp1595; _temp1595.curr= _temp1594; _temp1595.base=
_temp1594; _temp1595.last_plus_one= _temp1594 + 2; _temp1595;}); _LL1545: return(
struct _tagged_string)({ char* _temp1596=( char*)"|"; struct _tagged_string
_temp1597; _temp1597.curr= _temp1596; _temp1597.base= _temp1596; _temp1597.last_plus_one=
_temp1596 + 2; _temp1597;}); _LL1547: return( struct _tagged_string)({ char*
_temp1598=( char*)"^"; struct _tagged_string _temp1599; _temp1599.curr=
_temp1598; _temp1599.base= _temp1598; _temp1599.last_plus_one= _temp1598 + 2;
_temp1599;}); _LL1549: return( struct _tagged_string)({ char* _temp1600=( char*)"<<";
struct _tagged_string _temp1601; _temp1601.curr= _temp1600; _temp1601.base=
_temp1600; _temp1601.last_plus_one= _temp1600 + 3; _temp1601;}); _LL1551: return(
struct _tagged_string)({ char* _temp1602=( char*)">>"; struct _tagged_string
_temp1603; _temp1603.curr= _temp1602; _temp1603.base= _temp1602; _temp1603.last_plus_one=
_temp1602 + 3; _temp1603;}); _LL1553: return( struct _tagged_string)({ char*
_temp1604=( char*)">>>"; struct _tagged_string _temp1605; _temp1605.curr=
_temp1604; _temp1605.base= _temp1604; _temp1605.last_plus_one= _temp1604 + 4;
_temp1605;}); _LL1555: return( struct _tagged_string)({ char* _temp1606=( char*)"size";
struct _tagged_string _temp1607; _temp1607.curr= _temp1606; _temp1607.base=
_temp1606; _temp1607.last_plus_one= _temp1606 + 5; _temp1607;}); _LL1557: return(
struct _tagged_string)({ char* _temp1608=( char*)"printf"; struct _tagged_string
_temp1609; _temp1609.curr= _temp1608; _temp1609.base= _temp1608; _temp1609.last_plus_one=
_temp1608 + 7; _temp1609;}); _LL1559: return( struct _tagged_string)({ char*
_temp1610=( char*)"fprintf"; struct _tagged_string _temp1611; _temp1611.curr=
_temp1610; _temp1611.base= _temp1610; _temp1611.last_plus_one= _temp1610 + 8;
_temp1611;}); _LL1561: return( struct _tagged_string)({ char* _temp1612=( char*)"xprintf";
struct _tagged_string _temp1613; _temp1613.curr= _temp1612; _temp1613.base=
_temp1612; _temp1613.last_plus_one= _temp1612 + 8; _temp1613;}); _LL1563: return(
struct _tagged_string)({ char* _temp1614=( char*)"scanf"; struct _tagged_string
_temp1615; _temp1615.curr= _temp1614; _temp1615.base= _temp1614; _temp1615.last_plus_one=
_temp1614 + 6; _temp1615;}); _LL1565: return( struct _tagged_string)({ char*
_temp1616=( char*)"fscanf"; struct _tagged_string _temp1617; _temp1617.curr=
_temp1616; _temp1617.base= _temp1616; _temp1617.last_plus_one= _temp1616 + 7;
_temp1617;}); _LL1567: return( struct _tagged_string)({ char* _temp1618=( char*)"sscanf";
struct _tagged_string _temp1619; _temp1619.curr= _temp1618; _temp1619.base=
_temp1618; _temp1619.last_plus_one= _temp1618 + 7; _temp1619;}); _LL1515:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 ==
0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp1620=(* t).f1; if( _temp1620
== 0){ _throw( Null_Exception);} _temp1620->v;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1621=( char*)" = "; struct _tagged_string _temp1622;
_temp1622.curr= _temp1621; _temp1622.base= _temp1621; _temp1622.last_plus_one=
_temp1621 + 4; _temp1622;})), Cyc_Absynpp_exp2doc((* t).f2));}} struct Cyc_PP_Doc*
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
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1623=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1629; struct Cyc_Absyn_Decl* _temp1631;
_LL1625: if(( unsigned int) _temp1623 > 1u?(( struct _tunion_struct*) _temp1623)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL1632: _temp1631=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1623)->f1; goto _LL1630; _LL1630:
_temp1629=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1623)->f2;
goto _LL1626;} else{ goto _LL1627;} _LL1627: goto _LL1628; _LL1626: return 1;
_LL1628: return 0; _LL1624:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1633=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1675; struct Cyc_Absyn_Stmt* _temp1677; struct Cyc_Absyn_Stmt*
_temp1679; struct Cyc_Absyn_Exp* _temp1681; struct Cyc_Absyn_Stmt* _temp1683;
struct Cyc_Absyn_Stmt* _temp1685; struct Cyc_Absyn_Exp* _temp1687; struct Cyc_Absyn_Stmt*
_temp1689; struct _tuple2 _temp1691; struct Cyc_Absyn_Stmt* _temp1693; struct
Cyc_Absyn_Exp* _temp1695; struct Cyc_Absyn_Stmt* _temp1697; struct Cyc_Absyn_Stmt*
_temp1699; struct Cyc_Absyn_Stmt* _temp1701; struct _tagged_string* _temp1703;
struct Cyc_Absyn_Stmt* _temp1705; struct _tuple2 _temp1707; struct Cyc_Absyn_Stmt*
_temp1709; struct Cyc_Absyn_Exp* _temp1711; struct _tuple2 _temp1713; struct Cyc_Absyn_Stmt*
_temp1715; struct Cyc_Absyn_Exp* _temp1717; struct Cyc_Absyn_Exp* _temp1719;
struct Cyc_List_List* _temp1721; struct Cyc_Absyn_Exp* _temp1723; struct Cyc_Absyn_Switch_clause**
_temp1725; struct Cyc_List_List* _temp1727; struct Cyc_Absyn_Switch_clause**
_temp1729; struct Cyc_List_List* _temp1731; struct Cyc_Absyn_Stmt* _temp1733;
struct Cyc_Absyn_Decl* _temp1735; struct Cyc_Absyn_Stmt* _temp1737; struct Cyc_Absyn_Stmt*
_temp1739; struct Cyc_Absyn_Stmt* _temp1741; struct _tagged_string* _temp1743;
struct _tuple2 _temp1745; struct Cyc_Absyn_Stmt* _temp1747; struct Cyc_Absyn_Exp*
_temp1749; struct Cyc_Absyn_Stmt* _temp1751; struct Cyc_List_List* _temp1753;
struct Cyc_Absyn_Stmt* _temp1755; struct Cyc_Absyn_Stmt* _temp1757; struct Cyc_Absyn_Vardecl*
_temp1759; struct Cyc_Absyn_Tvar* _temp1761; _LL1635: if(( int) _temp1633 == Cyc_Absyn_Skip_s){
goto _LL1636;} else{ goto _LL1637;} _LL1637: if(( unsigned int) _temp1633 > 1u?((
struct _tunion_struct*) _temp1633)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1676:
_temp1675=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1633)->f1;
goto _LL1638;} else{ goto _LL1639;} _LL1639: if(( unsigned int) _temp1633 > 1u?((
struct _tunion_struct*) _temp1633)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1680:
_temp1679=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1633)->f1;
goto _LL1678; _LL1678: _temp1677=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1633)->f2; goto _LL1640;} else{ goto _LL1641;} _LL1641: if(( unsigned int)
_temp1633 > 1u?(( struct _tunion_struct*) _temp1633)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1682: _temp1681=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1633)->f1; goto _LL1642;} else{ goto _LL1643;} _LL1643: if(( unsigned int)
_temp1633 > 1u?(( struct _tunion_struct*) _temp1633)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1688: _temp1687=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1633)->f1; goto _LL1686; _LL1686: _temp1685=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1633)->f2; goto _LL1684; _LL1684:
_temp1683=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1633)->f3; goto _LL1644;} else{ goto _LL1645;} _LL1645: if(( unsigned int)
_temp1633 > 1u?(( struct _tunion_struct*) _temp1633)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1692: _temp1691=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*)
_temp1633)->f1; _LL1696: _temp1695= _temp1691.f1; goto _LL1694; _LL1694:
_temp1693= _temp1691.f2; goto _LL1690; _LL1690: _temp1689=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1633)->f2; goto _LL1646;} else{ goto
_LL1647;} _LL1647: if(( unsigned int) _temp1633 > 1u?(( struct _tunion_struct*)
_temp1633)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1698: _temp1697=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1633)->f1; goto _LL1648;} else{ goto
_LL1649;} _LL1649: if(( unsigned int) _temp1633 > 1u?(( struct _tunion_struct*)
_temp1633)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1700: _temp1699=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1633)->f1; goto _LL1650;} else{ goto
_LL1651;} _LL1651: if(( unsigned int) _temp1633 > 1u?(( struct _tunion_struct*)
_temp1633)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1704: _temp1703=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1633)->f1; goto _LL1702;
_LL1702: _temp1701=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1633)->f2; goto _LL1652;} else{ goto _LL1653;} _LL1653: if(( unsigned int)
_temp1633 > 1u?(( struct _tunion_struct*) _temp1633)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1720: _temp1719=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1633)->f1; goto _LL1714; _LL1714: _temp1713=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp1633)->f2; _LL1718: _temp1717= _temp1713.f1; goto _LL1716; _LL1716:
_temp1715= _temp1713.f2; goto _LL1708; _LL1708: _temp1707=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp1633)->f3; _LL1712: _temp1711= _temp1707.f1;
goto _LL1710; _LL1710: _temp1709= _temp1707.f2; goto _LL1706; _LL1706: _temp1705=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1633)->f4; goto
_LL1654;} else{ goto _LL1655;} _LL1655: if(( unsigned int) _temp1633 > 1u?((
struct _tunion_struct*) _temp1633)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1724:
_temp1723=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1633)->f1; goto _LL1722; _LL1722: _temp1721=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1633)->f2; goto _LL1656;} else{ goto
_LL1657;} _LL1657: if(( unsigned int) _temp1633 > 1u?(( struct _tunion_struct*)
_temp1633)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1728: _temp1727=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1633)->f1; if( _temp1727 == 0){ goto
_LL1726;} else{ goto _LL1659;} _LL1726: _temp1725=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1633)->f2; goto _LL1658;} else{ goto
_LL1659;} _LL1659: if(( unsigned int) _temp1633 > 1u?(( struct _tunion_struct*)
_temp1633)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1732: _temp1731=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1633)->f1; goto _LL1730; _LL1730:
_temp1729=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1633)->f2; goto _LL1660;} else{ goto _LL1661;} _LL1661: if(( unsigned int)
_temp1633 > 1u?(( struct _tunion_struct*) _temp1633)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1736: _temp1735=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1633)->f1; goto _LL1734; _LL1734: _temp1733=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1633)->f2; goto _LL1662;} else{ goto
_LL1663;} _LL1663: if(( unsigned int) _temp1633 > 1u?(( struct _tunion_struct*)
_temp1633)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1738: _temp1737=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1633)->f1; goto _LL1664;} else{ goto
_LL1665;} _LL1665: if(( unsigned int) _temp1633 > 1u?(( struct _tunion_struct*)
_temp1633)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1740: _temp1739=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1633)->f1; goto _LL1666;} else{ goto
_LL1667;} _LL1667: if(( unsigned int) _temp1633 > 1u?(( struct _tunion_struct*)
_temp1633)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1744: _temp1743=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1633)->f1; goto
_LL1742; _LL1742: _temp1741=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1633)->f2; goto _LL1668;} else{ goto _LL1669;} _LL1669: if(( unsigned int)
_temp1633 > 1u?(( struct _tunion_struct*) _temp1633)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1752: _temp1751=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1633)->f1; goto _LL1746; _LL1746: _temp1745=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp1633)->f2; _LL1750: _temp1749= _temp1745.f1; goto _LL1748; _LL1748:
_temp1747= _temp1745.f2; goto _LL1670;} else{ goto _LL1671;} _LL1671: if((
unsigned int) _temp1633 > 1u?(( struct _tunion_struct*) _temp1633)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1756: _temp1755=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1633)->f1; goto _LL1754; _LL1754: _temp1753=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1633)->f2; goto _LL1672;} else{ goto
_LL1673;} _LL1673: if(( unsigned int) _temp1633 > 1u?(( struct _tunion_struct*)
_temp1633)->tag == Cyc_Absyn_Region_s_tag: 0){ _LL1762: _temp1761=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_Region_s_struct*) _temp1633)->f1; goto _LL1760; _LL1760:
_temp1759=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1633)->f2; goto _LL1758; _LL1758: _temp1757=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Region_s_struct*) _temp1633)->f3; goto _LL1674;} else{ goto
_LL1634;} _LL1636: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1763=(
char*)";"; struct _tagged_string _temp1764; _temp1764.curr= _temp1763; _temp1764.base=
_temp1763; _temp1764.last_plus_one= _temp1763 + 2; _temp1764;})); goto _LL1634;
_LL1638: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc( _temp1675), Cyc_PP_text((
struct _tagged_string)({ char* _temp1765=( char*)";"; struct _tagged_string
_temp1766; _temp1766.curr= _temp1765; _temp1766.base= _temp1765; _temp1766.last_plus_one=
_temp1765 + 2; _temp1766;}))); goto _LL1634; _LL1640: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1679)){ s= Cyc_Absynpp_cat6( Cyc_PP_text((
struct _tagged_string)({ char* _temp1767=( char*)"{ "; struct _tagged_string
_temp1768; _temp1768.curr= _temp1767; _temp1768.base= _temp1767; _temp1768.last_plus_one=
_temp1767 + 3; _temp1768;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1679)),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1769=( char*)"}";
struct _tagged_string _temp1770; _temp1770.curr= _temp1769; _temp1770.base=
_temp1769; _temp1770.last_plus_one= _temp1769 + 2; _temp1770;})), Cyc_PP_line_doc(),
Cyc_Absynpp_is_declaration( _temp1677)? Cyc_Absynpp_cat4( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1771=( char*)"{ "; struct _tagged_string _temp1772;
_temp1772.curr= _temp1771; _temp1772.base= _temp1771; _temp1772.last_plus_one=
_temp1771 + 3; _temp1772;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1677)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1773=( char*)"}"; struct
_tagged_string _temp1774; _temp1774.curr= _temp1773; _temp1774.base= _temp1773;
_temp1774.last_plus_one= _temp1773 + 2; _temp1774;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1677));} else{ if( Cyc_Absynpp_is_declaration( _temp1677)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1679), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1775=( char*)"{ "; struct _tagged_string _temp1776;
_temp1776.curr= _temp1775; _temp1776.base= _temp1775; _temp1776.last_plus_one=
_temp1775 + 3; _temp1776;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1677)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1777=( char*)"}"; struct
_tagged_string _temp1778; _temp1778.curr= _temp1777; _temp1778.base= _temp1777;
_temp1778.last_plus_one= _temp1777 + 2; _temp1778;})), Cyc_PP_line_doc());}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,(
struct _tagged_string)({ char* _temp1779=( char*)""; struct _tagged_string
_temp1780; _temp1780.curr= _temp1779; _temp1780.base= _temp1779; _temp1780.last_plus_one=
_temp1779 + 1; _temp1780;}),({ struct Cyc_List_List* _temp1781=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1781->hd=( void*) _temp1679;
_temp1781->tl=({ struct Cyc_List_List* _temp1782=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1782->hd=( void*) _temp1677;
_temp1782->tl= 0; _temp1782;}); _temp1781;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,( struct
_tagged_string)({ char* _temp1783=( char*)""; struct _tagged_string _temp1784;
_temp1784.curr= _temp1783; _temp1784.base= _temp1783; _temp1784.last_plus_one=
_temp1783 + 1; _temp1784;}),({ struct Cyc_List_List* _temp1785=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1785->hd=( void*) _temp1679;
_temp1785->tl=({ struct Cyc_List_List* _temp1786=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1786->hd=( void*) _temp1677;
_temp1786->tl= 0; _temp1786;}); _temp1785;}));} goto _LL1634; _LL1642: if(
_temp1681 == 0){ s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1787=(
char*)"return;"; struct _tagged_string _temp1788; _temp1788.curr= _temp1787;
_temp1788.base= _temp1787; _temp1788.last_plus_one= _temp1787 + 8; _temp1788;}));}
else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1789=( char*)"return "; struct _tagged_string _temp1790; _temp1790.curr=
_temp1789; _temp1790.base= _temp1789; _temp1790.last_plus_one= _temp1789 + 8;
_temp1790;})), _temp1681 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1791= _temp1681; if( _temp1791 == 0){
_throw( Null_Exception);} _temp1791;})), Cyc_PP_text(( struct _tagged_string)({
char* _temp1792=( char*)";"; struct _tagged_string _temp1793; _temp1793.curr=
_temp1792; _temp1793.base= _temp1792; _temp1793.last_plus_one= _temp1792 + 2;
_temp1793;})));} goto _LL1634; _LL1644: { int print_else;{ void* _temp1794=(
void*) _temp1683->r; _LL1796: if(( int) _temp1794 == Cyc_Absyn_Skip_s){ goto
_LL1797;} else{ goto _LL1798;} _LL1798: goto _LL1799; _LL1797: print_else= 0;
goto _LL1795; _LL1799: print_else= 1; goto _LL1795; _LL1795:;} s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1800=( char*)"if ("; struct
_tagged_string _temp1801; _temp1801.curr= _temp1800; _temp1801.base= _temp1800;
_temp1801.last_plus_one= _temp1800 + 5; _temp1801;})), Cyc_Absynpp_exp2doc(
_temp1687), Cyc_PP_text(( struct _tagged_string)({ char* _temp1802=( char*)") {";
struct _tagged_string _temp1803; _temp1803.curr= _temp1802; _temp1803.base=
_temp1802; _temp1803.last_plus_one= _temp1802 + 4; _temp1803;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1685))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1804=( char*)"}"; struct
_tagged_string _temp1805; _temp1805.curr= _temp1804; _temp1805.base= _temp1804;
_temp1805.last_plus_one= _temp1804 + 2; _temp1805;})), print_else? Cyc_Absynpp_cat5(
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1806=( char*)"else {";
struct _tagged_string _temp1807; _temp1807.curr= _temp1806; _temp1807.base=
_temp1806; _temp1807.last_plus_one= _temp1806 + 7; _temp1807;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1683))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1808=( char*)"}"; struct
_tagged_string _temp1809; _temp1809.curr= _temp1808; _temp1809.base= _temp1808;
_temp1809.last_plus_one= _temp1808 + 2; _temp1809;}))): Cyc_PP_nil_doc()); goto
_LL1634;} _LL1646: s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({
char* _temp1810=( char*)"while ("; struct _tagged_string _temp1811; _temp1811.curr=
_temp1810; _temp1811.base= _temp1810; _temp1811.last_plus_one= _temp1810 + 8;
_temp1811;})), Cyc_Absynpp_exp2doc( _temp1695), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1812=( char*)") {"; struct _tagged_string _temp1813;
_temp1813.curr= _temp1812; _temp1813.base= _temp1812; _temp1813.last_plus_one=
_temp1812 + 4; _temp1813;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1689))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1814=( char*)"}"; struct _tagged_string _temp1815;
_temp1815.curr= _temp1814; _temp1815.base= _temp1814; _temp1815.last_plus_one=
_temp1814 + 2; _temp1815;}))); goto _LL1634; _LL1648: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1816=( char*)"break;"; struct _tagged_string
_temp1817; _temp1817.curr= _temp1816; _temp1817.base= _temp1816; _temp1817.last_plus_one=
_temp1816 + 7; _temp1817;})); goto _LL1634; _LL1650: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1818=( char*)"continue;"; struct _tagged_string
_temp1819; _temp1819.curr= _temp1818; _temp1819.base= _temp1818; _temp1819.last_plus_one=
_temp1818 + 10; _temp1819;})); goto _LL1634; _LL1652: s= Cyc_PP_text(({ struct
_tagged_string _temp1820=* _temp1703; xprintf("goto %.*s;", _temp1820.last_plus_one
- _temp1820.curr, _temp1820.curr);})); goto _LL1634; _LL1654: s= Cyc_Absynpp_cat10(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1821=( char*)"for("; struct
_tagged_string _temp1822; _temp1822.curr= _temp1821; _temp1822.base= _temp1821;
_temp1822.last_plus_one= _temp1821 + 5; _temp1822;})), Cyc_Absynpp_exp2doc(
_temp1719), Cyc_PP_text(( struct _tagged_string)({ char* _temp1823=( char*)"; ";
struct _tagged_string _temp1824; _temp1824.curr= _temp1823; _temp1824.base=
_temp1823; _temp1824.last_plus_one= _temp1823 + 3; _temp1824;})), Cyc_Absynpp_exp2doc(
_temp1717), Cyc_PP_text(( struct _tagged_string)({ char* _temp1825=( char*)"; ";
struct _tagged_string _temp1826; _temp1826.curr= _temp1825; _temp1826.base=
_temp1825; _temp1826.last_plus_one= _temp1825 + 3; _temp1826;})), Cyc_Absynpp_exp2doc(
_temp1711), Cyc_PP_text(( struct _tagged_string)({ char* _temp1827=( char*)") {";
struct _tagged_string _temp1828; _temp1828.curr= _temp1827; _temp1828.base=
_temp1827; _temp1828.last_plus_one= _temp1827 + 4; _temp1828;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1705))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1829=( char*)"}"; struct
_tagged_string _temp1830; _temp1830.curr= _temp1829; _temp1830.base= _temp1829;
_temp1830.last_plus_one= _temp1829 + 2; _temp1830;}))); goto _LL1634; _LL1656: s=
Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char* _temp1831=( char*)"switch (";
struct _tagged_string _temp1832; _temp1832.curr= _temp1831; _temp1832.base=
_temp1831; _temp1832.last_plus_one= _temp1831 + 9; _temp1832;})), Cyc_Absynpp_exp2doc(
_temp1723), Cyc_PP_text(( struct _tagged_string)({ char* _temp1833=( char*)") {";
struct _tagged_string _temp1834; _temp1834.curr= _temp1833; _temp1834.base=
_temp1833; _temp1834.last_plus_one= _temp1833 + 4; _temp1834;})), Cyc_PP_line_doc(),
Cyc_Absynpp_switchclauses2doc( _temp1721), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1835=( char*)"}"; struct _tagged_string
_temp1836; _temp1836.curr= _temp1835; _temp1836.base= _temp1835; _temp1836.last_plus_one=
_temp1835 + 2; _temp1836;}))); goto _LL1634; _LL1658: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1837=( char*)"fallthru;"; struct _tagged_string
_temp1838; _temp1838.curr= _temp1837; _temp1838.base= _temp1837; _temp1838.last_plus_one=
_temp1837 + 10; _temp1838;})); goto _LL1634; _LL1660: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1839=( char*)"fallthru("; struct
_tagged_string _temp1840; _temp1840.curr= _temp1839; _temp1840.base= _temp1839;
_temp1840.last_plus_one= _temp1839 + 10; _temp1840;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1731), Cyc_PP_text(( struct _tagged_string)({ char* _temp1841=( char*)");";
struct _tagged_string _temp1842; _temp1842.curr= _temp1841; _temp1842.base=
_temp1841; _temp1842.last_plus_one= _temp1841 + 3; _temp1842;}))); goto _LL1634;
_LL1662: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1735), Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1733)); goto _LL1634; _LL1664: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1843=( char*)"cut "; struct
_tagged_string _temp1844; _temp1844.curr= _temp1843; _temp1844.base= _temp1843;
_temp1844.last_plus_one= _temp1843 + 5; _temp1844;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1737))); goto _LL1634; _LL1666: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1845=( char*)"splice "; struct _tagged_string
_temp1846; _temp1846.curr= _temp1845; _temp1846.base= _temp1845; _temp1846.last_plus_one=
_temp1845 + 8; _temp1846;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1739)));
goto _LL1634; _LL1668: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1741): 0){ s= Cyc_Absynpp_cat6( Cyc_Absynpp_textptr( _temp1743), Cyc_PP_text((
struct _tagged_string)({ char* _temp1847=( char*)": {"; struct _tagged_string
_temp1848; _temp1848.curr= _temp1847; _temp1848.base= _temp1847; _temp1848.last_plus_one=
_temp1847 + 4; _temp1848;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1741)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1849=( char*)"}"; struct _tagged_string _temp1850; _temp1850.curr=
_temp1849; _temp1850.base= _temp1849; _temp1850.last_plus_one= _temp1849 + 2;
_temp1850;})));} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( _temp1743), Cyc_PP_text((
struct _tagged_string)({ char* _temp1851=( char*)": "; struct _tagged_string
_temp1852; _temp1852.curr= _temp1851; _temp1852.base= _temp1851; _temp1852.last_plus_one=
_temp1851 + 3; _temp1852;})), Cyc_Absynpp_stmt2doc( _temp1741));} goto _LL1634;
_LL1670: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1853=( char*)"do {"; struct _tagged_string _temp1854; _temp1854.curr=
_temp1853; _temp1854.base= _temp1853; _temp1854.last_plus_one= _temp1853 + 5;
_temp1854;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1751)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1855=( char*)"} while ("; struct _tagged_string _temp1856; _temp1856.curr=
_temp1855; _temp1856.base= _temp1855; _temp1856.last_plus_one= _temp1855 + 10;
_temp1856;})), Cyc_Absynpp_exp2doc( _temp1749), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1857=( char*)");"; struct _tagged_string _temp1858;
_temp1858.curr= _temp1857; _temp1858.base= _temp1857; _temp1858.last_plus_one=
_temp1857 + 3; _temp1858;}))); goto _LL1634; _LL1672: s= Cyc_Absynpp_cat9( Cyc_PP_text((
struct _tagged_string)({ char* _temp1859=( char*)"try {"; struct _tagged_string
_temp1860; _temp1860.curr= _temp1859; _temp1860.base= _temp1859; _temp1860.last_plus_one=
_temp1859 + 6; _temp1860;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1755)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1861=( char*)"} catch {"; struct _tagged_string _temp1862; _temp1862.curr=
_temp1861; _temp1862.base= _temp1861; _temp1862.last_plus_one= _temp1861 + 10;
_temp1862;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1753)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1863=( char*)"}"; struct _tagged_string _temp1864; _temp1864.curr=
_temp1863; _temp1864.base= _temp1863; _temp1864.last_plus_one= _temp1863 + 2;
_temp1864;}))); goto _LL1634; _LL1674: s= Cyc_Absynpp_cat9( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1865=( char*)"region<"; struct _tagged_string
_temp1866; _temp1866.curr= _temp1865; _temp1866.base= _temp1865; _temp1866.last_plus_one=
_temp1865 + 8; _temp1866;})), Cyc_Absynpp_textptr( Cyc_Absynpp_get_name(
_temp1761)), Cyc_PP_text(( struct _tagged_string)({ char* _temp1867=( char*)">";
struct _tagged_string _temp1868; _temp1868.curr= _temp1867; _temp1868.base=
_temp1867; _temp1868.last_plus_one= _temp1867 + 2; _temp1868;})), Cyc_Absynpp_qvar2doc(
_temp1759->name), Cyc_PP_text(( struct _tagged_string)({ char* _temp1869=( char*)"{";
struct _tagged_string _temp1870; _temp1870.curr= _temp1869; _temp1870.base=
_temp1869; _temp1870.last_plus_one= _temp1869 + 2; _temp1870;})), Cyc_PP_line_doc(),
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1757)), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1871=( char*)"}"; struct _tagged_string
_temp1872; _temp1872.curr= _temp1871; _temp1872.base= _temp1871; _temp1872.last_plus_one=
_temp1871 + 2; _temp1872;}))); goto _LL1634; _LL1634:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_pat2doc( struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void*
_temp1873=( void*) p->r; int _temp1907; void* _temp1909; char _temp1911; struct
_tagged_string _temp1913; struct Cyc_Absyn_Vardecl* _temp1915; struct Cyc_List_List*
_temp1917; struct Cyc_Absyn_Pat* _temp1919; struct Cyc_Absyn_Vardecl* _temp1921;
struct _tuple0* _temp1923; struct Cyc_List_List* _temp1925; struct Cyc_List_List*
_temp1927; struct _tuple0* _temp1929; struct Cyc_List_List* _temp1931; struct
Cyc_List_List* _temp1933; struct _tuple0* _temp1935; struct Cyc_List_List*
_temp1937; struct Cyc_List_List* _temp1939; struct Cyc_Core_Opt* _temp1941;
struct Cyc_Absyn_Structdecl* _temp1943; struct Cyc_Absyn_Enumfield* _temp1945;
struct Cyc_Absyn_Enumdecl* _temp1947; struct _tuple0* _temp1949; struct Cyc_Absyn_Tunionfield*
_temp1951; struct Cyc_Absyn_Tuniondecl* _temp1953; struct Cyc_List_List*
_temp1955; struct Cyc_List_List* _temp1957; struct Cyc_Core_Opt* _temp1959;
struct _tuple0* _temp1961; struct Cyc_Absyn_Tunionfield* _temp1963; struct Cyc_Absyn_XTuniondecl*
_temp1965; struct Cyc_List_List* _temp1967; struct Cyc_List_List* _temp1969;
struct _tuple0* _temp1971; _LL1875: if(( int) _temp1873 == Cyc_Absyn_Wild_p){
goto _LL1876;} else{ goto _LL1877;} _LL1877: if(( int) _temp1873 == Cyc_Absyn_Null_p){
goto _LL1878;} else{ goto _LL1879;} _LL1879: if(( unsigned int) _temp1873 > 2u?((
struct _tunion_struct*) _temp1873)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1910:
_temp1909=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1873)->f1; goto
_LL1908; _LL1908: _temp1907=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp1873)->f2;
goto _LL1880;} else{ goto _LL1881;} _LL1881: if(( unsigned int) _temp1873 > 2u?((
struct _tunion_struct*) _temp1873)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL1912:
_temp1911=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1873)->f1; goto
_LL1882;} else{ goto _LL1883;} _LL1883: if(( unsigned int) _temp1873 > 2u?((
struct _tunion_struct*) _temp1873)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1914:
_temp1913=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1873)->f1;
goto _LL1884;} else{ goto _LL1885;} _LL1885: if(( unsigned int) _temp1873 > 2u?((
struct _tunion_struct*) _temp1873)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1916:
_temp1915=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1873)->f1; goto _LL1886;} else{ goto _LL1887;} _LL1887: if(( unsigned int)
_temp1873 > 2u?(( struct _tunion_struct*) _temp1873)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1918: _temp1917=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1873)->f1; goto _LL1888;} else{ goto _LL1889;} _LL1889: if(( unsigned int)
_temp1873 > 2u?(( struct _tunion_struct*) _temp1873)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1920: _temp1919=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1873)->f1; goto _LL1890;} else{ goto _LL1891;} _LL1891: if(( unsigned int)
_temp1873 > 2u?(( struct _tunion_struct*) _temp1873)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1922: _temp1921=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1873)->f1; goto _LL1892;} else{ goto _LL1893;} _LL1893: if(( unsigned int)
_temp1873 > 2u?(( struct _tunion_struct*) _temp1873)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL1924: _temp1923=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1873)->f1; goto _LL1894;} else{ goto _LL1895;} _LL1895: if(( unsigned int)
_temp1873 > 2u?(( struct _tunion_struct*) _temp1873)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1930: _temp1929=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1873)->f1; goto _LL1928; _LL1928: _temp1927=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1873)->f2; goto _LL1926; _LL1926:
_temp1925=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1873)->f3; goto _LL1896;} else{ goto _LL1897;} _LL1897: if(( unsigned int)
_temp1873 > 2u?(( struct _tunion_struct*) _temp1873)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1936: _temp1935=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1873)->f1; goto _LL1934; _LL1934: _temp1933=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1873)->f2; goto _LL1932; _LL1932:
_temp1931=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1873)->f3; goto _LL1898;} else{ goto _LL1899;} _LL1899: if(( unsigned int)
_temp1873 > 2u?(( struct _tunion_struct*) _temp1873)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1944: _temp1943=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1873)->f1; goto _LL1942; _LL1942: _temp1941=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1873)->f2; goto _LL1940; _LL1940:
_temp1939=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1873)->f3; goto _LL1938; _LL1938: _temp1937=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1873)->f4; goto _LL1900;} else{ goto
_LL1901;} _LL1901: if(( unsigned int) _temp1873 > 2u?(( struct _tunion_struct*)
_temp1873)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1950: _temp1949=( struct _tuple0*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1873)->f1; goto _LL1948; _LL1948:
_temp1947=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1873)->f2; goto _LL1946; _LL1946: _temp1945=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1873)->f3; goto _LL1902;} else{ goto
_LL1903;} _LL1903: if(( unsigned int) _temp1873 > 2u?(( struct _tunion_struct*)
_temp1873)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1962: _temp1961=( struct
_tuple0*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1873)->f1; goto _LL1960;
_LL1960: _temp1959=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1873)->f2; goto _LL1958; _LL1958: _temp1957=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1873)->f3; goto _LL1956; _LL1956:
_temp1955=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1873)->f4; goto _LL1954; _LL1954: _temp1953=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1873)->f5; goto _LL1952; _LL1952:
_temp1951=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1873)->f6; goto _LL1904;} else{ goto _LL1905;} _LL1905: if(( unsigned int)
_temp1873 > 2u?(( struct _tunion_struct*) _temp1873)->tag == Cyc_Absyn_XTunion_p_tag:
0){ _LL1972: _temp1971=( struct _tuple0*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1873)->f1; goto _LL1970; _LL1970: _temp1969=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1873)->f2; goto _LL1968; _LL1968:
_temp1967=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1873)->f3; goto _LL1966; _LL1966: _temp1965=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1873)->f4; goto _LL1964; _LL1964:
_temp1963=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1873)->f5; goto _LL1906;} else{ goto _LL1874;} _LL1876: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp1973=( char*)"_"; struct _tagged_string
_temp1974; _temp1974.curr= _temp1973; _temp1974.base= _temp1973; _temp1974.last_plus_one=
_temp1973 + 2; _temp1974;})); goto _LL1874; _LL1878: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1975=( char*)"null"; struct _tagged_string
_temp1976; _temp1976.curr= _temp1975; _temp1976.base= _temp1975; _temp1976.last_plus_one=
_temp1975 + 5; _temp1976;})); goto _LL1874; _LL1880: if( _temp1909 ==( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d", _temp1907));} else{ s= Cyc_PP_text(
xprintf("%u",( unsigned int) _temp1907));} goto _LL1874; _LL1882: s= Cyc_PP_text(({
struct _tagged_string _temp1977= Cyc_Absynpp_char_escape( _temp1911); xprintf("'%.*s'",
_temp1977.last_plus_one - _temp1977.curr, _temp1977.curr);})); goto _LL1874;
_LL1884: s= Cyc_PP_text( _temp1913); goto _LL1874; _LL1886: s= Cyc_Absynpp_qvar2doc(
_temp1915->name); goto _LL1874; _LL1888: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1978=( char*)"$("; struct _tagged_string
_temp1979; _temp1979.curr= _temp1978; _temp1979.base= _temp1978; _temp1979.last_plus_one=
_temp1978 + 3; _temp1979;})),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Pat*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,( struct _tagged_string)({ char* _temp1980=(
char*)","; struct _tagged_string _temp1981; _temp1981.curr= _temp1980; _temp1981.base=
_temp1980; _temp1981.last_plus_one= _temp1980 + 2; _temp1981;}), _temp1917), Cyc_PP_text((
struct _tagged_string)({ char* _temp1982=( char*)")"; struct _tagged_string
_temp1983; _temp1983.curr= _temp1982; _temp1983.base= _temp1982; _temp1983.last_plus_one=
_temp1982 + 2; _temp1983;}))); goto _LL1874; _LL1890: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1984=( char*)"&"; struct _tagged_string
_temp1985; _temp1985.curr= _temp1984; _temp1985.base= _temp1984; _temp1985.last_plus_one=
_temp1984 + 2; _temp1985;})), Cyc_Absynpp_pat2doc( _temp1919)); goto _LL1874;
_LL1892: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1986=( char*)"*"; struct _tagged_string _temp1987; _temp1987.curr=
_temp1986; _temp1987.base= _temp1986; _temp1987.last_plus_one= _temp1986 + 2;
_temp1987;})), Cyc_Absynpp_qvar2doc( _temp1921->name)); goto _LL1874; _LL1894: s=
Cyc_Absynpp_qvar2doc( _temp1923); goto _LL1874; _LL1896: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1929), Cyc_Absynpp_tvars2doc( _temp1927), Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1988=( char*)"("; struct _tagged_string
_temp1989; _temp1989.curr= _temp1988; _temp1989.base= _temp1988; _temp1989.last_plus_one=
_temp1988 + 2; _temp1989;}),( struct _tagged_string)({ char* _temp1990=( char*)")";
struct _tagged_string _temp1991; _temp1991.curr= _temp1990; _temp1991.base=
_temp1990; _temp1991.last_plus_one= _temp1990 + 2; _temp1991;}),( struct
_tagged_string)({ char* _temp1992=( char*)","; struct _tagged_string _temp1993;
_temp1993.curr= _temp1992; _temp1993.base= _temp1992; _temp1993.last_plus_one=
_temp1992 + 2; _temp1993;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1925))); goto _LL1874; _LL1898: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1935), Cyc_Absynpp_tvars2doc( _temp1933), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1994=( char*)"{"; struct _tagged_string _temp1995;
_temp1995.curr= _temp1994; _temp1995.base= _temp1994; _temp1995.last_plus_one=
_temp1994 + 2; _temp1995;}),( struct _tagged_string)({ char* _temp1996=( char*)"}";
struct _tagged_string _temp1997; _temp1997.curr= _temp1996; _temp1997.base=
_temp1996; _temp1997.last_plus_one= _temp1996 + 2; _temp1997;}),( struct
_tagged_string)({ char* _temp1998=( char*)","; struct _tagged_string _temp1999;
_temp1999.curr= _temp1998; _temp1999.base= _temp1998; _temp1999.last_plus_one=
_temp1998 + 2; _temp1999;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1931))); goto _LL1874; _LL1900: s= Cyc_Absynpp_cat3( _temp1943->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp2000= _temp1943->name; if( _temp2000 == 0){ _throw( Null_Exception);}
_temp2000->v;})), Cyc_Absynpp_tvars2doc( _temp1939), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp2001=( char*)"{"; struct _tagged_string _temp2002;
_temp2002.curr= _temp2001; _temp2002.base= _temp2001; _temp2002.last_plus_one=
_temp2001 + 2; _temp2002;}),( struct _tagged_string)({ char* _temp2003=( char*)"}";
struct _tagged_string _temp2004; _temp2004.curr= _temp2003; _temp2004.base=
_temp2003; _temp2004.last_plus_one= _temp2003 + 2; _temp2004;}),( struct
_tagged_string)({ char* _temp2005=( char*)","; struct _tagged_string _temp2006;
_temp2006.curr= _temp2005; _temp2006.base= _temp2005; _temp2006.last_plus_one=
_temp2005 + 2; _temp2006;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1937))); goto _LL1874; _LL1902: s= Cyc_Absynpp_qvar2doc( _temp1949); goto
_LL1874; _LL1904: _temp1971= _temp1961; _temp1969= _temp1957; _temp1967=
_temp1955; goto _LL1906; _LL1906: if( _temp1967 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1971);} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1971), Cyc_Absynpp_tvars2doc(
_temp1969), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp2007=( char*)"(";
struct _tagged_string _temp2008; _temp2008.curr= _temp2007; _temp2008.base=
_temp2007; _temp2008.last_plus_one= _temp2007 + 2; _temp2008;}),( struct
_tagged_string)({ char* _temp2009=( char*)")"; struct _tagged_string _temp2010;
_temp2010.curr= _temp2009; _temp2010.base= _temp2009; _temp2010.last_plus_one=
_temp2009 + 2; _temp2010;}),( struct _tagged_string)({ char* _temp2011=( char*)",";
struct _tagged_string _temp2012; _temp2012.curr= _temp2011; _temp2012.base=
_temp2011; _temp2012.last_plus_one= _temp2011 + 2; _temp2012;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1967)));} goto _LL1874; _LL1874:;} return s;} struct
Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return Cyc_Absynpp_cat2(
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp2013=( char*)""; struct
_tagged_string _temp2014; _temp2014.curr= _temp2013; _temp2014.base= _temp2013;
_temp2014.last_plus_one= _temp2013 + 1; _temp2014;}),( struct _tagged_string)({
char* _temp2015=( char*)"="; struct _tagged_string _temp2016; _temp2016.curr=
_temp2015; _temp2016.base= _temp2015; _temp2016.last_plus_one= _temp2015 + 2;
_temp2016;}),( struct _tagged_string)({ char* _temp2017=( char*)"="; struct
_tagged_string _temp2018; _temp2018.curr= _temp2017; _temp2018.base= _temp2017;
_temp2018.last_plus_one= _temp2017 + 2; _temp2018;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)), Cyc_Absynpp_pat2doc((* dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
==( void*) Cyc_Absyn_Wild_p: 0){ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2019=( char*)"default: "; struct _tagged_string
_temp2020; _temp2020.curr= _temp2019; _temp2020.base= _temp2019; _temp2020.last_plus_one=
_temp2019 + 10; _temp2020;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause == 0){ return Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2021=( char*)"case "; struct
_tagged_string _temp2022; _temp2022.curr= _temp2021; _temp2022.base= _temp2021;
_temp2022.last_plus_one= _temp2021 + 6; _temp2022;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2023=( char*)": "; struct
_tagged_string _temp2024; _temp2024.curr= _temp2023; _temp2024.base= _temp2023;
_temp2024.last_plus_one= _temp2023 + 3; _temp2024;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ return Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2025=( char*)"case "; struct
_tagged_string _temp2026; _temp2026.curr= _temp2025; _temp2026.base= _temp2025;
_temp2026.last_plus_one= _temp2025 + 6; _temp2026;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2027=( char*)" && "; struct
_tagged_string _temp2028; _temp2028.curr= _temp2027; _temp2028.base= _temp2027;
_temp2028.last_plus_one= _temp2027 + 5; _temp2028;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2029= c->where_clause; if(
_temp2029 == 0){ _throw( Null_Exception);} _temp2029;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2030=( char*)": "; struct _tagged_string _temp2031;
_temp2031.curr= _temp2030; _temp2031.base= _temp2030; _temp2031.last_plus_one=
_temp2030 + 3; _temp2031;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,( struct _tagged_string)({
char* _temp2032=( char*)""; struct _tagged_string _temp2033; _temp2033.curr=
_temp2032; _temp2033.base= _temp2032; _temp2033.last_plus_one= _temp2032 + 1;
_temp2033;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield*
f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);} else{ return Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( f->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2034=( char*)" = "; struct _tagged_string _temp2035; _temp2035.curr=
_temp2034; _temp2035.base= _temp2034; _temp2035.last_plus_one= _temp2034 + 4;
_temp2035;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2036= f->tag; if( _temp2036 == 0){ _throw( Null_Exception);} _temp2036;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,(
struct _tagged_string)({ char* _temp2037=( char*)","; struct _tagged_string
_temp2038; _temp2038.curr= _temp2037; _temp2038.base= _temp2037; _temp2038.last_plus_one=
_temp2037 + 2; _temp2038;}), fs);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2039=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2065; struct Cyc_Absyn_Structdecl* _temp2067;
struct Cyc_Absyn_Uniondecl* _temp2069; struct Cyc_Absyn_Vardecl* _temp2071;
struct Cyc_Absyn_Vardecl _temp2073; struct Cyc_List_List* _temp2074; struct Cyc_Core_Opt*
_temp2076; struct Cyc_Absyn_Exp* _temp2078; void* _temp2080; struct Cyc_Absyn_Tqual
_temp2082; struct _tuple0* _temp2084; void* _temp2086; struct Cyc_Absyn_Tuniondecl*
_temp2088; struct Cyc_Absyn_XTuniondecl* _temp2090; int _temp2092; struct Cyc_Absyn_Exp*
_temp2094; struct Cyc_Core_Opt* _temp2096; struct Cyc_Core_Opt* _temp2098;
struct Cyc_Absyn_Pat* _temp2100; struct Cyc_Absyn_Enumdecl* _temp2102; struct
Cyc_Absyn_Enumdecl _temp2104; struct Cyc_List_List* _temp2105; struct _tuple0*
_temp2107; void* _temp2109; struct Cyc_Absyn_Typedefdecl* _temp2111; struct Cyc_List_List*
_temp2113; struct _tagged_string* _temp2115; struct Cyc_List_List* _temp2117;
struct _tuple0* _temp2119; struct Cyc_List_List* _temp2121; _LL2041: if(((
struct _tunion_struct*) _temp2039)->tag == Cyc_Absyn_Fn_d_tag){ _LL2066:
_temp2065=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp2039)->f1;
goto _LL2042;} else{ goto _LL2043;} _LL2043: if((( struct _tunion_struct*)
_temp2039)->tag == Cyc_Absyn_Struct_d_tag){ _LL2068: _temp2067=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp2039)->f1; goto _LL2044;} else{ goto
_LL2045;} _LL2045: if((( struct _tunion_struct*) _temp2039)->tag == Cyc_Absyn_Union_d_tag){
_LL2070: _temp2069=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp2039)->f1; goto _LL2046;} else{ goto _LL2047;} _LL2047: if((( struct
_tunion_struct*) _temp2039)->tag == Cyc_Absyn_Var_d_tag){ _LL2072: _temp2071=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp2039)->f1;
_temp2073=* _temp2071; _LL2087: _temp2086=( void*) _temp2073.sc; goto _LL2085;
_LL2085: _temp2084=( struct _tuple0*) _temp2073.name; goto _LL2083; _LL2083:
_temp2082=( struct Cyc_Absyn_Tqual) _temp2073.tq; goto _LL2081; _LL2081:
_temp2080=( void*) _temp2073.type; goto _LL2079; _LL2079: _temp2078=( struct Cyc_Absyn_Exp*)
_temp2073.initializer; goto _LL2077; _LL2077: _temp2076=( struct Cyc_Core_Opt*)
_temp2073.rgn; goto _LL2075; _LL2075: _temp2074=( struct Cyc_List_List*)
_temp2073.attributes; goto _LL2048;} else{ goto _LL2049;} _LL2049: if((( struct
_tunion_struct*) _temp2039)->tag == Cyc_Absyn_Tunion_d_tag){ _LL2089: _temp2088=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp2039)->f1;
goto _LL2050;} else{ goto _LL2051;} _LL2051: if((( struct _tunion_struct*)
_temp2039)->tag == Cyc_Absyn_XTunion_d_tag){ _LL2091: _temp2090=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp2039)->f1; goto _LL2052;} else{ goto
_LL2053;} _LL2053: if((( struct _tunion_struct*) _temp2039)->tag == Cyc_Absyn_Let_d_tag){
_LL2101: _temp2100=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2039)->f1; goto _LL2099; _LL2099: _temp2098=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2039)->f2; goto _LL2097; _LL2097: _temp2096=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2039)->f3; goto
_LL2095; _LL2095: _temp2094=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2039)->f4; goto _LL2093; _LL2093: _temp2092=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp2039)->f5; goto _LL2054;} else{ goto _LL2055;} _LL2055: if((( struct
_tunion_struct*) _temp2039)->tag == Cyc_Absyn_Enum_d_tag){ _LL2103: _temp2102=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp2039)->f1;
_temp2104=* _temp2102; _LL2110: _temp2109=( void*) _temp2104.sc; goto _LL2108;
_LL2108: _temp2107=( struct _tuple0*) _temp2104.name; goto _LL2106; _LL2106:
_temp2105=( struct Cyc_List_List*) _temp2104.fields; goto _LL2056;} else{ goto
_LL2057;} _LL2057: if((( struct _tunion_struct*) _temp2039)->tag == Cyc_Absyn_Typedef_d_tag){
_LL2112: _temp2111=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2039)->f1; goto _LL2058;} else{ goto _LL2059;} _LL2059: if((( struct
_tunion_struct*) _temp2039)->tag == Cyc_Absyn_Namespace_d_tag){ _LL2116:
_temp2115=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2039)->f1; goto _LL2114; _LL2114: _temp2113=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2039)->f2; goto _LL2060;} else{ goto
_LL2061;} _LL2061: if((( struct _tunion_struct*) _temp2039)->tag == Cyc_Absyn_Using_d_tag){
_LL2120: _temp2119=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2039)->f1; goto _LL2118; _LL2118: _temp2117=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp2039)->f2; goto _LL2062;} else{ goto
_LL2063;} _LL2063: if((( struct _tunion_struct*) _temp2039)->tag == Cyc_Absyn_ExternC_d_tag){
_LL2122: _temp2121=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2039)->f1; goto _LL2064;} else{ goto _LL2040;} _LL2042: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2133=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2133[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2134; _temp2134.tag= Cyc_Absyn_FnType_tag; _temp2134.f1=({ struct Cyc_Absyn_FnInfo
_temp2135; _temp2135.tvars= _temp2065->tvs; _temp2135.effect= _temp2065->effect;
_temp2135.ret_typ=( void*)(( void*) _temp2065->ret_type); _temp2135.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2065->args); _temp2135.varargs=
_temp2065->varargs; _temp2135.attributes= 0; _temp2135;}); _temp2134;});
_temp2133;}); s= Cyc_Absynpp_cat7( Cyc_PP_text( _temp2065->is_inline?( struct
_tagged_string)({ char* _temp2123=( char*)"inline "; struct _tagged_string
_temp2124; _temp2124.curr= _temp2123; _temp2124.base= _temp2123; _temp2124.last_plus_one=
_temp2123 + 8; _temp2124;}):( struct _tagged_string)({ char* _temp2125=( char*)"";
struct _tagged_string _temp2126; _temp2126.curr= _temp2125; _temp2126.base=
_temp2125; _temp2126.last_plus_one= _temp2125 + 1; _temp2126;})), Cyc_Absynpp_scope2doc((
void*) _temp2065->sc), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual _temp2127;
_temp2127.q_const= 0; _temp2127.q_volatile= 0; _temp2127.q_restrict= 0;
_temp2127;}), t,({ struct Cyc_Core_Opt* _temp2128=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2128->v=( void*) Cyc_Absynpp_cat2(
Cyc_Absynpp_atts2doc( _temp2065->attributes), Cyc_Absynpp_typedef_name2doc(
_temp2065->name)); _temp2128;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2129=( char*)" {"; struct _tagged_string _temp2130; _temp2130.curr=
_temp2129; _temp2130.base= _temp2129; _temp2130.last_plus_one= _temp2129 + 3;
_temp2130;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp2065->body))), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2131=( char*)"}"; struct _tagged_string _temp2132; _temp2132.curr=
_temp2131; _temp2132.base= _temp2131; _temp2132.last_plus_one= _temp2131 + 2;
_temp2132;}))); goto _LL2040;} _LL2044: if( _temp2067->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp2067->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2136=( char*)"struct "; struct _tagged_string
_temp2137; _temp2137.curr= _temp2136; _temp2137.base= _temp2136; _temp2137.last_plus_one=
_temp2136 + 8; _temp2137;})), _temp2067->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2138=( char*)""; struct _tagged_string _temp2139;
_temp2139.curr= _temp2138; _temp2139.base= _temp2138; _temp2139.last_plus_one=
_temp2138 + 1; _temp2139;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp2140= _temp2067->name; if( _temp2140 == 0){ _throw(
Null_Exception);} _temp2140->v;})), Cyc_Absynpp_ktvars2doc( _temp2067->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2141=( char*)";"; struct _tagged_string
_temp2142; _temp2142.curr= _temp2141; _temp2142.base= _temp2141; _temp2142.last_plus_one=
_temp2141 + 2; _temp2142;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2067->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2143=(
char*)"struct "; struct _tagged_string _temp2144; _temp2144.curr= _temp2143;
_temp2144.base= _temp2143; _temp2144.last_plus_one= _temp2143 + 8; _temp2144;})),
_temp2067->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2145=(
char*)""; struct _tagged_string _temp2146; _temp2146.curr= _temp2145; _temp2146.base=
_temp2145; _temp2146.last_plus_one= _temp2145 + 1; _temp2146;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2147= _temp2067->name; if(
_temp2147 == 0){ _throw( Null_Exception);} _temp2147->v;})), Cyc_Absynpp_ktvars2doc(
_temp2067->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2148=( char*)" {";
struct _tagged_string _temp2149; _temp2149.curr= _temp2148; _temp2149.base=
_temp2148; _temp2149.last_plus_one= _temp2148 + 3; _temp2149;})), Cyc_PP_nest( 2,
_temp2067->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2150=(
char*)""; struct _tagged_string _temp2151; _temp2151.curr= _temp2150; _temp2151.base=
_temp2150; _temp2151.last_plus_one= _temp2150 + 1; _temp2151;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2152= _temp2067->fields; if( _temp2152 == 0){ _throw(
Null_Exception);} _temp2152->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2153=( char*)"}"; struct _tagged_string _temp2154;
_temp2154.curr= _temp2153; _temp2154.base= _temp2153; _temp2154.last_plus_one=
_temp2153 + 2; _temp2154;})), Cyc_Absynpp_atts2doc( _temp2067->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2155=( char*)";"; struct _tagged_string
_temp2156; _temp2156.curr= _temp2155; _temp2156.base= _temp2155; _temp2156.last_plus_one=
_temp2155 + 2; _temp2156;})));} goto _LL2040; _LL2046: if( _temp2069->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp2069->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2157=( char*)"union "; struct _tagged_string
_temp2158; _temp2158.curr= _temp2157; _temp2158.base= _temp2157; _temp2158.last_plus_one=
_temp2157 + 7; _temp2158;})), _temp2069->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2159=( char*)""; struct _tagged_string _temp2160;
_temp2160.curr= _temp2159; _temp2160.base= _temp2159; _temp2160.last_plus_one=
_temp2159 + 1; _temp2160;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2161= _temp2069->name; if( _temp2161 == 0){ _throw(
Null_Exception);} _temp2161->v;})), Cyc_Absynpp_tvars2doc( _temp2069->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2162=( char*)";"; struct _tagged_string
_temp2163; _temp2163.curr= _temp2162; _temp2163.base= _temp2162; _temp2163.last_plus_one=
_temp2162 + 2; _temp2163;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2069->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2164=(
char*)"union "; struct _tagged_string _temp2165; _temp2165.curr= _temp2164;
_temp2165.base= _temp2164; _temp2165.last_plus_one= _temp2164 + 7; _temp2165;})),
_temp2069->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2166=(
char*)""; struct _tagged_string _temp2167; _temp2167.curr= _temp2166; _temp2167.base=
_temp2166; _temp2167.last_plus_one= _temp2166 + 1; _temp2167;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2168= _temp2069->name; if(
_temp2168 == 0){ _throw( Null_Exception);} _temp2168->v;})), Cyc_Absynpp_tvars2doc(
_temp2069->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2169=( char*)" {";
struct _tagged_string _temp2170; _temp2170.curr= _temp2169; _temp2170.base=
_temp2169; _temp2170.last_plus_one= _temp2169 + 3; _temp2170;})), Cyc_PP_nest( 2,
_temp2069->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2171=(
char*)""; struct _tagged_string _temp2172; _temp2172.curr= _temp2171; _temp2172.base=
_temp2171; _temp2172.last_plus_one= _temp2171 + 1; _temp2172;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2173= _temp2069->fields; if( _temp2173 == 0){ _throw(
Null_Exception);} _temp2173->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2174=( char*)"}"; struct _tagged_string _temp2175;
_temp2175.curr= _temp2174; _temp2175.base= _temp2174; _temp2175.last_plus_one=
_temp2174 + 2; _temp2175;})), Cyc_Absynpp_atts2doc( _temp2069->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2176=( char*)";"; struct _tagged_string
_temp2177; _temp2177.curr= _temp2176; _temp2177.base= _temp2176; _temp2177.last_plus_one=
_temp2176 + 2; _temp2177;})));} goto _LL2040; _LL2048: { struct Cyc_PP_Doc* sn=
Cyc_Absynpp_typedef_name2doc( _temp2084); s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(
_temp2086), Cyc_Absynpp_tqtd2doc( _temp2082, _temp2080,({ struct Cyc_Core_Opt*
_temp2178=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2178->v=( void*) sn; _temp2178;})), Cyc_Absynpp_atts2doc( _temp2074),
_temp2078 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2179=( char*)"";
struct _tagged_string _temp2180; _temp2180.curr= _temp2179; _temp2180.base=
_temp2179; _temp2180.last_plus_one= _temp2179 + 1; _temp2180;})): Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2181=( char*)" = "; struct
_tagged_string _temp2182; _temp2182.curr= _temp2181; _temp2182.base= _temp2181;
_temp2182.last_plus_one= _temp2181 + 4; _temp2182;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2183= _temp2078; if(
_temp2183 == 0){ _throw( Null_Exception);} _temp2183;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2184=( char*)";"; struct _tagged_string _temp2185;
_temp2185.curr= _temp2184; _temp2185.base= _temp2184; _temp2185.last_plus_one=
_temp2184 + 2; _temp2185;}))); goto _LL2040;} _LL2050: if( _temp2088->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp2088->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2186=( char*)"tunion "; struct
_tagged_string _temp2187; _temp2187.curr= _temp2186; _temp2187.base= _temp2186;
_temp2187.last_plus_one= _temp2186 + 8; _temp2187;})), _temp2088->name == 0? Cyc_PP_text((
struct _tagged_string)({ char* _temp2188=( char*)""; struct _tagged_string
_temp2189; _temp2189.curr= _temp2188; _temp2189.base= _temp2188; _temp2189.last_plus_one=
_temp2188 + 1; _temp2189;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp2190= _temp2088->name; if( _temp2190 == 0){ _throw(
Null_Exception);} _temp2190->v;})), Cyc_Absynpp_ktvars2doc( _temp2088->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2191=( char*)";"; struct _tagged_string
_temp2192; _temp2192.curr= _temp2191; _temp2192.base= _temp2191; _temp2192.last_plus_one=
_temp2191 + 2; _temp2192;})));} else{ s= Cyc_Absynpp_cat8( Cyc_Absynpp_scope2doc((
void*) _temp2088->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2193=(
char*)"tunion "; struct _tagged_string _temp2194; _temp2194.curr= _temp2193;
_temp2194.base= _temp2193; _temp2194.last_plus_one= _temp2193 + 8; _temp2194;})),
_temp2088->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2195=(
char*)""; struct _tagged_string _temp2196; _temp2196.curr= _temp2195; _temp2196.base=
_temp2195; _temp2196.last_plus_one= _temp2195 + 1; _temp2196;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp2197= _temp2088->name; if(
_temp2197 == 0){ _throw( Null_Exception);} _temp2197->v;})), Cyc_Absynpp_ktvars2doc(
_temp2088->tvs), Cyc_PP_text(( struct _tagged_string)({ char* _temp2198=( char*)" {";
struct _tagged_string _temp2199; _temp2199.curr= _temp2198; _temp2199.base=
_temp2198; _temp2199.last_plus_one= _temp2198 + 3; _temp2199;})), Cyc_PP_nest( 2,
_temp2088->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2200=(
char*)""; struct _tagged_string _temp2201; _temp2201.curr= _temp2200; _temp2201.base=
_temp2200; _temp2201.last_plus_one= _temp2200 + 1; _temp2201;})): Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2202= _temp2088->fields; if( _temp2202 == 0){ _throw(
Null_Exception);} _temp2202->v;})))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2203=( char*)"};"; struct _tagged_string _temp2204;
_temp2204.curr= _temp2203; _temp2204.base= _temp2203; _temp2204.last_plus_one=
_temp2203 + 3; _temp2204;})));} goto _LL2040; _LL2052: s= Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc((
void*) _temp2090->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2205=(
char*)"xtunion "; struct _tagged_string _temp2206; _temp2206.curr= _temp2205;
_temp2206.base= _temp2205; _temp2206.last_plus_one= _temp2205 + 9; _temp2206;})),
Cyc_Absynpp_typedef_name2doc( _temp2090->name), _temp2090->fields == 0? Cyc_PP_text((
struct _tagged_string)({ char* _temp2207=( char*)";"; struct _tagged_string
_temp2208; _temp2208.curr= _temp2207; _temp2208.base= _temp2207; _temp2208.last_plus_one=
_temp2207 + 2; _temp2208;})): Cyc_Absynpp_cat4( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2209=( char*)" {"; struct _tagged_string _temp2210;
_temp2210.curr= _temp2209; _temp2210.base= _temp2209; _temp2210.last_plus_one=
_temp2209 + 3; _temp2210;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_tunionfields2doc( _temp2090->fields))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp2211=( char*)"};"; struct _tagged_string
_temp2212; _temp2212.curr= _temp2211; _temp2212.base= _temp2211; _temp2212.last_plus_one=
_temp2211 + 3; _temp2212;})))); goto _LL2040; _LL2054: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp2213=( char*)"let "; struct _tagged_string
_temp2214; _temp2214.curr= _temp2213; _temp2214.base= _temp2213; _temp2214.last_plus_one=
_temp2213 + 5; _temp2214;})), Cyc_Absynpp_pat2doc( _temp2100), Cyc_PP_text((
struct _tagged_string)({ char* _temp2215=( char*)" = "; struct _tagged_string
_temp2216; _temp2216.curr= _temp2215; _temp2216.base= _temp2215; _temp2216.last_plus_one=
_temp2215 + 4; _temp2216;})), Cyc_Absynpp_exp2doc( _temp2094), Cyc_PP_text((
struct _tagged_string)({ char* _temp2217=( char*)";"; struct _tagged_string
_temp2218; _temp2218.curr= _temp2217; _temp2218.base= _temp2217; _temp2218.last_plus_one=
_temp2217 + 2; _temp2218;}))); goto _LL2040; _LL2056: s= Cyc_Absynpp_cat7( Cyc_Absynpp_scope2doc(
_temp2109), Cyc_PP_text(( struct _tagged_string)({ char* _temp2219=( char*)"enum ";
struct _tagged_string _temp2220; _temp2220.curr= _temp2219; _temp2220.base=
_temp2219; _temp2220.last_plus_one= _temp2219 + 6; _temp2220;})), Cyc_Absynpp_qvar2doc(
_temp2107), Cyc_PP_text(( struct _tagged_string)({ char* _temp2221=( char*)" {";
struct _tagged_string _temp2222; _temp2222.curr= _temp2221; _temp2222.base=
_temp2221; _temp2222.last_plus_one= _temp2221 + 3; _temp2222;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_enumfields2doc( _temp2105))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp2223=( char*)"};";
struct _tagged_string _temp2224; _temp2224.curr= _temp2223; _temp2224.base=
_temp2223; _temp2224.last_plus_one= _temp2223 + 3; _temp2224;}))); goto _LL2040;
_LL2058: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2225=( char*)"typedef "; struct _tagged_string _temp2226; _temp2226.curr=
_temp2225; _temp2226.base= _temp2225; _temp2226.last_plus_one= _temp2225 + 9;
_temp2226;})), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual _temp2227;
_temp2227.q_const= 0; _temp2227.q_volatile= 0; _temp2227.q_restrict= 0;
_temp2227;}),( void*) _temp2111->defn,({ struct Cyc_Core_Opt* _temp2228=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2228->v=( void*)
Cyc_Absynpp_cat2( Cyc_Absynpp_typedef_name2doc( _temp2111->name), Cyc_Absynpp_tvars2doc(
_temp2111->tvs)); _temp2228;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2229=( char*)";"; struct _tagged_string _temp2230; _temp2230.curr=
_temp2229; _temp2230.base= _temp2229; _temp2230.last_plus_one= _temp2229 + 2;
_temp2230;}))); goto _LL2040; _LL2060: if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_add(
_temp2115);} s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2231=( char*)"namespace "; struct _tagged_string _temp2232; _temp2232.curr=
_temp2231; _temp2232.base= _temp2231; _temp2232.last_plus_one= _temp2231 + 11;
_temp2232;})), Cyc_Absynpp_textptr( _temp2115), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2233=( char*)" {"; struct _tagged_string _temp2234;
_temp2234.curr= _temp2233; _temp2234.base= _temp2233; _temp2234.last_plus_one=
_temp2233 + 3; _temp2234;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2235=( char*)""; struct _tagged_string _temp2236; _temp2236.curr=
_temp2235; _temp2236.base= _temp2235; _temp2236.last_plus_one= _temp2235 + 1;
_temp2236;}), _temp2113), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2237=( char*)"}"; struct _tagged_string _temp2238; _temp2238.curr=
_temp2237; _temp2238.base= _temp2237; _temp2238.last_plus_one= _temp2237 + 2;
_temp2238;}))); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2040; _LL2062: if( Cyc_Absynpp_print_using_stmts){ s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2239=( char*)"using "; struct
_tagged_string _temp2240; _temp2240.curr= _temp2239; _temp2240.base= _temp2239;
_temp2240.last_plus_one= _temp2239 + 7; _temp2240;})), Cyc_Absynpp_qvar2doc(
_temp2119), Cyc_PP_text(( struct _tagged_string)({ char* _temp2241=( char*)" {";
struct _tagged_string _temp2242; _temp2242.curr= _temp2241; _temp2242.base=
_temp2241; _temp2242.last_plus_one= _temp2241 + 3; _temp2242;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2243=( char*)""; struct _tagged_string
_temp2244; _temp2244.curr= _temp2243; _temp2244.base= _temp2243; _temp2244.last_plus_one=
_temp2243 + 1; _temp2244;}), _temp2117), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2245=( char*)"}"; struct _tagged_string _temp2246;
_temp2246.curr= _temp2245; _temp2246.base= _temp2245; _temp2246.last_plus_one=
_temp2245 + 2; _temp2246;})));} else{ s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2247=( char*)"/* using "; struct _tagged_string
_temp2248; _temp2248.curr= _temp2247; _temp2248.base= _temp2247; _temp2248.last_plus_one=
_temp2247 + 10; _temp2248;})), Cyc_Absynpp_qvar2doc( _temp2119), Cyc_PP_text((
struct _tagged_string)({ char* _temp2249=( char*)" { */"; struct _tagged_string
_temp2250; _temp2250.curr= _temp2249; _temp2250.base= _temp2249; _temp2250.last_plus_one=
_temp2249 + 6; _temp2250;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2251=( char*)""; struct _tagged_string _temp2252; _temp2252.curr=
_temp2251; _temp2252.base= _temp2251; _temp2252.last_plus_one= _temp2251 + 1;
_temp2252;}), _temp2117), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2253=( char*)"/* } */"; struct _tagged_string _temp2254; _temp2254.curr=
_temp2253; _temp2254.base= _temp2253; _temp2254.last_plus_one= _temp2253 + 8;
_temp2254;})));} goto _LL2040; _LL2064: if( Cyc_Absynpp_print_externC_stmts){ s=
Cyc_Absynpp_cat5( Cyc_PP_text(( struct _tagged_string)({ char* _temp2255=( char*)"extern \"C\" {";
struct _tagged_string _temp2256; _temp2256.curr= _temp2255; _temp2256.base=
_temp2255; _temp2256.last_plus_one= _temp2255 + 13; _temp2256;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2257=( char*)""; struct _tagged_string
_temp2258; _temp2258.curr= _temp2257; _temp2258.base= _temp2257; _temp2258.last_plus_one=
_temp2257 + 1; _temp2258;}), _temp2121), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2259=( char*)"}"; struct _tagged_string _temp2260;
_temp2260.curr= _temp2259; _temp2260.base= _temp2259; _temp2260.last_plus_one=
_temp2259 + 2; _temp2260;})));} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2261=( char*)"/* extern \"C\" { */"; struct
_tagged_string _temp2262; _temp2262.curr= _temp2261; _temp2262.base= _temp2261;
_temp2262.last_plus_one= _temp2261 + 19; _temp2262;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2263=( char*)""; struct _tagged_string
_temp2264; _temp2264.curr= _temp2263; _temp2264.base= _temp2263; _temp2264.last_plus_one=
_temp2263 + 1; _temp2264;}), _temp2121), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2265=( char*)"/* } */"; struct _tagged_string
_temp2266; _temp2266.curr= _temp2265; _temp2266.base= _temp2265; _temp2266.last_plus_one=
_temp2265 + 8; _temp2266;})));} goto _LL2040; _LL2040:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void* sc){ void* _temp2267= sc; _LL2269: if(( int)
_temp2267 == Cyc_Absyn_Static){ goto _LL2270;} else{ goto _LL2271;} _LL2271: if((
int) _temp2267 == Cyc_Absyn_Public){ goto _LL2272;} else{ goto _LL2273;} _LL2273:
if(( int) _temp2267 == Cyc_Absyn_Extern){ goto _LL2274;} else{ goto _LL2275;}
_LL2275: if(( int) _temp2267 == Cyc_Absyn_ExternC){ goto _LL2276;} else{ goto
_LL2277;} _LL2277: if(( int) _temp2267 == Cyc_Absyn_Abstract){ goto _LL2278;}
else{ goto _LL2268;} _LL2270: return Cyc_PP_text(( struct _tagged_string)({ char*
_temp2279=( char*)"static "; struct _tagged_string _temp2280; _temp2280.curr=
_temp2279; _temp2280.base= _temp2279; _temp2280.last_plus_one= _temp2279 + 8;
_temp2280;})); _LL2272: return Cyc_PP_nil_doc(); _LL2274: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2281=( char*)"extern "; struct
_tagged_string _temp2282; _temp2282.curr= _temp2281; _temp2282.base= _temp2281;
_temp2282.last_plus_one= _temp2281 + 8; _temp2282;})); _LL2276: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2283=( char*)"extern \"C\" "; struct
_tagged_string _temp2284; _temp2284.curr= _temp2283; _temp2284.base= _temp2283;
_temp2284.last_plus_one= _temp2283 + 12; _temp2284;})); _LL2278: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2285=( char*)"abstract "; struct
_tagged_string _temp2286; _temp2286.curr= _temp2285; _temp2286.base= _temp2285;
_temp2286.last_plus_one= _temp2285 + 10; _temp2286;})); _LL2268:;} int Cyc_Absynpp_exists_temp_tvar_in_effect(
void* t){ void* _temp2287= t; struct Cyc_Absyn_Tvar* _temp2295; struct Cyc_List_List*
_temp2297; _LL2289: if(( unsigned int) _temp2287 > 4u?(( struct _tunion_struct*)
_temp2287)->tag == Cyc_Absyn_VarType_tag: 0){ _LL2296: _temp2295=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp2287)->f1; goto _LL2290;} else{ goto
_LL2291;} _LL2291: if(( unsigned int) _temp2287 > 4u?(( struct _tunion_struct*)
_temp2287)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL2298: _temp2297=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp2287)->f1; goto _LL2292;} else{ goto
_LL2293;} _LL2293: goto _LL2294; _LL2290: return Cyc_Tcutil_is_temp_tvar(
_temp2295); _LL2292: return(( int(*)( int(* pred)( void*), struct Cyc_List_List*
x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect, _temp2297);
_LL2294: return 0; _LL2288:;} struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual
tq, void* t){ void* _temp2299= t; struct Cyc_Absyn_Exp* _temp2313; struct Cyc_Absyn_Tqual
_temp2315; void* _temp2317; struct Cyc_Absyn_PtrInfo _temp2319; struct Cyc_Absyn_Conref*
_temp2321; struct Cyc_Absyn_Tqual _temp2323; struct Cyc_Absyn_Conref* _temp2325;
void* _temp2327; void* _temp2329; struct Cyc_Absyn_FnInfo _temp2331; struct Cyc_List_List*
_temp2333; int _temp2335; struct Cyc_List_List* _temp2337; void* _temp2339;
struct Cyc_Core_Opt* _temp2341; struct Cyc_List_List* _temp2343; int _temp2345;
struct Cyc_Core_Opt* _temp2347; void* _temp2349; struct Cyc_Core_Opt* _temp2351;
struct Cyc_List_List* _temp2353; struct _tuple0* _temp2355; _LL2301: if((
unsigned int) _temp2299 > 4u?(( struct _tunion_struct*) _temp2299)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2318: _temp2317=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2299)->f1;
goto _LL2316; _LL2316: _temp2315=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2299)->f2; goto _LL2314; _LL2314: _temp2313=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2299)->f3; goto _LL2302;} else{ goto
_LL2303;} _LL2303: if(( unsigned int) _temp2299 > 4u?(( struct _tunion_struct*)
_temp2299)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2320: _temp2319=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2299)->f1;
_LL2330: _temp2329=( void*) _temp2319.elt_typ; goto _LL2328; _LL2328: _temp2327=(
void*) _temp2319.rgn_typ; goto _LL2326; _LL2326: _temp2325=( struct Cyc_Absyn_Conref*)
_temp2319.nullable; goto _LL2324; _LL2324: _temp2323=( struct Cyc_Absyn_Tqual)
_temp2319.tq; goto _LL2322; _LL2322: _temp2321=( struct Cyc_Absyn_Conref*)
_temp2319.bounds; goto _LL2304;} else{ goto _LL2305;} _LL2305: if(( unsigned int)
_temp2299 > 4u?(( struct _tunion_struct*) _temp2299)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2332: _temp2331=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2299)->f1; _LL2344: _temp2343=( struct Cyc_List_List*) _temp2331.tvars;
goto _LL2342; _LL2342: _temp2341=( struct Cyc_Core_Opt*) _temp2331.effect; goto
_LL2340; _LL2340: _temp2339=( void*) _temp2331.ret_typ; goto _LL2338; _LL2338:
_temp2337=( struct Cyc_List_List*) _temp2331.args; goto _LL2336; _LL2336:
_temp2335=( int) _temp2331.varargs; goto _LL2334; _LL2334: _temp2333=( struct
Cyc_List_List*) _temp2331.attributes; goto _LL2306;} else{ goto _LL2307;}
_LL2307: if(( unsigned int) _temp2299 > 4u?(( struct _tunion_struct*) _temp2299)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL2350: _temp2349=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2299)->f1; goto _LL2348; _LL2348: _temp2347=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp2299)->f2; goto _LL2346; _LL2346: _temp2345=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp2299)->f3; goto _LL2308;} else{ goto
_LL2309;} _LL2309: if(( unsigned int) _temp2299 > 4u?(( struct _tunion_struct*)
_temp2299)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL2356: _temp2355=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2299)->f1; goto _LL2354;
_LL2354: _temp2353=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2299)->f2; goto _LL2352; _LL2352: _temp2351=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2299)->f3; goto _LL2310;} else{ goto
_LL2311;} _LL2311: goto _LL2312; _LL2302: { struct Cyc_List_List* _temp2359;
void* _temp2361; struct Cyc_Absyn_Tqual _temp2363; struct _tuple4 _temp2357= Cyc_Absynpp_to_tms(
_temp2315, _temp2317); _LL2364: _temp2363= _temp2357.f1; goto _LL2362; _LL2362:
_temp2361= _temp2357.f2; goto _LL2360; _LL2360: _temp2359= _temp2357.f3; goto
_LL2358; _LL2358: { void* tm; if( _temp2313 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;}
else{ tm=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct* _temp2365=( struct
Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct));
_temp2365[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct _temp2366; _temp2366.tag=
Cyc_Absyn_ConstArray_mod_tag; _temp2366.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2367= _temp2313; if( _temp2367 == 0){ _throw( Null_Exception);} _temp2367;});
_temp2366;}); _temp2365;});} return({ struct _tuple4 _temp2368; _temp2368.f1=
_temp2363; _temp2368.f2= _temp2361; _temp2368.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2369=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2369->hd=( void*) tm; _temp2369->tl= _temp2359;
_temp2369;}); _temp2368;});}} _LL2304: { struct Cyc_List_List* _temp2372; void*
_temp2374; struct Cyc_Absyn_Tqual _temp2376; struct _tuple4 _temp2370= Cyc_Absynpp_to_tms(
_temp2323, _temp2329); _LL2377: _temp2376= _temp2370.f1; goto _LL2375; _LL2375:
_temp2374= _temp2370.f2; goto _LL2373; _LL2373: _temp2372= _temp2370.f3; goto
_LL2371; _LL2371: { void* ps;{ struct _tuple8 _temp2379=({ struct _tuple8
_temp2378; _temp2378.f1=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2325))->v; _temp2378.f2=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2321))->v; _temp2378;}); void* _temp2389; void* _temp2391; struct Cyc_Absyn_Exp*
_temp2393; void* _temp2395; int _temp2397; void* _temp2399; void* _temp2401;
struct Cyc_Absyn_Exp* _temp2403; void* _temp2405; int _temp2407; void* _temp2409;
void* _temp2411; void* _temp2413; _LL2381: _LL2396: _temp2395= _temp2379.f1; if((
unsigned int) _temp2395 > 1u?(( struct _tunion_struct*) _temp2395)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL2398: _temp2397=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2395)->f1;
if( _temp2397 == 1){ goto _LL2390;} else{ goto _LL2383;}} else{ goto _LL2383;}
_LL2390: _temp2389= _temp2379.f2; if(( unsigned int) _temp2389 > 1u?(( struct
_tunion_struct*) _temp2389)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2392:
_temp2391=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2389)->f1; if((
unsigned int) _temp2391 > 1u?(( struct _tunion_struct*) _temp2391)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2394: _temp2393=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2391)->f1; goto _LL2382;} else{ goto _LL2383;}} else{ goto _LL2383;}
_LL2383: _LL2406: _temp2405= _temp2379.f1; if(( unsigned int) _temp2405 > 1u?((
struct _tunion_struct*) _temp2405)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2408:
_temp2407=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2405)->f1; if(
_temp2407 == 0){ goto _LL2400;} else{ goto _LL2385;}} else{ goto _LL2385;}
_LL2400: _temp2399= _temp2379.f2; if(( unsigned int) _temp2399 > 1u?(( struct
_tunion_struct*) _temp2399)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2402:
_temp2401=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2399)->f1; if((
unsigned int) _temp2401 > 1u?(( struct _tunion_struct*) _temp2401)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2404: _temp2403=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2401)->f1; goto _LL2384;} else{ goto _LL2385;}} else{ goto _LL2385;}
_LL2385: _LL2414: _temp2413= _temp2379.f1; goto _LL2410; _LL2410: _temp2409=
_temp2379.f2; if(( unsigned int) _temp2409 > 1u?(( struct _tunion_struct*)
_temp2409)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2412: _temp2411=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2409)->f1; if(( int) _temp2411 == Cyc_Absyn_Unknown_b){
goto _LL2386;} else{ goto _LL2387;}} else{ goto _LL2387;} _LL2387: goto _LL2388;
_LL2382: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2415=( struct
Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2415[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2416; _temp2416.tag=
Cyc_Absyn_Nullable_ps_tag; _temp2416.f1= _temp2393; _temp2416;}); _temp2415;});
goto _LL2380; _LL2384: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2417=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2417[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2418; _temp2418.tag= Cyc_Absyn_NonNullable_ps_tag; _temp2418.f1= _temp2403;
_temp2418;}); _temp2417;}); goto _LL2380; _LL2386: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2380; _LL2388: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2380;
_LL2380:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2421=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2421[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2422; _temp2422.tag=
Cyc_Absyn_Pointer_mod_tag; _temp2422.f1=( void*) ps; _temp2422.f2=( void*)
_temp2327; _temp2422.f3= tq; _temp2422;}); _temp2421;}); return({ struct _tuple4
_temp2419; _temp2419.f1= _temp2376; _temp2419.f2= _temp2374; _temp2419.f3=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2420=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2420->hd=( void*) tm; _temp2420->tl=
_temp2372; _temp2420;}); _temp2419;});}}} _LL2306: if( ! Cyc_Absynpp_print_all_tvars){
if( _temp2341 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)({ struct
Cyc_Core_Opt* _temp2423= _temp2341; if( _temp2423 == 0){ _throw( Null_Exception);}
_temp2423->v;}))){ _temp2341= 0; _temp2343= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2343);}}{ struct Cyc_List_List* _temp2426;
void* _temp2428; struct Cyc_Absyn_Tqual _temp2430; struct _tuple4 _temp2424= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp2339); _LL2431: _temp2430= _temp2424.f1; goto
_LL2429; _LL2429: _temp2428= _temp2424.f2; goto _LL2427; _LL2427: _temp2426=
_temp2424.f3; goto _LL2425; _LL2425: { struct Cyc_List_List* tms= _temp2426; if(
_temp2333 != 0){ tms=({ struct Cyc_List_List* _temp2432=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2432->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2433=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2433[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2434; _temp2434.tag= Cyc_Absyn_Attributes_mod_tag;
_temp2434.f1= 0; _temp2434.f2= _temp2333; _temp2434;}); _temp2433;})); _temp2432->tl=
tms; _temp2432;});} tms=({ struct Cyc_List_List* _temp2435=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2435->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2436=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2436[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2437; _temp2437.tag= Cyc_Absyn_Function_mod_tag;
_temp2437.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2438=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2438[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2439; _temp2439.tag= Cyc_Absyn_WithTypes_tag;
_temp2439.f1= _temp2337; _temp2439.f2= _temp2335; _temp2439.f3= _temp2341;
_temp2439;}); _temp2438;})); _temp2437;}); _temp2436;})); _temp2435->tl= tms;
_temp2435;}); if( _temp2343 != 0){ tms=({ struct Cyc_List_List* _temp2440=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2440->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2441=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2441[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2442; _temp2442.tag= Cyc_Absyn_TypeParams_mod_tag;
_temp2442.f1= _temp2343; _temp2442.f2= 0; _temp2442.f3= 1; _temp2442;});
_temp2441;})); _temp2440->tl= tms; _temp2440;});} return({ struct _tuple4
_temp2443; _temp2443.f1= _temp2430; _temp2443.f2= _temp2428; _temp2443.f3= tms;
_temp2443;});}} _LL2308: if( _temp2347 == 0){ return({ struct _tuple4 _temp2444;
_temp2444.f1= tq; _temp2444.f2= t; _temp2444.f3= 0; _temp2444;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2445= _temp2347; if(
_temp2445 == 0){ _throw( Null_Exception);} _temp2445->v;}));} _LL2310: if(
_temp2351 == 0? 1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4
_temp2446; _temp2446.f1= tq; _temp2446.f2= t; _temp2446.f3= 0; _temp2446;});}
else{ return Cyc_Absynpp_to_tms( tq,( void*)({ struct Cyc_Core_Opt* _temp2447=
_temp2351; if( _temp2447 == 0){ _throw( Null_Exception);} _temp2447->v;}));}
_LL2312: return({ struct _tuple4 _temp2448; _temp2448.f1= tq; _temp2448.f2= t;
_temp2448.f3= 0; _temp2448;}); _LL2300:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp2451; void* _temp2453; struct Cyc_Absyn_Tqual _temp2455; struct _tuple4
_temp2449= Cyc_Absynpp_to_tms( tq, t); _LL2456: _temp2455= _temp2449.f1; goto
_LL2454; _LL2454: _temp2453= _temp2449.f2; goto _LL2452; _LL2452: _temp2451=
_temp2449.f3; goto _LL2450; _LL2450: _temp2451=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2451); if( _temp2451 == 0?
dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc( _temp2455), Cyc_Absynpp_ntyp2doc(
_temp2453));} else{ return Cyc_Absynpp_cat4( Cyc_Absynpp_tqual2doc( _temp2455),
Cyc_Absynpp_ntyp2doc( _temp2453), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2457=( char*)" "; struct _tagged_string _temp2458; _temp2458.curr=
_temp2457; _temp2458.base= _temp2457; _temp2458.last_plus_one= _temp2457 + 2;
_temp2458;})), Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)({
struct Cyc_Core_Opt* _temp2459= dopt; if( _temp2459 == 0){ _throw(
Null_Exception);} _temp2459->v;}), _temp2451));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return Cyc_Absynpp_cat5(
Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2460=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2460->v=(
void*) Cyc_Absynpp_textptr( f->name); _temp2460;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2461=( char*)":"; struct _tagged_string _temp2462;
_temp2462.curr= _temp2461; _temp2462.base= _temp2461; _temp2462.last_plus_one=
_temp2461 + 2; _temp2462;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp2463= f->width; if( _temp2463 == 0){ _throw(
Null_Exception);} _temp2463->v;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2464=( char*)";"; struct _tagged_string
_temp2465; _temp2465.curr= _temp2464; _temp2465.base= _temp2464; _temp2465.last_plus_one=
_temp2464 + 2; _temp2465;})));} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2466=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2466->v=( void*) Cyc_Absynpp_textptr(
f->name); _temp2466;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2467=( char*)";"; struct _tagged_string
_temp2468; _temp2468.curr= _temp2467; _temp2468.base= _temp2467; _temp2468.last_plus_one=
_temp2467 + 2; _temp2468;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,( struct _tagged_string)({ char*
_temp2469=( char*)""; struct _tagged_string _temp2470; _temp2470.curr= _temp2469;
_temp2470.base= _temp2469; _temp2470.last_plus_one= _temp2469 + 1; _temp2470;}),
fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc( struct Cyc_Absyn_Tunionfield*
f){ return Cyc_Absynpp_cat3( Cyc_Absynpp_typedef_name2doc( f->name), f->tvs == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs), f->typs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,( struct _tagged_string)({ char*
_temp2471=( char*)","; struct _tagged_string _temp2472; _temp2472.curr=
_temp2471; _temp2472.base= _temp2471; _temp2472.last_plus_one= _temp2471 + 2;
_temp2472;}), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl=({ struct Cyc_List_List*
_temp2473= tdl; if( _temp2473 == 0){ _throw( Null_Exception);} _temp2473->tl;})){
Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp2474= tdl; if( _temp2474 == 0){ _throw( Null_Exception);} _temp2474->hd;})),
72, f); fprintf( f,"\n");}} struct _tagged_string Cyc_Absynpp_decllist2string(
struct Cyc_List_List* tdl){ return Cyc_PP_string_of_doc( Cyc_PP_seql(( struct
_tagged_string)({ char* _temp2475=( char*)""; struct _tagged_string _temp2476;
_temp2476.curr= _temp2475; _temp2476.base= _temp2475; _temp2476.last_plus_one=
_temp2475 + 1; _temp2476;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
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