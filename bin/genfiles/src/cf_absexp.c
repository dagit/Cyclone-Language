#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ void*
f1; void* f2; } ; struct _tuple4{ void* f1; void* f2; void* f3; } ; struct
_tuple5{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; typedef int Cyc_ptrdiff_t;
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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string(
int); extern int Cyc_Core_ptrcmp( void**, void**); extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern char
Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char* tag; } ; extern int Cyc_List_memq(
struct Cyc_List_List* l, void* x); struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[
8u]; struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; struct Cyc_Dict_Absent_struct{ char* tag; } ; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict*
d, void* key, void** ans_place); extern void Cyc_Dict_iter2_c( void(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col; } ; typedef
struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[ 5u];
struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
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
char* tag; } ; typedef void* Cyc_Tcenv_Resolved; static const int Cyc_Tcenv_VarRes_tag=
0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes_tag=
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
tag; } ; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); typedef void* Cyc_CfFlowInfo_LocalRoot; typedef void* Cyc_CfFlowInfo_Field;
struct Cyc_CfFlowInfo_Place; typedef void* Cyc_CfFlowInfo_Escaped; typedef void*
Cyc_CfFlowInfo_InitLevel; typedef void* Cyc_CfFlowInfo_InitState; typedef void*
Cyc_CfFlowInfo_PathInfo; typedef void* Cyc_CfFlowInfo_FlowInfo; typedef void*
Cyc_CfFlowInfo_local_root_t; typedef void* Cyc_CfFlowInfo_field_t; typedef
struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t; typedef void* Cyc_CfFlowInfo_escaped_t;
typedef void* Cyc_CfFlowInfo_init_level_t; typedef void* Cyc_CfFlowInfo_init_state_t;
typedef void* Cyc_CfFlowInfo_path_info_t; typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t;
typedef void* Cyc_CfFlowInfo_flow_info_t; static const int Cyc_CfFlowInfo_VarRoot_tag=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt_tag= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF_tag=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1; } ;
static const int Cyc_CfFlowInfo_TupleF_tag= 1; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; static const unsigned int Cyc_CfFlowInfo_Esc= 0; static const
unsigned int Cyc_CfFlowInfo_Unesc= 1; static const unsigned int Cyc_CfFlowInfo_NoneIL=
0; static const unsigned int Cyc_CfFlowInfo_ThisIL= 1; static const unsigned int
Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS_tag= 0;
struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ; static
const int Cyc_CfFlowInfo_MustPointTo_tag= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI_tag=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI_tag= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag;
struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI_tag= 2;
struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const unsigned int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL_tag=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void* Cyc_CfFlowInfo_mkLeafPI( void* esc, void* il); extern void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place, void* pinfo); extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto(
void* pinfo, struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict(
void*, void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int
Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); extern char Cyc_CfAbsexp_BadAbsexp_tag[ 10u]; struct
Cyc_CfAbsexp_BadAbsexp_struct{ char* tag; } ; typedef void* Cyc_CfAbsexp_Absexp;
typedef void* Cyc_CfAbsexp_Absexpgroup; static const unsigned int Cyc_CfAbsexp_OrderedG=
0; static const unsigned int Cyc_CfAbsexp_UnorderedG= 1; static const
unsigned int Cyc_CfAbsexp_OneofG= 2; typedef void* Cyc_CfAbsexp_Absop; typedef
void* Cyc_CfAbsexp_absexp_t; typedef void* Cyc_CfAbsexp_absexpgroup_t; typedef
void* Cyc_CfAbsexp_absop_t; extern void Cyc_CfAbsexp_check_absexp( void*);
extern struct _tagged_string Cyc_CfAbsexp_absexp2string( void* ae, int depth);
extern void* Cyc_CfAbsexp_mkAnyOrderG(); extern void* Cyc_CfAbsexp_mkUnknownOp();
extern void* Cyc_CfAbsexp_mkAddressOp( void* ao); extern void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd); extern void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f); extern void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e); extern
void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern void* Cyc_CfAbsexp_mkBottomAE();
extern void* Cyc_CfAbsexp_mkSkipAE(); extern void* Cyc_CfAbsexp_mkUseAE( void*
ao); extern void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern void* Cyc_CfAbsexp_eval_absexp( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void* ae,
void* in_flow); char Cyc_CfAbsexp_BadAbsexp_tag[ 10u]="BadAbsexp"; typedef
struct Cyc_Dict_Dict* Cyc_CfAbsexp_dict_t; typedef struct Cyc_Position_Segment*
Cyc_CfAbsexp_seg_t; void* Cyc_CfAbsexp_mkAnyOrderG(){ return( void*) Cyc_CfAbsexp_OrderedG;}
static const unsigned int Cyc_CfAbsexp_BottomAE= 0; static const unsigned int
Cyc_CfAbsexp_SkipAE= 1; static const int Cyc_CfAbsexp_UseAE_tag= 0; struct Cyc_CfAbsexp_UseAE_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_AssignAE_tag= 1; struct Cyc_CfAbsexp_AssignAE_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_MallocAE_tag= 2;
struct Cyc_CfAbsexp_MallocAE_struct{ int tag; void* f1; int f2; } ; static const
int Cyc_CfAbsexp_StmtAE_tag= 3; struct Cyc_CfAbsexp_StmtAE_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_CfAbsexp_GroupAE_tag= 4;
struct Cyc_CfAbsexp_GroupAE_struct{ int tag; void* f1; struct Cyc_List_List* f2;
} ; static const unsigned int Cyc_CfAbsexp_UnknownOp= 0; static const int Cyc_CfAbsexp_AddressOp_tag=
0; struct Cyc_CfAbsexp_AddressOp_struct{ int tag; void* f1; } ; static const int
Cyc_CfAbsexp_DerefOp_tag= 1; struct Cyc_CfAbsexp_DerefOp_struct{ int tag; void*
f1; } ; static const int Cyc_CfAbsexp_MemberOp_tag= 2; struct Cyc_CfAbsexp_MemberOp_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_LocalOp_tag= 3;
struct Cyc_CfAbsexp_LocalOp_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_CfAbsexp_MallocOp_tag= 4; struct Cyc_CfAbsexp_MallocOp_struct{
int tag; struct Cyc_Absyn_Exp* f1; int f2; } ; static void Cyc_CfAbsexp_ok_address_arg(
void* ao){ void* _temp0= ao; struct Cyc_Absyn_Vardecl* _temp10; void* _temp12;
void* _temp14; int _temp16; struct Cyc_Absyn_Exp* _temp18; _LL2: if((
unsigned int) _temp0 > 1u?(( struct _tunion_struct*) _temp0)->tag == Cyc_CfAbsexp_LocalOp_tag:
0){ _LL11: _temp10=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: if(( unsigned int) _temp0 > 1u?((
struct _tunion_struct*) _temp0)->tag == Cyc_CfAbsexp_MemberOp_tag: 0){ _LL15:
_temp14=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp0)->f1; goto _LL13;
_LL13: _temp12=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp0)->f2;
goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 1u?(( struct
_tunion_struct*) _temp0)->tag == Cyc_CfAbsexp_MallocOp_tag: 0){ _LL19: _temp18=(
struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp0)->f1; goto
_LL17; _LL17: _temp16=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp0)->f2;
goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9; _LL3: return; _LL5: return; _LL7:
return; _LL9:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_CfAbsexp_BadAbsexp_struct*
_temp20=( struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));
_temp20[ 0]=({ struct Cyc_CfAbsexp_BadAbsexp_struct _temp21; _temp21.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp21;}); _temp20;})); _LL1:;} static void Cyc_CfAbsexp_ok_member_arg( void*
ao){ void* _temp22= ao; struct Cyc_Absyn_Vardecl* _temp34; void* _temp36; void*
_temp38; int _temp40; struct Cyc_Absyn_Exp* _temp42; void* _temp44; _LL24: if((
unsigned int) _temp22 > 1u?(( struct _tunion_struct*) _temp22)->tag == Cyc_CfAbsexp_LocalOp_tag:
0){ _LL35: _temp34=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp22)->f1; goto _LL25;} else{ goto _LL26;} _LL26: if(( unsigned int) _temp22
> 1u?(( struct _tunion_struct*) _temp22)->tag == Cyc_CfAbsexp_MemberOp_tag: 0){
_LL39: _temp38=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp22)->f1;
goto _LL37; _LL37: _temp36=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp22)->f2; goto _LL27;} else{ goto _LL28;} _LL28: if(( unsigned int) _temp22
> 1u?(( struct _tunion_struct*) _temp22)->tag == Cyc_CfAbsexp_MallocOp_tag: 0){
_LL43: _temp42=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp22)->f1; goto _LL41; _LL41: _temp40=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp22)->f2; goto _LL29;} else{ goto _LL30;} _LL30: if(( unsigned int) _temp22
> 1u?(( struct _tunion_struct*) _temp22)->tag == Cyc_CfAbsexp_DerefOp_tag: 0){
_LL45: _temp44=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp22)->f1;
goto _LL31;} else{ goto _LL32;} _LL32: goto _LL33; _LL25: return; _LL27: return;
_LL29: return; _LL31: return; _LL33:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_CfAbsexp_BadAbsexp_struct* _temp46=( struct Cyc_CfAbsexp_BadAbsexp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct)); _temp46[ 0]=({ struct
Cyc_CfAbsexp_BadAbsexp_struct _temp47; _temp47.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp47;}); _temp46;})); _LL23:;} static void Cyc_CfAbsexp_ok_deref_arg( void*
ao){ void* _temp48= ao; struct Cyc_Absyn_Vardecl* _temp58; void* _temp60; void*
_temp62; void* _temp64; _LL50: if(( unsigned int) _temp48 > 1u?(( struct
_tunion_struct*) _temp48)->tag == Cyc_CfAbsexp_LocalOp_tag: 0){ _LL59: _temp58=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp48)->f1;
goto _LL51;} else{ goto _LL52;} _LL52: if(( unsigned int) _temp48 > 1u?(( struct
_tunion_struct*) _temp48)->tag == Cyc_CfAbsexp_MemberOp_tag: 0){ _LL63: _temp62=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp48)->f1; goto _LL61; _LL61:
_temp60=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp48)->f2; goto
_LL53;} else{ goto _LL54;} _LL54: if(( unsigned int) _temp48 > 1u?(( struct
_tunion_struct*) _temp48)->tag == Cyc_CfAbsexp_DerefOp_tag: 0){ _LL65: _temp64=(
void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp48)->f1; goto _LL55;} else{
goto _LL56;} _LL56: goto _LL57; _LL51: return; _LL53: return; _LL55: return;
_LL57:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_CfAbsexp_BadAbsexp_struct*
_temp66=( struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));
_temp66[ 0]=({ struct Cyc_CfAbsexp_BadAbsexp_struct _temp67; _temp67.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp67;}); _temp66;})); _LL49:;} static void Cyc_CfAbsexp_ok_lvalue( void* ao){
void* _temp68= ao; void* _temp76; _LL70: if(( int) _temp68 == Cyc_CfAbsexp_UnknownOp){
goto _LL71;} else{ goto _LL72;} _LL72: if(( unsigned int) _temp68 > 1u?(( struct
_tunion_struct*) _temp68)->tag == Cyc_CfAbsexp_AddressOp_tag: 0){ _LL77: _temp76=(
void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp68)->f1; goto _LL73;} else{
goto _LL74;} _LL74: goto _LL75; _LL71: goto _LL73; _LL73:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_CfAbsexp_BadAbsexp_struct* _temp78=( struct Cyc_CfAbsexp_BadAbsexp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct)); _temp78[ 0]=({ struct
Cyc_CfAbsexp_BadAbsexp_struct _temp79; _temp79.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp79;}); _temp78;})); _LL75: return; _LL69:;} static int Cyc_CfAbsexp_num_mallocpts=
0; static struct Cyc_Dict_Dict** Cyc_CfAbsexp_mallocpt_dict= 0; int Cyc_CfAbsexp_mallocpt_int(
struct Cyc_Absyn_Exp* e){ if( Cyc_CfAbsexp_mallocpt_dict == 0){ struct Cyc_Dict_Dict*
d=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)))
Cyc_Dict_empty)(( int(*)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp);
Cyc_CfAbsexp_mallocpt_dict=({ struct Cyc_Dict_Dict** _temp80=( struct Cyc_Dict_Dict**)
GC_malloc( sizeof( struct Cyc_Dict_Dict*) * 1); _temp80[ 0]= d; _temp80;});}{
int i= 0; if( !(( int(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp* key,
int* ans_place)) Cyc_Dict_lookup_bool)(* Cyc_CfAbsexp_mallocpt_dict, e,& i)){*
Cyc_CfAbsexp_mallocpt_dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_Absyn_Exp* key, int data)) Cyc_Dict_insert)(* Cyc_CfAbsexp_mallocpt_dict,
e,( i=( ++ Cyc_CfAbsexp_num_mallocpts)));} return i;}} void* Cyc_CfAbsexp_mkUnknownOp(){
return( void*) Cyc_CfAbsexp_UnknownOp;} void* Cyc_CfAbsexp_mkAddressOp( void* ao){
Cyc_CfAbsexp_ok_address_arg( ao); return( void*)({ struct Cyc_CfAbsexp_AddressOp_struct*
_temp81=( struct Cyc_CfAbsexp_AddressOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_AddressOp_struct));
_temp81[ 0]=({ struct Cyc_CfAbsexp_AddressOp_struct _temp82; _temp82.tag= Cyc_CfAbsexp_AddressOp_tag;
_temp82.f1=( void*) ao; _temp82;}); _temp81;});} void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd){ return( void*)({ struct Cyc_CfAbsexp_LocalOp_struct*
_temp83=( struct Cyc_CfAbsexp_LocalOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_LocalOp_struct));
_temp83[ 0]=({ struct Cyc_CfAbsexp_LocalOp_struct _temp84; _temp84.tag= Cyc_CfAbsexp_LocalOp_tag;
_temp84.f1= vd; _temp84;}); _temp83;});} void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f){ Cyc_CfAbsexp_ok_member_arg( ao); return( void*)({ struct Cyc_CfAbsexp_MemberOp_struct*
_temp85=( struct Cyc_CfAbsexp_MemberOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MemberOp_struct));
_temp85[ 0]=({ struct Cyc_CfAbsexp_MemberOp_struct _temp86; _temp86.tag= Cyc_CfAbsexp_MemberOp_tag;
_temp86.f1=( void*) ao; _temp86.f2=( void*) f; _temp86;}); _temp85;});} void*
Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e){ return( void*)({ struct Cyc_CfAbsexp_MallocOp_struct*
_temp87=( struct Cyc_CfAbsexp_MallocOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocOp_struct));
_temp87[ 0]=({ struct Cyc_CfAbsexp_MallocOp_struct _temp88; _temp88.tag= Cyc_CfAbsexp_MallocOp_tag;
_temp88.f1= e; _temp88.f2= Cyc_CfAbsexp_mallocpt_int( e); _temp88;}); _temp87;});}
void* Cyc_CfAbsexp_mkDerefOp( void* ao){ Cyc_CfAbsexp_ok_deref_arg( ao); return(
void*)({ struct Cyc_CfAbsexp_DerefOp_struct* _temp89=( struct Cyc_CfAbsexp_DerefOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_DerefOp_struct)); _temp89[ 0]=({ struct
Cyc_CfAbsexp_DerefOp_struct _temp90; _temp90.tag= Cyc_CfAbsexp_DerefOp_tag;
_temp90.f1=( void*) ao; _temp90;}); _temp89;});} void* Cyc_CfAbsexp_mkBottomAE(){
return( void*) Cyc_CfAbsexp_BottomAE;} void* Cyc_CfAbsexp_mkSkipAE(){ return(
void*) Cyc_CfAbsexp_SkipAE;} void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp*
e){ return( void*)({ struct Cyc_CfAbsexp_MallocAE_struct* _temp91=( struct Cyc_CfAbsexp_MallocAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocAE_struct)); _temp91[ 0]=({ struct
Cyc_CfAbsexp_MallocAE_struct _temp92; _temp92.tag= Cyc_CfAbsexp_MallocAE_tag;
_temp92.f1=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp93=(
struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp93[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp94; _temp94.tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp94.f1= e; _temp94;}); _temp93;})); _temp92.f2= Cyc_CfAbsexp_mallocpt_int( e);
_temp92;}); _temp91;});} void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s){
return( void*)({ struct Cyc_CfAbsexp_StmtAE_struct* _temp95=( struct Cyc_CfAbsexp_StmtAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_StmtAE_struct)); _temp95[ 0]=({ struct
Cyc_CfAbsexp_StmtAE_struct _temp96; _temp96.tag= Cyc_CfAbsexp_StmtAE_tag;
_temp96.f1= s; _temp96;}); _temp95;});} void* Cyc_CfAbsexp_mkUseAE( void* ao){
void* _temp97= ao; _LL99: if(( int) _temp97 == Cyc_CfAbsexp_UnknownOp){ goto
_LL100;} else{ goto _LL101;} _LL101: goto _LL102; _LL100: return( void*) Cyc_CfAbsexp_SkipAE;
_LL102: return( void*)({ struct Cyc_CfAbsexp_UseAE_struct* _temp103=( struct Cyc_CfAbsexp_UseAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_UseAE_struct)); _temp103[ 0]=({ struct
Cyc_CfAbsexp_UseAE_struct _temp104; _temp104.tag= Cyc_CfAbsexp_UseAE_tag;
_temp104.f1=( void*) ao; _temp104;}); _temp103;}); _LL98:;} void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r){ struct _tuple3 _temp106=({ struct _tuple3 _temp105; _temp105.f1=
l; _temp105.f2= r; _temp105;}); void* _temp114; void* _temp116; void* _temp118;
void* _temp120; _LL108: _LL117: _temp116= _temp106.f1; if(( int) _temp116 == Cyc_CfAbsexp_UnknownOp){
goto _LL115;} else{ goto _LL110;} _LL115: _temp114= _temp106.f2; if(( int)
_temp114 == Cyc_CfAbsexp_UnknownOp){ goto _LL109;} else{ goto _LL110;} _LL110:
_LL121: _temp120= _temp106.f1; if(( int) _temp120 == Cyc_CfAbsexp_UnknownOp){
goto _LL119;} else{ goto _LL112;} _LL119: _temp118= _temp106.f2; goto _LL111;
_LL112: goto _LL113; _LL109: return( void*) Cyc_CfAbsexp_SkipAE; _LL111: return
Cyc_CfAbsexp_mkUseAE( r); _LL113: Cyc_CfAbsexp_ok_lvalue( l); return( void*)({
struct Cyc_CfAbsexp_AssignAE_struct* _temp122=( struct Cyc_CfAbsexp_AssignAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AssignAE_struct)); _temp122[ 0]=({ struct
Cyc_CfAbsexp_AssignAE_struct _temp123; _temp123.tag= Cyc_CfAbsexp_AssignAE_tag;
_temp123.f1=( void*) l; _temp123.f2=( void*) r; _temp123;}); _temp122;}); _LL107:;}
void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2){ struct _tuple4
_temp125=({ struct _tuple4 _temp124; _temp124.f1= g; _temp124.f2= ae1; _temp124.f3=
ae2; _temp124;}); void* _temp148; void* _temp150; void* _temp152; void* _temp155;
void* _temp157; void* _temp159; void* _temp161; void* _temp163; void* _temp165;
void* _temp167; void* _temp169; void* _temp171; void* _temp173; void* _temp175;
void* _temp177; void* _temp180; struct Cyc_List_List* _temp182; void* _temp184;
void* _temp186; struct Cyc_List_List* _temp188; void* _temp190; void* _temp192;
void* _temp195; void* _temp197; struct Cyc_List_List* _temp199; void* _temp201;
void* _temp203; void* _temp205; void* _temp207; struct Cyc_List_List* _temp209;
void* _temp211; void* _temp213; void* _temp216; struct Cyc_List_List* _temp218;
void* _temp220; void* _temp222; void* _temp224; _LL127: _LL153: _temp152=
_temp125.f1; goto _LL151; _LL151: _temp150= _temp125.f2; goto _LL149; _LL149:
_temp148= _temp125.f3; if(( int) _temp148 == Cyc_CfAbsexp_SkipAE){ goto _LL147;}
else{ goto _LL129;} _LL147: if( g !=( void*) Cyc_CfAbsexp_OneofG){ goto _LL128;}
else{ goto _LL129;} _LL129: _LL160: _temp159= _temp125.f1; goto _LL158; _LL158:
_temp157= _temp125.f2; if(( int) _temp157 == Cyc_CfAbsexp_SkipAE){ goto _LL156;}
else{ goto _LL131;} _LL156: _temp155= _temp125.f3; goto _LL154; _LL154: if( g !=(
void*) Cyc_CfAbsexp_OneofG){ goto _LL130;} else{ goto _LL131;} _LL131: _LL166:
_temp165= _temp125.f1; if(( int) _temp165 == Cyc_CfAbsexp_OneofG){ goto _LL164;}
else{ goto _LL133;} _LL164: _temp163= _temp125.f2; goto _LL162; _LL162: _temp161=
_temp125.f3; if(( int) _temp161 == Cyc_CfAbsexp_BottomAE){ goto _LL132;} else{
goto _LL133;} _LL133: _LL172: _temp171= _temp125.f1; if(( int) _temp171 == Cyc_CfAbsexp_OneofG){
goto _LL170;} else{ goto _LL135;} _LL170: _temp169= _temp125.f2; if(( int)
_temp169 == Cyc_CfAbsexp_BottomAE){ goto _LL168;} else{ goto _LL135;} _LL168:
_temp167= _temp125.f3; goto _LL134; _LL135: _LL178: _temp177= _temp125.f1; if((
int) _temp177 == Cyc_CfAbsexp_OrderedG){ goto _LL176;} else{ goto _LL137;}
_LL176: _temp175= _temp125.f2; if(( int) _temp175 == Cyc_CfAbsexp_BottomAE){
goto _LL174;} else{ goto _LL137;} _LL174: _temp173= _temp125.f3; goto _LL136;
_LL137: _LL193: _temp192= _temp125.f1; goto _LL187; _LL187: _temp186= _temp125.f2;
if(( unsigned int) _temp186 > 2u?(( struct _tunion_struct*) _temp186)->tag ==
Cyc_CfAbsexp_GroupAE_tag: 0){ _LL191: _temp190=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp186)->f1; goto _LL189; _LL189: _temp188=( struct Cyc_List_List*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp186)->f2; goto _LL181;} else{ goto _LL139;}
_LL181: _temp180= _temp125.f3; if(( unsigned int) _temp180 > 2u?(( struct
_tunion_struct*) _temp180)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL185:
_temp184=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp180)->f1; goto
_LL183; _LL183: _temp182=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp180)->f2; goto _LL179;} else{ goto _LL139;} _LL179: if( _temp192 ==
_temp190? _temp192 == _temp184: 0){ goto _LL138;} else{ goto _LL139;} _LL139:
_LL204: _temp203= _temp125.f1; goto _LL198; _LL198: _temp197= _temp125.f2; if((
unsigned int) _temp197 > 2u?(( struct _tunion_struct*) _temp197)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL202: _temp201=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp197)->f1;
goto _LL200; _LL200: _temp199=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp197)->f2; goto _LL196;} else{ goto _LL141;} _LL196: _temp195= _temp125.f3;
goto _LL194; _LL194: if( _temp203 == _temp201? _temp203 !=( void*) Cyc_CfAbsexp_OrderedG:
0){ goto _LL140;} else{ goto _LL141;} _LL141: _LL214: _temp213= _temp125.f1; if((
int) _temp213 == Cyc_CfAbsexp_OrderedG){ goto _LL208;} else{ goto _LL143;}
_LL208: _temp207= _temp125.f2; if(( unsigned int) _temp207 > 2u?(( struct
_tunion_struct*) _temp207)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL212:
_temp211=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp207)->f1; if(( int)
_temp211 == Cyc_CfAbsexp_OrderedG){ goto _LL210;} else{ goto _LL143;} _LL210:
_temp209=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp207)->f2; goto _LL206;} else{ goto _LL143;} _LL206: _temp205= _temp125.f3;
goto _LL142; _LL143: _LL225: _temp224= _temp125.f1; goto _LL223; _LL223:
_temp222= _temp125.f2; goto _LL217; _LL217: _temp216= _temp125.f3; if((
unsigned int) _temp216 > 2u?(( struct _tunion_struct*) _temp216)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL221: _temp220=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp216)->f1;
goto _LL219; _LL219: _temp218=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp216)->f2; goto _LL215;} else{ goto _LL145;} _LL215: if( _temp224 ==
_temp220){ goto _LL144;} else{ goto _LL145;} _LL145: goto _LL146; _LL128: return
ae1; _LL130: return ae2; _LL132: return ae1; _LL134: return ae2; _LL136: return(
void*) Cyc_CfAbsexp_BottomAE; _LL138: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp226=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp226[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp227; _temp227.tag= Cyc_CfAbsexp_GroupAE_tag;
_temp227.f1=( void*) _temp192; _temp227.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp188, _temp182);
_temp227;}); _temp226;}); _LL140: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp228=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp228[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp229; _temp229.tag= Cyc_CfAbsexp_GroupAE_tag;
_temp229.f1=( void*) _temp203; _temp229.f2=({ struct Cyc_List_List* _temp230=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp230[
0]=({ struct Cyc_List_List _temp231; _temp231.hd=( void*) ae2; _temp231.tl=
_temp199; _temp231;}); _temp230;}); _temp229;}); _temp228;}); _LL142: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp232=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp232[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp233; _temp233.tag= Cyc_CfAbsexp_GroupAE_tag;
_temp233.f1=( void*)(( void*) Cyc_CfAbsexp_OrderedG); _temp233.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp209,({
struct Cyc_List_List* _temp234=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp234[ 0]=({ struct Cyc_List_List _temp235;
_temp235.hd=( void*) ae2; _temp235.tl= 0; _temp235;}); _temp234;})); _temp233;});
_temp232;}); _LL144: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp236=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp236[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp237; _temp237.tag= Cyc_CfAbsexp_GroupAE_tag;
_temp237.f1=( void*) _temp224; _temp237.f2=({ struct Cyc_List_List* _temp238=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp238[
0]=({ struct Cyc_List_List _temp239; _temp239.hd=( void*) ae1; _temp239.tl=
_temp218; _temp239;}); _temp238;}); _temp237;}); _temp236;}); _LL146: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp240=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp240[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp241; _temp241.tag= Cyc_CfAbsexp_GroupAE_tag;
_temp241.f1=( void*) g; _temp241.f2=({ struct Cyc_List_List* _temp242=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp242[ 0]=({
struct Cyc_List_List _temp243; _temp243.hd=( void*) ae1; _temp243.tl=({ struct
Cyc_List_List* _temp244=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp244[ 0]=({ struct Cyc_List_List _temp245; _temp245.hd=( void*) ae2;
_temp245.tl= 0; _temp245;}); _temp244;}); _temp243;}); _temp242;}); _temp241;});
_temp240;}); _LL126:;} void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List*
ael){ if( ael == 0){ return( void*) Cyc_CfAbsexp_SkipAE;} if( ael->tl == 0){
return( void*) ael->hd;} return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp246=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp246[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp247; _temp247.tag= Cyc_CfAbsexp_GroupAE_tag;
_temp247.f1=( void*) g; _temp247.f2= ael; _temp247;}); _temp246;});} int Cyc_CfAbsexp_isUnknownOp(
void* ao){ return ao ==( void*) Cyc_CfAbsexp_UnknownOp;} struct _tagged_string
Cyc_CfAbsexp_absop2string( void* ao){ void* _temp248= ao; void* _temp262; struct
Cyc_Absyn_Vardecl* _temp264; void* _temp266; void* _temp268; int _temp270;
struct Cyc_Absyn_Exp* _temp272; void* _temp274; _LL250: if(( int) _temp248 ==
Cyc_CfAbsexp_UnknownOp){ goto _LL251;} else{ goto _LL252;} _LL252: if((
unsigned int) _temp248 > 1u?(( struct _tunion_struct*) _temp248)->tag == Cyc_CfAbsexp_AddressOp_tag:
0){ _LL263: _temp262=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp248)->f1;
goto _LL253;} else{ goto _LL254;} _LL254: if(( unsigned int) _temp248 > 1u?((
struct _tunion_struct*) _temp248)->tag == Cyc_CfAbsexp_LocalOp_tag: 0){ _LL265:
_temp264=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp248)->f1; goto _LL255;} else{ goto _LL256;} _LL256: if(( unsigned int)
_temp248 > 1u?(( struct _tunion_struct*) _temp248)->tag == Cyc_CfAbsexp_MemberOp_tag:
0){ _LL269: _temp268=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp248)->f1;
goto _LL267; _LL267: _temp266=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp248)->f2; goto _LL257;} else{ goto _LL258;} _LL258: if(( unsigned int)
_temp248 > 1u?(( struct _tunion_struct*) _temp248)->tag == Cyc_CfAbsexp_MallocOp_tag:
0){ _LL273: _temp272=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp248)->f1; goto _LL271; _LL271: _temp270=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp248)->f2; goto _LL259;} else{ goto _LL260;} _LL260: if(( unsigned int)
_temp248 > 1u?(( struct _tunion_struct*) _temp248)->tag == Cyc_CfAbsexp_DerefOp_tag:
0){ _LL275: _temp274=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp248)->f1;
goto _LL261;} else{ goto _LL249;} _LL251: return( struct _tagged_string)({ char*
_temp276=( char*)"Unknown"; struct _tagged_string _temp277; _temp277.curr=
_temp276; _temp277.base= _temp276; _temp277.last_plus_one= _temp276 + 8;
_temp277;}); _LL253: return({ struct _tagged_string _temp278= Cyc_CfAbsexp_absop2string(
_temp262); xprintf("& %.*s", _temp278.last_plus_one - _temp278.curr, _temp278.curr);});
_LL255: return*(* _temp264->name).f2; _LL257: { struct _tagged_string fs;{ void*
_temp279= _temp266; struct _tagged_string* _temp285; int _temp287; _LL281: if(((
struct _tunion_struct*) _temp279)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL286:
_temp285=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp279)->f1; goto _LL282;} else{ goto _LL283;} _LL283: if((( struct
_tunion_struct*) _temp279)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL288: _temp287=(
int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp279)->f1; goto _LL284;} else{
goto _LL280;} _LL282: fs=* _temp285; goto _LL280; _LL284: fs= xprintf("%d",
_temp287); goto _LL280; _LL280:;} return({ struct _tagged_string _temp289= Cyc_CfAbsexp_absop2string(
_temp268); struct _tagged_string _temp290= fs; xprintf("%.*s.%.*s", _temp289.last_plus_one
- _temp289.curr, _temp289.curr, _temp290.last_plus_one - _temp290.curr, _temp290.curr);});}
_LL259: return xprintf("<mpt%d>", _temp270); _LL261: return({ struct
_tagged_string _temp291= Cyc_CfAbsexp_absop2string( _temp274); xprintf("(*%.*s)",
_temp291.last_plus_one - _temp291.curr, _temp291.curr);}); _LL249:;} struct
_tagged_string Cyc_CfAbsexp_absexp2string( void* ae, int depth){ struct
_tagged_string ans= Cyc_Core_new_string( depth);{ int i= 0; for( 0;(
unsigned int) i <({ struct _tagged_string _temp292= ans;( unsigned int)(
_temp292.last_plus_one - _temp292.curr);}); ++ i){({ struct _tagged_string
_temp293= ans; char* _temp295= _temp293.curr + i; if( _temp295 < _temp293.base?
1: _temp295 >= _temp293.last_plus_one){ _throw( Null_Exception);}* _temp295=' ';});}}{
void* _temp296= ae; void* _temp314; int _temp316; void* _temp318; struct Cyc_Absyn_Exp*
_temp320; void* _temp322; void* _temp324; struct Cyc_List_List* _temp326; void*
_temp328; struct Cyc_Absyn_Stmt* _temp330; _LL298: if(( int) _temp296 == Cyc_CfAbsexp_SkipAE){
goto _LL299;} else{ goto _LL300;} _LL300: if(( unsigned int) _temp296 > 2u?((
struct _tunion_struct*) _temp296)->tag == Cyc_CfAbsexp_UseAE_tag: 0){ _LL315:
_temp314=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp296)->f1; goto
_LL301;} else{ goto _LL302;} _LL302: if(( unsigned int) _temp296 > 2u?(( struct
_tunion_struct*) _temp296)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL319:
_temp318=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp296)->f1; if(((
struct _tunion_struct*) _temp318)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL321:
_temp320=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp318)->f1; goto _LL317;} else{ goto _LL304;} _LL317: _temp316=( int)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp296)->f2; goto _LL303;} else{ goto
_LL304;} _LL304: if(( unsigned int) _temp296 > 2u?(( struct _tunion_struct*)
_temp296)->tag == Cyc_CfAbsexp_AssignAE_tag: 0){ _LL325: _temp324=( void*)((
struct Cyc_CfAbsexp_AssignAE_struct*) _temp296)->f1; goto _LL323; _LL323:
_temp322=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp296)->f2; goto
_LL305;} else{ goto _LL306;} _LL306: if(( unsigned int) _temp296 > 2u?(( struct
_tunion_struct*) _temp296)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL329:
_temp328=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp296)->f1; goto
_LL327; _LL327: _temp326=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp296)->f2; goto _LL307;} else{ goto _LL308;} _LL308: if(( unsigned int)
_temp296 > 2u?(( struct _tunion_struct*) _temp296)->tag == Cyc_CfAbsexp_StmtAE_tag:
0){ _LL331: _temp330=( struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp296)->f1; goto _LL309;} else{ goto _LL310;} _LL310: if(( int) _temp296 ==
Cyc_CfAbsexp_BottomAE){ goto _LL311;} else{ goto _LL312;} _LL312: goto _LL313;
_LL299: ans=({ struct _tagged_string _temp332= ans; xprintf("%.*sskip", _temp332.last_plus_one
- _temp332.curr, _temp332.curr);}); goto _LL297; _LL301: ans=({ struct
_tagged_string _temp333= ans; struct _tagged_string _temp334= Cyc_CfAbsexp_absop2string(
_temp314); xprintf("%.*suse %.*s", _temp333.last_plus_one - _temp333.curr,
_temp333.curr, _temp334.last_plus_one - _temp334.curr, _temp334.curr);}); goto
_LL297; _LL303: ans=({ struct _tagged_string _temp335= ans; int _temp336=
_temp316; xprintf("%.*smalloc %i", _temp335.last_plus_one - _temp335.curr,
_temp335.curr, _temp336);}); goto _LL297; _LL305: ans=({ struct _tagged_string
_temp337= ans; struct _tagged_string _temp338= Cyc_CfAbsexp_absop2string(
_temp324); struct _tagged_string _temp339= Cyc_CfAbsexp_absop2string( _temp322);
xprintf("%.*sassign %.*s %.*s", _temp337.last_plus_one - _temp337.curr, _temp337.curr,
_temp338.last_plus_one - _temp338.curr, _temp338.curr, _temp339.last_plus_one -
_temp339.curr, _temp339.curr);}); goto _LL297; _LL307:{ void* _temp340= _temp328;
_LL342: if(( int) _temp340 == Cyc_CfAbsexp_OrderedG){ goto _LL343;} else{ goto
_LL344;} _LL344: if(( int) _temp340 == Cyc_CfAbsexp_UnorderedG){ goto _LL345;}
else{ goto _LL346;} _LL346: if(( int) _temp340 == Cyc_CfAbsexp_OneofG){ goto
_LL347;} else{ goto _LL341;} _LL343: ans=({ struct _tagged_string _temp348= ans;
xprintf("%.*sordered", _temp348.last_plus_one - _temp348.curr, _temp348.curr);});
goto _LL341; _LL345: ans=({ struct _tagged_string _temp349= ans; xprintf("%.*sunordered",
_temp349.last_plus_one - _temp349.curr, _temp349.curr);}); goto _LL341; _LL347:
ans=({ struct _tagged_string _temp350= ans; xprintf("%.*soneof", _temp350.last_plus_one
- _temp350.curr, _temp350.curr);}); goto _LL341; _LL341:;} for( 0; _temp326 != 0;
_temp326= _temp326->tl){ ans=({ struct _tagged_string _temp351= ans; struct
_tagged_string _temp352= Cyc_CfAbsexp_absexp2string(( void*) _temp326->hd, depth
+ 1); xprintf("%.*s\n%.*s", _temp351.last_plus_one - _temp351.curr, _temp351.curr,
_temp352.last_plus_one - _temp352.curr, _temp352.curr);});} goto _LL297; _LL309:
ans=({ struct _tagged_string _temp353= ans; xprintf("%.*s<<stmt>>", _temp353.last_plus_one
- _temp353.curr, _temp353.curr);}); goto _LL297; _LL311: ans=({ struct
_tagged_string _temp354= ans; xprintf("%.*sbottom", _temp354.last_plus_one -
_temp354.curr, _temp354.curr);}); goto _LL297; _LL313:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_CfAbsexp_BadAbsexp_struct* _temp355=( struct Cyc_CfAbsexp_BadAbsexp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct)); _temp355[ 0]=({
struct Cyc_CfAbsexp_BadAbsexp_struct _temp356; _temp356.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp356;}); _temp355;})); _LL297:;} return ans;} static void Cyc_CfAbsexp_check_absop(
void* ao){ void* _temp357= ao; void* _temp371; void* _temp373; void* _temp375;
void* _temp377; struct Cyc_Absyn_Vardecl* _temp379; int _temp381; struct Cyc_Absyn_Exp*
_temp383; _LL359: if(( int) _temp357 == Cyc_CfAbsexp_UnknownOp){ goto _LL360;}
else{ goto _LL361;} _LL361: if(( unsigned int) _temp357 > 1u?(( struct
_tunion_struct*) _temp357)->tag == Cyc_CfAbsexp_AddressOp_tag: 0){ _LL372:
_temp371=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp357)->f1; goto
_LL362;} else{ goto _LL363;} _LL363: if(( unsigned int) _temp357 > 1u?(( struct
_tunion_struct*) _temp357)->tag == Cyc_CfAbsexp_DerefOp_tag: 0){ _LL374:
_temp373=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp357)->f1; goto
_LL364;} else{ goto _LL365;} _LL365: if(( unsigned int) _temp357 > 1u?(( struct
_tunion_struct*) _temp357)->tag == Cyc_CfAbsexp_MemberOp_tag: 0){ _LL378:
_temp377=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp357)->f1; goto
_LL376; _LL376: _temp375=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp357)->f2; goto _LL366;} else{ goto _LL367;} _LL367: if(( unsigned int)
_temp357 > 1u?(( struct _tunion_struct*) _temp357)->tag == Cyc_CfAbsexp_LocalOp_tag:
0){ _LL380: _temp379=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp357)->f1; goto _LL368;} else{ goto _LL369;} _LL369: if(( unsigned int)
_temp357 > 1u?(( struct _tunion_struct*) _temp357)->tag == Cyc_CfAbsexp_MallocOp_tag:
0){ _LL384: _temp383=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp357)->f1; goto _LL382; _LL382: _temp381=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp357)->f2; goto _LL370;} else{ goto _LL358;} _LL360: return; _LL362: Cyc_CfAbsexp_check_absop(
_temp371); Cyc_CfAbsexp_ok_address_arg( _temp371); return; _LL364: Cyc_CfAbsexp_check_absop(
_temp373); Cyc_CfAbsexp_ok_deref_arg( _temp373); return; _LL366: Cyc_CfAbsexp_check_absop(
_temp377); Cyc_CfAbsexp_ok_member_arg( _temp377); return; _LL368: return; _LL370:
return; _LL358:;} void Cyc_CfAbsexp_check_absexp( void* ae){ void* _temp385= ae;
int _temp403; void* _temp405; struct Cyc_Absyn_Vardecl* _temp407; int _temp409;
void* _temp411; struct Cyc_Absyn_Stmt* _temp413; struct Cyc_List_List* _temp415;
void* _temp417; void* _temp419; void* _temp421; void* _temp423; _LL387: if(( int)
_temp385 == Cyc_CfAbsexp_BottomAE){ goto _LL388;} else{ goto _LL389;} _LL389:
if(( int) _temp385 == Cyc_CfAbsexp_SkipAE){ goto _LL390;} else{ goto _LL391;}
_LL391: if(( unsigned int) _temp385 > 2u?(( struct _tunion_struct*) _temp385)->tag
== Cyc_CfAbsexp_MallocAE_tag: 0){ _LL406: _temp405=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp385)->f1; if((( struct _tunion_struct*) _temp405)->tag == Cyc_CfFlowInfo_VarRoot_tag){
_LL408: _temp407=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp405)->f1; goto _LL404;} else{ goto _LL393;} _LL404: _temp403=( int)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp385)->f2; goto _LL392;} else{ goto
_LL393;} _LL393: if(( unsigned int) _temp385 > 2u?(( struct _tunion_struct*)
_temp385)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL412: _temp411=( void*)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp385)->f1; goto _LL410; _LL410:
_temp409=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp385)->f2; goto
_LL394;} else{ goto _LL395;} _LL395: if(( unsigned int) _temp385 > 2u?(( struct
_tunion_struct*) _temp385)->tag == Cyc_CfAbsexp_StmtAE_tag: 0){ _LL414: _temp413=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp385)->f1;
goto _LL396;} else{ goto _LL397;} _LL397: if(( unsigned int) _temp385 > 2u?((
struct _tunion_struct*) _temp385)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL418:
_temp417=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp385)->f1; goto
_LL416; _LL416: _temp415=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp385)->f2; goto _LL398;} else{ goto _LL399;} _LL399: if(( unsigned int)
_temp385 > 2u?(( struct _tunion_struct*) _temp385)->tag == Cyc_CfAbsexp_UseAE_tag:
0){ _LL420: _temp419=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp385)->f1;
goto _LL400;} else{ goto _LL401;} _LL401: if(( unsigned int) _temp385 > 2u?((
struct _tunion_struct*) _temp385)->tag == Cyc_CfAbsexp_AssignAE_tag: 0){ _LL424:
_temp423=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp385)->f1; goto
_LL422; _LL422: _temp421=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp385)->f2; goto _LL402;} else{ goto _LL386;} _LL388: return; _LL390: return;
_LL392:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_CfAbsexp_BadAbsexp_struct*
_temp425=( struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));
_temp425[ 0]=({ struct Cyc_CfAbsexp_BadAbsexp_struct _temp426; _temp426.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp426;}); _temp425;})); _LL394: return; _LL396: return; _LL398: for( 0;
_temp415 != 0; _temp415= _temp415->tl){ Cyc_CfAbsexp_check_absexp(( void*)
_temp415->hd);} return; _LL400: Cyc_CfAbsexp_check_absop( _temp419); return;
_LL402: Cyc_CfAbsexp_check_absop( _temp423); Cyc_CfAbsexp_check_absop( _temp421);
Cyc_CfAbsexp_ok_lvalue( _temp423); return; _LL386:;} void* Cyc_CfAbsexp_eval_absop_r(
struct Cyc_Dict_Dict* pinfo_dict, void* ao); void* Cyc_CfAbsexp_eval_absop_l(
struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void* _temp427= ao; struct Cyc_Absyn_Vardecl*
_temp441; int _temp443; struct Cyc_Absyn_Exp* _temp445; void* _temp447; void*
_temp449; void* _temp451; void* _temp453; _LL429: if(( unsigned int) _temp427 >
1u?(( struct _tunion_struct*) _temp427)->tag == Cyc_CfAbsexp_LocalOp_tag: 0){
_LL442: _temp441=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp427)->f1; goto _LL430;} else{ goto _LL431;} _LL431: if(( unsigned int)
_temp427 > 1u?(( struct _tunion_struct*) _temp427)->tag == Cyc_CfAbsexp_MallocOp_tag:
0){ _LL446: _temp445=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp427)->f1; goto _LL444; _LL444: _temp443=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp427)->f2; goto _LL432;} else{ goto _LL433;} _LL433: if(( unsigned int)
_temp427 > 1u?(( struct _tunion_struct*) _temp427)->tag == Cyc_CfAbsexp_MemberOp_tag:
0){ _LL450: _temp449=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp427)->f1;
goto _LL448; _LL448: _temp447=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp427)->f2; goto _LL434;} else{ goto _LL435;} _LL435: if(( unsigned int)
_temp427 > 1u?(( struct _tunion_struct*) _temp427)->tag == Cyc_CfAbsexp_DerefOp_tag:
0){ _LL452: _temp451=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp427)->f1;
goto _LL436;} else{ goto _LL437;} _LL437: if(( int) _temp427 == Cyc_CfAbsexp_UnknownOp){
goto _LL438;} else{ goto _LL439;} _LL439: if(( unsigned int) _temp427 > 1u?((
struct _tunion_struct*) _temp427)->tag == Cyc_CfAbsexp_AddressOp_tag: 0){ _LL454:
_temp453=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp427)->f1; goto
_LL440;} else{ goto _LL428;} _LL430: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp455=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp455[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp456; _temp456.tag= Cyc_CfFlowInfo_MustPointTo_tag; _temp456.f1=({ struct
Cyc_CfFlowInfo_Place* _temp457=( struct Cyc_CfFlowInfo_Place*) GC_malloc(
sizeof( struct Cyc_CfFlowInfo_Place) * 1); _temp457[ 0]=({ struct Cyc_CfFlowInfo_Place
_temp458; _temp458.root=( void*)(( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp459=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp459[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp460; _temp460.tag= Cyc_CfFlowInfo_VarRoot_tag;
_temp460.f1= _temp441; _temp460;}); _temp459;})); _temp458.fields= 0; _temp458;});
_temp457;}); _temp456;}); _temp455;}); _LL432: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp461=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp461[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp462; _temp462.tag= Cyc_CfFlowInfo_MustPointTo_tag; _temp462.f1=({ struct
Cyc_CfFlowInfo_Place* _temp463=( struct Cyc_CfFlowInfo_Place*) GC_malloc(
sizeof( struct Cyc_CfFlowInfo_Place) * 1); _temp463[ 0]=({ struct Cyc_CfFlowInfo_Place
_temp464; _temp464.root=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct*
_temp465=( struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp465[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp466; _temp466.tag=
Cyc_CfFlowInfo_MallocPt_tag; _temp466.f1= _temp445; _temp466;}); _temp465;}));
_temp464.fields= 0; _temp464;}); _temp463;}); _temp462;}); _temp461;}); _LL434: {
void* inner_state= Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp449); void*
_temp467= inner_state; void* _temp473; void* _temp475; struct Cyc_CfFlowInfo_Place*
_temp477; struct Cyc_CfFlowInfo_Place _temp479; struct Cyc_List_List* _temp480;
void* _temp482; _LL469: if((( struct _tunion_struct*) _temp467)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL476: _temp475=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp467)->f1;
goto _LL474; _LL474: _temp473=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp467)->f2; goto _LL470;} else{ goto _LL471;} _LL471: if((( struct
_tunion_struct*) _temp467)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL478:
_temp477=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp467)->f1; _temp479=* _temp477; _LL483: _temp482=( void*) _temp479.root;
goto _LL481; _LL481: _temp480=( struct Cyc_List_List*) _temp479.fields; goto
_LL472;} else{ goto _LL468;} _LL470: return inner_state; _LL472: return( void*)({
struct Cyc_CfFlowInfo_MustPointTo_struct* _temp484=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp484[ 0]=({
struct Cyc_CfFlowInfo_MustPointTo_struct _temp485; _temp485.tag= Cyc_CfFlowInfo_MustPointTo_tag;
_temp485.f1=({ struct Cyc_CfFlowInfo_Place* _temp486=( struct Cyc_CfFlowInfo_Place*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place) * 1); _temp486[ 0]=({ struct Cyc_CfFlowInfo_Place
_temp487; _temp487.root=( void*) _temp482; _temp487.fields=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp480,({
struct Cyc_List_List* _temp488=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp488[ 0]=({ struct Cyc_List_List _temp489;
_temp489.hd=( void*) _temp447; _temp489.tl= 0; _temp489;}); _temp488;}));
_temp487;}); _temp486;}); _temp485;}); _temp484;}); _LL468:;} _LL436: { void*
inner_pinfo= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp451); void* _temp490=
inner_pinfo; void* _temp498; struct Cyc_Dict_Dict* _temp500; struct Cyc_Dict_Dict*
_temp502; _LL492: if((( struct _tunion_struct*) _temp490)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL499: _temp498=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp490)->f1;
goto _LL493;} else{ goto _LL494;} _LL494: if((( struct _tunion_struct*) _temp490)->tag
== Cyc_CfFlowInfo_TuplePI_tag){ _LL501: _temp500=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp490)->f1; goto _LL495;} else{ goto
_LL496;} _LL496: if((( struct _tunion_struct*) _temp490)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL503: _temp502=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp490)->f1; goto _LL497;} else{ goto _LL491;} _LL493: return _temp498; _LL495:
goto _LL497; _LL497:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_CfAbsexp_BadAbsexp_struct*
_temp504=( struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));
_temp504[ 0]=({ struct Cyc_CfAbsexp_BadAbsexp_struct _temp505; _temp505.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp505;}); _temp504;})); _LL491:;} _LL438: goto _LL440; _LL440:( void) _throw((
struct _xtunion_struct*)({ struct Cyc_CfAbsexp_BadAbsexp_struct* _temp506=(
struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));
_temp506[ 0]=({ struct Cyc_CfAbsexp_BadAbsexp_struct _temp507; _temp507.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp507;}); _temp506;})); _LL428:;} void* Cyc_CfAbsexp_eval_absop_r( struct Cyc_Dict_Dict*
pinfo_dict, void* ao){ void* _temp508= ao; struct Cyc_Absyn_Vardecl* _temp522;
int _temp524; struct Cyc_Absyn_Exp* _temp526; void* _temp528; void* _temp530;
void* _temp532; void* _temp534; _LL510: if(( unsigned int) _temp508 > 1u?((
struct _tunion_struct*) _temp508)->tag == Cyc_CfAbsexp_LocalOp_tag: 0){ _LL523:
_temp522=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp508)->f1; goto _LL511;} else{ goto _LL512;} _LL512: if(( unsigned int)
_temp508 > 1u?(( struct _tunion_struct*) _temp508)->tag == Cyc_CfAbsexp_MallocOp_tag:
0){ _LL527: _temp526=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp508)->f1; goto _LL525; _LL525: _temp524=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp508)->f2; goto _LL513;} else{ goto _LL514;} _LL514: if(( unsigned int)
_temp508 > 1u?(( struct _tunion_struct*) _temp508)->tag == Cyc_CfAbsexp_MemberOp_tag:
0){ _LL531: _temp530=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp508)->f1;
goto _LL529; _LL529: _temp528=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp508)->f2; goto _LL515;} else{ goto _LL516;} _LL516: if(( unsigned int)
_temp508 > 1u?(( struct _tunion_struct*) _temp508)->tag == Cyc_CfAbsexp_DerefOp_tag:
0){ _LL533: _temp532=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp508)->f1;
goto _LL517;} else{ goto _LL518;} _LL518: if(( int) _temp508 == Cyc_CfAbsexp_UnknownOp){
goto _LL519;} else{ goto _LL520;} _LL520: if(( unsigned int) _temp508 > 1u?((
struct _tunion_struct*) _temp508)->tag == Cyc_CfAbsexp_AddressOp_tag: 0){ _LL535:
_temp534=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp508)->f1; goto
_LL521;} else{ goto _LL509;} _LL511: return(( void*(*)( struct Cyc_Dict_Dict* d,
void* key)) Cyc_Dict_lookup)( pinfo_dict,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp536=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp536[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp537; _temp537.tag= Cyc_CfFlowInfo_VarRoot_tag;
_temp537.f1= _temp522; _temp537;}); _temp536;})); _LL513: return(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( pinfo_dict,( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp538=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp538[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp539; _temp539.tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp539.f1= _temp526; _temp539;}); _temp538;})); _LL515: { void* inner_pinfo=
Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp530); struct _tuple3 _temp541=({
struct _tuple3 _temp540; _temp540.f1= inner_pinfo; _temp540.f2= _temp528;
_temp540;}); void* _temp551; void* _temp553; void* _temp555; void* _temp557;
void* _temp559; void* _temp561; int _temp563; void* _temp565; struct Cyc_Dict_Dict*
_temp567; void* _temp569; struct _tagged_string* _temp571; void* _temp573;
struct Cyc_Dict_Dict* _temp575; _LL543: _LL554: _temp553= _temp541.f1; if(((
struct _tunion_struct*) _temp553)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL556:
_temp555=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp553)->f1; if(((
struct _tunion_struct*) _temp555)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL560:
_temp559=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp555)->f1; goto
_LL558; _LL558: _temp557=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp555)->f2; goto _LL552;} else{ goto _LL545;}} else{ goto _LL545;} _LL552:
_temp551= _temp541.f2; goto _LL544; _LL545: _LL566: _temp565= _temp541.f1; if(((
struct _tunion_struct*) _temp565)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL568:
_temp567=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp565)->f1; goto _LL562;} else{ goto _LL547;} _LL562: _temp561= _temp541.f2;
if((( struct _tunion_struct*) _temp561)->tag == Cyc_CfFlowInfo_TupleF_tag){
_LL564: _temp563=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp561)->f1;
goto _LL546;} else{ goto _LL547;} _LL547: _LL574: _temp573= _temp541.f1; if(((
struct _tunion_struct*) _temp573)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL576:
_temp575=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp573)->f1; goto _LL570;} else{ goto _LL549;} _LL570: _temp569= _temp541.f2;
if((( struct _tunion_struct*) _temp569)->tag == Cyc_CfFlowInfo_StructF_tag){
_LL572: _temp571=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp569)->f1; goto _LL548;} else{ goto _LL549;} _LL549: goto _LL550; _LL544:
return inner_pinfo; _LL546: return(( void*(*)( struct Cyc_Dict_Dict* d, int key))
Cyc_Dict_lookup)( _temp567, _temp563); _LL548: return(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp575, _temp571); _LL550:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_CfAbsexp_BadAbsexp_struct*
_temp577=( struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));
_temp577[ 0]=({ struct Cyc_CfAbsexp_BadAbsexp_struct _temp578; _temp578.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp578;}); _temp577;})); _LL542:;} _LL517: { void* inner_pinfo= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp532); void* _temp579= inner_pinfo; void* _temp589; void*
_temp591; void* _temp593; void* _temp595; void* _temp597; void* _temp599; void*
_temp601; struct Cyc_CfFlowInfo_Place* _temp603; _LL581: if((( struct
_tunion_struct*) _temp579)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL590: _temp589=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp579)->f1; if((( struct
_tunion_struct*) _temp589)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL594:
_temp593=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp589)->f1; goto
_LL592; _LL592: _temp591=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp589)->f2; if(( int) _temp591 == Cyc_CfFlowInfo_ThisIL){ goto _LL582;} else{
goto _LL583;}} else{ goto _LL583;}} else{ goto _LL583;} _LL583: if((( struct
_tunion_struct*) _temp579)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL596: _temp595=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp579)->f1; if((( struct
_tunion_struct*) _temp595)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL600:
_temp599=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp595)->f1; goto
_LL598; _LL598: _temp597=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp595)->f2; goto _LL584;} else{ goto _LL585;}} else{ goto _LL585;} _LL585:
if((( struct _tunion_struct*) _temp579)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL602: _temp601=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp579)->f1;
if((( struct _tunion_struct*) _temp601)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL604: _temp603=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp601)->f1; goto _LL586;} else{ goto _LL587;}} else{ goto _LL587;} _LL587:
goto _LL588; _LL582: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_NoneIL); _LL584: return inner_pinfo; _LL586: return Cyc_CfFlowInfo_lookup_place(
pinfo_dict, _temp603); _LL588:( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_CfAbsexp_BadAbsexp_struct* _temp605=( struct Cyc_CfAbsexp_BadAbsexp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct)); _temp605[ 0]=({
struct Cyc_CfAbsexp_BadAbsexp_struct _temp606; _temp606.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp606;}); _temp605;})); _LL580:;} _LL519: return Cyc_CfFlowInfo_mkLeafPI((
void*) Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_AllIL); _LL521: return( void*)({
struct Cyc_CfFlowInfo_LeafPI_struct* _temp607=( struct Cyc_CfFlowInfo_LeafPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct)); _temp607[ 0]=({ struct
Cyc_CfFlowInfo_LeafPI_struct _temp608; _temp608.tag= Cyc_CfFlowInfo_LeafPI_tag;
_temp608.f1=( void*) Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp534); _temp608;});
_temp607;}); _LL509:;} void Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo){ struct _tuple3 _temp610=({
struct _tuple3 _temp609; _temp609.f1= old_pinfo; _temp609.f2= new_pinfo;
_temp609;}); void* _temp622; void* _temp624; void* _temp626; void* _temp628;
void* _temp630; void* _temp632; void* _temp634; void* _temp636; void* _temp638;
void* _temp640; void* _temp642; struct Cyc_Dict_Dict* _temp644; void* _temp646;
struct Cyc_Dict_Dict* _temp648; void* _temp650; struct Cyc_Dict_Dict* _temp652;
void* _temp654; struct Cyc_Dict_Dict* _temp656; _LL612: _LL627: _temp626=
_temp610.f1; if((( struct _tunion_struct*) _temp626)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL629: _temp628=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp626)->f1;
if((( struct _tunion_struct*) _temp628)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL633: _temp632=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp628)->f1;
if(( int) _temp632 == Cyc_CfFlowInfo_Esc){ goto _LL631;} else{ goto _LL614;}
_LL631: _temp630=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp628)->f2;
goto _LL623;} else{ goto _LL614;}} else{ goto _LL614;} _LL623: _temp622=
_temp610.f2; if((( struct _tunion_struct*) _temp622)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL625: _temp624=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp622)->f1;
goto _LL613;} else{ goto _LL614;} _LL614: _LL639: _temp638= _temp610.f1; if(((
struct _tunion_struct*) _temp638)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL641:
_temp640=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp638)->f1; goto
_LL635;} else{ goto _LL616;} _LL635: _temp634= _temp610.f2; if((( struct
_tunion_struct*) _temp634)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL637: _temp636=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp634)->f1; goto _LL615;}
else{ goto _LL616;} _LL616: _LL647: _temp646= _temp610.f1; if((( struct
_tunion_struct*) _temp646)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL649: _temp648=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp646)->f1;
goto _LL643;} else{ goto _LL618;} _LL643: _temp642= _temp610.f2; if((( struct
_tunion_struct*) _temp642)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL645: _temp644=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp642)->f1;
goto _LL617;} else{ goto _LL618;} _LL618: _LL655: _temp654= _temp610.f1; if(((
struct _tunion_struct*) _temp654)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL657:
_temp656=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp654)->f1; goto _LL651;} else{ goto _LL620;} _LL651: _temp650= _temp610.f2;
if((( struct _tunion_struct*) _temp650)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL653: _temp652=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp650)->f1; goto _LL619;} else{ goto _LL620;} _LL620: goto _LL621; _LL613:*
escaping_states=({ struct Cyc_List_List* _temp658=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp658[ 0]=({ struct Cyc_List_List
_temp659; _temp659.hd=( void*) new_pinfo; _temp659.tl=* escaping_states;
_temp659;}); _temp658;}); return; _LL615: return; _LL617:(( void(*)( void(* f)(
struct Cyc_List_List**, void*, void*), struct Cyc_List_List** env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2_c)( Cyc_CfAbsexp_assign_escape,
escaping_states, _temp648, _temp644); return; _LL619:(( void(*)( void(* f)(
struct Cyc_List_List**, void*, void*), struct Cyc_List_List** env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2_c)( Cyc_CfAbsexp_assign_escape,
escaping_states, _temp656, _temp652); return; _LL621:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp660=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp660[ 0]=({ struct
Cyc_Core_Impossible_struct _temp661; _temp661.tag= Cyc_Core_Impossible_tag;
_temp661.f1=( struct _tagged_string)({ char* _temp662=( char*)"bad pinfos in assign_escape";
struct _tagged_string _temp663; _temp663.curr= _temp662; _temp663.base= _temp662;
_temp663.last_plus_one= _temp662 + 28; _temp663;}); _temp661;}); _temp660;}));
_LL611:;} int Cyc_CfAbsexp_is_ok_malloc_assign( void* lval, struct Cyc_List_List*
ok_mallocs){ void* _temp664= lval; int _temp672; struct Cyc_Absyn_Exp* _temp674;
void* _temp676; void* _temp678; _LL666: if(( unsigned int) _temp664 > 1u?((
struct _tunion_struct*) _temp664)->tag == Cyc_CfAbsexp_MallocOp_tag: 0){ _LL675:
_temp674=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp664)->f1; goto _LL673; _LL673: _temp672=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp664)->f2; goto _LL667;} else{ goto _LL668;} _LL668: if(( unsigned int)
_temp664 > 1u?(( struct _tunion_struct*) _temp664)->tag == Cyc_CfAbsexp_MemberOp_tag:
0){ _LL679: _temp678=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp664)->f1;
goto _LL677; _LL677: _temp676=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp664)->f2; goto _LL669;} else{ goto _LL670;} _LL670: goto _LL671; _LL667:
return(( int(*)( struct Cyc_List_List* l, struct Cyc_Absyn_Exp* x)) Cyc_List_memq)(
ok_mallocs, _temp674); _LL669: return Cyc_CfAbsexp_is_ok_malloc_assign( _temp678,
ok_mallocs); _LL671: return 0; _LL665:;} struct _tuple5 Cyc_CfAbsexp_unordered_mallocs(
void* ae, struct Cyc_List_List* ok_mallocs){ struct Cyc_List_List* assigns= 0;
struct Cyc_List_List* mallocs= 0;{ void* _temp680= ae; void* _temp702; void*
_temp704; void* _temp706; int _temp708; void* _temp710; struct Cyc_Absyn_Exp*
_temp712; int _temp714; void* _temp716; struct Cyc_Absyn_Stmt* _temp718; struct
Cyc_List_List* _temp720; void* _temp722; struct Cyc_List_List* _temp724; void*
_temp726; struct Cyc_List_List* _temp728; void* _temp730; _LL682: if(( int)
_temp680 == Cyc_CfAbsexp_BottomAE){ goto _LL683;} else{ goto _LL684;} _LL684:
if(( int) _temp680 == Cyc_CfAbsexp_SkipAE){ goto _LL685;} else{ goto _LL686;}
_LL686: if(( unsigned int) _temp680 > 2u?(( struct _tunion_struct*) _temp680)->tag
== Cyc_CfAbsexp_UseAE_tag: 0){ _LL703: _temp702=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*)
_temp680)->f1; goto _LL687;} else{ goto _LL688;} _LL688: if(( unsigned int)
_temp680 > 2u?(( struct _tunion_struct*) _temp680)->tag == Cyc_CfAbsexp_AssignAE_tag:
0){ _LL707: _temp706=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp680)->f1;
goto _LL705; _LL705: _temp704=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp680)->f2; goto _LL689;} else{ goto _LL690;} _LL690: if(( unsigned int)
_temp680 > 2u?(( struct _tunion_struct*) _temp680)->tag == Cyc_CfAbsexp_MallocAE_tag:
0){ _LL711: _temp710=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp680)->f1;
if((( struct _tunion_struct*) _temp710)->tag == Cyc_CfFlowInfo_MallocPt_tag){
_LL713: _temp712=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp710)->f1; goto _LL709;} else{ goto _LL692;} _LL709: _temp708=( int)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp680)->f2; goto _LL691;} else{ goto
_LL692;} _LL692: if(( unsigned int) _temp680 > 2u?(( struct _tunion_struct*)
_temp680)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL717: _temp716=( void*)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp680)->f1; goto _LL715; _LL715:
_temp714=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp680)->f2; goto
_LL693;} else{ goto _LL694;} _LL694: if(( unsigned int) _temp680 > 2u?(( struct
_tunion_struct*) _temp680)->tag == Cyc_CfAbsexp_StmtAE_tag: 0){ _LL719: _temp718=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp680)->f1;
goto _LL695;} else{ goto _LL696;} _LL696: if(( unsigned int) _temp680 > 2u?((
struct _tunion_struct*) _temp680)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL723:
_temp722=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp680)->f1; if(( int)
_temp722 == Cyc_CfAbsexp_OrderedG){ goto _LL721;} else{ goto _LL698;} _LL721:
_temp720=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp680)->f2; goto _LL697;} else{ goto _LL698;} _LL698: if(( unsigned int)
_temp680 > 2u?(( struct _tunion_struct*) _temp680)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL727: _temp726=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp680)->f1;
if(( int) _temp726 == Cyc_CfAbsexp_OneofG){ goto _LL725;} else{ goto _LL700;}
_LL725: _temp724=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp680)->f2; goto _LL699;} else{ goto _LL700;} _LL700: if(( unsigned int)
_temp680 > 2u?(( struct _tunion_struct*) _temp680)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL731: _temp730=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp680)->f1;
if(( int) _temp730 == Cyc_CfAbsexp_UnorderedG){ goto _LL729;} else{ goto _LL681;}
_LL729: _temp728=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp680)->f2; goto _LL701;} else{ goto _LL681;} _LL683: goto _LL681; _LL685:
goto _LL681; _LL687: goto _LL681; _LL689: if( Cyc_CfAbsexp_is_ok_malloc_assign(
_temp706, ok_mallocs)){ assigns=({ struct Cyc_List_List* _temp732=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp732->hd=( void*) ae; _temp732->tl=
assigns; _temp732;});} goto _LL681; _LL691: mallocs=({ struct Cyc_List_List*
_temp733=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp733->hd=( void*) _temp712; _temp733->tl= mallocs; _temp733;}); goto _LL681;
_LL693:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp734=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp734[ 0]=({ struct Cyc_Core_Impossible_struct _temp735; _temp735.tag= Cyc_Core_Impossible_tag;
_temp735.f1=( struct _tagged_string)({ char* _temp736=( char*)"local variable in MallocAE";
struct _tagged_string _temp737; _temp737.curr= _temp736; _temp737.base= _temp736;
_temp737.last_plus_one= _temp736 + 27; _temp737;}); _temp735;}); _temp734;}));
_LL695: goto _LL681; _LL697: for( 0; _temp720 != 0; _temp720= _temp720->tl){
struct Cyc_List_List* _temp740; struct Cyc_List_List* _temp742; struct _tuple5
_temp738= Cyc_CfAbsexp_unordered_mallocs(( void*) _temp720->hd,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( mallocs,
ok_mallocs)); _LL743: _temp742= _temp738.f1; goto _LL741; _LL741: _temp740=
_temp738.f2; goto _LL739; _LL739: assigns=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp742, assigns);
mallocs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp740, mallocs);} goto _LL681; _LL699: goto _LL681;
_LL701: for( 0; _temp728 != 0; _temp728= _temp728->tl){ struct Cyc_List_List*
_temp746; struct Cyc_List_List* _temp748; struct _tuple5 _temp744= Cyc_CfAbsexp_unordered_mallocs((
void*) _temp728->hd, ok_mallocs); _LL749: _temp748= _temp744.f1; goto _LL747;
_LL747: _temp746= _temp744.f2; goto _LL745; _LL745: assigns=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp748,
assigns); mallocs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp746, mallocs);} goto _LL681; _LL681:;}
return({ struct _tuple5 _temp750; _temp750.f1= assigns; _temp750.f2= mallocs;
_temp750;});} void* Cyc_CfAbsexp_eval_absexp( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Position_Segment* loc, void* ae, void* in_flow){ struct Cyc_Dict_Dict*
pinfo_dict;{ void* _temp751= in_flow; struct Cyc_Dict_Dict* _temp757; _LL753:
if(( int) _temp751 == Cyc_CfFlowInfo_BottomFL){ goto _LL754;} else{ goto _LL755;}
_LL755: if(( unsigned int) _temp751 > 1u?(( struct _tunion_struct*) _temp751)->tag
== Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL758: _temp757=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp751)->f1; goto _LL756;} else{ goto
_LL752;} _LL754: return( void*) Cyc_CfFlowInfo_BottomFL; _LL756: pinfo_dict=
_temp757; goto _LL752; _LL752:;}{ void* _temp759= ae; void* _temp779; void*
_temp781; void* _temp783; int _temp785; void* _temp787; struct Cyc_Absyn_Stmt*
_temp789; struct Cyc_List_List* _temp791; void* _temp793; struct Cyc_List_List*
_temp795; void* _temp797; struct Cyc_List_List* _temp799; void* _temp801; _LL761:
if(( unsigned int) _temp759 > 2u?(( struct _tunion_struct*) _temp759)->tag ==
Cyc_CfAbsexp_AssignAE_tag: 0){ _LL782: _temp781=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp759)->f1; goto _LL780; _LL780: _temp779=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp759)->f2; goto _LL762;} else{ goto _LL763;} _LL763: if(( unsigned int)
_temp759 > 2u?(( struct _tunion_struct*) _temp759)->tag == Cyc_CfAbsexp_UseAE_tag:
0){ _LL784: _temp783=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp759)->f1;
goto _LL764;} else{ goto _LL765;} _LL765: if(( unsigned int) _temp759 > 2u?((
struct _tunion_struct*) _temp759)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL788:
_temp787=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp759)->f1; goto
_LL786; _LL786: _temp785=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp759)->f2;
goto _LL766;} else{ goto _LL767;} _LL767: if(( int) _temp759 == Cyc_CfAbsexp_BottomAE){
goto _LL768;} else{ goto _LL769;} _LL769: if(( int) _temp759 == Cyc_CfAbsexp_SkipAE){
goto _LL770;} else{ goto _LL771;} _LL771: if(( unsigned int) _temp759 > 2u?((
struct _tunion_struct*) _temp759)->tag == Cyc_CfAbsexp_StmtAE_tag: 0){ _LL790:
_temp789=( struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp759)->f1; goto _LL772;} else{ goto _LL773;} _LL773: if(( unsigned int)
_temp759 > 2u?(( struct _tunion_struct*) _temp759)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL794: _temp793=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp759)->f1;
if(( int) _temp793 == Cyc_CfAbsexp_OrderedG){ goto _LL792;} else{ goto _LL775;}
_LL792: _temp791=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp759)->f2; goto _LL774;} else{ goto _LL775;} _LL775: if(( unsigned int)
_temp759 > 2u?(( struct _tunion_struct*) _temp759)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL798: _temp797=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp759)->f1;
if(( int) _temp797 == Cyc_CfAbsexp_OneofG){ goto _LL796;} else{ goto _LL777;}
_LL796: _temp795=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp759)->f2; goto _LL776;} else{ goto _LL777;} _LL777: if(( unsigned int)
_temp759 > 2u?(( struct _tunion_struct*) _temp759)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL802: _temp801=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp759)->f1;
if(( int) _temp801 == Cyc_CfAbsexp_UnorderedG){ goto _LL800;} else{ goto _LL760;}
_LL800: _temp799=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp759)->f2; goto _LL778;} else{ goto _LL760;} _LL762:{ void* _temp803= Cyc_CfAbsexp_eval_absop_l(
pinfo_dict, _temp781); void* _temp809; void* _temp811; struct Cyc_CfFlowInfo_Place*
_temp813; _LL805: if((( struct _tunion_struct*) _temp803)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL812: _temp811=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp803)->f1;
goto _LL810; _LL810: _temp809=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp803)->f2; goto _LL806;} else{ goto _LL807;} _LL807: if((( struct
_tunion_struct*) _temp803)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL814:
_temp813=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp803)->f1; goto _LL808;} else{ goto _LL804;} _LL806: goto _LL804; _LL808: {
void* old_pinfo= Cyc_CfFlowInfo_lookup_place( pinfo_dict, _temp813); void*
new_pinfo= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp779); struct _tuple3
_temp816=({ struct _tuple3 _temp815; _temp815.f1= old_pinfo; _temp815.f2=
new_pinfo; _temp815;}); void* _temp824; void* _temp826; void* _temp828; void*
_temp830; void* _temp832; struct Cyc_Dict_Dict* _temp834; void* _temp836; void*
_temp838; void* _temp840; void* _temp842; void* _temp844; struct Cyc_Dict_Dict*
_temp846; _LL818: _LL833: _temp832= _temp816.f1; if((( struct _tunion_struct*)
_temp832)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL835: _temp834=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp832)->f1; goto _LL825;} else{ goto
_LL820;} _LL825: _temp824= _temp816.f2; if((( struct _tunion_struct*) _temp824)->tag
== Cyc_CfFlowInfo_LeafPI_tag){ _LL827: _temp826=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp824)->f1; if((( struct _tunion_struct*) _temp826)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL831: _temp830=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp826)->f1;
goto _LL829; _LL829: _temp828=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp826)->f2; goto _LL819;} else{ goto _LL820;}} else{ goto _LL820;} _LL820:
_LL845: _temp844= _temp816.f1; if((( struct _tunion_struct*) _temp844)->tag ==
Cyc_CfFlowInfo_StructPI_tag){ _LL847: _temp846=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_StructPI_struct*) _temp844)->f1; goto _LL837;} else{ goto _LL822;}
_LL837: _temp836= _temp816.f2; if((( struct _tunion_struct*) _temp836)->tag ==
Cyc_CfFlowInfo_LeafPI_tag){ _LL839: _temp838=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp836)->f1; if((( struct _tunion_struct*) _temp838)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL843: _temp842=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp838)->f1;
goto _LL841; _LL841: _temp840=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp838)->f2; goto _LL821;} else{ goto _LL822;}} else{ goto _LL822;} _LL822:
goto _LL823; _LL819: _temp840= _temp828; goto _LL821; _LL821: new_pinfo= Cyc_CfFlowInfo_assign_unknown_dict(
_temp840, old_pinfo); pinfo_dict= Cyc_CfFlowInfo_insert_place( pinfo_dict,
_temp813, new_pinfo);{ struct Cyc_CfFlowInfo_InitsFL_struct* ans=({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp848=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp848[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp849; _temp849.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp849.f1= pinfo_dict; _temp849;}); _temp848;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) ans); return( void*) ans;} _LL823: { struct Cyc_List_List*
escaping_pinfos= 0; Cyc_CfAbsexp_assign_escape(& escaping_pinfos, old_pinfo,
new_pinfo); for( 0; escaping_pinfos != 0; escaping_pinfos= escaping_pinfos->tl){
void* esc_pinfo=( void*) escaping_pinfos->hd; if( ! Cyc_CfFlowInfo_isAllInit(
pinfo_dict, esc_pinfo)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp850=( char*)"uninitialized value assigned to ambiguous place"; struct
_tagged_string _temp851; _temp851.curr= _temp850; _temp851.base= _temp850;
_temp851.last_plus_one= _temp850 + 48; _temp851;})); return( void*) Cyc_CfFlowInfo_BottomFL;}
pinfo_dict= Cyc_CfFlowInfo_escape_pointsto( esc_pinfo, pinfo_dict);} pinfo_dict=
Cyc_CfFlowInfo_insert_place( pinfo_dict, _temp813, new_pinfo);{ struct Cyc_CfFlowInfo_InitsFL_struct*
ans=({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp852=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp852[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp853; _temp853.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp853.f1= pinfo_dict; _temp853;}); _temp852;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) ans); return( void*) ans;}} _LL817:;} _LL804:;} _temp783= _temp779;
goto _LL764; _LL764: { void* pinfo= Cyc_CfAbsexp_eval_absop_r( pinfo_dict,
_temp783); if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict, pinfo)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp854= Cyc_CfAbsexp_absop2string( _temp783);
xprintf("%.*s must be fully initialized to be used here", _temp854.last_plus_one
- _temp854.curr, _temp854.curr);})); return( void*) Cyc_CfFlowInfo_BottomFL;}{
struct Cyc_Dict_Dict* out_dict= Cyc_CfFlowInfo_escape_pointsto( pinfo,
pinfo_dict); if( out_dict == pinfo_dict){ return in_flow;}{ struct Cyc_CfFlowInfo_InitsFL_struct*
ans=({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp855=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp855[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp856; _temp856.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp856.f1= pinfo_dict; _temp856;}); _temp855;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) ans); return( void*) ans;}}} _LL766: return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp857=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp857[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp858; _temp858.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp858.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key,
void* data)) Cyc_Dict_insert)( pinfo_dict, _temp787,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( env->roots, _temp787)); _temp858;}); _temp857;});
_LL768: return( void*) Cyc_CfFlowInfo_BottomFL; _LL770: return in_flow; _LL772:
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp789, in_flow); _LL774: for(
0; _temp791 != 0; _temp791= _temp791->tl){ in_flow= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*) _temp791->hd, in_flow);} return in_flow; _LL776: { void*
out_flow=( void*) Cyc_CfFlowInfo_BottomFL; for( 0; _temp795 != 0; _temp795=
_temp795->tl){ out_flow= Cyc_CfFlowInfo_join_flow( out_flow, Cyc_CfAbsexp_eval_absexp(
env, loc,( void*) _temp795->hd, in_flow));} return out_flow;} _LL778: { void*
out_flow= in_flow; do { in_flow= out_flow; out_flow= in_flow;{ struct Cyc_List_List*
ael2= _temp799; for( 0; ael2 != 0; ael2= ael2->tl){ out_flow= Cyc_CfFlowInfo_join_flow(
out_flow, Cyc_CfAbsexp_eval_absexp( env, loc,( void*) ael2->hd, out_flow));}}} while (
! Cyc_CfFlowInfo_flow_lessthan_approx( out_flow, in_flow));{ struct Cyc_Dict_Dict*
pinfo_dict;{ void* _temp859= in_flow; struct Cyc_Dict_Dict* _temp865; _LL861:
if(( int) _temp859 == Cyc_CfFlowInfo_BottomFL){ goto _LL862;} else{ goto _LL863;}
_LL863: if(( unsigned int) _temp859 > 1u?(( struct _tunion_struct*) _temp859)->tag
== Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL866: _temp865=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp859)->f1; goto _LL864;} else{ goto
_LL860;} _LL862: return( void*) Cyc_CfFlowInfo_BottomFL; _LL864: pinfo_dict=
_temp865; goto _LL860; _LL860:;}{ struct Cyc_List_List* _temp869; struct Cyc_List_List*
_temp871; struct _tuple5 _temp867= Cyc_CfAbsexp_unordered_mallocs( ae, 0);
_LL872: _temp871= _temp867.f1; goto _LL870; _LL870: _temp869= _temp867.f2; goto
_LL868; _LL868: for( 0; _temp869 != 0; _temp869= _temp869->tl){ struct Cyc_CfFlowInfo_MallocPt_struct*
malloc_pt=({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp873=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp873[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp874; _temp874.tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp874.f1=( struct Cyc_Absyn_Exp*) _temp869->hd; _temp874;}); _temp873;});
pinfo_dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( pinfo_dict,( void*) malloc_pt,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( env->roots,( void*) malloc_pt));} out_flow=(
void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp875=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp875[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp876; _temp876.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp876.f1= pinfo_dict; _temp876;}); _temp875;}); for( 0; _temp871 != 0;
_temp871= _temp871->tl){ out_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)
_temp871->hd, out_flow);} return out_flow;}}} _LL760:;}}