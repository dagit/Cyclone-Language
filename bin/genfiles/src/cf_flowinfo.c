#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ void*
f1; void* f2; } ; struct _tuple4{ struct Cyc_Absyn_Tqual* f1; void* f2; } ;
struct _tuple5{ void* f1; struct Cyc_List_List* f2; } ; struct _tuple6{ struct
Cyc_Dict_Dict* f1; struct Cyc_List_List* f2; } ; struct _tuple7{ struct Cyc_Dict_Dict*
f1; struct Cyc_Set_Set** f2; } ; typedef int Cyc_ptrdiff_t; typedef unsigned int
Cyc_size_t; typedef unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t;
typedef char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef
unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef unsigned short
Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec it_value; }
; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t; typedef unsigned int Cyc_ino_t;
typedef unsigned int Cyc_vm_offset_t; typedef unsigned int Cyc_vm_size_t;
typedef char Cyc_int8_t; typedef char Cyc_u_int8_t; typedef short Cyc_int16_t;
typedef unsigned short Cyc_u_int16_t; typedef int Cyc_int32_t; typedef
unsigned int Cyc_u_int32_t; typedef long long Cyc_int64_t; typedef
unsigned long long Cyc_u_int64_t; typedef int Cyc_register_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef char* Cyc_addr_t; typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t;
typedef int Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ;
typedef struct Cyc__types_fd_set Cyc__types_fd_set; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern int Cyc_Core_intcmp( int, int);
extern int Cyc_Core_ptrcmp( void**, void**); extern char Cyc_Core_InvalidArg_tag[
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
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[ 11u];
struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_tl( struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; extern int Cyc_List_mem( int(*
compare)( void*, void*), struct Cyc_List_List* l, void* x); extern int Cyc_List_list_cmp(
int(* cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern struct
Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern struct
Cyc_Set_Set* Cyc_Set_delete( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_is_empty(
struct Cyc_Set_Set* s); extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{
char* tag; } ; extern void* Cyc_Set_choose( struct Cyc_Set_Set* s); struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{
char* tag; } ; extern char Cyc_Dict_Absent_tag[ 7u]; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern void Cyc_Dict_iter_c( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map_c( void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern
int Cyc_Dict_forall_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern int Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); struct Cyc_Stdio___sFILE; typedef
struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t; extern
char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
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
Cyc_Absyn_Vardecl{ void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual* tq;
void* type; struct Cyc_Absyn_Exp* initializer; int shadow; struct Cyc_Core_Opt*
rgn; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc;
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
tag; } ; extern void* Cyc_Tcutil_compress( void* t); typedef void* Cyc_CfFlowInfo_LocalRoot;
typedef void* Cyc_CfFlowInfo_Field; struct Cyc_CfFlowInfo_Place; typedef void*
Cyc_CfFlowInfo_Escaped; typedef void* Cyc_CfFlowInfo_InitLevel; typedef void*
Cyc_CfFlowInfo_InitState; typedef void* Cyc_CfFlowInfo_PathInfo; typedef void*
Cyc_CfFlowInfo_FlowInfo; typedef void* Cyc_CfFlowInfo_local_root_t; typedef void*
Cyc_CfFlowInfo_field_t; typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t;
typedef void* Cyc_CfFlowInfo_escaped_t; typedef void* Cyc_CfFlowInfo_init_level_t;
typedef void* Cyc_CfFlowInfo_init_state_t; typedef void* Cyc_CfFlowInfo_path_info_t;
typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t; typedef void* Cyc_CfFlowInfo_flow_info_t;
static const int Cyc_CfFlowInfo_VarRoot_tag= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt_tag=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_CfFlowInfo_StructF_tag= 0; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_string* f1; } ; static const int Cyc_CfFlowInfo_TupleF_tag=
1; struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields; } ; static const unsigned int Cyc_CfFlowInfo_Esc=
0; static const unsigned int Cyc_CfFlowInfo_Unesc= 1; static const unsigned int
Cyc_CfFlowInfo_NoneIL= 0; static const unsigned int Cyc_CfFlowInfo_ThisIL= 1;
static const unsigned int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS_tag=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo_tag= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI_tag=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI_tag= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag;
struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI_tag= 2;
struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const unsigned int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL_tag=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void* Cyc_CfFlowInfo_mkLeafPI( void* esc, void* il); extern int Cyc_CfFlowInfo_local_root_cmp(
void*, void*); extern int Cyc_CfFlowInfo_field_cmp( void*, void*); extern int
Cyc_CfFlowInfo_place_cmp( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*);
extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t); extern void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place, void* pinfo); extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto(
void* pinfo, struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict(
void*, void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int
Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2); static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_unes_noneil={ 0u,( void*)(( void*) 1u),( void*)(( void*)
0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_thisil={
0u,( void*)(( void*) 1u),( void*)(( void*) 1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_unes_allil={ 0u,( void*)(( void*) 1u),( void*)(( void*)
2u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_esc_noneil={
0u,( void*)(( void*) 0u),( void*)(( void*) 0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_esc_thisil={ 0u,( void*)(( void*) 0u),( void*)(( void*)
1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_esc_allil={
0u,( void*)(( void*) 0u),( void*)(( void*) 2u)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_unesc_none_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_noneil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_this_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_thisil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_unesc_all_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_allil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_none_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_noneil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_esc_this_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_thisil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_all_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_allil)}; void* Cyc_CfFlowInfo_leaf_unesc_none=(
void*)& Cyc_CfFlowInfo_leaf_unesc_none_v; void* Cyc_CfFlowInfo_leaf_unesc_this=(
void*)& Cyc_CfFlowInfo_leaf_unesc_this_v; void* Cyc_CfFlowInfo_leaf_unesc_all=(
void*)& Cyc_CfFlowInfo_leaf_unesc_all_v; void* Cyc_CfFlowInfo_leaf_esc_none=(
void*)& Cyc_CfFlowInfo_leaf_esc_none_v; void* Cyc_CfFlowInfo_leaf_esc_this=(
void*)& Cyc_CfFlowInfo_leaf_esc_this_v; void* Cyc_CfFlowInfo_leaf_esc_all=( void*)&
Cyc_CfFlowInfo_leaf_esc_all_v; void* Cyc_CfFlowInfo_mkLeafPI( void* esc, void*
il){ struct _tuple3 _temp13=({ struct _tuple3 _temp12; _temp12.f1= esc; _temp12.f2=
il; _temp12;}); void* _temp27; void* _temp29; void* _temp31; void* _temp33; void*
_temp35; void* _temp37; void* _temp39; void* _temp41; void* _temp43; void*
_temp45; void* _temp47; void* _temp49; _LL15: _LL30: _temp29= _temp13.f1; if((
int) _temp29 == Cyc_CfFlowInfo_Unesc){ goto _LL28;} else{ goto _LL17;} _LL28:
_temp27= _temp13.f2; if(( int) _temp27 == Cyc_CfFlowInfo_NoneIL){ goto _LL16;}
else{ goto _LL17;} _LL17: _LL34: _temp33= _temp13.f1; if(( int) _temp33 == Cyc_CfFlowInfo_Unesc){
goto _LL32;} else{ goto _LL19;} _LL32: _temp31= _temp13.f2; if(( int) _temp31 ==
Cyc_CfFlowInfo_ThisIL){ goto _LL18;} else{ goto _LL19;} _LL19: _LL38: _temp37=
_temp13.f1; if(( int) _temp37 == Cyc_CfFlowInfo_Unesc){ goto _LL36;} else{ goto
_LL21;} _LL36: _temp35= _temp13.f2; if(( int) _temp35 == Cyc_CfFlowInfo_AllIL){
goto _LL20;} else{ goto _LL21;} _LL21: _LL42: _temp41= _temp13.f1; if(( int)
_temp41 == Cyc_CfFlowInfo_Esc){ goto _LL40;} else{ goto _LL23;} _LL40: _temp39=
_temp13.f2; if(( int) _temp39 == Cyc_CfFlowInfo_NoneIL){ goto _LL22;} else{ goto
_LL23;} _LL23: _LL46: _temp45= _temp13.f1; if(( int) _temp45 == Cyc_CfFlowInfo_Esc){
goto _LL44;} else{ goto _LL25;} _LL44: _temp43= _temp13.f2; if(( int) _temp43 ==
Cyc_CfFlowInfo_ThisIL){ goto _LL24;} else{ goto _LL25;} _LL25: _LL50: _temp49=
_temp13.f1; if(( int) _temp49 == Cyc_CfFlowInfo_Esc){ goto _LL48;} else{ goto
_LL14;} _LL48: _temp47= _temp13.f2; if(( int) _temp47 == Cyc_CfFlowInfo_AllIL){
goto _LL26;} else{ goto _LL14;} _LL16: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL18: return Cyc_CfFlowInfo_leaf_unesc_this; _LL20: return Cyc_CfFlowInfo_leaf_unesc_all;
_LL22: return Cyc_CfFlowInfo_leaf_esc_none; _LL24: return Cyc_CfFlowInfo_leaf_esc_this;
_LL26: return Cyc_CfFlowInfo_leaf_esc_all; _LL14:;} int Cyc_CfFlowInfo_local_root_cmp(
void* r1, void* r2){ if( r1 == r2){ return 0;}{ struct _tuple3 _temp52=({ struct
_tuple3 _temp51; _temp51.f1= r1; _temp51.f2= r2; _temp51;}); void* _temp62;
struct Cyc_Absyn_Exp* _temp64; void* _temp66; struct Cyc_Absyn_Vardecl* _temp68;
void* _temp70; struct Cyc_Absyn_Vardecl* _temp72; void* _temp74; struct Cyc_Absyn_Exp*
_temp76; void* _temp78; struct Cyc_Absyn_Vardecl* _temp80; void* _temp82; struct
Cyc_Absyn_Vardecl* _temp84; void* _temp86; struct Cyc_Absyn_Exp* _temp88; void*
_temp90; struct Cyc_Absyn_Exp* _temp92; _LL54: _LL67: _temp66= _temp52.f1; if(((
struct _tunion_struct*) _temp66)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL69:
_temp68=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp66)->f1; goto _LL63;} else{ goto _LL56;} _LL63: _temp62= _temp52.f2; if(((
struct _tunion_struct*) _temp62)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL65:
_temp64=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp62)->f1; goto _LL55;} else{ goto _LL56;} _LL56: _LL75: _temp74= _temp52.f1;
if((( struct _tunion_struct*) _temp74)->tag == Cyc_CfFlowInfo_MallocPt_tag){
_LL77: _temp76=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp74)->f1; goto _LL71;} else{ goto _LL58;} _LL71: _temp70= _temp52.f2; if(((
struct _tunion_struct*) _temp70)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL73:
_temp72=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp70)->f1; goto _LL57;} else{ goto _LL58;} _LL58: _LL83: _temp82= _temp52.f1;
if((( struct _tunion_struct*) _temp82)->tag == Cyc_CfFlowInfo_VarRoot_tag){
_LL85: _temp84=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp82)->f1; goto _LL79;} else{ goto _LL60;} _LL79: _temp78= _temp52.f2; if(((
struct _tunion_struct*) _temp78)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL81:
_temp80=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp78)->f1; goto _LL59;} else{ goto _LL60;} _LL60: _LL91: _temp90= _temp52.f1;
if((( struct _tunion_struct*) _temp90)->tag == Cyc_CfFlowInfo_MallocPt_tag){
_LL93: _temp92=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp90)->f1; goto _LL87;} else{ goto _LL53;} _LL87: _temp86= _temp52.f2; if(((
struct _tunion_struct*) _temp86)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL89:
_temp88=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp86)->f1; goto _LL61;} else{ goto _LL53;} _LL55: return 1; _LL57: return - 1;
_LL59: return(( int(*)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp)( _temp84, _temp80); _LL61: return(( int(*)( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp)( _temp92, _temp88); _LL53:;}} int Cyc_CfFlowInfo_field_cmp(
void* f1, void* f2){ if( f1 == f2){ return 0;}{ struct _tuple3 _temp95=({ struct
_tuple3 _temp94; _temp94.f1= f1; _temp94.f2= f2; _temp94;}); void* _temp105; int
_temp107; void* _temp109; struct _tagged_string* _temp111; void* _temp113;
struct _tagged_string* _temp115; void* _temp117; int _temp119; void* _temp121;
struct _tagged_string* _temp123; void* _temp125; struct _tagged_string* _temp127;
void* _temp129; int _temp131; void* _temp133; int _temp135; _LL97: _LL110:
_temp109= _temp95.f1; if((( struct _tunion_struct*) _temp109)->tag == Cyc_CfFlowInfo_StructF_tag){
_LL112: _temp111=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp109)->f1; goto _LL106;} else{ goto _LL99;} _LL106: _temp105= _temp95.f2;
if((( struct _tunion_struct*) _temp105)->tag == Cyc_CfFlowInfo_TupleF_tag){
_LL108: _temp107=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp105)->f1;
goto _LL98;} else{ goto _LL99;} _LL99: _LL118: _temp117= _temp95.f1; if(((
struct _tunion_struct*) _temp117)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL120:
_temp119=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp117)->f1; goto
_LL114;} else{ goto _LL101;} _LL114: _temp113= _temp95.f2; if((( struct
_tunion_struct*) _temp113)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL116: _temp115=(
struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp113)->f1;
goto _LL100;} else{ goto _LL101;} _LL101: _LL126: _temp125= _temp95.f1; if(((
struct _tunion_struct*) _temp125)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL128:
_temp127=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp125)->f1; goto _LL122;} else{ goto _LL103;} _LL122: _temp121= _temp95.f2;
if((( struct _tunion_struct*) _temp121)->tag == Cyc_CfFlowInfo_StructF_tag){
_LL124: _temp123=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp121)->f1; goto _LL102;} else{ goto _LL103;} _LL103: _LL134: _temp133=
_temp95.f1; if((( struct _tunion_struct*) _temp133)->tag == Cyc_CfFlowInfo_TupleF_tag){
_LL136: _temp135=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp133)->f1;
goto _LL130;} else{ goto _LL96;} _LL130: _temp129= _temp95.f2; if((( struct
_tunion_struct*) _temp129)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL132: _temp131=(
int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp129)->f1; goto _LL104;} else{
goto _LL96;} _LL98: return 1; _LL100: return - 1; _LL102: return(( int(*)(
struct _tagged_string*, struct _tagged_string*)) Cyc_Core_ptrcmp)( _temp127,
_temp123); _LL104: return _temp135 - _temp131; _LL96:;}} int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place* p1, struct Cyc_CfFlowInfo_Place* p2){ if( p1 == p2){
return 0;}{ int i= Cyc_CfFlowInfo_local_root_cmp(( void*) p1->root,( void*) p2->root);
if( i != 0){ return i;} return(( int(*)( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_CfFlowInfo_field_cmp, p1->fields,
p2->fields);}} void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t){ void*
_temp137= Cyc_Tcutil_compress( t); struct Cyc_List_List* _temp145; struct Cyc_Absyn_Structdecl**
_temp147; struct Cyc_Absyn_Structdecl* _temp149; struct Cyc_Absyn_Structdecl
_temp150; struct Cyc_List_List* _temp151; struct Cyc_Core_Opt* _temp153; struct
Cyc_Core_Opt _temp155; struct Cyc_List_List* _temp156; struct Cyc_List_List*
_temp158; struct Cyc_Core_Opt* _temp160; void* _temp162; struct Cyc_List_List*
_temp164; struct _tuple0* _temp166; _LL139: if(( unsigned int) _temp137 > 4u?((
struct _tunion_struct*) _temp137)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL146:
_temp145=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp137)->f1;
goto _LL140;} else{ goto _LL141;} _LL141: if(( unsigned int) _temp137 > 4u?((
struct _tunion_struct*) _temp137)->tag == Cyc_Absyn_StructType_tag: 0){ _LL167:
_temp166=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp137)->f1;
goto _LL165; _LL165: _temp164=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp137)->f2; goto _LL148; _LL148: _temp147=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp137)->f3; if( _temp147 == 0){ goto
_LL143;} else{ _temp149=* _temp147; _temp150=* _temp149; _LL163: _temp162=( void*)
_temp150.sc; goto _LL161; _LL161: _temp160=( struct Cyc_Core_Opt*) _temp150.name;
goto _LL159; _LL159: _temp158=( struct Cyc_List_List*) _temp150.tvs; goto _LL154;
_LL154: _temp153=( struct Cyc_Core_Opt*) _temp150.fields; if( _temp153 == 0){
goto _LL143;} else{ _temp155=* _temp153; _LL157: _temp156=( struct Cyc_List_List*)
_temp155.v; goto _LL152;} _LL152: _temp151=( struct Cyc_List_List*) _temp150.attributes;
goto _LL142;}} else{ goto _LL143;} _LL143: goto _LL144; _LL140: { struct Cyc_Dict_Dict*
d=(( struct Cyc_Dict_Dict*(*)( int(* comp)( int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);{
int i= 0; for( 0; _temp145 != 0; _temp145=({ struct Cyc_List_List* _temp168=
_temp145; if( _temp168 == 0){ _throw( Null_Exception);} _temp168->tl;}), ++ i){
struct _tuple4 _temp172; void* _temp173; struct Cyc_Absyn_Tqual* _temp175;
struct _tuple4* _temp170=( struct _tuple4*)({ struct Cyc_List_List* _temp169=
_temp145; if( _temp169 == 0){ _throw( Null_Exception);} _temp169->hd;});
_temp172=* _temp170; _LL176: _temp175= _temp172.f1; goto _LL174; _LL174:
_temp173= _temp172.f2; goto _LL171; _LL171: d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, int key, void* data)) Cyc_Dict_insert)( d, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp173));}} return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp177=(
struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp177[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp178; _temp178.tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp178.f1= d; _temp178;}); _temp177;});} _LL142: { struct Cyc_Dict_Dict* d=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); for( 0; _temp156 !=
0; _temp156=({ struct Cyc_List_List* _temp179= _temp156; if( _temp179 == 0){
_throw( Null_Exception);} _temp179->tl;})){ struct Cyc_Absyn_Structfield
_temp183; struct Cyc_List_List* _temp184; struct Cyc_Core_Opt* _temp186; void*
_temp188; struct Cyc_Absyn_Tqual* _temp190; struct _tagged_string* _temp192;
struct Cyc_Absyn_Structfield* _temp181=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp180= _temp156; if( _temp180 == 0){ _throw( Null_Exception);}
_temp180->hd;}); _temp183=* _temp181; _LL193: _temp192=( struct _tagged_string*)
_temp183.name; goto _LL191; _LL191: _temp190=( struct Cyc_Absyn_Tqual*) _temp183.tq;
goto _LL189; _LL189: _temp188=( void*) _temp183.type; goto _LL187; _LL187:
_temp186=( struct Cyc_Core_Opt*) _temp183.width; goto _LL185; _LL185: _temp184=(
struct Cyc_List_List*) _temp183.attributes; goto _LL182; _LL182: if( Cyc_String_strcmp(*
_temp192,( struct _tagged_string)({ char* _temp194=( char*)""; struct
_tagged_string _temp195; _temp195.curr= _temp194; _temp195.base= _temp194;
_temp195.last_plus_one= _temp194 + 1; _temp195;})) != 0){ d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
d, _temp192, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( _temp188));}} return( void*)({
struct Cyc_CfFlowInfo_StructPI_struct* _temp196=( struct Cyc_CfFlowInfo_StructPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct)); _temp196[ 0]=({
struct Cyc_CfFlowInfo_StructPI_struct _temp197; _temp197.tag= Cyc_CfFlowInfo_StructPI_tag;
_temp197.f1= d; _temp197;}); _temp196;});} _LL144: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL138:;} void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place){ struct Cyc_CfFlowInfo_Place _temp200; struct Cyc_List_List* _temp201;
void* _temp203; struct Cyc_CfFlowInfo_Place* _temp198= place; _temp200=*
_temp198; _LL204: _temp203=( void*) _temp200.root; goto _LL202; _LL202: _temp201=(
struct Cyc_List_List*) _temp200.fields; goto _LL199; _LL199: { void* ans=(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d, _temp203); for( 0;
_temp201 != 0; _temp201=({ struct Cyc_List_List* _temp205= _temp201; if(
_temp205 == 0){ _throw( Null_Exception);} _temp205->tl;})){ struct _tuple3
_temp208=({ struct _tuple3 _temp206; _temp206.f1= ans; _temp206.f2=( void*)({
struct Cyc_List_List* _temp207= _temp201; if( _temp207 == 0){ _throw(
Null_Exception);} _temp207->hd;}); _temp206;}); void* _temp216; int _temp218;
void* _temp220; struct Cyc_Dict_Dict* _temp222; void* _temp224; struct
_tagged_string* _temp226; void* _temp228; struct Cyc_Dict_Dict* _temp230; _LL210:
_LL221: _temp220= _temp208.f1; if((( struct _tunion_struct*) _temp220)->tag ==
Cyc_CfFlowInfo_TuplePI_tag){ _LL223: _temp222=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp220)->f1; goto _LL217;} else{ goto _LL212;}
_LL217: _temp216= _temp208.f2; if((( struct _tunion_struct*) _temp216)->tag ==
Cyc_CfFlowInfo_TupleF_tag){ _LL219: _temp218=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp216)->f1; goto _LL211;} else{ goto _LL212;} _LL212: _LL229: _temp228=
_temp208.f1; if((( struct _tunion_struct*) _temp228)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL231: _temp230=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp228)->f1; goto _LL225;} else{ goto _LL214;} _LL225: _temp224= _temp208.f2;
if((( struct _tunion_struct*) _temp224)->tag == Cyc_CfFlowInfo_StructF_tag){
_LL227: _temp226=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp224)->f1; goto _LL213;} else{ goto _LL214;} _LL214: goto _LL215; _LL211:
ans=(( void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp222,
_temp218); goto _LL209; _LL213: ans=(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp230, _temp226); goto _LL209; _LL215:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp232=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp232[ 0]=({ struct Cyc_Core_Impossible_struct _temp233; _temp233.tag= Cyc_Core_Impossible_tag;
_temp233.f1=( struct _tagged_string)({ char* _temp234=( char*)"bad lookup_place";
struct _tagged_string _temp235; _temp235.curr= _temp234; _temp235.base= _temp234;
_temp235.last_plus_one= _temp234 + 17; _temp235;}); _temp233;}); _temp232;}));
_LL209:;} return ans;}} void* Cyc_CfFlowInfo_insert_place_rec( void* insert_path,
struct Cyc_List_List* fields, void* pinfo){ if( fields == 0){ return pinfo;}{
struct _tuple5 _temp237=({ struct _tuple5 _temp236; _temp236.f1= insert_path;
_temp236.f2= fields; _temp236;}); struct Cyc_List_List* _temp245; struct Cyc_List_List
_temp247; struct Cyc_List_List* _temp248; void* _temp250; int _temp252; void*
_temp254; struct Cyc_Dict_Dict* _temp256; struct Cyc_List_List* _temp258; struct
Cyc_List_List _temp260; struct Cyc_List_List* _temp261; void* _temp263; struct
_tagged_string* _temp265; void* _temp267; struct Cyc_Dict_Dict* _temp269; _LL239:
_LL255: _temp254= _temp237.f1; if((( struct _tunion_struct*) _temp254)->tag ==
Cyc_CfFlowInfo_TuplePI_tag){ _LL257: _temp256=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp254)->f1; goto _LL246;} else{ goto _LL241;}
_LL246: _temp245= _temp237.f2; if( _temp245 == 0){ goto _LL241;} else{ _temp247=*
_temp245; _LL251: _temp250=( void*) _temp247.hd; if((( struct _tunion_struct*)
_temp250)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL253: _temp252=( int)(( struct
Cyc_CfFlowInfo_TupleF_struct*) _temp250)->f1; goto _LL249;} else{ goto _LL241;}
_LL249: _temp248=( struct Cyc_List_List*) _temp247.tl; goto _LL240;} _LL241:
_LL268: _temp267= _temp237.f1; if((( struct _tunion_struct*) _temp267)->tag ==
Cyc_CfFlowInfo_StructPI_tag){ _LL270: _temp269=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_StructPI_struct*) _temp267)->f1; goto _LL259;} else{ goto _LL243;}
_LL259: _temp258= _temp237.f2; if( _temp258 == 0){ goto _LL243;} else{ _temp260=*
_temp258; _LL264: _temp263=( void*) _temp260.hd; if((( struct _tunion_struct*)
_temp263)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL266: _temp265=( struct
_tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp263)->f1; goto
_LL262;} else{ goto _LL243;} _LL262: _temp261=( struct Cyc_List_List*) _temp260.tl;
goto _LL242;} _LL243: goto _LL244; _LL240: { void* next_path=(( void*(*)( struct
Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp256, _temp252); return( void*)({
struct Cyc_CfFlowInfo_TuplePI_struct* _temp271=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp271[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp272; _temp272.tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp272.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void*
data)) Cyc_Dict_insert)( _temp256, _temp252, Cyc_CfFlowInfo_insert_place_rec(
next_path, _temp248, pinfo)); _temp272;}); _temp271;});} _LL242: { void*
next_path=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
_temp269, _temp265); return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp273=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp273[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp274; _temp274.tag=
Cyc_CfFlowInfo_StructPI_tag; _temp274.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( _temp269, _temp265,
Cyc_CfFlowInfo_insert_place_rec( next_path, _temp261, pinfo)); _temp274;});
_temp273;});} _LL244:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp275=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp275[ 0]=({ struct Cyc_Core_Impossible_struct _temp276; _temp276.tag= Cyc_Core_Impossible_tag;
_temp276.f1=( struct _tagged_string)({ char* _temp277=( char*)"bad insert_place";
struct _tagged_string _temp278; _temp278.curr= _temp277; _temp278.base= _temp277;
_temp278.last_plus_one= _temp277 + 17; _temp278;}); _temp276;}); _temp275;}));
_LL238:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict*
d, struct Cyc_CfFlowInfo_Place* place, void* pinfo){ struct Cyc_CfFlowInfo_Place
_temp281; struct Cyc_List_List* _temp282; void* _temp284; struct Cyc_CfFlowInfo_Place*
_temp279= place; _temp281=* _temp279; _LL285: _temp284=( void*) _temp281.root;
goto _LL283; _LL283: _temp282=( struct Cyc_List_List*) _temp281.fields; goto
_LL280; _LL280: return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void*
key, void* data)) Cyc_Dict_insert)( d, _temp284, Cyc_CfFlowInfo_insert_place_rec(((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d, _temp284),
_temp282, pinfo));} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env,
void* path); int Cyc_CfFlowInfo_isAllInit_is( struct _tuple6* env, void* state){
void* _temp286= state; void* _temp294; void* _temp296; void* _temp298; void*
_temp300; struct Cyc_CfFlowInfo_Place* _temp302; _LL288: if((( struct
_tunion_struct*) _temp286)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL297:
_temp296=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp286)->f1; goto
_LL295; _LL295: _temp294=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp286)->f2; if(( int) _temp294 == Cyc_CfFlowInfo_AllIL){ goto _LL289;} else{
goto _LL290;}} else{ goto _LL290;} _LL290: if((( struct _tunion_struct*)
_temp286)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL301: _temp300=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp286)->f1; goto _LL299; _LL299:
_temp298=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp286)->f2; goto
_LL291;} else{ goto _LL292;} _LL292: if((( struct _tunion_struct*) _temp286)->tag
== Cyc_CfFlowInfo_MustPointTo_tag){ _LL303: _temp302=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp286)->f1; goto _LL293;} else{
goto _LL287;} _LL289: return 1; _LL291: return 0; _LL293: { struct _tuple6
_temp306; struct Cyc_List_List* _temp307; struct Cyc_List_List** _temp309;
struct Cyc_Dict_Dict* _temp310; struct _tuple6* _temp304= env; _temp306=*
_temp304; _LL311: _temp310= _temp306.f1; goto _LL308; _LL308: _temp307= _temp306.f2;
_temp309=&(* _temp304).f2; goto _LL305; _LL305: if((( int(*)( int(* compare)(
struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*), struct Cyc_List_List*
l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
_temp309, _temp302)){ return 1;}* _temp309=({ struct Cyc_List_List* _temp312=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp312[
0]=({ struct Cyc_List_List _temp313; _temp313.hd=( void*) _temp302; _temp313.tl=*
_temp309; _temp313;}); _temp312;}); return Cyc_CfFlowInfo_isAllInit_pinfo( env,
Cyc_CfFlowInfo_lookup_place( _temp310, _temp302));} _LL287:;} int Cyc_CfFlowInfo_isAllInit_f(
struct _tuple6* env, void* a, void* path){ return Cyc_CfFlowInfo_isAllInit_pinfo(
env, path);} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path){
void* _temp314= path; void* _temp322; struct Cyc_Dict_Dict* _temp324; struct Cyc_Dict_Dict*
_temp326; _LL316: if((( struct _tunion_struct*) _temp314)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL323: _temp322=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp314)->f1;
goto _LL317;} else{ goto _LL318;} _LL318: if((( struct _tunion_struct*) _temp314)->tag
== Cyc_CfFlowInfo_TuplePI_tag){ _LL325: _temp324=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp314)->f1; goto _LL319;} else{ goto
_LL320;} _LL320: if((( struct _tunion_struct*) _temp314)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL327: _temp326=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp314)->f1; goto _LL321;} else{ goto _LL315;} _LL317: return Cyc_CfFlowInfo_isAllInit_is(
env, _temp322); _LL319: return(( int(*)( int(* f)( struct _tuple6*, int, void*),
struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)(
struct _tuple6* env, int a, void* path)) Cyc_CfFlowInfo_isAllInit_f, env,
_temp324); _LL321: return(( int(*)( int(* f)( struct _tuple6*, struct
_tagged_string*, void*), struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)((
int(*)( struct _tuple6* env, struct _tagged_string* a, void* path)) Cyc_CfFlowInfo_isAllInit_f,
env, _temp326); _LL315:;} int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo){ struct _tuple6 isInitEnv=({ struct _tuple6 _temp328;
_temp328.f1= pinfo_dict; _temp328.f2= 0; _temp328;}); return Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv, pinfo);} void* Cyc_CfFlowInfo_pinfo_getplaces_esc( struct _tuple7*
env, void* pinfo){ struct _tuple7 _temp331; struct Cyc_Set_Set** _temp332;
struct Cyc_Dict_Dict* _temp334; struct _tuple7* _temp329= env; _temp331=*
_temp329; _LL335: _temp334= _temp331.f1; goto _LL333; _LL333: _temp332= _temp331.f2;
goto _LL330; _LL330: { void* _temp336= pinfo; void* _temp346; struct Cyc_CfFlowInfo_Place*
_temp348; void* _temp350; void* _temp352; void* _temp354; struct Cyc_Dict_Dict*
_temp356; struct Cyc_Dict_Dict* _temp358; _LL338: if((( struct _tunion_struct*)
_temp336)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL347: _temp346=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp336)->f1; if((( struct _tunion_struct*)
_temp346)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL349: _temp348=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp346)->f1; goto _LL339;} else{
goto _LL340;}} else{ goto _LL340;} _LL340: if((( struct _tunion_struct*)
_temp336)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL351: _temp350=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp336)->f1; if((( struct _tunion_struct*)
_temp350)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL355: _temp354=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp350)->f1; goto _LL353; _LL353:
_temp352=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp350)->f2; goto
_LL341;} else{ goto _LL342;}} else{ goto _LL342;} _LL342: if((( struct
_tunion_struct*) _temp336)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL357: _temp356=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp336)->f1;
goto _LL343;} else{ goto _LL344;} _LL344: if((( struct _tunion_struct*) _temp336)->tag
== Cyc_CfFlowInfo_StructPI_tag){ _LL359: _temp358=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp336)->f1; goto _LL345;} else{ goto
_LL337;} _LL339:* _temp332=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* _temp332, _temp348); if(
Cyc_CfFlowInfo_isAllInit( _temp334, Cyc_CfFlowInfo_lookup_place( _temp334,
_temp348))){ return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,( void*)
Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_ThisIL); _LL341: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Esc, _temp352); _LL343: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp360=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp360[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp361; _temp361.tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp361.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct _tuple7*, void*),
struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc,
env, _temp356); _temp361;}); _temp360;}); _LL345: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp362=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp362[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp363; _temp363.tag=
Cyc_CfFlowInfo_StructPI_tag; _temp363.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct _tuple7*, void*), struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)(
Cyc_CfFlowInfo_pinfo_getplaces_esc, env, _temp358); _temp363;}); _temp362;});
_LL337:;}} void Cyc_CfFlowInfo_pinfo_getplaces( struct Cyc_Set_Set** places_ptr,
void* a, void* pinfo){ void* _temp364= pinfo; void* _temp374; void* _temp376;
void* _temp378; void* _temp380; struct Cyc_CfFlowInfo_Place* _temp382; struct
Cyc_Dict_Dict* _temp384; struct Cyc_Dict_Dict* _temp386; _LL366: if((( struct
_tunion_struct*) _temp364)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL375: _temp374=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp364)->f1; if((( struct
_tunion_struct*) _temp374)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL379:
_temp378=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp374)->f1; goto
_LL377; _LL377: _temp376=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp374)->f2; goto _LL367;} else{ goto _LL368;}} else{ goto _LL368;} _LL368:
if((( struct _tunion_struct*) _temp364)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL381: _temp380=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp364)->f1;
if((( struct _tunion_struct*) _temp380)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL383: _temp382=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp380)->f1; goto _LL369;} else{ goto _LL370;}} else{ goto _LL370;} _LL370:
if((( struct _tunion_struct*) _temp364)->tag == Cyc_CfFlowInfo_TuplePI_tag){
_LL385: _temp384=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp364)->f1; goto _LL371;} else{ goto _LL372;} _LL372: if((( struct
_tunion_struct*) _temp364)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL387:
_temp386=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp364)->f1; goto _LL373;} else{ goto _LL365;} _LL367: return; _LL369:*
places_ptr=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place*
elt)) Cyc_Set_insert)(* places_ptr, _temp382); return; _LL371:(( void(*)( void(*
f)( struct Cyc_Set_Set**, int, void*), struct Cyc_Set_Set** env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct Cyc_Set_Set** places_ptr, int a, void*
pinfo)) Cyc_CfFlowInfo_pinfo_getplaces, places_ptr, _temp384); return; _LL373:((
void(*)( void(* f)( struct Cyc_Set_Set**, struct _tagged_string*, void*), struct
Cyc_Set_Set** env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct
Cyc_Set_Set** places_ptr, struct _tagged_string* a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces,
places_ptr, _temp386); return; _LL365:;} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_these(
struct Cyc_Set_Set* places, struct Cyc_Dict_Dict* d){ while( !(( int(*)( struct
Cyc_Set_Set* s)) Cyc_Set_is_empty)( places)) { struct Cyc_CfFlowInfo_Place*
place=(( struct Cyc_CfFlowInfo_Place*(*)( struct Cyc_Set_Set* s)) Cyc_Set_choose)(
places); places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place*
elt)) Cyc_Set_delete)( places, place);{ void* place_old_pinfo;{ struct
_handler_cons _temp388; _push_handler(& _temp388);{ struct _xtunion_struct*
_temp389=( struct _xtunion_struct*) setjmp( _temp388.handler); if( ! _temp389){
place_old_pinfo= Cyc_CfFlowInfo_lookup_place( d, place);; _pop_handler();} else{
struct _xtunion_struct* _temp391= _temp389; _LL393: if( _temp391->tag == Cyc_Dict_Absent_tag){
goto _LL394;} else{ goto _LL395;} _LL395: goto _LL396; _LL394: continue; _LL396:(
void) _throw( _temp391); _LL392:;}}}{ struct _tuple7 places_env=({ struct
_tuple7 _temp397; _temp397.f1= d; _temp397.f2=& places; _temp397;}); void*
place_new_pinfo= Cyc_CfFlowInfo_pinfo_getplaces_esc(& places_env,
place_old_pinfo); if( place_old_pinfo != place_new_pinfo){ d= Cyc_CfFlowInfo_insert_place(
d, place, place_new_pinfo);}}}} return d;} static struct Cyc_Set_Set** Cyc_CfFlowInfo_empty_place_set_ptr=
0; struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo, struct Cyc_Dict_Dict*
d){ if( Cyc_CfFlowInfo_empty_place_set_ptr == 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({
struct Cyc_Set_Set** _temp398=( struct Cyc_Set_Set**) GC_malloc( sizeof( struct
Cyc_Set_Set*) * 1); _temp398[ 0]=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*))) Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp);
_temp398;});}{ struct Cyc_Set_Set* places=({ struct Cyc_Set_Set** _temp399= Cyc_CfFlowInfo_empty_place_set_ptr;
if( _temp399 == 0){ _throw( Null_Exception);}* _temp399;});(( void(*)( struct
Cyc_Set_Set** places_ptr, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces)(&
places, 0, pinfo); return Cyc_CfFlowInfo_escape_these( places, d);}} void* Cyc_CfFlowInfo_assign_unknown_dict(
void* level, void* pinfo){ void* _temp400= pinfo; void* _temp410; void* _temp412;
void* _temp414; void* _temp416; struct Cyc_Dict_Dict* _temp418; struct Cyc_Dict_Dict*
_temp420; _LL402: if((( struct _tunion_struct*) _temp400)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL411: _temp410=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp400)->f1;
if((( struct _tunion_struct*) _temp410)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL415: _temp414=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp410)->f1;
if(( int) _temp414 == Cyc_CfFlowInfo_Esc){ goto _LL413;} else{ goto _LL404;}
_LL413: _temp412=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp410)->f2;
goto _LL403;} else{ goto _LL404;}} else{ goto _LL404;} _LL404: if((( struct
_tunion_struct*) _temp400)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL417: _temp416=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp400)->f1; goto _LL405;}
else{ goto _LL406;} _LL406: if((( struct _tunion_struct*) _temp400)->tag == Cyc_CfFlowInfo_TuplePI_tag){
_LL419: _temp418=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp400)->f1; goto _LL407;} else{ goto _LL408;} _LL408: if((( struct
_tunion_struct*) _temp400)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL421:
_temp420=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp400)->f1; goto _LL409;} else{ goto _LL401;} _LL403: return Cyc_CfFlowInfo_mkLeafPI((
void*) Cyc_CfFlowInfo_Esc, level); _LL405: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Unesc, level); _LL407: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp422=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp422[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp423; _temp423.tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp423.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp418); _temp423;}); _temp422;}); _LL409: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp424=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp424[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp425; _temp425.tag=
Cyc_CfFlowInfo_StructPI_tag; _temp425.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
void*, void*), void* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp420); _temp425;}); _temp424;}); _LL401:;} struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict* d2; struct Cyc_Set_Set** places;
} ; typedef struct Cyc_CfFlowInfo_JoinEnv* Cyc_CfFlowInfo_join_env_t; void* Cyc_CfFlowInfo_join_escape(
void* esc1, void* esc2){ struct _tuple3 _temp427=({ struct _tuple3 _temp426;
_temp426.f1= esc1; _temp426.f2= esc2; _temp426;}); void* _temp435; void*
_temp437; void* _temp439; void* _temp441; _LL429: _LL438: _temp437= _temp427.f1;
if(( int) _temp437 == Cyc_CfFlowInfo_Esc){ goto _LL436;} else{ goto _LL431;}
_LL436: _temp435= _temp427.f2; goto _LL430; _LL431: _LL442: _temp441= _temp427.f1;
goto _LL440; _LL440: _temp439= _temp427.f2; if(( int) _temp439 == Cyc_CfFlowInfo_Esc){
goto _LL432;} else{ goto _LL433;} _LL433: goto _LL434; _LL430: goto _LL432;
_LL432: return( void*) Cyc_CfFlowInfo_Esc; _LL434: return( void*) Cyc_CfFlowInfo_Unesc;
_LL428:;} void* Cyc_CfFlowInfo_join_initlevel( void* il1, void* il2){ struct
_tuple3 _temp444=({ struct _tuple3 _temp443; _temp443.f1= il1; _temp443.f2= il2;
_temp443;}); void* _temp456; void* _temp458; void* _temp460; void* _temp462;
void* _temp464; void* _temp466; void* _temp468; void* _temp470; _LL446: _LL459:
_temp458= _temp444.f1; if(( int) _temp458 == Cyc_CfFlowInfo_NoneIL){ goto _LL457;}
else{ goto _LL448;} _LL457: _temp456= _temp444.f2; goto _LL447; _LL448: _LL463:
_temp462= _temp444.f1; goto _LL461; _LL461: _temp460= _temp444.f2; if(( int)
_temp460 == Cyc_CfFlowInfo_NoneIL){ goto _LL449;} else{ goto _LL450;} _LL450:
_LL467: _temp466= _temp444.f1; if(( int) _temp466 == Cyc_CfFlowInfo_ThisIL){
goto _LL465;} else{ goto _LL452;} _LL465: _temp464= _temp444.f2; goto _LL451;
_LL452: _LL471: _temp470= _temp444.f1; goto _LL469; _LL469: _temp468= _temp444.f2;
if(( int) _temp468 == Cyc_CfFlowInfo_ThisIL){ goto _LL453;} else{ goto _LL454;}
_LL454: goto _LL455; _LL447: goto _LL449; _LL449: return( void*) Cyc_CfFlowInfo_NoneIL;
_LL451: goto _LL453; _LL453: return( void*) Cyc_CfFlowInfo_ThisIL; _LL455:
return( void*) Cyc_CfFlowInfo_AllIL; _LL445:;} void* Cyc_CfFlowInfo_join_initstate(
struct Cyc_CfFlowInfo_JoinEnv* env, void* state1, void* state2){ struct _tuple3
_temp473=({ struct _tuple3 _temp472; _temp472.f1= state1; _temp472.f2= state2;
_temp472;}); void* _temp483; void* _temp485; void* _temp487; void* _temp489;
void* _temp491; void* _temp493; void* _temp495; struct Cyc_CfFlowInfo_Place*
_temp497; void* _temp499; void* _temp501; void* _temp503; void* _temp505; void*
_temp507; void* _temp509; void* _temp511; struct Cyc_CfFlowInfo_Place* _temp513;
void* _temp515; struct Cyc_CfFlowInfo_Place* _temp517; void* _temp519; struct
Cyc_CfFlowInfo_Place* _temp521; _LL475: _LL490: _temp489= _temp473.f1; if(((
struct _tunion_struct*) _temp489)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL494:
_temp493=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp489)->f1; goto
_LL492; _LL492: _temp491=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp489)->f2; goto _LL484;} else{ goto _LL477;} _LL484: _temp483= _temp473.f2;
if((( struct _tunion_struct*) _temp483)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL488: _temp487=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp483)->f1;
goto _LL486; _LL486: _temp485=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp483)->f2; goto _LL476;} else{ goto _LL477;} _LL477: _LL500: _temp499=
_temp473.f1; if((( struct _tunion_struct*) _temp499)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL504: _temp503=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp499)->f1;
goto _LL502; _LL502: _temp501=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp499)->f2; goto _LL496;} else{ goto _LL479;} _LL496: _temp495= _temp473.f2;
if((( struct _tunion_struct*) _temp495)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL498: _temp497=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp495)->f1; goto _LL478;} else{ goto _LL479;} _LL479: _LL512: _temp511=
_temp473.f1; if((( struct _tunion_struct*) _temp511)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL514: _temp513=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp511)->f1; goto _LL506;} else{ goto _LL481;} _LL506: _temp505= _temp473.f2;
if((( struct _tunion_struct*) _temp505)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL510: _temp509=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp505)->f1;
goto _LL508; _LL508: _temp507=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp505)->f2; goto _LL480;} else{ goto _LL481;} _LL481: _LL520: _temp519=
_temp473.f1; if((( struct _tunion_struct*) _temp519)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL522: _temp521=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp519)->f1; goto _LL516;} else{ goto _LL474;} _LL516: _temp515= _temp473.f2;
if((( struct _tunion_struct*) _temp515)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL518: _temp517=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp515)->f1; goto _LL482;} else{ goto _LL474;} _LL476: return Cyc_CfFlowInfo_mkLeafPI(
Cyc_CfFlowInfo_join_escape( _temp493, _temp487), Cyc_CfFlowInfo_join_initlevel(
_temp491, _temp485)); _LL478: { struct _tuple6 isInitEnv=({ struct _tuple6
_temp523; _temp523.f1= env->d2; _temp523.f2= 0; _temp523;}); if( _temp501 ==(
void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(& isInitEnv, Cyc_CfFlowInfo_lookup_place(
env->d2, _temp497)): 0){* env->places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places, _temp497);
return Cyc_CfFlowInfo_mkLeafPI( _temp503, _temp501);} return Cyc_CfFlowInfo_mkLeafPI(
_temp503, Cyc_CfFlowInfo_join_initlevel( _temp501,( void*) Cyc_CfFlowInfo_ThisIL));}
_LL480: { struct _tuple6 isInitEnv=({ struct _tuple6 _temp524; _temp524.f1= env->d1;
_temp524.f2= 0; _temp524;}); if( _temp507 ==( void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv, Cyc_CfFlowInfo_lookup_place( env->d1, _temp513)): 0){* env->places=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(* env->places, _temp513); return Cyc_CfFlowInfo_mkLeafPI(
_temp509, _temp507);} return Cyc_CfFlowInfo_mkLeafPI( _temp509, Cyc_CfFlowInfo_join_initlevel((
void*) Cyc_CfFlowInfo_ThisIL, _temp507));} _LL482: if( Cyc_CfFlowInfo_place_cmp(
_temp521, _temp517) == 0){ return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct*
_temp525=( struct Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp525[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp526; _temp526.tag= Cyc_CfFlowInfo_LeafPI_tag;
_temp526.f1=( void*) state1; _temp526;}); _temp525;});}{ struct _tuple6
isInitEnv1=({ struct _tuple6 _temp527; _temp527.f1= env->d1; _temp527.f2= 0;
_temp527;}); struct _tuple6 isInitEnv2=({ struct _tuple6 _temp528; _temp528.f1=
env->d2; _temp528.f2= 0; _temp528;}); if( Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv1, Cyc_CfFlowInfo_lookup_place( env->d1, _temp521))? Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv2, Cyc_CfFlowInfo_lookup_place( env->d2, _temp517)): 0){* env->places=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(* env->places, _temp521);* env->places=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places,
_temp517); return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,( void*)
Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,(
void*) Cyc_CfFlowInfo_ThisIL);} _LL474:;} void* Cyc_CfFlowInfo_join_pathinfo(
struct Cyc_CfFlowInfo_JoinEnv* env, void* pinfo1, void* pinfo2){ if( pinfo1 ==
pinfo2){ return pinfo1;}{ struct _tuple3 _temp530=({ struct _tuple3 _temp529;
_temp529.f1= pinfo1; _temp529.f2= pinfo2; _temp529;}); void* _temp540; void*
_temp542; void* _temp544; void* _temp546; void* _temp548; struct Cyc_Dict_Dict*
_temp550; void* _temp552; struct Cyc_Dict_Dict* _temp554; void* _temp556; struct
Cyc_Dict_Dict* _temp558; void* _temp560; struct Cyc_Dict_Dict* _temp562; _LL532:
_LL545: _temp544= _temp530.f1; if((( struct _tunion_struct*) _temp544)->tag ==
Cyc_CfFlowInfo_LeafPI_tag){ _LL547: _temp546=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp544)->f1; goto _LL541;} else{ goto _LL534;} _LL541: _temp540= _temp530.f2;
if((( struct _tunion_struct*) _temp540)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL543: _temp542=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp540)->f1;
goto _LL533;} else{ goto _LL534;} _LL534: _LL553: _temp552= _temp530.f1; if(((
struct _tunion_struct*) _temp552)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL555:
_temp554=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp552)->f1; goto _LL549;} else{ goto _LL536;} _LL549: _temp548= _temp530.f2;
if((( struct _tunion_struct*) _temp548)->tag == Cyc_CfFlowInfo_TuplePI_tag){
_LL551: _temp550=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp548)->f1; goto _LL535;} else{ goto _LL536;} _LL536: _LL561: _temp560=
_temp530.f1; if((( struct _tunion_struct*) _temp560)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL563: _temp562=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp560)->f1; goto _LL557;} else{ goto _LL538;} _LL557: _temp556= _temp530.f2;
if((( struct _tunion_struct*) _temp556)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL559: _temp558=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp556)->f1; goto _LL537;} else{ goto _LL538;} _LL538: goto _LL539; _LL533:
if( _temp546 == _temp542){ return pinfo1;} return Cyc_CfFlowInfo_join_initstate(
env, _temp546, _temp542); _LL535: if( _temp554 == _temp550){ return pinfo1;}
return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp564=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp564[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp565; _temp565.tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp565.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,
env, _temp554, _temp550); _temp565;}); _temp564;}); _LL537: if( _temp562 ==
_temp558){ return pinfo1;} return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp566=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp566[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp567; _temp567.tag=
Cyc_CfFlowInfo_StructPI_tag; _temp567.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(
Cyc_CfFlowInfo_join_pathinfo, env, _temp562, _temp558); _temp567;}); _temp566;});
_LL539:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp568=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp568[ 0]=({ struct Cyc_Core_Impossible_struct _temp569; _temp569.tag= Cyc_Core_Impossible_tag;
_temp569.f1=( struct _tagged_string)({ char* _temp570=( char*)"join_pathinfo";
struct _tagged_string _temp571; _temp571.curr= _temp570; _temp571.base= _temp570;
_temp571.last_plus_one= _temp570 + 14; _temp571;}); _temp569;}); _temp568;}));
_LL531:;}} void* Cyc_CfFlowInfo_join_flow( void* f1, void* f2){ if( Cyc_CfFlowInfo_empty_place_set_ptr
== 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({ struct Cyc_Set_Set** _temp572=(
struct Cyc_Set_Set**) GC_malloc( sizeof( struct Cyc_Set_Set*) * 1); _temp572[ 0]=((
struct Cyc_Set_Set*(*)( int(* comp)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*)))
Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp); _temp572;});} if( f1 == f2){ return
f1;}{ struct _tuple3 _temp574=({ struct _tuple3 _temp573; _temp573.f1= f1;
_temp573.f2= f2; _temp573;}); void* _temp582; void* _temp584; void* _temp586;
void* _temp588; void* _temp590; struct Cyc_Dict_Dict* _temp592; void* _temp594;
struct Cyc_Dict_Dict* _temp596; _LL576: _LL585: _temp584= _temp574.f1; if(( int)
_temp584 == Cyc_CfFlowInfo_BottomFL){ goto _LL583;} else{ goto _LL578;} _LL583:
_temp582= _temp574.f2; goto _LL577; _LL578: _LL589: _temp588= _temp574.f1; goto
_LL587; _LL587: _temp586= _temp574.f2; if(( int) _temp586 == Cyc_CfFlowInfo_BottomFL){
goto _LL579;} else{ goto _LL580;} _LL580: _LL595: _temp594= _temp574.f1; if((
unsigned int) _temp594 > 1u?(( struct _tunion_struct*) _temp594)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL597: _temp596=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp594)->f1; goto _LL591;} else{ goto _LL575;} _LL591: _temp590= _temp574.f2;
if(( unsigned int) _temp590 > 1u?(( struct _tunion_struct*) _temp590)->tag ==
Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL593: _temp592=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp590)->f1; goto _LL581;} else{ goto
_LL575;} _LL577: return f2; _LL579: return f1; _LL581: if( _temp596 == _temp592){
return f1;}{ struct Cyc_Set_Set* places=({ struct Cyc_Set_Set** _temp598= Cyc_CfFlowInfo_empty_place_set_ptr;
if( _temp598 == 0){ _throw( Null_Exception);}* _temp598;}); struct Cyc_CfFlowInfo_JoinEnv
joinenv=({ struct Cyc_CfFlowInfo_JoinEnv _temp599; _temp599.d1= _temp596;
_temp599.d2= _temp592; _temp599.places=& places; _temp599;}); struct Cyc_Dict_Dict*
outdict=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,&
joinenv, _temp596, _temp592); return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp600=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp600[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp601; _temp601.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp601.f1= Cyc_CfFlowInfo_escape_these( places, outdict); _temp601;});
_temp600;});} _LL575:;}} int Cyc_CfFlowInfo_initstate_lessthan_approx( void*
state1, void* state2){ struct _tuple3 _temp603=({ struct _tuple3 _temp602;
_temp602.f1= state1; _temp602.f2= state2; _temp602;}); void* _temp611; void*
_temp613; void* _temp615; void* _temp617; void* _temp619; void* _temp621; void*
_temp623; struct Cyc_CfFlowInfo_Place* _temp625; void* _temp627; struct Cyc_CfFlowInfo_Place*
_temp629; _LL605: _LL618: _temp617= _temp603.f1; if((( struct _tunion_struct*)
_temp617)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL622: _temp621=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp617)->f1; goto _LL620; _LL620:
_temp619=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp617)->f2; goto
_LL612;} else{ goto _LL607;} _LL612: _temp611= _temp603.f2; if((( struct
_tunion_struct*) _temp611)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL616:
_temp615=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp611)->f1; goto
_LL614; _LL614: _temp613=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp611)->f2; goto _LL606;} else{ goto _LL607;} _LL607: _LL628: _temp627=
_temp603.f1; if((( struct _tunion_struct*) _temp627)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL630: _temp629=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp627)->f1; goto _LL624;} else{ goto _LL609;} _LL624: _temp623= _temp603.f2;
if((( struct _tunion_struct*) _temp623)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL626: _temp625=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp623)->f1; goto _LL608;} else{ goto _LL609;} _LL609: goto _LL610; _LL606:
return Cyc_CfFlowInfo_join_escape( _temp621, _temp615) == _temp615? Cyc_CfFlowInfo_join_initlevel(
_temp619, _temp613) == _temp613: 0; _LL608: return Cyc_CfFlowInfo_place_cmp(
_temp629, _temp625) == 0; _LL610: return 0; _LL604:;} int Cyc_CfFlowInfo_pinfo_lessthan_approx(
void* ignore, void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return 1;}{
struct _tuple3 _temp632=({ struct _tuple3 _temp631; _temp631.f1= pinfo1;
_temp631.f2= pinfo2; _temp631;}); void* _temp642; void* _temp644; void* _temp646;
void* _temp648; void* _temp650; struct Cyc_Dict_Dict* _temp652; void* _temp654;
struct Cyc_Dict_Dict* _temp656; void* _temp658; struct Cyc_Dict_Dict* _temp660;
void* _temp662; struct Cyc_Dict_Dict* _temp664; _LL634: _LL647: _temp646=
_temp632.f1; if((( struct _tunion_struct*) _temp646)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL649: _temp648=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp646)->f1;
goto _LL643;} else{ goto _LL636;} _LL643: _temp642= _temp632.f2; if((( struct
_tunion_struct*) _temp642)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL645: _temp644=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp642)->f1; goto _LL635;}
else{ goto _LL636;} _LL636: _LL655: _temp654= _temp632.f1; if((( struct
_tunion_struct*) _temp654)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL657: _temp656=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp654)->f1;
goto _LL651;} else{ goto _LL638;} _LL651: _temp650= _temp632.f2; if((( struct
_tunion_struct*) _temp650)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL653: _temp652=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp650)->f1;
goto _LL637;} else{ goto _LL638;} _LL638: _LL663: _temp662= _temp632.f1; if(((
struct _tunion_struct*) _temp662)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL665:
_temp664=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp662)->f1; goto _LL659;} else{ goto _LL640;} _LL659: _temp658= _temp632.f2;
if((( struct _tunion_struct*) _temp658)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL661: _temp660=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp658)->f1; goto _LL639;} else{ goto _LL640;} _LL640: goto _LL641; _LL635:
return _temp648 == _temp644? 1: Cyc_CfFlowInfo_initstate_lessthan_approx(
_temp648, _temp644); _LL637: return _temp656 == _temp652? 1:(( int(*)( int(* f)(
int, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( int ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp656, _temp652); _LL639: return _temp664 == _temp660? 1:(( int(*)( int(* f)(
struct _tagged_string*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_string* ignore, void*
pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp664, _temp660);
_LL641:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp666=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp666[ 0]=({ struct Cyc_Core_Impossible_struct _temp667; _temp667.tag= Cyc_Core_Impossible_tag;
_temp667.f1=( struct _tagged_string)({ char* _temp668=( char*)"pinfo_lessthan";
struct _tagged_string _temp669; _temp669.curr= _temp668; _temp669.base= _temp668;
_temp669.last_plus_one= _temp668 + 15; _temp669;}); _temp667;}); _temp666;}));
_LL633:;}} int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2){ if( f1
== f2){ return 1;}{ struct _tuple3 _temp671=({ struct _tuple3 _temp670; _temp670.f1=
f1; _temp670.f2= f2; _temp670;}); void* _temp679; void* _temp681; void* _temp683;
void* _temp685; void* _temp687; struct Cyc_Dict_Dict* _temp689; void* _temp691;
struct Cyc_Dict_Dict* _temp693; _LL673: _LL682: _temp681= _temp671.f1; if(( int)
_temp681 == Cyc_CfFlowInfo_BottomFL){ goto _LL680;} else{ goto _LL675;} _LL680:
_temp679= _temp671.f2; goto _LL674; _LL675: _LL686: _temp685= _temp671.f1; goto
_LL684; _LL684: _temp683= _temp671.f2; if(( int) _temp683 == Cyc_CfFlowInfo_BottomFL){
goto _LL676;} else{ goto _LL677;} _LL677: _LL692: _temp691= _temp671.f1; if((
unsigned int) _temp691 > 1u?(( struct _tunion_struct*) _temp691)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL694: _temp693=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp691)->f1; goto _LL688;} else{ goto _LL672;} _LL688: _temp687= _temp671.f2;
if(( unsigned int) _temp687 > 1u?(( struct _tunion_struct*) _temp687)->tag ==
Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL690: _temp689=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp687)->f1; goto _LL678;} else{ goto
_LL672;} _LL674: return 1; _LL676: return 0; _LL678: if( _temp693 == _temp689){
return 1;} return(( int(*)( int(* f)( void*, void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)(( int(*)( void* ignore,
void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp693,
_temp689); _LL672:;}}