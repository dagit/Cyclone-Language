#include "cyc_include.h"

 struct _tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct
_tuple1{ void* f1; struct _tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple4{ struct Cyc_Absyn_Tqual* f1;
void* f2; } ; struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct
_tuple6{ int f1; void* f2; } ; struct _tuple7{ void* f1; void* f2; } ; struct
_tuple8{ void* f1; struct Cyc_List_List* f2; } ; struct _tuple9{ struct _tuple8*
f1; struct _tuple8* f2; } ; struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; } ; struct _tuple11{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual* f2;
void* f3; } ; struct _tuple12{ void* f1; void* f2; void* f3; } ; struct _tuple13{
struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; } ; struct _tuple14{ struct
_tuple13* f1; void* f2; } ; struct _tuple15{ struct Cyc_Absyn_Structfield* f1;
int f2; } ; struct _tuple16{ struct Cyc_List_List* f1; void* f2; } ; struct
_tuple17{ struct Cyc_Absyn_Structfield* f1; void* f2; } ; struct _tuple18{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; typedef int Cyc_ptrdiff_t;
typedef unsigned int Cyc_size_t; typedef unsigned short Cyc_wchar_t; typedef
unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short Cyc_u_short;
typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef
unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int
Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int
tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec
it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t; typedef
unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern int Cyc_Core_intcmp( int, int);
extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t; extern int Cyc_Stdio_fflush(
struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length(
struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern void Cyc_List_iter2(
void(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_List_exists_c( int(* pred)( void*, void*), void*
env, struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_zip( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct _tuple0 Cyc_List_split(
struct Cyc_List_List* x); extern void* Cyc_List_assoc_cmp( int(* compare)( void*,
void*), struct Cyc_List_List* l, void* x); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; extern struct Cyc_List_List*
Cyc_Position_strings_of_segments( struct Cyc_List_List*); typedef void* Cyc_Position_Error_kind;
static const unsigned int Cyc_Position_Lex= 0; static const unsigned int Cyc_Position_Parse=
1; static const unsigned int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_string); extern char Cyc_Position_Nocontext_tag[
10u]; struct Cyc_Position_Nocontext_struct{ char* tag; } ; extern void Cyc_Position_post_error(
struct Cyc_Position_Error*); typedef struct _tagged_string* Cyc_Absyn_field_name_t;
typedef struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string*
Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t;
typedef struct _tuple1* Cyc_Absyn_qvar_t; typedef struct _tuple1* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1* Cyc_Absyn_typedef_name_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t;
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
attributes; } ; struct Cyc_Absyn_TunionInfo{ struct _tuple1* name; struct Cyc_List_List*
targs; void* rgn; struct Cyc_Absyn_Tuniondecl* tud; } ; struct Cyc_Absyn_XTunionInfo{
struct _tuple1* name; void* rgn; struct Cyc_Absyn_XTuniondecl* xtud; } ; struct
Cyc_Absyn_TunionFieldInfo{ struct _tuple1* name; struct Cyc_List_List* targs;
struct _tuple1* fname; struct Cyc_Absyn_Tuniondecl* tud; struct Cyc_Absyn_Tunionfield*
tufd; } ; struct Cyc_Absyn_XTunionFieldInfo{ struct _tuple1* name; struct
_tuple1* fname; struct Cyc_Absyn_XTuniondecl* xtud; struct Cyc_Absyn_Tunionfield*
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
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType_tag=
12; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType_tag=
13; struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType_tag= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType_tag= 15; struct
Cyc_Absyn_TypedefType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
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
int tag; struct _tuple1* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e_tag=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple1* f1; } ; static
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
23; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e_tag= 24; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e_tag=
25; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e_tag=
26; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_Tunion_e_tag= 27; struct Cyc_Absyn_Tunion_e_struct{ int tag;
struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ; static
const int Cyc_Absyn_XTunion_e_tag= 28; struct Cyc_Absyn_XTunion_e_struct{ int
tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_XTuniondecl*
f3; struct Cyc_Absyn_Tunionfield* f4; } ; static const int Cyc_Absyn_Enum_e_tag=
29; struct Cyc_Absyn_Enum_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
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
int tag; struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s_tag=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s_tag= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s_tag= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s_tag= 8; struct Cyc_Absyn_For_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct
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
int tag; struct Cyc_Absyn_Stmt* f1; struct _tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s_tag=
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
struct Cyc_Absyn_Tunion_p_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Tuniondecl*
f5; struct Cyc_Absyn_Tunionfield* f6; } ; static const int Cyc_Absyn_XTunion_p_tag=
9; struct Cyc_Absyn_XTunion_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_XTuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_p_tag= 10; struct Cyc_Absyn_Enum_p_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_UnknownId_p_tag= 11; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_UnknownCall_p_tag=
12; struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; static const int Cyc_Absyn_UnknownFields_p_tag=
13; struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple1* f1; struct
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
Cyc_Absyn_Vardecl{ void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual* tq;
void* type; struct Cyc_Absyn_Exp* initializer; int shadow; struct Cyc_Core_Opt*
region; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc;
int is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_Core_Opt*
width; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structdecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Uniondecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Tunionfield{
struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_List_List* typs;
struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{ void* sc;
struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_XTuniondecl{ void* sc; struct _tuple1* name; struct
Cyc_List_List* fields; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; static const
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
_tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d_tag=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement_tag= 0; struct Cyc_Absyn_ArrayElement_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName_tag=
1; struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot_tag[ 11u]; struct Cyc_Absyn_EmptyAnnot_struct{
char* tag; } ; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*);
extern int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*);
extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_ulong_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t; extern void*
Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual* tq, void* b);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_string*); extern struct _tuple4* Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*, int); extern struct _tagged_string Cyc_Absyn_attribute2string(
void*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_kind2string( void*); extern
struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); struct
Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set*
Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[
7u]; struct Cyc_Set_Absent_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; typedef void* Cyc_Tcenv_Resolved;
static const int Cyc_Tcenv_VarRes_tag= 0; struct Cyc_Tcenv_VarRes_struct{ int
tag; void* f1; } ; static const int Cyc_Tcenv_StructRes_tag= 1; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes_tag=
2; struct Cyc_Tcenv_TunionRes_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; static const int Cyc_Tcenv_XTunionRes_tag=
3; struct Cyc_Tcenv_XTunionRes_struct{ int tag; struct Cyc_Absyn_XTuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; static const int Cyc_Tcenv_EnumRes_tag= 4;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; typedef void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict*
xtuniondecls; struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* typedefs;
struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List* availables; } ; typedef
struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv; typedef struct
Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; typedef void* Cyc_Tcenv_Jumpee; static const
unsigned int Cyc_Tcenv_NotLoop_j= 0; static const unsigned int Cyc_Tcenv_CaseEnd_j=
1; static const unsigned int Cyc_Tcenv_FnEnd_j= 2; static const int Cyc_Tcenv_Stmt_j_tag=
0; struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
typedef void* Cyc_Tcenv_jumpee_t; typedef void* Cyc_Tcenv_Frames; static const
int Cyc_Tcenv_Outermost_tag= 0; struct Cyc_Tcenv_Outermost_struct{ int tag; void*
f1; } ; static const int Cyc_Tcenv_Frame_tag= 1; struct Cyc_Tcenv_Frame_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden_tag= 2;
struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void* f2; } ; typedef void*
Cyc_Tcenv_frames_t; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List* ns; struct Cyc_Dict_Dict*
ae; struct Cyc_Core_Opt* le; } ; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t;
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); extern struct _tagged_string Cyc_String_strconcat(
struct _tagged_string, struct _tagged_string); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag; } ; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern
void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_flush_warnings(); extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set;
extern void* Cyc_Tcutil_typ_kind( void* t); extern void* Cyc_Tcutil_compress(
void* t); extern void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable( void*); extern
int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_integral_promote(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_arithmetic_promote(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_comparison_promote(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcutil_max_arithmetic_type(
void*, void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern int Cyc_Tcutil_subset_effect( int
set_to_empty, void* e1, void* e2); extern int Cyc_Tcutil_region_in_effect( int
constrain, void* r, void* e); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_Position_Segment*); extern struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign( struct
Cyc_Absyn_Exp*); extern struct Cyc_List_List* Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*,
void* k, void*); extern void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct
_tagged_string err_msg); extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*,
struct Cyc_List_List*); extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual* tq1, struct Cyc_Absyn_Tqual* tq2); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct Cyc_Position_Segment* loc, struct
Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*, void* t,
struct Cyc_Absyn_Exp* e); extern struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k); extern int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*); extern int
Cyc_Tcutil_same_atts( struct Cyc_List_List*, struct Cyc_List_List*); extern int
Cyc_Tcutil_bits_only( void* t); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e); extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*,
void**, struct Cyc_Absyn_Exp*); char Cyc_Tcutil_TypeErr_tag[ 8u]="TypeErr";
extern void Cyc_Tcutil_unify_it( void* t1, void* t2); void Cyc_Tcutil_terr(
struct Cyc_Position_Segment* loc, struct _tagged_string s){ Cyc_Position_post_error(
Cyc_Position_mk_err_elab( loc, s));} void* Cyc_Tcutil_impos( struct
_tagged_string msg){({ struct _tagged_string _temp0= msg; fprintf( Cyc_Stdio_stderr,"Error: %.*s\n",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Tcutil_TypeErr_struct* _temp1=( struct Cyc_Tcutil_TypeErr_struct*)
GC_malloc( sizeof( struct Cyc_Tcutil_TypeErr_struct)); _temp1[ 0]=({ struct Cyc_Tcutil_TypeErr_struct
_temp2; _temp2.tag= Cyc_Tcutil_TypeErr_tag; _temp2;}); _temp1;}));} static
struct _tagged_string Cyc_Tcutil_tvar2string( struct Cyc_Absyn_Tvar* tv){ return*
tv->name;} void Cyc_Tcutil_print_tvars( struct Cyc_List_List* tvs){ for( 0; tvs
!= 0; tvs= tvs->tl){({ struct _tagged_string _temp3= Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*) tvs->hd); struct _tagged_string _temp4= Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); fprintf( Cyc_Stdio_stderr,"%.*s::%.*s ",
_temp3.last_plus_one - _temp3.curr, _temp3.curr, _temp4.last_plus_one - _temp4.curr,
_temp4.curr);});} fprintf( Cyc_Stdio_stderr,"\n"); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);} static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0;
static struct Cyc_List_List* Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn(
struct Cyc_Position_Segment* sg, struct _tagged_string msg){ Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp5=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp5->hd=( void*) sg; _temp5->tl= Cyc_Tcutil_warning_segs;
_temp5;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp6=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6->hd=( void*)(( struct
_tagged_string*)({ struct _tagged_string* _temp7=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp7[ 0]= msg; _temp7;}));
_temp6->tl= Cyc_Tcutil_warning_msgs; _temp6;});} void Cyc_Tcutil_flush_warnings(){
if( Cyc_Tcutil_warning_segs == 0){ return;} fprintf( Cyc_Stdio_stderr,"***Warnings***\n");{
struct Cyc_List_List* seg_strs= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while(
Cyc_Tcutil_warning_msgs != 0) {({ struct _tagged_string _temp8=*(( struct
_tagged_string*) seg_strs->hd); struct _tagged_string _temp9=*(( struct
_tagged_string*) Cyc_Tcutil_warning_msgs->hd); fprintf( Cyc_Stdio_stderr,"%.*s: %.*s\n",
_temp8.last_plus_one - _temp8.curr, _temp8.curr, _temp9.last_plus_one - _temp9.curr,
_temp9.curr);}); seg_strs= seg_strs->tl; Cyc_Tcutil_warning_msgs= Cyc_Tcutil_warning_msgs->tl;}
fprintf( Cyc_Stdio_stderr,"**************\n"); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set= 0; void* Cyc_Tcutil_compress(
void* t){ void* _temp10= t; int _temp22; struct Cyc_Core_Opt* _temp24; void*
_temp26; int _temp28; struct Cyc_Core_Opt* _temp30; struct Cyc_Core_Opt**
_temp32; void* _temp33; struct Cyc_Core_Opt* _temp35; struct Cyc_List_List*
_temp37; struct _tuple1* _temp39; struct Cyc_Core_Opt* _temp41; struct Cyc_Core_Opt**
_temp43; struct Cyc_List_List* _temp44; struct _tuple1* _temp46; _LL12: if((
unsigned int) _temp10 > 4u?(( struct _tunion_struct*) _temp10)->tag == Cyc_Absyn_Evar_tag:
0){ _LL27: _temp26=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp10)->f1; goto
_LL25; _LL25: _temp24=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp10)->f2; if( _temp24 == 0){ goto _LL23;} else{ goto _LL14;} _LL23: _temp22=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp10)->f3; goto _LL13;} else{ goto
_LL14;} _LL14: if(( unsigned int) _temp10 > 4u?(( struct _tunion_struct*)
_temp10)->tag == Cyc_Absyn_Evar_tag: 0){ _LL34: _temp33=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp10)->f1; goto _LL31; _LL31: _temp30=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp10)->f2; _temp32=&(( struct Cyc_Absyn_Evar_struct*) _temp10)->f2; goto
_LL29; _LL29: _temp28=( int)(( struct Cyc_Absyn_Evar_struct*) _temp10)->f3; goto
_LL15;} else{ goto _LL16;} _LL16: if(( unsigned int) _temp10 > 4u?(( struct
_tunion_struct*) _temp10)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL40: _temp39=(
struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp10)->f1; goto
_LL38; _LL38: _temp37=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp10)->f2; goto _LL36; _LL36: _temp35=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp10)->f3; if( _temp35 == 0){ goto _LL17;} else{ goto _LL18;}} else{ goto
_LL18;} _LL18: if(( unsigned int) _temp10 > 4u?(( struct _tunion_struct*)
_temp10)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL47: _temp46=( struct _tuple1*)((
struct Cyc_Absyn_TypedefType_struct*) _temp10)->f1; goto _LL45; _LL45: _temp44=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*) _temp10)->f2;
goto _LL42; _LL42: _temp41=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp10)->f3; _temp43=&(( struct Cyc_Absyn_TypedefType_struct*) _temp10)->f3;
goto _LL19;} else{ goto _LL20;} _LL20: goto _LL21; _LL13: return t; _LL15: {
void* t2= Cyc_Tcutil_compress(( void*)(* _temp32)->v); if( t2 !=( void*)(*
_temp32)->v){* _temp32=({ struct Cyc_Core_Opt* _temp48=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp48->v=( void*) t2; _temp48;});}
return t2;} _LL17: return t; _LL19: { void* t2= Cyc_Tcutil_compress(( void*)(*
_temp43)->v); if( t2 !=( void*)(* _temp43)->v){* _temp43=({ struct Cyc_Core_Opt*
_temp49=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp49->v=( void*) t2; _temp49;});} return t2;} _LL21: return t; _LL11:;}
static int Cyc_Tcutil_kind_leq( void* k1, void* k2){ if( k1 == k2){ return 1;}{
struct _tuple7 _temp51=({ struct _tuple7 _temp50; _temp50.f1= k1; _temp50.f2= k2;
_temp50;}); void* _temp61; void* _temp63; void* _temp65; void* _temp67; void*
_temp69; void* _temp71; _LL53: _LL64: _temp63= _temp51.f1; if(( int) _temp63 ==
Cyc_Absyn_BoxKind){ goto _LL62;} else{ goto _LL55;} _LL62: _temp61= _temp51.f2;
if(( int) _temp61 == Cyc_Absyn_MemKind){ goto _LL54;} else{ goto _LL55;} _LL55:
_LL68: _temp67= _temp51.f1; if(( int) _temp67 == Cyc_Absyn_BoxKind){ goto _LL66;}
else{ goto _LL57;} _LL66: _temp65= _temp51.f2; if(( int) _temp65 == Cyc_Absyn_AnyKind){
goto _LL56;} else{ goto _LL57;} _LL57: _LL72: _temp71= _temp51.f1; if(( int)
_temp71 == Cyc_Absyn_MemKind){ goto _LL70;} else{ goto _LL59;} _LL70: _temp69=
_temp51.f2; if(( int) _temp69 == Cyc_Absyn_AnyKind){ goto _LL58;} else{ goto
_LL59;} _LL59: goto _LL60; _LL54: return 1; _LL56: return 1; _LL58: return 1;
_LL60: return 0; _LL52:;}} void* Cyc_Tcutil_typ_kind( void* t){ void* _temp73=
Cyc_Tcutil_compress( t); int _temp123; struct Cyc_Core_Opt* _temp125; void*
_temp127; struct Cyc_Absyn_Tvar* _temp129; void* _temp131; void* _temp133;
struct Cyc_Absyn_FnInfo _temp135; void* _temp137; struct Cyc_Absyn_TunionInfo
_temp139; struct Cyc_Absyn_XTunionInfo _temp141; struct Cyc_Absyn_TunionFieldInfo
_temp143; struct Cyc_Absyn_Tunionfield* _temp145; struct Cyc_Absyn_Tuniondecl*
_temp147; struct _tuple1* _temp149; struct Cyc_List_List* _temp151; struct
_tuple1* _temp153; struct Cyc_Absyn_XTunionFieldInfo _temp155; struct Cyc_Absyn_Tunionfield*
_temp157; struct Cyc_Absyn_XTuniondecl* _temp159; struct _tuple1* _temp161;
struct _tuple1* _temp163; struct Cyc_Absyn_Structdecl** _temp165; struct Cyc_List_List*
_temp167; struct _tuple1* _temp169; struct Cyc_Absyn_Uniondecl** _temp171;
struct Cyc_List_List* _temp173; struct _tuple1* _temp175; struct Cyc_Absyn_Structdecl**
_temp177; struct Cyc_List_List* _temp179; struct _tuple1* _temp181; struct Cyc_Absyn_Uniondecl**
_temp183; struct Cyc_List_List* _temp185; struct _tuple1* _temp187; struct Cyc_Absyn_Enumdecl*
_temp189; struct _tuple1* _temp191; struct Cyc_Absyn_PtrInfo _temp193; struct
Cyc_Absyn_Exp* _temp195; struct Cyc_Absyn_Tqual* _temp197; void* _temp199;
struct Cyc_List_List* _temp201; struct Cyc_Core_Opt* _temp203; struct Cyc_List_List*
_temp205; struct _tuple1* _temp207; void* _temp209; struct Cyc_List_List*
_temp211; _LL75: if(( unsigned int) _temp73 > 4u?(( struct _tunion_struct*)
_temp73)->tag == Cyc_Absyn_Evar_tag: 0){ _LL128: _temp127=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp73)->f1; goto _LL126; _LL126: _temp125=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp73)->f2; goto _LL124; _LL124: _temp123=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp73)->f3; goto _LL76;} else{ goto _LL77;} _LL77: if(( unsigned int) _temp73
> 4u?(( struct _tunion_struct*) _temp73)->tag == Cyc_Absyn_VarType_tag: 0){
_LL130: _temp129=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp73)->f1; goto _LL78;} else{ goto _LL79;} _LL79: if(( int) _temp73 == Cyc_Absyn_VoidType){
goto _LL80;} else{ goto _LL81;} _LL81: if(( unsigned int) _temp73 > 4u?(( struct
_tunion_struct*) _temp73)->tag == Cyc_Absyn_IntType_tag: 0){ _LL134: _temp133=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp73)->f1; goto _LL132; _LL132:
_temp131=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp73)->f2; goto _LL82;}
else{ goto _LL83;} _LL83: if(( int) _temp73 == Cyc_Absyn_FloatType){ goto _LL84;}
else{ goto _LL85;} _LL85: if(( int) _temp73 == Cyc_Absyn_DoubleType){ goto _LL86;}
else{ goto _LL87;} _LL87: if(( unsigned int) _temp73 > 4u?(( struct
_tunion_struct*) _temp73)->tag == Cyc_Absyn_FnType_tag: 0){ _LL136: _temp135=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp73)->f1; goto
_LL88;} else{ goto _LL89;} _LL89: if(( unsigned int) _temp73 > 4u?(( struct
_tunion_struct*) _temp73)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL138:
_temp137=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp73)->f1; goto
_LL90;} else{ goto _LL91;} _LL91: if(( int) _temp73 == Cyc_Absyn_HeapRgn){ goto
_LL92;} else{ goto _LL93;} _LL93: if(( unsigned int) _temp73 > 4u?(( struct
_tunion_struct*) _temp73)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL140: _temp139=(
struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*) _temp73)->f1;
goto _LL94;} else{ goto _LL95;} _LL95: if(( unsigned int) _temp73 > 4u?(( struct
_tunion_struct*) _temp73)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL142:
_temp141=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp73)->f1; goto _LL96;} else{ goto _LL97;} _LL97: if(( unsigned int) _temp73
> 4u?(( struct _tunion_struct*) _temp73)->tag == Cyc_Absyn_TunionFieldType_tag:
0){ _LL144: _temp143=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp73)->f1; _LL154: _temp153=( struct _tuple1*) _temp143.name; goto _LL152;
_LL152: _temp151=( struct Cyc_List_List*) _temp143.targs; goto _LL150; _LL150:
_temp149=( struct _tuple1*) _temp143.fname; goto _LL148; _LL148: _temp147=(
struct Cyc_Absyn_Tuniondecl*) _temp143.tud; goto _LL146; _LL146: _temp145=(
struct Cyc_Absyn_Tunionfield*) _temp143.tufd; goto _LL98;} else{ goto _LL99;}
_LL99: if(( unsigned int) _temp73 > 4u?(( struct _tunion_struct*) _temp73)->tag
== Cyc_Absyn_XTunionFieldType_tag: 0){ _LL156: _temp155=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp73)->f1; _LL164: _temp163=(
struct _tuple1*) _temp155.name; goto _LL162; _LL162: _temp161=( struct _tuple1*)
_temp155.fname; goto _LL160; _LL160: _temp159=( struct Cyc_Absyn_XTuniondecl*)
_temp155.xtud; goto _LL158; _LL158: _temp157=( struct Cyc_Absyn_Tunionfield*)
_temp155.xtufd; goto _LL100;} else{ goto _LL101;} _LL101: if(( unsigned int)
_temp73 > 4u?(( struct _tunion_struct*) _temp73)->tag == Cyc_Absyn_StructType_tag:
0){ _LL170: _temp169=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp73)->f1; goto _LL168; _LL168: _temp167=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp73)->f2; goto _LL166; _LL166: _temp165=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp73)->f3;
if( _temp165 == 0){ goto _LL102;} else{ goto _LL103;}} else{ goto _LL103;}
_LL103: if(( unsigned int) _temp73 > 4u?(( struct _tunion_struct*) _temp73)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL176: _temp175=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp73)->f1; goto _LL174; _LL174: _temp173=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp73)->f2; goto _LL172; _LL172: _temp171=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp73)->f3;
if( _temp171 == 0){ goto _LL104;} else{ goto _LL105;}} else{ goto _LL105;}
_LL105: if(( unsigned int) _temp73 > 4u?(( struct _tunion_struct*) _temp73)->tag
== Cyc_Absyn_StructType_tag: 0){ _LL182: _temp181=( struct _tuple1*)(( struct
Cyc_Absyn_StructType_struct*) _temp73)->f1; goto _LL180; _LL180: _temp179=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp73)->f2; goto
_LL178; _LL178: _temp177=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp73)->f3; goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int)
_temp73 > 4u?(( struct _tunion_struct*) _temp73)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL188: _temp187=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp73)->f1; goto _LL186; _LL186: _temp185=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp73)->f2; goto _LL184; _LL184: _temp183=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp73)->f3;
goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int) _temp73 > 4u?((
struct _tunion_struct*) _temp73)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL192:
_temp191=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp73)->f1;
goto _LL190; _LL190: _temp189=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp73)->f2; goto _LL110;} else{ goto _LL111;} _LL111: if(( unsigned int)
_temp73 > 4u?(( struct _tunion_struct*) _temp73)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL194: _temp193=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp73)->f1; goto _LL112;} else{ goto _LL113;} _LL113: if(( unsigned int)
_temp73 > 4u?(( struct _tunion_struct*) _temp73)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL200: _temp199=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp73)->f1;
goto _LL198; _LL198: _temp197=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp73)->f2; goto _LL196; _LL196: _temp195=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp73)->f3; goto _LL114;} else{ goto _LL115;}
_LL115: if(( unsigned int) _temp73 > 4u?(( struct _tunion_struct*) _temp73)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL202: _temp201=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp73)->f1; goto _LL116;} else{ goto
_LL117;} _LL117: if(( unsigned int) _temp73 > 4u?(( struct _tunion_struct*)
_temp73)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL208: _temp207=( struct
_tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp73)->f1; goto _LL206;
_LL206: _temp205=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp73)->f2; goto _LL204; _LL204: _temp203=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp73)->f3; goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int)
_temp73 > 4u?(( struct _tunion_struct*) _temp73)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL210: _temp209=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp73)->f1;
goto _LL120;} else{ goto _LL121;} _LL121: if(( unsigned int) _temp73 > 4u?((
struct _tunion_struct*) _temp73)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL212:
_temp211=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp73)->f1;
goto _LL122;} else{ goto _LL74;} _LL76: return _temp127; _LL78: return(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp129->kind); _LL80:
return( void*) Cyc_Absyn_MemKind; _LL82: return _temp131 ==( void*) Cyc_Absyn_B4?(
void*) Cyc_Absyn_BoxKind:( void*) Cyc_Absyn_MemKind; _LL84: return( void*) Cyc_Absyn_MemKind;
_LL86: return( void*) Cyc_Absyn_MemKind; _LL88: return( void*) Cyc_Absyn_MemKind;
_LL90: return( void*) Cyc_Absyn_BoxKind; _LL92: return( void*) Cyc_Absyn_RgnKind;
_LL94: return( void*) Cyc_Absyn_BoxKind; _LL96: return( void*) Cyc_Absyn_BoxKind;
_LL98: _temp157= _temp145; goto _LL100; _LL100: if( _temp157 == 0){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp213=( char*)"typ_kind: [X]TunionFieldInfo missing Tunionfield ref"; struct
_tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 53; _temp214;}));} if( _temp157->typs == 0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL102: return( void*) Cyc_Absyn_AnyKind; _LL104: return( void*) Cyc_Absyn_AnyKind;
_LL106: return( void*) Cyc_Absyn_MemKind; _LL108: return( void*) Cyc_Absyn_MemKind;
_LL110: return( void*) Cyc_Absyn_BoxKind; _LL112: { void* _temp215=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp193.bounds))->v; void* _temp225; void* _temp227; struct Cyc_Absyn_Exp*
_temp229; struct Cyc_Absyn_Conref* _temp231; _LL217: if(( unsigned int) _temp215
> 1u?(( struct _tunion_struct*) _temp215)->tag == Cyc_Absyn_Eq_constr_tag: 0){
_LL226: _temp225=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp215)->f1;
if(( int) _temp225 == Cyc_Absyn_Unknown_b){ goto _LL218;} else{ goto _LL219;}}
else{ goto _LL219;} _LL219: if(( unsigned int) _temp215 > 1u?(( struct
_tunion_struct*) _temp215)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL228: _temp227=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp215)->f1; if(( unsigned int)
_temp227 > 1u?(( struct _tunion_struct*) _temp227)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL230: _temp229=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp227)->f1; goto _LL220;} else{ goto _LL221;}} else{ goto _LL221;} _LL221:
if(( int) _temp215 == Cyc_Absyn_No_constr){ goto _LL222;} else{ goto _LL223;}
_LL223: if(( unsigned int) _temp215 > 1u?(( struct _tunion_struct*) _temp215)->tag
== Cyc_Absyn_Forward_constr_tag: 0){ _LL232: _temp231=( struct Cyc_Absyn_Conref*)((
struct Cyc_Absyn_Forward_constr_struct*) _temp215)->f1; goto _LL224;} else{ goto
_LL216;} _LL218: return( void*) Cyc_Absyn_MemKind; _LL220: return( void*) Cyc_Absyn_BoxKind;
_LL222: return( void*) Cyc_Absyn_MemKind; _LL224: return(( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp233=(
char*)"typ_kind: forward constraint in ptr bounds"; struct _tagged_string
_temp234; _temp234.curr= _temp233; _temp234.base= _temp233; _temp234.last_plus_one=
_temp233 + 43; _temp234;})); _LL216:;} _LL114: return( void*) Cyc_Absyn_MemKind;
_LL116: return( void*) Cyc_Absyn_MemKind; _LL118: return(( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string _temp235= Cyc_Absynpp_typ2string(
t); xprintf("typ_kind: typedef found: %.*s", _temp235.last_plus_one - _temp235.curr,
_temp235.curr);})); _LL120: return( void*) Cyc_Absyn_EffKind; _LL122: return(
void*) Cyc_Absyn_EffKind; _LL74:;} char Cyc_Tcutil_Unify_tag[ 6u]="Unify";
struct Cyc_Tcutil_Unify_struct{ char* tag; } ; int Cyc_Tcutil_unify( void* t1,
void* t2){ struct _handler_cons _temp236; _push_handler(& _temp236);{ struct
_xtunion_struct* _temp237=( struct _xtunion_struct*) setjmp( _temp236.handler);
if( ! _temp237){ Cyc_Tcutil_unify_it( t1, t2);{ int _temp238= 1; _npop_handler(
0u); return _temp238;}; _pop_handler();} else{ struct _xtunion_struct* _temp240=
_temp237; _LL242: if( _temp240->tag == Cyc_Tcutil_Unify_tag){ goto _LL243;}
else{ goto _LL244;} _LL244: goto _LL245; _LL243: return 0; _LL245:( void) _throw(
_temp240); _LL241:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
ts); static void Cyc_Tcutil_occurs( void* evar, void* t){ void* _temp246= Cyc_Tcutil_compress(
t); int _temp278; struct Cyc_Core_Opt* _temp280; void* _temp282; struct Cyc_Absyn_PtrInfo
_temp284; struct Cyc_Absyn_Exp* _temp286; struct Cyc_Absyn_Tqual* _temp288; void*
_temp290; struct Cyc_Absyn_FnInfo _temp292; struct Cyc_List_List* _temp294; int
_temp296; struct Cyc_List_List* _temp298; void* _temp300; struct Cyc_Core_Opt*
_temp302; struct Cyc_List_List* _temp304; struct Cyc_List_List* _temp306; void*
_temp308; struct Cyc_Absyn_TunionInfo _temp310; struct Cyc_Absyn_Tuniondecl*
_temp312; void* _temp314; struct Cyc_List_List* _temp316; struct _tuple1*
_temp318; struct Cyc_Absyn_XTunionInfo _temp320; struct Cyc_Absyn_XTuniondecl*
_temp322; void* _temp324; struct _tuple1* _temp326; struct Cyc_Core_Opt*
_temp328; struct Cyc_List_List* _temp330; struct _tuple1* _temp332; struct Cyc_Absyn_TunionFieldInfo
_temp334; struct Cyc_Absyn_Tunionfield* _temp336; struct Cyc_Absyn_Tuniondecl*
_temp338; struct _tuple1* _temp340; struct Cyc_List_List* _temp342; struct
_tuple1* _temp344; struct Cyc_Absyn_Structdecl** _temp346; struct Cyc_List_List*
_temp348; struct _tuple1* _temp350; void* _temp352; struct Cyc_List_List*
_temp354; struct Cyc_Absyn_XTunionFieldInfo _temp356; _LL248: if(( unsigned int)
_temp246 > 4u?(( struct _tunion_struct*) _temp246)->tag == Cyc_Absyn_Evar_tag: 0){
_LL283: _temp282=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp246)->f1; goto
_LL281; _LL281: _temp280=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp246)->f2; goto _LL279; _LL279: _temp278=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp246)->f3; goto _LL249;} else{ goto _LL250;} _LL250: if(( unsigned int)
_temp246 > 4u?(( struct _tunion_struct*) _temp246)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL285: _temp284=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp246)->f1; goto _LL251;} else{ goto _LL252;} _LL252: if(( unsigned int)
_temp246 > 4u?(( struct _tunion_struct*) _temp246)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL291: _temp290=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp246)->f1;
goto _LL289; _LL289: _temp288=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp246)->f2; goto _LL287; _LL287: _temp286=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp246)->f3; goto _LL253;} else{ goto _LL254;}
_LL254: if(( unsigned int) _temp246 > 4u?(( struct _tunion_struct*) _temp246)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL293: _temp292=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp246)->f1; _LL305: _temp304=( struct Cyc_List_List*)
_temp292.tvars; goto _LL303; _LL303: _temp302=( struct Cyc_Core_Opt*) _temp292.effect;
goto _LL301; _LL301: _temp300=( void*) _temp292.ret_typ; goto _LL299; _LL299:
_temp298=( struct Cyc_List_List*) _temp292.args; goto _LL297; _LL297: _temp296=(
int) _temp292.varargs; goto _LL295; _LL295: _temp294=( struct Cyc_List_List*)
_temp292.attributes; goto _LL255;} else{ goto _LL256;} _LL256: if(( unsigned int)
_temp246 > 4u?(( struct _tunion_struct*) _temp246)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL307: _temp306=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp246)->f1; goto _LL257;} else{ goto _LL258;} _LL258: if(( unsigned int)
_temp246 > 4u?(( struct _tunion_struct*) _temp246)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL309: _temp308=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp246)->f1;
goto _LL259;} else{ goto _LL260;} _LL260: if(( unsigned int) _temp246 > 4u?((
struct _tunion_struct*) _temp246)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL311:
_temp310=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp246)->f1; _LL319: _temp318=( struct _tuple1*) _temp310.name; goto _LL317;
_LL317: _temp316=( struct Cyc_List_List*) _temp310.targs; goto _LL315; _LL315:
_temp314=( void*) _temp310.rgn; goto _LL313; _LL313: _temp312=( struct Cyc_Absyn_Tuniondecl*)
_temp310.tud; goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int)
_temp246 > 4u?(( struct _tunion_struct*) _temp246)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL321: _temp320=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp246)->f1; _LL327: _temp326=( struct _tuple1*) _temp320.name; goto _LL325;
_LL325: _temp324=( void*) _temp320.rgn; goto _LL323; _LL323: _temp322=( struct
Cyc_Absyn_XTuniondecl*) _temp320.xtud; goto _LL263;} else{ goto _LL264;} _LL264:
if(( unsigned int) _temp246 > 4u?(( struct _tunion_struct*) _temp246)->tag ==
Cyc_Absyn_TypedefType_tag: 0){ _LL333: _temp332=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp246)->f1; goto _LL331; _LL331: _temp330=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp246)->f2; goto _LL329; _LL329: _temp328=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*) _temp246)->f3;
goto _LL265;} else{ goto _LL266;} _LL266: if(( unsigned int) _temp246 > 4u?((
struct _tunion_struct*) _temp246)->tag == Cyc_Absyn_TunionFieldType_tag: 0){
_LL335: _temp334=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp246)->f1; _LL345: _temp344=( struct _tuple1*) _temp334.name; goto _LL343;
_LL343: _temp342=( struct Cyc_List_List*) _temp334.targs; goto _LL341; _LL341:
_temp340=( struct _tuple1*) _temp334.fname; goto _LL339; _LL339: _temp338=(
struct Cyc_Absyn_Tuniondecl*) _temp334.tud; goto _LL337; _LL337: _temp336=(
struct Cyc_Absyn_Tunionfield*) _temp334.tufd; goto _LL267;} else{ goto _LL268;}
_LL268: if(( unsigned int) _temp246 > 4u?(( struct _tunion_struct*) _temp246)->tag
== Cyc_Absyn_StructType_tag: 0){ _LL351: _temp350=( struct _tuple1*)(( struct
Cyc_Absyn_StructType_struct*) _temp246)->f1; goto _LL349; _LL349: _temp348=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp246)->f2;
goto _LL347; _LL347: _temp346=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp246)->f3; goto _LL269;} else{ goto _LL270;} _LL270: if(( unsigned int)
_temp246 > 4u?(( struct _tunion_struct*) _temp246)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL353: _temp352=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp246)->f1;
goto _LL271;} else{ goto _LL272;} _LL272: if(( unsigned int) _temp246 > 4u?((
struct _tunion_struct*) _temp246)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL355:
_temp354=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp246)->f1;
goto _LL273;} else{ goto _LL274;} _LL274: if(( unsigned int) _temp246 > 4u?((
struct _tunion_struct*) _temp246)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){
_LL357: _temp356=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp246)->f1; goto _LL275;} else{ goto _LL276;} _LL276: goto _LL277; _LL249:
if( t == evar){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Tcutil_Unify_struct*
_temp358=( struct Cyc_Tcutil_Unify_struct*) GC_malloc( sizeof( struct Cyc_Tcutil_Unify_struct));
_temp358[ 0]=({ struct Cyc_Tcutil_Unify_struct _temp359; _temp359.tag= Cyc_Tcutil_Unify_tag;
_temp359;}); _temp358;}));} else{ if( _temp280 != 0){ Cyc_Tcutil_occurs( evar,(
void*) _temp280->v);}} goto _LL247; _LL251: Cyc_Tcutil_occurs( evar,( void*)
_temp284.elt_typ); Cyc_Tcutil_occurs( evar,( void*) _temp284.rgn_typ); goto
_LL247; _LL253: Cyc_Tcutil_occurs( evar, _temp290); goto _LL247; _LL255: if(
_temp302 != 0){ Cyc_Tcutil_occurs( evar,( void*) _temp302->v);} Cyc_Tcutil_occurs(
evar, _temp300); for( 0; _temp298 != 0; _temp298= _temp298->tl){ Cyc_Tcutil_occurs(
evar,(*(( struct _tuple2*) _temp298->hd)).f3);} goto _LL247; _LL257: for( 0;
_temp306 != 0; _temp306= _temp306->tl){ Cyc_Tcutil_occurs( evar,(*(( struct
_tuple4*) _temp306->hd)).f2);} goto _LL247; _LL259: Cyc_Tcutil_occurs( evar,
_temp308); goto _LL247; _LL261: Cyc_Tcutil_occurs( evar, _temp314); Cyc_Tcutil_occurslist(
evar, _temp316); goto _LL247; _LL263: Cyc_Tcutil_occurs( evar, _temp324); goto
_LL247; _LL265: _temp342= _temp330; goto _LL267; _LL267: _temp348= _temp342;
goto _LL269; _LL269: Cyc_Tcutil_occurslist( evar, _temp348); goto _LL247; _LL271:
Cyc_Tcutil_occurs( evar, _temp352); goto _LL247; _LL273: Cyc_Tcutil_occurslist(
evar, _temp354); goto _LL247; _LL275: goto _LL247; _LL277: goto _LL247; _LL247:;}
static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* ts){ for( 0;
ts != 0; ts= ts->tl){ Cyc_Tcutil_occurs( evar,( void*) ts->hd);}} static void
Cyc_Tcutil_unify_list( struct Cyc_List_List* t1, struct Cyc_List_List* t2){
struct _handler_cons _temp360; _push_handler(& _temp360);{ struct
_xtunion_struct* _temp361=( struct _xtunion_struct*) setjmp( _temp360.handler);
if( ! _temp361){(( void(*)( void(* f)( void*, void*), struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_iter2)( Cyc_Tcutil_unify_it, t1, t2);;
_pop_handler();} else{ struct _xtunion_struct* _temp363= _temp361; _LL365: if(
_temp363->tag == Cyc_List_List_mismatch_tag){ goto _LL366;} else{ goto _LL367;}
_LL367: goto _LL368; _LL366:( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Tcutil_Unify_struct* _temp369=( struct Cyc_Tcutil_Unify_struct*) GC_malloc(
sizeof( struct Cyc_Tcutil_Unify_struct)); _temp369[ 0]=({ struct Cyc_Tcutil_Unify_struct
_temp370; _temp370.tag= Cyc_Tcutil_Unify_tag; _temp370;}); _temp369;})); _LL368:(
void) _throw( _temp363); _LL364:;}}} static void Cyc_Tcutil_unify_tqual( struct
Cyc_Absyn_Tqual* tq1, struct Cyc_Absyn_Tqual* tq2){ if(( tq1->q_const != tq2->q_const?
1: tq1->q_volatile != tq2->q_volatile)? 1: tq1->q_restrict != tq2->q_restrict){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Tcutil_Unify_struct*
_temp371=( struct Cyc_Tcutil_Unify_struct*) GC_malloc( sizeof( struct Cyc_Tcutil_Unify_struct));
_temp371[ 0]=({ struct Cyc_Tcutil_Unify_struct _temp372; _temp372.tag= Cyc_Tcutil_Unify_tag;
_temp372;}); _temp371;}));}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual*
tq1, struct Cyc_Absyn_Tqual* tq2){ return( tq1->q_const == tq2->q_const? tq1->q_volatile
== tq2->q_volatile: 0)? tq1->q_restrict == tq2->q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
y); if( x == y){ return;}{ void* _temp373=( void*) x->v; struct Cyc_Absyn_Conref*
_temp381; void* _temp383; _LL375: if(( int) _temp373 == Cyc_Absyn_No_constr){
goto _LL376;} else{ goto _LL377;} _LL377: if(( unsigned int) _temp373 > 1u?((
struct _tunion_struct*) _temp373)->tag == Cyc_Absyn_Forward_constr_tag: 0){
_LL382: _temp381=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp373)->f1; goto _LL378;} else{ goto _LL379;} _LL379: if(( unsigned int)
_temp373 > 1u?(( struct _tunion_struct*) _temp373)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL384: _temp383=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp373)->f1;
goto _LL380;} else{ goto _LL374;} _LL376:( void*)( x->v=( void*)(( void*)({
struct Cyc_Absyn_Forward_constr_struct* _temp385=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp385[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp386; _temp386.tag= Cyc_Absyn_Forward_constr_tag;
_temp386.f1= y; _temp386;}); _temp385;}))); return; _LL378:( void) _throw(((
struct _xtunion_struct*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp387=( char*)"unify_conref: forward after compress";
struct _tagged_string _temp388; _temp388.curr= _temp387; _temp388.base= _temp387;
_temp388.last_plus_one= _temp387 + 37; _temp388;}))); _LL380: { void* _temp389=(
void*) y->v; struct Cyc_Absyn_Conref* _temp397; void* _temp399; _LL391: if(( int)
_temp389 == Cyc_Absyn_No_constr){ goto _LL392;} else{ goto _LL393;} _LL393: if((
unsigned int) _temp389 > 1u?(( struct _tunion_struct*) _temp389)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL398: _temp397=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp389)->f1; goto _LL394;} else{ goto _LL395;} _LL395: if(( unsigned int)
_temp389 > 1u?(( struct _tunion_struct*) _temp389)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL400: _temp399=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp389)->f1;
goto _LL396;} else{ goto _LL390;} _LL392:( void*)( y->v=( void*)(( void*) x->v));
return; _LL394:( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp401=(
char*)"unify_conref: forward after compress(2)"; struct _tagged_string _temp402;
_temp402.curr= _temp401; _temp402.base= _temp401; _temp402.last_plus_one=
_temp401 + 40; _temp402;}))); _LL396: if( cmp( _temp383, _temp399) != 0){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Tcutil_Unify_struct* _temp403=(
struct Cyc_Tcutil_Unify_struct*) GC_malloc( sizeof( struct Cyc_Tcutil_Unify_struct));
_temp403[ 0]=({ struct Cyc_Tcutil_Unify_struct _temp404; _temp404.tag= Cyc_Tcutil_Unify_tag;
_temp404;}); _temp403;}));} return; _LL390:;} _LL374:;}} static int Cyc_Tcutil_unify_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ struct _handler_cons _temp405; _push_handler(& _temp405);{ struct
_xtunion_struct* _temp406=( struct _xtunion_struct*) setjmp( _temp405.handler);
if( ! _temp406){(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int
_temp407= 1; _npop_handler( 0u); return _temp407;}; _pop_handler();} else{
struct _xtunion_struct* _temp409= _temp406; _LL411: if( _temp409->tag == Cyc_Tcutil_Unify_tag){
goto _LL412;} else{ goto _LL413;} _LL413: goto _LL414; _LL412: return 0; _LL414:(
void) _throw( _temp409); _LL410:;}}} static int Cyc_Tcutil_boundscmp( void* b1,
void* b2){ struct _tuple7 _temp416=({ struct _tuple7 _temp415; _temp415.f1= b1;
_temp415.f2= b2; _temp415;}); void* _temp426; void* _temp428; void* _temp430;
void* _temp432; void* _temp434; void* _temp436; void* _temp438; struct Cyc_Absyn_Exp*
_temp440; void* _temp442; struct Cyc_Absyn_Exp* _temp444; _LL418: _LL429:
_temp428= _temp416.f1; if(( int) _temp428 == Cyc_Absyn_Unknown_b){ goto _LL427;}
else{ goto _LL420;} _LL427: _temp426= _temp416.f2; if(( int) _temp426 == Cyc_Absyn_Unknown_b){
goto _LL419;} else{ goto _LL420;} _LL420: _LL433: _temp432= _temp416.f1; if((
int) _temp432 == Cyc_Absyn_Unknown_b){ goto _LL431;} else{ goto _LL422;} _LL431:
_temp430= _temp416.f2; goto _LL421; _LL422: _LL437: _temp436= _temp416.f1; goto
_LL435; _LL435: _temp434= _temp416.f2; if(( int) _temp434 == Cyc_Absyn_Unknown_b){
goto _LL423;} else{ goto _LL424;} _LL424: _LL443: _temp442= _temp416.f1; if((
unsigned int) _temp442 > 1u?(( struct _tunion_struct*) _temp442)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL445: _temp444=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp442)->f1; goto _LL439;} else{ goto _LL417;} _LL439: _temp438= _temp416.f2;
if(( unsigned int) _temp438 > 1u?(( struct _tunion_struct*) _temp438)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL441: _temp440=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp438)->f1; goto _LL425;} else{ goto _LL417;}
_LL419: return 0; _LL421: return - 1; _LL423: return 1; _LL425: { int i1=( int)
Cyc_Evexp_eval_const_uint_exp( _temp444); int i2=( int) Cyc_Evexp_eval_const_uint_exp(
_temp440); if( i1 == i2){ return 0;} if( i1 < i2){ return - 1;} return 1;}
_LL417:;} static int Cyc_Tcutil_equal_att( void* a1, void* a2){ if( a1 == a2){
return 1;}{ struct _tuple7 _temp447=({ struct _tuple7 _temp446; _temp446.f1= a1;
_temp446.f2= a2; _temp446;}); void* _temp457; int _temp459; void* _temp461; int
_temp463; void* _temp465; int _temp467; void* _temp469; int _temp471; void*
_temp473; struct _tagged_string _temp475; void* _temp477; struct _tagged_string
_temp479; _LL449: _LL462: _temp461= _temp447.f1; if(( unsigned int) _temp461 >
15u?(( struct _tunion_struct*) _temp461)->tag == Cyc_Absyn_Regparm_att_tag: 0){
_LL464: _temp463=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp461)->f1;
goto _LL458;} else{ goto _LL451;} _LL458: _temp457= _temp447.f2; if((
unsigned int) _temp457 > 15u?(( struct _tunion_struct*) _temp457)->tag == Cyc_Absyn_Regparm_att_tag:
0){ _LL460: _temp459=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp457)->f1;
goto _LL450;} else{ goto _LL451;} _LL451: _LL470: _temp469= _temp447.f1; if((
unsigned int) _temp469 > 15u?(( struct _tunion_struct*) _temp469)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL472: _temp471=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp469)->f1;
goto _LL466;} else{ goto _LL453;} _LL466: _temp465= _temp447.f2; if((
unsigned int) _temp465 > 15u?(( struct _tunion_struct*) _temp465)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL468: _temp467=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp465)->f1;
goto _LL452;} else{ goto _LL453;} _LL453: _LL478: _temp477= _temp447.f1; if((
unsigned int) _temp477 > 15u?(( struct _tunion_struct*) _temp477)->tag == Cyc_Absyn_Section_att_tag:
0){ _LL480: _temp479=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp477)->f1; goto _LL474;} else{ goto _LL455;} _LL474: _temp473= _temp447.f2;
if(( unsigned int) _temp473 > 15u?(( struct _tunion_struct*) _temp473)->tag ==
Cyc_Absyn_Section_att_tag: 0){ _LL476: _temp475=( struct _tagged_string)((
struct Cyc_Absyn_Section_att_struct*) _temp473)->f1; goto _LL454;} else{ goto
_LL455;} _LL455: goto _LL456; _LL450: _temp471= _temp463; _temp467= _temp459;
goto _LL452; _LL452: return _temp471 == _temp467; _LL454: return Cyc_String_strcmp(
_temp479, _temp475) == 0; _LL456: return 0; _LL448:;}} int Cyc_Tcutil_same_atts(
struct Cyc_List_List* a1, struct Cyc_List_List* a2){{ struct Cyc_List_List* a=
a1; for( 0; a != 0; a= a->tl){ if( !(( int(*)( int(* pred)( void*, void*), void*
env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)
a->hd, a2)){ return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0; a= a->tl){
if( !(( int(*)( int(* pred)( void*, void*), void* env, struct Cyc_List_List* x))
Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*) a->hd, a1)){ return 0;}}}
return 1;} static void Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp481= e; struct Cyc_List_List* _temp487; struct Cyc_List_List**
_temp489; _LL483: if(( unsigned int) _temp481 > 4u?(( struct _tunion_struct*)
_temp481)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL488: _temp487=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp481)->f1; _temp489=&(( struct Cyc_Absyn_JoinEff_struct*)
_temp481)->f1; goto _LL484;} else{ goto _LL485;} _LL485: goto _LL486; _LL484: {
int nested_join= 0;{ struct Cyc_List_List* effs=* _temp489; for( 0; effs != 0;
effs= effs->tl){ void* eff=( void*) effs->hd; Cyc_Tcutil_normalize_effect( eff);{
void* _temp490= Cyc_Tcutil_compress( eff); struct Cyc_List_List* _temp496;
_LL492: if(( unsigned int) _temp490 > 4u?(( struct _tunion_struct*) _temp490)->tag
== Cyc_Absyn_JoinEff_tag: 0){ _LL497: _temp496=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp490)->f1; goto _LL493;} else{ goto _LL494;}
_LL494: goto _LL495; _LL493: nested_join= 1; goto _LL491; _LL495: goto _LL491;
_LL491:;}}} if( ! nested_join){ return;}{ struct Cyc_List_List* effects= 0;{
struct Cyc_List_List* effs=* _temp489; for( 0; effs != 0; effs= effs->tl){ void*
_temp498= Cyc_Tcutil_compress(( void*) effs->hd); struct Cyc_List_List* _temp504;
_LL500: if(( unsigned int) _temp498 > 4u?(( struct _tunion_struct*) _temp498)->tag
== Cyc_Absyn_JoinEff_tag: 0){ _LL505: _temp504=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp498)->f1; goto _LL501;} else{ goto _LL502;}
_LL502: goto _LL503; _LL501: effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_revappend)( _temp504, effects); goto
_LL499; _LL503: effects=({ struct Cyc_List_List* _temp506=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp506->hd=( void*) _temp498;
_temp506->tl= effects; _temp506;}); goto _LL499; _LL499:;}}* _temp489=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( effects); goto
_LL482;}} _LL486: goto _LL482; _LL482:;}} static struct _tuple8* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp507= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp515; struct Cyc_List_List _temp517; struct Cyc_List_List* _temp518; void*
_temp520; int _temp522; struct Cyc_Core_Opt* _temp524; struct Cyc_Core_Opt**
_temp526; void* _temp527; void** _temp529; _LL509: if(( unsigned int) _temp507 >
4u?(( struct _tunion_struct*) _temp507)->tag == Cyc_Absyn_JoinEff_tag: 0){
_LL516: _temp515=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp507)->f1; if( _temp515 == 0){ goto _LL511;} else{ _temp517=* _temp515;
_LL521: _temp520=( void*) _temp517.hd; goto _LL519; _LL519: _temp518=( struct
Cyc_List_List*) _temp517.tl; goto _LL510;}} else{ goto _LL511;} _LL511: if((
unsigned int) _temp507 > 4u?(( struct _tunion_struct*) _temp507)->tag == Cyc_Absyn_Evar_tag:
0){ _LL528: _temp527=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp507)->f1;
_temp529=&(( struct Cyc_Absyn_Evar_struct*) _temp507)->f1; goto _LL525; _LL525:
_temp524=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp507)->f2;
_temp526=&(( struct Cyc_Absyn_Evar_struct*) _temp507)->f2; goto _LL523; _LL523:
_temp522=( int)(( struct Cyc_Absyn_Evar_struct*) _temp507)->f3; goto _LL512;}
else{ goto _LL513;} _LL513: goto _LL514; _LL510: { void* _temp530= Cyc_Tcutil_compress(
_temp520); int _temp536; struct Cyc_Core_Opt* _temp538; void* _temp540; _LL532:
if(( unsigned int) _temp530 > 4u?(( struct _tunion_struct*) _temp530)->tag ==
Cyc_Absyn_Evar_tag: 0){ _LL541: _temp540=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp530)->f1; goto _LL539; _LL539: _temp538=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp530)->f2; goto _LL537; _LL537: _temp536=( int)((
struct Cyc_Absyn_Evar_struct*) _temp530)->f3; goto _LL533;} else{ goto _LL534;}
_LL534: goto _LL535; _LL533: return({ struct _tuple8* _temp542=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp542->f1= _temp520; _temp542->f2=
_temp518; _temp542;}); _LL535: return 0; _LL531:;} _LL512: if(* _temp529 !=(
void*) Cyc_Absyn_EffKind){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp543=( char*)"effect evar has wrong kind";
struct _tagged_string _temp544; _temp544.curr= _temp543; _temp544.base= _temp543;
_temp544.last_plus_one= _temp543 + 27; _temp544;}));} return({ struct _tuple8*
_temp545=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp545->f1= t;
_temp545->f2= 0; _temp545;}); _LL514: return 0; _LL508:;} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp546= Cyc_Tcutil_compress( e); void*
_temp556; struct Cyc_List_List* _temp558; int _temp560; struct Cyc_Core_Opt*
_temp562; struct Cyc_Core_Opt** _temp564; void* _temp565; void** _temp567;
_LL548: if(( unsigned int) _temp546 > 4u?(( struct _tunion_struct*) _temp546)->tag
== Cyc_Absyn_AccessEff_tag: 0){ _LL557: _temp556=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp546)->f1; goto _LL549;} else{ goto _LL550;} _LL550: if(( unsigned int)
_temp546 > 4u?(( struct _tunion_struct*) _temp546)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL559: _temp558=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp546)->f1; goto _LL551;} else{ goto _LL552;} _LL552: if(( unsigned int)
_temp546 > 4u?(( struct _tunion_struct*) _temp546)->tag == Cyc_Absyn_Evar_tag: 0){
_LL566: _temp565=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp546)->f1;
_temp567=&(( struct Cyc_Absyn_Evar_struct*) _temp546)->f1; goto _LL563; _LL563:
_temp562=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp546)->f2;
_temp564=&(( struct Cyc_Absyn_Evar_struct*) _temp546)->f2; goto _LL561; _LL561:
_temp560=( int)(( struct Cyc_Absyn_Evar_struct*) _temp546)->f3; goto _LL553;}
else{ goto _LL554;} _LL554: goto _LL555; _LL549: if( constrain){ return Cyc_Tcutil_unify(
r, _temp556);} Cyc_Tcutil_compress( _temp556); if( r == _temp556){ return 1;}{
struct _tuple7 _temp569=({ struct _tuple7 _temp568; _temp568.f1= r; _temp568.f2=
_temp556; _temp568;}); void* _temp575; struct Cyc_Absyn_Tvar* _temp577; void*
_temp579; struct Cyc_Absyn_Tvar* _temp581; _LL571: _LL580: _temp579= _temp569.f1;
if(( unsigned int) _temp579 > 4u?(( struct _tunion_struct*) _temp579)->tag ==
Cyc_Absyn_VarType_tag: 0){ _LL582: _temp581=( struct Cyc_Absyn_Tvar*)(( struct
Cyc_Absyn_VarType_struct*) _temp579)->f1; goto _LL576;} else{ goto _LL573;}
_LL576: _temp575= _temp569.f2; if(( unsigned int) _temp575 > 4u?(( struct
_tunion_struct*) _temp575)->tag == Cyc_Absyn_VarType_tag: 0){ _LL578: _temp577=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp575)->f1; goto
_LL572;} else{ goto _LL573;} _LL573: goto _LL574; _LL572: return Cyc_Absyn_tvar_cmp(
_temp581, _temp577) == 0; _LL574: return 0; _LL570:;} _LL551: for( 0; _temp558
!= 0; _temp558= _temp558->tl){ if( Cyc_Tcutil_region_in_effect( constrain, r,(
void*) _temp558->hd)){ return 1;}} return 0; _LL553: if(* _temp567 !=( void*)
Cyc_Absyn_EffKind){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp583=( char*)"effect evar has wrong kind";
struct _tagged_string _temp584; _temp584.curr= _temp583; _temp584.base= _temp583;
_temp584.last_plus_one= _temp583 + 27; _temp584;}));} if( ! constrain){ return 0;}{
void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind); struct Cyc_Absyn_JoinEff_struct*
new_typ=({ struct Cyc_Absyn_JoinEff_struct* _temp585=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp585[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp586; _temp586.tag= Cyc_Absyn_JoinEff_tag; _temp586.f1=({ struct Cyc_List_List*
_temp587=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp587->hd=( void*) ev; _temp587->tl=({ struct Cyc_List_List* _temp588=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp588->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp589=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp589[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp590; _temp590.tag= Cyc_Absyn_AccessEff_tag;
_temp590.f1=( void*) r; _temp590;}); _temp589;})); _temp588->tl= 0; _temp588;});
_temp587;}); _temp586;}); _temp585;});* _temp564=({ struct Cyc_Core_Opt*
_temp591=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp591->v=( void*)(( void*) new_typ); _temp591;}); return 1;} _LL555: return 0;
_LL547:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct Cyc_Absyn_Tvar*
v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp592= e; struct Cyc_Absyn_Tvar*
_temp602; struct Cyc_List_List* _temp604; int _temp606; struct Cyc_Core_Opt*
_temp608; struct Cyc_Core_Opt** _temp610; void* _temp611; void** _temp613;
_LL594: if(( unsigned int) _temp592 > 4u?(( struct _tunion_struct*) _temp592)->tag
== Cyc_Absyn_VarType_tag: 0){ _LL603: _temp602=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp592)->f1; goto _LL595;} else{ goto _LL596;}
_LL596: if(( unsigned int) _temp592 > 4u?(( struct _tunion_struct*) _temp592)->tag
== Cyc_Absyn_JoinEff_tag: 0){ _LL605: _temp604=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp592)->f1; goto _LL597;} else{ goto _LL598;}
_LL598: if(( unsigned int) _temp592 > 4u?(( struct _tunion_struct*) _temp592)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL612: _temp611=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp592)->f1; _temp613=&(( struct Cyc_Absyn_Evar_struct*) _temp592)->f1; goto
_LL609; _LL609: _temp608=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp592)->f2; _temp610=&(( struct Cyc_Absyn_Evar_struct*) _temp592)->f2; goto
_LL607; _LL607: _temp606=( int)(( struct Cyc_Absyn_Evar_struct*) _temp592)->f3;
goto _LL599;} else{ goto _LL600;} _LL600: goto _LL601; _LL595: return Cyc_Absyn_tvar_cmp(
v, _temp602) == 0; _LL597: for( 0; _temp604 != 0; _temp604= _temp604->tl){ if(
Cyc_Tcutil_variable_in_effect( constrain, v,( void*) _temp604->hd)){ return 1;}}
return 0; _LL599: if(* _temp613 !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp614=(
char*)"effect evar has wrong kind"; struct _tagged_string _temp615; _temp615.curr=
_temp614; _temp615.base= _temp614; _temp615.last_plus_one= _temp614 + 27;
_temp615;}));}{ void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind); struct
Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct* _temp616=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp616[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp617; _temp617.tag= Cyc_Absyn_JoinEff_tag;
_temp617.f1=({ struct Cyc_List_List* _temp618=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp618->hd=( void*) ev; _temp618->tl=({ struct
Cyc_List_List* _temp619=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp619->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp620=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp620[ 0]=({ struct Cyc_Absyn_VarType_struct _temp621; _temp621.tag= Cyc_Absyn_VarType_tag;
_temp621.f1= v; _temp621;}); _temp620;})); _temp619->tl= 0; _temp619;});
_temp618;}); _temp617;}); _temp616;});* _temp610=({ struct Cyc_Core_Opt*
_temp622=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp622->v=( void*)(( void*) new_typ); _temp622;}); return 1;} _LL601: return 0;
_LL593:;}} static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp623= e; struct Cyc_List_List* _temp631; int _temp633; struct Cyc_Core_Opt*
_temp635; void* _temp637; _LL625: if(( unsigned int) _temp623 > 4u?(( struct
_tunion_struct*) _temp623)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL632: _temp631=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp623)->f1; goto
_LL626;} else{ goto _LL627;} _LL627: if(( unsigned int) _temp623 > 4u?(( struct
_tunion_struct*) _temp623)->tag == Cyc_Absyn_Evar_tag: 0){ _LL638: _temp637=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp623)->f1; goto _LL636; _LL636:
_temp635=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp623)->f2;
goto _LL634; _LL634: _temp633=( int)(( struct Cyc_Absyn_Evar_struct*) _temp623)->f3;
goto _LL628;} else{ goto _LL629;} _LL629: goto _LL630; _LL626: for( 0; _temp631
!= 0; _temp631= _temp631->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)
_temp631->hd)){ return 1;}} return 0; _LL628: return evar == e; _LL630: return 0;
_LL624:;}} int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void* e2){{
struct _tuple8* _temp639= Cyc_Tcutil_get_effect_evar( e2); struct _tuple8
_temp645; struct Cyc_List_List* _temp646; void* _temp648; _LL641: if( _temp639
== 0){ goto _LL643;} else{ _temp645=* _temp639; _LL649: _temp648= _temp645.f1;
goto _LL647; _LL647: _temp646= _temp645.f2; goto _LL642;} _LL643: goto _LL644;
_LL642: { void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind); struct Cyc_Absyn_JoinEff_struct*
new_typ=({ struct Cyc_Absyn_JoinEff_struct* _temp650=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp650[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp651; _temp651.tag= Cyc_Absyn_JoinEff_tag; _temp651.f1=({ struct Cyc_List_List*
_temp652=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp652->hd=( void*) ev; _temp652->tl=({ struct Cyc_List_List* _temp653=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp653->hd=(
void*) e1; _temp653->tl= 0; _temp653;}); _temp652;}); _temp651;}); _temp650;});
return Cyc_Tcutil_unify( _temp648,( void*) new_typ);} _LL644: goto _LL640;
_LL640:;}{ void* _temp654= Cyc_Tcutil_compress( e1); struct Cyc_List_List*
_temp666; void* _temp668; struct Cyc_Absyn_Tvar* _temp670; int _temp672; struct
Cyc_Core_Opt* _temp674; struct Cyc_Core_Opt** _temp676; void* _temp677; _LL656:
if(( unsigned int) _temp654 > 4u?(( struct _tunion_struct*) _temp654)->tag ==
Cyc_Absyn_JoinEff_tag: 0){ _LL667: _temp666=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp654)->f1; goto _LL657;} else{ goto _LL658;}
_LL658: if(( unsigned int) _temp654 > 4u?(( struct _tunion_struct*) _temp654)->tag
== Cyc_Absyn_AccessEff_tag: 0){ _LL669: _temp668=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp654)->f1; goto _LL659;} else{ goto _LL660;} _LL660: if(( unsigned int)
_temp654 > 4u?(( struct _tunion_struct*) _temp654)->tag == Cyc_Absyn_VarType_tag:
0){ _LL671: _temp670=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp654)->f1; goto _LL661;} else{ goto _LL662;} _LL662: if(( unsigned int)
_temp654 > 4u?(( struct _tunion_struct*) _temp654)->tag == Cyc_Absyn_Evar_tag: 0){
_LL678: _temp677=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp654)->f1; goto
_LL675; _LL675: _temp674=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp654)->f2; _temp676=&(( struct Cyc_Absyn_Evar_struct*) _temp654)->f2; goto
_LL673; _LL673: _temp672=( int)(( struct Cyc_Absyn_Evar_struct*) _temp654)->f3;
goto _LL663;} else{ goto _LL664;} _LL664: goto _LL665; _LL657: for( 0; _temp666
!= 0; _temp666= _temp666->tl){ if( ! Cyc_Tcutil_subset_effect( set_to_empty,(
void*) _temp666->hd, e2)){ return 0;}} return 1; _LL659: return( Cyc_Tcutil_region_in_effect(
0, _temp668, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp668, e2))? 1: Cyc_Tcutil_unify(
_temp668,( void*) Cyc_Absyn_HeapRgn); _LL661: return Cyc_Tcutil_variable_in_effect(
0, _temp670, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp670, e2); _LL663:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp676=({
struct Cyc_Core_Opt* _temp679=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp679->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp680=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp680[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp681; _temp681.tag= Cyc_Absyn_JoinEff_tag;
_temp681.f1= 0; _temp681;}); _temp680;})); _temp679;});} else{* _temp676=({
struct Cyc_Core_Opt* _temp682=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp682->v=( void*) e2; _temp682;});}} return 1; _LL665: return((
int(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string
_temp683= Cyc_Absynpp_typ2string( e1); xprintf("subset_effect: bad effect: %.*s",
_temp683.last_plus_one - _temp683.curr, _temp683.curr);})); _LL655:;}} static
int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1= Cyc_Tcutil_compress( e1);
e2= Cyc_Tcutil_compress( e2); Cyc_Tcutil_normalize_effect( e1); Cyc_Tcutil_normalize_effect(
e2);{ struct _tuple9 _temp685=({ struct _tuple9 _temp684; _temp684.f1= Cyc_Tcutil_get_effect_evar(
e1); _temp684.f2= Cyc_Tcutil_get_effect_evar( e2); _temp684;}); struct _tuple8*
_temp691; struct _tuple8 _temp693; struct Cyc_List_List* _temp694; void*
_temp696; struct _tuple8* _temp698; struct _tuple8 _temp700; struct Cyc_List_List*
_temp701; void* _temp703; _LL687: _LL699: _temp698= _temp685.f1; if( _temp698 ==
0){ goto _LL689;} else{ _temp700=* _temp698; _LL704: _temp703= _temp700.f1; goto
_LL702; _LL702: _temp701= _temp700.f2; goto _LL692;} _LL692: _temp691= _temp685.f2;
if( _temp691 == 0){ goto _LL689;} else{ _temp693=* _temp691; _LL697: _temp696=
_temp693.f1; goto _LL695; _LL695: _temp694= _temp693.f2; goto _LL688;} _LL689:
goto _LL690; _LL688: { void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp705=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp705[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp706; _temp706.tag= Cyc_Absyn_JoinEff_tag;
_temp706.f1=({ struct Cyc_List_List* _temp707=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp707->hd=( void*) ev; _temp707->tl=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp701, _temp694); _temp707;}); _temp706;}); _temp705;}); return Cyc_Tcutil_unify(
_temp703,( void*) new_typ)? Cyc_Tcutil_unify( _temp696,( void*) new_typ): 0;}
_LL690: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0,
e2, e1): 0; _LL686:;}} void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp708= t1;
int _temp714; struct Cyc_Core_Opt* _temp716; struct Cyc_Core_Opt** _temp718;
void* _temp719; _LL710: if(( unsigned int) _temp708 > 4u?(( struct
_tunion_struct*) _temp708)->tag == Cyc_Absyn_Evar_tag: 0){ _LL720: _temp719=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp708)->f1; goto _LL717; _LL717:
_temp716=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp708)->f2;
_temp718=&(( struct Cyc_Absyn_Evar_struct*) _temp708)->f2; goto _LL715; _LL715:
_temp714=( int)(( struct Cyc_Absyn_Evar_struct*) _temp708)->f3; goto _LL711;}
else{ goto _LL712;} _LL712: goto _LL713; _LL711: Cyc_Tcutil_occurs( t1, t2);{
void* kind2= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq( kind2, _temp719)){*
_temp718=({ struct Cyc_Core_Opt* _temp721=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp721->v=( void*) t2; _temp721;}); return;}
else{{ void* _temp722= t2; int _temp730; struct Cyc_Core_Opt* _temp732; struct
Cyc_Core_Opt** _temp734; void* _temp735; struct Cyc_Absyn_PtrInfo _temp738;
_LL724: if(( unsigned int) _temp722 > 4u?(( struct _tunion_struct*) _temp722)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL736: _temp735=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp722)->f1; goto _LL733; _LL733: _temp732=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp722)->f2; _temp734=&(( struct Cyc_Absyn_Evar_struct*)
_temp722)->f2; goto _LL731; _LL731: _temp730=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp722)->f3; goto _LL725;} else{ goto _LL726;} _LL726: if(( unsigned int)
_temp722 > 4u?(( struct _tunion_struct*) _temp722)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL739: _temp738=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp722)->f1; goto _LL737;} else{ goto _LL728;} _LL737: if( _temp719 ==( void*)
Cyc_Absyn_BoxKind){ goto _LL727;} else{ goto _LL728;} _LL728: goto _LL729;
_LL725: if( Cyc_Tcutil_kind_leq( _temp719, kind2)){* _temp734=({ struct Cyc_Core_Opt*
_temp740=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp740->v=( void*) t1; _temp740;}); return;} goto _LL723; _LL727: { struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp738.bounds);{ void* _temp741=( void*) c->v; _LL743: if(( int) _temp741 ==
Cyc_Absyn_No_constr){ goto _LL744;} else{ goto _LL745;} _LL745: goto _LL746;
_LL744:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp747=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp747[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp748; _temp748.tag= Cyc_Absyn_Eq_constr_tag;
_temp748.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp749=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp749[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp750; _temp750.tag= Cyc_Absyn_Upper_b_tag;
_temp750.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp750;}); _temp749;}));
_temp748;}); _temp747;})));* _temp718=({ struct Cyc_Core_Opt* _temp751=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp751->v=( void*) t2;
_temp751;}); return; _LL746: goto _LL742; _LL742:;} goto _LL723;} _LL729: goto
_LL723; _LL723:;}( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Tcutil_Unify_struct*
_temp752=( struct Cyc_Tcutil_Unify_struct*) GC_malloc( sizeof( struct Cyc_Tcutil_Unify_struct));
_temp752[ 0]=({ struct Cyc_Tcutil_Unify_struct _temp753; _temp753.tag= Cyc_Tcutil_Unify_tag;
_temp753;}); _temp752;}));}} _LL713: goto _LL709; _LL709:;}{ struct _tuple7
_temp755=({ struct _tuple7 _temp754; _temp754.f1= t2; _temp754.f2= t1; _temp754;});
void* _temp805; void* _temp807; int _temp809; struct Cyc_Core_Opt* _temp811;
void* _temp813; void* _temp815; void* _temp817; void* _temp819; struct Cyc_Absyn_Tvar*
_temp821; void* _temp823; struct Cyc_Absyn_Tvar* _temp825; void* _temp827;
struct Cyc_Absyn_Structdecl** _temp829; struct Cyc_List_List* _temp831; struct
_tuple1* _temp833; void* _temp835; struct Cyc_Absyn_Structdecl** _temp837;
struct Cyc_List_List* _temp839; struct _tuple1* _temp841; void* _temp843; struct
Cyc_Absyn_Enumdecl* _temp845; struct _tuple1* _temp847; void* _temp849; struct
Cyc_Absyn_Enumdecl* _temp851; struct _tuple1* _temp853; void* _temp855; struct
Cyc_Absyn_Uniondecl** _temp857; struct Cyc_List_List* _temp859; struct _tuple1*
_temp861; void* _temp863; struct Cyc_Absyn_Uniondecl** _temp865; struct Cyc_List_List*
_temp867; struct _tuple1* _temp869; void* _temp871; struct Cyc_Absyn_TunionInfo
_temp873; struct Cyc_Absyn_Tuniondecl* _temp875; void* _temp877; struct Cyc_List_List*
_temp879; struct _tuple1* _temp881; void* _temp883; struct Cyc_Absyn_TunionInfo
_temp885; struct Cyc_Absyn_Tuniondecl* _temp887; void* _temp889; struct Cyc_List_List*
_temp891; struct _tuple1* _temp893; void* _temp895; struct Cyc_Absyn_XTunionInfo
_temp897; struct Cyc_Absyn_XTuniondecl* _temp899; void* _temp901; struct _tuple1*
_temp903; void* _temp905; struct Cyc_Absyn_XTunionInfo _temp907; struct Cyc_Absyn_XTuniondecl*
_temp909; void* _temp911; struct _tuple1* _temp913; void* _temp915; struct Cyc_Absyn_TunionFieldInfo
_temp917; struct Cyc_Absyn_Tunionfield* _temp919; struct Cyc_Absyn_Tuniondecl*
_temp921; struct _tuple1* _temp923; struct Cyc_List_List* _temp925; struct
_tuple1* _temp927; void* _temp929; struct Cyc_Absyn_TunionFieldInfo _temp931;
struct Cyc_Absyn_Tunionfield* _temp933; struct Cyc_Absyn_Tuniondecl* _temp935;
struct _tuple1* _temp937; struct Cyc_List_List* _temp939; struct _tuple1*
_temp941; void* _temp943; struct Cyc_Absyn_XTunionFieldInfo _temp945; struct Cyc_Absyn_Tunionfield*
_temp947; struct Cyc_Absyn_XTuniondecl* _temp949; struct _tuple1* _temp951;
struct _tuple1* _temp953; void* _temp955; struct Cyc_Absyn_XTunionFieldInfo
_temp957; struct Cyc_Absyn_Tunionfield* _temp959; struct Cyc_Absyn_XTuniondecl*
_temp961; struct _tuple1* _temp963; struct _tuple1* _temp965; void* _temp967;
struct Cyc_Absyn_PtrInfo _temp969; struct Cyc_Absyn_Conref* _temp971; struct Cyc_Absyn_Tqual*
_temp973; struct Cyc_Absyn_Conref* _temp975; void* _temp977; void* _temp979;
void* _temp981; struct Cyc_Absyn_PtrInfo _temp983; struct Cyc_Absyn_Conref*
_temp985; struct Cyc_Absyn_Tqual* _temp987; struct Cyc_Absyn_Conref* _temp989;
void* _temp991; void* _temp993; void* _temp995; void* _temp997; void* _temp999;
void* _temp1001; void* _temp1003; void* _temp1005; void* _temp1007; void*
_temp1009; void* _temp1011; void* _temp1013; void* _temp1015; struct Cyc_Absyn_Exp*
_temp1017; struct Cyc_Absyn_Tqual* _temp1019; void* _temp1021; void* _temp1023;
struct Cyc_Absyn_Exp* _temp1025; struct Cyc_Absyn_Tqual* _temp1027; void*
_temp1029; void* _temp1031; struct Cyc_Absyn_FnInfo _temp1033; struct Cyc_List_List*
_temp1035; int _temp1037; struct Cyc_List_List* _temp1039; void* _temp1041;
struct Cyc_Core_Opt* _temp1043; struct Cyc_List_List* _temp1045; void* _temp1047;
struct Cyc_Absyn_FnInfo _temp1049; struct Cyc_List_List* _temp1051; int
_temp1053; struct Cyc_List_List* _temp1055; void* _temp1057; struct Cyc_Core_Opt*
_temp1059; struct Cyc_List_List* _temp1061; void* _temp1063; struct Cyc_List_List*
_temp1065; void* _temp1067; struct Cyc_List_List* _temp1069; void* _temp1071;
void* _temp1073; void* _temp1075; void* _temp1077; void* _temp1079; void*
_temp1081; void* _temp1083; void* _temp1085; struct Cyc_List_List* _temp1087;
void* _temp1089; struct Cyc_List_List* _temp1091; void* _temp1093; void*
_temp1095; void* _temp1097; void* _temp1099; void* _temp1101; void* _temp1103;
void* _temp1105; _LL757: _LL808: _temp807= _temp755.f1; if(( unsigned int)
_temp807 > 4u?(( struct _tunion_struct*) _temp807)->tag == Cyc_Absyn_Evar_tag: 0){
_LL814: _temp813=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp807)->f1; goto
_LL812; _LL812: _temp811=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp807)->f2; goto _LL810; _LL810: _temp809=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp807)->f3; goto _LL806;} else{ goto _LL759;} _LL806: _temp805= _temp755.f2;
goto _LL758; _LL759: _LL818: _temp817= _temp755.f1; if(( int) _temp817 == Cyc_Absyn_VoidType){
goto _LL816;} else{ goto _LL761;} _LL816: _temp815= _temp755.f2; if(( int)
_temp815 == Cyc_Absyn_VoidType){ goto _LL760;} else{ goto _LL761;} _LL761:
_LL824: _temp823= _temp755.f1; if(( unsigned int) _temp823 > 4u?(( struct
_tunion_struct*) _temp823)->tag == Cyc_Absyn_VarType_tag: 0){ _LL826: _temp825=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp823)->f1; goto
_LL820;} else{ goto _LL763;} _LL820: _temp819= _temp755.f2; if(( unsigned int)
_temp819 > 4u?(( struct _tunion_struct*) _temp819)->tag == Cyc_Absyn_VarType_tag:
0){ _LL822: _temp821=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp819)->f1; goto _LL762;} else{ goto _LL763;} _LL763: _LL836: _temp835=
_temp755.f1; if(( unsigned int) _temp835 > 4u?(( struct _tunion_struct*)
_temp835)->tag == Cyc_Absyn_StructType_tag: 0){ _LL842: _temp841=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp835)->f1; goto _LL840;
_LL840: _temp839=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp835)->f2; goto _LL838; _LL838: _temp837=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp835)->f3; goto _LL828;} else{ goto
_LL765;} _LL828: _temp827= _temp755.f2; if(( unsigned int) _temp827 > 4u?((
struct _tunion_struct*) _temp827)->tag == Cyc_Absyn_StructType_tag: 0){ _LL834:
_temp833=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp827)->f1;
goto _LL832; _LL832: _temp831=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp827)->f2; goto _LL830; _LL830: _temp829=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp827)->f3; goto _LL764;} else{ goto
_LL765;} _LL765: _LL850: _temp849= _temp755.f1; if(( unsigned int) _temp849 > 4u?((
struct _tunion_struct*) _temp849)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL854:
_temp853=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp849)->f1;
goto _LL852; _LL852: _temp851=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp849)->f2; goto _LL844;} else{ goto _LL767;} _LL844: _temp843= _temp755.f2;
if(( unsigned int) _temp843 > 4u?(( struct _tunion_struct*) _temp843)->tag ==
Cyc_Absyn_EnumType_tag: 0){ _LL848: _temp847=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp843)->f1; goto _LL846; _LL846: _temp845=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp843)->f2; goto _LL766;} else{ goto
_LL767;} _LL767: _LL864: _temp863= _temp755.f1; if(( unsigned int) _temp863 > 4u?((
struct _tunion_struct*) _temp863)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL870:
_temp869=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp863)->f1;
goto _LL868; _LL868: _temp867=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp863)->f2; goto _LL866; _LL866: _temp865=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp863)->f3; goto _LL856;} else{ goto
_LL769;} _LL856: _temp855= _temp755.f2; if(( unsigned int) _temp855 > 4u?((
struct _tunion_struct*) _temp855)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL862:
_temp861=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp855)->f1;
goto _LL860; _LL860: _temp859=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp855)->f2; goto _LL858; _LL858: _temp857=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp855)->f3; goto _LL768;} else{ goto
_LL769;} _LL769: _LL884: _temp883= _temp755.f1; if(( unsigned int) _temp883 > 4u?((
struct _tunion_struct*) _temp883)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL886:
_temp885=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp883)->f1; _LL894: _temp893=( struct _tuple1*) _temp885.name; goto _LL892;
_LL892: _temp891=( struct Cyc_List_List*) _temp885.targs; goto _LL890; _LL890:
_temp889=( void*) _temp885.rgn; goto _LL888; _LL888: _temp887=( struct Cyc_Absyn_Tuniondecl*)
_temp885.tud; goto _LL872;} else{ goto _LL771;} _LL872: _temp871= _temp755.f2;
if(( unsigned int) _temp871 > 4u?(( struct _tunion_struct*) _temp871)->tag ==
Cyc_Absyn_TunionType_tag: 0){ _LL874: _temp873=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp871)->f1; _LL882: _temp881=( struct
_tuple1*) _temp873.name; goto _LL880; _LL880: _temp879=( struct Cyc_List_List*)
_temp873.targs; goto _LL878; _LL878: _temp877=( void*) _temp873.rgn; goto _LL876;
_LL876: _temp875=( struct Cyc_Absyn_Tuniondecl*) _temp873.tud; goto _LL770;}
else{ goto _LL771;} _LL771: _LL906: _temp905= _temp755.f1; if(( unsigned int)
_temp905 > 4u?(( struct _tunion_struct*) _temp905)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL908: _temp907=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp905)->f1; _LL914: _temp913=( struct _tuple1*) _temp907.name; goto _LL912;
_LL912: _temp911=( void*) _temp907.rgn; goto _LL910; _LL910: _temp909=( struct
Cyc_Absyn_XTuniondecl*) _temp907.xtud; goto _LL896;} else{ goto _LL773;} _LL896:
_temp895= _temp755.f2; if(( unsigned int) _temp895 > 4u?(( struct _tunion_struct*)
_temp895)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL898: _temp897=( struct Cyc_Absyn_XTunionInfo)((
struct Cyc_Absyn_XTunionType_struct*) _temp895)->f1; _LL904: _temp903=( struct
_tuple1*) _temp897.name; goto _LL902; _LL902: _temp901=( void*) _temp897.rgn;
goto _LL900; _LL900: _temp899=( struct Cyc_Absyn_XTuniondecl*) _temp897.xtud;
goto _LL772;} else{ goto _LL773;} _LL773: _LL930: _temp929= _temp755.f1; if((
unsigned int) _temp929 > 4u?(( struct _tunion_struct*) _temp929)->tag == Cyc_Absyn_TunionFieldType_tag:
0){ _LL932: _temp931=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp929)->f1; _LL942: _temp941=( struct _tuple1*) _temp931.name; goto _LL940;
_LL940: _temp939=( struct Cyc_List_List*) _temp931.targs; goto _LL938; _LL938:
_temp937=( struct _tuple1*) _temp931.fname; goto _LL936; _LL936: _temp935=(
struct Cyc_Absyn_Tuniondecl*) _temp931.tud; goto _LL934; _LL934: _temp933=(
struct Cyc_Absyn_Tunionfield*) _temp931.tufd; goto _LL916;} else{ goto _LL775;}
_LL916: _temp915= _temp755.f2; if(( unsigned int) _temp915 > 4u?(( struct
_tunion_struct*) _temp915)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL918:
_temp917=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp915)->f1; _LL928: _temp927=( struct _tuple1*) _temp917.name; goto _LL926;
_LL926: _temp925=( struct Cyc_List_List*) _temp917.targs; goto _LL924; _LL924:
_temp923=( struct _tuple1*) _temp917.fname; goto _LL922; _LL922: _temp921=(
struct Cyc_Absyn_Tuniondecl*) _temp917.tud; goto _LL920; _LL920: _temp919=(
struct Cyc_Absyn_Tunionfield*) _temp917.tufd; goto _LL774;} else{ goto _LL775;}
_LL775: _LL956: _temp955= _temp755.f1; if(( unsigned int) _temp955 > 4u?((
struct _tunion_struct*) _temp955)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){
_LL958: _temp957=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp955)->f1; _LL966: _temp965=( struct _tuple1*) _temp957.name; goto _LL964;
_LL964: _temp963=( struct _tuple1*) _temp957.fname; goto _LL962; _LL962:
_temp961=( struct Cyc_Absyn_XTuniondecl*) _temp957.xtud; goto _LL960; _LL960:
_temp959=( struct Cyc_Absyn_Tunionfield*) _temp957.xtufd; goto _LL944;} else{
goto _LL777;} _LL944: _temp943= _temp755.f2; if(( unsigned int) _temp943 > 4u?((
struct _tunion_struct*) _temp943)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){
_LL946: _temp945=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp943)->f1; _LL954: _temp953=( struct _tuple1*) _temp945.name; goto _LL952;
_LL952: _temp951=( struct _tuple1*) _temp945.fname; goto _LL950; _LL950:
_temp949=( struct Cyc_Absyn_XTuniondecl*) _temp945.xtud; goto _LL948; _LL948:
_temp947=( struct Cyc_Absyn_Tunionfield*) _temp945.xtufd; goto _LL776;} else{
goto _LL777;} _LL777: _LL982: _temp981= _temp755.f1; if(( unsigned int) _temp981
> 4u?(( struct _tunion_struct*) _temp981)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL984: _temp983=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp981)->f1; _LL994: _temp993=( void*) _temp983.elt_typ; goto _LL992; _LL992:
_temp991=( void*) _temp983.rgn_typ; goto _LL990; _LL990: _temp989=( struct Cyc_Absyn_Conref*)
_temp983.nullable; goto _LL988; _LL988: _temp987=( struct Cyc_Absyn_Tqual*)
_temp983.tq; goto _LL986; _LL986: _temp985=( struct Cyc_Absyn_Conref*) _temp983.bounds;
goto _LL968;} else{ goto _LL779;} _LL968: _temp967= _temp755.f2; if((
unsigned int) _temp967 > 4u?(( struct _tunion_struct*) _temp967)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL970: _temp969=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp967)->f1; _LL980: _temp979=( void*) _temp969.elt_typ; goto _LL978; _LL978:
_temp977=( void*) _temp969.rgn_typ; goto _LL976; _LL976: _temp975=( struct Cyc_Absyn_Conref*)
_temp969.nullable; goto _LL974; _LL974: _temp973=( struct Cyc_Absyn_Tqual*)
_temp969.tq; goto _LL972; _LL972: _temp971=( struct Cyc_Absyn_Conref*) _temp969.bounds;
goto _LL778;} else{ goto _LL779;} _LL779: _LL1002: _temp1001= _temp755.f1; if((
unsigned int) _temp1001 > 4u?(( struct _tunion_struct*) _temp1001)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1006: _temp1005=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1001)->f1;
goto _LL1004; _LL1004: _temp1003=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1001)->f2; goto _LL996;} else{ goto _LL781;} _LL996: _temp995= _temp755.f2;
if(( unsigned int) _temp995 > 4u?(( struct _tunion_struct*) _temp995)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL1000: _temp999=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp995)->f1; goto _LL998; _LL998: _temp997=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp995)->f2; goto _LL780;} else{ goto _LL781;} _LL781: _LL1010: _temp1009=
_temp755.f1; if(( int) _temp1009 == Cyc_Absyn_FloatType){ goto _LL1008;} else{
goto _LL783;} _LL1008: _temp1007= _temp755.f2; if(( int) _temp1007 == Cyc_Absyn_FloatType){
goto _LL782;} else{ goto _LL783;} _LL783: _LL1014: _temp1013= _temp755.f1; if((
int) _temp1013 == Cyc_Absyn_DoubleType){ goto _LL1012;} else{ goto _LL785;}
_LL1012: _temp1011= _temp755.f2; if(( int) _temp1011 == Cyc_Absyn_DoubleType){
goto _LL784;} else{ goto _LL785;} _LL785: _LL1024: _temp1023= _temp755.f1; if((
unsigned int) _temp1023 > 4u?(( struct _tunion_struct*) _temp1023)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1030: _temp1029=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1023)->f1;
goto _LL1028; _LL1028: _temp1027=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1023)->f2; goto _LL1026; _LL1026: _temp1025=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1023)->f3; goto _LL1016;} else{ goto
_LL787;} _LL1016: _temp1015= _temp755.f2; if(( unsigned int) _temp1015 > 4u?((
struct _tunion_struct*) _temp1015)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1022:
_temp1021=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1015)->f1; goto
_LL1020; _LL1020: _temp1019=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1015)->f2; goto _LL1018; _LL1018: _temp1017=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1015)->f3; goto _LL786;} else{ goto
_LL787;} _LL787: _LL1048: _temp1047= _temp755.f1; if(( unsigned int) _temp1047 >
4u?(( struct _tunion_struct*) _temp1047)->tag == Cyc_Absyn_FnType_tag: 0){
_LL1050: _temp1049=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1047)->f1; _LL1062: _temp1061=( struct Cyc_List_List*) _temp1049.tvars;
goto _LL1060; _LL1060: _temp1059=( struct Cyc_Core_Opt*) _temp1049.effect; goto
_LL1058; _LL1058: _temp1057=( void*) _temp1049.ret_typ; goto _LL1056; _LL1056:
_temp1055=( struct Cyc_List_List*) _temp1049.args; goto _LL1054; _LL1054:
_temp1053=( int) _temp1049.varargs; goto _LL1052; _LL1052: _temp1051=( struct
Cyc_List_List*) _temp1049.attributes; goto _LL1032;} else{ goto _LL789;} _LL1032:
_temp1031= _temp755.f2; if(( unsigned int) _temp1031 > 4u?(( struct
_tunion_struct*) _temp1031)->tag == Cyc_Absyn_FnType_tag: 0){ _LL1034: _temp1033=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp1031)->f1;
_LL1046: _temp1045=( struct Cyc_List_List*) _temp1033.tvars; goto _LL1044;
_LL1044: _temp1043=( struct Cyc_Core_Opt*) _temp1033.effect; goto _LL1042;
_LL1042: _temp1041=( void*) _temp1033.ret_typ; goto _LL1040; _LL1040: _temp1039=(
struct Cyc_List_List*) _temp1033.args; goto _LL1038; _LL1038: _temp1037=( int)
_temp1033.varargs; goto _LL1036; _LL1036: _temp1035=( struct Cyc_List_List*)
_temp1033.attributes; goto _LL788;} else{ goto _LL789;} _LL789: _LL1068:
_temp1067= _temp755.f1; if(( unsigned int) _temp1067 > 4u?(( struct
_tunion_struct*) _temp1067)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL1070:
_temp1069=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1067)->f1; goto _LL1064;} else{ goto _LL791;} _LL1064: _temp1063= _temp755.f2;
if(( unsigned int) _temp1063 > 4u?(( struct _tunion_struct*) _temp1063)->tag ==
Cyc_Absyn_TupleType_tag: 0){ _LL1066: _temp1065=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1063)->f1; goto _LL790;} else{ goto
_LL791;} _LL791: _LL1074: _temp1073= _temp755.f1; if(( int) _temp1073 == Cyc_Absyn_HeapRgn){
goto _LL1072;} else{ goto _LL793;} _LL1072: _temp1071= _temp755.f2; if(( int)
_temp1071 == Cyc_Absyn_HeapRgn){ goto _LL792;} else{ goto _LL793;} _LL793:
_LL1080: _temp1079= _temp755.f1; if(( unsigned int) _temp1079 > 4u?(( struct
_tunion_struct*) _temp1079)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL1082:
_temp1081=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1079)->f1;
goto _LL1076;} else{ goto _LL795;} _LL1076: _temp1075= _temp755.f2; if((
unsigned int) _temp1075 > 4u?(( struct _tunion_struct*) _temp1075)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL1078: _temp1077=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1075)->f1; goto _LL794;} else{ goto _LL795;} _LL795: _LL1086: _temp1085=
_temp755.f1; if(( unsigned int) _temp1085 > 4u?(( struct _tunion_struct*)
_temp1085)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL1088: _temp1087=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp1085)->f1; goto _LL1084;} else{ goto
_LL797;} _LL1084: _temp1083= _temp755.f2; goto _LL796; _LL797: _LL1094:
_temp1093= _temp755.f1; goto _LL1090; _LL1090: _temp1089= _temp755.f2; if((
unsigned int) _temp1089 > 4u?(( struct _tunion_struct*) _temp1089)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL1092: _temp1091=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp1089)->f1; goto _LL798;} else{ goto _LL799;} _LL799: _LL1098: _temp1097=
_temp755.f1; if(( unsigned int) _temp1097 > 4u?(( struct _tunion_struct*)
_temp1097)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL1100: _temp1099=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp1097)->f1; goto _LL1096;} else{ goto
_LL801;} _LL1096: _temp1095= _temp755.f2; goto _LL800; _LL801: _LL1106:
_temp1105= _temp755.f1; goto _LL1102; _LL1102: _temp1101= _temp755.f2; if((
unsigned int) _temp1101 > 4u?(( struct _tunion_struct*) _temp1101)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL1104: _temp1103=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1101)->f1;
goto _LL802;} else{ goto _LL803;} _LL803: goto _LL804; _LL758: Cyc_Tcutil_unify_it(
t2, t1); return; _LL760: return; _LL762: { struct _tagged_string* x2= _temp825->name;
struct _tagged_string* x1= _temp821->name; void* k2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp825->kind); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp821->kind); if( Cyc_String_zstrptrcmp( x2, x1)
== 0){ if( k2 != k1){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp1107=* x2; struct _tagged_string _temp1108= Cyc_Absynpp_kind2string(
k2); struct _tagged_string _temp1109= Cyc_Absynpp_kind2string( k1); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp1107.last_plus_one - _temp1107.curr, _temp1107.curr, _temp1108.last_plus_one
- _temp1108.curr, _temp1108.curr, _temp1109.last_plus_one - _temp1109.curr,
_temp1109.curr);}));} return;} goto _LL756;} _LL764: if((( _temp833 != 0?
_temp841 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*)({ struct _tuple1*
_temp1110= _temp833; if( _temp1110 == 0){ _throw( Null_Exception);} _temp1110;}),(
struct _tuple1*)({ struct _tuple1* _temp1111= _temp841; if( _temp1111 == 0){
_throw( Null_Exception);} _temp1111;})) == 0: 0)? 1:( _temp833 == 0? _temp841 ==
0: 0)){ Cyc_Tcutil_unify_list( _temp831, _temp839); return;} goto _LL756; _LL766:
if( Cyc_Absyn_qvar_cmp( _temp853, _temp847) == 0){ return;} goto _LL756; _LL768:
if((( _temp861 != 0? _temp869 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*)({
struct _tuple1* _temp1112= _temp861; if( _temp1112 == 0){ _throw( Null_Exception);}
_temp1112;}),( struct _tuple1*)({ struct _tuple1* _temp1113= _temp869; if(
_temp1113 == 0){ _throw( Null_Exception);} _temp1113;})) == 0: 0)? 1:( _temp861
== 0? _temp869 == 0: 0)){ Cyc_Tcutil_unify_list( _temp859, _temp867); return;}
goto _LL756; _LL770: if( Cyc_Absyn_qvar_cmp( _temp881, _temp893) == 0){ Cyc_Tcutil_unify_it(
_temp877, _temp889); Cyc_Tcutil_unify_list( _temp879, _temp891); return;} goto
_LL756; _LL772: if( Cyc_Absyn_qvar_cmp( _temp903, _temp913) == 0){ Cyc_Tcutil_unify_it(
_temp901, _temp911); return;} goto _LL756; _LL774: if( Cyc_Absyn_qvar_cmp(
_temp927, _temp941) == 0? Cyc_Absyn_qvar_cmp( _temp923, _temp937) == 0: 0){ Cyc_Tcutil_unify_list(
_temp925, _temp939); return;} goto _LL756; _LL776: if( Cyc_Absyn_qvar_cmp(
_temp953, _temp965) == 0? Cyc_Absyn_qvar_cmp( _temp951, _temp963) == 0: 0){
return;} goto _LL756; _LL778: Cyc_Tcutil_unify_it( _temp979, _temp993); Cyc_Tcutil_unify_it(
_temp991, _temp977); Cyc_Tcutil_unify_tqual( _temp973, _temp987);(( void(*)( int(*
cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp, _temp971, _temp985);{ void*
_temp1114=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp971))->v; void* _temp1120; _LL1116: if((
unsigned int) _temp1114 > 1u?(( struct _tunion_struct*) _temp1114)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL1121: _temp1120=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1114)->f1;
if(( int) _temp1120 == Cyc_Absyn_Unknown_b){ goto _LL1117;} else{ goto _LL1118;}}
else{ goto _LL1118;} _LL1118: goto _LL1119; _LL1117: return; _LL1119: goto
_LL1115; _LL1115:;}(( void(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Core_intcmp,
_temp975, _temp989); return; _LL780: if( _temp999 == _temp1005? _temp997 ==
_temp1003: 0){ return;} goto _LL756; _LL782: return; _LL784: return; _LL786: Cyc_Tcutil_unify_it(
_temp1021, _temp1029); Cyc_Tcutil_unify_tqual( _temp1019, _temp1027); if(
_temp1025 == _temp1017){ return;} if( _temp1025 == 0? 1: _temp1017 == 0){ goto
_LL756;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1122= _temp1025; if( _temp1122 == 0){ _throw( Null_Exception);} _temp1122;}))
== Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1123= _temp1017; if( _temp1123 == 0){ _throw( Null_Exception);} _temp1123;}))){
return;} goto _LL756; _LL788: { struct Cyc_List_List* inst= 0; while( _temp1045
!= 0) { if( _temp1061 == 0){ break;} inst=({ struct Cyc_List_List* _temp1124=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1124->hd=(
void*)({ struct _tuple5* _temp1125=( struct _tuple5*) GC_malloc( sizeof( struct
_tuple5)); _temp1125->f1=( struct Cyc_Absyn_Tvar*) _temp1061->hd; _temp1125->f2=(
void*)({ struct Cyc_Absyn_VarType_struct* _temp1126=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1126[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1127; _temp1127.tag= Cyc_Absyn_VarType_tag;
_temp1127.f1=( struct Cyc_Absyn_Tvar*) _temp1045->hd; _temp1127;}); _temp1126;});
_temp1125;}); _temp1124->tl= inst; _temp1124;}); _temp1045= _temp1045->tl;
_temp1061= _temp1061->tl;} if( _temp1061 != 0){ goto _LL756;} if( inst != 0){
Cyc_Tcutil_unify_it(( void*)({ struct Cyc_Absyn_FnType_struct* _temp1128=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1128[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1129; _temp1129.tag= Cyc_Absyn_FnType_tag;
_temp1129.f1=({ struct Cyc_Absyn_FnInfo _temp1130; _temp1130.tvars= 0; _temp1130.effect=
_temp1043; _temp1130.ret_typ=( void*) _temp1041; _temp1130.args= _temp1039;
_temp1130.varargs= _temp1037; _temp1130.attributes= _temp1035; _temp1130;});
_temp1129;}); _temp1128;}), Cyc_Tcutil_substitute( inst,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1131=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1131[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1132; _temp1132.tag= Cyc_Absyn_FnType_tag;
_temp1132.f1=({ struct Cyc_Absyn_FnInfo _temp1133; _temp1133.tvars= 0; _temp1133.effect=
_temp1059; _temp1133.ret_typ=( void*) _temp1057; _temp1133.args= _temp1055;
_temp1133.varargs= _temp1053; _temp1133.attributes= _temp1051; _temp1133;});
_temp1132;}); _temp1131;}))); return;} Cyc_Tcutil_unify_it( _temp1041, _temp1057);
for( 0; _temp1039 != 0? _temp1055 != 0: 0; _temp1039= _temp1039->tl, _temp1055=
_temp1055->tl){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*) _temp1039->hd)).f2,(*((
struct _tuple2*) _temp1055->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)
_temp1039->hd)).f3,(*(( struct _tuple2*) _temp1055->hd)).f3);} if( _temp1039 !=
0? 1: _temp1055 != 0){ goto _LL756;} if( _temp1037 != _temp1053){ goto _LL756;}{
int bad_effect= 0;{ struct _tuple10 _temp1135=({ struct _tuple10 _temp1134;
_temp1134.f1= _temp1043; _temp1134.f2= _temp1059; _temp1134;}); struct Cyc_Core_Opt*
_temp1145; struct Cyc_Core_Opt* _temp1147; struct Cyc_Core_Opt* _temp1149;
struct Cyc_Core_Opt* _temp1151; struct Cyc_Core_Opt* _temp1153; struct Cyc_Core_Opt*
_temp1155; _LL1137: _LL1148: _temp1147= _temp1135.f1; if( _temp1147 == 0){ goto
_LL1146;} else{ goto _LL1139;} _LL1146: _temp1145= _temp1135.f2; if( _temp1145
== 0){ goto _LL1138;} else{ goto _LL1139;} _LL1139: _LL1152: _temp1151=
_temp1135.f1; if( _temp1151 == 0){ goto _LL1150;} else{ goto _LL1141;} _LL1150:
_temp1149= _temp1135.f2; goto _LL1140; _LL1141: _LL1156: _temp1155= _temp1135.f1;
goto _LL1154; _LL1154: _temp1153= _temp1135.f2; if( _temp1153 == 0){ goto
_LL1142;} else{ goto _LL1143;} _LL1143: goto _LL1144; _LL1138: goto _LL1136;
_LL1140: goto _LL1142; _LL1142: bad_effect= 1; goto _LL1136; _LL1144: bad_effect=
! Cyc_Tcutil_unify_effect(( void*) _temp1043->v,( void*) _temp1059->v); goto
_LL1136; _LL1136:;} if( bad_effect){ goto _LL756;} if( ! Cyc_Tcutil_same_atts(
_temp1051, _temp1035)){ goto _LL756;} return;}} _LL790: for( 0; _temp1065 != 0?
_temp1069 != 0: 0; _temp1065= _temp1065->tl, _temp1069= _temp1069->tl){ Cyc_Tcutil_unify_tqual((*((
struct _tuple4*) _temp1065->hd)).f1,(*(( struct _tuple4*) _temp1069->hd)).f1);
Cyc_Tcutil_unify_it((*(( struct _tuple4*) _temp1065->hd)).f2,(*(( struct _tuple4*)
_temp1069->hd)).f2);} if( _temp1065 == 0? _temp1069 == 0: 0){ return;} goto
_LL756; _LL792: return; _LL794: Cyc_Tcutil_unify_it( _temp1081, _temp1077);
return; _LL796: goto _LL798; _LL798: goto _LL800; _LL800: goto _LL802; _LL802:
if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto _LL756;} _LL804: goto
_LL756; _LL756:;}( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Tcutil_Unify_struct*
_temp1157=( struct Cyc_Tcutil_Unify_struct*) GC_malloc( sizeof( struct Cyc_Tcutil_Unify_struct));
_temp1157[ 0]=({ struct Cyc_Tcutil_Unify_struct _temp1158; _temp1158.tag= Cyc_Tcutil_Unify_tag;
_temp1158;}); _temp1157;}));} int Cyc_Tcutil_is_arithmetic_type( void* t){ void*
_temp1159= Cyc_Tcutil_compress( t); void* _temp1171; void* _temp1173; struct Cyc_Absyn_Enumdecl*
_temp1175; struct _tuple1* _temp1177; _LL1161: if(( unsigned int) _temp1159 > 4u?((
struct _tunion_struct*) _temp1159)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1174:
_temp1173=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1159)->f1; goto
_LL1172; _LL1172: _temp1171=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1159)->f2; goto _LL1162;} else{ goto _LL1163;} _LL1163: if(( int) _temp1159
== Cyc_Absyn_FloatType){ goto _LL1164;} else{ goto _LL1165;} _LL1165: if(( int)
_temp1159 == Cyc_Absyn_DoubleType){ goto _LL1166;} else{ goto _LL1167;} _LL1167:
if(( unsigned int) _temp1159 > 4u?(( struct _tunion_struct*) _temp1159)->tag ==
Cyc_Absyn_EnumType_tag: 0){ _LL1178: _temp1177=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1159)->f1; goto _LL1176; _LL1176: _temp1175=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1159)->f2; goto _LL1168;} else{ goto
_LL1169;} _LL1169: goto _LL1170; _LL1162: return 1; _LL1164: return 1; _LL1166:
return 1; _LL1168: return 1; _LL1170: return 0; _LL1160:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp1180=({ struct _tuple7 _temp1179; _temp1179.f1= t1;
_temp1179.f2= t2; _temp1179;}); void* _temp1202; void* _temp1204; void*
_temp1206; void* _temp1208; void* _temp1210; void* _temp1212; void* _temp1214;
void* _temp1216; void* _temp1218; void* _temp1220; void* _temp1222; void*
_temp1224; void* _temp1226; void* _temp1228; void* _temp1230; void* _temp1232;
void* _temp1234; void* _temp1236; void* _temp1238; void* _temp1240; void*
_temp1242; void* _temp1244; void* _temp1246; void* _temp1248; void* _temp1250;
void* _temp1252; void* _temp1254; void* _temp1256; void* _temp1258; void*
_temp1260; void* _temp1262; void* _temp1264; void* _temp1266; void* _temp1268;
void* _temp1270; void* _temp1272; void* _temp1274; void* _temp1276; void*
_temp1278; void* _temp1280; void* _temp1282; void* _temp1284; _LL1182: _LL1205:
_temp1204= _temp1180.f1; if(( int) _temp1204 == Cyc_Absyn_DoubleType){ goto
_LL1203;} else{ goto _LL1184;} _LL1203: _temp1202= _temp1180.f2; if(( int)
_temp1202 == Cyc_Absyn_FloatType){ goto _LL1183;} else{ goto _LL1184;} _LL1184:
_LL1213: _temp1212= _temp1180.f1; if(( int) _temp1212 == Cyc_Absyn_DoubleType){
goto _LL1207;} else{ goto _LL1186;} _LL1207: _temp1206= _temp1180.f2; if((
unsigned int) _temp1206 > 4u?(( struct _tunion_struct*) _temp1206)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1211: _temp1210=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1206)->f1;
goto _LL1209; _LL1209: _temp1208=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1206)->f2; goto _LL1185;} else{ goto _LL1186;} _LL1186: _LL1221: _temp1220=
_temp1180.f1; if(( int) _temp1220 == Cyc_Absyn_FloatType){ goto _LL1215;} else{
goto _LL1188;} _LL1215: _temp1214= _temp1180.f2; if(( unsigned int) _temp1214 >
4u?(( struct _tunion_struct*) _temp1214)->tag == Cyc_Absyn_IntType_tag: 0){
_LL1219: _temp1218=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1214)->f1;
goto _LL1217; _LL1217: _temp1216=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1214)->f2; goto _LL1187;} else{ goto _LL1188;} _LL1188: _LL1229: _temp1228=
_temp1180.f1; if(( unsigned int) _temp1228 > 4u?(( struct _tunion_struct*)
_temp1228)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1233: _temp1232=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1228)->f1; goto _LL1231; _LL1231:
_temp1230=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1228)->f2; if(( int)
_temp1230 == Cyc_Absyn_B8){ goto _LL1223;} else{ goto _LL1190;}} else{ goto
_LL1190;} _LL1223: _temp1222= _temp1180.f2; if(( unsigned int) _temp1222 > 4u?((
struct _tunion_struct*) _temp1222)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1227:
_temp1226=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1222)->f1; goto
_LL1225; _LL1225: _temp1224=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1222)->f2; if(( int) _temp1224 == Cyc_Absyn_B8){ goto _LL1189;} else{ goto
_LL1190;}} else{ goto _LL1190;} _LL1190: _LL1237: _temp1236= _temp1180.f1; if((
unsigned int) _temp1236 > 4u?(( struct _tunion_struct*) _temp1236)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1241: _temp1240=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1236)->f1;
goto _LL1239; _LL1239: _temp1238=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1236)->f2; if(( int) _temp1238 == Cyc_Absyn_B8){ goto _LL1235;} else{ goto
_LL1192;}} else{ goto _LL1192;} _LL1235: _temp1234= _temp1180.f2; goto _LL1191;
_LL1192: _LL1245: _temp1244= _temp1180.f1; if(( unsigned int) _temp1244 > 4u?((
struct _tunion_struct*) _temp1244)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1249:
_temp1248=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1244)->f1; goto
_LL1247; _LL1247: _temp1246=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1244)->f2; if(( int) _temp1246 == Cyc_Absyn_B4){ goto _LL1243;} else{ goto
_LL1194;}} else{ goto _LL1194;} _LL1243: _temp1242= _temp1180.f2; if(( int)
_temp1242 == Cyc_Absyn_FloatType){ goto _LL1193;} else{ goto _LL1194;} _LL1194:
_LL1257: _temp1256= _temp1180.f1; if(( unsigned int) _temp1256 > 4u?(( struct
_tunion_struct*) _temp1256)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1261:
_temp1260=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1256)->f1; goto
_LL1259; _LL1259: _temp1258=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1256)->f2; if(( int) _temp1258 == Cyc_Absyn_B4){ goto _LL1251;} else{ goto
_LL1196;}} else{ goto _LL1196;} _LL1251: _temp1250= _temp1180.f2; if((
unsigned int) _temp1250 > 4u?(( struct _tunion_struct*) _temp1250)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1255: _temp1254=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1250)->f1;
goto _LL1253; _LL1253: _temp1252=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1250)->f2; if(( int) _temp1252 == Cyc_Absyn_B2){ goto _LL1195;} else{ goto
_LL1196;}} else{ goto _LL1196;} _LL1196: _LL1269: _temp1268= _temp1180.f1; if((
unsigned int) _temp1268 > 4u?(( struct _tunion_struct*) _temp1268)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1273: _temp1272=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1268)->f1;
goto _LL1271; _LL1271: _temp1270=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1268)->f2; if(( int) _temp1270 == Cyc_Absyn_B4){ goto _LL1263;} else{ goto
_LL1198;}} else{ goto _LL1198;} _LL1263: _temp1262= _temp1180.f2; if((
unsigned int) _temp1262 > 4u?(( struct _tunion_struct*) _temp1262)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1267: _temp1266=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1262)->f1;
goto _LL1265; _LL1265: _temp1264=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1262)->f2; if(( int) _temp1264 == Cyc_Absyn_B1){ goto _LL1197;} else{ goto
_LL1198;}} else{ goto _LL1198;} _LL1198: _LL1281: _temp1280= _temp1180.f1; if((
unsigned int) _temp1280 > 4u?(( struct _tunion_struct*) _temp1280)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1285: _temp1284=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1280)->f1;
goto _LL1283; _LL1283: _temp1282=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1280)->f2; if(( int) _temp1282 == Cyc_Absyn_B2){ goto _LL1275;} else{ goto
_LL1200;}} else{ goto _LL1200;} _LL1275: _temp1274= _temp1180.f2; if((
unsigned int) _temp1274 > 4u?(( struct _tunion_struct*) _temp1274)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1279: _temp1278=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1274)->f1;
goto _LL1277; _LL1277: _temp1276=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1274)->f2; if(( int) _temp1276 == Cyc_Absyn_B1){ goto _LL1199;} else{ goto
_LL1200;}} else{ goto _LL1200;} _LL1200: goto _LL1201; _LL1183: return 1;
_LL1185: return 1; _LL1187: return 1; _LL1189: return 0; _LL1191: return 1;
_LL1193: return 1; _LL1195: return 1; _LL1197: return 1; _LL1199: return 1;
_LL1201: return 0; _LL1181:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el= el->tl){ void* t1= Cyc_Tcutil_compress((
void*)((( struct Cyc_Absyn_Exp*) el->hd)->topt)->v); if( Cyc_Tcutil_is_arithmetic_type(
t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision( t1,( void*)
max_arith_type->v)){ max_arith_type=({ struct Cyc_Core_Opt* _temp1286=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1286->v=( void*)
t1; _temp1286;});}}}} if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)
max_arith_type->v)){ return 0;}}{ struct Cyc_List_List* el= es; for( 0; el != 0;
el= el->tl){ if( ! Cyc_Tcutil_coerce_assign( te,( struct Cyc_Absyn_Exp*) el->hd,
t)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) el->hd)->loc,({ struct
_tagged_string _temp1287= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1288= Cyc_Absynpp_typ2string(( void*)((( struct Cyc_Absyn_Exp*) el->hd)->topt)->v);
xprintf("type mismatch: expecting %.*s but found %.*s", _temp1287.last_plus_one
- _temp1287.curr, _temp1287.curr, _temp1288.last_plus_one - _temp1288.curr,
_temp1288.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1289= Cyc_Tcutil_compress(( void*)( e->topt)->v); struct Cyc_Absyn_PtrInfo
_temp1295; _LL1291: if(( unsigned int) _temp1289 > 4u?(( struct _tunion_struct*)
_temp1289)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1296: _temp1295=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1289)->f1; goto
_LL1292;} else{ goto _LL1293;} _LL1293: goto _LL1294; _LL1292: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1290; _LL1294: return 0; _LL1290:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1297= Cyc_Tcutil_compress(
t); void* _temp1305; void* _temp1307; struct Cyc_Absyn_Enumdecl* _temp1309;
struct _tuple1* _temp1311; _LL1299: if(( unsigned int) _temp1297 > 4u?(( struct
_tunion_struct*) _temp1297)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1308:
_temp1307=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1297)->f1; goto
_LL1306; _LL1306: _temp1305=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1297)->f2; goto _LL1300;} else{ goto _LL1301;} _LL1301: if(( unsigned int)
_temp1297 > 4u?(( struct _tunion_struct*) _temp1297)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1312: _temp1311=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1297)->f1; goto _LL1310; _LL1310: _temp1309=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1297)->f2; goto _LL1302;} else{ goto
_LL1303;} _LL1303: goto _LL1304; _LL1300: return 1; _LL1302: return 1; _LL1304:
return 0; _LL1298:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)( e->topt)->v, Cyc_Absyn_uint_t)){
return 1;} if( Cyc_Tcutil_is_integral_type(( void*)( e->topt)->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)( e->topt)->v, Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp1313=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1314; _temp1314.curr= _temp1313; _temp1314.base=
_temp1313; _temp1314.last_plus_one= _temp1313 + 44; _temp1314;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(
e->topt)->v, Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)( e->topt)->v)){ if( Cyc_Tcutil_will_lose_precision(( void*)( e->topt)->v,
Cyc_Absyn_sint_t)){ Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ char*
_temp1315=( char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1316; _temp1316.curr= _temp1315; _temp1316.base= _temp1315;
_temp1316.last_plus_one= _temp1315 + 44; _temp1316;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7
_temp1318=({ struct _tuple7 _temp1317; _temp1317.f1= t1; _temp1317.f2= t2;
_temp1317;}); void* _temp1332; struct Cyc_Absyn_PtrInfo _temp1334; void*
_temp1336; struct Cyc_Absyn_PtrInfo _temp1338; void* _temp1340; struct Cyc_Absyn_Exp*
_temp1342; struct Cyc_Absyn_Tqual* _temp1344; void* _temp1346; void* _temp1348;
struct Cyc_Absyn_Exp* _temp1350; struct Cyc_Absyn_Tqual* _temp1352; void*
_temp1354; void* _temp1356; struct Cyc_Absyn_TunionInfo _temp1358; struct Cyc_Absyn_Tuniondecl*
_temp1360; void* _temp1362; struct Cyc_List_List* _temp1364; struct _tuple1*
_temp1366; void* _temp1368; struct Cyc_Absyn_TunionFieldInfo _temp1370; struct
Cyc_Absyn_Tunionfield* _temp1372; struct Cyc_Absyn_Tuniondecl* _temp1374; struct
_tuple1* _temp1376; struct Cyc_List_List* _temp1378; struct _tuple1* _temp1380;
void* _temp1382; struct Cyc_Absyn_TunionInfo _temp1384; struct Cyc_Absyn_Tuniondecl*
_temp1386; void* _temp1388; struct Cyc_List_List* _temp1390; struct _tuple1*
_temp1392; void* _temp1394; struct Cyc_Absyn_PtrInfo _temp1396; struct Cyc_Absyn_Conref*
_temp1398; struct Cyc_Absyn_Tqual* _temp1400; struct Cyc_Absyn_Conref* _temp1402;
void* _temp1404; void* _temp1406; void* _temp1408; struct Cyc_Absyn_XTunionInfo
_temp1410; struct Cyc_Absyn_XTuniondecl* _temp1412; void* _temp1414; struct
_tuple1* _temp1416; void* _temp1418; struct Cyc_Absyn_PtrInfo _temp1420; struct
Cyc_Absyn_Conref* _temp1422; struct Cyc_Absyn_Tqual* _temp1424; struct Cyc_Absyn_Conref*
_temp1426; void* _temp1428; void* _temp1430; _LL1320: _LL1337: _temp1336=
_temp1318.f1; if(( unsigned int) _temp1336 > 4u?(( struct _tunion_struct*)
_temp1336)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1339: _temp1338=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1336)->f1; goto
_LL1333;} else{ goto _LL1322;} _LL1333: _temp1332= _temp1318.f2; if((
unsigned int) _temp1332 > 4u?(( struct _tunion_struct*) _temp1332)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1335: _temp1334=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1332)->f1; goto _LL1321;} else{ goto _LL1322;} _LL1322: _LL1349: _temp1348=
_temp1318.f1; if(( unsigned int) _temp1348 > 4u?(( struct _tunion_struct*)
_temp1348)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1355: _temp1354=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1348)->f1; goto _LL1353; _LL1353:
_temp1352=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1348)->f2; goto _LL1351; _LL1351: _temp1350=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1348)->f3; goto _LL1341;} else{ goto
_LL1324;} _LL1341: _temp1340= _temp1318.f2; if(( unsigned int) _temp1340 > 4u?((
struct _tunion_struct*) _temp1340)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1347:
_temp1346=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1340)->f1; goto
_LL1345; _LL1345: _temp1344=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1340)->f2; goto _LL1343; _LL1343: _temp1342=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1340)->f3; goto _LL1323;} else{ goto
_LL1324;} _LL1324: _LL1369: _temp1368= _temp1318.f1; if(( unsigned int)
_temp1368 > 4u?(( struct _tunion_struct*) _temp1368)->tag == Cyc_Absyn_TunionFieldType_tag:
0){ _LL1371: _temp1370=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1368)->f1; _LL1381: _temp1380=( struct _tuple1*) _temp1370.name; goto
_LL1379; _LL1379: _temp1378=( struct Cyc_List_List*) _temp1370.targs; goto
_LL1377; _LL1377: _temp1376=( struct _tuple1*) _temp1370.fname; goto _LL1375;
_LL1375: _temp1374=( struct Cyc_Absyn_Tuniondecl*) _temp1370.tud; goto _LL1373;
_LL1373: _temp1372=( struct Cyc_Absyn_Tunionfield*) _temp1370.tufd; goto _LL1357;}
else{ goto _LL1326;} _LL1357: _temp1356= _temp1318.f2; if(( unsigned int)
_temp1356 > 4u?(( struct _tunion_struct*) _temp1356)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL1359: _temp1358=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1356)->f1; _LL1367: _temp1366=( struct _tuple1*) _temp1358.name; goto
_LL1365; _LL1365: _temp1364=( struct Cyc_List_List*) _temp1358.targs; goto
_LL1363; _LL1363: _temp1362=( void*) _temp1358.rgn; goto _LL1361; _LL1361:
_temp1360=( struct Cyc_Absyn_Tuniondecl*) _temp1358.tud; goto _LL1325;} else{
goto _LL1326;} _LL1326: _LL1395: _temp1394= _temp1318.f1; if(( unsigned int)
_temp1394 > 4u?(( struct _tunion_struct*) _temp1394)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1397: _temp1396=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1394)->f1; _LL1407: _temp1406=( void*) _temp1396.elt_typ; goto _LL1405;
_LL1405: _temp1404=( void*) _temp1396.rgn_typ; goto _LL1403; _LL1403: _temp1402=(
struct Cyc_Absyn_Conref*) _temp1396.nullable; goto _LL1401; _LL1401: _temp1400=(
struct Cyc_Absyn_Tqual*) _temp1396.tq; goto _LL1399; _LL1399: _temp1398=( struct
Cyc_Absyn_Conref*) _temp1396.bounds; goto _LL1383;} else{ goto _LL1328;} _LL1383:
_temp1382= _temp1318.f2; if(( unsigned int) _temp1382 > 4u?(( struct
_tunion_struct*) _temp1382)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL1385:
_temp1384=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1382)->f1; _LL1393: _temp1392=( struct _tuple1*) _temp1384.name; goto
_LL1391; _LL1391: _temp1390=( struct Cyc_List_List*) _temp1384.targs; goto
_LL1389; _LL1389: _temp1388=( void*) _temp1384.rgn; goto _LL1387; _LL1387:
_temp1386=( struct Cyc_Absyn_Tuniondecl*) _temp1384.tud; goto _LL1327;} else{
goto _LL1328;} _LL1328: _LL1419: _temp1418= _temp1318.f1; if(( unsigned int)
_temp1418 > 4u?(( struct _tunion_struct*) _temp1418)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1421: _temp1420=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1418)->f1; _LL1431: _temp1430=( void*) _temp1420.elt_typ; goto _LL1429;
_LL1429: _temp1428=( void*) _temp1420.rgn_typ; goto _LL1427; _LL1427: _temp1426=(
struct Cyc_Absyn_Conref*) _temp1420.nullable; goto _LL1425; _LL1425: _temp1424=(
struct Cyc_Absyn_Tqual*) _temp1420.tq; goto _LL1423; _LL1423: _temp1422=( struct
Cyc_Absyn_Conref*) _temp1420.bounds; goto _LL1409;} else{ goto _LL1330;} _LL1409:
_temp1408= _temp1318.f2; if(( unsigned int) _temp1408 > 4u?(( struct
_tunion_struct*) _temp1408)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL1411:
_temp1410=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp1408)->f1; _LL1417: _temp1416=( struct _tuple1*) _temp1410.name; goto
_LL1415; _LL1415: _temp1414=( void*) _temp1410.rgn; goto _LL1413; _LL1413:
_temp1412=( struct Cyc_Absyn_XTuniondecl*) _temp1410.xtud; goto _LL1329;} else{
goto _LL1330;} _LL1330: goto _LL1331; _LL1321: { int okay= 1; if( !(( int(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1338.nullable, _temp1334.nullable)){
void* _temp1432=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1338.nullable))->v; int _temp1438; _LL1434:
if(( unsigned int) _temp1432 > 1u?(( struct _tunion_struct*) _temp1432)->tag ==
Cyc_Absyn_Eq_constr_tag: 0){ _LL1439: _temp1438=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1432)->f1; goto _LL1435;} else{ goto _LL1436;} _LL1436: goto _LL1437;
_LL1435: okay= ! _temp1438; goto _LL1433; _LL1437:( void) _throw((( struct
_xtunion_struct*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp1440=( char*)"silent_castable conref not eq";
struct _tagged_string _temp1441; _temp1441.curr= _temp1440; _temp1441.base=
_temp1440; _temp1441.last_plus_one= _temp1440 + 30; _temp1441;}))); _LL1433:;}
if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct
Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp, _temp1338.bounds,
_temp1334.bounds)){ struct _tuple7 _temp1443=({ struct _tuple7 _temp1442;
_temp1442.f1=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1338.bounds))->v; _temp1442.f2=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1334.bounds))->v; _temp1442;}); void* _temp1451; void* _temp1453; void*
_temp1455; void* _temp1457; struct Cyc_Absyn_Exp* _temp1459; void* _temp1461;
void* _temp1463; void* _temp1465; void* _temp1467; _LL1445: _LL1456: _temp1455=
_temp1443.f1; if(( unsigned int) _temp1455 > 1u?(( struct _tunion_struct*)
_temp1455)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1458: _temp1457=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1455)->f1; if(( unsigned int) _temp1457
> 1u?(( struct _tunion_struct*) _temp1457)->tag == Cyc_Absyn_Upper_b_tag: 0){
_LL1460: _temp1459=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1457)->f1; goto _LL1452;} else{ goto _LL1447;}} else{ goto _LL1447;}
_LL1452: _temp1451= _temp1443.f2; if(( unsigned int) _temp1451 > 1u?(( struct
_tunion_struct*) _temp1451)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1454:
_temp1453=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1451)->f1; if((
int) _temp1453 == Cyc_Absyn_Unknown_b){ goto _LL1446;} else{ goto _LL1447;}}
else{ goto _LL1447;} _LL1447: _LL1466: _temp1465= _temp1443.f1; if((
unsigned int) _temp1465 > 1u?(( struct _tunion_struct*) _temp1465)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL1468: _temp1467=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1465)->f1;
if(( int) _temp1467 == Cyc_Absyn_Unknown_b){ goto _LL1462;} else{ goto _LL1449;}}
else{ goto _LL1449;} _LL1462: _temp1461= _temp1443.f2; if(( unsigned int)
_temp1461 > 1u?(( struct _tunion_struct*) _temp1461)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL1464: _temp1463=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1461)->f1;
if(( int) _temp1463 == Cyc_Absyn_Unknown_b){ goto _LL1448;} else{ goto _LL1449;}}
else{ goto _LL1449;} _LL1449: goto _LL1450; _LL1446: okay= 1; goto _LL1444;
_LL1448: okay= 1; goto _LL1444; _LL1450: okay= 0; goto _LL1444; _LL1444:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1338.elt_typ,( void*) _temp1334.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1338.rgn_typ,( void*) _temp1334.rgn_typ): 0)?
!( _temp1338.tq)->q_const? 1:( _temp1334.tq)->q_const: 0;} _LL1323: { int okay;
okay=( _temp1350 != 0? _temp1342 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1469= _temp1350; if(
_temp1469 == 0){ _throw( Null_Exception);} _temp1469;})) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1470= _temp1342; if(
_temp1470 == 0){ _throw( Null_Exception);} _temp1470;})): 0; return( okay? Cyc_Tcutil_unify(
_temp1354, _temp1346): 0)? ! _temp1352->q_const? 1: _temp1344->q_const: 0;}
_LL1325: if( Cyc_Absyn_qvar_cmp( _temp1380, _temp1366) == 0? _temp1372->typs ==
0: 0){ int okay= 1; for( 0; _temp1378 != 0? _temp1364 != 0: 0; _temp1378=
_temp1378->tl, _temp1364= _temp1364->tl){ if( ! Cyc_Tcutil_unify(( void*)
_temp1378->hd,( void*) _temp1364->hd)){ okay= 0; break;}} if(( okay? _temp1378
== 0: 0)? _temp1364 == 0: 0){ return 1;}} return 0; _LL1327:{ void* _temp1471=
Cyc_Tcutil_compress( _temp1406); struct Cyc_Absyn_TunionFieldInfo _temp1477;
struct Cyc_Absyn_Tunionfield* _temp1479; struct Cyc_Absyn_Tuniondecl* _temp1481;
struct _tuple1* _temp1483; struct Cyc_List_List* _temp1485; struct _tuple1*
_temp1487; _LL1473: if(( unsigned int) _temp1471 > 4u?(( struct _tunion_struct*)
_temp1471)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL1478: _temp1477=(
struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1471)->f1; _LL1488: _temp1487=( struct _tuple1*) _temp1477.name; goto
_LL1486; _LL1486: _temp1485=( struct Cyc_List_List*) _temp1477.targs; goto
_LL1484; _LL1484: _temp1483=( struct _tuple1*) _temp1477.fname; goto _LL1482;
_LL1482: _temp1481=( struct Cyc_Absyn_Tuniondecl*) _temp1477.tud; goto _LL1480;
_LL1480: _temp1479=( struct Cyc_Absyn_Tunionfield*) _temp1477.tufd; goto _LL1474;}
else{ goto _LL1475;} _LL1475: goto _LL1476; _LL1474: if( ! Cyc_Tcutil_unify(
_temp1404, _temp1388)? _temp1404 !=( void*) Cyc_Absyn_HeapRgn: 0){ return 0;}
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1402,(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0))){ return 0;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1398,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1489=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1489[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1490; _temp1490.tag= Cyc_Absyn_Upper_b_tag;
_temp1490.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1490;}); _temp1489;})))){ return 0;}
if(( Cyc_Absyn_qvar_cmp( _temp1487, _temp1392) == 0? _temp1479 != 0: 0)?
_temp1479->typs != 0: 0){ int okay= 1; for( 0; _temp1485 != 0? _temp1390 != 0: 0;
_temp1485= _temp1485->tl, _temp1390= _temp1390->tl){ if( ! Cyc_Tcutil_unify((
void*) _temp1485->hd,( void*) _temp1390->hd)){ okay= 0; break;}} if(( ! okay? 1:
_temp1485 != 0)? 1: _temp1390 != 0){ return 0;} return 1;} goto _LL1472; _LL1476:
goto _LL1472; _LL1472:;} return 0; _LL1329:{ void* _temp1491= Cyc_Tcutil_compress(
_temp1430); struct Cyc_Absyn_XTunionFieldInfo _temp1497; struct Cyc_Absyn_Tunionfield*
_temp1499; struct Cyc_Absyn_XTuniondecl* _temp1501; struct _tuple1* _temp1503;
struct _tuple1* _temp1505; _LL1493: if(( unsigned int) _temp1491 > 4u?(( struct
_tunion_struct*) _temp1491)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL1498:
_temp1497=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp1491)->f1; _LL1506: _temp1505=( struct _tuple1*) _temp1497.name; goto
_LL1504; _LL1504: _temp1503=( struct _tuple1*) _temp1497.fname; goto _LL1502;
_LL1502: _temp1501=( struct Cyc_Absyn_XTuniondecl*) _temp1497.xtud; goto _LL1500;
_LL1500: _temp1499=( struct Cyc_Absyn_Tunionfield*) _temp1497.xtufd; goto
_LL1494;} else{ goto _LL1495;} _LL1495: goto _LL1496; _LL1494: if( ! Cyc_Tcutil_unify(
_temp1428, _temp1414)? _temp1428 !=( void*) Cyc_Absyn_HeapRgn: 0){ return 0;}
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1426,(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0))){ return 0;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1422,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1507=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1507[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1508; _temp1508.tag= Cyc_Absyn_Upper_b_tag;
_temp1508.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1508;}); _temp1507;})))){ return 0;}
if( Cyc_Absyn_qvar_cmp( _temp1505, _temp1416) == 0){ return 1;} goto _LL1492;
_LL1496: goto _LL1492; _LL1492:;} return 0; _LL1331: return Cyc_Tcutil_unify( t1,
t2); _LL1319:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)( e->topt)->v);
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type( t1)?
Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision( t1,
t2)){ Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ char* _temp1509=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1510; _temp1510.curr= _temp1509; _temp1510.base=
_temp1509; _temp1510.last_plus_one= _temp1509 + 44; _temp1510;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1511= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1512= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1511.last_plus_one - _temp1511.curr, _temp1511.curr, _temp1512.last_plus_one
- _temp1512.curr, _temp1512.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)( e->topt)->v);
void* t;{ void* _temp1513= old_typ; struct Cyc_Absyn_Enumdecl* _temp1525; struct
_tuple1* _temp1527; void* _temp1529; void* _temp1531; _LL1515: if(( unsigned int)
_temp1513 > 4u?(( struct _tunion_struct*) _temp1513)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1528: _temp1527=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1513)->f1; goto _LL1526; _LL1526: _temp1525=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1513)->f2; goto _LL1516;} else{ goto
_LL1517;} _LL1517: if(( unsigned int) _temp1513 > 4u?(( struct _tunion_struct*)
_temp1513)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1532: _temp1531=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1513)->f1; goto _LL1530; _LL1530:
_temp1529=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1513)->f2; goto
_LL1518;} else{ goto _LL1519;} _LL1519: if(( int) _temp1513 == Cyc_Absyn_FloatType){
goto _LL1520;} else{ goto _LL1521;} _LL1521: if(( int) _temp1513 == Cyc_Absyn_DoubleType){
goto _LL1522;} else{ goto _LL1523;} _LL1523: goto _LL1524; _LL1516: t= Cyc_Absyn_sint_t;
goto _LL1514; _LL1518:{ void* _temp1533= _temp1529; _LL1535: if(( int) _temp1533
== Cyc_Absyn_B1){ goto _LL1536;} else{ goto _LL1537;} _LL1537: if(( int)
_temp1533 == Cyc_Absyn_B2){ goto _LL1538;} else{ goto _LL1539;} _LL1539: if((
int) _temp1533 == Cyc_Absyn_B4){ goto _LL1540;} else{ goto _LL1541;} _LL1541:
if(( int) _temp1533 == Cyc_Absyn_B8){ goto _LL1542;} else{ goto _LL1534;}
_LL1536: t= Cyc_Absyn_sint_t; goto _LL1534; _LL1538: t= Cyc_Absyn_sint_t; goto
_LL1534; _LL1540: t= _temp1531 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1534; _LL1542: t= _temp1531 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1534; _LL1534:;} goto _LL1514;
_LL1520: t= Cyc_Absyn_float_t; goto _LL1514; _LL1522: t= Cyc_Absyn_double_t;
goto _LL1514; _LL1524: return 0; _LL1514:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)( e->topt)->v); if( Cyc_Tcutil_unify(
t1, t2)){ return 1;} Cyc_Tcutil_any_promote( te, e); if( Cyc_Tcutil_unify(( void*)(
e->topt)->v, t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc,
t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1543= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1544= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1543.last_plus_one - _temp1543.curr, _temp1543.curr, _temp1544.last_plus_one
- _temp1544.curr, _temp1544.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1545= Cyc_Tcutil_compress( t); void* _temp1555; void*
_temp1557; _LL1547: if(( unsigned int) _temp1545 > 4u?(( struct _tunion_struct*)
_temp1545)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1558: _temp1557=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1545)->f1; goto _LL1556; _LL1556:
_temp1555=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1545)->f2; goto
_LL1548;} else{ goto _LL1549;} _LL1549: if(( int) _temp1545 == Cyc_Absyn_FloatType){
goto _LL1550;} else{ goto _LL1551;} _LL1551: if(( int) _temp1545 == Cyc_Absyn_DoubleType){
goto _LL1552;} else{ goto _LL1553;} _LL1553: goto _LL1554; _LL1548: return 1;
_LL1550: return 1; _LL1552: return 1; _LL1554: return 0; _LL1546:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1559=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1559->f1= x->tq; _temp1559->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1559;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1560= t1; struct Cyc_List_List* _temp1570; struct Cyc_Absyn_Structdecl**
_temp1572; struct Cyc_List_List* _temp1574; struct _tuple1* _temp1576; _LL1562:
if(( int) _temp1560 == Cyc_Absyn_VoidType){ goto _LL1563;} else{ goto _LL1564;}
_LL1564: if(( unsigned int) _temp1560 > 4u?(( struct _tunion_struct*) _temp1560)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL1571: _temp1570=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1560)->f1; goto _LL1565;} else{ goto
_LL1566;} _LL1566: if(( unsigned int) _temp1560 > 4u?(( struct _tunion_struct*)
_temp1560)->tag == Cyc_Absyn_StructType_tag: 0){ _LL1577: _temp1576=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1560)->f1; goto _LL1575;
_LL1575: _temp1574=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1560)->f2; goto _LL1573; _LL1573: _temp1572=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1560)->f3; goto _LL1567;} else{ goto
_LL1568;} _LL1568: goto _LL1569; _LL1563: return 0; _LL1565: return _temp1570;
_LL1567: if( _temp1572 == 0? 1: _temp1576 == 0){ return({ struct Cyc_List_List*
_temp1578=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1578->hd=( void*)({ struct _tuple4* _temp1579=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1579->f1= Cyc_Absyn_empty_tqual(); _temp1579->f2=
t1; _temp1579;}); _temp1578->tl= 0; _temp1578;});}{ struct Cyc_Absyn_Structdecl*
sd=* _temp1572; struct _tuple1 n=* _temp1576; struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
sd->tvs, _temp1574); if( sd->fields == 0){ return({ struct Cyc_List_List*
_temp1580=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1580->hd=( void*)({ struct _tuple4* _temp1581=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1581->f1= Cyc_Absyn_empty_tqual(); _temp1581->f2=
t1; _temp1581;}); _temp1580->tl= 0; _temp1580;});} return(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f,
inst,( struct Cyc_List_List*)( sd->fields)->v);} _LL1569: return({ struct Cyc_List_List*
_temp1582=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1582->hd=( void*)({ struct _tuple4* _temp1583=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1583->f1= Cyc_Absyn_empty_tqual(); _temp1583->f2=
t1; _temp1583;}); _temp1582->tl= 0; _temp1582;}); _LL1561:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2);
static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1, t2)){ return 1;}{ struct
Cyc_List_List* a= assume; for( 0; a != 0; a= a->tl){ if( Cyc_Tcutil_unify( t1,(*((
struct _tuple7*) a->hd)).f1)? Cyc_Tcutil_unify( t2,(*(( struct _tuple7*) a->hd)).f2):
0){ return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp1585=({ struct _tuple7 _temp1584; _temp1584.f1= t1;
_temp1584.f2= t2; _temp1584;}); void* _temp1591; struct Cyc_Absyn_PtrInfo
_temp1593; struct Cyc_Absyn_Conref* _temp1595; struct Cyc_Absyn_Tqual* _temp1597;
struct Cyc_Absyn_Conref* _temp1599; void* _temp1601; void* _temp1603; void*
_temp1605; struct Cyc_Absyn_PtrInfo _temp1607; struct Cyc_Absyn_Conref*
_temp1609; struct Cyc_Absyn_Tqual* _temp1611; struct Cyc_Absyn_Conref* _temp1613;
void* _temp1615; void* _temp1617; _LL1587: _LL1606: _temp1605= _temp1585.f1; if((
unsigned int) _temp1605 > 4u?(( struct _tunion_struct*) _temp1605)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1608: _temp1607=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1605)->f1; _LL1618: _temp1617=( void*) _temp1607.elt_typ; goto _LL1616;
_LL1616: _temp1615=( void*) _temp1607.rgn_typ; goto _LL1614; _LL1614: _temp1613=(
struct Cyc_Absyn_Conref*) _temp1607.nullable; goto _LL1612; _LL1612: _temp1611=(
struct Cyc_Absyn_Tqual*) _temp1607.tq; goto _LL1610; _LL1610: _temp1609=( struct
Cyc_Absyn_Conref*) _temp1607.bounds; goto _LL1592;} else{ goto _LL1589;} _LL1592:
_temp1591= _temp1585.f2; if(( unsigned int) _temp1591 > 4u?(( struct
_tunion_struct*) _temp1591)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1594:
_temp1593=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1591)->f1; _LL1604: _temp1603=( void*) _temp1593.elt_typ; goto _LL1602;
_LL1602: _temp1601=( void*) _temp1593.rgn_typ; goto _LL1600; _LL1600: _temp1599=(
struct Cyc_Absyn_Conref*) _temp1593.nullable; goto _LL1598; _LL1598: _temp1597=(
struct Cyc_Absyn_Tqual*) _temp1593.tq; goto _LL1596; _LL1596: _temp1595=( struct
Cyc_Absyn_Conref*) _temp1593.bounds; goto _LL1588;} else{ goto _LL1589;} _LL1589:
goto _LL1590; _LL1588: if( _temp1611->q_const? ! _temp1597->q_const: 0){ return
0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct
Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1613,
_temp1599)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1613): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1599): 0){ return 0;}{ void* _temp1619= _temp1615; _LL1621: if(( int)
_temp1619 == Cyc_Absyn_HeapRgn){ goto _LL1622;} else{ goto _LL1623;} _LL1623:
goto _LL1624; _LL1622: goto _LL1620; _LL1624: Cyc_Tcutil_unify( _temp1615,
_temp1601); goto _LL1620; _LL1620:;} if( !(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1609, _temp1595)){ struct _tuple7 _temp1626=({ struct
_tuple7 _temp1625; _temp1625.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1609); _temp1625.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1595); _temp1625;}); void* _temp1634; void* _temp1636; struct Cyc_Absyn_Exp*
_temp1638; void* _temp1640; struct Cyc_Absyn_Exp* _temp1642; void* _temp1644;
struct Cyc_Absyn_Exp* _temp1646; _LL1628: _LL1637: _temp1636= _temp1626.f1; if((
unsigned int) _temp1636 > 1u?(( struct _tunion_struct*) _temp1636)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1639: _temp1638=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1636)->f1; goto _LL1635;} else{ goto _LL1630;} _LL1635: _temp1634=
_temp1626.f2; if(( int) _temp1634 == Cyc_Absyn_Unknown_b){ goto _LL1629;} else{
goto _LL1630;} _LL1630: _LL1645: _temp1644= _temp1626.f1; if(( unsigned int)
_temp1644 > 1u?(( struct _tunion_struct*) _temp1644)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1647: _temp1646=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1644)->f1; goto _LL1641;} else{ goto _LL1632;} _LL1641: _temp1640=
_temp1626.f2; if(( unsigned int) _temp1640 > 1u?(( struct _tunion_struct*)
_temp1640)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1643: _temp1642=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1640)->f1; goto _LL1631;} else{ goto
_LL1632;} _LL1632: goto _LL1633; _LL1629: goto _LL1627; _LL1631: if( Cyc_Evexp_eval_const_uint_exp(
_temp1646) < Cyc_Evexp_eval_const_uint_exp( _temp1642)){ return 0;} goto _LL1627;
_LL1633: return 0; _LL1627:;} return Cyc_Tcutil_ptrsubtype( te,({ struct Cyc_List_List*
_temp1648=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1648->hd=( void*)({ struct _tuple7* _temp1649=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1649->f1= t1; _temp1649->f2= t2; _temp1649;});
_temp1648->tl= assume; _temp1648;}), _temp1617, _temp1603); _LL1590: return 0;
_LL1586:;}} static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 != 0; tqs2= tqs2->tl, tqs1= tqs1->tl){ if( tqs1 == 0){ return 0;}{ struct
_tuple4 _temp1652; void* _temp1653; struct Cyc_Absyn_Tqual* _temp1655; struct
_tuple4* _temp1650=( struct _tuple4*) tqs1->hd; _temp1652=* _temp1650; _LL1656:
_temp1655= _temp1652.f1; goto _LL1654; _LL1654: _temp1653= _temp1652.f2; goto
_LL1651; _LL1651: { struct _tuple4 _temp1659; void* _temp1660; struct Cyc_Absyn_Tqual*
_temp1662; struct _tuple4* _temp1657=( struct _tuple4*) tqs2->hd; _temp1659=*
_temp1657; _LL1663: _temp1662= _temp1659.f1; goto _LL1661; _LL1661: _temp1660=
_temp1659.f2; goto _LL1658; _LL1658: if( _temp1662->q_const? Cyc_Tcutil_subtype(
te, assume, _temp1653, _temp1660): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1653, _temp1660)){ continue;} else{ return 0;}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1664= t2; struct Cyc_Absyn_Uniondecl** _temp1672; struct Cyc_List_List*
_temp1674; struct _tuple1* _temp1676; void* _temp1678; void* _temp1680; _LL1666:
if(( unsigned int) _temp1664 > 4u?(( struct _tunion_struct*) _temp1664)->tag ==
Cyc_Absyn_UnionType_tag: 0){ _LL1677: _temp1676=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1664)->f1; goto _LL1675; _LL1675: _temp1674=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp1664)->f2; goto _LL1673; _LL1673:
_temp1672=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1664)->f3; goto _LL1667;} else{ goto _LL1668;} _LL1668: if(( unsigned int)
_temp1664 > 4u?(( struct _tunion_struct*) _temp1664)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1681: _temp1680=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1664)->f1;
goto _LL1679; _LL1679: _temp1678=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1664)->f2; if(( int) _temp1678 == Cyc_Absyn_B4){ goto _LL1669;} else{ goto
_LL1670;}} else{ goto _LL1670;} _LL1670: goto _LL1671; _LL1667: if( _temp1672 ==
0){ goto _LL1665;}{ struct Cyc_Absyn_Uniondecl* ud=* _temp1672; if( ud->fields
== 0){ goto _LL1665;}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( ud->tvs,
_temp1674);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( ud->fields)->v;
for( 0; fs != 0; fs= fs->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
inst,( void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ return 1;}}}
goto _LL1665;}} _LL1669: if( Cyc_Tcutil_typ_kind( t2) ==( void*) Cyc_Absyn_BoxKind){
return 1;} goto _LL1665; _LL1671: goto _LL1665; _LL1665:;}{ void* _temp1682= t1;
struct Cyc_Absyn_PtrInfo _temp1698; struct Cyc_Absyn_Conref* _temp1700; struct
Cyc_Absyn_Tqual* _temp1702; struct Cyc_Absyn_Conref* _temp1704; void* _temp1706;
void* _temp1708; struct Cyc_Absyn_Exp* _temp1710; struct Cyc_Absyn_Tqual*
_temp1712; void* _temp1714; struct Cyc_Absyn_Enumdecl* _temp1716; struct _tuple1*
_temp1718; void* _temp1720; void* _temp1722; _LL1684: if(( unsigned int)
_temp1682 > 4u?(( struct _tunion_struct*) _temp1682)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1699: _temp1698=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1682)->f1; _LL1709: _temp1708=( void*) _temp1698.elt_typ; goto _LL1707;
_LL1707: _temp1706=( void*) _temp1698.rgn_typ; goto _LL1705; _LL1705: _temp1704=(
struct Cyc_Absyn_Conref*) _temp1698.nullable; goto _LL1703; _LL1703: _temp1702=(
struct Cyc_Absyn_Tqual*) _temp1698.tq; goto _LL1701; _LL1701: _temp1700=( struct
Cyc_Absyn_Conref*) _temp1698.bounds; goto _LL1685;} else{ goto _LL1686;} _LL1686:
if(( unsigned int) _temp1682 > 4u?(( struct _tunion_struct*) _temp1682)->tag ==
Cyc_Absyn_ArrayType_tag: 0){ _LL1715: _temp1714=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1682)->f1; goto _LL1713; _LL1713: _temp1712=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1682)->f2; goto _LL1711; _LL1711:
_temp1710=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1682)->f3; goto _LL1687;} else{ goto _LL1688;} _LL1688: if(( unsigned int)
_temp1682 > 4u?(( struct _tunion_struct*) _temp1682)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1719: _temp1718=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1682)->f1; goto _LL1717; _LL1717: _temp1716=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1682)->f2; goto _LL1689;} else{ goto
_LL1690;} _LL1690: if(( unsigned int) _temp1682 > 4u?(( struct _tunion_struct*)
_temp1682)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1723: _temp1722=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1682)->f1; goto _LL1721; _LL1721:
_temp1720=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1682)->f2; goto
_LL1691;} else{ goto _LL1692;} _LL1692: if(( int) _temp1682 == Cyc_Absyn_FloatType){
goto _LL1693;} else{ goto _LL1694;} _LL1694: if(( int) _temp1682 == Cyc_Absyn_DoubleType){
goto _LL1695;} else{ goto _LL1696;} _LL1696: goto _LL1697; _LL1685:{ void*
_temp1724= t2; struct Cyc_Absyn_PtrInfo _temp1730; struct Cyc_Absyn_Conref*
_temp1732; struct Cyc_Absyn_Tqual* _temp1734; struct Cyc_Absyn_Conref* _temp1736;
void* _temp1738; void* _temp1740; _LL1726: if(( unsigned int) _temp1724 > 4u?((
struct _tunion_struct*) _temp1724)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1731: _temp1730=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1724)->f1; _LL1741: _temp1740=( void*) _temp1730.elt_typ; goto _LL1739;
_LL1739: _temp1738=( void*) _temp1730.rgn_typ; goto _LL1737; _LL1737: _temp1736=(
struct Cyc_Absyn_Conref*) _temp1730.nullable; goto _LL1735; _LL1735: _temp1734=(
struct Cyc_Absyn_Tqual*) _temp1730.tq; goto _LL1733; _LL1733: _temp1732=( struct
Cyc_Absyn_Conref*) _temp1730.bounds; goto _LL1727;} else{ goto _LL1728;} _LL1728:
goto _LL1729; _LL1727: { struct Cyc_List_List* assump=({ struct Cyc_List_List*
_temp1742=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1742->hd=( void*)({ struct _tuple7* _temp1743=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1743->f1= t1; _temp1743->f2= t2; _temp1743;});
_temp1742->tl= 0; _temp1742;}); int ptrsub= Cyc_Tcutil_ptrsubtype( te, assump,
_temp1708, _temp1740)? ! _temp1702->q_const? 1: _temp1734->q_const: 0; int
bounds_ok=(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1700, _temp1732); if( ! bounds_ok){ struct _tuple7 _temp1745=({ struct
_tuple7 _temp1744; _temp1744.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1700); _temp1744.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1732); _temp1744;}); void* _temp1751; struct Cyc_Absyn_Exp* _temp1753; void*
_temp1755; struct Cyc_Absyn_Exp* _temp1757; _LL1747: _LL1756: _temp1755=
_temp1745.f1; if(( unsigned int) _temp1755 > 1u?(( struct _tunion_struct*)
_temp1755)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1758: _temp1757=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1755)->f1; goto _LL1752;} else{ goto
_LL1749;} _LL1752: _temp1751= _temp1745.f2; if(( unsigned int) _temp1751 > 1u?((
struct _tunion_struct*) _temp1751)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1754:
_temp1753=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1751)->f1;
goto _LL1748;} else{ goto _LL1749;} _LL1749: goto _LL1750; _LL1748: if( Cyc_Evexp_eval_const_uint_exp(
_temp1757) >= Cyc_Evexp_eval_const_uint_exp( _temp1753)){ bounds_ok= 1;} goto
_LL1746; _LL1750: bounds_ok= 1; goto _LL1746; _LL1746:;} return( bounds_ok?
ptrsub: 0)? _temp1706 ==( void*) Cyc_Absyn_HeapRgn? 1: Cyc_Tcutil_unify(
_temp1706, _temp1738): 0;} _LL1729: goto _LL1725; _LL1725:;} return 0; _LL1687:{
void* _temp1759= t2; struct Cyc_Absyn_Exp* _temp1765; struct Cyc_Absyn_Tqual*
_temp1767; void* _temp1769; _LL1761: if(( unsigned int) _temp1759 > 4u?(( struct
_tunion_struct*) _temp1759)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1770:
_temp1769=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1759)->f1; goto
_LL1768; _LL1768: _temp1767=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1759)->f2; goto _LL1766; _LL1766: _temp1765=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1759)->f3; goto _LL1762;} else{ goto
_LL1763;} _LL1763: goto _LL1764; _LL1762: { int okay; okay=( _temp1710 != 0?
_temp1765 != 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1771= _temp1710; if( _temp1771 == 0){ _throw(
Null_Exception);} _temp1771;})) >= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1772= _temp1765; if( _temp1772 == 0){ _throw(
Null_Exception);} _temp1772;})): 0; return( okay? Cyc_Tcutil_unify( _temp1714,
_temp1769): 0)? ! _temp1712->q_const? 1: _temp1767->q_const: 0;} _LL1764: return
0; _LL1760:;} return 0; _LL1689: goto _LL1691; _LL1691: goto _LL1693; _LL1693:
goto _LL1695; _LL1695: return Cyc_Tcutil_coerceable( t2); _LL1697: return 0;
_LL1683:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)( e->topt)->v, t)){
struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp1773=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1773[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp1774; _temp1774.tag= Cyc_Absyn_Cast_e_tag; _temp1774.f1=( void*) t;
_temp1774.f2= inner; _temp1774;}); _temp1773;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1775=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1775->v=( void*) t; _temp1775;});}} int Cyc_Tcutil_comparison_promote(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp1776=
Cyc_Tcutil_compress(( void*)( e->topt)->v); struct Cyc_Absyn_Enumdecl* _temp1788;
struct _tuple1* _temp1790; void* _temp1792; void* _temp1794; _LL1778: if((
unsigned int) _temp1776 > 4u?(( struct _tunion_struct*) _temp1776)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1791: _temp1790=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1776)->f1; goto _LL1789; _LL1789: _temp1788=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1776)->f2; goto _LL1779;} else{ goto
_LL1780;} _LL1780: if(( unsigned int) _temp1776 > 4u?(( struct _tunion_struct*)
_temp1776)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1795: _temp1794=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1776)->f1; goto _LL1793; _LL1793:
_temp1792=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1776)->f2; goto
_LL1781;} else{ goto _LL1782;} _LL1782: if(( int) _temp1776 == Cyc_Absyn_FloatType){
goto _LL1783;} else{ goto _LL1784;} _LL1784: if(( int) _temp1776 == Cyc_Absyn_DoubleType){
goto _LL1785;} else{ goto _LL1786;} _LL1786: goto _LL1787; _LL1779: t= Cyc_Absyn_sint_t;
goto _LL1777; _LL1781:{ void* _temp1796= _temp1792; _LL1798: if(( int) _temp1796
== Cyc_Absyn_B1){ goto _LL1799;} else{ goto _LL1800;} _LL1800: if(( int)
_temp1796 == Cyc_Absyn_B2){ goto _LL1801;} else{ goto _LL1802;} _LL1802: if((
int) _temp1796 == Cyc_Absyn_B4){ goto _LL1803;} else{ goto _LL1804;} _LL1804:
if(( int) _temp1796 == Cyc_Absyn_B8){ goto _LL1805;} else{ goto _LL1797;}
_LL1799: t= Cyc_Absyn_sint_t; goto _LL1797; _LL1801: t= Cyc_Absyn_sint_t; goto
_LL1797; _LL1803: t= _temp1794 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1797; _LL1805: return 1; _LL1797:;} goto _LL1777;
_LL1783: t= Cyc_Absyn_float_t; goto _LL1777; _LL1785: return 1; _LL1787: return
0; _LL1777:;} Cyc_Tcutil_unchecked_cast( te, e, t); return 1;} int Cyc_Tcutil_arithmetic_promote(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress((
void*)( e->topt)->v); void* t;{ void* _temp1806= old_typ; struct Cyc_Absyn_Enumdecl*
_temp1820; struct _tuple1* _temp1822; void* _temp1824; void* _temp1826; int
_temp1828; struct Cyc_Core_Opt* _temp1830; void* _temp1832; _LL1808: if((
unsigned int) _temp1806 > 4u?(( struct _tunion_struct*) _temp1806)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1823: _temp1822=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1806)->f1; goto _LL1821; _LL1821: _temp1820=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1806)->f2; goto _LL1809;} else{ goto
_LL1810;} _LL1810: if(( unsigned int) _temp1806 > 4u?(( struct _tunion_struct*)
_temp1806)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1827: _temp1826=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1806)->f1; goto _LL1825; _LL1825:
_temp1824=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1806)->f2; goto
_LL1811;} else{ goto _LL1812;} _LL1812: if(( int) _temp1806 == Cyc_Absyn_FloatType){
goto _LL1813;} else{ goto _LL1814;} _LL1814: if(( int) _temp1806 == Cyc_Absyn_DoubleType){
goto _LL1815;} else{ goto _LL1816;} _LL1816: if(( unsigned int) _temp1806 > 4u?((
struct _tunion_struct*) _temp1806)->tag == Cyc_Absyn_Evar_tag: 0){ _LL1833:
_temp1832=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp1806)->f1; goto _LL1831;
_LL1831: _temp1830=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp1806)->f2; goto _LL1829; _LL1829: _temp1828=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp1806)->f3; goto _LL1817;} else{ goto _LL1818;} _LL1818: goto _LL1819;
_LL1809: t= Cyc_Absyn_sint_t; goto _LL1807; _LL1811:{ void* _temp1834= _temp1824;
_LL1836: if(( int) _temp1834 == Cyc_Absyn_B1){ goto _LL1837;} else{ goto _LL1838;}
_LL1838: if(( int) _temp1834 == Cyc_Absyn_B2){ goto _LL1839;} else{ goto _LL1840;}
_LL1840: if(( int) _temp1834 == Cyc_Absyn_B4){ goto _LL1841;} else{ goto _LL1842;}
_LL1842: if(( int) _temp1834 == Cyc_Absyn_B8){ goto _LL1843;} else{ goto _LL1835;}
_LL1837: t= Cyc_Absyn_sint_t; goto _LL1835; _LL1839: t= Cyc_Absyn_sint_t; goto
_LL1835; _LL1841: t= _temp1826 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1835; _LL1843: t= _temp1826 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1835; _LL1835:;} goto _LL1807;
_LL1813: t= Cyc_Absyn_float_t; goto _LL1807; _LL1815: t= Cyc_Absyn_double_t;
goto _LL1807; _LL1817: t= Cyc_Absyn_sint_t; if( ! Cyc_Tcutil_unify( old_typ, t)){
return 0;} goto _LL1807; _LL1819: return 0; _LL1807:;} Cyc_Tcutil_unchecked_cast(
te, e, t); return 1;} int Cyc_Tcutil_integral_promote( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)( e->topt)->v);
void* t;{ void* _temp1844= old_typ; struct Cyc_Absyn_Enumdecl* _temp1854; struct
_tuple1* _temp1856; void* _temp1858; void* _temp1860; int _temp1862; struct Cyc_Core_Opt*
_temp1864; void* _temp1866; _LL1846: if(( unsigned int) _temp1844 > 4u?(( struct
_tunion_struct*) _temp1844)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL1857:
_temp1856=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1844)->f1;
goto _LL1855; _LL1855: _temp1854=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1844)->f2; goto _LL1847;} else{ goto _LL1848;} _LL1848: if(( unsigned int)
_temp1844 > 4u?(( struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1861: _temp1860=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1844)->f1;
goto _LL1859; _LL1859: _temp1858=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1844)->f2; goto _LL1849;} else{ goto _LL1850;} _LL1850: if(( unsigned int)
_temp1844 > 4u?(( struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_Evar_tag:
0){ _LL1867: _temp1866=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp1844)->f1;
goto _LL1865; _LL1865: _temp1864=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp1844)->f2; goto _LL1863; _LL1863: _temp1862=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp1844)->f3; goto _LL1851;} else{ goto _LL1852;} _LL1852: goto _LL1853;
_LL1847: t= Cyc_Absyn_sint_t; goto _LL1845; _LL1849:{ void* _temp1868= _temp1858;
_LL1870: if(( int) _temp1868 == Cyc_Absyn_B1){ goto _LL1871;} else{ goto _LL1872;}
_LL1872: if(( int) _temp1868 == Cyc_Absyn_B2){ goto _LL1873;} else{ goto _LL1874;}
_LL1874: if(( int) _temp1868 == Cyc_Absyn_B4){ goto _LL1875;} else{ goto _LL1876;}
_LL1876: if(( int) _temp1868 == Cyc_Absyn_B8){ goto _LL1877;} else{ goto _LL1869;}
_LL1871: t= Cyc_Absyn_sint_t; goto _LL1869; _LL1873: t= Cyc_Absyn_sint_t; goto
_LL1869; _LL1875: t= _temp1860 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1869; _LL1877: t= _temp1860 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1869; _LL1869:;} goto _LL1845;
_LL1851: t= Cyc_Absyn_sint_t; if( ! Cyc_Tcutil_unify( old_typ, t)){ return 0;}
goto _LL1845; _LL1853: return 0; _LL1845:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct
_tuple7 _temp1879=({ struct _tuple7 _temp1878; _temp1878.f1= t1; _temp1878.f2=
t2; _temp1878;}); void* _temp1903; void* _temp1905; void* _temp1907; void*
_temp1909; void* _temp1911; void* _temp1913; void* _temp1915; void* _temp1917;
void* _temp1919; void* _temp1921; void* _temp1923; void* _temp1925; void*
_temp1927; void* _temp1929; void* _temp1931; void* _temp1933; void* _temp1935;
void* _temp1937; void* _temp1939; void* _temp1941; void* _temp1943; void*
_temp1945; void* _temp1947; void* _temp1949; void* _temp1951; void* _temp1953;
void* _temp1955; void* _temp1957; void* _temp1959; void* _temp1961; void*
_temp1963; void* _temp1965; _LL1881: _LL1906: _temp1905= _temp1879.f1; if(( int)
_temp1905 == Cyc_Absyn_DoubleType){ goto _LL1904;} else{ goto _LL1883;} _LL1904:
_temp1903= _temp1879.f2; goto _LL1882; _LL1883: _LL1910: _temp1909= _temp1879.f1;
goto _LL1908; _LL1908: _temp1907= _temp1879.f2; if(( int) _temp1907 == Cyc_Absyn_DoubleType){
goto _LL1884;} else{ goto _LL1885;} _LL1885: _LL1914: _temp1913= _temp1879.f1;
if(( int) _temp1913 == Cyc_Absyn_FloatType){ goto _LL1912;} else{ goto _LL1887;}
_LL1912: _temp1911= _temp1879.f2; goto _LL1886; _LL1887: _LL1918: _temp1917=
_temp1879.f1; goto _LL1916; _LL1916: _temp1915= _temp1879.f2; if(( int)
_temp1915 == Cyc_Absyn_FloatType){ goto _LL1888;} else{ goto _LL1889;} _LL1889:
_LL1922: _temp1921= _temp1879.f1; if(( unsigned int) _temp1921 > 4u?(( struct
_tunion_struct*) _temp1921)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1926:
_temp1925=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1921)->f1; if(( int)
_temp1925 == Cyc_Absyn_Unsigned){ goto _LL1924;} else{ goto _LL1891;} _LL1924:
_temp1923=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1921)->f2; if(( int)
_temp1923 == Cyc_Absyn_B8){ goto _LL1920;} else{ goto _LL1891;}} else{ goto
_LL1891;} _LL1920: _temp1919= _temp1879.f2; goto _LL1890; _LL1891: _LL1934:
_temp1933= _temp1879.f1; goto _LL1928; _LL1928: _temp1927= _temp1879.f2; if((
unsigned int) _temp1927 > 4u?(( struct _tunion_struct*) _temp1927)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1932: _temp1931=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1927)->f1;
if(( int) _temp1931 == Cyc_Absyn_Unsigned){ goto _LL1930;} else{ goto _LL1893;}
_LL1930: _temp1929=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1927)->f2;
if(( int) _temp1929 == Cyc_Absyn_B8){ goto _LL1892;} else{ goto _LL1893;}} else{
goto _LL1893;} _LL1893: _LL1938: _temp1937= _temp1879.f1; if(( unsigned int)
_temp1937 > 4u?(( struct _tunion_struct*) _temp1937)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1942: _temp1941=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1937)->f1;
if(( int) _temp1941 == Cyc_Absyn_Signed){ goto _LL1940;} else{ goto _LL1895;}
_LL1940: _temp1939=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1937)->f2;
if(( int) _temp1939 == Cyc_Absyn_B8){ goto _LL1936;} else{ goto _LL1895;}} else{
goto _LL1895;} _LL1936: _temp1935= _temp1879.f2; goto _LL1894; _LL1895: _LL1950:
_temp1949= _temp1879.f1; goto _LL1944; _LL1944: _temp1943= _temp1879.f2; if((
unsigned int) _temp1943 > 4u?(( struct _tunion_struct*) _temp1943)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1948: _temp1947=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1943)->f1;
if(( int) _temp1947 == Cyc_Absyn_Signed){ goto _LL1946;} else{ goto _LL1897;}
_LL1946: _temp1945=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1943)->f2;
if(( int) _temp1945 == Cyc_Absyn_B8){ goto _LL1896;} else{ goto _LL1897;}} else{
goto _LL1897;} _LL1897: _LL1954: _temp1953= _temp1879.f1; if(( unsigned int)
_temp1953 > 4u?(( struct _tunion_struct*) _temp1953)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1958: _temp1957=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1953)->f1;
if(( int) _temp1957 == Cyc_Absyn_Unsigned){ goto _LL1956;} else{ goto _LL1899;}
_LL1956: _temp1955=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1953)->f2;
if(( int) _temp1955 == Cyc_Absyn_B4){ goto _LL1952;} else{ goto _LL1899;}} else{
goto _LL1899;} _LL1952: _temp1951= _temp1879.f2; goto _LL1898; _LL1899: _LL1966:
_temp1965= _temp1879.f1; goto _LL1960; _LL1960: _temp1959= _temp1879.f2; if((
unsigned int) _temp1959 > 4u?(( struct _tunion_struct*) _temp1959)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1964: _temp1963=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1959)->f1;
if(( int) _temp1963 == Cyc_Absyn_Unsigned){ goto _LL1962;} else{ goto _LL1901;}
_LL1962: _temp1961=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1959)->f2;
if(( int) _temp1961 == Cyc_Absyn_B4){ goto _LL1900;} else{ goto _LL1901;}} else{
goto _LL1901;} _LL1901: goto _LL1902; _LL1882: goto _LL1884; _LL1884: return(
void*) Cyc_Absyn_DoubleType; _LL1886: goto _LL1888; _LL1888: return( void*) Cyc_Absyn_FloatType;
_LL1890: goto _LL1892; _LL1892: return( void*)({ struct Cyc_Absyn_IntType_struct*
_temp1967=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp1967[ 0]=({ struct Cyc_Absyn_IntType_struct _temp1968; _temp1968.tag= Cyc_Absyn_IntType_tag;
_temp1968.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1968.f2=( void*)(( void*)
Cyc_Absyn_B8); _temp1968;}); _temp1967;}); _LL1894: goto _LL1896; _LL1896:
return( void*)({ struct Cyc_Absyn_IntType_struct* _temp1969=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp1969[ 0]=({ struct
Cyc_Absyn_IntType_struct _temp1970; _temp1970.tag= Cyc_Absyn_IntType_tag;
_temp1970.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp1970.f2=( void*)(( void*)
Cyc_Absyn_B8); _temp1970;}); _temp1969;}); _LL1898: goto _LL1900; _LL1900:
return( void*)({ struct Cyc_Absyn_IntType_struct* _temp1971=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp1971[ 0]=({ struct
Cyc_Absyn_IntType_struct _temp1972; _temp1972.tag= Cyc_Absyn_IntType_tag;
_temp1972.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1972.f2=( void*)(( void*)
Cyc_Absyn_B4); _temp1972;}); _temp1971;}); _LL1902: return( void*)({ struct Cyc_Absyn_IntType_struct*
_temp1973=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp1973[ 0]=({ struct Cyc_Absyn_IntType_struct _temp1974; _temp1974.tag= Cyc_Absyn_IntType_tag;
_temp1974.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp1974.f2=( void*)(( void*)
Cyc_Absyn_B4); _temp1974;}); _temp1973;}); _LL1880:;} void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp* e){ void* _temp1975=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1981; struct Cyc_Core_Opt* _temp1983; struct Cyc_Absyn_Exp* _temp1985;
_LL1977: if((( struct _tunion_struct*) _temp1975)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL1986: _temp1985=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1975)->f1; goto _LL1984; _LL1984: _temp1983=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1975)->f2; if( _temp1983 == 0){ goto
_LL1982;} else{ goto _LL1979;} _LL1982: _temp1981=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1975)->f3; goto _LL1978;} else{ goto
_LL1979;} _LL1979: goto _LL1980; _LL1978: Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp1987=( char*)"assignment in test"; struct
_tagged_string _temp1988; _temp1988.curr= _temp1987; _temp1988.base= _temp1987;
_temp1988.last_plus_one= _temp1987 + 19; _temp1988;})); goto _LL1976; _LL1980:
goto _LL1976; _LL1976:;} static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref*
c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){ return 1;}
if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)( c1->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1989=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1989[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1990; _temp1990.tag= Cyc_Absyn_Forward_constr_tag;
_temp1990.f1= c2; _temp1990;}); _temp1989;}))); return 1;} else{ if(( void*) c2->v
==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1991=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1991[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1992; _temp1992.tag= Cyc_Absyn_Forward_constr_tag; _temp1992.f1= c1;
_temp1992;}); _temp1991;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1993=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1993[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1994; _temp1994.tag= Cyc_Absyn_Forward_constr_tag;
_temp1994.f1= c1; _temp1994;}); _temp1993;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1995=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1995[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1996; _temp1996.tag= Cyc_Absyn_Forward_constr_tag; _temp1996.f1= c2;
_temp1996;}); _temp1995;}))); return 1;} else{ return 0;}}}}} static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc, struct Cyc_List_List*
tvs, struct Cyc_Absyn_Tvar* tv){ if( tvs == 0){ return({ struct Cyc_List_List*
_temp1997=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1997->hd=( void*) tv; _temp1997->tl= 0; _temp1997;});} else{{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){ if( Cyc_String_zstrptrcmp(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->name, tv->name) == 0){ struct Cyc_Absyn_Conref*
k1=(( struct Cyc_Absyn_Tvar*) tvs2->hd)->kind; struct Cyc_Absyn_Conref* k2= tv->kind;
if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1998=* tv->name; struct _tagged_string _temp1999= Cyc_Absynpp_ckind2string(
k1); struct _tagged_string _temp2000= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp1998.last_plus_one - _temp1998.curr, _temp1998.curr, _temp1999.last_plus_one
- _temp1999.curr, _temp1999.curr, _temp2000.last_plus_one - _temp2000.curr,
_temp2000.curr);}));} return tvs;} else{ if( tvs2->tl == 0){ tvs2->tl=({ struct
Cyc_List_List* _temp2001=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2001->hd=( void*) tv; _temp2001->tl= 0; _temp2001;}); return tvs;}}}}
return(( struct Cyc_List_List*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp2002=( char*)"Tcutil::add_free_tvar:expecting to have returned.";
struct _tagged_string _temp2003; _temp2003.curr= _temp2002; _temp2003.base=
_temp2002; _temp2003.last_plus_one= _temp2002 + 50; _temp2003;}));}} static
struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs,
struct Cyc_List_List* btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=
tvs->tl){ int present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b= b->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*) tvs->hd)->name,(( struct
Cyc_Absyn_Tvar*) b->hd)->name) == 0){ present= 1; break;}}} if( ! present){ r=({
struct Cyc_List_List* _temp2004=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2004->hd=( void*)(( struct Cyc_Absyn_Tvar*) tvs->hd);
_temp2004->tl= r; _temp2004;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} static int Cyc_Tcutil_tvar_counter= 0;
struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k){ struct _tagged_string s=
xprintf("#%d",( Cyc_Tcutil_tvar_counter ++)); return({ struct Cyc_Absyn_Tvar*
_temp2005=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp2005->name=({ struct _tagged_string* _temp2006=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp2006[ 0]= s; _temp2006;});
_temp2005->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
k); _temp2005;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_string s=* t->name; return( int)({ struct _tagged_string
_temp2007= s; char* _temp2009= _temp2007.curr + 0; if( _temp2009 < _temp2007.base?
1: _temp2009 >= _temp2007.last_plus_one){ _throw( Null_Exception);}* _temp2009;})
==( int)'#';} void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){ if(
! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_string s= Cyc_String_strconcat((
struct _tagged_string)({ char* _temp2010=( char*)"`"; struct _tagged_string
_temp2011; _temp2011.curr= _temp2010; _temp2011.base= _temp2010; _temp2011.last_plus_one=
_temp2010 + 2; _temp2011;}),* t->name);({ struct _tagged_string _temp2012= s;
char* _temp2014= _temp2012.curr + 1; if( _temp2014 < _temp2012.base? 1:
_temp2014 >= _temp2012.last_plus_one){ _throw( Null_Exception);}* _temp2014='t';});
t->name=({ struct _tagged_string* _temp2015=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2015[ 0]= s; _temp2015;});}} static
struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple11* x){ return({ struct
_tuple2* _temp2016=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2016->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2017=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2017->v=( void*)(*
x).f1; _temp2017;}); _temp2016->f2=(* x).f2; _temp2016->f3=(* x).f3; _temp2016;});}
int Cyc_Tcutil_fntype_att( void* a){ void* _temp2018= a; int _temp2032; _LL2020:
if(( unsigned int) _temp2018 > 15u?(( struct _tunion_struct*) _temp2018)->tag ==
Cyc_Absyn_Regparm_att_tag: 0){ _LL2033: _temp2032=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp2018)->f1; goto _LL2021;} else{ goto _LL2022;} _LL2022: if(( int) _temp2018
== Cyc_Absyn_Stdcall_att){ goto _LL2023;} else{ goto _LL2024;} _LL2024: if(( int)
_temp2018 == Cyc_Absyn_Cdecl_att){ goto _LL2025;} else{ goto _LL2026;} _LL2026:
if(( int) _temp2018 == Cyc_Absyn_Noreturn_att){ goto _LL2027;} else{ goto
_LL2028;} _LL2028: if(( int) _temp2018 == Cyc_Absyn_Const_att){ goto _LL2029;}
else{ goto _LL2030;} _LL2030: goto _LL2031; _LL2021: goto _LL2023; _LL2023: goto
_LL2025; _LL2025: goto _LL2027; _LL2027: goto _LL2029; _LL2029: return 1;
_LL2031: return 0; _LL2019:;} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*
fd){ if( fd->cached_typ == 0){ struct Cyc_List_List* fn_type_atts= 0;{ struct
Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts= atts->tl){ if( Cyc_Tcutil_fntype_att((
void*) atts->hd)){ fn_type_atts=({ struct Cyc_List_List* _temp2034=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2034->hd=( void*)(( void*) atts->hd);
_temp2034->tl= fn_type_atts; _temp2034;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2035=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2035[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2036; _temp2036.tag= Cyc_Absyn_FnType_tag;
_temp2036.f1=({ struct Cyc_Absyn_FnInfo _temp2037; _temp2037.tvars= fd->tvs;
_temp2037.effect= fd->effect; _temp2037.ret_typ=( void*)(( void*) fd->ret_type);
_temp2037.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple11*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp2037.varargs= fd->varargs; _temp2037.attributes= fn_type_atts; _temp2037;});
_temp2036;}); _temp2035;});} return( void*)( fd->cached_typ)->v;} static void*
Cyc_Tcutil_fst3( struct _tuple12* t){ return(* t).f1;} void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl*
fd){ void* t= Cyc_Tcutil_fndecl2typ( fd); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ void* _temp2038= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp2044; struct Cyc_List_List* _temp2046; int _temp2048; struct Cyc_List_List*
_temp2050; void* _temp2052; struct Cyc_Core_Opt* _temp2054; struct Cyc_List_List*
_temp2056; _LL2040: if(( unsigned int) _temp2038 > 4u?(( struct _tunion_struct*)
_temp2038)->tag == Cyc_Absyn_FnType_tag: 0){ _LL2045: _temp2044=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp2038)->f1; _LL2057: _temp2056=( struct Cyc_List_List*)
_temp2044.tvars; goto _LL2055; _LL2055: _temp2054=( struct Cyc_Core_Opt*)
_temp2044.effect; goto _LL2053; _LL2053: _temp2052=( void*) _temp2044.ret_typ;
goto _LL2051; _LL2051: _temp2050=( struct Cyc_List_List*) _temp2044.args; goto
_LL2049; _LL2049: _temp2048=( int) _temp2044.varargs; goto _LL2047; _LL2047:
_temp2046=( struct Cyc_List_List*) _temp2044.attributes; goto _LL2041;} else{
goto _LL2042;} _LL2042: goto _LL2043; _LL2041: fd->tvs= _temp2056; fd->effect=
_temp2054; goto _LL2039; _LL2043:(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp2058=( char*)"check_fndecl_valid_type: not a FnType";
struct _tagged_string _temp2059; _temp2059.curr= _temp2058; _temp2059.base=
_temp2058; _temp2059.last_plus_one= _temp2058 + 38; _temp2059;})); return;
_LL2039:;} Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct
_tagged_string*(* f)( struct _tuple11*), struct Cyc_List_List* x)) Cyc_List_map)((
struct _tagged_string*(*)( struct _tuple11* t)) Cyc_Tcutil_fst3, fd->args), loc,(
struct _tagged_string)({ char* _temp2060=( char*)"function declaration has repeated parameter";
struct _tagged_string _temp2061; _temp2061.curr= _temp2060; _temp2061.base=
_temp2060; _temp2061.last_plus_one= _temp2060 + 44; _temp2061;})); fd->cached_typ=({
struct Cyc_Core_Opt* _temp2062=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2062->v=( void*) t; _temp2062;});} static struct _tuple14*
Cyc_Tcutil_substitute_f1( struct _tuple2* y){ return({ struct _tuple14*
_temp2063=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2063->f1=({
struct _tuple13* _temp2064=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp2064->f1=(* y).f1; _temp2064->f2=(* y).f2; _temp2064;});
_temp2063->f2=(* y).f3; _temp2063;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple14* w){ void* _temp2067; struct _tuple13* _temp2069; struct
_tuple14 _temp2065=* w; _LL2070: _temp2069= _temp2065.f1; goto _LL2068; _LL2068:
_temp2067= _temp2065.f2; goto _LL2066; _LL2066: { struct Cyc_Absyn_Tqual*
_temp2073; struct Cyc_Core_Opt* _temp2075; struct _tuple13 _temp2071=* _temp2069;
_LL2076: _temp2075= _temp2071.f1; goto _LL2074; _LL2074: _temp2073= _temp2071.f2;
goto _LL2072; _LL2072: return({ struct _tuple2* _temp2077=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp2077->f1= _temp2075; _temp2077->f2=
_temp2073; _temp2077->f3= _temp2067; _temp2077;});}} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct Cyc_List_List* inst, struct Cyc_List_List* ts); void*
Cyc_Tcutil_substitute( struct Cyc_List_List* inst, void* t){ void* _temp2078=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp2124; struct Cyc_Absyn_Structdecl**
_temp2126; struct Cyc_List_List* _temp2128; struct _tuple1* _temp2130; struct
Cyc_Absyn_Uniondecl** _temp2132; struct Cyc_List_List* _temp2134; struct _tuple1*
_temp2136; struct Cyc_Absyn_TunionInfo _temp2138; struct Cyc_Absyn_Tuniondecl*
_temp2140; void* _temp2142; struct Cyc_List_List* _temp2144; struct _tuple1*
_temp2146; struct Cyc_Absyn_XTunionInfo _temp2148; struct Cyc_Absyn_XTuniondecl*
_temp2150; void* _temp2152; struct _tuple1* _temp2154; struct Cyc_Absyn_TunionFieldInfo
_temp2156; struct Cyc_Absyn_Tunionfield* _temp2158; struct Cyc_Absyn_Tuniondecl*
_temp2160; struct _tuple1* _temp2162; struct Cyc_List_List* _temp2164; struct
_tuple1* _temp2166; struct Cyc_Absyn_XTunionFieldInfo _temp2168; struct Cyc_Core_Opt*
_temp2170; struct Cyc_List_List* _temp2172; struct _tuple1* _temp2174; struct
Cyc_Absyn_Exp* _temp2176; struct Cyc_Absyn_Tqual* _temp2178; void* _temp2180;
struct Cyc_Absyn_PtrInfo _temp2182; struct Cyc_Absyn_Conref* _temp2184; struct
Cyc_Absyn_Tqual* _temp2186; struct Cyc_Absyn_Conref* _temp2188; void* _temp2190;
void* _temp2192; struct Cyc_Absyn_FnInfo _temp2194; struct Cyc_List_List*
_temp2196; int _temp2198; struct Cyc_List_List* _temp2200; void* _temp2202;
struct Cyc_Core_Opt* _temp2204; struct Cyc_List_List* _temp2206; struct Cyc_List_List*
_temp2208; int _temp2210; struct Cyc_Core_Opt* _temp2212; void* _temp2214; void*
_temp2216; struct Cyc_Absyn_Enumdecl* _temp2218; struct _tuple1* _temp2220; void*
_temp2222; void* _temp2224; void* _temp2226; struct Cyc_List_List* _temp2228;
_LL2080: if(( unsigned int) _temp2078 > 4u?(( struct _tunion_struct*) _temp2078)->tag
== Cyc_Absyn_VarType_tag: 0){ _LL2125: _temp2124=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp2078)->f1; goto _LL2081;} else{ goto
_LL2082;} _LL2082: if(( unsigned int) _temp2078 > 4u?(( struct _tunion_struct*)
_temp2078)->tag == Cyc_Absyn_StructType_tag: 0){ _LL2131: _temp2130=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2078)->f1; goto _LL2129;
_LL2129: _temp2128=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2078)->f2; goto _LL2127; _LL2127: _temp2126=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2078)->f3; goto _LL2083;} else{ goto
_LL2084;} _LL2084: if(( unsigned int) _temp2078 > 4u?(( struct _tunion_struct*)
_temp2078)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL2137: _temp2136=( struct
_tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp2078)->f1; goto _LL2135;
_LL2135: _temp2134=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2078)->f2; goto _LL2133; _LL2133: _temp2132=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2078)->f3; goto _LL2085;} else{ goto
_LL2086;} _LL2086: if(( unsigned int) _temp2078 > 4u?(( struct _tunion_struct*)
_temp2078)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL2139: _temp2138=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp2078)->f1; _LL2147: _temp2146=( struct
_tuple1*) _temp2138.name; goto _LL2145; _LL2145: _temp2144=( struct Cyc_List_List*)
_temp2138.targs; goto _LL2143; _LL2143: _temp2142=( void*) _temp2138.rgn; goto
_LL2141; _LL2141: _temp2140=( struct Cyc_Absyn_Tuniondecl*) _temp2138.tud; goto
_LL2087;} else{ goto _LL2088;} _LL2088: if(( unsigned int) _temp2078 > 4u?((
struct _tunion_struct*) _temp2078)->tag == Cyc_Absyn_XTunionType_tag: 0){
_LL2149: _temp2148=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp2078)->f1; _LL2155: _temp2154=( struct _tuple1*) _temp2148.name; goto
_LL2153; _LL2153: _temp2152=( void*) _temp2148.rgn; goto _LL2151; _LL2151:
_temp2150=( struct Cyc_Absyn_XTuniondecl*) _temp2148.xtud; goto _LL2089;} else{
goto _LL2090;} _LL2090: if(( unsigned int) _temp2078 > 4u?(( struct
_tunion_struct*) _temp2078)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL2157:
_temp2156=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2078)->f1; _LL2167: _temp2166=( struct _tuple1*) _temp2156.name; goto
_LL2165; _LL2165: _temp2164=( struct Cyc_List_List*) _temp2156.targs; goto
_LL2163; _LL2163: _temp2162=( struct _tuple1*) _temp2156.fname; goto _LL2161;
_LL2161: _temp2160=( struct Cyc_Absyn_Tuniondecl*) _temp2156.tud; goto _LL2159;
_LL2159: _temp2158=( struct Cyc_Absyn_Tunionfield*) _temp2156.tufd; goto _LL2091;}
else{ goto _LL2092;} _LL2092: if(( unsigned int) _temp2078 > 4u?(( struct
_tunion_struct*) _temp2078)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL2169:
_temp2168=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp2078)->f1; goto _LL2093;} else{ goto _LL2094;} _LL2094: if(( unsigned int)
_temp2078 > 4u?(( struct _tunion_struct*) _temp2078)->tag == Cyc_Absyn_TypedefType_tag:
0){ _LL2175: _temp2174=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2078)->f1; goto _LL2173; _LL2173: _temp2172=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2078)->f2; goto _LL2171; _LL2171:
_temp2170=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2078)->f3; goto _LL2095;} else{ goto _LL2096;} _LL2096: if(( unsigned int)
_temp2078 > 4u?(( struct _tunion_struct*) _temp2078)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2181: _temp2180=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2078)->f1;
goto _LL2179; _LL2179: _temp2178=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2078)->f2; goto _LL2177; _LL2177: _temp2176=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2078)->f3; goto _LL2097;} else{ goto
_LL2098;} _LL2098: if(( unsigned int) _temp2078 > 4u?(( struct _tunion_struct*)
_temp2078)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2183: _temp2182=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2078)->f1;
_LL2193: _temp2192=( void*) _temp2182.elt_typ; goto _LL2191; _LL2191: _temp2190=(
void*) _temp2182.rgn_typ; goto _LL2189; _LL2189: _temp2188=( struct Cyc_Absyn_Conref*)
_temp2182.nullable; goto _LL2187; _LL2187: _temp2186=( struct Cyc_Absyn_Tqual*)
_temp2182.tq; goto _LL2185; _LL2185: _temp2184=( struct Cyc_Absyn_Conref*)
_temp2182.bounds; goto _LL2099;} else{ goto _LL2100;} _LL2100: if(( unsigned int)
_temp2078 > 4u?(( struct _tunion_struct*) _temp2078)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2195: _temp2194=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2078)->f1; _LL2207: _temp2206=( struct Cyc_List_List*) _temp2194.tvars;
goto _LL2205; _LL2205: _temp2204=( struct Cyc_Core_Opt*) _temp2194.effect; goto
_LL2203; _LL2203: _temp2202=( void*) _temp2194.ret_typ; goto _LL2201; _LL2201:
_temp2200=( struct Cyc_List_List*) _temp2194.args; goto _LL2199; _LL2199:
_temp2198=( int) _temp2194.varargs; goto _LL2197; _LL2197: _temp2196=( struct
Cyc_List_List*) _temp2194.attributes; goto _LL2101;} else{ goto _LL2102;}
_LL2102: if(( unsigned int) _temp2078 > 4u?(( struct _tunion_struct*) _temp2078)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL2209: _temp2208=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp2078)->f1; goto _LL2103;} else{ goto
_LL2104;} _LL2104: if(( unsigned int) _temp2078 > 4u?(( struct _tunion_struct*)
_temp2078)->tag == Cyc_Absyn_Evar_tag: 0){ _LL2215: _temp2214=( void*)(( struct
Cyc_Absyn_Evar_struct*) _temp2078)->f1; goto _LL2213; _LL2213: _temp2212=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp2078)->f2; goto
_LL2211; _LL2211: _temp2210=( int)(( struct Cyc_Absyn_Evar_struct*) _temp2078)->f3;
goto _LL2105;} else{ goto _LL2106;} _LL2106: if(( unsigned int) _temp2078 > 4u?((
struct _tunion_struct*) _temp2078)->tag == Cyc_Absyn_RgnHandleType_tag: 0){
_LL2217: _temp2216=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2078)->f1;
goto _LL2107;} else{ goto _LL2108;} _LL2108: if(( unsigned int) _temp2078 > 4u?((
struct _tunion_struct*) _temp2078)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL2221:
_temp2220=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp2078)->f1;
goto _LL2219; _LL2219: _temp2218=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp2078)->f2; goto _LL2109;} else{ goto _LL2110;} _LL2110: if(( int) _temp2078
== Cyc_Absyn_VoidType){ goto _LL2111;} else{ goto _LL2112;} _LL2112: if((
unsigned int) _temp2078 > 4u?(( struct _tunion_struct*) _temp2078)->tag == Cyc_Absyn_IntType_tag:
0){ _LL2225: _temp2224=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2078)->f1;
goto _LL2223; _LL2223: _temp2222=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2078)->f2; goto _LL2113;} else{ goto _LL2114;} _LL2114: if(( int) _temp2078
== Cyc_Absyn_FloatType){ goto _LL2115;} else{ goto _LL2116;} _LL2116: if(( int)
_temp2078 == Cyc_Absyn_DoubleType){ goto _LL2117;} else{ goto _LL2118;} _LL2118:
if(( int) _temp2078 == Cyc_Absyn_HeapRgn){ goto _LL2119;} else{ goto _LL2120;}
_LL2120: if(( unsigned int) _temp2078 > 4u?(( struct _tunion_struct*) _temp2078)->tag
== Cyc_Absyn_AccessEff_tag: 0){ _LL2227: _temp2226=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2078)->f1; goto _LL2121;} else{ goto _LL2122;} _LL2122: if(( unsigned int)
_temp2078 > 4u?(( struct _tunion_struct*) _temp2078)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL2229: _temp2228=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp2078)->f1; goto _LL2123;} else{ goto _LL2079;} _LL2081: { struct
_handler_cons _temp2230; _push_handler(& _temp2230);{ struct _xtunion_struct*
_temp2231=( struct _xtunion_struct*) setjmp( _temp2230.handler); if( ! _temp2231){{
void* _temp2232=(( void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2124); _npop_handler( 0u); return _temp2232;}; _pop_handler();} else{
struct _xtunion_struct* _temp2234= _temp2231; _LL2236: if( _temp2234->tag == Cyc_Core_Not_found_tag){
goto _LL2237;} else{ goto _LL2238;} _LL2238: goto _LL2239; _LL2237: return t;
_LL2239:( void) _throw( _temp2234); _LL2235:;}}} _LL2083: { struct Cyc_List_List*
new_ts= Cyc_Tcutil_substs( inst, _temp2128); return new_ts == _temp2128? t:(
void*)({ struct Cyc_Absyn_StructType_struct* _temp2240=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2240[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2241; _temp2241.tag= Cyc_Absyn_StructType_tag;
_temp2241.f1= _temp2130; _temp2241.f2= new_ts; _temp2241.f3= _temp2126;
_temp2241;}); _temp2240;});} _LL2085: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs(
inst, _temp2134); return new_ts == _temp2134? t:( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp2242=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2242[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2243; _temp2243.tag= Cyc_Absyn_UnionType_tag;
_temp2243.f1= _temp2136; _temp2243.f2= new_ts; _temp2243.f3= _temp2132;
_temp2243;}); _temp2242;});} _LL2087: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs(
inst, _temp2144); void* new_r= Cyc_Tcutil_substitute( inst, _temp2142); return(
new_ts == _temp2144? new_r == _temp2142: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2244=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2244[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2245; _temp2245.tag=
Cyc_Absyn_TunionType_tag; _temp2245.f1=({ struct Cyc_Absyn_TunionInfo _temp2246;
_temp2246.name= _temp2146; _temp2246.targs= new_ts; _temp2246.rgn=( void*) new_r;
_temp2246.tud= _temp2140; _temp2246;}); _temp2245;}); _temp2244;});} _LL2089: {
void* new_r= Cyc_Tcutil_substitute( inst, _temp2152); return new_r == _temp2152?
t:( void*)({ struct Cyc_Absyn_XTunionType_struct* _temp2247=( struct Cyc_Absyn_XTunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct)); _temp2247[ 0]=({
struct Cyc_Absyn_XTunionType_struct _temp2248; _temp2248.tag= Cyc_Absyn_XTunionType_tag;
_temp2248.f1=({ struct Cyc_Absyn_XTunionInfo _temp2249; _temp2249.name=
_temp2154; _temp2249.rgn=( void*) new_r; _temp2249.xtud= _temp2150; _temp2249;});
_temp2248;}); _temp2247;});} _LL2091: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs(
inst, _temp2164); return new_ts == _temp2164? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2250=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2250[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2251; _temp2251.tag= Cyc_Absyn_TunionFieldType_tag; _temp2251.f1=({ struct
Cyc_Absyn_TunionFieldInfo _temp2252; _temp2252.name= _temp2166; _temp2252.targs=
new_ts; _temp2252.fname= _temp2162; _temp2252.tud= _temp2160; _temp2252.tufd=
_temp2158; _temp2252;}); _temp2251;}); _temp2250;});} _LL2093: return t; _LL2095: {
struct Cyc_List_List* new_ts= Cyc_Tcutil_substs( inst, _temp2172); return new_ts
== _temp2172? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2253=(
struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2253[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2254; _temp2254.tag=
Cyc_Absyn_TypedefType_tag; _temp2254.f1= _temp2174; _temp2254.f2= new_ts;
_temp2254.f3= _temp2170; _temp2254;}); _temp2253;});} _LL2097: { void* new_t1=
Cyc_Tcutil_substitute( inst, _temp2180); return new_t1 == _temp2180? t:( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp2255=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2255[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2256; _temp2256.tag= Cyc_Absyn_ArrayType_tag;
_temp2256.f1=( void*) new_t1; _temp2256.f2= _temp2178; _temp2256.f3= _temp2176;
_temp2256;}); _temp2255;});} _LL2099: { void* new_t1= Cyc_Tcutil_substitute(
inst, _temp2192); void* new_r= Cyc_Tcutil_substitute( inst, _temp2190); if(
new_t1 == _temp2192? new_r == _temp2190: 0){ return t;} return( void*)({ struct
Cyc_Absyn_PointerType_struct* _temp2257=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2257[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2258; _temp2258.tag= Cyc_Absyn_PointerType_tag;
_temp2258.f1=({ struct Cyc_Absyn_PtrInfo _temp2259; _temp2259.elt_typ=( void*)
new_t1; _temp2259.rgn_typ=( void*) new_r; _temp2259.nullable= _temp2188;
_temp2259.tq= _temp2186; _temp2259.bounds= _temp2184; _temp2259;}); _temp2258;});
_temp2257;});} _LL2101:{ struct Cyc_List_List* p= _temp2206; for( 0; p != 0; p=
p->tl){ inst=({ struct Cyc_List_List* _temp2260=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2260->hd=( void*)({ struct
_tuple5* _temp2261=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2261->f1=( struct Cyc_Absyn_Tvar*) p->hd; _temp2261->f2=( void*)({ struct
Cyc_Absyn_VarType_struct* _temp2262=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2262[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2263; _temp2263.tag= Cyc_Absyn_VarType_tag;
_temp2263.f1=( struct Cyc_Absyn_Tvar*) p->hd; _temp2263;}); _temp2262;});
_temp2261;}); _temp2260->tl= inst; _temp2260;});}}{ struct Cyc_List_List*
_temp2266; struct Cyc_List_List* _temp2268; struct _tuple0 _temp2264=(( struct
_tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)((( struct Cyc_List_List*(*)(
struct _tuple14*(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f1, _temp2200)); _LL2269: _temp2268= _temp2264.f1; goto
_LL2267; _LL2267: _temp2266= _temp2264.f2; goto _LL2265; _LL2265: { struct Cyc_List_List*
ts2= Cyc_Tcutil_substs( inst, _temp2266); struct Cyc_List_List* args2=(( struct
Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple14*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp2268, ts2));
struct Cyc_Core_Opt* eff2; if( _temp2204 == 0){ eff2= 0;} else{ void* new_eff=
Cyc_Tcutil_substitute( inst,( void*) _temp2204->v); if( new_eff ==( void*)
_temp2204->v){ eff2= _temp2204;} else{ eff2=({ struct Cyc_Core_Opt* _temp2270=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1); _temp2270[ 0]=({
struct Cyc_Core_Opt _temp2271; _temp2271.v=( void*) new_eff; _temp2271;});
_temp2270;});}} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp2272=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2272[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2273; _temp2273.tag= Cyc_Absyn_FnType_tag;
_temp2273.f1=({ struct Cyc_Absyn_FnInfo _temp2274; _temp2274.tvars= _temp2206;
_temp2274.effect= eff2; _temp2274.ret_typ=( void*) Cyc_Tcutil_substitute( inst,
_temp2202); _temp2274.args= args2; _temp2274.varargs= _temp2198; _temp2274.attributes=
_temp2196; _temp2274;}); _temp2273;}); _temp2272;});}} _LL2103: { struct Cyc_List_List*
_temp2277; struct Cyc_List_List* _temp2279; struct _tuple0 _temp2275=(( struct
_tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( _temp2208); _LL2280:
_temp2279= _temp2275.f1; goto _LL2278; _LL2278: _temp2277= _temp2275.f2; goto
_LL2276; _LL2276: { struct Cyc_List_List* ts2= Cyc_Tcutil_substs( inst,
_temp2277); if( ts2 == _temp2277){ return t;}{ struct Cyc_List_List* tqts2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp2279, ts2); return( void*)({ struct Cyc_Absyn_TupleType_struct* _temp2281=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2281[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2282; _temp2282.tag= Cyc_Absyn_TupleType_tag;
_temp2282.f1= tqts2; _temp2282;}); _temp2281;});}}} _LL2105: if( _temp2212 != 0){
return Cyc_Tcutil_substitute( inst,( void*) _temp2212->v);} else{ return t;}
_LL2107: { void* new_rt= Cyc_Tcutil_substitute( inst, _temp2216); return new_rt
== _temp2216? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2283=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2283[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2284; _temp2284.tag=
Cyc_Absyn_RgnHandleType_tag; _temp2284.f1=( void*) new_rt; _temp2284;});
_temp2283;});} _LL2109: return t; _LL2111: return t; _LL2113: return t; _LL2115:
return t; _LL2117: return t; _LL2119: return t; _LL2121: { void* new_r= Cyc_Tcutil_substitute(
inst, _temp2226); return new_r == _temp2226? t:( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp2285=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2285[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2286; _temp2286.tag= Cyc_Absyn_AccessEff_tag;
_temp2286.f1=( void*) new_r; _temp2286;}); _temp2285;});} _LL2123: { struct Cyc_List_List*
new_es= Cyc_Tcutil_substs( inst, _temp2228); return new_es == _temp2228? t:(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2287=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2287[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2288; _temp2288.tag= Cyc_Absyn_JoinEff_tag;
_temp2288.f1= new_es; _temp2288;}); _temp2287;});} _LL2079:;} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct Cyc_List_List* inst, struct Cyc_List_List* ts){ if( ts
== 0){ return 0;}{ void* old_hd=( void*) ts->hd; struct Cyc_List_List* old_tl=
ts->tl; void* new_hd= Cyc_Tcutil_substitute( inst, old_hd); struct Cyc_List_List*
new_tl= Cyc_Tcutil_substs( inst, old_tl); if( old_hd == new_hd? old_tl == new_tl:
0){ return ts;} return({ struct Cyc_List_List* _temp2289=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2289[ 0]=({ struct Cyc_List_List
_temp2290; _temp2290.hd=( void*) new_hd; _temp2290.tl= new_tl; _temp2290;});
_temp2289;});}} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2291=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2291[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2292; _temp2292.tag= Cyc_Absyn_Const_e_tag;
_temp2292.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp2292;}); _temp2291;}),
loc);{ void* _temp2293= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp2307; void* _temp2309; void* _temp2311; void* _temp2313; void* _temp2315;
_LL2295: if(( unsigned int) _temp2293 > 4u?(( struct _tunion_struct*) _temp2293)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL2308: _temp2307=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp2293)->f1; goto _LL2296;} else{ goto
_LL2297;} _LL2297: if(( unsigned int) _temp2293 > 4u?(( struct _tunion_struct*)
_temp2293)->tag == Cyc_Absyn_IntType_tag: 0){ _LL2312: _temp2311=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp2293)->f1; goto _LL2310; _LL2310:
_temp2309=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2293)->f2; if(( int)
_temp2309 == Cyc_Absyn_B1){ goto _LL2298;} else{ goto _LL2299;}} else{ goto
_LL2299;} _LL2299: if(( unsigned int) _temp2293 > 4u?(( struct _tunion_struct*)
_temp2293)->tag == Cyc_Absyn_IntType_tag: 0){ _LL2316: _temp2315=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp2293)->f1; goto _LL2314; _LL2314:
_temp2313=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2293)->f2; goto
_LL2300;} else{ goto _LL2301;} _LL2301: if(( int) _temp2293 == Cyc_Absyn_FloatType){
goto _LL2302;} else{ goto _LL2303;} _LL2303: if(( int) _temp2293 == Cyc_Absyn_DoubleType){
goto _LL2304;} else{ goto _LL2305;} _LL2305: goto _LL2306; _LL2296: goto _LL2294;
_LL2298:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2317=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2317[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2318; _temp2318.tag= Cyc_Absyn_Const_e_tag;
_temp2318.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2319=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2319[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2320; _temp2320.tag= Cyc_Absyn_Char_c_tag;
_temp2320.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2320.f2='\000'; _temp2320;});
_temp2319;})); _temp2318;}); _temp2317;}))); goto _LL2294; _LL2300:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2321=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2321[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2322; _temp2322.tag= Cyc_Absyn_Const_e_tag;
_temp2322.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2323=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2323[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2324; _temp2324.tag= Cyc_Absyn_Int_c_tag;
_temp2324.f1=( void*) _temp2315; _temp2324.f2= 0; _temp2324;}); _temp2323;}));
_temp2322;}); _temp2321;}))); if( _temp2313 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2325=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2325[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2326; _temp2326.tag= Cyc_Absyn_Cast_e_tag; _temp2326.f1=( void*) t;
_temp2326.f2= e; _temp2326;}); _temp2325;}), loc);} goto _LL2294; _LL2302:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2327=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2327[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2328; _temp2328.tag= Cyc_Absyn_Const_e_tag;
_temp2328.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2329=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2329[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2330; _temp2330.tag= Cyc_Absyn_Float_c_tag;
_temp2330.f1=( struct _tagged_string)({ char* _temp2331=( char*)"0.0"; struct
_tagged_string _temp2332; _temp2332.curr= _temp2331; _temp2332.base= _temp2331;
_temp2332.last_plus_one= _temp2331 + 4; _temp2332;}); _temp2330;}); _temp2329;}));
_temp2328;}); _temp2327;}))); goto _LL2294; _LL2304:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2333=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2333[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2334; _temp2334.tag= Cyc_Absyn_Cast_e_tag; _temp2334.f1=( void*) t;
_temp2334.f2= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2335=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2335[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2336; _temp2336.tag= Cyc_Absyn_Const_e_tag;
_temp2336.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2337=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2337[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2338; _temp2338.tag= Cyc_Absyn_Float_c_tag;
_temp2338.f1=( struct _tagged_string)({ char* _temp2339=( char*)"0.0"; struct
_tagged_string _temp2340; _temp2340.curr= _temp2339; _temp2340.base= _temp2339;
_temp2340.last_plus_one= _temp2339 + 4; _temp2340;}); _temp2338;}); _temp2337;}));
_temp2336;}); _temp2335;}), loc); _temp2334;}); _temp2333;}))); goto _LL2294;
_LL2306: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2341= Cyc_Absynpp_typ2string(
t); xprintf("declaration of type %.*s requires initializer", _temp2341.last_plus_one
- _temp2341.curr, _temp2341.curr);})); goto _LL2294; _LL2294:;} return e;}
struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar* tv){ void* k=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind); return({
struct _tuple5* _temp2342=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2342->f1= tv; _temp2342->f2= Cyc_Absyn_new_evar( k); _temp2342;});} struct
Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* free_vars, void* expected_kind,
void* t){{ void* _temp2343= Cyc_Tcutil_compress( t); int _temp2389; struct Cyc_Core_Opt*
_temp2391; void* _temp2393; struct Cyc_Absyn_Tvar* _temp2395; struct Cyc_Absyn_Enumdecl*
_temp2397; struct Cyc_Absyn_Enumdecl** _temp2399; struct _tuple1* _temp2400;
struct Cyc_Absyn_TunionInfo _temp2402; struct Cyc_Absyn_Tuniondecl* _temp2404;
struct Cyc_Absyn_Tuniondecl** _temp2406; void* _temp2407; struct Cyc_List_List*
_temp2409; struct _tuple1* _temp2411; struct Cyc_Absyn_XTunionInfo _temp2413;
struct Cyc_Absyn_XTuniondecl* _temp2415; struct Cyc_Absyn_XTuniondecl**
_temp2417; void* _temp2418; struct _tuple1* _temp2420; struct Cyc_Absyn_TunionFieldInfo
_temp2422; struct Cyc_Absyn_Tunionfield* _temp2424; struct Cyc_Absyn_Tunionfield**
_temp2426; struct Cyc_Absyn_Tuniondecl* _temp2427; struct Cyc_Absyn_Tuniondecl**
_temp2429; struct _tuple1* _temp2430; struct Cyc_List_List* _temp2432; struct
_tuple1* _temp2434; struct Cyc_Absyn_XTunionFieldInfo _temp2436; struct Cyc_Absyn_Tunionfield*
_temp2438; struct Cyc_Absyn_Tunionfield** _temp2440; struct Cyc_Absyn_XTuniondecl*
_temp2441; struct Cyc_Absyn_XTuniondecl** _temp2443; struct _tuple1* _temp2444;
struct _tuple1* _temp2446; struct Cyc_Absyn_PtrInfo _temp2448; struct Cyc_Absyn_Conref*
_temp2450; struct Cyc_Absyn_Tqual* _temp2452; struct Cyc_Absyn_Conref* _temp2454;
void* _temp2456; void* _temp2458; void* _temp2460; void* _temp2462; struct Cyc_Absyn_Exp*
_temp2464; struct Cyc_Absyn_Tqual* _temp2466; void* _temp2468; struct Cyc_Absyn_FnInfo
_temp2470; struct Cyc_List_List* _temp2472; int _temp2474; struct Cyc_List_List*
_temp2476; void* _temp2478; struct Cyc_Core_Opt* _temp2480; struct Cyc_Core_Opt**
_temp2482; struct Cyc_List_List* _temp2483; struct Cyc_List_List** _temp2485;
struct Cyc_List_List* _temp2486; struct Cyc_Absyn_Structdecl** _temp2488; struct
Cyc_Absyn_Structdecl*** _temp2490; struct Cyc_List_List* _temp2491; struct
_tuple1* _temp2493; struct Cyc_Absyn_Uniondecl** _temp2495; struct Cyc_Absyn_Uniondecl***
_temp2497; struct Cyc_List_List* _temp2498; struct _tuple1* _temp2500; struct
Cyc_Core_Opt* _temp2502; struct Cyc_Core_Opt** _temp2504; struct Cyc_List_List*
_temp2505; struct _tuple1* _temp2507; void* _temp2509; void* _temp2511; struct
Cyc_List_List* _temp2513; _LL2345: if(( int) _temp2343 == Cyc_Absyn_VoidType){
goto _LL2346;} else{ goto _LL2347;} _LL2347: if(( unsigned int) _temp2343 > 4u?((
struct _tunion_struct*) _temp2343)->tag == Cyc_Absyn_Evar_tag: 0){ _LL2394:
_temp2393=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp2343)->f1; goto _LL2392;
_LL2392: _temp2391=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp2343)->f2; goto _LL2390; _LL2390: _temp2389=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp2343)->f3; goto _LL2348;} else{ goto _LL2349;} _LL2349: if(( unsigned int)
_temp2343 > 4u?(( struct _tunion_struct*) _temp2343)->tag == Cyc_Absyn_VarType_tag:
0){ _LL2396: _temp2395=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp2343)->f1; goto _LL2350;} else{ goto _LL2351;} _LL2351: if(( unsigned int)
_temp2343 > 4u?(( struct _tunion_struct*) _temp2343)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL2401: _temp2400=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp2343)->f1; goto _LL2398; _LL2398: _temp2397=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp2343)->f2; _temp2399=&(( struct Cyc_Absyn_EnumType_struct*)
_temp2343)->f2; goto _LL2352;} else{ goto _LL2353;} _LL2353: if(( unsigned int)
_temp2343 > 4u?(( struct _tunion_struct*) _temp2343)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL2403: _temp2402=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp2343)->f1; _LL2412: _temp2411=( struct _tuple1*) _temp2402.name; goto
_LL2410; _LL2410: _temp2409=( struct Cyc_List_List*) _temp2402.targs; goto
_LL2408; _LL2408: _temp2407=( void*) _temp2402.rgn; goto _LL2405; _LL2405:
_temp2404=( struct Cyc_Absyn_Tuniondecl*) _temp2402.tud; _temp2406=&((( struct
Cyc_Absyn_TunionType_struct*) _temp2343)->f1).tud; goto _LL2354;} else{ goto
_LL2355;} _LL2355: if(( unsigned int) _temp2343 > 4u?(( struct _tunion_struct*)
_temp2343)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL2414: _temp2413=( struct
Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*) _temp2343)->f1;
_LL2421: _temp2420=( struct _tuple1*) _temp2413.name; goto _LL2419; _LL2419:
_temp2418=( void*) _temp2413.rgn; goto _LL2416; _LL2416: _temp2415=( struct Cyc_Absyn_XTuniondecl*)
_temp2413.xtud; _temp2417=&((( struct Cyc_Absyn_XTunionType_struct*) _temp2343)->f1).xtud;
goto _LL2356;} else{ goto _LL2357;} _LL2357: if(( unsigned int) _temp2343 > 4u?((
struct _tunion_struct*) _temp2343)->tag == Cyc_Absyn_TunionFieldType_tag: 0){
_LL2423: _temp2422=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2343)->f1; _LL2435: _temp2434=( struct _tuple1*) _temp2422.name; goto
_LL2433; _LL2433: _temp2432=( struct Cyc_List_List*) _temp2422.targs; goto
_LL2431; _LL2431: _temp2430=( struct _tuple1*) _temp2422.fname; goto _LL2428;
_LL2428: _temp2427=( struct Cyc_Absyn_Tuniondecl*) _temp2422.tud; _temp2429=&(((
struct Cyc_Absyn_TunionFieldType_struct*) _temp2343)->f1).tud; goto _LL2425;
_LL2425: _temp2424=( struct Cyc_Absyn_Tunionfield*) _temp2422.tufd; _temp2426=&(((
struct Cyc_Absyn_TunionFieldType_struct*) _temp2343)->f1).tufd; goto _LL2358;}
else{ goto _LL2359;} _LL2359: if(( unsigned int) _temp2343 > 4u?(( struct
_tunion_struct*) _temp2343)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL2437:
_temp2436=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp2343)->f1; _LL2447: _temp2446=( struct _tuple1*) _temp2436.name; goto
_LL2445; _LL2445: _temp2444=( struct _tuple1*) _temp2436.fname; goto _LL2442;
_LL2442: _temp2441=( struct Cyc_Absyn_XTuniondecl*) _temp2436.xtud; _temp2443=&(((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp2343)->f1).xtud; goto _LL2439;
_LL2439: _temp2438=( struct Cyc_Absyn_Tunionfield*) _temp2436.xtufd; _temp2440=&(((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp2343)->f1).xtufd; goto _LL2360;}
else{ goto _LL2361;} _LL2361: if(( unsigned int) _temp2343 > 4u?(( struct
_tunion_struct*) _temp2343)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2449:
_temp2448=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2343)->f1; _LL2459: _temp2458=( void*) _temp2448.elt_typ; goto _LL2457;
_LL2457: _temp2456=( void*) _temp2448.rgn_typ; goto _LL2455; _LL2455: _temp2454=(
struct Cyc_Absyn_Conref*) _temp2448.nullable; goto _LL2453; _LL2453: _temp2452=(
struct Cyc_Absyn_Tqual*) _temp2448.tq; goto _LL2451; _LL2451: _temp2450=( struct
Cyc_Absyn_Conref*) _temp2448.bounds; goto _LL2362;} else{ goto _LL2363;} _LL2363:
if(( unsigned int) _temp2343 > 4u?(( struct _tunion_struct*) _temp2343)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL2463: _temp2462=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2343)->f1; goto _LL2461; _LL2461: _temp2460=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2343)->f2; goto _LL2364;} else{ goto _LL2365;} _LL2365: if(( int) _temp2343
== Cyc_Absyn_FloatType){ goto _LL2366;} else{ goto _LL2367;} _LL2367: if(( int)
_temp2343 == Cyc_Absyn_DoubleType){ goto _LL2368;} else{ goto _LL2369;} _LL2369:
if(( unsigned int) _temp2343 > 4u?(( struct _tunion_struct*) _temp2343)->tag ==
Cyc_Absyn_ArrayType_tag: 0){ _LL2469: _temp2468=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2343)->f1; goto _LL2467; _LL2467: _temp2466=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2343)->f2; goto _LL2465; _LL2465:
_temp2464=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2343)->f3; goto _LL2370;} else{ goto _LL2371;} _LL2371: if(( unsigned int)
_temp2343 > 4u?(( struct _tunion_struct*) _temp2343)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2471: _temp2470=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2343)->f1; _LL2484: _temp2483=( struct Cyc_List_List*) _temp2470.tvars;
_temp2485=&((( struct Cyc_Absyn_FnType_struct*) _temp2343)->f1).tvars; goto
_LL2481; _LL2481: _temp2480=( struct Cyc_Core_Opt*) _temp2470.effect; _temp2482=&(((
struct Cyc_Absyn_FnType_struct*) _temp2343)->f1).effect; goto _LL2479; _LL2479:
_temp2478=( void*) _temp2470.ret_typ; goto _LL2477; _LL2477: _temp2476=( struct
Cyc_List_List*) _temp2470.args; goto _LL2475; _LL2475: _temp2474=( int)
_temp2470.varargs; goto _LL2473; _LL2473: _temp2472=( struct Cyc_List_List*)
_temp2470.attributes; goto _LL2372;} else{ goto _LL2373;} _LL2373: if((
unsigned int) _temp2343 > 4u?(( struct _tunion_struct*) _temp2343)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL2487: _temp2486=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp2343)->f1; goto _LL2374;} else{ goto _LL2375;} _LL2375: if(( unsigned int)
_temp2343 > 4u?(( struct _tunion_struct*) _temp2343)->tag == Cyc_Absyn_StructType_tag:
0){ _LL2494: _temp2493=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp2343)->f1; goto _LL2492; _LL2492: _temp2491=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp2343)->f2; goto _LL2489; _LL2489:
_temp2488=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp2343)->f3; _temp2490=&(( struct Cyc_Absyn_StructType_struct*) _temp2343)->f3;
goto _LL2376;} else{ goto _LL2377;} _LL2377: if(( unsigned int) _temp2343 > 4u?((
struct _tunion_struct*) _temp2343)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL2501:
_temp2500=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp2343)->f1;
goto _LL2499; _LL2499: _temp2498=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2343)->f2; goto _LL2496; _LL2496: _temp2495=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2343)->f3; _temp2497=&(( struct Cyc_Absyn_UnionType_struct*)
_temp2343)->f3; goto _LL2378;} else{ goto _LL2379;} _LL2379: if(( unsigned int)
_temp2343 > 4u?(( struct _tunion_struct*) _temp2343)->tag == Cyc_Absyn_TypedefType_tag:
0){ _LL2508: _temp2507=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2343)->f1; goto _LL2506; _LL2506: _temp2505=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2343)->f2; goto _LL2503; _LL2503:
_temp2502=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2343)->f3; _temp2504=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2343)->f3;
goto _LL2380;} else{ goto _LL2381;} _LL2381: if(( int) _temp2343 == Cyc_Absyn_HeapRgn){
goto _LL2382;} else{ goto _LL2383;} _LL2383: if(( unsigned int) _temp2343 > 4u?((
struct _tunion_struct*) _temp2343)->tag == Cyc_Absyn_RgnHandleType_tag: 0){
_LL2510: _temp2509=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2343)->f1;
goto _LL2384;} else{ goto _LL2385;} _LL2385: if(( unsigned int) _temp2343 > 4u?((
struct _tunion_struct*) _temp2343)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL2512:
_temp2511=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2343)->f1; goto
_LL2386;} else{ goto _LL2387;} _LL2387: if(( unsigned int) _temp2343 > 4u?((
struct _tunion_struct*) _temp2343)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL2514:
_temp2513=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp2343)->f1;
goto _LL2388;} else{ goto _LL2344;} _LL2346: goto _LL2344; _LL2348: goto _LL2344;
_LL2350: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2395->kind); if(( void*) c->v ==( void*)
Cyc_Absyn_No_constr){( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2515=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2515[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2516; _temp2516.tag= Cyc_Absyn_Eq_constr_tag;
_temp2516.f1=( void*) expected_kind; _temp2516;}); _temp2515;})));} free_vars=
Cyc_Tcutil_add_free_tvar( loc, free_vars, _temp2395); goto _LL2344;} _LL2352:
if(* _temp2399 == 0){ struct _handler_cons _temp2517; _push_handler(& _temp2517);{
struct _xtunion_struct* _temp2518=( struct _xtunion_struct*) setjmp( _temp2517.handler);
if( ! _temp2518){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2400);* _temp2399=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ struct _xtunion_struct* _temp2520= _temp2518; _LL2522: if( _temp2520->tag
== Cyc_Dict_Absent_tag){ goto _LL2523;} else{ goto _LL2524;} _LL2524: goto
_LL2525; _LL2523: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2526= Cyc_Absynpp_qvar2string(
_temp2400); xprintf("unbound type enum %.*s", _temp2526.last_plus_one -
_temp2526.curr, _temp2526.curr);})); return free_vars; _LL2525:( void) _throw(
_temp2520); _LL2521:;}}}{ struct Cyc_Absyn_Enumdecl* ed=( struct Cyc_Absyn_Enumdecl*)({
struct Cyc_Absyn_Enumdecl* _temp2527=* _temp2399; if( _temp2527 == 0){ _throw(
Null_Exception);} _temp2527;});* _temp2400=({ struct _tuple1* _temp2528= ed->name;
unsigned int _temp2529= 0; if( _temp2529 >= 1u){ _throw( Null_Exception);}
_temp2528[ _temp2529];}); goto _LL2344;} _LL2354: if(* _temp2406 == 0){ struct
_handler_cons _temp2530; _push_handler(& _temp2530);{ struct _xtunion_struct*
_temp2531=( struct _xtunion_struct*) setjmp( _temp2530.handler); if( ! _temp2531){{
struct Cyc_Absyn_Tuniondecl** tud= Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2411); if(( struct Cyc_Absyn_Tuniondecl**) tud == 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2532= Cyc_Absynpp_qvar2string( _temp2411); xprintf("unbound type tunion %.*s",
_temp2532.last_plus_one - _temp2532.curr, _temp2532.curr);}));{ struct Cyc_List_List*
_temp2533= free_vars; _npop_handler( 0u); return _temp2533;}}* _temp2406=(
struct Cyc_Absyn_Tuniondecl*)* tud;}; _pop_handler();} else{ struct
_xtunion_struct* _temp2535= _temp2531; _LL2537: if( _temp2535->tag == Cyc_Dict_Absent_tag){
goto _LL2538;} else{ goto _LL2539;} _LL2539: goto _LL2540; _LL2538: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2541= Cyc_Absynpp_qvar2string( _temp2411);
xprintf("unbound type tunion %.*s", _temp2541.last_plus_one - _temp2541.curr,
_temp2541.curr);})); return free_vars; _LL2540:( void) _throw( _temp2535);
_LL2536:;}}}{ struct Cyc_Absyn_Tuniondecl* tud=( struct Cyc_Absyn_Tuniondecl*)({
struct Cyc_Absyn_Tuniondecl* _temp2542=* _temp2406; if( _temp2542 == 0){ _throw(
Null_Exception);} _temp2542;}); if( tud->name != 0){* _temp2411=({ struct
_tuple1* _temp2543=( struct _tuple1*)( tud->name)->v; unsigned int _temp2544= 0;
if( _temp2544 >= 1u){ _throw( Null_Exception);} _temp2543[ _temp2544];});}
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2407);{ struct Cyc_List_List* tvs= tud->tvs; for( 0; _temp2409 != 0? tvs !=
0: 0; _temp2409= _temp2409->tl, tvs= tvs->tl){ void* t1=( void*) _temp2409->hd;
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2409 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2545= Cyc_Absynpp_qvar2string( _temp2411); xprintf("too many type arguments for tunion %.*s",
_temp2545.last_plus_one - _temp2545.curr, _temp2545.curr);}));} if( tvs != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2546= Cyc_Absynpp_qvar2string(
_temp2411); xprintf("too few type arguments for tunion %.*s", _temp2546.last_plus_one
- _temp2546.curr, _temp2546.curr);}));} goto _LL2344;}} _LL2356: if(* _temp2417
== 0){ struct _handler_cons _temp2547; _push_handler(& _temp2547);{ struct
_xtunion_struct* _temp2548=( struct _xtunion_struct*) setjmp( _temp2547.handler);
if( ! _temp2548){{ struct Cyc_Core_Opt* xtud= Cyc_Tcenv_lookup_xtuniondecl( te,
loc, _temp2420); if( xtud == 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2549= Cyc_Absynpp_qvar2string( _temp2420); xprintf("unbound type xtunion %.*s",
_temp2549.last_plus_one - _temp2549.curr, _temp2549.curr);}));{ struct Cyc_List_List*
_temp2550= free_vars; _npop_handler( 0u); return _temp2550;}}* _temp2417=(
struct Cyc_Absyn_XTuniondecl*)*(( struct Cyc_Absyn_XTuniondecl**) xtud->v);};
_pop_handler();} else{ struct _xtunion_struct* _temp2552= _temp2548; _LL2554:
if( _temp2552->tag == Cyc_Dict_Absent_tag){ goto _LL2555;} else{ goto _LL2556;}
_LL2556: goto _LL2557; _LL2555: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2558= Cyc_Absynpp_qvar2string( _temp2420); xprintf("unbound type xtunion %.*s",
_temp2558.last_plus_one - _temp2558.curr, _temp2558.curr);})); return free_vars;
_LL2557:( void) _throw( _temp2552); _LL2553:;}}}{ struct Cyc_Absyn_XTuniondecl*
xtud=( struct Cyc_Absyn_XTuniondecl*)({ struct Cyc_Absyn_XTuniondecl* _temp2559=*
_temp2417; if( _temp2559 == 0){ _throw( Null_Exception);} _temp2559;}); if((
struct _tuple1*) xtud->name != 0){* _temp2420=* xtud->name;} free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_RgnKind, _temp2418); goto _LL2344;}
_LL2358: if(* _temp2429 == 0){ struct _handler_cons _temp2560; _push_handler(&
_temp2560);{ struct _xtunion_struct* _temp2561=( struct _xtunion_struct*) setjmp(
_temp2560.handler); if( ! _temp2561){{ struct Cyc_Absyn_Tuniondecl** tud= Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2434);* _temp2429=( struct Cyc_Absyn_Tuniondecl*)* tud;};
_pop_handler();} else{ struct _xtunion_struct* _temp2563= _temp2561; _LL2565:
if( _temp2563->tag == Cyc_Dict_Absent_tag){ goto _LL2566;} else{ goto _LL2567;}
_LL2567: goto _LL2568; _LL2566: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2569= Cyc_Absynpp_qvar2string( _temp2434); xprintf("unbound type tunion %.*s",
_temp2569.last_plus_one - _temp2569.curr, _temp2569.curr);})); return free_vars;
_LL2568:( void) _throw( _temp2563); _LL2564:;}}} if(* _temp2426 == 0){ struct
_handler_cons _temp2570; _push_handler(& _temp2570);{ struct _xtunion_struct*
_temp2571=( struct _xtunion_struct*) setjmp( _temp2570.handler); if( ! _temp2571){{
void* _temp2572= Cyc_Tcenv_lookup_ordinary( te, loc, _temp2430); struct Cyc_Absyn_Tunionfield*
_temp2578; struct Cyc_Absyn_Tuniondecl* _temp2580; _LL2574: if((( struct
_tunion_struct*) _temp2572)->tag == Cyc_Tcenv_TunionRes_tag){ _LL2581: _temp2580=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp2572)->f1;
goto _LL2579; _LL2579: _temp2578=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp2572)->f2; goto _LL2575;} else{ goto _LL2576;} _LL2576: goto _LL2577;
_LL2575:* _temp2426=( struct Cyc_Absyn_Tunionfield*) _temp2578; if(( struct Cyc_Absyn_Tuniondecl*)
_temp2580 !=* _temp2429){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2582= Cyc_Absynpp_qvar2string( _temp2430); struct _tagged_string _temp2583=
Cyc_Absynpp_qvar2string( _temp2434); xprintf("unbound field %.*s in type tunion %.*s",
_temp2582.last_plus_one - _temp2582.curr, _temp2582.curr, _temp2583.last_plus_one
- _temp2583.curr, _temp2583.curr);}));{ struct Cyc_List_List* _temp2584=
free_vars; _npop_handler( 0u); return _temp2584;}} goto _LL2573; _LL2577: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2585= Cyc_Absynpp_qvar2string( _temp2430);
struct _tagged_string _temp2586= Cyc_Absynpp_qvar2string( _temp2434); xprintf("unbound field %.*s in type tunion %.*s",
_temp2585.last_plus_one - _temp2585.curr, _temp2585.curr, _temp2586.last_plus_one
- _temp2586.curr, _temp2586.curr);}));{ struct Cyc_List_List* _temp2587=
free_vars; _npop_handler( 0u); return _temp2587;} _LL2573:;}; _pop_handler();}
else{ struct _xtunion_struct* _temp2589= _temp2571; _LL2591: if( _temp2589->tag
== Cyc_Dict_Absent_tag){ goto _LL2592;} else{ goto _LL2593;} _LL2593: goto
_LL2594; _LL2592: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2595= Cyc_Absynpp_qvar2string(
_temp2430); struct _tagged_string _temp2596= Cyc_Absynpp_qvar2string( _temp2434);
xprintf("unbound field %.*s in type tunion %.*s", _temp2595.last_plus_one -
_temp2595.curr, _temp2595.curr, _temp2596.last_plus_one - _temp2596.curr,
_temp2596.curr);})); return free_vars; _LL2594:( void) _throw( _temp2589);
_LL2590:;}}}{ struct Cyc_Absyn_Tuniondecl* tud=( struct Cyc_Absyn_Tuniondecl*)({
struct Cyc_Absyn_Tuniondecl* _temp2597=* _temp2429; if( _temp2597 == 0){ _throw(
Null_Exception);} _temp2597;}); struct Cyc_Absyn_Tunionfield* tufd=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_Absyn_Tunionfield* _temp2598=* _temp2426; if( _temp2598 == 0){ _throw(
Null_Exception);} _temp2598;}); if( tud->name != 0){* _temp2434=*(( struct
_tuple1*)( tud->name)->v);} if(( struct _tuple1*) tufd->name != 0){* _temp2430=*
tufd->name;}{ struct Cyc_List_List* tvs= tud->tvs; for( 0; _temp2432 != 0? tvs
!= 0: 0; _temp2432= _temp2432->tl, tvs= tvs->tl){ void* t1=( void*) _temp2432->hd;
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2432 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2599= Cyc_Absynpp_qvar2string( _temp2434); struct
_tagged_string _temp2600= Cyc_Absynpp_qvar2string( _temp2430); xprintf("too many type arguments for tunion %.*s.%.*s",
_temp2599.last_plus_one - _temp2599.curr, _temp2599.curr, _temp2600.last_plus_one
- _temp2600.curr, _temp2600.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2601= Cyc_Absynpp_qvar2string( _temp2434); struct
_tagged_string _temp2602= Cyc_Absynpp_qvar2string( _temp2430); xprintf("too few type arguments for tunion %.*s.%.*s",
_temp2601.last_plus_one - _temp2601.curr, _temp2601.curr, _temp2602.last_plus_one
- _temp2602.curr, _temp2602.curr);}));} goto _LL2344;}} _LL2360: if(* _temp2443
== 0){ struct _handler_cons _temp2603; _push_handler(& _temp2603);{ struct
_xtunion_struct* _temp2604=( struct _xtunion_struct*) setjmp( _temp2603.handler);
if( ! _temp2604){{ struct Cyc_Core_Opt* xtud= Cyc_Tcenv_lookup_xtuniondecl( te,
loc, _temp2446); if( xtud == 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2605= Cyc_Absynpp_qvar2string( _temp2446); xprintf("unbound type xtunion %.*s",
_temp2605.last_plus_one - _temp2605.curr, _temp2605.curr);}));{ struct Cyc_List_List*
_temp2606= free_vars; _npop_handler( 0u); return _temp2606;}}* _temp2443=(
struct Cyc_Absyn_XTuniondecl*)*(( struct Cyc_Absyn_XTuniondecl**) xtud->v);};
_pop_handler();} else{ struct _xtunion_struct* _temp2608= _temp2604; _LL2610:
if( _temp2608->tag == Cyc_Dict_Absent_tag){ goto _LL2611;} else{ goto _LL2612;}
_LL2612: goto _LL2613; _LL2611: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2614= Cyc_Absynpp_qvar2string( _temp2446); xprintf("unbound type xtunion %.*s",
_temp2614.last_plus_one - _temp2614.curr, _temp2614.curr);})); return free_vars;
_LL2613:( void) _throw( _temp2608); _LL2609:;}}} if(* _temp2440 == 0){ struct
_handler_cons _temp2615; _push_handler(& _temp2615);{ struct _xtunion_struct*
_temp2616=( struct _xtunion_struct*) setjmp( _temp2615.handler); if( ! _temp2616){{
void* _temp2617= Cyc_Tcenv_lookup_ordinary( te, loc, _temp2444); struct Cyc_Absyn_Tunionfield*
_temp2623; struct Cyc_Absyn_XTuniondecl* _temp2625; _LL2619: if((( struct
_tunion_struct*) _temp2617)->tag == Cyc_Tcenv_XTunionRes_tag){ _LL2626:
_temp2625=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp2617)->f1; goto _LL2624; _LL2624: _temp2623=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp2617)->f2; goto _LL2620;} else{ goto
_LL2621;} _LL2621: goto _LL2622; _LL2620:* _temp2440=( struct Cyc_Absyn_Tunionfield*)
_temp2623; if(( struct Cyc_Absyn_XTuniondecl*) _temp2625 !=* _temp2443){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2627= Cyc_Absynpp_qvar2string( _temp2444);
struct _tagged_string _temp2628= Cyc_Absynpp_qvar2string( _temp2446); xprintf("unbound field %.*s in type xtunion %.*s",
_temp2627.last_plus_one - _temp2627.curr, _temp2627.curr, _temp2628.last_plus_one
- _temp2628.curr, _temp2628.curr);}));{ struct Cyc_List_List* _temp2629=
free_vars; _npop_handler( 0u); return _temp2629;}} goto _LL2618; _LL2622: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2630= Cyc_Absynpp_qvar2string( _temp2444);
struct _tagged_string _temp2631= Cyc_Absynpp_qvar2string( _temp2446); xprintf("unbound field %.*s in type xtunion %.*s",
_temp2630.last_plus_one - _temp2630.curr, _temp2630.curr, _temp2631.last_plus_one
- _temp2631.curr, _temp2631.curr);}));{ struct Cyc_List_List* _temp2632=
free_vars; _npop_handler( 0u); return _temp2632;} _LL2618:;}; _pop_handler();}
else{ struct _xtunion_struct* _temp2634= _temp2616; _LL2636: if( _temp2634->tag
== Cyc_Dict_Absent_tag){ goto _LL2637;} else{ goto _LL2638;} _LL2638: goto
_LL2639; _LL2637: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2640= Cyc_Absynpp_qvar2string(
_temp2444); struct _tagged_string _temp2641= Cyc_Absynpp_qvar2string( _temp2446);
xprintf("unbound field %.*s in type xtunion %.*s", _temp2640.last_plus_one -
_temp2640.curr, _temp2640.curr, _temp2641.last_plus_one - _temp2641.curr,
_temp2641.curr);})); return free_vars; _LL2639:( void) _throw( _temp2634);
_LL2635:;}}}{ struct Cyc_Absyn_XTuniondecl* xtud=( struct Cyc_Absyn_XTuniondecl*)({
struct Cyc_Absyn_XTuniondecl* _temp2642=* _temp2443; if( _temp2642 == 0){ _throw(
Null_Exception);} _temp2642;}); struct Cyc_Absyn_Tunionfield* xtufd=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_Absyn_Tunionfield* _temp2643=* _temp2440; if( _temp2643 == 0){ _throw(
Null_Exception);} _temp2643;}); if(( struct _tuple1*) xtud->name != 0){*
_temp2446=* xtud->name;} if(( struct _tuple1*) xtufd->name != 0){* _temp2444=*
xtufd->name;} goto _LL2344;} _LL2362: free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_AnyKind, _temp2458); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_RgnKind, _temp2456);{ void* _temp2644=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2450))->v; void* _temp2650; struct Cyc_Absyn_Exp* _temp2652; _LL2646: if((
unsigned int) _temp2644 > 1u?(( struct _tunion_struct*) _temp2644)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL2651: _temp2650=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2644)->f1;
if(( unsigned int) _temp2650 > 1u?(( struct _tunion_struct*) _temp2650)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL2653: _temp2652=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp2650)->f1; goto _LL2647;} else{ goto _LL2648;}}
else{ goto _LL2648;} _LL2648: goto _LL2649; _LL2647: if( ! Cyc_Tcutil_is_const_exp(
te, _temp2652)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp2654=(
char*)"pointer bounds expression is not a constant"; struct _tagged_string
_temp2655; _temp2655.curr= _temp2654; _temp2655.base= _temp2654; _temp2655.last_plus_one=
_temp2654 + 44; _temp2655;}));} Cyc_Tcexp_tcExp( te, 0, _temp2652); if( ! Cyc_Tcutil_coerce_uint_typ(
te, _temp2652)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp2656=(
char*)"pointer bounds expression is not an unsigned int"; struct _tagged_string
_temp2657; _temp2657.curr= _temp2656; _temp2657.base= _temp2656; _temp2657.last_plus_one=
_temp2656 + 49; _temp2657;}));} Cyc_Evexp_eval_const_uint_exp( _temp2652); goto
_LL2645; _LL2649: goto _LL2645; _LL2645:;} goto _LL2344; _LL2364: goto _LL2344;
_LL2366: goto _LL2344; _LL2368: goto _LL2344; _LL2370: free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_MemKind, _temp2468); if( _temp2464 == 0? 1:
! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2658= _temp2464; if( _temp2658 == 0){ _throw( Null_Exception);} _temp2658;}))){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp2659=( char*)"array bounds expression is not constant";
struct _tagged_string _temp2660; _temp2660.curr= _temp2659; _temp2660.base=
_temp2659; _temp2660.last_plus_one= _temp2659 + 40; _temp2660;}));} Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2661= _temp2464; if(
_temp2661 == 0){ _throw( Null_Exception);} _temp2661;})); if( ! Cyc_Tcutil_coerce_uint_typ(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2662= _temp2464; if(
_temp2662 == 0){ _throw( Null_Exception);} _temp2662;}))){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2663=( char*)"array bounds expression is not an unsigned int";
struct _tagged_string _temp2664; _temp2664.curr= _temp2663; _temp2664.base=
_temp2663; _temp2664.last_plus_one= _temp2663 + 47; _temp2664;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2665= _temp2464; if(
_temp2665 == 0){ _throw( Null_Exception);} _temp2665;})); goto _LL2344; _LL2372:
if( _temp2472 != 0){ int seen_cdecl= 0; int seen_stdcall= 0; for( 0; _temp2472
!= 0; _temp2472= _temp2472->tl){ if( ! Cyc_Tcutil_fntype_att(( void*) _temp2472->hd)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2666= Cyc_Absyn_attribute2string((
void*) _temp2472->hd); xprintf("bad function type attribute %.*s", _temp2666.last_plus_one
- _temp2666.curr, _temp2666.curr);}));} if(( void*) _temp2472->hd ==( void*) Cyc_Absyn_Stdcall_att){
if( seen_cdecl){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_stdcall= 1;} else{ if(( void*) _temp2472->hd ==( void*) Cyc_Absyn_Cdecl_att){
if( seen_stdcall){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_cdecl= 1;}}}} Cyc_Tcutil_check_unique_tvars( loc,* _temp2485);{ struct Cyc_List_List*
b=* _temp2485; for( 0; b != 0; b= b->tl){ void* _temp2667=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)
b->hd)->kind))->v; void* _temp2673; _LL2669: if(( unsigned int) _temp2667 > 1u?((
struct _tunion_struct*) _temp2667)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2674:
_temp2673=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2667)->f1; if((
int) _temp2673 == Cyc_Absyn_MemKind){ goto _LL2670;} else{ goto _LL2671;}} else{
goto _LL2671;} _LL2671: goto _LL2672; _LL2670: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2675=*(( struct Cyc_Absyn_Tvar*) b->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_temp2675.last_plus_one - _temp2675.curr, _temp2675.curr);})); goto _LL2668;
_LL2672: goto _LL2668; _LL2668:;}}{ struct Cyc_List_List* new_free_vars= 0;
new_free_vars= Cyc_Tcutil_check_valid_type( loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind,
_temp2478); for( 0; _temp2476 != 0; _temp2476= _temp2476->tl){ new_free_vars=
Cyc_Tcutil_check_valid_type( loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple2*) _temp2476->hd)).f3);} if(* _temp2482 != 0){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_EffKind,( void*)(* _temp2482)->v);}
else{ struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_EffKind);
new_free_vars=({ struct Cyc_List_List* _temp2676=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2676->hd=( void*) e; _temp2676->tl=
new_free_vars; _temp2676;});{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= new_free_vars; for( 0; tvs != 0; tvs= tvs->tl){ void* _temp2677=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind))->v; void* _temp2685; void* _temp2687;
_LL2679: if(( unsigned int) _temp2677 > 1u?(( struct _tunion_struct*) _temp2677)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL2686: _temp2685=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2677)->f1; if(( int) _temp2685 == Cyc_Absyn_RgnKind){ goto _LL2680;} else{
goto _LL2681;}} else{ goto _LL2681;} _LL2681: if(( unsigned int) _temp2677 > 1u?((
struct _tunion_struct*) _temp2677)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2688:
_temp2687=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2677)->f1; if((
int) _temp2687 == Cyc_Absyn_EffKind){ goto _LL2682;} else{ goto _LL2683;}} else{
goto _LL2683;} _LL2683: goto _LL2684; _LL2680: effect=({ struct Cyc_List_List*
_temp2689=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2689->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2690=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2690[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2691; _temp2691.tag= Cyc_Absyn_AccessEff_tag;
_temp2691.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2692=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2692[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2693; _temp2693.tag= Cyc_Absyn_VarType_tag;
_temp2693.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp2693;}); _temp2692;}));
_temp2691;}); _temp2690;})); _temp2689->tl= effect; _temp2689;}); goto _LL2678;
_LL2682: effect=({ struct Cyc_List_List* _temp2694=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2694->hd=( void*)(( void*)({
struct Cyc_Absyn_VarType_struct* _temp2695=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2695[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2696; _temp2696.tag= Cyc_Absyn_VarType_tag;
_temp2696.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp2696;}); _temp2695;}));
_temp2694->tl= effect; _temp2694;}); goto _LL2678; _LL2684: goto _LL2678;
_LL2678:;}}* _temp2482=({ struct Cyc_Core_Opt* _temp2697=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2697->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp2698=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2698[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2699; _temp2699.tag= Cyc_Absyn_JoinEff_tag;
_temp2699.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effect); _temp2699;}); _temp2698;})); _temp2697;});}} if(* _temp2485 != 0){
struct Cyc_List_List* new_btvs= 0;{ struct Cyc_List_List* bs=* _temp2485; for( 0;
bs != 0; bs= bs->tl){ new_free_vars= Cyc_Tcutil_add_free_tvar( loc,
new_free_vars,( struct Cyc_Absyn_Tvar*) bs->hd);{ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) bs->hd)->kind); void* _temp2700=( void*) c->v; void*
_temp2708; _LL2702: if(( int) _temp2700 == Cyc_Absyn_No_constr){ goto _LL2703;}
else{ goto _LL2704;} _LL2704: if(( unsigned int) _temp2700 > 1u?(( struct
_tunion_struct*) _temp2700)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2709:
_temp2708=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2700)->f1; if((
int) _temp2708 == Cyc_Absyn_MemKind){ goto _LL2705;} else{ goto _LL2706;}} else{
goto _LL2706;} _LL2706: goto _LL2707; _LL2703: Cyc_Tcutil_warn( loc,({ struct
_tagged_string _temp2710=*(( struct Cyc_Absyn_Tvar*) bs->hd)->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp2710.last_plus_one - _temp2710.curr, _temp2710.curr);})); goto _LL2705;
_LL2705:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2711=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2711[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2712; _temp2712.tag= Cyc_Absyn_Eq_constr_tag;
_temp2712.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2712;}); _temp2711;})));
goto _LL2701; _LL2707: goto _LL2701; _LL2701:;}}} new_free_vars= Cyc_Tcutil_remove_bound_tvars(
new_free_vars,* _temp2485);} for( 0; new_free_vars != 0; new_free_vars=
new_free_vars->tl){ free_vars= Cyc_Tcutil_add_free_tvar( loc, free_vars,( struct
Cyc_Absyn_Tvar*) new_free_vars->hd);} goto _LL2344;} _LL2374: for( 0; _temp2486
!= 0; _temp2486= _temp2486->tl){ free_vars= Cyc_Tcutil_check_valid_type( loc, te,
free_vars,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple4*) _temp2486->hd)).f2);}
goto _LL2344; _LL2376: if( _temp2493 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2713=( char*)"missing struct name"; struct
_tagged_string _temp2714; _temp2714.curr= _temp2713; _temp2714.base= _temp2713;
_temp2714.last_plus_one= _temp2713 + 20; _temp2714;})); return free_vars;} if(*
_temp2490 == 0){ struct _handler_cons _temp2715; _push_handler(& _temp2715);{
struct _xtunion_struct* _temp2716=( struct _xtunion_struct*) setjmp( _temp2715.handler);
if( ! _temp2716){* _temp2490=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*)({ struct _tuple1* _temp2717= _temp2493; if( _temp2717
== 0){ _throw( Null_Exception);} _temp2717;}));; _pop_handler();} else{ struct
_xtunion_struct* _temp2719= _temp2716; _LL2721: if( _temp2719->tag == Cyc_Dict_Absent_tag){
goto _LL2722;} else{ goto _LL2723;} _LL2723: goto _LL2724; _LL2722: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2726= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2725= _temp2493; if( _temp2725 == 0){ _throw(
Null_Exception);} _temp2725;})); xprintf("unbound type struct %.*s", _temp2726.last_plus_one
- _temp2726.curr, _temp2726.curr);})); return free_vars; _LL2724:( void) _throw(
_temp2719); _LL2720:;}}}{ struct Cyc_Absyn_Structdecl* sd=*(* _temp2490); if( sd->name
!= 0){* _temp2493=({ struct _tuple1* _temp2727=( struct _tuple1*)( sd->name)->v;
unsigned int _temp2728= 0; if( _temp2728 >= 1u){ _throw( Null_Exception);}
_temp2727[ _temp2728];});}{ struct Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp2491); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2730= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2729= _temp2493; if( _temp2729 == 0){ _throw(
Null_Exception);} _temp2729;})); int _temp2731= lvs; int _temp2732= largs;
xprintf("struct %.*s expects %d type arguments but was given %d", _temp2730.last_plus_one
- _temp2730.curr, _temp2730.curr, _temp2731, _temp2732);}));} for( 0; _temp2491
!= 0; _temp2491= _temp2491->tl, tvs= tvs->tl){ void* t1=( void*) _temp2491->hd;
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2344;}} _LL2378: if( _temp2500 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2733=( char*)"missing union name";
struct _tagged_string _temp2734; _temp2734.curr= _temp2733; _temp2734.base=
_temp2733; _temp2734.last_plus_one= _temp2733 + 19; _temp2734;})); return
free_vars;} if(* _temp2497 == 0){ struct _handler_cons _temp2735; _push_handler(&
_temp2735);{ struct _xtunion_struct* _temp2736=( struct _xtunion_struct*) setjmp(
_temp2735.handler); if( ! _temp2736){* _temp2497=( struct Cyc_Absyn_Uniondecl**)
Cyc_Tcenv_lookup_uniondecl( te, loc,( struct _tuple1*)({ struct _tuple1*
_temp2737= _temp2500; if( _temp2737 == 0){ _throw( Null_Exception);} _temp2737;}));;
_pop_handler();} else{ struct _xtunion_struct* _temp2739= _temp2736; _LL2741:
if( _temp2739->tag == Cyc_Dict_Absent_tag){ goto _LL2742;} else{ goto _LL2743;}
_LL2743: goto _LL2744; _LL2742: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2746= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp2745= _temp2500; if( _temp2745 == 0){ _throw( Null_Exception);} _temp2745;}));
xprintf("unbound type union %.*s", _temp2746.last_plus_one - _temp2746.curr,
_temp2746.curr);})); return free_vars; _LL2744:( void) _throw( _temp2739);
_LL2740:;}}}{ struct Cyc_Absyn_Uniondecl* ud=*(* _temp2497); if( ud->name != 0){*
_temp2500=({ struct _tuple1* _temp2747=( struct _tuple1*)( ud->name)->v;
unsigned int _temp2748= 0; if( _temp2748 >= 1u){ _throw( Null_Exception);}
_temp2747[ _temp2748];});}{ struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp2498); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2750= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2749= _temp2500; if( _temp2749 == 0){ _throw(
Null_Exception);} _temp2749;})); int _temp2751= lvs; int _temp2752= largs;
xprintf("union %.*s expects %d type arguments but was given %d", _temp2750.last_plus_one
- _temp2750.curr, _temp2750.curr, _temp2751, _temp2752);}));} for( 0; _temp2498
!= 0; _temp2498= _temp2498->tl, tvs= tvs->tl){ void* t1=( void*) _temp2498->hd;
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2344;}} _LL2380: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp2753; _push_handler(& _temp2753);{ struct
_xtunion_struct* _temp2754=( struct _xtunion_struct*) setjmp( _temp2753.handler);
if( ! _temp2754){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc, _temp2507);;
_pop_handler();} else{ struct _xtunion_struct* _temp2756= _temp2754; _LL2758:
if( _temp2756->tag == Cyc_Dict_Absent_tag){ goto _LL2759;} else{ goto _LL2760;}
_LL2760: goto _LL2761; _LL2759: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2762= Cyc_Absynpp_qvar2string( _temp2507); xprintf("unbound typedef name %.*s",
_temp2762.last_plus_one - _temp2762.curr, _temp2762.curr);})); return free_vars;
_LL2761:( void) _throw( _temp2756); _LL2757:;}}}({ struct _tuple1* _temp2763=
_temp2507; unsigned int _temp2764= 0; if( _temp2764 >= 1u){ _throw(
Null_Exception);} _temp2763[ _temp2764]=({ struct _tuple1* _temp2765= td->name;
unsigned int _temp2766= 0; if( _temp2766 >= 1u){ _throw( Null_Exception);}
_temp2765[ _temp2766];});});{ struct Cyc_List_List* tvs= td->tvs; struct Cyc_List_List*
ts= _temp2505; struct Cyc_List_List* inst= 0; for( 0; ts != 0? tvs != 0: 0; ts=
ts->tl, tvs= tvs->tl){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k,( void*) ts->hd); inst=({ struct Cyc_List_List* _temp2767=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2767->hd=(
void*)({ struct _tuple5* _temp2768=( struct _tuple5*) GC_malloc( sizeof( struct
_tuple5)); _temp2768->f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp2768->f2=( void*)
ts->hd; _temp2768;}); _temp2767->tl= inst; _temp2767;});} if( tvs != 0? 1: ts !=
0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2769= Cyc_Absynpp_qvar2string(
_temp2507); xprintf("typedef %.*s expects a different number of arguments",
_temp2769.last_plus_one - _temp2769.curr, _temp2769.curr);})); return free_vars;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2504=({
struct Cyc_Core_Opt* _temp2770=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2770->v=( void*) new_typ; _temp2770;}); goto _LL2344;}}}
_LL2382: goto _LL2344; _LL2384: _temp2511= _temp2509; goto _LL2386; _LL2386:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2511); goto _LL2344; _LL2388: for( 0; _temp2513 != 0; _temp2513= _temp2513->tl){
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_EffKind,(
void*) _temp2513->hd);} goto _LL2344; _LL2344:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind(
t), expected_kind)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2771=
Cyc_Absynpp_typ2string( t); struct _tagged_string _temp2772= Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind( t)); struct _tagged_string _temp2773= Cyc_Absynpp_kind2string(
expected_kind); xprintf("type %.*s has kind %.*s but as used here needs kind %.*s",
_temp2771.last_plus_one - _temp2771.curr, _temp2771.curr, _temp2772.last_plus_one
- _temp2772.curr, _temp2772.curr, _temp2773.last_plus_one - _temp2773.curr,
_temp2773.curr);}));} return free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t);{ struct Cyc_List_List* x= free_tvars; for( 0; x != 0; x= x->tl){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) x->hd)->kind); void* _temp2774=( void*) c->v; void*
_temp2782; _LL2776: if(( int) _temp2774 == Cyc_Absyn_No_constr){ goto _LL2777;}
else{ goto _LL2778;} _LL2778: if(( unsigned int) _temp2774 > 1u?(( struct
_tunion_struct*) _temp2774)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2783:
_temp2782=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2774)->f1; if((
int) _temp2782 == Cyc_Absyn_MemKind){ goto _LL2779;} else{ goto _LL2780;}} else{
goto _LL2780;} _LL2780: goto _LL2781; _LL2777: goto _LL2779; _LL2779:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2784=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2784[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp2785; _temp2785.tag= Cyc_Absyn_Eq_constr_tag;
_temp2785.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2785;}); _temp2784;})));
goto _LL2775; _LL2781: goto _LL2775; _LL2775:;}} if( free_tvars != 0){ void*
_temp2786= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2792; struct
Cyc_List_List* _temp2794; int _temp2796; struct Cyc_List_List* _temp2798; void*
_temp2800; struct Cyc_Core_Opt* _temp2802; struct Cyc_List_List* _temp2804;
struct Cyc_List_List** _temp2806; _LL2788: if(( unsigned int) _temp2786 > 4u?((
struct _tunion_struct*) _temp2786)->tag == Cyc_Absyn_FnType_tag: 0){ _LL2793:
_temp2792=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2786)->f1; _LL2805: _temp2804=( struct Cyc_List_List*) _temp2792.tvars;
_temp2806=&((( struct Cyc_Absyn_FnType_struct*) _temp2786)->f1).tvars; goto
_LL2803; _LL2803: _temp2802=( struct Cyc_Core_Opt*) _temp2792.effect; goto
_LL2801; _LL2801: _temp2800=( void*) _temp2792.ret_typ; goto _LL2799; _LL2799:
_temp2798=( struct Cyc_List_List*) _temp2792.args; goto _LL2797; _LL2797:
_temp2796=( int) _temp2792.varargs; goto _LL2795; _LL2795: _temp2794=( struct
Cyc_List_List*) _temp2792.attributes; goto _LL2789;} else{ goto _LL2790;}
_LL2790: goto _LL2791; _LL2789: if(* _temp2806 == 0){* _temp2806= free_tvars;}
else{ if( free_tvars != 0){ struct _tagged_string s=*(( struct Cyc_Absyn_Tvar*)
free_tvars->hd)->name; if(( int)({ struct _tagged_string _temp2807= s; char*
_temp2809= _temp2807.curr + 0; if( _temp2809 < _temp2807.base? 1: _temp2809 >=
_temp2807.last_plus_one){ _throw( Null_Exception);}* _temp2809;}) ==( int)'?'){
Cyc_Tcutil_terr( loc, xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2810= s; xprintf("unbound type variable %.*s",
_temp2810.last_plus_one - _temp2810.curr, _temp2810.curr);}));}}} goto _LL2787;
_LL2791: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2811=*(( struct Cyc_Absyn_Tvar*)
free_tvars->hd)->name; xprintf("unbound type variable %.*s", _temp2811.last_plus_one
- _temp2811.curr, _temp2811.curr);})); return; _LL2787:;}} void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List* free_tvars=
Cyc_Tcutil_check_valid_type( loc, te, 0, expected_kind, t); struct Cyc_List_List*
fs= free_tvars; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Tvar* ftv=(
struct Cyc_Absyn_Tvar*) fs->hd; struct _tagged_string* ftvn= ftv->name; struct
Cyc_Absyn_Conref* ftvk= ftv->kind; int present= 0;{ struct Cyc_List_List* bs=
bound_tvars; for( 0; bs != 0; bs= bs->tl){ struct Cyc_Absyn_Tvar* btv=( struct
Cyc_Absyn_Tvar*) bs->hd; struct _tagged_string* btvn= btv->name; struct Cyc_Absyn_Conref*
btvk= btv->kind; if( Cyc_String_zstrptrcmp( ftvn, btvn) == 0){ present= 1; if( !
Cyc_Tcutil_constrain_kinds( btvk, ftvk)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2812=* ftvn; struct _tagged_string _temp2813= Cyc_Absynpp_ckind2string(
btvk); struct _tagged_string _temp2814= Cyc_Absynpp_ckind2string( ftvk); xprintf("type variable %.*s has kind %.*s but is used here atkind %.*s",
_temp2812.last_plus_one - _temp2812.curr, _temp2812.curr, _temp2813.last_plus_one
- _temp2813.curr, _temp2813.curr, _temp2814.last_plus_one - _temp2814.curr,
_temp2814.curr);}));} break;}}} if( ! present){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2815=* ftvn; struct _tagged_string _temp2816= Cyc_Absynpp_typ2string(
t); xprintf("unbound type variable %.*s in type %.*s", _temp2815.last_plus_one -
_temp2815.curr, _temp2815.curr, _temp2816.last_plus_one - _temp2816.curr,
_temp2816.curr);}));}}} static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)(
void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for( 0;
vs != 0; vs= vs->tl){ struct Cyc_List_List* vs2= vs->tl; for( 0; vs2 != 0; vs2=
vs2->tl){ if( cmp(( void*) vs->hd,( void*) vs2->hd) == 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2817= msg; struct _tagged_string _temp2818= a2string((
void*) vs->hd); xprintf("%.*s: %.*s", _temp2817.last_plus_one - _temp2817.curr,
_temp2817.curr, _temp2818.last_plus_one - _temp2818.curr, _temp2818.curr);}));}}}}
static struct _tagged_string Cyc_Tcutil_strptr2string( struct _tagged_string* s){
return* s;} void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct
Cyc_Position_Segment* loc, struct _tagged_string msg){(( void(*)( int(* cmp)(
struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* vs,
struct Cyc_Position_Segment* loc, struct _tagged_string(* a2string)( struct
_tagged_string*), struct _tagged_string msg)) Cyc_Tcutil_check_unique_unsorted)(
Cyc_String_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string, msg);} void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs){(( void(*)( int(*
cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* vs,
struct Cyc_Position_Segment* loc, struct _tagged_string(* a2string)( struct Cyc_Absyn_Tvar*),
struct _tagged_string msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_Absyn_tvar_cmp,
tvs, loc, Cyc_Tcutil_tvar2string,( struct _tagged_string)({ char* _temp2819=(
char*)"duplicate type variable"; struct _tagged_string _temp2820; _temp2820.curr=
_temp2819; _temp2820.base= _temp2819; _temp2820.last_plus_one= _temp2819 + 24;
_temp2820;}));} struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd){ if( sd->fields == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2821=( char*)"inappropriate use of abstract struct"; struct _tagged_string
_temp2822; _temp2822.curr= _temp2821; _temp2822.base= _temp2821; _temp2822.last_plus_one=
_temp2821 + 37; _temp2822;}));}{ struct Cyc_List_List* fields= 0;{ struct Cyc_List_List*
sd_fields=( struct Cyc_List_List*)( sd->fields)->v; for( 0; sd_fields != 0;
sd_fields= sd_fields->tl){ if( Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)
sd_fields->hd)->name,( struct _tagged_string)({ char* _temp2823=( char*)"";
struct _tagged_string _temp2824; _temp2824.curr= _temp2823; _temp2824.base=
_temp2823; _temp2824.last_plus_one= _temp2823 + 1; _temp2824;})) != 0){ fields=({
struct Cyc_List_List* _temp2825=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2825->hd=( void*)({ struct _tuple15* _temp2826=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2826->f1=( struct
Cyc_Absyn_Structfield*) sd_fields->hd; _temp2826->f2= 0; _temp2826;}); _temp2825->tl=
fields; _temp2825;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des != 0;
des= des->tl){ struct _tuple16 _temp2829; void* _temp2830; struct Cyc_List_List*
_temp2832; struct _tuple16* _temp2827=( struct _tuple16*) des->hd; _temp2829=*
_temp2827; _LL2833: _temp2832= _temp2829.f1; goto _LL2831; _LL2831: _temp2830=
_temp2829.f2; goto _LL2828; _LL2828: if( _temp2832 == 0){ struct Cyc_List_List*
fields2= fields; for( 0; fields2 != 0; fields2= fields2->tl){ if( !(*(( struct
_tuple15*) fields2->hd)).f2){(*(( struct _tuple15*) fields2->hd)).f2= 1;(*((
struct _tuple16*) des->hd)).f1=({ struct Cyc_List_List* _temp2834=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2834->hd=( void*)(( void*)({
struct Cyc_Absyn_FieldName_struct* _temp2835=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp2835[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp2836; _temp2836.tag= Cyc_Absyn_FieldName_tag;
_temp2836.f1=((*(( struct _tuple15*) fields2->hd)).f1)->name; _temp2836;});
_temp2835;})); _temp2834->tl= 0; _temp2834;}); ans=({ struct Cyc_List_List*
_temp2837=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2837->hd=( void*)({ struct _tuple17* _temp2838=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp2838->f1=(*(( struct _tuple15*)
fields2->hd)).f1; _temp2838->f2= _temp2830; _temp2838;}); _temp2837->tl= ans;
_temp2837;}); break;}} if( fields2 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2839=( char*)"too many arguments to struct"; struct
_tagged_string _temp2840; _temp2840.curr= _temp2839; _temp2840.base= _temp2839;
_temp2840.last_plus_one= _temp2839 + 29; _temp2840;}));}} else{ if( _temp2832->tl
!= 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp2841=( char*)"multiple designators are not supported";
struct _tagged_string _temp2842; _temp2842.curr= _temp2841; _temp2842.base=
_temp2841; _temp2842.last_plus_one= _temp2841 + 39; _temp2842;}));} else{ void*
_temp2843=( void*) _temp2832->hd; struct Cyc_Absyn_Exp* _temp2849; struct
_tagged_string* _temp2851; _LL2845: if((( struct _tunion_struct*) _temp2843)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL2850: _temp2849=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp2843)->f1; goto _LL2846;} else{ goto
_LL2847;} _LL2847: if((( struct _tunion_struct*) _temp2843)->tag == Cyc_Absyn_FieldName_tag){
_LL2852: _temp2851=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp2843)->f1; goto _LL2848;} else{ goto _LL2844;} _LL2846: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2853=( char*)"array designator used in argument to struct";
struct _tagged_string _temp2854; _temp2854.curr= _temp2853; _temp2854.base=
_temp2853; _temp2854.last_plus_one= _temp2853 + 44; _temp2854;})); goto _LL2844;
_LL2848: { struct Cyc_List_List* fields2= fields; for( 0; fields2 != 0; fields2=
fields2->tl){ if( Cyc_String_zstrptrcmp( _temp2851,((*(( struct _tuple15*)
fields2->hd)).f1)->name) == 0){ if((*(( struct _tuple15*) fields2->hd)).f2){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2855=* _temp2851; xprintf("field %.*s has already been used as an argument",
_temp2855.last_plus_one - _temp2855.curr, _temp2855.curr);}));}(*(( struct
_tuple15*) fields2->hd)).f2= 1; ans=({ struct Cyc_List_List* _temp2856=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2856->hd=( void*)({
struct _tuple17* _temp2857=( struct _tuple17*) GC_malloc( sizeof( struct
_tuple17)); _temp2857->f1=(*(( struct _tuple15*) fields2->hd)).f1; _temp2857->f2=
_temp2830; _temp2857;}); _temp2856->tl= ans; _temp2856;}); break;}} if( fields2
== 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2858=* _temp2851;
xprintf("bad field designator %.*s", _temp2858.last_plus_one - _temp2858.curr,
_temp2858.curr);}));} goto _LL2844;} _LL2844:;}}} for( 0; fields != 0; fields=
fields->tl){ if( !(*(( struct _tuple15*) fields->hd)).f2){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2859=( char*)"too few arguments to struct";
struct _tagged_string _temp2860; _temp2860.curr= _temp2859; _temp2860.base=
_temp2859; _temp2860.last_plus_one= _temp2859 + 28; _temp2860;})); break;}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);}}} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* _temp2861= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp2867; struct Cyc_Absyn_Conref* _temp2869;
struct Cyc_Absyn_Tqual* _temp2871; struct Cyc_Absyn_Conref* _temp2873; void*
_temp2875; void* _temp2877; _LL2863: if(( unsigned int) _temp2861 > 4u?(( struct
_tunion_struct*) _temp2861)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2868:
_temp2867=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2861)->f1; _LL2878: _temp2877=( void*) _temp2867.elt_typ; goto _LL2876;
_LL2876: _temp2875=( void*) _temp2867.rgn_typ; goto _LL2874; _LL2874: _temp2873=(
struct Cyc_Absyn_Conref*) _temp2867.nullable; goto _LL2872; _LL2872: _temp2871=(
struct Cyc_Absyn_Tqual*) _temp2867.tq; goto _LL2870; _LL2870: _temp2869=( struct
Cyc_Absyn_Conref*) _temp2867.bounds; goto _LL2864;} else{ goto _LL2865;} _LL2865:
goto _LL2866; _LL2864: { struct Cyc_Absyn_Conref* b=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2869); void*
_temp2879=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( b))->v; void* _temp2887; _LL2881: if(( unsigned int)
_temp2879 > 1u?(( struct _tunion_struct*) _temp2879)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL2888: _temp2887=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2879)->f1;
if(( int) _temp2887 == Cyc_Absyn_Unknown_b){ goto _LL2882;} else{ goto _LL2883;}}
else{ goto _LL2883;} _LL2883: if(( int) _temp2879 == Cyc_Absyn_No_constr){ goto
_LL2884;} else{ goto _LL2885;} _LL2885: goto _LL2886; _LL2882: return 1; _LL2884:(
void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2889=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2889[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2890; _temp2890.tag= Cyc_Absyn_Eq_constr_tag;
_temp2890.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp2890;}); _temp2889;})));
return 1; _LL2886: return 0; _LL2880:;} _LL2866: return 0; _LL2862:;} struct
_tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple6 bogus_ans=({ struct _tuple6 _temp2891; _temp2891.f1= 0;
_temp2891.f2=( void*) Cyc_Absyn_HeapRgn; _temp2891;}); void* _temp2892=( void*)
e->r; void* _temp2906; struct _tuple1* _temp2908; struct _tagged_string*
_temp2910; struct Cyc_Absyn_Exp* _temp2912; struct _tagged_string* _temp2914;
struct Cyc_Absyn_Exp* _temp2916; struct Cyc_Absyn_Exp* _temp2918; struct Cyc_Absyn_Exp*
_temp2920; struct Cyc_Absyn_Exp* _temp2922; _LL2894: if((( struct _tunion_struct*)
_temp2892)->tag == Cyc_Absyn_Var_e_tag){ _LL2909: _temp2908=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp2892)->f1; goto _LL2907; _LL2907: _temp2906=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp2892)->f2; goto _LL2895;} else{
goto _LL2896;} _LL2896: if((( struct _tunion_struct*) _temp2892)->tag == Cyc_Absyn_StructMember_e_tag){
_LL2913: _temp2912=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp2892)->f1; goto _LL2911; _LL2911: _temp2910=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp2892)->f2; goto _LL2897;} else{
goto _LL2898;} _LL2898: if((( struct _tunion_struct*) _temp2892)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL2917: _temp2916=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp2892)->f1; goto _LL2915; _LL2915: _temp2914=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp2892)->f2; goto _LL2899;} else{
goto _LL2900;} _LL2900: if((( struct _tunion_struct*) _temp2892)->tag == Cyc_Absyn_Deref_e_tag){
_LL2919: _temp2918=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp2892)->f1; goto _LL2901;} else{ goto _LL2902;} _LL2902: if((( struct
_tunion_struct*) _temp2892)->tag == Cyc_Absyn_Subscript_e_tag){ _LL2923:
_temp2922=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp2892)->f1; goto _LL2921; _LL2921: _temp2920=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp2892)->f2; goto _LL2903;} else{ goto
_LL2904;} _LL2904: goto _LL2905; _LL2895: { void* _temp2924= _temp2906; struct
Cyc_Absyn_Fndecl* _temp2938; struct Cyc_Absyn_Vardecl* _temp2940; struct Cyc_Absyn_Vardecl*
_temp2942; struct Cyc_Absyn_Vardecl* _temp2944; struct Cyc_Absyn_Vardecl*
_temp2946; _LL2926: if(( int) _temp2924 == Cyc_Absyn_Unresolved_b){ goto _LL2927;}
else{ goto _LL2928;} _LL2928: if(( unsigned int) _temp2924 > 1u?(( struct
_tunion_struct*) _temp2924)->tag == Cyc_Absyn_Funname_b_tag: 0){ _LL2939:
_temp2938=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp2924)->f1; goto _LL2929;} else{ goto _LL2930;} _LL2930: if(( unsigned int)
_temp2924 > 1u?(( struct _tunion_struct*) _temp2924)->tag == Cyc_Absyn_Global_b_tag:
0){ _LL2941: _temp2940=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp2924)->f1; goto _LL2931;} else{ goto _LL2932;} _LL2932: if(( unsigned int)
_temp2924 > 1u?(( struct _tunion_struct*) _temp2924)->tag == Cyc_Absyn_Local_b_tag:
0){ _LL2943: _temp2942=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*)
_temp2924)->f1; goto _LL2933;} else{ goto _LL2934;} _LL2934: if(( unsigned int)
_temp2924 > 1u?(( struct _tunion_struct*) _temp2924)->tag == Cyc_Absyn_Pat_b_tag:
0){ _LL2945: _temp2944=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp2924)->f1; goto _LL2935;} else{ goto _LL2936;} _LL2936: if(( unsigned int)
_temp2924 > 1u?(( struct _tunion_struct*) _temp2924)->tag == Cyc_Absyn_Param_b_tag:
0){ _LL2947: _temp2946=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp2924)->f1; goto _LL2937;} else{ goto _LL2925;} _LL2927: return bogus_ans;
_LL2929: return({ struct _tuple6 _temp2948; _temp2948.f1= 1; _temp2948.f2=( void*)
Cyc_Absyn_HeapRgn; _temp2948;}); _LL2931: { void* _temp2949= Cyc_Tcutil_compress((
void*)( e->topt)->v); struct Cyc_Absyn_Exp* _temp2955; struct Cyc_Absyn_Tqual*
_temp2957; void* _temp2959; _LL2951: if(( unsigned int) _temp2949 > 4u?(( struct
_tunion_struct*) _temp2949)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL2960:
_temp2959=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2949)->f1; goto
_LL2958; _LL2958: _temp2957=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2949)->f2; goto _LL2956; _LL2956: _temp2955=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2949)->f3; goto _LL2952;} else{ goto
_LL2953;} _LL2953: goto _LL2954; _LL2952: return({ struct _tuple6 _temp2961;
_temp2961.f1= 1; _temp2961.f2=( void*) Cyc_Absyn_HeapRgn; _temp2961;}); _LL2954:
return({ struct _tuple6 _temp2962; _temp2962.f1=( _temp2940->tq)->q_const;
_temp2962.f2=( void*) Cyc_Absyn_HeapRgn; _temp2962;}); _LL2950:;} _LL2933: {
void* _temp2963= Cyc_Tcutil_compress(( void*)( e->topt)->v); struct Cyc_Absyn_Exp*
_temp2969; struct Cyc_Absyn_Tqual* _temp2971; void* _temp2973; _LL2965: if((
unsigned int) _temp2963 > 4u?(( struct _tunion_struct*) _temp2963)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2974: _temp2973=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2963)->f1;
goto _LL2972; _LL2972: _temp2971=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2963)->f2; goto _LL2970; _LL2970: _temp2969=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2963)->f3; goto _LL2966;} else{ goto
_LL2967;} _LL2967: goto _LL2968; _LL2966: return({ struct _tuple6 _temp2975;
_temp2975.f1= 1; _temp2975.f2=( void*)( _temp2942->region)->v; _temp2975;});
_LL2968: return({ struct _tuple6 _temp2976; _temp2976.f1=( _temp2942->tq)->q_const;
_temp2976.f2=( void*)( _temp2942->region)->v; _temp2976;}); _LL2964:;} _LL2935:
_temp2946= _temp2944; goto _LL2937; _LL2937: return({ struct _tuple6 _temp2977;
_temp2977.f1=( _temp2946->tq)->q_const; _temp2977.f2=( void*)( _temp2946->region)->v;
_temp2977;}); _LL2925:;} _LL2897: { void* _temp2978= Cyc_Tcutil_compress(( void*)(
_temp2912->topt)->v); struct Cyc_Absyn_Structdecl** _temp2986; struct Cyc_Absyn_Structdecl*
_temp2988; struct Cyc_List_List* _temp2989; struct _tuple1* _temp2991; struct
Cyc_Absyn_Uniondecl** _temp2993; struct Cyc_Absyn_Uniondecl* _temp2995; struct
Cyc_List_List* _temp2996; struct _tuple1* _temp2998; _LL2980: if(( unsigned int)
_temp2978 > 4u?(( struct _tunion_struct*) _temp2978)->tag == Cyc_Absyn_StructType_tag:
0){ _LL2992: _temp2991=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp2978)->f1; goto _LL2990; _LL2990: _temp2989=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp2978)->f2; goto _LL2987; _LL2987:
_temp2986=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp2978)->f3; if( _temp2986 == 0){ goto _LL2982;} else{ _temp2988=* _temp2986;
goto _LL2981;}} else{ goto _LL2982;} _LL2982: if(( unsigned int) _temp2978 > 4u?((
struct _tunion_struct*) _temp2978)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL2999:
_temp2998=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp2978)->f1;
goto _LL2997; _LL2997: _temp2996=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2978)->f2; goto _LL2994; _LL2994: _temp2993=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2978)->f3; if( _temp2993 == 0){ goto
_LL2984;} else{ _temp2995=* _temp2993; goto _LL2983;}} else{ goto _LL2984;}
_LL2984: goto _LL2985; _LL2981: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp2988, _temp2910); if( finfo != 0? finfo->width != 0: 0){ return({ struct
_tuple6 _temp3000; _temp3000.f1=( finfo->tq)->q_const; _temp3000.f2=( Cyc_Tcutil_addressof_props(
te, _temp2912)).f2; _temp3000;});} return bogus_ans;} _LL2983: { struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp2995, _temp2910); if( finfo != 0){
return({ struct _tuple6 _temp3001; _temp3001.f1=( finfo->tq)->q_const; _temp3001.f2=(
Cyc_Tcutil_addressof_props( te, _temp2912)).f2; _temp3001;});} goto _LL2985;}
_LL2985: return bogus_ans; _LL2979:;} _LL2899: { void* _temp3002= Cyc_Tcutil_compress((
void*)( _temp2916->topt)->v); struct Cyc_Absyn_PtrInfo _temp3008; struct Cyc_Absyn_Conref*
_temp3010; struct Cyc_Absyn_Tqual* _temp3012; struct Cyc_Absyn_Conref* _temp3014;
void* _temp3016; void* _temp3018; _LL3004: if(( unsigned int) _temp3002 > 4u?((
struct _tunion_struct*) _temp3002)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL3009: _temp3008=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp3002)->f1; _LL3019: _temp3018=( void*) _temp3008.elt_typ; goto _LL3017;
_LL3017: _temp3016=( void*) _temp3008.rgn_typ; goto _LL3015; _LL3015: _temp3014=(
struct Cyc_Absyn_Conref*) _temp3008.nullable; goto _LL3013; _LL3013: _temp3012=(
struct Cyc_Absyn_Tqual*) _temp3008.tq; goto _LL3011; _LL3011: _temp3010=( struct
Cyc_Absyn_Conref*) _temp3008.bounds; goto _LL3005;} else{ goto _LL3006;} _LL3006:
goto _LL3007; _LL3005: { void* _temp3020= Cyc_Tcutil_compress( _temp3018);
struct Cyc_Absyn_Structdecl** _temp3028; struct Cyc_Absyn_Structdecl* _temp3030;
struct Cyc_List_List* _temp3031; struct _tuple1* _temp3033; struct Cyc_Absyn_Uniondecl**
_temp3035; struct Cyc_Absyn_Uniondecl* _temp3037; struct Cyc_List_List*
_temp3038; struct _tuple1* _temp3040; _LL3022: if(( unsigned int) _temp3020 > 4u?((
struct _tunion_struct*) _temp3020)->tag == Cyc_Absyn_StructType_tag: 0){ _LL3034:
_temp3033=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3020)->f1;
goto _LL3032; _LL3032: _temp3031=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3020)->f2; goto _LL3029; _LL3029: _temp3028=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3020)->f3; if( _temp3028 == 0){ goto
_LL3024;} else{ _temp3030=* _temp3028; goto _LL3023;}} else{ goto _LL3024;}
_LL3024: if(( unsigned int) _temp3020 > 4u?(( struct _tunion_struct*) _temp3020)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL3041: _temp3040=( struct _tuple1*)(( struct
Cyc_Absyn_UnionType_struct*) _temp3020)->f1; goto _LL3039; _LL3039: _temp3038=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp3020)->f2;
goto _LL3036; _LL3036: _temp3035=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp3020)->f3; if( _temp3035 == 0){ goto _LL3026;} else{ _temp3037=* _temp3035;
goto _LL3025;}} else{ goto _LL3026;} _LL3026: goto _LL3027; _LL3023: { struct
Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field( _temp3030,
_temp2914); if( finfo != 0? finfo->width != 0: 0){ return({ struct _tuple6
_temp3042; _temp3042.f1=( finfo->tq)->q_const; _temp3042.f2= _temp3016;
_temp3042;});} return bogus_ans;} _LL3025: { struct Cyc_Absyn_Structfield* finfo=
Cyc_Absyn_lookup_union_field( _temp3037, _temp2914); if( finfo != 0){ return({
struct _tuple6 _temp3043; _temp3043.f1=( finfo->tq)->q_const; _temp3043.f2=
_temp3016; _temp3043;});} return bogus_ans;} _LL3027: return bogus_ans; _LL3021:;}
_LL3007: return bogus_ans; _LL3003:;} _LL2901: { void* _temp3044= Cyc_Tcutil_compress((
void*)( _temp2918->topt)->v); struct Cyc_Absyn_PtrInfo _temp3050; struct Cyc_Absyn_Conref*
_temp3052; struct Cyc_Absyn_Tqual* _temp3054; struct Cyc_Absyn_Conref* _temp3056;
void* _temp3058; void* _temp3060; _LL3046: if(( unsigned int) _temp3044 > 4u?((
struct _tunion_struct*) _temp3044)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL3051: _temp3050=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp3044)->f1; _LL3061: _temp3060=( void*) _temp3050.elt_typ; goto _LL3059;
_LL3059: _temp3058=( void*) _temp3050.rgn_typ; goto _LL3057; _LL3057: _temp3056=(
struct Cyc_Absyn_Conref*) _temp3050.nullable; goto _LL3055; _LL3055: _temp3054=(
struct Cyc_Absyn_Tqual*) _temp3050.tq; goto _LL3053; _LL3053: _temp3052=( struct
Cyc_Absyn_Conref*) _temp3050.bounds; goto _LL3047;} else{ goto _LL3048;} _LL3048:
goto _LL3049; _LL3047: return({ struct _tuple6 _temp3062; _temp3062.f1=
_temp3054->q_const; _temp3062.f2= _temp3058; _temp3062;}); _LL3049: return
bogus_ans; _LL3045:;} _LL2903: { void* t= Cyc_Tcutil_compress(( void*)(
_temp2922->topt)->v); void* _temp3063= t; struct Cyc_Absyn_Exp* _temp3073;
struct Cyc_Absyn_Tqual* _temp3075; void* _temp3077; struct Cyc_List_List*
_temp3079; struct Cyc_Absyn_PtrInfo _temp3081; struct Cyc_Absyn_Conref*
_temp3083; struct Cyc_Absyn_Tqual* _temp3085; struct Cyc_Absyn_Conref* _temp3087;
void* _temp3089; void* _temp3091; _LL3065: if(( unsigned int) _temp3063 > 4u?((
struct _tunion_struct*) _temp3063)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL3078:
_temp3077=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3063)->f1; goto
_LL3076; _LL3076: _temp3075=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3063)->f2; goto _LL3074; _LL3074: _temp3073=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3063)->f3; goto _LL3066;} else{ goto
_LL3067;} _LL3067: if(( unsigned int) _temp3063 > 4u?(( struct _tunion_struct*)
_temp3063)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL3080: _temp3079=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp3063)->f1; goto _LL3068;} else{ goto
_LL3069;} _LL3069: if(( unsigned int) _temp3063 > 4u?(( struct _tunion_struct*)
_temp3063)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL3082: _temp3081=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp3063)->f1;
_LL3092: _temp3091=( void*) _temp3081.elt_typ; goto _LL3090; _LL3090: _temp3089=(
void*) _temp3081.rgn_typ; goto _LL3088; _LL3088: _temp3087=( struct Cyc_Absyn_Conref*)
_temp3081.nullable; goto _LL3086; _LL3086: _temp3085=( struct Cyc_Absyn_Tqual*)
_temp3081.tq; goto _LL3084; _LL3084: _temp3083=( struct Cyc_Absyn_Conref*)
_temp3081.bounds; goto _LL3070;} else{ goto _LL3071;} _LL3071: goto _LL3072;
_LL3066: return({ struct _tuple6 _temp3093; _temp3093.f1= _temp3075->q_const;
_temp3093.f2=( Cyc_Tcutil_addressof_props( te, _temp2922)).f2; _temp3093;});
_LL3068: { struct _tuple4* finfo= Cyc_Absyn_lookup_tuple_field( _temp3079,( int)
Cyc_Evexp_eval_const_uint_exp( _temp2920)); if( finfo != 0){ return({ struct
_tuple6 _temp3094; _temp3094.f1=((* finfo).f1)->q_const; _temp3094.f2=( Cyc_Tcutil_addressof_props(
te, _temp2922)).f2; _temp3094;});} return bogus_ans;} _LL3070: return({ struct
_tuple6 _temp3095; _temp3095.f1= _temp3085->q_const; _temp3095.f2= _temp3089;
_temp3095;}); _LL3072: return bogus_ans; _LL3064:;} _LL2905: Cyc_Tcutil_terr( e->loc,(
struct _tagged_string)({ char* _temp3096=( char*)"unary & applied to non-lvalue";
struct _tagged_string _temp3097; _temp3097.curr= _temp3096; _temp3097.base=
_temp3096; _temp3097.last_plus_one= _temp3096 + 30; _temp3097;})); return
bogus_ans; _LL2893:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv* te,
void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3098= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Exp* _temp3104; struct Cyc_Absyn_Tqual* _temp3106; void*
_temp3108; _LL3100: if(( unsigned int) _temp3098 > 4u?(( struct _tunion_struct*)
_temp3098)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL3109: _temp3108=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3098)->f1; goto _LL3107; _LL3107:
_temp3106=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3098)->f2; goto _LL3105; _LL3105: _temp3104=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3098)->f3; goto _LL3101;} else{ goto
_LL3102;} _LL3102: goto _LL3103; _LL3101: { void* _temp3112; int _temp3114;
struct _tuple6 _temp3110= Cyc_Tcutil_addressof_props( te, e); _LL3115: _temp3114=
_temp3110.f1; goto _LL3113; _LL3113: _temp3112= _temp3110.f2; goto _LL3111;
_LL3111: return Cyc_Absyn_atb_typ( _temp3108, _temp3112, _temp3106,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp3116=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp3116[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp3117; _temp3117.tag= Cyc_Absyn_Upper_b_tag;
_temp3117.f1= e; _temp3117;}); _temp3116;}));} _LL3103: return e_typ; _LL3099:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( b);{ void* _temp3118=( void*) b->v; void*
_temp3126; void* _temp3128; struct Cyc_Absyn_Exp* _temp3130; _LL3120: if((
unsigned int) _temp3118 > 1u?(( struct _tunion_struct*) _temp3118)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL3127: _temp3126=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3118)->f1;
if(( int) _temp3126 == Cyc_Absyn_Unknown_b){ goto _LL3121;} else{ goto _LL3122;}}
else{ goto _LL3122;} _LL3122: if(( unsigned int) _temp3118 > 1u?(( struct
_tunion_struct*) _temp3118)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL3129:
_temp3128=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3118)->f1; if((
unsigned int) _temp3128 > 1u?(( struct _tunion_struct*) _temp3128)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL3131: _temp3130=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp3128)->f1; goto _LL3123;} else{ goto _LL3124;}} else{ goto _LL3124;}
_LL3124: goto _LL3125; _LL3121: return; _LL3123: if( Cyc_Evexp_eval_const_uint_exp(
_temp3130) <= i){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp3132=( char*)"dereference is out of bounds"; struct _tagged_string
_temp3133; _temp3133.curr= _temp3132; _temp3133.base= _temp3132; _temp3133.last_plus_one=
_temp3132 + 29; _temp3133;}));} return; _LL3125:( void*)( b->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp3134=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3134[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3135; _temp3135.tag= Cyc_Absyn_Eq_constr_tag;
_temp3135.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3136=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3136[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3137; _temp3137.tag= Cyc_Absyn_Upper_b_tag;
_temp3137.f1= Cyc_Absyn_uint_exp( i +( unsigned int) 1, 0); _temp3137;});
_temp3136;})); _temp3135;}); _temp3134;}))); return; _LL3119:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3138= Cyc_Tcutil_compress(
t); void* _temp3160; void* _temp3162; struct Cyc_Absyn_Enumdecl* _temp3164;
struct _tuple1* _temp3166; struct Cyc_Absyn_Exp* _temp3168; struct Cyc_Absyn_Tqual*
_temp3170; void* _temp3172; struct Cyc_List_List* _temp3174; struct Cyc_Absyn_Structdecl**
_temp3176; struct Cyc_List_List* _temp3178; struct _tuple1* _temp3180; struct
Cyc_Absyn_Uniondecl** _temp3182; struct Cyc_List_List* _temp3184; struct _tuple1*
_temp3186; _LL3140: if(( int) _temp3138 == Cyc_Absyn_VoidType){ goto _LL3141;}
else{ goto _LL3142;} _LL3142: if(( unsigned int) _temp3138 > 4u?(( struct
_tunion_struct*) _temp3138)->tag == Cyc_Absyn_IntType_tag: 0){ _LL3163:
_temp3162=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3138)->f1; goto
_LL3161; _LL3161: _temp3160=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3138)->f2; goto _LL3143;} else{ goto _LL3144;} _LL3144: if(( int) _temp3138
== Cyc_Absyn_FloatType){ goto _LL3145;} else{ goto _LL3146;} _LL3146: if(( int)
_temp3138 == Cyc_Absyn_DoubleType){ goto _LL3147;} else{ goto _LL3148;} _LL3148:
if(( unsigned int) _temp3138 > 4u?(( struct _tunion_struct*) _temp3138)->tag ==
Cyc_Absyn_EnumType_tag: 0){ _LL3167: _temp3166=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp3138)->f1; goto _LL3165; _LL3165: _temp3164=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp3138)->f2; goto _LL3149;} else{ goto
_LL3150;} _LL3150: if(( unsigned int) _temp3138 > 4u?(( struct _tunion_struct*)
_temp3138)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL3173: _temp3172=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3138)->f1; goto _LL3171; _LL3171:
_temp3170=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3138)->f2; goto _LL3169; _LL3169: _temp3168=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3138)->f3; goto _LL3151;} else{ goto
_LL3152;} _LL3152: if(( unsigned int) _temp3138 > 4u?(( struct _tunion_struct*)
_temp3138)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL3175: _temp3174=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp3138)->f1; goto _LL3153;} else{ goto
_LL3154;} _LL3154: if(( unsigned int) _temp3138 > 4u?(( struct _tunion_struct*)
_temp3138)->tag == Cyc_Absyn_StructType_tag: 0){ _LL3181: _temp3180=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3138)->f1; goto _LL3179;
_LL3179: _temp3178=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3138)->f2; goto _LL3177; _LL3177: _temp3176=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3138)->f3; goto _LL3155;} else{ goto
_LL3156;} _LL3156: if(( unsigned int) _temp3138 > 4u?(( struct _tunion_struct*)
_temp3138)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL3187: _temp3186=( struct
_tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp3138)->f1; goto _LL3185;
_LL3185: _temp3184=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3138)->f2; goto _LL3183; _LL3183: _temp3182=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp3138)->f3; goto _LL3157;} else{ goto
_LL3158;} _LL3158: goto _LL3159; _LL3141: return 1; _LL3143: return 1; _LL3145:
return 1; _LL3147: return 1; _LL3149: return 1; _LL3151: return Cyc_Tcutil_bits_only(
_temp3172); _LL3153: for( 0; _temp3174 != 0; _temp3174= _temp3174->tl){ if( !
Cyc_Tcutil_bits_only((*(( struct _tuple4*) _temp3174->hd)).f2)){ return 0;}}
return 1; _LL3155: if( _temp3176 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl*
sd=* _temp3176; if( sd->fields == 0){ return 0;}{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
sd->tvs, _temp3178);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( sd->fields)->v;
for( 0; fs != 0; fs= fs->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_substitute(
inst,( void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ return 0;}}}
return 1;}} _LL3157: if( _temp3182 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=* _temp3182; if( ud->fields == 0){ return 0;}{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
ud->tvs, _temp3184);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( ud->fields)->v;
for( 0; fs != 0; fs= fs->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_substitute(
inst,( void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ return 0;}}}
return 1;}} _LL3159: return 0; _LL3139:;} static int Cyc_Tcutil_cnst_exp( struct
Cyc_Tcenv_Tenv* te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3188=(
void*) e->r; void* _temp3226; void* _temp3228; struct Cyc_Absyn_Exp* _temp3230;
struct Cyc_Absyn_Enumfield* _temp3232; struct Cyc_Absyn_Enumdecl* _temp3234;
struct _tuple1* _temp3236; void* _temp3238; struct _tuple1* _temp3240; struct
Cyc_Absyn_Exp* _temp3242; struct Cyc_Absyn_Exp* _temp3244; struct Cyc_Absyn_Exp*
_temp3246; struct Cyc_Absyn_Exp* _temp3248; struct Cyc_Absyn_Exp* _temp3250;
struct Cyc_List_List* _temp3252; struct Cyc_Absyn_Exp* _temp3254; struct Cyc_Absyn_Exp*
_temp3256; void* _temp3258; struct Cyc_Absyn_Exp* _temp3260; struct Cyc_Absyn_Exp*
_temp3262; struct Cyc_Absyn_Exp* _temp3264; struct Cyc_Absyn_Vardecl* _temp3266;
struct Cyc_List_List* _temp3268; struct Cyc_Absyn_Structdecl* _temp3270; struct
Cyc_List_List* _temp3272; struct Cyc_Core_Opt* _temp3274; struct _tuple1*
_temp3276; struct Cyc_List_List* _temp3278; void* _temp3280; struct Cyc_List_List*
_temp3282; struct Cyc_Absyn_Tunionfield* _temp3284; struct Cyc_Absyn_Tuniondecl*
_temp3286; struct Cyc_List_List* _temp3288; struct Cyc_Core_Opt* _temp3290;
struct Cyc_Core_Opt* _temp3292; struct Cyc_Absyn_Tunionfield* _temp3294; struct
Cyc_Absyn_XTuniondecl* _temp3296; struct Cyc_List_List* _temp3298; struct Cyc_Core_Opt*
_temp3300; _LL3190: if((( struct _tunion_struct*) _temp3188)->tag == Cyc_Absyn_Const_e_tag){
_LL3227: _temp3226=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp3188)->f1;
goto _LL3191;} else{ goto _LL3192;} _LL3192: if((( struct _tunion_struct*)
_temp3188)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL3229: _temp3228=( void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp3188)->f1; goto _LL3193;} else{ goto
_LL3194;} _LL3194: if((( struct _tunion_struct*) _temp3188)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL3231: _temp3230=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp3188)->f1; goto _LL3195;} else{ goto _LL3196;} _LL3196: if((( struct
_tunion_struct*) _temp3188)->tag == Cyc_Absyn_Enum_e_tag){ _LL3237: _temp3236=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp3188)->f1; goto _LL3235;
_LL3235: _temp3234=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp3188)->f2; goto _LL3233; _LL3233: _temp3232=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp3188)->f3; goto _LL3197;} else{ goto
_LL3198;} _LL3198: if((( struct _tunion_struct*) _temp3188)->tag == Cyc_Absyn_Var_e_tag){
_LL3241: _temp3240=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3188)->f1; goto _LL3239; _LL3239: _temp3238=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3188)->f2; goto _LL3199;} else{ goto _LL3200;} _LL3200: if((( struct
_tunion_struct*) _temp3188)->tag == Cyc_Absyn_Conditional_e_tag){ _LL3247:
_temp3246=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3188)->f1; goto _LL3245; _LL3245: _temp3244=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp3188)->f2; goto _LL3243; _LL3243:
_temp3242=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3188)->f3; goto _LL3201;} else{ goto _LL3202;} _LL3202: if((( struct
_tunion_struct*) _temp3188)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL3251: _temp3250=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp3188)->f1; goto
_LL3249; _LL3249: _temp3248=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3188)->f2; goto _LL3203;} else{ goto _LL3204;} _LL3204: if((( struct
_tunion_struct*) _temp3188)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL3255:
_temp3254=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3188)->f1; goto _LL3253; _LL3253: _temp3252=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp3188)->f2; goto _LL3205;} else{
goto _LL3206;} _LL3206: if((( struct _tunion_struct*) _temp3188)->tag == Cyc_Absyn_Cast_e_tag){
_LL3259: _temp3258=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp3188)->f1;
goto _LL3257; _LL3257: _temp3256=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3188)->f2; goto _LL3207;} else{ goto _LL3208;} _LL3208: if((( struct
_tunion_struct*) _temp3188)->tag == Cyc_Absyn_Address_e_tag){ _LL3261: _temp3260=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp3188)->f1;
goto _LL3209;} else{ goto _LL3210;} _LL3210: if((( struct _tunion_struct*)
_temp3188)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL3267: _temp3266=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp3188)->f1;
goto _LL3265; _LL3265: _temp3264=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3188)->f2; goto _LL3263; _LL3263: _temp3262=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp3188)->f3; goto _LL3211;} else{
goto _LL3212;} _LL3212: if((( struct _tunion_struct*) _temp3188)->tag == Cyc_Absyn_Array_e_tag){
_LL3269: _temp3268=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp3188)->f1; goto _LL3213;} else{ goto _LL3214;} _LL3214: if((( struct
_tunion_struct*) _temp3188)->tag == Cyc_Absyn_Struct_e_tag){ _LL3277: _temp3276=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp3188)->f1; goto
_LL3275; _LL3275: _temp3274=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp3188)->f2; goto _LL3273; _LL3273: _temp3272=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp3188)->f3; goto _LL3271; _LL3271:
_temp3270=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp3188)->f4; goto _LL3215;} else{ goto _LL3216;} _LL3216: if((( struct
_tunion_struct*) _temp3188)->tag == Cyc_Absyn_Primop_e_tag){ _LL3281: _temp3280=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp3188)->f1; goto _LL3279;
_LL3279: _temp3278=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3188)->f2; goto _LL3217;} else{ goto _LL3218;} _LL3218: if((( struct
_tunion_struct*) _temp3188)->tag == Cyc_Absyn_Tuple_e_tag){ _LL3283: _temp3282=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp3188)->f1; goto
_LL3219;} else{ goto _LL3220;} _LL3220: if((( struct _tunion_struct*) _temp3188)->tag
== Cyc_Absyn_Tunion_e_tag){ _LL3293: _temp3292=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp3188)->f1; goto _LL3291; _LL3291: _temp3290=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp3188)->f2; goto
_LL3289; _LL3289: _temp3288=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3188)->f3; goto _LL3287; _LL3287: _temp3286=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp3188)->f4; goto _LL3285; _LL3285:
_temp3284=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3188)->f5; goto _LL3221;} else{ goto _LL3222;} _LL3222: if((( struct
_tunion_struct*) _temp3188)->tag == Cyc_Absyn_XTunion_e_tag){ _LL3301: _temp3300=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp3188)->f1; goto
_LL3299; _LL3299: _temp3298=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp3188)->f2; goto _LL3297; _LL3297: _temp3296=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp3188)->f3; goto _LL3295; _LL3295:
_temp3294=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp3188)->f4; goto _LL3223;} else{ goto _LL3224;} _LL3224: goto _LL3225;
_LL3191: return 1; _LL3193: return 1; _LL3195: return Cyc_Tcutil_cnst_exp( te, 1,
_temp3230); _LL3197: return 1; _LL3199: { struct _handler_cons _temp3302;
_push_handler(& _temp3302);{ struct _xtunion_struct* _temp3303=( struct
_xtunion_struct*) setjmp( _temp3302.handler); if( ! _temp3303){{ void* _temp3304=
Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3240); void* _temp3312; struct Cyc_Absyn_Fndecl*
_temp3314; void* _temp3316; struct Cyc_Absyn_Vardecl* _temp3318; _LL3306: if(((
struct _tunion_struct*) _temp3304)->tag == Cyc_Tcenv_VarRes_tag){ _LL3313:
_temp3312=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3304)->f1; if((
unsigned int) _temp3312 > 1u?(( struct _tunion_struct*) _temp3312)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL3315: _temp3314=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp3312)->f1; goto _LL3307;} else{ goto _LL3308;}} else{ goto _LL3308;}
_LL3308: if((( struct _tunion_struct*) _temp3304)->tag == Cyc_Tcenv_VarRes_tag){
_LL3317: _temp3316=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3304)->f1;
if(( unsigned int) _temp3316 > 1u?(( struct _tunion_struct*) _temp3316)->tag ==
Cyc_Absyn_Global_b_tag: 0){ _LL3319: _temp3318=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp3316)->f1; goto _LL3309;} else{ goto
_LL3310;}} else{ goto _LL3310;} _LL3310: goto _LL3311; _LL3307: { int _temp3320=
1; _npop_handler( 0u); return _temp3320;} _LL3309: { void* _temp3321= Cyc_Tcutil_compress((
void*) _temp3318->type); struct Cyc_Absyn_Exp* _temp3327; struct Cyc_Absyn_Tqual*
_temp3329; void* _temp3331; _LL3323: if(( unsigned int) _temp3321 > 4u?(( struct
_tunion_struct*) _temp3321)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL3332:
_temp3331=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3321)->f1; goto
_LL3330; _LL3330: _temp3329=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3321)->f2; goto _LL3328; _LL3328: _temp3327=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3321)->f3; goto _LL3324;} else{ goto
_LL3325;} _LL3325: goto _LL3326; _LL3324: { int _temp3333= 1; _npop_handler( 0u);
return _temp3333;} _LL3326: { int _temp3334= var_okay; _npop_handler( 0u);
return _temp3334;} _LL3322:;} _LL3311: { int _temp3335= var_okay; _npop_handler(
0u); return _temp3335;} _LL3305:;}; _pop_handler();} else{ struct
_xtunion_struct* _temp3337= _temp3303; _LL3339: if( _temp3337->tag == Cyc_Dict_Absent_tag){
goto _LL3340;} else{ goto _LL3341;} _LL3341: goto _LL3342; _LL3340: return 0;
_LL3342:( void) _throw( _temp3337); _LL3338:;}}} _LL3201: return( Cyc_Tcutil_cnst_exp(
te, 0, _temp3246)? Cyc_Tcutil_cnst_exp( te, 0, _temp3244): 0)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3242): 0; _LL3203: return Cyc_Tcutil_cnst_exp( te, 0, _temp3250)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3248): 0; _LL3205: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3254); _LL3207: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp3256); _LL3209: return Cyc_Tcutil_cnst_exp( te, 1, _temp3260); _LL3211:
return Cyc_Tcutil_cnst_exp( te, 0, _temp3264)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3262): 0; _LL3213: _temp3272= _temp3268; goto _LL3215; _LL3215: for( 0;
_temp3272 != 0; _temp3272= _temp3272->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,(*((
struct _tuple18*) _temp3272->hd)).f2)){ return 0;}} return 1; _LL3217: _temp3282=
_temp3278; goto _LL3219; _LL3219: _temp3288= _temp3282; goto _LL3221; _LL3221:
_temp3298= _temp3288; goto _LL3223; _LL3223: for( 0; _temp3298 != 0; _temp3298=
_temp3298->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,( struct Cyc_Absyn_Exp*)
_temp3298->hd)){ return 0;}} return 1; _LL3225: return 0; _LL3189:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);}