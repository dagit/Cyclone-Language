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
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify( void*,
void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern struct _tuple4* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*); extern
void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_string err_msg); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd); extern struct _tuple5* Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Pat* p, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); static struct _tuple6* Cyc_Tcpat_null_designator(
struct Cyc_Absyn_Pat* p){ return({ struct _tuple6* _temp0=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp0->f1= 0; _temp0->f2= p; _temp0;});}
static struct _tagged_string* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl* vd){
return(* vd->name).f2;} struct _tuple5* Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** region_opt){ void* t; struct Cyc_List_List*
tv_result= 0; struct Cyc_List_List* v_result= 0;{ void* _temp1=( void*) p->r;
int _temp35; void* _temp37; char _temp39; struct _tagged_string _temp41; struct
Cyc_Absyn_Vardecl* _temp43; struct Cyc_Absyn_Vardecl* _temp45; struct Cyc_Absyn_Pat*
_temp47; struct Cyc_List_List* _temp49; struct Cyc_List_List* _temp51; struct
Cyc_List_List* _temp53; struct Cyc_Core_Opt* _temp55; struct Cyc_Core_Opt**
_temp57; struct Cyc_Absyn_Structdecl* _temp58; struct Cyc_Absyn_Tunionfield*
_temp60; struct Cyc_Absyn_Tuniondecl* _temp62; struct Cyc_List_List* _temp64;
struct Cyc_List_List* _temp66; struct Cyc_Core_Opt* _temp68; struct _tuple1*
_temp70; struct Cyc_Absyn_Tunionfield* _temp72; struct Cyc_Absyn_XTuniondecl*
_temp74; struct Cyc_List_List* _temp76; struct Cyc_List_List* _temp78; struct
_tuple1* _temp80; struct Cyc_Absyn_Enumfield* _temp82; struct Cyc_Absyn_Enumdecl*
_temp84; struct _tuple1* _temp86; struct _tuple1* _temp88; struct Cyc_List_List*
_temp90; struct Cyc_List_List* _temp92; struct _tuple1* _temp94; struct Cyc_List_List*
_temp96; struct Cyc_List_List* _temp98; struct _tuple1* _temp100; _LL3: if(( int)
_temp1 == Cyc_Absyn_Wild_p){ goto _LL4;} else{ goto _LL5;} _LL5: if((
unsigned int) _temp1 > 2u?(( struct _tunion_struct*) _temp1)->tag == Cyc_Absyn_Int_p_tag:
0){ _LL38: _temp37=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1)->f1; goto
_LL36; _LL36: _temp35=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp1)->f2; goto
_LL6;} else{ goto _LL7;} _LL7: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL40: _temp39=( char)((
struct Cyc_Absyn_Char_p_struct*) _temp1)->f1; goto _LL8;} else{ goto _LL9;} _LL9:
if(( unsigned int) _temp1 > 2u?(( struct _tunion_struct*) _temp1)->tag == Cyc_Absyn_Float_p_tag:
0){ _LL42: _temp41=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*)
_temp1)->f1; goto _LL10;} else{ goto _LL11;} _LL11: if(( int) _temp1 == Cyc_Absyn_Null_p){
goto _LL12;} else{ goto _LL13;} _LL13: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL44: _temp43=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*) _temp1)->f1; goto
_LL14;} else{ goto _LL15;} _LL15: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL46: _temp45=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*) _temp1)->f1;
goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL48: _temp47=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*) _temp1)->f1; goto
_LL18;} else{ goto _LL19;} _LL19: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL50: _temp49=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp1)->f1; goto
_LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL59: _temp58=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1)->f1;
goto _LL56; _LL56: _temp55=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f2; _temp57=&(( struct Cyc_Absyn_Struct_p_struct*) _temp1)->f2; goto
_LL54; _LL54: _temp53=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f3; goto _LL52; _LL52: _temp51=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f4; goto _LL22;} else{ goto _LL23;} _LL23: if(( unsigned int) _temp1 >
2u?(( struct _tunion_struct*) _temp1)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL71:
_temp70=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1)->f1;
goto _LL69; _LL69: _temp68=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1)->f2; goto _LL67; _LL67: _temp66=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1)->f3; goto _LL65; _LL65: _temp64=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1)->f4; goto _LL63; _LL63: _temp62=( struct Cyc_Absyn_Tuniondecl*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp1)->f5; goto _LL61; _LL61: _temp60=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1)->f6; goto _LL24;} else{ goto _LL25;}
_LL25: if(( unsigned int) _temp1 > 2u?(( struct _tunion_struct*) _temp1)->tag ==
Cyc_Absyn_XTunion_p_tag: 0){ _LL81: _temp80=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1)->f1; goto _LL79; _LL79: _temp78=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1)->f2; goto _LL77; _LL77: _temp76=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1)->f3; goto _LL75; _LL75: _temp74=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1)->f4; goto _LL73; _LL73: _temp72=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp1)->f5;
goto _LL26;} else{ goto _LL27;} _LL27: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL87: _temp86=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1)->f1; goto _LL85;
_LL85: _temp84=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1)->f2; goto _LL83; _LL83: _temp82=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Absyn_Enum_p_struct*) _temp1)->f3; goto _LL28;} else{ goto _LL29;} _LL29:
if(( unsigned int) _temp1 > 2u?(( struct _tunion_struct*) _temp1)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL89: _temp88=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1)->f1; goto _LL30;} else{ goto _LL31;} _LL31: if(( unsigned int) _temp1 >
2u?(( struct _tunion_struct*) _temp1)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){
_LL95: _temp94=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1)->f1; goto _LL93; _LL93: _temp92=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1)->f2; goto _LL91; _LL91: _temp90=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1)->f3; goto _LL32;} else{ goto _LL33;} _LL33: if(( unsigned int) _temp1 >
2u?(( struct _tunion_struct*) _temp1)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){
_LL101: _temp100=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1)->f1; goto _LL99; _LL99: _temp98=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1)->f2; goto _LL97; _LL97: _temp96=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1)->f3; goto _LL34;} else{ goto _LL2;} _LL4: t= Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_MemKind); goto _LL2; _LL6: t= _temp37 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL2; _LL8: t= Cyc_Absyn_uchar_t; goto
_LL2; _LL10: t= Cyc_Absyn_float_t; goto _LL2; _LL12: t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp102=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp102[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp103; _temp103.tag= Cyc_Absyn_PointerType_tag;
_temp103.f1=({ struct Cyc_Absyn_PtrInfo _temp104; _temp104.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp104.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp104.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp104.tq= Cyc_Absyn_empty_tqual(); _temp104.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp104;}); _temp103;});
_temp102;}); goto _LL2; _LL14: t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);(
void*)( _temp43->type=( void*) t); _temp43->tq= Cyc_Absyn_empty_tqual(); _temp43->shadow=
0; v_result=({ struct Cyc_List_List* _temp105=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp105->hd=( void*) _temp43; _temp105->tl=
v_result; _temp105;}); goto _LL2; _LL16: t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
if( region_opt == 0){ Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char*
_temp106=( char*)"* pattern would point into an unknown region"; struct
_tagged_string _temp107; _temp107.curr= _temp106; _temp107.base= _temp106;
_temp107.last_plus_one= _temp106 + 45; _temp107;})); goto _LL2;}( void*)(
_temp45->type=( void*)(( void*)({ struct Cyc_Absyn_PointerType_struct* _temp108=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp108[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp109; _temp109.tag= Cyc_Absyn_PointerType_tag;
_temp109.f1=({ struct Cyc_Absyn_PtrInfo _temp110; _temp110.elt_typ=( void*) t;
_temp110.rgn_typ=( void*)({ void** _temp111= region_opt; if( _temp111 == 0){
_throw( Null_Exception);}* _temp111;}); _temp110.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp110.tq= Cyc_Absyn_empty_tqual();
_temp110.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp110;}); _temp109;}); _temp108;}))); _temp45->tq= Cyc_Absyn_empty_tqual();
_temp45->shadow= 0; v_result=({ struct Cyc_List_List* _temp112=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp112->hd=( void*) _temp45;
_temp112->tl= v_result; _temp112;}); goto _LL2; _LL18: { void* ptr_rgn= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); struct _tuple5 _temp115; struct Cyc_List_List*
_temp116; struct Cyc_List_List* _temp118; struct _tuple5* _temp113= Cyc_Tcpat_tcPat(
te, _temp47,( void**)& ptr_rgn); _temp115=* _temp113; _LL119: _temp118= _temp115.f1;
goto _LL117; _LL117: _temp116= _temp115.f2; goto _LL114; _LL114: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp118); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp116);{ void*
_temp121= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp120= _temp47->topt;
if( _temp120 == 0){ _throw( Null_Exception);} _temp120->v;})); struct Cyc_Absyn_TunionFieldInfo
_temp129; struct Cyc_Absyn_Tunionfield* _temp131; struct Cyc_Absyn_Tuniondecl*
_temp133; struct _tuple1* _temp135; struct Cyc_List_List* _temp137; struct
_tuple1* _temp139; struct Cyc_Absyn_XTunionFieldInfo _temp141; struct Cyc_Absyn_Tunionfield*
_temp143; struct Cyc_Absyn_XTuniondecl* _temp145; struct _tuple1* _temp147;
struct _tuple1* _temp149; _LL123: if(( unsigned int) _temp121 > 4u?(( struct
_tunion_struct*) _temp121)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL130:
_temp129=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp121)->f1; _LL140: _temp139=( struct _tuple1*) _temp129.name; goto _LL138;
_LL138: _temp137=( struct Cyc_List_List*) _temp129.targs; goto _LL136; _LL136:
_temp135=( struct _tuple1*) _temp129.fname; goto _LL134; _LL134: _temp133=(
struct Cyc_Absyn_Tuniondecl*) _temp129.tud; goto _LL132; _LL132: _temp131=(
struct Cyc_Absyn_Tunionfield*) _temp129.tufd; goto _LL124;} else{ goto _LL125;}
_LL125: if(( unsigned int) _temp121 > 4u?(( struct _tunion_struct*) _temp121)->tag
== Cyc_Absyn_XTunionFieldType_tag: 0){ _LL142: _temp141=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp121)->f1; _LL150: _temp149=(
struct _tuple1*) _temp141.name; goto _LL148; _LL148: _temp147=( struct _tuple1*)
_temp141.fname; goto _LL146; _LL146: _temp145=( struct Cyc_Absyn_XTuniondecl*)
_temp141.xtud; goto _LL144; _LL144: _temp143=( struct Cyc_Absyn_Tunionfield*)
_temp141.xtufd; goto _LL126;} else{ goto _LL127;} _LL127: goto _LL128; _LL124: t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp151=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp151[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp152; _temp152.tag= Cyc_Absyn_TunionType_tag;
_temp152.f1=({ struct Cyc_Absyn_TunionInfo _temp153; _temp153.name= _temp139;
_temp153.targs= _temp137; _temp153.rgn=( void*) ptr_rgn; _temp153.tud= _temp133;
_temp153;}); _temp152;}); _temp151;}); goto _LL122; _LL126: t=( void*)({ struct
Cyc_Absyn_XTunionType_struct* _temp154=( struct Cyc_Absyn_XTunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct)); _temp154[ 0]=({ struct
Cyc_Absyn_XTunionType_struct _temp155; _temp155.tag= Cyc_Absyn_XTunionType_tag;
_temp155.f1=({ struct Cyc_Absyn_XTunionInfo _temp156; _temp156.name= _temp149;
_temp156.rgn=( void*) ptr_rgn; _temp156.xtud= _temp145; _temp156;}); _temp155;});
_temp154;}); goto _LL122; _LL128: t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp157=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp157[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp158; _temp158.tag= Cyc_Absyn_PointerType_tag;
_temp158.f1=({ struct Cyc_Absyn_PtrInfo _temp159; _temp159.elt_typ=( void*)((
void*)({ struct Cyc_Core_Opt* _temp160= _temp47->topt; if( _temp160 == 0){
_throw( Null_Exception);} _temp160->v;})); _temp159.rgn_typ=( void*) ptr_rgn;
_temp159.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp159.tq= Cyc_Absyn_empty_tqual(); _temp159.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp159;}); _temp158;}); _temp157;}); goto _LL122;
_LL122:;} goto _LL2;} _LL20: { struct Cyc_List_List* ts= 0; for( 0; _temp49 != 0;
_temp49=({ struct Cyc_List_List* _temp161= _temp49; if( _temp161 == 0){ _throw(
Null_Exception);} _temp161->tl;})){ struct _tuple5 _temp165; struct Cyc_List_List*
_temp166; struct Cyc_List_List* _temp168; struct _tuple5* _temp163= Cyc_Tcpat_tcPat(
te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp162= _temp49; if(
_temp162 == 0){ _throw( Null_Exception);} _temp162->hd;}), region_opt); _temp165=*
_temp163; _LL169: _temp168= _temp165.f1; goto _LL167; _LL167: _temp166= _temp165.f2;
goto _LL164; _LL164: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp168); v_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp166); ts=({ struct Cyc_List_List* _temp170=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp170->hd=( void*)({ struct
_tuple7* _temp171=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp171->f1= Cyc_Absyn_empty_tqual(); _temp171->f2=( void*)({ struct Cyc_Core_Opt*
_temp173=(( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp172= _temp49;
if( _temp172 == 0){ _throw( Null_Exception);} _temp172->hd;}))->topt; if(
_temp173 == 0){ _throw( Null_Exception);} _temp173->v;}); _temp171;}); _temp170->tl=
ts; _temp170;});} t=( void*)({ struct Cyc_Absyn_TupleType_struct* _temp174=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp174[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp175; _temp175.tag= Cyc_Absyn_TupleType_tag;
_temp175.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ts); _temp175;}); _temp174;}); goto _LL2;} _LL22: if( _temp53 != 0){ return((
struct _tuple5*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp176=( char*)"tcPat: struct<...> not implemented";
struct _tagged_string _temp177; _temp177.curr= _temp176; _temp177.base= _temp176;
_temp177.last_plus_one= _temp176 + 35; _temp177;}));}{ struct Cyc_List_List*
dps0= _temp51; for( 0; dps0 != 0; dps0=({ struct Cyc_List_List* _temp178= dps0;
if( _temp178 == 0){ _throw( Null_Exception);} _temp178->tl;})){ struct _tuple5
_temp182; struct Cyc_List_List* _temp183; struct Cyc_List_List* _temp185; struct
_tuple5* _temp180= Cyc_Tcpat_tcPat( te,(*(( struct _tuple6*)({ struct Cyc_List_List*
_temp179= dps0; if( _temp179 == 0){ _throw( Null_Exception);} _temp179->hd;}))).f2,
region_opt); _temp182=* _temp180; _LL186: _temp185= _temp182.f1; goto _LL184;
_LL184: _temp183= _temp182.f2; goto _LL181; _LL181: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp185); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp183);}}{ struct Cyc_List_List*
all_inst=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp58->tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst);* _temp57=({ struct Cyc_Core_Opt* _temp187=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp187->v=( void*) all_typs;
_temp187;}); t=( void*)({ struct Cyc_Absyn_StructType_struct* _temp188=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp188[ 0]=({ struct Cyc_Absyn_StructType_struct _temp189; _temp189.tag= Cyc_Absyn_StructType_tag;
_temp189.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp190= _temp58->name; if( _temp190 == 0){ _throw( Null_Exception);} _temp190->v;}));
_temp189.f2= all_typs; _temp189.f3=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_Structdecl**)({
struct Cyc_Absyn_Structdecl** _temp191=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*) * 1); _temp191[ 0]= _temp58;
_temp191;})); _temp189;}); _temp188;});{ struct Cyc_List_List* fields=(( struct
Cyc_List_List*(*)( struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)( p->loc,
_temp51, _temp58); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp192=
fields; if( _temp192 == 0){ _throw( Null_Exception);} _temp192->tl;})){ struct
_tuple8 _temp196; struct Cyc_Absyn_Pat* _temp197; struct Cyc_Absyn_Structfield*
_temp199; struct _tuple8* _temp194=( struct _tuple8*)({ struct Cyc_List_List*
_temp193= fields; if( _temp193 == 0){ _throw( Null_Exception);} _temp193->hd;});
_temp196=* _temp194; _LL200: _temp199= _temp196.f1; goto _LL198; _LL198:
_temp197= _temp196.f2; goto _LL195; _LL195: { void* inst_fieldtyp= Cyc_Tcutil_substitute(
all_inst,( void*) _temp199->type); if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt*
_temp201= _temp197->topt; if( _temp201 == 0){ _throw( Null_Exception);} _temp201->v;}),
inst_fieldtyp)){ Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp203=*
_temp199->name; struct _tagged_string _temp204= Cyc_Absynpp_typ2string(
inst_fieldtyp); struct _tagged_string _temp205= Cyc_Absynpp_typ2string(( void*)({
struct Cyc_Core_Opt* _temp202= _temp197->topt; if( _temp202 == 0){ _throw(
Null_Exception);} _temp202->v;})); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp203.last_plus_one - _temp203.curr, _temp203.curr, _temp204.last_plus_one -
_temp204.curr, _temp204.curr, _temp205.last_plus_one - _temp205.curr, _temp205.curr);}));}}}
goto _LL2;}} _LL24:{ struct Cyc_List_List* t= _temp66; for( 0; t != 0; t=({
struct Cyc_List_List* _temp206= t; if( _temp206 == 0){ _throw( Null_Exception);}
_temp206->tl;})){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)({ struct
Cyc_List_List* _temp207= t; if( _temp207 == 0){ _throw( Null_Exception);}
_temp207->hd;});(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind);{ void* _temp208=( void*)( tv->kind)->v; void* _temp218; void*
_temp220; struct Cyc_Absyn_Conref* _temp222; _LL210: if(( unsigned int) _temp208
> 1u?(( struct _tunion_struct*) _temp208)->tag == Cyc_Absyn_Eq_constr_tag: 0){
_LL219: _temp218=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp208)->f1;
if(( int) _temp218 == Cyc_Absyn_BoxKind){ goto _LL211;} else{ goto _LL212;}}
else{ goto _LL212;} _LL212: if(( unsigned int) _temp208 > 1u?(( struct
_tunion_struct*) _temp208)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL221: _temp220=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp208)->f1; goto _LL213;} else{
goto _LL214;} _LL214: if(( int) _temp208 == Cyc_Absyn_No_constr){ goto _LL215;}
else{ goto _LL216;} _LL216: if(( unsigned int) _temp208 > 1u?(( struct
_tunion_struct*) _temp208)->tag == Cyc_Absyn_Forward_constr_tag: 0){ _LL223:
_temp222=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp208)->f1; goto _LL217;} else{ goto _LL209;} _LL211: goto _LL209; _LL213:
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp224= Cyc_Absynpp_qvar2string(
_temp70); struct _tagged_string _temp225=* tv->name; struct _tagged_string
_temp226= Cyc_Absynpp_kind2string( _temp220); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp224.last_plus_one - _temp224.curr, _temp224.curr, _temp225.last_plus_one -
_temp225.curr, _temp225.curr, _temp226.last_plus_one - _temp226.curr, _temp226.curr);}));
goto _LL209; _LL215:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp227=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp227[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp228; _temp228.tag= Cyc_Absyn_Eq_constr_tag;
_temp228.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp228;}); _temp227;})));
goto _LL209; _LL217:(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp229=( char*)"forward after compress in conref";
struct _tagged_string _temp230; _temp230.curr= _temp229; _temp230.base= _temp229;
_temp230.last_plus_one= _temp229 + 33; _temp230;})); goto _LL209; _LL209:;}}}{
struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp66);
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp66);{ struct Cyc_List_List* all_inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp62->tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst); struct Cyc_List_List* tqts= _temp60->typs; if( tqts ==
0){ t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp231=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp231[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp232; _temp232.tag= Cyc_Absyn_TunionType_tag;
_temp232.f1=({ struct Cyc_Absyn_TunionInfo _temp233; _temp233.name=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp234= _temp62->name; if( _temp234 == 0){
_throw( Null_Exception);} _temp234->v;}); _temp233.targs= all_typs; _temp233.rgn=(
void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp233.tud=( struct Cyc_Absyn_Tuniondecl*)
_temp62; _temp233;}); _temp232;}); _temp231;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp235=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp235[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp236; _temp236.tag= Cyc_Absyn_TunionFieldType_tag; _temp236.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp237; _temp237.name=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp238=
_temp62->name; if( _temp238 == 0){ _throw( Null_Exception);} _temp238->v;});
_temp237.targs= all_typs; _temp237.fname= _temp60->name; _temp237.tud=( struct
Cyc_Absyn_Tuniondecl*) _temp62; _temp237.tufd=( struct Cyc_Absyn_Tunionfield*)
_temp60; _temp237;}); _temp236;}); _temp235;});} for( 0; _temp64 != 0? tqts != 0:
0; _temp64=({ struct Cyc_List_List* _temp239= _temp64; if( _temp239 == 0){
_throw( Null_Exception);} _temp239->tl;}), tqts=({ struct Cyc_List_List*
_temp240= tqts; if( _temp240 == 0){ _throw( Null_Exception);} _temp240->tl;})){
struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp241= _temp64; if( _temp241 == 0){ _throw( Null_Exception);} _temp241->hd;});
struct _tuple5 _temp244; struct Cyc_List_List* _temp245; struct Cyc_List_List*
_temp247; struct _tuple5* _temp242= Cyc_Tcpat_tcPat( te2, p2, region_opt);
_temp244=* _temp242; _LL248: _temp247= _temp244.f1; goto _LL246; _LL246:
_temp245= _temp244.f2; goto _LL243; _LL243: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp247); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp245);{ void* field_typ= Cyc_Tcutil_substitute(
all_inst,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp249= tqts; if(
_temp249 == 0){ _throw( Null_Exception);} _temp249->hd;}))).f2); if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp250= p2->topt; if( _temp250 == 0){ _throw(
Null_Exception);} _temp250->v;}), field_typ)){ Cyc_Tcutil_terr( p2->loc,({
struct _tagged_string _temp252= Cyc_Absynpp_qvar2string( _temp70); struct
_tagged_string _temp253= Cyc_Absynpp_typ2string( field_typ); struct
_tagged_string _temp254= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp251= p2->topt; if( _temp251 == 0){ _throw( Null_Exception);} _temp251->v;}));
xprintf("%.*s expects argument type %.*s, not %.*s", _temp252.last_plus_one -
_temp252.curr, _temp252.curr, _temp253.last_plus_one - _temp253.curr, _temp253.curr,
_temp254.last_plus_one - _temp254.curr, _temp254.curr);}));}}} if( _temp64 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp255= Cyc_Absynpp_qvar2string(
_temp70); xprintf("too many arguments for tunion constructor %.*s", _temp255.last_plus_one
- _temp255.curr, _temp255.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp256= Cyc_Absynpp_qvar2string( _temp70); xprintf("too few arguments for tunion constructor %.*s",
_temp256.last_plus_one - _temp256.curr, _temp256.curr);}));} goto _LL2;}} _LL26: {
struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp78);
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp78);{ struct Cyc_List_List* tqts= _temp72->typs;
t=( void*)({ struct Cyc_Absyn_XTunionFieldType_struct* _temp257=( struct Cyc_Absyn_XTunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionFieldType_struct)); _temp257[ 0]=({
struct Cyc_Absyn_XTunionFieldType_struct _temp258; _temp258.tag= Cyc_Absyn_XTunionFieldType_tag;
_temp258.f1=({ struct Cyc_Absyn_XTunionFieldInfo _temp259; _temp259.name=
_temp74->name; _temp259.fname= _temp72->name; _temp259.xtud=( struct Cyc_Absyn_XTuniondecl*)
_temp74; _temp259.xtufd=( struct Cyc_Absyn_Tunionfield*) _temp72; _temp259;});
_temp258;}); _temp257;}); for( 0; _temp76 != 0? tqts != 0: 0; _temp76=({ struct
Cyc_List_List* _temp260= _temp76; if( _temp260 == 0){ _throw( Null_Exception);}
_temp260->tl;}), tqts=({ struct Cyc_List_List* _temp261= tqts; if( _temp261 == 0){
_throw( Null_Exception);} _temp261->tl;})){ struct Cyc_Absyn_Pat* p2=( struct
Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp262= _temp76; if( _temp262 == 0){
_throw( Null_Exception);} _temp262->hd;}); struct _tuple5 _temp265; struct Cyc_List_List*
_temp266; struct Cyc_List_List* _temp268; struct _tuple5* _temp263= Cyc_Tcpat_tcPat(
te2, p2, region_opt); _temp265=* _temp263; _LL269: _temp268= _temp265.f1; goto
_LL267; _LL267: _temp266= _temp265.f2; goto _LL264; _LL264: tv_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp268); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp266);{ void*
field_typ=(*(( struct _tuple7*)({ struct Cyc_List_List* _temp270= tqts; if(
_temp270 == 0){ _throw( Null_Exception);} _temp270->hd;}))).f2; if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp271= p2->topt; if( _temp271 == 0){ _throw(
Null_Exception);} _temp271->v;}), field_typ)){ Cyc_Tcutil_terr( p2->loc,({
struct _tagged_string _temp273= Cyc_Absynpp_qvar2string( _temp80); struct
_tagged_string _temp274= Cyc_Absynpp_typ2string( field_typ); struct
_tagged_string _temp275= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp272= p2->topt; if( _temp272 == 0){ _throw( Null_Exception);} _temp272->v;}));
xprintf("%.*s expects argument type %.*s, not %.*s", _temp273.last_plus_one -
_temp273.curr, _temp273.curr, _temp274.last_plus_one - _temp274.curr, _temp274.curr,
_temp275.last_plus_one - _temp275.curr, _temp275.curr);}));}}} if( _temp76 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp276= Cyc_Absynpp_qvar2string(
_temp80); xprintf("too many arguments for xtunion constructor %.*s", _temp276.last_plus_one
- _temp276.curr, _temp276.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp277= Cyc_Absynpp_qvar2string( _temp80); xprintf("too few arguments for xtunion constructor %.*s",
_temp277.last_plus_one - _temp277.curr, _temp277.curr);}));} goto _LL2;}} _LL28:
t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp278=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp278[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp279; _temp279.tag= Cyc_Absyn_EnumType_tag;
_temp279.f1= _temp84->name; _temp279.f2=( struct Cyc_Absyn_Enumdecl*) _temp84;
_temp279;}); _temp278;}); goto _LL2; _LL30:{ struct _handler_cons _temp280;
_push_handler(& _temp280);{ struct _xtunion_struct* _temp281=( struct
_xtunion_struct*) setjmp( _temp280.handler); if( ! _temp281){{ void* _temp282=
Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp88); struct Cyc_Absyn_Structdecl*
_temp294; struct Cyc_Absyn_Tunionfield* _temp296; struct Cyc_Absyn_Tuniondecl*
_temp298; struct Cyc_Absyn_Tunionfield* _temp300; struct Cyc_Absyn_XTuniondecl*
_temp302; struct Cyc_Absyn_Enumfield* _temp304; struct Cyc_Absyn_Enumdecl*
_temp306; void* _temp308; _LL284: if((( struct _tunion_struct*) _temp282)->tag
== Cyc_Tcenv_StructRes_tag){ _LL295: _temp294=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp282)->f1; goto _LL285;} else{ goto
_LL286;} _LL286: if((( struct _tunion_struct*) _temp282)->tag == Cyc_Tcenv_TunionRes_tag){
_LL299: _temp298=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp282)->f1; goto _LL297; _LL297: _temp296=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp282)->f2; goto _LL287;} else{ goto
_LL288;} _LL288: if((( struct _tunion_struct*) _temp282)->tag == Cyc_Tcenv_XTunionRes_tag){
_LL303: _temp302=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp282)->f1; goto _LL301; _LL301: _temp300=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp282)->f2; goto _LL289;} else{ goto
_LL290;} _LL290: if((( struct _tunion_struct*) _temp282)->tag == Cyc_Tcenv_EnumRes_tag){
_LL307: _temp306=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp282)->f1; goto _LL305; _LL305: _temp304=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp282)->f2; goto _LL291;} else{ goto _LL292;}
_LL292: if((( struct _tunion_struct*) _temp282)->tag == Cyc_Tcenv_VarRes_tag){
_LL309: _temp308=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp282)->f1; goto
_LL293;} else{ goto _LL283;} _LL285: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp310=( char*)"struct tag used without arguments in pattern";
struct _tagged_string _temp311; _temp311.curr= _temp310; _temp311.base= _temp310;
_temp311.last_plus_one= _temp310 + 45; _temp311;})); t= Cyc_Absyn_wildtyp();
goto _LL283; _LL287:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp312=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp312[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp313; _temp313.tag= Cyc_Absyn_Tunion_p_tag;
_temp313.f1= _temp296->name; _temp313.f2= 0; _temp313.f3= 0; _temp313.f4= 0;
_temp313.f5= _temp298; _temp313.f6= _temp296; _temp313;}); _temp312;})));{
struct _tuple5* _temp314= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u);
return _temp314;} _LL289:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_XTunion_p_struct*
_temp315=( struct Cyc_Absyn_XTunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct));
_temp315[ 0]=({ struct Cyc_Absyn_XTunion_p_struct _temp316; _temp316.tag= Cyc_Absyn_XTunion_p_tag;
_temp316.f1= _temp300->name; _temp316.f2= 0; _temp316.f3= 0; _temp316.f4=
_temp302; _temp316.f5= _temp300; _temp316;}); _temp315;})));{ struct _tuple5*
_temp317= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u); return
_temp317;} _LL291:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp318=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp318[ 0]=({ struct Cyc_Absyn_Enum_p_struct _temp319; _temp319.tag= Cyc_Absyn_Enum_p_tag;
_temp319.f1= _temp304->name; _temp319.f2= _temp306; _temp319.f3= _temp304;
_temp319;}); _temp318;})));{ struct _tuple5* _temp320= Cyc_Tcpat_tcPat( te, p,
region_opt); _npop_handler( 0u); return _temp320;} _LL293:{ void* _temp321=(*
_temp88).f1; struct Cyc_List_List* _temp329; _LL323: if(( int) _temp321 == Cyc_Absyn_Loc_n){
goto _LL324;} else{ goto _LL325;} _LL325: if(( unsigned int) _temp321 > 1u?((
struct _tunion_struct*) _temp321)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL330:
_temp329=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp321)->f1;
if( _temp329 == 0){ goto _LL326;} else{ goto _LL327;}} else{ goto _LL327;}
_LL327: goto _LL328; _LL324: goto _LL326; _LL326:(* _temp88).f1=( void*) Cyc_Absyn_Loc_n;(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp331=(
struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp331[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp332; _temp332.tag= Cyc_Absyn_Var_p_tag;
_temp332.f1= Cyc_Absyn_new_vardecl( _temp88,( void*) Cyc_Absyn_VoidType, 0);
_temp332;}); _temp331;})));{ struct _tuple5* _temp333= Cyc_Tcpat_tcPat( te, p,
region_opt); _npop_handler( 0u); return _temp333;} _LL328: Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp334=( char*)"qualified variable in pattern";
struct _tagged_string _temp335; _temp335.curr= _temp334; _temp335.base= _temp334;
_temp335.last_plus_one= _temp334 + 30; _temp335;})); goto _LL322; _LL322:;} Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp336= Cyc_Absynpp_qvar2string( _temp88);
xprintf("unresolved identifier %.*s in pattern", _temp336.last_plus_one -
_temp336.curr, _temp336.curr);})); t= Cyc_Absyn_wildtyp(); goto _LL283; _LL283:;};
_pop_handler();} else{ struct _xtunion_struct* _temp338= _temp281; _LL340: if(
_temp338->tag == Cyc_Dict_Absent_tag){ goto _LL341;} else{ goto _LL342;} _LL342:
goto _LL343; _LL341:{ void* _temp344=(* _temp88).f1; struct Cyc_List_List*
_temp352; _LL346: if(( int) _temp344 == Cyc_Absyn_Loc_n){ goto _LL347;} else{
goto _LL348;} _LL348: if(( unsigned int) _temp344 > 1u?(( struct _tunion_struct*)
_temp344)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL353: _temp352=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp344)->f1; if( _temp352 == 0){ goto _LL349;}
else{ goto _LL350;}} else{ goto _LL350;} _LL350: goto _LL351; _LL347: goto
_LL349; _LL349:(* _temp88).f1=( void*) Cyc_Absyn_Loc_n;( void*)( p->r=( void*)((
void*)({ struct Cyc_Absyn_Var_p_struct* _temp354=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp354[ 0]=({ struct Cyc_Absyn_Var_p_struct
_temp355; _temp355.tag= Cyc_Absyn_Var_p_tag; _temp355.f1= Cyc_Absyn_new_vardecl(
_temp88,( void*) Cyc_Absyn_VoidType, 0); _temp355;}); _temp354;}))); return Cyc_Tcpat_tcPat(
te, p, region_opt); _LL351: Cyc_Tcutil_terr( p->loc,({ struct _tagged_string
_temp356= Cyc_Absynpp_qvar2string( _temp88); xprintf("unbound identifier %.*s in pattern",
_temp356.last_plus_one - _temp356.curr, _temp356.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL345; _LL345:;} goto _LL339; _LL343:( void) _throw( _temp338); _LL339:;}}}
goto _LL2; _LL32:{ struct _handler_cons _temp357; _push_handler(& _temp357);{
struct _xtunion_struct* _temp358=( struct _xtunion_struct*) setjmp( _temp357.handler);
if( ! _temp358){{ void* _temp359= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp94);
struct Cyc_Absyn_Structdecl* _temp371; struct Cyc_Absyn_Tunionfield* _temp373;
struct Cyc_Absyn_Tuniondecl* _temp375; struct Cyc_Absyn_Tunionfield* _temp377;
struct Cyc_Absyn_XTuniondecl* _temp379; struct Cyc_Absyn_Enumfield* _temp381;
struct Cyc_Absyn_Enumdecl* _temp383; void* _temp385; _LL361: if((( struct
_tunion_struct*) _temp359)->tag == Cyc_Tcenv_StructRes_tag){ _LL372: _temp371=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp359)->f1;
goto _LL362;} else{ goto _LL363;} _LL363: if((( struct _tunion_struct*) _temp359)->tag
== Cyc_Tcenv_TunionRes_tag){ _LL376: _temp375=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp359)->f1; goto _LL374; _LL374: _temp373=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp359)->f2;
goto _LL364;} else{ goto _LL365;} _LL365: if((( struct _tunion_struct*) _temp359)->tag
== Cyc_Tcenv_XTunionRes_tag){ _LL380: _temp379=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp359)->f1; goto _LL378; _LL378:
_temp377=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp359)->f2; goto _LL366;} else{ goto _LL367;} _LL367: if((( struct
_tunion_struct*) _temp359)->tag == Cyc_Tcenv_EnumRes_tag){ _LL384: _temp383=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp359)->f1;
goto _LL382; _LL382: _temp381=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp359)->f2; goto _LL368;} else{ goto _LL369;} _LL369: if((( struct
_tunion_struct*) _temp359)->tag == Cyc_Tcenv_VarRes_tag){ _LL386: _temp385=(
void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp359)->f1; goto _LL370;} else{
goto _LL360;} _LL362: { struct Cyc_List_List* x=(( struct Cyc_List_List*(*)(
struct _tuple6*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcpat_null_designator, _temp90);( void*)( p->r=( void*)(( void*)({ struct
Cyc_Absyn_Struct_p_struct* _temp387=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp387[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp388; _temp388.tag= Cyc_Absyn_Struct_p_tag;
_temp388.f1= _temp371; _temp388.f2= 0; _temp388.f3= _temp92; _temp388.f4= x;
_temp388;}); _temp387;})));{ struct _tuple5* _temp389= Cyc_Tcpat_tcPat( te, p,
region_opt); _npop_handler( 0u); return _temp389;}} _LL364:( void*)( p->r=( void*)((
void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp390=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp390[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp391; _temp391.tag= Cyc_Absyn_Tunion_p_tag;
_temp391.f1= _temp373->name; _temp391.f2= 0; _temp391.f3= _temp92; _temp391.f4=
_temp90; _temp391.f5= _temp375; _temp391.f6= _temp373; _temp391;}); _temp390;})));{
struct _tuple5* _temp392= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u);
return _temp392;} _LL366:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_XTunion_p_struct*
_temp393=( struct Cyc_Absyn_XTunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct));
_temp393[ 0]=({ struct Cyc_Absyn_XTunion_p_struct _temp394; _temp394.tag= Cyc_Absyn_XTunion_p_tag;
_temp394.f1= _temp377->name; _temp394.f2= _temp92; _temp394.f3= _temp90;
_temp394.f4= _temp379; _temp394.f5= _temp377; _temp394;}); _temp393;})));{
struct _tuple5* _temp395= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u);
return _temp395;} _LL368: Cyc_Tcutil_terr( p->loc,({ struct _tagged_string
_temp396= Cyc_Absynpp_qvar2string( _temp94); xprintf("bad enum constructor %.*s in pattern",
_temp396.last_plus_one - _temp396.curr, _temp396.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL360; _LL370: goto _LL360; _LL360:;}; _pop_handler();} else{ struct
_xtunion_struct* _temp398= _temp358; _LL400: if( _temp398->tag == Cyc_Dict_Absent_tag){
goto _LL401;} else{ goto _LL402;} _LL402: goto _LL403; _LL401: goto _LL399;
_LL403:( void) _throw( _temp398); _LL399:;}}} Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp404= Cyc_Absynpp_qvar2string( _temp94); xprintf("%.*s is not a constructor in pattern",
_temp404.last_plus_one - _temp404.curr, _temp404.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL2; _LL34:{ struct _handler_cons _temp405; _push_handler(& _temp405);{
struct _xtunion_struct* _temp406=( struct _xtunion_struct*) setjmp( _temp405.handler);
if( ! _temp406){{ void* _temp407= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp100); struct Cyc_Absyn_Structdecl* _temp419; struct Cyc_Absyn_Tunionfield*
_temp421; struct Cyc_Absyn_Tuniondecl* _temp423; struct Cyc_Absyn_Tunionfield*
_temp425; struct Cyc_Absyn_XTuniondecl* _temp427; void* _temp429; struct Cyc_Absyn_Enumfield*
_temp431; struct Cyc_Absyn_Enumdecl* _temp433; _LL409: if((( struct
_tunion_struct*) _temp407)->tag == Cyc_Tcenv_StructRes_tag){ _LL420: _temp419=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp407)->f1;
goto _LL410;} else{ goto _LL411;} _LL411: if((( struct _tunion_struct*) _temp407)->tag
== Cyc_Tcenv_TunionRes_tag){ _LL424: _temp423=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp407)->f1; goto _LL422; _LL422: _temp421=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp407)->f2;
goto _LL412;} else{ goto _LL413;} _LL413: if((( struct _tunion_struct*) _temp407)->tag
== Cyc_Tcenv_XTunionRes_tag){ _LL428: _temp427=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp407)->f1; goto _LL426; _LL426:
_temp425=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp407)->f2; goto _LL414;} else{ goto _LL415;} _LL415: if((( struct
_tunion_struct*) _temp407)->tag == Cyc_Tcenv_VarRes_tag){ _LL430: _temp429=(
void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp407)->f1; goto _LL416;} else{
goto _LL417;} _LL417: if((( struct _tunion_struct*) _temp407)->tag == Cyc_Tcenv_EnumRes_tag){
_LL434: _temp433=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp407)->f1; goto _LL432; _LL432: _temp431=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp407)->f2; goto _LL418;} else{ goto _LL408;}
_LL410:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct*
_temp435=( struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp435[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp436; _temp436.tag= Cyc_Absyn_Struct_p_tag;
_temp436.f1= _temp419; _temp436.f2= 0; _temp436.f3= _temp98; _temp436.f4=
_temp96; _temp436;}); _temp435;})));{ struct _tuple5* _temp437= Cyc_Tcpat_tcPat(
te, p, region_opt); _npop_handler( 0u); return _temp437;} _LL412: Cyc_Tcutil_terr(
p->loc,( struct _tagged_string)({ char* _temp438=( char*)"tunion with designators not implemented";
struct _tagged_string _temp439; _temp439.curr= _temp438; _temp439.base= _temp438;
_temp439.last_plus_one= _temp438 + 40; _temp439;})); t= Cyc_Absyn_wildtyp();
goto _LL408; _LL414: Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char*
_temp440=( char*)"xtunion with designators not implemented"; struct
_tagged_string _temp441; _temp441.curr= _temp440; _temp441.base= _temp440;
_temp441.last_plus_one= _temp440 + 41; _temp441;})); t= Cyc_Absyn_wildtyp();
goto _LL408; _LL416: Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp442=
Cyc_Absynpp_qvar2string( _temp100); xprintf("unbound struct %.*s in pattern",
_temp442.last_plus_one - _temp442.curr, _temp442.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL408; _LL418: Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp443=
Cyc_Absynpp_qvar2string( _temp100); xprintf("enum field %.*s used in struct pattern",
_temp443.last_plus_one - _temp443.curr, _temp443.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL408; _LL408:;}; _pop_handler();} else{ struct _xtunion_struct* _temp445=
_temp406; _LL447: if( _temp445->tag == Cyc_Dict_Absent_tag){ goto _LL448;} else{
goto _LL449;} _LL449: goto _LL450; _LL448: Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp451= Cyc_Absynpp_qvar2string( _temp100); xprintf("unbound struct %.*s in pattern",
_temp451.last_plus_one - _temp451.curr, _temp451.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL446; _LL450:( void) _throw( _temp445); _LL446:;}}} goto _LL2; _LL2:;} p->topt=({
struct Cyc_Core_Opt* _temp452=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp452->v=( void*) t; _temp452;}); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _tagged_string*(* f)( struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_name, v_result), p->loc,(
struct _tagged_string)({ char* _temp453=( char*)"pattern contains a repeated variable";
struct _tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 37; _temp454;})); return({ struct _tuple5*
_temp455=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp455->f1=
tv_result; _temp455->f2= v_result; _temp455;});} void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p){ void* _temp456=( void*) p->r;
struct Cyc_Absyn_Pat* _temp470; struct Cyc_Absyn_Tunionfield* _temp472; struct
Cyc_Absyn_Tuniondecl* _temp474; struct Cyc_List_List* _temp476; struct Cyc_List_List*
_temp478; struct Cyc_Core_Opt* _temp480; struct _tuple1* _temp482; struct Cyc_Absyn_Tunionfield*
_temp484; struct Cyc_Absyn_XTuniondecl* _temp486; struct Cyc_List_List* _temp488;
struct Cyc_List_List* _temp490; struct _tuple1* _temp492; struct Cyc_List_List*
_temp494; struct Cyc_List_List* _temp496; struct Cyc_Core_Opt* _temp498; struct
Cyc_Absyn_Structdecl* _temp500; struct Cyc_List_List* _temp502; _LL458: if((
unsigned int) _temp456 > 2u?(( struct _tunion_struct*) _temp456)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL471: _temp470=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp456)->f1; goto _LL459;} else{ goto _LL460;} _LL460: if(( unsigned int)
_temp456 > 2u?(( struct _tunion_struct*) _temp456)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL483: _temp482=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp456)->f1; goto _LL481; _LL481: _temp480=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp456)->f2; goto _LL479; _LL479: _temp478=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp456)->f3; goto
_LL477; _LL477: _temp476=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp456)->f4; goto _LL475; _LL475: _temp474=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp456)->f5; goto _LL473; _LL473: _temp472=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp456)->f6;
goto _LL461;} else{ goto _LL462;} _LL462: if(( unsigned int) _temp456 > 2u?((
struct _tunion_struct*) _temp456)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL493:
_temp492=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp456)->f1;
goto _LL491; _LL491: _temp490=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp456)->f2; goto _LL489; _LL489: _temp488=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_p_struct*) _temp456)->f3; goto _LL487; _LL487: _temp486=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp456)->f4;
goto _LL485; _LL485: _temp484=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp456)->f5; goto _LL463;} else{ goto _LL464;} _LL464: if(( unsigned int)
_temp456 > 2u?(( struct _tunion_struct*) _temp456)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL501: _temp500=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp456)->f1; goto _LL499; _LL499: _temp498=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp456)->f2; goto _LL497; _LL497: _temp496=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp456)->f3; goto
_LL495; _LL495: _temp494=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp456)->f4; goto _LL465;} else{ goto _LL466;} _LL466: if(( unsigned int)
_temp456 > 2u?(( struct _tunion_struct*) _temp456)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL503: _temp502=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp456)->f1; goto _LL467;} else{ goto _LL468;} _LL468: goto _LL469; _LL459:
Cyc_Tcpat_check_pat_regions( te, _temp470);{ void* _temp505=( void*)({ struct
Cyc_Core_Opt* _temp504= p->topt; if( _temp504 == 0){ _throw( Null_Exception);}
_temp504->v;}); struct Cyc_Absyn_PtrInfo _temp515; struct Cyc_Absyn_Conref*
_temp517; struct Cyc_Absyn_Tqual* _temp519; struct Cyc_Absyn_Conref* _temp521;
void* _temp523; void* _temp525; struct Cyc_Absyn_TunionInfo _temp527; struct Cyc_Absyn_Tuniondecl*
_temp529; void* _temp531; struct Cyc_List_List* _temp533; struct _tuple1*
_temp535; struct Cyc_Absyn_XTunionInfo _temp537; struct Cyc_Absyn_XTuniondecl*
_temp539; void* _temp541; struct _tuple1* _temp543; _LL507: if(( unsigned int)
_temp505 > 4u?(( struct _tunion_struct*) _temp505)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL516: _temp515=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp505)->f1; _LL526: _temp525=( void*) _temp515.elt_typ; goto _LL524; _LL524:
_temp523=( void*) _temp515.rgn_typ; goto _LL522; _LL522: _temp521=( struct Cyc_Absyn_Conref*)
_temp515.nullable; goto _LL520; _LL520: _temp519=( struct Cyc_Absyn_Tqual*)
_temp515.tq; goto _LL518; _LL518: _temp517=( struct Cyc_Absyn_Conref*) _temp515.bounds;
goto _LL508;} else{ goto _LL509;} _LL509: if(( unsigned int) _temp505 > 4u?((
struct _tunion_struct*) _temp505)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL528:
_temp527=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp505)->f1; _LL536: _temp535=( struct _tuple1*) _temp527.name; goto _LL534;
_LL534: _temp533=( struct Cyc_List_List*) _temp527.targs; goto _LL532; _LL532:
_temp531=( void*) _temp527.rgn; goto _LL530; _LL530: _temp529=( struct Cyc_Absyn_Tuniondecl*)
_temp527.tud; goto _LL510;} else{ goto _LL511;} _LL511: if(( unsigned int)
_temp505 > 4u?(( struct _tunion_struct*) _temp505)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL538: _temp537=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp505)->f1; _LL544: _temp543=( struct _tuple1*) _temp537.name; goto _LL542;
_LL542: _temp541=( void*) _temp537.rgn; goto _LL540; _LL540: _temp539=( struct
Cyc_Absyn_XTuniondecl*) _temp537.xtud; goto _LL512;} else{ goto _LL513;} _LL513:
goto _LL514; _LL508: _temp531= _temp523; goto _LL510; _LL510: _temp541= _temp531;
goto _LL512; _LL512: Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp541);
return; _LL514:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp545=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp545[ 0]=({ struct Cyc_Core_Impossible_struct _temp546; _temp546.tag= Cyc_Core_Impossible_tag;
_temp546.f1=( struct _tagged_string)({ char* _temp547=( char*)"check_pat_regions: bad pointer type";
struct _tagged_string _temp548; _temp548.curr= _temp547; _temp548.base= _temp547;
_temp548.last_plus_one= _temp547 + 36; _temp548;}); _temp546;}); _temp545;}));
_LL506:;} _LL461: for( 0; _temp476 != 0; _temp476=({ struct Cyc_List_List*
_temp549= _temp476; if( _temp549 == 0){ _throw( Null_Exception);} _temp549->tl;})){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp550= _temp476; if( _temp550 == 0){ _throw( Null_Exception);} _temp550->hd;}));}{
void* _temp552=( void*)({ struct Cyc_Core_Opt* _temp551= p->topt; if( _temp551
== 0){ _throw( Null_Exception);} _temp551->v;}); struct Cyc_Absyn_TunionInfo
_temp560; struct Cyc_Absyn_Tuniondecl* _temp562; void* _temp564; struct Cyc_List_List*
_temp566; struct _tuple1* _temp568; struct Cyc_Absyn_TunionFieldInfo _temp570;
_LL554: if(( unsigned int) _temp552 > 4u?(( struct _tunion_struct*) _temp552)->tag
== Cyc_Absyn_TunionType_tag: 0){ _LL561: _temp560=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp552)->f1; _LL569: _temp568=( struct
_tuple1*) _temp560.name; goto _LL567; _LL567: _temp566=( struct Cyc_List_List*)
_temp560.targs; goto _LL565; _LL565: _temp564=( void*) _temp560.rgn; goto _LL563;
_LL563: _temp562=( struct Cyc_Absyn_Tuniondecl*) _temp560.tud; goto _LL555;}
else{ goto _LL556;} _LL556: if(( unsigned int) _temp552 > 4u?(( struct
_tunion_struct*) _temp552)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL571:
_temp570=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp552)->f1; goto _LL557;} else{ goto _LL558;} _LL558: goto _LL559; _LL555:
Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp564); return; _LL557: return;
_LL559:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp572=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp572[ 0]=({ struct Cyc_Core_Impossible_struct _temp573; _temp573.tag= Cyc_Core_Impossible_tag;
_temp573.f1=( struct _tagged_string)({ char* _temp574=( char*)"check_pat_regions: bad tunion type";
struct _tagged_string _temp575; _temp575.curr= _temp574; _temp575.base= _temp574;
_temp575.last_plus_one= _temp574 + 35; _temp575;}); _temp573;}); _temp572;}));
_LL553:;} _LL463: for( 0; _temp488 != 0; _temp488=({ struct Cyc_List_List*
_temp576= _temp488; if( _temp576 == 0){ _throw( Null_Exception);} _temp576->tl;})){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp577= _temp488; if( _temp577 == 0){ _throw( Null_Exception);} _temp577->hd;}));}{
void* _temp579=( void*)({ struct Cyc_Core_Opt* _temp578= p->topt; if( _temp578
== 0){ _throw( Null_Exception);} _temp578->v;}); struct Cyc_Absyn_XTunionInfo
_temp587; struct Cyc_Absyn_XTuniondecl* _temp589; void* _temp591; struct _tuple1*
_temp593; struct Cyc_Absyn_XTunionFieldInfo _temp595; _LL581: if(( unsigned int)
_temp579 > 4u?(( struct _tunion_struct*) _temp579)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL588: _temp587=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp579)->f1; _LL594: _temp593=( struct _tuple1*) _temp587.name; goto _LL592;
_LL592: _temp591=( void*) _temp587.rgn; goto _LL590; _LL590: _temp589=( struct
Cyc_Absyn_XTuniondecl*) _temp587.xtud; goto _LL582;} else{ goto _LL583;} _LL583:
if(( unsigned int) _temp579 > 4u?(( struct _tunion_struct*) _temp579)->tag ==
Cyc_Absyn_XTunionFieldType_tag: 0){ _LL596: _temp595=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp579)->f1; goto _LL584;} else{
goto _LL585;} _LL585: goto _LL586; _LL582: Cyc_Tcenv_check_rgn_accessible( te, p->loc,
_temp591); return; _LL584: return; _LL586:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp597=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp597[ 0]=({ struct
Cyc_Core_Impossible_struct _temp598; _temp598.tag= Cyc_Core_Impossible_tag;
_temp598.f1=( struct _tagged_string)({ char* _temp599=( char*)"check_pat_regions: bad xtunion type";
struct _tagged_string _temp600; _temp600.curr= _temp599; _temp600.base= _temp599;
_temp600.last_plus_one= _temp599 + 36; _temp600;}); _temp598;}); _temp597;}));
_LL580:;} _LL465: _temp502=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple6*)) Cyc_Core_snd, _temp494); goto _LL467; _LL467: for( 0; _temp502
!= 0; _temp502=({ struct Cyc_List_List* _temp601= _temp502; if( _temp601 == 0){
_throw( Null_Exception);} _temp601->tl;})){ Cyc_Tcpat_check_pat_regions( te,(
struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp602= _temp502; if( _temp602
== 0){ _throw( Null_Exception);} _temp602->hd;}));} return; _LL469: return;
_LL457:;} typedef void* Cyc_Tcpat_Name_value; static const int Cyc_Tcpat_Name_v_tag=
0; struct Cyc_Tcpat_Name_v_struct{ int tag; struct _tagged_string f1; } ; static
const int Cyc_Tcpat_Int_v_tag= 1; struct Cyc_Tcpat_Int_v_struct{ int tag; int f1;
} ; typedef void* Cyc_Tcpat_name_value_t; struct Cyc_Tcpat_Con_s{ void* name;
int arity; struct Cyc_Core_Opt* span; } ; typedef struct Cyc_Tcpat_Con_s* Cyc_Tcpat_con_t;
typedef void* Cyc_Tcpat_Simple_pat; static const unsigned int Cyc_Tcpat_Any= 0;
static const int Cyc_Tcpat_Con_tag= 0; struct Cyc_Tcpat_Con_struct{ int tag;
struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; typedef void* Cyc_Tcpat_simple_pat_t;
static int Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s*
c2){ void* _temp603=( void*) c1->name; struct _tagged_string _temp609; int
_temp611; _LL605: if((( struct _tunion_struct*) _temp603)->tag == Cyc_Tcpat_Name_v_tag){
_LL610: _temp609=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp603)->f1; goto _LL606;} else{ goto _LL607;} _LL607: if((( struct
_tunion_struct*) _temp603)->tag == Cyc_Tcpat_Int_v_tag){ _LL612: _temp611=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp603)->f1; goto _LL608;} else{ goto _LL604;}
_LL606: { void* _temp613=( void*) c2->name; struct _tagged_string _temp619; int
_temp621; _LL615: if((( struct _tunion_struct*) _temp613)->tag == Cyc_Tcpat_Name_v_tag){
_LL620: _temp619=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp613)->f1; goto _LL616;} else{ goto _LL617;} _LL617: if((( struct
_tunion_struct*) _temp613)->tag == Cyc_Tcpat_Int_v_tag){ _LL622: _temp621=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp613)->f1; goto _LL618;} else{ goto _LL614;}
_LL616: return Cyc_String_zstrcmp( _temp609, _temp619); _LL618: return - 1;
_LL614:;} _LL608: { void* _temp623=( void*) c2->name; struct _tagged_string
_temp629; int _temp631; _LL625: if((( struct _tunion_struct*) _temp623)->tag ==
Cyc_Tcpat_Name_v_tag){ _LL630: _temp629=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp623)->f1; goto _LL626;} else{ goto _LL627;} _LL627: if((( struct
_tunion_struct*) _temp623)->tag == Cyc_Tcpat_Int_v_tag){ _LL632: _temp631=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp623)->f1; goto _LL628;} else{ goto _LL624;}
_LL626: return 1; _LL628: return _temp611 - _temp631; _LL624:;} _LL604:;} static
struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct Cyc_Set_Set*(*)(
int(* comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*))) Cyc_Set_empty)(
Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt=( struct
Cyc_Core_Opt){.v=( void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt=(
struct Cyc_Core_Opt){.v=( void*) 2}; static struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static char _temp636[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={ 0u,( struct
_tagged_string){ _temp636, _temp636, _temp636 + 5u}}; static char _temp640[ 4u]="ptr";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={ 0u,( struct
_tagged_string){ _temp640, _temp640, _temp640 + 4u}}; static char _temp644[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,( struct
_tagged_string){ _temp644, _temp644, _temp644 + 6u}}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_null_name_value),.arity=
0,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt}; static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=&
Cyc_Tcpat_null_ptr_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp645=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp645->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp646=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp646[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp647; _temp647.tag= Cyc_Tcpat_Int_v_tag;
_temp647.f1= i; _temp647;}); _temp646;})); _temp645->arity= 0; _temp645->span= 0;
_temp645;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_char_con( char c){ return({
struct Cyc_Tcpat_Con_s* _temp648=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp648->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct*
_temp650=( struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp650[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp651; _temp651.tag= Cyc_Tcpat_Int_v_tag;
_temp651.f1=( int) c; _temp651;}); _temp650;})); _temp648->arity= 0; _temp648->span=({
struct Cyc_Core_Opt* _temp649=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp649->v=( void*) 256; _temp649;}); _temp648;});}
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct _tagged_string f){
return({ struct Cyc_Tcpat_Con_s* _temp652=( struct Cyc_Tcpat_Con_s*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_s)); _temp652->name=( void*)(( void*)({ struct Cyc_Tcpat_Name_v_struct*
_temp653=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct));
_temp653[ 0]=({ struct Cyc_Tcpat_Name_v_struct _temp654; _temp654.tag= Cyc_Tcpat_Name_v_tag;
_temp654.f1= f; _temp654;}); _temp653;})); _temp652->arity= 0; _temp652->span= 0;
_temp652;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_double_con( struct
_tagged_string d){ return({ struct Cyc_Tcpat_Con_s* _temp655=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp655->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp656=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp656[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp657; _temp657.tag= Cyc_Tcpat_Name_v_tag; _temp657.f1= d; _temp657;});
_temp656;})); _temp655->arity= 0; _temp655->span= 0; _temp655;});} static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp658=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp658->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp658->arity=
i; _temp658->span= Cyc_Tcpat_one_opt_ptr; _temp658;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp659=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp659[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp660; _temp660.tag= Cyc_Tcpat_Con_tag; _temp660.f1= Cyc_Tcpat_null_con;
_temp660.f2= 0; _temp660;}); _temp659;});} static void* Cyc_Tcpat_int_pat( int i){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp661=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp661[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp662; _temp662.tag= Cyc_Tcpat_Con_tag; _temp662.f1= Cyc_Tcpat_int_con( i);
_temp662.f2= 0; _temp662;}); _temp661;});} static void* Cyc_Tcpat_char_pat( char
c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp663=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp663[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp664; _temp664.tag= Cyc_Tcpat_Con_tag; _temp664.f1= Cyc_Tcpat_char_con( c);
_temp664.f2= 0; _temp664;}); _temp663;});} static void* Cyc_Tcpat_float_pat(
struct _tagged_string f){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp665=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp665[ 0]=({ struct Cyc_Tcpat_Con_struct _temp666; _temp666.tag= Cyc_Tcpat_Con_tag;
_temp666.f1= Cyc_Tcpat_float_con( f); _temp666.f2= 0; _temp666;}); _temp665;});}
static void* Cyc_Tcpat_double_pat( struct _tagged_string d){ return( void*)({
struct Cyc_Tcpat_Con_struct* _temp667=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp667[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp668; _temp668.tag= Cyc_Tcpat_Con_tag; _temp668.f1= Cyc_Tcpat_double_con( d);
_temp668.f2= 0; _temp668;}); _temp667;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp669=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp669[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp670; _temp670.tag= Cyc_Tcpat_Con_tag; _temp670.f1= Cyc_Tcpat_null_ptr_con;
_temp670.f2=({ struct Cyc_List_List* _temp671=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp671->hd=( void*) p; _temp671->tl= 0;
_temp671;}); _temp670;}); _temp669;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp672=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp672[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp673; _temp673.tag= Cyc_Tcpat_Con_tag; _temp673.f1= Cyc_Tcpat_ptr_con;
_temp673.f2=({ struct Cyc_List_List* _temp674=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp674->hd=( void*) p; _temp674->tl= 0;
_temp674;}); _temp673;}); _temp672;});} static void* Cyc_Tcpat_tuple_pat( struct
Cyc_List_List* ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp675=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp675[ 0]=({ struct Cyc_Tcpat_Con_struct _temp676; _temp676.tag= Cyc_Tcpat_Con_tag;
_temp676.f1= Cyc_Tcpat_tuple_con((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ss)); _temp676.f2= ss; _temp676;}); _temp675;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_string con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List*
ps){ struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp677=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp677->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp678=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp678[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp679; _temp679.tag= Cyc_Tcpat_Name_v_tag; _temp679.f1= con_name; _temp679;});
_temp678;})); _temp677->arity=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ps); _temp677->span= span; _temp677;}); return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp680=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp680[ 0]=({ struct Cyc_Tcpat_Con_struct _temp681; _temp681.tag= Cyc_Tcpat_Con_tag;
_temp681.f1= c; _temp681.f2= ps; _temp681;}); _temp680;});} static void* Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat* p){ void* s;{ void* _temp682=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp716; int _temp718; void* _temp720; char _temp722; struct _tagged_string
_temp724; struct Cyc_List_List* _temp726; struct Cyc_Absyn_Vardecl* _temp728;
struct Cyc_Absyn_Pat* _temp730; struct Cyc_List_List* _temp732; struct Cyc_List_List*
_temp734; struct Cyc_Core_Opt* _temp736; struct Cyc_Absyn_Structdecl* _temp738;
struct Cyc_Absyn_Tunionfield* _temp740; struct Cyc_Absyn_Tuniondecl* _temp742;
struct Cyc_List_List* _temp744; struct Cyc_List_List* _temp746; struct Cyc_Core_Opt*
_temp748; struct _tuple1* _temp750; struct Cyc_Absyn_Tunionfield* _temp752;
struct Cyc_Absyn_XTuniondecl* _temp754; struct Cyc_List_List* _temp756; struct
Cyc_List_List* _temp758; struct _tuple1* _temp760; struct Cyc_Absyn_Enumfield*
_temp762; struct Cyc_Absyn_Enumdecl* _temp764; struct _tuple1* _temp766; struct
_tuple1* _temp768; struct Cyc_List_List* _temp770; struct Cyc_List_List*
_temp772; struct _tuple1* _temp774; struct Cyc_List_List* _temp776; struct Cyc_List_List*
_temp778; struct _tuple1* _temp780; _LL684: if(( int) _temp682 == Cyc_Absyn_Wild_p){
goto _LL685;} else{ goto _LL686;} _LL686: if(( unsigned int) _temp682 > 2u?((
struct _tunion_struct*) _temp682)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL717:
_temp716=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*) _temp682)->f1;
goto _LL687;} else{ goto _LL688;} _LL688: if(( int) _temp682 == Cyc_Absyn_Null_p){
goto _LL689;} else{ goto _LL690;} _LL690: if(( unsigned int) _temp682 > 2u?((
struct _tunion_struct*) _temp682)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL721:
_temp720=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp682)->f1; goto _LL719;
_LL719: _temp718=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp682)->f2; goto
_LL691;} else{ goto _LL692;} _LL692: if(( unsigned int) _temp682 > 2u?(( struct
_tunion_struct*) _temp682)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL723: _temp722=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp682)->f1; goto _LL693;} else{ goto
_LL694;} _LL694: if(( unsigned int) _temp682 > 2u?(( struct _tunion_struct*)
_temp682)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL725: _temp724=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp682)->f1; goto _LL695;}
else{ goto _LL696;} _LL696: if(( unsigned int) _temp682 > 2u?(( struct
_tunion_struct*) _temp682)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL727: _temp726=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp682)->f1; goto
_LL697;} else{ goto _LL698;} _LL698: if(( unsigned int) _temp682 > 2u?(( struct
_tunion_struct*) _temp682)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL729:
_temp728=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp682)->f1; goto _LL699;} else{ goto _LL700;} _LL700: if(( unsigned int)
_temp682 > 2u?(( struct _tunion_struct*) _temp682)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL731: _temp730=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp682)->f1; goto _LL701;} else{ goto _LL702;} _LL702: if(( unsigned int)
_temp682 > 2u?(( struct _tunion_struct*) _temp682)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL739: _temp738=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp682)->f1; goto _LL737; _LL737: _temp736=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp682)->f2; goto _LL735; _LL735: _temp734=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp682)->f3; goto
_LL733; _LL733: _temp732=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp682)->f4; goto _LL703;} else{ goto _LL704;} _LL704: if(( unsigned int)
_temp682 > 2u?(( struct _tunion_struct*) _temp682)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL751: _temp750=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp682)->f1; goto _LL749; _LL749: _temp748=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp682)->f2; goto _LL747; _LL747: _temp746=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp682)->f3; goto
_LL745; _LL745: _temp744=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp682)->f4; goto _LL743; _LL743: _temp742=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp682)->f5; goto _LL741; _LL741: _temp740=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp682)->f6;
goto _LL705;} else{ goto _LL706;} _LL706: if(( unsigned int) _temp682 > 2u?((
struct _tunion_struct*) _temp682)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL761:
_temp760=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp682)->f1;
goto _LL759; _LL759: _temp758=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp682)->f2; goto _LL757; _LL757: _temp756=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_p_struct*) _temp682)->f3; goto _LL755; _LL755: _temp754=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp682)->f4;
goto _LL753; _LL753: _temp752=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp682)->f5; goto _LL707;} else{ goto _LL708;} _LL708: if(( unsigned int)
_temp682 > 2u?(( struct _tunion_struct*) _temp682)->tag == Cyc_Absyn_Enum_p_tag:
0){ _LL767: _temp766=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp682)->f1; goto _LL765; _LL765: _temp764=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp682)->f2; goto _LL763; _LL763: _temp762=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*) _temp682)->f3;
goto _LL709;} else{ goto _LL710;} _LL710: if(( unsigned int) _temp682 > 2u?((
struct _tunion_struct*) _temp682)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL769:
_temp768=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp682)->f1;
goto _LL711;} else{ goto _LL712;} _LL712: if(( unsigned int) _temp682 > 2u?((
struct _tunion_struct*) _temp682)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){
_LL775: _temp774=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp682)->f1; goto _LL773; _LL773: _temp772=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp682)->f2; goto _LL771; _LL771: _temp770=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp682)->f3;
goto _LL713;} else{ goto _LL714;} _LL714: if(( unsigned int) _temp682 > 2u?((
struct _tunion_struct*) _temp682)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){
_LL781: _temp780=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp682)->f1; goto _LL779; _LL779: _temp778=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp682)->f2; goto _LL777; _LL777: _temp776=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp682)->f3;
goto _LL715;} else{ goto _LL683;} _LL685: s=( void*) Cyc_Tcpat_Any; goto _LL683;
_LL687: s=( void*) Cyc_Tcpat_Any; goto _LL683; _LL689: s= Cyc_Tcpat_null_pat();
goto _LL683; _LL691: s= Cyc_Tcpat_int_pat( _temp718); goto _LL683; _LL693: s=
Cyc_Tcpat_char_pat( _temp722); goto _LL683; _LL695: s= Cyc_Tcpat_float_pat(
_temp724); goto _LL683; _LL697: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp726)); goto _LL683; _LL699: s=( void*) Cyc_Tcpat_Any; goto _LL683; _LL701: {
void* ss= Cyc_Tcpat_compile_pat( _temp730);{ void* _temp783= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp782= p->topt; if( _temp782 == 0){ _throw(
Null_Exception);} _temp782->v;})); struct Cyc_Absyn_PtrInfo _temp793; struct Cyc_Absyn_Conref*
_temp795; struct Cyc_Absyn_Tqual* _temp797; struct Cyc_Absyn_Conref* _temp799;
void* _temp801; void* _temp803; struct Cyc_Absyn_TunionInfo _temp805; struct Cyc_Absyn_XTunionInfo
_temp807; _LL785: if(( unsigned int) _temp783 > 4u?(( struct _tunion_struct*)
_temp783)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL794: _temp793=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp783)->f1; _LL804: _temp803=( void*)
_temp793.elt_typ; goto _LL802; _LL802: _temp801=( void*) _temp793.rgn_typ; goto
_LL800; _LL800: _temp799=( struct Cyc_Absyn_Conref*) _temp793.nullable; goto
_LL798; _LL798: _temp797=( struct Cyc_Absyn_Tqual*) _temp793.tq; goto _LL796;
_LL796: _temp795=( struct Cyc_Absyn_Conref*) _temp793.bounds; goto _LL786;}
else{ goto _LL787;} _LL787: if(( unsigned int) _temp783 > 4u?(( struct
_tunion_struct*) _temp783)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL806:
_temp805=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp783)->f1; goto _LL788;} else{ goto _LL789;} _LL789: if(( unsigned int)
_temp783 > 4u?(( struct _tunion_struct*) _temp783)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL808: _temp807=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp783)->f1; goto _LL790;} else{ goto _LL791;} _LL791: goto _LL792; _LL786: {
int is_nullable= 0; int still_working= 1; while( still_working) { void* _temp809=(
void*) _temp799->v; struct Cyc_Absyn_Conref* _temp817; int _temp819; _LL811: if((
unsigned int) _temp809 > 1u?(( struct _tunion_struct*) _temp809)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL818: _temp817=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp809)->f1; goto _LL812;} else{ goto _LL813;} _LL813: if(( int) _temp809 ==
Cyc_Absyn_No_constr){ goto _LL814;} else{ goto _LL815;} _LL815: if((
unsigned int) _temp809 > 1u?(( struct _tunion_struct*) _temp809)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL820: _temp819=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp809)->f1;
goto _LL816;} else{ goto _LL810;} _LL812:( void*)( _temp799->v=( void*)(( void*)
_temp817->v)); continue; _LL814:( void*)( _temp799->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp821=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp821[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp822; _temp822.tag= Cyc_Absyn_Eq_constr_tag;
_temp822.f1=( void*) 0; _temp822;}); _temp821;}))); is_nullable= 0;
still_working= 0; goto _LL810; _LL816: is_nullable=( int) _temp819;
still_working= 0; goto _LL810; _LL810:;} if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL784;} _LL788: s= ss; goto _LL784;
_LL790: s= ss; goto _LL784; _LL792: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp823=( char*)"pointer pattern does not have pointer type!";
struct _tagged_string _temp824; _temp824.curr= _temp823; _temp824.base= _temp823;
_temp824.last_plus_one= _temp823 + 44; _temp824;})); goto _LL784; _LL784:;} goto
_LL683;} _LL703: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp825= _temp738->fields; if(
_temp825 == 0){ _throw( Null_Exception);} _temp825->v;}); for( 0; fields != 0;
fields=({ struct Cyc_List_List* _temp826= fields; if( _temp826 == 0){ _throw(
Null_Exception);} _temp826->tl;})){ int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp827= fields; if( _temp827 == 0){ _throw(
Null_Exception);} _temp827->hd;}))->name,( struct _tagged_string)({ char*
_temp828=( char*)""; struct _tagged_string _temp829; _temp829.curr= _temp828;
_temp829.base= _temp828; _temp829.last_plus_one= _temp828 + 1; _temp829;})) == 0;{
struct Cyc_List_List* dlps0= _temp732; for( 0; ! found? dlps0 != 0: 0; dlps0=({
struct Cyc_List_List* _temp830= dlps0; if( _temp830 == 0){ _throw(
Null_Exception);} _temp830->tl;})){ struct _tuple6 _temp834; struct Cyc_Absyn_Pat*
_temp835; struct Cyc_List_List* _temp837; struct _tuple6* _temp832=( struct
_tuple6*)({ struct Cyc_List_List* _temp831= dlps0; if( _temp831 == 0){ _throw(
Null_Exception);} _temp831->hd;}); _temp834=* _temp832; _LL838: _temp837=
_temp834.f1; goto _LL836; _LL836: _temp835= _temp834.f2; goto _LL833; _LL833: {
struct Cyc_List_List* _temp839= _temp837; struct Cyc_List_List _temp845; struct
Cyc_List_List* _temp846; void* _temp848; struct _tagged_string* _temp850; _LL841:
if( _temp839 == 0){ goto _LL843;} else{ _temp845=* _temp839; _LL849: _temp848=(
void*) _temp845.hd; if((( struct _tunion_struct*) _temp848)->tag == Cyc_Absyn_FieldName_tag){
_LL851: _temp850=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp848)->f1; goto _LL847;} else{ goto _LL843;} _LL847: _temp846=( struct Cyc_List_List*)
_temp845.tl; if( _temp846 == 0){ goto _LL842;} else{ goto _LL843;}} _LL843: goto
_LL844; _LL842: if( Cyc_String_zstrptrcmp( _temp850,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp852= fields; if( _temp852 == 0){ _throw(
Null_Exception);} _temp852->hd;}))->name) == 0){ ps=({ struct Cyc_List_List*
_temp853=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp853->hd=( void*) Cyc_Tcpat_compile_pat( _temp835); _temp853->tl= ps;
_temp853;}); found= 1;} goto _LL840; _LL844:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp854=( char*)"bad designator(s)";
struct _tagged_string _temp855; _temp855.curr= _temp854; _temp855.base= _temp854;
_temp855.last_plus_one= _temp854 + 18; _temp855;})); goto _LL840; _LL840:;}}}
if( ! found){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp856=( char*)"bad designator"; struct _tagged_string
_temp857; _temp857.curr= _temp856; _temp857.base= _temp856; _temp857.last_plus_one=
_temp856 + 15; _temp857;}));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL683;}
_LL705: { int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp858= _temp742->fields; if(
_temp858 == 0){ _throw( Null_Exception);} _temp858->v;})); s= Cyc_Tcpat_con_pat(*(*
_temp740->name).f2,({ struct Cyc_Core_Opt* _temp859=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp859->v=( void*) span;
_temp859;}),(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp744)); goto
_LL683;} _LL707: s= Cyc_Tcpat_con_pat(*(* _temp752->name).f2, 0,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp756)); goto _LL683; _LL709: { int span=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp764->fields); s= Cyc_Tcpat_con_pat(*(* _temp762->name).f2,({
struct Cyc_Core_Opt* _temp860=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp860->v=( void*) span; _temp860;}), 0); goto _LL683;}
_LL711: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp861=( char*)"compile_pat: unknownid"; struct
_tagged_string _temp862; _temp862.curr= _temp861; _temp862.base= _temp861;
_temp862.last_plus_one= _temp861 + 23; _temp862;})); goto _LL683; _LL713: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp863=( char*)"compile_pat: unknowncall"; struct _tagged_string
_temp864; _temp864.curr= _temp863; _temp864.base= _temp863; _temp864.last_plus_one=
_temp863 + 25; _temp864;})); goto _LL683; _LL715: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp865=(
char*)"compile_pat: unknownfields"; struct _tagged_string _temp866; _temp866.curr=
_temp865; _temp866.base= _temp865; _temp866.last_plus_one= _temp865 + 27;
_temp866;})); goto _LL683; _LL683:;} return s;} typedef void* Cyc_Tcpat_Term_desc;
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
Cyc_Tcpat_add_neg( void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp867= td;
struct Cyc_Set_Set* _temp873; struct Cyc_List_List* _temp875; struct Cyc_Tcpat_Con_s*
_temp877; _LL869: if((( struct _tunion_struct*) _temp867)->tag == Cyc_Tcpat_Neg_tag){
_LL874: _temp873=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp867)->f1;
goto _LL870;} else{ goto _LL871;} _LL871: if((( struct _tunion_struct*) _temp867)->tag
== Cyc_Tcpat_Pos_tag){ _LL878: _temp877=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*)
_temp867)->f1; goto _LL876; _LL876: _temp875=( struct Cyc_List_List*)(( struct
Cyc_Tcpat_Pos_struct*) _temp867)->f2; goto _LL872;} else{ goto _LL868;} _LL870:
if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp873, c)){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp879=( char*)"add_neg called when constructor already in set";
struct _tagged_string _temp880; _temp880.curr= _temp879; _temp880.base= _temp879;
_temp880.last_plus_one= _temp879 + 47; _temp880;}));} if( c->span != 0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp873) + 1 >=( int)({ struct
Cyc_Core_Opt* _temp881= c->span; if( _temp881 == 0){ _throw( Null_Exception);}
_temp881->v;}): 0){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp882=( char*)"add_neg called when |cs U {c}| >= span(c)";
struct _tagged_string _temp883; _temp883.curr= _temp882; _temp883.base= _temp882;
_temp883.last_plus_one= _temp882 + 42; _temp883;}));} return( void*)({ struct
Cyc_Tcpat_Neg_struct* _temp884=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp884[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp885; _temp885.tag= Cyc_Tcpat_Neg_tag; _temp885.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp873,
c); _temp885;}); _temp884;}); _LL872: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp886=( char*)"add_neg called when td is Positive";
struct _tagged_string _temp887; _temp887.curr= _temp886; _temp887.base= _temp886;
_temp887.last_plus_one= _temp886 + 35; _temp887;})); _LL868:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp888= td; struct Cyc_List_List*
_temp894; struct Cyc_Tcpat_Con_s* _temp896; struct Cyc_Set_Set* _temp898; _LL890:
if((( struct _tunion_struct*) _temp888)->tag == Cyc_Tcpat_Pos_tag){ _LL897:
_temp896=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp888)->f1;
goto _LL895; _LL895: _temp894=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp888)->f2; goto _LL891;} else{ goto _LL892;} _LL892: if((( struct
_tunion_struct*) _temp888)->tag == Cyc_Tcpat_Neg_tag){ _LL899: _temp898=( struct
Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp888)->f1; goto _LL893;} else{
goto _LL889;} _LL891: if( Cyc_Tcpat_compare_con( c, _temp896) == 0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL893: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp898,
c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)({ struct Cyc_Core_Opt*
_temp900= c->span; if( _temp900 == 0){ _throw( Null_Exception);} _temp900->v;})
==(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp898) + 1: 0){
return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}} _LL889:;}
static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt, void*
dsc){ struct Cyc_List_List* _temp901= ctxt; struct Cyc_List_List _temp907;
struct Cyc_List_List* _temp908; struct _tuple9* _temp910; struct _tuple9
_temp912; struct Cyc_List_List* _temp913; struct Cyc_Tcpat_Con_s* _temp915;
_LL903: if( _temp901 == 0){ goto _LL904;} else{ goto _LL905;} _LL905: if(
_temp901 == 0){ goto _LL902;} else{ _temp907=* _temp901; _LL911: _temp910=(
struct _tuple9*) _temp907.hd; _temp912=* _temp910; _LL916: _temp915= _temp912.f1;
goto _LL914; _LL914: _temp913= _temp912.f2; goto _LL909; _LL909: _temp908=(
struct Cyc_List_List*) _temp907.tl; goto _LL906;} _LL904: return 0; _LL906:
return({ struct Cyc_List_List* _temp917=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp917->hd=( void*)({ struct _tuple9* _temp918=(
struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp918->f1= _temp915;
_temp918->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp919=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp919->hd=( void*)
dsc; _temp919->tl= _temp913; _temp919;}); _temp918;}); _temp917->tl= _temp908;
_temp917;}); _LL902:;} static struct Cyc_List_List* Cyc_Tcpat_norm_context(
struct Cyc_List_List* ctxt){ struct Cyc_List_List* _temp920= ctxt; struct Cyc_List_List
_temp926; struct Cyc_List_List* _temp927; struct _tuple9* _temp929; struct
_tuple9 _temp931; struct Cyc_List_List* _temp932; struct Cyc_Tcpat_Con_s*
_temp934; _LL922: if( _temp920 == 0){ goto _LL923;} else{ goto _LL924;} _LL924:
if( _temp920 == 0){ goto _LL921;} else{ _temp926=* _temp920; _LL930: _temp929=(
struct _tuple9*) _temp926.hd; _temp931=* _temp929; _LL935: _temp934= _temp931.f1;
goto _LL933; _LL933: _temp932= _temp931.f2; goto _LL928; _LL928: _temp927=(
struct Cyc_List_List*) _temp926.tl; goto _LL925;} _LL923: return(( struct Cyc_List_List*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp936=( char*)"norm_context: empty context"; struct _tagged_string _temp937;
_temp937.curr= _temp936; _temp937.base= _temp936; _temp937.last_plus_one=
_temp936 + 28; _temp937;})); _LL925: return Cyc_Tcpat_augment( _temp927,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp938=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp938[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp939; _temp939.tag= Cyc_Tcpat_Pos_tag; _temp939.f1= _temp934; _temp939.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp932);
_temp939;}); _temp938;})); _LL921:;} static void* Cyc_Tcpat_build_desc( struct
Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple5
_temp941=({ struct _tuple5 _temp940; _temp940.f1= ctxt; _temp940.f2= work;
_temp940;}); struct Cyc_List_List* _temp951; struct Cyc_List_List* _temp953;
struct Cyc_List_List* _temp955; struct Cyc_List_List* _temp957; struct Cyc_List_List*
_temp959; struct Cyc_List_List* _temp961; struct Cyc_List_List* _temp963; struct
Cyc_List_List _temp965; struct Cyc_List_List* _temp966; struct _tuple10*
_temp968; struct _tuple10 _temp970; struct Cyc_List_List* _temp971; struct Cyc_List_List*
_temp973; struct Cyc_List_List* _temp975; struct Cyc_List_List* _temp977; struct
Cyc_List_List _temp979; struct Cyc_List_List* _temp980; struct _tuple9* _temp982;
struct _tuple9 _temp984; struct Cyc_List_List* _temp985; struct Cyc_Tcpat_Con_s*
_temp987; _LL943: _LL954: _temp953= _temp941.f1; if( _temp953 == 0){ goto _LL952;}
else{ goto _LL945;} _LL952: _temp951= _temp941.f2; if( _temp951 == 0){ goto
_LL944;} else{ goto _LL945;} _LL945: _LL958: _temp957= _temp941.f1; if( _temp957
== 0){ goto _LL956;} else{ goto _LL947;} _LL956: _temp955= _temp941.f2; goto
_LL946; _LL947: _LL962: _temp961= _temp941.f1; goto _LL960; _LL960: _temp959=
_temp941.f2; if( _temp959 == 0){ goto _LL948;} else{ goto _LL949;} _LL949:
_LL978: _temp977= _temp941.f1; if( _temp977 == 0){ goto _LL942;} else{ _temp979=*
_temp977; _LL983: _temp982=( struct _tuple9*) _temp979.hd; _temp984=* _temp982;
_LL988: _temp987= _temp984.f1; goto _LL986; _LL986: _temp985= _temp984.f2; goto
_LL981; _LL981: _temp980=( struct Cyc_List_List*) _temp979.tl; goto _LL964;}
_LL964: _temp963= _temp941.f2; if( _temp963 == 0){ goto _LL942;} else{ _temp965=*
_temp963; _LL969: _temp968=( struct _tuple10*) _temp965.hd; _temp970=* _temp968;
_LL976: _temp975= _temp970.f1; goto _LL974; _LL974: _temp973= _temp970.f2; goto
_LL972; _LL972: _temp971= _temp970.f3; goto _LL967; _LL967: _temp966=( struct
Cyc_List_List*) _temp965.tl; goto _LL950;} _LL944: return dsc; _LL946: return((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp989=( char*)"build_desc: ctxt and work don't match"; struct
_tagged_string _temp990; _temp990.curr= _temp989; _temp990.base= _temp989;
_temp990.last_plus_one= _temp989 + 38; _temp990;})); _LL948: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp991=( char*)"build_desc: ctxt and work don't match"; struct _tagged_string
_temp992; _temp992.curr= _temp991; _temp992.base= _temp991; _temp992.last_plus_one=
_temp991 + 38; _temp992;})); _LL950: { struct Cyc_Tcpat_Pos_struct* td=({ struct
Cyc_Tcpat_Pos_struct* _temp993=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp993[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp994; _temp994.tag= Cyc_Tcpat_Pos_tag; _temp994.f1= _temp987; _temp994.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp985),({
struct Cyc_List_List* _temp995=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp995->hd=( void*) dsc; _temp995->tl= _temp971;
_temp995;})); _temp994;}); _temp993;}); return Cyc_Tcpat_build_desc( _temp980,(
void*) td, _temp966);} _LL942:;} static void* Cyc_Tcpat_match( void* p, void*
obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp996=
allmrules; struct Cyc_List_List _temp1002; struct Cyc_List_List* _temp1003;
struct _tuple0* _temp1005; struct _tuple0 _temp1007; void* _temp1008; void*
_temp1010; _LL998: if( _temp996 == 0){ goto _LL999;} else{ goto _LL1000;}
_LL1000: if( _temp996 == 0){ goto _LL997;} else{ _temp1002=* _temp996; _LL1006:
_temp1005=( struct _tuple0*) _temp1002.hd; _temp1007=* _temp1005; _LL1011:
_temp1010= _temp1007.f1; goto _LL1009; _LL1009: _temp1008= _temp1007.f2; goto
_LL1004; _LL1004: _temp1003=( struct Cyc_List_List*) _temp1002.tl; goto _LL1001;}
_LL999: return( void*) Cyc_Tcpat_Failure; _LL1001: return(( void*(*)( void* p,
void* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work,
void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp1010,(
void*) Cyc_Tcpat_Obj, dsc, 0, 0, _temp1008, _temp1003); _LL997:;} static void*
Cyc_Tcpat_match_compile( struct Cyc_List_List* allmrules){ return(( void*(*)(
void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)(( void*)({
struct Cyc_Tcpat_Neg_struct* _temp1012=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp1012[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp1013; _temp1013.tag= Cyc_Tcpat_Neg_tag; _temp1013.f1= Cyc_Tcpat_empty_con_set();
_temp1013;}); _temp1012;}), allmrules);} static void* Cyc_Tcpat_and_match(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules){ struct Cyc_List_List* _temp1014= work; struct Cyc_List_List
_temp1022; struct Cyc_List_List* _temp1023; struct _tuple10* _temp1025; struct
_tuple10 _temp1027; struct Cyc_List_List* _temp1028; struct Cyc_List_List*
_temp1030; struct Cyc_List_List* _temp1032; struct Cyc_List_List _temp1034;
struct Cyc_List_List* _temp1035; struct _tuple10* _temp1037; struct _tuple10
_temp1039; struct Cyc_List_List* _temp1040; struct Cyc_List_List* _temp1042;
struct Cyc_List_List* _temp1044; _LL1016: if( _temp1014 == 0){ goto _LL1017;}
else{ goto _LL1018;} _LL1018: if( _temp1014 == 0){ goto _LL1020;} else{
_temp1022=* _temp1014; _LL1026: _temp1025=( struct _tuple10*) _temp1022.hd;
_temp1027=* _temp1025; _LL1033: _temp1032= _temp1027.f1; if( _temp1032 == 0){
goto _LL1031;} else{ goto _LL1020;} _LL1031: _temp1030= _temp1027.f2; if(
_temp1030 == 0){ goto _LL1029;} else{ goto _LL1020;} _LL1029: _temp1028=
_temp1027.f3; if( _temp1028 == 0){ goto _LL1024;} else{ goto _LL1020;} _LL1024:
_temp1023=( struct Cyc_List_List*) _temp1022.tl; goto _LL1019;} _LL1020: if(
_temp1014 == 0){ goto _LL1015;} else{ _temp1034=* _temp1014; _LL1038: _temp1037=(
struct _tuple10*) _temp1034.hd; _temp1039=* _temp1037; _LL1045: _temp1044=
_temp1039.f1; goto _LL1043; _LL1043: _temp1042= _temp1039.f2; goto _LL1041;
_LL1041: _temp1040= _temp1039.f3; goto _LL1036; _LL1036: _temp1035=( struct Cyc_List_List*)
_temp1034.tl; goto _LL1021;} _LL1017: return( void*)({ struct Cyc_Tcpat_Success_struct*
_temp1046=( struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp1046[ 0]=({ struct Cyc_Tcpat_Success_struct _temp1047; _temp1047.tag= Cyc_Tcpat_Success_tag;
_temp1047.f1=( void*) right_hand_side; _temp1047;}); _temp1046;}); _LL1019:
return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context(
ctx), _temp1023, right_hand_side, rules); _LL1021: if(( _temp1044 == 0? 1:
_temp1042 == 0)? 1: _temp1040 == 0){ return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1048=( char*)"tcpat:and_match: malformed work frame";
struct _tagged_string _temp1049; _temp1049.curr= _temp1048; _temp1049.base=
_temp1048; _temp1049.last_plus_one= _temp1048 + 38; _temp1049;}));}{ struct Cyc_List_List
_temp1053; struct Cyc_List_List* _temp1054; void* _temp1056; struct Cyc_List_List*
_temp1051=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1050= _temp1044;
if( _temp1050 == 0){ _throw( Null_Exception);} _temp1050;}); _temp1053=*
_temp1051; _LL1057: _temp1056=( void*) _temp1053.hd; goto _LL1055; _LL1055:
_temp1054=( struct Cyc_List_List*) _temp1053.tl; goto _LL1052; _LL1052: { struct
Cyc_List_List _temp1061; struct Cyc_List_List* _temp1062; void* _temp1064;
struct Cyc_List_List* _temp1059=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1058= _temp1042; if( _temp1058 == 0){ _throw( Null_Exception);} _temp1058;});
_temp1061=* _temp1059; _LL1065: _temp1064=( void*) _temp1061.hd; goto _LL1063;
_LL1063: _temp1062=( struct Cyc_List_List*) _temp1061.tl; goto _LL1060; _LL1060: {
struct Cyc_List_List _temp1069; struct Cyc_List_List* _temp1070; void* _temp1072;
struct Cyc_List_List* _temp1067=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1066= _temp1040; if( _temp1066 == 0){ _throw( Null_Exception);} _temp1066;});
_temp1069=* _temp1067; _LL1073: _temp1072=( void*) _temp1069.hd; goto _LL1071;
_LL1071: _temp1070=( struct Cyc_List_List*) _temp1069.tl; goto _LL1068; _LL1068: {
struct _tuple10* wf=({ struct _tuple10* _temp1074=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp1074->f1= _temp1054; _temp1074->f2= _temp1062;
_temp1074->f3= _temp1070; _temp1074;}); return(( void*(*)( void* p, void* obj,
void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp1056,
_temp1064, _temp1072, ctx,({ struct Cyc_List_List* _temp1075=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1075->hd=( void*) wf; _temp1075->tl=
_temp1035; _temp1075;}), right_hand_side, rules);}}}} _LL1015:;} static struct
Cyc_List_List* Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon, void* dsc){
void* _temp1076= dsc; struct Cyc_Set_Set* _temp1082; struct Cyc_List_List*
_temp1084; struct Cyc_Tcpat_Con_s* _temp1086; _LL1078: if((( struct
_tunion_struct*) _temp1076)->tag == Cyc_Tcpat_Neg_tag){ _LL1083: _temp1082=(
struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp1076)->f1; goto
_LL1079;} else{ goto _LL1080;} _LL1080: if((( struct _tunion_struct*) _temp1076)->tag
== Cyc_Tcpat_Pos_tag){ _LL1087: _temp1086=( struct Cyc_Tcpat_Con_s*)(( struct
Cyc_Tcpat_Pos_struct*) _temp1076)->f1; goto _LL1085; _LL1085: _temp1084=( struct
Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*) _temp1076)->f2; goto _LL1081;}
else{ goto _LL1077;} _LL1079: { void* any=( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp1088=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp1088[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp1089; _temp1089.tag= Cyc_Tcpat_Neg_tag;
_temp1089.f1= Cyc_Tcpat_empty_con_set(); _temp1089;}); _temp1088;}); struct Cyc_List_List*
res= 0;{ int i= 0; for( 0; i < pcon->arity; ++ i){ res=({ struct Cyc_List_List*
_temp1090=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1090->hd=( void*) any; _temp1090->tl= res; _temp1090;});}} return res;}
_LL1081: return _temp1084; _LL1077:;} static void* Cyc_Tcpat_getoarg( void* obj,
int i){ return( void*)({ struct Cyc_Tcpat_Sel_struct* _temp1091=( struct Cyc_Tcpat_Sel_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Sel_struct)); _temp1091[ 0]=({ struct Cyc_Tcpat_Sel_struct
_temp1092; _temp1092.tag= Cyc_Tcpat_Sel_tag; _temp1092.f1= i + 1; _temp1092.f2=(
void*) obj; _temp1092;}); _temp1091;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs(
struct Cyc_Tcpat_Con_s* pcon, void* obj){ return(( struct Cyc_List_List*(*)( int
n, void*(* f)( void*, int), void* env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg,
obj);} static void* Cyc_Tcpat_match( void* p, void* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp1093= p; struct Cyc_List_List* _temp1099; struct Cyc_Tcpat_Con_s*
_temp1101; _LL1095: if(( int) _temp1093 == Cyc_Tcpat_Any){ goto _LL1096;} else{
goto _LL1097;} _LL1097: if(( unsigned int) _temp1093 > 1u?(( struct
_tunion_struct*) _temp1093)->tag == Cyc_Tcpat_Con_tag: 0){ _LL1102: _temp1101=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Con_struct*) _temp1093)->f1; goto
_LL1100; _LL1100: _temp1099=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Con_struct*)
_temp1093)->f2; goto _LL1098;} else{ goto _LL1094;} _LL1096: return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc),
work, right_hand_side, rules); _LL1098: { void* _temp1103= Cyc_Tcpat_static_match(
_temp1101, dsc); _LL1105: if(( int) _temp1103 == Cyc_Tcpat_Yes){ goto _LL1106;}
else{ goto _LL1107;} _LL1107: if(( int) _temp1103 == Cyc_Tcpat_No){ goto _LL1108;}
else{ goto _LL1109;} _LL1109: if(( int) _temp1103 == Cyc_Tcpat_Maybe){ goto
_LL1110;} else{ goto _LL1104;} _LL1106: { struct Cyc_List_List* ctx2=({ struct
Cyc_List_List* _temp1111=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1111->hd=( void*)({ struct _tuple9* _temp1112=( struct _tuple9*) GC_malloc(
sizeof( struct _tuple9)); _temp1112->f1= _temp1101; _temp1112->f2= 0; _temp1112;});
_temp1111->tl= ctx; _temp1111;}); struct _tuple10* work_frame=({ struct _tuple10*
_temp1113=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1113->f1=
_temp1099; _temp1113->f2= Cyc_Tcpat_getoargs( _temp1101, obj); _temp1113->f3=
Cyc_Tcpat_getdargs( _temp1101, dsc); _temp1113;}); struct Cyc_List_List* work2=({
struct Cyc_List_List* _temp1114=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1114->hd=( void*) work_frame; _temp1114->tl= work;
_temp1114;}); return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules);} _LL1108: return(( void*(*)( void* dsc,
struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx,
dsc, work), rules); _LL1110: { struct Cyc_List_List* ctx2=({ struct Cyc_List_List*
_temp1115=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1115->hd=( void*)({ struct _tuple9* _temp1116=( struct _tuple9*) GC_malloc(
sizeof( struct _tuple9)); _temp1116->f1= _temp1101; _temp1116->f2= 0; _temp1116;});
_temp1115->tl= ctx; _temp1115;}); struct _tuple10* work_frame=({ struct _tuple10*
_temp1117=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1117->f1=
_temp1099; _temp1117->f2= Cyc_Tcpat_getoargs( _temp1101, obj); _temp1117->f3=
Cyc_Tcpat_getdargs( _temp1101, dsc); _temp1117;}); struct Cyc_List_List* work2=({
struct Cyc_List_List* _temp1118=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1118->hd=( void*) work_frame; _temp1118->tl= work;
_temp1118;}); void* s=(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules); void* f=(( void*(*)( void* dsc, struct Cyc_List_List*
allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg(
dsc, _temp1101), work), rules); return( void*)({ struct Cyc_Tcpat_IfEq_struct*
_temp1119=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp1119[ 0]=({ struct Cyc_Tcpat_IfEq_struct _temp1120; _temp1120.tag= Cyc_Tcpat_IfEq_tag;
_temp1120.f1=( void*) obj; _temp1120.f2= _temp1101; _temp1120.f3=( void*) s;
_temp1120.f4=( void*) f; _temp1120;}); _temp1119;});} _LL1104:;} _LL1094:;}
static void Cyc_Tcpat_check_exhaust_overlap( void* d, void(* not_exhaust)( void*),
void* env1, void(* rhs_appears)( void*, void*), void* env2){ void* _temp1121= d;
void* _temp1129; void* _temp1131; void* _temp1133; struct Cyc_Tcpat_Con_s*
_temp1135; void* _temp1137; _LL1123: if(( int) _temp1121 == Cyc_Tcpat_Failure){
goto _LL1124;} else{ goto _LL1125;} _LL1125: if(( unsigned int) _temp1121 > 1u?((
struct _tunion_struct*) _temp1121)->tag == Cyc_Tcpat_Success_tag: 0){ _LL1130:
_temp1129=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1121)->f1; goto
_LL1126;} else{ goto _LL1127;} _LL1127: if(( unsigned int) _temp1121 > 1u?((
struct _tunion_struct*) _temp1121)->tag == Cyc_Tcpat_IfEq_tag: 0){ _LL1138:
_temp1137=( void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1121)->f1; goto _LL1136;
_LL1136: _temp1135=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1121)->f2; goto _LL1134; _LL1134: _temp1133=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1121)->f3; goto _LL1132; _LL1132: _temp1131=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1121)->f4; goto _LL1128;} else{ goto _LL1122;} _LL1124: not_exhaust( env1);
goto _LL1122; _LL1126: rhs_appears( env2, _temp1129); goto _LL1122; _LL1128:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1133,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1131, not_exhaust, env1, rhs_appears, env2); goto _LL1122; _LL1122:;}
static struct _tuple12* Cyc_Tcpat_get_match( int* r, struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({
struct _tuple11* _temp1139=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11)); _temp1139->f1= 0; _temp1139->f2=( swc->pattern)->loc; _temp1139;});
void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1140=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1140->hd=( void*) sp0; _temp1140->tl=({ struct Cyc_List_List* _temp1141=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1141->hd=(
void*) Cyc_Tcpat_int_pat(* r); _temp1141->tl= 0; _temp1141;}); _temp1140;}));* r=*
r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List* _temp1142=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1142->hd=(
void*) sp0; _temp1142->tl=({ struct Cyc_List_List* _temp1143=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1143->hd=( void*)(( void*) Cyc_Tcpat_Any);
_temp1143->tl= 0; _temp1143;}); _temp1142;}));} return({ struct _tuple12*
_temp1144=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1144->f1=
sp; _temp1144->f2= rhs; _temp1144;});} static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp1145=( char*)"patterns may not be exhaustive"; struct _tagged_string
_temp1146; _temp1146.curr= _temp1145; _temp1146.base= _temp1145; _temp1146.last_plus_one=
_temp1145 + 31; _temp1146;}));} static void Cyc_Tcpat_rule_occurs( int dummy,
struct _tuple11* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( int*, struct Cyc_Absyn_Switch_clause*),
int* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcpat_get_match,({ int*
_temp1147=( int*) GC_malloc_atomic( sizeof( int)); _temp1147[ 0]= 0; _temp1147;}),
swcs); void* dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple11*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=({ struct
Cyc_List_List* _temp1148= match_rules; if( _temp1148 == 0){ _throw(
Null_Exception);} _temp1148->tl;})){ struct _tuple11 _temp1152; struct Cyc_Position_Segment*
_temp1153; int _temp1155; struct _tuple11* _temp1150=(*(( struct _tuple12*)({
struct Cyc_List_List* _temp1149= match_rules; if( _temp1149 == 0){ _throw(
Null_Exception);} _temp1149->hd;}))).f2; _temp1152=* _temp1150; _LL1156:
_temp1155= _temp1152.f1; goto _LL1154; _LL1154: _temp1153= _temp1152.f2; goto
_LL1151; _LL1151: if( ! _temp1155){ Cyc_Tcutil_terr( _temp1153,( struct
_tagged_string)({ char* _temp1157=( char*)"redundant pattern"; struct
_tagged_string _temp1158; _temp1158.curr= _temp1157; _temp1158.base= _temp1157;
_temp1158.last_plus_one= _temp1157 + 18; _temp1158;}));}}} static void Cyc_Tcpat_not_exhaust_warn(
struct _tuple13* pr){(* pr).f2= 0; Cyc_Tcutil_warn((* pr).f1,( struct
_tagged_string)({ char* _temp1159=( char*)"pattern not exhaustive"; struct
_tagged_string _temp1160; _temp1160.curr= _temp1159; _temp1160.base= _temp1159;
_temp1160.last_plus_one= _temp1159 + 23; _temp1160;}));} static void Cyc_Tcpat_dummy_fn(
int i, int j){ return;} int Cyc_Tcpat_check_let_pat_exhaustive( struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List* match_rules=({ struct Cyc_List_List*
_temp1161=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1161->hd=( void*)({ struct _tuple14* _temp1162=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1162->f1= Cyc_Tcpat_compile_pat( p);
_temp1162->f2= 0; _temp1162;}); _temp1161->tl= 0; _temp1161;}); void* dec_tree=((
void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules); struct _tuple13* exhaust_env=({ struct _tuple13* _temp1163=(
struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1163->f1= loc;
_temp1163->f2= 1; _temp1163;});(( void(*)( void* d, void(* not_exhaust)( struct
_tuple13*), struct _tuple13* env1, void(* rhs_appears)( int, int), int env2))
Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_warn,
exhaust_env, Cyc_Tcpat_dummy_fn, 0); return(* exhaust_env).f2;} static struct
_tuple12* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause* swc){ void* sp0=
Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({ struct _tuple11*
_temp1164=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1164->f1=
0; _temp1164->f2=( swc->pattern)->loc; _temp1164;}); return({ struct _tuple12*
_temp1165=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1165->f1=
sp0; _temp1165->f2= rhs; _temp1165;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple11*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=({ struct
Cyc_List_List* _temp1166= match_rules; if( _temp1166 == 0){ _throw(
Null_Exception);} _temp1166->tl;})){ struct _tuple11* pair=(*(( struct _tuple12*)({
struct Cyc_List_List* _temp1167= match_rules; if( _temp1167 == 0){ _throw(
Null_Exception);} _temp1167->hd;}))).f2; if( !(* pair).f1){ Cyc_Tcutil_terr((*
pair).f2,( struct _tagged_string)({ char* _temp1168=( char*)"redundant pattern";
struct _tagged_string _temp1169; _temp1169.curr= _temp1168; _temp1169.base=
_temp1168; _temp1169.last_plus_one= _temp1168 + 18; _temp1169;}));}}}