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
struct Cyc_Core_Opt* Cyc_Core_opt_t; struct _tuple0{ void* f1; void* f2; } ;
extern void* Cyc_Core_snd( struct _tuple0*); extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap( struct
_RegionHandle*, void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char* tag; } ; extern
struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(* f)( void*, int), void*
env); struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[
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
typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t; struct
_tuple1{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple1* Cyc_Absyn_qvar_t;
typedef struct _tuple1* Cyc_Absyn_qvar_opt_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_t;
typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
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
8; struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual
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
23; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
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
f3; } ; static const int Cyc_Absyn_While_s_tag= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s_tag=
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
struct Cyc_Absyn_Tunion_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_XTunion_p_tag= 9; struct Cyc_Absyn_XTunion_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_XTuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ; static
const int Cyc_Absyn_Enum_p_tag= 10; struct Cyc_Absyn_Enum_p_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
Cyc_Absyn_Vardecl{ void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual tq;
void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct
Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline;
struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect;
void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt*
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
char* tag; } ; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern
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
void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle*, struct Cyc_List_List*, void*); struct _tuple4{ struct Cyc_Absyn_Tvar*
f1; void* f2; } ; extern struct _tuple4* Cyc_Tcutil_r_make_inst_var( struct
_RegionHandle*, struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct
_tagged_string err_msg); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_Absyn_Structdecl* sd); struct _tuple5{ struct Cyc_List_List* f1;
struct Cyc_List_List* f2; } ; extern struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; static void Cyc_Tcpat_resolve_pat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p){ void* _temp0=( void*) p->r; struct _tuple1*
_temp10; struct Cyc_List_List* _temp12; struct Cyc_List_List* _temp14; struct
_tuple1* _temp16; struct Cyc_List_List* _temp18; struct Cyc_List_List* _temp20;
struct _tuple1* _temp22; _LL2: if(( unsigned int) _temp0 > 2u?(( struct
_tunion_struct*) _temp0)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL11: _temp10=(
struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp0)->f1; goto _LL3;}
else{ goto _LL4;} _LL4: if(( unsigned int) _temp0 > 2u?(( struct _tunion_struct*)
_temp0)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){ _LL17: _temp16=( struct _tuple1*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp0)->f1; goto _LL15; _LL15: _temp14=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp0)->f2;
goto _LL13; _LL13: _temp12=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f3; goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 2u?((
struct _tunion_struct*) _temp0)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){ _LL23:
_temp22=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp0)->f1;
goto _LL21; _LL21: _temp20=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f2; goto _LL19; _LL19: _temp18=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f3; goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9; _LL3:{ struct
_handler_cons _temp24; _push_handler(& _temp24);{ struct _xtunion_struct*
_temp25=( struct _xtunion_struct*) setjmp( _temp24.handler); if( ! _temp25){{
void* _temp26= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp10); struct Cyc_Absyn_Structdecl*
_temp38; struct Cyc_Absyn_Tunionfield* _temp40; struct Cyc_Absyn_Tuniondecl*
_temp42; struct Cyc_Absyn_Tunionfield* _temp44; struct Cyc_Absyn_XTuniondecl*
_temp46; struct Cyc_Absyn_Enumfield* _temp48; struct Cyc_Absyn_Enumdecl* _temp50;
void* _temp52; _LL28: if((( struct _tunion_struct*) _temp26)->tag == Cyc_Tcenv_StructRes_tag){
_LL39: _temp38=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*)
_temp26)->f1; goto _LL29;} else{ goto _LL30;} _LL30: if((( struct _tunion_struct*)
_temp26)->tag == Cyc_Tcenv_TunionRes_tag){ _LL43: _temp42=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp26)->f1; goto _LL41; _LL41: _temp40=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp26)->f2;
goto _LL31;} else{ goto _LL32;} _LL32: if((( struct _tunion_struct*) _temp26)->tag
== Cyc_Tcenv_XTunionRes_tag){ _LL47: _temp46=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp26)->f1; goto _LL45; _LL45: _temp44=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_XTunionRes_struct*) _temp26)->f2;
goto _LL33;} else{ goto _LL34;} _LL34: if((( struct _tunion_struct*) _temp26)->tag
== Cyc_Tcenv_EnumRes_tag){ _LL51: _temp50=( struct Cyc_Absyn_Enumdecl*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp26)->f1; goto _LL49; _LL49: _temp48=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp26)->f2; goto _LL35;} else{ goto _LL36;}
_LL36: if((( struct _tunion_struct*) _temp26)->tag == Cyc_Tcenv_VarRes_tag){
_LL53: _temp52=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp26)->f1; goto
_LL37;} else{ goto _LL27;} _LL29: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp54=( char*)"struct tag used without arguments in pattern";
struct _tagged_string _temp55; _temp55.curr= _temp54; _temp55.base= _temp54;
_temp55.last_plus_one= _temp54 + 45; _temp55;})); _npop_handler( 0u); return;
_LL31:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp56=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp56[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp57; _temp57.tag= Cyc_Absyn_Tunion_p_tag;
_temp57.f1= _temp40->name; _temp57.f2= 0; _temp57.f3= 0; _temp57.f4= _temp42;
_temp57.f5= _temp40; _temp57;}); _temp56;}))); _npop_handler( 0u); return; _LL33:(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_XTunion_p_struct* _temp58=(
struct Cyc_Absyn_XTunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct));
_temp58[ 0]=({ struct Cyc_Absyn_XTunion_p_struct _temp59; _temp59.tag= Cyc_Absyn_XTunion_p_tag;
_temp59.f1= _temp44->name; _temp59.f2= 0; _temp59.f3= 0; _temp59.f4= _temp46;
_temp59.f5= _temp44; _temp59;}); _temp58;}))); _npop_handler( 0u); return; _LL35:(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_p_struct* _temp60=(
struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp60[ 0]=({ struct Cyc_Absyn_Enum_p_struct _temp61; _temp61.tag= Cyc_Absyn_Enum_p_tag;
_temp61.f1= _temp48->name; _temp61.f2= _temp50; _temp61.f3= _temp48; _temp61;});
_temp60;}))); _npop_handler( 0u); return; _LL37: goto _LL27; _LL27:;};
_pop_handler();} else{ struct _xtunion_struct* _temp63= _temp25; _LL65: if(
_temp63->tag == Cyc_Dict_Absent_tag){ goto _LL66;} else{ goto _LL67;} _LL67:
goto _LL68; _LL66: goto _LL64; _LL68:( void) _throw( _temp63); _LL64:;}}}{ void*
_temp69=(* _temp10).f1; struct Cyc_List_List* _temp77; _LL71: if(( int) _temp69
== Cyc_Absyn_Loc_n){ goto _LL72;} else{ goto _LL73;} _LL73: if(( unsigned int)
_temp69 > 1u?(( struct _tunion_struct*) _temp69)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL78: _temp77=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp69)->f1; if( _temp77 == 0){ goto _LL74;} else{ goto _LL75;}} else{ goto
_LL75;} _LL75: goto _LL76; _LL72: goto _LL74; _LL74:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp79=( struct
Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp79[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp80; _temp80.tag= Cyc_Absyn_Var_p_tag;
_temp80.f1= Cyc_Absyn_new_vardecl( _temp10,( void*) Cyc_Absyn_VoidType, 0);
_temp80;}); _temp79;}))); return; _LL76: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp81=( char*)"qualified variable in pattern"; struct
_tagged_string _temp82; _temp82.curr= _temp81; _temp82.base= _temp81; _temp82.last_plus_one=
_temp81 + 30; _temp82;})); return; _LL70:;} _LL5:{ struct _handler_cons _temp83;
_push_handler(& _temp83);{ struct _xtunion_struct* _temp84=( struct
_xtunion_struct*) setjmp( _temp83.handler); if( ! _temp84){{ void* _temp85= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp16); struct Cyc_Absyn_Structdecl* _temp97; struct Cyc_Absyn_Tunionfield*
_temp99; struct Cyc_Absyn_Tuniondecl* _temp101; struct Cyc_Absyn_Tunionfield*
_temp103; struct Cyc_Absyn_XTuniondecl* _temp105; struct Cyc_Absyn_Enumfield*
_temp107; struct Cyc_Absyn_Enumdecl* _temp109; void* _temp111; _LL87: if(((
struct _tunion_struct*) _temp85)->tag == Cyc_Tcenv_StructRes_tag){ _LL98:
_temp97=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*)
_temp85)->f1; goto _LL88;} else{ goto _LL89;} _LL89: if((( struct _tunion_struct*)
_temp85)->tag == Cyc_Tcenv_TunionRes_tag){ _LL102: _temp101=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp85)->f1; goto _LL100; _LL100: _temp99=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp85)->f2;
goto _LL90;} else{ goto _LL91;} _LL91: if((( struct _tunion_struct*) _temp85)->tag
== Cyc_Tcenv_XTunionRes_tag){ _LL106: _temp105=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp85)->f1; goto _LL104; _LL104: _temp103=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_XTunionRes_struct*) _temp85)->f2;
goto _LL92;} else{ goto _LL93;} _LL93: if((( struct _tunion_struct*) _temp85)->tag
== Cyc_Tcenv_EnumRes_tag){ _LL110: _temp109=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp85)->f1; goto _LL108; _LL108: _temp107=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp85)->f2;
goto _LL94;} else{ goto _LL95;} _LL95: if((( struct _tunion_struct*) _temp85)->tag
== Cyc_Tcenv_VarRes_tag){ _LL112: _temp111=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp85)->f1; goto _LL96;} else{ goto _LL86;} _LL88: { struct Cyc_List_List*
new_ps= 0; for( 0; _temp12 != 0; _temp12=({ struct Cyc_List_List* _temp113=
_temp12; if( _temp113 == 0){ _throw( Null_Exception);} _temp113->tl;})){ new_ps=({
struct Cyc_List_List* _temp114=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp114->hd=( void*)({ struct _tuple6* _temp115=(
struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp115->f1= 0; _temp115->f2=(
struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp116= _temp12; if( _temp116
== 0){ _throw( Null_Exception);} _temp116->hd;}); _temp115;}); _temp114->tl=
new_ps; _temp114;});}( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct*
_temp117=( struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp117[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp118; _temp118.tag= Cyc_Absyn_Struct_p_tag;
_temp118.f1= _temp97; _temp118.f2= 0; _temp118.f3= _temp14; _temp118.f4=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_ps);
_temp118;}); _temp117;}))); _npop_handler( 0u); return;} _LL90:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp119=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp119[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp120; _temp120.tag= Cyc_Absyn_Tunion_p_tag;
_temp120.f1= _temp99->name; _temp120.f2= _temp14; _temp120.f3= _temp12; _temp120.f4=
_temp101; _temp120.f5= _temp99; _temp120;}); _temp119;}))); _npop_handler( 0u);
return; _LL92:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_XTunion_p_struct*
_temp121=( struct Cyc_Absyn_XTunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct));
_temp121[ 0]=({ struct Cyc_Absyn_XTunion_p_struct _temp122; _temp122.tag= Cyc_Absyn_XTunion_p_tag;
_temp122.f1= _temp103->name; _temp122.f2= _temp14; _temp122.f3= _temp12;
_temp122.f4= _temp105; _temp122.f5= _temp103; _temp122;}); _temp121;})));
_npop_handler( 0u); return; _LL94: Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp123= Cyc_Absynpp_qvar2string( _temp16); xprintf("bad enum constructor %.*s in pattern",
_temp123.last_plus_one - _temp123.curr, _temp123.curr);})); _npop_handler( 0u);
return; _LL96: goto _LL86; _LL86:;}; _pop_handler();} else{ struct
_xtunion_struct* _temp125= _temp84; _LL127: if( _temp125->tag == Cyc_Dict_Absent_tag){
goto _LL128;} else{ goto _LL129;} _LL129: goto _LL130; _LL128: goto _LL126;
_LL130:( void) _throw( _temp125); _LL126:;}}} Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp131= Cyc_Absynpp_qvar2string( _temp16); xprintf("%.*s is not a constructor in pattern",
_temp131.last_plus_one - _temp131.curr, _temp131.curr);})); return; _LL7:{
struct _handler_cons _temp132; _push_handler(& _temp132);{ struct
_xtunion_struct* _temp133=( struct _xtunion_struct*) setjmp( _temp132.handler);
if( ! _temp133){{ void* _temp134= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp22);
struct Cyc_Absyn_Structdecl* _temp140; _LL136: if((( struct _tunion_struct*)
_temp134)->tag == Cyc_Tcenv_StructRes_tag){ _LL141: _temp140=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp134)->f1; goto _LL137;} else{ goto
_LL138;} _LL138: goto _LL139; _LL137:( void*)( p->r=( void*)(( void*)({ struct
Cyc_Absyn_Struct_p_struct* _temp142=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp142[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp143; _temp143.tag= Cyc_Absyn_Struct_p_tag;
_temp143.f1= _temp140; _temp143.f2= 0; _temp143.f3= _temp20; _temp143.f4=
_temp18; _temp143;}); _temp142;}))); _npop_handler( 0u); return; _LL139: goto
_LL135; _LL135:;}; _pop_handler();} else{ struct _xtunion_struct* _temp145=
_temp133; _LL147: if( _temp145->tag == Cyc_Dict_Absent_tag){ goto _LL148;} else{
goto _LL149;} _LL149: goto _LL150; _LL148: goto _LL146; _LL150:( void) _throw(
_temp145); _LL146:;}}} Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp151=
Cyc_Absynpp_qvar2string( _temp22); xprintf("%.*s is not a struct name, but pattern has designators",
_temp151.last_plus_one - _temp151.curr, _temp151.curr);})); return; _LL9:
return; _LL1:;} static struct _tagged_string* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl*
vd){ return(* vd->name).f2;} static void* Cyc_Tcpat_any_type( void** topt){ if(
topt != 0){ return({ void** _temp152= topt; if( _temp152 == 0){ _throw(
Null_Exception);}* _temp152;});} return Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);}
static void* Cyc_Tcpat_num_type( void** topt, void* numt){ if( topt != 0? Cyc_Tcutil_coerceable(({
void** _temp153= topt; if( _temp153 == 0){ _throw( Null_Exception);}* _temp153;})):
0){ return({ void** _temp154= topt; if( _temp154 == 0){ _throw( Null_Exception);}*
_temp154;});} return numt;} static void Cyc_Tcpat_set_vd( struct Cyc_Absyn_Vardecl*
vd, struct Cyc_List_List** v_result_ptr, void* t){( void*)( vd->type=( void*) t);
vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=({ struct Cyc_List_List* _temp155=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp155->hd=(
void*) vd; _temp155->tl=* v_result_ptr; _temp155;});} struct _tuple7{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; struct _tuple8{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat*
f2; } ; struct _tuple5 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t;
struct Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void*
_temp156=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp192; struct Cyc_Absyn_Vardecl*
_temp194; int _temp196; void* _temp198; int _temp200; void* _temp202; char
_temp204; struct _tagged_string _temp206; struct Cyc_Absyn_Enumfield* _temp208;
struct Cyc_Absyn_Enumdecl* _temp210; struct _tuple1* _temp212; struct Cyc_Absyn_Pat*
_temp214; struct Cyc_List_List* _temp216; struct Cyc_List_List* _temp218; struct
Cyc_List_List* _temp220; struct Cyc_Core_Opt* _temp222; struct Cyc_Core_Opt**
_temp224; struct Cyc_Absyn_Structdecl* _temp225; struct Cyc_Absyn_Tunionfield*
_temp227; struct Cyc_Absyn_Tuniondecl* _temp229; struct Cyc_List_List* _temp231;
struct Cyc_List_List* _temp233; struct _tuple1* _temp235; struct Cyc_Absyn_Tunionfield*
_temp237; struct Cyc_Absyn_XTuniondecl* _temp239; struct Cyc_List_List* _temp241;
struct Cyc_List_List* _temp243; struct _tuple1* _temp245; struct _tuple1*
_temp247; struct Cyc_List_List* _temp249; struct Cyc_List_List* _temp251; struct
_tuple1* _temp253; struct Cyc_List_List* _temp255; struct Cyc_List_List*
_temp257; struct _tuple1* _temp259; _LL158: if(( int) _temp156 == Cyc_Absyn_Wild_p){
goto _LL159;} else{ goto _LL160;} _LL160: if(( unsigned int) _temp156 > 2u?((
struct _tunion_struct*) _temp156)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL193:
_temp192=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*) _temp156)->f1;
goto _LL161;} else{ goto _LL162;} _LL162: if(( unsigned int) _temp156 > 2u?((
struct _tunion_struct*) _temp156)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL195:
_temp194=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp156)->f1; goto _LL163;} else{ goto _LL164;} _LL164: if(( unsigned int)
_temp156 > 2u?(( struct _tunion_struct*) _temp156)->tag == Cyc_Absyn_Int_p_tag:
0){ _LL199: _temp198=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp156)->f1;
if(( int) _temp198 == Cyc_Absyn_Unsigned){ goto _LL197;} else{ goto _LL166;}
_LL197: _temp196=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp156)->f2; goto
_LL165;} else{ goto _LL166;} _LL166: if(( unsigned int) _temp156 > 2u?(( struct
_tunion_struct*) _temp156)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL203: _temp202=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp156)->f1; if(( int) _temp202 ==
Cyc_Absyn_Signed){ goto _LL201;} else{ goto _LL168;} _LL201: _temp200=( int)((
struct Cyc_Absyn_Int_p_struct*) _temp156)->f2; goto _LL167;} else{ goto _LL168;}
_LL168: if(( unsigned int) _temp156 > 2u?(( struct _tunion_struct*) _temp156)->tag
== Cyc_Absyn_Char_p_tag: 0){ _LL205: _temp204=( char)(( struct Cyc_Absyn_Char_p_struct*)
_temp156)->f1; goto _LL169;} else{ goto _LL170;} _LL170: if(( unsigned int)
_temp156 > 2u?(( struct _tunion_struct*) _temp156)->tag == Cyc_Absyn_Float_p_tag:
0){ _LL207: _temp206=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*)
_temp156)->f1; goto _LL171;} else{ goto _LL172;} _LL172: if(( unsigned int)
_temp156 > 2u?(( struct _tunion_struct*) _temp156)->tag == Cyc_Absyn_Enum_p_tag:
0){ _LL213: _temp212=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp156)->f1; goto _LL211; _LL211: _temp210=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp156)->f2; goto _LL209; _LL209: _temp208=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*) _temp156)->f3;
goto _LL173;} else{ goto _LL174;} _LL174: if(( int) _temp156 == Cyc_Absyn_Null_p){
goto _LL175;} else{ goto _LL176;} _LL176: if(( unsigned int) _temp156 > 2u?((
struct _tunion_struct*) _temp156)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL215:
_temp214=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*) _temp156)->f1;
goto _LL177;} else{ goto _LL178;} _LL178: if(( unsigned int) _temp156 > 2u?((
struct _tunion_struct*) _temp156)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL217:
_temp216=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp156)->f1;
goto _LL179;} else{ goto _LL180;} _LL180: if(( unsigned int) _temp156 > 2u?((
struct _tunion_struct*) _temp156)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL226:
_temp225=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp156)->f1; goto _LL223; _LL223: _temp222=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp156)->f2; _temp224=&(( struct Cyc_Absyn_Struct_p_struct*)
_temp156)->f2; goto _LL221; _LL221: _temp220=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp156)->f3; goto _LL219; _LL219: _temp218=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp156)->f4; goto
_LL181;} else{ goto _LL182;} _LL182: if(( unsigned int) _temp156 > 2u?(( struct
_tunion_struct*) _temp156)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL236: _temp235=(
struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp156)->f1; goto _LL234;
_LL234: _temp233=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp156)->f2; goto _LL232; _LL232: _temp231=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp156)->f3; goto _LL230; _LL230: _temp229=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp156)->f4;
goto _LL228; _LL228: _temp227=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp156)->f5; goto _LL183;} else{ goto _LL184;} _LL184: if(( unsigned int)
_temp156 > 2u?(( struct _tunion_struct*) _temp156)->tag == Cyc_Absyn_XTunion_p_tag:
0){ _LL246: _temp245=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp156)->f1; goto _LL244; _LL244: _temp243=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_p_struct*) _temp156)->f2; goto _LL242; _LL242: _temp241=(
struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp156)->f3; goto
_LL240; _LL240: _temp239=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp156)->f4; goto _LL238; _LL238: _temp237=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp156)->f5; goto _LL185;} else{ goto
_LL186;} _LL186: if(( unsigned int) _temp156 > 2u?(( struct _tunion_struct*)
_temp156)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL248: _temp247=( struct
_tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp156)->f1; goto _LL187;}
else{ goto _LL188;} _LL188: if(( unsigned int) _temp156 > 2u?(( struct
_tunion_struct*) _temp156)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){ _LL254:
_temp253=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp156)->f1;
goto _LL252; _LL252: _temp251=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp156)->f2; goto _LL250; _LL250: _temp249=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp156)->f3; goto _LL189;} else{ goto _LL190;}
_LL190: if(( unsigned int) _temp156 > 2u?(( struct _tunion_struct*) _temp156)->tag
== Cyc_Absyn_UnknownFields_p_tag: 0){ _LL260: _temp259=( struct _tuple1*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp156)->f1; goto _LL258; _LL258:
_temp257=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp156)->f2; goto _LL256; _LL256: _temp255=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp156)->f3; goto _LL191;} else{ goto
_LL157;} _LL159: t= Cyc_Tcpat_any_type( topt); goto _LL157; _LL161: t= Cyc_Tcpat_any_type(
topt); Cyc_Tcpat_set_vd( _temp192,& v_result, t); goto _LL157; _LL163: t= Cyc_Tcpat_any_type(
topt); if( region_opt == 0){ Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({
char* _temp261=( char*)"* pattern would point into an unknown region"; struct
_tagged_string _temp262; _temp262.curr= _temp261; _temp262.base= _temp261;
_temp262.last_plus_one= _temp261 + 45; _temp262;})); goto _LL157;} Cyc_Tcpat_set_vd(
_temp194,& v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp263=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp263[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp264; _temp264.tag= Cyc_Absyn_PointerType_tag;
_temp264.f1=({ struct Cyc_Absyn_PtrInfo _temp265; _temp265.elt_typ=( void*) t;
_temp265.rgn_typ=( void*)({ void** _temp266= region_opt; if( _temp266 == 0){
_throw( Null_Exception);}* _temp266;}); _temp265.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp265.tq= Cyc_Absyn_empty_tqual();
_temp265.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp265;}); _temp264;}); _temp263;})); goto _LL157; _LL165: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_uint_t); goto _LL157; _LL167: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t);
goto _LL157; _LL169: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto
_LL157; _LL171: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_float_t); goto _LL157;
_LL173: t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp267=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp267[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp268; _temp268.tag= Cyc_Absyn_EnumType_tag;
_temp268.f1= _temp210->name; _temp268.f2=( struct Cyc_Absyn_Enumdecl*) _temp210;
_temp268;}); _temp267;}); goto _LL157; _LL175: if( topt != 0){ void* _temp270=
Cyc_Tcutil_compress(({ void** _temp269= topt; if( _temp269 == 0){ _throw(
Null_Exception);}* _temp269;})); struct Cyc_Absyn_PtrInfo _temp276; _LL272: if((
unsigned int) _temp270 > 4u?(( struct _tunion_struct*) _temp270)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL277: _temp276=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp270)->f1; goto _LL273;} else{ goto _LL274;} _LL274: goto _LL275; _LL273: t=({
void** _temp278= topt; if( _temp278 == 0){ _throw( Null_Exception);}* _temp278;});
goto tcpat_end; _LL275: goto _LL271; _LL271:;} t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp279=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp279[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp280; _temp280.tag= Cyc_Absyn_PointerType_tag;
_temp280.f1=({ struct Cyc_Absyn_PtrInfo _temp281; _temp281.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp281.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp281.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp281.tq= Cyc_Absyn_empty_tqual(); _temp281.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp281;}); _temp280;});
_temp279;}); goto _LL157; _LL177: { void* inner_typ=( void*) Cyc_Absyn_VoidType;
void** inner_topt= 0; if( topt != 0){ void* _temp283= Cyc_Tcutil_compress(({
void** _temp282= topt; if( _temp282 == 0){ _throw( Null_Exception);}* _temp282;}));
struct Cyc_Absyn_PtrInfo _temp289; struct Cyc_Absyn_Conref* _temp291; struct Cyc_Absyn_Tqual
_temp293; struct Cyc_Absyn_Conref* _temp295; void* _temp297; void* _temp299;
_LL285: if(( unsigned int) _temp283 > 4u?(( struct _tunion_struct*) _temp283)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL290: _temp289=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp283)->f1; _LL300: _temp299=( void*)
_temp289.elt_typ; goto _LL298; _LL298: _temp297=( void*) _temp289.rgn_typ; goto
_LL296; _LL296: _temp295=( struct Cyc_Absyn_Conref*) _temp289.nullable; goto
_LL294; _LL294: _temp293=( struct Cyc_Absyn_Tqual) _temp289.tq; goto _LL292;
_LL292: _temp291=( struct Cyc_Absyn_Conref*) _temp289.bounds; goto _LL286;}
else{ goto _LL287;} _LL287: goto _LL288; _LL286: inner_typ= _temp299; inner_topt=(
void**)& inner_typ; goto _LL284; _LL288: goto _LL284; _LL284:;}{ void* ptr_rgn=
Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); struct Cyc_List_List* _temp303;
struct Cyc_List_List* _temp305; struct _tuple5 _temp301= Cyc_Tcpat_tcPatRec( te,
_temp214, inner_topt,( void**)& ptr_rgn); _LL306: _temp305= _temp301.f1; goto
_LL304; _LL304: _temp303= _temp301.f2; goto _LL302; _LL302: tv_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp305); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp303); if(
inner_topt == 0){ void* _temp308= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp307= _temp214->topt; if( _temp307 == 0){ _throw( Null_Exception);} _temp307->v;}));
struct Cyc_Absyn_TunionFieldInfo _temp316; struct Cyc_Absyn_Tunionfield*
_temp318; struct Cyc_Absyn_Tuniondecl* _temp320; struct _tuple1* _temp322;
struct Cyc_List_List* _temp324; struct _tuple1* _temp326; struct Cyc_Absyn_XTunionFieldInfo
_temp328; struct Cyc_Absyn_Tunionfield* _temp330; struct Cyc_Absyn_XTuniondecl*
_temp332; struct _tuple1* _temp334; struct _tuple1* _temp336; _LL310: if((
unsigned int) _temp308 > 4u?(( struct _tunion_struct*) _temp308)->tag == Cyc_Absyn_TunionFieldType_tag:
0){ _LL317: _temp316=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp308)->f1; _LL327: _temp326=( struct _tuple1*) _temp316.name; goto _LL325;
_LL325: _temp324=( struct Cyc_List_List*) _temp316.targs; goto _LL323; _LL323:
_temp322=( struct _tuple1*) _temp316.fname; goto _LL321; _LL321: _temp320=(
struct Cyc_Absyn_Tuniondecl*) _temp316.tud; goto _LL319; _LL319: _temp318=(
struct Cyc_Absyn_Tunionfield*) _temp316.tufd; goto _LL311;} else{ goto _LL312;}
_LL312: if(( unsigned int) _temp308 > 4u?(( struct _tunion_struct*) _temp308)->tag
== Cyc_Absyn_XTunionFieldType_tag: 0){ _LL329: _temp328=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp308)->f1; _LL337: _temp336=(
struct _tuple1*) _temp328.name; goto _LL335; _LL335: _temp334=( struct _tuple1*)
_temp328.fname; goto _LL333; _LL333: _temp332=( struct Cyc_Absyn_XTuniondecl*)
_temp328.xtud; goto _LL331; _LL331: _temp330=( struct Cyc_Absyn_Tunionfield*)
_temp328.xtufd; goto _LL313;} else{ goto _LL314;} _LL314: goto _LL315; _LL311: t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp338=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp338[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp339; _temp339.tag= Cyc_Absyn_TunionType_tag;
_temp339.f1=({ struct Cyc_Absyn_TunionInfo _temp340; _temp340.name= _temp326;
_temp340.targs= _temp324; _temp340.rgn=( void*) ptr_rgn; _temp340.tud= _temp320;
_temp340;}); _temp339;}); _temp338;}); goto _LL309; _LL313: t=( void*)({ struct
Cyc_Absyn_XTunionType_struct* _temp341=( struct Cyc_Absyn_XTunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct)); _temp341[ 0]=({ struct
Cyc_Absyn_XTunionType_struct _temp342; _temp342.tag= Cyc_Absyn_XTunionType_tag;
_temp342.f1=({ struct Cyc_Absyn_XTunionInfo _temp343; _temp343.name= _temp336;
_temp343.rgn=( void*) ptr_rgn; _temp343.xtud= _temp332; _temp343;}); _temp342;});
_temp341;}); goto _LL309; _LL315: t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp344=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp344[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp345; _temp345.tag= Cyc_Absyn_PointerType_tag;
_temp345.f1=({ struct Cyc_Absyn_PtrInfo _temp346; _temp346.elt_typ=( void*)((
void*)({ struct Cyc_Core_Opt* _temp347= _temp214->topt; if( _temp347 == 0){
_throw( Null_Exception);} _temp347->v;})); _temp346.rgn_typ=( void*) ptr_rgn;
_temp346.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp346.tq= Cyc_Absyn_empty_tqual(); _temp346.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp346;}); _temp345;}); _temp344;}); goto _LL309;
_LL309:;} else{ t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp348=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp348[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp349; _temp349.tag= Cyc_Absyn_PointerType_tag;
_temp349.f1=({ struct Cyc_Absyn_PtrInfo _temp350; _temp350.elt_typ=( void*)((
void*)({ struct Cyc_Core_Opt* _temp351= _temp214->topt; if( _temp351 == 0){
_throw( Null_Exception);} _temp351->v;})); _temp350.rgn_typ=( void*) ptr_rgn;
_temp350.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp350.tq= Cyc_Absyn_empty_tqual(); _temp350.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp350;}); _temp349;}); _temp348;});} goto _LL157;}}
_LL179: { struct Cyc_List_List* pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if(
topt != 0){ void* _temp353= Cyc_Tcutil_compress(({ void** _temp352= topt; if(
_temp352 == 0){ _throw( Null_Exception);}* _temp352;})); struct Cyc_List_List*
_temp359; _LL355: if(( unsigned int) _temp353 > 4u?(( struct _tunion_struct*)
_temp353)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL360: _temp359=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp353)->f1; goto _LL356;} else{ goto
_LL357;} _LL357: goto _LL358; _LL356: topt_ts= _temp359; goto _LL354; _LL358:
goto _LL354; _LL354:;} for( 0; _temp216 != 0; _temp216=({ struct Cyc_List_List*
_temp361= _temp216; if( _temp361 == 0){ _throw( Null_Exception);} _temp361->tl;})){
void** inner_topt= 0; if( topt_ts != 0){ inner_topt=( void**)&(*(( struct
_tuple7*)({ struct Cyc_List_List* _temp362= topt_ts; if( _temp362 == 0){ _throw(
Null_Exception);} _temp362->hd;}))).f2; topt_ts=({ struct Cyc_List_List*
_temp363= topt_ts; if( _temp363 == 0){ _throw( Null_Exception);} _temp363->tl;});}{
struct Cyc_List_List* _temp367; struct Cyc_List_List* _temp369; struct _tuple5
_temp365= Cyc_Tcpat_tcPatRec( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp364= _temp216; if( _temp364 == 0){ _throw( Null_Exception);} _temp364->hd;}),
inner_topt, region_opt); _LL370: _temp369= _temp365.f1; goto _LL368; _LL368:
_temp367= _temp365.f2; goto _LL366; _LL366: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp369); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp367); pat_ts=({ struct Cyc_List_List*
_temp371=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp371->hd=( void*)({ struct _tuple7* _temp372=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp372->f1= Cyc_Absyn_empty_tqual(); _temp372->f2=(
void*)({ struct Cyc_Core_Opt* _temp374=(( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp373= _temp216; if( _temp373 == 0){ _throw( Null_Exception);} _temp373->hd;}))->topt;
if( _temp374 == 0){ _throw( Null_Exception);} _temp374->v;}); _temp372;});
_temp371->tl= pat_ts; _temp371;});}} t=( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp375=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp375[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp376; _temp376.tag= Cyc_Absyn_TupleType_tag;
_temp376.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
pat_ts); _temp376;}); _temp375;}); goto _LL157;} _LL181: if( _temp220 != 0){(
void) _throw((( struct _xtunion_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp377=( char*)"tcPat: struct<...> not implemented";
struct _tagged_string _temp378; _temp378.curr= _temp377; _temp378.base= _temp377;
_temp378.last_plus_one= _temp377 + 35; _temp378;})));}{ struct _RegionHandle
_temp379= _new_region(); struct _RegionHandle* rgn=& _temp379; struct
_handler_cons _temp380; _push_handler(& _temp380);{ struct _xtunion_struct*
_temp381=( struct _xtunion_struct*) setjmp( _temp380.handler); if( ! _temp381){{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp225->tvs); struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple4*)) Cyc_Core_snd, inst); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp382=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp382[ 0]=({ struct Cyc_Absyn_StructType_struct _temp383; _temp383.tag= Cyc_Absyn_StructType_tag;
_temp383.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp384= _temp225->name; if( _temp384 == 0){ _throw( Null_Exception);} _temp384->v;}));
_temp383.f2= all_typs; _temp383.f3=({ struct Cyc_Absyn_Structdecl** _temp385=(
struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp385[ 0]= _temp225; _temp385;}); _temp383;}); _temp382;}); if( topt != 0){
Cyc_Tcutil_unify( t,({ void** _temp386= topt; if( _temp386 == 0){ _throw(
Null_Exception);}* _temp386;}));}* _temp224=({ struct Cyc_Core_Opt* _temp387=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp387->v=(
void*) all_typs; _temp387;});{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)(
rgn, p->loc, _temp218, _temp225); for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp388= fields; if( _temp388 == 0){ _throw( Null_Exception);} _temp388->tl;})){
struct _tuple8 _temp392; struct Cyc_Absyn_Pat* _temp393; struct Cyc_Absyn_Structfield*
_temp395; struct _tuple8* _temp390=( struct _tuple8*)({ struct Cyc_List_List*
_temp389= fields; if( _temp389 == 0){ _throw( Null_Exception);} _temp389->hd;});
_temp392=* _temp390; _LL396: _temp395= _temp392.f1; goto _LL394; _LL394:
_temp393= _temp392.f2; goto _LL391; _LL391: { void* inst_fieldtyp= Cyc_Tcutil_rsubstitute(
rgn, inst,( void*) _temp395->type); struct Cyc_List_List* _temp399; struct Cyc_List_List*
_temp401; struct _tuple5 _temp397= Cyc_Tcpat_tcPatRec( te, _temp393,( void**)&
inst_fieldtyp, region_opt); _LL402: _temp401= _temp397.f1; goto _LL400; _LL400:
_temp399= _temp397.f2; goto _LL398; _LL398: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp401); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp399); if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp403= _temp393->topt; if( _temp403 == 0){
_throw( Null_Exception);} _temp403->v;}), inst_fieldtyp)){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp405=* _temp395->name; struct _tagged_string _temp406=
Cyc_Absynpp_typ2string( inst_fieldtyp); struct _tagged_string _temp407= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp404= _temp393->topt; if( _temp404 == 0){
_throw( Null_Exception);} _temp404->v;})); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp405.last_plus_one - _temp405.curr, _temp405.curr, _temp406.last_plus_one -
_temp406.curr, _temp406.curr, _temp407.last_plus_one - _temp407.curr, _temp407.curr);}));}}}}}
_free_region(& _temp379); _pop_handler();} else{ _free_region(& _temp379);
_throw( _temp381);}}} goto _LL157; _LL183: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp233) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp227->tvs)){ Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char*
_temp408=( char*)"wrong number of existential type variables in pattern"; struct
_tagged_string _temp409; _temp409.curr= _temp408; _temp409.base= _temp408;
_temp409.last_plus_one= _temp408 + 54; _temp409;}));}{ struct _RegionHandle
_temp410= _new_region(); struct _RegionHandle* rgn=& _temp410; struct
_handler_cons _temp411; _push_handler(& _temp411);{ struct _xtunion_struct*
_temp412=( struct _xtunion_struct*) setjmp( _temp411.handler); if( ! _temp412){{
struct Cyc_List_List* var_tvs= 0;{ struct Cyc_List_List* t= _temp233; for( 0; t
!= 0; t=({ struct Cyc_List_List* _temp413= t; if( _temp413 == 0){ _throw(
Null_Exception);} _temp413->tl;})){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp440= t; if( _temp440 == 0){ _throw( Null_Exception);}
_temp440->hd;});{ void* _temp414=( void*)((( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp424;
void* _temp426; struct Cyc_Absyn_Conref* _temp428; _LL416: if(( unsigned int)
_temp414 > 1u?(( struct _tunion_struct*) _temp414)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL425: _temp424=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp414)->f1;
if(( int) _temp424 == Cyc_Absyn_BoxKind){ goto _LL417;} else{ goto _LL418;}}
else{ goto _LL418;} _LL418: if(( unsigned int) _temp414 > 1u?(( struct
_tunion_struct*) _temp414)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL427: _temp426=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp414)->f1; goto _LL419;} else{
goto _LL420;} _LL420: if(( int) _temp414 == Cyc_Absyn_No_constr){ goto _LL421;}
else{ goto _LL422;} _LL422: if(( unsigned int) _temp414 > 1u?(( struct
_tunion_struct*) _temp414)->tag == Cyc_Absyn_Forward_constr_tag: 0){ _LL429:
_temp428=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp414)->f1; goto _LL423;} else{ goto _LL415;} _LL417: goto _LL415; _LL419:
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp430= Cyc_Absynpp_qvar2string(
_temp235); struct _tagged_string _temp431=* tv->name; struct _tagged_string
_temp432= Cyc_Absynpp_kind2string( _temp426); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp430.last_plus_one - _temp430.curr, _temp430.curr, _temp431.last_plus_one -
_temp431.curr, _temp431.curr, _temp432.last_plus_one - _temp432.curr, _temp432.curr);}));
goto _LL415; _LL421:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp433=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp433[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp434; _temp434.tag= Cyc_Absyn_Eq_constr_tag;
_temp434.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp434;}); _temp433;})));
goto _LL415; _LL423:( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp435=(
char*)"forward after compress in conref"; struct _tagged_string _temp436;
_temp436.curr= _temp435; _temp436.base= _temp435; _temp436.last_plus_one=
_temp435 + 33; _temp436;}))); _LL415:;} var_tvs=({ struct Cyc_List_List*
_temp437=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp437->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp438=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp438[ 0]=({ struct Cyc_Absyn_VarType_struct _temp439; _temp439.tag= Cyc_Absyn_VarType_tag;
_temp439.f1= tv; _temp439;}); _temp438;})); _temp437->tl= var_tvs; _temp437;});}}
var_tvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
var_tvs); tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp233);{ struct Cyc_Tcenv_Tenv*
te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp233); struct Cyc_List_List* tqts=
_temp227->typs; struct Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp229->tvs); struct Cyc_List_List* exist_inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp227->tvs, var_tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst); struct Cyc_List_List* exist_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple4*)) Cyc_Core_snd, exist_inst); struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, all_inst, exist_inst); if( tqts == 0){
t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp441=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp441[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp442; _temp442.tag= Cyc_Absyn_TunionType_tag;
_temp442.f1=({ struct Cyc_Absyn_TunionInfo _temp443; _temp443.name=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp444= _temp229->name; if( _temp444 == 0){
_throw( Null_Exception);} _temp444->v;}); _temp443.targs= all_typs; _temp443.rgn=(
void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp443.tud=( struct Cyc_Absyn_Tuniondecl*)
_temp229; _temp443;}); _temp442;}); _temp441;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp445=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp445[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp446; _temp446.tag= Cyc_Absyn_TunionFieldType_tag; _temp446.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp447; _temp447.name=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp448=
_temp229->name; if( _temp448 == 0){ _throw( Null_Exception);} _temp448->v;});
_temp447.targs= all_typs; _temp447.fname= _temp235; _temp447.tud=( struct Cyc_Absyn_Tuniondecl*)
_temp229; _temp447.tufd=( struct Cyc_Absyn_Tunionfield*) _temp227; _temp447;});
_temp446;}); _temp445;});} if( topt != 0? tqts == 0: 0){ void* _temp450= Cyc_Tcutil_compress(({
void** _temp449= topt; if( _temp449 == 0){ _throw( Null_Exception);}* _temp449;}));
struct Cyc_Absyn_TunionFieldInfo _temp458; struct Cyc_Absyn_TunionInfo _temp460;
struct Cyc_Absyn_Tuniondecl* _temp462; void* _temp464; struct Cyc_List_List*
_temp466; struct _tuple1* _temp468; _LL452: if(( unsigned int) _temp450 > 4u?((
struct _tunion_struct*) _temp450)->tag == Cyc_Absyn_TunionFieldType_tag: 0){
_LL459: _temp458=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp450)->f1; goto _LL453;} else{ goto _LL454;} _LL454: if(( unsigned int)
_temp450 > 4u?(( struct _tunion_struct*) _temp450)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL461: _temp460=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp450)->f1; _LL469: _temp468=( struct _tuple1*) _temp460.name; goto _LL467;
_LL467: _temp466=( struct Cyc_List_List*) _temp460.targs; goto _LL465; _LL465:
_temp464=( void*) _temp460.rgn; goto _LL463; _LL463: _temp462=( struct Cyc_Absyn_Tuniondecl*)
_temp460.tud; goto _LL455;} else{ goto _LL456;} _LL456: goto _LL457; _LL453: t=(
void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp470=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp470[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp471; _temp471.tag= Cyc_Absyn_TunionFieldType_tag;
_temp471.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp472; _temp472.name=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp473= _temp229->name; if( _temp473 == 0){
_throw( Null_Exception);} _temp473->v;}); _temp472.targs= all_typs; _temp472.fname=
_temp235; _temp472.tud=( struct Cyc_Absyn_Tuniondecl*) _temp229; _temp472.tufd=(
struct Cyc_Absyn_Tunionfield*) _temp227; _temp472;}); _temp471;}); _temp470;});
goto _LL451; _LL455: { struct Cyc_List_List* a= all_typs; for( 0; a != 0?
_temp466 != 0: 0; a=({ struct Cyc_List_List* _temp474= a; if( _temp474 == 0){
_throw( Null_Exception);} _temp474->tl;}), _temp466=({ struct Cyc_List_List*
_temp475= _temp466; if( _temp475 == 0){ _throw( Null_Exception);} _temp475->tl;})){
Cyc_Tcutil_unify(( void*)({ struct Cyc_List_List* _temp476= a; if( _temp476 == 0){
_throw( Null_Exception);} _temp476->hd;}),( void*)({ struct Cyc_List_List*
_temp477= _temp466; if( _temp477 == 0){ _throw( Null_Exception);} _temp477->hd;}));}
goto _LL451;} _LL457: goto _LL451; _LL451:;} for( 0; _temp231 != 0? tqts != 0: 0;
_temp231=({ struct Cyc_List_List* _temp478= _temp231; if( _temp478 == 0){ _throw(
Null_Exception);} _temp478->tl;}), tqts=({ struct Cyc_List_List* _temp479= tqts;
if( _temp479 == 0){ _throw( Null_Exception);} _temp479->tl;})){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp492= _temp231; if(
_temp492 == 0){ _throw( Null_Exception);} _temp492->hd;}); void* field_typ= Cyc_Tcutil_rsubstitute(
rgn, inst,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp491= tqts; if(
_temp491 == 0){ _throw( Null_Exception);} _temp491->hd;}))).f2); struct Cyc_List_List*
_temp482; struct Cyc_List_List* _temp484; struct _tuple5 _temp480= Cyc_Tcpat_tcPatRec(
te2, p2,( void**)& field_typ, region_opt); _LL485: _temp484= _temp480.f1; goto
_LL483; _LL483: _temp482= _temp480.f2; goto _LL481; _LL481: tv_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp484); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp482); if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp486= p2->topt; if( _temp486 == 0){ _throw(
Null_Exception);} _temp486->v;}), field_typ)){ Cyc_Tcutil_terr( p2->loc,({
struct _tagged_string _temp488= Cyc_Absynpp_qvar2string( _temp235); struct
_tagged_string _temp489= Cyc_Absynpp_typ2string( field_typ); struct
_tagged_string _temp490= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp487= p2->topt; if( _temp487 == 0){ _throw( Null_Exception);} _temp487->v;}));
xprintf("%.*s expects argument type %.*s, not %.*s", _temp488.last_plus_one -
_temp488.curr, _temp488.curr, _temp489.last_plus_one - _temp489.curr, _temp489.curr,
_temp490.last_plus_one - _temp490.curr, _temp490.curr);}));}} if( _temp231 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp493= Cyc_Absynpp_qvar2string(
_temp235); xprintf("too many arguments for tunion constructor %.*s", _temp493.last_plus_one
- _temp493.curr, _temp493.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp494= Cyc_Absynpp_qvar2string( _temp235); xprintf("too few arguments for tunion constructor %.*s",
_temp494.last_plus_one - _temp494.curr, _temp494.curr);}));}}} _free_region(&
_temp410); _pop_handler();} else{ _free_region(& _temp410); _throw( _temp412);}}}
goto _LL157; _LL185: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp243) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp237->tvs)){
Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char* _temp495=( char*)"wrong number of existential type variables in pattern";
struct _tagged_string _temp496; _temp496.curr= _temp495; _temp496.base= _temp495;
_temp496.last_plus_one= _temp495 + 54; _temp496;}));}{ struct _RegionHandle
_temp497= _new_region(); struct _RegionHandle* rgn=& _temp497; struct
_handler_cons _temp498; _push_handler(& _temp498);{ struct _xtunion_struct*
_temp499=( struct _xtunion_struct*) setjmp( _temp498.handler); if( ! _temp499){{
struct Cyc_List_List* var_tvs= 0;{ struct Cyc_List_List* t= _temp243; for( 0; t
!= 0; t=({ struct Cyc_List_List* _temp500= t; if( _temp500 == 0){ _throw(
Null_Exception);} _temp500->tl;})){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp527= t; if( _temp527 == 0){ _throw( Null_Exception);}
_temp527->hd;});{ void* _temp501=( void*)((( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp511;
void* _temp513; struct Cyc_Absyn_Conref* _temp515; _LL503: if(( unsigned int)
_temp501 > 1u?(( struct _tunion_struct*) _temp501)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL512: _temp511=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp501)->f1;
if(( int) _temp511 == Cyc_Absyn_BoxKind){ goto _LL504;} else{ goto _LL505;}}
else{ goto _LL505;} _LL505: if(( unsigned int) _temp501 > 1u?(( struct
_tunion_struct*) _temp501)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL514: _temp513=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp501)->f1; goto _LL506;} else{
goto _LL507;} _LL507: if(( int) _temp501 == Cyc_Absyn_No_constr){ goto _LL508;}
else{ goto _LL509;} _LL509: if(( unsigned int) _temp501 > 1u?(( struct
_tunion_struct*) _temp501)->tag == Cyc_Absyn_Forward_constr_tag: 0){ _LL516:
_temp515=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp501)->f1; goto _LL510;} else{ goto _LL502;} _LL504: goto _LL502; _LL506:
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp517= Cyc_Absynpp_qvar2string(
_temp245); struct _tagged_string _temp518=* tv->name; struct _tagged_string
_temp519= Cyc_Absynpp_kind2string( _temp513); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp517.last_plus_one - _temp517.curr, _temp517.curr, _temp518.last_plus_one -
_temp518.curr, _temp518.curr, _temp519.last_plus_one - _temp519.curr, _temp519.curr);}));
goto _LL502; _LL508:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp520=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp520[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp521; _temp521.tag= Cyc_Absyn_Eq_constr_tag;
_temp521.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp521;}); _temp520;})));
goto _LL502; _LL510:( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp522=(
char*)"forward after compress in conref"; struct _tagged_string _temp523;
_temp523.curr= _temp522; _temp523.base= _temp522; _temp523.last_plus_one=
_temp522 + 33; _temp523;}))); _LL502:;} var_tvs=({ struct Cyc_List_List*
_temp524=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp524->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp525=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp525[ 0]=({ struct Cyc_Absyn_VarType_struct _temp526; _temp526.tag= Cyc_Absyn_VarType_tag;
_temp526.f1= tv; _temp526;}); _temp525;})); _temp524->tl= var_tvs; _temp524;});}}
var_tvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
var_tvs); tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp243);{ struct Cyc_Tcenv_Tenv*
te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp243); struct Cyc_List_List* tqts=
_temp237->typs; struct Cyc_List_List* exist_inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp237->tvs, var_tvs);
struct Cyc_List_List* exist_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, exist_inst); t=( void*)({ struct Cyc_Absyn_XTunionFieldType_struct*
_temp528=( struct Cyc_Absyn_XTunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_XTunionFieldType_struct)); _temp528[ 0]=({ struct Cyc_Absyn_XTunionFieldType_struct
_temp529; _temp529.tag= Cyc_Absyn_XTunionFieldType_tag; _temp529.f1=({ struct
Cyc_Absyn_XTunionFieldInfo _temp530; _temp530.name= _temp239->name; _temp530.fname=
_temp245; _temp530.xtud=( struct Cyc_Absyn_XTuniondecl*) _temp239; _temp530.xtufd=(
struct Cyc_Absyn_Tunionfield*) _temp237; _temp530;}); _temp529;}); _temp528;});
for( 0; _temp241 != 0? tqts != 0: 0; _temp241=({ struct Cyc_List_List* _temp531=
_temp241; if( _temp531 == 0){ _throw( Null_Exception);} _temp531->tl;}), tqts=({
struct Cyc_List_List* _temp532= tqts; if( _temp532 == 0){ _throw( Null_Exception);}
_temp532->tl;})){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp545= _temp241; if( _temp545 == 0){ _throw( Null_Exception);} _temp545->hd;});
void* field_typ= Cyc_Tcutil_rsubstitute( rgn, exist_inst,(*(( struct _tuple7*)({
struct Cyc_List_List* _temp544= tqts; if( _temp544 == 0){ _throw( Null_Exception);}
_temp544->hd;}))).f2); struct Cyc_List_List* _temp535; struct Cyc_List_List*
_temp537; struct _tuple5 _temp533= Cyc_Tcpat_tcPatRec( te2, p2,( void**)&
field_typ, region_opt); _LL538: _temp537= _temp533.f1; goto _LL536; _LL536:
_temp535= _temp533.f2; goto _LL534; _LL534: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp537); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp535); if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp539= p2->topt; if( _temp539 == 0){ _throw(
Null_Exception);} _temp539->v;}), field_typ)){ Cyc_Tcutil_terr( p2->loc,({
struct _tagged_string _temp541= Cyc_Absynpp_qvar2string( _temp245); struct
_tagged_string _temp542= Cyc_Absynpp_typ2string( field_typ); struct
_tagged_string _temp543= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp540= p2->topt; if( _temp540 == 0){ _throw( Null_Exception);} _temp540->v;}));
xprintf("%.*s expects argument type %.*s, not %.*s", _temp541.last_plus_one -
_temp541.curr, _temp541.curr, _temp542.last_plus_one - _temp542.curr, _temp542.curr,
_temp543.last_plus_one - _temp543.curr, _temp543.curr);}));}} if( _temp241 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp546= Cyc_Absynpp_qvar2string(
_temp245); xprintf("too many arguments for xtunion constructor %.*s", _temp546.last_plus_one
- _temp546.curr, _temp546.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp547= Cyc_Absynpp_qvar2string( _temp245); xprintf("too few arguments for xtunion constructor %.*s",
_temp547.last_plus_one - _temp547.curr, _temp547.curr);}));}}} _free_region(&
_temp497); _pop_handler();} else{ _free_region(& _temp497); _throw( _temp499);}}}
goto _LL157; _LL187: goto _LL189; _LL189: goto _LL191; _LL191: t= Cyc_Absyn_wildtyp();
goto _LL157; _LL157:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt* _temp548=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp548->v=(
void*) t; _temp548;}); return({ struct _tuple5 _temp549; _temp549.f1= tv_result;
_temp549.f2= v_result; _temp549;});}} struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt){ struct _tuple5 ans=
Cyc_Tcpat_tcPatRec( te, p, topt, region_opt);{ struct _RegionHandle _temp550=
_new_region(); struct _RegionHandle* r=& _temp550; struct _handler_cons _temp551;
_push_handler(& _temp551);{ struct _xtunion_struct* _temp552=( struct
_xtunion_struct*) setjmp( _temp551.handler); if( ! _temp552){ Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_string*(* f)(
struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Tcpat_get_name,
ans.f2), p->loc,( struct _tagged_string)({ char* _temp553=( char*)"pattern contains a repeated variable";
struct _tagged_string _temp554; _temp554.curr= _temp553; _temp554.base= _temp553;
_temp554.last_plus_one= _temp553 + 37; _temp554;})); _free_region(& _temp550);
_pop_handler();} else{ _free_region(& _temp550); _throw( _temp552);}}} return
ans;} void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p){ void* _temp555=( void*) p->r; struct Cyc_Absyn_Pat* _temp569; struct Cyc_Absyn_Tunionfield*
_temp571; struct Cyc_Absyn_Tuniondecl* _temp573; struct Cyc_List_List* _temp575;
struct Cyc_List_List* _temp577; struct _tuple1* _temp579; struct Cyc_Absyn_Tunionfield*
_temp581; struct Cyc_Absyn_XTuniondecl* _temp583; struct Cyc_List_List* _temp585;
struct Cyc_List_List* _temp587; struct _tuple1* _temp589; struct Cyc_List_List*
_temp591; struct Cyc_List_List* _temp593; struct Cyc_Core_Opt* _temp595; struct
Cyc_Absyn_Structdecl* _temp597; struct Cyc_List_List* _temp599; _LL557: if((
unsigned int) _temp555 > 2u?(( struct _tunion_struct*) _temp555)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL570: _temp569=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp555)->f1; goto _LL558;} else{ goto _LL559;} _LL559: if(( unsigned int)
_temp555 > 2u?(( struct _tunion_struct*) _temp555)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL580: _temp579=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp555)->f1; goto _LL578; _LL578: _temp577=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp555)->f2; goto _LL576; _LL576: _temp575=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp555)->f3; goto
_LL574; _LL574: _temp573=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp555)->f4; goto _LL572; _LL572: _temp571=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp555)->f5; goto _LL560;} else{ goto
_LL561;} _LL561: if(( unsigned int) _temp555 > 2u?(( struct _tunion_struct*)
_temp555)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL590: _temp589=( struct _tuple1*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp555)->f1; goto _LL588; _LL588: _temp587=(
struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp555)->f2; goto
_LL586; _LL586: _temp585=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp555)->f3; goto _LL584; _LL584: _temp583=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp555)->f4; goto _LL582; _LL582: _temp581=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp555)->f5;
goto _LL562;} else{ goto _LL563;} _LL563: if(( unsigned int) _temp555 > 2u?((
struct _tunion_struct*) _temp555)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL598:
_temp597=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp555)->f1; goto _LL596; _LL596: _temp595=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp555)->f2; goto _LL594; _LL594: _temp593=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp555)->f3; goto
_LL592; _LL592: _temp591=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp555)->f4; goto _LL564;} else{ goto _LL565;} _LL565: if(( unsigned int)
_temp555 > 2u?(( struct _tunion_struct*) _temp555)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL600: _temp599=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp555)->f1; goto _LL566;} else{ goto _LL567;} _LL567: goto _LL568; _LL558:
Cyc_Tcpat_check_pat_regions( te, _temp569);{ void* _temp602=( void*)({ struct
Cyc_Core_Opt* _temp601= p->topt; if( _temp601 == 0){ _throw( Null_Exception);}
_temp601->v;}); struct Cyc_Absyn_PtrInfo _temp612; struct Cyc_Absyn_Conref*
_temp614; struct Cyc_Absyn_Tqual _temp616; struct Cyc_Absyn_Conref* _temp618;
void* _temp620; void* _temp622; struct Cyc_Absyn_TunionInfo _temp624; struct Cyc_Absyn_Tuniondecl*
_temp626; void* _temp628; struct Cyc_List_List* _temp630; struct _tuple1*
_temp632; struct Cyc_Absyn_XTunionInfo _temp634; struct Cyc_Absyn_XTuniondecl*
_temp636; void* _temp638; struct _tuple1* _temp640; _LL604: if(( unsigned int)
_temp602 > 4u?(( struct _tunion_struct*) _temp602)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL613: _temp612=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp602)->f1; _LL623: _temp622=( void*) _temp612.elt_typ; goto _LL621; _LL621:
_temp620=( void*) _temp612.rgn_typ; goto _LL619; _LL619: _temp618=( struct Cyc_Absyn_Conref*)
_temp612.nullable; goto _LL617; _LL617: _temp616=( struct Cyc_Absyn_Tqual)
_temp612.tq; goto _LL615; _LL615: _temp614=( struct Cyc_Absyn_Conref*) _temp612.bounds;
goto _LL605;} else{ goto _LL606;} _LL606: if(( unsigned int) _temp602 > 4u?((
struct _tunion_struct*) _temp602)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL625:
_temp624=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp602)->f1; _LL633: _temp632=( struct _tuple1*) _temp624.name; goto _LL631;
_LL631: _temp630=( struct Cyc_List_List*) _temp624.targs; goto _LL629; _LL629:
_temp628=( void*) _temp624.rgn; goto _LL627; _LL627: _temp626=( struct Cyc_Absyn_Tuniondecl*)
_temp624.tud; goto _LL607;} else{ goto _LL608;} _LL608: if(( unsigned int)
_temp602 > 4u?(( struct _tunion_struct*) _temp602)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL635: _temp634=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp602)->f1; _LL641: _temp640=( struct _tuple1*) _temp634.name; goto _LL639;
_LL639: _temp638=( void*) _temp634.rgn; goto _LL637; _LL637: _temp636=( struct
Cyc_Absyn_XTuniondecl*) _temp634.xtud; goto _LL609;} else{ goto _LL610;} _LL610:
goto _LL611; _LL605: _temp628= _temp620; goto _LL607; _LL607: _temp638= _temp628;
goto _LL609; _LL609: Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp638);
return; _LL611:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp642=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp642[ 0]=({ struct Cyc_Core_Impossible_struct _temp643; _temp643.tag= Cyc_Core_Impossible_tag;
_temp643.f1=( struct _tagged_string)({ char* _temp644=( char*)"check_pat_regions: bad pointer type";
struct _tagged_string _temp645; _temp645.curr= _temp644; _temp645.base= _temp644;
_temp645.last_plus_one= _temp644 + 36; _temp645;}); _temp643;}); _temp642;}));
_LL603:;} _LL560: for( 0; _temp575 != 0; _temp575=({ struct Cyc_List_List*
_temp646= _temp575; if( _temp646 == 0){ _throw( Null_Exception);} _temp646->tl;})){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp647= _temp575; if( _temp647 == 0){ _throw( Null_Exception);} _temp647->hd;}));}{
void* _temp649=( void*)({ struct Cyc_Core_Opt* _temp648= p->topt; if( _temp648
== 0){ _throw( Null_Exception);} _temp648->v;}); struct Cyc_Absyn_TunionInfo
_temp657; struct Cyc_Absyn_Tuniondecl* _temp659; void* _temp661; struct Cyc_List_List*
_temp663; struct _tuple1* _temp665; struct Cyc_Absyn_TunionFieldInfo _temp667;
_LL651: if(( unsigned int) _temp649 > 4u?(( struct _tunion_struct*) _temp649)->tag
== Cyc_Absyn_TunionType_tag: 0){ _LL658: _temp657=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp649)->f1; _LL666: _temp665=( struct
_tuple1*) _temp657.name; goto _LL664; _LL664: _temp663=( struct Cyc_List_List*)
_temp657.targs; goto _LL662; _LL662: _temp661=( void*) _temp657.rgn; goto _LL660;
_LL660: _temp659=( struct Cyc_Absyn_Tuniondecl*) _temp657.tud; goto _LL652;}
else{ goto _LL653;} _LL653: if(( unsigned int) _temp649 > 4u?(( struct
_tunion_struct*) _temp649)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL668:
_temp667=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp649)->f1; goto _LL654;} else{ goto _LL655;} _LL655: goto _LL656; _LL652:
Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp661); return; _LL654: return;
_LL656:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp669=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp669[ 0]=({ struct Cyc_Core_Impossible_struct _temp670; _temp670.tag= Cyc_Core_Impossible_tag;
_temp670.f1=( struct _tagged_string)({ char* _temp671=( char*)"check_pat_regions: bad tunion type";
struct _tagged_string _temp672; _temp672.curr= _temp671; _temp672.base= _temp671;
_temp672.last_plus_one= _temp671 + 35; _temp672;}); _temp670;}); _temp669;}));
_LL650:;} _LL562: for( 0; _temp585 != 0; _temp585=({ struct Cyc_List_List*
_temp673= _temp585; if( _temp673 == 0){ _throw( Null_Exception);} _temp673->tl;})){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp674= _temp585; if( _temp674 == 0){ _throw( Null_Exception);} _temp674->hd;}));}{
void* _temp676=( void*)({ struct Cyc_Core_Opt* _temp675= p->topt; if( _temp675
== 0){ _throw( Null_Exception);} _temp675->v;}); struct Cyc_Absyn_XTunionInfo
_temp684; struct Cyc_Absyn_XTuniondecl* _temp686; void* _temp688; struct _tuple1*
_temp690; struct Cyc_Absyn_XTunionFieldInfo _temp692; _LL678: if(( unsigned int)
_temp676 > 4u?(( struct _tunion_struct*) _temp676)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL685: _temp684=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp676)->f1; _LL691: _temp690=( struct _tuple1*) _temp684.name; goto _LL689;
_LL689: _temp688=( void*) _temp684.rgn; goto _LL687; _LL687: _temp686=( struct
Cyc_Absyn_XTuniondecl*) _temp684.xtud; goto _LL679;} else{ goto _LL680;} _LL680:
if(( unsigned int) _temp676 > 4u?(( struct _tunion_struct*) _temp676)->tag ==
Cyc_Absyn_XTunionFieldType_tag: 0){ _LL693: _temp692=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp676)->f1; goto _LL681;} else{
goto _LL682;} _LL682: goto _LL683; _LL679: Cyc_Tcenv_check_rgn_accessible( te, p->loc,
_temp688); return; _LL681: return; _LL683:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp694=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp694[ 0]=({ struct
Cyc_Core_Impossible_struct _temp695; _temp695.tag= Cyc_Core_Impossible_tag;
_temp695.f1=( struct _tagged_string)({ char* _temp696=( char*)"check_pat_regions: bad xtunion type";
struct _tagged_string _temp697; _temp697.curr= _temp696; _temp697.base= _temp696;
_temp697.last_plus_one= _temp696 + 36; _temp697;}); _temp695;}); _temp694;}));
_LL677:;} _LL564: _temp599=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple6*)) Cyc_Core_snd, _temp591); goto _LL566; _LL566: for( 0; _temp599
!= 0; _temp599=({ struct Cyc_List_List* _temp698= _temp599; if( _temp698 == 0){
_throw( Null_Exception);} _temp698->tl;})){ Cyc_Tcpat_check_pat_regions( te,(
struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp699= _temp599; if( _temp699
== 0){ _throw( Null_Exception);} _temp699->hd;}));} return; _LL568: return;
_LL556:;} typedef void* Cyc_Tcpat_Name_value; static const int Cyc_Tcpat_Name_v_tag=
0; struct Cyc_Tcpat_Name_v_struct{ int tag; struct _tagged_string f1; } ; static
const int Cyc_Tcpat_Int_v_tag= 1; struct Cyc_Tcpat_Int_v_struct{ int tag; int f1;
} ; typedef void* Cyc_Tcpat_name_value_t; struct Cyc_Tcpat_Con_s{ void* name;
int arity; struct Cyc_Core_Opt* span; } ; typedef struct Cyc_Tcpat_Con_s* Cyc_Tcpat_con_t;
typedef void* Cyc_Tcpat_Simple_pat; static const unsigned int Cyc_Tcpat_Any= 0;
static const int Cyc_Tcpat_Con_tag= 0; struct Cyc_Tcpat_Con_struct{ int tag;
struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; typedef void* Cyc_Tcpat_simple_pat_t;
static int Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s*
c2){ void* _temp700=( void*) c1->name; struct _tagged_string _temp706; int
_temp708; _LL702: if((( struct _tunion_struct*) _temp700)->tag == Cyc_Tcpat_Name_v_tag){
_LL707: _temp706=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp700)->f1; goto _LL703;} else{ goto _LL704;} _LL704: if((( struct
_tunion_struct*) _temp700)->tag == Cyc_Tcpat_Int_v_tag){ _LL709: _temp708=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp700)->f1; goto _LL705;} else{ goto _LL701;}
_LL703: { void* _temp710=( void*) c2->name; struct _tagged_string _temp716; int
_temp718; _LL712: if((( struct _tunion_struct*) _temp710)->tag == Cyc_Tcpat_Name_v_tag){
_LL717: _temp716=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp710)->f1; goto _LL713;} else{ goto _LL714;} _LL714: if((( struct
_tunion_struct*) _temp710)->tag == Cyc_Tcpat_Int_v_tag){ _LL719: _temp718=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp710)->f1; goto _LL715;} else{ goto _LL711;}
_LL713: return Cyc_String_zstrcmp( _temp706, _temp716); _LL715: return - 1;
_LL711:;} _LL705: { void* _temp720=( void*) c2->name; struct _tagged_string
_temp726; int _temp728; _LL722: if((( struct _tunion_struct*) _temp720)->tag ==
Cyc_Tcpat_Name_v_tag){ _LL727: _temp726=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp720)->f1; goto _LL723;} else{ goto _LL724;} _LL724: if((( struct
_tunion_struct*) _temp720)->tag == Cyc_Tcpat_Int_v_tag){ _LL729: _temp728=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp720)->f1; goto _LL725;} else{ goto _LL721;}
_LL723: return 1; _LL725: return _temp708 - _temp728; _LL721:;} _LL701:;} static
struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct Cyc_Set_Set*(*)(
int(* comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*))) Cyc_Set_empty)(
Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt=( struct
Cyc_Core_Opt){.v=( void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt=(
struct Cyc_Core_Opt){.v=( void*) 2}; static struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static char _temp733[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={ 0u,( struct
_tagged_string){ _temp733, _temp733, _temp733 + 5u}}; static char _temp737[ 4u]="ptr";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={ 0u,( struct
_tagged_string){ _temp737, _temp737, _temp737 + 4u}}; static char _temp741[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,( struct
_tagged_string){ _temp741, _temp741, _temp741 + 6u}}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_null_name_value),.arity=
0,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt}; static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=&
Cyc_Tcpat_null_ptr_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp742=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp742->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp743=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp743[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp744; _temp744.tag= Cyc_Tcpat_Int_v_tag;
_temp744.f1= i; _temp744;}); _temp743;})); _temp742->arity= 0; _temp742->span= 0;
_temp742;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_string f){ return({ struct Cyc_Tcpat_Con_s* _temp745=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp745->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp746=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp746[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp747; _temp747.tag= Cyc_Tcpat_Name_v_tag; _temp747.f1= f; _temp747;});
_temp746;})); _temp745->arity= 0; _temp745->span= 0; _temp745;});} static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_char_con( char c){ return({ struct Cyc_Tcpat_Con_s*
_temp748=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp748->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp750=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp750[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp751; _temp751.tag= Cyc_Tcpat_Int_v_tag;
_temp751.f1=( int) c; _temp751;}); _temp750;})); _temp748->arity= 0; _temp748->span=({
struct Cyc_Core_Opt* _temp749=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp749->v=( void*) 256; _temp749;}); _temp748;});}
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp752=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp752->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp752->arity=
i; _temp752->span= Cyc_Tcpat_one_opt_ptr; _temp752;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp753=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp753[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp754; _temp754.tag= Cyc_Tcpat_Con_tag; _temp754.f1= Cyc_Tcpat_null_con;
_temp754.f2= 0; _temp754;}); _temp753;});} static void* Cyc_Tcpat_int_pat( int i){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp755=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp755[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp756; _temp756.tag= Cyc_Tcpat_Con_tag; _temp756.f1= Cyc_Tcpat_int_con( i);
_temp756.f2= 0; _temp756;}); _temp755;});} static void* Cyc_Tcpat_char_pat( char
c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp757=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp757[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp758; _temp758.tag= Cyc_Tcpat_Con_tag; _temp758.f1= Cyc_Tcpat_char_con( c);
_temp758.f2= 0; _temp758;}); _temp757;});} static void* Cyc_Tcpat_float_pat(
struct _tagged_string f){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp759=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp759[ 0]=({ struct Cyc_Tcpat_Con_struct _temp760; _temp760.tag= Cyc_Tcpat_Con_tag;
_temp760.f1= Cyc_Tcpat_float_con( f); _temp760.f2= 0; _temp760;}); _temp759;});}
static void* Cyc_Tcpat_null_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp761=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp761[ 0]=({ struct Cyc_Tcpat_Con_struct _temp762; _temp762.tag= Cyc_Tcpat_Con_tag;
_temp762.f1= Cyc_Tcpat_null_ptr_con; _temp762.f2=({ struct Cyc_List_List*
_temp763=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp763->hd=( void*) p; _temp763->tl= 0; _temp763;}); _temp762;}); _temp761;});}
static void* Cyc_Tcpat_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp764=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp764[ 0]=({ struct Cyc_Tcpat_Con_struct _temp765; _temp765.tag= Cyc_Tcpat_Con_tag;
_temp765.f1= Cyc_Tcpat_ptr_con; _temp765.f2=({ struct Cyc_List_List* _temp766=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp766->hd=(
void*) p; _temp766->tl= 0; _temp766;}); _temp765;}); _temp764;});} static void*
Cyc_Tcpat_tuple_pat( struct Cyc_List_List* ss){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp767=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp767[ 0]=({ struct Cyc_Tcpat_Con_struct _temp768; _temp768.tag= Cyc_Tcpat_Con_tag;
_temp768.f1= Cyc_Tcpat_tuple_con((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ss)); _temp768.f2= ss; _temp768;}); _temp767;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_string con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List*
ps){ struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp771=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp771->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp772=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp772[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp773; _temp773.tag= Cyc_Tcpat_Name_v_tag; _temp773.f1= con_name; _temp773;});
_temp772;})); _temp771->arity=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ps); _temp771->span= span; _temp771;}); return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp769=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp769[ 0]=({ struct Cyc_Tcpat_Con_struct _temp770; _temp770.tag= Cyc_Tcpat_Con_tag;
_temp770.f1= c; _temp770.f2= ps; _temp770;}); _temp769;});} static void* Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat* p){ void* s;{ void* _temp774=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp808; int _temp810; void* _temp812; char _temp814; struct _tagged_string
_temp816; struct Cyc_List_List* _temp818; struct Cyc_Absyn_Vardecl* _temp820;
struct Cyc_Absyn_Pat* _temp822; struct Cyc_List_List* _temp824; struct Cyc_List_List*
_temp826; struct Cyc_Core_Opt* _temp828; struct Cyc_Absyn_Structdecl* _temp830;
struct Cyc_Absyn_Tunionfield* _temp832; struct Cyc_Absyn_Tuniondecl* _temp834;
struct Cyc_List_List* _temp836; struct Cyc_List_List* _temp838; struct _tuple1*
_temp840; struct Cyc_Absyn_Tunionfield* _temp842; struct Cyc_Absyn_XTuniondecl*
_temp844; struct Cyc_List_List* _temp846; struct Cyc_List_List* _temp848; struct
_tuple1* _temp850; struct Cyc_Absyn_Enumfield* _temp852; struct Cyc_Absyn_Enumdecl*
_temp854; struct _tuple1* _temp856; struct _tuple1* _temp858; struct Cyc_List_List*
_temp860; struct Cyc_List_List* _temp862; struct _tuple1* _temp864; struct Cyc_List_List*
_temp866; struct Cyc_List_List* _temp868; struct _tuple1* _temp870; _LL776: if((
int) _temp774 == Cyc_Absyn_Wild_p){ goto _LL777;} else{ goto _LL778;} _LL778:
if(( unsigned int) _temp774 > 2u?(( struct _tunion_struct*) _temp774)->tag ==
Cyc_Absyn_Var_p_tag: 0){ _LL809: _temp808=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Var_p_struct*) _temp774)->f1; goto _LL779;} else{ goto _LL780;} _LL780:
if(( int) _temp774 == Cyc_Absyn_Null_p){ goto _LL781;} else{ goto _LL782;}
_LL782: if(( unsigned int) _temp774 > 2u?(( struct _tunion_struct*) _temp774)->tag
== Cyc_Absyn_Int_p_tag: 0){ _LL813: _temp812=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp774)->f1; goto _LL811; _LL811: _temp810=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp774)->f2; goto _LL783;} else{ goto _LL784;} _LL784: if(( unsigned int)
_temp774 > 2u?(( struct _tunion_struct*) _temp774)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL815: _temp814=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp774)->f1;
goto _LL785;} else{ goto _LL786;} _LL786: if(( unsigned int) _temp774 > 2u?((
struct _tunion_struct*) _temp774)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL817:
_temp816=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp774)->f1;
goto _LL787;} else{ goto _LL788;} _LL788: if(( unsigned int) _temp774 > 2u?((
struct _tunion_struct*) _temp774)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL819:
_temp818=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp774)->f1;
goto _LL789;} else{ goto _LL790;} _LL790: if(( unsigned int) _temp774 > 2u?((
struct _tunion_struct*) _temp774)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL821:
_temp820=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp774)->f1; goto _LL791;} else{ goto _LL792;} _LL792: if(( unsigned int)
_temp774 > 2u?(( struct _tunion_struct*) _temp774)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL823: _temp822=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp774)->f1; goto _LL793;} else{ goto _LL794;} _LL794: if(( unsigned int)
_temp774 > 2u?(( struct _tunion_struct*) _temp774)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL831: _temp830=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp774)->f1; goto _LL829; _LL829: _temp828=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp774)->f2; goto _LL827; _LL827: _temp826=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp774)->f3; goto
_LL825; _LL825: _temp824=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp774)->f4; goto _LL795;} else{ goto _LL796;} _LL796: if(( unsigned int)
_temp774 > 2u?(( struct _tunion_struct*) _temp774)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL841: _temp840=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp774)->f1; goto _LL839; _LL839: _temp838=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp774)->f2; goto _LL837; _LL837: _temp836=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp774)->f3; goto
_LL835; _LL835: _temp834=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp774)->f4; goto _LL833; _LL833: _temp832=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp774)->f5; goto _LL797;} else{ goto
_LL798;} _LL798: if(( unsigned int) _temp774 > 2u?(( struct _tunion_struct*)
_temp774)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL851: _temp850=( struct _tuple1*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp774)->f1; goto _LL849; _LL849: _temp848=(
struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp774)->f2; goto
_LL847; _LL847: _temp846=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp774)->f3; goto _LL845; _LL845: _temp844=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp774)->f4; goto _LL843; _LL843: _temp842=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp774)->f5;
goto _LL799;} else{ goto _LL800;} _LL800: if(( unsigned int) _temp774 > 2u?((
struct _tunion_struct*) _temp774)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL857:
_temp856=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp774)->f1;
goto _LL855; _LL855: _temp854=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp774)->f2; goto _LL853; _LL853: _temp852=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp774)->f3; goto _LL801;} else{ goto _LL802;}
_LL802: if(( unsigned int) _temp774 > 2u?(( struct _tunion_struct*) _temp774)->tag
== Cyc_Absyn_UnknownId_p_tag: 0){ _LL859: _temp858=( struct _tuple1*)(( struct
Cyc_Absyn_UnknownId_p_struct*) _temp774)->f1; goto _LL803;} else{ goto _LL804;}
_LL804: if(( unsigned int) _temp774 > 2u?(( struct _tunion_struct*) _temp774)->tag
== Cyc_Absyn_UnknownCall_p_tag: 0){ _LL865: _temp864=( struct _tuple1*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp774)->f1; goto _LL863; _LL863: _temp862=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp774)->f2;
goto _LL861; _LL861: _temp860=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp774)->f3; goto _LL805;} else{ goto _LL806;} _LL806: if(( unsigned int)
_temp774 > 2u?(( struct _tunion_struct*) _temp774)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL871: _temp870=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp774)->f1; goto _LL869; _LL869: _temp868=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp774)->f2; goto _LL867; _LL867: _temp866=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp774)->f3;
goto _LL807;} else{ goto _LL775;} _LL777: s=( void*) Cyc_Tcpat_Any; goto _LL775;
_LL779: s=( void*) Cyc_Tcpat_Any; goto _LL775; _LL781: s= Cyc_Tcpat_null_pat();
goto _LL775; _LL783: s= Cyc_Tcpat_int_pat( _temp810); goto _LL775; _LL785: s=
Cyc_Tcpat_char_pat( _temp814); goto _LL775; _LL787: s= Cyc_Tcpat_float_pat(
_temp816); goto _LL775; _LL789: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp818)); goto _LL775; _LL791: s=( void*) Cyc_Tcpat_Any; goto _LL775; _LL793: {
void* ss= Cyc_Tcpat_compile_pat( _temp822);{ void* _temp873= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp872= p->topt; if( _temp872 == 0){ _throw(
Null_Exception);} _temp872->v;})); struct Cyc_Absyn_PtrInfo _temp883; struct Cyc_Absyn_Conref*
_temp885; struct Cyc_Absyn_Tqual _temp887; struct Cyc_Absyn_Conref* _temp889;
void* _temp891; void* _temp893; struct Cyc_Absyn_TunionInfo _temp895; struct Cyc_Absyn_XTunionInfo
_temp897; _LL875: if(( unsigned int) _temp873 > 4u?(( struct _tunion_struct*)
_temp873)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL884: _temp883=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp873)->f1; _LL894: _temp893=( void*)
_temp883.elt_typ; goto _LL892; _LL892: _temp891=( void*) _temp883.rgn_typ; goto
_LL890; _LL890: _temp889=( struct Cyc_Absyn_Conref*) _temp883.nullable; goto
_LL888; _LL888: _temp887=( struct Cyc_Absyn_Tqual) _temp883.tq; goto _LL886;
_LL886: _temp885=( struct Cyc_Absyn_Conref*) _temp883.bounds; goto _LL876;}
else{ goto _LL877;} _LL877: if(( unsigned int) _temp873 > 4u?(( struct
_tunion_struct*) _temp873)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL896:
_temp895=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp873)->f1; goto _LL878;} else{ goto _LL879;} _LL879: if(( unsigned int)
_temp873 > 4u?(( struct _tunion_struct*) _temp873)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL898: _temp897=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp873)->f1; goto _LL880;} else{ goto _LL881;} _LL881: goto _LL882; _LL876: {
int is_nullable= 0; int still_working= 1; while( still_working) { void* _temp899=(
void*) _temp889->v; struct Cyc_Absyn_Conref* _temp907; int _temp909; _LL901: if((
unsigned int) _temp899 > 1u?(( struct _tunion_struct*) _temp899)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL908: _temp907=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp899)->f1; goto _LL902;} else{ goto _LL903;} _LL903: if(( int) _temp899 ==
Cyc_Absyn_No_constr){ goto _LL904;} else{ goto _LL905;} _LL905: if((
unsigned int) _temp899 > 1u?(( struct _tunion_struct*) _temp899)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL910: _temp909=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp899)->f1;
goto _LL906;} else{ goto _LL900;} _LL902:( void*)( _temp889->v=( void*)(( void*)
_temp907->v)); continue; _LL904:( void*)( _temp889->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp911=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp911[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp912; _temp912.tag= Cyc_Absyn_Eq_constr_tag;
_temp912.f1=( void*) 0; _temp912;}); _temp911;}))); is_nullable= 0;
still_working= 0; goto _LL900; _LL906: is_nullable=( int) _temp909;
still_working= 0; goto _LL900; _LL900:;} if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL874;} _LL878: s= ss; goto _LL874;
_LL880: s= ss; goto _LL874; _LL882: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp913=( char*)"pointer pattern does not have pointer type!";
struct _tagged_string _temp914; _temp914.curr= _temp913; _temp914.base= _temp913;
_temp914.last_plus_one= _temp913 + 44; _temp914;})); goto _LL874; _LL874:;} goto
_LL775;} _LL795: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp947= _temp830->fields; if(
_temp947 == 0){ _throw( Null_Exception);} _temp947->v;}); for( 0; fields != 0;
fields=({ struct Cyc_List_List* _temp915= fields; if( _temp915 == 0){ _throw(
Null_Exception);} _temp915->tl;})){ int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp944= fields; if( _temp944 == 0){ _throw(
Null_Exception);} _temp944->hd;}))->name,( struct _tagged_string)({ char*
_temp945=( char*)""; struct _tagged_string _temp946; _temp946.curr= _temp945;
_temp946.base= _temp945; _temp946.last_plus_one= _temp945 + 1; _temp946;})) == 0;{
struct Cyc_List_List* dlps0= _temp824; for( 0; ! found? dlps0 != 0: 0; dlps0=({
struct Cyc_List_List* _temp916= dlps0; if( _temp916 == 0){ _throw(
Null_Exception);} _temp916->tl;})){ struct _tuple6 _temp920; struct Cyc_Absyn_Pat*
_temp921; struct Cyc_List_List* _temp923; struct _tuple6* _temp918=( struct
_tuple6*)({ struct Cyc_List_List* _temp917= dlps0; if( _temp917 == 0){ _throw(
Null_Exception);} _temp917->hd;}); _temp920=* _temp918; _LL924: _temp923=
_temp920.f1; goto _LL922; _LL922: _temp921= _temp920.f2; goto _LL919; _LL919: {
struct Cyc_List_List* _temp925= _temp923; struct Cyc_List_List _temp931; struct
Cyc_List_List* _temp932; void* _temp934; struct _tagged_string* _temp936; _LL927:
if( _temp925 == 0){ goto _LL929;} else{ _temp931=* _temp925; _LL935: _temp934=(
void*) _temp931.hd; if((( struct _tunion_struct*) _temp934)->tag == Cyc_Absyn_FieldName_tag){
_LL937: _temp936=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp934)->f1; goto _LL933;} else{ goto _LL929;} _LL933: _temp932=( struct Cyc_List_List*)
_temp931.tl; if( _temp932 == 0){ goto _LL928;} else{ goto _LL929;}} _LL929: goto
_LL930; _LL928: if( Cyc_String_zstrptrcmp( _temp936,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp938= fields; if( _temp938 == 0){ _throw(
Null_Exception);} _temp938->hd;}))->name) == 0){ ps=({ struct Cyc_List_List*
_temp939=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp939->hd=( void*) Cyc_Tcpat_compile_pat( _temp921); _temp939->tl= ps;
_temp939;}); found= 1;} goto _LL926; _LL930:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp940=( char*)"bad designator(s)";
struct _tagged_string _temp941; _temp941.curr= _temp940; _temp941.base= _temp940;
_temp941.last_plus_one= _temp940 + 18; _temp941;})); goto _LL926; _LL926:;}}}
if( ! found){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp942=( char*)"bad designator"; struct _tagged_string
_temp943; _temp943.curr= _temp942; _temp943.base= _temp942; _temp943.last_plus_one=
_temp942 + 15; _temp943;}));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL775;}
_LL797: { int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp949= _temp834->fields; if(
_temp949 == 0){ _throw( Null_Exception);} _temp949->v;})); s= Cyc_Tcpat_con_pat(*(*
_temp832->name).f2,({ struct Cyc_Core_Opt* _temp948=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp948->v=( void*) span;
_temp948;}),(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp836)); goto
_LL775;} _LL799: s= Cyc_Tcpat_con_pat(*(* _temp842->name).f2, 0,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp846)); goto _LL775; _LL801: { int span=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp854->fields); s= Cyc_Tcpat_con_pat(*(* _temp852->name).f2,({
struct Cyc_Core_Opt* _temp950=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp950->v=( void*) span; _temp950;}), 0); goto _LL775;}
_LL803: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp951=( char*)"compile_pat: unknownid"; struct
_tagged_string _temp952; _temp952.curr= _temp951; _temp952.base= _temp951;
_temp952.last_plus_one= _temp951 + 23; _temp952;})); goto _LL775; _LL805: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp953=( char*)"compile_pat: unknowncall"; struct _tagged_string
_temp954; _temp954.curr= _temp953; _temp954.base= _temp953; _temp954.last_plus_one=
_temp953 + 25; _temp954;})); goto _LL775; _LL807: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp955=(
char*)"compile_pat: unknownfields"; struct _tagged_string _temp956; _temp956.curr=
_temp955; _temp956.base= _temp955; _temp956.last_plus_one= _temp955 + 27;
_temp956;})); goto _LL775; _LL775:;} return s;} typedef struct Cyc_List_List*
Cyc_Tcpat_match_t; typedef void* Cyc_Tcpat_Term_desc; static const int Cyc_Tcpat_Pos_tag=
0; struct Cyc_Tcpat_Pos_struct{ int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Tcpat_Neg_tag= 1; struct Cyc_Tcpat_Neg_struct{ int
tag; struct Cyc_Set_Set* f1; } ; typedef void* Cyc_Tcpat_term_desc_t; typedef
struct Cyc_List_List* Cyc_Tcpat_context_t; typedef struct Cyc_List_List* Cyc_Tcpat_access_t;
typedef void* Cyc_Tcpat_Decision; static const unsigned int Cyc_Tcpat_Failure= 0;
static const int Cyc_Tcpat_Success_tag= 0; struct Cyc_Tcpat_Success_struct{ int
tag; void* f1; } ; static const int Cyc_Tcpat_IfEq_tag= 1; struct Cyc_Tcpat_IfEq_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s* f2; void* f3; void*
f4; } ; typedef void* Cyc_Tcpat_decision_t; struct _tuple9{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; typedef struct
_tuple9* Cyc_Tcpat_work_stack_frame_t; typedef struct Cyc_List_List* Cyc_Tcpat_work_stack_t;
typedef void* Cyc_Tcpat_Answer; static const unsigned int Cyc_Tcpat_Yes= 0;
static const unsigned int Cyc_Tcpat_No= 1; static const unsigned int Cyc_Tcpat_Maybe=
2; typedef void* Cyc_Tcpat_answer_t; static void* Cyc_Tcpat_add_neg( void* td,
struct Cyc_Tcpat_Con_s* c){ void* _temp957= td; struct Cyc_Set_Set* _temp963;
struct Cyc_List_List* _temp965; struct Cyc_Tcpat_Con_s* _temp967; _LL959: if(((
struct _tunion_struct*) _temp957)->tag == Cyc_Tcpat_Neg_tag){ _LL964: _temp963=(
struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp957)->f1; goto _LL960;}
else{ goto _LL961;} _LL961: if((( struct _tunion_struct*) _temp957)->tag == Cyc_Tcpat_Pos_tag){
_LL968: _temp967=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*)
_temp957)->f1; goto _LL966; _LL966: _temp965=( struct Cyc_List_List*)(( struct
Cyc_Tcpat_Pos_struct*) _temp957)->f2; goto _LL962;} else{ goto _LL958;} _LL960:
if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp963, c)){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp969=( char*)"add_neg called when constructor already in set";
struct _tagged_string _temp970; _temp970.curr= _temp969; _temp970.base= _temp969;
_temp970.last_plus_one= _temp969 + 47; _temp970;}));} if( c->span != 0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp963) + 1 >=( int)({ struct
Cyc_Core_Opt* _temp971= c->span; if( _temp971 == 0){ _throw( Null_Exception);}
_temp971->v;}): 0){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp972=( char*)"add_neg called when |cs U {c}| >= span(c)";
struct _tagged_string _temp973; _temp973.curr= _temp972; _temp973.base= _temp972;
_temp973.last_plus_one= _temp972 + 42; _temp973;}));} return( void*)({ struct
Cyc_Tcpat_Neg_struct* _temp974=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp974[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp975; _temp975.tag= Cyc_Tcpat_Neg_tag; _temp975.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp963,
c); _temp975;}); _temp974;}); _LL962: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp976=( char*)"add_neg called when td is Positive";
struct _tagged_string _temp977; _temp977.curr= _temp976; _temp977.base= _temp976;
_temp977.last_plus_one= _temp976 + 35; _temp977;})); _LL958:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp978= td; struct Cyc_List_List*
_temp984; struct Cyc_Tcpat_Con_s* _temp986; struct Cyc_Set_Set* _temp988; _LL980:
if((( struct _tunion_struct*) _temp978)->tag == Cyc_Tcpat_Pos_tag){ _LL987:
_temp986=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp978)->f1;
goto _LL985; _LL985: _temp984=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp978)->f2; goto _LL981;} else{ goto _LL982;} _LL982: if((( struct
_tunion_struct*) _temp978)->tag == Cyc_Tcpat_Neg_tag){ _LL989: _temp988=( struct
Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp978)->f1; goto _LL983;} else{
goto _LL979;} _LL981: if( Cyc_Tcpat_compare_con( c, _temp986) == 0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL983: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp988,
c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)({ struct Cyc_Core_Opt*
_temp990= c->span; if( _temp990 == 0){ _throw( Null_Exception);} _temp990->v;})
==(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp988) + 1: 0){
return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}} _LL979:;}
struct _tuple10{ struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ;
static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt, void*
dsc){ struct Cyc_List_List* _temp991= ctxt; struct Cyc_List_List _temp997;
struct Cyc_List_List* _temp998; struct _tuple10* _temp1000; struct _tuple10
_temp1002; struct Cyc_List_List* _temp1003; struct Cyc_Tcpat_Con_s* _temp1005;
_LL993: if( _temp991 == 0){ goto _LL994;} else{ goto _LL995;} _LL995: if(
_temp991 == 0){ goto _LL992;} else{ _temp997=* _temp991; _LL1001: _temp1000=(
struct _tuple10*) _temp997.hd; _temp1002=* _temp1000; _LL1006: _temp1005=
_temp1002.f1; goto _LL1004; _LL1004: _temp1003= _temp1002.f2; goto _LL999;
_LL999: _temp998=( struct Cyc_List_List*) _temp997.tl; goto _LL996;} _LL994:
return 0; _LL996: return({ struct Cyc_List_List* _temp1007=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1007->hd=( void*)({ struct
_tuple10* _temp1008=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp1008->f1= _temp1005; _temp1008->f2=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1009=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1009->hd=( void*) dsc; _temp1009->tl= _temp1003; _temp1009;}); _temp1008;});
_temp1007->tl= _temp998; _temp1007;}); _LL992:;} static struct Cyc_List_List*
Cyc_Tcpat_norm_context( struct Cyc_List_List* ctxt){ struct Cyc_List_List*
_temp1010= ctxt; struct Cyc_List_List _temp1016; struct Cyc_List_List* _temp1017;
struct _tuple10* _temp1019; struct _tuple10 _temp1021; struct Cyc_List_List*
_temp1022; struct Cyc_Tcpat_Con_s* _temp1024; _LL1012: if( _temp1010 == 0){ goto
_LL1013;} else{ goto _LL1014;} _LL1014: if( _temp1010 == 0){ goto _LL1011;}
else{ _temp1016=* _temp1010; _LL1020: _temp1019=( struct _tuple10*) _temp1016.hd;
_temp1021=* _temp1019; _LL1025: _temp1024= _temp1021.f1; goto _LL1023; _LL1023:
_temp1022= _temp1021.f2; goto _LL1018; _LL1018: _temp1017=( struct Cyc_List_List*)
_temp1016.tl; goto _LL1015;} _LL1013: return(( struct Cyc_List_List*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1026=(
char*)"norm_context: empty context"; struct _tagged_string _temp1027; _temp1027.curr=
_temp1026; _temp1027.base= _temp1026; _temp1027.last_plus_one= _temp1026 + 28;
_temp1027;})); _LL1015: return Cyc_Tcpat_augment( _temp1017,( void*)({ struct
Cyc_Tcpat_Pos_struct* _temp1028=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp1028[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp1029; _temp1029.tag= Cyc_Tcpat_Pos_tag; _temp1029.f1= _temp1024; _temp1029.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp1022);
_temp1029;}); _temp1028;})); _LL1011:;} static void* Cyc_Tcpat_build_desc(
struct Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct
_tuple5 _temp1031=({ struct _tuple5 _temp1030; _temp1030.f1= ctxt; _temp1030.f2=
work; _temp1030;}); struct Cyc_List_List* _temp1041; struct Cyc_List_List*
_temp1043; struct Cyc_List_List* _temp1045; struct Cyc_List_List* _temp1047;
struct Cyc_List_List* _temp1049; struct Cyc_List_List* _temp1051; struct Cyc_List_List*
_temp1053; struct Cyc_List_List _temp1055; struct Cyc_List_List* _temp1056;
struct _tuple9* _temp1058; struct _tuple9 _temp1060; struct Cyc_List_List*
_temp1061; struct Cyc_List_List* _temp1063; struct Cyc_List_List* _temp1065;
struct Cyc_List_List* _temp1067; struct Cyc_List_List _temp1069; struct Cyc_List_List*
_temp1070; struct _tuple10* _temp1072; struct _tuple10 _temp1074; struct Cyc_List_List*
_temp1075; struct Cyc_Tcpat_Con_s* _temp1077; _LL1033: _LL1044: _temp1043=
_temp1031.f1; if( _temp1043 == 0){ goto _LL1042;} else{ goto _LL1035;} _LL1042:
_temp1041= _temp1031.f2; if( _temp1041 == 0){ goto _LL1034;} else{ goto _LL1035;}
_LL1035: _LL1048: _temp1047= _temp1031.f1; if( _temp1047 == 0){ goto _LL1046;}
else{ goto _LL1037;} _LL1046: _temp1045= _temp1031.f2; goto _LL1036; _LL1037:
_LL1052: _temp1051= _temp1031.f1; goto _LL1050; _LL1050: _temp1049= _temp1031.f2;
if( _temp1049 == 0){ goto _LL1038;} else{ goto _LL1039;} _LL1039: _LL1068:
_temp1067= _temp1031.f1; if( _temp1067 == 0){ goto _LL1032;} else{ _temp1069=*
_temp1067; _LL1073: _temp1072=( struct _tuple10*) _temp1069.hd; _temp1074=*
_temp1072; _LL1078: _temp1077= _temp1074.f1; goto _LL1076; _LL1076: _temp1075=
_temp1074.f2; goto _LL1071; _LL1071: _temp1070=( struct Cyc_List_List*)
_temp1069.tl; goto _LL1054;} _LL1054: _temp1053= _temp1031.f2; if( _temp1053 ==
0){ goto _LL1032;} else{ _temp1055=* _temp1053; _LL1059: _temp1058=( struct
_tuple9*) _temp1055.hd; _temp1060=* _temp1058; _LL1066: _temp1065= _temp1060.f1;
goto _LL1064; _LL1064: _temp1063= _temp1060.f2; goto _LL1062; _LL1062: _temp1061=
_temp1060.f3; goto _LL1057; _LL1057: _temp1056=( struct Cyc_List_List*)
_temp1055.tl; goto _LL1040;} _LL1034: return dsc; _LL1036: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1079=( char*)"build_desc: ctxt and work don't match"; struct _tagged_string
_temp1080; _temp1080.curr= _temp1079; _temp1080.base= _temp1079; _temp1080.last_plus_one=
_temp1079 + 38; _temp1080;})); _LL1038: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1081=( char*)"build_desc: ctxt and work don't match";
struct _tagged_string _temp1082; _temp1082.curr= _temp1081; _temp1082.base=
_temp1081; _temp1082.last_plus_one= _temp1081 + 38; _temp1082;})); _LL1040: {
struct Cyc_Tcpat_Pos_struct* td=({ struct Cyc_Tcpat_Pos_struct* _temp1083=(
struct Cyc_Tcpat_Pos_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Pos_struct));
_temp1083[ 0]=({ struct Cyc_Tcpat_Pos_struct _temp1084; _temp1084.tag= Cyc_Tcpat_Pos_tag;
_temp1084.f1= _temp1077; _temp1084.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp1075),({ struct Cyc_List_List*
_temp1085=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1085->hd=( void*) dsc; _temp1085->tl= _temp1061; _temp1085;})); _temp1084;});
_temp1083;}); return Cyc_Tcpat_build_desc( _temp1070,( void*) td, _temp1056);}
_LL1032:;} static void* Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj,
void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp1086=
allmrules; struct Cyc_List_List _temp1092; struct Cyc_List_List* _temp1093;
struct _tuple0* _temp1095; struct _tuple0 _temp1097; void* _temp1098; void*
_temp1100; _LL1088: if( _temp1086 == 0){ goto _LL1089;} else{ goto _LL1090;}
_LL1090: if( _temp1086 == 0){ goto _LL1087;} else{ _temp1092=* _temp1086;
_LL1096: _temp1095=( struct _tuple0*) _temp1092.hd; _temp1097=* _temp1095;
_LL1101: _temp1100= _temp1097.f1; goto _LL1099; _LL1099: _temp1098= _temp1097.f2;
goto _LL1094; _LL1094: _temp1093=( struct Cyc_List_List*) _temp1092.tl; goto
_LL1091;} _LL1089: return( void*) Cyc_Tcpat_Failure; _LL1091: return(( void*(*)(
void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct
Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)(
_temp1100, 0, dsc, 0, 0, _temp1098, _temp1093); _LL1087:;} static void* Cyc_Tcpat_match_compile(
struct Cyc_List_List* allmrules){ return(( void*(*)( void* dsc, struct Cyc_List_List*
allmrules)) Cyc_Tcpat_or_match)(( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp1102=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp1102[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp1103; _temp1103.tag= Cyc_Tcpat_Neg_tag;
_temp1103.f1= Cyc_Tcpat_empty_con_set(); _temp1103;}); _temp1102;}), allmrules);}
static void* Cyc_Tcpat_and_match( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules){ struct Cyc_List_List*
_temp1104= work; struct Cyc_List_List _temp1112; struct Cyc_List_List* _temp1113;
struct _tuple9* _temp1115; struct _tuple9 _temp1117; struct Cyc_List_List*
_temp1118; struct Cyc_List_List* _temp1120; struct Cyc_List_List* _temp1122;
struct Cyc_List_List _temp1124; struct Cyc_List_List* _temp1125; struct _tuple9*
_temp1127; struct _tuple9 _temp1129; struct Cyc_List_List* _temp1130; struct Cyc_List_List*
_temp1132; struct Cyc_List_List* _temp1134; _LL1106: if( _temp1104 == 0){ goto
_LL1107;} else{ goto _LL1108;} _LL1108: if( _temp1104 == 0){ goto _LL1110;}
else{ _temp1112=* _temp1104; _LL1116: _temp1115=( struct _tuple9*) _temp1112.hd;
_temp1117=* _temp1115; _LL1123: _temp1122= _temp1117.f1; if( _temp1122 == 0){
goto _LL1121;} else{ goto _LL1110;} _LL1121: _temp1120= _temp1117.f2; if(
_temp1120 == 0){ goto _LL1119;} else{ goto _LL1110;} _LL1119: _temp1118=
_temp1117.f3; if( _temp1118 == 0){ goto _LL1114;} else{ goto _LL1110;} _LL1114:
_temp1113=( struct Cyc_List_List*) _temp1112.tl; goto _LL1109;} _LL1110: if(
_temp1104 == 0){ goto _LL1105;} else{ _temp1124=* _temp1104; _LL1128: _temp1127=(
struct _tuple9*) _temp1124.hd; _temp1129=* _temp1127; _LL1135: _temp1134=
_temp1129.f1; goto _LL1133; _LL1133: _temp1132= _temp1129.f2; goto _LL1131;
_LL1131: _temp1130= _temp1129.f3; goto _LL1126; _LL1126: _temp1125=( struct Cyc_List_List*)
_temp1124.tl; goto _LL1111;} _LL1107: return( void*)({ struct Cyc_Tcpat_Success_struct*
_temp1136=( struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp1136[ 0]=({ struct Cyc_Tcpat_Success_struct _temp1137; _temp1137.tag= Cyc_Tcpat_Success_tag;
_temp1137.f1=( void*) right_hand_side; _temp1137;}); _temp1136;}); _LL1109:
return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context(
ctx), _temp1113, right_hand_side, rules); _LL1111: if(( _temp1134 == 0? 1:
_temp1132 == 0)? 1: _temp1130 == 0){ return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1138=( char*)"tcpat:and_match: malformed work frame";
struct _tagged_string _temp1139; _temp1139.curr= _temp1138; _temp1139.base=
_temp1138; _temp1139.last_plus_one= _temp1138 + 38; _temp1139;}));}{ struct Cyc_List_List
_temp1143; struct Cyc_List_List* _temp1144; void* _temp1146; struct Cyc_List_List*
_temp1141=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1140= _temp1134;
if( _temp1140 == 0){ _throw( Null_Exception);} _temp1140;}); _temp1143=*
_temp1141; _LL1147: _temp1146=( void*) _temp1143.hd; goto _LL1145; _LL1145:
_temp1144=( struct Cyc_List_List*) _temp1143.tl; goto _LL1142; _LL1142: { struct
Cyc_List_List _temp1151; struct Cyc_List_List* _temp1152; struct Cyc_List_List*
_temp1154; struct Cyc_List_List* _temp1149=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1148= _temp1132; if( _temp1148 == 0){ _throw( Null_Exception);} _temp1148;});
_temp1151=* _temp1149; _LL1155: _temp1154=( struct Cyc_List_List*) _temp1151.hd;
goto _LL1153; _LL1153: _temp1152=( struct Cyc_List_List*) _temp1151.tl; goto
_LL1150; _LL1150: { struct Cyc_List_List _temp1159; struct Cyc_List_List*
_temp1160; void* _temp1162; struct Cyc_List_List* _temp1157=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1156= _temp1130; if( _temp1156 == 0){ _throw(
Null_Exception);} _temp1156;}); _temp1159=* _temp1157; _LL1163: _temp1162=( void*)
_temp1159.hd; goto _LL1161; _LL1161: _temp1160=( struct Cyc_List_List*)
_temp1159.tl; goto _LL1158; _LL1158: { struct _tuple9* wf=({ struct _tuple9*
_temp1165=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp1165->f1=
_temp1144; _temp1165->f2= _temp1152; _temp1165->f3= _temp1160; _temp1165;});
return(( void*(*)( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_match)( _temp1146, _temp1154, _temp1162, ctx,({ struct Cyc_List_List*
_temp1164=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1164->hd=( void*) wf; _temp1164->tl= _temp1125; _temp1164;}),
right_hand_side, rules);}}}} _LL1105:;} static struct Cyc_List_List* Cyc_Tcpat_getdargs(
struct Cyc_Tcpat_Con_s* pcon, void* dsc){ void* _temp1166= dsc; struct Cyc_Set_Set*
_temp1172; struct Cyc_List_List* _temp1174; struct Cyc_Tcpat_Con_s* _temp1176;
_LL1168: if((( struct _tunion_struct*) _temp1166)->tag == Cyc_Tcpat_Neg_tag){
_LL1173: _temp1172=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*)
_temp1166)->f1; goto _LL1169;} else{ goto _LL1170;} _LL1170: if((( struct
_tunion_struct*) _temp1166)->tag == Cyc_Tcpat_Pos_tag){ _LL1177: _temp1176=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp1166)->f1; goto
_LL1175; _LL1175: _temp1174=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp1166)->f2; goto _LL1171;} else{ goto _LL1167;} _LL1169: { void* any=( void*)({
struct Cyc_Tcpat_Neg_struct* _temp1179=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp1179[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp1180; _temp1180.tag= Cyc_Tcpat_Neg_tag; _temp1180.f1= Cyc_Tcpat_empty_con_set();
_temp1180;}); _temp1179;}); struct Cyc_List_List* res= 0;{ int i= 0; for( 0; i <
pcon->arity; ++ i){ res=({ struct Cyc_List_List* _temp1178=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1178->hd=( void*) any; _temp1178->tl=
res; _temp1178;});}} return res;} _LL1171: return _temp1174; _LL1167:;} static
struct Cyc_List_List* Cyc_Tcpat_getoarg( struct Cyc_List_List* obj, int i){
return({ struct Cyc_List_List* _temp1181=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1181->hd=( void*)( i + 1); _temp1181->tl=
obj; _temp1181;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s*
pcon, struct Cyc_List_List* obj){ return(( struct Cyc_List_List*(*)( int n,
struct Cyc_List_List*(* f)( struct Cyc_List_List*, int), struct Cyc_List_List*
env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static void*
Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp1182= p; struct Cyc_List_List* _temp1188; struct Cyc_Tcpat_Con_s*
_temp1190; _LL1184: if(( int) _temp1182 == Cyc_Tcpat_Any){ goto _LL1185;} else{
goto _LL1186;} _LL1186: if(( unsigned int) _temp1182 > 1u?(( struct
_tunion_struct*) _temp1182)->tag == Cyc_Tcpat_Con_tag: 0){ _LL1191: _temp1190=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Con_struct*) _temp1182)->f1; goto
_LL1189; _LL1189: _temp1188=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Con_struct*)
_temp1182)->f2; goto _LL1187;} else{ goto _LL1183;} _LL1185: return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc),
work, right_hand_side, rules); _LL1187: { void* _temp1192= Cyc_Tcpat_static_match(
_temp1190, dsc); _LL1194: if(( int) _temp1192 == Cyc_Tcpat_Yes){ goto _LL1195;}
else{ goto _LL1196;} _LL1196: if(( int) _temp1192 == Cyc_Tcpat_No){ goto _LL1197;}
else{ goto _LL1198;} _LL1198: if(( int) _temp1192 == Cyc_Tcpat_Maybe){ goto
_LL1199;} else{ goto _LL1193;} _LL1195: { struct Cyc_List_List* ctx2=({ struct
Cyc_List_List* _temp1202=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1202->hd=( void*)({ struct _tuple10* _temp1203=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1203->f1= _temp1190; _temp1203->f2= 0;
_temp1203;}); _temp1202->tl= ctx; _temp1202;}); struct _tuple9* work_frame=({
struct _tuple9* _temp1201=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp1201->f1= _temp1188; _temp1201->f2= Cyc_Tcpat_getoargs( _temp1190, obj);
_temp1201->f3= Cyc_Tcpat_getdargs( _temp1190, dsc); _temp1201;}); struct Cyc_List_List*
work2=({ struct Cyc_List_List* _temp1200=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1200->hd=( void*) work_frame; _temp1200->tl=
work; _temp1200;}); return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules);} _LL1197: return(( void*(*)( void* dsc,
struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx,
dsc, work), rules); _LL1199: { struct Cyc_List_List* ctx2=({ struct Cyc_List_List*
_temp1208=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1208->hd=( void*)({ struct _tuple10* _temp1209=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1209->f1= _temp1190; _temp1209->f2= 0;
_temp1209;}); _temp1208->tl= ctx; _temp1208;}); struct _tuple9* work_frame=({
struct _tuple9* _temp1207=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp1207->f1= _temp1188; _temp1207->f2= Cyc_Tcpat_getoargs( _temp1190, obj);
_temp1207->f3= Cyc_Tcpat_getdargs( _temp1190, dsc); _temp1207;}); struct Cyc_List_List*
work2=({ struct Cyc_List_List* _temp1206=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1206->hd=( void*) work_frame; _temp1206->tl=
work; _temp1206;}); void* s=(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules); void* f=(( void*(*)( void* dsc, struct Cyc_List_List*
allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg(
dsc, _temp1190), work), rules); return( void*)({ struct Cyc_Tcpat_IfEq_struct*
_temp1204=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp1204[ 0]=({ struct Cyc_Tcpat_IfEq_struct _temp1205; _temp1205.tag= Cyc_Tcpat_IfEq_tag;
_temp1205.f1= obj; _temp1205.f2= _temp1190; _temp1205.f3=( void*) s; _temp1205.f4=(
void*) f; _temp1205;}); _temp1204;});} _LL1193:;} _LL1183:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp1210= d; void* _temp1218; void* _temp1220; void*
_temp1222; struct Cyc_Tcpat_Con_s* _temp1224; struct Cyc_List_List* _temp1226;
_LL1212: if(( int) _temp1210 == Cyc_Tcpat_Failure){ goto _LL1213;} else{ goto
_LL1214;} _LL1214: if(( unsigned int) _temp1210 > 1u?(( struct _tunion_struct*)
_temp1210)->tag == Cyc_Tcpat_Success_tag: 0){ _LL1219: _temp1218=( void*)((
struct Cyc_Tcpat_Success_struct*) _temp1210)->f1; goto _LL1215;} else{ goto
_LL1216;} _LL1216: if(( unsigned int) _temp1210 > 1u?(( struct _tunion_struct*)
_temp1210)->tag == Cyc_Tcpat_IfEq_tag: 0){ _LL1227: _temp1226=( struct Cyc_List_List*)((
struct Cyc_Tcpat_IfEq_struct*) _temp1210)->f1; goto _LL1225; _LL1225: _temp1224=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1210)->f2; goto
_LL1223; _LL1223: _temp1222=( void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1210)->f3;
goto _LL1221; _LL1221: _temp1220=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1210)->f4; goto _LL1217;} else{ goto _LL1211;} _LL1213: not_exhaust( env1);
goto _LL1211; _LL1215: rhs_appears( env2, _temp1218); goto _LL1211; _LL1217:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1222,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1220, not_exhaust, env1, rhs_appears, env2); goto _LL1211; _LL1211:;}
struct _tuple11{ int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple12{
void* f1; struct _tuple11* f2; } ; static struct _tuple12* Cyc_Tcpat_get_match(
int* r, struct Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat(
swc->pattern); struct _tuple11* rhs=({ struct _tuple11* _temp1233=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1233->f1= 0; _temp1233->f2=(
swc->pattern)->loc; _temp1233;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp1228=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1228->hd=( void*) sp0; _temp1228->tl=({ struct Cyc_List_List*
_temp1229=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1229->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp1229->tl= 0; _temp1229;});
_temp1228;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1230=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1230->hd=( void*) sp0; _temp1230->tl=({ struct Cyc_List_List* _temp1231=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1231->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp1231->tl= 0; _temp1231;}); _temp1230;}));}
return({ struct _tuple12* _temp1232=( struct _tuple12*) GC_malloc( sizeof(
struct _tuple12)); _temp1232->f1= sp; _temp1232->f2= rhs; _temp1232;});} static
void Cyc_Tcpat_not_exhaust_err( struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp1234=( char*)"patterns may not be exhaustive";
struct _tagged_string _temp1235; _temp1235.curr= _temp1234; _temp1235.base=
_temp1234; _temp1235.last_plus_one= _temp1234 + 31; _temp1235;}));} static void
Cyc_Tcpat_rule_occurs( int dummy, struct _tuple11* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int where_ctr= 0;
struct Cyc_List_List* match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(*
f)( int*, struct Cyc_Absyn_Switch_clause*), int* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Tcpat_get_match,& where_ctr, swcs); void* dec_tree=(( void*(*)(
struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)( match_rules);(( void(*)(
void* d, void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple11*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
dec_tree, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
match_rules != 0; match_rules=({ struct Cyc_List_List* _temp1236= match_rules;
if( _temp1236 == 0){ _throw( Null_Exception);} _temp1236->tl;})){ struct
_tuple12 _temp1240; struct _tuple11* _temp1241; struct _tuple11 _temp1243;
struct Cyc_Position_Segment* _temp1244; int _temp1246; void* _temp1248; struct
_tuple12* _temp1238=( struct _tuple12*)({ struct Cyc_List_List* _temp1237=
match_rules; if( _temp1237 == 0){ _throw( Null_Exception);} _temp1237->hd;});
_temp1240=* _temp1238; _LL1249: _temp1248= _temp1240.f1; goto _LL1242; _LL1242:
_temp1241= _temp1240.f2; _temp1243=* _temp1241; _LL1247: _temp1246= _temp1243.f1;
goto _LL1245; _LL1245: _temp1244= _temp1243.f2; goto _LL1239; _LL1239: if( !
_temp1246){ Cyc_Tcutil_terr( _temp1244,( struct _tagged_string)({ char*
_temp1250=( char*)"redundant pattern"; struct _tagged_string _temp1251;
_temp1251.curr= _temp1250; _temp1251.base= _temp1250; _temp1251.last_plus_one=
_temp1250 + 18; _temp1251;}));}}} struct _tuple13{ struct Cyc_Position_Segment*
f1; int f2; } ; static void Cyc_Tcpat_not_exhaust_warn( struct _tuple13* pr){(*
pr).f2= 0; Cyc_Tcutil_warn((* pr).f1,( struct _tagged_string)({ char* _temp1252=(
char*)"pattern not exhaustive"; struct _tagged_string _temp1253; _temp1253.curr=
_temp1252; _temp1253.base= _temp1252; _temp1253.last_plus_one= _temp1252 + 23;
_temp1253;}));} static void Cyc_Tcpat_dummy_fn( int i, int j){ return;} struct
_tuple14{ void* f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive( struct
Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
match_rules=({ struct Cyc_List_List* _temp1255=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1255->hd=( void*)({ struct
_tuple14* _temp1256=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp1256->f1= Cyc_Tcpat_compile_pat( p); _temp1256->f2= 0; _temp1256;});
_temp1255->tl= 0; _temp1255;}); void* dec_tree=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( match_rules); struct _tuple13* exhaust_env=({
struct _tuple13* _temp1254=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp1254->f1= loc; _temp1254->f2= 1; _temp1254;});(( void(*)( void*
d, void(* not_exhaust)( struct _tuple13*), struct _tuple13* env1, void(*
rhs_appears)( int, int), int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree,
Cyc_Tcpat_not_exhaust_warn, exhaust_env, Cyc_Tcpat_dummy_fn, 0); return(*
exhaust_env).f2;} static struct _tuple12* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({
struct _tuple11* _temp1258=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11)); _temp1258->f1= 0; _temp1258->f2=( swc->pattern)->loc; _temp1258;});
return({ struct _tuple12* _temp1257=( struct _tuple12*) GC_malloc( sizeof(
struct _tuple12)); _temp1257->f1= sp0; _temp1257->f2= rhs; _temp1257;});} static
void Cyc_Tcpat_not_exhaust_err2( struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple11*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=({ struct
Cyc_List_List* _temp1259= match_rules; if( _temp1259 == 0){ _throw(
Null_Exception);} _temp1259->tl;})){ struct _tuple12 _temp1263; struct _tuple11*
_temp1264; struct _tuple11 _temp1266; struct Cyc_Position_Segment* _temp1267;
int _temp1269; void* _temp1271; struct _tuple12* _temp1261=( struct _tuple12*)({
struct Cyc_List_List* _temp1260= match_rules; if( _temp1260 == 0){ _throw(
Null_Exception);} _temp1260->hd;}); _temp1263=* _temp1261; _LL1272: _temp1271=
_temp1263.f1; goto _LL1265; _LL1265: _temp1264= _temp1263.f2; _temp1266=*
_temp1264; _LL1270: _temp1269= _temp1266.f1; goto _LL1268; _LL1268: _temp1267=
_temp1266.f2; goto _LL1262; _LL1262: if( ! _temp1269){ Cyc_Tcutil_terr(
_temp1267,( struct _tagged_string)({ char* _temp1273=( char*)"redundant pattern";
struct _tagged_string _temp1274; _temp1274.curr= _temp1273; _temp1274.base=
_temp1273; _temp1274.last_plus_one= _temp1273 + 18; _temp1274;}));}}}