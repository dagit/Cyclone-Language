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
struct _tuple0{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple0*
Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct
_tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
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
Cyc_Tcenv_Tenv* te, struct Cyc_List_List* new_tvs, struct Cyc_List_List* vds,
struct Cyc_Absyn_Switch_clause* clause); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next(
struct Cyc_Tcenv_Tenv*, void*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try(
struct Cyc_Tcenv_Tenv* te); extern int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv*
te); extern void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct _tagged_string*, struct
Cyc_Absyn_Stmt**); struct _tuple3{ struct Cyc_Absyn_Switch_clause* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; extern struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*, struct _tagged_string*, struct Cyc_Absyn_Stmt*); extern
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Tvar* name); extern void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region(
struct Cyc_Tcenv_Tenv* te, void* r); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern void
Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* eff); extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag; } ; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern
int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void* e2); extern int
Cyc_Tcutil_region_in_effect( int constrain, void* r, void* e); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k); char Cyc_Tcenv_Env_error_tag[ 10u]="Env_error";
struct Cyc_Tcenv_Env_error_struct{ char* tag; } ; void* Cyc_Tcenv_env_err(
struct _tagged_string msg){({ struct _tagged_string _temp0= msg; fprintf( Cyc_Stdio_stderr,"Error: %.*s\n",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush( Cyc_Stdio_stderr);(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Tcenv_Env_error_struct*
_temp1=( struct Cyc_Tcenv_Env_error_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Env_error_struct));
_temp1[ 0]=({ struct Cyc_Tcenv_Env_error_struct _temp2; _temp2.tag= Cyc_Tcenv_Env_error_tag;
_temp2;}); _temp1;}));} struct Cyc_Tcenv_Tenv; struct Cyc_Tcenv_Genv; struct Cyc_Tcenv_CtrlEnv{
void* continue_stmt; void* break_stmt; struct _tuple3* fallthru_clause; void*
next_stmt; int try_depth; } ; typedef struct Cyc_Tcenv_CtrlEnv* Cyc_Tcenv_ctrl_env_t;
struct Cyc_Tcenv_SharedFenv{ void* return_typ; struct Cyc_Dict_Dict* seen_labels;
struct Cyc_SlowDict_Dict* needed_labels; } ; struct Cyc_Tcenv_Fenv{ struct Cyc_Tcenv_SharedFenv*
shared; struct Cyc_List_List* type_vars; struct Cyc_Dict_Dict* locals; struct
Cyc_Absyn_Stmt* encloser; struct Cyc_Tcenv_CtrlEnv* ctrl_env; void* capability;
void* curr_rgn; } ; struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(){ Cyc_Tcutil_empty_var_set=({
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
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(({
struct Cyc_List_List* _temp11=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( ns); if( _temp11 == 0){ _throw( Null_Exception);} _temp11->tl;}));}
static void Cyc_Tcenv_check_repeat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct
_tagged_string* v, struct Cyc_List_List* nss){ for( 0; nss != 0; nss=({ struct
Cyc_List_List* _temp12= nss; if( _temp12 == 0){ _throw( Null_Exception);}
_temp12->tl;})){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp23= nss; if( _temp23 == 0){ _throw(
Null_Exception);} _temp23->hd;})); struct _handler_cons _temp13; _push_handler(&
_temp13);{ struct _xtunion_struct* _temp14=( struct _xtunion_struct*) setjmp(
_temp13.handler); if( ! _temp14){ lookup( ge2, v); Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp15=* v; xprintf("%.*s is ambiguous", _temp15.last_plus_one
- _temp15.curr, _temp15.curr);}));; _pop_handler();} else{ struct
_xtunion_struct* _temp17= _temp14; _LL19: if( _temp17->tag == Cyc_Dict_Absent_tag){
goto _LL20;} else{ goto _LL21;} _LL21: goto _LL22; _LL20: goto _LL18; _LL22:(
void) _throw( _temp17); _LL18:;}}} return;} static void* Cyc_Tcenv_scoped_lookup(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v){
struct Cyc_List_List* cns= te->ns; void* result; while( 1) { struct Cyc_Tcenv_Genv*
ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, cns);{ struct _handler_cons _temp24;
_push_handler(& _temp24);{ struct _xtunion_struct* _temp25=( struct
_xtunion_struct*) setjmp( _temp24.handler); if( ! _temp25){ result= lookup( ge,
v);(( void(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v, struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)( te, loc, lookup, v, ge->availables);{
void* _temp26= result; _npop_handler( 0u); return _temp26;}; _pop_handler();}
else{ struct _xtunion_struct* _temp28= _temp25; _LL30: if( _temp28->tag == Cyc_Dict_Absent_tag){
goto _LL31;} else{ goto _LL32;} _LL32: goto _LL33; _LL31: goto _LL29; _LL33:(
void) _throw( _temp28); _LL29:;}}}{ struct Cyc_List_List* nss= ge->availables;
for( 0; nss != 0; nss=({ struct Cyc_List_List* _temp34= nss; if( _temp34 == 0){
_throw( Null_Exception);} _temp34->tl;})){ struct Cyc_Tcenv_Genv* ge2=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp46= nss; if(
_temp46 == 0){ _throw( Null_Exception);} _temp46->hd;})); struct _handler_cons
_temp35; _push_handler(& _temp35);{ struct _xtunion_struct* _temp36=( struct
_xtunion_struct*) setjmp( _temp35.handler); if( ! _temp36){ result= lookup( ge2,
v);(( void(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v, struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)( te, loc, lookup, v,({
struct Cyc_List_List* _temp37= nss; if( _temp37 == 0){ _throw( Null_Exception);}
_temp37->tl;}));{ void* _temp38= result; _npop_handler( 0u); return _temp38;};
_pop_handler();} else{ struct _xtunion_struct* _temp40= _temp36; _LL42: if(
_temp40->tag == Cyc_Dict_Absent_tag){ goto _LL43;} else{ goto _LL44;} _LL44:
goto _LL45; _LL43: goto _LL41; _LL45:( void) _throw( _temp40); _LL41:;}}}} if(
cns == 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Dict_Absent_struct*
_temp47=( struct Cyc_Dict_Absent_struct*) GC_malloc( sizeof( struct Cyc_Dict_Absent_struct));
_temp47[ 0]=({ struct Cyc_Dict_Absent_struct _temp48; _temp48.tag= Cyc_Dict_Absent_tag;
_temp48;}); _temp47;}));} cns= Cyc_Tcenv_outer_namespace( cns);}} struct _tuple4{
void* f1; int f2; } ; static void* Cyc_Tcenv_lookup_ordinary_global_f( struct
Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ struct _tuple4* ans=(( struct
_tuple4*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
ge->ordinaries, v);(* ans).f2= 1; return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple0* q){
struct _tuple0 _temp51; struct _tagged_string* _temp52; void* _temp54; struct
_tuple0* _temp49= q; _temp51=* _temp49; _LL55: _temp54= _temp51.f1; goto _LL53;
_LL53: _temp52= _temp51.f2; goto _LL50; _LL50: { void* _temp56= _temp54; struct
Cyc_List_List* _temp66; struct Cyc_List_List* _temp68; struct Cyc_List_List
_temp70; struct Cyc_List_List* _temp71; struct _tagged_string* _temp73; struct
Cyc_List_List* _temp75; _LL58: if(( int) _temp56 == Cyc_Absyn_Loc_n){ goto _LL59;}
else{ goto _LL60;} _LL60: if(( unsigned int) _temp56 > 1u?(( struct
_tunion_struct*) _temp56)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL67: _temp66=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp56)->f1; if(
_temp66 == 0){ goto _LL61;} else{ goto _LL62;}} else{ goto _LL62;} _LL62: if((
unsigned int) _temp56 > 1u?(( struct _tunion_struct*) _temp56)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL69: _temp68=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp56)->f1; if( _temp68 == 0){ goto _LL64;} else{ _temp70=* _temp68; _LL74:
_temp73=( struct _tagged_string*) _temp70.hd; goto _LL72; _LL72: _temp71=(
struct Cyc_List_List*) _temp70.tl; goto _LL63;}} else{ goto _LL64;} _LL64: if((
unsigned int) _temp56 > 1u?(( struct _tunion_struct*) _temp56)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL76: _temp75=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp56)->f1; goto _LL65;} else{ goto _LL57;} _LL59: goto _LL61; _LL61: return((
void*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_ordinary_global_f,
_temp52); _LL63: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace( te,
loc, _temp73, _temp71); return Cyc_Tcenv_lookup_ordinary_global_f( ge, _temp52);}
_LL65: return Cyc_Tcenv_lookup_ordinary_global_f((( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp75), _temp52); _LL57:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct
_tagged_string* n, struct Cyc_List_List* ns){ struct Cyc_List_List* cns= te->ns;
struct Cyc_List_List* possible_results= 0; while( 1) { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, cns);{ struct Cyc_List_List* nss= ge->availables; for(
0; nss != 0; nss=({ struct Cyc_List_List* _temp77= nss; if( _temp77 == 0){
_throw( Null_Exception);} _temp77->tl;})){ struct Cyc_Tcenv_Genv* ge2=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp80= nss; if(
_temp80 == 0){ _throw( Null_Exception);} _temp80->hd;})); if((( int(*)( struct
Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( ge2->namespaces, n)){
possible_results=({ struct Cyc_List_List* _temp78=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp78->hd=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( cns,({
struct Cyc_List_List* _temp79=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp79->hd=( void*) n; _temp79->tl= ns; _temp79;})); _temp78->tl=
possible_results; _temp78;});}}} if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)( ge->namespaces, n)){ possible_results=({
struct Cyc_List_List* _temp81=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp81->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( cns,({ struct Cyc_List_List*
_temp82=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp82->hd=( void*) n; _temp82->tl= ns; _temp82;})); _temp81->tl=
possible_results; _temp81;});} if( possible_results != 0){ if(({ struct Cyc_List_List*
_temp83= possible_results; if( _temp83 == 0){ _throw( Null_Exception);} _temp83->tl;})
!= 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp84=* n; xprintf("%.*s is ambiguous",
_temp84.last_plus_one - _temp84.curr, _temp84.curr);}));} return( struct Cyc_List_List*)({
struct Cyc_List_List* _temp85= possible_results; if( _temp85 == 0){ _throw(
Null_Exception);} _temp85->hd;});} if( cns == 0){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Dict_Absent_struct* _temp86=( struct Cyc_Dict_Absent_struct*)
GC_malloc( sizeof( struct Cyc_Dict_Absent_struct)); _temp86[ 0]=({ struct Cyc_Dict_Absent_struct
_temp87; _temp87.tag= Cyc_Dict_Absent_tag; _temp87;}); _temp86;}));} cns= Cyc_Tcenv_outer_namespace(
cns);}} static struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
v);} struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp90;
struct _tagged_string* _temp91; void* _temp93; struct _tuple0* _temp88= q;
_temp90=* _temp88; _LL94: _temp93= _temp90.f1; goto _LL92; _LL92: _temp91=
_temp90.f2; goto _LL89; _LL89: { void* _temp95= _temp93; struct Cyc_List_List*
_temp105; struct Cyc_List_List* _temp107; struct Cyc_List_List* _temp109; struct
Cyc_List_List _temp111; struct Cyc_List_List* _temp112; struct _tagged_string*
_temp114; _LL97: if(( int) _temp95 == Cyc_Absyn_Loc_n){ goto _LL98;} else{ goto
_LL99;} _LL99: if(( unsigned int) _temp95 > 1u?(( struct _tunion_struct*)
_temp95)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL106: _temp105=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp95)->f1; if( _temp105 == 0){ goto _LL100;}
else{ goto _LL101;}} else{ goto _LL101;} _LL101: if(( unsigned int) _temp95 > 1u?((
struct _tunion_struct*) _temp95)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL108:
_temp107=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp95)->f1;
goto _LL102;} else{ goto _LL103;} _LL103: if(( unsigned int) _temp95 > 1u?((
struct _tunion_struct*) _temp95)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL110:
_temp109=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp95)->f1;
if( _temp109 == 0){ goto _LL96;} else{ _temp111=* _temp109; _LL115: _temp114=(
struct _tagged_string*) _temp111.hd; goto _LL113; _LL113: _temp112=( struct Cyc_List_List*)
_temp111.tl; goto _LL104;}} else{ goto _LL96;} _LL98: goto _LL100; _LL100:
return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_structdecl_f, _temp91); _LL102: { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, _temp107); return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
_temp91);} _LL104: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace( te,
loc, _temp114, _temp112); return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls, _temp91);}
_LL96:;}} static struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->uniondecls,
v);} struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp118; struct _tagged_string* _temp119; void* _temp121; struct _tuple0*
_temp116= q; _temp118=* _temp116; _LL122: _temp121= _temp118.f1; goto _LL120;
_LL120: _temp119= _temp118.f2; goto _LL117; _LL117: { void* _temp123= _temp121;
struct Cyc_List_List* _temp133; struct Cyc_List_List* _temp135; struct Cyc_List_List*
_temp137; struct Cyc_List_List _temp139; struct Cyc_List_List* _temp140; struct
_tagged_string* _temp142; _LL125: if(( int) _temp123 == Cyc_Absyn_Loc_n){ goto
_LL126;} else{ goto _LL127;} _LL127: if(( unsigned int) _temp123 > 1u?(( struct
_tunion_struct*) _temp123)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL134: _temp133=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp123)->f1; if(
_temp133 == 0){ goto _LL128;} else{ goto _LL129;}} else{ goto _LL129;} _LL129:
if(( unsigned int) _temp123 > 1u?(( struct _tunion_struct*) _temp123)->tag ==
Cyc_Absyn_Abs_n_tag: 0){ _LL136: _temp135=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp123)->f1; goto _LL130;} else{ goto _LL131;} _LL131: if(( unsigned int)
_temp123 > 1u?(( struct _tunion_struct*) _temp123)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL138: _temp137=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp123)->f1; if( _temp137 == 0){ goto _LL124;} else{ _temp139=* _temp137;
_LL143: _temp142=( struct _tagged_string*) _temp139.hd; goto _LL141; _LL141:
_temp140=( struct Cyc_List_List*) _temp139.tl; goto _LL132;}} else{ goto _LL124;}
_LL126: goto _LL128; _LL128: return(( struct Cyc_Absyn_Uniondecl**(*)( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_uniondecl_f, _temp119);
_LL130: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp135); return((
struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->uniondecls, _temp119);} _LL132: { struct Cyc_Tcenv_Genv*
ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp142, _temp140); return(( struct
Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( ge->uniondecls, _temp119);} _LL124:;}} static struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl_f( struct Cyc_Tcenv_Genv* ge, struct _tagged_string*
v){ return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls, v);} struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple0* q){ struct _tuple0 _temp146; struct _tagged_string*
_temp147; void* _temp149; struct _tuple0* _temp144= q; _temp146=* _temp144;
_LL150: _temp149= _temp146.f1; goto _LL148; _LL148: _temp147= _temp146.f2; goto
_LL145; _LL145: { void* _temp151= _temp149; struct Cyc_List_List* _temp161;
struct Cyc_List_List* _temp163; struct Cyc_List_List _temp165; struct Cyc_List_List*
_temp166; struct _tagged_string* _temp168; struct Cyc_List_List* _temp170;
_LL153: if(( int) _temp151 == Cyc_Absyn_Loc_n){ goto _LL154;} else{ goto _LL155;}
_LL155: if(( unsigned int) _temp151 > 1u?(( struct _tunion_struct*) _temp151)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL162: _temp161=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp151)->f1; if( _temp161 == 0){ goto _LL156;} else{
goto _LL157;}} else{ goto _LL157;} _LL157: if(( unsigned int) _temp151 > 1u?((
struct _tunion_struct*) _temp151)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL164:
_temp163=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp151)->f1;
if( _temp163 == 0){ goto _LL159;} else{ _temp165=* _temp163; _LL169: _temp168=(
struct _tagged_string*) _temp165.hd; goto _LL167; _LL167: _temp166=( struct Cyc_List_List*)
_temp165.tl; goto _LL158;}} else{ goto _LL159;} _LL159: if(( unsigned int)
_temp151 > 1u?(( struct _tunion_struct*) _temp151)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL171: _temp170=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp151)->f1; goto _LL160;} else{ goto _LL152;} _LL154: goto _LL156; _LL156:
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Tuniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_tuniondecl_f, _temp147); _LL158: { struct Cyc_Tcenv_Genv* ge=
Cyc_Tcenv_lookup_namespace( te, loc, _temp168, _temp166); return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp147);} _LL160: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp170); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp147);}
_LL152:;}} static struct Cyc_Absyn_XTuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_XTuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->xtuniondecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp174; struct _tagged_string* _temp175; void* _temp177; struct _tuple0*
_temp172= q; _temp174=* _temp172; _LL178: _temp177= _temp174.f1; goto _LL176;
_LL176: _temp175= _temp174.f2; goto _LL173; _LL173: { void* _temp179= _temp177;
struct Cyc_List_List* _temp189; struct Cyc_List_List* _temp191; struct Cyc_List_List
_temp193; struct Cyc_List_List* _temp194; struct _tagged_string* _temp196;
struct Cyc_List_List* _temp198; _LL181: if(( unsigned int) _temp179 > 1u?((
struct _tunion_struct*) _temp179)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL190:
_temp189=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp179)->f1;
if( _temp189 == 0){ goto _LL182;} else{ goto _LL183;}} else{ goto _LL183;}
_LL183: if(( int) _temp179 == Cyc_Absyn_Loc_n){ goto _LL184;} else{ goto _LL185;}
_LL185: if(( unsigned int) _temp179 > 1u?(( struct _tunion_struct*) _temp179)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL192: _temp191=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp179)->f1; if( _temp191 == 0){ goto _LL187;} else{
_temp193=* _temp191; _LL197: _temp196=( struct _tagged_string*) _temp193.hd;
goto _LL195; _LL195: _temp194=( struct Cyc_List_List*) _temp193.tl; goto _LL186;}}
else{ goto _LL187;} _LL187: if(( unsigned int) _temp179 > 1u?(( struct
_tunion_struct*) _temp179)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL199: _temp198=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp179)->f1; goto
_LL188;} else{ goto _LL180;} _LL182: { struct _handler_cons _temp200;
_push_handler(& _temp200);{ struct _xtunion_struct* _temp201=( struct
_xtunion_struct*) setjmp( _temp200.handler); if( ! _temp201){{ struct Cyc_Core_Opt*
_temp203=({ struct Cyc_Core_Opt* _temp202=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp202->v=( void*)(( struct Cyc_Absyn_XTuniondecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_XTuniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_xtuniondecl_f, _temp175);
_temp202;}); _npop_handler( 0u); return _temp203;}; _pop_handler();} else{
struct _xtunion_struct* _temp205= _temp201; _LL207: if( _temp205->tag == Cyc_Dict_Absent_tag){
goto _LL208;} else{ goto _LL209;} _LL209: goto _LL210; _LL208: return 0; _LL210:(
void) _throw( _temp205); _LL206:;}}} _LL184: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp211=( char*)"lookup_xtuniondecl: impossible";
struct _tagged_string _temp212; _temp212.curr= _temp211; _temp212.base= _temp211;
_temp212.last_plus_one= _temp211 + 31; _temp212;})); return 0; _LL186: { struct
Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp213; _push_handler(& _temp213);{
struct _xtunion_struct* _temp214=( struct _xtunion_struct*) setjmp( _temp213.handler);
if( ! _temp214){ ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp196, _temp194);;
_pop_handler();} else{ struct _xtunion_struct* _temp216= _temp214; _LL218: if(
_temp216->tag == Cyc_Dict_Absent_tag){ goto _LL219;} else{ goto _LL220;} _LL220:
goto _LL221; _LL219: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp222=( char*)"bad qualified name for xtunion"; struct _tagged_string
_temp223; _temp223.curr= _temp222; _temp223.base= _temp222; _temp223.last_plus_one=
_temp222 + 31; _temp223;}));( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Dict_Absent_struct* _temp224=( struct Cyc_Dict_Absent_struct*) GC_malloc(
sizeof( struct Cyc_Dict_Absent_struct)); _temp224[ 0]=({ struct Cyc_Dict_Absent_struct
_temp225; _temp225.tag= Cyc_Dict_Absent_tag; _temp225;}); _temp224;})); _LL221:(
void) _throw( _temp216); _LL217:;}}} return({ struct Cyc_Core_Opt* _temp226=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp226->v=(
void*)(( struct Cyc_Absyn_XTuniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->xtuniondecls, _temp175); _temp226;});}
_LL188: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp198); return({
struct Cyc_Core_Opt* _temp227=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp227->v=( void*)(( struct Cyc_Absyn_XTuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->xtuniondecls,
_temp175); _temp227;});} _LL180:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp230; struct _tagged_string* _temp231; void* _temp233; struct _tuple0*
_temp228= q; _temp230=* _temp228; _LL234: _temp233= _temp230.f1; goto _LL232;
_LL232: _temp231= _temp230.f2; goto _LL229; _LL229: { void* _temp235= _temp233;
struct Cyc_List_List* _temp245; struct Cyc_List_List* _temp247; struct Cyc_List_List
_temp249; struct Cyc_List_List* _temp250; struct _tagged_string* _temp252;
struct Cyc_List_List* _temp254; _LL237: if(( int) _temp235 == Cyc_Absyn_Loc_n){
goto _LL238;} else{ goto _LL239;} _LL239: if(( unsigned int) _temp235 > 1u?((
struct _tunion_struct*) _temp235)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL246:
_temp245=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp235)->f1;
if( _temp245 == 0){ goto _LL240;} else{ goto _LL241;}} else{ goto _LL241;}
_LL241: if(( unsigned int) _temp235 > 1u?(( struct _tunion_struct*) _temp235)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL248: _temp247=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp235)->f1; if( _temp247 == 0){ goto _LL243;} else{
_temp249=* _temp247; _LL253: _temp252=( struct _tagged_string*) _temp249.hd;
goto _LL251; _LL251: _temp250=( struct Cyc_List_List*) _temp249.tl; goto _LL242;}}
else{ goto _LL243;} _LL243: if(( unsigned int) _temp235 > 1u?(( struct
_tunion_struct*) _temp235)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL255: _temp254=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp235)->f1; goto
_LL244;} else{ goto _LL236;} _LL238: goto _LL240; _LL240: return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_enumdecl_f, _temp231);
_LL242: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace( te, loc,
_temp252, _temp250); return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, _temp231);}
_LL244: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp254); return((
struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->enumdecls, _temp231);} _LL236:;}} static struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl_f( struct Cyc_Tcenv_Genv* ge, struct _tagged_string*
v){ return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs, v);} struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple0* q){ struct _tuple0 _temp258; struct _tagged_string*
_temp259; void* _temp261; struct _tuple0* _temp256= q; _temp258=* _temp256;
_LL262: _temp261= _temp258.f1; goto _LL260; _LL260: _temp259= _temp258.f2; goto
_LL257; _LL257: { void* _temp263= _temp261; struct Cyc_List_List* _temp273;
struct Cyc_List_List* _temp275; struct Cyc_List_List _temp277; struct Cyc_List_List*
_temp278; struct _tagged_string* _temp280; struct Cyc_List_List* _temp282;
_LL265: if(( int) _temp263 == Cyc_Absyn_Loc_n){ goto _LL266;} else{ goto _LL267;}
_LL267: if(( unsigned int) _temp263 > 1u?(( struct _tunion_struct*) _temp263)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL274: _temp273=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp263)->f1; if( _temp273 == 0){ goto _LL268;} else{
goto _LL269;}} else{ goto _LL269;} _LL269: if(( unsigned int) _temp263 > 1u?((
struct _tunion_struct*) _temp263)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL276:
_temp275=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp263)->f1;
if( _temp275 == 0){ goto _LL271;} else{ _temp277=* _temp275; _LL281: _temp280=(
struct _tagged_string*) _temp277.hd; goto _LL279; _LL279: _temp278=( struct Cyc_List_List*)
_temp277.tl; goto _LL270;}} else{ goto _LL271;} _LL271: if(( unsigned int)
_temp263 > 1u?(( struct _tunion_struct*) _temp263)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL283: _temp282=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp263)->f1; goto _LL272;} else{ goto _LL264;} _LL266: goto _LL268; _LL268:
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_typedefdecl_f, _temp259); _LL270: { struct Cyc_Tcenv_Genv* ge=
Cyc_Tcenv_lookup_namespace( te, loc, _temp280, _temp278); return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs,
_temp259);} _LL272: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp282); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs, _temp259);} _LL264:;}}
struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({ struct Cyc_Tcenv_Genv*
_temp284=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof( struct Cyc_Tcenv_Genv));
_temp284->namespaces=( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp285= Cyc_Tcutil_empty_var_set;
if( _temp285 == 0){ _throw( Null_Exception);} _temp285->v;}); _temp284->structdecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp284->uniondecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp284->tuniondecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp284->xtuniondecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp284->enumdecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp284->typedefs=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp284->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp284->availables=
0; _temp284;});} void* Cyc_Tcenv_get_visible( void* f){ while( 1) { void*
_temp286= f; void* _temp294; void* _temp296; void* _temp298; void* _temp300;
void* _temp302; _LL288: if((( struct _tunion_struct*) _temp286)->tag == Cyc_Tcenv_Outermost_tag){
_LL295: _temp294=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp286)->f1;
goto _LL289;} else{ goto _LL290;} _LL290: if((( struct _tunion_struct*) _temp286)->tag
== Cyc_Tcenv_Frame_tag){ _LL299: _temp298=( void*)(( struct Cyc_Tcenv_Frame_struct*)
_temp286)->f1; goto _LL297; _LL297: _temp296=( void*)(( struct Cyc_Tcenv_Frame_struct*)
_temp286)->f2; goto _LL291;} else{ goto _LL292;} _LL292: if((( struct
_tunion_struct*) _temp286)->tag == Cyc_Tcenv_Hidden_tag){ _LL303: _temp302=(
void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp286)->f1; goto _LL301; _LL301:
_temp300=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp286)->f2; goto _LL293;}
else{ goto _LL287;} _LL289: return _temp294; _LL291: return _temp298; _LL293: f=
_temp300; goto _LL287; _LL287:;}} void* Cyc_Tcenv_put_visible( void* f, void* x){
void* _temp304= f; void* _temp312; void* _temp314; void* _temp316; void*
_temp318; void* _temp320; _LL306: if((( struct _tunion_struct*) _temp304)->tag
== Cyc_Tcenv_Outermost_tag){ _LL313: _temp312=( void*)(( struct Cyc_Tcenv_Outermost_struct*)
_temp304)->f1; goto _LL307;} else{ goto _LL308;} _LL308: if((( struct
_tunion_struct*) _temp304)->tag == Cyc_Tcenv_Frame_tag){ _LL317: _temp316=( void*)((
struct Cyc_Tcenv_Frame_struct*) _temp304)->f1; goto _LL315; _LL315: _temp314=(
void*)(( struct Cyc_Tcenv_Frame_struct*) _temp304)->f2; goto _LL309;} else{ goto
_LL310;} _LL310: if((( struct _tunion_struct*) _temp304)->tag == Cyc_Tcenv_Hidden_tag){
_LL321: _temp320=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp304)->f1; goto
_LL319; _LL319: _temp318=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp304)->f2;
goto _LL311;} else{ goto _LL305;} _LL307: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp322=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp322[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp323; _temp323.tag= Cyc_Tcenv_Outermost_tag;
_temp323.f1=( void*) x; _temp323;}); _temp322;}); _LL309: return( void*)({
struct Cyc_Tcenv_Frame_struct* _temp324=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp324[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp325; _temp325.tag= Cyc_Tcenv_Frame_tag; _temp325.f1=( void*) x; _temp325.f2=(
void*) _temp314; _temp325;}); _temp324;}); _LL311: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp326=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp326[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp327; _temp327.tag= Cyc_Tcenv_Hidden_tag;
_temp327.f1=( void*) _temp320; _temp327.f2=( void*)(( void*(*)( void* f, void* x))
Cyc_Tcenv_put_visible)( _temp318, x); _temp327;}); _temp326;}); _LL305:;} void*
Cyc_Tcenv_apply_to_visible( void*(* g)( void*), void* f){ return(( void*(*)(
void* f, void* x)) Cyc_Tcenv_put_visible)( f, g((( void*(*)( void* f)) Cyc_Tcenv_get_visible)(
f)));} void* Cyc_Tcenv_map_frames( void*(* g)( void*), void* f){ void* _temp328=
f; void* _temp336; void* _temp338; void* _temp340; void* _temp342; void*
_temp344; _LL330: if((( struct _tunion_struct*) _temp328)->tag == Cyc_Tcenv_Outermost_tag){
_LL337: _temp336=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp328)->f1;
goto _LL331;} else{ goto _LL332;} _LL332: if((( struct _tunion_struct*) _temp328)->tag
== Cyc_Tcenv_Frame_tag){ _LL341: _temp340=( void*)(( struct Cyc_Tcenv_Frame_struct*)
_temp328)->f1; goto _LL339; _LL339: _temp338=( void*)(( struct Cyc_Tcenv_Frame_struct*)
_temp328)->f2; goto _LL333;} else{ goto _LL334;} _LL334: if((( struct
_tunion_struct*) _temp328)->tag == Cyc_Tcenv_Hidden_tag){ _LL345: _temp344=(
void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp328)->f1; goto _LL343; _LL343:
_temp342=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp328)->f2; goto _LL335;}
else{ goto _LL329;} _LL331: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp346=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp346[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp347; _temp347.tag= Cyc_Tcenv_Outermost_tag;
_temp347.f1=( void*) g( _temp336); _temp347;}); _temp346;}); _LL333: return(
void*)({ struct Cyc_Tcenv_Frame_struct* _temp348=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp348[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp349; _temp349.tag= Cyc_Tcenv_Frame_tag; _temp349.f1=( void*) g( _temp340);
_temp349.f2=( void*)(( void*(*)( void*(* g)( void*), void* f)) Cyc_Tcenv_map_frames)(
g, _temp338); _temp349;}); _temp348;}); _LL335: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp350=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp350[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp351; _temp351.tag= Cyc_Tcenv_Hidden_tag;
_temp351.f1=( void*) g( _temp344); _temp351.f2=( void*)(( void*(*)( void*(* g)(
void*), void* f)) Cyc_Tcenv_map_frames)( g, _temp342); _temp351;}); _temp350;});
_LL329:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Tcenv_Env_error_struct*
_temp352=( struct Cyc_Tcenv_Env_error_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Env_error_struct));
_temp352[ 0]=({ struct Cyc_Tcenv_Env_error_struct _temp353; _temp353.tag= Cyc_Tcenv_Env_error_tag;
_temp353;}); _temp352;}));} return(( struct Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)((
void*)({ struct Cyc_Core_Opt* _temp354= te->le; if( _temp354 == 0){ _throw(
Null_Exception);} _temp354->v;}));} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv* fe){ if( te->le == 0){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Tcenv_Env_error_struct* _temp355=(
struct Cyc_Tcenv_Env_error_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Env_error_struct));
_temp355[ 0]=({ struct Cyc_Tcenv_Env_error_struct _temp356; _temp356.tag= Cyc_Tcenv_Env_error_tag;
_temp356;}); _temp355;}));} return({ struct Cyc_Tcenv_Tenv* _temp357=( struct
Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp357->ns= te->ns;
_temp357->ae= te->ae; _temp357->le=({ struct Cyc_Core_Opt* _temp358=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp358->v=( void*)(( void*(*)( void*
f, struct Cyc_Tcenv_Fenv* x)) Cyc_Tcenv_put_visible)(( void*)({ struct Cyc_Core_Opt*
_temp359= te->le; if( _temp359 == 0){ _throw( Null_Exception);} _temp359->v;}),
fe); _temp358;}); _temp357;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_old_ctrl(
struct Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv* _temp360=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp360->shared= f->shared;
_temp360->type_vars= f->type_vars; _temp360->locals= f->locals; _temp360->encloser=
f->encloser; _temp360->ctrl_env= f->ctrl_env; _temp360->capability=( void*)((
void*) f->capability); _temp360->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp360;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl( struct
Cyc_Tcenv_Fenv* f){ struct Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({ struct
Cyc_Tcenv_Fenv* _temp361=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp361->shared= f->shared; _temp361->type_vars= f->type_vars; _temp361->locals=
f->locals; _temp361->encloser= f->encloser; _temp361->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv*
_temp362=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv));
_temp362->continue_stmt=( void*)(( void*) c->continue_stmt); _temp362->break_stmt=(
void*)(( void*) c->break_stmt); _temp362->fallthru_clause= c->fallthru_clause;
_temp362->next_stmt=( void*)(( void*) c->next_stmt); _temp362->try_depth= c->try_depth;
_temp362;}); _temp361->capability=( void*)(( void*) f->capability); _temp361->curr_rgn=(
void*)(( void*) f->curr_rgn); _temp361;});} void* Cyc_Tcenv_return_typ( struct
Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string msg))
Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp363=( char*)"Tcenv: unexpected return_typ";
struct _tagged_string _temp364; _temp364.curr= _temp363; _temp364.base= _temp363;
_temp364.last_plus_one= _temp363 + 29; _temp364;}));} return( void*)(( Cyc_Tcenv_get_fenv(
te))->shared)->return_typ;} struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv(
te))->type_vars;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp365=( char*)"Tcenv: unexpected add_type_vars"; struct _tagged_string
_temp366; _temp366.curr= _temp365; _temp366.base= _temp365; _temp366.last_plus_one=
_temp365 + 32; _temp366;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct Cyc_List_List* new_tvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, fe->type_vars);
Cyc_Tcutil_check_unique_tvars( loc, new_tvs); fe->type_vars= new_tvs; return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp367=( char*)"Tcenv: unexpected add_local_var"; struct _tagged_string
_temp368; _temp368.curr= _temp367; _temp368.base= _temp367; _temp368.last_plus_one=
_temp367 + 32; _temp368;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_string* v=(* vd->name).f2;{ void*
_temp369=(* vd->name).f1; _LL371: if(( int) _temp369 == Cyc_Absyn_Loc_n){ goto
_LL372;} else{ goto _LL373;} _LL373: goto _LL374; _LL372: goto _LL370; _LL374:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp375=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp375[ 0]=({ struct Cyc_Core_Impossible_struct _temp376; _temp376.tag= Cyc_Core_Impossible_tag;
_temp376.f1=( struct _tagged_string)({ char* _temp377=( char*)"add_local_var: called with Rel_n";
struct _tagged_string _temp378; _temp378.curr= _temp377; _temp378.base= _temp377;
_temp378.last_plus_one= _temp377 + 33; _temp378;}); _temp376;}); _temp375;}));
goto _LL370; _LL370:;} fe->locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( fe->locals, v,(
void*)({ struct Cyc_Absyn_Local_b_struct* _temp379=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp379[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp380; _temp380.tag= Cyc_Absyn_Local_b_tag; _temp380.f1= vd; _temp380;});
_temp379;})); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl*
vd){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp381=( char*)"Tcenv: unexpected add_pat_var";
struct _tagged_string _temp382; _temp382.curr= _temp381; _temp382.base= _temp381;
_temp382.last_plus_one= _temp381 + 30; _temp382;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); struct _tagged_string* v=(*
vd->name).f2; fe->locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, void* data)) Cyc_Dict_insert)( fe->locals, v,( void*)({
struct Cyc_Absyn_Pat_b_struct* _temp383=( struct Cyc_Absyn_Pat_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct)); _temp383[ 0]=({ struct Cyc_Absyn_Pat_b_struct
_temp384; _temp384.tag= Cyc_Absyn_Pat_b_tag; _temp384.f1= vd; _temp384;});
_temp383;})); return Cyc_Tcenv_put_fenv( te, fe);}} void* Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple0* q){
struct _tuple0 _temp387; struct _tagged_string* _temp388; void* _temp390; struct
_tuple0* _temp385= q; _temp387=* _temp385; _LL391: _temp390= _temp387.f1; goto
_LL389; _LL389: _temp388= _temp387.f2; goto _LL386; _LL386: { void* _temp392=
_temp390; struct Cyc_List_List* _temp401; _LL394: if(( int) _temp392 == Cyc_Absyn_Loc_n){
goto _LL395;} else{ goto _LL396;} _LL396: if(( unsigned int) _temp392 > 1u?((
struct _tunion_struct*) _temp392)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL402:
_temp401=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp392)->f1;
if( _temp401 == 0){ goto _LL400;} else{ goto _LL398;}} else{ goto _LL398;}
_LL400: if( te->le != 0){ goto _LL397;} else{ goto _LL398;} _LL398: goto _LL399;
_LL395: if( te->le == 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Dict_Absent_struct*
_temp403=( struct Cyc_Dict_Absent_struct*) GC_malloc( sizeof( struct Cyc_Dict_Absent_struct));
_temp403[ 0]=({ struct Cyc_Dict_Absent_struct _temp404; _temp404.tag= Cyc_Dict_Absent_tag;
_temp404;}); _temp403;}));} else{ goto _LL397;} _LL397: { struct Cyc_Tcenv_Fenv*
fe= Cyc_Tcenv_get_fenv( te); struct _handler_cons _temp405; _push_handler(&
_temp405);{ struct _xtunion_struct* _temp406=( struct _xtunion_struct*) setjmp(
_temp405.handler); if( ! _temp406){{ void* _temp409=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp407=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp407[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp408; _temp408.tag= Cyc_Tcenv_VarRes_tag;
_temp408.f1=( void*)(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( fe->locals, _temp388); _temp408;}); _temp407;});
_npop_handler( 0u); return _temp409;}; _pop_handler();} else{ struct
_xtunion_struct* _temp411= _temp406; _LL413: if( _temp411->tag == Cyc_Dict_Absent_tag){
goto _LL414;} else{ goto _LL415;} _LL415: goto _LL416; _LL414: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL416:( void) _throw( _temp411); _LL412:;}}} _LL399: { struct
_handler_cons _temp417; _push_handler(& _temp417);{ struct _xtunion_struct*
_temp418=( struct _xtunion_struct*) setjmp( _temp417.handler); if( ! _temp418){{
void* _temp419= Cyc_Tcenv_lookup_ordinary_global( te, loc, q); _npop_handler( 0u);
return _temp419;}; _pop_handler();} else{ struct _xtunion_struct* _temp421=
_temp418; _LL423: if( _temp421->tag == Cyc_Dict_Absent_tag){ goto _LL424;} else{
goto _LL425;} _LL425: goto _LL426; _LL424: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp427= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifer: %.*s",
_temp427.last_plus_one - _temp427.curr, _temp427.curr);})); return( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp428=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp428[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp429; _temp429.tag= Cyc_Tcenv_VarRes_tag; _temp429.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b);
_temp429;}); _temp428;}); _LL426:( void) _throw( _temp421); _LL422:;}}} _LL393:;}}
void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp430=( char*)"Tcenv: unexpected process_continue"; struct _tagged_string
_temp431; _temp431.curr= _temp430; _temp431.base= _temp430; _temp431.last_plus_one=
_temp430 + 35; _temp431;}));}{ void* _temp432=( void*)(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt;
struct Cyc_Absyn_Stmt* _temp442; _LL434: if(( unsigned int) _temp432 > 3u?((
struct _tunion_struct*) _temp432)->tag == Cyc_Tcenv_Stmt_j_tag: 0){ _LL443:
_temp442=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Tcenv_Stmt_j_struct*) _temp432)->f1;
goto _LL435;} else{ goto _LL436;} _LL436: if(( int) _temp432 == Cyc_Tcenv_NotLoop_j){
goto _LL437;} else{ goto _LL438;} _LL438: if(( int) _temp432 == Cyc_Tcenv_CaseEnd_j){
goto _LL439;} else{ goto _LL440;} _LL440: if(( int) _temp432 == Cyc_Tcenv_FnEnd_j){
goto _LL441;} else{ goto _LL433;} _LL435: _temp442->non_local_preds=({ struct
Cyc_List_List* _temp444=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp444->hd=( void*) s; _temp444->tl= _temp442->non_local_preds; _temp444;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp442; return; _LL437: Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp445=( char*)"continue not in a loop"; struct
_tagged_string _temp446; _temp446.curr= _temp445; _temp446.base= _temp445;
_temp446.last_plus_one= _temp445 + 23; _temp446;})); return; _LL439:(( void(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp447=( char*)"Tcenv: continue dest is CaseEnd"; struct _tagged_string
_temp448; _temp448.curr= _temp447; _temp448.base= _temp447; _temp448.last_plus_one=
_temp447 + 32; _temp448;})); return; _LL441:(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp449=( char*)"Tcenv: continue dest is CaseEnd";
struct _tagged_string _temp450; _temp450.curr= _temp449; _temp450.base= _temp449;
_temp450.last_plus_one= _temp449 + 32; _temp450;})); return; _LL433:;}} void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp451=( char*)"Tcenv: unexpected process_break";
struct _tagged_string _temp452; _temp452.curr= _temp451; _temp452.base= _temp451;
_temp452.last_plus_one= _temp451 + 32; _temp452;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); void* _temp453=( void*)( fe->ctrl_env)->break_stmt;
struct Cyc_Absyn_Stmt* _temp463; _LL455: if(( unsigned int) _temp453 > 3u?((
struct _tunion_struct*) _temp453)->tag == Cyc_Tcenv_Stmt_j_tag: 0){ _LL464:
_temp463=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Tcenv_Stmt_j_struct*) _temp453)->f1;
goto _LL456;} else{ goto _LL457;} _LL457: if(( int) _temp453 == Cyc_Tcenv_NotLoop_j){
goto _LL458;} else{ goto _LL459;} _LL459: if(( int) _temp453 == Cyc_Tcenv_FnEnd_j){
goto _LL460;} else{ goto _LL461;} _LL461: if(( int) _temp453 == Cyc_Tcenv_CaseEnd_j){
goto _LL462;} else{ goto _LL454;} _LL456: _temp463->non_local_preds=({ struct
Cyc_List_List* _temp465=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp465->hd=( void*) s; _temp465->tl= _temp463->non_local_preds; _temp465;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp463; return; _LL458: Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp466=( char*)"break not in a loop or switch";
struct _tagged_string _temp467; _temp467.curr= _temp466; _temp467.base= _temp466;
_temp467.last_plus_one= _temp466 + 30; _temp467;})); return; _LL460: if(( void*)(
fe->shared)->return_typ !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp468=( char*)"break causes function not to return a value";
struct _tagged_string _temp469; _temp469.curr= _temp468; _temp469.base= _temp468;
_temp469.last_plus_one= _temp468 + 44; _temp469;}));} return; _LL462: Cyc_Tcutil_terr(
s->loc,( struct _tagged_string)({ char* _temp470=( char*)"break causes outer switch clause to implicitly fallthru";
struct _tagged_string _temp471; _temp471.curr= _temp470; _temp471.base= _temp470;
_temp471.last_plus_one= _temp470 + 56; _temp471;})); return; _LL454:;}} void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct _tagged_string* l,
struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp472=( char*)"Tcenv: unexpected process_goto"; struct _tagged_string
_temp473; _temp473.curr= _temp472; _temp473.base= _temp472; _temp473.last_plus_one=
_temp472 + 31; _temp473;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct Cyc_Core_Opt* sopt2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels, l);
if( sopt2 == 0){ struct Cyc_SlowDict_Dict* needed=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* slopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_lookup_opt)( needed, l); if( slopt
== 0){ slopt=({ struct Cyc_Core_Opt* _temp474=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp474->v=( void*) 0; _temp474;});}( fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_string*
key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)( needed, l,({ struct Cyc_List_List*
_temp475=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp475->hd=( void*) s; _temp475->tl=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp476= slopt; if( _temp476 == 0){ _throw( Null_Exception);} _temp476->v;});
_temp475;}));} else{(( struct Cyc_Absyn_Stmt*)({ struct Cyc_Core_Opt* _temp477=
sopt2; if( _temp477 == 0){ _throw( Null_Exception);} _temp477->v;}))->non_local_preds=({
struct Cyc_List_List* _temp478=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp478->hd=( void*) s; _temp478->tl=(( struct Cyc_Absyn_Stmt*)({
struct Cyc_Core_Opt* _temp479= sopt2; if( _temp479 == 0){ _throw( Null_Exception);}
_temp479->v;}))->non_local_preds; _temp478;});* sopt=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Core_Opt* _temp480= sopt2; if( _temp480 ==
0){ _throw( Null_Exception);} _temp480->v;}));}}} struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause***
clauseopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp481=( char*)"Tcenv: unexpected process_fallthru";
struct _tagged_string _temp482; _temp482.curr= _temp481; _temp482.base= _temp481;
_temp482.last_plus_one= _temp481 + 35; _temp482;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); struct _tuple3* ans=( fe->ctrl_env)->fallthru_clause;
if( ans != 0){((({ struct _tuple3* _temp483= ans; if( _temp483 == 0){ _throw(
Null_Exception);}* _temp483;}).f1)->body)->non_local_preds=({ struct Cyc_List_List*
_temp484=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp484->hd=( void*) s; _temp484->tl=((({ struct _tuple3* _temp485= ans; if(
_temp485 == 0){ _throw( Null_Exception);}* _temp485;}).f1)->body)->non_local_preds;
_temp484;});* clauseopt=({ struct Cyc_Absyn_Switch_clause** _temp486=( struct
Cyc_Absyn_Switch_clause**) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*) *
1); _temp486[ 0]=({ struct _tuple3* _temp487= ans; if( _temp487 == 0){ _throw(
Null_Exception);}* _temp487;}).f1; _temp486;});} return ans;}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* new_tvs,
struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause* clause){ if( te->le
== 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp488=( char*)"Tcenv: unexpected set_fallthru";
struct _tagged_string _temp489; _temp489.curr= _temp488; _temp489.base= _temp488;
_temp489.last_plus_one= _temp488 + 31; _temp489;}));}{ struct Cyc_List_List*
ft_typ= 0; for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp490= vds; if(
_temp490 == 0){ _throw( Null_Exception);} _temp490->tl;})){ ft_typ=({ struct Cyc_List_List*
_temp491=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp491->hd=( void*)(( void*)(( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List*
_temp492= vds; if( _temp492 == 0){ _throw( Null_Exception);} _temp492->hd;}))->type);
_temp491->tl= ft_typ; _temp491;});} ft_typ=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( ft_typ);{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=({ struct _tuple3*
_temp493=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp493->f1=
clause; _temp493->f2= new_tvs; _temp493->f3= ft_typ; _temp493;}); return Cyc_Tcenv_put_fenv(
te, fe);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv*
te){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp494=( char*)"Tcenv: unexpected clear_fallthru";
struct _tagged_string _temp495; _temp495.curr= _temp494; _temp495.base= _temp494;
_temp495.last_plus_one= _temp494 + 33; _temp495;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=
0; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp496=( char*)"Tcenv: unexpected set_in_loop"; struct
_tagged_string _temp497; _temp497.curr= _temp496; _temp497.base= _temp496;
_temp497.last_plus_one= _temp496 + 30; _temp497;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=(
void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp498=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp498[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp499; _temp499.tag= Cyc_Tcenv_Stmt_j_tag; _temp499.f1= continue_dest;
_temp499;}); _temp498;})));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)(
fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp500=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp500[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp501; _temp501.tag= Cyc_Tcenv_Stmt_j_tag; _temp501.f1= continue_dest;
_temp501;}); _temp500;}))); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp502=( char*)"Tcenv: unexpected enter_try"; struct _tagged_string _temp503;
_temp503.curr= _temp502; _temp503.base= _temp502; _temp503.last_plus_one=
_temp502 + 28; _temp503;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te)); ++( fe->ctrl_env)->try_depth; return Cyc_Tcenv_put_fenv(
te, fe);}} int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv* te){ if( te->le
== 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp504=( char*)"Tcenv: unexpected get_try_depth";
struct _tagged_string _temp505; _temp505.curr= _temp504; _temp505.base= _temp504;
_temp505.last_plus_one= _temp504 + 32; _temp505;}));} return(( Cyc_Tcenv_get_fenv(
te))->ctrl_env)->try_depth;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp506=( char*)"Tcenv: unexpected set_in_switch";
struct _tagged_string _temp507; _temp507.curr= _temp506; _temp507.base= _temp506;
_temp507.last_plus_one= _temp506 + 32; _temp507;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->break_stmt=(
void*)(( void*)( fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=(
void*)(( void*) Cyc_Tcenv_CaseEnd_j)); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv* te, void* j){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp508=( char*)"Tcenv: unexpected set_next";
struct _tagged_string _temp509; _temp509.curr= _temp508; _temp509.base= _temp508;
_temp509.last_plus_one= _temp508 + 27; _temp509;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->next_stmt=(
void*) j); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv* te, struct _tagged_string* v, struct Cyc_Absyn_Stmt* s){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp510=( char*)"Tcenv: unexpected add_label";
struct _tagged_string _temp511; _temp511.curr= _temp510; _temp511.base= _temp510;
_temp511.last_plus_one= _temp510 + 28; _temp511;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); struct Cyc_SlowDict_Dict* needed=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* sl_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_lookup_opt)( needed, v); if( sl_opt
!= 0){( fe->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_delete)( needed, v);{ struct Cyc_List_List*
sl=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp527= sl_opt; if(
_temp527 == 0){ _throw( Null_Exception);} _temp527->v;}); s->non_local_preds= sl;
for( 0; sl != 0; sl=({ struct Cyc_List_List* _temp512= sl; if( _temp512 == 0){
_throw( Null_Exception);} _temp512->tl;})){ void* _temp514=( void*)(( struct Cyc_Absyn_Stmt*)({
struct Cyc_List_List* _temp513= sl; if( _temp513 == 0){ _throw( Null_Exception);}
_temp513->hd;}))->r; struct Cyc_Absyn_Stmt* _temp520; struct Cyc_Absyn_Stmt**
_temp522; struct _tagged_string* _temp523; _LL516: if(( unsigned int) _temp514 >
1u?(( struct _tunion_struct*) _temp514)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL524:
_temp523=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp514)->f1;
goto _LL521; _LL521: _temp520=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp514)->f2; _temp522=&(( struct Cyc_Absyn_Goto_s_struct*) _temp514)->f2; goto
_LL517;} else{ goto _LL518;} _LL518: goto _LL519; _LL517:* _temp522=( struct Cyc_Absyn_Stmt*)
s; goto _LL515; _LL519:(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp525=( char*)"Tcenv: add_label backpatching of non-goto";
struct _tagged_string _temp526; _temp526.curr= _temp525; _temp526.base= _temp525;
_temp526.last_plus_one= _temp525 + 42; _temp526;})); goto _LL515; _LL515:;}}}
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)((
fe->shared)->seen_labels, v)){ Cyc_Tcutil_terr( s->loc,({ struct _tagged_string
_temp528=* v; xprintf("Repeated label: %.*s", _temp528.last_plus_one - _temp528.curr,
_temp528.curr);}));}( fe->shared)->seen_labels=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Stmt* data))
Cyc_Dict_insert)(( fe->shared)->seen_labels, v, s); return te;}} int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv* te){ return(( int(*)( struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)(((
Cyc_Tcenv_get_fenv( te))->shared)->needed_labels);} struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp529=( char*)"Tcenv: unexpected get_encloser";
struct _tagged_string _temp530; _temp530.curr= _temp529; _temp530.base= _temp529;
_temp530.last_plus_one= _temp529 + 31; _temp530;}));} return( Cyc_Tcenv_get_fenv(
te))->encloser;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp531=( char*)"Tcenv: unexpected set_encloser"; struct _tagged_string
_temp532; _temp532.curr= _temp531; _temp532.base= _temp531; _temp532.last_plus_one=
_temp531 + 31; _temp532;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp533=( char*)"Tcenv::add_region: unexpected add_region";
struct _tagged_string _temp534; _temp534.curr= _temp533; _temp534.base= _temp533;
_temp534.last_plus_one= _temp533 + 41; _temp534;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te));( void*)( fe->capability=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp535=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp535[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp536; _temp536.tag= Cyc_Absyn_JoinEff_tag; _temp536.f1=({ struct Cyc_List_List*
_temp537=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp537->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp539=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp539[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp540; _temp540.tag= Cyc_Absyn_AccessEff_tag;
_temp540.f1=( void*) r; _temp540;}); _temp539;})); _temp537->tl=({ struct Cyc_List_List*
_temp538=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp538->hd=( void*)(( void*) fe->capability); _temp538->tl= 0; _temp538;});
_temp537;}); _temp536;}); _temp535;}))); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp541=( char*)"Tcenv: unexpected new_block"; struct
_tagged_string _temp542; _temp542.curr= _temp541; _temp542.base= _temp541;
_temp542.last_plus_one= _temp541 + 28; _temp542;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({
struct Cyc_Absyn_VarType_struct* _temp550=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp550[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp551; _temp551.tag= Cyc_Absyn_VarType_tag; _temp551.f1= block_rgn; _temp551;});
_temp550;}); fe->type_vars=({ struct Cyc_List_List* _temp543=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp543->hd=( void*) block_rgn;
_temp543->tl= fe->type_vars; _temp543;}); Cyc_Tcutil_check_unique_tvars( loc, fe->type_vars);(
void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp544=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp544[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp545; _temp545.tag= Cyc_Absyn_JoinEff_tag;
_temp545.f1=({ struct Cyc_List_List* _temp546=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp546->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp548=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp548[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp549; _temp549.tag= Cyc_Absyn_AccessEff_tag;
_temp549.f1=( void*) block_typ; _temp549;}); _temp548;})); _temp546->tl=({
struct Cyc_List_List* _temp547=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp547->hd=( void*)(( void*) fe->capability); _temp547->tl=
0; _temp547;}); _temp546;}); _temp545;}); _temp544;})));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te){
return Cyc_Tcenv_new_named_block( loc, te, Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind));}
void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return(
void*) Cyc_Absyn_HeapRgn;} return( void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;}
void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* rgn){ if( te->le != 0){ void* capability=( void*)( Cyc_Tcenv_get_fenv(
te))->capability; if( Cyc_Tcutil_region_in_effect( 0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect(
1, rgn, capability)){ return;}} Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp552= Cyc_Absynpp_typ2string( rgn); xprintf("Expression accesses unavailable region %.*s",
_temp552.last_plus_one - _temp552.curr, _temp552.curr);}));} void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff){ void*
capability; if( te->le == 0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp553=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp553[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp554; _temp554.tag= Cyc_Absyn_JoinEff_tag;
_temp554.f1= 0; _temp554;}); _temp553;});} else{ struct Cyc_Tcenv_Fenv* fenv=
Cyc_Tcenv_get_fenv( te); capability=( void*) fenv->capability;} if( ! Cyc_Tcutil_subset_effect(
1, eff, capability)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp555=
Cyc_Absynpp_typ2string( capability); struct _tagged_string _temp556= Cyc_Absynpp_typ2string(
eff); xprintf("Capability %.*s does not cover function's effect %.*s", _temp555.last_plus_one
- _temp555.curr, _temp555.curr, _temp556.last_plus_one - _temp556.curr, _temp556.curr);}));}}
struct _tuple5{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict* locals=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); struct Cyc_Absyn_Tvar* rgn0=({ struct Cyc_Absyn_Tvar*
_temp587=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp587->name=({ struct _tagged_string* _temp588=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp588[ 0]=({ struct
_tagged_string _temp589=*(* fd->name).f2; xprintf("`%.*s", _temp589.last_plus_one
- _temp589.curr, _temp589.curr);}); _temp588;}); _temp587->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp587;}); struct
Cyc_List_List* tvs=({ struct Cyc_List_List* _temp586=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp586->hd=( void*) rgn0; _temp586->tl=
fd->tvs; _temp586;}); Cyc_Tcutil_check_unique_tvars( loc, tvs);{ void* param_rgn=(
void*)({ struct Cyc_Absyn_VarType_struct* _temp584=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp584[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp585; _temp585.tag= Cyc_Absyn_VarType_tag; _temp585.f1= rgn0; _temp585;});
_temp584;}); struct Cyc_List_List* vds= 0;{ struct Cyc_List_List* args= fd->args;
for( 0; args != 0; args=({ struct Cyc_List_List* _temp557= args; if( _temp557 ==
0){ _throw( Null_Exception);} _temp557->tl;})){ struct Cyc_Absyn_Vardecl* vd=({
struct Cyc_Absyn_Vardecl* _temp563=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp563[ 0]=({ struct Cyc_Absyn_Vardecl
_temp564; _temp564.sc=( void*)(( void*) Cyc_Absyn_Public); _temp564.name=({
struct _tuple0* _temp569=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp569[ 0]=({ struct _tuple0 _temp570; _temp570.f1=( void*) Cyc_Absyn_Loc_n;
_temp570.f2=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp571= args; if(
_temp571 == 0){ _throw( Null_Exception);} _temp571->hd;}))).f1; _temp570;});
_temp569;}); _temp564.tq=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp568=
args; if( _temp568 == 0){ _throw( Null_Exception);} _temp568->hd;}))).f2;
_temp564.type=( void*)(*(( struct _tuple5*)({ struct Cyc_List_List* _temp567=
args; if( _temp567 == 0){ _throw( Null_Exception);} _temp567->hd;}))).f3;
_temp564.initializer= 0; _temp564.rgn=({ struct Cyc_Core_Opt* _temp565=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1); _temp565[ 0]=({
struct Cyc_Core_Opt _temp566; _temp566.v=( void*) param_rgn; _temp566;});
_temp565;}); _temp564.attributes= 0; _temp564;}); _temp563;}); vds=({ struct Cyc_List_List*
_temp558=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp558[ 0]=({ struct Cyc_List_List _temp559; _temp559.hd=( void*) vd; _temp559.tl=
vds; _temp559;}); _temp558;}); locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( locals,(*(( struct
_tuple5*)({ struct Cyc_List_List* _temp560= args; if( _temp560 == 0){ _throw(
Null_Exception);} _temp560->hd;}))).f1,( void*)({ struct Cyc_Absyn_Param_b_struct*
_temp561=( struct Cyc_Absyn_Param_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct));
_temp561[ 0]=({ struct Cyc_Absyn_Param_b_struct _temp562; _temp562.tag= Cyc_Absyn_Param_b_tag;
_temp562.f1= vd; _temp562;}); _temp561;}));}} fd->param_vardecls=({ struct Cyc_Core_Opt*
_temp572=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp572[ 0]=({ struct Cyc_Core_Opt _temp573; _temp573.v=( void*) vds; _temp573;});
_temp572;}); return({ struct Cyc_Tcenv_Fenv* _temp574=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp574->shared=({ struct Cyc_Tcenv_SharedFenv*
_temp583=( struct Cyc_Tcenv_SharedFenv*) GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv));
_temp583->return_typ=( void*)(( void*) fd->ret_type); _temp583->seen_labels=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp583->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_SlowDict_empty)( Cyc_String_zstrptrcmp); _temp583;});
_temp574->type_vars= tvs; _temp574->locals= locals; _temp574->encloser= fd->body;
_temp574->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv* _temp582=( struct Cyc_Tcenv_CtrlEnv*)
GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp582->continue_stmt=( void*)((
void*) Cyc_Tcenv_NotLoop_j); _temp582->break_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j);
_temp582->fallthru_clause= 0; _temp582->next_stmt=( void*)(( void*) Cyc_Tcenv_FnEnd_j);
_temp582->try_depth= 0; _temp582;}); _temp574->capability=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp575=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp575[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp576; _temp576.tag= Cyc_Absyn_JoinEff_tag; _temp576.f1=({ struct Cyc_List_List*
_temp577=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp577->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp580=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp580[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp581; _temp581.tag= Cyc_Absyn_AccessEff_tag;
_temp581.f1=( void*) param_rgn; _temp581;}); _temp580;})); _temp577->tl=({
struct Cyc_List_List* _temp578=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp578->hd=( void*)(( void*)({ struct Cyc_Core_Opt*
_temp579= fd->effect; if( _temp579 == 0){ _throw( Null_Exception);} _temp579->v;}));
_temp578->tl= 0; _temp578;}); _temp577;}); _temp576;}); _temp575;})); _temp574->curr_rgn=(
void*) param_rgn; _temp574;});}}