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
rgn; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc;
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
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral( struct
Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_is_function_type( void* t); extern void* Cyc_Tcutil_max_arithmetic_type(
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
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush( Cyc_Stdio_stderr);(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Tcutil_TypeErr_struct*
_temp1=( struct Cyc_Tcutil_TypeErr_struct*) GC_malloc( sizeof( struct Cyc_Tcutil_TypeErr_struct));
_temp1[ 0]=({ struct Cyc_Tcutil_TypeErr_struct _temp2; _temp2.tag= Cyc_Tcutil_TypeErr_tag;
_temp2;}); _temp1;}));} static struct _tagged_string Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar* tv){ return* tv->name;} void Cyc_Tcutil_print_tvars(
struct Cyc_List_List* tvs){ for( 0; tvs != 0; tvs=({ struct Cyc_List_List*
_temp3= tvs; if( _temp3 == 0){ _throw( Null_Exception);} _temp3->tl;})){({
struct _tagged_string _temp6= Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp4= tvs; if( _temp4 == 0){ _throw( Null_Exception);}
_temp4->hd;})); struct _tagged_string _temp7= Cyc_Absynpp_ckind2string((( struct
Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp5= tvs; if( _temp5 == 0){ _throw(
Null_Exception);} _temp5->hd;}))->kind); fprintf( Cyc_Stdio_stderr,"%.*s::%.*s ",
_temp6.last_plus_one - _temp6.curr, _temp6.curr, _temp7.last_plus_one - _temp7.curr,
_temp7.curr);});} fprintf( Cyc_Stdio_stderr,"\n"); Cyc_Stdio_fflush( Cyc_Stdio_stderr);}
static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0; static struct Cyc_List_List*
Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn( struct Cyc_Position_Segment*
sg, struct _tagged_string msg){ Cyc_Tcutil_warning_segs=({ struct Cyc_List_List*
_temp8=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp8->hd=( void*) sg; _temp8->tl= Cyc_Tcutil_warning_segs; _temp8;}); Cyc_Tcutil_warning_msgs=({
struct Cyc_List_List* _temp9=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp9->hd=( void*)(( struct _tagged_string*)({ struct
_tagged_string* _temp10=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp10[ 0]= msg; _temp10;})); _temp9->tl= Cyc_Tcutil_warning_msgs;
_temp9;});} void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs == 0){
return;} fprintf( Cyc_Stdio_stderr,"***Warnings***\n");{ struct Cyc_List_List*
seg_strs= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs); Cyc_Tcutil_warning_segs=
0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while( Cyc_Tcutil_warning_msgs != 0) {({
struct _tagged_string _temp13=*(( struct _tagged_string*)({ struct Cyc_List_List*
_temp11= seg_strs; if( _temp11 == 0){ _throw( Null_Exception);} _temp11->hd;}));
struct _tagged_string _temp14=*(( struct _tagged_string*)({ struct Cyc_List_List*
_temp12= Cyc_Tcutil_warning_msgs; if( _temp12 == 0){ _throw( Null_Exception);}
_temp12->hd;})); fprintf( Cyc_Stdio_stderr,"%.*s: %.*s\n", _temp13.last_plus_one
- _temp13.curr, _temp13.curr, _temp14.last_plus_one - _temp14.curr, _temp14.curr);});
seg_strs=({ struct Cyc_List_List* _temp15= seg_strs; if( _temp15 == 0){ _throw(
Null_Exception);} _temp15->tl;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List*
_temp16= Cyc_Tcutil_warning_msgs; if( _temp16 == 0){ _throw( Null_Exception);}
_temp16->tl;});} fprintf( Cyc_Stdio_stderr,"**************\n"); Cyc_Stdio_fflush(
Cyc_Stdio_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set= 0; void* Cyc_Tcutil_compress(
void* t){ void* _temp17= t; int _temp29; struct Cyc_Core_Opt* _temp31; void*
_temp33; int _temp35; struct Cyc_Core_Opt* _temp37; struct Cyc_Core_Opt**
_temp39; void* _temp40; struct Cyc_Core_Opt* _temp42; struct Cyc_List_List*
_temp44; struct _tuple1* _temp46; struct Cyc_Core_Opt* _temp48; struct Cyc_Core_Opt**
_temp50; struct Cyc_List_List* _temp51; struct _tuple1* _temp53; _LL19: if((
unsigned int) _temp17 > 4u?(( struct _tunion_struct*) _temp17)->tag == Cyc_Absyn_Evar_tag:
0){ _LL34: _temp33=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp17)->f1; goto
_LL32; _LL32: _temp31=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp17)->f2; if( _temp31 == 0){ goto _LL30;} else{ goto _LL21;} _LL30: _temp29=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp17)->f3; goto _LL20;} else{ goto
_LL21;} _LL21: if(( unsigned int) _temp17 > 4u?(( struct _tunion_struct*)
_temp17)->tag == Cyc_Absyn_Evar_tag: 0){ _LL41: _temp40=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp17)->f1; goto _LL38; _LL38: _temp37=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp17)->f2; _temp39=&(( struct Cyc_Absyn_Evar_struct*) _temp17)->f2; goto
_LL36; _LL36: _temp35=( int)(( struct Cyc_Absyn_Evar_struct*) _temp17)->f3; goto
_LL22;} else{ goto _LL23;} _LL23: if(( unsigned int) _temp17 > 4u?(( struct
_tunion_struct*) _temp17)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL47: _temp46=(
struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp17)->f1; goto
_LL45; _LL45: _temp44=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp17)->f2; goto _LL43; _LL43: _temp42=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp17)->f3; if( _temp42 == 0){ goto _LL24;} else{ goto _LL25;}} else{ goto
_LL25;} _LL25: if(( unsigned int) _temp17 > 4u?(( struct _tunion_struct*)
_temp17)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL54: _temp53=( struct _tuple1*)((
struct Cyc_Absyn_TypedefType_struct*) _temp17)->f1; goto _LL52; _LL52: _temp51=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*) _temp17)->f2;
goto _LL49; _LL49: _temp48=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp17)->f3; _temp50=&(( struct Cyc_Absyn_TypedefType_struct*) _temp17)->f3;
goto _LL26;} else{ goto _LL27;} _LL27: goto _LL28; _LL20: return t; _LL22: {
void* t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp57=* _temp39;
if( _temp57 == 0){ _throw( Null_Exception);} _temp57->v;})); if( t2 !=( void*)({
struct Cyc_Core_Opt* _temp55=* _temp39; if( _temp55 == 0){ _throw(
Null_Exception);} _temp55->v;})){* _temp39=({ struct Cyc_Core_Opt* _temp56=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp56->v=(
void*) t2; _temp56;});} return t2;} _LL24: return t; _LL26: { void* t2= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp60=* _temp50; if( _temp60 == 0){ _throw(
Null_Exception);} _temp60->v;})); if( t2 !=( void*)({ struct Cyc_Core_Opt*
_temp58=* _temp50; if( _temp58 == 0){ _throw( Null_Exception);} _temp58->v;})){*
_temp50=({ struct Cyc_Core_Opt* _temp59=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp59->v=( void*) t2; _temp59;});} return t2;}
_LL28: return t; _LL18:;} static int Cyc_Tcutil_kind_leq( void* k1, void* k2){
if( k1 == k2){ return 1;}{ struct _tuple7 _temp62=({ struct _tuple7 _temp61;
_temp61.f1= k1; _temp61.f2= k2; _temp61;}); void* _temp72; void* _temp74; void*
_temp76; void* _temp78; void* _temp80; void* _temp82; _LL64: _LL75: _temp74=
_temp62.f1; if(( int) _temp74 == Cyc_Absyn_BoxKind){ goto _LL73;} else{ goto
_LL66;} _LL73: _temp72= _temp62.f2; if(( int) _temp72 == Cyc_Absyn_MemKind){
goto _LL65;} else{ goto _LL66;} _LL66: _LL79: _temp78= _temp62.f1; if(( int)
_temp78 == Cyc_Absyn_BoxKind){ goto _LL77;} else{ goto _LL68;} _LL77: _temp76=
_temp62.f2; if(( int) _temp76 == Cyc_Absyn_AnyKind){ goto _LL67;} else{ goto
_LL68;} _LL68: _LL83: _temp82= _temp62.f1; if(( int) _temp82 == Cyc_Absyn_MemKind){
goto _LL81;} else{ goto _LL70;} _LL81: _temp80= _temp62.f2; if(( int) _temp80 ==
Cyc_Absyn_AnyKind){ goto _LL69;} else{ goto _LL70;} _LL70: goto _LL71; _LL65:
return 1; _LL67: return 1; _LL69: return 1; _LL71: return 0; _LL63:;}} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp84= Cyc_Tcutil_compress( t); int _temp134; struct Cyc_Core_Opt*
_temp136; void* _temp138; struct Cyc_Absyn_Tvar* _temp140; void* _temp142; void*
_temp144; struct Cyc_Absyn_FnInfo _temp146; void* _temp148; struct Cyc_Absyn_TunionInfo
_temp150; struct Cyc_Absyn_XTunionInfo _temp152; struct Cyc_Absyn_TunionFieldInfo
_temp154; struct Cyc_Absyn_Tunionfield* _temp156; struct Cyc_Absyn_Tuniondecl*
_temp158; struct _tuple1* _temp160; struct Cyc_List_List* _temp162; struct
_tuple1* _temp164; struct Cyc_Absyn_XTunionFieldInfo _temp166; struct Cyc_Absyn_Tunionfield*
_temp168; struct Cyc_Absyn_XTuniondecl* _temp170; struct _tuple1* _temp172;
struct _tuple1* _temp174; struct Cyc_Absyn_Structdecl** _temp176; struct Cyc_List_List*
_temp178; struct _tuple1* _temp180; struct Cyc_Absyn_Uniondecl** _temp182;
struct Cyc_List_List* _temp184; struct _tuple1* _temp186; struct Cyc_Absyn_Structdecl**
_temp188; struct Cyc_List_List* _temp190; struct _tuple1* _temp192; struct Cyc_Absyn_Uniondecl**
_temp194; struct Cyc_List_List* _temp196; struct _tuple1* _temp198; struct Cyc_Absyn_Enumdecl*
_temp200; struct _tuple1* _temp202; struct Cyc_Absyn_PtrInfo _temp204; struct
Cyc_Absyn_Exp* _temp206; struct Cyc_Absyn_Tqual* _temp208; void* _temp210;
struct Cyc_List_List* _temp212; struct Cyc_Core_Opt* _temp214; struct Cyc_List_List*
_temp216; struct _tuple1* _temp218; void* _temp220; struct Cyc_List_List*
_temp222; _LL86: if(( unsigned int) _temp84 > 4u?(( struct _tunion_struct*)
_temp84)->tag == Cyc_Absyn_Evar_tag: 0){ _LL139: _temp138=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp84)->f1; goto _LL137; _LL137: _temp136=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp84)->f2; goto _LL135; _LL135: _temp134=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp84)->f3; goto _LL87;} else{ goto _LL88;} _LL88: if(( unsigned int) _temp84
> 4u?(( struct _tunion_struct*) _temp84)->tag == Cyc_Absyn_VarType_tag: 0){
_LL141: _temp140=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp84)->f1; goto _LL89;} else{ goto _LL90;} _LL90: if(( int) _temp84 == Cyc_Absyn_VoidType){
goto _LL91;} else{ goto _LL92;} _LL92: if(( unsigned int) _temp84 > 4u?(( struct
_tunion_struct*) _temp84)->tag == Cyc_Absyn_IntType_tag: 0){ _LL145: _temp144=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp84)->f1; goto _LL143; _LL143:
_temp142=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp84)->f2; goto _LL93;}
else{ goto _LL94;} _LL94: if(( int) _temp84 == Cyc_Absyn_FloatType){ goto _LL95;}
else{ goto _LL96;} _LL96: if(( int) _temp84 == Cyc_Absyn_DoubleType){ goto _LL97;}
else{ goto _LL98;} _LL98: if(( unsigned int) _temp84 > 4u?(( struct
_tunion_struct*) _temp84)->tag == Cyc_Absyn_FnType_tag: 0){ _LL147: _temp146=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp84)->f1; goto
_LL99;} else{ goto _LL100;} _LL100: if(( unsigned int) _temp84 > 4u?(( struct
_tunion_struct*) _temp84)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL149:
_temp148=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp84)->f1; goto
_LL101;} else{ goto _LL102;} _LL102: if(( int) _temp84 == Cyc_Absyn_HeapRgn){
goto _LL103;} else{ goto _LL104;} _LL104: if(( unsigned int) _temp84 > 4u?((
struct _tunion_struct*) _temp84)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL151:
_temp150=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp84)->f1; goto _LL105;} else{ goto _LL106;} _LL106: if(( unsigned int)
_temp84 > 4u?(( struct _tunion_struct*) _temp84)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL153: _temp152=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp84)->f1; goto _LL107;} else{ goto _LL108;} _LL108: if(( unsigned int)
_temp84 > 4u?(( struct _tunion_struct*) _temp84)->tag == Cyc_Absyn_TunionFieldType_tag:
0){ _LL155: _temp154=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp84)->f1; _LL165: _temp164=( struct _tuple1*) _temp154.name; goto _LL163;
_LL163: _temp162=( struct Cyc_List_List*) _temp154.targs; goto _LL161; _LL161:
_temp160=( struct _tuple1*) _temp154.fname; goto _LL159; _LL159: _temp158=(
struct Cyc_Absyn_Tuniondecl*) _temp154.tud; goto _LL157; _LL157: _temp156=(
struct Cyc_Absyn_Tunionfield*) _temp154.tufd; goto _LL109;} else{ goto _LL110;}
_LL110: if(( unsigned int) _temp84 > 4u?(( struct _tunion_struct*) _temp84)->tag
== Cyc_Absyn_XTunionFieldType_tag: 0){ _LL167: _temp166=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp84)->f1; _LL175: _temp174=(
struct _tuple1*) _temp166.name; goto _LL173; _LL173: _temp172=( struct _tuple1*)
_temp166.fname; goto _LL171; _LL171: _temp170=( struct Cyc_Absyn_XTuniondecl*)
_temp166.xtud; goto _LL169; _LL169: _temp168=( struct Cyc_Absyn_Tunionfield*)
_temp166.xtufd; goto _LL111;} else{ goto _LL112;} _LL112: if(( unsigned int)
_temp84 > 4u?(( struct _tunion_struct*) _temp84)->tag == Cyc_Absyn_StructType_tag:
0){ _LL181: _temp180=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp84)->f1; goto _LL179; _LL179: _temp178=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp84)->f2; goto _LL177; _LL177: _temp176=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp84)->f3;
if( _temp176 == 0){ goto _LL113;} else{ goto _LL114;}} else{ goto _LL114;}
_LL114: if(( unsigned int) _temp84 > 4u?(( struct _tunion_struct*) _temp84)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL187: _temp186=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp84)->f1; goto _LL185; _LL185: _temp184=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp84)->f2; goto _LL183; _LL183: _temp182=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp84)->f3;
if( _temp182 == 0){ goto _LL115;} else{ goto _LL116;}} else{ goto _LL116;}
_LL116: if(( unsigned int) _temp84 > 4u?(( struct _tunion_struct*) _temp84)->tag
== Cyc_Absyn_StructType_tag: 0){ _LL193: _temp192=( struct _tuple1*)(( struct
Cyc_Absyn_StructType_struct*) _temp84)->f1; goto _LL191; _LL191: _temp190=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp84)->f2; goto
_LL189; _LL189: _temp188=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp84)->f3; goto _LL117;} else{ goto _LL118;} _LL118: if(( unsigned int)
_temp84 > 4u?(( struct _tunion_struct*) _temp84)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL199: _temp198=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp84)->f1; goto _LL197; _LL197: _temp196=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp84)->f2; goto _LL195; _LL195: _temp194=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp84)->f3;
goto _LL119;} else{ goto _LL120;} _LL120: if(( unsigned int) _temp84 > 4u?((
struct _tunion_struct*) _temp84)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL203:
_temp202=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp84)->f1;
goto _LL201; _LL201: _temp200=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp84)->f2; goto _LL121;} else{ goto _LL122;} _LL122: if(( unsigned int)
_temp84 > 4u?(( struct _tunion_struct*) _temp84)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL205: _temp204=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp84)->f1; goto _LL123;} else{ goto _LL124;} _LL124: if(( unsigned int)
_temp84 > 4u?(( struct _tunion_struct*) _temp84)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL211: _temp210=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp84)->f1;
goto _LL209; _LL209: _temp208=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp84)->f2; goto _LL207; _LL207: _temp206=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp84)->f3; goto _LL125;} else{ goto _LL126;}
_LL126: if(( unsigned int) _temp84 > 4u?(( struct _tunion_struct*) _temp84)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL213: _temp212=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp84)->f1; goto _LL127;} else{ goto
_LL128;} _LL128: if(( unsigned int) _temp84 > 4u?(( struct _tunion_struct*)
_temp84)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL219: _temp218=( struct
_tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp84)->f1; goto _LL217;
_LL217: _temp216=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp84)->f2; goto _LL215; _LL215: _temp214=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp84)->f3; goto _LL129;} else{ goto _LL130;} _LL130: if(( unsigned int)
_temp84 > 4u?(( struct _tunion_struct*) _temp84)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL221: _temp220=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp84)->f1;
goto _LL131;} else{ goto _LL132;} _LL132: if(( unsigned int) _temp84 > 4u?((
struct _tunion_struct*) _temp84)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL223:
_temp222=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp84)->f1;
goto _LL133;} else{ goto _LL85;} _LL87: return _temp138; _LL89: return(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp140->kind); _LL91:
return( void*) Cyc_Absyn_MemKind; _LL93: return _temp142 ==( void*) Cyc_Absyn_B4?(
void*) Cyc_Absyn_BoxKind:( void*) Cyc_Absyn_MemKind; _LL95: return( void*) Cyc_Absyn_MemKind;
_LL97: return( void*) Cyc_Absyn_MemKind; _LL99: return( void*) Cyc_Absyn_MemKind;
_LL101: return( void*) Cyc_Absyn_BoxKind; _LL103: return( void*) Cyc_Absyn_RgnKind;
_LL105: return( void*) Cyc_Absyn_BoxKind; _LL107: return( void*) Cyc_Absyn_BoxKind;
_LL109: _temp168= _temp156; goto _LL111; _LL111: if( _temp168 == 0){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp224=( char*)"typ_kind: [X]TunionFieldInfo missing Tunionfield ref"; struct
_tagged_string _temp225; _temp225.curr= _temp224; _temp225.base= _temp224;
_temp225.last_plus_one= _temp224 + 53; _temp225;}));} if(({ struct Cyc_Absyn_Tunionfield*
_temp226= _temp168; if( _temp226 == 0){ _throw( Null_Exception);} _temp226->typs;})
== 0){ return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL113: return( void*) Cyc_Absyn_AnyKind; _LL115: return( void*) Cyc_Absyn_AnyKind;
_LL117: return( void*) Cyc_Absyn_MemKind; _LL119: return( void*) Cyc_Absyn_MemKind;
_LL121: return( void*) Cyc_Absyn_BoxKind; _LL123: { void* _temp227=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp204.bounds))->v; void* _temp237; void* _temp239; struct Cyc_Absyn_Exp*
_temp241; struct Cyc_Absyn_Conref* _temp243; _LL229: if(( unsigned int) _temp227
> 1u?(( struct _tunion_struct*) _temp227)->tag == Cyc_Absyn_Eq_constr_tag: 0){
_LL238: _temp237=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp227)->f1;
if(( int) _temp237 == Cyc_Absyn_Unknown_b){ goto _LL230;} else{ goto _LL231;}}
else{ goto _LL231;} _LL231: if(( unsigned int) _temp227 > 1u?(( struct
_tunion_struct*) _temp227)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL240: _temp239=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp227)->f1; if(( unsigned int)
_temp239 > 1u?(( struct _tunion_struct*) _temp239)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL242: _temp241=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp239)->f1; goto _LL232;} else{ goto _LL233;}} else{ goto _LL233;} _LL233:
if(( int) _temp227 == Cyc_Absyn_No_constr){ goto _LL234;} else{ goto _LL235;}
_LL235: if(( unsigned int) _temp227 > 1u?(( struct _tunion_struct*) _temp227)->tag
== Cyc_Absyn_Forward_constr_tag: 0){ _LL244: _temp243=( struct Cyc_Absyn_Conref*)((
struct Cyc_Absyn_Forward_constr_struct*) _temp227)->f1; goto _LL236;} else{ goto
_LL228;} _LL230: return( void*) Cyc_Absyn_MemKind; _LL232: return( void*) Cyc_Absyn_BoxKind;
_LL234: return( void*) Cyc_Absyn_MemKind; _LL236: return(( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp245=(
char*)"typ_kind: forward constraint in ptr bounds"; struct _tagged_string
_temp246; _temp246.curr= _temp245; _temp246.base= _temp245; _temp246.last_plus_one=
_temp245 + 43; _temp246;})); _LL228:;} _LL125: return( void*) Cyc_Absyn_MemKind;
_LL127: return( void*) Cyc_Absyn_MemKind; _LL129: return(( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string _temp247= Cyc_Absynpp_typ2string(
t); xprintf("typ_kind: typedef found: %.*s", _temp247.last_plus_one - _temp247.curr,
_temp247.curr);})); _LL131: return( void*) Cyc_Absyn_EffKind; _LL133: return(
void*) Cyc_Absyn_EffKind; _LL85:;} char Cyc_Tcutil_Unify_tag[ 6u]="Unify";
struct Cyc_Tcutil_Unify_struct{ char* tag; } ; int Cyc_Tcutil_unify( void* t1,
void* t2){ struct _handler_cons _temp248; _push_handler(& _temp248);{ struct
_xtunion_struct* _temp249=( struct _xtunion_struct*) setjmp( _temp248.handler);
if( ! _temp249){ Cyc_Tcutil_unify_it( t1, t2);{ int _temp250= 1; _npop_handler(
0u); return _temp250;}; _pop_handler();} else{ struct _xtunion_struct* _temp252=
_temp249; _LL254: if( _temp252->tag == Cyc_Tcutil_Unify_tag){ goto _LL255;}
else{ goto _LL256;} _LL256: goto _LL257; _LL255: return 0; _LL257:( void) _throw(
_temp252); _LL253:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
ts); static void Cyc_Tcutil_occurs( void* evar, void* t){ void* _temp258= Cyc_Tcutil_compress(
t); int _temp290; struct Cyc_Core_Opt* _temp292; void* _temp294; struct Cyc_Absyn_PtrInfo
_temp296; struct Cyc_Absyn_Exp* _temp298; struct Cyc_Absyn_Tqual* _temp300; void*
_temp302; struct Cyc_Absyn_FnInfo _temp304; struct Cyc_List_List* _temp306; int
_temp308; struct Cyc_List_List* _temp310; void* _temp312; struct Cyc_Core_Opt*
_temp314; struct Cyc_List_List* _temp316; struct Cyc_List_List* _temp318; void*
_temp320; struct Cyc_Absyn_TunionInfo _temp322; struct Cyc_Absyn_Tuniondecl*
_temp324; void* _temp326; struct Cyc_List_List* _temp328; struct _tuple1*
_temp330; struct Cyc_Absyn_XTunionInfo _temp332; struct Cyc_Absyn_XTuniondecl*
_temp334; void* _temp336; struct _tuple1* _temp338; struct Cyc_Core_Opt*
_temp340; struct Cyc_List_List* _temp342; struct _tuple1* _temp344; struct Cyc_Absyn_TunionFieldInfo
_temp346; struct Cyc_Absyn_Tunionfield* _temp348; struct Cyc_Absyn_Tuniondecl*
_temp350; struct _tuple1* _temp352; struct Cyc_List_List* _temp354; struct
_tuple1* _temp356; struct Cyc_Absyn_Structdecl** _temp358; struct Cyc_List_List*
_temp360; struct _tuple1* _temp362; void* _temp364; struct Cyc_List_List*
_temp366; struct Cyc_Absyn_XTunionFieldInfo _temp368; _LL260: if(( unsigned int)
_temp258 > 4u?(( struct _tunion_struct*) _temp258)->tag == Cyc_Absyn_Evar_tag: 0){
_LL295: _temp294=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp258)->f1; goto
_LL293; _LL293: _temp292=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp258)->f2; goto _LL291; _LL291: _temp290=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp258)->f3; goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int)
_temp258 > 4u?(( struct _tunion_struct*) _temp258)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL297: _temp296=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp258)->f1; goto _LL263;} else{ goto _LL264;} _LL264: if(( unsigned int)
_temp258 > 4u?(( struct _tunion_struct*) _temp258)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL303: _temp302=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp258)->f1;
goto _LL301; _LL301: _temp300=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp258)->f2; goto _LL299; _LL299: _temp298=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp258)->f3; goto _LL265;} else{ goto _LL266;}
_LL266: if(( unsigned int) _temp258 > 4u?(( struct _tunion_struct*) _temp258)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL305: _temp304=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp258)->f1; _LL317: _temp316=( struct Cyc_List_List*)
_temp304.tvars; goto _LL315; _LL315: _temp314=( struct Cyc_Core_Opt*) _temp304.effect;
goto _LL313; _LL313: _temp312=( void*) _temp304.ret_typ; goto _LL311; _LL311:
_temp310=( struct Cyc_List_List*) _temp304.args; goto _LL309; _LL309: _temp308=(
int) _temp304.varargs; goto _LL307; _LL307: _temp306=( struct Cyc_List_List*)
_temp304.attributes; goto _LL267;} else{ goto _LL268;} _LL268: if(( unsigned int)
_temp258 > 4u?(( struct _tunion_struct*) _temp258)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL319: _temp318=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp258)->f1; goto _LL269;} else{ goto _LL270;} _LL270: if(( unsigned int)
_temp258 > 4u?(( struct _tunion_struct*) _temp258)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL321: _temp320=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp258)->f1;
goto _LL271;} else{ goto _LL272;} _LL272: if(( unsigned int) _temp258 > 4u?((
struct _tunion_struct*) _temp258)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL323:
_temp322=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp258)->f1; _LL331: _temp330=( struct _tuple1*) _temp322.name; goto _LL329;
_LL329: _temp328=( struct Cyc_List_List*) _temp322.targs; goto _LL327; _LL327:
_temp326=( void*) _temp322.rgn; goto _LL325; _LL325: _temp324=( struct Cyc_Absyn_Tuniondecl*)
_temp322.tud; goto _LL273;} else{ goto _LL274;} _LL274: if(( unsigned int)
_temp258 > 4u?(( struct _tunion_struct*) _temp258)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL333: _temp332=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp258)->f1; _LL339: _temp338=( struct _tuple1*) _temp332.name; goto _LL337;
_LL337: _temp336=( void*) _temp332.rgn; goto _LL335; _LL335: _temp334=( struct
Cyc_Absyn_XTuniondecl*) _temp332.xtud; goto _LL275;} else{ goto _LL276;} _LL276:
if(( unsigned int) _temp258 > 4u?(( struct _tunion_struct*) _temp258)->tag ==
Cyc_Absyn_TypedefType_tag: 0){ _LL345: _temp344=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp258)->f1; goto _LL343; _LL343: _temp342=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp258)->f2; goto _LL341; _LL341: _temp340=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*) _temp258)->f3;
goto _LL277;} else{ goto _LL278;} _LL278: if(( unsigned int) _temp258 > 4u?((
struct _tunion_struct*) _temp258)->tag == Cyc_Absyn_TunionFieldType_tag: 0){
_LL347: _temp346=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp258)->f1; _LL357: _temp356=( struct _tuple1*) _temp346.name; goto _LL355;
_LL355: _temp354=( struct Cyc_List_List*) _temp346.targs; goto _LL353; _LL353:
_temp352=( struct _tuple1*) _temp346.fname; goto _LL351; _LL351: _temp350=(
struct Cyc_Absyn_Tuniondecl*) _temp346.tud; goto _LL349; _LL349: _temp348=(
struct Cyc_Absyn_Tunionfield*) _temp346.tufd; goto _LL279;} else{ goto _LL280;}
_LL280: if(( unsigned int) _temp258 > 4u?(( struct _tunion_struct*) _temp258)->tag
== Cyc_Absyn_StructType_tag: 0){ _LL363: _temp362=( struct _tuple1*)(( struct
Cyc_Absyn_StructType_struct*) _temp258)->f1; goto _LL361; _LL361: _temp360=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp258)->f2;
goto _LL359; _LL359: _temp358=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp258)->f3; goto _LL281;} else{ goto _LL282;} _LL282: if(( unsigned int)
_temp258 > 4u?(( struct _tunion_struct*) _temp258)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL365: _temp364=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp258)->f1;
goto _LL283;} else{ goto _LL284;} _LL284: if(( unsigned int) _temp258 > 4u?((
struct _tunion_struct*) _temp258)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL367:
_temp366=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp258)->f1;
goto _LL285;} else{ goto _LL286;} _LL286: if(( unsigned int) _temp258 > 4u?((
struct _tunion_struct*) _temp258)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){
_LL369: _temp368=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp258)->f1; goto _LL287;} else{ goto _LL288;} _LL288: goto _LL289; _LL261:
if( t == evar){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Tcutil_Unify_struct*
_temp370=( struct Cyc_Tcutil_Unify_struct*) GC_malloc( sizeof( struct Cyc_Tcutil_Unify_struct));
_temp370[ 0]=({ struct Cyc_Tcutil_Unify_struct _temp371; _temp371.tag= Cyc_Tcutil_Unify_tag;
_temp371;}); _temp370;}));} else{ if( _temp292 != 0){ Cyc_Tcutil_occurs( evar,(
void*)({ struct Cyc_Core_Opt* _temp372= _temp292; if( _temp372 == 0){ _throw(
Null_Exception);} _temp372->v;}));}} goto _LL259; _LL263: Cyc_Tcutil_occurs(
evar,( void*) _temp296.elt_typ); Cyc_Tcutil_occurs( evar,( void*) _temp296.rgn_typ);
goto _LL259; _LL265: Cyc_Tcutil_occurs( evar, _temp302); goto _LL259; _LL267:
if( _temp314 != 0){ Cyc_Tcutil_occurs( evar,( void*)({ struct Cyc_Core_Opt*
_temp373= _temp314; if( _temp373 == 0){ _throw( Null_Exception);} _temp373->v;}));}
Cyc_Tcutil_occurs( evar, _temp312); for( 0; _temp310 != 0; _temp310=({ struct
Cyc_List_List* _temp374= _temp310; if( _temp374 == 0){ _throw( Null_Exception);}
_temp374->tl;})){ Cyc_Tcutil_occurs( evar,(*(( struct _tuple2*)({ struct Cyc_List_List*
_temp375= _temp310; if( _temp375 == 0){ _throw( Null_Exception);} _temp375->hd;}))).f3);}
goto _LL259; _LL269: for( 0; _temp318 != 0; _temp318=({ struct Cyc_List_List*
_temp376= _temp318; if( _temp376 == 0){ _throw( Null_Exception);} _temp376->tl;})){
Cyc_Tcutil_occurs( evar,(*(( struct _tuple4*)({ struct Cyc_List_List* _temp377=
_temp318; if( _temp377 == 0){ _throw( Null_Exception);} _temp377->hd;}))).f2);}
goto _LL259; _LL271: Cyc_Tcutil_occurs( evar, _temp320); goto _LL259; _LL273:
Cyc_Tcutil_occurs( evar, _temp326); Cyc_Tcutil_occurslist( evar, _temp328); goto
_LL259; _LL275: Cyc_Tcutil_occurs( evar, _temp336); goto _LL259; _LL277:
_temp354= _temp342; goto _LL279; _LL279: _temp360= _temp354; goto _LL281; _LL281:
Cyc_Tcutil_occurslist( evar, _temp360); goto _LL259; _LL283: Cyc_Tcutil_occurs(
evar, _temp364); goto _LL259; _LL285: Cyc_Tcutil_occurslist( evar, _temp366);
goto _LL259; _LL287: goto _LL259; _LL289: goto _LL259; _LL259:;} static void Cyc_Tcutil_occurslist(
void* evar, struct Cyc_List_List* ts){ for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp378= ts; if( _temp378 == 0){ _throw( Null_Exception);} _temp378->tl;})){
Cyc_Tcutil_occurs( evar,( void*)({ struct Cyc_List_List* _temp379= ts; if(
_temp379 == 0){ _throw( Null_Exception);} _temp379->hd;}));}} static void Cyc_Tcutil_unify_list(
struct Cyc_List_List* t1, struct Cyc_List_List* t2){ struct _handler_cons
_temp380; _push_handler(& _temp380);{ struct _xtunion_struct* _temp381=( struct
_xtunion_struct*) setjmp( _temp380.handler); if( ! _temp381){(( void(*)( void(*
f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_iter2)(
Cyc_Tcutil_unify_it, t1, t2);; _pop_handler();} else{ struct _xtunion_struct*
_temp383= _temp381; _LL385: if( _temp383->tag == Cyc_List_List_mismatch_tag){
goto _LL386;} else{ goto _LL387;} _LL387: goto _LL388; _LL386:( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Tcutil_Unify_struct* _temp389=( struct Cyc_Tcutil_Unify_struct*)
GC_malloc( sizeof( struct Cyc_Tcutil_Unify_struct)); _temp389[ 0]=({ struct Cyc_Tcutil_Unify_struct
_temp390; _temp390.tag= Cyc_Tcutil_Unify_tag; _temp390;}); _temp389;})); _LL388:(
void) _throw( _temp383); _LL384:;}}} static void Cyc_Tcutil_unify_tqual( struct
Cyc_Absyn_Tqual* tq1, struct Cyc_Absyn_Tqual* tq2){ if(( tq1->q_const != tq2->q_const?
1: tq1->q_volatile != tq2->q_volatile)? 1: tq1->q_restrict != tq2->q_restrict){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Tcutil_Unify_struct*
_temp391=( struct Cyc_Tcutil_Unify_struct*) GC_malloc( sizeof( struct Cyc_Tcutil_Unify_struct));
_temp391[ 0]=({ struct Cyc_Tcutil_Unify_struct _temp392; _temp392.tag= Cyc_Tcutil_Unify_tag;
_temp392;}); _temp391;}));}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual*
tq1, struct Cyc_Absyn_Tqual* tq2){ return( tq1->q_const == tq2->q_const? tq1->q_volatile
== tq2->q_volatile: 0)? tq1->q_restrict == tq2->q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
y); if( x == y){ return;}{ void* _temp393=( void*) x->v; struct Cyc_Absyn_Conref*
_temp401; void* _temp403; _LL395: if(( int) _temp393 == Cyc_Absyn_No_constr){
goto _LL396;} else{ goto _LL397;} _LL397: if(( unsigned int) _temp393 > 1u?((
struct _tunion_struct*) _temp393)->tag == Cyc_Absyn_Forward_constr_tag: 0){
_LL402: _temp401=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp393)->f1; goto _LL398;} else{ goto _LL399;} _LL399: if(( unsigned int)
_temp393 > 1u?(( struct _tunion_struct*) _temp393)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL404: _temp403=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp393)->f1;
goto _LL400;} else{ goto _LL394;} _LL396:( void*)( x->v=( void*)(( void*)({
struct Cyc_Absyn_Forward_constr_struct* _temp405=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp405[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp406; _temp406.tag= Cyc_Absyn_Forward_constr_tag;
_temp406.f1= y; _temp406;}); _temp405;}))); return; _LL398:( void) _throw(((
struct _xtunion_struct*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp407=( char*)"unify_conref: forward after compress";
struct _tagged_string _temp408; _temp408.curr= _temp407; _temp408.base= _temp407;
_temp408.last_plus_one= _temp407 + 37; _temp408;}))); _LL400: { void* _temp409=(
void*) y->v; struct Cyc_Absyn_Conref* _temp417; void* _temp419; _LL411: if(( int)
_temp409 == Cyc_Absyn_No_constr){ goto _LL412;} else{ goto _LL413;} _LL413: if((
unsigned int) _temp409 > 1u?(( struct _tunion_struct*) _temp409)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL418: _temp417=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp409)->f1; goto _LL414;} else{ goto _LL415;} _LL415: if(( unsigned int)
_temp409 > 1u?(( struct _tunion_struct*) _temp409)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL420: _temp419=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp409)->f1;
goto _LL416;} else{ goto _LL410;} _LL412:( void*)( y->v=( void*)(( void*) x->v));
return; _LL414:( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp421=(
char*)"unify_conref: forward after compress(2)"; struct _tagged_string _temp422;
_temp422.curr= _temp421; _temp422.base= _temp421; _temp422.last_plus_one=
_temp421 + 40; _temp422;}))); _LL416: if( cmp( _temp403, _temp419) != 0){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Tcutil_Unify_struct* _temp423=(
struct Cyc_Tcutil_Unify_struct*) GC_malloc( sizeof( struct Cyc_Tcutil_Unify_struct));
_temp423[ 0]=({ struct Cyc_Tcutil_Unify_struct _temp424; _temp424.tag= Cyc_Tcutil_Unify_tag;
_temp424;}); _temp423;}));} return; _LL410:;} _LL394:;}} static int Cyc_Tcutil_unify_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ struct _handler_cons _temp425; _push_handler(& _temp425);{ struct
_xtunion_struct* _temp426=( struct _xtunion_struct*) setjmp( _temp425.handler);
if( ! _temp426){(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int
_temp427= 1; _npop_handler( 0u); return _temp427;}; _pop_handler();} else{
struct _xtunion_struct* _temp429= _temp426; _LL431: if( _temp429->tag == Cyc_Tcutil_Unify_tag){
goto _LL432;} else{ goto _LL433;} _LL433: goto _LL434; _LL432: return 0; _LL434:(
void) _throw( _temp429); _LL430:;}}} static int Cyc_Tcutil_boundscmp( void* b1,
void* b2){ struct _tuple7 _temp436=({ struct _tuple7 _temp435; _temp435.f1= b1;
_temp435.f2= b2; _temp435;}); void* _temp446; void* _temp448; void* _temp450;
void* _temp452; void* _temp454; void* _temp456; void* _temp458; struct Cyc_Absyn_Exp*
_temp460; void* _temp462; struct Cyc_Absyn_Exp* _temp464; _LL438: _LL449:
_temp448= _temp436.f1; if(( int) _temp448 == Cyc_Absyn_Unknown_b){ goto _LL447;}
else{ goto _LL440;} _LL447: _temp446= _temp436.f2; if(( int) _temp446 == Cyc_Absyn_Unknown_b){
goto _LL439;} else{ goto _LL440;} _LL440: _LL453: _temp452= _temp436.f1; if((
int) _temp452 == Cyc_Absyn_Unknown_b){ goto _LL451;} else{ goto _LL442;} _LL451:
_temp450= _temp436.f2; goto _LL441; _LL442: _LL457: _temp456= _temp436.f1; goto
_LL455; _LL455: _temp454= _temp436.f2; if(( int) _temp454 == Cyc_Absyn_Unknown_b){
goto _LL443;} else{ goto _LL444;} _LL444: _LL463: _temp462= _temp436.f1; if((
unsigned int) _temp462 > 1u?(( struct _tunion_struct*) _temp462)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL465: _temp464=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp462)->f1; goto _LL459;} else{ goto _LL437;} _LL459: _temp458= _temp436.f2;
if(( unsigned int) _temp458 > 1u?(( struct _tunion_struct*) _temp458)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL461: _temp460=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp458)->f1; goto _LL445;} else{ goto _LL437;}
_LL439: return 0; _LL441: return - 1; _LL443: return 1; _LL445: { int i1=( int)
Cyc_Evexp_eval_const_uint_exp( _temp464); int i2=( int) Cyc_Evexp_eval_const_uint_exp(
_temp460); if( i1 == i2){ return 0;} if( i1 < i2){ return - 1;} return 1;}
_LL437:;} static int Cyc_Tcutil_equal_att( void* a1, void* a2){ if( a1 == a2){
return 1;}{ struct _tuple7 _temp467=({ struct _tuple7 _temp466; _temp466.f1= a1;
_temp466.f2= a2; _temp466;}); void* _temp477; int _temp479; void* _temp481; int
_temp483; void* _temp485; int _temp487; void* _temp489; int _temp491; void*
_temp493; struct _tagged_string _temp495; void* _temp497; struct _tagged_string
_temp499; _LL469: _LL482: _temp481= _temp467.f1; if(( unsigned int) _temp481 >
15u?(( struct _tunion_struct*) _temp481)->tag == Cyc_Absyn_Regparm_att_tag: 0){
_LL484: _temp483=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp481)->f1;
goto _LL478;} else{ goto _LL471;} _LL478: _temp477= _temp467.f2; if((
unsigned int) _temp477 > 15u?(( struct _tunion_struct*) _temp477)->tag == Cyc_Absyn_Regparm_att_tag:
0){ _LL480: _temp479=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp477)->f1;
goto _LL470;} else{ goto _LL471;} _LL471: _LL490: _temp489= _temp467.f1; if((
unsigned int) _temp489 > 15u?(( struct _tunion_struct*) _temp489)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL492: _temp491=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp489)->f1;
goto _LL486;} else{ goto _LL473;} _LL486: _temp485= _temp467.f2; if((
unsigned int) _temp485 > 15u?(( struct _tunion_struct*) _temp485)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL488: _temp487=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp485)->f1;
goto _LL472;} else{ goto _LL473;} _LL473: _LL498: _temp497= _temp467.f1; if((
unsigned int) _temp497 > 15u?(( struct _tunion_struct*) _temp497)->tag == Cyc_Absyn_Section_att_tag:
0){ _LL500: _temp499=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp497)->f1; goto _LL494;} else{ goto _LL475;} _LL494: _temp493= _temp467.f2;
if(( unsigned int) _temp493 > 15u?(( struct _tunion_struct*) _temp493)->tag ==
Cyc_Absyn_Section_att_tag: 0){ _LL496: _temp495=( struct _tagged_string)((
struct Cyc_Absyn_Section_att_struct*) _temp493)->f1; goto _LL474;} else{ goto
_LL475;} _LL475: goto _LL476; _LL470: _temp491= _temp483; _temp487= _temp479;
goto _LL472; _LL472: return _temp491 == _temp487; _LL474: return Cyc_String_strcmp(
_temp499, _temp495) == 0; _LL476: return 0; _LL468:;}} int Cyc_Tcutil_same_atts(
struct Cyc_List_List* a1, struct Cyc_List_List* a2){{ struct Cyc_List_List* a=
a1; for( 0; a != 0; a=({ struct Cyc_List_List* _temp501= a; if( _temp501 == 0){
_throw( Null_Exception);} _temp501->tl;})){ if( !(( int(*)( int(* pred)( void*,
void*), void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,(
void*)({ struct Cyc_List_List* _temp502= a; if( _temp502 == 0){ _throw(
Null_Exception);} _temp502->hd;}), a2)){ return 0;}}}{ struct Cyc_List_List* a=
a2; for( 0; a != 0; a=({ struct Cyc_List_List* _temp503= a; if( _temp503 == 0){
_throw( Null_Exception);} _temp503->tl;})){ if( !(( int(*)( int(* pred)( void*,
void*), void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,(
void*)({ struct Cyc_List_List* _temp504= a; if( _temp504 == 0){ _throw(
Null_Exception);} _temp504->hd;}), a1)){ return 0;}}} return 1;} static void Cyc_Tcutil_normalize_effect(
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp505= e; struct Cyc_List_List*
_temp511; struct Cyc_List_List** _temp513; _LL507: if(( unsigned int) _temp505 >
4u?(( struct _tunion_struct*) _temp505)->tag == Cyc_Absyn_JoinEff_tag: 0){
_LL512: _temp511=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp505)->f1; _temp513=&(( struct Cyc_Absyn_JoinEff_struct*) _temp505)->f1;
goto _LL508;} else{ goto _LL509;} _LL509: goto _LL510; _LL508: { int nested_join=
0;{ struct Cyc_List_List* effs=* _temp513; for( 0; effs != 0; effs=({ struct Cyc_List_List*
_temp514= effs; if( _temp514 == 0){ _throw( Null_Exception);} _temp514->tl;})){
void* eff=( void*)({ struct Cyc_List_List* _temp523= effs; if( _temp523 == 0){
_throw( Null_Exception);} _temp523->hd;}); Cyc_Tcutil_normalize_effect( eff);{
void* _temp515= Cyc_Tcutil_compress( eff); struct Cyc_List_List* _temp521;
_LL517: if(( unsigned int) _temp515 > 4u?(( struct _tunion_struct*) _temp515)->tag
== Cyc_Absyn_JoinEff_tag: 0){ _LL522: _temp521=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp515)->f1; goto _LL518;} else{ goto _LL519;}
_LL519: goto _LL520; _LL518: nested_join= 1; goto _LL516; _LL520: goto _LL516;
_LL516:;}}} if( ! nested_join){ return;}{ struct Cyc_List_List* effects= 0;{
struct Cyc_List_List* effs=* _temp513; for( 0; effs != 0; effs=({ struct Cyc_List_List*
_temp524= effs; if( _temp524 == 0){ _throw( Null_Exception);} _temp524->tl;})){
void* _temp526= Cyc_Tcutil_compress(( void*)({ struct Cyc_List_List* _temp525=
effs; if( _temp525 == 0){ _throw( Null_Exception);} _temp525->hd;})); struct Cyc_List_List*
_temp532; _LL528: if(( unsigned int) _temp526 > 4u?(( struct _tunion_struct*)
_temp526)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL533: _temp532=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp526)->f1; goto _LL529;} else{ goto _LL530;}
_LL530: goto _LL531; _LL529: effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_revappend)( _temp532, effects); goto
_LL527; _LL531: effects=({ struct Cyc_List_List* _temp534=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp534->hd=( void*) _temp526;
_temp534->tl= effects; _temp534;}); goto _LL527; _LL527:;}}* _temp513=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( effects); goto
_LL506;}} _LL510: goto _LL506; _LL506:;}} static struct _tuple8* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp535= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp543; struct Cyc_List_List _temp545; struct Cyc_List_List* _temp546; void*
_temp548; int _temp550; struct Cyc_Core_Opt* _temp552; struct Cyc_Core_Opt**
_temp554; void* _temp555; void** _temp557; _LL537: if(( unsigned int) _temp535 >
4u?(( struct _tunion_struct*) _temp535)->tag == Cyc_Absyn_JoinEff_tag: 0){
_LL544: _temp543=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp535)->f1; if( _temp543 == 0){ goto _LL539;} else{ _temp545=* _temp543;
_LL549: _temp548=( void*) _temp545.hd; goto _LL547; _LL547: _temp546=( struct
Cyc_List_List*) _temp545.tl; goto _LL538;}} else{ goto _LL539;} _LL539: if((
unsigned int) _temp535 > 4u?(( struct _tunion_struct*) _temp535)->tag == Cyc_Absyn_Evar_tag:
0){ _LL556: _temp555=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp535)->f1;
_temp557=&(( struct Cyc_Absyn_Evar_struct*) _temp535)->f1; goto _LL553; _LL553:
_temp552=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp535)->f2;
_temp554=&(( struct Cyc_Absyn_Evar_struct*) _temp535)->f2; goto _LL551; _LL551:
_temp550=( int)(( struct Cyc_Absyn_Evar_struct*) _temp535)->f3; goto _LL540;}
else{ goto _LL541;} _LL541: goto _LL542; _LL538: { void* _temp558= Cyc_Tcutil_compress(
_temp548); int _temp564; struct Cyc_Core_Opt* _temp566; void* _temp568; _LL560:
if(( unsigned int) _temp558 > 4u?(( struct _tunion_struct*) _temp558)->tag ==
Cyc_Absyn_Evar_tag: 0){ _LL569: _temp568=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp558)->f1; goto _LL567; _LL567: _temp566=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp558)->f2; goto _LL565; _LL565: _temp564=( int)((
struct Cyc_Absyn_Evar_struct*) _temp558)->f3; goto _LL561;} else{ goto _LL562;}
_LL562: goto _LL563; _LL561: return({ struct _tuple8* _temp570=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp570->f1= _temp548; _temp570->f2=
_temp546; _temp570;}); _LL563: return 0; _LL559:;} _LL540: if(* _temp557 !=(
void*) Cyc_Absyn_EffKind){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp571=( char*)"effect evar has wrong kind";
struct _tagged_string _temp572; _temp572.curr= _temp571; _temp572.base= _temp571;
_temp572.last_plus_one= _temp571 + 27; _temp572;}));} return({ struct _tuple8*
_temp573=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp573->f1= t;
_temp573->f2= 0; _temp573;}); _LL542: return 0; _LL536:;} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp574= Cyc_Tcutil_compress( e); void*
_temp584; struct Cyc_List_List* _temp586; int _temp588; struct Cyc_Core_Opt*
_temp590; struct Cyc_Core_Opt** _temp592; void* _temp593; void** _temp595;
_LL576: if(( unsigned int) _temp574 > 4u?(( struct _tunion_struct*) _temp574)->tag
== Cyc_Absyn_AccessEff_tag: 0){ _LL585: _temp584=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp574)->f1; goto _LL577;} else{ goto _LL578;} _LL578: if(( unsigned int)
_temp574 > 4u?(( struct _tunion_struct*) _temp574)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL587: _temp586=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp574)->f1; goto _LL579;} else{ goto _LL580;} _LL580: if(( unsigned int)
_temp574 > 4u?(( struct _tunion_struct*) _temp574)->tag == Cyc_Absyn_Evar_tag: 0){
_LL594: _temp593=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp574)->f1;
_temp595=&(( struct Cyc_Absyn_Evar_struct*) _temp574)->f1; goto _LL591; _LL591:
_temp590=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp574)->f2;
_temp592=&(( struct Cyc_Absyn_Evar_struct*) _temp574)->f2; goto _LL589; _LL589:
_temp588=( int)(( struct Cyc_Absyn_Evar_struct*) _temp574)->f3; goto _LL581;}
else{ goto _LL582;} _LL582: goto _LL583; _LL577: if( constrain){ return Cyc_Tcutil_unify(
r, _temp584);} Cyc_Tcutil_compress( _temp584); if( r == _temp584){ return 1;}{
struct _tuple7 _temp597=({ struct _tuple7 _temp596; _temp596.f1= r; _temp596.f2=
_temp584; _temp596;}); void* _temp603; struct Cyc_Absyn_Tvar* _temp605; void*
_temp607; struct Cyc_Absyn_Tvar* _temp609; _LL599: _LL608: _temp607= _temp597.f1;
if(( unsigned int) _temp607 > 4u?(( struct _tunion_struct*) _temp607)->tag ==
Cyc_Absyn_VarType_tag: 0){ _LL610: _temp609=( struct Cyc_Absyn_Tvar*)(( struct
Cyc_Absyn_VarType_struct*) _temp607)->f1; goto _LL604;} else{ goto _LL601;}
_LL604: _temp603= _temp597.f2; if(( unsigned int) _temp603 > 4u?(( struct
_tunion_struct*) _temp603)->tag == Cyc_Absyn_VarType_tag: 0){ _LL606: _temp605=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp603)->f1; goto
_LL600;} else{ goto _LL601;} _LL601: goto _LL602; _LL600: return Cyc_Absyn_tvar_cmp(
_temp609, _temp605) == 0; _LL602: return 0; _LL598:;} _LL579: for( 0; _temp586
!= 0; _temp586=({ struct Cyc_List_List* _temp611= _temp586; if( _temp611 == 0){
_throw( Null_Exception);} _temp611->tl;})){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)({ struct Cyc_List_List* _temp612= _temp586; if( _temp612
== 0){ _throw( Null_Exception);} _temp612->hd;}))){ return 1;}} return 0; _LL581:
if(* _temp595 !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct _tagged_string
msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp613=( char*)"effect evar has wrong kind";
struct _tagged_string _temp614; _temp614.curr= _temp613; _temp614.base= _temp613;
_temp614.last_plus_one= _temp613 + 27; _temp614;}));} if( ! constrain){ return 0;}{
void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind); struct Cyc_Absyn_JoinEff_struct*
new_typ=({ struct Cyc_Absyn_JoinEff_struct* _temp616=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp616[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp617; _temp617.tag= Cyc_Absyn_JoinEff_tag; _temp617.f1=({ struct Cyc_List_List*
_temp618=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp618->hd=( void*) ev; _temp618->tl=({ struct Cyc_List_List* _temp619=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp619->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp620=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp620[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp621; _temp621.tag= Cyc_Absyn_AccessEff_tag;
_temp621.f1=( void*) r; _temp621;}); _temp620;})); _temp619->tl= 0; _temp619;});
_temp618;}); _temp617;}); _temp616;});* _temp592=({ struct Cyc_Core_Opt*
_temp615=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp615->v=( void*)(( void*) new_typ); _temp615;}); return 1;} _LL583: return 0;
_LL575:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct Cyc_Absyn_Tvar*
v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp622= e; struct Cyc_Absyn_Tvar*
_temp632; struct Cyc_List_List* _temp634; int _temp636; struct Cyc_Core_Opt*
_temp638; struct Cyc_Core_Opt** _temp640; void* _temp641; void** _temp643;
_LL624: if(( unsigned int) _temp622 > 4u?(( struct _tunion_struct*) _temp622)->tag
== Cyc_Absyn_VarType_tag: 0){ _LL633: _temp632=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp622)->f1; goto _LL625;} else{ goto _LL626;}
_LL626: if(( unsigned int) _temp622 > 4u?(( struct _tunion_struct*) _temp622)->tag
== Cyc_Absyn_JoinEff_tag: 0){ _LL635: _temp634=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp622)->f1; goto _LL627;} else{ goto _LL628;}
_LL628: if(( unsigned int) _temp622 > 4u?(( struct _tunion_struct*) _temp622)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL642: _temp641=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp622)->f1; _temp643=&(( struct Cyc_Absyn_Evar_struct*) _temp622)->f1; goto
_LL639; _LL639: _temp638=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp622)->f2; _temp640=&(( struct Cyc_Absyn_Evar_struct*) _temp622)->f2; goto
_LL637; _LL637: _temp636=( int)(( struct Cyc_Absyn_Evar_struct*) _temp622)->f3;
goto _LL629;} else{ goto _LL630;} _LL630: goto _LL631; _LL625: return Cyc_Absyn_tvar_cmp(
v, _temp632) == 0; _LL627: for( 0; _temp634 != 0; _temp634=({ struct Cyc_List_List*
_temp644= _temp634; if( _temp644 == 0){ _throw( Null_Exception);} _temp644->tl;})){
if( Cyc_Tcutil_variable_in_effect( constrain, v,( void*)({ struct Cyc_List_List*
_temp645= _temp634; if( _temp645 == 0){ _throw( Null_Exception);} _temp645->hd;}))){
return 1;}} return 0; _LL629: if(* _temp643 !=( void*) Cyc_Absyn_EffKind){((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp646=( char*)"effect evar has wrong kind"; struct _tagged_string
_temp647; _temp647.curr= _temp646; _temp647.base= _temp646; _temp647.last_plus_one=
_temp646 + 27; _temp647;}));}{ void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp649=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp649[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp650; _temp650.tag= Cyc_Absyn_JoinEff_tag;
_temp650.f1=({ struct Cyc_List_List* _temp651=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp651->hd=( void*) ev; _temp651->tl=({ struct
Cyc_List_List* _temp652=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp652->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp653=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp653[ 0]=({ struct Cyc_Absyn_VarType_struct _temp654; _temp654.tag= Cyc_Absyn_VarType_tag;
_temp654.f1= v; _temp654;}); _temp653;})); _temp652->tl= 0; _temp652;});
_temp651;}); _temp650;}); _temp649;});* _temp640=({ struct Cyc_Core_Opt*
_temp648=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp648->v=( void*)(( void*) new_typ); _temp648;}); return 1;} _LL631: return 0;
_LL623:;}} static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp655= e; struct Cyc_List_List* _temp663; int _temp665; struct Cyc_Core_Opt*
_temp667; void* _temp669; _LL657: if(( unsigned int) _temp655 > 4u?(( struct
_tunion_struct*) _temp655)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL664: _temp663=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp655)->f1; goto
_LL658;} else{ goto _LL659;} _LL659: if(( unsigned int) _temp655 > 4u?(( struct
_tunion_struct*) _temp655)->tag == Cyc_Absyn_Evar_tag: 0){ _LL670: _temp669=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp655)->f1; goto _LL668; _LL668:
_temp667=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp655)->f2;
goto _LL666; _LL666: _temp665=( int)(( struct Cyc_Absyn_Evar_struct*) _temp655)->f3;
goto _LL660;} else{ goto _LL661;} _LL661: goto _LL662; _LL658: for( 0; _temp663
!= 0; _temp663=({ struct Cyc_List_List* _temp671= _temp663; if( _temp671 == 0){
_throw( Null_Exception);} _temp671->tl;})){ if( Cyc_Tcutil_evar_in_effect( evar,(
void*)({ struct Cyc_List_List* _temp672= _temp663; if( _temp672 == 0){ _throw(
Null_Exception);} _temp672->hd;}))){ return 1;}} return 0; _LL660: return evar
== e; _LL662: return 0; _LL656:;}} int Cyc_Tcutil_subset_effect( int
set_to_empty, void* e1, void* e2){{ struct _tuple8* _temp673= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple8 _temp679; struct Cyc_List_List* _temp680; void* _temp682;
_LL675: if( _temp673 == 0){ goto _LL677;} else{ _temp679=* _temp673; _LL683:
_temp682= _temp679.f1; goto _LL681; _LL681: _temp680= _temp679.f2; goto _LL676;}
_LL677: goto _LL678; _LL676: { void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp684=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp684[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp685; _temp685.tag= Cyc_Absyn_JoinEff_tag;
_temp685.f1=({ struct Cyc_List_List* _temp686=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp686->hd=( void*) ev; _temp686->tl=({ struct
Cyc_List_List* _temp687=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp687->hd=( void*) e1; _temp687->tl= 0; _temp687;}); _temp686;}); _temp685;});
_temp684;}); return Cyc_Tcutil_unify( _temp682,( void*) new_typ);} _LL678: goto
_LL674; _LL674:;}{ void* _temp688= Cyc_Tcutil_compress( e1); struct Cyc_List_List*
_temp700; void* _temp702; struct Cyc_Absyn_Tvar* _temp704; int _temp706; struct
Cyc_Core_Opt* _temp708; struct Cyc_Core_Opt** _temp710; void* _temp711; _LL690:
if(( unsigned int) _temp688 > 4u?(( struct _tunion_struct*) _temp688)->tag ==
Cyc_Absyn_JoinEff_tag: 0){ _LL701: _temp700=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp688)->f1; goto _LL691;} else{ goto _LL692;}
_LL692: if(( unsigned int) _temp688 > 4u?(( struct _tunion_struct*) _temp688)->tag
== Cyc_Absyn_AccessEff_tag: 0){ _LL703: _temp702=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp688)->f1; goto _LL693;} else{ goto _LL694;} _LL694: if(( unsigned int)
_temp688 > 4u?(( struct _tunion_struct*) _temp688)->tag == Cyc_Absyn_VarType_tag:
0){ _LL705: _temp704=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp688)->f1; goto _LL695;} else{ goto _LL696;} _LL696: if(( unsigned int)
_temp688 > 4u?(( struct _tunion_struct*) _temp688)->tag == Cyc_Absyn_Evar_tag: 0){
_LL712: _temp711=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp688)->f1; goto
_LL709; _LL709: _temp708=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp688)->f2; _temp710=&(( struct Cyc_Absyn_Evar_struct*) _temp688)->f2; goto
_LL707; _LL707: _temp706=( int)(( struct Cyc_Absyn_Evar_struct*) _temp688)->f3;
goto _LL697;} else{ goto _LL698;} _LL698: goto _LL699; _LL691: for( 0; _temp700
!= 0; _temp700=({ struct Cyc_List_List* _temp713= _temp700; if( _temp713 == 0){
_throw( Null_Exception);} _temp713->tl;})){ if( ! Cyc_Tcutil_subset_effect(
set_to_empty,( void*)({ struct Cyc_List_List* _temp714= _temp700; if( _temp714
== 0){ _throw( Null_Exception);} _temp714->hd;}), e2)){ return 0;}} return 1;
_LL693: return( Cyc_Tcutil_region_in_effect( 0, _temp702, e2)? 1: Cyc_Tcutil_region_in_effect(
1, _temp702, e2))? 1: Cyc_Tcutil_unify( _temp702,( void*) Cyc_Absyn_HeapRgn);
_LL695: return Cyc_Tcutil_variable_in_effect( 0, _temp704, e2)? 1: Cyc_Tcutil_variable_in_effect(
1, _temp704, e2); _LL697: if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if(
set_to_empty){* _temp710=({ struct Cyc_Core_Opt* _temp715=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp715->v=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp716=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp716[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp717; _temp717.tag= Cyc_Absyn_JoinEff_tag; _temp717.f1= 0; _temp717;});
_temp716;})); _temp715;});} else{* _temp710=({ struct Cyc_Core_Opt* _temp718=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp718->v=(
void*) e2; _temp718;});}} return 1; _LL699: return(( int(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string _temp719= Cyc_Absynpp_typ2string(
e1); xprintf("subset_effect: bad effect: %.*s", _temp719.last_plus_one -
_temp719.curr, _temp719.curr);})); _LL689:;}} static int Cyc_Tcutil_unify_effect(
void* e1, void* e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2);
Cyc_Tcutil_normalize_effect( e1); Cyc_Tcutil_normalize_effect( e2);{ struct
_tuple9 _temp721=({ struct _tuple9 _temp720; _temp720.f1= Cyc_Tcutil_get_effect_evar(
e1); _temp720.f2= Cyc_Tcutil_get_effect_evar( e2); _temp720;}); struct _tuple8*
_temp727; struct _tuple8 _temp729; struct Cyc_List_List* _temp730; void*
_temp732; struct _tuple8* _temp734; struct _tuple8 _temp736; struct Cyc_List_List*
_temp737; void* _temp739; _LL723: _LL735: _temp734= _temp721.f1; if( _temp734 ==
0){ goto _LL725;} else{ _temp736=* _temp734; _LL740: _temp739= _temp736.f1; goto
_LL738; _LL738: _temp737= _temp736.f2; goto _LL728;} _LL728: _temp727= _temp721.f2;
if( _temp727 == 0){ goto _LL725;} else{ _temp729=* _temp727; _LL733: _temp732=
_temp729.f1; goto _LL731; _LL731: _temp730= _temp729.f2; goto _LL724;} _LL725:
goto _LL726; _LL724: { void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp741=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp741[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp742; _temp742.tag= Cyc_Absyn_JoinEff_tag;
_temp742.f1=({ struct Cyc_List_List* _temp743=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp743->hd=( void*) ev; _temp743->tl=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp737, _temp730); _temp743;}); _temp742;}); _temp741;}); return Cyc_Tcutil_unify(
_temp739,( void*) new_typ)? Cyc_Tcutil_unify( _temp732,( void*) new_typ): 0;}
_LL726: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0,
e2, e1): 0; _LL722:;}} void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp744= t1;
int _temp750; struct Cyc_Core_Opt* _temp752; struct Cyc_Core_Opt** _temp754;
void* _temp755; _LL746: if(( unsigned int) _temp744 > 4u?(( struct
_tunion_struct*) _temp744)->tag == Cyc_Absyn_Evar_tag: 0){ _LL756: _temp755=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp744)->f1; goto _LL753; _LL753:
_temp752=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp744)->f2;
_temp754=&(( struct Cyc_Absyn_Evar_struct*) _temp744)->f2; goto _LL751; _LL751:
_temp750=( int)(( struct Cyc_Absyn_Evar_struct*) _temp744)->f3; goto _LL747;}
else{ goto _LL748;} _LL748: goto _LL749; _LL747: Cyc_Tcutil_occurs( t1, t2);{
void* kind2= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq( kind2, _temp755)){*
_temp754=({ struct Cyc_Core_Opt* _temp757=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp757->v=( void*) t2; _temp757;}); return;}
else{{ void* _temp758= t2; int _temp766; struct Cyc_Core_Opt* _temp768; struct
Cyc_Core_Opt** _temp770; void* _temp771; struct Cyc_Absyn_PtrInfo _temp774;
_LL760: if(( unsigned int) _temp758 > 4u?(( struct _tunion_struct*) _temp758)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL772: _temp771=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp758)->f1; goto _LL769; _LL769: _temp768=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp758)->f2; _temp770=&(( struct Cyc_Absyn_Evar_struct*)
_temp758)->f2; goto _LL767; _LL767: _temp766=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp758)->f3; goto _LL761;} else{ goto _LL762;} _LL762: if(( unsigned int)
_temp758 > 4u?(( struct _tunion_struct*) _temp758)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL775: _temp774=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp758)->f1; goto _LL773;} else{ goto _LL764;} _LL773: if( _temp755 ==( void*)
Cyc_Absyn_BoxKind){ goto _LL763;} else{ goto _LL764;} _LL764: goto _LL765;
_LL761: if( Cyc_Tcutil_kind_leq( _temp755, kind2)){* _temp770=({ struct Cyc_Core_Opt*
_temp776=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp776->v=( void*) t1; _temp776;}); return;} goto _LL759; _LL763: { struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp774.bounds);{ void* _temp777=( void*) c->v; _LL779: if(( int) _temp777 ==
Cyc_Absyn_No_constr){ goto _LL780;} else{ goto _LL781;} _LL781: goto _LL782;
_LL780:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp783=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp783[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp784; _temp784.tag= Cyc_Absyn_Eq_constr_tag;
_temp784.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp785=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp785[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp786; _temp786.tag= Cyc_Absyn_Upper_b_tag;
_temp786.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp786;}); _temp785;}));
_temp784;}); _temp783;})));* _temp754=({ struct Cyc_Core_Opt* _temp787=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp787->v=( void*) t2;
_temp787;}); return; _LL782: goto _LL778; _LL778:;} goto _LL759;} _LL765: goto
_LL759; _LL759:;}( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Tcutil_Unify_struct*
_temp788=( struct Cyc_Tcutil_Unify_struct*) GC_malloc( sizeof( struct Cyc_Tcutil_Unify_struct));
_temp788[ 0]=({ struct Cyc_Tcutil_Unify_struct _temp789; _temp789.tag= Cyc_Tcutil_Unify_tag;
_temp789;}); _temp788;}));}} _LL749: goto _LL745; _LL745:;}{ struct _tuple7
_temp791=({ struct _tuple7 _temp790; _temp790.f1= t2; _temp790.f2= t1; _temp790;});
void* _temp841; void* _temp843; int _temp845; struct Cyc_Core_Opt* _temp847;
void* _temp849; void* _temp851; void* _temp853; void* _temp855; struct Cyc_Absyn_Tvar*
_temp857; void* _temp859; struct Cyc_Absyn_Tvar* _temp861; void* _temp863;
struct Cyc_Absyn_Structdecl** _temp865; struct Cyc_List_List* _temp867; struct
_tuple1* _temp869; void* _temp871; struct Cyc_Absyn_Structdecl** _temp873;
struct Cyc_List_List* _temp875; struct _tuple1* _temp877; void* _temp879; struct
Cyc_Absyn_Enumdecl* _temp881; struct _tuple1* _temp883; void* _temp885; struct
Cyc_Absyn_Enumdecl* _temp887; struct _tuple1* _temp889; void* _temp891; struct
Cyc_Absyn_Uniondecl** _temp893; struct Cyc_List_List* _temp895; struct _tuple1*
_temp897; void* _temp899; struct Cyc_Absyn_Uniondecl** _temp901; struct Cyc_List_List*
_temp903; struct _tuple1* _temp905; void* _temp907; struct Cyc_Absyn_TunionInfo
_temp909; struct Cyc_Absyn_Tuniondecl* _temp911; void* _temp913; struct Cyc_List_List*
_temp915; struct _tuple1* _temp917; void* _temp919; struct Cyc_Absyn_TunionInfo
_temp921; struct Cyc_Absyn_Tuniondecl* _temp923; void* _temp925; struct Cyc_List_List*
_temp927; struct _tuple1* _temp929; void* _temp931; struct Cyc_Absyn_XTunionInfo
_temp933; struct Cyc_Absyn_XTuniondecl* _temp935; void* _temp937; struct _tuple1*
_temp939; void* _temp941; struct Cyc_Absyn_XTunionInfo _temp943; struct Cyc_Absyn_XTuniondecl*
_temp945; void* _temp947; struct _tuple1* _temp949; void* _temp951; struct Cyc_Absyn_TunionFieldInfo
_temp953; struct Cyc_Absyn_Tunionfield* _temp955; struct Cyc_Absyn_Tuniondecl*
_temp957; struct _tuple1* _temp959; struct Cyc_List_List* _temp961; struct
_tuple1* _temp963; void* _temp965; struct Cyc_Absyn_TunionFieldInfo _temp967;
struct Cyc_Absyn_Tunionfield* _temp969; struct Cyc_Absyn_Tuniondecl* _temp971;
struct _tuple1* _temp973; struct Cyc_List_List* _temp975; struct _tuple1*
_temp977; void* _temp979; struct Cyc_Absyn_XTunionFieldInfo _temp981; struct Cyc_Absyn_Tunionfield*
_temp983; struct Cyc_Absyn_XTuniondecl* _temp985; struct _tuple1* _temp987;
struct _tuple1* _temp989; void* _temp991; struct Cyc_Absyn_XTunionFieldInfo
_temp993; struct Cyc_Absyn_Tunionfield* _temp995; struct Cyc_Absyn_XTuniondecl*
_temp997; struct _tuple1* _temp999; struct _tuple1* _temp1001; void* _temp1003;
struct Cyc_Absyn_PtrInfo _temp1005; struct Cyc_Absyn_Conref* _temp1007; struct
Cyc_Absyn_Tqual* _temp1009; struct Cyc_Absyn_Conref* _temp1011; void* _temp1013;
void* _temp1015; void* _temp1017; struct Cyc_Absyn_PtrInfo _temp1019; struct Cyc_Absyn_Conref*
_temp1021; struct Cyc_Absyn_Tqual* _temp1023; struct Cyc_Absyn_Conref* _temp1025;
void* _temp1027; void* _temp1029; void* _temp1031; void* _temp1033; void*
_temp1035; void* _temp1037; void* _temp1039; void* _temp1041; void* _temp1043;
void* _temp1045; void* _temp1047; void* _temp1049; void* _temp1051; struct Cyc_Absyn_Exp*
_temp1053; struct Cyc_Absyn_Tqual* _temp1055; void* _temp1057; void* _temp1059;
struct Cyc_Absyn_Exp* _temp1061; struct Cyc_Absyn_Tqual* _temp1063; void*
_temp1065; void* _temp1067; struct Cyc_Absyn_FnInfo _temp1069; struct Cyc_List_List*
_temp1071; int _temp1073; struct Cyc_List_List* _temp1075; void* _temp1077;
struct Cyc_Core_Opt* _temp1079; struct Cyc_List_List* _temp1081; void* _temp1083;
struct Cyc_Absyn_FnInfo _temp1085; struct Cyc_List_List* _temp1087; int
_temp1089; struct Cyc_List_List* _temp1091; void* _temp1093; struct Cyc_Core_Opt*
_temp1095; struct Cyc_List_List* _temp1097; void* _temp1099; struct Cyc_List_List*
_temp1101; void* _temp1103; struct Cyc_List_List* _temp1105; void* _temp1107;
void* _temp1109; void* _temp1111; void* _temp1113; void* _temp1115; void*
_temp1117; void* _temp1119; void* _temp1121; struct Cyc_List_List* _temp1123;
void* _temp1125; struct Cyc_List_List* _temp1127; void* _temp1129; void*
_temp1131; void* _temp1133; void* _temp1135; void* _temp1137; void* _temp1139;
void* _temp1141; _LL793: _LL844: _temp843= _temp791.f1; if(( unsigned int)
_temp843 > 4u?(( struct _tunion_struct*) _temp843)->tag == Cyc_Absyn_Evar_tag: 0){
_LL850: _temp849=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp843)->f1; goto
_LL848; _LL848: _temp847=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp843)->f2; goto _LL846; _LL846: _temp845=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp843)->f3; goto _LL842;} else{ goto _LL795;} _LL842: _temp841= _temp791.f2;
goto _LL794; _LL795: _LL854: _temp853= _temp791.f1; if(( int) _temp853 == Cyc_Absyn_VoidType){
goto _LL852;} else{ goto _LL797;} _LL852: _temp851= _temp791.f2; if(( int)
_temp851 == Cyc_Absyn_VoidType){ goto _LL796;} else{ goto _LL797;} _LL797:
_LL860: _temp859= _temp791.f1; if(( unsigned int) _temp859 > 4u?(( struct
_tunion_struct*) _temp859)->tag == Cyc_Absyn_VarType_tag: 0){ _LL862: _temp861=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp859)->f1; goto
_LL856;} else{ goto _LL799;} _LL856: _temp855= _temp791.f2; if(( unsigned int)
_temp855 > 4u?(( struct _tunion_struct*) _temp855)->tag == Cyc_Absyn_VarType_tag:
0){ _LL858: _temp857=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp855)->f1; goto _LL798;} else{ goto _LL799;} _LL799: _LL872: _temp871=
_temp791.f1; if(( unsigned int) _temp871 > 4u?(( struct _tunion_struct*)
_temp871)->tag == Cyc_Absyn_StructType_tag: 0){ _LL878: _temp877=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp871)->f1; goto _LL876;
_LL876: _temp875=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp871)->f2; goto _LL874; _LL874: _temp873=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp871)->f3; goto _LL864;} else{ goto
_LL801;} _LL864: _temp863= _temp791.f2; if(( unsigned int) _temp863 > 4u?((
struct _tunion_struct*) _temp863)->tag == Cyc_Absyn_StructType_tag: 0){ _LL870:
_temp869=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp863)->f1;
goto _LL868; _LL868: _temp867=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp863)->f2; goto _LL866; _LL866: _temp865=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp863)->f3; goto _LL800;} else{ goto
_LL801;} _LL801: _LL886: _temp885= _temp791.f1; if(( unsigned int) _temp885 > 4u?((
struct _tunion_struct*) _temp885)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL890:
_temp889=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp885)->f1;
goto _LL888; _LL888: _temp887=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp885)->f2; goto _LL880;} else{ goto _LL803;} _LL880: _temp879= _temp791.f2;
if(( unsigned int) _temp879 > 4u?(( struct _tunion_struct*) _temp879)->tag ==
Cyc_Absyn_EnumType_tag: 0){ _LL884: _temp883=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp879)->f1; goto _LL882; _LL882: _temp881=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp879)->f2; goto _LL802;} else{ goto
_LL803;} _LL803: _LL900: _temp899= _temp791.f1; if(( unsigned int) _temp899 > 4u?((
struct _tunion_struct*) _temp899)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL906:
_temp905=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp899)->f1;
goto _LL904; _LL904: _temp903=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp899)->f2; goto _LL902; _LL902: _temp901=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp899)->f3; goto _LL892;} else{ goto
_LL805;} _LL892: _temp891= _temp791.f2; if(( unsigned int) _temp891 > 4u?((
struct _tunion_struct*) _temp891)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL898:
_temp897=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp891)->f1;
goto _LL896; _LL896: _temp895=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp891)->f2; goto _LL894; _LL894: _temp893=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp891)->f3; goto _LL804;} else{ goto
_LL805;} _LL805: _LL920: _temp919= _temp791.f1; if(( unsigned int) _temp919 > 4u?((
struct _tunion_struct*) _temp919)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL922:
_temp921=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp919)->f1; _LL930: _temp929=( struct _tuple1*) _temp921.name; goto _LL928;
_LL928: _temp927=( struct Cyc_List_List*) _temp921.targs; goto _LL926; _LL926:
_temp925=( void*) _temp921.rgn; goto _LL924; _LL924: _temp923=( struct Cyc_Absyn_Tuniondecl*)
_temp921.tud; goto _LL908;} else{ goto _LL807;} _LL908: _temp907= _temp791.f2;
if(( unsigned int) _temp907 > 4u?(( struct _tunion_struct*) _temp907)->tag ==
Cyc_Absyn_TunionType_tag: 0){ _LL910: _temp909=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp907)->f1; _LL918: _temp917=( struct
_tuple1*) _temp909.name; goto _LL916; _LL916: _temp915=( struct Cyc_List_List*)
_temp909.targs; goto _LL914; _LL914: _temp913=( void*) _temp909.rgn; goto _LL912;
_LL912: _temp911=( struct Cyc_Absyn_Tuniondecl*) _temp909.tud; goto _LL806;}
else{ goto _LL807;} _LL807: _LL942: _temp941= _temp791.f1; if(( unsigned int)
_temp941 > 4u?(( struct _tunion_struct*) _temp941)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL944: _temp943=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp941)->f1; _LL950: _temp949=( struct _tuple1*) _temp943.name; goto _LL948;
_LL948: _temp947=( void*) _temp943.rgn; goto _LL946; _LL946: _temp945=( struct
Cyc_Absyn_XTuniondecl*) _temp943.xtud; goto _LL932;} else{ goto _LL809;} _LL932:
_temp931= _temp791.f2; if(( unsigned int) _temp931 > 4u?(( struct _tunion_struct*)
_temp931)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL934: _temp933=( struct Cyc_Absyn_XTunionInfo)((
struct Cyc_Absyn_XTunionType_struct*) _temp931)->f1; _LL940: _temp939=( struct
_tuple1*) _temp933.name; goto _LL938; _LL938: _temp937=( void*) _temp933.rgn;
goto _LL936; _LL936: _temp935=( struct Cyc_Absyn_XTuniondecl*) _temp933.xtud;
goto _LL808;} else{ goto _LL809;} _LL809: _LL966: _temp965= _temp791.f1; if((
unsigned int) _temp965 > 4u?(( struct _tunion_struct*) _temp965)->tag == Cyc_Absyn_TunionFieldType_tag:
0){ _LL968: _temp967=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp965)->f1; _LL978: _temp977=( struct _tuple1*) _temp967.name; goto _LL976;
_LL976: _temp975=( struct Cyc_List_List*) _temp967.targs; goto _LL974; _LL974:
_temp973=( struct _tuple1*) _temp967.fname; goto _LL972; _LL972: _temp971=(
struct Cyc_Absyn_Tuniondecl*) _temp967.tud; goto _LL970; _LL970: _temp969=(
struct Cyc_Absyn_Tunionfield*) _temp967.tufd; goto _LL952;} else{ goto _LL811;}
_LL952: _temp951= _temp791.f2; if(( unsigned int) _temp951 > 4u?(( struct
_tunion_struct*) _temp951)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL954:
_temp953=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp951)->f1; _LL964: _temp963=( struct _tuple1*) _temp953.name; goto _LL962;
_LL962: _temp961=( struct Cyc_List_List*) _temp953.targs; goto _LL960; _LL960:
_temp959=( struct _tuple1*) _temp953.fname; goto _LL958; _LL958: _temp957=(
struct Cyc_Absyn_Tuniondecl*) _temp953.tud; goto _LL956; _LL956: _temp955=(
struct Cyc_Absyn_Tunionfield*) _temp953.tufd; goto _LL810;} else{ goto _LL811;}
_LL811: _LL992: _temp991= _temp791.f1; if(( unsigned int) _temp991 > 4u?((
struct _tunion_struct*) _temp991)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){
_LL994: _temp993=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp991)->f1; _LL1002: _temp1001=( struct _tuple1*) _temp993.name; goto _LL1000;
_LL1000: _temp999=( struct _tuple1*) _temp993.fname; goto _LL998; _LL998:
_temp997=( struct Cyc_Absyn_XTuniondecl*) _temp993.xtud; goto _LL996; _LL996:
_temp995=( struct Cyc_Absyn_Tunionfield*) _temp993.xtufd; goto _LL980;} else{
goto _LL813;} _LL980: _temp979= _temp791.f2; if(( unsigned int) _temp979 > 4u?((
struct _tunion_struct*) _temp979)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){
_LL982: _temp981=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp979)->f1; _LL990: _temp989=( struct _tuple1*) _temp981.name; goto _LL988;
_LL988: _temp987=( struct _tuple1*) _temp981.fname; goto _LL986; _LL986:
_temp985=( struct Cyc_Absyn_XTuniondecl*) _temp981.xtud; goto _LL984; _LL984:
_temp983=( struct Cyc_Absyn_Tunionfield*) _temp981.xtufd; goto _LL812;} else{
goto _LL813;} _LL813: _LL1018: _temp1017= _temp791.f1; if(( unsigned int)
_temp1017 > 4u?(( struct _tunion_struct*) _temp1017)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1020: _temp1019=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1017)->f1; _LL1030: _temp1029=( void*) _temp1019.elt_typ; goto _LL1028;
_LL1028: _temp1027=( void*) _temp1019.rgn_typ; goto _LL1026; _LL1026: _temp1025=(
struct Cyc_Absyn_Conref*) _temp1019.nullable; goto _LL1024; _LL1024: _temp1023=(
struct Cyc_Absyn_Tqual*) _temp1019.tq; goto _LL1022; _LL1022: _temp1021=( struct
Cyc_Absyn_Conref*) _temp1019.bounds; goto _LL1004;} else{ goto _LL815;} _LL1004:
_temp1003= _temp791.f2; if(( unsigned int) _temp1003 > 4u?(( struct
_tunion_struct*) _temp1003)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1006:
_temp1005=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1003)->f1; _LL1016: _temp1015=( void*) _temp1005.elt_typ; goto _LL1014;
_LL1014: _temp1013=( void*) _temp1005.rgn_typ; goto _LL1012; _LL1012: _temp1011=(
struct Cyc_Absyn_Conref*) _temp1005.nullable; goto _LL1010; _LL1010: _temp1009=(
struct Cyc_Absyn_Tqual*) _temp1005.tq; goto _LL1008; _LL1008: _temp1007=( struct
Cyc_Absyn_Conref*) _temp1005.bounds; goto _LL814;} else{ goto _LL815;} _LL815:
_LL1038: _temp1037= _temp791.f1; if(( unsigned int) _temp1037 > 4u?(( struct
_tunion_struct*) _temp1037)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1042:
_temp1041=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1037)->f1; goto
_LL1040; _LL1040: _temp1039=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1037)->f2; goto _LL1032;} else{ goto _LL817;} _LL1032: _temp1031= _temp791.f2;
if(( unsigned int) _temp1031 > 4u?(( struct _tunion_struct*) _temp1031)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL1036: _temp1035=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1031)->f1; goto _LL1034; _LL1034: _temp1033=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1031)->f2; goto _LL816;} else{ goto _LL817;} _LL817: _LL1046: _temp1045=
_temp791.f1; if(( int) _temp1045 == Cyc_Absyn_FloatType){ goto _LL1044;} else{
goto _LL819;} _LL1044: _temp1043= _temp791.f2; if(( int) _temp1043 == Cyc_Absyn_FloatType){
goto _LL818;} else{ goto _LL819;} _LL819: _LL1050: _temp1049= _temp791.f1; if((
int) _temp1049 == Cyc_Absyn_DoubleType){ goto _LL1048;} else{ goto _LL821;}
_LL1048: _temp1047= _temp791.f2; if(( int) _temp1047 == Cyc_Absyn_DoubleType){
goto _LL820;} else{ goto _LL821;} _LL821: _LL1060: _temp1059= _temp791.f1; if((
unsigned int) _temp1059 > 4u?(( struct _tunion_struct*) _temp1059)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1066: _temp1065=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1059)->f1;
goto _LL1064; _LL1064: _temp1063=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1059)->f2; goto _LL1062; _LL1062: _temp1061=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1059)->f3; goto _LL1052;} else{ goto
_LL823;} _LL1052: _temp1051= _temp791.f2; if(( unsigned int) _temp1051 > 4u?((
struct _tunion_struct*) _temp1051)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1058:
_temp1057=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1051)->f1; goto
_LL1056; _LL1056: _temp1055=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1051)->f2; goto _LL1054; _LL1054: _temp1053=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1051)->f3; goto _LL822;} else{ goto
_LL823;} _LL823: _LL1084: _temp1083= _temp791.f1; if(( unsigned int) _temp1083 >
4u?(( struct _tunion_struct*) _temp1083)->tag == Cyc_Absyn_FnType_tag: 0){
_LL1086: _temp1085=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1083)->f1; _LL1098: _temp1097=( struct Cyc_List_List*) _temp1085.tvars;
goto _LL1096; _LL1096: _temp1095=( struct Cyc_Core_Opt*) _temp1085.effect; goto
_LL1094; _LL1094: _temp1093=( void*) _temp1085.ret_typ; goto _LL1092; _LL1092:
_temp1091=( struct Cyc_List_List*) _temp1085.args; goto _LL1090; _LL1090:
_temp1089=( int) _temp1085.varargs; goto _LL1088; _LL1088: _temp1087=( struct
Cyc_List_List*) _temp1085.attributes; goto _LL1068;} else{ goto _LL825;} _LL1068:
_temp1067= _temp791.f2; if(( unsigned int) _temp1067 > 4u?(( struct
_tunion_struct*) _temp1067)->tag == Cyc_Absyn_FnType_tag: 0){ _LL1070: _temp1069=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp1067)->f1;
_LL1082: _temp1081=( struct Cyc_List_List*) _temp1069.tvars; goto _LL1080;
_LL1080: _temp1079=( struct Cyc_Core_Opt*) _temp1069.effect; goto _LL1078;
_LL1078: _temp1077=( void*) _temp1069.ret_typ; goto _LL1076; _LL1076: _temp1075=(
struct Cyc_List_List*) _temp1069.args; goto _LL1074; _LL1074: _temp1073=( int)
_temp1069.varargs; goto _LL1072; _LL1072: _temp1071=( struct Cyc_List_List*)
_temp1069.attributes; goto _LL824;} else{ goto _LL825;} _LL825: _LL1104:
_temp1103= _temp791.f1; if(( unsigned int) _temp1103 > 4u?(( struct
_tunion_struct*) _temp1103)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL1106:
_temp1105=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1103)->f1; goto _LL1100;} else{ goto _LL827;} _LL1100: _temp1099= _temp791.f2;
if(( unsigned int) _temp1099 > 4u?(( struct _tunion_struct*) _temp1099)->tag ==
Cyc_Absyn_TupleType_tag: 0){ _LL1102: _temp1101=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1099)->f1; goto _LL826;} else{ goto
_LL827;} _LL827: _LL1110: _temp1109= _temp791.f1; if(( int) _temp1109 == Cyc_Absyn_HeapRgn){
goto _LL1108;} else{ goto _LL829;} _LL1108: _temp1107= _temp791.f2; if(( int)
_temp1107 == Cyc_Absyn_HeapRgn){ goto _LL828;} else{ goto _LL829;} _LL829:
_LL1116: _temp1115= _temp791.f1; if(( unsigned int) _temp1115 > 4u?(( struct
_tunion_struct*) _temp1115)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL1118:
_temp1117=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1115)->f1;
goto _LL1112;} else{ goto _LL831;} _LL1112: _temp1111= _temp791.f2; if((
unsigned int) _temp1111 > 4u?(( struct _tunion_struct*) _temp1111)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL1114: _temp1113=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1111)->f1; goto _LL830;} else{ goto _LL831;} _LL831: _LL1122: _temp1121=
_temp791.f1; if(( unsigned int) _temp1121 > 4u?(( struct _tunion_struct*)
_temp1121)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL1124: _temp1123=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp1121)->f1; goto _LL1120;} else{ goto
_LL833;} _LL1120: _temp1119= _temp791.f2; goto _LL832; _LL833: _LL1130:
_temp1129= _temp791.f1; goto _LL1126; _LL1126: _temp1125= _temp791.f2; if((
unsigned int) _temp1125 > 4u?(( struct _tunion_struct*) _temp1125)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL1128: _temp1127=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp1125)->f1; goto _LL834;} else{ goto _LL835;} _LL835: _LL1134: _temp1133=
_temp791.f1; if(( unsigned int) _temp1133 > 4u?(( struct _tunion_struct*)
_temp1133)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL1136: _temp1135=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp1133)->f1; goto _LL1132;} else{ goto
_LL837;} _LL1132: _temp1131= _temp791.f2; goto _LL836; _LL837: _LL1142:
_temp1141= _temp791.f1; goto _LL1138; _LL1138: _temp1137= _temp791.f2; if((
unsigned int) _temp1137 > 4u?(( struct _tunion_struct*) _temp1137)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL1140: _temp1139=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1137)->f1;
goto _LL838;} else{ goto _LL839;} _LL839: goto _LL840; _LL794: Cyc_Tcutil_unify_it(
t2, t1); return; _LL796: return; _LL798: { struct _tagged_string* x2= _temp861->name;
struct _tagged_string* x1= _temp857->name; void* k2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp861->kind); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp857->kind); if( Cyc_String_zstrptrcmp( x2, x1)
== 0){ if( k2 != k1){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp1143=* x2; struct _tagged_string _temp1144= Cyc_Absynpp_kind2string(
k2); struct _tagged_string _temp1145= Cyc_Absynpp_kind2string( k1); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp1143.last_plus_one - _temp1143.curr, _temp1143.curr, _temp1144.last_plus_one
- _temp1144.curr, _temp1144.curr, _temp1145.last_plus_one - _temp1145.curr,
_temp1145.curr);}));} return;} goto _LL792;} _LL800: if((( _temp869 != 0?
_temp877 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*)({ struct _tuple1*
_temp1146= _temp869; if( _temp1146 == 0){ _throw( Null_Exception);} _temp1146;}),(
struct _tuple1*)({ struct _tuple1* _temp1147= _temp877; if( _temp1147 == 0){
_throw( Null_Exception);} _temp1147;})) == 0: 0)? 1:( _temp869 == 0? _temp877 ==
0: 0)){ Cyc_Tcutil_unify_list( _temp867, _temp875); return;} goto _LL792; _LL802:
if( Cyc_Absyn_qvar_cmp( _temp889, _temp883) == 0){ return;} goto _LL792; _LL804:
if((( _temp897 != 0? _temp905 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*)({
struct _tuple1* _temp1148= _temp897; if( _temp1148 == 0){ _throw( Null_Exception);}
_temp1148;}),( struct _tuple1*)({ struct _tuple1* _temp1149= _temp905; if(
_temp1149 == 0){ _throw( Null_Exception);} _temp1149;})) == 0: 0)? 1:( _temp897
== 0? _temp905 == 0: 0)){ Cyc_Tcutil_unify_list( _temp895, _temp903); return;}
goto _LL792; _LL806: if( Cyc_Absyn_qvar_cmp( _temp917, _temp929) == 0){ Cyc_Tcutil_unify_it(
_temp913, _temp925); Cyc_Tcutil_unify_list( _temp915, _temp927); return;} goto
_LL792; _LL808: if( Cyc_Absyn_qvar_cmp( _temp939, _temp949) == 0){ Cyc_Tcutil_unify_it(
_temp937, _temp947); return;} goto _LL792; _LL810: if( Cyc_Absyn_qvar_cmp(
_temp963, _temp977) == 0? Cyc_Absyn_qvar_cmp( _temp959, _temp973) == 0: 0){ Cyc_Tcutil_unify_list(
_temp961, _temp975); return;} goto _LL792; _LL812: if( Cyc_Absyn_qvar_cmp(
_temp989, _temp1001) == 0? Cyc_Absyn_qvar_cmp( _temp987, _temp999) == 0: 0){
return;} goto _LL792; _LL814: Cyc_Tcutil_unify_it( _temp1015, _temp1029); Cyc_Tcutil_unify_it(
_temp1027, _temp1013); Cyc_Tcutil_unify_tqual( _temp1009, _temp1023);(( void(*)(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp, _temp1007, _temp1021);{
void* _temp1150=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1007))->v; void* _temp1156; _LL1152: if((
unsigned int) _temp1150 > 1u?(( struct _tunion_struct*) _temp1150)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL1157: _temp1156=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1150)->f1;
if(( int) _temp1156 == Cyc_Absyn_Unknown_b){ goto _LL1153;} else{ goto _LL1154;}}
else{ goto _LL1154;} _LL1154: goto _LL1155; _LL1153: return; _LL1155: goto
_LL1151; _LL1151:;}(( void(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Core_intcmp,
_temp1011, _temp1025); return; _LL816: if( _temp1035 == _temp1041? _temp1033 ==
_temp1039: 0){ return;} goto _LL792; _LL818: return; _LL820: return; _LL822: Cyc_Tcutil_unify_it(
_temp1057, _temp1065); Cyc_Tcutil_unify_tqual( _temp1055, _temp1063); if(
_temp1061 == _temp1053){ return;} if( _temp1061 == 0? 1: _temp1053 == 0){ goto
_LL792;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1158= _temp1061; if( _temp1158 == 0){ _throw( Null_Exception);} _temp1158;}))
== Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1159= _temp1053; if( _temp1159 == 0){ _throw( Null_Exception);} _temp1159;}))){
return;} goto _LL792; _LL824: { struct Cyc_List_List* inst= 0; while( _temp1081
!= 0) { if( _temp1097 == 0){ break;} inst=({ struct Cyc_List_List* _temp1160=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1160->hd=(
void*)({ struct _tuple5* _temp1161=( struct _tuple5*) GC_malloc( sizeof( struct
_tuple5)); _temp1161->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp1165= _temp1097; if( _temp1165 == 0){ _throw( Null_Exception);} _temp1165->hd;});
_temp1161->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp1162=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1162[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1163; _temp1163.tag= Cyc_Absyn_VarType_tag;
_temp1163.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1164=
_temp1081; if( _temp1164 == 0){ _throw( Null_Exception);} _temp1164->hd;});
_temp1163;}); _temp1162;}); _temp1161;}); _temp1160->tl= inst; _temp1160;});
_temp1081=({ struct Cyc_List_List* _temp1166= _temp1081; if( _temp1166 == 0){
_throw( Null_Exception);} _temp1166->tl;}); _temp1097=({ struct Cyc_List_List*
_temp1167= _temp1097; if( _temp1167 == 0){ _throw( Null_Exception);} _temp1167->tl;});}
if( _temp1097 != 0){ goto _LL792;} if( inst != 0){ Cyc_Tcutil_unify_it(( void*)({
struct Cyc_Absyn_FnType_struct* _temp1168=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1168[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1169; _temp1169.tag= Cyc_Absyn_FnType_tag; _temp1169.f1=({ struct Cyc_Absyn_FnInfo
_temp1170; _temp1170.tvars= 0; _temp1170.effect= _temp1079; _temp1170.ret_typ=(
void*) _temp1077; _temp1170.args= _temp1075; _temp1170.varargs= _temp1073;
_temp1170.attributes= _temp1071; _temp1170;}); _temp1169;}); _temp1168;}), Cyc_Tcutil_substitute(
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1171=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1171[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1172; _temp1172.tag= Cyc_Absyn_FnType_tag; _temp1172.f1=({ struct Cyc_Absyn_FnInfo
_temp1173; _temp1173.tvars= 0; _temp1173.effect= _temp1095; _temp1173.ret_typ=(
void*) _temp1093; _temp1173.args= _temp1091; _temp1173.varargs= _temp1089;
_temp1173.attributes= _temp1087; _temp1173;}); _temp1172;}); _temp1171;})));
return;} Cyc_Tcutil_unify_it( _temp1077, _temp1093); for( 0; _temp1075 != 0?
_temp1091 != 0: 0; _temp1075=({ struct Cyc_List_List* _temp1174= _temp1075; if(
_temp1174 == 0){ _throw( Null_Exception);} _temp1174->tl;}), _temp1091=({ struct
Cyc_List_List* _temp1175= _temp1091; if( _temp1175 == 0){ _throw( Null_Exception);}
_temp1175->tl;})){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)({ struct Cyc_List_List*
_temp1176= _temp1075; if( _temp1176 == 0){ _throw( Null_Exception);} _temp1176->hd;}))).f2,(*((
struct _tuple2*)({ struct Cyc_List_List* _temp1177= _temp1091; if( _temp1177 ==
0){ _throw( Null_Exception);} _temp1177->hd;}))).f2); Cyc_Tcutil_unify_it((*((
struct _tuple2*)({ struct Cyc_List_List* _temp1178= _temp1075; if( _temp1178 ==
0){ _throw( Null_Exception);} _temp1178->hd;}))).f3,(*(( struct _tuple2*)({
struct Cyc_List_List* _temp1179= _temp1091; if( _temp1179 == 0){ _throw(
Null_Exception);} _temp1179->hd;}))).f3);} if( _temp1075 != 0? 1: _temp1091 != 0){
goto _LL792;} if( _temp1073 != _temp1089){ goto _LL792;}{ int bad_effect= 0;{
struct _tuple10 _temp1181=({ struct _tuple10 _temp1180; _temp1180.f1= _temp1079;
_temp1180.f2= _temp1095; _temp1180;}); struct Cyc_Core_Opt* _temp1191; struct
Cyc_Core_Opt* _temp1193; struct Cyc_Core_Opt* _temp1195; struct Cyc_Core_Opt*
_temp1197; struct Cyc_Core_Opt* _temp1199; struct Cyc_Core_Opt* _temp1201;
_LL1183: _LL1194: _temp1193= _temp1181.f1; if( _temp1193 == 0){ goto _LL1192;}
else{ goto _LL1185;} _LL1192: _temp1191= _temp1181.f2; if( _temp1191 == 0){ goto
_LL1184;} else{ goto _LL1185;} _LL1185: _LL1198: _temp1197= _temp1181.f1; if(
_temp1197 == 0){ goto _LL1196;} else{ goto _LL1187;} _LL1196: _temp1195=
_temp1181.f2; goto _LL1186; _LL1187: _LL1202: _temp1201= _temp1181.f1; goto
_LL1200; _LL1200: _temp1199= _temp1181.f2; if( _temp1199 == 0){ goto _LL1188;}
else{ goto _LL1189;} _LL1189: goto _LL1190; _LL1184: goto _LL1182; _LL1186: goto
_LL1188; _LL1188: bad_effect= 1; goto _LL1182; _LL1190: bad_effect= ! Cyc_Tcutil_unify_effect((
void*)({ struct Cyc_Core_Opt* _temp1203= _temp1079; if( _temp1203 == 0){ _throw(
Null_Exception);} _temp1203->v;}),( void*)({ struct Cyc_Core_Opt* _temp1204=
_temp1095; if( _temp1204 == 0){ _throw( Null_Exception);} _temp1204->v;})); goto
_LL1182; _LL1182:;} if( bad_effect){ goto _LL792;} if( ! Cyc_Tcutil_same_atts(
_temp1087, _temp1071)){ goto _LL792;} return;}} _LL826: for( 0; _temp1101 != 0?
_temp1105 != 0: 0; _temp1101=({ struct Cyc_List_List* _temp1205= _temp1101; if(
_temp1205 == 0){ _throw( Null_Exception);} _temp1205->tl;}), _temp1105=({ struct
Cyc_List_List* _temp1206= _temp1105; if( _temp1206 == 0){ _throw( Null_Exception);}
_temp1206->tl;})){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)({ struct Cyc_List_List*
_temp1207= _temp1101; if( _temp1207 == 0){ _throw( Null_Exception);} _temp1207->hd;}))).f1,(*((
struct _tuple4*)({ struct Cyc_List_List* _temp1208= _temp1105; if( _temp1208 ==
0){ _throw( Null_Exception);} _temp1208->hd;}))).f1); Cyc_Tcutil_unify_it((*((
struct _tuple4*)({ struct Cyc_List_List* _temp1209= _temp1101; if( _temp1209 ==
0){ _throw( Null_Exception);} _temp1209->hd;}))).f2,(*(( struct _tuple4*)({
struct Cyc_List_List* _temp1210= _temp1105; if( _temp1210 == 0){ _throw(
Null_Exception);} _temp1210->hd;}))).f2);} if( _temp1101 == 0? _temp1105 == 0: 0){
return;} goto _LL792; _LL828: return; _LL830: Cyc_Tcutil_unify_it( _temp1117,
_temp1113); return; _LL832: goto _LL834; _LL834: goto _LL836; _LL836: goto
_LL838; _LL838: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL792;} _LL840: goto _LL792; _LL792:;}( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Tcutil_Unify_struct* _temp1211=( struct Cyc_Tcutil_Unify_struct*)
GC_malloc( sizeof( struct Cyc_Tcutil_Unify_struct)); _temp1211[ 0]=({ struct Cyc_Tcutil_Unify_struct
_temp1212; _temp1212.tag= Cyc_Tcutil_Unify_tag; _temp1212;}); _temp1211;}));}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1213= Cyc_Tcutil_compress(
t); void* _temp1225; void* _temp1227; struct Cyc_Absyn_Enumdecl* _temp1229;
struct _tuple1* _temp1231; _LL1215: if(( unsigned int) _temp1213 > 4u?(( struct
_tunion_struct*) _temp1213)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1228:
_temp1227=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1213)->f1; goto
_LL1226; _LL1226: _temp1225=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1213)->f2; goto _LL1216;} else{ goto _LL1217;} _LL1217: if(( int) _temp1213
== Cyc_Absyn_FloatType){ goto _LL1218;} else{ goto _LL1219;} _LL1219: if(( int)
_temp1213 == Cyc_Absyn_DoubleType){ goto _LL1220;} else{ goto _LL1221;} _LL1221:
if(( unsigned int) _temp1213 > 4u?(( struct _tunion_struct*) _temp1213)->tag ==
Cyc_Absyn_EnumType_tag: 0){ _LL1232: _temp1231=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1213)->f1; goto _LL1230; _LL1230: _temp1229=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1213)->f2; goto _LL1222;} else{ goto
_LL1223;} _LL1223: goto _LL1224; _LL1216: return 1; _LL1218: return 1; _LL1220:
return 1; _LL1222: return 1; _LL1224: return 0; _LL1214:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp1234=({ struct _tuple7 _temp1233; _temp1233.f1= t1;
_temp1233.f2= t2; _temp1233;}); void* _temp1256; void* _temp1258; void*
_temp1260; void* _temp1262; void* _temp1264; void* _temp1266; void* _temp1268;
void* _temp1270; void* _temp1272; void* _temp1274; void* _temp1276; void*
_temp1278; void* _temp1280; void* _temp1282; void* _temp1284; void* _temp1286;
void* _temp1288; void* _temp1290; void* _temp1292; void* _temp1294; void*
_temp1296; void* _temp1298; void* _temp1300; void* _temp1302; void* _temp1304;
void* _temp1306; void* _temp1308; void* _temp1310; void* _temp1312; void*
_temp1314; void* _temp1316; void* _temp1318; void* _temp1320; void* _temp1322;
void* _temp1324; void* _temp1326; void* _temp1328; void* _temp1330; void*
_temp1332; void* _temp1334; void* _temp1336; void* _temp1338; _LL1236: _LL1259:
_temp1258= _temp1234.f1; if(( int) _temp1258 == Cyc_Absyn_DoubleType){ goto
_LL1257;} else{ goto _LL1238;} _LL1257: _temp1256= _temp1234.f2; if(( int)
_temp1256 == Cyc_Absyn_FloatType){ goto _LL1237;} else{ goto _LL1238;} _LL1238:
_LL1267: _temp1266= _temp1234.f1; if(( int) _temp1266 == Cyc_Absyn_DoubleType){
goto _LL1261;} else{ goto _LL1240;} _LL1261: _temp1260= _temp1234.f2; if((
unsigned int) _temp1260 > 4u?(( struct _tunion_struct*) _temp1260)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1265: _temp1264=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1260)->f1;
goto _LL1263; _LL1263: _temp1262=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1260)->f2; goto _LL1239;} else{ goto _LL1240;} _LL1240: _LL1275: _temp1274=
_temp1234.f1; if(( int) _temp1274 == Cyc_Absyn_FloatType){ goto _LL1269;} else{
goto _LL1242;} _LL1269: _temp1268= _temp1234.f2; if(( unsigned int) _temp1268 >
4u?(( struct _tunion_struct*) _temp1268)->tag == Cyc_Absyn_IntType_tag: 0){
_LL1273: _temp1272=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1268)->f1;
goto _LL1271; _LL1271: _temp1270=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1268)->f2; goto _LL1241;} else{ goto _LL1242;} _LL1242: _LL1283: _temp1282=
_temp1234.f1; if(( unsigned int) _temp1282 > 4u?(( struct _tunion_struct*)
_temp1282)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1287: _temp1286=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1282)->f1; goto _LL1285; _LL1285:
_temp1284=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1282)->f2; if(( int)
_temp1284 == Cyc_Absyn_B8){ goto _LL1277;} else{ goto _LL1244;}} else{ goto
_LL1244;} _LL1277: _temp1276= _temp1234.f2; if(( unsigned int) _temp1276 > 4u?((
struct _tunion_struct*) _temp1276)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1281:
_temp1280=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1276)->f1; goto
_LL1279; _LL1279: _temp1278=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1276)->f2; if(( int) _temp1278 == Cyc_Absyn_B8){ goto _LL1243;} else{ goto
_LL1244;}} else{ goto _LL1244;} _LL1244: _LL1291: _temp1290= _temp1234.f1; if((
unsigned int) _temp1290 > 4u?(( struct _tunion_struct*) _temp1290)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1295: _temp1294=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1290)->f1;
goto _LL1293; _LL1293: _temp1292=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1290)->f2; if(( int) _temp1292 == Cyc_Absyn_B8){ goto _LL1289;} else{ goto
_LL1246;}} else{ goto _LL1246;} _LL1289: _temp1288= _temp1234.f2; goto _LL1245;
_LL1246: _LL1299: _temp1298= _temp1234.f1; if(( unsigned int) _temp1298 > 4u?((
struct _tunion_struct*) _temp1298)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1303:
_temp1302=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1298)->f1; goto
_LL1301; _LL1301: _temp1300=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1298)->f2; if(( int) _temp1300 == Cyc_Absyn_B4){ goto _LL1297;} else{ goto
_LL1248;}} else{ goto _LL1248;} _LL1297: _temp1296= _temp1234.f2; if(( int)
_temp1296 == Cyc_Absyn_FloatType){ goto _LL1247;} else{ goto _LL1248;} _LL1248:
_LL1311: _temp1310= _temp1234.f1; if(( unsigned int) _temp1310 > 4u?(( struct
_tunion_struct*) _temp1310)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1315:
_temp1314=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1310)->f1; goto
_LL1313; _LL1313: _temp1312=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1310)->f2; if(( int) _temp1312 == Cyc_Absyn_B4){ goto _LL1305;} else{ goto
_LL1250;}} else{ goto _LL1250;} _LL1305: _temp1304= _temp1234.f2; if((
unsigned int) _temp1304 > 4u?(( struct _tunion_struct*) _temp1304)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1309: _temp1308=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1304)->f1;
goto _LL1307; _LL1307: _temp1306=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1304)->f2; if(( int) _temp1306 == Cyc_Absyn_B2){ goto _LL1249;} else{ goto
_LL1250;}} else{ goto _LL1250;} _LL1250: _LL1323: _temp1322= _temp1234.f1; if((
unsigned int) _temp1322 > 4u?(( struct _tunion_struct*) _temp1322)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1327: _temp1326=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1322)->f1;
goto _LL1325; _LL1325: _temp1324=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1322)->f2; if(( int) _temp1324 == Cyc_Absyn_B4){ goto _LL1317;} else{ goto
_LL1252;}} else{ goto _LL1252;} _LL1317: _temp1316= _temp1234.f2; if((
unsigned int) _temp1316 > 4u?(( struct _tunion_struct*) _temp1316)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1321: _temp1320=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1316)->f1;
goto _LL1319; _LL1319: _temp1318=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1316)->f2; if(( int) _temp1318 == Cyc_Absyn_B1){ goto _LL1251;} else{ goto
_LL1252;}} else{ goto _LL1252;} _LL1252: _LL1335: _temp1334= _temp1234.f1; if((
unsigned int) _temp1334 > 4u?(( struct _tunion_struct*) _temp1334)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1339: _temp1338=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1334)->f1;
goto _LL1337; _LL1337: _temp1336=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1334)->f2; if(( int) _temp1336 == Cyc_Absyn_B2){ goto _LL1329;} else{ goto
_LL1254;}} else{ goto _LL1254;} _LL1329: _temp1328= _temp1234.f2; if((
unsigned int) _temp1328 > 4u?(( struct _tunion_struct*) _temp1328)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1333: _temp1332=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1328)->f1;
goto _LL1331; _LL1331: _temp1330=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1328)->f2; if(( int) _temp1330 == Cyc_Absyn_B1){ goto _LL1253;} else{ goto
_LL1254;}} else{ goto _LL1254;} _LL1254: goto _LL1255; _LL1237: return 1;
_LL1239: return 1; _LL1241: return 1; _LL1243: return 0; _LL1245: return 1;
_LL1247: return 1; _LL1249: return 1; _LL1251: return 1; _LL1253: return 1;
_LL1255: return 0; _LL1235:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=({ struct Cyc_List_List*
_temp1340= el; if( _temp1340 == 0){ _throw( Null_Exception);} _temp1340->tl;})){
void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1344=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1343= el; if( _temp1343 == 0){
_throw( Null_Exception);} _temp1343->hd;}))->topt; if( _temp1344 == 0){ _throw(
Null_Exception);} _temp1344->v;})); if( Cyc_Tcutil_is_arithmetic_type( t1)){ if(
max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision( t1,( void*)({ struct Cyc_Core_Opt*
_temp1341= max_arith_type; if( _temp1341 == 0){ _throw( Null_Exception);}
_temp1341->v;}))){ max_arith_type=({ struct Cyc_Core_Opt* _temp1342=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1342->v=( void*) t1; _temp1342;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)({ struct Cyc_Core_Opt*
_temp1345= max_arith_type; if( _temp1345 == 0){ _throw( Null_Exception);}
_temp1345->v;}))){ return 0;}}{ struct Cyc_List_List* el= es; for( 0; el != 0;
el=({ struct Cyc_List_List* _temp1346= el; if( _temp1346 == 0){ _throw(
Null_Exception);} _temp1346->tl;})){ if( ! Cyc_Tcutil_coerce_assign( te,( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1347= el; if( _temp1347 == 0){
_throw( Null_Exception);} _temp1347->hd;}), t)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1348= el; if( _temp1348 == 0){ _throw( Null_Exception);}
_temp1348->hd;}))->loc,({ struct _tagged_string _temp1351= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp1352= Cyc_Absynpp_typ2string(( void*)({ struct
Cyc_Core_Opt* _temp1350=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1349= el; if( _temp1349 == 0){ _throw( Null_Exception);} _temp1349->hd;}))->topt;
if( _temp1350 == 0){ _throw( Null_Exception);} _temp1350->v;})); xprintf("type mismatch: expecting %.*s but found %.*s",
_temp1351.last_plus_one - _temp1351.curr, _temp1351.curr, _temp1352.last_plus_one
- _temp1352.curr, _temp1352.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1354= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1353= e->topt; if( _temp1353 == 0){ _throw( Null_Exception);} _temp1353->v;}));
struct Cyc_Absyn_PtrInfo _temp1360; _LL1356: if(( unsigned int) _temp1354 > 4u?((
struct _tunion_struct*) _temp1354)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1361: _temp1360=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1354)->f1; goto _LL1357;} else{ goto _LL1358;} _LL1358: goto _LL1359;
_LL1357: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_uint_t); goto _LL1355;
_LL1359: return 0; _LL1355:;} return 1;} int Cyc_Tcutil_is_integral_type( void*
t){ void* _temp1362= Cyc_Tcutil_compress( t); void* _temp1370; void* _temp1372;
struct Cyc_Absyn_Enumdecl* _temp1374; struct _tuple1* _temp1376; _LL1364: if((
unsigned int) _temp1362 > 4u?(( struct _tunion_struct*) _temp1362)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1373: _temp1372=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1362)->f1;
goto _LL1371; _LL1371: _temp1370=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1362)->f2; goto _LL1365;} else{ goto _LL1366;} _LL1366: if(( unsigned int)
_temp1362 > 4u?(( struct _tunion_struct*) _temp1362)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1377: _temp1376=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1362)->f1; goto _LL1375; _LL1375: _temp1374=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1362)->f2; goto _LL1367;} else{ goto
_LL1368;} _LL1368: goto _LL1369; _LL1365: return 1; _LL1367: return 1; _LL1369:
return 0; _LL1363:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt*
_temp1378= e->topt; if( _temp1378 == 0){ _throw( Null_Exception);} _temp1378->v;}),
Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type(( void*)({ struct
Cyc_Core_Opt* _temp1379= e->topt; if( _temp1379 == 0){ _throw( Null_Exception);}
_temp1379->v;}))){ if( Cyc_Tcutil_will_lose_precision(( void*)({ struct Cyc_Core_Opt*
_temp1380= e->topt; if( _temp1380 == 0){ _throw( Null_Exception);} _temp1380->v;}),
Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ char*
_temp1381=( char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1382; _temp1382.curr= _temp1381; _temp1382.base= _temp1381;
_temp1382.last_plus_one= _temp1381 + 44; _temp1382;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)({
struct Cyc_Core_Opt* _temp1383= e->topt; if( _temp1383 == 0){ _throw(
Null_Exception);} _temp1383->v;}), Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)({ struct Cyc_Core_Opt* _temp1384= e->topt; if( _temp1384 == 0){ _throw(
Null_Exception);} _temp1384->v;}))){ if( Cyc_Tcutil_will_lose_precision(( void*)({
struct Cyc_Core_Opt* _temp1385= e->topt; if( _temp1385 == 0){ _throw(
Null_Exception);} _temp1385->v;}), Cyc_Absyn_sint_t)){ Cyc_Tcutil_warn( e->loc,(
struct _tagged_string)({ char* _temp1386=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1387; _temp1387.curr= _temp1386; _temp1387.base=
_temp1386; _temp1387.last_plus_one= _temp1386 + 44; _temp1387;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7
_temp1389=({ struct _tuple7 _temp1388; _temp1388.f1= t1; _temp1388.f2= t2;
_temp1388;}); void* _temp1403; struct Cyc_Absyn_PtrInfo _temp1405; void*
_temp1407; struct Cyc_Absyn_PtrInfo _temp1409; void* _temp1411; struct Cyc_Absyn_Exp*
_temp1413; struct Cyc_Absyn_Tqual* _temp1415; void* _temp1417; void* _temp1419;
struct Cyc_Absyn_Exp* _temp1421; struct Cyc_Absyn_Tqual* _temp1423; void*
_temp1425; void* _temp1427; struct Cyc_Absyn_TunionInfo _temp1429; struct Cyc_Absyn_Tuniondecl*
_temp1431; void* _temp1433; struct Cyc_List_List* _temp1435; struct _tuple1*
_temp1437; void* _temp1439; struct Cyc_Absyn_TunionFieldInfo _temp1441; struct
Cyc_Absyn_Tunionfield* _temp1443; struct Cyc_Absyn_Tuniondecl* _temp1445; struct
_tuple1* _temp1447; struct Cyc_List_List* _temp1449; struct _tuple1* _temp1451;
void* _temp1453; struct Cyc_Absyn_TunionInfo _temp1455; struct Cyc_Absyn_Tuniondecl*
_temp1457; void* _temp1459; struct Cyc_List_List* _temp1461; struct _tuple1*
_temp1463; void* _temp1465; struct Cyc_Absyn_PtrInfo _temp1467; struct Cyc_Absyn_Conref*
_temp1469; struct Cyc_Absyn_Tqual* _temp1471; struct Cyc_Absyn_Conref* _temp1473;
void* _temp1475; void* _temp1477; void* _temp1479; struct Cyc_Absyn_XTunionInfo
_temp1481; struct Cyc_Absyn_XTuniondecl* _temp1483; void* _temp1485; struct
_tuple1* _temp1487; void* _temp1489; struct Cyc_Absyn_PtrInfo _temp1491; struct
Cyc_Absyn_Conref* _temp1493; struct Cyc_Absyn_Tqual* _temp1495; struct Cyc_Absyn_Conref*
_temp1497; void* _temp1499; void* _temp1501; _LL1391: _LL1408: _temp1407=
_temp1389.f1; if(( unsigned int) _temp1407 > 4u?(( struct _tunion_struct*)
_temp1407)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1410: _temp1409=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1407)->f1; goto
_LL1404;} else{ goto _LL1393;} _LL1404: _temp1403= _temp1389.f2; if((
unsigned int) _temp1403 > 4u?(( struct _tunion_struct*) _temp1403)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1406: _temp1405=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1403)->f1; goto _LL1392;} else{ goto _LL1393;} _LL1393: _LL1420: _temp1419=
_temp1389.f1; if(( unsigned int) _temp1419 > 4u?(( struct _tunion_struct*)
_temp1419)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1426: _temp1425=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1419)->f1; goto _LL1424; _LL1424:
_temp1423=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1419)->f2; goto _LL1422; _LL1422: _temp1421=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1419)->f3; goto _LL1412;} else{ goto
_LL1395;} _LL1412: _temp1411= _temp1389.f2; if(( unsigned int) _temp1411 > 4u?((
struct _tunion_struct*) _temp1411)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1418:
_temp1417=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1411)->f1; goto
_LL1416; _LL1416: _temp1415=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1411)->f2; goto _LL1414; _LL1414: _temp1413=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1411)->f3; goto _LL1394;} else{ goto
_LL1395;} _LL1395: _LL1440: _temp1439= _temp1389.f1; if(( unsigned int)
_temp1439 > 4u?(( struct _tunion_struct*) _temp1439)->tag == Cyc_Absyn_TunionFieldType_tag:
0){ _LL1442: _temp1441=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1439)->f1; _LL1452: _temp1451=( struct _tuple1*) _temp1441.name; goto
_LL1450; _LL1450: _temp1449=( struct Cyc_List_List*) _temp1441.targs; goto
_LL1448; _LL1448: _temp1447=( struct _tuple1*) _temp1441.fname; goto _LL1446;
_LL1446: _temp1445=( struct Cyc_Absyn_Tuniondecl*) _temp1441.tud; goto _LL1444;
_LL1444: _temp1443=( struct Cyc_Absyn_Tunionfield*) _temp1441.tufd; goto _LL1428;}
else{ goto _LL1397;} _LL1428: _temp1427= _temp1389.f2; if(( unsigned int)
_temp1427 > 4u?(( struct _tunion_struct*) _temp1427)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL1430: _temp1429=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1427)->f1; _LL1438: _temp1437=( struct _tuple1*) _temp1429.name; goto
_LL1436; _LL1436: _temp1435=( struct Cyc_List_List*) _temp1429.targs; goto
_LL1434; _LL1434: _temp1433=( void*) _temp1429.rgn; goto _LL1432; _LL1432:
_temp1431=( struct Cyc_Absyn_Tuniondecl*) _temp1429.tud; goto _LL1396;} else{
goto _LL1397;} _LL1397: _LL1466: _temp1465= _temp1389.f1; if(( unsigned int)
_temp1465 > 4u?(( struct _tunion_struct*) _temp1465)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1468: _temp1467=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1465)->f1; _LL1478: _temp1477=( void*) _temp1467.elt_typ; goto _LL1476;
_LL1476: _temp1475=( void*) _temp1467.rgn_typ; goto _LL1474; _LL1474: _temp1473=(
struct Cyc_Absyn_Conref*) _temp1467.nullable; goto _LL1472; _LL1472: _temp1471=(
struct Cyc_Absyn_Tqual*) _temp1467.tq; goto _LL1470; _LL1470: _temp1469=( struct
Cyc_Absyn_Conref*) _temp1467.bounds; goto _LL1454;} else{ goto _LL1399;} _LL1454:
_temp1453= _temp1389.f2; if(( unsigned int) _temp1453 > 4u?(( struct
_tunion_struct*) _temp1453)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL1456:
_temp1455=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1453)->f1; _LL1464: _temp1463=( struct _tuple1*) _temp1455.name; goto
_LL1462; _LL1462: _temp1461=( struct Cyc_List_List*) _temp1455.targs; goto
_LL1460; _LL1460: _temp1459=( void*) _temp1455.rgn; goto _LL1458; _LL1458:
_temp1457=( struct Cyc_Absyn_Tuniondecl*) _temp1455.tud; goto _LL1398;} else{
goto _LL1399;} _LL1399: _LL1490: _temp1489= _temp1389.f1; if(( unsigned int)
_temp1489 > 4u?(( struct _tunion_struct*) _temp1489)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1492: _temp1491=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1489)->f1; _LL1502: _temp1501=( void*) _temp1491.elt_typ; goto _LL1500;
_LL1500: _temp1499=( void*) _temp1491.rgn_typ; goto _LL1498; _LL1498: _temp1497=(
struct Cyc_Absyn_Conref*) _temp1491.nullable; goto _LL1496; _LL1496: _temp1495=(
struct Cyc_Absyn_Tqual*) _temp1491.tq; goto _LL1494; _LL1494: _temp1493=( struct
Cyc_Absyn_Conref*) _temp1491.bounds; goto _LL1480;} else{ goto _LL1401;} _LL1480:
_temp1479= _temp1389.f2; if(( unsigned int) _temp1479 > 4u?(( struct
_tunion_struct*) _temp1479)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL1482:
_temp1481=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp1479)->f1; _LL1488: _temp1487=( struct _tuple1*) _temp1481.name; goto
_LL1486; _LL1486: _temp1485=( void*) _temp1481.rgn; goto _LL1484; _LL1484:
_temp1483=( struct Cyc_Absyn_XTuniondecl*) _temp1481.xtud; goto _LL1400;} else{
goto _LL1401;} _LL1401: goto _LL1402; _LL1392: { int okay= 1; if( !(( int(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1409.nullable, _temp1405.nullable)){
void* _temp1503=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1409.nullable))->v; int _temp1509; _LL1505:
if(( unsigned int) _temp1503 > 1u?(( struct _tunion_struct*) _temp1503)->tag ==
Cyc_Absyn_Eq_constr_tag: 0){ _LL1510: _temp1509=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1503)->f1; goto _LL1506;} else{ goto _LL1507;} _LL1507: goto _LL1508;
_LL1506: okay= ! _temp1509; goto _LL1504; _LL1508:( void) _throw((( struct
_xtunion_struct*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp1511=( char*)"silent_castable conref not eq";
struct _tagged_string _temp1512; _temp1512.curr= _temp1511; _temp1512.base=
_temp1511; _temp1512.last_plus_one= _temp1511 + 30; _temp1512;}))); _LL1504:;}
if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct
Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp, _temp1409.bounds,
_temp1405.bounds)){ struct _tuple7 _temp1514=({ struct _tuple7 _temp1513;
_temp1513.f1=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1409.bounds))->v; _temp1513.f2=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1405.bounds))->v; _temp1513;}); void* _temp1522; void* _temp1524; void*
_temp1526; void* _temp1528; struct Cyc_Absyn_Exp* _temp1530; void* _temp1532;
void* _temp1534; void* _temp1536; void* _temp1538; _LL1516: _LL1527: _temp1526=
_temp1514.f1; if(( unsigned int) _temp1526 > 1u?(( struct _tunion_struct*)
_temp1526)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1529: _temp1528=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1526)->f1; if(( unsigned int) _temp1528
> 1u?(( struct _tunion_struct*) _temp1528)->tag == Cyc_Absyn_Upper_b_tag: 0){
_LL1531: _temp1530=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1528)->f1; goto _LL1523;} else{ goto _LL1518;}} else{ goto _LL1518;}
_LL1523: _temp1522= _temp1514.f2; if(( unsigned int) _temp1522 > 1u?(( struct
_tunion_struct*) _temp1522)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1525:
_temp1524=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1522)->f1; if((
int) _temp1524 == Cyc_Absyn_Unknown_b){ goto _LL1517;} else{ goto _LL1518;}}
else{ goto _LL1518;} _LL1518: _LL1537: _temp1536= _temp1514.f1; if((
unsigned int) _temp1536 > 1u?(( struct _tunion_struct*) _temp1536)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL1539: _temp1538=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1536)->f1;
if(( int) _temp1538 == Cyc_Absyn_Unknown_b){ goto _LL1533;} else{ goto _LL1520;}}
else{ goto _LL1520;} _LL1533: _temp1532= _temp1514.f2; if(( unsigned int)
_temp1532 > 1u?(( struct _tunion_struct*) _temp1532)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL1535: _temp1534=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1532)->f1;
if(( int) _temp1534 == Cyc_Absyn_Unknown_b){ goto _LL1519;} else{ goto _LL1520;}}
else{ goto _LL1520;} _LL1520: goto _LL1521; _LL1517: okay= 1; goto _LL1515;
_LL1519: okay= 1; goto _LL1515; _LL1521: okay= 0; goto _LL1515; _LL1515:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1409.elt_typ,( void*) _temp1405.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1409.rgn_typ,( void*) _temp1405.rgn_typ): 0)?
!( _temp1409.tq)->q_const? 1:( _temp1405.tq)->q_const: 0;} _LL1394: { int okay;
okay=( _temp1421 != 0? _temp1413 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1540= _temp1421; if(
_temp1540 == 0){ _throw( Null_Exception);} _temp1540;})) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1541= _temp1413; if(
_temp1541 == 0){ _throw( Null_Exception);} _temp1541;})): 0; return( okay? Cyc_Tcutil_unify(
_temp1425, _temp1417): 0)? ! _temp1423->q_const? 1: _temp1415->q_const: 0;}
_LL1396: if( Cyc_Absyn_qvar_cmp( _temp1451, _temp1437) == 0?({ struct Cyc_Absyn_Tunionfield*
_temp1542= _temp1443; if( _temp1542 == 0){ _throw( Null_Exception);} _temp1542->typs;})
== 0: 0){ int okay= 1; for( 0; _temp1449 != 0? _temp1435 != 0: 0; _temp1449=({
struct Cyc_List_List* _temp1543= _temp1449; if( _temp1543 == 0){ _throw(
Null_Exception);} _temp1543->tl;}), _temp1435=({ struct Cyc_List_List* _temp1544=
_temp1435; if( _temp1544 == 0){ _throw( Null_Exception);} _temp1544->tl;})){ if(
! Cyc_Tcutil_unify(( void*)({ struct Cyc_List_List* _temp1545= _temp1449; if(
_temp1545 == 0){ _throw( Null_Exception);} _temp1545->hd;}),( void*)({ struct
Cyc_List_List* _temp1546= _temp1435; if( _temp1546 == 0){ _throw( Null_Exception);}
_temp1546->hd;}))){ okay= 0; break;}} if(( okay? _temp1449 == 0: 0)? _temp1435
== 0: 0){ return 1;}} return 0; _LL1398:{ void* _temp1547= Cyc_Tcutil_compress(
_temp1477); struct Cyc_Absyn_TunionFieldInfo _temp1553; struct Cyc_Absyn_Tunionfield*
_temp1555; struct Cyc_Absyn_Tuniondecl* _temp1557; struct _tuple1* _temp1559;
struct Cyc_List_List* _temp1561; struct _tuple1* _temp1563; _LL1549: if((
unsigned int) _temp1547 > 4u?(( struct _tunion_struct*) _temp1547)->tag == Cyc_Absyn_TunionFieldType_tag:
0){ _LL1554: _temp1553=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1547)->f1; _LL1564: _temp1563=( struct _tuple1*) _temp1553.name; goto
_LL1562; _LL1562: _temp1561=( struct Cyc_List_List*) _temp1553.targs; goto
_LL1560; _LL1560: _temp1559=( struct _tuple1*) _temp1553.fname; goto _LL1558;
_LL1558: _temp1557=( struct Cyc_Absyn_Tuniondecl*) _temp1553.tud; goto _LL1556;
_LL1556: _temp1555=( struct Cyc_Absyn_Tunionfield*) _temp1553.tufd; goto _LL1550;}
else{ goto _LL1551;} _LL1551: goto _LL1552; _LL1550: if( ! Cyc_Tcutil_unify(
_temp1475, _temp1459)? _temp1475 !=( void*) Cyc_Absyn_HeapRgn: 0){ return 0;}
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1473,(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0))){ return 0;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1469,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1565=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1565[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1566; _temp1566.tag= Cyc_Absyn_Upper_b_tag;
_temp1566.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1566;}); _temp1565;})))){ return 0;}
if(( Cyc_Absyn_qvar_cmp( _temp1563, _temp1463) == 0? _temp1555 != 0: 0)?({
struct Cyc_Absyn_Tunionfield* _temp1567= _temp1555; if( _temp1567 == 0){ _throw(
Null_Exception);} _temp1567->typs;}) != 0: 0){ int okay= 1; for( 0; _temp1561 !=
0? _temp1461 != 0: 0; _temp1561=({ struct Cyc_List_List* _temp1568= _temp1561;
if( _temp1568 == 0){ _throw( Null_Exception);} _temp1568->tl;}), _temp1461=({
struct Cyc_List_List* _temp1569= _temp1461; if( _temp1569 == 0){ _throw(
Null_Exception);} _temp1569->tl;})){ if( ! Cyc_Tcutil_unify(( void*)({ struct
Cyc_List_List* _temp1570= _temp1561; if( _temp1570 == 0){ _throw( Null_Exception);}
_temp1570->hd;}),( void*)({ struct Cyc_List_List* _temp1571= _temp1461; if(
_temp1571 == 0){ _throw( Null_Exception);} _temp1571->hd;}))){ okay= 0; break;}}
if(( ! okay? 1: _temp1561 != 0)? 1: _temp1461 != 0){ return 0;} return 1;} goto
_LL1548; _LL1552: goto _LL1548; _LL1548:;} return 0; _LL1400:{ void* _temp1572=
Cyc_Tcutil_compress( _temp1501); struct Cyc_Absyn_XTunionFieldInfo _temp1578;
struct Cyc_Absyn_Tunionfield* _temp1580; struct Cyc_Absyn_XTuniondecl* _temp1582;
struct _tuple1* _temp1584; struct _tuple1* _temp1586; _LL1574: if(( unsigned int)
_temp1572 > 4u?(( struct _tunion_struct*) _temp1572)->tag == Cyc_Absyn_XTunionFieldType_tag:
0){ _LL1579: _temp1578=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp1572)->f1; _LL1587: _temp1586=( struct _tuple1*) _temp1578.name; goto
_LL1585; _LL1585: _temp1584=( struct _tuple1*) _temp1578.fname; goto _LL1583;
_LL1583: _temp1582=( struct Cyc_Absyn_XTuniondecl*) _temp1578.xtud; goto _LL1581;
_LL1581: _temp1580=( struct Cyc_Absyn_Tunionfield*) _temp1578.xtufd; goto
_LL1575;} else{ goto _LL1576;} _LL1576: goto _LL1577; _LL1575: if( ! Cyc_Tcutil_unify(
_temp1499, _temp1485)? _temp1499 !=( void*) Cyc_Absyn_HeapRgn: 0){ return 0;}
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1497,(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0))){ return 0;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1493,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1588=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1588[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1589; _temp1589.tag= Cyc_Absyn_Upper_b_tag;
_temp1589.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1589;}); _temp1588;})))){ return 0;}
if( Cyc_Absyn_qvar_cmp( _temp1586, _temp1487) == 0){ return 1;} goto _LL1573;
_LL1577: goto _LL1573; _LL1573:;} return 0; _LL1402: return Cyc_Tcutil_unify( t1,
t2); _LL1390:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1594= e->topt; if( _temp1594 == 0){ _throw( Null_Exception);} _temp1594->v;}));
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type( t1)?
Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision( t1,
t2)){ Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ char* _temp1590=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1591; _temp1591.curr= _temp1590; _temp1591.base=
_temp1590; _temp1591.last_plus_one= _temp1590 + 44; _temp1591;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1592= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1593= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1592.last_plus_one - _temp1592.curr, _temp1592.curr, _temp1593.last_plus_one
- _temp1593.curr, _temp1593.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1625= e->topt; if( _temp1625 == 0){ _throw( Null_Exception);} _temp1625->v;}));
void* t;{ void* _temp1595= old_typ; struct Cyc_Absyn_Enumdecl* _temp1607; struct
_tuple1* _temp1609; void* _temp1611; void* _temp1613; _LL1597: if(( unsigned int)
_temp1595 > 4u?(( struct _tunion_struct*) _temp1595)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1610: _temp1609=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1595)->f1; goto _LL1608; _LL1608: _temp1607=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1595)->f2; goto _LL1598;} else{ goto
_LL1599;} _LL1599: if(( unsigned int) _temp1595 > 4u?(( struct _tunion_struct*)
_temp1595)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1614: _temp1613=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1595)->f1; goto _LL1612; _LL1612:
_temp1611=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1595)->f2; goto
_LL1600;} else{ goto _LL1601;} _LL1601: if(( int) _temp1595 == Cyc_Absyn_FloatType){
goto _LL1602;} else{ goto _LL1603;} _LL1603: if(( int) _temp1595 == Cyc_Absyn_DoubleType){
goto _LL1604;} else{ goto _LL1605;} _LL1605: goto _LL1606; _LL1598: t= Cyc_Absyn_sint_t;
goto _LL1596; _LL1600:{ void* _temp1615= _temp1611; _LL1617: if(( int) _temp1615
== Cyc_Absyn_B1){ goto _LL1618;} else{ goto _LL1619;} _LL1619: if(( int)
_temp1615 == Cyc_Absyn_B2){ goto _LL1620;} else{ goto _LL1621;} _LL1621: if((
int) _temp1615 == Cyc_Absyn_B4){ goto _LL1622;} else{ goto _LL1623;} _LL1623:
if(( int) _temp1615 == Cyc_Absyn_B8){ goto _LL1624;} else{ goto _LL1616;}
_LL1618: t= Cyc_Absyn_sint_t; goto _LL1616; _LL1620: t= Cyc_Absyn_sint_t; goto
_LL1616; _LL1622: t= _temp1613 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1616; _LL1624: t= _temp1613 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1616; _LL1616:;} goto _LL1596;
_LL1602: t= Cyc_Absyn_float_t; goto _LL1596; _LL1604: t= Cyc_Absyn_double_t;
goto _LL1596; _LL1606: return 0; _LL1596:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1629= e->topt; if( _temp1629 == 0){ _throw( Null_Exception);} _temp1629->v;}));
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote( te, e); if(
Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp1626= e->topt; if(
_temp1626 == 0){ _throw( Null_Exception);} _temp1626->v;}), t2)){ return 1;}
else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,({ struct _tagged_string _temp1627= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp1628= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1627.last_plus_one - _temp1627.curr, _temp1627.curr, _temp1628.last_plus_one
- _temp1628.curr, _temp1628.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1630= Cyc_Tcutil_compress( t); void* _temp1640; void*
_temp1642; _LL1632: if(( unsigned int) _temp1630 > 4u?(( struct _tunion_struct*)
_temp1630)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1643: _temp1642=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1630)->f1; goto _LL1641; _LL1641:
_temp1640=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1630)->f2; goto
_LL1633;} else{ goto _LL1634;} _LL1634: if(( int) _temp1630 == Cyc_Absyn_FloatType){
goto _LL1635;} else{ goto _LL1636;} _LL1636: if(( int) _temp1630 == Cyc_Absyn_DoubleType){
goto _LL1637;} else{ goto _LL1638;} _LL1638: goto _LL1639; _LL1633: return 1;
_LL1635: return 1; _LL1637: return 1; _LL1639: return 0; _LL1631:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1644=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1644->f1= x->tq; _temp1644->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1644;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1645= t1; struct Cyc_List_List* _temp1655; struct Cyc_Absyn_Structdecl**
_temp1657; struct Cyc_List_List* _temp1659; struct _tuple1* _temp1661; _LL1647:
if(( int) _temp1645 == Cyc_Absyn_VoidType){ goto _LL1648;} else{ goto _LL1649;}
_LL1649: if(( unsigned int) _temp1645 > 4u?(( struct _tunion_struct*) _temp1645)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL1656: _temp1655=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1645)->f1; goto _LL1650;} else{ goto
_LL1651;} _LL1651: if(( unsigned int) _temp1645 > 4u?(( struct _tunion_struct*)
_temp1645)->tag == Cyc_Absyn_StructType_tag: 0){ _LL1662: _temp1661=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1645)->f1; goto _LL1660;
_LL1660: _temp1659=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1645)->f2; goto _LL1658; _LL1658: _temp1657=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1645)->f3; goto _LL1652;} else{ goto
_LL1653;} _LL1653: goto _LL1654; _LL1648: return 0; _LL1650: return _temp1655;
_LL1652: if( _temp1657 == 0? 1: _temp1661 == 0){ return({ struct Cyc_List_List*
_temp1663=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1663->hd=( void*)({ struct _tuple4* _temp1664=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1664->f1= Cyc_Absyn_empty_tqual(); _temp1664->f2=
t1; _temp1664;}); _temp1663->tl= 0; _temp1663;});}{ struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl** _temp1669= _temp1657; if( _temp1669 == 0){
_throw( Null_Exception);}* _temp1669;}); struct _tuple1 n=({ struct _tuple1*
_temp1668= _temp1661; if( _temp1668 == 0){ _throw( Null_Exception);}* _temp1668;});
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( sd->tvs, _temp1659); if( sd->fields ==
0){ return({ struct Cyc_List_List* _temp1665=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1665->hd=( void*)({ struct _tuple4*
_temp1666=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1666->f1=
Cyc_Absyn_empty_tqual(); _temp1666->f2= t1; _temp1666;}); _temp1665->tl= 0;
_temp1665;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, inst,(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1667= sd->fields; if(
_temp1667 == 0){ _throw( Null_Exception);} _temp1667->v;}));} _LL1654: return({
struct Cyc_List_List* _temp1670=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1670->hd=( void*)({ struct _tuple4* _temp1671=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1671->f1= Cyc_Absyn_empty_tqual();
_temp1671->f2= t1; _temp1671;}); _temp1670->tl= 0; _temp1670;}); _LL1646:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=({ struct
Cyc_List_List* _temp1672= a; if( _temp1672 == 0){ _throw( Null_Exception);}
_temp1672->tl;})){ if( Cyc_Tcutil_unify( t1,(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp1673= a; if( _temp1673 == 0){ _throw( Null_Exception);} _temp1673->hd;}))).f1)?
Cyc_Tcutil_unify( t2,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1674= a;
if( _temp1674 == 0){ _throw( Null_Exception);} _temp1674->hd;}))).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple7 _temp1676=({ struct _tuple7 _temp1675; _temp1675.f1= t1; _temp1675.f2=
t2; _temp1675;}); void* _temp1682; struct Cyc_Absyn_PtrInfo _temp1684; struct
Cyc_Absyn_Conref* _temp1686; struct Cyc_Absyn_Tqual* _temp1688; struct Cyc_Absyn_Conref*
_temp1690; void* _temp1692; void* _temp1694; void* _temp1696; struct Cyc_Absyn_PtrInfo
_temp1698; struct Cyc_Absyn_Conref* _temp1700; struct Cyc_Absyn_Tqual* _temp1702;
struct Cyc_Absyn_Conref* _temp1704; void* _temp1706; void* _temp1708; _LL1678:
_LL1697: _temp1696= _temp1676.f1; if(( unsigned int) _temp1696 > 4u?(( struct
_tunion_struct*) _temp1696)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1699:
_temp1698=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1696)->f1; _LL1709: _temp1708=( void*) _temp1698.elt_typ; goto _LL1707;
_LL1707: _temp1706=( void*) _temp1698.rgn_typ; goto _LL1705; _LL1705: _temp1704=(
struct Cyc_Absyn_Conref*) _temp1698.nullable; goto _LL1703; _LL1703: _temp1702=(
struct Cyc_Absyn_Tqual*) _temp1698.tq; goto _LL1701; _LL1701: _temp1700=( struct
Cyc_Absyn_Conref*) _temp1698.bounds; goto _LL1683;} else{ goto _LL1680;} _LL1683:
_temp1682= _temp1676.f2; if(( unsigned int) _temp1682 > 4u?(( struct
_tunion_struct*) _temp1682)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1685:
_temp1684=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1682)->f1; _LL1695: _temp1694=( void*) _temp1684.elt_typ; goto _LL1693;
_LL1693: _temp1692=( void*) _temp1684.rgn_typ; goto _LL1691; _LL1691: _temp1690=(
struct Cyc_Absyn_Conref*) _temp1684.nullable; goto _LL1689; _LL1689: _temp1688=(
struct Cyc_Absyn_Tqual*) _temp1684.tq; goto _LL1687; _LL1687: _temp1686=( struct
Cyc_Absyn_Conref*) _temp1684.bounds; goto _LL1679;} else{ goto _LL1680;} _LL1680:
goto _LL1681; _LL1679: if( _temp1702->q_const? ! _temp1688->q_const: 0){ return
0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct
Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1704,
_temp1690)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1704): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1690): 0){ return 0;}{ void* _temp1710= _temp1706; _LL1712: if(( int)
_temp1710 == Cyc_Absyn_HeapRgn){ goto _LL1713;} else{ goto _LL1714;} _LL1714:
goto _LL1715; _LL1713: goto _LL1711; _LL1715: Cyc_Tcutil_unify( _temp1706,
_temp1692); goto _LL1711; _LL1711:;} if( !(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1700, _temp1686)){ struct _tuple7 _temp1717=({ struct
_tuple7 _temp1716; _temp1716.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1700); _temp1716.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1686); _temp1716;}); void* _temp1725; void* _temp1727; struct Cyc_Absyn_Exp*
_temp1729; void* _temp1731; struct Cyc_Absyn_Exp* _temp1733; void* _temp1735;
struct Cyc_Absyn_Exp* _temp1737; _LL1719: _LL1728: _temp1727= _temp1717.f1; if((
unsigned int) _temp1727 > 1u?(( struct _tunion_struct*) _temp1727)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1730: _temp1729=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1727)->f1; goto _LL1726;} else{ goto _LL1721;} _LL1726: _temp1725=
_temp1717.f2; if(( int) _temp1725 == Cyc_Absyn_Unknown_b){ goto _LL1720;} else{
goto _LL1721;} _LL1721: _LL1736: _temp1735= _temp1717.f1; if(( unsigned int)
_temp1735 > 1u?(( struct _tunion_struct*) _temp1735)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1738: _temp1737=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1735)->f1; goto _LL1732;} else{ goto _LL1723;} _LL1732: _temp1731=
_temp1717.f2; if(( unsigned int) _temp1731 > 1u?(( struct _tunion_struct*)
_temp1731)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1734: _temp1733=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1731)->f1; goto _LL1722;} else{ goto
_LL1723;} _LL1723: goto _LL1724; _LL1720: goto _LL1718; _LL1722: if( Cyc_Evexp_eval_const_uint_exp(
_temp1737) < Cyc_Evexp_eval_const_uint_exp( _temp1733)){ return 0;} goto _LL1718;
_LL1724: return 0; _LL1718:;} return Cyc_Tcutil_ptrsubtype( te,({ struct Cyc_List_List*
_temp1739=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1739->hd=( void*)({ struct _tuple7* _temp1740=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1740->f1= t1; _temp1740->f2= t2; _temp1740;});
_temp1739->tl= assume; _temp1739;}), _temp1708, _temp1694); _LL1681: return 0;
_LL1677:;}} static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 != 0; tqs2=({ struct Cyc_List_List* _temp1741= tqs2; if( _temp1741 == 0){
_throw( Null_Exception);} _temp1741->tl;}), tqs1=({ struct Cyc_List_List*
_temp1742= tqs1; if( _temp1742 == 0){ _throw( Null_Exception);} _temp1742->tl;})){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1746; void* _temp1747; struct
Cyc_Absyn_Tqual* _temp1749; struct _tuple4* _temp1744=( struct _tuple4*)({
struct Cyc_List_List* _temp1743= tqs1; if( _temp1743 == 0){ _throw(
Null_Exception);} _temp1743->hd;}); _temp1746=* _temp1744; _LL1750: _temp1749=
_temp1746.f1; goto _LL1748; _LL1748: _temp1747= _temp1746.f2; goto _LL1745;
_LL1745: { struct _tuple4 _temp1754; void* _temp1755; struct Cyc_Absyn_Tqual*
_temp1757; struct _tuple4* _temp1752=( struct _tuple4*)({ struct Cyc_List_List*
_temp1751= tqs2; if( _temp1751 == 0){ _throw( Null_Exception);} _temp1751->hd;});
_temp1754=* _temp1752; _LL1758: _temp1757= _temp1754.f1; goto _LL1756; _LL1756:
_temp1755= _temp1754.f2; goto _LL1753; _LL1753: if( _temp1757->q_const? Cyc_Tcutil_subtype(
te, assume, _temp1747, _temp1755): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1747, _temp1755)){ continue;} else{ return 0;}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1759= t2; struct Cyc_Absyn_Uniondecl** _temp1767; struct Cyc_List_List*
_temp1769; struct _tuple1* _temp1771; void* _temp1773; void* _temp1775; _LL1761:
if(( unsigned int) _temp1759 > 4u?(( struct _tunion_struct*) _temp1759)->tag ==
Cyc_Absyn_UnionType_tag: 0){ _LL1772: _temp1771=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1759)->f1; goto _LL1770; _LL1770: _temp1769=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp1759)->f2; goto _LL1768; _LL1768:
_temp1767=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1759)->f3; goto _LL1762;} else{ goto _LL1763;} _LL1763: if(( unsigned int)
_temp1759 > 4u?(( struct _tunion_struct*) _temp1759)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1776: _temp1775=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1759)->f1;
goto _LL1774; _LL1774: _temp1773=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1759)->f2; if(( int) _temp1773 == Cyc_Absyn_B4){ goto _LL1764;} else{ goto
_LL1765;}} else{ goto _LL1765;} _LL1765: goto _LL1766; _LL1762: if( _temp1767 ==
0){ goto _LL1760;}{ struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl**
_temp1780= _temp1767; if( _temp1780 == 0){ _throw( Null_Exception);}* _temp1780;});
if( ud->fields == 0){ goto _LL1760;}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( ud->tvs,
_temp1769);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1779= ud->fields; if( _temp1779 == 0){ _throw( Null_Exception);} _temp1779->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1777= fs; if( _temp1777 == 0){
_throw( Null_Exception);} _temp1777->tl;})){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
inst,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp1778=
fs; if( _temp1778 == 0){ _throw( Null_Exception);} _temp1778->hd;}))->type))){
return 1;}}} goto _LL1760;}} _LL1764: if( Cyc_Tcutil_typ_kind( t2) ==( void*)
Cyc_Absyn_BoxKind){ return 1;} goto _LL1760; _LL1766: goto _LL1760; _LL1760:;}{
void* _temp1781= t1; struct Cyc_Absyn_PtrInfo _temp1797; struct Cyc_Absyn_Conref*
_temp1799; struct Cyc_Absyn_Tqual* _temp1801; struct Cyc_Absyn_Conref* _temp1803;
void* _temp1805; void* _temp1807; struct Cyc_Absyn_Exp* _temp1809; struct Cyc_Absyn_Tqual*
_temp1811; void* _temp1813; struct Cyc_Absyn_Enumdecl* _temp1815; struct _tuple1*
_temp1817; void* _temp1819; void* _temp1821; _LL1783: if(( unsigned int)
_temp1781 > 4u?(( struct _tunion_struct*) _temp1781)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1798: _temp1797=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1781)->f1; _LL1808: _temp1807=( void*) _temp1797.elt_typ; goto _LL1806;
_LL1806: _temp1805=( void*) _temp1797.rgn_typ; goto _LL1804; _LL1804: _temp1803=(
struct Cyc_Absyn_Conref*) _temp1797.nullable; goto _LL1802; _LL1802: _temp1801=(
struct Cyc_Absyn_Tqual*) _temp1797.tq; goto _LL1800; _LL1800: _temp1799=( struct
Cyc_Absyn_Conref*) _temp1797.bounds; goto _LL1784;} else{ goto _LL1785;} _LL1785:
if(( unsigned int) _temp1781 > 4u?(( struct _tunion_struct*) _temp1781)->tag ==
Cyc_Absyn_ArrayType_tag: 0){ _LL1814: _temp1813=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1781)->f1; goto _LL1812; _LL1812: _temp1811=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1781)->f2; goto _LL1810; _LL1810:
_temp1809=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1781)->f3; goto _LL1786;} else{ goto _LL1787;} _LL1787: if(( unsigned int)
_temp1781 > 4u?(( struct _tunion_struct*) _temp1781)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1818: _temp1817=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1781)->f1; goto _LL1816; _LL1816: _temp1815=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1781)->f2; goto _LL1788;} else{ goto
_LL1789;} _LL1789: if(( unsigned int) _temp1781 > 4u?(( struct _tunion_struct*)
_temp1781)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1822: _temp1821=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1781)->f1; goto _LL1820; _LL1820:
_temp1819=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1781)->f2; goto
_LL1790;} else{ goto _LL1791;} _LL1791: if(( int) _temp1781 == Cyc_Absyn_FloatType){
goto _LL1792;} else{ goto _LL1793;} _LL1793: if(( int) _temp1781 == Cyc_Absyn_DoubleType){
goto _LL1794;} else{ goto _LL1795;} _LL1795: goto _LL1796; _LL1784:{ void*
_temp1823= t2; struct Cyc_Absyn_PtrInfo _temp1829; struct Cyc_Absyn_Conref*
_temp1831; struct Cyc_Absyn_Tqual* _temp1833; struct Cyc_Absyn_Conref* _temp1835;
void* _temp1837; void* _temp1839; _LL1825: if(( unsigned int) _temp1823 > 4u?((
struct _tunion_struct*) _temp1823)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1830: _temp1829=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1823)->f1; _LL1840: _temp1839=( void*) _temp1829.elt_typ; goto _LL1838;
_LL1838: _temp1837=( void*) _temp1829.rgn_typ; goto _LL1836; _LL1836: _temp1835=(
struct Cyc_Absyn_Conref*) _temp1829.nullable; goto _LL1834; _LL1834: _temp1833=(
struct Cyc_Absyn_Tqual*) _temp1829.tq; goto _LL1832; _LL1832: _temp1831=( struct
Cyc_Absyn_Conref*) _temp1829.bounds; goto _LL1826;} else{ goto _LL1827;} _LL1827:
goto _LL1828; _LL1826: { struct Cyc_List_List* assump=({ struct Cyc_List_List*
_temp1856=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1856->hd=( void*)({ struct _tuple7* _temp1857=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1857->f1= t1; _temp1857->f2= t2; _temp1857;});
_temp1856->tl= 0; _temp1856;}); int ptrsub= Cyc_Tcutil_ptrsubtype( te, assump,
_temp1807, _temp1839)? ! _temp1801->q_const? 1: _temp1833->q_const: 0; int
bounds_ok=(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1799, _temp1831); if( ! bounds_ok){ struct _tuple7 _temp1842=({ struct
_tuple7 _temp1841; _temp1841.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1799); _temp1841.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1831); _temp1841;}); void* _temp1848; struct Cyc_Absyn_Exp* _temp1850; void*
_temp1852; struct Cyc_Absyn_Exp* _temp1854; _LL1844: _LL1853: _temp1852=
_temp1842.f1; if(( unsigned int) _temp1852 > 1u?(( struct _tunion_struct*)
_temp1852)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1855: _temp1854=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1852)->f1; goto _LL1849;} else{ goto
_LL1846;} _LL1849: _temp1848= _temp1842.f2; if(( unsigned int) _temp1848 > 1u?((
struct _tunion_struct*) _temp1848)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1851:
_temp1850=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1848)->f1;
goto _LL1845;} else{ goto _LL1846;} _LL1846: goto _LL1847; _LL1845: if( Cyc_Evexp_eval_const_uint_exp(
_temp1854) >= Cyc_Evexp_eval_const_uint_exp( _temp1850)){ bounds_ok= 1;} goto
_LL1843; _LL1847: bounds_ok= 1; goto _LL1843; _LL1843:;} return( bounds_ok?
ptrsub: 0)? _temp1805 ==( void*) Cyc_Absyn_HeapRgn? 1: Cyc_Tcutil_unify(
_temp1805, _temp1837): 0;} _LL1828: goto _LL1824; _LL1824:;} return 0; _LL1786:{
void* _temp1858= t2; struct Cyc_Absyn_Exp* _temp1864; struct Cyc_Absyn_Tqual*
_temp1866; void* _temp1868; _LL1860: if(( unsigned int) _temp1858 > 4u?(( struct
_tunion_struct*) _temp1858)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1869:
_temp1868=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1858)->f1; goto
_LL1867; _LL1867: _temp1866=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1858)->f2; goto _LL1865; _LL1865: _temp1864=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1858)->f3; goto _LL1861;} else{ goto
_LL1862;} _LL1862: goto _LL1863; _LL1861: { int okay; okay=( _temp1809 != 0?
_temp1864 != 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1870= _temp1809; if( _temp1870 == 0){ _throw(
Null_Exception);} _temp1870;})) >= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1871= _temp1864; if( _temp1871 == 0){ _throw(
Null_Exception);} _temp1871;})): 0; return( okay? Cyc_Tcutil_unify( _temp1813,
_temp1868): 0)? ! _temp1811->q_const? 1: _temp1866->q_const: 0;} _LL1863: return
0; _LL1859:;} return 0; _LL1788: goto _LL1790; _LL1790: goto _LL1792; _LL1792:
goto _LL1794; _LL1794: return Cyc_Tcutil_coerceable( t2); _LL1796: return 0;
_LL1782:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt*
_temp1872= e->topt; if( _temp1872 == 0){ _throw( Null_Exception);} _temp1872->v;}),
t)){ struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp1873=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1873[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp1874; _temp1874.tag= Cyc_Absyn_Cast_e_tag; _temp1874.f1=( void*) t;
_temp1874.f2= inner; _temp1874;}); _temp1873;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1875=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1875->v=( void*) t; _temp1875;});}} int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*
e){ void* _temp1877= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1876= e->topt; if( _temp1876 == 0){ _throw( Null_Exception);} _temp1876->v;}));
struct Cyc_Absyn_Enumdecl* _temp1887; struct _tuple1* _temp1889; void* _temp1891;
void* _temp1893; int _temp1895; struct Cyc_Core_Opt* _temp1897; void* _temp1899;
_LL1879: if(( unsigned int) _temp1877 > 4u?(( struct _tunion_struct*) _temp1877)->tag
== Cyc_Absyn_EnumType_tag: 0){ _LL1890: _temp1889=( struct _tuple1*)(( struct
Cyc_Absyn_EnumType_struct*) _temp1877)->f1; goto _LL1888; _LL1888: _temp1887=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp1877)->f2;
goto _LL1880;} else{ goto _LL1881;} _LL1881: if(( unsigned int) _temp1877 > 4u?((
struct _tunion_struct*) _temp1877)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1894:
_temp1893=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1877)->f1; goto
_LL1892; _LL1892: _temp1891=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1877)->f2; goto _LL1882;} else{ goto _LL1883;} _LL1883: if(( unsigned int)
_temp1877 > 4u?(( struct _tunion_struct*) _temp1877)->tag == Cyc_Absyn_Evar_tag:
0){ _LL1900: _temp1899=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp1877)->f1;
goto _LL1898; _LL1898: _temp1897=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp1877)->f2; goto _LL1896; _LL1896: _temp1895=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp1877)->f3; goto _LL1884;} else{ goto _LL1885;} _LL1885: goto _LL1886;
_LL1880: goto _LL1882; _LL1882: return 1; _LL1884: if( Cyc_Tcutil_unify(( void*)({
struct Cyc_Core_Opt* _temp1901= e->topt; if( _temp1901 == 0){ _throw(
Null_Exception);} _temp1901->v;}), Cyc_Absyn_sint_t)){ return 1;} return 0;
_LL1886: return 0; _LL1878:;} int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp* e){
if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void* _temp1903= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1902= e->topt; if( _temp1902 == 0){ _throw(
Null_Exception);} _temp1902->v;})); _LL1905: if(( int) _temp1903 == Cyc_Absyn_FloatType){
goto _LL1906;} else{ goto _LL1907;} _LL1907: if(( int) _temp1903 == Cyc_Absyn_DoubleType){
goto _LL1908;} else{ goto _LL1909;} _LL1909: goto _LL1910; _LL1906: goto _LL1908;
_LL1908: return 1; _LL1910: return 0; _LL1904:;}} int Cyc_Tcutil_is_function_type(
void* t){ void* _temp1911= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp1917; _LL1913: if(( unsigned int) _temp1911 > 4u?(( struct _tunion_struct*)
_temp1911)->tag == Cyc_Absyn_FnType_tag: 0){ _LL1918: _temp1917=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp1911)->f1; goto _LL1914;} else{ goto
_LL1915;} _LL1915: goto _LL1916; _LL1914: return 1; _LL1916: return 0; _LL1912:;}
void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct _tuple7
_temp1920=({ struct _tuple7 _temp1919; _temp1919.f1= t1; _temp1919.f2= t2;
_temp1919;}); void* _temp1944; void* _temp1946; void* _temp1948; void* _temp1950;
void* _temp1952; void* _temp1954; void* _temp1956; void* _temp1958; void*
_temp1960; void* _temp1962; void* _temp1964; void* _temp1966; void* _temp1968;
void* _temp1970; void* _temp1972; void* _temp1974; void* _temp1976; void*
_temp1978; void* _temp1980; void* _temp1982; void* _temp1984; void* _temp1986;
void* _temp1988; void* _temp1990; void* _temp1992; void* _temp1994; void*
_temp1996; void* _temp1998; void* _temp2000; void* _temp2002; void* _temp2004;
void* _temp2006; _LL1922: _LL1947: _temp1946= _temp1920.f1; if(( int) _temp1946
== Cyc_Absyn_DoubleType){ goto _LL1945;} else{ goto _LL1924;} _LL1945: _temp1944=
_temp1920.f2; goto _LL1923; _LL1924: _LL1951: _temp1950= _temp1920.f1; goto
_LL1949; _LL1949: _temp1948= _temp1920.f2; if(( int) _temp1948 == Cyc_Absyn_DoubleType){
goto _LL1925;} else{ goto _LL1926;} _LL1926: _LL1955: _temp1954= _temp1920.f1;
if(( int) _temp1954 == Cyc_Absyn_FloatType){ goto _LL1953;} else{ goto _LL1928;}
_LL1953: _temp1952= _temp1920.f2; goto _LL1927; _LL1928: _LL1959: _temp1958=
_temp1920.f1; goto _LL1957; _LL1957: _temp1956= _temp1920.f2; if(( int)
_temp1956 == Cyc_Absyn_FloatType){ goto _LL1929;} else{ goto _LL1930;} _LL1930:
_LL1963: _temp1962= _temp1920.f1; if(( unsigned int) _temp1962 > 4u?(( struct
_tunion_struct*) _temp1962)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1967:
_temp1966=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1962)->f1; if(( int)
_temp1966 == Cyc_Absyn_Unsigned){ goto _LL1965;} else{ goto _LL1932;} _LL1965:
_temp1964=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1962)->f2; if(( int)
_temp1964 == Cyc_Absyn_B8){ goto _LL1961;} else{ goto _LL1932;}} else{ goto
_LL1932;} _LL1961: _temp1960= _temp1920.f2; goto _LL1931; _LL1932: _LL1975:
_temp1974= _temp1920.f1; goto _LL1969; _LL1969: _temp1968= _temp1920.f2; if((
unsigned int) _temp1968 > 4u?(( struct _tunion_struct*) _temp1968)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1973: _temp1972=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1968)->f1;
if(( int) _temp1972 == Cyc_Absyn_Unsigned){ goto _LL1971;} else{ goto _LL1934;}
_LL1971: _temp1970=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1968)->f2;
if(( int) _temp1970 == Cyc_Absyn_B8){ goto _LL1933;} else{ goto _LL1934;}} else{
goto _LL1934;} _LL1934: _LL1979: _temp1978= _temp1920.f1; if(( unsigned int)
_temp1978 > 4u?(( struct _tunion_struct*) _temp1978)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1983: _temp1982=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1978)->f1;
if(( int) _temp1982 == Cyc_Absyn_Signed){ goto _LL1981;} else{ goto _LL1936;}
_LL1981: _temp1980=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1978)->f2;
if(( int) _temp1980 == Cyc_Absyn_B8){ goto _LL1977;} else{ goto _LL1936;}} else{
goto _LL1936;} _LL1977: _temp1976= _temp1920.f2; goto _LL1935; _LL1936: _LL1991:
_temp1990= _temp1920.f1; goto _LL1985; _LL1985: _temp1984= _temp1920.f2; if((
unsigned int) _temp1984 > 4u?(( struct _tunion_struct*) _temp1984)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1989: _temp1988=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1984)->f1;
if(( int) _temp1988 == Cyc_Absyn_Signed){ goto _LL1987;} else{ goto _LL1938;}
_LL1987: _temp1986=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1984)->f2;
if(( int) _temp1986 == Cyc_Absyn_B8){ goto _LL1937;} else{ goto _LL1938;}} else{
goto _LL1938;} _LL1938: _LL1995: _temp1994= _temp1920.f1; if(( unsigned int)
_temp1994 > 4u?(( struct _tunion_struct*) _temp1994)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1999: _temp1998=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1994)->f1;
if(( int) _temp1998 == Cyc_Absyn_Unsigned){ goto _LL1997;} else{ goto _LL1940;}
_LL1997: _temp1996=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1994)->f2;
if(( int) _temp1996 == Cyc_Absyn_B4){ goto _LL1993;} else{ goto _LL1940;}} else{
goto _LL1940;} _LL1993: _temp1992= _temp1920.f2; goto _LL1939; _LL1940: _LL2007:
_temp2006= _temp1920.f1; goto _LL2001; _LL2001: _temp2000= _temp1920.f2; if((
unsigned int) _temp2000 > 4u?(( struct _tunion_struct*) _temp2000)->tag == Cyc_Absyn_IntType_tag:
0){ _LL2005: _temp2004=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2000)->f1;
if(( int) _temp2004 == Cyc_Absyn_Unsigned){ goto _LL2003;} else{ goto _LL1942;}
_LL2003: _temp2002=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2000)->f2;
if(( int) _temp2002 == Cyc_Absyn_B4){ goto _LL1941;} else{ goto _LL1942;}} else{
goto _LL1942;} _LL1942: goto _LL1943; _LL1923: goto _LL1925; _LL1925: return(
void*) Cyc_Absyn_DoubleType; _LL1927: goto _LL1929; _LL1929: return( void*) Cyc_Absyn_FloatType;
_LL1931: goto _LL1933; _LL1933: return Cyc_Absyn_ulong_t; _LL1935: goto _LL1937;
_LL1937: return Cyc_Absyn_slong_t; _LL1939: goto _LL1941; _LL1941: return Cyc_Absyn_uint_t;
_LL1943: return Cyc_Absyn_sint_t; _LL1921:;} void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp* e){ void* _temp2008=( void*) e->r; struct Cyc_Absyn_Exp*
_temp2014; struct Cyc_Core_Opt* _temp2016; struct Cyc_Absyn_Exp* _temp2018;
_LL2010: if((( struct _tunion_struct*) _temp2008)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL2019: _temp2018=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2008)->f1; goto _LL2017; _LL2017: _temp2016=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp2008)->f2; if( _temp2016 == 0){ goto
_LL2015;} else{ goto _LL2012;} _LL2015: _temp2014=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp2008)->f3; goto _LL2011;} else{ goto
_LL2012;} _LL2012: goto _LL2013; _LL2011: Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp2020=( char*)"assignment in test"; struct
_tagged_string _temp2021; _temp2021.curr= _temp2020; _temp2021.base= _temp2020;
_temp2021.last_plus_one= _temp2020 + 19; _temp2021;})); goto _LL2009; _LL2013:
goto _LL2009; _LL2009:;} static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref*
c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){ return 1;}
if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)( c1->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2022=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2022[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2023; _temp2023.tag= Cyc_Absyn_Forward_constr_tag;
_temp2023.f1= c2; _temp2023;}); _temp2022;}))); return 1;} else{ if(( void*) c2->v
==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2024=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2024[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2025; _temp2025.tag= Cyc_Absyn_Forward_constr_tag; _temp2025.f1= c1;
_temp2025;}); _temp2024;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2026=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2026[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2027; _temp2027.tag= Cyc_Absyn_Forward_constr_tag;
_temp2027.f1= c1; _temp2027;}); _temp2026;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2028=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2028[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2029; _temp2029.tag= Cyc_Absyn_Forward_constr_tag; _temp2029.f1= c2;
_temp2029;}); _temp2028;}))); return 1;} else{ return 0;}}}}} static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc, struct Cyc_List_List*
tvs, struct Cyc_Absyn_Tvar* tv){ if( tvs == 0){ return({ struct Cyc_List_List*
_temp2030=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2030->hd=( void*) tv; _temp2030->tl= 0; _temp2030;});} else{{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp2031= tvs2; if(
_temp2031 == 0){ _throw( Null_Exception);} _temp2031->tl;})){ if( Cyc_String_zstrptrcmp(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2032= tvs2; if( _temp2032
== 0){ _throw( Null_Exception);} _temp2032->hd;}))->name, tv->name) == 0){
struct Cyc_Absyn_Conref* k1=(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2036= tvs2; if( _temp2036 == 0){ _throw( Null_Exception);} _temp2036->hd;}))->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2033=* tv->name; struct
_tagged_string _temp2034= Cyc_Absynpp_ckind2string( k1); struct _tagged_string
_temp2035= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp2033.last_plus_one - _temp2033.curr, _temp2033.curr, _temp2034.last_plus_one
- _temp2034.curr, _temp2034.curr, _temp2035.last_plus_one - _temp2035.curr,
_temp2035.curr);}));} return tvs;} else{ if(({ struct Cyc_List_List* _temp2037=
tvs2; if( _temp2037 == 0){ _throw( Null_Exception);} _temp2037->tl;}) == 0){({
struct Cyc_List_List* _temp2038= tvs2; if( _temp2038 == 0){ _throw(
Null_Exception);} _temp2038->tl=({ struct Cyc_List_List* _temp2039=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2039->hd=( void*) tv; _temp2039->tl=
0; _temp2039;});}); return tvs;}}}} return(( struct Cyc_List_List*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp2040=( char*)"Tcutil::add_free_tvar:expecting to have returned."; struct
_tagged_string _temp2041; _temp2041.curr= _temp2040; _temp2041.base= _temp2040;
_temp2041.last_plus_one= _temp2040 + 50; _temp2041;}));}} static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs, struct Cyc_List_List*
btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=({ struct Cyc_List_List*
_temp2042= tvs; if( _temp2042 == 0){ _throw( Null_Exception);} _temp2042->tl;})){
int present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b=({ struct Cyc_List_List*
_temp2043= b; if( _temp2043 == 0){ _throw( Null_Exception);} _temp2043->tl;})){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2044= tvs; if( _temp2044 == 0){ _throw( Null_Exception);} _temp2044->hd;}))->name,((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2045= b; if( _temp2045 == 0){
_throw( Null_Exception);} _temp2045->hd;}))->name) == 0){ present= 1; break;}}}
if( ! present){ r=({ struct Cyc_List_List* _temp2046=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2046->hd=( void*)(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2047= tvs; if( _temp2047 == 0){ _throw(
Null_Exception);} _temp2047->hd;})); _temp2046->tl= r; _temp2046;});}} r=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ struct _tagged_string s= xprintf("#%d",( Cyc_Tcutil_tvar_counter ++));
return({ struct Cyc_Absyn_Tvar* _temp2048=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp2048->name=({ struct _tagged_string*
_temp2049=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2049[ 0]= s; _temp2049;}); _temp2048->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)( k); _temp2048;});} int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar* t){ struct _tagged_string s=* t->name; return({ struct
_tagged_string _temp2050= s; char* _temp2052= _temp2050.curr + 0; if( _temp2050.base
== 0? 1:( _temp2052 < _temp2050.base? 1: _temp2052 >= _temp2050.last_plus_one)){
_throw( Null_Exception);}* _temp2052;}) =='#';} void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct
_tagged_string s= Cyc_String_strconcat(( struct _tagged_string)({ char*
_temp2057=( char*)"`"; struct _tagged_string _temp2058; _temp2058.curr=
_temp2057; _temp2058.base= _temp2057; _temp2058.last_plus_one= _temp2057 + 2;
_temp2058;}),* t->name);({ struct _tagged_string _temp2053= s; char* _temp2055=
_temp2053.curr + 1; if( _temp2053.base == 0? 1:( _temp2055 < _temp2053.base? 1:
_temp2055 >= _temp2053.last_plus_one)){ _throw( Null_Exception);}* _temp2055='t';});
t->name=({ struct _tagged_string* _temp2056=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2056[ 0]= s; _temp2056;});}} static
struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple11* x){ return({ struct
_tuple2* _temp2059=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2059->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2060=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2060->v=( void*)(*
x).f1; _temp2060;}); _temp2059->f2=(* x).f2; _temp2059->f3=(* x).f3; _temp2059;});}
int Cyc_Tcutil_fntype_att( void* a){ void* _temp2061= a; int _temp2075; _LL2063:
if(( unsigned int) _temp2061 > 15u?(( struct _tunion_struct*) _temp2061)->tag ==
Cyc_Absyn_Regparm_att_tag: 0){ _LL2076: _temp2075=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp2061)->f1; goto _LL2064;} else{ goto _LL2065;} _LL2065: if(( int) _temp2061
== Cyc_Absyn_Stdcall_att){ goto _LL2066;} else{ goto _LL2067;} _LL2067: if(( int)
_temp2061 == Cyc_Absyn_Cdecl_att){ goto _LL2068;} else{ goto _LL2069;} _LL2069:
if(( int) _temp2061 == Cyc_Absyn_Noreturn_att){ goto _LL2070;} else{ goto
_LL2071;} _LL2071: if(( int) _temp2061 == Cyc_Absyn_Const_att){ goto _LL2072;}
else{ goto _LL2073;} _LL2073: goto _LL2074; _LL2064: goto _LL2066; _LL2066: goto
_LL2068; _LL2068: goto _LL2070; _LL2070: goto _LL2072; _LL2072: return 1;
_LL2074: return 0; _LL2062:;} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*
fd){ if( fd->cached_typ == 0){ struct Cyc_List_List* fn_type_atts= 0;{ struct
Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List*
_temp2077= atts; if( _temp2077 == 0){ _throw( Null_Exception);} _temp2077->tl;})){
if( Cyc_Tcutil_fntype_att(( void*)({ struct Cyc_List_List* _temp2078= atts; if(
_temp2078 == 0){ _throw( Null_Exception);} _temp2078->hd;}))){ fn_type_atts=({
struct Cyc_List_List* _temp2079=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2079->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp2080= atts; if( _temp2080 == 0){ _throw( Null_Exception);} _temp2080->hd;}));
_temp2079->tl= fn_type_atts; _temp2079;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2081=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2081[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2082; _temp2082.tag= Cyc_Absyn_FnType_tag;
_temp2082.f1=({ struct Cyc_Absyn_FnInfo _temp2083; _temp2083.tvars= fd->tvs;
_temp2083.effect= fd->effect; _temp2083.ret_typ=( void*)(( void*) fd->ret_type);
_temp2083.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple11*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp2083.varargs= fd->varargs; _temp2083.attributes= fn_type_atts; _temp2083;});
_temp2082;}); _temp2081;});} return( void*)({ struct Cyc_Core_Opt* _temp2084= fd->cached_typ;
if( _temp2084 == 0){ _throw( Null_Exception);} _temp2084->v;});} static void*
Cyc_Tcutil_fst3( struct _tuple12* t){ return(* t).f1;} void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl*
fd){ void* t= Cyc_Tcutil_fndecl2typ( fd); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ void* _temp2085= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp2091; struct Cyc_List_List* _temp2093; int _temp2095; struct Cyc_List_List*
_temp2097; void* _temp2099; struct Cyc_Core_Opt* _temp2101; struct Cyc_List_List*
_temp2103; _LL2087: if(( unsigned int) _temp2085 > 4u?(( struct _tunion_struct*)
_temp2085)->tag == Cyc_Absyn_FnType_tag: 0){ _LL2092: _temp2091=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp2085)->f1; _LL2104: _temp2103=( struct Cyc_List_List*)
_temp2091.tvars; goto _LL2102; _LL2102: _temp2101=( struct Cyc_Core_Opt*)
_temp2091.effect; goto _LL2100; _LL2100: _temp2099=( void*) _temp2091.ret_typ;
goto _LL2098; _LL2098: _temp2097=( struct Cyc_List_List*) _temp2091.args; goto
_LL2096; _LL2096: _temp2095=( int) _temp2091.varargs; goto _LL2094; _LL2094:
_temp2093=( struct Cyc_List_List*) _temp2091.attributes; goto _LL2088;} else{
goto _LL2089;} _LL2089: goto _LL2090; _LL2088: fd->tvs= _temp2103; fd->effect=
_temp2101; goto _LL2086; _LL2090:(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp2105=( char*)"check_fndecl_valid_type: not a FnType";
struct _tagged_string _temp2106; _temp2106.curr= _temp2105; _temp2106.base=
_temp2105; _temp2106.last_plus_one= _temp2105 + 38; _temp2106;})); return;
_LL2086:;} Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct
_tagged_string*(* f)( struct _tuple11*), struct Cyc_List_List* x)) Cyc_List_map)((
struct _tagged_string*(*)( struct _tuple11* t)) Cyc_Tcutil_fst3, fd->args), loc,(
struct _tagged_string)({ char* _temp2107=( char*)"function declaration has repeated parameter";
struct _tagged_string _temp2108; _temp2108.curr= _temp2107; _temp2108.base=
_temp2107; _temp2108.last_plus_one= _temp2107 + 44; _temp2108;})); fd->cached_typ=({
struct Cyc_Core_Opt* _temp2109=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2109->v=( void*) t; _temp2109;});} static struct _tuple14*
Cyc_Tcutil_substitute_f1( struct _tuple2* y){ return({ struct _tuple14*
_temp2110=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2110->f1=({
struct _tuple13* _temp2111=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp2111->f1=(* y).f1; _temp2111->f2=(* y).f2; _temp2111;});
_temp2110->f2=(* y).f3; _temp2110;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple14* w){ void* _temp2114; struct _tuple13* _temp2116; struct
_tuple14 _temp2112=* w; _LL2117: _temp2116= _temp2112.f1; goto _LL2115; _LL2115:
_temp2114= _temp2112.f2; goto _LL2113; _LL2113: { struct Cyc_Absyn_Tqual*
_temp2120; struct Cyc_Core_Opt* _temp2122; struct _tuple13 _temp2118=* _temp2116;
_LL2123: _temp2122= _temp2118.f1; goto _LL2121; _LL2121: _temp2120= _temp2118.f2;
goto _LL2119; _LL2119: return({ struct _tuple2* _temp2124=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp2124->f1= _temp2122; _temp2124->f2=
_temp2120; _temp2124->f3= _temp2114; _temp2124;});}} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct Cyc_List_List* inst, struct Cyc_List_List* ts); void*
Cyc_Tcutil_substitute( struct Cyc_List_List* inst, void* t){ void* _temp2125=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp2171; struct Cyc_Absyn_Structdecl**
_temp2173; struct Cyc_List_List* _temp2175; struct _tuple1* _temp2177; struct
Cyc_Absyn_Uniondecl** _temp2179; struct Cyc_List_List* _temp2181; struct _tuple1*
_temp2183; struct Cyc_Absyn_TunionInfo _temp2185; struct Cyc_Absyn_Tuniondecl*
_temp2187; void* _temp2189; struct Cyc_List_List* _temp2191; struct _tuple1*
_temp2193; struct Cyc_Absyn_XTunionInfo _temp2195; struct Cyc_Absyn_XTuniondecl*
_temp2197; void* _temp2199; struct _tuple1* _temp2201; struct Cyc_Absyn_TunionFieldInfo
_temp2203; struct Cyc_Absyn_Tunionfield* _temp2205; struct Cyc_Absyn_Tuniondecl*
_temp2207; struct _tuple1* _temp2209; struct Cyc_List_List* _temp2211; struct
_tuple1* _temp2213; struct Cyc_Absyn_XTunionFieldInfo _temp2215; struct Cyc_Core_Opt*
_temp2217; struct Cyc_List_List* _temp2219; struct _tuple1* _temp2221; struct
Cyc_Absyn_Exp* _temp2223; struct Cyc_Absyn_Tqual* _temp2225; void* _temp2227;
struct Cyc_Absyn_PtrInfo _temp2229; struct Cyc_Absyn_Conref* _temp2231; struct
Cyc_Absyn_Tqual* _temp2233; struct Cyc_Absyn_Conref* _temp2235; void* _temp2237;
void* _temp2239; struct Cyc_Absyn_FnInfo _temp2241; struct Cyc_List_List*
_temp2243; int _temp2245; struct Cyc_List_List* _temp2247; void* _temp2249;
struct Cyc_Core_Opt* _temp2251; struct Cyc_List_List* _temp2253; struct Cyc_List_List*
_temp2255; int _temp2257; struct Cyc_Core_Opt* _temp2259; void* _temp2261; void*
_temp2263; struct Cyc_Absyn_Enumdecl* _temp2265; struct _tuple1* _temp2267; void*
_temp2269; void* _temp2271; void* _temp2273; struct Cyc_List_List* _temp2275;
_LL2127: if(( unsigned int) _temp2125 > 4u?(( struct _tunion_struct*) _temp2125)->tag
== Cyc_Absyn_VarType_tag: 0){ _LL2172: _temp2171=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp2125)->f1; goto _LL2128;} else{ goto
_LL2129;} _LL2129: if(( unsigned int) _temp2125 > 4u?(( struct _tunion_struct*)
_temp2125)->tag == Cyc_Absyn_StructType_tag: 0){ _LL2178: _temp2177=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2125)->f1; goto _LL2176;
_LL2176: _temp2175=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2125)->f2; goto _LL2174; _LL2174: _temp2173=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2125)->f3; goto _LL2130;} else{ goto
_LL2131;} _LL2131: if(( unsigned int) _temp2125 > 4u?(( struct _tunion_struct*)
_temp2125)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL2184: _temp2183=( struct
_tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp2125)->f1; goto _LL2182;
_LL2182: _temp2181=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2125)->f2; goto _LL2180; _LL2180: _temp2179=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2125)->f3; goto _LL2132;} else{ goto
_LL2133;} _LL2133: if(( unsigned int) _temp2125 > 4u?(( struct _tunion_struct*)
_temp2125)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL2186: _temp2185=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp2125)->f1; _LL2194: _temp2193=( struct
_tuple1*) _temp2185.name; goto _LL2192; _LL2192: _temp2191=( struct Cyc_List_List*)
_temp2185.targs; goto _LL2190; _LL2190: _temp2189=( void*) _temp2185.rgn; goto
_LL2188; _LL2188: _temp2187=( struct Cyc_Absyn_Tuniondecl*) _temp2185.tud; goto
_LL2134;} else{ goto _LL2135;} _LL2135: if(( unsigned int) _temp2125 > 4u?((
struct _tunion_struct*) _temp2125)->tag == Cyc_Absyn_XTunionType_tag: 0){
_LL2196: _temp2195=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp2125)->f1; _LL2202: _temp2201=( struct _tuple1*) _temp2195.name; goto
_LL2200; _LL2200: _temp2199=( void*) _temp2195.rgn; goto _LL2198; _LL2198:
_temp2197=( struct Cyc_Absyn_XTuniondecl*) _temp2195.xtud; goto _LL2136;} else{
goto _LL2137;} _LL2137: if(( unsigned int) _temp2125 > 4u?(( struct
_tunion_struct*) _temp2125)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL2204:
_temp2203=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2125)->f1; _LL2214: _temp2213=( struct _tuple1*) _temp2203.name; goto
_LL2212; _LL2212: _temp2211=( struct Cyc_List_List*) _temp2203.targs; goto
_LL2210; _LL2210: _temp2209=( struct _tuple1*) _temp2203.fname; goto _LL2208;
_LL2208: _temp2207=( struct Cyc_Absyn_Tuniondecl*) _temp2203.tud; goto _LL2206;
_LL2206: _temp2205=( struct Cyc_Absyn_Tunionfield*) _temp2203.tufd; goto _LL2138;}
else{ goto _LL2139;} _LL2139: if(( unsigned int) _temp2125 > 4u?(( struct
_tunion_struct*) _temp2125)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL2216:
_temp2215=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp2125)->f1; goto _LL2140;} else{ goto _LL2141;} _LL2141: if(( unsigned int)
_temp2125 > 4u?(( struct _tunion_struct*) _temp2125)->tag == Cyc_Absyn_TypedefType_tag:
0){ _LL2222: _temp2221=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2125)->f1; goto _LL2220; _LL2220: _temp2219=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2125)->f2; goto _LL2218; _LL2218:
_temp2217=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2125)->f3; goto _LL2142;} else{ goto _LL2143;} _LL2143: if(( unsigned int)
_temp2125 > 4u?(( struct _tunion_struct*) _temp2125)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2228: _temp2227=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2125)->f1;
goto _LL2226; _LL2226: _temp2225=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2125)->f2; goto _LL2224; _LL2224: _temp2223=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2125)->f3; goto _LL2144;} else{ goto
_LL2145;} _LL2145: if(( unsigned int) _temp2125 > 4u?(( struct _tunion_struct*)
_temp2125)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2230: _temp2229=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2125)->f1;
_LL2240: _temp2239=( void*) _temp2229.elt_typ; goto _LL2238; _LL2238: _temp2237=(
void*) _temp2229.rgn_typ; goto _LL2236; _LL2236: _temp2235=( struct Cyc_Absyn_Conref*)
_temp2229.nullable; goto _LL2234; _LL2234: _temp2233=( struct Cyc_Absyn_Tqual*)
_temp2229.tq; goto _LL2232; _LL2232: _temp2231=( struct Cyc_Absyn_Conref*)
_temp2229.bounds; goto _LL2146;} else{ goto _LL2147;} _LL2147: if(( unsigned int)
_temp2125 > 4u?(( struct _tunion_struct*) _temp2125)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2242: _temp2241=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2125)->f1; _LL2254: _temp2253=( struct Cyc_List_List*) _temp2241.tvars;
goto _LL2252; _LL2252: _temp2251=( struct Cyc_Core_Opt*) _temp2241.effect; goto
_LL2250; _LL2250: _temp2249=( void*) _temp2241.ret_typ; goto _LL2248; _LL2248:
_temp2247=( struct Cyc_List_List*) _temp2241.args; goto _LL2246; _LL2246:
_temp2245=( int) _temp2241.varargs; goto _LL2244; _LL2244: _temp2243=( struct
Cyc_List_List*) _temp2241.attributes; goto _LL2148;} else{ goto _LL2149;}
_LL2149: if(( unsigned int) _temp2125 > 4u?(( struct _tunion_struct*) _temp2125)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL2256: _temp2255=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp2125)->f1; goto _LL2150;} else{ goto
_LL2151;} _LL2151: if(( unsigned int) _temp2125 > 4u?(( struct _tunion_struct*)
_temp2125)->tag == Cyc_Absyn_Evar_tag: 0){ _LL2262: _temp2261=( void*)(( struct
Cyc_Absyn_Evar_struct*) _temp2125)->f1; goto _LL2260; _LL2260: _temp2259=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp2125)->f2; goto
_LL2258; _LL2258: _temp2257=( int)(( struct Cyc_Absyn_Evar_struct*) _temp2125)->f3;
goto _LL2152;} else{ goto _LL2153;} _LL2153: if(( unsigned int) _temp2125 > 4u?((
struct _tunion_struct*) _temp2125)->tag == Cyc_Absyn_RgnHandleType_tag: 0){
_LL2264: _temp2263=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2125)->f1;
goto _LL2154;} else{ goto _LL2155;} _LL2155: if(( unsigned int) _temp2125 > 4u?((
struct _tunion_struct*) _temp2125)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL2268:
_temp2267=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp2125)->f1;
goto _LL2266; _LL2266: _temp2265=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp2125)->f2; goto _LL2156;} else{ goto _LL2157;} _LL2157: if(( int) _temp2125
== Cyc_Absyn_VoidType){ goto _LL2158;} else{ goto _LL2159;} _LL2159: if((
unsigned int) _temp2125 > 4u?(( struct _tunion_struct*) _temp2125)->tag == Cyc_Absyn_IntType_tag:
0){ _LL2272: _temp2271=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2125)->f1;
goto _LL2270; _LL2270: _temp2269=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2125)->f2; goto _LL2160;} else{ goto _LL2161;} _LL2161: if(( int) _temp2125
== Cyc_Absyn_FloatType){ goto _LL2162;} else{ goto _LL2163;} _LL2163: if(( int)
_temp2125 == Cyc_Absyn_DoubleType){ goto _LL2164;} else{ goto _LL2165;} _LL2165:
if(( int) _temp2125 == Cyc_Absyn_HeapRgn){ goto _LL2166;} else{ goto _LL2167;}
_LL2167: if(( unsigned int) _temp2125 > 4u?(( struct _tunion_struct*) _temp2125)->tag
== Cyc_Absyn_AccessEff_tag: 0){ _LL2274: _temp2273=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2125)->f1; goto _LL2168;} else{ goto _LL2169;} _LL2169: if(( unsigned int)
_temp2125 > 4u?(( struct _tunion_struct*) _temp2125)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL2276: _temp2275=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp2125)->f1; goto _LL2170;} else{ goto _LL2126;} _LL2128: { struct
_handler_cons _temp2277; _push_handler(& _temp2277);{ struct _xtunion_struct*
_temp2278=( struct _xtunion_struct*) setjmp( _temp2277.handler); if( ! _temp2278){{
void* _temp2279=(( void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2171); _npop_handler( 0u); return _temp2279;}; _pop_handler();} else{
struct _xtunion_struct* _temp2281= _temp2278; _LL2283: if( _temp2281->tag == Cyc_Core_Not_found_tag){
goto _LL2284;} else{ goto _LL2285;} _LL2285: goto _LL2286; _LL2284: return t;
_LL2286:( void) _throw( _temp2281); _LL2282:;}}} _LL2130: { struct Cyc_List_List*
new_ts= Cyc_Tcutil_substs( inst, _temp2175); return new_ts == _temp2175? t:(
void*)({ struct Cyc_Absyn_StructType_struct* _temp2287=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2287[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2288; _temp2288.tag= Cyc_Absyn_StructType_tag;
_temp2288.f1= _temp2177; _temp2288.f2= new_ts; _temp2288.f3= _temp2173;
_temp2288;}); _temp2287;});} _LL2132: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs(
inst, _temp2181); return new_ts == _temp2181? t:( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp2289=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2289[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2290; _temp2290.tag= Cyc_Absyn_UnionType_tag;
_temp2290.f1= _temp2183; _temp2290.f2= new_ts; _temp2290.f3= _temp2179;
_temp2290;}); _temp2289;});} _LL2134: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs(
inst, _temp2191); void* new_r= Cyc_Tcutil_substitute( inst, _temp2189); return(
new_ts == _temp2191? new_r == _temp2189: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2291=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2291[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2292; _temp2292.tag=
Cyc_Absyn_TunionType_tag; _temp2292.f1=({ struct Cyc_Absyn_TunionInfo _temp2293;
_temp2293.name= _temp2193; _temp2293.targs= new_ts; _temp2293.rgn=( void*) new_r;
_temp2293.tud= _temp2187; _temp2293;}); _temp2292;}); _temp2291;});} _LL2136: {
void* new_r= Cyc_Tcutil_substitute( inst, _temp2199); return new_r == _temp2199?
t:( void*)({ struct Cyc_Absyn_XTunionType_struct* _temp2294=( struct Cyc_Absyn_XTunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct)); _temp2294[ 0]=({
struct Cyc_Absyn_XTunionType_struct _temp2295; _temp2295.tag= Cyc_Absyn_XTunionType_tag;
_temp2295.f1=({ struct Cyc_Absyn_XTunionInfo _temp2296; _temp2296.name=
_temp2201; _temp2296.rgn=( void*) new_r; _temp2296.xtud= _temp2197; _temp2296;});
_temp2295;}); _temp2294;});} _LL2138: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs(
inst, _temp2211); return new_ts == _temp2211? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2297=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2297[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2298; _temp2298.tag= Cyc_Absyn_TunionFieldType_tag; _temp2298.f1=({ struct
Cyc_Absyn_TunionFieldInfo _temp2299; _temp2299.name= _temp2213; _temp2299.targs=
new_ts; _temp2299.fname= _temp2209; _temp2299.tud= _temp2207; _temp2299.tufd=
_temp2205; _temp2299;}); _temp2298;}); _temp2297;});} _LL2140: return t; _LL2142: {
struct Cyc_List_List* new_ts= Cyc_Tcutil_substs( inst, _temp2219); return new_ts
== _temp2219? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2300=(
struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2300[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2301; _temp2301.tag=
Cyc_Absyn_TypedefType_tag; _temp2301.f1= _temp2221; _temp2301.f2= new_ts;
_temp2301.f3= _temp2217; _temp2301;}); _temp2300;});} _LL2144: { void* new_t1=
Cyc_Tcutil_substitute( inst, _temp2227); return new_t1 == _temp2227? t:( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp2302=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2302[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2303; _temp2303.tag= Cyc_Absyn_ArrayType_tag;
_temp2303.f1=( void*) new_t1; _temp2303.f2= _temp2225; _temp2303.f3= _temp2223;
_temp2303;}); _temp2302;});} _LL2146: { void* new_t1= Cyc_Tcutil_substitute(
inst, _temp2239); void* new_r= Cyc_Tcutil_substitute( inst, _temp2237); if(
new_t1 == _temp2239? new_r == _temp2237: 0){ return t;} return( void*)({ struct
Cyc_Absyn_PointerType_struct* _temp2304=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2304[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2305; _temp2305.tag= Cyc_Absyn_PointerType_tag;
_temp2305.f1=({ struct Cyc_Absyn_PtrInfo _temp2306; _temp2306.elt_typ=( void*)
new_t1; _temp2306.rgn_typ=( void*) new_r; _temp2306.nullable= _temp2235;
_temp2306.tq= _temp2233; _temp2306.bounds= _temp2231; _temp2306;}); _temp2305;});
_temp2304;});} _LL2148:{ struct Cyc_List_List* p= _temp2253; for( 0; p != 0; p=({
struct Cyc_List_List* _temp2307= p; if( _temp2307 == 0){ _throw( Null_Exception);}
_temp2307->tl;})){ inst=({ struct Cyc_List_List* _temp2308=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2308->hd=( void*)({ struct
_tuple5* _temp2309=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2309->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2313= p;
if( _temp2313 == 0){ _throw( Null_Exception);} _temp2313->hd;}); _temp2309->f2=(
void*)({ struct Cyc_Absyn_VarType_struct* _temp2310=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2310[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2311; _temp2311.tag= Cyc_Absyn_VarType_tag;
_temp2311.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2312= p; if(
_temp2312 == 0){ _throw( Null_Exception);} _temp2312->hd;}); _temp2311;});
_temp2310;}); _temp2309;}); _temp2308->tl= inst; _temp2308;});}}{ struct Cyc_List_List*
_temp2316; struct Cyc_List_List* _temp2318; struct _tuple0 _temp2314=(( struct
_tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)((( struct Cyc_List_List*(*)(
struct _tuple14*(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f1, _temp2247)); _LL2319: _temp2318= _temp2314.f1; goto
_LL2317; _LL2317: _temp2316= _temp2314.f2; goto _LL2315; _LL2315: { struct Cyc_List_List*
ts2= Cyc_Tcutil_substs( inst, _temp2316); struct Cyc_List_List* args2=(( struct
Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple14*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp2318, ts2));
struct Cyc_Core_Opt* eff2; if( _temp2251 == 0){ eff2= 0;} else{ void* new_eff=
Cyc_Tcutil_substitute( inst,( void*)({ struct Cyc_Core_Opt* _temp2323= _temp2251;
if( _temp2323 == 0){ _throw( Null_Exception);} _temp2323->v;})); if( new_eff ==(
void*)({ struct Cyc_Core_Opt* _temp2320= _temp2251; if( _temp2320 == 0){ _throw(
Null_Exception);} _temp2320->v;})){ eff2= _temp2251;} else{ eff2=({ struct Cyc_Core_Opt*
_temp2321=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp2321[ 0]=({ struct Cyc_Core_Opt _temp2322; _temp2322.v=( void*) new_eff;
_temp2322;}); _temp2321;});}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2324=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2324[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2325; _temp2325.tag= Cyc_Absyn_FnType_tag;
_temp2325.f1=({ struct Cyc_Absyn_FnInfo _temp2326; _temp2326.tvars= _temp2253;
_temp2326.effect= eff2; _temp2326.ret_typ=( void*) Cyc_Tcutil_substitute( inst,
_temp2249); _temp2326.args= args2; _temp2326.varargs= _temp2245; _temp2326.attributes=
_temp2243; _temp2326;}); _temp2325;}); _temp2324;});}} _LL2150: { struct Cyc_List_List*
_temp2329; struct Cyc_List_List* _temp2331; struct _tuple0 _temp2327=(( struct
_tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( _temp2255); _LL2332:
_temp2331= _temp2327.f1; goto _LL2330; _LL2330: _temp2329= _temp2327.f2; goto
_LL2328; _LL2328: { struct Cyc_List_List* ts2= Cyc_Tcutil_substs( inst,
_temp2329); if( ts2 == _temp2329){ return t;}{ struct Cyc_List_List* tqts2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp2331, ts2); return( void*)({ struct Cyc_Absyn_TupleType_struct* _temp2333=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2333[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2334; _temp2334.tag= Cyc_Absyn_TupleType_tag;
_temp2334.f1= tqts2; _temp2334;}); _temp2333;});}}} _LL2152: if( _temp2259 != 0){
return Cyc_Tcutil_substitute( inst,( void*)({ struct Cyc_Core_Opt* _temp2335=
_temp2259; if( _temp2335 == 0){ _throw( Null_Exception);} _temp2335->v;}));}
else{ return t;} _LL2154: { void* new_rt= Cyc_Tcutil_substitute( inst, _temp2263);
return new_rt == _temp2263? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2336=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2336[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2337; _temp2337.tag= Cyc_Absyn_RgnHandleType_tag; _temp2337.f1=( void*)
new_rt; _temp2337;}); _temp2336;});} _LL2156: return t; _LL2158: return t;
_LL2160: return t; _LL2162: return t; _LL2164: return t; _LL2166: return t;
_LL2168: { void* new_r= Cyc_Tcutil_substitute( inst, _temp2273); return new_r ==
_temp2273? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2338=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2338[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2339; _temp2339.tag= Cyc_Absyn_AccessEff_tag;
_temp2339.f1=( void*) new_r; _temp2339;}); _temp2338;});} _LL2170: { struct Cyc_List_List*
new_es= Cyc_Tcutil_substs( inst, _temp2275); return new_es == _temp2275? t:(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2340=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2340[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2341; _temp2341.tag= Cyc_Absyn_JoinEff_tag;
_temp2341.f1= new_es; _temp2341;}); _temp2340;});} _LL2126:;} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct Cyc_List_List* inst, struct Cyc_List_List* ts){ if( ts
== 0){ return 0;}{ void* old_hd=( void*)({ struct Cyc_List_List* _temp2345= ts;
if( _temp2345 == 0){ _throw( Null_Exception);} _temp2345->hd;}); struct Cyc_List_List*
old_tl=({ struct Cyc_List_List* _temp2344= ts; if( _temp2344 == 0){ _throw(
Null_Exception);} _temp2344->tl;}); void* new_hd= Cyc_Tcutil_substitute( inst,
old_hd); struct Cyc_List_List* new_tl= Cyc_Tcutil_substs( inst, old_tl); if(
old_hd == new_hd? old_tl == new_tl: 0){ return ts;} return({ struct Cyc_List_List*
_temp2342=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2342[ 0]=({ struct Cyc_List_List _temp2343; _temp2343.hd=( void*) new_hd;
_temp2343.tl= new_tl; _temp2343;}); _temp2342;});}} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer(
struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_Position_Segment* loc){ struct
Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2395=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2395[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2396; _temp2396.tag= Cyc_Absyn_Const_e_tag;
_temp2396.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp2396;}); _temp2395;}),
loc);{ void* _temp2346= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp2360; void* _temp2362; void* _temp2364; void* _temp2366; void* _temp2368;
_LL2348: if(( unsigned int) _temp2346 > 4u?(( struct _tunion_struct*) _temp2346)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL2361: _temp2360=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp2346)->f1; goto _LL2349;} else{ goto
_LL2350;} _LL2350: if(( unsigned int) _temp2346 > 4u?(( struct _tunion_struct*)
_temp2346)->tag == Cyc_Absyn_IntType_tag: 0){ _LL2365: _temp2364=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp2346)->f1; goto _LL2363; _LL2363:
_temp2362=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2346)->f2; if(( int)
_temp2362 == Cyc_Absyn_B1){ goto _LL2351;} else{ goto _LL2352;}} else{ goto
_LL2352;} _LL2352: if(( unsigned int) _temp2346 > 4u?(( struct _tunion_struct*)
_temp2346)->tag == Cyc_Absyn_IntType_tag: 0){ _LL2369: _temp2368=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp2346)->f1; goto _LL2367; _LL2367:
_temp2366=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2346)->f2; goto
_LL2353;} else{ goto _LL2354;} _LL2354: if(( int) _temp2346 == Cyc_Absyn_FloatType){
goto _LL2355;} else{ goto _LL2356;} _LL2356: if(( int) _temp2346 == Cyc_Absyn_DoubleType){
goto _LL2357;} else{ goto _LL2358;} _LL2358: goto _LL2359; _LL2349: goto _LL2347;
_LL2351:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2370=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2370[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2371; _temp2371.tag= Cyc_Absyn_Const_e_tag;
_temp2371.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2372=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2372[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2373; _temp2373.tag= Cyc_Absyn_Char_c_tag;
_temp2373.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2373.f2='\000'; _temp2373;});
_temp2372;})); _temp2371;}); _temp2370;}))); goto _LL2347; _LL2353:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2374=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2374[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2375; _temp2375.tag= Cyc_Absyn_Const_e_tag;
_temp2375.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2376=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2376[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2377; _temp2377.tag= Cyc_Absyn_Int_c_tag;
_temp2377.f1=( void*) _temp2368; _temp2377.f2= 0; _temp2377;}); _temp2376;}));
_temp2375;}); _temp2374;}))); if( _temp2366 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2378=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2378[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2379; _temp2379.tag= Cyc_Absyn_Cast_e_tag; _temp2379.f1=( void*) t;
_temp2379.f2= e; _temp2379;}); _temp2378;}), loc);} goto _LL2347; _LL2355:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2380=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2380[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2381; _temp2381.tag= Cyc_Absyn_Const_e_tag;
_temp2381.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2382=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2382[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2383; _temp2383.tag= Cyc_Absyn_Float_c_tag;
_temp2383.f1=( struct _tagged_string)({ char* _temp2384=( char*)"0.0"; struct
_tagged_string _temp2385; _temp2385.curr= _temp2384; _temp2385.base= _temp2384;
_temp2385.last_plus_one= _temp2384 + 4; _temp2385;}); _temp2383;}); _temp2382;}));
_temp2381;}); _temp2380;}))); goto _LL2347; _LL2357:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2386=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2386[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2387; _temp2387.tag= Cyc_Absyn_Cast_e_tag; _temp2387.f1=( void*) t;
_temp2387.f2= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2388=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2388[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2389; _temp2389.tag= Cyc_Absyn_Const_e_tag;
_temp2389.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2390=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2390[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2391; _temp2391.tag= Cyc_Absyn_Float_c_tag;
_temp2391.f1=( struct _tagged_string)({ char* _temp2392=( char*)"0.0"; struct
_tagged_string _temp2393; _temp2393.curr= _temp2392; _temp2393.base= _temp2392;
_temp2393.last_plus_one= _temp2392 + 4; _temp2393;}); _temp2391;}); _temp2390;}));
_temp2389;}); _temp2388;}), loc); _temp2387;}); _temp2386;}))); goto _LL2347;
_LL2359: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2394= Cyc_Absynpp_typ2string(
t); xprintf("declaration of type %.*s requires initializer", _temp2394.last_plus_one
- _temp2394.curr, _temp2394.curr);})); goto _LL2347; _LL2347:;} return e;}
struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar* tv){ void* k=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind); return({
struct _tuple5* _temp2397=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2397->f1= tv; _temp2397->f2= Cyc_Absyn_new_evar( k); _temp2397;});} struct
Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* free_vars, void* expected_kind,
void* t){{ void* _temp2398= Cyc_Tcutil_compress( t); int _temp2444; struct Cyc_Core_Opt*
_temp2446; void* _temp2448; struct Cyc_Absyn_Tvar* _temp2450; struct Cyc_Absyn_Enumdecl*
_temp2452; struct Cyc_Absyn_Enumdecl** _temp2454; struct _tuple1* _temp2455;
struct Cyc_Absyn_TunionInfo _temp2457; struct Cyc_Absyn_Tuniondecl* _temp2459;
struct Cyc_Absyn_Tuniondecl** _temp2461; void* _temp2462; struct Cyc_List_List*
_temp2464; struct _tuple1* _temp2466; struct Cyc_Absyn_XTunionInfo _temp2468;
struct Cyc_Absyn_XTuniondecl* _temp2470; struct Cyc_Absyn_XTuniondecl**
_temp2472; void* _temp2473; struct _tuple1* _temp2475; struct Cyc_Absyn_TunionFieldInfo
_temp2477; struct Cyc_Absyn_Tunionfield* _temp2479; struct Cyc_Absyn_Tunionfield**
_temp2481; struct Cyc_Absyn_Tuniondecl* _temp2482; struct Cyc_Absyn_Tuniondecl**
_temp2484; struct _tuple1* _temp2485; struct Cyc_List_List* _temp2487; struct
_tuple1* _temp2489; struct Cyc_Absyn_XTunionFieldInfo _temp2491; struct Cyc_Absyn_Tunionfield*
_temp2493; struct Cyc_Absyn_Tunionfield** _temp2495; struct Cyc_Absyn_XTuniondecl*
_temp2496; struct Cyc_Absyn_XTuniondecl** _temp2498; struct _tuple1* _temp2499;
struct _tuple1* _temp2501; struct Cyc_Absyn_PtrInfo _temp2503; struct Cyc_Absyn_Conref*
_temp2505; struct Cyc_Absyn_Tqual* _temp2507; struct Cyc_Absyn_Conref* _temp2509;
void* _temp2511; void* _temp2513; void* _temp2515; void* _temp2517; struct Cyc_Absyn_Exp*
_temp2519; struct Cyc_Absyn_Tqual* _temp2521; void* _temp2523; struct Cyc_Absyn_FnInfo
_temp2525; struct Cyc_List_List* _temp2527; int _temp2529; struct Cyc_List_List*
_temp2531; void* _temp2533; struct Cyc_Core_Opt* _temp2535; struct Cyc_Core_Opt**
_temp2537; struct Cyc_List_List* _temp2538; struct Cyc_List_List** _temp2540;
struct Cyc_List_List* _temp2541; struct Cyc_Absyn_Structdecl** _temp2543; struct
Cyc_Absyn_Structdecl*** _temp2545; struct Cyc_List_List* _temp2546; struct
_tuple1* _temp2548; struct Cyc_Absyn_Uniondecl** _temp2550; struct Cyc_Absyn_Uniondecl***
_temp2552; struct Cyc_List_List* _temp2553; struct _tuple1* _temp2555; struct
Cyc_Core_Opt* _temp2557; struct Cyc_Core_Opt** _temp2559; struct Cyc_List_List*
_temp2560; struct _tuple1* _temp2562; void* _temp2564; void* _temp2566; struct
Cyc_List_List* _temp2568; _LL2400: if(( int) _temp2398 == Cyc_Absyn_VoidType){
goto _LL2401;} else{ goto _LL2402;} _LL2402: if(( unsigned int) _temp2398 > 4u?((
struct _tunion_struct*) _temp2398)->tag == Cyc_Absyn_Evar_tag: 0){ _LL2449:
_temp2448=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp2398)->f1; goto _LL2447;
_LL2447: _temp2446=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp2398)->f2; goto _LL2445; _LL2445: _temp2444=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp2398)->f3; goto _LL2403;} else{ goto _LL2404;} _LL2404: if(( unsigned int)
_temp2398 > 4u?(( struct _tunion_struct*) _temp2398)->tag == Cyc_Absyn_VarType_tag:
0){ _LL2451: _temp2450=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp2398)->f1; goto _LL2405;} else{ goto _LL2406;} _LL2406: if(( unsigned int)
_temp2398 > 4u?(( struct _tunion_struct*) _temp2398)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL2456: _temp2455=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp2398)->f1; goto _LL2453; _LL2453: _temp2452=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp2398)->f2; _temp2454=&(( struct Cyc_Absyn_EnumType_struct*)
_temp2398)->f2; goto _LL2407;} else{ goto _LL2408;} _LL2408: if(( unsigned int)
_temp2398 > 4u?(( struct _tunion_struct*) _temp2398)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL2458: _temp2457=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp2398)->f1; _LL2467: _temp2466=( struct _tuple1*) _temp2457.name; goto
_LL2465; _LL2465: _temp2464=( struct Cyc_List_List*) _temp2457.targs; goto
_LL2463; _LL2463: _temp2462=( void*) _temp2457.rgn; goto _LL2460; _LL2460:
_temp2459=( struct Cyc_Absyn_Tuniondecl*) _temp2457.tud; _temp2461=&((( struct
Cyc_Absyn_TunionType_struct*) _temp2398)->f1).tud; goto _LL2409;} else{ goto
_LL2410;} _LL2410: if(( unsigned int) _temp2398 > 4u?(( struct _tunion_struct*)
_temp2398)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL2469: _temp2468=( struct
Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*) _temp2398)->f1;
_LL2476: _temp2475=( struct _tuple1*) _temp2468.name; goto _LL2474; _LL2474:
_temp2473=( void*) _temp2468.rgn; goto _LL2471; _LL2471: _temp2470=( struct Cyc_Absyn_XTuniondecl*)
_temp2468.xtud; _temp2472=&((( struct Cyc_Absyn_XTunionType_struct*) _temp2398)->f1).xtud;
goto _LL2411;} else{ goto _LL2412;} _LL2412: if(( unsigned int) _temp2398 > 4u?((
struct _tunion_struct*) _temp2398)->tag == Cyc_Absyn_TunionFieldType_tag: 0){
_LL2478: _temp2477=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2398)->f1; _LL2490: _temp2489=( struct _tuple1*) _temp2477.name; goto
_LL2488; _LL2488: _temp2487=( struct Cyc_List_List*) _temp2477.targs; goto
_LL2486; _LL2486: _temp2485=( struct _tuple1*) _temp2477.fname; goto _LL2483;
_LL2483: _temp2482=( struct Cyc_Absyn_Tuniondecl*) _temp2477.tud; _temp2484=&(((
struct Cyc_Absyn_TunionFieldType_struct*) _temp2398)->f1).tud; goto _LL2480;
_LL2480: _temp2479=( struct Cyc_Absyn_Tunionfield*) _temp2477.tufd; _temp2481=&(((
struct Cyc_Absyn_TunionFieldType_struct*) _temp2398)->f1).tufd; goto _LL2413;}
else{ goto _LL2414;} _LL2414: if(( unsigned int) _temp2398 > 4u?(( struct
_tunion_struct*) _temp2398)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL2492:
_temp2491=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp2398)->f1; _LL2502: _temp2501=( struct _tuple1*) _temp2491.name; goto
_LL2500; _LL2500: _temp2499=( struct _tuple1*) _temp2491.fname; goto _LL2497;
_LL2497: _temp2496=( struct Cyc_Absyn_XTuniondecl*) _temp2491.xtud; _temp2498=&(((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp2398)->f1).xtud; goto _LL2494;
_LL2494: _temp2493=( struct Cyc_Absyn_Tunionfield*) _temp2491.xtufd; _temp2495=&(((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp2398)->f1).xtufd; goto _LL2415;}
else{ goto _LL2416;} _LL2416: if(( unsigned int) _temp2398 > 4u?(( struct
_tunion_struct*) _temp2398)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2504:
_temp2503=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2398)->f1; _LL2514: _temp2513=( void*) _temp2503.elt_typ; goto _LL2512;
_LL2512: _temp2511=( void*) _temp2503.rgn_typ; goto _LL2510; _LL2510: _temp2509=(
struct Cyc_Absyn_Conref*) _temp2503.nullable; goto _LL2508; _LL2508: _temp2507=(
struct Cyc_Absyn_Tqual*) _temp2503.tq; goto _LL2506; _LL2506: _temp2505=( struct
Cyc_Absyn_Conref*) _temp2503.bounds; goto _LL2417;} else{ goto _LL2418;} _LL2418:
if(( unsigned int) _temp2398 > 4u?(( struct _tunion_struct*) _temp2398)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL2518: _temp2517=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2398)->f1; goto _LL2516; _LL2516: _temp2515=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2398)->f2; goto _LL2419;} else{ goto _LL2420;} _LL2420: if(( int) _temp2398
== Cyc_Absyn_FloatType){ goto _LL2421;} else{ goto _LL2422;} _LL2422: if(( int)
_temp2398 == Cyc_Absyn_DoubleType){ goto _LL2423;} else{ goto _LL2424;} _LL2424:
if(( unsigned int) _temp2398 > 4u?(( struct _tunion_struct*) _temp2398)->tag ==
Cyc_Absyn_ArrayType_tag: 0){ _LL2524: _temp2523=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2398)->f1; goto _LL2522; _LL2522: _temp2521=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2398)->f2; goto _LL2520; _LL2520:
_temp2519=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2398)->f3; goto _LL2425;} else{ goto _LL2426;} _LL2426: if(( unsigned int)
_temp2398 > 4u?(( struct _tunion_struct*) _temp2398)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2526: _temp2525=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2398)->f1; _LL2539: _temp2538=( struct Cyc_List_List*) _temp2525.tvars;
_temp2540=&((( struct Cyc_Absyn_FnType_struct*) _temp2398)->f1).tvars; goto
_LL2536; _LL2536: _temp2535=( struct Cyc_Core_Opt*) _temp2525.effect; _temp2537=&(((
struct Cyc_Absyn_FnType_struct*) _temp2398)->f1).effect; goto _LL2534; _LL2534:
_temp2533=( void*) _temp2525.ret_typ; goto _LL2532; _LL2532: _temp2531=( struct
Cyc_List_List*) _temp2525.args; goto _LL2530; _LL2530: _temp2529=( int)
_temp2525.varargs; goto _LL2528; _LL2528: _temp2527=( struct Cyc_List_List*)
_temp2525.attributes; goto _LL2427;} else{ goto _LL2428;} _LL2428: if((
unsigned int) _temp2398 > 4u?(( struct _tunion_struct*) _temp2398)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL2542: _temp2541=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp2398)->f1; goto _LL2429;} else{ goto _LL2430;} _LL2430: if(( unsigned int)
_temp2398 > 4u?(( struct _tunion_struct*) _temp2398)->tag == Cyc_Absyn_StructType_tag:
0){ _LL2549: _temp2548=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp2398)->f1; goto _LL2547; _LL2547: _temp2546=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp2398)->f2; goto _LL2544; _LL2544:
_temp2543=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp2398)->f3; _temp2545=&(( struct Cyc_Absyn_StructType_struct*) _temp2398)->f3;
goto _LL2431;} else{ goto _LL2432;} _LL2432: if(( unsigned int) _temp2398 > 4u?((
struct _tunion_struct*) _temp2398)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL2556:
_temp2555=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp2398)->f1;
goto _LL2554; _LL2554: _temp2553=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2398)->f2; goto _LL2551; _LL2551: _temp2550=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2398)->f3; _temp2552=&(( struct Cyc_Absyn_UnionType_struct*)
_temp2398)->f3; goto _LL2433;} else{ goto _LL2434;} _LL2434: if(( unsigned int)
_temp2398 > 4u?(( struct _tunion_struct*) _temp2398)->tag == Cyc_Absyn_TypedefType_tag:
0){ _LL2563: _temp2562=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2398)->f1; goto _LL2561; _LL2561: _temp2560=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2398)->f2; goto _LL2558; _LL2558:
_temp2557=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2398)->f3; _temp2559=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2398)->f3;
goto _LL2435;} else{ goto _LL2436;} _LL2436: if(( int) _temp2398 == Cyc_Absyn_HeapRgn){
goto _LL2437;} else{ goto _LL2438;} _LL2438: if(( unsigned int) _temp2398 > 4u?((
struct _tunion_struct*) _temp2398)->tag == Cyc_Absyn_RgnHandleType_tag: 0){
_LL2565: _temp2564=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2398)->f1;
goto _LL2439;} else{ goto _LL2440;} _LL2440: if(( unsigned int) _temp2398 > 4u?((
struct _tunion_struct*) _temp2398)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL2567:
_temp2566=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2398)->f1; goto
_LL2441;} else{ goto _LL2442;} _LL2442: if(( unsigned int) _temp2398 > 4u?((
struct _tunion_struct*) _temp2398)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL2569:
_temp2568=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp2398)->f1;
goto _LL2443;} else{ goto _LL2399;} _LL2401: goto _LL2399; _LL2403: goto _LL2399;
_LL2405: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2450->kind); if(( void*) c->v ==( void*)
Cyc_Absyn_No_constr){( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2570=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2570[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2571; _temp2571.tag= Cyc_Absyn_Eq_constr_tag;
_temp2571.f1=( void*) expected_kind; _temp2571;}); _temp2570;})));} free_vars=
Cyc_Tcutil_add_free_tvar( loc, free_vars, _temp2450); goto _LL2399;} _LL2407:
if(* _temp2454 == 0){ struct _handler_cons _temp2572; _push_handler(& _temp2572);{
struct _xtunion_struct* _temp2573=( struct _xtunion_struct*) setjmp( _temp2572.handler);
if( ! _temp2573){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2455);* _temp2454=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ struct _xtunion_struct* _temp2575= _temp2573; _LL2577: if( _temp2575->tag
== Cyc_Dict_Absent_tag){ goto _LL2578;} else{ goto _LL2579;} _LL2579: goto
_LL2580; _LL2578: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2581= Cyc_Absynpp_qvar2string(
_temp2455); xprintf("unbound type enum %.*s", _temp2581.last_plus_one -
_temp2581.curr, _temp2581.curr);})); return free_vars; _LL2580:( void) _throw(
_temp2575); _LL2576:;}}}{ struct Cyc_Absyn_Enumdecl* ed=( struct Cyc_Absyn_Enumdecl*)({
struct Cyc_Absyn_Enumdecl* _temp2584=* _temp2454; if( _temp2584 == 0){ _throw(
Null_Exception);} _temp2584;});* _temp2455=({ struct _tuple1* _temp2582= ed->name;
unsigned int _temp2583= 0; if( _temp2583 >= 1u){ _throw( Null_Exception);}
_temp2582[ _temp2583];}); goto _LL2399;} _LL2409: if(* _temp2461 == 0){ struct
_handler_cons _temp2585; _push_handler(& _temp2585);{ struct _xtunion_struct*
_temp2586=( struct _xtunion_struct*) setjmp( _temp2585.handler); if( ! _temp2586){{
struct Cyc_Absyn_Tuniondecl** tud= Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2466); if(( struct Cyc_Absyn_Tuniondecl**) tud == 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2587= Cyc_Absynpp_qvar2string( _temp2466); xprintf("unbound type tunion %.*s",
_temp2587.last_plus_one - _temp2587.curr, _temp2587.curr);}));{ struct Cyc_List_List*
_temp2588= free_vars; _npop_handler( 0u); return _temp2588;}}* _temp2461=(
struct Cyc_Absyn_Tuniondecl*)* tud;}; _pop_handler();} else{ struct
_xtunion_struct* _temp2590= _temp2586; _LL2592: if( _temp2590->tag == Cyc_Dict_Absent_tag){
goto _LL2593;} else{ goto _LL2594;} _LL2594: goto _LL2595; _LL2593: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2596= Cyc_Absynpp_qvar2string( _temp2466);
xprintf("unbound type tunion %.*s", _temp2596.last_plus_one - _temp2596.curr,
_temp2596.curr);})); return free_vars; _LL2595:( void) _throw( _temp2590);
_LL2591:;}}}{ struct Cyc_Absyn_Tuniondecl* tud=( struct Cyc_Absyn_Tuniondecl*)({
struct Cyc_Absyn_Tuniondecl* _temp2606=* _temp2461; if( _temp2606 == 0){ _throw(
Null_Exception);} _temp2606;}); if( tud->name != 0){* _temp2466=({ struct
_tuple1* _temp2598=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp2597= tud->name;
if( _temp2597 == 0){ _throw( Null_Exception);} _temp2597->v;}); unsigned int
_temp2599= 0; if( _temp2599 >= 1u){ _throw( Null_Exception);} _temp2598[
_temp2599];});} free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,(
void*) Cyc_Absyn_RgnKind, _temp2462);{ struct Cyc_List_List* tvs= tud->tvs; for(
0; _temp2464 != 0? tvs != 0: 0; _temp2464=({ struct Cyc_List_List* _temp2600=
_temp2464; if( _temp2600 == 0){ _throw( Null_Exception);} _temp2600->tl;}), tvs=({
struct Cyc_List_List* _temp2601= tvs; if( _temp2601 == 0){ _throw(
Null_Exception);} _temp2601->tl;})){ void* t1=( void*)({ struct Cyc_List_List*
_temp2603= _temp2464; if( _temp2603 == 0){ _throw( Null_Exception);} _temp2603->hd;});
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2602= tvs; if( _temp2602 ==
0){ _throw( Null_Exception);} _temp2602->hd;}))->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2464 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2604= Cyc_Absynpp_qvar2string( _temp2466); xprintf("too many type arguments for tunion %.*s",
_temp2604.last_plus_one - _temp2604.curr, _temp2604.curr);}));} if( tvs != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2605= Cyc_Absynpp_qvar2string(
_temp2466); xprintf("too few type arguments for tunion %.*s", _temp2605.last_plus_one
- _temp2605.curr, _temp2605.curr);}));} goto _LL2399;}} _LL2411: if(* _temp2472
== 0){ struct _handler_cons _temp2607; _push_handler(& _temp2607);{ struct
_xtunion_struct* _temp2608=( struct _xtunion_struct*) setjmp( _temp2607.handler);
if( ! _temp2608){{ struct Cyc_Core_Opt* xtud= Cyc_Tcenv_lookup_xtuniondecl( te,
loc, _temp2475); if( xtud == 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2609= Cyc_Absynpp_qvar2string( _temp2475); xprintf("unbound type xtunion %.*s",
_temp2609.last_plus_one - _temp2609.curr, _temp2609.curr);}));{ struct Cyc_List_List*
_temp2610= free_vars; _npop_handler( 0u); return _temp2610;}}* _temp2472=(
struct Cyc_Absyn_XTuniondecl*)*(( struct Cyc_Absyn_XTuniondecl**)({ struct Cyc_Core_Opt*
_temp2611= xtud; if( _temp2611 == 0){ _throw( Null_Exception);} _temp2611->v;}));};
_pop_handler();} else{ struct _xtunion_struct* _temp2613= _temp2608; _LL2615:
if( _temp2613->tag == Cyc_Dict_Absent_tag){ goto _LL2616;} else{ goto _LL2617;}
_LL2617: goto _LL2618; _LL2616: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2619= Cyc_Absynpp_qvar2string( _temp2475); xprintf("unbound type xtunion %.*s",
_temp2619.last_plus_one - _temp2619.curr, _temp2619.curr);})); return free_vars;
_LL2618:( void) _throw( _temp2613); _LL2614:;}}}{ struct Cyc_Absyn_XTuniondecl*
xtud=( struct Cyc_Absyn_XTuniondecl*)({ struct Cyc_Absyn_XTuniondecl* _temp2620=*
_temp2472; if( _temp2620 == 0){ _throw( Null_Exception);} _temp2620;}); if((
struct _tuple1*) xtud->name != 0){* _temp2475=* xtud->name;} free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_RgnKind, _temp2473); goto _LL2399;}
_LL2413: if(* _temp2484 == 0){ struct _handler_cons _temp2621; _push_handler(&
_temp2621);{ struct _xtunion_struct* _temp2622=( struct _xtunion_struct*) setjmp(
_temp2621.handler); if( ! _temp2622){{ struct Cyc_Absyn_Tuniondecl** tud= Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2489);* _temp2484=( struct Cyc_Absyn_Tuniondecl*)* tud;};
_pop_handler();} else{ struct _xtunion_struct* _temp2624= _temp2622; _LL2626:
if( _temp2624->tag == Cyc_Dict_Absent_tag){ goto _LL2627;} else{ goto _LL2628;}
_LL2628: goto _LL2629; _LL2627: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2630= Cyc_Absynpp_qvar2string( _temp2489); xprintf("unbound type tunion %.*s",
_temp2630.last_plus_one - _temp2630.curr, _temp2630.curr);})); return free_vars;
_LL2629:( void) _throw( _temp2624); _LL2625:;}}} if(* _temp2481 == 0){ struct
_handler_cons _temp2631; _push_handler(& _temp2631);{ struct _xtunion_struct*
_temp2632=( struct _xtunion_struct*) setjmp( _temp2631.handler); if( ! _temp2632){{
void* _temp2633= Cyc_Tcenv_lookup_ordinary( te, loc, _temp2485); struct Cyc_Absyn_Tunionfield*
_temp2639; struct Cyc_Absyn_Tuniondecl* _temp2641; _LL2635: if((( struct
_tunion_struct*) _temp2633)->tag == Cyc_Tcenv_TunionRes_tag){ _LL2642: _temp2641=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp2633)->f1;
goto _LL2640; _LL2640: _temp2639=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp2633)->f2; goto _LL2636;} else{ goto _LL2637;} _LL2637: goto _LL2638;
_LL2636:* _temp2481=( struct Cyc_Absyn_Tunionfield*) _temp2639; if(( struct Cyc_Absyn_Tuniondecl*)
_temp2641 !=* _temp2484){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2643= Cyc_Absynpp_qvar2string( _temp2485); struct _tagged_string _temp2644=
Cyc_Absynpp_qvar2string( _temp2489); xprintf("unbound field %.*s in type tunion %.*s",
_temp2643.last_plus_one - _temp2643.curr, _temp2643.curr, _temp2644.last_plus_one
- _temp2644.curr, _temp2644.curr);}));{ struct Cyc_List_List* _temp2645=
free_vars; _npop_handler( 0u); return _temp2645;}} goto _LL2634; _LL2638: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2646= Cyc_Absynpp_qvar2string( _temp2485);
struct _tagged_string _temp2647= Cyc_Absynpp_qvar2string( _temp2489); xprintf("unbound field %.*s in type tunion %.*s",
_temp2646.last_plus_one - _temp2646.curr, _temp2646.curr, _temp2647.last_plus_one
- _temp2647.curr, _temp2647.curr);}));{ struct Cyc_List_List* _temp2648=
free_vars; _npop_handler( 0u); return _temp2648;} _LL2634:;}; _pop_handler();}
else{ struct _xtunion_struct* _temp2650= _temp2632; _LL2652: if( _temp2650->tag
== Cyc_Dict_Absent_tag){ goto _LL2653;} else{ goto _LL2654;} _LL2654: goto
_LL2655; _LL2653: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2656= Cyc_Absynpp_qvar2string(
_temp2485); struct _tagged_string _temp2657= Cyc_Absynpp_qvar2string( _temp2489);
xprintf("unbound field %.*s in type tunion %.*s", _temp2656.last_plus_one -
_temp2656.curr, _temp2656.curr, _temp2657.last_plus_one - _temp2657.curr,
_temp2657.curr);})); return free_vars; _LL2655:( void) _throw( _temp2650);
_LL2651:;}}}{ struct Cyc_Absyn_Tuniondecl* tud=( struct Cyc_Absyn_Tuniondecl*)({
struct Cyc_Absyn_Tuniondecl* _temp2668=* _temp2484; if( _temp2668 == 0){ _throw(
Null_Exception);} _temp2668;}); struct Cyc_Absyn_Tunionfield* tufd=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_Absyn_Tunionfield* _temp2667=* _temp2481; if( _temp2667 == 0){ _throw(
Null_Exception);} _temp2667;}); if( tud->name != 0){* _temp2489=*(( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp2658= tud->name; if( _temp2658 == 0){
_throw( Null_Exception);} _temp2658->v;}));} if(( struct _tuple1*) tufd->name !=
0){* _temp2485=* tufd->name;}{ struct Cyc_List_List* tvs= tud->tvs; for( 0;
_temp2487 != 0? tvs != 0: 0; _temp2487=({ struct Cyc_List_List* _temp2659=
_temp2487; if( _temp2659 == 0){ _throw( Null_Exception);} _temp2659->tl;}), tvs=({
struct Cyc_List_List* _temp2660= tvs; if( _temp2660 == 0){ _throw(
Null_Exception);} _temp2660->tl;})){ void* t1=( void*)({ struct Cyc_List_List*
_temp2662= _temp2487; if( _temp2662 == 0){ _throw( Null_Exception);} _temp2662->hd;});
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2661= tvs; if( _temp2661 ==
0){ _throw( Null_Exception);} _temp2661->hd;}))->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2487 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2663= Cyc_Absynpp_qvar2string( _temp2489); struct
_tagged_string _temp2664= Cyc_Absynpp_qvar2string( _temp2485); xprintf("too many type arguments for tunion %.*s.%.*s",
_temp2663.last_plus_one - _temp2663.curr, _temp2663.curr, _temp2664.last_plus_one
- _temp2664.curr, _temp2664.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2665= Cyc_Absynpp_qvar2string( _temp2489); struct
_tagged_string _temp2666= Cyc_Absynpp_qvar2string( _temp2485); xprintf("too few type arguments for tunion %.*s.%.*s",
_temp2665.last_plus_one - _temp2665.curr, _temp2665.curr, _temp2666.last_plus_one
- _temp2666.curr, _temp2666.curr);}));} goto _LL2399;}} _LL2415: if(* _temp2498
== 0){ struct _handler_cons _temp2669; _push_handler(& _temp2669);{ struct
_xtunion_struct* _temp2670=( struct _xtunion_struct*) setjmp( _temp2669.handler);
if( ! _temp2670){{ struct Cyc_Core_Opt* xtud= Cyc_Tcenv_lookup_xtuniondecl( te,
loc, _temp2501); if( xtud == 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2671= Cyc_Absynpp_qvar2string( _temp2501); xprintf("unbound type xtunion %.*s",
_temp2671.last_plus_one - _temp2671.curr, _temp2671.curr);}));{ struct Cyc_List_List*
_temp2672= free_vars; _npop_handler( 0u); return _temp2672;}}* _temp2498=(
struct Cyc_Absyn_XTuniondecl*)*(( struct Cyc_Absyn_XTuniondecl**)({ struct Cyc_Core_Opt*
_temp2673= xtud; if( _temp2673 == 0){ _throw( Null_Exception);} _temp2673->v;}));};
_pop_handler();} else{ struct _xtunion_struct* _temp2675= _temp2670; _LL2677:
if( _temp2675->tag == Cyc_Dict_Absent_tag){ goto _LL2678;} else{ goto _LL2679;}
_LL2679: goto _LL2680; _LL2678: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2681= Cyc_Absynpp_qvar2string( _temp2501); xprintf("unbound type xtunion %.*s",
_temp2681.last_plus_one - _temp2681.curr, _temp2681.curr);})); return free_vars;
_LL2680:( void) _throw( _temp2675); _LL2676:;}}} if(* _temp2495 == 0){ struct
_handler_cons _temp2682; _push_handler(& _temp2682);{ struct _xtunion_struct*
_temp2683=( struct _xtunion_struct*) setjmp( _temp2682.handler); if( ! _temp2683){{
void* _temp2684= Cyc_Tcenv_lookup_ordinary( te, loc, _temp2499); struct Cyc_Absyn_Tunionfield*
_temp2690; struct Cyc_Absyn_XTuniondecl* _temp2692; _LL2686: if((( struct
_tunion_struct*) _temp2684)->tag == Cyc_Tcenv_XTunionRes_tag){ _LL2693:
_temp2692=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp2684)->f1; goto _LL2691; _LL2691: _temp2690=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp2684)->f2; goto _LL2687;} else{ goto
_LL2688;} _LL2688: goto _LL2689; _LL2687:* _temp2495=( struct Cyc_Absyn_Tunionfield*)
_temp2690; if(( struct Cyc_Absyn_XTuniondecl*) _temp2692 !=* _temp2498){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2694= Cyc_Absynpp_qvar2string( _temp2499);
struct _tagged_string _temp2695= Cyc_Absynpp_qvar2string( _temp2501); xprintf("unbound field %.*s in type xtunion %.*s",
_temp2694.last_plus_one - _temp2694.curr, _temp2694.curr, _temp2695.last_plus_one
- _temp2695.curr, _temp2695.curr);}));{ struct Cyc_List_List* _temp2696=
free_vars; _npop_handler( 0u); return _temp2696;}} goto _LL2685; _LL2689: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2697= Cyc_Absynpp_qvar2string( _temp2499);
struct _tagged_string _temp2698= Cyc_Absynpp_qvar2string( _temp2501); xprintf("unbound field %.*s in type xtunion %.*s",
_temp2697.last_plus_one - _temp2697.curr, _temp2697.curr, _temp2698.last_plus_one
- _temp2698.curr, _temp2698.curr);}));{ struct Cyc_List_List* _temp2699=
free_vars; _npop_handler( 0u); return _temp2699;} _LL2685:;}; _pop_handler();}
else{ struct _xtunion_struct* _temp2701= _temp2683; _LL2703: if( _temp2701->tag
== Cyc_Dict_Absent_tag){ goto _LL2704;} else{ goto _LL2705;} _LL2705: goto
_LL2706; _LL2704: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2707= Cyc_Absynpp_qvar2string(
_temp2499); struct _tagged_string _temp2708= Cyc_Absynpp_qvar2string( _temp2501);
xprintf("unbound field %.*s in type xtunion %.*s", _temp2707.last_plus_one -
_temp2707.curr, _temp2707.curr, _temp2708.last_plus_one - _temp2708.curr,
_temp2708.curr);})); return free_vars; _LL2706:( void) _throw( _temp2701);
_LL2702:;}}}{ struct Cyc_Absyn_XTuniondecl* xtud=( struct Cyc_Absyn_XTuniondecl*)({
struct Cyc_Absyn_XTuniondecl* _temp2710=* _temp2498; if( _temp2710 == 0){ _throw(
Null_Exception);} _temp2710;}); struct Cyc_Absyn_Tunionfield* xtufd=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_Absyn_Tunionfield* _temp2709=* _temp2495; if( _temp2709 == 0){ _throw(
Null_Exception);} _temp2709;}); if(( struct _tuple1*) xtud->name != 0){*
_temp2501=* xtud->name;} if(( struct _tuple1*) xtufd->name != 0){* _temp2499=*
xtufd->name;} goto _LL2399;} _LL2417: free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_AnyKind, _temp2513); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_RgnKind, _temp2511);{ void* _temp2711=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2505))->v; void* _temp2717; struct Cyc_Absyn_Exp* _temp2719; _LL2713: if((
unsigned int) _temp2711 > 1u?(( struct _tunion_struct*) _temp2711)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL2718: _temp2717=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2711)->f1;
if(( unsigned int) _temp2717 > 1u?(( struct _tunion_struct*) _temp2717)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL2720: _temp2719=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp2717)->f1; goto _LL2714;} else{ goto _LL2715;}}
else{ goto _LL2715;} _LL2715: goto _LL2716; _LL2714: if( ! Cyc_Tcutil_is_const_exp(
te, _temp2719)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp2721=(
char*)"pointer bounds expression is not a constant"; struct _tagged_string
_temp2722; _temp2722.curr= _temp2721; _temp2722.base= _temp2721; _temp2722.last_plus_one=
_temp2721 + 44; _temp2722;}));} Cyc_Tcexp_tcExp( te, 0, _temp2719); if( ! Cyc_Tcutil_coerce_uint_typ(
te, _temp2719)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp2723=(
char*)"pointer bounds expression is not an unsigned int"; struct _tagged_string
_temp2724; _temp2724.curr= _temp2723; _temp2724.base= _temp2723; _temp2724.last_plus_one=
_temp2723 + 49; _temp2724;}));} Cyc_Evexp_eval_const_uint_exp( _temp2719); goto
_LL2712; _LL2716: goto _LL2712; _LL2712:;} goto _LL2399; _LL2419: goto _LL2399;
_LL2421: goto _LL2399; _LL2423: goto _LL2399; _LL2425: free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_MemKind, _temp2523); if( _temp2519 == 0? 1:
! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2725= _temp2519; if( _temp2725 == 0){ _throw( Null_Exception);} _temp2725;}))){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp2726=( char*)"array bounds expression is not constant";
struct _tagged_string _temp2727; _temp2727.curr= _temp2726; _temp2727.base=
_temp2726; _temp2727.last_plus_one= _temp2726 + 40; _temp2727;}));} Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2728= _temp2519; if(
_temp2728 == 0){ _throw( Null_Exception);} _temp2728;})); if( ! Cyc_Tcutil_coerce_uint_typ(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2729= _temp2519; if(
_temp2729 == 0){ _throw( Null_Exception);} _temp2729;}))){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2730=( char*)"array bounds expression is not an unsigned int";
struct _tagged_string _temp2731; _temp2731.curr= _temp2730; _temp2731.base=
_temp2730; _temp2731.last_plus_one= _temp2730 + 47; _temp2731;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2732= _temp2519; if(
_temp2732 == 0){ _throw( Null_Exception);} _temp2732;})); goto _LL2399; _LL2427:
if( _temp2527 != 0){ int seen_cdecl= 0; int seen_stdcall= 0; for( 0; _temp2527
!= 0; _temp2527=({ struct Cyc_List_List* _temp2733= _temp2527; if( _temp2733 ==
0){ _throw( Null_Exception);} _temp2733->tl;})){ if( ! Cyc_Tcutil_fntype_att((
void*)({ struct Cyc_List_List* _temp2734= _temp2527; if( _temp2734 == 0){ _throw(
Null_Exception);} _temp2734->hd;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2736= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp2735= _temp2527; if( _temp2735 == 0){ _throw( Null_Exception);} _temp2735->hd;}));
xprintf("bad function type attribute %.*s", _temp2736.last_plus_one - _temp2736.curr,
_temp2736.curr);}));} if(( void*)({ struct Cyc_List_List* _temp2737= _temp2527;
if( _temp2737 == 0){ _throw( Null_Exception);} _temp2737->hd;}) ==( void*) Cyc_Absyn_Stdcall_att){
if( seen_cdecl){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_stdcall= 1;} else{ if(( void*)({ struct Cyc_List_List* _temp2738= _temp2527;
if( _temp2738 == 0){ _throw( Null_Exception);} _temp2738->hd;}) ==( void*) Cyc_Absyn_Cdecl_att){
if( seen_stdcall){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_cdecl= 1;}}}} Cyc_Tcutil_check_unique_tvars( loc,* _temp2540);{ struct Cyc_List_List*
b=* _temp2540; for( 0; b != 0; b=({ struct Cyc_List_List* _temp2739= b; if(
_temp2739 == 0){ _throw( Null_Exception);} _temp2739->tl;})){ void* _temp2741=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2740= b; if( _temp2740 == 0){
_throw( Null_Exception);} _temp2740->hd;}))->kind))->v; void* _temp2747; _LL2743:
if(( unsigned int) _temp2741 > 1u?(( struct _tunion_struct*) _temp2741)->tag ==
Cyc_Absyn_Eq_constr_tag: 0){ _LL2748: _temp2747=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2741)->f1; if(( int) _temp2747 == Cyc_Absyn_MemKind){ goto _LL2744;} else{
goto _LL2745;}} else{ goto _LL2745;} _LL2745: goto _LL2746; _LL2744: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2750=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2749= b; if( _temp2749 == 0){ _throw( Null_Exception);} _temp2749->hd;}))->name;
xprintf("function attempts to abstract Mem type variable %.*s", _temp2750.last_plus_one
- _temp2750.curr, _temp2750.curr);})); goto _LL2742; _LL2746: goto _LL2742;
_LL2742:;}}{ struct Cyc_List_List* new_free_vars= 0; new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind, _temp2533); for( 0; _temp2531
!= 0; _temp2531=({ struct Cyc_List_List* _temp2751= _temp2531; if( _temp2751 ==
0){ _throw( Null_Exception);} _temp2751->tl;})){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)({ struct
Cyc_List_List* _temp2752= _temp2531; if( _temp2752 == 0){ _throw( Null_Exception);}
_temp2752->hd;}))).f3);} if(* _temp2537 != 0){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_EffKind,( void*)({ struct Cyc_Core_Opt*
_temp2753=* _temp2537; if( _temp2753 == 0){ _throw( Null_Exception);} _temp2753->v;}));}
else{ struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_EffKind);
new_free_vars=({ struct Cyc_List_List* _temp2754=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2754->hd=( void*) e; _temp2754->tl=
new_free_vars; _temp2754;});{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= new_free_vars; for( 0; tvs != 0; tvs=({ struct Cyc_List_List* _temp2755=
tvs; if( _temp2755 == 0){ _throw( Null_Exception);} _temp2755->tl;})){ void*
_temp2757=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2756= tvs; if( _temp2756 == 0){ _throw( Null_Exception);} _temp2756->hd;}))->kind))->v;
void* _temp2765; void* _temp2767; _LL2759: if(( unsigned int) _temp2757 > 1u?((
struct _tunion_struct*) _temp2757)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2766:
_temp2765=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2757)->f1; if((
int) _temp2765 == Cyc_Absyn_RgnKind){ goto _LL2760;} else{ goto _LL2761;}} else{
goto _LL2761;} _LL2761: if(( unsigned int) _temp2757 > 1u?(( struct
_tunion_struct*) _temp2757)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2768:
_temp2767=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2757)->f1; if((
int) _temp2767 == Cyc_Absyn_EffKind){ goto _LL2762;} else{ goto _LL2763;}} else{
goto _LL2763;} _LL2763: goto _LL2764; _LL2760: effect=({ struct Cyc_List_List*
_temp2769=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2769->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2770=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2770[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2771; _temp2771.tag= Cyc_Absyn_AccessEff_tag;
_temp2771.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2772=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2772[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2773; _temp2773.tag= Cyc_Absyn_VarType_tag;
_temp2773.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2774= tvs;
if( _temp2774 == 0){ _throw( Null_Exception);} _temp2774->hd;}); _temp2773;});
_temp2772;})); _temp2771;}); _temp2770;})); _temp2769->tl= effect; _temp2769;});
goto _LL2758; _LL2762: effect=({ struct Cyc_List_List* _temp2775=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2775->hd=( void*)(( void*)({
struct Cyc_Absyn_VarType_struct* _temp2776=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2776[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2777; _temp2777.tag= Cyc_Absyn_VarType_tag;
_temp2777.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2778= tvs;
if( _temp2778 == 0){ _throw( Null_Exception);} _temp2778->hd;}); _temp2777;});
_temp2776;})); _temp2775->tl= effect; _temp2775;}); goto _LL2758; _LL2764: goto
_LL2758; _LL2758:;}}* _temp2537=({ struct Cyc_Core_Opt* _temp2779=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2779->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp2780=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2780[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2781; _temp2781.tag= Cyc_Absyn_JoinEff_tag;
_temp2781.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effect); _temp2781;}); _temp2780;})); _temp2779;});}} if(* _temp2540 != 0){
struct Cyc_List_List* new_btvs= 0;{ struct Cyc_List_List* bs=* _temp2540; for( 0;
bs != 0; bs=({ struct Cyc_List_List* _temp2782= bs; if( _temp2782 == 0){ _throw(
Null_Exception);} _temp2782->tl;})){ new_free_vars= Cyc_Tcutil_add_free_tvar(
loc, new_free_vars,( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2783=
bs; if( _temp2783 == 0){ _throw( Null_Exception);} _temp2783->hd;}));{ struct
Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2798= bs; if( _temp2798 == 0){ _throw( Null_Exception);} _temp2798->hd;}))->kind);
void* _temp2784=( void*) c->v; void* _temp2792; _LL2786: if(( int) _temp2784 ==
Cyc_Absyn_No_constr){ goto _LL2787;} else{ goto _LL2788;} _LL2788: if((
unsigned int) _temp2784 > 1u?(( struct _tunion_struct*) _temp2784)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL2793: _temp2792=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2784)->f1;
if(( int) _temp2792 == Cyc_Absyn_MemKind){ goto _LL2789;} else{ goto _LL2790;}}
else{ goto _LL2790;} _LL2790: goto _LL2791; _LL2787: Cyc_Tcutil_warn( loc,({
struct _tagged_string _temp2795=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2794= bs; if( _temp2794 == 0){ _throw( Null_Exception);} _temp2794->hd;}))->name;
xprintf("Type variable %.*s unconstrained, assuming boxed", _temp2795.last_plus_one
- _temp2795.curr, _temp2795.curr);})); goto _LL2789; _LL2789:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2796=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2796[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp2797; _temp2797.tag= Cyc_Absyn_Eq_constr_tag;
_temp2797.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2797;}); _temp2796;})));
goto _LL2785; _LL2791: goto _LL2785; _LL2785:;}}} new_free_vars= Cyc_Tcutil_remove_bound_tvars(
new_free_vars,* _temp2540);} for( 0; new_free_vars != 0; new_free_vars=({ struct
Cyc_List_List* _temp2799= new_free_vars; if( _temp2799 == 0){ _throw(
Null_Exception);} _temp2799->tl;})){ free_vars= Cyc_Tcutil_add_free_tvar( loc,
free_vars,( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2800=
new_free_vars; if( _temp2800 == 0){ _throw( Null_Exception);} _temp2800->hd;}));}
goto _LL2399;} _LL2429: for( 0; _temp2541 != 0; _temp2541=({ struct Cyc_List_List*
_temp2801= _temp2541; if( _temp2801 == 0){ _throw( Null_Exception);} _temp2801->tl;})){
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple4*)({ struct Cyc_List_List* _temp2802= _temp2541; if( _temp2802 ==
0){ _throw( Null_Exception);} _temp2802->hd;}))).f2);} goto _LL2399; _LL2431:
if( _temp2548 == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2803=( char*)"missing struct name"; struct _tagged_string _temp2804;
_temp2804.curr= _temp2803; _temp2804.base= _temp2803; _temp2804.last_plus_one=
_temp2803 + 20; _temp2804;})); return free_vars;} if(* _temp2545 == 0){ struct
_handler_cons _temp2805; _push_handler(& _temp2805);{ struct _xtunion_struct*
_temp2806=( struct _xtunion_struct*) setjmp( _temp2805.handler); if( ! _temp2806){*
_temp2545=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl( te, loc,(
struct _tuple1*)({ struct _tuple1* _temp2807= _temp2548; if( _temp2807 == 0){
_throw( Null_Exception);} _temp2807;}));; _pop_handler();} else{ struct
_xtunion_struct* _temp2809= _temp2806; _LL2811: if( _temp2809->tag == Cyc_Dict_Absent_tag){
goto _LL2812;} else{ goto _LL2813;} _LL2813: goto _LL2814; _LL2812: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2816= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2815= _temp2548; if( _temp2815 == 0){ _throw(
Null_Exception);} _temp2815;})); xprintf("unbound type struct %.*s", _temp2816.last_plus_one
- _temp2816.curr, _temp2816.curr);})); return free_vars; _LL2814:( void) _throw(
_temp2809); _LL2810:;}}}{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl**
_temp2829=* _temp2545; if( _temp2829 == 0){ _throw( Null_Exception);}* _temp2829;});
if( sd->name != 0){({ struct _tuple1* _temp2817= _temp2548; if( _temp2817 == 0){
_throw( Null_Exception);}* _temp2817=({ struct _tuple1* _temp2819=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp2818= sd->name; if( _temp2818 == 0){
_throw( Null_Exception);} _temp2818->v;}); unsigned int _temp2820= 0; if(
_temp2820 >= 1u){ _throw( Null_Exception);} _temp2819[ _temp2820];});});}{
struct Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( tvs); int largs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2546); if( lvs != largs){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2822= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp2821= _temp2548; if( _temp2821 == 0){ _throw( Null_Exception);} _temp2821;}));
int _temp2823= lvs; int _temp2824= largs; xprintf("struct %.*s expects %d type arguments but was given %d",
_temp2822.last_plus_one - _temp2822.curr, _temp2822.curr, _temp2823, _temp2824);}));}
for( 0; _temp2546 != 0; _temp2546=({ struct Cyc_List_List* _temp2825= _temp2546;
if( _temp2825 == 0){ _throw( Null_Exception);} _temp2825->tl;}), tvs=({ struct
Cyc_List_List* _temp2826= tvs; if( _temp2826 == 0){ _throw( Null_Exception);}
_temp2826->tl;})){ void* t1=( void*)({ struct Cyc_List_List* _temp2828=
_temp2546; if( _temp2828 == 0){ _throw( Null_Exception);} _temp2828->hd;}); void*
k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2827= tvs; if( _temp2827 == 0){ _throw(
Null_Exception);} _temp2827->hd;}))->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2399;}} _LL2433: if( _temp2555 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2830=( char*)"missing union name";
struct _tagged_string _temp2831; _temp2831.curr= _temp2830; _temp2831.base=
_temp2830; _temp2831.last_plus_one= _temp2830 + 19; _temp2831;})); return
free_vars;} if(* _temp2552 == 0){ struct _handler_cons _temp2832; _push_handler(&
_temp2832);{ struct _xtunion_struct* _temp2833=( struct _xtunion_struct*) setjmp(
_temp2832.handler); if( ! _temp2833){* _temp2552=( struct Cyc_Absyn_Uniondecl**)
Cyc_Tcenv_lookup_uniondecl( te, loc,( struct _tuple1*)({ struct _tuple1*
_temp2834= _temp2555; if( _temp2834 == 0){ _throw( Null_Exception);} _temp2834;}));;
_pop_handler();} else{ struct _xtunion_struct* _temp2836= _temp2833; _LL2838:
if( _temp2836->tag == Cyc_Dict_Absent_tag){ goto _LL2839;} else{ goto _LL2840;}
_LL2840: goto _LL2841; _LL2839: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2843= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp2842= _temp2555; if( _temp2842 == 0){ _throw( Null_Exception);} _temp2842;}));
xprintf("unbound type union %.*s", _temp2843.last_plus_one - _temp2843.curr,
_temp2843.curr);})); return free_vars; _LL2841:( void) _throw( _temp2836);
_LL2837:;}}}{ struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl**
_temp2856=* _temp2552; if( _temp2856 == 0){ _throw( Null_Exception);}* _temp2856;});
if( ud->name != 0){({ struct _tuple1* _temp2844= _temp2555; if( _temp2844 == 0){
_throw( Null_Exception);}* _temp2844=({ struct _tuple1* _temp2846=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp2845= ud->name; if( _temp2845 == 0){
_throw( Null_Exception);} _temp2845->v;}); unsigned int _temp2847= 0; if(
_temp2847 >= 1u){ _throw( Null_Exception);} _temp2846[ _temp2847];});});}{
struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( tvs); int largs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2553); if( lvs != largs){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2849= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp2848= _temp2555; if( _temp2848 == 0){ _throw( Null_Exception);} _temp2848;}));
int _temp2850= lvs; int _temp2851= largs; xprintf("union %.*s expects %d type arguments but was given %d",
_temp2849.last_plus_one - _temp2849.curr, _temp2849.curr, _temp2850, _temp2851);}));}
for( 0; _temp2553 != 0; _temp2553=({ struct Cyc_List_List* _temp2852= _temp2553;
if( _temp2852 == 0){ _throw( Null_Exception);} _temp2852->tl;}), tvs=({ struct
Cyc_List_List* _temp2853= tvs; if( _temp2853 == 0){ _throw( Null_Exception);}
_temp2853->tl;})){ void* t1=( void*)({ struct Cyc_List_List* _temp2855=
_temp2553; if( _temp2855 == 0){ _throw( Null_Exception);} _temp2855->hd;}); void*
k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2854= tvs; if( _temp2854 == 0){ _throw(
Null_Exception);} _temp2854->hd;}))->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2399;}} _LL2435: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp2857; _push_handler(& _temp2857);{ struct
_xtunion_struct* _temp2858=( struct _xtunion_struct*) setjmp( _temp2857.handler);
if( ! _temp2858){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc, _temp2562);;
_pop_handler();} else{ struct _xtunion_struct* _temp2860= _temp2858; _LL2862:
if( _temp2860->tag == Cyc_Dict_Absent_tag){ goto _LL2863;} else{ goto _LL2864;}
_LL2864: goto _LL2865; _LL2863: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2866= Cyc_Absynpp_qvar2string( _temp2562); xprintf("unbound typedef name %.*s",
_temp2866.last_plus_one - _temp2866.curr, _temp2866.curr);})); return free_vars;
_LL2865:( void) _throw( _temp2860); _LL2861:;}}}({ struct _tuple1* _temp2867=
_temp2562; unsigned int _temp2868= 0; if( _temp2868 >= 1u){ _throw(
Null_Exception);} _temp2867[ _temp2868]=({ struct _tuple1* _temp2869= td->name;
unsigned int _temp2870= 0; if( _temp2870 >= 1u){ _throw( Null_Exception);}
_temp2869[ _temp2870];});});{ struct Cyc_List_List* tvs= td->tvs; struct Cyc_List_List*
ts= _temp2560; struct Cyc_List_List* inst= 0; for( 0; ts != 0? tvs != 0: 0; ts=({
struct Cyc_List_List* _temp2871= ts; if( _temp2871 == 0){ _throw( Null_Exception);}
_temp2871->tl;}), tvs=({ struct Cyc_List_List* _temp2872= tvs; if( _temp2872 ==
0){ _throw( Null_Exception);} _temp2872->tl;})){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2878= tvs; if( _temp2878 == 0){ _throw( Null_Exception);} _temp2878->hd;}))->kind);
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, k,( void*)({ struct
Cyc_List_List* _temp2873= ts; if( _temp2873 == 0){ _throw( Null_Exception);}
_temp2873->hd;})); inst=({ struct Cyc_List_List* _temp2874=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2874->hd=( void*)({ struct
_tuple5* _temp2875=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2875->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2877= tvs;
if( _temp2877 == 0){ _throw( Null_Exception);} _temp2877->hd;}); _temp2875->f2=(
void*)({ struct Cyc_List_List* _temp2876= ts; if( _temp2876 == 0){ _throw(
Null_Exception);} _temp2876->hd;}); _temp2875;}); _temp2874->tl= inst; _temp2874;});}
if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2879= Cyc_Absynpp_qvar2string( _temp2562); xprintf("typedef %.*s expects a different number of arguments",
_temp2879.last_plus_one - _temp2879.curr, _temp2879.curr);})); return free_vars;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2559=({
struct Cyc_Core_Opt* _temp2880=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2880->v=( void*) new_typ; _temp2880;}); goto _LL2399;}}}
_LL2437: goto _LL2399; _LL2439: _temp2566= _temp2564; goto _LL2441; _LL2441:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2566); goto _LL2399; _LL2443: for( 0; _temp2568 != 0; _temp2568=({ struct
Cyc_List_List* _temp2881= _temp2568; if( _temp2881 == 0){ _throw( Null_Exception);}
_temp2881->tl;})){ free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,(
void*) Cyc_Absyn_EffKind,( void*)({ struct Cyc_List_List* _temp2882= _temp2568;
if( _temp2882 == 0){ _throw( Null_Exception);} _temp2882->hd;}));} goto _LL2399;
_LL2399:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t), expected_kind)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2883= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp2884= Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind(
t)); struct _tagged_string _temp2885= Cyc_Absynpp_kind2string( expected_kind);
xprintf("type %.*s has kind %.*s but as used here needs kind %.*s", _temp2883.last_plus_one
- _temp2883.curr, _temp2883.curr, _temp2884.last_plus_one - _temp2884.curr,
_temp2884.curr, _temp2885.last_plus_one - _temp2885.curr, _temp2885.curr);}));}
return free_vars;} void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct Cyc_List_List* free_tvars= Cyc_Tcutil_check_valid_type(
loc, te, 0,( void*) Cyc_Absyn_MemKind, t);{ struct Cyc_List_List* x= free_tvars;
for( 0; x != 0; x=({ struct Cyc_List_List* _temp2886= x; if( _temp2886 == 0){
_throw( Null_Exception);} _temp2886->tl;})){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2899= x; if( _temp2899 == 0){
_throw( Null_Exception);} _temp2899->hd;}))->kind); void* _temp2887=( void*) c->v;
void* _temp2895; _LL2889: if(( int) _temp2887 == Cyc_Absyn_No_constr){ goto
_LL2890;} else{ goto _LL2891;} _LL2891: if(( unsigned int) _temp2887 > 1u?((
struct _tunion_struct*) _temp2887)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2896:
_temp2895=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2887)->f1; if((
int) _temp2895 == Cyc_Absyn_MemKind){ goto _LL2892;} else{ goto _LL2893;}} else{
goto _LL2893;} _LL2893: goto _LL2894; _LL2890: goto _LL2892; _LL2892:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2897=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2897[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp2898; _temp2898.tag= Cyc_Absyn_Eq_constr_tag;
_temp2898.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2898;}); _temp2897;})));
goto _LL2888; _LL2894: goto _LL2888; _LL2888:;}} if( free_tvars != 0){ void*
_temp2900= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2906; struct
Cyc_List_List* _temp2908; int _temp2910; struct Cyc_List_List* _temp2912; void*
_temp2914; struct Cyc_Core_Opt* _temp2916; struct Cyc_List_List* _temp2918;
struct Cyc_List_List** _temp2920; _LL2902: if(( unsigned int) _temp2900 > 4u?((
struct _tunion_struct*) _temp2900)->tag == Cyc_Absyn_FnType_tag: 0){ _LL2907:
_temp2906=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2900)->f1; _LL2919: _temp2918=( struct Cyc_List_List*) _temp2906.tvars;
_temp2920=&((( struct Cyc_Absyn_FnType_struct*) _temp2900)->f1).tvars; goto
_LL2917; _LL2917: _temp2916=( struct Cyc_Core_Opt*) _temp2906.effect; goto
_LL2915; _LL2915: _temp2914=( void*) _temp2906.ret_typ; goto _LL2913; _LL2913:
_temp2912=( struct Cyc_List_List*) _temp2906.args; goto _LL2911; _LL2911:
_temp2910=( int) _temp2906.varargs; goto _LL2909; _LL2909: _temp2908=( struct
Cyc_List_List*) _temp2906.attributes; goto _LL2903;} else{ goto _LL2904;}
_LL2904: goto _LL2905; _LL2903: if(* _temp2920 == 0){* _temp2920= free_tvars;}
else{ if( free_tvars != 0){ struct _tagged_string s=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2925= free_tvars; if( _temp2925 == 0){ _throw(
Null_Exception);} _temp2925->hd;}))->name; if(({ struct _tagged_string _temp2921=
s; char* _temp2923= _temp2921.curr + 0; if( _temp2921.base == 0? 1:( _temp2923 <
_temp2921.base? 1: _temp2923 >= _temp2921.last_plus_one)){ _throw(
Null_Exception);}* _temp2923;}) =='?'){ Cyc_Tcutil_terr( loc, xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2924= s; xprintf("unbound type variable %.*s",
_temp2924.last_plus_one - _temp2924.curr, _temp2924.curr);}));}}} goto _LL2901;
_LL2905: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2927=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2926= free_tvars; if( _temp2926 == 0){ _throw(
Null_Exception);} _temp2926->hd;}))->name; xprintf("unbound type variable %.*s",
_temp2927.last_plus_one - _temp2927.curr, _temp2927.curr);})); return; _LL2901:;}}
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0, expected_kind, t); struct
Cyc_List_List* fs= free_tvars; for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp2928= fs; if( _temp2928 == 0){ _throw( Null_Exception);} _temp2928->tl;})){
struct Cyc_Absyn_Tvar* ftv=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2936= fs; if( _temp2936 == 0){ _throw( Null_Exception);} _temp2936->hd;});
struct _tagged_string* ftvn= ftv->name; struct Cyc_Absyn_Conref* ftvk= ftv->kind;
int present= 0;{ struct Cyc_List_List* bs= bound_tvars; for( 0; bs != 0; bs=({
struct Cyc_List_List* _temp2929= bs; if( _temp2929 == 0){ _throw( Null_Exception);}
_temp2929->tl;})){ struct Cyc_Absyn_Tvar* btv=( struct Cyc_Absyn_Tvar*)({ struct
Cyc_List_List* _temp2933= bs; if( _temp2933 == 0){ _throw( Null_Exception);}
_temp2933->hd;}); struct _tagged_string* btvn= btv->name; struct Cyc_Absyn_Conref*
btvk= btv->kind; if( Cyc_String_zstrptrcmp( ftvn, btvn) == 0){ present= 1; if( !
Cyc_Tcutil_constrain_kinds( btvk, ftvk)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2930=* ftvn; struct _tagged_string _temp2931= Cyc_Absynpp_ckind2string(
btvk); struct _tagged_string _temp2932= Cyc_Absynpp_ckind2string( ftvk); xprintf("type variable %.*s has kind %.*s but is used here atkind %.*s",
_temp2930.last_plus_one - _temp2930.curr, _temp2930.curr, _temp2931.last_plus_one
- _temp2931.curr, _temp2931.curr, _temp2932.last_plus_one - _temp2932.curr,
_temp2932.curr);}));} break;}}} if( ! present){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2934=* ftvn; struct _tagged_string _temp2935= Cyc_Absynpp_typ2string(
t); xprintf("unbound type variable %.*s in type %.*s", _temp2934.last_plus_one -
_temp2934.curr, _temp2934.curr, _temp2935.last_plus_one - _temp2935.curr,
_temp2935.curr);}));}}} static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)(
void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for( 0;
vs != 0; vs=({ struct Cyc_List_List* _temp2937= vs; if( _temp2937 == 0){ _throw(
Null_Exception);} _temp2937->tl;})){ struct Cyc_List_List* vs2=({ struct Cyc_List_List*
_temp2944= vs; if( _temp2944 == 0){ _throw( Null_Exception);} _temp2944->tl;});
for( 0; vs2 != 0; vs2=({ struct Cyc_List_List* _temp2938= vs2; if( _temp2938 ==
0){ _throw( Null_Exception);} _temp2938->tl;})){ if( cmp(( void*)({ struct Cyc_List_List*
_temp2939= vs; if( _temp2939 == 0){ _throw( Null_Exception);} _temp2939->hd;}),(
void*)({ struct Cyc_List_List* _temp2940= vs2; if( _temp2940 == 0){ _throw(
Null_Exception);} _temp2940->hd;})) == 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2942= msg; struct _tagged_string _temp2943= a2string(( void*)({
struct Cyc_List_List* _temp2941= vs; if( _temp2941 == 0){ _throw( Null_Exception);}
_temp2941->hd;})); xprintf("%.*s: %.*s", _temp2942.last_plus_one - _temp2942.curr,
_temp2942.curr, _temp2943.last_plus_one - _temp2943.curr, _temp2943.curr);}));}}}}
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
tvs, loc, Cyc_Tcutil_tvar2string,( struct _tagged_string)({ char* _temp2945=(
char*)"duplicate type variable"; struct _tagged_string _temp2946; _temp2946.curr=
_temp2945; _temp2946.base= _temp2945; _temp2946.last_plus_one= _temp2945 + 24;
_temp2946;}));} struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd){ if( sd->fields == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2947=( char*)"inappropriate use of abstract struct"; struct _tagged_string
_temp2948; _temp2948.curr= _temp2947; _temp2948.base= _temp2947; _temp2948.last_plus_one=
_temp2947 + 37; _temp2948;}));}{ struct Cyc_List_List* fields= 0;{ struct Cyc_List_List*
sd_fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2956= sd->fields;
if( _temp2956 == 0){ _throw( Null_Exception);} _temp2956->v;}); for( 0;
sd_fields != 0; sd_fields=({ struct Cyc_List_List* _temp2949= sd_fields; if(
_temp2949 == 0){ _throw( Null_Exception);} _temp2949->tl;})){ if( Cyc_String_strcmp(*((
struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp2950= sd_fields; if(
_temp2950 == 0){ _throw( Null_Exception);} _temp2950->hd;}))->name,( struct
_tagged_string)({ char* _temp2951=( char*)""; struct _tagged_string _temp2952;
_temp2952.curr= _temp2951; _temp2952.base= _temp2951; _temp2952.last_plus_one=
_temp2951 + 1; _temp2952;})) != 0){ fields=({ struct Cyc_List_List* _temp2953=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2953->hd=(
void*)({ struct _tuple15* _temp2954=( struct _tuple15*) GC_malloc( sizeof(
struct _tuple15)); _temp2954->f1=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2955= sd_fields; if( _temp2955 == 0){ _throw( Null_Exception);} _temp2955->hd;});
_temp2954->f2= 0; _temp2954;}); _temp2953->tl= fields; _temp2953;});}}} fields=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{
struct Cyc_List_List* ans= 0; for( 0; des != 0; des=({ struct Cyc_List_List*
_temp2957= des; if( _temp2957 == 0){ _throw( Null_Exception);} _temp2957->tl;})){
struct _tuple16 _temp2961; void* _temp2962; struct Cyc_List_List* _temp2964;
struct _tuple16* _temp2959=( struct _tuple16*)({ struct Cyc_List_List* _temp2958=
des; if( _temp2958 == 0){ _throw( Null_Exception);} _temp2958->hd;}); _temp2961=*
_temp2959; _LL2965: _temp2964= _temp2961.f1; goto _LL2963; _LL2963: _temp2962=
_temp2961.f2; goto _LL2960; _LL2960: if( _temp2964 == 0){ struct Cyc_List_List*
fields2= fields; for( 0; fields2 != 0; fields2=({ struct Cyc_List_List*
_temp2966= fields2; if( _temp2966 == 0){ _throw( Null_Exception);} _temp2966->tl;})){
if( !(*(( struct _tuple15*)({ struct Cyc_List_List* _temp2967= fields2; if(
_temp2967 == 0){ _throw( Null_Exception);} _temp2967->hd;}))).f2){(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2968= fields2; if( _temp2968 == 0){
_throw( Null_Exception);} _temp2968->hd;}))).f2= 1;(*(( struct _tuple16*)({
struct Cyc_List_List* _temp2969= des; if( _temp2969 == 0){ _throw(
Null_Exception);} _temp2969->hd;}))).f1=({ struct Cyc_List_List* _temp2970=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2970->hd=(
void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2971=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp2971[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp2972; _temp2972.tag= Cyc_Absyn_FieldName_tag;
_temp2972.f1=((*(( struct _tuple15*)({ struct Cyc_List_List* _temp2973= fields2;
if( _temp2973 == 0){ _throw( Null_Exception);} _temp2973->hd;}))).f1)->name;
_temp2972;}); _temp2971;})); _temp2970->tl= 0; _temp2970;}); ans=({ struct Cyc_List_List*
_temp2974=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2974->hd=( void*)({ struct _tuple17* _temp2975=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp2975->f1=(*(( struct _tuple15*)({
struct Cyc_List_List* _temp2976= fields2; if( _temp2976 == 0){ _throw(
Null_Exception);} _temp2976->hd;}))).f1; _temp2975->f2= _temp2962; _temp2975;});
_temp2974->tl= ans; _temp2974;}); break;}} if( fields2 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2977=( char*)"too many arguments to struct";
struct _tagged_string _temp2978; _temp2978.curr= _temp2977; _temp2978.base=
_temp2977; _temp2978.last_plus_one= _temp2977 + 29; _temp2978;}));}} else{ if(({
struct Cyc_List_List* _temp2979= _temp2964; if( _temp2979 == 0){ _throw(
Null_Exception);} _temp2979->tl;}) != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2980=( char*)"multiple designators are not supported";
struct _tagged_string _temp2981; _temp2981.curr= _temp2980; _temp2981.base=
_temp2980; _temp2981.last_plus_one= _temp2980 + 39; _temp2981;}));} else{ void*
_temp2983=( void*)({ struct Cyc_List_List* _temp2982= _temp2964; if( _temp2982
== 0){ _throw( Null_Exception);} _temp2982->hd;}); struct Cyc_Absyn_Exp*
_temp2989; struct _tagged_string* _temp2991; _LL2985: if((( struct
_tunion_struct*) _temp2983)->tag == Cyc_Absyn_ArrayElement_tag){ _LL2990:
_temp2989=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp2983)->f1; goto _LL2986;} else{ goto _LL2987;} _LL2987: if((( struct
_tunion_struct*) _temp2983)->tag == Cyc_Absyn_FieldName_tag){ _LL2992: _temp2991=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp2983)->f1;
goto _LL2988;} else{ goto _LL2984;} _LL2986: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2993=( char*)"array designator used in argument to struct";
struct _tagged_string _temp2994; _temp2994.curr= _temp2993; _temp2994.base=
_temp2993; _temp2994.last_plus_one= _temp2993 + 44; _temp2994;})); goto _LL2984;
_LL2988: { struct Cyc_List_List* fields2= fields; for( 0; fields2 != 0; fields2=({
struct Cyc_List_List* _temp2995= fields2; if( _temp2995 == 0){ _throw(
Null_Exception);} _temp2995->tl;})){ if( Cyc_String_zstrptrcmp( _temp2991,((*((
struct _tuple15*)({ struct Cyc_List_List* _temp2996= fields2; if( _temp2996 == 0){
_throw( Null_Exception);} _temp2996->hd;}))).f1)->name) == 0){ if((*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2997= fields2; if( _temp2997 == 0){
_throw( Null_Exception);} _temp2997->hd;}))).f2){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2998=* _temp2991; xprintf("field %.*s has already been used as an argument",
_temp2998.last_plus_one - _temp2998.curr, _temp2998.curr);}));}(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2999= fields2; if( _temp2999 == 0){
_throw( Null_Exception);} _temp2999->hd;}))).f2= 1; ans=({ struct Cyc_List_List*
_temp3000=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3000->hd=( void*)({ struct _tuple17* _temp3001=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp3001->f1=(*(( struct _tuple15*)({
struct Cyc_List_List* _temp3002= fields2; if( _temp3002 == 0){ _throw(
Null_Exception);} _temp3002->hd;}))).f1; _temp3001->f2= _temp2962; _temp3001;});
_temp3000->tl= ans; _temp3000;}); break;}} if( fields2 == 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp3003=* _temp2991; xprintf("bad field designator %.*s",
_temp3003.last_plus_one - _temp3003.curr, _temp3003.curr);}));} goto _LL2984;}
_LL2984:;}}} for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp3004=
fields; if( _temp3004 == 0){ _throw( Null_Exception);} _temp3004->tl;})){ if( !(*((
struct _tuple15*)({ struct Cyc_List_List* _temp3005= fields; if( _temp3005 == 0){
_throw( Null_Exception);} _temp3005->hd;}))).f2){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp3006=( char*)"too few arguments to struct"; struct
_tagged_string _temp3007; _temp3007.curr= _temp3006; _temp3007.base= _temp3006;
_temp3007.last_plus_one= _temp3006 + 28; _temp3007;})); break;}} return(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}}} int Cyc_Tcutil_is_tagged_pointer_typ(
void* t){ void* _temp3008= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3014; struct Cyc_Absyn_Conref* _temp3016; struct Cyc_Absyn_Tqual* _temp3018;
struct Cyc_Absyn_Conref* _temp3020; void* _temp3022; void* _temp3024; _LL3010:
if(( unsigned int) _temp3008 > 4u?(( struct _tunion_struct*) _temp3008)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL3015: _temp3014=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp3008)->f1; _LL3025: _temp3024=( void*)
_temp3014.elt_typ; goto _LL3023; _LL3023: _temp3022=( void*) _temp3014.rgn_typ;
goto _LL3021; _LL3021: _temp3020=( struct Cyc_Absyn_Conref*) _temp3014.nullable;
goto _LL3019; _LL3019: _temp3018=( struct Cyc_Absyn_Tqual*) _temp3014.tq; goto
_LL3017; _LL3017: _temp3016=( struct Cyc_Absyn_Conref*) _temp3014.bounds; goto
_LL3011;} else{ goto _LL3012;} _LL3012: goto _LL3013; _LL3011: { struct Cyc_Absyn_Conref*
b=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp3016); void* _temp3026=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( b))->v; void* _temp3034; _LL3028: if((
unsigned int) _temp3026 > 1u?(( struct _tunion_struct*) _temp3026)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL3035: _temp3034=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3026)->f1;
if(( int) _temp3034 == Cyc_Absyn_Unknown_b){ goto _LL3029;} else{ goto _LL3030;}}
else{ goto _LL3030;} _LL3030: if(( int) _temp3026 == Cyc_Absyn_No_constr){ goto
_LL3031;} else{ goto _LL3032;} _LL3032: goto _LL3033; _LL3029: return 1; _LL3031:(
void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3036=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3036[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3037; _temp3037.tag= Cyc_Absyn_Eq_constr_tag;
_temp3037.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3037;}); _temp3036;})));
return 1; _LL3033: return 0; _LL3027:;} _LL3013: return 0; _LL3009:;} struct
_tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple6 bogus_ans=({ struct _tuple6 _temp3260; _temp3260.f1= 0;
_temp3260.f2=( void*) Cyc_Absyn_HeapRgn; _temp3260;}); void* _temp3038=( void*)
e->r; void* _temp3052; struct _tuple1* _temp3054; struct _tagged_string*
_temp3056; struct Cyc_Absyn_Exp* _temp3058; struct _tagged_string* _temp3060;
struct Cyc_Absyn_Exp* _temp3062; struct Cyc_Absyn_Exp* _temp3064; struct Cyc_Absyn_Exp*
_temp3066; struct Cyc_Absyn_Exp* _temp3068; _LL3040: if((( struct _tunion_struct*)
_temp3038)->tag == Cyc_Absyn_Var_e_tag){ _LL3055: _temp3054=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp3038)->f1; goto _LL3053; _LL3053: _temp3052=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp3038)->f2; goto _LL3041;} else{
goto _LL3042;} _LL3042: if((( struct _tunion_struct*) _temp3038)->tag == Cyc_Absyn_StructMember_e_tag){
_LL3059: _temp3058=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3038)->f1; goto _LL3057; _LL3057: _temp3056=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp3038)->f2; goto _LL3043;} else{
goto _LL3044;} _LL3044: if((( struct _tunion_struct*) _temp3038)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL3063: _temp3062=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3038)->f1; goto _LL3061; _LL3061: _temp3060=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp3038)->f2; goto _LL3045;} else{
goto _LL3046;} _LL3046: if((( struct _tunion_struct*) _temp3038)->tag == Cyc_Absyn_Deref_e_tag){
_LL3065: _temp3064=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp3038)->f1; goto _LL3047;} else{ goto _LL3048;} _LL3048: if((( struct
_tunion_struct*) _temp3038)->tag == Cyc_Absyn_Subscript_e_tag){ _LL3069:
_temp3068=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3038)->f1; goto _LL3067; _LL3067: _temp3066=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp3038)->f2; goto _LL3049;} else{ goto
_LL3050;} _LL3050: goto _LL3051; _LL3041: { void* _temp3070= _temp3052; struct
Cyc_Absyn_Fndecl* _temp3084; struct Cyc_Absyn_Vardecl* _temp3086; struct Cyc_Absyn_Vardecl*
_temp3088; struct Cyc_Absyn_Vardecl* _temp3090; struct Cyc_Absyn_Vardecl*
_temp3092; _LL3072: if(( int) _temp3070 == Cyc_Absyn_Unresolved_b){ goto _LL3073;}
else{ goto _LL3074;} _LL3074: if(( unsigned int) _temp3070 > 1u?(( struct
_tunion_struct*) _temp3070)->tag == Cyc_Absyn_Funname_b_tag: 0){ _LL3085:
_temp3084=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp3070)->f1; goto _LL3075;} else{ goto _LL3076;} _LL3076: if(( unsigned int)
_temp3070 > 1u?(( struct _tunion_struct*) _temp3070)->tag == Cyc_Absyn_Global_b_tag:
0){ _LL3087: _temp3086=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp3070)->f1; goto _LL3077;} else{ goto _LL3078;} _LL3078: if(( unsigned int)
_temp3070 > 1u?(( struct _tunion_struct*) _temp3070)->tag == Cyc_Absyn_Local_b_tag:
0){ _LL3089: _temp3088=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*)
_temp3070)->f1; goto _LL3079;} else{ goto _LL3080;} _LL3080: if(( unsigned int)
_temp3070 > 1u?(( struct _tunion_struct*) _temp3070)->tag == Cyc_Absyn_Pat_b_tag:
0){ _LL3091: _temp3090=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp3070)->f1; goto _LL3081;} else{ goto _LL3082;} _LL3082: if(( unsigned int)
_temp3070 > 1u?(( struct _tunion_struct*) _temp3070)->tag == Cyc_Absyn_Param_b_tag:
0){ _LL3093: _temp3092=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp3070)->f1; goto _LL3083;} else{ goto _LL3071;} _LL3073: return bogus_ans;
_LL3075: return({ struct _tuple6 _temp3094; _temp3094.f1= 1; _temp3094.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3094;}); _LL3077: { void* _temp3096= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp3095= e->topt; if( _temp3095 == 0){ _throw(
Null_Exception);} _temp3095->v;})); struct Cyc_Absyn_Exp* _temp3102; struct Cyc_Absyn_Tqual*
_temp3104; void* _temp3106; _LL3098: if(( unsigned int) _temp3096 > 4u?(( struct
_tunion_struct*) _temp3096)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL3107:
_temp3106=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3096)->f1; goto
_LL3105; _LL3105: _temp3104=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3096)->f2; goto _LL3103; _LL3103: _temp3102=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3096)->f3; goto _LL3099;} else{ goto
_LL3100;} _LL3100: goto _LL3101; _LL3099: return({ struct _tuple6 _temp3108;
_temp3108.f1= 1; _temp3108.f2=( void*) Cyc_Absyn_HeapRgn; _temp3108;}); _LL3101:
return({ struct _tuple6 _temp3109; _temp3109.f1=( _temp3086->tq)->q_const;
_temp3109.f2=( void*) Cyc_Absyn_HeapRgn; _temp3109;}); _LL3097:;} _LL3079: {
void* _temp3111= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp3110=
e->topt; if( _temp3110 == 0){ _throw( Null_Exception);} _temp3110->v;})); struct
Cyc_Absyn_Exp* _temp3117; struct Cyc_Absyn_Tqual* _temp3119; void* _temp3121;
_LL3113: if(( unsigned int) _temp3111 > 4u?(( struct _tunion_struct*) _temp3111)->tag
== Cyc_Absyn_ArrayType_tag: 0){ _LL3122: _temp3121=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3111)->f1; goto _LL3120; _LL3120: _temp3119=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3111)->f2; goto _LL3118; _LL3118:
_temp3117=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3111)->f3; goto _LL3114;} else{ goto _LL3115;} _LL3115: goto _LL3116;
_LL3114: return({ struct _tuple6 _temp3123; _temp3123.f1= 1; _temp3123.f2=( void*)({
struct Cyc_Core_Opt* _temp3124= _temp3088->rgn; if( _temp3124 == 0){ _throw(
Null_Exception);} _temp3124->v;}); _temp3123;}); _LL3116: return({ struct
_tuple6 _temp3125; _temp3125.f1=( _temp3088->tq)->q_const; _temp3125.f2=( void*)({
struct Cyc_Core_Opt* _temp3126= _temp3088->rgn; if( _temp3126 == 0){ _throw(
Null_Exception);} _temp3126->v;}); _temp3125;}); _LL3112:;} _LL3081: _temp3092=
_temp3090; goto _LL3083; _LL3083: return({ struct _tuple6 _temp3127; _temp3127.f1=(
_temp3092->tq)->q_const; _temp3127.f2=( void*)({ struct Cyc_Core_Opt* _temp3128=
_temp3092->rgn; if( _temp3128 == 0){ _throw( Null_Exception);} _temp3128->v;});
_temp3127;}); _LL3071:;} _LL3043: { void* _temp3130= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp3129= _temp3058->topt; if( _temp3129 == 0){ _throw(
Null_Exception);} _temp3129->v;})); struct Cyc_Absyn_Structdecl** _temp3138;
struct Cyc_Absyn_Structdecl* _temp3140; struct Cyc_List_List* _temp3141; struct
_tuple1* _temp3143; struct Cyc_Absyn_Uniondecl** _temp3145; struct Cyc_Absyn_Uniondecl*
_temp3147; struct Cyc_List_List* _temp3148; struct _tuple1* _temp3150; _LL3132:
if(( unsigned int) _temp3130 > 4u?(( struct _tunion_struct*) _temp3130)->tag ==
Cyc_Absyn_StructType_tag: 0){ _LL3144: _temp3143=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp3130)->f1; goto _LL3142; _LL3142: _temp3141=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp3130)->f2; goto _LL3139; _LL3139:
_temp3138=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp3130)->f3; if( _temp3138 == 0){ goto _LL3134;} else{ _temp3140=* _temp3138;
goto _LL3133;}} else{ goto _LL3134;} _LL3134: if(( unsigned int) _temp3130 > 4u?((
struct _tunion_struct*) _temp3130)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL3151:
_temp3150=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp3130)->f1;
goto _LL3149; _LL3149: _temp3148=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3130)->f2; goto _LL3146; _LL3146: _temp3145=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp3130)->f3; if( _temp3145 == 0){ goto
_LL3136;} else{ _temp3147=* _temp3145; goto _LL3135;}} else{ goto _LL3136;}
_LL3136: goto _LL3137; _LL3133: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp3140, _temp3056); if( finfo != 0?({ struct Cyc_Absyn_Structfield* _temp3152=
finfo; if( _temp3152 == 0){ _throw( Null_Exception);} _temp3152->width;}) != 0:
0){ return({ struct _tuple6 _temp3153; _temp3153.f1=({ struct Cyc_Absyn_Structfield*
_temp3154= finfo; if( _temp3154 == 0){ _throw( Null_Exception);} _temp3154->tq;})->q_const;
_temp3153.f2=( Cyc_Tcutil_addressof_props( te, _temp3058)).f2; _temp3153;});}
return bogus_ans;} _LL3135: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_union_field(
_temp3147, _temp3056); if( finfo != 0){ return({ struct _tuple6 _temp3155;
_temp3155.f1=({ struct Cyc_Absyn_Structfield* _temp3156= finfo; if( _temp3156 ==
0){ _throw( Null_Exception);} _temp3156->tq;})->q_const; _temp3155.f2=( Cyc_Tcutil_addressof_props(
te, _temp3058)).f2; _temp3155;});} goto _LL3137;} _LL3137: return bogus_ans;
_LL3131:;} _LL3045: { void* _temp3158= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp3157= _temp3062->topt; if( _temp3157 == 0){ _throw( Null_Exception);}
_temp3157->v;})); struct Cyc_Absyn_PtrInfo _temp3164; struct Cyc_Absyn_Conref*
_temp3166; struct Cyc_Absyn_Tqual* _temp3168; struct Cyc_Absyn_Conref* _temp3170;
void* _temp3172; void* _temp3174; _LL3160: if(( unsigned int) _temp3158 > 4u?((
struct _tunion_struct*) _temp3158)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL3165: _temp3164=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp3158)->f1; _LL3175: _temp3174=( void*) _temp3164.elt_typ; goto _LL3173;
_LL3173: _temp3172=( void*) _temp3164.rgn_typ; goto _LL3171; _LL3171: _temp3170=(
struct Cyc_Absyn_Conref*) _temp3164.nullable; goto _LL3169; _LL3169: _temp3168=(
struct Cyc_Absyn_Tqual*) _temp3164.tq; goto _LL3167; _LL3167: _temp3166=( struct
Cyc_Absyn_Conref*) _temp3164.bounds; goto _LL3161;} else{ goto _LL3162;} _LL3162:
goto _LL3163; _LL3161: { void* _temp3176= Cyc_Tcutil_compress( _temp3174);
struct Cyc_Absyn_Structdecl** _temp3184; struct Cyc_Absyn_Structdecl* _temp3186;
struct Cyc_List_List* _temp3187; struct _tuple1* _temp3189; struct Cyc_Absyn_Uniondecl**
_temp3191; struct Cyc_Absyn_Uniondecl* _temp3193; struct Cyc_List_List*
_temp3194; struct _tuple1* _temp3196; _LL3178: if(( unsigned int) _temp3176 > 4u?((
struct _tunion_struct*) _temp3176)->tag == Cyc_Absyn_StructType_tag: 0){ _LL3190:
_temp3189=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3176)->f1;
goto _LL3188; _LL3188: _temp3187=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3176)->f2; goto _LL3185; _LL3185: _temp3184=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3176)->f3; if( _temp3184 == 0){ goto
_LL3180;} else{ _temp3186=* _temp3184; goto _LL3179;}} else{ goto _LL3180;}
_LL3180: if(( unsigned int) _temp3176 > 4u?(( struct _tunion_struct*) _temp3176)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL3197: _temp3196=( struct _tuple1*)(( struct
Cyc_Absyn_UnionType_struct*) _temp3176)->f1; goto _LL3195; _LL3195: _temp3194=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp3176)->f2;
goto _LL3192; _LL3192: _temp3191=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp3176)->f3; if( _temp3191 == 0){ goto _LL3182;} else{ _temp3193=* _temp3191;
goto _LL3181;}} else{ goto _LL3182;} _LL3182: goto _LL3183; _LL3179: { struct
Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field( _temp3186,
_temp3060); if( finfo != 0?({ struct Cyc_Absyn_Structfield* _temp3198= finfo;
if( _temp3198 == 0){ _throw( Null_Exception);} _temp3198->width;}) != 0: 0){
return({ struct _tuple6 _temp3199; _temp3199.f1=({ struct Cyc_Absyn_Structfield*
_temp3200= finfo; if( _temp3200 == 0){ _throw( Null_Exception);} _temp3200->tq;})->q_const;
_temp3199.f2= _temp3172; _temp3199;});} return bogus_ans;} _LL3181: { struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp3193, _temp3060); if( finfo != 0){
return({ struct _tuple6 _temp3201; _temp3201.f1=({ struct Cyc_Absyn_Structfield*
_temp3202= finfo; if( _temp3202 == 0){ _throw( Null_Exception);} _temp3202->tq;})->q_const;
_temp3201.f2= _temp3172; _temp3201;});} return bogus_ans;} _LL3183: return
bogus_ans; _LL3177:;} _LL3163: return bogus_ans; _LL3159:;} _LL3047: { void*
_temp3204= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp3203=
_temp3064->topt; if( _temp3203 == 0){ _throw( Null_Exception);} _temp3203->v;}));
struct Cyc_Absyn_PtrInfo _temp3210; struct Cyc_Absyn_Conref* _temp3212; struct
Cyc_Absyn_Tqual* _temp3214; struct Cyc_Absyn_Conref* _temp3216; void* _temp3218;
void* _temp3220; _LL3206: if(( unsigned int) _temp3204 > 4u?(( struct
_tunion_struct*) _temp3204)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL3211:
_temp3210=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp3204)->f1; _LL3221: _temp3220=( void*) _temp3210.elt_typ; goto _LL3219;
_LL3219: _temp3218=( void*) _temp3210.rgn_typ; goto _LL3217; _LL3217: _temp3216=(
struct Cyc_Absyn_Conref*) _temp3210.nullable; goto _LL3215; _LL3215: _temp3214=(
struct Cyc_Absyn_Tqual*) _temp3210.tq; goto _LL3213; _LL3213: _temp3212=( struct
Cyc_Absyn_Conref*) _temp3210.bounds; goto _LL3207;} else{ goto _LL3208;} _LL3208:
goto _LL3209; _LL3207: return({ struct _tuple6 _temp3222; _temp3222.f1=
_temp3214->q_const; _temp3222.f2= _temp3218; _temp3222;}); _LL3209: return
bogus_ans; _LL3205:;} _LL3049: { void* t= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp3257= _temp3068->topt; if( _temp3257 == 0){ _throw(
Null_Exception);} _temp3257->v;})); void* _temp3223= t; struct Cyc_Absyn_Exp*
_temp3233; struct Cyc_Absyn_Tqual* _temp3235; void* _temp3237; struct Cyc_List_List*
_temp3239; struct Cyc_Absyn_PtrInfo _temp3241; struct Cyc_Absyn_Conref*
_temp3243; struct Cyc_Absyn_Tqual* _temp3245; struct Cyc_Absyn_Conref* _temp3247;
void* _temp3249; void* _temp3251; _LL3225: if(( unsigned int) _temp3223 > 4u?((
struct _tunion_struct*) _temp3223)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL3238:
_temp3237=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3223)->f1; goto
_LL3236; _LL3236: _temp3235=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3223)->f2; goto _LL3234; _LL3234: _temp3233=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3223)->f3; goto _LL3226;} else{ goto
_LL3227;} _LL3227: if(( unsigned int) _temp3223 > 4u?(( struct _tunion_struct*)
_temp3223)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL3240: _temp3239=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp3223)->f1; goto _LL3228;} else{ goto
_LL3229;} _LL3229: if(( unsigned int) _temp3223 > 4u?(( struct _tunion_struct*)
_temp3223)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL3242: _temp3241=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp3223)->f1;
_LL3252: _temp3251=( void*) _temp3241.elt_typ; goto _LL3250; _LL3250: _temp3249=(
void*) _temp3241.rgn_typ; goto _LL3248; _LL3248: _temp3247=( struct Cyc_Absyn_Conref*)
_temp3241.nullable; goto _LL3246; _LL3246: _temp3245=( struct Cyc_Absyn_Tqual*)
_temp3241.tq; goto _LL3244; _LL3244: _temp3243=( struct Cyc_Absyn_Conref*)
_temp3241.bounds; goto _LL3230;} else{ goto _LL3231;} _LL3231: goto _LL3232;
_LL3226: return({ struct _tuple6 _temp3253; _temp3253.f1= _temp3235->q_const;
_temp3253.f2=( Cyc_Tcutil_addressof_props( te, _temp3068)).f2; _temp3253;});
_LL3228: { struct _tuple4* finfo= Cyc_Absyn_lookup_tuple_field( _temp3239,( int)
Cyc_Evexp_eval_const_uint_exp( _temp3066)); if( finfo != 0){ return({ struct
_tuple6 _temp3254; _temp3254.f1=(({ struct _tuple4* _temp3255= finfo; if(
_temp3255 == 0){ _throw( Null_Exception);}* _temp3255;}).f1)->q_const; _temp3254.f2=(
Cyc_Tcutil_addressof_props( te, _temp3068)).f2; _temp3254;});} return bogus_ans;}
_LL3230: return({ struct _tuple6 _temp3256; _temp3256.f1= _temp3245->q_const;
_temp3256.f2= _temp3249; _temp3256;}); _LL3232: return bogus_ans; _LL3224:;}
_LL3051: Cyc_Tcutil_terr( e->loc,( struct _tagged_string)({ char* _temp3258=(
char*)"unary & applied to non-lvalue"; struct _tagged_string _temp3259;
_temp3259.curr= _temp3258; _temp3259.base= _temp3258; _temp3259.last_plus_one=
_temp3258 + 30; _temp3259;})); return bogus_ans; _LL3039:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp3261= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Exp* _temp3267; struct
Cyc_Absyn_Tqual* _temp3269; void* _temp3271; _LL3263: if(( unsigned int)
_temp3261 > 4u?(( struct _tunion_struct*) _temp3261)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL3272: _temp3271=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3261)->f1;
goto _LL3270; _LL3270: _temp3269=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3261)->f2; goto _LL3268; _LL3268: _temp3267=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3261)->f3; goto _LL3264;} else{ goto
_LL3265;} _LL3265: goto _LL3266; _LL3264: { void* _temp3275; int _temp3277;
struct _tuple6 _temp3273= Cyc_Tcutil_addressof_props( te, e); _LL3278: _temp3277=
_temp3273.f1; goto _LL3276; _LL3276: _temp3275= _temp3273.f2; goto _LL3274;
_LL3274: return Cyc_Absyn_atb_typ( _temp3271, _temp3275, _temp3269,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp3279=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp3279[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp3280; _temp3280.tag= Cyc_Absyn_Upper_b_tag;
_temp3280.f1= e; _temp3280;}); _temp3279;}));} _LL3266: return e_typ; _LL3262:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( b);{ void* _temp3281=( void*) b->v; void*
_temp3289; void* _temp3291; struct Cyc_Absyn_Exp* _temp3293; _LL3283: if((
unsigned int) _temp3281 > 1u?(( struct _tunion_struct*) _temp3281)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL3290: _temp3289=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3281)->f1;
if(( int) _temp3289 == Cyc_Absyn_Unknown_b){ goto _LL3284;} else{ goto _LL3285;}}
else{ goto _LL3285;} _LL3285: if(( unsigned int) _temp3281 > 1u?(( struct
_tunion_struct*) _temp3281)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL3292:
_temp3291=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3281)->f1; if((
unsigned int) _temp3291 > 1u?(( struct _tunion_struct*) _temp3291)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL3294: _temp3293=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp3291)->f1; goto _LL3286;} else{ goto _LL3287;}} else{ goto _LL3287;}
_LL3287: goto _LL3288; _LL3284: return; _LL3286: if( Cyc_Evexp_eval_const_uint_exp(
_temp3293) <= i){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp3295=( char*)"dereference is out of bounds"; struct _tagged_string
_temp3296; _temp3296.curr= _temp3295; _temp3296.base= _temp3295; _temp3296.last_plus_one=
_temp3295 + 29; _temp3296;}));} return; _LL3288:( void*)( b->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp3297=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3297[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3298; _temp3298.tag= Cyc_Absyn_Eq_constr_tag;
_temp3298.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3299=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3299[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3300; _temp3300.tag= Cyc_Absyn_Upper_b_tag;
_temp3300.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3300;}); _temp3299;}));
_temp3298;}); _temp3297;}))); return; _LL3282:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3301= Cyc_Tcutil_compress(
t); void* _temp3323; void* _temp3325; struct Cyc_Absyn_Enumdecl* _temp3327;
struct _tuple1* _temp3329; struct Cyc_Absyn_Exp* _temp3331; struct Cyc_Absyn_Tqual*
_temp3333; void* _temp3335; struct Cyc_List_List* _temp3337; struct Cyc_Absyn_Structdecl**
_temp3339; struct Cyc_List_List* _temp3341; struct _tuple1* _temp3343; struct
Cyc_Absyn_Uniondecl** _temp3345; struct Cyc_List_List* _temp3347; struct _tuple1*
_temp3349; _LL3303: if(( int) _temp3301 == Cyc_Absyn_VoidType){ goto _LL3304;}
else{ goto _LL3305;} _LL3305: if(( unsigned int) _temp3301 > 4u?(( struct
_tunion_struct*) _temp3301)->tag == Cyc_Absyn_IntType_tag: 0){ _LL3326:
_temp3325=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3301)->f1; goto
_LL3324; _LL3324: _temp3323=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3301)->f2; goto _LL3306;} else{ goto _LL3307;} _LL3307: if(( int) _temp3301
== Cyc_Absyn_FloatType){ goto _LL3308;} else{ goto _LL3309;} _LL3309: if(( int)
_temp3301 == Cyc_Absyn_DoubleType){ goto _LL3310;} else{ goto _LL3311;} _LL3311:
if(( unsigned int) _temp3301 > 4u?(( struct _tunion_struct*) _temp3301)->tag ==
Cyc_Absyn_EnumType_tag: 0){ _LL3330: _temp3329=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp3301)->f1; goto _LL3328; _LL3328: _temp3327=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp3301)->f2; goto _LL3312;} else{ goto
_LL3313;} _LL3313: if(( unsigned int) _temp3301 > 4u?(( struct _tunion_struct*)
_temp3301)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL3336: _temp3335=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3301)->f1; goto _LL3334; _LL3334:
_temp3333=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3301)->f2; goto _LL3332; _LL3332: _temp3331=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3301)->f3; goto _LL3314;} else{ goto
_LL3315;} _LL3315: if(( unsigned int) _temp3301 > 4u?(( struct _tunion_struct*)
_temp3301)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL3338: _temp3337=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp3301)->f1; goto _LL3316;} else{ goto
_LL3317;} _LL3317: if(( unsigned int) _temp3301 > 4u?(( struct _tunion_struct*)
_temp3301)->tag == Cyc_Absyn_StructType_tag: 0){ _LL3344: _temp3343=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3301)->f1; goto _LL3342;
_LL3342: _temp3341=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3301)->f2; goto _LL3340; _LL3340: _temp3339=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3301)->f3; goto _LL3318;} else{ goto
_LL3319;} _LL3319: if(( unsigned int) _temp3301 > 4u?(( struct _tunion_struct*)
_temp3301)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL3350: _temp3349=( struct
_tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp3301)->f1; goto _LL3348;
_LL3348: _temp3347=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3301)->f2; goto _LL3346; _LL3346: _temp3345=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp3301)->f3; goto _LL3320;} else{ goto
_LL3321;} _LL3321: goto _LL3322; _LL3304: return 1; _LL3306: return 1; _LL3308:
return 1; _LL3310: return 1; _LL3312: return 1; _LL3314: return Cyc_Tcutil_bits_only(
_temp3335); _LL3316: for( 0; _temp3337 != 0; _temp3337=({ struct Cyc_List_List*
_temp3351= _temp3337; if( _temp3351 == 0){ _throw( Null_Exception);} _temp3351->tl;})){
if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)({ struct Cyc_List_List*
_temp3352= _temp3337; if( _temp3352 == 0){ _throw( Null_Exception);} _temp3352->hd;}))).f2)){
return 0;}} return 1; _LL3318: if( _temp3339 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl** _temp3356= _temp3339; if( _temp3356 == 0){
_throw( Null_Exception);}* _temp3356;}); if( sd->fields == 0){ return 0;}{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( sd->tvs, _temp3341);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp3355= sd->fields; if(
_temp3355 == 0){ _throw( Null_Exception);} _temp3355->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp3353= fs; if( _temp3353 == 0){ _throw( Null_Exception);}
_temp3353->tl;})){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_substitute( inst,(
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp3354= fs;
if( _temp3354 == 0){ _throw( Null_Exception);} _temp3354->hd;}))->type))){
return 0;}}} return 1;}} _LL3320: if( _temp3345 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=({ struct Cyc_Absyn_Uniondecl** _temp3360= _temp3345; if( _temp3360 == 0){
_throw( Null_Exception);}* _temp3360;}); if( ud->fields == 0){ return 0;}{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( ud->tvs, _temp3347);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp3359= ud->fields; if(
_temp3359 == 0){ _throw( Null_Exception);} _temp3359->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp3357= fs; if( _temp3357 == 0){ _throw( Null_Exception);}
_temp3357->tl;})){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_substitute( inst,(
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp3358= fs;
if( _temp3358 == 0){ _throw( Null_Exception);} _temp3358->hd;}))->type))){
return 0;}}} return 1;}} _LL3322: return 0; _LL3302:;} static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv* te, int var_okay, struct Cyc_Absyn_Exp* e){ void*
_temp3361=( void*) e->r; void* _temp3399; void* _temp3401; struct Cyc_Absyn_Exp*
_temp3403; struct Cyc_Absyn_Enumfield* _temp3405; struct Cyc_Absyn_Enumdecl*
_temp3407; struct _tuple1* _temp3409; void* _temp3411; struct _tuple1* _temp3413;
struct Cyc_Absyn_Exp* _temp3415; struct Cyc_Absyn_Exp* _temp3417; struct Cyc_Absyn_Exp*
_temp3419; struct Cyc_Absyn_Exp* _temp3421; struct Cyc_Absyn_Exp* _temp3423;
struct Cyc_List_List* _temp3425; struct Cyc_Absyn_Exp* _temp3427; struct Cyc_Absyn_Exp*
_temp3429; void* _temp3431; struct Cyc_Absyn_Exp* _temp3433; struct Cyc_Absyn_Exp*
_temp3435; struct Cyc_Absyn_Exp* _temp3437; struct Cyc_Absyn_Vardecl* _temp3439;
struct Cyc_List_List* _temp3441; struct Cyc_Absyn_Structdecl* _temp3443; struct
Cyc_List_List* _temp3445; struct Cyc_Core_Opt* _temp3447; struct _tuple1*
_temp3449; struct Cyc_List_List* _temp3451; void* _temp3453; struct Cyc_List_List*
_temp3455; struct Cyc_Absyn_Tunionfield* _temp3457; struct Cyc_Absyn_Tuniondecl*
_temp3459; struct Cyc_List_List* _temp3461; struct Cyc_Core_Opt* _temp3463;
struct Cyc_Core_Opt* _temp3465; struct Cyc_Absyn_Tunionfield* _temp3467; struct
Cyc_Absyn_XTuniondecl* _temp3469; struct Cyc_List_List* _temp3471; struct Cyc_Core_Opt*
_temp3473; _LL3363: if((( struct _tunion_struct*) _temp3361)->tag == Cyc_Absyn_Const_e_tag){
_LL3400: _temp3399=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp3361)->f1;
goto _LL3364;} else{ goto _LL3365;} _LL3365: if((( struct _tunion_struct*)
_temp3361)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL3402: _temp3401=( void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp3361)->f1; goto _LL3366;} else{ goto
_LL3367;} _LL3367: if((( struct _tunion_struct*) _temp3361)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL3404: _temp3403=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp3361)->f1; goto _LL3368;} else{ goto _LL3369;} _LL3369: if((( struct
_tunion_struct*) _temp3361)->tag == Cyc_Absyn_Enum_e_tag){ _LL3410: _temp3409=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp3361)->f1; goto _LL3408;
_LL3408: _temp3407=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp3361)->f2; goto _LL3406; _LL3406: _temp3405=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp3361)->f3; goto _LL3370;} else{ goto
_LL3371;} _LL3371: if((( struct _tunion_struct*) _temp3361)->tag == Cyc_Absyn_Var_e_tag){
_LL3414: _temp3413=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3361)->f1; goto _LL3412; _LL3412: _temp3411=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3361)->f2; goto _LL3372;} else{ goto _LL3373;} _LL3373: if((( struct
_tunion_struct*) _temp3361)->tag == Cyc_Absyn_Conditional_e_tag){ _LL3420:
_temp3419=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3361)->f1; goto _LL3418; _LL3418: _temp3417=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp3361)->f2; goto _LL3416; _LL3416:
_temp3415=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3361)->f3; goto _LL3374;} else{ goto _LL3375;} _LL3375: if((( struct
_tunion_struct*) _temp3361)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL3424: _temp3423=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp3361)->f1; goto
_LL3422; _LL3422: _temp3421=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3361)->f2; goto _LL3376;} else{ goto _LL3377;} _LL3377: if((( struct
_tunion_struct*) _temp3361)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL3428:
_temp3427=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3361)->f1; goto _LL3426; _LL3426: _temp3425=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp3361)->f2; goto _LL3378;} else{
goto _LL3379;} _LL3379: if((( struct _tunion_struct*) _temp3361)->tag == Cyc_Absyn_Cast_e_tag){
_LL3432: _temp3431=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp3361)->f1;
goto _LL3430; _LL3430: _temp3429=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3361)->f2; goto _LL3380;} else{ goto _LL3381;} _LL3381: if((( struct
_tunion_struct*) _temp3361)->tag == Cyc_Absyn_Address_e_tag){ _LL3434: _temp3433=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp3361)->f1;
goto _LL3382;} else{ goto _LL3383;} _LL3383: if((( struct _tunion_struct*)
_temp3361)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL3440: _temp3439=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp3361)->f1;
goto _LL3438; _LL3438: _temp3437=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3361)->f2; goto _LL3436; _LL3436: _temp3435=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp3361)->f3; goto _LL3384;} else{
goto _LL3385;} _LL3385: if((( struct _tunion_struct*) _temp3361)->tag == Cyc_Absyn_Array_e_tag){
_LL3442: _temp3441=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp3361)->f1; goto _LL3386;} else{ goto _LL3387;} _LL3387: if((( struct
_tunion_struct*) _temp3361)->tag == Cyc_Absyn_Struct_e_tag){ _LL3450: _temp3449=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp3361)->f1; goto
_LL3448; _LL3448: _temp3447=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp3361)->f2; goto _LL3446; _LL3446: _temp3445=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp3361)->f3; goto _LL3444; _LL3444:
_temp3443=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp3361)->f4; goto _LL3388;} else{ goto _LL3389;} _LL3389: if((( struct
_tunion_struct*) _temp3361)->tag == Cyc_Absyn_Primop_e_tag){ _LL3454: _temp3453=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp3361)->f1; goto _LL3452;
_LL3452: _temp3451=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3361)->f2; goto _LL3390;} else{ goto _LL3391;} _LL3391: if((( struct
_tunion_struct*) _temp3361)->tag == Cyc_Absyn_Tuple_e_tag){ _LL3456: _temp3455=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp3361)->f1; goto
_LL3392;} else{ goto _LL3393;} _LL3393: if((( struct _tunion_struct*) _temp3361)->tag
== Cyc_Absyn_Tunion_e_tag){ _LL3466: _temp3465=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp3361)->f1; goto _LL3464; _LL3464: _temp3463=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp3361)->f2; goto
_LL3462; _LL3462: _temp3461=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3361)->f3; goto _LL3460; _LL3460: _temp3459=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp3361)->f4; goto _LL3458; _LL3458:
_temp3457=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3361)->f5; goto _LL3394;} else{ goto _LL3395;} _LL3395: if((( struct
_tunion_struct*) _temp3361)->tag == Cyc_Absyn_XTunion_e_tag){ _LL3474: _temp3473=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp3361)->f1; goto
_LL3472; _LL3472: _temp3471=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp3361)->f2; goto _LL3470; _LL3470: _temp3469=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp3361)->f3; goto _LL3468; _LL3468:
_temp3467=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp3361)->f4; goto _LL3396;} else{ goto _LL3397;} _LL3397: goto _LL3398;
_LL3364: return 1; _LL3366: return 1; _LL3368: return Cyc_Tcutil_cnst_exp( te, 1,
_temp3403); _LL3370: return 1; _LL3372: { struct _handler_cons _temp3475;
_push_handler(& _temp3475);{ struct _xtunion_struct* _temp3476=( struct
_xtunion_struct*) setjmp( _temp3475.handler); if( ! _temp3476){{ void* _temp3477=
Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3413); void* _temp3485; struct Cyc_Absyn_Fndecl*
_temp3487; void* _temp3489; struct Cyc_Absyn_Vardecl* _temp3491; _LL3479: if(((
struct _tunion_struct*) _temp3477)->tag == Cyc_Tcenv_VarRes_tag){ _LL3486:
_temp3485=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3477)->f1; if((
unsigned int) _temp3485 > 1u?(( struct _tunion_struct*) _temp3485)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL3488: _temp3487=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp3485)->f1; goto _LL3480;} else{ goto _LL3481;}} else{ goto _LL3481;}
_LL3481: if((( struct _tunion_struct*) _temp3477)->tag == Cyc_Tcenv_VarRes_tag){
_LL3490: _temp3489=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3477)->f1;
if(( unsigned int) _temp3489 > 1u?(( struct _tunion_struct*) _temp3489)->tag ==
Cyc_Absyn_Global_b_tag: 0){ _LL3492: _temp3491=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp3489)->f1; goto _LL3482;} else{ goto
_LL3483;}} else{ goto _LL3483;} _LL3483: goto _LL3484; _LL3480: { int _temp3493=
1; _npop_handler( 0u); return _temp3493;} _LL3482: { void* _temp3494= Cyc_Tcutil_compress((
void*) _temp3491->type); struct Cyc_Absyn_Exp* _temp3500; struct Cyc_Absyn_Tqual*
_temp3502; void* _temp3504; _LL3496: if(( unsigned int) _temp3494 > 4u?(( struct
_tunion_struct*) _temp3494)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL3505:
_temp3504=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3494)->f1; goto
_LL3503; _LL3503: _temp3502=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3494)->f2; goto _LL3501; _LL3501: _temp3500=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3494)->f3; goto _LL3497;} else{ goto
_LL3498;} _LL3498: goto _LL3499; _LL3497: { int _temp3506= 1; _npop_handler( 0u);
return _temp3506;} _LL3499: { int _temp3507= var_okay; _npop_handler( 0u);
return _temp3507;} _LL3495:;} _LL3484: { int _temp3508= var_okay; _npop_handler(
0u); return _temp3508;} _LL3478:;}; _pop_handler();} else{ struct
_xtunion_struct* _temp3510= _temp3476; _LL3512: if( _temp3510->tag == Cyc_Dict_Absent_tag){
goto _LL3513;} else{ goto _LL3514;} _LL3514: goto _LL3515; _LL3513: return 0;
_LL3515:( void) _throw( _temp3510); _LL3511:;}}} _LL3374: return( Cyc_Tcutil_cnst_exp(
te, 0, _temp3419)? Cyc_Tcutil_cnst_exp( te, 0, _temp3417): 0)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3415): 0; _LL3376: return Cyc_Tcutil_cnst_exp( te, 0, _temp3423)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3421): 0; _LL3378: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3427); _LL3380: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp3429); _LL3382: return Cyc_Tcutil_cnst_exp( te, 1, _temp3433); _LL3384:
return Cyc_Tcutil_cnst_exp( te, 0, _temp3437)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3435): 0; _LL3386: _temp3445= _temp3441; goto _LL3388; _LL3388: for( 0;
_temp3445 != 0; _temp3445=({ struct Cyc_List_List* _temp3516= _temp3445; if(
_temp3516 == 0){ _throw( Null_Exception);} _temp3516->tl;})){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple18*)({ struct Cyc_List_List* _temp3517= _temp3445; if(
_temp3517 == 0){ _throw( Null_Exception);} _temp3517->hd;}))).f2)){ return 0;}}
return 1; _LL3390: _temp3455= _temp3451; goto _LL3392; _LL3392: _temp3461=
_temp3455; goto _LL3394; _LL3394: _temp3471= _temp3461; goto _LL3396; _LL3396:
for( 0; _temp3471 != 0; _temp3471=({ struct Cyc_List_List* _temp3518= _temp3471;
if( _temp3518 == 0){ _throw( Null_Exception);} _temp3518->tl;})){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp3519= _temp3471; if(
_temp3519 == 0){ _throw( Null_Exception);} _temp3519->hd;}))){ return 0;}}
return 1; _LL3398: return 0; _LL3362:;} int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp( te, 0, e);}