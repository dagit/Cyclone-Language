#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct
_tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct _tuple6{ struct
Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple7{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Pat* f2; } ; struct _tuple8{ struct Cyc_Absyn_Structfield* f1;
struct Cyc_Absyn_Pat* f2; } ; struct _tuple9{ struct Cyc_List_List* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct _tuple10{ struct Cyc_Tcpat_Con_s*
f1; struct Cyc_List_List* f2; } ; struct _tuple11{ int f1; struct Cyc_Position_Segment*
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
Cyc_List_rmap( struct _RegionHandle*, void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void*
env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[ 14u];
struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; extern struct Cyc_List_List* Cyc_List_tabulate_c(
int n, void*(* f)( void*, int), void* env); struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{ char*
tag; } ; struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef
struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern
int Cyc_Set_cardinality( struct Cyc_Set_Set* s); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent_tag[ 7u]; struct
Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int
Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern int Cyc_String_zstrcmp(
struct _tagged_string, struct _tagged_string); extern int Cyc_String_zstrptrcmp(
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
struct Cyc_Position_Segment*, struct Cyc_List_List*); static struct
_tagged_string* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl* vd){ return(* vd->name).f2;}
static void* Cyc_Tcpat_any_type( void** topt){ if( topt != 0){ return({ void**
_temp0= topt; if( _temp0 == 0){ _throw( Null_Exception);}* _temp0;});} return
Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);} static void* Cyc_Tcpat_num_type(
void** topt, void* numt){ if( topt != 0? Cyc_Tcutil_coerceable(({ void** _temp1=
topt; if( _temp1 == 0){ _throw( Null_Exception);}* _temp1;})): 0){ return({ void**
_temp2= topt; if( _temp2 == 0){ _throw( Null_Exception);}* _temp2;});} return
numt;} static void Cyc_Tcpat_set_vd( struct Cyc_Absyn_Vardecl* vd, struct Cyc_List_List**
v_result_ptr, void* t){( void*)( vd->type=( void*) t); vd->tq= Cyc_Absyn_empty_tqual();
vd->shadow= 0;* v_result_ptr=({ struct Cyc_List_List* _temp3=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3->hd=( void*) vd; _temp3->tl=*
v_result_ptr; _temp3;});} struct _tuple5 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt){ void* t; struct
Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void* _temp4=(
void*) p->r; int _temp40; void* _temp42; int _temp44; void* _temp46; char
_temp48; struct _tagged_string _temp50; struct Cyc_Absyn_Vardecl* _temp52;
struct Cyc_Absyn_Vardecl* _temp54; struct Cyc_Absyn_Pat* _temp56; struct Cyc_List_List*
_temp58; struct Cyc_List_List* _temp60; struct Cyc_List_List* _temp62; struct
Cyc_Core_Opt* _temp64; struct Cyc_Core_Opt** _temp66; struct Cyc_Absyn_Structdecl*
_temp67; struct Cyc_Absyn_Tunionfield* _temp69; struct Cyc_Absyn_Tuniondecl*
_temp71; struct Cyc_List_List* _temp73; struct Cyc_List_List* _temp75; struct
Cyc_Core_Opt* _temp77; struct _tuple1* _temp79; struct Cyc_Absyn_Tunionfield*
_temp81; struct Cyc_Absyn_XTuniondecl* _temp83; struct Cyc_List_List* _temp85;
struct Cyc_List_List* _temp87; struct _tuple1* _temp89; struct Cyc_Absyn_Enumfield*
_temp91; struct Cyc_Absyn_Enumdecl* _temp93; struct _tuple1* _temp95; struct
_tuple1* _temp97; struct Cyc_List_List* _temp99; struct Cyc_List_List* _temp101;
struct _tuple1* _temp103; struct Cyc_List_List* _temp105; struct Cyc_List_List*
_temp107; struct _tuple1* _temp109; _LL6: if(( unsigned int) _temp4 > 2u?((
struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL43: _temp42=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp4)->f1; if(( int) _temp42 == Cyc_Absyn_Unsigned){
goto _LL41;} else{ goto _LL8;} _LL41: _temp40=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp4)->f2; goto _LL7;} else{ goto _LL8;} _LL8: if(( unsigned int) _temp4 > 2u?((
struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL47: _temp46=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp4)->f1; if(( int) _temp46 == Cyc_Absyn_Signed){
goto _LL45;} else{ goto _LL10;} _LL45: _temp44=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp4)->f2; goto _LL9;} else{ goto _LL10;} _LL10: if(( unsigned int) _temp4 > 2u?((
struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL49: _temp48=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp4)->f1; goto _LL11;} else{ goto
_LL12;} _LL12: if(( unsigned int) _temp4 > 2u?(( struct _tunion_struct*) _temp4)->tag
== Cyc_Absyn_Float_p_tag: 0){ _LL51: _temp50=( struct _tagged_string)(( struct
Cyc_Absyn_Float_p_struct*) _temp4)->f1; goto _LL13;} else{ goto _LL14;} _LL14:
if(( int) _temp4 == Cyc_Absyn_Null_p){ goto _LL15;} else{ goto _LL16;} _LL16:
if(( int) _temp4 == Cyc_Absyn_Wild_p){ goto _LL17;} else{ goto _LL18;} _LL18:
if(( unsigned int) _temp4 > 2u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Var_p_tag:
0){ _LL53: _temp52=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp4)->f1; goto _LL19;} else{ goto _LL20;} _LL20: if(( unsigned int) _temp4 >
2u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Reference_p_tag: 0){
_LL55: _temp54=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp4)->f1; goto _LL21;} else{ goto _LL22;} _LL22: if(( unsigned int) _temp4 >
2u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL57:
_temp56=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*) _temp4)->f1;
goto _LL23;} else{ goto _LL24;} _LL24: if(( unsigned int) _temp4 > 2u?(( struct
_tunion_struct*) _temp4)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL59: _temp58=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp4)->f1; goto
_LL25;} else{ goto _LL26;} _LL26: if(( unsigned int) _temp4 > 2u?(( struct
_tunion_struct*) _temp4)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL68: _temp67=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*) _temp4)->f1;
goto _LL65; _LL65: _temp64=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp4)->f2; _temp66=&(( struct Cyc_Absyn_Struct_p_struct*) _temp4)->f2; goto
_LL63; _LL63: _temp62=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp4)->f3; goto _LL61; _LL61: _temp60=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp4)->f4; goto _LL27;} else{ goto _LL28;} _LL28: if(( unsigned int) _temp4 >
2u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL80:
_temp79=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp4)->f1;
goto _LL78; _LL78: _temp77=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp4)->f2; goto _LL76; _LL76: _temp75=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp4)->f3; goto _LL74; _LL74: _temp73=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp4)->f4; goto _LL72; _LL72: _temp71=( struct Cyc_Absyn_Tuniondecl*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp4)->f5; goto _LL70; _LL70: _temp69=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp4)->f6; goto _LL29;} else{ goto _LL30;}
_LL30: if(( unsigned int) _temp4 > 2u?(( struct _tunion_struct*) _temp4)->tag ==
Cyc_Absyn_XTunion_p_tag: 0){ _LL90: _temp89=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp4)->f1; goto _LL88; _LL88: _temp87=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp4)->f2; goto _LL86; _LL86: _temp85=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp4)->f3; goto _LL84; _LL84: _temp83=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp4)->f4; goto _LL82; _LL82: _temp81=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp4)->f5;
goto _LL31;} else{ goto _LL32;} _LL32: if(( unsigned int) _temp4 > 2u?(( struct
_tunion_struct*) _temp4)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL96: _temp95=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp4)->f1; goto _LL94;
_LL94: _temp93=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp4)->f2; goto _LL92; _LL92: _temp91=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Absyn_Enum_p_struct*) _temp4)->f3; goto _LL33;} else{ goto _LL34;} _LL34:
if(( unsigned int) _temp4 > 2u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL98: _temp97=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp4)->f1; goto _LL35;} else{ goto _LL36;} _LL36: if(( unsigned int) _temp4 >
2u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){
_LL104: _temp103=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp4)->f1; goto _LL102; _LL102: _temp101=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp4)->f2; goto _LL100; _LL100: _temp99=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp4)->f3; goto _LL37;} else{ goto _LL38;} _LL38: if(( unsigned int) _temp4 >
2u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){
_LL110: _temp109=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp4)->f1; goto _LL108; _LL108: _temp107=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp4)->f2; goto _LL106; _LL106: _temp105=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp4)->f3; goto _LL39;} else{ goto _LL5;} _LL7: t= Cyc_Tcpat_num_type( topt,
Cyc_Absyn_uint_t); goto _LL5; _LL9: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t);
goto _LL5; _LL11: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto _LL5;
_LL13: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_float_t); goto _LL5; _LL15: if(
topt != 0){ void* _temp112= Cyc_Tcutil_compress(({ void** _temp111= topt; if(
_temp111 == 0){ _throw( Null_Exception);}* _temp111;})); struct Cyc_Absyn_PtrInfo
_temp118; _LL114: if(( unsigned int) _temp112 > 4u?(( struct _tunion_struct*)
_temp112)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL119: _temp118=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp112)->f1; goto _LL115;} else{ goto
_LL116;} _LL116: goto _LL117; _LL115: t=({ void** _temp120= topt; if( _temp120
== 0){ _throw( Null_Exception);}* _temp120;}); goto tcpat_end; _LL117: goto
_LL113; _LL113:;} t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp121=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp121[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp122; _temp122.tag= Cyc_Absyn_PointerType_tag;
_temp122.f1=({ struct Cyc_Absyn_PtrInfo _temp123; _temp123.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp123.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp123.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp123.tq= Cyc_Absyn_empty_tqual(); _temp123.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp123;}); _temp122;});
_temp121;}); goto _LL5; _LL17: t= Cyc_Tcpat_any_type( topt); goto _LL5; _LL19: t=
Cyc_Tcpat_any_type( topt); Cyc_Tcpat_set_vd( _temp52,& v_result, t); goto _LL5;
_LL21: t= Cyc_Tcpat_any_type( topt); if( region_opt == 0){ Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp124=( char*)"* pattern would point into an unknown region";
struct _tagged_string _temp125; _temp125.curr= _temp124; _temp125.base= _temp124;
_temp125.last_plus_one= _temp124 + 45; _temp125;})); goto _LL5;} Cyc_Tcpat_set_vd(
_temp54,& v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp126=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp126[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp127; _temp127.tag= Cyc_Absyn_PointerType_tag;
_temp127.f1=({ struct Cyc_Absyn_PtrInfo _temp128; _temp128.elt_typ=( void*) t;
_temp128.rgn_typ=( void*)({ void** _temp129= region_opt; if( _temp129 == 0){
_throw( Null_Exception);}* _temp129;}); _temp128.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp128.tq= Cyc_Absyn_empty_tqual();
_temp128.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp128;}); _temp127;}); _temp126;})); goto _LL5; _LL23: { void* inner_typ=(
void*) Cyc_Absyn_VoidType; void** inner_topt= 0; if( topt != 0){ void* _temp131=
Cyc_Tcutil_compress(({ void** _temp130= topt; if( _temp130 == 0){ _throw(
Null_Exception);}* _temp130;})); struct Cyc_Absyn_PtrInfo _temp137; struct Cyc_Absyn_Conref*
_temp139; struct Cyc_Absyn_Tqual* _temp141; struct Cyc_Absyn_Conref* _temp143;
void* _temp145; void* _temp147; _LL133: if(( unsigned int) _temp131 > 4u?((
struct _tunion_struct*) _temp131)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL138:
_temp137=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp131)->f1; _LL148: _temp147=( void*) _temp137.elt_typ; goto _LL146; _LL146:
_temp145=( void*) _temp137.rgn_typ; goto _LL144; _LL144: _temp143=( struct Cyc_Absyn_Conref*)
_temp137.nullable; goto _LL142; _LL142: _temp141=( struct Cyc_Absyn_Tqual*)
_temp137.tq; goto _LL140; _LL140: _temp139=( struct Cyc_Absyn_Conref*) _temp137.bounds;
goto _LL134;} else{ goto _LL135;} _LL135: goto _LL136; _LL134: inner_typ=
_temp147; inner_topt=( void**)& inner_typ; goto _LL132; _LL136: goto _LL132;
_LL132:;}{ void* ptr_rgn= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); struct
Cyc_List_List* _temp151; struct Cyc_List_List* _temp153; struct _tuple5 _temp149=
Cyc_Tcpat_tcPatRec( te, _temp56, inner_topt,( void**)& ptr_rgn); _LL154:
_temp153= _temp149.f1; goto _LL152; _LL152: _temp151= _temp149.f2; goto _LL150;
_LL150: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp153); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp151); if( inner_topt == 0){ void* _temp156= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp155= _temp56->topt; if( _temp155 == 0){
_throw( Null_Exception);} _temp155->v;})); struct Cyc_Absyn_TunionFieldInfo
_temp164; struct Cyc_Absyn_Tunionfield* _temp166; struct Cyc_Absyn_Tuniondecl*
_temp168; struct _tuple1* _temp170; struct Cyc_List_List* _temp172; struct
_tuple1* _temp174; struct Cyc_Absyn_XTunionFieldInfo _temp176; struct Cyc_Absyn_Tunionfield*
_temp178; struct Cyc_Absyn_XTuniondecl* _temp180; struct _tuple1* _temp182;
struct _tuple1* _temp184; _LL158: if(( unsigned int) _temp156 > 4u?(( struct
_tunion_struct*) _temp156)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL165:
_temp164=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp156)->f1; _LL175: _temp174=( struct _tuple1*) _temp164.name; goto _LL173;
_LL173: _temp172=( struct Cyc_List_List*) _temp164.targs; goto _LL171; _LL171:
_temp170=( struct _tuple1*) _temp164.fname; goto _LL169; _LL169: _temp168=(
struct Cyc_Absyn_Tuniondecl*) _temp164.tud; goto _LL167; _LL167: _temp166=(
struct Cyc_Absyn_Tunionfield*) _temp164.tufd; goto _LL159;} else{ goto _LL160;}
_LL160: if(( unsigned int) _temp156 > 4u?(( struct _tunion_struct*) _temp156)->tag
== Cyc_Absyn_XTunionFieldType_tag: 0){ _LL177: _temp176=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp156)->f1; _LL185: _temp184=(
struct _tuple1*) _temp176.name; goto _LL183; _LL183: _temp182=( struct _tuple1*)
_temp176.fname; goto _LL181; _LL181: _temp180=( struct Cyc_Absyn_XTuniondecl*)
_temp176.xtud; goto _LL179; _LL179: _temp178=( struct Cyc_Absyn_Tunionfield*)
_temp176.xtufd; goto _LL161;} else{ goto _LL162;} _LL162: goto _LL163; _LL159: t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp186=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp186[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp187; _temp187.tag= Cyc_Absyn_TunionType_tag;
_temp187.f1=({ struct Cyc_Absyn_TunionInfo _temp188; _temp188.name= _temp174;
_temp188.targs= _temp172; _temp188.rgn=( void*) ptr_rgn; _temp188.tud= _temp168;
_temp188;}); _temp187;}); _temp186;}); goto _LL157; _LL161: t=( void*)({ struct
Cyc_Absyn_XTunionType_struct* _temp189=( struct Cyc_Absyn_XTunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct)); _temp189[ 0]=({ struct
Cyc_Absyn_XTunionType_struct _temp190; _temp190.tag= Cyc_Absyn_XTunionType_tag;
_temp190.f1=({ struct Cyc_Absyn_XTunionInfo _temp191; _temp191.name= _temp184;
_temp191.rgn=( void*) ptr_rgn; _temp191.xtud= _temp180; _temp191;}); _temp190;});
_temp189;}); goto _LL157; _LL163: t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp192=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp192[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp193; _temp193.tag= Cyc_Absyn_PointerType_tag;
_temp193.f1=({ struct Cyc_Absyn_PtrInfo _temp194; _temp194.elt_typ=( void*)((
void*)({ struct Cyc_Core_Opt* _temp195= _temp56->topt; if( _temp195 == 0){
_throw( Null_Exception);} _temp195->v;})); _temp194.rgn_typ=( void*) ptr_rgn;
_temp194.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp194.tq= Cyc_Absyn_empty_tqual(); _temp194.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp194;}); _temp193;}); _temp192;}); goto _LL157;
_LL157:;} else{ t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp196=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp196[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp197; _temp197.tag= Cyc_Absyn_PointerType_tag;
_temp197.f1=({ struct Cyc_Absyn_PtrInfo _temp198; _temp198.elt_typ=( void*)((
void*)({ struct Cyc_Core_Opt* _temp199= _temp56->topt; if( _temp199 == 0){
_throw( Null_Exception);} _temp199->v;})); _temp198.rgn_typ=( void*) ptr_rgn;
_temp198.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp198.tq= Cyc_Absyn_empty_tqual(); _temp198.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp198;}); _temp197;}); _temp196;});} goto _LL5;}}
_LL25: { struct Cyc_List_List* pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if(
topt != 0){ void* _temp201= Cyc_Tcutil_compress(({ void** _temp200= topt; if(
_temp200 == 0){ _throw( Null_Exception);}* _temp200;})); struct Cyc_List_List*
_temp207; _LL203: if(( unsigned int) _temp201 > 4u?(( struct _tunion_struct*)
_temp201)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL208: _temp207=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp201)->f1; goto _LL204;} else{ goto
_LL205;} _LL205: goto _LL206; _LL204: topt_ts= _temp207; goto _LL202; _LL206:
goto _LL202; _LL202:;} for( 0; _temp58 != 0; _temp58=({ struct Cyc_List_List*
_temp209= _temp58; if( _temp209 == 0){ _throw( Null_Exception);} _temp209->tl;})){
void** inner_topt= 0; if( topt_ts != 0){ inner_topt=( void**)&(*(( struct
_tuple6*)({ struct Cyc_List_List* _temp210= topt_ts; if( _temp210 == 0){ _throw(
Null_Exception);} _temp210->hd;}))).f2; topt_ts=({ struct Cyc_List_List*
_temp211= topt_ts; if( _temp211 == 0){ _throw( Null_Exception);} _temp211->tl;});}{
struct Cyc_List_List* _temp215; struct Cyc_List_List* _temp217; struct _tuple5
_temp213= Cyc_Tcpat_tcPatRec( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp212= _temp58; if( _temp212 == 0){ _throw( Null_Exception);} _temp212->hd;}),
inner_topt, region_opt); _LL218: _temp217= _temp213.f1; goto _LL216; _LL216:
_temp215= _temp213.f2; goto _LL214; _LL214: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp217); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp215); pat_ts=({ struct Cyc_List_List*
_temp219=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp219->hd=( void*)({ struct _tuple6* _temp220=( struct _tuple6*) GC_malloc(
sizeof( struct _tuple6)); _temp220->f1= Cyc_Absyn_empty_tqual(); _temp220->f2=(
void*)({ struct Cyc_Core_Opt* _temp222=(( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp221= _temp58; if( _temp221 == 0){ _throw( Null_Exception);} _temp221->hd;}))->topt;
if( _temp222 == 0){ _throw( Null_Exception);} _temp222->v;}); _temp220;});
_temp219->tl= pat_ts; _temp219;});}} t=( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp223=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp223[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp224; _temp224.tag= Cyc_Absyn_TupleType_tag;
_temp224.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
pat_ts); _temp224;}); _temp223;}); goto _LL5;} _LL27: if( _temp62 != 0){( void)
_throw((( struct _xtunion_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp225=( char*)"tcPat: struct<...> not implemented";
struct _tagged_string _temp226; _temp226.curr= _temp225; _temp226.base= _temp225;
_temp226.last_plus_one= _temp225 + 35; _temp226;})));}{ struct Cyc_List_List*
dps0= _temp60; for( 0; dps0 != 0; dps0=({ struct Cyc_List_List* _temp227= dps0;
if( _temp227 == 0){ _throw( Null_Exception);} _temp227->tl;})){ struct Cyc_List_List*
_temp231; struct Cyc_List_List* _temp233; struct _tuple5 _temp229= Cyc_Tcpat_tcPatRec(
te,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp228= dps0; if( _temp228 ==
0){ _throw( Null_Exception);} _temp228->hd;}))).f2, 0, region_opt); _LL234:
_temp233= _temp229.f1; goto _LL232; _LL232: _temp231= _temp229.f2; goto _LL230;
_LL230: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp233); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp231);}}{ struct Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_make_inst_var, _temp67->tvs); struct Cyc_List_List* all_typs=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, all_inst);* _temp66=({
struct Cyc_Core_Opt* _temp235=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp235->v=( void*) all_typs; _temp235;}); t=( void*)({ struct
Cyc_Absyn_StructType_struct* _temp236=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp236[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp237; _temp237.tag= Cyc_Absyn_StructType_tag;
_temp237.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp238= _temp67->name; if( _temp238 == 0){ _throw( Null_Exception);} _temp238->v;}));
_temp237.f2= all_typs; _temp237.f3=({ struct Cyc_Absyn_Structdecl** _temp239=(
struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp239[ 0]= _temp67; _temp239;}); _temp237;}); _temp236;});{ struct Cyc_List_List*
fields=(( struct Cyc_List_List*(*)( struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)( p->loc,
_temp60, _temp67); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp240=
fields; if( _temp240 == 0){ _throw( Null_Exception);} _temp240->tl;})){ struct
_tuple8 _temp244; struct Cyc_Absyn_Pat* _temp245; struct Cyc_Absyn_Structfield*
_temp247; struct _tuple8* _temp242=( struct _tuple8*)({ struct Cyc_List_List*
_temp241= fields; if( _temp241 == 0){ _throw( Null_Exception);} _temp241->hd;});
_temp244=* _temp242; _LL248: _temp247= _temp244.f1; goto _LL246; _LL246:
_temp245= _temp244.f2; goto _LL243; _LL243: { void* inst_fieldtyp= Cyc_Tcutil_substitute(
all_inst,( void*) _temp247->type); if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt*
_temp249= _temp245->topt; if( _temp249 == 0){ _throw( Null_Exception);} _temp249->v;}),
inst_fieldtyp)){ Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp251=*
_temp247->name; struct _tagged_string _temp252= Cyc_Absynpp_typ2string(
inst_fieldtyp); struct _tagged_string _temp253= Cyc_Absynpp_typ2string(( void*)({
struct Cyc_Core_Opt* _temp250= _temp245->topt; if( _temp250 == 0){ _throw(
Null_Exception);} _temp250->v;})); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp251.last_plus_one - _temp251.curr, _temp251.curr, _temp252.last_plus_one -
_temp252.curr, _temp252.curr, _temp253.last_plus_one - _temp253.curr, _temp253.curr);}));}}}
goto _LL5;}} _LL29: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp75) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp69->tvs)){
Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char* _temp254=( char*)"wrong number of existential type variables in pattern";
struct _tagged_string _temp255; _temp255.curr= _temp254; _temp255.base= _temp254;
_temp255.last_plus_one= _temp254 + 54; _temp255;}));}{ struct Cyc_List_List* t=
_temp75; for( 0; t != 0; t=({ struct Cyc_List_List* _temp256= t; if( _temp256 ==
0){ _throw( Null_Exception);} _temp256->tl;})){ struct Cyc_Absyn_Tvar* tv=(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp280= t; if( _temp280 == 0){
_throw( Null_Exception);} _temp280->hd;});(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( tv->kind);{ void* _temp257=(
void*)( tv->kind)->v; void* _temp267; void* _temp269; struct Cyc_Absyn_Conref*
_temp271; _LL259: if(( unsigned int) _temp257 > 1u?(( struct _tunion_struct*)
_temp257)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL268: _temp267=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp257)->f1; if(( int) _temp267 == Cyc_Absyn_BoxKind){
goto _LL260;} else{ goto _LL261;}} else{ goto _LL261;} _LL261: if(( unsigned int)
_temp257 > 1u?(( struct _tunion_struct*) _temp257)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL270: _temp269=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp257)->f1;
goto _LL262;} else{ goto _LL263;} _LL263: if(( int) _temp257 == Cyc_Absyn_No_constr){
goto _LL264;} else{ goto _LL265;} _LL265: if(( unsigned int) _temp257 > 1u?((
struct _tunion_struct*) _temp257)->tag == Cyc_Absyn_Forward_constr_tag: 0){
_LL272: _temp271=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp257)->f1; goto _LL266;} else{ goto _LL258;} _LL260: goto _LL258; _LL262:
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp273= Cyc_Absynpp_qvar2string(
_temp79); struct _tagged_string _temp274=* tv->name; struct _tagged_string
_temp275= Cyc_Absynpp_kind2string( _temp269); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp273.last_plus_one - _temp273.curr, _temp273.curr, _temp274.last_plus_one -
_temp274.curr, _temp274.curr, _temp275.last_plus_one - _temp275.curr, _temp275.curr);}));
goto _LL258; _LL264:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp276=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp276[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp277; _temp277.tag= Cyc_Absyn_Eq_constr_tag;
_temp277.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp277;}); _temp276;})));
goto _LL258; _LL266:( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp278=(
char*)"forward after compress in conref"; struct _tagged_string _temp279;
_temp279.curr= _temp278; _temp279.base= _temp278; _temp279.last_plus_one=
_temp278 + 33; _temp279;}))); _LL258:;}}} tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp75);{ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te,
_temp75); struct Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_make_inst_var, _temp71->tvs); struct Cyc_List_List* all_typs=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, all_inst); struct Cyc_List_List*
tqts= _temp69->typs; if( tqts == 0){ t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp281=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp281[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp282; _temp282.tag= Cyc_Absyn_TunionType_tag;
_temp282.f1=({ struct Cyc_Absyn_TunionInfo _temp283; _temp283.name=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp284= _temp71->name; if( _temp284 == 0){
_throw( Null_Exception);} _temp284->v;}); _temp283.targs= all_typs; _temp283.rgn=(
void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp283.tud=( struct Cyc_Absyn_Tuniondecl*)
_temp71; _temp283;}); _temp282;}); _temp281;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp285=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp285[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp286; _temp286.tag= Cyc_Absyn_TunionFieldType_tag; _temp286.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp287; _temp287.name=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp288=
_temp71->name; if( _temp288 == 0){ _throw( Null_Exception);} _temp288->v;});
_temp287.targs= all_typs; _temp287.fname= _temp69->name; _temp287.tud=( struct
Cyc_Absyn_Tuniondecl*) _temp71; _temp287.tufd=( struct Cyc_Absyn_Tunionfield*)
_temp69; _temp287;}); _temp286;}); _temp285;});} for( 0; _temp73 != 0? tqts != 0:
0; _temp73=({ struct Cyc_List_List* _temp289= _temp73; if( _temp289 == 0){
_throw( Null_Exception);} _temp289->tl;}), tqts=({ struct Cyc_List_List*
_temp290= tqts; if( _temp290 == 0){ _throw( Null_Exception);} _temp290->tl;})){
struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp303= _temp73; if( _temp303 == 0){ _throw( Null_Exception);} _temp303->hd;});
struct Cyc_List_List* _temp293; struct Cyc_List_List* _temp295; struct _tuple5
_temp291= Cyc_Tcpat_tcPatRec( te2, p2, 0, region_opt); _LL296: _temp295=
_temp291.f1; goto _LL294; _LL294: _temp293= _temp291.f2; goto _LL292; _LL292:
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp295); v_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( v_result,
_temp293);{ void* field_typ= Cyc_Tcutil_substitute( all_inst,(*(( struct _tuple6*)({
struct Cyc_List_List* _temp302= tqts; if( _temp302 == 0){ _throw( Null_Exception);}
_temp302->hd;}))).f2); if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt*
_temp297= p2->topt; if( _temp297 == 0){ _throw( Null_Exception);} _temp297->v;}),
field_typ)){ Cyc_Tcutil_terr( p2->loc,({ struct _tagged_string _temp299= Cyc_Absynpp_qvar2string(
_temp79); struct _tagged_string _temp300= Cyc_Absynpp_typ2string( field_typ);
struct _tagged_string _temp301= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp298= p2->topt; if( _temp298 == 0){ _throw( Null_Exception);} _temp298->v;}));
xprintf("%.*s expects argument type %.*s, not %.*s", _temp299.last_plus_one -
_temp299.curr, _temp299.curr, _temp300.last_plus_one - _temp300.curr, _temp300.curr,
_temp301.last_plus_one - _temp301.curr, _temp301.curr);}));}}} if( _temp73 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp304= Cyc_Absynpp_qvar2string(
_temp79); xprintf("too many arguments for tunion constructor %.*s", _temp304.last_plus_one
- _temp304.curr, _temp304.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp305= Cyc_Absynpp_qvar2string( _temp79); xprintf("too few arguments for tunion constructor %.*s",
_temp305.last_plus_one - _temp305.curr, _temp305.curr);}));} goto _LL5;} _LL31:
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp87) !=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp81->tvs)){ Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp306=( char*)"wrong number of existential type variables in pattern";
struct _tagged_string _temp307; _temp307.curr= _temp306; _temp307.base= _temp306;
_temp307.last_plus_one= _temp306 + 54; _temp307;}));}{ struct Cyc_Tcenv_Tenv*
te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp87); tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp87);{ struct Cyc_List_List* tqts= _temp81->typs; t=( void*)({ struct Cyc_Absyn_XTunionFieldType_struct*
_temp308=( struct Cyc_Absyn_XTunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_XTunionFieldType_struct)); _temp308[ 0]=({ struct Cyc_Absyn_XTunionFieldType_struct
_temp309; _temp309.tag= Cyc_Absyn_XTunionFieldType_tag; _temp309.f1=({ struct
Cyc_Absyn_XTunionFieldInfo _temp310; _temp310.name= _temp83->name; _temp310.fname=
_temp81->name; _temp310.xtud=( struct Cyc_Absyn_XTuniondecl*) _temp83; _temp310.xtufd=(
struct Cyc_Absyn_Tunionfield*) _temp81; _temp310;}); _temp309;}); _temp308;});
for( 0; _temp85 != 0? tqts != 0: 0; _temp85=({ struct Cyc_List_List* _temp311=
_temp85; if( _temp311 == 0){ _throw( Null_Exception);} _temp311->tl;}), tqts=({
struct Cyc_List_List* _temp312= tqts; if( _temp312 == 0){ _throw( Null_Exception);}
_temp312->tl;})){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp325= _temp85; if( _temp325 == 0){ _throw( Null_Exception);} _temp325->hd;});
struct Cyc_List_List* _temp315; struct Cyc_List_List* _temp317; struct _tuple5
_temp313= Cyc_Tcpat_tcPatRec( te2, p2, 0, region_opt); _LL318: _temp317=
_temp313.f1; goto _LL316; _LL316: _temp315= _temp313.f2; goto _LL314; _LL314:
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp317); v_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( v_result,
_temp315);{ void* field_typ=(*(( struct _tuple6*)({ struct Cyc_List_List*
_temp324= tqts; if( _temp324 == 0){ _throw( Null_Exception);} _temp324->hd;}))).f2;
if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp319= p2->topt; if(
_temp319 == 0){ _throw( Null_Exception);} _temp319->v;}), field_typ)){ Cyc_Tcutil_terr(
p2->loc,({ struct _tagged_string _temp321= Cyc_Absynpp_qvar2string( _temp89);
struct _tagged_string _temp322= Cyc_Absynpp_typ2string( field_typ); struct
_tagged_string _temp323= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp320= p2->topt; if( _temp320 == 0){ _throw( Null_Exception);} _temp320->v;}));
xprintf("%.*s expects argument type %.*s, not %.*s", _temp321.last_plus_one -
_temp321.curr, _temp321.curr, _temp322.last_plus_one - _temp322.curr, _temp322.curr,
_temp323.last_plus_one - _temp323.curr, _temp323.curr);}));}}} if( _temp85 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp326= Cyc_Absynpp_qvar2string(
_temp89); xprintf("too many arguments for xtunion constructor %.*s", _temp326.last_plus_one
- _temp326.curr, _temp326.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp327= Cyc_Absynpp_qvar2string( _temp89); xprintf("too few arguments for xtunion constructor %.*s",
_temp327.last_plus_one - _temp327.curr, _temp327.curr);}));} goto _LL5;}} _LL33:
t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp328=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp328[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp329; _temp329.tag= Cyc_Absyn_EnumType_tag;
_temp329.f1= _temp93->name; _temp329.f2=( struct Cyc_Absyn_Enumdecl*) _temp93;
_temp329;}); _temp328;}); goto _LL5; _LL35:{ struct _handler_cons _temp330;
_push_handler(& _temp330);{ struct _xtunion_struct* _temp331=( struct
_xtunion_struct*) setjmp( _temp330.handler); if( ! _temp331){{ void* _temp332=
Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp97); struct Cyc_Absyn_Structdecl*
_temp344; struct Cyc_Absyn_Tunionfield* _temp346; struct Cyc_Absyn_Tuniondecl*
_temp348; struct Cyc_Absyn_Tunionfield* _temp350; struct Cyc_Absyn_XTuniondecl*
_temp352; struct Cyc_Absyn_Enumfield* _temp354; struct Cyc_Absyn_Enumdecl*
_temp356; void* _temp358; _LL334: if((( struct _tunion_struct*) _temp332)->tag
== Cyc_Tcenv_StructRes_tag){ _LL345: _temp344=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp332)->f1; goto _LL335;} else{ goto
_LL336;} _LL336: if((( struct _tunion_struct*) _temp332)->tag == Cyc_Tcenv_TunionRes_tag){
_LL349: _temp348=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp332)->f1; goto _LL347; _LL347: _temp346=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp332)->f2; goto _LL337;} else{ goto
_LL338;} _LL338: if((( struct _tunion_struct*) _temp332)->tag == Cyc_Tcenv_XTunionRes_tag){
_LL353: _temp352=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp332)->f1; goto _LL351; _LL351: _temp350=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp332)->f2; goto _LL339;} else{ goto
_LL340;} _LL340: if((( struct _tunion_struct*) _temp332)->tag == Cyc_Tcenv_EnumRes_tag){
_LL357: _temp356=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp332)->f1; goto _LL355; _LL355: _temp354=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp332)->f2; goto _LL341;} else{ goto _LL342;}
_LL342: if((( struct _tunion_struct*) _temp332)->tag == Cyc_Tcenv_VarRes_tag){
_LL359: _temp358=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp332)->f1; goto
_LL343;} else{ goto _LL333;} _LL335: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp360=( char*)"struct tag used without arguments in pattern";
struct _tagged_string _temp361; _temp361.curr= _temp360; _temp361.base= _temp360;
_temp361.last_plus_one= _temp360 + 45; _temp361;})); t= Cyc_Absyn_wildtyp();
_npop_handler( 0u); goto tcpat_end; _LL337:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_p_struct* _temp362=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp362[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp363; _temp363.tag= Cyc_Absyn_Tunion_p_tag;
_temp363.f1= _temp346->name; _temp363.f2= 0; _temp363.f3= 0; _temp363.f4= 0;
_temp363.f5= _temp348; _temp363.f6= _temp346; _temp363;}); _temp362;})));{
struct _tuple5 _temp364= Cyc_Tcpat_tcPatRec( te, p, topt, region_opt);
_npop_handler( 0u); return _temp364;} _LL339:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_XTunion_p_struct* _temp365=( struct Cyc_Absyn_XTunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct)); _temp365[ 0]=({ struct
Cyc_Absyn_XTunion_p_struct _temp366; _temp366.tag= Cyc_Absyn_XTunion_p_tag;
_temp366.f1= _temp350->name; _temp366.f2= 0; _temp366.f3= 0; _temp366.f4=
_temp352; _temp366.f5= _temp350; _temp366;}); _temp365;})));{ struct _tuple5
_temp367= Cyc_Tcpat_tcPatRec( te, p, topt, region_opt); _npop_handler( 0u);
return _temp367;} _LL341:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp368=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp368[ 0]=({ struct Cyc_Absyn_Enum_p_struct _temp369; _temp369.tag= Cyc_Absyn_Enum_p_tag;
_temp369.f1= _temp354->name; _temp369.f2= _temp356; _temp369.f3= _temp354;
_temp369;}); _temp368;})));{ struct _tuple5 _temp370= Cyc_Tcpat_tcPatRec( te, p,
topt, region_opt); _npop_handler( 0u); return _temp370;} _LL343: goto _LL333;
_LL333:;}; _pop_handler();} else{ struct _xtunion_struct* _temp372= _temp331;
_LL374: if( _temp372->tag == Cyc_Dict_Absent_tag){ goto _LL375;} else{ goto
_LL376;} _LL376: goto _LL377; _LL375: goto _LL373; _LL377:( void) _throw(
_temp372); _LL373:;}}}{ void* _temp378=(* _temp97).f1; struct Cyc_List_List*
_temp386; _LL380: if(( int) _temp378 == Cyc_Absyn_Loc_n){ goto _LL381;} else{
goto _LL382;} _LL382: if(( unsigned int) _temp378 > 1u?(( struct _tunion_struct*)
_temp378)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL387: _temp386=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp378)->f1; if( _temp386 == 0){ goto _LL383;}
else{ goto _LL384;}} else{ goto _LL384;} _LL384: goto _LL385; _LL381: goto
_LL383; _LL383:(* _temp97).f1=( void*) Cyc_Absyn_Loc_n;( void*)( p->r=( void*)((
void*)({ struct Cyc_Absyn_Var_p_struct* _temp388=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp388[ 0]=({ struct Cyc_Absyn_Var_p_struct
_temp389; _temp389.tag= Cyc_Absyn_Var_p_tag; _temp389.f1= Cyc_Absyn_new_vardecl(
_temp97,( void*) Cyc_Absyn_VoidType, 0); _temp389;}); _temp388;}))); return Cyc_Tcpat_tcPatRec(
te, p, topt, region_opt); _LL385: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp390=( char*)"qualified variable in pattern"; struct
_tagged_string _temp391; _temp391.curr= _temp390; _temp391.base= _temp390;
_temp391.last_plus_one= _temp390 + 30; _temp391;})); t= Cyc_Absyn_wildtyp();
goto _LL379; _LL379:;} goto _LL5; _LL37:{ struct _handler_cons _temp392;
_push_handler(& _temp392);{ struct _xtunion_struct* _temp393=( struct
_xtunion_struct*) setjmp( _temp392.handler); if( ! _temp393){{ void* _temp394=
Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp103); struct Cyc_Absyn_Structdecl*
_temp406; struct Cyc_Absyn_Tunionfield* _temp408; struct Cyc_Absyn_Tuniondecl*
_temp410; struct Cyc_Absyn_Tunionfield* _temp412; struct Cyc_Absyn_XTuniondecl*
_temp414; struct Cyc_Absyn_Enumfield* _temp416; struct Cyc_Absyn_Enumdecl*
_temp418; void* _temp420; _LL396: if((( struct _tunion_struct*) _temp394)->tag
== Cyc_Tcenv_StructRes_tag){ _LL407: _temp406=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp394)->f1; goto _LL397;} else{ goto
_LL398;} _LL398: if((( struct _tunion_struct*) _temp394)->tag == Cyc_Tcenv_TunionRes_tag){
_LL411: _temp410=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp394)->f1; goto _LL409; _LL409: _temp408=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp394)->f2; goto _LL399;} else{ goto
_LL400;} _LL400: if((( struct _tunion_struct*) _temp394)->tag == Cyc_Tcenv_XTunionRes_tag){
_LL415: _temp414=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp394)->f1; goto _LL413; _LL413: _temp412=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp394)->f2; goto _LL401;} else{ goto
_LL402;} _LL402: if((( struct _tunion_struct*) _temp394)->tag == Cyc_Tcenv_EnumRes_tag){
_LL419: _temp418=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp394)->f1; goto _LL417; _LL417: _temp416=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp394)->f2; goto _LL403;} else{ goto _LL404;}
_LL404: if((( struct _tunion_struct*) _temp394)->tag == Cyc_Tcenv_VarRes_tag){
_LL421: _temp420=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp394)->f1; goto
_LL405;} else{ goto _LL395;} _LL397: { struct Cyc_List_List* new_ps= 0; for( 0;
_temp99 != 0; _temp99=({ struct Cyc_List_List* _temp422= _temp99; if( _temp422
== 0){ _throw( Null_Exception);} _temp422->tl;})){ new_ps=({ struct Cyc_List_List*
_temp423=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp423->hd=( void*)({ struct _tuple7* _temp424=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp424->f1= 0; _temp424->f2=( struct Cyc_Absyn_Pat*)({
struct Cyc_List_List* _temp425= _temp99; if( _temp425 == 0){ _throw(
Null_Exception);} _temp425->hd;}); _temp424;}); _temp423->tl= new_ps; _temp423;});}(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp426=(
struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp426[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp427; _temp427.tag= Cyc_Absyn_Struct_p_tag;
_temp427.f1= _temp406; _temp427.f2= 0; _temp427.f3= _temp101; _temp427.f4=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_ps);
_temp427;}); _temp426;})));{ struct _tuple5 _temp428= Cyc_Tcpat_tcPatRec( te, p,
topt, region_opt); _npop_handler( 0u); return _temp428;}} _LL399:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp429=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp429[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp430; _temp430.tag= Cyc_Absyn_Tunion_p_tag;
_temp430.f1= _temp408->name; _temp430.f2= 0; _temp430.f3= _temp101; _temp430.f4=
_temp99; _temp430.f5= _temp410; _temp430.f6= _temp408; _temp430;}); _temp429;})));{
struct _tuple5 _temp431= Cyc_Tcpat_tcPatRec( te, p, topt, region_opt);
_npop_handler( 0u); return _temp431;} _LL401:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_XTunion_p_struct* _temp432=( struct Cyc_Absyn_XTunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct)); _temp432[ 0]=({ struct
Cyc_Absyn_XTunion_p_struct _temp433; _temp433.tag= Cyc_Absyn_XTunion_p_tag;
_temp433.f1= _temp412->name; _temp433.f2= _temp101; _temp433.f3= _temp99;
_temp433.f4= _temp414; _temp433.f5= _temp412; _temp433;}); _temp432;})));{
struct _tuple5 _temp434= Cyc_Tcpat_tcPatRec( te, p, topt, region_opt);
_npop_handler( 0u); return _temp434;} _LL403: Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp435= Cyc_Absynpp_qvar2string( _temp103); xprintf("bad enum constructor %.*s in pattern",
_temp435.last_plus_one - _temp435.curr, _temp435.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL395; _LL405: goto _LL395; _LL395:;}; _pop_handler();} else{ struct
_xtunion_struct* _temp437= _temp393; _LL439: if( _temp437->tag == Cyc_Dict_Absent_tag){
goto _LL440;} else{ goto _LL441;} _LL441: goto _LL442; _LL440: goto _LL438;
_LL442:( void) _throw( _temp437); _LL438:;}}} Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp443= Cyc_Absynpp_qvar2string( _temp103); xprintf("%.*s is not a constructor in pattern",
_temp443.last_plus_one - _temp443.curr, _temp443.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL5; _LL39:{ struct _handler_cons _temp444; _push_handler(& _temp444);{
struct _xtunion_struct* _temp445=( struct _xtunion_struct*) setjmp( _temp444.handler);
if( ! _temp445){{ void* _temp446= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp109); struct Cyc_Absyn_Structdecl* _temp452; _LL448: if((( struct
_tunion_struct*) _temp446)->tag == Cyc_Tcenv_StructRes_tag){ _LL453: _temp452=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp446)->f1;
goto _LL449;} else{ goto _LL450;} _LL450: goto _LL451; _LL449:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp454=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp454[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp455; _temp455.tag= Cyc_Absyn_Struct_p_tag;
_temp455.f1= _temp452; _temp455.f2= 0; _temp455.f3= _temp107; _temp455.f4=
_temp105; _temp455;}); _temp454;})));{ struct _tuple5 _temp456= Cyc_Tcpat_tcPatRec(
te, p, topt, region_opt); _npop_handler( 0u); return _temp456;} _LL451: goto
_LL447; _LL447:;}; _pop_handler();} else{ struct _xtunion_struct* _temp458=
_temp445; _LL460: if( _temp458->tag == Cyc_Dict_Absent_tag){ goto _LL461;} else{
goto _LL462;} _LL462: goto _LL463; _LL461: goto _LL459; _LL463:( void) _throw(
_temp458); _LL459:;}}} Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp464=
Cyc_Absynpp_qvar2string( _temp109); xprintf("%.*s is not a struct name, but pattern has designators",
_temp464.last_plus_one - _temp464.curr, _temp464.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL5; _LL5:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt* _temp465=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp465->v=( void*) t;
_temp465;}); return({ struct _tuple5 _temp466; _temp466.f1= tv_result; _temp466.f2=
v_result; _temp466;});} struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt){ struct _tuple5 ans=
Cyc_Tcpat_tcPatRec( te, p, topt, region_opt);{ struct _RegionHandle _temp467=
_new_region(); struct _RegionHandle* r=& _temp467; struct _handler_cons _temp468;
_push_handler(& _temp468);{ struct _xtunion_struct* _temp469=( struct
_xtunion_struct*) setjmp( _temp468.handler); if( ! _temp469){ Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_string*(* f)(
struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Tcpat_get_name,
ans.f2), p->loc,( struct _tagged_string)({ char* _temp470=( char*)"pattern contains a repeated variable";
struct _tagged_string _temp471; _temp471.curr= _temp470; _temp471.base= _temp470;
_temp471.last_plus_one= _temp470 + 37; _temp471;})); _free_region(& _temp467);
_pop_handler();} else{ _free_region(& _temp467); _throw( _temp469);}}} return
ans;} void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p){ void* _temp472=( void*) p->r; struct Cyc_Absyn_Pat* _temp486; struct Cyc_Absyn_Tunionfield*
_temp488; struct Cyc_Absyn_Tuniondecl* _temp490; struct Cyc_List_List* _temp492;
struct Cyc_List_List* _temp494; struct Cyc_Core_Opt* _temp496; struct _tuple1*
_temp498; struct Cyc_Absyn_Tunionfield* _temp500; struct Cyc_Absyn_XTuniondecl*
_temp502; struct Cyc_List_List* _temp504; struct Cyc_List_List* _temp506; struct
_tuple1* _temp508; struct Cyc_List_List* _temp510; struct Cyc_List_List*
_temp512; struct Cyc_Core_Opt* _temp514; struct Cyc_Absyn_Structdecl* _temp516;
struct Cyc_List_List* _temp518; _LL474: if(( unsigned int) _temp472 > 2u?((
struct _tunion_struct*) _temp472)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL487:
_temp486=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*) _temp472)->f1;
goto _LL475;} else{ goto _LL476;} _LL476: if(( unsigned int) _temp472 > 2u?((
struct _tunion_struct*) _temp472)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL499:
_temp498=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp472)->f1;
goto _LL497; _LL497: _temp496=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp472)->f2; goto _LL495; _LL495: _temp494=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp472)->f3; goto _LL493; _LL493: _temp492=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp472)->f4; goto
_LL491; _LL491: _temp490=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp472)->f5; goto _LL489; _LL489: _temp488=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp472)->f6; goto _LL477;} else{ goto
_LL478;} _LL478: if(( unsigned int) _temp472 > 2u?(( struct _tunion_struct*)
_temp472)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL509: _temp508=( struct _tuple1*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp472)->f1; goto _LL507; _LL507: _temp506=(
struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp472)->f2; goto
_LL505; _LL505: _temp504=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp472)->f3; goto _LL503; _LL503: _temp502=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp472)->f4; goto _LL501; _LL501: _temp500=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp472)->f5;
goto _LL479;} else{ goto _LL480;} _LL480: if(( unsigned int) _temp472 > 2u?((
struct _tunion_struct*) _temp472)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL517:
_temp516=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp472)->f1; goto _LL515; _LL515: _temp514=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp472)->f2; goto _LL513; _LL513: _temp512=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp472)->f3; goto
_LL511; _LL511: _temp510=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp472)->f4; goto _LL481;} else{ goto _LL482;} _LL482: if(( unsigned int)
_temp472 > 2u?(( struct _tunion_struct*) _temp472)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL519: _temp518=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp472)->f1; goto _LL483;} else{ goto _LL484;} _LL484: goto _LL485; _LL475:
Cyc_Tcpat_check_pat_regions( te, _temp486);{ void* _temp521=( void*)({ struct
Cyc_Core_Opt* _temp520= p->topt; if( _temp520 == 0){ _throw( Null_Exception);}
_temp520->v;}); struct Cyc_Absyn_PtrInfo _temp531; struct Cyc_Absyn_Conref*
_temp533; struct Cyc_Absyn_Tqual* _temp535; struct Cyc_Absyn_Conref* _temp537;
void* _temp539; void* _temp541; struct Cyc_Absyn_TunionInfo _temp543; struct Cyc_Absyn_Tuniondecl*
_temp545; void* _temp547; struct Cyc_List_List* _temp549; struct _tuple1*
_temp551; struct Cyc_Absyn_XTunionInfo _temp553; struct Cyc_Absyn_XTuniondecl*
_temp555; void* _temp557; struct _tuple1* _temp559; _LL523: if(( unsigned int)
_temp521 > 4u?(( struct _tunion_struct*) _temp521)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL532: _temp531=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp521)->f1; _LL542: _temp541=( void*) _temp531.elt_typ; goto _LL540; _LL540:
_temp539=( void*) _temp531.rgn_typ; goto _LL538; _LL538: _temp537=( struct Cyc_Absyn_Conref*)
_temp531.nullable; goto _LL536; _LL536: _temp535=( struct Cyc_Absyn_Tqual*)
_temp531.tq; goto _LL534; _LL534: _temp533=( struct Cyc_Absyn_Conref*) _temp531.bounds;
goto _LL524;} else{ goto _LL525;} _LL525: if(( unsigned int) _temp521 > 4u?((
struct _tunion_struct*) _temp521)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL544:
_temp543=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp521)->f1; _LL552: _temp551=( struct _tuple1*) _temp543.name; goto _LL550;
_LL550: _temp549=( struct Cyc_List_List*) _temp543.targs; goto _LL548; _LL548:
_temp547=( void*) _temp543.rgn; goto _LL546; _LL546: _temp545=( struct Cyc_Absyn_Tuniondecl*)
_temp543.tud; goto _LL526;} else{ goto _LL527;} _LL527: if(( unsigned int)
_temp521 > 4u?(( struct _tunion_struct*) _temp521)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL554: _temp553=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp521)->f1; _LL560: _temp559=( struct _tuple1*) _temp553.name; goto _LL558;
_LL558: _temp557=( void*) _temp553.rgn; goto _LL556; _LL556: _temp555=( struct
Cyc_Absyn_XTuniondecl*) _temp553.xtud; goto _LL528;} else{ goto _LL529;} _LL529:
goto _LL530; _LL524: _temp547= _temp539; goto _LL526; _LL526: _temp557= _temp547;
goto _LL528; _LL528: Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp557);
return; _LL530:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp561=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp561[ 0]=({ struct Cyc_Core_Impossible_struct _temp562; _temp562.tag= Cyc_Core_Impossible_tag;
_temp562.f1=( struct _tagged_string)({ char* _temp563=( char*)"check_pat_regions: bad pointer type";
struct _tagged_string _temp564; _temp564.curr= _temp563; _temp564.base= _temp563;
_temp564.last_plus_one= _temp563 + 36; _temp564;}); _temp562;}); _temp561;}));
_LL522:;} _LL477: for( 0; _temp492 != 0; _temp492=({ struct Cyc_List_List*
_temp565= _temp492; if( _temp565 == 0){ _throw( Null_Exception);} _temp565->tl;})){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp566= _temp492; if( _temp566 == 0){ _throw( Null_Exception);} _temp566->hd;}));}{
void* _temp568=( void*)({ struct Cyc_Core_Opt* _temp567= p->topt; if( _temp567
== 0){ _throw( Null_Exception);} _temp567->v;}); struct Cyc_Absyn_TunionInfo
_temp576; struct Cyc_Absyn_Tuniondecl* _temp578; void* _temp580; struct Cyc_List_List*
_temp582; struct _tuple1* _temp584; struct Cyc_Absyn_TunionFieldInfo _temp586;
_LL570: if(( unsigned int) _temp568 > 4u?(( struct _tunion_struct*) _temp568)->tag
== Cyc_Absyn_TunionType_tag: 0){ _LL577: _temp576=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp568)->f1; _LL585: _temp584=( struct
_tuple1*) _temp576.name; goto _LL583; _LL583: _temp582=( struct Cyc_List_List*)
_temp576.targs; goto _LL581; _LL581: _temp580=( void*) _temp576.rgn; goto _LL579;
_LL579: _temp578=( struct Cyc_Absyn_Tuniondecl*) _temp576.tud; goto _LL571;}
else{ goto _LL572;} _LL572: if(( unsigned int) _temp568 > 4u?(( struct
_tunion_struct*) _temp568)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL587:
_temp586=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp568)->f1; goto _LL573;} else{ goto _LL574;} _LL574: goto _LL575; _LL571:
Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp580); return; _LL573: return;
_LL575:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp588=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp588[ 0]=({ struct Cyc_Core_Impossible_struct _temp589; _temp589.tag= Cyc_Core_Impossible_tag;
_temp589.f1=( struct _tagged_string)({ char* _temp590=( char*)"check_pat_regions: bad tunion type";
struct _tagged_string _temp591; _temp591.curr= _temp590; _temp591.base= _temp590;
_temp591.last_plus_one= _temp590 + 35; _temp591;}); _temp589;}); _temp588;}));
_LL569:;} _LL479: for( 0; _temp504 != 0; _temp504=({ struct Cyc_List_List*
_temp592= _temp504; if( _temp592 == 0){ _throw( Null_Exception);} _temp592->tl;})){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp593= _temp504; if( _temp593 == 0){ _throw( Null_Exception);} _temp593->hd;}));}{
void* _temp595=( void*)({ struct Cyc_Core_Opt* _temp594= p->topt; if( _temp594
== 0){ _throw( Null_Exception);} _temp594->v;}); struct Cyc_Absyn_XTunionInfo
_temp603; struct Cyc_Absyn_XTuniondecl* _temp605; void* _temp607; struct _tuple1*
_temp609; struct Cyc_Absyn_XTunionFieldInfo _temp611; _LL597: if(( unsigned int)
_temp595 > 4u?(( struct _tunion_struct*) _temp595)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL604: _temp603=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp595)->f1; _LL610: _temp609=( struct _tuple1*) _temp603.name; goto _LL608;
_LL608: _temp607=( void*) _temp603.rgn; goto _LL606; _LL606: _temp605=( struct
Cyc_Absyn_XTuniondecl*) _temp603.xtud; goto _LL598;} else{ goto _LL599;} _LL599:
if(( unsigned int) _temp595 > 4u?(( struct _tunion_struct*) _temp595)->tag ==
Cyc_Absyn_XTunionFieldType_tag: 0){ _LL612: _temp611=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp595)->f1; goto _LL600;} else{
goto _LL601;} _LL601: goto _LL602; _LL598: Cyc_Tcenv_check_rgn_accessible( te, p->loc,
_temp607); return; _LL600: return; _LL602:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp613=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp613[ 0]=({ struct
Cyc_Core_Impossible_struct _temp614; _temp614.tag= Cyc_Core_Impossible_tag;
_temp614.f1=( struct _tagged_string)({ char* _temp615=( char*)"check_pat_regions: bad xtunion type";
struct _tagged_string _temp616; _temp616.curr= _temp615; _temp616.base= _temp615;
_temp616.last_plus_one= _temp615 + 36; _temp616;}); _temp614;}); _temp613;}));
_LL596:;} _LL481: _temp518=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple7*)) Cyc_Core_snd, _temp510); goto _LL483; _LL483: for( 0; _temp518
!= 0; _temp518=({ struct Cyc_List_List* _temp617= _temp518; if( _temp617 == 0){
_throw( Null_Exception);} _temp617->tl;})){ Cyc_Tcpat_check_pat_regions( te,(
struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp618= _temp518; if( _temp618
== 0){ _throw( Null_Exception);} _temp618->hd;}));} return; _LL485: return;
_LL473:;} typedef void* Cyc_Tcpat_Name_value; static const int Cyc_Tcpat_Name_v_tag=
0; struct Cyc_Tcpat_Name_v_struct{ int tag; struct _tagged_string f1; } ; static
const int Cyc_Tcpat_Int_v_tag= 1; struct Cyc_Tcpat_Int_v_struct{ int tag; int f1;
} ; typedef void* Cyc_Tcpat_name_value_t; struct Cyc_Tcpat_Con_s{ void* name;
int arity; struct Cyc_Core_Opt* span; } ; typedef struct Cyc_Tcpat_Con_s* Cyc_Tcpat_con_t;
typedef void* Cyc_Tcpat_Simple_pat; static const unsigned int Cyc_Tcpat_Any= 0;
static const int Cyc_Tcpat_Con_tag= 0; struct Cyc_Tcpat_Con_struct{ int tag;
struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; typedef void* Cyc_Tcpat_simple_pat_t;
static int Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s*
c2){ void* _temp619=( void*) c1->name; struct _tagged_string _temp625; int
_temp627; _LL621: if((( struct _tunion_struct*) _temp619)->tag == Cyc_Tcpat_Name_v_tag){
_LL626: _temp625=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp619)->f1; goto _LL622;} else{ goto _LL623;} _LL623: if((( struct
_tunion_struct*) _temp619)->tag == Cyc_Tcpat_Int_v_tag){ _LL628: _temp627=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp619)->f1; goto _LL624;} else{ goto _LL620;}
_LL622: { void* _temp629=( void*) c2->name; struct _tagged_string _temp635; int
_temp637; _LL631: if((( struct _tunion_struct*) _temp629)->tag == Cyc_Tcpat_Name_v_tag){
_LL636: _temp635=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp629)->f1; goto _LL632;} else{ goto _LL633;} _LL633: if((( struct
_tunion_struct*) _temp629)->tag == Cyc_Tcpat_Int_v_tag){ _LL638: _temp637=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp629)->f1; goto _LL634;} else{ goto _LL630;}
_LL632: return Cyc_String_zstrcmp( _temp625, _temp635); _LL634: return - 1;
_LL630:;} _LL624: { void* _temp639=( void*) c2->name; struct _tagged_string
_temp645; int _temp647; _LL641: if((( struct _tunion_struct*) _temp639)->tag ==
Cyc_Tcpat_Name_v_tag){ _LL646: _temp645=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp639)->f1; goto _LL642;} else{ goto _LL643;} _LL643: if((( struct
_tunion_struct*) _temp639)->tag == Cyc_Tcpat_Int_v_tag){ _LL648: _temp647=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp639)->f1; goto _LL644;} else{ goto _LL640;}
_LL642: return 1; _LL644: return _temp627 - _temp647; _LL640:;} _LL620:;} static
struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct Cyc_Set_Set*(*)(
int(* comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*))) Cyc_Set_empty)(
Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt=( struct
Cyc_Core_Opt){.v=( void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt=(
struct Cyc_Core_Opt){.v=( void*) 2}; static struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static char _temp652[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={ 0u,( struct
_tagged_string){ _temp652, _temp652, _temp652 + 5u}}; static char _temp656[ 4u]="ptr";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={ 0u,( struct
_tagged_string){ _temp656, _temp656, _temp656 + 4u}}; static char _temp660[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,( struct
_tagged_string){ _temp660, _temp660, _temp660 + 6u}}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_null_name_value),.arity=
0,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt}; static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=&
Cyc_Tcpat_null_ptr_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp661=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp661->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp662=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp662[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp663; _temp663.tag= Cyc_Tcpat_Int_v_tag;
_temp663.f1= i; _temp663;}); _temp662;})); _temp661->arity= 0; _temp661->span= 0;
_temp661;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_string f){ return({ struct Cyc_Tcpat_Con_s* _temp664=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp664->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp665=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp665[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp666; _temp666.tag= Cyc_Tcpat_Name_v_tag; _temp666.f1= f; _temp666;});
_temp665;})); _temp664->arity= 0; _temp664->span= 0; _temp664;});} static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_char_con( char c){ return({ struct Cyc_Tcpat_Con_s*
_temp667=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp667->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp669=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp669[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp670; _temp670.tag= Cyc_Tcpat_Int_v_tag;
_temp670.f1=( int) c; _temp670;}); _temp669;})); _temp667->arity= 0; _temp667->span=({
struct Cyc_Core_Opt* _temp668=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp668->v=( void*) 256; _temp668;}); _temp667;});}
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp671=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp671->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp671->arity=
i; _temp671->span= Cyc_Tcpat_one_opt_ptr; _temp671;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp672=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp672[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp673; _temp673.tag= Cyc_Tcpat_Con_tag; _temp673.f1= Cyc_Tcpat_null_con;
_temp673.f2= 0; _temp673;}); _temp672;});} static void* Cyc_Tcpat_int_pat( int i){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp674=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp674[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp675; _temp675.tag= Cyc_Tcpat_Con_tag; _temp675.f1= Cyc_Tcpat_int_con( i);
_temp675.f2= 0; _temp675;}); _temp674;});} static void* Cyc_Tcpat_char_pat( char
c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp676=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp676[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp677; _temp677.tag= Cyc_Tcpat_Con_tag; _temp677.f1= Cyc_Tcpat_char_con( c);
_temp677.f2= 0; _temp677;}); _temp676;});} static void* Cyc_Tcpat_float_pat(
struct _tagged_string f){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp678=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp678[ 0]=({ struct Cyc_Tcpat_Con_struct _temp679; _temp679.tag= Cyc_Tcpat_Con_tag;
_temp679.f1= Cyc_Tcpat_float_con( f); _temp679.f2= 0; _temp679;}); _temp678;});}
static void* Cyc_Tcpat_null_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp680=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp680[ 0]=({ struct Cyc_Tcpat_Con_struct _temp681; _temp681.tag= Cyc_Tcpat_Con_tag;
_temp681.f1= Cyc_Tcpat_null_ptr_con; _temp681.f2=({ struct Cyc_List_List*
_temp682=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp682->hd=( void*) p; _temp682->tl= 0; _temp682;}); _temp681;}); _temp680;});}
static void* Cyc_Tcpat_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp683=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp683[ 0]=({ struct Cyc_Tcpat_Con_struct _temp684; _temp684.tag= Cyc_Tcpat_Con_tag;
_temp684.f1= Cyc_Tcpat_ptr_con; _temp684.f2=({ struct Cyc_List_List* _temp685=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp685->hd=(
void*) p; _temp685->tl= 0; _temp685;}); _temp684;}); _temp683;});} static void*
Cyc_Tcpat_tuple_pat( struct Cyc_List_List* ss){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp686=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp686[ 0]=({ struct Cyc_Tcpat_Con_struct _temp687; _temp687.tag= Cyc_Tcpat_Con_tag;
_temp687.f1= Cyc_Tcpat_tuple_con((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ss)); _temp687.f2= ss; _temp687;}); _temp686;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_string con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List*
ps){ struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp690=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp690->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp691=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp691[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp692; _temp692.tag= Cyc_Tcpat_Name_v_tag; _temp692.f1= con_name; _temp692;});
_temp691;})); _temp690->arity=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ps); _temp690->span= span; _temp690;}); return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp688=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp688[ 0]=({ struct Cyc_Tcpat_Con_struct _temp689; _temp689.tag= Cyc_Tcpat_Con_tag;
_temp689.f1= c; _temp689.f2= ps; _temp689;}); _temp688;});} static void* Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat* p){ void* s;{ void* _temp693=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp727; int _temp729; void* _temp731; char _temp733; struct _tagged_string
_temp735; struct Cyc_List_List* _temp737; struct Cyc_Absyn_Vardecl* _temp739;
struct Cyc_Absyn_Pat* _temp741; struct Cyc_List_List* _temp743; struct Cyc_List_List*
_temp745; struct Cyc_Core_Opt* _temp747; struct Cyc_Absyn_Structdecl* _temp749;
struct Cyc_Absyn_Tunionfield* _temp751; struct Cyc_Absyn_Tuniondecl* _temp753;
struct Cyc_List_List* _temp755; struct Cyc_List_List* _temp757; struct Cyc_Core_Opt*
_temp759; struct _tuple1* _temp761; struct Cyc_Absyn_Tunionfield* _temp763;
struct Cyc_Absyn_XTuniondecl* _temp765; struct Cyc_List_List* _temp767; struct
Cyc_List_List* _temp769; struct _tuple1* _temp771; struct Cyc_Absyn_Enumfield*
_temp773; struct Cyc_Absyn_Enumdecl* _temp775; struct _tuple1* _temp777; struct
_tuple1* _temp779; struct Cyc_List_List* _temp781; struct Cyc_List_List*
_temp783; struct _tuple1* _temp785; struct Cyc_List_List* _temp787; struct Cyc_List_List*
_temp789; struct _tuple1* _temp791; _LL695: if(( int) _temp693 == Cyc_Absyn_Wild_p){
goto _LL696;} else{ goto _LL697;} _LL697: if(( unsigned int) _temp693 > 2u?((
struct _tunion_struct*) _temp693)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL728:
_temp727=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*) _temp693)->f1;
goto _LL698;} else{ goto _LL699;} _LL699: if(( int) _temp693 == Cyc_Absyn_Null_p){
goto _LL700;} else{ goto _LL701;} _LL701: if(( unsigned int) _temp693 > 2u?((
struct _tunion_struct*) _temp693)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL732:
_temp731=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp693)->f1; goto _LL730;
_LL730: _temp729=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp693)->f2; goto
_LL702;} else{ goto _LL703;} _LL703: if(( unsigned int) _temp693 > 2u?(( struct
_tunion_struct*) _temp693)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL734: _temp733=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp693)->f1; goto _LL704;} else{ goto
_LL705;} _LL705: if(( unsigned int) _temp693 > 2u?(( struct _tunion_struct*)
_temp693)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL736: _temp735=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp693)->f1; goto _LL706;}
else{ goto _LL707;} _LL707: if(( unsigned int) _temp693 > 2u?(( struct
_tunion_struct*) _temp693)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL738: _temp737=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp693)->f1; goto
_LL708;} else{ goto _LL709;} _LL709: if(( unsigned int) _temp693 > 2u?(( struct
_tunion_struct*) _temp693)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL740:
_temp739=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp693)->f1; goto _LL710;} else{ goto _LL711;} _LL711: if(( unsigned int)
_temp693 > 2u?(( struct _tunion_struct*) _temp693)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL742: _temp741=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp693)->f1; goto _LL712;} else{ goto _LL713;} _LL713: if(( unsigned int)
_temp693 > 2u?(( struct _tunion_struct*) _temp693)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL750: _temp749=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp693)->f1; goto _LL748; _LL748: _temp747=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp693)->f2; goto _LL746; _LL746: _temp745=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp693)->f3; goto
_LL744; _LL744: _temp743=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp693)->f4; goto _LL714;} else{ goto _LL715;} _LL715: if(( unsigned int)
_temp693 > 2u?(( struct _tunion_struct*) _temp693)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL762: _temp761=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp693)->f1; goto _LL760; _LL760: _temp759=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp693)->f2; goto _LL758; _LL758: _temp757=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp693)->f3; goto
_LL756; _LL756: _temp755=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp693)->f4; goto _LL754; _LL754: _temp753=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp693)->f5; goto _LL752; _LL752: _temp751=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp693)->f6;
goto _LL716;} else{ goto _LL717;} _LL717: if(( unsigned int) _temp693 > 2u?((
struct _tunion_struct*) _temp693)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL772:
_temp771=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp693)->f1;
goto _LL770; _LL770: _temp769=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp693)->f2; goto _LL768; _LL768: _temp767=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_p_struct*) _temp693)->f3; goto _LL766; _LL766: _temp765=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp693)->f4;
goto _LL764; _LL764: _temp763=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp693)->f5; goto _LL718;} else{ goto _LL719;} _LL719: if(( unsigned int)
_temp693 > 2u?(( struct _tunion_struct*) _temp693)->tag == Cyc_Absyn_Enum_p_tag:
0){ _LL778: _temp777=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp693)->f1; goto _LL776; _LL776: _temp775=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp693)->f2; goto _LL774; _LL774: _temp773=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*) _temp693)->f3;
goto _LL720;} else{ goto _LL721;} _LL721: if(( unsigned int) _temp693 > 2u?((
struct _tunion_struct*) _temp693)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL780:
_temp779=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp693)->f1;
goto _LL722;} else{ goto _LL723;} _LL723: if(( unsigned int) _temp693 > 2u?((
struct _tunion_struct*) _temp693)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){
_LL786: _temp785=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp693)->f1; goto _LL784; _LL784: _temp783=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp693)->f2; goto _LL782; _LL782: _temp781=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp693)->f3;
goto _LL724;} else{ goto _LL725;} _LL725: if(( unsigned int) _temp693 > 2u?((
struct _tunion_struct*) _temp693)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){
_LL792: _temp791=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp693)->f1; goto _LL790; _LL790: _temp789=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp693)->f2; goto _LL788; _LL788: _temp787=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp693)->f3;
goto _LL726;} else{ goto _LL694;} _LL696: s=( void*) Cyc_Tcpat_Any; goto _LL694;
_LL698: s=( void*) Cyc_Tcpat_Any; goto _LL694; _LL700: s= Cyc_Tcpat_null_pat();
goto _LL694; _LL702: s= Cyc_Tcpat_int_pat( _temp729); goto _LL694; _LL704: s=
Cyc_Tcpat_char_pat( _temp733); goto _LL694; _LL706: s= Cyc_Tcpat_float_pat(
_temp735); goto _LL694; _LL708: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp737)); goto _LL694; _LL710: s=( void*) Cyc_Tcpat_Any; goto _LL694; _LL712: {
void* ss= Cyc_Tcpat_compile_pat( _temp741);{ void* _temp794= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp793= p->topt; if( _temp793 == 0){ _throw(
Null_Exception);} _temp793->v;})); struct Cyc_Absyn_PtrInfo _temp804; struct Cyc_Absyn_Conref*
_temp806; struct Cyc_Absyn_Tqual* _temp808; struct Cyc_Absyn_Conref* _temp810;
void* _temp812; void* _temp814; struct Cyc_Absyn_TunionInfo _temp816; struct Cyc_Absyn_XTunionInfo
_temp818; _LL796: if(( unsigned int) _temp794 > 4u?(( struct _tunion_struct*)
_temp794)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL805: _temp804=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp794)->f1; _LL815: _temp814=( void*)
_temp804.elt_typ; goto _LL813; _LL813: _temp812=( void*) _temp804.rgn_typ; goto
_LL811; _LL811: _temp810=( struct Cyc_Absyn_Conref*) _temp804.nullable; goto
_LL809; _LL809: _temp808=( struct Cyc_Absyn_Tqual*) _temp804.tq; goto _LL807;
_LL807: _temp806=( struct Cyc_Absyn_Conref*) _temp804.bounds; goto _LL797;}
else{ goto _LL798;} _LL798: if(( unsigned int) _temp794 > 4u?(( struct
_tunion_struct*) _temp794)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL817:
_temp816=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp794)->f1; goto _LL799;} else{ goto _LL800;} _LL800: if(( unsigned int)
_temp794 > 4u?(( struct _tunion_struct*) _temp794)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL819: _temp818=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp794)->f1; goto _LL801;} else{ goto _LL802;} _LL802: goto _LL803; _LL797: {
int is_nullable= 0; int still_working= 1; while( still_working) { void* _temp820=(
void*) _temp810->v; struct Cyc_Absyn_Conref* _temp828; int _temp830; _LL822: if((
unsigned int) _temp820 > 1u?(( struct _tunion_struct*) _temp820)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL829: _temp828=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp820)->f1; goto _LL823;} else{ goto _LL824;} _LL824: if(( int) _temp820 ==
Cyc_Absyn_No_constr){ goto _LL825;} else{ goto _LL826;} _LL826: if((
unsigned int) _temp820 > 1u?(( struct _tunion_struct*) _temp820)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL831: _temp830=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp820)->f1;
goto _LL827;} else{ goto _LL821;} _LL823:( void*)( _temp810->v=( void*)(( void*)
_temp828->v)); continue; _LL825:( void*)( _temp810->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp832=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp832[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp833; _temp833.tag= Cyc_Absyn_Eq_constr_tag;
_temp833.f1=( void*) 0; _temp833;}); _temp832;}))); is_nullable= 0;
still_working= 0; goto _LL821; _LL827: is_nullable=( int) _temp830;
still_working= 0; goto _LL821; _LL821:;} if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL795;} _LL799: s= ss; goto _LL795;
_LL801: s= ss; goto _LL795; _LL803: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp834=( char*)"pointer pattern does not have pointer type!";
struct _tagged_string _temp835; _temp835.curr= _temp834; _temp835.base= _temp834;
_temp835.last_plus_one= _temp834 + 44; _temp835;})); goto _LL795; _LL795:;} goto
_LL694;} _LL714: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp868= _temp749->fields; if(
_temp868 == 0){ _throw( Null_Exception);} _temp868->v;}); for( 0; fields != 0;
fields=({ struct Cyc_List_List* _temp836= fields; if( _temp836 == 0){ _throw(
Null_Exception);} _temp836->tl;})){ int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp865= fields; if( _temp865 == 0){ _throw(
Null_Exception);} _temp865->hd;}))->name,( struct _tagged_string)({ char*
_temp866=( char*)""; struct _tagged_string _temp867; _temp867.curr= _temp866;
_temp867.base= _temp866; _temp867.last_plus_one= _temp866 + 1; _temp867;})) == 0;{
struct Cyc_List_List* dlps0= _temp743; for( 0; ! found? dlps0 != 0: 0; dlps0=({
struct Cyc_List_List* _temp837= dlps0; if( _temp837 == 0){ _throw(
Null_Exception);} _temp837->tl;})){ struct _tuple7 _temp841; struct Cyc_Absyn_Pat*
_temp842; struct Cyc_List_List* _temp844; struct _tuple7* _temp839=( struct
_tuple7*)({ struct Cyc_List_List* _temp838= dlps0; if( _temp838 == 0){ _throw(
Null_Exception);} _temp838->hd;}); _temp841=* _temp839; _LL845: _temp844=
_temp841.f1; goto _LL843; _LL843: _temp842= _temp841.f2; goto _LL840; _LL840: {
struct Cyc_List_List* _temp846= _temp844; struct Cyc_List_List _temp852; struct
Cyc_List_List* _temp853; void* _temp855; struct _tagged_string* _temp857; _LL848:
if( _temp846 == 0){ goto _LL850;} else{ _temp852=* _temp846; _LL856: _temp855=(
void*) _temp852.hd; if((( struct _tunion_struct*) _temp855)->tag == Cyc_Absyn_FieldName_tag){
_LL858: _temp857=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp855)->f1; goto _LL854;} else{ goto _LL850;} _LL854: _temp853=( struct Cyc_List_List*)
_temp852.tl; if( _temp853 == 0){ goto _LL849;} else{ goto _LL850;}} _LL850: goto
_LL851; _LL849: if( Cyc_String_zstrptrcmp( _temp857,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp859= fields; if( _temp859 == 0){ _throw(
Null_Exception);} _temp859->hd;}))->name) == 0){ ps=({ struct Cyc_List_List*
_temp860=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp860->hd=( void*) Cyc_Tcpat_compile_pat( _temp842); _temp860->tl= ps;
_temp860;}); found= 1;} goto _LL847; _LL851:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp861=( char*)"bad designator(s)";
struct _tagged_string _temp862; _temp862.curr= _temp861; _temp862.base= _temp861;
_temp862.last_plus_one= _temp861 + 18; _temp862;})); goto _LL847; _LL847:;}}}
if( ! found){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp863=( char*)"bad designator"; struct _tagged_string
_temp864; _temp864.curr= _temp863; _temp864.base= _temp863; _temp864.last_plus_one=
_temp863 + 15; _temp864;}));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL694;}
_LL716: { int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp870= _temp753->fields; if(
_temp870 == 0){ _throw( Null_Exception);} _temp870->v;})); s= Cyc_Tcpat_con_pat(*(*
_temp751->name).f2,({ struct Cyc_Core_Opt* _temp869=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp869->v=( void*) span;
_temp869;}),(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp755)); goto
_LL694;} _LL718: s= Cyc_Tcpat_con_pat(*(* _temp763->name).f2, 0,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp767)); goto _LL694; _LL720: { int span=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp775->fields); s= Cyc_Tcpat_con_pat(*(* _temp773->name).f2,({
struct Cyc_Core_Opt* _temp871=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp871->v=( void*) span; _temp871;}), 0); goto _LL694;}
_LL722: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp872=( char*)"compile_pat: unknownid"; struct
_tagged_string _temp873; _temp873.curr= _temp872; _temp873.base= _temp872;
_temp873.last_plus_one= _temp872 + 23; _temp873;})); goto _LL694; _LL724: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp874=( char*)"compile_pat: unknowncall"; struct _tagged_string
_temp875; _temp875.curr= _temp874; _temp875.base= _temp874; _temp875.last_plus_one=
_temp874 + 25; _temp875;})); goto _LL694; _LL726: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp876=(
char*)"compile_pat: unknownfields"; struct _tagged_string _temp877; _temp877.curr=
_temp876; _temp877.base= _temp876; _temp877.last_plus_one= _temp876 + 27;
_temp877;})); goto _LL694; _LL694:;} return s;} typedef void* Cyc_Tcpat_Term_desc;
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
struct Cyc_Tcpat_Con_s* c){ void* _temp878= td; struct Cyc_Set_Set* _temp884;
struct Cyc_List_List* _temp886; struct Cyc_Tcpat_Con_s* _temp888; _LL880: if(((
struct _tunion_struct*) _temp878)->tag == Cyc_Tcpat_Neg_tag){ _LL885: _temp884=(
struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp878)->f1; goto _LL881;}
else{ goto _LL882;} _LL882: if((( struct _tunion_struct*) _temp878)->tag == Cyc_Tcpat_Pos_tag){
_LL889: _temp888=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*)
_temp878)->f1; goto _LL887; _LL887: _temp886=( struct Cyc_List_List*)(( struct
Cyc_Tcpat_Pos_struct*) _temp878)->f2; goto _LL883;} else{ goto _LL879;} _LL881:
if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp884, c)){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp890=( char*)"add_neg called when constructor already in set";
struct _tagged_string _temp891; _temp891.curr= _temp890; _temp891.base= _temp890;
_temp891.last_plus_one= _temp890 + 47; _temp891;}));} if( c->span != 0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp884) + 1 >=( int)({ struct
Cyc_Core_Opt* _temp892= c->span; if( _temp892 == 0){ _throw( Null_Exception);}
_temp892->v;}): 0){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp893=( char*)"add_neg called when |cs U {c}| >= span(c)";
struct _tagged_string _temp894; _temp894.curr= _temp893; _temp894.base= _temp893;
_temp894.last_plus_one= _temp893 + 42; _temp894;}));} return( void*)({ struct
Cyc_Tcpat_Neg_struct* _temp895=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp895[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp896; _temp896.tag= Cyc_Tcpat_Neg_tag; _temp896.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp884,
c); _temp896;}); _temp895;}); _LL883: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp897=( char*)"add_neg called when td is Positive";
struct _tagged_string _temp898; _temp898.curr= _temp897; _temp898.base= _temp897;
_temp898.last_plus_one= _temp897 + 35; _temp898;})); _LL879:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp899= td; struct Cyc_List_List*
_temp905; struct Cyc_Tcpat_Con_s* _temp907; struct Cyc_Set_Set* _temp909; _LL901:
if((( struct _tunion_struct*) _temp899)->tag == Cyc_Tcpat_Pos_tag){ _LL908:
_temp907=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp899)->f1;
goto _LL906; _LL906: _temp905=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp899)->f2; goto _LL902;} else{ goto _LL903;} _LL903: if((( struct
_tunion_struct*) _temp899)->tag == Cyc_Tcpat_Neg_tag){ _LL910: _temp909=( struct
Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp899)->f1; goto _LL904;} else{
goto _LL900;} _LL902: if( Cyc_Tcpat_compare_con( c, _temp907) == 0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL904: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp909,
c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)({ struct Cyc_Core_Opt*
_temp911= c->span; if( _temp911 == 0){ _throw( Null_Exception);} _temp911->v;})
==(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp909) + 1: 0){
return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}} _LL900:;}
static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt, void*
dsc){ struct Cyc_List_List* _temp912= ctxt; struct Cyc_List_List _temp918;
struct Cyc_List_List* _temp919; struct _tuple10* _temp921; struct _tuple10
_temp923; struct Cyc_List_List* _temp924; struct Cyc_Tcpat_Con_s* _temp926;
_LL914: if( _temp912 == 0){ goto _LL915;} else{ goto _LL916;} _LL916: if(
_temp912 == 0){ goto _LL913;} else{ _temp918=* _temp912; _LL922: _temp921=(
struct _tuple10*) _temp918.hd; _temp923=* _temp921; _LL927: _temp926= _temp923.f1;
goto _LL925; _LL925: _temp924= _temp923.f2; goto _LL920; _LL920: _temp919=(
struct Cyc_List_List*) _temp918.tl; goto _LL917;} _LL915: return 0; _LL917:
return({ struct Cyc_List_List* _temp928=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp928->hd=( void*)({ struct _tuple10*
_temp929=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp929->f1=
_temp926; _temp929->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp930=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp930->hd=(
void*) dsc; _temp930->tl= _temp924; _temp930;}); _temp929;}); _temp928->tl=
_temp919; _temp928;}); _LL913:;} static struct Cyc_List_List* Cyc_Tcpat_norm_context(
struct Cyc_List_List* ctxt){ struct Cyc_List_List* _temp931= ctxt; struct Cyc_List_List
_temp937; struct Cyc_List_List* _temp938; struct _tuple10* _temp940; struct
_tuple10 _temp942; struct Cyc_List_List* _temp943; struct Cyc_Tcpat_Con_s*
_temp945; _LL933: if( _temp931 == 0){ goto _LL934;} else{ goto _LL935;} _LL935:
if( _temp931 == 0){ goto _LL932;} else{ _temp937=* _temp931; _LL941: _temp940=(
struct _tuple10*) _temp937.hd; _temp942=* _temp940; _LL946: _temp945= _temp942.f1;
goto _LL944; _LL944: _temp943= _temp942.f2; goto _LL939; _LL939: _temp938=(
struct Cyc_List_List*) _temp937.tl; goto _LL936;} _LL934: return(( struct Cyc_List_List*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp947=( char*)"norm_context: empty context"; struct _tagged_string _temp948;
_temp948.curr= _temp947; _temp948.base= _temp947; _temp948.last_plus_one=
_temp947 + 28; _temp948;})); _LL936: return Cyc_Tcpat_augment( _temp938,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp949=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp949[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp950; _temp950.tag= Cyc_Tcpat_Pos_tag; _temp950.f1= _temp945; _temp950.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp943);
_temp950;}); _temp949;})); _LL932:;} static void* Cyc_Tcpat_build_desc( struct
Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple5
_temp952=({ struct _tuple5 _temp951; _temp951.f1= ctxt; _temp951.f2= work;
_temp951;}); struct Cyc_List_List* _temp962; struct Cyc_List_List* _temp964;
struct Cyc_List_List* _temp966; struct Cyc_List_List* _temp968; struct Cyc_List_List*
_temp970; struct Cyc_List_List* _temp972; struct Cyc_List_List* _temp974; struct
Cyc_List_List _temp976; struct Cyc_List_List* _temp977; struct _tuple9* _temp979;
struct _tuple9 _temp981; struct Cyc_List_List* _temp982; struct Cyc_List_List*
_temp984; struct Cyc_List_List* _temp986; struct Cyc_List_List* _temp988; struct
Cyc_List_List _temp990; struct Cyc_List_List* _temp991; struct _tuple10*
_temp993; struct _tuple10 _temp995; struct Cyc_List_List* _temp996; struct Cyc_Tcpat_Con_s*
_temp998; _LL954: _LL965: _temp964= _temp952.f1; if( _temp964 == 0){ goto _LL963;}
else{ goto _LL956;} _LL963: _temp962= _temp952.f2; if( _temp962 == 0){ goto
_LL955;} else{ goto _LL956;} _LL956: _LL969: _temp968= _temp952.f1; if( _temp968
== 0){ goto _LL967;} else{ goto _LL958;} _LL967: _temp966= _temp952.f2; goto
_LL957; _LL958: _LL973: _temp972= _temp952.f1; goto _LL971; _LL971: _temp970=
_temp952.f2; if( _temp970 == 0){ goto _LL959;} else{ goto _LL960;} _LL960:
_LL989: _temp988= _temp952.f1; if( _temp988 == 0){ goto _LL953;} else{ _temp990=*
_temp988; _LL994: _temp993=( struct _tuple10*) _temp990.hd; _temp995=* _temp993;
_LL999: _temp998= _temp995.f1; goto _LL997; _LL997: _temp996= _temp995.f2; goto
_LL992; _LL992: _temp991=( struct Cyc_List_List*) _temp990.tl; goto _LL975;}
_LL975: _temp974= _temp952.f2; if( _temp974 == 0){ goto _LL953;} else{ _temp976=*
_temp974; _LL980: _temp979=( struct _tuple9*) _temp976.hd; _temp981=* _temp979;
_LL987: _temp986= _temp981.f1; goto _LL985; _LL985: _temp984= _temp981.f2; goto
_LL983; _LL983: _temp982= _temp981.f3; goto _LL978; _LL978: _temp977=( struct
Cyc_List_List*) _temp976.tl; goto _LL961;} _LL955: return dsc; _LL957: return((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp1000=( char*)"build_desc: ctxt and work don't match"; struct
_tagged_string _temp1001; _temp1001.curr= _temp1000; _temp1001.base= _temp1000;
_temp1001.last_plus_one= _temp1000 + 38; _temp1001;})); _LL959: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1002=( char*)"build_desc: ctxt and work don't match"; struct _tagged_string
_temp1003; _temp1003.curr= _temp1002; _temp1003.base= _temp1002; _temp1003.last_plus_one=
_temp1002 + 38; _temp1003;})); _LL961: { struct Cyc_Tcpat_Pos_struct* td=({
struct Cyc_Tcpat_Pos_struct* _temp1004=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp1004[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp1005; _temp1005.tag= Cyc_Tcpat_Pos_tag; _temp1005.f1= _temp998; _temp1005.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp996),({
struct Cyc_List_List* _temp1006=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1006->hd=( void*) dsc; _temp1006->tl= _temp982;
_temp1006;})); _temp1005;}); _temp1004;}); return Cyc_Tcpat_build_desc( _temp991,(
void*) td, _temp977);} _LL953:;} static void* Cyc_Tcpat_match( void* p, void*
obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp1007=
allmrules; struct Cyc_List_List _temp1013; struct Cyc_List_List* _temp1014;
struct _tuple0* _temp1016; struct _tuple0 _temp1018; void* _temp1019; void*
_temp1021; _LL1009: if( _temp1007 == 0){ goto _LL1010;} else{ goto _LL1011;}
_LL1011: if( _temp1007 == 0){ goto _LL1008;} else{ _temp1013=* _temp1007;
_LL1017: _temp1016=( struct _tuple0*) _temp1013.hd; _temp1018=* _temp1016;
_LL1022: _temp1021= _temp1018.f1; goto _LL1020; _LL1020: _temp1019= _temp1018.f2;
goto _LL1015; _LL1015: _temp1014=( struct Cyc_List_List*) _temp1013.tl; goto
_LL1012;} _LL1010: return( void*) Cyc_Tcpat_Failure; _LL1012: return(( void*(*)(
void* p, void* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)(
_temp1021,( void*) Cyc_Tcpat_Obj, dsc, 0, 0, _temp1019, _temp1014); _LL1008:;}
static void* Cyc_Tcpat_match_compile( struct Cyc_List_List* allmrules){ return((
void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)((
void*)({ struct Cyc_Tcpat_Neg_struct* _temp1023=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp1023[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp1024; _temp1024.tag= Cyc_Tcpat_Neg_tag; _temp1024.f1= Cyc_Tcpat_empty_con_set();
_temp1024;}); _temp1023;}), allmrules);} static void* Cyc_Tcpat_and_match(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules){ struct Cyc_List_List* _temp1025= work; struct Cyc_List_List
_temp1033; struct Cyc_List_List* _temp1034; struct _tuple9* _temp1036; struct
_tuple9 _temp1038; struct Cyc_List_List* _temp1039; struct Cyc_List_List*
_temp1041; struct Cyc_List_List* _temp1043; struct Cyc_List_List _temp1045;
struct Cyc_List_List* _temp1046; struct _tuple9* _temp1048; struct _tuple9
_temp1050; struct Cyc_List_List* _temp1051; struct Cyc_List_List* _temp1053;
struct Cyc_List_List* _temp1055; _LL1027: if( _temp1025 == 0){ goto _LL1028;}
else{ goto _LL1029;} _LL1029: if( _temp1025 == 0){ goto _LL1031;} else{
_temp1033=* _temp1025; _LL1037: _temp1036=( struct _tuple9*) _temp1033.hd;
_temp1038=* _temp1036; _LL1044: _temp1043= _temp1038.f1; if( _temp1043 == 0){
goto _LL1042;} else{ goto _LL1031;} _LL1042: _temp1041= _temp1038.f2; if(
_temp1041 == 0){ goto _LL1040;} else{ goto _LL1031;} _LL1040: _temp1039=
_temp1038.f3; if( _temp1039 == 0){ goto _LL1035;} else{ goto _LL1031;} _LL1035:
_temp1034=( struct Cyc_List_List*) _temp1033.tl; goto _LL1030;} _LL1031: if(
_temp1025 == 0){ goto _LL1026;} else{ _temp1045=* _temp1025; _LL1049: _temp1048=(
struct _tuple9*) _temp1045.hd; _temp1050=* _temp1048; _LL1056: _temp1055=
_temp1050.f1; goto _LL1054; _LL1054: _temp1053= _temp1050.f2; goto _LL1052;
_LL1052: _temp1051= _temp1050.f3; goto _LL1047; _LL1047: _temp1046=( struct Cyc_List_List*)
_temp1045.tl; goto _LL1032;} _LL1028: return( void*)({ struct Cyc_Tcpat_Success_struct*
_temp1057=( struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp1057[ 0]=({ struct Cyc_Tcpat_Success_struct _temp1058; _temp1058.tag= Cyc_Tcpat_Success_tag;
_temp1058.f1=( void*) right_hand_side; _temp1058;}); _temp1057;}); _LL1030:
return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context(
ctx), _temp1034, right_hand_side, rules); _LL1032: if(( _temp1055 == 0? 1:
_temp1053 == 0)? 1: _temp1051 == 0){ return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1059=( char*)"tcpat:and_match: malformed work frame";
struct _tagged_string _temp1060; _temp1060.curr= _temp1059; _temp1060.base=
_temp1059; _temp1060.last_plus_one= _temp1059 + 38; _temp1060;}));}{ struct Cyc_List_List
_temp1064; struct Cyc_List_List* _temp1065; void* _temp1067; struct Cyc_List_List*
_temp1062=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1061= _temp1055;
if( _temp1061 == 0){ _throw( Null_Exception);} _temp1061;}); _temp1064=*
_temp1062; _LL1068: _temp1067=( void*) _temp1064.hd; goto _LL1066; _LL1066:
_temp1065=( struct Cyc_List_List*) _temp1064.tl; goto _LL1063; _LL1063: { struct
Cyc_List_List _temp1072; struct Cyc_List_List* _temp1073; void* _temp1075;
struct Cyc_List_List* _temp1070=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1069= _temp1053; if( _temp1069 == 0){ _throw( Null_Exception);} _temp1069;});
_temp1072=* _temp1070; _LL1076: _temp1075=( void*) _temp1072.hd; goto _LL1074;
_LL1074: _temp1073=( struct Cyc_List_List*) _temp1072.tl; goto _LL1071; _LL1071: {
struct Cyc_List_List _temp1080; struct Cyc_List_List* _temp1081; void* _temp1083;
struct Cyc_List_List* _temp1078=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1077= _temp1051; if( _temp1077 == 0){ _throw( Null_Exception);} _temp1077;});
_temp1080=* _temp1078; _LL1084: _temp1083=( void*) _temp1080.hd; goto _LL1082;
_LL1082: _temp1081=( struct Cyc_List_List*) _temp1080.tl; goto _LL1079; _LL1079: {
struct _tuple9* wf=({ struct _tuple9* _temp1086=( struct _tuple9*) GC_malloc(
sizeof( struct _tuple9)); _temp1086->f1= _temp1065; _temp1086->f2= _temp1073;
_temp1086->f3= _temp1081; _temp1086;}); return(( void*(*)( void* p, void* obj,
void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp1067,
_temp1075, _temp1083, ctx,({ struct Cyc_List_List* _temp1085=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1085->hd=( void*) wf; _temp1085->tl=
_temp1046; _temp1085;}), right_hand_side, rules);}}}} _LL1026:;} static struct
Cyc_List_List* Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon, void* dsc){
void* _temp1087= dsc; struct Cyc_Set_Set* _temp1093; struct Cyc_List_List*
_temp1095; struct Cyc_Tcpat_Con_s* _temp1097; _LL1089: if((( struct
_tunion_struct*) _temp1087)->tag == Cyc_Tcpat_Neg_tag){ _LL1094: _temp1093=(
struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp1087)->f1; goto
_LL1090;} else{ goto _LL1091;} _LL1091: if((( struct _tunion_struct*) _temp1087)->tag
== Cyc_Tcpat_Pos_tag){ _LL1098: _temp1097=( struct Cyc_Tcpat_Con_s*)(( struct
Cyc_Tcpat_Pos_struct*) _temp1087)->f1; goto _LL1096; _LL1096: _temp1095=( struct
Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*) _temp1087)->f2; goto _LL1092;}
else{ goto _LL1088;} _LL1090: { void* any=( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp1100=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp1100[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp1101; _temp1101.tag= Cyc_Tcpat_Neg_tag;
_temp1101.f1= Cyc_Tcpat_empty_con_set(); _temp1101;}); _temp1100;}); struct Cyc_List_List*
res= 0;{ int i= 0; for( 0; i < pcon->arity; ++ i){ res=({ struct Cyc_List_List*
_temp1099=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1099->hd=( void*) any; _temp1099->tl= res; _temp1099;});}} return res;}
_LL1092: return _temp1095; _LL1088:;} static void* Cyc_Tcpat_getoarg( void* obj,
int i){ return( void*)({ struct Cyc_Tcpat_Sel_struct* _temp1102=( struct Cyc_Tcpat_Sel_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Sel_struct)); _temp1102[ 0]=({ struct Cyc_Tcpat_Sel_struct
_temp1103; _temp1103.tag= Cyc_Tcpat_Sel_tag; _temp1103.f1= i + 1; _temp1103.f2=(
void*) obj; _temp1103;}); _temp1102;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs(
struct Cyc_Tcpat_Con_s* pcon, void* obj){ return(( struct Cyc_List_List*(*)( int
n, void*(* f)( void*, int), void* env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg,
obj);} static void* Cyc_Tcpat_match( void* p, void* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp1104= p; struct Cyc_List_List* _temp1110; struct Cyc_Tcpat_Con_s*
_temp1112; _LL1106: if(( int) _temp1104 == Cyc_Tcpat_Any){ goto _LL1107;} else{
goto _LL1108;} _LL1108: if(( unsigned int) _temp1104 > 1u?(( struct
_tunion_struct*) _temp1104)->tag == Cyc_Tcpat_Con_tag: 0){ _LL1113: _temp1112=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Con_struct*) _temp1104)->f1; goto
_LL1111; _LL1111: _temp1110=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Con_struct*)
_temp1104)->f2; goto _LL1109;} else{ goto _LL1105;} _LL1107: return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc),
work, right_hand_side, rules); _LL1109: { void* _temp1114= Cyc_Tcpat_static_match(
_temp1112, dsc); _LL1116: if(( int) _temp1114 == Cyc_Tcpat_Yes){ goto _LL1117;}
else{ goto _LL1118;} _LL1118: if(( int) _temp1114 == Cyc_Tcpat_No){ goto _LL1119;}
else{ goto _LL1120;} _LL1120: if(( int) _temp1114 == Cyc_Tcpat_Maybe){ goto
_LL1121;} else{ goto _LL1115;} _LL1117: { struct Cyc_List_List* ctx2=({ struct
Cyc_List_List* _temp1124=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1124->hd=( void*)({ struct _tuple10* _temp1125=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1125->f1= _temp1112; _temp1125->f2= 0;
_temp1125;}); _temp1124->tl= ctx; _temp1124;}); struct _tuple9* work_frame=({
struct _tuple9* _temp1123=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp1123->f1= _temp1110; _temp1123->f2= Cyc_Tcpat_getoargs( _temp1112, obj);
_temp1123->f3= Cyc_Tcpat_getdargs( _temp1112, dsc); _temp1123;}); struct Cyc_List_List*
work2=({ struct Cyc_List_List* _temp1122=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1122->hd=( void*) work_frame; _temp1122->tl=
work; _temp1122;}); return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules);} _LL1119: return(( void*(*)( void* dsc,
struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx,
dsc, work), rules); _LL1121: { struct Cyc_List_List* ctx2=({ struct Cyc_List_List*
_temp1130=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1130->hd=( void*)({ struct _tuple10* _temp1131=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1131->f1= _temp1112; _temp1131->f2= 0;
_temp1131;}); _temp1130->tl= ctx; _temp1130;}); struct _tuple9* work_frame=({
struct _tuple9* _temp1129=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp1129->f1= _temp1110; _temp1129->f2= Cyc_Tcpat_getoargs( _temp1112, obj);
_temp1129->f3= Cyc_Tcpat_getdargs( _temp1112, dsc); _temp1129;}); struct Cyc_List_List*
work2=({ struct Cyc_List_List* _temp1128=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1128->hd=( void*) work_frame; _temp1128->tl=
work; _temp1128;}); void* s=(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules); void* f=(( void*(*)( void* dsc, struct Cyc_List_List*
allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg(
dsc, _temp1112), work), rules); return( void*)({ struct Cyc_Tcpat_IfEq_struct*
_temp1126=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp1126[ 0]=({ struct Cyc_Tcpat_IfEq_struct _temp1127; _temp1127.tag= Cyc_Tcpat_IfEq_tag;
_temp1127.f1=( void*) obj; _temp1127.f2= _temp1112; _temp1127.f3=( void*) s;
_temp1127.f4=( void*) f; _temp1127;}); _temp1126;});} _LL1115:;} _LL1105:;}
static void Cyc_Tcpat_check_exhaust_overlap( void* d, void(* not_exhaust)( void*),
void* env1, void(* rhs_appears)( void*, void*), void* env2){ void* _temp1132= d;
void* _temp1140; void* _temp1142; void* _temp1144; struct Cyc_Tcpat_Con_s*
_temp1146; void* _temp1148; _LL1134: if(( int) _temp1132 == Cyc_Tcpat_Failure){
goto _LL1135;} else{ goto _LL1136;} _LL1136: if(( unsigned int) _temp1132 > 1u?((
struct _tunion_struct*) _temp1132)->tag == Cyc_Tcpat_Success_tag: 0){ _LL1141:
_temp1140=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1132)->f1; goto
_LL1137;} else{ goto _LL1138;} _LL1138: if(( unsigned int) _temp1132 > 1u?((
struct _tunion_struct*) _temp1132)->tag == Cyc_Tcpat_IfEq_tag: 0){ _LL1149:
_temp1148=( void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1132)->f1; goto _LL1147;
_LL1147: _temp1146=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1132)->f2; goto _LL1145; _LL1145: _temp1144=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1132)->f3; goto _LL1143; _LL1143: _temp1142=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1132)->f4; goto _LL1139;} else{ goto _LL1133;} _LL1135: not_exhaust( env1);
goto _LL1133; _LL1137: rhs_appears( env2, _temp1140); goto _LL1133; _LL1139:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1144,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1142, not_exhaust, env1, rhs_appears, env2); goto _LL1133; _LL1133:;}
static struct _tuple12* Cyc_Tcpat_get_match( int* r, struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({
struct _tuple11* _temp1155=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11)); _temp1155->f1= 0; _temp1155->f2=( swc->pattern)->loc; _temp1155;});
void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1150=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1150->hd=( void*) sp0; _temp1150->tl=({ struct Cyc_List_List* _temp1151=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1151->hd=(
void*) Cyc_Tcpat_int_pat(* r); _temp1151->tl= 0; _temp1151;}); _temp1150;}));* r=*
r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List* _temp1152=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1152->hd=(
void*) sp0; _temp1152->tl=({ struct Cyc_List_List* _temp1153=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1153->hd=( void*)(( void*) Cyc_Tcpat_Any);
_temp1153->tl= 0; _temp1153;}); _temp1152;}));} return({ struct _tuple12*
_temp1154=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1154->f1=
sp; _temp1154->f2= rhs; _temp1154;});} static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp1156=( char*)"patterns may not be exhaustive"; struct _tagged_string
_temp1157; _temp1157.curr= _temp1156; _temp1157.base= _temp1156; _temp1157.last_plus_one=
_temp1156 + 31; _temp1157;}));} static void Cyc_Tcpat_rule_occurs( int dummy,
struct _tuple11* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int where_ctr= 0;
struct Cyc_List_List* match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(*
f)( int*, struct Cyc_Absyn_Switch_clause*), int* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Tcpat_get_match,& where_ctr, swcs); void* dec_tree=(( void*(*)(
struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)( match_rules);(( void(*)(
void* d, void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple11*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
dec_tree, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
match_rules != 0; match_rules=({ struct Cyc_List_List* _temp1158= match_rules;
if( _temp1158 == 0){ _throw( Null_Exception);} _temp1158->tl;})){ struct
_tuple11 _temp1162; struct Cyc_Position_Segment* _temp1163; int _temp1165;
struct _tuple11* _temp1160=(*(( struct _tuple12*)({ struct Cyc_List_List*
_temp1159= match_rules; if( _temp1159 == 0){ _throw( Null_Exception);} _temp1159->hd;}))).f2;
_temp1162=* _temp1160; _LL1166: _temp1165= _temp1162.f1; goto _LL1164; _LL1164:
_temp1163= _temp1162.f2; goto _LL1161; _LL1161: if( ! _temp1165){ Cyc_Tcutil_terr(
_temp1163,( struct _tagged_string)({ char* _temp1167=( char*)"redundant pattern";
struct _tagged_string _temp1168; _temp1168.curr= _temp1167; _temp1168.base=
_temp1167; _temp1168.last_plus_one= _temp1167 + 18; _temp1168;}));}}} static
void Cyc_Tcpat_not_exhaust_warn( struct _tuple13* pr){(* pr).f2= 0; Cyc_Tcutil_warn((*
pr).f1,( struct _tagged_string)({ char* _temp1169=( char*)"pattern not exhaustive";
struct _tagged_string _temp1170; _temp1170.curr= _temp1169; _temp1170.base=
_temp1169; _temp1170.last_plus_one= _temp1169 + 23; _temp1170;}));} static void
Cyc_Tcpat_dummy_fn( int i, int j){ return;} int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
match_rules=({ struct Cyc_List_List* _temp1172=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1172->hd=( void*)({ struct
_tuple14* _temp1173=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp1173->f1= Cyc_Tcpat_compile_pat( p); _temp1173->f2= 0; _temp1173;});
_temp1172->tl= 0; _temp1172;}); void* dec_tree=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( match_rules); struct _tuple13* exhaust_env=({
struct _tuple13* _temp1171=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp1171->f1= loc; _temp1171->f2= 1; _temp1171;});(( void(*)( void*
d, void(* not_exhaust)( struct _tuple13*), struct _tuple13* env1, void(*
rhs_appears)( int, int), int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree,
Cyc_Tcpat_not_exhaust_warn, exhaust_env, Cyc_Tcpat_dummy_fn, 0); return(*
exhaust_env).f2;} static struct _tuple12* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({
struct _tuple11* _temp1175=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11)); _temp1175->f1= 0; _temp1175->f2=( swc->pattern)->loc; _temp1175;});
return({ struct _tuple12* _temp1174=( struct _tuple12*) GC_malloc( sizeof(
struct _tuple12)); _temp1174->f1= sp0; _temp1174->f2= rhs; _temp1174;});} static
void Cyc_Tcpat_not_exhaust_err2( struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple11*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=({ struct
Cyc_List_List* _temp1176= match_rules; if( _temp1176 == 0){ _throw(
Null_Exception);} _temp1176->tl;})){ struct _tuple11* pair=(*(( struct _tuple12*)({
struct Cyc_List_List* _temp1179= match_rules; if( _temp1179 == 0){ _throw(
Null_Exception);} _temp1179->hd;}))).f2; if( !(* pair).f1){ Cyc_Tcutil_terr((*
pair).f2,( struct _tagged_string)({ char* _temp1177=( char*)"redundant pattern";
struct _tagged_string _temp1178; _temp1178.curr= _temp1177; _temp1178.base=
_temp1177; _temp1178.last_plus_one= _temp1177 + 18; _temp1178;}));}}}