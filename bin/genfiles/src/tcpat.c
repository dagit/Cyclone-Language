#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct
_tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct _tuple6{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct _tuple7{ struct Cyc_Absyn_Tqual*
f1; void* f2; } ; struct _tuple8{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat*
f2; } ; struct _tuple9{ struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; }
; struct _tuple10{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct
Cyc_List_List* f3; } ; struct _tuple11{ int f1; struct Cyc_Position_Segment* f2;
} ; struct _tuple12{ void* f1; struct _tuple11* f2; } ; struct _tuple13{ struct
Cyc_Position_Segment* f1; int f2; } ; struct _tuple14{ void* f1; int f2; } ;
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
goto _LL6;}} _LL30:{ struct Cyc_List_List* t= _temp76; for( 0; t != 0; t=({
struct Cyc_List_List* _temp255= t; if( _temp255 == 0){ _throw( Null_Exception);}
_temp255->tl;})){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)({ struct
Cyc_List_List* _temp256= t; if( _temp256 == 0){ _throw( Null_Exception);}
_temp256->hd;});(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind);{ void* _temp257=( void*)( tv->kind)->v; void* _temp267; void*
_temp269; struct Cyc_Absyn_Conref* _temp271; _LL259: if(( unsigned int) _temp257
> 1u?(( struct _tunion_struct*) _temp257)->tag == Cyc_Absyn_Eq_constr_tag: 0){
_LL268: _temp267=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp257)->f1;
if(( int) _temp267 == Cyc_Absyn_BoxKind){ goto _LL260;} else{ goto _LL261;}}
else{ goto _LL261;} _LL261: if(( unsigned int) _temp257 > 1u?(( struct
_tunion_struct*) _temp257)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL270: _temp269=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp257)->f1; goto _LL262;} else{
goto _LL263;} _LL263: if(( int) _temp257 == Cyc_Absyn_No_constr){ goto _LL264;}
else{ goto _LL265;} _LL265: if(( unsigned int) _temp257 > 1u?(( struct
_tunion_struct*) _temp257)->tag == Cyc_Absyn_Forward_constr_tag: 0){ _LL272:
_temp271=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp257)->f1; goto _LL266;} else{ goto _LL258;} _LL260: goto _LL258; _LL262:
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp273= Cyc_Absynpp_qvar2string(
_temp80); struct _tagged_string _temp274=* tv->name; struct _tagged_string
_temp275= Cyc_Absynpp_kind2string( _temp269); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp273.last_plus_one - _temp273.curr, _temp273.curr, _temp274.last_plus_one -
_temp274.curr, _temp274.curr, _temp275.last_plus_one - _temp275.curr, _temp275.curr);}));
goto _LL258; _LL264:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp276=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp276[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp277; _temp277.tag= Cyc_Absyn_Eq_constr_tag;
_temp277.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp277;}); _temp276;})));
goto _LL258; _LL266:(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp278=( char*)"forward after compress in conref";
struct _tagged_string _temp279; _temp279.curr= _temp278; _temp279.base= _temp278;
_temp279.last_plus_one= _temp278 + 33; _temp279;})); goto _LL258; _LL258:;}}}{
struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp76);
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp76);{ struct Cyc_List_List* all_inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp72->tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst); struct Cyc_List_List* tqts= _temp70->typs; if( tqts ==
0){ t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp280=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp280[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp281; _temp281.tag= Cyc_Absyn_TunionType_tag;
_temp281.f1=({ struct Cyc_Absyn_TunionInfo _temp282; _temp282.name=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp283= _temp72->name; if( _temp283 == 0){
_throw( Null_Exception);} _temp283->v;}); _temp282.targs= all_typs; _temp282.rgn=(
void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp282.tud=( struct Cyc_Absyn_Tuniondecl*)
_temp72; _temp282;}); _temp281;}); _temp280;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp284=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp284[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp285; _temp285.tag= Cyc_Absyn_TunionFieldType_tag; _temp285.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp286; _temp286.name=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp287=
_temp72->name; if( _temp287 == 0){ _throw( Null_Exception);} _temp287->v;});
_temp286.targs= all_typs; _temp286.fname= _temp70->name; _temp286.tud=( struct
Cyc_Absyn_Tuniondecl*) _temp72; _temp286.tufd=( struct Cyc_Absyn_Tunionfield*)
_temp70; _temp286;}); _temp285;}); _temp284;});} for( 0; _temp74 != 0? tqts != 0:
0; _temp74=({ struct Cyc_List_List* _temp288= _temp74; if( _temp288 == 0){
_throw( Null_Exception);} _temp288->tl;}), tqts=({ struct Cyc_List_List*
_temp289= tqts; if( _temp289 == 0){ _throw( Null_Exception);} _temp289->tl;})){
struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp290= _temp74; if( _temp290 == 0){ _throw( Null_Exception);} _temp290->hd;});
struct Cyc_List_List* _temp293; struct Cyc_List_List* _temp295; struct _tuple5
_temp291= Cyc_Tcpat_tcPat( te2, p2, 0, region_opt); _LL296: _temp295= _temp291.f1;
goto _LL294; _LL294: _temp293= _temp291.f2; goto _LL292; _LL292: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp295); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp293);{ void*
field_typ= Cyc_Tcutil_substitute( all_inst,(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp297= tqts; if( _temp297 == 0){ _throw( Null_Exception);} _temp297->hd;}))).f2);
if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp298= p2->topt; if(
_temp298 == 0){ _throw( Null_Exception);} _temp298->v;}), field_typ)){ Cyc_Tcutil_terr(
p2->loc,({ struct _tagged_string _temp300= Cyc_Absynpp_qvar2string( _temp80);
struct _tagged_string _temp301= Cyc_Absynpp_typ2string( field_typ); struct
_tagged_string _temp302= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp299= p2->topt; if( _temp299 == 0){ _throw( Null_Exception);} _temp299->v;}));
xprintf("%.*s expects argument type %.*s, not %.*s", _temp300.last_plus_one -
_temp300.curr, _temp300.curr, _temp301.last_plus_one - _temp301.curr, _temp301.curr,
_temp302.last_plus_one - _temp302.curr, _temp302.curr);}));}}} if( _temp74 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp303= Cyc_Absynpp_qvar2string(
_temp80); xprintf("too many arguments for tunion constructor %.*s", _temp303.last_plus_one
- _temp303.curr, _temp303.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp304= Cyc_Absynpp_qvar2string( _temp80); xprintf("too few arguments for tunion constructor %.*s",
_temp304.last_plus_one - _temp304.curr, _temp304.curr);}));} goto _LL6;}} _LL32: {
struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp88);
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp88);{ struct Cyc_List_List* tqts= _temp82->typs;
t=( void*)({ struct Cyc_Absyn_XTunionFieldType_struct* _temp305=( struct Cyc_Absyn_XTunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionFieldType_struct)); _temp305[ 0]=({
struct Cyc_Absyn_XTunionFieldType_struct _temp306; _temp306.tag= Cyc_Absyn_XTunionFieldType_tag;
_temp306.f1=({ struct Cyc_Absyn_XTunionFieldInfo _temp307; _temp307.name=
_temp84->name; _temp307.fname= _temp82->name; _temp307.xtud=( struct Cyc_Absyn_XTuniondecl*)
_temp84; _temp307.xtufd=( struct Cyc_Absyn_Tunionfield*) _temp82; _temp307;});
_temp306;}); _temp305;}); for( 0; _temp86 != 0? tqts != 0: 0; _temp86=({ struct
Cyc_List_List* _temp308= _temp86; if( _temp308 == 0){ _throw( Null_Exception);}
_temp308->tl;}), tqts=({ struct Cyc_List_List* _temp309= tqts; if( _temp309 == 0){
_throw( Null_Exception);} _temp309->tl;})){ struct Cyc_Absyn_Pat* p2=( struct
Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp310= _temp86; if( _temp310 == 0){
_throw( Null_Exception);} _temp310->hd;}); struct Cyc_List_List* _temp313;
struct Cyc_List_List* _temp315; struct _tuple5 _temp311= Cyc_Tcpat_tcPat( te2,
p2, 0, region_opt); _LL316: _temp315= _temp311.f1; goto _LL314; _LL314: _temp313=
_temp311.f2; goto _LL312; _LL312: tv_result=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp315); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp313);{ void* field_typ=(*((
struct _tuple7*)({ struct Cyc_List_List* _temp317= tqts; if( _temp317 == 0){
_throw( Null_Exception);} _temp317->hd;}))).f2; if( ! Cyc_Tcutil_unify(( void*)({
struct Cyc_Core_Opt* _temp318= p2->topt; if( _temp318 == 0){ _throw(
Null_Exception);} _temp318->v;}), field_typ)){ Cyc_Tcutil_terr( p2->loc,({
struct _tagged_string _temp320= Cyc_Absynpp_qvar2string( _temp90); struct
_tagged_string _temp321= Cyc_Absynpp_typ2string( field_typ); struct
_tagged_string _temp322= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp319= p2->topt; if( _temp319 == 0){ _throw( Null_Exception);} _temp319->v;}));
xprintf("%.*s expects argument type %.*s, not %.*s", _temp320.last_plus_one -
_temp320.curr, _temp320.curr, _temp321.last_plus_one - _temp321.curr, _temp321.curr,
_temp322.last_plus_one - _temp322.curr, _temp322.curr);}));}}} if( _temp86 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp323= Cyc_Absynpp_qvar2string(
_temp90); xprintf("too many arguments for xtunion constructor %.*s", _temp323.last_plus_one
- _temp323.curr, _temp323.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp324= Cyc_Absynpp_qvar2string( _temp90); xprintf("too few arguments for xtunion constructor %.*s",
_temp324.last_plus_one - _temp324.curr, _temp324.curr);}));} goto _LL6;}} _LL34:
t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp325=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp325[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp326; _temp326.tag= Cyc_Absyn_EnumType_tag;
_temp326.f1= _temp94->name; _temp326.f2=( struct Cyc_Absyn_Enumdecl*) _temp94;
_temp326;}); _temp325;}); goto _LL6; _LL36:{ struct _handler_cons _temp327;
_push_handler(& _temp327);{ struct _xtunion_struct* _temp328=( struct
_xtunion_struct*) setjmp( _temp327.handler); if( ! _temp328){{ void* _temp329=
Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp98); struct Cyc_Absyn_Structdecl*
_temp341; struct Cyc_Absyn_Tunionfield* _temp343; struct Cyc_Absyn_Tuniondecl*
_temp345; struct Cyc_Absyn_Tunionfield* _temp347; struct Cyc_Absyn_XTuniondecl*
_temp349; struct Cyc_Absyn_Enumfield* _temp351; struct Cyc_Absyn_Enumdecl*
_temp353; void* _temp355; _LL331: if((( struct _tunion_struct*) _temp329)->tag
== Cyc_Tcenv_StructRes_tag){ _LL342: _temp341=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp329)->f1; goto _LL332;} else{ goto
_LL333;} _LL333: if((( struct _tunion_struct*) _temp329)->tag == Cyc_Tcenv_TunionRes_tag){
_LL346: _temp345=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp329)->f1; goto _LL344; _LL344: _temp343=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp329)->f2; goto _LL334;} else{ goto
_LL335;} _LL335: if((( struct _tunion_struct*) _temp329)->tag == Cyc_Tcenv_XTunionRes_tag){
_LL350: _temp349=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp329)->f1; goto _LL348; _LL348: _temp347=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp329)->f2; goto _LL336;} else{ goto
_LL337;} _LL337: if((( struct _tunion_struct*) _temp329)->tag == Cyc_Tcenv_EnumRes_tag){
_LL354: _temp353=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp329)->f1; goto _LL352; _LL352: _temp351=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp329)->f2; goto _LL338;} else{ goto _LL339;}
_LL339: if((( struct _tunion_struct*) _temp329)->tag == Cyc_Tcenv_VarRes_tag){
_LL356: _temp355=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp329)->f1; goto
_LL340;} else{ goto _LL330;} _LL332: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp357=( char*)"struct tag used without arguments in pattern";
struct _tagged_string _temp358; _temp358.curr= _temp357; _temp358.base= _temp357;
_temp358.last_plus_one= _temp357 + 45; _temp358;})); t= Cyc_Absyn_wildtyp();
_npop_handler( 0u); goto tcpat_end; _LL334:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_p_struct* _temp359=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp359[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp360; _temp360.tag= Cyc_Absyn_Tunion_p_tag;
_temp360.f1= _temp343->name; _temp360.f2= 0; _temp360.f3= 0; _temp360.f4= 0;
_temp360.f5= _temp345; _temp360.f6= _temp343; _temp360;}); _temp359;})));{
struct _tuple5 _temp361= Cyc_Tcpat_tcPat( te, p, topt, region_opt);
_npop_handler( 0u); return _temp361;} _LL336:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_XTunion_p_struct* _temp362=( struct Cyc_Absyn_XTunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct)); _temp362[ 0]=({ struct
Cyc_Absyn_XTunion_p_struct _temp363; _temp363.tag= Cyc_Absyn_XTunion_p_tag;
_temp363.f1= _temp347->name; _temp363.f2= 0; _temp363.f3= 0; _temp363.f4=
_temp349; _temp363.f5= _temp347; _temp363;}); _temp362;})));{ struct _tuple5
_temp364= Cyc_Tcpat_tcPat( te, p, topt, region_opt); _npop_handler( 0u); return
_temp364;} _LL338:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp365=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp365[ 0]=({ struct Cyc_Absyn_Enum_p_struct _temp366; _temp366.tag= Cyc_Absyn_Enum_p_tag;
_temp366.f1= _temp351->name; _temp366.f2= _temp353; _temp366.f3= _temp351;
_temp366;}); _temp365;})));{ struct _tuple5 _temp367= Cyc_Tcpat_tcPat( te, p,
topt, region_opt); _npop_handler( 0u); return _temp367;} _LL340: goto _LL330;
_LL330:;}; _pop_handler();} else{ struct _xtunion_struct* _temp369= _temp328;
_LL371: if( _temp369->tag == Cyc_Dict_Absent_tag){ goto _LL372;} else{ goto
_LL373;} _LL373: goto _LL374; _LL372: goto _LL370; _LL374:( void) _throw(
_temp369); _LL370:;}}}{ void* _temp375=(* _temp98).f1; struct Cyc_List_List*
_temp383; _LL377: if(( int) _temp375 == Cyc_Absyn_Loc_n){ goto _LL378;} else{
goto _LL379;} _LL379: if(( unsigned int) _temp375 > 1u?(( struct _tunion_struct*)
_temp375)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL384: _temp383=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp375)->f1; if( _temp383 == 0){ goto _LL380;}
else{ goto _LL381;}} else{ goto _LL381;} _LL381: goto _LL382; _LL378: goto
_LL380; _LL380:(* _temp98).f1=( void*) Cyc_Absyn_Loc_n;( void*)( p->r=( void*)((
void*)({ struct Cyc_Absyn_Var_p_struct* _temp385=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp385[ 0]=({ struct Cyc_Absyn_Var_p_struct
_temp386; _temp386.tag= Cyc_Absyn_Var_p_tag; _temp386.f1= Cyc_Absyn_new_vardecl(
_temp98,( void*) Cyc_Absyn_VoidType, 0); _temp386;}); _temp385;}))); return Cyc_Tcpat_tcPat(
te, p, topt, region_opt); _LL382: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp387=( char*)"qualified variable in pattern"; struct
_tagged_string _temp388; _temp388.curr= _temp387; _temp388.base= _temp387;
_temp388.last_plus_one= _temp387 + 30; _temp388;})); t= Cyc_Absyn_wildtyp();
goto _LL376; _LL376:;} goto _LL6; _LL38:{ struct _handler_cons _temp389;
_push_handler(& _temp389);{ struct _xtunion_struct* _temp390=( struct
_xtunion_struct*) setjmp( _temp389.handler); if( ! _temp390){{ void* _temp391=
Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp104); struct Cyc_Absyn_Structdecl*
_temp403; struct Cyc_Absyn_Tunionfield* _temp405; struct Cyc_Absyn_Tuniondecl*
_temp407; struct Cyc_Absyn_Tunionfield* _temp409; struct Cyc_Absyn_XTuniondecl*
_temp411; struct Cyc_Absyn_Enumfield* _temp413; struct Cyc_Absyn_Enumdecl*
_temp415; void* _temp417; _LL393: if((( struct _tunion_struct*) _temp391)->tag
== Cyc_Tcenv_StructRes_tag){ _LL404: _temp403=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp391)->f1; goto _LL394;} else{ goto
_LL395;} _LL395: if((( struct _tunion_struct*) _temp391)->tag == Cyc_Tcenv_TunionRes_tag){
_LL408: _temp407=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp391)->f1; goto _LL406; _LL406: _temp405=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp391)->f2; goto _LL396;} else{ goto
_LL397;} _LL397: if((( struct _tunion_struct*) _temp391)->tag == Cyc_Tcenv_XTunionRes_tag){
_LL412: _temp411=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp391)->f1; goto _LL410; _LL410: _temp409=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp391)->f2; goto _LL398;} else{ goto
_LL399;} _LL399: if((( struct _tunion_struct*) _temp391)->tag == Cyc_Tcenv_EnumRes_tag){
_LL416: _temp415=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp391)->f1; goto _LL414; _LL414: _temp413=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp391)->f2; goto _LL400;} else{ goto _LL401;}
_LL401: if((( struct _tunion_struct*) _temp391)->tag == Cyc_Tcenv_VarRes_tag){
_LL418: _temp417=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp391)->f1; goto
_LL402;} else{ goto _LL392;} _LL394: { struct Cyc_List_List* x=(( struct Cyc_List_List*(*)(
struct _tuple6*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcpat_null_designator, _temp100);( void*)( p->r=( void*)(( void*)({ struct
Cyc_Absyn_Struct_p_struct* _temp419=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp419[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp420; _temp420.tag= Cyc_Absyn_Struct_p_tag;
_temp420.f1= _temp403; _temp420.f2= 0; _temp420.f3= _temp102; _temp420.f4= x;
_temp420;}); _temp419;})));{ struct _tuple5 _temp421= Cyc_Tcpat_tcPat( te, p,
topt, region_opt); _npop_handler( 0u); return _temp421;}} _LL396:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp422=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp422[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp423; _temp423.tag= Cyc_Absyn_Tunion_p_tag;
_temp423.f1= _temp405->name; _temp423.f2= 0; _temp423.f3= _temp102; _temp423.f4=
_temp100; _temp423.f5= _temp407; _temp423.f6= _temp405; _temp423;}); _temp422;})));{
struct _tuple5 _temp424= Cyc_Tcpat_tcPat( te, p, topt, region_opt);
_npop_handler( 0u); return _temp424;} _LL398:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_XTunion_p_struct* _temp425=( struct Cyc_Absyn_XTunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct)); _temp425[ 0]=({ struct
Cyc_Absyn_XTunion_p_struct _temp426; _temp426.tag= Cyc_Absyn_XTunion_p_tag;
_temp426.f1= _temp409->name; _temp426.f2= _temp102; _temp426.f3= _temp100;
_temp426.f4= _temp411; _temp426.f5= _temp409; _temp426;}); _temp425;})));{
struct _tuple5 _temp427= Cyc_Tcpat_tcPat( te, p, topt, region_opt);
_npop_handler( 0u); return _temp427;} _LL400: Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp428= Cyc_Absynpp_qvar2string( _temp104); xprintf("bad enum constructor %.*s in pattern",
_temp428.last_plus_one - _temp428.curr, _temp428.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL392; _LL402: goto _LL392; _LL392:;}; _pop_handler();} else{ struct
_xtunion_struct* _temp430= _temp390; _LL432: if( _temp430->tag == Cyc_Dict_Absent_tag){
goto _LL433;} else{ goto _LL434;} _LL434: goto _LL435; _LL433: goto _LL431;
_LL435:( void) _throw( _temp430); _LL431:;}}} Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp436= Cyc_Absynpp_qvar2string( _temp104); xprintf("%.*s is not a constructor in pattern",
_temp436.last_plus_one - _temp436.curr, _temp436.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL6; _LL40:{ struct _handler_cons _temp437; _push_handler(& _temp437);{
struct _xtunion_struct* _temp438=( struct _xtunion_struct*) setjmp( _temp437.handler);
if( ! _temp438){{ void* _temp439= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp110); struct Cyc_Absyn_Structdecl* _temp445; _LL441: if((( struct
_tunion_struct*) _temp439)->tag == Cyc_Tcenv_StructRes_tag){ _LL446: _temp445=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp439)->f1;
goto _LL442;} else{ goto _LL443;} _LL443: goto _LL444; _LL442:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp447=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp447[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp448; _temp448.tag= Cyc_Absyn_Struct_p_tag;
_temp448.f1= _temp445; _temp448.f2= 0; _temp448.f3= _temp108; _temp448.f4=
_temp106; _temp448;}); _temp447;})));{ struct _tuple5 _temp449= Cyc_Tcpat_tcPat(
te, p, topt, region_opt); _npop_handler( 0u); return _temp449;} _LL444: goto
_LL440; _LL440:;}; _pop_handler();} else{ struct _xtunion_struct* _temp451=
_temp438; _LL453: if( _temp451->tag == Cyc_Dict_Absent_tag){ goto _LL454;} else{
goto _LL455;} _LL455: goto _LL456; _LL454: goto _LL452; _LL456:( void) _throw(
_temp451); _LL452:;}}} Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp457=
Cyc_Absynpp_qvar2string( _temp110); xprintf("%.*s is not a struct name, but pattern has designators",
_temp457.last_plus_one - _temp457.curr, _temp457.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL6; _LL6:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt* _temp458=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp458->v=( void*) t;
_temp458;}); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct
_tagged_string*(* f)( struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcpat_get_name, v_result), p->loc,( struct _tagged_string)({ char* _temp459=(
char*)"pattern contains a repeated variable"; struct _tagged_string _temp460;
_temp460.curr= _temp459; _temp460.base= _temp459; _temp460.last_plus_one=
_temp459 + 37; _temp460;})); return({ struct _tuple5 _temp461; _temp461.f1=
tv_result; _temp461.f2= v_result; _temp461;});} void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p){ void* _temp462=( void*) p->r;
struct Cyc_Absyn_Pat* _temp476; struct Cyc_Absyn_Tunionfield* _temp478; struct
Cyc_Absyn_Tuniondecl* _temp480; struct Cyc_List_List* _temp482; struct Cyc_List_List*
_temp484; struct Cyc_Core_Opt* _temp486; struct _tuple1* _temp488; struct Cyc_Absyn_Tunionfield*
_temp490; struct Cyc_Absyn_XTuniondecl* _temp492; struct Cyc_List_List* _temp494;
struct Cyc_List_List* _temp496; struct _tuple1* _temp498; struct Cyc_List_List*
_temp500; struct Cyc_List_List* _temp502; struct Cyc_Core_Opt* _temp504; struct
Cyc_Absyn_Structdecl* _temp506; struct Cyc_List_List* _temp508; _LL464: if((
unsigned int) _temp462 > 2u?(( struct _tunion_struct*) _temp462)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL477: _temp476=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp462)->f1; goto _LL465;} else{ goto _LL466;} _LL466: if(( unsigned int)
_temp462 > 2u?(( struct _tunion_struct*) _temp462)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL489: _temp488=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp462)->f1; goto _LL487; _LL487: _temp486=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp462)->f2; goto _LL485; _LL485: _temp484=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp462)->f3; goto
_LL483; _LL483: _temp482=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp462)->f4; goto _LL481; _LL481: _temp480=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp462)->f5; goto _LL479; _LL479: _temp478=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp462)->f6;
goto _LL467;} else{ goto _LL468;} _LL468: if(( unsigned int) _temp462 > 2u?((
struct _tunion_struct*) _temp462)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL499:
_temp498=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp462)->f1;
goto _LL497; _LL497: _temp496=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp462)->f2; goto _LL495; _LL495: _temp494=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_p_struct*) _temp462)->f3; goto _LL493; _LL493: _temp492=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp462)->f4;
goto _LL491; _LL491: _temp490=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp462)->f5; goto _LL469;} else{ goto _LL470;} _LL470: if(( unsigned int)
_temp462 > 2u?(( struct _tunion_struct*) _temp462)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL507: _temp506=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp462)->f1; goto _LL505; _LL505: _temp504=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp462)->f2; goto _LL503; _LL503: _temp502=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp462)->f3; goto
_LL501; _LL501: _temp500=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp462)->f4; goto _LL471;} else{ goto _LL472;} _LL472: if(( unsigned int)
_temp462 > 2u?(( struct _tunion_struct*) _temp462)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL509: _temp508=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp462)->f1; goto _LL473;} else{ goto _LL474;} _LL474: goto _LL475; _LL465:
Cyc_Tcpat_check_pat_regions( te, _temp476);{ void* _temp511=( void*)({ struct
Cyc_Core_Opt* _temp510= p->topt; if( _temp510 == 0){ _throw( Null_Exception);}
_temp510->v;}); struct Cyc_Absyn_PtrInfo _temp521; struct Cyc_Absyn_Conref*
_temp523; struct Cyc_Absyn_Tqual* _temp525; struct Cyc_Absyn_Conref* _temp527;
void* _temp529; void* _temp531; struct Cyc_Absyn_TunionInfo _temp533; struct Cyc_Absyn_Tuniondecl*
_temp535; void* _temp537; struct Cyc_List_List* _temp539; struct _tuple1*
_temp541; struct Cyc_Absyn_XTunionInfo _temp543; struct Cyc_Absyn_XTuniondecl*
_temp545; void* _temp547; struct _tuple1* _temp549; _LL513: if(( unsigned int)
_temp511 > 4u?(( struct _tunion_struct*) _temp511)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL522: _temp521=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp511)->f1; _LL532: _temp531=( void*) _temp521.elt_typ; goto _LL530; _LL530:
_temp529=( void*) _temp521.rgn_typ; goto _LL528; _LL528: _temp527=( struct Cyc_Absyn_Conref*)
_temp521.nullable; goto _LL526; _LL526: _temp525=( struct Cyc_Absyn_Tqual*)
_temp521.tq; goto _LL524; _LL524: _temp523=( struct Cyc_Absyn_Conref*) _temp521.bounds;
goto _LL514;} else{ goto _LL515;} _LL515: if(( unsigned int) _temp511 > 4u?((
struct _tunion_struct*) _temp511)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL534:
_temp533=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp511)->f1; _LL542: _temp541=( struct _tuple1*) _temp533.name; goto _LL540;
_LL540: _temp539=( struct Cyc_List_List*) _temp533.targs; goto _LL538; _LL538:
_temp537=( void*) _temp533.rgn; goto _LL536; _LL536: _temp535=( struct Cyc_Absyn_Tuniondecl*)
_temp533.tud; goto _LL516;} else{ goto _LL517;} _LL517: if(( unsigned int)
_temp511 > 4u?(( struct _tunion_struct*) _temp511)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL544: _temp543=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp511)->f1; _LL550: _temp549=( struct _tuple1*) _temp543.name; goto _LL548;
_LL548: _temp547=( void*) _temp543.rgn; goto _LL546; _LL546: _temp545=( struct
Cyc_Absyn_XTuniondecl*) _temp543.xtud; goto _LL518;} else{ goto _LL519;} _LL519:
goto _LL520; _LL514: _temp537= _temp529; goto _LL516; _LL516: _temp547= _temp537;
goto _LL518; _LL518: Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp547);
return; _LL520:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp551=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp551[ 0]=({ struct Cyc_Core_Impossible_struct _temp552; _temp552.tag= Cyc_Core_Impossible_tag;
_temp552.f1=( struct _tagged_string)({ char* _temp553=( char*)"check_pat_regions: bad pointer type";
struct _tagged_string _temp554; _temp554.curr= _temp553; _temp554.base= _temp553;
_temp554.last_plus_one= _temp553 + 36; _temp554;}); _temp552;}); _temp551;}));
_LL512:;} _LL467: for( 0; _temp482 != 0; _temp482=({ struct Cyc_List_List*
_temp555= _temp482; if( _temp555 == 0){ _throw( Null_Exception);} _temp555->tl;})){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp556= _temp482; if( _temp556 == 0){ _throw( Null_Exception);} _temp556->hd;}));}{
void* _temp558=( void*)({ struct Cyc_Core_Opt* _temp557= p->topt; if( _temp557
== 0){ _throw( Null_Exception);} _temp557->v;}); struct Cyc_Absyn_TunionInfo
_temp566; struct Cyc_Absyn_Tuniondecl* _temp568; void* _temp570; struct Cyc_List_List*
_temp572; struct _tuple1* _temp574; struct Cyc_Absyn_TunionFieldInfo _temp576;
_LL560: if(( unsigned int) _temp558 > 4u?(( struct _tunion_struct*) _temp558)->tag
== Cyc_Absyn_TunionType_tag: 0){ _LL567: _temp566=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp558)->f1; _LL575: _temp574=( struct
_tuple1*) _temp566.name; goto _LL573; _LL573: _temp572=( struct Cyc_List_List*)
_temp566.targs; goto _LL571; _LL571: _temp570=( void*) _temp566.rgn; goto _LL569;
_LL569: _temp568=( struct Cyc_Absyn_Tuniondecl*) _temp566.tud; goto _LL561;}
else{ goto _LL562;} _LL562: if(( unsigned int) _temp558 > 4u?(( struct
_tunion_struct*) _temp558)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL577:
_temp576=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp558)->f1; goto _LL563;} else{ goto _LL564;} _LL564: goto _LL565; _LL561:
Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp570); return; _LL563: return;
_LL565:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp578=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp578[ 0]=({ struct Cyc_Core_Impossible_struct _temp579; _temp579.tag= Cyc_Core_Impossible_tag;
_temp579.f1=( struct _tagged_string)({ char* _temp580=( char*)"check_pat_regions: bad tunion type";
struct _tagged_string _temp581; _temp581.curr= _temp580; _temp581.base= _temp580;
_temp581.last_plus_one= _temp580 + 35; _temp581;}); _temp579;}); _temp578;}));
_LL559:;} _LL469: for( 0; _temp494 != 0; _temp494=({ struct Cyc_List_List*
_temp582= _temp494; if( _temp582 == 0){ _throw( Null_Exception);} _temp582->tl;})){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp583= _temp494; if( _temp583 == 0){ _throw( Null_Exception);} _temp583->hd;}));}{
void* _temp585=( void*)({ struct Cyc_Core_Opt* _temp584= p->topt; if( _temp584
== 0){ _throw( Null_Exception);} _temp584->v;}); struct Cyc_Absyn_XTunionInfo
_temp593; struct Cyc_Absyn_XTuniondecl* _temp595; void* _temp597; struct _tuple1*
_temp599; struct Cyc_Absyn_XTunionFieldInfo _temp601; _LL587: if(( unsigned int)
_temp585 > 4u?(( struct _tunion_struct*) _temp585)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL594: _temp593=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp585)->f1; _LL600: _temp599=( struct _tuple1*) _temp593.name; goto _LL598;
_LL598: _temp597=( void*) _temp593.rgn; goto _LL596; _LL596: _temp595=( struct
Cyc_Absyn_XTuniondecl*) _temp593.xtud; goto _LL588;} else{ goto _LL589;} _LL589:
if(( unsigned int) _temp585 > 4u?(( struct _tunion_struct*) _temp585)->tag ==
Cyc_Absyn_XTunionFieldType_tag: 0){ _LL602: _temp601=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp585)->f1; goto _LL590;} else{
goto _LL591;} _LL591: goto _LL592; _LL588: Cyc_Tcenv_check_rgn_accessible( te, p->loc,
_temp597); return; _LL590: return; _LL592:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp603=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp603[ 0]=({ struct
Cyc_Core_Impossible_struct _temp604; _temp604.tag= Cyc_Core_Impossible_tag;
_temp604.f1=( struct _tagged_string)({ char* _temp605=( char*)"check_pat_regions: bad xtunion type";
struct _tagged_string _temp606; _temp606.curr= _temp605; _temp606.base= _temp605;
_temp606.last_plus_one= _temp605 + 36; _temp606;}); _temp604;}); _temp603;}));
_LL586:;} _LL471: _temp508=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple6*)) Cyc_Core_snd, _temp500); goto _LL473; _LL473: for( 0; _temp508
!= 0; _temp508=({ struct Cyc_List_List* _temp607= _temp508; if( _temp607 == 0){
_throw( Null_Exception);} _temp607->tl;})){ Cyc_Tcpat_check_pat_regions( te,(
struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp608= _temp508; if( _temp608
== 0){ _throw( Null_Exception);} _temp608->hd;}));} return; _LL475: return;
_LL463:;} typedef void* Cyc_Tcpat_Name_value; static const int Cyc_Tcpat_Name_v_tag=
0; struct Cyc_Tcpat_Name_v_struct{ int tag; struct _tagged_string f1; } ; static
const int Cyc_Tcpat_Int_v_tag= 1; struct Cyc_Tcpat_Int_v_struct{ int tag; int f1;
} ; typedef void* Cyc_Tcpat_name_value_t; struct Cyc_Tcpat_Con_s{ void* name;
int arity; struct Cyc_Core_Opt* span; } ; typedef struct Cyc_Tcpat_Con_s* Cyc_Tcpat_con_t;
typedef void* Cyc_Tcpat_Simple_pat; static const unsigned int Cyc_Tcpat_Any= 0;
static const int Cyc_Tcpat_Con_tag= 0; struct Cyc_Tcpat_Con_struct{ int tag;
struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; typedef void* Cyc_Tcpat_simple_pat_t;
static int Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s*
c2){ void* _temp609=( void*) c1->name; struct _tagged_string _temp615; int
_temp617; _LL611: if((( struct _tunion_struct*) _temp609)->tag == Cyc_Tcpat_Name_v_tag){
_LL616: _temp615=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp609)->f1; goto _LL612;} else{ goto _LL613;} _LL613: if((( struct
_tunion_struct*) _temp609)->tag == Cyc_Tcpat_Int_v_tag){ _LL618: _temp617=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp609)->f1; goto _LL614;} else{ goto _LL610;}
_LL612: { void* _temp619=( void*) c2->name; struct _tagged_string _temp625; int
_temp627; _LL621: if((( struct _tunion_struct*) _temp619)->tag == Cyc_Tcpat_Name_v_tag){
_LL626: _temp625=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp619)->f1; goto _LL622;} else{ goto _LL623;} _LL623: if((( struct
_tunion_struct*) _temp619)->tag == Cyc_Tcpat_Int_v_tag){ _LL628: _temp627=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp619)->f1; goto _LL624;} else{ goto _LL620;}
_LL622: return Cyc_String_zstrcmp( _temp615, _temp625); _LL624: return - 1;
_LL620:;} _LL614: { void* _temp629=( void*) c2->name; struct _tagged_string
_temp635; int _temp637; _LL631: if((( struct _tunion_struct*) _temp629)->tag ==
Cyc_Tcpat_Name_v_tag){ _LL636: _temp635=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp629)->f1; goto _LL632;} else{ goto _LL633;} _LL633: if((( struct
_tunion_struct*) _temp629)->tag == Cyc_Tcpat_Int_v_tag){ _LL638: _temp637=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp629)->f1; goto _LL634;} else{ goto _LL630;}
_LL632: return 1; _LL634: return _temp617 - _temp637; _LL630:;} _LL610:;} static
struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct Cyc_Set_Set*(*)(
int(* comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*))) Cyc_Set_empty)(
Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt=( struct
Cyc_Core_Opt){.v=( void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt=(
struct Cyc_Core_Opt){.v=( void*) 2}; static struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static char _temp642[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={ 0u,( struct
_tagged_string){ _temp642, _temp642, _temp642 + 5u}}; static char _temp646[ 4u]="ptr";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={ 0u,( struct
_tagged_string){ _temp646, _temp646, _temp646 + 4u}}; static char _temp650[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,( struct
_tagged_string){ _temp650, _temp650, _temp650 + 6u}}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_null_name_value),.arity=
0,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt}; static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=&
Cyc_Tcpat_null_ptr_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp651=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp651->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp652=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp652[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp653; _temp653.tag= Cyc_Tcpat_Int_v_tag;
_temp653.f1= i; _temp653;}); _temp652;})); _temp651->arity= 0; _temp651->span= 0;
_temp651;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_char_con( char c){ return({
struct Cyc_Tcpat_Con_s* _temp654=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp654->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct*
_temp656=( struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp656[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp657; _temp657.tag= Cyc_Tcpat_Int_v_tag;
_temp657.f1=( int) c; _temp657;}); _temp656;})); _temp654->arity= 0; _temp654->span=({
struct Cyc_Core_Opt* _temp655=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp655->v=( void*) 256; _temp655;}); _temp654;});}
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct _tagged_string f){
return({ struct Cyc_Tcpat_Con_s* _temp658=( struct Cyc_Tcpat_Con_s*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_s)); _temp658->name=( void*)(( void*)({ struct Cyc_Tcpat_Name_v_struct*
_temp659=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct));
_temp659[ 0]=({ struct Cyc_Tcpat_Name_v_struct _temp660; _temp660.tag= Cyc_Tcpat_Name_v_tag;
_temp660.f1= f; _temp660;}); _temp659;})); _temp658->arity= 0; _temp658->span= 0;
_temp658;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_double_con( struct
_tagged_string d){ return({ struct Cyc_Tcpat_Con_s* _temp661=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp661->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp662=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp662[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp663; _temp663.tag= Cyc_Tcpat_Name_v_tag; _temp663.f1= d; _temp663;});
_temp662;})); _temp661->arity= 0; _temp661->span= 0; _temp661;});} static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp664=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp664->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp664->arity=
i; _temp664->span= Cyc_Tcpat_one_opt_ptr; _temp664;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp665=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp665[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp666; _temp666.tag= Cyc_Tcpat_Con_tag; _temp666.f1= Cyc_Tcpat_null_con;
_temp666.f2= 0; _temp666;}); _temp665;});} static void* Cyc_Tcpat_int_pat( int i){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp667=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp667[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp668; _temp668.tag= Cyc_Tcpat_Con_tag; _temp668.f1= Cyc_Tcpat_int_con( i);
_temp668.f2= 0; _temp668;}); _temp667;});} static void* Cyc_Tcpat_char_pat( char
c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp669=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp669[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp670; _temp670.tag= Cyc_Tcpat_Con_tag; _temp670.f1= Cyc_Tcpat_char_con( c);
_temp670.f2= 0; _temp670;}); _temp669;});} static void* Cyc_Tcpat_float_pat(
struct _tagged_string f){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp671=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp671[ 0]=({ struct Cyc_Tcpat_Con_struct _temp672; _temp672.tag= Cyc_Tcpat_Con_tag;
_temp672.f1= Cyc_Tcpat_float_con( f); _temp672.f2= 0; _temp672;}); _temp671;});}
static void* Cyc_Tcpat_double_pat( struct _tagged_string d){ return( void*)({
struct Cyc_Tcpat_Con_struct* _temp673=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp673[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp674; _temp674.tag= Cyc_Tcpat_Con_tag; _temp674.f1= Cyc_Tcpat_double_con( d);
_temp674.f2= 0; _temp674;}); _temp673;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp675=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp675[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp676; _temp676.tag= Cyc_Tcpat_Con_tag; _temp676.f1= Cyc_Tcpat_null_ptr_con;
_temp676.f2=({ struct Cyc_List_List* _temp677=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp677->hd=( void*) p; _temp677->tl= 0;
_temp677;}); _temp676;}); _temp675;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp678=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp678[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp679; _temp679.tag= Cyc_Tcpat_Con_tag; _temp679.f1= Cyc_Tcpat_ptr_con;
_temp679.f2=({ struct Cyc_List_List* _temp680=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp680->hd=( void*) p; _temp680->tl= 0;
_temp680;}); _temp679;}); _temp678;});} static void* Cyc_Tcpat_tuple_pat( struct
Cyc_List_List* ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp681=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp681[ 0]=({ struct Cyc_Tcpat_Con_struct _temp682; _temp682.tag= Cyc_Tcpat_Con_tag;
_temp682.f1= Cyc_Tcpat_tuple_con((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ss)); _temp682.f2= ss; _temp682;}); _temp681;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_string con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List*
ps){ struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp683=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp683->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp684=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp684[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp685; _temp685.tag= Cyc_Tcpat_Name_v_tag; _temp685.f1= con_name; _temp685;});
_temp684;})); _temp683->arity=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ps); _temp683->span= span; _temp683;}); return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp686=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp686[ 0]=({ struct Cyc_Tcpat_Con_struct _temp687; _temp687.tag= Cyc_Tcpat_Con_tag;
_temp687.f1= c; _temp687.f2= ps; _temp687;}); _temp686;});} static void* Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat* p){ void* s;{ void* _temp688=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp722; int _temp724; void* _temp726; char _temp728; struct _tagged_string
_temp730; struct Cyc_List_List* _temp732; struct Cyc_Absyn_Vardecl* _temp734;
struct Cyc_Absyn_Pat* _temp736; struct Cyc_List_List* _temp738; struct Cyc_List_List*
_temp740; struct Cyc_Core_Opt* _temp742; struct Cyc_Absyn_Structdecl* _temp744;
struct Cyc_Absyn_Tunionfield* _temp746; struct Cyc_Absyn_Tuniondecl* _temp748;
struct Cyc_List_List* _temp750; struct Cyc_List_List* _temp752; struct Cyc_Core_Opt*
_temp754; struct _tuple1* _temp756; struct Cyc_Absyn_Tunionfield* _temp758;
struct Cyc_Absyn_XTuniondecl* _temp760; struct Cyc_List_List* _temp762; struct
Cyc_List_List* _temp764; struct _tuple1* _temp766; struct Cyc_Absyn_Enumfield*
_temp768; struct Cyc_Absyn_Enumdecl* _temp770; struct _tuple1* _temp772; struct
_tuple1* _temp774; struct Cyc_List_List* _temp776; struct Cyc_List_List*
_temp778; struct _tuple1* _temp780; struct Cyc_List_List* _temp782; struct Cyc_List_List*
_temp784; struct _tuple1* _temp786; _LL690: if(( int) _temp688 == Cyc_Absyn_Wild_p){
goto _LL691;} else{ goto _LL692;} _LL692: if(( unsigned int) _temp688 > 2u?((
struct _tunion_struct*) _temp688)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL723:
_temp722=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*) _temp688)->f1;
goto _LL693;} else{ goto _LL694;} _LL694: if(( int) _temp688 == Cyc_Absyn_Null_p){
goto _LL695;} else{ goto _LL696;} _LL696: if(( unsigned int) _temp688 > 2u?((
struct _tunion_struct*) _temp688)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL727:
_temp726=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp688)->f1; goto _LL725;
_LL725: _temp724=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp688)->f2; goto
_LL697;} else{ goto _LL698;} _LL698: if(( unsigned int) _temp688 > 2u?(( struct
_tunion_struct*) _temp688)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL729: _temp728=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp688)->f1; goto _LL699;} else{ goto
_LL700;} _LL700: if(( unsigned int) _temp688 > 2u?(( struct _tunion_struct*)
_temp688)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL731: _temp730=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp688)->f1; goto _LL701;}
else{ goto _LL702;} _LL702: if(( unsigned int) _temp688 > 2u?(( struct
_tunion_struct*) _temp688)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL733: _temp732=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp688)->f1; goto
_LL703;} else{ goto _LL704;} _LL704: if(( unsigned int) _temp688 > 2u?(( struct
_tunion_struct*) _temp688)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL735:
_temp734=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp688)->f1; goto _LL705;} else{ goto _LL706;} _LL706: if(( unsigned int)
_temp688 > 2u?(( struct _tunion_struct*) _temp688)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL737: _temp736=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp688)->f1; goto _LL707;} else{ goto _LL708;} _LL708: if(( unsigned int)
_temp688 > 2u?(( struct _tunion_struct*) _temp688)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL745: _temp744=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp688)->f1; goto _LL743; _LL743: _temp742=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp688)->f2; goto _LL741; _LL741: _temp740=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp688)->f3; goto
_LL739; _LL739: _temp738=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp688)->f4; goto _LL709;} else{ goto _LL710;} _LL710: if(( unsigned int)
_temp688 > 2u?(( struct _tunion_struct*) _temp688)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL757: _temp756=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp688)->f1; goto _LL755; _LL755: _temp754=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp688)->f2; goto _LL753; _LL753: _temp752=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp688)->f3; goto
_LL751; _LL751: _temp750=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp688)->f4; goto _LL749; _LL749: _temp748=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp688)->f5; goto _LL747; _LL747: _temp746=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp688)->f6;
goto _LL711;} else{ goto _LL712;} _LL712: if(( unsigned int) _temp688 > 2u?((
struct _tunion_struct*) _temp688)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL767:
_temp766=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp688)->f1;
goto _LL765; _LL765: _temp764=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp688)->f2; goto _LL763; _LL763: _temp762=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_p_struct*) _temp688)->f3; goto _LL761; _LL761: _temp760=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp688)->f4;
goto _LL759; _LL759: _temp758=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp688)->f5; goto _LL713;} else{ goto _LL714;} _LL714: if(( unsigned int)
_temp688 > 2u?(( struct _tunion_struct*) _temp688)->tag == Cyc_Absyn_Enum_p_tag:
0){ _LL773: _temp772=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp688)->f1; goto _LL771; _LL771: _temp770=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp688)->f2; goto _LL769; _LL769: _temp768=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*) _temp688)->f3;
goto _LL715;} else{ goto _LL716;} _LL716: if(( unsigned int) _temp688 > 2u?((
struct _tunion_struct*) _temp688)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL775:
_temp774=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp688)->f1;
goto _LL717;} else{ goto _LL718;} _LL718: if(( unsigned int) _temp688 > 2u?((
struct _tunion_struct*) _temp688)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){
_LL781: _temp780=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp688)->f1; goto _LL779; _LL779: _temp778=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp688)->f2; goto _LL777; _LL777: _temp776=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp688)->f3;
goto _LL719;} else{ goto _LL720;} _LL720: if(( unsigned int) _temp688 > 2u?((
struct _tunion_struct*) _temp688)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){
_LL787: _temp786=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp688)->f1; goto _LL785; _LL785: _temp784=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp688)->f2; goto _LL783; _LL783: _temp782=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp688)->f3;
goto _LL721;} else{ goto _LL689;} _LL691: s=( void*) Cyc_Tcpat_Any; goto _LL689;
_LL693: s=( void*) Cyc_Tcpat_Any; goto _LL689; _LL695: s= Cyc_Tcpat_null_pat();
goto _LL689; _LL697: s= Cyc_Tcpat_int_pat( _temp724); goto _LL689; _LL699: s=
Cyc_Tcpat_char_pat( _temp728); goto _LL689; _LL701: s= Cyc_Tcpat_float_pat(
_temp730); goto _LL689; _LL703: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp732)); goto _LL689; _LL705: s=( void*) Cyc_Tcpat_Any; goto _LL689; _LL707: {
void* ss= Cyc_Tcpat_compile_pat( _temp736);{ void* _temp789= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp788= p->topt; if( _temp788 == 0){ _throw(
Null_Exception);} _temp788->v;})); struct Cyc_Absyn_PtrInfo _temp799; struct Cyc_Absyn_Conref*
_temp801; struct Cyc_Absyn_Tqual* _temp803; struct Cyc_Absyn_Conref* _temp805;
void* _temp807; void* _temp809; struct Cyc_Absyn_TunionInfo _temp811; struct Cyc_Absyn_XTunionInfo
_temp813; _LL791: if(( unsigned int) _temp789 > 4u?(( struct _tunion_struct*)
_temp789)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL800: _temp799=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp789)->f1; _LL810: _temp809=( void*)
_temp799.elt_typ; goto _LL808; _LL808: _temp807=( void*) _temp799.rgn_typ; goto
_LL806; _LL806: _temp805=( struct Cyc_Absyn_Conref*) _temp799.nullable; goto
_LL804; _LL804: _temp803=( struct Cyc_Absyn_Tqual*) _temp799.tq; goto _LL802;
_LL802: _temp801=( struct Cyc_Absyn_Conref*) _temp799.bounds; goto _LL792;}
else{ goto _LL793;} _LL793: if(( unsigned int) _temp789 > 4u?(( struct
_tunion_struct*) _temp789)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL812:
_temp811=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp789)->f1; goto _LL794;} else{ goto _LL795;} _LL795: if(( unsigned int)
_temp789 > 4u?(( struct _tunion_struct*) _temp789)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL814: _temp813=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp789)->f1; goto _LL796;} else{ goto _LL797;} _LL797: goto _LL798; _LL792: {
int is_nullable= 0; int still_working= 1; while( still_working) { void* _temp815=(
void*) _temp805->v; struct Cyc_Absyn_Conref* _temp823; int _temp825; _LL817: if((
unsigned int) _temp815 > 1u?(( struct _tunion_struct*) _temp815)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL824: _temp823=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp815)->f1; goto _LL818;} else{ goto _LL819;} _LL819: if(( int) _temp815 ==
Cyc_Absyn_No_constr){ goto _LL820;} else{ goto _LL821;} _LL821: if((
unsigned int) _temp815 > 1u?(( struct _tunion_struct*) _temp815)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL826: _temp825=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp815)->f1;
goto _LL822;} else{ goto _LL816;} _LL818:( void*)( _temp805->v=( void*)(( void*)
_temp823->v)); continue; _LL820:( void*)( _temp805->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp827=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp827[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp828; _temp828.tag= Cyc_Absyn_Eq_constr_tag;
_temp828.f1=( void*) 0; _temp828;}); _temp827;}))); is_nullable= 0;
still_working= 0; goto _LL816; _LL822: is_nullable=( int) _temp825;
still_working= 0; goto _LL816; _LL816:;} if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL790;} _LL794: s= ss; goto _LL790;
_LL796: s= ss; goto _LL790; _LL798: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp829=( char*)"pointer pattern does not have pointer type!";
struct _tagged_string _temp830; _temp830.curr= _temp829; _temp830.base= _temp829;
_temp830.last_plus_one= _temp829 + 44; _temp830;})); goto _LL790; _LL790:;} goto
_LL689;} _LL709: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp831= _temp744->fields; if(
_temp831 == 0){ _throw( Null_Exception);} _temp831->v;}); for( 0; fields != 0;
fields=({ struct Cyc_List_List* _temp832= fields; if( _temp832 == 0){ _throw(
Null_Exception);} _temp832->tl;})){ int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp833= fields; if( _temp833 == 0){ _throw(
Null_Exception);} _temp833->hd;}))->name,( struct _tagged_string)({ char*
_temp834=( char*)""; struct _tagged_string _temp835; _temp835.curr= _temp834;
_temp835.base= _temp834; _temp835.last_plus_one= _temp834 + 1; _temp835;})) == 0;{
struct Cyc_List_List* dlps0= _temp738; for( 0; ! found? dlps0 != 0: 0; dlps0=({
struct Cyc_List_List* _temp836= dlps0; if( _temp836 == 0){ _throw(
Null_Exception);} _temp836->tl;})){ struct _tuple6 _temp840; struct Cyc_Absyn_Pat*
_temp841; struct Cyc_List_List* _temp843; struct _tuple6* _temp838=( struct
_tuple6*)({ struct Cyc_List_List* _temp837= dlps0; if( _temp837 == 0){ _throw(
Null_Exception);} _temp837->hd;}); _temp840=* _temp838; _LL844: _temp843=
_temp840.f1; goto _LL842; _LL842: _temp841= _temp840.f2; goto _LL839; _LL839: {
struct Cyc_List_List* _temp845= _temp843; struct Cyc_List_List _temp851; struct
Cyc_List_List* _temp852; void* _temp854; struct _tagged_string* _temp856; _LL847:
if( _temp845 == 0){ goto _LL849;} else{ _temp851=* _temp845; _LL855: _temp854=(
void*) _temp851.hd; if((( struct _tunion_struct*) _temp854)->tag == Cyc_Absyn_FieldName_tag){
_LL857: _temp856=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp854)->f1; goto _LL853;} else{ goto _LL849;} _LL853: _temp852=( struct Cyc_List_List*)
_temp851.tl; if( _temp852 == 0){ goto _LL848;} else{ goto _LL849;}} _LL849: goto
_LL850; _LL848: if( Cyc_String_zstrptrcmp( _temp856,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp858= fields; if( _temp858 == 0){ _throw(
Null_Exception);} _temp858->hd;}))->name) == 0){ ps=({ struct Cyc_List_List*
_temp859=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp859->hd=( void*) Cyc_Tcpat_compile_pat( _temp841); _temp859->tl= ps;
_temp859;}); found= 1;} goto _LL846; _LL850:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp860=( char*)"bad designator(s)";
struct _tagged_string _temp861; _temp861.curr= _temp860; _temp861.base= _temp860;
_temp861.last_plus_one= _temp860 + 18; _temp861;})); goto _LL846; _LL846:;}}}
if( ! found){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp862=( char*)"bad designator"; struct _tagged_string
_temp863; _temp863.curr= _temp862; _temp863.base= _temp862; _temp863.last_plus_one=
_temp862 + 15; _temp863;}));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL689;}
_LL711: { int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp864= _temp748->fields; if(
_temp864 == 0){ _throw( Null_Exception);} _temp864->v;})); s= Cyc_Tcpat_con_pat(*(*
_temp746->name).f2,({ struct Cyc_Core_Opt* _temp865=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp865->v=( void*) span;
_temp865;}),(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp750)); goto
_LL689;} _LL713: s= Cyc_Tcpat_con_pat(*(* _temp758->name).f2, 0,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp762)); goto _LL689; _LL715: { int span=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp770->fields); s= Cyc_Tcpat_con_pat(*(* _temp768->name).f2,({
struct Cyc_Core_Opt* _temp866=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp866->v=( void*) span; _temp866;}), 0); goto _LL689;}
_LL717: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp867=( char*)"compile_pat: unknownid"; struct
_tagged_string _temp868; _temp868.curr= _temp867; _temp868.base= _temp867;
_temp868.last_plus_one= _temp867 + 23; _temp868;})); goto _LL689; _LL719: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp869=( char*)"compile_pat: unknowncall"; struct _tagged_string
_temp870; _temp870.curr= _temp869; _temp870.base= _temp869; _temp870.last_plus_one=
_temp869 + 25; _temp870;})); goto _LL689; _LL721: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp871=(
char*)"compile_pat: unknownfields"; struct _tagged_string _temp872; _temp872.curr=
_temp871; _temp872.base= _temp871; _temp872.last_plus_one= _temp871 + 27;
_temp872;})); goto _LL689; _LL689:;} return s;} typedef void* Cyc_Tcpat_Term_desc;
static const int Cyc_Tcpat_Pos_tag= 0; struct Cyc_Tcpat_Pos_struct{ int tag;
struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Tcpat_Neg_tag=
1; struct Cyc_Tcpat_Neg_struct{ int tag; struct Cyc_Set_Set* f1; } ; typedef
void* Cyc_Tcpat_term_desc_t; typedef void* Cyc_Tcpat_Access; static const
unsigned int Cyc_Tcpat_Obj= 0; static const int Cyc_Tcpat_Sel_tag= 0; struct Cyc_Tcpat_Sel_struct{
int tag; int f1; void* f2; } ; typedef void* Cyc_Tcpat_access_t; typedef void*
Cyc_Tcpat_Decision; static const unsigned int Cyc_Tcpat_Failure= 0; static const
int Cyc_Tcpat_Success_tag= 0; struct Cyc_Tcpat_Success_struct{ int tag; void* f1;
} ; static const int Cyc_Tcpat_IfEq_tag= 1; struct Cyc_Tcpat_IfEq_struct{ int
tag; void* f1; struct Cyc_Tcpat_Con_s* f2; void* f3; void* f4; } ; typedef void*
Cyc_Tcpat_decision_t; typedef void* Cyc_Tcpat_Answer; static const unsigned int
Cyc_Tcpat_Yes= 0; static const unsigned int Cyc_Tcpat_No= 1; static const
unsigned int Cyc_Tcpat_Maybe= 2; typedef void* Cyc_Tcpat_answer_t; static void*
Cyc_Tcpat_add_neg( void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp873= td;
struct Cyc_Set_Set* _temp879; struct Cyc_List_List* _temp881; struct Cyc_Tcpat_Con_s*
_temp883; _LL875: if((( struct _tunion_struct*) _temp873)->tag == Cyc_Tcpat_Neg_tag){
_LL880: _temp879=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp873)->f1;
goto _LL876;} else{ goto _LL877;} _LL877: if((( struct _tunion_struct*) _temp873)->tag
== Cyc_Tcpat_Pos_tag){ _LL884: _temp883=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*)
_temp873)->f1; goto _LL882; _LL882: _temp881=( struct Cyc_List_List*)(( struct
Cyc_Tcpat_Pos_struct*) _temp873)->f2; goto _LL878;} else{ goto _LL874;} _LL876:
if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp879, c)){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp885=( char*)"add_neg called when constructor already in set";
struct _tagged_string _temp886; _temp886.curr= _temp885; _temp886.base= _temp885;
_temp886.last_plus_one= _temp885 + 47; _temp886;}));} if( c->span != 0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp879) + 1 >=( int)({ struct
Cyc_Core_Opt* _temp887= c->span; if( _temp887 == 0){ _throw( Null_Exception);}
_temp887->v;}): 0){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp888=( char*)"add_neg called when |cs U {c}| >= span(c)";
struct _tagged_string _temp889; _temp889.curr= _temp888; _temp889.base= _temp888;
_temp889.last_plus_one= _temp888 + 42; _temp889;}));} return( void*)({ struct
Cyc_Tcpat_Neg_struct* _temp890=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp890[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp891; _temp891.tag= Cyc_Tcpat_Neg_tag; _temp891.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp879,
c); _temp891;}); _temp890;}); _LL878: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp892=( char*)"add_neg called when td is Positive";
struct _tagged_string _temp893; _temp893.curr= _temp892; _temp893.base= _temp892;
_temp893.last_plus_one= _temp892 + 35; _temp893;})); _LL874:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp894= td; struct Cyc_List_List*
_temp900; struct Cyc_Tcpat_Con_s* _temp902; struct Cyc_Set_Set* _temp904; _LL896:
if((( struct _tunion_struct*) _temp894)->tag == Cyc_Tcpat_Pos_tag){ _LL903:
_temp902=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp894)->f1;
goto _LL901; _LL901: _temp900=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp894)->f2; goto _LL897;} else{ goto _LL898;} _LL898: if((( struct
_tunion_struct*) _temp894)->tag == Cyc_Tcpat_Neg_tag){ _LL905: _temp904=( struct
Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp894)->f1; goto _LL899;} else{
goto _LL895;} _LL897: if( Cyc_Tcpat_compare_con( c, _temp902) == 0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL899: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp904,
c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)({ struct Cyc_Core_Opt*
_temp906= c->span; if( _temp906 == 0){ _throw( Null_Exception);} _temp906->v;})
==(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp904) + 1: 0){
return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}} _LL895:;}
static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt, void*
dsc){ struct Cyc_List_List* _temp907= ctxt; struct Cyc_List_List _temp913;
struct Cyc_List_List* _temp914; struct _tuple9* _temp916; struct _tuple9
_temp918; struct Cyc_List_List* _temp919; struct Cyc_Tcpat_Con_s* _temp921;
_LL909: if( _temp907 == 0){ goto _LL910;} else{ goto _LL911;} _LL911: if(
_temp907 == 0){ goto _LL908;} else{ _temp913=* _temp907; _LL917: _temp916=(
struct _tuple9*) _temp913.hd; _temp918=* _temp916; _LL922: _temp921= _temp918.f1;
goto _LL920; _LL920: _temp919= _temp918.f2; goto _LL915; _LL915: _temp914=(
struct Cyc_List_List*) _temp913.tl; goto _LL912;} _LL910: return 0; _LL912:
return({ struct Cyc_List_List* _temp923=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp923->hd=( void*)({ struct _tuple9* _temp924=(
struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp924->f1= _temp921;
_temp924->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp925=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp925->hd=( void*)
dsc; _temp925->tl= _temp919; _temp925;}); _temp924;}); _temp923->tl= _temp914;
_temp923;}); _LL908:;} static struct Cyc_List_List* Cyc_Tcpat_norm_context(
struct Cyc_List_List* ctxt){ struct Cyc_List_List* _temp926= ctxt; struct Cyc_List_List
_temp932; struct Cyc_List_List* _temp933; struct _tuple9* _temp935; struct
_tuple9 _temp937; struct Cyc_List_List* _temp938; struct Cyc_Tcpat_Con_s*
_temp940; _LL928: if( _temp926 == 0){ goto _LL929;} else{ goto _LL930;} _LL930:
if( _temp926 == 0){ goto _LL927;} else{ _temp932=* _temp926; _LL936: _temp935=(
struct _tuple9*) _temp932.hd; _temp937=* _temp935; _LL941: _temp940= _temp937.f1;
goto _LL939; _LL939: _temp938= _temp937.f2; goto _LL934; _LL934: _temp933=(
struct Cyc_List_List*) _temp932.tl; goto _LL931;} _LL929: return(( struct Cyc_List_List*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp942=( char*)"norm_context: empty context"; struct _tagged_string _temp943;
_temp943.curr= _temp942; _temp943.base= _temp942; _temp943.last_plus_one=
_temp942 + 28; _temp943;})); _LL931: return Cyc_Tcpat_augment( _temp933,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp944=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp944[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp945; _temp945.tag= Cyc_Tcpat_Pos_tag; _temp945.f1= _temp940; _temp945.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp938);
_temp945;}); _temp944;})); _LL927:;} static void* Cyc_Tcpat_build_desc( struct
Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple5
_temp947=({ struct _tuple5 _temp946; _temp946.f1= ctxt; _temp946.f2= work;
_temp946;}); struct Cyc_List_List* _temp957; struct Cyc_List_List* _temp959;
struct Cyc_List_List* _temp961; struct Cyc_List_List* _temp963; struct Cyc_List_List*
_temp965; struct Cyc_List_List* _temp967; struct Cyc_List_List* _temp969; struct
Cyc_List_List _temp971; struct Cyc_List_List* _temp972; struct _tuple10*
_temp974; struct _tuple10 _temp976; struct Cyc_List_List* _temp977; struct Cyc_List_List*
_temp979; struct Cyc_List_List* _temp981; struct Cyc_List_List* _temp983; struct
Cyc_List_List _temp985; struct Cyc_List_List* _temp986; struct _tuple9* _temp988;
struct _tuple9 _temp990; struct Cyc_List_List* _temp991; struct Cyc_Tcpat_Con_s*
_temp993; _LL949: _LL960: _temp959= _temp947.f1; if( _temp959 == 0){ goto _LL958;}
else{ goto _LL951;} _LL958: _temp957= _temp947.f2; if( _temp957 == 0){ goto
_LL950;} else{ goto _LL951;} _LL951: _LL964: _temp963= _temp947.f1; if( _temp963
== 0){ goto _LL962;} else{ goto _LL953;} _LL962: _temp961= _temp947.f2; goto
_LL952; _LL953: _LL968: _temp967= _temp947.f1; goto _LL966; _LL966: _temp965=
_temp947.f2; if( _temp965 == 0){ goto _LL954;} else{ goto _LL955;} _LL955:
_LL984: _temp983= _temp947.f1; if( _temp983 == 0){ goto _LL948;} else{ _temp985=*
_temp983; _LL989: _temp988=( struct _tuple9*) _temp985.hd; _temp990=* _temp988;
_LL994: _temp993= _temp990.f1; goto _LL992; _LL992: _temp991= _temp990.f2; goto
_LL987; _LL987: _temp986=( struct Cyc_List_List*) _temp985.tl; goto _LL970;}
_LL970: _temp969= _temp947.f2; if( _temp969 == 0){ goto _LL948;} else{ _temp971=*
_temp969; _LL975: _temp974=( struct _tuple10*) _temp971.hd; _temp976=* _temp974;
_LL982: _temp981= _temp976.f1; goto _LL980; _LL980: _temp979= _temp976.f2; goto
_LL978; _LL978: _temp977= _temp976.f3; goto _LL973; _LL973: _temp972=( struct
Cyc_List_List*) _temp971.tl; goto _LL956;} _LL950: return dsc; _LL952: return((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp995=( char*)"build_desc: ctxt and work don't match"; struct
_tagged_string _temp996; _temp996.curr= _temp995; _temp996.base= _temp995;
_temp996.last_plus_one= _temp995 + 38; _temp996;})); _LL954: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp997=( char*)"build_desc: ctxt and work don't match"; struct _tagged_string
_temp998; _temp998.curr= _temp997; _temp998.base= _temp997; _temp998.last_plus_one=
_temp997 + 38; _temp998;})); _LL956: { struct Cyc_Tcpat_Pos_struct* td=({ struct
Cyc_Tcpat_Pos_struct* _temp999=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp999[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp1000; _temp1000.tag= Cyc_Tcpat_Pos_tag; _temp1000.f1= _temp993; _temp1000.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp991),({
struct Cyc_List_List* _temp1001=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1001->hd=( void*) dsc; _temp1001->tl= _temp977;
_temp1001;})); _temp1000;}); _temp999;}); return Cyc_Tcpat_build_desc( _temp986,(
void*) td, _temp972);} _LL948:;} static void* Cyc_Tcpat_match( void* p, void*
obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp1002=
allmrules; struct Cyc_List_List _temp1008; struct Cyc_List_List* _temp1009;
struct _tuple0* _temp1011; struct _tuple0 _temp1013; void* _temp1014; void*
_temp1016; _LL1004: if( _temp1002 == 0){ goto _LL1005;} else{ goto _LL1006;}
_LL1006: if( _temp1002 == 0){ goto _LL1003;} else{ _temp1008=* _temp1002;
_LL1012: _temp1011=( struct _tuple0*) _temp1008.hd; _temp1013=* _temp1011;
_LL1017: _temp1016= _temp1013.f1; goto _LL1015; _LL1015: _temp1014= _temp1013.f2;
goto _LL1010; _LL1010: _temp1009=( struct Cyc_List_List*) _temp1008.tl; goto
_LL1007;} _LL1005: return( void*) Cyc_Tcpat_Failure; _LL1007: return(( void*(*)(
void* p, void* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)(
_temp1016,( void*) Cyc_Tcpat_Obj, dsc, 0, 0, _temp1014, _temp1009); _LL1003:;}
static void* Cyc_Tcpat_match_compile( struct Cyc_List_List* allmrules){ return((
void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)((
void*)({ struct Cyc_Tcpat_Neg_struct* _temp1018=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp1018[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp1019; _temp1019.tag= Cyc_Tcpat_Neg_tag; _temp1019.f1= Cyc_Tcpat_empty_con_set();
_temp1019;}); _temp1018;}), allmrules);} static void* Cyc_Tcpat_and_match(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules){ struct Cyc_List_List* _temp1020= work; struct Cyc_List_List
_temp1028; struct Cyc_List_List* _temp1029; struct _tuple10* _temp1031; struct
_tuple10 _temp1033; struct Cyc_List_List* _temp1034; struct Cyc_List_List*
_temp1036; struct Cyc_List_List* _temp1038; struct Cyc_List_List _temp1040;
struct Cyc_List_List* _temp1041; struct _tuple10* _temp1043; struct _tuple10
_temp1045; struct Cyc_List_List* _temp1046; struct Cyc_List_List* _temp1048;
struct Cyc_List_List* _temp1050; _LL1022: if( _temp1020 == 0){ goto _LL1023;}
else{ goto _LL1024;} _LL1024: if( _temp1020 == 0){ goto _LL1026;} else{
_temp1028=* _temp1020; _LL1032: _temp1031=( struct _tuple10*) _temp1028.hd;
_temp1033=* _temp1031; _LL1039: _temp1038= _temp1033.f1; if( _temp1038 == 0){
goto _LL1037;} else{ goto _LL1026;} _LL1037: _temp1036= _temp1033.f2; if(
_temp1036 == 0){ goto _LL1035;} else{ goto _LL1026;} _LL1035: _temp1034=
_temp1033.f3; if( _temp1034 == 0){ goto _LL1030;} else{ goto _LL1026;} _LL1030:
_temp1029=( struct Cyc_List_List*) _temp1028.tl; goto _LL1025;} _LL1026: if(
_temp1020 == 0){ goto _LL1021;} else{ _temp1040=* _temp1020; _LL1044: _temp1043=(
struct _tuple10*) _temp1040.hd; _temp1045=* _temp1043; _LL1051: _temp1050=
_temp1045.f1; goto _LL1049; _LL1049: _temp1048= _temp1045.f2; goto _LL1047;
_LL1047: _temp1046= _temp1045.f3; goto _LL1042; _LL1042: _temp1041=( struct Cyc_List_List*)
_temp1040.tl; goto _LL1027;} _LL1023: return( void*)({ struct Cyc_Tcpat_Success_struct*
_temp1052=( struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp1052[ 0]=({ struct Cyc_Tcpat_Success_struct _temp1053; _temp1053.tag= Cyc_Tcpat_Success_tag;
_temp1053.f1=( void*) right_hand_side; _temp1053;}); _temp1052;}); _LL1025:
return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context(
ctx), _temp1029, right_hand_side, rules); _LL1027: if(( _temp1050 == 0? 1:
_temp1048 == 0)? 1: _temp1046 == 0){ return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1054=( char*)"tcpat:and_match: malformed work frame";
struct _tagged_string _temp1055; _temp1055.curr= _temp1054; _temp1055.base=
_temp1054; _temp1055.last_plus_one= _temp1054 + 38; _temp1055;}));}{ struct Cyc_List_List
_temp1059; struct Cyc_List_List* _temp1060; void* _temp1062; struct Cyc_List_List*
_temp1057=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1056= _temp1050;
if( _temp1056 == 0){ _throw( Null_Exception);} _temp1056;}); _temp1059=*
_temp1057; _LL1063: _temp1062=( void*) _temp1059.hd; goto _LL1061; _LL1061:
_temp1060=( struct Cyc_List_List*) _temp1059.tl; goto _LL1058; _LL1058: { struct
Cyc_List_List _temp1067; struct Cyc_List_List* _temp1068; void* _temp1070;
struct Cyc_List_List* _temp1065=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1064= _temp1048; if( _temp1064 == 0){ _throw( Null_Exception);} _temp1064;});
_temp1067=* _temp1065; _LL1071: _temp1070=( void*) _temp1067.hd; goto _LL1069;
_LL1069: _temp1068=( struct Cyc_List_List*) _temp1067.tl; goto _LL1066; _LL1066: {
struct Cyc_List_List _temp1075; struct Cyc_List_List* _temp1076; void* _temp1078;
struct Cyc_List_List* _temp1073=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1072= _temp1046; if( _temp1072 == 0){ _throw( Null_Exception);} _temp1072;});
_temp1075=* _temp1073; _LL1079: _temp1078=( void*) _temp1075.hd; goto _LL1077;
_LL1077: _temp1076=( struct Cyc_List_List*) _temp1075.tl; goto _LL1074; _LL1074: {
struct _tuple10* wf=({ struct _tuple10* _temp1080=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp1080->f1= _temp1060; _temp1080->f2= _temp1068;
_temp1080->f3= _temp1076; _temp1080;}); return(( void*(*)( void* p, void* obj,
void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp1062,
_temp1070, _temp1078, ctx,({ struct Cyc_List_List* _temp1081=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1081->hd=( void*) wf; _temp1081->tl=
_temp1041; _temp1081;}), right_hand_side, rules);}}}} _LL1021:;} static struct
Cyc_List_List* Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon, void* dsc){
void* _temp1082= dsc; struct Cyc_Set_Set* _temp1088; struct Cyc_List_List*
_temp1090; struct Cyc_Tcpat_Con_s* _temp1092; _LL1084: if((( struct
_tunion_struct*) _temp1082)->tag == Cyc_Tcpat_Neg_tag){ _LL1089: _temp1088=(
struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp1082)->f1; goto
_LL1085;} else{ goto _LL1086;} _LL1086: if((( struct _tunion_struct*) _temp1082)->tag
== Cyc_Tcpat_Pos_tag){ _LL1093: _temp1092=( struct Cyc_Tcpat_Con_s*)(( struct
Cyc_Tcpat_Pos_struct*) _temp1082)->f1; goto _LL1091; _LL1091: _temp1090=( struct
Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*) _temp1082)->f2; goto _LL1087;}
else{ goto _LL1083;} _LL1085: { void* any=( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp1094=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp1094[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp1095; _temp1095.tag= Cyc_Tcpat_Neg_tag;
_temp1095.f1= Cyc_Tcpat_empty_con_set(); _temp1095;}); _temp1094;}); struct Cyc_List_List*
res= 0;{ int i= 0; for( 0; i < pcon->arity; ++ i){ res=({ struct Cyc_List_List*
_temp1096=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1096->hd=( void*) any; _temp1096->tl= res; _temp1096;});}} return res;}
_LL1087: return _temp1090; _LL1083:;} static void* Cyc_Tcpat_getoarg( void* obj,
int i){ return( void*)({ struct Cyc_Tcpat_Sel_struct* _temp1097=( struct Cyc_Tcpat_Sel_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Sel_struct)); _temp1097[ 0]=({ struct Cyc_Tcpat_Sel_struct
_temp1098; _temp1098.tag= Cyc_Tcpat_Sel_tag; _temp1098.f1= i + 1; _temp1098.f2=(
void*) obj; _temp1098;}); _temp1097;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs(
struct Cyc_Tcpat_Con_s* pcon, void* obj){ return(( struct Cyc_List_List*(*)( int
n, void*(* f)( void*, int), void* env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg,
obj);} static void* Cyc_Tcpat_match( void* p, void* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp1099= p; struct Cyc_List_List* _temp1105; struct Cyc_Tcpat_Con_s*
_temp1107; _LL1101: if(( int) _temp1099 == Cyc_Tcpat_Any){ goto _LL1102;} else{
goto _LL1103;} _LL1103: if(( unsigned int) _temp1099 > 1u?(( struct
_tunion_struct*) _temp1099)->tag == Cyc_Tcpat_Con_tag: 0){ _LL1108: _temp1107=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Con_struct*) _temp1099)->f1; goto
_LL1106; _LL1106: _temp1105=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Con_struct*)
_temp1099)->f2; goto _LL1104;} else{ goto _LL1100;} _LL1102: return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc),
work, right_hand_side, rules); _LL1104: { void* _temp1109= Cyc_Tcpat_static_match(
_temp1107, dsc); _LL1111: if(( int) _temp1109 == Cyc_Tcpat_Yes){ goto _LL1112;}
else{ goto _LL1113;} _LL1113: if(( int) _temp1109 == Cyc_Tcpat_No){ goto _LL1114;}
else{ goto _LL1115;} _LL1115: if(( int) _temp1109 == Cyc_Tcpat_Maybe){ goto
_LL1116;} else{ goto _LL1110;} _LL1112: { struct Cyc_List_List* ctx2=({ struct
Cyc_List_List* _temp1117=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1117->hd=( void*)({ struct _tuple9* _temp1118=( struct _tuple9*) GC_malloc(
sizeof( struct _tuple9)); _temp1118->f1= _temp1107; _temp1118->f2= 0; _temp1118;});
_temp1117->tl= ctx; _temp1117;}); struct _tuple10* work_frame=({ struct _tuple10*
_temp1119=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1119->f1=
_temp1105; _temp1119->f2= Cyc_Tcpat_getoargs( _temp1107, obj); _temp1119->f3=
Cyc_Tcpat_getdargs( _temp1107, dsc); _temp1119;}); struct Cyc_List_List* work2=({
struct Cyc_List_List* _temp1120=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1120->hd=( void*) work_frame; _temp1120->tl= work;
_temp1120;}); return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules);} _LL1114: return(( void*(*)( void* dsc,
struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx,
dsc, work), rules); _LL1116: { struct Cyc_List_List* ctx2=({ struct Cyc_List_List*
_temp1121=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1121->hd=( void*)({ struct _tuple9* _temp1122=( struct _tuple9*) GC_malloc(
sizeof( struct _tuple9)); _temp1122->f1= _temp1107; _temp1122->f2= 0; _temp1122;});
_temp1121->tl= ctx; _temp1121;}); struct _tuple10* work_frame=({ struct _tuple10*
_temp1123=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1123->f1=
_temp1105; _temp1123->f2= Cyc_Tcpat_getoargs( _temp1107, obj); _temp1123->f3=
Cyc_Tcpat_getdargs( _temp1107, dsc); _temp1123;}); struct Cyc_List_List* work2=({
struct Cyc_List_List* _temp1124=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1124->hd=( void*) work_frame; _temp1124->tl= work;
_temp1124;}); void* s=(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules); void* f=(( void*(*)( void* dsc, struct Cyc_List_List*
allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg(
dsc, _temp1107), work), rules); return( void*)({ struct Cyc_Tcpat_IfEq_struct*
_temp1125=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp1125[ 0]=({ struct Cyc_Tcpat_IfEq_struct _temp1126; _temp1126.tag= Cyc_Tcpat_IfEq_tag;
_temp1126.f1=( void*) obj; _temp1126.f2= _temp1107; _temp1126.f3=( void*) s;
_temp1126.f4=( void*) f; _temp1126;}); _temp1125;});} _LL1110:;} _LL1100:;}
static void Cyc_Tcpat_check_exhaust_overlap( void* d, void(* not_exhaust)( void*),
void* env1, void(* rhs_appears)( void*, void*), void* env2){ void* _temp1127= d;
void* _temp1135; void* _temp1137; void* _temp1139; struct Cyc_Tcpat_Con_s*
_temp1141; void* _temp1143; _LL1129: if(( int) _temp1127 == Cyc_Tcpat_Failure){
goto _LL1130;} else{ goto _LL1131;} _LL1131: if(( unsigned int) _temp1127 > 1u?((
struct _tunion_struct*) _temp1127)->tag == Cyc_Tcpat_Success_tag: 0){ _LL1136:
_temp1135=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1127)->f1; goto
_LL1132;} else{ goto _LL1133;} _LL1133: if(( unsigned int) _temp1127 > 1u?((
struct _tunion_struct*) _temp1127)->tag == Cyc_Tcpat_IfEq_tag: 0){ _LL1144:
_temp1143=( void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1127)->f1; goto _LL1142;
_LL1142: _temp1141=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1127)->f2; goto _LL1140; _LL1140: _temp1139=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1127)->f3; goto _LL1138; _LL1138: _temp1137=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1127)->f4; goto _LL1134;} else{ goto _LL1128;} _LL1130: not_exhaust( env1);
goto _LL1128; _LL1132: rhs_appears( env2, _temp1135); goto _LL1128; _LL1134:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1139,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1137, not_exhaust, env1, rhs_appears, env2); goto _LL1128; _LL1128:;}
static struct _tuple12* Cyc_Tcpat_get_match( int* r, struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({
struct _tuple11* _temp1145=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11)); _temp1145->f1= 0; _temp1145->f2=( swc->pattern)->loc; _temp1145;});
void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1146=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1146->hd=( void*) sp0; _temp1146->tl=({ struct Cyc_List_List* _temp1147=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1147->hd=(
void*) Cyc_Tcpat_int_pat(* r); _temp1147->tl= 0; _temp1147;}); _temp1146;}));* r=*
r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List* _temp1148=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1148->hd=(
void*) sp0; _temp1148->tl=({ struct Cyc_List_List* _temp1149=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1149->hd=( void*)(( void*) Cyc_Tcpat_Any);
_temp1149->tl= 0; _temp1149;}); _temp1148;}));} return({ struct _tuple12*
_temp1150=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1150->f1=
sp; _temp1150->f2= rhs; _temp1150;});} static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp1151=( char*)"patterns may not be exhaustive"; struct _tagged_string
_temp1152; _temp1152.curr= _temp1151; _temp1152.base= _temp1151; _temp1152.last_plus_one=
_temp1151 + 31; _temp1152;}));} static void Cyc_Tcpat_rule_occurs( int dummy,
struct _tuple11* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( int*, struct Cyc_Absyn_Switch_clause*),
int* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcpat_get_match,({ int*
_temp1153=( int*) GC_malloc_atomic( sizeof( int)); _temp1153[ 0]= 0; _temp1153;}),
swcs); void* dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple11*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=({ struct
Cyc_List_List* _temp1154= match_rules; if( _temp1154 == 0){ _throw(
Null_Exception);} _temp1154->tl;})){ struct _tuple11 _temp1158; struct Cyc_Position_Segment*
_temp1159; int _temp1161; struct _tuple11* _temp1156=(*(( struct _tuple12*)({
struct Cyc_List_List* _temp1155= match_rules; if( _temp1155 == 0){ _throw(
Null_Exception);} _temp1155->hd;}))).f2; _temp1158=* _temp1156; _LL1162:
_temp1161= _temp1158.f1; goto _LL1160; _LL1160: _temp1159= _temp1158.f2; goto
_LL1157; _LL1157: if( ! _temp1161){ Cyc_Tcutil_terr( _temp1159,( struct
_tagged_string)({ char* _temp1163=( char*)"redundant pattern"; struct
_tagged_string _temp1164; _temp1164.curr= _temp1163; _temp1164.base= _temp1163;
_temp1164.last_plus_one= _temp1163 + 18; _temp1164;}));}}} static void Cyc_Tcpat_not_exhaust_warn(
struct _tuple13* pr){(* pr).f2= 0; Cyc_Tcutil_warn((* pr).f1,( struct
_tagged_string)({ char* _temp1165=( char*)"pattern not exhaustive"; struct
_tagged_string _temp1166; _temp1166.curr= _temp1165; _temp1166.base= _temp1165;
_temp1166.last_plus_one= _temp1165 + 23; _temp1166;}));} static void Cyc_Tcpat_dummy_fn(
int i, int j){ return;} int Cyc_Tcpat_check_let_pat_exhaustive( struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List* match_rules=({ struct Cyc_List_List*
_temp1167=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1167->hd=( void*)({ struct _tuple14* _temp1168=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1168->f1= Cyc_Tcpat_compile_pat( p);
_temp1168->f2= 0; _temp1168;}); _temp1167->tl= 0; _temp1167;}); void* dec_tree=((
void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules); struct _tuple13* exhaust_env=({ struct _tuple13* _temp1169=(
struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1169->f1= loc;
_temp1169->f2= 1; _temp1169;});(( void(*)( void* d, void(* not_exhaust)( struct
_tuple13*), struct _tuple13* env1, void(* rhs_appears)( int, int), int env2))
Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_warn,
exhaust_env, Cyc_Tcpat_dummy_fn, 0); return(* exhaust_env).f2;} static struct
_tuple12* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause* swc){ void* sp0=
Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({ struct _tuple11*
_temp1170=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1170->f1=
0; _temp1170->f2=( swc->pattern)->loc; _temp1170;}); return({ struct _tuple12*
_temp1171=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1171->f1=
sp0; _temp1171->f2= rhs; _temp1171;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple11*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=({ struct
Cyc_List_List* _temp1172= match_rules; if( _temp1172 == 0){ _throw(
Null_Exception);} _temp1172->tl;})){ struct _tuple11* pair=(*(( struct _tuple12*)({
struct Cyc_List_List* _temp1173= match_rules; if( _temp1173 == 0){ _throw(
Null_Exception);} _temp1173->hd;}))).f2; if( !(* pair).f1){ Cyc_Tcutil_terr((*
pair).f2,( struct _tagged_string)({ char* _temp1174=( char*)"redundant pattern";
struct _tagged_string _temp1175; _temp1175.curr= _temp1174; _temp1175.base=
_temp1174; _temp1175.last_plus_one= _temp1174 + 18; _temp1175;}));}}}