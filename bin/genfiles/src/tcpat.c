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
_temp110.rgn_typ=( void*)* region_opt; _temp110.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp110.tq= Cyc_Absyn_empty_tqual();
_temp110.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp110;}); _temp109;}); _temp108;}))); _temp45->tq= Cyc_Absyn_empty_tqual();
_temp45->shadow= 0; v_result=({ struct Cyc_List_List* _temp111=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp111->hd=( void*) _temp45;
_temp111->tl= v_result; _temp111;}); goto _LL2; _LL18: { void* ptr_rgn= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); struct _tuple5 _temp114; struct Cyc_List_List*
_temp115; struct Cyc_List_List* _temp117; struct _tuple5* _temp112= Cyc_Tcpat_tcPat(
te, _temp47,( void**)& ptr_rgn); _temp114=* _temp112; _LL118: _temp117= _temp114.f1;
goto _LL116; _LL116: _temp115= _temp114.f2; goto _LL113; _LL113: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp117); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp115);{ void*
_temp119= Cyc_Tcutil_compress(( void*)( _temp47->topt)->v); struct Cyc_Absyn_TunionFieldInfo
_temp127; struct Cyc_Absyn_Tunionfield* _temp129; struct Cyc_Absyn_Tuniondecl*
_temp131; struct _tuple1* _temp133; struct Cyc_List_List* _temp135; struct
_tuple1* _temp137; struct Cyc_Absyn_XTunionFieldInfo _temp139; struct Cyc_Absyn_Tunionfield*
_temp141; struct Cyc_Absyn_XTuniondecl* _temp143; struct _tuple1* _temp145;
struct _tuple1* _temp147; _LL121: if(( unsigned int) _temp119 > 4u?(( struct
_tunion_struct*) _temp119)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL128:
_temp127=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp119)->f1; _LL138: _temp137=( struct _tuple1*) _temp127.name; goto _LL136;
_LL136: _temp135=( struct Cyc_List_List*) _temp127.targs; goto _LL134; _LL134:
_temp133=( struct _tuple1*) _temp127.fname; goto _LL132; _LL132: _temp131=(
struct Cyc_Absyn_Tuniondecl*) _temp127.tud; goto _LL130; _LL130: _temp129=(
struct Cyc_Absyn_Tunionfield*) _temp127.tufd; goto _LL122;} else{ goto _LL123;}
_LL123: if(( unsigned int) _temp119 > 4u?(( struct _tunion_struct*) _temp119)->tag
== Cyc_Absyn_XTunionFieldType_tag: 0){ _LL140: _temp139=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp119)->f1; _LL148: _temp147=(
struct _tuple1*) _temp139.name; goto _LL146; _LL146: _temp145=( struct _tuple1*)
_temp139.fname; goto _LL144; _LL144: _temp143=( struct Cyc_Absyn_XTuniondecl*)
_temp139.xtud; goto _LL142; _LL142: _temp141=( struct Cyc_Absyn_Tunionfield*)
_temp139.xtufd; goto _LL124;} else{ goto _LL125;} _LL125: goto _LL126; _LL122: t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp149=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp149[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp150; _temp150.tag= Cyc_Absyn_TunionType_tag;
_temp150.f1=({ struct Cyc_Absyn_TunionInfo _temp151; _temp151.name= _temp137;
_temp151.targs= _temp135; _temp151.rgn=( void*) ptr_rgn; _temp151.tud= _temp131;
_temp151;}); _temp150;}); _temp149;}); goto _LL120; _LL124: t=( void*)({ struct
Cyc_Absyn_XTunionType_struct* _temp152=( struct Cyc_Absyn_XTunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct)); _temp152[ 0]=({ struct
Cyc_Absyn_XTunionType_struct _temp153; _temp153.tag= Cyc_Absyn_XTunionType_tag;
_temp153.f1=({ struct Cyc_Absyn_XTunionInfo _temp154; _temp154.name= _temp147;
_temp154.rgn=( void*) ptr_rgn; _temp154.xtud= _temp143; _temp154;}); _temp153;});
_temp152;}); goto _LL120; _LL126: t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp155=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp155[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp156; _temp156.tag= Cyc_Absyn_PointerType_tag;
_temp156.f1=({ struct Cyc_Absyn_PtrInfo _temp157; _temp157.elt_typ=( void*)((
void*)( _temp47->topt)->v); _temp157.rgn_typ=( void*) ptr_rgn; _temp157.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp157.tq= Cyc_Absyn_empty_tqual();
_temp157.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp157;}); _temp156;}); _temp155;}); goto _LL120; _LL120:;} goto _LL2;} _LL20: {
struct Cyc_List_List* ts= 0; for( 0; _temp49 != 0; _temp49= _temp49->tl){ struct
_tuple5 _temp160; struct Cyc_List_List* _temp161; struct Cyc_List_List* _temp163;
struct _tuple5* _temp158= Cyc_Tcpat_tcPat( te,( struct Cyc_Absyn_Pat*) _temp49->hd,
region_opt); _temp160=* _temp158; _LL164: _temp163= _temp160.f1; goto _LL162;
_LL162: _temp161= _temp160.f2; goto _LL159; _LL159: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp163); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp161); ts=({ struct Cyc_List_List*
_temp165=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp165->hd=( void*)({ struct _tuple7* _temp166=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp166->f1= Cyc_Absyn_empty_tqual(); _temp166->f2=(
void*)((( struct Cyc_Absyn_Pat*) _temp49->hd)->topt)->v; _temp166;}); _temp165->tl=
ts; _temp165;});} t=( void*)({ struct Cyc_Absyn_TupleType_struct* _temp167=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp167[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp168; _temp168.tag= Cyc_Absyn_TupleType_tag;
_temp168.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ts); _temp168;}); _temp167;}); goto _LL2;} _LL22: if( _temp53 != 0){ return((
struct _tuple5*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp169=( char*)"tcPat: struct<...> not implemented";
struct _tagged_string _temp170; _temp170.curr= _temp169; _temp170.base= _temp169;
_temp170.last_plus_one= _temp169 + 35; _temp170;}));}{ struct Cyc_List_List*
dps0= _temp51; for( 0; dps0 != 0; dps0= dps0->tl){ struct _tuple5 _temp173;
struct Cyc_List_List* _temp174; struct Cyc_List_List* _temp176; struct _tuple5*
_temp171= Cyc_Tcpat_tcPat( te,(*(( struct _tuple6*) dps0->hd)).f2, region_opt);
_temp173=* _temp171; _LL177: _temp176= _temp173.f1; goto _LL175; _LL175:
_temp174= _temp173.f2; goto _LL172; _LL172: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp176); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp174);}}{ struct Cyc_List_List*
all_inst=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp58->tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst);* _temp57=({ struct Cyc_Core_Opt* _temp178=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp178->v=( void*) all_typs;
_temp178;}); t=( void*)({ struct Cyc_Absyn_StructType_struct* _temp179=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp179[ 0]=({ struct Cyc_Absyn_StructType_struct _temp180; _temp180.tag= Cyc_Absyn_StructType_tag;
_temp180.f1=( struct _tuple1*)(( struct _tuple1*)( _temp58->name)->v); _temp180.f2=
all_typs; _temp180.f3=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_Structdecl**)({
struct Cyc_Absyn_Structdecl** _temp181=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*) * 1); _temp181[ 0]= _temp58;
_temp181;})); _temp180;}); _temp179;});{ struct Cyc_List_List* fields=(( struct
Cyc_List_List*(*)( struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)( p->loc,
_temp51, _temp58); for( 0; fields != 0; fields= fields->tl){ struct _tuple8
_temp184; struct Cyc_Absyn_Pat* _temp185; struct Cyc_Absyn_Structfield* _temp187;
struct _tuple8* _temp182=( struct _tuple8*) fields->hd; _temp184=* _temp182;
_LL188: _temp187= _temp184.f1; goto _LL186; _LL186: _temp185= _temp184.f2; goto
_LL183; _LL183: { void* inst_fieldtyp= Cyc_Tcutil_substitute( all_inst,( void*)
_temp187->type); if( ! Cyc_Tcutil_unify(( void*)( _temp185->topt)->v,
inst_fieldtyp)){ Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp189=*
_temp187->name; struct _tagged_string _temp190= Cyc_Absynpp_typ2string(
inst_fieldtyp); struct _tagged_string _temp191= Cyc_Absynpp_typ2string(( void*)(
_temp185->topt)->v); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp189.last_plus_one - _temp189.curr, _temp189.curr, _temp190.last_plus_one -
_temp190.curr, _temp190.curr, _temp191.last_plus_one - _temp191.curr, _temp191.curr);}));}}}
goto _LL2;}} _LL24:{ struct Cyc_List_List* t= _temp66; for( 0; t != 0; t= t->tl){
struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*) t->hd;(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( tv->kind);{ void*
_temp192=( void*)( tv->kind)->v; void* _temp202; void* _temp204; struct Cyc_Absyn_Conref*
_temp206; _LL194: if(( unsigned int) _temp192 > 1u?(( struct _tunion_struct*)
_temp192)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL203: _temp202=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp192)->f1; if(( int) _temp202 == Cyc_Absyn_BoxKind){
goto _LL195;} else{ goto _LL196;}} else{ goto _LL196;} _LL196: if(( unsigned int)
_temp192 > 1u?(( struct _tunion_struct*) _temp192)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL205: _temp204=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp192)->f1;
goto _LL197;} else{ goto _LL198;} _LL198: if(( int) _temp192 == Cyc_Absyn_No_constr){
goto _LL199;} else{ goto _LL200;} _LL200: if(( unsigned int) _temp192 > 1u?((
struct _tunion_struct*) _temp192)->tag == Cyc_Absyn_Forward_constr_tag: 0){
_LL207: _temp206=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp192)->f1; goto _LL201;} else{ goto _LL193;} _LL195: goto _LL193; _LL197:
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp208= Cyc_Absynpp_qvar2string(
_temp70); struct _tagged_string _temp209=* tv->name; struct _tagged_string
_temp210= Cyc_Absynpp_kind2string( _temp204); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp208.last_plus_one - _temp208.curr, _temp208.curr, _temp209.last_plus_one -
_temp209.curr, _temp209.curr, _temp210.last_plus_one - _temp210.curr, _temp210.curr);}));
goto _LL193; _LL199:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp211=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp211[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp212; _temp212.tag= Cyc_Absyn_Eq_constr_tag;
_temp212.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp212;}); _temp211;})));
goto _LL193; _LL201:(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp213=( char*)"forward after compress in conref";
struct _tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 33; _temp214;})); goto _LL193; _LL193:;}}}{
struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp66);
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp66);{ struct Cyc_List_List* all_inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp62->tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst); struct Cyc_List_List* tqts= _temp60->typs; if( tqts ==
0){ t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp215=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp215[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp216; _temp216.tag= Cyc_Absyn_TunionType_tag;
_temp216.f1=({ struct Cyc_Absyn_TunionInfo _temp217; _temp217.name=( struct
_tuple1*)( _temp62->name)->v; _temp217.targs= all_typs; _temp217.rgn=( void*)
Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp217.tud=( struct Cyc_Absyn_Tuniondecl*)
_temp62; _temp217;}); _temp216;}); _temp215;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp218=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp218[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp219; _temp219.tag= Cyc_Absyn_TunionFieldType_tag; _temp219.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp220; _temp220.name=( struct _tuple1*)( _temp62->name)->v; _temp220.targs=
all_typs; _temp220.fname= _temp60->name; _temp220.tud=( struct Cyc_Absyn_Tuniondecl*)
_temp62; _temp220.tufd=( struct Cyc_Absyn_Tunionfield*) _temp60; _temp220;});
_temp219;}); _temp218;});} for( 0; _temp64 != 0? tqts != 0: 0; _temp64= _temp64->tl,
tqts= tqts->tl){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) _temp64->hd;
struct _tuple5 _temp223; struct Cyc_List_List* _temp224; struct Cyc_List_List*
_temp226; struct _tuple5* _temp221= Cyc_Tcpat_tcPat( te2, p2, region_opt);
_temp223=* _temp221; _LL227: _temp226= _temp223.f1; goto _LL225; _LL225:
_temp224= _temp223.f2; goto _LL222; _LL222: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp226); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp224);{ void* field_typ= Cyc_Tcutil_substitute(
all_inst,(*(( struct _tuple7*) tqts->hd)).f2); if( ! Cyc_Tcutil_unify(( void*)(
p2->topt)->v, field_typ)){ Cyc_Tcutil_terr( p2->loc,({ struct _tagged_string
_temp228= Cyc_Absynpp_qvar2string( _temp70); struct _tagged_string _temp229= Cyc_Absynpp_typ2string(
field_typ); struct _tagged_string _temp230= Cyc_Absynpp_typ2string(( void*)( p2->topt)->v);
xprintf("%.*s expects argument type %.*s, not %.*s", _temp228.last_plus_one -
_temp228.curr, _temp228.curr, _temp229.last_plus_one - _temp229.curr, _temp229.curr,
_temp230.last_plus_one - _temp230.curr, _temp230.curr);}));}}} if( _temp64 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp231= Cyc_Absynpp_qvar2string(
_temp70); xprintf("too many arguments for tunion constructor %.*s", _temp231.last_plus_one
- _temp231.curr, _temp231.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp232= Cyc_Absynpp_qvar2string( _temp70); xprintf("too few arguments for tunion constructor %.*s",
_temp232.last_plus_one - _temp232.curr, _temp232.curr);}));} goto _LL2;}} _LL26: {
struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp78);
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp78);{ struct Cyc_List_List* tqts= _temp72->typs;
t=( void*)({ struct Cyc_Absyn_XTunionFieldType_struct* _temp233=( struct Cyc_Absyn_XTunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionFieldType_struct)); _temp233[ 0]=({
struct Cyc_Absyn_XTunionFieldType_struct _temp234; _temp234.tag= Cyc_Absyn_XTunionFieldType_tag;
_temp234.f1=({ struct Cyc_Absyn_XTunionFieldInfo _temp235; _temp235.name=
_temp74->name; _temp235.fname= _temp72->name; _temp235.xtud=( struct Cyc_Absyn_XTuniondecl*)
_temp74; _temp235.xtufd=( struct Cyc_Absyn_Tunionfield*) _temp72; _temp235;});
_temp234;}); _temp233;}); for( 0; _temp76 != 0? tqts != 0: 0; _temp76= _temp76->tl,
tqts= tqts->tl){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) _temp76->hd;
struct _tuple5 _temp238; struct Cyc_List_List* _temp239; struct Cyc_List_List*
_temp241; struct _tuple5* _temp236= Cyc_Tcpat_tcPat( te2, p2, region_opt);
_temp238=* _temp236; _LL242: _temp241= _temp238.f1; goto _LL240; _LL240:
_temp239= _temp238.f2; goto _LL237; _LL237: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp241); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp239);{ void* field_typ=(*((
struct _tuple7*) tqts->hd)).f2; if( ! Cyc_Tcutil_unify(( void*)( p2->topt)->v,
field_typ)){ Cyc_Tcutil_terr( p2->loc,({ struct _tagged_string _temp243= Cyc_Absynpp_qvar2string(
_temp80); struct _tagged_string _temp244= Cyc_Absynpp_typ2string( field_typ);
struct _tagged_string _temp245= Cyc_Absynpp_typ2string(( void*)( p2->topt)->v);
xprintf("%.*s expects argument type %.*s, not %.*s", _temp243.last_plus_one -
_temp243.curr, _temp243.curr, _temp244.last_plus_one - _temp244.curr, _temp244.curr,
_temp245.last_plus_one - _temp245.curr, _temp245.curr);}));}}} if( _temp76 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp246= Cyc_Absynpp_qvar2string(
_temp80); xprintf("too many arguments for xtunion constructor %.*s", _temp246.last_plus_one
- _temp246.curr, _temp246.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp247= Cyc_Absynpp_qvar2string( _temp80); xprintf("too few arguments for xtunion constructor %.*s",
_temp247.last_plus_one - _temp247.curr, _temp247.curr);}));} goto _LL2;}} _LL28:
t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp248=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp248[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp249; _temp249.tag= Cyc_Absyn_EnumType_tag;
_temp249.f1= _temp84->name; _temp249.f2=( struct Cyc_Absyn_Enumdecl*) _temp84;
_temp249;}); _temp248;}); goto _LL2; _LL30:{ struct _handler_cons _temp250;
_push_handler(& _temp250);{ struct _xtunion_struct* _temp251=( struct
_xtunion_struct*) setjmp( _temp250.handler); if( ! _temp251){{ void* _temp252=
Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp88); struct Cyc_Absyn_Structdecl*
_temp264; struct Cyc_Absyn_Tunionfield* _temp266; struct Cyc_Absyn_Tuniondecl*
_temp268; struct Cyc_Absyn_Tunionfield* _temp270; struct Cyc_Absyn_XTuniondecl*
_temp272; struct Cyc_Absyn_Enumfield* _temp274; struct Cyc_Absyn_Enumdecl*
_temp276; void* _temp278; _LL254: if((( struct _tunion_struct*) _temp252)->tag
== Cyc_Tcenv_StructRes_tag){ _LL265: _temp264=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp252)->f1; goto _LL255;} else{ goto
_LL256;} _LL256: if((( struct _tunion_struct*) _temp252)->tag == Cyc_Tcenv_TunionRes_tag){
_LL269: _temp268=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp252)->f1; goto _LL267; _LL267: _temp266=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp252)->f2; goto _LL257;} else{ goto
_LL258;} _LL258: if((( struct _tunion_struct*) _temp252)->tag == Cyc_Tcenv_XTunionRes_tag){
_LL273: _temp272=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp252)->f1; goto _LL271; _LL271: _temp270=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp252)->f2; goto _LL259;} else{ goto
_LL260;} _LL260: if((( struct _tunion_struct*) _temp252)->tag == Cyc_Tcenv_EnumRes_tag){
_LL277: _temp276=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp252)->f1; goto _LL275; _LL275: _temp274=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp252)->f2; goto _LL261;} else{ goto _LL262;}
_LL262: if((( struct _tunion_struct*) _temp252)->tag == Cyc_Tcenv_VarRes_tag){
_LL279: _temp278=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp252)->f1; goto
_LL263;} else{ goto _LL253;} _LL255: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp280=( char*)"struct tag used without arguments in pattern";
struct _tagged_string _temp281; _temp281.curr= _temp280; _temp281.base= _temp280;
_temp281.last_plus_one= _temp280 + 45; _temp281;})); t= Cyc_Absyn_wildtyp();
goto _LL253; _LL257:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp282=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp282[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp283; _temp283.tag= Cyc_Absyn_Tunion_p_tag;
_temp283.f1= _temp266->name; _temp283.f2= 0; _temp283.f3= 0; _temp283.f4= 0;
_temp283.f5= _temp268; _temp283.f6= _temp266; _temp283;}); _temp282;})));{
struct _tuple5* _temp284= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u);
return _temp284;} _LL259:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_XTunion_p_struct*
_temp285=( struct Cyc_Absyn_XTunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct));
_temp285[ 0]=({ struct Cyc_Absyn_XTunion_p_struct _temp286; _temp286.tag= Cyc_Absyn_XTunion_p_tag;
_temp286.f1= _temp270->name; _temp286.f2= 0; _temp286.f3= 0; _temp286.f4=
_temp272; _temp286.f5= _temp270; _temp286;}); _temp285;})));{ struct _tuple5*
_temp287= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u); return
_temp287;} _LL261:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp288=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp288[ 0]=({ struct Cyc_Absyn_Enum_p_struct _temp289; _temp289.tag= Cyc_Absyn_Enum_p_tag;
_temp289.f1= _temp274->name; _temp289.f2= _temp276; _temp289.f3= _temp274;
_temp289;}); _temp288;})));{ struct _tuple5* _temp290= Cyc_Tcpat_tcPat( te, p,
region_opt); _npop_handler( 0u); return _temp290;} _LL263:{ void* _temp291=(*
_temp88).f1; struct Cyc_List_List* _temp299; _LL293: if(( int) _temp291 == Cyc_Absyn_Loc_n){
goto _LL294;} else{ goto _LL295;} _LL295: if(( unsigned int) _temp291 > 1u?((
struct _tunion_struct*) _temp291)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL300:
_temp299=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp291)->f1;
if( _temp299 == 0){ goto _LL296;} else{ goto _LL297;}} else{ goto _LL297;}
_LL297: goto _LL298; _LL294: goto _LL296; _LL296:(* _temp88).f1=( void*) Cyc_Absyn_Loc_n;(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp301=(
struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp301[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp302; _temp302.tag= Cyc_Absyn_Var_p_tag;
_temp302.f1= Cyc_Absyn_new_vardecl( _temp88,( void*) Cyc_Absyn_VoidType, 0);
_temp302;}); _temp301;})));{ struct _tuple5* _temp303= Cyc_Tcpat_tcPat( te, p,
region_opt); _npop_handler( 0u); return _temp303;} _LL298: Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp304=( char*)"qualified variable in pattern";
struct _tagged_string _temp305; _temp305.curr= _temp304; _temp305.base= _temp304;
_temp305.last_plus_one= _temp304 + 30; _temp305;})); goto _LL292; _LL292:;} Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp306= Cyc_Absynpp_qvar2string( _temp88);
xprintf("unresolved identifier %.*s in pattern", _temp306.last_plus_one -
_temp306.curr, _temp306.curr);})); t= Cyc_Absyn_wildtyp(); goto _LL253; _LL253:;};
_pop_handler();} else{ struct _xtunion_struct* _temp308= _temp251; _LL310: if(
_temp308->tag == Cyc_Dict_Absent_tag){ goto _LL311;} else{ goto _LL312;} _LL312:
goto _LL313; _LL311:{ void* _temp314=(* _temp88).f1; struct Cyc_List_List*
_temp322; _LL316: if(( int) _temp314 == Cyc_Absyn_Loc_n){ goto _LL317;} else{
goto _LL318;} _LL318: if(( unsigned int) _temp314 > 1u?(( struct _tunion_struct*)
_temp314)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL323: _temp322=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp314)->f1; if( _temp322 == 0){ goto _LL319;}
else{ goto _LL320;}} else{ goto _LL320;} _LL320: goto _LL321; _LL317: goto
_LL319; _LL319:(* _temp88).f1=( void*) Cyc_Absyn_Loc_n;( void*)( p->r=( void*)((
void*)({ struct Cyc_Absyn_Var_p_struct* _temp324=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp324[ 0]=({ struct Cyc_Absyn_Var_p_struct
_temp325; _temp325.tag= Cyc_Absyn_Var_p_tag; _temp325.f1= Cyc_Absyn_new_vardecl(
_temp88,( void*) Cyc_Absyn_VoidType, 0); _temp325;}); _temp324;}))); return Cyc_Tcpat_tcPat(
te, p, region_opt); _LL321: Cyc_Tcutil_terr( p->loc,({ struct _tagged_string
_temp326= Cyc_Absynpp_qvar2string( _temp88); xprintf("unbound identifier %.*s in pattern",
_temp326.last_plus_one - _temp326.curr, _temp326.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL315; _LL315:;} goto _LL309; _LL313:( void) _throw( _temp308); _LL309:;}}}
goto _LL2; _LL32:{ struct _handler_cons _temp327; _push_handler(& _temp327);{
struct _xtunion_struct* _temp328=( struct _xtunion_struct*) setjmp( _temp327.handler);
if( ! _temp328){{ void* _temp329= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp94);
struct Cyc_Absyn_Structdecl* _temp341; struct Cyc_Absyn_Tunionfield* _temp343;
struct Cyc_Absyn_Tuniondecl* _temp345; struct Cyc_Absyn_Tunionfield* _temp347;
struct Cyc_Absyn_XTuniondecl* _temp349; struct Cyc_Absyn_Enumfield* _temp351;
struct Cyc_Absyn_Enumdecl* _temp353; void* _temp355; _LL331: if((( struct
_tunion_struct*) _temp329)->tag == Cyc_Tcenv_StructRes_tag){ _LL342: _temp341=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp329)->f1;
goto _LL332;} else{ goto _LL333;} _LL333: if((( struct _tunion_struct*) _temp329)->tag
== Cyc_Tcenv_TunionRes_tag){ _LL346: _temp345=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp329)->f1; goto _LL344; _LL344: _temp343=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp329)->f2;
goto _LL334;} else{ goto _LL335;} _LL335: if((( struct _tunion_struct*) _temp329)->tag
== Cyc_Tcenv_XTunionRes_tag){ _LL350: _temp349=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp329)->f1; goto _LL348; _LL348:
_temp347=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp329)->f2; goto _LL336;} else{ goto _LL337;} _LL337: if((( struct
_tunion_struct*) _temp329)->tag == Cyc_Tcenv_EnumRes_tag){ _LL354: _temp353=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp329)->f1;
goto _LL352; _LL352: _temp351=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp329)->f2; goto _LL338;} else{ goto _LL339;} _LL339: if((( struct
_tunion_struct*) _temp329)->tag == Cyc_Tcenv_VarRes_tag){ _LL356: _temp355=(
void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp329)->f1; goto _LL340;} else{
goto _LL330;} _LL332: { struct Cyc_List_List* x=(( struct Cyc_List_List*(*)(
struct _tuple6*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcpat_null_designator, _temp90);( void*)( p->r=( void*)(( void*)({ struct
Cyc_Absyn_Struct_p_struct* _temp357=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp357[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp358; _temp358.tag= Cyc_Absyn_Struct_p_tag;
_temp358.f1= _temp341; _temp358.f2= 0; _temp358.f3= _temp92; _temp358.f4= x;
_temp358;}); _temp357;})));{ struct _tuple5* _temp359= Cyc_Tcpat_tcPat( te, p,
region_opt); _npop_handler( 0u); return _temp359;}} _LL334:( void*)( p->r=( void*)((
void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp360=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp360[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp361; _temp361.tag= Cyc_Absyn_Tunion_p_tag;
_temp361.f1= _temp343->name; _temp361.f2= 0; _temp361.f3= _temp92; _temp361.f4=
_temp90; _temp361.f5= _temp345; _temp361.f6= _temp343; _temp361;}); _temp360;})));{
struct _tuple5* _temp362= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u);
return _temp362;} _LL336:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_XTunion_p_struct*
_temp363=( struct Cyc_Absyn_XTunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct));
_temp363[ 0]=({ struct Cyc_Absyn_XTunion_p_struct _temp364; _temp364.tag= Cyc_Absyn_XTunion_p_tag;
_temp364.f1= _temp347->name; _temp364.f2= _temp92; _temp364.f3= _temp90;
_temp364.f4= _temp349; _temp364.f5= _temp347; _temp364;}); _temp363;})));{
struct _tuple5* _temp365= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u);
return _temp365;} _LL338: Cyc_Tcutil_terr( p->loc,({ struct _tagged_string
_temp366= Cyc_Absynpp_qvar2string( _temp94); xprintf("bad enum constructor %.*s in pattern",
_temp366.last_plus_one - _temp366.curr, _temp366.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL330; _LL340: goto _LL330; _LL330:;}; _pop_handler();} else{ struct
_xtunion_struct* _temp368= _temp328; _LL370: if( _temp368->tag == Cyc_Dict_Absent_tag){
goto _LL371;} else{ goto _LL372;} _LL372: goto _LL373; _LL371: goto _LL369;
_LL373:( void) _throw( _temp368); _LL369:;}}} Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp374= Cyc_Absynpp_qvar2string( _temp94); xprintf("%.*s is not a constructor in pattern",
_temp374.last_plus_one - _temp374.curr, _temp374.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL2; _LL34:{ struct _handler_cons _temp375; _push_handler(& _temp375);{
struct _xtunion_struct* _temp376=( struct _xtunion_struct*) setjmp( _temp375.handler);
if( ! _temp376){{ void* _temp377= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp100); struct Cyc_Absyn_Structdecl* _temp389; struct Cyc_Absyn_Tunionfield*
_temp391; struct Cyc_Absyn_Tuniondecl* _temp393; struct Cyc_Absyn_Tunionfield*
_temp395; struct Cyc_Absyn_XTuniondecl* _temp397; void* _temp399; struct Cyc_Absyn_Enumfield*
_temp401; struct Cyc_Absyn_Enumdecl* _temp403; _LL379: if((( struct
_tunion_struct*) _temp377)->tag == Cyc_Tcenv_StructRes_tag){ _LL390: _temp389=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp377)->f1;
goto _LL380;} else{ goto _LL381;} _LL381: if((( struct _tunion_struct*) _temp377)->tag
== Cyc_Tcenv_TunionRes_tag){ _LL394: _temp393=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp377)->f1; goto _LL392; _LL392: _temp391=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp377)->f2;
goto _LL382;} else{ goto _LL383;} _LL383: if((( struct _tunion_struct*) _temp377)->tag
== Cyc_Tcenv_XTunionRes_tag){ _LL398: _temp397=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp377)->f1; goto _LL396; _LL396:
_temp395=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp377)->f2; goto _LL384;} else{ goto _LL385;} _LL385: if((( struct
_tunion_struct*) _temp377)->tag == Cyc_Tcenv_VarRes_tag){ _LL400: _temp399=(
void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp377)->f1; goto _LL386;} else{
goto _LL387;} _LL387: if((( struct _tunion_struct*) _temp377)->tag == Cyc_Tcenv_EnumRes_tag){
_LL404: _temp403=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp377)->f1; goto _LL402; _LL402: _temp401=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp377)->f2; goto _LL388;} else{ goto _LL378;}
_LL380:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct*
_temp405=( struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp405[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp406; _temp406.tag= Cyc_Absyn_Struct_p_tag;
_temp406.f1= _temp389; _temp406.f2= 0; _temp406.f3= _temp98; _temp406.f4=
_temp96; _temp406;}); _temp405;})));{ struct _tuple5* _temp407= Cyc_Tcpat_tcPat(
te, p, region_opt); _npop_handler( 0u); return _temp407;} _LL382: Cyc_Tcutil_terr(
p->loc,( struct _tagged_string)({ char* _temp408=( char*)"tunion with designators not implemented";
struct _tagged_string _temp409; _temp409.curr= _temp408; _temp409.base= _temp408;
_temp409.last_plus_one= _temp408 + 40; _temp409;})); t= Cyc_Absyn_wildtyp();
goto _LL378; _LL384: Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char*
_temp410=( char*)"xtunion with designators not implemented"; struct
_tagged_string _temp411; _temp411.curr= _temp410; _temp411.base= _temp410;
_temp411.last_plus_one= _temp410 + 41; _temp411;})); t= Cyc_Absyn_wildtyp();
goto _LL378; _LL386: Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp412=
Cyc_Absynpp_qvar2string( _temp100); xprintf("unbound struct %.*s in pattern",
_temp412.last_plus_one - _temp412.curr, _temp412.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL378; _LL388: Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp413=
Cyc_Absynpp_qvar2string( _temp100); xprintf("enum field %.*s used in struct pattern",
_temp413.last_plus_one - _temp413.curr, _temp413.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL378; _LL378:;}; _pop_handler();} else{ struct _xtunion_struct* _temp415=
_temp376; _LL417: if( _temp415->tag == Cyc_Dict_Absent_tag){ goto _LL418;} else{
goto _LL419;} _LL419: goto _LL420; _LL418: Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp421= Cyc_Absynpp_qvar2string( _temp100); xprintf("unbound struct %.*s in pattern",
_temp421.last_plus_one - _temp421.curr, _temp421.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL416; _LL420:( void) _throw( _temp415); _LL416:;}}} goto _LL2; _LL2:;} p->topt=({
struct Cyc_Core_Opt* _temp422=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp422->v=( void*) t; _temp422;}); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _tagged_string*(* f)( struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_name, v_result), p->loc,(
struct _tagged_string)({ char* _temp423=( char*)"pattern contains a repeated variable";
struct _tagged_string _temp424; _temp424.curr= _temp423; _temp424.base= _temp423;
_temp424.last_plus_one= _temp423 + 37; _temp424;})); return({ struct _tuple5*
_temp425=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp425->f1=
tv_result; _temp425->f2= v_result; _temp425;});} void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p){ void* _temp426=( void*) p->r;
struct Cyc_Absyn_Pat* _temp440; struct Cyc_Absyn_Tunionfield* _temp442; struct
Cyc_Absyn_Tuniondecl* _temp444; struct Cyc_List_List* _temp446; struct Cyc_List_List*
_temp448; struct Cyc_Core_Opt* _temp450; struct _tuple1* _temp452; struct Cyc_Absyn_Tunionfield*
_temp454; struct Cyc_Absyn_XTuniondecl* _temp456; struct Cyc_List_List* _temp458;
struct Cyc_List_List* _temp460; struct _tuple1* _temp462; struct Cyc_List_List*
_temp464; struct Cyc_List_List* _temp466; struct Cyc_Core_Opt* _temp468; struct
Cyc_Absyn_Structdecl* _temp470; struct Cyc_List_List* _temp472; _LL428: if((
unsigned int) _temp426 > 2u?(( struct _tunion_struct*) _temp426)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL441: _temp440=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp426)->f1; goto _LL429;} else{ goto _LL430;} _LL430: if(( unsigned int)
_temp426 > 2u?(( struct _tunion_struct*) _temp426)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL453: _temp452=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp426)->f1; goto _LL451; _LL451: _temp450=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp426)->f2; goto _LL449; _LL449: _temp448=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp426)->f3; goto
_LL447; _LL447: _temp446=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp426)->f4; goto _LL445; _LL445: _temp444=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp426)->f5; goto _LL443; _LL443: _temp442=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp426)->f6;
goto _LL431;} else{ goto _LL432;} _LL432: if(( unsigned int) _temp426 > 2u?((
struct _tunion_struct*) _temp426)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL463:
_temp462=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp426)->f1;
goto _LL461; _LL461: _temp460=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp426)->f2; goto _LL459; _LL459: _temp458=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_p_struct*) _temp426)->f3; goto _LL457; _LL457: _temp456=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp426)->f4;
goto _LL455; _LL455: _temp454=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp426)->f5; goto _LL433;} else{ goto _LL434;} _LL434: if(( unsigned int)
_temp426 > 2u?(( struct _tunion_struct*) _temp426)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL471: _temp470=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp426)->f1; goto _LL469; _LL469: _temp468=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp426)->f2; goto _LL467; _LL467: _temp466=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp426)->f3; goto
_LL465; _LL465: _temp464=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp426)->f4; goto _LL435;} else{ goto _LL436;} _LL436: if(( unsigned int)
_temp426 > 2u?(( struct _tunion_struct*) _temp426)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL473: _temp472=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp426)->f1; goto _LL437;} else{ goto _LL438;} _LL438: goto _LL439; _LL429:
Cyc_Tcpat_check_pat_regions( te, _temp440);{ void* _temp474=( void*)( p->topt)->v;
struct Cyc_Absyn_PtrInfo _temp484; struct Cyc_Absyn_Conref* _temp486; struct Cyc_Absyn_Tqual*
_temp488; struct Cyc_Absyn_Conref* _temp490; void* _temp492; void* _temp494;
struct Cyc_Absyn_TunionInfo _temp496; struct Cyc_Absyn_Tuniondecl* _temp498;
void* _temp500; struct Cyc_List_List* _temp502; struct _tuple1* _temp504; struct
Cyc_Absyn_XTunionInfo _temp506; struct Cyc_Absyn_XTuniondecl* _temp508; void*
_temp510; struct _tuple1* _temp512; _LL476: if(( unsigned int) _temp474 > 4u?((
struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL485:
_temp484=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp474)->f1; _LL495: _temp494=( void*) _temp484.elt_typ; goto _LL493; _LL493:
_temp492=( void*) _temp484.rgn_typ; goto _LL491; _LL491: _temp490=( struct Cyc_Absyn_Conref*)
_temp484.nullable; goto _LL489; _LL489: _temp488=( struct Cyc_Absyn_Tqual*)
_temp484.tq; goto _LL487; _LL487: _temp486=( struct Cyc_Absyn_Conref*) _temp484.bounds;
goto _LL477;} else{ goto _LL478;} _LL478: if(( unsigned int) _temp474 > 4u?((
struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL497:
_temp496=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp474)->f1; _LL505: _temp504=( struct _tuple1*) _temp496.name; goto _LL503;
_LL503: _temp502=( struct Cyc_List_List*) _temp496.targs; goto _LL501; _LL501:
_temp500=( void*) _temp496.rgn; goto _LL499; _LL499: _temp498=( struct Cyc_Absyn_Tuniondecl*)
_temp496.tud; goto _LL479;} else{ goto _LL480;} _LL480: if(( unsigned int)
_temp474 > 4u?(( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL507: _temp506=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp474)->f1; _LL513: _temp512=( struct _tuple1*) _temp506.name; goto _LL511;
_LL511: _temp510=( void*) _temp506.rgn; goto _LL509; _LL509: _temp508=( struct
Cyc_Absyn_XTuniondecl*) _temp506.xtud; goto _LL481;} else{ goto _LL482;} _LL482:
goto _LL483; _LL477: _temp500= _temp492; goto _LL479; _LL479: _temp510= _temp500;
goto _LL481; _LL481: Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp510);
return; _LL483:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp514=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp514[ 0]=({ struct Cyc_Core_Impossible_struct _temp515; _temp515.tag= Cyc_Core_Impossible_tag;
_temp515.f1=( struct _tagged_string)({ char* _temp516=( char*)"check_pat_regions: bad pointer type";
struct _tagged_string _temp517; _temp517.curr= _temp516; _temp517.base= _temp516;
_temp517.last_plus_one= _temp516 + 36; _temp517;}); _temp515;}); _temp514;}));
_LL475:;} _LL431: for( 0; _temp446 != 0; _temp446= _temp446->tl){ Cyc_Tcpat_check_pat_regions(
te,( struct Cyc_Absyn_Pat*) _temp446->hd);}{ void* _temp518=( void*)( p->topt)->v;
struct Cyc_Absyn_TunionInfo _temp526; struct Cyc_Absyn_Tuniondecl* _temp528;
void* _temp530; struct Cyc_List_List* _temp532; struct _tuple1* _temp534; struct
Cyc_Absyn_TunionFieldInfo _temp536; _LL520: if(( unsigned int) _temp518 > 4u?((
struct _tunion_struct*) _temp518)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL527:
_temp526=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp518)->f1; _LL535: _temp534=( struct _tuple1*) _temp526.name; goto _LL533;
_LL533: _temp532=( struct Cyc_List_List*) _temp526.targs; goto _LL531; _LL531:
_temp530=( void*) _temp526.rgn; goto _LL529; _LL529: _temp528=( struct Cyc_Absyn_Tuniondecl*)
_temp526.tud; goto _LL521;} else{ goto _LL522;} _LL522: if(( unsigned int)
_temp518 > 4u?(( struct _tunion_struct*) _temp518)->tag == Cyc_Absyn_TunionFieldType_tag:
0){ _LL537: _temp536=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp518)->f1; goto _LL523;} else{ goto _LL524;} _LL524: goto _LL525; _LL521:
Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp530); return; _LL523: return;
_LL525:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp538=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp538[ 0]=({ struct Cyc_Core_Impossible_struct _temp539; _temp539.tag= Cyc_Core_Impossible_tag;
_temp539.f1=( struct _tagged_string)({ char* _temp540=( char*)"check_pat_regions: bad tunion type";
struct _tagged_string _temp541; _temp541.curr= _temp540; _temp541.base= _temp540;
_temp541.last_plus_one= _temp540 + 35; _temp541;}); _temp539;}); _temp538;}));
_LL519:;} _LL433: for( 0; _temp458 != 0; _temp458= _temp458->tl){ Cyc_Tcpat_check_pat_regions(
te,( struct Cyc_Absyn_Pat*) _temp458->hd);}{ void* _temp542=( void*)( p->topt)->v;
struct Cyc_Absyn_XTunionInfo _temp550; struct Cyc_Absyn_XTuniondecl* _temp552;
void* _temp554; struct _tuple1* _temp556; struct Cyc_Absyn_XTunionFieldInfo
_temp558; _LL544: if(( unsigned int) _temp542 > 4u?(( struct _tunion_struct*)
_temp542)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL551: _temp550=( struct Cyc_Absyn_XTunionInfo)((
struct Cyc_Absyn_XTunionType_struct*) _temp542)->f1; _LL557: _temp556=( struct
_tuple1*) _temp550.name; goto _LL555; _LL555: _temp554=( void*) _temp550.rgn;
goto _LL553; _LL553: _temp552=( struct Cyc_Absyn_XTuniondecl*) _temp550.xtud;
goto _LL545;} else{ goto _LL546;} _LL546: if(( unsigned int) _temp542 > 4u?((
struct _tunion_struct*) _temp542)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){
_LL559: _temp558=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp542)->f1; goto _LL547;} else{ goto _LL548;} _LL548: goto _LL549; _LL545:
Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp554); return; _LL547: return;
_LL549:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp560=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp560[ 0]=({ struct Cyc_Core_Impossible_struct _temp561; _temp561.tag= Cyc_Core_Impossible_tag;
_temp561.f1=( struct _tagged_string)({ char* _temp562=( char*)"check_pat_regions: bad xtunion type";
struct _tagged_string _temp563; _temp563.curr= _temp562; _temp563.base= _temp562;
_temp563.last_plus_one= _temp562 + 36; _temp563;}); _temp561;}); _temp560;}));
_LL543:;} _LL435: _temp472=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple6*)) Cyc_Core_snd, _temp464); goto _LL437; _LL437: for( 0; _temp472
!= 0; _temp472= _temp472->tl){ Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)
_temp472->hd);} return; _LL439: return; _LL427:;} typedef void* Cyc_Tcpat_Name_value;
static const int Cyc_Tcpat_Name_v_tag= 0; struct Cyc_Tcpat_Name_v_struct{ int
tag; struct _tagged_string f1; } ; static const int Cyc_Tcpat_Int_v_tag= 1;
struct Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; typedef void* Cyc_Tcpat_name_value_t;
struct Cyc_Tcpat_Con_s{ void* name; int arity; struct Cyc_Core_Opt* span; } ;
typedef struct Cyc_Tcpat_Con_s* Cyc_Tcpat_con_t; typedef void* Cyc_Tcpat_Simple_pat;
static const unsigned int Cyc_Tcpat_Any= 0; static const int Cyc_Tcpat_Con_tag=
0; struct Cyc_Tcpat_Con_struct{ int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List*
f2; } ; typedef void* Cyc_Tcpat_simple_pat_t; static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s* c2){ void* _temp564=( void*)
c1->name; struct _tagged_string _temp570; int _temp572; _LL566: if((( struct
_tunion_struct*) _temp564)->tag == Cyc_Tcpat_Name_v_tag){ _LL571: _temp570=(
struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp564)->f1; goto
_LL567;} else{ goto _LL568;} _LL568: if((( struct _tunion_struct*) _temp564)->tag
== Cyc_Tcpat_Int_v_tag){ _LL573: _temp572=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp564)->f1; goto _LL569;} else{ goto _LL565;} _LL567: { void* _temp574=( void*)
c2->name; struct _tagged_string _temp580; int _temp582; _LL576: if((( struct
_tunion_struct*) _temp574)->tag == Cyc_Tcpat_Name_v_tag){ _LL581: _temp580=(
struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp574)->f1; goto
_LL577;} else{ goto _LL578;} _LL578: if((( struct _tunion_struct*) _temp574)->tag
== Cyc_Tcpat_Int_v_tag){ _LL583: _temp582=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp574)->f1; goto _LL579;} else{ goto _LL575;} _LL577: return Cyc_String_zstrcmp(
_temp570, _temp580); _LL579: return - 1; _LL575:;} _LL569: { void* _temp584=(
void*) c2->name; struct _tagged_string _temp590; int _temp592; _LL586: if(((
struct _tunion_struct*) _temp584)->tag == Cyc_Tcpat_Name_v_tag){ _LL591:
_temp590=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp584)->f1;
goto _LL587;} else{ goto _LL588;} _LL588: if((( struct _tunion_struct*) _temp584)->tag
== Cyc_Tcpat_Int_v_tag){ _LL593: _temp592=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp584)->f1; goto _LL589;} else{ goto _LL585;} _LL587: return 1; _LL589:
return _temp572 - _temp592; _LL585:;} _LL565:;} static struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){
return(( struct Cyc_Set_Set*(*)( int(* comp)( struct Cyc_Tcpat_Con_s*, struct
Cyc_Tcpat_Con_s*))) Cyc_Set_empty)( Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt
Cyc_Tcpat_one_opt=( struct Cyc_Core_Opt){.v=( void*) 1}; static struct Cyc_Core_Opt
Cyc_Tcpat_two_opt=( struct Cyc_Core_Opt){.v=( void*) 2}; static struct Cyc_Core_Opt*
Cyc_Tcpat_one_opt_ptr=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static char
_temp597[ 5u]="null"; static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={
0u,( struct _tagged_string){ _temp597, _temp597, _temp597 + 5u}}; static char
_temp601[ 4u]="ptr"; static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={
0u,( struct _tagged_string){ _temp601, _temp601, _temp601 + 4u}}; static char
_temp605[ 6u]="tuple"; static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={
0u,( struct _tagged_string){ _temp605, _temp605, _temp605 + 6u}}; static struct
Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)((
void*)& Cyc_Tcpat_null_name_value),.arity= 0,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=(
void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity= 1,.span=( struct Cyc_Core_Opt*)&
Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0=( struct
Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity= 1,.span=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt}; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=&
Cyc_Tcpat_null_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0;
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp606=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp606->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp607=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp607[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp608; _temp608.tag= Cyc_Tcpat_Int_v_tag;
_temp608.f1= i; _temp608;}); _temp607;})); _temp606->arity= 0; _temp606->span= 0;
_temp606;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_char_con( char c){ return({
struct Cyc_Tcpat_Con_s* _temp609=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp609->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct*
_temp611=( struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp611[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp612; _temp612.tag= Cyc_Tcpat_Int_v_tag;
_temp612.f1=( int) c; _temp612;}); _temp611;})); _temp609->arity= 0; _temp609->span=({
struct Cyc_Core_Opt* _temp610=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp610->v=( void*) 256; _temp610;}); _temp609;});}
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct _tagged_string f){
return({ struct Cyc_Tcpat_Con_s* _temp613=( struct Cyc_Tcpat_Con_s*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_s)); _temp613->name=( void*)(( void*)({ struct Cyc_Tcpat_Name_v_struct*
_temp614=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct));
_temp614[ 0]=({ struct Cyc_Tcpat_Name_v_struct _temp615; _temp615.tag= Cyc_Tcpat_Name_v_tag;
_temp615.f1= f; _temp615;}); _temp614;})); _temp613->arity= 0; _temp613->span= 0;
_temp613;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_double_con( struct
_tagged_string d){ return({ struct Cyc_Tcpat_Con_s* _temp616=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp616->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp617=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp617[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp618; _temp618.tag= Cyc_Tcpat_Name_v_tag; _temp618.f1= d; _temp618;});
_temp617;})); _temp616->arity= 0; _temp616->span= 0; _temp616;});} static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp619=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp619->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp619->arity=
i; _temp619->span= Cyc_Tcpat_one_opt_ptr; _temp619;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp620=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp620[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp621; _temp621.tag= Cyc_Tcpat_Con_tag; _temp621.f1= Cyc_Tcpat_null_con;
_temp621.f2= 0; _temp621;}); _temp620;});} static void* Cyc_Tcpat_int_pat( int i){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp622=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp622[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp623; _temp623.tag= Cyc_Tcpat_Con_tag; _temp623.f1= Cyc_Tcpat_int_con( i);
_temp623.f2= 0; _temp623;}); _temp622;});} static void* Cyc_Tcpat_char_pat( char
c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp624=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp624[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp625; _temp625.tag= Cyc_Tcpat_Con_tag; _temp625.f1= Cyc_Tcpat_char_con( c);
_temp625.f2= 0; _temp625;}); _temp624;});} static void* Cyc_Tcpat_float_pat(
struct _tagged_string f){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp626=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp626[ 0]=({ struct Cyc_Tcpat_Con_struct _temp627; _temp627.tag= Cyc_Tcpat_Con_tag;
_temp627.f1= Cyc_Tcpat_float_con( f); _temp627.f2= 0; _temp627;}); _temp626;});}
static void* Cyc_Tcpat_double_pat( struct _tagged_string d){ return( void*)({
struct Cyc_Tcpat_Con_struct* _temp628=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp628[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp629; _temp629.tag= Cyc_Tcpat_Con_tag; _temp629.f1= Cyc_Tcpat_double_con( d);
_temp629.f2= 0; _temp629;}); _temp628;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp630=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp630[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp631; _temp631.tag= Cyc_Tcpat_Con_tag; _temp631.f1= Cyc_Tcpat_null_ptr_con;
_temp631.f2=({ struct Cyc_List_List* _temp632=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp632->hd=( void*) p; _temp632->tl= 0;
_temp632;}); _temp631;}); _temp630;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp633=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp633[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp634; _temp634.tag= Cyc_Tcpat_Con_tag; _temp634.f1= Cyc_Tcpat_ptr_con;
_temp634.f2=({ struct Cyc_List_List* _temp635=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp635->hd=( void*) p; _temp635->tl= 0;
_temp635;}); _temp634;}); _temp633;});} static void* Cyc_Tcpat_tuple_pat( struct
Cyc_List_List* ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp636=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp636[ 0]=({ struct Cyc_Tcpat_Con_struct _temp637; _temp637.tag= Cyc_Tcpat_Con_tag;
_temp637.f1= Cyc_Tcpat_tuple_con((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ss)); _temp637.f2= ss; _temp637;}); _temp636;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_string con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List*
ps){ struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp638=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp638->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp639=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp639[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp640; _temp640.tag= Cyc_Tcpat_Name_v_tag; _temp640.f1= con_name; _temp640;});
_temp639;})); _temp638->arity=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ps); _temp638->span= span; _temp638;}); return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp641=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp641[ 0]=({ struct Cyc_Tcpat_Con_struct _temp642; _temp642.tag= Cyc_Tcpat_Con_tag;
_temp642.f1= c; _temp642.f2= ps; _temp642;}); _temp641;});} static void* Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat* p){ void* s;{ void* _temp643=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp677; int _temp679; void* _temp681; char _temp683; struct _tagged_string
_temp685; struct Cyc_List_List* _temp687; struct Cyc_Absyn_Vardecl* _temp689;
struct Cyc_Absyn_Pat* _temp691; struct Cyc_List_List* _temp693; struct Cyc_List_List*
_temp695; struct Cyc_Core_Opt* _temp697; struct Cyc_Absyn_Structdecl* _temp699;
struct Cyc_Absyn_Tunionfield* _temp701; struct Cyc_Absyn_Tuniondecl* _temp703;
struct Cyc_List_List* _temp705; struct Cyc_List_List* _temp707; struct Cyc_Core_Opt*
_temp709; struct _tuple1* _temp711; struct Cyc_Absyn_Tunionfield* _temp713;
struct Cyc_Absyn_XTuniondecl* _temp715; struct Cyc_List_List* _temp717; struct
Cyc_List_List* _temp719; struct _tuple1* _temp721; struct Cyc_Absyn_Enumfield*
_temp723; struct Cyc_Absyn_Enumdecl* _temp725; struct _tuple1* _temp727; struct
_tuple1* _temp729; struct Cyc_List_List* _temp731; struct Cyc_List_List*
_temp733; struct _tuple1* _temp735; struct Cyc_List_List* _temp737; struct Cyc_List_List*
_temp739; struct _tuple1* _temp741; _LL645: if(( int) _temp643 == Cyc_Absyn_Wild_p){
goto _LL646;} else{ goto _LL647;} _LL647: if(( unsigned int) _temp643 > 2u?((
struct _tunion_struct*) _temp643)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL678:
_temp677=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*) _temp643)->f1;
goto _LL648;} else{ goto _LL649;} _LL649: if(( int) _temp643 == Cyc_Absyn_Null_p){
goto _LL650;} else{ goto _LL651;} _LL651: if(( unsigned int) _temp643 > 2u?((
struct _tunion_struct*) _temp643)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL682:
_temp681=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp643)->f1; goto _LL680;
_LL680: _temp679=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp643)->f2; goto
_LL652;} else{ goto _LL653;} _LL653: if(( unsigned int) _temp643 > 2u?(( struct
_tunion_struct*) _temp643)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL684: _temp683=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp643)->f1; goto _LL654;} else{ goto
_LL655;} _LL655: if(( unsigned int) _temp643 > 2u?(( struct _tunion_struct*)
_temp643)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL686: _temp685=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp643)->f1; goto _LL656;}
else{ goto _LL657;} _LL657: if(( unsigned int) _temp643 > 2u?(( struct
_tunion_struct*) _temp643)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL688: _temp687=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp643)->f1; goto
_LL658;} else{ goto _LL659;} _LL659: if(( unsigned int) _temp643 > 2u?(( struct
_tunion_struct*) _temp643)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL690:
_temp689=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp643)->f1; goto _LL660;} else{ goto _LL661;} _LL661: if(( unsigned int)
_temp643 > 2u?(( struct _tunion_struct*) _temp643)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL692: _temp691=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp643)->f1; goto _LL662;} else{ goto _LL663;} _LL663: if(( unsigned int)
_temp643 > 2u?(( struct _tunion_struct*) _temp643)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL700: _temp699=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp643)->f1; goto _LL698; _LL698: _temp697=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp643)->f2; goto _LL696; _LL696: _temp695=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp643)->f3; goto
_LL694; _LL694: _temp693=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp643)->f4; goto _LL664;} else{ goto _LL665;} _LL665: if(( unsigned int)
_temp643 > 2u?(( struct _tunion_struct*) _temp643)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL712: _temp711=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp643)->f1; goto _LL710; _LL710: _temp709=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp643)->f2; goto _LL708; _LL708: _temp707=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp643)->f3; goto
_LL706; _LL706: _temp705=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp643)->f4; goto _LL704; _LL704: _temp703=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp643)->f5; goto _LL702; _LL702: _temp701=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp643)->f6;
goto _LL666;} else{ goto _LL667;} _LL667: if(( unsigned int) _temp643 > 2u?((
struct _tunion_struct*) _temp643)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL722:
_temp721=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp643)->f1;
goto _LL720; _LL720: _temp719=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp643)->f2; goto _LL718; _LL718: _temp717=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_p_struct*) _temp643)->f3; goto _LL716; _LL716: _temp715=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp643)->f4;
goto _LL714; _LL714: _temp713=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp643)->f5; goto _LL668;} else{ goto _LL669;} _LL669: if(( unsigned int)
_temp643 > 2u?(( struct _tunion_struct*) _temp643)->tag == Cyc_Absyn_Enum_p_tag:
0){ _LL728: _temp727=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp643)->f1; goto _LL726; _LL726: _temp725=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp643)->f2; goto _LL724; _LL724: _temp723=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*) _temp643)->f3;
goto _LL670;} else{ goto _LL671;} _LL671: if(( unsigned int) _temp643 > 2u?((
struct _tunion_struct*) _temp643)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL730:
_temp729=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp643)->f1;
goto _LL672;} else{ goto _LL673;} _LL673: if(( unsigned int) _temp643 > 2u?((
struct _tunion_struct*) _temp643)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){
_LL736: _temp735=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp643)->f1; goto _LL734; _LL734: _temp733=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp643)->f2; goto _LL732; _LL732: _temp731=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp643)->f3;
goto _LL674;} else{ goto _LL675;} _LL675: if(( unsigned int) _temp643 > 2u?((
struct _tunion_struct*) _temp643)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){
_LL742: _temp741=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp643)->f1; goto _LL740; _LL740: _temp739=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp643)->f2; goto _LL738; _LL738: _temp737=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp643)->f3;
goto _LL676;} else{ goto _LL644;} _LL646: s=( void*) Cyc_Tcpat_Any; goto _LL644;
_LL648: s=( void*) Cyc_Tcpat_Any; goto _LL644; _LL650: s= Cyc_Tcpat_null_pat();
goto _LL644; _LL652: s= Cyc_Tcpat_int_pat( _temp679); goto _LL644; _LL654: s=
Cyc_Tcpat_char_pat( _temp683); goto _LL644; _LL656: s= Cyc_Tcpat_float_pat(
_temp685); goto _LL644; _LL658: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp687)); goto _LL644; _LL660: s=( void*) Cyc_Tcpat_Any; goto _LL644; _LL662: {
void* ss= Cyc_Tcpat_compile_pat( _temp691);{ void* _temp743= Cyc_Tcutil_compress((
void*)( p->topt)->v); struct Cyc_Absyn_PtrInfo _temp753; struct Cyc_Absyn_Conref*
_temp755; struct Cyc_Absyn_Tqual* _temp757; struct Cyc_Absyn_Conref* _temp759;
void* _temp761; void* _temp763; struct Cyc_Absyn_TunionInfo _temp765; struct Cyc_Absyn_XTunionInfo
_temp767; _LL745: if(( unsigned int) _temp743 > 4u?(( struct _tunion_struct*)
_temp743)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL754: _temp753=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp743)->f1; _LL764: _temp763=( void*)
_temp753.elt_typ; goto _LL762; _LL762: _temp761=( void*) _temp753.rgn_typ; goto
_LL760; _LL760: _temp759=( struct Cyc_Absyn_Conref*) _temp753.nullable; goto
_LL758; _LL758: _temp757=( struct Cyc_Absyn_Tqual*) _temp753.tq; goto _LL756;
_LL756: _temp755=( struct Cyc_Absyn_Conref*) _temp753.bounds; goto _LL746;}
else{ goto _LL747;} _LL747: if(( unsigned int) _temp743 > 4u?(( struct
_tunion_struct*) _temp743)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL766:
_temp765=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp743)->f1; goto _LL748;} else{ goto _LL749;} _LL749: if(( unsigned int)
_temp743 > 4u?(( struct _tunion_struct*) _temp743)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL768: _temp767=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp743)->f1; goto _LL750;} else{ goto _LL751;} _LL751: goto _LL752; _LL746: {
int is_nullable= 0; int still_working= 1; while( still_working) { void* _temp769=(
void*) _temp759->v; struct Cyc_Absyn_Conref* _temp777; int _temp779; _LL771: if((
unsigned int) _temp769 > 1u?(( struct _tunion_struct*) _temp769)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL778: _temp777=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp769)->f1; goto _LL772;} else{ goto _LL773;} _LL773: if(( int) _temp769 ==
Cyc_Absyn_No_constr){ goto _LL774;} else{ goto _LL775;} _LL775: if((
unsigned int) _temp769 > 1u?(( struct _tunion_struct*) _temp769)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL780: _temp779=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp769)->f1;
goto _LL776;} else{ goto _LL770;} _LL772:( void*)( _temp759->v=( void*)(( void*)
_temp777->v)); continue; _LL774:( void*)( _temp759->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp781=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp781[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp782; _temp782.tag= Cyc_Absyn_Eq_constr_tag;
_temp782.f1=( void*) 0; _temp782;}); _temp781;}))); is_nullable= 0;
still_working= 0; goto _LL770; _LL776: is_nullable=( int) _temp779;
still_working= 0; goto _LL770; _LL770:;} if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL744;} _LL748: s= ss; goto _LL744;
_LL750: s= ss; goto _LL744; _LL752: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp783=( char*)"pointer pattern does not have pointer type!";
struct _tagged_string _temp784; _temp784.curr= _temp783; _temp784.base= _temp783;
_temp784.last_plus_one= _temp783 + 44; _temp784;})); goto _LL744; _LL744:;} goto
_LL644;} _LL664: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=(
struct Cyc_List_List*)( _temp699->fields)->v; for( 0; fields != 0; fields=
fields->tl){ int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)
fields->hd)->name,( struct _tagged_string)({ char* _temp785=( char*)""; struct
_tagged_string _temp786; _temp786.curr= _temp785; _temp786.base= _temp785;
_temp786.last_plus_one= _temp785 + 1; _temp786;})) == 0;{ struct Cyc_List_List*
dlps0= _temp693; for( 0; ! found? dlps0 != 0: 0; dlps0= dlps0->tl){ struct
_tuple6 _temp789; struct Cyc_Absyn_Pat* _temp790; struct Cyc_List_List* _temp792;
struct _tuple6* _temp787=( struct _tuple6*) dlps0->hd; _temp789=* _temp787;
_LL793: _temp792= _temp789.f1; goto _LL791; _LL791: _temp790= _temp789.f2; goto
_LL788; _LL788: { struct Cyc_List_List* _temp794= _temp792; struct Cyc_List_List
_temp800; struct Cyc_List_List* _temp801; void* _temp803; struct _tagged_string*
_temp805; _LL796: if( _temp794 == 0){ goto _LL798;} else{ _temp800=* _temp794;
_LL804: _temp803=( void*) _temp800.hd; if((( struct _tunion_struct*) _temp803)->tag
== Cyc_Absyn_FieldName_tag){ _LL806: _temp805=( struct _tagged_string*)(( struct
Cyc_Absyn_FieldName_struct*) _temp803)->f1; goto _LL802;} else{ goto _LL798;}
_LL802: _temp801=( struct Cyc_List_List*) _temp800.tl; if( _temp801 == 0){ goto
_LL797;} else{ goto _LL798;}} _LL798: goto _LL799; _LL797: if( Cyc_String_zstrptrcmp(
_temp805,(( struct Cyc_Absyn_Structfield*) fields->hd)->name) == 0){ ps=({
struct Cyc_List_List* _temp807=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp807->hd=( void*) Cyc_Tcpat_compile_pat( _temp790);
_temp807->tl= ps; _temp807;}); found= 1;} goto _LL795; _LL799:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp808=(
char*)"bad designator(s)"; struct _tagged_string _temp809; _temp809.curr=
_temp808; _temp809.base= _temp808; _temp809.last_plus_one= _temp808 + 18;
_temp809;})); goto _LL795; _LL795:;}}} if( ! found){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp810=(
char*)"bad designator"; struct _tagged_string _temp811; _temp811.curr= _temp810;
_temp811.base= _temp810; _temp811.last_plus_one= _temp810 + 15; _temp811;}));}}}
s= Cyc_Tcpat_tuple_pat( ps); goto _LL644;} _LL666: { int span=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)( _temp703->fields)->v);
s= Cyc_Tcpat_con_pat(*(* _temp701->name).f2,({ struct Cyc_Core_Opt* _temp812=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp812->v=(
void*) span; _temp812;}),(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp705)); goto
_LL644;} _LL668: s= Cyc_Tcpat_con_pat(*(* _temp713->name).f2, 0,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp717)); goto _LL644; _LL670: { int span=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp725->fields); s= Cyc_Tcpat_con_pat(*(* _temp723->name).f2,({
struct Cyc_Core_Opt* _temp813=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp813->v=( void*) span; _temp813;}), 0); goto _LL644;}
_LL672: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp814=( char*)"compile_pat: unknownid"; struct
_tagged_string _temp815; _temp815.curr= _temp814; _temp815.base= _temp814;
_temp815.last_plus_one= _temp814 + 23; _temp815;})); goto _LL644; _LL674: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp816=( char*)"compile_pat: unknowncall"; struct _tagged_string
_temp817; _temp817.curr= _temp816; _temp817.base= _temp816; _temp817.last_plus_one=
_temp816 + 25; _temp817;})); goto _LL644; _LL676: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp818=(
char*)"compile_pat: unknownfields"; struct _tagged_string _temp819; _temp819.curr=
_temp818; _temp819.base= _temp818; _temp819.last_plus_one= _temp818 + 27;
_temp819;})); goto _LL644; _LL644:;} return s;} typedef void* Cyc_Tcpat_Term_desc;
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
Cyc_Tcpat_add_neg( void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp820= td;
struct Cyc_Set_Set* _temp826; struct Cyc_List_List* _temp828; struct Cyc_Tcpat_Con_s*
_temp830; _LL822: if((( struct _tunion_struct*) _temp820)->tag == Cyc_Tcpat_Neg_tag){
_LL827: _temp826=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp820)->f1;
goto _LL823;} else{ goto _LL824;} _LL824: if((( struct _tunion_struct*) _temp820)->tag
== Cyc_Tcpat_Pos_tag){ _LL831: _temp830=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*)
_temp820)->f1; goto _LL829; _LL829: _temp828=( struct Cyc_List_List*)(( struct
Cyc_Tcpat_Pos_struct*) _temp820)->f2; goto _LL825;} else{ goto _LL821;} _LL823:
if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp826, c)){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp832=( char*)"add_neg called when constructor already in set";
struct _tagged_string _temp833; _temp833.curr= _temp832; _temp833.base= _temp832;
_temp833.last_plus_one= _temp832 + 47; _temp833;}));} if( c->span != 0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp826) + 1 >=( int)( c->span)->v:
0){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp834=( char*)"add_neg called when |cs U {c}| >= span(c)"; struct
_tagged_string _temp835; _temp835.curr= _temp834; _temp835.base= _temp834;
_temp835.last_plus_one= _temp834 + 42; _temp835;}));} return( void*)({ struct
Cyc_Tcpat_Neg_struct* _temp836=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp836[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp837; _temp837.tag= Cyc_Tcpat_Neg_tag; _temp837.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp826,
c); _temp837;}); _temp836;}); _LL825: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp838=( char*)"add_neg called when td is Positive";
struct _tagged_string _temp839; _temp839.curr= _temp838; _temp839.base= _temp838;
_temp839.last_plus_one= _temp838 + 35; _temp839;})); _LL821:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp840= td; struct Cyc_List_List*
_temp846; struct Cyc_Tcpat_Con_s* _temp848; struct Cyc_Set_Set* _temp850; _LL842:
if((( struct _tunion_struct*) _temp840)->tag == Cyc_Tcpat_Pos_tag){ _LL849:
_temp848=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp840)->f1;
goto _LL847; _LL847: _temp846=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp840)->f2; goto _LL843;} else{ goto _LL844;} _LL844: if((( struct
_tunion_struct*) _temp840)->tag == Cyc_Tcpat_Neg_tag){ _LL851: _temp850=( struct
Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp840)->f1; goto _LL845;} else{
goto _LL841;} _LL843: if( Cyc_Tcpat_compare_con( c, _temp848) == 0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL845: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp850,
c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)( c->span)->v ==((
int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp850) + 1: 0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}} _LL841:;} static
struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt, void* dsc){
struct Cyc_List_List* _temp852= ctxt; struct Cyc_List_List _temp858; struct Cyc_List_List*
_temp859; struct _tuple9* _temp861; struct _tuple9 _temp863; struct Cyc_List_List*
_temp864; struct Cyc_Tcpat_Con_s* _temp866; _LL854: if( _temp852 == 0){ goto
_LL855;} else{ goto _LL856;} _LL856: if( _temp852 == 0){ goto _LL853;} else{
_temp858=* _temp852; _LL862: _temp861=( struct _tuple9*) _temp858.hd; _temp863=*
_temp861; _LL867: _temp866= _temp863.f1; goto _LL865; _LL865: _temp864= _temp863.f2;
goto _LL860; _LL860: _temp859=( struct Cyc_List_List*) _temp858.tl; goto _LL857;}
_LL855: return 0; _LL857: return({ struct Cyc_List_List* _temp868=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp868->hd=( void*)({ struct
_tuple9* _temp869=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp869->f1= _temp866; _temp869->f2=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp870=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp870->hd=( void*) dsc; _temp870->tl= _temp864; _temp870;}); _temp869;});
_temp868->tl= _temp859; _temp868;}); _LL853:;} static struct Cyc_List_List* Cyc_Tcpat_norm_context(
struct Cyc_List_List* ctxt){ struct Cyc_List_List* _temp871= ctxt; struct Cyc_List_List
_temp877; struct Cyc_List_List* _temp878; struct _tuple9* _temp880; struct
_tuple9 _temp882; struct Cyc_List_List* _temp883; struct Cyc_Tcpat_Con_s*
_temp885; _LL873: if( _temp871 == 0){ goto _LL874;} else{ goto _LL875;} _LL875:
if( _temp871 == 0){ goto _LL872;} else{ _temp877=* _temp871; _LL881: _temp880=(
struct _tuple9*) _temp877.hd; _temp882=* _temp880; _LL886: _temp885= _temp882.f1;
goto _LL884; _LL884: _temp883= _temp882.f2; goto _LL879; _LL879: _temp878=(
struct Cyc_List_List*) _temp877.tl; goto _LL876;} _LL874: return(( struct Cyc_List_List*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp887=( char*)"norm_context: empty context"; struct _tagged_string _temp888;
_temp888.curr= _temp887; _temp888.base= _temp887; _temp888.last_plus_one=
_temp887 + 28; _temp888;})); _LL876: return Cyc_Tcpat_augment( _temp878,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp889=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp889[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp890; _temp890.tag= Cyc_Tcpat_Pos_tag; _temp890.f1= _temp885; _temp890.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp883);
_temp890;}); _temp889;})); _LL872:;} static void* Cyc_Tcpat_build_desc( struct
Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple5
_temp892=({ struct _tuple5 _temp891; _temp891.f1= ctxt; _temp891.f2= work;
_temp891;}); struct Cyc_List_List* _temp902; struct Cyc_List_List* _temp904;
struct Cyc_List_List* _temp906; struct Cyc_List_List* _temp908; struct Cyc_List_List*
_temp910; struct Cyc_List_List* _temp912; struct Cyc_List_List* _temp914; struct
Cyc_List_List _temp916; struct Cyc_List_List* _temp917; struct _tuple10*
_temp919; struct _tuple10 _temp921; struct Cyc_List_List* _temp922; struct Cyc_List_List*
_temp924; struct Cyc_List_List* _temp926; struct Cyc_List_List* _temp928; struct
Cyc_List_List _temp930; struct Cyc_List_List* _temp931; struct _tuple9* _temp933;
struct _tuple9 _temp935; struct Cyc_List_List* _temp936; struct Cyc_Tcpat_Con_s*
_temp938; _LL894: _LL905: _temp904= _temp892.f1; if( _temp904 == 0){ goto _LL903;}
else{ goto _LL896;} _LL903: _temp902= _temp892.f2; if( _temp902 == 0){ goto
_LL895;} else{ goto _LL896;} _LL896: _LL909: _temp908= _temp892.f1; if( _temp908
== 0){ goto _LL907;} else{ goto _LL898;} _LL907: _temp906= _temp892.f2; goto
_LL897; _LL898: _LL913: _temp912= _temp892.f1; goto _LL911; _LL911: _temp910=
_temp892.f2; if( _temp910 == 0){ goto _LL899;} else{ goto _LL900;} _LL900:
_LL929: _temp928= _temp892.f1; if( _temp928 == 0){ goto _LL893;} else{ _temp930=*
_temp928; _LL934: _temp933=( struct _tuple9*) _temp930.hd; _temp935=* _temp933;
_LL939: _temp938= _temp935.f1; goto _LL937; _LL937: _temp936= _temp935.f2; goto
_LL932; _LL932: _temp931=( struct Cyc_List_List*) _temp930.tl; goto _LL915;}
_LL915: _temp914= _temp892.f2; if( _temp914 == 0){ goto _LL893;} else{ _temp916=*
_temp914; _LL920: _temp919=( struct _tuple10*) _temp916.hd; _temp921=* _temp919;
_LL927: _temp926= _temp921.f1; goto _LL925; _LL925: _temp924= _temp921.f2; goto
_LL923; _LL923: _temp922= _temp921.f3; goto _LL918; _LL918: _temp917=( struct
Cyc_List_List*) _temp916.tl; goto _LL901;} _LL895: return dsc; _LL897: return((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp940=( char*)"build_desc: ctxt and work don't match"; struct
_tagged_string _temp941; _temp941.curr= _temp940; _temp941.base= _temp940;
_temp941.last_plus_one= _temp940 + 38; _temp941;})); _LL899: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp942=( char*)"build_desc: ctxt and work don't match"; struct _tagged_string
_temp943; _temp943.curr= _temp942; _temp943.base= _temp942; _temp943.last_plus_one=
_temp942 + 38; _temp943;})); _LL901: { struct Cyc_Tcpat_Pos_struct* td=({ struct
Cyc_Tcpat_Pos_struct* _temp944=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp944[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp945; _temp945.tag= Cyc_Tcpat_Pos_tag; _temp945.f1= _temp938; _temp945.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp936),({
struct Cyc_List_List* _temp946=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp946->hd=( void*) dsc; _temp946->tl= _temp922;
_temp946;})); _temp945;}); _temp944;}); return Cyc_Tcpat_build_desc( _temp931,(
void*) td, _temp917);} _LL893:;} static void* Cyc_Tcpat_match( void* p, void*
obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp947=
allmrules; struct Cyc_List_List _temp953; struct Cyc_List_List* _temp954; struct
_tuple0* _temp956; struct _tuple0 _temp958; void* _temp959; void* _temp961;
_LL949: if( _temp947 == 0){ goto _LL950;} else{ goto _LL951;} _LL951: if(
_temp947 == 0){ goto _LL948;} else{ _temp953=* _temp947; _LL957: _temp956=(
struct _tuple0*) _temp953.hd; _temp958=* _temp956; _LL962: _temp961= _temp958.f1;
goto _LL960; _LL960: _temp959= _temp958.f2; goto _LL955; _LL955: _temp954=(
struct Cyc_List_List*) _temp953.tl; goto _LL952;} _LL950: return( void*) Cyc_Tcpat_Failure;
_LL952: return(( void*(*)( void* p, void* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_match)( _temp961,( void*) Cyc_Tcpat_Obj, dsc, 0, 0, _temp959,
_temp954); _LL948:;} static void* Cyc_Tcpat_match_compile( struct Cyc_List_List*
allmrules){ return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)((
void*)({ struct Cyc_Tcpat_Neg_struct* _temp963=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp963[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp964; _temp964.tag= Cyc_Tcpat_Neg_tag; _temp964.f1= Cyc_Tcpat_empty_con_set();
_temp964;}); _temp963;}), allmrules);} static void* Cyc_Tcpat_and_match( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules){ struct Cyc_List_List* _temp965= work; struct Cyc_List_List
_temp973; struct Cyc_List_List* _temp974; struct _tuple10* _temp976; struct
_tuple10 _temp978; struct Cyc_List_List* _temp979; struct Cyc_List_List*
_temp981; struct Cyc_List_List* _temp983; struct Cyc_List_List _temp985; struct
Cyc_List_List* _temp986; struct _tuple10* _temp988; struct _tuple10 _temp990;
struct Cyc_List_List* _temp991; struct Cyc_List_List* _temp993; struct Cyc_List_List*
_temp995; _LL967: if( _temp965 == 0){ goto _LL968;} else{ goto _LL969;} _LL969:
if( _temp965 == 0){ goto _LL971;} else{ _temp973=* _temp965; _LL977: _temp976=(
struct _tuple10*) _temp973.hd; _temp978=* _temp976; _LL984: _temp983= _temp978.f1;
if( _temp983 == 0){ goto _LL982;} else{ goto _LL971;} _LL982: _temp981= _temp978.f2;
if( _temp981 == 0){ goto _LL980;} else{ goto _LL971;} _LL980: _temp979= _temp978.f3;
if( _temp979 == 0){ goto _LL975;} else{ goto _LL971;} _LL975: _temp974=( struct
Cyc_List_List*) _temp973.tl; goto _LL970;} _LL971: if( _temp965 == 0){ goto
_LL966;} else{ _temp985=* _temp965; _LL989: _temp988=( struct _tuple10*)
_temp985.hd; _temp990=* _temp988; _LL996: _temp995= _temp990.f1; goto _LL994;
_LL994: _temp993= _temp990.f2; goto _LL992; _LL992: _temp991= _temp990.f3; goto
_LL987; _LL987: _temp986=( struct Cyc_List_List*) _temp985.tl; goto _LL972;}
_LL968: return( void*)({ struct Cyc_Tcpat_Success_struct* _temp997=( struct Cyc_Tcpat_Success_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct)); _temp997[ 0]=({ struct Cyc_Tcpat_Success_struct
_temp998; _temp998.tag= Cyc_Tcpat_Success_tag; _temp998.f1=( void*)
right_hand_side; _temp998;}); _temp997;}); _LL970: return(( void*(*)( struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context( ctx), _temp974,
right_hand_side, rules); _LL972: if(( _temp995 == 0? 1: _temp993 == 0)? 1:
_temp991 == 0){ return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp999=( char*)"tcpat:and_match: malformed work frame";
struct _tagged_string _temp1000; _temp1000.curr= _temp999; _temp1000.base=
_temp999; _temp1000.last_plus_one= _temp999 + 38; _temp1000;}));}{ struct Cyc_List_List
_temp1004; struct Cyc_List_List* _temp1005; void* _temp1007; struct Cyc_List_List*
_temp1002=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1001= _temp995;
if( _temp1001 == 0){ _throw( Null_Exception);} _temp1001;}); _temp1004=*
_temp1002; _LL1008: _temp1007=( void*) _temp1004.hd; goto _LL1006; _LL1006:
_temp1005=( struct Cyc_List_List*) _temp1004.tl; goto _LL1003; _LL1003: { struct
Cyc_List_List _temp1012; struct Cyc_List_List* _temp1013; void* _temp1015;
struct Cyc_List_List* _temp1010=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1009= _temp993; if( _temp1009 == 0){ _throw( Null_Exception);} _temp1009;});
_temp1012=* _temp1010; _LL1016: _temp1015=( void*) _temp1012.hd; goto _LL1014;
_LL1014: _temp1013=( struct Cyc_List_List*) _temp1012.tl; goto _LL1011; _LL1011: {
struct Cyc_List_List _temp1020; struct Cyc_List_List* _temp1021; void* _temp1023;
struct Cyc_List_List* _temp1018=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1017= _temp991; if( _temp1017 == 0){ _throw( Null_Exception);} _temp1017;});
_temp1020=* _temp1018; _LL1024: _temp1023=( void*) _temp1020.hd; goto _LL1022;
_LL1022: _temp1021=( struct Cyc_List_List*) _temp1020.tl; goto _LL1019; _LL1019: {
struct _tuple10* wf=({ struct _tuple10* _temp1025=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp1025->f1= _temp1005; _temp1025->f2= _temp1013;
_temp1025->f3= _temp1021; _temp1025;}); return(( void*(*)( void* p, void* obj,
void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp1007,
_temp1015, _temp1023, ctx,({ struct Cyc_List_List* _temp1026=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1026->hd=( void*) wf; _temp1026->tl=
_temp986; _temp1026;}), right_hand_side, rules);}}}} _LL966:;} static struct Cyc_List_List*
Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon, void* dsc){ void* _temp1027=
dsc; struct Cyc_Set_Set* _temp1033; struct Cyc_List_List* _temp1035; struct Cyc_Tcpat_Con_s*
_temp1037; _LL1029: if((( struct _tunion_struct*) _temp1027)->tag == Cyc_Tcpat_Neg_tag){
_LL1034: _temp1033=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*)
_temp1027)->f1; goto _LL1030;} else{ goto _LL1031;} _LL1031: if((( struct
_tunion_struct*) _temp1027)->tag == Cyc_Tcpat_Pos_tag){ _LL1038: _temp1037=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp1027)->f1; goto
_LL1036; _LL1036: _temp1035=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp1027)->f2; goto _LL1032;} else{ goto _LL1028;} _LL1030: { void* any=( void*)({
struct Cyc_Tcpat_Neg_struct* _temp1039=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp1039[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp1040; _temp1040.tag= Cyc_Tcpat_Neg_tag; _temp1040.f1= Cyc_Tcpat_empty_con_set();
_temp1040;}); _temp1039;}); struct Cyc_List_List* res= 0;{ int i= 0; for( 0; i <
pcon->arity; ++ i){ res=({ struct Cyc_List_List* _temp1041=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1041->hd=( void*) any; _temp1041->tl=
res; _temp1041;});}} return res;} _LL1032: return _temp1035; _LL1028:;} static
void* Cyc_Tcpat_getoarg( void* obj, int i){ return( void*)({ struct Cyc_Tcpat_Sel_struct*
_temp1042=( struct Cyc_Tcpat_Sel_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Sel_struct));
_temp1042[ 0]=({ struct Cyc_Tcpat_Sel_struct _temp1043; _temp1043.tag= Cyc_Tcpat_Sel_tag;
_temp1043.f1= i + 1; _temp1043.f2=( void*) obj; _temp1043;}); _temp1042;});}
static struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s* pcon,
void* obj){ return(( struct Cyc_List_List*(*)( int n, void*(* f)( void*, int),
void* env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static
void* Cyc_Tcpat_match( void* p, void* obj, void* dsc, struct Cyc_List_List* ctx,
struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules){
void* _temp1044= p; struct Cyc_List_List* _temp1050; struct Cyc_Tcpat_Con_s*
_temp1052; _LL1046: if(( int) _temp1044 == Cyc_Tcpat_Any){ goto _LL1047;} else{
goto _LL1048;} _LL1048: if(( unsigned int) _temp1044 > 1u?(( struct
_tunion_struct*) _temp1044)->tag == Cyc_Tcpat_Con_tag: 0){ _LL1053: _temp1052=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Con_struct*) _temp1044)->f1; goto
_LL1051; _LL1051: _temp1050=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Con_struct*)
_temp1044)->f2; goto _LL1049;} else{ goto _LL1045;} _LL1047: return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc),
work, right_hand_side, rules); _LL1049: { void* _temp1054= Cyc_Tcpat_static_match(
_temp1052, dsc); _LL1056: if(( int) _temp1054 == Cyc_Tcpat_Yes){ goto _LL1057;}
else{ goto _LL1058;} _LL1058: if(( int) _temp1054 == Cyc_Tcpat_No){ goto _LL1059;}
else{ goto _LL1060;} _LL1060: if(( int) _temp1054 == Cyc_Tcpat_Maybe){ goto
_LL1061;} else{ goto _LL1055;} _LL1057: { struct Cyc_List_List* ctx2=({ struct
Cyc_List_List* _temp1062=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1062->hd=( void*)({ struct _tuple9* _temp1063=( struct _tuple9*) GC_malloc(
sizeof( struct _tuple9)); _temp1063->f1= _temp1052; _temp1063->f2= 0; _temp1063;});
_temp1062->tl= ctx; _temp1062;}); struct _tuple10* work_frame=({ struct _tuple10*
_temp1064=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1064->f1=
_temp1050; _temp1064->f2= Cyc_Tcpat_getoargs( _temp1052, obj); _temp1064->f3=
Cyc_Tcpat_getdargs( _temp1052, dsc); _temp1064;}); struct Cyc_List_List* work2=({
struct Cyc_List_List* _temp1065=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1065->hd=( void*) work_frame; _temp1065->tl= work;
_temp1065;}); return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules);} _LL1059: return(( void*(*)( void* dsc,
struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx,
dsc, work), rules); _LL1061: { struct Cyc_List_List* ctx2=({ struct Cyc_List_List*
_temp1066=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1066->hd=( void*)({ struct _tuple9* _temp1067=( struct _tuple9*) GC_malloc(
sizeof( struct _tuple9)); _temp1067->f1= _temp1052; _temp1067->f2= 0; _temp1067;});
_temp1066->tl= ctx; _temp1066;}); struct _tuple10* work_frame=({ struct _tuple10*
_temp1068=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1068->f1=
_temp1050; _temp1068->f2= Cyc_Tcpat_getoargs( _temp1052, obj); _temp1068->f3=
Cyc_Tcpat_getdargs( _temp1052, dsc); _temp1068;}); struct Cyc_List_List* work2=({
struct Cyc_List_List* _temp1069=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1069->hd=( void*) work_frame; _temp1069->tl= work;
_temp1069;}); void* s=(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules); void* f=(( void*(*)( void* dsc, struct Cyc_List_List*
allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg(
dsc, _temp1052), work), rules); return( void*)({ struct Cyc_Tcpat_IfEq_struct*
_temp1070=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp1070[ 0]=({ struct Cyc_Tcpat_IfEq_struct _temp1071; _temp1071.tag= Cyc_Tcpat_IfEq_tag;
_temp1071.f1=( void*) obj; _temp1071.f2= _temp1052; _temp1071.f3=( void*) s;
_temp1071.f4=( void*) f; _temp1071;}); _temp1070;});} _LL1055:;} _LL1045:;}
static void Cyc_Tcpat_check_exhaust_overlap( void* d, void(* not_exhaust)( void*),
void* env1, void(* rhs_appears)( void*, void*), void* env2){ void* _temp1072= d;
void* _temp1080; void* _temp1082; void* _temp1084; struct Cyc_Tcpat_Con_s*
_temp1086; void* _temp1088; _LL1074: if(( int) _temp1072 == Cyc_Tcpat_Failure){
goto _LL1075;} else{ goto _LL1076;} _LL1076: if(( unsigned int) _temp1072 > 1u?((
struct _tunion_struct*) _temp1072)->tag == Cyc_Tcpat_Success_tag: 0){ _LL1081:
_temp1080=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1072)->f1; goto
_LL1077;} else{ goto _LL1078;} _LL1078: if(( unsigned int) _temp1072 > 1u?((
struct _tunion_struct*) _temp1072)->tag == Cyc_Tcpat_IfEq_tag: 0){ _LL1089:
_temp1088=( void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1072)->f1; goto _LL1087;
_LL1087: _temp1086=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1072)->f2; goto _LL1085; _LL1085: _temp1084=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1072)->f3; goto _LL1083; _LL1083: _temp1082=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1072)->f4; goto _LL1079;} else{ goto _LL1073;} _LL1075: not_exhaust( env1);
goto _LL1073; _LL1077: rhs_appears( env2, _temp1080); goto _LL1073; _LL1079:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1084,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1082, not_exhaust, env1, rhs_appears, env2); goto _LL1073; _LL1073:;}
static struct _tuple12* Cyc_Tcpat_get_match( int* r, struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({
struct _tuple11* _temp1090=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11)); _temp1090->f1= 0; _temp1090->f2=( swc->pattern)->loc; _temp1090;});
void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1091=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1091->hd=( void*) sp0; _temp1091->tl=({ struct Cyc_List_List* _temp1092=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1092->hd=(
void*) Cyc_Tcpat_int_pat(* r); _temp1092->tl= 0; _temp1092;}); _temp1091;}));* r=*
r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List* _temp1093=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1093->hd=(
void*) sp0; _temp1093->tl=({ struct Cyc_List_List* _temp1094=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1094->hd=( void*)(( void*) Cyc_Tcpat_Any);
_temp1094->tl= 0; _temp1094;}); _temp1093;}));} return({ struct _tuple12*
_temp1095=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1095->f1=
sp; _temp1095->f2= rhs; _temp1095;});} static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp1096=( char*)"patterns may not be exhaustive"; struct _tagged_string
_temp1097; _temp1097.curr= _temp1096; _temp1097.base= _temp1096; _temp1097.last_plus_one=
_temp1096 + 31; _temp1097;}));} static void Cyc_Tcpat_rule_occurs( int dummy,
struct _tuple11* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( int*, struct Cyc_Absyn_Switch_clause*),
int* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcpat_get_match,({ int*
_temp1098=( int*) GC_malloc_atomic( sizeof( int)); _temp1098[ 0]= 0; _temp1098;}),
swcs); void* dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple11*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=
match_rules->tl){ struct _tuple11 _temp1101; struct Cyc_Position_Segment*
_temp1102; int _temp1104; struct _tuple11* _temp1099=(*(( struct _tuple12*)
match_rules->hd)).f2; _temp1101=* _temp1099; _LL1105: _temp1104= _temp1101.f1;
goto _LL1103; _LL1103: _temp1102= _temp1101.f2; goto _LL1100; _LL1100: if( !
_temp1104){ Cyc_Tcutil_terr( _temp1102,( struct _tagged_string)({ char*
_temp1106=( char*)"redundant pattern"; struct _tagged_string _temp1107;
_temp1107.curr= _temp1106; _temp1107.base= _temp1106; _temp1107.last_plus_one=
_temp1106 + 18; _temp1107;}));}}} static void Cyc_Tcpat_not_exhaust_warn( struct
_tuple13* pr){(* pr).f2= 0; Cyc_Tcutil_warn((* pr).f1,( struct _tagged_string)({
char* _temp1108=( char*)"pattern not exhaustive"; struct _tagged_string
_temp1109; _temp1109.curr= _temp1108; _temp1109.base= _temp1108; _temp1109.last_plus_one=
_temp1108 + 23; _temp1109;}));} static void Cyc_Tcpat_dummy_fn( int i, int j){
return;} int Cyc_Tcpat_check_let_pat_exhaustive( struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List* match_rules=({ struct Cyc_List_List*
_temp1110=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1110->hd=( void*)({ struct _tuple14* _temp1111=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1111->f1= Cyc_Tcpat_compile_pat( p);
_temp1111->f2= 0; _temp1111;}); _temp1110->tl= 0; _temp1110;}); void* dec_tree=((
void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules); struct _tuple13* exhaust_env=({ struct _tuple13* _temp1112=(
struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1112->f1= loc;
_temp1112->f2= 1; _temp1112;});(( void(*)( void* d, void(* not_exhaust)( struct
_tuple13*), struct _tuple13* env1, void(* rhs_appears)( int, int), int env2))
Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_warn,
exhaust_env, Cyc_Tcpat_dummy_fn, 0); return(* exhaust_env).f2;} static struct
_tuple12* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause* swc){ void* sp0=
Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({ struct _tuple11*
_temp1113=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1113->f1=
0; _temp1113->f2=( swc->pattern)->loc; _temp1113;}); return({ struct _tuple12*
_temp1114=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1114->f1=
sp0; _temp1114->f2= rhs; _temp1114;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple11*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=
match_rules->tl){ struct _tuple11* pair=(*(( struct _tuple12*) match_rules->hd)).f2;
if( !(* pair).f1){ Cyc_Tcutil_terr((* pair).f2,( struct _tagged_string)({ char*
_temp1115=( char*)"redundant pattern"; struct _tagged_string _temp1116;
_temp1116.curr= _temp1115; _temp1116.base= _temp1115; _temp1116.last_plus_one=
_temp1115 + 18; _temp1116;}));}}}