#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ void*
f1; void* f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
struct _tuple5{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple6{ void*
f1; int f2; } ; struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2;
} ; struct _tuple8{ struct Cyc_Absyn_XTuniondecl* f1; } ; typedef int Cyc_ptrdiff_t;
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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag; } ;
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_List_mem( int(* compare)( void*, void*), struct
Cyc_List_List* l, void* x); extern struct Cyc_List_List* Cyc_List_filter_c( int(*
f)( void*, void*), void* env, struct Cyc_List_List* l); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); struct Cyc_Lineno_Pos{ struct
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
char* tag; } ; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*);
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
extern struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); struct
Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set*
Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set*
Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent_tag[ 7u]; struct
Cyc_Set_Absent_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[
8u]; struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void*
data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key);
typedef void* Cyc_Tcenv_Resolved; static const int Cyc_Tcenv_VarRes_tag= 0;
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
extern struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(); extern struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment*, struct Cyc_Absyn_Fndecl*);
extern struct Cyc_List_List* Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tagged_string*, struct Cyc_List_List*);
extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*); extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag; } ; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern
void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_string);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_unify(
void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_Position_Segment*);
extern void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual* tq1, struct Cyc_Absyn_Tqual* tq2); extern int Cyc_Tcutil_same_atts(
struct Cyc_List_List*, struct Cyc_List_List*); extern int Cyc_Tcutil_bits_only(
void* t); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e); extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*,
void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
add_cyc_namespace, struct Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); static void Cyc_Tc_redecl_err(
struct Cyc_Position_Segment* loc, struct _tagged_string t, struct _tagged_string*
v, struct _tagged_string new_sc, struct _tagged_string old_sc){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp0= t; struct _tagged_string _temp1=* v; struct
_tagged_string _temp2= new_sc; struct _tagged_string _temp3= old_sc; xprintf("redeclaration of %.*s %.*s cannot be %.*s when earlier definition is %.*s",
_temp0.last_plus_one - _temp0.curr, _temp0.curr, _temp1.last_plus_one - _temp1.curr,
_temp1.curr, _temp2.last_plus_one - _temp2.curr, _temp2.curr, _temp3.last_plus_one
- _temp3.curr, _temp3.curr);}));} static void* Cyc_Tc_scope_redecl_okay( void*
s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt* fields2, struct
Cyc_Position_Segment* loc, struct _tagged_string t, struct _tagged_string* v){{
struct _tuple3 _temp5=({ struct _tuple3 _temp4; _temp4.f1= s1; _temp4.f2= s2;
_temp4;}); void* _temp35; void* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; void* _temp47; void* _temp49; void* _temp51; void*
_temp53; void* _temp55; void* _temp57; void* _temp59; void* _temp61; void*
_temp63; void* _temp65; void* _temp68; void* _temp70; void* _temp72; void*
_temp74; void* _temp76; void* _temp78; void* _temp80; void* _temp82; void*
_temp85; void* _temp87; void* _temp89; void* _temp91; _LL7: _LL38: _temp37=
_temp5.f1; if(( int) _temp37 == Cyc_Absyn_ExternC){ goto _LL36;} else{ goto _LL9;}
_LL36: _temp35= _temp5.f2; if(( int) _temp35 == Cyc_Absyn_ExternC){ goto _LL8;}
else{ goto _LL9;} _LL9: _LL42: _temp41= _temp5.f1; goto _LL40; _LL40: _temp39=
_temp5.f2; if(( int) _temp39 == Cyc_Absyn_ExternC){ goto _LL10;} else{ goto
_LL11;} _LL11: _LL46: _temp45= _temp5.f1; if(( int) _temp45 == Cyc_Absyn_Static){
goto _LL44;} else{ goto _LL13;} _LL44: _temp43= _temp5.f2; if(( int) _temp43 ==
Cyc_Absyn_Static){ goto _LL12;} else{ goto _LL13;} _LL13: _LL50: _temp49= _temp5.f1;
goto _LL48; _LL48: _temp47= _temp5.f2; if(( int) _temp47 == Cyc_Absyn_Static){
goto _LL14;} else{ goto _LL15;} _LL15: _LL54: _temp53= _temp5.f1; if(( int)
_temp53 == Cyc_Absyn_Static){ goto _LL52;} else{ goto _LL17;} _LL52: _temp51=
_temp5.f2; if(( int) _temp51 == Cyc_Absyn_Public){ goto _LL16;} else{ goto _LL17;}
_LL17: _LL58: _temp57= _temp5.f1; if(( int) _temp57 == Cyc_Absyn_Abstract){ goto
_LL56;} else{ goto _LL19;} _LL56: _temp55= _temp5.f2; if(( int) _temp55 == Cyc_Absyn_Public){
goto _LL18;} else{ goto _LL19;} _LL19: _LL62: _temp61= _temp5.f1; goto _LL60;
_LL60: _temp59= _temp5.f2; if(( int) _temp59 == Cyc_Absyn_Public){ goto _LL20;}
else{ goto _LL21;} _LL21: _LL66: _temp65= _temp5.f1; if(( int) _temp65 == Cyc_Absyn_Static){
goto _LL64;} else{ goto _LL23;} _LL64: _temp63= _temp5.f2; if(( int) _temp63 ==
Cyc_Absyn_Extern){ goto _LL22;} else{ goto _LL23;} _LL23: _LL71: _temp70= _temp5.f1;
if(( int) _temp70 == Cyc_Absyn_Abstract){ goto _LL69;} else{ goto _LL25;} _LL69:
_temp68= _temp5.f2; if(( int) _temp68 == Cyc_Absyn_Extern){ goto _LL67;} else{
goto _LL25;} _LL67: if( fields2 != 0){ goto _LL24;} else{ goto _LL25;} _LL25:
_LL75: _temp74= _temp5.f1; goto _LL73; _LL73: _temp72= _temp5.f2; if(( int)
_temp72 == Cyc_Absyn_Extern){ goto _LL26;} else{ goto _LL27;} _LL27: _LL79:
_temp78= _temp5.f1; if(( int) _temp78 == Cyc_Absyn_Static){ goto _LL77;} else{
goto _LL29;} _LL77: _temp76= _temp5.f2; if(( int) _temp76 == Cyc_Absyn_Abstract){
goto _LL28;} else{ goto _LL29;} _LL29: _LL83: _temp82= _temp5.f1; if(( int)
_temp82 == Cyc_Absyn_Public){ goto _LL81;} else{ goto _LL31;} _LL81: _temp80=
_temp5.f2; if(( int) _temp80 == Cyc_Absyn_Abstract){ goto _LL30;} else{ goto
_LL31;} _LL31: _LL88: _temp87= _temp5.f1; if(( int) _temp87 == Cyc_Absyn_Extern){
goto _LL86;} else{ goto _LL33;} _LL86: _temp85= _temp5.f2; if(( int) _temp85 ==
Cyc_Absyn_Abstract){ goto _LL84;} else{ goto _LL33;} _LL84: if( fields1 != 0){
goto _LL32;} else{ goto _LL33;} _LL33: _LL92: _temp91= _temp5.f1; goto _LL90;
_LL90: _temp89= _temp5.f2; if(( int) _temp89 == Cyc_Absyn_Abstract){ goto _LL34;}
else{ goto _LL6;} _LL8: goto _LL6; _LL10: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp93=( char*)"non-extern \"C\""; struct
_tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93; _temp94.last_plus_one=
_temp93 + 15; _temp94;}),( struct _tagged_string)({ char* _temp95=( char*)"extern \"C\"";
struct _tagged_string _temp96; _temp96.curr= _temp95; _temp96.base= _temp95;
_temp96.last_plus_one= _temp95 + 11; _temp96;})); goto _LL6; _LL12: goto _LL6;
_LL14: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp97=(
char*)"non-static"; struct _tagged_string _temp98; _temp98.curr= _temp97;
_temp98.base= _temp97; _temp98.last_plus_one= _temp97 + 11; _temp98;}),( struct
_tagged_string)({ char* _temp99=( char*)"static"; struct _tagged_string _temp100;
_temp100.curr= _temp99; _temp100.base= _temp99; _temp100.last_plus_one= _temp99
+ 7; _temp100;})); goto _LL6; _LL16: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp101=( char*)"static"; struct _tagged_string
_temp102; _temp102.curr= _temp101; _temp102.base= _temp101; _temp102.last_plus_one=
_temp101 + 7; _temp102;}),( struct _tagged_string)({ char* _temp103=( char*)"public";
struct _tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 7; _temp104;})); goto _LL6; _LL18: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp105=( char*)"abstract"; struct
_tagged_string _temp106; _temp106.curr= _temp105; _temp106.base= _temp105;
_temp106.last_plus_one= _temp105 + 9; _temp106;}),( struct _tagged_string)({
char* _temp107=( char*)"public"; struct _tagged_string _temp108; _temp108.curr=
_temp107; _temp108.base= _temp107; _temp108.last_plus_one= _temp107 + 7;
_temp108;})); goto _LL6; _LL20: s1=( void*) Cyc_Absyn_Public; goto _LL6; _LL22:
Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp109=( char*)"static";
struct _tagged_string _temp110; _temp110.curr= _temp109; _temp110.base= _temp109;
_temp110.last_plus_one= _temp109 + 7; _temp110;}),( struct _tagged_string)({
char* _temp111=( char*)"extern"; struct _tagged_string _temp112; _temp112.curr=
_temp111; _temp112.base= _temp111; _temp112.last_plus_one= _temp111 + 7;
_temp112;})); goto _LL6; _LL24: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp113=( char*)"abstract"; struct _tagged_string
_temp114; _temp114.curr= _temp113; _temp114.base= _temp113; _temp114.last_plus_one=
_temp113 + 9; _temp114;}),( struct _tagged_string)({ char* _temp115=( char*)"transparent";
struct _tagged_string _temp116; _temp116.curr= _temp115; _temp116.base= _temp115;
_temp116.last_plus_one= _temp115 + 12; _temp116;})); goto _LL6; _LL26: goto _LL6;
_LL28: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp117=(
char*)"static"; struct _tagged_string _temp118; _temp118.curr= _temp117;
_temp118.base= _temp117; _temp118.last_plus_one= _temp117 + 7; _temp118;}),(
struct _tagged_string)({ char* _temp119=( char*)"abstract"; struct
_tagged_string _temp120; _temp120.curr= _temp119; _temp120.base= _temp119;
_temp120.last_plus_one= _temp119 + 9; _temp120;})); goto _LL6; _LL30: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp121=( char*)"public"; struct
_tagged_string _temp122; _temp122.curr= _temp121; _temp122.base= _temp121;
_temp122.last_plus_one= _temp121 + 7; _temp122;}),( struct _tagged_string)({
char* _temp123=( char*)"abstract"; struct _tagged_string _temp124; _temp124.curr=
_temp123; _temp124.base= _temp123; _temp124.last_plus_one= _temp123 + 9;
_temp124;})); goto _LL6; _LL32: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp125=( char*)"[extern] transparent"; struct
_tagged_string _temp126; _temp126.curr= _temp125; _temp126.base= _temp125;
_temp126.last_plus_one= _temp125 + 21; _temp126;}),( struct _tagged_string)({
char* _temp127=( char*)"abstract"; struct _tagged_string _temp128; _temp128.curr=
_temp127; _temp128.base= _temp127; _temp128.last_plus_one= _temp127 + 9;
_temp128;})); goto _LL6; _LL34: s1=( void*) Cyc_Absyn_Abstract; goto _LL6; _LL6:;}
return s1;} static void Cyc_Tc_field_redecl_okay( struct Cyc_Absyn_Tunionfield*
f1, struct Cyc_Absyn_Tunionfield* f2, struct Cyc_List_List* inst, struct Cyc_Tcenv_Tenv*
te, struct _tagged_string t, struct _tagged_string* v){ struct Cyc_Position_Segment*
loc= f1->loc; if( Cyc_String_zstrptrcmp((* f1->name).f2,(* f2->name).f2) != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp129= t; struct _tagged_string
_temp130=* v; struct _tagged_string _temp131=*(* f1->name).f2; struct
_tagged_string _temp132=*(* f2->name).f2; xprintf("redeclaration of %.*s %.*s: field name mismatch %.*s != %.*s",
_temp129.last_plus_one - _temp129.curr, _temp129.curr, _temp130.last_plus_one -
_temp130.curr, _temp130.curr, _temp131.last_plus_one - _temp131.curr, _temp131.curr,
_temp132.last_plus_one - _temp132.curr, _temp132.curr);}));}{ struct Cyc_List_List*
tvs1= f1->tvs; struct Cyc_List_List* tvs2= f2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs1) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp133= t; struct
_tagged_string _temp134=* v; struct _tagged_string _temp135=*(* f1->name).f2;
xprintf("redeclaration of %.*s %.*s, field %.*s: type parameter number mismatch",
_temp133.last_plus_one - _temp133.curr, _temp133.curr, _temp134.last_plus_one -
_temp134.curr, _temp134.curr, _temp135.last_plus_one - _temp135.curr, _temp135.curr);}));}
for( 0; tvs1 != 0; tvs1=({ struct Cyc_List_List* _temp136= tvs1; if( _temp136 ==
0){ _throw( Null_Exception);} _temp136->tl;}), tvs2=({ struct Cyc_List_List*
_temp137= tvs2; if( _temp137 == 0){ _throw( Null_Exception);} _temp137->tl;})){
inst=({ struct Cyc_List_List* _temp138=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp138->hd=( void*)({ struct _tuple4* _temp139=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp139->f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp143= tvs2; if( _temp143 == 0){ _throw( Null_Exception);}
_temp143->hd;}); _temp139->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp140=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp140[ 0]=({ struct Cyc_Absyn_VarType_struct _temp141; _temp141.tag= Cyc_Absyn_VarType_tag;
_temp141.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp142= tvs1;
if( _temp142 == 0){ _throw( Null_Exception);} _temp142->hd;}); _temp141;});
_temp140;}); _temp139;}); _temp138->tl= inst; _temp138;});}{ struct Cyc_List_List*
typs1= f1->typs; struct Cyc_List_List* typs2= f2->typs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( typs1) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
typs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp144= t; struct
_tagged_string _temp145=* v; struct _tagged_string _temp146=*(* f1->name).f2;
xprintf("redeclaration of %.*s %.*s, field %.*s: parameter number mismatch",
_temp144.last_plus_one - _temp144.curr, _temp144.curr, _temp145.last_plus_one -
_temp145.curr, _temp145.curr, _temp146.last_plus_one - _temp146.curr, _temp146.curr);}));}
for( 0; typs1 != 0; typs1=({ struct Cyc_List_List* _temp147= typs1; if( _temp147
== 0){ _throw( Null_Exception);} _temp147->tl;}), typs2=({ struct Cyc_List_List*
_temp148= typs2; if( _temp148 == 0){ _throw( Null_Exception);} _temp148->tl;})){
if( ! Cyc_Tcutil_equal_tqual((*(( struct _tuple5*)({ struct Cyc_List_List*
_temp149= typs1; if( _temp149 == 0){ _throw( Null_Exception);} _temp149->hd;}))).f1,(*((
struct _tuple5*)({ struct Cyc_List_List* _temp150= typs2; if( _temp150 == 0){
_throw( Null_Exception);} _temp150->hd;}))).f1)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp151= t; struct _tagged_string _temp152=* v; struct
_tagged_string _temp153=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: parameter qualifier",
_temp151.last_plus_one - _temp151.curr, _temp151.curr, _temp152.last_plus_one -
_temp152.curr, _temp152.curr, _temp153.last_plus_one - _temp153.curr, _temp153.curr);}));}{
void* subst_t2= Cyc_Tcutil_substitute( inst,(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp154= typs2; if( _temp154 == 0){ _throw( Null_Exception);} _temp154->hd;}))).f2);
if( ! Cyc_Tcutil_unify((*(( struct _tuple5*)({ struct Cyc_List_List* _temp155=
typs1; if( _temp155 == 0){ _throw( Null_Exception);} _temp155->hd;}))).f2,
subst_t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp157= t; struct
_tagged_string _temp158=* v; struct _tagged_string _temp159=*(* f1->name).f2;
struct _tagged_string _temp160= Cyc_Absynpp_typ2string((*(( struct _tuple5*)({
struct Cyc_List_List* _temp156= typs1; if( _temp156 == 0){ _throw(
Null_Exception);} _temp156->hd;}))).f2); struct _tagged_string _temp161= Cyc_Absynpp_typ2string(
subst_t2); xprintf("redeclaration of %.*s %.*s, field %.*s: parameter type mismatch %.*s != %.*s",
_temp157.last_plus_one - _temp157.curr, _temp157.curr, _temp158.last_plus_one -
_temp158.curr, _temp158.curr, _temp159.last_plus_one - _temp159.curr, _temp159.curr,
_temp160.last_plus_one - _temp160.curr, _temp160.curr, _temp161.last_plus_one -
_temp161.curr, _temp161.curr);}));}}}}}} static int Cyc_Tc_var_redecl_okay(
struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_string* name, void* t, void* sc, struct Cyc_List_List* atts){ struct Cyc_Core_Opt*
ans=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)( ge->ordinaries, name); if( ans == 0){ return 0;}(*((
struct _tuple6*)({ struct Cyc_Core_Opt* _temp162= ans; if( _temp162 == 0){
_throw( Null_Exception);} _temp162->v;}))).f2= 1;{ void* _temp164=(*(( struct
_tuple6*)({ struct Cyc_Core_Opt* _temp163= ans; if( _temp163 == 0){ _throw(
Null_Exception);} _temp163->v;}))).f1; void* _temp170; struct Cyc_Absyn_Vardecl*
_temp172; _LL166: if((( struct _tunion_struct*) _temp164)->tag == Cyc_Tcenv_VarRes_tag){
_LL171: _temp170=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp164)->f1; if((
unsigned int) _temp170 > 1u?(( struct _tunion_struct*) _temp170)->tag == Cyc_Absyn_Global_b_tag:
0){ _LL173: _temp172=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp170)->f1; goto _LL167;} else{ goto _LL168;}} else{ goto _LL168;} _LL168:
goto _LL169; _LL167: if( sc ==( void*) Cyc_Absyn_Static?( void*) _temp172->sc !=(
void*) Cyc_Absyn_Static: 0){ Cyc_Tcutil_warn( loc,({ struct _tagged_string
_temp174=* name; xprintf("static declaration of %.*s follows non-static declaration",
_temp174.last_plus_one - _temp174.curr, _temp174.curr);}));} if(( void*)
_temp172->sc ==( void*) Cyc_Absyn_Static? sc !=( void*) Cyc_Absyn_Static: 0){
Cyc_Tcutil_warn( loc,({ struct _tagged_string _temp175=* name; xprintf("non-static declaration of %.*s follows static declaration",
_temp175.last_plus_one - _temp175.curr, _temp175.curr);}));} if((( void*)
_temp172->sc ==( void*) Cyc_Absyn_ExternC? sc !=( void*) Cyc_Absyn_ExternC: 0)?
1:(( void*) _temp172->sc !=( void*) Cyc_Absyn_ExternC? sc ==( void*) Cyc_Absyn_ExternC:
0)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp176=* name; xprintf("incompatible redeclaration of %.*s",
_temp176.last_plus_one - _temp176.curr, _temp176.curr);}));} else{ if((( sc !=(
void*) Cyc_Absyn_Extern?( void*) _temp172->sc !=( void*) Cyc_Absyn_Extern: 0)? !(
sc ==( void*) Cyc_Absyn_Static?( void*) _temp172->sc ==( void*) Cyc_Absyn_Static:
0): 0)? !( sc ==( void*) Cyc_Absyn_Public?( void*) _temp172->sc ==( void*) Cyc_Absyn_Public:
0): 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp177=* name; xprintf("incompatible redeclaration of %.*s",
_temp177.last_plus_one - _temp177.curr, _temp177.curr);}));}} if( ! Cyc_Tcutil_unify((
void*) _temp172->type, t)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp178=* name; struct _tagged_string _temp179= Cyc_Absynpp_typ2string( t);
struct _tagged_string _temp180= Cyc_Absynpp_typ2string(( void*) _temp172->type);
xprintf("redeclaration of %.*s at a different type: %.*s != %.*s", _temp178.last_plus_one
- _temp178.curr, _temp178.curr, _temp179.last_plus_one - _temp179.curr, _temp179.curr,
_temp180.last_plus_one - _temp180.curr, _temp180.curr);}));} if( ! Cyc_Tcutil_same_atts(
atts, _temp172->attributes)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp181=* name; xprintf("redeclaration of %.*s with different attributes",
_temp181.last_plus_one - _temp181.curr, _temp181.curr);}));} goto _LL165; _LL169:
goto _LL165; _LL165:;} return 1;} static int Cyc_Tc_is_function_type( void* t){
void* _temp182= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp188;
_LL184: if(( unsigned int) _temp182 > 4u?(( struct _tunion_struct*) _temp182)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL189: _temp188=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp182)->f1; goto _LL185;} else{ goto _LL186;}
_LL186: goto _LL187; _LL185: return 1; _LL187: return 0; _LL183:;} static struct
Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List* atts){
void* _temp190= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp196;
struct Cyc_List_List* _temp198; struct Cyc_List_List** _temp200; int _temp201;
struct Cyc_List_List* _temp203; void* _temp205; struct Cyc_Core_Opt* _temp207;
struct Cyc_List_List* _temp209; _LL192: if(( unsigned int) _temp190 > 4u?((
struct _tunion_struct*) _temp190)->tag == Cyc_Absyn_FnType_tag: 0){ _LL197:
_temp196=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp190)->f1;
_LL210: _temp209=( struct Cyc_List_List*) _temp196.tvars; goto _LL208; _LL208:
_temp207=( struct Cyc_Core_Opt*) _temp196.effect; goto _LL206; _LL206: _temp205=(
void*) _temp196.ret_typ; goto _LL204; _LL204: _temp203=( struct Cyc_List_List*)
_temp196.args; goto _LL202; _LL202: _temp201=( int) _temp196.varargs; goto
_LL199; _LL199: _temp198=( struct Cyc_List_List*) _temp196.attributes; _temp200=&(((
struct Cyc_Absyn_FnType_struct*) _temp190)->f1).attributes; goto _LL193;} else{
goto _LL194;} _LL194: goto _LL195; _LL193: { struct Cyc_List_List* res_atts= 0;
for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp211= atts; if( _temp211 ==
0){ _throw( Null_Exception);} _temp211->tl;})){ void* _temp213=( void*)({ struct
Cyc_List_List* _temp212= atts; if( _temp212 == 0){ _throw( Null_Exception);}
_temp212->hd;}); int _temp227; _LL215: if(( unsigned int) _temp213 > 15u?((
struct _tunion_struct*) _temp213)->tag == Cyc_Absyn_Regparm_att_tag: 0){ _LL228:
_temp227=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp213)->f1; goto
_LL216;} else{ goto _LL217;} _LL217: if(( int) _temp213 == Cyc_Absyn_Stdcall_att){
goto _LL218;} else{ goto _LL219;} _LL219: if(( int) _temp213 == Cyc_Absyn_Cdecl_att){
goto _LL220;} else{ goto _LL221;} _LL221: if(( int) _temp213 == Cyc_Absyn_Noreturn_att){
goto _LL222;} else{ goto _LL223;} _LL223: if(( int) _temp213 == Cyc_Absyn_Const_att){
goto _LL224;} else{ goto _LL225;} _LL225: goto _LL226; _LL216: goto _LL218;
_LL218: goto _LL220; _LL220: goto _LL222; _LL222: goto _LL224; _LL224:* _temp200=({
struct Cyc_List_List* _temp229=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp229->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp230= atts; if( _temp230 == 0){ _throw( Null_Exception);} _temp230->hd;}));
_temp229->tl=* _temp200; _temp229;}); continue; _LL226: res_atts=({ struct Cyc_List_List*
_temp231=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp231->hd=( void*)(( void*)({ struct Cyc_List_List* _temp232= atts; if(
_temp232 == 0){ _throw( Null_Exception);} _temp232->hd;})); _temp231->tl=
res_atts; _temp231;}); continue; _LL214:;} return res_atts;} _LL195: return((
struct Cyc_List_List*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp233=( char*)"transfer_fn_type_atts"; struct
_tagged_string _temp234; _temp234.curr= _temp233; _temp234.base= _temp233;
_temp234.last_plus_one= _temp233 + 22; _temp234;})); _LL191:;} static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Vardecl* vd){ struct _tagged_string* v=(* vd->name).f2;
void* t=( void*) vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List* atts=
vd->attributes;{ void* _temp235=(* vd->name).f1; struct Cyc_List_List* _temp243;
struct Cyc_List_List* _temp245; _LL237: if(( unsigned int) _temp235 > 1u?((
struct _tunion_struct*) _temp235)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL244:
_temp243=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp235)->f1;
if( _temp243 == 0){ goto _LL238;} else{ goto _LL239;}} else{ goto _LL239;}
_LL239: if(( unsigned int) _temp235 > 1u?(( struct _tunion_struct*) _temp235)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL246: _temp245=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp235)->f1; if( _temp245 == 0){ goto _LL240;} else{
goto _LL241;}} else{ goto _LL241;} _LL241: goto _LL242; _LL238: goto _LL236;
_LL240: goto _LL236; _LL242: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp247=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp248; _temp248.curr= _temp247; _temp248.base= _temp247;
_temp248.last_plus_one= _temp247 + 43; _temp248;})); return; _LL236:;}(* vd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp249=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp249[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp250; _temp250.tag= Cyc_Absyn_Abs_n_tag; _temp250.f1= te->ns; _temp250;});
_temp249;});{ void* _temp251= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp*
_temp258; struct Cyc_Absyn_Tqual* _temp260; void* _temp262; _LL253: if((
unsigned int) _temp251 > 4u?(( struct _tunion_struct*) _temp251)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL263: _temp262=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp251)->f1;
goto _LL261; _LL261: _temp260=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp251)->f2; goto _LL259; _LL259: _temp258=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp251)->f3; if( _temp258 == 0){ goto _LL257;}
else{ goto _LL255;}} else{ goto _LL255;} _LL257: if( vd->initializer != 0){ goto
_LL254;} else{ goto _LL255;} _LL255: goto _LL256; _LL254:{ void* _temp265=( void*)({
struct Cyc_Absyn_Exp* _temp264= vd->initializer; if( _temp264 == 0){ _throw(
Null_Exception);} _temp264->r;}); void* _temp275; struct _tagged_string _temp277;
struct Cyc_List_List* _temp279; struct Cyc_Core_Opt* _temp281; struct Cyc_List_List*
_temp283; _LL267: if((( struct _tunion_struct*) _temp265)->tag == Cyc_Absyn_Const_e_tag){
_LL276: _temp275=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp265)->f1; if((
unsigned int) _temp275 > 1u?(( struct _tunion_struct*) _temp275)->tag == Cyc_Absyn_String_c_tag:
0){ _LL278: _temp277=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp275)->f1; goto _LL268;} else{ goto _LL269;}} else{ goto _LL269;} _LL269:
if((( struct _tunion_struct*) _temp265)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL282: _temp281=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp265)->f1; goto _LL280; _LL280: _temp279=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp265)->f2; goto _LL270;} else{ goto
_LL271;} _LL271: if((( struct _tunion_struct*) _temp265)->tag == Cyc_Absyn_Array_e_tag){
_LL284: _temp283=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp265)->f1; goto _LL272;} else{ goto _LL273;} _LL273: goto _LL274; _LL268: t=(
void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp285=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp285[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp286; _temp286.tag= Cyc_Absyn_ArrayType_tag;
_temp286.f1=( void*) _temp262; _temp286.f2= _temp260; _temp286.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(({ struct _tagged_string _temp287= _temp277;( unsigned int)(
_temp287.last_plus_one - _temp287.curr);}), 0); _temp286;}); _temp285;}))); goto
_LL266; _LL270: _temp283= _temp279; goto _LL272; _LL272: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp288=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp288[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp289; _temp289.tag= Cyc_Absyn_ArrayType_tag;
_temp289.f1=( void*) _temp262; _temp289.f2= _temp260; _temp289.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp283), 0); _temp289;}); _temp288;}))); goto _LL266; _LL274: goto _LL266;
_LL266:;} goto _LL252; _LL256: goto _LL252; _LL252:;} Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t); if( Cyc_Tc_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);}{ int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, atts); if(
sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){ if( vd->initializer
!= 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp290=( char*)"extern declaration should not have initializer";
struct _tagged_string _temp291; _temp291.curr= _temp290; _temp291.base= _temp290;
_temp291.last_plus_one= _temp290 + 47; _temp291;}));}} else{ if( ! Cyc_Tc_is_function_type(
t)){ for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp292= atts; if(
_temp292 == 0){ _throw( Null_Exception);} _temp292->tl;})){ void* _temp294=(
void*)({ struct Cyc_List_List* _temp293= atts; if( _temp293 == 0){ _throw(
Null_Exception);} _temp293->hd;}); int _temp314; struct _tagged_string _temp316;
_LL296: if(( unsigned int) _temp294 > 15u?(( struct _tunion_struct*) _temp294)->tag
== Cyc_Absyn_Aligned_att_tag: 0){ _LL315: _temp314=( int)(( struct Cyc_Absyn_Aligned_att_struct*)
_temp294)->f1; goto _LL297;} else{ goto _LL298;} _LL298: if(( unsigned int)
_temp294 > 15u?(( struct _tunion_struct*) _temp294)->tag == Cyc_Absyn_Section_att_tag:
0){ _LL317: _temp316=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp294)->f1; goto _LL299;} else{ goto _LL300;} _LL300: if(( int) _temp294 ==
Cyc_Absyn_Nocommon_att){ goto _LL301;} else{ goto _LL302;} _LL302: if(( int)
_temp294 == Cyc_Absyn_Shared_att){ goto _LL303;} else{ goto _LL304;} _LL304: if((
int) _temp294 == Cyc_Absyn_Unused_att){ goto _LL305;} else{ goto _LL306;} _LL306:
if(( int) _temp294 == Cyc_Absyn_Weak_att){ goto _LL307;} else{ goto _LL308;}
_LL308: if(( int) _temp294 == Cyc_Absyn_Dllimport_att){ goto _LL309;} else{ goto
_LL310;} _LL310: if(( int) _temp294 == Cyc_Absyn_Dllexport_att){ goto _LL311;}
else{ goto _LL312;} _LL312: goto _LL313; _LL297: continue; _LL299: continue;
_LL301: continue; _LL303: continue; _LL305: continue; _LL307: continue; _LL309:
continue; _LL311: continue; _LL313: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp319= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp318= atts; if( _temp318 == 0){ _throw( Null_Exception);} _temp318->hd;}));
struct _tagged_string _temp320= Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_temp319.last_plus_one - _temp319.curr, _temp319.curr, _temp320.last_plus_one -
_temp320.curr, _temp320.curr);})); goto _LL295; _LL295:;}{ struct Cyc_Absyn_Exp*
e; if( vd->initializer == 0){ e= Cyc_Tcutil_default_initializer( te, t, loc); vd->initializer=(
struct Cyc_Absyn_Exp*) e;} else{ e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp321= vd->initializer; if( _temp321 == 0){ _throw( Null_Exception);}
_temp321;});}{ void* t2= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( !
Cyc_Tcutil_coerce_assign( te, e, t)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp322= Cyc_Absynpp_qvar2string( vd->name); struct
_tagged_string _temp323= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp324= Cyc_Absynpp_typ2string( t2); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp322.last_plus_one - _temp322.curr, _temp322.curr, _temp323.last_plus_one -
_temp323.curr, _temp323.curr, _temp324.last_plus_one - _temp324.curr, _temp324.curr);}));}
if( ! Cyc_Tcutil_is_const_exp( te, e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp325=( char*)"initializer is not a constant expression";
struct _tagged_string _temp326; _temp326.curr= _temp325; _temp326.base= _temp325;
_temp326.last_plus_one= _temp325 + 41; _temp326;}));}}}} else{ for( 0; atts != 0;
atts=({ struct Cyc_List_List* _temp327= atts; if( _temp327 == 0){ _throw(
Null_Exception);} _temp327->tl;})){ void* _temp329=( void*)({ struct Cyc_List_List*
_temp328= atts; if( _temp328 == 0){ _throw( Null_Exception);} _temp328->hd;});
int _temp347; int _temp349; _LL331: if(( unsigned int) _temp329 > 15u?(( struct
_tunion_struct*) _temp329)->tag == Cyc_Absyn_Regparm_att_tag: 0){ _LL348:
_temp347=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp329)->f1; goto
_LL332;} else{ goto _LL333;} _LL333: if(( int) _temp329 == Cyc_Absyn_Stdcall_att){
goto _LL334;} else{ goto _LL335;} _LL335: if(( int) _temp329 == Cyc_Absyn_Cdecl_att){
goto _LL336;} else{ goto _LL337;} _LL337: if(( int) _temp329 == Cyc_Absyn_Noreturn_att){
goto _LL338;} else{ goto _LL339;} _LL339: if(( int) _temp329 == Cyc_Absyn_Const_att){
goto _LL340;} else{ goto _LL341;} _LL341: if(( unsigned int) _temp329 > 15u?((
struct _tunion_struct*) _temp329)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL350:
_temp349=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp329)->f1; goto
_LL342;} else{ goto _LL343;} _LL343: if(( int) _temp329 == Cyc_Absyn_Packed_att){
goto _LL344;} else{ goto _LL345;} _LL345: goto _LL346; _LL332: goto _LL334;
_LL334: goto _LL336; _LL336: goto _LL338; _LL338: goto _LL340; _LL340:(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp351=( char*)"tcVardecl: fn type attributes in function var decl"; struct
_tagged_string _temp352; _temp352.curr= _temp351; _temp352.base= _temp351;
_temp352.last_plus_one= _temp351 + 51; _temp352;})); goto _LL330; _LL342: goto
_LL344; _LL344: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp354= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp353= atts; if( _temp353 == 0){ _throw(
Null_Exception);} _temp353->hd;})); xprintf("bad attribute %.*s in function declaration",
_temp354.last_plus_one - _temp354.curr, _temp354.curr);})); goto _LL330; _LL346:
continue; _LL330:;}}} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp355=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp355->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp356=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp356[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp357; _temp357.tag= Cyc_Tcenv_VarRes_tag;
_temp357.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp358=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp358[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp359; _temp359.tag= Cyc_Absyn_Global_b_tag;
_temp359.f1= vd; _temp359;}); _temp358;})); _temp357;}); _temp356;}); _temp355->f2=
is_redecl; _temp355;}));}} static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl*
fd){ struct _tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void*
_temp360=(* fd->name).f1; struct Cyc_List_List* _temp368; struct Cyc_List_List*
_temp370; _LL362: if(( unsigned int) _temp360 > 1u?(( struct _tunion_struct*)
_temp360)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL369: _temp368=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp360)->f1; if( _temp368 == 0){ goto _LL363;}
else{ goto _LL364;}} else{ goto _LL364;} _LL364: if(( unsigned int) _temp360 > 1u?((
struct _tunion_struct*) _temp360)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL371:
_temp370=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp360)->f1;
goto _LL365;} else{ goto _LL366;} _LL366: goto _LL367; _LL363: goto _LL361;
_LL365:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp372=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp372[ 0]=({ struct Cyc_Core_Impossible_struct _temp373; _temp373.tag= Cyc_Core_Impossible_tag;
_temp373.f1=( struct _tagged_string)({ char* _temp374=( char*)"tc: Abs_n in tcFndecl";
struct _tagged_string _temp375; _temp375.curr= _temp374; _temp375.base= _temp374;
_temp375.last_plus_one= _temp374 + 22; _temp375;}); _temp373;}); _temp372;}));
_LL367: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp376=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp377; _temp377.curr= _temp376; _temp377.base= _temp376;
_temp377.last_plus_one= _temp376 + 43; _temp377;})); return; _LL361:;}(* fd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp378=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp378[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp379; _temp379.tag= Cyc_Absyn_Abs_n_tag; _temp379.f1= te->ns; _temp379;});
_temp378;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd); int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, fd->attributes);{
struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=({ struct
Cyc_List_List* _temp380= atts; if( _temp380 == 0){ _throw( Null_Exception);}
_temp380->tl;})){ void* _temp382=( void*)({ struct Cyc_List_List* _temp381= atts;
if( _temp381 == 0){ _throw( Null_Exception);} _temp381->hd;}); int _temp390;
_LL384: if(( int) _temp382 == Cyc_Absyn_Packed_att){ goto _LL385;} else{ goto
_LL386;} _LL386: if(( unsigned int) _temp382 > 15u?(( struct _tunion_struct*)
_temp382)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL391: _temp390=( int)((
struct Cyc_Absyn_Aligned_att_struct*) _temp382)->f1; goto _LL387;} else{ goto
_LL388;} _LL388: goto _LL389; _LL385: goto _LL387; _LL387: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp393= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp392= atts; if( _temp392 == 0){ _throw( Null_Exception);} _temp392->hd;}));
xprintf("bad attribute %.*s for function", _temp393.last_plus_one - _temp393.curr,
_temp393.curr);})); goto _LL383; _LL389: goto _LL383; _LL383:;}} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple6*
_temp394=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp394->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp395=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp395[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp396; _temp396.tag= Cyc_Tcenv_VarRes_tag; _temp396.f1=( void*)(( void*)({
struct Cyc_Absyn_Funname_b_struct* _temp397=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp397[ 0]=({ struct
Cyc_Absyn_Funname_b_struct _temp398; _temp398.tag= Cyc_Absyn_Funname_b_tag;
_temp398.f1= fd; _temp398;}); _temp397;})); _temp396;}); _temp395;}); _temp394->f2=
is_redecl; _temp394;})); te->le=({ struct Cyc_Core_Opt* _temp399=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp399->v=( void*)(( void*)({ struct
Cyc_Tcenv_Outermost_struct* _temp400=( struct Cyc_Tcenv_Outermost_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct)); _temp400[ 0]=({ struct
Cyc_Tcenv_Outermost_struct _temp401; _temp401.tag= Cyc_Tcenv_Outermost_tag;
_temp401.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp401;}); _temp400;}));
_temp399;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp402=( char*)"function has goto statements to undefined labels";
struct _tagged_string _temp403; _temp403.curr= _temp402; _temp403.base= _temp402;
_temp403.last_plus_one= _temp402 + 49; _temp403;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp404=(* td->name).f1; struct Cyc_List_List*
_temp412; struct Cyc_List_List* _temp414; _LL406: if(( unsigned int) _temp404 >
1u?(( struct _tunion_struct*) _temp404)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL413:
_temp412=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp404)->f1;
if( _temp412 == 0){ goto _LL407;} else{ goto _LL408;}} else{ goto _LL408;}
_LL408: if(( unsigned int) _temp404 > 1u?(( struct _tunion_struct*) _temp404)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL415: _temp414=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp404)->f1; if( _temp414 == 0){ goto _LL409;} else{
goto _LL410;}} else{ goto _LL410;} _LL410: goto _LL411; _LL407: goto _LL405;
_LL409: goto _LL405; _LL411: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp416=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp417; _temp417.curr= _temp416; _temp417.base= _temp416;
_temp417.last_plus_one= _temp416 + 43; _temp417;})); return; _LL405:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp418=* v; xprintf("redeclaration of typedef %.*s",
_temp418.last_plus_one - _temp418.curr, _temp418.curr);})); return;}(* td->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp419=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp419[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp420; _temp420.tag= Cyc_Absyn_Abs_n_tag; _temp420.f1= te->ns; _temp420;});
_temp419;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ if( sd->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp421=( char*)"anonymous structs are not allowed at top level";
struct _tagged_string _temp422; _temp422.curr= _temp421; _temp422.base= _temp421;
_temp422.last_plus_one= _temp421 + 47; _temp422;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp423= sd->name;
if( _temp423 == 0){ _throw( Null_Exception);} _temp423->v;}))).f2;{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp424=
atts; if( _temp424 == 0){ _throw( Null_Exception);} _temp424->tl;})){ void*
_temp426=( void*)({ struct Cyc_List_List* _temp425= atts; if( _temp425 == 0){
_throw( Null_Exception);} _temp425->hd;}); int _temp434; _LL428: if(( int)
_temp426 == Cyc_Absyn_Packed_att){ goto _LL429;} else{ goto _LL430;} _LL430: if((
unsigned int) _temp426 > 15u?(( struct _tunion_struct*) _temp426)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL435: _temp434=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp426)->f1;
goto _LL431;} else{ goto _LL432;} _LL432: goto _LL433; _LL429: continue; _LL431:
continue; _LL433: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp437= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp436= atts; if( _temp436 == 0){ _throw(
Null_Exception);} _temp436->hd;})); struct _tagged_string _temp438=* v; xprintf("bad attribute %.*s in struct %.*s definition",
_temp437.last_plus_one - _temp437.curr, _temp437.curr, _temp438.last_plus_one -
_temp438.curr, _temp438.curr);})); goto _LL427; _LL427:;}}{ struct Cyc_List_List*
tvs2= sd->tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp439= tvs2;
if( _temp439 == 0){ _throw( Null_Exception);} _temp439->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp440= tvs2; if( _temp440 ==
0){ _throw( Null_Exception);} _temp440->hd;}))->kind); void* _temp441=( void*) c->v;
void* _temp449; _LL443: if(( int) _temp441 == Cyc_Absyn_No_constr){ goto _LL444;}
else{ goto _LL445;} _LL445: if(( unsigned int) _temp441 > 1u?(( struct
_tunion_struct*) _temp441)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL450: _temp449=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp441)->f1; if(( int) _temp449
== Cyc_Absyn_MemKind){ goto _LL446;} else{ goto _LL447;}} else{ goto _LL447;}
_LL447: goto _LL448; _LL444:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp451=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp451[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp452; _temp452.tag= Cyc_Absyn_Eq_constr_tag;
_temp452.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp452;}); _temp451;})));
continue; _LL446: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp454=* v;
struct _tagged_string _temp455=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp453= tvs2; if( _temp453 == 0){ _throw( Null_Exception);} _temp453->hd;}))->name;
xprintf("struct %.*s attempts to abstract type variable %.*s of kind M",
_temp454.last_plus_one - _temp454.curr, _temp454.curr, _temp455.last_plus_one -
_temp455.curr, _temp455.curr);})); continue; _LL448: continue; _LL442:;}}{
struct Cyc_List_List* tvs= sd->tvs;{ void* _temp457=(*(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp456= sd->name; if( _temp456 == 0){ _throw(
Null_Exception);} _temp456->v;}))).f1; struct Cyc_List_List* _temp465; struct
Cyc_List_List* _temp467; _LL459: if(( unsigned int) _temp457 > 1u?(( struct
_tunion_struct*) _temp457)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL466: _temp465=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp457)->f1; if(
_temp465 == 0){ goto _LL460;} else{ goto _LL461;}} else{ goto _LL461;} _LL461:
if(( unsigned int) _temp457 > 1u?(( struct _tunion_struct*) _temp457)->tag ==
Cyc_Absyn_Abs_n_tag: 0){ _LL468: _temp467=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp457)->f1; if( _temp467 == 0){ goto _LL462;} else{ goto _LL463;}} else{ goto
_LL463;} _LL463: goto _LL464; _LL460: goto _LL458; _LL462: goto _LL458; _LL464:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp469=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp470; _temp470.curr= _temp469; _temp470.base= _temp469;
_temp470.last_plus_one= _temp469 + 43; _temp470;})); return; _LL458:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp471= sd->name; if( _temp471 == 0){
_throw( Null_Exception);} _temp471->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp472=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp472[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp473; _temp473.tag= Cyc_Absyn_Abs_n_tag;
_temp473.f1= te->ns; _temp473;}); _temp472;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp474=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->structdecls, v);
struct Cyc_Core_Opt _temp480; struct Cyc_Absyn_Structdecl** _temp481; _LL476:
if( _temp474 == 0){ goto _LL477;} else{ goto _LL478;} _LL478: if( _temp474 == 0){
goto _LL475;} else{ _temp480=* _temp474; _LL482: _temp481=( struct Cyc_Absyn_Structdecl**)
_temp480.v; goto _LL479;} _LL477: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, v,({ struct Cyc_Absyn_Structdecl**
_temp483=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp483[ 0]= sd; _temp483;})); if( sd->fields == 0){ return;} goto _LL475;
_LL479: { struct Cyc_Absyn_Structdecl* sd2=* _temp481; struct Cyc_List_List*
tvs2= sd2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp484=* v; xprintf("redeclaration of struct %.*s has a different number of type parameters",
_temp484.last_plus_one - _temp484.curr, _temp484.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp485= x1; if( _temp485 == 0){ _throw( Null_Exception);} _temp485->tl;}), x2=({
struct Cyc_List_List* _temp486= x2; if( _temp486 == 0){ _throw( Null_Exception);}
_temp486->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp487= x1; if( _temp487 == 0){ _throw( Null_Exception);}
_temp487->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp488= x2; if( _temp488 == 0){ _throw( Null_Exception);}
_temp488->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp489=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp489[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp490; _temp490.tag= Cyc_Absyn_Forward_constr_tag; _temp490.f1= c2; _temp490;});
_temp489;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp492=* v; struct _tagged_string _temp493= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp494=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp491= x1; if( _temp491 == 0){ _throw( Null_Exception);} _temp491->hd;}))->name;
struct _tagged_string _temp495= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of struct %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp492.last_plus_one - _temp492.curr, _temp492.curr, _temp493.last_plus_one -
_temp493.curr, _temp493.curr, _temp494.last_plus_one - _temp494.curr, _temp494.curr,
_temp495.last_plus_one - _temp495.curr, _temp495.curr);}));}}( void*)( sd->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) sd->sc, sd->fields,( void*)
sd2->sc, sd2->fields, loc,( struct _tagged_string)({ char* _temp496=( char*)"struct";
struct _tagged_string _temp497; _temp497.curr= _temp496; _temp497.base= _temp496;
_temp497.last_plus_one= _temp496 + 7; _temp497;}), v));{ struct _tuple7 _temp499=({
struct _tuple7 _temp498; _temp498.f1= sd->fields; _temp498.f2= sd2->fields;
_temp498;}); struct Cyc_Core_Opt* _temp509; struct Cyc_Core_Opt* _temp511;
struct Cyc_Core_Opt* _temp513; struct Cyc_Core_Opt* _temp515; struct Cyc_Core_Opt*
_temp517; struct Cyc_Core_Opt* _temp519; struct Cyc_Core_Opt* _temp521; struct
Cyc_Core_Opt* _temp523; _LL501: _LL512: _temp511= _temp499.f1; if( _temp511 == 0){
goto _LL510;} else{ goto _LL503;} _LL510: _temp509= _temp499.f2; if( _temp509 ==
0){ goto _LL502;} else{ goto _LL503;} _LL503: _LL516: _temp515= _temp499.f1;
goto _LL514; _LL514: _temp513= _temp499.f2; if( _temp513 == 0){ goto _LL504;}
else{ goto _LL505;} _LL505: _LL520: _temp519= _temp499.f1; if( _temp519 == 0){
goto _LL518;} else{ goto _LL507;} _LL518: _temp517= _temp499.f2; goto _LL506;
_LL507: _LL524: _temp523= _temp499.f1; goto _LL522; _LL522: _temp521= _temp499.f2;
goto _LL508; _LL502: return; _LL504:* _temp481= sd; goto _LL500; _LL506: sd->fields=
sd2->fields; sd->tvs= tvs; return; _LL508: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp525= tvs0; if( _temp525 == 0){ _throw( Null_Exception);} _temp525->tl;}),
tvs2=({ struct Cyc_List_List* _temp526= tvs2; if( _temp526 == 0){ _throw(
Null_Exception);} _temp526->tl;})){ inst=({ struct Cyc_List_List* _temp527=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp527->hd=(
void*)({ struct _tuple4* _temp528=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp528->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp532= tvs2; if( _temp532 == 0){ _throw( Null_Exception);} _temp532->hd;});
_temp528->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp529=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp529[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp530; _temp530.tag= Cyc_Absyn_VarType_tag; _temp530.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp531= tvs0; if( _temp531 == 0){ _throw( Null_Exception);}
_temp531->hd;}); _temp530;}); _temp529;}); _temp528;}); _temp527->tl= inst;
_temp527;});}} if( ! Cyc_Tcutil_same_atts( sd->attributes, sd2->attributes)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp533=* v; xprintf("redeclaration of struct %.*s has different attributes",
_temp533.last_plus_one - _temp533.curr, _temp533.curr);}));}{ struct Cyc_List_List*
f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp534= sd->fields; if(
_temp534 == 0){ _throw( Null_Exception);} _temp534->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp535= sd2->fields; if(
_temp535 == 0){ _throw( Null_Exception);} _temp535->v;}); for( 0; f1s != 0? f2s
!= 0: 0; f1s=({ struct Cyc_List_List* _temp536= f1s; if( _temp536 == 0){ _throw(
Null_Exception);} _temp536->tl;}), f2s=({ struct Cyc_List_List* _temp537= f2s;
if( _temp537 == 0){ _throw( Null_Exception);} _temp537->tl;})){ struct Cyc_Absyn_Structfield
_temp541; struct Cyc_List_List* _temp542; struct Cyc_Core_Opt* _temp544; void*
_temp546; struct Cyc_Absyn_Tqual* _temp548; struct _tagged_string* _temp550;
struct Cyc_Absyn_Structfield* _temp539=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp538= f1s; if( _temp538 == 0){ _throw( Null_Exception);}
_temp538->hd;}); _temp541=* _temp539; _LL551: _temp550=( struct _tagged_string*)
_temp541.name; goto _LL549; _LL549: _temp548=( struct Cyc_Absyn_Tqual*) _temp541.tq;
goto _LL547; _LL547: _temp546=( void*) _temp541.type; goto _LL545; _LL545:
_temp544=( struct Cyc_Core_Opt*) _temp541.width; goto _LL543; _LL543: _temp542=(
struct Cyc_List_List*) _temp541.attributes; goto _LL540; _LL540: { struct Cyc_Absyn_Structfield
_temp555; struct Cyc_List_List* _temp556; struct Cyc_Core_Opt* _temp558; void*
_temp560; struct Cyc_Absyn_Tqual* _temp562; struct _tagged_string* _temp564;
struct Cyc_Absyn_Structfield* _temp553=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp552= f2s; if( _temp552 == 0){ _throw( Null_Exception);}
_temp552->hd;}); _temp555=* _temp553; _LL565: _temp564=( struct _tagged_string*)
_temp555.name; goto _LL563; _LL563: _temp562=( struct Cyc_Absyn_Tqual*) _temp555.tq;
goto _LL561; _LL561: _temp560=( void*) _temp555.type; goto _LL559; _LL559:
_temp558=( struct Cyc_Core_Opt*) _temp555.width; goto _LL557; _LL557: _temp556=(
struct Cyc_List_List*) _temp555.attributes; goto _LL554; _LL554: if( Cyc_String_zstrptrcmp(
_temp550, _temp564) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp566=* v; struct _tagged_string _temp567=* _temp550; struct _tagged_string
_temp568=* _temp564; xprintf("redeclaration of struct %.*s: field name mismatch %.*s != %.*s",
_temp566.last_plus_one - _temp566.curr, _temp566.curr, _temp567.last_plus_one -
_temp567.curr, _temp567.curr, _temp568.last_plus_one - _temp568.curr, _temp568.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp542, _temp556)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp569=* v; struct _tagged_string _temp570=* _temp550; xprintf("redeclaration of struct %.*s: attribute mismatch on field %.*s",
_temp569.last_plus_one - _temp569.curr, _temp569.curr, _temp570.last_plus_one -
_temp570.curr, _temp570.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp548,
_temp562)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp571=* v; struct
_tagged_string _temp572=* _temp550; xprintf("redeclaration of struct %.*s: qualifier mismatch on field %.*s",
_temp571.last_plus_one - _temp571.curr, _temp571.curr, _temp572.last_plus_one -
_temp572.curr, _temp572.curr);}));} if( _temp544 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp573= _temp544; if(
_temp573 == 0){ _throw( Null_Exception);} _temp573->v;})): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp574=* v; struct _tagged_string _temp575=*
_temp550; xprintf("redeclaration of struct %.*s: bad bitfield %.*s", _temp574.last_plus_one
- _temp574.curr, _temp574.curr, _temp575.last_plus_one - _temp575.curr, _temp575.curr);}));}
else{ if((( _temp544 != 0? _temp558 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp576= _temp544; if( _temp576
== 0){ _throw( Null_Exception);} _temp576->v;})) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp577= _temp558; if( _temp577
== 0){ _throw( Null_Exception);} _temp577->v;})): 0)? 1: _temp544 != _temp558){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp578=* v; struct
_tagged_string _temp579=* _temp550; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp578.last_plus_one - _temp578.curr, _temp578.curr, _temp579.last_plus_one -
_temp579.curr, _temp579.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp546);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp560); if( ! Cyc_Tcutil_unify( _temp546, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp580=* v; struct _tagged_string _temp581=* _temp550;
struct _tagged_string _temp582= Cyc_Absynpp_typ2string( _temp546); struct
_tagged_string _temp583= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of struct %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp580.last_plus_one - _temp580.curr, _temp580.curr, _temp581.last_plus_one -
_temp581.curr, _temp581.curr, _temp582.last_plus_one - _temp582.curr, _temp582.curr,
_temp583.last_plus_one - _temp583.curr, _temp583.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp585=* v; struct
_tagged_string _temp586=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp584= f2s; if( _temp584 == 0){ _throw( Null_Exception);} _temp584->hd;}))->name;
xprintf("redeclaration of struct %.*s is missing field %.*s", _temp585.last_plus_one
- _temp585.curr, _temp585.curr, _temp586.last_plus_one - _temp586.curr, _temp586.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp588=* v;
struct _tagged_string _temp589=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp587= f1s; if( _temp587 == 0){ _throw( Null_Exception);} _temp587->hd;}))->name;
xprintf("redeclaration of struct %.*s has extra field %.*s", _temp588.last_plus_one
- _temp588.curr, _temp588.curr, _temp589.last_plus_one - _temp589.curr, _temp589.curr);}));}
return;}} _LL500:;} goto _LL475;}} _LL475:;}{ struct Cyc_Absyn_Structdecl** sdp=((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->structdecls, v);* sdp=({ struct Cyc_Absyn_Structdecl*
_temp590=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp590->sc=( void*)(( void*) sd->sc); _temp590->name= sd->name; _temp590->tvs=
tvs; _temp590->fields= 0; _temp590->attributes= 0; _temp590;});{ struct Cyc_List_List*
prev_fields= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp591= sd->fields; if( _temp591 == 0){ _throw( Null_Exception);} _temp591->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp592= fs; if( _temp592 == 0){
_throw( Null_Exception);} _temp592->tl;})){ struct Cyc_Absyn_Structfield
_temp596; struct Cyc_List_List* _temp597; struct Cyc_Core_Opt* _temp599; void*
_temp601; struct Cyc_Absyn_Tqual* _temp603; struct _tagged_string* _temp605;
struct Cyc_Absyn_Structfield* _temp594=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp593= fs; if( _temp593 == 0){ _throw( Null_Exception);}
_temp593->hd;}); _temp596=* _temp594; _LL606: _temp605=( struct _tagged_string*)
_temp596.name; goto _LL604; _LL604: _temp603=( struct Cyc_Absyn_Tqual*) _temp596.tq;
goto _LL602; _LL602: _temp601=( void*) _temp596.type; goto _LL600; _LL600:
_temp599=( struct Cyc_Core_Opt*) _temp596.width; goto _LL598; _LL598: _temp597=(
struct Cyc_List_List*) _temp596.attributes; goto _LL595; _LL595: if((( int(*)(
int(* compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp605)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp607=* _temp605;
xprintf("duplicate field %.*s in struct", _temp607.last_plus_one - _temp607.curr,
_temp607.curr);}));} if( Cyc_String_strcmp(* _temp605,( struct _tagged_string)({
char* _temp608=( char*)""; struct _tagged_string _temp609; _temp609.curr=
_temp608; _temp609.base= _temp608; _temp609.last_plus_one= _temp608 + 1;
_temp609;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp610=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp610->hd=( void*) _temp605;
_temp610->tl= prev_fields; _temp610;});} Cyc_Tcutil_check_type( loc, te, tvs,(
void*) Cyc_Absyn_MemKind, _temp601); if( _temp599 != 0){ unsigned int w= 0; if(
! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp611= _temp599; if( _temp611 == 0){ _throw( Null_Exception);} _temp611->v;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp612=* _temp605; xprintf("bitfield %.*s does not have constant width",
_temp612.last_plus_one - _temp612.curr, _temp612.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp613= _temp599; if( _temp613
== 0){ _throw( Null_Exception);} _temp613->v;}));}{ void* _temp614= Cyc_Tcutil_compress(
_temp601); void* _temp620; void* _temp622; _LL616: if(( unsigned int) _temp614 >
4u?(( struct _tunion_struct*) _temp614)->tag == Cyc_Absyn_IntType_tag: 0){
_LL623: _temp622=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp614)->f1;
goto _LL621; _LL621: _temp620=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp614)->f2; goto _LL617;} else{ goto _LL618;} _LL618: goto _LL619; _LL617:{
void* _temp624= _temp620; _LL626: if(( int) _temp624 == Cyc_Absyn_B1){ goto
_LL627;} else{ goto _LL628;} _LL628: if(( int) _temp624 == Cyc_Absyn_B2){ goto
_LL629;} else{ goto _LL630;} _LL630: if(( int) _temp624 == Cyc_Absyn_B4){ goto
_LL631;} else{ goto _LL632;} _LL632: if(( int) _temp624 == Cyc_Absyn_B8){ goto
_LL633;} else{ goto _LL625;} _LL627: if( w > 8){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp634=( char*)"bitfield larger than type"; struct
_tagged_string _temp635; _temp635.curr= _temp634; _temp635.base= _temp634;
_temp635.last_plus_one= _temp634 + 26; _temp635;}));} goto _LL625; _LL629: if( w
> 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp636=( char*)"bitfield larger than type";
struct _tagged_string _temp637; _temp637.curr= _temp636; _temp637.base= _temp636;
_temp637.last_plus_one= _temp636 + 26; _temp637;}));} goto _LL625; _LL631: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp638=( char*)"bitfield larger than type";
struct _tagged_string _temp639; _temp639.curr= _temp638; _temp639.base= _temp638;
_temp639.last_plus_one= _temp638 + 26; _temp639;}));} goto _LL625; _LL633: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp640=( char*)"bitfield larger than type";
struct _tagged_string _temp641; _temp641.curr= _temp640; _temp641.base= _temp640;
_temp641.last_plus_one= _temp640 + 26; _temp641;}));} goto _LL625; _LL625:;}
goto _LL615; _LL619: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp642=*
_temp605; struct _tagged_string _temp643= Cyc_Absynpp_typ2string( _temp601);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp642.last_plus_one
- _temp642.curr, _temp642.curr, _temp643.last_plus_one - _temp643.curr, _temp643.curr);}));
goto _LL615; _LL615:;}} for( 0; _temp597 != 0; _temp597=({ struct Cyc_List_List*
_temp644= _temp597; if( _temp644 == 0){ _throw( Null_Exception);} _temp644->tl;})){
void* _temp646=( void*)({ struct Cyc_List_List* _temp645= _temp597; if( _temp645
== 0){ _throw( Null_Exception);} _temp645->hd;}); int _temp654; _LL648: if(( int)
_temp646 == Cyc_Absyn_Packed_att){ goto _LL649;} else{ goto _LL650;} _LL650: if((
unsigned int) _temp646 > 15u?(( struct _tunion_struct*) _temp646)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL655: _temp654=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp646)->f1;
goto _LL651;} else{ goto _LL652;} _LL652: goto _LL653; _LL649: continue; _LL651:
continue; _LL653: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp657= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp656= _temp597; if( _temp656 == 0){ _throw(
Null_Exception);} _temp656->hd;})); struct _tagged_string _temp658=* v; struct
_tagged_string _temp659=* _temp605; xprintf("bad attribute %.*s in struct %.*s, member %.*s",
_temp657.last_plus_one - _temp657.curr, _temp657.curr, _temp658.last_plus_one -
_temp658.curr, _temp658.curr, _temp659.last_plus_one - _temp659.curr, _temp659.curr);}));
goto _LL647; _LL647:;}}} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp660=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp660->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct* _temp661=(
struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp661[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp662; _temp662.tag= Cyc_Tcenv_StructRes_tag;
_temp662.f1= sd; _temp662;}); _temp661;}); _temp660->f2= 1; _temp660;}));* sdp=
sd;}}}}} static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ if( ud->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp663=( char*)"anonymous unions are not allowed at top level";
struct _tagged_string _temp664; _temp664.curr= _temp663; _temp664.base= _temp663;
_temp664.last_plus_one= _temp663 + 46; _temp664;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp665= ud->name;
if( _temp665 == 0){ _throw( Null_Exception);} _temp665->v;}))).f2;{ struct Cyc_List_List*
atts= ud->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp666=
atts; if( _temp666 == 0){ _throw( Null_Exception);} _temp666->tl;})){ void*
_temp668=( void*)({ struct Cyc_List_List* _temp667= atts; if( _temp667 == 0){
_throw( Null_Exception);} _temp667->hd;}); int _temp676; _LL670: if(( int)
_temp668 == Cyc_Absyn_Packed_att){ goto _LL671;} else{ goto _LL672;} _LL672: if((
unsigned int) _temp668 > 15u?(( struct _tunion_struct*) _temp668)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL677: _temp676=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp668)->f1;
goto _LL673;} else{ goto _LL674;} _LL674: goto _LL675; _LL671: continue; _LL673:
continue; _LL675: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp679= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp678= atts; if( _temp678 == 0){ _throw(
Null_Exception);} _temp678->hd;})); struct _tagged_string _temp680=* v; xprintf("bad attribute %.*s in union %.*s definition",
_temp679.last_plus_one - _temp679.curr, _temp679.curr, _temp680.last_plus_one -
_temp680.curr, _temp680.curr);})); goto _LL669; _LL669:;}}{ struct Cyc_List_List*
tvs2= ud->tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp681= tvs2;
if( _temp681 == 0){ _throw( Null_Exception);} _temp681->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp682= tvs2; if( _temp682 ==
0){ _throw( Null_Exception);} _temp682->hd;}))->kind); void* _temp683=( void*) c->v;
void* _temp691; _LL685: if(( int) _temp683 == Cyc_Absyn_No_constr){ goto _LL686;}
else{ goto _LL687;} _LL687: if(( unsigned int) _temp683 > 1u?(( struct
_tunion_struct*) _temp683)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL692: _temp691=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp683)->f1; if(( int) _temp691
== Cyc_Absyn_MemKind){ goto _LL688;} else{ goto _LL689;}} else{ goto _LL689;}
_LL689: goto _LL690; _LL686:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp693=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp693[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp694; _temp694.tag= Cyc_Absyn_Eq_constr_tag;
_temp694.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp694;}); _temp693;})));
continue; _LL688: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp696=* v;
struct _tagged_string _temp697=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp695= tvs2; if( _temp695 == 0){ _throw( Null_Exception);} _temp695->hd;}))->name;
xprintf("union %.*s attempts to abstract type variable %.*s of kind M", _temp696.last_plus_one
- _temp696.curr, _temp696.curr, _temp697.last_plus_one - _temp697.curr, _temp697.curr);}));
continue; _LL690: continue; _LL684:;}}{ struct Cyc_List_List* tvs= ud->tvs;{
void* _temp699=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp698= ud->name;
if( _temp698 == 0){ _throw( Null_Exception);} _temp698->v;}))).f1; struct Cyc_List_List*
_temp707; struct Cyc_List_List* _temp709; _LL701: if(( unsigned int) _temp699 >
1u?(( struct _tunion_struct*) _temp699)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL708:
_temp707=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp699)->f1;
if( _temp707 == 0){ goto _LL702;} else{ goto _LL703;}} else{ goto _LL703;}
_LL703: if(( unsigned int) _temp699 > 1u?(( struct _tunion_struct*) _temp699)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL710: _temp709=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp699)->f1; if( _temp709 == 0){ goto _LL704;} else{
goto _LL705;}} else{ goto _LL705;} _LL705: goto _LL706; _LL702: goto _LL700;
_LL704: goto _LL700; _LL706: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp711=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp712; _temp712.curr= _temp711; _temp712.base= _temp711;
_temp712.last_plus_one= _temp711 + 43; _temp712;})); return; _LL700:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp713= ud->name; if( _temp713 == 0){
_throw( Null_Exception);} _temp713->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp714=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp714[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp715; _temp715.tag= Cyc_Absyn_Abs_n_tag;
_temp715.f1= te->ns; _temp715;}); _temp714;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp716=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->uniondecls, v); struct
Cyc_Core_Opt _temp722; struct Cyc_Absyn_Uniondecl** _temp723; _LL718: if(
_temp716 == 0){ goto _LL719;} else{ goto _LL720;} _LL720: if( _temp716 == 0){
goto _LL717;} else{ _temp722=* _temp716; _LL724: _temp723=( struct Cyc_Absyn_Uniondecl**)
_temp722.v; goto _LL721;} _LL719: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl**
data)) Cyc_Dict_insert)( ge->uniondecls, v,({ struct Cyc_Absyn_Uniondecl**
_temp725=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp725[ 0]= ud; _temp725;})); if( ud->fields == 0){ return;} goto _LL717;
_LL721: { struct Cyc_Absyn_Uniondecl* ud2=* _temp723; struct Cyc_List_List* tvs2=
ud2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp726=* v; xprintf("redeclaration of union %.*s has a different number of type parameters",
_temp726.last_plus_one - _temp726.curr, _temp726.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp727= x1; if( _temp727 == 0){ _throw( Null_Exception);} _temp727->tl;}), x2=({
struct Cyc_List_List* _temp728= x2; if( _temp728 == 0){ _throw( Null_Exception);}
_temp728->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp729= x1; if( _temp729 == 0){ _throw( Null_Exception);}
_temp729->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp730= x2; if( _temp730 == 0){ _throw( Null_Exception);}
_temp730->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp731=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp731[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp732; _temp732.tag= Cyc_Absyn_Forward_constr_tag; _temp732.f1= c2; _temp732;});
_temp731;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp734=* v; struct _tagged_string _temp735= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp736=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp733= x1; if( _temp733 == 0){ _throw( Null_Exception);} _temp733->hd;}))->name;
struct _tagged_string _temp737= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of union %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp734.last_plus_one - _temp734.curr, _temp734.curr, _temp735.last_plus_one -
_temp735.curr, _temp735.curr, _temp736.last_plus_one - _temp736.curr, _temp736.curr,
_temp737.last_plus_one - _temp737.curr, _temp737.curr);}));}}( void*)( ud->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ud->sc, ud->fields,( void*)
ud2->sc, ud2->fields, loc,( struct _tagged_string)({ char* _temp738=( char*)"union";
struct _tagged_string _temp739; _temp739.curr= _temp738; _temp739.base= _temp738;
_temp739.last_plus_one= _temp738 + 6; _temp739;}), v));{ struct _tuple7 _temp741=({
struct _tuple7 _temp740; _temp740.f1= ud->fields; _temp740.f2= ud2->fields;
_temp740;}); struct Cyc_Core_Opt* _temp751; struct Cyc_Core_Opt* _temp753;
struct Cyc_Core_Opt* _temp755; struct Cyc_Core_Opt* _temp757; struct Cyc_Core_Opt*
_temp759; struct Cyc_Core_Opt* _temp761; struct Cyc_Core_Opt* _temp763; struct
Cyc_Core_Opt* _temp765; _LL743: _LL754: _temp753= _temp741.f1; if( _temp753 == 0){
goto _LL752;} else{ goto _LL745;} _LL752: _temp751= _temp741.f2; if( _temp751 ==
0){ goto _LL744;} else{ goto _LL745;} _LL745: _LL758: _temp757= _temp741.f1;
goto _LL756; _LL756: _temp755= _temp741.f2; if( _temp755 == 0){ goto _LL746;}
else{ goto _LL747;} _LL747: _LL762: _temp761= _temp741.f1; if( _temp761 == 0){
goto _LL760;} else{ goto _LL749;} _LL760: _temp759= _temp741.f2; goto _LL748;
_LL749: _LL766: _temp765= _temp741.f1; goto _LL764; _LL764: _temp763= _temp741.f2;
goto _LL750; _LL744: return; _LL746:* _temp723= ud; goto _LL742; _LL748: ud->fields=
ud2->fields; ud->tvs= tvs; return; _LL750: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp767= tvs0; if( _temp767 == 0){ _throw( Null_Exception);} _temp767->tl;}),
tvs2=({ struct Cyc_List_List* _temp768= tvs2; if( _temp768 == 0){ _throw(
Null_Exception);} _temp768->tl;})){ inst=({ struct Cyc_List_List* _temp769=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp769->hd=(
void*)({ struct _tuple4* _temp770=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp770->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp774= tvs2; if( _temp774 == 0){ _throw( Null_Exception);} _temp774->hd;});
_temp770->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp771=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp771[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp772; _temp772.tag= Cyc_Absyn_VarType_tag; _temp772.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp773= tvs0; if( _temp773 == 0){ _throw( Null_Exception);}
_temp773->hd;}); _temp772;}); _temp771;}); _temp770;}); _temp769->tl= inst;
_temp769;});}} if( ! Cyc_Tcutil_same_atts( ud->attributes, ud2->attributes)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp775=* v; xprintf("redeclaration of union %.*s has different attributes",
_temp775.last_plus_one - _temp775.curr, _temp775.curr);}));}{ struct Cyc_List_List*
f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp776= ud->fields; if(
_temp776 == 0){ _throw( Null_Exception);} _temp776->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp777= ud2->fields; if(
_temp777 == 0){ _throw( Null_Exception);} _temp777->v;}); for( 0; f1s != 0? f2s
!= 0: 0; f1s=({ struct Cyc_List_List* _temp778= f1s; if( _temp778 == 0){ _throw(
Null_Exception);} _temp778->tl;}), f2s=({ struct Cyc_List_List* _temp779= f2s;
if( _temp779 == 0){ _throw( Null_Exception);} _temp779->tl;})){ struct Cyc_Absyn_Structfield
_temp783; struct Cyc_List_List* _temp784; struct Cyc_Core_Opt* _temp786; void*
_temp788; struct Cyc_Absyn_Tqual* _temp790; struct _tagged_string* _temp792;
struct Cyc_Absyn_Structfield* _temp781=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp780= f1s; if( _temp780 == 0){ _throw( Null_Exception);}
_temp780->hd;}); _temp783=* _temp781; _LL793: _temp792=( struct _tagged_string*)
_temp783.name; goto _LL791; _LL791: _temp790=( struct Cyc_Absyn_Tqual*) _temp783.tq;
goto _LL789; _LL789: _temp788=( void*) _temp783.type; goto _LL787; _LL787:
_temp786=( struct Cyc_Core_Opt*) _temp783.width; goto _LL785; _LL785: _temp784=(
struct Cyc_List_List*) _temp783.attributes; goto _LL782; _LL782: { struct Cyc_Absyn_Structfield
_temp797; struct Cyc_List_List* _temp798; struct Cyc_Core_Opt* _temp800; void*
_temp802; struct Cyc_Absyn_Tqual* _temp804; struct _tagged_string* _temp806;
struct Cyc_Absyn_Structfield* _temp795=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp794= f2s; if( _temp794 == 0){ _throw( Null_Exception);}
_temp794->hd;}); _temp797=* _temp795; _LL807: _temp806=( struct _tagged_string*)
_temp797.name; goto _LL805; _LL805: _temp804=( struct Cyc_Absyn_Tqual*) _temp797.tq;
goto _LL803; _LL803: _temp802=( void*) _temp797.type; goto _LL801; _LL801:
_temp800=( struct Cyc_Core_Opt*) _temp797.width; goto _LL799; _LL799: _temp798=(
struct Cyc_List_List*) _temp797.attributes; goto _LL796; _LL796: if( Cyc_String_zstrptrcmp(
_temp792, _temp806) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp808=* v; struct _tagged_string _temp809=* _temp792; struct _tagged_string
_temp810=* _temp806; xprintf("redeclaration of union %.*s: field name mismatch %.*s != %.*s",
_temp808.last_plus_one - _temp808.curr, _temp808.curr, _temp809.last_plus_one -
_temp809.curr, _temp809.curr, _temp810.last_plus_one - _temp810.curr, _temp810.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp784, _temp798)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp811=* v; struct _tagged_string _temp812=* _temp792; xprintf("redeclaration of union %.*s: attribute mismatch on field %.*s",
_temp811.last_plus_one - _temp811.curr, _temp811.curr, _temp812.last_plus_one -
_temp812.curr, _temp812.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp790,
_temp804)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp813=* v; struct
_tagged_string _temp814=* _temp792; xprintf("redeclaration of union %.*s: qualifier mismatch on field %.*s",
_temp813.last_plus_one - _temp813.curr, _temp813.curr, _temp814.last_plus_one -
_temp814.curr, _temp814.curr);}));} if( _temp786 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp815= _temp786; if(
_temp815 == 0){ _throw( Null_Exception);} _temp815->v;})): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp816=* v; struct _tagged_string _temp817=*
_temp792; xprintf("redeclaration of struct %.*s: bad bitfield %.*s", _temp816.last_plus_one
- _temp816.curr, _temp816.curr, _temp817.last_plus_one - _temp817.curr, _temp817.curr);}));}
else{ if((( _temp786 != 0? _temp800 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp818= _temp786; if( _temp818
== 0){ _throw( Null_Exception);} _temp818->v;})) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp819= _temp800; if( _temp819
== 0){ _throw( Null_Exception);} _temp819->v;})): 0)? 1: _temp786 != _temp800){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp820=* v; struct
_tagged_string _temp821=* _temp792; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp820.last_plus_one - _temp820.curr, _temp820.curr, _temp821.last_plus_one -
_temp821.curr, _temp821.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp788);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp802); if( ! Cyc_Tcutil_unify( _temp788, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp822=* v; struct _tagged_string _temp823=* _temp792;
struct _tagged_string _temp824= Cyc_Absynpp_typ2string( _temp788); struct
_tagged_string _temp825= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of union %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp822.last_plus_one - _temp822.curr, _temp822.curr, _temp823.last_plus_one -
_temp823.curr, _temp823.curr, _temp824.last_plus_one - _temp824.curr, _temp824.curr,
_temp825.last_plus_one - _temp825.curr, _temp825.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp827=* v; struct
_tagged_string _temp828=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp826= f2s; if( _temp826 == 0){ _throw( Null_Exception);} _temp826->hd;}))->name;
xprintf("redeclaration of union %.*s is missing field %.*s", _temp827.last_plus_one
- _temp827.curr, _temp827.curr, _temp828.last_plus_one - _temp828.curr, _temp828.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp830=* v;
struct _tagged_string _temp831=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp829= f1s; if( _temp829 == 0){ _throw( Null_Exception);} _temp829->hd;}))->name;
xprintf("redeclaration of union %.*s has extra field %.*s", _temp830.last_plus_one
- _temp830.curr, _temp830.curr, _temp831.last_plus_one - _temp831.curr, _temp831.curr);}));}
return;}} _LL742:;} goto _LL717;}} _LL717:;}{ struct Cyc_Absyn_Uniondecl** udp=((
struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->uniondecls, v);* udp=({ struct Cyc_Absyn_Uniondecl*
_temp832=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp832->sc=( void*)(( void*) ud->sc); _temp832->name= ud->name; _temp832->tvs=
tvs; _temp832->fields= 0; _temp832->attributes= 0; _temp832;});{ struct Cyc_List_List*
prev_fields= 0; struct Cyc_List_List* prev_types= 0;{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp833= ud->fields; if( _temp833
== 0){ _throw( Null_Exception);} _temp833->v;}); for( 0; fs != 0; fs=({ struct
Cyc_List_List* _temp834= fs; if( _temp834 == 0){ _throw( Null_Exception);}
_temp834->tl;})){ struct Cyc_Absyn_Structfield _temp838; struct Cyc_List_List*
_temp839; struct Cyc_Core_Opt* _temp841; void* _temp843; struct Cyc_Absyn_Tqual*
_temp845; struct _tagged_string* _temp847; struct Cyc_Absyn_Structfield*
_temp836=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp835= fs;
if( _temp835 == 0){ _throw( Null_Exception);} _temp835->hd;}); _temp838=*
_temp836; _LL848: _temp847=( struct _tagged_string*) _temp838.name; goto _LL846;
_LL846: _temp845=( struct Cyc_Absyn_Tqual*) _temp838.tq; goto _LL844; _LL844:
_temp843=( void*) _temp838.type; goto _LL842; _LL842: _temp841=( struct Cyc_Core_Opt*)
_temp838.width; goto _LL840; _LL840: _temp839=( struct Cyc_List_List*) _temp838.attributes;
goto _LL837; _LL837: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp847)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp849=* _temp847; xprintf("duplicate field %.*s in union",
_temp849.last_plus_one - _temp849.curr, _temp849.curr);}));} prev_fields=({
struct Cyc_List_List* _temp850=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp850->hd=( void*) _temp847; _temp850->tl=
prev_fields; _temp850;}); Cyc_Tcutil_check_type( loc, te, tvs,( void*) Cyc_Absyn_MemKind,
_temp843); if( _temp841 != 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp851= _temp841; if(
_temp851 == 0){ _throw( Null_Exception);} _temp851->v;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp852=* _temp847; xprintf("bitfield %.*s does not have constant width",
_temp852.last_plus_one - _temp852.curr, _temp852.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp853= _temp841; if( _temp853
== 0){ _throw( Null_Exception);} _temp853->v;}));}{ void* _temp854= Cyc_Tcutil_compress(
_temp843); void* _temp860; void* _temp862; _LL856: if(( unsigned int) _temp854 >
4u?(( struct _tunion_struct*) _temp854)->tag == Cyc_Absyn_IntType_tag: 0){
_LL863: _temp862=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp854)->f1;
goto _LL861; _LL861: _temp860=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp854)->f2; goto _LL857;} else{ goto _LL858;} _LL858: goto _LL859; _LL857:{
void* _temp864= _temp860; _LL866: if(( int) _temp864 == Cyc_Absyn_B1){ goto
_LL867;} else{ goto _LL868;} _LL868: if(( int) _temp864 == Cyc_Absyn_B2){ goto
_LL869;} else{ goto _LL870;} _LL870: if(( int) _temp864 == Cyc_Absyn_B4){ goto
_LL871;} else{ goto _LL872;} _LL872: if(( int) _temp864 == Cyc_Absyn_B8){ goto
_LL873;} else{ goto _LL865;} _LL867: if( w > 8){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp874=( char*)"bitfield larger than type"; struct
_tagged_string _temp875; _temp875.curr= _temp874; _temp875.base= _temp874;
_temp875.last_plus_one= _temp874 + 26; _temp875;}));} goto _LL865; _LL869: if( w
> 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp876=( char*)"bitfield larger than type";
struct _tagged_string _temp877; _temp877.curr= _temp876; _temp877.base= _temp876;
_temp877.last_plus_one= _temp876 + 26; _temp877;}));} goto _LL865; _LL871: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp878=( char*)"bitfield larger than type";
struct _tagged_string _temp879; _temp879.curr= _temp878; _temp879.base= _temp878;
_temp879.last_plus_one= _temp878 + 26; _temp879;}));} goto _LL865; _LL873: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp880=( char*)"bitfield larger than type";
struct _tagged_string _temp881; _temp881.curr= _temp880; _temp881.base= _temp880;
_temp881.last_plus_one= _temp880 + 26; _temp881;}));} goto _LL865; _LL865:;}
goto _LL855; _LL859: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp882=*
_temp847; struct _tagged_string _temp883= Cyc_Absynpp_typ2string( _temp843);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp882.last_plus_one
- _temp882.curr, _temp882.curr, _temp883.last_plus_one - _temp883.curr, _temp883.curr);}));
goto _LL855; _LL855:;}} if( ! Cyc_Tcutil_bits_only( _temp843)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp884=* _temp847; struct _tagged_string _temp885=*
v; xprintf("%.*s has a type that is possibly incompatible with other members of union %.*s",
_temp884.last_plus_one - _temp884.curr, _temp884.curr, _temp885.last_plus_one -
_temp885.curr, _temp885.curr);}));} for( 0; _temp839 != 0; _temp839=({ struct
Cyc_List_List* _temp886= _temp839; if( _temp886 == 0){ _throw( Null_Exception);}
_temp886->tl;})){ void* _temp888=( void*)({ struct Cyc_List_List* _temp887=
_temp839; if( _temp887 == 0){ _throw( Null_Exception);} _temp887->hd;}); int
_temp896; _LL890: if(( int) _temp888 == Cyc_Absyn_Packed_att){ goto _LL891;}
else{ goto _LL892;} _LL892: if(( unsigned int) _temp888 > 15u?(( struct
_tunion_struct*) _temp888)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL897:
_temp896=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp888)->f1; goto
_LL893;} else{ goto _LL894;} _LL894: goto _LL895; _LL891: continue; _LL893:
continue; _LL895: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp899= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp898= _temp839; if( _temp898 == 0){ _throw(
Null_Exception);} _temp898->hd;})); struct _tagged_string _temp900=* v; struct
_tagged_string _temp901=* _temp847; xprintf("bad attribute %.*s in union %.*s, member %.*s",
_temp899.last_plus_one - _temp899.curr, _temp899.curr, _temp900.last_plus_one -
_temp900.curr, _temp900.curr, _temp901.last_plus_one - _temp901.curr, _temp901.curr);}));
goto _LL889; _LL889:;}}}* udp= ud;}}}}} static void Cyc_Tc_tcTuniondecl( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Tuniondecl* ed){ if( ed->name == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp902=( char*)"anonymous tunions are not allowed";
struct _tagged_string _temp903; _temp903.curr= _temp902; _temp903.base= _temp902;
_temp903.last_plus_one= _temp902 + 34; _temp903;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp904= ed->name;
if( _temp904 == 0){ _throw( Null_Exception);} _temp904->v;}))).f2; struct Cyc_List_List*
tvs= ed->tvs;{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2=({
struct Cyc_List_List* _temp905= tvs2; if( _temp905 == 0){ _throw( Null_Exception);}
_temp905->tl;})){ struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp906= tvs2; if( _temp906 == 0){ _throw( Null_Exception);}
_temp906->hd;}))->kind); void* _temp907=( void*) c->v; void* _temp915; _LL909:
if(( int) _temp907 == Cyc_Absyn_No_constr){ goto _LL910;} else{ goto _LL911;}
_LL911: if(( unsigned int) _temp907 > 1u?(( struct _tunion_struct*) _temp907)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL916: _temp915=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp907)->f1; if(( int) _temp915 == Cyc_Absyn_MemKind){ goto _LL912;} else{
goto _LL913;}} else{ goto _LL913;} _LL913: goto _LL914; _LL910:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp917=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp917[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp918; _temp918.tag= Cyc_Absyn_Eq_constr_tag;
_temp918.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp918;}); _temp917;})));
goto _LL908; _LL912: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp920=* v;
struct _tagged_string _temp921=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp919= tvs2; if( _temp919 == 0){ _throw( Null_Exception);} _temp919->hd;}))->name;
xprintf("tunion %.*s attempts to abstract type variable %.*s of kind M",
_temp920.last_plus_one - _temp920.curr, _temp920.curr, _temp921.last_plus_one -
_temp921.curr, _temp921.curr);})); goto _LL908; _LL914: goto _LL908; _LL908:;}}{
void* _temp923=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp922= ed->name;
if( _temp922 == 0){ _throw( Null_Exception);} _temp922->v;}))).f1; struct Cyc_List_List*
_temp931; struct Cyc_List_List* _temp933; _LL925: if(( unsigned int) _temp923 >
1u?(( struct _tunion_struct*) _temp923)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL932:
_temp931=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp923)->f1;
if( _temp931 == 0){ goto _LL926;} else{ goto _LL927;}} else{ goto _LL927;}
_LL927: if(( unsigned int) _temp923 > 1u?(( struct _tunion_struct*) _temp923)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL934: _temp933=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp923)->f1; if( _temp933 == 0){ goto _LL928;} else{
goto _LL929;}} else{ goto _LL929;} _LL929: goto _LL930; _LL926: goto _LL924;
_LL928: goto _LL924; _LL930: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp935=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp936; _temp936.curr= _temp935; _temp936.base= _temp935;
_temp936.last_plus_one= _temp935 + 43; _temp936;})); return; _LL924:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp937= ed->name; if( _temp937 == 0){
_throw( Null_Exception);} _temp937->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp938=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp938[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp939; _temp939.tag= Cyc_Absyn_Abs_n_tag;
_temp939.f1= te->ns; _temp939;}); _temp938;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp940=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);
struct Cyc_Core_Opt _temp946; struct Cyc_Absyn_Tuniondecl** _temp947; _LL942:
if( _temp940 == 0){ goto _LL943;} else{ goto _LL944;} _LL944: if( _temp940 == 0){
goto _LL941;} else{ _temp946=* _temp940; _LL948: _temp947=( struct Cyc_Absyn_Tuniondecl**)
_temp946.v; goto _LL945;} _LL943: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl**
data)) Cyc_Dict_insert)( ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl**
_temp949=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp949[ 0]= ed; _temp949;})); if( ed->fields == 0){ return;} goto _LL941;
_LL945: { struct Cyc_Absyn_Tuniondecl* ed2=* _temp947; struct Cyc_List_List*
tvs2= ed2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp950=* v; xprintf("redeclaration of tunion %.*s has a different number of type parameters",
_temp950.last_plus_one - _temp950.curr, _temp950.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp951= x1; if( _temp951 == 0){ _throw( Null_Exception);} _temp951->tl;}), x2=({
struct Cyc_List_List* _temp952= x2; if( _temp952 == 0){ _throw( Null_Exception);}
_temp952->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp953= x1; if( _temp953 == 0){ _throw( Null_Exception);}
_temp953->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp954= x2; if( _temp954 == 0){ _throw( Null_Exception);}
_temp954->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp955=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp955[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp956; _temp956.tag= Cyc_Absyn_Forward_constr_tag; _temp956.f1= c2; _temp956;});
_temp955;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp958=* v; struct _tagged_string _temp959= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp960=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp957= x1; if( _temp957 == 0){ _throw( Null_Exception);} _temp957->hd;}))->name;
struct _tagged_string _temp961= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of tunion %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp958.last_plus_one - _temp958.curr, _temp958.curr, _temp959.last_plus_one -
_temp959.curr, _temp959.curr, _temp960.last_plus_one - _temp960.curr, _temp960.curr,
_temp961.last_plus_one - _temp961.curr, _temp961.curr);}));}}( void*)( ed->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, ed->fields,( void*)
ed2->sc, ed2->fields, loc,( struct _tagged_string)({ char* _temp962=( char*)"tunion";
struct _tagged_string _temp963; _temp963.curr= _temp962; _temp963.base= _temp962;
_temp963.last_plus_one= _temp962 + 7; _temp963;}), v));{ struct _tuple7 _temp965=({
struct _tuple7 _temp964; _temp964.f1= ed->fields; _temp964.f2= ed2->fields;
_temp964;}); struct Cyc_Core_Opt* _temp975; struct Cyc_Core_Opt* _temp977;
struct Cyc_Core_Opt* _temp979; struct Cyc_Core_Opt* _temp981; struct Cyc_Core_Opt*
_temp983; struct Cyc_Core_Opt* _temp985; struct Cyc_Core_Opt* _temp987; struct
Cyc_Core_Opt* _temp989; _LL967: _LL978: _temp977= _temp965.f1; if( _temp977 == 0){
goto _LL976;} else{ goto _LL969;} _LL976: _temp975= _temp965.f2; if( _temp975 ==
0){ goto _LL968;} else{ goto _LL969;} _LL969: _LL982: _temp981= _temp965.f1;
goto _LL980; _LL980: _temp979= _temp965.f2; if( _temp979 == 0){ goto _LL970;}
else{ goto _LL971;} _LL971: _LL986: _temp985= _temp965.f1; if( _temp985 == 0){
goto _LL984;} else{ goto _LL973;} _LL984: _temp983= _temp965.f2; goto _LL972;
_LL973: _LL990: _temp989= _temp965.f1; goto _LL988; _LL988: _temp987= _temp965.f2;
goto _LL974; _LL968: return; _LL970:* _temp947= ed; goto _LL966; _LL972: ed->fields=
ed2->fields; ed->tvs= tvs2; return; _LL974: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp991= tvs0; if( _temp991 == 0){ _throw( Null_Exception);} _temp991->tl;}),
tvs2=({ struct Cyc_List_List* _temp992= tvs2; if( _temp992 == 0){ _throw(
Null_Exception);} _temp992->tl;})){ inst=({ struct Cyc_List_List* _temp993=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp993->hd=(
void*)({ struct _tuple4* _temp994=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp994->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp998= tvs2; if( _temp998 == 0){ _throw( Null_Exception);} _temp998->hd;});
_temp994->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp995=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp995[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp996; _temp996.tag= Cyc_Absyn_VarType_tag; _temp996.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp997= tvs0; if( _temp997 == 0){ _throw( Null_Exception);}
_temp997->hd;}); _temp996;}); _temp995;}); _temp994;}); _temp993->tl= inst;
_temp993;});}}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp999= ed->fields; if( _temp999 == 0){ _throw( Null_Exception);} _temp999->v;});
struct Cyc_List_List* f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1000= ed2->fields; if( _temp1000 == 0){ _throw( Null_Exception);} _temp1000->v;});
for( 0; f1s != 0? f2s != 0: 0; f1s=({ struct Cyc_List_List* _temp1001= f1s; if(
_temp1001 == 0){ _throw( Null_Exception);} _temp1001->tl;}), f2s=({ struct Cyc_List_List*
_temp1002= f2s; if( _temp1002 == 0){ _throw( Null_Exception);} _temp1002->tl;})){(*((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1003= f1s; if(
_temp1003 == 0){ _throw( Null_Exception);} _temp1003->hd;}))->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp1004=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1004[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1005; _temp1005.tag= Cyc_Absyn_Abs_n_tag; _temp1005.f1= te->ns; _temp1005;});
_temp1004;});{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1006= f1s; if( _temp1006 == 0){ _throw(
Null_Exception);} _temp1006->hd;}))->tvs); Cyc_Tcutil_check_unique_tvars(((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1007= f1s; if(
_temp1007 == 0){ _throw( Null_Exception);} _temp1007->hd;}))->loc, alltvs);{
struct Cyc_List_List* typs=(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1008= f1s; if( _temp1008 == 0){ _throw( Null_Exception);} _temp1008->hd;}))->typs;
for( 0; typs != 0; typs=({ struct Cyc_List_List* _temp1009= typs; if( _temp1009
== 0){ _throw( Null_Exception);} _temp1009->tl;})){ void* t=(*(( struct _tuple5*)({
struct Cyc_List_List* _temp1010= typs; if( _temp1010 == 0){ _throw(
Null_Exception);} _temp1010->hd;}))).f2; Cyc_Tcutil_check_type((( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1011= f1s; if( _temp1011 == 0){ _throw(
Null_Exception);} _temp1011->hd;}))->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,
t);}} Cyc_Tc_field_redecl_okay(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1012= f1s; if( _temp1012 == 0){ _throw( Null_Exception);} _temp1012->hd;}),(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1013= f2s; if(
_temp1013 == 0){ _throw( Null_Exception);} _temp1013->hd;}), inst, te,( struct
_tagged_string)({ char* _temp1014=( char*)"tunion"; struct _tagged_string
_temp1015; _temp1015.curr= _temp1014; _temp1015.base= _temp1014; _temp1015.last_plus_one=
_temp1014 + 7; _temp1015;}), v);}} if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1017=* v; struct _tagged_string _temp1018=*(*(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1016= f1s; if( _temp1016 == 0){ _throw(
Null_Exception);} _temp1016->hd;}))->name).f2; xprintf("redeclaration of tunion %.*s has extra field %.*s",
_temp1017.last_plus_one - _temp1017.curr, _temp1017.curr, _temp1018.last_plus_one
- _temp1018.curr, _temp1018.curr);}));} if( f2s != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1020=* v; struct _tagged_string _temp1021=*(*((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1019= f2s; if(
_temp1019 == 0){ _throw( Null_Exception);} _temp1019->hd;}))->name).f2; xprintf("redeclaration of tunion %.*s is missing field %.*s",
_temp1020.last_plus_one - _temp1020.curr, _temp1020.curr, _temp1021.last_plus_one
- _temp1021.curr, _temp1021.curr);}));} return;}} _LL966:;} goto _LL941;}}
_LL941:;}{ struct Cyc_Absyn_Tuniondecl** edp=(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);* edp=({ struct Cyc_Absyn_Tuniondecl* _temp1022=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp1022->sc=( void*)(( void*)
ed->sc); _temp1022->name= ed->name; _temp1022->tvs= tvs; _temp1022->fields= 0;
_temp1022;});{ struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1023= ed->fields; if(
_temp1023 == 0){ _throw( Null_Exception);} _temp1023->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp1024= fs; if( _temp1024 == 0){ _throw( Null_Exception);}
_temp1024->tl;})){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1025= fs; if( _temp1025 == 0){ _throw( Null_Exception);}
_temp1025->hd;}); if((( int(*)( int(* compare)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* f->name).f2)){ Cyc_Tcutil_terr( f->loc,({
struct _tagged_string _temp1026=*(* f->name).f2; xprintf("duplicate field name %.*s",
_temp1026.last_plus_one - _temp1026.curr, _temp1026.curr);}));} prev_fields=({
struct Cyc_List_List* _temp1027=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1027->hd=( void*)(* f->name).f2; _temp1027->tl=
prev_fields; _temp1027;});{ struct Cyc_List_List* tvs= f->tvs; for( 0; tvs != 0;
tvs=({ struct Cyc_List_List* _temp1028= tvs; if( _temp1028 == 0){ _throw(
Null_Exception);} _temp1028->tl;})){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1029= tvs; if( _temp1029 == 0){ _throw(
Null_Exception);} _temp1029->hd;}); struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( tv->kind); void*
_temp1030=( void*) c->v; void* _temp1038; _LL1032: if(( int) _temp1030 == Cyc_Absyn_No_constr){
goto _LL1033;} else{ goto _LL1034;} _LL1034: if(( unsigned int) _temp1030 > 1u?((
struct _tunion_struct*) _temp1030)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1039:
_temp1038=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1030)->f1; if((
int) _temp1038 == Cyc_Absyn_MemKind){ goto _LL1035;} else{ goto _LL1036;}} else{
goto _LL1036;} _LL1036: goto _LL1037; _LL1033:( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp1040=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1040[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1041; _temp1041.tag= Cyc_Absyn_Eq_constr_tag;
_temp1041.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp1041;}); _temp1040;})));
goto _LL1031; _LL1035: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1042=*(*
f->name).f2; xprintf("field %.*s abstracts type variable of kind M", _temp1042.last_plus_one
- _temp1042.curr, _temp1042.curr);})); goto _LL1031; _LL1037: goto _LL1031;
_LL1031:;}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, f->tvs); Cyc_Tcutil_check_unique_tvars(
loc, alltvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=({
struct Cyc_List_List* _temp1043= typs; if( _temp1043 == 0){ _throw(
Null_Exception);} _temp1043->tl;})){ void* t=(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp1044= typs; if( _temp1044 == 0){ _throw( Null_Exception);} _temp1044->hd;}))).f2;
Cyc_Tcutil_check_type( f->loc, te, alltvs,( void*) Cyc_Absyn_MemKind, t);}}(* f->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1045=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1045[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1046; _temp1046.tag= Cyc_Absyn_Abs_n_tag; _temp1046.f1= te->ns; _temp1046;});
_temp1045;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,(*
f->name).f2,({ struct _tuple6* _temp1047=( struct _tuple6*) GC_malloc( sizeof(
struct _tuple6)); _temp1047->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct*
_temp1048=( struct Cyc_Tcenv_TunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct));
_temp1048[ 0]=({ struct Cyc_Tcenv_TunionRes_struct _temp1049; _temp1049.tag= Cyc_Tcenv_TunionRes_tag;
_temp1049.f1= ed; _temp1049.f2= f; _temp1049;}); _temp1048;}); _temp1047->f2= 1;
_temp1047;}));}}} ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp1050=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp1050[ 0]= ed; _temp1050;}));*
edp= ed;}}}} static void Cyc_Tc_tcXTuniondecl( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_XTuniondecl*
xd){ struct _tagged_string* v=(* xd->name).f2; struct Cyc_Core_Opt* xd2o;{
struct _handler_cons _temp1051; _push_handler(& _temp1051);{ struct
_xtunion_struct* _temp1052=( struct _xtunion_struct*) setjmp( _temp1051.handler);
if( ! _temp1052){ xd2o= Cyc_Tcenv_lookup_xtuniondecl( te, loc, xd->name);;
_pop_handler();} else{ struct _xtunion_struct* _temp1054= _temp1052; _LL1056:
if( _temp1054->tag == Cyc_Dict_Absent_tag){ goto _LL1057;} else{ goto _LL1058;}
_LL1058: goto _LL1059; _LL1057: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp1060= Cyc_Absynpp_qvar2string( xd->name); xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp1060.last_plus_one - _temp1060.curr, _temp1060.curr);})); return; _LL1059:(
void) _throw( _temp1054); _LL1055:;}}}{ void* _temp1061=(* xd->name).f1; struct
Cyc_List_List* _temp1071; struct Cyc_List_List* _temp1073; struct Cyc_List_List*
_temp1075; _LL1063: if(( int) _temp1061 == Cyc_Absyn_Loc_n){ goto _LL1064;}
else{ goto _LL1065;} _LL1065: if(( unsigned int) _temp1061 > 1u?(( struct
_tunion_struct*) _temp1061)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1072: _temp1071=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1061)->f1; if(
_temp1071 == 0){ goto _LL1066;} else{ goto _LL1067;}} else{ goto _LL1067;}
_LL1067: if(( unsigned int) _temp1061 > 1u?(( struct _tunion_struct*) _temp1061)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL1074: _temp1073=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp1061)->f1; goto _LL1068;} else{ goto _LL1069;}
_LL1069: if(( unsigned int) _temp1061 > 1u?(( struct _tunion_struct*) _temp1061)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL1076: _temp1075=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp1061)->f1; goto _LL1070;} else{ goto _LL1062;}
_LL1064:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1077=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1077[ 0]=({ struct Cyc_Core_Impossible_struct _temp1078; _temp1078.tag= Cyc_Core_Impossible_tag;
_temp1078.f1=( struct _tagged_string)({ char* _temp1079=( char*)"tcXTuniondecl: Loc_n";
struct _tagged_string _temp1080; _temp1080.curr= _temp1079; _temp1080.base=
_temp1079; _temp1080.last_plus_one= _temp1079 + 21; _temp1080;}); _temp1078;});
_temp1077;})); _LL1066:(* xd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp1081=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp1081[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp1082; _temp1082.tag= Cyc_Absyn_Abs_n_tag;
_temp1082.f1= te->ns; _temp1082;}); _temp1081;}); goto _LL1062; _LL1068:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp1083=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1083[ 0]=({ struct Cyc_Core_Impossible_struct _temp1084; _temp1084.tag= Cyc_Core_Impossible_tag;
_temp1084.f1=( struct _tagged_string)({ char* _temp1085=( char*)"tcXTuniondecl: Rel_n non-null";
struct _tagged_string _temp1086; _temp1086.curr= _temp1085; _temp1086.base=
_temp1085; _temp1086.last_plus_one= _temp1085 + 30; _temp1086;}); _temp1084;});
_temp1083;})); _LL1070:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1087=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1087[ 0]=({ struct Cyc_Core_Impossible_struct _temp1088; _temp1088.tag= Cyc_Core_Impossible_tag;
_temp1088.f1=( struct _tagged_string)({ char* _temp1089=( char*)"tcXTuniondecl: Abs_n";
struct _tagged_string _temp1090; _temp1090.curr= _temp1089; _temp1090.base=
_temp1089; _temp1090.last_plus_one= _temp1089 + 21; _temp1090;}); _temp1088;});
_temp1087;})); _LL1062:;} if( xd2o == 0){ struct Cyc_Absyn_XTuniondecl** x=(
struct Cyc_Absyn_XTuniondecl**)({ struct _tuple8* _temp1091=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp1091->f1=({ struct Cyc_Absyn_XTuniondecl*
_temp1092=( struct Cyc_Absyn_XTuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl));
_temp1092->sc=( void*)(( void*) xd->sc); _temp1092->name= xd->name; _temp1092->fields=
0; _temp1092;}); _temp1091;}); xd2o=({ struct Cyc_Core_Opt* _temp1093=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1093->v=( void*) x;
_temp1093;}); ge->xtuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_XTuniondecl** data)) Cyc_Dict_insert)(
ge->xtuniondecls, v, x);} else{ struct Cyc_Absyn_XTuniondecl* x=*(( struct Cyc_Absyn_XTuniondecl**)({
struct Cyc_Core_Opt* _temp1094= xd2o; if( _temp1094 == 0){ _throw(
Null_Exception);} _temp1094->v;}));( void*)( x->sc=( void*)(( void*(*)( void* s1,
struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt* fields2, struct Cyc_Position_Segment*
loc, struct _tagged_string t, struct _tagged_string* v)) Cyc_Tc_scope_redecl_okay)((
void*) xd->sc,({ struct Cyc_Core_Opt* _temp1095=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1095->v=( void*) 0;
_temp1095;}),( void*) x->sc,({ struct Cyc_Core_Opt* _temp1096=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1096->v=( void*) 0;
_temp1096;}), loc,( struct _tagged_string)({ char* _temp1097=( char*)"xtunion";
struct _tagged_string _temp1098; _temp1098.curr= _temp1097; _temp1098.base=
_temp1097; _temp1098.last_plus_one= _temp1097 + 8; _temp1098;}), v));}{ struct
Cyc_Absyn_XTuniondecl* xd_result=*(( struct Cyc_Absyn_XTuniondecl**)({ struct
Cyc_Core_Opt* _temp1099= xd2o; if( _temp1099 == 0){ _throw( Null_Exception);}
_temp1099->v;})); struct Cyc_List_List* fs= xd->fields; for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp1100= fs; if( _temp1100 == 0){ _throw( Null_Exception);}
_temp1100->tl;})){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1101= fs; if( _temp1101 == 0){ _throw( Null_Exception);}
_temp1101->hd;});{ void* _temp1102=(* f->name).f1; struct Cyc_List_List*
_temp1112; struct Cyc_List_List* _temp1114; struct Cyc_List_List* _temp1116;
_LL1104: if(( unsigned int) _temp1102 > 1u?(( struct _tunion_struct*) _temp1102)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL1113: _temp1112=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp1102)->f1; if( _temp1112 == 0){ goto _LL1105;}
else{ goto _LL1106;}} else{ goto _LL1106;} _LL1106: if(( unsigned int) _temp1102
> 1u?(( struct _tunion_struct*) _temp1102)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL1115: _temp1114=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1102)->f1; goto _LL1107;} else{ goto _LL1108;} _LL1108: if(( unsigned int)
_temp1102 > 1u?(( struct _tunion_struct*) _temp1102)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL1117: _temp1116=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1102)->f1; goto _LL1109;} else{ goto _LL1110;} _LL1110: if(( int) _temp1102
== Cyc_Absyn_Loc_n){ goto _LL1111;} else{ goto _LL1103;} _LL1105:(* f->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1118=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1118[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1119; _temp1119.tag= Cyc_Absyn_Abs_n_tag; _temp1119.f1= te->ns; _temp1119;});
_temp1118;}); goto _LL1103; _LL1107: goto _LL1109; _LL1109: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp1120=( char*)"qualified declarations are not allowed\n";
struct _tagged_string _temp1121; _temp1121.curr= _temp1120; _temp1121.base=
_temp1120; _temp1121.last_plus_one= _temp1120 + 40; _temp1121;})); goto _LL1103;
_LL1111:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1122=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1122[ 0]=({ struct Cyc_Core_Impossible_struct _temp1123; _temp1123.tag= Cyc_Core_Impossible_tag;
_temp1123.f1=( struct _tagged_string)({ char* _temp1124=( char*)"tcXtuniondecl: Loc_n";
struct _tagged_string _temp1125; _temp1125.curr= _temp1124; _temp1125.base=
_temp1124; _temp1125.last_plus_one= _temp1124 + 21; _temp1125;}); _temp1123;});
_temp1122;})); goto _LL1103; _LL1103:;}{ struct Cyc_List_List* tvs= f->tvs; for(
0; tvs != 0; tvs=({ struct Cyc_List_List* _temp1126= tvs; if( _temp1126 == 0){
_throw( Null_Exception);} _temp1126->tl;})){ struct Cyc_Absyn_Tvar* tv=( struct
Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1127= tvs; if( _temp1127 == 0){
_throw( Null_Exception);} _temp1127->hd;}); struct Cyc_Absyn_Conref* c=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind); void* _temp1128=( void*) c->v; void* _temp1136; _LL1130: if(( int)
_temp1128 == Cyc_Absyn_No_constr){ goto _LL1131;} else{ goto _LL1132;} _LL1132:
if(( unsigned int) _temp1128 > 1u?(( struct _tunion_struct*) _temp1128)->tag ==
Cyc_Absyn_Eq_constr_tag: 0){ _LL1137: _temp1136=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1128)->f1; if(( int) _temp1136 == Cyc_Absyn_MemKind){ goto _LL1133;} else{
goto _LL1134;}} else{ goto _LL1134;} _LL1134: goto _LL1135; _LL1131:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp1138=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1138[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1139; _temp1139.tag= Cyc_Absyn_Eq_constr_tag;
_temp1139.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp1139;}); _temp1138;})));
goto _LL1129; _LL1133: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1140=*(*
f->name).f2; xprintf("field %.*s abstracts type variable of kind M", _temp1140.last_plus_one
- _temp1140.curr, _temp1140.curr);})); goto _LL1129; _LL1135: goto _LL1129;
_LL1129:;}}{ struct Cyc_List_List* tvs= f->tvs; Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=({
struct Cyc_List_List* _temp1141= typs; if( _temp1141 == 0){ _throw(
Null_Exception);} _temp1141->tl;})){ void* t=(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp1142= typs; if( _temp1142 == 0){ _throw( Null_Exception);} _temp1142->hd;}))).f2;
Cyc_Tcutil_check_type( f->loc, te, tvs,( void*) Cyc_Absyn_MemKind, t);}}{ int
dup= 0;{ struct Cyc_List_List* prev_fs= xd_result->fields; for( 0; prev_fs != 0;
prev_fs=({ struct Cyc_List_List* _temp1143= prev_fs; if( _temp1143 == 0){ _throw(
Null_Exception);} _temp1143->tl;})){ struct Cyc_Absyn_Tunionfield* prev=( struct
Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1144= prev_fs; if(
_temp1144 == 0){ _throw( Null_Exception);} _temp1144->hd;}); if( Cyc_Absyn_qvar_cmp(
f->name, prev->name) == 0){ Cyc_Tc_field_redecl_okay( prev, f, 0, te,( struct
_tagged_string)({ char* _temp1145=( char*)"xtunion"; struct _tagged_string
_temp1146; _temp1146.curr= _temp1145; _temp1146.base= _temp1145; _temp1146.last_plus_one=
_temp1145 + 8; _temp1146;}), v); dup= 1; break;}}} if( dup){ continue;}
xd_result->fields=({ struct Cyc_List_List* _temp1147=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1147->hd=( void*) f; _temp1147->tl=
xd_result->fields; _temp1147;});{ struct Cyc_Tcenv_Genv* ge_f; struct Cyc_List_List*
ns_f;{ void* _temp1148=(* f->name).f1; struct Cyc_List_List* _temp1156; struct
Cyc_List_List* _temp1158; _LL1150: if(( unsigned int) _temp1148 > 1u?(( struct
_tunion_struct*) _temp1148)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1157: _temp1156=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1148)->f1; goto
_LL1151;} else{ goto _LL1152;} _LL1152: if(( int) _temp1148 == Cyc_Absyn_Loc_n){
goto _LL1153;} else{ goto _LL1154;} _LL1154: if(( unsigned int) _temp1148 > 1u?((
struct _tunion_struct*) _temp1148)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1159:
_temp1158=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1148)->f1;
goto _LL1155;} else{ goto _LL1149;} _LL1151: ns_f= _temp1156; goto _LL1149;
_LL1153: goto _LL1155; _LL1155:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp1160=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1160[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1161; _temp1161.tag= Cyc_Core_Impossible_tag;
_temp1161.f1=( struct _tagged_string)({ char* _temp1162=( char*)"tcXTuniondecl: Rel_n or Loc_n";
struct _tagged_string _temp1163; _temp1163.curr= _temp1162; _temp1163.base=
_temp1162; _temp1163.last_plus_one= _temp1162 + 30; _temp1163;}); _temp1161;});
_temp1160;})); goto _LL1149; _LL1149:;}{ struct _handler_cons _temp1164;
_push_handler(& _temp1164);{ struct _xtunion_struct* _temp1165=( struct
_xtunion_struct*) setjmp( _temp1164.handler); if( ! _temp1165){ ge_f=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae, ns_f);; _pop_handler();} else{ struct _xtunion_struct* _temp1167=
_temp1165; _LL1169: if( _temp1167->tag == Cyc_Dict_Absent_tag){ goto _LL1170;}
else{ goto _LL1171;} _LL1171: goto _LL1172; _LL1170: Cyc_Tcutil_terr( f->loc,({
struct _tagged_string _temp1173= Cyc_Absynpp_qvar2string( f->name); xprintf("bad namespace for xtunion field %.*s",
_temp1173.last_plus_one - _temp1173.curr, _temp1173.curr);})); return; _LL1172:(
void) _throw( _temp1167); _LL1168:;}}} ge_f->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge_f->ordinaries,(* f->name).f2,({ struct _tuple6* _temp1174=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp1174->f1=( void*)({ struct Cyc_Tcenv_XTunionRes_struct*
_temp1175=( struct Cyc_Tcenv_XTunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_XTunionRes_struct));
_temp1175[ 0]=({ struct Cyc_Tcenv_XTunionRes_struct _temp1176; _temp1176.tag=
Cyc_Tcenv_XTunionRes_tag; _temp1176.f1= xd_result; _temp1176.f2= f; _temp1176;});
_temp1175;}); _temp1174->f2= 1; _temp1174;}));}}}}}} static void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl* ed){ struct _tagged_string* v=(* ed->name).f2;{
void* _temp1177=(* ed->name).f1; struct Cyc_List_List* _temp1185; struct Cyc_List_List*
_temp1187; _LL1179: if(( unsigned int) _temp1177 > 1u?(( struct _tunion_struct*)
_temp1177)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1186: _temp1185=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp1177)->f1; if( _temp1185 == 0){ goto
_LL1180;} else{ goto _LL1181;}} else{ goto _LL1181;} _LL1181: if(( unsigned int)
_temp1177 > 1u?(( struct _tunion_struct*) _temp1177)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL1188: _temp1187=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1177)->f1; if( _temp1187 == 0){ goto _LL1182;} else{ goto _LL1183;}} else{
goto _LL1183;} _LL1183: goto _LL1184; _LL1180: goto _LL1178; _LL1182: goto
_LL1178; _LL1184: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1189=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp1190; _temp1190.curr= _temp1189; _temp1190.base= _temp1189;
_temp1190.last_plus_one= _temp1189 + 43; _temp1190;})); return; _LL1178:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1191=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1191[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1192; _temp1192.tag= Cyc_Absyn_Abs_n_tag; _temp1192.f1= te->ns; _temp1192;});
_temp1191;});{ struct Cyc_Core_Opt* _temp1193=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->enumdecls,
v); struct Cyc_Core_Opt _temp1199; struct Cyc_Absyn_Enumdecl** _temp1200;
_LL1195: if( _temp1193 == 0){ goto _LL1196;} else{ goto _LL1197;} _LL1197: if(
_temp1193 == 0){ goto _LL1194;} else{ _temp1199=* _temp1193; _LL1201: _temp1200=(
struct Cyc_Absyn_Enumdecl**) _temp1199.v; goto _LL1198;} _LL1196: ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls, v,({ struct
Cyc_Absyn_Enumdecl** _temp1202=( struct Cyc_Absyn_Enumdecl**) GC_malloc( sizeof(
struct Cyc_Absyn_Enumdecl*)); _temp1202[ 0]= ed; _temp1202;})); goto _LL1194;
_LL1198: { struct Cyc_Absyn_Enumdecl* ed2=* _temp1200;( void*)( ed->sc=( void*)((
void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, 0,( void*) ed2->sc,
0, loc,( struct _tagged_string)({ char* _temp1203=( char*)"enum"; struct
_tagged_string _temp1204; _temp1204.curr= _temp1203; _temp1204.base= _temp1203;
_temp1204.last_plus_one= _temp1203 + 5; _temp1204;}), v));{ struct Cyc_List_List*
f1s= ed->fields; struct Cyc_List_List* f2s= ed2->fields; unsigned int tag_count=
0; for( 0; f1s != 0? f2s != 0: 0; f1s=({ struct Cyc_List_List* _temp1205= f1s;
if( _temp1205 == 0){ _throw( Null_Exception);} _temp1205->tl;}), f2s=({ struct
Cyc_List_List* _temp1206= f2s; if( _temp1206 == 0){ _throw( Null_Exception);}
_temp1206->tl;})){ struct Cyc_Absyn_Enumfield _temp1210; struct Cyc_Position_Segment*
_temp1211; struct Cyc_Absyn_Exp* _temp1213; struct Cyc_Absyn_Exp** _temp1215;
struct _tuple0* _temp1216; struct Cyc_Absyn_Enumfield* _temp1208=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp1207= f1s; if( _temp1207 == 0){ _throw(
Null_Exception);} _temp1207->hd;}); _temp1210=* _temp1208; _LL1217: _temp1216=(
struct _tuple0*) _temp1210.name; goto _LL1214; _LL1214: _temp1213=( struct Cyc_Absyn_Exp*)
_temp1210.tag; _temp1215=&(* _temp1208).tag; goto _LL1212; _LL1212: _temp1211=(
struct Cyc_Position_Segment*) _temp1210.loc; goto _LL1209; _LL1209: { struct Cyc_Absyn_Enumfield
_temp1221; struct Cyc_Position_Segment* _temp1222; struct Cyc_Absyn_Exp*
_temp1224; struct _tuple0* _temp1226; struct Cyc_Absyn_Enumfield* _temp1219=(
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp1218= f2s; if(
_temp1218 == 0){ _throw( Null_Exception);} _temp1218->hd;}); _temp1221=*
_temp1219; _LL1227: _temp1226=( struct _tuple0*) _temp1221.name; goto _LL1225;
_LL1225: _temp1224=( struct Cyc_Absyn_Exp*) _temp1221.tag; goto _LL1223; _LL1223:
_temp1222=( struct Cyc_Position_Segment*) _temp1221.loc; goto _LL1220; _LL1220:(*
_temp1216).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1228=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1228[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1229; _temp1229.tag= Cyc_Absyn_Abs_n_tag; _temp1229.f1= te->ns; _temp1229;});
_temp1228;}); if( Cyc_String_zstrptrcmp((* _temp1216).f2,(* _temp1226).f2) != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1230=* v; struct
_tagged_string _temp1231=*(* _temp1216).f2; struct _tagged_string _temp1232=*(*
_temp1226).f2; xprintf("redeclaration of enum %.*s: field name mismatch %.*s != %.*s",
_temp1230.last_plus_one - _temp1230.curr, _temp1230.curr, _temp1231.last_plus_one
- _temp1231.curr, _temp1231.curr, _temp1232.last_plus_one - _temp1232.curr,
_temp1232.curr);}));} if(* _temp1215 == 0){* _temp1215=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( tag_count, _temp1211);} else{ if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1233=* _temp1215; if(
_temp1233 == 0){ _throw( Null_Exception);} _temp1233;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1234=* v; struct _tagged_string _temp1235=*(*
_temp1216).f2; xprintf("redeclaration of enum %.*s, field %.*s: expression is not constant",
_temp1234.last_plus_one - _temp1234.curr, _temp1234.curr, _temp1235.last_plus_one
- _temp1235.curr, _temp1235.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1236=* _temp1215; if(
_temp1236 == 0){ _throw( Null_Exception);} _temp1236;})); tag_count= t1 + 1; if(
t1 != Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1237= _temp1224; if( _temp1237 == 0){ _throw( Null_Exception);} _temp1237;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1238=* v; struct
_tagged_string _temp1239=*(* _temp1216).f2; xprintf("redeclaration of enum %.*s, field %.*s, value mismatch",
_temp1238.last_plus_one - _temp1238.curr, _temp1238.curr, _temp1239.last_plus_one
- _temp1239.curr, _temp1239.curr);}));}}}} return;}} _LL1194:;}{ struct Cyc_List_List*
prev_fields= 0; unsigned int tag_count= 0;{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1240= fs; if( _temp1240 == 0){
_throw( Null_Exception);} _temp1240->tl;})){ struct Cyc_Absyn_Enumfield* f=(
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp1241= fs; if(
_temp1241 == 0){ _throw( Null_Exception);} _temp1241->hd;}); if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(*
f->name).f2)){ Cyc_Tcutil_terr( f->loc,({ struct _tagged_string _temp1242=*(* f->name).f2;
xprintf("duplicate field name %.*s", _temp1242.last_plus_one - _temp1242.curr,
_temp1242.curr);}));} prev_fields=({ struct Cyc_List_List* _temp1243=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1243->hd=( void*)(*
f->name).f2; _temp1243->tl= prev_fields; _temp1243;}); if( f->tag == 0){ f->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, f->loc);} else{ if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1244= f->tag; if(
_temp1244 == 0){ _throw( Null_Exception);} _temp1244;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1245=* v; struct _tagged_string _temp1246=*(* f->name).f2;
xprintf("enum %.*s, field %.*s: expression is not constant", _temp1245.last_plus_one
- _temp1245.curr, _temp1245.curr, _temp1246.last_plus_one - _temp1246.curr,
_temp1246.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1247= f->tag; if( _temp1247 == 0){
_throw( Null_Exception);} _temp1247;})); tag_count= t1 + 1;(* f->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp1248=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1248[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1249; _temp1249.tag= Cyc_Absyn_Abs_n_tag; _temp1249.f1= te->ns; _temp1249;});
_temp1248;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,(*
f->name).f2,({ struct _tuple6* _temp1250=( struct _tuple6*) GC_malloc( sizeof(
struct _tuple6)); _temp1250->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp1251=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp1251[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp1252; _temp1252.tag= Cyc_Tcenv_EnumRes_tag;
_temp1252.f1= ed; _temp1252.f2= f; _temp1252;}); _temp1251;}); _temp1250->f2= 1;
_temp1250;}));}}} ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v,({ struct Cyc_Absyn_Enumdecl** _temp1253=( struct Cyc_Absyn_Enumdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*)); _temp1253[ 0]= ed; _temp1253;}));}}
static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){
void* _temp1254= sc; _LL1256: if(( int) _temp1254 == Cyc_Absyn_Static){ goto
_LL1257;} else{ goto _LL1258;} _LL1258: if(( int) _temp1254 == Cyc_Absyn_Abstract){
goto _LL1259;} else{ goto _LL1260;} _LL1260: if(( int) _temp1254 == Cyc_Absyn_Public){
goto _LL1261;} else{ goto _LL1262;} _LL1262: if(( int) _temp1254 == Cyc_Absyn_Extern){
goto _LL1263;} else{ goto _LL1264;} _LL1264: if(( int) _temp1254 == Cyc_Absyn_ExternC){
goto _LL1265;} else{ goto _LL1255;} _LL1257: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp1266=( char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp1267; _temp1267.curr= _temp1266; _temp1267.base=
_temp1266; _temp1267.last_plus_one= _temp1266 + 44; _temp1267;})); return 0;
_LL1259: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char* _temp1268=( char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp1269; _temp1269.curr= _temp1268; _temp1269.base=
_temp1268; _temp1269.last_plus_one= _temp1268 + 46; _temp1269;})); return 0;
_LL1261: return 1; _LL1263: return 1; _LL1265: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp1270=( char*)"nested extern \"C\" declaration";
struct _tagged_string _temp1271; _temp1271.curr= _temp1270; _temp1271.base=
_temp1270; _temp1271.last_plus_one= _temp1270 + 30; _temp1271;})); return 1;
_LL1255:;} static void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
ds0, int in_externC){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
te->ns); struct Cyc_List_List* ds= ds0; for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp1272= ds; if( _temp1272 == 0){ _throw( Null_Exception);} _temp1272->tl;})){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1273= ds; if( _temp1273 == 0){ _throw( Null_Exception);} _temp1273->hd;});
struct Cyc_Position_Segment* loc= d->loc; void* _temp1274=( void*) d->r; int
_temp1300; struct Cyc_Absyn_Exp* _temp1302; struct Cyc_Core_Opt* _temp1304;
struct Cyc_Core_Opt* _temp1306; struct Cyc_Absyn_Pat* _temp1308; struct Cyc_Absyn_Vardecl*
_temp1310; struct Cyc_Absyn_Fndecl* _temp1312; struct Cyc_Absyn_Typedefdecl*
_temp1314; struct Cyc_Absyn_Structdecl* _temp1316; struct Cyc_Absyn_Uniondecl*
_temp1318; struct Cyc_Absyn_Tuniondecl* _temp1320; struct Cyc_Absyn_XTuniondecl*
_temp1322; struct Cyc_Absyn_Enumdecl* _temp1324; struct Cyc_List_List* _temp1326;
struct _tagged_string* _temp1328; struct Cyc_List_List* _temp1330; struct
_tuple0* _temp1332; struct _tuple0 _temp1334; struct _tagged_string* _temp1335;
void* _temp1337; struct Cyc_List_List* _temp1339; _LL1276: if((( struct
_tunion_struct*) _temp1274)->tag == Cyc_Absyn_Let_d_tag){ _LL1309: _temp1308=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1274)->f1; goto
_LL1307; _LL1307: _temp1306=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1274)->f2; goto _LL1305; _LL1305: _temp1304=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1274)->f3; goto _LL1303; _LL1303: _temp1302=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1274)->f4; goto
_LL1301; _LL1301: _temp1300=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1274)->f5;
goto _LL1277;} else{ goto _LL1278;} _LL1278: if((( struct _tunion_struct*)
_temp1274)->tag == Cyc_Absyn_Var_d_tag){ _LL1311: _temp1310=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1274)->f1; goto _LL1279;} else{ goto
_LL1280;} _LL1280: if((( struct _tunion_struct*) _temp1274)->tag == Cyc_Absyn_Fn_d_tag){
_LL1313: _temp1312=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1274)->f1; goto _LL1281;} else{ goto _LL1282;} _LL1282: if((( struct
_tunion_struct*) _temp1274)->tag == Cyc_Absyn_Typedef_d_tag){ _LL1315: _temp1314=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp1274)->f1;
goto _LL1283;} else{ goto _LL1284;} _LL1284: if((( struct _tunion_struct*)
_temp1274)->tag == Cyc_Absyn_Struct_d_tag){ _LL1317: _temp1316=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1274)->f1; goto _LL1285;} else{ goto
_LL1286;} _LL1286: if((( struct _tunion_struct*) _temp1274)->tag == Cyc_Absyn_Union_d_tag){
_LL1319: _temp1318=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp1274)->f1; goto _LL1287;} else{ goto _LL1288;} _LL1288: if((( struct
_tunion_struct*) _temp1274)->tag == Cyc_Absyn_Tunion_d_tag){ _LL1321: _temp1320=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp1274)->f1;
goto _LL1289;} else{ goto _LL1290;} _LL1290: if((( struct _tunion_struct*)
_temp1274)->tag == Cyc_Absyn_XTunion_d_tag){ _LL1323: _temp1322=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp1274)->f1; goto _LL1291;} else{ goto
_LL1292;} _LL1292: if((( struct _tunion_struct*) _temp1274)->tag == Cyc_Absyn_Enum_d_tag){
_LL1325: _temp1324=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp1274)->f1; goto _LL1293;} else{ goto _LL1294;} _LL1294: if((( struct
_tunion_struct*) _temp1274)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1329:
_temp1328=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1274)->f1; goto _LL1327; _LL1327: _temp1326=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1274)->f2; goto _LL1295;} else{ goto
_LL1296;} _LL1296: if((( struct _tunion_struct*) _temp1274)->tag == Cyc_Absyn_Using_d_tag){
_LL1333: _temp1332=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1274)->f1; _temp1334=* _temp1332; _LL1338: _temp1337= _temp1334.f1; goto
_LL1336; _LL1336: _temp1335= _temp1334.f2; goto _LL1331; _LL1331: _temp1330=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp1274)->f2; goto
_LL1297;} else{ goto _LL1298;} _LL1298: if((( struct _tunion_struct*) _temp1274)->tag
== Cyc_Absyn_ExternC_d_tag){ _LL1340: _temp1339=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp1274)->f1; goto _LL1299;} else{ goto
_LL1275;} _LL1277: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1341=( char*)"top level let-declarations are not implemented"; struct
_tagged_string _temp1342; _temp1342.curr= _temp1341; _temp1342.base= _temp1341;
_temp1342.last_plus_one= _temp1341 + 47; _temp1342;})); goto _LL1275; _LL1279:
if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1310->sc): 0){( void*)(
_temp1310->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl( te, ge,
loc, _temp1310); goto _LL1275; _LL1281: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp1312->sc): 0){( void*)( _temp1312->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp1312); goto _LL1275; _LL1283: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp1314); goto _LL1275; _LL1285: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1316->sc): 0){( void*)( _temp1316->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp1316); goto _LL1275; _LL1287: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1318->sc): 0){( void*)(
_temp1318->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge,
loc, _temp1318); goto _LL1275; _LL1289: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp1320->sc): 0){( void*)( _temp1320->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp1320); goto _LL1275; _LL1291: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1322->sc): 0){( void*)(
_temp1322->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcXTuniondecl( te,
ge, loc, _temp1322); goto _LL1275; _LL1293: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1324->sc): 0){( void*)( _temp1324->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcEnumdecl( te, ge, loc, _temp1324); goto _LL1275; _LL1295: { struct Cyc_List_List*
ns= te->ns; struct Cyc_List_List* ns2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List*
_temp1343=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1343->hd=( void*) _temp1328; _temp1343->tl= 0; _temp1343;})); if( !(( int(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( ge->namespaces,
_temp1328)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp1328); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, ns2, Cyc_Tcenv_empty_genv());}
te->ns= ns2; Cyc_Tc_tc_decls( te, _temp1326, in_externC); te->ns= ns; goto
_LL1275;} _LL1297: { struct _tagged_string* first; struct Cyc_List_List* rest;{
void* _temp1344= _temp1337; struct Cyc_List_List* _temp1356; struct Cyc_List_List*
_temp1358; struct Cyc_List_List* _temp1360; struct Cyc_List_List _temp1362;
struct Cyc_List_List* _temp1363; struct _tagged_string* _temp1365; struct Cyc_List_List*
_temp1367; struct Cyc_List_List _temp1369; struct Cyc_List_List* _temp1370;
struct _tagged_string* _temp1372; _LL1346: if(( int) _temp1344 == Cyc_Absyn_Loc_n){
goto _LL1347;} else{ goto _LL1348;} _LL1348: if(( unsigned int) _temp1344 > 1u?((
struct _tunion_struct*) _temp1344)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1357:
_temp1356=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1344)->f1;
if( _temp1356 == 0){ goto _LL1349;} else{ goto _LL1350;}} else{ goto _LL1350;}
_LL1350: if(( unsigned int) _temp1344 > 1u?(( struct _tunion_struct*) _temp1344)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL1359: _temp1358=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp1344)->f1; if( _temp1358 == 0){ goto _LL1351;}
else{ goto _LL1352;}} else{ goto _LL1352;} _LL1352: if(( unsigned int) _temp1344
> 1u?(( struct _tunion_struct*) _temp1344)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL1361: _temp1360=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1344)->f1; if( _temp1360 == 0){ goto _LL1354;} else{ _temp1362=* _temp1360;
_LL1366: _temp1365=( struct _tagged_string*) _temp1362.hd; goto _LL1364; _LL1364:
_temp1363=( struct Cyc_List_List*) _temp1362.tl; goto _LL1353;}} else{ goto
_LL1354;} _LL1354: if(( unsigned int) _temp1344 > 1u?(( struct _tunion_struct*)
_temp1344)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1368: _temp1367=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp1344)->f1; if( _temp1367 == 0){ goto
_LL1345;} else{ _temp1369=* _temp1367; _LL1373: _temp1372=( struct
_tagged_string*) _temp1369.hd; goto _LL1371; _LL1371: _temp1370=( struct Cyc_List_List*)
_temp1369.tl; goto _LL1355;}} else{ goto _LL1345;} _LL1347: goto _LL1349;
_LL1349: goto _LL1351; _LL1351: first= _temp1335; rest= 0; goto _LL1345; _LL1353:
_temp1372= _temp1365; _temp1370= _temp1363; goto _LL1355; _LL1355: first=
_temp1372; rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp1370,({ struct Cyc_List_List* _temp1374=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1374->hd=( void*) _temp1335;
_temp1374->tl= 0; _temp1374;})); goto _LL1345; _LL1345:;}{ struct Cyc_List_List*
ns2= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp1375=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1375->hd=( void*) ns2; _temp1375->tl= ge->availables;
_temp1375;}); Cyc_Tc_tc_decls( te, _temp1330, in_externC); ge->availables=({
struct Cyc_List_List* _temp1376= ge->availables; if( _temp1376 == 0){ _throw(
Null_Exception);} _temp1376->tl;}); goto _LL1275;}} _LL1299: Cyc_Tc_tc_decls( te,
_temp1339, 1); goto _LL1275; _LL1275:;}} static char _temp1379[ 4u]="Cyc";
static struct _tagged_string Cyc_Tc_cyc_string=( struct _tagged_string){
_temp1379, _temp1379, _temp1379 + 4u}; static struct _tagged_string* Cyc_Tc_cyc_ns=&
Cyc_Tc_cyc_string; void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
add_cyc_namespace, struct Cyc_List_List* ds){ if( add_cyc_namespace){ ds=({
struct Cyc_List_List* _temp1380=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1380->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1381=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1381->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1382=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1382[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1383; _temp1383.tag=
Cyc_Absyn_Namespace_d_tag; _temp1383.f1= Cyc_Tc_cyc_ns; _temp1383.f2= ds;
_temp1383;}); _temp1382;})); _temp1381->loc= 0; _temp1381;}); _temp1380->tl= 0;
_temp1380;});} Cyc_Tc_tc_decls( te, ds, 0);} static int Cyc_Tc_vardecl_needed(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Decl* d){ void* _temp1384=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp1394; struct Cyc_List_List* _temp1396; struct Cyc_List_List**
_temp1398; struct _tuple0* _temp1399; struct Cyc_List_List* _temp1401; struct
Cyc_List_List** _temp1403; struct _tagged_string* _temp1404; _LL1386: if(((
struct _tunion_struct*) _temp1384)->tag == Cyc_Absyn_Var_d_tag){ _LL1395:
_temp1394=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1384)->f1; goto _LL1387;} else{ goto _LL1388;} _LL1388: if((( struct
_tunion_struct*) _temp1384)->tag == Cyc_Absyn_Using_d_tag){ _LL1400: _temp1399=(
struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1384)->f1; goto
_LL1397; _LL1397: _temp1396=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1384)->f2; _temp1398=&(( struct Cyc_Absyn_Using_d_struct*) _temp1384)->f2;
goto _LL1389;} else{ goto _LL1390;} _LL1390: if((( struct _tunion_struct*)
_temp1384)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1405: _temp1404=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1384)->f1; goto
_LL1402; _LL1402: _temp1401=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1384)->f2; _temp1403=&(( struct Cyc_Absyn_Namespace_d_struct*) _temp1384)->f2;
goto _LL1391;} else{ goto _LL1392;} _LL1392: goto _LL1393; _LL1387: if(( void*)
_temp1394->sc !=( void*) Cyc_Absyn_Extern){ return 1;}{ struct _tuple0 _temp1408;
struct _tagged_string* _temp1409; void* _temp1411; struct _tuple0* _temp1406=
_temp1394->name; _temp1408=* _temp1406; _LL1412: _temp1411= _temp1408.f1; goto
_LL1410; _LL1410: _temp1409= _temp1408.f2; goto _LL1407; _LL1407: { struct Cyc_List_List*
ns;{ void* _temp1413= _temp1411; struct Cyc_List_List* _temp1421; struct Cyc_List_List*
_temp1423; _LL1415: if(( int) _temp1413 == Cyc_Absyn_Loc_n){ goto _LL1416;}
else{ goto _LL1417;} _LL1417: if(( unsigned int) _temp1413 > 1u?(( struct
_tunion_struct*) _temp1413)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1422: _temp1421=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1413)->f1; goto
_LL1418;} else{ goto _LL1419;} _LL1419: if(( unsigned int) _temp1413 > 1u?((
struct _tunion_struct*) _temp1413)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1424:
_temp1423=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1413)->f1;
goto _LL1420;} else{ goto _LL1414;} _LL1416: ns= 0; goto _LL1414; _LL1418: ns=
_temp1421; goto _LL1414; _LL1420: ns= _temp1423; goto _LL1414; _LL1414:;}{
struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, ns); return(*(( struct
_tuple6*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
ge->ordinaries, _temp1409)).f2;}}} _LL1389: _temp1403= _temp1398; goto _LL1391;
_LL1391:* _temp1403= Cyc_Tc_treeshake( te,* _temp1403); return 1; _LL1393:
return 1; _LL1385:;} struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)( int(* f)(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Decl*), struct Cyc_Tcenv_Tenv* env,
struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, te, ds);}