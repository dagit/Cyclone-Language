#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct
_tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct _tuple6{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct _tuple7{ struct Cyc_Absyn_Tqual*
f1; void* f2; } ; struct _tuple8{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat*
f2; } ; struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; } ; struct _tuple10{ struct Cyc_Tcpat_Con_s* f1;
struct Cyc_List_List* f2; } ; struct _tuple11{ int f1; struct Cyc_Position_Segment*
f2; } ; struct _tuple12{ void* f1; struct _tuple11* f2; } ; struct _tuple13{
struct Cyc_Position_Segment* f1; int f2; } ; struct _tuple14{ void* f1; int f2;
} ; typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern void* Cyc_Core_snd( struct _tuple0*);
extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(*
f)( void*, int), void* env); struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[
8u]; struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; struct Cyc_Dict_Absent_struct{ char* tag; } ; struct Cyc_Set_Set; typedef
struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set* Cyc_Set_empty(
int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct
Cyc_Set_Set* s, void* elt); extern int Cyc_Set_cardinality( struct Cyc_Set_Set*
s); extern int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern char
Cyc_Set_Absent_tag[ 7u]; struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[
14u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrcmp( struct _tagged_string, struct
_tagged_string); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); struct Cyc_Lineno_Pos{ struct _tagged_string
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
struct _tuple1* Cyc_Absyn_qvar_t; typedef struct _tuple1* Cyc_Absyn_qvar_opt_t;
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
char* tag; } ; extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar( void*); extern
void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t; extern struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl( struct _tuple1* x, void* t, struct Cyc_Absyn_Exp* init);
struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct
Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef
struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_kind2string( void*); extern
struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); typedef void*
Cyc_Tcenv_Resolved; static const int Cyc_Tcenv_VarRes_tag= 0; struct Cyc_Tcenv_VarRes_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes_tag= 1; struct Cyc_Tcenv_StructRes_struct{
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
struct _tuple1*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct
Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern
void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void* rgn); extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag; } ; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern
void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_string);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern struct _tuple4* Cyc_Tcutil_make_inst_var(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List*
vs, struct Cyc_Position_Segment* loc, struct _tagged_string err_msg); extern
struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd); extern struct
_tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p,
void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); static struct _tuple6* Cyc_Tcpat_null_designator(
struct Cyc_Absyn_Pat* p){ return({ struct _tuple6* _temp0=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp0->f1= 0; _temp0->f2= p; _temp0;});}
static struct _tagged_string* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl* vd){
return(* vd->name).f2;} static void* Cyc_Tcpat_any_type( void** topt){ if( topt
!= 0){ return({ void** _temp1= topt; if( _temp1 == 0){ _throw( Null_Exception);}*
_temp1;});} return Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);} static void*
Cyc_Tcpat_num_type( void** topt, void* numt){ if( topt != 0? Cyc_Tcutil_coerceable(({
void** _temp2= topt; if( _temp2 == 0){ _throw( Null_Exception);}* _temp2;})): 0){
return({ void** _temp3= topt; if( _temp3 == 0){ _throw( Null_Exception);}*
_temp3;});} return numt;} static void Cyc_Tcpat_set_vd( struct Cyc_Absyn_Vardecl*
vd, struct Cyc_List_List** v_result_ptr, void* t){( void*)( vd->type=( void*) t);
vd->tq= Cyc_Absyn_empty_tqual(); vd->shadow= 0;* v_result_ptr=({ struct Cyc_List_List*
_temp4=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4->hd=( void*) vd; _temp4->tl=* v_result_ptr; _temp4;});} struct _tuple5
Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p, void** topt,
void** region_opt){ void* t; struct Cyc_List_List* tv_result= 0; struct Cyc_List_List*
v_result= 0;{ void* _temp5=( void*) p->r; int _temp41; void* _temp43; int
_temp45; void* _temp47; char _temp49; struct _tagged_string _temp51; struct Cyc_Absyn_Vardecl*
_temp53; struct Cyc_Absyn_Vardecl* _temp55; struct Cyc_Absyn_Pat* _temp57;
struct Cyc_List_List* _temp59; struct Cyc_List_List* _temp61; struct Cyc_List_List*
_temp63; struct Cyc_Core_Opt* _temp65; struct Cyc_Core_Opt** _temp67; struct Cyc_Absyn_Structdecl*
_temp68; struct Cyc_Absyn_Tunionfield* _temp70; struct Cyc_Absyn_Tuniondecl*
_temp72; struct Cyc_List_List* _temp74; struct Cyc_List_List* _temp76; struct
Cyc_Core_Opt* _temp78; struct _tuple1* _temp80; struct Cyc_Absyn_Tunionfield*
_temp82; struct Cyc_Absyn_XTuniondecl* _temp84; struct Cyc_List_List* _temp86;
struct Cyc_List_List* _temp88; struct _tuple1* _temp90; struct Cyc_Absyn_Enumfield*
_temp92; struct Cyc_Absyn_Enumdecl* _temp94; struct _tuple1* _temp96; struct
_tuple1* _temp98; struct Cyc_List_List* _temp100; struct Cyc_List_List* _temp102;
struct _tuple1* _temp104; struct Cyc_List_List* _temp106; struct Cyc_List_List*
_temp108; struct _tuple1* _temp110; _LL7: if(( unsigned int) _temp5 > 2u?((
struct _tunion_struct*) _temp5)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL44: _temp43=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp5)->f1; if(( int) _temp43 == Cyc_Absyn_Unsigned){
goto _LL42;} else{ goto _LL9;} _LL42: _temp41=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp5)->f2; goto _LL8;} else{ goto _LL9;} _LL9: if(( unsigned int) _temp5 > 2u?((
struct _tunion_struct*) _temp5)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL48: _temp47=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp5)->f1; if(( int) _temp47 == Cyc_Absyn_Signed){
goto _LL46;} else{ goto _LL11;} _LL46: _temp45=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp5)->f2; goto _LL10;} else{ goto _LL11;} _LL11: if(( unsigned int) _temp5 >
2u?(( struct _tunion_struct*) _temp5)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL50:
_temp49=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp5)->f1; goto _LL12;}
else{ goto _LL13;} _LL13: if(( unsigned int) _temp5 > 2u?(( struct
_tunion_struct*) _temp5)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL52: _temp51=(
struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp5)->f1; goto
_LL14;} else{ goto _LL15;} _LL15: if(( int) _temp5 == Cyc_Absyn_Null_p){ goto
_LL16;} else{ goto _LL17;} _LL17: if(( int) _temp5 == Cyc_Absyn_Wild_p){ goto
_LL18;} else{ goto _LL19;} _LL19: if(( unsigned int) _temp5 > 2u?(( struct
_tunion_struct*) _temp5)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL54: _temp53=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*) _temp5)->f1; goto
_LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp5 > 2u?(( struct
_tunion_struct*) _temp5)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL56: _temp55=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*) _temp5)->f1;
goto _LL22;} else{ goto _LL23;} _LL23: if(( unsigned int) _temp5 > 2u?(( struct
_tunion_struct*) _temp5)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL58: _temp57=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*) _temp5)->f1; goto
_LL24;} else{ goto _LL25;} _LL25: if(( unsigned int) _temp5 > 2u?(( struct
_tunion_struct*) _temp5)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL60: _temp59=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp5)->f1; goto
_LL26;} else{ goto _LL27;} _LL27: if(( unsigned int) _temp5 > 2u?(( struct
_tunion_struct*) _temp5)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL69: _temp68=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*) _temp5)->f1;
goto _LL66; _LL66: _temp65=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp5)->f2; _temp67=&(( struct Cyc_Absyn_Struct_p_struct*) _temp5)->f2; goto
_LL64; _LL64: _temp63=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp5)->f3; goto _LL62; _LL62: _temp61=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp5)->f4; goto _LL28;} else{ goto _LL29;} _LL29: if(( unsigned int) _temp5 >
2u?(( struct _tunion_struct*) _temp5)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL81:
_temp80=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp5)->f1;
goto _LL79; _LL79: _temp78=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp5)->f2; goto _LL77; _LL77: _temp76=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp5)->f3; goto _LL75; _LL75: _temp74=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp5)->f4; goto _LL73; _LL73: _temp72=( struct Cyc_Absyn_Tuniondecl*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp5)->f5; goto _LL71; _LL71: _temp70=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp5)->f6; goto _LL30;} else{ goto _LL31;}
_LL31: if(( unsigned int) _temp5 > 2u?(( struct _tunion_struct*) _temp5)->tag ==
Cyc_Absyn_XTunion_p_tag: 0){ _LL91: _temp90=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp5)->f1; goto _LL89; _LL89: _temp88=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp5)->f2; goto _LL87; _LL87: _temp86=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp5)->f3; goto _LL85; _LL85: _temp84=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp5)->f4; goto _LL83; _LL83: _temp82=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp5)->f5;
goto _LL32;} else{ goto _LL33;} _LL33: if(( unsigned int) _temp5 > 2u?(( struct
_tunion_struct*) _temp5)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL97: _temp96=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp5)->f1; goto _LL95;
_LL95: _temp94=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp5)->f2; goto _LL93; _LL93: _temp92=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Absyn_Enum_p_struct*) _temp5)->f3; goto _LL34;} else{ goto _LL35;} _LL35:
if(( unsigned int) _temp5 > 2u?(( struct _tunion_struct*) _temp5)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL99: _temp98=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp5)->f1; goto _LL36;} else{ goto _LL37;} _LL37: if(( unsigned int) _temp5 >
2u?(( struct _tunion_struct*) _temp5)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){
_LL105: _temp104=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp5)->f1; goto _LL103; _LL103: _temp102=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp5)->f2; goto _LL101; _LL101: _temp100=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp5)->f3; goto _LL38;} else{ goto _LL39;} _LL39: if(( unsigned int) _temp5 >
2u?(( struct _tunion_struct*) _temp5)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){
_LL111: _temp110=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp5)->f1; goto _LL109; _LL109: _temp108=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp5)->f2; goto _LL107; _LL107: _temp106=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp5)->f3; goto _LL40;} else{ goto _LL6;} _LL8: t= Cyc_Tcpat_num_type( topt,
Cyc_Absyn_uint_t); goto _LL6; _LL10: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t);
goto _LL6; _LL12: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto _LL6;
_LL14: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_float_t); goto _LL6; _LL16: if(
topt != 0){ void* _temp113= Cyc_Tcutil_compress(({ void** _temp112= topt; if(
_temp112 == 0){ _throw( Null_Exception);}* _temp112;})); struct Cyc_Absyn_PtrInfo
_temp119; _LL115: if(( unsigned int) _temp113 > 4u?(( struct _tunion_struct*)
_temp113)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL120: _temp119=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp113)->f1; goto _LL116;} else{ goto
_LL117;} _LL117: goto _LL118; _LL116: t=({ void** _temp121= topt; if( _temp121
== 0){ _throw( Null_Exception);}* _temp121;}); goto tcpat_end; _LL118: goto
_LL114; _LL114:;} t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp122=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp122[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp123; _temp123.tag= Cyc_Absyn_PointerType_tag;
_temp123.f1=({ struct Cyc_Absyn_PtrInfo _temp124; _temp124.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp124.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp124.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp124.tq= Cyc_Absyn_empty_tqual(); _temp124.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp124;}); _temp123;});
_temp122;}); goto _LL6; _LL18: t= Cyc_Tcpat_any_type( topt); goto _LL6; _LL20: t=
Cyc_Tcpat_any_type( topt); Cyc_Tcpat_set_vd( _temp53,& v_result, t); goto _LL6;
_LL22: t= Cyc_Tcpat_any_type( topt); if( region_opt == 0){ Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp125=( char*)"* pattern would point into an unknown region";
struct _tagged_string _temp126; _temp126.curr= _temp125; _temp126.base= _temp125;
_temp126.last_plus_one= _temp125 + 45; _temp126;})); goto _LL6;} Cyc_Tcpat_set_vd(
_temp55,& v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp127=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp127[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp128; _temp128.tag= Cyc_Absyn_PointerType_tag;
_temp128.f1=({ struct Cyc_Absyn_PtrInfo _temp129; _temp129.elt_typ=( void*) t;
_temp129.rgn_typ=( void*)({ void** _temp130= region_opt; if( _temp130 == 0){
_throw( Null_Exception);}* _temp130;}); _temp129.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp129.tq= Cyc_Absyn_empty_tqual();
_temp129.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp129;}); _temp128;}); _temp127;})); goto _LL6; _LL24: { void* inner_typ=(
void*) Cyc_Absyn_VoidType; void** inner_topt= 0; if( topt != 0){ void* _temp132=
Cyc_Tcutil_compress(({ void** _temp131= topt; if( _temp131 == 0){ _throw(
Null_Exception);}* _temp131;})); struct Cyc_Absyn_PtrInfo _temp138; struct Cyc_Absyn_Conref*
_temp140; struct Cyc_Absyn_Tqual* _temp142; struct Cyc_Absyn_Conref* _temp144;
void* _temp146; void* _temp148; _LL134: if(( unsigned int) _temp132 > 4u?((
struct _tunion_struct*) _temp132)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL139:
_temp138=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp132)->f1; _LL149: _temp148=( void*) _temp138.elt_typ; goto _LL147; _LL147:
_temp146=( void*) _temp138.rgn_typ; goto _LL145; _LL145: _temp144=( struct Cyc_Absyn_Conref*)
_temp138.nullable; goto _LL143; _LL143: _temp142=( struct Cyc_Absyn_Tqual*)
_temp138.tq; goto _LL141; _LL141: _temp140=( struct Cyc_Absyn_Conref*) _temp138.bounds;
goto _LL135;} else{ goto _LL136;} _LL136: goto _LL137; _LL135: inner_typ=
_temp148; inner_topt=( void**)& inner_typ; goto _LL133; _LL137: goto _LL133;
_LL133:;}{ void* ptr_rgn= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); struct
Cyc_List_List* _temp152; struct Cyc_List_List* _temp154; struct _tuple5 _temp150=
Cyc_Tcpat_tcPat( te, _temp57, inner_topt,( void**)& ptr_rgn); _LL155: _temp154=
_temp150.f1; goto _LL153; _LL153: _temp152= _temp150.f2; goto _LL151; _LL151:
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp154); v_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( v_result,
_temp152); if( inner_topt == 0){ void* _temp157= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp156= _temp57->topt; if( _temp156 == 0){ _throw(
Null_Exception);} _temp156->v;})); struct Cyc_Absyn_TunionFieldInfo _temp165;
struct Cyc_Absyn_Tunionfield* _temp167; struct Cyc_Absyn_Tuniondecl* _temp169;
struct _tuple1* _temp171; struct Cyc_List_List* _temp173; struct _tuple1*
_temp175; struct Cyc_Absyn_XTunionFieldInfo _temp177; struct Cyc_Absyn_Tunionfield*
_temp179; struct Cyc_Absyn_XTuniondecl* _temp181; struct _tuple1* _temp183;
struct _tuple1* _temp185; _LL159: if(( unsigned int) _temp157 > 4u?(( struct
_tunion_struct*) _temp157)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL166:
_temp165=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp157)->f1; _LL176: _temp175=( struct _tuple1*) _temp165.name; goto _LL174;
_LL174: _temp173=( struct Cyc_List_List*) _temp165.targs; goto _LL172; _LL172:
_temp171=( struct _tuple1*) _temp165.fname; goto _LL170; _LL170: _temp169=(
struct Cyc_Absyn_Tuniondecl*) _temp165.tud; goto _LL168; _LL168: _temp167=(
struct Cyc_Absyn_Tunionfield*) _temp165.tufd; goto _LL160;} else{ goto _LL161;}
_LL161: if(( unsigned int) _temp157 > 4u?(( struct _tunion_struct*) _temp157)->tag
== Cyc_Absyn_XTunionFieldType_tag: 0){ _LL178: _temp177=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp157)->f1; _LL186: _temp185=(
struct _tuple1*) _temp177.name; goto _LL184; _LL184: _temp183=( struct _tuple1*)
_temp177.fname; goto _LL182; _LL182: _temp181=( struct Cyc_Absyn_XTuniondecl*)
_temp177.xtud; goto _LL180; _LL180: _temp179=( struct Cyc_Absyn_Tunionfield*)
_temp177.xtufd; goto _LL162;} else{ goto _LL163;} _LL163: goto _LL164; _LL160: t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp187=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp187[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp188; _temp188.tag= Cyc_Absyn_TunionType_tag;
_temp188.f1=({ struct Cyc_Absyn_TunionInfo _temp189; _temp189.name= _temp175;
_temp189.targs= _temp173; _temp189.rgn=( void*) ptr_rgn; _temp189.tud= _temp169;
_temp189;}); _temp188;}); _temp187;}); goto _LL158; _LL162: t=( void*)({ struct
Cyc_Absyn_XTunionType_struct* _temp190=( struct Cyc_Absyn_XTunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct)); _temp190[ 0]=({ struct
Cyc_Absyn_XTunionType_struct _temp191; _temp191.tag= Cyc_Absyn_XTunionType_tag;
_temp191.f1=({ struct Cyc_Absyn_XTunionInfo _temp192; _temp192.name= _temp185;
_temp192.rgn=( void*) ptr_rgn; _temp192.xtud= _temp181; _temp192;}); _temp191;});
_temp190;}); goto _LL158; _LL164: t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp193=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp193[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp194; _temp194.tag= Cyc_Absyn_PointerType_tag;
_temp194.f1=({ struct Cyc_Absyn_PtrInfo _temp195; _temp195.elt_typ=( void*)((
void*)({ struct Cyc_Core_Opt* _temp196= _temp57->topt; if( _temp196 == 0){
_throw( Null_Exception);} _temp196->v;})); _temp195.rgn_typ=( void*) ptr_rgn;
_temp195.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp195.tq= Cyc_Absyn_empty_tqual(); _temp195.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp195;}); _temp194;}); _temp193;}); goto _LL158;
_LL158:;} else{ t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp197=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp197[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp198; _temp198.tag= Cyc_Absyn_PointerType_tag;
_temp198.f1=({ struct Cyc_Absyn_PtrInfo _temp199; _temp199.elt_typ=( void*)((
void*)({ struct Cyc_Core_Opt* _temp200= _temp57->topt; if( _temp200 == 0){
_throw( Null_Exception);} _temp200->v;})); _temp199.rgn_typ=( void*) ptr_rgn;
_temp199.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp199.tq= Cyc_Absyn_empty_tqual(); _temp199.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp199;}); _temp198;}); _temp197;});} goto _LL6;}}
_LL26: { struct Cyc_List_List* pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if(
topt != 0){ void* _temp202= Cyc_Tcutil_compress(({ void** _temp201= topt; if(
_temp201 == 0){ _throw( Null_Exception);}* _temp201;})); struct Cyc_List_List*
_temp208; _LL204: if(( unsigned int) _temp202 > 4u?(( struct _tunion_struct*)
_temp202)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL209: _temp208=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp202)->f1; goto _LL205;} else{ goto
_LL206;} _LL206: goto _LL207; _LL205: topt_ts= _temp208; goto _LL203; _LL207:
goto _LL203; _LL203:;} for( 0; _temp59 != 0; _temp59=({ struct Cyc_List_List*
_temp210= _temp59; if( _temp210 == 0){ _throw( Null_Exception);} _temp210->tl;})){
void** inner_topt= 0; if( topt_ts != 0){ inner_topt=( void**)&(*(( struct
_tuple7*)({ struct Cyc_List_List* _temp211= topt_ts; if( _temp211 == 0){ _throw(
Null_Exception);} _temp211->hd;}))).f2; topt_ts=({ struct Cyc_List_List*
_temp212= topt_ts; if( _temp212 == 0){ _throw( Null_Exception);} _temp212->tl;});}{
struct Cyc_List_List* _temp216; struct Cyc_List_List* _temp218; struct _tuple5
_temp214= Cyc_Tcpat_tcPat( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp213= _temp59; if( _temp213 == 0){ _throw( Null_Exception);} _temp213->hd;}),
inner_topt, region_opt); _LL219: _temp218= _temp214.f1; goto _LL217; _LL217:
_temp216= _temp214.f2; goto _LL215; _LL215: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp218); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp216); pat_ts=({ struct Cyc_List_List*
_temp220=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp220->hd=( void*)({ struct _tuple7* _temp221=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp221->f1= Cyc_Absyn_empty_tqual(); _temp221->f2=(
void*)({ struct Cyc_Core_Opt* _temp223=(( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp222= _temp59; if( _temp222 == 0){ _throw( Null_Exception);} _temp222->hd;}))->topt;
if( _temp223 == 0){ _throw( Null_Exception);} _temp223->v;}); _temp221;});
_temp220->tl= pat_ts; _temp220;});}} t=( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp224=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp224[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp225; _temp225.tag= Cyc_Absyn_TupleType_tag;
_temp225.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
pat_ts); _temp225;}); _temp224;}); goto _LL6;} _LL28: if( _temp63 != 0){( void)
_throw((( struct _xtunion_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp226=( char*)"tcPat: struct<...> not implemented";
struct _tagged_string _temp227; _temp227.curr= _temp226; _temp227.base= _temp226;
_temp227.last_plus_one= _temp226 + 35; _temp227;})));}{ struct Cyc_List_List*
dps0= _temp61; for( 0; dps0 != 0; dps0=({ struct Cyc_List_List* _temp228= dps0;
if( _temp228 == 0){ _throw( Null_Exception);} _temp228->tl;})){ struct Cyc_List_List*
_temp232; struct Cyc_List_List* _temp234; struct _tuple5 _temp230= Cyc_Tcpat_tcPat(
te,(*(( struct _tuple6*)({ struct Cyc_List_List* _temp229= dps0; if( _temp229 ==
0){ _throw( Null_Exception);} _temp229->hd;}))).f2, 0, region_opt); _LL235:
_temp234= _temp230.f1; goto _LL233; _LL233: _temp232= _temp230.f2; goto _LL231;
_LL231: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp234); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp232);}}{ struct Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_make_inst_var, _temp68->tvs); struct Cyc_List_List* all_typs=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, all_inst);* _temp67=({
struct Cyc_Core_Opt* _temp236=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp236->v=( void*) all_typs; _temp236;}); t=( void*)({ struct
Cyc_Absyn_StructType_struct* _temp237=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp237[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp238; _temp238.tag= Cyc_Absyn_StructType_tag;
_temp238.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp239= _temp68->name; if( _temp239 == 0){ _throw( Null_Exception);} _temp239->v;}));
_temp238.f2= all_typs; _temp238.f3=({ struct Cyc_Absyn_Structdecl** _temp240=(
struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp240[ 0]= _temp68; _temp240;}); _temp238;}); _temp237;});{ struct Cyc_List_List*
fields=(( struct Cyc_List_List*(*)( struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)( p->loc,
_temp61, _temp68); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp241=
fields; if( _temp241 == 0){ _throw( Null_Exception);} _temp241->tl;})){ struct
_tuple8 _temp245; struct Cyc_Absyn_Pat* _temp246; struct Cyc_Absyn_Structfield*
_temp248; struct _tuple8* _temp243=( struct _tuple8*)({ struct Cyc_List_List*
_temp242= fields; if( _temp242 == 0){ _throw( Null_Exception);} _temp242->hd;});
_temp245=* _temp243; _LL249: _temp248= _temp245.f1; goto _LL247; _LL247:
_temp246= _temp245.f2; goto _LL244; _LL244: { void* inst_fieldtyp= Cyc_Tcutil_substitute(
all_inst,( void*) _temp248->type); if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt*
_temp250= _temp246->topt; if( _temp250 == 0){ _throw( Null_Exception);} _temp250->v;}),
inst_fieldtyp)){ Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp252=*
_temp248->name; struct _tagged_string _temp253= Cyc_Absynpp_typ2string(
inst_fieldtyp); struct _tagged_string _temp254= Cyc_Absynpp_typ2string(( void*)({
struct Cyc_Core_Opt* _temp251= _temp246->topt; if( _temp251 == 0){ _throw(
Null_Exception);} _temp251->v;})); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp252.last_plus_one - _temp252.curr, _temp252.curr, _temp253.last_plus_one -
_temp253.curr, _temp253.curr, _temp254.last_plus_one - _temp254.curr, _temp254.curr);}));}}}
goto _LL6;}} _LL30: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp76) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp70->tvs)){
Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char* _temp255=( char*)"wrong number of existential type variables in pattern";
struct _tagged_string _temp256; _temp256.curr= _temp255; _temp256.base= _temp255;
_temp256.last_plus_one= _temp255 + 54; _temp256;}));}{ struct Cyc_List_List* t=
_temp76; for( 0; t != 0; t=({ struct Cyc_List_List* _temp257= t; if( _temp257 ==
0){ _throw( Null_Exception);} _temp257->tl;})){ struct Cyc_Absyn_Tvar* tv=(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp258= t; if( _temp258 == 0){
_throw( Null_Exception);} _temp258->hd;});(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( tv->kind);{ void* _temp259=(
void*)( tv->kind)->v; void* _temp269; void* _temp271; struct Cyc_Absyn_Conref*
_temp273; _LL261: if(( unsigned int) _temp259 > 1u?(( struct _tunion_struct*)
_temp259)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL270: _temp269=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp259)->f1; if(( int) _temp269 == Cyc_Absyn_BoxKind){
goto _LL262;} else{ goto _LL263;}} else{ goto _LL263;} _LL263: if(( unsigned int)
_temp259 > 1u?(( struct _tunion_struct*) _temp259)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL272: _temp271=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp259)->f1;
goto _LL264;} else{ goto _LL265;} _LL265: if(( int) _temp259 == Cyc_Absyn_No_constr){
goto _LL266;} else{ goto _LL267;} _LL267: if(( unsigned int) _temp259 > 1u?((
struct _tunion_struct*) _temp259)->tag == Cyc_Absyn_Forward_constr_tag: 0){
_LL274: _temp273=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp259)->f1; goto _LL268;} else{ goto _LL260;} _LL262: goto _LL260; _LL264:
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp275= Cyc_Absynpp_qvar2string(
_temp80); struct _tagged_string _temp276=* tv->name; struct _tagged_string
_temp277= Cyc_Absynpp_kind2string( _temp271); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp275.last_plus_one - _temp275.curr, _temp275.curr, _temp276.last_plus_one -
_temp276.curr, _temp276.curr, _temp277.last_plus_one - _temp277.curr, _temp277.curr);}));
goto _LL260; _LL266:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp278=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp278[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp279; _temp279.tag= Cyc_Absyn_Eq_constr_tag;
_temp279.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp279;}); _temp278;})));
goto _LL260; _LL268:( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp280=(
char*)"forward after compress in conref"; struct _tagged_string _temp281;
_temp281.curr= _temp280; _temp281.base= _temp280; _temp281.last_plus_one=
_temp280 + 33; _temp281;}))); _LL260:;}}} tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp76);{ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te,
_temp76); struct Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_make_inst_var, _temp72->tvs); struct Cyc_List_List* all_typs=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, all_inst); struct Cyc_List_List*
tqts= _temp70->typs; if( tqts == 0){ t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp282=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp282[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp283; _temp283.tag= Cyc_Absyn_TunionType_tag;
_temp283.f1=({ struct Cyc_Absyn_TunionInfo _temp284; _temp284.name=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp285= _temp72->name; if( _temp285 == 0){
_throw( Null_Exception);} _temp285->v;}); _temp284.targs= all_typs; _temp284.rgn=(
void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp284.tud=( struct Cyc_Absyn_Tuniondecl*)
_temp72; _temp284;}); _temp283;}); _temp282;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp286=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp286[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp287; _temp287.tag= Cyc_Absyn_TunionFieldType_tag; _temp287.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp288; _temp288.name=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp289=
_temp72->name; if( _temp289 == 0){ _throw( Null_Exception);} _temp289->v;});
_temp288.targs= all_typs; _temp288.fname= _temp70->name; _temp288.tud=( struct
Cyc_Absyn_Tuniondecl*) _temp72; _temp288.tufd=( struct Cyc_Absyn_Tunionfield*)
_temp70; _temp288;}); _temp287;}); _temp286;});} for( 0; _temp74 != 0? tqts != 0:
0; _temp74=({ struct Cyc_List_List* _temp290= _temp74; if( _temp290 == 0){
_throw( Null_Exception);} _temp290->tl;}), tqts=({ struct Cyc_List_List*
_temp291= tqts; if( _temp291 == 0){ _throw( Null_Exception);} _temp291->tl;})){
struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp292= _temp74; if( _temp292 == 0){ _throw( Null_Exception);} _temp292->hd;});
struct Cyc_List_List* _temp295; struct Cyc_List_List* _temp297; struct _tuple5
_temp293= Cyc_Tcpat_tcPat( te2, p2, 0, region_opt); _LL298: _temp297= _temp293.f1;
goto _LL296; _LL296: _temp295= _temp293.f2; goto _LL294; _LL294: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp297); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp295);{ void*
field_typ= Cyc_Tcutil_substitute( all_inst,(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp299= tqts; if( _temp299 == 0){ _throw( Null_Exception);} _temp299->hd;}))).f2);
if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp300= p2->topt; if(
_temp300 == 0){ _throw( Null_Exception);} _temp300->v;}), field_typ)){ Cyc_Tcutil_terr(
p2->loc,({ struct _tagged_string _temp302= Cyc_Absynpp_qvar2string( _temp80);
struct _tagged_string _temp303= Cyc_Absynpp_typ2string( field_typ); struct
_tagged_string _temp304= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp301= p2->topt; if( _temp301 == 0){ _throw( Null_Exception);} _temp301->v;}));
xprintf("%.*s expects argument type %.*s, not %.*s", _temp302.last_plus_one -
_temp302.curr, _temp302.curr, _temp303.last_plus_one - _temp303.curr, _temp303.curr,
_temp304.last_plus_one - _temp304.curr, _temp304.curr);}));}}} if( _temp74 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp305= Cyc_Absynpp_qvar2string(
_temp80); xprintf("too many arguments for tunion constructor %.*s", _temp305.last_plus_one
- _temp305.curr, _temp305.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp306= Cyc_Absynpp_qvar2string( _temp80); xprintf("too few arguments for tunion constructor %.*s",
_temp306.last_plus_one - _temp306.curr, _temp306.curr);}));} goto _LL6;} _LL32:
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp88) !=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp82->tvs)){ Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp307=( char*)"wrong number of existential type variables in pattern";
struct _tagged_string _temp308; _temp308.curr= _temp307; _temp308.base= _temp307;
_temp308.last_plus_one= _temp307 + 54; _temp308;}));}{ struct Cyc_Tcenv_Tenv*
te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp88); tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp88);{ struct Cyc_List_List* tqts= _temp82->typs; t=( void*)({ struct Cyc_Absyn_XTunionFieldType_struct*
_temp309=( struct Cyc_Absyn_XTunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_XTunionFieldType_struct)); _temp309[ 0]=({ struct Cyc_Absyn_XTunionFieldType_struct
_temp310; _temp310.tag= Cyc_Absyn_XTunionFieldType_tag; _temp310.f1=({ struct
Cyc_Absyn_XTunionFieldInfo _temp311; _temp311.name= _temp84->name; _temp311.fname=
_temp82->name; _temp311.xtud=( struct Cyc_Absyn_XTuniondecl*) _temp84; _temp311.xtufd=(
struct Cyc_Absyn_Tunionfield*) _temp82; _temp311;}); _temp310;}); _temp309;});
for( 0; _temp86 != 0? tqts != 0: 0; _temp86=({ struct Cyc_List_List* _temp312=
_temp86; if( _temp312 == 0){ _throw( Null_Exception);} _temp312->tl;}), tqts=({
struct Cyc_List_List* _temp313= tqts; if( _temp313 == 0){ _throw( Null_Exception);}
_temp313->tl;})){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp314= _temp86; if( _temp314 == 0){ _throw( Null_Exception);} _temp314->hd;});
struct Cyc_List_List* _temp317; struct Cyc_List_List* _temp319; struct _tuple5
_temp315= Cyc_Tcpat_tcPat( te2, p2, 0, region_opt); _LL320: _temp319= _temp315.f1;
goto _LL318; _LL318: _temp317= _temp315.f2; goto _LL316; _LL316: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp319); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp317);{ void*
field_typ=(*(( struct _tuple7*)({ struct Cyc_List_List* _temp321= tqts; if(
_temp321 == 0){ _throw( Null_Exception);} _temp321->hd;}))).f2; if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp322= p2->topt; if( _temp322 == 0){ _throw(
Null_Exception);} _temp322->v;}), field_typ)){ Cyc_Tcutil_terr( p2->loc,({
struct _tagged_string _temp324= Cyc_Absynpp_qvar2string( _temp90); struct
_tagged_string _temp325= Cyc_Absynpp_typ2string( field_typ); struct
_tagged_string _temp326= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp323= p2->topt; if( _temp323 == 0){ _throw( Null_Exception);} _temp323->v;}));
xprintf("%.*s expects argument type %.*s, not %.*s", _temp324.last_plus_one -
_temp324.curr, _temp324.curr, _temp325.last_plus_one - _temp325.curr, _temp325.curr,
_temp326.last_plus_one - _temp326.curr, _temp326.curr);}));}}} if( _temp86 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp327= Cyc_Absynpp_qvar2string(
_temp90); xprintf("too many arguments for xtunion constructor %.*s", _temp327.last_plus_one
- _temp327.curr, _temp327.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp328= Cyc_Absynpp_qvar2string( _temp90); xprintf("too few arguments for xtunion constructor %.*s",
_temp328.last_plus_one - _temp328.curr, _temp328.curr);}));} goto _LL6;}} _LL34:
t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp329=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp329[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp330; _temp330.tag= Cyc_Absyn_EnumType_tag;
_temp330.f1= _temp94->name; _temp330.f2=( struct Cyc_Absyn_Enumdecl*) _temp94;
_temp330;}); _temp329;}); goto _LL6; _LL36:{ struct _handler_cons _temp331;
_push_handler(& _temp331);{ struct _xtunion_struct* _temp332=( struct
_xtunion_struct*) setjmp( _temp331.handler); if( ! _temp332){{ void* _temp333=
Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp98); struct Cyc_Absyn_Structdecl*
_temp345; struct Cyc_Absyn_Tunionfield* _temp347; struct Cyc_Absyn_Tuniondecl*
_temp349; struct Cyc_Absyn_Tunionfield* _temp351; struct Cyc_Absyn_XTuniondecl*
_temp353; struct Cyc_Absyn_Enumfield* _temp355; struct Cyc_Absyn_Enumdecl*
_temp357; void* _temp359; _LL335: if((( struct _tunion_struct*) _temp333)->tag
== Cyc_Tcenv_StructRes_tag){ _LL346: _temp345=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp333)->f1; goto _LL336;} else{ goto
_LL337;} _LL337: if((( struct _tunion_struct*) _temp333)->tag == Cyc_Tcenv_TunionRes_tag){
_LL350: _temp349=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp333)->f1; goto _LL348; _LL348: _temp347=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp333)->f2; goto _LL338;} else{ goto
_LL339;} _LL339: if((( struct _tunion_struct*) _temp333)->tag == Cyc_Tcenv_XTunionRes_tag){
_LL354: _temp353=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp333)->f1; goto _LL352; _LL352: _temp351=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp333)->f2; goto _LL340;} else{ goto
_LL341;} _LL341: if((( struct _tunion_struct*) _temp333)->tag == Cyc_Tcenv_EnumRes_tag){
_LL358: _temp357=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp333)->f1; goto _LL356; _LL356: _temp355=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp333)->f2; goto _LL342;} else{ goto _LL343;}
_LL343: if((( struct _tunion_struct*) _temp333)->tag == Cyc_Tcenv_VarRes_tag){
_LL360: _temp359=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp333)->f1; goto
_LL344;} else{ goto _LL334;} _LL336: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp361=( char*)"struct tag used without arguments in pattern";
struct _tagged_string _temp362; _temp362.curr= _temp361; _temp362.base= _temp361;
_temp362.last_plus_one= _temp361 + 45; _temp362;})); t= Cyc_Absyn_wildtyp();
_npop_handler( 0u); goto tcpat_end; _LL338:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_p_struct* _temp363=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp363[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp364; _temp364.tag= Cyc_Absyn_Tunion_p_tag;
_temp364.f1= _temp347->name; _temp364.f2= 0; _temp364.f3= 0; _temp364.f4= 0;
_temp364.f5= _temp349; _temp364.f6= _temp347; _temp364;}); _temp363;})));{
struct _tuple5 _temp365= Cyc_Tcpat_tcPat( te, p, topt, region_opt);
_npop_handler( 0u); return _temp365;} _LL340:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_XTunion_p_struct* _temp366=( struct Cyc_Absyn_XTunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct)); _temp366[ 0]=({ struct
Cyc_Absyn_XTunion_p_struct _temp367; _temp367.tag= Cyc_Absyn_XTunion_p_tag;
_temp367.f1= _temp351->name; _temp367.f2= 0; _temp367.f3= 0; _temp367.f4=
_temp353; _temp367.f5= _temp351; _temp367;}); _temp366;})));{ struct _tuple5
_temp368= Cyc_Tcpat_tcPat( te, p, topt, region_opt); _npop_handler( 0u); return
_temp368;} _LL342:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp369=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp369[ 0]=({ struct Cyc_Absyn_Enum_p_struct _temp370; _temp370.tag= Cyc_Absyn_Enum_p_tag;
_temp370.f1= _temp355->name; _temp370.f2= _temp357; _temp370.f3= _temp355;
_temp370;}); _temp369;})));{ struct _tuple5 _temp371= Cyc_Tcpat_tcPat( te, p,
topt, region_opt); _npop_handler( 0u); return _temp371;} _LL344: goto _LL334;
_LL334:;}; _pop_handler();} else{ struct _xtunion_struct* _temp373= _temp332;
_LL375: if( _temp373->tag == Cyc_Dict_Absent_tag){ goto _LL376;} else{ goto
_LL377;} _LL377: goto _LL378; _LL376: goto _LL374; _LL378:( void) _throw(
_temp373); _LL374:;}}}{ void* _temp379=(* _temp98).f1; struct Cyc_List_List*
_temp387; _LL381: if(( int) _temp379 == Cyc_Absyn_Loc_n){ goto _LL382;} else{
goto _LL383;} _LL383: if(( unsigned int) _temp379 > 1u?(( struct _tunion_struct*)
_temp379)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL388: _temp387=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp379)->f1; if( _temp387 == 0){ goto _LL384;}
else{ goto _LL385;}} else{ goto _LL385;} _LL385: goto _LL386; _LL382: goto
_LL384; _LL384:(* _temp98).f1=( void*) Cyc_Absyn_Loc_n;( void*)( p->r=( void*)((
void*)({ struct Cyc_Absyn_Var_p_struct* _temp389=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp389[ 0]=({ struct Cyc_Absyn_Var_p_struct
_temp390; _temp390.tag= Cyc_Absyn_Var_p_tag; _temp390.f1= Cyc_Absyn_new_vardecl(
_temp98,( void*) Cyc_Absyn_VoidType, 0); _temp390;}); _temp389;}))); return Cyc_Tcpat_tcPat(
te, p, topt, region_opt); _LL386: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp391=( char*)"qualified variable in pattern"; struct
_tagged_string _temp392; _temp392.curr= _temp391; _temp392.base= _temp391;
_temp392.last_plus_one= _temp391 + 30; _temp392;})); t= Cyc_Absyn_wildtyp();
goto _LL380; _LL380:;} goto _LL6; _LL38:{ struct _handler_cons _temp393;
_push_handler(& _temp393);{ struct _xtunion_struct* _temp394=( struct
_xtunion_struct*) setjmp( _temp393.handler); if( ! _temp394){{ void* _temp395=
Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp104); struct Cyc_Absyn_Structdecl*
_temp407; struct Cyc_Absyn_Tunionfield* _temp409; struct Cyc_Absyn_Tuniondecl*
_temp411; struct Cyc_Absyn_Tunionfield* _temp413; struct Cyc_Absyn_XTuniondecl*
_temp415; struct Cyc_Absyn_Enumfield* _temp417; struct Cyc_Absyn_Enumdecl*
_temp419; void* _temp421; _LL397: if((( struct _tunion_struct*) _temp395)->tag
== Cyc_Tcenv_StructRes_tag){ _LL408: _temp407=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp395)->f1; goto _LL398;} else{ goto
_LL399;} _LL399: if((( struct _tunion_struct*) _temp395)->tag == Cyc_Tcenv_TunionRes_tag){
_LL412: _temp411=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp395)->f1; goto _LL410; _LL410: _temp409=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp395)->f2; goto _LL400;} else{ goto
_LL401;} _LL401: if((( struct _tunion_struct*) _temp395)->tag == Cyc_Tcenv_XTunionRes_tag){
_LL416: _temp415=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp395)->f1; goto _LL414; _LL414: _temp413=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp395)->f2; goto _LL402;} else{ goto
_LL403;} _LL403: if((( struct _tunion_struct*) _temp395)->tag == Cyc_Tcenv_EnumRes_tag){
_LL420: _temp419=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp395)->f1; goto _LL418; _LL418: _temp417=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp395)->f2; goto _LL404;} else{ goto _LL405;}
_LL405: if((( struct _tunion_struct*) _temp395)->tag == Cyc_Tcenv_VarRes_tag){
_LL422: _temp421=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp395)->f1; goto
_LL406;} else{ goto _LL396;} _LL398: { struct Cyc_List_List* x=(( struct Cyc_List_List*(*)(
struct _tuple6*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcpat_null_designator, _temp100);( void*)( p->r=( void*)(( void*)({ struct
Cyc_Absyn_Struct_p_struct* _temp423=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp423[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp424; _temp424.tag= Cyc_Absyn_Struct_p_tag;
_temp424.f1= _temp407; _temp424.f2= 0; _temp424.f3= _temp102; _temp424.f4= x;
_temp424;}); _temp423;})));{ struct _tuple5 _temp425= Cyc_Tcpat_tcPat( te, p,
topt, region_opt); _npop_handler( 0u); return _temp425;}} _LL400:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp426=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp426[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp427; _temp427.tag= Cyc_Absyn_Tunion_p_tag;
_temp427.f1= _temp409->name; _temp427.f2= 0; _temp427.f3= _temp102; _temp427.f4=
_temp100; _temp427.f5= _temp411; _temp427.f6= _temp409; _temp427;}); _temp426;})));{
struct _tuple5 _temp428= Cyc_Tcpat_tcPat( te, p, topt, region_opt);
_npop_handler( 0u); return _temp428;} _LL402:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_XTunion_p_struct* _temp429=( struct Cyc_Absyn_XTunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct)); _temp429[ 0]=({ struct
Cyc_Absyn_XTunion_p_struct _temp430; _temp430.tag= Cyc_Absyn_XTunion_p_tag;
_temp430.f1= _temp413->name; _temp430.f2= _temp102; _temp430.f3= _temp100;
_temp430.f4= _temp415; _temp430.f5= _temp413; _temp430;}); _temp429;})));{
struct _tuple5 _temp431= Cyc_Tcpat_tcPat( te, p, topt, region_opt);
_npop_handler( 0u); return _temp431;} _LL404: Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp432= Cyc_Absynpp_qvar2string( _temp104); xprintf("bad enum constructor %.*s in pattern",
_temp432.last_plus_one - _temp432.curr, _temp432.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL396; _LL406: goto _LL396; _LL396:;}; _pop_handler();} else{ struct
_xtunion_struct* _temp434= _temp394; _LL436: if( _temp434->tag == Cyc_Dict_Absent_tag){
goto _LL437;} else{ goto _LL438;} _LL438: goto _LL439; _LL437: goto _LL435;
_LL439:( void) _throw( _temp434); _LL435:;}}} Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp440= Cyc_Absynpp_qvar2string( _temp104); xprintf("%.*s is not a constructor in pattern",
_temp440.last_plus_one - _temp440.curr, _temp440.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL6; _LL40:{ struct _handler_cons _temp441; _push_handler(& _temp441);{
struct _xtunion_struct* _temp442=( struct _xtunion_struct*) setjmp( _temp441.handler);
if( ! _temp442){{ void* _temp443= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp110); struct Cyc_Absyn_Structdecl* _temp449; _LL445: if((( struct
_tunion_struct*) _temp443)->tag == Cyc_Tcenv_StructRes_tag){ _LL450: _temp449=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp443)->f1;
goto _LL446;} else{ goto _LL447;} _LL447: goto _LL448; _LL446:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp451=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp451[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp452; _temp452.tag= Cyc_Absyn_Struct_p_tag;
_temp452.f1= _temp449; _temp452.f2= 0; _temp452.f3= _temp108; _temp452.f4=
_temp106; _temp452;}); _temp451;})));{ struct _tuple5 _temp453= Cyc_Tcpat_tcPat(
te, p, topt, region_opt); _npop_handler( 0u); return _temp453;} _LL448: goto
_LL444; _LL444:;}; _pop_handler();} else{ struct _xtunion_struct* _temp455=
_temp442; _LL457: if( _temp455->tag == Cyc_Dict_Absent_tag){ goto _LL458;} else{
goto _LL459;} _LL459: goto _LL460; _LL458: goto _LL456; _LL460:( void) _throw(
_temp455); _LL456:;}}} Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp461=
Cyc_Absynpp_qvar2string( _temp110); xprintf("%.*s is not a struct name, but pattern has designators",
_temp461.last_plus_one - _temp461.curr, _temp461.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL6; _LL6:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt* _temp462=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp462->v=( void*) t;
_temp462;}); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct
_tagged_string*(* f)( struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcpat_get_name, v_result), p->loc,( struct _tagged_string)({ char* _temp463=(
char*)"pattern contains a repeated variable"; struct _tagged_string _temp464;
_temp464.curr= _temp463; _temp464.base= _temp463; _temp464.last_plus_one=
_temp463 + 37; _temp464;})); return({ struct _tuple5 _temp465; _temp465.f1=
tv_result; _temp465.f2= v_result; _temp465;});} void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p){ void* _temp466=( void*) p->r;
struct Cyc_Absyn_Pat* _temp480; struct Cyc_Absyn_Tunionfield* _temp482; struct
Cyc_Absyn_Tuniondecl* _temp484; struct Cyc_List_List* _temp486; struct Cyc_List_List*
_temp488; struct Cyc_Core_Opt* _temp490; struct _tuple1* _temp492; struct Cyc_Absyn_Tunionfield*
_temp494; struct Cyc_Absyn_XTuniondecl* _temp496; struct Cyc_List_List* _temp498;
struct Cyc_List_List* _temp500; struct _tuple1* _temp502; struct Cyc_List_List*
_temp504; struct Cyc_List_List* _temp506; struct Cyc_Core_Opt* _temp508; struct
Cyc_Absyn_Structdecl* _temp510; struct Cyc_List_List* _temp512; _LL468: if((
unsigned int) _temp466 > 2u?(( struct _tunion_struct*) _temp466)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL481: _temp480=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp466)->f1; goto _LL469;} else{ goto _LL470;} _LL470: if(( unsigned int)
_temp466 > 2u?(( struct _tunion_struct*) _temp466)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL493: _temp492=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp466)->f1; goto _LL491; _LL491: _temp490=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp466)->f2; goto _LL489; _LL489: _temp488=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp466)->f3; goto
_LL487; _LL487: _temp486=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp466)->f4; goto _LL485; _LL485: _temp484=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp466)->f5; goto _LL483; _LL483: _temp482=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp466)->f6;
goto _LL471;} else{ goto _LL472;} _LL472: if(( unsigned int) _temp466 > 2u?((
struct _tunion_struct*) _temp466)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL503:
_temp502=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp466)->f1;
goto _LL501; _LL501: _temp500=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp466)->f2; goto _LL499; _LL499: _temp498=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_p_struct*) _temp466)->f3; goto _LL497; _LL497: _temp496=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp466)->f4;
goto _LL495; _LL495: _temp494=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp466)->f5; goto _LL473;} else{ goto _LL474;} _LL474: if(( unsigned int)
_temp466 > 2u?(( struct _tunion_struct*) _temp466)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL511: _temp510=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp466)->f1; goto _LL509; _LL509: _temp508=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp466)->f2; goto _LL507; _LL507: _temp506=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp466)->f3; goto
_LL505; _LL505: _temp504=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp466)->f4; goto _LL475;} else{ goto _LL476;} _LL476: if(( unsigned int)
_temp466 > 2u?(( struct _tunion_struct*) _temp466)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL513: _temp512=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp466)->f1; goto _LL477;} else{ goto _LL478;} _LL478: goto _LL479; _LL469:
Cyc_Tcpat_check_pat_regions( te, _temp480);{ void* _temp515=( void*)({ struct
Cyc_Core_Opt* _temp514= p->topt; if( _temp514 == 0){ _throw( Null_Exception);}
_temp514->v;}); struct Cyc_Absyn_PtrInfo _temp525; struct Cyc_Absyn_Conref*
_temp527; struct Cyc_Absyn_Tqual* _temp529; struct Cyc_Absyn_Conref* _temp531;
void* _temp533; void* _temp535; struct Cyc_Absyn_TunionInfo _temp537; struct Cyc_Absyn_Tuniondecl*
_temp539; void* _temp541; struct Cyc_List_List* _temp543; struct _tuple1*
_temp545; struct Cyc_Absyn_XTunionInfo _temp547; struct Cyc_Absyn_XTuniondecl*
_temp549; void* _temp551; struct _tuple1* _temp553; _LL517: if(( unsigned int)
_temp515 > 4u?(( struct _tunion_struct*) _temp515)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL526: _temp525=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp515)->f1; _LL536: _temp535=( void*) _temp525.elt_typ; goto _LL534; _LL534:
_temp533=( void*) _temp525.rgn_typ; goto _LL532; _LL532: _temp531=( struct Cyc_Absyn_Conref*)
_temp525.nullable; goto _LL530; _LL530: _temp529=( struct Cyc_Absyn_Tqual*)
_temp525.tq; goto _LL528; _LL528: _temp527=( struct Cyc_Absyn_Conref*) _temp525.bounds;
goto _LL518;} else{ goto _LL519;} _LL519: if(( unsigned int) _temp515 > 4u?((
struct _tunion_struct*) _temp515)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL538:
_temp537=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp515)->f1; _LL546: _temp545=( struct _tuple1*) _temp537.name; goto _LL544;
_LL544: _temp543=( struct Cyc_List_List*) _temp537.targs; goto _LL542; _LL542:
_temp541=( void*) _temp537.rgn; goto _LL540; _LL540: _temp539=( struct Cyc_Absyn_Tuniondecl*)
_temp537.tud; goto _LL520;} else{ goto _LL521;} _LL521: if(( unsigned int)
_temp515 > 4u?(( struct _tunion_struct*) _temp515)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL548: _temp547=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp515)->f1; _LL554: _temp553=( struct _tuple1*) _temp547.name; goto _LL552;
_LL552: _temp551=( void*) _temp547.rgn; goto _LL550; _LL550: _temp549=( struct
Cyc_Absyn_XTuniondecl*) _temp547.xtud; goto _LL522;} else{ goto _LL523;} _LL523:
goto _LL524; _LL518: _temp541= _temp533; goto _LL520; _LL520: _temp551= _temp541;
goto _LL522; _LL522: Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp551);
return; _LL524:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp555=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp555[ 0]=({ struct Cyc_Core_Impossible_struct _temp556; _temp556.tag= Cyc_Core_Impossible_tag;
_temp556.f1=( struct _tagged_string)({ char* _temp557=( char*)"check_pat_regions: bad pointer type";
struct _tagged_string _temp558; _temp558.curr= _temp557; _temp558.base= _temp557;
_temp558.last_plus_one= _temp557 + 36; _temp558;}); _temp556;}); _temp555;}));
_LL516:;} _LL471: for( 0; _temp486 != 0; _temp486=({ struct Cyc_List_List*
_temp559= _temp486; if( _temp559 == 0){ _throw( Null_Exception);} _temp559->tl;})){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp560= _temp486; if( _temp560 == 0){ _throw( Null_Exception);} _temp560->hd;}));}{
void* _temp562=( void*)({ struct Cyc_Core_Opt* _temp561= p->topt; if( _temp561
== 0){ _throw( Null_Exception);} _temp561->v;}); struct Cyc_Absyn_TunionInfo
_temp570; struct Cyc_Absyn_Tuniondecl* _temp572; void* _temp574; struct Cyc_List_List*
_temp576; struct _tuple1* _temp578; struct Cyc_Absyn_TunionFieldInfo _temp580;
_LL564: if(( unsigned int) _temp562 > 4u?(( struct _tunion_struct*) _temp562)->tag
== Cyc_Absyn_TunionType_tag: 0){ _LL571: _temp570=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp562)->f1; _LL579: _temp578=( struct
_tuple1*) _temp570.name; goto _LL577; _LL577: _temp576=( struct Cyc_List_List*)
_temp570.targs; goto _LL575; _LL575: _temp574=( void*) _temp570.rgn; goto _LL573;
_LL573: _temp572=( struct Cyc_Absyn_Tuniondecl*) _temp570.tud; goto _LL565;}
else{ goto _LL566;} _LL566: if(( unsigned int) _temp562 > 4u?(( struct
_tunion_struct*) _temp562)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL581:
_temp580=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp562)->f1; goto _LL567;} else{ goto _LL568;} _LL568: goto _LL569; _LL565:
Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp574); return; _LL567: return;
_LL569:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp582=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp582[ 0]=({ struct Cyc_Core_Impossible_struct _temp583; _temp583.tag= Cyc_Core_Impossible_tag;
_temp583.f1=( struct _tagged_string)({ char* _temp584=( char*)"check_pat_regions: bad tunion type";
struct _tagged_string _temp585; _temp585.curr= _temp584; _temp585.base= _temp584;
_temp585.last_plus_one= _temp584 + 35; _temp585;}); _temp583;}); _temp582;}));
_LL563:;} _LL473: for( 0; _temp498 != 0; _temp498=({ struct Cyc_List_List*
_temp586= _temp498; if( _temp586 == 0){ _throw( Null_Exception);} _temp586->tl;})){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp587= _temp498; if( _temp587 == 0){ _throw( Null_Exception);} _temp587->hd;}));}{
void* _temp589=( void*)({ struct Cyc_Core_Opt* _temp588= p->topt; if( _temp588
== 0){ _throw( Null_Exception);} _temp588->v;}); struct Cyc_Absyn_XTunionInfo
_temp597; struct Cyc_Absyn_XTuniondecl* _temp599; void* _temp601; struct _tuple1*
_temp603; struct Cyc_Absyn_XTunionFieldInfo _temp605; _LL591: if(( unsigned int)
_temp589 > 4u?(( struct _tunion_struct*) _temp589)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL598: _temp597=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp589)->f1; _LL604: _temp603=( struct _tuple1*) _temp597.name; goto _LL602;
_LL602: _temp601=( void*) _temp597.rgn; goto _LL600; _LL600: _temp599=( struct
Cyc_Absyn_XTuniondecl*) _temp597.xtud; goto _LL592;} else{ goto _LL593;} _LL593:
if(( unsigned int) _temp589 > 4u?(( struct _tunion_struct*) _temp589)->tag ==
Cyc_Absyn_XTunionFieldType_tag: 0){ _LL606: _temp605=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp589)->f1; goto _LL594;} else{
goto _LL595;} _LL595: goto _LL596; _LL592: Cyc_Tcenv_check_rgn_accessible( te, p->loc,
_temp601); return; _LL594: return; _LL596:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp607=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp607[ 0]=({ struct
Cyc_Core_Impossible_struct _temp608; _temp608.tag= Cyc_Core_Impossible_tag;
_temp608.f1=( struct _tagged_string)({ char* _temp609=( char*)"check_pat_regions: bad xtunion type";
struct _tagged_string _temp610; _temp610.curr= _temp609; _temp610.base= _temp609;
_temp610.last_plus_one= _temp609 + 36; _temp610;}); _temp608;}); _temp607;}));
_LL590:;} _LL475: _temp512=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple6*)) Cyc_Core_snd, _temp504); goto _LL477; _LL477: for( 0; _temp512
!= 0; _temp512=({ struct Cyc_List_List* _temp611= _temp512; if( _temp611 == 0){
_throw( Null_Exception);} _temp611->tl;})){ Cyc_Tcpat_check_pat_regions( te,(
struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp612= _temp512; if( _temp612
== 0){ _throw( Null_Exception);} _temp612->hd;}));} return; _LL479: return;
_LL467:;} typedef void* Cyc_Tcpat_Name_value; static const int Cyc_Tcpat_Name_v_tag=
0; struct Cyc_Tcpat_Name_v_struct{ int tag; struct _tagged_string f1; } ; static
const int Cyc_Tcpat_Int_v_tag= 1; struct Cyc_Tcpat_Int_v_struct{ int tag; int f1;
} ; typedef void* Cyc_Tcpat_name_value_t; struct Cyc_Tcpat_Con_s{ void* name;
int arity; struct Cyc_Core_Opt* span; } ; typedef struct Cyc_Tcpat_Con_s* Cyc_Tcpat_con_t;
typedef void* Cyc_Tcpat_Simple_pat; static const unsigned int Cyc_Tcpat_Any= 0;
static const int Cyc_Tcpat_Con_tag= 0; struct Cyc_Tcpat_Con_struct{ int tag;
struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; typedef void* Cyc_Tcpat_simple_pat_t;
static int Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s*
c2){ void* _temp613=( void*) c1->name; struct _tagged_string _temp619; int
_temp621; _LL615: if((( struct _tunion_struct*) _temp613)->tag == Cyc_Tcpat_Name_v_tag){
_LL620: _temp619=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp613)->f1; goto _LL616;} else{ goto _LL617;} _LL617: if((( struct
_tunion_struct*) _temp613)->tag == Cyc_Tcpat_Int_v_tag){ _LL622: _temp621=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp613)->f1; goto _LL618;} else{ goto _LL614;}
_LL616: { void* _temp623=( void*) c2->name; struct _tagged_string _temp629; int
_temp631; _LL625: if((( struct _tunion_struct*) _temp623)->tag == Cyc_Tcpat_Name_v_tag){
_LL630: _temp629=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp623)->f1; goto _LL626;} else{ goto _LL627;} _LL627: if((( struct
_tunion_struct*) _temp623)->tag == Cyc_Tcpat_Int_v_tag){ _LL632: _temp631=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp623)->f1; goto _LL628;} else{ goto _LL624;}
_LL626: return Cyc_String_zstrcmp( _temp619, _temp629); _LL628: return - 1;
_LL624:;} _LL618: { void* _temp633=( void*) c2->name; struct _tagged_string
_temp639; int _temp641; _LL635: if((( struct _tunion_struct*) _temp633)->tag ==
Cyc_Tcpat_Name_v_tag){ _LL640: _temp639=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp633)->f1; goto _LL636;} else{ goto _LL637;} _LL637: if((( struct
_tunion_struct*) _temp633)->tag == Cyc_Tcpat_Int_v_tag){ _LL642: _temp641=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp633)->f1; goto _LL638;} else{ goto _LL634;}
_LL636: return 1; _LL638: return _temp621 - _temp641; _LL634:;} _LL614:;} static
struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct Cyc_Set_Set*(*)(
int(* comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*))) Cyc_Set_empty)(
Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt=( struct
Cyc_Core_Opt){.v=( void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt=(
struct Cyc_Core_Opt){.v=( void*) 2}; static struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static char _temp646[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={ 0u,( struct
_tagged_string){ _temp646, _temp646, _temp646 + 5u}}; static char _temp650[ 4u]="ptr";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={ 0u,( struct
_tagged_string){ _temp650, _temp650, _temp650 + 4u}}; static char _temp654[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,( struct
_tagged_string){ _temp654, _temp654, _temp654 + 6u}}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_null_name_value),.arity=
0,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt}; static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=&
Cyc_Tcpat_null_ptr_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp655=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp655->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp656=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp656[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp657; _temp657.tag= Cyc_Tcpat_Int_v_tag;
_temp657.f1= i; _temp657;}); _temp656;})); _temp655->arity= 0; _temp655->span= 0;
_temp655;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_string f){ return({ struct Cyc_Tcpat_Con_s* _temp658=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp658->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp659=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp659[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp660; _temp660.tag= Cyc_Tcpat_Name_v_tag; _temp660.f1= f; _temp660;});
_temp659;})); _temp658->arity= 0; _temp658->span= 0; _temp658;});} static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_char_con( char c){ return({ struct Cyc_Tcpat_Con_s*
_temp661=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp661->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp663=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp663[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp664; _temp664.tag= Cyc_Tcpat_Int_v_tag;
_temp664.f1=( int) c; _temp664;}); _temp663;})); _temp661->arity= 0; _temp661->span=({
struct Cyc_Core_Opt* _temp662=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp662->v=( void*) 256; _temp662;}); _temp661;});}
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp665=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp665->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp665->arity=
i; _temp665->span= Cyc_Tcpat_one_opt_ptr; _temp665;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp666=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp666[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp667; _temp667.tag= Cyc_Tcpat_Con_tag; _temp667.f1= Cyc_Tcpat_null_con;
_temp667.f2= 0; _temp667;}); _temp666;});} static void* Cyc_Tcpat_int_pat( int i){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp668=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp668[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp669; _temp669.tag= Cyc_Tcpat_Con_tag; _temp669.f1= Cyc_Tcpat_int_con( i);
_temp669.f2= 0; _temp669;}); _temp668;});} static void* Cyc_Tcpat_char_pat( char
c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp670=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp670[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp671; _temp671.tag= Cyc_Tcpat_Con_tag; _temp671.f1= Cyc_Tcpat_char_con( c);
_temp671.f2= 0; _temp671;}); _temp670;});} static void* Cyc_Tcpat_float_pat(
struct _tagged_string f){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp672=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp672[ 0]=({ struct Cyc_Tcpat_Con_struct _temp673; _temp673.tag= Cyc_Tcpat_Con_tag;
_temp673.f1= Cyc_Tcpat_float_con( f); _temp673.f2= 0; _temp673;}); _temp672;});}
static void* Cyc_Tcpat_null_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp674=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp674[ 0]=({ struct Cyc_Tcpat_Con_struct _temp675; _temp675.tag= Cyc_Tcpat_Con_tag;
_temp675.f1= Cyc_Tcpat_null_ptr_con; _temp675.f2=({ struct Cyc_List_List*
_temp676=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp676->hd=( void*) p; _temp676->tl= 0; _temp676;}); _temp675;}); _temp674;});}
static void* Cyc_Tcpat_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp677=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp677[ 0]=({ struct Cyc_Tcpat_Con_struct _temp678; _temp678.tag= Cyc_Tcpat_Con_tag;
_temp678.f1= Cyc_Tcpat_ptr_con; _temp678.f2=({ struct Cyc_List_List* _temp679=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp679->hd=(
void*) p; _temp679->tl= 0; _temp679;}); _temp678;}); _temp677;});} static void*
Cyc_Tcpat_tuple_pat( struct Cyc_List_List* ss){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp680=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp680[ 0]=({ struct Cyc_Tcpat_Con_struct _temp681; _temp681.tag= Cyc_Tcpat_Con_tag;
_temp681.f1= Cyc_Tcpat_tuple_con((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ss)); _temp681.f2= ss; _temp681;}); _temp680;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_string con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List*
ps){ struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp682=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp682->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp683=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp683[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp684; _temp684.tag= Cyc_Tcpat_Name_v_tag; _temp684.f1= con_name; _temp684;});
_temp683;})); _temp682->arity=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ps); _temp682->span= span; _temp682;}); return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp685=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp685[ 0]=({ struct Cyc_Tcpat_Con_struct _temp686; _temp686.tag= Cyc_Tcpat_Con_tag;
_temp686.f1= c; _temp686.f2= ps; _temp686;}); _temp685;});} static void* Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat* p){ void* s;{ void* _temp687=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp721; int _temp723; void* _temp725; char _temp727; struct _tagged_string
_temp729; struct Cyc_List_List* _temp731; struct Cyc_Absyn_Vardecl* _temp733;
struct Cyc_Absyn_Pat* _temp735; struct Cyc_List_List* _temp737; struct Cyc_List_List*
_temp739; struct Cyc_Core_Opt* _temp741; struct Cyc_Absyn_Structdecl* _temp743;
struct Cyc_Absyn_Tunionfield* _temp745; struct Cyc_Absyn_Tuniondecl* _temp747;
struct Cyc_List_List* _temp749; struct Cyc_List_List* _temp751; struct Cyc_Core_Opt*
_temp753; struct _tuple1* _temp755; struct Cyc_Absyn_Tunionfield* _temp757;
struct Cyc_Absyn_XTuniondecl* _temp759; struct Cyc_List_List* _temp761; struct
Cyc_List_List* _temp763; struct _tuple1* _temp765; struct Cyc_Absyn_Enumfield*
_temp767; struct Cyc_Absyn_Enumdecl* _temp769; struct _tuple1* _temp771; struct
_tuple1* _temp773; struct Cyc_List_List* _temp775; struct Cyc_List_List*
_temp777; struct _tuple1* _temp779; struct Cyc_List_List* _temp781; struct Cyc_List_List*
_temp783; struct _tuple1* _temp785; _LL689: if(( int) _temp687 == Cyc_Absyn_Wild_p){
goto _LL690;} else{ goto _LL691;} _LL691: if(( unsigned int) _temp687 > 2u?((
struct _tunion_struct*) _temp687)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL722:
_temp721=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*) _temp687)->f1;
goto _LL692;} else{ goto _LL693;} _LL693: if(( int) _temp687 == Cyc_Absyn_Null_p){
goto _LL694;} else{ goto _LL695;} _LL695: if(( unsigned int) _temp687 > 2u?((
struct _tunion_struct*) _temp687)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL726:
_temp725=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp687)->f1; goto _LL724;
_LL724: _temp723=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp687)->f2; goto
_LL696;} else{ goto _LL697;} _LL697: if(( unsigned int) _temp687 > 2u?(( struct
_tunion_struct*) _temp687)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL728: _temp727=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp687)->f1; goto _LL698;} else{ goto
_LL699;} _LL699: if(( unsigned int) _temp687 > 2u?(( struct _tunion_struct*)
_temp687)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL730: _temp729=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp687)->f1; goto _LL700;}
else{ goto _LL701;} _LL701: if(( unsigned int) _temp687 > 2u?(( struct
_tunion_struct*) _temp687)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL732: _temp731=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp687)->f1; goto
_LL702;} else{ goto _LL703;} _LL703: if(( unsigned int) _temp687 > 2u?(( struct
_tunion_struct*) _temp687)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL734:
_temp733=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp687)->f1; goto _LL704;} else{ goto _LL705;} _LL705: if(( unsigned int)
_temp687 > 2u?(( struct _tunion_struct*) _temp687)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL736: _temp735=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp687)->f1; goto _LL706;} else{ goto _LL707;} _LL707: if(( unsigned int)
_temp687 > 2u?(( struct _tunion_struct*) _temp687)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL744: _temp743=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp687)->f1; goto _LL742; _LL742: _temp741=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp687)->f2; goto _LL740; _LL740: _temp739=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp687)->f3; goto
_LL738; _LL738: _temp737=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp687)->f4; goto _LL708;} else{ goto _LL709;} _LL709: if(( unsigned int)
_temp687 > 2u?(( struct _tunion_struct*) _temp687)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL756: _temp755=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp687)->f1; goto _LL754; _LL754: _temp753=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp687)->f2; goto _LL752; _LL752: _temp751=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp687)->f3; goto
_LL750; _LL750: _temp749=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp687)->f4; goto _LL748; _LL748: _temp747=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp687)->f5; goto _LL746; _LL746: _temp745=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp687)->f6;
goto _LL710;} else{ goto _LL711;} _LL711: if(( unsigned int) _temp687 > 2u?((
struct _tunion_struct*) _temp687)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL766:
_temp765=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp687)->f1;
goto _LL764; _LL764: _temp763=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp687)->f2; goto _LL762; _LL762: _temp761=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_p_struct*) _temp687)->f3; goto _LL760; _LL760: _temp759=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp687)->f4;
goto _LL758; _LL758: _temp757=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp687)->f5; goto _LL712;} else{ goto _LL713;} _LL713: if(( unsigned int)
_temp687 > 2u?(( struct _tunion_struct*) _temp687)->tag == Cyc_Absyn_Enum_p_tag:
0){ _LL772: _temp771=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp687)->f1; goto _LL770; _LL770: _temp769=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp687)->f2; goto _LL768; _LL768: _temp767=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*) _temp687)->f3;
goto _LL714;} else{ goto _LL715;} _LL715: if(( unsigned int) _temp687 > 2u?((
struct _tunion_struct*) _temp687)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL774:
_temp773=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp687)->f1;
goto _LL716;} else{ goto _LL717;} _LL717: if(( unsigned int) _temp687 > 2u?((
struct _tunion_struct*) _temp687)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){
_LL780: _temp779=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp687)->f1; goto _LL778; _LL778: _temp777=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp687)->f2; goto _LL776; _LL776: _temp775=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp687)->f3;
goto _LL718;} else{ goto _LL719;} _LL719: if(( unsigned int) _temp687 > 2u?((
struct _tunion_struct*) _temp687)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){
_LL786: _temp785=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp687)->f1; goto _LL784; _LL784: _temp783=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp687)->f2; goto _LL782; _LL782: _temp781=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp687)->f3;
goto _LL720;} else{ goto _LL688;} _LL690: s=( void*) Cyc_Tcpat_Any; goto _LL688;
_LL692: s=( void*) Cyc_Tcpat_Any; goto _LL688; _LL694: s= Cyc_Tcpat_null_pat();
goto _LL688; _LL696: s= Cyc_Tcpat_int_pat( _temp723); goto _LL688; _LL698: s=
Cyc_Tcpat_char_pat( _temp727); goto _LL688; _LL700: s= Cyc_Tcpat_float_pat(
_temp729); goto _LL688; _LL702: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp731)); goto _LL688; _LL704: s=( void*) Cyc_Tcpat_Any; goto _LL688; _LL706: {
void* ss= Cyc_Tcpat_compile_pat( _temp735);{ void* _temp788= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp787= p->topt; if( _temp787 == 0){ _throw(
Null_Exception);} _temp787->v;})); struct Cyc_Absyn_PtrInfo _temp798; struct Cyc_Absyn_Conref*
_temp800; struct Cyc_Absyn_Tqual* _temp802; struct Cyc_Absyn_Conref* _temp804;
void* _temp806; void* _temp808; struct Cyc_Absyn_TunionInfo _temp810; struct Cyc_Absyn_XTunionInfo
_temp812; _LL790: if(( unsigned int) _temp788 > 4u?(( struct _tunion_struct*)
_temp788)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL799: _temp798=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp788)->f1; _LL809: _temp808=( void*)
_temp798.elt_typ; goto _LL807; _LL807: _temp806=( void*) _temp798.rgn_typ; goto
_LL805; _LL805: _temp804=( struct Cyc_Absyn_Conref*) _temp798.nullable; goto
_LL803; _LL803: _temp802=( struct Cyc_Absyn_Tqual*) _temp798.tq; goto _LL801;
_LL801: _temp800=( struct Cyc_Absyn_Conref*) _temp798.bounds; goto _LL791;}
else{ goto _LL792;} _LL792: if(( unsigned int) _temp788 > 4u?(( struct
_tunion_struct*) _temp788)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL811:
_temp810=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp788)->f1; goto _LL793;} else{ goto _LL794;} _LL794: if(( unsigned int)
_temp788 > 4u?(( struct _tunion_struct*) _temp788)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL813: _temp812=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp788)->f1; goto _LL795;} else{ goto _LL796;} _LL796: goto _LL797; _LL791: {
int is_nullable= 0; int still_working= 1; while( still_working) { void* _temp814=(
void*) _temp804->v; struct Cyc_Absyn_Conref* _temp822; int _temp824; _LL816: if((
unsigned int) _temp814 > 1u?(( struct _tunion_struct*) _temp814)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL823: _temp822=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp814)->f1; goto _LL817;} else{ goto _LL818;} _LL818: if(( int) _temp814 ==
Cyc_Absyn_No_constr){ goto _LL819;} else{ goto _LL820;} _LL820: if((
unsigned int) _temp814 > 1u?(( struct _tunion_struct*) _temp814)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL825: _temp824=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp814)->f1;
goto _LL821;} else{ goto _LL815;} _LL817:( void*)( _temp804->v=( void*)(( void*)
_temp822->v)); continue; _LL819:( void*)( _temp804->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp826=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp826[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp827; _temp827.tag= Cyc_Absyn_Eq_constr_tag;
_temp827.f1=( void*) 0; _temp827;}); _temp826;}))); is_nullable= 0;
still_working= 0; goto _LL815; _LL821: is_nullable=( int) _temp824;
still_working= 0; goto _LL815; _LL815:;} if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL789;} _LL793: s= ss; goto _LL789;
_LL795: s= ss; goto _LL789; _LL797: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp828=( char*)"pointer pattern does not have pointer type!";
struct _tagged_string _temp829; _temp829.curr= _temp828; _temp829.base= _temp828;
_temp829.last_plus_one= _temp828 + 44; _temp829;})); goto _LL789; _LL789:;} goto
_LL688;} _LL708: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp830= _temp743->fields; if(
_temp830 == 0){ _throw( Null_Exception);} _temp830->v;}); for( 0; fields != 0;
fields=({ struct Cyc_List_List* _temp831= fields; if( _temp831 == 0){ _throw(
Null_Exception);} _temp831->tl;})){ int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp832= fields; if( _temp832 == 0){ _throw(
Null_Exception);} _temp832->hd;}))->name,( struct _tagged_string)({ char*
_temp833=( char*)""; struct _tagged_string _temp834; _temp834.curr= _temp833;
_temp834.base= _temp833; _temp834.last_plus_one= _temp833 + 1; _temp834;})) == 0;{
struct Cyc_List_List* dlps0= _temp737; for( 0; ! found? dlps0 != 0: 0; dlps0=({
struct Cyc_List_List* _temp835= dlps0; if( _temp835 == 0){ _throw(
Null_Exception);} _temp835->tl;})){ struct _tuple6 _temp839; struct Cyc_Absyn_Pat*
_temp840; struct Cyc_List_List* _temp842; struct _tuple6* _temp837=( struct
_tuple6*)({ struct Cyc_List_List* _temp836= dlps0; if( _temp836 == 0){ _throw(
Null_Exception);} _temp836->hd;}); _temp839=* _temp837; _LL843: _temp842=
_temp839.f1; goto _LL841; _LL841: _temp840= _temp839.f2; goto _LL838; _LL838: {
struct Cyc_List_List* _temp844= _temp842; struct Cyc_List_List _temp850; struct
Cyc_List_List* _temp851; void* _temp853; struct _tagged_string* _temp855; _LL846:
if( _temp844 == 0){ goto _LL848;} else{ _temp850=* _temp844; _LL854: _temp853=(
void*) _temp850.hd; if((( struct _tunion_struct*) _temp853)->tag == Cyc_Absyn_FieldName_tag){
_LL856: _temp855=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp853)->f1; goto _LL852;} else{ goto _LL848;} _LL852: _temp851=( struct Cyc_List_List*)
_temp850.tl; if( _temp851 == 0){ goto _LL847;} else{ goto _LL848;}} _LL848: goto
_LL849; _LL847: if( Cyc_String_zstrptrcmp( _temp855,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp857= fields; if( _temp857 == 0){ _throw(
Null_Exception);} _temp857->hd;}))->name) == 0){ ps=({ struct Cyc_List_List*
_temp858=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp858->hd=( void*) Cyc_Tcpat_compile_pat( _temp840); _temp858->tl= ps;
_temp858;}); found= 1;} goto _LL845; _LL849:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp859=( char*)"bad designator(s)";
struct _tagged_string _temp860; _temp860.curr= _temp859; _temp860.base= _temp859;
_temp860.last_plus_one= _temp859 + 18; _temp860;})); goto _LL845; _LL845:;}}}
if( ! found){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp861=( char*)"bad designator"; struct _tagged_string
_temp862; _temp862.curr= _temp861; _temp862.base= _temp861; _temp862.last_plus_one=
_temp861 + 15; _temp862;}));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL688;}
_LL710: { int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp863= _temp747->fields; if(
_temp863 == 0){ _throw( Null_Exception);} _temp863->v;})); s= Cyc_Tcpat_con_pat(*(*
_temp745->name).f2,({ struct Cyc_Core_Opt* _temp864=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp864->v=( void*) span;
_temp864;}),(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp749)); goto
_LL688;} _LL712: s= Cyc_Tcpat_con_pat(*(* _temp757->name).f2, 0,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp761)); goto _LL688; _LL714: { int span=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp769->fields); s= Cyc_Tcpat_con_pat(*(* _temp767->name).f2,({
struct Cyc_Core_Opt* _temp865=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp865->v=( void*) span; _temp865;}), 0); goto _LL688;}
_LL716: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp866=( char*)"compile_pat: unknownid"; struct
_tagged_string _temp867; _temp867.curr= _temp866; _temp867.base= _temp866;
_temp867.last_plus_one= _temp866 + 23; _temp867;})); goto _LL688; _LL718: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp868=( char*)"compile_pat: unknowncall"; struct _tagged_string
_temp869; _temp869.curr= _temp868; _temp869.base= _temp868; _temp869.last_plus_one=
_temp868 + 25; _temp869;})); goto _LL688; _LL720: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp870=(
char*)"compile_pat: unknownfields"; struct _tagged_string _temp871; _temp871.curr=
_temp870; _temp871.base= _temp870; _temp871.last_plus_one= _temp870 + 27;
_temp871;})); goto _LL688; _LL688:;} return s;} typedef void* Cyc_Tcpat_Term_desc;
static const int Cyc_Tcpat_Pos_tag= 0; struct Cyc_Tcpat_Pos_struct{ int tag;
struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Tcpat_Neg_tag=
1; struct Cyc_Tcpat_Neg_struct{ int tag; struct Cyc_Set_Set* f1; } ; typedef
void* Cyc_Tcpat_term_desc_t; typedef struct Cyc_List_List* Cyc_Tcpat_context_t;
typedef void* Cyc_Tcpat_Access; static const unsigned int Cyc_Tcpat_Obj= 0;
static const int Cyc_Tcpat_Sel_tag= 0; struct Cyc_Tcpat_Sel_struct{ int tag; int
f1; void* f2; } ; typedef void* Cyc_Tcpat_access_t; typedef void* Cyc_Tcpat_Decision;
static const unsigned int Cyc_Tcpat_Failure= 0; static const int Cyc_Tcpat_Success_tag=
0; struct Cyc_Tcpat_Success_struct{ int tag; void* f1; } ; static const int Cyc_Tcpat_IfEq_tag=
1; struct Cyc_Tcpat_IfEq_struct{ int tag; void* f1; struct Cyc_Tcpat_Con_s* f2;
void* f3; void* f4; } ; typedef void* Cyc_Tcpat_decision_t; typedef struct
_tuple9* Cyc_Tcpat_work_stack_frame_t; typedef struct Cyc_List_List* Cyc_Tcpat_work_stack_t;
typedef void* Cyc_Tcpat_Answer; static const unsigned int Cyc_Tcpat_Yes= 0;
static const unsigned int Cyc_Tcpat_No= 1; static const unsigned int Cyc_Tcpat_Maybe=
2; typedef void* Cyc_Tcpat_answer_t; static void* Cyc_Tcpat_add_neg( void* td,
struct Cyc_Tcpat_Con_s* c){ void* _temp872= td; struct Cyc_Set_Set* _temp878;
struct Cyc_List_List* _temp880; struct Cyc_Tcpat_Con_s* _temp882; _LL874: if(((
struct _tunion_struct*) _temp872)->tag == Cyc_Tcpat_Neg_tag){ _LL879: _temp878=(
struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp872)->f1; goto _LL875;}
else{ goto _LL876;} _LL876: if((( struct _tunion_struct*) _temp872)->tag == Cyc_Tcpat_Pos_tag){
_LL883: _temp882=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*)
_temp872)->f1; goto _LL881; _LL881: _temp880=( struct Cyc_List_List*)(( struct
Cyc_Tcpat_Pos_struct*) _temp872)->f2; goto _LL877;} else{ goto _LL873;} _LL875:
if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp878, c)){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp884=( char*)"add_neg called when constructor already in set";
struct _tagged_string _temp885; _temp885.curr= _temp884; _temp885.base= _temp884;
_temp885.last_plus_one= _temp884 + 47; _temp885;}));} if( c->span != 0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp878) + 1 >=( int)({ struct
Cyc_Core_Opt* _temp886= c->span; if( _temp886 == 0){ _throw( Null_Exception);}
_temp886->v;}): 0){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp887=( char*)"add_neg called when |cs U {c}| >= span(c)";
struct _tagged_string _temp888; _temp888.curr= _temp887; _temp888.base= _temp887;
_temp888.last_plus_one= _temp887 + 42; _temp888;}));} return( void*)({ struct
Cyc_Tcpat_Neg_struct* _temp889=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp889[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp890; _temp890.tag= Cyc_Tcpat_Neg_tag; _temp890.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp878,
c); _temp890;}); _temp889;}); _LL877: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp891=( char*)"add_neg called when td is Positive";
struct _tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 35; _temp892;})); _LL873:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp893= td; struct Cyc_List_List*
_temp899; struct Cyc_Tcpat_Con_s* _temp901; struct Cyc_Set_Set* _temp903; _LL895:
if((( struct _tunion_struct*) _temp893)->tag == Cyc_Tcpat_Pos_tag){ _LL902:
_temp901=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp893)->f1;
goto _LL900; _LL900: _temp899=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp893)->f2; goto _LL896;} else{ goto _LL897;} _LL897: if((( struct
_tunion_struct*) _temp893)->tag == Cyc_Tcpat_Neg_tag){ _LL904: _temp903=( struct
Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp893)->f1; goto _LL898;} else{
goto _LL894;} _LL896: if( Cyc_Tcpat_compare_con( c, _temp901) == 0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL898: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp903,
c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)({ struct Cyc_Core_Opt*
_temp905= c->span; if( _temp905 == 0){ _throw( Null_Exception);} _temp905->v;})
==(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp903) + 1: 0){
return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}} _LL894:;}
static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt, void*
dsc){ struct Cyc_List_List* _temp906= ctxt; struct Cyc_List_List _temp912;
struct Cyc_List_List* _temp913; struct _tuple10* _temp915; struct _tuple10
_temp917; struct Cyc_List_List* _temp918; struct Cyc_Tcpat_Con_s* _temp920;
_LL908: if( _temp906 == 0){ goto _LL909;} else{ goto _LL910;} _LL910: if(
_temp906 == 0){ goto _LL907;} else{ _temp912=* _temp906; _LL916: _temp915=(
struct _tuple10*) _temp912.hd; _temp917=* _temp915; _LL921: _temp920= _temp917.f1;
goto _LL919; _LL919: _temp918= _temp917.f2; goto _LL914; _LL914: _temp913=(
struct Cyc_List_List*) _temp912.tl; goto _LL911;} _LL909: return 0; _LL911:
return({ struct Cyc_List_List* _temp922=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp922->hd=( void*)({ struct _tuple10*
_temp923=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp923->f1=
_temp920; _temp923->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp924=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp924->hd=(
void*) dsc; _temp924->tl= _temp918; _temp924;}); _temp923;}); _temp922->tl=
_temp913; _temp922;}); _LL907:;} static struct Cyc_List_List* Cyc_Tcpat_norm_context(
struct Cyc_List_List* ctxt){ struct Cyc_List_List* _temp925= ctxt; struct Cyc_List_List
_temp931; struct Cyc_List_List* _temp932; struct _tuple10* _temp934; struct
_tuple10 _temp936; struct Cyc_List_List* _temp937; struct Cyc_Tcpat_Con_s*
_temp939; _LL927: if( _temp925 == 0){ goto _LL928;} else{ goto _LL929;} _LL929:
if( _temp925 == 0){ goto _LL926;} else{ _temp931=* _temp925; _LL935: _temp934=(
struct _tuple10*) _temp931.hd; _temp936=* _temp934; _LL940: _temp939= _temp936.f1;
goto _LL938; _LL938: _temp937= _temp936.f2; goto _LL933; _LL933: _temp932=(
struct Cyc_List_List*) _temp931.tl; goto _LL930;} _LL928: return(( struct Cyc_List_List*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp941=( char*)"norm_context: empty context"; struct _tagged_string _temp942;
_temp942.curr= _temp941; _temp942.base= _temp941; _temp942.last_plus_one=
_temp941 + 28; _temp942;})); _LL930: return Cyc_Tcpat_augment( _temp932,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp943=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp943[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp944; _temp944.tag= Cyc_Tcpat_Pos_tag; _temp944.f1= _temp939; _temp944.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp937);
_temp944;}); _temp943;})); _LL926:;} static void* Cyc_Tcpat_build_desc( struct
Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple5
_temp946=({ struct _tuple5 _temp945; _temp945.f1= ctxt; _temp945.f2= work;
_temp945;}); struct Cyc_List_List* _temp956; struct Cyc_List_List* _temp958;
struct Cyc_List_List* _temp960; struct Cyc_List_List* _temp962; struct Cyc_List_List*
_temp964; struct Cyc_List_List* _temp966; struct Cyc_List_List* _temp968; struct
Cyc_List_List _temp970; struct Cyc_List_List* _temp971; struct _tuple9* _temp973;
struct _tuple9 _temp975; struct Cyc_List_List* _temp976; struct Cyc_List_List*
_temp978; struct Cyc_List_List* _temp980; struct Cyc_List_List* _temp982; struct
Cyc_List_List _temp984; struct Cyc_List_List* _temp985; struct _tuple10*
_temp987; struct _tuple10 _temp989; struct Cyc_List_List* _temp990; struct Cyc_Tcpat_Con_s*
_temp992; _LL948: _LL959: _temp958= _temp946.f1; if( _temp958 == 0){ goto _LL957;}
else{ goto _LL950;} _LL957: _temp956= _temp946.f2; if( _temp956 == 0){ goto
_LL949;} else{ goto _LL950;} _LL950: _LL963: _temp962= _temp946.f1; if( _temp962
== 0){ goto _LL961;} else{ goto _LL952;} _LL961: _temp960= _temp946.f2; goto
_LL951; _LL952: _LL967: _temp966= _temp946.f1; goto _LL965; _LL965: _temp964=
_temp946.f2; if( _temp964 == 0){ goto _LL953;} else{ goto _LL954;} _LL954:
_LL983: _temp982= _temp946.f1; if( _temp982 == 0){ goto _LL947;} else{ _temp984=*
_temp982; _LL988: _temp987=( struct _tuple10*) _temp984.hd; _temp989=* _temp987;
_LL993: _temp992= _temp989.f1; goto _LL991; _LL991: _temp990= _temp989.f2; goto
_LL986; _LL986: _temp985=( struct Cyc_List_List*) _temp984.tl; goto _LL969;}
_LL969: _temp968= _temp946.f2; if( _temp968 == 0){ goto _LL947;} else{ _temp970=*
_temp968; _LL974: _temp973=( struct _tuple9*) _temp970.hd; _temp975=* _temp973;
_LL981: _temp980= _temp975.f1; goto _LL979; _LL979: _temp978= _temp975.f2; goto
_LL977; _LL977: _temp976= _temp975.f3; goto _LL972; _LL972: _temp971=( struct
Cyc_List_List*) _temp970.tl; goto _LL955;} _LL949: return dsc; _LL951: return((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp994=( char*)"build_desc: ctxt and work don't match"; struct
_tagged_string _temp995; _temp995.curr= _temp994; _temp995.base= _temp994;
_temp995.last_plus_one= _temp994 + 38; _temp995;})); _LL953: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp996=( char*)"build_desc: ctxt and work don't match"; struct _tagged_string
_temp997; _temp997.curr= _temp996; _temp997.base= _temp996; _temp997.last_plus_one=
_temp996 + 38; _temp997;})); _LL955: { struct Cyc_Tcpat_Pos_struct* td=({ struct
Cyc_Tcpat_Pos_struct* _temp998=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp998[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp999; _temp999.tag= Cyc_Tcpat_Pos_tag; _temp999.f1= _temp992; _temp999.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp990),({
struct Cyc_List_List* _temp1000=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1000->hd=( void*) dsc; _temp1000->tl= _temp976;
_temp1000;})); _temp999;}); _temp998;}); return Cyc_Tcpat_build_desc( _temp985,(
void*) td, _temp971);} _LL947:;} static void* Cyc_Tcpat_match( void* p, void*
obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp1001=
allmrules; struct Cyc_List_List _temp1007; struct Cyc_List_List* _temp1008;
struct _tuple0* _temp1010; struct _tuple0 _temp1012; void* _temp1013; void*
_temp1015; _LL1003: if( _temp1001 == 0){ goto _LL1004;} else{ goto _LL1005;}
_LL1005: if( _temp1001 == 0){ goto _LL1002;} else{ _temp1007=* _temp1001;
_LL1011: _temp1010=( struct _tuple0*) _temp1007.hd; _temp1012=* _temp1010;
_LL1016: _temp1015= _temp1012.f1; goto _LL1014; _LL1014: _temp1013= _temp1012.f2;
goto _LL1009; _LL1009: _temp1008=( struct Cyc_List_List*) _temp1007.tl; goto
_LL1006;} _LL1004: return( void*) Cyc_Tcpat_Failure; _LL1006: return(( void*(*)(
void* p, void* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)(
_temp1015,( void*) Cyc_Tcpat_Obj, dsc, 0, 0, _temp1013, _temp1008); _LL1002:;}
static void* Cyc_Tcpat_match_compile( struct Cyc_List_List* allmrules){ return((
void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)((
void*)({ struct Cyc_Tcpat_Neg_struct* _temp1017=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp1017[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp1018; _temp1018.tag= Cyc_Tcpat_Neg_tag; _temp1018.f1= Cyc_Tcpat_empty_con_set();
_temp1018;}); _temp1017;}), allmrules);} static void* Cyc_Tcpat_and_match(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules){ struct Cyc_List_List* _temp1019= work; struct Cyc_List_List
_temp1027; struct Cyc_List_List* _temp1028; struct _tuple9* _temp1030; struct
_tuple9 _temp1032; struct Cyc_List_List* _temp1033; struct Cyc_List_List*
_temp1035; struct Cyc_List_List* _temp1037; struct Cyc_List_List _temp1039;
struct Cyc_List_List* _temp1040; struct _tuple9* _temp1042; struct _tuple9
_temp1044; struct Cyc_List_List* _temp1045; struct Cyc_List_List* _temp1047;
struct Cyc_List_List* _temp1049; _LL1021: if( _temp1019 == 0){ goto _LL1022;}
else{ goto _LL1023;} _LL1023: if( _temp1019 == 0){ goto _LL1025;} else{
_temp1027=* _temp1019; _LL1031: _temp1030=( struct _tuple9*) _temp1027.hd;
_temp1032=* _temp1030; _LL1038: _temp1037= _temp1032.f1; if( _temp1037 == 0){
goto _LL1036;} else{ goto _LL1025;} _LL1036: _temp1035= _temp1032.f2; if(
_temp1035 == 0){ goto _LL1034;} else{ goto _LL1025;} _LL1034: _temp1033=
_temp1032.f3; if( _temp1033 == 0){ goto _LL1029;} else{ goto _LL1025;} _LL1029:
_temp1028=( struct Cyc_List_List*) _temp1027.tl; goto _LL1024;} _LL1025: if(
_temp1019 == 0){ goto _LL1020;} else{ _temp1039=* _temp1019; _LL1043: _temp1042=(
struct _tuple9*) _temp1039.hd; _temp1044=* _temp1042; _LL1050: _temp1049=
_temp1044.f1; goto _LL1048; _LL1048: _temp1047= _temp1044.f2; goto _LL1046;
_LL1046: _temp1045= _temp1044.f3; goto _LL1041; _LL1041: _temp1040=( struct Cyc_List_List*)
_temp1039.tl; goto _LL1026;} _LL1022: return( void*)({ struct Cyc_Tcpat_Success_struct*
_temp1051=( struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp1051[ 0]=({ struct Cyc_Tcpat_Success_struct _temp1052; _temp1052.tag= Cyc_Tcpat_Success_tag;
_temp1052.f1=( void*) right_hand_side; _temp1052;}); _temp1051;}); _LL1024:
return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context(
ctx), _temp1028, right_hand_side, rules); _LL1026: if(( _temp1049 == 0? 1:
_temp1047 == 0)? 1: _temp1045 == 0){ return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1053=( char*)"tcpat:and_match: malformed work frame";
struct _tagged_string _temp1054; _temp1054.curr= _temp1053; _temp1054.base=
_temp1053; _temp1054.last_plus_one= _temp1053 + 38; _temp1054;}));}{ struct Cyc_List_List
_temp1058; struct Cyc_List_List* _temp1059; void* _temp1061; struct Cyc_List_List*
_temp1056=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1055= _temp1049;
if( _temp1055 == 0){ _throw( Null_Exception);} _temp1055;}); _temp1058=*
_temp1056; _LL1062: _temp1061=( void*) _temp1058.hd; goto _LL1060; _LL1060:
_temp1059=( struct Cyc_List_List*) _temp1058.tl; goto _LL1057; _LL1057: { struct
Cyc_List_List _temp1066; struct Cyc_List_List* _temp1067; void* _temp1069;
struct Cyc_List_List* _temp1064=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1063= _temp1047; if( _temp1063 == 0){ _throw( Null_Exception);} _temp1063;});
_temp1066=* _temp1064; _LL1070: _temp1069=( void*) _temp1066.hd; goto _LL1068;
_LL1068: _temp1067=( struct Cyc_List_List*) _temp1066.tl; goto _LL1065; _LL1065: {
struct Cyc_List_List _temp1074; struct Cyc_List_List* _temp1075; void* _temp1077;
struct Cyc_List_List* _temp1072=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1071= _temp1045; if( _temp1071 == 0){ _throw( Null_Exception);} _temp1071;});
_temp1074=* _temp1072; _LL1078: _temp1077=( void*) _temp1074.hd; goto _LL1076;
_LL1076: _temp1075=( struct Cyc_List_List*) _temp1074.tl; goto _LL1073; _LL1073: {
struct _tuple9* wf=({ struct _tuple9* _temp1079=( struct _tuple9*) GC_malloc(
sizeof( struct _tuple9)); _temp1079->f1= _temp1059; _temp1079->f2= _temp1067;
_temp1079->f3= _temp1075; _temp1079;}); return(( void*(*)( void* p, void* obj,
void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp1061,
_temp1069, _temp1077, ctx,({ struct Cyc_List_List* _temp1080=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1080->hd=( void*) wf; _temp1080->tl=
_temp1040; _temp1080;}), right_hand_side, rules);}}}} _LL1020:;} static struct
Cyc_List_List* Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon, void* dsc){
void* _temp1081= dsc; struct Cyc_Set_Set* _temp1087; struct Cyc_List_List*
_temp1089; struct Cyc_Tcpat_Con_s* _temp1091; _LL1083: if((( struct
_tunion_struct*) _temp1081)->tag == Cyc_Tcpat_Neg_tag){ _LL1088: _temp1087=(
struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp1081)->f1; goto
_LL1084;} else{ goto _LL1085;} _LL1085: if((( struct _tunion_struct*) _temp1081)->tag
== Cyc_Tcpat_Pos_tag){ _LL1092: _temp1091=( struct Cyc_Tcpat_Con_s*)(( struct
Cyc_Tcpat_Pos_struct*) _temp1081)->f1; goto _LL1090; _LL1090: _temp1089=( struct
Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*) _temp1081)->f2; goto _LL1086;}
else{ goto _LL1082;} _LL1084: { void* any=( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp1093=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp1093[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp1094; _temp1094.tag= Cyc_Tcpat_Neg_tag;
_temp1094.f1= Cyc_Tcpat_empty_con_set(); _temp1094;}); _temp1093;}); struct Cyc_List_List*
res= 0;{ int i= 0; for( 0; i < pcon->arity; ++ i){ res=({ struct Cyc_List_List*
_temp1095=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1095->hd=( void*) any; _temp1095->tl= res; _temp1095;});}} return res;}
_LL1086: return _temp1089; _LL1082:;} static void* Cyc_Tcpat_getoarg( void* obj,
int i){ return( void*)({ struct Cyc_Tcpat_Sel_struct* _temp1096=( struct Cyc_Tcpat_Sel_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Sel_struct)); _temp1096[ 0]=({ struct Cyc_Tcpat_Sel_struct
_temp1097; _temp1097.tag= Cyc_Tcpat_Sel_tag; _temp1097.f1= i + 1; _temp1097.f2=(
void*) obj; _temp1097;}); _temp1096;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs(
struct Cyc_Tcpat_Con_s* pcon, void* obj){ return(( struct Cyc_List_List*(*)( int
n, void*(* f)( void*, int), void* env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg,
obj);} static void* Cyc_Tcpat_match( void* p, void* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp1098= p; struct Cyc_List_List* _temp1104; struct Cyc_Tcpat_Con_s*
_temp1106; _LL1100: if(( int) _temp1098 == Cyc_Tcpat_Any){ goto _LL1101;} else{
goto _LL1102;} _LL1102: if(( unsigned int) _temp1098 > 1u?(( struct
_tunion_struct*) _temp1098)->tag == Cyc_Tcpat_Con_tag: 0){ _LL1107: _temp1106=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Con_struct*) _temp1098)->f1; goto
_LL1105; _LL1105: _temp1104=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Con_struct*)
_temp1098)->f2; goto _LL1103;} else{ goto _LL1099;} _LL1101: return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc),
work, right_hand_side, rules); _LL1103: { void* _temp1108= Cyc_Tcpat_static_match(
_temp1106, dsc); _LL1110: if(( int) _temp1108 == Cyc_Tcpat_Yes){ goto _LL1111;}
else{ goto _LL1112;} _LL1112: if(( int) _temp1108 == Cyc_Tcpat_No){ goto _LL1113;}
else{ goto _LL1114;} _LL1114: if(( int) _temp1108 == Cyc_Tcpat_Maybe){ goto
_LL1115;} else{ goto _LL1109;} _LL1111: { struct Cyc_List_List* ctx2=({ struct
Cyc_List_List* _temp1116=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1116->hd=( void*)({ struct _tuple10* _temp1117=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1117->f1= _temp1106; _temp1117->f2= 0;
_temp1117;}); _temp1116->tl= ctx; _temp1116;}); struct _tuple9* work_frame=({
struct _tuple9* _temp1118=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp1118->f1= _temp1104; _temp1118->f2= Cyc_Tcpat_getoargs( _temp1106, obj);
_temp1118->f3= Cyc_Tcpat_getdargs( _temp1106, dsc); _temp1118;}); struct Cyc_List_List*
work2=({ struct Cyc_List_List* _temp1119=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1119->hd=( void*) work_frame; _temp1119->tl=
work; _temp1119;}); return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules);} _LL1113: return(( void*(*)( void* dsc,
struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx,
dsc, work), rules); _LL1115: { struct Cyc_List_List* ctx2=({ struct Cyc_List_List*
_temp1120=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1120->hd=( void*)({ struct _tuple10* _temp1121=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1121->f1= _temp1106; _temp1121->f2= 0;
_temp1121;}); _temp1120->tl= ctx; _temp1120;}); struct _tuple9* work_frame=({
struct _tuple9* _temp1122=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp1122->f1= _temp1104; _temp1122->f2= Cyc_Tcpat_getoargs( _temp1106, obj);
_temp1122->f3= Cyc_Tcpat_getdargs( _temp1106, dsc); _temp1122;}); struct Cyc_List_List*
work2=({ struct Cyc_List_List* _temp1123=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1123->hd=( void*) work_frame; _temp1123->tl=
work; _temp1123;}); void* s=(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules); void* f=(( void*(*)( void* dsc, struct Cyc_List_List*
allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg(
dsc, _temp1106), work), rules); return( void*)({ struct Cyc_Tcpat_IfEq_struct*
_temp1124=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp1124[ 0]=({ struct Cyc_Tcpat_IfEq_struct _temp1125; _temp1125.tag= Cyc_Tcpat_IfEq_tag;
_temp1125.f1=( void*) obj; _temp1125.f2= _temp1106; _temp1125.f3=( void*) s;
_temp1125.f4=( void*) f; _temp1125;}); _temp1124;});} _LL1109:;} _LL1099:;}
static void Cyc_Tcpat_check_exhaust_overlap( void* d, void(* not_exhaust)( void*),
void* env1, void(* rhs_appears)( void*, void*), void* env2){ void* _temp1126= d;
void* _temp1134; void* _temp1136; void* _temp1138; struct Cyc_Tcpat_Con_s*
_temp1140; void* _temp1142; _LL1128: if(( int) _temp1126 == Cyc_Tcpat_Failure){
goto _LL1129;} else{ goto _LL1130;} _LL1130: if(( unsigned int) _temp1126 > 1u?((
struct _tunion_struct*) _temp1126)->tag == Cyc_Tcpat_Success_tag: 0){ _LL1135:
_temp1134=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1126)->f1; goto
_LL1131;} else{ goto _LL1132;} _LL1132: if(( unsigned int) _temp1126 > 1u?((
struct _tunion_struct*) _temp1126)->tag == Cyc_Tcpat_IfEq_tag: 0){ _LL1143:
_temp1142=( void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1126)->f1; goto _LL1141;
_LL1141: _temp1140=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1126)->f2; goto _LL1139; _LL1139: _temp1138=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1126)->f3; goto _LL1137; _LL1137: _temp1136=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1126)->f4; goto _LL1133;} else{ goto _LL1127;} _LL1129: not_exhaust( env1);
goto _LL1127; _LL1131: rhs_appears( env2, _temp1134); goto _LL1127; _LL1133:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1138,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1136, not_exhaust, env1, rhs_appears, env2); goto _LL1127; _LL1127:;}
static struct _tuple12* Cyc_Tcpat_get_match( int* r, struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({
struct _tuple11* _temp1144=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11)); _temp1144->f1= 0; _temp1144->f2=( swc->pattern)->loc; _temp1144;});
void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1145=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1145->hd=( void*) sp0; _temp1145->tl=({ struct Cyc_List_List* _temp1146=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1146->hd=(
void*) Cyc_Tcpat_int_pat(* r); _temp1146->tl= 0; _temp1146;}); _temp1145;}));* r=*
r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List* _temp1147=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1147->hd=(
void*) sp0; _temp1147->tl=({ struct Cyc_List_List* _temp1148=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1148->hd=( void*)(( void*) Cyc_Tcpat_Any);
_temp1148->tl= 0; _temp1148;}); _temp1147;}));} return({ struct _tuple12*
_temp1149=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1149->f1=
sp; _temp1149->f2= rhs; _temp1149;});} static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp1150=( char*)"patterns may not be exhaustive"; struct _tagged_string
_temp1151; _temp1151.curr= _temp1150; _temp1151.base= _temp1150; _temp1151.last_plus_one=
_temp1150 + 31; _temp1151;}));} static void Cyc_Tcpat_rule_occurs( int dummy,
struct _tuple11* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int where_ctr= 0;
struct Cyc_List_List* match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(*
f)( int*, struct Cyc_Absyn_Switch_clause*), int* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Tcpat_get_match,& where_ctr, swcs); void* dec_tree=(( void*(*)(
struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)( match_rules);(( void(*)(
void* d, void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple11*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
dec_tree, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
match_rules != 0; match_rules=({ struct Cyc_List_List* _temp1152= match_rules;
if( _temp1152 == 0){ _throw( Null_Exception);} _temp1152->tl;})){ struct
_tuple11 _temp1156; struct Cyc_Position_Segment* _temp1157; int _temp1159;
struct _tuple11* _temp1154=(*(( struct _tuple12*)({ struct Cyc_List_List*
_temp1153= match_rules; if( _temp1153 == 0){ _throw( Null_Exception);} _temp1153->hd;}))).f2;
_temp1156=* _temp1154; _LL1160: _temp1159= _temp1156.f1; goto _LL1158; _LL1158:
_temp1157= _temp1156.f2; goto _LL1155; _LL1155: if( ! _temp1159){ Cyc_Tcutil_terr(
_temp1157,( struct _tagged_string)({ char* _temp1161=( char*)"redundant pattern";
struct _tagged_string _temp1162; _temp1162.curr= _temp1161; _temp1162.base=
_temp1161; _temp1162.last_plus_one= _temp1161 + 18; _temp1162;}));}}} static
void Cyc_Tcpat_not_exhaust_warn( struct _tuple13* pr){(* pr).f2= 0; Cyc_Tcutil_warn((*
pr).f1,( struct _tagged_string)({ char* _temp1163=( char*)"pattern not exhaustive";
struct _tagged_string _temp1164; _temp1164.curr= _temp1163; _temp1164.base=
_temp1163; _temp1164.last_plus_one= _temp1163 + 23; _temp1164;}));} static void
Cyc_Tcpat_dummy_fn( int i, int j){ return;} int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
match_rules=({ struct Cyc_List_List* _temp1165=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1165->hd=( void*)({ struct
_tuple14* _temp1166=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp1166->f1= Cyc_Tcpat_compile_pat( p); _temp1166->f2= 0; _temp1166;});
_temp1165->tl= 0; _temp1165;}); void* dec_tree=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( match_rules); struct _tuple13* exhaust_env=({
struct _tuple13* _temp1167=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp1167->f1= loc; _temp1167->f2= 1; _temp1167;});(( void(*)( void*
d, void(* not_exhaust)( struct _tuple13*), struct _tuple13* env1, void(*
rhs_appears)( int, int), int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree,
Cyc_Tcpat_not_exhaust_warn, exhaust_env, Cyc_Tcpat_dummy_fn, 0); return(*
exhaust_env).f2;} static struct _tuple12* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({
struct _tuple11* _temp1168=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11)); _temp1168->f1= 0; _temp1168->f2=( swc->pattern)->loc; _temp1168;});
return({ struct _tuple12* _temp1169=( struct _tuple12*) GC_malloc( sizeof(
struct _tuple12)); _temp1169->f1= sp0; _temp1169->f2= rhs; _temp1169;});} static
void Cyc_Tcpat_not_exhaust_err2( struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple11*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=({ struct
Cyc_List_List* _temp1170= match_rules; if( _temp1170 == 0){ _throw(
Null_Exception);} _temp1170->tl;})){ struct _tuple11* pair=(*(( struct _tuple12*)({
struct Cyc_List_List* _temp1171= match_rules; if( _temp1171 == 0){ _throw(
Null_Exception);} _temp1171->hd;}))).f2; if( !(* pair).f1){ Cyc_Tcutil_terr((*
pair).f2,( struct _tagged_string)({ char* _temp1172=( char*)"redundant pattern";
struct _tagged_string _temp1173; _temp1173.curr= _temp1172; _temp1173.base=
_temp1172; _temp1173.last_plus_one= _temp1172 + 18; _temp1173;}));}}}