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
int* ans_place)) Cyc_Dict_lookup_bool)(({ struct Cyc_Dict_Dict** _temp81= Cyc_CfAbsexp_mallocpt_dict;
if( _temp81 == 0){ _throw( Null_Exception);}* _temp81;}), e,& i)){({ struct Cyc_Dict_Dict**
_temp82= Cyc_CfAbsexp_mallocpt_dict; if( _temp82 == 0){ _throw( Null_Exception);}*
_temp82=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp*
key, int data)) Cyc_Dict_insert)(({ struct Cyc_Dict_Dict** _temp83= Cyc_CfAbsexp_mallocpt_dict;
if( _temp83 == 0){ _throw( Null_Exception);}* _temp83;}), e,( i=( ++ Cyc_CfAbsexp_num_mallocpts)));});}
return i;}} void* Cyc_CfAbsexp_mkUnknownOp(){ return( void*) Cyc_CfAbsexp_UnknownOp;}
void* Cyc_CfAbsexp_mkAddressOp( void* ao){ Cyc_CfAbsexp_ok_address_arg( ao);
return( void*)({ struct Cyc_CfAbsexp_AddressOp_struct* _temp84=( struct Cyc_CfAbsexp_AddressOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AddressOp_struct)); _temp84[ 0]=({ struct
Cyc_CfAbsexp_AddressOp_struct _temp85; _temp85.tag= Cyc_CfAbsexp_AddressOp_tag;
_temp85.f1=( void*) ao; _temp85;}); _temp84;});} void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd){ return( void*)({ struct Cyc_CfAbsexp_LocalOp_struct*
_temp86=( struct Cyc_CfAbsexp_LocalOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_LocalOp_struct));
_temp86[ 0]=({ struct Cyc_CfAbsexp_LocalOp_struct _temp87; _temp87.tag= Cyc_CfAbsexp_LocalOp_tag;
_temp87.f1= vd; _temp87;}); _temp86;});} void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f){ Cyc_CfAbsexp_ok_member_arg( ao); return( void*)({ struct Cyc_CfAbsexp_MemberOp_struct*
_temp88=( struct Cyc_CfAbsexp_MemberOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MemberOp_struct));
_temp88[ 0]=({ struct Cyc_CfAbsexp_MemberOp_struct _temp89; _temp89.tag= Cyc_CfAbsexp_MemberOp_tag;
_temp89.f1=( void*) ao; _temp89.f2=( void*) f; _temp89;}); _temp88;});} void*
Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e){ return( void*)({ struct Cyc_CfAbsexp_MallocOp_struct*
_temp90=( struct Cyc_CfAbsexp_MallocOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocOp_struct));
_temp90[ 0]=({ struct Cyc_CfAbsexp_MallocOp_struct _temp91; _temp91.tag= Cyc_CfAbsexp_MallocOp_tag;
_temp91.f1= e; _temp91.f2= Cyc_CfAbsexp_mallocpt_int( e); _temp91;}); _temp90;});}
void* Cyc_CfAbsexp_mkDerefOp( void* ao){ Cyc_CfAbsexp_ok_deref_arg( ao); return(
void*)({ struct Cyc_CfAbsexp_DerefOp_struct* _temp92=( struct Cyc_CfAbsexp_DerefOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_DerefOp_struct)); _temp92[ 0]=({ struct
Cyc_CfAbsexp_DerefOp_struct _temp93; _temp93.tag= Cyc_CfAbsexp_DerefOp_tag;
_temp93.f1=( void*) ao; _temp93;}); _temp92;});} void* Cyc_CfAbsexp_mkBottomAE(){
return( void*) Cyc_CfAbsexp_BottomAE;} void* Cyc_CfAbsexp_mkSkipAE(){ return(
void*) Cyc_CfAbsexp_SkipAE;} void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp*
e){ return( void*)({ struct Cyc_CfAbsexp_MallocAE_struct* _temp94=( struct Cyc_CfAbsexp_MallocAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocAE_struct)); _temp94[ 0]=({ struct
Cyc_CfAbsexp_MallocAE_struct _temp95; _temp95.tag= Cyc_CfAbsexp_MallocAE_tag;
_temp95.f1=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp96=(
struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp96[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp97; _temp97.tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp97.f1= e; _temp97;}); _temp96;})); _temp95.f2= Cyc_CfAbsexp_mallocpt_int( e);
_temp95;}); _temp94;});} void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s){
return( void*)({ struct Cyc_CfAbsexp_StmtAE_struct* _temp98=( struct Cyc_CfAbsexp_StmtAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_StmtAE_struct)); _temp98[ 0]=({ struct
Cyc_CfAbsexp_StmtAE_struct _temp99; _temp99.tag= Cyc_CfAbsexp_StmtAE_tag;
_temp99.f1= s; _temp99;}); _temp98;});} void* Cyc_CfAbsexp_mkUseAE( void* ao){
void* _temp100= ao; _LL102: if(( int) _temp100 == Cyc_CfAbsexp_UnknownOp){ goto
_LL103;} else{ goto _LL104;} _LL104: goto _LL105; _LL103: return( void*) Cyc_CfAbsexp_SkipAE;
_LL105: return( void*)({ struct Cyc_CfAbsexp_UseAE_struct* _temp106=( struct Cyc_CfAbsexp_UseAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_UseAE_struct)); _temp106[ 0]=({ struct
Cyc_CfAbsexp_UseAE_struct _temp107; _temp107.tag= Cyc_CfAbsexp_UseAE_tag;
_temp107.f1=( void*) ao; _temp107;}); _temp106;}); _LL101:;} struct _tuple3{
void* f1; void* f2; } ; void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r){ struct
_tuple3 _temp109=({ struct _tuple3 _temp108; _temp108.f1= l; _temp108.f2= r;
_temp108;}); void* _temp117; void* _temp119; void* _temp121; void* _temp123;
_LL111: _LL120: _temp119= _temp109.f1; if(( int) _temp119 == Cyc_CfAbsexp_UnknownOp){
goto _LL118;} else{ goto _LL113;} _LL118: _temp117= _temp109.f2; if(( int)
_temp117 == Cyc_CfAbsexp_UnknownOp){ goto _LL112;} else{ goto _LL113;} _LL113:
_LL124: _temp123= _temp109.f1; if(( int) _temp123 == Cyc_CfAbsexp_UnknownOp){
goto _LL122;} else{ goto _LL115;} _LL122: _temp121= _temp109.f2; goto _LL114;
_LL115: goto _LL116; _LL112: return( void*) Cyc_CfAbsexp_SkipAE; _LL114: return
Cyc_CfAbsexp_mkUseAE( r); _LL116: Cyc_CfAbsexp_ok_lvalue( l); return( void*)({
struct Cyc_CfAbsexp_AssignAE_struct* _temp125=( struct Cyc_CfAbsexp_AssignAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AssignAE_struct)); _temp125[ 0]=({ struct
Cyc_CfAbsexp_AssignAE_struct _temp126; _temp126.tag= Cyc_CfAbsexp_AssignAE_tag;
_temp126.f1=( void*) l; _temp126.f2=( void*) r; _temp126;}); _temp125;}); _LL110:;}
struct _tuple4{ void* f1; void* f2; void* f3; } ; void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2){ struct _tuple4 _temp128=({ struct _tuple4
_temp127; _temp127.f1= g; _temp127.f2= ae1; _temp127.f3= ae2; _temp127;}); void*
_temp151; void* _temp153; void* _temp155; void* _temp158; void* _temp160; void*
_temp162; void* _temp164; void* _temp166; void* _temp168; void* _temp170; void*
_temp172; void* _temp174; void* _temp176; void* _temp178; void* _temp180; void*
_temp183; struct Cyc_List_List* _temp185; void* _temp187; void* _temp189; struct
Cyc_List_List* _temp191; void* _temp193; void* _temp195; void* _temp198; void*
_temp200; struct Cyc_List_List* _temp202; void* _temp204; void* _temp206; void*
_temp208; void* _temp210; struct Cyc_List_List* _temp212; void* _temp214; void*
_temp216; void* _temp219; struct Cyc_List_List* _temp221; void* _temp223; void*
_temp225; void* _temp227; _LL130: _LL156: _temp155= _temp128.f1; goto _LL154;
_LL154: _temp153= _temp128.f2; goto _LL152; _LL152: _temp151= _temp128.f3; if((
int) _temp151 == Cyc_CfAbsexp_SkipAE){ goto _LL150;} else{ goto _LL132;} _LL150:
if( g !=( void*) Cyc_CfAbsexp_OneofG){ goto _LL131;} else{ goto _LL132;} _LL132:
_LL163: _temp162= _temp128.f1; goto _LL161; _LL161: _temp160= _temp128.f2; if((
int) _temp160 == Cyc_CfAbsexp_SkipAE){ goto _LL159;} else{ goto _LL134;} _LL159:
_temp158= _temp128.f3; goto _LL157; _LL157: if( g !=( void*) Cyc_CfAbsexp_OneofG){
goto _LL133;} else{ goto _LL134;} _LL134: _LL169: _temp168= _temp128.f1; if((
int) _temp168 == Cyc_CfAbsexp_OneofG){ goto _LL167;} else{ goto _LL136;} _LL167:
_temp166= _temp128.f2; goto _LL165; _LL165: _temp164= _temp128.f3; if(( int)
_temp164 == Cyc_CfAbsexp_BottomAE){ goto _LL135;} else{ goto _LL136;} _LL136:
_LL175: _temp174= _temp128.f1; if(( int) _temp174 == Cyc_CfAbsexp_OneofG){ goto
_LL173;} else{ goto _LL138;} _LL173: _temp172= _temp128.f2; if(( int) _temp172
== Cyc_CfAbsexp_BottomAE){ goto _LL171;} else{ goto _LL138;} _LL171: _temp170=
_temp128.f3; goto _LL137; _LL138: _LL181: _temp180= _temp128.f1; if(( int)
_temp180 == Cyc_CfAbsexp_OrderedG){ goto _LL179;} else{ goto _LL140;} _LL179:
_temp178= _temp128.f2; if(( int) _temp178 == Cyc_CfAbsexp_BottomAE){ goto _LL177;}
else{ goto _LL140;} _LL177: _temp176= _temp128.f3; goto _LL139; _LL140: _LL196:
_temp195= _temp128.f1; goto _LL190; _LL190: _temp189= _temp128.f2; if((
unsigned int) _temp189 > 2u?(( struct _tunion_struct*) _temp189)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL194: _temp193=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp189)->f1;
goto _LL192; _LL192: _temp191=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp189)->f2; goto _LL184;} else{ goto _LL142;} _LL184: _temp183= _temp128.f3;
if(( unsigned int) _temp183 > 2u?(( struct _tunion_struct*) _temp183)->tag ==
Cyc_CfAbsexp_GroupAE_tag: 0){ _LL188: _temp187=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp183)->f1; goto _LL186; _LL186: _temp185=( struct Cyc_List_List*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp183)->f2; goto _LL182;} else{ goto _LL142;}
_LL182: if( _temp195 == _temp193? _temp195 == _temp187: 0){ goto _LL141;} else{
goto _LL142;} _LL142: _LL207: _temp206= _temp128.f1; goto _LL201; _LL201:
_temp200= _temp128.f2; if(( unsigned int) _temp200 > 2u?(( struct _tunion_struct*)
_temp200)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL205: _temp204=( void*)((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp200)->f1; goto _LL203; _LL203:
_temp202=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp200)->f2; goto _LL199;} else{ goto _LL144;} _LL199: _temp198= _temp128.f3;
goto _LL197; _LL197: if( _temp206 == _temp204? _temp206 !=( void*) Cyc_CfAbsexp_OrderedG:
0){ goto _LL143;} else{ goto _LL144;} _LL144: _LL217: _temp216= _temp128.f1; if((
int) _temp216 == Cyc_CfAbsexp_OrderedG){ goto _LL211;} else{ goto _LL146;}
_LL211: _temp210= _temp128.f2; if(( unsigned int) _temp210 > 2u?(( struct
_tunion_struct*) _temp210)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL215:
_temp214=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp210)->f1; if(( int)
_temp214 == Cyc_CfAbsexp_OrderedG){ goto _LL213;} else{ goto _LL146;} _LL213:
_temp212=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp210)->f2; goto _LL209;} else{ goto _LL146;} _LL209: _temp208= _temp128.f3;
goto _LL145; _LL146: _LL228: _temp227= _temp128.f1; goto _LL226; _LL226:
_temp225= _temp128.f2; goto _LL220; _LL220: _temp219= _temp128.f3; if((
unsigned int) _temp219 > 2u?(( struct _tunion_struct*) _temp219)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL224: _temp223=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp219)->f1;
goto _LL222; _LL222: _temp221=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp219)->f2; goto _LL218;} else{ goto _LL148;} _LL218: if( _temp227 ==
_temp223){ goto _LL147;} else{ goto _LL148;} _LL148: goto _LL149; _LL131: return
ae1; _LL133: return ae2; _LL135: return ae1; _LL137: return ae2; _LL139: return(
void*) Cyc_CfAbsexp_BottomAE; _LL141: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp229=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp229[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp230; _temp230.tag= Cyc_CfAbsexp_GroupAE_tag;
_temp230.f1=( void*) _temp195; _temp230.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp191, _temp185);
_temp230;}); _temp229;}); _LL143: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp231=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp231[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp232; _temp232.tag= Cyc_CfAbsexp_GroupAE_tag;
_temp232.f1=( void*) _temp206; _temp232.f2=({ struct Cyc_List_List* _temp233=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp233[
0]=({ struct Cyc_List_List _temp234; _temp234.hd=( void*) ae2; _temp234.tl=
_temp202; _temp234;}); _temp233;}); _temp232;}); _temp231;}); _LL145: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp235=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp235[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp236; _temp236.tag= Cyc_CfAbsexp_GroupAE_tag;
_temp236.f1=( void*)(( void*) Cyc_CfAbsexp_OrderedG); _temp236.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp212,({
struct Cyc_List_List* _temp237=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp237[ 0]=({ struct Cyc_List_List _temp238;
_temp238.hd=( void*) ae2; _temp238.tl= 0; _temp238;}); _temp237;})); _temp236;});
_temp235;}); _LL147: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp239=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp239[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp240; _temp240.tag= Cyc_CfAbsexp_GroupAE_tag;
_temp240.f1=( void*) _temp227; _temp240.f2=({ struct Cyc_List_List* _temp241=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp241[
0]=({ struct Cyc_List_List _temp242; _temp242.hd=( void*) ae1; _temp242.tl=
_temp221; _temp242;}); _temp241;}); _temp240;}); _temp239;}); _LL149: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp243=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp243[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp244; _temp244.tag= Cyc_CfAbsexp_GroupAE_tag;
_temp244.f1=( void*) g; _temp244.f2=({ struct Cyc_List_List* _temp245=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp245[ 0]=({
struct Cyc_List_List _temp246; _temp246.hd=( void*) ae1; _temp246.tl=({ struct
Cyc_List_List* _temp247=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp247[ 0]=({ struct Cyc_List_List _temp248; _temp248.hd=( void*) ae2;
_temp248.tl= 0; _temp248;}); _temp247;}); _temp246;}); _temp245;}); _temp244;});
_temp243;}); _LL129:;} void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List*
ael){ if( ael == 0){ return( void*) Cyc_CfAbsexp_SkipAE;} if(({ struct Cyc_List_List*
_temp249= ael; if( _temp249 == 0){ _throw( Null_Exception);} _temp249->tl;}) ==
0){ return( void*)({ struct Cyc_List_List* _temp250= ael; if( _temp250 == 0){
_throw( Null_Exception);} _temp250->hd;});} return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp251=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp251[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp252; _temp252.tag= Cyc_CfAbsexp_GroupAE_tag;
_temp252.f1=( void*) g; _temp252.f2= ael; _temp252;}); _temp251;});} int Cyc_CfAbsexp_isUnknownOp(
void* ao){ return ao ==( void*) Cyc_CfAbsexp_UnknownOp;} struct _tagged_string
Cyc_CfAbsexp_absop2string( void* ao){ void* _temp253= ao; void* _temp267; struct
Cyc_Absyn_Vardecl* _temp269; void* _temp271; void* _temp273; int _temp275;
struct Cyc_Absyn_Exp* _temp277; void* _temp279; _LL255: if(( int) _temp253 ==
Cyc_CfAbsexp_UnknownOp){ goto _LL256;} else{ goto _LL257;} _LL257: if((
unsigned int) _temp253 > 1u?(( struct _tunion_struct*) _temp253)->tag == Cyc_CfAbsexp_AddressOp_tag:
0){ _LL268: _temp267=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp253)->f1;
goto _LL258;} else{ goto _LL259;} _LL259: if(( unsigned int) _temp253 > 1u?((
struct _tunion_struct*) _temp253)->tag == Cyc_CfAbsexp_LocalOp_tag: 0){ _LL270:
_temp269=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp253)->f1; goto _LL260;} else{ goto _LL261;} _LL261: if(( unsigned int)
_temp253 > 1u?(( struct _tunion_struct*) _temp253)->tag == Cyc_CfAbsexp_MemberOp_tag:
0){ _LL274: _temp273=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp253)->f1;
goto _LL272; _LL272: _temp271=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp253)->f2; goto _LL262;} else{ goto _LL263;} _LL263: if(( unsigned int)
_temp253 > 1u?(( struct _tunion_struct*) _temp253)->tag == Cyc_CfAbsexp_MallocOp_tag:
0){ _LL278: _temp277=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp253)->f1; goto _LL276; _LL276: _temp275=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp253)->f2; goto _LL264;} else{ goto _LL265;} _LL265: if(( unsigned int)
_temp253 > 1u?(( struct _tunion_struct*) _temp253)->tag == Cyc_CfAbsexp_DerefOp_tag:
0){ _LL280: _temp279=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp253)->f1;
goto _LL266;} else{ goto _LL254;} _LL256: return( struct _tagged_string)({ char*
_temp281=( char*)"Unknown"; struct _tagged_string _temp282; _temp282.curr=
_temp281; _temp282.base= _temp281; _temp282.last_plus_one= _temp281 + 8;
_temp282;}); _LL258: return({ struct _tagged_string _temp283= Cyc_CfAbsexp_absop2string(
_temp267); xprintf("& %.*s", _temp283.last_plus_one - _temp283.curr, _temp283.curr);});
_LL260: return*(* _temp269->name).f2; _LL262: { struct _tagged_string fs;{ void*
_temp284= _temp271; struct _tagged_string* _temp290; int _temp292; _LL286: if(((
struct _tunion_struct*) _temp284)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL291:
_temp290=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp284)->f1; goto _LL287;} else{ goto _LL288;} _LL288: if((( struct
_tunion_struct*) _temp284)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL293: _temp292=(
int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp284)->f1; goto _LL289;} else{
goto _LL285;} _LL287: fs=* _temp290; goto _LL285; _LL289: fs= xprintf("%d",
_temp292); goto _LL285; _LL285:;} return({ struct _tagged_string _temp294= Cyc_CfAbsexp_absop2string(
_temp273); struct _tagged_string _temp295= fs; xprintf("%.*s.%.*s", _temp294.last_plus_one
- _temp294.curr, _temp294.curr, _temp295.last_plus_one - _temp295.curr, _temp295.curr);});}
_LL264: return xprintf("<mpt%d>", _temp275); _LL266: return({ struct
_tagged_string _temp296= Cyc_CfAbsexp_absop2string( _temp279); xprintf("(*%.*s)",
_temp296.last_plus_one - _temp296.curr, _temp296.curr);}); _LL254:;} struct
_tagged_string Cyc_CfAbsexp_absexp2string( void* ae, int depth){ struct
_tagged_string ans= Cyc_Core_new_string( depth);{ int i= 0; for( 0; i <({ struct
_tagged_string _temp297= ans;( unsigned int)( _temp297.last_plus_one - _temp297.curr);});
++ i){({ struct _tagged_string _temp298= ans; char* _temp300= _temp298.curr + i;
if( _temp298.base == 0? 1:( _temp300 < _temp298.base? 1: _temp300 >= _temp298.last_plus_one)){
_throw( Null_Exception);}* _temp300=' ';});}}{ void* _temp301= ae; void*
_temp319; int _temp321; void* _temp323; struct Cyc_Absyn_Exp* _temp325; void*
_temp327; void* _temp329; struct Cyc_List_List* _temp331; void* _temp333; struct
Cyc_Absyn_Stmt* _temp335; _LL303: if(( int) _temp301 == Cyc_CfAbsexp_SkipAE){
goto _LL304;} else{ goto _LL305;} _LL305: if(( unsigned int) _temp301 > 2u?((
struct _tunion_struct*) _temp301)->tag == Cyc_CfAbsexp_UseAE_tag: 0){ _LL320:
_temp319=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp301)->f1; goto
_LL306;} else{ goto _LL307;} _LL307: if(( unsigned int) _temp301 > 2u?(( struct
_tunion_struct*) _temp301)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL324:
_temp323=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp301)->f1; if(((
struct _tunion_struct*) _temp323)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL326:
_temp325=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp323)->f1; goto _LL322;} else{ goto _LL309;} _LL322: _temp321=( int)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp301)->f2; goto _LL308;} else{ goto
_LL309;} _LL309: if(( unsigned int) _temp301 > 2u?(( struct _tunion_struct*)
_temp301)->tag == Cyc_CfAbsexp_AssignAE_tag: 0){ _LL330: _temp329=( void*)((
struct Cyc_CfAbsexp_AssignAE_struct*) _temp301)->f1; goto _LL328; _LL328:
_temp327=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp301)->f2; goto
_LL310;} else{ goto _LL311;} _LL311: if(( unsigned int) _temp301 > 2u?(( struct
_tunion_struct*) _temp301)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL334:
_temp333=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp301)->f1; goto
_LL332; _LL332: _temp331=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp301)->f2; goto _LL312;} else{ goto _LL313;} _LL313: if(( unsigned int)
_temp301 > 2u?(( struct _tunion_struct*) _temp301)->tag == Cyc_CfAbsexp_StmtAE_tag:
0){ _LL336: _temp335=( struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp301)->f1; goto _LL314;} else{ goto _LL315;} _LL315: if(( int) _temp301 ==
Cyc_CfAbsexp_BottomAE){ goto _LL316;} else{ goto _LL317;} _LL317: goto _LL318;
_LL304: ans=({ struct _tagged_string _temp337= ans; xprintf("%.*sskip", _temp337.last_plus_one
- _temp337.curr, _temp337.curr);}); goto _LL302; _LL306: ans=({ struct
_tagged_string _temp338= ans; struct _tagged_string _temp339= Cyc_CfAbsexp_absop2string(
_temp319); xprintf("%.*suse %.*s", _temp338.last_plus_one - _temp338.curr,
_temp338.curr, _temp339.last_plus_one - _temp339.curr, _temp339.curr);}); goto
_LL302; _LL308: ans=({ struct _tagged_string _temp340= ans; int _temp341=
_temp321; xprintf("%.*smalloc %i", _temp340.last_plus_one - _temp340.curr,
_temp340.curr, _temp341);}); goto _LL302; _LL310: ans=({ struct _tagged_string
_temp342= ans; struct _tagged_string _temp343= Cyc_CfAbsexp_absop2string(
_temp329); struct _tagged_string _temp344= Cyc_CfAbsexp_absop2string( _temp327);
xprintf("%.*sassign %.*s %.*s", _temp342.last_plus_one - _temp342.curr, _temp342.curr,
_temp343.last_plus_one - _temp343.curr, _temp343.curr, _temp344.last_plus_one -
_temp344.curr, _temp344.curr);}); goto _LL302; _LL312:{ void* _temp345= _temp333;
_LL347: if(( int) _temp345 == Cyc_CfAbsexp_OrderedG){ goto _LL348;} else{ goto
_LL349;} _LL349: if(( int) _temp345 == Cyc_CfAbsexp_UnorderedG){ goto _LL350;}
else{ goto _LL351;} _LL351: if(( int) _temp345 == Cyc_CfAbsexp_OneofG){ goto
_LL352;} else{ goto _LL346;} _LL348: ans=({ struct _tagged_string _temp353= ans;
xprintf("%.*sordered", _temp353.last_plus_one - _temp353.curr, _temp353.curr);});
goto _LL346; _LL350: ans=({ struct _tagged_string _temp354= ans; xprintf("%.*sunordered",
_temp354.last_plus_one - _temp354.curr, _temp354.curr);}); goto _LL346; _LL352:
ans=({ struct _tagged_string _temp355= ans; xprintf("%.*soneof", _temp355.last_plus_one
- _temp355.curr, _temp355.curr);}); goto _LL346; _LL346:;} for( 0; _temp331 != 0;
_temp331=({ struct Cyc_List_List* _temp356= _temp331; if( _temp356 == 0){ _throw(
Null_Exception);} _temp356->tl;})){ ans=({ struct _tagged_string _temp358= ans;
struct _tagged_string _temp359= Cyc_CfAbsexp_absexp2string(( void*)({ struct Cyc_List_List*
_temp357= _temp331; if( _temp357 == 0){ _throw( Null_Exception);} _temp357->hd;}),
depth + 1); xprintf("%.*s\n%.*s", _temp358.last_plus_one - _temp358.curr,
_temp358.curr, _temp359.last_plus_one - _temp359.curr, _temp359.curr);});} goto
_LL302; _LL314: ans=({ struct _tagged_string _temp360= ans; xprintf("%.*s<<stmt>>",
_temp360.last_plus_one - _temp360.curr, _temp360.curr);}); goto _LL302; _LL316:
ans=({ struct _tagged_string _temp361= ans; xprintf("%.*sbottom", _temp361.last_plus_one
- _temp361.curr, _temp361.curr);}); goto _LL302; _LL318:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_CfAbsexp_BadAbsexp_struct* _temp362=( struct Cyc_CfAbsexp_BadAbsexp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct)); _temp362[ 0]=({
struct Cyc_CfAbsexp_BadAbsexp_struct _temp363; _temp363.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp363;}); _temp362;})); _LL302:;} return ans;} static void Cyc_CfAbsexp_check_absop(
void* ao){ void* _temp364= ao; void* _temp378; void* _temp380; void* _temp382;
void* _temp384; struct Cyc_Absyn_Vardecl* _temp386; int _temp388; struct Cyc_Absyn_Exp*
_temp390; _LL366: if(( int) _temp364 == Cyc_CfAbsexp_UnknownOp){ goto _LL367;}
else{ goto _LL368;} _LL368: if(( unsigned int) _temp364 > 1u?(( struct
_tunion_struct*) _temp364)->tag == Cyc_CfAbsexp_AddressOp_tag: 0){ _LL379:
_temp378=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp364)->f1; goto
_LL369;} else{ goto _LL370;} _LL370: if(( unsigned int) _temp364 > 1u?(( struct
_tunion_struct*) _temp364)->tag == Cyc_CfAbsexp_DerefOp_tag: 0){ _LL381:
_temp380=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp364)->f1; goto
_LL371;} else{ goto _LL372;} _LL372: if(( unsigned int) _temp364 > 1u?(( struct
_tunion_struct*) _temp364)->tag == Cyc_CfAbsexp_MemberOp_tag: 0){ _LL385:
_temp384=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp364)->f1; goto
_LL383; _LL383: _temp382=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp364)->f2; goto _LL373;} else{ goto _LL374;} _LL374: if(( unsigned int)
_temp364 > 1u?(( struct _tunion_struct*) _temp364)->tag == Cyc_CfAbsexp_LocalOp_tag:
0){ _LL387: _temp386=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp364)->f1; goto _LL375;} else{ goto _LL376;} _LL376: if(( unsigned int)
_temp364 > 1u?(( struct _tunion_struct*) _temp364)->tag == Cyc_CfAbsexp_MallocOp_tag:
0){ _LL391: _temp390=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp364)->f1; goto _LL389; _LL389: _temp388=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp364)->f2; goto _LL377;} else{ goto _LL365;} _LL367: return; _LL369: Cyc_CfAbsexp_check_absop(
_temp378); Cyc_CfAbsexp_ok_address_arg( _temp378); return; _LL371: Cyc_CfAbsexp_check_absop(
_temp380); Cyc_CfAbsexp_ok_deref_arg( _temp380); return; _LL373: Cyc_CfAbsexp_check_absop(
_temp384); Cyc_CfAbsexp_ok_member_arg( _temp384); return; _LL375: return; _LL377:
return; _LL365:;} void Cyc_CfAbsexp_check_absexp( void* ae){ void* _temp392= ae;
int _temp410; void* _temp412; struct Cyc_Absyn_Vardecl* _temp414; int _temp416;
void* _temp418; struct Cyc_Absyn_Stmt* _temp420; struct Cyc_List_List* _temp422;
void* _temp424; void* _temp426; void* _temp428; void* _temp430; _LL394: if(( int)
_temp392 == Cyc_CfAbsexp_BottomAE){ goto _LL395;} else{ goto _LL396;} _LL396:
if(( int) _temp392 == Cyc_CfAbsexp_SkipAE){ goto _LL397;} else{ goto _LL398;}
_LL398: if(( unsigned int) _temp392 > 2u?(( struct _tunion_struct*) _temp392)->tag
== Cyc_CfAbsexp_MallocAE_tag: 0){ _LL413: _temp412=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp392)->f1; if((( struct _tunion_struct*) _temp412)->tag == Cyc_CfFlowInfo_VarRoot_tag){
_LL415: _temp414=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp412)->f1; goto _LL411;} else{ goto _LL400;} _LL411: _temp410=( int)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp392)->f2; goto _LL399;} else{ goto
_LL400;} _LL400: if(( unsigned int) _temp392 > 2u?(( struct _tunion_struct*)
_temp392)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL419: _temp418=( void*)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp392)->f1; goto _LL417; _LL417:
_temp416=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp392)->f2; goto
_LL401;} else{ goto _LL402;} _LL402: if(( unsigned int) _temp392 > 2u?(( struct
_tunion_struct*) _temp392)->tag == Cyc_CfAbsexp_StmtAE_tag: 0){ _LL421: _temp420=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp392)->f1;
goto _LL403;} else{ goto _LL404;} _LL404: if(( unsigned int) _temp392 > 2u?((
struct _tunion_struct*) _temp392)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL425:
_temp424=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp392)->f1; goto
_LL423; _LL423: _temp422=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp392)->f2; goto _LL405;} else{ goto _LL406;} _LL406: if(( unsigned int)
_temp392 > 2u?(( struct _tunion_struct*) _temp392)->tag == Cyc_CfAbsexp_UseAE_tag:
0){ _LL427: _temp426=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp392)->f1;
goto _LL407;} else{ goto _LL408;} _LL408: if(( unsigned int) _temp392 > 2u?((
struct _tunion_struct*) _temp392)->tag == Cyc_CfAbsexp_AssignAE_tag: 0){ _LL431:
_temp430=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp392)->f1; goto
_LL429; _LL429: _temp428=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp392)->f2; goto _LL409;} else{ goto _LL393;} _LL395: return; _LL397: return;
_LL399:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_CfAbsexp_BadAbsexp_struct*
_temp432=( struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));
_temp432[ 0]=({ struct Cyc_CfAbsexp_BadAbsexp_struct _temp433; _temp433.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp433;}); _temp432;})); _LL401: return; _LL403: return; _LL405: for( 0;
_temp422 != 0; _temp422=({ struct Cyc_List_List* _temp434= _temp422; if(
_temp434 == 0){ _throw( Null_Exception);} _temp434->tl;})){ Cyc_CfAbsexp_check_absexp((
void*)({ struct Cyc_List_List* _temp435= _temp422; if( _temp435 == 0){ _throw(
Null_Exception);} _temp435->hd;}));} return; _LL407: Cyc_CfAbsexp_check_absop(
_temp426); return; _LL409: Cyc_CfAbsexp_check_absop( _temp430); Cyc_CfAbsexp_check_absop(
_temp428); Cyc_CfAbsexp_ok_lvalue( _temp430); return; _LL393:;} void* Cyc_CfAbsexp_eval_absop_r(
struct Cyc_Dict_Dict* pinfo_dict, void* ao); void* Cyc_CfAbsexp_eval_absop_l(
struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void* _temp436= ao; struct Cyc_Absyn_Vardecl*
_temp450; int _temp452; struct Cyc_Absyn_Exp* _temp454; void* _temp456; void*
_temp458; void* _temp460; void* _temp462; _LL438: if(( unsigned int) _temp436 >
1u?(( struct _tunion_struct*) _temp436)->tag == Cyc_CfAbsexp_LocalOp_tag: 0){
_LL451: _temp450=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp436)->f1; goto _LL439;} else{ goto _LL440;} _LL440: if(( unsigned int)
_temp436 > 1u?(( struct _tunion_struct*) _temp436)->tag == Cyc_CfAbsexp_MallocOp_tag:
0){ _LL455: _temp454=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp436)->f1; goto _LL453; _LL453: _temp452=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp436)->f2; goto _LL441;} else{ goto _LL442;} _LL442: if(( unsigned int)
_temp436 > 1u?(( struct _tunion_struct*) _temp436)->tag == Cyc_CfAbsexp_MemberOp_tag:
0){ _LL459: _temp458=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp436)->f1;
goto _LL457; _LL457: _temp456=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp436)->f2; goto _LL443;} else{ goto _LL444;} _LL444: if(( unsigned int)
_temp436 > 1u?(( struct _tunion_struct*) _temp436)->tag == Cyc_CfAbsexp_DerefOp_tag:
0){ _LL461: _temp460=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp436)->f1;
goto _LL445;} else{ goto _LL446;} _LL446: if(( int) _temp436 == Cyc_CfAbsexp_UnknownOp){
goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int) _temp436 > 1u?((
struct _tunion_struct*) _temp436)->tag == Cyc_CfAbsexp_AddressOp_tag: 0){ _LL463:
_temp462=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp436)->f1; goto
_LL449;} else{ goto _LL437;} _LL439: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp464=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp464[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp465; _temp465.tag= Cyc_CfFlowInfo_MustPointTo_tag; _temp465.f1=({ struct
Cyc_CfFlowInfo_Place* _temp466=( struct Cyc_CfFlowInfo_Place*) GC_malloc(
sizeof( struct Cyc_CfFlowInfo_Place) * 1); _temp466[ 0]=({ struct Cyc_CfFlowInfo_Place
_temp467; _temp467.root=( void*)(( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp468=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp468[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp469; _temp469.tag= Cyc_CfFlowInfo_VarRoot_tag;
_temp469.f1= _temp450; _temp469;}); _temp468;})); _temp467.fields= 0; _temp467;});
_temp466;}); _temp465;}); _temp464;}); _LL441: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp470=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp470[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp471; _temp471.tag= Cyc_CfFlowInfo_MustPointTo_tag; _temp471.f1=({ struct
Cyc_CfFlowInfo_Place* _temp472=( struct Cyc_CfFlowInfo_Place*) GC_malloc(
sizeof( struct Cyc_CfFlowInfo_Place) * 1); _temp472[ 0]=({ struct Cyc_CfFlowInfo_Place
_temp473; _temp473.root=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct*
_temp474=( struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp474[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp475; _temp475.tag=
Cyc_CfFlowInfo_MallocPt_tag; _temp475.f1= _temp454; _temp475;}); _temp474;}));
_temp473.fields= 0; _temp473;}); _temp472;}); _temp471;}); _temp470;}); _LL443: {
void* inner_state= Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp458); void*
_temp476= inner_state; void* _temp482; void* _temp484; struct Cyc_CfFlowInfo_Place*
_temp486; struct Cyc_CfFlowInfo_Place _temp488; struct Cyc_List_List* _temp489;
void* _temp491; _LL478: if((( struct _tunion_struct*) _temp476)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL485: _temp484=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp476)->f1;
goto _LL483; _LL483: _temp482=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp476)->f2; goto _LL479;} else{ goto _LL480;} _LL480: if((( struct
_tunion_struct*) _temp476)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL487:
_temp486=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp476)->f1; _temp488=* _temp486; _LL492: _temp491=( void*) _temp488.root;
goto _LL490; _LL490: _temp489=( struct Cyc_List_List*) _temp488.fields; goto
_LL481;} else{ goto _LL477;} _LL479: return inner_state; _LL481: return( void*)({
struct Cyc_CfFlowInfo_MustPointTo_struct* _temp493=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp493[ 0]=({
struct Cyc_CfFlowInfo_MustPointTo_struct _temp494; _temp494.tag= Cyc_CfFlowInfo_MustPointTo_tag;
_temp494.f1=({ struct Cyc_CfFlowInfo_Place* _temp495=( struct Cyc_CfFlowInfo_Place*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place) * 1); _temp495[ 0]=({ struct Cyc_CfFlowInfo_Place
_temp496; _temp496.root=( void*) _temp491; _temp496.fields=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp489,({
struct Cyc_List_List* _temp497=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp497[ 0]=({ struct Cyc_List_List _temp498;
_temp498.hd=( void*) _temp456; _temp498.tl= 0; _temp498;}); _temp497;}));
_temp496;}); _temp495;}); _temp494;}); _temp493;}); _LL477:;} _LL445: { void*
inner_pinfo= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp460); void* _temp499=
inner_pinfo; void* _temp507; struct Cyc_Dict_Dict* _temp509; struct Cyc_Dict_Dict*
_temp511; _LL501: if((( struct _tunion_struct*) _temp499)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL508: _temp507=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp499)->f1;
goto _LL502;} else{ goto _LL503;} _LL503: if((( struct _tunion_struct*) _temp499)->tag
== Cyc_CfFlowInfo_TuplePI_tag){ _LL510: _temp509=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp499)->f1; goto _LL504;} else{ goto
_LL505;} _LL505: if((( struct _tunion_struct*) _temp499)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL512: _temp511=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp499)->f1; goto _LL506;} else{ goto _LL500;} _LL502: return _temp507; _LL504:
goto _LL506; _LL506:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_CfAbsexp_BadAbsexp_struct*
_temp513=( struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));
_temp513[ 0]=({ struct Cyc_CfAbsexp_BadAbsexp_struct _temp514; _temp514.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp514;}); _temp513;})); _LL500:;} _LL447: goto _LL449; _LL449:( void) _throw((
struct _xtunion_struct*)({ struct Cyc_CfAbsexp_BadAbsexp_struct* _temp515=(
struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));
_temp515[ 0]=({ struct Cyc_CfAbsexp_BadAbsexp_struct _temp516; _temp516.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp516;}); _temp515;})); _LL437:;} void* Cyc_CfAbsexp_eval_absop_r( struct Cyc_Dict_Dict*
pinfo_dict, void* ao){ void* _temp517= ao; struct Cyc_Absyn_Vardecl* _temp531;
int _temp533; struct Cyc_Absyn_Exp* _temp535; void* _temp537; void* _temp539;
void* _temp541; void* _temp543; _LL519: if(( unsigned int) _temp517 > 1u?((
struct _tunion_struct*) _temp517)->tag == Cyc_CfAbsexp_LocalOp_tag: 0){ _LL532:
_temp531=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp517)->f1; goto _LL520;} else{ goto _LL521;} _LL521: if(( unsigned int)
_temp517 > 1u?(( struct _tunion_struct*) _temp517)->tag == Cyc_CfAbsexp_MallocOp_tag:
0){ _LL536: _temp535=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp517)->f1; goto _LL534; _LL534: _temp533=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp517)->f2; goto _LL522;} else{ goto _LL523;} _LL523: if(( unsigned int)
_temp517 > 1u?(( struct _tunion_struct*) _temp517)->tag == Cyc_CfAbsexp_MemberOp_tag:
0){ _LL540: _temp539=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp517)->f1;
goto _LL538; _LL538: _temp537=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp517)->f2; goto _LL524;} else{ goto _LL525;} _LL525: if(( unsigned int)
_temp517 > 1u?(( struct _tunion_struct*) _temp517)->tag == Cyc_CfAbsexp_DerefOp_tag:
0){ _LL542: _temp541=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp517)->f1;
goto _LL526;} else{ goto _LL527;} _LL527: if(( int) _temp517 == Cyc_CfAbsexp_UnknownOp){
goto _LL528;} else{ goto _LL529;} _LL529: if(( unsigned int) _temp517 > 1u?((
struct _tunion_struct*) _temp517)->tag == Cyc_CfAbsexp_AddressOp_tag: 0){ _LL544:
_temp543=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp517)->f1; goto
_LL530;} else{ goto _LL518;} _LL520: return(( void*(*)( struct Cyc_Dict_Dict* d,
void* key)) Cyc_Dict_lookup)( pinfo_dict,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp545=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp545[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp546; _temp546.tag= Cyc_CfFlowInfo_VarRoot_tag;
_temp546.f1= _temp531; _temp546;}); _temp545;})); _LL522: return(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( pinfo_dict,( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp547=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp547[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp548; _temp548.tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp548.f1= _temp535; _temp548;}); _temp547;})); _LL524: { void* inner_pinfo=
Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp539); struct _tuple3 _temp550=({
struct _tuple3 _temp549; _temp549.f1= inner_pinfo; _temp549.f2= _temp537;
_temp549;}); void* _temp560; void* _temp562; void* _temp564; void* _temp566;
void* _temp568; void* _temp570; int _temp572; void* _temp574; struct Cyc_Dict_Dict*
_temp576; void* _temp578; struct _tagged_string* _temp580; void* _temp582;
struct Cyc_Dict_Dict* _temp584; _LL552: _LL563: _temp562= _temp550.f1; if(((
struct _tunion_struct*) _temp562)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL565:
_temp564=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp562)->f1; if(((
struct _tunion_struct*) _temp564)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL569:
_temp568=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp564)->f1; goto
_LL567; _LL567: _temp566=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp564)->f2; goto _LL561;} else{ goto _LL554;}} else{ goto _LL554;} _LL561:
_temp560= _temp550.f2; goto _LL553; _LL554: _LL575: _temp574= _temp550.f1; if(((
struct _tunion_struct*) _temp574)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL577:
_temp576=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp574)->f1; goto _LL571;} else{ goto _LL556;} _LL571: _temp570= _temp550.f2;
if((( struct _tunion_struct*) _temp570)->tag == Cyc_CfFlowInfo_TupleF_tag){
_LL573: _temp572=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp570)->f1;
goto _LL555;} else{ goto _LL556;} _LL556: _LL583: _temp582= _temp550.f1; if(((
struct _tunion_struct*) _temp582)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL585:
_temp584=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp582)->f1; goto _LL579;} else{ goto _LL558;} _LL579: _temp578= _temp550.f2;
if((( struct _tunion_struct*) _temp578)->tag == Cyc_CfFlowInfo_StructF_tag){
_LL581: _temp580=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp578)->f1; goto _LL557;} else{ goto _LL558;} _LL558: goto _LL559; _LL553:
return inner_pinfo; _LL555: return(( void*(*)( struct Cyc_Dict_Dict* d, int key))
Cyc_Dict_lookup)( _temp576, _temp572); _LL557: return(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp584, _temp580); _LL559:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_CfAbsexp_BadAbsexp_struct*
_temp586=( struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));
_temp586[ 0]=({ struct Cyc_CfAbsexp_BadAbsexp_struct _temp587; _temp587.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp587;}); _temp586;})); _LL551:;} _LL526: { void* inner_pinfo= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp541); void* _temp588= inner_pinfo; void* _temp598; void*
_temp600; void* _temp602; void* _temp604; void* _temp606; void* _temp608; void*
_temp610; struct Cyc_CfFlowInfo_Place* _temp612; _LL590: if((( struct
_tunion_struct*) _temp588)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL599: _temp598=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp588)->f1; if((( struct
_tunion_struct*) _temp598)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL603:
_temp602=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp598)->f1; goto
_LL601; _LL601: _temp600=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp598)->f2; if(( int) _temp600 == Cyc_CfFlowInfo_ThisIL){ goto _LL591;} else{
goto _LL592;}} else{ goto _LL592;}} else{ goto _LL592;} _LL592: if((( struct
_tunion_struct*) _temp588)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL605: _temp604=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp588)->f1; if((( struct
_tunion_struct*) _temp604)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL609:
_temp608=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp604)->f1; goto
_LL607; _LL607: _temp606=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp604)->f2; goto _LL593;} else{ goto _LL594;}} else{ goto _LL594;} _LL594:
if((( struct _tunion_struct*) _temp588)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL611: _temp610=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp588)->f1;
if((( struct _tunion_struct*) _temp610)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL613: _temp612=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp610)->f1; goto _LL595;} else{ goto _LL596;}} else{ goto _LL596;} _LL596:
goto _LL597; _LL591: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_NoneIL); _LL593: return inner_pinfo; _LL595: return Cyc_CfFlowInfo_lookup_place(
pinfo_dict, _temp612); _LL597:( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_CfAbsexp_BadAbsexp_struct* _temp614=( struct Cyc_CfAbsexp_BadAbsexp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct)); _temp614[ 0]=({
struct Cyc_CfAbsexp_BadAbsexp_struct _temp615; _temp615.tag= Cyc_CfAbsexp_BadAbsexp_tag;
_temp615;}); _temp614;})); _LL589:;} _LL528: return Cyc_CfFlowInfo_mkLeafPI((
void*) Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_AllIL); _LL530: return( void*)({
struct Cyc_CfFlowInfo_LeafPI_struct* _temp616=( struct Cyc_CfFlowInfo_LeafPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct)); _temp616[ 0]=({ struct
Cyc_CfFlowInfo_LeafPI_struct _temp617; _temp617.tag= Cyc_CfFlowInfo_LeafPI_tag;
_temp617.f1=( void*) Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp543); _temp617;});
_temp616;}); _LL518:;} void Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo){ struct _tuple3 _temp619=({
struct _tuple3 _temp618; _temp618.f1= old_pinfo; _temp618.f2= new_pinfo;
_temp618;}); void* _temp631; void* _temp633; void* _temp635; void* _temp637;
void* _temp639; void* _temp641; void* _temp643; void* _temp645; void* _temp647;
void* _temp649; void* _temp651; struct Cyc_Dict_Dict* _temp653; void* _temp655;
struct Cyc_Dict_Dict* _temp657; void* _temp659; struct Cyc_Dict_Dict* _temp661;
void* _temp663; struct Cyc_Dict_Dict* _temp665; _LL621: _LL636: _temp635=
_temp619.f1; if((( struct _tunion_struct*) _temp635)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL638: _temp637=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp635)->f1;
if((( struct _tunion_struct*) _temp637)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL642: _temp641=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp637)->f1;
if(( int) _temp641 == Cyc_CfFlowInfo_Esc){ goto _LL640;} else{ goto _LL623;}
_LL640: _temp639=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp637)->f2;
goto _LL632;} else{ goto _LL623;}} else{ goto _LL623;} _LL632: _temp631=
_temp619.f2; if((( struct _tunion_struct*) _temp631)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL634: _temp633=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp631)->f1;
goto _LL622;} else{ goto _LL623;} _LL623: _LL648: _temp647= _temp619.f1; if(((
struct _tunion_struct*) _temp647)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL650:
_temp649=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp647)->f1; goto
_LL644;} else{ goto _LL625;} _LL644: _temp643= _temp619.f2; if((( struct
_tunion_struct*) _temp643)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL646: _temp645=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp643)->f1; goto _LL624;}
else{ goto _LL625;} _LL625: _LL656: _temp655= _temp619.f1; if((( struct
_tunion_struct*) _temp655)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL658: _temp657=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp655)->f1;
goto _LL652;} else{ goto _LL627;} _LL652: _temp651= _temp619.f2; if((( struct
_tunion_struct*) _temp651)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL654: _temp653=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp651)->f1;
goto _LL626;} else{ goto _LL627;} _LL627: _LL664: _temp663= _temp619.f1; if(((
struct _tunion_struct*) _temp663)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL666:
_temp665=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp663)->f1; goto _LL660;} else{ goto _LL629;} _LL660: _temp659= _temp619.f2;
if((( struct _tunion_struct*) _temp659)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL662: _temp661=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp659)->f1; goto _LL628;} else{ goto _LL629;} _LL629: goto _LL630; _LL622:*
escaping_states=({ struct Cyc_List_List* _temp667=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp667[ 0]=({ struct Cyc_List_List
_temp668; _temp668.hd=( void*) new_pinfo; _temp668.tl=* escaping_states;
_temp668;}); _temp667;}); return; _LL624: return; _LL626:(( void(*)( void(* f)(
struct Cyc_List_List**, void*, void*), struct Cyc_List_List** env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2_c)( Cyc_CfAbsexp_assign_escape,
escaping_states, _temp657, _temp653); return; _LL628:(( void(*)( void(* f)(
struct Cyc_List_List**, void*, void*), struct Cyc_List_List** env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2_c)( Cyc_CfAbsexp_assign_escape,
escaping_states, _temp665, _temp661); return; _LL630:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp669=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp669[ 0]=({ struct
Cyc_Core_Impossible_struct _temp670; _temp670.tag= Cyc_Core_Impossible_tag;
_temp670.f1=( struct _tagged_string)({ char* _temp671=( char*)"bad pinfos in assign_escape";
struct _tagged_string _temp672; _temp672.curr= _temp671; _temp672.base= _temp671;
_temp672.last_plus_one= _temp671 + 28; _temp672;}); _temp670;}); _temp669;}));
_LL620:;} int Cyc_CfAbsexp_is_ok_malloc_assign( void* lval, struct Cyc_List_List*
ok_mallocs){ void* _temp673= lval; int _temp681; struct Cyc_Absyn_Exp* _temp683;
void* _temp685; void* _temp687; _LL675: if(( unsigned int) _temp673 > 1u?((
struct _tunion_struct*) _temp673)->tag == Cyc_CfAbsexp_MallocOp_tag: 0){ _LL684:
_temp683=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp673)->f1; goto _LL682; _LL682: _temp681=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp673)->f2; goto _LL676;} else{ goto _LL677;} _LL677: if(( unsigned int)
_temp673 > 1u?(( struct _tunion_struct*) _temp673)->tag == Cyc_CfAbsexp_MemberOp_tag:
0){ _LL688: _temp687=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp673)->f1;
goto _LL686; _LL686: _temp685=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp673)->f2; goto _LL678;} else{ goto _LL679;} _LL679: goto _LL680; _LL676:
return(( int(*)( struct Cyc_List_List* l, struct Cyc_Absyn_Exp* x)) Cyc_List_memq)(
ok_mallocs, _temp683); _LL678: return Cyc_CfAbsexp_is_ok_malloc_assign( _temp687,
ok_mallocs); _LL680: return 0; _LL674:;} struct _tuple5{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; struct _tuple5 Cyc_CfAbsexp_unordered_mallocs(
void* ae, struct Cyc_List_List* ok_mallocs){ struct Cyc_List_List* assigns= 0;
struct Cyc_List_List* mallocs= 0;{ void* _temp689= ae; void* _temp711; void*
_temp713; void* _temp715; int _temp717; void* _temp719; struct Cyc_Absyn_Exp*
_temp721; int _temp723; void* _temp725; struct Cyc_Absyn_Stmt* _temp727; struct
Cyc_List_List* _temp729; void* _temp731; struct Cyc_List_List* _temp733; void*
_temp735; struct Cyc_List_List* _temp737; void* _temp739; _LL691: if(( int)
_temp689 == Cyc_CfAbsexp_BottomAE){ goto _LL692;} else{ goto _LL693;} _LL693:
if(( int) _temp689 == Cyc_CfAbsexp_SkipAE){ goto _LL694;} else{ goto _LL695;}
_LL695: if(( unsigned int) _temp689 > 2u?(( struct _tunion_struct*) _temp689)->tag
== Cyc_CfAbsexp_UseAE_tag: 0){ _LL712: _temp711=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*)
_temp689)->f1; goto _LL696;} else{ goto _LL697;} _LL697: if(( unsigned int)
_temp689 > 2u?(( struct _tunion_struct*) _temp689)->tag == Cyc_CfAbsexp_AssignAE_tag:
0){ _LL716: _temp715=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp689)->f1;
goto _LL714; _LL714: _temp713=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp689)->f2; goto _LL698;} else{ goto _LL699;} _LL699: if(( unsigned int)
_temp689 > 2u?(( struct _tunion_struct*) _temp689)->tag == Cyc_CfAbsexp_MallocAE_tag:
0){ _LL720: _temp719=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp689)->f1;
if((( struct _tunion_struct*) _temp719)->tag == Cyc_CfFlowInfo_MallocPt_tag){
_LL722: _temp721=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp719)->f1; goto _LL718;} else{ goto _LL701;} _LL718: _temp717=( int)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp689)->f2; goto _LL700;} else{ goto
_LL701;} _LL701: if(( unsigned int) _temp689 > 2u?(( struct _tunion_struct*)
_temp689)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL726: _temp725=( void*)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp689)->f1; goto _LL724; _LL724:
_temp723=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp689)->f2; goto
_LL702;} else{ goto _LL703;} _LL703: if(( unsigned int) _temp689 > 2u?(( struct
_tunion_struct*) _temp689)->tag == Cyc_CfAbsexp_StmtAE_tag: 0){ _LL728: _temp727=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp689)->f1;
goto _LL704;} else{ goto _LL705;} _LL705: if(( unsigned int) _temp689 > 2u?((
struct _tunion_struct*) _temp689)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL732:
_temp731=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp689)->f1; if(( int)
_temp731 == Cyc_CfAbsexp_OrderedG){ goto _LL730;} else{ goto _LL707;} _LL730:
_temp729=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp689)->f2; goto _LL706;} else{ goto _LL707;} _LL707: if(( unsigned int)
_temp689 > 2u?(( struct _tunion_struct*) _temp689)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL736: _temp735=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp689)->f1;
if(( int) _temp735 == Cyc_CfAbsexp_OneofG){ goto _LL734;} else{ goto _LL709;}
_LL734: _temp733=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp689)->f2; goto _LL708;} else{ goto _LL709;} _LL709: if(( unsigned int)
_temp689 > 2u?(( struct _tunion_struct*) _temp689)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL740: _temp739=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp689)->f1;
if(( int) _temp739 == Cyc_CfAbsexp_UnorderedG){ goto _LL738;} else{ goto _LL690;}
_LL738: _temp737=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp689)->f2; goto _LL710;} else{ goto _LL690;} _LL692: goto _LL690; _LL694:
goto _LL690; _LL696: goto _LL690; _LL698: if( Cyc_CfAbsexp_is_ok_malloc_assign(
_temp715, ok_mallocs)){ assigns=({ struct Cyc_List_List* _temp741=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp741->hd=( void*) ae; _temp741->tl=
assigns; _temp741;});} goto _LL690; _LL700: mallocs=({ struct Cyc_List_List*
_temp742=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp742->hd=( void*) _temp721; _temp742->tl= mallocs; _temp742;}); goto _LL690;
_LL702:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp743=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp743[ 0]=({ struct Cyc_Core_Impossible_struct _temp744; _temp744.tag= Cyc_Core_Impossible_tag;
_temp744.f1=( struct _tagged_string)({ char* _temp745=( char*)"local variable in MallocAE";
struct _tagged_string _temp746; _temp746.curr= _temp745; _temp746.base= _temp745;
_temp746.last_plus_one= _temp745 + 27; _temp746;}); _temp744;}); _temp743;}));
_LL704: goto _LL690; _LL706: for( 0; _temp729 != 0; _temp729=({ struct Cyc_List_List*
_temp747= _temp729; if( _temp747 == 0){ _throw( Null_Exception);} _temp747->tl;})){
struct Cyc_List_List* _temp751; struct Cyc_List_List* _temp753; struct _tuple5
_temp749= Cyc_CfAbsexp_unordered_mallocs(( void*)({ struct Cyc_List_List*
_temp748= _temp729; if( _temp748 == 0){ _throw( Null_Exception);} _temp748->hd;}),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
mallocs, ok_mallocs)); _LL754: _temp753= _temp749.f1; goto _LL752; _LL752:
_temp751= _temp749.f2; goto _LL750; _LL750: assigns=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp753,
assigns); mallocs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp751, mallocs);} goto _LL690; _LL708:
goto _LL690; _LL710: for( 0; _temp737 != 0; _temp737=({ struct Cyc_List_List*
_temp755= _temp737; if( _temp755 == 0){ _throw( Null_Exception);} _temp755->tl;})){
struct Cyc_List_List* _temp759; struct Cyc_List_List* _temp761; struct _tuple5
_temp757= Cyc_CfAbsexp_unordered_mallocs(( void*)({ struct Cyc_List_List*
_temp756= _temp737; if( _temp756 == 0){ _throw( Null_Exception);} _temp756->hd;}),
ok_mallocs); _LL762: _temp761= _temp757.f1; goto _LL760; _LL760: _temp759=
_temp757.f2; goto _LL758; _LL758: assigns=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp761, assigns);
mallocs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp759, mallocs);} goto _LL690; _LL690:;} return({
struct _tuple5 _temp763; _temp763.f1= assigns; _temp763.f2= mallocs; _temp763;});}
void* Cyc_CfAbsexp_eval_absexp( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, void* ae, void* in_flow){ struct Cyc_Dict_Dict*
pinfo_dict;{ void* _temp764= in_flow; struct Cyc_Dict_Dict* _temp770; _LL766:
if(( int) _temp764 == Cyc_CfFlowInfo_BottomFL){ goto _LL767;} else{ goto _LL768;}
_LL768: if(( unsigned int) _temp764 > 1u?(( struct _tunion_struct*) _temp764)->tag
== Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL771: _temp770=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp764)->f1; goto _LL769;} else{ goto
_LL765;} _LL767: return( void*) Cyc_CfFlowInfo_BottomFL; _LL769: pinfo_dict=
_temp770; goto _LL765; _LL765:;}{ void* _temp772= ae; void* _temp792; void*
_temp794; void* _temp796; int _temp798; void* _temp800; struct Cyc_Absyn_Stmt*
_temp802; struct Cyc_List_List* _temp804; void* _temp806; struct Cyc_List_List*
_temp808; void* _temp810; struct Cyc_List_List* _temp812; void* _temp814; _LL774:
if(( unsigned int) _temp772 > 2u?(( struct _tunion_struct*) _temp772)->tag ==
Cyc_CfAbsexp_AssignAE_tag: 0){ _LL795: _temp794=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp772)->f1; goto _LL793; _LL793: _temp792=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp772)->f2; goto _LL775;} else{ goto _LL776;} _LL776: if(( unsigned int)
_temp772 > 2u?(( struct _tunion_struct*) _temp772)->tag == Cyc_CfAbsexp_UseAE_tag:
0){ _LL797: _temp796=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp772)->f1;
goto _LL777;} else{ goto _LL778;} _LL778: if(( unsigned int) _temp772 > 2u?((
struct _tunion_struct*) _temp772)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL801:
_temp800=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp772)->f1; goto
_LL799; _LL799: _temp798=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp772)->f2;
goto _LL779;} else{ goto _LL780;} _LL780: if(( int) _temp772 == Cyc_CfAbsexp_BottomAE){
goto _LL781;} else{ goto _LL782;} _LL782: if(( int) _temp772 == Cyc_CfAbsexp_SkipAE){
goto _LL783;} else{ goto _LL784;} _LL784: if(( unsigned int) _temp772 > 2u?((
struct _tunion_struct*) _temp772)->tag == Cyc_CfAbsexp_StmtAE_tag: 0){ _LL803:
_temp802=( struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp772)->f1; goto _LL785;} else{ goto _LL786;} _LL786: if(( unsigned int)
_temp772 > 2u?(( struct _tunion_struct*) _temp772)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL807: _temp806=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp772)->f1;
if(( int) _temp806 == Cyc_CfAbsexp_OrderedG){ goto _LL805;} else{ goto _LL788;}
_LL805: _temp804=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp772)->f2; goto _LL787;} else{ goto _LL788;} _LL788: if(( unsigned int)
_temp772 > 2u?(( struct _tunion_struct*) _temp772)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL811: _temp810=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp772)->f1;
if(( int) _temp810 == Cyc_CfAbsexp_OneofG){ goto _LL809;} else{ goto _LL790;}
_LL809: _temp808=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp772)->f2; goto _LL789;} else{ goto _LL790;} _LL790: if(( unsigned int)
_temp772 > 2u?(( struct _tunion_struct*) _temp772)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL815: _temp814=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp772)->f1;
if(( int) _temp814 == Cyc_CfAbsexp_UnorderedG){ goto _LL813;} else{ goto _LL773;}
_LL813: _temp812=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp772)->f2; goto _LL791;} else{ goto _LL773;} _LL775:{ void* _temp816= Cyc_CfAbsexp_eval_absop_l(
pinfo_dict, _temp794); void* _temp822; void* _temp824; struct Cyc_CfFlowInfo_Place*
_temp826; _LL818: if((( struct _tunion_struct*) _temp816)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL825: _temp824=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp816)->f1;
goto _LL823; _LL823: _temp822=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp816)->f2; goto _LL819;} else{ goto _LL820;} _LL820: if((( struct
_tunion_struct*) _temp816)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL827:
_temp826=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp816)->f1; goto _LL821;} else{ goto _LL817;} _LL819: goto _LL817; _LL821: {
void* old_pinfo= Cyc_CfFlowInfo_lookup_place( pinfo_dict, _temp826); void*
new_pinfo= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp792); struct _tuple3
_temp829=({ struct _tuple3 _temp828; _temp828.f1= old_pinfo; _temp828.f2=
new_pinfo; _temp828;}); void* _temp837; void* _temp839; void* _temp841; void*
_temp843; void* _temp845; struct Cyc_Dict_Dict* _temp847; void* _temp849; void*
_temp851; void* _temp853; void* _temp855; void* _temp857; struct Cyc_Dict_Dict*
_temp859; _LL831: _LL846: _temp845= _temp829.f1; if((( struct _tunion_struct*)
_temp845)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL848: _temp847=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp845)->f1; goto _LL838;} else{ goto
_LL833;} _LL838: _temp837= _temp829.f2; if((( struct _tunion_struct*) _temp837)->tag
== Cyc_CfFlowInfo_LeafPI_tag){ _LL840: _temp839=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp837)->f1; if((( struct _tunion_struct*) _temp839)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL844: _temp843=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp839)->f1;
goto _LL842; _LL842: _temp841=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp839)->f2; goto _LL832;} else{ goto _LL833;}} else{ goto _LL833;} _LL833:
_LL858: _temp857= _temp829.f1; if((( struct _tunion_struct*) _temp857)->tag ==
Cyc_CfFlowInfo_StructPI_tag){ _LL860: _temp859=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_StructPI_struct*) _temp857)->f1; goto _LL850;} else{ goto _LL835;}
_LL850: _temp849= _temp829.f2; if((( struct _tunion_struct*) _temp849)->tag ==
Cyc_CfFlowInfo_LeafPI_tag){ _LL852: _temp851=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp849)->f1; if((( struct _tunion_struct*) _temp851)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL856: _temp855=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp851)->f1;
goto _LL854; _LL854: _temp853=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp851)->f2; goto _LL834;} else{ goto _LL835;}} else{ goto _LL835;} _LL835:
goto _LL836; _LL832: _temp853= _temp841; goto _LL834; _LL834: new_pinfo= Cyc_CfFlowInfo_assign_unknown_dict(
_temp853, old_pinfo); pinfo_dict= Cyc_CfFlowInfo_insert_place( pinfo_dict,
_temp826, new_pinfo);{ struct Cyc_CfFlowInfo_InitsFL_struct* ans=({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp861=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp861[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp862; _temp862.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp862.f1= pinfo_dict; _temp862;}); _temp861;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) ans); return( void*) ans;} _LL836: { struct Cyc_List_List*
escaping_pinfos= 0; Cyc_CfAbsexp_assign_escape(& escaping_pinfos, old_pinfo,
new_pinfo); for( 0; escaping_pinfos != 0; escaping_pinfos=({ struct Cyc_List_List*
_temp863= escaping_pinfos; if( _temp863 == 0){ _throw( Null_Exception);}
_temp863->tl;})){ void* esc_pinfo=( void*)({ struct Cyc_List_List* _temp866=
escaping_pinfos; if( _temp866 == 0){ _throw( Null_Exception);} _temp866->hd;});
if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict, esc_pinfo)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp864=( char*)"uninitialized value assigned to ambiguous place";
struct _tagged_string _temp865; _temp865.curr= _temp864; _temp865.base= _temp864;
_temp865.last_plus_one= _temp864 + 48; _temp865;})); return( void*) Cyc_CfFlowInfo_BottomFL;}
pinfo_dict= Cyc_CfFlowInfo_escape_pointsto( esc_pinfo, pinfo_dict);} pinfo_dict=
Cyc_CfFlowInfo_insert_place( pinfo_dict, _temp826, new_pinfo);{ struct Cyc_CfFlowInfo_InitsFL_struct*
ans=({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp867=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp867[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp868; _temp868.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp868.f1= pinfo_dict; _temp868;}); _temp867;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) ans); return( void*) ans;}} _LL830:;} _LL817:;} _temp796= _temp792;
goto _LL777; _LL777: { void* pinfo= Cyc_CfAbsexp_eval_absop_r( pinfo_dict,
_temp796); if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict, pinfo)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp869= Cyc_CfAbsexp_absop2string( _temp796);
xprintf("%.*s must be fully initialized to be used here", _temp869.last_plus_one
- _temp869.curr, _temp869.curr);})); return( void*) Cyc_CfFlowInfo_BottomFL;}{
struct Cyc_Dict_Dict* out_dict= Cyc_CfFlowInfo_escape_pointsto( pinfo,
pinfo_dict); if( out_dict == pinfo_dict){ return in_flow;}{ struct Cyc_CfFlowInfo_InitsFL_struct*
ans=({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp870=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp870[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp871; _temp871.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp871.f1= pinfo_dict; _temp871;}); _temp870;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) ans); return( void*) ans;}}} _LL779: return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp872=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp872[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp873; _temp873.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp873.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key,
void* data)) Cyc_Dict_insert)( pinfo_dict, _temp800,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( env->roots, _temp800)); _temp873;}); _temp872;});
_LL781: return( void*) Cyc_CfFlowInfo_BottomFL; _LL783: return in_flow; _LL785:
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp802, in_flow); _LL787: for(
0; _temp804 != 0; _temp804=({ struct Cyc_List_List* _temp874= _temp804; if(
_temp874 == 0){ _throw( Null_Exception);} _temp874->tl;})){ in_flow= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)({ struct Cyc_List_List* _temp875= _temp804; if( _temp875 == 0){
_throw( Null_Exception);} _temp875->hd;}), in_flow);} return in_flow; _LL789: {
void* out_flow=( void*) Cyc_CfFlowInfo_BottomFL; for( 0; _temp808 != 0; _temp808=({
struct Cyc_List_List* _temp876= _temp808; if( _temp876 == 0){ _throw(
Null_Exception);} _temp876->tl;})){ out_flow= Cyc_CfFlowInfo_join_flow( out_flow,
Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({ struct Cyc_List_List* _temp877=
_temp808; if( _temp877 == 0){ _throw( Null_Exception);} _temp877->hd;}), in_flow));}
return out_flow;} _LL791: { void* out_flow= in_flow; do { in_flow= out_flow;
out_flow= in_flow;{ struct Cyc_List_List* ael2= _temp812; for( 0; ael2 != 0;
ael2=({ struct Cyc_List_List* _temp878= ael2; if( _temp878 == 0){ _throw(
Null_Exception);} _temp878->tl;})){ out_flow= Cyc_CfFlowInfo_join_flow( out_flow,
Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({ struct Cyc_List_List* _temp879=
ael2; if( _temp879 == 0){ _throw( Null_Exception);} _temp879->hd;}), out_flow));}}} while (
! Cyc_CfFlowInfo_flow_lessthan_approx( out_flow, in_flow));{ struct Cyc_Dict_Dict*
pinfo_dict;{ void* _temp880= in_flow; struct Cyc_Dict_Dict* _temp886; _LL882:
if(( int) _temp880 == Cyc_CfFlowInfo_BottomFL){ goto _LL883;} else{ goto _LL884;}
_LL884: if(( unsigned int) _temp880 > 1u?(( struct _tunion_struct*) _temp880)->tag
== Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL887: _temp886=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp880)->f1; goto _LL885;} else{ goto
_LL881;} _LL883: return( void*) Cyc_CfFlowInfo_BottomFL; _LL885: pinfo_dict=
_temp886; goto _LL881; _LL881:;}{ struct Cyc_List_List* _temp890; struct Cyc_List_List*
_temp892; struct _tuple5 _temp888= Cyc_CfAbsexp_unordered_mallocs( ae, 0);
_LL893: _temp892= _temp888.f1; goto _LL891; _LL891: _temp890= _temp888.f2; goto
_LL889; _LL889: for( 0; _temp890 != 0; _temp890=({ struct Cyc_List_List*
_temp894= _temp890; if( _temp894 == 0){ _throw( Null_Exception);} _temp894->tl;})){
struct Cyc_CfFlowInfo_MallocPt_struct* malloc_pt=({ struct Cyc_CfFlowInfo_MallocPt_struct*
_temp895=( struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp895[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp896; _temp896.tag=
Cyc_CfFlowInfo_MallocPt_tag; _temp896.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp897= _temp890; if( _temp897 == 0){ _throw( Null_Exception);} _temp897->hd;});
_temp896;}); _temp895;}); pinfo_dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( pinfo_dict,( void*) malloc_pt,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,(
void*) malloc_pt));} out_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp898=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp898[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp899; _temp899.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp899.f1= pinfo_dict; _temp899;}); _temp898;}); for( 0; _temp892 != 0;
_temp892=({ struct Cyc_List_List* _temp900= _temp892; if( _temp900 == 0){ _throw(
Null_Exception);} _temp900->tl;})){ out_flow= Cyc_CfAbsexp_eval_absexp( env, loc,(
void*)({ struct Cyc_List_List* _temp901= _temp892; if( _temp901 == 0){ _throw(
Null_Exception);} _temp901->hd;}), out_flow);} return out_flow;}}} _LL773:;}}