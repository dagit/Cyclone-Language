#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct _tuple4{ struct Cyc_Absyn_Switch_clause*
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct _tuple5{ void*
f1; int f2; } ; struct _tuple6{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t;
typedef unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef
char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int;
typedef unsigned int Cyc_u_long; typedef unsigned short Cyc_ushort; typedef
unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t;
struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t;
typedef char* Cyc_caddr_t; typedef unsigned int Cyc_ino_t; typedef unsigned int
Cyc_vm_offset_t; typedef unsigned int Cyc_vm_size_t; typedef char Cyc_int8_t;
typedef char Cyc_u_int8_t; typedef short Cyc_int16_t; typedef unsigned short Cyc_u_int16_t;
typedef int Cyc_int32_t; typedef unsigned int Cyc_u_int32_t; typedef long long
Cyc_int64_t; typedef unsigned long long Cyc_u_int64_t; typedef int Cyc_register_t;
typedef short Cyc_dev_t; typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t;
typedef unsigned short Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t;
typedef int Cyc_ssize_t; typedef char* Cyc_addr_t; typedef int Cyc_mode_t;
typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[
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
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t;
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError_tag[
14u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{ char*
tag; } ; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern struct
Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void* data);
extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern struct
Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key); struct
Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set*
Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* comp)( void*, void*)); extern int Cyc_Set_member( struct
Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_SlowDict_Dict;
typedef struct Cyc_SlowDict_Dict* Cyc_SlowDict_hdict_t; typedef struct Cyc_SlowDict_Dict*
Cyc_SlowDict_dict_t; extern char Cyc_SlowDict_Present_tag[ 8u]; extern char Cyc_SlowDict_Absent_tag[
7u]; extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_empty( int(* comp)( void*,
void*)); extern int Cyc_SlowDict_is_empty( struct Cyc_SlowDict_Dict* d); extern
struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert( struct Cyc_SlowDict_Dict* d, void*
key, void* data); extern struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt( struct
Cyc_SlowDict_Dict* d, void* key); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete(
struct Cyc_SlowDict_Dict* d, void* key); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
static const unsigned int Cyc_Position_Lex= 0; static const unsigned int Cyc_Position_Parse=
1; static const unsigned int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct
_tagged_string*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; typedef struct _tagged_string* Cyc_Absyn_field_name_t;
typedef struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string*
Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t;
typedef struct _tuple0* Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
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
char* tag; } ; extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct
Cyc_List_List*); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x);
extern struct _tuple0* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_XTuniondecl*
Cyc_Absyn_exn_xed; extern void* Cyc_Absyn_exn_typ; extern struct _tagged_string
Cyc_Absynpp_typ2string( void*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0*); typedef void* Cyc_Tcenv_Resolved; static const int Cyc_Tcenv_VarRes_tag=
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
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); extern struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(); extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Fndecl*); extern struct Cyc_List_List*
Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tagged_string*, struct Cyc_List_List*); extern void* Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern struct Cyc_Absyn_Uniondecl**
Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple0*); extern struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple0*); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
continue_dest); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct
Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct
Cyc_Tcenv_Tenv* te, struct _tuple3* pat_typ, struct Cyc_Absyn_Switch_clause*
clause); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv*, void*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te);
extern int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv* te); extern void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern
void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_goto( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*, struct _tagged_string*, struct Cyc_Absyn_Stmt**); extern
struct _tuple4* Cyc_Tcenv_process_fallthru( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***); extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label( struct Cyc_Tcenv_Tenv*, struct _tagged_string*, struct Cyc_Absyn_Stmt*);
extern int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Tvar* name); extern void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void* rgn); extern void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff); extern
char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; }
; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern
int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void* e2); extern int
Cyc_Tcutil_region_in_effect( int constrain, void* r, void* e); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k); char Cyc_Tcenv_Env_error_tag[ 10u]="Env_error";
struct Cyc_Tcenv_Env_error_struct{ char* tag; } ; void* Cyc_Tcenv_env_err(
struct _tagged_string msg){({ struct _tagged_string _temp0= msg; fprintf( Cyc_Stdio_stderr,"Error: %.*s\n",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Tcenv_Env_error_struct* _temp1=( struct Cyc_Tcenv_Env_error_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Env_error_struct)); _temp1[ 0]=({ struct Cyc_Tcenv_Env_error_struct
_temp2; _temp2.tag= Cyc_Tcenv_Env_error_tag; _temp2;}); _temp1;}));} struct Cyc_Tcenv_Tenv;
struct Cyc_Tcenv_Genv; struct Cyc_Tcenv_CtrlEnv{ void* continue_stmt; void*
break_stmt; struct _tuple4* fallthru_clause; void* next_stmt; int try_depth; } ;
typedef struct Cyc_Tcenv_CtrlEnv* Cyc_Tcenv_ctrl_env_t; struct Cyc_Tcenv_SharedFenv{
void* return_typ; struct Cyc_Dict_Dict* seen_labels; struct Cyc_SlowDict_Dict*
needed_labels; } ; struct Cyc_Tcenv_Fenv{ struct Cyc_Tcenv_SharedFenv* shared;
struct Cyc_List_List* type_vars; struct Cyc_Dict_Dict* locals; struct Cyc_Absyn_Stmt*
encloser; struct Cyc_Tcenv_CtrlEnv* ctrl_env; void* capability; void* curr_rgn;
} ; struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(){ Cyc_Tcutil_empty_var_set=({
struct Cyc_Core_Opt* _temp3=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp);
_temp3;});{ struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_empty_genv(); struct Cyc_List_List*
top_ns= 0; Cyc_Absyn_exn_xed=({ struct Cyc_Absyn_XTuniondecl* _temp4=( struct
Cyc_Absyn_XTuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl));
_temp4->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp4->name= Cyc_Absyn_exn_name;
_temp4->fields= 0; _temp4;}); Cyc_Absyn_exn_typ=( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp5=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp5[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp6; _temp6.tag= Cyc_Absyn_XTunionType_tag;
_temp6.f1=({ struct Cyc_Absyn_XTunionInfo _temp7; _temp7.name= Cyc_Absyn_exn_name;
_temp7.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp7.xtud=( struct Cyc_Absyn_XTuniondecl*)
Cyc_Absyn_exn_xed; _temp7;}); _temp6;}); _temp5;}); ge->xtuniondecls=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_XTuniondecl** data)) Cyc_Dict_insert)( ge->xtuniondecls,(* Cyc_Absyn_exn_name).f2,&
Cyc_Absyn_exn_xed);{ struct Cyc_Dict_Dict* ae=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data))
Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp), top_ns, ge);
return({ struct Cyc_Tcenv_Tenv* _temp8=( struct Cyc_Tcenv_Tenv*) GC_malloc(
sizeof( struct Cyc_Tcenv_Tenv)); _temp8->ns= top_ns; _temp8->ae= ae; _temp8->le=
0; _temp8;});}}} static struct Cyc_Tcenv_Genv* Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct
_tagged_string* n, struct Cyc_List_List* ns){ return(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
Cyc_Tcenv_resolve_namespace( te, loc, n, ns));} static struct Cyc_List_List* Cyc_Tcenv_outer_namespace(
struct Cyc_List_List* ns){ if( ns == 0){ return(( struct Cyc_List_List*(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp9=( char*)"Tcenv::outer_namespace"; struct _tagged_string _temp10; _temp10.curr=
_temp9; _temp10.base= _temp9; _temp10.last_plus_one= _temp9 + 23; _temp10;}));}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)((((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( ns))->tl);}
static void Cyc_Tcenv_check_repeat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct
_tagged_string* v, struct Cyc_List_List* nss){ for( 0; nss != 0; nss= nss->tl){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)
nss->hd); struct _handler_cons _temp11; _push_handler(& _temp11);{ struct
_xtunion_struct* _temp12=( struct _xtunion_struct*) setjmp( _temp11.handler);
if( ! _temp12){ lookup( ge2, v); Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp13=* v; xprintf("%.*s is ambiguous", _temp13.last_plus_one - _temp13.curr,
_temp13.curr);}));; _pop_handler();} else{ struct _xtunion_struct* _temp15=
_temp12; _LL17: if( _temp15->tag == Cyc_Dict_Absent_tag){ goto _LL18;} else{
goto _LL19;} _LL19: goto _LL20; _LL18: goto _LL16; _LL20:( void) _throw( _temp15);
_LL16:;}}} return;} static void* Cyc_Tcenv_scoped_lookup( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_string*), struct _tagged_string* v){ struct Cyc_List_List* cns=
te->ns; void* result; while( 1) { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
cns);{ struct _handler_cons _temp21; _push_handler(& _temp21);{ struct
_xtunion_struct* _temp22=( struct _xtunion_struct*) setjmp( _temp21.handler);
if( ! _temp22){ result= lookup( ge, v);(( void(*)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss))
Cyc_Tcenv_check_repeat)( te, loc, lookup, v, ge->availables);{ void* _temp23=
result; _npop_handler( 0u); return _temp23;}; _pop_handler();} else{ struct
_xtunion_struct* _temp25= _temp22; _LL27: if( _temp25->tag == Cyc_Dict_Absent_tag){
goto _LL28;} else{ goto _LL29;} _LL29: goto _LL30; _LL28: goto _LL26; _LL30:(
void) _throw( _temp25); _LL26:;}}}{ struct Cyc_List_List* nss= ge->availables;
for( 0; nss != 0; nss= nss->tl){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,(
struct Cyc_List_List*) nss->hd); struct _handler_cons _temp31; _push_handler(&
_temp31);{ struct _xtunion_struct* _temp32=( struct _xtunion_struct*) setjmp(
_temp31.handler); if( ! _temp32){ result= lookup( ge2, v);(( void(*)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss))
Cyc_Tcenv_check_repeat)( te, loc, lookup, v, nss->tl);{ void* _temp33= result;
_npop_handler( 0u); return _temp33;}; _pop_handler();} else{ struct
_xtunion_struct* _temp35= _temp32; _LL37: if( _temp35->tag == Cyc_Dict_Absent_tag){
goto _LL38;} else{ goto _LL39;} _LL39: goto _LL40; _LL38: goto _LL36; _LL40:(
void) _throw( _temp35); _LL36:;}}}} if( cns == 0){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Dict_Absent_struct* _temp41=( struct Cyc_Dict_Absent_struct*)
GC_malloc( sizeof( struct Cyc_Dict_Absent_struct)); _temp41[ 0]=({ struct Cyc_Dict_Absent_struct
_temp42; _temp42.tag= Cyc_Dict_Absent_tag; _temp42;}); _temp41;}));} cns= Cyc_Tcenv_outer_namespace(
cns);}} static void* Cyc_Tcenv_lookup_ordinary_global_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_string* v){ struct _tuple5* ans=(( struct _tuple5*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries,
v);(* ans).f2= 1; return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple0* q){
struct _tuple0 _temp45; struct _tagged_string* _temp46; void* _temp48; struct
_tuple0* _temp43= q; _temp45=* _temp43; _LL49: _temp48= _temp45.f1; goto _LL47;
_LL47: _temp46= _temp45.f2; goto _LL44; _LL44: { void* _temp50= _temp48; struct
Cyc_List_List* _temp60; struct Cyc_List_List* _temp62; struct Cyc_List_List
_temp64; struct Cyc_List_List* _temp65; struct _tagged_string* _temp67; struct
Cyc_List_List* _temp69; _LL52: if(( int) _temp50 == Cyc_Absyn_Loc_n){ goto _LL53;}
else{ goto _LL54;} _LL54: if(( unsigned int) _temp50 > 1u?(( struct
_tunion_struct*) _temp50)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL61: _temp60=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp50)->f1; if(
_temp60 == 0){ goto _LL55;} else{ goto _LL56;}} else{ goto _LL56;} _LL56: if((
unsigned int) _temp50 > 1u?(( struct _tunion_struct*) _temp50)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL63: _temp62=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp50)->f1; if( _temp62 == 0){ goto _LL58;} else{ _temp64=* _temp62; _LL68:
_temp67=( struct _tagged_string*) _temp64.hd; goto _LL66; _LL66: _temp65=(
struct Cyc_List_List*) _temp64.tl; goto _LL57;}} else{ goto _LL58;} _LL58: if((
unsigned int) _temp50 > 1u?(( struct _tunion_struct*) _temp50)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL70: _temp69=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp50)->f1; goto _LL59;} else{ goto _LL51;} _LL53: goto _LL55; _LL55: return((
void*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_ordinary_global_f,
_temp46); _LL57: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace( te,
loc, _temp67, _temp65); return Cyc_Tcenv_lookup_ordinary_global_f( ge, _temp46);}
_LL59: return Cyc_Tcenv_lookup_ordinary_global_f((( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp69), _temp46); _LL51:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct
_tagged_string* n, struct Cyc_List_List* ns){ struct Cyc_List_List* cns= te->ns;
struct Cyc_List_List* possible_results= 0; while( 1) { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, cns);{ struct Cyc_List_List* nss= ge->availables; for(
0; nss != 0; nss= nss->tl){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,(
struct Cyc_List_List*) nss->hd); if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)( ge2->namespaces, n)){ possible_results=({
struct Cyc_List_List* _temp71=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp71->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( cns,({ struct Cyc_List_List*
_temp72=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp72->hd=( void*) n; _temp72->tl= ns; _temp72;})); _temp71->tl=
possible_results; _temp71;});}}} if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)( ge->namespaces, n)){ possible_results=({
struct Cyc_List_List* _temp73=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp73->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( cns,({ struct Cyc_List_List*
_temp74=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp74->hd=( void*) n; _temp74->tl= ns; _temp74;})); _temp73->tl=
possible_results; _temp73;});} if( possible_results != 0){ if( possible_results->tl
!= 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp75=* n; xprintf("%.*s is ambiguous",
_temp75.last_plus_one - _temp75.curr, _temp75.curr);}));} return( struct Cyc_List_List*)
possible_results->hd;} if( cns == 0){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Dict_Absent_struct* _temp76=( struct Cyc_Dict_Absent_struct*)
GC_malloc( sizeof( struct Cyc_Dict_Absent_struct)); _temp76[ 0]=({ struct Cyc_Dict_Absent_struct
_temp77; _temp77.tag= Cyc_Dict_Absent_tag; _temp77;}); _temp76;}));} cns= Cyc_Tcenv_outer_namespace(
cns);}} static struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
v);} struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp80;
struct _tagged_string* _temp81; void* _temp83; struct _tuple0* _temp78= q;
_temp80=* _temp78; _LL84: _temp83= _temp80.f1; goto _LL82; _LL82: _temp81=
_temp80.f2; goto _LL79; _LL79: { void* _temp85= _temp83; struct Cyc_List_List*
_temp95; struct Cyc_List_List* _temp97; struct Cyc_List_List* _temp99; struct
Cyc_List_List _temp101; struct Cyc_List_List* _temp102; struct _tagged_string*
_temp104; _LL87: if(( int) _temp85 == Cyc_Absyn_Loc_n){ goto _LL88;} else{ goto
_LL89;} _LL89: if(( unsigned int) _temp85 > 1u?(( struct _tunion_struct*)
_temp85)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL96: _temp95=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp85)->f1; if( _temp95 == 0){ goto _LL90;}
else{ goto _LL91;}} else{ goto _LL91;} _LL91: if(( unsigned int) _temp85 > 1u?((
struct _tunion_struct*) _temp85)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL98: _temp97=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp85)->f1; goto
_LL92;} else{ goto _LL93;} _LL93: if(( unsigned int) _temp85 > 1u?(( struct
_tunion_struct*) _temp85)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL100: _temp99=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp85)->f1; if(
_temp99 == 0){ goto _LL86;} else{ _temp101=* _temp99; _LL105: _temp104=( struct
_tagged_string*) _temp101.hd; goto _LL103; _LL103: _temp102=( struct Cyc_List_List*)
_temp101.tl; goto _LL94;}} else{ goto _LL86;} _LL88: goto _LL90; _LL90: return((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_structdecl_f, _temp81); _LL92: { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, _temp97); return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
_temp81);} _LL94: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace( te,
loc, _temp104, _temp102); return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls, _temp81);}
_LL86:;}} static struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->uniondecls,
v);} struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp108; struct _tagged_string* _temp109; void* _temp111; struct _tuple0*
_temp106= q; _temp108=* _temp106; _LL112: _temp111= _temp108.f1; goto _LL110;
_LL110: _temp109= _temp108.f2; goto _LL107; _LL107: { void* _temp113= _temp111;
struct Cyc_List_List* _temp123; struct Cyc_List_List* _temp125; struct Cyc_List_List*
_temp127; struct Cyc_List_List _temp129; struct Cyc_List_List* _temp130; struct
_tagged_string* _temp132; _LL115: if(( int) _temp113 == Cyc_Absyn_Loc_n){ goto
_LL116;} else{ goto _LL117;} _LL117: if(( unsigned int) _temp113 > 1u?(( struct
_tunion_struct*) _temp113)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL124: _temp123=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp113)->f1; if(
_temp123 == 0){ goto _LL118;} else{ goto _LL119;}} else{ goto _LL119;} _LL119:
if(( unsigned int) _temp113 > 1u?(( struct _tunion_struct*) _temp113)->tag ==
Cyc_Absyn_Abs_n_tag: 0){ _LL126: _temp125=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp113)->f1; goto _LL120;} else{ goto _LL121;} _LL121: if(( unsigned int)
_temp113 > 1u?(( struct _tunion_struct*) _temp113)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL128: _temp127=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp113)->f1; if( _temp127 == 0){ goto _LL114;} else{ _temp129=* _temp127;
_LL133: _temp132=( struct _tagged_string*) _temp129.hd; goto _LL131; _LL131:
_temp130=( struct Cyc_List_List*) _temp129.tl; goto _LL122;}} else{ goto _LL114;}
_LL116: goto _LL118; _LL118: return(( struct Cyc_Absyn_Uniondecl**(*)( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_uniondecl_f, _temp109);
_LL120: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp125); return((
struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->uniondecls, _temp109);} _LL122: { struct Cyc_Tcenv_Genv*
ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp132, _temp130); return(( struct
Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( ge->uniondecls, _temp109);} _LL114:;}} static struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl_f( struct Cyc_Tcenv_Genv* ge, struct _tagged_string*
v){ return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls, v);} struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple0* q){ struct _tuple0 _temp136; struct _tagged_string*
_temp137; void* _temp139; struct _tuple0* _temp134= q; _temp136=* _temp134;
_LL140: _temp139= _temp136.f1; goto _LL138; _LL138: _temp137= _temp136.f2; goto
_LL135; _LL135: { void* _temp141= _temp139; struct Cyc_List_List* _temp151;
struct Cyc_List_List* _temp153; struct Cyc_List_List _temp155; struct Cyc_List_List*
_temp156; struct _tagged_string* _temp158; struct Cyc_List_List* _temp160;
_LL143: if(( int) _temp141 == Cyc_Absyn_Loc_n){ goto _LL144;} else{ goto _LL145;}
_LL145: if(( unsigned int) _temp141 > 1u?(( struct _tunion_struct*) _temp141)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL152: _temp151=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp141)->f1; if( _temp151 == 0){ goto _LL146;} else{
goto _LL147;}} else{ goto _LL147;} _LL147: if(( unsigned int) _temp141 > 1u?((
struct _tunion_struct*) _temp141)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL154:
_temp153=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp141)->f1;
if( _temp153 == 0){ goto _LL149;} else{ _temp155=* _temp153; _LL159: _temp158=(
struct _tagged_string*) _temp155.hd; goto _LL157; _LL157: _temp156=( struct Cyc_List_List*)
_temp155.tl; goto _LL148;}} else{ goto _LL149;} _LL149: if(( unsigned int)
_temp141 > 1u?(( struct _tunion_struct*) _temp141)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL161: _temp160=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp141)->f1; goto _LL150;} else{ goto _LL142;} _LL144: goto _LL146; _LL146:
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Tuniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_tuniondecl_f, _temp137); _LL148: { struct Cyc_Tcenv_Genv* ge=
Cyc_Tcenv_lookup_namespace( te, loc, _temp158, _temp156); return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp137);} _LL150: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp160); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp137);}
_LL142:;}} static struct Cyc_Absyn_XTuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_XTuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->xtuniondecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp164; struct _tagged_string* _temp165; void* _temp167; struct _tuple0*
_temp162= q; _temp164=* _temp162; _LL168: _temp167= _temp164.f1; goto _LL166;
_LL166: _temp165= _temp164.f2; goto _LL163; _LL163: { void* _temp169= _temp167;
struct Cyc_List_List* _temp179; struct Cyc_List_List* _temp181; struct Cyc_List_List
_temp183; struct Cyc_List_List* _temp184; struct _tagged_string* _temp186;
struct Cyc_List_List* _temp188; _LL171: if(( unsigned int) _temp169 > 1u?((
struct _tunion_struct*) _temp169)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL180:
_temp179=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp169)->f1;
if( _temp179 == 0){ goto _LL172;} else{ goto _LL173;}} else{ goto _LL173;}
_LL173: if(( int) _temp169 == Cyc_Absyn_Loc_n){ goto _LL174;} else{ goto _LL175;}
_LL175: if(( unsigned int) _temp169 > 1u?(( struct _tunion_struct*) _temp169)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL182: _temp181=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp169)->f1; if( _temp181 == 0){ goto _LL177;} else{
_temp183=* _temp181; _LL187: _temp186=( struct _tagged_string*) _temp183.hd;
goto _LL185; _LL185: _temp184=( struct Cyc_List_List*) _temp183.tl; goto _LL176;}}
else{ goto _LL177;} _LL177: if(( unsigned int) _temp169 > 1u?(( struct
_tunion_struct*) _temp169)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL189: _temp188=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp169)->f1; goto
_LL178;} else{ goto _LL170;} _LL172: { struct _handler_cons _temp190;
_push_handler(& _temp190);{ struct _xtunion_struct* _temp191=( struct
_xtunion_struct*) setjmp( _temp190.handler); if( ! _temp191){{ struct Cyc_Core_Opt*
_temp193=({ struct Cyc_Core_Opt* _temp192=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp192->v=( void*)(( struct Cyc_Absyn_XTuniondecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_XTuniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_xtuniondecl_f, _temp165);
_temp192;}); _npop_handler( 0u); return _temp193;}; _pop_handler();} else{
struct _xtunion_struct* _temp195= _temp191; _LL197: if( _temp195->tag == Cyc_Dict_Absent_tag){
goto _LL198;} else{ goto _LL199;} _LL199: goto _LL200; _LL198: return 0; _LL200:(
void) _throw( _temp195); _LL196:;}}} _LL174: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp201=( char*)"lookup_xtuniondecl: impossible";
struct _tagged_string _temp202; _temp202.curr= _temp201; _temp202.base= _temp201;
_temp202.last_plus_one= _temp201 + 31; _temp202;})); return 0; _LL176: { struct
Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp203; _push_handler(& _temp203);{
struct _xtunion_struct* _temp204=( struct _xtunion_struct*) setjmp( _temp203.handler);
if( ! _temp204){ ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp186, _temp184);;
_pop_handler();} else{ struct _xtunion_struct* _temp206= _temp204; _LL208: if(
_temp206->tag == Cyc_Dict_Absent_tag){ goto _LL209;} else{ goto _LL210;} _LL210:
goto _LL211; _LL209: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp212=( char*)"bad qualified name for xtunion"; struct _tagged_string
_temp213; _temp213.curr= _temp212; _temp213.base= _temp212; _temp213.last_plus_one=
_temp212 + 31; _temp213;}));( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Dict_Absent_struct* _temp214=( struct Cyc_Dict_Absent_struct*) GC_malloc(
sizeof( struct Cyc_Dict_Absent_struct)); _temp214[ 0]=({ struct Cyc_Dict_Absent_struct
_temp215; _temp215.tag= Cyc_Dict_Absent_tag; _temp215;}); _temp214;})); _LL211:(
void) _throw( _temp206); _LL207:;}}} return({ struct Cyc_Core_Opt* _temp216=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp216->v=(
void*)(( struct Cyc_Absyn_XTuniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->xtuniondecls, _temp165); _temp216;});}
_LL178: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp188); return({
struct Cyc_Core_Opt* _temp217=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp217->v=( void*)(( struct Cyc_Absyn_XTuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->xtuniondecls,
_temp165); _temp217;});} _LL170:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp220; struct _tagged_string* _temp221; void* _temp223; struct _tuple0*
_temp218= q; _temp220=* _temp218; _LL224: _temp223= _temp220.f1; goto _LL222;
_LL222: _temp221= _temp220.f2; goto _LL219; _LL219: { void* _temp225= _temp223;
struct Cyc_List_List* _temp235; struct Cyc_List_List* _temp237; struct Cyc_List_List
_temp239; struct Cyc_List_List* _temp240; struct _tagged_string* _temp242;
struct Cyc_List_List* _temp244; _LL227: if(( int) _temp225 == Cyc_Absyn_Loc_n){
goto _LL228;} else{ goto _LL229;} _LL229: if(( unsigned int) _temp225 > 1u?((
struct _tunion_struct*) _temp225)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL236:
_temp235=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp225)->f1;
if( _temp235 == 0){ goto _LL230;} else{ goto _LL231;}} else{ goto _LL231;}
_LL231: if(( unsigned int) _temp225 > 1u?(( struct _tunion_struct*) _temp225)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL238: _temp237=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp225)->f1; if( _temp237 == 0){ goto _LL233;} else{
_temp239=* _temp237; _LL243: _temp242=( struct _tagged_string*) _temp239.hd;
goto _LL241; _LL241: _temp240=( struct Cyc_List_List*) _temp239.tl; goto _LL232;}}
else{ goto _LL233;} _LL233: if(( unsigned int) _temp225 > 1u?(( struct
_tunion_struct*) _temp225)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL245: _temp244=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp225)->f1; goto
_LL234;} else{ goto _LL226;} _LL228: goto _LL230; _LL230: return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_enumdecl_f, _temp221);
_LL232: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace( te, loc,
_temp242, _temp240); return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, _temp221);}
_LL234: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp244); return((
struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->enumdecls, _temp221);} _LL226:;}} static struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl_f( struct Cyc_Tcenv_Genv* ge, struct _tagged_string*
v){ return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs, v);} struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple0* q){ struct _tuple0 _temp248; struct _tagged_string*
_temp249; void* _temp251; struct _tuple0* _temp246= q; _temp248=* _temp246;
_LL252: _temp251= _temp248.f1; goto _LL250; _LL250: _temp249= _temp248.f2; goto
_LL247; _LL247: { void* _temp253= _temp251; struct Cyc_List_List* _temp263;
struct Cyc_List_List* _temp265; struct Cyc_List_List _temp267; struct Cyc_List_List*
_temp268; struct _tagged_string* _temp270; struct Cyc_List_List* _temp272;
_LL255: if(( int) _temp253 == Cyc_Absyn_Loc_n){ goto _LL256;} else{ goto _LL257;}
_LL257: if(( unsigned int) _temp253 > 1u?(( struct _tunion_struct*) _temp253)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL264: _temp263=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp253)->f1; if( _temp263 == 0){ goto _LL258;} else{
goto _LL259;}} else{ goto _LL259;} _LL259: if(( unsigned int) _temp253 > 1u?((
struct _tunion_struct*) _temp253)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL266:
_temp265=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp253)->f1;
if( _temp265 == 0){ goto _LL261;} else{ _temp267=* _temp265; _LL271: _temp270=(
struct _tagged_string*) _temp267.hd; goto _LL269; _LL269: _temp268=( struct Cyc_List_List*)
_temp267.tl; goto _LL260;}} else{ goto _LL261;} _LL261: if(( unsigned int)
_temp253 > 1u?(( struct _tunion_struct*) _temp253)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL273: _temp272=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp253)->f1; goto _LL262;} else{ goto _LL254;} _LL256: goto _LL258; _LL258:
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_typedefdecl_f, _temp249); _LL260: { struct Cyc_Tcenv_Genv* ge=
Cyc_Tcenv_lookup_namespace( te, loc, _temp270, _temp268); return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs,
_temp249);} _LL262: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp272); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs, _temp249);} _LL254:;}}
struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({ struct Cyc_Tcenv_Genv*
_temp274=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof( struct Cyc_Tcenv_Genv));
_temp274->namespaces=( struct Cyc_Set_Set*) Cyc_Tcutil_empty_var_set->v;
_temp274->structdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp274->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp274->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp274->xtuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp274->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp274->typedefs=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp274->ordinaries=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp274->availables= 0; _temp274;});} void* Cyc_Tcenv_get_visible( void* f){
while( 1) { void* _temp275= f; void* _temp283; void* _temp285; void* _temp287;
void* _temp289; void* _temp291; _LL277: if((( struct _tunion_struct*) _temp275)->tag
== Cyc_Tcenv_Outermost_tag){ _LL284: _temp283=( void*)(( struct Cyc_Tcenv_Outermost_struct*)
_temp275)->f1; goto _LL278;} else{ goto _LL279;} _LL279: if((( struct
_tunion_struct*) _temp275)->tag == Cyc_Tcenv_Frame_tag){ _LL288: _temp287=( void*)((
struct Cyc_Tcenv_Frame_struct*) _temp275)->f1; goto _LL286; _LL286: _temp285=(
void*)(( struct Cyc_Tcenv_Frame_struct*) _temp275)->f2; goto _LL280;} else{ goto
_LL281;} _LL281: if((( struct _tunion_struct*) _temp275)->tag == Cyc_Tcenv_Hidden_tag){
_LL292: _temp291=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp275)->f1; goto
_LL290; _LL290: _temp289=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp275)->f2;
goto _LL282;} else{ goto _LL276;} _LL278: return _temp283; _LL280: return
_temp287; _LL282: f= _temp289; goto _LL276; _LL276:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp293= f; void* _temp301; void* _temp303; void*
_temp305; void* _temp307; void* _temp309; _LL295: if((( struct _tunion_struct*)
_temp293)->tag == Cyc_Tcenv_Outermost_tag){ _LL302: _temp301=( void*)(( struct
Cyc_Tcenv_Outermost_struct*) _temp293)->f1; goto _LL296;} else{ goto _LL297;}
_LL297: if((( struct _tunion_struct*) _temp293)->tag == Cyc_Tcenv_Frame_tag){
_LL306: _temp305=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp293)->f1; goto
_LL304; _LL304: _temp303=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp293)->f2;
goto _LL298;} else{ goto _LL299;} _LL299: if((( struct _tunion_struct*) _temp293)->tag
== Cyc_Tcenv_Hidden_tag){ _LL310: _temp309=( void*)(( struct Cyc_Tcenv_Hidden_struct*)
_temp293)->f1; goto _LL308; _LL308: _temp307=( void*)(( struct Cyc_Tcenv_Hidden_struct*)
_temp293)->f2; goto _LL300;} else{ goto _LL294;} _LL296: return( void*)({ struct
Cyc_Tcenv_Outermost_struct* _temp311=( struct Cyc_Tcenv_Outermost_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct)); _temp311[ 0]=({ struct
Cyc_Tcenv_Outermost_struct _temp312; _temp312.tag= Cyc_Tcenv_Outermost_tag;
_temp312.f1=( void*) x; _temp312;}); _temp311;}); _LL298: return( void*)({
struct Cyc_Tcenv_Frame_struct* _temp313=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp313[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp314; _temp314.tag= Cyc_Tcenv_Frame_tag; _temp314.f1=( void*) x; _temp314.f2=(
void*) _temp303; _temp314;}); _temp313;}); _LL300: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp315=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp315[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp316; _temp316.tag= Cyc_Tcenv_Hidden_tag;
_temp316.f1=( void*) _temp309; _temp316.f2=( void*)(( void*(*)( void* f, void* x))
Cyc_Tcenv_put_visible)( _temp307, x); _temp316;}); _temp315;}); _LL294:;} void*
Cyc_Tcenv_apply_to_visible( void*(* g)( void*), void* f){ return(( void*(*)(
void* f, void* x)) Cyc_Tcenv_put_visible)( f, g((( void*(*)( void* f)) Cyc_Tcenv_get_visible)(
f)));} void* Cyc_Tcenv_map_frames( void*(* g)( void*), void* f){ void* _temp317=
f; void* _temp325; void* _temp327; void* _temp329; void* _temp331; void*
_temp333; _LL319: if((( struct _tunion_struct*) _temp317)->tag == Cyc_Tcenv_Outermost_tag){
_LL326: _temp325=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp317)->f1;
goto _LL320;} else{ goto _LL321;} _LL321: if((( struct _tunion_struct*) _temp317)->tag
== Cyc_Tcenv_Frame_tag){ _LL330: _temp329=( void*)(( struct Cyc_Tcenv_Frame_struct*)
_temp317)->f1; goto _LL328; _LL328: _temp327=( void*)(( struct Cyc_Tcenv_Frame_struct*)
_temp317)->f2; goto _LL322;} else{ goto _LL323;} _LL323: if((( struct
_tunion_struct*) _temp317)->tag == Cyc_Tcenv_Hidden_tag){ _LL334: _temp333=(
void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp317)->f1; goto _LL332; _LL332:
_temp331=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp317)->f2; goto _LL324;}
else{ goto _LL318;} _LL320: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp335=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp335[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp336; _temp336.tag= Cyc_Tcenv_Outermost_tag;
_temp336.f1=( void*) g( _temp325); _temp336;}); _temp335;}); _LL322: return(
void*)({ struct Cyc_Tcenv_Frame_struct* _temp337=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp337[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp338; _temp338.tag= Cyc_Tcenv_Frame_tag; _temp338.f1=( void*) g( _temp329);
_temp338.f2=( void*)(( void*(*)( void*(* g)( void*), void* f)) Cyc_Tcenv_map_frames)(
g, _temp327); _temp338;}); _temp337;}); _LL324: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp339=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp339[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp340; _temp340.tag= Cyc_Tcenv_Hidden_tag;
_temp340.f1=( void*) g( _temp333); _temp340.f2=( void*)(( void*(*)( void*(* g)(
void*), void* f)) Cyc_Tcenv_map_frames)( g, _temp331); _temp340;}); _temp339;});
_LL318:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Tcenv_Env_error_struct*
_temp341=( struct Cyc_Tcenv_Env_error_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Env_error_struct));
_temp341[ 0]=({ struct Cyc_Tcenv_Env_error_struct _temp342; _temp342.tag= Cyc_Tcenv_Env_error_tag;
_temp342;}); _temp341;}));} return(( struct Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)((
void*)( te->le)->v);} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Fenv* fe){ if( te->le == 0){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Tcenv_Env_error_struct* _temp343=( struct Cyc_Tcenv_Env_error_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Env_error_struct)); _temp343[ 0]=({ struct
Cyc_Tcenv_Env_error_struct _temp344; _temp344.tag= Cyc_Tcenv_Env_error_tag;
_temp344;}); _temp343;}));} return({ struct Cyc_Tcenv_Tenv* _temp345=( struct
Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp345->ns= te->ns;
_temp345->ae= te->ae; _temp345->le=({ struct Cyc_Core_Opt* _temp346=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp346->v=( void*)(( void*(*)( void*
f, struct Cyc_Tcenv_Fenv* x)) Cyc_Tcenv_put_visible)(( void*)( te->le)->v, fe);
_temp346;}); _temp345;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_old_ctrl(
struct Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv* _temp347=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp347->shared= f->shared;
_temp347->type_vars= f->type_vars; _temp347->locals= f->locals; _temp347->encloser=
f->encloser; _temp347->ctrl_env= f->ctrl_env; _temp347->capability=( void*)((
void*) f->capability); _temp347->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp347;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl( struct
Cyc_Tcenv_Fenv* f){ struct Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({ struct
Cyc_Tcenv_Fenv* _temp348=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp348->shared= f->shared; _temp348->type_vars= f->type_vars; _temp348->locals=
f->locals; _temp348->encloser= f->encloser; _temp348->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv*
_temp349=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv));
_temp349->continue_stmt=( void*)(( void*) c->continue_stmt); _temp349->break_stmt=(
void*)(( void*) c->break_stmt); _temp349->fallthru_clause= c->fallthru_clause;
_temp349->next_stmt=( void*)(( void*) c->next_stmt); _temp349->try_depth= c->try_depth;
_temp349;}); _temp348->capability=( void*)(( void*) f->capability); _temp348->curr_rgn=(
void*)(( void*) f->curr_rgn); _temp348;});} void* Cyc_Tcenv_return_typ( struct
Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string msg))
Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp350=( char*)"Tcenv: unexpected return_typ";
struct _tagged_string _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 29; _temp351;}));} return( void*)(( Cyc_Tcenv_get_fenv(
te))->shared)->return_typ;} struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv(
te))->type_vars;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp352=( char*)"Tcenv: unexpected add_type_vars"; struct _tagged_string
_temp353; _temp353.curr= _temp352; _temp353.base= _temp352; _temp353.last_plus_one=
_temp352 + 32; _temp353;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct Cyc_List_List* new_tvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, fe->type_vars);
Cyc_Tcutil_check_unique_tvars( loc, new_tvs); fe->type_vars= new_tvs; return Cyc_Tcenv_put_fenv(
te, fe);}} static int Cyc_Tcenv_shadow_ctr= 1; struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl*
vd){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp354=( char*)"Tcenv: unexpected add_local_var";
struct _tagged_string _temp355; _temp355.curr= _temp354; _temp355.base= _temp354;
_temp355.last_plus_one= _temp354 + 32; _temp355;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); struct _tagged_string* v=(*
vd->name).f2;{ void* _temp356=(* vd->name).f1; _LL358: if(( int) _temp356 == Cyc_Absyn_Loc_n){
goto _LL359;} else{ goto _LL360;} _LL360: goto _LL361; _LL359: goto _LL357;
_LL361:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp362=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp362[ 0]=({ struct Cyc_Core_Impossible_struct _temp363; _temp363.tag= Cyc_Core_Impossible_tag;
_temp363.f1=( struct _tagged_string)({ char* _temp364=( char*)"add_local_var: called with Rel_n";
struct _tagged_string _temp365; _temp365.curr= _temp364; _temp365.base= _temp364;
_temp365.last_plus_one= _temp364 + 33; _temp365;}); _temp363;}); _temp362;}));
goto _LL357; _LL357:;} if((( int(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_member)( fe->locals, v)){ vd->shadow=( ++ Cyc_Tcenv_shadow_ctr);}
fe->locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, void* data)) Cyc_Dict_insert)( fe->locals, v,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp366=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp366[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp367; _temp367.tag= Cyc_Absyn_Local_b_tag; _temp367.f1= vd; _temp367;});
_temp366;})); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl*
vd){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp368=( char*)"Tcenv: unexpected add_pat_var";
struct _tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 30; _temp369;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); struct _tagged_string* v=(*
vd->name).f2; if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_member)( fe->locals, v)){ vd->shadow=( ++ Cyc_Tcenv_shadow_ctr);} fe->locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
void* data)) Cyc_Dict_insert)( fe->locals, v,( void*)({ struct Cyc_Absyn_Pat_b_struct*
_temp370=( struct Cyc_Absyn_Pat_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct));
_temp370[ 0]=({ struct Cyc_Absyn_Pat_b_struct _temp371; _temp371.tag= Cyc_Absyn_Pat_b_tag;
_temp371.f1= vd; _temp371;}); _temp370;})); return Cyc_Tcenv_put_fenv( te, fe);}}
void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple0* q){ struct _tuple0 _temp374; struct _tagged_string*
_temp375; void* _temp377; struct _tuple0* _temp372= q; _temp374=* _temp372;
_LL378: _temp377= _temp374.f1; goto _LL376; _LL376: _temp375= _temp374.f2; goto
_LL373; _LL373: { void* _temp379= _temp377; struct Cyc_List_List* _temp388;
_LL381: if(( int) _temp379 == Cyc_Absyn_Loc_n){ goto _LL382;} else{ goto _LL383;}
_LL383: if(( unsigned int) _temp379 > 1u?(( struct _tunion_struct*) _temp379)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL389: _temp388=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp379)->f1; if( _temp388 == 0){ goto _LL387;} else{
goto _LL385;}} else{ goto _LL385;} _LL387: if( te->le != 0){ goto _LL384;} else{
goto _LL385;} _LL385: goto _LL386; _LL382: if( te->le == 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Dict_Absent_struct* _temp390=( struct Cyc_Dict_Absent_struct*)
GC_malloc( sizeof( struct Cyc_Dict_Absent_struct)); _temp390[ 0]=({ struct Cyc_Dict_Absent_struct
_temp391; _temp391.tag= Cyc_Dict_Absent_tag; _temp391;}); _temp390;}));} else{
goto _LL384;} _LL384: { struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct _handler_cons _temp392; _push_handler(& _temp392);{ struct
_xtunion_struct* _temp393=( struct _xtunion_struct*) setjmp( _temp392.handler);
if( ! _temp393){{ void* _temp396=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp394=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp394[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp395; _temp395.tag= Cyc_Tcenv_VarRes_tag;
_temp395.f1=( void*)(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( fe->locals, _temp375); _temp395;}); _temp394;});
_npop_handler( 0u); return _temp396;}; _pop_handler();} else{ struct
_xtunion_struct* _temp398= _temp393; _LL400: if( _temp398->tag == Cyc_Dict_Absent_tag){
goto _LL401;} else{ goto _LL402;} _LL402: goto _LL403; _LL401: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL403:( void) _throw( _temp398); _LL399:;}}} _LL386: { struct
_handler_cons _temp404; _push_handler(& _temp404);{ struct _xtunion_struct*
_temp405=( struct _xtunion_struct*) setjmp( _temp404.handler); if( ! _temp405){{
void* _temp406= Cyc_Tcenv_lookup_ordinary_global( te, loc, q); _npop_handler( 0u);
return _temp406;}; _pop_handler();} else{ struct _xtunion_struct* _temp408=
_temp405; _LL410: if( _temp408->tag == Cyc_Dict_Absent_tag){ goto _LL411;} else{
goto _LL412;} _LL412: goto _LL413; _LL411: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp414= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifer: %.*s",
_temp414.last_plus_one - _temp414.curr, _temp414.curr);})); return( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp415=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp415[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp416; _temp416.tag= Cyc_Tcenv_VarRes_tag; _temp416.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b);
_temp416;}); _temp415;}); _LL413:( void) _throw( _temp408); _LL409:;}}} _LL380:;}}
void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp417=( char*)"Tcenv: unexpected process_continue"; struct _tagged_string
_temp418; _temp418.curr= _temp417; _temp418.base= _temp417; _temp418.last_plus_one=
_temp417 + 35; _temp418;}));}{ void* _temp419=( void*)(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt;
struct Cyc_Absyn_Stmt* _temp429; _LL421: if(( unsigned int) _temp419 > 3u?((
struct _tunion_struct*) _temp419)->tag == Cyc_Tcenv_Stmt_j_tag: 0){ _LL430:
_temp429=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Tcenv_Stmt_j_struct*) _temp419)->f1;
goto _LL422;} else{ goto _LL423;} _LL423: if(( int) _temp419 == Cyc_Tcenv_NotLoop_j){
goto _LL424;} else{ goto _LL425;} _LL425: if(( int) _temp419 == Cyc_Tcenv_CaseEnd_j){
goto _LL426;} else{ goto _LL427;} _LL427: if(( int) _temp419 == Cyc_Tcenv_FnEnd_j){
goto _LL428;} else{ goto _LL420;} _LL422: _temp429->non_local_preds=({ struct
Cyc_List_List* _temp431=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp431->hd=( void*) s; _temp431->tl= _temp429->non_local_preds; _temp431;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp429; return; _LL424: Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp432=( char*)"continue not in a loop"; struct
_tagged_string _temp433; _temp433.curr= _temp432; _temp433.base= _temp432;
_temp433.last_plus_one= _temp432 + 23; _temp433;})); return; _LL426:(( void(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp434=( char*)"Tcenv: continue dest is CaseEnd"; struct _tagged_string
_temp435; _temp435.curr= _temp434; _temp435.base= _temp434; _temp435.last_plus_one=
_temp434 + 32; _temp435;})); return; _LL428:(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp436=( char*)"Tcenv: continue dest is CaseEnd";
struct _tagged_string _temp437; _temp437.curr= _temp436; _temp437.base= _temp436;
_temp437.last_plus_one= _temp436 + 32; _temp437;})); return; _LL420:;}} void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp438=( char*)"Tcenv: unexpected process_break";
struct _tagged_string _temp439; _temp439.curr= _temp438; _temp439.base= _temp438;
_temp439.last_plus_one= _temp438 + 32; _temp439;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); void* _temp440=( void*)( fe->ctrl_env)->break_stmt;
struct Cyc_Absyn_Stmt* _temp450; _LL442: if(( unsigned int) _temp440 > 3u?((
struct _tunion_struct*) _temp440)->tag == Cyc_Tcenv_Stmt_j_tag: 0){ _LL451:
_temp450=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Tcenv_Stmt_j_struct*) _temp440)->f1;
goto _LL443;} else{ goto _LL444;} _LL444: if(( int) _temp440 == Cyc_Tcenv_NotLoop_j){
goto _LL445;} else{ goto _LL446;} _LL446: if(( int) _temp440 == Cyc_Tcenv_FnEnd_j){
goto _LL447;} else{ goto _LL448;} _LL448: if(( int) _temp440 == Cyc_Tcenv_CaseEnd_j){
goto _LL449;} else{ goto _LL441;} _LL443: _temp450->non_local_preds=({ struct
Cyc_List_List* _temp452=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp452->hd=( void*) s; _temp452->tl= _temp450->non_local_preds; _temp452;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp450; return; _LL445: Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp453=( char*)"break not in a loop or switch";
struct _tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 30; _temp454;})); return; _LL447: if(( void*)(
fe->shared)->return_typ !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp455=( char*)"break causes function not to return a value";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 44; _temp456;}));} return; _LL449: Cyc_Tcutil_terr(
s->loc,( struct _tagged_string)({ char* _temp457=( char*)"break causes outer switch clause to implicitly fallthru";
struct _tagged_string _temp458; _temp458.curr= _temp457; _temp458.base= _temp457;
_temp458.last_plus_one= _temp457 + 56; _temp458;})); return; _LL441:;}} void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct _tagged_string* l,
struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp459=( char*)"Tcenv: unexpected process_goto"; struct _tagged_string
_temp460; _temp460.curr= _temp459; _temp460.base= _temp459; _temp460.last_plus_one=
_temp459 + 31; _temp460;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct Cyc_Core_Opt* sopt2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels, l);
if( sopt2 == 0){ struct Cyc_SlowDict_Dict* needed=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* slopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_lookup_opt)( needed, l); if( slopt
== 0){ slopt=({ struct Cyc_Core_Opt* _temp461=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp461->v=( void*) 0; _temp461;});}( fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_string*
key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)( needed, l,({ struct Cyc_List_List*
_temp462=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp462->hd=( void*) s; _temp462->tl=( struct Cyc_List_List*) slopt->v;
_temp462;}));} else{(( struct Cyc_Absyn_Stmt*) sopt2->v)->non_local_preds=({
struct Cyc_List_List* _temp463=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp463->hd=( void*) s; _temp463->tl=(( struct Cyc_Absyn_Stmt*)
sopt2->v)->non_local_preds; _temp463;});* sopt=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Stmt*) sopt2->v);}}} struct _tuple4* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause***
clauseopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp464=( char*)"Tcenv: unexpected process_fallthru";
struct _tagged_string _temp465; _temp465.curr= _temp464; _temp465.base= _temp464;
_temp465.last_plus_one= _temp464 + 35; _temp465;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); struct _tuple4* ans=( fe->ctrl_env)->fallthru_clause;
if( ans != 0){(((* ans).f1)->body)->non_local_preds=({ struct Cyc_List_List*
_temp466=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp466->hd=( void*) s; _temp466->tl=(((* ans).f1)->body)->non_local_preds;
_temp466;});* clauseopt=({ struct Cyc_Absyn_Switch_clause** _temp467=( struct
Cyc_Absyn_Switch_clause**) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*) *
1); _temp467[ 0]=(* ans).f1; _temp467;});} return ans;}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv* te, struct _tuple3* pat_typ,
struct Cyc_Absyn_Switch_clause* clause){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp468=( char*)"Tcenv: unexpected set_fallthru"; struct _tagged_string
_temp469; _temp469.curr= _temp468; _temp469.base= _temp468; _temp469.last_plus_one=
_temp468 + 31; _temp469;}));}{ struct Cyc_List_List* ft_typ= 0;{ struct Cyc_List_List*
vds=(* pat_typ).f2; for( 0; vds != 0; vds= vds->tl){ ft_typ=({ struct Cyc_List_List*
_temp470=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp470->hd=( void*)(( void*)(( struct Cyc_Absyn_Vardecl*) vds->hd)->type);
_temp470->tl= ft_typ; _temp470;});}} ft_typ=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( ft_typ);{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=({ struct _tuple4*
_temp471=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp471->f1=
clause; _temp471->f2=(* pat_typ).f1; _temp471->f3= ft_typ; _temp471;}); return
Cyc_Tcenv_put_fenv( te, fe);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp472=( char*)"Tcenv: unexpected clear_fallthru";
struct _tagged_string _temp473; _temp473.curr= _temp472; _temp473.base= _temp472;
_temp473.last_plus_one= _temp472 + 33; _temp473;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=
0; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp474=( char*)"Tcenv: unexpected set_in_loop"; struct
_tagged_string _temp475; _temp475.curr= _temp474; _temp475.base= _temp474;
_temp475.last_plus_one= _temp474 + 30; _temp475;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=(
void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp476=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp476[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp477; _temp477.tag= Cyc_Tcenv_Stmt_j_tag; _temp477.f1= continue_dest;
_temp477;}); _temp476;})));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)(
fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp478=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp478[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp479; _temp479.tag= Cyc_Tcenv_Stmt_j_tag; _temp479.f1= continue_dest;
_temp479;}); _temp478;}))); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp480=( char*)"Tcenv: unexpected enter_try"; struct _tagged_string _temp481;
_temp481.curr= _temp480; _temp481.base= _temp480; _temp481.last_plus_one=
_temp480 + 28; _temp481;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te)); ++( fe->ctrl_env)->try_depth; return Cyc_Tcenv_put_fenv(
te, fe);}} int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv* te){ if( te->le
== 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp482=( char*)"Tcenv: unexpected get_try_depth";
struct _tagged_string _temp483; _temp483.curr= _temp482; _temp483.base= _temp482;
_temp483.last_plus_one= _temp482 + 32; _temp483;}));} return(( Cyc_Tcenv_get_fenv(
te))->ctrl_env)->try_depth;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp484=( char*)"Tcenv: unexpected set_in_switch";
struct _tagged_string _temp485; _temp485.curr= _temp484; _temp485.base= _temp484;
_temp485.last_plus_one= _temp484 + 32; _temp485;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->break_stmt=(
void*)(( void*)( fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=(
void*)(( void*) Cyc_Tcenv_CaseEnd_j)); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv* te, void* j){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp486=( char*)"Tcenv: unexpected set_next";
struct _tagged_string _temp487; _temp487.curr= _temp486; _temp487.base= _temp486;
_temp487.last_plus_one= _temp486 + 27; _temp487;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->next_stmt=(
void*) j); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv* te, struct _tagged_string* v, struct Cyc_Absyn_Stmt* s){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp488=( char*)"Tcenv: unexpected add_label";
struct _tagged_string _temp489; _temp489.curr= _temp488; _temp489.base= _temp488;
_temp489.last_plus_one= _temp488 + 28; _temp489;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); struct Cyc_SlowDict_Dict* needed=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* sl_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_lookup_opt)( needed, v); if( sl_opt
!= 0){( fe->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_delete)( needed, v);{ struct Cyc_List_List*
sl=( struct Cyc_List_List*) sl_opt->v; s->non_local_preds= sl; for( 0; sl != 0;
sl= sl->tl){ void* _temp490=( void*)(( struct Cyc_Absyn_Stmt*) sl->hd)->r;
struct Cyc_Absyn_Stmt* _temp496; struct Cyc_Absyn_Stmt** _temp498; struct
_tagged_string* _temp499; _LL492: if(( unsigned int) _temp490 > 1u?(( struct
_tunion_struct*) _temp490)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL500: _temp499=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp490)->f1; goto
_LL497; _LL497: _temp496=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp490)->f2; _temp498=&(( struct Cyc_Absyn_Goto_s_struct*) _temp490)->f2; goto
_LL493;} else{ goto _LL494;} _LL494: goto _LL495; _LL493:* _temp498=( struct Cyc_Absyn_Stmt*)
s; goto _LL491; _LL495:(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp501=( char*)"Tcenv: add_label backpatching of non-goto";
struct _tagged_string _temp502; _temp502.curr= _temp501; _temp502.base= _temp501;
_temp502.last_plus_one= _temp501 + 42; _temp502;})); goto _LL491; _LL491:;}}}
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)((
fe->shared)->seen_labels, v)){ Cyc_Tcutil_terr( s->loc,({ struct _tagged_string
_temp503=* v; xprintf("Repeated label: %.*s", _temp503.last_plus_one - _temp503.curr,
_temp503.curr);}));}( fe->shared)->seen_labels=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Stmt* data))
Cyc_Dict_insert)(( fe->shared)->seen_labels, v, s); return te;}} int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv* te){ return(( int(*)( struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)(((
Cyc_Tcenv_get_fenv( te))->shared)->needed_labels);} struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp504=( char*)"Tcenv: unexpected get_encloser";
struct _tagged_string _temp505; _temp505.curr= _temp504; _temp505.base= _temp504;
_temp505.last_plus_one= _temp504 + 31; _temp505;}));} return( Cyc_Tcenv_get_fenv(
te))->encloser;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp506=( char*)"Tcenv: unexpected set_encloser"; struct _tagged_string
_temp507; _temp507.curr= _temp506; _temp507.base= _temp506; _temp507.last_plus_one=
_temp506 + 31; _temp507;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp508=( char*)"Tcenv: unexpected new_block"; struct
_tagged_string _temp509; _temp509.curr= _temp508; _temp509.base= _temp508;
_temp509.last_plus_one= _temp508 + 28; _temp509;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({
struct Cyc_Absyn_VarType_struct* _temp510=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp510[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp511; _temp511.tag= Cyc_Absyn_VarType_tag; _temp511.f1= block_rgn; _temp511;});
_temp510;}); fe->type_vars=({ struct Cyc_List_List* _temp512=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp512->hd=( void*) block_rgn;
_temp512->tl= fe->type_vars; _temp512;}); Cyc_Tcutil_check_unique_tvars( loc, fe->type_vars);(
void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp513=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp513[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp514; _temp514.tag= Cyc_Absyn_JoinEff_tag;
_temp514.f1=({ struct Cyc_List_List* _temp515=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp515->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp517=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp517[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp518; _temp518.tag= Cyc_Absyn_AccessEff_tag;
_temp518.f1=( void*) block_typ; _temp518;}); _temp517;})); _temp515->tl=({
struct Cyc_List_List* _temp516=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp516->hd=( void*)(( void*) fe->capability); _temp516->tl=
0; _temp516;}); _temp515;}); _temp514;}); _temp513;})));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te){
return Cyc_Tcenv_new_named_block( loc, te, Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind));}
void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return(
void*) Cyc_Absyn_HeapRgn;} return( void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;}
void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* rgn){ if( te->le != 0){ void* capability=( void*)( Cyc_Tcenv_get_fenv(
te))->capability; if( Cyc_Tcutil_region_in_effect( 0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect(
1, rgn, capability)){ return;}} Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp519= Cyc_Absynpp_typ2string( rgn); xprintf("Expression accesses unavailable region %.*s",
_temp519.last_plus_one - _temp519.curr, _temp519.curr);}));} void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff){ void*
capability; if( te->le == 0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp520=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp520[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp521; _temp521.tag= Cyc_Absyn_JoinEff_tag;
_temp521.f1= 0; _temp521;}); _temp520;});} else{ struct Cyc_Tcenv_Fenv* fenv=
Cyc_Tcenv_get_fenv( te); capability=( void*) fenv->capability;} if( ! Cyc_Tcutil_subset_effect(
1, eff, capability)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp522=
Cyc_Absynpp_typ2string( capability); struct _tagged_string _temp523= Cyc_Absynpp_typ2string(
eff); xprintf("Capability %.*s does not cover function's effect %.*s", _temp522.last_plus_one
- _temp522.curr, _temp522.curr, _temp523.last_plus_one - _temp523.curr, _temp523.curr);}));}}
struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict* locals=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); struct Cyc_Absyn_Tvar* rgn0=({ struct Cyc_Absyn_Tvar*
_temp524=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp524->name=({ struct _tagged_string* _temp525=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp525[ 0]=({ struct
_tagged_string _temp526=*(* fd->name).f2; xprintf("`%.*s", _temp526.last_plus_one
- _temp526.curr, _temp526.curr);}); _temp525;}); _temp524->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp524;}); struct
Cyc_List_List* tvs=({ struct Cyc_List_List* _temp527=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp527->hd=( void*) rgn0; _temp527->tl=
fd->tvs; _temp527;}); Cyc_Tcutil_check_unique_tvars( loc, tvs);{ void* param_rgn=(
void*)({ struct Cyc_Absyn_VarType_struct* _temp528=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp528[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp529; _temp529.tag= Cyc_Absyn_VarType_tag; _temp529.f1= rgn0; _temp529;});
_temp528;}); struct Cyc_List_List* vds= 0;{ struct Cyc_List_List* args= fd->args;
for( 0; args != 0; args= args->tl){ struct Cyc_Absyn_Vardecl* vd=({ struct Cyc_Absyn_Vardecl*
_temp530=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)
* 1); _temp530[ 0]=({ struct Cyc_Absyn_Vardecl _temp531; _temp531.sc=( void*)((
void*) Cyc_Absyn_Public); _temp531.name=({ struct _tuple0* _temp534=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0) * 1); _temp534[ 0]=({ struct
_tuple0 _temp535; _temp535.f1=( void*) Cyc_Absyn_Loc_n; _temp535.f2=(*(( struct
_tuple6*) args->hd)).f1; _temp535;}); _temp534;}); _temp531.tq=(*(( struct
_tuple6*) args->hd)).f2; _temp531.type=( void*)(*(( struct _tuple6*) args->hd)).f3;
_temp531.initializer= 0; _temp531.shadow= 0; _temp531.region=({ struct Cyc_Core_Opt*
_temp532=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp532[ 0]=({ struct Cyc_Core_Opt _temp533; _temp533.v=( void*) param_rgn;
_temp533;}); _temp532;}); _temp531.attributes= 0; _temp531;}); _temp530;}); vds=({
struct Cyc_List_List* _temp536=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp536[ 0]=({ struct Cyc_List_List _temp537;
_temp537.hd=( void*) vd; _temp537.tl= vds; _temp537;}); _temp536;}); locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
void* data)) Cyc_Dict_insert)( locals,(*(( struct _tuple6*) args->hd)).f1,( void*)({
struct Cyc_Absyn_Param_b_struct* _temp538=( struct Cyc_Absyn_Param_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct)); _temp538[ 0]=({ struct Cyc_Absyn_Param_b_struct
_temp539; _temp539.tag= Cyc_Absyn_Param_b_tag; _temp539.f1= vd; _temp539;});
_temp538;}));}} fd->param_vardecls=({ struct Cyc_Core_Opt* _temp540=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt) * 1); _temp540[ 0]=({ struct Cyc_Core_Opt
_temp541; _temp541.v=( void*) vds; _temp541;}); _temp540;}); return({ struct Cyc_Tcenv_Fenv*
_temp542=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp542->shared=({ struct Cyc_Tcenv_SharedFenv* _temp550=( struct Cyc_Tcenv_SharedFenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv)); _temp550->return_typ=( void*)((
void*) fd->ret_type); _temp550->seen_labels=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp550->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_SlowDict_empty)( Cyc_String_zstrptrcmp);
_temp550;}); _temp542->type_vars= tvs; _temp542->locals= locals; _temp542->encloser=
fd->body; _temp542->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv* _temp549=( struct Cyc_Tcenv_CtrlEnv*)
GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp549->continue_stmt=( void*)((
void*) Cyc_Tcenv_NotLoop_j); _temp549->break_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j);
_temp549->fallthru_clause= 0; _temp549->next_stmt=( void*)(( void*) Cyc_Tcenv_FnEnd_j);
_temp549->try_depth= 0; _temp549;}); _temp542->capability=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp543=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp543[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp544; _temp544.tag= Cyc_Absyn_JoinEff_tag; _temp544.f1=({ struct Cyc_List_List*
_temp545=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp545->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp547=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp547[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp548; _temp548.tag= Cyc_Absyn_AccessEff_tag;
_temp548.f1=( void*) param_rgn; _temp548;}); _temp547;})); _temp545->tl=({
struct Cyc_List_List* _temp546=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp546->hd=( void*)(( void*)( fd->effect)->v);
_temp546->tl= 0; _temp546;}); _temp545;}); _temp544;}); _temp543;})); _temp542->curr_rgn=(
void*) param_rgn; _temp542;});}}