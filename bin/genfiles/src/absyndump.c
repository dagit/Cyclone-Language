#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple4{
struct Cyc_Absyn_Tqual* f1; void* f2; struct Cyc_List_List* f3; } ; struct
_tuple5{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; struct _tuple7{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; struct _tuple8{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Pat* f2; } ; typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t;
typedef unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef
char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int;
typedef unsigned int Cyc_u_long; typedef unsigned short Cyc_ushort; typedef
unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t;
struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t;
typedef char* Cyc_caddr_t; typedef unsigned int Cyc_ino_t; typedef unsigned int
Cyc_vm_offset_t; typedef unsigned int Cyc_vm_size_t; typedef char Cyc_int8_t;
typedef char Cyc_u_int8_t; typedef short Cyc_int16_t; typedef unsigned short Cyc_u_int16_t;
typedef int Cyc_int32_t; typedef unsigned int Cyc_u_int32_t; typedef long long
Cyc_int64_t; typedef unsigned long long Cyc_u_int64_t; typedef int Cyc_register_t;
typedef short Cyc_dev_t; typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t;
typedef unsigned short Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t;
typedef int Cyc_ssize_t; typedef char* Cyc_addr_t; typedef int Cyc_mode_t;
typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[
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
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stdout; typedef int Cyc_Stdio_fpos_t; extern int Cyc_Stdio_fputc( int,
struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern int Cyc_Stdio_file_string_write( struct Cyc_Stdio___sFILE*
fd, struct _tagged_string src, int src_offset, int max_count); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
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
char* tag; } ; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct
Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim( void* p); extern
struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern int Cyc_Absynpp_qvar_to_Cids; extern int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_char_escape(
char); extern struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void* p);
extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); extern struct
_tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); extern struct _tuple4 Cyc_Absynpp_to_tms(
struct Cyc_Absyn_Tqual* tq, void* t); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
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
tag; } ; extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Absyndump_dumptyp(
void*); extern void Cyc_Absyndump_dumpntyp( void* t); extern void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*); extern void Cyc_Absyndump_dumpexp_prec( int, struct Cyc_Absyn_Exp*);
extern void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat*); extern void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt*); extern void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl*);
extern void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual*, void*, void(* f)(
void*), void*); typedef struct _tagged_string Cyc_Absyndump_dump_string_t;
struct Cyc_Stdio___sFILE** Cyc_Absyndump_dump_file=& Cyc_Stdio_stdout; void Cyc_Absyndump_ignore(
void* x){ return;} static unsigned int Cyc_Absyndump_pos= 0; void Cyc_Absyndump_dump(
struct _tagged_string s){ int sz=( int)({ struct _tagged_string _temp0= s;(
unsigned int)( _temp0.last_plus_one - _temp0.curr);}); if( !(( int)({ struct
_tagged_string _temp1= s; char* _temp3= _temp1.curr +( sz - 1); if( _temp1.base
== 0? 1:( _temp3 < _temp1.base? 1: _temp3 >= _temp1.last_plus_one)){ _throw(
Null_Exception);}* _temp3;}))){ -- sz;} Cyc_Absyndump_pos += sz + 1; if( Cyc_Absyndump_pos
> 80){ Cyc_Absyndump_pos=( unsigned int) sz; Cyc_Stdio_fputc(( int)'\n',( struct
Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file);} else{ Cyc_Stdio_fputc(( int)' ',(
struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file);} Cyc_Stdio_file_string_write((
struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_nospace(
struct _tagged_string s){ int sz=( int)({ struct _tagged_string _temp4= s;(
unsigned int)( _temp4.last_plus_one - _temp4.curr);}); if( !(( int)({ struct
_tagged_string _temp5= s; char* _temp7= _temp5.curr +( sz - 1); if( _temp5.base
== 0? 1:( _temp7 < _temp5.base? 1: _temp7 >= _temp5.last_plus_one)){ _throw(
Null_Exception);}* _temp7;}))){ -- sz;} Cyc_Absyndump_pos += sz; Cyc_Stdio_file_string_write((
struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_char(
int c){ ++ Cyc_Absyndump_pos; Cyc_Stdio_fputc( c,( struct Cyc_Stdio___sFILE*)*
Cyc_Absyndump_dump_file);} void Cyc_Absyndump_dump_str( struct _tagged_string* s){
Cyc_Absyndump_dump(* s);} void Cyc_Absyndump_dump_semi(){ Cyc_Absyndump_dump_char((
int)';');} void Cyc_Absyndump_dump_sep( void(* f)( void*), struct Cyc_List_List*
l, struct _tagged_string sep){ if( l == 0){ return;} for( 0;({ struct Cyc_List_List*
_temp8= l; if( _temp8 == 0){ _throw( Null_Exception);} _temp8->tl;}) != 0; l=({
struct Cyc_List_List* _temp9= l; if( _temp9 == 0){ _throw( Null_Exception);}
_temp9->tl;})){ f(( void*)({ struct Cyc_List_List* _temp10= l; if( _temp10 == 0){
_throw( Null_Exception);} _temp10->hd;})); Cyc_Absyndump_dump_nospace( sep);} f((
void*)({ struct Cyc_List_List* _temp11= l; if( _temp11 == 0){ _throw(
Null_Exception);} _temp11->hd;}));} void Cyc_Absyndump_dump_sep_c( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string sep){
if( l == 0){ return;} for( 0;({ struct Cyc_List_List* _temp12= l; if( _temp12 ==
0){ _throw( Null_Exception);} _temp12->tl;}) != 0; l=({ struct Cyc_List_List*
_temp13= l; if( _temp13 == 0){ _throw( Null_Exception);} _temp13->tl;})){ f( env,(
void*)({ struct Cyc_List_List* _temp14= l; if( _temp14 == 0){ _throw(
Null_Exception);} _temp14->hd;})); Cyc_Absyndump_dump_nospace( sep);} f( env,(
void*)({ struct Cyc_List_List* _temp15= l; if( _temp15 == 0){ _throw(
Null_Exception);} _temp15->hd;}));} void Cyc_Absyndump_group( void(* f)( void*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep){ Cyc_Absyndump_dump_nospace( start);(( void(*)( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
f, l, sep); Cyc_Absyndump_dump_nospace( end);} void Cyc_Absyndump_group_c( void(*
f)( void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep){ Cyc_Absyndump_dump_nospace(
start);(( void(*)( void(* f)( void*, void*), void* env, struct Cyc_List_List* l,
struct _tagged_string sep)) Cyc_Absyndump_dump_sep_c)( f, env, l, sep); Cyc_Absyndump_dump_nospace(
end);} void Cyc_Absyndump_egroup( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep){ if( l != 0){(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
f, l, start, end, sep);}} void Cyc_Absyndump_dumpqvar( struct _tuple0* v){
struct Cyc_List_List* nsl= 0;{ void* _temp16=(* v).f1; struct Cyc_List_List*
_temp28; struct Cyc_List_List* _temp30; struct Cyc_List_List* _temp32; struct
Cyc_List_List* _temp34; _LL18: if(( int) _temp16 == Cyc_Absyn_Loc_n){ goto _LL19;}
else{ goto _LL20;} _LL20: if(( unsigned int) _temp16 > 1u?(( struct
_tunion_struct*) _temp16)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL29: _temp28=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp16)->f1; if(
_temp28 == 0){ goto _LL21;} else{ goto _LL22;}} else{ goto _LL22;} _LL22: if((
unsigned int) _temp16 > 1u?(( struct _tunion_struct*) _temp16)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL31: _temp30=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp16)->f1; if( _temp30 == 0){ goto _LL23;} else{ goto _LL24;}} else{ goto
_LL24;} _LL24: if(( unsigned int) _temp16 > 1u?(( struct _tunion_struct*)
_temp16)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL33: _temp32=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp16)->f1; goto _LL25;} else{ goto _LL26;}
_LL26: if(( unsigned int) _temp16 > 1u?(( struct _tunion_struct*) _temp16)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL35: _temp34=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp16)->f1; goto _LL27;} else{ goto _LL17;} _LL19: goto _LL21; _LL21: goto
_LL23; _LL23: Cyc_Absyndump_dump_str((* v).f2); return; _LL25: _temp34= _temp32;
goto _LL27; _LL27: nsl= _temp34; goto _LL17; _LL17:;} Cyc_Absyndump_dump_str((
struct _tagged_string*)({ struct Cyc_List_List* _temp36= nsl; if( _temp36 == 0){
_throw( Null_Exception);} _temp36->hd;})); for( nsl=({ struct Cyc_List_List*
_temp37= nsl; if( _temp37 == 0){ _throw( Null_Exception);} _temp37->tl;}); nsl
!= 0; nsl=({ struct Cyc_List_List* _temp38= nsl; if( _temp38 == 0){ _throw(
Null_Exception);} _temp38->tl;})){ if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_char((
int)'_');} else{ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp39=( char*)"::"; struct _tagged_string _temp40; _temp40.curr= _temp39;
_temp40.base= _temp39; _temp40.last_plus_one= _temp39 + 3; _temp40;}));} Cyc_Absyndump_dump_nospace(*((
struct _tagged_string*)({ struct Cyc_List_List* _temp41= nsl; if( _temp41 == 0){
_throw( Null_Exception);} _temp41->hd;})));} if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp42=( char*)"_"; struct _tagged_string
_temp43; _temp43.curr= _temp42; _temp43.base= _temp42; _temp43.last_plus_one=
_temp42 + 2; _temp43;}));} else{ Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp44=( char*)"::"; struct _tagged_string _temp45;
_temp45.curr= _temp44; _temp45.base= _temp44; _temp45.last_plus_one= _temp44 + 3;
_temp45;}));} Cyc_Absyndump_dump_nospace(*(* v).f2);} void Cyc_Absyndump_dumptq(
struct Cyc_Absyn_Tqual* tq){ if( tq->q_restrict){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp46=( char*)"restrict"; struct _tagged_string
_temp47; _temp47.curr= _temp46; _temp47.base= _temp46; _temp47.last_plus_one=
_temp46 + 9; _temp47;}));} if( tq->q_volatile){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp48=( char*)"volatile"; struct _tagged_string
_temp49; _temp49.curr= _temp48; _temp49.base= _temp48; _temp49.last_plus_one=
_temp48 + 9; _temp49;}));} if( tq->q_const){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp50=( char*)"const"; struct _tagged_string _temp51;
_temp51.curr= _temp50; _temp51.base= _temp50; _temp51.last_plus_one= _temp50 + 6;
_temp51;}));}} void Cyc_Absyndump_dumpscope( void* sc){ void* _temp52= sc; _LL54:
if(( int) _temp52 == Cyc_Absyn_Static){ goto _LL55;} else{ goto _LL56;} _LL56:
if(( int) _temp52 == Cyc_Absyn_Public){ goto _LL57;} else{ goto _LL58;} _LL58:
if(( int) _temp52 == Cyc_Absyn_Extern){ goto _LL59;} else{ goto _LL60;} _LL60:
if(( int) _temp52 == Cyc_Absyn_ExternC){ goto _LL61;} else{ goto _LL62;} _LL62:
if(( int) _temp52 == Cyc_Absyn_Abstract){ goto _LL63;} else{ goto _LL53;} _LL55:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp64=( char*)"static";
struct _tagged_string _temp65; _temp65.curr= _temp64; _temp65.base= _temp64;
_temp65.last_plus_one= _temp64 + 7; _temp65;})); return; _LL57: return; _LL59:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp66=( char*)"extern";
struct _tagged_string _temp67; _temp67.curr= _temp66; _temp67.base= _temp66;
_temp67.last_plus_one= _temp66 + 7; _temp67;})); return; _LL61: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp68=( char*)"extern \"C\""; struct
_tagged_string _temp69; _temp69.curr= _temp68; _temp69.base= _temp68; _temp69.last_plus_one=
_temp68 + 11; _temp69;})); return; _LL63: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp70=( char*)"abstract"; struct _tagged_string
_temp71; _temp71.curr= _temp70; _temp71.base= _temp70; _temp71.last_plus_one=
_temp70 + 9; _temp71;})); return; _LL53:;} void Cyc_Absyndump_dumpkind( void* k){
void* _temp72= k; _LL74: if(( int) _temp72 == Cyc_Absyn_AnyKind){ goto _LL75;}
else{ goto _LL76;} _LL76: if(( int) _temp72 == Cyc_Absyn_MemKind){ goto _LL77;}
else{ goto _LL78;} _LL78: if(( int) _temp72 == Cyc_Absyn_BoxKind){ goto _LL79;}
else{ goto _LL80;} _LL80: if(( int) _temp72 == Cyc_Absyn_RgnKind){ goto _LL81;}
else{ goto _LL82;} _LL82: if(( int) _temp72 == Cyc_Absyn_EffKind){ goto _LL83;}
else{ goto _LL73;} _LL75: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp84=( char*)"A"; struct _tagged_string _temp85; _temp85.curr= _temp84;
_temp85.base= _temp84; _temp85.last_plus_one= _temp84 + 2; _temp85;})); return;
_LL77: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp86=( char*)"M";
struct _tagged_string _temp87; _temp87.curr= _temp86; _temp87.base= _temp86;
_temp87.last_plus_one= _temp86 + 2; _temp87;})); return; _LL79: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp88=( char*)"B"; struct _tagged_string
_temp89; _temp89.curr= _temp88; _temp89.base= _temp88; _temp89.last_plus_one=
_temp88 + 2; _temp89;})); return; _LL81: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp90=( char*)"R"; struct _tagged_string _temp91;
_temp91.curr= _temp90; _temp91.base= _temp90; _temp91.last_plus_one= _temp90 + 2;
_temp91;})); return; _LL83: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp92=( char*)"E"; struct _tagged_string _temp93; _temp93.curr= _temp92;
_temp93.base= _temp92; _temp93.last_plus_one= _temp92 + 2; _temp93;})); return;
_LL73:;} void Cyc_Absyndump_dumptps( struct Cyc_List_List* ts){(( void(*)( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptyp,
ts,( struct _tagged_string)({ char* _temp94=( char*)"<"; struct _tagged_string
_temp95; _temp95.curr= _temp94; _temp95.base= _temp94; _temp95.last_plus_one=
_temp94 + 2; _temp95;}),( struct _tagged_string)({ char* _temp96=( char*)">";
struct _tagged_string _temp97; _temp97.curr= _temp96; _temp97.base= _temp96;
_temp97.last_plus_one= _temp96 + 2; _temp97;}),( struct _tagged_string)({ char*
_temp98=( char*)","; struct _tagged_string _temp99; _temp99.curr= _temp98;
_temp99.base= _temp98; _temp99.last_plus_one= _temp98 + 2; _temp99;}));} void
Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);}
void Cyc_Absyndump_dumpkindedtvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str(
tv->name);{ void* _temp100=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp108; void* _temp110;
_LL102: if(( unsigned int) _temp100 > 1u?(( struct _tunion_struct*) _temp100)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL109: _temp108=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp100)->f1; if(( int) _temp108 == Cyc_Absyn_BoxKind){ goto _LL103;} else{
goto _LL104;}} else{ goto _LL104;} _LL104: if(( unsigned int) _temp100 > 1u?((
struct _tunion_struct*) _temp100)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL111:
_temp110=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp100)->f1; goto
_LL105;} else{ goto _LL106;} _LL106: goto _LL107; _LL103: goto _LL101; _LL105:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp112=( char*)"::";
struct _tagged_string _temp113; _temp113.curr= _temp112; _temp113.base= _temp112;
_temp113.last_plus_one= _temp112 + 3; _temp113;})); Cyc_Absyndump_dumpkind(
_temp110); goto _LL101; _LL107: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp114=( char*)"::?"; struct _tagged_string _temp115; _temp115.curr=
_temp114; _temp115.base= _temp114; _temp115.last_plus_one= _temp114 + 4;
_temp115;})); goto _LL101; _LL101:;}} void Cyc_Absyndump_dumptvars( struct Cyc_List_List*
tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptvar, tvs,( struct _tagged_string)({
char* _temp116=( char*)"<"; struct _tagged_string _temp117; _temp117.curr=
_temp116; _temp117.base= _temp116; _temp117.last_plus_one= _temp116 + 2;
_temp117;}),( struct _tagged_string)({ char* _temp118=( char*)">"; struct
_tagged_string _temp119; _temp119.curr= _temp118; _temp119.base= _temp118;
_temp119.last_plus_one= _temp118 + 2; _temp119;}),( struct _tagged_string)({
char* _temp120=( char*)","; struct _tagged_string _temp121; _temp121.curr=
_temp120; _temp121.base= _temp120; _temp121.last_plus_one= _temp120 + 2;
_temp121;}));} void Cyc_Absyndump_dumpkindedtvars( struct Cyc_List_List* tvs){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar, tvs,( struct _tagged_string)({ char* _temp122=(
char*)"<"; struct _tagged_string _temp123; _temp123.curr= _temp122; _temp123.base=
_temp122; _temp123.last_plus_one= _temp122 + 2; _temp123;}),( struct
_tagged_string)({ char* _temp124=( char*)">"; struct _tagged_string _temp125;
_temp125.curr= _temp124; _temp125.base= _temp124; _temp125.last_plus_one=
_temp124 + 2; _temp125;}),( struct _tagged_string)({ char* _temp126=( char*)",";
struct _tagged_string _temp127; _temp127.curr= _temp126; _temp127.base= _temp126;
_temp127.last_plus_one= _temp126 + 2; _temp127;}));} void Cyc_Absyndump_dumparg(
struct _tuple5* pr){(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( int),
int)) Cyc_Absyndump_dumptqtd)(( struct Cyc_Absyn_Tqual*)(* pr).f1,(* pr).f2,(
void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpargs( struct
Cyc_List_List* ts){(( void(*)( void(* f)( struct _tuple5*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumparg, ts,( struct _tagged_string)({
char* _temp128=( char*)"("; struct _tagged_string _temp129; _temp129.curr=
_temp128; _temp129.base= _temp128; _temp129.last_plus_one= _temp128 + 2;
_temp129;}),( struct _tagged_string)({ char* _temp130=( char*)")"; struct
_tagged_string _temp131; _temp131.curr= _temp130; _temp131.base= _temp130;
_temp131.last_plus_one= _temp130 + 2; _temp131;}),( struct _tagged_string)({
char* _temp132=( char*)","; struct _tagged_string _temp133; _temp133.curr=
_temp132; _temp133.base= _temp132; _temp133.last_plus_one= _temp132 + 2;
_temp133;}));} void Cyc_Absyndump_dumpatts( struct Cyc_List_List* atts){ if(
atts == 0){ return;} Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp134=( char*)" __attribute__(("; struct _tagged_string _temp135; _temp135.curr=
_temp134; _temp135.base= _temp134; _temp135.last_plus_one= _temp134 + 17;
_temp135;})); for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp136= atts;
if( _temp136 == 0){ _throw( Null_Exception);} _temp136->tl;})){ Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp137= atts; if(
_temp137 == 0){ _throw( Null_Exception);} _temp137->hd;}))); if(({ struct Cyc_List_List*
_temp138= atts; if( _temp138 == 0){ _throw( Null_Exception);} _temp138->tl;}) !=
0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp139=( char*)",";
struct _tagged_string _temp140; _temp140.curr= _temp139; _temp140.base= _temp139;
_temp140.last_plus_one= _temp139 + 2; _temp140;}));}} Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp141=( char*)")) "; struct _tagged_string
_temp142; _temp142.curr= _temp141; _temp142.base= _temp141; _temp142.last_plus_one=
_temp141 + 4; _temp142;}));} int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List*
tms){ if( tms == 0){ return 0;}{ void* _temp144=( void*)({ struct Cyc_List_List*
_temp143= tms; if( _temp143 == 0){ _throw( Null_Exception);} _temp143->hd;});
struct Cyc_Absyn_Tqual* _temp150; void* _temp152; void* _temp154; _LL146: if((
unsigned int) _temp144 > 1u?(( struct _tunion_struct*) _temp144)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL155: _temp154=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp144)->f1;
goto _LL153; _LL153: _temp152=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp144)->f2; goto _LL151; _LL151: _temp150=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp144)->f3; goto _LL147;} else{ goto _LL148;}
_LL148: goto _LL149; _LL147: return 1; _LL149: return 0; _LL145:;}} static void
Cyc_Absyndump_dumprgn( void* t){ void* _temp156= Cyc_Tcutil_compress( t); _LL158:
if(( int) _temp156 == Cyc_Absyn_HeapRgn){ goto _LL159;} else{ goto _LL160;}
_LL160: goto _LL161; _LL159: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp162=( char*)"`H"; struct _tagged_string _temp163; _temp163.curr= _temp162;
_temp163.base= _temp162; _temp163.last_plus_one= _temp162 + 3; _temp163;}));
goto _LL157; _LL161: Cyc_Absyndump_dumpntyp( t); goto _LL157; _LL157:;} static
struct _tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List*
regions= 0; struct Cyc_List_List* effects= 0;{ void* _temp164= Cyc_Tcutil_compress(
t); void* _temp172; struct Cyc_List_List* _temp174; _LL166: if(( unsigned int)
_temp164 > 4u?(( struct _tunion_struct*) _temp164)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL173: _temp172=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp164)->f1;
goto _LL167;} else{ goto _LL168;} _LL168: if(( unsigned int) _temp164 > 4u?((
struct _tunion_struct*) _temp164)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL175:
_temp174=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp164)->f1;
goto _LL169;} else{ goto _LL170;} _LL170: goto _LL171; _LL167: regions=({ struct
Cyc_List_List* _temp176=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp176->hd=( void*) _temp172; _temp176->tl= regions; _temp176;}); goto _LL165;
_LL169: for( 0; _temp174 != 0; _temp174=({ struct Cyc_List_List* _temp177=
_temp174; if( _temp177 == 0){ _throw( Null_Exception);} _temp177->tl;})){ struct
Cyc_List_List* _temp181; struct Cyc_List_List* _temp183; struct _tuple6 _temp179=
Cyc_Absyndump_effects_split(( void*)({ struct Cyc_List_List* _temp178= _temp174;
if( _temp178 == 0){ _throw( Null_Exception);} _temp178->hd;})); _LL184: _temp183=
_temp179.f1; goto _LL182; _LL182: _temp181= _temp179.f2; goto _LL180; _LL180:
regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( _temp183, regions); effects=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp181, effects);} goto _LL165; _LL171: effects=({ struct Cyc_List_List*
_temp185=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp185->hd=( void*) t; _temp185->tl= effects; _temp185;}); goto _LL165; _LL165:;}
return({ struct _tuple6 _temp186; _temp186.f1= regions; _temp186.f2= effects;
_temp186;});} static void Cyc_Absyndump_dumpeff( void* t){ struct Cyc_List_List*
_temp189; struct Cyc_List_List* _temp191; struct _tuple6 _temp187= Cyc_Absyndump_effects_split(
t); _LL192: _temp191= _temp187.f1; goto _LL190; _LL190: _temp189= _temp187.f2;
goto _LL188; _LL188: _temp191=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp191); _temp189=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( _temp189); for( 0; _temp189 != 0; _temp189=({
struct Cyc_List_List* _temp193= _temp189; if( _temp193 == 0){ _throw(
Null_Exception);} _temp193->tl;})){ Cyc_Absyndump_dumpntyp(( void*)({ struct Cyc_List_List*
_temp194= _temp189; if( _temp194 == 0){ _throw( Null_Exception);} _temp194->hd;}));
Cyc_Absyndump_dump_char(( int)'+');} Cyc_Absyndump_dump_char(( int)'{'); for( 0;
_temp191 != 0; _temp191=({ struct Cyc_List_List* _temp195= _temp191; if(
_temp195 == 0){ _throw( Null_Exception);} _temp195->tl;})){ Cyc_Absyndump_dumprgn((
void*)({ struct Cyc_List_List* _temp196= _temp191; if( _temp196 == 0){ _throw(
Null_Exception);} _temp196->hd;})); if(({ struct Cyc_List_List* _temp197=
_temp191; if( _temp197 == 0){ _throw( Null_Exception);} _temp197->tl;}) != 0){
Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char(( int)'}');} void
Cyc_Absyndump_dumpntyp( void* t){ void* _temp198= t; struct Cyc_Absyn_Exp*
_temp264; struct Cyc_Absyn_Tqual* _temp266; void* _temp268; struct Cyc_Absyn_FnInfo
_temp270; struct Cyc_Absyn_PtrInfo _temp272; struct Cyc_Absyn_Tvar* _temp274;
int _temp276; struct Cyc_Core_Opt* _temp278; void* _temp280; int _temp282;
struct Cyc_Core_Opt* _temp284; struct Cyc_Core_Opt _temp286; void* _temp287;
void* _temp289; struct Cyc_Absyn_TunionInfo _temp291; struct Cyc_Absyn_Tuniondecl*
_temp293; void* _temp295; struct Cyc_List_List* _temp297; struct _tuple0*
_temp299; struct Cyc_Absyn_XTunionInfo _temp301; struct Cyc_Absyn_XTuniondecl*
_temp303; void* _temp305; struct _tuple0* _temp307; struct Cyc_Absyn_TunionFieldInfo
_temp309; struct Cyc_Absyn_Tunionfield* _temp311; struct Cyc_Absyn_Tuniondecl*
_temp313; struct _tuple0* _temp315; struct Cyc_List_List* _temp317; struct
_tuple0* _temp319; struct Cyc_Absyn_XTunionFieldInfo _temp321; struct Cyc_Absyn_Tunionfield*
_temp323; struct Cyc_Absyn_XTuniondecl* _temp325; struct _tuple0* _temp327;
struct _tuple0* _temp329; struct Cyc_Absyn_Enumdecl* _temp331; struct _tuple0*
_temp333; void* _temp335; void* _temp337; void* _temp339; void* _temp341; void*
_temp343; void* _temp345; void* _temp347; void* _temp349; void* _temp351; void*
_temp353; void* _temp355; void* _temp357; void* _temp359; void* _temp361; void*
_temp363; void* _temp365; struct Cyc_List_List* _temp367; struct Cyc_Absyn_Structdecl**
_temp369; struct Cyc_List_List* _temp371; struct _tuple0* _temp373; struct Cyc_Absyn_Structdecl**
_temp375; struct Cyc_List_List* _temp377; struct _tuple0* _temp379; struct Cyc_Absyn_Uniondecl**
_temp381; struct Cyc_List_List* _temp383; struct _tuple0* _temp385; struct Cyc_Absyn_Uniondecl**
_temp387; struct Cyc_List_List* _temp389; struct _tuple0* _temp391; struct Cyc_Core_Opt*
_temp393; struct Cyc_List_List* _temp395; struct _tuple0* _temp397; void*
_temp399; void* _temp401; struct Cyc_List_List* _temp403; _LL200: if((
unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL269: _temp268=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp198)->f1;
goto _LL267; _LL267: _temp266=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp198)->f2; goto _LL265; _LL265: _temp264=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp198)->f3; goto _LL201;} else{ goto _LL202;}
_LL202: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL271: _temp270=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp198)->f1; goto _LL203;} else{ goto _LL204;}
_LL204: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL273: _temp272=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp198)->f1; goto _LL205;} else{ goto
_LL206;} _LL206: if(( int) _temp198 == Cyc_Absyn_VoidType){ goto _LL207;} else{
goto _LL208;} _LL208: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*)
_temp198)->tag == Cyc_Absyn_VarType_tag: 0){ _LL275: _temp274=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp198)->f1; goto _LL209;} else{ goto _LL210;}
_LL210: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL281: _temp280=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp198)->f1; goto _LL279; _LL279: _temp278=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp198)->f2; if( _temp278 == 0){ goto _LL277;} else{
goto _LL212;} _LL277: _temp276=( int)(( struct Cyc_Absyn_Evar_struct*) _temp198)->f3;
goto _LL211;} else{ goto _LL212;} _LL212: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_Evar_tag: 0){ _LL290:
_temp289=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp198)->f1; goto _LL285;
_LL285: _temp284=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp198)->f2; if( _temp284 == 0){ goto _LL214;} else{ _temp286=* _temp284;
_LL288: _temp287=( void*) _temp286.v; goto _LL283;} _LL283: _temp282=( int)((
struct Cyc_Absyn_Evar_struct*) _temp198)->f3; goto _LL213;} else{ goto _LL214;}
_LL214: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag
== Cyc_Absyn_TunionType_tag: 0){ _LL292: _temp291=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp198)->f1; _LL300: _temp299=( struct
_tuple0*) _temp291.name; goto _LL298; _LL298: _temp297=( struct Cyc_List_List*)
_temp291.targs; goto _LL296; _LL296: _temp295=( void*) _temp291.rgn; goto _LL294;
_LL294: _temp293=( struct Cyc_Absyn_Tuniondecl*) _temp291.tud; goto _LL215;}
else{ goto _LL216;} _LL216: if(( unsigned int) _temp198 > 4u?(( struct
_tunion_struct*) _temp198)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL302:
_temp301=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp198)->f1; _LL308: _temp307=( struct _tuple0*) _temp301.name; goto _LL306;
_LL306: _temp305=( void*) _temp301.rgn; goto _LL304; _LL304: _temp303=( struct
Cyc_Absyn_XTuniondecl*) _temp301.xtud; goto _LL217;} else{ goto _LL218;} _LL218:
if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag ==
Cyc_Absyn_TunionFieldType_tag: 0){ _LL310: _temp309=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp198)->f1; _LL320: _temp319=(
struct _tuple0*) _temp309.name; goto _LL318; _LL318: _temp317=( struct Cyc_List_List*)
_temp309.targs; goto _LL316; _LL316: _temp315=( struct _tuple0*) _temp309.fname;
goto _LL314; _LL314: _temp313=( struct Cyc_Absyn_Tuniondecl*) _temp309.tud; goto
_LL312; _LL312: _temp311=( struct Cyc_Absyn_Tunionfield*) _temp309.tufd; goto
_LL219;} else{ goto _LL220;} _LL220: if(( unsigned int) _temp198 > 4u?(( struct
_tunion_struct*) _temp198)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL322:
_temp321=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp198)->f1; _LL330: _temp329=( struct _tuple0*) _temp321.name; goto _LL328;
_LL328: _temp327=( struct _tuple0*) _temp321.fname; goto _LL326; _LL326:
_temp325=( struct Cyc_Absyn_XTuniondecl*) _temp321.xtud; goto _LL324; _LL324:
_temp323=( struct Cyc_Absyn_Tunionfield*) _temp321.xtufd; goto _LL221;} else{
goto _LL222;} _LL222: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*)
_temp198)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL334: _temp333=( struct _tuple0*)((
struct Cyc_Absyn_EnumType_struct*) _temp198)->f1; goto _LL332; _LL332: _temp331=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp198)->f2;
goto _LL223;} else{ goto _LL224;} _LL224: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_IntType_tag: 0){ _LL338:
_temp337=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if(( int)
_temp337 == Cyc_Absyn_Signed){ goto _LL336;} else{ goto _LL226;} _LL336:
_temp335=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if(( int)
_temp335 == Cyc_Absyn_B1){ goto _LL225;} else{ goto _LL226;}} else{ goto _LL226;}
_LL226: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL342: _temp341=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp198)->f1; if(( int) _temp341 == Cyc_Absyn_Unsigned){ goto _LL340;} else{
goto _LL228;} _LL340: _temp339=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp198)->f2; if(( int) _temp339 == Cyc_Absyn_B1){ goto _LL227;} else{ goto
_LL228;}} else{ goto _LL228;} _LL228: if(( unsigned int) _temp198 > 4u?(( struct
_tunion_struct*) _temp198)->tag == Cyc_Absyn_IntType_tag: 0){ _LL346: _temp345=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if(( int) _temp345 ==
Cyc_Absyn_Signed){ goto _LL344;} else{ goto _LL230;} _LL344: _temp343=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if(( int) _temp343 == Cyc_Absyn_B2){
goto _LL229;} else{ goto _LL230;}} else{ goto _LL230;} _LL230: if(( unsigned int)
_temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_IntType_tag:
0){ _LL350: _temp349=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1;
if(( int) _temp349 == Cyc_Absyn_Unsigned){ goto _LL348;} else{ goto _LL232;}
_LL348: _temp347=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if((
int) _temp347 == Cyc_Absyn_B2){ goto _LL231;} else{ goto _LL232;}} else{ goto
_LL232;} _LL232: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*)
_temp198)->tag == Cyc_Absyn_IntType_tag: 0){ _LL354: _temp353=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp198)->f1; if(( int) _temp353 == Cyc_Absyn_Signed){
goto _LL352;} else{ goto _LL234;} _LL352: _temp351=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp198)->f2; if(( int) _temp351 == Cyc_Absyn_B4){ goto _LL233;} else{ goto
_LL234;}} else{ goto _LL234;} _LL234: if(( unsigned int) _temp198 > 4u?(( struct
_tunion_struct*) _temp198)->tag == Cyc_Absyn_IntType_tag: 0){ _LL358: _temp357=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if(( int) _temp357 ==
Cyc_Absyn_Unsigned){ goto _LL356;} else{ goto _LL236;} _LL356: _temp355=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if(( int) _temp355 == Cyc_Absyn_B4){
goto _LL235;} else{ goto _LL236;}} else{ goto _LL236;} _LL236: if(( unsigned int)
_temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_IntType_tag:
0){ _LL362: _temp361=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1;
if(( int) _temp361 == Cyc_Absyn_Signed){ goto _LL360;} else{ goto _LL238;}
_LL360: _temp359=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if((
int) _temp359 == Cyc_Absyn_B8){ goto _LL237;} else{ goto _LL238;}} else{ goto
_LL238;} _LL238: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*)
_temp198)->tag == Cyc_Absyn_IntType_tag: 0){ _LL366: _temp365=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp198)->f1; if(( int) _temp365 == Cyc_Absyn_Unsigned){
goto _LL364;} else{ goto _LL240;} _LL364: _temp363=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp198)->f2; if(( int) _temp363 == Cyc_Absyn_B8){ goto _LL239;} else{ goto
_LL240;}} else{ goto _LL240;} _LL240: if(( int) _temp198 == Cyc_Absyn_FloatType){
goto _LL241;} else{ goto _LL242;} _LL242: if(( int) _temp198 == Cyc_Absyn_DoubleType){
goto _LL243;} else{ goto _LL244;} _LL244: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL368:
_temp367=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp198)->f1;
goto _LL245;} else{ goto _LL246;} _LL246: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_StructType_tag: 0){ _LL374:
_temp373=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp198)->f1;
if( _temp373 == 0){ goto _LL372;} else{ goto _LL248;} _LL372: _temp371=( struct
Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp198)->f2; goto
_LL370; _LL370: _temp369=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp198)->f3; goto _LL247;} else{ goto _LL248;} _LL248: if(( unsigned int)
_temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_StructType_tag:
0){ _LL380: _temp379=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp198)->f1; goto _LL378; _LL378: _temp377=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp198)->f2; goto _LL376; _LL376: _temp375=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp198)->f3;
goto _LL249;} else{ goto _LL250;} _LL250: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL386:
_temp385=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp198)->f1;
if( _temp385 == 0){ goto _LL384;} else{ goto _LL252;} _LL384: _temp383=( struct
Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp198)->f2; goto _LL382;
_LL382: _temp381=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp198)->f3; goto _LL251;} else{ goto _LL252;} _LL252: if(( unsigned int)
_temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL392: _temp391=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp198)->f1; goto _LL390; _LL390: _temp389=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp198)->f2; goto _LL388; _LL388: _temp387=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp198)->f3;
goto _LL253;} else{ goto _LL254;} _LL254: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL398:
_temp397=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp198)->f1;
goto _LL396; _LL396: _temp395=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp198)->f2; goto _LL394; _LL394: _temp393=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp198)->f3; goto _LL255;} else{ goto _LL256;}
_LL256: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL400: _temp399=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp198)->f1; goto _LL257;} else{ goto _LL258;} _LL258: if(( int) _temp198 ==
Cyc_Absyn_HeapRgn){ goto _LL259;} else{ goto _LL260;} _LL260: if(( unsigned int)
_temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL402: _temp401=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp198)->f1;
goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL404:
_temp403=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp198)->f1;
goto _LL263;} else{ goto _LL199;} _LL201: return; _LL203: return; _LL205:
return; _LL207: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp405=(
char*)"void"; struct _tagged_string _temp406; _temp406.curr= _temp405; _temp406.base=
_temp405; _temp406.last_plus_one= _temp405 + 5; _temp406;})); return; _LL209:
Cyc_Absyndump_dump_str( _temp274->name); return; _LL211: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp407=( char*)"%"; struct _tagged_string
_temp408; _temp408.curr= _temp407; _temp408.base= _temp407; _temp408.last_plus_one=
_temp407 + 2; _temp408;})); Cyc_Absyndump_dumpkind( _temp280); Cyc_Absyndump_dump(
xprintf("(%d)", _temp276)); return; _LL213: Cyc_Absyndump_dumpntyp( _temp287);
return; _LL215: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp409=(
char*)"tunion "; struct _tagged_string _temp410; _temp410.curr= _temp409;
_temp410.base= _temp409; _temp410.last_plus_one= _temp409 + 8; _temp410;}));{
void* _temp411= Cyc_Tcutil_compress( _temp295); _LL413: if(( int) _temp411 ==
Cyc_Absyn_HeapRgn){ goto _LL414;} else{ goto _LL415;} _LL415: goto _LL416;
_LL414: goto _LL412; _LL416: Cyc_Absyndump_dumptyp( _temp295); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp417=( char*)" "; struct _tagged_string
_temp418; _temp418.curr= _temp417; _temp418.base= _temp417; _temp418.last_plus_one=
_temp417 + 2; _temp418;})); goto _LL412; _LL412:;} Cyc_Absyndump_dumpqvar(
_temp299); Cyc_Absyndump_dumptps( _temp297); goto _LL199; _LL217: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp419=( char*)"xtunion "; struct
_tagged_string _temp420; _temp420.curr= _temp419; _temp420.base= _temp419;
_temp420.last_plus_one= _temp419 + 9; _temp420;}));{ void* _temp421= Cyc_Tcutil_compress(
_temp305); _LL423: if(( int) _temp421 == Cyc_Absyn_HeapRgn){ goto _LL424;} else{
goto _LL425;} _LL425: goto _LL426; _LL424: goto _LL422; _LL426: Cyc_Absyndump_dumptyp(
_temp305); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp427=( char*)" ";
struct _tagged_string _temp428; _temp428.curr= _temp427; _temp428.base= _temp427;
_temp428.last_plus_one= _temp427 + 2; _temp428;})); goto _LL422; _LL422:;} Cyc_Absyndump_dumpqvar(
_temp307); goto _LL199; _LL219: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp429=( char*)"tunion "; struct _tagged_string _temp430; _temp430.curr=
_temp429; _temp430.base= _temp429; _temp430.last_plus_one= _temp429 + 8;
_temp430;})); Cyc_Absyndump_dumpqvar( _temp319); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp431=( char*)"."; struct _tagged_string _temp432;
_temp432.curr= _temp431; _temp432.base= _temp431; _temp432.last_plus_one=
_temp431 + 2; _temp432;})); Cyc_Absyndump_dumpqvar( _temp315); Cyc_Absyndump_dumptps(
_temp317); goto _LL199; _LL221: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp433=( char*)"xtunion "; struct _tagged_string _temp434; _temp434.curr=
_temp433; _temp434.base= _temp433; _temp434.last_plus_one= _temp433 + 9;
_temp434;})); Cyc_Absyndump_dumpqvar( _temp329); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp435=( char*)"."; struct _tagged_string _temp436;
_temp436.curr= _temp435; _temp436.base= _temp435; _temp436.last_plus_one=
_temp435 + 2; _temp436;})); Cyc_Absyndump_dumpqvar( _temp327); goto _LL199;
_LL223: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp437=( char*)"enum ";
struct _tagged_string _temp438; _temp438.curr= _temp437; _temp438.base= _temp437;
_temp438.last_plus_one= _temp437 + 6; _temp438;})); Cyc_Absyndump_dumpqvar(
_temp333); return; _LL225: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp439=( char*)"signed char"; struct _tagged_string _temp440; _temp440.curr=
_temp439; _temp440.base= _temp439; _temp440.last_plus_one= _temp439 + 12;
_temp440;})); return; _LL227: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp441=( char*)"char"; struct _tagged_string _temp442; _temp442.curr= _temp441;
_temp442.base= _temp441; _temp442.last_plus_one= _temp441 + 5; _temp442;}));
return; _LL229: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp443=(
char*)"short"; struct _tagged_string _temp444; _temp444.curr= _temp443; _temp444.base=
_temp443; _temp444.last_plus_one= _temp443 + 6; _temp444;})); return; _LL231:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp445=( char*)"unsigned short";
struct _tagged_string _temp446; _temp446.curr= _temp445; _temp446.base= _temp445;
_temp446.last_plus_one= _temp445 + 15; _temp446;})); return; _LL233: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp447=( char*)"int"; struct _tagged_string
_temp448; _temp448.curr= _temp447; _temp448.base= _temp447; _temp448.last_plus_one=
_temp447 + 4; _temp448;})); return; _LL235: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp449=( char*)"unsigned int"; struct _tagged_string
_temp450; _temp450.curr= _temp449; _temp450.base= _temp449; _temp450.last_plus_one=
_temp449 + 13; _temp450;})); return; _LL237: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp451=( char*)"long long"; struct _tagged_string
_temp452; _temp452.curr= _temp451; _temp452.base= _temp451; _temp452.last_plus_one=
_temp451 + 10; _temp452;})); return; _LL239: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp453=( char*)"unsigned long long"; struct
_tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 19; _temp454;})); return; _LL241: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp455=( char*)"float"; struct _tagged_string
_temp456; _temp456.curr= _temp455; _temp456.base= _temp455; _temp456.last_plus_one=
_temp455 + 6; _temp456;})); return; _LL243: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp457=( char*)"double"; struct _tagged_string
_temp458; _temp458.curr= _temp457; _temp458.base= _temp457; _temp458.last_plus_one=
_temp457 + 7; _temp458;})); return; _LL245: Cyc_Absyndump_dump_char(( int)'$');
Cyc_Absyndump_dumpargs( _temp367); return; _LL247: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp459=( char*)"struct"; struct _tagged_string
_temp460; _temp460.curr= _temp459; _temp460.base= _temp459; _temp460.last_plus_one=
_temp459 + 7; _temp460;})); Cyc_Absyndump_dumptps( _temp371); return; _LL249:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp461=( char*)"struct";
struct _tagged_string _temp462; _temp462.curr= _temp461; _temp462.base= _temp461;
_temp462.last_plus_one= _temp461 + 7; _temp462;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct _tuple0* _temp463= _temp379; if( _temp463 == 0){
_throw( Null_Exception);} _temp463;})); Cyc_Absyndump_dumptps( _temp377);
return; _LL251: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp464=(
char*)"union"; struct _tagged_string _temp465; _temp465.curr= _temp464; _temp465.base=
_temp464; _temp465.last_plus_one= _temp464 + 6; _temp465;})); Cyc_Absyndump_dumptps(
_temp383); return; _LL253: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp466=( char*)"union"; struct _tagged_string _temp467; _temp467.curr=
_temp466; _temp467.base= _temp466; _temp467.last_plus_one= _temp466 + 6;
_temp467;})); Cyc_Absyndump_dumpqvar(( struct _tuple0*)({ struct _tuple0*
_temp468= _temp391; if( _temp468 == 0){ _throw( Null_Exception);} _temp468;}));
Cyc_Absyndump_dumptps( _temp389); return; _LL255: Cyc_Absyndump_dumpqvar(
_temp397), Cyc_Absyndump_dumptps( _temp395); return; _LL257: Cyc_Absyndump_dumprgn(
_temp399); return; _LL259: return; _LL261: return; _LL263: return; _LL199:;}
void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp469= vo; if( _temp469 == 0){
_throw( Null_Exception);} _temp469->v;}));}} void Cyc_Absyndump_dumpfunarg(
struct _tuple1* t){(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct
Cyc_Core_Opt*), struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)(( struct Cyc_Absyn_Tqual*)(*
t).f2,(* t).f3, Cyc_Absyndump_dumpvaropt,(* t).f1);} void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt){ Cyc_Absyndump_dump_char((
int)'('); for( 0; args != 0; args=({ struct Cyc_List_List* _temp470= args; if(
_temp470 == 0){ _throw( Null_Exception);} _temp470->tl;})){ Cyc_Absyndump_dumpfunarg((
struct _tuple1*)({ struct Cyc_List_List* _temp471= args; if( _temp471 == 0){
_throw( Null_Exception);} _temp471->hd;})); if(({ struct Cyc_List_List* _temp472=
args; if( _temp472 == 0){ _throw( Null_Exception);} _temp472->tl;}) != 0? 1:
varargs){ Cyc_Absyndump_dump_char(( int)',');}} if( varargs){ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp473=( char*)"..."; struct _tagged_string
_temp474; _temp474.curr= _temp473; _temp474.base= _temp473; _temp474.last_plus_one=
_temp473 + 4; _temp474;}));} if( effopt != 0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff((
void*)({ struct Cyc_Core_Opt* _temp475= effopt; if( _temp475 == 0){ _throw(
Null_Exception);} _temp475->v;}));} Cyc_Absyndump_dump_char(( int)')');} void
Cyc_Absyndump_dumptyp( void* t){(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(*
f)( int), int)) Cyc_Absyndump_dumptqtd)( 0, t,( void(*)( int x)) Cyc_Absyndump_ignore,
0);} void Cyc_Absyndump_dumpdesignator( void* d){ void* _temp476= d; struct Cyc_Absyn_Exp*
_temp482; struct _tagged_string* _temp484; _LL478: if((( struct _tunion_struct*)
_temp476)->tag == Cyc_Absyn_ArrayElement_tag){ _LL483: _temp482=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp476)->f1; goto _LL479;} else{ goto
_LL480;} _LL480: if((( struct _tunion_struct*) _temp476)->tag == Cyc_Absyn_FieldName_tag){
_LL485: _temp484=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp476)->f1; goto _LL481;} else{ goto _LL477;} _LL479: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp486=( char*)".["; struct _tagged_string
_temp487; _temp487.curr= _temp486; _temp487.base= _temp486; _temp487.last_plus_one=
_temp486 + 3; _temp487;})); Cyc_Absyndump_dumpexp( _temp482); Cyc_Absyndump_dump_char((
int)']'); goto _LL477; _LL481: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp484); goto _LL477; _LL477:;} void Cyc_Absyndump_dumpde( struct _tuple7* de){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* de).f1,( struct _tagged_string)({ char* _temp488=(
char*)""; struct _tagged_string _temp489; _temp489.curr= _temp488; _temp489.base=
_temp488; _temp489.last_plus_one= _temp488 + 1; _temp489;}),( struct
_tagged_string)({ char* _temp490=( char*)"="; struct _tagged_string _temp491;
_temp491.curr= _temp490; _temp491.base= _temp490; _temp491.last_plus_one=
_temp490 + 2; _temp491;}),( struct _tagged_string)({ char* _temp492=( char*)"=";
struct _tagged_string _temp493; _temp493.curr= _temp492; _temp493.base= _temp492;
_temp493.last_plus_one= _temp492 + 2; _temp493;})); Cyc_Absyndump_dumpexp((* de).f2);}
void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){(( void(*)(
void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec, inprec, es,( struct _tagged_string)({ char* _temp494=(
char*)""; struct _tagged_string _temp495; _temp495.curr= _temp494; _temp495.base=
_temp494; _temp495.last_plus_one= _temp494 + 1; _temp495;}),( struct
_tagged_string)({ char* _temp496=( char*)""; struct _tagged_string _temp497;
_temp497.curr= _temp496; _temp497.base= _temp496; _temp497.last_plus_one=
_temp496 + 1; _temp497;}),( struct _tagged_string)({ char* _temp498=( char*)",";
struct _tagged_string _temp499; _temp499.curr= _temp498; _temp499.base= _temp498;
_temp499.last_plus_one= _temp498 + 2; _temp499;}));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >= myprec){ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp500=( char*)"("; struct _tagged_string _temp501; _temp501.curr= _temp500;
_temp501.base= _temp500; _temp501.last_plus_one= _temp500 + 2; _temp501;}));}{
void* _temp502=( void*) e->r; void* _temp594; char _temp596; void* _temp598;
void* _temp600; short _temp602; void* _temp604; void* _temp606; int _temp608;
void* _temp610; void* _temp612; int _temp614; void* _temp616; void* _temp618;
long long _temp620; void* _temp622; void* _temp624; struct _tagged_string
_temp626; void* _temp628; void* _temp630; struct _tagged_string _temp632; struct
_tuple0* _temp634; void* _temp636; struct _tuple0* _temp638; struct Cyc_List_List*
_temp640; void* _temp642; struct Cyc_Absyn_Exp* _temp644; struct Cyc_Core_Opt*
_temp646; struct Cyc_Absyn_Exp* _temp648; void* _temp650; struct Cyc_Absyn_Exp*
_temp652; void* _temp654; struct Cyc_Absyn_Exp* _temp656; void* _temp658; struct
Cyc_Absyn_Exp* _temp660; void* _temp662; struct Cyc_Absyn_Exp* _temp664; struct
Cyc_Absyn_Exp* _temp666; struct Cyc_Absyn_Exp* _temp668; struct Cyc_Absyn_Exp*
_temp670; struct Cyc_Absyn_Exp* _temp672; struct Cyc_Absyn_Exp* _temp674; struct
Cyc_List_List* _temp676; struct Cyc_Absyn_Exp* _temp678; struct Cyc_List_List*
_temp680; struct Cyc_Absyn_Exp* _temp682; struct Cyc_Absyn_Exp* _temp684; struct
Cyc_Absyn_Exp* _temp686; struct Cyc_List_List* _temp688; struct Cyc_Absyn_Exp*
_temp690; struct Cyc_Absyn_Exp* _temp692; void* _temp694; struct Cyc_Absyn_Exp*
_temp696; struct Cyc_Absyn_Exp* _temp698; void* _temp700; struct Cyc_Absyn_Exp*
_temp702; struct Cyc_Absyn_Exp* _temp704; struct _tagged_string* _temp706;
struct Cyc_Absyn_Exp* _temp708; struct _tagged_string* _temp710; struct Cyc_Absyn_Exp*
_temp712; struct Cyc_Absyn_Exp* _temp714; struct Cyc_Absyn_Exp* _temp716; struct
Cyc_List_List* _temp718; struct Cyc_List_List* _temp720; struct _tuple1*
_temp722; struct Cyc_List_List* _temp724; struct Cyc_Absyn_Exp* _temp726; struct
Cyc_Absyn_Exp* _temp728; struct Cyc_Absyn_Vardecl* _temp730; struct Cyc_Absyn_Structdecl*
_temp732; struct Cyc_List_List* _temp734; struct Cyc_Core_Opt* _temp736; struct
_tuple0* _temp738; struct Cyc_Absyn_Tunionfield* _temp740; struct Cyc_Absyn_Tuniondecl*
_temp742; struct Cyc_List_List* _temp744; struct Cyc_Core_Opt* _temp746; struct
Cyc_Core_Opt* _temp748; struct Cyc_Absyn_Tunionfield* _temp750; struct Cyc_Absyn_XTuniondecl*
_temp752; struct Cyc_List_List* _temp754; struct Cyc_Core_Opt* _temp756; struct
Cyc_Absyn_Enumfield* _temp758; struct Cyc_Absyn_Enumdecl* _temp760; struct
_tuple0* _temp762; void* _temp764; struct Cyc_List_List* _temp766; struct Cyc_Core_Opt*
_temp768; struct Cyc_Absyn_Stmt* _temp770; struct Cyc_Absyn_Fndecl* _temp772;
struct Cyc_Absyn_Exp* _temp774; _LL504: if((( struct _tunion_struct*) _temp502)->tag
== Cyc_Absyn_Const_e_tag){ _LL595: _temp594=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp502)->f1; if(( unsigned int) _temp594 > 1u?(( struct _tunion_struct*)
_temp594)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL599: _temp598=( void*)(( struct
Cyc_Absyn_Char_c_struct*) _temp594)->f1; goto _LL597; _LL597: _temp596=( char)((
struct Cyc_Absyn_Char_c_struct*) _temp594)->f2; goto _LL505;} else{ goto _LL506;}}
else{ goto _LL506;} _LL506: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_Const_e_tag){
_LL601: _temp600=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp502)->f1; if((
unsigned int) _temp600 > 1u?(( struct _tunion_struct*) _temp600)->tag == Cyc_Absyn_Short_c_tag:
0){ _LL605: _temp604=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp600)->f1;
goto _LL603; _LL603: _temp602=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp600)->f2; goto _LL507;} else{ goto _LL508;}} else{ goto _LL508;} _LL508:
if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_Const_e_tag){ _LL607:
_temp606=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp502)->f1; if((
unsigned int) _temp606 > 1u?(( struct _tunion_struct*) _temp606)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL611: _temp610=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp606)->f1;
if(( int) _temp610 == Cyc_Absyn_Signed){ goto _LL609;} else{ goto _LL510;}
_LL609: _temp608=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp606)->f2; goto
_LL509;} else{ goto _LL510;}} else{ goto _LL510;} _LL510: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_Const_e_tag){ _LL613: _temp612=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp502)->f1; if(( unsigned int)
_temp612 > 1u?(( struct _tunion_struct*) _temp612)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL617: _temp616=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp612)->f1;
if(( int) _temp616 == Cyc_Absyn_Unsigned){ goto _LL615;} else{ goto _LL512;}
_LL615: _temp614=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp612)->f2; goto
_LL511;} else{ goto _LL512;}} else{ goto _LL512;} _LL512: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_Const_e_tag){ _LL619: _temp618=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp502)->f1; if(( unsigned int)
_temp618 > 1u?(( struct _tunion_struct*) _temp618)->tag == Cyc_Absyn_LongLong_c_tag:
0){ _LL623: _temp622=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp618)->f1;
goto _LL621; _LL621: _temp620=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp618)->f2; goto _LL513;} else{ goto _LL514;}} else{ goto _LL514;} _LL514:
if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_Const_e_tag){ _LL625:
_temp624=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp502)->f1; if((
unsigned int) _temp624 > 1u?(( struct _tunion_struct*) _temp624)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL627: _temp626=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp624)->f1; goto _LL515;} else{ goto _LL516;}} else{ goto _LL516;} _LL516:
if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_Const_e_tag){ _LL629:
_temp628=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp502)->f1; if(( int)
_temp628 == Cyc_Absyn_Null_c){ goto _LL517;} else{ goto _LL518;}} else{ goto
_LL518;} _LL518: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_Const_e_tag){
_LL631: _temp630=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp502)->f1; if((
unsigned int) _temp630 > 1u?(( struct _tunion_struct*) _temp630)->tag == Cyc_Absyn_String_c_tag:
0){ _LL633: _temp632=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp630)->f1; goto _LL519;} else{ goto _LL520;}} else{ goto _LL520;} _LL520:
if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL635: _temp634=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp502)->f1; goto _LL521;} else{ goto _LL522;} _LL522: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_Var_e_tag){ _LL639: _temp638=(
struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp502)->f1; goto _LL637;
_LL637: _temp636=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp502)->f2; goto
_LL523;} else{ goto _LL524;} _LL524: if((( struct _tunion_struct*) _temp502)->tag
== Cyc_Absyn_Primop_e_tag){ _LL643: _temp642=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp502)->f1; goto _LL641; _LL641: _temp640=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp502)->f2; goto _LL525;} else{ goto _LL526;}
_LL526: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL649: _temp648=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp502)->f1; goto _LL647; _LL647: _temp646=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp502)->f2; goto _LL645; _LL645: _temp644=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp502)->f3;
goto _LL527;} else{ goto _LL528;} _LL528: if((( struct _tunion_struct*) _temp502)->tag
== Cyc_Absyn_Increment_e_tag){ _LL653: _temp652=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp502)->f1; goto _LL651; _LL651:
_temp650=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp502)->f2; if((
int) _temp650 == Cyc_Absyn_PreInc){ goto _LL529;} else{ goto _LL530;}} else{
goto _LL530;} _LL530: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_Increment_e_tag){
_LL657: _temp656=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp502)->f1; goto _LL655; _LL655: _temp654=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp502)->f2; if(( int) _temp654 == Cyc_Absyn_PreDec){ goto _LL531;} else{ goto
_LL532;}} else{ goto _LL532;} _LL532: if((( struct _tunion_struct*) _temp502)->tag
== Cyc_Absyn_Increment_e_tag){ _LL661: _temp660=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp502)->f1; goto _LL659; _LL659:
_temp658=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp502)->f2; if((
int) _temp658 == Cyc_Absyn_PostInc){ goto _LL533;} else{ goto _LL534;}} else{
goto _LL534;} _LL534: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_Increment_e_tag){
_LL665: _temp664=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp502)->f1; goto _LL663; _LL663: _temp662=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp502)->f2; if(( int) _temp662 == Cyc_Absyn_PostDec){ goto _LL535;} else{
goto _LL536;}} else{ goto _LL536;} _LL536: if((( struct _tunion_struct*)
_temp502)->tag == Cyc_Absyn_Conditional_e_tag){ _LL671: _temp670=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp502)->f1; goto _LL669; _LL669:
_temp668=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp502)->f2; goto _LL667; _LL667: _temp666=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp502)->f3; goto _LL537;} else{ goto _LL538;}
_LL538: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL675: _temp674=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp502)->f1; goto _LL673; _LL673: _temp672=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp502)->f2; goto _LL539;} else{ goto _LL540;}
_LL540: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL679: _temp678=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp502)->f1; goto _LL677; _LL677: _temp676=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp502)->f2; goto _LL541;} else{ goto _LL542;}
_LL542: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_FnCall_e_tag){
_LL683: _temp682=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp502)->f1; goto _LL681; _LL681: _temp680=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp502)->f2; goto _LL543;} else{ goto _LL544;}
_LL544: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_Throw_e_tag){
_LL685: _temp684=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp502)->f1; goto _LL545;} else{ goto _LL546;} _LL546: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL687:
_temp686=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp502)->f1; goto _LL547;} else{ goto _LL548;} _LL548: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL691:
_temp690=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp502)->f1; goto _LL689; _LL689: _temp688=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp502)->f2; goto _LL549;} else{ goto _LL550;}
_LL550: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_Cast_e_tag){
_LL695: _temp694=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp502)->f1; goto
_LL693; _LL693: _temp692=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp502)->f2; goto _LL551;} else{ goto _LL552;} _LL552: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_Address_e_tag){ _LL697: _temp696=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp502)->f1; goto
_LL553;} else{ goto _LL554;} _LL554: if((( struct _tunion_struct*) _temp502)->tag
== Cyc_Absyn_New_e_tag){ _LL699: _temp698=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp502)->f1; goto _LL555;} else{ goto _LL556;} _LL556: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL701: _temp700=(
void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp502)->f1; goto _LL557;}
else{ goto _LL558;} _LL558: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL703: _temp702=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp502)->f1; goto _LL559;} else{ goto _LL560;} _LL560: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_Deref_e_tag){ _LL705: _temp704=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp502)->f1; goto
_LL561;} else{ goto _LL562;} _LL562: if((( struct _tunion_struct*) _temp502)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL709: _temp708=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp502)->f1; goto _LL707; _LL707:
_temp706=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp502)->f2; goto _LL563;} else{ goto _LL564;} _LL564: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL713:
_temp712=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp502)->f1; goto _LL711; _LL711: _temp710=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp502)->f2; goto _LL565;} else{ goto _LL566;}
_LL566: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_Subscript_e_tag){
_LL717: _temp716=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp502)->f1; goto _LL715; _LL715: _temp714=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp502)->f2; goto _LL567;} else{ goto _LL568;}
_LL568: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_Tuple_e_tag){
_LL719: _temp718=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp502)->f1; goto _LL569;} else{ goto _LL570;} _LL570: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL723:
_temp722=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp502)->f1;
goto _LL721; _LL721: _temp720=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp502)->f2; goto _LL571;} else{ goto _LL572;} _LL572: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_Array_e_tag){ _LL725: _temp724=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp502)->f1; goto
_LL573;} else{ goto _LL574;} _LL574: if((( struct _tunion_struct*) _temp502)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL731: _temp730=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp502)->f1; goto _LL729; _LL729:
_temp728=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp502)->f2; goto _LL727; _LL727: _temp726=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp502)->f3; goto _LL575;} else{ goto
_LL576;} _LL576: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_Struct_e_tag){
_LL739: _temp738=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp502)->f1; goto _LL737; _LL737: _temp736=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp502)->f2; goto _LL735; _LL735: _temp734=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp502)->f3; goto
_LL733; _LL733: _temp732=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp502)->f4; goto _LL577;} else{ goto _LL578;} _LL578: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_Tunion_e_tag){ _LL749: _temp748=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp502)->f1; goto
_LL747; _LL747: _temp746=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp502)->f2; goto _LL745; _LL745: _temp744=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp502)->f3; goto _LL743; _LL743: _temp742=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp502)->f4;
goto _LL741; _LL741: _temp740=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp502)->f5; goto _LL579;} else{ goto _LL580;} _LL580: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_XTunion_e_tag){ _LL757: _temp756=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp502)->f1; goto
_LL755; _LL755: _temp754=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp502)->f2; goto _LL753; _LL753: _temp752=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp502)->f3; goto _LL751; _LL751: _temp750=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp502)->f4;
goto _LL581;} else{ goto _LL582;} _LL582: if((( struct _tunion_struct*) _temp502)->tag
== Cyc_Absyn_Enum_e_tag){ _LL763: _temp762=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp502)->f1; goto _LL761; _LL761: _temp760=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp502)->f2; goto _LL759; _LL759: _temp758=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp502)->f3;
goto _LL583;} else{ goto _LL584;} _LL584: if((( struct _tunion_struct*) _temp502)->tag
== Cyc_Absyn_Malloc_e_tag){ _LL765: _temp764=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp502)->f1; goto _LL585;} else{ goto _LL586;} _LL586: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL769:
_temp768=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp502)->f1; goto _LL767; _LL767: _temp766=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp502)->f2; goto _LL587;} else{ goto
_LL588;} _LL588: if((( struct _tunion_struct*) _temp502)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL771: _temp770=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp502)->f1; goto _LL589;} else{ goto _LL590;} _LL590: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_Codegen_e_tag){ _LL773: _temp772=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp502)->f1;
goto _LL591;} else{ goto _LL592;} _LL592: if((( struct _tunion_struct*) _temp502)->tag
== Cyc_Absyn_Fill_e_tag){ _LL775: _temp774=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp502)->f1; goto _LL593;} else{ goto _LL503;} _LL505: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp596)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL503; _LL507: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp776=( char*)"(short)"; struct _tagged_string _temp777; _temp777.curr=
_temp776; _temp777.base= _temp776; _temp777.last_plus_one= _temp776 + 8;
_temp777;})); Cyc_Absyndump_dump_nospace( Cyc_Core_string_of_int(( int) _temp602));
goto _LL503; _LL509: Cyc_Absyndump_dump( Cyc_Core_string_of_int( _temp608));
goto _LL503; _LL511: Cyc_Absyndump_dump( Cyc_Core_string_of_int( _temp614)); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp778=( char*)"u"; struct _tagged_string
_temp779; _temp779.curr= _temp778; _temp779.base= _temp778; _temp779.last_plus_one=
_temp778 + 2; _temp779;})); goto _LL503; _LL513: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp780=( char*)"<<FIX LONG LONG CONSTANT>>"; struct
_tagged_string _temp781; _temp781.curr= _temp780; _temp781.base= _temp780;
_temp781.last_plus_one= _temp780 + 27; _temp781;})); goto _LL503; _LL515: Cyc_Absyndump_dump(
_temp626); goto _LL503; _LL517: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp782=( char*)"null"; struct _tagged_string _temp783; _temp783.curr=
_temp782; _temp783.base= _temp782; _temp783.last_plus_one= _temp782 + 5;
_temp783;})); goto _LL503; _LL519: Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp632)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL503; _LL521: _temp638= _temp634; goto _LL523; _LL523: Cyc_Absyndump_dumpqvar(
_temp638); goto _LL503; _LL525: { struct _tagged_string pstr= Cyc_Absynpp_prim2str(
_temp642); if( ! Cyc_Absyn_is_format_prim( _temp642)){ switch((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp640)){ case 1: _LL784: if( _temp642 ==(
void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp786= _temp640; if( _temp786 == 0){ _throw(
Null_Exception);} _temp786->hd;})); Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp787=( char*)".size"; struct _tagged_string _temp788; _temp788.curr=
_temp787; _temp788.base= _temp787; _temp788.last_plus_one= _temp787 + 6;
_temp788;}));} else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp789= _temp640; if(
_temp789 == 0){ _throw( Null_Exception);} _temp789->hd;}));} break; case 2:
_LL785: Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp791= _temp640; if( _temp791 == 0){ _throw( Null_Exception);} _temp791->hd;}));
Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp793=({ struct Cyc_List_List* _temp792= _temp640; if(
_temp792 == 0){ _throw( Null_Exception);} _temp792->tl;}); if( _temp793 == 0){
_throw( Null_Exception);} _temp793->hd;})); break; default: _LL790:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp795=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp795[ 0]=({ struct Cyc_Core_Failure_struct _temp796; _temp796.tag= Cyc_Core_Failure_tag;
_temp796.f1=( struct _tagged_string)({ char* _temp797=( char*)"Absyndump -- Bad number of arguments to primop";
struct _tagged_string _temp798; _temp798.curr= _temp797; _temp798.base= _temp797;
_temp798.last_plus_one= _temp797 + 47; _temp798;}); _temp796;}); _temp795;}));}}
else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp799=( char*)"("; struct _tagged_string _temp800;
_temp800.curr= _temp799; _temp800.base= _temp799; _temp800.last_plus_one=
_temp799 + 2; _temp800;})); Cyc_Absyndump_dumpexps_prec( 20, _temp640); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp801=( char*)")"; struct _tagged_string
_temp802; _temp802.curr= _temp801; _temp802.base= _temp801; _temp802.last_plus_one=
_temp801 + 2; _temp802;}));} goto _LL503;} _LL527: Cyc_Absyndump_dumpexp_prec(
myprec, _temp648); if( _temp646 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*)({ struct Cyc_Core_Opt* _temp803= _temp646; if( _temp803 == 0){ _throw(
Null_Exception);} _temp803->v;})));} Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp804=( char*)"="; struct _tagged_string _temp805;
_temp805.curr= _temp804; _temp805.base= _temp804; _temp805.last_plus_one=
_temp804 + 2; _temp805;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp644); goto
_LL503; _LL529: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp806=(
char*)"++"; struct _tagged_string _temp807; _temp807.curr= _temp806; _temp807.base=
_temp806; _temp807.last_plus_one= _temp806 + 3; _temp807;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp652); goto _LL503; _LL531: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp808=( char*)"--"; struct _tagged_string _temp809;
_temp809.curr= _temp808; _temp809.base= _temp808; _temp809.last_plus_one=
_temp808 + 3; _temp809;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp656); goto
_LL503; _LL533: Cyc_Absyndump_dumpexp_prec( myprec, _temp660); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp810=( char*)"++"; struct _tagged_string
_temp811; _temp811.curr= _temp810; _temp811.base= _temp810; _temp811.last_plus_one=
_temp810 + 3; _temp811;})); goto _LL503; _LL535: Cyc_Absyndump_dumpexp_prec(
myprec, _temp664); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp812=(
char*)"--"; struct _tagged_string _temp813; _temp813.curr= _temp812; _temp813.base=
_temp812; _temp813.last_plus_one= _temp812 + 3; _temp813;})); goto _LL503;
_LL537: Cyc_Absyndump_dumpexp_prec( myprec, _temp670); Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumpexp_prec( 0, _temp668); Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp_prec( myprec, _temp666); goto _LL503; _LL539:
Cyc_Absyndump_dumpexp_prec( myprec, _temp674); Cyc_Absyndump_dump_char(( int)',');
Cyc_Absyndump_dumpexp_prec( myprec, _temp672); goto _LL503; _LL541: _temp682=
_temp678; _temp680= _temp676; goto _LL543; _LL543: Cyc_Absyndump_dumpexp_prec(
myprec, _temp682); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp814=( char*)"("; struct _tagged_string _temp815; _temp815.curr= _temp814;
_temp815.base= _temp814; _temp815.last_plus_one= _temp814 + 2; _temp815;})); Cyc_Absyndump_dumpexps_prec(
20, _temp680); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp816=( char*)")"; struct _tagged_string _temp817; _temp817.curr= _temp816;
_temp817.base= _temp816; _temp817.last_plus_one= _temp816 + 2; _temp817;}));
goto _LL503; _LL545: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp818=( char*)"throw"; struct _tagged_string _temp819; _temp819.curr=
_temp818; _temp819.base= _temp818; _temp819.last_plus_one= _temp818 + 6;
_temp819;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp684); goto _LL503; _LL547:
_temp690= _temp686; goto _LL549; _LL549: Cyc_Absyndump_dumpexp_prec( inprec,
_temp690); goto _LL503; _LL551: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp(
_temp694); Cyc_Absyndump_dump_char(( int)')'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp692); goto _LL503; _LL553: Cyc_Absyndump_dump_char(( int)'&'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp696); goto _LL503; _LL555: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp820=( char*)"new "; struct _tagged_string _temp821;
_temp821.curr= _temp820; _temp821.base= _temp820; _temp821.last_plus_one=
_temp820 + 5; _temp821;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp698); goto
_LL503; _LL557: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp822=(
char*)"sizeof("; struct _tagged_string _temp823; _temp823.curr= _temp822;
_temp823.base= _temp822; _temp823.last_plus_one= _temp822 + 8; _temp823;})); Cyc_Absyndump_dumptyp(
_temp700); Cyc_Absyndump_dump_char(( int)')'); goto _LL503; _LL559: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp824=( char*)"sizeof("; struct _tagged_string
_temp825; _temp825.curr= _temp824; _temp825.base= _temp824; _temp825.last_plus_one=
_temp824 + 8; _temp825;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp702); Cyc_Absyndump_dump_char((
int)')'); goto _LL503; _LL561: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp704); goto _LL503; _LL563: Cyc_Absyndump_dumpexp_prec( myprec,
_temp708); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp706); goto _LL503; _LL565: Cyc_Absyndump_dumpexp_prec( myprec, _temp712);
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp826=( char*)"->";
struct _tagged_string _temp827; _temp827.curr= _temp826; _temp827.base= _temp826;
_temp827.last_plus_one= _temp826 + 3; _temp827;})); Cyc_Absyndump_dump_nospace(*
_temp710); goto _LL503; _LL567: Cyc_Absyndump_dumpexp_prec( myprec, _temp716);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp714); Cyc_Absyndump_dump_char((
int)']'); goto _LL503; _LL569: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp828=( char*)"$("; struct _tagged_string _temp829; _temp829.curr=
_temp828; _temp829.base= _temp828; _temp829.last_plus_one= _temp828 + 3;
_temp829;})); Cyc_Absyndump_dumpexps_prec( 20, _temp718); Cyc_Absyndump_dump_char((
int)')'); goto _LL503; _LL571: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp722).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp720,( struct _tagged_string)({ char* _temp830=( char*)"{"; struct
_tagged_string _temp831; _temp831.curr= _temp830; _temp831.base= _temp830;
_temp831.last_plus_one= _temp830 + 2; _temp831;}),( struct _tagged_string)({
char* _temp832=( char*)"}"; struct _tagged_string _temp833; _temp833.curr=
_temp832; _temp833.base= _temp832; _temp833.last_plus_one= _temp832 + 2;
_temp833;}),( struct _tagged_string)({ char* _temp834=( char*)","; struct
_tagged_string _temp835; _temp835.curr= _temp834; _temp835.base= _temp834;
_temp835.last_plus_one= _temp834 + 2; _temp835;})); goto _LL503; _LL573:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp724,( struct _tagged_string)({ char* _temp836=( char*)"{";
struct _tagged_string _temp837; _temp837.curr= _temp836; _temp837.base= _temp836;
_temp837.last_plus_one= _temp836 + 2; _temp837;}),( struct _tagged_string)({
char* _temp838=( char*)"}"; struct _tagged_string _temp839; _temp839.curr=
_temp838; _temp839.base= _temp838; _temp839.last_plus_one= _temp838 + 2;
_temp839;}),( struct _tagged_string)({ char* _temp840=( char*)","; struct
_tagged_string _temp841; _temp841.curr= _temp840; _temp841.base= _temp840;
_temp841.last_plus_one= _temp840 + 2; _temp841;})); goto _LL503; _LL575: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp842=( char*)"new {for"; struct
_tagged_string _temp843; _temp843.curr= _temp842; _temp843.base= _temp842;
_temp843.last_plus_one= _temp842 + 9; _temp843;})); Cyc_Absyndump_dump_str((*
_temp730->name).f2); Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumpexp(
_temp728); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp( _temp726);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL503; _LL577: Cyc_Absyndump_dumpqvar(
_temp738);(( void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp734,( struct
_tagged_string)({ char* _temp844=( char*)"{"; struct _tagged_string _temp845;
_temp845.curr= _temp844; _temp845.base= _temp844; _temp845.last_plus_one=
_temp844 + 2; _temp845;}),( struct _tagged_string)({ char* _temp846=( char*)"}";
struct _tagged_string _temp847; _temp847.curr= _temp846; _temp847.base= _temp846;
_temp847.last_plus_one= _temp846 + 2; _temp847;}),( struct _tagged_string)({
char* _temp848=( char*)","; struct _tagged_string _temp849; _temp849.curr=
_temp848; _temp849.base= _temp848; _temp849.last_plus_one= _temp848 + 2;
_temp849;})); goto _LL503; _LL579: _temp754= _temp744; _temp750= _temp740; goto
_LL581; _LL581: Cyc_Absyndump_dumpqvar( _temp750->name); if( _temp754 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp754,( struct _tagged_string)({ char* _temp850=( char*)"(";
struct _tagged_string _temp851; _temp851.curr= _temp850; _temp851.base= _temp850;
_temp851.last_plus_one= _temp850 + 2; _temp851;}),( struct _tagged_string)({
char* _temp852=( char*)")"; struct _tagged_string _temp853; _temp853.curr=
_temp852; _temp853.base= _temp852; _temp853.last_plus_one= _temp852 + 2;
_temp853;}),( struct _tagged_string)({ char* _temp854=( char*)","; struct
_tagged_string _temp855; _temp855.curr= _temp854; _temp855.base= _temp854;
_temp855.last_plus_one= _temp854 + 2; _temp855;}));} goto _LL503; _LL583: Cyc_Absyndump_dumpqvar(
_temp762); goto _LL503; _LL585: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp856=( char*)"malloc(sizeof("; struct _tagged_string _temp857;
_temp857.curr= _temp856; _temp857.base= _temp856; _temp857.last_plus_one=
_temp856 + 15; _temp857;})); Cyc_Absyndump_dumptyp( _temp764); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp858=( char*)"))"; struct _tagged_string
_temp859; _temp859.curr= _temp858; _temp859.base= _temp858; _temp859.last_plus_one=
_temp858 + 3; _temp859;})); goto _LL503; _LL587:(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp766,( struct _tagged_string)({ char* _temp860=( char*)"{"; struct
_tagged_string _temp861; _temp861.curr= _temp860; _temp861.base= _temp860;
_temp861.last_plus_one= _temp860 + 2; _temp861;}),( struct _tagged_string)({
char* _temp862=( char*)"}"; struct _tagged_string _temp863; _temp863.curr=
_temp862; _temp863.base= _temp862; _temp863.last_plus_one= _temp862 + 2;
_temp863;}),( struct _tagged_string)({ char* _temp864=( char*)","; struct
_tagged_string _temp865; _temp865.curr= _temp864; _temp865.base= _temp864;
_temp865.last_plus_one= _temp864 + 2; _temp865;})); goto _LL503; _LL589: Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp866=( char*)"({"; struct _tagged_string
_temp867; _temp867.curr= _temp866; _temp867.base= _temp866; _temp867.last_plus_one=
_temp866 + 3; _temp867;})); Cyc_Absyndump_dumpstmt( _temp770); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp868=( char*)"})"; struct _tagged_string
_temp869; _temp869.curr= _temp868; _temp869.base= _temp868; _temp869.last_plus_one=
_temp868 + 3; _temp869;})); goto _LL503; _LL591: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp870=( char*)"codegen("; struct _tagged_string
_temp871; _temp871.curr= _temp870; _temp871.base= _temp870; _temp871.last_plus_one=
_temp870 + 9; _temp871;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp872=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp872->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp873=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp873[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp874; _temp874.tag= Cyc_Absyn_Fn_d_tag;
_temp874.f1= _temp772; _temp874;}); _temp873;})); _temp872->loc= e->loc;
_temp872;})); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp875=(
char*)")"; struct _tagged_string _temp876; _temp876.curr= _temp875; _temp876.base=
_temp875; _temp876.last_plus_one= _temp875 + 2; _temp876;})); goto _LL503;
_LL593: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp877=( char*)"fill(";
struct _tagged_string _temp878; _temp878.curr= _temp877; _temp878.base= _temp877;
_temp878.last_plus_one= _temp877 + 6; _temp878;})); Cyc_Absyndump_dumpexp(
_temp774); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp879=( char*)")";
struct _tagged_string _temp880; _temp880.curr= _temp879; _temp880.base= _temp879;
_temp880.last_plus_one= _temp879 + 2; _temp880;})); goto _LL503; _LL503:;} if(
inprec >= myprec){ Cyc_Absyndump_dump_char(( int)')');}} void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec( 0, e);} void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List* scs){ for( 0; scs != 0; scs=({ struct Cyc_List_List*
_temp881= scs; if( _temp881 == 0){ _throw( Null_Exception);} _temp881->tl;})){
struct Cyc_Absyn_Switch_clause* c=( struct Cyc_Absyn_Switch_clause*)({ struct
Cyc_List_List* _temp882= scs; if( _temp882 == 0){ _throw( Null_Exception);}
_temp882->hd;}); if( c->where_clause == 0?( void*)( c->pattern)->r ==( void*)
Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp883=( char*)"default:"; struct _tagged_string _temp884; _temp884.curr=
_temp883; _temp884.base= _temp883; _temp884.last_plus_one= _temp883 + 9;
_temp884;}));} else{ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp885=( char*)"case"; struct _tagged_string _temp886; _temp886.curr= _temp885;
_temp886.base= _temp885; _temp886.last_plus_one= _temp885 + 5; _temp886;})); Cyc_Absyndump_dumppat(
c->pattern); if( c->where_clause != 0){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp887=( char*)"&&"; struct _tagged_string _temp888;
_temp888.curr= _temp887; _temp888.base= _temp887; _temp888.last_plus_one=
_temp887 + 3; _temp888;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp889= c->where_clause; if( _temp889 == 0){ _throw(
Null_Exception);} _temp889;}));} Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp890=( char*)":"; struct _tagged_string _temp891;
_temp891.curr= _temp890; _temp891.base= _temp890; _temp891.last_plus_one=
_temp890 + 2; _temp891;}));} Cyc_Absyndump_dumpstmt( c->body);}} void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt* s){ void* _temp892=( void*) s->r; struct Cyc_Absyn_Exp*
_temp934; struct Cyc_Absyn_Stmt* _temp936; struct Cyc_Absyn_Stmt* _temp938;
struct Cyc_Absyn_Exp* _temp940; struct Cyc_Absyn_Exp* _temp942; struct Cyc_Absyn_Stmt*
_temp944; struct Cyc_Absyn_Stmt* _temp946; struct Cyc_Absyn_Exp* _temp948;
struct Cyc_Absyn_Stmt* _temp950; struct _tuple2 _temp952; struct Cyc_Absyn_Stmt*
_temp954; struct Cyc_Absyn_Exp* _temp956; struct Cyc_Absyn_Stmt* _temp958;
struct Cyc_Absyn_Stmt* _temp960; struct Cyc_Absyn_Stmt* _temp962; struct
_tagged_string* _temp964; struct Cyc_Absyn_Switch_clause** _temp966; struct Cyc_List_List*
_temp968; struct Cyc_Absyn_Switch_clause** _temp970; struct Cyc_List_List*
_temp972; struct Cyc_Absyn_Stmt* _temp974; struct _tuple2 _temp976; struct Cyc_Absyn_Stmt*
_temp978; struct Cyc_Absyn_Exp* _temp980; struct _tuple2 _temp982; struct Cyc_Absyn_Stmt*
_temp984; struct Cyc_Absyn_Exp* _temp986; struct Cyc_Absyn_Exp* _temp988; struct
Cyc_List_List* _temp990; struct Cyc_Absyn_Exp* _temp992; struct Cyc_Absyn_Stmt*
_temp994; struct Cyc_Absyn_Decl* _temp996; struct Cyc_Absyn_Stmt* _temp998;
struct Cyc_Absyn_Stmt* _temp1000; struct Cyc_Absyn_Stmt* _temp1002; struct
_tagged_string* _temp1004; struct _tuple2 _temp1006; struct Cyc_Absyn_Stmt*
_temp1008; struct Cyc_Absyn_Exp* _temp1010; struct Cyc_Absyn_Stmt* _temp1012;
struct Cyc_List_List* _temp1014; struct Cyc_Absyn_Stmt* _temp1016; _LL894: if((
int) _temp892 == Cyc_Absyn_Skip_s){ goto _LL895;} else{ goto _LL896;} _LL896:
if(( unsigned int) _temp892 > 1u?(( struct _tunion_struct*) _temp892)->tag ==
Cyc_Absyn_Exp_s_tag: 0){ _LL935: _temp934=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp892)->f1; goto _LL897;} else{ goto _LL898;} _LL898: if(( unsigned int)
_temp892 > 1u?(( struct _tunion_struct*) _temp892)->tag == Cyc_Absyn_Seq_s_tag:
0){ _LL939: _temp938=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp892)->f1; goto _LL937; _LL937: _temp936=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp892)->f2; goto _LL899;} else{ goto _LL900;} _LL900:
if(( unsigned int) _temp892 > 1u?(( struct _tunion_struct*) _temp892)->tag ==
Cyc_Absyn_Return_s_tag: 0){ _LL941: _temp940=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Return_s_struct*) _temp892)->f1; if( _temp940 == 0){ goto _LL901;}
else{ goto _LL902;}} else{ goto _LL902;} _LL902: if(( unsigned int) _temp892 > 1u?((
struct _tunion_struct*) _temp892)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL943:
_temp942=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*) _temp892)->f1;
goto _LL903;} else{ goto _LL904;} _LL904: if(( unsigned int) _temp892 > 1u?((
struct _tunion_struct*) _temp892)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL949:
_temp948=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp892)->f1; goto _LL947; _LL947: _temp946=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp892)->f2; goto _LL945; _LL945: _temp944=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp892)->f3;
goto _LL905;} else{ goto _LL906;} _LL906: if(( unsigned int) _temp892 > 1u?((
struct _tunion_struct*) _temp892)->tag == Cyc_Absyn_While_s_tag: 0){ _LL953:
_temp952=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp892)->f1;
_LL957: _temp956= _temp952.f1; goto _LL955; _LL955: _temp954= _temp952.f2; goto
_LL951; _LL951: _temp950=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp892)->f2; goto _LL907;} else{ goto _LL908;} _LL908: if(( unsigned int)
_temp892 > 1u?(( struct _tunion_struct*) _temp892)->tag == Cyc_Absyn_Break_s_tag:
0){ _LL959: _temp958=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp892)->f1; goto _LL909;} else{ goto _LL910;} _LL910: if(( unsigned int)
_temp892 > 1u?(( struct _tunion_struct*) _temp892)->tag == Cyc_Absyn_Continue_s_tag:
0){ _LL961: _temp960=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp892)->f1; goto _LL911;} else{ goto _LL912;} _LL912: if(( unsigned int)
_temp892 > 1u?(( struct _tunion_struct*) _temp892)->tag == Cyc_Absyn_Goto_s_tag:
0){ _LL965: _temp964=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp892)->f1; goto _LL963; _LL963: _temp962=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Goto_s_struct*) _temp892)->f2; goto _LL913;} else{ goto _LL914;}
_LL914: if(( unsigned int) _temp892 > 1u?(( struct _tunion_struct*) _temp892)->tag
== Cyc_Absyn_Fallthru_s_tag: 0){ _LL969: _temp968=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp892)->f1; if( _temp968 == 0){ goto
_LL967;} else{ goto _LL916;} _LL967: _temp966=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp892)->f2; goto _LL915;} else{ goto
_LL916;} _LL916: if(( unsigned int) _temp892 > 1u?(( struct _tunion_struct*)
_temp892)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL973: _temp972=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp892)->f1; goto _LL971; _LL971:
_temp970=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp892)->f2; goto _LL917;} else{ goto _LL918;} _LL918: if(( unsigned int)
_temp892 > 1u?(( struct _tunion_struct*) _temp892)->tag == Cyc_Absyn_For_s_tag:
0){ _LL989: _temp988=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp892)->f1; goto _LL983; _LL983: _temp982=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp892)->f2; _LL987: _temp986= _temp982.f1; goto _LL985; _LL985: _temp984=
_temp982.f2; goto _LL977; _LL977: _temp976=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp892)->f3; _LL981: _temp980= _temp976.f1; goto _LL979; _LL979: _temp978=
_temp976.f2; goto _LL975; _LL975: _temp974=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_For_s_struct*) _temp892)->f4; goto _LL919;} else{ goto _LL920;} _LL920:
if(( unsigned int) _temp892 > 1u?(( struct _tunion_struct*) _temp892)->tag ==
Cyc_Absyn_Switch_s_tag: 0){ _LL993: _temp992=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Switch_s_struct*) _temp892)->f1; goto _LL991; _LL991: _temp990=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*) _temp892)->f2; goto
_LL921;} else{ goto _LL922;} _LL922: if(( unsigned int) _temp892 > 1u?(( struct
_tunion_struct*) _temp892)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL997: _temp996=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp892)->f1; goto
_LL995; _LL995: _temp994=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp892)->f2; goto _LL923;} else{ goto _LL924;} _LL924: if(( unsigned int)
_temp892 > 1u?(( struct _tunion_struct*) _temp892)->tag == Cyc_Absyn_Cut_s_tag:
0){ _LL999: _temp998=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp892)->f1; goto _LL925;} else{ goto _LL926;} _LL926: if(( unsigned int)
_temp892 > 1u?(( struct _tunion_struct*) _temp892)->tag == Cyc_Absyn_Splice_s_tag:
0){ _LL1001: _temp1000=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp892)->f1; goto _LL927;} else{ goto _LL928;} _LL928: if(( unsigned int)
_temp892 > 1u?(( struct _tunion_struct*) _temp892)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1005: _temp1004=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp892)->f1; goto _LL1003; _LL1003: _temp1002=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp892)->f2; goto _LL929;} else{ goto _LL930;}
_LL930: if(( unsigned int) _temp892 > 1u?(( struct _tunion_struct*) _temp892)->tag
== Cyc_Absyn_Do_s_tag: 0){ _LL1013: _temp1012=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Do_s_struct*) _temp892)->f1; goto _LL1007; _LL1007: _temp1006=( struct
_tuple2)(( struct Cyc_Absyn_Do_s_struct*) _temp892)->f2; _LL1011: _temp1010=
_temp1006.f1; goto _LL1009; _LL1009: _temp1008= _temp1006.f2; goto _LL931;}
else{ goto _LL932;} _LL932: if(( unsigned int) _temp892 > 1u?(( struct
_tunion_struct*) _temp892)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1017:
_temp1016=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp892)->f1; goto _LL1015; _LL1015: _temp1014=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp892)->f2; goto _LL933;} else{ goto
_LL893;} _LL895: Cyc_Absyndump_dump_semi(); goto _LL893; _LL897: Cyc_Absyndump_dumpexp(
_temp934); Cyc_Absyndump_dump_semi(); goto _LL893; _LL899: if( Cyc_Absynpp_is_declaration(
_temp938)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp938);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp938);}
if( Cyc_Absynpp_is_declaration( _temp936)){ Cyc_Absyndump_dump_char(( int)'{');
Cyc_Absyndump_dumpstmt( _temp936); Cyc_Absyndump_dump_char(( int)'}');} else{
Cyc_Absyndump_dumpstmt( _temp936);} goto _LL893; _LL901: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1018=( char*)"return;"; struct
_tagged_string _temp1019; _temp1019.curr= _temp1018; _temp1019.base= _temp1018;
_temp1019.last_plus_one= _temp1018 + 8; _temp1019;})); goto _LL893; _LL903: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1020=( char*)"return"; struct _tagged_string
_temp1021; _temp1021.curr= _temp1020; _temp1021.base= _temp1020; _temp1021.last_plus_one=
_temp1020 + 7; _temp1021;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1022= _temp942; if( _temp1022 == 0){ _throw(
Null_Exception);} _temp1022;})); Cyc_Absyndump_dump_semi(); goto _LL893; _LL905:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1023=( char*)"if(";
struct _tagged_string _temp1024; _temp1024.curr= _temp1023; _temp1024.base=
_temp1023; _temp1024.last_plus_one= _temp1023 + 4; _temp1024;})); Cyc_Absyndump_dumpexp(
_temp948); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp1025=(
char*)"){"; struct _tagged_string _temp1026; _temp1026.curr= _temp1025;
_temp1026.base= _temp1025; _temp1026.last_plus_one= _temp1025 + 3; _temp1026;}));
Cyc_Absyndump_dumpstmt( _temp946); Cyc_Absyndump_dump_char(( int)'}');{ void*
_temp1027=( void*) _temp944->r; _LL1029: if(( int) _temp1027 == Cyc_Absyn_Skip_s){
goto _LL1030;} else{ goto _LL1031;} _LL1031: goto _LL1032; _LL1030: goto _LL1028;
_LL1032: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1033=( char*)"else{";
struct _tagged_string _temp1034; _temp1034.curr= _temp1033; _temp1034.base=
_temp1033; _temp1034.last_plus_one= _temp1033 + 6; _temp1034;})); Cyc_Absyndump_dumpstmt(
_temp944); Cyc_Absyndump_dump_char(( int)'}'); goto _LL1028; _LL1028:;} goto
_LL893; _LL907: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1035=(
char*)"while("; struct _tagged_string _temp1036; _temp1036.curr= _temp1035;
_temp1036.base= _temp1035; _temp1036.last_plus_one= _temp1035 + 7; _temp1036;}));
Cyc_Absyndump_dumpexp( _temp956); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp1037=( char*)") {"; struct _tagged_string _temp1038;
_temp1038.curr= _temp1037; _temp1038.base= _temp1037; _temp1038.last_plus_one=
_temp1037 + 4; _temp1038;})); Cyc_Absyndump_dumpstmt( _temp950); Cyc_Absyndump_dump_char((
int)'}'); goto _LL893; _LL909: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1039=( char*)"break;"; struct _tagged_string _temp1040; _temp1040.curr=
_temp1039; _temp1040.base= _temp1039; _temp1040.last_plus_one= _temp1039 + 7;
_temp1040;})); goto _LL893; _LL911: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1041=( char*)"continue;"; struct _tagged_string _temp1042; _temp1042.curr=
_temp1041; _temp1042.base= _temp1041; _temp1042.last_plus_one= _temp1041 + 10;
_temp1042;})); goto _LL893; _LL913: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1043=( char*)"goto"; struct _tagged_string _temp1044; _temp1044.curr=
_temp1043; _temp1044.base= _temp1043; _temp1044.last_plus_one= _temp1043 + 5;
_temp1044;})); Cyc_Absyndump_dump_str( _temp964); Cyc_Absyndump_dump_semi();
goto _LL893; _LL915: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1045=( char*)"fallthru;"; struct _tagged_string _temp1046; _temp1046.curr=
_temp1045; _temp1046.base= _temp1045; _temp1046.last_plus_one= _temp1045 + 10;
_temp1046;})); goto _LL893; _LL917: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1047=( char*)"fallthru("; struct _tagged_string _temp1048; _temp1048.curr=
_temp1047; _temp1048.base= _temp1047; _temp1048.last_plus_one= _temp1047 + 10;
_temp1048;})); Cyc_Absyndump_dumpexps_prec( 20, _temp972); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1049=( char*)");"; struct _tagged_string
_temp1050; _temp1050.curr= _temp1049; _temp1050.base= _temp1049; _temp1050.last_plus_one=
_temp1049 + 3; _temp1050;})); goto _LL893; _LL919: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1051=( char*)"for("; struct _tagged_string
_temp1052; _temp1052.curr= _temp1051; _temp1052.base= _temp1051; _temp1052.last_plus_one=
_temp1051 + 5; _temp1052;})); Cyc_Absyndump_dumpexp( _temp988); Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp( _temp986); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp980); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp1053=(
char*)"){"; struct _tagged_string _temp1054; _temp1054.curr= _temp1053;
_temp1054.base= _temp1053; _temp1054.last_plus_one= _temp1053 + 3; _temp1054;}));
Cyc_Absyndump_dumpstmt( _temp974); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL893; _LL921: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1055=(
char*)"switch("; struct _tagged_string _temp1056; _temp1056.curr= _temp1055;
_temp1056.base= _temp1055; _temp1056.last_plus_one= _temp1055 + 8; _temp1056;}));
Cyc_Absyndump_dumpexp( _temp992); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp1057=( char*)"){"; struct _tagged_string _temp1058;
_temp1058.curr= _temp1057; _temp1058.base= _temp1057; _temp1058.last_plus_one=
_temp1057 + 3; _temp1058;})); Cyc_Absyndump_dumpswitchclauses( _temp990); Cyc_Absyndump_dump_char((
int)'}'); goto _LL893; _LL923: Cyc_Absyndump_dumpdecl( _temp996); Cyc_Absyndump_dumpstmt(
_temp994); goto _LL893; _LL925: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1059=( char*)"cut"; struct _tagged_string _temp1060; _temp1060.curr=
_temp1059; _temp1060.base= _temp1059; _temp1060.last_plus_one= _temp1059 + 4;
_temp1060;})); Cyc_Absyndump_dumpstmt( _temp998); goto _LL893; _LL927: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1061=( char*)"splice"; struct _tagged_string
_temp1062; _temp1062.curr= _temp1061; _temp1062.base= _temp1061; _temp1062.last_plus_one=
_temp1061 + 7; _temp1062;})); Cyc_Absyndump_dumpstmt( _temp1000); goto _LL893;
_LL929: if( Cyc_Absynpp_is_declaration( _temp1002)){ Cyc_Absyndump_dump_str(
_temp1004); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1063=( char*)": {"; struct _tagged_string _temp1064; _temp1064.curr=
_temp1063; _temp1064.base= _temp1063; _temp1064.last_plus_one= _temp1063 + 4;
_temp1064;})); Cyc_Absyndump_dumpstmt( _temp1002); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dump_str( _temp1004); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpstmt( _temp1002);} goto _LL893; _LL931: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1065=( char*)"do {"; struct _tagged_string
_temp1066; _temp1066.curr= _temp1065; _temp1066.base= _temp1065; _temp1066.last_plus_one=
_temp1065 + 5; _temp1066;})); Cyc_Absyndump_dumpstmt( _temp1012); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1067=( char*)"} while ("; struct
_tagged_string _temp1068; _temp1068.curr= _temp1067; _temp1068.base= _temp1067;
_temp1068.last_plus_one= _temp1067 + 10; _temp1068;})); Cyc_Absyndump_dumpexp(
_temp1010); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1069=( char*)");"; struct _tagged_string _temp1070; _temp1070.curr=
_temp1069; _temp1070.base= _temp1069; _temp1070.last_plus_one= _temp1069 + 3;
_temp1070;})); goto _LL893; _LL933: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1071=( char*)"try"; struct _tagged_string _temp1072; _temp1072.curr=
_temp1071; _temp1072.base= _temp1071; _temp1072.last_plus_one= _temp1071 + 4;
_temp1072;})); Cyc_Absyndump_dumpstmt( _temp1016); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1073=( char*)"catch {"; struct _tagged_string
_temp1074; _temp1074.curr= _temp1073; _temp1074.base= _temp1073; _temp1074.last_plus_one=
_temp1073 + 8; _temp1074;})); Cyc_Absyndump_dumpswitchclauses( _temp1014); Cyc_Absyndump_dump_char((
int)'}'); goto _LL893; _LL893:;} void Cyc_Absyndump_dumpdp( struct _tuple8* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,( struct _tagged_string)({ char*
_temp1075=( char*)""; struct _tagged_string _temp1076; _temp1076.curr= _temp1075;
_temp1076.base= _temp1075; _temp1076.last_plus_one= _temp1075 + 1; _temp1076;}),(
struct _tagged_string)({ char* _temp1077=( char*)"="; struct _tagged_string
_temp1078; _temp1078.curr= _temp1077; _temp1078.base= _temp1077; _temp1078.last_plus_one=
_temp1077 + 2; _temp1078;}),( struct _tagged_string)({ char* _temp1079=( char*)"=";
struct _tagged_string _temp1080; _temp1080.curr= _temp1079; _temp1080.base=
_temp1079; _temp1080.last_plus_one= _temp1079 + 2; _temp1080;})); Cyc_Absyndump_dumppat((*
dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp1081=(
void*) p->r; int _temp1117; void* _temp1119; int _temp1121; void* _temp1123;
char _temp1125; struct _tagged_string _temp1127; struct Cyc_Absyn_Vardecl*
_temp1129; struct Cyc_List_List* _temp1131; struct Cyc_Absyn_Pat* _temp1133;
struct Cyc_Absyn_Vardecl* _temp1135; struct _tuple0* _temp1137; struct Cyc_List_List*
_temp1139; struct Cyc_List_List* _temp1141; struct _tuple0* _temp1143; struct
Cyc_List_List* _temp1145; struct Cyc_List_List* _temp1147; struct _tuple0*
_temp1149; struct Cyc_List_List* _temp1151; struct Cyc_List_List* _temp1153;
struct Cyc_Core_Opt* _temp1155; struct Cyc_Absyn_Structdecl* _temp1157; struct
Cyc_Absyn_Tunionfield* _temp1159; struct Cyc_Absyn_Tuniondecl* _temp1161; struct
Cyc_List_List* _temp1163; struct Cyc_List_List* _temp1165; struct Cyc_Core_Opt*
_temp1167; struct _tuple0* _temp1169; struct Cyc_Absyn_Tunionfield* _temp1171;
struct Cyc_Absyn_XTuniondecl* _temp1173; struct Cyc_List_List* _temp1175; struct
Cyc_List_List* _temp1177; struct _tuple0* _temp1179; struct Cyc_Absyn_Enumfield*
_temp1181; struct Cyc_Absyn_Enumdecl* _temp1183; struct _tuple0* _temp1185;
_LL1083: if(( int) _temp1081 == Cyc_Absyn_Wild_p){ goto _LL1084;} else{ goto
_LL1085;} _LL1085: if(( int) _temp1081 == Cyc_Absyn_Null_p){ goto _LL1086;}
else{ goto _LL1087;} _LL1087: if(( unsigned int) _temp1081 > 2u?(( struct
_tunion_struct*) _temp1081)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1120: _temp1119=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1081)->f1; if(( int) _temp1119 ==
Cyc_Absyn_Signed){ goto _LL1118;} else{ goto _LL1089;} _LL1118: _temp1117=( int)((
struct Cyc_Absyn_Int_p_struct*) _temp1081)->f2; goto _LL1088;} else{ goto
_LL1089;} _LL1089: if(( unsigned int) _temp1081 > 2u?(( struct _tunion_struct*)
_temp1081)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1124: _temp1123=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1081)->f1; if(( int) _temp1123 == Cyc_Absyn_Unsigned){
goto _LL1122;} else{ goto _LL1091;} _LL1122: _temp1121=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1081)->f2; goto _LL1090;} else{ goto _LL1091;} _LL1091: if(( unsigned int)
_temp1081 > 2u?(( struct _tunion_struct*) _temp1081)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1126: _temp1125=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1081)->f1;
goto _LL1092;} else{ goto _LL1093;} _LL1093: if(( unsigned int) _temp1081 > 2u?((
struct _tunion_struct*) _temp1081)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1128:
_temp1127=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1081)->f1;
goto _LL1094;} else{ goto _LL1095;} _LL1095: if(( unsigned int) _temp1081 > 2u?((
struct _tunion_struct*) _temp1081)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1130:
_temp1129=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1081)->f1; goto _LL1096;} else{ goto _LL1097;} _LL1097: if(( unsigned int)
_temp1081 > 2u?(( struct _tunion_struct*) _temp1081)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1132: _temp1131=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1081)->f1; goto _LL1098;} else{ goto _LL1099;} _LL1099: if(( unsigned int)
_temp1081 > 2u?(( struct _tunion_struct*) _temp1081)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1134: _temp1133=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1081)->f1; goto _LL1100;} else{ goto _LL1101;} _LL1101: if(( unsigned int)
_temp1081 > 2u?(( struct _tunion_struct*) _temp1081)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1136: _temp1135=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1081)->f1; goto _LL1102;} else{ goto _LL1103;} _LL1103: if(( unsigned int)
_temp1081 > 2u?(( struct _tunion_struct*) _temp1081)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL1138: _temp1137=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1081)->f1; goto _LL1104;} else{ goto _LL1105;} _LL1105: if(( unsigned int)
_temp1081 > 2u?(( struct _tunion_struct*) _temp1081)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1144: _temp1143=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1081)->f1; goto _LL1142; _LL1142: _temp1141=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1081)->f2; goto _LL1140; _LL1140:
_temp1139=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1081)->f3; goto _LL1106;} else{ goto _LL1107;} _LL1107: if(( unsigned int)
_temp1081 > 2u?(( struct _tunion_struct*) _temp1081)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1150: _temp1149=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1081)->f1; goto _LL1148; _LL1148: _temp1147=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1081)->f2; goto _LL1146; _LL1146:
_temp1145=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1081)->f3; goto _LL1108;} else{ goto _LL1109;} _LL1109: if(( unsigned int)
_temp1081 > 2u?(( struct _tunion_struct*) _temp1081)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1158: _temp1157=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1081)->f1; goto _LL1156; _LL1156: _temp1155=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1081)->f2; goto _LL1154; _LL1154:
_temp1153=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1081)->f3; goto _LL1152; _LL1152: _temp1151=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1081)->f4; goto _LL1110;} else{ goto
_LL1111;} _LL1111: if(( unsigned int) _temp1081 > 2u?(( struct _tunion_struct*)
_temp1081)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1170: _temp1169=( struct
_tuple0*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1081)->f1; goto _LL1168;
_LL1168: _temp1167=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1081)->f2; goto _LL1166; _LL1166: _temp1165=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1081)->f3; goto _LL1164; _LL1164:
_temp1163=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1081)->f4; goto _LL1162; _LL1162: _temp1161=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1081)->f5; goto _LL1160; _LL1160:
_temp1159=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1081)->f6; goto _LL1112;} else{ goto _LL1113;} _LL1113: if(( unsigned int)
_temp1081 > 2u?(( struct _tunion_struct*) _temp1081)->tag == Cyc_Absyn_XTunion_p_tag:
0){ _LL1180: _temp1179=( struct _tuple0*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1081)->f1; goto _LL1178; _LL1178: _temp1177=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1081)->f2; goto _LL1176; _LL1176:
_temp1175=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1081)->f3; goto _LL1174; _LL1174: _temp1173=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1081)->f4; goto _LL1172; _LL1172:
_temp1171=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1081)->f5; goto _LL1114;} else{ goto _LL1115;} _LL1115: if(( unsigned int)
_temp1081 > 2u?(( struct _tunion_struct*) _temp1081)->tag == Cyc_Absyn_Enum_p_tag:
0){ _LL1186: _temp1185=( struct _tuple0*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1081)->f1; goto _LL1184; _LL1184: _temp1183=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1081)->f2; goto _LL1182; _LL1182:
_temp1181=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1081)->f3; goto _LL1116;} else{ goto _LL1082;} _LL1084: Cyc_Absyndump_dump_char((
int)'_'); goto _LL1082; _LL1086: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1187=( char*)"null"; struct _tagged_string _temp1188; _temp1188.curr=
_temp1187; _temp1188.base= _temp1187; _temp1188.last_plus_one= _temp1187 + 5;
_temp1188;})); goto _LL1082; _LL1088: Cyc_Absyndump_dump( xprintf("%d",
_temp1117)); goto _LL1082; _LL1090: Cyc_Absyndump_dump( xprintf("%u",(
unsigned int) _temp1121)); goto _LL1082; _LL1092: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1189=( char*)"'"; struct _tagged_string _temp1190;
_temp1190.curr= _temp1189; _temp1190.base= _temp1189; _temp1190.last_plus_one=
_temp1189 + 2; _temp1190;})); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape(
_temp1125)); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1191=( char*)"'"; struct _tagged_string _temp1192; _temp1192.curr=
_temp1191; _temp1192.base= _temp1191; _temp1192.last_plus_one= _temp1191 + 2;
_temp1192;})); goto _LL1082; _LL1094: Cyc_Absyndump_dump( _temp1127); goto
_LL1082; _LL1096: Cyc_Absyndump_dumpqvar( _temp1129->name); goto _LL1082;
_LL1098:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1131,( struct
_tagged_string)({ char* _temp1193=( char*)"$("; struct _tagged_string _temp1194;
_temp1194.curr= _temp1193; _temp1194.base= _temp1193; _temp1194.last_plus_one=
_temp1193 + 3; _temp1194;}),( struct _tagged_string)({ char* _temp1195=( char*)")";
struct _tagged_string _temp1196; _temp1196.curr= _temp1195; _temp1196.base=
_temp1195; _temp1196.last_plus_one= _temp1195 + 2; _temp1196;}),( struct
_tagged_string)({ char* _temp1197=( char*)","; struct _tagged_string _temp1198;
_temp1198.curr= _temp1197; _temp1198.base= _temp1197; _temp1198.last_plus_one=
_temp1197 + 2; _temp1198;})); goto _LL1082; _LL1100: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1199=( char*)"&"; struct _tagged_string _temp1200;
_temp1200.curr= _temp1199; _temp1200.base= _temp1199; _temp1200.last_plus_one=
_temp1199 + 2; _temp1200;})); Cyc_Absyndump_dumppat( _temp1133); goto _LL1082;
_LL1102: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1201=( char*)"*";
struct _tagged_string _temp1202; _temp1202.curr= _temp1201; _temp1202.base=
_temp1201; _temp1202.last_plus_one= _temp1201 + 2; _temp1202;})); Cyc_Absyndump_dumpqvar(
_temp1135->name); goto _LL1082; _LL1104: Cyc_Absyndump_dumpqvar( _temp1137);
goto _LL1082; _LL1106: Cyc_Absyndump_dumpqvar( _temp1143); Cyc_Absyndump_dumptvars(
_temp1141);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1139,( struct
_tagged_string)({ char* _temp1203=( char*)"("; struct _tagged_string _temp1204;
_temp1204.curr= _temp1203; _temp1204.base= _temp1203; _temp1204.last_plus_one=
_temp1203 + 2; _temp1204;}),( struct _tagged_string)({ char* _temp1205=( char*)")";
struct _tagged_string _temp1206; _temp1206.curr= _temp1205; _temp1206.base=
_temp1205; _temp1206.last_plus_one= _temp1205 + 2; _temp1206;}),( struct
_tagged_string)({ char* _temp1207=( char*)","; struct _tagged_string _temp1208;
_temp1208.curr= _temp1207; _temp1208.base= _temp1207; _temp1208.last_plus_one=
_temp1207 + 2; _temp1208;})); goto _LL1082; _LL1108: Cyc_Absyndump_dumpqvar(
_temp1149); Cyc_Absyndump_dumptvars( _temp1147);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1145,( struct _tagged_string)({ char* _temp1209=( char*)"{"; struct
_tagged_string _temp1210; _temp1210.curr= _temp1209; _temp1210.base= _temp1209;
_temp1210.last_plus_one= _temp1209 + 2; _temp1210;}),( struct _tagged_string)({
char* _temp1211=( char*)"}"; struct _tagged_string _temp1212; _temp1212.curr=
_temp1211; _temp1212.base= _temp1211; _temp1212.last_plus_one= _temp1211 + 2;
_temp1212;}),( struct _tagged_string)({ char* _temp1213=( char*)","; struct
_tagged_string _temp1214; _temp1214.curr= _temp1213; _temp1214.base= _temp1213;
_temp1214.last_plus_one= _temp1213 + 2; _temp1214;})); goto _LL1082; _LL1110:
if( _temp1157->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp1215= _temp1157->name; if( _temp1215 == 0){ _throw(
Null_Exception);} _temp1215->v;}));} Cyc_Absyndump_dumptvars( _temp1153);(( void(*)(
void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp, _temp1151,( struct _tagged_string)({ char* _temp1216=(
char*)"{"; struct _tagged_string _temp1217; _temp1217.curr= _temp1216; _temp1217.base=
_temp1216; _temp1217.last_plus_one= _temp1216 + 2; _temp1217;}),( struct
_tagged_string)({ char* _temp1218=( char*)"}"; struct _tagged_string _temp1219;
_temp1219.curr= _temp1218; _temp1219.base= _temp1218; _temp1219.last_plus_one=
_temp1218 + 2; _temp1219;}),( struct _tagged_string)({ char* _temp1220=( char*)",";
struct _tagged_string _temp1221; _temp1221.curr= _temp1220; _temp1221.base=
_temp1220; _temp1221.last_plus_one= _temp1220 + 2; _temp1221;})); goto _LL1082;
_LL1112: _temp1179= _temp1169; _temp1177= _temp1165; _temp1175= _temp1163; goto
_LL1114; _LL1114: Cyc_Absyndump_dumpqvar( _temp1179); Cyc_Absyndump_dumptvars(
_temp1177); if( _temp1175 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp1175,( struct _tagged_string)({ char* _temp1222=( char*)"("; struct
_tagged_string _temp1223; _temp1223.curr= _temp1222; _temp1223.base= _temp1222;
_temp1223.last_plus_one= _temp1222 + 2; _temp1223;}),( struct _tagged_string)({
char* _temp1224=( char*)")"; struct _tagged_string _temp1225; _temp1225.curr=
_temp1224; _temp1225.base= _temp1224; _temp1225.last_plus_one= _temp1224 + 2;
_temp1225;}),( struct _tagged_string)({ char* _temp1226=( char*)","; struct
_tagged_string _temp1227; _temp1227.curr= _temp1226; _temp1227.base= _temp1226;
_temp1227.last_plus_one= _temp1226 + 2; _temp1227;}));} goto _LL1082; _LL1116:
Cyc_Absyndump_dumpqvar( _temp1185); goto _LL1082; _LL1082:;} void Cyc_Absyndump_dumptunionfield(
struct Cyc_Absyn_Tunionfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs
!= 0){ Cyc_Absyndump_dumpargs( ef->typs);}} void Cyc_Absyndump_dumptunionfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumptunionfield, fields,( struct _tagged_string)({ char* _temp1228=(
char*)","; struct _tagged_string _temp1229; _temp1229.curr= _temp1228; _temp1229.base=
_temp1228; _temp1229.last_plus_one= _temp1228 + 2; _temp1229;}));} void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag
!= 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1230=( char*)" = ";
struct _tagged_string _temp1231; _temp1231.curr= _temp1230; _temp1231.base=
_temp1230; _temp1231.last_plus_one= _temp1230 + 4; _temp1231;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1232= ef->tag; if( _temp1232
== 0){ _throw( Null_Exception);} _temp1232;}));}} void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumpenumfield, fields,( struct _tagged_string)({ char* _temp1233=(
char*)","; struct _tagged_string _temp1234; _temp1234.curr= _temp1233; _temp1234.base=
_temp1233; _temp1234.last_plus_one= _temp1233 + 2; _temp1234;}));} void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp1235= fields; if( _temp1235 == 0){ _throw( Null_Exception);} _temp1235->tl;})){
struct Cyc_Absyn_Structfield _temp1239; struct Cyc_List_List* _temp1240; struct
Cyc_Core_Opt* _temp1242; void* _temp1244; struct Cyc_Absyn_Tqual* _temp1246;
struct _tagged_string* _temp1248; struct Cyc_Absyn_Structfield* _temp1237=(
struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp1236= fields; if(
_temp1236 == 0){ _throw( Null_Exception);} _temp1236->hd;}); _temp1239=*
_temp1237; _LL1249: _temp1248=( struct _tagged_string*) _temp1239.name; goto
_LL1247; _LL1247: _temp1246=( struct Cyc_Absyn_Tqual*) _temp1239.tq; goto
_LL1245; _LL1245: _temp1244=( void*) _temp1239.type; goto _LL1243; _LL1243:
_temp1242=( struct Cyc_Core_Opt*) _temp1239.width; goto _LL1241; _LL1241:
_temp1240=( struct Cyc_List_List*) _temp1239.attributes; goto _LL1238; _LL1238:((
void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct _tagged_string*),
struct _tagged_string*)) Cyc_Absyndump_dumptqtd)(( struct Cyc_Absyn_Tqual*)
_temp1246, _temp1244, Cyc_Absyndump_dump_str, _temp1248); Cyc_Absyndump_dumpatts(
_temp1240); if( _temp1242 != 0){ Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp1250= _temp1242; if(
_temp1250 == 0){ _throw( Null_Exception);} _temp1250->v;}));} Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname( struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar(
td->name); Cyc_Absyndump_dumptvars( td->tvs);} static void Cyc_Absyndump_dump_atts_qvar(
struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar(
fd->name);} void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl* d){ void*
_temp1251=( void*) d->r; struct Cyc_Absyn_Fndecl* _temp1277; struct Cyc_Absyn_Structdecl*
_temp1279; struct Cyc_Absyn_Uniondecl* _temp1281; struct Cyc_Absyn_Vardecl*
_temp1283; struct Cyc_Absyn_Vardecl _temp1285; struct Cyc_List_List* _temp1286;
struct Cyc_Core_Opt* _temp1288; int _temp1290; struct Cyc_Absyn_Exp* _temp1292;
void* _temp1294; struct Cyc_Absyn_Tqual* _temp1296; struct _tuple0* _temp1298;
void* _temp1300; struct Cyc_Absyn_Tuniondecl* _temp1302; struct Cyc_Absyn_XTuniondecl*
_temp1304; struct Cyc_Absyn_Enumdecl* _temp1306; struct Cyc_Absyn_Enumdecl
_temp1308; struct Cyc_List_List* _temp1309; struct _tuple0* _temp1311; void*
_temp1313; int _temp1315; struct Cyc_Absyn_Exp* _temp1317; struct Cyc_Core_Opt*
_temp1319; struct Cyc_Core_Opt* _temp1321; struct Cyc_Absyn_Pat* _temp1323;
struct Cyc_Absyn_Typedefdecl* _temp1325; struct Cyc_List_List* _temp1327; struct
_tagged_string* _temp1329; struct Cyc_List_List* _temp1331; struct _tuple0*
_temp1333; struct Cyc_List_List* _temp1335; _LL1253: if((( struct _tunion_struct*)
_temp1251)->tag == Cyc_Absyn_Fn_d_tag){ _LL1278: _temp1277=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1251)->f1; goto _LL1254;} else{ goto _LL1255;}
_LL1255: if((( struct _tunion_struct*) _temp1251)->tag == Cyc_Absyn_Struct_d_tag){
_LL1280: _temp1279=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp1251)->f1; goto _LL1256;} else{ goto _LL1257;} _LL1257: if((( struct
_tunion_struct*) _temp1251)->tag == Cyc_Absyn_Union_d_tag){ _LL1282: _temp1281=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp1251)->f1;
goto _LL1258;} else{ goto _LL1259;} _LL1259: if((( struct _tunion_struct*)
_temp1251)->tag == Cyc_Absyn_Var_d_tag){ _LL1284: _temp1283=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1251)->f1; _temp1285=* _temp1283; _LL1301:
_temp1300=( void*) _temp1285.sc; goto _LL1299; _LL1299: _temp1298=( struct
_tuple0*) _temp1285.name; goto _LL1297; _LL1297: _temp1296=( struct Cyc_Absyn_Tqual*)
_temp1285.tq; goto _LL1295; _LL1295: _temp1294=( void*) _temp1285.type; goto
_LL1293; _LL1293: _temp1292=( struct Cyc_Absyn_Exp*) _temp1285.initializer; goto
_LL1291; _LL1291: _temp1290=( int) _temp1285.shadow; goto _LL1289; _LL1289:
_temp1288=( struct Cyc_Core_Opt*) _temp1285.region; goto _LL1287; _LL1287:
_temp1286=( struct Cyc_List_List*) _temp1285.attributes; goto _LL1260;} else{
goto _LL1261;} _LL1261: if((( struct _tunion_struct*) _temp1251)->tag == Cyc_Absyn_Tunion_d_tag){
_LL1303: _temp1302=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp1251)->f1; goto _LL1262;} else{ goto _LL1263;} _LL1263: if((( struct
_tunion_struct*) _temp1251)->tag == Cyc_Absyn_XTunion_d_tag){ _LL1305: _temp1304=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*) _temp1251)->f1;
goto _LL1264;} else{ goto _LL1265;} _LL1265: if((( struct _tunion_struct*)
_temp1251)->tag == Cyc_Absyn_Enum_d_tag){ _LL1307: _temp1306=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp1251)->f1; _temp1308=* _temp1306; _LL1314:
_temp1313=( void*) _temp1308.sc; goto _LL1312; _LL1312: _temp1311=( struct
_tuple0*) _temp1308.name; goto _LL1310; _LL1310: _temp1309=( struct Cyc_List_List*)
_temp1308.fields; goto _LL1266;} else{ goto _LL1267;} _LL1267: if((( struct
_tunion_struct*) _temp1251)->tag == Cyc_Absyn_Let_d_tag){ _LL1324: _temp1323=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1251)->f1; goto
_LL1322; _LL1322: _temp1321=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1251)->f2; goto _LL1320; _LL1320: _temp1319=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1251)->f3; goto _LL1318; _LL1318: _temp1317=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1251)->f4; goto
_LL1316; _LL1316: _temp1315=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1251)->f5;
goto _LL1268;} else{ goto _LL1269;} _LL1269: if((( struct _tunion_struct*)
_temp1251)->tag == Cyc_Absyn_Typedef_d_tag){ _LL1326: _temp1325=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp1251)->f1; goto _LL1270;} else{ goto
_LL1271;} _LL1271: if((( struct _tunion_struct*) _temp1251)->tag == Cyc_Absyn_Namespace_d_tag){
_LL1330: _temp1329=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1251)->f1; goto _LL1328; _LL1328: _temp1327=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1251)->f2; goto _LL1272;} else{ goto
_LL1273;} _LL1273: if((( struct _tunion_struct*) _temp1251)->tag == Cyc_Absyn_Using_d_tag){
_LL1334: _temp1333=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1251)->f1; goto _LL1332; _LL1332: _temp1331=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1251)->f2; goto _LL1274;} else{ goto
_LL1275;} _LL1275: if((( struct _tunion_struct*) _temp1251)->tag == Cyc_Absyn_ExternC_d_tag){
_LL1336: _temp1335=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1251)->f1; goto _LL1276;} else{ goto _LL1252;} _LL1254: if( _temp1277->is_inline){
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1337=( char*)"inline";
struct _tagged_string _temp1338; _temp1338.curr= _temp1337; _temp1338.base=
_temp1337; _temp1338.last_plus_one= _temp1337 + 7; _temp1338;}));} Cyc_Absyndump_dumpscope((
void*) _temp1277->sc);{ void* t=( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1339=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1339[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1340; _temp1340.tag= Cyc_Absyn_FnType_tag;
_temp1340.f1=({ struct Cyc_Absyn_FnInfo _temp1341; _temp1341.tvars= _temp1277->tvs;
_temp1341.effect= _temp1277->effect; _temp1341.ret_typ=( void*)(( void*)
_temp1277->ret_type); _temp1341.args=(( struct Cyc_List_List*(*)( struct _tuple1*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt,
_temp1277->args); _temp1341.varargs= _temp1277->varargs; _temp1341.attributes= 0;
_temp1341;}); _temp1340;}); _temp1339;});(( void(*)( struct Cyc_Absyn_Tqual*,
void*, void(* f)( struct Cyc_Absyn_Fndecl*), struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(
0, t, Cyc_Absyndump_dump_atts_qvar, _temp1277); Cyc_Absyndump_dump_char(( int)'{');
Cyc_Absyndump_dumpstmt( _temp1277->body); Cyc_Absyndump_dump_char(( int)'}');
goto _LL1252;} _LL1256: Cyc_Absyndump_dumpscope(( void*) _temp1279->sc); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1342=( char*)"struct"; struct _tagged_string
_temp1343; _temp1343.curr= _temp1342; _temp1343.base= _temp1342; _temp1343.last_plus_one=
_temp1342 + 7; _temp1343;})); if( _temp1279->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp1344= _temp1279->name; if(
_temp1344 == 0){ _throw( Null_Exception);} _temp1344->v;}));} Cyc_Absyndump_dumptvars(
_temp1279->tvs); if( _temp1279->fields == 0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp1345= _temp1279->fields; if( _temp1345 == 0){ _throw(
Null_Exception);} _temp1345->v;})); Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1346=( char*)"}"; struct _tagged_string _temp1347; _temp1347.curr=
_temp1346; _temp1347.base= _temp1346; _temp1347.last_plus_one= _temp1346 + 2;
_temp1347;})); Cyc_Absyndump_dumpatts( _temp1279->attributes); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1348=( char*)";"; struct _tagged_string
_temp1349; _temp1349.curr= _temp1348; _temp1349.base= _temp1348; _temp1349.last_plus_one=
_temp1348 + 2; _temp1349;}));} goto _LL1252; _LL1258: Cyc_Absyndump_dumpscope((
void*) _temp1281->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1350=( char*)"union"; struct _tagged_string _temp1351; _temp1351.curr=
_temp1350; _temp1351.base= _temp1350; _temp1351.last_plus_one= _temp1350 + 6;
_temp1351;})); if( _temp1281->name != 0){ Cyc_Absyndump_dumpqvar(( struct
_tuple0*)({ struct Cyc_Core_Opt* _temp1352= _temp1281->name; if( _temp1352 == 0){
_throw( Null_Exception);} _temp1352->v;}));} Cyc_Absyndump_dumptvars( _temp1281->tvs);
if( _temp1281->fields == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1353= _temp1281->fields; if( _temp1353 == 0){ _throw( Null_Exception);}
_temp1353->v;})); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1354=(
char*)"}"; struct _tagged_string _temp1355; _temp1355.curr= _temp1354; _temp1355.base=
_temp1354; _temp1355.last_plus_one= _temp1354 + 2; _temp1355;})); Cyc_Absyndump_dumpatts(
_temp1281->attributes); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1356=( char*)";"; struct _tagged_string _temp1357; _temp1357.curr=
_temp1356; _temp1357.base= _temp1356; _temp1357.last_plus_one= _temp1356 + 2;
_temp1357;}));} goto _LL1252; _LL1260: Cyc_Absyndump_dumpscope( _temp1300);((
void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct _tuple0*), struct
_tuple0*)) Cyc_Absyndump_dumptqtd)(( struct Cyc_Absyn_Tqual*) _temp1296,
_temp1294, Cyc_Absyndump_dumpqvar, _temp1298); Cyc_Absyndump_dumpatts( _temp1286);
if( _temp1292 != 0){ Cyc_Absyndump_dump_char(( int)'='); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1358= _temp1292; if(
_temp1358 == 0){ _throw( Null_Exception);} _temp1358;}));} Cyc_Absyndump_dump_semi();
goto _LL1252; _LL1262: Cyc_Absyndump_dumpscope(( void*) _temp1302->sc); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1359=( char*)"tunion "; struct
_tagged_string _temp1360; _temp1360.curr= _temp1359; _temp1360.base= _temp1359;
_temp1360.last_plus_one= _temp1359 + 8; _temp1360;})); if( _temp1302->name != 0){
Cyc_Absyndump_dumpqvar(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp1361=
_temp1302->name; if( _temp1361 == 0){ _throw( Null_Exception);} _temp1361->v;}));}
Cyc_Absyndump_dumptvars( _temp1302->tvs); if( _temp1302->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumptunionfields((
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1362= _temp1302->fields; if(
_temp1362 == 0){ _throw( Null_Exception);} _temp1362->v;})); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1363=( char*)"};"; struct _tagged_string
_temp1364; _temp1364.curr= _temp1363; _temp1364.base= _temp1363; _temp1364.last_plus_one=
_temp1363 + 3; _temp1364;}));} goto _LL1252; _LL1264: Cyc_Absyndump_dumpscope((
void*) _temp1304->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1365=( char*)"xtunion "; struct _tagged_string _temp1366; _temp1366.curr=
_temp1365; _temp1366.base= _temp1365; _temp1366.last_plus_one= _temp1365 + 9;
_temp1366;})); Cyc_Absyndump_dumpqvar( _temp1304->name); if( _temp1304->fields
== 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumptunionfields(
_temp1304->fields); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1367=( char*)"};"; struct _tagged_string _temp1368; _temp1368.curr=
_temp1367; _temp1368.base= _temp1367; _temp1368.last_plus_one= _temp1367 + 3;
_temp1368;}));} goto _LL1252; _LL1266: Cyc_Absyndump_dumpscope( _temp1313); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1369=( char*)"enum "; struct _tagged_string
_temp1370; _temp1370.curr= _temp1369; _temp1370.base= _temp1369; _temp1370.last_plus_one=
_temp1369 + 6; _temp1370;})); Cyc_Absyndump_dumpqvar( _temp1311); Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpenumfields( _temp1309); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1371=( char*)"};"; struct _tagged_string
_temp1372; _temp1372.curr= _temp1371; _temp1372.base= _temp1371; _temp1372.last_plus_one=
_temp1371 + 3; _temp1372;})); return; _LL1268: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1373=( char*)"let"; struct _tagged_string _temp1374;
_temp1374.curr= _temp1373; _temp1374.base= _temp1373; _temp1374.last_plus_one=
_temp1373 + 4; _temp1374;})); Cyc_Absyndump_dumppat( _temp1323); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1317); Cyc_Absyndump_dump_semi(); goto
_LL1252; _LL1270: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1375=(
char*)"typedef"; struct _tagged_string _temp1376; _temp1376.curr= _temp1375;
_temp1376.base= _temp1375; _temp1376.last_plus_one= _temp1375 + 8; _temp1376;}));((
void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)( 0,( void*) _temp1325->defn,
Cyc_Absyndump_dumptypedefname, _temp1325); Cyc_Absyndump_dump_semi(); goto
_LL1252; _LL1272: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1377=(
char*)"namespace"; struct _tagged_string _temp1378; _temp1378.curr= _temp1377;
_temp1378.base= _temp1377; _temp1378.last_plus_one= _temp1377 + 10; _temp1378;}));
Cyc_Absyndump_dump_str( _temp1329); Cyc_Absyndump_dump_char(( int)'{'); for( 0;
_temp1327 != 0; _temp1327=({ struct Cyc_List_List* _temp1379= _temp1327; if(
_temp1379 == 0){ _throw( Null_Exception);} _temp1379->tl;})){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp1380= _temp1327; if(
_temp1380 == 0){ _throw( Null_Exception);} _temp1380->hd;}));} Cyc_Absyndump_dump_char((
int)'}'); goto _LL1252; _LL1274: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1381=( char*)"using"; struct _tagged_string _temp1382; _temp1382.curr=
_temp1381; _temp1382.base= _temp1381; _temp1382.last_plus_one= _temp1381 + 6;
_temp1382;})); Cyc_Absyndump_dumpqvar( _temp1333); Cyc_Absyndump_dump_char(( int)'{');
for( 0; _temp1331 != 0; _temp1331=({ struct Cyc_List_List* _temp1383= _temp1331;
if( _temp1383 == 0){ _throw( Null_Exception);} _temp1383->tl;})){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp1384= _temp1331; if(
_temp1384 == 0){ _throw( Null_Exception);} _temp1384->hd;}));} Cyc_Absyndump_dump_char((
int)'}'); goto _LL1252; _LL1276: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1385=( char*)"extern \"C\" {"; struct _tagged_string _temp1386;
_temp1386.curr= _temp1385; _temp1386.base= _temp1385; _temp1386.last_plus_one=
_temp1385 + 13; _temp1386;})); for( 0; _temp1335 != 0; _temp1335=({ struct Cyc_List_List*
_temp1387= _temp1335; if( _temp1387 == 0){ _throw( Null_Exception);} _temp1387->tl;})){
Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1388= _temp1335; if( _temp1388 == 0){ _throw( Null_Exception);} _temp1388->hd;}));}
Cyc_Absyndump_dump_char(( int)'}'); goto _LL1252; _LL1252:;} static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp* e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if(
i != 1){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char((
int)'}');}} void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)(
void*), void* a){ if( tms == 0){ f( a); return;}{ void* _temp1390=( void*)({
struct Cyc_List_List* _temp1389= tms; if( _temp1389 == 0){ _throw(
Null_Exception);} _temp1389->hd;}); struct Cyc_Absyn_Tqual* _temp1408; void*
_temp1410; void* _temp1412; struct Cyc_Absyn_Exp* _temp1414; struct Cyc_Absyn_Tqual*
_temp1416; void* _temp1418; void* _temp1420; struct Cyc_Absyn_Exp* _temp1422;
struct Cyc_Absyn_Tqual* _temp1424; void* _temp1426; void* _temp1428; struct Cyc_Absyn_Tqual*
_temp1430; void* _temp1432; struct Cyc_Absyn_Tvar* _temp1434; void* _temp1436;
struct Cyc_Absyn_Exp* _temp1438; struct Cyc_Absyn_Tqual* _temp1440; void*
_temp1442; struct Cyc_Absyn_Tvar* _temp1444; void* _temp1446; struct Cyc_Absyn_Exp*
_temp1448; struct Cyc_Absyn_Tqual* _temp1450; void* _temp1452; struct Cyc_Absyn_Tvar*
_temp1454; void* _temp1456; struct Cyc_Absyn_Tqual* _temp1458; void* _temp1460;
void* _temp1462; _LL1392: if(( unsigned int) _temp1390 > 1u?(( struct
_tunion_struct*) _temp1390)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1413:
_temp1412=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1390)->f1; if((
unsigned int) _temp1412 > 1u?(( struct _tunion_struct*) _temp1412)->tag == Cyc_Absyn_Nullable_ps_tag:
0){ _LL1415: _temp1414=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp1412)->f1; goto _LL1411;} else{ goto _LL1394;} _LL1411: _temp1410=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1390)->f2; if(( int) _temp1410 == Cyc_Absyn_HeapRgn){
goto _LL1409;} else{ goto _LL1394;} _LL1409: _temp1408=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1390)->f3; goto _LL1393;} else{ goto
_LL1394;} _LL1394: if(( unsigned int) _temp1390 > 1u?(( struct _tunion_struct*)
_temp1390)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1421: _temp1420=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1390)->f1; if(( unsigned int)
_temp1420 > 1u?(( struct _tunion_struct*) _temp1420)->tag == Cyc_Absyn_NonNullable_ps_tag:
0){ _LL1423: _temp1422=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp1420)->f1; goto _LL1419;} else{ goto _LL1396;} _LL1419: _temp1418=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1390)->f2; if(( int) _temp1418 == Cyc_Absyn_HeapRgn){
goto _LL1417;} else{ goto _LL1396;} _LL1417: _temp1416=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1390)->f3; goto _LL1395;} else{ goto
_LL1396;} _LL1396: if(( unsigned int) _temp1390 > 1u?(( struct _tunion_struct*)
_temp1390)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1429: _temp1428=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1390)->f1; if(( int) _temp1428 == Cyc_Absyn_TaggedArray_ps){
goto _LL1427;} else{ goto _LL1398;} _LL1427: _temp1426=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1390)->f2; if(( int) _temp1426 == Cyc_Absyn_HeapRgn){ goto _LL1425;} else{
goto _LL1398;} _LL1425: _temp1424=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1390)->f3; goto _LL1397;} else{ goto _LL1398;} _LL1398: if(( unsigned int)
_temp1390 > 1u?(( struct _tunion_struct*) _temp1390)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1437: _temp1436=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1390)->f1; if(( unsigned int) _temp1436 > 1u?(( struct _tunion_struct*)
_temp1436)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL1439: _temp1438=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1436)->f1; goto
_LL1433;} else{ goto _LL1400;} _LL1433: _temp1432=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1390)->f2; if(( unsigned int) _temp1432 > 4u?(( struct _tunion_struct*)
_temp1432)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1435: _temp1434=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1432)->f1; goto _LL1431;} else{ goto
_LL1400;} _LL1431: _temp1430=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1390)->f3; goto _LL1399;} else{ goto _LL1400;} _LL1400: if(( unsigned int)
_temp1390 > 1u?(( struct _tunion_struct*) _temp1390)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1447: _temp1446=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1390)->f1; if(( unsigned int) _temp1446 > 1u?(( struct _tunion_struct*)
_temp1446)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL1449: _temp1448=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1446)->f1; goto
_LL1443;} else{ goto _LL1402;} _LL1443: _temp1442=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1390)->f2; if(( unsigned int) _temp1442 > 4u?(( struct _tunion_struct*)
_temp1442)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1445: _temp1444=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1442)->f1; goto _LL1441;} else{ goto
_LL1402;} _LL1441: _temp1440=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1390)->f3; goto _LL1401;} else{ goto _LL1402;} _LL1402: if(( unsigned int)
_temp1390 > 1u?(( struct _tunion_struct*) _temp1390)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1457: _temp1456=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1390)->f1; if(( int) _temp1456 == Cyc_Absyn_TaggedArray_ps){ goto _LL1453;}
else{ goto _LL1404;} _LL1453: _temp1452=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1390)->f2; if(( unsigned int) _temp1452 > 4u?(( struct _tunion_struct*)
_temp1452)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1455: _temp1454=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1452)->f1; goto _LL1451;} else{ goto
_LL1404;} _LL1451: _temp1450=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1390)->f3; goto _LL1403;} else{ goto _LL1404;} _LL1404: if(( unsigned int)
_temp1390 > 1u?(( struct _tunion_struct*) _temp1390)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1463: _temp1462=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1390)->f1; goto _LL1461; _LL1461: _temp1460=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1390)->f2; goto _LL1459; _LL1459: _temp1458=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1390)->f3; goto _LL1405;} else{ goto
_LL1406;} _LL1406: goto _LL1407; _LL1393: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1414);(( void(*)( struct Cyc_List_List* tms,
void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List*
_temp1464= tms; if( _temp1464 == 0){ _throw( Null_Exception);} _temp1464->tl;}),
f, a); return; _LL1395: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1422);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1465= tms; if( _temp1465 ==
0){ _throw( Null_Exception);} _temp1465->tl;}), f, a); return; _LL1397: Cyc_Absyndump_dump_char((
int)'?');(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(({
struct Cyc_List_List* _temp1466= tms; if( _temp1466 == 0){ _throw(
Null_Exception);} _temp1466->tl;}), f, a); return; _LL1399: Cyc_Absyndump_dump_char((
int)'*'); Cyc_Absyndump_dump_upperbound( _temp1438); Cyc_Absyndump_dump_str(
_temp1434->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1467= tms; if(
_temp1467 == 0){ _throw( Null_Exception);} _temp1467->tl;}), f, a); return;
_LL1401: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1448); Cyc_Absyndump_dump_str( _temp1444->name);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List*
_temp1468= tms; if( _temp1468 == 0){ _throw( Null_Exception);} _temp1468->tl;}),
f, a); return; _LL1403: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1454->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1469= tms; if(
_temp1469 == 0){ _throw( Null_Exception);} _temp1469->tl;}), f, a); return;
_LL1405:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1470=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1470[ 0]=({ struct Cyc_Core_Impossible_struct _temp1471; _temp1471.tag= Cyc_Core_Impossible_tag;
_temp1471.f1=( struct _tagged_string)({ char* _temp1472=( char*)"dumptms: bad Pointer_mod";
struct _tagged_string _temp1473; _temp1473.curr= _temp1472; _temp1473.base=
_temp1472; _temp1473.last_plus_one= _temp1472 + 25; _temp1473;}); _temp1471;});
_temp1470;})); _LL1407: { int next_is_pointer= 0; if(({ struct Cyc_List_List*
_temp1474= tms; if( _temp1474 == 0){ _throw( Null_Exception);} _temp1474->tl;})
!= 0){ void* _temp1477=( void*)({ struct Cyc_List_List* _temp1476=({ struct Cyc_List_List*
_temp1475= tms; if( _temp1475 == 0){ _throw( Null_Exception);} _temp1475->tl;});
if( _temp1476 == 0){ _throw( Null_Exception);} _temp1476->hd;}); struct Cyc_Absyn_Tqual*
_temp1483; void* _temp1485; void* _temp1487; _LL1479: if(( unsigned int)
_temp1477 > 1u?(( struct _tunion_struct*) _temp1477)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1488: _temp1487=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1477)->f1; goto _LL1486; _LL1486: _temp1485=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1477)->f2; goto _LL1484; _LL1484: _temp1483=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1477)->f3; goto _LL1480;} else{ goto
_LL1481;} _LL1481: goto _LL1482; _LL1480: next_is_pointer= 1; goto _LL1478;
_LL1482: goto _LL1478; _LL1478:;} if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)'(');}(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1489= tms; if( _temp1489 ==
0){ _throw( Null_Exception);} _temp1489->tl;}), f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1491=( void*)({ struct Cyc_List_List* _temp1490= tms; if(
_temp1490 == 0){ _throw( Null_Exception);} _temp1490->hd;}); struct Cyc_Absyn_Exp*
_temp1507; void* _temp1509; struct Cyc_Core_Opt* _temp1511; int _temp1513;
struct Cyc_List_List* _temp1515; void* _temp1517; struct Cyc_Position_Segment*
_temp1519; struct Cyc_List_List* _temp1521; int _temp1523; struct Cyc_Position_Segment*
_temp1525; struct Cyc_List_List* _temp1527; struct Cyc_List_List* _temp1529;
struct Cyc_Position_Segment* _temp1531; struct Cyc_Absyn_Tqual* _temp1533; void*
_temp1535; void* _temp1537; _LL1493: if(( int) _temp1491 == Cyc_Absyn_Carray_mod){
goto _LL1494;} else{ goto _LL1495;} _LL1495: if(( unsigned int) _temp1491 > 1u?((
struct _tunion_struct*) _temp1491)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){
_LL1508: _temp1507=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp1491)->f1; goto _LL1496;} else{ goto _LL1497;} _LL1497: if(( unsigned int)
_temp1491 > 1u?(( struct _tunion_struct*) _temp1491)->tag == Cyc_Absyn_Function_mod_tag:
0){ _LL1510: _temp1509=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1491)->f1; if((( struct _tunion_struct*) _temp1509)->tag == Cyc_Absyn_WithTypes_tag){
_LL1516: _temp1515=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1509)->f1; goto _LL1514; _LL1514: _temp1513=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1509)->f2; goto _LL1512; _LL1512: _temp1511=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_WithTypes_struct*) _temp1509)->f3; goto _LL1498;} else{ goto
_LL1499;}} else{ goto _LL1499;} _LL1499: if(( unsigned int) _temp1491 > 1u?((
struct _tunion_struct*) _temp1491)->tag == Cyc_Absyn_Function_mod_tag: 0){
_LL1518: _temp1517=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1491)->f1;
if((( struct _tunion_struct*) _temp1517)->tag == Cyc_Absyn_NoTypes_tag){ _LL1522:
_temp1521=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp1517)->f1;
goto _LL1520; _LL1520: _temp1519=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp1517)->f2; goto _LL1500;} else{ goto _LL1501;}} else{ goto _LL1501;}
_LL1501: if(( unsigned int) _temp1491 > 1u?(( struct _tunion_struct*) _temp1491)->tag
== Cyc_Absyn_TypeParams_mod_tag: 0){ _LL1528: _temp1527=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp1491)->f1; goto _LL1526; _LL1526:
_temp1525=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1491)->f2; goto _LL1524; _LL1524: _temp1523=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1491)->f3; goto _LL1502;} else{ goto _LL1503;} _LL1503: if(( unsigned int)
_temp1491 > 1u?(( struct _tunion_struct*) _temp1491)->tag == Cyc_Absyn_Attributes_mod_tag:
0){ _LL1532: _temp1531=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1491)->f1; goto _LL1530; _LL1530: _temp1529=( struct Cyc_List_List*)((
struct Cyc_Absyn_Attributes_mod_struct*) _temp1491)->f2; goto _LL1504;} else{
goto _LL1505;} _LL1505: if(( unsigned int) _temp1491 > 1u?(( struct
_tunion_struct*) _temp1491)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1538:
_temp1537=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1491)->f1; goto
_LL1536; _LL1536: _temp1535=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1491)->f2; goto _LL1534; _LL1534: _temp1533=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1491)->f3; goto _LL1506;} else{ goto
_LL1492;} _LL1494: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1539=(
char*)"[]"; struct _tagged_string _temp1540; _temp1540.curr= _temp1539;
_temp1540.base= _temp1539; _temp1540.last_plus_one= _temp1539 + 3; _temp1540;}));
goto _LL1492; _LL1496: Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp(
_temp1507); Cyc_Absyndump_dump_char(( int)']'); goto _LL1492; _LL1498: Cyc_Absyndump_dumpfunargs(
_temp1515, _temp1513, _temp1511); goto _LL1492; _LL1500:(( void(*)( void(* f)(
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1521,( struct _tagged_string)({ char* _temp1541=( char*)"("; struct
_tagged_string _temp1542; _temp1542.curr= _temp1541; _temp1542.base= _temp1541;
_temp1542.last_plus_one= _temp1541 + 2; _temp1542;}),( struct _tagged_string)({
char* _temp1543=( char*)")"; struct _tagged_string _temp1544; _temp1544.curr=
_temp1543; _temp1544.base= _temp1543; _temp1544.last_plus_one= _temp1543 + 2;
_temp1544;}),( struct _tagged_string)({ char* _temp1545=( char*)","; struct
_tagged_string _temp1546; _temp1546.curr= _temp1545; _temp1546.base= _temp1545;
_temp1546.last_plus_one= _temp1545 + 2; _temp1546;})); goto _LL1492; _LL1502:
if( _temp1523){ Cyc_Absyndump_dumpkindedtvars( _temp1527);} else{ Cyc_Absyndump_dumptvars(
_temp1527);} goto _LL1492; _LL1504: Cyc_Absyndump_dumpatts( _temp1529); goto
_LL1492; _LL1506:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1547=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1547[ 0]=({ struct Cyc_Core_Impossible_struct _temp1548; _temp1548.tag= Cyc_Core_Impossible_tag;
_temp1548.f1=( struct _tagged_string)({ char* _temp1549=( char*)"dumptms";
struct _tagged_string _temp1550; _temp1550.curr= _temp1549; _temp1550.base=
_temp1549; _temp1550.last_plus_one= _temp1549 + 8; _temp1550;}); _temp1548;});
_temp1547;})); _LL1492:;} return;} _LL1391:;}} static struct Cyc_Absyn_Tqual Cyc_Absyndump_empty_tq_v=(
struct Cyc_Absyn_Tqual){.q_const= 0,.q_volatile= 0,.q_restrict= 0}; static
struct Cyc_Absyn_Tqual* Cyc_Absyndump_empty_tq=& Cyc_Absyndump_empty_tq_v; void
Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual* tq, void* t, void(* f)( void*),
void* a){ if( tq == 0){ tq=( struct Cyc_Absyn_Tqual*) Cyc_Absyndump_empty_tq;}{
struct Cyc_List_List* _temp1554; void* _temp1556; struct Cyc_Absyn_Tqual*
_temp1558; struct _tuple4 _temp1552= Cyc_Absynpp_to_tms(( struct Cyc_Absyn_Tqual*)({
struct Cyc_Absyn_Tqual* _temp1551= tq; if( _temp1551 == 0){ _throw(
Null_Exception);} _temp1551;}), t); _LL1559: _temp1558= _temp1552.f1; goto
_LL1557; _LL1557: _temp1556= _temp1552.f2; goto _LL1555; _LL1555: _temp1554=
_temp1552.f3; goto _LL1553; _LL1553: Cyc_Absyndump_dumptq( _temp1558); Cyc_Absyndump_dumpntyp(
_temp1556);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp1554), f, a);}} void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;*
Cyc_Absyndump_dump_file= f; for( 0; tdl != 0; tdl=({ struct Cyc_List_List*
_temp1560= tdl; if( _temp1560 == 0){ _throw( Null_Exception);} _temp1560->tl;})){
Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1561= tdl; if( _temp1561 == 0){ _throw( Null_Exception);} _temp1561->hd;}));}}