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
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t;
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError_tag[
14u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length(
struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern void Cyc_List_iter_c(
void(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_revappend( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_flatten( struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; extern int Cyc_List_forall( int(*
pred)( void*), struct Cyc_List_List* x); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern struct _tagged_string
Cyc_String_strconcat( struct _tagged_string, struct _tagged_string); extern
struct _tagged_string Cyc_String_implode( struct Cyc_List_List* c); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[ 7u];
struct Cyc_Set_Absent_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict* d, void* key); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; extern struct
_tagged_string Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
typedef void* Cyc_Position_Error_kind; static const unsigned int Cyc_Position_Lex=
0; static const unsigned int Cyc_Position_Parse= 1; static const unsigned int
Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{
struct _tagged_string source; struct Cyc_Position_Segment* seg; void* kind;
struct _tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
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
struct Cyc_Absyn_Tunion_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_XTunion_p_tag= 9; struct Cyc_Absyn_XTunion_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_XTuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ; static
const int Cyc_Absyn_Enum_p_tag= 10; struct Cyc_Absyn_Enum_p_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
char* tag; } ; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*);
extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ();
extern void* Cyc_Absyn_strct( struct _tagged_string* name); extern void* Cyc_Absyn_strctq(
struct _tuple0* name); extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_string s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*, struct Cyc_List_List*
es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp(
void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp( struct
Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*, void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_pointer_exn_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt(
struct _tuple0*, void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt(
struct _tagged_string* v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string*
lab, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x,
void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init); extern int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate*
Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t;
struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct
_tagged_string Cyc_Absynpp_typ2string( void*); extern struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0*); extern struct _tagged_string Cyc_Absynpp_prim2string( void* p);
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
extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{ char*
tag; } ; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void* Cyc_Tcutil_typ_kind(
void* t); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify(
void*, void*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_Absyn_Structdecl* sd); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern void*
Cyc_Tcutil_snd_tqt( struct _tuple3*); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification(
struct _tagged_string s, int i); extern struct Cyc_List_List* Cyc_Toc_toc(
struct Cyc_List_List* ds); static struct Cyc_List_List* Cyc_Toc_result_decls= 0;
struct Cyc_Absyn_Abs_n_struct Cyc_Toc_abs_ns_value={ 1u, 0}; void* Cyc_Toc_abs_ns=(
void*)& Cyc_Toc_abs_ns_value; struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq=( struct Cyc_Absyn_Tqual){.q_const=
0,.q_volatile= 0,.q_restrict= 0}; char Cyc_Toc_Toc_Unimplemented_tag[ 18u]="Toc_Unimplemented";
struct Cyc_Toc_Toc_Unimplemented_struct{ char* tag; } ; char Cyc_Toc_Toc_Impossible_tag[
15u]="Toc_Impossible"; struct Cyc_Toc_Toc_Impossible_struct{ char* tag; } ;
static void* Cyc_Toc_unimp( struct _tagged_string s){({ struct _tagged_string
_temp1= s; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp1.last_plus_one - _temp1.curr,
_temp1.curr);}); Cyc_Stdio_fflush( Cyc_Stdio_stderr);( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Toc_Toc_Unimplemented_struct* _temp2=( struct Cyc_Toc_Toc_Unimplemented_struct*)
GC_malloc( sizeof( struct Cyc_Toc_Toc_Unimplemented_struct)); _temp2[ 0]=({
struct Cyc_Toc_Toc_Unimplemented_struct _temp3; _temp3.tag= Cyc_Toc_Toc_Unimplemented_tag;
_temp3;}); _temp2;}));} static void* Cyc_Toc_toc_impos( struct _tagged_string s){({
struct _tagged_string _temp4= s; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp4.last_plus_one
- _temp4.curr, _temp4.curr);}); Cyc_Stdio_fflush( Cyc_Stdio_stderr);( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Toc_Toc_Impossible_struct* _temp5=(
struct Cyc_Toc_Toc_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Toc_Toc_Impossible_struct));
_temp5[ 0]=({ struct Cyc_Toc_Toc_Impossible_struct _temp6; _temp6.tag= Cyc_Toc_Toc_Impossible_tag;
_temp6;}); _temp5;}));} char Cyc_Toc_Match_error_tag[ 12u]="Match_error"; struct
Cyc_Toc_Match_error_struct{ char* tag; } ; static char _temp9[ 5u]="curr";
static struct _tagged_string Cyc_Toc_curr_string=( struct _tagged_string){
_temp9, _temp9, _temp9 + 5u}; static struct _tagged_string* Cyc_Toc_curr_sp=&
Cyc_Toc_curr_string; static char _temp12[ 14u]="last_plus_one"; static struct
_tagged_string Cyc_Toc_last_plus_one_string=( struct _tagged_string){ _temp12,
_temp12, _temp12 + 14u}; static struct _tagged_string* Cyc_Toc_last_plus_one_sp=&
Cyc_Toc_last_plus_one_string; static char _temp15[ 5u]="base"; static struct
_tagged_string Cyc_Toc_base_string=( struct _tagged_string){ _temp15, _temp15,
_temp15 + 5u}; static struct _tagged_string* Cyc_Toc_base_sp=& Cyc_Toc_base_string;
static char _temp18[ 15u]="_tagged_string"; static struct _tagged_string Cyc_Toc__tagged_string_string=(
struct _tagged_string){ _temp18, _temp18, _temp18 + 15u}; static struct
_tagged_string* Cyc_Toc__tagged_string_sp=& Cyc_Toc__tagged_string_string;
static char _temp21[ 15u]="_tunion_struct"; static struct _tagged_string Cyc_Toc__tunion_struct_string=(
struct _tagged_string){ _temp21, _temp21, _temp21 + 15u}; static struct
_tagged_string* Cyc_Toc__tunion_struct_sp=& Cyc_Toc__tunion_struct_string;
static char _temp24[ 16u]="_xtunion_struct"; static struct _tagged_string Cyc_Toc__xtunion_struct_string=(
struct _tagged_string){ _temp24, _temp24, _temp24 + 16u}; static struct
_tagged_string* Cyc_Toc__xtunion_struct_sp=& Cyc_Toc__xtunion_struct_string;
static char _temp27[ 2u]="v"; static struct _tagged_string Cyc_Toc_v_string=(
struct _tagged_string){ _temp27, _temp27, _temp27 + 2u}; static struct
_tagged_string* Cyc_Toc_v_sp=& Cyc_Toc_v_string; static char _temp30[ 4u]="tag";
static struct _tagged_string Cyc_Toc_tag_string=( struct _tagged_string){
_temp30, _temp30, _temp30 + 4u}; static struct _tagged_string* Cyc_Toc_tag_sp=&
Cyc_Toc_tag_string; static char _temp33[ 14u]="_handler_cons"; static struct
_tagged_string Cyc_Toc__handler_cons_string=( struct _tagged_string){ _temp33,
_temp33, _temp33 + 14u}; static struct _tagged_string* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static char _temp36[ 7u]="setjmp"; static struct
_tagged_string Cyc_Toc_setjmp_string=( struct _tagged_string){ _temp36, _temp36,
_temp36 + 7u}; static struct _tagged_string* Cyc_Toc_setjmp_sp=& Cyc_Toc_setjmp_string;
static char _temp39[ 14u]="_push_handler"; static struct _tagged_string Cyc_Toc__push_handler_string=(
struct _tagged_string){ _temp39, _temp39, _temp39 + 14u}; static struct
_tagged_string* Cyc_Toc__push_handler_sp=& Cyc_Toc__push_handler_string; static
char _temp42[ 13u]="_pop_handler"; static struct _tagged_string Cyc_Toc__pop_handler_string=(
struct _tagged_string){ _temp42, _temp42, _temp42 + 13u}; static struct
_tagged_string* Cyc_Toc__pop_handler_sp=& Cyc_Toc__pop_handler_string; static
char _temp45[ 8u]="handler"; static struct _tagged_string Cyc_Toc_handler_string=(
struct _tagged_string){ _temp45, _temp45, _temp45 + 8u}; static struct
_tagged_string* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static char _temp48[
14u]="_npop_handler"; static struct _tagged_string Cyc_Toc__npop_handler_string=(
struct _tagged_string){ _temp48, _temp48, _temp48 + 14u}; static struct _tuple0
Cyc_Toc__npop_handler_pr=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__npop_handler_string}; static struct _tuple0* Cyc_Toc__npop_handler_qv=&
Cyc_Toc__npop_handler_pr; static char _temp51[ 16u]="Match_Exception"; static
struct _tagged_string Cyc_Toc_Match_Exception_string=( struct _tagged_string){
_temp51, _temp51, _temp51 + 16u}; static struct _tagged_string* Cyc_Toc_Match_Exception_sp=&
Cyc_Toc_Match_Exception_string; static char _temp54[ 15u]="Cyc_new_string";
static struct _tagged_string Cyc_Toc_Cyc_new_string_string=( struct
_tagged_string){ _temp54, _temp54, _temp54 + 15u}; static struct _tagged_string*
Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string; static char _temp57[
16u]="Cyc_rnew_string"; static struct _tagged_string Cyc_Toc_Cyc_rnew_string_string=(
struct _tagged_string){ _temp57, _temp57, _temp57 + 16u}; static struct
_tagged_string* Cyc_Toc_Cyc_rnew_string_sp=& Cyc_Toc_Cyc_rnew_string_string;
static char _temp60[ 14u]="_RegionHandle"; static struct _tagged_string Cyc_Toc__region_handle_string=(
struct _tagged_string){ _temp60, _temp60, _temp60 + 14u}; static struct
_tagged_string* Cyc_Toc__region_handle_sp=& Cyc_Toc__region_handle_string;
static char _temp63[ 13u]="_free_region"; static struct _tagged_string Cyc_Toc__free_region_string=(
struct _tagged_string){ _temp63, _temp63, _temp63 + 13u}; static struct
_tagged_string* Cyc_Toc__free_region_sp=& Cyc_Toc__free_region_string; static
char _temp66[ 12u]="_new_region"; static struct _tagged_string Cyc_Toc__new_region_string=(
struct _tagged_string){ _temp66, _temp66, _temp66 + 12u}; static struct
_tagged_string* Cyc_Toc__new_region_sp=& Cyc_Toc__new_region_string; struct
_tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp* e){ void* _temp67=( void*) e->r; void* _temp93; char
_temp95; void* _temp97; void* _temp99; short _temp101; void* _temp103; void*
_temp105; int _temp107; void* _temp109; void* _temp111; long long _temp113; void*
_temp115; void* _temp117; struct Cyc_Absyn_Exp* _temp119; void* _temp121; struct
Cyc_List_List* _temp123; struct Cyc_List_List* _temp125; struct Cyc_Absyn_Structdecl*
_temp127; struct Cyc_List_List* _temp129; struct Cyc_Core_Opt* _temp131; struct
_tuple0* _temp133; struct Cyc_List_List* _temp135; struct _tuple1* _temp137;
struct Cyc_List_List* _temp139; struct Cyc_Core_Opt* _temp141; _LL69: if(((
struct _tunion_struct*) _temp67)->tag == Cyc_Absyn_Const_e_tag){ _LL94: _temp93=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp67)->f1; if(( unsigned int)
_temp93 > 1u?(( struct _tunion_struct*) _temp93)->tag == Cyc_Absyn_Char_c_tag: 0){
_LL98: _temp97=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp93)->f1; goto
_LL96; _LL96: _temp95=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp93)->f2;
goto _LL70;} else{ goto _LL71;}} else{ goto _LL71;} _LL71: if((( struct
_tunion_struct*) _temp67)->tag == Cyc_Absyn_Const_e_tag){ _LL100: _temp99=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp67)->f1; if(( unsigned int) _temp99 > 1u?((
struct _tunion_struct*) _temp99)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL104:
_temp103=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp99)->f1; goto _LL102;
_LL102: _temp101=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp99)->f2; goto
_LL72;} else{ goto _LL73;}} else{ goto _LL73;} _LL73: if((( struct
_tunion_struct*) _temp67)->tag == Cyc_Absyn_Const_e_tag){ _LL106: _temp105=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp67)->f1; if(( unsigned int)
_temp105 > 1u?(( struct _tunion_struct*) _temp105)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL110: _temp109=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp105)->f1;
goto _LL108; _LL108: _temp107=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp105)->f2;
goto _LL74;} else{ goto _LL75;}} else{ goto _LL75;} _LL75: if((( struct
_tunion_struct*) _temp67)->tag == Cyc_Absyn_Const_e_tag){ _LL112: _temp111=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp67)->f1; if(( unsigned int)
_temp111 > 1u?(( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_LongLong_c_tag:
0){ _LL116: _temp115=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp111)->f1;
goto _LL114; _LL114: _temp113=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp111)->f2; goto _LL76;} else{ goto _LL77;}} else{ goto _LL77;} _LL77: if(((
struct _tunion_struct*) _temp67)->tag == Cyc_Absyn_Const_e_tag){ _LL118:
_temp117=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp67)->f1; if(( int)
_temp117 == Cyc_Absyn_Null_c){ goto _LL78;} else{ goto _LL79;}} else{ goto _LL79;}
_LL79: if((( struct _tunion_struct*) _temp67)->tag == Cyc_Absyn_Cast_e_tag){
_LL122: _temp121=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp67)->f1; goto
_LL120; _LL120: _temp119=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp67)->f2; goto _LL80;} else{ goto _LL81;} _LL81: if((( struct _tunion_struct*)
_temp67)->tag == Cyc_Absyn_Tuple_e_tag){ _LL124: _temp123=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_e_struct*) _temp67)->f1; goto _LL82;} else{ goto _LL83;}
_LL83: if((( struct _tunion_struct*) _temp67)->tag == Cyc_Absyn_Array_e_tag){
_LL126: _temp125=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp67)->f1; goto _LL84;} else{ goto _LL85;} _LL85: if((( struct _tunion_struct*)
_temp67)->tag == Cyc_Absyn_Struct_e_tag){ _LL134: _temp133=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp67)->f1; goto _LL132; _LL132: _temp131=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp67)->f2; goto
_LL130; _LL130: _temp129=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp67)->f3; goto _LL128; _LL128: _temp127=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp67)->f4; goto _LL86;} else{ goto _LL87;}
_LL87: if((( struct _tunion_struct*) _temp67)->tag == Cyc_Absyn_CompoundLit_e_tag){
_LL138: _temp137=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp67)->f1; goto _LL136; _LL136: _temp135=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp67)->f2; goto _LL88;} else{ goto _LL89;}
_LL89: if((( struct _tunion_struct*) _temp67)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL142: _temp141=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp67)->f1; goto _LL140; _LL140: _temp139=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp67)->f2; goto _LL90;} else{ goto _LL91;}
_LL91: goto _LL92; _LL70: return _temp95 =='\000'; _LL72: return _temp101 == 0;
_LL74: return _temp107 == 0; _LL76: return _temp113 == 0; _LL78: return 1; _LL80:
return Cyc_Toc_is_zero( _temp119); _LL82: return(( int(*)( int(* pred)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)( Cyc_Toc_is_zero,
_temp123); _LL84: _temp129= _temp125; goto _LL86; _LL86: _temp135= _temp129;
goto _LL88; _LL88: _temp139= _temp135; goto _LL90; _LL90: for( 0; _temp139 != 0;
_temp139=({ struct Cyc_List_List* _temp143= _temp139; if( _temp143 == 0){ _throw(
Null_Exception);} _temp143->tl;})){ if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)({
struct Cyc_List_List* _temp144= _temp139; if( _temp144 == 0){ _throw(
Null_Exception);} _temp144->hd;}))).f2)){ return 0;}} return 1; _LL92: return 0;
_LL68:;} static int Cyc_Toc_is_nullable( void* t){ void* _temp145= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp151; struct Cyc_Absyn_Conref* _temp153; struct
Cyc_Absyn_Tqual _temp155; struct Cyc_Absyn_Conref* _temp157; void* _temp159;
void* _temp161; _LL147: if(( unsigned int) _temp145 > 4u?(( struct
_tunion_struct*) _temp145)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL152:
_temp151=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp145)->f1; _LL162: _temp161=( void*) _temp151.elt_typ; goto _LL160; _LL160:
_temp159=( void*) _temp151.rgn_typ; goto _LL158; _LL158: _temp157=( struct Cyc_Absyn_Conref*)
_temp151.nullable; goto _LL156; _LL156: _temp155=( struct Cyc_Absyn_Tqual)
_temp151.tq; goto _LL154; _LL154: _temp153=( struct Cyc_Absyn_Conref*) _temp151.bounds;
goto _LL148;} else{ goto _LL149;} _LL149: goto _LL150; _LL148: { void* _temp163=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp157))->v; int _temp169; _LL165: if(( unsigned int) _temp163 > 1u?(( struct
_tunion_struct*) _temp163)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL170: _temp169=(
int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp163)->f1; goto _LL166;} else{
goto _LL167;} _LL167: goto _LL168; _LL166: return _temp169; _LL168: return 0;
_LL164:;} _LL150:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp171=( char*)"is_nullable"; struct
_tagged_string _temp172; _temp172.curr= _temp171; _temp172.base= _temp171;
_temp172.last_plus_one= _temp171 + 12; _temp172;})); return 0; _LL146:;} static
struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct _tuple0* x, struct
_tagged_string tag){ return({ struct _tuple0* _temp173=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp173->f1=(* x).f1; _temp173->f2=({
struct _tagged_string* _temp174=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp174[ 0]= Cyc_String_strconcat(*(* x).f2, tag);
_temp174;}); _temp173;});} static void Cyc_Toc_add_tuple_decl( struct
_tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List* fs= 0;{ int
i= 1; for( 0; ts != 0; ts=({ struct Cyc_List_List* _temp175= ts; if( _temp175 ==
0){ _throw( Null_Exception);} _temp175->tl;}), i ++){ fs=({ struct Cyc_List_List*
_temp176=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp176->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp177=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp177->name=({ struct
_tagged_string* _temp179=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp179[ 0]= xprintf("f%d", i); _temp179;}); _temp177->tq=
Cyc_Toc_mt_tq; _temp177->type=( void*)(( void*)({ struct Cyc_List_List* _temp178=
ts; if( _temp178 == 0){ _throw( Null_Exception);} _temp178->hd;})); _temp177->width=
0; _temp177->attributes= 0; _temp177;}); _temp176->tl= fs; _temp176;});}} fs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);{
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp183=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp183->sc=( void*)(( void*) Cyc_Absyn_Public); _temp183->name=({ struct Cyc_Core_Opt*
_temp185=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp185->v=( void*)({ struct _tuple0* _temp186=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp186->f1= Cyc_Toc_abs_ns; _temp186->f2= x;
_temp186;}); _temp185;}); _temp183->tvs= 0; _temp183->fields=({ struct Cyc_Core_Opt*
_temp184=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp184->v=( void*) fs; _temp184;}); _temp183->attributes= 0; _temp183;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp180=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp180->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp181=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp181[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp182; _temp182.tag= Cyc_Absyn_Struct_d_tag;
_temp182.f1= sd; _temp182;}); _temp181;}), 0); _temp180->tl= Cyc_Toc_result_decls;
_temp180;});}} static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x,
void* t){ void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Structfield* f1=({ struct Cyc_Absyn_Structfield* _temp199=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp199->name= Cyc_Toc_curr_sp; _temp199->tq= Cyc_Toc_mt_tq; _temp199->type=(
void*) tptr; _temp199->width= 0; _temp199->attributes= 0; _temp199;}); struct
Cyc_Absyn_Structfield* f2=({ struct Cyc_Absyn_Structfield* _temp198=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp198->name= Cyc_Toc_base_sp;
_temp198->tq= Cyc_Toc_mt_tq; _temp198->type=( void*) tptr; _temp198->width= 0;
_temp198->attributes= 0; _temp198;}); struct Cyc_Absyn_Structfield* f3=({ struct
Cyc_Absyn_Structfield* _temp197=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp197->name= Cyc_Toc_last_plus_one_sp;
_temp197->tq= Cyc_Toc_mt_tq; _temp197->type=( void*) tptr; _temp197->width= 0;
_temp197->attributes= 0; _temp197;}); struct Cyc_List_List* fs=({ struct Cyc_List_List*
_temp194=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp194->hd=( void*) f1; _temp194->tl=({ struct Cyc_List_List* _temp195=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp195->hd=(
void*) f2; _temp195->tl=({ struct Cyc_List_List* _temp196=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp196->hd=( void*) f3; _temp196->tl=
0; _temp196;}); _temp195;}); _temp194;}); struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp190=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp190->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp190->name=({ struct Cyc_Core_Opt* _temp192=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp192->v=( void*)({ struct _tuple0*
_temp193=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp193->f1=
Cyc_Toc_abs_ns; _temp193->f2= x; _temp193;}); _temp192;}); _temp190->tvs= 0;
_temp190->fields=({ struct Cyc_Core_Opt* _temp191=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp191->v=( void*)(( struct Cyc_List_List*)
fs); _temp191;}); _temp190->attributes= 0; _temp190;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp187=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp187->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp188=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp188[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp189; _temp189.tag= Cyc_Absyn_Struct_d_tag;
_temp189.f1= sd; _temp189;}); _temp188;}), 0); _temp187->tl= Cyc_Toc_result_decls;
_temp187;});} static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int
Cyc_Toc_tuple_type_counter= 0; struct _tuple5{ struct _tagged_string* f1; struct
Cyc_List_List* f2; } ; static struct _tagged_string* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* tts= Cyc_Toc_tuple_types;
for( 0; tts != 0; tts=({ struct Cyc_List_List* _temp200= tts; if( _temp200 == 0){
_throw( Null_Exception);} _temp200->tl;})){ struct _tuple5 _temp204; struct Cyc_List_List*
_temp205; struct _tagged_string* _temp207; struct _tuple5* _temp202=( struct
_tuple5*)({ struct Cyc_List_List* _temp201= tts; if( _temp201 == 0){ _throw(
Null_Exception);} _temp201->hd;}); _temp204=* _temp202; _LL208: _temp207=
_temp204.f1; goto _LL206; _LL206: _temp205= _temp204.f2; goto _LL203; _LL203: {
int okay= 1; struct Cyc_List_List* tqs= tqs0; for( 0; tqs != 0? _temp205 != 0: 0;
tqs=({ struct Cyc_List_List* _temp209= tqs; if( _temp209 == 0){ _throw(
Null_Exception);} _temp209->tl;}), _temp205=({ struct Cyc_List_List* _temp210=
_temp205; if( _temp210 == 0){ _throw( Null_Exception);} _temp210->tl;})){ if( !
Cyc_Tcutil_unify((*(( struct _tuple3*)({ struct Cyc_List_List* _temp211= tqs;
if( _temp211 == 0){ _throw( Null_Exception);} _temp211->hd;}))).f2,( void*)({
struct Cyc_List_List* _temp212= _temp205; if( _temp212 == 0){ _throw(
Null_Exception);} _temp212->hd;}))){ okay= 0; break;}} if(( ! okay? 1: tqs != 0)?
1: _temp205 != 0){ continue;} return _temp207;}}}{ struct _tagged_string* x=({
struct _tagged_string* _temp215=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp215[ 0]= xprintf("_tuple%d",( Cyc_Toc_tuple_type_counter
++)); _temp215;}); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt,
tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({ struct Cyc_List_List*
_temp213=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp213->hd=( void*)({ struct _tuple5* _temp214=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp214->f1= x; _temp214->f2= ts; _temp214;});
_temp213->tl= Cyc_Toc_tuple_types; _temp213;}); return x;}} static struct Cyc_List_List*
Cyc_Toc_tagged_ptr_types= 0; static int Cyc_Toc_tagged_ptr_counter= 0; struct
_tuple6{ struct _tagged_string* f1; void* f2; } ; static struct _tagged_string*
Cyc_Toc_add_tagged_ptr_type( void* t, struct Cyc_Absyn_Tqual tq){{ void*
_temp216= t; void* _temp222; void* _temp224; _LL218: if(( unsigned int) _temp216
> 4u?(( struct _tunion_struct*) _temp216)->tag == Cyc_Absyn_IntType_tag: 0){
_LL225: _temp224=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp216)->f1; if((
int) _temp224 == Cyc_Absyn_Unsigned){ goto _LL223;} else{ goto _LL220;} _LL223:
_temp222=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp216)->f2; if(( int)
_temp222 == Cyc_Absyn_B1){ goto _LL219;} else{ goto _LL220;}} else{ goto _LL220;}
_LL220: goto _LL221; _LL219: return Cyc_Toc__tagged_string_sp; _LL221: goto
_LL217; _LL217:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types; for( 0;
tts != 0; tts=({ struct Cyc_List_List* _temp226= tts; if( _temp226 == 0){ _throw(
Null_Exception);} _temp226->tl;})){ struct _tuple6 _temp230; void* _temp231;
struct _tagged_string* _temp233; struct _tuple6* _temp228=( struct _tuple6*)({
struct Cyc_List_List* _temp227= tts; if( _temp227 == 0){ _throw( Null_Exception);}
_temp227->hd;}); _temp230=* _temp228; _LL234: _temp233= _temp230.f1; goto _LL232;
_LL232: _temp231= _temp230.f2; goto _LL229; _LL229: if( ! Cyc_Tcutil_unify( t,
_temp231)){ continue;} return _temp233;}}{ struct _tagged_string* x=({ struct
_tagged_string* _temp237=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp237[ 0]= xprintf("_tagged_ptr%d",( Cyc_Toc_tagged_ptr_counter
++)); _temp237;}); Cyc_Toc_add_tagged_ptr_decl( x, t); Cyc_Toc_tagged_ptr_types=({
struct Cyc_List_List* _temp235=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp235->hd=( void*)({ struct _tuple6* _temp236=(
struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp236->f1= x; _temp236->f2=
t; _temp236;}); _temp235->tl= Cyc_Toc_tagged_ptr_types; _temp235;}); return x;}}
static int Cyc_Toc_temp_var_counter= 0; static struct _tuple0* Cyc_Toc_temp_var(){
return({ struct _tuple0* _temp238=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp238->f1= Cyc_Toc_abs_ns; _temp238->f2=({ struct _tagged_string*
_temp239=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp239[ 0]= xprintf("_temp%d",( Cyc_Toc_temp_var_counter ++)); _temp239;});
_temp238;});} static int Cyc_Toc_fresh_label_counter= 0; static struct
_tagged_string* Cyc_Toc_fresh_label(){ return({ struct _tagged_string* _temp240=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp240[
0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter ++)); _temp240;});} static void*
Cyc_Toc_typ_to_c( void* t); static struct _tuple1* Cyc_Toc_arg_to_c( struct
_tuple1* a){ void* _temp243; struct Cyc_Absyn_Tqual _temp245; struct Cyc_Core_Opt*
_temp247; struct _tuple1 _temp241=* a; _LL248: _temp247= _temp241.f1; goto
_LL246; _LL246: _temp245= _temp241.f2; goto _LL244; _LL244: _temp243= _temp241.f3;
goto _LL242; _LL242: return({ struct _tuple1* _temp249=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp249->f1= _temp247; _temp249->f2=
_temp245; _temp249->f3= Cyc_Toc_typ_to_c( _temp243); _temp249;});} static struct
_tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){ void* _temp252; struct Cyc_Absyn_Tqual
_temp254; struct _tuple3 _temp250=* x; _LL255: _temp254= _temp250.f1; goto
_LL253; _LL253: _temp252= _temp250.f2; goto _LL251; _LL251: return({ struct
_tuple3* _temp256=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3));
_temp256->f1= _temp254; _temp256->f2= Cyc_Toc_typ_to_c( _temp252); _temp256;});}
static void* Cyc_Toc_typ_to_c_array( void* t){ void* _temp257= t; struct Cyc_Absyn_Exp*
_temp265; struct Cyc_Absyn_Tqual _temp267; void* _temp269; int _temp271; struct
Cyc_Core_Opt* _temp273; struct Cyc_Core_Opt _temp275; void* _temp276; void*
_temp278; _LL259: if(( unsigned int) _temp257 > 4u?(( struct _tunion_struct*)
_temp257)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL270: _temp269=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp257)->f1; goto _LL268; _LL268: _temp267=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp257)->f2;
goto _LL266; _LL266: _temp265=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp257)->f3; goto _LL260;} else{ goto _LL261;} _LL261: if(( unsigned int)
_temp257 > 4u?(( struct _tunion_struct*) _temp257)->tag == Cyc_Absyn_Evar_tag: 0){
_LL279: _temp278=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp257)->f1; goto
_LL274; _LL274: _temp273=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp257)->f2; if( _temp273 == 0){ goto _LL263;} else{ _temp275=* _temp273;
_LL277: _temp276=( void*) _temp275.v; goto _LL272;} _LL272: _temp271=( int)((
struct Cyc_Absyn_Evar_struct*) _temp257)->f3; goto _LL262;} else{ goto _LL263;}
_LL263: goto _LL264; _LL260: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp280=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp280[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp281; _temp281.tag= Cyc_Absyn_ArrayType_tag;
_temp281.f1=( void*) Cyc_Toc_typ_to_c_array( _temp269); _temp281.f2= _temp267;
_temp281.f3= _temp265; _temp281;}); _temp280;}); _LL262: return Cyc_Toc_typ_to_c_array(
_temp276); _LL264: return Cyc_Toc_typ_to_c( t); _LL258:;} static void* Cyc_Toc_typ_to_c(
void* t){ void* _temp282= t; int _temp330; struct Cyc_Core_Opt* _temp332; void*
_temp334; int _temp336; struct Cyc_Core_Opt* _temp338; struct Cyc_Core_Opt
_temp340; void* _temp341; void* _temp343; struct Cyc_Absyn_Tvar* _temp345;
struct Cyc_Absyn_TunionInfo _temp347; struct Cyc_Absyn_Tuniondecl* _temp349;
void* _temp351; struct Cyc_List_List* _temp353; struct _tuple0* _temp355; struct
Cyc_Absyn_XTunionInfo _temp357; struct Cyc_Absyn_XTuniondecl* _temp359; void*
_temp361; struct _tuple0* _temp363; struct Cyc_Absyn_TunionFieldInfo _temp365;
struct Cyc_Absyn_Tunionfield* _temp367; struct Cyc_Absyn_Tuniondecl* _temp369;
struct _tuple0* _temp371; struct Cyc_List_List* _temp373; struct _tuple0*
_temp375; struct Cyc_Absyn_XTunionFieldInfo _temp377; struct Cyc_Absyn_Tunionfield*
_temp379; struct Cyc_Absyn_XTuniondecl* _temp381; struct _tuple0* _temp383;
struct _tuple0* _temp385; struct Cyc_Absyn_PtrInfo _temp387; struct Cyc_Absyn_Conref*
_temp389; struct Cyc_Absyn_Tqual _temp391; struct Cyc_Absyn_Conref* _temp393;
void* _temp395; void* _temp397; void* _temp399; void* _temp401; struct Cyc_Absyn_Exp*
_temp403; struct Cyc_Absyn_Tqual _temp405; void* _temp407; struct Cyc_Absyn_FnInfo
_temp409; struct Cyc_List_List* _temp411; int _temp413; struct Cyc_List_List*
_temp415; void* _temp417; struct Cyc_Core_Opt* _temp419; struct Cyc_List_List*
_temp421; struct Cyc_List_List* _temp423; struct Cyc_Absyn_Uniondecl** _temp425;
struct Cyc_List_List* _temp427; struct _tuple0* _temp429; struct Cyc_Absyn_Structdecl**
_temp431; struct Cyc_List_List* _temp433; struct _tuple0* _temp435; struct Cyc_Absyn_Enumdecl*
_temp437; struct _tuple0* _temp439; struct Cyc_Core_Opt* _temp441; struct Cyc_List_List*
_temp443; struct _tuple0* _temp445; void* _temp447; void* _temp449; struct Cyc_List_List*
_temp451; _LL284: if(( int) _temp282 == Cyc_Absyn_VoidType){ goto _LL285;} else{
goto _LL286;} _LL286: if(( unsigned int) _temp282 > 4u?(( struct _tunion_struct*)
_temp282)->tag == Cyc_Absyn_Evar_tag: 0){ _LL335: _temp334=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp282)->f1; goto _LL333; _LL333: _temp332=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp282)->f2; if( _temp332 == 0){ goto _LL331;} else{
goto _LL288;} _LL331: _temp330=( int)(( struct Cyc_Absyn_Evar_struct*) _temp282)->f3;
goto _LL287;} else{ goto _LL288;} _LL288: if(( unsigned int) _temp282 > 4u?((
struct _tunion_struct*) _temp282)->tag == Cyc_Absyn_Evar_tag: 0){ _LL344:
_temp343=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp282)->f1; goto _LL339;
_LL339: _temp338=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp282)->f2; if( _temp338 == 0){ goto _LL290;} else{ _temp340=* _temp338;
_LL342: _temp341=( void*) _temp340.v; goto _LL337;} _LL337: _temp336=( int)((
struct Cyc_Absyn_Evar_struct*) _temp282)->f3; goto _LL289;} else{ goto _LL290;}
_LL290: if(( unsigned int) _temp282 > 4u?(( struct _tunion_struct*) _temp282)->tag
== Cyc_Absyn_VarType_tag: 0){ _LL346: _temp345=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp282)->f1; goto _LL291;} else{ goto _LL292;}
_LL292: if(( unsigned int) _temp282 > 4u?(( struct _tunion_struct*) _temp282)->tag
== Cyc_Absyn_TunionType_tag: 0){ _LL348: _temp347=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp282)->f1; _LL356: _temp355=( struct
_tuple0*) _temp347.name; goto _LL354; _LL354: _temp353=( struct Cyc_List_List*)
_temp347.targs; goto _LL352; _LL352: _temp351=( void*) _temp347.rgn; goto _LL350;
_LL350: _temp349=( struct Cyc_Absyn_Tuniondecl*) _temp347.tud; goto _LL293;}
else{ goto _LL294;} _LL294: if(( unsigned int) _temp282 > 4u?(( struct
_tunion_struct*) _temp282)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL358:
_temp357=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp282)->f1; _LL364: _temp363=( struct _tuple0*) _temp357.name; goto _LL362;
_LL362: _temp361=( void*) _temp357.rgn; goto _LL360; _LL360: _temp359=( struct
Cyc_Absyn_XTuniondecl*) _temp357.xtud; goto _LL295;} else{ goto _LL296;} _LL296:
if(( unsigned int) _temp282 > 4u?(( struct _tunion_struct*) _temp282)->tag ==
Cyc_Absyn_TunionFieldType_tag: 0){ _LL366: _temp365=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp282)->f1; _LL376: _temp375=(
struct _tuple0*) _temp365.name; goto _LL374; _LL374: _temp373=( struct Cyc_List_List*)
_temp365.targs; goto _LL372; _LL372: _temp371=( struct _tuple0*) _temp365.fname;
goto _LL370; _LL370: _temp369=( struct Cyc_Absyn_Tuniondecl*) _temp365.tud; goto
_LL368; _LL368: _temp367=( struct Cyc_Absyn_Tunionfield*) _temp365.tufd; goto
_LL297;} else{ goto _LL298;} _LL298: if(( unsigned int) _temp282 > 4u?(( struct
_tunion_struct*) _temp282)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL378:
_temp377=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp282)->f1; _LL386: _temp385=( struct _tuple0*) _temp377.name; goto _LL384;
_LL384: _temp383=( struct _tuple0*) _temp377.fname; goto _LL382; _LL382:
_temp381=( struct Cyc_Absyn_XTuniondecl*) _temp377.xtud; goto _LL380; _LL380:
_temp379=( struct Cyc_Absyn_Tunionfield*) _temp377.xtufd; goto _LL299;} else{
goto _LL300;} _LL300: if(( unsigned int) _temp282 > 4u?(( struct _tunion_struct*)
_temp282)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL388: _temp387=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp282)->f1; _LL398: _temp397=( void*)
_temp387.elt_typ; goto _LL396; _LL396: _temp395=( void*) _temp387.rgn_typ; goto
_LL394; _LL394: _temp393=( struct Cyc_Absyn_Conref*) _temp387.nullable; goto
_LL392; _LL392: _temp391=( struct Cyc_Absyn_Tqual) _temp387.tq; goto _LL390;
_LL390: _temp389=( struct Cyc_Absyn_Conref*) _temp387.bounds; goto _LL301;}
else{ goto _LL302;} _LL302: if(( unsigned int) _temp282 > 4u?(( struct
_tunion_struct*) _temp282)->tag == Cyc_Absyn_IntType_tag: 0){ _LL402: _temp401=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp282)->f1; goto _LL400; _LL400:
_temp399=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp282)->f2; goto _LL303;}
else{ goto _LL304;} _LL304: if(( int) _temp282 == Cyc_Absyn_FloatType){ goto
_LL305;} else{ goto _LL306;} _LL306: if(( int) _temp282 == Cyc_Absyn_DoubleType){
goto _LL307;} else{ goto _LL308;} _LL308: if(( unsigned int) _temp282 > 4u?((
struct _tunion_struct*) _temp282)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL408:
_temp407=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp282)->f1; goto
_LL406; _LL406: _temp405=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp282)->f2; goto _LL404; _LL404: _temp403=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp282)->f3; goto _LL309;} else{ goto _LL310;}
_LL310: if(( unsigned int) _temp282 > 4u?(( struct _tunion_struct*) _temp282)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL410: _temp409=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp282)->f1; _LL422: _temp421=( struct Cyc_List_List*)
_temp409.tvars; goto _LL420; _LL420: _temp419=( struct Cyc_Core_Opt*) _temp409.effect;
goto _LL418; _LL418: _temp417=( void*) _temp409.ret_typ; goto _LL416; _LL416:
_temp415=( struct Cyc_List_List*) _temp409.args; goto _LL414; _LL414: _temp413=(
int) _temp409.varargs; goto _LL412; _LL412: _temp411=( struct Cyc_List_List*)
_temp409.attributes; goto _LL311;} else{ goto _LL312;} _LL312: if(( unsigned int)
_temp282 > 4u?(( struct _tunion_struct*) _temp282)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL424: _temp423=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp282)->f1; goto _LL313;} else{ goto _LL314;} _LL314: if(( unsigned int)
_temp282 > 4u?(( struct _tunion_struct*) _temp282)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL430: _temp429=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp282)->f1; goto _LL428; _LL428: _temp427=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp282)->f2; goto _LL426; _LL426: _temp425=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp282)->f3;
goto _LL315;} else{ goto _LL316;} _LL316: if(( unsigned int) _temp282 > 4u?((
struct _tunion_struct*) _temp282)->tag == Cyc_Absyn_StructType_tag: 0){ _LL436:
_temp435=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp282)->f1;
goto _LL434; _LL434: _temp433=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp282)->f2; goto _LL432; _LL432: _temp431=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp282)->f3; goto _LL317;} else{ goto
_LL318;} _LL318: if(( unsigned int) _temp282 > 4u?(( struct _tunion_struct*)
_temp282)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL440: _temp439=( struct _tuple0*)((
struct Cyc_Absyn_EnumType_struct*) _temp282)->f1; goto _LL438; _LL438: _temp437=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp282)->f2;
goto _LL319;} else{ goto _LL320;} _LL320: if(( unsigned int) _temp282 > 4u?((
struct _tunion_struct*) _temp282)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL446:
_temp445=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp282)->f1;
goto _LL444; _LL444: _temp443=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp282)->f2; goto _LL442; _LL442: _temp441=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp282)->f3; goto _LL321;} else{ goto _LL322;}
_LL322: if(( unsigned int) _temp282 > 4u?(( struct _tunion_struct*) _temp282)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL448: _temp447=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp282)->f1; goto _LL323;} else{ goto _LL324;} _LL324: if(( int) _temp282 ==
Cyc_Absyn_HeapRgn){ goto _LL325;} else{ goto _LL326;} _LL326: if(( unsigned int)
_temp282 > 4u?(( struct _tunion_struct*) _temp282)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL450: _temp449=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp282)->f1;
goto _LL327;} else{ goto _LL328;} _LL328: if(( unsigned int) _temp282 > 4u?((
struct _tunion_struct*) _temp282)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL452:
_temp451=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp282)->f1;
goto _LL329;} else{ goto _LL283;} _LL285: return t; _LL287: return( void*) Cyc_Absyn_VoidType;
_LL289: return Cyc_Toc_typ_to_c( _temp341); _LL291: return Cyc_Absyn_void_star_typ();
_LL293: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp453=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp453[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp454; _temp454.tag= Cyc_Absyn_TypedefType_tag;
_temp454.f1=( struct _tuple0*) _temp355; _temp454.f2= 0; _temp454.f3=({ struct
Cyc_Core_Opt* _temp455=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp455->v=( void*) Cyc_Absyn_void_star_typ(); _temp455;}); _temp454;});
_temp453;}); _LL295: return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp456=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp456[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp457; _temp457.tag= Cyc_Absyn_TypedefType_tag;
_temp457.f1= _temp363; _temp457.f2= 0; _temp457.f3=({ struct Cyc_Core_Opt*
_temp458=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp458->v=( void*) Cyc_Absyn_star_typ( Cyc_Absyn_strct( Cyc_Toc__xtunion_struct_sp),(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq); _temp458;}); _temp457;}); _temp456;});
_LL297: if(({ struct Cyc_Absyn_Tunionfield* _temp459= _temp367; if( _temp459 ==
0){ _throw( Null_Exception);} _temp459->typs;}) == 0){ return Cyc_Absyn_uint_t;}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp371,( struct
_tagged_string)({ char* _temp460=( char*)"_struct"; struct _tagged_string
_temp461; _temp461.curr= _temp460; _temp461.base= _temp460; _temp461.last_plus_one=
_temp460 + 8; _temp461;})));} _LL299: return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp383,( struct _tagged_string)({ char* _temp462=( char*)"_struct"; struct
_tagged_string _temp463; _temp463.curr= _temp462; _temp463.base= _temp462;
_temp463.last_plus_one= _temp462 + 8; _temp463;}))); _LL301: _temp397= Cyc_Toc_typ_to_c_array(
_temp397);{ void* _temp464=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp389))->v; void* _temp470; _LL466: if((
unsigned int) _temp464 > 1u?(( struct _tunion_struct*) _temp464)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL471: _temp470=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp464)->f1;
if(( int) _temp470 == Cyc_Absyn_Unknown_b){ goto _LL467;} else{ goto _LL468;}}
else{ goto _LL468;} _LL468: goto _LL469; _LL467: { struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
_temp397, _temp391); return Cyc_Absyn_strct( n);} _LL469: return Cyc_Absyn_star_typ(
_temp397,( void*) Cyc_Absyn_HeapRgn, _temp391); _LL465:;} _LL303: return t;
_LL305: return t; _LL307: return t; _LL309: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp472=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp472[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp473; _temp473.tag= Cyc_Absyn_ArrayType_tag;
_temp473.f1=( void*) Cyc_Toc_typ_to_c_array( _temp407); _temp473.f2= _temp405;
_temp473.f3= _temp403; _temp473;}); _temp472;}); _LL311: { struct Cyc_List_List*
new_atts= 0; for( 0; _temp411 != 0; _temp411=({ struct Cyc_List_List* _temp474=
_temp411; if( _temp474 == 0){ _throw( Null_Exception);} _temp474->tl;})){ void*
_temp476=( void*)({ struct Cyc_List_List* _temp475= _temp411; if( _temp475 == 0){
_throw( Null_Exception);} _temp475->hd;}); _LL478: if(( int) _temp476 == Cyc_Absyn_Cdecl_att){
goto _LL479;} else{ goto _LL480;} _LL480: if(( int) _temp476 == Cyc_Absyn_Noreturn_att){
goto _LL481;} else{ goto _LL482;} _LL482: if(( int) _temp476 == Cyc_Absyn_Const_att){
goto _LL483;} else{ goto _LL484;} _LL484: goto _LL485; _LL479: goto _LL481;
_LL481: goto _LL483; _LL483: continue; _LL485: new_atts=({ struct Cyc_List_List*
_temp486=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp486->hd=( void*)(( void*)({ struct Cyc_List_List* _temp487= _temp411; if(
_temp487 == 0){ _throw( Null_Exception);} _temp487->hd;})); _temp486->tl=
new_atts; _temp486;}); goto _LL477; _LL477:;} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp488=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp488[ 0]=({ struct Cyc_Absyn_FnType_struct _temp489; _temp489.tag= Cyc_Absyn_FnType_tag;
_temp489.f1=({ struct Cyc_Absyn_FnInfo _temp490; _temp490.tvars= 0; _temp490.effect=
0; _temp490.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp417); _temp490.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp415); _temp490.varargs= _temp413;
_temp490.attributes= new_atts; _temp490;}); _temp489;}); _temp488;});} _LL313:
_temp423=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp423);{ struct
_tagged_string* n= Cyc_Toc_add_tuple_type( _temp423); return Cyc_Absyn_strct( n);}
_LL315: if( _temp429 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp491=( char*)"anonymous union"; struct
_tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 16; _temp492;}));} return Cyc_Absyn_unionq_typ((
struct _tuple0*)({ struct _tuple0* _temp493= _temp429; if( _temp493 == 0){
_throw( Null_Exception);} _temp493;})); _LL317: if( _temp435 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp494=( char*)"anonymous struct"; struct _tagged_string _temp495; _temp495.curr=
_temp494; _temp495.base= _temp494; _temp495.last_plus_one= _temp494 + 17;
_temp495;}));} return Cyc_Absyn_strctq(( struct _tuple0*)({ struct _tuple0*
_temp496= _temp435; if( _temp496 == 0){ _throw( Null_Exception);} _temp496;}));
_LL319: return t; _LL321: if( _temp441 == 0){ return(( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp497=(
char*)"Toc::typ_to_c: unresolved TypedefType"; struct _tagged_string _temp498;
_temp498.curr= _temp497; _temp498.base= _temp497; _temp498.last_plus_one=
_temp497 + 38; _temp498;}));} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp499=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp499[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp500; _temp500.tag= Cyc_Absyn_TypedefType_tag;
_temp500.f1= _temp445; _temp500.f2= 0; _temp500.f3=({ struct Cyc_Core_Opt*
_temp501=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp501->v=( void*) Cyc_Toc_typ_to_c_array(( void*)({ struct Cyc_Core_Opt*
_temp502= _temp441; if( _temp502 == 0){ _throw( Null_Exception);} _temp502->v;}));
_temp501;}); _temp500;}); _temp499;}); _LL323: return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__region_handle_sp), Cyc_Toc_mt_tq); _LL325: return(( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp503=(
char*)"Toc::typ_to_c: type translation passed the heap region"; struct
_tagged_string _temp504; _temp504.curr= _temp503; _temp504.base= _temp503;
_temp504.last_plus_one= _temp503 + 55; _temp504;})); _LL327: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp505=( char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp506; _temp506.curr= _temp505; _temp506.base= _temp505;
_temp506.last_plus_one= _temp505 + 54; _temp506;})); _LL329: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp507=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp508; _temp508.curr= _temp507; _temp508.base= _temp507;
_temp508.last_plus_one= _temp507 + 52; _temp508;})); _LL283:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp509= t; struct Cyc_Absyn_Exp* _temp515; struct Cyc_Absyn_Tqual
_temp517; void* _temp519; _LL511: if(( unsigned int) _temp509 > 4u?(( struct
_tunion_struct*) _temp509)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL520: _temp519=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp509)->f1; goto _LL518; _LL518:
_temp517=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp509)->f2; goto _LL516; _LL516: _temp515=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp509)->f3; goto _LL512;} else{ goto _LL513;}
_LL513: goto _LL514; _LL512: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp519,( void*) Cyc_Absyn_HeapRgn, _temp517), e, l); _LL514: return e; _LL510:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* l){ void* _temp521= t; struct Cyc_Absyn_Exp*
_temp527; struct Cyc_Absyn_Tqual _temp529; void* _temp531; _LL523: if((
unsigned int) _temp521 > 4u?(( struct _tunion_struct*) _temp521)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL532: _temp531=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp521)->f1;
goto _LL530; _LL530: _temp529=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp521)->f2; goto _LL528; _LL528: _temp527=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp521)->f3; goto _LL524;} else{ goto _LL525;}
_LL525: goto _LL526; _LL524: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp531,( void*) Cyc_Absyn_HeapRgn, _temp529), e, l); _LL526: return Cyc_Absyn_cast_exp(
t, e, l); _LL522:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp533= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp569; void* _temp571; void* _temp573; struct Cyc_Absyn_Exp*
_temp575; struct Cyc_Absyn_Tqual _temp577; void* _temp579; struct Cyc_Absyn_FnInfo
_temp581; struct Cyc_List_List* _temp583; struct Cyc_Absyn_Structdecl** _temp585;
struct Cyc_List_List* _temp587; struct _tuple0* _temp589; struct Cyc_Absyn_TunionInfo
_temp591; struct Cyc_Absyn_XTunionInfo _temp593; struct Cyc_Absyn_TunionFieldInfo
_temp595; struct Cyc_Absyn_Tunionfield* _temp597; struct Cyc_Absyn_Tuniondecl*
_temp599; struct _tuple0* _temp601; struct Cyc_List_List* _temp603; struct
_tuple0* _temp605; struct Cyc_Absyn_XTunionFieldInfo _temp607; struct Cyc_Absyn_Uniondecl**
_temp609; struct Cyc_List_List* _temp611; struct _tuple0* _temp613; struct Cyc_Absyn_PtrInfo
_temp615; void* _temp617; _LL535: if(( int) _temp533 == Cyc_Absyn_VoidType){
goto _LL536;} else{ goto _LL537;} _LL537: if(( unsigned int) _temp533 > 4u?((
struct _tunion_struct*) _temp533)->tag == Cyc_Absyn_VarType_tag: 0){ _LL570:
_temp569=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp533)->f1;
goto _LL538;} else{ goto _LL539;} _LL539: if(( unsigned int) _temp533 > 4u?((
struct _tunion_struct*) _temp533)->tag == Cyc_Absyn_IntType_tag: 0){ _LL574:
_temp573=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp533)->f1; goto _LL572;
_LL572: _temp571=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp533)->f2;
goto _LL540;} else{ goto _LL541;} _LL541: if(( int) _temp533 == Cyc_Absyn_FloatType){
goto _LL542;} else{ goto _LL543;} _LL543: if(( int) _temp533 == Cyc_Absyn_DoubleType){
goto _LL544;} else{ goto _LL545;} _LL545: if(( unsigned int) _temp533 > 4u?((
struct _tunion_struct*) _temp533)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL580:
_temp579=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp533)->f1; goto
_LL578; _LL578: _temp577=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp533)->f2; goto _LL576; _LL576: _temp575=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp533)->f3; goto _LL546;} else{ goto _LL547;}
_LL547: if(( unsigned int) _temp533 > 4u?(( struct _tunion_struct*) _temp533)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL582: _temp581=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp533)->f1; goto _LL548;} else{ goto _LL549;}
_LL549: if(( unsigned int) _temp533 > 4u?(( struct _tunion_struct*) _temp533)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL584: _temp583=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp533)->f1; goto _LL550;} else{ goto
_LL551;} _LL551: if(( unsigned int) _temp533 > 4u?(( struct _tunion_struct*)
_temp533)->tag == Cyc_Absyn_StructType_tag: 0){ _LL590: _temp589=( struct
_tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp533)->f1; goto _LL588;
_LL588: _temp587=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp533)->f2; goto _LL586; _LL586: _temp585=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp533)->f3; goto _LL552;} else{ goto
_LL553;} _LL553: if(( unsigned int) _temp533 > 4u?(( struct _tunion_struct*)
_temp533)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL592: _temp591=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp533)->f1; goto _LL554;} else{ goto
_LL555;} _LL555: if(( unsigned int) _temp533 > 4u?(( struct _tunion_struct*)
_temp533)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL594: _temp593=( struct Cyc_Absyn_XTunionInfo)((
struct Cyc_Absyn_XTunionType_struct*) _temp533)->f1; goto _LL556;} else{ goto
_LL557;} _LL557: if(( unsigned int) _temp533 > 4u?(( struct _tunion_struct*)
_temp533)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL596: _temp595=( struct
Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*) _temp533)->f1;
_LL606: _temp605=( struct _tuple0*) _temp595.name; goto _LL604; _LL604: _temp603=(
struct Cyc_List_List*) _temp595.targs; goto _LL602; _LL602: _temp601=( struct
_tuple0*) _temp595.fname; goto _LL600; _LL600: _temp599=( struct Cyc_Absyn_Tuniondecl*)
_temp595.tud; goto _LL598; _LL598: _temp597=( struct Cyc_Absyn_Tunionfield*)
_temp595.tufd; goto _LL558;} else{ goto _LL559;} _LL559: if(( unsigned int)
_temp533 > 4u?(( struct _tunion_struct*) _temp533)->tag == Cyc_Absyn_XTunionFieldType_tag:
0){ _LL608: _temp607=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp533)->f1; goto _LL560;} else{ goto _LL561;} _LL561: if(( unsigned int)
_temp533 > 4u?(( struct _tunion_struct*) _temp533)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL614: _temp613=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp533)->f1; goto _LL612; _LL612: _temp611=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp533)->f2; goto _LL610; _LL610: _temp609=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp533)->f3;
goto _LL562;} else{ goto _LL563;} _LL563: if(( unsigned int) _temp533 > 4u?((
struct _tunion_struct*) _temp533)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL616:
_temp615=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp533)->f1; goto _LL564;} else{ goto _LL565;} _LL565: if(( unsigned int)
_temp533 > 4u?(( struct _tunion_struct*) _temp533)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL618: _temp617=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp533)->f1;
goto _LL566;} else{ goto _LL567;} _LL567: goto _LL568; _LL536: return 1; _LL538:
return 0; _LL540: return 1; _LL542: return 1; _LL544: return 0; _LL546: return
Cyc_Toc_atomic_typ( _temp579); _LL548: return 1; _LL550: for( 0; _temp583 != 0;
_temp583=({ struct Cyc_List_List* _temp619= _temp583; if( _temp619 == 0){ _throw(
Null_Exception);} _temp619->tl;})){ if( ! Cyc_Toc_atomic_typ((*(( struct _tuple3*)({
struct Cyc_List_List* _temp620= _temp583; if( _temp620 == 0){ _throw(
Null_Exception);} _temp620->hd;}))).f2)){ return 0;}} return 1; _LL552: return 0;
_LL554: return 0; _LL556: return 0; _LL558: return({ struct Cyc_Absyn_Tunionfield*
_temp621= _temp597; if( _temp621 == 0){ _throw( Null_Exception);} _temp621->typs;})
== 0; _LL560: return 0; _LL562: return 0; _LL564: return 0; _LL566: return 0;
_LL568:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp622= Cyc_Absynpp_typ2string( t); xprintf("atomic_typ:  bad type %.*s",
_temp622.last_plus_one - _temp622.curr, _temp622.curr);})); return 0; _LL534:;}
static int Cyc_Toc_is_void_star( void* t){ void* _temp623= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp629; struct Cyc_Absyn_Conref* _temp631; struct
Cyc_Absyn_Tqual _temp633; struct Cyc_Absyn_Conref* _temp635; void* _temp637;
void* _temp639; _LL625: if(( unsigned int) _temp623 > 4u?(( struct
_tunion_struct*) _temp623)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL630:
_temp629=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp623)->f1; _LL640: _temp639=( void*) _temp629.elt_typ; goto _LL638; _LL638:
_temp637=( void*) _temp629.rgn_typ; goto _LL636; _LL636: _temp635=( struct Cyc_Absyn_Conref*)
_temp629.nullable; goto _LL634; _LL634: _temp633=( struct Cyc_Absyn_Tqual)
_temp629.tq; goto _LL632; _LL632: _temp631=( struct Cyc_Absyn_Conref*) _temp629.bounds;
goto _LL626;} else{ goto _LL627;} _LL627: goto _LL628; _LL626: { void* _temp641=
Cyc_Tcutil_compress( _temp639); _LL643: if(( int) _temp641 == Cyc_Absyn_VoidType){
goto _LL644;} else{ goto _LL645;} _LL645: goto _LL646; _LL644: return 1; _LL646:
return 0; _LL642:;} _LL628: return 0; _LL624:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp647= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Structdecl** _temp659; struct Cyc_Absyn_Structdecl* _temp661;
struct Cyc_List_List* _temp662; struct _tuple0* _temp664; struct Cyc_Absyn_Structdecl**
_temp666; struct Cyc_List_List* _temp668; struct _tuple0* _temp670; struct Cyc_Absyn_Uniondecl**
_temp672; struct Cyc_Absyn_Uniondecl* _temp674; struct Cyc_List_List* _temp675;
struct _tuple0* _temp677; struct Cyc_Absyn_Uniondecl** _temp679; struct Cyc_List_List*
_temp681; struct _tuple0* _temp683; _LL649: if(( unsigned int) _temp647 > 4u?((
struct _tunion_struct*) _temp647)->tag == Cyc_Absyn_StructType_tag: 0){ _LL665:
_temp664=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp647)->f1;
goto _LL663; _LL663: _temp662=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp647)->f2; goto _LL660; _LL660: _temp659=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp647)->f3; if( _temp659 == 0){ goto
_LL651;} else{ _temp661=* _temp659; goto _LL650;}} else{ goto _LL651;} _LL651:
if(( unsigned int) _temp647 > 4u?(( struct _tunion_struct*) _temp647)->tag ==
Cyc_Absyn_StructType_tag: 0){ _LL671: _temp670=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp647)->f1; goto _LL669; _LL669: _temp668=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp647)->f2; goto _LL667; _LL667: _temp666=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp647)->f3;
if( _temp666 == 0){ goto _LL652;} else{ goto _LL653;}} else{ goto _LL653;}
_LL653: if(( unsigned int) _temp647 > 4u?(( struct _tunion_struct*) _temp647)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL678: _temp677=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp647)->f1; goto _LL676; _LL676: _temp675=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp647)->f2; goto _LL673; _LL673: _temp672=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp647)->f3;
if( _temp672 == 0){ goto _LL655;} else{ _temp674=* _temp672; goto _LL654;}}
else{ goto _LL655;} _LL655: if(( unsigned int) _temp647 > 4u?(( struct
_tunion_struct*) _temp647)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL684: _temp683=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp647)->f1; goto
_LL682; _LL682: _temp681=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp647)->f2; goto _LL680; _LL680: _temp679=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp647)->f3; if( _temp679 == 0){ goto
_LL656;} else{ goto _LL657;}} else{ goto _LL657;} _LL657: goto _LL658; _LL650:
if( _temp664 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp685=( char*)"StructType name not properly set";
struct _tagged_string _temp686; _temp686.curr= _temp685; _temp686.base= _temp685;
_temp686.last_plus_one= _temp685 + 33; _temp686;}));} if( _temp661->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp688= Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp687=
_temp664; if( _temp687 == 0){ _throw( Null_Exception);} _temp687;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp688.last_plus_one - _temp688.curr, _temp688.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp695= _temp661->fields;
if( _temp695 == 0){ _throw( Null_Exception);} _temp695->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp689= fields; if( _temp689 == 0){ _throw(
Null_Exception);} _temp689->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp690= fields; if( _temp690 == 0){ _throw(
Null_Exception);} _temp690->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp691= fields;
if( _temp691 == 0){ _throw( Null_Exception);} _temp691->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp693=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp692= _temp664;
if( _temp692 == 0){ _throw( Null_Exception);} _temp692;})); struct
_tagged_string _temp694=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp693.last_plus_one - _temp693.curr, _temp693.curr, _temp694.last_plus_one -
_temp694.curr, _temp694.curr);})); return 0;} _LL652: if( _temp670 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp696=( char*)"StructType name not properly set"; struct _tagged_string
_temp697; _temp697.curr= _temp696; _temp697.base= _temp696; _temp697.last_plus_one=
_temp696 + 33; _temp697;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp699= Cyc_Absynpp_qvar2string(( struct _tuple0*)({
struct _tuple0* _temp698= _temp670; if( _temp698 == 0){ _throw( Null_Exception);}
_temp698;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp699.last_plus_one
- _temp699.curr, _temp699.curr);})); return 0; _LL654: if( _temp677 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp700=( char*)"UnionType name not properly set"; struct _tagged_string
_temp701; _temp701.curr= _temp700; _temp701.base= _temp700; _temp701.last_plus_one=
_temp700 + 32; _temp701;}));} if( _temp674->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp703= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct _tuple0* _temp702= _temp677; if( _temp702 == 0){
_throw( Null_Exception);} _temp702;})); xprintf("is_poly_field:  union %.*s missing fields",
_temp703.last_plus_one - _temp703.curr, _temp703.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp710= _temp674->fields;
if( _temp710 == 0){ _throw( Null_Exception);} _temp710->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp704= fields; if( _temp704 == 0){ _throw(
Null_Exception);} _temp704->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp705= fields; if( _temp705 == 0){ _throw(
Null_Exception);} _temp705->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp706= fields;
if( _temp706 == 0){ _throw( Null_Exception);} _temp706->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp708=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct _tuple0* _temp707= _temp677;
if( _temp707 == 0){ _throw( Null_Exception);} _temp707;})); struct
_tagged_string _temp709=* f; xprintf("is_poly_field:  union %.*s has no %.*s field",
_temp708.last_plus_one - _temp708.curr, _temp708.curr, _temp709.last_plus_one -
_temp709.curr, _temp709.curr);})); return 0;} _LL656: if( _temp683 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp711=( char*)"UnionType name not properly set"; struct _tagged_string
_temp712; _temp712.curr= _temp711; _temp712.base= _temp711; _temp712.last_plus_one=
_temp711 + 32; _temp712;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp714= Cyc_Absynpp_qvar2string(( struct _tuple0*)({
struct _tuple0* _temp713= _temp683; if( _temp713 == 0){ _throw( Null_Exception);}
_temp713;})); xprintf("is_poly_field:  union %.*s missing structdecl", _temp714.last_plus_one
- _temp714.curr, _temp714.curr);})); return 0; _LL658:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp715= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp715.last_plus_one - _temp715.curr,
_temp715.curr);})); return 0; _LL648:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp716=( void*) e->r; struct _tagged_string*
_temp724; struct Cyc_Absyn_Exp* _temp726; struct _tagged_string* _temp728;
struct Cyc_Absyn_Exp* _temp730; _LL718: if((( struct _tunion_struct*) _temp716)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL727: _temp726=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp716)->f1; goto _LL725; _LL725:
_temp724=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp716)->f2; goto _LL719;} else{ goto _LL720;} _LL720: if((( struct
_tunion_struct*) _temp716)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL731:
_temp730=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp716)->f1; goto _LL729; _LL729: _temp728=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp716)->f2; goto _LL721;} else{ goto _LL722;}
_LL722: goto _LL723; _LL719: return Cyc_Toc_is_poly_field(( void*)({ struct Cyc_Core_Opt*
_temp732= _temp726->topt; if( _temp732 == 0){ _throw( Null_Exception);} _temp732->v;}),
_temp724); _LL721: { void* _temp734= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp733= _temp730->topt; if( _temp733 == 0){ _throw( Null_Exception);} _temp733->v;}));
struct Cyc_Absyn_PtrInfo _temp740; struct Cyc_Absyn_Conref* _temp742; struct Cyc_Absyn_Tqual
_temp744; struct Cyc_Absyn_Conref* _temp746; void* _temp748; void* _temp750;
_LL736: if(( unsigned int) _temp734 > 4u?(( struct _tunion_struct*) _temp734)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL741: _temp740=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp734)->f1; _LL751: _temp750=( void*)
_temp740.elt_typ; goto _LL749; _LL749: _temp748=( void*) _temp740.rgn_typ; goto
_LL747; _LL747: _temp746=( struct Cyc_Absyn_Conref*) _temp740.nullable; goto
_LL745; _LL745: _temp744=( struct Cyc_Absyn_Tqual) _temp740.tq; goto _LL743;
_LL743: _temp742=( struct Cyc_Absyn_Conref*) _temp740.bounds; goto _LL737;}
else{ goto _LL738;} _LL738: goto _LL739; _LL737: return Cyc_Toc_is_poly_field(
_temp750, _temp728); _LL739:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp753= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp752= _temp730->topt; if( _temp752 == 0){ _throw( Null_Exception);} _temp752->v;}));
xprintf("is_poly_project:  bad type %.*s", _temp753.last_plus_one - _temp753.curr,
_temp753.curr);})); return 0; _LL735:;} _LL723: return 0; _LL717:;} static char
_temp756[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp756, _temp756, _temp756 + 10u}; static struct
_tuple0 Cyc_Toc_gc_malloc_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var,
0),({ struct Cyc_List_List* _temp757=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp757->hd=( void*) s; _temp757->tl= 0; _temp757;}), 0);}
static char _temp760[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp760, _temp760, _temp760 + 17u}; static struct
_tuple0 Cyc_Toc_gc_malloc_atomic_var=( struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_atomic_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var,
0),({ struct Cyc_List_List* _temp761=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp761->hd=( void*) s; _temp761->tl= 0; _temp761;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static char _temp764[ 15u]="_region_malloc"; static struct _tagged_string
Cyc_Toc__region_malloc_string=( struct _tagged_string){ _temp764, _temp764,
_temp764 + 15u}; static struct _tuple0 Cyc_Toc_region_malloc_var=( struct
_tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__region_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_region_malloc_var,
0),({ struct Cyc_List_List* _temp765=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp765->hd=( void*) rgn; _temp765->tl=({ struct Cyc_List_List*
_temp766=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp766->hd=( void*) s; _temp766->tl= 0; _temp766;}); _temp765;}), 0);} typedef
void* Cyc_Toc_Conv; static const int Cyc_Toc_NullCheck_tag= 0; struct Cyc_Toc_NullCheck_struct{
int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr_tag= 1; struct Cyc_Toc_UntagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; int f3; } ; static const int Cyc_Toc_TagPtr_tag=
2; struct Cyc_Toc_TagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; }
; typedef void* Cyc_Toc_conv; struct _tuple7{ void* f1; void* f2; } ; static
struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp768=({ struct _tuple7
_temp767; _temp767.f1= t1; _temp767.f2= t2; _temp767;}); void* _temp774; struct
Cyc_Absyn_PtrInfo _temp776; void* _temp778; struct Cyc_Absyn_PtrInfo _temp780;
_LL770: _LL779: _temp778= _temp768.f1; if(( unsigned int) _temp778 > 4u?((
struct _tunion_struct*) _temp778)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL781:
_temp780=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp778)->f1; goto _LL775;} else{ goto _LL772;} _LL775: _temp774= _temp768.f2;
if(( unsigned int) _temp774 > 4u?(( struct _tunion_struct*) _temp774)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL777: _temp776=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp774)->f1; goto _LL771;} else{ goto
_LL772;} _LL772: goto _LL773; _LL771: { int n1=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp780.nullable); int n2=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp776.nullable); void* b1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp780.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp776.bounds); struct Cyc_List_List* convs= 0;{
struct _tuple7 _temp783=({ struct _tuple7 _temp782; _temp782.f1= b1; _temp782.f2=
b2; _temp782;}); void* _temp793; struct Cyc_Absyn_Exp* _temp795; void* _temp797;
struct Cyc_Absyn_Exp* _temp799; void* _temp801; void* _temp803; void* _temp805;
void* _temp807; struct Cyc_Absyn_Exp* _temp809; void* _temp811; struct Cyc_Absyn_Exp*
_temp813; void* _temp815; _LL785: _LL798: _temp797= _temp783.f1; if((
unsigned int) _temp797 > 1u?(( struct _tunion_struct*) _temp797)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL800: _temp799=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp797)->f1; goto _LL794;} else{ goto _LL787;} _LL794: _temp793= _temp783.f2;
if(( unsigned int) _temp793 > 1u?(( struct _tunion_struct*) _temp793)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL796: _temp795=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp793)->f1; goto _LL786;} else{ goto _LL787;}
_LL787: _LL804: _temp803= _temp783.f1; if(( int) _temp803 == Cyc_Absyn_Unknown_b){
goto _LL802;} else{ goto _LL789;} _LL802: _temp801= _temp783.f2; if(( int)
_temp801 == Cyc_Absyn_Unknown_b){ goto _LL788;} else{ goto _LL789;} _LL789:
_LL808: _temp807= _temp783.f1; if(( unsigned int) _temp807 > 1u?(( struct
_tunion_struct*) _temp807)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL810: _temp809=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp807)->f1; goto
_LL806;} else{ goto _LL791;} _LL806: _temp805= _temp783.f2; if(( int) _temp805
== Cyc_Absyn_Unknown_b){ goto _LL790;} else{ goto _LL791;} _LL791: _LL816:
_temp815= _temp783.f1; if(( int) _temp815 == Cyc_Absyn_Unknown_b){ goto _LL812;}
else{ goto _LL784;} _LL812: _temp811= _temp783.f2; if(( unsigned int) _temp811 >
1u?(( struct _tunion_struct*) _temp811)->tag == Cyc_Absyn_Upper_b_tag: 0){
_LL814: _temp813=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp811)->f1; goto _LL792;} else{ goto _LL784;} _LL786: if( n1? ! n2: 0){ convs=({
struct Cyc_List_List* _temp817=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp817->hd=( void*)(( void*)({ struct Cyc_Toc_NullCheck_struct*
_temp818=( struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp818[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp819; _temp819.tag= Cyc_Toc_NullCheck_tag;
_temp819.f1=( void*) t2; _temp819;}); _temp818;})); _temp817->tl= convs;
_temp817;});} goto _LL784; _LL788: goto _LL784; _LL790: convs=({ struct Cyc_List_List*
_temp820=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp820->hd=( void*)(( void*)({ struct Cyc_Toc_TagPtr_struct* _temp821=( struct
Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp821[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp822; _temp822.tag= Cyc_Toc_TagPtr_tag;
_temp822.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp822.f2= _temp809; _temp822;});
_temp821;})); _temp820->tl= convs; _temp820;}); goto _LL784; _LL792: convs=({
struct Cyc_List_List* _temp823=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp823->hd=( void*)(( void*)({ struct Cyc_Toc_UntagPtr_struct*
_temp824=( struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp824[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp825; _temp825.tag= Cyc_Toc_UntagPtr_tag;
_temp825.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp825.f2= _temp813; _temp825.f3=
n2; _temp825;}); _temp824;})); _temp823->tl= convs; _temp823;}); goto _LL784;
_LL784:;} return convs;} _LL773: return 0; _LL769:;}} static char _temp828[ 7u]="_throw";
static struct _tagged_string Cyc_Toc__throw_string=( struct _tagged_string){
_temp828, _temp828, _temp828 + 7u}; static struct _tuple0 Cyc_Toc__throw_var=(
struct _tuple0){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__throw_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__throw_var, 0),({
struct Cyc_List_List* _temp829=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp829->hd=( void*) e; _temp829->tl= 0; _temp829;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp830=( char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp831; _temp831.curr= _temp830; _temp831.base= _temp830;
_temp831.last_plus_one= _temp830 + 44; _temp831;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple0* a= Cyc_Toc_temp_var();
struct _tuple0* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp832= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp838; struct Cyc_Absyn_Conref* _temp840; struct Cyc_Absyn_Tqual
_temp842; struct Cyc_Absyn_Conref* _temp844; void* _temp846; void* _temp848;
_LL834: if(( unsigned int) _temp832 > 4u?(( struct _tunion_struct*) _temp832)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL839: _temp838=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp832)->f1; _LL849: _temp848=( void*)
_temp838.elt_typ; goto _LL847; _LL847: _temp846=( void*) _temp838.rgn_typ; goto
_LL845; _LL845: _temp844=( struct Cyc_Absyn_Conref*) _temp838.nullable; goto
_LL843; _LL843: _temp842=( struct Cyc_Absyn_Tqual) _temp838.tq; goto _LL841;
_LL841: _temp840=( struct Cyc_Absyn_Conref*) _temp838.bounds; goto _LL835;}
else{ goto _LL836;} _LL836: goto _LL837; _LL835: elt_typ= _temp848;{ struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp848, Cyc_Toc_mt_tq);
strt_typ= Cyc_Absyn_strct( n); goto _LL833;} _LL837: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp850=( char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp851;
_temp851.curr= _temp850; _temp851.base= _temp850; _temp851.last_plus_one=
_temp850 + 28; _temp851;})); _LL833:;} if( toplevel){ int is_string= 0;{ void*
_temp852=( void*) e->r; void* _temp858; struct _tagged_string _temp860; _LL854:
if((( struct _tunion_struct*) _temp852)->tag == Cyc_Absyn_Const_e_tag){ _LL859:
_temp858=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp852)->f1; if((
unsigned int) _temp858 > 1u?(( struct _tunion_struct*) _temp858)->tag == Cyc_Absyn_String_c_tag:
0){ _LL861: _temp860=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp858)->f1; goto _LL855;} else{ goto _LL856;}} else{ goto _LL856;} _LL856:
goto _LL857; _LL855: is_string= 1; goto _LL853; _LL857: goto _LL853; _LL853:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp865=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp865[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp866; _temp866.tag= Cyc_Absyn_ArrayType_tag;
_temp866.f1=( void*) elt_typ; _temp866.f2= Cyc_Toc_mt_tq; _temp866.f3=( struct
Cyc_Absyn_Exp*) sz; _temp866;}); _temp865;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp862=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp862->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp863=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp863[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp864; _temp864.tag= Cyc_Absyn_Var_d_tag;
_temp864.f1= vd; _temp864;}); _temp863;}), 0); _temp862->tl= Cyc_Toc_result_decls;
_temp862;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp867=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp867->hd=( void*)({ struct _tuple4* _temp872=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp872->f1= 0; _temp872->f2=
xexp; _temp872;}); _temp867->tl=({ struct Cyc_List_List* _temp868=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp868->hd=( void*)({ struct
_tuple4* _temp871=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp871->f1= 0; _temp871->f2= xexp; _temp871;}); _temp868->tl=({ struct Cyc_List_List*
_temp869=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp869->hd=( void*)({ struct _tuple4* _temp870=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp870->f1= 0; _temp870->f2= xplussz; _temp870;});
_temp869->tl= 0; _temp869;}); _temp868;}); _temp867;}), 0); return urm_exp;}}}
else{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0);
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
b, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_signed_int_exp( szvalue, 0), 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( b, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_var_exp(
a, 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( b, 0), Cyc_Toc_curr_sp, 0), Cyc_Absyn_var_exp( a, 0), 0), s,
0); s= Cyc_Absyn_declare_stmt( b, strt_typ, 0, s, 0); s= Cyc_Absyn_declare_stmt(
a, Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq),( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, e, 0), s, 0); return Cyc_Absyn_stmt_exp( s, 0);}} static struct Cyc_Absyn_Exp*
Cyc_Toc_untag_ptr_conv( int toplevel, void* t1, struct Cyc_Absyn_Exp* sz, int
nullable, struct Cyc_Absyn_Exp* e){ if( toplevel){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp873=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp874; _temp874.curr= _temp873; _temp874.base= _temp873; _temp874.last_plus_one=
_temp873 + 45; _temp874;}));}{ struct _tuple0* x= Cyc_Toc_temp_var(); int
szvalue=( int) Cyc_Evexp_eval_const_uint_exp( sz); struct Cyc_Absyn_Exp* xcurr=
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xcurr, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_add_exp( xcurr, Cyc_Absyn_signed_int_exp( szvalue, 0), 0); struct
Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_gt_exp( e1, e2, 0); struct Cyc_Absyn_Exp*
e4= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( xcurr, Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), e3, 0); struct Cyc_Absyn_Exp* e5= ! nullable? e4:
Cyc_Absyn_and_exp( Cyc_Absyn_neq_exp( xcurr, Cyc_Absyn_uint_exp( 0, 0), 0), e4,
0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_ifthenelse_stmt( e5, Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt(
0), 0), s, 0); s= Cyc_Absyn_declare_stmt( x, t1,( struct Cyc_Absyn_Exp*) e, s, 0);
return Cyc_Absyn_stmt_exp( s, 0);}} static struct Cyc_Absyn_Exp* Cyc_Toc_convert_exp(
int toplevel, void* t1, void* t2, struct Cyc_Absyn_Exp* e){{ struct Cyc_List_List*
cs= Cyc_Toc_conversion( t1, t2); for( 0; cs != 0; cs=({ struct Cyc_List_List*
_temp875= cs; if( _temp875 == 0){ _throw( Null_Exception);} _temp875->tl;})){
void* _temp877=( void*)({ struct Cyc_List_List* _temp876= cs; if( _temp876 == 0){
_throw( Null_Exception);} _temp876->hd;}); void* _temp885; struct Cyc_Absyn_Exp*
_temp887; void* _temp889; int _temp891; struct Cyc_Absyn_Exp* _temp893; void*
_temp895; _LL879: if((( struct _tunion_struct*) _temp877)->tag == Cyc_Toc_NullCheck_tag){
_LL886: _temp885=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp877)->f1;
goto _LL880;} else{ goto _LL881;} _LL881: if((( struct _tunion_struct*) _temp877)->tag
== Cyc_Toc_TagPtr_tag){ _LL890: _temp889=( void*)(( struct Cyc_Toc_TagPtr_struct*)
_temp877)->f1; goto _LL888; _LL888: _temp887=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Toc_TagPtr_struct*) _temp877)->f2; goto _LL882;} else{ goto _LL883;} _LL883:
if((( struct _tunion_struct*) _temp877)->tag == Cyc_Toc_UntagPtr_tag){ _LL896:
_temp895=( void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp877)->f1; goto _LL894;
_LL894: _temp893=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_UntagPtr_struct*)
_temp877)->f2; goto _LL892; _LL892: _temp891=( int)(( struct Cyc_Toc_UntagPtr_struct*)
_temp877)->f3; goto _LL884;} else{ goto _LL878;} _LL880: e= Cyc_Toc_null_check_conv(
toplevel, _temp885, e); goto _LL878; _LL882: e= Cyc_Toc_tag_ptr_conv( toplevel,
_temp889, _temp887, e); goto _LL878; _LL884: e= Cyc_Toc_untag_ptr_conv( toplevel,
_temp895, _temp893, _temp891, e); goto _LL878; _LL878:;}} return e;} struct Cyc_Toc_Env{
struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt*
fallthru_info; struct Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct
Cyc_Toc_Env* Cyc_Toc_env_t; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){
return({ struct Cyc_Toc_Env* _temp897=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp897->break_lab= 0; _temp897->continue_lab= 0;
_temp897->fallthru_info= 0; _temp897->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp897->toplevel= 1; _temp897;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env(
struct Cyc_Toc_Env* e){ return({ struct Cyc_Toc_Env* _temp898=( struct Cyc_Toc_Env*)
GC_malloc( sizeof( struct Cyc_Toc_Env)); _temp898->break_lab= e->break_lab;
_temp898->continue_lab= e->continue_lab; _temp898->fallthru_info= e->fallthru_info;
_temp898->varmap= e->varmap; _temp898->toplevel= e->toplevel; _temp898;});}
static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env* e, struct
_tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp899=(* x).f1; struct Cyc_List_List*
_temp905; _LL901: if(( unsigned int) _temp899 > 1u?(( struct _tunion_struct*)
_temp899)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL906: _temp905=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp899)->f1; goto _LL902;} else{ goto _LL903;}
_LL903: goto _LL904; _LL902:( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Core_Impossible_struct* _temp907=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp907[ 0]=({ struct
Cyc_Core_Impossible_struct _temp908; _temp908.tag= Cyc_Core_Impossible_tag;
_temp908.f1=({ struct _tagged_string _temp909= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp909.last_plus_one - _temp909.curr,
_temp909.curr);}); _temp908;}); _temp907;})); _LL904: goto _LL900; _LL900:;}{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)(
ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=
0; ans->continue_lab= 0; return ans;} struct _tuple8{ struct _tagged_string* f1;
struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env*
Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_string*
break_l, struct _tagged_string* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
fallthru_vars= 0; for( 0; fallthru_binders != 0; fallthru_binders=({ struct Cyc_List_List*
_temp910= fallthru_binders; if( _temp910 == 0){ _throw( Null_Exception);}
_temp910->tl;})){ fallthru_vars=({ struct Cyc_List_List* _temp911=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp911->hd=( void*)(( struct Cyc_Absyn_Vardecl*)({
struct Cyc_List_List* _temp912= fallthru_binders; if( _temp912 == 0){ _throw(
Null_Exception);} _temp912->hd;}))->name; _temp911->tl= fallthru_vars; _temp911;});}
fallthru_vars=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fallthru_vars);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp913=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp913->v=( void*) break_l; _temp913;}); ans->fallthru_info=({
struct Cyc_Core_Opt* _temp914=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp914->v=( void*)({ struct _tuple8* _temp915=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp915->f1= fallthru_l; _temp915->f2=
fallthru_vars; _temp915->f3= next_case_env->varmap; _temp915;}); _temp914;});
return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct
Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab=({ struct Cyc_Core_Opt* _temp916=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp916->v=( void*) break_l; _temp916;});
ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env* e, struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp917=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp917->v=( void*)({ struct _tuple8* _temp918=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp918->f1= next_l; _temp918->f2= 0; _temp918->f3=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp918;}); _temp917;}); return ans;} struct _tuple9{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
struct Cyc_List_List* f4; char f5; int f6; } ; struct _tuple10{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp919=( char*)"expecting a literal format string";
struct _tagged_string _temp920; _temp920.curr= _temp919; _temp920.base= _temp919;
_temp920.last_plus_one= _temp919 + 34; _temp920;}));}{ struct _tagged_string
fmt_str;{ void* _temp922=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp921= args; if( _temp921 == 0){ _throw( Null_Exception);} _temp921->hd;}))->r;
void* _temp928; struct _tagged_string _temp930; _LL924: if((( struct
_tunion_struct*) _temp922)->tag == Cyc_Absyn_Const_e_tag){ _LL929: _temp928=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp922)->f1; if(( unsigned int)
_temp928 > 1u?(( struct _tunion_struct*) _temp928)->tag == Cyc_Absyn_String_c_tag:
0){ _LL931: _temp930=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp928)->f1; goto _LL925;} else{ goto _LL926;}} else{ goto _LL926;} _LL926:
goto _LL927; _LL925: fmt_str= _temp930; goto _LL923; _LL927: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp932=( char*)"expecting a literal format string"; struct _tagged_string
_temp933; _temp933.curr= _temp932; _temp933.base= _temp932; _temp933.last_plus_one=
_temp932 + 34; _temp933;})); _LL923:;}{ int len=( int)({ struct _tagged_string
_temp1040= fmt_str;( unsigned int)( _temp1040.last_plus_one - _temp1040.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(({ struct _tagged_string _temp934=
fmt_str; char* _temp936= _temp934.curr + i; if( _temp934.base == 0? 1:( _temp936
< _temp934.base? 1: _temp936 >= _temp934.last_plus_one)){ _throw( Null_Exception);}*
_temp936;}) =='%'){ i ++; if( i < len?({ struct _tagged_string _temp937= fmt_str;
char* _temp939= _temp937.curr + i; if( _temp937.base == 0? 1:( _temp939 <
_temp937.base? 1: _temp939 >= _temp937.last_plus_one)){ _throw( Null_Exception);}*
_temp939;}) =='s': 0){ break;}}} if( i == len){ if( aopt != 0){ args=({ struct
Cyc_List_List* _temp940=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp940->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp941=
aopt; if( _temp941 == 0){ _throw( Null_Exception);} _temp941->v;})); _temp940->tl=
args; _temp940;});} return Cyc_Absyn_primop_exp( p, args, 0);} args=({ struct
Cyc_List_List* _temp942= args; if( _temp942 == 0){ _throw( Null_Exception);}
_temp942->tl;}); typs=({ struct Cyc_List_List* _temp943= typs; if( _temp943 == 0){
_throw( Null_Exception);} _temp943->tl;});{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ char c=({ struct _tagged_string _temp1030= fmt_str; char*
_temp1032= _temp1030.curr + i; if( _temp1030.base == 0? 1:( _temp1032 <
_temp1030.base? 1: _temp1032 >= _temp1030.last_plus_one)){ _throw(
Null_Exception);}* _temp1032;}); rev_fmt=({ struct Cyc_List_List* _temp944=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp944->hd=(
void*)(( int) c); _temp944->tl= rev_fmt; _temp944;}); if( c !='%'){ continue;}{
struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification( fmt_str, i
+ 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp945=( char*)"bad format string"; struct
_tagged_string _temp946; _temp946.curr= _temp945; _temp946.base= _temp945;
_temp946.last_plus_one= _temp945 + 18; _temp946;}));}{ struct _tuple9 _temp950;
int _temp951; char _temp953; struct Cyc_List_List* _temp955; struct Cyc_List_List*
_temp957; struct Cyc_List_List* _temp959; struct Cyc_List_List* _temp961; struct
_tuple9* _temp948=( struct _tuple9*)({ struct Cyc_Core_Opt* _temp947= x; if(
_temp947 == 0){ _throw( Null_Exception);} _temp947->v;}); _temp950=* _temp948;
_LL962: _temp961= _temp950.f1; goto _LL960; _LL960: _temp959= _temp950.f2; goto
_LL958; _LL958: _temp957= _temp950.f3; goto _LL956; _LL956: _temp955= _temp950.f4;
goto _LL954; _LL954: _temp953= _temp950.f5; goto _LL952; _LL952: _temp951=
_temp950.f6; goto _LL949; _LL949: i= _temp951 - 1; if( _temp953 !='s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp963=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp963->hd=( void*) _temp961; _temp963->tl=({ struct Cyc_List_List* _temp964=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp964->hd=(
void*) _temp959; _temp964->tl=({ struct Cyc_List_List* _temp965=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp965->hd=( void*) _temp957;
_temp965->tl=({ struct Cyc_List_List* _temp966=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp966->hd=( void*) _temp955;
_temp966->tl= 0; _temp966;}); _temp965;}); _temp964;}); _temp963;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp967=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp967->hd=( void*)(( int) _temp953); _temp967->tl=
rev_fmt; _temp967;});{ struct Cyc_List_List* _temp968= _temp959; struct Cyc_List_List
_temp975; struct Cyc_List_List* _temp976; int _temp978; _LL970: if( _temp968 ==
0){ goto _LL972;} else{ _temp975=* _temp968; _LL979: _temp978=( int) _temp975.hd;
goto _LL977; _LL977: _temp976=( struct Cyc_List_List*) _temp975.tl; if( _temp976
== 0){ goto _LL974;} else{ goto _LL972;}} _LL974: if( _temp978 =='*'){ goto
_LL971;} else{ goto _LL972;} _LL972: goto _LL973; _LL971: { struct _tuple0* temp=
Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp980=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp980->hd=( void*)({ struct
_tuple10* _temp981=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp981->f1= temp; _temp981->f2=( void*)({ struct Cyc_List_List* _temp983= typs;
if( _temp983 == 0){ _throw( Null_Exception);} _temp983->hd;}); _temp981->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp982= args; if( _temp982 == 0){
_throw( Null_Exception);} _temp982->hd;}); _temp981;}); _temp980->tl= rev_temps;
_temp980;}); rev_result=({ struct Cyc_List_List* _temp984=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp984->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp984->tl= rev_result; _temp984;}); args=({ struct Cyc_List_List*
_temp985= args; if( _temp985 == 0){ _throw( Null_Exception);} _temp985->tl;});
typs=({ struct Cyc_List_List* _temp986= typs; if( _temp986 == 0){ _throw(
Null_Exception);} _temp986->tl;}); goto _LL969;} _LL973: goto _LL969; _LL969:;}{
struct Cyc_List_List* _temp987= _temp957; struct Cyc_List_List _temp994; struct
Cyc_List_List* _temp995; struct Cyc_List_List _temp997; struct Cyc_List_List*
_temp998; int _temp1000; int _temp1002; _LL989: if( _temp987 == 0){ goto _LL991;}
else{ _temp994=* _temp987; _LL1003: _temp1002=( int) _temp994.hd; goto _LL996;
_LL996: _temp995=( struct Cyc_List_List*) _temp994.tl; if( _temp995 == 0){ goto
_LL991;} else{ _temp997=* _temp995; _LL1001: _temp1000=( int) _temp997.hd; goto
_LL999; _LL999: _temp998=( struct Cyc_List_List*) _temp997.tl; if( _temp998 == 0){
goto _LL993;} else{ goto _LL991;}}} _LL993: if( _temp1002 =='.'? _temp1000 =='*':
0){ goto _LL990;} else{ goto _LL991;} _LL991: goto _LL992; _LL990: { struct
_tuple0* temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp1004=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1004->hd=(
void*)({ struct _tuple10* _temp1005=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1005->f1= temp; _temp1005->f2=( void*)({ struct Cyc_List_List*
_temp1007= typs; if( _temp1007 == 0){ _throw( Null_Exception);} _temp1007->hd;});
_temp1005->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1006= args;
if( _temp1006 == 0){ _throw( Null_Exception);} _temp1006->hd;}); _temp1005;});
_temp1004->tl= rev_temps; _temp1004;}); rev_result=({ struct Cyc_List_List*
_temp1008=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1008->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp1008->tl= rev_result;
_temp1008;}); args=({ struct Cyc_List_List* _temp1009= args; if( _temp1009 == 0){
_throw( Null_Exception);} _temp1009->tl;}); typs=({ struct Cyc_List_List*
_temp1010= typs; if( _temp1010 == 0){ _throw( Null_Exception);} _temp1010->tl;});
goto _LL988;} _LL992: goto _LL988; _LL988:;} if( _temp953 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1011= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp1011.last_plus_one - _temp1011.curr, _temp1011.curr);}));}{ struct _tuple0*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp1012=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1012->hd=( void*)({
struct _tuple10* _temp1013=( struct _tuple10*) GC_malloc( sizeof( struct
_tuple10)); _temp1013->f1= temp; _temp1013->f2=( void*)({ struct Cyc_List_List*
_temp1015= typs; if( _temp1015 == 0){ _throw( Null_Exception);} _temp1015->hd;});
_temp1013->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1014= args;
if( _temp1014 == 0){ _throw( Null_Exception);} _temp1014->hd;}); _temp1013;});
_temp1012->tl= rev_temps; _temp1012;}); rev_result=({ struct Cyc_List_List*
_temp1016=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1016->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp1016->tl= rev_result;
_temp1016;}); args=({ struct Cyc_List_List* _temp1017= args; if( _temp1017 == 0){
_throw( Null_Exception);} _temp1017->tl;}); typs=({ struct Cyc_List_List*
_temp1018= typs; if( _temp1018 == 0){ _throw( Null_Exception);} _temp1018->tl;});}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp1019=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1019->hd=( void*)(( int)'.');
_temp1019->tl= rev_fmt; _temp1019;}); rev_fmt=({ struct Cyc_List_List* _temp1020=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1020->hd=(
void*)(( int)'*'); _temp1020->tl= rev_fmt; _temp1020;}); rev_fmt=({ struct Cyc_List_List*
_temp1021=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1021->hd=( void*)(( int)'s'); _temp1021->tl= rev_fmt; _temp1021;});{ struct
_tuple0* temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp1022=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1022->hd=(
void*)({ struct _tuple10* _temp1023=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1023->f1= temp; _temp1023->f2=( void*)({ struct Cyc_List_List*
_temp1025= typs; if( _temp1025 == 0){ _throw( Null_Exception);} _temp1025->hd;});
_temp1023->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1024= args;
if( _temp1024 == 0){ _throw( Null_Exception);} _temp1024->hd;}); _temp1023;});
_temp1022->tl= rev_temps; _temp1022;});{ struct Cyc_Absyn_Exp* size_exp= Cyc_Absyn_subtract_exp(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp,
0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0),
0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List* _temp1026=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1026->hd=(
void*) size_exp; _temp1026->tl= rev_result; _temp1026;}); rev_result=({ struct
Cyc_List_List* _temp1027=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1027->hd=( void*) cstring_exp; _temp1027->tl= rev_result; _temp1027;});
args=({ struct Cyc_List_List* _temp1028= args; if( _temp1028 == 0){ _throw(
Null_Exception);} _temp1028->tl;}); typs=({ struct Cyc_List_List* _temp1029=
typs; if( _temp1029 == 0){ _throw( Null_Exception);} _temp1029->tl;});}}}}}}{
struct Cyc_List_List* es=({ struct Cyc_List_List* _temp1039=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1039->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp1039->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_result); _temp1039;}); if( aopt != 0){ es=({ struct Cyc_List_List*
_temp1033=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1033->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp1034=
aopt; if( _temp1034 == 0){ _throw( Null_Exception);} _temp1034->v;})); _temp1033->tl=
es; _temp1033;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p, es, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps != 0;
rev_temps=({ struct Cyc_List_List* _temp1035= rev_temps; if( _temp1035 == 0){
_throw( Null_Exception);} _temp1035->tl;})){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)({ struct Cyc_List_List* _temp1036= rev_temps; if( _temp1036 ==
0){ _throw( Null_Exception);} _temp1036->hd;}))).f1,(*(( struct _tuple10*)({
struct Cyc_List_List* _temp1037= rev_temps; if( _temp1037 == 0){ _throw(
Null_Exception);} _temp1037->hd;}))).f2,( struct Cyc_Absyn_Exp*)(*(( struct
_tuple10*)({ struct Cyc_List_List* _temp1038= rev_temps; if( _temp1038 == 0){
_throw( Null_Exception);} _temp1038->hd;}))).f3, s, 0);} return Cyc_Absyn_stmt_exp(
s, 0);}}}}}} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1041=( char*)"Missing type in primop "; struct
_tagged_string _temp1042; _temp1042.curr= _temp1041; _temp1042.base= _temp1041;
_temp1042.last_plus_one= _temp1041 + 24; _temp1042;}));} return Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp1043= e->topt; if( _temp1043 == 0){ _throw(
Null_Exception);} _temp1043->v;}));} static void* Cyc_Toc_get_cyc_typ( struct
Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1044=( char*)"Missing type in primop ";
struct _tagged_string _temp1045; _temp1045.curr= _temp1044; _temp1045.base=
_temp1044; _temp1045.last_plus_one= _temp1044 + 24; _temp1045;}));} return( void*)({
struct Cyc_Core_Opt* _temp1046= e->topt; if( _temp1046 == 0){ _throw(
Null_Exception);} _temp1046->v;});} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1047=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1047->f1= Cyc_Toc_mt_tq; _temp1047->f2=
Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1048= e->topt; if(
_temp1048 == 0){ _throw( Null_Exception);} _temp1048->v;})); _temp1047;});}
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e);
static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp1049=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1049[ 0]=({
struct _tuple4 _temp1050; _temp1050.f1= 0; _temp1050.f2= e; _temp1050;});
_temp1049;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
se= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic( se): Cyc_Toc_malloc_ptr(
se), 0);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1051= rgnopt; if( _temp1051 == 0){ _throw( Null_Exception);} _temp1051;});
Cyc_Toc_exp_to_c( nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, se), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles0); for( 0; dles != 0; dles=({ struct Cyc_List_List*
_temp1052= dles; if( _temp1052 == 0){ _throw( Null_Exception);} _temp1052->tl;})){
struct _tuple4 _temp1056; struct Cyc_Absyn_Exp* _temp1057; struct Cyc_List_List*
_temp1059; struct _tuple4* _temp1054=( struct _tuple4*)({ struct Cyc_List_List*
_temp1053= dles; if( _temp1053 == 0){ _throw( Null_Exception);} _temp1053->hd;});
_temp1056=* _temp1054; _LL1060: _temp1059= _temp1056.f1; goto _LL1058; _LL1058:
_temp1057= _temp1056.f2; goto _LL1055; _LL1055: { struct Cyc_Absyn_Exp* e_index;
if( _temp1059 == 0){ e_index= Cyc_Absyn_signed_int_exp(( count --), 0);} else{
if(({ struct Cyc_List_List* _temp1061= _temp1059; if( _temp1061 == 0){ _throw(
Null_Exception);} _temp1061->tl;}) != 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1062=( char*)"multiple designators in array";
struct _tagged_string _temp1063; _temp1063.curr= _temp1062; _temp1063.base=
_temp1062; _temp1063.last_plus_one= _temp1062 + 30; _temp1063;}));}{ void* d=(
void*)({ struct Cyc_List_List* _temp1076= _temp1059; if( _temp1076 == 0){ _throw(
Null_Exception);} _temp1076->hd;}); void* _temp1064= d; struct Cyc_Absyn_Exp*
_temp1070; struct _tagged_string* _temp1072; _LL1066: if((( struct
_tunion_struct*) _temp1064)->tag == Cyc_Absyn_ArrayElement_tag){ _LL1071:
_temp1070=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1064)->f1; goto _LL1067;} else{ goto _LL1068;} _LL1068: if((( struct
_tunion_struct*) _temp1064)->tag == Cyc_Absyn_FieldName_tag){ _LL1073: _temp1072=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1064)->f1;
goto _LL1069;} else{ goto _LL1065;} _LL1067: Cyc_Toc_exp_to_c( nv, _temp1070);
e_index= _temp1070; goto _LL1065; _LL1069: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1074=( char*)"field name designators in array"; struct _tagged_string
_temp1075; _temp1075.curr= _temp1074; _temp1075.base= _temp1074; _temp1075.last_plus_one=
_temp1074 + 32; _temp1075;})); goto _LL1065; _LL1065:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp1077=( void*)
_temp1057->r; struct Cyc_List_List* _temp1085; struct Cyc_Absyn_Exp* _temp1087;
struct Cyc_Absyn_Exp* _temp1089; struct Cyc_Absyn_Vardecl* _temp1091; _LL1079:
if((( struct _tunion_struct*) _temp1077)->tag == Cyc_Absyn_Array_e_tag){ _LL1086:
_temp1085=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1077)->f1;
goto _LL1080;} else{ goto _LL1081;} _LL1081: if((( struct _tunion_struct*)
_temp1077)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL1092: _temp1091=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1077)->f1;
goto _LL1090; _LL1090: _temp1089=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1077)->f2; goto _LL1088; _LL1088: _temp1087=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1077)->f3; goto _LL1082;} else{
goto _LL1083;} _LL1083: goto _LL1084; _LL1080: s= Cyc_Toc_init_array( nv, lval,
_temp1085, s); goto _LL1078; _LL1082: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1091, _temp1089, _temp1087, s, 0); goto _LL1078; _LL1084: Cyc_Toc_exp_to_c(
nv, _temp1057); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp1057, 0), s, 0); goto _LL1078; _LL1078:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* x1= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp( x1,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1109=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1109[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1110; _temp1110.tag= Cyc_Absyn_Local_b_tag;
_temp1110.f1= vd; _temp1110;}); _temp1109;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_var_exp( max, 0), 0);
struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( x1, 0), 0);
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( x1,
0), 0); struct Cyc_Absyn_Stmt* body;{ void* _temp1093=( void*) e2->r; struct Cyc_List_List*
_temp1101; struct Cyc_Absyn_Exp* _temp1103; struct Cyc_Absyn_Exp* _temp1105;
struct Cyc_Absyn_Vardecl* _temp1107; _LL1095: if((( struct _tunion_struct*)
_temp1093)->tag == Cyc_Absyn_Array_e_tag){ _LL1102: _temp1101=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1093)->f1; goto _LL1096;} else{ goto
_LL1097;} _LL1097: if((( struct _tunion_struct*) _temp1093)->tag == Cyc_Absyn_Comprehension_e_tag){
_LL1108: _temp1107=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1093)->f1; goto _LL1106; _LL1106: _temp1105=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1093)->f2; goto _LL1104; _LL1104:
_temp1103=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1093)->f3; goto _LL1098;} else{ goto _LL1099;} _LL1099: goto _LL1100;
_LL1096: body= Cyc_Toc_init_array( nv2, lval, _temp1101, Cyc_Absyn_skip_stmt( 0));
goto _LL1094; _LL1098: body= Cyc_Toc_init_comprehension( nv2, lval, _temp1107,
_temp1105, _temp1103, Cyc_Absyn_skip_stmt( 0), 0); goto _LL1094; _LL1100: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL1094; _LL1094:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
x1, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct Cyc_List_List* tqs=(( struct Cyc_List_List*(*)(
struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, es); struct _tagged_string* n= Cyc_Toc_add_tuple_type( tqs);
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(*
lval_f)( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( es);{
int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es); for( 0; es !=
0; es=({ struct Cyc_List_List* _temp1111= es; if( _temp1111 == 0){ _throw(
Null_Exception);} _temp1111->tl;}), -- i){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1131= es; if( _temp1131 == 0){ _throw( Null_Exception);}
_temp1131->hd;}); struct Cyc_Absyn_Exp* lval= lval_f( xexp,({ struct
_tagged_string* _temp1130=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1130[ 0]= xprintf("f%d", i); _temp1130;}), 0);
is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)({ struct Cyc_Core_Opt*
_temp1112= e->topt; if( _temp1112 == 0){ _throw( Null_Exception);} _temp1112->v;})):
0;{ void* _temp1113=( void*) e->r; struct Cyc_List_List* _temp1121; struct Cyc_Absyn_Exp*
_temp1123; struct Cyc_Absyn_Exp* _temp1125; struct Cyc_Absyn_Vardecl* _temp1127;
_LL1115: if((( struct _tunion_struct*) _temp1113)->tag == Cyc_Absyn_Array_e_tag){
_LL1122: _temp1121=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1113)->f1; goto _LL1116;} else{ goto _LL1117;} _LL1117: if((( struct
_tunion_struct*) _temp1113)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL1128:
_temp1127=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1113)->f1; goto _LL1126; _LL1126: _temp1125=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1113)->f2; goto _LL1124; _LL1124:
_temp1123=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1113)->f3; goto _LL1118;} else{ goto _LL1119;} _LL1119: goto _LL1120;
_LL1116: s= Cyc_Toc_init_array( nv, lval, _temp1121, s); goto _LL1114; _LL1118:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1127, _temp1125, _temp1123, s, 0);
goto _LL1114; _LL1120: Cyc_Toc_exp_to_c( nv, e); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval_f( xexp,({ struct _tagged_string* _temp1129=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp1129[ 0]=
xprintf("f%d", i); _temp1129;}), 0), e, 0), 0), s, 0); goto _LL1114; _LL1114:;}}}
return Cyc_Toc_make_struct( nv, x, Cyc_Absyn_strct( n), s, pointer, rgnopt,
is_atomic);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Exp*(* lval_f)( struct Cyc_Absyn_Exp*, struct
_tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1;{ struct Cyc_List_List* es=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( dles); for( 0;
es != 0; es=({ struct Cyc_List_List* _temp1132= es; if( _temp1132 == 0){ _throw(
Null_Exception);} _temp1132->tl;})){ struct _tuple4 _temp1136; struct Cyc_Absyn_Exp*
_temp1137; struct Cyc_List_List* _temp1139; struct _tuple4* _temp1134=( struct
_tuple4*)({ struct Cyc_List_List* _temp1133= es; if( _temp1133 == 0){ _throw(
Null_Exception);} _temp1133->hd;}); _temp1136=* _temp1134; _LL1140: _temp1139=
_temp1136.f1; goto _LL1138; _LL1138: _temp1137= _temp1136.f2; goto _LL1135;
_LL1135: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)({ struct Cyc_Core_Opt*
_temp1141= _temp1137->topt; if( _temp1141 == 0){ _throw( Null_Exception);}
_temp1141->v;})): 0; if( _temp1139 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1142=( char*)"empty designator list";
struct _tagged_string _temp1143; _temp1143.curr= _temp1142; _temp1143.base=
_temp1142; _temp1143.last_plus_one= _temp1142 + 22; _temp1143;}));} if(({ struct
Cyc_List_List* _temp1144= _temp1139; if( _temp1144 == 0){ _throw( Null_Exception);}
_temp1144->tl;}) != 0){ struct _tuple0* y= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
yexp= Cyc_Absyn_var_exp( y, 0); for( 0; _temp1139 != 0; _temp1139=({ struct Cyc_List_List*
_temp1145= _temp1139; if( _temp1145 == 0){ _throw( Null_Exception);} _temp1145->tl;})){
void* _temp1147=( void*)({ struct Cyc_List_List* _temp1146= _temp1139; if(
_temp1146 == 0){ _throw( Null_Exception);} _temp1146->hd;}); struct
_tagged_string* _temp1153; _LL1149: if((( struct _tunion_struct*) _temp1147)->tag
== Cyc_Absyn_FieldName_tag){ _LL1154: _temp1153=( struct _tagged_string*)((
struct Cyc_Absyn_FieldName_struct*) _temp1147)->f1; goto _LL1150;} else{ goto
_LL1151;} _LL1151: goto _LL1152; _LL1150: if( Cyc_Toc_is_poly_field( struct_type,
_temp1153)){ yexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), yexp, 0);} s=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f( xexp,
_temp1153, 0), yexp, 0), 0), s, 0); goto _LL1148; _LL1152:( void) _throw(((
struct _xtunion_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1155=( char*)"array designator in struct";
struct _tagged_string _temp1156; _temp1156.curr= _temp1155; _temp1156.base=
_temp1155; _temp1156.last_plus_one= _temp1155 + 27; _temp1156;}))); _LL1148:;}
Cyc_Toc_exp_to_c( nv, _temp1137); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
yexp, _temp1137, 0), 0), s, 0);} else{ void* _temp1158=( void*)({ struct Cyc_List_List*
_temp1157= _temp1139; if( _temp1157 == 0){ _throw( Null_Exception);} _temp1157->hd;});
struct _tagged_string* _temp1164; _LL1160: if((( struct _tunion_struct*)
_temp1158)->tag == Cyc_Absyn_FieldName_tag){ _LL1165: _temp1164=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1158)->f1; goto
_LL1161;} else{ goto _LL1162;} _LL1162: goto _LL1163; _LL1161: { struct Cyc_Absyn_Exp*
lval= lval_f( xexp, _temp1164, 0);{ void* _temp1166=( void*) _temp1137->r;
struct Cyc_List_List* _temp1174; struct Cyc_Absyn_Exp* _temp1176; struct Cyc_Absyn_Exp*
_temp1178; struct Cyc_Absyn_Vardecl* _temp1180; _LL1168: if((( struct
_tunion_struct*) _temp1166)->tag == Cyc_Absyn_Array_e_tag){ _LL1175: _temp1174=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1166)->f1; goto
_LL1169;} else{ goto _LL1170;} _LL1170: if((( struct _tunion_struct*) _temp1166)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1181: _temp1180=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1166)->f1; goto _LL1179; _LL1179:
_temp1178=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1166)->f2; goto _LL1177; _LL1177: _temp1176=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1166)->f3; goto _LL1171;} else{
goto _LL1172;} _LL1172: goto _LL1173; _LL1169: s= Cyc_Toc_init_array( nv, lval,
_temp1174, s); goto _LL1167; _LL1171: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1180, _temp1178, _temp1176, s, 0); goto _LL1167; _LL1173: Cyc_Toc_exp_to_c(
nv, _temp1137); if( Cyc_Toc_is_poly_field( struct_type, _temp1164)){ _temp1137=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1137, 0);} s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval, _temp1137, 0), 0), s, 0); goto
_LL1167; _LL1167:;} goto _LL1159;} _LL1163:( void) _throw((( struct
_xtunion_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1182=( char*)"array designator in struct"; struct
_tagged_string _temp1183; _temp1183.curr= _temp1182; _temp1183.base= _temp1182;
_temp1183.last_plus_one= _temp1182 + 27; _temp1183;}))); _LL1159:;}}} return Cyc_Toc_make_struct(
nv, x, Cyc_Absyn_strctq( tdn), s, pointer, rgnopt, is_atomic);} struct _tuple11{
struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return
Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp(
e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp*
e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp1184=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp1184[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp1185; _temp1185.tag=
Cyc_Absyn_Increment_e_tag; _temp1185.f1= e1; _temp1185.f2=( void*) incr;
_temp1185;}); _temp1184;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct
Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct
Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp1186=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp1196; struct Cyc_Absyn_Exp* _temp1198; void* _temp1200; struct
_tagged_string* _temp1202; struct Cyc_Absyn_Exp* _temp1204; _LL1188: if(((
struct _tunion_struct*) _temp1186)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1197:
_temp1196=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1186)->f1; goto _LL1189;} else{ goto _LL1190;} _LL1190: if((( struct
_tunion_struct*) _temp1186)->tag == Cyc_Absyn_Cast_e_tag){ _LL1201: _temp1200=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1186)->f1; goto _LL1199; _LL1199:
_temp1198=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1186)->f2;
goto _LL1191;} else{ goto _LL1192;} _LL1192: if((( struct _tunion_struct*)
_temp1186)->tag == Cyc_Absyn_StructMember_e_tag){ _LL1205: _temp1204=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp1186)->f1; goto
_LL1203; _LL1203: _temp1202=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1186)->f2; goto _LL1193;} else{ goto _LL1194;} _LL1194: goto _LL1195;
_LL1189:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1196, fs, f, f_env); goto _LL1187; _LL1191:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1198, fs, f, f_env); goto
_LL1187; _LL1193:( void*)( e1->r=( void*)(( void*) _temp1204->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1206=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1206->hd=( void*) _temp1202; _temp1206->tl= fs;
_temp1206;}), f, f_env); goto _LL1187; _LL1195: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1207= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1207.last_plus_one - _temp1207.curr, _temp1207.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp1208= fs; if( _temp1208 == 0){ _throw( Null_Exception);} _temp1208->tl;})){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_string*)({ struct
Cyc_List_List* _temp1209= fs; if( _temp1209 == 0){ _throw( Null_Exception);}
_temp1209->hd;}), e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy,
f_env))->r)); goto _LL1187;} _LL1187:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp1210=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1220; struct Cyc_Absyn_Stmt* _temp1222; struct Cyc_Absyn_Decl*
_temp1224; struct Cyc_Absyn_Stmt* _temp1226; struct Cyc_Absyn_Stmt* _temp1228;
_LL1212: if(( unsigned int) _temp1210 > 1u?(( struct _tunion_struct*) _temp1210)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL1221: _temp1220=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp_s_struct*) _temp1210)->f1; goto _LL1213;} else{ goto _LL1214;}
_LL1214: if(( unsigned int) _temp1210 > 1u?(( struct _tunion_struct*) _temp1210)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL1225: _temp1224=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1210)->f1; goto _LL1223; _LL1223:
_temp1222=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1210)->f2;
goto _LL1215;} else{ goto _LL1216;} _LL1216: if(( unsigned int) _temp1210 > 1u?((
struct _tunion_struct*) _temp1210)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1229:
_temp1228=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1210)->f1;
goto _LL1227; _LL1227: _temp1226=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1210)->f2; goto _LL1217;} else{ goto _LL1218;} _LL1218: goto _LL1219;
_LL1213:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1220, fs, f, f_env); goto _LL1211; _LL1215:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1222, fs, f, f_env);
goto _LL1211; _LL1217:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1226, fs, f, f_env); goto _LL1211; _LL1219:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1230= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1230.last_plus_one - _temp1230.curr,
_temp1230.curr);})); goto _LL1211; _LL1211:;} static struct Cyc_Absyn_Exp* Cyc_Toc_known_size_subscript(
struct Cyc_Absyn_Exp* arr, struct Cyc_Absyn_Exp* ind, int possibly_null, struct
Cyc_Absyn_Exp* esz, void* elt_typ, struct Cyc_Absyn_Tqual tq, int take_address){
unsigned int sz= Cyc_Evexp_eval_const_uint_exp( esz); struct _tuple0* a= Cyc_Toc_temp_var();
struct _tuple0* i= Cyc_Toc_temp_var(); void* ta= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
elt_typ), tq); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte,
Cyc_Absyn_var_exp( i, 0), Cyc_Absyn_uint_exp( sz, 0), 0); if( possibly_null){ e3=
Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_new_exp(( void*) e3->r, 0), 0);}{ struct Cyc_Absyn_Exp* e4=
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_ifthenelse_stmt(
e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0), 0); if( take_address){ e5=
Cyc_Absyn_address_exp( e5, 0);}{ struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt(
e5, 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) ind, s3,
0); return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta,( struct Cyc_Absyn_Exp*)
arr, s1, 0), 0);}}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Stmt* s); struct _tuple12{ struct Cyc_Absyn_Structfield* f1;
struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* r=( void*) e->r; if( e->topt == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1231=
Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s", _temp1231.last_plus_one
- _temp1231.curr, _temp1231.curr);})); return;}{ void* old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1938= e->topt; if( _temp1938 == 0){ _throw( Null_Exception);}
_temp1938->v;}); void* _temp1232= r; void* _temp1308; void* _temp1310; void*
_temp1312; struct _tuple0* _temp1314; struct _tuple0* _temp1316; struct Cyc_List_List*
_temp1318; void* _temp1320; void* _temp1322; struct Cyc_Absyn_Exp* _temp1324;
struct Cyc_Absyn_Exp* _temp1326; struct Cyc_Core_Opt* _temp1328; struct Cyc_Absyn_Exp*
_temp1330; struct Cyc_Absyn_Exp* _temp1332; struct Cyc_Absyn_Exp* _temp1334;
struct Cyc_Absyn_Exp* _temp1336; struct Cyc_Absyn_Exp* _temp1338; struct Cyc_Absyn_Exp*
_temp1340; struct Cyc_List_List* _temp1342; struct Cyc_Absyn_Exp* _temp1344;
struct Cyc_List_List* _temp1346; struct Cyc_Absyn_Exp* _temp1348; struct Cyc_Absyn_Exp*
_temp1350; struct Cyc_Absyn_Exp* _temp1352; struct Cyc_List_List* _temp1354;
struct Cyc_Absyn_Exp* _temp1356; struct Cyc_Absyn_Exp* _temp1358; struct Cyc_Absyn_Exp**
_temp1360; void* _temp1361; void** _temp1363; struct Cyc_Absyn_Exp* _temp1364;
struct Cyc_Absyn_Exp* _temp1366; struct Cyc_Absyn_Exp* _temp1368; void*
_temp1370; struct Cyc_Absyn_Exp* _temp1372; struct Cyc_Absyn_Exp* _temp1374;
struct _tagged_string* _temp1376; struct Cyc_Absyn_Exp* _temp1378; struct
_tagged_string* _temp1380; struct Cyc_Absyn_Exp* _temp1382; struct Cyc_Absyn_Exp*
_temp1384; struct Cyc_Absyn_Exp* _temp1386; struct Cyc_List_List* _temp1388;
struct Cyc_List_List* _temp1390; struct _tuple1* _temp1392; struct Cyc_List_List*
_temp1394; struct Cyc_Absyn_Exp* _temp1396; struct Cyc_Absyn_Exp* _temp1398;
struct Cyc_Absyn_Vardecl* _temp1400; struct Cyc_Absyn_Structdecl* _temp1402;
struct Cyc_List_List* _temp1404; struct Cyc_Core_Opt* _temp1406; struct _tuple0*
_temp1408; struct Cyc_Absyn_Tunionfield* _temp1410; struct Cyc_Absyn_Tuniondecl*
_temp1412; struct Cyc_List_List* _temp1414; struct Cyc_Core_Opt* _temp1416;
struct Cyc_Core_Opt* _temp1418; struct Cyc_Absyn_Tunionfield* _temp1420; struct
Cyc_Absyn_Tuniondecl* _temp1422; struct Cyc_List_List* _temp1424; struct Cyc_Core_Opt*
_temp1426; struct Cyc_Core_Opt* _temp1428; struct Cyc_Absyn_Tunionfield*
_temp1430; struct Cyc_Absyn_XTuniondecl* _temp1432; struct Cyc_List_List*
_temp1434; struct Cyc_Core_Opt* _temp1436; struct Cyc_Absyn_Enumfield* _temp1438;
struct Cyc_Absyn_Enumdecl* _temp1440; struct _tuple0* _temp1442; void* _temp1444;
struct Cyc_Absyn_Exp* _temp1446; struct Cyc_Absyn_Stmt* _temp1448; struct Cyc_List_List*
_temp1450; struct Cyc_Core_Opt* _temp1452; struct Cyc_Absyn_Fndecl* _temp1454;
struct Cyc_Absyn_Exp* _temp1456; _LL1234: if((( struct _tunion_struct*)
_temp1232)->tag == Cyc_Absyn_Const_e_tag){ _LL1309: _temp1308=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp1232)->f1; if(( int) _temp1308 == Cyc_Absyn_Null_c){
goto _LL1235;} else{ goto _LL1236;}} else{ goto _LL1236;} _LL1236: if((( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_Const_e_tag){ _LL1311: _temp1310=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1232)->f1; goto _LL1237;} else{
goto _LL1238;} _LL1238: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_Var_e_tag){
_LL1315: _temp1314=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1232)->f1; goto _LL1313; _LL1313: _temp1312=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1232)->f2; goto _LL1239;} else{ goto _LL1240;} _LL1240: if((( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1317:
_temp1316=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1232)->f1;
goto _LL1241;} else{ goto _LL1242;} _LL1242: if((( struct _tunion_struct*)
_temp1232)->tag == Cyc_Absyn_Primop_e_tag){ _LL1321: _temp1320=( void*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp1232)->f1; goto _LL1319; _LL1319: _temp1318=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1232)->f2; goto
_LL1243;} else{ goto _LL1244;} _LL1244: if((( struct _tunion_struct*) _temp1232)->tag
== Cyc_Absyn_Increment_e_tag){ _LL1325: _temp1324=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp1232)->f1; goto _LL1323; _LL1323:
_temp1322=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1232)->f2; goto
_LL1245;} else{ goto _LL1246;} _LL1246: if((( struct _tunion_struct*) _temp1232)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL1331: _temp1330=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1232)->f1; goto _LL1329; _LL1329:
_temp1328=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1232)->f2; goto _LL1327; _LL1327: _temp1326=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1232)->f3; goto _LL1247;} else{ goto
_LL1248;} _LL1248: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_Conditional_e_tag){
_LL1337: _temp1336=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1232)->f1; goto _LL1335; _LL1335: _temp1334=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1232)->f2; goto _LL1333; _LL1333:
_temp1332=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1232)->f3; goto _LL1249;} else{ goto _LL1250;} _LL1250: if((( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1341: _temp1340=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1232)->f1; goto
_LL1339; _LL1339: _temp1338=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1232)->f2; goto _LL1251;} else{ goto _LL1252;} _LL1252: if((( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1345:
_temp1344=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1232)->f1; goto _LL1343; _LL1343: _temp1342=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1232)->f2; goto _LL1253;} else{
goto _LL1254;} _LL1254: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1349: _temp1348=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1232)->f1; goto _LL1347; _LL1347: _temp1346=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1232)->f2; goto _LL1255;} else{ goto
_LL1256;} _LL1256: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_Throw_e_tag){
_LL1351: _temp1350=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp1232)->f1; goto _LL1257;} else{ goto _LL1258;} _LL1258: if((( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1353:
_temp1352=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1232)->f1; goto _LL1259;} else{ goto _LL1260;} _LL1260: if((( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1357:
_temp1356=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1232)->f1; goto _LL1355; _LL1355: _temp1354=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1232)->f2; goto _LL1261;} else{
goto _LL1262;} _LL1262: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_Cast_e_tag){
_LL1362: _temp1361=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1232)->f1;
_temp1363=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1232)->f1; goto _LL1359;
_LL1359: _temp1358=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1232)->f2; _temp1360=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1232)->f2;
goto _LL1263;} else{ goto _LL1264;} _LL1264: if((( struct _tunion_struct*)
_temp1232)->tag == Cyc_Absyn_Address_e_tag){ _LL1365: _temp1364=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1232)->f1; goto _LL1265;} else{ goto
_LL1266;} _LL1266: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_New_e_tag){
_LL1369: _temp1368=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1232)->f1; goto _LL1367; _LL1367: _temp1366=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1232)->f2; goto _LL1267;} else{ goto
_LL1268;} _LL1268: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL1371: _temp1370=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1232)->f1;
goto _LL1269;} else{ goto _LL1270;} _LL1270: if((( struct _tunion_struct*)
_temp1232)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL1373: _temp1372=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp1232)->f1; goto _LL1271;} else{ goto
_LL1272;} _LL1272: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_Deref_e_tag){
_LL1375: _temp1374=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp1232)->f1; goto _LL1273;} else{ goto _LL1274;} _LL1274: if((( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_StructMember_e_tag){ _LL1379:
_temp1378=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1232)->f1; goto _LL1377; _LL1377: _temp1376=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1232)->f2; goto _LL1275;} else{
goto _LL1276;} _LL1276: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1383: _temp1382=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1232)->f1; goto _LL1381; _LL1381: _temp1380=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1232)->f2; goto _LL1277;} else{
goto _LL1278;} _LL1278: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1387: _temp1386=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1232)->f1; goto _LL1385; _LL1385: _temp1384=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1232)->f2; goto _LL1279;} else{ goto
_LL1280;} _LL1280: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1389: _temp1388=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1232)->f1; goto _LL1281;} else{ goto _LL1282;} _LL1282: if((( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1393:
_temp1392=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1232)->f1;
goto _LL1391; _LL1391: _temp1390=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1232)->f2; goto _LL1283;} else{ goto _LL1284;} _LL1284: if((( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_Array_e_tag){ _LL1395: _temp1394=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1232)->f1; goto
_LL1285;} else{ goto _LL1286;} _LL1286: if((( struct _tunion_struct*) _temp1232)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1401: _temp1400=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1232)->f1; goto _LL1399; _LL1399:
_temp1398=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1232)->f2; goto _LL1397; _LL1397: _temp1396=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1232)->f3; goto _LL1287;} else{
goto _LL1288;} _LL1288: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_Struct_e_tag){
_LL1409: _temp1408=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1232)->f1; goto _LL1407; _LL1407: _temp1406=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1232)->f2; goto _LL1405; _LL1405:
_temp1404=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1232)->f3; goto _LL1403; _LL1403: _temp1402=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1232)->f4; goto _LL1289;} else{ goto
_LL1290;} _LL1290: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_Tunion_e_tag){
_LL1419: _temp1418=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1232)->f1; goto _LL1417; _LL1417: _temp1416=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1232)->f2; goto _LL1415; _LL1415:
_temp1414=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1232)->f3; if( _temp1414 == 0){ goto _LL1413;} else{ goto _LL1292;} _LL1413:
_temp1412=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1232)->f4; goto _LL1411; _LL1411: _temp1410=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1232)->f5; goto _LL1291;} else{ goto
_LL1292;} _LL1292: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_Tunion_e_tag){
_LL1429: _temp1428=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1232)->f1; goto _LL1427; _LL1427: _temp1426=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1232)->f2; goto _LL1425; _LL1425:
_temp1424=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1232)->f3; goto _LL1423; _LL1423: _temp1422=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1232)->f4; goto _LL1421; _LL1421:
_temp1420=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1232)->f5; goto _LL1293;} else{ goto _LL1294;} _LL1294: if((( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1437: _temp1436=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1232)->f1; goto
_LL1435; _LL1435: _temp1434=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1232)->f2; goto _LL1433; _LL1433: _temp1432=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1232)->f3; goto _LL1431; _LL1431:
_temp1430=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1232)->f4; goto _LL1295;} else{ goto _LL1296;} _LL1296: if((( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_Enum_e_tag){ _LL1443: _temp1442=(
struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1232)->f1; goto _LL1441;
_LL1441: _temp1440=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1232)->f2; goto _LL1439; _LL1439: _temp1438=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1232)->f3; goto _LL1297;} else{ goto
_LL1298;} _LL1298: if((( struct _tunion_struct*) _temp1232)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1447: _temp1446=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1232)->f1; goto _LL1445; _LL1445: _temp1444=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1232)->f2; goto _LL1299;} else{ goto _LL1300;} _LL1300: if((( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1449: _temp1448=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1232)->f1;
goto _LL1301;} else{ goto _LL1302;} _LL1302: if((( struct _tunion_struct*)
_temp1232)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1453: _temp1452=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1232)->f1; goto
_LL1451; _LL1451: _temp1450=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1232)->f2; goto _LL1303;} else{ goto _LL1304;} _LL1304: if((( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1455: _temp1454=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp1232)->f1;
goto _LL1305;} else{ goto _LL1306;} _LL1306: if((( struct _tunion_struct*)
_temp1232)->tag == Cyc_Absyn_Fill_e_tag){ _LL1457: _temp1456=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp1232)->f1; goto _LL1307;} else{ goto
_LL1233;} _LL1235:{ void* _temp1458= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1464; struct Cyc_Absyn_Conref* _temp1466; struct Cyc_Absyn_Tqual _temp1468;
struct Cyc_Absyn_Conref* _temp1470; void* _temp1472; void* _temp1474; _LL1460:
if(( unsigned int) _temp1458 > 4u?(( struct _tunion_struct*) _temp1458)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1465: _temp1464=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1458)->f1; _LL1475: _temp1474=( void*)
_temp1464.elt_typ; goto _LL1473; _LL1473: _temp1472=( void*) _temp1464.rgn_typ;
goto _LL1471; _LL1471: _temp1470=( struct Cyc_Absyn_Conref*) _temp1464.nullable;
goto _LL1469; _LL1469: _temp1468=( struct Cyc_Absyn_Tqual) _temp1464.tq; goto
_LL1467; _LL1467: _temp1466=( struct Cyc_Absyn_Conref*) _temp1464.bounds; goto
_LL1461;} else{ goto _LL1462;} _LL1462: goto _LL1463; _LL1461: if(( void*)
_temp1466->v ==( void*) Cyc_Absyn_No_constr){( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto _LL1459;}{ void* _temp1476=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1466); struct Cyc_Absyn_Exp*
_temp1482; _LL1478: if(( int) _temp1476 == Cyc_Absyn_Unknown_b){ goto _LL1479;}
else{ goto _LL1480;} _LL1480: if(( unsigned int) _temp1476 > 1u?(( struct
_tunion_struct*) _temp1476)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1483:
_temp1482=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1476)->f1;
goto _LL1481;} else{ goto _LL1477;} _LL1479: { struct _tuple4* zero=({ struct
_tuple4* _temp1487=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1487->f1= 0; _temp1487->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1487;});
struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*
_temp1484=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1484->hd=( void*) zero; _temp1484->tl=({ struct Cyc_List_List* _temp1485=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1485->hd=(
void*) zero; _temp1485->tl=({ struct Cyc_List_List* _temp1486=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1486->hd=( void*) zero;
_temp1486->tl= 0; _temp1486;}); _temp1485;}); _temp1484;}), 0); if( nv->toplevel){(
void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct( Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1474), Cyc_Toc_mt_tq)); struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*) um, Cyc_Absyn_exp_stmt(
xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}
goto _LL1477;} _LL1481:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1477; _LL1477:;} goto _LL1459; _LL1463:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1488= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1488.last_plus_one -
_temp1488.curr, _temp1488.curr);})); goto _LL1459; _LL1459:;} goto _LL1233;
_LL1237: goto _LL1233; _LL1239:{ struct _handler_cons _temp1489; _push_handler(&
_temp1489);{ struct _xtunion_struct* _temp1490=( struct _xtunion_struct*) setjmp(
_temp1489.handler); if( ! _temp1490){( void*)( e->r=( void*)(( void*)((( struct
Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
nv->varmap, _temp1314))->r));; _pop_handler();} else{ struct _xtunion_struct*
_temp1492= _temp1490; _LL1494: if( _temp1492->tag == Cyc_Dict_Absent_tag){ goto
_LL1495;} else{ goto _LL1496;} _LL1496: goto _LL1497; _LL1495:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1498= Cyc_Absynpp_qvar2string(
_temp1314); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1498.last_plus_one
- _temp1498.curr, _temp1498.curr);})); return; _LL1497:( void) _throw( _temp1492);
_LL1493:;}}} goto _LL1233; _LL1241:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1499=( char*)"unknownid"; struct
_tagged_string _temp1500; _temp1500.curr= _temp1499; _temp1500.base= _temp1499;
_temp1500.last_plus_one= _temp1499 + 10; _temp1500;})); goto _LL1233; _LL1243: {
struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1318); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1318);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1318);{ void* _temp1501= _temp1320; _LL1503: if(( int) _temp1501 == Cyc_Absyn_Size){
goto _LL1504;} else{ goto _LL1505;} _LL1505: if(( int) _temp1501 == Cyc_Absyn_Printf){
goto _LL1506;} else{ goto _LL1507;} _LL1507: if(( int) _temp1501 == Cyc_Absyn_Xprintf){
goto _LL1508;} else{ goto _LL1509;} _LL1509: if(( int) _temp1501 == Cyc_Absyn_Fprintf){
goto _LL1510;} else{ goto _LL1511;} _LL1511: if(( int) _temp1501 == Cyc_Absyn_Sscanf){
goto _LL1512;} else{ goto _LL1513;} _LL1513: if(( int) _temp1501 == Cyc_Absyn_Plus){
goto _LL1514;} else{ goto _LL1515;} _LL1515: if(( int) _temp1501 == Cyc_Absyn_Minus){
goto _LL1516;} else{ goto _LL1517;} _LL1517: if(( int) _temp1501 == Cyc_Absyn_Eq){
goto _LL1518;} else{ goto _LL1519;} _LL1519: if(( int) _temp1501 == Cyc_Absyn_Neq){
goto _LL1520;} else{ goto _LL1521;} _LL1521: if(( int) _temp1501 == Cyc_Absyn_Gt){
goto _LL1522;} else{ goto _LL1523;} _LL1523: if(( int) _temp1501 == Cyc_Absyn_Gte){
goto _LL1524;} else{ goto _LL1525;} _LL1525: if(( int) _temp1501 == Cyc_Absyn_Lt){
goto _LL1526;} else{ goto _LL1527;} _LL1527: if(( int) _temp1501 == Cyc_Absyn_Lte){
goto _LL1528;} else{ goto _LL1529;} _LL1529: goto _LL1530; _LL1504: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1564= _temp1318; if(
_temp1564 == 0){ _throw( Null_Exception);} _temp1564->hd;});{ void* _temp1532=(
void*)({ struct Cyc_Core_Opt* _temp1531= arg->topt; if( _temp1531 == 0){ _throw(
Null_Exception);} _temp1531->v;}); struct Cyc_Absyn_Exp* _temp1540; struct Cyc_Absyn_Tqual
_temp1542; void* _temp1544; struct Cyc_Absyn_PtrInfo _temp1546; struct Cyc_Absyn_Conref*
_temp1548; struct Cyc_Absyn_Tqual _temp1550; struct Cyc_Absyn_Conref* _temp1552;
void* _temp1554; void* _temp1556; _LL1534: if(( unsigned int) _temp1532 > 4u?((
struct _tunion_struct*) _temp1532)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1545:
_temp1544=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1532)->f1; goto
_LL1543; _LL1543: _temp1542=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1532)->f2; goto _LL1541; _LL1541: _temp1540=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1532)->f3; goto _LL1535;} else{ goto
_LL1536;} _LL1536: if(( unsigned int) _temp1532 > 4u?(( struct _tunion_struct*)
_temp1532)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1547: _temp1546=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1532)->f1;
_LL1557: _temp1556=( void*) _temp1546.elt_typ; goto _LL1555; _LL1555: _temp1554=(
void*) _temp1546.rgn_typ; goto _LL1553; _LL1553: _temp1552=( struct Cyc_Absyn_Conref*)
_temp1546.nullable; goto _LL1551; _LL1551: _temp1550=( struct Cyc_Absyn_Tqual)
_temp1546.tq; goto _LL1549; _LL1549: _temp1548=( struct Cyc_Absyn_Conref*)
_temp1546.bounds; goto _LL1537;} else{ goto _LL1538;} _LL1538: goto _LL1539;
_LL1535:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Exp* _temp1558=
_temp1540; if( _temp1558 == 0){ _throw( Null_Exception);} _temp1558->r;})));
goto _LL1533; _LL1537: { struct _tuple0* temp= Cyc_Toc_temp_var(); void*
temp_typ= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1559= arg->topt;
if( _temp1559 == 0){ _throw( Null_Exception);} _temp1559->v;})); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
Cyc_Absyn_subtract_exp( e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt(
e4, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1533;}
_LL1539:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1562= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_List_List*
_temp1560= typs; if( _temp1560 == 0){ _throw( Null_Exception);} _temp1560->hd;}));
struct _tagged_string _temp1563= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1561= arg->topt; if( _temp1561 == 0){ _throw( Null_Exception);} _temp1561->v;}));
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1562.last_plus_one
- _temp1562.curr, _temp1562.curr, _temp1563.last_plus_one - _temp1563.curr,
_temp1563.curr);})); return; _LL1533:;} goto _LL1502;} _LL1506:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1320, 0, _temp1318, typs))->r)); goto
_LL1502; _LL1508:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1320, 0, _temp1318, typs))->r)); goto _LL1502; _LL1510: if( _temp1318 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1565=( char*)"fprintf without arguments"; struct _tagged_string
_temp1566; _temp1566.curr= _temp1565; _temp1566.base= _temp1565; _temp1566.last_plus_one=
_temp1565 + 26; _temp1566;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1320,({ struct Cyc_Core_Opt* _temp1567=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1567->v=( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1568= _temp1318; if( _temp1568 == 0){ _throw(
Null_Exception);} _temp1568->hd;})); _temp1567;}),({ struct Cyc_List_List*
_temp1569= _temp1318; if( _temp1569 == 0){ _throw( Null_Exception);} _temp1569->tl;}),({
struct Cyc_List_List* _temp1570= typs; if( _temp1570 == 0){ _throw(
Null_Exception);} _temp1570->tl;})))->r)); goto _LL1502; _LL1512: if( _temp1318
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1571=( char*)"sscanf without arguments"; struct
_tagged_string _temp1572; _temp1572.curr= _temp1571; _temp1572.base= _temp1571;
_temp1572.last_plus_one= _temp1571 + 25; _temp1572;}));}( void*)((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1573= _temp1318; if( _temp1573 == 0){ _throw(
Null_Exception);} _temp1573->hd;}))->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1574= _temp1318; if( _temp1574 == 0){ _throw( Null_Exception);} _temp1574->hd;}))->r,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1575= _temp1318; if(
_temp1575 == 0){ _throw( Null_Exception);} _temp1575->hd;}))->loc), Cyc_Toc_curr_sp,
0))->r)); goto _LL1502; _LL1514: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1579= _temp1318; if( _temp1579 == 0){ _throw( Null_Exception);} _temp1579->hd;});
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1578=({ struct Cyc_List_List* _temp1577= _temp1318; if( _temp1577 == 0){
_throw( Null_Exception);} _temp1577->tl;}); if( _temp1578 == 0){ _throw(
Null_Exception);} _temp1578->hd;}); struct _tuple0* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1576=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1576->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1576;}),
e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1502; _LL1516: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)({ struct Cyc_List_List* _temp1580= old_typs; if( _temp1580 == 0){ _throw(
Null_Exception);} _temp1580->hd;}))){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1587= _temp1318; if( _temp1587 == 0){ _throw(
Null_Exception);} _temp1587->hd;}); struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1586=({ struct Cyc_List_List* _temp1585= _temp1318;
if( _temp1585 == 0){ _throw( Null_Exception);} _temp1585->tl;}); if( _temp1586
== 0){ _throw( Null_Exception);} _temp1586->hd;}); void* old_typ_e2=( void*)({
struct Cyc_List_List* _temp1584=({ struct Cyc_List_List* _temp1583= old_typs;
if( _temp1583 == 0){ _throw( Null_Exception);} _temp1583->tl;}); if( _temp1584
== 0){ _throw( Null_Exception);} _temp1584->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_List_List* _temp1582= old_typs; if( _temp1582 == 0){ _throw(
Null_Exception);} _temp1582->hd;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1581=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1581->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp1581;}), e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0);
s= Cyc_Absyn_declare_stmt( temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1502;
_LL1518: goto _LL1520; _LL1520: goto _LL1522; _LL1522: goto _LL1524; _LL1524:
goto _LL1526; _LL1526: goto _LL1528; _LL1528: { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1593= _temp1318; if(
_temp1593 == 0){ _throw( Null_Exception);} _temp1593->hd;}); struct Cyc_Absyn_Exp*
e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1592=({ struct Cyc_List_List*
_temp1591= _temp1318; if( _temp1591 == 0){ _throw( Null_Exception);} _temp1591->tl;});
if( _temp1592 == 0){ _throw( Null_Exception);} _temp1592->hd;}); void* t1=( void*)({
struct Cyc_List_List* _temp1590= old_typs; if( _temp1590 == 0){ _throw(
Null_Exception);} _temp1590->hd;}); void* t2=( void*)({ struct Cyc_List_List*
_temp1589=({ struct Cyc_List_List* _temp1588= old_typs; if( _temp1588 == 0){
_throw( Null_Exception);} _temp1588->tl;}); if( _temp1589 == 0){ _throw(
Null_Exception);} _temp1589->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1502;} _LL1530: goto
_LL1502; _LL1502:;} goto _LL1233;} _LL1245: { void* e2_cyc_typ=( void*)({ struct
Cyc_Core_Opt* _temp1594= _temp1324->topt; if( _temp1594 == 0){ _throw(
Null_Exception);} _temp1594->v;}); Cyc_Toc_exp_to_c( nv, _temp1324); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){ if( _temp1322 ==( void*) Cyc_Absyn_PreInc? 1: _temp1322 ==( void*)
Cyc_Absyn_PreDec){ struct _tuple0* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Absyn_star_typ(
Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
temp, 0), 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0), _temp1322, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp, temp_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1324, 0), s, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));} else{ struct _tuple0* temp=
Cyc_Toc_temp_var(); struct _tuple0* temp2= Cyc_Toc_temp_var(); void* temp2_typ=
Cyc_Toc_typ_to_c( e2_cyc_typ); void* temp_typ= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c(
e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp2, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( temp,
0), Cyc_Toc_curr_sp, 0), _temp1322, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp2, temp2_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( temp, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1324, 0), s,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} else{
if( ! Cyc_Absyn_is_lvalue( _temp1324)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1324, 0, Cyc_Toc_incr_lvalue,
_temp1322);( void*)( e->r=( void*)(( void*) _temp1324->r));}} goto _LL1233;}
_LL1247: { int e1_poly= Cyc_Toc_is_poly_project( _temp1330); void* e1_old_typ=(
void*)({ struct Cyc_Core_Opt* _temp1600= _temp1330->topt; if( _temp1600 == 0){
_throw( Null_Exception);} _temp1600->v;}); void* e2_old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1599= _temp1326->topt; if( _temp1599 == 0){ _throw(
Null_Exception);} _temp1599->v;}); Cyc_Toc_exp_to_c( nv, _temp1330); Cyc_Toc_exp_to_c(
nv, _temp1326); if( _temp1328 != 0?( void*)({ struct Cyc_Core_Opt* _temp1595=
_temp1328; if( _temp1595 == 0){ _throw( Null_Exception);} _temp1595->v;}) ==(
void*) Cyc_Absyn_Plus? 1:( void*)({ struct Cyc_Core_Opt* _temp1596= _temp1328;
if( _temp1596 == 0){ _throw( Null_Exception);} _temp1596->v;}) ==( void*) Cyc_Absyn_Minus:
0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){( void*)( _temp1330->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)
_temp1330->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1326->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1326->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1326->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1326->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1330)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1330, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1597=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1597[ 0]=({ struct _tuple11 _temp1598; _temp1598.f1=
_temp1328; _temp1598.f2= _temp1326; _temp1598;}); _temp1597;}));( void*)( e->r=(
void*)(( void*) _temp1330->r));} goto _LL1233;} _LL1249: Cyc_Toc_exp_to_c( nv,
_temp1336); Cyc_Toc_exp_to_c( nv, _temp1334); Cyc_Toc_exp_to_c( nv, _temp1332);
goto _LL1233; _LL1251: Cyc_Toc_exp_to_c( nv, _temp1340); Cyc_Toc_exp_to_c( nv,
_temp1338); goto _LL1233; _LL1253: _temp1348= _temp1344; _temp1346= _temp1342;
goto _LL1255; _LL1255: Cyc_Toc_exp_to_c( nv, _temp1348);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1346); goto _LL1233; _LL1257:
Cyc_Toc_exp_to_c( nv, _temp1350);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1350), 0))->r)); goto
_LL1233; _LL1259: Cyc_Toc_exp_to_c( nv, _temp1352); goto _LL1233; _LL1261: Cyc_Toc_exp_to_c(
nv, _temp1356);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1603= e->topt; if( _temp1603 == 0){ _throw( Null_Exception);} _temp1603->v;}));
for( 0; _temp1354 != 0; _temp1354=({ struct Cyc_List_List* _temp1601= _temp1354;
if( _temp1601 == 0){ _throw( Null_Exception);} _temp1601->tl;})){ void* k= Cyc_Tcutil_typ_kind((
void*)({ struct Cyc_List_List* _temp1602= _temp1354; if( _temp1602 == 0){ _throw(
Null_Exception);} _temp1602->hd;})); if( k !=( void*) Cyc_Absyn_EffKind? k !=(
void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1356, 0))->r)); break;}} goto _LL1233;} _LL1263: { void* old_t2=(
void*)({ struct Cyc_Core_Opt* _temp1604=(* _temp1360)->topt; if( _temp1604 == 0){
_throw( Null_Exception);} _temp1604->v;}); void* new_typ=* _temp1363;* _temp1363=
Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,* _temp1360);* _temp1360= Cyc_Toc_convert_exp(
nv->toplevel, old_t2, new_typ,* _temp1360); goto _LL1233;} _LL1265:{ void*
_temp1605=( void*) _temp1364->r; struct Cyc_Absyn_Structdecl* _temp1613; struct
Cyc_List_List* _temp1615; struct Cyc_Core_Opt* _temp1617; struct _tuple0*
_temp1619; struct Cyc_List_List* _temp1621; _LL1607: if((( struct _tunion_struct*)
_temp1605)->tag == Cyc_Absyn_Struct_e_tag){ _LL1620: _temp1619=( struct _tuple0*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1605)->f1; goto _LL1618; _LL1618:
_temp1617=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1605)->f2;
goto _LL1616; _LL1616: _temp1615=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1605)->f3; goto _LL1614; _LL1614: _temp1613=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1605)->f4; goto _LL1608;} else{ goto
_LL1609;} _LL1609: if((( struct _tunion_struct*) _temp1605)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1622: _temp1621=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1605)->f1; goto _LL1610;} else{ goto _LL1611;} _LL1611: goto _LL1612;
_LL1608: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1623= Cyc_Position_string_of_segment( _temp1364->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1623.last_plus_one -
_temp1623.curr, _temp1623.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1624= _temp1364->topt; if( _temp1624 ==
0){ _throw( Null_Exception);} _temp1624->v;}), 1, 0, _temp1615, _temp1619))->r));
goto _LL1606; _LL1610: if( nv->toplevel){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ struct _tagged_string _temp1625= Cyc_Position_string_of_segment(
_temp1364->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1625.last_plus_one - _temp1625.curr, _temp1625.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1621))->r)); goto _LL1606;
_LL1612: Cyc_Toc_exp_to_c( nv, _temp1364); if( ! Cyc_Absyn_is_lvalue( _temp1364)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1364,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*) _temp1364->r));}
goto _LL1606; _LL1606:;} goto _LL1233; _LL1267: if( nv->toplevel){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1626= Cyc_Position_string_of_segment(
_temp1366->loc); xprintf("%.*s: new at top-level", _temp1626.last_plus_one -
_temp1626.curr, _temp1626.curr);}));}{ void* _temp1627=( void*) _temp1366->r;
struct Cyc_List_List* _temp1641; void* _temp1643; struct _tagged_string
_temp1645; struct Cyc_Absyn_Exp* _temp1647; struct Cyc_Absyn_Exp* _temp1649;
struct Cyc_Absyn_Vardecl* _temp1651; struct Cyc_Absyn_Structdecl* _temp1653;
struct Cyc_List_List* _temp1655; struct Cyc_Core_Opt* _temp1657; struct _tuple0*
_temp1659; struct Cyc_List_List* _temp1661; _LL1629: if((( struct _tunion_struct*)
_temp1627)->tag == Cyc_Absyn_Array_e_tag){ _LL1642: _temp1641=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1627)->f1; goto _LL1630;} else{ goto
_LL1631;} _LL1631: if((( struct _tunion_struct*) _temp1627)->tag == Cyc_Absyn_Const_e_tag){
_LL1644: _temp1643=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1627)->f1;
if(( unsigned int) _temp1643 > 1u?(( struct _tunion_struct*) _temp1643)->tag ==
Cyc_Absyn_String_c_tag: 0){ _LL1646: _temp1645=( struct _tagged_string)(( struct
Cyc_Absyn_String_c_struct*) _temp1643)->f1; goto _LL1632;} else{ goto _LL1633;}}
else{ goto _LL1633;} _LL1633: if((( struct _tunion_struct*) _temp1627)->tag ==
Cyc_Absyn_Comprehension_e_tag){ _LL1652: _temp1651=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1627)->f1; goto _LL1650; _LL1650:
_temp1649=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1627)->f2; goto _LL1648; _LL1648: _temp1647=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1627)->f3; goto _LL1634;} else{
goto _LL1635;} _LL1635: if((( struct _tunion_struct*) _temp1627)->tag == Cyc_Absyn_Struct_e_tag){
_LL1660: _temp1659=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1627)->f1; goto _LL1658; _LL1658: _temp1657=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1627)->f2; goto _LL1656; _LL1656:
_temp1655=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1627)->f3; goto _LL1654; _LL1654: _temp1653=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1627)->f4; goto _LL1636;} else{ goto
_LL1637;} _LL1637: if((( struct _tunion_struct*) _temp1627)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1662: _temp1661=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1627)->f1; goto _LL1638;} else{ goto _LL1639;} _LL1639: goto _LL1640;
_LL1630: { struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp=
Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_array( nv, xexp,
_temp1641, Cyc_Absyn_exp_stmt( xexp, 0)); void* old_elt_typ;{ void* _temp1663=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1669; struct Cyc_Absyn_Conref*
_temp1671; struct Cyc_Absyn_Tqual _temp1673; struct Cyc_Absyn_Conref* _temp1675;
void* _temp1677; void* _temp1679; _LL1665: if(( unsigned int) _temp1663 > 4u?((
struct _tunion_struct*) _temp1663)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1670: _temp1669=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1663)->f1; _LL1680: _temp1679=( void*) _temp1669.elt_typ; goto _LL1678;
_LL1678: _temp1677=( void*) _temp1669.rgn_typ; goto _LL1676; _LL1676: _temp1675=(
struct Cyc_Absyn_Conref*) _temp1669.nullable; goto _LL1674; _LL1674: _temp1673=(
struct Cyc_Absyn_Tqual) _temp1669.tq; goto _LL1672; _LL1672: _temp1671=( struct
Cyc_Absyn_Conref*) _temp1669.bounds; goto _LL1666;} else{ goto _LL1667;} _LL1667:
goto _LL1668; _LL1666: old_elt_typ= _temp1679; goto _LL1664; _LL1668:
old_elt_typ=(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1681=( char*)"exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1682; _temp1682.curr= _temp1681; _temp1682.base=
_temp1681; _temp1682.last_plus_one= _temp1681 + 52; _temp1682;})); goto _LL1664;
_LL1664:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1641), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1368 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, e2);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1683= _temp1368; if( _temp1683 == 0){ _throw(
Null_Exception);} _temp1683;}); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp(
r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,(
struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1628;}}} _LL1632: if(
_temp1368 == 0){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1684=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1684->f1= Cyc_Toc_abs_ns; _temp1684->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1684;}), 0),({ struct Cyc_List_List* _temp1685=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1685->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1686=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1686[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1687; _temp1687.tag= Cyc_Absyn_Const_e_tag;
_temp1687.f1=( void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1688=(
struct Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp1688[ 0]=({ struct Cyc_Absyn_String_c_struct _temp1689; _temp1689.tag= Cyc_Absyn_String_c_tag;
_temp1689.f1= _temp1645; _temp1689;}); _temp1688;})); _temp1687;}); _temp1686;}),
0); _temp1685->tl= 0; _temp1685;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1696= _temp1368; if(
_temp1696 == 0){ _throw( Null_Exception);} _temp1696;}); Cyc_Toc_exp_to_c( nv, r);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1690=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1690->f1= Cyc_Toc_abs_ns; _temp1690->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1690;}), 0),({ struct Cyc_List_List* _temp1691=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1691->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1692=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1692[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1693; _temp1693.tag= Cyc_Absyn_Const_e_tag;
_temp1693.f1=( void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1694=(
struct Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp1694[ 0]=({ struct Cyc_Absyn_String_c_struct _temp1695; _temp1695.tag= Cyc_Absyn_String_c_tag;
_temp1695.f1= _temp1645; _temp1695;}); _temp1694;})); _temp1693;}); _temp1692;}),
0); _temp1691->tl= 0; _temp1691;}), 0))->r));} goto _LL1628; _LL1634: { int
is_tagged_ptr= 0;{ void* _temp1697= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1703; struct Cyc_Absyn_Conref* _temp1705; struct Cyc_Absyn_Tqual _temp1707;
struct Cyc_Absyn_Conref* _temp1709; void* _temp1711; void* _temp1713; _LL1699:
if(( unsigned int) _temp1697 > 4u?(( struct _tunion_struct*) _temp1697)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1704: _temp1703=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1697)->f1; _LL1714: _temp1713=( void*)
_temp1703.elt_typ; goto _LL1712; _LL1712: _temp1711=( void*) _temp1703.rgn_typ;
goto _LL1710; _LL1710: _temp1709=( struct Cyc_Absyn_Conref*) _temp1703.nullable;
goto _LL1708; _LL1708: _temp1707=( struct Cyc_Absyn_Tqual) _temp1703.tq; goto
_LL1706; _LL1706: _temp1705=( struct Cyc_Absyn_Conref*) _temp1703.bounds; goto
_LL1700;} else{ goto _LL1701;} _LL1701: goto _LL1702; _LL1700:{ void* _temp1715=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1705))->v; void* _temp1721; _LL1717: if(( unsigned int) _temp1715 > 1u?((
struct _tunion_struct*) _temp1715)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1722:
_temp1721=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1715)->f1; if((
int) _temp1721 == Cyc_Absyn_Unknown_b){ goto _LL1718;} else{ goto _LL1719;}}
else{ goto _LL1719;} _LL1719: goto _LL1720; _LL1718: is_tagged_ptr= 1; goto
_LL1716; _LL1720: goto _LL1716; _LL1716:;} goto _LL1698; _LL1702:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1723=( char*)"exp_to_c: comprehension not an array type"; struct
_tagged_string _temp1724; _temp1724.curr= _temp1723; _temp1724.base= _temp1723;
_temp1724.last_plus_one= _temp1723 + 42; _temp1724;})); goto _LL1698; _LL1698:;}{
struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)({ struct Cyc_Core_Opt* _temp1748= _temp1647->topt;
if( _temp1748 == 0){ _throw( Null_Exception);} _temp1748->v;}); void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1649);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1651, Cyc_Absyn_var_exp( max, 0), _temp1647,
Cyc_Absyn_skip_stmt( 0), 1); struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1746=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1746->hd=( void*)({ struct _tuple10* _temp1747=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1747->f1= max; _temp1747->f2= Cyc_Absyn_uint_t;
_temp1747->f3=( struct Cyc_Absyn_Exp*) _temp1649; _temp1747;}); _temp1746->tl= 0;
_temp1746;}); struct Cyc_Absyn_Exp* ai; if( _temp1368 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp1725= _temp1368; if( _temp1725 == 0){ _throw( Null_Exception);}
_temp1725;}); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct
Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1726=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1726->hd=( void*)({ struct _tuple10* _temp1727=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1727->f1= a; _temp1727->f2= ptr_typ;
_temp1727->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1727;}); _temp1726->tl= decls;
_temp1726;}); if( is_tagged_ptr){ struct _tuple0* b= Cyc_Toc_temp_var(); void* t=
Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* binit= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp1730=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1730->hd=( void*)({ struct _tuple4* _temp1735=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1735->f1= 0;
_temp1735->f2= Cyc_Absyn_var_exp( a, 0); _temp1735;}); _temp1730->tl=({ struct
Cyc_List_List* _temp1731=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1731->hd=( void*)({ struct _tuple4* _temp1734=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1734->f1= 0; _temp1734->f2= Cyc_Absyn_var_exp( a,
0); _temp1734;}); _temp1731->tl=({ struct Cyc_List_List* _temp1732=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1732->hd=( void*)({ struct
_tuple4* _temp1733=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1733->f1= 0; _temp1733->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1733;}); _temp1732->tl= 0; _temp1732;});
_temp1731;}); _temp1730;}), 0); decls=({ struct Cyc_List_List* _temp1728=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1728->hd=(
void*)({ struct _tuple10* _temp1729=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1729->f1= b; _temp1729->f2= t; _temp1729->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1729;}); _temp1728->tl= decls; _temp1728;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp1736= ds; if(
_temp1736 == 0){ _throw( Null_Exception);} _temp1736->tl;})){ struct Cyc_Absyn_Exp*
_temp1740; void* _temp1742; struct _tuple0* _temp1744; struct _tuple10 _temp1738=*((
struct _tuple10*)({ struct Cyc_List_List* _temp1737= ds; if( _temp1737 == 0){
_throw( Null_Exception);} _temp1737->hd;})); _LL1745: _temp1744= _temp1738.f1;
goto _LL1743; _LL1743: _temp1742= _temp1738.f2; goto _LL1741; _LL1741: _temp1740=
_temp1738.f3; goto _LL1739; _LL1739: s= Cyc_Absyn_declare_stmt( _temp1744,
_temp1742, _temp1740, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r)); goto _LL1628;}}}} _LL1636:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1749= _temp1366->topt; if( _temp1749 ==
0){ _throw( Null_Exception);} _temp1749->v;}), 1, _temp1368, _temp1655,
_temp1659))->r)); goto _LL1628; _LL1638:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1368, _temp1661))->r)); goto _LL1628; _LL1640: { void* old_elt_typ=(
void*)({ struct Cyc_Core_Opt* _temp1751= _temp1366->topt; if( _temp1751 == 0){
_throw( Null_Exception);} _temp1751->v;}); struct _tuple0* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0);
struct Cyc_Absyn_Exp* e_index= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c(
nv, _temp1366); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1366, 0), s, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); struct Cyc_Absyn_Exp*
e1; if( _temp1368 == 0){ e1= Cyc_Toc_malloc_exp( old_elt_typ, e2);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1750=
_temp1368; if( _temp1750 == 0){ _throw( Null_Exception);} _temp1750;}); Cyc_Toc_exp_to_c(
nv, r); e1= Cyc_Toc_rmalloc_exp( r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp(
ptr_typ, e1, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, ptr_typ,( struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1628;}}}
_LL1628:;} goto _LL1233; _LL1269:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1752=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1752[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1753; _temp1753.tag=
Cyc_Absyn_Sizeoftyp_e_tag; _temp1753.f1=( void*) Cyc_Toc_typ_to_c_array(
_temp1370); _temp1753;}); _temp1752;}))); goto _LL1233; _LL1271: Cyc_Toc_exp_to_c(
nv, _temp1372); goto _LL1233; _LL1273: { void* e1_typ= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1783= _temp1374->topt; if( _temp1783 == 0){
_throw( Null_Exception);} _temp1783->v;})); Cyc_Toc_exp_to_c( nv, _temp1374);{
void* _temp1754= e1_typ; struct Cyc_Absyn_PtrInfo _temp1760; struct Cyc_Absyn_Conref*
_temp1762; struct Cyc_Absyn_Tqual _temp1764; struct Cyc_Absyn_Conref* _temp1766;
void* _temp1768; void* _temp1770; _LL1756: if(( unsigned int) _temp1754 > 4u?((
struct _tunion_struct*) _temp1754)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1761: _temp1760=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1754)->f1; _LL1771: _temp1770=( void*) _temp1760.elt_typ; goto _LL1769;
_LL1769: _temp1768=( void*) _temp1760.rgn_typ; goto _LL1767; _LL1767: _temp1766=(
struct Cyc_Absyn_Conref*) _temp1760.nullable; goto _LL1765; _LL1765: _temp1764=(
struct Cyc_Absyn_Tqual) _temp1760.tq; goto _LL1763; _LL1763: _temp1762=( struct
Cyc_Absyn_Conref*) _temp1760.bounds; goto _LL1757;} else{ goto _LL1758;} _LL1758:
goto _LL1759; _LL1757:{ void* _temp1772=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1762); struct Cyc_Absyn_Exp* _temp1778; _LL1774: if((
int) _temp1772 == Cyc_Absyn_Unknown_b){ goto _LL1775;} else{ goto _LL1776;}
_LL1776: if(( unsigned int) _temp1772 > 1u?(( struct _tunion_struct*) _temp1772)->tag
== Cyc_Absyn_Upper_b_tag: 0){ _LL1779: _temp1778=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1772)->f1; goto _LL1777;} else{ goto
_LL1773;} _LL1775: { struct _tuple0* x= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array( _temp1770); struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1, _temp1764); void* tx= Cyc_Absyn_strct(
n); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1764); struct Cyc_Absyn_Exp*
test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_or_exp(
Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt(
test_exp, s4, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt(
s3, s5, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt( a, ta2,( struct
Cyc_Absyn_Exp*) a_init, s2, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp1374, s1, 0), 0))->r));
goto _LL1773;} _LL1777: if( Cyc_Toc_is_nullable( e1_typ)){ struct _tuple0* x=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1780= _temp1374->topt; if( _temp1780 == 0){ _throw( Null_Exception);}
_temp1780->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1374, s3, 0), 0))->r));} goto _LL1773; _LL1773:;} goto _LL1755; _LL1759:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1781=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1782; _temp1782.curr= _temp1781; _temp1782.base= _temp1781; _temp1782.last_plus_one=
_temp1781 + 29; _temp1782;})); goto _LL1755; _LL1755:;} goto _LL1233;} _LL1275:
Cyc_Toc_exp_to_c( nv, _temp1378); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({ struct
Cyc_Core_Opt* _temp1784= e->topt; if( _temp1784 == 0){ _throw( Null_Exception);}
_temp1784->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1233;
_LL1277: Cyc_Toc_exp_to_c( nv, _temp1382);{ int is_poly= Cyc_Toc_is_poly_project(
e); if( Cyc_Toc_is_nullable(( void*)({ struct Cyc_Core_Opt* _temp1785= _temp1382->topt;
if( _temp1785 == 0){ _throw( Null_Exception);} _temp1785->v;}))){ struct _tuple0*
x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1786= _temp1382->topt; if( _temp1786 == 0){ _throw( Null_Exception);}
_temp1786->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0),
_temp1380, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1382, s3, 0), 0))->r));} if( is_poly){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1787= e->topt; if( _temp1787 == 0){ _throw(
Null_Exception);} _temp1787->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}
goto _LL1233;} _LL1279: { void* e1_typ= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp1840= _temp1386->topt; if( _temp1840 == 0){ _throw(
Null_Exception);} _temp1840->v;})); Cyc_Toc_exp_to_c( nv, _temp1386); Cyc_Toc_exp_to_c(
nv, _temp1384);( void*)( _temp1386->r=( void*)(( void*)( Cyc_Toc_array_to_ptr(
Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp(( void*) _temp1386->r, 0), 0))->r));{
void* _temp1788= e1_typ; struct Cyc_List_List* _temp1798; struct Cyc_Absyn_Exp*
_temp1800; struct Cyc_Absyn_Tqual _temp1802; void* _temp1804; struct Cyc_Absyn_PtrInfo
_temp1806; struct Cyc_Absyn_Conref* _temp1808; struct Cyc_Absyn_Tqual _temp1810;
struct Cyc_Absyn_Conref* _temp1812; void* _temp1814; void* _temp1816; _LL1790:
if(( unsigned int) _temp1788 > 4u?(( struct _tunion_struct*) _temp1788)->tag ==
Cyc_Absyn_TupleType_tag: 0){ _LL1799: _temp1798=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1788)->f1; goto _LL1791;} else{ goto
_LL1792;} _LL1792: if(( unsigned int) _temp1788 > 4u?(( struct _tunion_struct*)
_temp1788)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1805: _temp1804=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1788)->f1; goto _LL1803; _LL1803:
_temp1802=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1788)->f2; goto _LL1801; _LL1801: _temp1800=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1788)->f3; goto _LL1793;} else{ goto
_LL1794;} _LL1794: if(( unsigned int) _temp1788 > 4u?(( struct _tunion_struct*)
_temp1788)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1807: _temp1806=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1788)->f1;
_LL1817: _temp1816=( void*) _temp1806.elt_typ; goto _LL1815; _LL1815: _temp1814=(
void*) _temp1806.rgn_typ; goto _LL1813; _LL1813: _temp1812=( struct Cyc_Absyn_Conref*)
_temp1806.nullable; goto _LL1811; _LL1811: _temp1810=( struct Cyc_Absyn_Tqual)
_temp1806.tq; goto _LL1809; _LL1809: _temp1808=( struct Cyc_Absyn_Conref*)
_temp1806.bounds; goto _LL1795;} else{ goto _LL1796;} _LL1796: goto _LL1797;
_LL1791: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1384) + 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp1386,({ struct
_tagged_string* _temp1818=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp1818[ 0]= xprintf("f%d",( int) i); _temp1818;}), 0))->r));
goto _LL1789;} _LL1793: if( _temp1800 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1819=( char*)"exp_to_c: Subscript_e array type bound null";
struct _tagged_string _temp1820; _temp1820.curr= _temp1819; _temp1820.base=
_temp1819; _temp1820.last_plus_one= _temp1819 + 44; _temp1820;}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_known_size_subscript( _temp1386, _temp1384, 0,( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1821= _temp1800; if( _temp1821 == 0){
_throw( Null_Exception);} _temp1821;}), _temp1804, _temp1802, 0))->r)); goto
_LL1789; _LL1795:{ void* _temp1822=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1808); struct Cyc_Absyn_Exp* _temp1828; _LL1824: if(( unsigned int)
_temp1822 > 1u?(( struct _tunion_struct*) _temp1822)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1829: _temp1828=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1822)->f1; goto _LL1825;} else{ goto _LL1826;} _LL1826: if(( int) _temp1822
== Cyc_Absyn_Unknown_b){ goto _LL1827;} else{ goto _LL1823;} _LL1825: { int
possibly_null= 1;{ void* _temp1830=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1812))->v; int
_temp1836; _LL1832: if(( unsigned int) _temp1830 > 1u?(( struct _tunion_struct*)
_temp1830)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1837: _temp1836=( int)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1830)->f1; goto _LL1833;} else{ goto
_LL1834;} _LL1834: goto _LL1835; _LL1833: possibly_null= _temp1836; goto _LL1831;
_LL1835: possibly_null= 0; goto _LL1831; _LL1831:;}( void*)( e->r=( void*)((
void*)( Cyc_Toc_known_size_subscript( _temp1386, _temp1384, possibly_null,
_temp1828, _temp1816, _temp1810, 0))->r)); goto _LL1823;} _LL1827: { struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct
_tuple0* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array( _temp1816);
struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1, _temp1810); void* tx=
Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1810); struct Cyc_Absyn_Exp*
test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_or_exp(
Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_curr_sp, 0), _temp1384, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4=
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1386, s1, 0), 0))->r)); goto _LL1823;} _LL1823:;}
goto _LL1789; _LL1797:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1838=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1839; _temp1839.curr= _temp1838; _temp1839.base=
_temp1838; _temp1839.last_plus_one= _temp1838 + 49; _temp1839;})); goto _LL1789;
_LL1789:;} goto _LL1233;} _LL1281: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1388))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1388); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp1388 != 0; _temp1388=({ struct Cyc_List_List* _temp1841=
_temp1388; if( _temp1841 == 0){ _throw( Null_Exception);} _temp1841->tl;}), i ++){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1842=
_temp1388; if( _temp1842 == 0){ _throw( Null_Exception);} _temp1842->hd;}));{
struct Cyc_List_List* des=({ struct Cyc_List_List* _temp1846=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1846->hd=( void*)(( void*)({
struct Cyc_Absyn_FieldName_struct* _temp1847=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp1847[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp1848; _temp1848.tag= Cyc_Absyn_FieldName_tag;
_temp1848.f1=({ struct _tagged_string* _temp1849=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1849[ 0]= xprintf("f%d", i);
_temp1849;}); _temp1848;}); _temp1847;})); _temp1846->tl= 0; _temp1846;}); dles=({
struct Cyc_List_List* _temp1843=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1843->hd=( void*)({ struct _tuple4* _temp1844=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1844->f1=( struct Cyc_List_List*)
des; _temp1844->f2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1845=
_temp1388; if( _temp1845 == 0){ _throw( Null_Exception);} _temp1845->hd;});
_temp1844;}); _temp1843->tl= dles; _temp1843;});}}} dles=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp( 0,
dles, 0), 0))->r));} goto _LL1233; _LL1283:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1850=( char*)"compoundlit";
struct _tagged_string _temp1851; _temp1851.curr= _temp1850; _temp1851.base=
_temp1850; _temp1851.last_plus_one= _temp1850 + 12; _temp1851;})); goto _LL1233;
_LL1285:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1394, 0))->r));{ struct Cyc_List_List* dles= _temp1394; for( 0; dles != 0;
dles=({ struct Cyc_List_List* _temp1852= dles; if( _temp1852 == 0){ _throw(
Null_Exception);} _temp1852->tl;})){ struct _tuple4 _temp1856; struct Cyc_Absyn_Exp*
_temp1857; struct Cyc_List_List* _temp1859; struct _tuple4* _temp1854=( struct
_tuple4*)({ struct Cyc_List_List* _temp1853= dles; if( _temp1853 == 0){ _throw(
Null_Exception);} _temp1853->hd;}); _temp1856=* _temp1854; _LL1860: _temp1859=
_temp1856.f1; goto _LL1858; _LL1858: _temp1857= _temp1856.f2; goto _LL1855;
_LL1855: Cyc_Toc_exp_to_c( nv, _temp1857);}} goto _LL1233; _LL1287: {
unsigned int sz= Cyc_Evexp_eval_const_uint_exp( _temp1398); Cyc_Toc_exp_to_c( nv,
_temp1396);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp1396)){
unsigned int i= 0; for( 0; i < sz; i ++){ es=({ struct Cyc_List_List* _temp1861=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1861->hd=(
void*)({ struct _tuple4* _temp1862=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1862->f1= 0; _temp1862->f2= _temp1396; _temp1862;}); _temp1861->tl=
es; _temp1861;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL1233;}} _LL1289: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1404, _temp1408))->r));}
else{ struct _tuple0* n= _temp1408; struct Cyc_Absyn_Structdecl* sd2=( struct
Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl* _temp1876= _temp1402 == 0?((
struct Cyc_Absyn_Structdecl*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1875= Cyc_Absynpp_qvar2string( _temp1408); xprintf("no structdecl %.*s",
_temp1875.last_plus_one - _temp1875.curr, _temp1875.curr);})): _temp1402; if(
_temp1876 == 0){ _throw( Null_Exception);} _temp1876;}); struct _RegionHandle
_temp1863= _new_region(); struct _RegionHandle* rgn=& _temp1863; struct
_handler_cons _temp1864; _push_handler(& _temp1864);{ struct _xtunion_struct*
_temp1865=( struct _xtunion_struct*) setjmp( _temp1864.handler); if( ! _temp1865){{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct _RegionHandle*
rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd)) Cyc_Tcutil_resolve_struct_designators)( rgn, e->loc, _temp1404,( struct Cyc_Absyn_Structdecl*)
sd2); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp1866= fields;
if( _temp1866 == 0){ _throw( Null_Exception);} _temp1866->tl;})){ struct
_tuple12 _temp1870; struct Cyc_Absyn_Exp* _temp1871; struct Cyc_Absyn_Structfield*
_temp1873; struct _tuple12* _temp1868=( struct _tuple12*)({ struct Cyc_List_List*
_temp1867= fields; if( _temp1867 == 0){ _throw( Null_Exception);} _temp1867->hd;});
_temp1870=* _temp1868; _LL1874: _temp1873= _temp1870.f1; goto _LL1872; _LL1872:
_temp1871= _temp1870.f2; goto _LL1869; _LL1869: { void* fieldtyp=( void*)
_temp1873->type; Cyc_Toc_exp_to_c( nv, _temp1871); if( Cyc_Toc_is_void_star(
fieldtyp)){( void*)( _temp1871->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1871->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0,
_temp1404, 0), 0))->r));} _free_region(& _temp1863); _pop_handler();} else{
_free_region(& _temp1863); _throw( _temp1865);}}} goto _LL1233; _LL1291: {
struct _tuple0* qv= _temp1410->name; if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int tag_count= 0; struct Cyc_List_List*
fields= _temp1412->fields == 0? 0:( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1880= _temp1412->fields; if( _temp1880 == 0){ _throw( Null_Exception);}
_temp1880->v;}); while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1877= fields; if( _temp1877 == 0){ _throw(
Null_Exception);} _temp1877->hd;}))->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1878= fields; if( _temp1878 == 0){ _throw(
Null_Exception);} _temp1878->hd;}))->typs == 0){ tag_count ++;} fields=({ struct
Cyc_List_List* _temp1879= fields; if( _temp1879 == 0){ _throw( Null_Exception);}
_temp1879->tl;});}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0))->r));} goto _LL1233;} _LL1293: { struct _tuple0* qv=
_temp1420->name; struct Cyc_List_List* tqts= _temp1420->typs; struct _tuple0* x=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); void*
strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1910=( char*)"_struct"; struct _tagged_string
_temp1911; _temp1911.curr= _temp1910; _temp1911.base= _temp1910; _temp1911.last_plus_one=
_temp1910 + 8; _temp1911;}))); if( nv->toplevel){ int tag_count= 0; struct Cyc_List_List*
fields= _temp1422->fields == 0? 0:( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1899= _temp1422->fields; if( _temp1899 == 0){ _throw( Null_Exception);}
_temp1899->v;}); while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1881= fields; if( _temp1881 == 0){ _throw(
Null_Exception);} _temp1881->hd;}))->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1882= fields; if( _temp1882 == 0){ _throw(
Null_Exception);} _temp1882->hd;}))->typs != 0){ tag_count ++;} fields=({ struct
Cyc_List_List* _temp1883= fields; if( _temp1883 == 0){ _throw( Null_Exception);}
_temp1883->tl;});}{ struct Cyc_List_List* dles= 0; for( 0; _temp1424 != 0;
_temp1424=({ struct Cyc_List_List* _temp1884= _temp1424; if( _temp1884 == 0){
_throw( Null_Exception);} _temp1884->tl;}), tqts=({ struct Cyc_List_List*
_temp1885= tqts; if( _temp1885 == 0){ _throw( Null_Exception);} _temp1885->tl;})){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1891= _temp1424; if( _temp1891 == 0){ _throw( Null_Exception);} _temp1891->hd;});
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp1890= tqts; if( _temp1890 == 0){ _throw( Null_Exception);} _temp1890->hd;}))).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} dles=({ struct Cyc_List_List* _temp1886=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1886[ 0]=({ struct Cyc_List_List
_temp1887; _temp1887.hd=( void*)({ struct _tuple4* _temp1888=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp1888[ 0]=({ struct _tuple4
_temp1889; _temp1889.f1= 0; _temp1889.f2= cur_e; _temp1889;}); _temp1888;});
_temp1887.tl= dles; _temp1887;}); _temp1886;});}{ struct Cyc_Absyn_Exp* tag_exp=
Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0); dles=({ struct Cyc_List_List*
_temp1892=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1892[ 0]=({ struct Cyc_List_List _temp1893; _temp1893.hd=( void*)({ struct
_tuple4* _temp1894=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp1894[ 0]=({ struct _tuple4 _temp1895; _temp1895.f1= 0; _temp1895.f2=
tag_exp; _temp1895;}); _temp1894;}); _temp1893.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); _temp1893;}); _temp1892;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1896=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1896->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({
char* _temp1897=( char*)"_struct"; struct _tagged_string _temp1898; _temp1898.curr=
_temp1897; _temp1898.base= _temp1897; _temp1898.last_plus_one= _temp1897 + 8;
_temp1898;})); _temp1896;}), dles, 0))->r));}}} else{ struct Cyc_List_List*
rev_assign=({ struct Cyc_List_List* _temp1907=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1907->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( xexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1908=( char*)"_tag"; struct
_tagged_string _temp1909; _temp1909.curr= _temp1908; _temp1909.base= _temp1908;
_temp1909.last_plus_one= _temp1908 + 5; _temp1909;})), 0), 0); _temp1907->tl= 0;
_temp1907;});{ int i= 1; for( 0; _temp1424 != 0;( _temp1424=({ struct Cyc_List_List*
_temp1900= _temp1424; if( _temp1900 == 0){ _throw( Null_Exception);} _temp1900->tl;}),
i ++), tqts=({ struct Cyc_List_List* _temp1901= tqts; if( _temp1901 == 0){
_throw( Null_Exception);} _temp1901->tl;})){ struct Cyc_Absyn_Exp* cur_e=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1905= _temp1424; if(
_temp1905 == 0){ _throw( Null_Exception);} _temp1905->hd;}); void* field_typ=
Cyc_Toc_typ_to_c((*(( struct _tuple3*)({ struct Cyc_List_List* _temp1904= tqts;
if( _temp1904 == 0){ _throw( Null_Exception);} _temp1904->hd;}))).f2); Cyc_Toc_exp_to_c(
nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1903=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1903[ 0]= xprintf("f%d", i); _temp1903;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1902=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1902->hd=( void*) a; _temp1902->tl=
rev_assign; _temp1902;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1906=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1906->hd=(
void*) final_stmt; _temp1906->tl= rev_assign; _temp1906;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL1233;} _LL1295: { struct _tuple0* qv= _temp1430->name;
struct Cyc_List_List* tqts= _temp1430->typs; struct Cyc_Absyn_Exp* tag_exp= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({ char* _temp1929=( char*)"_tag";
struct _tagged_string _temp1930; _temp1930.curr= _temp1929; _temp1930.base=
_temp1929; _temp1930.last_plus_one= _temp1929 + 5; _temp1930;})), 0); if( nv->toplevel){
struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Toc_add_designator, nv, _temp1434); dles=({ struct Cyc_List_List*
_temp1912=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1912->hd=( void*)({ struct _tuple4* _temp1913=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1913->f1= 0; _temp1913->f2= tag_exp; _temp1913;});
_temp1912->tl= dles; _temp1912;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1914=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1914->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1915=( char*)"_struct"; struct _tagged_string
_temp1916; _temp1916.curr= _temp1915; _temp1916.base= _temp1915; _temp1916.last_plus_one=
_temp1915 + 8; _temp1916;})); _temp1914;}), dles, 0))->r));} else{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x,
0); void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1927=( char*)"_struct"; struct _tagged_string
_temp1928; _temp1928.curr= _temp1927; _temp1928.base= _temp1927; _temp1928.last_plus_one=
_temp1927 + 8; _temp1928;}))); struct Cyc_List_List* rev_assign=({ struct Cyc_List_List*
_temp1924=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1924->hd=( void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( xexp,
Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1925=( char*)"_tag"; struct _tagged_string
_temp1926; _temp1926.curr= _temp1925; _temp1926.base= _temp1925; _temp1926.last_plus_one=
_temp1925 + 5; _temp1926;})), 0), 0); _temp1924->tl= 0; _temp1924;});{ int i= 1;
for( 0; _temp1434 != 0;( _temp1434=({ struct Cyc_List_List* _temp1917= _temp1434;
if( _temp1917 == 0){ _throw( Null_Exception);} _temp1917->tl;}), i ++), tqts=({
struct Cyc_List_List* _temp1918= tqts; if( _temp1918 == 0){ _throw(
Null_Exception);} _temp1918->tl;})){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1922= _temp1434; if( _temp1922 == 0){ _throw(
Null_Exception);} _temp1922->hd;}); void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)({ struct Cyc_List_List* _temp1921= tqts; if( _temp1921 == 0){
_throw( Null_Exception);} _temp1921->hd;}))).f2); Cyc_Toc_exp_to_c( nv, cur_e);
if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ,
cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1920=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1920[ 0]= xprintf("f%d", i); _temp1920;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1919=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1919->hd=( void*) a; _temp1919->tl=
rev_assign; _temp1919;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1923=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1923->hd=(
void*) final_stmt; _temp1923->tl= rev_assign; _temp1923;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL1233;} _LL1297: goto _LL1233; _LL1299: { void* t_c= Cyc_Toc_typ_to_c(
_temp1444); if( _temp1446 != 0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1931= _temp1446; if( _temp1931 == 0){ _throw(
Null_Exception);} _temp1931;}); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)((
void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1444, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} goto _LL1233;} _LL1301: Cyc_Toc_stmt_to_c( nv, _temp1448); goto
_LL1233; _LL1303:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1932=( char*)"UnresolvedMem"; struct
_tagged_string _temp1933; _temp1933.curr= _temp1932; _temp1933.base= _temp1932;
_temp1933.last_plus_one= _temp1932 + 14; _temp1933;})); goto _LL1233; _LL1305:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1934=( char*)"codegen"; struct _tagged_string _temp1935; _temp1935.curr=
_temp1934; _temp1935.base= _temp1934; _temp1935.last_plus_one= _temp1934 + 8;
_temp1935;})); goto _LL1233; _LL1307:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1936=( char*)"fill"; struct _tagged_string
_temp1937; _temp1937.curr= _temp1936; _temp1937.base= _temp1936; _temp1937.last_plus_one=
_temp1936 + 5; _temp1937;})); goto _LL1233; _LL1233:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple14{ struct _tuple0* f1; void* f2;
} ; struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, void* t,
struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat* p,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1939=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1979; struct Cyc_Absyn_Vardecl _temp1981; struct Cyc_List_List* _temp1982;
struct Cyc_Core_Opt* _temp1984; struct Cyc_Absyn_Exp* _temp1986; void* _temp1988;
struct Cyc_Absyn_Tqual _temp1990; struct _tuple0* _temp1992; void* _temp1994;
struct Cyc_Absyn_Vardecl* _temp1996; int _temp1998; void* _temp2000; char
_temp2002; struct _tagged_string _temp2004; struct Cyc_Absyn_Enumfield*
_temp2006; struct Cyc_Absyn_Enumdecl* _temp2008; struct _tuple0* _temp2010;
struct Cyc_Absyn_Tunionfield* _temp2012; struct Cyc_Absyn_Tuniondecl* _temp2014;
struct Cyc_List_List* _temp2016; struct Cyc_List_List* _temp2018; struct _tuple0*
_temp2020; struct Cyc_Absyn_Pat* _temp2023; struct Cyc_Absyn_Pat _temp2025;
struct Cyc_Position_Segment* _temp2026; struct Cyc_Core_Opt* _temp2028; void*
_temp2030; struct Cyc_Absyn_Tunionfield* _temp2032; struct Cyc_Absyn_Tuniondecl*
_temp2034; struct Cyc_List_List* _temp2036; struct Cyc_List_List* _temp2038;
struct _tuple0* _temp2040; struct Cyc_Absyn_Pat* _temp2042; struct Cyc_Absyn_Pat
_temp2044; struct Cyc_Position_Segment* _temp2045; struct Cyc_Core_Opt*
_temp2047; void* _temp2049; struct Cyc_Absyn_Tunionfield* _temp2051; struct Cyc_Absyn_XTuniondecl*
_temp2053; struct Cyc_List_List* _temp2055; struct Cyc_List_List* _temp2057;
struct _tuple0* _temp2059; struct Cyc_Absyn_Tunionfield* _temp2061; struct Cyc_Absyn_Tuniondecl*
_temp2063; struct Cyc_List_List* _temp2065; struct Cyc_List_List* _temp2067;
struct _tuple0* _temp2069; struct Cyc_Absyn_Tunionfield* _temp2071; struct Cyc_Absyn_XTuniondecl*
_temp2073; struct Cyc_List_List* _temp2075; struct Cyc_List_List* _temp2077;
struct _tuple0* _temp2079; struct Cyc_List_List* _temp2081; struct Cyc_List_List*
_temp2083; struct Cyc_List_List* _temp2085; struct Cyc_Core_Opt* _temp2087;
struct Cyc_Absyn_Structdecl* _temp2089; struct Cyc_Absyn_Pat* _temp2091; struct
_tuple0* _temp2093; struct Cyc_List_List* _temp2095; struct Cyc_List_List*
_temp2097; struct _tuple0* _temp2099; struct Cyc_List_List* _temp2101; struct
Cyc_List_List* _temp2103; struct _tuple0* _temp2105; _LL1941: if(( unsigned int)
_temp1939 > 2u?(( struct _tunion_struct*) _temp1939)->tag == Cyc_Absyn_Var_p_tag:
0){ _LL1980: _temp1979=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1939)->f1; _temp1981=* _temp1979; _LL1995: _temp1994=( void*) _temp1981.sc;
goto _LL1993; _LL1993: _temp1992=( struct _tuple0*) _temp1981.name; goto _LL1991;
_LL1991: _temp1990=( struct Cyc_Absyn_Tqual) _temp1981.tq; goto _LL1989; _LL1989:
_temp1988=( void*) _temp1981.type; goto _LL1987; _LL1987: _temp1986=( struct Cyc_Absyn_Exp*)
_temp1981.initializer; goto _LL1985; _LL1985: _temp1984=( struct Cyc_Core_Opt*)
_temp1981.rgn; goto _LL1983; _LL1983: _temp1982=( struct Cyc_List_List*)
_temp1981.attributes; goto _LL1942;} else{ goto _LL1943;} _LL1943: if(( int)
_temp1939 == Cyc_Absyn_Wild_p){ goto _LL1944;} else{ goto _LL1945;} _LL1945: if((
unsigned int) _temp1939 > 2u?(( struct _tunion_struct*) _temp1939)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1997: _temp1996=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1939)->f1; goto _LL1946;} else{ goto _LL1947;} _LL1947: if(( int) _temp1939
== Cyc_Absyn_Null_p){ goto _LL1948;} else{ goto _LL1949;} _LL1949: if((
unsigned int) _temp1939 > 2u?(( struct _tunion_struct*) _temp1939)->tag == Cyc_Absyn_Int_p_tag:
0){ _LL2001: _temp2000=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1939)->f1;
goto _LL1999; _LL1999: _temp1998=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1939)->f2; goto _LL1950;} else{ goto _LL1951;} _LL1951: if(( unsigned int)
_temp1939 > 2u?(( struct _tunion_struct*) _temp1939)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL2003: _temp2002=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1939)->f1;
goto _LL1952;} else{ goto _LL1953;} _LL1953: if(( unsigned int) _temp1939 > 2u?((
struct _tunion_struct*) _temp1939)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL2005:
_temp2004=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1939)->f1;
goto _LL1954;} else{ goto _LL1955;} _LL1955: if(( unsigned int) _temp1939 > 2u?((
struct _tunion_struct*) _temp1939)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL2011:
_temp2010=( struct _tuple0*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1939)->f1;
goto _LL2009; _LL2009: _temp2008=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1939)->f2; goto _LL2007; _LL2007: _temp2006=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1939)->f3; goto _LL1956;} else{ goto
_LL1957;} _LL1957: if(( unsigned int) _temp1939 > 2u?(( struct _tunion_struct*)
_temp1939)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL2021: _temp2020=( struct
_tuple0*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1939)->f1; goto _LL2019;
_LL2019: _temp2018=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1939)->f2; goto _LL2017; _LL2017: _temp2016=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1939)->f3; if( _temp2016 == 0){ goto
_LL2015;} else{ goto _LL1959;} _LL2015: _temp2014=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1939)->f4; goto _LL2013; _LL2013:
_temp2012=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1939)->f5; goto _LL1958;} else{ goto _LL1959;} _LL1959: if(( unsigned int)
_temp1939 > 2u?(( struct _tunion_struct*) _temp1939)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL2024: _temp2023=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1939)->f1; _temp2025=* _temp2023; _LL2031: _temp2030=( void*) _temp2025.r;
if(( unsigned int) _temp2030 > 2u?(( struct _tunion_struct*) _temp2030)->tag ==
Cyc_Absyn_Tunion_p_tag: 0){ _LL2041: _temp2040=( struct _tuple0*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2030)->f1; goto _LL2039; _LL2039: _temp2038=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2030)->f2; goto _LL2037; _LL2037:
_temp2036=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2030)->f3; goto _LL2035; _LL2035: _temp2034=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp2030)->f4; goto _LL2033; _LL2033:
_temp2032=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2030)->f5; goto _LL2029;} else{ goto _LL1961;} _LL2029: _temp2028=( struct
Cyc_Core_Opt*) _temp2025.topt; goto _LL2027; _LL2027: _temp2026=( struct Cyc_Position_Segment*)
_temp2025.loc; goto _LL2022;} else{ goto _LL1961;} _LL2022: if( _temp2036 != 0){
goto _LL1960;} else{ goto _LL1961;} _LL1961: if(( unsigned int) _temp1939 > 2u?((
struct _tunion_struct*) _temp1939)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL2043:
_temp2042=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1939)->f1; _temp2044=* _temp2042; _LL2050: _temp2049=( void*) _temp2044.r;
if(( unsigned int) _temp2049 > 2u?(( struct _tunion_struct*) _temp2049)->tag ==
Cyc_Absyn_XTunion_p_tag: 0){ _LL2060: _temp2059=( struct _tuple0*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp2049)->f1; goto _LL2058; _LL2058: _temp2057=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp2049)->f2; goto _LL2056; _LL2056:
_temp2055=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp2049)->f3; goto _LL2054; _LL2054: _temp2053=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp2049)->f4; goto _LL2052; _LL2052:
_temp2051=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp2049)->f5; goto _LL2048;} else{ goto _LL1963;} _LL2048: _temp2047=( struct
Cyc_Core_Opt*) _temp2044.topt; goto _LL2046; _LL2046: _temp2045=( struct Cyc_Position_Segment*)
_temp2044.loc; goto _LL1962;} else{ goto _LL1963;} _LL1963: if(( unsigned int)
_temp1939 > 2u?(( struct _tunion_struct*) _temp1939)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL2070: _temp2069=( struct _tuple0*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1939)->f1; goto _LL2068; _LL2068: _temp2067=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1939)->f2; goto _LL2066; _LL2066:
_temp2065=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1939)->f3; goto _LL2064; _LL2064: _temp2063=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1939)->f4; goto _LL2062; _LL2062:
_temp2061=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1939)->f5; goto _LL1964;} else{ goto _LL1965;} _LL1965: if(( unsigned int)
_temp1939 > 2u?(( struct _tunion_struct*) _temp1939)->tag == Cyc_Absyn_XTunion_p_tag:
0){ _LL2080: _temp2079=( struct _tuple0*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1939)->f1; goto _LL2078; _LL2078: _temp2077=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1939)->f2; goto _LL2076; _LL2076:
_temp2075=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1939)->f3; goto _LL2074; _LL2074: _temp2073=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1939)->f4; goto _LL2072; _LL2072:
_temp2071=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1939)->f5; goto _LL1966;} else{ goto _LL1967;} _LL1967: if(( unsigned int)
_temp1939 > 2u?(( struct _tunion_struct*) _temp1939)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL2082: _temp2081=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1939)->f1; goto _LL1968;} else{ goto _LL1969;} _LL1969: if(( unsigned int)
_temp1939 > 2u?(( struct _tunion_struct*) _temp1939)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL2090: _temp2089=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1939)->f1; goto _LL2088; _LL2088: _temp2087=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1939)->f2; goto _LL2086; _LL2086:
_temp2085=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1939)->f3; goto _LL2084; _LL2084: _temp2083=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1939)->f4; goto _LL1970;} else{ goto
_LL1971;} _LL1971: if(( unsigned int) _temp1939 > 2u?(( struct _tunion_struct*)
_temp1939)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL2092: _temp2091=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1939)->f1; goto _LL1972;} else{ goto
_LL1973;} _LL1973: if(( unsigned int) _temp1939 > 2u?(( struct _tunion_struct*)
_temp1939)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL2094: _temp2093=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1939)->f1; goto _LL1974;}
else{ goto _LL1975;} _LL1975: if(( unsigned int) _temp1939 > 2u?(( struct
_tunion_struct*) _temp1939)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){ _LL2100:
_temp2099=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1939)->f1;
goto _LL2098; _LL2098: _temp2097=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1939)->f2; goto _LL2096; _LL2096: _temp2095=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1939)->f3; goto _LL1976;} else{
goto _LL1977;} _LL1977: if(( unsigned int) _temp1939 > 2u?(( struct
_tunion_struct*) _temp1939)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){ _LL2106:
_temp2105=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1939)->f1; goto _LL2104; _LL2104: _temp2103=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1939)->f2; goto _LL2102; _LL2102:
_temp2101=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1939)->f3; goto _LL1978;} else{ goto _LL1940;} _LL1942: nv= Cyc_Toc_add_varmap(
nv, _temp1992, r); goto _LL1944; _LL1944: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL1940; _LL1946: { struct _tuple0* v= Cyc_Toc_temp_var(); decls=({ struct
Cyc_List_List* _temp2107=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2107->hd=( void*)({ struct _tuple14* _temp2108=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2108->f1= v; _temp2108->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2108;}); _temp2107->tl= decls;
_temp2107;}); nv= Cyc_Toc_add_varmap( nv, _temp1996->name, Cyc_Absyn_var_exp( v,
0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp(
path, 0), 0), Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1940;} _LL1948: s=
Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab);
goto _LL1940; _LL1950: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp(
_temp2000, _temp1998, 0), succ_lab, fail_lab); goto _LL1940; _LL1952: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_char_exp( _temp2002, 0), succ_lab, fail_lab); goto _LL1940; _LL1954:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp( _temp2004, 0), succ_lab,
fail_lab); goto _LL1940; _LL1956: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2109=( struct Cyc_Absyn_Enum_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct)); _temp2109[ 0]=({ struct Cyc_Absyn_Enum_e_struct
_temp2110; _temp2110.tag= Cyc_Absyn_Enum_e_tag; _temp2110.f1= _temp2010;
_temp2110.f2=( struct Cyc_Absyn_Enumdecl*) _temp2008; _temp2110.f3=( struct Cyc_Absyn_Enumfield*)
_temp2006; _temp2110;}); _temp2109;}), 0), succ_lab, fail_lab); goto _LL1940;
_LL1958: s= Cyc_Toc_if_eq_goto_stmt( Cyc_Absyn_cast_exp( Cyc_Absyn_sint_t, r, 0),
Cyc_Absyn_var_exp( _temp2020, 0), succ_lab, fail_lab); goto _LL1940; _LL1960: {
struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2036); struct _tuple0* efstrct= Cyc_Toc_collapse_qvar_tag( _temp2032->name,(
struct _tagged_string)({ char* _temp2124=( char*)"_struct"; struct
_tagged_string _temp2125; _temp2125.curr= _temp2124; _temp2125.base= _temp2124;
_temp2125.last_plus_one= _temp2124 + 8; _temp2125;})); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( efstrct), Cyc_Toc_mt_tq),
r, 0);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2036); for( 0; rps != 0; rps=({ struct Cyc_List_List*
_temp2111= rps; if( _temp2111 == 0){ _throw( Null_Exception);} _temp2111->tl;}),
cnt --){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp2118= rps; if( _temp2118 == 0){ _throw( Null_Exception);} _temp2118->hd;});
struct _tuple0* v= Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp2117= p2->topt; if( _temp2117 == 0){ _throw( Null_Exception);} _temp2117->v;});
void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp2112=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2112->hd=( void*)({ struct _tuple14*
_temp2113=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2113->f1=
v; _temp2113->f2= t2c; _temp2113;}); _temp2112->tl= decls; _temp2112;});{ struct
_tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp2116=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2116[ 0]= xprintf("f%d", cnt);
_temp2116;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp2115=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2115[ 0]= xprintf("f%d", cnt);
_temp2115;}), 0), 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3,
0); ss=({ struct Cyc_List_List* _temp2114=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2114->hd=( void*) Cyc_Absyn_label_stmt( lab,
s2, 0); _temp2114->tl= ss; _temp2114;}); succ_lab= lab;}}}}{ unsigned int
max_tag= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2121= _temp2034->fields; if( _temp2121 == 0){ _throw( Null_Exception);}
_temp2121->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp2119= fs; if(
_temp2119 == 0){ _throw( Null_Exception);} _temp2119->tl;})){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2120= fs; if(
_temp2120 == 0){ _throw( Null_Exception);} _temp2120->hd;}); if( f->typs == 0){
max_tag ++;}}}{ struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__tunion_struct_sp), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structarrow_exp(
e3, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( _temp2040,( struct _tagged_string)({ char* _temp2122=(
char*)"_tag"; struct _tagged_string _temp2123; _temp2123.curr= _temp2122;
_temp2123.base= _temp2122; _temp2123.last_plus_one= _temp2122 + 5; _temp2123;})),
0); struct Cyc_Absyn_Exp* test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1940;}}} _LL1962: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2055); struct _tuple0* xfstrct=
Cyc_Toc_collapse_qvar_tag( _temp2051->name,( struct _tagged_string)({ char*
_temp2138=( char*)"_struct"; struct _tagged_string _temp2139; _temp2139.curr=
_temp2138; _temp2139.base= _temp2138; _temp2139.last_plus_one= _temp2138 + 8;
_temp2139;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq( xfstrct), Cyc_Toc_mt_tq), r, 0);{ struct Cyc_List_List* rps=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp2055);
for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp2126= rps; if( _temp2126 ==
0){ _throw( Null_Exception);} _temp2126->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2133= rps; if(
_temp2133 == 0){ _throw( Null_Exception);} _temp2133->hd;}); struct _tuple0* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp2132= p2->topt;
if( _temp2132 == 0){ _throw( Null_Exception);} _temp2132->v;}); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2127=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2127->hd=( void*)({ struct _tuple14* _temp2128=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2128->f1= v; _temp2128->f2= Cyc_Toc_typ_to_c(
t2); _temp2128;}); _temp2127->tl= decls; _temp2127;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp2131=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2131[ 0]= xprintf("f%d", cnt);
_temp2131;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp2130=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2130[ 0]= xprintf("f%d", cnt); _temp2130;}), 0), 0);
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List*
_temp2129=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2129->hd=( void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp2129->tl= ss;
_temp2129;}); succ_lab= lab;}}}} if( _temp2055 == 0){ s= Cyc_Toc_if_eq_goto_stmt(
Cyc_Absyn_structarrow_exp( r, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
_temp2059,( struct _tagged_string)({ char* _temp2134=( char*)"_tag"; struct
_tagged_string _temp2135; _temp2135.curr= _temp2134; _temp2135.base= _temp2134;
_temp2135.last_plus_one= _temp2134 + 5; _temp2135;})), 0), succ_lab, fail_lab);}
else{ struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__xtunion_struct_sp), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e2=
Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structmember_exp(
e2, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
_temp2059,( struct _tagged_string)({ char* _temp2136=( char*)"_tag"; struct
_tagged_string _temp2137; _temp2137.curr= _temp2136; _temp2137.base= _temp2136;
_temp2137.last_plus_one= _temp2136 + 5; _temp2137;})), 0); s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} goto _LL1940;} _LL1964: _temp2075= _temp2065; goto _LL1966;
_LL1966: _temp2081= _temp2075; goto _LL1968; _LL1968: { struct Cyc_List_List* ss=
0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2081);{
struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp2081); for( 0; rps != 0; rps=({ struct Cyc_List_List*
_temp2140= rps; if( _temp2140 == 0){ _throw( Null_Exception);} _temp2140->tl;}),
cnt --){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp2147= rps; if( _temp2147 == 0){ _throw( Null_Exception);} _temp2147->hd;});
struct _tuple0* v= Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp2146= p2->topt; if( _temp2146 == 0){ _throw( Null_Exception);} _temp2146->v;});
struct _tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2141=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2141->hd=( void*)({ struct _tuple14* _temp2142=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2142->f1= v; _temp2142->f2= Cyc_Toc_typ_to_c(
t2); _temp2142;}); _temp2141->tl= decls; _temp2141;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp2145=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2145[ 0]= xprintf("f%d", cnt);
_temp2145;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp2144=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2144[ 0]= xprintf("f%d", cnt); _temp2144;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp2143=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2143->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp2143->tl= ss; _temp2143;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1940;} _LL1970: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2083); for( 0; dlps != 0; dlps=({ struct Cyc_List_List* _temp2148= dlps;
if( _temp2148 == 0){ _throw( Null_Exception);} _temp2148->tl;})){ struct
_tuple15* tup=( struct _tuple15*)({ struct Cyc_List_List* _temp2164= dlps; if(
_temp2164 == 0){ _throw( Null_Exception);} _temp2164->hd;}); struct Cyc_Absyn_Pat*
p2=(* tup).f2; struct _tagged_string* f;{ void* _temp2150=( void*)({ struct Cyc_List_List*
_temp2149=(* tup).f1; if( _temp2149 == 0){ _throw( Null_Exception);} _temp2149->hd;});
struct _tagged_string* _temp2156; _LL2152: if((( struct _tunion_struct*)
_temp2150)->tag == Cyc_Absyn_FieldName_tag){ _LL2157: _temp2156=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp2150)->f1; goto
_LL2153;} else{ goto _LL2154;} _LL2154: goto _LL2155; _LL2153: f= _temp2156;
goto _LL2151; _LL2155:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Toc_Match_error_struct*
_temp2158=( struct Cyc_Toc_Match_error_struct*) GC_malloc( sizeof( struct Cyc_Toc_Match_error_struct));
_temp2158[ 0]=({ struct Cyc_Toc_Match_error_struct _temp2159; _temp2159.tag= Cyc_Toc_Match_error_tag;
_temp2159;}); _temp2158;})); _LL2151:;}{ struct _tuple0* v= Cyc_Toc_temp_var();
void* t2=( void*)({ struct Cyc_Core_Opt* _temp2163= p2->topt; if( _temp2163 == 0){
_throw( Null_Exception);} _temp2163->v;}); void* t2c= Cyc_Toc_typ_to_c( t2);
struct _tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2160=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2160->hd=( void*)({ struct _tuple14* _temp2161=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2161->f1= v; _temp2161->f2= t2c;
_temp2161;}); _temp2160->tl= decls; _temp2160;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( path, f, 0), p2,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp( r,
f, 0), 0), 0), s2, 0); ss=({ struct Cyc_List_List* _temp2162=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2162->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp2162->tl= ss; _temp2162;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1940;} _LL1972: { struct _tuple0* v= Cyc_Toc_temp_var(); void*
t2=( void*)({ struct Cyc_Core_Opt* _temp2167= _temp2091->topt; if( _temp2167 ==
0){ _throw( Null_Exception);} _temp2167->v;}); decls=({ struct Cyc_List_List*
_temp2165=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2165->hd=( void*)({ struct _tuple14* _temp2166=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2166->f1= v; _temp2166->f2= Cyc_Toc_typ_to_c(
t2); _temp2166;}); _temp2165->tl= decls; _temp2165;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path,
0), _temp2091, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0),
s2, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);}
else{ s= s3;} goto _LL1940;}}} _LL1974: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2168=(
char*)"unknownid"; struct _tagged_string _temp2169; _temp2169.curr= _temp2168;
_temp2169.base= _temp2168; _temp2169.last_plus_one= _temp2168 + 10; _temp2169;}));
goto _LL1940; _LL1976: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2170=( char*)"unknowncall";
struct _tagged_string _temp2171; _temp2171.curr= _temp2170; _temp2171.base=
_temp2170; _temp2171.last_plus_one= _temp2170 + 12; _temp2171;})); goto _LL1940;
_LL1978: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2172=( char*)"unknownfields"; struct
_tagged_string _temp2173; _temp2173.curr= _temp2172; _temp2173.base= _temp2172;
_temp2173.last_plus_one= _temp2172 + 14; _temp2173;})); goto _LL1940; _LL1940:;}
return({ struct _tuple13 _temp2174; _temp2174.f1= nv; _temp2174.f2= decls;
_temp2174.f3= s; _temp2174;});} struct _tuple16{ struct _tagged_string* f1;
struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause* f3; } ; static struct
_tuple16* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause* sc){ return({
struct _tuple16* _temp2175=( struct _tuple16*) GC_malloc( sizeof( struct
_tuple16)); _temp2175->f1= Cyc_Toc_fresh_label(); _temp2175->f2= Cyc_Toc_fresh_label();
_temp2175->f3= sc; _temp2175;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* t=( void*)({ struct Cyc_Core_Opt*
_temp2231= e->topt; if( _temp2231 == 0){ _throw( Null_Exception);} _temp2231->v;});
int leave_as_switch;{ void* _temp2176= Cyc_Tcutil_compress( t); void* _temp2182;
void* _temp2184; _LL2178: if(( unsigned int) _temp2176 > 4u?(( struct
_tunion_struct*) _temp2176)->tag == Cyc_Absyn_IntType_tag: 0){ _LL2185:
_temp2184=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2176)->f1; goto
_LL2183; _LL2183: _temp2182=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2176)->f2; goto _LL2179;} else{ goto _LL2180;} _LL2180: goto _LL2181;
_LL2179: leave_as_switch= 1; goto _LL2177; _LL2181: leave_as_switch= 0; goto
_LL2177; _LL2177:;}{ struct Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1=({
struct Cyc_List_List* _temp2186= scs1; if( _temp2186 == 0){ _throw(
Null_Exception);} _temp2186->tl;})){ if(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2188=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp2187=
scs1; if( _temp2187 == 0){ _throw( Null_Exception);} _temp2187->hd;}))->pat_vars;
if( _temp2188 == 0){ _throw( Null_Exception);} _temp2188->v;}) != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* scs1= scs; for( 0; scs1 !=
0; scs1=({ struct Cyc_List_List* _temp2189= scs1; if( _temp2189 == 0){ _throw(
Null_Exception);} _temp2189->tl;})){ struct Cyc_Absyn_Stmt* s=(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2191= scs1; if( _temp2191 == 0){ _throw(
Null_Exception);} _temp2191->hd;}))->body;(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2190= scs1; if( _temp2190 == 0){ _throw(
Null_Exception);} _temp2190->hd;}))->body= Cyc_Absyn_label_stmt( next_l, s, 0);
next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), s);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0);
struct _tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple16*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* lscs2= lscs; for( 0; lscs2 != 0; lscs2=({ struct Cyc_List_List*
_temp2192= lscs2; if( _temp2192 == 0){ _throw( Null_Exception);} _temp2192->tl;})){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2204= lscs2; if( _temp2204 == 0){ _throw( Null_Exception);} _temp2204->hd;}))).f3;
struct _tagged_string* fail_lab=({ struct Cyc_List_List* _temp2201= lscs2; if(
_temp2201 == 0){ _throw( Null_Exception);} _temp2201->tl;}) == 0? end_l:(*((
struct _tuple16*)({ struct Cyc_List_List* _temp2203=({ struct Cyc_List_List*
_temp2202= lscs2; if( _temp2202 == 0){ _throw( Null_Exception);} _temp2202->tl;});
if( _temp2203 == 0){ _throw( Null_Exception);} _temp2203->hd;}))).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2200= lscs2; if( _temp2200 == 0){ _throw( Null_Exception);} _temp2200->hd;}))).f2;
if( sc->where_clause == 0){ struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r,
path, sc->pattern, succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List*
_temp2193=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2193->hd=( void*) res.f1; _temp2193->tl= nvs; _temp2193;}); decls= res.f2;
test_stmts=({ struct Cyc_List_List* _temp2194=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2194->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2195= lscs2; if( _temp2195 == 0){
_throw( Null_Exception);} _temp2195->hd;}))).f1, res.f3, 0); _temp2194->tl=
test_stmts; _temp2194;});} else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2199= sc->where_clause; if( _temp2199 == 0){ _throw(
Null_Exception);} _temp2199;}); struct _tagged_string* intermed_lab= Cyc_Toc_fresh_label();
struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r, path, sc->pattern,
intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c( res.f1, e); nvs=({ struct Cyc_List_List*
_temp2196=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2196->hd=( void*) res.f1; _temp2196->tl= nvs; _temp2196;}); decls= res.f2;{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt( e, Cyc_Absyn_goto_stmt(
succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3=
Cyc_Absyn_label_stmt( intermed_lab, s2, 0); test_stmts=({ struct Cyc_List_List*
_temp2197=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2197->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2198= lscs2; if( _temp2198 == 0){ _throw( Null_Exception);} _temp2198->hd;}))).f1,
Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0); _temp2197->tl= test_stmts; _temp2197;});}}}}{
struct Cyc_Absyn_Stmt* test_stmt= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( test_stmts), 0); nvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt*
clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){ for( 0;({ struct Cyc_List_List*
_temp2205= lscs; if( _temp2205 == 0){ _throw( Null_Exception);} _temp2205->tl;})
!= 0; lscs=({ struct Cyc_List_List* _temp2206= lscs; if( _temp2206 == 0){ _throw(
Null_Exception);} _temp2206->tl;}), nvs=({ struct Cyc_List_List* _temp2207= nvs;
if( _temp2207 == 0){ _throw( Null_Exception);} _temp2207->tl;})){ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2217= nvs; if( _temp2217
== 0){ _throw( Null_Exception);} _temp2217->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2216= lscs; if( _temp2216 == 0){
_throw( Null_Exception);} _temp2216->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
e, end_l,(*(( struct _tuple16*)({ struct Cyc_List_List* _temp2209=({ struct Cyc_List_List*
_temp2208= lscs; if( _temp2208 == 0){ _throw( Null_Exception);} _temp2208->tl;});
if( _temp2209 == 0){ _throw( Null_Exception);} _temp2209->hd;}))).f2,( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2212=((*(( struct _tuple16*)({
struct Cyc_List_List* _temp2211=({ struct Cyc_List_List* _temp2210= lscs; if(
_temp2210 == 0){ _throw( Null_Exception);} _temp2210->tl;}); if( _temp2211 == 0){
_throw( Null_Exception);} _temp2211->hd;}))).f3)->pat_vars; if( _temp2212 == 0){
_throw( Null_Exception);} _temp2212->v;}),( struct Cyc_Toc_Env*)({ struct Cyc_List_List*
_temp2214=({ struct Cyc_List_List* _temp2213= nvs; if( _temp2213 == 0){ _throw(
Null_Exception);} _temp2213->tl;}); if( _temp2214 == 0){ _throw( Null_Exception);}
_temp2214->hd;})), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2215= lscs; if( _temp2215 == 0){
_throw( Null_Exception);} _temp2215->hd;}))).f2, s, 0), 0);}{ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2220= nvs; if( _temp2220
== 0){ _throw( Null_Exception);} _temp2220->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2219= lscs; if( _temp2219 == 0){
_throw( Null_Exception);} _temp2219->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env(
e, end_l), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2218= lscs; if( _temp2218 == 0){
_throw( Null_Exception);} _temp2218->hd;}))).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( test_stmt, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=({
struct Cyc_List_List* _temp2221= decls; if( _temp2221 == 0){ _throw(
Null_Exception);} _temp2221->tl;})){ struct _tuple14 _temp2225; void* _temp2226;
struct _tuple0* _temp2228; struct _tuple14* _temp2223=( struct _tuple14*)({
struct Cyc_List_List* _temp2222= decls; if( _temp2222 == 0){ _throw(
Null_Exception);} _temp2222->hd;}); _temp2225=* _temp2223; _LL2229: _temp2228=
_temp2225.f1; goto _LL2227; _LL2227: _temp2226= _temp2225.f2; goto _LL2224;
_LL2224: res= Cyc_Absyn_declare_stmt( _temp2228, _temp2226, 0, res, 0);}( void*)(
whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( v, Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp2230= e->topt; if( _temp2230 == 0){ _throw(
Null_Exception);} _temp2230->v;})),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2232=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2232[ 0]=({ struct Cyc_List_List _temp2233; _temp2233.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2233.tl= 0; _temp2233;}); _temp2232;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2234=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2274; struct Cyc_Absyn_Stmt* _temp2276; struct Cyc_Absyn_Stmt*
_temp2278; struct Cyc_Absyn_Exp* _temp2280; struct Cyc_Absyn_Stmt* _temp2282;
struct Cyc_Absyn_Stmt* _temp2284; struct Cyc_Absyn_Exp* _temp2286; struct Cyc_Absyn_Stmt*
_temp2288; struct _tuple2 _temp2290; struct Cyc_Absyn_Stmt* _temp2292; struct
Cyc_Absyn_Exp* _temp2294; struct Cyc_Absyn_Stmt* _temp2296; struct Cyc_Absyn_Stmt*
_temp2298; struct Cyc_Absyn_Stmt* _temp2300; struct _tagged_string* _temp2302;
struct Cyc_Absyn_Stmt* _temp2304; struct _tuple2 _temp2306; struct Cyc_Absyn_Stmt*
_temp2308; struct Cyc_Absyn_Exp* _temp2310; struct _tuple2 _temp2312; struct Cyc_Absyn_Stmt*
_temp2314; struct Cyc_Absyn_Exp* _temp2316; struct Cyc_Absyn_Exp* _temp2318;
struct Cyc_List_List* _temp2320; struct Cyc_Absyn_Exp* _temp2322; struct Cyc_Absyn_Switch_clause**
_temp2324; struct Cyc_List_List* _temp2326; struct Cyc_Absyn_Stmt* _temp2328;
struct Cyc_Absyn_Decl* _temp2330; struct Cyc_Absyn_Stmt* _temp2332; struct Cyc_Absyn_Stmt*
_temp2334; struct Cyc_Absyn_Stmt* _temp2336; struct _tagged_string* _temp2338;
struct _tuple2 _temp2340; struct Cyc_Absyn_Stmt* _temp2342; struct Cyc_Absyn_Exp*
_temp2344; struct Cyc_Absyn_Stmt* _temp2346; struct Cyc_List_List* _temp2348;
struct Cyc_Absyn_Stmt* _temp2350; struct Cyc_Absyn_Stmt* _temp2352; struct Cyc_Absyn_Vardecl*
_temp2354; struct Cyc_Absyn_Tvar* _temp2356; _LL2236: if(( int) _temp2234 == Cyc_Absyn_Skip_s){
goto _LL2237;} else{ goto _LL2238;} _LL2238: if(( unsigned int) _temp2234 > 1u?((
struct _tunion_struct*) _temp2234)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL2275:
_temp2274=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp2234)->f1;
goto _LL2239;} else{ goto _LL2240;} _LL2240: if(( unsigned int) _temp2234 > 1u?((
struct _tunion_struct*) _temp2234)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL2279:
_temp2278=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp2234)->f1;
goto _LL2277; _LL2277: _temp2276=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp2234)->f2; goto _LL2241;} else{ goto _LL2242;} _LL2242: if(( unsigned int)
_temp2234 > 1u?(( struct _tunion_struct*) _temp2234)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL2281: _temp2280=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp2234)->f1; goto _LL2243;} else{ goto _LL2244;} _LL2244: if(( unsigned int)
_temp2234 > 1u?(( struct _tunion_struct*) _temp2234)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL2287: _temp2286=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2234)->f1; goto _LL2285; _LL2285: _temp2284=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2234)->f2; goto _LL2283; _LL2283:
_temp2282=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2234)->f3; goto _LL2245;} else{ goto _LL2246;} _LL2246: if(( unsigned int)
_temp2234 > 1u?(( struct _tunion_struct*) _temp2234)->tag == Cyc_Absyn_While_s_tag:
0){ _LL2291: _temp2290=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*)
_temp2234)->f1; _LL2295: _temp2294= _temp2290.f1; goto _LL2293; _LL2293:
_temp2292= _temp2290.f2; goto _LL2289; _LL2289: _temp2288=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp2234)->f2; goto _LL2247;} else{ goto
_LL2248;} _LL2248: if(( unsigned int) _temp2234 > 1u?(( struct _tunion_struct*)
_temp2234)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL2297: _temp2296=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp2234)->f1; goto _LL2249;} else{ goto
_LL2250;} _LL2250: if(( unsigned int) _temp2234 > 1u?(( struct _tunion_struct*)
_temp2234)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL2299: _temp2298=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp2234)->f1; goto _LL2251;} else{ goto
_LL2252;} _LL2252: if(( unsigned int) _temp2234 > 1u?(( struct _tunion_struct*)
_temp2234)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL2303: _temp2302=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp2234)->f1; goto _LL2301;
_LL2301: _temp2300=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp2234)->f2; goto _LL2253;} else{ goto _LL2254;} _LL2254: if(( unsigned int)
_temp2234 > 1u?(( struct _tunion_struct*) _temp2234)->tag == Cyc_Absyn_For_s_tag:
0){ _LL2319: _temp2318=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp2234)->f1; goto _LL2313; _LL2313: _temp2312=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp2234)->f2; _LL2317: _temp2316= _temp2312.f1; goto _LL2315; _LL2315:
_temp2314= _temp2312.f2; goto _LL2307; _LL2307: _temp2306=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp2234)->f3; _LL2311: _temp2310= _temp2306.f1;
goto _LL2309; _LL2309: _temp2308= _temp2306.f2; goto _LL2305; _LL2305: _temp2304=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp2234)->f4; goto
_LL2255;} else{ goto _LL2256;} _LL2256: if(( unsigned int) _temp2234 > 1u?((
struct _tunion_struct*) _temp2234)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL2323:
_temp2322=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp2234)->f1; goto _LL2321; _LL2321: _temp2320=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp2234)->f2; goto _LL2257;} else{ goto
_LL2258;} _LL2258: if(( unsigned int) _temp2234 > 1u?(( struct _tunion_struct*)
_temp2234)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL2327: _temp2326=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp2234)->f1; goto _LL2325; _LL2325:
_temp2324=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2234)->f2; goto _LL2259;} else{ goto _LL2260;} _LL2260: if(( unsigned int)
_temp2234 > 1u?(( struct _tunion_struct*) _temp2234)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL2331: _temp2330=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp2234)->f1; goto _LL2329; _LL2329: _temp2328=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp2234)->f2; goto _LL2261;} else{ goto
_LL2262;} _LL2262: if(( unsigned int) _temp2234 > 1u?(( struct _tunion_struct*)
_temp2234)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL2333: _temp2332=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp2234)->f1; goto _LL2263;} else{ goto
_LL2264;} _LL2264: if(( unsigned int) _temp2234 > 1u?(( struct _tunion_struct*)
_temp2234)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL2335: _temp2334=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp2234)->f1; goto _LL2265;} else{ goto
_LL2266;} _LL2266: if(( unsigned int) _temp2234 > 1u?(( struct _tunion_struct*)
_temp2234)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL2339: _temp2338=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp2234)->f1; goto
_LL2337; _LL2337: _temp2336=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp2234)->f2; goto _LL2267;} else{ goto _LL2268;} _LL2268: if(( unsigned int)
_temp2234 > 1u?(( struct _tunion_struct*) _temp2234)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL2347: _temp2346=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp2234)->f1; goto _LL2341; _LL2341: _temp2340=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp2234)->f2; _LL2345: _temp2344= _temp2340.f1; goto _LL2343; _LL2343:
_temp2342= _temp2340.f2; goto _LL2269;} else{ goto _LL2270;} _LL2270: if((
unsigned int) _temp2234 > 1u?(( struct _tunion_struct*) _temp2234)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL2351: _temp2350=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2234)->f1; goto _LL2349; _LL2349: _temp2348=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp2234)->f2; goto _LL2271;} else{ goto
_LL2272;} _LL2272: if(( unsigned int) _temp2234 > 1u?(( struct _tunion_struct*)
_temp2234)->tag == Cyc_Absyn_Region_s_tag: 0){ _LL2357: _temp2356=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_Region_s_struct*) _temp2234)->f1; goto _LL2355; _LL2355:
_temp2354=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2234)->f2; goto _LL2353; _LL2353: _temp2352=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Region_s_struct*) _temp2234)->f3; goto _LL2273;} else{ goto
_LL2235;} _LL2237: return; _LL2239: Cyc_Toc_exp_to_c( nv, _temp2274); return;
_LL2241: Cyc_Toc_stmt_to_c( nv, _temp2278); s= _temp2276; continue; _LL2243: {
struct Cyc_Core_Opt* topt= 0; if( _temp2280 != 0){ topt=({ struct Cyc_Core_Opt*
_temp2358=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2358->v=( void*) Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp2360=({
struct Cyc_Absyn_Exp* _temp2359= _temp2280; if( _temp2359 == 0){ _throw(
Null_Exception);} _temp2359->topt;}); if( _temp2360 == 0){ _throw(
Null_Exception);} _temp2360->v;})); _temp2358;}); Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2361= _temp2280; if( _temp2361 == 0){
_throw( Null_Exception);} _temp2361;}));} if( s->try_depth > 0){ if( topt != 0){
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* retn_stmt= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x, 0), 0);( void*)( s->r=( void*)((
void*)( Cyc_Absyn_declare_stmt( x,( void*)({ struct Cyc_Core_Opt* _temp2362=
topt; if( _temp2362 == 0){ _throw( Null_Exception);} _temp2362->v;}), _temp2280,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), retn_stmt, 0), 0))->r));}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2245: Cyc_Toc_exp_to_c(
nv, _temp2286); Cyc_Toc_stmt_to_c( nv, _temp2284); s= _temp2282; continue;
_LL2247: Cyc_Toc_exp_to_c( nv, _temp2294); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2288); return; _LL2249: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)({ struct Cyc_Core_Opt*
_temp2363= nv->break_lab; if( _temp2363 == 0){ _throw( Null_Exception);}
_temp2363->v;}), 0))->r));}{ int dest_depth= _temp2296 == 0? 0:({ struct Cyc_Absyn_Stmt*
_temp2364= _temp2296; if( _temp2364 == 0){ _throw( Null_Exception);} _temp2364->try_depth;});
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL2251: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)({ struct Cyc_Core_Opt* _temp2365= nv->continue_lab; if(
_temp2365 == 0){ _throw( Null_Exception);} _temp2365->v;}), 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2366= _temp2298; if( _temp2366 == 0){
_throw( Null_Exception);} _temp2366->try_depth;}), s); return; _LL2253: Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2367= _temp2300; if( _temp2367 == 0){
_throw( Null_Exception);} _temp2367->try_depth;}), s); return; _LL2255: Cyc_Toc_exp_to_c(
nv, _temp2318); Cyc_Toc_exp_to_c( nv, _temp2316); Cyc_Toc_exp_to_c( nv,
_temp2310); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2304); return;
_LL2257: Cyc_Toc_xlate_switch( nv, s, _temp2322, _temp2320); return; _LL2259:
if( nv->fallthru_info == 0){( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2368=(
char*)"fallthru in unexpected place"; struct _tagged_string _temp2369; _temp2369.curr=
_temp2368; _temp2369.base= _temp2368; _temp2369.last_plus_one= _temp2368 + 29;
_temp2369;})));}{ struct _tuple8 _temp2373; struct Cyc_Dict_Dict* _temp2374;
struct Cyc_List_List* _temp2376; struct _tagged_string* _temp2378; struct
_tuple8* _temp2371=( struct _tuple8*)({ struct Cyc_Core_Opt* _temp2370= nv->fallthru_info;
if( _temp2370 == 0){ _throw( Null_Exception);} _temp2370->v;}); _temp2373=*
_temp2371; _LL2379: _temp2378= _temp2373.f1; goto _LL2377; _LL2377: _temp2376=
_temp2373.f2; goto _LL2375; _LL2375: _temp2374= _temp2373.f3; goto _LL2372;
_LL2372: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp2378, 0); Cyc_Toc_do_npop_before(
s->try_depth -(({ struct Cyc_Absyn_Switch_clause** _temp2380= _temp2324; if(
_temp2380 == 0){ _throw( Null_Exception);}* _temp2380;})->body)->try_depth, s2);{
struct Cyc_List_List* vs2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp2376); struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp2326); for( 0; vs2 != 0; vs2=({
struct Cyc_List_List* _temp2381= vs2; if( _temp2381 == 0){ _throw(
Null_Exception);} _temp2381->tl;}), es2=({ struct Cyc_List_List* _temp2382= es2;
if( _temp2382 == 0){ _throw( Null_Exception);} _temp2382->tl;})){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp2383= es2; if(
_temp2383 == 0){ _throw( Null_Exception);} _temp2383->hd;})); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp2374,( struct _tuple0*)({ struct
Cyc_List_List* _temp2384= vs2; if( _temp2384 == 0){ _throw( Null_Exception);}
_temp2384->hd;})),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp2385=
es2; if( _temp2385 == 0){ _throw( Null_Exception);} _temp2385->hd;}), 0), s2, 0);}(
void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL2261:{ void* _temp2386=(
void*) _temp2330->r; int _temp2394; struct Cyc_Absyn_Exp* _temp2396; struct Cyc_Core_Opt*
_temp2398; struct Cyc_Core_Opt* _temp2400; struct Cyc_Absyn_Pat* _temp2402;
struct Cyc_Absyn_Vardecl* _temp2404; _LL2388: if((( struct _tunion_struct*)
_temp2386)->tag == Cyc_Absyn_Let_d_tag){ _LL2403: _temp2402=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp2386)->f1; goto _LL2401; _LL2401: _temp2400=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2386)->f2; goto
_LL2399; _LL2399: _temp2398=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2386)->f3; goto _LL2397; _LL2397: _temp2396=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp2386)->f4; goto _LL2395; _LL2395: _temp2394=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp2386)->f5; goto _LL2389;} else{ goto
_LL2390;} _LL2390: if((( struct _tunion_struct*) _temp2386)->tag == Cyc_Absyn_Var_d_tag){
_LL2405: _temp2404=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2386)->f1; goto _LL2391;} else{ goto _LL2392;} _LL2392: goto _LL2393;
_LL2389: { int var_case= 0; struct Cyc_Absyn_Vardecl* vdopt= 0;{ void* _temp2406=(
void*) _temp2402->r; struct Cyc_Absyn_Vardecl* _temp2412; _LL2408: if((
unsigned int) _temp2406 > 2u?(( struct _tunion_struct*) _temp2406)->tag == Cyc_Absyn_Var_p_tag:
0){ _LL2413: _temp2412=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp2406)->f1; goto _LL2409;} else{ goto _LL2410;} _LL2410: goto _LL2411;
_LL2409: var_case= 1; vdopt=( struct Cyc_Absyn_Vardecl*) _temp2412; _temp2412->initializer=(
struct Cyc_Absyn_Exp*) _temp2396;( void*)( _temp2330->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2414=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2414[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2415; _temp2415.tag= Cyc_Absyn_Var_d_tag; _temp2415.f1= _temp2412;
_temp2415;}); _temp2414;}))); goto _LL2407; _LL2411: goto _LL2407; _LL2407:;}
if( var_case){ _temp2404=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_Absyn_Vardecl*
_temp2416= vdopt; if( _temp2416 == 0){ _throw( Null_Exception);} _temp2416;});
goto _LL2391;}( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv,
_temp2402, _temp2398, _temp2396, _temp2394, _temp2328))->r)); goto _LL2387;}
_LL2391: { struct _tuple0* c_name= _temp2404->name;( void*)( _temp2404->type=(
void*) Cyc_Toc_typ_to_c_array(( void*) _temp2404->type));{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, c_name, Cyc_Absyn_varb_exp( c_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2430=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2430[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2431; _temp2431.tag= Cyc_Absyn_Local_b_tag;
_temp2431.f1= _temp2404; _temp2431;}); _temp2430;}), 0)); Cyc_Toc_stmt_to_c( nv2,
_temp2328); if( _temp2404->initializer != 0){ struct Cyc_Absyn_Exp* init=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2429= _temp2404->initializer;
if( _temp2429 == 0){ _throw( Null_Exception);} _temp2429;}); void* _temp2417=(
void*) init->r; struct Cyc_Absyn_Exp* _temp2423; struct Cyc_Absyn_Exp* _temp2425;
struct Cyc_Absyn_Vardecl* _temp2427; _LL2419: if((( struct _tunion_struct*)
_temp2417)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL2428: _temp2427=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2417)->f1;
goto _LL2426; _LL2426: _temp2425=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2417)->f2; goto _LL2424; _LL2424: _temp2423=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2417)->f3; goto _LL2420;} else{
goto _LL2421;} _LL2421: goto _LL2422; _LL2420: _temp2404->initializer= 0;( void*)(
_temp2328->r=( void*)(( void*)( Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
c_name, 0), _temp2427, _temp2425, _temp2423, Cyc_Absyn_new_stmt(( void*)
_temp2328->r, 0), 0))->r)); goto _LL2418; _LL2422: Cyc_Toc_exp_to_c( nv, init);
goto _LL2418; _LL2418:;} goto _LL2387;}} _LL2393:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2432=(
char*)"bad nested declaration within function"; struct _tagged_string _temp2433;
_temp2433.curr= _temp2432; _temp2433.base= _temp2432; _temp2433.last_plus_one=
_temp2432 + 39; _temp2433;})); goto _LL2387; _LL2387:;} return; _LL2263:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp2434=( char*)"cut"; struct _tagged_string _temp2435; _temp2435.curr=
_temp2434; _temp2435.base= _temp2434; _temp2435.last_plus_one= _temp2434 + 4;
_temp2435;})); return; _LL2265:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2436=( char*)"splice"; struct _tagged_string
_temp2437; _temp2437.curr= _temp2436; _temp2437.base= _temp2436; _temp2437.last_plus_one=
_temp2436 + 7; _temp2437;})); return; _LL2267: s= _temp2336; continue; _LL2269:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2346); Cyc_Toc_exp_to_c( nv,
_temp2344); return; _LL2271: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct
_tuple0* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2453=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2453->f1= Cyc_Toc_abs_ns; _temp2453->f2= Cyc_Toc__push_handler_sp;
_temp2453;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple0* _temp2452=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2452->f1= Cyc_Toc_abs_ns; _temp2452->f2= Cyc_Toc_setjmp_sp; _temp2452;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2451=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2451->f1=
Cyc_Toc_abs_ns; _temp2451->f2= Cyc_Toc__pop_handler_sp; _temp2451;}), 0); e_exp->topt=({
struct Cyc_Core_Opt* _temp2438=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2438->v=( void*) e_typ; _temp2438;}); nv= Cyc_Toc_add_varmap(
nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2350);{ struct Cyc_Absyn_Stmt*
tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp2350, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0); struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl* x_vd= Cyc_Absyn_new_vardecl(
x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt* _temp2439=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2439->v=( void*)
Cyc_Absyn_exn_typ; _temp2439;});{ struct Cyc_Absyn_Pat* p=({ struct Cyc_Absyn_Pat*
_temp2447=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof( struct Cyc_Absyn_Pat));
_temp2447->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp2449=(
struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2449[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2450; _temp2450.tag= Cyc_Absyn_Var_p_tag;
_temp2450.f1= x_vd; _temp2450;}); _temp2449;})); _temp2447->topt=({ struct Cyc_Core_Opt*
_temp2448=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2448->v=( void*) Cyc_Absyn_exn_typ; _temp2448;}); _temp2447->loc= 0;
_temp2447;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp2440=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2440->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2440;});{
struct Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp2444=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2444->pattern= p;
_temp2444->pat_vars=({ struct Cyc_Core_Opt* _temp2445=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2445->v=( void*)({ struct Cyc_List_List*
_temp2446=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2446->hd=( void*) x_vd; _temp2446->tl= 0; _temp2446;}); _temp2445;});
_temp2444->where_clause= 0; _temp2444->body= rs; _temp2444->loc= 0; _temp2444;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2348,({
struct Cyc_List_List* _temp2443=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2443->hd=( void*) default_case; _temp2443->tl= 0;
_temp2443;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( e_typ, Cyc_Absyn_fncall_exp( setjmp_exp,({
struct Cyc_List_List* _temp2442=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2442->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2442->tl= 0; _temp2442;}), 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List*
_temp2441=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2441->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2441->tl= 0;
_temp2441;}), 0), 0), Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Not,
e_exp, 0), tryandpop_stmt, handler_stmt, 0), 0), 0), 0))->r)); return;}}}}}
_LL2273: { void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__region_handle_sp);
void* rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq);
void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct _tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0*
x_var= _temp2354->name; struct _tuple0* h_var= Cyc_Toc_temp_var(); struct
_tuple0* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2462=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2462[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2463; _temp2463.tag= Cyc_Absyn_Local_b_tag;
_temp2463.f1= _temp2354; _temp2463;}); _temp2462;}), 0); struct Cyc_Absyn_Exp*
h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({ struct
_tuple0* _temp2461=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2461->f1= Cyc_Toc_abs_ns; _temp2461->f2= Cyc_Toc__push_handler_sp;
_temp2461;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple0* _temp2460=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2460->f1= Cyc_Toc_abs_ns; _temp2460->f2= Cyc_Toc_setjmp_sp; _temp2460;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2459=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2459->f1=
Cyc_Toc_abs_ns; _temp2459->f2= Cyc_Toc__pop_handler_sp; _temp2459;}), 0); struct
Cyc_Absyn_Exp* freeregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0* _temp2458=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2458->f1= Cyc_Toc_abs_ns;
_temp2458->f2= Cyc_Toc__free_region_sp; _temp2458;}), 0); struct Cyc_Absyn_Exp*
newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0* _temp2457=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2457->f1= Cyc_Toc_abs_ns; _temp2457->f2=
Cyc_Toc__new_region_sp; _temp2457;}), 0); struct Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp(
rh_exp, 0); struct Cyc_Absyn_Exp* newregion_call= Cyc_Absyn_fncall_exp(
newregion_exp, 0, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv, x_var, x_exp),
_temp2352);{ struct Cyc_Absyn_Stmt* free_region_stmt= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
freeregion_exp,({ struct Cyc_List_List* _temp2456=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2456->hd=( void*) addr_rh_exp;
_temp2456->tl= 0; _temp2456;}), 0), 0); struct Cyc_Absyn_Stmt* tryandpop_stmt=
Cyc_Absyn_seq_stmt( _temp2352, Cyc_Absyn_seq_stmt( free_region_stmt, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0), 0); struct Cyc_Absyn_Stmt*
rethrow_stmt= Cyc_Absyn_seq_stmt( free_region_stmt, Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
e_exp), 0), 0); struct Cyc_Absyn_Stmt* if_stmt= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, e_exp, 0), tryandpop_stmt, rethrow_stmt, 0); struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ), Cyc_Absyn_fncall_exp(
setjmp_exp,({ struct Cyc_List_List* _temp2455=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2455->hd=( void*) Cyc_Absyn_structmember_exp(
h_exp, Cyc_Toc_handler_sp, 0); _temp2455->tl= 0; _temp2455;}), 0), 0); struct
Cyc_Absyn_Stmt* push_stmt= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pushhandler_exp,({ struct Cyc_List_List* _temp2454=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2454->hd=( void*) Cyc_Absyn_address_exp(
h_exp, 0); _temp2454->tl= 0; _temp2454;}), 0), 0);( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
push_stmt, Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, if_stmt, 0), 0), 0), 0), 0))->r)); return;}} _LL2235:;}} struct
_tuple17{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* args= f->args; for( 0; args != 0;
args=({ struct Cyc_List_List* _temp2464= args; if( _temp2464 == 0){ _throw(
Null_Exception);} _temp2464->tl;})){(*(( struct _tuple17*)({ struct Cyc_List_List*
_temp2465= args; if( _temp2465 == 0){ _throw( Null_Exception);} _temp2465->hd;}))).f3=
Cyc_Toc_typ_to_c((*(( struct _tuple17*)({ struct Cyc_List_List* _temp2466= args;
if( _temp2466 == 0){ _throw( Null_Exception);} _temp2466->hd;}))).f3);{ struct
_tuple0* x=({ struct _tuple0* _temp2467=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2467->f1=( void*) Cyc_Absyn_Loc_n; _temp2467->f2=(*((
struct _tuple17*)({ struct Cyc_List_List* _temp2468= args; if( _temp2468 == 0){
_throw( Null_Exception);} _temp2468->hd;}))).f1; _temp2467;}); nv= Cyc_Toc_add_varmap(
nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2469= s;
_LL2471: if(( int) _temp2469 == Cyc_Absyn_Abstract){ goto _LL2472;} else{ goto
_LL2473;} _LL2473: if(( int) _temp2469 == Cyc_Absyn_ExternC){ goto _LL2474;}
else{ goto _LL2475;} _LL2475: goto _LL2476; _LL2472: return( void*) Cyc_Absyn_Public;
_LL2474: return( void*) Cyc_Absyn_Extern; _LL2476: return s; _LL2470:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* n=( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp2489= s->name; if( _temp2489 == 0){ _throw( Null_Exception);}
_temp2489->v;}); if( Cyc_Toc_structs_so_far == 0){ Cyc_Toc_structs_so_far=({
struct Cyc_Core_Opt* _temp2477=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2477->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2477;});}{ int seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2488= Cyc_Toc_structs_so_far;
if( _temp2488 == 0){ _throw( Null_Exception);} _temp2488->v;}),(* n).f2); if(
dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2478= Cyc_Toc_structs_so_far; if( _temp2478 == 0){ _throw( Null_Exception);}
_temp2478->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2479= Cyc_Toc_structs_so_far;
if( _temp2479 == 0){ _throw( Null_Exception);} _temp2479->v;}),(* n).f2, s);});}
else{ if((( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Core_Opt* _temp2480= dopt;
if( _temp2480 == 0){ _throw( Null_Exception);} _temp2480->v;}))->fields == 0){(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2481= Cyc_Toc_structs_so_far;
if( _temp2481 == 0){ _throw( Null_Exception);} _temp2481->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2482= Cyc_Toc_structs_so_far; if( _temp2482 == 0){ _throw(
Null_Exception);} _temp2482->v;}),(* n).f2, s);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) s->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) s->sc))); s->tvs= 0; s->name=({
struct Cyc_Core_Opt* _temp2483=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2483->v=( void*) n; _temp2483;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2487= s->fields; if( _temp2487 == 0){
_throw( Null_Exception);} _temp2487->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2484= fields; if( _temp2484 == 0){ _throw( Null_Exception);}
_temp2484->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2485= fields; if( _temp2485 == 0){ _throw( Null_Exception);} _temp2485->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2486= fields; if( _temp2486 == 0){ _throw( Null_Exception);}
_temp2486->hd;}))->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* n=( struct _tuple0*)({ struct Cyc_Core_Opt* _temp2502= u->name; if(
_temp2502 == 0){ _throw( Null_Exception);} _temp2502->v;}); if( Cyc_Toc_unions_so_far
== 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2490=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2490->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2490;});}{ int seen_defn_before; struct Cyc_Core_Opt*
dopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2501= Cyc_Toc_unions_so_far; if( _temp2501 == 0){ _throw( Null_Exception);}
_temp2501->v;}),(* n).f2); if( dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2491= Cyc_Toc_unions_so_far; if( _temp2491 == 0){
_throw( Null_Exception);} _temp2491->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2492= Cyc_Toc_unions_so_far; if( _temp2492 == 0){ _throw( Null_Exception);}
_temp2492->v;}),(* n).f2, u);});} else{ if((( struct Cyc_Absyn_Uniondecl*)({
struct Cyc_Core_Opt* _temp2493= dopt; if( _temp2493 == 0){ _throw(
Null_Exception);} _temp2493->v;}))->fields == 0){( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2494= Cyc_Toc_unions_so_far; if( _temp2494 == 0){
_throw( Null_Exception);} _temp2494->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2495= Cyc_Toc_unions_so_far; if( _temp2495 == 0){ _throw( Null_Exception);}
_temp2495->v;}),(* n).f2, u);}); seen_defn_before= 0;} else{ seen_defn_before= 1;}}(
void*)( u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*)
u->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2496=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2496->v=( void*) n; _temp2496;}); if( u->fields != 0){ if(
seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2500= u->fields; if( _temp2500 == 0){
_throw( Null_Exception);} _temp2500->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2497= fields; if( _temp2497 == 0){ _throw( Null_Exception);}
_temp2497->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2498= fields; if( _temp2498 == 0){ _throw( Null_Exception);} _temp2498->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2499= fields; if( _temp2499 == 0){ _throw( Null_Exception);}
_temp2499->hd;}))->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* ed){ struct
_tuple0* n=( struct _tuple0*)({ struct Cyc_Core_Opt* _temp2543= ed->name; if(
_temp2543 == 0){ _throw( Null_Exception);} _temp2543->v;}); if( Cyc_Toc_tunions_so_far
== 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt* _temp2503=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2503->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2503;});}{ int seen_before; int seen_defn_before;
struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2542= Cyc_Toc_tunions_so_far; if( _temp2542 == 0){
_throw( Null_Exception);} _temp2542->v;}),(* n).f2); if( dopt == 0){ seen_before=
0; seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2504=
Cyc_Toc_tunions_so_far; if( _temp2504 == 0){ _throw( Null_Exception);} _temp2504->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2505= Cyc_Toc_tunions_so_far;
if( _temp2505 == 0){ _throw( Null_Exception);} _temp2505->v;}),(* n).f2, ed);});}
else{ seen_before= 1; if((( struct Cyc_Absyn_Tuniondecl*)({ struct Cyc_Core_Opt*
_temp2506= dopt; if( _temp2506 == 0){ _throw( Null_Exception);} _temp2506->v;}))->fields
== 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2507= Cyc_Toc_tunions_so_far; if( _temp2507 == 0){ _throw( Null_Exception);}
_temp2507->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2508= Cyc_Toc_tunions_so_far;
if( _temp2508 == 0){ _throw( Null_Exception);} _temp2508->v;}),(* n).f2, ed);});}
else{ seen_defn_before= 1;}} if( ! seen_before){ struct Cyc_Absyn_Typedefdecl*
tdef=({ struct Cyc_Absyn_Typedefdecl* _temp2512=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp2512->name= n; _temp2512->tvs=
0; _temp2512->defn=( void*) Cyc_Absyn_void_star_typ(); _temp2512;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2509=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2509->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp2510=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp2510[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp2511; _temp2511.tag= Cyc_Absyn_Typedef_d_tag;
_temp2511.f1= tdef; _temp2511;}); _temp2510;}), 0); _temp2509->tl= Cyc_Toc_result_decls;
_temp2509;});} if( ! seen_defn_before){ int tag_count= 0; int box_count= 0;
struct Cyc_List_List* fields= ed->fields == 0? 0:( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2541= ed->fields; if( _temp2541 == 0){ _throw(
Null_Exception);} _temp2541->v;}); for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp2513= fields; if( _temp2513 == 0){ _throw( Null_Exception);} _temp2513->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp2540= fields; if( _temp2540 == 0){ _throw( Null_Exception);} _temp2540->hd;});
if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
tag_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_uint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2514; _temp2514.q_const=
1; _temp2514.q_volatile= 0; _temp2514.q_restrict= 0; _temp2514;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2515=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2515->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2516=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2516[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2517; _temp2517.tag= Cyc_Absyn_Var_d_tag; _temp2517.f1= vd; _temp2517;});
_temp2516;}), 0); _temp2515->tl= Cyc_Toc_result_decls; _temp2515;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
box_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp2538=( char*)"_tag"; struct
_tagged_string _temp2539; _temp2539.curr= _temp2538; _temp2539.base= _temp2538;
_temp2539.last_plus_one= _temp2538 + 5; _temp2539;})), Cyc_Absyn_sint_t, eopt);
vd->tq=({ struct Cyc_Absyn_Tqual _temp2518; _temp2518.q_const= 1; _temp2518.q_volatile=
0; _temp2518.q_restrict= 0; _temp2518;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2519=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2519->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2520=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2520[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2521; _temp2521.tag= Cyc_Absyn_Var_d_tag;
_temp2521.f1= vd; _temp2521;}); _temp2520;}), 0); _temp2519->tl= Cyc_Toc_result_decls;
_temp2519;});{ struct Cyc_List_List* fs= 0; int i= 1;{ struct Cyc_List_List* ts=
f->typs; for( 0; ts != 0; ts=({ struct Cyc_List_List* _temp2522= ts; if(
_temp2522 == 0){ _throw( Null_Exception);} _temp2522->tl;}), i ++){ struct
_tagged_string* fname=( struct _tagged_string*)({ struct _tagged_string*
_temp2527=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp2527[ 0]= xprintf("f%d", i); _temp2527;}); struct Cyc_Absyn_Structfield* f=({
struct Cyc_Absyn_Structfield* _temp2524=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2524->name= fname;
_temp2524->tq=(*(( struct _tuple3*)({ struct Cyc_List_List* _temp2526= ts; if(
_temp2526 == 0){ _throw( Null_Exception);} _temp2526->hd;}))).f1; _temp2524->type=(
void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp2525= ts; if( _temp2525 == 0){ _throw( Null_Exception);} _temp2525->hd;}))).f2);
_temp2524->width= 0; _temp2524->attributes= 0; _temp2524;}); fs=({ struct Cyc_List_List*
_temp2523=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2523->hd=( void*) f; _temp2523->tl= fs; _temp2523;});}} fs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs); fs=({ struct Cyc_List_List*
_temp2528=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2528->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2529=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2529->name= Cyc_Toc_tag_sp;
_temp2529->tq= Cyc_Toc_mt_tq; _temp2529->type=( void*) Cyc_Absyn_sint_t;
_temp2529->width= 0; _temp2529->attributes= 0; _temp2529;}); _temp2528->tl= fs;
_temp2528;});{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp2533=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2533->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2533->name=({ struct Cyc_Core_Opt*
_temp2535=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2535->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2536=( char*)"_struct"; struct _tagged_string _temp2537; _temp2537.curr=
_temp2536; _temp2537.base= _temp2536; _temp2537.last_plus_one= _temp2536 + 8;
_temp2537;})); _temp2535;}); _temp2533->tvs= 0; _temp2533->fields=({ struct Cyc_Core_Opt*
_temp2534=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2534->v=( void*) fs; _temp2534;}); _temp2533->attributes= 0; _temp2533;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2530=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2530->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2531=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2531[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2532; _temp2532.tag= Cyc_Absyn_Struct_d_tag;
_temp2532.f1= sd; _temp2532;}); _temp2531;}), 0); _temp2530->tl= Cyc_Toc_result_decls;
_temp2530;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunions_so_far= 0;
static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_XTuniondecl* xd){ struct
_tuple0* n= xd->name; if( xd->fields == 0){ void* t= Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__xtunion_struct_sp), Cyc_Toc_mt_tq); struct Cyc_Absyn_Typedefdecl* td=({
struct Cyc_Absyn_Typedefdecl* _temp2547=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp2547->name= n; _temp2547->tvs=
0; _temp2547->defn=( void*) t; _temp2547;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2544=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2544->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp2545=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp2545[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp2546; _temp2546.tag= Cyc_Absyn_Typedef_d_tag;
_temp2546.f1= td; _temp2546;}); _temp2545;}), 0); _temp2544->tl= Cyc_Toc_result_decls;
_temp2544;});} else{ if( Cyc_Toc_xtunions_so_far == 0){ Cyc_Toc_xtunions_so_far=({
struct Cyc_Core_Opt* _temp2548=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2548->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2548;});}{ struct Cyc_List_List* fs= xd->fields; for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp2549= fs; if( _temp2549 == 0){ _throw( Null_Exception);}
_temp2549->tl;})){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp2580= fs; if( _temp2580 == 0){ _throw( Null_Exception);}
_temp2580->hd;}); struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2579=* fn;(
unsigned int)( _temp2579.last_plus_one - _temp2579.curr);}), 0); struct Cyc_Absyn_ArrayType_struct*
tag_typ=({ struct Cyc_Absyn_ArrayType_struct* _temp2577=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2577[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2578; _temp2578.tag= Cyc_Absyn_ArrayType_tag;
_temp2578.f1=( void*) Cyc_Absyn_uchar_t; _temp2578.f2= Cyc_Toc_mt_tq; _temp2578.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp2578;}); _temp2577;}); struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) xd->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(* fn, 0);}{ struct Cyc_Absyn_Vardecl*
tag_decl= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag( f->name,( struct
_tagged_string)({ char* _temp2575=( char*)"_tag"; struct _tagged_string
_temp2576; _temp2576.curr= _temp2575; _temp2576.base= _temp2575; _temp2576.last_plus_one=
_temp2575 + 5; _temp2576;})),( void*) tag_typ, initopt);( void*)( tag_decl->sc=(
void*)(( void*) xd->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2550=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2550->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2551=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2551[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2552; _temp2552.tag= Cyc_Absyn_Var_d_tag;
_temp2552.f1= tag_decl; _temp2552;}); _temp2551;}), 0); _temp2550->tl= Cyc_Toc_result_decls;
_temp2550;});{ int seen_before; int seen_defn_before; struct Cyc_Core_Opt* dopt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2574=
Cyc_Toc_xtunions_so_far; if( _temp2574 == 0){ _throw( Null_Exception);}
_temp2574->v;}), fn); if( dopt == 0){ seen_before= 0; seen_defn_before= 0;(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2553= Cyc_Toc_xtunions_so_far;
if( _temp2553 == 0){ _throw( Null_Exception);} _temp2553->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_XTuniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2554= Cyc_Toc_xtunions_so_far; if( _temp2554 == 0){ _throw(
Null_Exception);} _temp2554->v;}), fn, xd);});} else{ seen_before= 1; if(((
struct Cyc_Absyn_XTuniondecl*)({ struct Cyc_Core_Opt* _temp2555= dopt; if(
_temp2555 == 0){ _throw( Null_Exception);} _temp2555->v;}))->fields == 0){(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2556= Cyc_Toc_xtunions_so_far;
if( _temp2556 == 0){ _throw( Null_Exception);} _temp2556->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_XTuniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2557= Cyc_Toc_xtunions_so_far; if( _temp2557 == 0){ _throw(
Null_Exception);} _temp2557->v;}), fn, xd);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}} if( ! seen_before){ struct Cyc_List_List* fields= 0; int
i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp2558= ts; if( _temp2558 == 0){ _throw( Null_Exception);} _temp2558->tl;}),
i ++){ struct _tagged_string* field_n=( struct _tagged_string*)({ struct
_tagged_string* _temp2563=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2563[ 0]= xprintf("f%d", i); _temp2563;}); struct Cyc_Absyn_Structfield*
newf=({ struct Cyc_Absyn_Structfield* _temp2560=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2560->name= field_n;
_temp2560->tq=(*(( struct _tuple3*)({ struct Cyc_List_List* _temp2562= ts; if(
_temp2562 == 0){ _throw( Null_Exception);} _temp2562->hd;}))).f1; _temp2560->type=(
void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*)({ struct Cyc_List_List*
_temp2561= ts; if( _temp2561 == 0){ _throw( Null_Exception);} _temp2561->hd;}))).f2);
_temp2560->width= 0; _temp2560->attributes= 0; _temp2560;}); fields=({ struct
Cyc_List_List* _temp2559=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2559->hd=( void*) newf; _temp2559->tl= fields; _temp2559;});}} fields=({
struct Cyc_List_List* _temp2564=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2564->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2565=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2565->name= Cyc_Toc_tag_sp; _temp2565->tq= Cyc_Toc_mt_tq; _temp2565->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2565->width=
0; _temp2565->attributes= 0; _temp2565;}); _temp2564->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2564;});{ struct Cyc_Absyn_Structdecl*
strct_decl=({ struct Cyc_Absyn_Structdecl* _temp2569=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2569->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2569->name=({ struct Cyc_Core_Opt* _temp2571=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2571->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({ char* _temp2572=(
char*)"_struct"; struct _tagged_string _temp2573; _temp2573.curr= _temp2572;
_temp2573.base= _temp2572; _temp2573.last_plus_one= _temp2572 + 8; _temp2573;}));
_temp2571;}); _temp2569->tvs= 0; _temp2569->fields=({ struct Cyc_Core_Opt*
_temp2570=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2570->v=( void*) fields; _temp2570;}); _temp2569->attributes= 0; _temp2569;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2566=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2566->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2567=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2567[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2568; _temp2568.tag= Cyc_Absyn_Struct_d_tag;
_temp2568.f1= strct_decl; _temp2568;}); _temp2567;}), 0); _temp2566->tl= Cyc_Toc_result_decls;
_temp2566;});}}}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* n= ed->name;( void*)( ed->sc=(
void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) ed->sc ==( void*)
Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public:
Cyc_Toc_scope_to_c(( void*) ed->sc)));{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp2581= fs; if( _temp2581 == 0){
_throw( Null_Exception);} _temp2581->tl;})){ if((( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2582= fs; if( _temp2582 == 0){ _throw( Null_Exception);}
_temp2582->hd;}))->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2585=(( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2584= fs; if( _temp2584 == 0){ _throw( Null_Exception);}
_temp2584->hd;}))->tag; if( _temp2585 == 0){ _throw( Null_Exception);} _temp2585;}));((
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp2583= fs; if(
_temp2583 == 0){ _throw( Null_Exception);} _temp2583->hd;}))->tag=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)({
struct Cyc_Core_Opt* _temp2604= topt; if( _temp2604 == 0){ _throw(
Null_Exception);} _temp2604->v;}); Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x=
Cyc_Toc_temp_var(); struct _tagged_string* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_string* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();
struct Cyc_Absyn_Stmt* _temp2588; struct Cyc_List_List* _temp2590; struct Cyc_Toc_Env*
_temp2592; struct _tuple13 _temp2586= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL2593: _temp2592=
_temp2586.f1; goto _LL2591; _LL2591: _temp2590= _temp2586.f2; goto _LL2589;
_LL2589: _temp2588= _temp2586.f3; goto _LL2587; _LL2587: Cyc_Toc_stmt_to_c(
_temp2592, s);{ struct Cyc_Absyn_Stmt* succ_stmt= Cyc_Absyn_label_stmt( succ_lab,
s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2588, succ_stmt, 0), 0);} else{
struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({ struct
_tuple0* _temp2594=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2594->f1= Cyc_Toc_abs_ns; _temp2594->f2= Cyc_Toc_Match_Exception_sp;
_temp2594;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2588, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp2590 != 0; _temp2590=({ struct
Cyc_List_List* _temp2595= _temp2590; if( _temp2595 == 0){ _throw( Null_Exception);}
_temp2595->tl;})){ struct _tuple14 _temp2599; void* _temp2600; struct _tuple0*
_temp2602; struct _tuple14* _temp2597=( struct _tuple14*)({ struct Cyc_List_List*
_temp2596= _temp2590; if( _temp2596 == 0){ _throw( Null_Exception);} _temp2596->hd;});
_temp2599=* _temp2597; _LL2603: _temp2602= _temp2599.f1; goto _LL2601; _LL2601:
_temp2600= _temp2599.f2; goto _LL2598; _LL2598: s= Cyc_Absyn_declare_stmt(
_temp2602, _temp2600, 0, s, 0);} return s;}}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=({
struct Cyc_List_List* _temp2605= ds; if( _temp2605 == 0){ _throw( Null_Exception);}
_temp2605->tl;})){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct
Cyc_List_List* _temp2683= ds; if( _temp2683 == 0){ _throw( Null_Exception);}
_temp2683->hd;}); void* _temp2606=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2632; struct Cyc_Absyn_Fndecl* _temp2634; int _temp2636; struct Cyc_Absyn_Exp*
_temp2638; struct Cyc_Core_Opt* _temp2640; struct Cyc_Core_Opt* _temp2642;
struct Cyc_Absyn_Pat* _temp2644; struct Cyc_Absyn_Structdecl* _temp2646; struct
Cyc_Absyn_Uniondecl* _temp2648; struct Cyc_Absyn_Tuniondecl* _temp2650; struct
Cyc_Absyn_XTuniondecl* _temp2652; struct Cyc_Absyn_Enumdecl* _temp2654; struct
Cyc_Absyn_Typedefdecl* _temp2656; struct Cyc_List_List* _temp2658; struct
_tagged_string* _temp2660; struct Cyc_List_List* _temp2662; struct _tuple0*
_temp2664; struct Cyc_List_List* _temp2666; _LL2608: if((( struct _tunion_struct*)
_temp2606)->tag == Cyc_Absyn_Var_d_tag){ _LL2633: _temp2632=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp2606)->f1; goto _LL2609;} else{ goto
_LL2610;} _LL2610: if((( struct _tunion_struct*) _temp2606)->tag == Cyc_Absyn_Fn_d_tag){
_LL2635: _temp2634=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp2606)->f1; goto _LL2611;} else{ goto _LL2612;} _LL2612: if((( struct
_tunion_struct*) _temp2606)->tag == Cyc_Absyn_Let_d_tag){ _LL2645: _temp2644=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp2606)->f1; goto
_LL2643; _LL2643: _temp2642=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2606)->f2; goto _LL2641; _LL2641: _temp2640=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2606)->f3; goto _LL2639; _LL2639: _temp2638=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp2606)->f4; goto
_LL2637; _LL2637: _temp2636=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp2606)->f5;
goto _LL2613;} else{ goto _LL2614;} _LL2614: if((( struct _tunion_struct*)
_temp2606)->tag == Cyc_Absyn_Struct_d_tag){ _LL2647: _temp2646=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp2606)->f1; goto _LL2615;} else{ goto
_LL2616;} _LL2616: if((( struct _tunion_struct*) _temp2606)->tag == Cyc_Absyn_Union_d_tag){
_LL2649: _temp2648=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp2606)->f1; goto _LL2617;} else{ goto _LL2618;} _LL2618: if((( struct
_tunion_struct*) _temp2606)->tag == Cyc_Absyn_Tunion_d_tag){ _LL2651: _temp2650=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp2606)->f1;
goto _LL2619;} else{ goto _LL2620;} _LL2620: if((( struct _tunion_struct*)
_temp2606)->tag == Cyc_Absyn_XTunion_d_tag){ _LL2653: _temp2652=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp2606)->f1; goto _LL2621;} else{ goto
_LL2622;} _LL2622: if((( struct _tunion_struct*) _temp2606)->tag == Cyc_Absyn_Enum_d_tag){
_LL2655: _temp2654=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp2606)->f1; goto _LL2623;} else{ goto _LL2624;} _LL2624: if((( struct
_tunion_struct*) _temp2606)->tag == Cyc_Absyn_Typedef_d_tag){ _LL2657: _temp2656=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp2606)->f1;
goto _LL2625;} else{ goto _LL2626;} _LL2626: if((( struct _tunion_struct*)
_temp2606)->tag == Cyc_Absyn_Namespace_d_tag){ _LL2661: _temp2660=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp2606)->f1; goto
_LL2659; _LL2659: _temp2658=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2606)->f2; goto _LL2627;} else{ goto _LL2628;} _LL2628: if((( struct
_tunion_struct*) _temp2606)->tag == Cyc_Absyn_Using_d_tag){ _LL2665: _temp2664=(
struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp2606)->f1; goto
_LL2663; _LL2663: _temp2662=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2606)->f2; goto _LL2629;} else{ goto _LL2630;} _LL2630: if((( struct
_tunion_struct*) _temp2606)->tag == Cyc_Absyn_ExternC_d_tag){ _LL2667: _temp2666=(
struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*) _temp2606)->f1;
goto _LL2631;} else{ goto _LL2607;} _LL2609: { struct _tuple0* c_name= _temp2632->name;
if(( void*) _temp2632->sc ==( void*) Cyc_Absyn_ExternC){ c_name=({ struct
_tuple0* _temp2668=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2668->f1= Cyc_Toc_abs_ns; _temp2668->f2=(* c_name).f2; _temp2668;});} if(
_temp2632->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2669= _temp2632->initializer; if( _temp2669 == 0){
_throw( Null_Exception);} _temp2669;}));} if( ! nv->toplevel){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2670=(
char*)"decls_to_c: not at toplevel!"; struct _tagged_string _temp2671; _temp2671.curr=
_temp2670; _temp2671.base= _temp2670; _temp2671.last_plus_one= _temp2670 + 29;
_temp2671;}));} nv= Cyc_Toc_add_varmap( nv, _temp2632->name, Cyc_Absyn_varb_exp(
c_name,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp2672=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp2672[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp2673; _temp2673.tag= Cyc_Absyn_Global_b_tag;
_temp2673.f1= _temp2632; _temp2673;}); _temp2672;}), 0)); _temp2632->name=
c_name;( void*)( _temp2632->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2632->sc));(
void*)( _temp2632->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2632->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2674=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2674->hd=( void*) d; _temp2674->tl=
Cyc_Toc_result_decls; _temp2674;}); goto _LL2607;} _LL2611: { struct _tuple0*
c_name= _temp2634->name; if(( void*) _temp2634->sc ==( void*) Cyc_Absyn_ExternC){
c_name=({ struct _tuple0* _temp2675=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp2675->f1= Cyc_Toc_abs_ns; _temp2675->f2=(* c_name).f2; _temp2675;});}
nv= Cyc_Toc_add_varmap( nv, _temp2634->name, Cyc_Absyn_var_exp( c_name, 0));
_temp2634->name= c_name; Cyc_Toc_fndecl_to_c( nv, _temp2634); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2676=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2676->hd=( void*) d; _temp2676->tl= Cyc_Toc_result_decls;
_temp2676;}); goto _LL2607;} _LL2613:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2677=( char*)"letdecl"; struct
_tagged_string _temp2678; _temp2678.curr= _temp2677; _temp2678.base= _temp2677;
_temp2678.last_plus_one= _temp2677 + 8; _temp2678;})); goto _LL2607; _LL2615:
Cyc_Toc_structdecl_to_c( _temp2646); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2679=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2679->hd=( void*) d; _temp2679->tl= Cyc_Toc_result_decls; _temp2679;});
goto _LL2607; _LL2617: Cyc_Toc_uniondecl_to_c( _temp2648); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2680=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2680->hd=( void*) d; _temp2680->tl= Cyc_Toc_result_decls;
_temp2680;}); goto _LL2607; _LL2619: Cyc_Toc_tuniondecl_to_c( _temp2650); goto
_LL2607; _LL2621: Cyc_Toc_xtuniondecl_to_c( _temp2652); goto _LL2607; _LL2623:
Cyc_Toc_enumdecl_to_c( nv, _temp2654); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2681=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2681->hd=( void*) d; _temp2681->tl= Cyc_Toc_result_decls; _temp2681;});
goto _LL2607; _LL2625: _temp2656->name= _temp2656->name; _temp2656->tvs= 0;(
void*)( _temp2656->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2656->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2682=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2682->hd=( void*) d; _temp2682->tl=
Cyc_Toc_result_decls; _temp2682;}); goto _LL2607; _LL2627: _temp2662= _temp2658;
goto _LL2629; _LL2629: _temp2666= _temp2662; goto _LL2631; _LL2631: nv= Cyc_Toc_decls_to_c(
nv, _temp2666, top); goto _LL2607; _LL2607:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunions_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List*
ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds, 1); return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Toc_result_decls);}